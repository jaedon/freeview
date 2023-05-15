/**
	@file     	pmgr_search.c
	@brief

	Description:  			\n
	Module: 					 					\n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <bus.h>

#include <svc_sys.h>
#include <mgr_common.h>
#include <mgr_appmgr.h>
#include <mgr_3rdparty.h>

#include <mgr_init.h>
#include "./local_include/_xmgr_init.h"

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
/********************      Functions       *************************/
/*******************************************************************/

HERROR	pmgr_init_Module(void)
{
#if defined(CONFIG_3RD_RFVOD)
	MGR_RFVOD_Init();
#endif
#if defined(CONFIG_3RD_MHEG)
	(void)MGR_MHEG_Init();
#endif

	/* setup encryptMode*/
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	// iCordPro 즉 독일향 SES 모델은 3DES로 Encryption을 건다.
	SVC_SYS_SetPvrEncryptMode(ePvrEncrypt3Des);
#else
	SVC_SYS_SetPvrEncryptMode(ePvrEncryptAes);
#endif

	return ERR_OK;
}


/*********************** End of File ******************************/
