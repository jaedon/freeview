/**
 * drv_remocon.h
*/

/**
 * @defgroup		DRV_RC DRV_RC : DRV_RC module
 * @author		Jin-hong Kim
 * @note			DRV_RC APIs
 * @brief			Define broadcom dependent DRV_RC
 * @file			drv_remocon.h
*/

#ifndef __DRV_REMOCON_H__
#define __DRV_REMOCON_H__

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

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
//typedef void (*pfnDRV_REMOCON_Notify)(HINT32 repeat, HUINT32 code);
typedef void (*pfnDRV_REMOCON_Notify)(void);


typedef enum
{
	DRV_IR_FORMAT_NEC = 0,
	DRV_IR_FORMAT_RC5,
	DRV_IR_FORMAT_JAPAN_CO,
}DRV_RCU_FORMAT;

#if (NEXUS_VERSION >= 1502)
#if defined(CONFIG_RCU_INPUTMODE_NEC)
#define IR_MAX_REPEAT_PERIOD		0		//use H/W repeat filtering
#else
#define IR_MAX_REPEAT_PERIOD	  150      // 150ms
#endif
#else //NEXUS_VERSION < 1502
#define IR_MAX_REPEAT_PERIOD	  150      // 150ms
#endif

#if defined(CONFIG_RCU_INPUTMODE_XMP) || defined(CONFIG_RCU_INPUTMODE_MULTI_XMP)
#define DIGITURK_XMP_01_OWNER_CODE					0x1
#define DIGITURK_XMP_01_OWNER_CODE_SUPERVISOR		0x0
#define DIGITURK_XMP_01_TAG_CODE					0x1

#define URC_178200_XMP_01_REG_NUM					0x4480
#define DIGITURK_XMP_01_REG_NUM						0x4429

// data structure of registry packet, based on XMP-01 Version 1.6
typedef struct
{
	HUINT32 RegCode;    // Registry Code
	HUINT8 owner;		// Owner (setup by user) (default = 1)
	HUINT8 checksum;	// Checksum
	HUINT8 tag; 		// Tag from Peripheral
	HUINT8 peripheralId;// 1111b: Peripheral Identification for Basic Registry
	HUINT16 RegNum; 	// Basic Registry Number
} DRV_RCU_XMP_REGISTRY_PACKET;

// data structure of data packet, based on XMP-01 Version 1.6
typedef struct
{
	HUINT32 DataCode;       // Data Code
	HUINT8 fRepeat; 		// Repeat Flag bit
	HUINT8 tag;			// Tag Number
	HUINT8 owner; 			// Owner (setup by user) (default = 1)
	HUINT8 checksum;		// Checksum
	HUINT8 fmb;			// Make / Break flag
	HUINT8 KeyData;        // Key Data
	HUINT8 fCntl;			// Cntl flag
	HUINT8 fShift;			// Shift flag
	HUINT8 fAlt;			// Alt flag
} DRV_RCU_XMP_DATA_PACKET;
#endif

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
DRV_Error DRV_REMOCON_Init(void);
DRV_Error DRV_REMOCON_UnInit(void);
#if defined(CONFIG_PRODUCT_MULTI_PROCESS)
DRV_Error DRV_REMOCON_Join(void);
#endif
HUINT8 DRV_REMOCON_GetEvents(HINT8 *ucRepeatType, HUINT32 *pucKeyCode, HUINT8 *ulQueueNum);
void DRV_REMOCON_RegisterKeyEventCallback(pfnDRV_REMOCON_Notify pfnNotify);
#if defined(CONFIG_RCU_INPUTMODE_XMP) || defined(CONFIG_RCU_INPUTMODE_MULTI_XMP)
DRV_Error DRV_REMOCON_Xmp_GetEvents(DRV_RCU_XMP_REGISTRY_PACKET *pRegPacket, DRV_RCU_XMP_DATA_PACKET *pDataPacket, HUINT8 *ulQueueNum);
#endif
void DRV_REMOCON_RegisterKeyEventCallback2(pfnDRV_REMOCON_Notify pfnNotify);

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
DRV_Error DRV_REMOCON_SetWakeupCode(HUINT8 ucKeyCode, HUINT32 *pulCode);
DRV_Error DRV_REMOCON_ClearWakeupCode(void);
#endif

#endif /* !__DRV_REMOCON_H__ */

