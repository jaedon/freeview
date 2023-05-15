
/**
	@file     cmd_pal_dsc.c
	@brief    PAL/Descrambler command tool

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

#include <octo_common.h>

#include <pal_dsc.h>

#if (defined(CONFIG_MW_CAS) || defined(CONFIG_MW_CI_PLUS))
/********************************************************************
 Macro Definition
 ********************************************************************/


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
STATIC int pal_dsc_ProcessCommand_info(void *szArgList)
{
	HUINT8	 		*aucArg=NULL;
	HUINT32 		p2;

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
	if( aucArg != NULL )
	{
		if( sscanf(aucArg, "%d", &p2) == 0 )
			return HxCMD_ERR;

		PAL_DSC_PrintDscStatus(p2);
	}
	else
	{
		PAL_DSC_PrintAllDscStatus();
	}

	return HxCMD_OK;
}
#endif

/**********************************************
 Command 수행 함수를 등록하는 통합 등록 함수.
***********************************************/
void CMD_Register_PAL_Dsc(void)
{
#if defined(CONFIG_DEBUG)
	HLIB_CMD_RegisterWord("pal_dsc", pal_dsc_ProcessCommand_info,
			"info",							/* keyword */
			"Print descrambler info",							/*   help  */
			"info [id]");				/*  usage  */
#endif
}

#endif // #if (defined(CONFIG_MW_CAS) || defined(CONFIG_MW_CI_PLUS))
