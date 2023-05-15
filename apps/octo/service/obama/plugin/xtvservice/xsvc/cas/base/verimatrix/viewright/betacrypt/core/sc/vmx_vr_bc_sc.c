/* (c) 2011-2015 Humax Co., Ltd. 
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
/**
	@file     vmx_vr_bc_sc.c
	@brief    vmx_vr_bc_sc.c (Verimatrix BC SC)

	Description:   \n
	Module: MW/CAS/ VERIMATRIX \n
	Remarks :\n

	Copyright (c) 2015 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "pal_sc.h"
#include "bc_consts.h"
#include "_xsvc_cas_vmx_bc_main.h"
#include "vmx_vr_bc_api.h"
#include "_xsvc_cas_vmx_bc_sc.h"
#include "_xsvc_cas_vmx_os.h"
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define VMXBC_PPS_EXCHANGE	// PPS exchange를 여기서 구현한다. ==> sc mgr이나 PAL_SC에서 해주는 것이 맞는 것 같은데...

#if (VMXBC_SC_SLOT_NUM ==1)
#define VMXBC_SC_DEFAULT_SLOT_ID		(0)
#endif
#define VMXBC_SC_T1_PROLOGUE_LEN      	(3)          	// T=1 Prologue field Length : NAD(1 byte), PCB(1 byte), LEN(1 byte)
#define VMXBC_SC_T1_INFORMATION_LEN	(254)		// T=1 Information field : INF( 0 to 254 byte )
#define VMXBC_SC_T1_EPILOGUE_LEN      	(1)   	      // T=1 Epilogue field Length : LRC(1 byte)
#define VMXTC_SC_T1_LENG					(VMXBC_SC_T1_INFORMATION_LEN + VMXBC_SC_T1_PROLOGUE_LEN + VMXBC_SC_T1_EPILOGUE_LEN)
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/*
typedef enum
{
	eSvcXxx_Type,

} xsvcXxx_Type_e;

typedef  struct
{
	...
} xsvcXxx_Type_t;
*/
typedef enum
{
	eVmxBc_ScState_Extracted = 0,
	eVmxBc_ScState_Inserted,
} VmxBc_ScState_e;

typedef enum
{
	eVmxBc_ScActionType_Init = 0,
	eVmxBc_ScActionType_Reset,
} VmxBc_ScActionType_e;
 
typedef struct
{
	VmxBc_ScState_e		stScState;
	VmxBc_ScActionType_e	stScActionType;

	HUINT8 ucProtocol;

	HUINT32 ucAtrLength;
	HUINT8 ucAtrBuffer[256];

	HUINT16 ucReadLength;
	HUINT8 ucReadData[VMXTC_SC_T1_LENG];
} VmxBc_SC_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HULONG sVmxBcScQid;
STATIC HULONG sVmxBcScTid;

VmxBc_SC_t stVmxBcSc[VMXBC_SC_SLOT_NUM];

STATIC HERROR bc_sc_WriteToSC(HUINT8* pucBuffer, HUINT16 *pusLen, HUINT16 usTimeout);

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
/*	
// Local function
STATIC HERROR xsvc_xxx_DoSomething_Yyy( ... )
{
	...
}
*/

/*
// Global function
HERROR xsvc_xxx_DoSomething_Yyy( ... )
{
	...
}
*/

HUINT8 *VmxBc_Cmd[] =
{
	"eVmxBc_Cmd_ConnectSc",
	"eVmxBc_Cmd_DisconnectSc",
	"eVmxBc_Cmd_ResetSc",
	"eVmxBc_Cmd_GetATRSc",
	"eVmxBc_Cmd_CardDetectSc"
};

HUINT8 *VmxBc_SC_MsgType[] =
{
	"eVmxBc_MsgType_Init",
	"eVmxBc_MsgType_Reset",
	"eVmxBc_MsgType_ScExtracted",
	"eVmxBc_MsgType_ScInserted",
	"eVmxBc_MsgType_WriteToSc",
	"eVmxBc_MsgType_Error"
};
 
STATIC VmxBc_SC_t * bc_sc_GetVmxBcSc(HUINT16 idx)
{
	return &stVmxBcSc[idx];
}

STATIC void bc_sc_Task ( void * args )
{
	Vmx_Message_t stMsg;
	VmxBc_SC_t	 *pSC = NULL;
	
	HUINT32 ulScSlot;

	while (1)
	{
		if (VMX_MSG_Receive(sVmxBcScQid, (void*)&stMsg, sizeof(Vmx_Message_t)) == VK_OK)
		{
			HxLOG_Print(DBG_PRINT, ("[SC Task] Received[%d] - %s\n", stMsg.param1, VmxBc_SC_MsgType[stMsg.param1]);
			switch (stMsg.param1)
			{
				case eVmxBc_MsgType_Reset:
					ulScSlot = stMsg.param2;
					pSC = bc_sc_GetVmxBcSc(ulScSlot);
					if(pSC == NULL)
					{
						HxLOG_Error(DBG_ASSERT, ("pSC is NULL\n");
						vmx_vr_bc_api_ScNotify(BC_SC_ERROR);
						break;
					}
					pSC->ucAtrLength = 0;
					VMX_memset(pSC->ucAtrBuffer, 0, sizeof(pSC->ucAtrBuffer));
					pSC->stScActionType = eVmxBc_ScActionType_Init;
					if(xsvc_cas_vmx_bc_sc_Reset(ulScSlot, pSC->ucAtrBuffer, &pSC->ucAtrLength, &pSC->ucProtocol) != ERR_OK)
					{
						HxLOG_Error(DBG_ASSERT, ("xsvc_cas_vmx_bc_sc_Reset : error\n");
						vmx_vr_bc_api_ScNotify(BC_SC_ERROR);
						break;
					}
					pSC->stScActionType = eVmxBc_ScActionType_Reset;
					vmx_vr_bc_api_ScNotify(BC_SC_RESET);
					break;
				case eVmxBc_MsgType_ScExtracted:
					ulScSlot = stMsg.param2;
					pSC = bc_sc_GetVmxBcSc(ulScSlot);
					if(pSC == NULL)
					{
						HxLOG_Error(DBG_ASSERT, ("pSC is NULL\n");
						vmx_vr_bc_api_ScNotify(BC_SC_REMOVED);
						break;
					}
					if(pSC->stScState == eVmxBc_ScState_Extracted)
					{
						HxLOG_Error(DBG_ASSERT, ("smartcard alreay removed\n");
					}
					pSC->stScState = eVmxBc_ScState_Extracted;
					vmx_vr_bc_api_ScNotify(BC_SC_REMOVED);
					break;

				case eVmxBc_MsgType_ScInserted:
					ulScSlot = stMsg.param2;
					pSC = bc_sc_GetVmxBcSc(ulScSlot);
					if(pSC == NULL)
					{
						HxLOG_Error(DBG_ASSERT, ("pSC is NULL\n");
						vmx_vr_bc_api_ScNotify(BC_SC_INSERTED);
						break;
					}
					pSC->stScState = eVmxBc_ScState_Inserted;
					vmx_vr_bc_api_ScNotify(BC_SC_INSERTED);
					break;
				case eVmxBc_MsgType_WriteToSc:
					bc_sc_WriteToSC(stMsg.param2, &stMsg.param3, stMsg.param4);
					vmx_vr_bc_api_ScNotify(BC_SC_RW_COMPLETED);
					break;
				default:
					HxLOG_Error(DBG_ASSERT, ("Unknown Msg (%d)\n", stMsg.param1);
					break;
			}
		}
	}
}

STATIC HERROR bc_sc_CreateTask(void)
{
	int nErr;
	HxLOG_Print(DBG_PRINT, ("[Enter]\n");

	VMX_MSG_Create(VMX_TASK_MSGQ_SIZE,  sizeof(Vmx_Message_t), VMXBC_SC_QUEUE_NAME, &sVmxBcScQid);

	nErr = VK_TASK_Create((void *)bc_sc_Task, VMXBC_SC_TASK_PRIORITY, VMXBC_SC_STACK_SIZE, VMXBC_SC_TASK_NAME, NULL, &sVmxBcScTid, 0);
	if (nErr)
	{
		HxLOG_Error(DBG_ASSERT, ("failed to create task (%d)\n", nErr);
		return ERR_FAIL;
	}

	nErr = VK_TASK_Start(sVmxBcScTid);
	if (nErr)
	{
		HxLOG_Error(DBG_ASSERT, ("failed to start task (%d)\n", nErr);
		return ERR_FAIL;
	}
	HxLOG_Print(DBG_PRINT, ("[Exit]\n");

	return ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////    Betacrypt Library API   //////////////////////////////////////////
/////////////////////////////////////    Betacrypt Library API   //////////////////////////////////////////
/////////////////////////////////////    Betacrypt Library API   //////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(CONFIG_DEBUG)
void bc_sc_TestScNotify(HUINT8 ucCmd)
{
	VmxBc_SC_t *pSC = bc_sc_GetVmxBcSc(VMXBC_SC_DEFAULT_SLOT_ID);
	enBcScNotify_t notify = 0;

	if(pSC == NULL)
		return; 

	if(ucCmd==0)				notify = BC_SC_NONE;
	else if(ucCmd==1)		notify = BC_SC_RW_COMPLETED;
	else if(ucCmd==2)		notify = BC_SC_SETPARAM_COMPLETED;
	else if(ucCmd==3)		notify = BC_SC_POWERDOWN_COMPLETED;
	else if(ucCmd==4)		notify = BC_SC_INSERTED;
	else if(ucCmd==5)		notify = BC_SC_REMOVED;
	else if(ucCmd==6)		notify = BC_SC_ERROR;
	else if(ucCmd==7)		notify = BC_SC_RESET;
	else if(ucCmd==8)		notify = BC_SC_MUTE;

	HxLOG_Print(DBG_PRINT, ("[Enter]\n");
	vmx_vr_bc_api_ScNotify(notify);
	HxLOG_Print(DBG_PRINT, ("[Exit]\n");
}
#endif

STATIC HERROR bc_sc_BcInit(void)
{
	int i;
	VmxBc_SC_t *pSC = NULL;


	HxLOG_Print(DBG_PRINT, ("[Enter]\n");

	for (i = 0; i < VMXBC_SC_SLOT_NUM; i++)
	{
		pSC = bc_sc_GetVmxBcSc(i);

		if(pSC == NULL)
			continue;

		VMX_memset(pSC, 0, sizeof(VmxBc_SC_t)); 
		pSC->stScState = eVmxBc_ScState_Extracted;
		pSC->stScActionType = eVmxBc_ScActionType_Init;
	}

	HxLOG_Print(DBG_PRINT, ("[Exit]\n");

	return ERR_OK;
}

STATIC HUINT8 bc_sc_GenerateLRC(HUINT8 lrcData[], HUINT16 lrcLength)
{
	HUINT8 edc;
	HUINT16 i;

//	HxLOG_Print(DBG_PRINT, ("GenerateLRC++\n\r");
	edc = lrcData[0];
//	HxLOG_Print(DBG_PRINT, ("\t %x",edc);

	for (i = 1; i < lrcLength; ++i)
	{
		edc ^= lrcData[i];
		//HxLOG_Print(DBG_PRINT, (" %2x ",edc);
	}
//	HxLOG_Print(DBG_PRINT, ("\n\r");
//	HxLOG_Print(DBG_PRINT, ("GenerateLRC- 0x%x -\n\r", edc);

	return (edc);
}

STATIC HERROR bc_sc_WriteToSC(HUINT8* pucBuffer, HUINT16 *pusLen, HUINT16 usTimeout)
{
	HUINT16 usWriteLen =0;
	HUINT32 usReadLen =0;
	HUINT8 ucWriteData[VMXTC_SC_T1_LENG];
	HUINT8 ucReadData[VMXTC_SC_T1_LENG];
	VmxBc_SC_t * pSC = bc_sc_GetVmxBcSc(VMXBC_SC_DEFAULT_SLOT_ID);	

	HxLOG_Print(DBG_PRINT, ("[Enter] Len = %d\n", *pusLen);
	if(pSC == NULL || pucBuffer == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("Buffer Null or Len(%d)\n", *pusLen);
		return ERR_FAIL;
	}

	if(pSC->stScState == eVmxBc_ScState_Extracted)
	{
		HxLOG_Error(DBG_ASSERT, ("smart card is not inserted\n");
		return ERR_FAIL;
	}

	if(pSC->stScActionType == eVmxBc_ScActionType_Init)
	{
		HxLOG_Error(DBG_ASSERT, ("smart card is not inserted\n");
		return ERR_FAIL;
	}

	VMX_memset(ucWriteData, 0, sizeof(ucWriteData));
	VMX_memset(ucReadData, 0, sizeof(ucReadData));

	VMX_memset(pSC->ucReadData, 0, sizeof(pSC->ucReadData));
	pSC->ucReadLength = 0;
	 // Compose T=1 prologue block frame
	ucWriteData[0] = 0;			// NAD (Verimatrix do not use Node Address Byte)
	ucWriteData[1] = 0;       		// PCB (Verimatrix use only I-Block for send command to smartcard)
	ucWriteData[2] = *pusLen;   	// LEN (Information length)

	VMX_memcpy(ucWriteData + VMXBC_SC_T1_PROLOGUE_LEN, pucBuffer, *pusLen);
    	usWriteLen = *pusLen + VMXBC_SC_T1_PROLOGUE_LEN + VMXBC_SC_T1_EPILOGUE_LEN;
	ucWriteData[usWriteLen-1] = bc_sc_GenerateLRC(&ucWriteData[0], usWriteLen - VMXBC_SC_T1_EPILOGUE_LEN);

	//xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_SC, "WriteToSC", &ucWriteData[0], usWriteLen);

	if(xsvc_cas_vmx_bc_sc_Write(VMXBC_SC_DEFAULT_SLOT_ID, ucWriteData, usWriteLen, &ucReadData[0], &usReadLen) != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("xsvc_cas_vmx_bc_sc_Write : error\n");
		return ERR_FAIL;
	}

	// 수신된 DATA가 VALID한지 CHECK
	if(bc_sc_GenerateLRC(&ucReadData[0], usReadLen) !=  0)
	{
		HxLOG_Print(DBG_PRINT, ("[Exit] Invalid Data\n");
		return ERR_FAIL;
	}

 	pSC->ucReadLength = usReadLen - VMXBC_SC_T1_PROLOGUE_LEN - VMXBC_SC_T1_EPILOGUE_LEN;
	VMX_memcpy(pSC->ucReadData, ucReadData + VMXBC_SC_T1_PROLOGUE_LEN, pSC->ucReadLength);

//	xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_SC, "ReadFromSC", &ucReadData[0], usReadLen);

	HxLOG_Print(DBG_PRINT, ("[Exit]\n");
	return ERR_OK;
}

STATIC HERROR bc_sc_ReadFromSC(HUINT8* pucBuffer, HUINT16 *pusLen, HBOOL *pbTimeout)
{
	VmxBc_SC_t * pSC = bc_sc_GetVmxBcSc(VMXBC_SC_DEFAULT_SLOT_ID);

	HxLOG_Print(DBG_PRINT, ("[Enter]\n");

	if(pSC == NULL || pucBuffer == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("Buffer Null or Len(%d)\n", *pusLen);
		return ERR_FAIL;
	}

	if(pSC->stScState == eVmxBc_ScState_Extracted)
	{
		HxLOG_Error(DBG_ASSERT, ("smart card is not inserted\n");
		return ERR_FAIL;
	}

	if(pSC->stScActionType == eVmxBc_ScActionType_Init)
	{
		HxLOG_Error(DBG_ASSERT, ("smart card is not inserted\n");
		return ERR_FAIL;
	}

	// ucReadLength <= 0이라는 것은 아직 SC로부터 data를 수신하지 못한다는것을 뜻한다. 그래서 timeout으로 간주한다. 
	if(pSC->ucReadLength <= 0)
	{
		HxLOG_Error(DBG_ASSERT, ("smartcard read : timeout\n");
		*pbTimeout = TRUE;
		return ERR_FAIL;
	}

	*pbTimeout = FALSE;
	*pusLen = pSC->ucReadLength;
	VMX_memcpy(pucBuffer, pSC->ucReadData, pSC->ucReadLength);
 
	HxLOG_Print(DBG_PRINT, ("[Exitr]\n");
	return ERR_OK;
}

STATIC HERROR bc_sc_IoCtlWithSC(VmxBc_Cmd_e eCmd, void* pvParams, HUINT16* pusLen)
{
	Vmx_Message_t stMsg; 
	VmxBc_SC_t *pSC = bc_sc_GetVmxBcSc(VMXBC_SC_DEFAULT_SLOT_ID);

	if(pSC == NULL)
	{
		HxLOG_Error(DBG_ASSERT, ("Pointer NULL\n");
		return ERR_FAIL;
	}

	HxLOG_Print(DBG_PRINT, ("[Enter] eCmd[%d] = %s\n", eCmd, VmxBc_Cmd[eCmd]);

	switch(eCmd)
	{
  	 	case eVmxBc_Cmd_ConnectSc: 		/* connection between the host software and the betacrypt library is established. As parameter a function pointer to the notify function must be passed.*/
			if(pvParams == NULL)
			{
				HxLOG_Error(DBG_ASSERT, ("pvParams is NULL\n");
				goto err_fail;
			}
			vmx_vr_bc_api_ConnectScNotify(pvParams);
			break;		
  	 	case eVmxBc_Cmd_DisconnectSc: 	/* close the connection between the two systems */
			vmx_vr_bc_api_ConnectScNotify(NULL);
			break;		
  	 	case eVmxBc_Cmd_ResetSc:			/* Reset smartcard */
			if(pSC->stScState == eVmxBc_ScState_Extracted)
			{
				HxLOG_Error(DBG_ASSERT, ("Smartcard is not inserted\n");
				vmx_vr_bc_api_ScNotify(BC_SC_REMOVED);
				goto err_fail;
			}
			stMsg.param1 = eVmxBc_MsgType_Reset;
			stMsg.param2 = VMXBC_SC_DEFAULT_SLOT_ID;
			VMX_MSG_Send(sVmxBcScQid, &stMsg, sizeof(Vmx_Message_t));
			break;
  	 	case eVmxBc_Cmd_GetATRSc:		/* Get the ATR string sent by the SC */
			if(pSC->stScState == eVmxBc_ScState_Extracted ||pSC->stScActionType != eVmxBc_ScActionType_Reset || pSC->ucAtrLength == 0)
			{
				HxLOG_Error(DBG_ASSERT, ("Smartcard is not inserted\n");
				vmx_vr_bc_api_ScNotify(BC_SC_ERROR);
				goto err_fail;
			}
			VMX_memcpy(pvParams, pSC->ucAtrBuffer, pSC->ucAtrLength);
			* pusLen = pSC->ucAtrLength;

			xsvc_vmx_main_PrintDump(VMX_DBG_VR_BC_SC, "ATR", pvParams, * pusLen);			
			break;		
  	 	case eVmxBc_Cmd_CardDetectSc:		/* With this command the betacrypt library can check if a SC is present */
			if(pSC->stScState == eVmxBc_ScState_Extracted)
				vmx_vr_bc_api_ScNotify(BC_SC_REMOVED);
			else
				vmx_vr_bc_api_ScNotify(BC_SC_INSERTED);
			break;
		default:
			HxLOG_Error(DBG_ASSERT, ("Cmd is not valid\n");
			goto err_fail;
			break;
	}
	HxLOG_Print(DBG_PRINT, ("[Exit]\n");
	return ERR_OK;

err_fail:
	HxLOG_Print(DBG_PRINT, ("[Exit] - Fail\n");
	return ERR_FAIL;
}

HULONG vmx_vr_bc_sc_GetMsgQueueId(void)
{
	return sVmxBcScQid;
}

HERROR vmx_vr_bc_sc_WriteToSC(HUINT8* pucBuffer, HUINT16 *pusLen, HUINT16 usTimeout)
{
	HERROR rst;
	Vmx_Message_t stMsg;

	stMsg.param1 = eVmxBc_MsgType_WriteToSc;
	stMsg.param2 = pucBuffer;
	stMsg.param3 = *pusLen;
	stMsg.param4 = usTimeout;

	return VMX_MSG_Send(sVmxBcScQid, &stMsg, sizeof(Vmx_Message_t));
}

HERROR vmx_vr_bc_sc_ReadFromSC(HUINT8* pucBuffer, HUINT16 *pusLen, HBOOL *pbTimeout)
{
	return bc_sc_ReadFromSC(pucBuffer, pusLen, pbTimeout);
}

HERROR  vmx_vr_bc_sc_IoCtrlWithSC(VmxBc_Cmd_e eCmd, void* pvParams, HUINT16* pusLen)
{
	return bc_sc_IoCtlWithSC(eCmd, pvParams, pusLen);
}

HERROR vmx_vr_bc_sc_Init(void)
{
	HxLOG_Print(DBG_PRINT, ("[Enter]\n");
	if(bc_sc_CreateTask() != ERR_OK)
	{
		HxLOG_Error(DBG_ASSERT, ("bc_sc_CreateTask : error\n");
		return ERR_FAIL;
	}

	return bc_sc_BcInit();
}
