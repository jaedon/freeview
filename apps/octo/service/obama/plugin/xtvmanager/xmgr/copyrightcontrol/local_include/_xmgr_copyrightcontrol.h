/**
	@file     _xmgr_copycontrol.h
	@brief    file_name & simple comment.

	Description :			\n
	Module :				\n
	Remarks :				\n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2013/04/12		initiated					moonsg\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__XMGR_COPYRIGHTCONTROL_INT_H__
#define	__XMGR_COPYRIGHTCONTROL_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <mgr_common.h>
#include <mgr_copyrightcontrol.h>
#include <pmgr_copyrightcontrol.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Plug-In Functions    ********************/
/*******************************************************************/


#if defined(CONFIG_MW_CAS_NAGRA_OP_SES) // DRM_TEST
HERROR xmgr_copyrightcontrol_InitCasDrmInfo_NagraSes(DxCopyrightControl_t *pstCasDrmInfo, MgrCopyrightControl_MediaControlInfo_t *pstDrmInfo);
HERROR xmgr_copyrightcontrol_InitSiDrmInfo_NagraSes(DxCopyrightControl_t *pstSiDrmInfo, MgrCopyrightControl_MediaControlInfo_t *pstDrmInfo);
HERROR xmgr_copyrightcontrol_GetCasCopyProtectInfo_NagraSes(DxCopyrightControl_t *pstCasDrmInfo, MgrCopyrightControl_OutputControlInfo_t *pstCpInfo);
HERROR xmgr_copyrightcontrol_GetSiCopyProtectInfo_Ses(DxCopyrightControl_t *pstSiDrmInfo, MgrCopyrightControl_OutputControlInfo_t *pstCpInfo);

HERROR xmgr_copyrightcontrol_GetCasMediaControlInfo_NagraSes(DxCopyrightControl_t *pstCasDrmInfo, MgrCopyrightControl_MediaControlInfo_t *pstCpInfo);
HERROR xmgr_copyrightcontrol_GetSiMediaControlInfo_NagraSes(DxCopyrightControl_t *pstSiDrmInfo, MgrCopyrightControl_MediaControlInfo_t *pstCpInfo);
#endif

#if defined(CONFIG_MW_CI_PLUS)
//HERROR MGR_CIPLUS_GetCopyProtectInfo(DxCopyrightControl_t *pstCasDrmInfo, MgrCopyrightControl_OutputControlInfo_t *pstCpInfo);
HERROR xmgr_copyrightcontrol_GetCopyProtectInfo_CiPlus(HBOOL bPlayback, DxCopyrightControl_t *pstCasDrmInfo, MgrCopyrightControl_OutputControlInfo_t *pstCpInfo);
#endif

#if defined(CONFIG_MW_CAS_NAGRA)
HERROR xmgr_copyrightcontrol_GetCopyProtectInfo_Nagra(DxCopyrightControl_t *pstCasDrmInfo, MgrCopyrightControl_OutputControlInfo_t *pstCpInfo);
#endif

#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
HERROR xmgr_copyrightcontrol_GetCasCopyProtectInfo_VmxIptv(DxCopyrightControl_t *pstCasDrmInfo, MgrCopyrightControl_OutputControlInfo_t *pstCpInfo);
HERROR xmgr_copyrightcontrol_GetCasMediaControlInfo_Verimatrix(DxCopyrightControl_t *pstCasDrmInfo, MgrCopyrightControl_MediaControlInfo_t *pstCpInfo);
#endif

#if defined(CONFIG_MW_CAS_CONAX)
HERROR xmgr_copyrightcontrol_GetCopyProtectInfo_Conax(HBOOL bPlayback, DxCopyrightControl_t *pstCasDrmInfo, MgrCopyrightControl_OutputControlInfo_t *pstCpInfo);
HERROR xmgr_copyrightcontrol_GetCasMediaControlInfo_Conax(DxCopyrightControl_t *pstCasDrmInfo, MgrCopyrightControl_MediaControlInfo_t *pstCpInfo);
#endif
#endif /* __XMGR_COPYRIGHTCONTROL_INT_H__ */

