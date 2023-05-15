/*******************************************************************************
* File name : di_rf4ce_priv.h
* Author : D.K Lee
* description :
*
* Copyright (c)  by Humax Co., Ltd. Since 2007.
* All right reserved
*******************************************************************************/
 
#ifndef _DI_RF4CE_PRIV_H_
#define _DI_RF4CE_PRIV_H_

/*******************************************************************************
* Headers
*******************************************************************************/
#include "htype.h"


/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#if defined(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST)
#define RF4CE_MAX_MSGS_CNTS		500
#define RF4CE_MAX_MSG_SIZE		128
#else
#define RF4CE_MAX_MSGS_CNTS		10
#define RF4CE_MAX_MSG_SIZE		2
#endif

typedef enum
{
	RF4CE_EVT_PARING_RESULT,
	RF4CE_EVT_ZRC_USER_DATA_INCOMING,
	RF4CE_EVT_MAX
} RF4CE_EVENT_e;

typedef struct
{
	pfnRF4CE_EVT_CALLBACK pfnCallback;
} RF4CE_CALLBACK_INFO_t;

typedef struct
{
	HUINT32					ulMsgQID;
	HUINT32					ulTaskID;
	HUINT32					ulSemID;	
	RF4CE_CALLBACK_INFO_t	tCallbackInfo[DI_RF4CE_NOTIFY_EVT_MAX];
} RF4CE_INSTANCE_t;

typedef struct
{
	RF4CE_EVENT_e	eMsg;	
	HUINT32			ulMsgSize;
	unsigned char		aucParam[RF4CE_MAX_MSG_SIZE];
} RF4CE_MSG_t;


/*******************************************************************************
* Global variables and structures
*******************************************************************************/


/*******************************************************************************
* External variables and functions
*******************************************************************************/


/*******************************************************************************
* Static variables and structures
*******************************************************************************/


/*******************************************************************************
* Static function prototypes
*******************************************************************************/


/*******************************************************************************
* function : PutString
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/


#endif /* _DI_RF4CE_PRIV_H_ */


/* end of file */
