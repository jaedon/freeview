/**************************************************************
*
*	http://www.humaxdigital.com
*
*	@data			2012/10/24
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ⓒ 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include "oapi.h"
#include "nova_tuner_object.h"
#include "nova_ringbuf.h"
#include "nova_utils.h"
#include "nova_cas.h"
#include "sapi.h"

#define SATIP_SCHEDULE_DURATION		60		// second
#define NOVA_INVALID_TUNERID		0xffffffff
#define NOVA_INVALID_ID				0xffffffff
#define NOVA_INVALID_HANDLE			0xffffffff


// PID Info for RTL HD Channel (temporally)
#define PID_PAT						0
#define PID_PMT						96
#define PID_AUDIO					259
#define PID_VIDEO					255
#define PID_ETC						8190
#define PID_INVALID					0xffffffff

#define TRANSPORT_SYNC_BYTE			0x47

/* 2016/2/2 mhkang: 우광희부장님의 Encryption 단위 크기 조언
	Encryptor를 돌릴 때 performance를 제대로 내려면 size를 2의 n 승을 사용 하는 것이 가장 좋습니다. (DMA를 사용 하기 때문)
	그런데 TS packet이 188 bytes 라서 2의 승수로 떨어지지 않죠.
	DMA는 2의 승수 단위로 Copy 동작을 진행 하고 나머지 길이만큼 2의 승수 단위로 copy 해 나가는 방식 입니다.

	아래에서 적용 하신 188 x 7 x 4 = 5264 = 4096 + 1024 + 128 + 16  으로 CPU가 4번을 DMA 명령을 내리도록 관여를 합니다.
	CPU가 관여하는 횟수를 줄이려면 188 뒤에 곱해지는 숫자가 솟수로 오면 좋은데요.
	이 조합에서 가장 CPU관여를 줄일 수 있는 숫자가 11인 것 같습니다. (엑셀로 테이블을 만들어 계산 해 본 결과 입니다.)
	188 x 11 = 2068 = 2048 + 16 + 4 으로 CPU가 3번만 DMA명령을 내리도록 관여 하게 됩니다.

	여기서 11뒤에 곱해지는 숫자가 2의 승수(1, 2, 4, 8, 16, 32)이기만 하면 CPU의 관여 횟수가 늘어나지 않습니다.
	188 x 11 x 4 = 8272 = 8192 + 64 + 16  으로 3위의 경우와 같이 3번만 명령을 내리게 됩니다.

	즉 아래의 x 7 대신 x 11을 사용 해 주시고 x 4 의 값은 2의 승수중 적당한 값을 찾아 내면 될 것 같습니다.
	일단 아래 188 x 7 x 4 대신 188 x 11 x 4 를 하시는 것이 훨씬 더 효율적인 방법이 되겠네요.
*/

// 188 x 11 x 4 : 8272 ,  188 x 11 x 8 : 16544 ,  188 x 11 x 16 : 33088, 188 x 11 x 32 : 66176
#define	ENCRYPTION_SIZE			33088
// 2016/1/12 mhkang: Re-encrypt 구현이 완료되면 아래 define은 기본으로 활성화되도록 한다.
#define SUPPORT_REENCRYPTION

/* 2016/1/18 mhkang: Debugging
Tivizen, HD+, HD+Connect, Tara Test App  Source설정할 수 없기 때문에 사내에서 사용 불가능하다(Src=1으로 고정되어 있음)
사내에서 위 App들을 사용하려면 아래 IGNORE_SOURCE_NUMBER Define을 켠다.
*/
// #define IGNORE_SOURCE_NUMBER


// same enum is defined in Homma
typedef enum _plSatipSrcType
{
	eSatipSrcType_None 			= 0,
	eSatipSrcType_LNB_ONLY		= 0x01,

	/* diseqc A~D는 spec에 명시되어 있음. sat/ip v1.2 */
	eSatipSrcType_DISEQC		= 0x00,
	eSatipSrcType_DISEQC_A		= 0x01,
	eSatipSrcType_DISEQC_B		= 0x02,
	eSatipSrcType_DISEQC_C		= 0x03,
	eSatipSrcType_DISEQC_D		= 0x04,

	/* SCD */
	eSatipSrcType_SCD			= 0x00,
	eSatipSrcType_SCD_A			= 0x01,
	eSatipSrcType_SCD_B			= 0x02,

	/* SMATV */
	eSatipSrcType_SMATV			= 0x20,

	/* MONOBLOCK LNB */
	eSatipSrcType_MONOBLOCK_LNB	= 0x30,

	eSatipSrcType_End			= 0xFF
} plSatipSrcType_e;


extern NovaspreadTTunerReleaseRequestListener g_fnReleaseRequestListener;

static void		print_pid(unsigned pid);
static void		print_pids(unsigned char* inBuffer,  int size);
static bool		is_si_pid(unsigned pid);
static bool		is_av_pid(unsigned pid);
static unsigned parse_pid(unsigned char packet[188]);
static bool		does_si_packet_exist(unsigned char* in_buffer, int size);

static HINT32	convert_source_number(DxAntInfo_t*pstAntInfo);
bool PSEUDO_DI_MI_DECRYPTOR_SEC_SessionEncrypt(unsigned int* keyHanle, unsigned char* in_buffer, unsigned char* out_buffer, unsigned Size, HUINT16 audioPID, HUINT16 videoPID);


// ========================== Implementation of TunerObject ================================

TunerObject::TunerObject(TUNER_PARAM_t aTunerParam,	 NovaspreadTTunerAllocationFinishedListener afFinishedListener)
{
	int ret;

	_sema = new sem_t;
	ret = sem_init(_sema, 0, 1);
	if (ret != 0)
	{
		HxLOG_Error("sem_init failed(errno:%d)\n", errno);
		delete[] _sema;
		_sema = NULL;
	}

	_slotid				= TO_INVALID_SLOTID;
	_tunerid			= TO_INVALID_TUNERID;
	_hringbuf_fromObama	= HANDLE_NULL;
	_hringbuf_forDRM	= HANDLE_NULL;
	_hringbuf_forRTP	= HANDLE_NULL;
	_pidflags			= MASK_PID_NONE;
	_thread				= 0;
	_src				= aTunerParam.Src;
	_tuneParam			= NULL;
	_dataAvailable		= false;
	_isSetPids			= false;
	_tsid				= NOVA_INVALID_ID;
	_keyHandle			= NOVA_INVALID_HANDLE;
#ifdef CONFIG_DEBUG
	_keyHandlerInfo		= NULL;
#endif

	_audioPID			= NOVA_INVALID_PID ;
	_videoPID			= NOVA_INVALID_PID ;

	_contextDataAvailableListener= NULL;
	_contextStateChangeListener = NULL;
	_fStateChangeListener		= NULL;

	_fFinishedListener			= afFinishedListener;
	_fReleaseRequestListener	= NULL;
	_contextAllocator			= NULL;

	setState(TS_NONE);

	if (aTunerParam.eTunerType != TT_SAT)
	{
		HxLOG_Error(HxANSI_COLOR_RED("Not supported TunerType:%d\n"), aTunerParam.eTunerType);
		return;
	}

	_tuneParam = new DxTuneParam_t;

	memset(_tuneParam, 0, sizeof(DxTuneParam_t));
	_tuneParam->eDeliType			= eDxDELIVERY_TYPE_SAT;

	// @sat params
	_tuneParam->sat.ulRfInputId		= 0;
	_tuneParam->sat.ulTunerGroupId	= NOVA_INVALID_TUNERID;
	_tuneParam->sat.deliveryType	= eDxDELIVERY_TYPE_SAT;

	// @tuningInfo
	_tuneParam->sat.tuningInfo.antuid		= 0;
	_tuneParam->sat.tuningInfo.eFecCodeRate	= aTunerParam.eFec;
	_tuneParam->sat.tuningInfo.ePilot		= aTunerParam.ePilot;
	_tuneParam->sat.tuningInfo.ePolarization= aTunerParam.ePol;
	_tuneParam->sat.tuningInfo.ePskMod		= aTunerParam.ePskMod;
	_tuneParam->sat.tuningInfo.eRollOff		= aTunerParam.eRollOff;
	_tuneParam->sat.tuningInfo.eTransSpec	= aTunerParam.eTransSpec;
	_tuneParam->sat.tuningInfo.ulFrequency	= aTunerParam.Freq;
	_tuneParam->sat.tuningInfo.ulSymbolRate = aTunerParam.SymbolRate;

	// mode & priority;
	_mode		= aTunerParam.Mode;
	_priority	= aTunerParam.Priority;

	// dvb id
	memcpy(&_dvbId, &aTunerParam.DvbId, sizeof(NovaspreadTDvbId));

	// Use Transcoder or Cryto
	_useTranscoder	= aTunerParam.UseTranscoder;
	_useCrypto		= aTunerParam.UseCrypto;

	// Set Transcoding Parameter
	memcpy(&_transcodingParam, &aTunerParam.Transcoding, sizeof(aTunerParam.Transcoding));

#ifdef TEST_UDP_SENDER
	if (_UdpSender.openUDP(TEST_IP, TEST_PORT) == false)
	{
		printf(HxANSI_COLOR_RED("_UdpSender.openUDP(%s, %d) failed(errno:%d)\n"), TEST_IP, TEST_PORT, errno);
	}
#endif
}

TunerObject::~TunerObject()
{
	void* pRet;
	NOVA_ErrCode_e ErrCode = NERR_ERROR;
	DxSchedule_t stSchedule;

	if (getState() != TS_EXIT)
	{
		ErrCode = cancelSchedule();
		if (ErrCode != NERR_OK)
		{
			HxLOG_Error(HxANSI_COLOR_RED("cancelSchedule failed(Err:%d)\n"), ErrCode);
		}
		else
		{
			memset(&stSchedule, 0, sizeof(DxSchedule_t));

			while( SAPI_GetSchedule(_slotid, &stSchedule) == ERR_OK)
			{
				HxLOG_Print("waiting...\n");
				usleep(10000); // 10 msec
			}
		}
	}

	setState(TS_EXIT);
	if (_thread != 0)
	{
		pthread_join(_thread,&pRet);
		_thread = 0;
	}

	if (_hringbuf_fromObama != HANDLE_NULL)
	{
		destroyRingbuf(_hringbuf_fromObama);
		_hringbuf_fromObama = HANDLE_NULL;
	}

	if (_hringbuf_forDRM != HANDLE_NULL)
	{
		destroyRingbuf(_hringbuf_forDRM);
		_hringbuf_forDRM = HANDLE_NULL;
	}

	if (_tuneParam != NULL)
	{
		delete[] _tuneParam;
	}

	if (_sema != NULL)
	{
		sem_destroy(_sema);
		delete[] _sema;
		_sema = NULL;
	}

#ifdef TEST_UDP_SENDER
	_UdpSender.clsoeUDP();
#endif
}

bool TunerObject::loadAntennaInfo()
{
	HxList_t		*pAntRootList = NULL;
	DxAntInfo_t 	*pAntInfo = NULL;
	HxList_t		*pAntList = NULL;
	HERROR			hErr;
	NOVA_ErrCode_e	ErrCode = NERR_ERROR;
	HINT32			SourceNumber;

	hErr = NOVA_GetAntenaList(&pAntRootList);
	if (hErr != ERR_OK)
	{
		ErrCode = NERR_TUNER_LOADANTENNA;
		HxLOG_Error(HxANSI_COLOR_RED("NOVA_GetAntenaList failed(Err:%d)\n"), ErrCode);
		return false;
	}

	pAntList = pAntRootList;
	while(pAntList)
	{
		pAntInfo = (DxAntInfo_t*)HLIB_LIST_Data(pAntList);
		if(NULL == pAntInfo)
		{
			pAntList = HLIB_LIST_Next(pAntList);
			continue;
		}

#ifndef IGNORE_SOURCE_NUMBER	// 2016/1/18 mhkang: http://svn:3000/issues/109994#note-7
		SourceNumber = convert_source_number(pAntInfo);
		if(SourceNumber != _src)
		{
			HxLOG_Print("pAnt->Name(%s) nTpSrcNum(%d) nQuerySrcNum(%d)\r\n", pAntInfo->satName, SourceNumber, _src);
			pAntList = HLIB_LIST_Next(pAntList);
			continue;
		}
#endif

		HxSTD_MemCopy(&_tuneParam->sat.antInfo , pAntInfo, sizeof(DxAntInfo_t));
		_tuneParam->sat.antennaType 	= pAntInfo->antType;
		HxSTD_memcpy(_tuneParam->sat.tuningInfo.satType, pAntInfo->satType, DxANTINFO_NAME_LEN);
		_tuneParam->sat.tuningInfo.antuid = pAntInfo->uid;

		NOVA_ReleseAntenaList(pAntRootList);

		return true;
	}

	HxLOG_Error(HxANSI_COLOR_RED("##############  pAntRootList(NULL) ##############\r\n"));

	if (pAntRootList != NULL)
	{
		NOVA_ReleseAntenaList(pAntRootList);
	}

	return false;
}

NOVA_ErrCode_e TunerObject::makeSchedule()
{
	NOVA_ErrCode_e		ErrCode = NERR_ERROR;
	HERROR				hErr = ERR_OK;
	HxDATETIME_t		stStartTime;
	DxRSV_Streaming_t	*pstDxStreaming = NULL;
	SAPI_Conflict_t 	stConfilct;
	DxSchedule_t		stSchedule;
	PID_SET_IT			it;

	HxLOG_Info("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	// load antena info
	if (loadAntennaInfo() == false)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] loadAntennaInfo failed\n"), __FUNCTION__, __LINE__);
		ErrCode = NERR_INVALIDRESULT;
		goto ErrBlock;
	}

	_hringbuf_fromObama = createRingbuf();
	if (_hringbuf_fromObama == HANDLE_NULL)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] createRingbuf failed\n"), __FUNCTION__, __LINE__);
		ErrCode = NERR_RINGBUF_CREATE;
		goto ErrBlock;
	}

	if (_useCrypto == true)
	{
		_hringbuf_forDRM = createRingbuf();
		if (_hringbuf_forDRM == HANDLE_NULL)
		{
			HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] createRingbuf(DRM) failed\n"), __FUNCTION__, __LINE__);
			ErrCode = NERR_RINGBUF_CREATE;
			goto ErrBlock;
		}

		_hringbuf_forRTP = _hringbuf_forDRM;
	}
	else
	{
		_hringbuf_forRTP = _hringbuf_fromObama;
	}

	_utSchStartTime		= HLIB_STD_GetSystemTime();
	_utSchEndTime		= _utSchStartTime + SATIP_SCHEDULE_DURATION;

	memset(&stSchedule, 0, sizeof(DxSchedule_t));
	HLIB_DATETIME_ConvertUnixTimeToDateTime(_utSchStartTime, &stStartTime);

	stSchedule.eScheduleSpec	= eDxSchedule_Spec_Base;
	stSchedule.ulSvcUid			= 0;
	stSchedule.eRsvType 		= DxRSVTYPE_NETSTREAMING_LIVE;
	stSchedule.eRsvReady		= DxRSVREADY_0_SEC;
	stSchedule.stStartTime		= stStartTime;
	stSchedule.ulDuration 		= SATIP_SCHEDULE_DURATION; // sec
	stSchedule.eRepeat			= DxRSVREPEAT_ONCE;
	stSchedule.ulRepeatDays		= 0;
	stSchedule.eStatus			= DxRSVSTATUS_WAITING;
	stSchedule.bExceptConflict	= FALSE;
	stSchedule.eEpgType			= eDxEPG_TYPE_NONE;

	stSchedule.uExtInfo.stStrm.eAllocMode = convertAllocationMode(_mode);
	stSchedule.uExtInfo.stStrm.usPriority = convertSatipPriority(_priority);


	//=====================
	pstDxStreaming = &stSchedule.uExtInfo.stStrm;
	pstDxStreaming->ulMsgId				= _hringbuf_fromObama;
	pstDxStreaming->eUpdate				= DxRsvStream_Update_None;
	pstDxStreaming->eStreamType			= DxRSVSTREAM_TYPE_SATIP_LIVE;
	pstDxStreaming->bTranscoding 		= _useTranscoder;
	pstDxStreaming->bDrm				= _useCrypto;

	if (_useTranscoder == true || _useCrypto == true)
	{
		pstDxStreaming->stTripleId.usOnId	= _dvbId.OriginalNetworkId;
		pstDxStreaming->stTripleId.usSvcId	= _dvbId.ServiceId;
		pstDxStreaming->stTripleId.usTsId	= _dvbId.TransportStreamId;
	}

	if (_useTranscoder == true)
	{
		// default transcoding parameter info : http://svn.humaxdigital.com:3000/issues/106989
		pstDxStreaming->stProfile.bInterlaceMode		= false;
		pstDxStreaming->stProfile.bUsingLiveAspectRatio	= false;
		pstDxStreaming->stProfile.bUsingLiveAudioCodec	= false;

		pstDxStreaming->stProfile.eAudioCodec			= convertAudioCodec(_transcodingParam.AudioCodec);

		pstDxStreaming->stProfile.nAudioBitrateKbps		= _transcodingParam.AudioBitrate;	// kbps

		pstDxStreaming->stProfile.eVideoAspectRatio		= eDxASPECTRATIO_16X9;
		pstDxStreaming->stProfile.eVideoCodec			= convertVideoCodec(_transcodingParam.VideoCodec);

		pstDxStreaming->stProfile.eVideoCodecLevel		= eDxVIDEO_ENC_LEVEL_30;
		pstDxStreaming->stProfile.eVideoCodecProfile	= eDxVIDEO_ENC_PROFILE_BASELINE;
		pstDxStreaming->stProfile.nVideoBitrateKbps		= _transcodingParam.VideoBitrate;	// kbps

		pstDxStreaming->stProfile.nHdVideoBitrateKbps	= 0;

		pstDxStreaming->stProfile.nFrameRate			= 50;

		pstDxStreaming->stProfile.nScreenHeight			= convertScreenHeight(_transcodingParam.VideoResolution);
		pstDxStreaming->stProfile.nScreenWidth			= convertScreenWidth(_transcodingParam.VideoResolution);
	}

	pstDxStreaming->bPcrTsPacketInsert	= TRUE;

	pstDxStreaming->nPIDsCount			= 1;
	pstDxStreaming->usPIDs[0]			= 0;	// PAT

	memcpy(&pstDxStreaming->stTuningParam, _tuneParam, sizeof(DxTuneParam_t));

	pstDxStreaming->ulSvcUId		= 0;
	pstDxStreaming->ulContentUId	= 0;

	hErr = SAPI_GetAvailableSlot(&_slotid);
	if (ERR_OK != hErr)
	{
		HxLOG_Error(HxANSI_COLOR_RED("SAPI_GetAvailableSlot failed(hErr:%d)\r\n"), hErr);
		ErrCode = NERR_SAPI_GETAVAILABLESLOT;
		goto ErrBlock;
	}

	stSchedule.ulSlot  = _slotid;

	sprintf(pstDxStreaming->szAppDisplayString, "SAT>IP:%d ", _slotid);

	hErr = SAPI_MakeSchedule(_slotid, &stSchedule, &stConfilct);
	if (ERR_OK != hErr)
	{
		HxLOG_Error(HxANSI_COLOR_RED("SAPI_MakeSchedule failed(stConflict.eTyp:%d, hErr:%d)\n"), stConfilct.eType, hErr);
		ErrCode = NERR_SAPI_MAKESCHEDULE;
		goto ErrBlock;
	}

	return NERR_OK;

ErrBlock:
	return ErrCode;
}

NOVA_ErrCode_e TunerObject::cancelSchedule()
{
	NOVA_ErrCode_e  ErrCode = NERR_ERROR;
	HERROR hErr = ERR_FAIL;
	TUNER_STATE_e state;
	DxSchedule_t stSchedule;

	HxLOG_Info("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	if( _slotid == TO_INVALID_SLOTID)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] Invalid args(slotID is -1)\n"), __FUNCTION__, __LINE__);
		goto ErrBlock;
	}

	state = getState();
	if (state == TS_CANCELING || state == TS_EXIT)
	{
		HxLOG_Warning(HxANSI_COLOR_YELLOW("[%s:%d] already cancelSchedule is called(sloiid:%d)\n"),__FUNCTION__, __LINE__, _slotid);
		return NERR_OK;
	}

	setState(TS_CANCELING);

	hErr = SAPI_CancelSchedule(_slotid);
	if(ERR_OK != hErr)
	{
		ErrCode = NERR_SAPI_CANCELSCHEDULE;
		HxLOG_Error(HxANSI_COLOR_RED("SAPI_CancelSchedule fail(hErr:%d)\n"), hErr);
		goto ErrBlock;
	}

	memset(&stSchedule, 0, sizeof(DxSchedule_t));
	while( SAPI_GetSchedule(_slotid, &stSchedule) == ERR_OK)
	{
		HxLOG_Print("waiting...\n");
		usleep(10000); // 10 msec
	}

	setState(TS_EXIT);

	return NERR_OK;

ErrBlock:
	return ErrCode;
}


bool TunerObject::start()
{
	int state;
	bool ret;

	HxLOG_Info("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	while (getState() == TS_NONE)
	{
		printf("...\n");
		usleep(10000);
	}

	state = getState();
	if (state != TS_INACTIVE)
	{
		HxLOG_Warning("[%s:%d] state:%d\n", __FUNCTION__, __LINE__, _state);
		return false;
	}

	ret = setState(TS_ACTIVE);
	if (ret == true)
	{
		if (_fStateChangeListener != NULL)
		{
			_fStateChangeListener(_contextStateChangeListener, NOVASPREAD_TUNER_STATE_STREAMING);
		}
	}

	HxLOG_Info("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ret;
}

bool TunerObject::stop()
{
	int state;
	bool ret;

	HxLOG_Info("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	state = getState();
	if (state == TS_EXIT)
	{
		HxLOG_Warning("[%s:%d] server is already stopped.\n", __FUNCTION__, __LINE__);
		return true;
	}

	if (state != TS_ACTIVE)
	{
		HxLOG_Warning("[%s:%d] state:%d\n", __FUNCTION__, __LINE__, _state);
		return false;
	}

	ret = setState(TS_INACTIVE);
	if (ret == true)
	{
		if (_fStateChangeListener != NULL)
		{
			_fStateChangeListener(_contextStateChangeListener, NOVASPREAD_TUNER_STATE_STOPPED);
		}
	}


	HxLOG_Info("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ret;
}

NOVA_ErrCode_e TunerObject::setPids(unsigned short* aPids, int aPidCount)
{
	NOVA_ErrCode_e ErrCode = NERR_ERROR;
	PID_SET_IT it;
	int i;

	if (aPids == NULL)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] Invalid args\n"), __FUNCTION__, __LINE__);
		ErrCode = NERR_INVALIDARGS;
		goto ErrBlock;
	}

	if (_slotid == TO_INVALID_SLOTID)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] Invalid slotid(-1)\n"), __FUNCTION__, __LINE__);
		ErrCode = NERR_INVALIDARGS;
		goto ErrBlock;
	}

	if (aPidCount == _pidflags)
	{
		HxLOG_Info("[%s:%d] same setPids request(pidflag:%d).\n", __FUNCTION__, __LINE__, aPidCount);
		return NERR_OK;
	}

	sem_wait(_sema);
	if (aPidCount == MASK_PID_NONE)		// PID_NONE
	{
		_pidflags = MASK_PID_NONE;
		_isSetPids = true;
	}
	else if (aPidCount == MASK_PID_ALL)	// PID_ALL
	{
		_pidflags = MASK_PID_ALL;
		_isSetPids = true;
	}
	else								// PID_SET
	{
		_isSetPids = false;
		_pidflags = MASK_PID_SET;
		for (i = 0 ; i < aPidCount ; i++)
		{
			it = _pids.find(aPids[i]);

			if (it == _pids.end()) // When requested pids is different from _pids, updates pids.
			{
				_isSetPids = true;
				break;
			}
		}
	}

	if (_isSetPids == true)
	{
		_pids.clear();

		if (_pidflags == MASK_PID_SET)
		{
			for (i = 0 ; i < aPidCount ; i++)
			{
				_pids.insert(aPids[i]);
			}
		}
		else
		{
			_pids.insert(0);	// 2015/7/28 mhkang: When SAPI_UpdateSchedule is called, default pid(0) must be used.
		}
	}

	sem_post(_sema);

	return NERR_OK;

ErrBlock:
	return ErrCode;
}

NOVA_ErrCode_e TunerObject::updatePids()
{
	NOVA_ErrCode_e ErrCode = NERR_ERROR;
	DxSchedule_t		stSchedule;
	SAPI_Conflict_t 	stConfilct;
	HERROR hErr;
	PID_SET_IT it;
	int i;

	if (_slotid == TO_INVALID_SLOTID)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] Invalid slotid(-1)\n"), __FUNCTION__, __LINE__);
		ErrCode = NERR_INVALIDARGS;
		goto ErrBlock;
	}

	if (_isLocked == false)
	{
		HxLOG_Warning(HxANSI_COLOR_YELLOW("[%s:%d] Cannot call setPids because of unlocked signal.\n"), __FUNCTION__, __LINE__);
		ErrCode = NERR_OAPI_ISLOCKED;
		goto ErrBlock;
	}

	hErr = SAPI_GetSchedule(_slotid, &stSchedule);
	if (ERR_OK != hErr)
	{
		HxLOG_Error(HxANSI_COLOR_RED("SAPI_GetSchedule failed(hErr:%d)\n"), hErr);
		ErrCode = NERR_SAPI_GETSCHEDULE;
		goto ErrBlock;
	}

	stSchedule.uExtInfo.stStrm.eUpdate		= DxRsvStream_Update_Pids;
	stSchedule.uExtInfo.stStrm.ulMsgId		= (HUINT32)_hringbuf_fromObama;

	sem_wait(_sema);
	if (_pidflags == MASK_PID_NONE)		// PID_NONE
	{
		stSchedule.uExtInfo.stStrm.usPIDs[0] = 0;
		stSchedule.uExtInfo.stStrm.eStreamType = DxRSVSTREAM_TYPE_SATIP_LIVE;
	}
	else if (_pidflags == MASK_PID_ALL)	// PID_ALL
	{
		stSchedule.uExtInfo.stStrm.usPIDs[0] = 0;
		stSchedule.uExtInfo.stStrm.eStreamType = DxRSVSTREAM_TYPE_SATIP_LIVE_ALLPID;
	}
	else								// PID_SET
	{
		for (i =0 , it = _pids.begin() ; it != _pids.end() ; it++, i++)
		{
			stSchedule.uExtInfo.stStrm.usPIDs[i] = *it;
		}
		stSchedule.uExtInfo.stStrm.eStreamType = DxRSVSTREAM_TYPE_SATIP_LIVE;
	}
	_isSetPids = false;
	sem_post(_sema);

	stSchedule.uExtInfo.stStrm.nPIDsCount 	= _pids.size();

	hErr = SAPI_UpdateSchedule(_slotid, &stSchedule, &stConfilct, FALSE, FALSE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error(HxANSI_COLOR_RED("SAPI_UpdateSchedule failed(hErR:%d)\n"), hErr);
		ErrCode = NERR_SAPI_UPDATESCHEDULE;
		goto ErrBlock;
	}

	return NERR_OK;

ErrBlock:
	return ErrCode;
}

int TunerObject::readPackets(unsigned char* aOutBuffer, unsigned int aReqSize)
{
	HxRingBuf_Status_t stStatus;
	HERROR hErr;
	HUINT32 ulReadSize;
	HUINT32 ulDataSize;
	NOVA_ErrCode_e ErrCode = NERR_ERROR;

	if (aOutBuffer == NULL || aReqSize <= 0)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] Invalid pBuffer\n"), __FUNCTION__, __LINE__);
		ErrCode = NERR_INVALIDARGS;
		goto ErrBlock;
	}

	if (_hringbuf_forRTP == HANDLE_NULL)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] Invalid RingBuf"), __FUNCTION__, __LINE__);
		ErrCode = NERR_RINGBUF_INVALIDHANLDE;
		goto ErrBlock;
	}

	if (getState() != TS_ACTIVE)
	{
		HxLOG_Warning(HxANSI_COLOR_YELLOW("[%s:%d] cannot read packets because of no TS_ACTIVE\n"), __FUNCTION__, __LINE__);
		ErrCode = NERR_NOTALLOWED;
		goto ErrBlock;
	}

	if (_pidflags == MASK_PID_NONE)
	{
		HxLOG_Warning(HxANSI_COLOR_YELLOW("[%s:%d] Cannot read packet because of PID_NONE\n"), __FUNCTION__, __LINE__);
		ErrCode = NERR_INVALIDARGS;
		goto ErrBlock;
	}

	memset(&stStatus, 0, sizeof(stStatus));
	hErr = PL_RINGBUF_GetBufferingStatus(_hringbuf_forRTP, &stStatus);
	if(ERR_OK != hErr)
	{
		HxLOG_Error(HxANSI_COLOR_RED("PL_RINGBUF_GetBufferingStatus failed(ringbuf:%d, hErr:%d)\n"),_hringbuf_forRTP, hErr);
		ErrCode = NERR_RINGBUF_GETSTATUS;
		goto ErrBlock;
	}

	if (stStatus.ulUnreadSize == 0)
	{
		_dataAvailable = false;
		return 0;
	}
	else if (aReqSize < stStatus.ulUnreadSize)
	{
		ulReadSize = aReqSize;
	}
	else
	{
		ulReadSize = stStatus.ulUnreadSize;
	}

	hErr = ::PL_RINGBUF_ReadData(_hringbuf_forRTP, ulReadSize, (HUINT8*)aOutBuffer, &ulDataSize);

	if(ERR_OK != hErr || 0 == ulDataSize)
	{
		ErrCode = NERR_RINGBUF_READ;
		HxLOG_Error(HxANSI_COLOR_RED("Can not read ts from ringbuf(%d), ulDataSize:%d, hErr:%d\n"), _hringbuf_forRTP, ulDataSize, hErr);
		goto ErrBlock;
	}


#ifdef TEST_UDP_SENDER
	if (_UdpSender.sendAsyncPacket((char*)aOutBuffer, ulDataSize) == false)
	{
		printf(HxANSI_COLOR_RED("_UdpSender.sendPacket failed(errno:%d)\n"), errno);
	}
#endif

	return ulDataSize;

ErrBlock:

	return -1;
}

NOVA_ErrCode_e	TunerObject::isLocked(bool* aLocked)
{
	NOVA_ErrCode_e ErrCode = NERR_ERROR;

	if (aLocked == NULL)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] Invalid args(aLocked is null)\n"), __FUNCTION__, __LINE__);
		ErrCode = NERR_INVALIDARGS;
		goto ErrBlock;
	}

	sem_wait(_sema);
	*aLocked = _isLocked;
	sem_post(_sema);

	return NERR_OK;

ErrBlock:
	return ErrCode;
}

NOVA_ErrCode_e	TunerObject::getSignalLevel(int* aLevel)
{
	NOVA_ErrCode_e ErrCode = NERR_ERROR;

	if (aLevel == NULL)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] Invalid args(aLevel is null)\n"), __FUNCTION__, __LINE__);
		ErrCode = NERR_INVALIDARGS;
		goto ErrBlock;
	}

	sem_wait(_sema);
	*aLevel = _signalLevel;
	sem_post(_sema);

	return NERR_OK;

ErrBlock:
	return ErrCode;
}

NOVA_ErrCode_e	TunerObject::getSignalQuality(int* aQuality)
{
	NOVA_ErrCode_e ErrCode = NERR_ERROR;

	if (aQuality == NULL)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] Invalid args(aQuality is null)\n"), __FUNCTION__, __LINE__);
		ErrCode = NERR_INVALIDARGS;
		goto ErrBlock;
	}

	sem_wait(_sema);
	*aQuality = _signalQuality;
	sem_post(_sema);

	return NERR_OK;

ErrBlock:
	return ErrCode;
}

bool TunerObject::setDataAvailableListener(TunerDataAvailableListener afDataAvailableListener, void* aContextDataAvailableListener)
{
	_fDataAvailableListener			= afDataAvailableListener;
	_contextDataAvailableListener	= aContextDataAvailableListener;
	return true;
}



bool TunerObject::setStateChangeListener(NovaspreadTTunerStateChangeListener afStateChangeListener, void* aContextStateChangeListener)
{
	_fStateChangeListener			= afStateChangeListener;
	_contextStateChangeListener		= aContextStateChangeListener;
	return true;
}

unsigned int TunerObject::getTunerId()
{
	HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] Invalid args(aQuality is null)\n"), __FUNCTION__, __LINE__);
	return _tunerid;
}

NOVA_ErrCode_e TunerObject::setPriority(unsigned short aPriority)
{
	NOVA_ErrCode_e		ErrCode = NERR_OK;
	HERROR				hErr = ERR_OK;
	DxSchedule_t		stSchedule;
	SAPI_Conflict_t 	stConfilct;

	_priority = convertSatipPriority(aPriority);

	hErr = SAPI_GetSchedule(_slotid, &stSchedule);
	if (ERR_OK != hErr)
	{
		HxLOG_Error(HxANSI_COLOR_RED("SAPI_GetSchedule eHOM_Result_Error\n"));
		ErrCode =  NERR_SAPI_GETSCHEDULE;
		goto ErrBlock;
	}

	// 2015/9/9 mhakng: http://svn.humaxdigital.com:3000/issues/106798#note-3
	stSchedule.uExtInfo.stStrm.eUpdate	= DxRsvStream_Update_Priority;
	stSchedule.uExtInfo.stStrm.usPriority = _priority;

	hErr = SAPI_UpdateSchedule(_slotid, &stSchedule, &stConfilct, FALSE, FALSE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error(HxANSI_COLOR_RED("SAPI_UpdateSchedule failed(hErr:%d), ulSlot = %d\n"), hErr, _slotid);
		ErrCode = NERR_SAPI_UPDATESCHEDULE;
		goto ErrBlock;
	}

ErrBlock:
	return ErrCode;
}

NOVA_ErrCode_e  TunerObject::setTranscoding(NovaspreadTTranscoding* aTranscoding)
{
	NOVA_ErrCode_e		ErrCode = NERR_OK;
	HERROR				hErr = ERR_OK;
	DxSchedule_t		stSchedule;
	SAPI_Conflict_t 	stConfilct;
	DxRSV_Streaming_t*  pstDxStreaming = NULL;

#ifdef CONFIG_DEBUG
	printf(HxANSI_COLOR_YELLOW("[%s:%d]\n"), __FUNCTION__, __LINE__);
	printf(HxANSI_COLOR_YELLOW(" - Resolution  :%d\n"), aTranscoding->VideoResolution);
	printf(HxANSI_COLOR_YELLOW(" - VideoCodec  :%d\n"), aTranscoding->VideoCodec);
	printf(HxANSI_COLOR_YELLOW(" - VideoBitrate:%d\n"), aTranscoding->VideoBitrate);
	printf(HxANSI_COLOR_YELLOW(" - AudioCodec  :%d\n"), aTranscoding->AudioCodec);
	printf(HxANSI_COLOR_YELLOW(" - AudioBitrate:%d\n"), aTranscoding->AudioBitrate);
#endif

	if (_useTranscoder == false)
	{
		printf("[%s:%d] _useTranscoding is false\n", __FUNCTION__, __LINE__);
		return NERR_OK;
	}

	hErr = SAPI_GetSchedule(_slotid, &stSchedule);
	if (ERR_OK != hErr)
	{
		HxLOG_Error(HxANSI_COLOR_RED("SAPI_GetSchedule eHOM_Result_Error\n"));
		ErrCode =  NERR_SAPI_GETSCHEDULE;
		goto ErrBlock;
	}

	pstDxStreaming = &stSchedule.uExtInfo.stStrm;

	// Set the transcoding Profile(http://svn.humaxdigital.com:3000/issues/106989#note-11)
	pstDxStreaming->stProfile.bInterlaceMode		= false;
	pstDxStreaming->stProfile.bUsingLiveAspectRatio	= false;
	pstDxStreaming->stProfile.bUsingLiveAudioCodec	= false;

	pstDxStreaming->stProfile.eAudioCodec			= convertAudioCodec(aTranscoding->AudioCodec);
	pstDxStreaming->stProfile.nAudioBitrateKbps		= aTranscoding->AudioBitrate;	// kbps

	pstDxStreaming->stProfile.eVideoAspectRatio		= eDxASPECTRATIO_UNKNOWN;
	pstDxStreaming->stProfile.eVideoCodec			= convertVideoCodec(aTranscoding->VideoCodec);
	pstDxStreaming->stProfile.eVideoCodecLevel		= eDxVIDEO_ENC_LEVEL_AUTO;
	pstDxStreaming->stProfile.eVideoCodecProfile	= eDxVIDEO_ENC_PROFILE_AUTO;
	pstDxStreaming->stProfile.nVideoBitrateKbps		= aTranscoding->VideoBitrate;	// kbps
	pstDxStreaming->stProfile.nHdVideoBitrateKbps	= 0;

	pstDxStreaming->stProfile.nFrameRate			= 50;

	pstDxStreaming->stProfile.nScreenHeight			= convertScreenHeight(aTranscoding->VideoResolution);
	pstDxStreaming->stProfile.nScreenWidth			= convertScreenWidth(aTranscoding->VideoResolution);

	if (_useTranscoder == true)
	{
		pstDxStreaming->stTripleId.usOnId	= _dvbId.OriginalNetworkId;;
		pstDxStreaming->stTripleId.usSvcId	= _dvbId.ServiceId;
		pstDxStreaming->stTripleId.usTsId	= _dvbId.TransportStreamId;
	}


	stSchedule.uExtInfo.stStrm.eUpdate	= DxRsvStream_Update_Transcoding;

	hErr = SAPI_UpdateSchedule(_slotid, &stSchedule, &stConfilct, FALSE, FALSE);
	if (ERR_OK != hErr)
	{
		printf(HxANSI_COLOR_RED("SAPI_UpdateSchedule failed(hErr:%d), ulSlot = %d\n"), hErr, _slotid);
		HxLOG_Error(HxANSI_COLOR_RED("SAPI_UpdateSchedule failed(hErr:%d), ulSlot = %d\n"), hErr, _slotid);
		ErrCode = NERR_SAPI_UPDATESCHEDULE;
		goto ErrBlock;
	}


ErrBlock:
	return ErrCode;
}

NovaspreadTUInt32 TunerObject::getTransportSessionId()
{
	if (_tsid == NOVA_INVALID_ID)
	{
		_tsid = NOVA_CAS_GetTransportSessionIdFromTripleId(hAction, _dvbId.OriginalNetworkId, _dvbId.TransportStreamId, _dvbId.ServiceId);
		HxLOG_Warning(HxANSI_COLOR_YELLOW("%s : Get ulTransportSessionId (%d)\n"), __FUNCTION__, _tsid);
	}

	return _tsid;
}

//============================================================================================================================
NOVA_ErrCode_e TunerObject::updateSchedule()
{
	NOVA_ErrCode_e		ErrCode = NERR_OK;
	HERROR				hErr = ERR_OK;
	DxSchedule_t		stSchedule;
	SAPI_Conflict_t 	stConfilct;
	UNIXTIME			utCurTime = (HLIB_STD_GetSystemTime());

	HxLOG_Info(HxANSI_COLOR_CYAN("Update ScheduleTime : +%d sec\n"), SATIP_SCHEDULE_DURATION);

	hErr = SAPI_GetSchedule(_slotid, &stSchedule);
	if (ERR_OK != hErr)
	{
		HxLOG_Error(HxANSI_COLOR_RED("SAPI_GetSchedule eHOM_Result_Error\n"));
		ErrCode =  NERR_SAPI_GETSCHEDULE;
		goto ErrBlock;
	}

	stSchedule.ulDuration = utCurTime - _utSchStartTime + SATIP_SCHEDULE_DURATION;
	stSchedule.uExtInfo.stStrm.eUpdate	= DxRsvStream_Update_Time;

	hErr = SAPI_UpdateSchedule(_slotid, &stSchedule, &stConfilct, FALSE, FALSE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error(HxANSI_COLOR_RED("SAPI_UpdateSchedule failed(hErr:%d), ulSlot = %d\n"), hErr, _slotid);
		ErrCode = NERR_SAPI_UPDATESCHEDULE;
		goto ErrBlock;
	}

ErrBlock:
	return ErrCode;
}

bool TunerObject::checkScheduleTime()
{
	double timeNow, interval = SATIP_SCHEDULE_DURATION - 10;

	timeNow = NOVA_getTime();

	if (timeNow - _updateScheduleTime >= interval )
	{
		_updateScheduleTime = timeNow;
		return true;
	}

	return false;
}

bool TunerObject::checkTunerInfoTime()
{
	double timeNow, interval;

	timeNow = NOVA_getTime();

	if (_isLocked == false)
	{
		interval = 0.1;
	}
	else
	{
		interval = 1.0;
	}

	if (timeNow - _updateTunerInfoTime >= interval )
	{
		_updateTunerInfoTime = timeNow;
		return true;
	}

	return false;
}

NOVA_ErrCode_e TunerObject::updateTunerInfo()
{
	NOVA_ErrCode_e ErrCode = NERR_ERROR;
	HERROR hErr = ERR_FAIL;
	OxSysInfoTuner_SignalInfo_t	stSignalInfo;
	HBOOL bLock;

	if(_tunerid == TO_INVALID_TUNERID)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] Invalid tuner id\n"), __FUNCTION__, __LINE__);
		ErrCode = NERR_TUNER_INVALID_TUNERID;
		goto ErrBlock;
	}

	hErr = OAPI_SYSINFO_TUNER_IsLocked(_tunerid, &bLock);
	if (hErr != ERR_OK)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] OAPI_SYSINFO_TUNER_IsLocked failed(hErr:%d)\n"), __FUNCTION__, __LINE__, hErr);
		ErrCode = NERR_OAPI_ISLOCKED;
		goto ErrBlock;
	}

	if (bLock == TRUE)
	{
		hErr = OAPI_SYSINFO_TUNER_GetSignalInfo(_tunerid, &stSignalInfo);
		if (hErr != ERR_OK)
		{
			HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] OAPI_SYSINFO_TUNER_GetSignalInfo failed(hErr:%d)\n"), __FUNCTION__, __LINE__, hErr);
			ErrCode = NERR_OAPI_GETSIGNALINFO;
			goto ErrBlock;
		}

		sem_wait(_sema);
		_isLocked		= true;
		_signalLevel	= (stSignalInfo.nSignalLevel * 255) / 100;	// refer to sat>ip spec(3.5.7 Listing available media streams (DESCRIBE)
		_signalQuality	= (stSignalInfo.nSignalQuality * 15) / 100;
		sem_post(_sema);
	}
	else
	{
		sem_wait(_sema);
		_isLocked		= false;
		_signalLevel	= 0;
		_signalQuality	= 0;
		sem_post(_sema);
	}

	HxLOG_Info("tuner(id,locked,level,quality)=%d,%d,%d,%d (%f)\n",_tunerid, _isLocked, _signalLevel, _signalQuality, NOVA_getTime());

	return NERR_OK;

ErrBlock:

	// in case of invalid tunerid , set values as below (lock=false, level=0, quality=0).
	sem_wait(_sema);
	_isLocked		= false;
	_signalLevel	= 0;
	_signalQuality	= 0;
	sem_post(_sema);

	return ErrCode;
}

bool TunerObject::checkDrmKeyHandle()
{
	if (_useCrypto == false)
	{
		return false;
	}

	if (_tsid == NOVA_INVALID_ID)
	{
		return false;
	}

	if (_keyHandle == NOVA_INVALID_HANDLE)
	{
		return true;
	}

#ifdef CONFIG_DEBUG
	if (_keyHandlerInfo == NULL)
	{
		return true;
	}
#endif

	return false;
}

void TunerObject::updateDrmKeyHandle()
{
	if (_keyHandle == NOVA_INVALID_HANDLE)
	{
		_keyHandle = NOVA_CAS_DRM_GetKeyHandlerFromTsId(_tsid);
		if (_keyHandle == NOVA_INVALID_HANDLE)
		{
			HxLOG_Error("NOVA_CAS_DRM_GetKeyHandlerFromTsId(tsid:%d) failed\n", _tsid);
		}
		else
		{
			HxLOG_Error(HxANSI_COLOR_YELLOW("tsid:%u keyHandle:%x\n"), _tsid, _keyHandle);
		}
	}

#ifdef CONFIG_DEBUG
	if (_keyHandlerInfo == NULL)
	{
		_keyHandlerInfo = NOVA_CAS_DRM_GetKeyInfoFromTsId(_tsid);
		if (_keyHandlerInfo == NULL)
		{
			HxLOG_Error("NOVA_CAS_DRM_GetKeyInfoFromTsId(tsid:%d) failed\n", _tsid);
		}
		else
		{
			HxLOG_Error(HxANSI_COLOR_YELLOW("tsid:%u _keyHandlerInfo:%p\n"), _tsid, _keyHandlerInfo);
		}
	}
#endif
}

bool TunerObject::checkAudioVideoPID(void)
{
	if (_useCrypto == false)			return false;

	if (_audioPID == NOVA_INVALID_PID || _videoPID == NOVA_INVALID_PID)	return true;

	return false;
}

void TunerObject::updateAudioVideoPID(void)
{
	NOVA_CAS_TransportSessionInfo_t *  tsInfo;

	tsInfo =  NOVA_CAS_GetTransportSessionInfoFromTripleId(hAction, _dvbId.OriginalNetworkId, _dvbId.TransportStreamId, _dvbId.ServiceId);
	if (tsInfo != NULL)
	{
		_audioPID = tsInfo->usAudioPid;
		_videoPID = tsInfo->usVideoPid;

		HxLOG_Error(HxANSI_COLOR_YELLOW("audioPID:%d videoPID:%d\n"), _audioPID, _videoPID);
	}
}

void* TunerObject::_scheduleThread(void* aParam)
{
	TunerObject* pTuner = NULL;

	if (aParam == NULL)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] Invalid args. _scheduleThread exits!\n"), __FUNCTION__, __LINE__);
		return 0;
	}

	pTuner = static_cast<TunerObject*>(aParam);
	return pTuner->scheduleThread();
}

void* TunerObject::scheduleThread()
{
	int sleepTime = 0;
	TUNER_STATE_e state;
	HxRingBuf_Status_t stStatus;
	HERROR hErr;
	unsigned char *InBuffer = NULL;
	unsigned char *OutBuffer = NULL;

	HxLOG_Info("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	if (_hringbuf_fromObama == HANDLE_NULL ||  _slotid == TO_INVALID_SLOTID)
	{
		HxLOG_Error(HxANSI_COLOR_RED("Invalid args(hringbuf:%d, slotid:%d)\n"),  _hringbuf_fromObama, _slotid);
		return 0;
	}

	state = getState();
	_updateScheduleTime		= 0;
	_updateTunerInfoTime	= 0;

	if (_useCrypto == true)
	{
		/* 2016/2/15 mhkang:
			DI_MI_DECRYPTOR_SEC_SessionEncrypt함수는 DMA를 이용하여 CPU 도움없이 encrypt를 수행한다.
			이때 사용되는 buffer를 malloc으로 할당받게 되면, 이 buffer의 실제 Physical 메모리는 불연속으로된 메모리일 수 있다.
			따라서 DMA 성능 향상을 위해서는 연속적인 Physical 메모리를 할당받아야 한다.
		*/
		InBuffer = (unsigned char *)DI_MI_DECRYPTOR_SOC_Malloc(ENCRYPTION_SIZE);
		if (NULL == InBuffer)
		{
			HxLOG_Error(HxANSI_COLOR_RED("InBuffer = DI_MI_DECRYPTOR_SOC_Malloc(%d) filed\n"), ENCRYPTION_SIZE);
			return 0;
		}

		OutBuffer = (unsigned char *)DI_MI_DECRYPTOR_SOC_Malloc(ENCRYPTION_SIZE);
		if (NULL == OutBuffer)
		{
			HxLOG_Error(HxANSI_COLOR_RED("OutBuffer = DI_MI_DECRYPTOR_SOC_Malloc(%d) filed\n"), ENCRYPTION_SIZE);
			return 0;
		}
	}

	while(state == TS_INACTIVE || state == TS_ACTIVE)
	{
		sleepTime = 1000;	// 1msec

		// If the state is TS_INACTIVE,  clear RingBuf.
		if (getState() == TS_INACTIVE)
		{
			PL_RINGBUF_ClearBuffer(_hringbuf_fromObama);
			sleepTime = 10000; // 10 msec
		}

		if (getState() == TS_ACTIVE)
		{
			memset(&stStatus, 0, sizeof(stStatus));
			hErr = PL_RINGBUF_GetBufferingStatus(_hringbuf_fromObama, &stStatus);
			if(ERR_OK == hErr && stStatus.ulUnreadSize > 0)
			{
				if (_dataAvailable == false)
				{
					_dataAvailable = true;
					if (_fDataAvailableListener != NULL)
					{
						_fDataAvailableListener(_contextDataAvailableListener);
					}
				}

				if (_useCrypto == true && checkDrmKeyHandle() == false && checkAudioVideoPID() == false)
				{
					HUINT32		ulDataSize, ulWrittenSize;
					int enc_size;

					enc_size =  (stStatus.ulUnreadSize > ENCRYPTION_SIZE) ? ENCRYPTION_SIZE : stStatus.ulUnreadSize;

					// Read Clear TS
					hErr = ::PL_RINGBUF_ReadData(_hringbuf_fromObama, enc_size, (HUINT8*)InBuffer, &ulDataSize);
					if(ERR_OK != hErr || 0 == ulDataSize)
					{
						HxLOG_Error(HxANSI_COLOR_RED("Can not read ts from ringbuf(%d), ulDataSize:%d, hErr:%d\n"), _hringbuf_fromObama, ulDataSize, hErr);
					}

					if (encryptOnlyAvPackets( InBuffer, OutBuffer, ulDataSize, _audioPID, _videoPID) == false)
					{
						HxLOG_Error(HxANSI_COLOR_RED("PSEUDO_DI_MI_DECRYPTOR_SEC_SessionEncrypt(keyHandle:%p,InBuf:%p,OutBuf:%p,Size:%d) failed!\n"), _keyHandle, InBuffer, OutBuffer, ulDataSize);
					}
					else
					{
						// Append Re-encryption TS to the RingBuf for DRM Streaming
						hErr = PL_RINGBUF_WriteData(_hringbuf_forDRM, ulDataSize, OutBuffer, &ulWrittenSize);
						if (ERR_OK != hErr)
						{
							HxLOG_Error (HxANSI_COLOR_RED("PL_RINGBUF_WriteData(hringbuf:%p,size:%u,..) failed\n"), _hringbuf_forDRM, ulDataSize);
						}

						sleepTime = 0;	// no delay
					}
				}
			}
		}

		// delayed setPids
		if (_isSetPids)
		{
			if (_isLocked == true)
			{
				updatePids();
			}
		}

		// Update TunerInfo
		if (checkTunerInfoTime() == true)
		{
			updateTunerInfo();
		}

		// Update ScheduleTime
		if (checkScheduleTime() == true)
		{
			updateSchedule();
		}

		// In case of using DRM, KeyHandle must be updated.
		if (checkDrmKeyHandle() == true)
		{
			updateDrmKeyHandle();
		}

		// Update Audio and Video PID
		if (checkAudioVideoPID() == true)
		{
			updateAudioVideoPID();
		}

		usleep(sleepTime);
		state = getState();
	}

	if (_useCrypto == true)
	{
		if (InBuffer != NULL)	DI_MI_DECRYPTOR_SOC_Free(InBuffer);
		if (OutBuffer != NULL)	DI_MI_DECRYPTOR_SOC_Free(OutBuffer);
	}

	HxLOG_Info("[%s:%d] (-)\n", __FUNCTION__, __LINE__);
	return 0;
}

bool  TunerObject::onSapiNotifierTuner(SAPI_NotifyType_e eNotifyType, DxSchedule_t* pstSchedule, unsigned int aRequestPid)
{
	int ret;
	void* pRet;
	string  reqProcess;
#ifndef CONFIG_ENHANCED_SECURITY
	unsigned int ulPid;
#endif

	HxLOG_Print("[%s:%d] slotid:%d eNotifyType:%d ReqPID:%d (+) %f\n", __FUNCTION__, __LINE__, _slotid, eNotifyType , aRequestPid, NOVA_getTime());

	switch (eNotifyType)
	{
	case eSAPI_NOTIFY_TYPE_ScheduleAdded:
		setState(TS_INACTIVE);
		ret = pthread_create(&_thread, NULL, TunerObject::_scheduleThread, this);
		if (ret != 0)
		{
			HxLOG_Error("pthread_create failed(ret:%d errno:%d)\n", ret, errno);
		}

		break;

	case eSAPI_NOTIFY_TYPE_ScheduleChanged:
		if ( pstSchedule != NULL)
		{
			if (_tunerid == TO_INVALID_TUNERID)
			{
				_tunerid = pstSchedule->uExtInfo.stStrm.stTuningParam.sat.ulTunerGroupId;
				HxLOG_Info("selected tuner id: %d\n", _tunerid);
			}

			if (pstSchedule->uExtInfo.stStrm.stTuningParam.sat.ulTunerGroupId != NOVA_INVALID_TUNERID) // tuner 할당 완료한 경우
			{
				hAction = pstSchedule->uExtInfo.stStrm.hAction;
				if (_fFinishedListener != NULL)
				{
					printf("Call _fFinishedListener\n");
					_fFinishedListener((NovaspreadTTunerRequestId)this, this, NOVASPREAD_TUNER_ALLOCATION_ERROR_NONE);
					_fFinishedListener = NULL;
				}
			}
		}

		break;

	case eSAPI_NOTIFY_TYPE_ScheduleRemoved:
		setState(TS_EXIT);
		pthread_join(_thread, &pRet);
		_thread = 0;

		if (pstSchedule->stRemovedInfo.eRemovedReason == DxSchedule_RemovedReason_SatipFailed)  // TP Conflict
		{
			printf("Call _fFinishedListener : DetailInfo(%d)\n", pstSchedule->stRemovedInfo.eDetailInfo);
			if (_fFinishedListener != NULL)
			{
				_fFinishedListener((NovaspreadTTunerRequestId)this, NULL, convertTunerAllocationError(pstSchedule->stRemovedInfo.eDetailInfo));
				_fFinishedListener = NULL;

				/* 2016/1/16 mhkang: http://svn.humaxdigital.com:3000/issues/110304#note-7
				  Because TUNERAPI_ReleaseTuner API is never called, TunerManager must get rid of this tuner object.
				*/
				return false;
			}
		}
		else
		{
#ifdef CONFIG_ENHANCED_SECURITY
			if (eSAPI_UNIQUE_PROCESS_Indicator_Nova != aRequestPid)
#else
			ulPid = HLIB_STD_GetPID();

			if (ulPid != aRequestPid)
#endif
			{
				reqProcess = getProcessName(aRequestPid); // Not thread safe!!! Check it!!
				HxLOG_Info("Schedule(slotid:%d) was removed by '%s',  eRemovedReason:%d eDetailInfo:%d\n", _slotid ,reqProcess.c_str(), pstSchedule->stRemovedInfo.eRemovedReason, pstSchedule->stRemovedInfo.eDetailInfo);

				if (g_fnReleaseRequestListener != NULL)
				{
					printf("Call _fReleaseRequestListener\n");
					g_fnReleaseRequestListener( NOVASPREAD_TUNER_RELEASE_REASON_OTHER, this);
				}
				else
				{
					printf("_fReleaseRequestListener is null\n");
				}
			}
		}

		break;

	default:
		// do nothing
		break;
	}

	return true;
}

Handle_t TunerObject::createRingbuf()
{
	Handle_t			hRingbuf = HANDLE_NULL;
	HINT32				nMaxSize = 0;
	HxRingBuf_Setup_t	stRingbufSetup;

	HxSTD_memset(&stRingbufSetup, 0, sizeof(HxRingBuf_Setup_t));

	nMaxSize = 7 * 188 * 5000;		// 2015/7/28 mhkang: it is necessary to tune value of nMaxSize.

	stRingbufSetup.eBufType 		= eHxRINGBUF_TYPE_LINEAR;
	stRingbufSetup.ulMaxSize		= nMaxSize;
	stRingbufSetup.ulChunkSize		= 0;
	stRingbufSetup.bBlockOverflow	= FALSE;			/* true:overflow시 write block 됨. */
	stRingbufSetup.eRuleAfterRead	= eHxRINGBUF_READRULE_REMOVE;

	hRingbuf = PL_RINGBUF_Create(&stRingbufSetup);

	return hRingbuf;
}

bool TunerObject::destroyRingbuf(Handle_t hringbuf)
{
	if (hringbuf == HANDLE_NULL)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] Invalid args(hringbuf is null)\n"), __FUNCTION__, __LINE__);
		return false;
	}

	PL_RINGBUF_Destroy(hringbuf);

	return true;
}

unsigned int TunerObject::getSlotId()
{
	return _slotid;
}


bool  TunerObject::setState(TUNER_STATE_e state)
{
	if (_sema != NULL)
	{
		sem_wait(_sema);
		_state = state;
		sem_post(_sema);
	}

	return true;
}

TUNER_STATE_e TunerObject::getState()
{
	TUNER_STATE_e state = TS_UNKNOWN;

	if (_sema != NULL)
	{
		sem_wait(_sema);
		state = _state;
		sem_post(_sema);
	}

	return state;
}


DxRsvStream_AllocMode_e TunerObject::convertAllocationMode(NovaspreadTTunerAllocationMode mode)
{
	if (mode == NOVASPREAD_TUNER_ALLOCATION_MODE_BEG)
	{
		return DxRsvStream_AllocMode_BEG;
	}
	else if (mode == NOVASPREAD_TUNER_ALLOCATION_MODE_FORCE)
	{
		return DxRsvStream_AllocMode_FORCE;
	}
	else
	{
		HxLOG_Warning("Unknown NovaspreadTTunerAllocationMode value:%d\n", mode);
		return DxRsvStream_AllocMode_None;
	}
}

/* 2015/9/9 mhkang:
[AS-20502_Novaspread-S-reference-manual_v-0-53.pdf/3.2.14 NovaspreadTunerSetPriority]
  * 0000-0999 : reserved for future use
  * 1000-1999 : low priority host features (PIP, Download, etc.)
  * 2000-2999 : NovaspreadClient features
  * 3000-3999 : high priority host features (HDMI=3500)
  * 4000-65535 : reserved for future use
*/
unsigned short TunerObject::convertSatipPriority(unsigned short priority)
{
	// 2015/9/9 mhakng: priority-- 해준 이유  => http://svn.humaxdigital.com:3000/issues/106798#note-5
	if (priority == 0xffff) priority--;

	return priority;
}


DxAudioCodec_e TunerObject::convertAudioCodec(NovaspreadTAudioCodec AudioCodec)
{
	switch(AudioCodec)
	{
	case NOVASPREAD_AUDIO_CODEC_MP2:	return eDxAUDIO_CODEC_AAC;//return eDxAUDIO_CODEC_MPEG;  // MPEG CODEC 사용하면 Transcoding + DRM 요청시 Obama 먹통됨.
	case NOVASPREAD_AUDIO_CODEC_AC3:	return eDxAUDIO_CODEC_DOLBY_AC3;
	case NOVASPREAD_AUDIO_CODEC_LC_AAC: return eDxAUDIO_CODEC_AAC;
	case NOVASPREAD_AUDIO_CODEC_HE_AAC: return eDxAUDIO_CODEC_AAC_PLUS;
	default:							return eDxAUDIO_CODEC_UNKNOWN;
	}
}

DxVideoCodec_e TunerObject::convertVideoCodec(NovaspreadTVideoCodec VideoCodec)
{
	switch(VideoCodec)
	{
	case NOVASPREAD_VIDEO_CODEC_MPEG_2: return eDxVIDEO_CODEC_MPEG2;
	case NOVASPREAD_VIDEO_CODEC_AVC:	return eDxVIDEO_CODEC_H264;
	case NOVASPREAD_VIDEO_CODEC_HEVC:	return eDxVIDEO_CODEC_UNKNOWN;
	default:							return eDxVIDEO_CODEC_UNKNOWN;
	}
}

HINT32 TunerObject::convertScreenHeight(NovaspreadTVideoResolution VideoResolution)
{
	switch(VideoResolution)
	{
	case  NOVASPREAD_VIDEO_RESOLUTION_144P:		return 144;
	case  NOVASPREAD_VIDEO_RESOLUTION_288P:		return 288;
	case  NOVASPREAD_VIDEO_RESOLUTION_576P:		return 576;
	case  NOVASPREAD_VIDEO_RESOLUTION_576I:		return 576;
	case  NOVASPREAD_VIDEO_RESOLUTION_720P:		return 720;
	case  NOVASPREAD_VIDEO_RESOLUTION_1080P:	return 1080;
	case  NOVASPREAD_VIDEO_RESOLUTION_1080I:	return 1080;
	default:									return 720;
	}
}

HINT32 TunerObject::convertScreenWidth(NovaspreadTVideoResolution VideoResolution)
{
	switch(VideoResolution)
	{
	case  NOVASPREAD_VIDEO_RESOLUTION_144P:		return 176;
	case  NOVASPREAD_VIDEO_RESOLUTION_288P:		return 352;
	case  NOVASPREAD_VIDEO_RESOLUTION_576P:		return 720;
	case  NOVASPREAD_VIDEO_RESOLUTION_576I:		return 720;
	case  NOVASPREAD_VIDEO_RESOLUTION_720P:		return 1280;
	case  NOVASPREAD_VIDEO_RESOLUTION_1080P:	return 1920;
	case  NOVASPREAD_VIDEO_RESOLUTION_1080I:	return 1920;
	default:									return 1280;
	}
}

NovaspreadTTunerAllocationError TunerObject::convertTunerAllocationError(HUINT32 DetailInfo)
{
	switch(DetailInfo)
	{
	case DxSchedule_DetailInfo_SatIp_NoTuner:		return NOVASPREAD_TUNER_ALLOCATION_ERROR_NO_TUNER_AVAILABLE;
	case DxSchedule_DetailInfo_SatIp_ForcePossible: return NOVASPREAD_TUNER_ALLOCATION_ERROR_FORCE_POSSIBLE;
	case DxSchedule_DetailInfo_SatIp_NoTransCoder:	return NOVASPREAD_TUNER_ALLOCATION_ERROR_NO_TRANSCODER_AVAILABLE;
	case DxSchedule_DetailInfo_SatIp_NoLNB:			return NOVASPREAD_TUNER_ALLOCATION_ERROR_NO_LNB_AVAILABLE;
	default:										return NOVASPREAD_TUNER_ALLOCATION_ERROR_NO_TUNER_AVAILABLE;	// default error로 처리
	}
}

HCHAR* TunerObject::getProcessName(HINT32 nRequestPid)
{
#if defined(CONFIG_ENHANCED_SECURITY)
	static char szProcessName[128];

	switch(nRequestPid)
	{
	case eSAPI_UNIQUE_PROCESS_Indicator_None:
		snprintf(szProcessName,128,"%s(%d)","None", nRequestPid);
		break;
	case eSAPI_UNIQUE_PROCESS_Indicator_Sama:
		snprintf(szProcessName,128,"%s(%d)","Sama", nRequestPid);
		break;
	case eSAPI_UNIQUE_PROCESS_Indicator_Obama:
		snprintf(szProcessName,128,"%s(%d)","Obama", nRequestPid);
		break;
	case eSAPI_UNIQUE_PROCESS_Indicator_Homma:
		snprintf(szProcessName,128,"%s(%d)","Homma", nRequestPid);
		break;
	case eSAPI_UNIQUE_PROCESS_Indicator_Corsair:
		snprintf(szProcessName,128,"%s(%d)","Corsair", nRequestPid);
		break;
	case eSAPI_UNIQUE_PROCESS_Indicator_Ipepg:
		snprintf(szProcessName,128,"%s(%d)","Ipepg", nRequestPid);
		break;
	case eSAPI_UNIQUE_PROCESS_Indicator_Nova:
		snprintf(szProcessName,128,"%s(%d)","Nova", nRequestPid);
		break;
	case eSAPI_UNIQUE_PROCESS_Indicator_Unknown:
	default:
		snprintf(szProcessName,128,"%s(%d)","Unknown", nRequestPid);
		break;
	}
	return szProcessName;
#else
	return HLIB_STD_GetProcessName(nRequestPid);
#endif
}


//  ==================== PID Parser =====================================
// 2016/1/16 mhkang:http://svn.humaxdigital.com:3000/issues/110335

static void print_pid(unsigned pid)
{
	switch(pid)
	{
	case PID_PAT	: printf("pid:");printf(HxANSI_COLOR_YELLOW("%d\n"), pid); break;
	case PID_PMT	: printf("pid:");printf(HxANSI_COLOR_PURPLE("%d\n"), pid); break;
	case PID_AUDIO	: printf("pid:");printf(HxANSI_COLOR_WHITE ("%d\n"), pid); break;
	case PID_VIDEO	: printf("pid:");printf(HxANSI_COLOR_CYAN  ("%d\n"), pid); break;
	default:		  printf("pid:");printf(HxANSI_COLOR_GREEN ("%d\n"), pid); break;
	}
}

static void print_pids(unsigned char* inBuffer,  int size)
{
	int ts_cnt = size / 188;
	int i;
	int pid;

	printf("-------\n");
	for (i = 0 ; i < ts_cnt ; i++)
	{
		pid = parse_pid( &inBuffer[i * 188] );
		print_pid(pid);
	}
}

static unsigned parse_pid(unsigned char packet[188])
{
	unsigned pid;

	if (packet[0] != TRANSPORT_SYNC_BYTE)
	{
		printf("Missing Sync Byte!(%X)\n", packet[0]);
		return PID_INVALID;
	}

	pid = ((packet[1] & 0x1F) << 8) | packet[2];

	return pid;
}

static HINT32 convert_source_number(DxAntInfo_t*pstAntInfo)
{
	if (pstAntInfo == NULL)
	{
		HxLOG_Error("Invalid args(pstAntInfo is null)\n");
		return -1;
	}
	switch(pstAntInfo->antType)
	{
		case eDxANT_TYPE_LNB_ONLY:
			return (HINT32)eSatipSrcType_LNB_ONLY;

		case eDxANT_TYPE_DISEQC:
			switch(pstAntInfo->info.diseqc.diseqcInput)
			{
			case eDxDISEQC_INPUT_A:		return (HINT32)eSatipSrcType_DISEQC_A;
			case eDxDISEQC_INPUT_B:		return (HINT32)eSatipSrcType_DISEQC_B;
			case eDxDISEQC_INPUT_C:		return (HINT32)eSatipSrcType_DISEQC_C;
			case eDxDISEQC_INPUT_D:		return (HINT32)eSatipSrcType_DISEQC_D;
			default:
				break;
			}
			break;

		case eDxANT_TYPE_SCD:
			switch(pstAntInfo->info.scd.scdInput)
			{
			case eDxSCD_INPUT_SCD_A:	return (HINT32)eSatipSrcType_SCD_A;
			case eDxSCD_INPUT_SCD_B:	return (HINT32)eSatipSrcType_SCD_B;
			default:
				break;
			}
			break;

		case eDxANT_TYPE_SMATV:
			return (HINT32)eSatipSrcType_SMATV;

		case eDxANT_TYPE_MONOBLOCK_LNB:
			return (HINT32)eSatipSrcType_MONOBLOCK_LNB;
	}

	return (HINT32)eSatipSrcType_None;
}

bool TunerObject::encryptOnlyAvPackets(unsigned char* in_buffer, unsigned char* out_buffer, unsigned size, HUINT16 audioPID, HUINT16 videoPID)
{
	int ts_cnt = size / 188;
	int i;
	int pid;

#ifdef SUPPORT_REENCRYPTION
	// When you test re-encryption/decryption, Use ((NOVA_CAS_KeyHandlerInfo_t *)_keyHandlerInfo)->ucKey instead of NULL.
	if (DI_MI_DECRYPTOR_SEC_SessionEncrypt((unsigned int*)_keyHandle, in_buffer, out_buffer, size, NULL) != 0)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] DI_MI_DECRYPTOR_SEC_SessionEncrypt failed, keyHandle:%u, size:%d\n"), __FUNCTION__, __LINE__, _keyHandle, size);
		return false;
	}


	for (i = 0 ; i < ts_cnt ; i++)
	{
		pid = parse_pid( &in_buffer[i * 188] );

		if (pid == audioPID || pid == videoPID)
		{
			// do nothing
		}
		else
		{
			memcpy(&out_buffer[i * 188], &in_buffer[i * 188], 188);
		}
	}
#else
	memcpy(out_buffer, in_buffer, size);
#endif

	return true;
}
