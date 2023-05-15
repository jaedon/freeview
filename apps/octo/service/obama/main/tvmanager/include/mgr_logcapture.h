/**
	@file     mgr_logcapture.h
	@brief    file_name & simple comment.

	Description: Applicatuion for searching Services\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2013/03/27		initiated					chmin\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___MGR_LOGCAPTURE_H___
#define	___MGR_LOGCAPTURE_H___

/*
	Related Config(s)
	CONFIG_MW_LOGCAPTURE	: dependancy
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
enum
{
	eMEVT_LOGCAPTURE			= eMEVT_LOGCAPTURE_START,

	eMEVT_LOGCAPTURE_USB_DEVICE_INSERTION,
};

typedef enum
{
	eLOGCAPTURE_CAS_TYPE_BCAS = 0,
	eLOGCAPTURE_CAS_TYPE_CCAS,
	eLOGCAPTURE_CAS_TYPE_MAX,
} MgrLogcapture_CasType_e;

typedef enum
{
	eLOGCAPTURE_CAS_ERROR_INIT = 0,
	eLOGCAPTURE_CAS_ERROR_READ,
	eLOGCAPTURE_CAS_ERROR_WRITE,
	eLOGCAPTURE_CAS_ERROR_MAX,
} MgrLogcapture_CasError_e;

typedef enum
{
	eLOGCAPTURE_LIFECYCLE_POWERDOWN = 0,			// 기록 못 함
	eLOGCAPTURE_LIFECYCLE_POWERON,
	eLOGCAPTURE_LIFECYCLE_STANDBY,
	eLOGCAPTURE_LIFECYCLE_WAKEUP,
	eLOGCAPTURE_LIFECYCLE_MAX,
} MgrLogcapture_LifeCycle_e;

typedef enum
{
	eLOGCAPTURE_ERR_CAS_SC_ATR = 4,
	eLOGCAPTURE_ERR_CAS_SC_TRANSMIT,		//write
	eLOGCAPTURE_ERR_CAS_SC_RECEIVE,			//read
} MgrLogcapture_CasScError_e;

typedef enum
{
	eLOGCAPTURE_PVR_REC_STATUS_START = 0,		// ContentsID, RecordID
	eLOGCAPTURE_PVR_REC_STATUS_STOP,				// ContentsID, RecordID
	eLOGCAPTURE_PVR_REC_STATUS_PAUSE,			// ContentsID, RecordID
	eLOGCAPTURE_PVR_REC_STATUS_RESUME,			// ContentsID, RecordID
	eLOGCAPTURE_PVR_REC_STATUS_PAT,				// TS Unique ID, PMT Svc ID, PMT PID
	eLOGCAPTURE_PVR_REC_STATUS_PMT,				// svcID, tsID, onID
	eLOGCAPTURE_PVR_REC_STATUS_CAT,				// RecordID
} MgrLogcapture_PvrRecStatus_e;

typedef enum
{
	eLOGCAPTURE_PVR_PB_STATUS_START = 0,		// ContentID, PlayerID
	eLOGCAPTURE_PVR_PB_STATUS_STOP,				// ContentID, PlayerID
	eLOGCAPTURE_PVR_PB_STATUS_PAUSE,			// ContentID, PlayerID
	eLOGCAPTURE_PVR_PB_STATUS_RESUME,			// ContentID, PlayerID
	eLOGCAPTURE_PVR_PB_STATUS_PAT,				// TS Unique ID, PMT Svc ID, PMT PID
	eLOGCAPTURE_PVR_PB_STATUS_PMT,				// svcID, tsID, onID
} MgrLogcapture_PvrPbState_e;

/*******************************************************************/
/********************      Messages             ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
// mgr_logcapture.c
HERROR			MGR_Logcapture_Start (void);
// mgr_logcapture_flushdata.c
BUS_Callback_t	MGR_Logcapture_FlushDataGetProc (void);
HERROR			MGR_Logcapture_FlushDataStart (void);
HERROR			MGR_Logcapture_FlushDataStop (void);

HERROR			MGR_Logcapture_GetReport (void);

#endif /* ___MGR_LOGCAPTURE_H___ */

