/**
@defgroup Demux demux di layer
Define Demux functions
*/
/*@{*/

/**
@file di_demux.h
@brief demux header file
@author beng & hykim

@remarks
    -# 2009.03.28 : second release
	-# 2008.08.17 : first release
*/


#ifndef __DI_DEMUX_H__
#define __DI_DEMUX_H__

#include "htype.h"
#include "vkernel.h"
#include "di_err.h"
#include "di_channel.h"

#define PID_BITMASK_PCR  0x00000001
#define PID_BITMASK_VIDEO 0x00000002
#define PID_BITMASK_AUDIO_MAIN 0x00000004
#define PID_BITMASK_AUDIO_SUB 0x00000008
#define PID_BITMASK_AUDIO_SPDIF 0x00000010

/* add for PIP... */
#define PID_BITMASK_PCR_SUB  	0x00000040
#define PID_BITMASK_VIDEO_SUB 	0x00000080

/* add for TRANSCODE... 2 transcode */
#define PID_BITMASK_PCR_TRANS_1  	0x00000100
#define PID_BITMASK_VIDEO_TRANS_1 	0x00000200
#define PID_BITMASK_AUDIO_TRANS_1 	0x00000400
#define PID_BITMASK_AUDIO_SUB_TRANS_1 0x00000800
#define PID_BITMASK_PCR_TRANS_2  	0x00001000
#define PID_BITMASK_VIDEO_TRANS_2 	0x00002000
#define PID_BITMASK_AUDIO_TRANS_2 	0x00004000
#define PID_BITMASK_AUDIO_SUB_TRANS_2 0x00008000

/* add for Fast channel change */
#define PID_BITMASK_VIDEO_PRIMER 0x00008000
#define PID_BITMASK_AUDIO_PRIMER 0x00010000
#define PID_BITMASK_PCR_PRIMER 0x00020000

typedef enum
{
	DI_DEMUX_CAP_LIVE= 0x00000001, 						/**< demux capability : support live decoding  */
	DI_DEMUX_CAP_PLAYBACK = 0x00000004,					/**< demux capability : support playback  */
	DI_DEMUX_CAP_SECTIONFILTER = 0x00000008, 			/**< demux capability : support section filting */
	DI_DEMUX_CAP_DUPLICATION = 0x00000010,				/**< demux capability : support duplicate pid channel \n example send ES to decoder and hdd with one pid channel */
	DI_DEMUX_CAP_PESFILTER= 0x00000020, 				/**< demux capability : support PES filtering for Teletext or Subtitle etc */
	DI_DEMUX_CAP_DESCRAMBLE = 0x00000040,				/**< demux capability : support Descrambe with it's own descrambler  */
	DI_DEMUX_CAP_DESCRAMBLE_CI= 0x00000080,				/**< demux capability : support Descrambe with CI  */
	DI_DEMUX_CAP_CI_ROUTING	 = 0x00000100, 				/**< demux capability : support CI routing by using remux*/
	DI_DEMUX_CAP_NOTIFY_SCRAMBLE_TS = 0x00000200, 		/**< demux capability : support Scramble or clean TS notification  */
	DI_DEMUX_CAP_RECORD = 0x00000400,					/**< demux capability : support Recording */
	DI_DEMUX_CAP_EXTERNAL = 0x00000800					/** < demux capability : support external input or output or both. support only output in current */
}DI_DEMUX_CAP;



/**
@brief Demux's cababliity
*/
typedef struct DI_DEMUX_capability
{
	HUINT32 ulCapability;					/**< logical demux Capability \n example DI_DEMUX_CAP_PESFILTER | DI_DEMUX_CAP_LIVE */
	HUINT32 NumOfPESFilter;					/**< the number of PES filter */
	HUINT32 NumOfSectoinFilter;				/**< the number of Section filter */
	HUINT32 NumOfSectoinFilterPerPid;		/**< the number of Section filter per a PID */
	HUINT32 NumOfMaximumFilterLength;
	HUINT32 NumOfPrimer;					/** < the number of Primer index */
	HUINT32 NumOfAvailableStcChannel;		/** < the number of Available(remaining) STC Channel */
} DI_DEMUX_CAPABILITY;

/**
@brief Demux's total cababliity
*/
typedef struct DI_DEMUX_totalcapability
{
	HUINT32 NumOfDemux;
	HUINT32 NumOfTotalSectoinFilter;		/**< the number of Section filter for all demux ids. */
} DI_DEMUX_TOTALCAPABILITY;

/**
@brief Demux's Input device types
*/
typedef enum DI_DEMUX_InputDeviceType
{
	DI_DEMUX_INPUT_NONE = 0,    			/**< Input device type : Not connected */
	DI_DEMUX_INPUT_CURRENT ,    			/**< Input device type : Use previsous setting*/
	DI_DEMUX_INPUT_TUNER,					/**< Input device type : tuner */
	DI_DEMUX_INPUT_RECORDER,				/**< Input device type : recorder: play file that is currently recording. */
	DI_DEMUX_INPUT_SCRAMBLED_FILE_RECORDER, /**< Input device type : play scrambled file that is currently recording. */
	DI_DEMUX_INPUT_PLAYER,					/**< Input device type : player: play file in storage media. */
	DI_DEMUX_INPUT_SCRAMBLED_FILE_PLAYER,	/**< Input device type : play scrambled file */
	DI_DEMUX_INPUT_MEDIA_PLAYER,			/**< Input device type : used to set demux path for media module.>*/
	DI_DEMUX_INPUT_EXTERNAL,				/**< Input device type : external input */
	DI_DEMUX_INPUT_MAX						/**< Input device type : wrong Input type*/
}DI_DEMUX_INPUTDEVICETYPE;


/**
@brief Demux's Output device types
*/
typedef enum DI_DEMUX_OutputDeviceType
{
	DI_DEMUX_OUTPUT_NONE = 0,				/**< Output device type : Not specefic - initial value */
	DI_DEMUX_OUTPUT_CURRENT ,    			/**< Output device type : Use previsous setting*/
	DI_DEMUX_OUTPUT_RECORDER,				/**< Output device type : recorder */
	DI_DEMUX_OUTPUT_DECODER,				/**< Output device type : decoder */
	DI_DEMUX_OUTPUT_EXTERNAL,				/**< Output device type : output stream without modification.*/
	DI_DEMUX_OUTPUT_TRANSCODER,				/**< Output device type : Transcoder.*/
	DI_DEMUX_OUTPUT_MAX						/**< Output device type : Wrong Output type */
}DI_DEMUX_OUTPUTDEVICETYPE;


/**
@brief Pid channel type
*/
typedef enum DI_DEMUX_channelType
{
	DI_DEMUX_CHANNELTYPE_VIDEO = 0,
	DI_DEMUX_CHANNELTYPE_AUDIO,
	DI_DEMUX_CHANNELTYPE_AUDIO_MAIN = 1,
	DI_DEMUX_CHANNELTYPE_AUDIO_DESCRIPTION,
	DI_DEMUX_CHANNELTYPE_AUDIO_SUB = 2,
	DI_DEMUX_CHANNELTYPE_AUDIO_SPDIF,
	DI_DEMUX_CHANNELTYPE_VIDEO_PRIMER,
	DI_DEMUX_CHANNELTYPE_AUDIO_PRIMER,
	DI_DEMUX_CHANNELTYPE_PCR_PRIMER,
	DI_DEMUX_CHANNELTYPE_PCR,
	DI_DEMUX_CHANNELTYPE_RECORD,
	DI_DEMUX_CHANNELTYPE_SECTION,
	DI_DEMUX_CHANNELTYPE_PES,
	DI_DEMUX_CHANNELTYPE_ALLPASS,
	DI_DEMUX_CHANNELTYPE_PRIVATE, /*Channel created by demux internally (shall not be used for channel requested by above layers explicitly.)*/
	DI_DEMUX_CHANNELTYPE_REMUX,
/* add for PIP*/
	DI_DEMUX_CHANNELTYPE_VIDEO_SUB,
	DI_DEMUX_CHANNELTYPE_PCR_SUB,
/* add for transcode*/
	DI_DEMUX_CHANNELTYPE_VIDEO_TRANS_1,
	DI_DEMUX_CHANNELTYPE_AUDIO_TRANS_1,
	DI_DEMUX_CHANNELTYPE_AUDIO_SUB_TRANS_1,
	DI_DEMUX_CHANNELTYPE_PCR_TRANS_1,
	DI_DEMUX_CHANNELTYPE_VIDEO_TRANS_2,
	DI_DEMUX_CHANNELTYPE_AUDIO_TRANS_2,
	DI_DEMUX_CHANNELTYPE_AUDIO_SUB_TRANS_2,
	DI_DEMUX_CHANNELTYPE_PCR_TRANS_2,
	DI_DEMUX_CHANNELTYPE_MAX
} DI_DEMUX_CHANNELTYPE;


/**
@brief PES Types
*/
typedef enum DI_DEMUX_PESTYPE
{
	DI_DEMUX_PESTYPE_SUBTITLE = 0,
	DI_DEMUX_PESTYPE_TELETEXT,
	DI_DEMUX_PESTYPE_USERDEFINE,
	DI_DEMUX_PESTYPE_MAX
} DI_DEMUX_PESTYPE;


typedef enum
{
	eDI_DMX_TS_Scramble,
	eDI_DMX_TS_Clean,
	eDI_DMX_TS_UnKnown
}DI_DEMUX_TsState_t;

/**
@brief Descrambler Types
*/
typedef enum {
           DI_DESC_DVB,
           DI_DESC_AES, // for ci +
           DI_DESC_DES  // for ci +
} DESC_TYPE;

typedef enum {
           DI_DESC_ODD,
           DI_DESC_EVEN
} DESC_ODD_EVEN;

typedef struct PrimerPID_INFO
{
	HUINT32 ulPrimerIndex;
	HUINT32 ulPidBitmask;
	HUINT16 usVideoPid;
	HUINT16 usAudioPid;
	HUINT16 usPcrPid;
}PrimerPid_Info;

typedef struct PID_INFO
{
	HUINT32 pidBitmask;
	HUINT16 usPCRPid;
	HUINT16 usVideoPid;
	HUINT16 usAudioMainPid;
	HUINT16 usAudioSubPid;
	HUINT16 usAudioSpdifPid;
}Pid_Info;

typedef DI_ERR_CODE	(*pfnDI_DEMUX_PES_Notify)(HUINT32 ulDemuxId, HUINT8 *pucBuf,  HUINT32 ulRequestID);
typedef DI_ERR_CODE	(*pfnDI_DEMUX_Notify)(HUINT32 ulDemuxId, HUINT8 *pucBuf,  HUINT32 ulRequestId);
typedef DI_ERR_CODE	(*pfnDI_DEMUX_AllocBuffer)(HUINT32 usBufLen, HUINT8 **ppucBuf);
typedef DI_ERR_CODE	(*pfnDI_DEMUX_FreeBufferForError)( HUINT8 *ppucBuf); /*modify001*/
typedef DI_ERR_CODE	(*pfnDI_DEMUX_NoInputData)(HUINT32 ulDemuxId);
typedef void (*pfnDI_DEMUX_TSStateNotify)(HUINT32 ulDemuxId, HUINT16 ulPid,  DI_DEMUX_TsState_t etState);


/********************************************************************************************/
/********************************************************************************************
						FOR MW LAYER FUNCTION
********************************************************************************************/
/********************************************************************************************/

/**
@ingroup Demux
Return the number of logical demux number.
@param[out] HUINT32 * ulNumOfDemux

@retval DI_ERR_OK
@retval DI_ERR_ERROR - pstCapability is NULL
*/
DI_ERR_CODE DI_DEMUX_GetNumOfDemux(HUINT32 * ulNumOfDemux);

/**
@ingroup Demux
This function returns demux's capability.
@param[in] HUINT32 ulDemuxID - demux id
@param[out] DI_DEMUX_TOTALCAPABILITY * pstCapability

@retval DI_ERR_OK
@retval DI_ERR_ERROR - pstCapability is NULL
*/
DI_ERR_CODE DI_DEMUX_GetTotalCapability(DI_DEMUX_TOTALCAPABILITY * pstTotalCapability );

/**
@ingroup Demux
This function returns demux's capability.
@param[in] HUINT32 ulDemuxID - demux id
@param[out] DI_DEMUX_CAPABILITY * pstCapability

@retval DI_ERR_OK
@retval DI_ERR_ERROR - pstCapability is NULL
*/
DI_ERR_CODE DI_DEMUX_GetCapability(HUINT32 ulDemuxID, DI_DEMUX_CAPABILITY * pstCapability );


/**
@ingroup Demux
Set Path Information
@param[in] HUINT32 ulDemuxID - demux id
@param[in] DI_DEMUX_INPUTDEVICETYPE * etProducer
@param[in] HUINT32 * ulInputDevice_id
@param[in] DI_DEMUX_OUTPUTDEVICETYPE * etConsumer
@param[in] HUINT32  * ulConsumerGroupId
@param[in] BOOL * IsCIRouting

@retval DI_ERR_OK
@retval DI_ERR_ERROR - pstCapability is NULL
*/
DI_ERR_CODE DI_DEMUX_SetPath(HUINT32 ulDemuxId,
												DI_DEMUX_INPUTDEVICETYPE etProducer, HUINT32 ulInputDevice_id,
												DI_DEMUX_OUTPUTDEVICETYPE etConsumer, HUINT32 ulConsumerGroupId,
												HINT32 nCI_Id);

/**
@ingroup Demux
Get Path Information
@param[in] HUINT32 ulDemuxID - demux id
@param[out] DI_DEMUX_INPUTDEVICETYPE * etProducer
@param[out] HUINT32 * ulInputDevice_id
@param[out] DI_DEMUX_OUTPUTDEVICETYPE * etConsumer
@param[out] HUINT32  * ulConsumerGroupId
@param[out] BOOL * IsCIRouting

@retval DI_ERR_OK
@retval DI_ERR_ERROR - pstCapability is NULL
*/
DI_ERR_CODE DI_DEMUX_GetPath(HUINT32 ulDemuxId ,
												DI_DEMUX_INPUTDEVICETYPE * etProducer, HUINT32 * ulInputDevice_id,
												DI_DEMUX_OUTPUTDEVICETYPE * etConsumer, HUINT32  * ulConsumerGroupId,
												HINT32 * nCI_Id);

/**
@ingroup Demux
This function returns demux's capability.
@param[in] HUINT32 ulDemuxID - demux id
@param[out] DI_DEMUX_CAPABILITY * pstCapability

@retval DI_ERR_OK
@retval DI_ERR_ERROR - pstCapability is NULL
*/
DI_ERR_CODE DI_DEMUX_ReleasePath(HUINT32 ulDemuxId);


/**
@ingroup Demux
Start Audio, Video, PCR filtering.
@param[in] HUINT32 ulDemuxID - demux id
@param[in] Pid_Info *ppidInfo - pid info. structure

@retval DI_ERR_OK
@retval DI_ERR_ERROR - pstCapability is NULL
*/
DI_ERR_CODE DI_DEMUX_StartPID(HUINT32 ulDemuxId, Pid_Info *ppidInfo);


/**
@ingroup Demux
Stop Audio, Video, PCR filtering.
@param[in] HUINT32 ulDemuxID - demux id
@param[in] HUINT32 pidBitmask  - pid bitmask info.

@retval DI_ERR_OK
@retval DI_ERR_ERROR - pstCapability is NULL
*/
DI_ERR_CODE DI_DEMUX_StopPID(HUINT32 ulDemuxId, HUINT32 pidBitmask);


/**
@ingroup Demux
Register callback function for notify, allocate, free buffer. .
@param[in] HUINT32 ulDemuxID - demux id
@param[in] pfnDI_DEMUX_PES_Notify pfnNotify
@param[in] pfnDI_DEMUX_AllocBuffer pfnAllocBuffer
@param[in] pfnDI_DEMUX_FreeBufferForError	pfnErrorFreeBuffer

@retval DI_ERR_OK
@retval DI_ERR_ERROR - pstCapability is NULL
*/
DI_ERR_CODE DI_DEMUX_RegisterPESCallback (HUINT32 ulDemuxId,
											DI_DEMUX_PESTYPE etPesType,
											pfnDI_DEMUX_PES_Notify pfnNotify,
											pfnDI_DEMUX_AllocBuffer pfnAllocBuffer,
											pfnDI_DEMUX_FreeBufferForError pfnErrorFreeBuffer);

/**
@ingroup Demux
Start PES  filtering for subtitle or teletext..
@param[in] HUINT32 ulDemuxID - demux id
@param[in] HUINT16 ulPid
@param[in] DI_DEMUX_PESTYPE etPESType
@param[in] HUINT32 ulRequestID        - key

@retval DI_ERR_OK
@retval DI_ERR_ERROR - pstCapability is NULL
*/
DI_ERR_CODE DI_DEMUX_StartPESFilter (HUINT32 ulDemuxId,HUINT16 ulPid, DI_DEMUX_PESTYPE etPESType, HUINT32 ulRequestID);

/**
@ingroup Demux
Stop PES  filtering for subtitle or teletext..
@param[in] HUINT32 ulDemuxID - demux id
@param[in] HUINT32 ulRequestID

@retval DI_ERR_OK
@retval DI_ERR_ERROR - pstCapability is NULL
*/
DI_ERR_CODE DI_DEMUX_StopPESFilter (HUINT32 ulDemuxId, HUINT32 ulRequestID);



/**
@ingroup Demux
Register Callback function for Allocate buffer and notify the section data arrival.
@param[in] HUINT32 ulDemuxID - demux id
@param[in] pfnDI_DEMUX_Notify	pfnNotify
@param[in] pfnDI_DEMUX_AllocBuffer	pfnAllocBuffer

@retval DI_ERR_OK
@retval DI_ERR_ERROR - pstCapability is NULL
*/
DI_ERR_CODE DI_DEMUX_RegisterSectionCallback (HUINT32 ulDemuxId,pfnDI_DEMUX_Notify	pfnNotify,pfnDI_DEMUX_AllocBuffer	pfnAllocBuffer);
/**
@ingroup Demux
Allocate, set the section filter. \n
Douglas chip does not support multibyte negative filtering. It Only support 1 byte negative section filtering\n
If you set bIsOnce TRUE, you do not call DI_DEMUX_ReleaseSectionFilter function. If you call it, It makes error. \n

@param[in] HUINT32 ulDemuxId
@param[in] HUINT16 usPid
@param[in] HUINT32 ulRequestID   - Key
@param[in] HBOOL bIsOnce
@param[in] HUINT32 ulFilterLen
@param[in] HUINT8 * pucValue
@param[in] HUINT8 * pucMask
@param[in] HUINT8 * pucExclusion
@param[in] HBOOL bCheckCRC

@retval DI_ERR_OK
@retval DI_ERR_ERROR - pstCapability is NULL
*/
DI_ERR_CODE DI_DEMUX_SetSectionFilter (HUINT32 ulDemuxId,HUINT16 usPid,HUINT32 ulRequestID, HBOOL bIsOnce,HUINT32 ulFilterLen,HUINT8 * pucValue,HUINT8 * pucMask, HUINT8 * pucExclusion,HBOOL bCheckCRC);

/**
@ingroup Demux
Release Section Filter. \n
If you set bIsOnce TRUE in DI_DEMUX_SetSectionFilter, you do not call this function.

@param[in] HUINT32 ulRequestID

@retval DI_ERR_OK
@retval DI_ERR_ERROR - pstCapability is NULL
*/
DI_ERR_CODE DI_DEMUX_ReleaseSectionFilter (HUINT32 ulDemuxId, HUINT32	ulRequestID);

/**
@ingroup Demux
Get STC Value.\n
The real STC value is 33bit, but in this function it returns PCR by 32bit. MSB bit was omitted.
@param[in] HUINT32 ulDemuxId
@param[out] HUINT32 *pulHighBitSTC
@param[out] HUINT32 *pulLowBitSTC

@retval DI_ERR_OK  - always return this. So need not check this return value.
*/
DI_ERR_CODE DI_DEMUX_GetSTC (HUINT32 ulDemuxId,HUINT32 *pulHighBitSTC, HUINT32 *pulLowBitSTC);

/**
@ingroup Demux
Start monitoring TSC(Transport Scrambling Control bit).\n
@param[in] HUINT32 ulDemuxId
@param[in] HUINT16 usPid
@param[out] HUINT32 *pulHandle

@retval DI_ERR_OK  - success
@retval DI_ERR_INVALID_PARAM - pulHandle is NULL
@retval DI_ERR_ERROR - No matching PID-channel found
*/
DI_ERR_CODE DI_DEMUX_StartTSCMonitor(HUINT32 ulDemuxId, HUINT16 usPid, HUINT32 *pulHandle);

/**
@ingroup Demux
Stop monitoring TSC(Transport Scrambling Control bit).\n
@param[in] HUINT32 ulHandle

@retval DI_ERR_OK  - always success
*/
DI_ERR_CODE DI_DEMUX_StopTSCMonitor(HUINT32 ulHandle);

/**
@ingroup Demux
Get current TSC(Transport Scrambling Control bit) info.\n
@param[in] HUINT32 ulHandle
@param[in] DI_DEMUX_TsState_t *peTsState

@retval DI_ERR_OK  - success
@retval DI_ERR_ERROR - Not opened or not supported.
*/
DI_ERR_CODE DI_DEMUX_GetTSCStatus(HUINT32 ulHandle, DI_DEMUX_TsState_t *peTsState);

/**
@ingroup Demux
Specify the tuner delivery type in a tuner group to bind demod output to a specific TS input (input band).\n
@param[in] HUINT32 ulTunerGroupId
@param[in] DI_CH_Type_e DeliveryType

@retval DI_ERR_OK  - success
@retval DI_ERR_ERROR - Not opened or not supported.
*/
DI_ERR_CODE DI_DEMUX_BindChannel(HUINT32 ulTunerGroupId, DI_CH_Type_e DeliveryType);

DI_ERR_CODE DI_DEMUX_RegisterInbandStatusCheckCallback (pfnDI_DEMUX_NoInputData pfnNoInputData);

DI_ERR_CODE DI_DEMUX_ReplacePacketModeOn(HUINT32 ulDemuxId, HUINT16 usVirtualPid, HUINT16 usPid, HUINT8 *pucSection, HUINT32 ulSectionSize, HUINT32 *pulHandle);
DI_ERR_CODE DI_DEMUX_ReplacePacketModeOff(HUINT32 ulHandle);
DI_ERR_CODE DI_DEMUX_InsertPacketModeOn(HUINT32 ulDemuxId, HUINT16 usPid, HUINT8 *pucSection, HUINT32 ulSectionSize, HUINT32 ulInterval, HUINT32 *pulHandle);
DI_ERR_CODE DI_DEMUX_InsertPacketModeOff(HUINT32 ulHandle);
DI_ERR_CODE DI_DEMUX_AddTsOutPid(HUINT32 ulDemuxId, HUINT16 usPid, HUINT32 *pulRequestId);
DI_ERR_CODE DI_DEMUX_StartTsOut(HUINT32 ulDemuxId);
DI_ERR_CODE DI_DEMUX_StopTsOut(void);

DI_ERR_CODE DI_DEMUX_GetInfo(HUINT32 demux_id,HUINT16 *videoPid, HUINT16 *audioPid);
DI_ERR_CODE DI_DEMUX_GetPlaybackId( HUINT32 ulDemuxId ,HUINT32 *pPlayBackId);
DI_ERR_CODE DI_DEMUX_ChangeSTCRate(HUINT32 unDeviceId, unsigned int unIncrement, unsigned int unPrescale);
DI_ERR_CODE DI_DEMUX_StartPrimerPID(HUINT32 ulDemuxId, PrimerPid_Info *pPrimerPidInfo);
DI_ERR_CODE DI_DEMUX_StopPrimerPID(HUINT32 ulDemuxId, PrimerPid_Info *pPrimerPidInfo);
DI_ERR_CODE DI_DEMUX_GetPrimerSTC (HUINT32 *pulHighBitSTC, HUINT32 *pulLowBitSTC);
DI_ERR_CODE DI_DEMUX_SetInputSource(HUINT32 ulDemuxId, DI_DEMUX_INPUTDEVICETYPE etProducer, HUINT32 ulInputDevice_id);
#endif
/*@}*/

