/* $Header:   //BASE/archives/Group_MHP/Premium_MHP/mw/cas/satsa/satsa_sci/satsa_sci_adapt.h-arc   1.4   Jul 28 2007 16:13:18   kimys  $ */
/******************************************************************************
 * $Workfile:   satsa_sci_adapt.h  $
 * $Modtime:   Jul 28 2007 15:55:14  $
 * Author : ChangHyun Lim(POOH)
 * description : SATSA interface
 *
 * Copyright (c) 2006 by Humax Co., Ltd.
 * All right reserved
 ******************************************************************************/

#ifndef _SATSA_SCI_ADAPT_H_
#define _SATSA_SCI_ADAPT_H_

/********************************************************************
 Header Files
 ********************************************************************/

#include "htype.h"
#include "pal_sc.h"
#include "satsa_error.h"
/********************************************************************
 Constant/Macro Definition
 ********************************************************************/
#define MAX_SATSA_NUM_SC		1
#define MAX_VALID_SLOT_NUM		1
//#define SATSA_SC_INVALID_SLOT		-1
#define SATSA_SC_INVALID_SLOT		0xff

// CARD STATUS
#define CARD_OUT				((unsigned char) 0)
#define CARD_IN					((unsigned char) 1)
#define CARD_MUTE				((unsigned char) 2)
// SMC Instance State
#define CARD_INSERTED			((unsigned char) 0)
#define CARD_ATR_RECEIVED		((unsigned char) 1)
#define CARD_VALIDITY_RECEIVED	((unsigned char) 2)
#define CARD_EXTRACTED			((unsigned char) 3)
#define CARD_OTHER_SC_ACTION	((unsigned char) 4)
#define CARD_ACCESS_DENIED		((unsigned char) 5)

/********************************************************************
 Structure and Type Definition
 ********************************************************************/
typedef struct SATSA_SC_MGR_INST_STRUCT
{
	PalSc_Handle_t		hCardHandle;
	PalSc_Callback_t 	pfnCallBack;
	unsigned char	CardStatus; // IN or OUT
	unsigned char	state;		// My Card or Other Card
	unsigned char	atr[33];
	HUINT32			atrLen;
	HUINT8			ucSlotId;
	HUINT8			ucIsCardMarrying;	// sc_mgr와 slot marrying 단계에 있음을 알린다.
	HUINT8			dummy1; 	// for 4 byte align
	HUINT8			dummy2;
} SATSA_SC_MGR_INST;


/********************************************************************
 Global Variable
 ********************************************************************/

/********************************************************************
 Global Function
 ********************************************************************/
HUINT32 MW_Satsa_Sci_Adapt_TransferData (
							HUINT16 slot,
							HUINT8 *sendBuf,
							HUINT16 lenSendBuf,
							HUINT8 *recvBuf,
							HUINT8 *lenRecvBuf,
							HUINT16 recvMaxLength);


int MW_Satsa_Sci_Adapt_getATR(HUINT16 slotId, HUINT8 *atrBuf,HUINT16 *atrLength);

void MW_Satsa_Sci_Adapt_cardStatus(HUINT8 slotId,HUINT16 cardStatus);

HUINT8 * MW_Satsa_Sci_Adapt_getSCList (void);

int MW_Satsa_Sci_Adapt_StartCard( );

#endif /* _SATSA_SCI_ADAPT_H_ */

