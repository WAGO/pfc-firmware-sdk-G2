#ifndef _CT_CALL_HELPER_LIB_H
#define _CT_CALL_HELPER_LIB_H

#include <inttypes.h>
#include <stddef.h>

enum CallConfigToolResult {
  CCTR_Success              = 0,
  CCTR_InvalidParameter     = -1,
  CCTR_ExecutionError       = -2,
  CCTR_UnknownConfigTool    = -3,
  CCTR_ResultStringOverflow = -4,
};

/**
 *   ... tbd
 *
 */
int CallConfigTool(char*    pConfigToolCallString,
                   char*    pConfigToolOutput,
                   size_t  sizeofConfigToolOutput,
                   int32_t* pConfigToolResult);


int CallSystemShell(const char*  pSystemCallString,
                    int32_t* pSystemCallResult);

#endif
