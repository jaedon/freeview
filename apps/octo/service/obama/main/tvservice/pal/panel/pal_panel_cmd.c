/**
	@file     cmd_pal_panel.c
	@brief    PAL/Panel command tool

	Description: 		\n
	Module: PAL/KEY			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/********************************************************************
 Header Files
 ********************************************************************/
#include <stdio.h>
#include <string.h>

#include <octo_common.h>
#include <pal_panel.h>

/********************************************************************
 Macro Definition
 ********************************************************************/


/********************************************************************
 External Variable Declaration
 ********************************************************************/

/********************************************************************
 Static Function
 ********************************************************************/

/********************************************************************
 Global/Static Variable Definition
 ********************************************************************/

/************************
	Command 수행 함수
*************************/
#if defined(CONFIG_DEBUG)
static int pal_panel_ProcessCommand_onoff (void *szArgList)
{
	HCHAR				*szArg = NULL;
	int					ledNum, onoff;
	unsigned int		result = HxCMD_ERR;

	if(szArgList != NULL)
	{
		// get 1st argument
		szArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if (szArg != NULL)
		{
			sscanf(szArg, "%d", &ledNum);

			szArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
			if (szArg != NULL)
			{
				sscanf(szArg, "%d", &onoff);

				HLIB_CMD_Printf("Set LED[%d] to %d\n", ledNum, onoff);
				PAL_PANEL_SetLedOnOff(ledNum, onoff);
			}
		}

		result = HxCMD_OK;
	}

	return result;

}



static int pal_panel_ProcessCommand_setdim(void *szArgList)
{
	HCHAR				*szArg = NULL;
	int					nValue;
	unsigned int		result = HxCMD_ERR;

	if(szArgList != NULL)
	{
		// get 1st argument
		szArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if (szArg != NULL)
		{
			sscanf(szArg, "%d", &nValue);
			HLIB_CMD_Printf("[pal_panel_ProcessCommand_setdim] Level [%d]\n", nValue);
			PAL_PANEL_SetDim(nValue);
		}
		
		result = HxCMD_OK;
	}

	return result;

}
#endif

/**********************************************
	Command 수행 함수를 등록하는 통합 등록 함수.
***********************************************/
#define	hCmdHandle		"pal_panel"
void CMD_Register_PAL_PANEL(void)
{
#if defined(CONFIG_DEBUG)
	HLIB_CMD_RegisterWord(hCmdHandle, pal_panel_ProcessCommand_onoff,
			"onoff",	/* keyword */
			"On or off LED",	/*	 help  */
			"onoff [led#] [0/1]");	/*	usage  */

	HLIB_CMD_RegisterWord(hCmdHandle, pal_panel_ProcessCommand_setdim,
			"setdim",						/* keyword */
			"Set LCD dimmer",				/*	 help  */
			"setdim [0 to 100]");	/*	usage  */
#endif
}


