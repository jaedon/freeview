/**
	@file    	pal_pb.c
	@brief   	adaptation for player

	Description:	implements playback.	\n
	Module: 		playback			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <vkernel.h>

#include <di_err.h>
#include <di_pvr.h>
#include <di_demux.h>
#include <di_osd.h>

#include <pal_pb.h>
#include <pal_video.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



#define PalPb_ENTER
#define PalPb_EXIT
//#define	FOR_TEMP_DI_IMPLEMENT

#define palPb_MAX_IGNORED_REC					4

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	unsigned long			 ullSemId;				// 추후 문제되면 Semaphore를 사용하도록 한다.

	PalPb_Callback_t		 pfCallback;			// M/W로 올리기 위한 Callback
} palPb_Context_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HUINT32				 s_ulPvrNumPb = 0;
STATIC palPb_Context_t		*s_pstPalPb_Context = NULL;

/* Transcoding 때문에 추가. 추후 삭제 절대 필요 */
STATIC HUINT32				 s_ulPalPb_NumIgnorePb = 0;
STATIC HUINT32				 s_aulPalPb_IgnoredPbId[palPb_MAX_IGNORED_REC];

/* for thumbnail: thumbnail 생성시마다 framebuffer를 얻어오면 메모리 fragment가 생기는지 나중에 memory leak 에러 발생  */
/* 한번 만들어놓고 계속 사용하는 것으로.. */
STATIC DI_OSD_HANDLE 		 s_hThumbOsd = (DI_OSD_HANDLE)NULL;
STATIC HUINT8				*s_pucThumbFrameBuffer = NULL;
STATIC HUINT32 				 s_ulThumbWidth = 0, s_ulThumbHeight = 0;

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
STATIC palPb_Context_t *pal_pb_GetContext (HUINT32 ulDeviceId)
{
	if ((ulDeviceId < s_ulPvrNumPb) && (NULL != s_pstPalPb_Context))
	{
		return &(s_pstPalPb_Context[ulDeviceId]);
	}

	return NULL;
}

STATIC void		pal_pb_HandleEvent(void *pMsg)
{
#ifdef FOR_TEMP_DI_IMPLEMENT // disabled for di integration

#else
	HUINT32				 ulCnt;
	PalFs_Msg_t  		*pstDiMsg = NULL;
	palPb_Context_t		*pstContext = NULL;

	pstDiMsg = (PalFs_Msg_t *)pMsg;
	if( pstDiMsg == NULL )
	{
		HxLOG_Print("[pal_pb_HandleEvent] PAL_PVR_MSG is NULL\n");
		return;
	}

	for (ulCnt = 0; ulCnt < s_ulPalPb_NumIgnorePb; ulCnt++)
	{
		if (pstDiMsg->ulInstanceId == s_aulPalPb_IgnoredPbId[ulCnt])
		{
			return;
		}
	}

	pstContext = pal_pb_GetContext (pstDiMsg->ulInstanceId);
	if ((NULL == pstContext) || (NULL == pstContext->pfCallback))
	{
		HxLOG_Error ("No Context or No Callback\n");
		return;
	}

//	HxLOG_Print(" ulEvent:0x%X, ulInstance:%d, ulParam1:%d\n", pstDiMsg->ulEvent, pstDiMsg->ulInstanceId, pstDiMsg->ulParam1);

	switch( pstDiMsg->ulEvent )
	{
		case DI_PVR_EVENT_TIMECODE_CHANGED:
			HxLOG_Info("DI_PVR_EVENT_TIMECODE_CHANGED : ulInstanceId(%d), ulParam1(%d), ulParam2(%d)\r\n", pstDiMsg->ulInstanceId, pstDiMsg->ulParam1, pstDiMsg->ulParam2);
			pstContext->pfCallback ( ePAL_PB_EVENT_TIMECODE_CHANGED, pstDiMsg->ulInstanceId, pstDiMsg->ulParam1, pstDiMsg->ulParam2, pstDiMsg->ulParam3);
			break;

		case DI_PVR_EVENT_EOF:
			HxLOG_Info("DI_PVR_EVENT_EOF : ulInstanceId(%d)\r\n", pstDiMsg->ulInstanceId);
			pstContext->pfCallback ( ePAL_PB_EVENT_EOF, pstDiMsg->ulInstanceId, pstDiMsg->ulParam1, pstDiMsg->ulParam2, pstDiMsg->ulParam3 );
			break;

		case DI_PVR_EVENT_BOF:
			HxLOG_Info("DI_PVR_EVENT_BOF : ulInstanceId(%d)\r\n", pstDiMsg->ulInstanceId);
			pstContext->pfCallback ( ePAL_PB_EVENT_BOF, pstDiMsg->ulInstanceId, pstDiMsg->ulParam1, pstDiMsg->ulParam2, pstDiMsg->ulParam3 );
			break;

		default:
			HxLOG_Print("[%s:%d] Unknown event : 0x%X\n", __FUNCTION__, __LINE__, pstDiMsg->ulEvent);
			break;
	}

#endif
}

#if 0	// defined but not used -> disable by #if 0
STATIC HINT32 _pal_pvr_play_convertDiSpeed2OctoSpeed (DI_MEDIA_PlaySpeed_e eDiPlaySpeed)
{
	switch (eDiPlaySpeed)
	{
	case eDI_MEDIA_Speed_RW_X128:				return -12800;
	case eDI_MEDIA_Speed_RW_X64:				return -6400;
	case eDI_MEDIA_Speed_RW_X32:				return -3200;
	case eDI_MEDIA_Speed_RW_X16:				return -1600;
	case eDI_MEDIA_Speed_RW_X8:					return -800;
	case eDI_MEDIA_Speed_RW_X4:					return -400;
	case eDI_MEDIA_Speed_RW_X2:					return -200;
	case eDI_MEDIA_Speed_RW_X1:					return -100;
	case eDI_MEDIA_Speed_RW_X_1_2:				return -50;
	case eDI_MEDIA_Speed_RW_X_1_4:				return -25;
	case eDI_MEDIA_Speed_RW_X_1_8:				return -12;
	case eDI_MEDIA_Speed_RW_X_1_10:				return -10;
	case eDI_MEDIA_Speed_Pause:					return 0;
	case eDI_MEDIA_Speed_FF_X_1_10:				return 10;
	case eDI_MEDIA_Speed_FF_X_1_8:				return 12;
	case eDI_MEDIA_Speed_FF_X_1_4:				return 25;
	case eDI_MEDIA_Speed_FF_X_1_2:				return 50;
	case eDI_MEDIA_Speed_Normal:				return 100;
	case eDI_MEDIA_Speed_FF_X2:					return 200;
	case eDI_MEDIA_Speed_FF_X4:					return 400;
	case eDI_MEDIA_Speed_FF_X8:					return 800;
	case eDI_MEDIA_Speed_FF_X16:				return 1600;
	case eDI_MEDIA_Speed_FF_X32:				return 3200;
	case eDI_MEDIA_Speed_FF_X64:				return 6400;
	case eDI_MEDIA_Speed_FF_X128:				return 12800;
	default:
		break;
	}

	return 0;
}
#endif // #if 0

STATIC DI_MEDIA_PlaySpeed_e pal_pb_ConvertSpeed2Di (HINT32 nPlaySpeed)
{
	if ((nPlaySpeed >= -5) && (nPlaySpeed < 5))
	{
		return eDI_MEDIA_Speed_Pause;
	}

	if (nPlaySpeed >= 5)
	{
		if (nPlaySpeed < 75)
		{
			if (nPlaySpeed < 11)						{ return eDI_MEDIA_Speed_FF_X_1_10; }
			if (nPlaySpeed < 18)						{ return eDI_MEDIA_Speed_FF_X_1_8; }
			if (nPlaySpeed < 37)						{ return eDI_MEDIA_Speed_FF_X_1_4; }
			//if (nPlaySpeed < 75)						{ return eDI_MEDIA_Speed_FF_X_1_2; }
			return eDI_MEDIA_Speed_FF_X_1_2;
		}
		else
		{
			if (nPlaySpeed < 150)						{ return eDI_MEDIA_Speed_Normal; }
			if (nPlaySpeed < 300)						{ return eDI_MEDIA_Speed_FF_X2; }
			if (nPlaySpeed < 600)						{ return eDI_MEDIA_Speed_FF_X4; }
			if (nPlaySpeed < 1200)						{ return eDI_MEDIA_Speed_FF_X8; }
			if (nPlaySpeed < 2400)						{ return eDI_MEDIA_Speed_FF_X16; }
			if (nPlaySpeed < 4800)						{ return eDI_MEDIA_Speed_FF_X32; }
			if (nPlaySpeed < 9600)						{ return eDI_MEDIA_Speed_FF_X64; }
			if (nPlaySpeed < 19200)						{ return eDI_MEDIA_Speed_FF_X128; }
			//if (nPlaySpeed < 38400)						{ return eDI_MEDIA_Speed_FF_X256; }
			return eDI_MEDIA_Speed_FF_X256;
		}
	}
	else if (nPlaySpeed < -5)
	{
		if (nPlaySpeed < -75)
		{
			if (nPlaySpeed < -19200) 					{ return eDI_MEDIA_Speed_RW_X256; }	
			if (nPlaySpeed < -9600) 					{ return eDI_MEDIA_Speed_RW_X128; }
			if (nPlaySpeed < -4800) 					{ return eDI_MEDIA_Speed_RW_X64; }
			if (nPlaySpeed < -2400) 					{ return eDI_MEDIA_Speed_RW_X32; }
			if (nPlaySpeed < -1200) 					{ return eDI_MEDIA_Speed_RW_X16; }
			if (nPlaySpeed < -600)						{ return eDI_MEDIA_Speed_RW_X8; }
			if (nPlaySpeed < -300)						{ return eDI_MEDIA_Speed_RW_X4; }
			if (nPlaySpeed < -150)						{ return eDI_MEDIA_Speed_RW_X2; }
			//if (nPlaySpeed < -75)						{ return eDI_MEDIA_Speed_RW_X1; }
			return eDI_MEDIA_Speed_RW_X1;
		}
		else
		{
			if (nPlaySpeed < -37)						{ return eDI_MEDIA_Speed_RW_X_1_2; }
			if (nPlaySpeed < -18)						{ return eDI_MEDIA_Speed_RW_X_1_4; }
			if (nPlaySpeed < -11)						{ return eDI_MEDIA_Speed_RW_X_1_8; }
			//if (nPlaySpeed < -5)						{ return eDI_MEDIA_Speed_RW_X_1_10; }
			return eDI_MEDIA_Speed_RW_X_1_10;
		}

	}
	/*
	else			// if ((nPlaySpeed >= -5) && (nPlaySpeed < 5))
	{
		return eDI_MEDIA_Speed_Pause;
	}
	*/

	return eDI_MEDIA_Speed_Pause;
}

HUINT32			PAL_PB_Init(void)
{
#ifdef	FOR_TEMP_DI_IMPLEMENT

#else
	HUINT32					 ulRet = ERR_FAIL;
	HUINT32					 i, ulNumPlayer = 0;
	palPb_Context_t		*pstContextArray = NULL;

	PalPb_ENTER;

	/* init recorder */
	ulRet = DI_PVR_PLAY_GetCapability(&ulNumPlayer);
	if ((DI_ERR_OK != ulRet) || (0 == ulNumPlayer))
	{
		HxLOG_Error ("DI_PVR_PLAY_GetCapability err or Player=0\n");
		PalPb_EXIT;
		return ERR_FAIL;
	}

	pstContextArray = (palPb_Context_t *)OxMW_Calloc (sizeof(palPb_Context_t) * ulNumPlayer);
	if (NULL == pstContextArray)
	{
		HxLOG_Error ("Context Allocation failed\n");
		PalPb_EXIT;
		return ERR_FAIL;
	}

	s_ulPvrNumPb = ulNumPlayer;
	s_pstPalPb_Context = pstContextArray;

	for (i = 0; i < ulNumPlayer; i++)
	{
		ulRet = DI_PVR_PLAY_RegisterEventCallback( i, DI_PVR_EVENT_EOF, pal_pb_HandleEvent );
		ulRet |= DI_PVR_PLAY_RegisterEventCallback( i, DI_PVR_EVENT_BOF, pal_pb_HandleEvent );
		ulRet |= DI_PVR_PLAY_RegisterEventCallback( i, DI_PVR_EVENT_TIMECODE_CHANGED, pal_pb_HandleEvent );
		if (ulRet != DI_ERR_OK)
		{
			HxLOG_Print("[InitPlayer] fail to register recorder callback\n");
			PalPb_EXIT;
			return ulRet;
		}
	}

#endif
	return ERR_OK;
}


void				PAL_PB_RegisterNotifier(HUINT32 ulDeviceId, PalPb_Callback_t pbCallback)
{
	palPb_Context_t		*pstContext = pal_pb_GetContext(ulDeviceId);

	if ((NULL == pstContext) || (NULL == pbCallback))
	{
		HxLOG_Error ("DeviceId Error or Callback NULL\n");
		return;
	}

	pstContext->pfCallback = pbCallback;
}

void				PAL_PB_UnregisterNotifier (HUINT32 ulDeviceId, PalPb_Callback_t pbCallback)
{
	palPb_Context_t		*pstContext = pal_pb_GetContext(ulDeviceId);

	if ((NULL == pstContext) || (pbCallback != pstContext->pfCallback))
	{
		HxLOG_Error ("DeviceId Error or Callback not same\n");
		return;
	}

	pstContext->pfCallback = NULL;
}

HUINT32 			PAL_PB_GetCapability(HUINT32 *pulNumOfDeviceId)
{
	HUINT32 ulRet = ERR_OK;

#ifdef FOR_TEMP_DI_IMPLEMENT

#else
	ulRet = DI_PVR_PLAY_GetCapability( pulNumOfDeviceId );
#endif

	return ulRet;
}


HUINT32 			PAL_PB_Setup(HUINT32 ulDeviceId, PalPb_Setup_t *pstSetup )
{
	DI_PVR_PLAY_SETUP_t		 stDiSetup;
	HUINT32					 ulRet;

#ifdef FOR_TEMP_DI_IMPLEMENT

#else
	PalPb_ENTER;
	if( ulDeviceId > s_ulPvrNumPb )
	{
		HxLOG_Error ("DeviceID(%d) Error:\n");
		PalPb_EXIT;
		return ERR_FAIL;
	}

	if (NULL == pstSetup)
	{
		HxLOG_Error ("pstSetup NULL\n");
		PalPb_EXIT;
		return ERR_FAIL;
	}

	HxSTD_memset(&stDiSetup,0,sizeof(DI_PVR_PLAY_SETUP_t));

	stDiSetup.ulDemuxId = pstSetup->ulDemuxId;
	stDiSetup.eFileType = (DI_PVR_FILE_TYPE_t)pstSetup->eFileType;
	stDiSetup.eStreamType = (DI_PVR_STREAMTYPE_t)pstSetup->eStreamType;
	HxSTD_memcpy (stDiSetup.aucFileName, pstSetup->aucFileName, DI_PVR_MAX_NAME_LEN);
	stDiSetup.eStartCondition = (DI_PVR_PLAY_START_MODE)pstSetup->eStartPosType;
	stDiSetup.ulStartTime = pstSetup->ulStartTime;
	stDiSetup.bIsRecording = pstSetup->bIsRecording;
	stDiSetup.stDecryptSettings.eEncType = pstSetup->stEncryptSettings.eEncType;
	HxSTD_memcpy (stDiSetup.stDecryptSettings.ucKey, pstSetup->stEncryptSettings.aucKey, DI_PVR_CRYPTO_KEY_LEN);
	HxSTD_memcpy (stDiSetup.stDecryptSettings.ucIv, pstSetup->stEncryptSettings.aucIv, DI_PVR_CRYPTO_KEY_LEN);
	HxSTD_memcpy (stDiSetup.stDecryptSettings.ucProtectionKey, pstSetup->stEncryptSettings.aucProtectionKey, DI_PVR_CRYPTO_KEY_LEN);
	stDiSetup.ulPacketSize = PACKET_SIZE_PVR;
	stDiSetup.bStartPaused = pstSetup->bStartPaused;

	/*
	 * note :
	 		Playback Target이 USB일 경우 Flag Setting
	 */
	{
		stDiSetup.bIsUsbFlashRecord = pstSetup->bIsUsbFlashRecord; /* Set TRUE, if recording or recorded content in USB Flash */
		HxLOG_Debug ("[PAL_PB] Setup.bIsUsbFlashRecord : %d\n", stDiSetup.bIsUsbFlashRecord);
	}


	ulRet = DI_PVR_PLAY_Setup (ulDeviceId, &stDiSetup);
	PalPb_EXIT;
#endif
	return ulRet;
}


HUINT32 			PAL_PB_Start(HUINT32 ulDeviceId )
{
	HUINT32 ulRet = ERR_FAIL;
#ifdef FOR_TEMP_DI_IMPLEMENT

#else
	PalPb_ENTER;
	if( ulDeviceId > s_ulPvrNumPb )
	{
		HxLOG_Print("[Start] DeviceId Error, ulDeviceId:%d, MAX:%d\n", ulDeviceId, s_ulPvrNumPb);
		PalPb_EXIT;
		return ulRet;
	}

	ulRet = DI_PVR_PLAY_Start( ulDeviceId );
	PalPb_EXIT;
#endif
	return ulRet;
}


HUINT32 			PAL_PB_Stop(HUINT32 ulDeviceId )
{
	HUINT32 ulRet = ERR_FAIL;
#ifdef FOR_TEMP_DI_IMPLEMENT

#else
	PalPb_ENTER;
	if( ulDeviceId > s_ulPvrNumPb )
	{
		HxLOG_Print("[Stop] DeviceId Error, ulDeviceId:%d, MAX:%d\n", ulDeviceId, s_ulPvrNumPb);
		PalPb_EXIT;
		return ulRet;
	}

	ulRet = DI_PVR_PLAY_Stop( ulDeviceId );
	PalPb_EXIT;
#endif
	return ulRet;
}


HUINT32 			PAL_PB_Pause(HUINT32 ulDeviceId )
{
	HUINT32 ulRet = ERR_FAIL;
#ifdef FOR_TEMP_DI_IMPLEMENT

#else
	PalPb_ENTER;
	if( ulDeviceId > s_ulPvrNumPb )
	{
		HxLOG_Print("[Pause] DeviceId Error, ulDeviceId:%d, MAX:%d\n", ulDeviceId, s_ulPvrNumPb);
		PalPb_EXIT;
		return ulRet;
	}

	ulRet = DI_PVR_PLAY_Pause( ulDeviceId );
	PalPb_EXIT;
#endif
	return ulRet;
}


HUINT32 			PAL_PB_Resume(HUINT32 ulDeviceId )
{
	HUINT32 ulRet = ERR_FAIL;
#ifdef FOR_TEMP_DI_IMPLEMENT

#else
	PalPb_ENTER;
	if( ulDeviceId > s_ulPvrNumPb )
	{
		HxLOG_Print("[Resume] DeviceId Error, ulDeviceId:%d, MAX:%d\n", ulDeviceId, s_ulPvrNumPb);
		PalPb_EXIT;
		return ulRet;
	}

	ulRet = DI_PVR_PLAY_Resume( ulDeviceId );
	PalPb_EXIT;
#endif
	return ulRet;

}

HUINT32 			PAL_PB_FrameByFrame(HUINT32 ulDeviceId, HBOOL bForward )
{
	HUINT32 ulRet = ERR_FAIL;

	return ulRet;
}


HUINT32 			PAL_PB_SetSpeed (HUINT32 ulDeviceId, HINT32 nPlaySpeed)
{
	DI_MEDIA_PlaySpeed_e		 	ePrvPlaySpeed,  eNewPlaySpeed;
	DI_ERR_CODE					 eDiErr = DI_ERR_ERROR;

#ifdef FOR_TEMP_DI_IMPLEMENT

#else
	PalPb_ENTER;
	if( ulDeviceId > s_ulPvrNumPb )
	{
		HxLOG_Print("[SetSpeed] DeviceId Error, ulDeviceId:%d, MAX:%d\n", ulDeviceId, s_ulPvrNumPb);
		PalPb_EXIT;
		return ERR_FAIL;
	}

	eNewPlaySpeed = pal_pb_ConvertSpeed2Di (nPlaySpeed);
	eDiErr = DI_PVR_PLAY_GetSpeed( ulDeviceId, &ePrvPlaySpeed );
	if ( (eDiErr == DI_ERR_OK) && (ePrvPlaySpeed != eNewPlaySpeed) )
		eDiErr = DI_PVR_PLAY_SetSpeed (ulDeviceId, eNewPlaySpeed);
	PalPb_EXIT;
#endif
	return (DI_ERR_OK == eDiErr) ? ERR_OK : ERR_FAIL;
}


HUINT32 			PAL_PB_GetSpeed(HUINT32 ulDeviceId, PalPb_TrickSpeed_e *peSpeed )
{
	HUINT32 ulRet = ERR_FAIL;
#ifdef FOR_TEMP_DI_IMPLEMENT

#else
	PalPb_ENTER;
	if( ulDeviceId > s_ulPvrNumPb )
	{
		HxLOG_Print("[GetSpeed] DeviceId Error, ulDeviceId:%d, MAX:%d\n", ulDeviceId, s_ulPvrNumPb);
		PalPb_EXIT;
		return ulRet;
	}

	ulRet = DI_PVR_PLAY_GetSpeed( ulDeviceId, (DI_MEDIA_PlaySpeed_e *) peSpeed );
	PalPb_EXIT;
#endif
	return ulRet;
}

#define PAL_PB_EndOfSupportSpeed					0xFFFFFF
STATIC HINT32		 s_anPalPb_SupportSpeeds_Normal[] =
#if defined(CONFIG_OP_TDC)
					{
						-25600,
						-6400,
						-3200,
						-200,
						0,
						100,
						200,
						3200,
						6400,
						25600,
						PAL_PB_EndOfSupportSpeed
					};
#else	// Prism Base
					{
						-6400,
						-3200,
						-1600,
						-800,
						-400,
						-200,
						0,
						100,
						200,
						400,
						800,
						1600,
						3200,
						6400,

						PAL_PB_EndOfSupportSpeed
					};
#endif

STATIC HINT32		 s_anPalPb_SupportSpeeds_Scrambled[] =
					{
						100,			// Normal Play
						0,				// Pause

						PAL_PB_EndOfSupportSpeed
					};

HUINT32				PAL_PB_GetSupportSpeeds (HBOOL bScrambled, HUINT32 *pulSpeedNum, HINT32 **ppnSpeedArray)
{

	HUINT32			 ulCount = 0;
	HINT32			*pnSpeeds = NULL;
	HUINT32			nTotalCount = 0;

#if defined(CONFIG_MW_CAS_CONAX)
	// CONAX CAS가 적용된 모델의 경우 descramble to re-encryption PVR 모델이다.
	// playback시 SVC CAS를 이용해야 하기에 scrambled bit를 켜 둔 상태일 뿐...Contents는 clear다...
	bScrambled = FALSE;
#endif

	if(bScrambled != TRUE)
	{
		pnSpeeds = (HINT32 *)s_anPalPb_SupportSpeeds_Normal;
		nTotalCount = sizeof(s_anPalPb_SupportSpeeds_Normal)/sizeof(HINT32);
	}
	else
	{
		pnSpeeds = (HINT32 *)s_anPalPb_SupportSpeeds_Scrambled;
		nTotalCount = sizeof(s_anPalPb_SupportSpeeds_Scrambled)/sizeof(HINT32);
	}

	for (ulCount = 0; ulCount < nTotalCount; ulCount++)
	{
		if (PAL_PB_EndOfSupportSpeed == pnSpeeds[ulCount])
		{
			break;
		}
	}

	if (NULL != pulSpeedNum)
	{
		*pulSpeedNum = ulCount;
	}

	if (NULL != ppnSpeedArray)
	{
		*ppnSpeedArray = pnSpeeds;
	}

	return ERR_OK;


}

HUINT32 			PAL_PB_SetPosition(HUINT32 ulDeviceId, HUINT32 ulTime )
{
	HUINT32 ulRet = ERR_FAIL;
#ifdef FOR_TEMP_DI_IMPLEMENT

#else
	PalPb_ENTER;
	if( ulDeviceId > s_ulPvrNumPb )
	{
		HxLOG_Print("[SetPosition] DeviceId Error, ulDeviceId:%d, MAX:%d\n", ulDeviceId, s_ulPvrNumPb);
		PalPb_EXIT;
		return ulRet;
	}

	HxLOG_Print("ulDeviceId = %d, ulTime = %d\n", ulDeviceId, ulTime);
	ulRet = DI_PVR_PLAY_SetPosition( ulDeviceId, ulTime );
	PalPb_EXIT;
#endif
	return ulRet;
}


HUINT32 			PAL_PB_GetPosition(HUINT32 ulDeviceId, HUINT32 *pulTime )
{
	HUINT32 ulRet = ERR_FAIL;
#ifdef FOR_TEMP_DI_IMPLEMENT

#else
	PalPb_ENTER;
	if( ulDeviceId > s_ulPvrNumPb )
	{
		HxLOG_Print("[GetPosition] DeviceId Error, ulDeviceId:%d, MAX:%d\n", ulDeviceId, s_ulPvrNumPb);
		PalPb_EXIT;
		return ulRet;
	}

	ulRet = DI_PVR_PLAY_GetPosition( ulDeviceId, pulTime );
	PalPb_EXIT;
#endif
	return ulRet;
}


HUINT32 			PAL_PB_GetDurationByIndex(HUINT8 *pszRecordedFilePath, HULONG *pulDuration )
{
	HUINT32 ulRet = ERR_FAIL;

	PalPb_ENTER;
	ulRet = DI_PVR_PLAY_GetDurationByIndex( pszRecordedFilePath, pulDuration );
	PalPb_EXIT;

	return ulRet;
}


HUINT32 			PAL_PB_GetDuration(HUINT32 ulDeviceId, HUINT32 *pulDuration )
{
	HUINT32 ulRet = ERR_FAIL;
#ifdef FOR_TEMP_DI_IMPLEMENT

#else
	PalPb_ENTER;
	if( ulDeviceId > s_ulPvrNumPb )
	{
		HxLOG_Print("[GetDuration] DeviceId Error, ulDeviceId:%d, MAX:%d\n", ulDeviceId, s_ulPvrNumPb);
		PalPb_EXIT;
		return ulRet;
	}

	ulRet = DI_PVR_PLAY_GetDuration( ulDeviceId, pulDuration );
	PalPb_EXIT;
#endif
	return ulRet;
}

#if !defined(CONFIG_MWC_PIPE)
HUINT32		PAL_PB_SetPath( HUINT32 ulDeviceId, PalPb_InputDevice_e eInputType, HUINT32 ulDemuxId )
{
	HUINT32 ulRet = ERR_FAIL;

#ifdef FOR_TEMP_DI_IMPLEMENT

#else
	DI_DEMUX_INPUTDEVICETYPE eProducer;

	PalPb_ENTER;
	switch( eInputType )
	{
		case ePAL_PB_INPUT_DEVICE_RECORDER:
		eProducer = DI_DEMUX_INPUT_RECORDER;
			break;

		case ePAL_PB_INPUT_DEVICE_PLAYER:
			eProducer = DI_DEMUX_INPUT_PLAYER;
			break;

		case ePAL_PB_INPUT_DEVICE_DESCRAMBLE_RECORDER:
			eProducer = DI_DEMUX_INPUT_SCRAMBLED_FILE_RECORDER;
			break;

		case ePAL_PB_INPUT_DEVICE_DESCRAMBLE_PLAYER:
			eProducer = DI_DEMUX_INPUT_SCRAMBLED_FILE_PLAYER;
			break;

		default:
		eProducer = DI_DEMUX_INPUT_PLAYER;
			break;
	}

	ulRet = DI_DEMUX_SetPath( ulDemuxId, eProducer, ulDeviceId, DI_DEMUX_OUTPUT_DECODER, 0, -1 );
	PalPb_EXIT;
#endif

	return ulRet;
}

HUINT32		PAL_PB_ReleasePath( HUINT32 ulDeviceId, HUINT32 ulDemuxId )
{
	HUINT32 ulRet = ERR_FAIL;
#ifdef FOR_TEMP_DI_IMPLEMENT

#else
	PalPb_ENTER;
	if( ulDeviceId > s_ulPvrNumPb )
	{
		HxLOG_Print("[ReleasePath] DeviceId Error, ulDeviceId:%d, MAX:%d\n", ulDeviceId, s_ulPvrNumPb);
		PalPb_EXIT;
		return ulRet;
	}

	ulRet = DI_DEMUX_ReleasePath( ulDemuxId );
	PalPb_EXIT;
#endif

	return ulRet;
}
#endif


HUINT32		PAL_PB_SetDecryption( HUINT32 ulDeviceId, PalCrypt_EncryptionType_e eEncType, HUINT8 *pucKey, HUINT8 *pucIv, HUINT8 *pucProtectionKey)
{
	DI_PVR_ENCRYPTION_TYPE	 eDiEncType = DI_PVR_ENCRYPTION_TYPE_NONE;
	DI_PVR_CRYPTO_SETTINGS	 stDecryptSettings;
	HUINT32					 ulRet = ERR_FAIL;

	PalPb_ENTER;
	if( ulDeviceId > s_ulPvrNumPb )
	{
		HxLOG_Print("[SetDecryption] DeviceId Error, ulDeviceId:%d, MAX:%d\n", ulDeviceId, s_ulPvrNumPb);
		PalPb_EXIT;
		return ERR_FAIL;
	}

	HxLOG_Print("[%s] Encryption type for Play [%d]\n", __FUNCTION__, eEncType);

	switch (eEncType)
	{
	case ePAL_PVR_ENCRYPTION_TYPE_NONE:
		eDiEncType = DI_PVR_ENCRYPTION_TYPE_NONE;
		break;

	case ePAL_PVR_ENCRYPTION_TYPE_3DES:
		eDiEncType = DI_PVR_ENCRYPTION_TYPE_3DES;
		break;

	case ePAL_PVR_ENCRYPTION_TYPE_AES:
		eDiEncType = DI_PVR_ENCRYPTION_TYPE_AES;
		break;

	case ePAL_PVR_ENCRYPTION_TYPE_CURRENT:
		eDiEncType = DI_PVR_ENCRYPTION_TYPE_CURRENT;
		break;

	default:
		eDiEncType = DI_PVR_ENCRYPTION_TYPE_NONE;
		break;
	}

	stDecryptSettings.eEncType = eDiEncType;

	if (NULL == pucKey)
	{
		HxSTD_memset (stDecryptSettings.ucKey, 0x00, sizeof(HUINT8) * DI_PVR_CRYPTO_KEY_LEN);
	}
	else
	{
		HxSTD_memcpy (stDecryptSettings.ucKey, pucKey, sizeof(HUINT8) * DI_PVR_CRYPTO_KEY_LEN);
	}

	if (NULL == pucIv)
	{
		HxSTD_memset (stDecryptSettings.ucIv, 0x00, sizeof(HUINT8) * DI_PVR_CRYPTO_KEY_LEN);
	}
	else
	{
		HxSTD_memcpy (stDecryptSettings.ucIv, pucIv, sizeof(HUINT8) * DI_PVR_CRYPTO_KEY_LEN);
	}

	if (NULL == pucProtectionKey)
	{
		HxSTD_memset (stDecryptSettings.ucProtectionKey, 0x00, sizeof(HUINT8) * DI_PVR_CRYPTO_KEY_LEN);
	}
	else
	{
		HxSTD_memcpy (stDecryptSettings.ucProtectionKey, pucProtectionKey, sizeof(HUINT8) * DI_PVR_CRYPTO_KEY_LEN);
	}


	ulRet = DI_PVR_PLAY_SetDecryption (ulDeviceId, &stDecryptSettings);

	PalPb_EXIT;
	return (DI_ERR_OK == ulRet) ? ERR_OK : ERR_FAIL;
}

HERROR PAL_PB_AddIgnoringPlayback (HUINT32 ulDeviceId)
{
	if (s_ulPalPb_NumIgnorePb >= palPb_MAX_IGNORED_REC)				{ return ERR_FAIL; }

	s_aulPalPb_IgnoredPbId[s_ulPalPb_NumIgnorePb++] = ulDeviceId;
	return ERR_OK;
}

HERROR PAL_PB_RemoveIgnoringPlayback (HUINT32 ulDeviceId)
{
	HUINT32				 ulCount;

	for (ulCount = 0; ulCount < s_ulPalPb_NumIgnorePb; ulCount++)
	{
		if (s_aulPalPb_IgnoredPbId[ulCount] == ulDeviceId)
		{
			for (; ulCount < palPb_MAX_IGNORED_REC - 1; ulCount++)
			{
				s_aulPalPb_IgnoredPbId[ulCount] = s_aulPalPb_IgnoredPbId[ulCount + 1];
			}

			s_ulPalPb_NumIgnorePb --;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

/* return 값은 계속 유효하지 않으므로 받으면 바로 copy해야 한다 */
HUINT8 *PAL_PB_ExtractThumbnail(				HUINT32 ulWidth, 					/* target video window size */
													HUINT32 ulHeight, 					/* target video window size */
													PalPb_ThumbnailSetup_t *pstSetup )
{
	DI_ERR_CODE				diErr;
	DI_OSD_PIXEL_FORMAT		eDiOSDFormat = DI_OSD_PIXEL_FORMAT_ARGB_8888;
	DI_PVR_THUMBNAIL_SETUP_t	stDiSetup;
	HUINT32 						*pBuffer;

	//	HMS-1000S ST3인증 이후에, 해당 Buffer는 PVR Init시 우선 할당 받아 놓은것이 좋다.
	//	왠만하면 크게 해서 한번만 alloc되게...
	HxLOG_Debug("%s(w:%d, h:%d)\n", __FUNCTION__, ulWidth, ulHeight);

	if (ulWidth != s_ulThumbWidth || ulHeight != s_ulThumbHeight)
	{
		// 기존에 처리하던 것과 size가 달라지면 기존 buffer 모두 clear 후 다시 잡는다.
		// 같다면 기존 buffer를 계속 이용
		if (s_hThumbOsd != (DI_OSD_HANDLE)NULL)
		{
			DI_OSD_DeleteFrameBuffer(s_hThumbOsd);
			s_hThumbOsd = (DI_OSD_HANDLE)NULL;
		}

		if (s_pucThumbFrameBuffer != NULL)
		{
			OxMW_Free(s_pucThumbFrameBuffer);
			s_pucThumbFrameBuffer = NULL;
		}
	}

	if (s_hThumbOsd == (DI_OSD_HANDLE)NULL)
	{
		diErr = DI_OSD_CreateFrameBuffer(
									ulWidth,
									ulHeight,
									eDiOSDFormat,//DI_OSD_PIXEL_FORMAT format,
									&s_hThumbOsd//DI_OSD_HANDLE * phOsd
									);
		if(diErr != DI_ERR_OK)
		{
			HxLOG_Error("\tERROR from DI_OSD_CreateFrameBuffer()\n");
			return NULL;
		}
	}

	if (s_pucThumbFrameBuffer == NULL)
	{
		s_pucThumbFrameBuffer = (HUINT8 *)OxMW_Malloc(ulWidth * ulHeight * sizeof(HUINT32));
		if( s_pucThumbFrameBuffer == NULL )
		{
			return NULL;
		}
	}

	s_ulThumbWidth = ulWidth;
	s_ulThumbHeight = ulHeight;

	HxSTD_memset(&stDiSetup, 0, sizeof(DI_PVR_THUMBNAIL_SETUP_t));

	HxSTD_memcpy (stDiSetup.aucFileName, pstSetup->aucFileName, DI_PVR_MAX_NAME_LEN);
	stDiSetup.bTsr = pstSetup->bTsr;
	stDiSetup.iRecDeviceId = pstSetup->ulDecId;
	stDiSetup.ulCaptureTime = pstSetup->ulCaptureTime;
	stDiSetup.stDecryptSettings.eEncType = pstSetup->stEncryptSettings.eEncType;
	HxSTD_memcpy (stDiSetup.stDecryptSettings.ucKey, pstSetup->stEncryptSettings.aucKey, DI_PVR_CRYPTO_KEY_LEN);
	HxSTD_memcpy (stDiSetup.stDecryptSettings.ucIv, pstSetup->stEncryptSettings.aucIv, DI_PVR_CRYPTO_KEY_LEN);
	HxSTD_memcpy (stDiSetup.stDecryptSettings.ucProtectionKey, pstSetup->stEncryptSettings.aucProtectionKey, DI_PVR_CRYPTO_KEY_LEN);

	stDiSetup.ulPacketSize = PACKET_SIZE_PVR;

	stDiSetup.ulVideoPid = pstSetup->ulVideoPid;
//	stDiSetup.VideoCodec = pstSetup->VideoCodec;
	if (PAL_VIDEO_ConvertPal2Di_Codec(pstSetup->VideoCodec, (HUINT32 *)&stDiSetup.VideoCodec) != ERR_OK)
	{
		HxLOG_Error("\tERROR from PAL_VIDEO_ConvertPal2Di_Codec()\n");
		DI_OSD_DeleteFrameBuffer(s_hThumbOsd);
		s_hThumbOsd = (DI_OSD_HANDLE)NULL;
		return NULL;
	}

	diErr = DI_PVR_PLAY_GetThumbnail(&stDiSetup, (HUINT8*)&s_hThumbOsd);
	if(diErr != DI_ERR_OK)
	{
		HxLOG_Error("\tERROR from DI_PVR_PLAY_GetThumbnail()\n");
		DI_OSD_DeleteFrameBuffer(s_hThumbOsd);
		s_hThumbOsd = (DI_OSD_HANDLE)NULL;
		return NULL;
	}

	diErr = DI_OSD_GetFrameBuffer(s_hThumbOsd, (void **)&pBuffer);
	if(diErr != DI_ERR_OK)
	{
		HxLOG_Error("\tERROR from DI_OSD_GetFrameBuffer()\n");
		DI_OSD_DeleteFrameBuffer(s_hThumbOsd);
		s_hThumbOsd = (DI_OSD_HANDLE)NULL;
		return NULL;
	}

	HxSTD_memcpy(s_pucThumbFrameBuffer, pBuffer, sizeof(HUINT32) * ulWidth * ulHeight);

/* 지우지말고 재활용..
	diErr = DI_OSD_DeleteFrameBuffer(s_hThumbOsd);
	if(diErr != DI_ERR_OK)
	{
		HxLOG_Error("\tERROR from DI_OSD_DeleteFrameBuffer()\n");
		return ERR_FAIL;
	}
*/
	return s_pucThumbFrameBuffer;
}

/*********************** End of File ******************************/

