/*
 * (c) 2011-2012 Humax Co., Ltd.
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
/*******************************************************************
	File Description
********************************************************************/
/**
@file				cx_core_sc.h
@brief			cx_core_sc.h (Conax SC)

	Description:   \n
	Module: MW/CAS/ CONAX \n

	Copyright (c) 2009 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

#ifndef	__CXCORE_SC_H__
#define	__CXCORE_SC_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <conax.h>

/*******************************************************************/
/********************      Global Variables 	********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition 	********************/
/*******************************************************************/

// error code
#define CX_ERR_OK						0			// ERR_OK
#define CX_ERR_FAIL						1			// ERR_FAIL
#define CX_ERR_SC_ACCESS_DENIED			2			// 0x90 12
#define CX_ERR_SC_BIT_ERROR				3			// 0x90 13 : bit error
#define CX_ERR_SC_WRONG_PIN				4			// 0x90 17
#define CX_ERR_SC_PIN_LOCKED			5			// 0x90 19
#define CX_ERR_SC_INCORRECT_CARD		6			// 0x6d 00 & 0x6e 00
#define CX_ERR_SC_CHANGE_PIN_FAILED		7			// 0x90 30
#define CX_ERR_SC_READ_NULL_BYTE		8
#define CX_ERR_SC_READ					10			// 0x98 nn : read

#define CX_INVALID_PRODUCT_REF		0xFFFFFFFF	//## 스펙에 나온거는 아니고, 임의로 정한 거임


#define CX_SC_NONE						(-1)
#define CX_SC_SLOT_0					0
#define CX_SC_SLOT_1					1


/*******************************************************************/
/********************      Typedef  	   *************************/
/*******************************************************************/

typedef enum
{
	eCxSc_InitCass		= 0x26,
	eCxSc_InitOaa		= 0x82,
	eCxSc_EmmCom		= 0x84,
	eCxSc_EcmCom		= 0xA2,
	eCxSc_SecurityOp	= 0xC2,
	eCxSc_RetChan		= 0xC4,
	eCxSc_CaStatusCom	= 0xC6,
	eCxSc_PinCom		= 0xC8,
	eCxSc_Read			= 0xCA
} CxSc_Cmd_t;

typedef enum
{
	eCxSc_HostVer			= 0x10,
	eCxSc_Cat				= 0x11,
	eCxSc_Emm				= 0x12,
	eCxSc_Ecm				= 0x14,
	eCxSc_ReqRetChanConfig	= 0x1B,
	eCxSc_CaStatusSelect	= 0x1C,
	eCxSc_CaPinIn			= 0x1D,
	eCxSc_CaPinChange		= 0x1E,
	eCxSc_MatRatChange		= 0x1F,
	eCxSc_ReqOrderApdu		= 0x40,
	eCxSc_OrderRespApdu		= 0x41,
	eCxSc_SignPinChange		= 0x60,
	eCxSc_GenerateSign		= 0x61,
	eCxSc_VerifySign		= 0x62,
	eCxSc_EncryptMsg		= 0x63,
	eCxSc_DecryptMsg		= 0x64,
	eCxSc_ReqBlockSize		= 0x65,
	eCxSc_ReqCardNumber		= 0x66,
	eCxSc_ReqSequenceNumber	= 0x67,
	eCxSc_ReqRandomOctets	= 0x68,
	eCxSc_ResetSession		= 0x69,
	eCxSc_ReqPinStatus		= 0x6A,
	eCxSc_ReqDebitLog		= 0x6B,
	eCxSc_PairingMech		= 0x6C
} CxSc_InputParam_t;

typedef enum
{
	eCxSc_CassVer				= 0x20,
	eCxSc_CaDescEmm				= 0x22,
	eCxSc_SessionInfo			= 0x23,
	eCxSc_Cw					= 0x25,
	eCxSc_CaSysId				= 0x28,
	eCxSc_OrderReqApdu			= 0x2A,
	eCxSc_RetChanConfig			= 0x2D,
	eCxSc_CountryIndicator		= 0x2F,
	eCxSc_MatRatLevel			= 0x30,
	eCxSc_AccessStatus			= 0x31,
	eCxSc_SubscriptionStatus	= 0x32,
	eCxSc_OrderInfo				= 0x37,
	eCxSc_AcceptViewingInfo		= 0x38,
	eCxSc_EventStatus			= 0x39,
	eCxSc_TokenDebitInfo		= 0x3A,
	eCxSc_PurseStatus			= 0x3B,
	eCxSc_DebitStatus			= 0x3C,
	eCxSc_CreditStatus			= 0x3D,
	eCxSc_SignatureResp			= 0x70,
	eCxSc_EncryptedMsg			= 0x71,
	eCxSc_DecryptedMsg			= 0x72,
	eCxSc_CryptoBlockSize		= 0x73,
	eCxSc_CardNumber			= 0x74,
	eCxSc_SequenceNumber		= 0x75,
	eCxSc_RandomOctets			= 0x76,
	eCxSc_PinStatus				= 0x77,
	eCxSc_HostData				= 0x80,
	eCxSc_ProtectedData			= 0x81,
	eCxSc_DebitLogData			= 0x90
} CxSc_OutputParam_t;

typedef enum
{
	eCxSc_StateNone,			// no card inserted
	eCxSc_StateInserted,
	eCxSc_StateReset,
	eCxSc_StateCassVerReceived,
	eCxSc_StateReady,
	eCxSc_StateDifferentVer,		// incorrect conax card
	eCxSc_StateInvalid,			// incorrect card
	eCxSc_StateCommError		// card problem - check card
} CxSc_State_t;

typedef enum
{
	eCxTokenPPV_Event	= 0,
	eCxTokenPPV_Time
} CxTokenPpvType_t;

typedef enum
{
	eCxPinCheck_General	= 0x0,
	eCxPinCheck_MatRatOverride,
	eCxPinCheck_StartViewing,
	eCxPinCheck_DebitToken
} CxPinCheckType_t;

typedef enum
{
	// basic data type
	eCxData_AsciiText	= 0x01,
	eCxData_OctetStr	= 0x20,
	eCxData_Time		= 0x30,
	eCxData_ProductId	= 0x46,
	eCxData_Price		= 0x47,
	// security data type
	eCxData_Digest		= 0x50,
	eCxData_Signature,
	eCxData_PlainText,
	eCxData_ChpherText,
	eCxData_SignPin
} CxDataTag_t;

typedef struct
{
	CxDataTag_t	eDataTag;
	HUINT8		ucAlgorithm;
	HUINT8		ucDataLen;
	HUINT8* 	pucData;
} CxSecuData_t;

typedef enum
{
	eCxAuth_TimeLimitedAccess	= 0,
	eCxAuth_UnlimitedAccess,
	eCxAuth_CreditBasedAccess
} CxAuthType_t;

// ui 관련..
typedef struct
{
	HUINT8	ucStatus1;
	HUINT8	ucStatus2;

	HUINT8	ucExt1;
	HUINT8	ucExt2;
	HUINT8	ucExt3;	// 현재 사용되고 있지 않아서 Cpm ErrorCode를 위한 parameter로 사용한다.
	HUINT8	ucExt4;	// 현재 사용되고 있지 않아서 MR, PPV 등이 있는지를 위한 parameter로 사용한다.
} CxAccessStatus_t;

typedef struct
{
	HUINT32	ulKeyId;
	HUINT8	ucPresentationCounter;
	HUINT8	ucPinChangeAllowed;
	HUINT8	ucPinChosenByUser;
	HUINT8	ucPinSet;
} CxPinStatus_t;

/*******************************************************************/
/********************      Public Functions 	********************/
/*******************************************************************/
HINT32 CXSC_GetCaSysId(HINT32 nScSlot);
HINT32 CXSC_GetSessionCount(HINT32 nScSlot);
HINT32 CXSC_GetCountryIndicator(HINT32 nScSlot);
HINT32 CXSC_GetCardNumber(HINT32 nScSlot);
HINT32 CXSC_GetMaturityRatingLevel(HINT32 nScSlot);
HINT32 CXSC_GetHostVersion(void);
HERROR CXSC_GetSCState(HINT32 lScSlot);
HERROR CXSC_AllSessionReset(void);

HERROR CXSC_ResetSession(HINT32 nScSlot, HUINT8 ucSessionIdx);
HERROR CXSC_SendCat(HINT32 nScSlot, Handle_t hEmm, HUINT8* pucData);
HERROR CXSC_SendEmm(HINT32 nScSlot, HUINT8* pucData);
HERROR CXSC_SendEcm(Handle_t hSession, Handle_t hEcm, HUINT8* pucData);
CxStatusList_t* CXSC_GetCaStatus(HINT32 nScSlot, CxStatusSelectType_t eType, HUINT32 ulRef);
HERROR CXSC_EnterCaPin(Handle_t hSession, HINT32 nScSlot, CxPinCheckType_t eType, HUINT8* pucPin);
HERROR CXSC_ChangeCaPin(HINT32 nScSlot, HUINT8* pucOldPin, HUINT8* pucNewPin);
HERROR CXSC_ChangeSignPin(Handle_t hSession, HUINT32 ulKeyId, HUINT8* pucOldSignPin, HUINT8* pucNewSignPin);
HERROR CXSC_GetSignPinStatus(Handle_t hSession, HUINT32 ulKeyId);
HERROR CXSC_ChangeMaturityRating(HINT32 nScSlot, HUINT8 ucMatRatValue, HUINT8* pucPin);
HERROR CXSC_RequestOrderApdu(Handle_t hSession, HUINT8* pucPin, HUINT32 ulProdId);
HERROR CXSC_OrderResponseApdu(Handle_t hSession, HUINT16 usLen, HUINT8* pucApdu);
HERROR CXSC_ReturnChannelConfig(Handle_t hSession);
HERROR CXSC_RequestDebitLog(Handle_t hSession, HUINT16 usPurseRef, HUINT32 ulCutOffDate);
HERROR CXSC_GenerateSign(Handle_t hSession, HUINT8 ucAlgorithm, HUINT32 ulKeyId, CxSecuData_t* pstDigest, CxSecuData_t* pstSignPin);
HERROR CXSC_VerifySign(Handle_t hSession, HUINT32 ulKeyId, CxSecuData_t* pstDigest, CxSecuData_t* pstSignature);
HERROR CXSC_EncryptMsg(Handle_t hSession, HUINT8 ucAlgorithm, HUINT32 ulKeyId, CxSecuData_t* pstPlainText);
HERROR CXSC_DecryptMsg(Handle_t hSession, HUINT32 ulKeyId, CxSecuData_t* pstCipherText, CxSecuData_t* pstSignPin);
HERROR CXSC_RequestBlockSize(Handle_t hSession, HUINT8 ucAlgorithm, HUINT32 ulKeyId);
HERROR CXSC_RequestCardNumber(HINT32 nScSlot);
HERROR CXSC_RequestSequenceNumber(Handle_t hSession);
HERROR CXSC_RequestRandomOctets(Handle_t hSession, HUINT8 ucRandomLength);

HERROR CXSC_InitCore(void);

HERROR CXSC_ColdReset(HINT32 nScSlot);
HERROR CXSC_RecoveryReset(HINT32 nScSlot, HBOOL bUiDisplay);
HINT32 CXSC_CheckErrorByAccessDenied(void);

#if defined(CONFIG_MW_CAS_CONAX_URI)
HUINT8 CXSC_ConvertUriMrToScMr(eDxCopyrightControl_ConaxMaturityRating_e eUriMr);
eDxCopyrightControl_ConaxMaturityRating_e CXSC_ConvertScMrToUriMr(HUINT8 ucScMr);
HERROR CXSC_GetDefaultUri(DxCopyrightControl_ConaxDrmInfo_t* pstDrmInfo);
HERROR CXSC_SetDefaultUri(Handle_t hSession, Handle_t  hStbHandle);
#endif

#if defined(CONFIG_DEBUG)
HERROR CXSC_PrintUriValue(DxCopyrightControl_ConaxDrmInfo_t* pstDrmInfo);
void CMD_CXSC_PrintScContext(void);
#endif
#endif /* __CXCORE_SC_H__ */

