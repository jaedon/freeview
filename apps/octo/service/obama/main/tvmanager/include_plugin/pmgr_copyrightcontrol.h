/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pmgr_copyrightcontrol.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___PMGR_COPYRIGHTCONTROL_H___
#define	___PMGR_COPYRIGHTCONTROL_H___


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <mgr_copyrightcontrol.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eCopyrightControl_Priority_Cas,
	eCopyrightControl_Priority_Si,
	eCopyrightControl_Priority_Component
} MgrCopyrightControl_Priority_e;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Plug-In Functions    ********************/
/*******************************************************************/

HERROR						pmgr_copyrightcontrol_GetSiCopyProtectInfo(HBOOL bPlayback, const DxCopyrightControl_t *pstSiDrmInfo, MgrCopyrightControl_OutputControlInfo_t *pstCpInfo);
HERROR						pmgr_copyrightcontrol_GetCasCopyProtectInfo(HBOOL bPlayback, const DxCopyrightControl_t *pstCasDrmInfo, MgrCopyrightControl_OutputControlInfo_t *pstCpInfo);

HERROR 						pmgr_copyrightcontrol_GetSiMediaControlInfo(HBOOL bPlayback, const DxCopyrightControl_t *pstSiDrmInfo, MgrCopyrightControl_MediaControlInfo_t *pstCpInfo);
HERROR						pmgr_copyrightcontrol_GetCasMediaControlInfo(DxCopyrightControl_CasCcType_e eDrmType, const DxCopyrightControl_t *pstCasDrmInfo, MgrCopyrightControl_MediaControlInfo_t *pstCpInfo);

MgrCopyrightControl_Priority_e		pmgr_copyrightcontrol_GetPriority(void);
HERROR 						pmgr_copyrightcontrol_InitCasCopyProtectInfo(DxCopyrightControl_t *pstCasDrmInfo, MgrCopyrightControl_MediaControlInfo_t *pstDrmInfo);
HERROR 						pmgr_copyrightcontrol_InitSiCopyProtectInfo(DxCopyrightControl_t *pstSiDrmInfo, MgrCopyrightControl_MediaControlInfo_t *pstDrmInfo);



#endif /* !___PMGR_COPYRIGHTCONTROL_H___ */

