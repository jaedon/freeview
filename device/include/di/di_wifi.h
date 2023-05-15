
#ifndef	_DI_WIFI_H_
#define	_DI_WIFI_H_

#include "htype.h"
#include "di_err.h"

/* for future use */
#define DI_WIFI_WOWL_NET_PATTERN_MAX_LEN	128
#define DI_WIFI_WOWL_MAX_NET_PATTERNS		4

typedef enum
{
	DI_WIFI_NOTIFY_EVT_INSERTED,
	DI_WIFI_NOTIFY_EVT_EXTRACTED,
	DI_WIFI_NOTIFY_EVT_SCAN_SUCCESS,
	DI_WIFI_NOTIFY_EVT_SCAN_FAIL,
	DI_WIFI_NOTIFY_EVT_AUTHEN_SUCCESS,
	DI_WIFI_NOTIFY_EVT_AUTHEN_FAIL,
	DI_WIFI_NOTIFY_EVT_CONNECT_SUCCESS,
	DI_WIFI_NOTIFY_EVT_CONNECT_FAIL,
	DI_WIFI_NOTIFY_EVT_MAX
} DI_WIFI_NOTIFY_EVT_e;

typedef enum
{
	DI_WIFI_OP_MODE_ALL,
	DI_WIFI_OP_MODE_ADHOC,
	DI_WIFI_OP_MODE_MANAGED,
	DI_WIFI_OP_MODE_MASTER,
	DI_WIFI_OP_MODE_UNKNOWN,
	DI_WIFI_OP_MODE_MAX
} DI_WIFI_OP_MODE_e;

typedef enum
{
	DI_WIFI_SECURE_OPEN,
	DI_WIFI_SECURE_WEP,
	DI_WIFI_SECURE_WPA,
	DI_WIFI_SECURE_WPA2,
	DI_WIFI_SECURE_WPA_WPA2,
	DI_WIFI_SECURE_UNKNOWN,
	DI_WIFI_SECURE_MAX
} DI_WIFI_SECURE_e;

typedef enum
{
	DI_WIFI_AUTHEN_OPEN = 0x0,
	DI_WIFI_AUTHEN_PSK,
	DI_WIFI_AUTHEN_802_1X,
	DI_WIFI_AUTHEN_UNKNOWN,
	DI_WIFI_AUTHEN_MAX
} DI_WIFI_AUTHEN_e;

typedef enum
{
	DI_WIFI_ENCRYPT_NONE,			/**< needs to 10 hex digits shared key */
	DI_WIFI_ENCRYPT_WEP_64,			/*DI_WIFI_ENCRYPT_WEP_64_ASCII*< needs to 10 hex digits shared key */
	DI_WIFI_ENCRYPT_WEP_64HEX,
	DI_WIFI_ENCRYPT_WEP_128,		/*DI_WIFI_ENCRYPT_WEP_128_ASCII*< needs to 26 hex digits shared key */
	DI_WIFI_ENCRYPT_WEP_128HEX,
	DI_WIFI_ENCRYPT_TKIP,
	DI_WIFI_ENCRYPT_AES,
	DI_WIFI_ENCRYPT_TKIP_AES,

	DI_WIFI_ENCRYPT_UNKNOWN,
	DI_WIFI_ENCRYPT_MAX
} DI_WIFI_ENCRYPT_e;

typedef enum
{
	DI_WIFI_WPS_NONE,
	DI_WIFI_WPS_PBC,
	DI_WIFI_WPS_PIN,
	DI_WIFI_WPS_MAX
}DI_WIFI_WPS_e;

typedef enum
{
	DI_WIFI_BAND_2_4G = 0x0,
	DI_WIFI_BAND_5G,
	DI_WIFI_BAND_MAX
}DI_WIFI_BAND_e;

typedef enum
{
	DI_WIFI_ANT_0 = 0x0,
	DI_WIFI_ANT_1,
	DI_WIFI_ANT_2,
	DI_WIFI_ANT_3,
	DI_WIFI_ANT_MAX
}DI_WIFI_ANT_e;

typedef enum
{
	DI_WIFI_PROTOCOL_MODE_B = 0x0,
	DI_WIFI_PROTOCOL_MODE_G,
	DI_WIFI_PROTOCOL_MODE_A,
	DI_WIFI_PROTOCOL_MODE_N,
	DI_WIFI_PROTOCOL_MODE_AC,
	DI_WIFI_PROTOCOL_MODE_MAX
}DI_WIFI_PROTOCOL_MODE_e;

typedef enum
{
	DI_WIFI_BAND_WIDTH_20MHZ = 0x0,
	DI_WIFI_BAND_WIDTH_40MHZ,
	DI_WIFI_BAND_WIDTH_80MHZ,
	DI_WIFI_BAND_WIDTH_MAX
}DI_WIFI_BAND_WIDTH_e;

typedef enum
{
	DI_WIFI_SIDE_BAND_OFF = 0x0,
	DI_WIFI_SIDE_BAND_40MHZ,
	DI_WIFI_SIDE_BAND_80MHZ,
	DI_WIFI_SIDE_BAND_MAX
}DI_WIFI_SIDE_BAND_e;

typedef enum
{
	DI_WIFI_WOWL_EVENT_NONE				= 0x00,		/* Do not wake up on any event. */
	DI_WIFI_WOWL_EVENT_MAGIC_PATTERN	= 0x01,		/* Wake up on magic pattern. */
	DI_WIFI_WOWL_EVENT_NET_PATTERN		= 0x02,		/* Wake up on a special net pattern. */
	DI_WIFI_WOWL_EVENT_DISASSOC_DEAUTH	= 0x04,		/* Wake up on disassociate from AP. */
	DI_WIFI_WOWL_EVENT_RETROGRADE_TSF	= 0x08,		/* Wake up on retrograde tsf. */
	DI_WIFI_WOWL_EVENT_BCN_LOSS			= 0x10,		/* Wake up on loss of beacon. */
} DI_WIFI_WOWL_EVENT_e;

typedef struct
{
	DI_WIFI_BAND_e eBand;
	DI_WIFI_ANT_e eAntenna;
	DI_WIFI_PROTOCOL_MODE_e eProtocolMode;
	HUINT32 ulChannel;
	DI_WIFI_BAND_WIDTH_e eBandWidth;
	DI_WIFI_SIDE_BAND_e eSideBand;
	HUINT32 ulDataRate;
	HINT32 ulTxPowerLevel;
} DI_WIFI_TX_PARAM_t;

typedef struct
{
	DI_WIFI_BAND_e eBand;
	DI_WIFI_ANT_e eAntenna;
	DI_WIFI_PROTOCOL_MODE_e eProtocolMode;
	HUINT32 ulChannel;
	DI_WIFI_BAND_WIDTH_e eBandWidth;
	DI_WIFI_SIDE_BAND_e eSideBand;
	HUINT32 ulDataRate;
} DI_WIFI_RX_PARAM_t;

typedef struct
{
	HUINT8 				aucESSID[33];	/*String*/
	HUINT32				ulChannel;
	HUINT32				ulFreq;			/* Mhz*/
	HBOOL				bWPS;
	DI_WIFI_OP_MODE_e	eOpMode;
	HUINT8 				aucMAC[6];		/*HEX*/
	HINT32				nWaveStrength;
	HINT32				nBitRate;		/* nBitRate XX Kb/s */
	DI_WIFI_SECURE_e	eSecureType;
	DI_WIFI_AUTHEN_e	eAuthenType;
	DI_WIFI_ENCRYPT_e 	eEncryptType;
	HUINT8 				aucKeyIndex;		/* WEP KeyIndex */
	HUINT8 				aucId[32];		/* ID */
	HUINT8 				aucKey[128];	/* Key or Password */
} DI_WIFI_INFO_t;

typedef struct
{
	HUINT32		iAntCount;
	HINT32		uiRssi;
	HINT32		uiAntRssi[DI_WIFI_ANT_MAX];
} DI_WIFI_RSSI_INFO_t;

typedef struct
{
	HUINT8 		aucDrvName[16];		/* Driver name string */
	HUINT8		aucDrvVer[16];		/* Driver version string */
	HUINT8		aucFwVer[16];		/* Driver version string */
	HUINT32 	ulWifiNvramCrc;		/* CRC32 value of WIFI nvram */
} DI_WIFI_DRV_INFO_t;

typedef struct
{
	HUINT32	ulOffset;
	HCHAR	acMask[DI_WIFI_WOWL_NET_PATTERN_MAX_LEN/8];
	HCHAR	acValue[DI_WIFI_WOWL_NET_PATTERN_MAX_LEN];
	HUINT8	ucLength;
} DI_WIFI_WOWL_NET_PATTERN_t;

typedef struct
{
	DI_WIFI_WOWL_EVENT_e		eWowlMask;				/* Mask of events to wakeup on. */
	DI_WIFI_WOWL_NET_PATTERN_t	stNetPattern[DI_WIFI_WOWL_MAX_NET_PATTERNS]; /* Net Patterns. (for the future use)*/
	HUINT32						ulBcnLossSec;			/* Number of second of beacon loss. */
} DI_WIFI_WOWL_SETTINGS_t;

typedef	struct
{
	HUINT32					ulDevId;
	DI_WIFI_NOTIFY_EVT_e	eEvent;
	HUINT32					ulNumOfData;
	void					*pvEventData;
}DI_WIFI_CALLBACK_DATA_t;

typedef void (*pfnWIFI_EVT_CALLBACK )(void  *pEventData);


DI_ERR_CODE		DI_WIFI_Scan(HUINT32 ulDevId);
DI_ERR_CODE		DI_WIFI_Connect(HUINT32 ulDevId, DI_WIFI_WPS_e eWPS, DI_WIFI_INFO_t *pstApInfo,HUINT32 ulWaitSeconds);
DI_ERR_CODE		DI_WIFI_Disconnect(HUINT32 ulDevId);
DI_ERR_CODE		DI_WIFI_GetAPInfo(HUINT32 ulDevId, DI_WIFI_INFO_t *pstApInfo);
DI_ERR_CODE		DI_WIFI_GetStrength(HUINT32 ulDevId, HINT32 *pnStrength);
DI_ERR_CODE		DI_WIFI_GetBitRate(HUINT32 ulDevId, HINT32 *pnBitRate);
DI_ERR_CODE 	DI_WIFI_RegisterEventCallback(HUINT32 ulDevId, DI_WIFI_NOTIFY_EVT_e eEvent, pfnWIFI_EVT_CALLBACK pfnCallback);
DI_ERR_CODE		DI_WIFI_GetRSSI(HUINT32 ulDevId, DI_WIFI_RSSI_INFO_t *pRssiInfo);
DI_ERR_CODE		DI_WIFI_GetDrvInfo(HUINT32 ulDevId, DI_WIFI_DRV_INFO_t * pstWifiDrvInfo);
DI_ERR_CODE		DI_WIFI_ResetModule(HUINT32 ulDevId);
DI_ERR_CODE		DI_WIFI_SetWakeOnWLAN(HUINT32 ulDevId, DI_WIFI_WOWL_SETTINGS_t *pstWowlSettings);

DI_ERR_CODE		DI_WIFI_On(HUINT32 ulDevId);
DI_ERR_CODE		DI_WIFI_Off(HUINT32 ulDevId);
DI_ERR_CODE		DI_WIFI_GetVersion(HUINT32 ulDevId, char* ver);
DI_ERR_CODE		DI_WIFI_GetWpsPinCode(HUINT32 ulDevId, char* pin);

#endif	/* _DI_WIFI_H_ */
/* end of file */


