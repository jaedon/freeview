/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  	pal_rec.h
	@brief 	driver interfaces for Recorder operation

	Description: 		\n
	Module: pal/pvr			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__PAL_REC_H__
#define	__PAL_REC_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <pal_crypt.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	ePAL_REC_EVENT_TIMECODE_CHANGED 		= 0,
	ePAL_REC_EVENT_DISKFULL,
	ePAL_REC_EVENT_STREAM_UNSCRAMBLED,
	ePAL_REC_EVENT_STREAM_SCRAMBLED,
	ePAL_REC_EVENT_DELAYED_RECORD_STATUS_CHANGED,
	ePAL_REC_EVENT_FB_IMPOSSIBLE,
} PalRec_Event_e;

typedef enum
{
	eREC_Type_Unknown,
	eREC_Type_Rec,
	eREC_Type_Trans,
	eREC_Type_SatIp,

} PalRec_DeviceType_e;

typedef struct
{
	HUINT32					ulDemuxId;		/* Demux ID */
	PalFs_FileType_e		eFileType;		/* Linear or Circular */
	StreamType_e			eStreamType;	/* Video or Audio */
	DxVideoCodec_e			eVideoType;		// it depends on broadcom chip, so need to specify this.
	HUINT8					aucFileName[PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH];
	PalCrypt_Setting_t		stEncryptSettings;

	/*when output-type is file*/
	HBOOL	bTimestamp;
	HBOOL	bPcrTsPacketInsert;
	HBOOL	bIsAllPidChannel;

	struct {
		HBOOL			bIsUsbFlashRecord;	/* Set TRUE if recording content into USB Flash */
		HUINT32			ulLimitTimeSecond;	/* Time limitation for TSR */
		HUINT64			ullLimitDataSize;	/* Data limitation for TSR */
		HUINT64			ullLimitIndexSize;	/* Index limitation for TSR */
	} stUsbFlash;
	
} PalRec_Setup_t;


typedef struct
{
	HUINT8					aucFileName[PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH];
	HUINT8 					aucTempFileName[PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH]; /* Delayed copy source file name */
	HULONG					ulStartTime; /* Delayed record start position in time(sec) */
	HUINT32					ulInstanceId; /* copy process instance id */

	PalCrypt_Setting_t	stEncryptSettings;
} PalRec_DelayedSetup_t;


typedef struct
{
	HUINT8					aucFileName[PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH];
	HUINT8 					aucTempFileName[PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH]; /* Delayed copy source file name */
	HULONG					ulStartTime; /* Delayed record start position in time(sec) */
	HULONG					ulEndTime;	/* Delayed record end position in time(sec) */
	HUINT32					ulInstanceId; /* copy process instance id */

	PalCrypt_Setting_t	stEncryptSettings;
} PalRec_DelayedSplitSetup_t;

/** @brief proto for callback which MW monitors recording operation */
typedef void (*PalRec_Callback_t)(HUINT32 event, HUINT32 deviceId, HUINT32 param1, HUINT32 param2, HUINT32 param3);

/** @brief proto for callback which MW receives the recording contents via the memory  */
typedef void (*PalRec_MemCallback_t) (HUINT32 deviceId, HUINT8 *pucBuf, HUINT32 ulLength);

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

/**
  @remark	initializes recorder adaptor and context.\n
  @remark	API Type : inline \n

  @return		error
*/
HUINT32			PAL_REC_Init(void);

/**
  @remark	register a recorder's event callback for MW notification.\n
  @remark	API Type : inline \n

  @return		none
*/
void			PAL_REC_RegisterNotifier(
						HUINT32 ulDeviceId,
						PalRec_Callback_t pfRecCallback		/**< */
						);

/**
  @remark	unregister a recorder's event callback for MW notification.\n
  @remark	API Type : inline \n

  @return		none
*/
void			PAL_REC_UnregisterNotifier(
						HUINT32 ulDeviceId,
						PalRec_Callback_t pbCallback	/**< */
						);

/**
  @remark	register a recorder's memory contents callback for MW notification.\n
  @remark	API Type : inline \n

  @return		none
*/
void			PAL_REC_RegisterMemoryCallback(
						HUINT32 ulDeviceId,
						PalRec_MemCallback_t pfMemCb
						);

/**
  @remark	unregister a recorder's memory contents callback for MW notification.\n
  @remark	API Type : inline \n

  @return		none
*/
void			PAL_REC_UnregisterMemoryCallback(
						HUINT32 ulDeviceId,
						PalRec_MemCallback_t pfMemCb
						);

/**
Get Recorder capability
@param[out] HUINT32 *pulNumOfDeviceId   - number of recorder device available
*/
HUINT32			PAL_REC_GetCapability(HUINT32 *pulNumOfDeviceId);


/**
Get Recorder Device Type
@param[out] PalRec_DeviceType_e *pRecType   - device type
*/
HERROR			PAL_REC_GetType(HUINT32 ulDeviceId, PalRec_DeviceType_e *pRecType);


/**
Setup Recorder Device
@param[in] HUINT32 ulDeviceId - recorder id
@param[in] PalRec_Setup_t *pstSetup   - recorder setup information
*/
HUINT32	 		PAL_REC_Setup(HUINT32 ulDeviceId, PalRec_Setup_t *pstSetup );

/**
Start Recorder device
@param[in] HUINT32 ulDeviceId - recorder id
*/
HUINT32 		PAL_REC_Start(HUINT32 ulDeviceId );


/**
Start Delayed Record
@param[in] HUINT32 ulDeviceId - recorder id
@param[in] PalRec_DelayedSetup_t *pstSetup   - delayed record setup information
*/
HUINT32	 		PAL_REC_StartDelayed(HUINT32 ulDeviceId, PalRec_DelayedSetup_t *pstSetup );


/**
It is used only for copy of recorded contents in delayed record process
@param[in] PalRec_DelayedSetup_t *pstSetup   - delayed record setup information
*/
HUINT32	 		PAL_REC_StartDelayedSplit( PalRec_DelayedSplitSetup_t *pstSetup );


/**
Start Delayed Record
@param[in] HUINT32 ulRequestId - copy process instance id
@param[out] HUINT32 *pulStatus - copy status ( 0~100 : normal (percent), 200: error)
*/
HUINT32			PAL_REC_GetDelayRecordStatus( HUINT32 ulRequestId, HUINT32 *pulStatus );


/**
Stop Recorder device
@param[in] HUINT32 ulDeviceId - recorder id
*/
HUINT32 		PAL_REC_Stop( HUINT32 ulDeviceId );


/**
Pause Recorder device
@param[in] HUINT32 ulDeviceId - recorder id
*/
HUINT32 		PAL_REC_Pause( HUINT32 ulDeviceId );


/**
Resume Recorder device
@param[in] HUINT32 ulDeviceId - recorder id
*/
HUINT32 		PAL_REC_Resume(HUINT32 ulDeviceId );


/**
Resume Recorder device
@param[in] HUINT32 ulDeviceId - recorder id
@param[out] HUINT32 *pulTime   - recorder duration
*/
HUINT32 		PAL_REC_GetDuration(HUINT32 ulDeviceId, HUINT32 *pulTime );

/**
Start Record Pid Filter
@param[in] HUINT32 ulDeviceId - recorder id
@param[in] HUINT16 usPid - Pid
@param[in] EsType_t eEsType - Record ES Type
@param[out] HUINT32 *pulRequestId  - request id (need to stop RecordPid )
*/
HUINT32 		PAL_REC_StartPid(HUINT32 ulDeviceId, HUINT16 usPid, EsType_t eEsType, HUINT32 *pulRequestId );

/**
Stop Record Pid Filter
@param[in] HUINT32 ulDeviceId - recorder id
@param[out] HUINT32 ulRequestId  - request id (paramter that got PAL_REC_StartPid )
*/
HUINT32 		PAL_REC_StopPid(HUINT32 ulDeviceId, HUINT32 ulRequestId );

/**
set encrytion info
@param[in] HUINT32 ulDeviceId - recorder id
@param[in] PalCrypt_EncryptionType_e eEncType - encryption type
@param[in] HUINT8 pucKey - Key (16bytes)
@param[in] HUINT8 pucIv - Iv key
*/
HUINT32			PAL_REC_SetEncryption( HUINT32 ulDeviceId, PalCrypt_EncryptionType_e eEncType, HUINT8 *pucKey, HUINT8 *pucIv );


/**
set replace packet info
@param[in] HUINT32 ulDemuxId - demux id
@param[in] HUINT16 usPid - Pid
@param[in] HUINT8 *pucReplaced - replace packet
@param[in] HUINT32 packetSize - replace packet size
@param[out] HUINT32 *pulHandle - replace packet handle for calling PAL_REC_ReplacePacketModeOff
*/
HUINT32 		PAL_REC_ReplacePacketModeOn(HUINT32 ulDemuxId, HUINT16 usVirtualPid, HUINT16 usPid, HUINT8 *pucSection, HUINT32 ulSectionSize, HUINT32 *pulHandle);

/**
set replace packet info
@param[in] HUINT32 ulHandle - replace packet handle
*/
HUINT32 		PAL_REC_ReplacePacketModeOff(HUINT32 ulHandle);

/**
set replace packet info
@param[in] HUINT32 ulDemuxId - demux id
@param[in] HUINT16 usPid - Pid
@param[in] HUINT8 *pucReplaced - replace packet
@param[in] HUINT32 packetSize - replace packet size
@param[out] HUINT32 *pulHandle - insert packet handle for calling PAL_REC_InsertPacketModeOff
*/
HUINT32 		PAL_REC_InsertPacketModeOn(HUINT32 ulDemuxId, HUINT16 uPid, HUINT8 *pucSection, HUINT32 ulSectionSize, HUINT32 ulInterval, HUINT32 *pulHandle);

/**
set replace packet info
@param[in] HUINT32 ulHandle - insert packet handle
*/
HUINT32 		PAL_REC_InsertPacketModeOff(HUINT32 ulHandle);


#endif /* !__PAL_REC_H__ */
