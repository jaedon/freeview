/* $Header:   //BASE/archives/Group_MHP/Premium_MHP/mw/cas/satsa/satsa_ui/satsa_ui_adapt.h-arc   1.2   Jul 06 2007 09:21:14   chlim  $ */
/******************************************************************************
 * $Workfile:   satsa_ui_adapt.h  $
 * $Modtime:   Jun 28 2007 15:21:50  $
 * Author : ChangHyun Lim(POOH)
 * description : SATSA interface
 *
 * Copyright (c) 2006 by Humax Co., Ltd.
 * All right reserved
 ******************************************************************************/

#ifndef _SATSA_UI_ADAPT_H_
#define _SATSA_UI_ADAPT_H_

/********************************************************************
 Header Files
 ********************************************************************/

#include "htype.h"

/********************************************************************
 Constant/Macro Definition
 ********************************************************************/

typedef enum {
	SMART_CARD_ERROR =0,
	SMART_CARD_IN =1,
	SMART_CARD_MUTED =2,
	SMART_CARD_OUT =4
} AC_SC_STATUS_TYPE;


/********************************************************************
 Structure and Type Definition
 ********************************************************************/
 typedef void (*Satsa_Ui_Adapt_CardStatusListener)(HUINT16 slotid, HUINT16 status);

/********************************************************************
 Global Variable
 ********************************************************************/

/********************************************************************
 Global Function
 ********************************************************************/

HUINT32 MW_Satsa_Ui_Adapt_TransferData (
							HUINT16 slot,
							HUINT8 *sendBuf,
							HUINT16 lenSendBuf,
							HUINT8 *recvBuf,
							HUINT8 *lenRecvBuf,
							HUINT16 recvMaxLength);



int MW_Satsa_Ui_Adapt_getATR(HUINT16 slotId, HUINT8 *atrBuf,HUINT16 *atrLength);

HUINT8 * MW_Satsa_Ui_Adapt_getSCList(void);
HUINT8 MW_Satsa_Ui_Adapt_getSCStatus(void);
void  MW_Satsa_Ui_Adapt_setStatusListener(Satsa_Ui_Adapt_CardStatusListener listener);

void MW_Satsa_Ui_ConvertEvent2Gule(HUINT8 slotId,HUINT8 evtType);

void MW_Satsa_Ui_setSecureStatus(HUINT8 flag);


void MW_Satsa_Ui_Adapt_Init(void);

#endif /* _SATSA_UI_ADAPT_H_ */

