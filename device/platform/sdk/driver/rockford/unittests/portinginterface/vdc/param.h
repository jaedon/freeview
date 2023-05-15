/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: param.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 4/10/12 1:33p $
 *
 * File Description: This file provides a framework for very simple key/value
 * configuration parsing from stdin. It will take a list, separated by spaces
 * in the form: "key:value" and provide a way to map the value to a variable
 * in the application's scope.  "value" must be an unsigned.
 *
 * "help" and "end" are reserved keys.
 * "help:0" will print help information.
 * "end:0" will end the config parsing loop
 *
 * Usage:
 *            typedef struct BXXX_Settings
 *            {
 *               unsigned uiParameter;
 *               bool bEnableSetting;
 *            } BXXX_Settings;
 *
 *            BXXX_Settings stSettings;
 *            BXXX_Settings *pstSettings = &stSettings;
 *
 *            PARAM_PREFIX(64)
 *
 *            #define PARAM(_key,_field,_type,_help) \
 *               else if ( 0 == strcmp(PARAM_KEY,_key) )\
 *               {\
 *                  pstSettings->_field = (_type) PARAM_VALUE;\
 *               }
 *            #include "params.txt"
 *
 *            PARAM_HELP_PREFIX()
 *
 *            #define PARAM(_key,_field,_type,_help) \
 *                  BKNI_Printf("\""_key"\"" _help "\n");
 *            #include "params.txt"
 *
 *            PARAM_HELP_POSTFIX()
 *            PARAM_POSTFIX()
 *
 *   And "params.txt" contains:
 *            PARAM("parameter", uiParameter,    unsigned, "\n\tA Parameter to Configure (0=Setting 1, 1=Setting 2, 2=Setting 3)")
 *            PARAM("enable",    bEnableSetting, bool,     "\n\tA Boolean Enable (0=Disable, 1=Enable)")
 *            #undef PARAM
 *
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vdc/param.h $
 * 
 * Hydra_Software_Devel/1   4/10/12 1:33p nilesh
 * SW7425-2568: Added generic key/value parameter parsing
 *
 ***************************************************************************/

#ifndef PARAM_H_
#define PARAM_H_

#ifdef __cplusplus
extern "C" {
#endif

#define PARAM_PREFIX(_maxKeyLength) \
         { \
            char szTemp[_maxKeyLength];\
            char szKey[_maxKeyLength];\
            unsigned uiValue;\
            BKNI_Printf("Enter configuration key/value pairs (\"help:0\" for key list, \"end:0\" to end config):");\
            while ( 2 == scanf("%[^:]:%u", szTemp, &uiValue) )\
            {\
               /* Strip Whitespace */\
               sscanf( szTemp, "%s", szKey );\
               if ( 0 == strcmp(szKey,"end") )\
               {\
                  BKNI_Printf("<<< DONE >>>\n");\
                  break;\
               }
#define PARAM_KEY szKey
#define PARAM_VALUE uiValue
#define PARAM_POSTFIX() \
               else\
               {\
                  BKNI_Printf("<<< Unrecognized Key (%s) >>>\n", szKey);\
               }\
            }\
         }

#define PARAM_POSTFIX_NO_ERROR_CHECK() \
            }\
         }

#define PARAM_HELP_PREFIX() \
   else if ( 0 == strcmp(PARAM_KEY,"help"))\
   {\
      BKNI_Printf("Command List:\n");\

#define PARAM_HELP_POSTFIX() \
      BKNI_Printf("\"help:0\"\n\tPrint this help\n");\
      BKNI_Printf("\"end:0\"\n\tEnds configuration parsing loop\n");\
   }\

#ifdef __cplusplus
}
#endif

#endif /* PARAM_H_ */
/* End of File */
