/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  	pal_pb.h
	@brief 	driver interfaces for PVR operation

	Description: 		\n
	Module: pal/pvr			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__PAL_PB_H__
#define	__PAL_PB_H__

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
	ePAL_PB_EVENT_EOF						= 0,	// Pb
	ePAL_PB_EVENT_BOF, 								// Pb
	ePAL_PB_EVENT_TIMECODE_CHANGED,					// Pb, Rec
} PalPb_Event_e;

typedef enum
{
	ePAL_PB_INPUT_DEVICE_NONE,    					/**< Input device type : Not connected */
	ePAL_PB_INPUT_DEVICE_CURRENT ,    				/**< Input device type : Use previsous setting*/
	ePAL_PB_INPUT_DEVICE_TUNER,						/**< Input device type : tuner */
	ePAL_PB_INPUT_DEVICE_RECORDER,				/**< Input device type : recorder: play file that is currently recording. */
	ePAL_PB_INPUT_DEVICE_DESCRAMBLE_RECORDER,	  	/**< Input device type : play scrambled file that is currently recording. */
	ePAL_PB_INPUT_DEVICE_PLAYER,					/**< Input device type : player: play file in storage media. */
	ePAL_PB_INPUT_DEVICE_DESCRAMBLE_PLAYER,    /**< Input device type : play scrambled file */
	ePAL_PB_INPUT_DEVICE_MAX							/**< Input device type : wrong Input type*/
} PalPb_InputDevice_e;

typedef enum
{
	/* fast backward  motion 	*/
	ePAL_PB_SPEED_FAST_BACKWARD7 	= -12, // reserved
	ePAL_PB_SPEED_FAST_BACKWARD6 	= -11,  // 64x
	ePAL_PB_SPEED_FAST_BACKWARD5 	= -10,  // 32x
	ePAL_PB_SPEED_FAST_BACKWARD4 	= -9,  // 16x
	ePAL_PB_SPEED_FAST_BACKWARD3 	= -8,	// 8x
	ePAL_PB_SPEED_FAST_BACKWARD2 	= -7,	// 4x
	ePAL_PB_SPEED_FAST_BACKWARD1 	= -6, // 2x

	/* normal backward motion 	*/
	ePAL_PB_SPEED_NORMAL_BACKWARD	= -5, // 1x

	/* slow backward motion */
	ePAL_PB_SPEED_SLOW_BACKWARD1 	= -1, // 1/2
	ePAL_PB_SPEED_SLOW_BACKWARD2 	= -2, // 1/4
	ePAL_PB_SPEED_SLOW_BACKWARD3 	= -3, // 1/8
	ePAL_PB_SPEED_SLOW_BACKWARD4 	= -4, // 1/10

	/* pause */
	ePAL_PB_SPEED_PAUSE				= 0,

	/* slow forward motion */
	ePAL_PB_SPEED_SLOW_FORWARD4 	= 4, // 1/10
	ePAL_PB_SPEED_SLOW_FORWARD3 	= 3, // 1/8
	ePAL_PB_SPEED_SLOW_FORWARD2 	= 2, // 1/4
	ePAL_PB_SPEED_SLOW_FORWARD1		= 1, // 1/2

	/* normal speed	*/
	ePAL_PB_SPEED_NORMAL			= 5, // 1x

	/* fast forward  motion 	*/
	ePAL_PB_SPEED_FAST_FORWARD1 	= 6,	// 2x
	ePAL_PB_SPEED_FAST_FORWARD2 	= 7,	// 4x
	ePAL_PB_SPEED_FAST_FORWARD3 	= 8,	// 8x
	ePAL_PB_SPEED_FAST_FORWARD4 	= 9,	// 16x
	ePAL_PB_SPEED_FAST_FORWARD5 	= 10,	// 32x
	ePAL_PB_SPEED_FAST_FORWARD6 	= 11,	// 64x
	ePAL_PB_SPEED_FAST_FORWARD7 	= 12 // reserved.
} PalPb_TrickSpeed_e;

typedef enum
{
	ePAL_PB_SEEKPOSITION_START,
	ePAL_PB_SEEKPOSITION_CURRENT,
	ePAL_PB_SEEKPOSITION_END,
	ePAL_PB_SEEKPOSITION_MAX
} PalPb_SeekPosition_e;

typedef struct
{
	HUINT32					ulDemuxId;

	PalFs_FileType_e		eFileType;		/* Linear or Circular */
	StreamType_e			eStreamType;	/* Video or Audio */
	HUINT8					aucFileName[PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH];
	PalFs_StartPosition_e	eStartPosType;
	HUINT32					ulStartTime;		/* Specify start time  */
	HBOOL					bIsRecording;	/* Specify if recording is progressing or not  */
	PalCrypt_Setting_t	stEncryptSettings;
	HBOOL					bIsUsbFlashRecord; /* Set TRUE, if recording or recorded content in USB Flash */
} PalPb_Setup_t;

typedef struct
{
	HUINT8				aucFileName[PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH];
	HUINT32				ulCaptureTime;
	PalCrypt_Setting_t	stEncryptSettings;
	HUINT32				VideoCodec;
	HUINT32				ulVideoPid;
	HBOOL				bTsr;
	HUINT32				ulDecId;
} PalPb_ThumbnailSetup_t;

/** @brief proto for callback which MW monitors playback operation */
typedef void (*PalPb_Callback_t)(HUINT32 event, HUINT32 deviceId, HUINT32 param1, HUINT32 param2, HUINT32 param3);

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

/**
  @remark	initializes playback adaptor and context.\n
  @remark	API Type : inline \n

  @return		error
*/
HUINT32			PAL_PB_Init(void);

/**
  @remark	register a playback's event callback for MW notification.\n
  @remark	API Type : inline \n

  @return		none
*/
void			PAL_PB_RegisterNotifier(
						HUINT32 ulDeviceId,
						PalPb_Callback_t pbCallback	/**< */
						);


/**
  @remark	unregister a playback's event callback for MW notification.\n
  @remark	API Type : inline \n

  @return		none
*/
void			PAL_PB_UnregisterNotifier(
						HUINT32 ulDeviceId,
						PalPb_Callback_t pbCallback	/**< */
						);

/**
Get player capability
@param[out] HUINT32 *pulNumOfDeviceId   - number of player device available
*/
HUINT32 		PAL_PB_GetCapability(HUINT32 *pulNumOfDeviceId);


/**
Setup player device
@param[in] HUINT32 ulDeviceId - player id
@param[in] PalPb_Setup_t *pstSetup   - player setup information
*/
HUINT32 		PAL_PB_Setup(HUINT32 ulDeviceId, PalPb_Setup_t *pstSetup);


/**
Start player device
@param[in] HUINT32 ulDeviceId - player id
*/
HUINT32 		PAL_PB_Start(HUINT32 ulDeviceId);


/**
Stop player device
@param[in] HUINT32 ulDeviceId - player id
*/
HUINT32 		PAL_PB_Stop(HUINT32 ulDeviceId);


/**
Pause player device
@param[in] HUINT32 ulDeviceId - player id
*/
HUINT32 		PAL_PB_Pause(HUINT32 ulDeviceId);


/**
Resume player device
@param[in] HUINT32 ulDeviceId - player id
*/
HUINT32 		PAL_PB_Resume(HUINT32 ulDeviceId);


/**
Play frame by frame in pause mode.
@param[in] HUINT32 ulDeviceId - player id
@param[in] HBOOL bForward - direction to go to frame
*/
HUINT32 		PAL_PB_FrameByFrame(HUINT32 ulDeviceId, HBOOL bForward);


/**
Set playback speed
@param[in] HUINT32 ulDeviceId - player id
@param[in] PAL_PVR_TRICKSPEED_t eSpeed - speed
*/
HUINT32 		PAL_PB_SetSpeed (HUINT32 ulDeviceId, HINT32 nPlaySpeed);


/**
Get playback speed
@param[in] HUINT32 ulDeviceId - player id
@param[out] PAL_PVR_TRICKSPEED_t *peSpeed - speed
*/
HUINT32 		PAL_PB_GetSpeed(HUINT32 ulDeviceId, PalPb_TrickSpeed_e *peSpeed);


/**
Get the supporting playback speed
@param[in] HBOOL bScrambled:
@param[out] PAL_PVR_TRICKSPEED_t *peSpeed - speed
*/
HUINT32			PAL_PB_GetSupportSpeeds (HBOOL bScrambled, HUINT32 *pulSpeedNum, HINT32 **ppnSpeedArray);

/**
Set playback time
@param[in] HUINT32 ulDeviceId - player id
@param[in] HUINT32 ulTime - time to play
*/
HUINT32 		PAL_PB_SetPosition(HUINT32 ulDeviceId, HUINT32 ulTime);


/**
Get current playing time
@param[in] HUINT32 ulDeviceId - player id
@param[out] HUINT32 *pulTime - current playing time
*/
HUINT32 		PAL_PB_GetPosition(HUINT32 ulDeviceId, HUINT32 *pulTime);


/**
Get play duration
@param[in] HUINT32 ulDeviceId - player id
@param[out] HUINT32 *pulDuration - play duration
*/
HUINT32 		PAL_PB_GetDuration(HUINT32 ulDeviceId, HUINT32 *pulDuration);


/**
Get play duration by index file
@param[in] HUINT8 *pszRecordedFilePath - file path
@param[out] HUINT32 *pulDuration - play duration
*/
HUINT32 		PAL_PB_GetDurationByIndex(HUINT8 *pszRecordedFilePath, HULONG *pulDuration);


/**
set playback path
@param[in] HUINT32 ulDeviceId - player id  in case player, record id in case recorder
@param[in] HUINT32 ulDemuxId - demux id
*/
HUINT32			PAL_PB_SetPath(HUINT32 ulDeviceId, PalPb_InputDevice_e eInputType, HUINT32 ulDemuxId);


/**
release playback path
@param[in] HUINT32 ulDeviceId - player id
@param[in] HUINT32 ulDemuxId - demux id
*/
HUINT32			PAL_PB_ReleasePath(HUINT32 ulDeviceId, HUINT32 ulDemuxId);


/**
set encrytion info
@param[in] HUINT32 ulDeviceId - player id
@param[in] PalCrypt_EncryptionType_e eEncType - encryption type
@param[in] HUINT8 pucKey - Key (16bytes)
@param[in] HUINT8 pucIv - Iv key
*/
HUINT32 		PAL_PB_SetDecryption(HUINT32 ulDeviceId, PalCrypt_EncryptionType_e eEncType, HUINT8 *pucKey, HUINT8 *pucIv, HUINT8 *pucProtectionKey);


HERROR			PAL_PB_AddIgnoringPlayback(HUINT32 ulDeviceId);


HERROR			PAL_PB_RemoveIgnoringPlayback(HUINT32 ulDeviceId);

HUINT8			*PAL_PB_ExtractThumbnail(HUINT32 ulWidth, HUINT32 ulHeight, PalPb_ThumbnailSetup_t *pstSetup);


#endif /* !__PAL_PB_H__ */
