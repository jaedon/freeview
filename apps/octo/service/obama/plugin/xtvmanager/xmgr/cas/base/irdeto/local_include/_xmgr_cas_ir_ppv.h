#ifndef __XMGR_CAS_IR_IPPV_INT_H__
#define	__XMGR_CAS_IR_PPV_INT_H__

#include <ir_ppv.h>

#define 	IR_PPV_WINDOW_PREVIEW					0x01
#define 	IR_PPV_WINDOW_NOVIEW					0x02
#define 	IR_PPV_WINDOW_PURCHASE					0x04  //purchasable
#define		IR_PPV_WINDOW_NONPURCHASABLE			0x08
#define 	IR_PPV_WINDOW_PPV_EVENT					0x10

#define		MAX_PPV_SECTOR_NUM			4
#define		MAX_PPV_PINCODE_SIZE		4

typedef enum
{
    CAS_IR_SC_RESULT_OK,           /**< The request has been processed
                                      without problem */
    CAS_IR_SC_RESULT_ALREADY_PURCHASED,
                                    /**< It's impossible to buy event because
                                      a user already have rights to watch it */
    CAS_IR_SC_RESULT_CARD_BLOCKED, /**< The card is blocked */
    CAS_IR_SC_RESULT_CARD_DAMAGED, /**< The card is damaged */
    CAS_IR_SC_RESULT_CARD_ERROR,   /**< A generic communication error with
                                      the smartcard */
    CAS_IR_SC_RESULT_CARD_EXPIRED, /**< The card is expired */
    CAS_IR_SC_RESULT_CARD_MUTED,   /**< The card is muted */
    CAS_IR_SC_RESULT_CARD_REMOVED, /**< The card is removed
                                      during the process */
    CAS_IR_SC_RESULT_CONNECTION_ERROR,
                                    /**< The recharge ended with a failure
                                      due to a connection problem
                                      [2nd TI-NOT REQUIRED] */
    CAS_IR_SC_RESULT_CREDIT_LACK,  /**< The credit is not sufficient to
                                      proceed buying the PPV event */
    CAS_IR_SC_RESULT_GENERIC_ERROR,
                                    /**< Generic error */
    CAS_IR_SC_RESULT_GENERIC_TRASACTION_PARAMETER_ERROR,
                                    /**< The recharge ended with a failure
                                      because any of the parameters used in
                                      the transaction were wrong
                                      [2nd TI-NOT REQUIRED] */
    CAS_IR_SC_RESULT_INVALID_SCRATCH,
                                    /**< The recharge ended with a failure
                                      due to a wrong scratch card number
                                      [2nd TI-NOT REQUIRED] */
    CAS_IR_SC_RESULT_MAXIMUM_CREDIT,
                                    /**< The recharge ended with a failure
                                      because the user reached the maximum
                                      credit permitted */
    CAS_IR_SC_RESULT_NO_OFFERS,    /**< No events are offered currently */
    CAS_IR_SC_RESULT_PIN_ERROR,    /**< Given PIN code was wrong */
    CAS_IR_SC_RESULT_PIN_REQUIRED, /**< PIN is required, but there is no
                                      given PIN code */
    CAS_IR_SC_RESULT_PURCHASE_TIME_ENDED,
                                    /**< The purchase time ended while
                                      processing the purchase request */
    CAS_IR_SC_RESULT_SMS_DENIAL,   /**< SMS denied the recharge
                                      [2nd TI-NOT REQUIRED] */
    CAS_IR_SC_RESULT_UNSUPPORTED_FEATURE
                                    /**< The request is not supported */
} XmgrCas_IrScResult_e;

typedef struct tag_CAS_IR_IPPV_CONTROL
{
	HUINT8 	curWindow;
	HUINT16	caEvtId;			//Preview Ca Event Id
	HUINT8	Purchased;
	HUINT8	eitReceived;		//Now Eit Received
} XmgrCas_IrPpvControl_t;

typedef struct tag_CAS_SECTOR_INFO_STRUCT
{
	HUINT8 		sector;
//	HBOOL 		isActive;

	HUINT16 	debit;
	HUINT16 	debit_limit;
	HUINT16		token;   //credit in LPPV, limit-debit in IPPV

	HUINT16		rechargeToken;

//	HUINT8		ppvActivationStatus;
//	PPV_MODE 	ppvMode;
} XmgrCas_IrSectorInfo_t;


typedef struct tag_CAS_IR_IPPV_EIT_INFO_STRUCT
{
	HINT32	evtId;						//event id in eit
	UNIXTIME 	startTime; 					//event start time
	UNIXTIME 	endTime;						//event end time

	HUINT8 		evtName[255];	//event name
	HUINT16 	evtNameSize;					//event name size
	HUINT8 		evtText[255];	//event text(short event desc³»ÀÇ text)
	HUINT16 	evtTextSize;					//event text size

	HUINT8		szSvcName[/*CONFIG_DB_SVC_MAX_NAME_BYTES*/DbSvc_GetNameLength()];
	HUINT16		usLcn;
//	HUINT16		nowEvtId;						//current Event Id
} XmgrCas_IrIppvEitInfo_t;


void		xmgr_cas_IrIppvInitialize();
void		xmgr_cas_IrIppvDestroy();
HUINT8		xmgr_cas_IrIppvSetPreviewEvent(msg_ippv_event_notify_st *pNotify, HUINT8 *pSubNotify);
void		xmgr_cas_IrIppvSetSectorInfo( msg_ippv_sector_reply_st *pstSectorReply );
void		xmgr_cas_IrPppvReceiveBuyReply(msg_ippv_buy_reply_st *pBuyReply);

void		xmgr_cas_IrPpvCopyEITInfo(XmgrCas_IrIppvEitInfo_t *pDest);
void		xmgr_cas_IrPpvCopySectorInfo(XmgrCas_IrSectorInfo_t *pDest, HUINT8 ucSector);
void		xmgr_cas_IrPpvCopyProductIInfo(CAS_IR_PPV_PRODUCT_STRUCT *pDest);

HUINT32		xmgr_cas_IrPpvGetPurchasedProductNum(void);
void		xmgr_cas_IrPpvCopyPurchasedProductInfo(CAS_IR_PPV_PRODUCT_STRUCT *pProduct, HUINT32 nIndex);
void		xmgr_cas_IrPpvReceiveEventListReply(msg_ippv_event_list_reply_st *pNotify, msg_ippv_event_list_reply_event_st *pEvtList);
void		xmgr_cas_IrPpvUnhideAllProducts();
HUINT8		xmgr_cas_IrPpvIsHideProduct(HUINT32 nIndex);
void		xmgr_cas_IrPpvSetHideStatus(HUINT32 nIndex);
HUINT8		xmgr_cas_IrPpvIsHiddenStatusChanged(void);
void		xmgr_cas_IrPpvCommitHiddenStatus(void);
void		xmgr_cas_IrPpvRollbackHiddenStatus(void);
HERROR		xmgr_cas_IrPpvUpdateIPPVInfo(void);

#endif	/*	__XMGR_CAS_IR_PPV_INT_H__	*/


