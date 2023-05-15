/**
	@file     _xmgr_cas.h
	@brief    file_name & simple comment.

	Description :			\n
	Module :				\n
	Remarks :				\n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2013/04/17		initiated					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__XMGR_CAS_INT_H__
#define	__XMGR_CAS_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <bus.h>

#include <mgr_cas.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#if defined(CONFIG_MW_CI)
BUS_Result_t 	xproc_cas_CiMain (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
#endif

#if defined(CONFIG_MW_CAS_NAGRA)
BUS_Result_t	xproc_cas_NaMain (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
void			xmgr_cas_NaCmdInit (HxCMD_Handle_t hCmdHandle);
HERROR			xmgr_cas_NaInitialize (void);
HERROR			xmgr_cas_NaDefaultSetting (void);
HERROR			xmgr_cas_NaTotTdtUpdated (void);
#if  defined(CONFIG_MW_CAS_NAGRA_OP_UPC)
HERROR			xmgr_cas_NaIsScRemoved (HUINT8 ucIndex, HBOOL *pbRemoved);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
BUS_Result_t	xproc_cas_NaEmmUpdate (HINT32 lMessage, Handle_t hAction, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3);

HERROR	xmgr_cas_NaEmmUpdate_GetConfiguration(HUINT32 *pulWakeupInterval, HUINT32 *pulStartMin, HUINT32 *pulUpdateDuration);
HERROR	xmgr_cas_NaEmmUpdateRescheduleBatch(void);
#endif
#endif

#if defined(CONFIG_MW_CAS_IRDETO)
BUS_Result_t	xproc_cas_IrMain (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
HERROR			xmgr_cas_IrInitialize (void);
#endif

#if defined(CONFIG_MW_CAS_VIACCESS)
BUS_Result_t	xporc_cas_VaMain(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
HERROR			xmgr_cas_VaInitialize(void);
HERROR 		xmgr_cas_VaStart(void);
#endif

#if defined(CONFIG_MW_CAS_CONAX)
BUS_Result_t	xproc_cas_CxMain (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
HERROR			xmgr_cas_CxInitialize(void);
HERROR			xmgr_cas_CxDefaultSetting(void);
void			xmgr_cas_CxCmdInit(HxCMD_Handle_t hCmdHandle);
#endif

#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
BUS_Result_t	xproc_cas_VmxMain (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
//HERROR			xmgr_cas_VmxInitialize(void);
HERROR 			xmgr_cas_VmxDefaultSetting(void);
void 			xmgr_cas_VmxIptvCmd(HxCMD_Handle_t hCmdHandle);
#endif

#if defined(CONFIG_MW_CI_PLUS)
HBOOL 			xmgr_cas_CiPcIsRatingOK(HUINT32 ulActionId, DxRatingAge_e eRatingSetup);
#endif	// CONFIG_MW_CI_PLUS

#endif /* __XMGR_CAS_INT_H__ */


