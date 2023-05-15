/**
	@file	pal_cec.c
	@brief	pal_cec.c

	Description: HDMI 동작 처리. 실제 상태를 저장. \n
	Module: PAL /HDMI												\n

	Copyright (c) 2008 HUMAX Co., Ltd.								\n
	All rights reserved.											\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <di_hdmi.h>

#include <get_enum_str.h>
#include <hlib.h>

#include <pal_cec.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/**************************************************************************/
/********************      Typedef            *****************************/
/**************************************************************************/
STATIC PalCecNotifier_t			s_fnCecNotifier;

/**************************************************************************/
/********************      Static Variables      **************************/
/**************************************************************************/

/**************************************************************************/
/********************      Functions       ********************************/
/**************************************************************************/

STATIC void pal_cec_CbEventNotify(void *pvParam)
{
	DI_HDMI_CEC_MSG_t *pstDiCecMsg;
	PalCec_Msg_t 			stCecMsg;

	pstDiCecMsg = (DI_HDMI_CEC_MSG_t *)pvParam;

	HxSTD_memset(&stCecMsg, 0, sizeof(PalCec_Msg_t));

	stCecMsg.initiator = pstDiCecMsg->ucInitAddr;
	stCecMsg.destination = pstDiCecMsg->ucDestAddr;

	switch (pstDiCecMsg->ucMegLength)
	{
		case 0:
			stCecMsg.opcodeValid = FALSE;
			stCecMsg.numOperand = 0;
			break;

		case 1:
			stCecMsg.opcodeValid = TRUE;
			stCecMsg.opcode = pstDiCecMsg->ucMessage[0];

			stCecMsg.numOperand = 0;
			break;

		default:
			stCecMsg.opcodeValid = TRUE;
			stCecMsg.opcode = pstDiCecMsg->ucMessage[0];

			stCecMsg.numOperand = pstDiCecMsg->ucMegLength - 1;

			/* check length */
			if (stCecMsg.numOperand > CEC_MAX_OPERAND_LEN)
			{
				/* too big */
				return;
			}
			else
			{
				stCecMsg.opcodeValid = TRUE;
				stCecMsg.opcode = pstDiCecMsg->ucMessage[0];

				stCecMsg.numOperand = pstDiCecMsg->ucMegLength - 1;
				HxSTD_memcpy(&stCecMsg.operand, &pstDiCecMsg->ucMessage[1], stCecMsg.numOperand);
			}
			break;
	}

	/* callback */
	if(s_fnCecNotifier != NULL)
	{
		(* s_fnCecNotifier)(eCEC_MESSAGE_RCVED, (HUINT32)&stCecMsg);
	}
}

HERROR PAL_CEC_Init(void)
{
	/* init PAL callback */
	s_fnCecNotifier = NULL;

	/* register DI callback */
	DI_HDMI_RegisterEventNotify(DH_EVENT_CEC_MSG_RECEIVED,(DI_HDMI_CALLBACK)pal_cec_CbEventNotify);

	return ERR_OK;
}

HERROR PAL_CEC_RegisterCallback(PalCecNotifier_t fnCecNotifier)
{
	/* null 로 주면 해제 합니다. */
	s_fnCecNotifier = fnCecNotifier;
	return ERR_OK;
}

HERROR PAL_CEC_GetLogicalAddress(HUINT8 *pucLogicalAddr)
{
	HERROR	err = ERR_OK;

	if (pucLogicalAddr == NULL)
	{
		return ERR_FAIL;
	}

	err = DI_HDMI_GetLogicalAddress(pucLogicalAddr);
	if(err != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR PAL_CEC_GetPhysicalAddress(HUINT16 *pusPhysicalAddr)
{
	HERROR						diErr;
	HDMI_RECEIVER_INFO			rcvInfo;
	HDMI_RECEIVER_INFO_OPTION	rcvInfoOption;
	DI_HDMI_AVAILABLE_FORMATS	stCapa;
	HUINT16	phyA,phyB,phyC,phyD;

	rcvInfoOption = IS_CONNECTED | IS_HDMI_DEVICE | IS_AUTHENCATED;
	DI_HDMI_GetState(rcvInfoOption, &rcvInfo);

	if (rcvInfo.bIsConnected == FALSE)
	{
		return ERR_FAIL;
	}

	/* get info */
	diErr = DI_HDMI_GetCapability(&stCapa);
	if(diErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	phyA = (HUINT16)stCapa.sPhysicalAddr.NodeA;
	phyB = (HUINT16)stCapa.sPhysicalAddr.NodeB;
	phyC = (HUINT16)stCapa.sPhysicalAddr.NodeC;
	phyD = (HUINT16)stCapa.sPhysicalAddr.NodeD;

	*pusPhysicalAddr = (phyA << 12) | (phyB << 8) | (phyC << 4) | phyD;

	return ERR_OK;
}

HERROR PAL_CEC_SendCecMessage(PalCec_Msg_t *pstTxMsg)
{
	HERROR	err = ERR_OK;
	DI_HDMI_CEC_MSG_t diMsg;

	if(pstTxMsg->opcodeValid == FALSE)
	{
		return ERR_FAIL;
	}

	HxSTD_memset(&diMsg, 0, sizeof(DI_HDMI_CEC_MSG_t));

	/* set initiator and destination */
	diMsg.ucInitAddr = pstTxMsg->initiator;
	diMsg.ucDestAddr = pstTxMsg->destination;

	/* set opcode */
	if (pstTxMsg->opcodeValid == TRUE)
	{
		diMsg.ucMessage[0] = pstTxMsg->opcode;
		diMsg.ucMegLength = 1;

		if (pstTxMsg->numOperand != 0)
		{
			/* copy operand */
			if(pstTxMsg->numOperand < CEC_MAX_OPERAND_LEN)
			{
				HxSTD_memcpy(&diMsg.ucMessage[1], &pstTxMsg->operand, pstTxMsg->numOperand);
				diMsg.ucMegLength += pstTxMsg->numOperand;
			}
			else
			{
				/* too many operand */
				return ERR_FAIL;
			}
		}
	}
	HxLOG_Print("PAL_CEC_SendCecMessage || DI_HDMI_SendCecMsg\n");
	err = DI_HDMI_SendCecMsg(&diMsg);
	if (err != DI_ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

/*---------------------------------------------------------------------------*/


/*********************** End of File ******************************/
