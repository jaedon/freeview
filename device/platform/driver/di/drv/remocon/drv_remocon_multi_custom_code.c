/*******************************************************************/
/************************* File Description *****************************/
/*******************************************************************/
// File Name: drv_remocon_multi_custom_code.c
// Original Author: Yoo Tae Hyun
// File Description: IR Multi Custom Code
// Module: InfraRed
// Remarks:

/*******************************************************************/
/* Copyright (c) 2007 HUMAX Co., Ltd. 							   */
/* All rights reserved.											   */
/*******************************************************************/

/*******************************************************************/
/**************************** Header Files ****************************/
/*******************************************************************/

/* include common utils first! */
#include "linden_trace.h"
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#endif
#include "vkernel.h"
#include "taskdef.h"

#include "bstd.h"

/* include drv headers */
#include "drv_key.h"
#include "drv_remocon_multi_custom_code.h"
#include "drv_micom.h"

/* include di headers */
#include "di_err.h"
#include "di_nvram.h"
#include "drv_nvram.h"


/*******************************************************************/
/****************************** define *******************************/
/*******************************************************************/

#define DEBUG_MODULE	TRACE_MODULE_DRV_REMOCON


/*******************************************************************/
/****************************** typedef ******************************/
/*******************************************************************/
/* System Dependent �� �κ� ��, System���� ����ϴ� message type�� define �� */
typedef IR_MSG IR_MULTIC_CUSTOM_MSG;

/*******************************************************************/
/************************ global variables *****************************/
/*******************************************************************/

/*******************************************************************/
/************************ static variables ******************************/
/*******************************************************************/
static IRINFO s_stIRInfo;
#if !defined(CONFIG_DIRECTFB)
static IR_MULTICUSTOMCODE_NOTIFY_APP_FUNC s_pfnIRMultiCustomCodeNotifyCallBack = NULL;
#else
static pfnDI_KEY_Notify s_pfnIRMultiCustomCodeNotifyCallBack = NULL;
#endif

#if defined(CONFIG_DIRECTFB)
/* These values should be same with DIKS_CUSTOM7, DISK_CUSTOM8, DISK_CUSTOM9 of IrAdvanCodeMap array in bcmnexus_ir_keycodes_remotea.h */
static HUINT32 s_IRCodeForMultiCustomCode[MSG_MULTICUSTOMCODE_MAX] = { 0xCCD7, 0xCCD8, 0xCCD9 };
#endif

/*******************************************************************/
/************************ static functions ******************************/
/*******************************************************************/
static void DRV_REMOCON_ChangeStateTimer_Callback(unsigned long TimerId, void *params);
static IR_MULTI_CUSTOM_OPERATION_STATE DRV_REMOCON_MultiCusomCode_NormalState(pIRINFO pIRInfo);
static IR_MULTI_CUSTOM_OPERATION_STATE DRV_REMOCON_MultiCusomCode_StartChangeCustomCodeState(pIRINFO pIRInfo);
static IR_MULTI_CUSTOM_OPERATION_STATE DRV_REMOCON_MultiCusomCode_ChangeCustomCodeState(pIRINFO pIRInfo);
static IR_MULTI_CUSTOM_RET_CODE DRV_REMOCON_MultiCustomCode_ReadCustomCodeFromE2P(HUINT16 *pusCustomCode);
static IR_MULTI_CUSTOM_RET_CODE DRV_REMOCON_MultiCustomCode_WriteCustomCodeToE2P(HUINT16 usCustomCode);
static HUINT32 DRV_REMOCON_MultiCustomCode_GetIRTaskQueueID(void);

/*******************************************************************/
/*********************** Function Description***************************/
/*******************************************************************/
// Function Name: DRV_REMOCON_MultiCustomCode_Init
// Function Description:
// Parameter:
// Return Value:
/*******************************************************************/
void DRV_REMOCON_MultiCustomCode_Init(void)
{
	IR_MULTI_CUSTOM_RET_CODE eRet = IR_MULTI_CUSTOM_RET_CODE_NO_ERR;
	DRV_Error	nDrvErr = DRV_OK;

	eRet = DRV_REMOCON_MultiCustomCode_ReadCustomCodeFromE2P(&s_stIRInfo.usCurrentCustomCode);

#if defined(CONFIG_PRODUCT_MULTI_PROCESS)
	s_stIRInfo.usCurrentCustomCode=1;
#else
	if(eRet != IR_MULTI_CUSTOM_RET_CODE_NO_ERR)
	{
		s_stIRInfo.usCurrentCustomCode = IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT;
		DRV_REMOCON_MultiCustomCode_WriteCustomCodeToE2P(s_stIRInfo.usCurrentCustomCode);
		PrintDebug("[DRV_REMOCON_MultiCustomCode_Init] ReadCustomCode Error so Write Default Custom Code=0x%x\n", s_stIRInfo.usCurrentCustomCode);
	}
#endif
//	PrintDebug("[DRV_REMOCON_MultiCustomCode_Init] Read Custom Code=0x%x\n", s_stIRInfo.usCurrentCustomCode);

	switch(s_stIRInfo.usCurrentCustomCode)
	{
		case IR_MULTI_CUSTOM_CODE_1 :
			 s_stIRInfo.ulCurrentMode = IR_MULTI_CUSTOM_MODE_1;
			 break;
		case IR_MULTI_CUSTOM_CODE_2 :
			 s_stIRInfo.ulCurrentMode = IR_MULTI_CUSTOM_MODE_2;
			 break;
		case IR_MULTI_CUSTOM_CODE_3  :
			 s_stIRInfo.ulCurrentMode = IR_MULTI_CUSTOM_MODE_3;
			 break;
		case IR_MULTI_CUSTOM_CODE_4 :
			 s_stIRInfo.ulCurrentMode = IR_MULTI_CUSTOM_MODE_4;
			 break;
		case IR_MULTI_CUSTOM_CODE_5 :
			 s_stIRInfo.ulCurrentMode = IR_MULTI_CUSTOM_MODE_5;
			 break;
		case IR_MULTI_CUSTOM_CODE_6 :
			 s_stIRInfo.ulCurrentMode = IR_MULTI_CUSTOM_MODE_6;
			 break;
		default :
			 s_stIRInfo.usCurrentCustomCode = IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT;
			 s_stIRInfo.ulCurrentMode = IR_MULTI_CUSTOM_MODE_1;
			 DRV_REMOCON_MultiCustomCode_WriteCustomCodeToE2P(s_stIRInfo.usCurrentCustomCode);
			break;
	}

	s_stIRInfo.usPreviousCustomCode = s_stIRInfo.usCurrentCustomCode;
	s_stIRInfo.usDecodedCustomCode = s_stIRInfo.usCurrentCustomCode;
	s_stIRInfo.eOperationState = IR_MULTI_CUSTOM_OPERATION_IS_NORMAL;
	s_stIRInfo.ulIRChangeStateTimerID = 0;
	s_stIRInfo.ulQueueID = DRV_REMOCON_MultiCustomCode_GetIRTaskQueueID();

#if !defined(CONFIG_PRODUCT_MULTI_PROCESS)
	nDrvErr = DRV_MICOM_SetCustomCode(s_stIRInfo.usDecodedCustomCode);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DRV_MICOM_SetCustomCode() \n", nDrvErr);
		return;
	}
#endif
#if 0
	PrintDebug("====================================================\n");
	PrintDebug("[DRV_REMOCON_MultiCustomCode_Init] CurrentCustomCode=0x%x, PreviousCustomCode=0x%x, DecodedCustomCode=0x%x, CurrentMode=%d\n",
			s_stIRInfo.usCurrentCustomCode, s_stIRInfo.usPreviousCustomCode, s_stIRInfo.usDecodedCustomCode, s_stIRInfo.ulCurrentMode);
	PrintDebug("[DRV_REMOCON_MultiCustomCode_Init] OperationState=0x%x, IRChangeStateTimerID=0x%x, QueueID=0x%x\n",
			s_stIRInfo.eOperationState, s_stIRInfo.ulIRChangeStateTimerID, s_stIRInfo.ulQueueID);
	PrintDebug("====================================================\n");
#endif
}

/*******************************************************************/
/*********************** Function Description***************************/
/*******************************************************************/
// Function Name: DRV_REMOCON_ChangeStateTimer_Callback
// Function Description:
// Parameter:
// Return Value:
/*******************************************************************/
static void DRV_REMOCON_ChangeStateTimer_Callback(unsigned long TimerId, void *params)
{
	HUINT32 ulNextState, ulReturnCode;
	IR_MULTIC_CUSTOM_MSG   stIRMSG;

	BSTD_UNUSED(TimerId);

	ulNextState = *(HUINT32*)params;

	s_stIRInfo.ulIRChangeStateTimerID = 0;

	switch(s_stIRInfo.eOperationState)
	{
		case IR_MULTI_CUSTOM_OPERATION_IS_START_CHANGE_CUSTOM_CODE :
		case IR_MULTI_CUSTOM_OPERATION_IS_CHANGE_CUSTOM_CODE :
			if(ulNextState == IR_MULTI_CUSTOM_OPERATION_IS_NORMAL)
			{
				// Notify Fail Information to Application
				PrintDebug("\n===================================================\n");
				PrintDebug("=DRV_REMOCON_ChangeStateTimer_Callback===================\n");
				PrintDebug("=[Application Notify2]======================================\n");
				PrintDebug("====================================================\n");
				PrintDebug("Changing remote control mode failed. Try agin\n");
				PrintDebug("====================================================\n\n");

				if(s_pfnIRMultiCustomCodeNotifyCallBack)
				{
#if defined(CONFIG_DIRECTFB)
					s_pfnIRMultiCustomCodeNotifyCallBack(DI_RCU_KEY, s_IRCodeForMultiCustomCode[MSG_MULTICUSTOMCODE_CANCEL], DI_KEY_PRESSED);
					s_pfnIRMultiCustomCodeNotifyCallBack(DI_RCU_KEY, s_IRCodeForMultiCustomCode[MSG_MULTICUSTOMCODE_CANCEL], DI_KEY_RELEASED);
#else
					s_pfnIRMultiCustomCodeNotifyCallBack(MSG_MULTICUSTOMCODE_CANCEL, IR_MULTI_CUSTOM_MODE_NONE);
#endif
				}
			}
			break;
		default:
			break;
	}

#if defined(SUPPORT_IR_MULTI_CUSTOM_CODE)
	switch(ulNextState)
	{
		case IR_MULTI_CUSTOM_OPERATION_IS_NORMAL :
		case IR_MULTI_CUSTOM_OPERATION_IS_START_CHANGE_CUSTOM_CODE :
		case IR_MULTI_CUSTOM_OPERATION_IS_CHANGE_CUSTOM_CODE :
			stIRMSG.ulNextState = ulNextState;
			break;
		default :
		  /* Operation Satus�� Normal Operation ���·� ��ȯ�Ѵ�. */
		  stIRMSG.ulNextState = IR_MULTI_CUSTOM_OPERATION_IS_NORMAL;
	      break;
	}

	PrintDebug("====================================================\n");
	PrintDebug("[DRV_REMOCON_ChangeStateTimer_Callback] \n");
	PrintDebug("** ulNextState=0x%x\n", stIRMSG.ulNextState );
	PrintDebug("====================================================\n");

	// Task�� command message ����
	stIRMSG.bMultiCustomMessage = TRUE;
#endif

	ulReturnCode = VK_MSG_Send(s_stIRInfo.ulQueueID, &stIRMSG, sizeof(IR_MULTIC_CUSTOM_MSG));
	if(ulReturnCode != VK_OK)
	{
		//���� �߻��ÿ� ���� ó����?
		PrintError("[DRV_REMOCON_ChangeStateTimer_Callback] VK_MSG_Send error : ulReturnCode is 0x%x\n", ulReturnCode);
	}
}

/*******************************************************************/
/*********************** Function Description***************************/
/*******************************************************************/
// Function Name: DRV_REMOCON_MultiCusomCode_StateMachine
// Function Description:
// Parameter:
// Return Value:
/*******************************************************************/
IR_MULTI_CUSTOM_OPERATION_STATE DRV_REMOCON_MultiCusomCode_StateMachine(pIRINFO pIRInfo)
{
	IR_MULTI_CUSTOM_OPERATION_STATE eOperationResult;

	PrintDebug("====================================================\n");
	PrintDebug("[DRV_REMOCON_MultiCusomCode_StateMachine] \n");
	PrintDebug("** CurrentState=0x%x, CurrentCustomCode=0x%x, CurrentMode=%d\n", pIRInfo->eOperationState, pIRInfo->usCurrentCustomCode, pIRInfo->ulCurrentMode);
	PrintDebug("** usDecodedCustomCode=0x%x , ucDecodedCode=0x%x\n", pIRInfo->usDecodedCustomCode, pIRInfo->ucDecodedCode);
	PrintDebug("** InterimCustomCode=0x%x, SelectKeyCodeStart=0x%x\n",IR_MULTI_CUSTOM_CODE_INTERIM, IR_MULTI_CUSTOM_SELECT_KEY_CODE_START);
	PrintDebug("====================================================\n");

    switch (pIRInfo->eOperationState)
	{
		case IR_MULTI_CUSTOM_OPERATION_IS_NORMAL :
			eOperationResult = DRV_REMOCON_MultiCusomCode_NormalState(pIRInfo);
			break;

	    case IR_MULTI_CUSTOM_OPERATION_IS_START_CHANGE_CUSTOM_CODE :
			eOperationResult = DRV_REMOCON_MultiCusomCode_StartChangeCustomCodeState(pIRInfo);
			break;

		case IR_MULTI_CUSTOM_OPERATION_IS_CHANGE_CUSTOM_CODE :
			eOperationResult = DRV_REMOCON_MultiCusomCode_ChangeCustomCodeState(pIRInfo);
			break;

		default :
			eOperationResult = IR_MULTI_CUSTOM_OPERATION_IS_INVALID;
			break;
    }

    /* Operation �� error�� �߻��ϸ� �������� timer�� ��� kill�ϰ� ���� ���¸� Normal Operation���� ��ȯ�Ѵ�. */
	if(eOperationResult == IR_MULTI_CUSTOM_OPERATION_IS_INVALID)
	{
		// Notify Fail Information to Application
		PrintDebug("\n===================================================\n");
		PrintDebug("=DRV_REMOCON_MultiCusomCode_StateMachine===================\n");
		PrintDebug("=[Application Notify2]======================================\n");
		PrintDebug("====================================================\n");
		PrintDebug("Changing remote control mode failed. Try agin\n");
		PrintDebug("====================================================\n\n");

		if(s_pfnIRMultiCustomCodeNotifyCallBack)
		{
#if defined(CONFIG_DIRECTFB)
			s_pfnIRMultiCustomCodeNotifyCallBack(DI_RCU_KEY, s_IRCodeForMultiCustomCode[MSG_MULTICUSTOMCODE_CANCEL], DI_KEY_PRESSED);
			s_pfnIRMultiCustomCodeNotifyCallBack(DI_RCU_KEY, s_IRCodeForMultiCustomCode[MSG_MULTICUSTOMCODE_CANCEL], DI_KEY_RELEASED);
#else
			s_pfnIRMultiCustomCodeNotifyCallBack(MSG_MULTICUSTOMCODE_CANCEL, IR_MULTI_CUSTOM_MODE_NONE);
#endif
		}

		DRV_REMOCON_MultiCusomCode_ClearState(pIRInfo, IR_MULTI_CUSTOM_OPERATION_IS_NORMAL);
	}

	return eOperationResult;
}

/*******************************************************************/
/*********************** Function Description***************************/
/*******************************************************************/
// Function Name: DRV_REMOCON_MultiCusomCode_NormalState
// Function Description:
// Parameter:
// Return Value:
/*******************************************************************/
static IR_MULTI_CUSTOM_OPERATION_STATE DRV_REMOCON_MultiCusomCode_NormalState(pIRINFO pIRInfo)
{
	HUINT32 ulReturnCode, ulNextState;
	IR_MULTI_CUSTOM_OPERATION_STATE eOperationResult;

	/***************************************************************************************/
    /* <0>&<OK> Key�� ���ÿ� ������ [Current_Custom_Cdoe, 0x70] Key�� ���������� ���� ��µȴ�.         */
    /* Ȥ �������� Current Custom Code�� Mode2�ε� STB�� Curretn Custom Code�� Mode1 �ΰ��� ����    */
    /* �������� STB�� Custom Code�� �ٸ� ��찡 �߻��� ���� ����ؼ� [Mode1-Mode6 Custom Code, 0x70]  */
    /* �� �ԷµǸ� Custom Code ��ȯ �������� �����Ͽ� ������ �����Ѵ�. ex) �������� ���� Custom Code��    */
    /* ���� �ϰ� STB�� ���� Custom Code ������ �Ϸ� �ϴ� ��� STB�� �������� Current Custom Code��       */
    /* �޶��� �������� ����� �� ���� �ȴ�. �̸� �����ϱ� ���ؼ� �Ʒ��� ���� �ڵ���.                   ************/
	/***************************************************************************************/
	if(( (pIRInfo->usDecodedCustomCode == IR_MULTI_CUSTOM_CODE_1) ||
	    (pIRInfo->usDecodedCustomCode == IR_MULTI_CUSTOM_CODE_2) ||
	    (pIRInfo->usDecodedCustomCode == IR_MULTI_CUSTOM_CODE_3) ||
	    (pIRInfo->usDecodedCustomCode == IR_MULTI_CUSTOM_CODE_4) ||
	    (pIRInfo->usDecodedCustomCode == IR_MULTI_CUSTOM_CODE_5) ||
	    (pIRInfo->usDecodedCustomCode == IR_MULTI_CUSTOM_CODE_6) ) 	&&
	  (pIRInfo->ucDecodedCode == IR_MULTI_CUSTOM_SELECT_KEY_CODE_START))
	 {
		/****************************************/
		/* �Էµ� Key�� <0> & <OK> Key �̸�         ****/
		/****************************************/
		/*<0>&<OK> = [Current Custom Code, 0x70]   */
		/****************************************/

		PrintDebug("\n===================================================\n");
		PrintDebug("[DRV_REMOCON_MultiCusomCode_NormalState] Pressed <0>&<OK>, Decoded Custom Code = 0x%x, ucDecodedCode = 0x%x\n",
					pIRInfo->usDecodedCustomCode, pIRInfo->ucDecodedCode);
		PrintDebug("====================================================\n\n");


		/* 1. Custom Code Change Start State �� ���� ��ȯ�Ѵ�.*/
		pIRInfo->eOperationState = IR_MULTI_CUSTOM_OPERATION_IS_START_CHANGE_CUSTOM_CODE;
		eOperationResult = IR_MULTI_CUSTOM_OPERATION_IS_START_CHANGE_CUSTOM_CODE;

		/* 2. Create Change State Timer. 5���Ŀ� State�� IR_MULTI_CUSTOM_OPERATION_IS_NORMAL�� �����ϴ� timer�� �����Ѵ�.
             5�� �ȿ� <Interim_Custom_Code, 0x70> �Է��� ������ Normal ���·� ��ȯ�ϱ� ���� timer�� ����Ѵ�. */
		ulNextState = IR_MULTI_CUSTOM_OPERATION_IS_NORMAL;
		ulReturnCode = VK_TIMER_EventAfter(IR_CHANGE_STATE_TO_INPUT_CUSTOM_CODE_PERIOD, DRV_REMOCON_ChangeStateTimer_Callback,
					   &ulNextState, sizeof(ulNextState),  (unsigned long *)&s_stIRInfo.ulIRChangeStateTimerID) ;
		if(ulReturnCode != VK_OK)
		{
		   /* Custom Code Change ���� */
		   eOperationResult = IR_MULTI_CUSTOM_OPERATION_IS_INVALID;
		   PrintError("[DRV_REMOCON_MultiCusomCode_StateMachine] IR_CHANGE_State_PERIOD Timer Create Fail\n");
		}

	 }
	 else if(pIRInfo->usDecodedCustomCode == IR_MULTI_CUSTOM_CODE_INTERIM)
	 {
		 /*********************************************************/
		 /* Normal State���� Interim Custom Code�� �ԷµǸ� �̸� �����Ѵ�. */
		 /*********************************************************/
		 eOperationResult = IR_MULTI_CUSTOM_OPERATION_IS_IGNORE;
	 }
	 else
	 {
		 /***********************************/
		 /* �Ϲ� Key�� ��� 				*/
		 /***********************************/
		 eOperationResult = IR_MULTI_CUSTOM_OPERATION_IS_NORMAL;
	 }

	return eOperationResult;
}

/*******************************************************************/
/*********************** Function Description***************************/
/*******************************************************************/
// Function Name: DRV_REMOCON_MultiCusomCode_StartChangeCustomCodeState
// Function Description:
// Parameter:
// Return Value:
/*******************************************************************/
static IR_MULTI_CUSTOM_OPERATION_STATE DRV_REMOCON_MultiCusomCode_StartChangeCustomCodeState(pIRINFO pIRInfo)
{
	IR_MULTI_CUSTOM_OPERATION_STATE eOperationResult;
	HUINT32 ulReturnCode, ulNextState;

	if((pIRInfo->usDecodedCustomCode == IR_MULTI_CUSTOM_CODE_INTERIM) &&
	  (pIRInfo->ucDecodedCode == IR_MULTI_CUSTOM_SELECT_KEY_CODE_START))
	{
		/***************************************************************/
		/* �Էµ� Key�� [INTERIM_CUSTOM_CODE, KEY_CODE_START] Key�̸�  ***/
		/***************************************************************/

		/* 1. Normal State�� ��ȯ�ϴ� s_stIRInfo.ulIRChangeStateTimerID�� �����ϰ� �ִٸ� kill�Ѵ�.*/
		if(s_stIRInfo.ulIRChangeStateTimerID)
	{
			VK_TIMER_Cancel(s_stIRInfo.ulIRChangeStateTimerID);
			s_stIRInfo.ulIRChangeStateTimerID = 0;
	}

		/* 2. Change Custom Code State �� ���� ��ȯ�Ѵ�.*/
		pIRInfo->eOperationState = IR_MULTI_CUSTOM_OPERATION_IS_CHANGE_CUSTOM_CODE;
		eOperationResult = IR_MULTI_CUSTOM_OPERATION_IS_CHANGE_CUSTOM_CODE;

		/* 3. 15�� �̳׿� Key �Է��� �ִ��� ������ check�ϴ� timer�� ���� �����Ѵ�. */
		ulNextState = IR_MULTI_CUSTOM_OPERATION_IS_NORMAL;
		ulReturnCode = VK_TIMER_EventAfter(IR_CHANGE_STATE_TO_FINISH_CUSTOM_CODE_PERIOD, DRV_REMOCON_ChangeStateTimer_Callback,
					   &ulNextState, sizeof(ulNextState),  (unsigned long *)&s_stIRInfo.ulIRChangeStateTimerID) ;
		if(ulReturnCode != VK_OK)
		{
		   PrintError("[DRV_REMOCON_MultiCusomCode_StateMachine] IR_CHANGE_STATE_PERIOD Timer Create Fail\n");
		   eOperationResult = IR_MULTI_CUSTOM_OPERATION_IS_INVALID;
		   return eOperationResult;
		}

		/*Application�� ���� ������ Custom Code�� <1> - <6>�� ��ư �� �ϳ��� ���� ���� display�ϵ��� Application�� Notify�� �ش�.*/
		PrintDebug("\n[01;35m===================================================\n");
		PrintDebug("=[Application Notify1]======================================\n");
		PrintDebug("====================================================\n");
		PrintDebug("Remote control mode can be changed now.\n");
		PrintDebug("Press numeric key <1> - <6> to select the mode\n");
		PrintDebug("Current Mode : Mode[%d]\n", pIRInfo->ulCurrentMode);
		PrintDebug("====================================================[00m\n\n");

		if(s_pfnIRMultiCustomCodeNotifyCallBack)
		{
#if defined(CONFIG_DIRECTFB)
			s_pfnIRMultiCustomCodeNotifyCallBack(DI_RCU_KEY, s_IRCodeForMultiCustomCode[MSG_MULTICUSTOMCODE_INPUT], DI_KEY_PRESSED);
			s_pfnIRMultiCustomCodeNotifyCallBack(DI_RCU_KEY, s_IRCodeForMultiCustomCode[MSG_MULTICUSTOMCODE_INPUT], DI_KEY_RELEASED);
#else
			s_pfnIRMultiCustomCodeNotifyCallBack(MSG_MULTICUSTOMCODE_INPUT, pIRInfo->ulCurrentMode);
#endif
		}

	}
	else
	{
		/****************************************************************/
		/* �Էµ� key�� [INTERIM_CUSTOM_CODE, KEY_CODE_START] �� �ƴ� ���  */
		/* Normal State�� ���¸� ��ȯ�Ѵ�.                              */
		/****************************************************************/
		pIRInfo->eOperationState = IR_MULTI_CUSTOM_OPERATION_IS_NORMAL;

		/* Normal State�� ��ȯ�ϴ� s_stIRInfo.ulIRChangeStateTimerID�� �����ϰ� �ִٸ� kill�Ѵ�.*/
		if(s_stIRInfo.ulIRChangeStateTimerID)
		{
			VK_TIMER_Cancel(s_stIRInfo.ulIRChangeStateTimerID);
			s_stIRInfo.ulIRChangeStateTimerID = 0;
		}

		eOperationResult = IR_MULTI_CUSTOM_OPERATION_IS_INVALID;
	}

	return eOperationResult;
	}

/*******************************************************************/
/*********************** Function Description***************************/
/*******************************************************************/
// Function Name: DRV_REMOCON_MultiCusomCode_ChangeCustomCodeState
// Function Description:
// Parameter:
// Return Value:
/*******************************************************************/
static IR_MULTI_CUSTOM_OPERATION_STATE DRV_REMOCON_MultiCusomCode_ChangeCustomCodeState(pIRINFO pIRInfo)
{
	HUINT32 ulTargetMode = 0;
	IR_MULTI_CUSTOM_OPERATION_STATE eOperationResult;
	HUINT16 usTargetCustomCode= 0;
	DRV_Error	nDrvErr = DRV_OK;

	if(pIRInfo->usDecodedCustomCode == IR_MULTI_CUSTOM_CODE_INTERIM)
	{
		switch(pIRInfo->ucDecodedCode)
		{
			case IR_MULTI_CUSTOM_SELECT_KEY_CODE_1:
			case IR_MULTI_CUSTOM_SELECT_KEY_CODE_2:
			case IR_MULTI_CUSTOM_SELECT_KEY_CODE_3:
			case IR_MULTI_CUSTOM_SELECT_KEY_CODE_4:
			case IR_MULTI_CUSTOM_SELECT_KEY_CODE_5:
			case IR_MULTI_CUSTOM_SELECT_KEY_CODE_6:

				/***********************************/
				/* �Էµ� Key�� <1> - <6> �� ���  */
				/***********************************/

				/* 1. s_stIRInfo.ulIRChangeStateTimerID�� �����ϰ� �ִٸ� kill�Ѵ�.*/
				if(s_stIRInfo.ulIRChangeStateTimerID)
				{
					VK_TIMER_Cancel(s_stIRInfo.ulIRChangeStateTimerID);
					s_stIRInfo.ulIRChangeStateTimerID = 0;
				}

				/* 2. Custom Code�� ������ ���� ���� Custom Code�� �����Ѵ�. */
				switch(pIRInfo->ucDecodedCode)
				{
					case IR_MULTI_CUSTOM_SELECT_KEY_CODE_1:
						 usTargetCustomCode = IR_MULTI_CUSTOM_CODE_1;
						 ulTargetMode = IR_MULTI_CUSTOM_MODE_1;
						 break;
					case IR_MULTI_CUSTOM_SELECT_KEY_CODE_2:
						 usTargetCustomCode = IR_MULTI_CUSTOM_CODE_2;
						 ulTargetMode = IR_MULTI_CUSTOM_MODE_2;
						 break;
					case IR_MULTI_CUSTOM_SELECT_KEY_CODE_3:
						 usTargetCustomCode = IR_MULTI_CUSTOM_CODE_3;
						 ulTargetMode = IR_MULTI_CUSTOM_MODE_3;
						 break;
					case IR_MULTI_CUSTOM_SELECT_KEY_CODE_4:
						 usTargetCustomCode = IR_MULTI_CUSTOM_CODE_4;
						 ulTargetMode = IR_MULTI_CUSTOM_MODE_4;
						 break;
					case IR_MULTI_CUSTOM_SELECT_KEY_CODE_5:
						 usTargetCustomCode = IR_MULTI_CUSTOM_CODE_5;
						 ulTargetMode = IR_MULTI_CUSTOM_MODE_5;
						 break;
					case IR_MULTI_CUSTOM_SELECT_KEY_CODE_6:
						 usTargetCustomCode = IR_MULTI_CUSTOM_CODE_6;
						 ulTargetMode = IR_MULTI_CUSTOM_MODE_6;
						 break;
				}

				if(DRV_REMOCON_MultiCustomCode_WriteCustomCodeToE2P(usTargetCustomCode) != IR_MULTI_CUSTOM_RET_CODE_NO_ERR)
				{
					PrintError("[DRV_REMOCON_MultiCusomCode_ChangeCustomCodeState] Update is failed\n");
					eOperationResult = IR_MULTI_CUSTOM_OPERATION_IS_INVALID;
					return eOperationResult;
				}

				nDrvErr = DRV_MICOM_SetCustomCode(usTargetCustomCode);
				if (nDrvErr != DRV_OK)
				{
					PrintError("Error(0x%x) In DRV_MICOM_SetCustomCode() \n", nDrvErr);
					return	DRV_ERR;
				}

				/* 3. ������ Custom Code�� Previous Custom Code�� �����Ѵ�. */
				pIRInfo->usPreviousCustomCode = pIRInfo->usCurrentCustomCode;
				pIRInfo->usCurrentCustomCode = usTargetCustomCode;
				pIRInfo->ulCurrentMode = ulTargetMode;

				PrintDebug("====================================================\n");
				PrintDebug("[DRV_REMOCON_MultiCusomCode_StateMachine] Changed Custom Code = 0x%x\n", pIRInfo->usCurrentCustomCode);
				PrintDebug("[DRV_REMOCON_MultiCusomCode_StateMachine] Previous Custom Code = 0x%x\n", pIRInfo->usPreviousCustomCode);
				PrintDebug("====================================================\n");

				/* Custom Code ������ ���������� ����Ǿ����� Application�� Notify ���ش�. */
				PrintDebug("\n===================================================\n");
				PrintDebug("=[Application Notify3]======================================\n");
				PrintDebug("====================================================\n");
				PrintDebug("Remote control mode has changed to mode[%d]\n", pIRInfo->ulCurrentMode);
				PrintDebug("====================================================\n\n");

				if(s_pfnIRMultiCustomCodeNotifyCallBack)
				{
#if defined(CONFIG_DIRECTFB)
					s_pfnIRMultiCustomCodeNotifyCallBack(DI_RCU_KEY, s_IRCodeForMultiCustomCode[MSG_MULTICUSTOMCODE_SUCCESS], DI_KEY_PRESSED);
					s_pfnIRMultiCustomCodeNotifyCallBack(DI_RCU_KEY, s_IRCodeForMultiCustomCode[MSG_MULTICUSTOMCODE_SUCCESS], DI_KEY_RELEASED);
#else
					s_pfnIRMultiCustomCodeNotifyCallBack(MSG_MULTICUSTOMCODE_SUCCESS, pIRInfo->ulCurrentMode);
#endif
				}

				/* 4. State�� IR_MULTI_CUSTOM_OPERATION_IS_NORMAL �� �����Ѵ�. */
				pIRInfo->eOperationState = IR_MULTI_CUSTOM_OPERATION_IS_NORMAL;
				eOperationResult =IR_MULTI_CUSTOM_OPERATION_IS_CHANGE_CUSTOM_CODE;
				break;

			case IR_MULTI_CUSTOM_SELECT_KEY_CODE_START:
				/**************************************************************************************/
				/* �Էµ� Key�� [INTERIM_CUSTOM_CODE, KEY_CODE_START] Key�̸�                       ***/
				/**************************************************************************************/
				/* ���� MultiCustom Code�� ������ ������������ [INTERIM_CUSTOM_CODE, KEY_CODE_START] Key��  */
                /* �� �� �߻��ϳ� Ȥ�� ���� �ϴ� �Ʒ� �ڵ�� �־� �д�.                             */
				/**************************************************************************************/
				eOperationResult =IR_MULTI_CUSTOM_OPERATION_IS_CHANGE_CUSTOM_CODE;
				break;

			default :
				/* �Էµ� Key�� <1> - <6> / [INTERIM_CUSTOM_CODE, KEY_CODE_START]��  �ƴ� ��� */
				eOperationResult = IR_MULTI_CUSTOM_OPERATION_IS_INVALID;
				break;
		}
	}
	else
	{
		/* �Էµ� Key�� [INTERIM_CUSTOM_CODE, KEY_CODE] �� �ƴ� ��� */
		eOperationResult = IR_MULTI_CUSTOM_OPERATION_IS_INVALID;
	}

	return eOperationResult;
}

/*******************************************************************/
/*********************** Function Description***************************/
/*******************************************************************/
// Function Name: DRV_REMOCON_MultiCusomCode_ClearState
// Function Description:
// Parameter:
// Return Value:
/*******************************************************************/
void DRV_REMOCON_MultiCusomCode_ClearState(pIRINFO pIRInfo, IR_MULTI_CUSTOM_OPERATION_STATE eState)
{
	PrintDebug("[DRV_REMOCON_MultiCusomCode_ClearState] State is Cleared\n");

	switch(eState)
	{
		case IR_MULTI_CUSTOM_OPERATION_IS_NORMAL :
		case IR_MULTI_CUSTOM_OPERATION_IS_START_CHANGE_CUSTOM_CODE :
		case IR_MULTI_CUSTOM_OPERATION_IS_CHANGE_CUSTOM_CODE :
			pIRInfo->eOperationState = eState;
			break;
		default :
			pIRInfo->eOperationState = IR_MULTI_CUSTOM_OPERATION_IS_NORMAL;
		    break;
	}

	/* Timer�� �����Ѵٸ� Kill �Ѵ�. */
	if(s_stIRInfo.ulIRChangeStateTimerID)
	{
		VK_TIMER_Cancel(s_stIRInfo.ulIRChangeStateTimerID);
		s_stIRInfo.ulIRChangeStateTimerID = 0;
		PrintDebug("==>s_stIRInfo.ulIRChangeStateTimerID is killed\n");
	}
}

/*******************************************************************/
/*********************** Function Description***************************/
/*******************************************************************/
// Function Name: DRV_REMOCON_MultiCusomCode_GetIRInfo
// Function Description:
// Parameter:
// Return Value:
/*******************************************************************/
pIRINFO DRV_REMOCON_MultiCusomCode_GetIRInfo(void)
{
	return &s_stIRInfo;
}

/*******************************************************************/
/*********************** Function Description***************************/
/*******************************************************************/
// Function Name: DRV_REMOCON_MultiCusomCode_GetCurrentCustomCode
// Function Description:
// Parameter:
// Return Value:
/*******************************************************************/
HUINT16 DRV_REMOCON_MultiCusomCode_GetCurrentCustomCode(void)
{
	return s_stIRInfo.usCurrentCustomCode;
}

/*******************************************************************/
/*********************** Function Description***************************/
/*******************************************************************/
// Function Name: DRV_REMOCON_MultiCusomCode_IsValidKey
// Function Description:
// Parameter:
// Return Value:
/*******************************************************************/
HUINT8 DRV_REMOCON_MultiCusomCode_IsValidKey(HUINT16 usCustomCode, HUINT16 usKeyCode)
{
	/* �Էµ� �������� Custom Code�� ������ �ڵ嵵 �ƴϰ�, Multi Custom Code ������ ���� �ڵ嵵 �ƴϸ� */
	if ((usCustomCode != s_stIRInfo.usCurrentCustomCode) && (usCustomCode != IR_MULTI_CUSTOM_CODE_INTERIM))
	{
		/***************************************************************************************/
		/* <0>&<OK> Key�� ���ÿ� ������ [Current_Custom_Cdoe, 0x70] Key�� ���������� ���� ��µȴ�.         */
		/* Ȥ �������� Current Custom Code�� Mode2�ε� STB�� Curretn Custom Code�� Mode1 �ΰ��� ����    */
		/* �������� STB�� Custom Code�� �ٸ� ��찡 �߻��� ���� ����ؼ� [Mode1-Mode6 Custom Code, 0x70]  */
		/* �� �ԷµǸ� Custom Code ��ȯ �������� �����Ͽ� ������ �����Ѵ�. ex) �������� ���� Custom Code��    */
		/* ���� �ϰ� STB�� ���� Custom Code ������ �Ϸ� �ϴ� ��� STB�� �������� Current Custom Code��       */
		/* �޶��� �������� ����� �� ���� �ȴ�. �̸� �����ϱ� ���ؼ� �Ʒ��� ���� �ڵ���.	    */
		/***************************************************************************************/
		if(( (usCustomCode == IR_MULTI_CUSTOM_CODE_1) ||
			(usCustomCode == IR_MULTI_CUSTOM_CODE_2) ||
			(usCustomCode == IR_MULTI_CUSTOM_CODE_3) ||
			(usCustomCode == IR_MULTI_CUSTOM_CODE_4) ||
			(usCustomCode == IR_MULTI_CUSTOM_CODE_5) ||
			(usCustomCode == IR_MULTI_CUSTOM_CODE_6) ) &&
			(usKeyCode == IR_MULTI_CUSTOM_SELECT_KEY_CODE_START))
		{
			PrintDebug("[DRV_REMOCON_MultiCusomCode_IsValidKey] Key Code Start is pressed to change custom code\n");
			return IR_MULTI_CUSTOM_CODE_VALID_KEY;
		}
		else
		{
			PrintDebug("[DRV_REMOCON_MultiCusomCode_IsValidKey] Invalid Input Custom Code=0x%x, Current Custom Code=0x%x\n", usCustomCode, s_stIRInfo.usCurrentCustomCode);
			return IR_MULTI_CUSTOM_CODE_INVALID_KEY;
		}
	}
	else
	{
	    /* �Էµ� �������� Custom Code�� ������ �ڵ��̰ų�, Multi Custom Code ������ ���� Interim �ڵ��̸� */
		return IR_MULTI_CUSTOM_CODE_VALID_KEY;
	}
}

/*******************************************************************/
/*********************** Function Description***************************/
/*******************************************************************/
// Function Name: DRV_REMOCON_MultiCustomCode_ReadCustomCodeFromE2P
// Function Description:
// Parameter:
// Return Value:
/*******************************************************************/
static IR_MULTI_CUSTOM_RET_CODE DRV_REMOCON_MultiCustomCode_ReadCustomCodeFromE2P(HUINT16 *pusCustomCode)
{
	DI_ERR_CODE eRet = DI_ERR_OK;

	/****************************************************************/
	/* Syste Dependent �� �κ� ��. System���� ����ϴ� NVRAM API�� ����Ѵ�. */
	/****************************************************************/

	if(pusCustomCode == NULL)
	{
		PrintError("[DRV_REMOCON_MultiCustomCode_ReadCustomCodeFromE2P] Invalid pusCustomCode\n");
		return IR_MULTI_CUSTOM_RET_CODE_ERR;
	}

	eRet = DRV_NVRAM_GetField(DI_NVRAM_FIELD_RCU_CUSTOM_CODE, 0, (HUINT16 *)pusCustomCode, sizeof(*pusCustomCode));
	if(eRet != DI_ERR_OK)
	{
		PrintError("[DRV_REMOCON_MultiCustomCode_ReadCustomCodeFromE2P] Read NVRAM_IR_CUSTOM_CODE Error\n");
		return IR_MULTI_CUSTOM_RET_CODE_ERR;
	}
	return IR_MULTI_CUSTOM_RET_CODE_NO_ERR;
}

/*******************************************************************/
/*********************** Function Description***************************/
/*******************************************************************/
// Function Name: DRV_REMOCON_MultiCustomCode_WriteCustomCodeToE2P
// Function Description:
// Parameter:
// Return Value:
/*******************************************************************/
static IR_MULTI_CUSTOM_RET_CODE DRV_REMOCON_MultiCustomCode_WriteCustomCodeToE2P(HUINT16 usCustomCode)
{
	DI_ERR_CODE eRet = DI_ERR_OK;

	eRet = DRV_NVRAM_SetField(DI_NVRAM_FIELD_RCU_CUSTOM_CODE, 0, (HUINT16 *)&usCustomCode, sizeof(usCustomCode));

	if(eRet != DI_ERR_OK)
	{
		PrintError("[DRV_REMOCON_MultiCustomCode_WriteCustomCodeToE2P] Write NVRAM_IR_CUSTOM_CODE Error\n");
		return IR_MULTI_CUSTOM_RET_CODE_ERR;
	}

	return IR_MULTI_CUSTOM_RET_CODE_NO_ERR;
}
/*******************************************************************/
/*********************** Function Description***************************/
/*******************************************************************/
// Function Name: DRV_REMOCON_MultiCustomCode_WriteDefaultCustomCodeToE2P
// Function Description:
// Parameter:
// Return Value:
/*******************************************************************/
IR_MULTI_CUSTOM_RET_CODE DRV_REMOCON_MultiCustomCode_WriteDefaultCustomCodeToE2P(void)
{
	s_stIRInfo.usCurrentCustomCode = IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT;
	DRV_REMOCON_MultiCustomCode_WriteCustomCodeToE2P(s_stIRInfo.usCurrentCustomCode);
	PrintDebug("[DRV_REMOCON_MultiCustomCode_WriteDefaultCustomCodeToE2P] Write Default Custom Code=0x%x\n", s_stIRInfo.usCurrentCustomCode);
	return IR_MULTI_CUSTOM_RET_CODE_NO_ERR;
}

/*******************************************************************/
/*********************** Function Description***************************/
/*******************************************************************/
// Function Name: DRV_REMOCON_MultiCustomCode_RegisterCallBack
// Function Description:
// Parameter:
// Return Value:
/*******************************************************************/
#if defined(CONFIG_DIRECTFB)
void DRV_REMOCON_MultiCustomCode_RegisterCallBack(pfnDI_KEY_Notify notifyFunc)
{
	s_pfnIRMultiCustomCodeNotifyCallBack = notifyFunc;
}
#else
void DRV_REMOCON_MultiCustomCode_RegisterCallBack(IR_MULTICUSTOMCODE_NOTIFY_APP_FUNC notifyFunc)
{
	s_pfnIRMultiCustomCodeNotifyCallBack = notifyFunc;
}
#endif

/*******************************************************************/
/*********************** Function Description***************************/
/*******************************************************************/
// Function Name: DRV_REMOCON_MultiCustomCode_GetIRTaskQueueID
// Function Description:
// Parameter:
// Return Value:
/*******************************************************************/
static HUINT32 DRV_REMOCON_MultiCustomCode_GetIRTaskQueueID(void)
{
	HUINT32 ulQueueID;

	/*********************************************************************/
	/* Syste Dependent �� �κ� ��. System���� ����ϴ� IR Task Queue ID�� �����´�. */
	/* �� System���� ���� �� Interface�� ���� �Ʒ� �Լ��� �޶� �� �� ����                     */
	/*********************************************************************/
	ulQueueID = DRV_KEY_GetIRTaskQueueID();

	return ulQueueID;
}

/*********************** End of File ***********************************/


