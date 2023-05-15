#ifndef __PL_COMMON_H__
#define __PL_COMMON_H__

#define	________________Header_Files_______________________________
#include "common.h"

#define ________________Defines____________________________________

#define HOMxEX_Malloc			HLIB_STD_MemAlloc
#define HOMxEX_Calloc			HLIB_STD_MemCalloc
#define HOMxEX_Realloc			HLIB_STD_MemRealloc
#define HOMxEX_StrDup			HLIB_STD_StrDup
#define HOMxEX_MemDup			HLIB_STD_MemDup
#define HOMxEX_Free				HLIB_STD_MemFree

#define HOMxFS_Malloc			HLIB_STD_MemAlloc
#define HOMxFS_Calloc			HLIB_STD_MemCalloc
#define HOMxFS_Realloc			HLIB_STD_MemRealloc
#define HOMxFS_StrDup			HLIB_STD_StrDup
#define HOMxFS_MemDup			HLIB_STD_MemDup
#define HOMxFS_Free				HLIB_STD_MemFree

#define ________________Typedef____________________________________
#define	PL_COMMON_URL_MAX				( 512 )
#define	PL_COMMON_NAME_MAX				( 512 )
#define PL_COMMON_NAME_MIDDLE			( 128 )
#define PL_COMMON_NAME_SHORT			( 64 )
#define PL_COMMON_PATH_MAX				( 1024 )

#define DEFAULT_MANUFACTURER_NAME	"Humax"
#define DEFAULT_MANUFACTURER_URL	"http://www.humaxdigital.com"

#define DEFAULT_MODEL_NAME			"HUMAX_STB"

#ifdef CONFIG_PROD_HMS1000S
#define DEFAULT_MODEL_NUMBER		"91-00529"
#else
#define DEFAULT_MODEL_NUMBER		"undefined"
#endif

#define DEFAULT_MODEL_URL			"http://www.humaxdigital.com"
#define	DEFAULT_MODEL_SERIAL_NUMBER	"undefined"
#define DEFAULT_MODEL_DESCRIPTION	"HUMAX Set-Top Box OCTO2.0 Platform"

typedef HINT32	(*PL_COMMON_DATARECEIVER_Callback)(HUINT32 p1, HUINT32 p2, void *pData);

/* �� ���� DRM ���Ǹ� �����ϱ� ���� enum */
// SI DRM Model Type
//typedef enum
//{
//    eDrmModelType_None      = 0,
//    eDrmModelType_ApsHd     = 1,
//    eDrmModelType_Nordig    = 2,
//    eDrmModelType_CiPlus    = 3,                /* ci plus �Ϲ� */
//    eDrmModelType_Nagra     = 4,                /* op�� �ƴ� nagra �Ϲ� cop ó�� �� */
//    eDrmModelType_SkyD      = 5,
//    eDrmModelType_Upc       = 6,
//    eDrmModelType_Irdeto    = 7,                /* op�� �ƴ� Irdeto Optional, OP �䱸�� ��� */
//    eDrmModelType_Arib  = 8,
//    eDrmModelType_Viaccess  = 9,
//
//    // �߰����� ó���� �ʿ��� ��� ��� �߰�
//
//    eDrmModelType_Max
//} DrmModelType_t;


typedef struct {
	HCHAR	szManufacturer[ PL_COMMON_NAME_SHORT ];
	HCHAR	szManufacturerUrl[ PL_COMMON_URL_MAX ];

	HCHAR	szModelName[ PL_COMMON_NAME_MIDDLE ];
	HCHAR	szModelDescription[ PL_COMMON_NAME_MAX ];
	HCHAR	szModelNumber[ PL_COMMON_NAME_SHORT ];
	HCHAR	szModelUrl[ PL_COMMON_URL_MAX ];
	HCHAR	szSerialNumber[ PL_COMMON_NAME_SHORT ];
} PL_COMMON_ProductInfo_t;

typedef struct {
	HINT32	nDevIdx;
	HBOOL	bValid;
	HBOOL	bConnected;
	HBOOL	bActive;
	HBOOL	bRunning;
	HCHAR	szMacAddr[20];

	HULONG	ulIpAddr;
	HULONG 	ulNetmask;
	HULONG 	ulGateway;
	HCHAR 	szIp[20];
	HCHAR   szNetmask[20];
	HCHAR   szGateway[20];
} PL_COMMON_NetworkInfo_t;

typedef struct {
	HINT32	nDevIdx;
	HBOOL	bValid;
	HCHAR	szMacAddr[20];
	/* NetworkIfType_e eIfType; */
	/* additional params... */
} PL_COMMON_MacInfo_t;

typedef struct
{
	PL_COMMON_DATARECEIVER_Callback		pCallback;
} PL_COMMON_INSTANCE_t;

#define ________________Public_Interfaces__________________________
void 	PL_DLNA_Init(void);

void 	PL_USERDATA_Cds_Dump(CO_MediaInstence_t *pItem);

HERROR	PL_COMMON_GetProductInfo(PL_COMMON_ProductInfo_t *pInfo);

HERROR	PL_COMMON_GetDefaultNetwork(PL_COMMON_NetworkInfo_t *pInfo);

HERROR	PL_COMMON_GetActiveNetwork(PL_COMMON_NetworkInfo_t *pInfo);

HERROR	PL_COMMON_GetDefaultMac(PL_COMMON_MacInfo_t *pMacInfo);

HERROR	PL_COMMON_GetEthMac(PL_COMMON_MacInfo_t *pMacInfo);

void 	PL_DLNA_MxLib_LogLevel(int nlevel);

void 	PL_COMMON_PrintTime(UNIXTIME *putCurrentTime);

HERROR	PL_COMMON_WaitShutDown(const HCHAR *pszFrom, HUINT32 ulWaitTime);

HUID	PL_COMMON_GetContentID(HCHAR* media_path);

#endif /* !__PL_COMMON_H__ */

