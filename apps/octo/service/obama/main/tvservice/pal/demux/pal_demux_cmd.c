/**
	@file     cmd_pal_demux.c
	@brief    PAL/Demux command tool

	Description: 		\n
	Module: PAL/Demux			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/********************************************************************
 Header Files
 ********************************************************************/
#include <stdio.h>
#include <string.h>

#include <hlib.h>

#include <octo_common.h>

/********************************************************************
 Macro Definition
 ********************************************************************/


#if defined(CONFIG_DEBUG)
//원본에 header선언이 안되어 있음... header가  있다면 삭제 ...
void		pal_sef_PrintFilterTable(HINT32 nPrintLevel, HUINT16 usPid);
#endif


/********************************************************************
 Global/Static Variable Definition
 ********************************************************************/



/********************************************************************
 External Variable Declaration
 ********************************************************************/

/********************************************************************
 Static Function
 ********************************************************************/

/********************************************************************
  Global/Static Variable Definition
*********************************************************************/
#if defined(CONFIG_DEBUG)
STATIC int local_cmd_sitable(void *szArgList)
{
	HCHAR	*szArg = NULL;
	HINT32	nPrintLevel, nTemp, res;
	HUINT16	usPid;

	nPrintLevel = 0;
	usPid = 0xffff;
	if(szArgList != NULL)
	{
		szArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if (szArg != NULL)
		{
			res = sscanf(szArg, "%d", &nPrintLevel);
			if (res == 0)
				return HxCMD_ERR;

			szArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
			if (szArg != NULL)
			{
				res = sscanf(szArg, "%d", &nTemp);
				if (res == 0)
					return HxCMD_ERR;
				usPid = (HUINT16)nTemp;
			}
		}
	}

	pal_sef_PrintFilterTable(nPrintLevel, usPid);
	return HxCMD_OK;
}
#endif

/**********************************************
 Command 수행 함수를 등록하는 통합 등록 함수.
***********************************************/
#define	hCmdHandle		"pal_demux"
void CMD_Register_PAL_Demux(void)
{
#if defined(CONFIG_DEBUG)
	HLIB_CMD_RegisterWord(hCmdHandle,
			local_cmd_sitable,
			"sitable", 						/* keyword */
			"sitable",							/*	 help  */
			"sitable");				/*	usage  */
#endif
}


