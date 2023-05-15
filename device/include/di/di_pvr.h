#ifndef	__DI_PVR_H__
#define	__DI_PVR_H__

#include <htype.h>
#include <di_err.h>
#include <di_media_types.h>

#define	DI_PVR_MAX_NAME_LEN		255
#define	DI_PVR_CRYPTO_KEY_LEN		16

typedef enum
{
	DI_PVR_FREE=0,
	DI_PVR_ALLOCATED,
	DI_PVR_ENABLED,
	DI_PVR_PAUSED
}	DI_PVR_STATUS;

typedef enum
{
	DI_PVR_PLAY_START_MODE_BEGIN,
	DI_PVR_PLAY_START_MODE_LASTVIEW,
	DI_PVR_PLAY_START_MODE_END
}	DI_PVR_PLAY_START_MODE;

typedef enum
{
	DI_PVR_REC_PID_PES_NONE=0,
	DI_PVR_REC_PID_PES_TYPE_VIDEO,
	DI_PVR_REC_PID_PES_TYPE_AUDIO,
	DI_PVR_REC_PID_PES_TYPE_PCR,
	DI_PVR_REC_PID_PES_TYPE_SUBTITLE,
	DI_PVR_REC_PID_PES_TYPE_TELETEXT,
	DI_PVR_REC_PID_PES_TYPE_ALL
}	DI_PVR_REC_PID_PES_TYPE;

typedef	enum
{
	DI_PVR_EVENT_EOF=0,
	DI_PVR_EVENT_BOF,
	DI_PVR_EVENT_TIMECODE_CHANGED,
	DI_PVR_EVENT_STREAM_UNSCRAMBLED,
	DI_PVR_EVENT_STREAM_SCRAMBLED,
	DI_PVR_EVENT_DELAYED_RECORD_STATUS_CHANGED,
	DI_PVR_EVENT_DISK_INIT_START,
	DI_PVR_EVENT_DISK_INIT_END,
	DI_PVR_EVENT_DISK_FULL,
	DI_PVR_EVENT_DISK_ERROR,
	DI_PVR_EVENT_EDIT_STATUS_CHANGED,
	DI_PVR_EVENT_REC_BUFFER_OVERFLOW,
	DI_PVR_EVENT_REC_WRITE_ERROR,
	DI_PVR_EVENT_MAX_NUMBER
}	DI_PVR_EVENT_t;

typedef enum
{
	DI_PVR_RECORDER,
	DI_PVR_PLAYER,
	DI_PVR_EDITOR
}	DI_PVR_DEVICE;

typedef	enum
{
	eDI_PVR_FileType_Linear = 0,
	eDI_PVR_FileType_Circular	
}	DI_PVR_FILE_TYPE_t;

typedef	enum
{
	eDI_PVR_Stream_Video = 0,
	eDI_PVR_Stream_Audio
}	DI_PVR_STREAMTYPE_t;

typedef enum
{
	eDI_PVR_VideoType_None 			= 0x00000000,	/* not coded video */
	eDI_PVR_VideoType_Mpeg1 			= 0x00000001,	/* MPEG-1 Video (ISO/IEC 11172-2) */
	eDI_PVR_VideoType_Mpeg2 			= 0x00000002,	/* MPEG-2 Video (ISO/IEC 13818-2) */
	eDI_PVR_VideoType_Mpeg2Dtv      	= 0x00000004,	/* MPEG2DTV Video */
	eDI_PVR_VideoType_Mpeg2Dtv_Pes	= 0x00000008,	/* MPEG2DTV_PES Video */
	eDI_PVR_VideoType_Mpeg4_Part2 	= 0x00000010,	/* MPEG-4 Part 2 Video */
	eDI_PVR_VideoType_H261			= 0x00000020,	/**< H.261 Video */
	eDI_PVR_VideoType_H263 	 		= 0x00000040,	/* H.263 Video. The value of the enum is not based on PSI standards. */
	eDI_PVR_VideoType_H264 			= 0x00000080,	/* H.264 (ITU-T) or ISO/IEC 14496-10/MPEG-4 AVC */
	eDI_PVR_VideoType_Vc1_Sm 		= 0x00000100,	/* VC-1 Simple&Main Profile */
	eDI_PVR_VideoType_Vc1				= 0x00000200,     	/* VC-1 Advanced Profile */
	eDI_PVR_VideoType_Avs           	= 0x00000400,	/* AVS Video */
	eDI_PVR_VideoType_Divx_311 		= 0x00000800,      	/* DivX 3.11 coded video */
	eDI_PVR_VideoType_H265 			= 0x00008000	/* H.265 */
}	DI_PVR_VIDEOTYPE_t;

typedef enum
{
	DI_PVR_ENCRYPTION_TYPE_NONE,
	DI_PVR_ENCRYPTION_TYPE_3DES,
	DI_PVR_ENCRYPTION_TYPE_AES,
	DI_PVR_ENCRYPTION_TYPE_CURRENT, /* Used for delayed record(or split) */
	DI_PVR_ENCRYPTION_TYPE_3DES_SECURED_KEY,
	DI_PVR_ENCRYPTION_TYPE_AES_SECURED_KEY
}	DI_PVR_ENCRYPTION_TYPE;

typedef enum
{
	DI_PVR_INST_TYPE_NONE,
	DI_PVR_INST_TYPE_REC,
	DI_PVR_INST_TYPE_TRANS,
	DI_PVR_INST_TYPE_SATIP
}	DI_PVR_INST_TYPE;

typedef struct
{
	HUINT32 	ulSender;		// sender is recorder or player
	HUINT32 	ulInstance;		// Instance Id
	HUINT32	   	ulEvent;			//DI_PVR_EVENT_t
	HUINT32		ulParam1;		
	HUINT32		ulParam2;
	HUINT32		ulParam3;
} DI_PVR_MSG_t;

typedef struct
{
	DI_PVR_ENCRYPTION_TYPE eEncType;
	HUINT8 ucKey[DI_PVR_CRYPTO_KEY_LEN];
	HUINT8 ucIv[DI_PVR_CRYPTO_KEY_LEN];
	HUINT8 ucProtectionKey[DI_PVR_CRYPTO_KEY_LEN];
} DI_PVR_CRYPTO_SETTINGS;

typedef struct
{
	DI_PVR_FILE_TYPE_t		eFileType;		/* Linear or Circular */
	DI_PVR_STREAMTYPE_t		eStreamType;	/* Video or Audio */
	DI_PVR_VIDEOTYPE_t		eVideoType;		// it depends on broadcom chip, so need to specify this.
	HUINT8					aucFileName[DI_PVR_MAX_NAME_LEN];	
	DI_PVR_CRYPTO_SETTINGS	stEncryptSettings;
	HUINT32 				ulDemuxId;     /* Demux ID(path info) associated with this REC instance */
	HUINT32					ulPacketSize; /* 188 or 192*/
	HBOOL					bIsAllPidChannel;
	struct {
		HBOOL				bIsUsbFlashRecord; /* Set TRUE if recording content into USB Flash */
		HUINT32				ulLimitTimeSecond; /* Time limitation for TSR */
		HUINT64				ullLimitDataSize; /* Data limitation for TSR */
		HUINT64				ullLimitIndexSize; /* Index limitation for TSR */
	} stUsbFlash;

} DI_PVR_REC_SETUP_t;

typedef struct
{
	HUINT8 aucFileName[DI_PVR_MAX_NAME_LEN]; /* Record file name */
	HUINT8 aucTempFileName[DI_PVR_MAX_NAME_LEN]; /* Delayed copy source file name */
	unsigned long ulStartTime; /* Delayed record start position in time(sec) */
	HUINT32 ulRequestId;
	DI_PVR_CRYPTO_SETTINGS stEncryptSettings;
} DI_PVR_REC_DELAYRECORD_SETUP_t;

typedef struct
{
	HUINT8 aucFileName[DI_PVR_MAX_NAME_LEN]; /* Record file name */
	HUINT8 aucTempFileName[DI_PVR_MAX_NAME_LEN]; /* Delayed split source file name */
	unsigned long ulStartTime; /* Delayed split start position in time(sec) */
	unsigned long ulEndTime; /* Delayed split end position in time(sec) */
	HUINT32 ulRequestId;
	DI_PVR_CRYPTO_SETTINGS stEncryptSettings;
} DI_PVR_REC_DELAYSPLIT_SETUP_t;

typedef struct
{
	DI_PVR_FILE_TYPE_t		eFileType;		/* Linear or Circular */
	DI_PVR_STREAMTYPE_t		eStreamType;	/* Video or Audio */
	HUINT8					aucFileName[DI_PVR_MAX_NAME_LEN];
	DI_PVR_PLAY_START_MODE	eStartCondition;
	HUINT32					ulStartTime;		/* Specify start time  */
	HBOOL					bIsRecording;	/* Specify if recording is progressing or not  */
	DI_PVR_CRYPTO_SETTINGS	stDecryptSettings;
	HUINT32 				ulDemuxId;     /* Demux ID(path info) associated with this PB instance */
	HUINT32					ulPacketSize; /* 188 or 192*/
	HBOOL					bIsUsbFlashRecord; /* Set TRUE, if recording or recorded content in USB Flash */
	HBOOL					bStartPaused;		/* Playback start with pause */
} DI_PVR_PLAY_SETUP_t;

typedef struct
{
	HUINT8			aucFileName[DI_PVR_MAX_NAME_LEN];
	HUINT32			ulCaptureTime;
	DI_PVR_CRYPTO_SETTINGS	stDecryptSettings;
	HUINT32			VideoCodec;
	HUINT32			ulVideoPid;
	HBOOL			bTsr;
	HINT32			iRecDeviceId;	
	HUINT32			ulPacketSize; /* 188 or 192*/
} DI_PVR_THUMBNAIL_SETUP_t;

typedef struct
{
	HUINT8					aucSrcFileName[DI_PVR_MAX_NAME_LEN];	
	HUINT64 				ullStartOffset;
	HUINT64					ullEndOffset;
} DI_PVR_EDIT_FILE_t;

typedef struct
{
	HUINT8					aucDestFileName[DI_PVR_MAX_NAME_LEN];	
	HUINT32					ulSrcFileNum;
	DI_PVR_EDIT_FILE_t 		*stSrcFileInfo;
} DI_PVR_EDIT_SETUP_t;

typedef void (*DI_PVR_Event_Callback)(void *pvParam);
typedef void (*DI_PVR_Rec_DataRdy_Callback)(int deviceId, const void *buf, unsigned int length);


/************* RECORDER ********************/
/**
Get Recorder capability
@param[out] HUINT32 *pulNumOfDeviceId   - number of recorder device available
*/
DI_ERR_CODE DI_PVR_REC_GetCapability(HUINT32 *pulNumOfDeviceId);

/**
Get Recorder Device Tyhpe
@param[in] HUINT32 ulDeviceId - recorder device ID
@param[out] DI_PVR_INST_TYPE *eInstType - recorder instance type
*/
DI_ERR_CODE DI_PVR_REC_GetDeviceType(HUINT32 ulDeviceId, DI_PVR_INST_TYPE *eInstType);

/**
Register Recorder Event & Callback function
@param[in] HUINT32 ulDeviceId - recorder device ID
@param[in] DI_PVR_EVENT_t eEvent - recorder event
@param[in] DI_PVR_Event_Callback pCallbackFunc - callback processing recorder event
*/
DI_ERR_CODE DI_PVR_REC_RegisterEventCallback(HUINT32 ulDeviceId, DI_PVR_EVENT_t eEvent, DI_PVR_Event_Callback pCallbackFunc);


/**
Setup Recorder Device
@param[in] HUINT32 ulDeviceId - recorder device ID
@param[in] DI_PVR_REC_SETUP_t *pstSetup   - recorder setup information
*/
DI_ERR_CODE DI_PVR_REC_Setup(HUINT32 ulDeviceId, DI_PVR_REC_SETUP_t *pstSetup );


/**
Start Recorder device
@param[in] HUINT32 ulDeviceId - recorder device ID
*/
DI_ERR_CODE DI_PVR_REC_Start(HUINT32 ulDeviceId );


/**
Stop Recorder device
@param[in] HUINT32 ulDeviceId - recorder device ID
*/
DI_ERR_CODE DI_PVR_REC_Stop(HUINT32 ulDeviceId );


/**
Setup Recorder Device for delayed recording
@param[in] HUINT32 ulDeviceId - recorder device ID
@param[in] DI_PVR_REC_DELAYRECORD_SETUP_t *pstSetup   - recorder setup information
*/
DI_ERR_CODE DI_PVR_REC_SetDelayRecording(HUINT32 ulDeviceId, DI_PVR_REC_DELAYRECORD_SETUP_t *pstSetup);


/**
Start delayed split
@param[in] DI_PVR_REC_DELAYSPLIT_SETUP_t *pstSetup   - setup information
*/
DI_ERR_CODE DI_PVR_REC_StartDelaySplit(DI_PVR_REC_DELAYSPLIT_SETUP_t *pstSetup);


/**
Get delayed recording status
@param[in] HUINT32 ulRequestId - delayed recording request  ID
@param[in] HUINT32 *pulStatus   - delayed recording status
*/
DI_ERR_CODE DI_PVR_REC_GetDelayRecordingStatus(HUINT32 ulRequestId, HUINT32 *pulStatus);
	

/**
Add PID channel to Recorder Device
@param[in] HUINT32 ulDeviceId - recorder device ID
@param[in] HUINT16 usPid - recorder PID
@param[in] DI_PVR_REC_PID_PES_TYPE ePesType - PES type of record PID
@param[out] HUINT32 *pulRequestId - pointer to request ID output
*/
DI_ERR_CODE DI_PVR_REC_AddPidChannel(HUINT32 ulDeviceId, HUINT16 usPid, DI_PVR_REC_PID_PES_TYPE ePesType, HUINT32 *pulRequestId);

/**
Remove PID channel from Recorder Device
@param[in] HUINT32 ulDeviceId - recorder device ID
@param[in] HUINT32 ulRequestId - request ID
*/
DI_ERR_CODE DI_PVR_REC_RemovePidChannel(HUINT32 ulDeviceId, HUINT32 ulRequestId);

/**
Enable/Disable encryption & set encryption key
@param[in] HUINT32 ulDeviceId - recorder device ID
@param[in] DI_PVR_CRYPTO_SETTINGS *pstEncryptSettings - pointer to encryption settings
*/
DI_ERR_CODE DI_PVR_REC_SetEncryption(HUINT32 ulDeviceId, DI_PVR_CRYPTO_SETTINGS *pstEncryptSettings);

/**
Pause Recorder device
@param[in] HUINT32 ulDeviceId - recorder device ID
*/
DI_ERR_CODE DI_PVR_REC_Pause(HUINT32 ulDeviceId );


/**
Resume Recorder device
@param[in] HUINT32 ulDeviceId - recorder device ID
*/
DI_ERR_CODE DI_PVR_REC_Resume(HUINT32 ulDeviceId );


/**
Resume Recorder device
@param[in] HUINT32 ulDeviceId - recorder device ID
@param[out] HUINT32 *pulTime   - recorder duration
*/
DI_ERR_CODE DI_PVR_REC_GetDuration(HUINT32 ulDeviceId, HUINT32 *pulTime );

/**
Convert Time to Position
@param[in] HUINT32 ulDeviceId - recorder device ID
@param[in] HUINT32 ulTime - recorded time 
@param[out] HUINT64 *pulPosition   - recorded position
*/
DI_ERR_CODE  DI_PVR_REC_ConvertTimeToPosition(HUINT32 ulDeviceId, HUINT32 ulTime, HUINT64 *pulPosition) ;


/**
Convert Time to Position
@param[in] HUINT32 ulDeviceId - recorder device ID
@param[in] HUINT64 ulPosition - recorded position
@param[out] HUINT32 *pulTime   - recorded time
*/
DI_ERR_CODE  DI_PVR_REC_ConvertPositionToTime(HUINT32 ulDeviceId, HUINT64 ulPosition, HUINT32 *pulTime) ;


/**
Register Callback to get record data.
@param[in] HUINT32 ulDeviceId - recorder device ID
@param[in] DI_PVR_Rec_DataRdy_Callback pCallbackFunc - CB function pointer
*/
DI_ERR_CODE DI_PVR_REC_RegisterMemIOCallback(HUINT32 ulDeviceId, DI_PVR_Rec_DataRdy_Callback pCallbackFunc);


/************* PLAYER ********************/
/**
Get player capability
@param[out] HUINT32 *pulNumOfDeviceId   - number of player device available
*/
DI_ERR_CODE DI_PVR_PLAY_GetCapability(HUINT32 *pulNumOfDeviceId);


/**
Register player Event & Callback function
@param[in] HUINT32 ulDeviceId - player device ID
@param[in] DI_PVR_EVENT_t eEvent - player event
@param[in] DI_PVR_Event_Callback pCallbackFunc - callback processing player event
*/
DI_ERR_CODE DI_PVR_PLAY_RegisterEventCallback(HUINT32 ulDeviceId, DI_PVR_EVENT_t eEvent, DI_PVR_Event_Callback pCallbackFunc);


/**
Setup player device
@param[in] HUINT32 ulDeviceId - player device ID
@param[in] DI_PVR_PLAY_SETUP_t *pstSetup   - player setup information
*/
DI_ERR_CODE DI_PVR_PLAY_Setup(HUINT32 ulDeviceId, DI_PVR_PLAY_SETUP_t *pstSetup );


/**
Start player device
@param[in] HUINT32 ulDeviceId - player device ID
*/
DI_ERR_CODE DI_PVR_PLAY_Start(HUINT32 ulDeviceId );


/**
Stop player device
@param[in] HUINT32 ulDeviceId - player device ID
*/
DI_ERR_CODE DI_PVR_PLAY_Stop(HUINT32 ulDeviceId );

/**
Enable/Disable decryption & set decryption key
@param[in] HUINT32 ulDeviceId - player device ID
@param[in] DI_PVR_CRYPTO_SETTINGS *pstDecryptSettings - pointer to decryption settings
*/
DI_ERR_CODE DI_PVR_PLAY_SetDecryption(HUINT32 ulDeviceId, DI_PVR_CRYPTO_SETTINGS *pstDecryptSettings);

/**
Pause player device
@param[in] HUINT32 ulDeviceId - player device ID
*/
DI_ERR_CODE DI_PVR_PLAY_Pause(HUINT32 ulDeviceId );


/**
Resume player device
@param[in] HUINT32 ulDeviceId - player device ID
*/
DI_ERR_CODE DI_PVR_PLAY_Resume(HUINT32 ulDeviceId );

/**
Advance frame
@param[in] HUINT32 ulDeviceId - player device ID
@param[in] HBOOL bForward - flag for advance forward
*/
DI_ERR_CODE DI_PVR_PLAY_FrameAdvance(HUINT32 ulDeviceId, HBOOL bForward );


/**
Set playback speed
@param[in] HUINT32 ulDeviceId - player device ID
@param[in] DI_MEDIA_PlaySpeed_e eSpeed - speed
*/
DI_ERR_CODE DI_PVR_PLAY_SetSpeed(HUINT32 ulDeviceId, DI_MEDIA_PlaySpeed_e eSpeed );


/**
Get playback speed
@param[in] HUINT32 ulDeviceId - player device ID
@param[out] DI_MEDIA_PlaySpeed_e *peSpeed - speed
*/
DI_ERR_CODE DI_PVR_PLAY_GetSpeed(HUINT32 ulDeviceId, DI_MEDIA_PlaySpeed_e *peSpeed );


/**
Set playback time
@param[in] HUINT32 ulDeviceId - player device ID
@param[in] HUINT32 ulTime - time to play
*/
DI_ERR_CODE DI_PVR_PLAY_SetPosition(HUINT32 ulDeviceId, HUINT32 ulTime );


/**
Get current playing time
@param[in] HUINT32 ulDeviceId - player device ID
@param[out] HUINT32 *pulTime - current playing time
*/
DI_ERR_CODE DI_PVR_PLAY_GetPosition(HUINT32 ulDeviceId, HUINT32 *pulTime );


/**
Get play duration
@param[in] HUINT32 ulDeviceId - player device ID
@param[out] HUINT32 *pulDuration - play duration
*/
DI_ERR_CODE DI_PVR_PLAY_GetDuration(HUINT32 ulDeviceId, HUINT32 *pulDuration );


/**
Convert Time to Position
@param[in] HUINT32 ulDeviceId - player device ID
@param[in] HUINT32 ulTime - play time 
@param[out] HUINT64 *pulPosition   - play position
*/
DI_ERR_CODE  DI_PVR_PLAY_ConvertTimeToPosition(HUINT32 ulDeviceId, HUINT32 ulTime, HUINT64 *pulPosition) ;


/**
Convert Time to Position
@param[in] HUINT32 ulDeviceId - player device ID
@param[in] HUINT64 ulPosition - play position
@param[out] HUINT32 *pulTime   - play time
*/
DI_ERR_CODE  DI_PVR_PLAY_ConvertPositionToTime(HUINT32 ulDeviceId, HUINT64 ulPosition, HUINT32 *pulTime) ;

/**
Convert Time to File offset
@param[in] const HUINT8 *pszRecordedFilePath - Index file path
@param[in] HULONG ulFileOffsetHi
@param[in] HULONG ulFileOffsetLow
@param[out] HULONG *pulResultTimeMs
*/
DI_ERR_CODE  DI_PVR_PLAY_ConvertBytesByIndexToTime(const HUINT8 *pszRecordedFilePath, 
					HULONG ulFileOffsetHi, HULONG ulFileOffsetLow, HULONG *pulResultTimeMs);

/**
Convert Time to File offset
@param[in] const HUINT8 *pszRecordedFilePath - Index file path
@param[in] HULONG ulRequestTimeMs 
@param[out] HULONG *pulResultFileOffsetHi
@param[out] HULONG *pulResultFileOffsetLow
@param[out] HULONG *pulResultTimeMs
*/
DI_ERR_CODE  DI_PVR_PLAY_ConvertTimeToBytesByIndex(const HUINT8 *pszRecordedFilePath, 
					HULONG ulRequestTimeMs, HULONG *pulResultFileOffsetHi, 
					HULONG *pulResultFileOffsetLow, HULONG *pulResultTimeMs);

/**
Get duration
@param[in] const HUINT8 *pszRecordedFilePath - Index file path
@param[out] HULONG *pulDurationMs
*/
DI_ERR_CODE  DI_PVR_PLAY_GetDurationByIndex(const HUINT8 *pszRecordedFilePath, HULONG *pulDurationMs);
DI_ERR_CODE DI_PVR_PLAY_GetPlaypumpResourceMap(void** pMap, HUINT32* puiResourceNum);
DI_ERR_CODE DI_PVR_PLAY_GetPlaypumpIndex(HUINT32* puiIndex);
DI_ERR_CODE DI_PVR_PLAY_ReleasePlaypumpByIndex(HUINT32 uiIndex);

DI_ERR_CODE DI_TRANSCODE_REC_Setup(HUINT32 ulDeviceId, HUINT32 ulMuxId , DI_PVR_REC_SETUP_t *pstSetup );
DI_ERR_CODE DI_TRANSCODE_REC_Start(HUINT32 ulDeviceId );
DI_ERR_CODE DI_TRANSCODE_REC_Stop(HUINT32 ulDeviceId );
DI_ERR_CODE DI_TRANSCODE_REC_Unset(HUINT32 ulDeviceId, HUINT32 ulMuxId );
typedef void (*DI_TRANCODE_DataRdy_Callback)(int deviceId, const void *buf, unsigned int length);
DI_ERR_CODE DI_TRANSCODE_RegisterCallback(HUINT32 ulDeviceId, DI_TRANCODE_DataRdy_Callback pCallbackFunc);

DI_ERR_CODE DI_PVR_PLAY_GetThumbnail(DI_PVR_THUMBNAIL_SETUP_t *pthumbSetup, HUINT8 *CapturedBitmap);
/**
Setup file edit
@param[in] DI_PVR_EDIT_SETUP_t *pstEditSetup   - File Edit Information
*/
DI_ERR_CODE DI_PVR_EDIT_CreateIndexFile(HUINT32 ulDeviceId, DI_PVR_EDIT_SETUP_t *pstEditSetup);

/**
Register player Event & Callback function
@param[in] DI_PVR_EVENT_t eEvent - edit event
@param[in] DI_PVR_Event_Callback pCallbackFunc - callback processing edit event
*/
DI_ERR_CODE DI_PVR_EDIT_RegisterEventCallback(HUINT32 ulDeviceId, DI_PVR_EVENT_t eEvent, DI_PVR_Event_Callback pCallbackFunc);
DI_ERR_CODE DI_PVR_REC_Flush(HUINT32 ulDeviceId);
#endif /* !__DI_PVR_H__ */

