
#ifndef	_DI_BT_H_
#define	_DI_BT_H_

#include "htype.h"
#include "di_err.h"

#define BT_NB_DISC_DEVICES	(20)
#define BT_NB_PAIR_DEVICES 10
#define BT_NB_NAME_LEN 249
#define BT_NB_BDADDR_LEN 6
#define BT_NB_CLASS_LEN 3
#define BT_NB_PINCODE_LEN 16	/* Pin Code (up to 128 bits) MSB is 0 */
#define BT_NB_LINKKEY_LEN 16
#define BT_NB_ROOT_PATH_LEN 500
#define BT_NB_DEVICE_FEATURE_LEN 8

typedef enum
{
	DI_BT_PROFILE_NONE =  0x00000000,	/* None */
	DI_BT_PROFILE_A2DP =  0x00040000,	/* Adavanced Audio Distribution */
	DI_BT_PROFILE_AVRCP = 0x00080000,	/* Audio/Video Remote Control */
	DI_BT_PROFILE_HID =   0x00100000 ,	/* Human Interface Device */
#if defined(CONFIG_BLUETOOTH_BLE)
	DI_BT_PROFILE_BLE =   0x40000000 ,	/* GATT based Service */
#endif
	DI_BT_PROFILE_ALL =   0x1FFFFFFF	/* All Profile */
} DI_BT_PROFILE_e;

typedef enum
{
	DI_BT_NOTIFY_EVT_INSERTED,
	DI_BT_NOTIFY_EVT_EXTRACTED,
	DI_BT_NOTIFY_EVT_ENABLE,
	DI_BT_NOTIFY_EVT_DISABLE,
	DI_BT_NOTIFY_EVT_SCAN_SUCCESS,
	DI_BT_NOTIFY_EVT_SCAN_FAIL,
	DI_BT_NOTIFY_EVT_AUTHEN_SUCCESS,
	DI_BT_NOTIFY_EVT_AUTHEN_FAIL,
	DI_BT_NOTIFY_EVT_CONNECT_SUCCESS,
	DI_BT_NOTIFY_EVT_CONNECT_FAIL,
	DI_BT_NOTIFY_EVT_REMOTE_CMD,
	DI_BT_NOTIFY_EVT_PASSKEY,
	DI_BT_NOTIFY_EVT_MAX
} DI_BT_NOTIFY_EVT_e;

typedef enum
{
	DI_BT_DEV_NOT_PAIRED,		//New discovered Dev
	DI_BT_DEV_CONNECTED,		//In Use
	DI_BT_DEV_DISCONNECTED		//Once paired Dev
} DI_BT_DEV_STATUS_e;

#if defined(CONFIG_BLUETOOTH_BLE)
typedef enum
{
	DI_BT_DEV_TYPE_BREDR = 0x01,
	DI_BT_DEV_TYPE_BLE = 0x02,
	DI_BT_DEV_TYPE_DUMO = 0x03,
} DI_BT_DEV_TYPE_e;
#endif

typedef struct
{
	HCHAR ucName[BT_NB_NAME_LEN];
	HUINT8 ucBdAddr[BT_NB_BDADDR_LEN];
	HUINT8 ucClass[BT_NB_CLASS_LEN];
#if defined(CONFIG_BLUETOOTH_BLE)
	HUINT8 ucBleAddrType;
	HUINT8 ucDevType;
#endif
	DI_BT_DEV_STATUS_e etDevStatus;
/*	DI_BT_PROFILE_e etAvailableProfile; //ToDoDI_BT_PROFILE_e --> HUINT32 */
	HUINT32 etAvailableProfile;
}DI_BT_DEV_INFO_t;

typedef struct
{
	DI_BT_DEV_INFO_t stBtDevList[BT_NB_DISC_DEVICES];
	HUINT32			ulNumOfData;
}DI_BT_DEV_LIST_t;

typedef	struct
{
	DI_BT_NOTIFY_EVT_e	eEvent;
	HUINT32				ulDevId;
	HUINT32				ulNumOfData;
	HUINT8				*pucBdAddr;
	void				*pvEventData;

}DI_BT_CALLBACK_DATA_t;

typedef struct
{
	HUINT32		ulKey;              //Input code
	HBOOL		bPressed;           //Pressed or released
}DI_BT_REMOTE_CMD_INFO_t;

typedef struct
{
	HUINT32		status;
	HUINT8		ucFwVer[50];
	HUINT8		ucBsaVer[50];
}DI_BT_VERSION_t;

typedef void (*pfnBT_EVT_CALLBACK )(void  *pEventData);

DI_ERR_CODE DI_BT_Init(void);
DI_ERR_CODE DI_BT_Enable(HBOOL bEnable);
DI_ERR_CODE DI_BT_Scan(HUINT32 ulServiceMask);
DI_ERR_CODE DI_BT_Connect(HUINT32 ulDevId);
DI_ERR_CODE DI_BT_SendPinCode(HUINT32 ulDevId, HUINT8 *pucPinCode);
DI_ERR_CODE DI_BT_Disconnect(HUINT32 ulDevId);
DI_ERR_CODE DI_BT_Remove(HUINT32 ulDevId);
DI_ERR_CODE DI_BT_GetHostInfo(DI_BT_DEV_INFO_t *stDevInfo);
DI_ERR_CODE DI_BT_SetHostInfo(HUINT32 ulDevId);
DI_ERR_CODE DI_BT_SetPinCode(HUINT8 *pucPinCode);
DI_ERR_CODE DI_BT_GetDeviceList(DI_BT_DEV_LIST_t *pstDevList);
DI_ERR_CODE DI_BT_OpenProfile(HUINT32 ulDevId, DI_BT_PROFILE_e eProfile);/*DI_BT_PROFILE_e--> HUINT32 */
DI_ERR_CODE DI_BT_CloseProfile(HUINT32 ulDevId, DI_BT_PROFILE_e eProfile);/*DI_BT_PROFILE_e--> HUINT32 */
DI_ERR_CODE DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_e eEvt, pfnBT_EVT_CALLBACK pfnCallback);

DI_ERR_CODE DI_BT_Connect2BdAddr(HUINT8 *pucBdAddr);
DI_ERR_CODE DI_BT_SendPinCode2BdAddr(HUINT8 *pucBdAddr, HUINT8 *pucPinCode);
DI_ERR_CODE DI_BT_Disconnect2BdAddr(HUINT8 *pucBdAddr);
DI_ERR_CODE DI_BT_Remove2BdAddr(HUINT8 *pucBdAddr);
DI_ERR_CODE DI_BT_OpenProfile2BdAddr(HUINT8 *pucBdAddr, DI_BT_PROFILE_e eProfile);
DI_ERR_CODE DI_BT_CloseProfile2BdAddr(HUINT8 *pucBdAddr, DI_BT_PROFILE_e eProfile);

DI_ERR_CODE DI_BT_SetBdAddrToNvram(HUINT8 *pucBdAddr);
DI_ERR_CODE DI_BT_GetBdAddrFromNvram(HUINT8 *pucBdAddr);
DI_ERR_CODE DI_BT_ReadVersion(DI_BT_VERSION_t *pstBTVersion);
DI_ERR_CODE DI_BT_SetTraceLevel(int nLevel);

DI_ERR_CODE DI_BT_DelXmlFiles(void);
DI_ERR_CODE DI_BT_SetTestMode(HBOOL bTestMode);
HUINT32 DI_BT_GetPatchramCRC(void);

#if defined(CONFIG_BLUETOOTH_SIG)
DI_ERR_CODE DI_BT_SetDiscoverableMode(HBOOL bDiscoverableMode);
DI_ERR_CODE DI_BT_HID_SendData(HUINT32 ulDevId);
#endif

#endif
/* end of file */


