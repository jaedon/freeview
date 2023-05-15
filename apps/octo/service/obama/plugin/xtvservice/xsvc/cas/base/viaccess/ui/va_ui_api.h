/*
	Viaccess UI API
		User API Manual ACS 3.0 - Doc. Ref. 2218
*/

#ifndef VA_UI_API_H
#define VA_UI_API_H

#include "va_def.h"
#include "va_ui.h"

#define VA_ERR_UI_WRONG_PIN_CODE											(VA_ERR_UI - 1)
#define VA_ERR_UI_LOCKED_PIN_CODE											(VA_ERR_UI - 2)
#define VA_ERR_UI_MAX_CHANGES_PIN_CODE									(VA_ERR_UI - 3)
#define VA_ERR_UI_FORBIDDEN_PIN_CODE										(VA_ERR_UI - 4)
#define VA_ERR_UI_NO_PIN_CODE												(VA_ERR_UI - 5)
#define VA_ERR_UI_SMARTCARD_INSUFFICIENT_MEMORY						(VA_ERR_UI - 6)
#define VA_ERR_UI_WRONG_SLOT												(VA_ERR_UI - 7)
#define VA_ERR_UI_SMARTCARD_ERROR										(VA_ERR_UI - 8)
#define VA_ERR_UI_CONSULTATION_LOCKED									(VA_ERR_UI - 9)
#define VA_ERR_UI_INVALID_MATURITY_RATING								(VA_ERR_UI - 10)
#define VA_ERR_UI_OPERATION_NOT_ACCEPTED								(VA_ERR_UI - 11)
#define VA_ERR_UI_PRESELECTION_AREA_FULL									(VA_ERR_UI - 12)
#define VA_ERR_UI_REFERENCE_ALREADY_STORED_IN_PRESELECTION_AREA		(VA_ERR_UI - 13)
#define VA_ERR_UI_REFERENCE_NOT_FOUND_IN_PRESELECTION_AREA			(VA_ERR_UI - 14)
#define VA_ERR_UI_PRESELECTION_MODE_NOT_ACCEPTED						(VA_ERR_UI - 15)

void VA_UI_Init();

void VA_UI_RegisterEventCallbackFunction();
int VA_UI_GetVersionName(UINT8 *pName);
int VA_UI_GetVersion(tVA_UI_GetVersionRequest *pVersion);
int VA_UI_GetAccessCondition(UINT32 ulAcsId, UINT32 ulStbStreamHandle, UINT32 ulStreamType, tVA_UI_GetAccessConditionRequest *pParam);
int VA_UI_GetSmartcardInfo(UINT32 ulScSlot, tVA_UI_SmartcardInfo *pInfo);
int VA_UI_GetSmartcardListInfo(tVA_UI_GetSmartcardListInfoRequest *pInfo);
int VA_UI_GetUA(UINT32 ulScSlot, tVA_UI_Ua *pUA);
tVA_UI_Soid *VA_UI_GetSoidList(UINT32 ulScSlot, UINT32 *pulCount);
int VA_UI_GetLabel(UINT32 ulScSlot, UINT32 ulSoid, UINT8 *pLabel);
int VA_UI_GetPinCodeAttempts(UINT32 ulScSlot, UINT8 *pucRemainingCount, UINT8 *pucMaxCount);
int VA_UI_ModifyPinCode(UINT32 ulScSlot, UINT8 *pPinCode, UINT8 *pNewPinCode);
int VA_UI_UnlockConsultation(UINT32 ulScSlot, UINT8 *pPinCode);
int VA_UI_GetClassListAll(UINT32 ulScSlot, UINT32 ulSoid, UINT32 *pulCount, tVA_UI_ClassItem **pClass);
int VA_UI_GetClassList(UINT32 ulScSlot, UINT32 ulSoid, tVA_UI_DateRange *pDate, UINT32 *pulCount, tVA_UI_ClassItem **pClass);
int VA_UI_GetThemeLevelListAll(UINT32 ulScSlot, UINT32 ulSoid, UINT32 *pulCount, tVA_UI_ThemeLevelItem **pTheme);
int VA_UI_GetThemeLevelList(UINT32 ulScSlot, UINT32 ulSoid, tVA_UI_DateRange *pDate, UINT32 *pulCount, tVA_UI_ThemeLevelItem **pTheme);
int VA_UI_GetPrebookedListAll(UINT32 ulScSlot, UINT32 ulSoid, UINT32 *pulCount, tVA_UI_ProgramNumberRange **pProgNum);
int VA_UI_GetPrebookedList(UINT32 ulScSlot, UINT32 ulSoid, UINT32 ulMin, UINT32 ulMax, UINT32 *pulCount, tVA_UI_ProgramNumberRange **pProgNum);
int VA_UI_GetPreviewInfo(UINT32 ulScSlot, UINT32 ulSoid, UINT8 ucPreviewIndex, UINT16 usPreviewNumber, UINT8 *ucUsedCwNumber);
int VA_UI_GetMaturityRating(UINT32 ulScSlot, UINT8 *ucMaturityRating, UINT8 *ucMaxMaturityRating);
int VA_UI_ModifyMaturityRating(UINT32 ulScSlot, UINT8 *pPinCode, UINT8 ucMaturityRating);
int VA_UI_AcceptMaturityRating(UINT32 ulAcsId, UINT32 ulScSlot, UINT8 *pPinCode);
int VA_UI_GetPreselection(UINT32 ulScSlot, UINT32 *pulCount, tVA_UI_ReferenceItem **pParam);
int VA_UI_GetOperatorPreselection(UINT32 ulScSlot, UINT32 ulSoid, UINT32 *pulCount, tVA_UI_ReferenceItem **pParam);
int VA_UI_ModifyPreselection(UINT32 ulScSlot, UINT32 ulSoid, UINT8 *pPinCode, UINT32 ulAction, tVA_UI_ReferenceType eType, tVA_UI_ReferenceInfo *pInfo);
int VA_UI_GetGeographicalCodeAddress(UINT32 ulScSlot, UINT32 ulSoid, UINT8 *pGca);
int VA_UI_GetFacData(UINT32 ulScSlot, UINT32 ulSoid, UINT8 ucNP1, UINT8 ucNP2, UINT32 *pulCount, UINT8 *pFac);
int VA_UI_GetFacList(UINT32 ulScSlot, UINT32 ulSoid, UINT32 *pulCount, tVA_UI_FacItem *pFac);
int VA_UI_SubmitExm(UINT32 ulScSlot, UINT32 ulExmType, UINT32 ulSize, UINT8 *pExm, UINT32 *pulStatus, UINT32 *pulCount, UINT8 *pReply);
int VA_UI_GetOperatorMessage(UINT32 ulScSlot, UINT32 *pulSoid, UINT32 ulPriority, UINT32 *pulEncode, UINT32 *pulCount, UINT8 *pMsg);
int VA_UI_GetOperatorData(UINT32 ulScSlot, UINT32 *pulSoid, UINT32 ulPriority, UINT32 *pulCount, UINT8 *pData);
int VA_UI_GetCamlockInfo(UINT32 *pulState, UINT8 *pCamlock);
int VA_UI_GetPairingMode(UINT32 *pulMode);
int VA_UI_GetPairingSmartcard(UINT32 *pulMaxNum, UINT32 *pulNum);
 tVA_UI_Ua *VA_UI_GetPairingUaList(UINT32 *pulUaNum);
int VA_UI_GetStbId(UINT8 *pStbId, UINT32 *pbIsDateValid, tVA_UI_DateEx *pDate);
int VA_UI_GetTraceOutput(UINT32 *pulType, UINT32 *pulMode, UINT32 *pulHeaderType, UINT32 *pulLineWidth, tVA_UI_TraceOutputParameters *pParam);
int VA_UI_SetTraceOutput(UINT32 ulType, UINT32 ulMode, UINT32 ulHeaderType, UINT32 ulLineWidth, tVA_UI_TraceOutputParameters *pParam);
int VA_UI_GetTraceList(UINT32 *pulCount, tVA_UI_TraceModule **pTrace);
int VA_UI_SetTraceLevel(tVA_UI_TraceModule *pTrace);
int VA_UI_FlushTrace();

#ifdef CONFIG_MW_CAS_VIACCESS_IPPV

#define IPPV_T		0
#define IPPV_P		1

int VA_UI_GetIPPVListAll(UINT32 ulScSlot, UINT32 ulSoid, UINT32 ulIppvType, UINT32 *pulCount, tVA_UI_IppvItem **pIppvItem);
int VA_UI_GetIPPVList(UINT32 ulScSlot, UINT32 ulSoid, UINT32 ulIppvType, UINT32 ulMin, UINT32 ulMax, UINT32 *pulCount, tVA_UI_IppvItem **pIppvItem);
int VA_UI_GetCreditInfo(UINT32 ulScSlot, UINT32 ulSoid, tVA_UI_GetCreditInfoRequest *pInfo);
int VA_UI_GetIPPVInfo(UINT32 ulAcsId, UINT32 ulScSlot, UINT32 ulStbStreamHandle, tVA_UI_GetIppvInfoRequest *pInfo);
int VA_UI_AcceptIPPVP(UINT32 ulAcsId, UINT32 ulScSlot, UINT32 ulStbStreamHandle, UINT8 *pPinCode, UINT32 ulProgNum);
int VA_UI_AcceptIPPVT(UINT32 ulAcsId, UINT32 ulScSlot, UINT32 ulStbStreamHandle, UINT8 *pPinCode, UINT32 ulProgNum, tVA_UI_Price stCeiling);
int VA_UI_GetThreshold(UINT32 ulScSlot, tVA_UI_Price *pstThreshold);
int VA_UI_ModifyThreshold(UINT32 ulScSlot, UINT8 *pPinCode,  tVA_UI_Price stThreshold);
#ifdef CONFIG_MW_CAS_VIACCESS_IPPV_SURVEY
int VA_UI_StartIPPVSurvey(UINT32 ulScSlot, UINT32 ulSoid);
int VA_UI_CancelIPPVSurvey();
#endif
#endif

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
int VA_UI_GetRecordInfo(UINT32 ulAcsId, UINT32 ulStbStreamHandle, tVA_UI_Date *pDate, UINT32 *pulPlaybackDuration, UINT32 *pulMaxNumberOfPlayback);
int VA_UI_GetPlaybackInfo(UINT32 ulAcsId, HUINT32 *pulPlaybackCounter, tVA_UI_DateEx *pRecordDate, tVA_UI_DateEx *pFirstPlaybackDate);
int VA_UI_IncrementPlaybackCount(UINT32 ulAcsId);
#endif

#endif //VA_UI_API_H

