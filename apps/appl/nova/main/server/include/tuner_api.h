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


#ifndef __TUNER_API_H__
#define __TUNER_API_H__

#include <stdbool.h>
#include <NovaspreadTuner.h>
#include "nova_errcode.h"
#include "nova_types.h"


//typedef void (* TunerAllocationFinishedListener )		(NovaspreadTTunerRequestId       aRequestId,    NovaspreadTTuner                aTuner,	    NovaspreadTTunerAllocationError aAllocationError );
typedef void (* TunerReleaseRequestedListener )			(void * aContext, void* aTuner );
typedef void (* TunerDataAvailableListener )			(void * aContext);
typedef void (* TunerStateChangeListener )				(void * aContext, int aNewState );


#ifdef __cplusplus
extern "C" {
#endif

/**
****************************************************************************************
@brief
@param
@return 성공하면 NERR_OK가 반환된다.
*****************************************************************************************
*/
NOVA_ErrCode_e	TUNERAPI_Init(void);

/**
****************************************************************************************
@brief
@param
@return 성공하면 NERR_OK가 반환된다.
*****************************************************************************************
*/
NOVA_ErrCode_e	TUNERAPI_DeInit(void);

/**
****************************************************************************************
@brief
@param[IN] aTunerParam
@param[IN] afFinishedListener
@param[IN] afReleaseRequestListener
@param[IN] aContext
@return 성공하면 aTuner 가 반환된다. Type of aTuner is class TunerObject.
*****************************************************************************************
*/
void* TUNERAPI_AllocateTuner(TUNER_PARAM_t aTunerParam, NovaspreadTTunerAllocationFinishedListener afFinishedListener);

/**
****************************************************************************************
  @brief aTuner의 RequestID를 반환한다.
  @param aTuners
****************************************************************************************
*/
NovaspreadTTunerRequestId TUNERAPI_GetTunerRequestID(void* aTuner);

/**
****************************************************************************************
@brief
@param[IN] aTuner
@return 성공하면 true 실패하면 false가 반환된다.
*****************************************************************************************
*/
NOVA_ErrCode_e	TUNERAPI_ReleaseTuner(void* aTuner);

/**
****************************************************************************************
@brief
@param[IN] aTuner
@return 성공하면 true 실패하면 false가 반환된다.
*****************************************************************************************
*/
NOVA_ErrCode_e	TUNERAPI_Start(void* aTuner);

/**
****************************************************************************************
@brief
@param[IN] aTuner
@return 성공하면 true 실패하면 false가 반환된다.
*****************************************************************************************
*/
NOVA_ErrCode_e	TUNERAPI_Stop(void* aTuner);

/**
****************************************************************************************
@brief
@param[IN] aTuner
@param[IN] aPids
@param[IN] aPidCount
@return 성공하면 NERR_OK가 반환된다.
*****************************************************************************************
*/
NOVA_ErrCode_e 	TUNERAPI_SetPids(void* aTuner, unsigned short* aPids, int aPidCount);

/**
****************************************************************************************
@brief
@param[IN] aTuner
@param[OUT] aBuffer
@param[IN] aReqSize
@return 읽은 크기를 반환한다. 에러발생시 < 0 값이 반환된다.
*****************************************************************************************
*/
int	TUNERAPI_ReadPackets(void* aTuner,  unsigned char* aBuffer, unsigned int aReqSize);

/**
****************************************************************************************
@brief
@param[IN] aTuner
@param[OUT] aLocked
@return 성공하면 NERR_OK가 반환된다.
*****************************************************************************************
*/
NOVA_ErrCode_e	TUNERAPI_IsLocked(void* aTuner, bool* aLocked);

/**
****************************************************************************************
@brief
@param[IN] aTuner
@param[OUT] aLevel
@return 성공하면 NERR_OK가 반환된다.
*****************************************************************************************
*/
NOVA_ErrCode_e	TUNERAPI_GetSignalLevel(void* aTuner, int* aLevel);

/**
****************************************************************************************
@brief
@param[IN] aTuner
@param[OUT] aQuality
@return 성공하면 NERR_OK가 반환된다.
*****************************************************************************************
*/
NOVA_ErrCode_e	TUNERAPI_GetSignalQuality(void* aTuner, int* aQuality);

/**
****************************************************************************************
@brief
@param[IN]
@return
*****************************************************************************************
*/
NOVA_ErrCode_e	TUNERAPI_GetErrorCode(void* aTuner); // not implemented

/**
****************************************************************************************
@brief
@param[IN]
@return
*****************************************************************************************
*/
NOVA_ErrCode_e	TUNERAPI_GetTunerID(void* aTuner);

/**
****************************************************************************************
@brief
@param[IN]
@param[IN]
@param[IN]
@return
*****************************************************************************************
*/
NOVA_ErrCode_e	TUNERAPI_SetDataAvailableListner(void* aTuner, TunerDataAvailableListener afListener, void* aContextDataAvailableListener);

/**
****************************************************************************************
@brief
@param[IN]
@param[IN]
@param[IN]
@return
*****************************************************************************************
*/
NOVA_ErrCode_e	TUNERAPI_SetStateChangeListner(void* aTuner, NovaspreadTTunerStateChangeListener afListener, void* aContextStateChangeListener);


/**
****************************************************************************************
@brief
@param[IN]	aTuner
@param[IN]	aPriority
@return
*****************************************************************************************
*/
NOVA_ErrCode_e	TUNERAPI_SetPriority(void* aTuner, unsigned short aPriority);


NOVA_ErrCode_e TUNERAPI_SetTranscoding(void* aTuner, NovaspreadTTranscoding* aTranscoding);


NovaspreadTUInt32 TUNERAPI_GetTransportSessionId(void* aTuner);

#ifdef __cplusplus
}
#endif

#endif
