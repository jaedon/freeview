#ifndef __DI_RF4CE_H__
#define __DI_RF4CE_H__

#include "htype.h"
#include "di_err.h"

typedef enum
{
	DI_RF4CE_NOTIFY_EVT_PAIRING_SUCCESS=0,
	DI_RF4CE_NOTIFY_EVT_PAIRING_FAILED,
	DI_RF4CE_NOTIFY_EVT_MAX
} DI_RF4CE_NOTIFY_EVT_e;

typedef enum
{
	DI_RF4CE_EVENT_PRESSED = 0,
	DI_RF4CE_EVENT_REPEATED,
	DI_RF4CE_EVENT_RELEASED,
	DI_RF4CE_EVENT_REL_MOTION,
	DI_RF4CE_EVENT_ABS_MOTION,
	DI_RF4CE_EVENT_MAX
} DI_RF4CE_EVENT_e;

typedef struct
{
	DI_RF4CE_EVENT_e eRf4ceEvent;
	HUINT8 ucDecodedCode;    		/* 디코딩된 코드 */
	HUINT32 ulKeyCode;
	HUINT32 ulRepeatCounter;		   /* 리모컨 키 repetition 을 처리 */
	int nXDisplacement;
	int nYDisplacement;
	int nZDisplacement;
} DI_RF4CE_EVENT_MSG_t;

typedef void (*pfnDI_RF4CE_Notify)(DI_RF4CE_EVENT_MSG_t *ptEventData);

typedef   struct
{
	DI_RF4CE_NOTIFY_EVT_e eEvent;    
	void *pvEventData;
} DI_RF4CE_CALLBACK_DATA_t;

typedef void (*pfnRF4CE_EVT_CALLBACK )(DI_RF4CE_CALLBACK_DATA_t *ptEventData);

void DI_RF4CE_RegisterButtonCallback(pfnDI_RF4CE_Notify pfnNotify);
DI_ERR_CODE DI_RF4CE_RegisterEventCallback(DI_RF4CE_NOTIFY_EVT_e eEvent, pfnRF4CE_EVT_CALLBACK pfnCallback);
DI_ERR_CODE DI_RF4CE_SetPairing(HBOOL bPairing,HUINT32 ulTimeout);
DI_ERR_CODE DI_RF4CE_GetPairedDevCount(HUINT32 *pulCnt);
DI_ERR_CODE DI_RF4CE_GetIEEEAddr(HUINT32 ulIndex, HUINT8 *pucValue);

#endif
