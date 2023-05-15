/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  	pal_sc.c
	@brief	  smartcard.

	Description: 	\n
	Module: PAL/SC			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#include <octo_common.h>

#include <di_smartcard.h>

#include <pal_sc.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



#ifdef CONFIG_MW_CAS_NUM_EMBEDDED
#define palSc_SCI_NUM_CAS				CONFIG_MW_CAS_NUM_EMBEDDED					// Porting 되는 EMBEDDED CAS의 개수
#else
#define palSc_SCI_NUM_CAS				1
#endif

#ifdef CONFIG_MW_CAS_NUM_SC_SLOT										// Smart card slot 개수, Kconfig에서 설정 됨.
#define palSc_SCI_NUM_SLOT				CONFIG_MW_CAS_NUM_SC_SLOT
#else
#define palSc_SCI_NUM_SLOT				1
#endif

#define palSc_SCI_NUM_HANDLE			palSc_SCI_NUM_SLOT * palSc_SCI_NUM_CAS

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HUINT32	ulProtocol;
	HUINT32 ulMinClock;
	HUINT32	ulBaudrate;
	HUINT8	ucK;
	HUINT8	ucFI;
	HUINT8	ucDI;
	HUINT8	ucN;
	HUINT8	ucWI;
	HUINT8	ucTFirst;
	HUINT8	ucT;
	HUINT8	ucIFSC;
	HUINT8	ucCWI;
	HUINT8	ucBWI;
	HUINT8	ucII;
	HUINT8	ucPI1;
	HUINT8	ucPI2;
	HBOOL	bSpecificMode;
	HBOOL	bImplicitlyDefined;
	HBOOL	bPpsUnable;
} palSc_SciAtrInfo_t;

typedef struct
{
	HUINT8	ucLen;
	HUINT8	aucAtr[34];
} palSc_RawAtr_t;

typedef struct
{
	HUINT32			ulSlotId;
	DxCAS_GroupId_e	eCasId;
	PalSc_Callback_t 	pfnCbSci;

/*
	instance id에 해당하는 cas가 card의 소유자임을 나타냄.
	card가 제가되면 반드시 초기화 되어야 함.
*/
	PalSc_Flag_e		eMatched;

} palSc_SciInst_t;


typedef enum
{
	eSciCmd_None,
	eSciCmd_CardState,
	eSciCmd_CardSelect,
	eSciCmd_Max
} palSc_SciTaskCmd_e;

typedef struct
{
	palSc_SciTaskCmd_e		eCmd;
	HUINT32					p1;			// 0: removed, 1 : inserted
	HUINT32					p2;
} palSc_SciMsg_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HUINT32 				s_ulMaxNumOfSlot;
STATIC palSc_SciInst_t		s_stScInst[palSc_SCI_NUM_HANDLE];
STATIC HBOOL				s_bScInit;

STATIC HULONG				s_ulPalScCmdMsgQId;
STATIC HULONG				s_ulPalScTaskId;

/********************************************************************/
/********************      proto-type functions  ********************/
/********************************************************************/



/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
STATIC palSc_SciInst_t *pal_sc_GetInstFromHandle(PalSc_Handle_t hSci)
{
	HUINT32 ulInstId = 0;

	if(hSci >= palSc_SCI_NUM_HANDLE)
	{
		return (palSc_SciInst_t *)NULL;
	}

	/*
		handle의 개념을 위해 아래와 같이 명시적으로 convert 함.
	*/
	ulInstId = (HUINT32)hSci;

	return &s_stScInst[ulInstId];
}

STATIC DI_SC_PROTOCOL pal_sc_ConvertProtocolPal2Di(PalSc_Protocol_e eProtocol)
{
	DI_SC_PROTOCOL di_protocol;

	switch(eProtocol)
	{
		case ePAL_SC_PROTOCOL_T0:
#if defined(CONFIG_MW_CAS_CONAX)
			di_protocol = DI_T_0_CX;
#else
			di_protocol = DI_T_0;
#endif
			break;
		case ePAL_SC_PROTOCOL_T1:
			di_protocol = DI_T_1;
			break;
		case ePAL_SC_PROTOCOL_T14:
			di_protocol = DI_T_14;
			break;
		case ePAL_SC_PROTOCOL_T0_IR:
			di_protocol = DI_T_0_IR;
			break;
		case ePAL_SC_PROTOCOL_T0_CX:
			di_protocol = DI_T_0_CX;
			break;
		case ePAL_SC_PROTOCOL_T1_NA:
			di_protocol = DI_T_1_NA;
			break;
		case ePAL_SC_PROTOCOL_T1_ARIB:
			di_protocol = DI_T_1_ARIB;
			break;
		default:
			HxLOG_Critical("\n\n");
			di_protocol = DI_T_0;
			break;
	}

	HxLOG_Print("pal (%d), di (%d) \n", eProtocol, di_protocol);

	return di_protocol;
}

STATIC PalSc_Protocol_e pal_sc_ConvertProtocolDi2Pal(DI_SC_PROTOCOL eProtocol)
{
	PalSc_Protocol_e pal_protocol;

	switch(eProtocol)
	{
		case DI_T_0:
			pal_protocol = ePAL_SC_PROTOCOL_T0;
			break;
		case DI_T_1:
			pal_protocol = ePAL_SC_PROTOCOL_T1;
			break;
		case DI_T_14:
			pal_protocol = ePAL_SC_PROTOCOL_T14;
			break;
		case DI_T_0_IR:
			pal_protocol = ePAL_SC_PROTOCOL_T0_IR;
			break;
		case DI_T_0_CX:
			pal_protocol = ePAL_SC_PROTOCOL_T0_CX;
			break;
		case DI_T_1_NA:
			pal_protocol = ePAL_SC_PROTOCOL_T1_NA;
			break;
		case DI_T_1_ARIB:
			pal_protocol = ePAL_SC_PROTOCOL_T1_ARIB;
			break;
		default:
			HxLOG_Critical("\n\n");
			pal_protocol = ePAL_SC_PROTOCOL_END;
			break;
	}

	HxLOG_Print("pal (%d), di (%d) \n", pal_protocol, eProtocol);

	return pal_protocol;
}

STATIC void pal_sc_DeviceCallback(HUINT32 ulSlotId, DI_SC_STATUS eDiStatus)
{
	PalSc_CardStatus_e ePalStatus;
	palSc_SciInst_t *pSci = (palSc_SciInst_t *)NULL;
	HUINT32 i = 0;

	if(ulSlotId >= s_ulMaxNumOfSlot)
	{
		HxLOG_Critical("\n\n");
		return;
	}

	if(eDiStatus == DI_SC_NOT_PRESENT)
	{
		ePalStatus = ePAL_SC_CARDSTATUS_NOT_PRESENT;
	}
	else
	{
		ePalStatus = ePAL_SC_CARDSTATUS_PRESENT;
	}

/*
	embedded cas 가 1개 이상인 경우.
	가장 보수적인 방법으로 mw에 구현한다.
	smart card의 historical byte 등 platform 및 시장 상황이 받침이 되면 개선은 가능하나 일반적인 방법은 될 수 없으므로, 보수적인 방법 선택 함.
*/
if (palSc_SCI_NUM_CAS > 1)
{
	palSc_SciMsg_t		msg;
	HUINT32 			nResult = ERR_OK;

	/* Message Create */
	HxSTD_memset(&msg, 0x00, sizeof(palSc_SciMsg_t));
	msg.eCmd = eSciCmd_CardState;
	msg.p1 = ePalStatus;
	msg.p2 = ulSlotId;

	HxLOG_Print("eSciCmd_CardState, PalState (%d), Slot Id (%d) \n",
		ePalStatus, ulSlotId);

	nResult = VK_MSG_Send(s_ulPalScCmdMsgQId, &msg, sizeof(palSc_SciMsg_t));
	if(nResult != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return;
	}

}
else
{
	if(palSc_SCI_NUM_SLOT > 1)	/* Case of 1 CAS 2 SLOT */
	{
		pSci = pal_sc_GetInstFromHandle(ulSlotId);
	}
	else
	{
		pSci = pal_sc_GetInstFromHandle(0);
	}

	if(pSci == NULL)
	{
		HxLOG_Critical("\n\n");
		return;
	}

	if((pSci->eCasId) != 0 && (pSci->ulSlotId == ulSlotId))
	{
		if(pSci->pfnCbSci != NULL)
		{
			HxLOG_Print("handle (%d) cas id (%x), slot id (%d), status (%d)\n",
				0, pSci->eCasId, pSci->ulSlotId, ePalStatus);
			pSci->pfnCbSci((PalSc_Handle_t)i, ulSlotId, ePalStatus);
		}
	}
}

	return;
}

STATIC PalSc_Handle_t pal_sc_GetNextSCIHandle(PalSc_Handle_t hSci)
{
	palSc_SciInst_t *pSci = NULL, *pNextSci = NULL;
	HUINT32	ulHandleCounter = 0;

	pSci = pal_sc_GetInstFromHandle(hSci);
	if(pSci == NULL)
	{
		HxLOG_Critical("\n\n");
		return HANDLE_NULL;
	}

	for(ulHandleCounter = (HUINT32)(hSci + 1); ulHandleCounter < palSc_SCI_NUM_HANDLE; ulHandleCounter++)
	{
		pNextSci = pal_sc_GetInstFromHandle(ulHandleCounter);
		if(pNextSci == NULL)
		{
			continue;
		}

		if(pNextSci->eCasId == 0)
		{
			continue;
		}

		if(pSci->ulSlotId == pNextSci->ulSlotId)
			return ulHandleCounter;
	}

	return HANDLE_NULL;
}

STATIC void pal_sc_HandleMsg(palSc_SciMsg_t *pMsg)
{
	HUINT32	ulHandleCounter = 0, ulEvtSlotId = 0;
	palSc_SciInst_t *pSci = (palSc_SciInst_t *)NULL;
	PalSc_Handle_t hSci = HANDLE_NULL, hNextSci = HANDLE_NULL;
	PalSc_Flag_e eMatched;
	PalSc_CardStatus_e ePalStatus, eCurSlotStatus;
	HERROR	hError = ERR_FAIL;

	switch (pMsg->eCmd)
	{
		case eSciCmd_CardState:
			// Card In/Out에 대한 처리 담당
			ePalStatus = pMsg->p1;
			ulEvtSlotId = pMsg->p2;

			hError = PAL_SC_GetCardStatusBySlotID(ulEvtSlotId, &eCurSlotStatus);
			if(ERR_FAIL == hError)
			{
				HxLOG_Print("Current Card Slot(%d) Status cannot be retreived !!! ", ulEvtSlotId);
				break;
			}

/*
			if (eCurSlotStatus == ePalStatus)
			{
				break;
			}
*/
			HxLOG_Print("HandleMessage (eSciCmd_CardState) , PalState (%d), Slot Id (%d) \n",
				ePalStatus, ulEvtSlotId);

			if (ePalStatus == ePAL_SC_CARDSTATUS_NOT_PRESENT)
			{
				/*
					변경 : removed callback은 모든 cas에게 전달 한다.
				*/
				for(ulHandleCounter=0; ulHandleCounter<palSc_SCI_NUM_HANDLE; ulHandleCounter++)
				{
					pSci = pal_sc_GetInstFromHandle(ulHandleCounter);
					if(pSci == NULL)
					{
						HxLOG_Critical("\n\n");
						continue;
					}

					HxLOG_Print("Inst Handle [%d] its Slot Id [%d] and Event Slot ID[%d]\n", ulHandleCounter, pSci->ulSlotId, ulEvtSlotId);

					if((pSci->eCasId) != 0 && (pSci->ulSlotId == ulEvtSlotId))
					{
						if(pSci->pfnCbSci != NULL)
						{
							HxLOG_Print("callback to cas id (%x), slot id (%d), status (%d)\n", pSci->eCasId, pSci->ulSlotId, ePalStatus);
							pSci->pfnCbSci(ulHandleCounter, ulEvtSlotId, ePalStatus);
						}
					}
				}
			}
			else
			{
				for(ulHandleCounter=0; ulHandleCounter<palSc_SCI_NUM_HANDLE; ulHandleCounter++)
				{
					pSci = pal_sc_GetInstFromHandle(ulHandleCounter);
					if(pSci == NULL)
					{
						HxLOG_Critical("\n\n");
						continue;
					}

					HxLOG_Print("Inst Handle [%d] its Slot Id [%d] and Event Slot ID[%d]\n", ulHandleCounter, pSci->ulSlotId, ulEvtSlotId);

					// Find out SCI for same SC Slot ID
					if(pSci->ulSlotId == ulEvtSlotId)
						break;
				}

				if (ulHandleCounter >= palSc_SCI_NUM_HANDLE)
				{
					HxLOG_Print("callback can not be called with slot ID (%d)\n", ulEvtSlotId);
					break;
				}

				if((pSci->eCasId) != 0 && (pSci->ulSlotId == ulEvtSlotId))
				{
					if(pSci->pfnCbSci != NULL)
					{
						HxLOG_Print("callback to cas id (%x), slot id (%d), status (%d)\n", pSci->eCasId, pSci->ulSlotId, ePalStatus);
						pSci->pfnCbSci(ulHandleCounter, ulEvtSlotId, ePalStatus);
					}
				}
			}
			HxLOG_Print("HandleMessage (eSciCmd_CardState) , PalState (%d), Slot Id (%d) \n",
				ePalStatus, ulEvtSlotId);

			break;

		case eSciCmd_CardSelect:
			// multi Cas 군 중, 해당 Slot 별 점유할 CAS 선택
			hSci = pMsg->p1;
			eMatched = pMsg->p2;

			HxLOG_Print("HandleMessage (eSciCmd_CardSelect), hSci (%d), eMatched (%d) \n",
				hSci, eMatched);

			pSci = pal_sc_GetInstFromHandle(hSci);
			if(pSci == NULL)
			{
				HxLOG_Critical("\n\n");
				break;
			}

			pSci->eMatched = eMatched;

			if(eMatched == ePAL_SC_FLAG_ENABLE)
			{
				/*
					필요한 경우, 동일 slot의 다른 cas id가 Matched가 Enable인 경우를 살펴 보고 ,
					존재하는 경우 Error 처리한다.
				*/
				HxLOG_Print("[%d]th instance matched with [%d]th slot \n", (HUINT32)hSci, pSci->ulSlotId);
			}
			else
			{
				hNextSci = pal_sc_GetNextSCIHandle(hSci);
				if (hNextSci == HANDLE_NULL)
				{
					HxLOG_Print("Next SCI INST for hSci (%d) was NULL \n", hSci);
					break;
				}

				hError = PAL_SC_GetCardStatusBySlotID(pSci->ulSlotId, &eCurSlotStatus);
				if(ERR_FAIL == hError)
				{
					HxLOG_Print("Current Card Slot(%d) Status cannot be retreived !!! ", pSci->ulSlotId);
					break;
				}

				if (eCurSlotStatus == ePAL_SC_CARDSTATUS_PRESENT)
				{
					HUINT32 ulPrevSlotId = pSci->ulSlotId;

					// Dont have to check Null for pSci
					pSci = pal_sc_GetInstFromHandle(hNextSci);
					if(pSci == NULL)
					{
						HxLOG_Print("It can't be NULL return from pal_sc_GetInstFromHandle()\n");
						break;
					}

					HxLOG_Print("callback to s_ePalStatus (%d) pSci->eCasId (%x), pSci->eMatched (%d), pSci->ulSlotId (%d) \n",
						eCurSlotStatus, pSci->eCasId, pSci->eMatched, pSci->ulSlotId);

if (palSc_SCI_NUM_CAS > 1)
{
					HUINT32 i = 0;
					for (i = 0; i < hSci; i++)
					{
						palSc_SciInst_t *pPrevSci = pal_sc_GetInstFromHandle(i);
						if (pPrevSci)
						{
							HxLOG_Print("pPrevSci[eCasId : 0x%x][eMatched : %d][pPrevSci->ulSlotId : %d]\n", pPrevSci->eCasId, pPrevSci->eMatched, pPrevSci->ulSlotId);
							if (pPrevSci->eMatched == ePAL_SC_FLAG_ENABLE && pPrevSci->ulSlotId == ulPrevSlotId)
							{
								return;
							}
						}
					}
}
#if 0
					if ((pSci->eCasId == eDxCAS_GROUPID_IR) && (hSci != 0))
					{
						palSc_SciInst_t *pPrevSci = pal_sc_GetInstFromHandle(0);
						if (pPrevSci)
						{
							if ((pPrevSci->eCasId == eDxCAS_GROUPID_NA) && (pPrevSci->eMatched == ePAL_SC_FLAG_ENABLE))
							{
								break;
							}
						}
					}
#endif

					if(pSci->pfnCbSci != NULL)
					{
						HxLOG_Print("cas id (%x), slot id (%d), status (%d)\n", pSci->eCasId, pSci->ulSlotId, eCurSlotStatus);
						pSci->pfnCbSci(hNextSci, pSci->ulSlotId, eCurSlotStatus);
					}
				}
			}
			break;

		default:
			break;
	}

	return;
}

STATIC void pal_sc_Task(void *p)
{
	HUINT32 nResult = 0;
	palSc_SciMsg_t sc_msg;

	while (1)
	{
		nResult = VK_MSG_Receive (s_ulPalScCmdMsgQId, &sc_msg, sizeof(palSc_SciMsg_t));
		if(nResult == ERR_OK)
		{
			pal_sc_HandleMsg(&sc_msg);
		}
	}

	return;
}

HERROR PAL_SC_Init(void)
{
	DI_ERR_CODE nResult = 0;
	HUINT32 i = 0;

	if(s_bScInit != FALSE)
	{
		return ERR_FAIL;
	}

	// 1. get capability
	nResult = DI_SC_GetCapability(&s_ulMaxNumOfSlot);

	if(nResult != DI_ERR_OK)
	{
		HxLOG_Critical("\n");
	}
	HxLOG_Print("num of slot (%d) \n", s_ulMaxNumOfSlot);

#if defined(CONFIG_MW_CAS_NUM_SC_SLOT)
	if(CONFIG_MW_CAS_NUM_SC_SLOT != s_ulMaxNumOfSlot)
	{
		HxLOG_Error("The number of Smart Card slot is invaild. From Config[%d], From DI[%d]\n", CONFIG_MW_CAS_NUM_SC_SLOT, s_ulMaxNumOfSlot);
	}
#endif

	// 2. status initialize
	HxSTD_memset(&s_stScInst[0], 0x00, sizeof( palSc_SciInst_t)* palSc_SCI_NUM_HANDLE);

	HxLOG_Print("palSc_SCI_NUM_CAS (%d) \n", palSc_SCI_NUM_CAS);

if (palSc_SCI_NUM_CAS > 1)
{

#if defined (CONFIG_MW_CAS_NUM_SC_SLOT)

	nResult = (HUINT32)VK_MSG_Create(PAL_SC_MSGQ_SIZE, sizeof(palSc_SciMsg_t), "ScMsgQ", (unsigned long *)&s_ulPalScCmdMsgQId, VK_SUSPENDTYPE_PRIORITY);
	if (nResult != ERR_OK)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	HxLOG_Print("Task. \n");

	nResult = (HUINT32)VK_TASK_Create (pal_sc_Task,
										PAL_SC_TASK_PRIORITY,
										PAL_SC_TASK_STACK_SIZE,
										"tPSc",
										NULL,
										&s_ulPalScTaskId, 0);
	if (nResult != ERR_OK)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	nResult = VK_TASK_Start(s_ulPalScTaskId);
	if (nResult != ERR_OK)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

#endif

}


	for(i=0; i<s_ulMaxNumOfSlot; i++)
	{
		DI_SC_RegisterCallback(i, pal_sc_DeviceCallback);
	}

	s_bScInit = TRUE;

	return ERR_OK;
}

HUINT32 PAL_SC_GetNumberOfModule(void)
{
	return s_ulMaxNumOfSlot;
}

HERROR PAL_SC_RegisterCbFunc(HUINT32 ulSlotId, DxCAS_GroupId_e eCasId, PalSc_Callback_t pfnCb, PalSc_Handle_t *hSci)
{
	HUINT32 ulErr;
	HUINT8	i;
	DI_SC_STATUS eStatus;
	PalSc_CardStatus_e ePalStatus = ePAL_SC_CARDSTATUS_NOT_PRESENT;

	HxLOG_Print("cas<%x> pfnCb<%x>\n", eCasId, (HUINT32)pfnCb);

	if(ulSlotId >= palSc_SCI_NUM_SLOT)
	{
		return ERR_FAIL;
	}

	if(s_bScInit == FALSE)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	for(i=0; i<palSc_SCI_NUM_HANDLE; i++)
	{
		if(s_stScInst[i].eCasId == 0)
		{
			s_stScInst[i].ulSlotId = ulSlotId;
			s_stScInst[i].eCasId = eCasId;
			s_stScInst[i].pfnCbSci = pfnCb;
			*hSci = (PalSc_Handle_t)i;
			break;
		}
	}

	if(i == palSc_SCI_NUM_HANDLE)
	{
		HxLOG_Error("\n");
		*hSci = (PalSc_Handle_t)NULL;
		return ERR_FAIL;
	}

	ulErr = DI_SC_GetCardStatus(ulSlotId, &eStatus);
	if(ulErr != DI_ERR_OK)
	{
		eStatus = DI_SC_NOT_PRESENT;
	}

	if (eStatus == DI_SC_PRESENT)
		ePalStatus = ePAL_SC_CARDSTATUS_PRESENT;

	s_stScInst[i].ulSlotId = ulSlotId;

if (palSc_SCI_NUM_CAS > 1)
{
	palSc_SciMsg_t		msg;
	HUINT32 			nResult = ERR_OK;

	/* Message Create */
	HxSTD_memset(&msg, 0x00, sizeof(palSc_SciMsg_t));
	msg.eCmd = eSciCmd_CardState;
	msg.p1 = ePalStatus;
	msg.p2 = ulSlotId;

	HxLOG_Print("eSciCmd_CardState, PalState (%d), Slot Id (%d) \n",
		ePalStatus, ulSlotId);

	nResult = VK_MSG_Send(s_ulPalScCmdMsgQId, &msg, sizeof(palSc_SciMsg_t));
	if(nResult != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

}
else
{
	if (s_stScInst[i].pfnCbSci)
	{
		s_stScInst[i].pfnCbSci((PalSc_Handle_t)i, ulSlotId, ePalStatus);
	}
}

	HxLOG_Print("[SCI_RegisterCbFunc] *hSci<%x>\n", (HUINT32)*hSci);

	return ERR_OK;
}

HERROR PAL_SC_UnregisterCbFunc(PalSc_Handle_t hSci)
{
	palSc_SciInst_t *pSci = (palSc_SciInst_t *)NULL;

	HxLOG_Print(" hSci (%d)\n", hSci);

	pSci = pal_sc_GetInstFromHandle(hSci);
	if(pSci == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	pSci->pfnCbSci = NULL;

	return ERR_OK;
}

HERROR PAL_SC_NotifyScValidity(PalSc_Handle_t hSci, PalSc_Flag_e eMatched)										/* handle에 해당하는 cas id 및 slot이 init 완료 됨을 의미 */
{
	palSc_SciInst_t *pSci = (palSc_SciInst_t *)NULL;

if (palSc_SCI_NUM_CAS > 1)
{
	palSc_SciMsg_t		msg;
	HUINT32 			nResult = ERR_OK;

	HxLOG_Print(" hSci (%d), eMatched (%d)\n", hSci, eMatched);

	/* Message Create */
	HxSTD_memset(&msg, 0x00, sizeof(palSc_SciMsg_t));
	msg.eCmd = eSciCmd_CardSelect;
	msg.p1 = hSci;
	msg.p2 = eMatched;

	nResult = VK_MSG_Send(s_ulPalScCmdMsgQId, &msg, sizeof(palSc_SciMsg_t));
	if(nResult != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}
}
else
{
	HxLOG_Print(" hSci (%d), eMatched (%d)\n", hSci, eMatched);

	pSci = pal_sc_GetInstFromHandle(hSci);
	if(pSci == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	pSci->eMatched = eMatched;

	if(eMatched == ePAL_SC_FLAG_ENABLE)
	{
		/*
			필요한 경우, 동일 slot의 다른 cas id가 Matched가 Enable인 경우를 살펴 보고 ,
			존재하는 경우 Error 처리한다.
		*/
		HxLOG_Print("[%d]th instance matched with [%d]th slot \n", (HUINT32)hSci, pSci->ulSlotId);
	}
}

	return ERR_OK;
}

/* cold , warm에 대한 파라미터 비교. */
/* 개선 사항 : sci sc param의 경우에는 protocol에 dependent한 값이므로 초기 default 값은 cas가 전달해주도록 한다. */
HERROR PAL_SC_Reset(
						PalSc_Handle_t hSci,
						HUINT8 *pucAtr,
						HUINT32 *pulLen,
						PalSc_ResetType_e eResetType,
						PalSc_Param_t *pstDefaultParam
						)
{
	palSc_SciInst_t *pSci = (palSc_SciInst_t *)NULL;
	DI_ERR_CODE nResult = 0;
	DI_SC_PROTOCOL di_protocol;
	DI_SC_STATUS eStatus;

	pSci = pal_sc_GetInstFromHandle(hSci);
	if(pSci == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	/*
		Multi cas 진행 시 sub cas에서 동시에 Reset을 거는 경우에 대한 교통 정리 필요할 것 같다.
	*/

	/*
		해당 slot에 card가 없는 경우 처리.
	*/
	nResult = DI_SC_GetCardStatus(pSci->ulSlotId, &eStatus);
	if(nResult != ERR_OK || eStatus != DI_SC_PRESENT)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	di_protocol = pal_sc_ConvertProtocolPal2Di(pstDefaultParam->eProtocol);

	nResult = DI_SC_SetParams(
							pSci->ulSlotId, 						// slot id
							di_protocol, 							// protocol
							pstDefaultParam->ulClock, 				//unsigned long ulMinClock,
							pstDefaultParam->ulBaudrate, 			//unsigned long ulSrcBaudrate,
							pstDefaultParam->ucFI, 					//unsigned char ucFI,
							pstDefaultParam->ucDI, 					//unsigned char ucDI,
							pstDefaultParam->ucN, 					//unsigned char ucN,
							pstDefaultParam->ucCWI, 				//unsigned char ucCWI,
							pstDefaultParam->ucBWI 					//unsigned char ucBWI
							);

	if(nResult != DI_ERR_OK)
	{
		HxLOG_Error("set param (%d)\n", nResult);
		return ERR_FAIL;
	}

	if(eResetType == ePAL_SC_RESETTYPE_COLD)
	{
		nResult = DI_SC_DownUpVCC(pSci->ulSlotId, 20);
		if(nResult != DI_ERR_OK)
		{
			HxLOG_Error("down up vcc (%d)\n", nResult);
			return ERR_FAIL;
		}
	}

	nResult = DI_SC_Reset(
							pSci->ulSlotId, 						//HUINT32 unDeviceId,
							pucAtr, 								//HUINT8 * pucAtr,
							pulLen									//HUINT32 * pucAtrlen,
							);
	if(nResult != DI_ERR_OK)
	{
		HxLOG_Error("reset (%d)\n", nResult);
		return ERR_FAIL;
	}


	return ERR_OK;
}

HERROR PAL_SC_GetCardStatus(PalSc_Handle_t hSci, PalSc_CardStatus_e *peStatus)
{
	palSc_SciInst_t *pSci = (palSc_SciInst_t *)NULL;
	DI_SC_STATUS eDiStatus = DI_SC_NOT_PRESENT;
	DI_ERR_CODE nResult = DI_ERR_OK;

	pSci = pal_sc_GetInstFromHandle(hSci);
	if(pSci == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	nResult = DI_SC_GetCardStatus(pSci->ulSlotId, &eDiStatus);
	if(nResult != DI_ERR_OK)
	{
		HxLOG_Error("status (%d)\n", nResult);
		return ERR_FAIL;
	}

	if(eDiStatus == DI_SC_NOT_PRESENT)
	{
		*peStatus = ePAL_SC_CARDSTATUS_NOT_PRESENT;
	}
	else
	{
		*peStatus = ePAL_SC_CARDSTATUS_PRESENT;
	}

	return ERR_OK;
}

HERROR PAL_SC_GetCardStatusBySlotID(HUINT32 ulSlotId, PalSc_CardStatus_e *peStatus)
{
	DI_SC_STATUS eDiStatus = DI_SC_NOT_PRESENT;
	DI_ERR_CODE nResult = DI_ERR_OK;

	nResult = DI_SC_GetCardStatus(ulSlotId, &eDiStatus);
	if(nResult != DI_ERR_OK)
	{
		HxLOG_Error("status (%d)\n", nResult);
		return ERR_FAIL;
	}

	if(eDiStatus == DI_SC_NOT_PRESENT)
	{
		*peStatus = ePAL_SC_CARDSTATUS_NOT_PRESENT;
	}
	else
	{
		*peStatus = ePAL_SC_CARDSTATUS_PRESENT;
	}

	return ERR_OK;
}

HERROR PAL_SC_GetSlotId(PalSc_Handle_t hSci, HUINT8 *pulSlotId)
{
	palSc_SciInst_t *pSci = (palSc_SciInst_t *)NULL;

	pSci = pal_sc_GetInstFromHandle(hSci);
	if(pSci == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	*pulSlotId = pSci->ulSlotId;

	HxLOG_Print("slot id (%d) \n", pSci->ulSlotId);

	return ERR_OK;
}

/* after ATR */
HERROR PAL_SC_SetParams(PalSc_Handle_t hSci, PalSc_Param_t *pstParam)
{
	palSc_SciInst_t *pSci = (palSc_SciInst_t *)NULL;
	DI_ERR_CODE nResult = DI_ERR_OK;
	DI_SC_PROTOCOL di_protocol;

	pSci = pal_sc_GetInstFromHandle(hSci);
	if(pSci == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	di_protocol = pal_sc_ConvertProtocolPal2Di(pstParam->eProtocol);

	nResult = DI_SC_SetParams(
							pSci->ulSlotId, 						// slot id
							di_protocol, 							// protocol
							pstParam->ulClock, 				//unsigned long ulMinClock,
							pstParam->ulBaudrate, 			//unsigned long ulSrcBaudrate,
							pstParam->ucFI, 					//unsigned char ucFI,
							pstParam->ucDI, 					//unsigned char ucDI,
							pstParam->ucN, 					//unsigned char ucN,
							pstParam->ucCWI, 				//unsigned char ucCWI,
							pstParam->ucBWI 				//unsigned char ucBWI
							);

	if(nResult != DI_ERR_OK)
	{
		HxLOG_Error("set param (%d)\n", nResult);
		return ERR_FAIL;
	}


	return ERR_OK;
}

HERROR PAL_SC_GetParams(PalSc_Handle_t hSci, PalSc_Param_t *pstParam)
{
	palSc_SciInst_t *pSci = (palSc_SciInst_t *)NULL;
	DI_ERR_CODE nResult = DI_ERR_OK;
	DI_SC_PROTOCOL di_protocol;

	pSci = pal_sc_GetInstFromHandle(hSci);
	if(pSci == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	nResult = DI_SC_GetParams(
							pSci->ulSlotId, 						//HUINT32 unDeviceId,
							&di_protocol,	 						//DI_SC_PROTOCOL * pnProtocol,
							(unsigned long *)&pstParam->ulClock, 					//unsigned long * pulClock,
							(unsigned long *)&pstParam->ulBaudrate, 					//unsigned long * pulBaudrate,
							&pstParam->ucFI, 						//unsigned char * pucFI,
							&pstParam->ucDI, 						//unsigned char * pucDI,
							&pstParam->ucN, 						//unsigned char * pucN,
							&pstParam->ucCWI, 						//unsigned char * pucCWI,
							&pstParam->ucBWI						//unsigned char * pucBWI
							);
	if(nResult != DI_ERR_OK)
	{
		HxLOG_Error("get param (%d)\n", nResult);
		return ERR_FAIL;
	}

	pstParam->eProtocol = pal_sc_ConvertProtocolDi2Pal(di_protocol);

	return ERR_OK;
}

/* 사용처 확인. T0 */
HERROR PAL_SC_Write(PalSc_Handle_t hSci, HUINT8 *pucWriteBuf, HUINT32 ulNumWrite, HUINT8 *pucReadBuf, HUINT32 *pulNumRead)
{
	palSc_SciInst_t *pSci = (palSc_SciInst_t *)NULL;
	DI_ERR_CODE nResult = DI_ERR_OK;

	if( (pucWriteBuf == NULL) || (pucReadBuf == NULL) || (ulNumWrite == 0) )
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	pSci = pal_sc_GetInstFromHandle(hSci);
	if(pSci == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	nResult = DI_SC_WriteData(
							pSci->ulSlotId,				//HUINT32 unDeviceId,
							pucWriteBuf,				//unsigned char * pucWriteBuf,
							ulNumWrite,					//int ucNumToWrite,
							pucReadBuf,					//unsigned char * pucReadBuf,
							pulNumRead					//int * pucNumRead
							);
	if(nResult != DI_ERR_OK)
	{
		HxLOG_Error("write (%d)\n", nResult);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR PAL_SC_Read(PalSc_Handle_t hSci, HUINT8 *pucWriteBuf, HUINT32 ulNumWrite, HUINT8 *pucReadBuf, HUINT32 *pulNumRead)
{
	palSc_SciInst_t *pSci = (palSc_SciInst_t *)NULL;
	DI_ERR_CODE nResult = DI_ERR_OK;

	if( (pucWriteBuf == NULL) || (pucReadBuf == NULL) || (ulNumWrite == 0) )
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	pSci = pal_sc_GetInstFromHandle(hSci);
	if(pSci == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	nResult = DI_SC_ReadData(
							pSci->ulSlotId,				//HUINT32 unDeviceId,
							pucWriteBuf,				//unsigned char * pucWriteBuf,
							ulNumWrite,					//int ucNumToWrite,
							pucReadBuf,					//unsigned char * pucReadBuf,
							pulNumRead					//int * pucNumRead
							);
	if(nResult != DI_ERR_OK)
	{
		HxLOG_Error("read (%d)\n", nResult);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/* T1 */
HERROR PAL_SC_Transfer(PalSc_Handle_t hSci, HUINT8 *pucWriteBuf, HUINT32 ulNumWrite, HUINT8 *pucReadBuf, HUINT32 *pulNumRead, HUINT32 ulEpxectedNumRead)
{
	palSc_SciInst_t *pSci = (palSc_SciInst_t *)NULL;
	DI_ERR_CODE nResult = DI_ERR_OK;

	if( (pucWriteBuf == NULL) || (pucReadBuf == NULL) || (ulNumWrite == 0) )
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	pSci = pal_sc_GetInstFromHandle(hSci);
	if(pSci == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	nResult = DI_SC_TransferData(
								pSci->ulSlotId,				//HUINT32 unDeviceId,
								pucWriteBuf,				//unsigned char * pucWriteBuf,
								ulNumWrite, 				//int ucNumToWrite,
								pucReadBuf, 				//unsigned char * pucReadBuf,
								pulNumRead 				//int * pucNumRead
								);
	if(nResult != DI_ERR_OK)
	{
		HxLOG_Error("transfer (%d)\n", nResult);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR PAL_SC_Deactivate(PalSc_Handle_t hSci)
{
	palSc_SciInst_t *pSci = (palSc_SciInst_t *)NULL;
	DI_ERR_CODE nResult = DI_ERR_OK;

	pSci = pal_sc_GetInstFromHandle(hSci);
	if(pSci == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	nResult = DI_SC_Deactivate(pSci->ulSlotId);
	if(nResult != DI_ERR_OK)
	{
		HxLOG_Error("deactivate (%d)\n", nResult);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR PAL_SC_Activate(PalSc_Handle_t hSci)
{
	palSc_SciInst_t *pSci = (palSc_SciInst_t *)NULL;
	DI_ERR_CODE nResult = DI_ERR_OK;

	pSci = pal_sc_GetInstFromHandle(hSci);
	if(pSci == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	nResult = DI_SC_Activate(pSci->ulSlotId);
	if(nResult != DI_ERR_OK)
	{
		HxLOG_Error("activate (%d)\n", nResult);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR PAL_SC_NegotiatePTS(PalSc_Handle_t hSci, HUINT8 *pucWriteBuf, HUINT8 ucNumToWrite, HUINT8 *pucReadBuf, HUINT8 *pucNumRead)
{
   palSc_SciInst_t *pSci = (palSc_SciInst_t *)NULL;
   DI_ERR_CODE nResult = DI_ERR_OK;
   int nNumRead;

   pSci = pal_sc_GetInstFromHandle(hSci);
   if(pSci == NULL)
   {
             HxLOG_Critical("\n\n");
             return ERR_FAIL;
   }

   nResult = DI_SC_NegotiatePTS(pSci->ulSlotId, pucWriteBuf, ucNumToWrite, pucReadBuf, &nNumRead);

   *pucNumRead = nNumRead;

   if(nResult != DI_ERR_OK)
   {
             HxLOG_Error("nego (%d)\n", nResult);
             return ERR_FAIL;
   }

   return ERR_OK;
}


void PAL_SC_PrintStatus(void)
{
	HUINT32 i = 0;

	HxLOG_Print("\n\n================== pal_sc =====================\n\n");

	for(i=0; i<palSc_SCI_NUM_HANDLE; i++)
	{
		HxLOG_Print("\n	%d-th instance", i);

		HxLOG_Print("\n	ulSlotId		: %d ", s_stScInst[i].ulSlotId);
		HxLOG_Print("\n	eCasId	 		: %x ", s_stScInst[i].eCasId);
		HxLOG_Print("\n	eMatched 		: %d ", s_stScInst[i].eMatched);
		HxLOG_Print("\n	pfnCbSci		: %x ", s_stScInst[i].pfnCbSci);
	}

	HxLOG_Print("\n\n================== end of pal_sc =====================\n\n");

	return;
}

/*********************** End of File ******************************/
