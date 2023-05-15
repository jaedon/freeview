/**
 * drv_hotplug.h
*/

#ifndef __DRV_HOTPLUG_H__
#define __DRV_HOTPLUG_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"
#include "drv_err.h"

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
#define HP_EVT_NUM	16

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
typedef enum {
	DRV_HP_HDD=0,
	DRV_HP_USB,
	DRV_HP_USB_STOR,
	DRV_HP_USB_INPUT,
	DRV_HP_USB_WIRELESS,
	DRV_HP_WIRELESS = DRV_HP_USB_WIRELESS,
	DRV_HP_SD,
	DRV_HP_MIC,
	DRV_HP_MAX = HP_EVT_NUM
} DRV_HP_DEV_EVT;

typedef enum {
	DRV_HP_CONN=0,
	DRV_HP_DISCONN,
	DRV_HP_CONN_BY_PWR,
	DRV_HP_DISCONN_BY_PWR
} DRV_HP_CONNECT_EVT;

typedef struct {
	DRV_HP_CONNECT_EVT eConnEvt;
	int len;
	char devpath[1024];
	unsigned int idvendor;
	unsigned int idproduct;
	int portnum;
	int busnum;
} hp_evt_t;

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/

DRV_Error DRV_HOTPLUG_Init(void);
DRV_Error DRV_HOTPLUG_EventRegister(DRV_HP_DEV_EVT HpEvt, unsigned long EventId);
DRV_Error DRV_HOTPLUG_MsgRegister(DRV_HP_DEV_EVT HpEvt, unsigned long MsgId);
DRV_Error DRV_HOTPLUG_ReadSysUsb(void);

#endif /* !__DRV_HOTPLUG_H__ */

