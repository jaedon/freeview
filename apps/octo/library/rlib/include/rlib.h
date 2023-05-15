/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  rlib.h
	@brief

	Description:  									\n
	Module: Library/RLIB		\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding �� ����ϴ� source file �Դϴ�.
* MS949 encoding�����Ī��� source file�Ǫ���
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

#ifndef ___RLIB_H___
#define ___RLIB_H___

#ifdef	__cplusplus
extern "C" {
#endif
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <dlib.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define RxRSCID_NULL				(HUINT32)0xFFFFFFFF
#define RxRSCID_DYNAMIC				(HUINT32)0xFFFFFFFE

#define RxRFINPUT_NUM_MAX			8
#define RxTUNER_NUM_MAX				8

#define RxHANDLE_NULL				(Handle_t)NULL
#define RxINST_NAME_LEN				32

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eRxRSCTYPE_TUNER_GROUP,			/**< Tuner group ���ҽ� */
	eRxRSCTYPE_DEMUX,				/**< Demux ���ҽ� */
	eRxRSCTYPE_AV_DEC,				/**< Video decoder ���ҽ� */
	eRxRSCTYPE_PVR_PLAYER,			/**< PVR Player ���ҽ� */
	eRxRSCTYPE_HDD_REC,				/**< HDD recording ���ҽ� */
	eRxRSCTYPE_RFINPUT, 			/**< RF Input ���ҽ� : Satellite �̿ܿ����� ������ 1 */
	eRxRSCTYPE_TRANSCODER,			/**< Transcoder ���ҽ� */

	eRxRSCTYPE_MAX
} RxRscType_e;

typedef enum
{
	eRxUSAGETYPE_NONE,

	eRxUSAGETYPE_DEMUX_LIVEREC,
	eRxUSAGETYPE_DEMUX_PLAYBACK,

	eRxUSAGETYPE_AVDEC_MAIN,
	eRxUSAGETYPE_AVDEC_SUB,

	eRxUSAGETYPE_RECHDD_REC,
	eRxUSAGETYPE_RECHDD_TRANS,
	eRxUSAGETYPE_RECHDD_SATIP,

	eRxUSAGETYPE_END
} RxUsageType_e;

typedef enum
{
	eRxARGTYPE_NONE,

	eRxARGTYPE_TUNEPARAM,

	eRxARGTYPE_END
} RxTrialArgType_e;

typedef struct
{
	DxDeliveryType_e	 eSupportedDeliType;
	HUINT32				 ulConnectableTunerNum;
	HUINT32				 aulConnectableTunerId[RxTUNER_NUM_MAX];
} RxRfInfoCapacity_t;

/*
	Resource Request ���� Structure:
*/
typedef HUINT32 		RxRscReqHandle_t;

typedef struct
{
	HBOOL				 bRequest;
	HUINT32				 ulResourceId;
} RxRscAlloc_t;

typedef struct
{
	RxTrialArgType_e	 eArgType;
	DxTuneParam_t		 stTuneParam;
} RxTrialArg_TuneParam_t;

typedef union
{
	RxTrialArgType_e		 eArgType;
	RxTrialArg_TuneParam_t	 stTuneParam;
} RxTrialArg_t;

typedef struct
{
	HUINT32				 ulActionId;
	HUINT32				 ulNumActionToBeKilled;
	HUINT32				*pulActionsToBeKilled;
	RxRscAlloc_t		 astRscAllocTable[eRxRSCTYPE_MAX];

	HBOOL				 bDuplicated;
} RxRscTrialResult_t;


/*
	Resource Status ���� :
*/
typedef struct
{
	RxUsageType_e		 eUsageType;
} RxTemplateObject_t;

typedef struct
{
	RxUsageType_e		 eUsageType;
	DxDeliveryType_e	 eSupportedDeliType;	/**< ������/���̺��/�����������Ŀ�/�Ƴ��α������Ŀ� ���� TS port�� �Է� ������ delivery type�뵵�� ��Ÿ���� ��.
														������ �ǰ� ������� ����. �� tuner group�� 2�� �̻��� delivery type�� �Էµ� �� ����. */
	DxTuneParam_t		 stTuneParam;			/**< �� Ʃ�ʰ� locking�ϰ� �ִ� Ʃ�� ���� */
	HBOOL				 bRfInputTaken; 		/**< �� Ʃ�ʰ� RF-Input�� ��� �ִ°� ? */
	HUINT32 			 ulRfInputIndex;		/**< �� Ʃ�ʰ� ��� �ִ� RF-Input�� RscID */
} RxTunerObject_t;

typedef struct
{
	HBOOL				bAvailable;

	DxTuneParam_t		stMasterTuneParam;			/**< �� RF Input�� Tuning�ϰ� �ִ� TP Info: ��Ȯ�� ���ϸ� �� TP Info�� �浹���� �ʴ� TP�� �̶�� �� */

	HUINT32 			ulConnectableTunerNum;
	HUINT32 			aulConnectableTunerId[RxTUNER_NUM_MAX];
} RxSatRfInputObj_t;

typedef struct
{
	HUINT32 			 ulReserved;
} RxCabRfInputObj_t;

typedef struct
{
	HUINT32 			 ulReserved;
} RxTerRfInputObj_t;

typedef struct
{
	RxUsageType_e		 eUsageType;
	DxDeliveryType_e	 eSupportedDeliType;	/**< ������/���̺��/�����������Ŀ�/�Ƴ��α������Ŀ� ���� TS port�� �Է� ������ delivery type�뵵�� ��Ÿ���� ��.
													������ �ǰ� ������� ����. �� tuner group�� 2�� �̻��� delivery type�� �Էµ� �� ����. */
	union
	{
		RxSatRfInputObj_t	 stSatRf;
		RxCabRfInputObj_t	 stCabRf;
		RxTerRfInputObj_t	 stTerRf;
	} utObj;
} RxRfInputObject_t;

typedef struct
{
	RxRscType_e 			 eRscType;					/**< �� Resource�� Type */
	HUINT32					 ulRscId;					/** <�� Resource�� Resource ID */
	HINT32					 nRefCount; 				/**< �� Resource�� ����ϰ� �ִ� action�� ���� */
	HBOOL					 bSharable; 				/**< �� Resource�� �ܵ����� �����Ǿ� �ִ���,  �����ǰ� �ִ��� ��Ÿ��. */

	// Ư���� Property�� �ִ� Resource���� ����
	union
	{
		RxTemplateObject_t	 stTemplate;		/** < DEMUX, A/V DEC, etc.. */
		RxTunerObject_t 	 stTuner;			/**< Tuner Object */
		RxRfInputObject_t	 stRfInput; 		/**< R/F Input Object */
	} utObj;
} RxRscObject_t;


// For Debugging:
typedef void (*RxDbgPrintCallback_t) (const HCHAR *szFormat,...);


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
/*
	RLIB Initialization Functions:
*/
extern Handle_t				RLIB_CreateSessionHandle (HCHAR *szInstName);
extern HERROR 			RLIB_ClearSessionHandleInfo (Handle_t hSession);
extern HERROR 			RLIB_CopySessionHandleInfo (Handle_t hSessionSrc, Handle_t hSessionDst);
extern HERROR				RLIB_SetActionNumber (Handle_t hSession, HUINT32 ulMaxActNum);
extern HERROR				RLIB_SetResourceNumber (Handle_t hSession, RxRscType_e eRscType, HUINT32 ulRscNum);
extern HERROR				RLIB_SetRfInfoCapacity (Handle_t hSession, HUINT32 ulRscId, RxRfInfoCapacity_t *pstCapacity);
extern HERROR				RLIB_SetTunerCapacity (Handle_t hSession, HUINT32 ulRscId, DxDeliveryType_e eSupportedDeliType);
extern HERROR				RLIB_SetResourceUsageType (Handle_t hSession, RxRscType_e eRscType, HUINT32 ulRscId, RxUsageType_e eUsageType);

/*
	RLIB Resource Allocation & Deallocation Functions:
*/
extern RxRscReqHandle_t		RLIB_MakeResourceRequestHandle (Handle_t hSession, HUINT32 ulActionId, HUINT32 ulActionPriority);
extern RxRscReqHandle_t		RLIB_DuplicateRequestHandle (RxRscReqHandle_t hRscReq);
extern HERROR				RLIB_AddRemovableActionIdToRequestHandle (RxRscReqHandle_t hRscReq, HUINT32 ulActionId, HUINT32 ulActionPriority);
extern HERROR				RLIB_AddIgnorableActionIdToRequestHandle (RxRscReqHandle_t hRscReq, HUINT32 ulActionId);
extern HERROR				RLIB_AddRequiredResourceToRequestHandle (RxRscReqHandle_t hRscReq, RxRscType_e eRscType, HUINT32 ulResourceId, HBOOL bSharable, RxUsageType_e eUsageType);
extern HERROR				RLIB_AddArgumentToRequestHandle (RxRscReqHandle_t hRscReq, RxTrialArg_t *punArg);
extern HERROR				RLIB_TryResourceRequest (RxRscReqHandle_t hRscReq);
extern HERROR				RLIB_SetResourcesByTrialResults (RxRscReqHandle_t hRscReq);
extern RxRscTrialResult_t*	RLIB_GetRequestTrialResult (RxRscReqHandle_t hRscReq);
extern HERROR				RLIB_DestroyResourceRequestHandle (RxRscReqHandle_t hRscReq);
extern HERROR				RLIB_ClearActionResource (Handle_t hSession, HUINT32 ulActionId);

/*
	RLIB General Functions:
*/
extern HERROR				RLIB_GetResourceId (Handle_t hSession, HUINT32 ulActionId, RxRscType_e eRscType, HUINT32 *pulRscId);
extern HERROR				RLIB_ChangeResourceId (Handle_t hSession, HUINT32 ulActionId, RxRscType_e eRscType, HUINT32 ulRscId);
extern HERROR				RLIB_GetActionIdByResourceId (Handle_t hSession, HUINT32 ulRscId, RxRscType_e eRscType, HUINT32 *pulActionId);

extern HERROR				RLIB_GetRfInputAbility (Handle_t hSession, HUINT32 ulRfInputId, HBOOL *pbAbility);
extern HERROR				RLIB_SetRfInputAbility (Handle_t hSession, HUINT32 ulRfInputId, HBOOL bAbility);

/*
	for Updating the resource status:
*/
extern HERROR				RLIB_GetResourceObject (Handle_t hSession, HUINT32 ulActionId, RxRscType_e eRscType, RxRscObject_t *pstRscObj);
extern HERROR				RLIB_SetResourceObject (Handle_t hSession, HUINT32 ulActionId, RxRscType_e eRscType, RxRscObject_t *pstRscObj);
extern HERROR				RLIB_GetResourceNumber (Handle_t hSession, RxRscType_e eRscType, HUINT32 *pulRscNum);
extern HERROR				RLIB_GetRfInfoCapacity (Handle_t hSession, HUINT32 ulRscId, RxRfInfoCapacity_t *pstCapacity);
extern HERROR				RLIB_GetTunerCapacity (Handle_t hSession, HUINT32 ulRscId, DxDeliveryType_e *peSupportedDeliType);
extern HERROR				RLIB_GetResourceUsageType (Handle_t hSession, RxRscType_e eRscType, HUINT32 ulRscId, RxUsageType_e *peUsageType);
extern HERROR				RLIB_GetResourceReferenceCount (Handle_t hSession, RxRscType_e eRscType, HUINT32 ulRscId, HINT32 *pnRefCount);
extern HERROR				RLIB_GetPhysicalTuningParamInSatellite (HUINT32 ulLnbFreq, HUINT32 ulFreq, DxSat_Polarization_e ePolar, HBOOL b22kTone, HUINT32 *pulPhysicalFreq, HBOOL *pbPhysical22kTone);
extern HERROR				RLIB_GetActualLnbFrequency (HUINT32 ulLnbFreq, HUINT32 ulFreq, DxSat_Polarization_e ePolar, HUINT32 *pulActualLnbFreq);

extern HCHAR*				RLIB_GetResourceStr (RxRscType_e eRscType);
extern void					RLIB_PrintRscInfo (RxDbgPrintCallback_t pfPrintFunc, HINT32 nPrintIndex);

#ifdef __cplusplus
}
#endif
#endif
/* end of file */
