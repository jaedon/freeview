/**************************************************************************************************/
#define ________Nagra_mgr_Private_Include__________________________________________________
/**************************************************************************************************/
#include "namgr_main.h"
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
//#include <cas_aps_mgr.h>
#include <cas_ses_mgr.h>
#endif


#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
#include "na_mgr_cak_ppv.h"
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL) ||defined(CONFIG_NAGRA_IRDCMD_POPUP)
#include <na_mgr_cak_irdcmd.h>
#endif

#include "na_mgr_cak_pairingdata.h"

#include <_svc_cas_mgr_common.h>
#include <_svc_cas_sub_svc_api.h>

#include<svc_pipe.h>

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST) && !defined(CONFIG_DEBUG)
/*
	DLK (cardless) 의 경우 Production SW에도 DLK library가 Uart를 사용할 수 있게 해야 함.
	Nagra에서 생산된 물건을 가지고 debugging을 할 때 Card모델의 경우 Card interface를 통해 하지만
	cardless 모델의 경우 Card interface가 없으므로 Uart를 통해 직접 Library와 연결하여 통신해야 한다.
	실제 나그라에서 자기들 분석 툴을 통해 library와 통신하여 정상 동작하는 지를 확인 하고 있음.
*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#endif

/**************************************************************************************************/
#define ________Nagra_mgr_Golbal_Value____________________________________________________
/**************************************************************************************************/

extern HERROR CA_NA_CakMgrInitialization(void);
extern HERROR CA_NA_CakSvcInitialization(void);
extern HERROR CA_NA_CakRegisterCaPopUpProc(TCaPopupCallback callback);
extern HERROR CA_NA_CakSvcStartup(void);
extern HERROR CA_NA_CakMgrStartUp(void);
extern HERROR CA_NA_GetCscMaxIndex(TUnsignedInt8* pxCscMaxIndex);
extern HERROR CA_NA_PsiGetCasInstanceID(Handle_t *phCasHandle);
extern HERROR CA_NA_RegisterCtrlList(void);
extern HERROR xmgr_cas_NaResetIRDCmdData(void);
extern HERROR xmgr_cas_NaIrdCmdEmmGetWakeupTime(UNIXTIME *pulTime);

/**************************************************************************************************/
#define ________Nagra_mgr_Private_Define___________________________________________________
/**************************************************************************************************/





#define MAX_CMD_TYPE					(eCmd_End)

#define MAX_MSG_POOL_SIZE				(25)
#define MAX_CMD_POOL_SIZE				(10)

#define VIDEO_BIT_MASK					(0x01)
#define AUDIO_BIT_MASK					(0x02)

#define WAIT_FOR_SEM(x)				VK_SEM_Get(x);
#define RELEASE_SEM(x)					VK_SEM_Release(x);


/**************************************************************************************************/
#define ________Nagra_mgr_Private_Type_Define_____________________________________________
/**************************************************************************************************/
typedef enum tagNaBufferSize
{
	eAlarmBufferSize 	= MAX_MSG_POOL_SIZE,
	eCmdBufferSize 		= MAX_MSG_POOL_SIZE,

	eBdcBufferSize 		= MAX_CMD_POOL_SIZE,
	eIrdCmdBufferSize 	= MAX_CMD_POOL_SIZE,
	eDrmCopBufferSize 	= MAX_CMD_POOL_SIZE
} NaBufferSize_t;

typedef enum tagNaBufferIndex
{
	eAlarmBufferIdx = 0,
	eCmdBufferIdx,

	eBdcBufferIdx,
	eIrdCmdBufferIdx,
	eDrmCopBufferIdx,
	eEndBufferIdx
} NaBufferIndex_t;

typedef struct tagNaBufferInfo
{
	TUnsignedInt8 ucIdxOfSize[eEndBufferIdx];
	TUnsignedInt8 ucIdxOfValue[eEndBufferIdx];
} NaBufferInfo_t;


/**************************************************************************************************/
#define ________Nagra_mgr_Private_Static_Value______________________________________________
/**************************************************************************************************/
static TUnsignedInt32		s_ulNaMgrInt = CAS_NOT_INITAILIZED;

static TUnsignedInt32		s_ulNaMgrSem;
static NaBufferInfo_t		s_stNaBufferInfo;

static NaScAlarmItem_t 		s_stScInfoList[NO_SMC_SLOT];
static NaAccessAlarmList_t	s_stAccessInfoList[MAX_NO_SESSION];
static NaAlarmData_t 		s_stNaAlarmData[MAX_MSG_POOL_SIZE];

#if 1
static NaBdcItem_t			s_stBdcData[MAX_CMD_POOL_SIZE];
#endif

static NaIrdCmdItem_t		s_stIrdCmdData[MAX_CMD_POOL_SIZE];
static NaDrmCopItem_t		s_stDrmCopData[MAX_CMD_POOL_SIZE];
static NaCmdData_t			s_stCmdInfoList[MAX_MSG_POOL_SIZE];

#if defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
static HBOOL		s_bTivuService = FALSE;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES) //DRM_TEST
static DxCopyrightControl_t			s_stDrmInfo[CAS_MAX_NUM_OF_ACTION];
static int SendDrmCnt = 0;
#endif

static HUINT8				s_aucPairingData[MAX_PD_SIZE];

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST) && !defined(CONFIG_DEBUG)
static HINT32	s_ulTtyFd = 0;
#endif

/**************************************************************************************************/
#define ________Nagra_mgr_Private_Static_Prototype__________________________________________
/**************************************************************************************************/
static HERROR 		_naMgr_InitMgr(void);

static HERROR 		_naMgr_GetSlotMsgBuffer(NaBufferIndex_t eBuffIdx, TUnsignedInt8 *pucSlot);

static HERROR 		_naMgr_CheckAlarmMsg(TCaAccess eAccessAlarm);
static HERROR 		_naMgr_GetMsgFromScAlarmData(TUnsignedInt8 xNumberOfSmartcardAlarms, const NaSmartcardStatus_t *pxSmartcardAlarmArray, NaAlarmData_t *pstNaAlarmData);
static HERROR 		_naMgr_GetMsgFromAccessAlarmData(TUnsignedInt8 xNumberOfAccesAlarms, const NaAccessAlarm_t *pxAccessAlarmArray, NaAlarmData_t *pstNaAlarmData);
static TCaStatus 		_naMgr_ProcAlarmExportation(TUnsignedInt8 xNumberOfSmartcardAlarms, const NaSmartcardStatus_t *pxSmartcardAlarmArray,
																TUnsignedInt8 xNumberOfAccesAlarms, const NaAccessAlarm_t *pxAccessAlarmArray);
static TBdcStatus 		_naMgr_procDisplayPopUp(TBdcPopUpPersistence xPersistence, TUnsignedInt8 xNumLanguages, const TBdcPopUpContent *pxContent);
static TBdcStatus 		_naMgr_procRemovePopUp(void);

static void 			_naMgr_procIrdCommand(NA_IRDCMD_ID_t eCmdId, NA_IRDCMD_OPCODE_t eCmdOpCode, TUnsignedInt32 ulParam1, TUnsignedInt32 ulParam2);
static HERROR 		_naMgr_GetLastAlarmData(TUnsignedInt32 hAction, SvcCas_MsgType_e *peMsgType, DxCAS_GroupId_e *peCasId, void **ppMessage, SvcCas_ShowType_e *peShowType);
static HERROR 		_naMgr_GetCurrScStatus(TUnsignedInt32 ulSlotId, TSmartcardState *peScAlarm);
static HERROR 		_naMgr_ReportCurrScStatus(TUnsignedInt32 hAction);
static HERROR 		_naMgr_ReportCurrAlarmData(void);

static HERROR 		_naMgr_SetSessionStatus(TUnsignedInt32 hAction, TBoolean bRunStatus);

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
static HERROR 		_naMgr_IsHdPlusSmartCard(TUnsignedInt8 ucSlotId, TBoolean *pbHdPlusSc);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SKYD) || defined(CONFIG_MW_CAS_NAGRA_OP_KDG)
static HERROR 		_naMgr_CheckSmardStatus(TIccSessionId iccSessionId);
#endif


/**************************************************************************************************/
#define ________Nagra_mgr_Private_Static_Prototype_Body_____________________________________
/**************************************************************************************************/
static HERROR _naMgr_InitMgr(void)
{
	TUnsignedInt16	i;
	HUINT8			*pucPdBuffer;
	HUINT32			ulPdSize;

	HxSTD_memset(&s_stNaBufferInfo, 0x00, sizeof(NaBufferInfo_t));

	s_stNaBufferInfo.ucIdxOfSize[0] = eAlarmBufferSize;
	s_stNaBufferInfo.ucIdxOfSize[1] = eCmdBufferSize;

	s_stNaBufferInfo.ucIdxOfSize[2] = eBdcBufferSize;
	s_stNaBufferInfo.ucIdxOfSize[3] = eIrdCmdBufferSize;
	s_stNaBufferInfo.ucIdxOfSize[4] = eDrmCopBufferSize;

	for (i=0; i<NO_SMC_SLOT; i++)
		HxSTD_memset(&s_stScInfoList[i], 0x00, sizeof(NaScAlarmItem_t));
	for (i=0; i<MAX_NO_SESSION; i++)
		HxSTD_memset(&s_stAccessInfoList[i], 0x00, sizeof(NaAccessAlarmList_t));
	for (i=0; i<MAX_MSG_POOL_SIZE; i++)
		HxSTD_memset(&s_stNaAlarmData[i], 0x00, sizeof(NaAlarmData_t));
	for (i=0; i<MAX_CMD_POOL_SIZE; i++)
	{
		#if 1
		HxSTD_memset(&s_stBdcData[i], 0x00, sizeof(NaBdcItem_t));
		#endif

		HxSTD_memset(&s_stIrdCmdData[i], 0x00, sizeof(NaIrdCmdItem_t));
		HxSTD_memset(&s_stDrmCopData[i], 0x00, sizeof(NaDrmCopItem_t));
	}
	for (i=0; i<MAX_MSG_POOL_SIZE; i++)
		HxSTD_memset(&s_stCmdInfoList[i], 0x00, sizeof(NaCmdData_t));

	if (VK_SEM_Create(&s_ulNaMgrSem, "nagraMgr", VK_SUSPENDTYPE_PRIORITY) != ERR_OK)
	{
		HxLOG_Error("error in VK_SEM_Create...\n");
		return ERR_FAIL;
	}

	if(HLIB_EXTRA_GetSpecialMemory(HxEXTRA_MEM_NaPd, &pucPdBuffer, &ulPdSize) != ERR_OK)
	{
		HxLOG_Error("HLIB_EXTRA_GetSpecialMemory() error\n");
		return ERR_FAIL;
	}

#if defined(TEMP_SW_PAIRING_KEY)
	// na_mgr_cak_pairingdata.h에 모델별로 적용된 데이터를 사용
	HxSTD_memcpy(s_aucPairingData, s_aucNaPairingData, MAX_PD_SIZE);
#else
	if(ulPdSize > MAX_PD_SIZE)
	{
		HxLOG_Warning("Check SpecialMemory Table...(%d, %d)\n", ulPdSize, MAX_PD_SIZE);
		ulPdSize = MAX_PD_SIZE;
	}

	if(pucPdBuffer != NULL)
	{
		HxSTD_memcpy(s_aucPairingData, pucPdBuffer, ulPdSize);
	}
#endif

	HLIB_EXTRA_ReleaseSpecialMemory(HxEXTRA_MEM_NaPd);

	return ERR_OK;
}


static HERROR _naMgr_GetSlotMsgBuffer(NaBufferIndex_t eBuffIdx, TUnsignedInt8 *pucSlot)
{
	if (pucSlot == NULL)
		return ERR_FAIL;

	if ( (eBuffIdx < eAlarmBufferIdx) || (eBuffIdx > eDrmCopBufferIdx) )
		return ERR_FAIL;

	if (s_stNaBufferInfo.ucIdxOfValue[eBuffIdx]++ >= s_stNaBufferInfo.ucIdxOfSize[eBuffIdx]-1)
		s_stNaBufferInfo.ucIdxOfValue[eBuffIdx] = 0;
	*pucSlot = s_stNaBufferInfo.ucIdxOfValue[eBuffIdx];
	return ERR_OK;
}


static HERROR _naMgr_CheckAlarmMsg(TCaAccess eAccessAlarm)
{
	if ( (eAccessAlarm == CA_ACCESS_DENIED)
		|| (eAccessAlarm == CA_ACCESS_NO_VALID_SMARTCARD)
		|| (eAccessAlarm == CA_ACCESS_SMARTCARD_BLACKLISTED)
		|| (eAccessAlarm == CA_ACCESS_SMARTCARD_SUSPENDED)
		|| (eAccessAlarm == CA_ACCESS_BLACKED_OUT)
		|| (eAccessAlarm == CA_ACCESS_DENIED_NO_VALID_CREDIT)
		|| (eAccessAlarm == CA_ACCESS_DENIED_COPY_PROTECTED)
		|| (eAccessAlarm == CA_ACCESS_DENIED_PARENTAL_CONTROL)
		|| (eAccessAlarm == CA_ACCESS_DENIED_DIALOG_REQUIRED)
		|| (eAccessAlarm == CA_ACCESS_DENIED_PAIRING_REQUIRED)
		|| (eAccessAlarm == CA_ACCESS_DENIED_CHIPSET_PAIRING_REQUIRED)
		|| (eAccessAlarm == CA_ACCESS_DENIED_FOR_PARENTAL_RATING)
		|| (eAccessAlarm == CA_ACCESS_RIGHT_SUSPENDED)
		|| (eAccessAlarm == CA_ACCESS_DENIED_BUT_PPT) )
	{
		HxLOG_Error("Access alarm (0x%x)\n", eAccessAlarm);
		return ERR_FAIL;
	}
	return ERR_OK;
}


static HERROR _naMgr_GetMsgFromScAlarmData(TUnsignedInt8 xNumberOfSmartcardAlarms, const NaSmartcardStatus_t *pxSmartcardAlarmArray, NaAlarmData_t *pstNaAlarmData)
{
	TUnsignedInt8 i;
	TUnsignedInt8 ucCnt = 0;

	if (pstNaAlarmData == NULL)
	{
		HxLOG_Error("target is null...\n");
		return ERR_FAIL;
	}

	if ( (pxSmartcardAlarmArray == NULL) || (xNumberOfSmartcardAlarms <= 0) )
	{
		HxLOG_Error("targets are null...\n");
		return ERR_FAIL;
	}

	WAIT_FOR_SEM(s_ulNaMgrSem)
	HxSTD_memset(pstNaAlarmData, 0x00, sizeof(NaAlarmData_t));

	// Smart Card Alarm에 우선순위를 둔다.
	for (i=0; i<xNumberOfSmartcardAlarms && i<NO_SMC_SLOT;  i++)
	{
		s_stScInfoList[i].ulSlotId 	= pxSmartcardAlarmArray[i].iccSessionId;
		s_stScInfoList[i].bChanged = pxSmartcardAlarmArray[i].changed;
		s_stScInfoList[i].eAlarm 	= pxSmartcardAlarmArray[i].alarm;

		if (pxSmartcardAlarmArray[i].alarm != CA_SMARTCARD_STATE_OK)
		{
			ucCnt++;
		}
	}

	if (ucCnt == NO_SMC_SLOT)
	{
		HxLOG_Print("it is sc alarm error (%d)\n", ucCnt);
		pstNaAlarmData->hAction 			= 0;
		pstNaAlarmData->eAlarmType 			= (NaAlarmType_t)eAlarm_Smartcard;
		pstNaAlarmData->ucNumOfScAlarms 	= xNumberOfSmartcardAlarms;
		pstNaAlarmData->pstScAlarmItems 	= (NaScAlarmItem_t *)&s_stScInfoList;
		RELEASE_SEM(s_ulNaMgrSem)
		return ERR_OK;
	}
	RELEASE_SEM(s_ulNaMgrSem)
	return ERR_FAIL;
}


static HERROR _naMgr_GetMsgFromAccessAlarmData(TUnsignedInt8 xNumberOfAccesAlarms, const NaAccessAlarm_t *pxAccessAlarmArray, NaAlarmData_t *pstNaAlarmData)
{
	TUnsignedInt8 	i;
	TUnsignedInt8  	ucTot = 0;
	TUnsignedInt8 	ucCnt = 0;

	TUnsignedInt32	hAction;
	TUnsignedInt32	ulActionId;

	TBoolean			bFristAlarm = FALSE;

	if (pstNaAlarmData == NULL || pxAccessAlarmArray == NULL || xNumberOfAccesAlarms <= 0)
	{
		HxLOG_Error("target is null...\n");
		return ERR_FAIL;
	}

	WAIT_FOR_SEM(s_ulNaMgrSem)
	HxSTD_memset(pstNaAlarmData, 0x00, sizeof(NaAlarmData_t));
	for (i=0; i<xNumberOfAccesAlarms; i++)
	{
		if (pxAccessAlarmArray[i].changed == FALSE)
		{
			continue;
		}

		if (!bFristAlarm)
		{
			bFristAlarm = TRUE;
			hAction = pxAccessAlarmArray[i].transportSessionId;
			ulActionId = svc_cas_SubSvcRmGetActionId(pxAccessAlarmArray[i].transportSessionId);
			if ( (ulActionId < 0) || (ulActionId >= MAX_NO_SESSION) )
			{
				HxLOG_Error("over of index (0x%x)\n", ulActionId);
				RELEASE_SEM(s_ulNaMgrSem)
				return ERR_FAIL;
			}

			s_stAccessInfoList[ulActionId].ucNumOfStreams = 0;
			HxSTD_memset(&s_stAccessInfoList[ulActionId].stAccessAlarmItems, 0x00, (sizeof(NaAccessAlarmItem_t) * MAX_TOTAL_NO_STREAM));
		}

		ucTot++;
		if (_naMgr_CheckAlarmMsg(pxAccessAlarmArray[i].alarm) != ERR_OK)
		{
			if (ucCnt < MAX_TOTAL_NO_STREAM)
			{
				s_stAccessInfoList[ulActionId].ucNumOfStreams++;
				s_stAccessInfoList[ulActionId].stAccessAlarmItems[ucCnt].bChanged = pxAccessAlarmArray[i].changed;
				s_stAccessInfoList[ulActionId].stAccessAlarmItems[ucCnt++].eAlarm = pxAccessAlarmArray[i].alarm;
			}
		}
	}

	if (ucTot > 0)
	{
		pstNaAlarmData->hAction = hAction;
		pstNaAlarmData->ucNumOfAccessAlarms = ucCnt;

		if (ucCnt > 0)
		{
			s_stAccessInfoList[ulActionId].eAlarmType = eAlarm_Access;
		}
		else
		{
			s_stAccessInfoList[ulActionId].eAlarmType = eAlarm_None;
		}

		if (ucCnt > 0)
		{
			pstNaAlarmData->eAlarmType = eAlarm_Access;
			pstNaAlarmData->pstAccessAlarmItems = s_stAccessInfoList[ulActionId].stAccessAlarmItems;
		}
		else
		{
			pstNaAlarmData->eAlarmType = eAlarm_None;
			pstNaAlarmData->pstAccessAlarmItems = NULL;
		}
		RELEASE_SEM(s_ulNaMgrSem)
		return ERR_OK;
	}
	RELEASE_SEM(s_ulNaMgrSem)
	return ERR_FAIL;
}


static TCaStatus _naMgr_ProcAlarmExportation(TUnsignedInt8 xNumberOfSmartcardAlarms, const NaSmartcardStatus_t *pxSmartcardAlarmArray,
														TUnsignedInt8 xNumberOfAccesAlarms, const NaAccessAlarm_t *pxAccessAlarmArray)
{
	TUnsignedInt8				i;
	TUnsignedInt8				ucMsgSlot;
	TUnsignedInt32			ulActionId;

	NaAlarmData_t			stNaAlarmData;

	SvcCas_MsgType_e			eMsgType = eCAS_MSG_TYPE_Ok;
	SvcCas_ShowType_e			eShowType = eCasShowType_Both;

	static TUnsignedInt8		s_ucNumOfAccessAlarms[MAX_NO_SESSION];
	static NaAccessAlarm_t		s_stAccessAlarmsList[MAX_NO_SESSION][MAX_NO_STREAM];

#if defined(CONFIG_DEBUG)
	for(i = 0 ; i < xNumberOfSmartcardAlarms ; i++)
		HxLOG_Error(HxANSI_COLOR_PURPLE("SMARTCARD, id(%d), alarm(%s)")"\n", pxSmartcardAlarmArray[i].iccSessionId, NAMGR_GetStrSmartcardState(pxSmartcardAlarmArray[i].alarm));

	for(i = 0 ; i < xNumberOfAccesAlarms ; i++)
		HxLOG_Error(HxANSI_COLOR_PURPLE("ACCESS, id(0x%x), alarm(%s)")"\n", pxAccessAlarmArray[i].transportSessionId, NAMGR_GetStrAccessState(pxAccessAlarmArray[i].alarm));
#endif

#if (CONFIG_MW_CAS_NAGRA_PREDSC_TEST == 0)
	// 1) Sc alarm부터 확인
	if (_naMgr_GetMsgFromScAlarmData(xNumberOfSmartcardAlarms,
											pxSmartcardAlarmArray,
											&stNaAlarmData) == ERR_OK)
	{
		eMsgType = eCAS_MSG_TYPE_Fail;
		eShowType = eCasShowType_None;

#if (CONFIG_FOR_DAL_TEST_DEF == 0)
		for (i=0; i<MAX_NO_SESSION; i++)
		{
			if (s_stAccessInfoList[i].bRunStatus == TRUE)
			{
				_naMgr_GetSlotMsgBuffer(eAlarmBufferIdx, &ucMsgSlot);
				HxLOG_Print("[%s:%d] SC ALARM : Slot No (%d)\n", __FUNCTION__, __HxLINE__, ucMsgSlot);
				HxSTD_memcpy(&s_stNaAlarmData[ucMsgSlot], &stNaAlarmData, sizeof(NaAlarmData_t));

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
				if (CASMGR_APS_UpdateCasMessage(s_stAccessInfoList[i].hAction,
												eMsgType,
												eDxCAS_GROUPID_NA,
												(void*)&s_stNaAlarmData[ucMsgSlot],
												eShowType) != ERR_OK)
#else
				if (svc_cas_MgrSubUpdateCasMessage(s_stAccessInfoList[i].hAction,
												eMsgType,
												eDxCAS_GROUPID_NA,
												(void*)&s_stNaAlarmData[ucMsgSlot],
												eShowType) != ERR_OK)
#endif
				{
					// error일시 memory free하도록 하자.
					HxLOG_Error("error in svc_cas_MgrSubUpdateCasMessage\n");
					return CA_ERROR;
				}

			}
		}
#endif
		return CA_NO_ERROR;
	}
	else
	{
		if (xNumberOfAccesAlarms <= 0)
		{
			/* sc 삽입시 일단 sc ok가 올라오므로 이후 access error, sc error일 경우 화면 꺼벅거림 문제 해결 */
			/* sc ok는 menu에서만 관심 - callback에서 받아서 처리하므로 여기서는 올릴 필요 없슴. */
			/* stream 이 있는 경우 access alarm 올라옴 */
#if defined(CONFIG_MW_CAS_NAGRA_OP_UPC)
			for (i=0; i<MAX_NO_SESSION; i++)
			{
				if (s_stAccessInfoList[i].bRunStatus == TRUE)
				{
					SvcCas_ChannelType_e eCasChType;
					if (CASMGR_GetCasChannelType(s_stAccessInfoList[i].hAction, &eCasChType) == ERR_OK)
					{
						HxLOG_Print("eCasChType = 0x%x\n", eCasChType);
						if (eCasChType != eCasChannel_Scramble)
						{
							if (svc_cas_MgrSubUpdateCasMessage(s_stAccessInfoList[i].hAction,
											eCAS_MSG_TYPE_Ok,
											eDxCAS_GROUPID_NA,
											NULL,
											eCasShowType_Both) != ERR_OK)
							{
								// error일시 memory free하도록 하자.
								HxLOG_Error("error in svc_cas_MgrSubUpdateCasMessage\n");
							}
						}
					}
				}
			}
#endif
			return CA_NO_ERROR;
		}
	}
#endif

	// 2) Access alarm 확인
	HxSTD_memset(s_ucNumOfAccessAlarms, 0x00, (sizeof(TUnsignedInt8) * MAX_NO_SESSION));
	HxSTD_memset(&s_stAccessAlarmsList, 0x00, (sizeof(NaAccessAlarm_t) * MAX_NO_SESSION * MAX_NO_STREAM));
	for (i=0; i<xNumberOfAccesAlarms; i++)
	{
		ulActionId = svc_cas_SubSvcRmGetActionId(pxAccessAlarmArray[i].transportSessionId);
		if ( (ulActionId < 0) || (ulActionId >= (TUnsignedInt32)MAX_NO_SESSION) )
			continue;

		if (s_ucNumOfAccessAlarms[ulActionId] < MAX_TOTAL_NO_STREAM)
		{
			HxSTD_memcpy(&s_stAccessAlarmsList[ulActionId][s_ucNumOfAccessAlarms[ulActionId]], &pxAccessAlarmArray[i], sizeof(TAccessAlarmItem));
			s_ucNumOfAccessAlarms[ulActionId]++;
		}
	}

	for (i=0; i<MAX_NO_SESSION; i++)
	{
		if (s_ucNumOfAccessAlarms[i] <= 0)
			continue;

		if (_naMgr_GetMsgFromAccessAlarmData(s_ucNumOfAccessAlarms[i],
													s_stAccessAlarmsList[i],
													&stNaAlarmData) != ERR_OK)
		{

			HxLOG_Error("error in _naMgr_GetMsgFromAccessAlarmData\n");
			continue;
		}

		if (stNaAlarmData.eAlarmType == eAlarm_Access)
		{
			eMsgType = eCAS_MSG_TYPE_Fail;
			eShowType = eCasShowType_None;
		}
		else if (stNaAlarmData.eAlarmType == eAlarm_None)
		{
			eMsgType = eCAS_MSG_TYPE_Ok;
			eShowType = eCasShowType_Both;
		}

		if (s_stAccessInfoList[i].bRunStatus != TRUE)
		{
			HxLOG_Info("This session is already stopped Index (%d),  Action Handle (0x%x)\n", i, s_stAccessInfoList[i].hAction);
			continue;
		}

		if (s_stAccessInfoList[i].hAction != stNaAlarmData.hAction)
		{
			HxLOG_Error("Wrong Action Handle (0x%x, 0x%x)\n", s_stAccessInfoList[i].hAction, stNaAlarmData.hAction);
			continue;
		}

#if (CONFIG_FOR_DAL_TEST_DEF == 0)
		_naMgr_GetSlotMsgBuffer(eAlarmBufferIdx, &ucMsgSlot);
		HxLOG_Print("[%s:%d] ACCESS ALARM : Slot No (%d)\n", __FUNCTION__, __HxLINE__, ucMsgSlot);
		HxSTD_memcpy(&s_stNaAlarmData[ucMsgSlot], &stNaAlarmData, sizeof(NaAlarmData_t));
	#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
		if (CASMGR_APS_UpdateCasMessage(s_stAccessInfoList[i].hAction,
										eMsgType,
										eDxCAS_GROUPID_NA,
										(void*)&s_stNaAlarmData[ucMsgSlot],
										eShowType) != ERR_OK)
	#else
		if (svc_cas_MgrSubUpdateCasMessage(s_stAccessInfoList[i].hAction,
										eMsgType,
										eDxCAS_GROUPID_NA,
										(void*)&s_stNaAlarmData[ucMsgSlot],
										eShowType) != ERR_OK)
	#endif
		{
			// error일시 memory free하도록 하자.
			HxLOG_Error("error in svc_cas_MgrSubUpdateCasMessage\n");
		}

	#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
	if(s_stAccessInfoList[i].stAccessAlarmItems[0].eAlarm == CA_ACCESS_DENIED_PARENTAL_CONTROL)
	{
		SvcCas_ActionType_e eActType = NAMGR_CAKSVC_GetActionTypeByActionHandle(s_stAccessInfoList[i].hAction);

		switch(eActType)
		{
			case eCAS_ActionType_Live:
			case eCAS_ActionType_Tsr_Rec:
			case eCAS_ActionType_Recording:
				NAMGR_CAKSVC_SetParentalControl(s_stAccessInfoList[i].hAction, FALSE);
				break;

			default:
				break;
		}
	}
	#endif
#endif
	}
	return CA_NO_ERROR;
}

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
static void _naMgr_procPopupCallback(NaPopupUI_t *pstPopupUi)
{
	HUINT8			ucCmdSlot;
	Handle_t		hAction;
	NaPopupItem_t	*pstItem;

	hAction = SVC_PIPE_GetActionHandle(eActionId_VIEW_0);

	if(pstPopupUi == NULL)
		return;

	/* header file을 참조하면 NaPopupItem_t가 NaPopupUI_t로 재정의 되어 있다. 틀려질 경우 코드 수정이 필요한 상황이다. */
	pstItem = (NaPopupItem_t *)pstPopupUi;

	_naMgr_GetSlotMsgBuffer(eCmdBufferIdx, &ucCmdSlot);

	s_stCmdInfoList[ucCmdSlot].hAction = hAction;
	s_stCmdInfoList[ucCmdSlot].eCmdType = eCmd_PopUp;
	s_stCmdInfoList[ucCmdSlot].item.pstPopupItem = pstItem;

	if (svc_cas_MgrSubUpdateCasMessage(hAction,
									eCAS_MSG_TYPE_Command,
									eDxCAS_GROUPID_NA,
									&s_stCmdInfoList[ucCmdSlot],
									eCasShowType_Both) != ERR_OK)
	{
		HxLOG_Error("error in svc_cas_MgrSubUpdateCasMessage\n");
		if(pstItem->pucMsg != NULL)
			OxCAS_Free(pstItem->pucMsg);

		if(pstItem != NULL)
			OxCAS_Free(pstItem);
		return;
	}
}
#endif

static TBdcStatus _naMgr_procDisplayPopUp(TBdcPopUpPersistence xPersistence, TUnsignedInt8 xNumLanguages, const TBdcPopUpContent *pxContent)
{
	TUnsignedInt8		i;

	TUnsignedInt8		ucMsgSlot;
	TUnsignedInt8		ucCmdSlot;

	TUnsignedInt32 		hAction = 0;

	SvcCas_MsgType_e		eMsgType = eCAS_MSG_TYPE_Command;
	SvcCas_ShowType_e		eShowType = eCasShowType_Both;

	hAction = SVC_PIPE_GetActionHandle(eActionId_VIEW_0);

	_naMgr_GetSlotMsgBuffer(eBdcBufferIdx, &ucMsgSlot);
	HxSTD_memset(&s_stBdcData[ucMsgSlot], 0x00, sizeof(NaBdcItem_t));

	s_stBdcData[ucMsgSlot].eBdcCmd = eBdc_Display;
	s_stBdcData[ucMsgSlot].eBdcType = xPersistence;
	s_stBdcData[ucMsgSlot].ucNumOfLang = xNumLanguages;

	if (xNumLanguages > 0)
	{
		for (i=0; i<xNumLanguages && i<MAX_BDC_LANGUAGE; i++)
		{
			HxSTD_memcpy((TChar*)s_stBdcData[ucMsgSlot].stBdcContent[i].xLang, (TChar*)pxContent[i].language, 3);
			s_stBdcData[ucMsgSlot].stBdcContent[i].ulNumBytes = (pxContent[i].numBytes <= MAX_BDC_STRING_SIZE) ? pxContent[i].numBytes : MAX_BDC_STRING_SIZE;
			HxSTD_memcpy((TChar*)s_stBdcData[ucMsgSlot].stBdcContent[i].ucText, (TChar*)pxContent[i].pContent, s_stBdcData[ucMsgSlot].stBdcContent[i].ulNumBytes);
		}
	}

	_naMgr_GetSlotMsgBuffer(eCmdBufferIdx, &ucCmdSlot);
	s_stCmdInfoList[ucCmdSlot].hAction = hAction;
	s_stCmdInfoList[ucCmdSlot].eCmdType = eCmd_Bdc;
	s_stCmdInfoList[ucCmdSlot].item.pstBdcItem = &s_stBdcData[ucMsgSlot];

	if (svc_cas_MgrSubUpdateCasMessage(hAction,
									eMsgType,
									eDxCAS_GROUPID_NA,
									(void*)&s_stCmdInfoList[ucCmdSlot],
									eShowType) != ERR_OK)
	{
		// error일시 memory free하도록 하자.
		HxLOG_Error(" error in svc_cas_MgrSubUpdateCasMessage\n");
		return BDC_ERROR;
	}
	return BDC_NO_ERROR;
}

static TBdcStatus _naMgr_procRemovePopUp(void)
{
	TUnsignedInt32 		hAction = 0;

	TUnsignedInt8			ucMsgSlot;
	TUnsignedInt8			ucCmdSlot;

	SvcCas_MsgType_e		eMsgType = eCAS_MSG_TYPE_Command;
	SvcCas_ShowType_e		eShowType = eCasShowType_Both;

	hAction = SVC_PIPE_GetActionHandle(eActionId_VIEW_0);

	_naMgr_GetSlotMsgBuffer(eBdcBufferIdx, &ucMsgSlot);
	HxSTD_memset(&s_stBdcData[ucMsgSlot], 0x00, sizeof(NaBdcItem_t));

	s_stBdcData[ucMsgSlot].eBdcCmd = eBdc_Remove;

	_naMgr_GetSlotMsgBuffer(eCmdBufferIdx, &ucCmdSlot);
	s_stCmdInfoList[ucCmdSlot].hAction = hAction;
	s_stCmdInfoList[ucCmdSlot].eCmdType = eCmd_Bdc;
	s_stCmdInfoList[ucCmdSlot].item.pstBdcItem = &s_stBdcData[ucMsgSlot];

	if (svc_cas_MgrSubUpdateCasMessage(hAction,
									eMsgType,
									eDxCAS_GROUPID_NA,
									(void*)&s_stCmdInfoList[ucCmdSlot],
									eShowType) != ERR_OK)
	{
		// error일시 memory free하도록 하자.
		HxLOG_Error("error in svc_cas_MgrSubUpdateCasMessage\n");
		return BDC_ERROR;
	}
	return BDC_NO_ERROR;
}

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES) // DRM_TEST
#if defined(CONFIG_MW_CAS_NAGRA_APS_CAKTEST)|| defined(CONFIG_DEBUG)
// DetailDrmInfoSes_t와 동일한 내용이어야함.
typedef struct
{
	eDxCopyrightControl_SesTrickModeCtrl_e		eTrickModeCtrl;
	HBOOL		 				bSvcBlock;
	HBOOL		 				bDigiCpProtect, bAnaCpProtect;

	eDxCopyrightControl_SesStorageRight_e		eStorageRight;
	HBOOL		 				bStorageEncrypt;
	eDxCopyrightControl_SesComponentCtrl_e		eCompVidCtrl;
	HBOOL		 				bTransCtrl;
	HBOOL		 				bBlucomCtrl;
} DrmInfoSes_t;

static HERROR _naMgr_procDrmParseApsCopyCtrlBytes (HUINT8 ucCtrlByteA, HUINT8 ucCtrlByteB)
{
	DrmInfoSes_t stDrmInfo;

	VK_memset (&stDrmInfo, 0, sizeof(DrmInfoSes_t));

	// Byte A
	stDrmInfo.eTrickModeCtrl = (HUINT8)((ucCtrlByteA & 0x38) >> 3);
	stDrmInfo.bSvcBlock		 = (ucCtrlByteA & 0x04) ? TRUE : FALSE;
	stDrmInfo.bDigiCpProtect	 = (ucCtrlByteA & 0x02) ? TRUE : FALSE;
	stDrmInfo.bAnaCpProtect	 = (ucCtrlByteA & 0x01) ? TRUE : FALSE;

	// Byte B
	stDrmInfo.eStorageRight	 = (HUINT8)((ucCtrlByteB & 0xE0) >> 5);
	stDrmInfo.bStorageEncrypt = (ucCtrlByteB & 0x10) ? TRUE : FALSE;
	stDrmInfo.eCompVidCtrl	 = (HUINT8)((ucCtrlByteB & 0x0C) >> 2);
	stDrmInfo.bTransCtrl		 =  (ucCtrlByteB & 0x02) ? TRUE : FALSE;
	stDrmInfo.bBlucomCtrl	 =  (ucCtrlByteB & 0x01) ? TRUE : FALSE;

	// cak7 - CAK-MER-MetadataCopyCtrl test 시 보여주는 코드
	HxLOG_Info("[METADATA] eTrickModeCtrl(%x)SvcBlock(%x) DigiCpProtect(%x)AnaCpProtect(%x) eStorageRight(%x) bStorageEncrypt(%x) eCompVidCtrl(%x)bTransCtrl(%x) bBlucomCtrl(%x)\n\n",
		stDrmInfo.eTrickModeCtrl, stDrmInfo.bSvcBlock, stDrmInfo.bDigiCpProtect, stDrmInfo.bAnaCpProtect,
		stDrmInfo.eStorageRight, stDrmInfo.bStorageEncrypt, stDrmInfo.eCompVidCtrl, stDrmInfo.bTransCtrl,
		stDrmInfo.bBlucomCtrl);

	return ERR_OK;
}
#endif

static HERROR _naMgr_procDrmCopInfo(TTransportSessionId xTransportSessionId, TPid xPid, /*TCopyProtectionData*/TUnsignedInt8 xCopyProtectionData[2])
{
	DxCopyrightControl_t	*pstDrmInfo = NULL;
	TUnsignedInt8		ucMsgSlot;

	TUnsignedInt32	hCasInstance;

	if(CA_NA_PsiGetCasInstanceID(&hCasInstance) != ERR_OK)
	{
		HxLOG_Error("error in CA_NA_PsiGetCasInstanceID\n");
		return ERR_FAIL;
	}

	if(SendDrmCnt >= CAS_MAX_NUM_OF_ACTION)
	{
		SendDrmCnt = 0;
	}

	pstDrmInfo = &s_stDrmInfo[SendDrmCnt++];
	if(pstDrmInfo == NULL)
	{
		return ERR_FAIL;
	}

	if ( (xCopyProtectionData[0] != 0xFF) || (xCopyProtectionData[1] != 0xFF) )
	{
		_naMgr_GetSlotMsgBuffer(eDrmCopBufferIdx, &ucMsgSlot);
		HxSTD_memset(&s_stDrmCopData[ucMsgSlot], 0x00, sizeof(NaDrmCopItem_t));
		HxSTD_memset(pstDrmInfo, 0, sizeof(DxCopyrightControl_t));

		s_stDrmCopData[ucMsgSlot].ucByte_A = xCopyProtectionData[0];
		s_stDrmCopData[ucMsgSlot].ucByte_B = xCopyProtectionData[1];

		// control byte 민 올려주고 xmgr_copyrightcontrol_GetCasCopyProtectInfo_NagraSes 에서 파싱한다.
		pstDrmInfo->stCasCcInfo.eCasCcType = eDxCopyrightControl_CasCcType_NagraSes;
		pstDrmInfo->stCasCcInfo.info.stNagraDrmInfo_Ses.ucControlByteA = s_stDrmCopData[ucMsgSlot].ucByte_A;
		pstDrmInfo->stCasCcInfo.info.stNagraDrmInfo_Ses.ucControlByteB = s_stDrmCopData[ucMsgSlot].ucByte_B;

#if 0//defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)|| defined(CONFIG_DEBUG)
		_naMgr_procDrmParseApsCopyCtrlBytes(s_stDrmCopData[ucMsgSlot].ucByte_A, s_stDrmCopData[ucMsgSlot].ucByte_B);
#endif

		if(svc_cas_MgrSubUpdateDrmInfo(xTransportSessionId, hCasInstance, (void*)pstDrmInfo, eDxCopyrightControl_CasCcType_NagraSes, 0) != ERR_OK)
		{
			HxLOG_Error("error in svc_cas_MgrSubUpdateDrmInfo\n");
			return ERR_FAIL;
		}
	}
	else
	{
		if(svc_cas_MgrSubUpdateDrmInfo(xTransportSessionId, hCasInstance, NULL, eDxCopyrightControl_CasCcType_NagraSes, 0) != ERR_OK)
		{
			// error일시 memory free하도록 하자.
			HxLOG_Error("error in svc_cas_MgrSubUpdateDrmInfo\n");
			return ERR_FAIL;
		}
	}
	return ERR_OK;
}
#endif

static void _naMgr_procIrdCommand(NA_IRDCMD_ID_t eCmdId, NA_IRDCMD_OPCODE_t eCmdOpCode, TUnsignedInt32 ulParam1, TUnsignedInt32 ulParam2)
{
	TUnsignedInt32 		hAction;

	TUnsignedInt8			ucMsgSlot;
	TUnsignedInt8			ucCmdSlot;

	SvcCas_MsgType_e		eMsgType = eCAS_MSG_TYPE_Command;
	SvcCas_ShowType_e		eShowType = eCasShowType_Both;

	if (CA_NA_PsiGetLiveAction((Handle_t *)&hAction) != ERR_OK)
	{
		HxLOG_Error("error in CA_NA_PsiGetLiveAction\n");
		return;
	}

	_naMgr_GetSlotMsgBuffer(eIrdCmdBufferIdx, &ucMsgSlot);
	HxSTD_memset(&s_stIrdCmdData[ucMsgSlot], 0x00, sizeof(NaIrdCmdItem_t));

	s_stIrdCmdData[ucMsgSlot].eIrdCmd = eCmdId;
	s_stIrdCmdData[ucMsgSlot].eIrdOpCode = eCmdOpCode;
	s_stIrdCmdData[ucMsgSlot].ulParam1 = ulParam1;
	s_stIrdCmdData[ucMsgSlot].ulParam2 = ulParam2;

	_naMgr_GetSlotMsgBuffer(eCmdBufferIdx, &ucCmdSlot);
	s_stCmdInfoList[ucCmdSlot].hAction = hAction;
	s_stCmdInfoList[ucCmdSlot].eCmdType = eCmd_IrdCmd;
	s_stCmdInfoList[ucCmdSlot].item.pstIrcCmdItem = &s_stIrdCmdData[ucMsgSlot];

	if (svc_cas_MgrSubUpdateCasMessage(hAction,
									eMsgType,
									eDxCAS_GROUPID_NA,
									(void*)&s_stCmdInfoList[ucCmdSlot],
									eShowType) != ERR_OK)
	{
		// error일시 memory free하도록 하자.
		HxLOG_Error("error in svc_cas_MgrSubUpdateCasMessage\n");
		return;
	}
	return;
}

static HERROR _naMgr_GetLastAlarmData(TUnsignedInt32 hAction, SvcCas_MsgType_e *peMsgType, DxCAS_GroupId_e *peCasId, void **ppMessage, SvcCas_ShowType_e *peShowType)
{
	TUnsignedInt8 	i;
	TUnsignedInt8		ucMsgSlot;

	NaAlarmData_t 	stNaAlarmData;

	if ( (peMsgType == NULL) || (peCasId == NULL)
		|| (ppMessage == NULL) || (peShowType == NULL) )
	{
		HxLOG_Error("targets are null...\n");
		return ERR_FAIL;
	}

	HxLOG_Print("hAction (0x%x)\n", hAction);

    HxSTD_memset(&stNaAlarmData, 0x00, sizeof(NaAlarmData_t));

	*peCasId = eDxCAS_GROUPID_NA;
	*peMsgType = eCAS_MSG_TYPE_Ok;
	*peShowType = eCasShowType_Both;

	WAIT_FOR_SEM(s_ulNaMgrSem)
	// sc alarm 부터 check
	for (i=0; i<NO_SMC_SLOT; i++)
	{
		if (s_stScInfoList[i].eAlarm != CA_SMARTCARD_STATE_OK)
		{
			stNaAlarmData.hAction 			= hAction;
			stNaAlarmData.eAlarmType 		= eAlarm_Smartcard;
			stNaAlarmData.ucNumOfScAlarms = NO_SMC_SLOT;
			stNaAlarmData.pstScAlarmItems 	= &s_stScInfoList;
			break;
		}
	}

	if (i < NO_SMC_SLOT)
	{
		// sc error 이므로 그 상태 전달
		*peMsgType = eCAS_MSG_TYPE_Fail;
		*peShowType = eCasShowType_None;
	}
	else
	{
		for (i=0; i<MAX_NO_SESSION; i++)
		{
			if ( (s_stAccessInfoList[i].hAction == hAction)
				&& (s_stAccessInfoList[i].bRunStatus == TRUE) )
			{
			    break;
			}
		}

		if (i < MAX_NO_SESSION)
		{
			if (s_stAccessInfoList[i].ucNumOfStreams > 0) 	// alarm error (FAIL)
			{
				stNaAlarmData.hAction 				= hAction;
				stNaAlarmData.eAlarmType 			= eAlarm_Access;
				stNaAlarmData.ucNumOfAccessAlarms = s_stAccessInfoList[i].ucNumOfStreams;
				stNaAlarmData.pstAccessAlarmItems 	= &s_stAccessInfoList[i].stAccessAlarmItems;

				*peMsgType = eCAS_MSG_TYPE_Fail;
				*peShowType = eCasShowType_None;
			}
			else											// no error (OK)
			{
				stNaAlarmData.hAction 				= hAction;
				stNaAlarmData.eAlarmType 			= eAlarm_None;
				stNaAlarmData.ucNumOfAccessAlarms	= 0;
				stNaAlarmData.pstAccessAlarmItems 	= NULL;
			}
		}
		else
		{
			RELEASE_SEM(s_ulNaMgrSem)
			return ERR_FAIL;
		}
	}

	_naMgr_GetSlotMsgBuffer(eAlarmBufferIdx, &ucMsgSlot);
	HxLOG_Print("SC or ACCESS ALARM : Slot No (%d)\n", ucMsgSlot);
	HxSTD_memcpy(&s_stNaAlarmData[ucMsgSlot], &stNaAlarmData, sizeof(NaAlarmData_t));
//	HxSTD_memcpy(s_stNaAlarmData[ucMsgSlot].pstScAlarmItems, stNaAlarmData.pstScAlarmItems, sizeof(NaScAlarmItem_t));
//	HxSTD_memcpy(s_stNaAlarmData[ucMsgSlot].pstAccessAlarmItems, stNaAlarmData.pstAccessAlarmItems, sizeof(NaAccessAlarmItem_t));
	*ppMessage = (void*)&s_stNaAlarmData[ucMsgSlot];
	RELEASE_SEM(s_ulNaMgrSem)
	return ERR_OK;
}


static HERROR _naMgr_GetCurrScStatus(TUnsignedInt32 ulSlotId, TSmartcardState *peScAlarm)
{
	TUnsignedInt8 i;

	if (peScAlarm == NULL)
	{
		HxLOG_Error("target is null...\n");
		return ERR_FAIL;
	}

	WAIT_FOR_SEM(s_ulNaMgrSem)

	for (i=0; i<NO_SMC_SLOT; i++)
	{
		if (s_stScInfoList[i].ulSlotId == ulSlotId)
		{
			*peScAlarm = s_stScInfoList[i].eAlarm;

			#if (CONFIG_MW_CAS_NUM_EMBEDDED > 1)
			{
				ScStatusItem_t stScStatus[NO_SMC_SLOT];

				if (*peScAlarm == CA_ALARM_SMARTCARD_REMOVED)
				{
					if (CA_NA_IccGetCardStatus(stScStatus) == ERR_OK)
					{
						for (i=0; i<NO_SMC_SLOT; i++)
						{
							if (stScStatus[i].ucSlotId == ucSlotId)
							{
								if (stScStatus[i].eCardStatus == eSc_Out)
								{
									HxLOG_Print("CA_ALARM_SMARTCARD_REMOVED is right!!!\n");
								}
								else
								{
									HxLOG_Print("CA_ALARM_SMARTCARD_REMOVED is not right(%d)!!!\n", stScStatus[i].eCardStatus);
									*peScAlarm = CA_SMARTCARD_STATE_INVALID;
								}
								break;
							}
						}
					}
				}
			}
			#endif

			RELEASE_SEM(s_ulNaMgrSem)
			return ERR_OK;
		}
	}

	RELEASE_SEM(s_ulNaMgrSem)
	return ERR_FAIL;
}


#if defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
//#define TEST_MEDIASET_CARD_ENABLE
static HBOOL _naMgr_TIVU_CheckCardSystemId(void)
{
	HUINT32 			i =0;
	HERROR 			hErr = ERR_OK;
	TUnsignedInt16	usCaSystemId = 0;

	for (i=0; i < CAS_MAX_NUM_OF_SC; i ++)
	{
		hErr = CA_NA_CakGetEcmCaSystemId(i, &usCaSystemId);
		HxLOG_Print("hErr = %d, usCaSystemId = 0x%x\n", hErr, usCaSystemId);
		if (hErr == ERR_OK && usCaSystemId != 0)
		{
			if ((usCaSystemId >= NA_CAS_TIVU_RANGE_FROM) && (usCaSystemId <= NA_CAS_TIVU_RANGE_TO))
			{
				// tivu stream
				HxLOG_Print("Tivu Nagra Stream\n");
				return TRUE;
			}
#if defined(TEST_MEDIASET_CARD_ENABLE)
			else if ((usCaSystemId >= NA_CAS_MEDIASET_RANGE_FROM) && (usCaSystemId <= NA_CAS_MEDIASET_RANGE_TO))
			{
				// mediaset stream
				HxLOG_Print("Mediaset Nagra Stream\n");
				if (usCaSystemId == NA_CAS_TIVU_TEST_CA_SYSTEM_ID)
				{
					HxLOG_Print("Tivu test Stream\n");
					return TRUE;
				}
				else
				{
					HxLOG_Print("not Tivu test Stream\n");
				}
			}
#endif
			else
			{
				HxLOG_Print("Other Nagra Stream\n");
			}
		}
	}
	return FALSE;
}
#endif


static HERROR _naMgr_ReportCurrScStatus(TUnsignedInt32 hAction)
{
	TUnsignedInt8			i;
	TUnsignedInt8 		ucMsgSlot;
	TSmartcardState		eScAlarm = CA_SMARTCARD_STATE_OK;

	NaAlarmData_t 		stNaAlarmData;

	SvcCas_MsgType_e		eMsgType = eCAS_MSG_TYPE_Ok;
	SvcCas_ShowType_e		eShowType = eCasShowType_Both;

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
	if (_naMgr_GetCurrScStatus(CARDLESS_SLOT_ID, &eScAlarm) != ERR_OK)
	{
		HxLOG_Error("error in _naMgr_GetCurrScStatus\n");
		return ERR_FAIL;
	}

	if (eScAlarm == CA_SMARTCARD_STATE_OK)
	{
		HxLOG_Print("This Smartcard is Ok : Slot (%d) \n", i);
		return ERR_OK;
	}
#else
	for (i=0; i<NO_SMC_SLOT; i++)
	{
		if (_naMgr_GetCurrScStatus(i, &eScAlarm) != ERR_OK)
		{
			HxLOG_Error("error in _naMgr_GetCurrScStatus\n");
			return ERR_FAIL;
		}

		if (eScAlarm == CA_SMARTCARD_STATE_OK)
		{
#if defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
			if (_naMgr_TIVU_CheckCardSystemId())
#endif
			{
				HxLOG_Print("This Smartcard is Ok : Slot (%d) \n", i);
				return ERR_OK;
			}
		}

		HxLOG_Print("This Smartcard is Error (0x%x) : Slot (%d)\n", eScAlarm, i);
	}
#endif

	eMsgType = eCAS_MSG_TYPE_Fail;
	eShowType = eCasShowType_None;

	HxSTD_memset(&stNaAlarmData, 0x00, sizeof(NaAlarmData_t));

	stNaAlarmData.hAction 			= hAction;
	stNaAlarmData.eAlarmType 		= eAlarm_Smartcard;
	stNaAlarmData.ucNumOfScAlarms = NO_SMC_SLOT;
	stNaAlarmData.pstScAlarmItems 	= (NaScAlarmItem_t *)&s_stScInfoList;

#if (CONFIG_FOR_DAL_TEST_DEF == 0)
	_naMgr_GetSlotMsgBuffer(eAlarmBufferIdx, &ucMsgSlot);
	HxLOG_Print("SC ALARM : Slot No (%d)\n", ucMsgSlot);
	HxSTD_memcpy(&s_stNaAlarmData[ucMsgSlot], &stNaAlarmData, sizeof(NaAlarmData_t));
//	HxSTD_memcpy(s_stNaAlarmData[ucMsgSlot].pstScAlarmItems, stNaAlarmData.pstScAlarmItems, sizeof(NaScAlarmItem_t));
//	HxSTD_memcpy(s_stNaAlarmData[ucMsgSlot].pstAccessAlarmItems, stNaAlarmData.pstAccessAlarmItems, sizeof(NaAccessAlarmItem_t));
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	if (CASMGR_APS_UpdateCasMessage(hAction,
										eMsgType,
										eDxCAS_GROUPID_NA,
										(void*)&s_stNaAlarmData[ucMsgSlot],
										eShowType) != ERR_OK)
	{
		// error일시 memory free하도록 하자.
		HxLOG_Error("error in svc_cas_MgrSubUpdateCasMessage\n");
		return ERR_FAIL;
	}
#else
	if (svc_cas_MgrSubUpdateCasMessage(hAction,
									eMsgType,
									eDxCAS_GROUPID_NA,
									(void*)&s_stNaAlarmData[ucMsgSlot],
									eShowType) != ERR_OK)
	{
		// error일시 memory free하도록 하자.
		HxLOG_Error("error in svc_cas_MgrSubUpdateCasMessage\n");
		return ERR_FAIL;
	}
#endif
#endif

	return ERR_OK;
}


static HERROR _naMgr_ReportCurrAlarmData(void)
{
	TUnsignedInt8 	i;
	TUnsignedInt8	ucMsgSlot;

	NaAlarmData_t 	stNaAlarmData;

	SvcCas_MsgType_e	eMsgType = eCAS_MSG_TYPE_Ok;
	SvcCas_ShowType_e	eShowType = eCasShowType_Both;

        HxSTD_memset(&stNaAlarmData, 0x00, sizeof(NaAlarmData_t));

	WAIT_FOR_SEM(s_ulNaMgrSem)
	for (i=0; i<MAX_NO_SESSION; i++)
	{
		if ( (s_stAccessInfoList[i].bRunStatus == FALSE)
			|| (s_stAccessInfoList[i].eAlarmType == eAlarm_Unknown) )
		{
			continue;
		}

		if (s_stAccessInfoList[i].ucNumOfStreams > 0) 	// alarm error (FAIL)
		{
			stNaAlarmData.hAction 				= s_stAccessInfoList[i].hAction;
			stNaAlarmData.eAlarmType 			= eAlarm_Access;
			stNaAlarmData.ucNumOfAccessAlarms = s_stAccessInfoList[i].ucNumOfStreams;
			stNaAlarmData.pstAccessAlarmItems 	= &s_stAccessInfoList[i].stAccessAlarmItems;

			eMsgType = eCAS_MSG_TYPE_Fail;
			eShowType = eCasShowType_None;
		}
		else											// no error (OK)
		{
			stNaAlarmData.hAction 				= s_stAccessInfoList[i].hAction;
			stNaAlarmData.eAlarmType 			= eAlarm_None;
			stNaAlarmData.ucNumOfAccessAlarms	= 0;
			stNaAlarmData.pstAccessAlarmItems 	= NULL;
		}

#if (CONFIG_FOR_DAL_TEST_DEF == 0)
		_naMgr_GetSlotMsgBuffer(eAlarmBufferIdx, &ucMsgSlot);
		HxLOG_Print("ACCESS ALARM : Slot No (%d)\n", ucMsgSlot);
		HxSTD_memcpy(&s_stNaAlarmData[ucMsgSlot], &stNaAlarmData, sizeof(NaAlarmData_t));
//		HxSTD_memcpy(s_stNaAlarmData[ucMsgSlot].pstScAlarmItems, stNaAlarmData.pstScAlarmItems, sizeof(NaScAlarmItem_t));
//		HxSTD_memcpy(s_stNaAlarmData[ucMsgSlot].pstAccessAlarmItems, stNaAlarmData.pstAccessAlarmItems, sizeof(NaAccessAlarmItem_t));
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
		if (CASMGR_APS_UpdateCasMessage(s_stAccessInfoList[i].hAction,
										eMsgType,
										eDxCAS_GROUPID_NA,
										(void*)&s_stNaAlarmData[ucMsgSlot],
										eShowType) != ERR_OK)
#else
		if (svc_cas_MgrSubUpdateCasMessage(s_stAccessInfoList[i].hAction,
										eMsgType,
										eDxCAS_GROUPID_NA,
										(void*)&s_stNaAlarmData[ucMsgSlot],
										eShowType) != ERR_OK)
#endif
		{
			// error일시 memory free하도록 하자.
			HxLOG_Info("error in svc_cas_MgrSubUpdateCasMessage\n");
		}
#endif
	}

	RELEASE_SEM(s_ulNaMgrSem)
	return ERR_OK;
}


static HERROR _naMgr_SetSessionStatus(TUnsignedInt32 hAction, TBoolean bRunStatus)
{
	TUnsignedInt32 ulActionId;

	ulActionId = svc_cas_SubSvcRmGetActionId(hAction);
	HxLOG_Print("Action:0x%x, Action ID(%d), Run Status(%d)\n", hAction, ulActionId, bRunStatus);

	if ( (ulActionId < 0) || (ulActionId >= MAX_NO_SESSION) )
	{
		HxLOG_Error("over of index (0x%x)\n", ulActionId);
		return ERR_FAIL;
	}

	if (bRunStatus)
	{
		HxSTD_memset(&s_stAccessInfoList[ulActionId], 0x00, sizeof(NaAccessAlarmList_t));

		// sc error 경우 svc전환해도 다시 올라오지 않아서 svc전환때 마다 call해주어야 함.
		// start svc에서 pmt update시로 변경 함.
#if (0)
		if (_naMgr_ReportCurrScStatus(hAction) != ERR_OK)
		{
			HxLOG_Error("error in _naMgr_ReportCurrScStatus (0x%x)\n", ulActionId);
		}
#endif
	}

	s_stAccessInfoList[ulActionId].hAction = hAction;
	s_stAccessInfoList[ulActionId].bRunStatus = bRunStatus;

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	if (bRunStatus == TRUE)
	{
		CASMGR_APS_SetServiceStatus(bRunStatus);
	}
#endif
	return ERR_OK;
}


#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
static HERROR _naMgr_IsHdPlusSmartCard(TUnsignedInt8 ucSlotId, TBoolean *pbHdPlusSc)
{
	TSmartcardState eScAlarm;

	if (pbHdPlusSc == NULL)
	{
		HxLOG_Error("target is null...\n");
		return ERR_FAIL;
	}

	HxLOG_Print("Slot No (%d)\n", ucSlotId);

	*pbHdPlusSc = FALSE;
	if (_naMgr_GetCurrScStatus(ucSlotId, &eScAlarm) != ERR_OK)
	{
		HxLOG_Error("error in _naMgr_GetCurrScStatus\n");
		return ERR_FAIL;
	}

	HxLOG_Print("Sc Alarm status (0x%x)\n", eScAlarm);
	if ( (eScAlarm != CA_SMARTCARD_STATE_MUTE)
		&& (eScAlarm != CA_SMARTCARD_STATE_INVALID)
		&& (eScAlarm != CA_SMARTCARD_STATE_NOT_CERTIFIED)
		&& (eScAlarm != CA_ALARM_SMARTCARD_REMOVED) )
	{
		*pbHdPlusSc = TRUE;
	}

	return ERR_OK;
}
#endif


// 임시...
#if defined(CONFIG_MW_CAS_NAGRA_OP_SKYD) || defined(CONFIG_MW_CAS_NAGRA_OP_KDG)
static HERROR _naMgr_CheckSmardStatus(TIccSessionId iccSessionId)
{
	PalSc_Handle_t				sHandle;
	PalSc_CardStatus_e			eScStatus;
	TBoolean				bIsFound = FALSE;
	TUnsignedInt32			i, ulErrCode;

	extern TUnsignedInt32	g_ulNumOfSlot;
	extern ScMgrIntance_t	*g_pstScMgrInstance;

	for (i=0; i<g_ulNumOfSlot; i++)
	{
		if (g_pstScMgrInstance[i].ulSessionId == iccSessionId)
		{
			sHandle = g_pstScMgrInstance[i].hSmcHandle;
			bIsFound = TRUE;
			break;
		}
	}

	if (bIsFound == FALSE)
		return ERR_FAIL;

	ulErrCode = svc_cas_DevScGetCardStatus(sHandle, &eScStatus);
	if (ulErrCode != ERR_OK)
		return ERR_FAIL;

	if (eScStatus == ePAL_SC_CARDSTATUS_PRESENT)
		return ERR_OK;
	else
		return ERR_FAIL;
}
#endif

	/**************************************************************************************************/
static HERROR _naMgr_InitFileSystem(void)
{
	if (HLIB_DIR_IsExist(CAS_NA_JFFS2_PATH) != TRUE)
	{
		if (HLIB_DIR_Make(CAS_NA_JFFS2_PATH) != ERR_OK)
		{
			HxLOG_Error("error occurred in HLIB_DIR_Make\n");
			return ERR_FAIL;
		}
#if defined(CONFIG_DROP_ROOT_PRIVILEGES)
		/* obama만 access 가능하도록 file permission 변경 */
		HLIB_EXTRA_Chmod(CAS_NA_JFFS2_PATH, 0700);

		{
			HBOOL bRoot = FALSE;

			if(HLIB_EXTRA_AmIRoot(&bRoot) == ERR_OK)
			{
				if(bRoot == TRUE)
				{
					if(HLIB_EXTRA_Chown(CAS_NA_JFFS2_PATH, "obama", "obama") != ERR_OK)
					{
						HxLOG_Error("ERROR In [HLIB_EXTRA_Chown]. [%s]\n", CAS_NA_JFFS2_PATH);
					}
				}
			}
		}
#endif
	}
	return ERR_OK;
}


/**************************************************************************************************/
#define ________Nagra_mgr_Public_Functions_Body____________________________________________
/**************************************************************************************************/
HERROR xsvc_cas_NaMgrInit(void)
{
	//HLIB_LOG_SetLevel(HxLOG_DOMAIN, HxLOG_INFO);

	HxLOG_Info("xsvc_cas_NaMgrInit....\n");

	if (s_ulNaMgrInt == CAS_INITAILIZED)
	{
		HxLOG_Critical("\n\n");
		return ERR_OK;
	}

	// step - 0 : init static memory
	if (_naMgr_InitMgr() != ERR_OK)
	{
		HxLOG_Error("error in _naMgr_InitMgr...\n");
		return ERR_FAIL;
	}

	if(_naMgr_InitFileSystem() != ERR_OK)
	{
		HxLOG_Error("error in _naMgr_InitFileSystem...\n");
	}

	// step - 1 : init all devices
	if (CA_NA_DmxInitialization() != ERR_OK)
	{
		HxLOG_Error("error in CA_NA_DmxInitialization...\n");
		return ERR_FAIL;
	}

#if defined (CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	if(CA_NA_CaSessionMgrInitialize() != ERR_OK)
	{
		HxLOG_Error("error in CA_NA_CaSessionMgrInitialize...\n");
		return ERR_FAIL;
	}
#else
	if (CA_NA_DscInitialization() != ERR_OK)
	{
		HxLOG_Error("error in CA_NA_DscInitialization...\n");
		return ERR_FAIL;
	}
#endif

	if (CA_NA_CakMgrInitialization() != ERR_OK)
	{
		HxLOG_Error("error in CA_NA_CakMgrInitialization...\n");
		return ERR_FAIL;
	}

	if (CA_NA_CakSvcInitialization() != ERR_OK)
	{
		HxLOG_Error("error in CA_NA_CakSvcInitialization...\n");
		return ERR_FAIL;
	}

	if (CA_NA_IccInitialization() != ERR_OK)
	{
		HxLOG_Error("error in CA_NA_IccInitialization...\n");
		return ERR_FAIL;
	}

	if (CA_NA_OsInitialization() != ERR_OK)
	{
		HxLOG_Error("error in CA_NA_OsInitialization...\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	if (CAS_NA_SecInitialize() != ERR_OK)
	{
		HxLOG_Error("error in CAS_NA_SecInitialize...\n");
		return ERR_FAIL;
	}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_NOCS_3_X)
	if (CAS_NA_CertInitialize() != ERR_OK)
	{
		HxLOG_Error("error in CAS_NA_CertInitialize...\n");
		return ERR_FAIL;
	}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
	if (CAS_NA_VfsInitialize() != ERR_OK)
	{
		HxLOG_Error("error in CAS_NA_VfsInitialize...\n");
		return ERR_FAIL;
	}
#endif

	// step - 2 : register callback funs
	// for alarm message
	CA_NA_CakRegisterAlarmExportation(_naMgr_ProcAlarmExportation);

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	// for popup message
	CA_NA_CakRegisterCaPopUpProc(_naMgr_procPopupCallback);
#endif

	// for popup message
	CA_NA_BdcRegisterPopUpFunc(_naMgr_procDisplayPopUp, _naMgr_procRemovePopUp);

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD)
	// for ird command
	CA_NA_IRDCMD_RegisterCallback(_naMgr_procIrdCommand);
#endif

	// for DRM &  Copy protection
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES) // DRM_TEST
	CA_NA_CopRegisterFunc(_naMgr_procDrmCopInfo);
#endif

	CA_NA_RegisterCtrlList();

	// step - 4: start-up cak_svc
	if(CA_NA_CakSvcStartup() != ERR_OK)
	{
		HxLOG_Error("error in CA_NA_CakSvcStartup...\n");
		return ERR_FAIL;
	}

#if 1
	// step - 5: start-up cak_mgr
	if (CA_NA_CakMgrStartUp() != ERR_OK)
	{
		HxLOG_Error("error in CA_NA_CakMgrStartUp...\n");
		return ERR_FAIL;
	}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_PRM)
	if (CA_NA_DvlInitialze() != ERR_OK)
	{
		HxLOG_Error("error in CA_NA_DvlInitialze...\n");
		//1 Todo DVL initial 과정에서 error가 발생하고 있음, 문제의 원인을 해결한 후에 아래 코드 활성화 필요.
		//return ERR_FAIL;
	}
#endif

	s_ulNaMgrInt = CAS_INITAILIZED;
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST) && !defined(CONFIG_DEBUG)
	MW_CA_NA_InitTerminalForRelease();
#endif
	return ERR_OK;
}


HERROR NAMGR_SetSessionStatus(TUnsignedInt32 hAction, TBoolean bRunStatus)
{
	return _naMgr_SetSessionStatus(hAction, bRunStatus);
}


HERROR NAMGR_GetLastAlarmData(TUnsignedInt32 hAction, SvcCas_MsgType_e *peMsgType, DxCAS_GroupId_e *peCasId, void **ppMessage, SvcCas_ShowType_e *peShowType)
{
	return _naMgr_GetLastAlarmData(hAction, peMsgType, peCasId, ppMessage, peShowType);
}


/**************************************************************************************************/
#define ________Nagra_mgr_External_Functions_Body__________________________________________
/**************************************************************************************************/
// for support cak
// application으로 특별한 정보 요청시 사용(주로 PPV기능에서 사용될 예정)
HERROR MW_CA_NA_RequestInfo(TUnsignedInt32 ulReqId)
{
	return CA_NA_CakRequest(ulReqId);
}


// application에서 특정 event 혹은 정보가 변경될때 등록하는 Listeners
// 1-System Info update
HERROR MW_CA_NA_RegisterSystemInfoCallback(TSystemInfoCallback callback)
{
	return CA_NA_CakRegisterSystemInfoCallback(callback);
}


// 2-Location Info update
HERROR MW_CA_NA_RegisterLocInfoCallback(TLocationInfoCallback callback)
{
	return CA_NA_CakRegisterLocInfoCallback(callback);
}


// 3-Smartcard alarm update
HERROR MW_CA_NA_RegisterScAlarmCallback(TSmartcardAlarmCallback callback)
{
	return CA_NA_CakRegisterScAlarmCallback(callback);
}


HERROR MW_CA_NA_GetSystemInfo(TSystemInfoStatus *xStatus, TIrdInfo **pxIrdInfo,
 											TUnsignedInt8 *xNumberOfSmartcards, TSmartcardInfoItem **pxSmartcardInfoArray)
{
	return CA_NA_CakGetSystemInfo(xStatus, pxIrdInfo, xNumberOfSmartcards, pxSmartcardInfoArray);
}

HERROR MW_CA_NA_GetChipsetInfo(NaChipsetInfo_t **pstChipInfo)
{
	return CA_NA_CakGetChipsetInfo(pstChipInfo);
}

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ALME)
HERROR MW_CA_NA_GetCscMaxIndex(TUnsignedInt8 *pxCscMaxIndex)
{
	return CA_NA_GetCscMaxIndex(pxCscMaxIndex);
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
HERROR MW_CA_NA_GetAccessErrorCode(TUnsignedInt32 ulCurrentSessionId, TUnsignedInt32* pAccessErrorCode)
{
	return CA_NA_GetAccessErrorCode(ulCurrentSessionId, pAccessErrorCode);
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
HERROR MW_CA_NA_GetNumberOfIcc(TUnsignedInt8 *pxNumberOfIcc)
{
	return CA_NA_GetNumberOfIcc(pxNumberOfIcc);
}
#endif

HERROR MW_CA_NA_GetLocInfo(TLocationInfoStatus *xStatus, TUnsignedInt8 *xNumberOfSmartcards, TLocationInfoItem **pxLocationInfoArray)
{
	return CA_NA_CakGetLocInfo(xStatus, xNumberOfSmartcards, pxLocationInfoArray);
}


HERROR MW_CA_NA_CheckPincode(TPinIndex xPinIndex, TPinCode xPinCode, TUnsignedInt8 *pucPinCode, TPinCodeStatus *xStatus)
{
	return CA_NA_CakCheckPincode(xPinIndex, xPinCode, pucPinCode, xStatus);
}


HERROR MW_CA_NA_ChangePincode(TPinIndex xPinIndex, TPinCode xOldPinCode, TPinCode xNewPinCode, TUnsignedInt8 *pucOldPinCode, TUnsignedInt8 *pucNewPinCode, TPinCodeStatus *xStatus)
{
	return CA_NA_CakChangePincode(xPinIndex, xOldPinCode, xNewPinCode, pucOldPinCode, pucNewPinCode, xStatus);
}


#if defined(CONFIG_OP_CAS_NAGRA_CAKTEST_PPT)
HERROR MW_CA_NA_Cak7_EnablingConsumptionForced(TBoolean bOneShot)
{
	return CA_NA_Cak7_EnablingConsumptionForced(bOneShot);
}
#endif


#if 0
HERROR MW_CA_NA_GetProviderId(TUnsignedInt8 ucSlotId, TPpid *pProviderId)
{
	CA_NA_CakGetProviderId(ucSlotId, pProviderId);
	return ERR_FAIL;
}


HERROR MW_CA_NA_GetEcmCaSystemId(TUnsignedInt8 ucSlotId, TUnsignedInt16 *pusCaSysId)
{
	CA_NA_CakGetEcmCaSystemId(ucSlotId, pusCaSysId);
	return ERR_FAIL;
}
#endif


HERROR MW_CA_NA_GetCurrScStatus(TUnsignedInt32 ulSlotId, TSmartcardState *peScAlarm)
{
	return _naMgr_GetCurrScStatus(ulSlotId, peScAlarm);
}


HERROR MW_CA_NA_ReportCurrScStatus(TUnsignedInt32 hAction)
{
	return _naMgr_ReportCurrScStatus(hAction);
}


HERROR MW_CA_NA_ReportCurrAlarmData(void)
{
	return _naMgr_ReportCurrAlarmData();
}


// for support psi
HERROR MW_CA_NA_GetNumberOfSlots(TUnsignedInt8 *pucSlots)
{
	return CA_NA_IccGetNumberOfSlots(pucSlots);
}


HERROR MW_CA_NA_IsSameCasIdInSc(TUnsignedInt8 ucSlotId, TUnsignedInt16 usCaSysId)
{
	return CA_NA_CakSameCasIdInSc(ucSlotId, usCaSysId);
}

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD)
// for irdcmd
HERROR MW_CA_NA_ResetIRDCmdData(void)
{
	return CA_NA_IRDCMD_ResetIRDCmdData();
}


// for ird cmd mail
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
HERROR MW_CA_NA_IRDCMD_MAIL_CheckUnreadMessage(void)
{
	return CA_NA_IRDCMD_MAIL_CheckUnreadMessage();
}


HERROR MW_CA_NA_IRDCMD_MAIL_ClearMailMessageHeader(void)
{
	return CA_NA_IRDCMD_MAIL_ClearMailMessageHeader();
}


HERROR MW_CA_NA_IRDCMD_MAIL_MakeMailMessage(TUnsignedInt8 *pIrdCmd, TUnsignedInt16	*pnMailId, TBoolean *pfCompleted, TUnsignedInt16 *pnDeletedMailId)
{
	return CA_NA_IRDCMD_MAIL_MakeMailMessage(pIrdCmd, pnMailId, pfCompleted, pnDeletedMailId);
}


HERROR MW_CA_NA_IRDCMD_MAIL_LoadMailMessages(void)
{
	return CA_NA_IRDCMD_MAIL_LoadMailMessages();
}


HERROR MW_CA_NA_IRDCMD_MAIL_GetMailPriority(TUnsignedInt16 nMailId, NA_MAIL_PRIORITY *pnPriority)
{
	return CA_NA_IRDCMD_MAIL_GetMailPriority(nMailId, pnPriority);
}


HERROR MW_CA_NA_IRDCMD_MAIL_GetMailList(NA_MAIL_MSG ***pppMailList, TUnsignedInt32 *pnCount)
{
	return CA_NA_IRDCMD_MAIL_GetMailList(pppMailList, pnCount);
}


HERROR MW_CA_NA_IRDCMD_MAIL_GetMailData(TUnsignedInt16 nMailId, NA_MAIL_MSG **ppMailData)
{
	return CA_NA_IRDCMD_MAIL_GetMailData(nMailId, ppMailData);
}

HERROR MW_CA_NA_IRDCMD_MAIL_DeleteMailMessage(TUnsignedInt16 nMailId)
{
	return CA_NA_IRDCMD_MAIL_DeleteMailMessage(nMailId);
}


HERROR MW_CA_NA_IRDCMD_MAIL_UpdateReadFlag(TUnsignedInt16 nMailId, TBoolean bRead)
{
	return CA_NA_IRDCMD_MAIL_UpdateReadFlag(nMailId, bRead);
}
#endif


// for ird cmd popup msg
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
HERROR MW_CA_NA_IRDCMD_POPUP_DisplayPopupMessage(void)
{
	return CA_NA_IRDCMD_POPUP_DisplayPopupMessage();
}
#endif


// for ird cmd emm wake-up
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
HERROR MW_CA_NA_IRDCMD_EMM_GetWakeupTime(UNIXTIME *pulTime)
{
	return CA_NA_IRDCMD_EMM_GetWakeupTime(pulTime);
}


HERROR MW_CA_NA_IRDCMD_EMM_InitWakingTable(void)
{
	return CA_NA_IRDCMD_EMM_InitWakingTable();
}


HERROR MW_CA_NA_IRDCMD_EMM_HasMoreNextWaking(TBoolean *pbNext)
{
	return CA_NA_IRDCMD_EMM_HasMoreNextWaking(pbNext);
}


HERROR MW_CA_NA_IRDCMD_EMM_GetWakingTuneInfo(TUnsignedInt8 *pucNetworkIdFlag, TUnsignedInt16 *pusNetworkId,
																TUnsignedInt8 *pucTsIdFlag, TUnsignedInt16 *pusTsId,
																TUnsignedInt8 *pucServiceIdFlag, TUnsignedInt16 *pusServiceId)
{
	return CA_NA_IRDCMD_EMM_GetWakingTuneInfo(pucNetworkIdFlag, pusNetworkId,
													pucTsIdFlag, pusTsId,
													pucServiceIdFlag, pusServiceId);
}


HERROR MW_CA_NA_IRDCMD_EMM_GetWakingTimer(UNIXTIME *pulTime)
{
	return CA_NA_IRDCMD_EMM_GetWakingTimer(pulTime);
}
#endif
#endif //#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD)

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_ZONEID)
HERROR MW_CA_NA_GetZoneId(TUnsignedInt8* pZoneId, TUnsignedInt32 unMaxLen)
{
	return CA_NA_IRDCMD_ZoneId_GetID(pZoneId, unMaxLen);
}

HERROR MW_CA_NA_GetCountryCode(TUnsignedInt8* pCountryCode, TUnsignedInt32 unMaxLen)
{
	return CA_NA_IRDCMD_CountryCode_GetID(pCountryCode, unMaxLen);
}
#endif

// for aps spec
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
// Zip Code는 OP에 따라 지원할 수도 안할 수도 있다. 지원여부를 꼭 확인한 후 해당 함수를 이용할 것 !!!!
HERROR MW_CA_NA_GetZipCode(TUnsignedInt8 ucSlotId, TUnsignedInt32 *pulZipCode)
{
	// 양방향에서 사용중.... cas에서는 CA_NA_GetZipCode 사용할 것...
	return CA_NA_CakGetZipCode(ucSlotId, pulZipCode);
}

HERROR MW_CA_NA_GetScExpirationDate(TUnsignedInt8 ucSlotId, TBoolean *pbEndOfTime, TCalendarTime *pstDataTime)
{
	return CA_NA_GetScExpirationDate(ucSlotId, pbEndOfTime, pstDataTime);
}


HERROR MW_CA_NA_IsHdPlusSmartCard(TUnsignedInt8 ucSlotId, TBoolean *pbHdPlusSc)
{
	return _naMgr_IsHdPlusSmartCard(ucSlotId, pbHdPlusSc);
}


HERROR MW_CA_NA_IsHdPlusService(TUnsignedInt32 hAction)
{
	return CA_NA_PSI_IsHdPlusService(hAction);
}


HERROR MW_CA_NA_GetPdExpirationDuration(TCaDuration *expireDuraion)
{
	return CA_NA_GetScExpirationDuration(expireDuraion);
}


HERROR MW_CA_NA_GetAccessStatus(TCaAccess *accessStatus)
{
	return CA_NA_CakGetAccessStatus(accessStatus);
}


HERROR MW_CA_NA_GetSizeOfMacCaDesc(TUnsignedInt32 hAction, TUnsignedInt16 usScCaSysId, TUnsignedInt8 *pucLen)
{
	return CA_NA_PSI_GetSizeOfMacCaDesc(hAction, usScCaSysId, pucLen);
}


HERROR MW_CA_NA_GetContentOfMacCaDesc(TUnsignedInt32 hAction, TUnsignedInt16 usScCaSysId, TUnsignedInt8 *pucDesc)
{
	return CA_NA_PSI_GetContentOfMacCaDesc(hAction, usScCaSysId, pucDesc);
}


HERROR MW_CA_NA_GetValidityHdProduct(TRightType eType, TUnsignedInt32 ucDscLen, TUnsignedInt8 *pucDesc, TBoolean *pbExpired, TCaDuration *pulDuration)
{
	return CA_NA_GetValidityHdProduct(eType, ucDscLen, pucDesc, pbExpired, pulDuration);
}
#endif


#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET) || defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT)
HERROR MW_CA_NA_NotifyTPChanged(TBoolean	bTpChanged)
{
	return CA_NA_PpvNotifyTPChanged(bTpChanged);
}


#if defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT)
HERROR MW_CA_NA_GetPPVStateInfo(NA_PPV_State_t *peState, HBOOL *pbNagraCaSys)
{
	return CA_NA_GetPPVStateInfo(peState, pbNagraCaSys);
}


HERROR MW_CA_NA_GetPackageList(HUINT32 *pulNumOfProducts, PPV_PACKAGE_LIST_ITEM **ppPackageList)
{
	return CA_NA_GetPackageList(pulNumOfProducts, ppPackageList);
}


HERROR MW_CA_NA_GetPurchaseHistoryList(HUINT32 *pulNumOfPurchase, PPV_PURCHASE_LIST_ITEM **ppPurchaseList)
{
	return CA_NA_GetPurchaseHistoryList(pulNumOfPurchase, ppPurchaseList);
}


HERROR MW_CA_NA_GetProviderName(HUINT32 *pulSize,HUINT8 **ppucProviderName)
{
	return CA_NA_GetProviderName(pulSize, ppucProviderName);
}


HERROR MW_CA_NA_ReleaseProductList(PPV_PACKAGE_LIST_ITEM *pPackageList)
{
	return CA_NA_ReleaseProductList(pPackageList);
}


HERROR MW_CA_NA_ReleasePurchaseHistoryList(PPV_PURCHASE_LIST_ITEM *pPurchaseList)
{
	return CA_NA_ReleasePurchaseHistoryList(pPurchaseList);
}


HERROR MW_CA_NA_ReleaseProviderName(HUINT8 *pProviderName)
{
	return CA_NA_ReleaseProviderName(pProviderName);
}
#endif
#endif


// for mediaset spec
#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
HERROR MW_CA_NA_RegisterScCreditCallback(TCardCreditCallback callback)
{
	return CA_NA_PpvRegisterScCreditCallback(callback);
}


HERROR MW_CA_NA_RegisterProductsLoadingCallback(TCaListenerArrivedNotifyCallback callback)
{
	return CA_NA_PpvRegisterProductsLoadingCallback(callback);
}


HERROR MW_CA_NA_RegisterTsCheckNotifyCallback(TCardCreditCallback callback)
{
	return CA_NA_PpvRegisterTSCheckNotifyCallback(callback);
}


HERROR MW_CA_NA_RegisterPurchaseHistoryCallback(TPurchaseHistoryNotifyCallback callback)
{
	return CA_NA_PpvRegisterPurchaseHistoryNotifyCallback(callback);
}


HERROR MW_CA_NA_RegisterRechargeListCallback(TRechargeListNotifyCallback callback)
{
	return CA_NA_PpvRegisterRechargeListNotifyCallback(callback);
}


HERROR MW_CA_NA_RegisterOppvPurchaseListCallback(TOPPVPurchaseListNotifyCallback callback)
{
	return CA_NA_PpvRegisterOppvPurchaseListNotifyCallback(callback);
}


HERROR MW_CA_NA_RegisterRechargeAlarmCallback(TRechargeNotifyCallback callback)
{
	return CA_NA_PpvRegisterRechargeAlarmNotifyCallback(callback);
}


HERROR MW_CA_NA_RegisterEventInfoCallback(TRequestEventNotifyCallback callback)
{
	//CAK에서 CAK_EVENT_INFO_RESPONSE을 받게 됐을 때 call될 callback을 등록한다.
	return CA_NA_PpvRegisterEventInfoNotifyCallback(callback);
}


HERROR MW_CA_NA_RegisterPackageListCallback(TPackageListNotifyCallback callback)
{
	return CA_NA_PpvRegisterPackageListNotifyCallback(callback);
}


HERROR MW_CA_NA_RegisterFullInfoPackageListNotifyCallback(TPackageListNotifyCallback callback)
{
	return CA_NA_PpvRegisterFullInfoPackageListNotifyCallback(callback);
}


HERROR MW_CA_NA_RegisterRequestPurchaseEventCallback(TPurchaseEventNotifyCallback callback)
{
	return CA_NA_PpvRegisterRequestPurchaseEventNotifyCallback(callback);
}


HERROR MW_CA_NA_RegisterRequestPurchasePackageCallback(TPurchasePackageNotifyCallback callback)
{
	return CA_NA_PpvRegisterRequestPurchasePackageNotifyCallback(callback);
}


HERROR MW_CA_NA_RegisterDgtviScAlarmCallback(TSmartcardAlarmCallback callback)
{
	return CA_NA_CakRegisterDgtviScAlarmCallback(callback);
}


HERROR MW_CA_NA_TsCheck(Handle_t hSvcHandle)
{
	return CA_NA_PpvRequestTsCheck(hSvcHandle);
}


HERROR MW_CA_NA_RequestEventInfo(void)
{
	return CA_NA_PpvRequestEventInfo();
}


HERROR MW_CA_NA_RequestPurchaseHistory(TUnsignedInt32 ulProviderId)
{
	return CA_NA_PpvRequestPurchaseHistory(ulProviderId);
}


HERROR MW_CA_NA_RequestRechargeList(void)
{
	return CA_NA_PpvRequestRechargeList();
}


HERROR MW_CA_NA_GetScExpireDateForPpv(TUnsignedInt8 *pszDate, TBoolean *bExpired)
{
	TBoolean bEndOfTime;
	TCalendarTime stDataTime;
	HERROR	res;

#ifndef WIN32
	if(pszDate == NULL)
		return ERR_FAIL;

	res = CA_NA_GetScExpirationDate(0, &bEndOfTime, &stDataTime);
	if(res != ERR_FAIL)
	{
		snprintf(pszDate, 16, "%02d/%02d/%04d\n", stDataTime.day, stDataTime.month, stDataTime.year+1900);
	}
	*bExpired = bEndOfTime;

	return res;
#else
	snprintf(pszDate, 16, "%s/%s/%s\n", "18", "09", "2010");
	*bExpired = FALSE;
	return ERR_OK;
#endif
}


HERROR MW_CA_NA_GetCurrentDateStr(TUnsignedInt8 *pszDate)
{
	HERROR			hError = ERR_FAIL;
	UNIXTIME		ulSystemTime = 0;
	HxDATETIME_t 	stDateTime;

	if (!pszDate)
	{
		HxLOG_Error("pszDate is NULL!!\n");
		return ERR_FAIL;
	}

	hError = (HERROR)VK_CLOCK_GetTime((unsigned long*)&ulSystemTime);
	if (hError != VK_OK || UNIX_FIRST_DAY >= ulSystemTime)
	{
		ulSystemTime = UNIX_FIRST_DAY;
	}

	hError = HLIB_DATETIME_ConvertUnixTimeToDateTime(ulSystemTime, &stDateTime);
	if (hError != ERR_OK)
	{
		HxLOG_Error("[xmgr_cas_Nappv_mset_scerror_SetCurrentDate] ConvertUnixTimeToDateTime fail!!\n");
		return ERR_FAIL;
	}

	snprintf(pszDate, 32, "%02d/%02d/%04d", stDateTime.stDate.ucDay, stDateTime.stDate.ucMonth, stDateTime.stDate.usYear);

	return ERR_OK;
}


HERROR MW_CA_NA_GetScCreditStr(TUnsignedInt8 *pszCredit)
{
	TCreditAmount  	xCredit;
	HERROR			res;

#ifndef WIN32
	res = CA_NA_PpvGetScCredit(&xCredit);
	if(res != ERR_FAIL)
	{
		snprintf(pszCredit, 16, "%d,%02d", xCredit/100, xCredit%100);
	}

	return res;
#else
	snprintf(pszCredit, 16, "%s", "34,00");
	return ERR_OK;
#endif
}


HERROR MW_CA_NA_GetScCredit(TCreditAmount *pCredit)
{
#ifndef WIN32
	return CA_NA_PpvGetScCredit(pCredit);
#else
	*pCredit = 3400;
	return ERR_OK;
#endif
}


HERROR MW_CA_NA_NotifySvcChanged(Handle_t	hSvc, Handle_t hAction)
{
	return CA_NA_PpvNotifySvcChanged(hSvc, hAction);
}


HERROR MW_CA_NA_DisposeRequest(TUnsignedInt32* pRequest)
{
	return CA_NA_PpvDisposeRequest((TCaRequest*)pRequest);
}


HERROR MW_CA_NA_CakGetProviderId(TPpid *pProviderId)
{
	return CA_NA_PpvGetProviderId(pProviderId);
}


HERROR MW_CA_NA_RequestFullInfoPackageList(TPpid *pProviderId)
{
	return CA_NA_PpvRequestFullInfoPackageList(pProviderId);
}


HERROR MW_CA_NA_RequestPackageList(TUnsignedInt32 ulProviderId)
{
	return CA_NA_PpvRequestPackageList(ulProviderId);
}


HERROR MW_CA_NA_RequestEventPurchase(TPurchasableEventType xStatus, TCaRequest* pRequest, TCaEvent *pEvent, TCaProduct *pProduct)
{
	return CA_NA_PpvRequestEventPurchase(xStatus, pRequest, pEvent, pProduct);
}


HERROR MW_CA_NA_RequestPackagePurchase(TCaProduct* pProduct)
{
	return CA_NA_PpvRequestPackagePurchase(pProduct);
}


HERROR MW_CA_NA_RequestProductPurchasable(TCaRequest* pRequest, TCaProduct*  pProduct)
{
	return CA_NA_PpvRequestProductPurchasable(pRequest, pProduct);
}


HERROR MW_CA_NA_GetProductGetDescription(const TCaProduct* pxProduct, TSize* pStrLen, TUnsignedInt8** ppBuffer)
{
	return CA_NA_PpvGetProductGetDescription(pxProduct, pStrLen, ppBuffer);
}


HERROR MW_CA_NA_GetProductGetDisclaimer(const TCaProduct* pxProduct, TSize* pStrLen, TUnsignedInt8** ppBuffer)
{
	return CA_NA_PpvGetProductGetDisclaimer(pxProduct, pStrLen, ppBuffer);
}


HERROR MW_CA_NA_GetPackageEventInfo(TCaRequest* pRequest, TCaProduct* pPackage, PACKAGE_INFO **pPackageInfo)
{
	return CA_NA_PpvGetPackageEventInfo(pRequest, pPackage, pPackageInfo);
}


HERROR MW_CA_NA_GetPurchaseHistoryNum(TUnsignedInt32* pNbItems)
{
	return CA_NA_PpvGetPurchaseHistoryNum(pNbItems);
}
#endif	//end of #if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)


// for rcs spec
#if	defined(CONFIG_MW_CAS_NAGRA_OP_RCS)
HERROR  MW_CA_NA_PPV_SetEpgMode(TBoolean xEpgMode)
{
	return CA_NA_PPV_SetEpgMode(xEpgMode);
}


HERROR  MW_CA_NA_PPV_ForceCreatEventInfo(TUnsignedInt32 svcHandle)
{
	return CA_NA_PPV_ForceCreatEventInfo(svcHandle);
}


HERROR MW_CA_NA_PPV_GetPpvList(TEventPurchaseListStatus* xStatus, TUnsignedInt8* xNumberOfItems, TEventPurchaseListItem** pxEventPurchaseListArray)
{
	return CA_NA_PPV_GetPpvList(xStatus, xNumberOfItems, pxEventPurchaseListArray);
}


HERROR  MW_CA_NA_PPV_GetCredit(TCreditStatus* xStatus, TUnsignedInt8* xNumberOfItems, TCreditItem** pxCreditArray)
{
	return CA_NA_PPV_GetCredit(xStatus, xNumberOfItems, pxCreditArray);
}


HERROR 	MW_CA_NA_PPV_GetPurchaseEvtInfo(TUnsignedInt32 svcHandle, TUnsignedInt32 currTime, TUnsignedInt16 *pusEventId)
{
	return CA_NA_PPV_GetPurchaseEvtInfo(svcHandle, currTime, eventId);
}


HERROR MW_CA_NA_PPV_RequestEventInfo(TUnsignedInt32 svcHandle, TUnsignedInt16 nEvtId)
{
	return CA_NA_PPV_RequestEventInfo(svcHandle, nEvtId);
}


HERROR MW_CA_NA_PPV_RequestEventInfoByEpg(TUnsignedInt32 svcHandle, TUnsignedInt16 nEvtId,
															TUnsignedInt32 *xStatus, TCaRequest **pRequest, PPV_EVENT_INFO **pEventInfo, TCaProduct **pProduct)
{
	return CA_NA_PPV_RequestEventInfoByEpg(svcHandle, nEvtId, xStatus, pRequest, pEventInfo, pProduct);
}


HERROR  MW_CA_NA_PPV_RequestPurchaseEvent(TPurchasableEventType xType, TCaRequest* pRequest, TCaProduct *pProduct)
{
	return CA_NA_PPV_RequestPurchaseEvent(xType, pRequest, pProduct);
}


HERROR  MW_CA_NA_PPV_RequestPurchaseEventByEpg(TPurchasableEventType xType, TCaRequest* pRequest, TCaProduct *pProduct, TCaProcessingStatus *pProStatus)
{
	return CA_NA_PPV_RequestPurchaseEventByEpg(xType, pRequest, pProduct, pProStatus);
}


HERROR  MW_CA_NA_PPV_RegisterCreditCallback(TCreditCallback callback)
{
	return CA_NA_PPV_RegisterCreditCallback(callback);
}


HERROR  MW_CA_NA_PPV_RegisterPpvListCallback(TEventPurchaseListCallback callback)
{
	return CA_NA_PPV_RegisterPpvListCallback(callback);
}


HERROR  MW_CA_NA_PPV_RegisterRechargeNotifyCallback(TRechargeNotifyCallback callback)
{
	return CA_NA_PPV_RegisterRechargeNotifyCallback(callback);
}


HERROR  MW_CA_NA_PPV_RegisterRequestPurchaseEventNotifyCallback(TPurchaseEventNotifyCallback callback)
{
	return CA_NA_PPV_RegisterRequestPurchaseEventNotifyCallback(callback);
}


HERROR  MW_CA_NA_PPV_RegisterRequestEventNotifyCallback(TRequestEventNotifyCallback callback)
{
	return CA_NA_PPV_RegisterRequestEventNotifyCallback(callback);
}


HERROR  MW_CA_NA_PPV_RegisterPurchaseHistoryNotifyCallback(TPurchaseHistoryNotifyCallback callback)
{
	return CA_NA_PPV_RegisterPurchaseHistoryNotifyCallback(callback);
}
#endif


#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
HERROR MW_CA_NA_GetSlotIDbySessionID(TIccSessionId iccSessionId, TUnsignedInt32 *pulSlotId)
{
	TUnsignedInt32			i;

	extern TUnsignedInt32		g_ulNumOfSlot;
	extern ScMgrIntance_t		*g_pstScMgrInstance;

	for (i=0; i<g_ulNumOfSlot; i++)
	{
		if (g_pstScMgrInstance[i].ulSessionId == iccSessionId)
		{
			*pulSlotId = i;
			return ERR_OK;
		}
	}
	return ERR_FAIL;
}
#endif


#if defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
HBOOL MW_CA_NA_CheckTivuService(void)
{
	return s_bTivuService;
}


/*Set*/
HERROR MW_CA_NA_SetTivuService(HBOOL bTivu)
{
	HxLOG_Print("bTivu = %d\n", bTivu);
	s_bTivuService = (TUnsignedInt32)bTivu;
	return ERR_OK;
}


HBOOL MW_CA_NA_CheckTivuCardSystemId(void)
{
	return _naMgr_TIVU_CheckCardSystemId();
}
#endif

HUINT8 *NAMGR_GetPairingData(void)
{
#if defined(CONFIG_DEBUG)
{
	TUnsignedInt16 i;

	for (i=0; i<MAX_PD_SIZE; i++)
	{
		if ( (i != 0) && ((i%16) == 0) )
			HxLOG_Print("\n");
		HxLOG_Print(" %02X", s_aucPairingData[i]);
	}
	HxLOG_Print("\n");
}
#endif
	return s_aucPairingData;
}

HERROR NAMGR_SetMetaDescriptor(TTransportSessionId ulSessionId, MetaTagType_t eMetaType, TUnsignedInt8 *pucMeta, TUnsignedInt8 ucMetaSize)
{
	static HUINT32 s_ulVersion = 0;

	Handle_t	hCasHandle;
	DxCopyrightControl_t	*pstDrmInfo = NULL;

	if (pucMeta == NULL)
	{
		HxLOG_Error("error pucMeta == NULL\n");
		return ERR_FAIL;
	}

	switch(eMetaType)
	{
		case eTagCopyCtlInfo:
			HxLOG_Print("[%s:%d] Not implemented...\n", __FUNCTION__, __HxLINE__);
			return ERR_OK;

		case eTagUriInfo:
			pstDrmInfo = (DxCopyrightControl_t *)OxCAS_Malloc(sizeof(DxCopyrightControl_t));
			if(pstDrmInfo != NULL)
			{
				DxCopyrightControl_NagraUri_t	*pstUri;

				pstDrmInfo->stCasCcInfo.eCasCcType = eDxCopyrightControl_CasCcType_Nagra;
				pstDrmInfo->stCasCcInfo.info.stNagraDrmInfo.eType = eDxCopyrightControl_NagraDrmType_Uri;

				pstUri = &pstDrmInfo->stCasCcInfo.info.stNagraDrmInfo.info.stUri;

				pstUri->ucProtocol	= pucMeta[0];
				pstUri->ucAps		= (pucMeta[1] >> 6) & 0x03;
				pstUri->ucEmi		= (pucMeta[1] >> 4) & 0x03;
				pstUri->ucIct		= (pucMeta[1] >> 3) & 0x01;
				pstUri->ucRct		= (pucMeta[1] >> 2) & 0x01;
				pstUri->ucRL		= pucMeta[2] & 0x3f;

				if( (ucMetaSize-3) && ((ucMetaSize-3) <= 5) )
				{
					HxSTD_memcpy(pstUri->aucReserved, pucMeta+ 3, ucMetaSize-3);
				}

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
#if defined(CONFIG_DEBUG)
				HxLOG_Print("\t\t UriInfo(0x4) :\n");
				HxLOG_Print("\t\t\t  pstUri->ucProtocol = 0x%x \n",  pstUri->ucProtocol);
				HxLOG_Print("\t\t\t  pstUri->ucAps = 0x%x \n", pstUri->ucAps);
				HxLOG_Print("\t\t\t  pstUri->ucEmi = 0x%x \n", pstUri->ucEmi);
				HxLOG_Print("\t\t\t  pstUri->ucIct = 0x%x \n", pstUri->ucIct);
				HxLOG_Print("\t\t\t  pstUri->ucRct = 0x%x \n", pstUri->ucRct);
				HxLOG_Print("\t\t\t  pstUri->ucRL = 0x%x \n", pstUri->ucRL);
#else
{
				HUINT8 ucTmp[128];

				MW_CA_NA_PrintForRelease("UriInfo(0x4) :\n");

				HxSTD_memset(ucTmp, 0x00, 128);
				snprintf(ucTmp, 128, "pstUri->ucProtocol = 0x%x", pstUri->ucProtocol);
				MW_CA_NA_PrintForRelease(ucTmp);

				HxSTD_memset(ucTmp, 0x00, 128);
				snprintf(ucTmp, 128, "pstUri->ucAps = 0x%x \n", pstUri->ucAps);
				MW_CA_NA_PrintForRelease(ucTmp);

				HxSTD_memset(ucTmp, 0x00, 128);
				snprintf(ucTmp, 128, "pstUri->ucEmi = 0x%x \n", pstUri->ucEmi);
				MW_CA_NA_PrintForRelease(ucTmp);

				HxSTD_memset(ucTmp, 0x00, 128);
				snprintf(ucTmp, 128, "pstUri->ucIct = 0x%x \n", pstUri->ucIct);
				MW_CA_NA_PrintForRelease(ucTmp);

				HxSTD_memset(ucTmp, 0x00, 128);
				snprintf(ucTmp, 128, "pstUri->ucRct = 0x%x \n", pstUri->ucRct);
				MW_CA_NA_PrintForRelease(ucTmp);

				HxSTD_memset(ucTmp, 0x00, 128);
				snprintf(ucTmp, 128, "pstUri->ucRL = 0x%x \n", pstUri->ucRL);
				MW_CA_NA_PrintForRelease(ucTmp);
}

#endif
#endif
			}
			break;

		default:
			HxLOG_Error("Unknown metadata...(0x%x)\n", eMetaType);
			return ERR_FAIL;
	}

	CA_NA_PsiGetCasInstanceID(&hCasHandle);

	if(svc_cas_MgrSubUpdateDrmInfo((Handle_t)ulSessionId, hCasHandle, (void*)pstDrmInfo, eDxCopyrightControl_CasCcType_Nagra, s_ulVersion++) != ERR_OK)
	{
		HxLOG_Error("error in svc_cas_MgrSubUpdateDrmInfo\n");
		goto _ERROR;
	}

	return ERR_OK;

_ERROR:
	if(pstDrmInfo != NULL)
		OxCAS_Free(pstDrmInfo);

	return ERR_FAIL;
}

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
void NAMGR_SetParentalControl(Handle_t hAction, TBoolean bUserPinPass)
{
	NAMGR_CAKSVC_SetParentalControl(hAction, bUserPinPass);
}

void NAMGR_SetParentalControlLevel(void)
{
	NAMGR_CAKMGR_SetParentalControlLevel();
}
#endif

HERROR NAMGR_FactoryDefault(void)
{
	HERROR hErr = ERR_OK;

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD)
	hErr = CA_NA_IRDCMD_ResetIRDCmdData();

	if(hErr != ERR_OK)
	{
		HxLOG_Error("Error in CA_NA_IRDCMD_ResetIRDCmdData() \n");
	}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_PRM)
	hErr |= CA_NA_DvlResetPersistentMemory();
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Error in CA_NA_DvlResetPersistentMemory() \n");
	}
#endif

	return hErr;
}

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST) && !defined(CONFIG_DEBUG)
void MW_CA_NA_InitTerminalForRelease(void)
{
	struct termios tio;
	struct termios stdio;

	memset(&stdio, 0, sizeof(stdio));
	stdio.c_iflag = 0;
	stdio.c_oflag = 0;
	stdio.c_cflag = 0;
	stdio.c_lflag = 0;
	stdio.c_cc[VMIN] = 1;
	stdio.c_cc[VTIME] = 0;
	tcsetattr(STDOUT_FILENO, TCSANOW, &stdio);
	tcsetattr(STDOUT_FILENO, TCSAFLUSH, &stdio);
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);       // make the reads non-blocking

	memset(&tio, 0, sizeof(tio));
	tio.c_iflag = 0 /*ICRNL*/;
	tio.c_oflag = 0 /*ONLCR*/;
	tio.c_cflag = CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
	tio.c_lflag = 0;
	tio.c_cc[VMIN] = 1;
	tio.c_cc[VTIME] = 5;

	s_ulTtyFd = open("/dev/ttyS0", O_RDWR | O_NONBLOCK);
	cfsetospeed(&tio, B115200);            // 115200 baud
	cfsetispeed(&tio, B115200);            // 115200 baud

	tcsetattr(s_ulTtyFd, TCSANOW, &tio);

	//close(s_ulTtyFd);
}

void MW_CA_NA_PrintForRelease(const HUINT8 *pxString)
{
	HUINT32	ulLen = 0, ulCnt = 0;
	HUINT8	ucChar = 0, ucPreChar = 0, ucCharCr = '\r';

	if(pxString != NULL)
	{
		ulLen = HxSTD_StrLen(pxString);
		ulCnt = 0;
		while(1)
		{
			ucChar = pxString[ulCnt++];

			if(ucChar == '\n' && ucPreChar != ucCharCr)	/* LF만 올경우 CR/LF로 처리해 주자! */
			{
				write(s_ulTtyFd, (void *)&ucCharCr, 1);
			}
			write(s_ulTtyFd, (void *)&ucChar, 1);

			ucPreChar = ucChar;

			if(ulCnt >= ulLen)
				break;
		}
	}
}
#endif

#if defined(CONFIG_DEBUG)
HCHAR *NAMGR_GetStrSmartcardState(TSmartcardState eSmartcard)
{
	switch(eSmartcard)
	{
		ENUM_TO_STR(CA_SMARTCARD_STATE_OK);
		ENUM_TO_STR(CA_SMARTCARD_STATE_ERROR);
		ENUM_TO_STR(CA_SMARTCARD_STATE_MUTE);
		ENUM_TO_STR(CA_SMARTCARD_STATE_INVALID);
		ENUM_TO_STR(CA_SMARTCARD_STATE_BLACKLISTED);
		ENUM_TO_STR(CA_SMARTCARD_STATE_SUSPENDED);
		ENUM_TO_STR(CA_SMARTCARD_STATE_NEVER_PAIRED);
		ENUM_TO_STR(CA_SMARTCARD_STATE_NOT_PAIRED);
		ENUM_TO_STR(CA_SMARTCARD_STATE_EXPIRED);
		ENUM_TO_STR(CA_SMARTCARD_STATE_NOT_CERTIFIED);
		ENUM_TO_STR(CA_SMARTCARD_STATE_INCOMPATIBLE);
	}

	return "UNKNOWN";
}

HCHAR *NAMGR_GetStrAccessState(TCaAccess eAccess)
{
	switch(eAccess)
	{
		ENUM_TO_STR(CA_ACCESS_CLEAR);
		ENUM_TO_STR(CA_ACCESS_GRANTED);
		ENUM_TO_STR(CA_ACCESS_FREE);
		ENUM_TO_STR(CA_ACCESS_DENIED);
		ENUM_TO_STR(CA_ACCESS_NO_VALID_SECURE_DEVICE);
		ENUM_TO_STR(CA_ACCESS_SMARTCARD_BLACKLISTED);
		ENUM_TO_STR(CA_ACCESS_SMARTCARD_SUSPENDED);
		ENUM_TO_STR(CA_ACCESS_BLACKED_OUT);
		ENUM_TO_STR(CA_ACCESS_DENIED_NO_VALID_CREDIT);
		ENUM_TO_STR(CA_ACCESS_DENIED_COPY_PROTECTED);
		ENUM_TO_STR(CA_ACCESS_DENIED_PARENTAL_CONTROL);
		ENUM_TO_STR(CA_ACCESS_DENIED_DIALOG_REQUIRED);
		ENUM_TO_STR(CA_ACCESS_DENIED_PAIRING_REQUIRED);
		ENUM_TO_STR(CA_ACCESS_DENIED_CHIPSET_PAIRING_REQUIRED);
		ENUM_TO_STR(CA_ACCESS_EMI_UNSUPPORTED);
		ENUM_TO_STR(CA_ACCESS_GRANTED_PPT);
		ENUM_TO_STR(CA_ACCESS_DENIED_FOR_PARENTAL_RATING);
		ENUM_TO_STR(CA_ACCESS_RIGHT_SUSPENDED);
		ENUM_TO_STR(CA_ACCESS_DENIED_BUT_PPT);
	}

	return "UNKNOWN";
}

HCHAR *NAMGR_GetStrCasActionType(SvcCas_ActionType_e eActType)
{
	switch(eActType)
	{
		ENUM_TO_STR(eCAS_ActionType_None);
		ENUM_TO_STR(eCAS_ActionType_Live);
		ENUM_TO_STR(eCAS_ActionType_Playback);
		ENUM_TO_STR(eCAS_ActionType_Recording);
		ENUM_TO_STR(eCAS_ActionType_Descramble_a_File);
		ENUM_TO_STR(eCAS_ActionType_Tsr_Play);
		ENUM_TO_STR(eCAS_ActionType_Tsr_Rec);
		ENUM_TO_STR(eCAS_ActionType_Data);
		ENUM_TO_STR(eCAS_ActionType_EngineeringTS);
		ENUM_TO_STR(eCAS_ActionType_RfVod);
		ENUM_TO_STR(eCAS_ActionType_Max);
	}

	return "UNKNOWN";
} SvcCas_ActionType_e;

/* from flash_map.h */
#define NAGRA_PK_SIZE							1020
#define NAGRA_PK_LOGICAL_OFFSET					(0x00020000)

static HERROR local_ca_na_CmdEncryptPkFromUsb(void)
{
	HUINT8		aucFileName[128] = {0, };
	HUINT8		*pPkData, *pPkBuf, *pEncPkData;
	HUINT32		ulPkSize = NAGRA_PK_SIZE + 4, ulPkFileSize;
#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	TSecFunctionTable* secFunctionTable = secGetFunctionTable();
#endif

	HxSTD_sprintf(aucFileName, "/media/drive1/%s", "Pure_PK.bin");

	HLIB_CMD_Printf("PK File Name : [%s]\n", aucFileName);

	if(HLIB_FILE_ReadFile((const HUINT8 *)aucFileName, (void **)&pPkData, &ulPkFileSize) != ERR_OK)
	{
		HLIB_CMD_Printf("[%s][%d] Error in HLIB_FILE_ReadFile[%s]\n", __FUNCTION__, __LINE__, aucFileName);
		return ERR_FAIL;
	}

	if(ulPkFileSize > NAGRA_PK_SIZE)
	{
		HLIB_CMD_Printf("[%s][%d] The File size is too big. len[%d]\n", __FUNCTION__, __LINE__, ulPkSize);
		HLIB_MEM_Free(pPkData);
		return ERR_FAIL;
	}

	pPkBuf = (HUINT8 *)HLIB_MEM_Malloc(ulPkSize);
	if(pPkBuf == NULL)
	{
		HLIB_CMD_Printf("[%s][%d] Error in HLIB_MEM_Malloc\n", __FUNCTION__, __LINE__);
		HLIB_MEM_Free(pPkData);
		return ERR_FAIL;
	}

	HxSTD_memset(pPkBuf, 0, ulPkSize);
	HxSTD_MemCopy(pPkBuf, pPkData, ulPkFileSize);

	pEncPkData = (HUINT8 *)HLIB_MEM_Malloc(ulPkSize);

	if(pEncPkData == NULL)
	{
		HLIB_CMD_Printf("[%s][%d] Error in HLIB_MEM_Malloc\n", __FUNCTION__, __LINE__);
		HLIB_MEM_Free(pPkData);
		HLIB_MEM_Free(pPkBuf);
		return ERR_FAIL;
	}

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	secFunctionTable->secEncryptData(pEncPkData, pPkBuf, ulPkSize);
#else
	secEncryptData(pEncPkData, pPkBuf, ulPkSize);
#endif

	HxSTD_sprintf(aucFileName, "/media/drive1/%s", "Encrypted_PK_Out.bin");
	//HxSTD_sprintf(aucFileName, "/%s", "Encrypt_PK_Out.bin");

	HLIB_CMD_Printf("Encrypt PK File Name : [%s]\n", aucFileName);

	HLIB_FILE_Delete(aucFileName);
	if(HLIB_FILE_WriteFile((const HUINT8 *)aucFileName, (void *)pEncPkData, NAGRA_PK_SIZE) != ERR_OK)
	{
		HLIB_MEM_Free(pPkData);
		HLIB_MEM_Free(pPkBuf);
		HLIB_MEM_Free(pEncPkData);
		HLIB_CMD_Printf("[%s][%d] Error in HLIB_FILE_WriteFile[%s]\n", __FUNCTION__, __LINE__, aucFileName);
		return ERR_FAIL;
	}

	HLIB_CMD_Printf("Secceed to write the PK : [%s]\n", aucFileName);

	HLIB_MEM_Free(pPkData);
	HLIB_MEM_Free(pPkBuf);
	HLIB_MEM_Free(pEncPkData);
	return ERR_OK;
}

static HERROR local_ca_na_CmdWritePkFromUsb(void)
{
	HUINT8		aucFileName[128] = {0, };
	HUINT8		*pPkData, *pPkBuf, *pEncPkData, *pTmpBuf;
	HUINT32		ulPkSize = NAGRA_PK_SIZE + 4, ulPkFileSize;
#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	TSecFunctionTable* secFunctionTable = secGetFunctionTable();
#endif

	HxSTD_sprintf(aucFileName, "/media/drive1/%s", "Pure_PK.bin");

	HLIB_CMD_Printf("PK File Name : [%s]\n", aucFileName);

	if(HLIB_FILE_ReadFile((const HUINT8 *)aucFileName, (void **)&pPkData, &ulPkFileSize) != ERR_OK)
	{
		HLIB_CMD_Printf("[%s][%d] Error in HLIB_FILE_ReadFile[%s]\n", __FUNCTION__, __LINE__, aucFileName);
		return ERR_FAIL;
	}

	if(ulPkFileSize > NAGRA_PK_SIZE)
	{
		HLIB_CMD_Printf("[%s][%d] The File size is too big. len[%d]\n", __FUNCTION__, __LINE__, ulPkSize);
		HLIB_MEM_Free(pPkData);
		return ERR_FAIL;
	}

	pPkBuf = (HUINT8 *)HLIB_MEM_Malloc(ulPkSize);
	if(pPkBuf == NULL)
	{
		HLIB_CMD_Printf("[%s][%d] Error in HLIB_MEM_Malloc\n", __FUNCTION__, __LINE__);
		HLIB_MEM_Free(pPkData);
		return ERR_FAIL;
	}

	HxSTD_memset(pPkBuf, 0xFF, ulPkSize);
	HxSTD_MemCopy(pPkBuf, pPkData, ulPkFileSize);

	pEncPkData = (HUINT8 *)HLIB_MEM_Malloc(ulPkSize);

	if(pEncPkData == NULL)
	{
		HLIB_CMD_Printf("[%s][%d] Error in HLIB_MEM_Malloc\n", __FUNCTION__, __LINE__);
		HLIB_MEM_Free(pPkData);
		HLIB_MEM_Free(pPkBuf);
		return ERR_FAIL;
	}

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	secFunctionTable->secEncryptData(pEncPkData, pPkBuf, ulPkSize);
#else
	secEncryptData(pEncPkData, pPkBuf, ulPkSize);
#endif

	pTmpBuf = (HUINT8 *)HLIB_MEM_Malloc(1024 * 128);
	if(DI_NANDFLASH_Read(0x00000000 + NAGRA_PK_LOGICAL_OFFSET, pTmpBuf, 1024 * 128) != 0)
	{
		HLIB_CMD_Printf("[%s][%d] Error in DI_NANDFLASH_Write\n", __FUNCTION__, __LINE__);
		HLIB_MEM_Free(pPkData);
		HLIB_MEM_Free(pPkBuf);
		HLIB_MEM_Free(pEncPkData);
		HLIB_MEM_Free(pTmpBuf);
		return ERR_FAIL;
	}

	HxSTD_MemCopy(pTmpBuf, pEncPkData, ulPkSize);

	/* 원래 DI를 직접 사용하면 않되는데.... command tool을 위한 거니까.. ^^ */
	if(DI_NANDFLASH_Write(0x00000000 + NAGRA_PK_LOGICAL_OFFSET, pTmpBuf, 1024 * 128) != 0)
	{
		HLIB_CMD_Printf("[%s][%d] Error in DI_NANDFLASH_Write\n", __FUNCTION__, __LINE__);
		HLIB_MEM_Free(pPkData);
		HLIB_MEM_Free(pPkBuf);
		HLIB_MEM_Free(pEncPkData);
		HLIB_MEM_Free(pTmpBuf);
		return ERR_FAIL;
	}

	HLIB_CMD_Printf("Secceed to write the PK : [%s]\n", aucFileName);

	HLIB_MEM_Free(pPkData);
	HLIB_MEM_Free(pPkBuf);
	HLIB_MEM_Free(pEncPkData);
	HLIB_MEM_Free(pTmpBuf);
	return ERR_OK;
}

static HERROR local_ca_na_CmdReadPkToUsb(void)
{
	HUINT8		aucFileName[128] = {0, };
	HUINT8		*pPkData, *pEncPkData;
	HUINT32		ulPkSize = NAGRA_PK_SIZE + 4;
#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	TSecFunctionTable* secFunctionTable = secGetFunctionTable();
#endif

	pPkData = (HUINT8 *)HLIB_MEM_Malloc(ulPkSize);
	if(pPkData == NULL)
	{
		return ERR_FAIL;
	}

	pEncPkData = (HUINT8 *)HLIB_MEM_Malloc(ulPkSize);
	if(pEncPkData == NULL)
	{
		HLIB_MEM_Free(pPkData);
		return ERR_FAIL;
	}
	HxSTD_memset(pEncPkData, 0, ulPkSize);

	/* 원래 DI를 직접 사용하면 않되는데.... command tool을 위한 거니까.. ^^ */
	if(DI_NANDFLASH_Read(0x00000000 + NAGRA_PK_LOGICAL_OFFSET, pEncPkData, ulPkSize - 4) != 0)
	{
		HLIB_CMD_Printf("[%s][%d] Error in DI_NANDFLASH_Read\n", __FUNCTION__, __LINE__);
		HLIB_MEM_Free(pPkData);
		HLIB_MEM_Free(pEncPkData);
		return ERR_FAIL;
	}

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	secFunctionTable->secDecryptData(pPkData, pEncPkData, ulPkSize);
#else
	secDecryptData(pPkData, pEncPkData, ulPkSize);
#endif

	HxSTD_sprintf(aucFileName, "/media/drive1/%s", "Pure_PK_Out.bin");
	//HxSTD_sprintf(aucFileName, "/%s", "Pure_PK_Out.bin");

	HLIB_CMD_Printf("PK File Name : [%s]\n", aucFileName);

	HLIB_FILE_Delete(aucFileName);
	if(HLIB_FILE_WriteFile((const HUINT8 *)aucFileName, (void *)pPkData, ulPkSize - 4) != ERR_OK)
	{
		HLIB_CMD_Printf("[%s][%d] Error in HLIB_FILE_WriteFile[%s]\n", __FUNCTION__, __LINE__, aucFileName);
		HLIB_MEM_Free(pPkData);
		HLIB_MEM_Free(pEncPkData);
		return ERR_FAIL;
	}

	HLIB_CMD_Printf("Seccess to extract the pure PK : [%s]\n", aucFileName);
	HLIB_MEM_Free(pPkData);
	HLIB_MEM_Free(pEncPkData);
	return ERR_OK;
}


/* from flash_map.h */
#if defined(CONFIG_PROD_HGS1000S)
#define NAGRA_CSC_LOGICAL_OFFSET			(0x140000)
#else
#define NAGRA_CSC_LOGICAL_OFFSET			(0x002C0000)
#endif

/* from ca_os.c */
#if defined (CONFIG_MW_CAS_NAGRA_NOCS_3_X)
// a CSC data record shall be probrammed in NVM during the STB personallization.
// The maximux size of this record depends on the type of chipset integrated and is defined as folows:
// NOCS1 : 256 byte
// NOCS3 : 3000 byte
#define OS_PERSISTENT_CSC_FLASH_SIZE				(3000)
#else
#define OS_PERSISTENT_CSC_FLASH_SIZE				(256)
#endif
static HERROR local_ca_na_CmdWriteCSCFromUsb(HBOOL bToNvram)
{
	HUINT8		aucFileName[128] = {0, };
	HUINT8		*pCscData;
	HUINT32		ulCscSize;

	HxSTD_sprintf(aucFileName, "/media/drive1/%s", "CSC.bin");

	HLIB_CMD_Printf("CSC File Name : [%s]\n", aucFileName);

	if(HLIB_FILE_ReadFile((const HUINT8 *)aucFileName, (void **)&pCscData, &ulCscSize) != ERR_OK)
	{
		HLIB_CMD_Printf("[%s][%d] Error in HLIB_FILE_ReadFile[%s]\n", __FUNCTION__, __LINE__, aucFileName);
		return ERR_FAIL;
	}

	if(ulCscSize > OS_PERSISTENT_CSC_FLASH_SIZE)
	{
		HLIB_CMD_Printf("[%s][%d] The File size is too big. len[%d]\n", __FUNCTION__, __LINE__, ulCscSize);
		HLIB_MEM_Free(pCscData);
		return ERR_FAIL;
	}

	if(bToNvram != TRUE)
	{
		/* 원래 DI를 직접 사용하면 않되는데.... command tool을 위한 거니까.. ^^ */
		if(DI_NANDFLASH_Write(0x00000000 + NAGRA_CSC_LOGICAL_OFFSET, pCscData, ulCscSize) != 0)
		{
			HLIB_CMD_Printf("[%s][%d] Error in DI_NANDFLASH_Write\n", __FUNCTION__, __LINE__);
			HLIB_MEM_Free(pCscData);
			return ERR_FAIL;
		}
	}
	else
	{
		if(svc_cas_DevSysSetField(eCAS_DEV_FLASH_FIELD_CSC_KEY, 0, (void*)pCscData, ulCscSize) != ERR_OK)
		{
			HLIB_CMD_Printf("[%s][%d] Error in svc_cas_DevSysSetField\n", __FUNCTION__, __LINE__);
			HLIB_MEM_Free(pCscData);
			return ERR_FAIL;
		}
	}

	HLIB_CMD_Printf("Secceed to write the CSC : [%s]\n", aucFileName);

	HLIB_MEM_Free(pCscData);
	return ERR_OK;
}

static HERROR local_ca_na_CmdReadCSCToUsb(void)
{
	HUINT8		aucFileName[128] = {0, };
	HUINT8		*pCscData;
	HUINT32		ulCscSize = OS_PERSISTENT_CSC_FLASH_SIZE, ulNuid;

	pCscData = (HUINT8 *)HLIB_MEM_Malloc(ulCscSize);
	if(pCscData == NULL)
	{
		return ERR_FAIL;
	}

	/* 원래 DI를 직접 사용하면 않되는데.... command tool을 위한 거니까.. ^^ */
	if(DI_NANDFLASH_Read(0x00000000 + NAGRA_CSC_LOGICAL_OFFSET, pCscData, ulCscSize) != 0)
	{
		HLIB_CMD_Printf("[%s][%d] Error in DI_NANDFLASH_Read\n", __FUNCTION__, __LINE__);
		HLIB_MEM_Free(pCscData);
		return ERR_FAIL;
	}

	svc_cas_DevPortSysGetChipId(&ulNuid);
	HxSTD_sprintf(aucFileName, "/media/drive1/%s_0x%02X%02X%02X%02X%s", "CSC_Out", ((ulNuid>>24) & 0xFF), ((ulNuid>>16) & 0xFF), ((ulNuid>>8) & 0xFF), (ulNuid & 0xFF), ".bin");
	//HxSTD_sprintf(aucFileName, "/%s_0x%02X%02X%02X%02X%s", "CSC_Out", ((ulNuid>>24) & 0xFF), ((ulNuid>>16) & 0xFF), ((ulNuid>>8) & 0xFF), (ulNuid & 0xFF), ".bin");

	HLIB_CMD_Printf("CSC File Name : [%s]\n", aucFileName);

	HLIB_FILE_Delete(aucFileName);
	if(HLIB_FILE_WriteFile((const HUINT8 *)aucFileName, (void *)pCscData, ulCscSize) != ERR_OK)
	{
		HLIB_CMD_Printf("[%s][%d] Error in HLIB_FILE_ReadFile[%s]\n", __FUNCTION__, __LINE__, aucFileName);
		HLIB_MEM_Free(pCscData);
		return ERR_FAIL;
	}

	HLIB_CMD_Printf("Seccess to extract the CSC : [%s]\n", aucFileName);
	HLIB_MEM_Free(pCscData);
	return ERR_OK;
}

#include <_svc_cas_mgr_nvram.h>
static void local_ca_na_PrintNvram(HINT32 nVal)
{
	HERROR nErr = ERR_OK;
	HUINT8* pOldDataFormat = NULL;
	HUINT8* pNewDataFormat = NULL;	
	HUINT8* pFlashData = NULL;
	
	if(nVal == 0)
	{
		// old version data format.
		pOldDataFormat = (HUINT8*)OxCAS_Malloc(CA_NVRAM_CAS_NA_CSCD_END_OLD);
		if(pOldDataFormat == NULL)
		{
			HxLOG_Error("Can't allocate memory for old data format\n");
			goto FUNCTION_EXIT;
		}
		HxSTD_memset(pOldDataFormat, 0xFF, CA_NVRAM_CAS_NA_CSCD_END_OLD);
		nErr = svc_cas_DevPortNVRAMReadData(eDxCAS_GROUPID_NA, CA_NVRAM_CAS_NA_OLD, CA_NVRAM_CAS_NA_CSCD_END_OLD, pOldDataFormat);
		if (nErr != ERR_OK)
		{
			HxLOG_Error("Can't get old data filed from the NVRAM\n");
			goto FUNCTION_EXIT;
		}

		HxLOG_Print("---- START NVRAM Old data Format -----------------------\n");

		HxLOG_Print("## CAS NA ----\n");
		HLIB_LOG_Dump(pOldDataFormat+CA_NVRAM_CAS_NA_OLD, 16, 0, "NAGRA DATA");
		
		HxLOG_Print("## PIN INFORMATION ----\n");
		HLIB_LOG_Dump(pOldDataFormat+CA_NVRAM_PIN_INFORMATION_OFFSET_OLD, 16, 0, "PIN Information");

		HxLOG_Print("## IRD SEQ NUMBER ----\n");
		HLIB_LOG_Dump(pOldDataFormat+CA_NVRAM_IRDCMD_SEQNUM_OFFSET_OLD, 16, 0, "SEQ Number");
		
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
		HxLOG_Print("## MAIL SEQ NUMBER ----\n");
		HLIB_LOG_Dump(pOldDataFormat+CA_NVRAM_IRDCMD_MAIL_SEQNUM_OFFSET_OLD, 16, 0, "MAIL Seq");

		HxLOG_Print("## MAIL HEADER ----\n");
		HLIB_LOG_Dump(pOldDataFormat+CA_NVRAM_IRDCMD_MAIL_HEADER_OFFSET_OLD, 16, 0, "MAIL Header");
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
		HxLOG_Print("## POPUP SEQ NUMBER ----\n");
		HLIB_LOG_Dump(pOldDataFormat+CA_NVRAM_IRDCMD_POPUP_SEQNUM_OFFSET_OLD, 16, 0, "POPUP Seq");
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
		HxLOG_Print("## EMM WAKEUP ----\n");
		HLIB_LOG_Dump(pOldDataFormat+CA_NVRAM_IRDCMD_EMMWAKEUP_OFFSET_OLD, 16, 0, "EMM Wakeup");
#endif
		HxLOG_Print("## CSC DATA ----\n");
		HLIB_LOG_Dump(pOldDataFormat+CA_NVRAM_CAS_NA_CSCD_OFFSET_OLD, 16, 0, "CSC Data");
		
		HxLOG_Print("---- END NVRAM Old data Format -----------------------\n");
	}
	else
	{
		// current version data format
		pNewDataFormat = (HUINT8*)OxCAS_Malloc(CA_NVRAM_CAS_NA_CSCD_END);
		if(pNewDataFormat == NULL)
		{
			HxLOG_Error("Can't allocate memory for old data format\n");
			goto FUNCTION_EXIT;
		}
		HxSTD_memset(pNewDataFormat, 0xFF, CA_NVRAM_CAS_NA_CSCD_END);
		nErr = svc_cas_DevPortNVRAMReadData(eDxCAS_GROUPID_NA, CA_NVRAM_CAS_NA_VERSION, CA_NVRAM_CAS_NA_CSCD_END, pNewDataFormat);
		if (nErr != ERR_OK)
		{
			HxLOG_Error("Can't get old data filed from the NVRAM\n");
			goto FUNCTION_EXIT;
		}
		HxLOG_Print("---- START NVRAM CURRENT Version data Format -----------------------\n");

		HxLOG_Print("## NVRAM VERSION INFO ----\n");
		HLIB_LOG_Dump(pNewDataFormat+CA_NVRAM_CAS_NA_VERSION, 8, 0, "NVRAM VERSION");

		HxLOG_Print("## CAS NA ----\n");
		HLIB_LOG_Dump(pNewDataFormat+CA_NVRAM_CAS_NA, 16, 0, "NAGRA DATA");

		HxLOG_Print("## PIN INFORMATION ----\n");
		HLIB_LOG_Dump(pNewDataFormat+CA_NVRAM_PIN_INFORMATION_OFFSET, 16, 0, "PIN Information");

		HxLOG_Print("## IRD SEQ NUMBER ----\n");
		HLIB_LOG_Dump(pNewDataFormat+CA_NVRAM_IRDCMD_SEQNUM_OFFSET, 16, 0, "SEQ Number");
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
		HxLOG_Print("## MAIL SEQ NUMBER ----\n");
		HLIB_LOG_Dump(pNewDataFormat+CA_NVRAM_IRDCMD_MAIL_SEQNUM_OFFSET, 16, 0, "MAIL Seq");

		HxLOG_Print("## MAIL HEADER ----\n");
		HLIB_LOG_Dump(pNewDataFormat+CA_NVRAM_IRDCMD_MAIL_HEADER_OFFSET, 16, 0, "MAIL Header");
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
		HxLOG_Print("## POPUP SEQ NUMBER ----\n");
		HLIB_LOG_Dump(pNewDataFormat+CA_NVRAM_IRDCMD_POPUP_SEQNUM_OFFSET, 16, 0, "POPUP Seq");
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
		HxLOG_Print("## EMM WAKEUP ----\n");
		HLIB_LOG_Dump(pNewDataFormat+CA_NVRAM_IRDCMD_EMMWAKEUP_OFFSET, 16, 0, "EMM Wakeup");
#endif
#if defined (CONFIG_MW_CAS_NAGRA_NOCS_3_X)
		pFlashData = (HUINT8*)OxCAS_Malloc(3000);
		if(pFlashData == NULL)
		{
			HxLOG_Error("Can't allocate memory for old data format\n");
			goto FUNCTION_EXIT;
		}
		nErr = CA_NA_OsFlashRead((TSignedInt32)STORAGE_ID_4, 0, 3000, pFlashData);
		if(nErr != ERR_OK)
		{
			goto FUNCTION_EXIT;
		}
		HxLOG_Print("## CSC DATA on the flash ----\n");
		HLIB_LOG_Dump(pFlashData, 16, 0, "CSC Data from flash");
#else
		HLIB_LOG_Dump(pNewDataFormat+CA_NVRAM_CAS_NA_CSCD_OFFSET_OLD, 16, 0, "CSC Data");
#endif
		
		
		HxLOG_Print("---- END NVRAM Old data Format -----------------------\n");
	}

FUNCTION_EXIT:
	if(pOldDataFormat != NULL)
	{
		OxCAS_Free(pOldDataFormat);
		pOldDataFormat = NULL;
	}

	if(pNewDataFormat != NULL)
	{
		OxCAS_Free(pNewDataFormat);
		pNewDataFormat = NULL;
	}

	if(pFlashData != NULL)
	{
		OxCAS_Free(pFlashData);
		pFlashData = NULL;
	}
	return;
}

static void local_ca_na_MakeOldDataFormat(HUINT8* pBuffer)
{
	HUINT8 auc16ByteData[16] = {0x00,0x01,0x02,0x03, 0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};
	HUINT8 auc10ByteData[10] = {0x00,0x11,0x22,0x33, 0x44,0x55,0x66,0x77,0x88,0x99};
	HUINT8 auc8ByteData[8] = {0x01,0x23,0x45,0x67, 0x89,0x0A,0xBC,0xDE};
	HUINT8 auc4ByteData[4] = {0xAA, 0xBB, 0xCC, 0xDD};

	HxSTD_memcpy(pBuffer+CA_NVRAM_CAS_NA_OLD, auc16ByteData, sizeof(auc16ByteData));
	
	HxSTD_memcpy(pBuffer+CA_NVRAM_PIN_INFORMATION_OFFSET_OLD, auc10ByteData, sizeof(auc10ByteData));
	
	HxSTD_memcpy(pBuffer+CA_NVRAM_IRDCMD_SEQNUM_OFFSET_OLD, auc16ByteData, sizeof(auc16ByteData));
	
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
	HxSTD_memcpy(pBuffer+CA_NVRAM_IRDCMD_MAIL_SEQNUM_OFFSET_OLD, auc8ByteData, sizeof(auc8ByteData));
	HxSTD_memcpy(pBuffer+CA_NVRAM_IRDCMD_MAIL_HEADER_OFFSET_OLD, auc8ByteData, sizeof(auc8ByteData));
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
	HxSTD_memcpy(pBuffer+CA_NVRAM_IRDCMD_POPUP_SEQNUM_OFFSET_OLD, auc10ByteData, sizeof(auc10ByteData));
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
	HxSTD_memcpy(pBuffer+CA_NVRAM_IRDCMD_EMMWAKEUP_OFFSET_OLD, auc16ByteData, sizeof(auc16ByteData));
#endif

	HxSTD_memcpy(pBuffer+CA_NVRAM_CAS_NA_CSCD_OFFSET_OLD, auc4ByteData, sizeof(auc4ByteData));

	return;
}

static void local_ca_na_MakeNewDataFormat(HUINT8* pBuffer)
{
	HUINT8 auc16ByteData[16] = {0x00,0x01,0x02,0x03, 0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};
	HUINT8 auc10ByteData[10] = {0x00,0x11,0x22,0x33, 0x44,0x55,0x66,0x77,0x88,0x99};
	HUINT8 auc8ByteData[8] = {0x01,0x23,0x45,0x67, 0x89,0x0A,0xBC,0xDE};
	HUINT8 auc4ByteData[4] = {0xAA, 0xBB, 0xCC, 0xDD};
	HUINT8 aucVersion[8] = {0, };

	aucVersion[0] = (CA_NVRAM_CAS_NA_VERSION_HEADER >> 24) & 0xFF;
	aucVersion[1] = (CA_NVRAM_CAS_NA_VERSION_HEADER >> 16) & 0xFF;
	aucVersion[2] = (CA_NVRAM_CAS_NA_VERSION_HEADER >> 8) & 0xFF;
	aucVersion[3] = CA_NVRAM_CAS_NA_VERSION_HEADER & 0xFF;

	aucVersion[4] = (CA_NVRAM_CAS_NA_VERSION_VALUE >> 24) & 0xFF;
	aucVersion[5] = (CA_NVRAM_CAS_NA_VERSION_VALUE >> 16) & 0xFF;
	aucVersion[6] = (CA_NVRAM_CAS_NA_VERSION_VALUE >> 8) & 0xFF;
	aucVersion[7] = CA_NVRAM_CAS_NA_VERSION_VALUE & 0xFF;
	HxSTD_memcpy(pBuffer+CA_NVRAM_CAS_NA_VERSION, aucVersion, sizeof(aucVersion));
	
	HxSTD_memcpy(pBuffer+CA_NVRAM_CAS_NA, auc8ByteData, sizeof(auc8ByteData));
	
	HxSTD_memcpy(pBuffer+CA_NVRAM_PIN_INFORMATION_OFFSET, auc16ByteData, sizeof(auc16ByteData));
	
	HxSTD_memcpy(pBuffer+CA_NVRAM_IRDCMD_SEQNUM_OFFSET, auc8ByteData, sizeof(auc8ByteData));
	
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
	HxSTD_memcpy(pBuffer+CA_NVRAM_IRDCMD_MAIL_SEQNUM_OFFSET, auc16ByteData, sizeof(auc16ByteData));
	HxSTD_memcpy(pBuffer+CA_NVRAM_IRDCMD_MAIL_HEADER_OFFSET, auc16ByteData, sizeof(auc16ByteData));
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
	HxSTD_memcpy(pBuffer+CA_NVRAM_IRDCMD_POPUP_SEQNUM_OFFSET, auc8ByteData, sizeof(auc8ByteData));
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
	HxSTD_memcpy(pBuffer+CA_NVRAM_IRDCMD_EMMWAKEUP_OFFSET, auc8ByteData, sizeof(auc8ByteData));
#endif

#if !defined (CONFIG_MW_CAS_NAGRA_NOCS_3_X)
	HxSTD_memcpy(pBuffer+CA_NVRAM_CAS_NA_CSCD_OFFSET, auc16ByteData, sizeof(auc16ByteData));
#endif

	return;
}

static void local_ca_na_CmdConvertNvram(HINT32 nVal)
{
	HERROR nErr = ERR_OK;
	HUINT8* pOldDataFormat = NULL;
	HUINT8* pNewDataFormat = NULL;
	
	switch(nVal)
	{
		case 0:
			// delete all nvram data
			pNewDataFormat = (HUINT8*)OxCAS_Malloc(CA_NVRAM_CAS_NA_CSCD_END);
			if(pNewDataFormat == NULL)
			{
				HxLOG_Error("Can't allocate memory for old data format\n");
				goto FUNCTION_EXIT;
			}
			HxSTD_memset(pNewDataFormat, 0xFF, CA_NVRAM_CAS_NA_CSCD_END);

			nErr = svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, CA_NVRAM_CAS_NA_VERSION, CA_NVRAM_CAS_NA_CSCD_END, pNewDataFormat);
			if (nErr != ERR_OK)
			{
				HxLOG_Error("Can't write current version data format to the NVRAM\n");
				goto FUNCTION_EXIT;
			}
			HxLOG_Print("Deleted all nvram data");
			break;
			
		case 1:
			// write old data format to nvram
			pOldDataFormat = (HUINT8*)OxCAS_Malloc(CA_NVRAM_CAS_NA_CSCD_END_OLD);
			if(pOldDataFormat == NULL)
			{
				HxLOG_Error("Can't allocate memory for old data format\n");
				goto FUNCTION_EXIT;
			}
			HxSTD_memset(pOldDataFormat, 0xFF, CA_NVRAM_CAS_NA_CSCD_END_OLD);
			HxLOG_Error("Make data format\n");
			local_ca_na_MakeOldDataFormat(pOldDataFormat);

			HxLOG_Error("Write to file\n");
			nErr = svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, CA_NVRAM_CAS_NA_OLD, CA_NVRAM_CAS_NA_CSCD_END_OLD, pOldDataFormat);
			if (nErr != ERR_OK)
			{
				HxLOG_Error("Can't write old version data format to the NVRAM\n");
				goto FUNCTION_EXIT;
			}
			local_ca_na_PrintNvram(0);
			break;
			
		case 2:
			// write new data format to nvram
			pNewDataFormat = (HUINT8*)OxCAS_Malloc(CA_NVRAM_CAS_NA_CSCD_END);
			if(pNewDataFormat == NULL)
			{
				HxLOG_Error("Can't allocate memory for old data format\n");
				goto FUNCTION_EXIT;
			}
			HxSTD_memset(pNewDataFormat, 0xFF, CA_NVRAM_CAS_NA_CSCD_END);

			HxLOG_Error("Make NEW data format\n");
			local_ca_na_MakeNewDataFormat(pNewDataFormat);

			HxLOG_Error("Write NEW Data to file\n");
			nErr = svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, CA_NVRAM_CAS_NA_VERSION, CA_NVRAM_CAS_NA_CSCD_END, pNewDataFormat);
			if (nErr != ERR_OK)
			{
				HxLOG_Error("Can't write current version data format to the NVRAM\n");
				goto FUNCTION_EXIT;
			}
			local_ca_na_PrintNvram(1);
			break;
			
		case 3:
			// convert nvram data format
			local_ca_na_PrintNvram(0);
			nErr = CA_NA_OsCheckEepromVersion();
			if (nErr != ERR_OK)
			{
				HxLOG_Error("Can't convert the NVRAM\n");
				goto FUNCTION_EXIT;
			}
			local_ca_na_PrintNvram(1);
			break;
			
		case 4:
			local_ca_na_PrintNvram(0);
			break;
			
		case 5:
			local_ca_na_PrintNvram(1);
			break;
			
		default:
			HxLOG_Error("Unknown command number(%d)", nVal);
			break;
	}

FUNCTION_EXIT:

	if(pOldDataFormat != NULL)
	{
		OxCAS_Free(pOldDataFormat);
		pOldDataFormat = NULL;
	}

	if(pNewDataFormat != NULL)
	{
		OxCAS_Free(pNewDataFormat);
		pNewDataFormat = NULL;
	}
	
	return;
}

static int local_ca_na_NagraCommand(void *szArgList)
{
	char *aucArg=NULL, *aucArg2=NULL;
	HUINT8 aucStr[20+1]={0,};
	HUINT8 aucStr2[20+1]={0,};
	UNIXTIME ulTime = 0;
    	unsigned long   arg2 = 0, arg3 = 0;

	/* 1st argument */
	aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
		return HxCMD_ERR;
	if( snprintf(aucStr, 20, "%s", aucArg) == 0 )
		return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

	if(strcmp(aucStr, "PK") == 0 )
	{
		aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if(aucArg == NULL)
		{
			return HxCMD_ERR;
		}
		if( snprintf(aucStr, 20, "%s", aucArg) == 0  )
		{
			return HxCMD_ERR;
		}
		HLIB_STD_StrUpper(aucStr);

		if(strcmp(aucStr, "WRITE") == 0)
		{
			if(local_ca_na_CmdWritePkFromUsb() == ERR_OK)
				return HxCMD_OK;
		}
		else if(strcmp(aucStr, "READ") == 0)
		{
			if(local_ca_na_CmdReadPkToUsb() == ERR_OK)
				return HxCMD_OK;
		}
		else if(strcmp(aucStr, "ENCRYPT") == 0)
		{
			if(local_ca_na_CmdEncryptPkFromUsb() == ERR_OK)
				return HxCMD_OK;
		}
		else
		{
			HLIB_CMD_Printf("choose WRITE or READ\n");
		}
	}
	else if(strcmp(aucStr, "CSC") == 0 )
	{
		aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if(aucArg == NULL)
		{
			HLIB_CMD_Printf("choose [ca] or [ir] or [loader]\n");
			return HxCMD_ERR;
		}
		if( snprintf(aucStr, 20, "%s", aucArg) == 0  )
		{
			HLIB_CMD_Printf("aucArg error \n");
			return HxCMD_ERR;
		}
		HLIB_STD_StrUpper(aucStr);

		if(strcmp(aucStr, "WRITE") == 0)
		{
			if(local_ca_na_CmdWriteCSCFromUsb(FALSE) == ERR_OK)
				return HxCMD_OK;
		}
		else if(strcmp(aucStr, "READ") == 0)
		{
			if(local_ca_na_CmdReadCSCToUsb() == ERR_OK)
				return HxCMD_OK;
		}
		else if(strcmp(aucStr, "WRITETONVRAM") == 0)
		{
			if(local_ca_na_CmdWriteCSCFromUsb(TRUE) == ERR_OK)
				return HxCMD_OK;
		}
		else if( strcmp(aucStr, "PRINT") == 0 )
		{
			HUINT16 i;
			HUINT8 *pucCsc = NULL;
			HERROR nErrCode = ERR_OK;

			pucCsc = VK_MEM_Alloc(256);
			if(pucCsc != NULL)
			{
				VK_memset(pucCsc, 0x0, 256);

				HxLOG_Error("CSC Data Print : \n");

				nErrCode = svc_cas_DevSysGetField(eCAS_DEV_FLASH_FIELD_CSC_KEY, 0, (void*)pucCsc, 256);
				if (nErrCode != ERR_OK)
				{
					HxLOG_Error("error svc_cas_DevSysGetField, nErrCode:0x%X\n", nErrCode);
					return SEC_ERROR;
				}

				for( i=0; i<256; i++ )
				{
					if ( i%16 == 0 ) HxLOG_Print("\r\n");
					HxLOG_Print(" %02X", pucCsc[i]);
				}
				HxLOG_Print("\r\n");
				if(pucCsc != NULL)
					VK_MEM_Free(pucCsc);

				HxLOG_Error("-------------------------------------------------\n");

			}
		}
	}
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD) && defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
	else if( strcmp(aucStr, "WAKEUP") == 0 )
	{
		HxLOG_Error("IrdCmd : WAKEUP >> \n");
		xmgr_cas_NaResetIRDCmdData();
		CA_NA_CakTestIrdCmd(10);
	}
	else if( strcmp(aucStr, "WAKEUPGET") == 0 )
	{
		HxLOG_Error("IrdCmd : WAKEUPGET >> \n");
		xmgr_cas_NaIrdCmdEmmGetWakeupTime(&ulTime);
		HxLOG_Error("ulTime = %d \n", ulTime);
	}
	else if( strcmp(aucStr, "WAKEUPRESET") == 0 )
	{
		HxLOG_Error("IrdCmd : xmgr_cas_NaResetIRDCmdData >> \n");
		xmgr_cas_NaResetIRDCmdData();
	}
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	else if( strcmp(aucStr, "PROFILE") == 0 )
	{
		HERROR					nErrorReturn;
		HUINT8					ucProfieType = 0;
		NA_IRDCMD_IDX_PROFILE_t 	stCurrentProfileSet;

		HxLOG_Error("IrdCmd : PROFILE Index - eProfileSmartcard\n");
		ucProfieType = eProfileSmartcard;
		nErrorReturn = CASMGR_APS_NaGetIndexDisplayProfile(&stCurrentProfileSet, ucProfieType);
		if (nErrorReturn == ERR_OK)
		{
			HxLOG_Error("[MJAHN] pstCurrentProfileSet->nCurrentIdx = %d \n", stCurrentProfileSet.nCurrentIdx);

		}

		HxLOG_Error("IrdCmd : PROFILE Index - eProfileProducts\n");
		ucProfieType = eProfileProducts;
		nErrorReturn = CASMGR_APS_NaGetIndexDisplayProfile(&stCurrentProfileSet, ucProfieType);
		if (nErrorReturn == ERR_OK)
		{
			HxLOG_Error("[MJAHN] pstCurrentProfileSet->nCurrentIdx = %d \n", stCurrentProfileSet.nCurrentIdx);
		}

		HxLOG_Error("IrdCmd : PROFILE - eProfileSmartcard\n");
		nErrorReturn = CASMGR_APS_NaPrintDisplayProfile(eProfileSmartcard);
		if (nErrorReturn != ERR_OK)
		{
			HxLOG_Error(" CASMGR_APS_NaPrintDisplayProfile error [%x]\n", nErrorReturn);
		}

		HxLOG_Error("IrdCmd : PROFILE - eProfileProducts\n");
		nErrorReturn = CASMGR_APS_NaPrintDisplayProfile(eProfileProducts);
		if (nErrorReturn != ERR_OK)
		{
			HxLOG_Error(" CASMGR_APS_NaPrintDisplayProfile error [%x]\n", nErrorReturn);
		}

		HxLOG_Error("IrdCmd : PROFILE ---\n");

	}
#endif
#endif
	else if( strcmp(aucStr, "SES") == 0 )
	{
		char *aucArg_4="bcd test msg";

		/* 2nd argument */
		aucArg2 = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if( aucArg2 == NULL )
			return HxCMD_ERR;
		if( snprintf(aucArg2, 20, "%s", aucArg) == 0 )
			return HxCMD_ERR;

		arg2 = atoi(aucStr2);

		HxLOG_Error("IrdCmd : reset all ird command  >> arg2=%d\n", arg2);
		xmgr_cas_NaCMD(aucArg, arg2, arg3, aucArg_4);
	}
	else if(strcmp(aucStr, "NVRAM") == 0)
	{
		aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if(aucArg == NULL)
		{
			return HxCMD_ERR;
		}
		if(sscanf(aucArg, "%d", &arg2) == 0)
			return HxCMD_ERR;

		HxLOG_Print("nvram command (%d)\n", arg2);
		local_ca_na_CmdConvertNvram(arg2);
	}
	else if(strcmp(aucStr, "IRD") == 0)
	{
		aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if(aucArg == NULL)
		{
			return HxCMD_ERR;
		}
		if(sscanf(aucArg, "%d", &arg2) == 0)
			return HxCMD_ERR;

		HxLOG_Print("IRD command (%d)\n", arg2);
		CA_NA_CakTestIrdCmd(arg2);
	}
	else
	{
		HxLOG_Error("Unknow command[%s]\n", aucStr);
	}

	return HxCMD_ERR;
}

static int local_ca_na_NagraUICommand(void *szArgList)
{
	char *aucArg=NULL;
	HUINT8 aucStr[20+1]={0,};

	/* 1st argument */
	aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
		return HxCMD_ERR;
	if( snprintf(aucStr, 20, "%s", aucArg) == 0 )
		return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

	if(strcmp(aucStr, "POPUP") == 0 )
	{
		NaPopupUI_t *pstPopupUi;

		aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if(aucArg == NULL)
			return HxCMD_ERR;

		if( snprintf(aucStr, 20, "%s", aucArg) == 0 )
			return HxCMD_ERR;

		pstPopupUi = OxCAS_Calloc(sizeof(NaPopupUI_t));
		if(pstPopupUi == NULL)
			return HxCMD_ERR;

		HLIB_STD_StrUpper(aucStr);

		if(strcmp(aucStr, "CATASTROPHIC") == 0 )
		{
			pstPopupUi->eType = ePopup_Catastrophic;
		}
		else if(strcmp(aucStr, "SWUPGRADE") == 0 )
		{
			pstPopupUi->eType = ePopup_SWupgrade;
		}
		else
		{
			HUINT32 ulStrLen = HxSTD_StrLen(aucArg);

			pstPopupUi->eType = ePopup_Message;
			pstPopupUi->pucMsg = (HUINT8 *)OxCAS_Calloc(ulStrLen + 1);
			if(pstPopupUi->pucMsg != NULL)
				HxSTD_memcpy(pstPopupUi->pucMsg, aucArg, ulStrLen);
		}

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
		_naMgr_procPopupCallback(pstPopupUi);
#endif

		return HxCMD_OK;
	}
	else if(strcmp(aucStr, "PINCODE") == 0 )
	{
#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
		HUINT8 ucMsgSlot;
		NaAlarmData_t stNaAlarmData;

		s_stAccessInfoList[0].ucNumOfStreams = 0;
		HxSTD_memset(&s_stAccessInfoList[0].stAccessAlarmItems, 0x00, (sizeof(NaAccessAlarmItem_t) * MAX_TOTAL_NO_STREAM));

		s_stAccessInfoList[0].ucNumOfStreams = 1;
		s_stAccessInfoList[0].stAccessAlarmItems[0].bChanged = 1;
		s_stAccessInfoList[0].stAccessAlarmItems[0].eAlarm = CA_ACCESS_DENIED_PARENTAL_CONTROL;
		s_stAccessInfoList[0].eAlarmType = eAlarm_Access;

		stNaAlarmData.hAction = s_stAccessInfoList[0].hAction;
		stNaAlarmData.ucNumOfAccessAlarms = 1;
		stNaAlarmData.eAlarmType = eAlarm_Access;
		stNaAlarmData.pstAccessAlarmItems = s_stAccessInfoList[0].stAccessAlarmItems;

		_naMgr_GetSlotMsgBuffer(eAlarmBufferIdx, &ucMsgSlot);

		HxSTD_memcpy(&s_stNaAlarmData[ucMsgSlot], &stNaAlarmData, sizeof(NaAlarmData_t));

		svc_cas_MgrSubUpdateCasMessage(s_stAccessInfoList[0].hAction, eCAS_MSG_TYPE_Fail, eDxCAS_GROUPID_NA, (void*)&s_stNaAlarmData[ucMsgSlot], eCasShowType_None);
#else
		HxLOG_Error("#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)!!!\n");
#endif
		return HxCMD_OK;
	}
	else if(strcmp(aucStr, "IRDCMD") == 0 )
	{
		HUINT8 ucIrdCmd;

		aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if(aucArg == NULL)
			return HxCMD_ERR;

		if( snprintf(aucStr, 20, "%s", aucArg) == 0 )
			return HxCMD_ERR;

		HLIB_STD_StrUpper(aucStr);

		if(strcmp(aucStr, "MAIL") == 0 )
			ucIrdCmd = 0x64;
		else
			return HxCMD_ERR;

		NAMGR_CAKMGR_SendIrdCommand(ucIrdCmd);

		return HxCMD_OK;
	}

	return HxCMD_ERR;
}

static int local_ca_na_NagraDbgCommand(void *szArgList)
{
	char *aucArg=NULL;
	HUINT8 aucStr[20+1]={0,};

	/* 1st argument */
	aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
		return HxCMD_ERR;

	if( HxSTD_snprintf(aucStr, 20, "%s", aucArg) == 0 )
		return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

	if(HxSTD_StrCmp(aucStr, "DMX") == 0 )
	{
		aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if(aucArg == NULL)
			return HxCMD_ERR;

		if(HxSTD_snprintf(aucStr, 20, "%s", aucArg) == 0 )
			return HxCMD_ERR;

		HLIB_STD_StrUpper(aucStr);

		if(HxSTD_StrCmp(aucStr, "INFO") == 0 )
			CA_NA_DmxPrintInfo();
		else if(HxSTD_StrCmp(aucStr, "ECM") == 0 )
			CA_NA_DmxEcmSectionTrace();
		else if(HxSTD_StrCmp(aucStr, "EMM") == 0 )
			CA_NA_DmxEmmSectionTrace();
		else
			return HxCMD_ERR;

		return HxCMD_OK;
	}
	else if(HxSTD_StrCmp(aucStr, "OS") == 0 )
	{
		aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if(aucArg == NULL)
			return HxCMD_ERR;

		if(HxSTD_snprintf(aucStr, 20, "%s", aucArg) == 0 )
			return HxCMD_ERR;

		HLIB_STD_StrUpper(aucStr);

		if(HxSTD_StrCmp(aucStr, "CAK") == 0 )
			CA_NA_OsCakMessage();
		else
			return HxCMD_ERR;

		return HxCMD_OK;
	}
	else if(HxSTD_StrCmp(aucStr, "SVC") == 0 )
	{
		aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if(aucArg == NULL)
			return HxCMD_ERR;

		if(HxSTD_snprintf(aucStr, 20, "%s", aucArg) == 0 )
			return HxCMD_ERR;

		HLIB_STD_StrUpper(aucStr);

		if(HxSTD_StrCmp(aucStr, "LIST") == 0 )
			NAMGR_CAKSVC_PrintNagraInstance();
		else
			return HxCMD_ERR;

		return HxCMD_OK;
	}

	return HxCMD_ERR;
}

void CMD_Register_SVC_CAS_NagraCmd(HxCMD_Handle_t hCasNaCmdHandle)
{
	HLIB_CMD_RegisterWord(hCasNaCmdHandle,
						local_ca_na_NagraCommand,
						"nacmd",						/* keyword */
						"nagra command : nacmd [PK|CSC] [WRITE|READ|ENCRYPT(PK only)] [WAKEUP|WAKEUPGET|WAKEUPRESET] [SES 0]",	/* help   */
						"nacmd [PK|CSC] [WRITE|READ|ENCRYPT(PK only)] [WAKEUP|WAKEUPGET|WAKEUPRESET] [SES  0]");	/* usage  */

	HLIB_CMD_RegisterWord(hCasNaCmdHandle,
						local_ca_na_NagraUICommand,
						"nauicmd",						/* keyword */
						"nagra command : nauicmd [POPUP | PINCODE | IRDCMD | ] [POPUP {CATASTROPHIC | SWUPGRADE | H/E MESSAGE | }, IRDCMD {MAIL | }, ]",	/* help   */
						"nauicmd [POPUP | PINCODE | IRDCMD| ] [POPUP {CATASTROPHIC | SWUPGRADE | H/E MESSAGE | }, IRDCMD {MAIL | }, ]");	/* usage  */

	HLIB_CMD_RegisterWord(hCasNaCmdHandle,
						local_ca_na_NagraDbgCommand,
						"nadbg",						/* keyword */
						"nagra command : nadbg [DMX, ...] [DMX {INFO, ECM, EMM, ...}, ...]",	/* help   */
						"nadbg [DMX, ...] [DMX {INFO, ECM, EMM, ...}, ...]");	/* usage  */

	return;
}


#endif

/*** End of File ***/


