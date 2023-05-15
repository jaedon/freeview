/* $Header:   //BASE/archives/Group_MHP/Premium_MHP/mw/cas/satsa/satsa_ui/satsa_ui_adapt.c-arc   1.3   Jul 06 2007 09:21:02   chlim  $ */
/******************************************************************************
 * $Workfile:   satsa_ui_adapt.c  $
 * $Modtime:   Jul 05 2007 20:17:30  $
 * Author : ChangHyun Lim(POOH)
 * description : SATSA interface
 *
 * Copyright (c) 2006 by Humax Co., Ltd.
 * All right reserved
 ******************************************************************************/

/*****************************************************************************
							include file
******************************************************************************/

#include "satsa_ui_adapt.h"

#include "satsa_sci.h"
#include "satsa_sci_adapt.h"
#include "ac_ca_dgtvi.h"

/**************************************************************************************************/
#define ________SATSA_Ui_Adapt_Golbal_Value________________________________________________________
/**************************************************************************************************/



/**************************************************************************************************/
#define ________SATSA_Ui_adapt_Private_Define_______________________________________________________
/**************************************************************************************************/



/*****************************************************************************
							typedef
******************************************************************************/

/*****************************************************************************
							external variable declaration
******************************************************************************/

static Satsa_Ui_Adapt_CardStatusListener		s_stCardStatusCallback;

static HUINT8 cardStatus;
/*****************************************************************************
							global/static variable declaration
******************************************************************************/

/*****************************************************************************
							static function prototype
******************************************************************************/

/*****************************************************************************
							static function body
******************************************************************************/


/*****************************************************************************
							global function body
******************************************************************************/

HUINT32 MW_Satsa_Ui_Adapt_TransferData (
							HUINT16 slot,
							HUINT8 *sendBuf,
							HUINT16 lenSendBuf,
							HUINT8 *recvBuf,
							HUINT8 *lenRecvBuf,
							HUINT16 recvMaxLength)

{
	return MW_Satsa_Sci_Adapt_TransferData (slot, (unsigned char*)sendBuf, lenSendBuf, (unsigned char*)recvBuf, (unsigned char*)lenRecvBuf,recvMaxLength);
}

int MW_Satsa_Ui_Adapt_getATR(HUINT16 slotId, HUINT8 *atrBuf,HUINT16 *atrLength)
{
	return MW_Satsa_Sci_Adapt_getATR (slotId, atrBuf, atrLength);
}

HUINT8 * MW_Satsa_Ui_Adapt_getSCList(void)
{
	return MW_Satsa_Sci_Adapt_getSCList();
}

HUINT8 MW_Satsa_Ui_Adapt_getSCStatus(void)
{
	return cardStatus;
}

void  MW_Satsa_Ui_Adapt_setStatusListener(Satsa_Ui_Adapt_CardStatusListener listener)
{
	s_stCardStatusCallback = listener;
	return ;
}

void MW_Satsa_Ui_ConvertEvent2Gule(HUINT8 slotId, HUINT8 evtType)
{
	AC_ITCA_SLOTEVT_TYPE slotEvtType = AC_ITCA_SLOTEVT_CARD_ERROR_UNKNOWN;

	switch(evtType)
	{
		case CARD_IN:
			slotEvtType = AC_ITCA_SLOTEVT_CARD_IN;
			cardStatus =SMART_CARD_IN;
			break;
		case CARD_OUT:
			slotEvtType = AC_ITCA_SLOTEVT_CARD_OUT;
			cardStatus =SMART_CARD_OUT;
			break;
		case CARD_MUTE:
			slotEvtType = AC_ITCA_SLOTEVT_CARD_MUTED;
			cardStatus =SMART_CARD_MUTED;
			break;
		case CARD_OTHER_SC_ACTION:
//20060905	TestByPooh : 다른 card와 상태를 맞춘다면 AC_ITCA_SLOTEVT_CARD_OUT으로 올려줘야 되지만
//						test case의 error를 올려줄 것이 없어서 일단 AC_ITCA_SLOTEVT_CARD_ERROR 처리
//			slotEvtType = AC_ITCA_SLOTEVT_CARD_ERROR;
//20060911	TestByPooh : Irdeto와 동일하게 AC_ITCA_SLOTEVT_CARD_ACCESS_DENIED로 에러처리.
//			slotEvtType = AC_ITCA_SLOTEVT_CARD_ACCESS_DENIED;
			slotEvtType = AC_ITCA_SLOTEVT_CARD_ERROR;

			cardStatus =SMART_CARD_ERROR;
			break;
		case CARD_ACCESS_DENIED:
			slotEvtType = AC_ITCA_SLOTEVT_CARD_ACCESS_DENIED;
			cardStatus =SMART_CARD_MUTED;
			break;
		default:
			slotEvtType = AC_ITCA_SLOTEVT_CARD_ERROR_UNKNOWN;
			break;
	}
	HxLOG_Print("[Satsa_Ui_ConvertEvent2Gule] slot event type = %d\n\r",slotEvtType);
	AP_MHP_Satsa_NotifySlotEvent(slotId, slotEvtType);

	if (s_stCardStatusCallback != NULL)
	{
		s_stCardStatusCallback(0,cardStatus);
		HxLOG_Print("[Satsa_Ui_ConvertEvent2Gule] s_stCardStatusCallback != NULL\n\r");
	}

}
//#endif

void MW_Satsa_Ui_setSecureStatus(HUINT8 flag)
{
	SATSA_MSG msg;

	msg.wMsg = MSG_SCI_FROM_UI;
	msg.nMsg = SET_SECURE_STATUS;
	msg.lParam1 = flag;
	msg.lParam2 = 0;
	msg.lParam3 = 0;
	msg.lParam4 = 0;

	VK_MSG_Send(Get_satsaQueueId(), (void *)&msg, sizeof(SATSA_MSG));

}


void MW_Satsa_Ui_Adapt_Init(void)
{

	static int firstInitFlag =0;

	if( firstInitFlag == 0 )
	{
//		Satsa_Sci_Init();
		firstInitFlag = 1;
	}
	HxLOG_Print("[ac_satsa_sc_init] firstInitFlag = %d \r\n",firstInitFlag);

}

