/**
	@file     xmgr_cas.c
	@brief    file_name & simple comment.

	Description: Applicatuion for searching Services\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <mgr_common.h>

#include <xmgr_cas.h>
#include "./local_include/_xmgr_cas.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Functions     ********************/
/*******************************************************************/

BUS_Callback_t	XMGR_CAS_GetEmmupdateProc(void)
{
	BUS_Callback_t	pfnProc = NULL;

#if defined(CONFIG_MW_CAS_NAGRA)
	#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
		pfnProc = xproc_cas_NaEmmUpdate;
	#else
		pfnProc = NULL;
	#endif
#elif defined(CONFIG_MW_CAS_CONAX)
	pfnProc = NULL;
#elif defined(CONFIG_MW_CAS_IRDETO)
	pfnProc = NULL;
#elif defined(CONFIG_MW_CAS_VIACCESS)
	pfnProc = NULL;
#else
	pfnProc = NULL;
#endif

	return pfnProc;
}

/* end of file */
