/**************************************************************************************
 * @file nxlink.c
 * defines APIs's body
 *
 * NXLINK (NetfliX external LINK API for OCTO interface)
 *
 * @author
 * @date
 * @attention   Copyright (c) 2015-2016 Humax - All rights reserved.
 *
 **************************************************************************************/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ctype.h>

#include <nxlink.h>
#include "nxlink_int.h"
/* "CONFIG_PRODUCT_NAME" */
#if defined(__NXLINK_IPC_APKV1__)
#include <global_config.h> 
#include "nxlink_ipc_apkv1.h"
#elif defined(__NXLINK_IPC_APKV2__)
#include <globalconfig.h> 
#include <hlib.h>
#include "nxlink_ipc_apkv2.h"
#elif defined(__NXLINK_IPC_PROCAPI__)
#include <globalconfig.h> 
#include <hlib.h>
#include "nxlink_ipc_procapi.h"
#endif

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#undef		HxLOG_DOMAIN
#define		HxLOG_DOMAIN				"Netflix"

#define	_SYSID_PRODUCT_HDR1000S         "809C.7D00"  /* hdr-1000sph2(hdr-1000s) */
#define	_SYSID_PRODUCT_HDR1010S         "809C.7D01"  /* hdr-1000sph2(hdr-1010s) */
#define	_SYSID_PRODUCT_HDR1010S_RSV1    "809C.7D02"  /* hdr-1000sph2(hdr-1010s), (809C7D00 ~ 809C7D0F) assigned at FreeSat-G2 */
#define	_SYSID_PRODUCT_HDR1010S_RSV2    "809C.7D03"  /* hdr-1000sph2(hdr-1010s) */
#define	_SYSID_PRODUCT_HDR1010S_RSV3    "809C.7D04"  /* hdr-1000sph2(hdr-1010s) */
#define	_SYSID_PRODUCT_HDR1100S         "809C.7D05"  /* hdr-1000sph2(hdr-1100s) */
#define	_SYSID_PRODUCT_HB1000S          "809C.7D10"  /* hb-1000sph2(hb-1000s)   */
#define	_SYSID_PRODUCT_FVP5000T         "9010.7D44"  /* fvp-5000t */


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/**
 * Product/Model type
 */
typedef enum {
	PROFILE_PRODUCT_NA           = 0,  /* Unsupported                      */
	PROFILE_PRODUCT_HDR1000SPH2  = 1,  /* hdr1000sph2 default(unsupported) */
	PROFILE_PRODUCT_HDR1000S     = 2,  /* hdr1000sph2 group(but hdr1000s)  */
	PROFILE_PRODUCT_HDR1010S     = 3,  /* hdr1100sph2 group(but hdr1010s)  */
	PROFILE_PRODUCT_HDR1100S     = 4,  /* hdr1000sph2 group(but hdr1100s)  */
	PROFILE_PRODUCT_HB1000S      = 5,  /* hb1000s, hb1000sph2              */
	PROFILE_PRODUCT_HDR3000T     = 6,  /* hdr3000t, group#2                */
	PROFILE_PRODUCT_HMS1000T     = 7,  /* hms1000t, group#3                */
	PROFILE_PRODUCT_HDR4000T     = 8,  /* hdr4000t, group#2-1              */
	PROFILE_PRODUCT_FVP4000T     = 9,  /* fvp4000t, group#2-1              */
	PROFILE_PRODUCT_ICORDPRO     = 10, /* icordpro, group#2-2              */
	PROFILE_PRODUCT_HDNANONEO    = 11, /* hdnanoneo, group#2-2             */
	PROFILE_PRODUCT_HMS1000SPH2  = 12, /* hms1000sph2                      */
	PROFILE_PRODUCT_HMSLITE      = 13, /* hmslite                          */
	PROFILE_PRODUCT_YSR2000      = 14, /* ysr2000                          */
	PROFILE_PRODUCT_YS1000       = 15, /* ys1000                           */
	PROFILE_PRODUCT_MARQUEEPOC   = 16, /* MARQUEEPOC                       */
	PROFILE_PRODUCT_FVP5000T     = 17, /* fvp5000t, group#2-1              */
	PROFILE_PRODUCT_MAX,
} eProfile_ProductType;

/*
 * ###############################################################################################
 * ## Group #1 : HDR-1010S (base), HDR-1000S and HB-1000S  (URSR12.04) / HDR-1010S VE
 * ## Group #2 : HDR-4000T, iCord Pro, HDR-3000T (base) and HD NANO NEO (above URSR12.04)
 * ## Group #3 : HMS-1000T(base), "iCord Evolution (base)" and "iCord Evolution Lite (HMS Lite)"
 * ###############################################################################################
 * ## Appkit_v1 : HDR-1010S (base), HDR-1000S and HB-1000S  (URSR12.04) / HDR-1010S VE
 * ###############################################################################################
 * ifeq ($(CONFIG_PRODUCT_NAME),"hdr1000s")
 * obj-y += nxlink_ipc_apkv1.o
 * endif
 * ifeq ($(CONFIG_PRODUCT_NAME),"hdr1000sph2")
 * obj-y += nxlink_ipc_apkv1.o
 * endif
 * ifeq ($(CONFIG_PRODUCT_NAME),"hdr1010s")
 * obj-y += nxlink_ipc_apkv1.o
 * endif
 * ifeq ($(CONFIG_PRODUCT_NAME),"hdr1100s")
 * obj-y += nxlink_ipc_apkv1.o
 * endif
 * ifeq ($(CONFIG_PRODUCT_NAME),"hb1000s")
 * obj-y += nxlink_ipc_apkv1.o
 * endif
 * ifeq ($(CONFIG_PRODUCT_NAME),"hb1000sph2")
 * obj-y += nxlink_ipc_apkv1.o
 * endif
 * ###############################################################################################
 * ## Appkit_v2 : HMS-1000T(base), "iCord Evolution (base)" and "iCord Evolution Lite (HMS Lite)"
 * ###############################################################################################
 * ifeq ($(CONFIG_PRODUCT_NAME),"hms1000sph2")
 * obj-y += nxlink_ipc_apkv2.o
 * endif
 * ifeq ($(CONFIG_PRODUCT_NAME),"hms1000t")
 * obj-y += nxlink_ipc_apkv2.o
 * endif
 * ifeq ($(CONFIG_PRODUCT_NAME),"hdr3000t")
 * obj-y += nxlink_ipc_apkv2.o
 * endif
 * ifeq ($(CONFIG_PRODUCT_NAME),"hmslite")
 * obj-y += nxlink_ipc_apkv2.o
 * endif
 * ifeq ($(CONFIG_PRODUCT_NAME),"fvp4000t")
 * obj-y += nxlink_ipc_procapi.o
 * endif
 * ifeq ($(CONFIG_PRODUCT_NAME),"fvp5000t")
 * obj-y += nxlink_ipc_procapi.o
 * endif
 * ###############################################################################################
 * ## ProcessAPI #1 : iCord Pro, HD NANO NEO (above URSR12.04)
 * ###############################################################################################
 * ifeq ($(CONFIG_PRODUCT_NAME),"icordpro")
 * obj-y += nxlink_ipc_procapi.o
 * endif
 * ifeq ($(CONFIG_PRODUCT_NAME),"hdnanoneo")
 * obj-y += nxlink_ipc_procapi.o
 * endif
 */
typedef enum {
	PROFILE_GROUP_NA     = 0,
	PROFILE_GROUP_1ST    = 1,  /* Initial Group#1  : HDR-1000S, HDR-1010S, HDR-1100S and HB-1000S  */
	PROFILE_GROUP_2ND    = 2,  /* Initial Group#2  : FVP-4000T(HDR-4000T), iCordNEO(iCordPro), HDR-3000T, HD-NEO */
	PROFILE_GROUP_3RD    = 3,  /* Initial Group#3  : HMS-1000T, iCord-Evolution(HMS-1000SPH2) */
	PROFILE_GROUP_EXT    = 4   /* Group EXT: all of ce-oem type's products should be placed in here. */
} eProfile_GroupType;

typedef enum {
	PROFILE_IPC_NA      = 0,
	PROFILE_IPC_APKV1   = 1, /* Appkit_v1  : hdr1100sph2, hdr1010s, hb1100s, hb1000sph2 */
	PROFILE_IPC_APKV2   = 2, /* Appkit_v2  : hms1000sph2, hms1000t, hdr3000t            */
	PROFILE_IPC_PROCAPI = 3  /* ProcessAPI : icordpro, hdneo, hdnanoneo                 */
} eProfile_IpcType;

typedef enum {
	PROFILE_PVR_NONE = 0,      /* Zapper */
	PROFILE_PVR_INTERNAL,      /* PVR with CONFIG_PROD_DEVICE_INTERNAL_HDD */
	PROFILE_PVR_EXTERNAL       /* PVR ... */
} eProfile_PvrType;

typedef enum
{
	PROFILE_NETIF_NA           = 0x0,
	PROFILE_NETIF_ETH          = 0x1,
	PROFILE_NETIF_WIFI         = 0x2,
	PROFILE_NETIF_WIFI_DONGLE  = 0x4,
	PROFILE_NETIF_WIFI_ETH1_BR = 0x8, /* For exception case such as wifi_thru_eth1_bridge like YSR-2000 : http://svn:3000/issues/98026 */
	PROFILE_NETIF_PPPOE        = 0x10,
	PROFILE_NETIF_UNKOWN       = 0x20
} eProfile_NetworkType;

typedef struct
{
	uint32_t isSupported;
	char szlModelName[_NXL_BLEN_32];
	eProfile_ProductType  productType;
	eProfile_GroupType    groupType;
	eProfile_IpcType      ipcType;
	uint32_t              hasScreenSaver;
	eProfile_PvrType      pvrType;
	eProfile_NetworkType  networkType;
}NXL_ProductInfo_t;

typedef struct
{
	uint32_t              isInitialized;
	uint32_t              isSupported;
	eProfile_ProductType  productType;
	NXL_ProductInfo_t    *pstProductInfo;
	NXL_BoxInfo_t         tBoxInfo;
}NXL_ContextInfo_t;


/*******************************************************************/
/******************** Global/Static Variables   ********************/
/*******************************************************************/
static NXL_ContextInfo_t s_stNxlinkMainInfo = { 0, };
static uint32_t _nxl_module_init = 0;
static uint8_t tmpbuf[_NXL_BLEN_64] = {0, };

static NXL_ProductInfo_t s_stProductInfo_Default = { /* Default Setting */
	.isSupported            = 0,
	.szlModelName           = "N/A",
	.productType            = PROFILE_PRODUCT_NA,
	.groupType              = PROFILE_GROUP_NA,
	.ipcType                = PROFILE_IPC_NA,
	.hasScreenSaver         = 0,
	.pvrType                = PROFILE_PVR_NONE,
	.networkType            = PROFILE_NETIF_NA,
};

static NXL_ProductInfo_t s_stProductInfo_Hdr1000sph2Default = { /* Group#1 : HDR-1000SPH2 Default */
	.isSupported            = 0,
	.szlModelName           = "Humax Digital_HDR-1000S",
	.productType            = PROFILE_PRODUCT_HDR1000SPH2,
	.groupType              = PROFILE_GROUP_1ST,
	.ipcType                = PROFILE_IPC_APKV1,
	.hasScreenSaver         = 0,
	.pvrType                = PROFILE_PVR_INTERNAL,
	.networkType            = (PROFILE_NETIF_ETH | PROFILE_NETIF_WIFI),
};

static NXL_ProductInfo_t s_stProductInfo_Hdr1000s = { /* Group#1 : HDR-1000SPH2 */
	.isSupported            = 1,
	.szlModelName           = "Humax Digital_HDR-1000S",
	.productType            = PROFILE_PRODUCT_HDR1000S,
	.groupType              = PROFILE_GROUP_1ST,
	.ipcType                = PROFILE_IPC_APKV1,
	.hasScreenSaver         = 0,
	.pvrType                = PROFILE_PVR_INTERNAL,
	.networkType            = (PROFILE_NETIF_ETH | PROFILE_NETIF_WIFI_DONGLE), /* WiFi-dongle */
};

static NXL_ProductInfo_t s_stProductInfo_Hdr1010s = { /* Group#1 : HDR-1010S */
	.isSupported            = 1,
	.szlModelName           = "Humax Digital_HDR-1010S",
	.productType            = PROFILE_PRODUCT_HDR1010S,
	.groupType              = PROFILE_GROUP_1ST,
	.ipcType                = PROFILE_IPC_APKV1,
	.hasScreenSaver         = 0,
	.pvrType                = PROFILE_PVR_INTERNAL,
	.networkType            = (PROFILE_NETIF_ETH | PROFILE_NETIF_WIFI), /* embedded-WiFi */
};

static NXL_ProductInfo_t s_stProductInfo_Hdr1100s = { /* Group#1 : HDR-1100S */
	.isSupported            = 1,
	.szlModelName           = "Humax Digital_HDR-1100S",
	.productType            = PROFILE_PRODUCT_HDR1100S,
	.groupType              = PROFILE_GROUP_1ST,
	.ipcType                = PROFILE_IPC_APKV1,
	.hasScreenSaver         = 0,
	.pvrType                = PROFILE_PVR_INTERNAL,
	.networkType            = (PROFILE_NETIF_ETH | PROFILE_NETIF_WIFI), /* embedded-WiFi */
};

static NXL_ProductInfo_t s_stProductInfo_Hb1000s = { /* Group#1 : HB-1000S */
	.isSupported            = 1,
	.szlModelName           = "Humax Digital_HB-1000S",
	.productType            = PROFILE_PRODUCT_HB1000S,
	.groupType              = PROFILE_GROUP_1ST,
	.ipcType                = PROFILE_IPC_APKV1,
	.hasScreenSaver         = 0,
	.pvrType                = PROFILE_PVR_NONE,
	.networkType            = (PROFILE_NETIF_ETH | PROFILE_NETIF_WIFI_DONGLE), /* WiFi-dongle */
};

static NXL_ProductInfo_t s_stProductInfo_Hms1000sph2 = { /* Group#2 : HMS-1000SPH2 */
	.isSupported            = 0,
	.szlModelName           = "Humax Digital_HMS-1000S",
	.productType            = PROFILE_PRODUCT_HMS1000SPH2,
	.groupType              = PROFILE_GROUP_2ND,
	.ipcType                = PROFILE_IPC_APKV2,
	.hasScreenSaver         = 1,
	.pvrType                = PROFILE_PVR_INTERNAL,
	.networkType            = (PROFILE_NETIF_ETH | PROFILE_NETIF_WIFI), /* internal WiFi */
};

static NXL_ProductInfo_t s_stProductInfo_Hms1000t = { /* Group#2 : HMS-1000T */
	.isSupported            = 1,
	.szlModelName           = "Humax Digital_HMS-1000T",
	.productType            = PROFILE_PRODUCT_HMS1000T,
	.groupType              = PROFILE_GROUP_2ND,
	.ipcType                = PROFILE_IPC_APKV2,
	.hasScreenSaver         = 1,
	.pvrType                = PROFILE_PVR_INTERNAL,
	.networkType            = (PROFILE_NETIF_ETH | PROFILE_NETIF_WIFI), /* internal WiFi */
};

static NXL_ProductInfo_t s_stProductInfo_Hdr3000t = { /* Group#2 : HMS-1000SPH2 */
	.isSupported            = 1,
	.szlModelName           = "Humax Digital_HDR-3000T",
	.productType            = PROFILE_PRODUCT_HDR3000T,
	.groupType              = PROFILE_GROUP_2ND,
	.ipcType                = PROFILE_IPC_APKV2,
	.hasScreenSaver         = 1,
	.pvrType                = PROFILE_PVR_INTERNAL,
	.networkType            = (PROFILE_NETIF_ETH | PROFILE_NETIF_WIFI_DONGLE), /* WiFi dongle */
};

static NXL_ProductInfo_t s_stProductInfo_Hdr4000t = { /* Group#2 : HDR-4000T */
	.isSupported            = 0,
	.szlModelName           = "Humax Digital_FVP-4000T",
	.productType            = PROFILE_PRODUCT_HDR4000T,
	.groupType              = PROFILE_GROUP_2ND,
	.ipcType                = PROFILE_IPC_APKV2,
	.hasScreenSaver         = 0, /* CONFIG_APUI_SCREEN_SAVER : off */
	.pvrType                = PROFILE_PVR_INTERNAL,
	.networkType            = (PROFILE_NETIF_ETH | PROFILE_NETIF_WIFI), /* internal dongle */
};

static NXL_ProductInfo_t s_stProductInfo_Fvp4000t = { /* Group#2 : FVP-4000T */
	.isSupported            = 1,
	.szlModelName           = "Humax Digital_FVP-4000T",
	.productType            = PROFILE_PRODUCT_FVP4000T,
	.groupType              = PROFILE_GROUP_2ND,
	.ipcType                = PROFILE_IPC_APKV2,
	.hasScreenSaver         = 0, /* CONFIG_APUI_SCREEN_SAVER : off */
	.pvrType                = PROFILE_PVR_INTERNAL,
	.networkType            = (PROFILE_NETIF_ETH | PROFILE_NETIF_WIFI), /* internal dongle */
};

static NXL_ProductInfo_t s_stProductInfo_Fvp5000t = { /* Group#2 : FVP-5000T */
	.isSupported            = 1,
	.szlModelName           = "Humax Digital_FVP-5000T",
	.productType            = PROFILE_PRODUCT_FVP5000T,
	.groupType              = PROFILE_GROUP_2ND,
	.ipcType                = PROFILE_IPC_APKV2,
	.hasScreenSaver         = 0, /* CONFIG_APUI_SCREEN_SAVER : off */
	.pvrType                = PROFILE_PVR_INTERNAL,
	.networkType            = (PROFILE_NETIF_ETH | PROFILE_NETIF_WIFI), /* internal dongle */
};

static NXL_ProductInfo_t s_stProductInfo_Ysr2000 = { /* YSR-2000 */
	.isSupported            = 1,
	.szlModelName           = "TDC_YSR-2000",
	.productType            = PROFILE_PRODUCT_YSR2000,
	.groupType              = PROFILE_GROUP_EXT,
	.ipcType                = PROFILE_IPC_APKV2,
	.hasScreenSaver         = 0, /* CONFIG_APUI_SCREEN_SAVER : off */
	.pvrType                = PROFILE_PVR_INTERNAL,
	.networkType            = (PROFILE_NETIF_ETH | PROFILE_NETIF_WIFI_ETH1_BR), /* For exception case such as wifi_thru_eth1_bridge like YSR-2000 : http://svn:3000/issues/98026 */
};

static NXL_ProductInfo_t s_stProductInfo_Ys1000 = { /* YS1000 */
	.isSupported            = 1,
	.szlModelName           = "TDC_YS-1000",
	.productType            = PROFILE_PRODUCT_YS1000,
	.groupType              = PROFILE_GROUP_EXT,
	.ipcType                = PROFILE_IPC_APKV2,
	.hasScreenSaver         = 0, /* CONFIG_APUI_SCREEN_SAVER : off */
	.pvrType                = PROFILE_PVR_INTERNAL,
	.networkType            = (PROFILE_NETIF_ETH | PROFILE_NETIF_WIFI), /* internal dongle */
};

/* TODO : Check ESN key */
static NXL_ProductInfo_t s_stProductInfo_MarqueePoC = { /* MARQUEE-POC */
	.isSupported            = 1,
#if 1
	.szlModelName           = "Humax Digital_FVP-4000T", /* Use FVP-4000T modelName temporarily util being authorized */
#else
	.szlModelName           = "Humax Digital_MARQUEE-POC", 
#endif
	.productType            = PROFILE_PRODUCT_MARQUEEPOC,
	.groupType              = PROFILE_GROUP_EXT,
	.ipcType                = PROFILE_IPC_APKV2,
	.hasScreenSaver         = 0, /* CONFIG_APUI_SCREEN_SAVER : off */
	.pvrType                = PROFILE_PVR_INTERNAL,
	.networkType            = (PROFILE_NETIF_ETH | PROFILE_NETIF_WIFI), /* For exception case such as wifi_thru_eth1_bridge like MARQUEE-POC : http://svn:3000/issues/98026 */
};

/* TODO: move to profile */
static eProfile_ProductType local_profile_getProductType(const char *productName);
static eProfile_IpcType local_profile_getIpcTypeByProductType(eProfile_ProductType productType);
static int32_t NXL_UTIL_StrCaseCmp (const char *s1, const char *s2);
static int32_t NXL_UTIL_StrNCaseCmp (const char *s1, const char *s2, uint32_t n);
static char *NXL_UTIL_StrCaseStr(const char *haystack, const char *needle);

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#define __________Internal_Function__________

static NXL_ContextInfo_t *local_getContext(void)
{
	return &s_stNxlinkMainInfo;
}

static NXL_ProductInfo_t *local_profile_getProductInfo(NXL_ContextInfo_t *pstContextInfo)
{
	NXL_Assert(pstContextInfo);
	if (pstContextInfo)
	{
		return pstContextInfo->pstProductInfo;
	}
	NXL_LOG_ERR("[NXL] Error> invalid context for getting productInfo!\n");
	return NULL;
}

static eProfile_IpcType local_profile_getProductIPC(NXL_ContextInfo_t *pstContextInfo)
{
	eProfile_ProductType productType = PROFILE_PRODUCT_NA;
#if defined(CONFIG_PRODUCT_NAME)
	const char *productName = CONFIG_PRODUCT_NAME;
#else
	const char *productName = "UNKNOWN";
#endif

	NXL_Assert(pstContextInfo);
	if ((pstContextInfo != NULL) && (pstContextInfo->pstProductInfo != NULL))
	{
		return pstContextInfo->pstProductInfo->ipcType;
	}
	NXL_LOG_ERR("[NXL] Error> invalid context for getting productIPC!\n");

	/*
	 * TODO : consideration between standby/operation and netflix background
	 *        Some functions (applicationID/windowID, ...) have to be immediately available regardless of NXL_Init().
	 *        This is because NXLink is controlled by partner/dpi,
	 *                but partner/platform(dfb, ...) can be operated asynchronously with partner/dpi.
	 *        So, some functions must allow for the external IPC operation.
	 */
	productType = local_profile_getProductType(productName);
	if (productType != PROFILE_PRODUCT_NA)
	{
		return local_profile_getIpcTypeByProductType(productType);
	}
	
	return PROFILE_IPC_NA;
}

static NXL_ERR local_nxl_isInitialized(void)
{
	NXL_ContextInfo_t *hContext = local_getContext();
	return hContext->isInitialized;
}

static eProfile_IpcType local_profile_getIpcTypeByProductType(eProfile_ProductType productType)
{
	eProfile_IpcType ipcType = PROFILE_IPC_NA;

	switch(productType) {
	case PROFILE_PRODUCT_HDR1000SPH2 :
	case PROFILE_PRODUCT_HDR1000S :
	case PROFILE_PRODUCT_HDR1010S :
	case PROFILE_PRODUCT_HDR1100S :
	case PROFILE_PRODUCT_HB1000S :
		ipcType = PROFILE_IPC_APKV1;
		break;
	case PROFILE_PRODUCT_HMS1000SPH2 :
	case PROFILE_PRODUCT_HMS1000T :
	case PROFILE_PRODUCT_HDR3000T :
	case PROFILE_PRODUCT_HDR4000T :
	case PROFILE_PRODUCT_FVP4000T :
	case PROFILE_PRODUCT_FVP5000T :
	case PROFILE_PRODUCT_HMSLITE :
	case PROFILE_PRODUCT_ICORDPRO :
	case PROFILE_PRODUCT_HDNANONEO :
	case PROFILE_PRODUCT_YSR2000 :
	case PROFILE_PRODUCT_MARQUEEPOC :
	case PROFILE_PRODUCT_YS1000 :
		ipcType = PROFILE_IPC_APKV2;
		break;
	case PROFILE_PRODUCT_NA :
	default :
		ipcType = PROFILE_IPC_NA;
		break;
	}
	return ipcType;
}

static eProfile_IpcType local_profile_getIpcType(NXL_ProductInfo_t *ptProductInfo)
{
	eProfile_IpcType ipcType = PROFILE_IPC_NA;
	if (ptProductInfo) ipcType = local_profile_getIpcTypeByProductType(ptProductInfo->productType);
	return ipcType;
}

static char *getProductProfileToStr(eProfile_ProductType type)
{
	switch(type) {
	_ENUM_TO_STR(PROFILE_PRODUCT_HDR1000SPH2);
	_ENUM_TO_STR(PROFILE_PRODUCT_HDR1000S);
	_ENUM_TO_STR(PROFILE_PRODUCT_HDR1010S);
	_ENUM_TO_STR(PROFILE_PRODUCT_HDR1100S);
	_ENUM_TO_STR(PROFILE_PRODUCT_HB1000S);
	_ENUM_TO_STR(PROFILE_PRODUCT_HMS1000SPH2);
	_ENUM_TO_STR(PROFILE_PRODUCT_HMS1000T);
	_ENUM_TO_STR(PROFILE_PRODUCT_HDR3000T);
	_ENUM_TO_STR(PROFILE_PRODUCT_HDR4000T);
	_ENUM_TO_STR(PROFILE_PRODUCT_FVP4000T);
	_ENUM_TO_STR(PROFILE_PRODUCT_FVP5000T);
	_ENUM_TO_STR(PROFILE_PRODUCT_HMSLITE);
	_ENUM_TO_STR(PROFILE_PRODUCT_ICORDPRO);
	_ENUM_TO_STR(PROFILE_PRODUCT_HDNANONEO);
	default:
		break;
	}
	return "N/A";
}

static eProfile_ProductType local_profile_getProductType(const char *productName)
{
	eProfile_ProductType productType = PROFILE_PRODUCT_NA;

	if (productName == NULL)
	{
		NXL_LOG_ERR("[NXL] Error> NULL product name!\n");
		goto func_error;
	}
	NXL_LOG_DBG("[NXL] [productName:%s]\n", productName);

/*
#if defined(CONFIG_PROD_HMS1000S)
        hErr = local_profile_Init_Hms1000s();
#elif defined(CONFIG_PROD_IR4000HD)
        hErr = local_profile_Init_Ir4000hd();
#elif defined(CONFIG_PROD_ICORDPRO)
        hErr = local_profile_Init_Icordpro();
#elif defined(CONFIG_PROD_HD9000I)
        hErr = local_profile_Init_hd9000i();
#elif defined(CONFIG_PROD_HMS1000T)
        hErr = local_profile_Init_Hms1000t();
#elif defined(CONFIG_PROD_IHDR6100C)
        hErr = local_profile_Init_Ihdr6100c();
#elif defined(CONFIG_PROD_HDR3000T)
        hErr = local_profile_Init_Hdr3000t();
#elif defined(CONFIG_PROD_HDR4000T)
        hErr = local_profile_Init_Hdr4000t();
#elif defined(CONFIG_PROD_FVP4000T)
        hErr = local_profile_Init_Fvp4000t();
#elif defined(CONFIG_PROD_YSR2000)
        hErr = local_profile_Init_Ysr2000();
#elif defined(CONFIG_PROD_YS1000)
        hErr = local_profile_Init_Ys1000();
#elif defined(CONFIG_PROD_MARQUEEPOC)
        hErr = local_profile_Init_MarqueePoC();
#else
        #error ("local_profile_Init_XXX for a Product is not activated!\n")
#endif
*/

	if (NXL_UTIL_StrCaseStr(productName, "hdr1000sph2"))
	{
		productType = PROFILE_PRODUCT_HDR1000SPH2;
	}
	else if (NXL_UTIL_StrCaseStr(CONFIG_PRODUCT_NAME, "hdr1000s"))
	{
		productType = PROFILE_PRODUCT_HDR1000S;
	}
	else if (NXL_UTIL_StrCaseStr(CONFIG_PRODUCT_NAME, "hdr1010s")) /* no hdr1010s_defconfig */
	{
		productType = PROFILE_PRODUCT_HDR1010S;
	}
	else if (NXL_UTIL_StrCaseStr(CONFIG_PRODUCT_NAME, "hdr1100s")) /* no hdr1100s_defconfig */
	{
		productType = PROFILE_PRODUCT_HDR1100S;
	}
	else if (NXL_UTIL_StrCaseStr(CONFIG_PRODUCT_NAME, "hb1000s"))
	{
		productType = PROFILE_PRODUCT_HB1000S;
	}
	else if (NXL_UTIL_StrCaseStr(CONFIG_PRODUCT_NAME, "hb1000sph2"))
	{
		productType = PROFILE_PRODUCT_HB1000S;
	}
	else if (NXL_UTIL_StrCaseStr(CONFIG_PRODUCT_NAME, "hms1000sph2"))
	{
		productType = PROFILE_PRODUCT_HMS1000SPH2;
	}
	else if (NXL_UTIL_StrCaseStr(CONFIG_PRODUCT_NAME, "hms1000t"))
	{
		productType = PROFILE_PRODUCT_HMS1000T;
	}
	else if (NXL_UTIL_StrCaseStr(productName, "hdr3000t"))
	{
		productType = PROFILE_PRODUCT_HDR3000T;
	}
	else if (NXL_UTIL_StrCaseStr(CONFIG_PRODUCT_NAME, "hdr4000t"))
	{
		productType = PROFILE_PRODUCT_HDR4000T;
	}
	else if (NXL_UTIL_StrCaseStr(CONFIG_PRODUCT_NAME, "fvp4000t"))
	{
		productType = PROFILE_PRODUCT_FVP4000T;
	}
	else if (NXL_UTIL_StrCaseStr(CONFIG_PRODUCT_NAME, "fvp5000t"))
	{
		productType = PROFILE_PRODUCT_FVP5000T;
	}
	else if (NXL_UTIL_StrCaseStr(CONFIG_PRODUCT_NAME, "hmslite"))
	{
		productType = PROFILE_PRODUCT_HMSLITE;
	}
	else if (NXL_UTIL_StrCaseStr(CONFIG_PRODUCT_NAME, "icordpro"))
	{
		productType = PROFILE_PRODUCT_ICORDPRO;
	}
	else if (NXL_UTIL_StrCaseStr(CONFIG_PRODUCT_NAME, "hdnanoneo"))
	{
		productType = PROFILE_PRODUCT_HDNANONEO;
	}
	else if (NXL_UTIL_StrCaseStr(CONFIG_PRODUCT_NAME, "ysr2000"))
	{
		productType = PROFILE_PRODUCT_YSR2000;
	}
	else if (NXL_UTIL_StrCaseStr(CONFIG_PRODUCT_NAME, "ys1000"))
	{
		productType = PROFILE_PRODUCT_YS1000;
	}
	else if (NXL_UTIL_StrCaseStr(CONFIG_PRODUCT_NAME, "marqueepoc"))
	{
		productType = PROFILE_PRODUCT_MARQUEEPOC;
	}

func_error:
	NXL_LOG_DBG("[NXL] [productType:%d,%s]\n", productType, getProductProfileToStr(productType));
	return productType;
}

static NXL_ERR local_profile_setDefaultProductInfo(eProfile_ProductType productType, NXL_ProductInfo_t *ptProductInfo) /* rather than (const char *productName) */
{
	if ((productType == PROFILE_PRODUCT_NA) || (ptProductInfo == NULL))
	{
		NXL_LOG_ERR("[NXL] Error> Unsupported product(%d)!\n", productType);
		return NXL_ERR_UNSUPPORTED;
	}

	memset(ptProductInfo, 0x0, sizeof(NXL_ProductInfo_t));
	switch(productType) {
	case PROFILE_PRODUCT_HDR1000SPH2 : { memcpy(ptProductInfo, &s_stProductInfo_Hdr1000sph2Default, sizeof(NXL_ProductInfo_t)); return NXL_ERR_OK; }
	case PROFILE_PRODUCT_HDR1000S    : { memcpy(ptProductInfo, &s_stProductInfo_Hdr1000s, sizeof(NXL_ProductInfo_t)); return NXL_ERR_OK; }
	case PROFILE_PRODUCT_HDR1010S    : { memcpy(ptProductInfo, &s_stProductInfo_Hdr1010s, sizeof(NXL_ProductInfo_t));    return NXL_ERR_OK; }
	case PROFILE_PRODUCT_HDR1100S    : { memcpy(ptProductInfo, &s_stProductInfo_Hdr1100s, sizeof(NXL_ProductInfo_t)); return NXL_ERR_OK; }
	case PROFILE_PRODUCT_HB1000S     : { memcpy(ptProductInfo, &s_stProductInfo_Hb1000s, sizeof(NXL_ProductInfo_t));     return NXL_ERR_OK; }
	case PROFILE_PRODUCT_HMS1000SPH2 : { memcpy(ptProductInfo, &s_stProductInfo_Hms1000sph2, sizeof(NXL_ProductInfo_t)); return NXL_ERR_OK; }
	case PROFILE_PRODUCT_HMS1000T    : { memcpy(ptProductInfo, &s_stProductInfo_Hms1000t, sizeof(NXL_ProductInfo_t));    return NXL_ERR_OK; }
	case PROFILE_PRODUCT_HDR3000T    : { memcpy(ptProductInfo, &s_stProductInfo_Hdr3000t, sizeof(NXL_ProductInfo_t));    return NXL_ERR_OK; }
	case PROFILE_PRODUCT_HDR4000T    : { memcpy(ptProductInfo, &s_stProductInfo_Hdr4000t, sizeof(NXL_ProductInfo_t));    return NXL_ERR_OK; }
	case PROFILE_PRODUCT_FVP4000T    : { memcpy(ptProductInfo, &s_stProductInfo_Fvp4000t, sizeof(NXL_ProductInfo_t));    return NXL_ERR_OK; }
	case PROFILE_PRODUCT_FVP5000T    : { memcpy(ptProductInfo, &s_stProductInfo_Fvp5000t, sizeof(NXL_ProductInfo_t));    return NXL_ERR_OK; }
	case PROFILE_PRODUCT_YSR2000     : { memcpy(ptProductInfo, &s_stProductInfo_Ysr2000, sizeof(NXL_ProductInfo_t));    return NXL_ERR_OK; }
	case PROFILE_PRODUCT_YS1000      : { memcpy(ptProductInfo, &s_stProductInfo_Ys1000, sizeof(NXL_ProductInfo_t));    return NXL_ERR_OK; }
	case PROFILE_PRODUCT_MARQUEEPOC  : { memcpy(ptProductInfo, &s_stProductInfo_MarqueePoC, sizeof(NXL_ProductInfo_t));    return NXL_ERR_OK; }
	case PROFILE_PRODUCT_HMSLITE     :
	case PROFILE_PRODUCT_ICORDPRO    :
	case PROFILE_PRODUCT_HDNANONEO   :
	default :
		{
			NXL_LOG_ERR("[NXL] Error> Unsupported product!\n");
			if(0) ptProductInfo = NULL;
			//else  ptProductInfo = &s_stProductInfo_Default;
			else  memcpy(ptProductInfo, &s_stProductInfo_Default, sizeof(NXL_ProductInfo_t));
			break;
		}
	}
	return NXL_ERR_UNSUPPORTED;
}

uint32_t NXL_Init(void)
{
	uint32_t nret = NXL_ERR_OK;
#if defined(CONFIG_PRODUCT_NAME)
	const char *productName = CONFIG_PRODUCT_NAME;
#else
	const char *productName = "UNKNOWN";
#endif
	eProfile_ProductType productType = PROFILE_PRODUCT_NA;
	//NXL_ProductInfo_t *pProductInfo = &s_stProductInfo_Default;
	NXL_ContextInfo_t *hContext = local_getContext();
	NXL_BoxInfo_t *pstBoxInfo = NULL;
	eProfile_IpcType ipcType = PROFILE_IPC_NA;

	NXL_LOG_ERR("++\n");
	NXL_LOG_DBG("++Product/Model is [%s]\n", productName);
	
	/* CONFIG_PRODUCT_NAME */
#if defined(CONFIG_PROD_HDR1000S)
        ;
#elif defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
        ;
#elif defined(CONFIG_PROD_ICORDPRO)
        ;
#elif defined(CONFIG_PROD_HDNEO)
        ;
#elif defined(CONFIG_PROD_HMS1000T)  || defined(CONFIG_PROD_HDR3000T)
        ;
#elif defined(CONFIG_PROD_HDR4000T)
        ;
#elif defined(CONFIG_PROD_FVP4000T)
        ;
#elif defined(CONFIG_PROD_YSR2000)
        ;
#elif defined(CONFIG_PROD_YS1000)
        ;
#elif defined(CONFIG_PROD_MARQUEEPOC)
        ;
#else
	#error ("Error> Unsupported product/model! Check the CONFIG_PRODUCT_NAME!\n")
#endif
	
	if (hContext->isInitialized)
	{
		NXL_LOG_ERR("[NXL] Error> Already initialized!\n");
		return NXL_ERR_ALREADY_INITIALIZED;
	}
	hContext->pstProductInfo = NULL;

	productType = local_profile_getProductType(productName);
	NXL_LOG_ERR("product[%s, %s]\n", productName, getProductProfileToStr(productType));
	if (productType == PROFILE_PRODUCT_NA)
	{
		NXL_LOG_ERR("[NXL] Error> Already initialized!\n");
		return NXL_ERR_UNSUPPORTED;
	}

	hContext->pstProductInfo = (NXL_ProductInfo_t * )malloc(sizeof(NXL_ProductInfo_t));
	memset(hContext->pstProductInfo, 0x0, sizeof(NXL_ProductInfo_t));
	if (local_profile_setDefaultProductInfo(productType, hContext->pstProductInfo) != NXL_ERR_OK)
	{
		NXL_LOG_ERR("[NXL] Error> Unsupported product!\n");
		return NXL_ERR_UNSUPPORTED;
	}
	NXL_LOG_DBG("PortalModelName:%s\n", hContext->pstProductInfo->szlModelName);

	pstBoxInfo = &hContext->tBoxInfo;
	ipcType = local_profile_getProductIPC(hContext);
	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_init(pstBoxInfo); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_init(pstBoxInfo); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_init(pstBoxInfo); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}

	/* Check Freesat product group : hdr-1000s, hdr-1010s, hdr-1100s */
	if ((productType == PROFILE_PRODUCT_HDR1000SPH2) || /* local_profile_setPostProductInfo() */
		(productType == PROFILE_PRODUCT_HDR1000S) ||
		(productType == PROFILE_PRODUCT_FVP4000T))
	{
		switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
		case PROFILE_IPC_APKV1 : nret = nxl_apkv1_sys_getSystemID(tmpbuf); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
		case PROFILE_IPC_APKV2 : nret = nxl_apkv2_sys_getSystemID(tmpbuf); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
		case PROFILE_IPC_PROCAPI : nret = nxl_procapi_sys_getSystemID(tmpbuf); break;
#endif
		case PROFILE_IPC_NA :
		default :
			nret = NXL_ERR_UNSUPPORTED;
			break;
		}

		NXL_LOG_DBG("++[product(%d), getSystemID(%s)]\n", productType, tmpbuf);

		if (NXL_ERR_OK == nret)
		{
			/* HDR-1000S */
			if (strncmp(tmpbuf, _SYSID_PRODUCT_HDR1000S, strlen(_SYSID_PRODUCT_HDR1000S)) == 0)
			{
				productType = PROFILE_PRODUCT_HDR1000S;
				if (local_profile_setDefaultProductInfo(productType, hContext->pstProductInfo) != NXL_ERR_OK)
				{
					NXL_LOG_ERR("[NXL] Error> Unsupported product!\n");
					return NXL_ERR_UNSUPPORTED;
				}
			}
			/* HDR-1010S Freesat Gen2 White Version (WiFi embedded) : (809C7D00 ~ 809C7D0F) was assigned to FreeSat-G2 */
			else if (strncmp(tmpbuf, _SYSID_PRODUCT_HDR1010S, strlen(_SYSID_PRODUCT_HDR1010S)) == 0)
			{
				productType = PROFILE_PRODUCT_HDR1010S;
				if (local_profile_setDefaultProductInfo(productType, hContext->pstProductInfo) != NXL_ERR_OK)
				{
					NXL_LOG_ERR("[NXL] Error> Unsupported product!\n");
					return NXL_ERR_UNSUPPORTED;
				}
			}
			else if (strncmp(tmpbuf, _SYSID_PRODUCT_HDR1010S_RSV1, strlen(_SYSID_PRODUCT_HDR1010S_RSV1)) == 0)
			{
				productType = PROFILE_PRODUCT_HDR1010S;
				if (local_profile_setDefaultProductInfo(productType, hContext->pstProductInfo) != NXL_ERR_OK)
				{
					NXL_LOG_ERR("[NXL] Error> Unsupported product!\n");
					return NXL_ERR_UNSUPPORTED;
				}
			}
			else if (strncmp(tmpbuf, _SYSID_PRODUCT_HDR1010S_RSV2, strlen(_SYSID_PRODUCT_HDR1010S_RSV2)) == 0)
			{
				productType = PROFILE_PRODUCT_HDR1010S;
				if (local_profile_setDefaultProductInfo(productType, hContext->pstProductInfo) != NXL_ERR_OK)
				{
					NXL_LOG_ERR("[NXL] Error> Unsupported product!\n");
					return NXL_ERR_UNSUPPORTED;
				}
			}
			else if (strncmp(tmpbuf, _SYSID_PRODUCT_HDR1010S_RSV3, strlen(_SYSID_PRODUCT_HDR1010S_RSV3)) == 0)
			{
				productType = PROFILE_PRODUCT_HDR1010S;
				if (local_profile_setDefaultProductInfo(productType, hContext->pstProductInfo) != NXL_ERR_OK)
				{
					NXL_LOG_ERR("[NXL] Error> Unsupported product!\n");
					return NXL_ERR_UNSUPPORTED;
				}
			}
			/* HDR-1100S */
			else if (strncmp(tmpbuf, _SYSID_PRODUCT_HDR1100S, strlen(_SYSID_PRODUCT_HDR1100S)) == 0)
			{
				productType = PROFILE_PRODUCT_HDR1100S;
				if (local_profile_setDefaultProductInfo(productType, hContext->pstProductInfo) != NXL_ERR_OK)
				{
					NXL_LOG_ERR("[NXL] Error> Unsupported product!\n");
					return NXL_ERR_UNSUPPORTED;
				}
			}
			else if (strncmp(tmpbuf, _SYSID_PRODUCT_FVP5000T, strlen(_SYSID_PRODUCT_FVP5000T)) == 0)
			{
				productType = PROFILE_PRODUCT_FVP5000T;
				if (local_profile_setDefaultProductInfo(productType, hContext->pstProductInfo) != NXL_ERR_OK)
				{
					NXL_LOG_ERR("[NXL] Error> Unsupported product!\n");
					return NXL_ERR_UNSUPPORTED;
				}
			}
		}
		else
		{
			NXL_LOG_ERR("[NXL] Error> Cant get system Info!\n");
		}

		NXL_LOG_DBG("--[product(%d), getSystemID(%s)]\n", productType, tmpbuf);
	}

	hContext->isInitialized = 1;
	_nxl_module_init = 1;
	NXL_LOG_ERR("--\n");
	return nret;

/* func_error: */
	NXL_LOG_ERR("[NXL] Error> failed to initialize (product:%d)!\n", (int)productType);
	if (hContext->pstProductInfo != NULL)
	{
		free(hContext->pstProductInfo); hContext->pstProductInfo = NULL;
	}
	return NXL_ERR_UNSUPPORTED;
}

uint32_t NXL_Deinit(void)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	NXL_BoxInfo_t *pstBoxInfo = NULL;
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);
	NXL_LOG_ERR("++\n");
	__CHECK_MODULE_DEINIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nxl_apkv1_deinit(); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nxl_apkv2_deinit(); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nxl_procapi_deinit(); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}

	if (hContext->pstProductInfo != NULL)
	{
		free(hContext->pstProductInfo); hContext->pstProductInfo = NULL;
	}

	pstBoxInfo = &hContext->tBoxInfo;
	if (pstBoxInfo != NULL)
	{
		if (pstBoxInfo->model_name != NULL)
		{
			free(pstBoxInfo->model_name); pstBoxInfo->model_name = NULL;
		}
		if (pstBoxInfo->software_version != NULL)
		{
			free(pstBoxInfo->software_version); pstBoxInfo->software_version = NULL;
		}
		if (pstBoxInfo->screen_language != NULL)
		{
			free(pstBoxInfo->screen_language); pstBoxInfo->screen_language = NULL;
		}
		if (pstBoxInfo->system_id != NULL)
		{
			free(pstBoxInfo->system_id); pstBoxInfo->system_id = NULL;
		}
	}

	hContext->isInitialized = 0;
	_nxl_module_init = 0;
	NXL_LOG_ERR("--\n");
	return nret;
}

uint32_t NXL_GetApplicationID(uint32_t *pAppId)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	*pAppId = 0x0;
	if (0)
	{
		__CHECK_MODULE_INIT;
		__CHECK_NULL_PTR(pAppId);
	}

	/* This is called by partner/platform(dfb, ...) ,
	   but partner/platform(dfb, ...) can be operated asynchronously with partner/dpi.  */
	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_sys_getApplicationID(pAppId); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_sys_getApplicationID(pAppId); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_sys_getApplicationID(pAppId); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SetInputGrap(uint32_t bOn)
{
	uint32_t nret = NXL_ERR_OK;
#if 0 /* KeyGrab() cause the change of HWStyle and key froze-up for netflix relaunch */
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	if (0)
	{
		__CHECK_MODULE_INIT;
	}

	/* This is called by partner/platform(dfb, ...) ,
	   but partner/platform(dfb, ...) can be operated asynchronously with partner/dpi.  */
	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_sys_setInputGrap(bOn); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_sys_setInputGrap(bOn); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_sys_setInputGrap(bOn); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
#endif
	return nret;
}

/* System */
uint32_t NXL_SYSTEM_GetModelName(char *pBuf, uint32_t size)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	NXL_ProductInfo_t *ptProductInfo = local_profile_getProductInfo(hContext);

	__CHECK_MODULE_INIT;
	__CHECK_NULL_PTR(pBuf);

	if (ptProductInfo != NULL)
	{
		strcpy(pBuf, ptProductInfo->szlModelName);
	}
	NXL_LOG_DBG("--[model:%s]\n", pBuf);
	return nret;
}

uint32_t NXL_SYSTEM_GetOctoModelName(char *pBuf, uint32_t size)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;
	__CHECK_NULL_PTR(pBuf);

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_sys_getModelName(pBuf, size); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_sys_getModelName(pBuf, size); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_sys_getModelName(pBuf, size); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_GetSoftwareVersion(char *pBuf, uint32_t size)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;
	__CHECK_NULL_PTR(pBuf);

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_sys_getSoftwareVersion(pBuf, size); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_sys_getSoftwareVersion(pBuf, size); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_sys_getSoftwareVersion(pBuf, size); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_GetFriendlyName(char *pBuf, uint32_t size)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;
	__CHECK_NULL_PTR(pBuf);

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_sys_getDialFriendlyName(pBuf, size); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_sys_getDialFriendlyName(pBuf, size); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_sys_getDialFriendlyName(pBuf, size); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_GetLanguage(char *pBuf, uint32_t size)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;
	__CHECK_NULL_PTR(pBuf);

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 :
		{
  #if 0
			nret = nxl_apkv1_sys_getLanguage(pBuf, size);
  #else
			/* TODO :
			 * Language code should be in BCP-47 language code table
			 * And It should be matched with NRD Portal value.
			 * Please refer to https://tools.ietf.org/html/bcp47 and https://nrd.netflix.com/display/DOC40/UI+Language+Selection#UILanguageSelection-langtag
			 * Anyway, "en" tag only for netflix g1.
			 */
			strcpy(pBuf,"en"); // TODO
  #endif
		}
		break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 :
		{
  #if 0
			nret = nxl_apkv2_sys_getLanguage(pBuf, size);
  #else
			/* TODO :
			 * Language code should be in BCP-47 language code table
			 * And It should be matched with NRD Portal value.
			 * Please refer to https://tools.ietf.org/html/bcp47 and https://nrd.netflix.com/display/DOC40/UI+Language+Selection#UILanguageSelection-langtag
			 * Anyway, "en" tag only for netflix g1.
			 */

#if defined (CONFIG_OP_TDC)
            snprintf(pBuf, size, "da-DK");
#else
			strcpy(pBuf,"en"); // TODO
#endif			
  #endif
		}
		break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI :
		{
  #if 0
			nret = nxl_procapi_sys_getLanguage(pBuf, size);
  #else
			/* TODO :
			 * Language code should be in BCP-47 language code table
			 * And It should be matched with NRD Portal value.
			 * Please refer to https://tools.ietf.org/html/bcp47 and https://nrd.netflix.com/display/DOC40/UI+Language+Selection#UILanguageSelection-langtag
			 * Anyway, "en" tag only for netflix g1.
			 */
			strcpy(pBuf,"en");
  #endif
		}
		break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}

	return nret;
}

/* Audio Volume Control */
uint32_t NXL_SYSTEM_RegisterOutputVolumeListener(void *pfunc)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_registerOutputVolumeListener(pfunc); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_registerOutputVolumeListener(pfunc); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_registerOutputVolumeListener(pfunc); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_UnregisterOutputVolumeListener(void *pfunc)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_unregisterOutputVolumeListener(pfunc); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_unregisterOutputVolumeListener(pfunc); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_unregisterOutputVolumeListener(pfunc); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_CheckOutputVolumeUpdated(uint32_t *updated)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_checkLastOutputVolumeChanged(updated); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_checkLastOutputVolumeChanged(updated); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_checkLastOutputVolumeChanged(updated); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_RegisterOutputHdmiListener(void *pfunc)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_registerOutputHdmiListener(pfunc); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_registerOutputHdmiListener(pfunc); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_registerOutputHdmiListener(pfunc); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_UnregisterOutputHdmiListener(void *pfunc)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_unregisterOutputHdmiListener(pfunc); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_unregisterOutputHdmiListener(pfunc); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_unregisterOutputHdmiListener(pfunc); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_CheckOutputHdmiUpdated(uint32_t *updated)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_checkLastOutputHdmiChanged(updated); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_checkLastOutputHdmiChanged(updated); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_checkLastOutputHdmiChanged(updated); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_RegisterOutputResolutionListener(void *pfunc)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

#if 1 /* TODO */
	_NOT_USED_PARAM(nret);
	_NOT_USED_PARAM(ipcType);
	return NXL_ERR_OK;
#else
	switch(ipcType) {
  #if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_registerOutputResolutionListener(pfunc); break;
  #endif
  #if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_registerOutputResolutionListener(pfunc); break;
  #endif
  #if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_registerOutputResolutionListener(pfunc); break;
  #endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
#endif
	return nret;
}

uint32_t NXL_SYSTEM_UnregisterOutputResolutionListener(void *pfunc) /* TODO */
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

#if 1 /* TODO */
	_NOT_USED_PARAM(nret);
	_NOT_USED_PARAM(ipcType);
	return NXL_ERR_OK;
#else
	switch(ipcType) {
  #if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_unregisterOutputResolutionListener(pfunc); break;
  #endif
  #if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_unregisterOutputResolutionListener(pfunc); break;
  #endif
  #if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_unregisterOutputResolutionListener(pfunc); break;
  #endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
#endif
	return nret;
}

uint32_t NXL_SYSTEM_CheckOutputResolutionUpdated(uint32_t *updated)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_checkLastOutputResolutionChanged(updated); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_checkLastOutputResolutionChanged(updated); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_checkLastOutputResolutionChanged(updated); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_RegisterScreenSaverListener(void *pfunc)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

#if 1 /* TODO : Change with product profile because some models like hdr-1000sph2 have no screenSaver */
	_NOT_USED_PARAM(nret);
	_NOT_USED_PARAM(ipcType);
	return NXL_ERR_OK;
#else
	switch(ipcType) {
  #if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_registerScreenSaverListener(pfunc); break;
  #endif
  #if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_registerScreenSaverListener(pfunc); break;
  #endif
  #if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_registerScreenSaverListener(pfunc); break;
  #endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
#endif
	return nret;
}

uint32_t NXL_SYSTEM_UnregisterScreenSaverListener(void *pfunc)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

#if 1 /* TODO */
	_NOT_USED_PARAM(nret);
	_NOT_USED_PARAM(ipcType);
	return NXL_ERR_OK;
#else
	switch(ipcType) {
  #if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_unregisterScreenSaverListener(pfunc); break;
  #endif
  #if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_unregisterScreenSaverListener(pfunc); break;
  #endif
  #if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_unregisterScreenSaverListener(pfunc); break;
  #endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
#endif
	return nret;
}

uint32_t NXL_SYSTEM_CheckScreenSaverUpdated(uint32_t *updated) /* TODO */
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

#if 1 /* TODO */
	_NOT_USED_PARAM(nret);
	_NOT_USED_PARAM(ipcType);
	return NXL_ERR_OK;
#else
	switch(ipcType) {
  #if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_sys_checkLastOutputScreenSaverChanged(updated); break;
  #endif
  #if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_sys_checkLastOutputScreenSaverChanged(updated); break;
  #endif
  #if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_sys_checkLastOutputScreenSaverChanged(updated); break;
  #endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
#endif
	return nret;
}

uint32_t NXL_SYSTEM_CheckNetworkEventUpdated(uint32_t *updated)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_sys_checkNetworkChanged(updated); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_sys_checkNetworkChanged(updated); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_sys_checkNetworkChanged(updated); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

/* Network : WiFi  */
uint32_t NXL_SYSTEM_HasWifiDevice(uint32_t *hasWifi)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	NXL_ProductInfo_t *ptProductInfo = local_profile_getProductInfo(hContext);

	*hasWifi = 0;
	__CHECK_MODULE_INIT;
	
	if (ptProductInfo == NULL)
	{
		goto func_done;
	}
	
	if (((ptProductInfo->networkType & PROFILE_NETIF_WIFI)==PROFILE_NETIF_WIFI) ||
		((ptProductInfo->networkType & PROFILE_NETIF_WIFI_DONGLE)==PROFILE_NETIF_WIFI_DONGLE) ||
		((ptProductInfo->networkType & PROFILE_NETIF_WIFI_ETH1_BR)==PROFILE_NETIF_WIFI_ETH1_BR))
	{
		*hasWifi = 1;
	}
	
func_done:
	return nret;
}

/* Network : wifi_thru_eth1_bridge */
uint32_t NXL_SYSTEM_HasWifiEth1Bridge(uint32_t *hasWifiEth1Bridge) /* For exception case such as wifi_thru_eth1_bridge like YSR-2000 : http://svn:3000/issues/98026 */
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	NXL_ProductInfo_t *ptProductInfo = local_profile_getProductInfo(hContext);

	*hasWifiEth1Bridge = 0;
	__CHECK_MODULE_INIT;
	
	if (ptProductInfo == NULL)
	{
		goto func_done;
	}
	
	if ((ptProductInfo->networkType & PROFILE_NETIF_WIFI_ETH1_BR)==PROFILE_NETIF_WIFI_ETH1_BR)
	{
		*hasWifiEth1Bridge = 1;
	}
	
func_done:
	return nret;
}

/* Audio Volume Control */
uint32_t NXL_SYSTEM_GetVolume(uint32_t *pPercent)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_getVolumePercent(pPercent); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_getVolumePercent(pPercent); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_getVolumePercent(pPercent); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_SetVolume(uint32_t percent)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_setVolumePercent(percent); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_setVolumePercent(percent); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_setVolumePercent(percent); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_GetVolumeStep(uint32_t *pStep)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_getVolumeStep(pStep); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_getVolumeStep(pStep); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_getVolumeStep(pStep); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_IsMuted(uint32_t *pMute)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_isMuted(pMute); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_isMuted(pMute); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_isMuted(pMute); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_SetMute(uint32_t mute)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_setMute(mute); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_setMute(mute); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_setMute(mute); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

/* Video Output */
uint32_t NXL_SYSTEM_GetDigitalAudioOutputMode(uint32_t *pDigitalOut) // eNXL_AudioDigitalOutType
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_audio_getDigitalAudioOutputMode(pDigitalOut); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_audio_getDigitalAudioOutputMode(pDigitalOut); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_audio_getDigitalAudioOutputMode(pDigitalOut); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_IsDigitalAudioMultiChannel(uint32_t *pMultiChannel)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);
	eNXL_AudioDigitalOutType digitalOut;
	
	*pMultiChannel = 0;
	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 :
		if (nxl_apkv1_audio_getDigitalAudioOutputMode(&digitalOut) == NXL_ERR_OK)
		{
			if (digitalOut == NXL_AUDIO_DIGITAL_OUT_BYPASS)
			{
				*pMultiChannel = 1;
			}
		}
		break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 :

		if (nxl_apkv2_audio_getDigitalAudioOutputMode(&digitalOut) == NXL_ERR_OK)
		{
			if (digitalOut == NXL_AUDIO_DIGITAL_OUT_BYPASS)
			{
				*pMultiChannel = 1;
			}
		}

		break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI :
		if (nxl_procapi_audio_getDigitalAudioOutputMode(&digitalOut) == NXL_ERR_OK)
		{
			if (digitalOut == NXL_AUDIO_DIGITAL_OUT_BYPASS)
			{
				*pMultiChannel = 1;
			}
		}
		break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	/* return nret; */
	return nret;
}

uint32_t NXL_SYSTEM_GetSupportedVidoeOutput(void) // TODO : unused
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

#if 1 /* TODO */
	_NOT_USED_PARAM(nret);
	_NOT_USED_PARAM(ipcType);
	return 0;
#else
	switch(ipcType) {
  #if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_getSupportedVidoeOutput(); break;
  #endif
  #if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_getSupportedVidoeOutput(); break;
  #endif
  #if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_getSupportedVidoeOutput(); break;
  #endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
#endif
}

uint32_t NXL_SYSTEM_IsSupport1080pVideo(uint32_t *p1080pSupported)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);
	uint32_t uWidth, uHeight;

	__CHECK_MODULE_INIT;
	
	*p1080pSupported = 0;
	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_getOutputResolution(&uWidth, &uHeight); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_getOutputResolution(&uWidth, &uHeight); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_getOutputResolution(&uWidth, &uHeight); break;
#endif	
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	
	if ((nret == NXL_ERR_OK) && (uHeight >= 1080))
	{
		*p1080pSupported = 1;
	}
	return nret;
}

uint32_t NXL_SYSTEM_GetOutputDeviceNum(uint32_t *pnNumOfDev)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_getDeviceNum(pnNumOfDev); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_getDeviceNum(pnNumOfDev); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_getDeviceNum(pnNumOfDev); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_HasOutputScart(uint32_t *pExisted)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_isSupportScart(pExisted); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_isSupportScart(pExisted); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_isSupportScart(pExisted); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_HasOutputTvScart(uint32_t *pExisted)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_isSupportTvScart(pExisted); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_isSupportTvScart(pExisted); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_isSupportTvScart(pExisted); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_HasOutputVcrScart(uint32_t *pExisted)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_isSupportVcrScart(pExisted); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_isSupportVcrScart(pExisted); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_isSupportVcrScart(pExisted); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_HasOutputComposite(uint32_t *pExisted)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_isSupportComposite(pExisted); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_isSupportComposite(pExisted); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_isSupportComposite(pExisted); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_HasOutputComponent(uint32_t *pExisted)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_isSupportComponent(pExisted); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_isSupportComponent(pExisted); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_isSupportComponent(pExisted); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_HasOutputRF(uint32_t *pExisted)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_isSupportRF(pExisted); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_isSupportRF(pExisted); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_isSupportRF(pExisted); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_HasOutputHDMI(uint32_t *pExisted, eNXL_HdcpVer *pHdcpVer)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	*pExisted = 0; *pHdcpVer = NXL_HDCP_VERSION_NA;
	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_isSupportHDMI(pExisted, pHdcpVer); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_isSupportHDMI(pExisted, pHdcpVer); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_isSupportHDMI(pExisted, pHdcpVer); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_HasOutputSpdif(uint32_t *pExisted)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_isSupportSpdif(pExisted); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_isSupportSpdif(pExisted); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_isSupportSpdif(pExisted); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_IsActiveOutputHDMI(uint32_t *pActivated, eNXL_HdcpVer *pHdcpVer)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	*pActivated = 0; *pHdcpVer = NXL_HDCP_VERSION_NA;
	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_isActiveOutputHDMI(pActivated, pHdcpVer); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_isActiveOutputHDMI(pActivated, pHdcpVer); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_isActiveOutputHDMI(pActivated, pHdcpVer); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_IsActiveOutputSpdif(uint32_t *pActivated)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_isActiveOutputSpdif(pActivated); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_isActiveOutputSpdif(pActivated); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_isActiveOutputSpdif(pActivated); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_GetHdcpStatus(uint32_t *pbOn)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_getHdcpStatus(pbOn); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_getHdcpStatus(pbOn); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_getHdcpStatus(pbOn); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_SetHdcpStatus(uint32_t bOn, uint32_t bForce)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_setHdcpStatus(bOn, bForce); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_setHdcpStatus(bOn, bForce); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_setHdcpStatus(bOn, bForce); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_GetHdcpVersion(eNXL_HdcpVer *pHdcpVer)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_getHdcpVersion(pHdcpVer); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_getHdcpVersion(pHdcpVer); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_getHdcpVersion(pHdcpVer); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_GetCgmsStatus(eNXL_CopyRight *pCgmsStatus)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_getCgmsStatus(pCgmsStatus); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_getCgmsStatus(pCgmsStatus); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_getCgmsStatus(pCgmsStatus); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_SetCgmsStatus(eNXL_CopyRight eCgmsStatus)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_setCgmsStatus(eCgmsStatus); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_setCgmsStatus(eCgmsStatus); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_setCgmsStatus(eCgmsStatus); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_GetEdidDisplayResolution(uint32_t *puDisplayResX, uint32_t *puDisplayResY)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_getEdidDisplayResolution(puDisplayResX, puDisplayResY); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_getEdidDisplayResolution(puDisplayResX, puDisplayResY); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_getEdidDisplayResolution(puDisplayResX, puDisplayResY); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_GetActiveVideoOutput(uint32_t ulDevIdx, uint32_t *pActivated)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_getActiveVideoOutput(); break; 
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_getActiveVideoOutput(); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_getActiveVideoOutput(); break; 
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_SYSTEM_GetVideoOutputResolution(uint32_t *pWidth, uint32_t *pHeight)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;
	__CHECK_NULL_PTR(pWidth);
	__CHECK_NULL_PTR(pHeight);

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_output_getOutputResolution(pWidth, pHeight); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_output_getOutputResolution(pWidth, pHeight); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_output_getOutputResolution(pWidth, pHeight); break;
#endif	
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

/* Screen Saver */
uint32_t NXL_SYSTEM_HasScreenSaver(uint32_t *hasScreenSaver)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	NXL_ProductInfo_t *ptProductInfo = local_profile_getProductInfo(hContext);
	/* eProfile_IpcType ipcType = local_profile_getProductIPC(hContext); */

	*hasScreenSaver = 0;
	__CHECK_MODULE_INIT;

	if ((ptProductInfo != NULL) && (ptProductInfo->hasScreenSaver))
	{
		*hasScreenSaver = 1; /* or check the CONFIG_APUI_SCREEN_SAVER defconfig */
	}
	
	/*
	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_sys_hasScreenSaver(hasScreenSaver); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_sys_hasScreenSaver(hasScreenSaver); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_sys_hasScreenSaver(hasScreenSaver); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	*/
	return nret;
}

uint32_t NXL_SYSTEM_IsScreensaverOn(uint32_t *onScreenSaver)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	NXL_ProductInfo_t *ptProductInfo = local_profile_getProductInfo(hContext);
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	*onScreenSaver = 0;
	__CHECK_MODULE_INIT;

	if ((ptProductInfo != NULL) && (!ptProductInfo->hasScreenSaver))
	{
		*onScreenSaver = 0;
	}

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_sys_isScreensaverOn(onScreenSaver); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_sys_isScreensaverOn(onScreenSaver); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_sys_isScreensaverOn(onScreenSaver); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}


/* ES Manager */
uint32_t NXL_ESMANAGER_GetResourceMap(void *pMap) /* 현재는 Group1/2/3 동일하나 달라질 수 있는 부분 */
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_mediapb_getResourceMap(pMap); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_mediapb_getResourceMap(pMap); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_mediapb_getResourceMap(pMap); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

/* Playback Group */
uint32_t NXL_PLAYBACKGROUP_RegisterNotifier(void *pFunc)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_mediapb_registerNotifier(pFunc); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_mediapb_registerNotifier(pFunc); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_mediapb_registerNotifier(pFunc); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_PLAYBACKGROUP_UnregisterNotifier(void *pFunc)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_mediapb_unregisterNotifier(pFunc); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_mediapb_unregisterNotifier(pFunc); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_mediapb_unregisterNotifier(pFunc); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_PLAYBACKGROUP_MediaPbStart(void *pProbeInfo)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_mediapb_start(pProbeInfo); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_mediapb_start(pProbeInfo); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_mediapb_start(pProbeInfo); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_PLAYBACKGROUP_MediaPbStop(void)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_mediapb_stop(); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_mediapb_stop(); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_mediapb_stop(); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

/* uint32_t NXL_PLAYBACKGROUP_GetNotifyArgs(uint32_t p1, void *pEvent) */
uint32_t NXL_PLAYBACKGROUP_GetNotifyArgs(uint32_t ulViewId, uint32_t ulSessionId, uint32_t eType, uint32_t eEvent, void *pAVCtrlEvent)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_mediapb_getNotifyArgs(p1, pEvent); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_mediapb_getNotifyArgs(ulViewId, ulSessionId, eType, eEvent, pAVCtrlEvent); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_mediapb_getNotifyArgs(p1, pEvent); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_PLAYBACKGROUP_SetPlaySpeed(int32_t nSpeedSec)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_mediapb_setSpeed(nSpeedSec); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_mediapb_setSpeed(nSpeedSec); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_mediapb_setSpeed(nSpeedSec); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_PLAYBACKGROUP_Jump(uint32_t unPlayTime)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_mediapb_setPosition(unPlayTime); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_mediapb_setPosition(unPlayTime); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_mediapb_setPosition(unPlayTime); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_PLAYBACKGROUP_GetPumpIndex(uint32_t *punIndex)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_mediapb_getPumperIndex(punIndex); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_mediapb_getPumperIndex(punIndex); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_mediapb_getPumperIndex(punIndex); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_PLAYBACKGROUP_GetPlayPosition(uint32_t *punPosition)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_mediapb_getPosition(punPosition); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_mediapb_getPosition(punPosition); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_mediapb_getPosition(punPosition); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_PLAYBACKGROUP_GetMediaCtrlPlayState(void *pState)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_mediapb_getPlayState(pState); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_mediapb_getPlayState(pState); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_mediapb_getPlayState(pState); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_PLAYBACKGROUP_GetMediaCtrlBufferState(void *pState)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = NXL_ERR_UNSUPPORTED; break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_mediapb_getBufferState(pState); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = NXL_ERR_UNSUPPORTED; break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_PLAYBACKGROUP_CheckLastPlayCmd(uint32_t *pCmd) /* NXL_PLAYBACKGROUP_CMD_e */
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	*pCmd = 0;
	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_mediapb_checkLastPlayCmd(pCmd); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_mediapb_checkLastPlayCmd(pCmd); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_mediapb_checkLastPlayCmd(pCmd); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_PLAYBACKGROUP_InformLastPlayCmd(uint32_t eCmd) /* NXL_PLAYBACKGROUP_CMD_e */
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_mediapb_informLastPlayCmd(eCmd); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_mediapb_informLastPlayCmd(eCmd); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_mediapb_informLastPlayCmd(eCmd); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_PLAYBACKGROUP_SetDuration(double dblCurrentMaxPts)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_mediapb_setDuration(dblCurrentMaxPts); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_mediapb_setDuration(dblCurrentMaxPts); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_mediapb_setDuration(dblCurrentMaxPts); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_PLAYBACKGROUP_AudioFlush(uint32_t unTryToChangeAudioTrack)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_mediapb_audioFlush(unTryToChangeAudioTrack); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_mediapb_audioFlush(unTryToChangeAudioTrack); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_mediapb_audioFlush(unTryToChangeAudioTrack); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_PLAYBACKGROUP_AudioRestart()
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_mediapb_audioRestart(); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_mediapb_audioRestart(); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_mediapb_audioRestart(); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_PLAYBACKGROUP_AudioCodecSwitch(uint32_t unTryToChangeAudioTrack, int32_t nAudioCodec)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_mediapb_audioCodecSwitch(unTryToChangeAudioTrack, nAudioCodec); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_mediapb_audioCodecSwitch(unTryToChangeAudioTrack, nAudioCodec); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_mediapb_audioCodecSwitch(unTryToChangeAudioTrack, nAudioCodec); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_PLAYBACKGROUP_NotifyStreamCurrentPAR(uint32_t pixelAspectRatioX, uint32_t pixelAspectRatioY)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_mediapb_notifyStreamCurrentPAR(pixelAspectRatioX, pixelAspectRatioY); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_mediapb_notifyStreamCurrentPAR(pixelAspectRatioX, pixelAspectRatioY); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_mediapb_notifyStreamCurrentPAR(pixelAspectRatioX, pixelAspectRatioY); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_PLAYBACKGROUP_NotifyStreamCurrentSize(uint32_t imageWidth, uint32_t imageHeight)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_mediapb_notifyStreamCurrentSize(imageWidth, imageHeight); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_mediapb_notifyStreamCurrentSize(imageWidth, imageHeight); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_mediapb_notifyStreamCurrentSize(imageWidth, imageHeight); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_PLAYBACKGROUP_NotifyStreamMaxSize(uint32_t maxImageWidth, uint32_t maxImageHeight)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_mediapb_notifyStreamMaxSize(maxImageWidth, maxImageHeight); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_mediapb_notifyStreamMaxSize(maxImageWidth, maxImageHeight); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_mediapb_notifyStreamMaxSize(maxImageWidth, maxImageHeight); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_PLAYBACKGROUP_SetVideoSize(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_mediapb_setVideoSize(x, y, width, height); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_mediapb_setVideoSize(x, y, width, height); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_mediapb_setVideoSize(x, y, width, height); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_PLAYBACKGROUP_ResetVideoSize(void)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_mediapb_resetVideoSize(); break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_mediapb_resetVideoSize(); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_mediapb_resetVideoSize(); break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
	return nret;
}

uint32_t NXL_PLAYBACKGROUP_ResetPlayback(void)
{
	uint32_t nret = NXL_ERR_OK;
#if 0 /* Commented by insufficient verification (this action doesn't ensure) */
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;
	
	switch(ipcType) {
  #if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : nret = nxl_apkv1_mediapb_resetPlayback(); break;
  #endif
  #if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_mediapb_resetPlayback(); break;
  #endif
  #if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : nret = nxl_procapi_mediapb_resetPlayback(); break;
  #endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}
#endif
	return nret;
}

uint32_t NXL_PLAYBACKGROUP_SetPlayerType(void)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_mediapb_setPlayerType(); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}

	return nret;
}

uint32_t NXL_PLAYBACKGROUP_SetFade(uint32_t unStartVolume, uint32_t unTargetVolume, int32_t nEase, int32_t nEaseTimeElapsed, int32_t nEaseTransitionFrames)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_mediapb_setFade(unStartVolume, unTargetVolume, nEase, nEaseTimeElapsed, nEaseTransitionFrames); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}

	return nret;
}

uint32_t NXL_PLAYBACKGROUP_GetAudioDecoderVolume(uint32_t *punVolume)
{
	uint32_t nret = NXL_ERR_OK;
	NXL_ContextInfo_t *hContext = local_getContext();
	eProfile_IpcType ipcType = local_profile_getProductIPC(hContext);

	__CHECK_MODULE_INIT;

	switch(ipcType) {
#if defined(__NXLINK_IPC_APKV1__)
	case PROFILE_IPC_APKV1 : break;
#endif
#if defined(__NXLINK_IPC_APKV2__)
	case PROFILE_IPC_APKV2 : nret = nxl_apkv2_mediapb_getAudioDecoderVolume(punVolume); break;
#endif
#if defined(__NXLINK_IPC_PROCAPI__)
	case PROFILE_IPC_PROCAPI : break;
#endif
	case PROFILE_IPC_NA :
	default :
		nret = NXL_ERR_UNSUPPORTED;
		break;
	}

	return nret;
}

/* TODO: move to profile */
static int32_t NXL_UTIL_StrCaseCmp (const char *s1, const char *s2)
{
	int32_t	ch1;
	int32_t	ch2;

	do
	{
		ch1	= toupper(*s1++);
		ch2	= toupper(*s2++);
	} while (ch1 == ch2 && ch1 != 0);

	return ch1 - ch2;
}

static int32_t NXL_UTIL_StrNCaseCmp (const char *s1, const char *s2, uint32_t n)
{
	int32_t	ch1;
	int32_t	ch2;

	while (n)
	{
		ch1	= toupper(*s1++);
		ch2	= toupper(*s2++);

		if (ch1 != ch2)
			return ch1 - ch2;

		if (ch1 == 0)
			break;

		n--;
	}
	return 0;
}

static char *NXL_UTIL_StrCaseStr(const char *haystack, const char *needle)
{
	const char *p;
	const char *startn = NULL;
	const char *np = NULL;

	for (p = haystack ; *p ; p++)
	{
		if (np)
		{
			if (toupper(*p) == toupper(*np))
			{
				if (!*++np)
					return (char *)startn;
			}
			else
			{
				np = NULL;
			}
		}
		else if (toupper(*p) == toupper(*needle))
		{
			np = needle + 1;
			startn = p;
			if ( strlen( needle ) == 1 )
			{/* only 1 byte ch not found - a ~ z , 1 ~ 0 */
				return (char *)startn;
			}
		}
	}
	return NULL;
}
