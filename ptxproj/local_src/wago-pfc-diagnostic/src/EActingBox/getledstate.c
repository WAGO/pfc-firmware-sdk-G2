//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material.
/// All manufacturing, reproduction, use and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     getledstate.c
///
///  \version  $Revision: 1 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <glib.h>
#include <dbus/dbus.h>
#include <time.h>
#include "parse_log.h"

#include "eactingbox.h"
#include "parse_log.h"
#include "diagnostic_xml.h"
#include "led_info_json.h"


//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------
typedef struct  {
    GList * ledOrderList;
    GList * error;
}tOrderHelp;

typedef enum
{
  TIMESTRING_FORMAT_WD_M_MD_H_M_S_Y,
  TIMESTRING_FORMAT_WD_M_MD_Y_H_M_S
} tTimestringFormat;

typedef struct {
    tDiagXml ** doc;
    tTimestringFormat tsformat;
}tLedPrintParameters;

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------

/*
 *-- Function: ShowHelp ---------------------------------------------------
 *
 *  Zeigt den Hilfe-Text für das Programm an
 *
 *------------------------------------------------------------------------------
 */
static void ShowHelp(void)
{
  printf("\n* Get State of LED *\n\n");
  printf("Usage: getledstate [<LED-Name>] [options]\n\n");
  printf("options:\n");
  printf("  -a [--all]            : prints all LED state values (only if LED-Name is not given)\n");
  printf("  -T [--time-reformat]  : prints LED state with alternative format of timestring\n");
  printf("  -h [--help]           : prints this help\n");
}

static const char * _ColorToString(tLedStaticColor color)
{
  const char * ret = NULL;

  switch(color)
  {
    case LED_COLOR_RED:
      ret = "RED";
      break;
    case LED_COLOR_GREEN:
      ret = "GRN";
      break;
    case LED_COLOR_YELLOW:
      ret = "YLW";
      break;
    case LED_COLOR_BLUE:
      ret = "BLU";
      break;
    case LED_COLOR_CYAN:
      ret = "CYN";
      break;
    case LED_COLOR_MAGENTA:
      ret = "MAG";
      break;
    case LED_COLOR_WHITE:
      ret = "WHT";
      break;
    case LED_COLOR_OFF:
      ret = "OFF";
      break;
  }
  return ret;
}
static int _StateToDepricated(tLedStatus * ledStatus,char * statestring,uint32_t * var1,uint32_t * var2)
{
  int ret =1;
   switch(ledStatus->state)
   {
     case LED_STATE_STATIC:
       sprintf(statestring,"%s",
               _ColorToString(ledStatus->vars.staticVar));
       *var1=0;
       *var2=0;
       break;
     case LED_STATE_BLINK:
       if(ledStatus->vars.blinkVar.color2 == LED_COLOR_OFF)
       {
         sprintf(statestring,"BLINK_%s",
                 _ColorToString(ledStatus->vars.blinkVar.color1));
       }
       else
       {
         sprintf(statestring,"BLINK_%s_%s",
                 _ColorToString(ledStatus->vars.blinkVar.color1),
                 _ColorToString(ledStatus->vars.blinkVar.color2));
       }
       *var1=ledStatus->vars.blinkVar.time1;
       *var2=ledStatus->vars.blinkVar.time2;
      break;
     case LED_STATE_FLASH:
       sprintf(statestring,"FLASH_%s_%s",
               _ColorToString(ledStatus->vars.flashVar.flashColor),
               _ColorToString(ledStatus->vars.flashVar.staticColor));
       *var1=ledStatus->vars.flashVar.flashTime;
       *var2=0;
       break;
     case LED_STATE_750_ERR:
       sprintf(statestring,"ERR_SEQ_750");
       *var1=ledStatus->vars.err750Var.errorCode;
       *var2=ledStatus->vars.err750Var.errorArg;
       break;
     case LED_STATE_CAN:
       sprintf(statestring,"SPECIAL_BLINK");
       *var1=1;
       *var2=ledStatus->vars.canVar.canCode;
       break;
     default:
       ret =0;
       break;
   }

   return ret;
}
static void _PrintLedState(tDiagXml ** doc, tTimestringFormat tsformat, tLedStatus * ledStatus)
{
  char statestring[255];
  uint32_t var1=0;
  uint32_t var2=0;

  if(_StateToDepricated(ledStatus,statestring,&var1,&var2))
  {
    char  timeString[32];
    char * valString;
    tLogData logData;
    int docnum;
    struct tm tm = *localtime(&ledStatus->timestamp.tv_sec);
    switch(tsformat)
    {
      default:
        //no break;
      case TIMESTRING_FORMAT_WD_M_MD_H_M_S_Y:
        //e.g. "Fri Feb 10 08:45:00 2023"
        strftime(timeString, sizeof(timeString), "%a %b %d %H:%M:%S %Y", &tm);
        break;
      case TIMESTRING_FORMAT_WD_M_MD_Y_H_M_S:
        //e.g. "Fri Feb 10 2023 08:45:00"
        strftime(timeString, sizeof(timeString), "%a %b %d %Y %H:%M:%S", &tm);
        break;
    }

    for(docnum=0;docnum<2;docnum++)
    {
      valString = (char*)GetStringOfId(ledStatus->info.id, doc[docnum], NULL);
      if(valString != NULL)
      {
        break;
      }
    }
    logData.id = ledStatus->info.id;
    logData.progName = NULL;
    logData.set = TRUE;
    logData.timestamp.tv_sec = ledStatus->timestamp.tv_sec;
    logData.timestamp.tv_usec = ledStatus->timestamp.tv_usec;
    logData.variables = ledStatus->info.info;

    if(valString == NULL)
    {
      printf("%s: %s %u %u %s: \"Undefined ID: %.8X\"\n",
             ledStatus->name,
             statestring,
             var1,
             var2,
             timeString,
             ledStatus->info.id);
    }
    else
    {
      char   completeString[4096];
      ReplaceVariables(completeString,valString, &logData, sizeof(completeString));
      printf("%s: %s %u %u %s: \"%s\"\n",
             ledStatus->name,
             statestring,
             var1,
             var2,
             timeString,
             completeString);
    }
  }
  else
  {
    printf("unknown LED-State: %s 0x%X %u %d 0x%.8X\n",
           ledStatus->name,
           ledStatus->state,
           var1,
           var2,
           ledStatus->info.id);
  }
}

static GList * _CreateLedStateList(void)
{
  GList * list = NULL;
  DBusMessage * message;
  DBusMessage * reply;
  DBusMessageIter iter;

  int type;
  com_tConnection   dBusCon = { .bus = NULL };
  com_GEN_Init(&dBusCon);
  message = dbus_message_new_method_call(LED_DBUS_NAME, LED_DBUS_PATH_ALL,
                                         NULL, LED_DBUS_GET_STATE_ALL);
  reply = dbus_connection_send_with_reply_and_block   (dBusCon.bus,message,-1,NULL);
  dbus_message_iter_init(reply, &iter);
  type = dbus_message_iter_get_arg_type (&iter);
  while(DBUS_TYPE_INVALID != type)
  {
    tLedStatus * ledState = malloc(sizeof(tLedStatus));
    char * ledName = NULL;

    memset(ledState,0,sizeof(tLedStatus));

    dbus_message_iter_get_basic (&iter, &ledName);
    strcpy(ledState->name,ledName);
    if(FALSE == dbus_message_iter_has_next(&iter))break;
    dbus_message_iter_next(&iter);

    dbus_message_iter_get_basic (&iter, &ledState->state);
    if(FALSE == dbus_message_iter_has_next(&iter))break;
    dbus_message_iter_next(&iter);

    dbus_message_iter_get_basic (&iter, &ledState->timestamp.tv_sec);
    if(FALSE == dbus_message_iter_has_next(&iter))break;
    dbus_message_iter_next(&iter);

    dbus_message_iter_get_basic (&iter, &ledState->timestamp.tv_usec);
    if(FALSE == dbus_message_iter_has_next(&iter))break;
    dbus_message_iter_next(&iter);

    led_GetDbusInfoByState(&iter,ledState);

    list = g_list_append(list, ledState);
    if(FALSE == dbus_message_iter_has_next(&iter))break;
    dbus_message_iter_next(&iter);
    type = dbus_message_iter_get_arg_type (&iter);
  }

  return list;
}

static void _FreeLedStatus(gpointer gpLedStatus)
{
  tLedStatus * ledState = (tLedStatus*)gpLedStatus;

  if(ledState != NULL)
  {
    free(ledState);
  }
}

static void _PrintLedStateWrapper(tLedCtlState * status, tLedPrintParameters * params)
{
  _PrintLedState(params->doc, params->tsformat, (tLedStatus*)status);
}

static void _PrintLedError(char * error,int* ignore)
{
  (void)ignore;
  syslog(LOG_USER|LOG_ERR,"%s",error);
}

static gint _LedFinder(gconstpointer pLedState, gconstpointer pLedName)
{
  tLEDInfo * ledState = (tLEDInfo*)pLedState;
  char * ledName = (char*)pLedName;
  return strcmp(ledState->name,ledName);
}

static void _GenerateLedError(tOrderHelp * orderHelp,char * name)
{
  char errormsg[255];
  sprintf(errormsg,"ERROR: Unknown LED Detected: %s",name);

  if( NULL == g_list_find_custom(orderHelp->error,errormsg,(GCompareFunc)strcmp))
  {
    orderHelp->error = g_list_append(orderHelp->error,strdup(errormsg));
  }
}

static void _FreeLedError(gpointer gpErrorText)
{
  char * errorText = (char*)gpErrorText;

  if(errorText != NULL)
  {
    free(errorText);
  }
}

/*
 * Compare the Order of an LED using the Incoming List.
 * if an LED is unknown (g_list_index = -1) it has to be put at the end and an error has to be reported
 */
static gint _CompareLedOrder(gconstpointer pLed1, gconstpointer pLed2, gpointer pOrderHelp)
{
  gint retval = 0;
  gint posLed1 = -1;
  gint posLed2 = -1;
  tLedStatus * led1 = (tLedStatus*)pLed1;
  tLedStatus * led2 = (tLedStatus*)pLed2;

  tOrderHelp * orderHelp =(tOrderHelp*) pOrderHelp;
  GList * llink;
  llink = g_list_find_custom(orderHelp->ledOrderList,led1->name,_LedFinder);
  posLed1 = g_list_position(orderHelp->ledOrderList,llink);
  llink = g_list_find_custom(orderHelp->ledOrderList,led2->name,_LedFinder);
  posLed2 = g_list_position(orderHelp->ledOrderList,llink);

  if(posLed1 < 0)
  {
    retval=1;
    _GenerateLedError(orderHelp,led1->name);
  }
  if(posLed2 < 0)
  {
    retval=-1;
    _GenerateLedError(orderHelp,led2->name);
  }

  if(retval == 0)
  {
    if(posLed1 < posLed2)
    {
      retval = -1;
    }
    else if( posLed1 > posLed2 )
    {
      retval = 1;
    }
  }

  return retval;

}

static int _ShowLedInfoList(tDiagXml ** doc, tTimestringFormat tsformat)
{
  int exitval = 0;

  GList * states = NULL;
  tOrderHelp orderHelp;
  orderHelp.ledOrderList = getInfoLedListFromFile(LED_INFO_FILE);
  orderHelp.error=NULL;

  states=_CreateLedStateList();

  //print list filtered
  states = g_list_sort_with_data(states,_CompareLedOrder,(gpointer)&orderHelp);
  if(orderHelp.error == NULL)
  {
    tLedPrintParameters printParameters;
    printParameters.doc = doc;
    printParameters.tsformat = tsformat;
    g_list_foreach(states,(GFunc)_PrintLedStateWrapper,&printParameters);
  }
  else
  {
    g_list_foreach(orderHelp.error,(GFunc)_PrintLedError,NULL);
    g_list_free_full(orderHelp.error,_FreeLedError);
    exitval = -1;
  }
  cleanUpInfoLedList(orderHelp.ledOrderList);
  g_list_free_full(states,_FreeLedStatus);
  return exitval;
}

static int _ShowLedInfo(tDiagXml ** doc, tTimestringFormat tsformat, char *argName)
{
  int exitval = 0;
  tLedStatus ledState;
  char name[255];
  size_t size_led_name_buffer = sizeof(name);
  size_t len_led_name = strlen(argName);
  size_t bytes_to_copy = MIN(len_led_name, size_led_name_buffer - 1);
  memset(name, 0x00, size_led_name_buffer);
  memcpy(name, argName, bytes_to_copy);
  if( 0 != led_GetLedStatus(name,&ledState))
  {
    int errr = errno;
    syslog(LOG_USER|LOG_ERR,"Error: check correct spelling of name.");
    printf("Error: %d: %s\n",errr, strerror(errr));
    syslog(LOG_USER|LOG_ERR,"Error: %d: %s\n",errr, strerror(errr));
    exitval = -1;
  }
  else
  {
    _PrintLedState(doc, tsformat, &ledState);
  }
  return exitval;
}

/*
 *-- Function: getledstate_main ---------------------------------------------------
 * 
 *  Main-Funktion für getledstate
 * 
 *  \param argc
 *  \param argv
 * 
 *  \return <description of the return values of the function>
 *------------------------------------------------------------------------------
 */
int getledstate_main(int argc, char **argv)
{
  tDiagXml * doc[2];
  int exitval = 0;

  //expect either one or two additional arguments, see help
  if ((argc == 2) || (argc == 3))
  {
    if ((strcmp(argv[1], "--help") == 0) || strcmp(argv[1], "-h") == 0)
    {
      ShowHelp();
      return -1;
    }
  }
  else
  {
    printf("WRONG NUMBER OF ELEMENTS\n\n");
    ShowHelp();
    return -1;
  }
  log_EVENT_Init("getledstate");
  closelog();
  openlog("getledstate",LOG_PERROR,LOG_USER);

  doc[0] = ParseDoc(DIAGNOSTIC_XML_DOCUMENT_CUSTOM);
  doc[1] = ParseDoc(DIAGNOSTIC_XML_DOCUMENT);
  if(argc==2)
  {
    if(strcmp(argv[1], "--all") == 0 || strcmp(argv[1], "-a") == 0)
    {
      exitval = _ShowLedInfoList(doc, TIMESTRING_FORMAT_WD_M_MD_H_M_S_Y);
    }
    else
    {
      exitval = _ShowLedInfo(doc, TIMESTRING_FORMAT_WD_M_MD_H_M_S_Y, argv[1]);
    }
  }
  else //argc==3
  {
    if(strcmp(argv[1], "--all") == 0 || strcmp(argv[1], "-a") == 0)
    {
      if(strcmp(argv[2], "--time-reformat") == 0 || strcmp(argv[2], "-T") == 0)
      {
        exitval = _ShowLedInfoList(doc, TIMESTRING_FORMAT_WD_M_MD_Y_H_M_S);
      }
      else
      {
        exitval = _ShowLedInfoList(doc, TIMESTRING_FORMAT_WD_M_MD_H_M_S_Y);
      }
    }
    else
    {
      if(strcmp(argv[2], "--time-reformat") == 0 || strcmp(argv[2], "-T") == 0)
      {
        exitval = _ShowLedInfo(doc, TIMESTRING_FORMAT_WD_M_MD_Y_H_M_S, argv[1]);
      }
      else
      {
        exitval = _ShowLedInfo(doc, TIMESTRING_FORMAT_WD_M_MD_H_M_S_Y, argv[1]);
      }
    }
  }
  FreeDiagXml(doc[0]);
  FreeDiagXml(doc[1]);
  return exitval;
}
//---- End of source file ------------------------------------------------------
