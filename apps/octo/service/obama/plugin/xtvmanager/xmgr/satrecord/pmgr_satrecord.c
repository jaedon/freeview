/**
	@file     	pmgr_swup.c
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
#include <mgr_common.h>
#include <mgr_satrecord.h>
#include <pmgr_satrecord.h>
/*
#include <svc_rec.h>
*/
#include <_xmgr_satrecord_base.h>


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
BUS_Callback_t pmgr_satrecord_GetDetectPIDAction (void)
{

#if defined (CONFIG_MW_SATRECORD)
	return xproc_satrecord_DetectPID_Base;
#endif

	return NULL;
}

BUS_Callback_t pmgr_satrecord_GetDownloadAction (void)
{

#if defined (CONFIG_MW_SATRECORD)
	return xproc_satrecord_proc_Base;
#endif

	return NULL;
}

HERROR pmgr_satrecord_RescheduleBatch(void)
{
#if defined (CONFIG_MW_SATRECORD)
	return XMGR_SatRecord_RescheduleBatch_Base();
#endif
	return ERR_OK;

}

