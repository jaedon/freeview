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
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
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
#include "nova_tuner_manager.h"
#include "nova_tuner_object.h"
#if 0
#include "int_pl_ringbuf_context.h"
#else
#include "nova_ringbuf.h"
#endif

/**********************************************************************************************************/
#define ____DEFINE________________________
/**********************************************************************************************************/

/**********************************************************************************************************/
#define ____TYPEDEF________________________
/**********************************************************************************************************/

/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/



/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/


TunerManager* TunerManager::_instance = NULL;


TunerManager* TunerManager::instance()
{
	if (_instance == NULL)
	{
		_instance = new TunerManager();
	}

	return _instance;
}

TunerManager::TunerManager()
{
	_sema = new sem_t;
	sem_init(_sema, 0, 1);
}

TunerManager::~TunerManager()
{
	sem_destroy(_sema);
	delete[] _sema;
	_sema = NULL;
}

NOVA_ErrCode_e TunerManager::init()
{
	HERROR hErr = ERR_FAIL;
	NOVA_ErrCode_e ErrCode = NERR_ERROR;

	HxLOG_Info("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	hErr = SAPI_RegisterNotifier (this, _onSapiNotifier);
	if (ERR_OK != hErr)
	{
		HxLOG_Error (HxANSI_COLOR_RED("SAPI_RegisterNotifier failed(hErr:%d)\n"), hErr);
		ErrCode = NERR_SAPI_REGISTER_NOFIFIER;
		return ErrCode;
	}

	HxLOG_Info("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return NERR_OK;
}


NOVA_ErrCode_e TunerManager::deinit()
{
	HERROR hErr = ERR_FAIL;
	TUNER_MAP_IT it;
	TunerObject* pTuner;
	NOVA_ErrCode_e ErrCode = NERR_ERROR;

	HxLOG_Info("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	sem_wait(_sema);
	for (it = _tuners.begin() ; it !=  _tuners.end() ; it++)
	{
		pTuner = (TunerObject*)it->second;
		delete pTuner;
	}
	_tuners.clear();
	sem_post(_sema);

	hErr = SAPI_UnregisterNotifier(this, _onSapiNotifier);
	if (ERR_OK != hErr)
	{
		HxLOG_Error (HxANSI_COLOR_RED ("SAPI_UnregisterNotifier failed(hErr:%d)\n"), hErr);
		ErrCode = NERR_SAPI_UNREGISTER_NOFIFIER;
		return ErrCode;
	}

	HxLOG_Info("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return NERR_OK;
}

TunerObject* TunerManager::allocateTuner(TUNER_PARAM_t aTunerParam, NovaspreadTTunerAllocationFinishedListener afFinishedListener)
{
	TunerObject* pTuner = NULL;
	NOVA_ErrCode_e ErrCode = NERR_ERROR;

	HxLOG_Info("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	pTuner = new TunerObject( aTunerParam, afFinishedListener);
	ErrCode = pTuner->makeSchedule(); // Call SAPI_MakeSchedule with param

	if (ErrCode != NERR_OK)
	{
		HxLOG_Error (HxANSI_COLOR_RED("pTuner->makeSchedule failed(Err:%d)\n"), ErrCode);
		goto ErrBlock;
	}

	sem_wait(_sema);
	_tuners.insert(make_pair(pTuner->getSlotId(), pTuner));
	sem_post(_sema);

	return pTuner;

ErrBlock:
	if (pTuner != NULL)	delete pTuner;
	pTuner = NULL;

	return NULL;
}

NOVA_ErrCode_e TunerManager::releaseTuner(TunerObject* pTuner)
{
	NOVA_ErrCode_e ErrCode = NERR_ERROR;
	int slotid;
	TUNER_MAP_IT	it;

	HxLOG_Info("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	if (pTuner == NULL)
	{
		HxLOG_Error (HxANSI_COLOR_RED("invalid args(pTuner is null)\n"));
		ErrCode = NERR_INVALIDARGS;
		goto ErrBlock;
	}

	slotid = pTuner->getSlotId();

	if (pTuner->getState() != TS_EXIT)
	{
		ErrCode = pTuner->cancelSchedule();	// blocking
		if (ErrCode != NERR_OK)
		{
			HxLOG_Warning (HxANSI_COLOR_RED("pTuner->cancelSchedule failed(Err:%d)\n"), ErrCode);
			/* 2016/1/15 mhkang: http://svn.humaxdigital.com:3000/issues/110304#note-6
			goto ErrBlock;
			*/
		}
	}

	sem_wait(_sema);
	it = _tuners.find(slotid);
	if (it != _tuners.end())
	{
		_tuners.erase(it);
	}
	else
	{
		HxLOG_Info("[%s:%d] invalid slotid:%d\n", __FUNCTION__, __LINE__, slotid);
	}
	sem_post(_sema);

	delete pTuner;

	HxLOG_Info("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return NERR_OK;

ErrBlock:
	return ErrCode;
}

HERROR TunerManager::_onSapiNotifier(void *UserData, SAPI_NotiInfo_t *punNotify)
{
	TunerManager*	mgr;

	if (punNotify == NULL || UserData == NULL)
	{
		HxLOG_Error (HxANSI_COLOR_RED( "Invalid Args\n"));
		return ERR_FAIL;
	}

	mgr = static_cast<TunerManager*>(UserData);

	return mgr->onSapiNotifier(punNotify);
}

HERROR TunerManager::onSapiNotifier(SAPI_NotiInfo_t *punNotify)
{
	TUNER_MAP_IT	it;
	DxSchedule_t*	pstSchedule = NULL;	
	HINT32			ulSlot = TO_INVALID_SLOTID;
	unsigned int	nRequestPid = 0;
	TunerObject*	pTuner = NULL;

	HxLOG_Info("[%s:%d] eNotifyType:%d\n", __FUNCTION__, __LINE__,  punNotify->eNotifyType);

	switch (punNotify->eNotifyType)
	{
	case eSAPI_NOTIFY_TYPE_SchedulesLoaded:
		if(NULL != punNotify->stSchLoaded.pstSchArray)
		{
			ulSlot = punNotify->stSchLoaded.pstSchArray->ulSlot;
		}
		pstSchedule = punNotify->stSchLoaded.pstSchArray;
		nRequestPid = punNotify->stSchLoaded.nRequestPid;
		break;

	case eSAPI_NOTIFY_TYPE_ScheduleAdded:
		ulSlot = punNotify->stSchAdded.ulSlotId;
		pstSchedule = punNotify->stSchAdded.pstSchedule;
		nRequestPid = punNotify->stSchAdded.nRequestPid;
		break;

	case eSAPI_NOTIFY_TYPE_ScheduleChanged:
		ulSlot = punNotify->stSchChanged.ulSlotId;
		pstSchedule = punNotify->stSchChanged.pstSchedule;
		nRequestPid = punNotify->stSchChanged.nRequestPid;
		break;

	case eSAPI_NOTIFY_TYPE_ScheduleRemoved:
		ulSlot = punNotify->stSchRemoved.ulSlotId;
		pstSchedule = punNotify->stSchRemoved.pstSchedule;
		nRequestPid = punNotify->stSchRemoved.nRequestPid;
		break;

	case eSAPI_NOTIFY_TYPE_TimeUp:
		ulSlot = punNotify->stTimeUp.ulSlotId;
		pstSchedule = punNotify->stTimeUp.pstSchedule;
		nRequestPid = 0;
		break;

	case eSAPI_NOTIFY_TYPE_ConflictScheduleAdded:
		ulSlot		= punNotify->stConfSchAdded.stConflicted.stTriedSchedule.ulSlot;
		nRequestPid = 0;
		break;

	default:
		HxLOG_Warning (HxANSI_COLOR_RED("Unknown sapi event:%d\n"), punNotify->eNotifyType);
		return ERR_FAIL;
	}

	sem_wait(_sema);
	it = _tuners.find(ulSlot);
	if (it != _tuners.end())
	{		
		pTuner = (TunerObject*)it->second;
		if (pTuner->onSapiNotifierTuner(punNotify->eNotifyType, pstSchedule, nRequestPid) == false)
		{
			// 2016/1/15 mhkang: http://svn.humaxdigital.com:3000/issues/110304#note-7
			_tuners.erase(it);
			delete pTuner;
		}
	}
	sem_post(_sema);

	return ERR_OK;
}
