/**
	@file     _xmgr_time.h
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

#ifndef	__XMGR_CLOCKRECOVER_INT_H__
#define	__XMGR_CLOCKRECOVER_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <mgr_common.h>
#include <mgr_time.h>
#include <pmgr_time.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

HERROR xmgr_time_Init_Base (void);

HERROR xmgr_time_ClockRecoverGetDstMode_Base (VK_CLOCK_DST_MODE *peDstMode);
MgrTime_ClockRecoverRefTpInfo_t *xmgr_time_ClockRecoverGetRefTpList_Base (void);

#if defined(CONFIG_OP_MIDDLE_EAST)
HERROR xmgr_time_ClockRecoverGetDstMode_MeSat (VK_CLOCK_DST_MODE *peDstMode);
MgrTime_ClockRecoverRefTpInfo_t *xmgr_time_ClockRecoverGetRefTpList_MeSat (void);
#endif
#if defined(CONFIG_OP_FREESAT)
HERROR xmgr_time_ClockRecoverGetDstMode_FreeSat (VK_CLOCK_DST_MODE *peDstMode);
MgrTime_ClockRecoverRefTpInfo_t *xmgr_time_ClockRecoverGetRefTpList_FreeSat (void);
#endif

#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
HERROR xmgr_time_ClockRecoverGetDstMode_Eu (VK_CLOCK_DST_MODE *peDstMode);
MgrTime_ClockRecoverRefTpInfo_t *xmgr_time_ClockRecoverGetRefTpList_Eu (void);
#endif

#if defined(CONFIG_PROD_FVP4000T)
MgrTime_ClockRecoverRefTpInfo_t *xmgr_time_ClockRecoverGetRefTpList_Ukdtt (void);
#endif

#if defined(CONFIG_PROD_HMX4KBBC)
HERROR xmgr_time_Init_Hmx4kbbc (void);
HERROR xmgr_time_SetGmtOffset_Hmx4kbbc(HINT32 nGmtOffset, HBOOL bUpdateTzFile);
#endif

HERROR xmgr_time_ClockRecoverSearchByFirstSvc_Base (MgrTime_ClockRecoverRefMethod_e eRefMethod, DxTuneParam_t *tuningInfo);

HERROR xmgr_time_SetGmtOffset_Base(HINT32 nGmtOffset, HBOOL bUpdateTzFile);
HERROR xmgr_time_SetGmtOffset_FreeSat(HINT32 nGmtOffset, HBOOL bUpdateTzFile);
HERROR xmgr_time_ClockRecoverSearchByTerTp_Base(HUINT32 ulRefTpIdx, DxTuneParam_t	*tuningInfo);
HERROR xmgr_time_ClockRecoverTune_Base(Handle_t hAction,DxTuneParam_t	*tuningInfo);
#if defined(CONFIG_PROD_FVP4000T)
HERROR xmgr_time_ClockRecoverSearchByTerTpList_Ukdtt(void);
HERROR xmgr_time_ClockRecoverSearchByTerTp_Ukdtt(HUINT32 ulRefTpIdx, DxTuneParam_t	*tuningInfo);
HERROR xmgr_time_ClockRecoverTune_Ukdtt(Handle_t hAction,DxTuneParam_t	*tuningInfo);
#endif


#endif /* __XMGR_CLOCKRECOVER_INT_H__ */

