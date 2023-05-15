#ifndef __DI_HDD_H__
#define __DI_HDD_H__

#include "di_err.h"
#include "di_fs.h" /* DI_FS_StorDevIdx_t 姥繕端研 紫遂馬奄 是背 琶推 */

#define USBHDD_VENDOR_LEN		50 /* 須舌 USB HDD 舛左研 穿含馬奄 是背 琶推 */
#define USBHDD_PRODUCT_LEN		50
#define USBHDD_SERIALNUM_LEN		50

#define DI_HDD_ATA_SERIAL_NUM_LEN		(20)
#define DI_HDD_ATA_FIRMWARE_NUM_LEN		(8)
#define DI_HDD_ATA_MODEL_NUM_LEN		(40)

#define HDD_ENCRYPTION_KEY_LEN	32

#define DEV_NAME_LEN	3

typedef enum
{
	DST_HDD_NOT_DETECTED, 
	DST_FAILED_TRY_AGAIN_LATER,
	DST_IS_ON_GOING,
	DST_COMPLETED_OK,
	DST_COMPLETED_FATAL_ERROR,
	DST_COMPLETED_TEST_ELEMENT_FAILED,
	DST_COMPLETED_ELECTRICAL_ELEMENT_FAILED,
	DST_COMPLETED_SERVO_ELEMENT_FAILED,
	DST_COMPLETED_READ_ELEMENT_FAILED,
	DST_COMPLETED_HANDLING_DAMAGE,
	DST_COMPLETED_UNKNOWN
}HDD_DST_RESULT;

typedef enum
{
	DI_HDD_Event_Pwroff,
	DI_HDD_Event_Pwron,
	DI_HDD_Event_PendingSector
}DI_HDD_Event_t;

typedef enum
{
	DI_HDD_PWR_OFF,
	DI_HDD_PWR_ON
}DI_HDD_PWR;

enum
{
	SATA_VIRTUAL_PORT_DUMMY = 0,
	SATA_VIRTUAL_PORT_INTERNAL = 1,
	SATA_VIRTUAL_PORT_DETACHABLE,
	SATA_VIRTUAL_PORT_ESATA
};

typedef enum {
	DI_HDD_PORT_NUM_1 = SATA_VIRTUAL_PORT_INTERNAL,
	DI_HDD_PORT_NUM_2 = SATA_VIRTUAL_PORT_DETACHABLE,
	DI_HDD_PORT_NUM_3 = SATA_VIRTUAL_PORT_ESATA,
	DI_HDD_PORT_NUM_MAX
} DI_HDD_PORT_NUM_E;

typedef struct hdd_info_s
{	
	unsigned long devCount;	
	char devName[DEV_NAME_LEN+1];
	int portnum; /* virtual port number */
}hdd_info_t;

typedef struct
{
	HUINT8 aucUsbHddVendor[USBHDD_VENDOR_LEN];
	HUINT8 aucUsbHddProduct[USBHDD_PRODUCT_LEN];
	HUINT8 aucUsbHddSerialNum[USBHDD_SERIALNUM_LEN];
} DI_HDD_USBHDD_INFO_t;

typedef struct
{
	HUINT8	aucModelName[DI_HDD_ATA_MODEL_NUM_LEN+1];
	HUINT8	aucFirmwareVer[DI_HDD_ATA_FIRMWARE_NUM_LEN+1];
	HUINT8	aucSerialNo[DI_HDD_ATA_SERIAL_NUM_LEN+1];
	HUINT32	ulSizeMB;
} DI_HDD_INFO_t;

typedef void	(*pfnDI_HDD_DSTNotify)(HDD_DST_RESULT dstResult, int progressStatus);
typedef DI_ERR_CODE	(*pfnDI_HDD_NotifyFUNC)(DI_HDD_Event_t etHddEventType,void *pucHddInfo);
typedef void	(*pfnDI_HDD_ErrNotify)(DI_HDD_Event_t etHddEventType, const char *devname, int devname_len, unsigned long long sector);
void DI_HDD_RegisterCallback(pfnDI_HDD_NotifyFUNC pfnNotify);
void DI_HDD_RegisterErrHandleCallback(pfnDI_HDD_ErrNotify pfnNotify);
DI_ERR_CODE DI_HDD_PWR_Ctrl(DI_HDD_PWR OnOff);
void DI_HDD_RegisterDstCallback(pfnDI_HDD_DSTNotify pfnDstNotify);
void DI_HDD_DstStart(void);
DI_ERR_CODE DI_HDD_GetEncryptionKey(DI_HDD_PORT_NUM_E portnum, char *pKey);

/* Add new DI API for mutiple HDD */
DI_ERR_CODE DI_HDD_ControlPower(DI_HDD_PORT_NUM_E portnum, DI_HDD_PWR on);
void DI_HDD_StartDst(DI_HDD_PORT_NUM_E portnum);

DI_ERR_CODE DI_HDD_GetInfoUsbHdd(DI_FS_StorDevIdx_t eDevIdx, DI_HDD_USBHDD_INFO_t *pstUsbHddInfo);
DI_ERR_CODE DI_HDD_RestorePendingSector(const char *devname, unsigned long long sector);
DI_ERR_CODE DI_HDD_Init(void);
DI_ERR_CODE DI_HDD_GetHddInfo(DI_HDD_PORT_NUM_E ePortnum, DI_HDD_INFO_t *pstHddInfo);
#endif
