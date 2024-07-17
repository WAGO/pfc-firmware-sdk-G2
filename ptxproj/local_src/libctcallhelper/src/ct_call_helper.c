#include "ct_call_helper.h"

#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/wait.h>                 // waitpid
#include <unistd.h>                   // vfork
#include <errno.h>
#include <stdint.h>
#include <unistd.h>



// maximum size of config tool call string in fb
#define FB_SIZE_OF_CALL_STRING        251

// defines for the comunication with the bash to call the config tools and get their result
#define MAX_LENGTH_TEMPFILE_NAME      40
#define TEMPFILE_NAME                 "/tmp/configtools"
#define MAX_TEMPFILES                 99
#define MAX_LENGTH_SYSTEM_CALL        (FB_SIZE_OF_CALL_STRING + 100)


#define SYSTEM_RESULT_GENERAL_ERROR   -1
#define SYSTEM_RESULT_UNKNOWN_FILE    32512


#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

//------------------------------------------------------------------------------
// External variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local typedefs
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local functions
//------------------------------------------------------------------------------

static int mysystem(const char* inputstring)
//
// Emulate system function by using vfork instead of fork because memory is not sufficient to dublicate 
// the whole plclinux_rt thread.
//
// input: inputstring: command which should be executed by shell.
// return: error code: 0 (successfull execution), -1 (error)
//
{
  pid_t pid;
  int   status  = 0;

  if(inputstring == NULL)
  {
    return -1;
  }

  // create process
  if((pid = vfork()) < 0 )
  {
    status = -1;
  }

  // child process - call shell command and terminate
  else if(pid == 0)
  {
    execl("/bin/sh", "sh","-c", inputstring, NULL);
    _exit(127);
  }

  // parent process - wait until child is ready
  else while(waitpid(pid, &status, 0) < 0)
  {
    if(errno != EINTR)
    {
      status = -1;
      break;
    }
  }

  return status;
}

static int FileContent_Get(char* pFilename, 
                           char* pOutputBuffer,
                           ssize_t outputBufferLength)
//
// Open file and read its content to an allocated buffer
// !!! It is absolutly necessairy to call the function FileContent_Destruct after handling
// the data to free the allocated memory !!!
//
// input: pFilename: name of file that should be read
// output: ppOutputBuffer: pointer to buffer with the contents of file
//                         NULL if an error occured of file is not existing
// return: error code: CCTR_Success, CCTR_InvalidParameter
//
{
  int     status                = CCTR_ExecutionError;
  FILE*   fFile                 = NULL;

  // check input parameter
  if((pFilename == NULL) || (strlen(pFilename) == 0) || (pOutputBuffer == NULL))
  {
    return CCTR_InvalidParameter;
  }

  // open file
  if(NULL != (fFile = fopen(pFilename, "r")))
  {
    // get size of file to allocate buffer
    struct  stat fileAttributes;
    if(stat(pFilename, &fileAttributes) != -1)
    {
      const size_t size = min(fileAttributes.st_size + 1, outputBufferLength);
      
      // initialize memory with 0 (end of string) and read read whole file-content into buffer 
      memset(pOutputBuffer, 0, size);
      fread(pOutputBuffer, size, 1, fFile);

      status =  (outputBufferLength < fileAttributes.st_size)
                ? CCTR_ResultStringOverflow
                : CCTR_Success;
    }
    fclose(fFile);
  }

  return status;
}


static int  SystemCall_GetOutput(char*  pSystemCallString,
                                 char*  pSystemCallOutput,
                                 size_t systemCallOuputLength,
                                 int*   pSystemCallResult)
//
// Make a system-call and return its in pSystemCallOutput.
//
// input:  string with the system-call
// return: error code (CCTR_Success, CCTR_InvalidParameter, CCTR_ExecutionError, UNKNOWN_CONFIG_TOOL)
//
{
  int   status        = CCTR_Success;
  FILE* fOutputFile   = NULL;
  int   fileIndex     = 0;

  char  completeSystemCallString[MAX_LENGTH_SYSTEM_CALL]  = "";
  char  filename[MAX_LENGTH_TEMPFILE_NAME]                = "";

  if((pSystemCallString == NULL) || (pSystemCallOutput == NULL) || (pSystemCallResult == NULL))
  {
    return CCTR_InvalidParameter;
  }

  // find a not yet existing name for temporary file - if it is not existing, we can't open it.
  // this is some what racy .. as the file my be created between our check and the actual config tool
  // call..
  fileIndex = 0;
  while(fileIndex < MAX_TEMPFILES)
  {
    snprintf(filename, sizeof(filename), "%s_%x_%02d", TEMPFILE_NAME, (uint32_t) pthread_self(), fileIndex);

    fOutputFile = fopen(filename, "r");
    if(fOutputFile == NULL)
    {
      break;
    }
    else
    {
      fclose(fOutputFile);
      ++fileIndex;
    }
  }

  // no not existing name for tempfile was found -> error
  if(fileIndex >= MAX_TEMPFILES)
  {
    status = CCTR_ExecutionError;
  }

  // tempfile name was found -> do sytemcall, redirect its outputs to tempfile
  else
  {
    int   systemResult  = 0;

    // add redirection from sytemcall-outputs to the commandline
    int rc = snprintf(completeSystemCallString, MAX_LENGTH_SYSTEM_CALL, "%s > %s 2> /dev/null", pSystemCallString, filename);
    if (0 > rc)
    {
       completeSystemCallString[0] = '\0';
    }
    systemResult = mysystem(completeSystemCallString);
    //printf("completeSystemCallString:%s systemResult:%d\n", completeSystemCallString, systemResult);

    // if an error occured by system function -> set execution error status
    if(systemResult == SYSTEM_RESULT_GENERAL_ERROR) 
    {
      status = CCTR_ExecutionError;
    }

    else if(systemResult == SYSTEM_RESULT_UNKNOWN_FILE)
    {
      status = CCTR_UnknownConfigTool;
    }

    else 
    {
      // if the config tool itself returns a value (= error) -> set config tool result accordingly
      if(systemResult != 0) *pSystemCallResult = systemResult >> 8;
      //printf("*pSystemCallResult:%d\n", *pSystemCallResult);

      // read output of config tool from tempfile to an allocated buffer and afterwards delete the file
      status = FileContent_Get(filename, pSystemCallOutput, systemCallOuputLength);
      remove(filename);
    }
  }

  return status;
}


int CallConfigTool(char*    pConfigToolCallString,
                   char*    pConfigToolOutput,
                   size_t   sizeofConfigToolOutput,
                   int32_t* pConfigToolResult)
//
// Execute a system call in config tool directory and confer its outputs to a given string.
//
// input: pConfigToolCallString: string with the config tool system call (without directory)
//        sizeofConfigToolOutput: maximum size of the string for the system call outputs
//
// output: pConfigToolOutput: outpt string of config tool - must be allocated in calling function
//         pConfigToolResult: result value of config tool executable
//
// return: error code (CCTR_Success, CCTR_InvalidParameter, CCTR_ExecutionError, CCTR_ResultStringOverflow)
//
{
  int status  = CCTR_Success;

  char  configToolSystemCall[MAX_LENGTH_SYSTEM_CALL];

  // check input parameter
  if((pConfigToolCallString == NULL) || (pConfigToolOutput == NULL) || (sizeofConfigToolOutput == 0) || (pConfigToolResult == NULL))
  {
    return CCTR_InvalidParameter;
  }

  // create system call string for config tool (precede config tool directory)
  int rc = snprintf(configToolSystemCall, MAX_LENGTH_SYSTEM_CALL, "/etc/config-tools/%s", pConfigToolCallString);
  if (0 > rc)
  {
    configToolSystemCall[0] = '\0';
  }
  //printf("configToolSystemCall:%s\n", configToolSystemCall);

  // call config tool
  status = SystemCall_GetOutput(configToolSystemCall, pConfigToolOutput, sizeofConfigToolOutput, pConfigToolResult);

  return status;
}


int CallSystemShell(const char*  pSystemCallString,
                    int32_t* pSystemCallResult)
//
// Execute any system call.
//
// input: pSystemCallString: string with the system call
//
// output: pSystemCallResult: result value of the called executable
//
// return: error code (CCTR_Success, CCTR_InvalidParameter, CCTR_ExecutionError, CCTR_UnknownConfigTool)
//
{
  int status;
  // check input parameter
  if((pSystemCallString == NULL) || (pSystemCallResult == NULL))
  {
    status  = CCTR_InvalidParameter;
  }
  else
  {
    int   systemResult;
    char  completeSystemCallString[MAX_LENGTH_SYSTEM_CALL]  = "";
    // add redirection from sytemcall-outputs
    snprintf(completeSystemCallString, MAX_LENGTH_SYSTEM_CALL, "%s >/dev/null 2>&1", pSystemCallString);

    systemResult = mysystem(completeSystemCallString);

    // if an error occured by system function -> set execution error status
    if(systemResult == SYSTEM_RESULT_GENERAL_ERROR) 
    {
      status = CCTR_ExecutionError;
    }
    else if(systemResult == SYSTEM_RESULT_UNKNOWN_FILE)
    {
      status = CCTR_UnknownConfigTool;
    }
    else 
    {
      status  = CCTR_Success;
      // if the config tool itself returns a value (= error) -> set config tool result accordingly
      *pSystemCallResult = systemResult >> 8;
    }
  }
  return status;
}
