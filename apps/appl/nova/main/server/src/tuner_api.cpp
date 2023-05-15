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

#include "apk.h"
#include "tuner_api.h"
#include "nova_tuner_manager.h"
#include "nova_tuner_object.h"
#include "nova_receiver.h"
#include "nova_utils.h"


// ========================== PUBLIC API =========================================
NOVA_ErrCode_e TUNERAPI_Init(void)
{
	NOVA_ErrCode_e ErrCode = NERR_ERROR;
	HERROR hErr;

	HxLOG_Info(HxANSI_COLOR_PURPLE("[%s:%d] (+) : %f\n"), __FUNCTION__, __LINE__, NOVA_getTime());

	hErr = APK_SYSINFO_TUNER_Init();
	if (hErr != ERR_OK)
	{
		HxLOG_Error(HxANSI_COLOR_RED("APK_SYSINFO_TUNER_Init failed(hErr:%d)\n"), hErr);
		ErrCode = NERR_INIT;
		goto ErrBlock;
	}

	ErrCode = NOVA_Receiver_Init();
	if (ErrCode != NERR_OK)
	{
		HxLOG_Error(HxANSI_COLOR_RED("NOVA_Receiver_Init failed(Err:%d)\n"), ErrCode);
		goto ErrBlock;
	}

	ErrCode = TunerManager::instance()->init();
	if (ErrCode != NERR_OK)
	{
		HxLOG_Error(HxANSI_COLOR_RED("TunerManager::instance()->init failed(Err:%d)\n"), ErrCode);
		goto ErrBlock;
	}

	HxLOG_Info(HxANSI_COLOR_PURPLE("[%s:%d] (-) : %f\n"), __FUNCTION__, __LINE__, NOVA_getTime());

	return  NERR_OK;

ErrBlock:
	return ErrCode;
}

NOVA_ErrCode_e TUNERAPI_DeInit(void)
{
	NOVA_ErrCode_e ErrCode = NERR_ERROR;
	HERROR hErr;

	HxLOG_Info(HxANSI_COLOR_PURPLE("[%s:%d] (+) : %f\n"), __FUNCTION__, __LINE__, NOVA_getTime());

	ErrCode = TunerManager::instance()->deinit();
	if (ErrCode != NERR_OK)
	{
		HxLOG_Error(HxANSI_COLOR_RED("TunerManager::instance()->deinit failed(Err:%d)\n"), ErrCode);
		goto ErrBlock;
	}

	ErrCode = NOVA_Receiver_DeInit();
	if (ErrCode != NERR_OK)
	{
		HxLOG_Error(HxANSI_COLOR_RED("NOVA_Receiver_Init failed(Err:%d)\n"), ErrCode);
		goto ErrBlock;
	}

	hErr = APK_SYSINFO_TUNER_DeInit();
	if (hErr != ERR_OK)
	{
		HxLOG_Error(HxANSI_COLOR_RED("APK_SYSINFO_TUNER_DeInit failed(hErr:%d)\n"), hErr);
		ErrCode = NERR_INIT;
		goto ErrBlock;
	}

	HxLOG_Info(HxANSI_COLOR_PURPLE("[%s:%d] (-) : %f\n"), __FUNCTION__, __LINE__, NOVA_getTime());

	return NERR_OK;

ErrBlock:
	return ErrCode;
}

void* TUNERAPI_AllocateTuner(TUNER_PARAM_t aTunerParam, NovaspreadTTunerAllocationFinishedListener afFinishedListener)
{
	TunerObject* pTuner = NULL;

	HxLOG_Info(HxANSI_COLOR_PURPLE("[%s:%d] (+) : %f\n"), __FUNCTION__, __LINE__, NOVA_getTime());

	pTuner = TunerManager::instance()->allocateTuner(aTunerParam, afFinishedListener);
	if (pTuner == NULL)
	{
		HxLOG_Error(HxANSI_COLOR_RED("TunerManager::instance()->allocateTuner failed\n"));
	}

	HxLOG_Info(HxANSI_COLOR_PURPLE("[%s:%d] (-) : %f\n"), __FUNCTION__, __LINE__, NOVA_getTime());

	return pTuner;
}


NovaspreadTTunerRequestId TUNERAPI_GetTunerRequestID(void* aTuner)
{
	return (NovaspreadTTunerRequestId)aTuner;
}


NOVA_ErrCode_e TUNERAPI_ReleaseTuner(void* aTuner)
{
	TunerObject* pTuner;
	NOVA_ErrCode_e ErrCode = NERR_ERROR;

	HxLOG_Info(HxANSI_COLOR_PURPLE("[%s:%d] (+) : %f\n"), __FUNCTION__, __LINE__, NOVA_getTime());

	if (aTuner == NULL)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] Invalida args\n"), __FUNCTION__, __LINE__);
		ErrCode = NERR_INVALIDARGS;
		goto ErrBlock;
	}

	pTuner = static_cast<TunerObject*>(aTuner);
	ErrCode = TunerManager::instance()->releaseTuner(pTuner);
	if (ErrCode != NERR_OK)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] TunerManager::instance()->releaseTuner failed(Err:%d)\n"), __FUNCTION__, __LINE__, ErrCode);
		goto ErrBlock;
	}

	HxLOG_Info(HxANSI_COLOR_PURPLE("[%s:%d] (-) : %f\n"), __FUNCTION__, __LINE__, NOVA_getTime());

	return NERR_OK;

ErrBlock:
	return ErrCode;
}


NOVA_ErrCode_e TUNERAPI_SetPids(void* aTuner, unsigned short* aPids, int aPidCount)
{
	NOVA_ErrCode_e ErrCode = NERR_ERROR;
	TunerObject* pTuner;

	string pids;
	char buf[100];

	for (int i = 0 ; i < aPidCount ; i++)
	{
		sprintf(buf, "%d ", aPids[i]);
		pids += buf;
	}

	HxLOG_Info(HxANSI_COLOR_PURPLE("[%s:%d] pids:%s, cnt:%d (+) : %f\n"), __FUNCTION__, __LINE__, pids.c_str(), aPidCount, NOVA_getTime());

	if (aTuner == NULL || aPids == NULL)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] Invalida args\n"), __FUNCTION__, __LINE__);
		ErrCode = NERR_INVALIDARGS;
		goto ErrBlock;
	}

	pTuner = static_cast<TunerObject*>(aTuner);
	ErrCode = pTuner->setPids(aPids, aPidCount);
	if (ErrCode != NERR_OK)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] pTuner->setPids Failed(Err:%d)\n"), __FUNCTION__, __LINE__, ErrCode);
		goto ErrBlock;
	}

	HxLOG_Info(HxANSI_COLOR_PURPLE("[%s:%d] (-) : %f\n"), __FUNCTION__, __LINE__, NOVA_getTime());

	return NERR_OK;

ErrBlock:
	return ErrCode;
}

int TUNERAPI_ReadPackets(void* aTuner,  unsigned char* aBuffer, unsigned int aReqSize)
{
	TunerObject* pTuner;
	int readSize;

	if (aTuner == NULL || aBuffer == NULL || aReqSize <= 0)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] Invalida args\n"), __FUNCTION__, __LINE__);
		goto ErrBlock;
	}

	pTuner = static_cast<TunerObject*>(aTuner);
	readSize = pTuner->readPackets(aBuffer, aReqSize);

	return readSize;

ErrBlock:
	return -1;
}


NOVA_ErrCode_e TUNERAPI_IsLocked(void* aTuner, bool* aLocked)
{
	NOVA_ErrCode_e ErrCode = NERR_ERROR;
	TunerObject* pTuner;

	/* 2015/8/28 mhkang: too many log
	HxLOG_Info(HxANSI_COLOR_PURPLE("[%s:%d] (+) : %f\n"), __FUNCTION__, __LINE__, NOVA_getTime());
	*/

	if (aTuner == NULL || aLocked == NULL)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] Invalida args\n"), __FUNCTION__, __LINE__);
		ErrCode = NERR_INVALIDARGS;
		goto ErrBlock;
	}

	pTuner = static_cast<TunerObject*>(aTuner);
	ErrCode = pTuner->isLocked(aLocked);

	/* 2015/8/28 mhkang: too many log
	HxLOG_Info(HxANSI_COLOR_PURPLE("[%s:%d] (-) : %f\n"), __FUNCTION__, __LINE__, NOVA_getTime());
	*/

	return ErrCode;

ErrBlock:
	return ErrCode;
}


NOVA_ErrCode_e TUNERAPI_GetSignalLevel(void* aTuner, int* aLevel)
{
	NOVA_ErrCode_e ErrCode = NERR_ERROR;
	TunerObject* pTuner;

	/* 2015/8/28 mhkang: too many log
	HxLOG_Info(HxANSI_COLOR_PURPLE("[%s:%d] (+) : %f\n"), __FUNCTION__, __LINE__, NOVA_getTime());
	*/

	if (aTuner == NULL || aLevel == NULL)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] Invalida args\n"), __FUNCTION__, __LINE__);
		ErrCode = NERR_INVALIDARGS;
		goto ErrBlock;
	}

	pTuner = static_cast<TunerObject*>(aTuner);
	ErrCode =  pTuner->getSignalLevel(aLevel);

	/* 2015/8/28 mhkang: too many log
	HxLOG_Info(HxANSI_COLOR_PURPLE("[%s:%d] (-) : %f\n"), __FUNCTION__, __LINE__, NOVA_getTime());
	*/

	return ErrCode;

ErrBlock:
	return ErrCode;
}

NOVA_ErrCode_e TUNERAPI_GetSignalQuality(void* aTuner,int* aQuality)
{
	NOVA_ErrCode_e ErrCode = NERR_ERROR;
	TunerObject* pTuner;

	/* 2015/8/28 mhkang: too many log
	HxLOG_Info(HxANSI_COLOR_PURPLE("[%s:%d] (+) : %f\n"), __FUNCTION__, __LINE__, NOVA_getTime());
	*/

	if (aTuner == NULL || aQuality == NULL)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] Invalida args\n"), __FUNCTION__, __LINE__);
		ErrCode = NERR_INVALIDARGS;
		goto ErrBlock;
	}

	pTuner = static_cast<TunerObject*>(aTuner);
	ErrCode = pTuner->getSignalQuality(aQuality);

	/* 2015/8/28 mhkang: too many log
	HxLOG_Info(HxANSI_COLOR_PURPLE("[%s:%d] (-) : %f\n"), __FUNCTION__, __LINE__, NOVA_getTime());
	*/

	return ErrCode;

ErrBlock:
	return ErrCode;
}


NOVA_ErrCode_e TUNERAPI_Start(void* aTuner)
{
	NOVA_ErrCode_e ErrCode = NERR_ERROR;
	TunerObject* pTuner;

	HxLOG_Info(HxANSI_COLOR_PURPLE("[%s:%d] (+) : %f\n"), __FUNCTION__, __LINE__, NOVA_getTime());

	if (aTuner == NULL)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] Invalida args\n"), __FUNCTION__, __LINE__);
		ErrCode = NERR_INVALIDARGS;
		goto ErrBlock;
	}

	pTuner = static_cast<TunerObject*>(aTuner);

	if (pTuner->start() == false)
	{
		ErrCode = NERR_START;
		goto ErrBlock;
	}

	HxLOG_Info(HxANSI_COLOR_PURPLE("[%s:%d] (-) : %f\n"), __FUNCTION__, __LINE__, NOVA_getTime());

	return NERR_OK;

ErrBlock:
	return ErrCode;
}


NOVA_ErrCode_e TUNERAPI_Stop(void* aTuner)
{
	NOVA_ErrCode_e ErrCode = NERR_ERROR;
	TunerObject* pTuner;

	HxLOG_Info(HxANSI_COLOR_PURPLE("[%s:%d] (+) : %f\n"), __FUNCTION__, __LINE__, NOVA_getTime());

	if (aTuner == NULL)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] Invalida args\n"), __FUNCTION__, __LINE__);
		ErrCode = NERR_INVALIDARGS;
		goto ErrBlock;
	}

	pTuner = static_cast<TunerObject*>(aTuner);

	if (pTuner->stop() == false)
	{
		ErrCode = NERR_START;
		goto ErrBlock;
	}

	HxLOG_Info(HxANSI_COLOR_PURPLE("[%s:%d] (-) : %f\n"), __FUNCTION__, __LINE__, NOVA_getTime());

	return NERR_OK;

ErrBlock:
	return ErrCode;
}

NOVA_ErrCode_e	TUNERAPI_GetTunerID(void* aTuner, unsigned int* aTunerID)
{
	NOVA_ErrCode_e ErrCode = NERR_ERROR;
	TunerObject* pTuner;

	HxLOG_Info(HxANSI_COLOR_PURPLE("[%s:%d] (+) : %f\n"), __FUNCTION__, __LINE__, NOVA_getTime());

	if (aTuner == NULL)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] Invalida args\n"), __FUNCTION__, __LINE__);
		ErrCode = NERR_INVALIDARGS;
		goto ErrBlock;
	}

	pTuner = static_cast<TunerObject*>(aTuner);
	*aTunerID = pTuner->getTunerId();

	HxLOG_Info(HxANSI_COLOR_PURPLE("[%s:%d] (-) : %f\n"), __FUNCTION__, __LINE__, NOVA_getTime());

	return NERR_OK;

ErrBlock:
	return ErrCode;
}


NOVA_ErrCode_e	TUNERAPI_SetDataAvailableListner(void* aTuner, TunerDataAvailableListener afListener, void* aContextDataAvailableListener)
{
	NOVA_ErrCode_e ErrCode = NERR_ERROR;
	TunerObject* pTuner;

	HxLOG_Info(HxANSI_COLOR_PURPLE("[%s:%d] (+) : %f\n"), __FUNCTION__, __LINE__, NOVA_getTime());

	if (aTuner == NULL)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] Invalida args\n"), __FUNCTION__, __LINE__);
		ErrCode = NERR_INVALIDARGS;
		goto ErrBlock;
	}

	pTuner = static_cast<TunerObject*>(aTuner);
	pTuner->setDataAvailableListener(afListener, aContextDataAvailableListener);

	HxLOG_Info(HxANSI_COLOR_PURPLE("[%s:%d] (-) : %f\n"), __FUNCTION__, __LINE__, NOVA_getTime());

	return NERR_OK;

ErrBlock:
	return ErrCode;
}

NOVA_ErrCode_e	TUNERAPI_SetStateChangeListner(void* aTuner, NovaspreadTTunerStateChangeListener afListener, void* aContextStateChangeListener)
{
	NOVA_ErrCode_e ErrCode = NERR_ERROR;
	TunerObject* pTuner;

	HxLOG_Info(HxANSI_COLOR_PURPLE("[%s:%d] (+) : %f\n"), __FUNCTION__, __LINE__, NOVA_getTime());

	if (aTuner == NULL)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] Invalida args\n"), __FUNCTION__, __LINE__);
		ErrCode = NERR_INVALIDARGS;
		goto ErrBlock;
	}

	pTuner = static_cast<TunerObject*>(aTuner);
	pTuner->setStateChangeListener(afListener, aContextStateChangeListener);


	HxLOG_Info(HxANSI_COLOR_PURPLE("[%s:%d] (-) : %f\n"), __FUNCTION__, __LINE__, NOVA_getTime());

	return NERR_OK;

ErrBlock:
	return ErrCode;
}

NOVA_ErrCode_e	TUNERAPI_SetPriority(void* aTuner, unsigned short aPriority)
{
	NOVA_ErrCode_e ErrCode = NERR_ERROR;
	TunerObject* pTuner;

	HxLOG_Info(HxANSI_COLOR_PURPLE("[%s:%d] (+) : %f\n"), __FUNCTION__, __LINE__, NOVA_getTime());

	if (aTuner == NULL)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] Invalida args\n"), __FUNCTION__, __LINE__);
		ErrCode = NERR_INVALIDARGS;
		goto ErrBlock;
	}

	pTuner = static_cast<TunerObject*>(aTuner);
	ErrCode = pTuner->setPriority(aPriority);
	if (ErrCode != NERR_OK)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] setPriority failed(Err:%d)\n"), __FUNCTION__, __LINE__, ErrCode);
		goto ErrBlock;
	}

	return NERR_OK;

ErrBlock:
	return ErrCode;
}

NOVA_ErrCode_e TUNERAPI_SetTranscoding(void* aTuner, NovaspreadTTranscoding* aTranscoding)
{
	NOVA_ErrCode_e ErrCode = NERR_ERROR;
	TunerObject* pTuner;

	HxLOG_Info(HxANSI_COLOR_PURPLE("[%s:%d] (+) : %f\n"), __FUNCTION__, __LINE__, NOVA_getTime());

	if (aTuner == NULL)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] Invalida args\n"), __FUNCTION__, __LINE__);
		ErrCode = NERR_INVALIDARGS;
		goto ErrBlock;
	}

	pTuner = static_cast<TunerObject*>(aTuner);
	ErrCode = pTuner->setTranscoding(aTranscoding);
	if (ErrCode != NERR_OK)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] setPriority failed(Err:%d)\n"), __FUNCTION__, __LINE__, ErrCode);
		goto ErrBlock;
	}

	return NERR_OK;

ErrBlock:
	return ErrCode;
}

NOVA_ErrCode_e	TUNERAPI_GetErrorCode(void* aTuner)
{
	printf("TUNERPAI_GetErrorCode is not implemented!!\n");
	return NERR_NOTIMPLEMENTED;
}


NovaspreadTUInt32 TUNERAPI_GetTransportSessionId(void* aTuner)
{
	TunerObject* pTuner;

	HxLOG_Info(HxANSI_COLOR_PURPLE("[%s:%d] (+) : %f\n"), __FUNCTION__, __LINE__, NOVA_getTime());

	if (aTuner == NULL)
	{
		HxLOG_Error(HxANSI_COLOR_RED("[%s:%d] Invalida args\n"), __FUNCTION__, __LINE__);
		goto ErrBlock;
	}

	pTuner = static_cast<TunerObject*>(aTuner);
	return pTuner->getTransportSessionId();

ErrBlock:
	return 0;
}
