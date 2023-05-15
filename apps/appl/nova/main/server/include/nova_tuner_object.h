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

#pragma once

/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include <stdbool.h>
#include <assert.h>
#include <string>
#include <set>
#include <map>
#include <semaphore.h>

#include "dlib.h"

#include "nova_errcode.h"
#include "nova_types.h"
#include "tuner_api.h"
#include "udp_sender.h"

using namespace std;
/**********************************************************************************************************/
#define ____DEFINE________________________
/**********************************************************************************************************/
//#define TEST_UDP_SENDER
#ifdef TEST_UDP_SENDER
#define TEST_IP						"192.168.0.3"
#define TEST_PORT					55555
#endif

/**********************************************************************************************************/
#define ____TYPEDEF________________________
/**********************************************************************************************************/
typedef set<int>			PID_SET;
typedef PID_SET::iterator	PID_SET_IT;

/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/



/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/

typedef enum
{
	TS_UNKNOWN = -1,
	TS_NONE,
	TS_INACTIVE,
	TS_ACTIVE,
	TS_CANCELING,
	TS_EXIT
} TUNER_STATE_e;


class TunerObject
{
public:
	DxTuneParam_t*	_tuneParam;
	int				_src;
	unsigned int	_tunerid;
	PID_SET			_pids;
	unsigned int	_slotid;

	TunerObject(TUNER_PARAM_t aTunerParam, NovaspreadTTunerAllocationFinishedListener afFinishedListener);
	~TunerObject();

	NOVA_ErrCode_e	makeSchedule();
	NOVA_ErrCode_e	cancelSchedule();
	bool			start();
	bool			stop();
	NOVA_ErrCode_e	setPids(unsigned short* aPids, int aPidCount);
	int				readPackets(unsigned char* pBuffer, unsigned int reqSize);

	NOVA_ErrCode_e	isLocked(bool* aLocked);
	NOVA_ErrCode_e	getSignalLevel(int* aLevel);
	NOVA_ErrCode_e	getSignalQuality(int* aLevel);
	bool			setDataAvailableListener(TunerDataAvailableListener afDataAvailableListener, void* aContextDataAvailableListener);
	bool			setStateChangeListener(NovaspreadTTunerStateChangeListener afStateChangeListener, void* aContextStateChangeListener);
	NOVA_ErrCode_e	setPriority(unsigned short aPriority);
	NOVA_ErrCode_e	setTranscoding(NovaspreadTTranscoding* aTranscoding);

	unsigned int	getSlotId();
	unsigned int	getTunerId();
	NovaspreadTUInt32 getTransportSessionId();

protected:
	friend class TunerManager;

	TUNER_STATE_e	_state;					// NONE ==> INACTIVE <==> ACTIVE ==> EXIT
	Handle_t		_hringbuf_fromObama;	// Obama로부터 수신한 TS를 저장하는 RingBuf
	Handle_t		_hringbuf_forDRM;		// SAT>IP DRM 요청시 Re-encryption된 TS가 저장되는 RingBuf
	Handle_t		_hringbuf_forRTP;		// NovaspreadTunerReadData함수 호출시(RTP생성할때 사용되는 RingBuf. hringbuf_fromObama 또는 hringbuf_forDRM 임.
	int				_pidflags;
	pthread_t		_thread;

	bool			_isLocked;
	int				_signalLevel;
	int				_signalQuality;
	double			_updateScheduleTime;
	double			_updateTunerInfoTime;
	sem_t*			_sema;
	bool			_dataAvailable;
	bool			_isSetPids;
	UNIXTIME		_utSchStartTime;
	UNIXTIME		_utSchEndTime;
	NovaspreadTTunerAllocationMode	_mode;
	unsigned short		_priority;
	NovaspreadTDvbId	_dvbId;
	bool				_useTranscoder;
	bool				_useCrypto;
	NovaspreadTTranscoding  _transcodingParam;
	NovaspreadTUInt32	_tsid;
	HUINT32				_keyHandle;
	HUINT32				hAction;
#ifdef CONFIG_DEBUG
	void* _keyHandlerInfo;
#endif

	HUINT16			_audioPID;
	HUINT16			_videoPID;

#ifdef TEST_UDP_SENDER
	UdpSender		_UdpSender;
#endif
	bool			onSapiNotifierTuner(SAPI_NotifyType_e eNotifyType, DxSchedule_t* pstSchedule, unsigned int aRequestPid);
	Handle_t		createRingbuf(void);
	bool			destroyRingbuf(Handle_t ahRingBuf);
	bool			loadAntennaInfo(void);
	bool			checkScheduleTime(void);
	bool			checkTunerInfoTime(void);
	bool			checkDrmKeyHandle(void);
	bool			checkAudioVideoPID(void);

	NOVA_ErrCode_e	updateSchedule(void);
	NOVA_ErrCode_e	updateTunerInfo(void);
	NOVA_ErrCode_e	updatePids(void);
	void			updateDrmKeyHandle(void);
	void			updateAudioVideoPID(void);

	bool			setState(TUNER_STATE_e state);
	TUNER_STATE_e	getState(void);

	DxAudioCodec_e	convertAudioCodec(NovaspreadTAudioCodec AudioCodec);
	DxVideoCodec_e	convertVideoCodec(NovaspreadTVideoCodec VideoCodec);
	HINT32			convertScreenHeight(NovaspreadTVideoResolution VideoResolution);
	HINT32			convertScreenWidth(NovaspreadTVideoResolution VideoResolution);
	NovaspreadTTunerAllocationError convertTunerAllocationError(HUINT32 DetailInfo);

	static void*	_scheduleThread(void* aParam);
	void*			scheduleThread();
	DxRsvStream_AllocMode_e convertAllocationMode(NovaspreadTTunerAllocationMode mode);
	unsigned short	convertSatipPriority(unsigned short priority);	
	HCHAR*			getProcessName(HINT32 nRequestPid);
	bool			encryptOnlyAvPackets(unsigned char* in_buffer, unsigned char* out_buffer, unsigned size, HUINT16 audioPID, HUINT16 videoPID);

	void*			_contextAllocator;				// for TunerAllocationFinishedListener and TunerReleaseRequestedListener
	void*			_contextDataAvailableListener;	// for TunerDataAvailableListener
	void*			_contextStateChangeListener;	// for TunerStateChangeListener

	NovaspreadTTunerAllocationFinishedListener	_fFinishedListener;
	NovaspreadTTunerReleaseRequestListener		_fReleaseRequestListener;
	TunerDataAvailableListener					_fDataAvailableListener;
	NovaspreadTTunerStateChangeListener			_fStateChangeListener;

};
