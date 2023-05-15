/**
 * @note
 * Copyright (C) 2010-2011 Humax Corporation. All Rights Reserved. <br>
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute
 * this software except in compliance with the terms and conditions
 * of any applicable license agreement in writing between Humax
 * Corporation and you.
 */

/**
 * @defgroup DLNA DLNA Core
 *
 * 이 모듈은 Broadcom DLNA 관련 API 를 이용한 DLNA interface 를 제공한다.
 *
 */

/**
 * @author kimdh1@humaxdigital.com
 */

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "expat.h"
#include "expat_external.h"

#include "vkernel.h"
#include "taskdef.h"
#include "drv_err.h"
#include "di_dlna.h"
#include "di_audio.h"
#include "drv_audio.h"
#include "di_video.h"
#include "drv_video.h"
#include "drv_pvr.h"
#include "drv_demux.h"
#include "bdlna.h"
#include "upnp.h"
#include "b_playback_ip_lib.h"
#include "b_os_lib.h"
#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_audio_output.h"
#include "nexus_audio_mixer.h"
#include "nexus_stc_channel.h"
#include "nexus_timebase.h"
#include "nexus_playpump.h"
#include "nexus_playback.h"

//#include "b_os_lib.h"
/*@{*/

/**
 * @file DI_dlna.c
 *
 */


#define DI_DLNA_DEBUG
//#define DI_DLNA_CRITICAL_DEBUG

#ifdef DI_DLNA_DEBUG
#define PrintData		VK_printf
#define PrintDebug		VK_printf
#define PrintError		VK_printf
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		VK_printf
#endif

#ifdef DI_DLNA_DEBUG
#define PrintEnter		VK_printf("(+)%s\n", __FUNCTION__)
#define PrintExit		VK_printf("(-)%s\n", __FUNCTION__)
#else
#define PrintEnter
#define PrintExit
#endif

#ifdef DI_DLNA_CRITICAL_DEBUG
#define PrintCriticalDebug		VK_printf
#else
#define PrintCriticalDebug		while (0) ((int (*)(char *, ...)) 0)
#endif

#define MAX_NUMBER_OF_PROPERTY			100

#define DI_DLNA_MSG_SIZE			MSGQ_SIZE_NORMAL

#define DI_DLNA_PRECHARGETIME		(5)

/* Define for DMS */
#define MAX_DLNA_NETWORKINTERFACELENG 12

#define MAX_DLNA_HUMAXDMS_PORT				4572

#define MAX_FILE_PATH 1024

#define _DI_DLNA_DMS_BRCM_DIR_LEN                128 // Brcm use too short length
#define	_DI_DLNA_DMS_DEFAULT_DEVICE_UDN          "uuid:0E96FF1A-6771-2F44-A0C5-000102030405"
#define _DI_DLNA_DMS_TASK_PRIORITY               AUDIO_CLIP_TASK_PRIORITY
#define _DI_DLNA_DMS_STACK_SIZE                  (64*1024)
#define _DI_DLNA_DMS_TASK_START_WATING_TIME      (300)
#define _DI_DLNA_DMS_PORT_NUM                    (2345)
#define _DI_DLNA_DMS_SSDP_REFRESH                (1800) /* sec */

#define UNUSED_PARAMETER(x)	(void)x

typedef enum DLNA_SCAN_ITEM_RESULT_{
	SCAN_ITEM_FILE=0,
	SCAN_ITEM_DIR=1,
	SCAN_ITEM_IGNORE=2
}DI_DLNA_SCAN_ITEM_RESULT;

typedef struct DLNA_SCAN_ITEM
{
	char *DirPathToScan; 		/* Full Path Of the Directory For Scanning*/
	struct DLNA_SCAN_ITEM *next;	/* List pointing to next directory to scan*/
}DI_DLNA_SCAN_ITEM, *DI_DLNA_PSCAN_ITEM;

typedef enum
{
	DI_DLNA_CORE_NA = 0,
	DI_DLNA_CORE_INIT,
	DI_DLNA_CORE_STOPPED,
	DI_DLNA_CORE_STARTED
} DI_DLNA_CORE_STATE;

typedef enum
{
	DI_DLNA_DEVICE_DMS,
	DI_DLNA_DEVICE_DMP,
	/*
	DI_DLNA_DEVICE_DMR,
	DI_DLNA_DEVICE_DMC,
	*/
	DI_DLNA_DEVICE_INVALID
} DI_DLNA_DEVICE_TYPE;

typedef struct
{
	unsigned long		ulSender;
	unsigned long		ulStreamHandle;
	unsigned long	   	ulEvent;
	unsigned char		*pucParam;
	unsigned long		ulParam2;
	unsigned long		ulParam3;
}DI_DLNA_MSG;

/* Start static variablee */
/** Static Media Semaphore Id */
static unsigned long				s_ulDlnaSemId = 0;
#if 0
/** Static DLNA Task Id */
static unsigned long				s_ulDlnaTaskId = 0;
/** Static DLNA Msg Id */
static unsigned long				s_ulDlnaMId = 0;
/** Static DLNA Event Id */
static unsigned long				s_ulDlnaEId = 0;
#endif
static BDlna_DmpHandle 				s_hDmp = NULL;
static HINT32 						s_untotal_device_count = 0;
static DI_DLNA_CORE_STATE			s_eDlnaCoreState = DI_DLNA_CORE_NA;
static HBOOL						s_bDmpCoreInited = FALSE;
static HBOOL						s_bDmsCoreInited = FALSE;

static struct DmpDeviceHandle		*s_DLNA_DeviceList=NULL;

static DI_DLNA_DeviceCallback		s_DmpDeviceCallback = NULL;
static DI_DLNA_ContentCallback		s_DmpContentCallback = NULL;

static HUINT32 						s_untotal_contents_count = 0;
static DI_DLNA_Content				*s_pHnContents = NULL;

static DI_DLNA_Metadata				*s_tItemMetadata = NULL;
static DI_DLNA_MetadataAttr			*s_tItemAttrs = NULL;

//static DI_DLNA_MEDIA_EventCallback	s_tDlnaMediaEventCallback	= NULL;
static DI_DLNA_NetworkInterface		s_tNetworkInfo;

/* XML 파서를 위한 데이터 */
static HUINT32						s_unTagDepth;
static HUINT32						s_uns_unIs_value;
static HUINT8						*s_pCurrent_Property;   // 가장 최근 태그 기억용
static HUINT32						s_unPropertyCount;
HUINT8								**s_ppCurrent_Attr;  // 태그의 특성값 기억용

/* DMS DI Handle */
struct stDMS_DI_RUNNING_THREAD
{
	pthread_t taskID;
	HBOOL bThreadError;        // FALSE;
	HBOOL bBrcmScanRunning;    // FALSE; Internal scanning after BDlna_Dms_Create is finished.
	HBOOL bStartTaskRunning;   // FALSE; Whether this startTask is finished
};

typedef struct stDMS_DI_HANDLE
{
	BDlna_Dms_OpenSettings          *pDmsOpenSettings;
	BUPnPOpenSettings               *pUPnPOpenSettings;
	BDlnaDmsHandle                  hBrcmDms;
	HUINT8                          bBrcmInternalTaskActivated;   // FALSE; Whether bdlna() is started, or not
	/*
	DMS_SERVICE_STATE_e             eServicetate;                 // FALSE;
	HUINT8                          bDmsQuit;                     // FALSE;
	HBOOL                           bDmsPeriodicTaskFinished;      // FALSE;
	*/

	struct stDMS_DI_RUNNING_THREAD *hStartThread;
#if 0
	struct stDMS_DI_RUNNING_THREAD *hStopThread;
#endif
} DMS_DI_HANDLE;

DMS_DI_HANDLE *_ghDmsDiHandle = NULL;

/*
 * xml (태그가 아닌) 데이터를 만났을 때 호출된다.
 * 실제 파싱 작업은 여기서 하며 Metadata 값을 채워야 한다.
 */
static void P_DLNA_XML_DataParser(void *data, const char *s, int len)
{
    int i = 0;
    int Attr_count = 0;
    int nParserIndex =  0;
    char buf[1024] = {0x00, };

	UNUSED_ARGUMENT(data);

    // 개행,탭문자 제거
    for (i = 0; i < len; i++)
    {
        if (s[i] != '\n' && s[i] != '\t' && i < len)
        {
            buf[nParserIndex] = s[i];
            nParserIndex++;
        }
    }

    if (s_uns_unIs_value && s_pCurrent_Property && (buf[0] != 0x00))
    {
    	if ( s_unPropertyCount > MAX_NUMBER_OF_PROPERTY )
    	{
    		PrintError("[---- MAX Property count Excceed!!!");
    		return;
    	}
    	/* Found Property */
    	if ( s_tItemMetadata == NULL )
    	{
    		/* Create Grand Grand Parents */
    		s_tItemMetadata = (DI_DLNA_Metadata *)calloc( 1, sizeof(DI_DLNA_Metadata));
        	if (s_tItemMetadata == NULL )
        	{
        		PrintError("[---- MEM calloc failed!!!");
        	}
    	}

    	s_tItemMetadata[s_unPropertyCount].key = VK_StrDup(s_pCurrent_Property);
    	s_tItemMetadata[s_unPropertyCount].value = VK_StrDup(buf);
    	if (s_unPropertyCount == 0 )
    	{
    		s_tItemMetadata[s_unPropertyCount].parent = NULL;
        	s_tItemMetadata[0].child_cnt = 0; /* 나머지의 개수를 아직 모름. */
        	s_tItemMetadata[s_unPropertyCount].childs = NULL;

    	}
    	else
    	{
    		/* 현재 프로퍼티의 부모는 이전 프로퍼티 */
    		s_tItemMetadata[s_unPropertyCount].parent = &s_tItemMetadata[s_unPropertyCount-1];
    		/* Child 개수는 Property count 와 같음. 최상위에만 저장 */
        	s_tItemMetadata[0].child_cnt = s_unPropertyCount; /* 자신의 개수를 포함하지 않은 나머지 Metadata 의 개수 */
        	/* 이전 */
        	s_tItemMetadata[s_unPropertyCount-1].childs = &s_tItemMetadata[s_unPropertyCount];

    	}

        //VK_printf("Property = %s, value = %s\n", s_pCurrent_Property, buf );

        /* ATTR 초기화 - Property 마다 ATTR 다르기 때문에  */
        s_tItemAttrs = NULL;
    	while ( s_ppCurrent_Attr[Attr_count*2] != NULL )
    	{
        	/* Found Attr */
        	if ( s_tItemAttrs == NULL )
        	{
        		/* Create Grand Grand Parents */
        		s_tItemAttrs = (DI_DLNA_MetadataAttr *)calloc( 1, sizeof(DI_DLNA_MetadataAttr));
            	if (s_tItemAttrs == NULL )
            	{
            		PrintError("[---- MEM calloc failed!!!");
            	}
        	}
        	else
        	{
        		s_tItemAttrs = (DI_DLNA_MetadataAttr *)realloc(s_tItemAttrs, sizeof(DI_DLNA_MetadataAttr) + sizeof(DI_DLNA_MetadataAttr) *  Attr_count );
            	if (s_tItemAttrs == NULL )
            	{
            		PrintError("[---- MEM realloc failed!!!");
            	}
        	}

        	s_tItemAttrs[Attr_count].key = VK_StrDup(s_ppCurrent_Attr[Attr_count*2]);
        	s_tItemAttrs[Attr_count].value = VK_StrDup(s_ppCurrent_Attr[Attr_count*2+1]);

    		//VK_printf("%s = %s\n", s_ppCurrent_Attr[Attr_count*2], s_ppCurrent_Attr[Attr_count*2+1] );
    		Attr_count++;
    	}

    	if ( s_tItemAttrs != NULL )
    	{
    		s_tItemMetadata[s_unPropertyCount].attr_cnt = Attr_count;
    		s_tItemMetadata[s_unPropertyCount].attrs = s_tItemAttrs;
    	}
    	else
    	{
    		s_tItemMetadata[s_unPropertyCount].attr_cnt = 0;
    		s_tItemMetadata[s_unPropertyCount].attrs = NULL;
    	}
    	s_tItemMetadata[s_unPropertyCount].ns_cnt = 0; /* NAME SPACE ATTR 없음*/
    	s_tItemMetadata[s_unPropertyCount].ns = NULL;

    	s_unPropertyCount++;
    }
}


/**
 * xml (여는)태그를 만났을 때 호출되는 함수다. depth를 1증가 시키고, depth가 0즉 처음 만나나는 테그라면
 * version을 검사한다. 물론 테그(el)를 직접 비교해도 될 것이다.
 */
static void P_DLNA_XML_StartTagParser(void *data, const char *el, const char **attr)
{
	s_uns_unIs_value = 1;
	HUINT32 Attr_count = 0;

	UNUSED_ARGUMENT(data);

	/* Current Property 와 Attr 을 저장하여 Parse 에서 처리할 수 있도록 함 */
	s_pCurrent_Property = (HUINT8*)el;
	s_ppCurrent_Attr = (HUINT8**)attr;

	if(s_unTagDepth == 0)
	{
		/* 첫 Depth 태그는 item or container 이고 value 는 NULL 이지만 ATTR 은 존재함. */
		if ( s_pCurrent_Property != NULL )
		{
    		/* Create Grand Grand Parents */
    		s_tItemMetadata = (DI_DLNA_Metadata *)calloc( MAX_NUMBER_OF_PROPERTY, sizeof(DI_DLNA_Metadata));
        	if (s_tItemMetadata == NULL )
        	{
        		PrintError("[---- MEM calloc failed!!!");
        	}

        	s_tItemMetadata[s_unPropertyCount].key = VK_StrDup(s_pCurrent_Property);
        	s_tItemMetadata[s_unPropertyCount].value = NULL;

    		s_tItemMetadata[s_unPropertyCount].parent = NULL;
        	s_tItemMetadata[0].child_cnt = 0; /* 나머지의 개수를 아직 모름. */
        	s_tItemMetadata[s_unPropertyCount].childs = NULL; /* 나머지의 포인터를 모름 */

        	s_tItemAttrs = NULL;
    		while ( s_ppCurrent_Attr[Attr_count*2] != NULL )
    		{
            	/* Found Attr */
            	if ( s_tItemAttrs == NULL )
            	{
            		/* Create Grand Grand Parents */
            		s_tItemAttrs = (DI_DLNA_MetadataAttr *)calloc( 1, sizeof(DI_DLNA_MetadataAttr));
                	if (s_tItemAttrs == NULL )
                	{
                		PrintError("[---- MEM calloc failed!!!");
                	}
            	}
            	else
            	{
            		s_tItemAttrs = (DI_DLNA_MetadataAttr *)realloc(s_tItemAttrs, sizeof(DI_DLNA_MetadataAttr) + sizeof(DI_DLNA_MetadataAttr) *  Attr_count );
                	if (s_tItemAttrs == NULL )
                	{
                		PrintError("[---- MEM realloc failed!!!");
                	}
            	}

            	s_tItemAttrs[Attr_count].key = VK_StrDup(s_ppCurrent_Attr[Attr_count*2]);
            	s_tItemAttrs[Attr_count].value = VK_StrDup(s_ppCurrent_Attr[Attr_count*2+1]);

        		//VK_printf("%s = %s\n", s_ppCurrent_Attr[Attr_count*2], s_ppCurrent_Attr[Attr_count*2+1] );
        		Attr_count++;
    		}
        	if ( s_tItemAttrs != NULL )
        	{
        		s_tItemMetadata[s_unPropertyCount].attr_cnt = Attr_count;
        		s_tItemMetadata[s_unPropertyCount].attrs = s_tItemAttrs;
        	}
        	else
        	{
        		s_tItemMetadata[s_unPropertyCount].attr_cnt = 0;
        		s_tItemMetadata[s_unPropertyCount].attrs = NULL;
        	}
        	s_tItemMetadata[s_unPropertyCount].ns_cnt = 0; /* NAME SPACE ATTR 없음*/
        	s_tItemMetadata[s_unPropertyCount].ns = NULL;

        	s_unPropertyCount++;
		}
	}
	s_unTagDepth++;
}

/**
 * xml (닫는)태그를 만났을 때 호출되는 함수다. 닫히게 되므로 depth가 1감소할 것이다.
 */
static void P_DLNA_XML_EndTagParser(void *data, const char *el)
{
	UNUSED_ARGUMENT(data);
	UNUSED_ARGUMENT(el);

    s_uns_unIs_value = 0;
    s_unTagDepth--;
}
#if 0
static void P_DLNA_MEDIA_PrintResource(struct BMedia* hPrintNexus)
{
	PrintData("[hPrintNexus]   (%x)\n", hPrintNexus);

	PrintData("[PALTFS-DISPLAY]   (%x)\n", &hPrintNexus->platformSettings.displayModuleSettings);
	PrintData("[PALTFS-AUDIO]   (%x)\n", &hPrintNexus->platformSettings.audioModuleSettings);
	PrintData("[PALTFS-VIDEO]   (%x)\n", &hPrintNexus->platformSettings.videoDecoderModuleSettings);
	PrintData("[PALTFS-SURFACE]   (%x)\n", &hPrintNexus->platformSettings.surfacePlatformSettings);
	PrintData("[PALTFS-SMART]   (%x)\n", &hPrintNexus->platformSettings.smartCardSettings);

	PrintData("[PALTFS]   (%x)\n", &hPrintNexus->platformSettings);

	PrintData("[PLATFC-COMPONENT] (%x)\n", hPrintNexus->platformConfig.outputs.component);
	PrintData("[PLATFC-COMPOSITE] (%x)\n", hPrintNexus->platformConfig.outputs.composite);
	PrintData("[PLATFC-SPDIF] (%x)\n", hPrintNexus->platformConfig.outputs.spdif);

	PrintData("[PLATFC] (%x)\n", &hPrintNexus->platformConfig);
	PrintData("[PLAYPUMP]   (%x)\n", hPrintNexus->playpump);
	PrintData("[STCCHANNEL] (%x)\n", hPrintNexus->stcChannel);
	PrintData("[VIDDEC]     (%x)\n", hPrintNexus->videoDecoder);
	PrintData("[PCMDEC]     (%x)\n", hPrintNexus->pcmDecoder);
	PrintData("[COMPDEC]    (%x)\n", hPrintNexus->compressedDecoder);
	PrintData("[PLAYBACK]   (%x)\n", hPrintNexus->playback);
	PrintData("[PLAYBACKIP] (%x)\n", hPrintNexus->playbackIp);
	PrintData("[PLATFORMC]   (%x)\n", &hPrintNexus->platformConfig);
	PrintData("[PLATFORMS] (%x)\n", &hPrintNexus->platformSettings);
	PrintData("[DISPLAY] (%x)\n", hPrintNexus->display);
	PrintData("[VIDEODECORDER_SETTING] (%x)\n", &hPrintNexus->videoDecoderSettings);
	PrintData("[AUDIODECORDER_SETTING] (%x)\n", &hPrintNexus->audioDecoderSettings);
	PrintData("[PLAYBACKIP_SETTING] (%x)\n", &hPrintNexus->playbackIpSettings);
	PrintData("[PLAYBACK_SETTING] (%x)\n", &hPrintNexus->playbackSettings);
	PrintData("[AUDOUTPUT] (%x)\n", hPrintNexus->output);
	PrintData("[PBAS] (%x)\n", &hPrintNexus->playbackSettings);
}

void P_DLNA_UriChangedCallback(void* context, int param)
{
	UNUSED_ARGUMENT(context);
	UNUSED_ARGUMENT(param);
	PrintData("App got Uri Changed callback");
}

void P_DLNA_EndOfStreamCallback(void* context, int param)
{
	UNUSED_ARGUMENT(context);
	UNUSED_ARGUMENT(param);
	PrintData("App got End of stream callback");
}

void P_DLNA_ErrorOfCallback(void* context, int param)
{
	UNUSED_ARGUMENT(context);
	UNUSED_ARGUMENT(param);
	PrintData("App got Error of callback");
}

void P_DLNA_PlayStopCallback(void* context, int param)
{
	UNUSED_ARGUMENT(context);
	UNUSED_ARGUMENT(param);
	PrintData("App got Play Stop callback");
}

void P_DLNA_PlayReadyCallback(void* context, int param)
{
	UNUSED_ARGUMENT(context);
	UNUSED_ARGUMENT(param);
	PrintData("App got Play Ready callback");
}

void P_DLNA_MEDIA_MmiStateChangeCallback(BDlna_MmiHandle hMmi, void* context, BDlna_MediaState state)
{
	UNUSED_ARGUMENT(hMmi);
	UNUSED_ARGUMENT(context);
	PrintData("----INT_DLNA_MEDIA_MmiStateChangeCallback State Changed! %d\n", state);
}

void P_DLNA_MEDIA_MmiUriChangedCallback(BDlna_MmiHandle hMmi, void* context, const char* uri)
{
	UNUSED_ARGUMENT(hMmi);
	UNUSED_ARGUMENT(context);
	PrintData("----INT_DLNA_MEDIA_MmiUriChangedCallback Uri Changed! %s\n", uri);
}

void P_DLNA_MEDIA_MmiPreparePlaybackCallback(BDlna_MmiHandle hMmi, void* context, const char* uri)
{
	UNUSED_ARGUMENT(hMmi);
	UNUSED_ARGUMENT(context);
	PrintData("----INT_DLNA_MEDIA_MmiPreparePlaybackCallback Uri = %s\n", uri);
}
#endif
struct DmpDeviceHandle
{
	BDlna_DmpDeviceHandle hDevice;
	struct DmpDeviceHandle* next;
};

static void P_DLNA_Dmp_DeviceAdded(void *context, const char* uuid)
{
	BDlna_Error BDlna_ret = BDLNA_E_SUCCESS;
	struct DmpDeviceHandle* h = NULL;
	BUPnPDeviceInfo deviceInfo;
	DI_DLNA_DeviceEvent	tDeviceEvent; 		/* 캡터에게 알려주는 Devie Event */
	DI_DLNA_Device tDeviceInfo;

	PrintEnter;

	UNUSED_PARAMETER(context);

	/* BDLNA Device 신규 등록 및 Linked List 연결 */
	h = (struct DmpDeviceHandle*)DD_MEM_Calloc(sizeof(*h));
	h->hDevice = BDlna_Dmp_CreateDevice(uuid);
	PrintData("[%s] Added Device = %s\n", __FUNCTION__, uuid);

	BDlna_ret = BDlna_Dmp_GetDeviceInfo(s_hDmp, h->hDevice, &deviceInfo);
	PrintData("---- deviceInfo.friendlyName = %s\n",deviceInfo.friendlyName );
	PrintData("---- deviceInfo.manufacturer = %s\n",deviceInfo.manufacturer );
	PrintData("---- deviceInfo.manufacturerUrl = %s\n",deviceInfo.manufacturerUrl);
	PrintData("---- deviceInfo.modelDescription = %s\n",deviceInfo.modelDescription);
	PrintData("---- deviceInfo.modelName = %s\n",deviceInfo.modelName);
	PrintData("---- deviceInfo.modelNumber = %s\n",deviceInfo.modelNumber);
	PrintData("---- deviceInfo.modelUrl = %s\n",deviceInfo.modelUrl);
	PrintData("---- deviceInfo.serialNumber = %s\n",deviceInfo.serialNumber);
	PrintData("---- deviceInfo.udn = %s\n",deviceInfo.udn);
	PrintData("---- deviceInfo.upc = %s\n",deviceInfo.upc);
	BDlna_Dmp_SubscribeServer(s_hDmp, h->hDevice);

	h->next = s_DLNA_DeviceList;
	s_DLNA_DeviceList = h;
	s_untotal_device_count++;

	/* Callback 이벤트를 발생시킨다. */
	if ( s_DmpDeviceCallback != NULL )
	{
		tDeviceEvent.evtType = DI_DLNA_EVENT_DEVICE_ADDED;

		tDeviceInfo.friendlyName = (HUINT8*)deviceInfo.friendlyName;
		tDeviceInfo.manufacturer = (HUINT8*)deviceInfo.manufacturer;
		tDeviceInfo.manufacturerUrl = (HUINT8*)deviceInfo.manufacturerUrl;
		tDeviceInfo.modelDescription = (HUINT8*)deviceInfo.modelDescription;
		tDeviceInfo.modelName = (HUINT8*)deviceInfo.modelName;
		tDeviceInfo.modelNumber = (HUINT8*)deviceInfo.modelNumber;
		tDeviceInfo.modelUrl = (HUINT8*)deviceInfo.modelUrl;
		tDeviceInfo.serialNumber = (HUINT8*)deviceInfo.serialNumber;
		tDeviceInfo.udn = (HUINT8*)deviceInfo.udn;
		tDeviceInfo.upc = (HUINT8*)deviceInfo.upc;

		tDeviceEvent.device = &tDeviceInfo;

		s_DmpDeviceCallback(&tDeviceEvent);	/* Device Event 발생 */
	}
	else
	{
		PrintError("[%s] DeviceCallback is NULL\n", __FUNCTION__);
	}

	PrintExit;
}


static void P_DLNA_Dmp_DeviceRemoved(void *context, const char* uuid)
{
    struct DmpDeviceHandle* node = NULL;
    struct DmpDeviceHandle* previous= NULL;
	BUPnPDeviceInfo deviceInfo;
	BDlna_DmpHandle hDmp;
	DI_DLNA_DeviceEvent	tDeviceEvent; 		/* 캡터에게 알려주는 Devie Event */

	PrintEnter;

	hDmp = (BDlna_DmpHandle)context;
	PrintData("[%s] Removed Device = %s\n", __FUNCTION__, uuid);
	PrintData("[%s] s_untotal_device_count = %d\n", __FUNCTION__, s_untotal_device_count);

	node = s_DLNA_DeviceList;
	while (node)
	{
		BDlna_Dmp_GetDeviceInfo(s_hDmp, node->hDevice, &deviceInfo);
		PrintData("---- deviceInfo.friendlyName = %s\n",deviceInfo.friendlyName );
		PrintData("---- deviceInfo.manufacturer = %s\n",deviceInfo.manufacturer );
		PrintData("---- deviceInfo.manufacturerUrl = %s\n",deviceInfo.manufacturerUrl);
		PrintData("---- deviceInfo.modelDescription = %s\n",deviceInfo.modelDescription);
		PrintData("---- deviceInfo.modelName = %s\n",deviceInfo.modelName);
		PrintData("---- deviceInfo.modelNumber = %s\n",deviceInfo.modelNumber);
		PrintData("---- deviceInfo.modelUrl = %s\n",deviceInfo.modelUrl);
		PrintData("---- deviceInfo.serialNumber = %s\n",deviceInfo.serialNumber);
		PrintData("---- deviceInfo.udn = %s\n",deviceInfo.udn);
		PrintData("---- deviceInfo.upc = %s\n",deviceInfo.upc);

		if (VK_strcmp(deviceInfo.udn, uuid) == 0)
		{
			tDeviceEvent.evtType = DI_DLNA_EVENT_DEVICE_REMOVED;
			tDeviceEvent.device = DD_MEM_Alloc(sizeof(DI_DLNA_Device));
			if(tDeviceEvent.device != NULL)
			{
				tDeviceEvent.device->friendlyName = (HUINT8*)deviceInfo.friendlyName;
				tDeviceEvent.device->manufacturer = (HUINT8*)deviceInfo.manufacturer;
				tDeviceEvent.device->manufacturerUrl = (HUINT8*)deviceInfo.manufacturerUrl;
				tDeviceEvent.device->modelDescription = (HUINT8*)deviceInfo.modelDescription;
				tDeviceEvent.device->modelName = (HUINT8*)deviceInfo.modelName;
				tDeviceEvent.device->modelNumber = (HUINT8*)deviceInfo.modelNumber;
				tDeviceEvent.device->modelUrl = (HUINT8*)deviceInfo.modelUrl;
				tDeviceEvent.device->serialNumber = (HUINT8*)deviceInfo.serialNumber;
				tDeviceEvent.device->udn = (HUINT8*)deviceInfo.udn;
				tDeviceEvent.device->upc = (HUINT8*)deviceInfo.upc;
			}

			s_untotal_device_count--;

			/* Callback 이벤트를 발생시킨다. */
			if ( s_DmpDeviceCallback != NULL )
			{
				s_DmpDeviceCallback(&tDeviceEvent);	/* Device Event 발생 */
			}
			else
			{
				PrintError("[%s] DeviceCallback is NULL\n", __FUNCTION__);
			}

			BDlna_Dmp_DestroyDevice(node->hDevice);
			if (previous == NULL)
			{
				s_DLNA_DeviceList = node->next;
			}
			else
			{
				previous->next = node->next;
			}

			if(tDeviceEvent.device != NULL)
			{
				DD_MEM_Free(tDeviceEvent.device);
			}

			if(node != NULL)
			{
				DD_MEM_Free(node);
			}

			PrintExit;

			return;
		}

		previous = node;
		node = node->next;
	}

	PrintExit;
}

static void P_DLNA_Dmp_PropertyChangedCallback(void *context, char* variable, char* value)
{
	UNUSED_PARAMETER(context);
	UNUSED_PARAMETER(variable);
	UNUSED_PARAMETER(value);

	VK_printf("P_DLNA_Dmp_PropertyChangedCallback\n");
}

static DI_DLNA_MetadataAttr * P_DLNA_GetContainerAttributes(BDlna_DmpDeviceHandle hDevice, BDlna_DidlHandle Didl_handle, int nIndex, BDlna_DidlMediaType MediaType, HUINT8* ParentID, HUINT32 *nAttrs )
{
	DI_DLNA_MetadataAttr * Attrs;
	uint32_t			chid_count;
	HUINT8				a_child_count[10];
//	char				ctmp[10];

	UNUSED_ARGUMENT(hDevice);
	UNUSED_ARGUMENT(ParentID);

	PrintEnter;

	Attrs = (DI_DLNA_MetadataAttr *)DD_MEM_Calloc(3*sizeof(DI_DLNA_MetadataAttr));
	if (Attrs == NULL )
	{
		PrintData("[---- MEM Alloc failed!!!");
		return NULL;
	}

	Attrs[0].key = VK_StrDup("id");
	Attrs[0].value = VK_StrDup(BDlna_Didl_GetId(Didl_handle, nIndex));

	if ( MediaType == BDlna_DidlMediaType_eMediaContainer ) /* Container 일 경우 Metadata를 넘겨 주지 않음 */
	{
		Attrs[1].key = VK_StrDup("parentID");
		Attrs[1].value = NULL;

		Attrs[2].key = VK_StrDup("childCount");
		if ( BDLNA_E_SUCCESS == BDlna_Didl_GetChildrenCount(Didl_handle, nIndex, &chid_count) )
		{
#if 0 /* Prevent Defect로 수정. 추가 Test필요 */
			VK_MEM_Memset(ctmp, 0x0, sizeof(ctmp));
			itoa(chid_count, ctmp, 10);

			VK_snprintf((char *)a_child_count, VK_strlen(ctmp), "%d", chid_count);
#else
			VK_sprintf((char *)a_child_count, "%d", chid_count);
#endif
			Attrs[2].value = VK_StrDup(a_child_count);
		}
		else
		{
			PrintError("[---- Get Child Count Failed!!!");
			Attrs[2].value = VK_StrDup("0");
		}

	}
	/*else*/

	*nAttrs = 3;

	PrintExit;

	return Attrs;
}

static DI_DLNA_Metadata * P_DLNA_Get_ItemMetadata(HUINT8 * xml, HUINT32 length)
{
	HUINT32  done=0;

	PrintEnter;

	/* Parser 에서 채워줄 Metadata 포인터 */
	s_tItemMetadata = NULL;
	s_tItemAttrs = NULL;
	s_unPropertyCount = 0;

	/* XML Parser 생성 */
	XML_Parser p = XML_ParserCreate(NULL);
    if (! p)
    {
    	PrintError("Couldn't allocate memory for parser\n");
    	return NULL;
    }

    // xml의 element를 만났을 때 호출될 함수를 등록한다.
    // start는 처음태그, end는 마지막 태그를 만났을 때 호출된다.
    XML_SetElementHandler(p, P_DLNA_XML_StartTagParser, P_DLNA_XML_EndTagParser);

    // 실제 데이터 (예 : <tag>data</tag>의 data)를 처리하기 위해서
    // 호출될 함수를 등록한다.
    XML_SetCharacterDataHandler(p, P_DLNA_XML_DataParser);

    if (!XML_Parse(p, (char *)xml, (int)length, (int)done))
    {
    	PrintError("Parse error at line %d:\n%s\n", XML_GetCurrentLineNumber(p), XML_ErrorString(XML_GetErrorCode(p)));

    	PrintError("Check this XML\n %s\n", xml);

    	return NULL;
    }

    /* Property 의 개수만큼 Realloc 한다. 메모리의 효율성을 위해. */
	s_tItemMetadata = (DI_DLNA_Metadata *)realloc(s_tItemMetadata, sizeof(DI_DLNA_Metadata) * s_unPropertyCount );
	if (s_tItemMetadata == NULL )
	{
		PrintError("[---- MEM realloc failed!!!");
	}

	PrintExit;

    return s_tItemMetadata;
}

static DI_DLNA_Metadata * P_DLNA_GetMetadata(BDlna_DmpDeviceHandle hDevice, BDlna_DidlHandle Didl_handle, int nIndex, BDlna_DidlMediaType MediaType, DI_DLNA_MetadataAttr * Attrs, HUINT32 nAttrs )
{
	DI_DLNA_Metadata	*Metadata = NULL;
	HUINT8				*temp_Metadata = NULL;

	PrintEnter;

	UNUSED_ARGUMENT(hDevice);

	/* 현재의 Metadata는 Container의 경우 3개로 정한다. */
	if ( MediaType == BDlna_DidlMediaType_eMediaContainer ) /* Container */
	{
		Metadata = (DI_DLNA_Metadata *)DD_MEM_Calloc(3*sizeof(DI_DLNA_Metadata));
		if (Metadata == NULL )
		{
			PrintError("[---- MEM Alloc failed!!!");
			return NULL;
		}

		Metadata[0].key = VK_StrDup("container");
		Metadata[0].value = VK_StrDup("");
		Metadata[0].parent = NULL;
		Metadata[0].child_cnt = 2; /* 자신의 개수를 포함하지 않은 나머지 Metadata 의 개수 */
		Metadata[0].childs = &Metadata[1];
		Metadata[0].attr_cnt = nAttrs;
		Metadata[0].attrs = Attrs;
		Metadata[0].ns_cnt = 0;  /* NAME SPACE ATTR 없음*/
		Metadata[0].ns = NULL;

		Metadata[1].key = VK_StrDup("dc:title");
		Metadata[1].value = VK_StrDup(BDlna_Didl_GetTitle(Didl_handle, nIndex));
		Metadata[1].parent = &Metadata[0];
		Metadata[1].child_cnt = 0;
		Metadata[1].childs = &Metadata[2];
		Metadata[1].attr_cnt = 0;
		Metadata[1].attrs = NULL;
		Metadata[1].ns_cnt = 0;
		Metadata[1].ns = NULL;

		Metadata[2].key = VK_StrDup("upnp:class");
		Metadata[2].value = VK_StrDup(BDlna_Didl_GetClass(Didl_handle, nIndex));
		Metadata[2].parent = &Metadata[1];
		Metadata[2].child_cnt = 0;
		Metadata[2].childs = NULL;
		Metadata[2].attr_cnt = 0;
		Metadata[2].attrs = NULL;
		Metadata[2].ns_cnt = 0;
		Metadata[2].ns = NULL;

		for ( int i =0 ; i < 3 ; i++ )
		{
			PrintData("---- Item Metadata[%d].key = %s\n", i, Metadata[i].key);
			PrintData("---- Item Metadata[%d].value = %s\n", i, Metadata[i].value);
			if ( i == 0 )
			{
				for ( int j =0; j < (int)nAttrs ; j++ )
				{
					PrintData("-------- Item Attr[%d].key = %s\n", j, Attrs[j].key);
					PrintData("-------- Item Attr[%d].value = %s\n", j, Attrs[j].value);
				}
			}
		}
	}
	else /* Item */
	{
#if 0
		Metadata_didlObjects = BDlna_Didl_Create();
		if ( Metadata_didlObjects == NULL )
		{
			PrintError("[%s] BDlna_Didl_Create failed!", __FUNCTION__);
		}

		BDlna_Dmp_BrowseMetaData(s_hDmp, hDevice, BDlna_Didl_GetId(Didl_handle, nIndex), Metadata_didlObjects);

		temp_Metadata = VK_StrDup(BDlna_Didl_GetMetaData(Metadata_didlObjects, 0));
#else
		temp_Metadata = VK_StrDup(BDlna_Didl_GetMetaData(Didl_handle, nIndex));
#endif

		//PRINT_ERROR(("@@@@@ Metadata \n%s", temp_Metadata));
		if(temp_Metadata != NULL)
		{
			Metadata = P_DLNA_Get_ItemMetadata(temp_Metadata, VK_strlen((char*)temp_Metadata));
			if ( Metadata == NULL )
			{
				PrintError("DLNA_Get_ItemMetadata failed!!!!!!!!!!!! title = %s", BDlna_Didl_GetTitle(Didl_handle, nIndex));
			}
			else
			{
				for ( int i =0 ; i <= (int)Metadata[0].child_cnt ; i++ )
				{
					PrintData("---- Item Metadata[%d].key = %s\n", i, Metadata[i].key);
					PrintData("---- Item Metadata[%d].value = %s\n", i, Metadata[i].value);
					if ( Metadata[i].attr_cnt != 0 )
					{
						for ( int j =0; j < (int)Metadata[i].attr_cnt ; j++ )
						{
							PrintData("-------- Item Attr[%d].key = %s\n", j, Metadata[i].attrs[j].key);
							PrintData("-------- Item Attr[%d].value = %s\n", j, Metadata[i].attrs[j].value);
						}
					}
				}
			}

			/* parsing 종료 하였기 때문에 Free */
			DD_MEM_Free(temp_Metadata);
		}
		else
		{
			Metadata = NULL;
		}
#if 0
		BDlna_Didl_Destroy(Metadata_didlObjects);
#endif
	}

	PrintExit;

	return Metadata;
}

static HUINT32	P_DLNA_FreeMetadata(DI_DLNA_Metadata *pMeta, HUINT32 nMeta)
{
	PrintEnter;

	/* Metadata is always allocated entirely */
	VK_free(pMeta->attrs);
	VK_free(pMeta);

	PrintExit;

	return nMeta;
}

static HINT32 P_DLNA_InitializeDlnaCore(bool bCoreInit, DI_DLNA_DEVICE_TYPE eDeviceType)
{
	HINT32 nDiRet = DI_ERR_OK;
	BDlna_OpenSettings open_settings;
	BDlna_Error ret;

	PrintEnter;

	if(bCoreInit == TRUE)
	{
		if (eDeviceType == DI_DLNA_DEVICE_DMP) {
			s_bDmpCoreInited = TRUE;
		}
		if (eDeviceType == DI_DLNA_DEVICE_DMS) {
			s_bDmsCoreInited = TRUE;
		}

		if(s_eDlnaCoreState != DI_DLNA_CORE_NA)
		{
			PrintError("[DLNA][%s:%d] Warning! Already BDlna initialized", __FUNCTION__, __LINE__);
			nDiRet = DI_ERR_OK;
		}
		else
		{
		    /* Initialize NEXUS */
		    B_Os_Init();

			/* Initialize DLNA engine */
		    BDlna_GetDefaultOpenSettings(&open_settings);

		    open_settings.networkInterface = s_tNetworkInfo.networkInterface;
		    open_settings.ipAddress = s_tNetworkInfo.ipAddress;
		    open_settings.port = s_tNetworkInfo.port;
		    open_settings.ssdpRefreshPeriod = s_tNetworkInfo.ssdpRefreshPeriod;
		    open_settings.waitOnStop = s_tNetworkInfo.waitOnStop;

			PrintData("[DLNA][%s:%d] --> BDlna_Initialize()\n", __FUNCTION__, __LINE__);
			ret = BDlna_Initialize(&open_settings);
			if (ret != BDLNA_E_SUCCESS)
		    {
				PrintError("[DLNA][%s:%d] Error! BDlna_Initialize failed", __FUNCTION__, __LINE__);
		        return DI_ERR_ERROR;
		    }

			s_eDlnaCoreState = DI_DLNA_CORE_INIT;
			nDiRet = DI_ERR_OK;
		}
	}
	else
	{
		if (eDeviceType == DI_DLNA_DEVICE_DMP) {
			s_bDmpCoreInited = FALSE;
		}
		if (eDeviceType == DI_DLNA_DEVICE_DMS) {
			s_bDmsCoreInited = FALSE;
		}

		if ((s_eDlnaCoreState == DI_DLNA_CORE_INIT) || (s_eDlnaCoreState == DI_DLNA_CORE_STOPPED))
		{
			PrintData("[DLNA][%s:%d] --> BDlna_Uninitialize()\n", __FUNCTION__, __LINE__);
			BDlna_Uninitialize();
			s_eDlnaCoreState = DI_DLNA_CORE_NA;
		}

		nDiRet = DI_ERR_OK;
	}

	PrintExit;

	return nDiRet;
}

// - Stop: BUPnP_StopDevice(hDms);
// - Start: BUPnP_StartDevice(hDms);
static HINT32 P_DLNA_StartDlnaCore(bool bCoreStart, DI_DLNA_DEVICE_TYPE eDeviceType)
{
	HINT32 nDiRet = DI_ERR_OK;
	//BDlna_OpenSettings open_settings;
	BDlna_Error ret;

	PrintEnter;

	if(bCoreStart == TRUE)
	{
		if (eDeviceType == DI_DLNA_DEVICE_DMP) {
			s_bDmpCoreInited = TRUE;
		}
		if (eDeviceType == DI_DLNA_DEVICE_DMS) {
			s_bDmsCoreInited = TRUE;
		}

		if(s_eDlnaCoreState == DI_DLNA_CORE_NA)
		{
			PrintError("[DLNA][%s:%d] Error! BDlna uninitialized", __FUNCTION__, __LINE__);
			nDiRet = DI_ERR_OK;
		}
		else if(s_eDlnaCoreState == DI_DLNA_CORE_STARTED)
		{
			PrintError("[DLNA][%s:%d] Warning! Already BDlna started", __FUNCTION__, __LINE__);
			nDiRet = DI_ERR_OK;
		}
		else
		{
			/* BDLNA 시작*/
			PrintData("[DLNA][%s:%d] --> BDlna_Start()\n", __FUNCTION__, __LINE__);
			ret = BDlna_Start();
			if (ret != BDLNA_E_SUCCESS)
			{
				PrintError("[DLNA][%s:%d] Error! BDlna_Start failed\n", __FUNCTION__, __LINE__);
				PrintData("[DLNA][%s:%d] --> BDlna_Uninitialize()\n", __FUNCTION__, __LINE__);
				BDlna_Uninitialize();
				s_eDlnaCoreState = DI_DLNA_CORE_NA;
				return DI_ERR_ERROR;
			}

			s_eDlnaCoreState = DI_DLNA_CORE_STARTED;
			nDiRet = DI_ERR_OK;
#if 1
			BUPnPDebug_SetSettings(0x07, 0x08);
#endif
		}
	}
	else
	{
		if (eDeviceType == DI_DLNA_DEVICE_DMP) {
			s_bDmpCoreInited = FALSE;
		}
		if (eDeviceType == DI_DLNA_DEVICE_DMS) {
			s_bDmsCoreInited = FALSE;
		}

		if(s_eDlnaCoreState == DI_DLNA_CORE_STARTED)
		{
			PrintData("[DLNA][%s:%d] --> BDlna_Stop()\n", __FUNCTION__, __LINE__);
			BDlna_Stop();

			// Don't exec BDlna_Uninitialize() here...
			s_eDlnaCoreState = DI_DLNA_CORE_STOPPED;
		}
		else {
			PrintError("[DLNA][%s:%d] Error! Invalid BDlna state:%d", __FUNCTION__, __LINE__, s_eDlnaCoreState);
		}

		nDiRet = DI_ERR_OK;
	}

	PrintExit;

	return nDiRet;
}

static HINT32 P_DLNA_Dmp_CoreInitialize(void)
{
	return P_DLNA_InitializeDlnaCore(TRUE, DI_DLNA_DEVICE_DMP);
}

static HINT32 P_DLNA_Dmp_CoreUninitialize(void)
{
	return P_DLNA_InitializeDlnaCore(FALSE, DI_DLNA_DEVICE_DMP);
}

static HINT32 P_DLNA_DMS_CoreInitialize(void)
{
	return P_DLNA_InitializeDlnaCore(TRUE, DI_DLNA_DEVICE_DMS);
}

static HINT32 P_DLNA_DMS_CoreUninitialize(void)
{
	return P_DLNA_InitializeDlnaCore(FALSE, DI_DLNA_DEVICE_DMS);
}

static HINT32 P_DLNA_Dmp_CoreStart(void)
{
	return P_DLNA_StartDlnaCore(TRUE, DI_DLNA_DEVICE_DMP);
}

static HINT32 P_DLNA_Dmp_CoreStop(void)
{
	return P_DLNA_StartDlnaCore(FALSE, DI_DLNA_DEVICE_DMP);
}

static HINT32 P_DLNA_DMS_CoreStart(void)
{
	return P_DLNA_StartDlnaCore(TRUE, DI_DLNA_DEVICE_DMS);
}

static HINT32 P_DLNA_DMS_CoreStop(void)
{
	return P_DLNA_StartDlnaCore(FALSE, DI_DLNA_DEVICE_DMS);
}

static DI_DLNA_PSCAN_ITEM P_DLNA_DMS_NewScanItem(char * PathToScan)
{
	DI_DLNA_PSCAN_ITEM pNewItem=NULL;

	pNewItem = (DI_DLNA_PSCAN_ITEM) DD_MEM_Alloc(sizeof(DI_DLNA_SCAN_ITEM));
	if(pNewItem)
	{
		pNewItem->DirPathToScan = VK_StrDup(PathToScan);
		pNewItem->next=NULL;
	}

	return pNewItem;
}

static void P_DLNA_DMS_FreeScanItem(DI_DLNA_PSCAN_ITEM item)
{
	//if (item) {
		DD_MEM_Free(item->DirPathToScan);
		DD_MEM_Free(item);
	//}
}

static void P_DLNA_DMS_Insert_ListTail(DI_DLNA_PSCAN_ITEM *pHead, DI_DLNA_PSCAN_ITEM item)
{
	DI_DLNA_PSCAN_ITEM temp = *pHead;

	if(!*pHead)
	{
		*pHead = item;
		return;
	}

	while(temp)
	{
		if(temp->next)
		{
			temp = temp->next;
		}
		else
		{
			temp->next = item;
			break;
		}
	}
}

static DI_DLNA_PSCAN_ITEM P_DLNA_DMS_Remove_ListHead(DI_DLNA_PSCAN_ITEM *pHead)
{
	DI_DLNA_PSCAN_ITEM temp = *pHead;

	if(!*pHead)
	{
		return NULL;
	}
	else
	{
		*pHead = temp->next;
	}

	return temp;
}

static char P_DLNA_DMS_Is_ListEmpty(DI_DLNA_PSCAN_ITEM *pHead)
{
	if(!*pHead)
	{
		return TRUE;
	}

	return FALSE;
}

static DI_DLNA_SCAN_ITEM_RESULT P_DLNA_DMS_NeedItemScan(struct stat *FileStats, char *FileName, char *Extension)
{
	UNUSED_PARAMETER(Extension);

	PrintData("=======%s  %lld          %s\n", 	(S_ISDIR(FileStats->st_mode)) ? "<Dir >":"<File>", FileStats->st_size,FileName);

	if(!FileName)
	{
		return SCAN_ITEM_IGNORE;
	}

	if(!S_ISDIR(FileStats->st_mode))
	{
		/*Match the Extension here. If matches, then return SCAN_ITEM_FILE else return SCAN_ITEM_IGNORE*/
		return SCAN_ITEM_FILE;
	}

	if( (S_ISDIR(FileStats->st_mode)) && (VK_strcmp(FileName,".") ) && (VK_strcmp(FileName,"..")) )
	{
		/*This is a new Directory to Scan*/
		return SCAN_ITEM_DIR;
	}

	return SCAN_ITEM_IGNORE;
}

static char P_DLNA_DMS_HandleScanResult(DI_DLNA_SCAN_ITEM_RESULT 	ScanRes,
				 char 				ScanSubDir,
				 DI_DLNA_PSCAN_ITEM 		*Head,
				 char 				*FileName,
				 char 				*FullFilePath,
				 off_t				FileSz,
				 DI_DLNA_PSCAN_ITEM 		scan_item,
				 DLNA_NEW_FILE_FOUND 	pfnFoundFile,
				 void				*cb_context)
{
	DI_DLNA_FOUND_FILE_INFO		FileInfo;
	DI_DLNA_PSCAN_ITEM  NewItem=NULL;

	UNUSED_PARAMETER(scan_item);

	if(!FileName || !Head)
	{
		return FALSE;
	}

	if(ScanRes == SCAN_ITEM_IGNORE)
	{
		return TRUE; /*This is not a error*/
	}

	if(ScanRes == SCAN_ITEM_DIR)
	{
		if(ScanSubDir)
		{
			NewItem = P_DLNA_DMS_NewScanItem(FullFilePath);
			P_DLNA_DMS_Insert_ListTail(Head,NewItem);
		}
	}

	if(ScanRes == SCAN_ITEM_FILE)
	{
		/*Invoke the callback if provided and the file matched the extension*/
		FileInfo.FileName	  = FileName;
		FileInfo.FullFilePath = FullFilePath;
		FileInfo.FileSize = FileSz;
		pfnFoundFile(cb_context,&FileInfo);
	}

	return TRUE;
}


/**
 * @b Function @b Description <br>
 * DLNA Core Module ??珥덇린???섎뒗 ?⑥닔?낅땲??
 *
 *  initialize all the DLNA related modules. <br>
 *  Needs to be called before any other DLNA related procedures <br>
 *
 *  @return  #  TRUE  (1) : Success <br>
 *           #  FALSE (0) : Failed <br>
 */
DRV_Error DRV_DLNA_Init(void)
{
	DRV_Error 		nDrvRet = DRV_OK;
	VK_ERR_CODE		nVkRet = VK_OK;

	PrintEnter;

	nVkRet = VK_SEM_Create(&s_ulDlnaSemId,"di_dlna", DEFAULT_SUSPEND_TYPE);
	if( nVkRet != VK_OK )
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, nVkRet);
		return DRV_ERR;
	}
#if 0
	//DLNA Task & Msg
	nVkRet = VK_MSG_Create(DI_DLNA_MSG_SIZE, sizeof(DI_DLNA_MSG),
						"MSMsgQ", &s_ulDlnaMId, VK_SUSPENDTYPE_FIFO);
	if(nVkRet != VK_OK)
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, nVkRet);
		return DRV_ERR;
	}

	//DLNA Task & Event
	nVkRet = VK_EVENT_Create(&s_ulDlnaEId, "DlnaEvent");
	if(nVkRet != VK_OK)
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, nVkRet);
		return DRV_ERR;
	}

	nVkRet = VK_TASK_Create(P_DLNA_EventCallbackThread,
						DLNA_TIME_TASK_PRIORITY,
						DLNA_TIME_TASK_STACK_SIZE,
						"DlnaTime",
						NULL,
						&s_ulDlnaTaskId,
						FALSE);
	if(nVkRet != VK_OK)
	{
		PrintError("[%s] [%d] Error(0x%x) in VK_TASK_Create()\n\r",__FUNCTION__,__LINE__, nVkRet);
		return DRV_ERR;
	}

	VK_TASK_Start(s_ulDlnaTaskId);
#endif
	VK_MEM_Memset(&s_tNetworkInfo, 0x0, sizeof(DI_DLNA_NetworkInterface));

	PrintExit;

	return nDrvRet;
}

/**
 * @b Function @b Description <br>
 * DLNA Core Module ??醫낅즺?섎뒗 ?⑥닔?낅땲??
 *
 *  close and free all the DLNA related modules. <br>
 *  @return  #  TRUE  (1) : Success <br>
 *           #  FALSE (0) : Failed <br>
 */
DRV_Error DRV_DLNA_Term(void)
{
	DRV_Error 		nDrvRet = DRV_OK;
	VK_ERR_CODE		nVkRet = VK_OK;

	PrintEnter;

#if 0
	nVkRet = VK_MSG_Delete(s_ulDlnaMId, "MSMsgQ", sizeof(DI_DLNA_MSG));
	if(nVkRet != VK_OK)
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, nVkRet);
		return DRV_ERR;
	}

	nVkRet = VK_EVENT_Destroy(s_ulDlnaEId);
	if(nVkRet != VK_OK)
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, nVkRet);
		return DRV_ERR;
	}

	VK_TASK_Stop(s_ulDlnaTaskId);

   	nVkRet = VK_TASK_Destroy( s_ulDlnaTaskId);
	if( nVkRet != VK_OK )
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, nVkRet);
		return DRV_ERR;
	}
#endif

	nVkRet = VK_SEM_Release(s_ulDlnaSemId);
	if( nVkRet != VK_OK )
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, nVkRet);
		return DRV_ERR;
	}

	if(s_tNetworkInfo.networkInterface != NULL)
	{
		DD_MEM_Free(s_tNetworkInfo.networkInterface);
		s_tNetworkInfo.networkInterface = NULL;
	}

	if(s_tNetworkInfo.ipAddress != NULL)
	{
		DD_MEM_Free(s_tNetworkInfo.ipAddress);
		s_tNetworkInfo.ipAddress = NULL;
	}

	s_tNetworkInfo.port = 0;
	s_tNetworkInfo.ssdpRefreshPeriod = 0;
	s_tNetworkInfo.waitOnStop = 0;

	PrintExit;

	return nDrvRet;
}

/**
 *	Function: DI_DLNA_SetNetworkInterface
 *	Description: DLNA Module 동작을 위해 NetworkInterce를 설정하는 함수
 *	Param: [in] ptNetworkInfo Network정보 구조체
 *	return  DI_ERR_CODE
 *
 */
DI_ERR_CODE	DI_DLNA_SetNetworkInterface(DI_DLNA_NetworkInterface *ptNetworkInfo)
{
	HINT32 			nDiRet = DI_ERR_OK;

	PrintEnter;

	if(ptNetworkInfo == NULL)
    {
		PrintError("[%s] ptNetworkInfo is null", __FUNCTION__);
        return DI_ERR_ERROR;
    }

	if(ptNetworkInfo->networkInterface != NULL)
	{
		if(s_tNetworkInfo.networkInterface != NULL)
		{
			DD_MEM_Free(s_tNetworkInfo.networkInterface); // VK_MEM_Free??
			s_tNetworkInfo.networkInterface = NULL;
		}

		s_tNetworkInfo.networkInterface = VK_StrDup(ptNetworkInfo->networkInterface); // <-- free()
	}

	if(ptNetworkInfo->ipAddress != NULL)
	{
		if(s_tNetworkInfo.ipAddress != NULL)
		{
			DD_MEM_Free(s_tNetworkInfo.ipAddress);
			s_tNetworkInfo.ipAddress = NULL;
		}

		s_tNetworkInfo.ipAddress = VK_StrDup(ptNetworkInfo->ipAddress);
	}

	s_tNetworkInfo.port = ptNetworkInfo->port;
	s_tNetworkInfo.ssdpRefreshPeriod = ptNetworkInfo->ssdpRefreshPeriod;
	s_tNetworkInfo.waitOnStop = ptNetworkInfo->waitOnStop;

	PrintExit;

	return nDiRet;
}

/**
 *	Function: DI_DLNA_StartDMP
 *	Description: DLNA DMP Module을 동작하는 함수. DI_DLNA_SetNetworkInterface가 설정된후 호출되어야 함.
 *	Param:	void
 *	Return:  DI_ERR_CODE
 *
 */
DI_ERR_CODE	DI_DLNA_StartDMP(void)
{
	DI_ERR_CODE 		nDiRet = DI_ERR_OK;
	BDlna_Dmp_Settings 		tDmpSettings;

	PrintEnter;

	P_DLNA_Dmp_CoreInitialize();

	/* DMP Class 에 대한 시작 */
	(void)BDlna_Dmp_GetDefaultSettings(&tDmpSettings);
	tDmpSettings.deviceAddedCallback= P_DLNA_Dmp_DeviceAdded;
	tDmpSettings.deviceRemovedCallback= P_DLNA_Dmp_DeviceRemoved;
	tDmpSettings.propertyChangedCallback = P_DLNA_Dmp_PropertyChangedCallback;
	s_hDmp = BDlna_Dmp_Create(&tDmpSettings);
	if (s_hDmp == NULL)
    {
		PrintError("[%s] HAAL_DLNA_DMP_Open failed", __FUNCTION__);
		nDiRet = DI_ERR_ERROR;
        return nDiRet;
    }
	else
	{
		(void)BDlna_Dmp_GetSettings(s_hDmp, &tDmpSettings);
		tDmpSettings.context = s_hDmp;
		(void)BDlna_Dmp_SetSettings(s_hDmp, &tDmpSettings);

		P_DLNA_Dmp_CoreStart();
		nDiRet = DI_ERR_OK;
	}

	PrintExit;

	return nDiRet;
}

/**
 *	Function: DI_DLNA_StopDMP
 *	Description: DLNA DMP Module을 정지하는 함수.
 *	Param:	void
 *	Return:  DI_ERR_CODE
 *
 */
DI_ERR_CODE	DI_DLNA_StopDMP(void)
{
	DI_ERR_CODE 			nDiRet = DI_ERR_OK;

	PrintEnter;

	(void)P_DLNA_Dmp_CoreStop();

	(void)BDlna_Dmp_Destroy(s_hDmp);

	(void)P_DLNA_Dmp_CoreUninitialize();

	PrintExit;

	return nDiRet;
}


/**
 *	Function: DI_DLNA_RegisterDeviceCB
 *	Description: Device변경에 대한 정보를 얻기위한 Callback함수를 등록하는 함수
 *	Param:	[in] callback	등록할 Callback함수
 *	Return:  DI_ERR_CODE
 *
 */
DI_ERR_CODE DI_DLNA_RegisterDeviceCB(DI_DLNA_DeviceCallback callback)
{
	HINT32 			nDiRet = DI_ERR_OK;

	PrintEnter;

	s_DmpDeviceCallback = callback;

	PrintExit;

	return nDiRet;
}

/**
 *	Function: DI_DLNA_RegisterDeviceCB
 *	Description: 등록한 Callback을 해제함.
 *	Param:	[in] callback	등록할 Callback함수
 *	Return:  DI_ERR_CODE
 *
 */
DI_ERR_CODE DI_DLNA_UnregisterDeviceCB(void)
{
	HINT32 			nDiRet = DI_ERR_OK;

	PrintEnter;

	s_DmpDeviceCallback = NULL;

	PrintExit;

	return nDiRet;
}



/**
 *	Function: DI_DLNA_RegisterContentCB
 *	Description: Content변경에 대한 정보를 얻기위한 Callback함수를 등록하는 함수
 *	Param:	callback	등록할 Callback함수
 *	Return:  DI_ERR_CODE
 *
 */
DI_ERR_CODE DI_DLNA_RegisterContentCB(DI_DLNA_ContentCallback callback)
{
	HINT32 			nDiRet = DI_ERR_OK;

	PrintEnter;

	s_DmpContentCallback = callback;

	PrintExit;

	return nDiRet;
}

/**
 *	Function: DI_DLNA_UnregisterContentCB
 *	Description: 등록한 Callback을 해제함.
 *	Param:	callback	등록할 Callback함수
 *	Return:  DI_ERR_CODE
 *
 */
DI_ERR_CODE DI_DLNA_UnregisterContentCB(void)
{
	HINT32 			nDiRet = DI_ERR_OK;

	PrintEnter;

	s_DmpContentCallback = NULL;

	PrintExit;

	return nDiRet;
}

/**
 *	Function: DI_DLNA_GetDevices
 *	Description: 현재 연결된 Device List 개수를 얻는 함수
 *	Param:	[out] NumberofDevices	현재 연결된 Device 수
 *	Return:  DI_ERR_CODE
 *
 */
DI_ERR_CODE DI_DLNA_GetNumberOfDevices(HUINT32 *NumberofDevices)
{
	HINT32 			nDiRet = DI_ERR_OK;

	PrintEnter;

	*NumberofDevices = s_untotal_device_count;

	PrintExit;

	return nDiRet;
}


/**
 *	Function: DI_DLNA_GetDevices
 *	Description: 현재 연결된 Device List를 얻는 함수
 *	Param:	[out] pDevicelist	현재 연결된 Device List
 *	Return:  DI_ERR_CODE
 *
*/
DI_ERR_CODE DI_DLNA_GetDevices(DI_DLNA_Device **pDevicelist)
{
	DI_ERR_CODE 			nDiRet = DI_ERR_OK;
    struct DmpDeviceHandle	*node=NULL;
    struct DmpDeviceHandle	*previous= NULL;
	BUPnPDeviceInfo			deviceInfo;
	HUINT32					count=0;
	DI_DLNA_Device			*pTempDevicelist = NULL;

	pTempDevicelist = DD_MEM_Calloc(s_untotal_device_count*sizeof(DI_DLNA_Device));
	*pDevicelist = pTempDevicelist;

	PrintEnter;

	node = s_DLNA_DeviceList;
	while (node)
	{
		BDlna_Dmp_GetDeviceInfo(s_hDmp, node->hDevice, &deviceInfo);
		PrintData("---- GetDevices %d\n", count);
		PrintData("---- deviceInfo.friendlyName = %s\n",deviceInfo.friendlyName );
		PrintData("---- deviceInfo.manufacturer = %s\n",deviceInfo.manufacturer );
		PrintData("---- deviceInfo.manufacturerUrl = %s\n",deviceInfo.manufacturerUrl);
		PrintData("---- deviceInfo.modelDescription = %s\n",deviceInfo.modelDescription);
		PrintData("---- deviceInfo.modelName = %s\n",deviceInfo.modelName);
		PrintData("---- deviceInfo.modelNumber = %s\n",deviceInfo.modelNumber);
		PrintData("---- deviceInfo.modelUrl = %s\n",deviceInfo.modelUrl);
		PrintData("---- deviceInfo.serialNumber = %s\n",deviceInfo.serialNumber);
		PrintData("---- deviceInfo.udn = %s\n",deviceInfo.udn);
		PrintData("---- deviceInfo.upc = %s\n",deviceInfo.upc);

		pTempDevicelist[count].friendlyName = (HUINT8*)deviceInfo.friendlyName;
		pTempDevicelist[count].manufacturer = (HUINT8*)deviceInfo.manufacturer;
		pTempDevicelist[count].manufacturerUrl = (HUINT8*)deviceInfo.manufacturerUrl;
		pTempDevicelist[count].modelDescription = (HUINT8*)deviceInfo.modelDescription;
		pTempDevicelist[count].modelName = (HUINT8*)deviceInfo.modelName;
		pTempDevicelist[count].modelNumber = (HUINT8*)deviceInfo.modelNumber;
		pTempDevicelist[count].modelUrl = (HUINT8*)deviceInfo.modelUrl;
		pTempDevicelist[count].serialNumber = (HUINT8*)deviceInfo.serialNumber;
		pTempDevicelist[count].udn = (HUINT8*)deviceInfo.udn;
		pTempDevicelist[count].upc = (HUINT8*)deviceInfo.upc;

		count++;

		previous = node;
		node = node->next;
	}

	PrintExit;

	return nDiRet;
}

/**
 *	Function: DI_DLNA_BrowseContent
 *	Description: 현재 연결된 Item 정보 List를 얻어오는 함수
 *  Param[in]:  device_uuid	pointer to the device_uuid <br>
 *  Param[in]:  starting_entryID	the ID of the ContentEntry on the server to start the browse from.
 *  							   A value of null SHALL indicate the root container on this server
 *  							   	ex) "0"
 *  Param[in]:  browse_flag 		if set to true, this operation will browse all of the direct children of the startingEntryID parameter.
 *  					      If false, this operation will return a content list containing the entry identified by startingEntryID only.
 *  							   	ex) "BrowseMetadata" , "BrowseDirectChildren"
 *  Param[in]:  starting_index		starting zero-based offset to enumerate children under the container specified by parent.
 *  							   	ex) 0
 *  Param[in]:  requested_count		requested number of entries under the ContentContainer specified by parent.
 *  							  Setting this parameter to 0 indicates request all entries.
 *  							   	ex) 0
 *  Param[out]:	out_content			content of DI_DLNA_Content type
 *  Param[out]:	cnt_content			number of allocated out_content
 *	Return:  DI_ERR_CODE
 *
 */
DI_ERR_CODE DI_DLNA_BrowseContent(const HUINT8 *device_uuid, const HUINT8 *starting_entryID, HUINT32 starting_index,
							HUINT32 requested_count, DI_DLNA_Content **out_content, HUINT32 *cnt_content)
{
	HINT32 			nDiRet = DI_ERR_OK;
	HUINT32	container_count;
	HUINT32	updateID;
	HUINT32	children_total_matches;
	BUPnPDeviceInfo deviceInfo;
	DI_DLNA_MetadataAttr * temp_pAttrs;
	HUINT32 			temp_nAttrs;
	BDlna_DidlHandle didlObjects=NULL;
	/* Get device handle */
    struct DmpDeviceHandle* node;
    struct DmpDeviceHandle* previous= NULL;
	HUINT32 i = 0;

	PrintEnter;

	PrintData("[%s] starting_index=%d, requested_count=%d, starting_entryID = %s", __FUNCTION__, starting_index, requested_count, starting_entryID);

	/* Get device handle */
	s_untotal_contents_count=0;

	/* 리턴 value 초기화*/
	*out_content = NULL;
	*cnt_content = 0;

	node = s_DLNA_DeviceList;
	if ( node == NULL )
	{
		PrintData("---- No devices Found!!!!!!! ");
		return DI_ERR_ERROR;
	}

	while (node)
	{
		BDlna_Dmp_GetDeviceInfo(s_hDmp, node->hDevice, &deviceInfo);
		if (VK_strcmp(deviceInfo.udn, device_uuid) == 0 )
		{
			PrintData("---- Found device %s", device_uuid);
			BDlna_Dmp_SubscribeServer(s_hDmp, node->hDevice);
			break;
		}
		previous = node;
		node = node->next;
		if ( node == NULL )
		{
			PrintError("---- There are No devices Found!!!!!!! ");
			return DI_ERR_ERROR;
		}
	}

	PrintData("---- Finding Entry %s\n", starting_entryID);
	didlObjects = BDlna_Didl_Create();

	if ( BDLNA_E_SUCCESS != BDlna_Dmp_BrowseDirectChildrenCustom(s_hDmp, node->hDevice, (char *)starting_entryID, "*", starting_index, requested_count, &container_count, &children_total_matches, &updateID, didlObjects) )
	{
		PrintError("[---- BDlna_Dmp_BrowseDirectChildren failed!!!");
		return DI_ERR_ERROR;
	}

	PrintData("---- Number of Counts %d, children_total_matches =%d, UPDATE ID = %d\n", container_count, children_total_matches, updateID);

	s_pHnContents = (DI_DLNA_Content *)DD_MEM_Calloc(container_count * sizeof(DI_DLNA_Content));
	if (s_pHnContents == NULL )
	{
		PrintError("[---- MEM Alloc failed!!!");
		return DI_ERR_ERROR;
	}

	/* GET Root Container Contents it is also Contaioner or Item .*/
	for (; i < container_count; i++ )
	{
		if ( BDlna_Didl_GetMediaType(didlObjects, i) == BDlna_DidlMediaType_eMediaContainer)
		{
			PrintData("---- Found 1st Depth Container... \n");
			s_pHnContents[i].isContentItem = BDlna_DidlMediaType_eMediaContainer; /* It's Contents Container */
			s_pHnContents[i].id = VK_StrDup(BDlna_Didl_GetId(didlObjects, i));
			/* Get Attributes First. It is nessesary for Make Metadata */
			temp_pAttrs = P_DLNA_GetContainerAttributes(node->hDevice,didlObjects, i,BDlna_DidlMediaType_eMediaContainer, NULL, &temp_nAttrs);
			/* Get Metadata */
			s_pHnContents[i].metadata = P_DLNA_GetMetadata(node->hDevice, didlObjects, i, BDlna_DidlMediaType_eMediaContainer, temp_pAttrs, temp_nAttrs);
			s_untotal_contents_count++;
		}
		else
		{
			PrintData("---- Found 1st Depth item... \n");
			s_pHnContents[i].isContentItem = BDlna_DidlMediaType_eMediaItem; /* It's Contents Item */
			s_pHnContents[i].id = VK_StrDup(BDlna_Didl_GetId(didlObjects, i));
			/* Get Metadata */
			s_pHnContents[i].metadata = P_DLNA_GetMetadata(node->hDevice, didlObjects, i, BDlna_DidlMediaType_eMediaItem , NULL, 0);
			s_untotal_contents_count++;
		}
	}

	if ( didlObjects != NULL)
	{
		BDlna_Didl_Destroy(didlObjects);
	}

	*out_content = s_pHnContents;

	/* Contents Count */
	*cnt_content = s_untotal_contents_count;

	PrintExit;

	return nDiRet;
}


/**
 *	Function: DI_DLNA_GetContentInfo
 *	Description: 특정 Item 정보를 얻어오는 함수
 *  Param[in]:  device_uuid	pointer to the device_uuid <br>
 *  Param[in]:  starting_entryID	the ID of the ContentEntry on the server to start the browse from.
 *  							   A value of null SHALL indicate the root container on this server
 *  							   	ex) "0"
 *  Param[out]:	out_content			content of DI_DLNA_Content type
 *  Param[out]:	cnt_content			number of allocated out_content
 *	Return:  DI_ERR_CODE
 *
 */
DI_ERR_CODE DI_DLNA_GetContentInfo(const HUINT8 *device_uuid, const HUINT8 *starting_entryID, DI_DLNA_Content **out_content, HUINT32 *cnt_content)
{
	HINT32 			nDiRet = DI_ERR_OK;
    struct DmpDeviceHandle* node;
    struct DmpDeviceHandle* previous= NULL;
	BUPnPDeviceInfo deviceInfo;
	DI_DLNA_MetadataAttr * temp_pAttrs;
	HUINT32 			temp_nAttrs;
	BDlna_DidlHandle didlObjects=NULL;

	PrintEnter;

	PrintData("[%s] starting_entryID = %s", __FUNCTION__, starting_entryID);

	/* Get device handle */
	s_untotal_contents_count=0;

	/* 리턴 value 초기화*/
	*out_content = NULL;
	*cnt_content = 0;

	node = s_DLNA_DeviceList;
	if ( node == NULL )
	{
		PrintData("---- No devices Found!!!!!!! ");
		return DI_ERR_ERROR;
	}

	while (node)
	{
		BDlna_Dmp_GetDeviceInfo(s_hDmp, node->hDevice, &deviceInfo);
		if (VK_strcmp(deviceInfo.udn, device_uuid) == 0 )
		{
			PrintData("---- Found device %s", device_uuid);
			BDlna_Dmp_SubscribeServer(s_hDmp, node->hDevice);
			break;
		}
		previous = node;
		node = node->next;
		if ( node == NULL )
		{
			PrintError("---- There are No devices Found!!!!!!! ");
			return DI_ERR_ERROR;
		}
	}

	PrintData("---- Finding Entry %s", starting_entryID);
	didlObjects = BDlna_Didl_Create();

	if ( BDLNA_E_SUCCESS != BDlna_Dmp_BrowseMetaData(s_hDmp, node->hDevice, (char *)starting_entryID, didlObjects) )
	{
		PrintError("[---- BDlna_Dmp_BrowseMetaData failed!!!");
		return DI_ERR_ERROR;
	}

	s_pHnContents = (DI_DLNA_Content *)DD_MEM_Calloc(sizeof(DI_DLNA_Content));
	if (s_pHnContents == NULL )
	{
		PrintError("[---- MEM Alloc failed!!!");
		return DI_ERR_ERROR;
	}

	if ( BDlna_Didl_GetMediaType(didlObjects, 0) == BDlna_DidlMediaType_eMediaContainer)
	{
		s_pHnContents[0].isContentItem = BDlna_DidlMediaType_eMediaContainer; /* It's Contents Container */
		s_pHnContents[0].id = VK_StrDup(BDlna_Didl_GetId(didlObjects, 0));
		/* Get Attributes First. It is nessesary for Make Metadata */
		temp_pAttrs = P_DLNA_GetContainerAttributes(node->hDevice,didlObjects, 0,BDlna_DidlMediaType_eMediaContainer, NULL, &temp_nAttrs);
		/* Get Metadata */
		s_pHnContents[0].metadata = P_DLNA_GetMetadata(node->hDevice, didlObjects, 0, BDlna_DidlMediaType_eMediaContainer, temp_pAttrs, temp_nAttrs);
		s_untotal_contents_count++;
	}
	else
	{
		s_pHnContents[0].isContentItem = BDlna_DidlMediaType_eMediaItem; /* It's Contents Item */
		s_pHnContents[0].id = VK_StrDup(BDlna_Didl_GetId(didlObjects, 0));
		/* Get Metadata */
		s_pHnContents[0].metadata = P_DLNA_GetMetadata(node->hDevice, didlObjects, 0, BDlna_DidlMediaType_eMediaItem , NULL, 0);
		s_untotal_contents_count++;
	}

	if ( didlObjects != NULL)
	{
		BDlna_Didl_Destroy(didlObjects);
	}

	*out_content = s_pHnContents;

	/* Contents Count */
	*cnt_content = s_untotal_contents_count;

	PrintExit;

	return nDiRet;
}

/**
 *	Function: DI_DLNA_SearchContent
 *	Description: Sort를 변경하는 함수
 *  Param[in]:  device_uuid	pointer to the device_uuid <br>
 *  Param[in]:  starting_entryID	the ID of the ContentEntry on the server to start the browse from.
 *  							   A value of null SHALL indicate the root container on this server
 *  							   	ex) "0"
 *  Param[in]:  browse_flag 		if set to true, this operation will browse all of the direct children of the startingEntryID parameter.
 *  					      If false, this operation will return a content list containing the entry identified by startingEntryID only.
 *  							   	ex) "BrowseMetadata" , "BrowseDirectChildren"
 *  Param[in]:  starting_index		starting zero-based offset to enumerate children under the container specified by parent.
 *  							   	ex) 0
 *  Param[in]:  requested_count		requested number of entries under the ContentContainer specified by parent.
 *  							  Setting this parameter to 0 indicates request all entries.
 *  							   	ex) 0
 *  Param[in]:  search_criteria		contains the criteria string to search for. <br>
 *  							  If this parameter is null, the implementation SHALL consider all entries in the parent container as matching the search criteria. <br>
 *  							ex) "@id == \\"0\\""
 *  Param[out]:	out_content			content of DI_DLNA_Content type
 *  Param[out]:	cnt_content			number of allocated out_content
 *	Return:  DI_ERR_CODE
 *
 */
HUINT32 DI_DLNA_SearchContent(
        const HUINT8 *device_uuid,
        const HUINT8 *parent_ID,
        const HUINT32 starting_index,
        const HUINT32 requested_count,
        const HUINT8 *search_criteria,
        DI_DLNA_Content **out_content,
        HUINT32 *cnt_content
		)
{
	DI_DLNA_MetadataAttr * temp_pAttrs;
	HUINT32 			temp_nAttrs;
	HUINT32	container_count, root_container_total_matches, updateID;
	HUINT32 i = 0;
	/* Get device handle */
    struct DmpDeviceHandle* node;
    struct DmpDeviceHandle* previous= NULL;
	BUPnPDeviceInfo deviceInfo;
	BDlna_DidlHandle didlObjects=NULL;

	PrintEnter;

	PrintData("[%s] Parent_ID = %s", __FUNCTION__, parent_ID);

	s_untotal_contents_count=0;

	/* 리턴 value 초기화*/
	*out_content = NULL;
	*cnt_content = 0;

	node = s_DLNA_DeviceList;
	if ( node == NULL )
	{
		PrintData("---- No devices Found!!!!!!! ");
		return DI_ERR_ERROR;
	}

	while (node)
	{
		BDlna_Dmp_GetDeviceInfo(s_hDmp, node->hDevice, &deviceInfo);
		if (VK_strcmp(deviceInfo.udn, device_uuid) == 0 )
		{
			PrintData("---- Found device %s", device_uuid);
			BDlna_Dmp_SubscribeServer(s_hDmp, node->hDevice);

			break;
		}
		previous = node;
		node = node->next;
		if ( node == NULL )
		{
			PrintError("---- There are No devices Found!!!!!!! ");
			return DI_ERR_ERROR;
		}
	}

	PrintData("---- Searching Entry %s", parent_ID);
	didlObjects = BDlna_Didl_Create();

	if (BDlna_Dmp_Search(s_hDmp, node->hDevice, "0", (char *)search_criteria, starting_index, requested_count, &container_count, &root_container_total_matches, &updateID, didlObjects) != BDLNA_E_SUCCESS )
	{
		PrintError("[---- BDlna_Dmp_Search failed!!!");
		return DI_ERR_ERROR;
	}

	PrintData("---- Number of Counts %d, UPDATE ID = %d", container_count, updateID);

	s_pHnContents = (DI_DLNA_Content *)DD_MEM_Calloc(container_count*sizeof(DI_DLNA_Content));
	if (s_pHnContents == NULL )
	{
		PrintError("[---- MEM Alloc failed!!!");
		return DI_ERR_ERROR;
	}

	/* GET Root Container Contents it is also Contaioner or Item .*/
	for ( i = starting_index; i < (container_count + starting_index) ; i++ )
	{
		if ( BDlna_Didl_GetMediaType(didlObjects, i) == BDlna_DidlMediaType_eMediaContainer)
		{
			PrintData("---- Found 1st Depth Container... ");
			s_pHnContents[i].isContentItem = BDlna_DidlMediaType_eMediaContainer; /* It's Contents Container */
			s_pHnContents[i].id = VK_StrDup(BDlna_Didl_GetId(didlObjects, i));
			/* Get Attributes First. It is nessesary for Make Metadata */
			temp_pAttrs = P_DLNA_GetContainerAttributes(node->hDevice,didlObjects, i,BDlna_DidlMediaType_eMediaContainer, NULL, &temp_nAttrs);
			/* Get Metadata */
			s_pHnContents[i].metadata = P_DLNA_GetMetadata(node->hDevice, didlObjects, i, BDlna_DidlMediaType_eMediaContainer, temp_pAttrs, temp_nAttrs);
			s_untotal_contents_count++;
		}
		else
		{
			PrintData("---- Found 1st Depth item... ");
			s_pHnContents[i].isContentItem = BDlna_DidlMediaType_eMediaItem; /* It's Contents Item */
			s_pHnContents[i].id = VK_StrDup(BDlna_Didl_GetId(didlObjects, i));
			/* Get Attributes First. It is nessesary for Make Metadata */
			/* Get Metadata */
			s_pHnContents[i].metadata = P_DLNA_GetMetadata(node->hDevice, didlObjects, i, BDlna_DidlMediaType_eMediaItem, NULL, 0);
			s_untotal_contents_count++;
		}
	}

	*out_content = s_pHnContents;

	/* Contents Count */
	*cnt_content = s_untotal_contents_count;

	PrintExit;


	return 0;
}

/**
 *	Function: DI_DLNA_UpdateDeviceList
 *	Description: 연결된 Device 정보를 다시 요청하는 함수
 *	Param:	void
 *	Return:  DI_ERR_CODE
 *
 */
void DI_DLNA_UpdateDeviceList(void)
{
	(void)BUPnPControlPoint_SearchDevices();

	return;
}

/**
 *	Function: DI_DLNA_FreeDevices
 *	Description: Device List를 정리하는 함수
 *  Param[in]:  pDevice		delivered address. <br>
 *  Param[in]:  nDevice		Count of DI_DLNADevice. <br>
 *                        When received with DI_DLNADevice, count does same. <br>
 *	Return:  DI_ERR_CODE
 *
 */
DI_ERR_CODE DI_DLNA_FreeDevices( DI_DLNA_Device *pDevice)
{
	DI_ERR_CODE 			nDiRet = DI_ERR_OK;

	PrintEnter;

	if(pDevice != NULL)
	{
		DD_MEM_Free(pDevice);
		pDevice = NULL;
	}

	PrintExit;

	return nDiRet;
}

/**
 *	Function: DI_DLNA_FreeContents
 *	Description: Content List를 정리하는 함수
 *  Param[in]:  pDevice		delivered address. <br>
 *  Param[in]:  nDevice		Count of DI_DLNADevice. <br>
 *                        When received with DI_DLNADevice, count does same. <br>
 *	Return:  DI_ERR_CODE
 *
 */
DI_ERR_CODE DI_DLNA_FreeContents( DI_DLNA_Content *pContent,  HUINT32 nContent)
{
	DI_ERR_CODE 			nDiRet = DI_ERR_OK;

	PrintEnter;

	/* Contents are always allocated entirely */
	(void)P_DLNA_FreeMetadata(pContent->metadata, nContent);
	DD_MEM_Free(pContent);

	PrintExit;

	return nDiRet;
}

/**
 *	Function: DI_DLNA_ScanDir
 *	Description: 지정한 Directory를 Scan하는 함수
 *  Param[in]:  StartDir		The Root for starting the Scan
 *  Param[in]:  ScanSubDir		Scan The Sub Directories
 *  Param[in]:  Extension		The Files To Report eg. mpg
 *  Param[in]:  pfnFoundFile		The callback function when file is found
 *  Param[in]:  cb_context
 *
 *	Return:
 *
 */
char DI_DLNA_ScanDir(char *StartDir,	char ScanSubDir, char *Extension, DLNA_NEW_FILE_FOUND pfnFoundFile,	void *cb_context)
{
	DI_DLNA_PSCAN_ITEM	Head=NULL;
	DI_DLNA_PSCAN_ITEM	scan_item= NULL;
	struct dirent **namelist=NULL;
	struct stat FileStat;
	char FullPath[MAX_FILE_PATH];
	DI_DLNA_SCAN_ITEM_RESULT ScanRes;
	int i,n;

  	if(!StartDir)
  	{
  		return FALSE;
  	}

	scan_item = P_DLNA_DMS_NewScanItem(StartDir);
	P_DLNA_DMS_Insert_ListTail(&Head,scan_item);

	while(!P_DLNA_DMS_Is_ListEmpty(&Head))
	{
		scan_item = P_DLNA_DMS_Remove_ListHead(&Head);
		if(scan_item == NULL)
		{
			PrintError("List Cannot Be NULL Here!!");
			return FALSE;
		}

		PrintData("Scanning %s\n",scan_item->DirPathToScan);
		n = scandir(scan_item->DirPathToScan,&namelist,0,0);
		if(n < 0)
		{
			PrintError("Scanning Failure\n");
			return 0;
		}

		for(i=0;i < n; i++)
		{
			VK_snprintf(FullPath,
				(VK_strlen(scan_item->DirPathToScan)+VK_strlen(namelist[i]->d_name)+VK_strlen("/")+1),
				"%s%s%s", scan_item->DirPathToScan,"/",namelist[i]->d_name);

			if(stat(FullPath,&FileStat) < 0)
			{
				continue;
			}

			ScanRes = P_DLNA_DMS_NeedItemScan(&FileStat,
									 namelist[i]->d_name,
									 Extension);

			P_DLNA_DMS_HandleScanResult(ScanRes,
							 ScanSubDir,
							 &Head,
							 namelist[i]->d_name,
							 FullPath,
							 FileStat.st_size,
							 scan_item,
							 pfnFoundFile,
							 cb_context);
		}

		P_DLNA_DMS_FreeScanItem(scan_item);
	}

	return TRUE;
}

// Need mutex lock...
static void P_DLNA_DMS_StartDmsTask(void *pParam)
{
	//struct stDMS_DI_RUNNING_THREAD *pThread = (struct stDMS_DI_RUNNING_THREAD *) pParam;
#if 0
	DMS_DI_HANDLE *hDms = (DMS_DI_HANDLE *) pParam;
#endif
	BUPnPError buRet;

//	lock(_ghDmsDiHandle->lock);
	PrintEnter;

	UNUSED_ARGUMENT(pParam);

	if (_ghDmsDiHandle == NULL) {
		PrintError("[DLNA/DMS] %s(%d) Error! Invalid NULL handle!\n", __FUNCTION__,__LINE__);
		goto func_error;
	}
	if (_ghDmsDiHandle->pDmsOpenSettings == NULL) {
		PrintError("[DLNA/DMS] %s(%d) Error! Invalid NULL setting!\n", __FUNCTION__,__LINE__);
		goto func_error;
	}
	if (_ghDmsDiHandle->pUPnPOpenSettings == NULL) {
		PrintError("[DLNA/DMS] %s(%d) Error! Invalid NULL setting!\n", __FUNCTION__,__LINE__);
		goto func_error;
	}
	if (_ghDmsDiHandle->hStartThread == NULL) {
		PrintError("[DLNA/DMS] %s(%d) Error! Invalid NULL task!\n", __FUNCTION__,__LINE__);
		;
	}

	_ghDmsDiHandle->hStartThread->bStartTaskRunning = TRUE;

	// Start BRCM DLNA core
#if 0
	(void)P_DLNA_DMS_CoreInitialize();
#else
	VK_printf( "[DLNA/DMS][%s:%d] --> +BUPnP_Initialize()\n", __func__, __LINE__);
	buRet = BUPnP_Initialize(_ghDmsDiHandle->pUPnPOpenSettings);
	VK_printf( "[DLNA/DMS][%s:%d] --> -BUPnP_Initialize()\n", __func__, __LINE__);
	if (UPNP_E_SUCCESS != buRet) {
		PrintError("[DLNA/DMS] %s(%d) Error! Failed to BUPnP_Initialize!\n", __FUNCTION__,__LINE__);
		goto func_error;
	}
#endif

	/* Sometimes, takes very long time by number of files */
	_ghDmsDiHandle->hStartThread->bBrcmScanRunning = TRUE;
	VK_printf( "[DLNA/DMS][%s:%d] --> +BDlna_Dms_Create()\n", __func__, __LINE__);
	_ghDmsDiHandle->hBrcmDms = BDlna_Dms_Create(_ghDmsDiHandle->pDmsOpenSettings);
	VK_printf( "[DLNA/DMS][%s:%d] --> -BDlna_Dms_Create()\n", __func__, __LINE__);
	_ghDmsDiHandle->hStartThread->bBrcmScanRunning = FALSE; // Scanning is finished
	if (!_ghDmsDiHandle->hBrcmDms)
	{
		PrintError("[DLNA/DMS] %s(%d) Error! Failed to Create bDMS!\n", __FUNCTION__,__LINE__);
		goto func_error;
	}

	_ghDmsDiHandle->bBrcmInternalTaskActivated = TRUE;
	PrintData("[DLNA/DMS] Success Create bDMS\n");

#if 0
	(void)P_DLNA_DMS_CoreStart();
#else
	VK_printf( "[DLNA/DMS][%s:%d] --> +BUPnP_Start()\n", __func__, __LINE__);
	BUPnP_Start();
	VK_printf( "[DLNA/DMS][%s:%d] --> -BUPnP_Start()\n", __func__, __LINE__);
#endif

	PrintExit;
//	unlock(_ghDmsDiHandle->lock);
	return;

func_error:
	if (_ghDmsDiHandle && _ghDmsDiHandle->hStartThread) {
		_ghDmsDiHandle->hStartThread->bStartTaskRunning = FALSE;
		_ghDmsDiHandle->hStartThread->bThreadError = TRUE;
		// Dealloceatiion for task will be done externally
	}

	PrintExit;
//	unlock(_ghDmsDiHandle->lock);
	return;
}

/**
 *	Function: DI_DLNA_StartDMS
 *	Description: DMS를 Start하는 함수
 *  Param[in]:  tDMSInfo		DMS 정보 설정
 *
 *	Return:
 *
 */
#if 0
DI_ERR_CODE	DI_DLNA_StartDMS(DI_DLNA_DMSSetting_t *tDMSInfo)
#else
DI_ERR_CODE DI_DLNA_StartDMS(DI_DLNA_DMSSetting_t *tDMSInfo, DI_DLNA_NetworkInterface *ptNetworkInfo)
#endif
{
	DI_ERR_CODE nDiRet = DI_ERR_OK;
	HINT32 nVkRet = 0;
	HBOOL bDebugArgs = TRUE;
	HBOOL bDebugSetting = TRUE;
	struct stat fstat;
	// Dont use local DmsOpenSettings variable.
	// Maybe, brcm don't copy setting vars.
#if 0
	//BDlna_Dms_OpenSettings DmsOpenSettings;
	DMS_DI_HANDLE *hDms = _ghDmsDiHandle;
#endif

	PrintEnter;

	if (tDMSInfo == NULL) {
		PrintError("[DLNA/DMS] %s(%d) Error - Invalid NULL argments!\n", __FUNCTION__,__LINE__);
		nDiRet = DI_ERR_INVALID_PARAM;
	 	goto func_error;
	}
	if (ptNetworkInfo == NULL) {
		PrintError("[DLNA/DMS] %s(%d) Error - Invalid NULL network info!\n",__FUNCTION__,__LINE__);
		return DI_ERR_INVALID_PARAM;
	}

	if (bDebugArgs == TRUE)
	{
		PrintData("[DLNA/DMS] %s(%d) ===============================\n", __FUNCTION__,__LINE__);
		PrintData("[DLNA/DMS] %s(%d) [Network Info]\n", __FUNCTION__,__LINE__);
		PrintData("[DLNA/DMS] %s(%d) ===============================\n", __FUNCTION__,__LINE__);
		if (ptNetworkInfo->networkInterface)
		  PrintData("[DLNA/DMS] %s(%d) + [networkInterface: %s]\n", __FUNCTION__, __LINE__, ptNetworkInfo->networkInterface);
		else
		  PrintData("[DLNA/DMS] %s(%d) + [networkInterface: N/A]\n", __FUNCTION__,__LINE__);
		if (ptNetworkInfo->ipAddress)
		  PrintData("[DLNA/DMS] %s(%d) + [ipAddress: %s]\n", __FUNCTION__, __LINE__, ptNetworkInfo->ipAddress);
		else
		  PrintData("[DLNA/DMS] %s(%d) + [ipAddress: N/A]\n", __FUNCTION__,__LINE__);
		PrintData("[DLNA/DMS] %s(%d) + [port: %d]\n", __FUNCTION__, __LINE__, ptNetworkInfo->port);
		PrintData("[DLNA/DMS] %s(%d) + [ssdpRefreshPeriod: %d]\n", __FUNCTION__, __LINE__, ptNetworkInfo->ssdpRefreshPeriod);
		PrintData("[DLNA/DMS] %s(%d) + [waitOnStop: %d]\n", __FUNCTION__, __LINE__, ptNetworkInfo->waitOnStop);
	}

	/*****************************************************************************/
	/* Validation checking for tDMSInfo args is needed before allocating handle  */
	/* BRCM limitation checking for directory configuration                      */
	/* All the args null-termincated??                                           */
	if (1)
	{
		/*****************************************************************************
		 * metaDbDir
		 *     The directory hold the SQLite database file. The default is /tmp.
		 *     If /tmp is a RAMFS, reboot the system will create a new dtatbase file again.
		 *
		 * mediaDir
		 *     The directory of the internal storage contains media files for streaming.
		 *
		 * extMediaDir
		 *     The directory of the external storage contains media files for streaming,
		 *     this must be a path belongs to a mounting point.
		 *     This directory must begin with "/mnt/".
		 *
		 * metaInfoDir
		 *     The default is not set.
		 *     The directory hold the meta information files such as albumArt,
		 *         stream's information(.info) and index file(.nav) for trick mode operation.
		 *     If this is not set, albumArt is under mediaDir and other meta information is located
		 *         at the same directory which holds the stream file.
		 *     This directory can not be overlapped with mediaDir and extMediaDir
		 *
		 ******************************** NOTE ***************************************
		 * mediaDir, recordedDir and metaDbDir must be not overlapped with each other
		 *****************************************************************************/
		if ((VK_strlen(tDMSInfo->contentDir) < 2) &&
			(VK_strlen(tDMSInfo->extMediaDir) < 2))
		{
 			/* make sure that mediaDir is set in dms.conf */
			PrintError("[DLNA/DMS] %s(%d) Error - [BRCM] mediaDir or extMediaDir must be set!\n",
						__FUNCTION__,__LINE__);
		 	nDiRet = DI_ERR_ERROR;
		 	goto func_error;
 		}

		if (VK_strlen(tDMSInfo->extMediaDir) > 0) {
 			if (VK_strncmp(tDMSInfo->extMediaDir, "/mnt/", 5)) {
 				/* make sure extMediaDir is started with "/mnt" */
				PrintError("[DLNA/DMS] %s(%d) Error - [BRCM] extMediaDir must be set to started with /mnt/ in dms.conf!!!\n",
							__FUNCTION__,__LINE__);
 				nDiRet = DI_ERR_INVALID_PARAM;
		 		goto func_error;
 			}
 		}

		if ((VK_strlen(tDMSInfo->recordDir) > 1) &&
			(VK_strlen(tDMSInfo->contentDir) > 1))
		{
		 	/* make sure that "mediaDir" and "recordedDir" are not overlapped in dms.conf */
			if (!VK_strncmp(tDMSInfo->contentDir,tDMSInfo->recordDir,
		 		(VK_strlen(tDMSInfo->contentDir)<VK_strlen(tDMSInfo->recordDir)?VK_strlen(tDMSInfo->contentDir):VK_strlen(tDMSInfo->recordDir))))
		 	{
 				PrintError("[DLNA/DMS] %s(%d) Warning - [BRCM] mediaDir=%s or recordedDir=%s should not be contained by another!\n",
 							__FUNCTION__,__LINE__, tDMSInfo->contentDir, tDMSInfo->recordDir);
 				;
 			}
 		}

		if ((VK_strlen(tDMSInfo->extMediaDir) > 1) &&
			(VK_strlen(tDMSInfo->contentDir) > 1))
		{
			/* make sure that "mediaDir" and "extMediaDir" are not overlapped in dms.conf */
			if(!VK_strncmp(tDMSInfo->contentDir,tDMSInfo->extMediaDir,
				(VK_strlen(tDMSInfo->contentDir)<VK_strlen(tDMSInfo->extMediaDir)?VK_strlen(tDMSInfo->contentDir):VK_strlen(tDMSInfo->extMediaDir))))
			{
 				PrintError("[DLNA/DMS] %s(%d) Error - [BRCM] mediaDir=%s or extMediaDir=%s should not be contained by another!\n",
 							__FUNCTION__,__LINE__, tDMSInfo->contentDir, tDMSInfo->extMediaDir);
 				nDiRet = DI_ERR_INVALID_PARAM;
		 		goto func_error;
 			}
 		}

		if ((VK_strlen(tDMSInfo->metaInfoDir) > 1) &&
			(VK_strlen(tDMSInfo->contentDir) > 1))
		{
 			/* make sure that "mediaDir" and "metaInfoDir" are not overlapped in dms.conf */
			if(!VK_strncmp(tDMSInfo->contentDir,tDMSInfo->metaInfoDir,
				(VK_strlen(tDMSInfo->contentDir)<VK_strlen(tDMSInfo->metaInfoDir)?VK_strlen(tDMSInfo->contentDir):VK_strlen(tDMSInfo->metaInfoDir))))
			{
 				PrintError("[DLNA/DMS] %s(%d) Error - [BRCM] mediaDir=%s or metaInfoDir=%s should not be contained by another!\n",
 							__FUNCTION__,__LINE__, tDMSInfo->contentDir, tDMSInfo->metaInfoDir);
 				;
 			}
 		}

		if ((VK_strlen(tDMSInfo->extMediaDir) > 1) &&
			(VK_strlen(tDMSInfo->metaInfoDir) >1) )
		{
		 	/* make sure that "metaInfoDir" and "extMediaDir" are not overlapped in dms.conf */
			if(!VK_strncmp(tDMSInfo->contentDir,tDMSInfo->extMediaDir,
				(VK_strlen(tDMSInfo->metaInfoDir)<VK_strlen(tDMSInfo->extMediaDir)?VK_strlen(tDMSInfo->metaInfoDir):VK_strlen(tDMSInfo->extMediaDir))))
			{
 				PrintError("[DLNA/DMS] %s(%d) Error - [BRCM] metaInfoDir=%s or extMediaDir=%s should not be contained by another!\n",
 							__FUNCTION__,__LINE__, tDMSInfo->metaInfoDir, tDMSInfo->extMediaDir);
 				nDiRet = DI_ERR_INVALID_PARAM;
		 		goto func_error;
 			}
 		}
	}

	/* Check directory stat */
	if (1)
	{
 		if (VK_strlen(tDMSInfo->contentDir) > 1) /* Allow "/" directory */
		{
	 		if(stat((char*)tDMSInfo->contentDir, &fstat) < 0) {
 				/* Sometimes, stat() system call cause out of order          */
 				/* Don't trust stat() system call; Just show use warning msg */
 				PrintError("[DLNA/DMS] %s(%d) Error - Can't stat to (path:%s)!\n",
 							__FUNCTION__,__LINE__, tDMSInfo->extMediaDir);
	 			;
 			}
 			else {
 				if(!S_ISDIR(fstat.st_mode)) {
 					PrintError("[DLNA/DMS] %s(%d) Error - It isn't directory stat for (path:%s)!\n",
 								__FUNCTION__,__LINE__, tDMSInfo->extMediaDir);
 					;
	 			}
	 		}
	 	}

	 	if (VK_strlen(tDMSInfo->metaDbDir) > 1) /* Allow "/" directory */
		{
	 		if(stat((char*)tDMSInfo->metaDbDir, &fstat) < 0) {
 				/* Sometimes, stat() system call cause out of order          */
 				/* Don't trust stat() system call; Just show use warning msg */
 				PrintError("[DLNA/DMS] %s(%d) Error - Can't stat to (path:%s)!\n",
 							__FUNCTION__,__LINE__, tDMSInfo->metaDbDir);
	 			;
 			}
 			else {
 				if(!S_ISDIR(fstat.st_mode)) {
 					PrintError("[DLNA/DMS] %s(%d) Error - It isn't directory stat for (path:%s)!\n",
 								__FUNCTION__,__LINE__, tDMSInfo->metaDbDir);
 					;
	 			}
	 		}
	 	}

	 	if (VK_strlen(tDMSInfo->recordDir) > 1) /* Allow "/" directory */
		{
	 		if(stat((char*)tDMSInfo->recordDir, &fstat) < 0) {
 				/* Sometimes, stat() system call cause out of order          */
 				/* Don't trust stat() system call; Just show use warning msg */
 				PrintError("[DLNA/DMS] %s(%d) Error - Can't stat to (path:%s)!\n",
 							__FUNCTION__,__LINE__, tDMSInfo->recordDir);
	 			;
 			}
 			else {
 				if(!S_ISDIR(fstat.st_mode)) {
 					PrintError("[DLNA/DMS] %s(%d) Error - It isn't directory stat for (path:%s)!\n",
 								__FUNCTION__,__LINE__, tDMSInfo->recordDir);
 					;
	 			}
	 		}
	 	}

	 	if (VK_strlen(tDMSInfo->metaInfoDir) > 1) /* Allow "/" directory */
		{
	 		if(stat((char*)tDMSInfo->metaInfoDir, &fstat) < 0) {
 				/* Sometimes, stat() system call cause out of order          */
 				/* Don't trust stat() system call; Just show use warning msg */
 				PrintError("[DLNA/DMS] %s(%d) Error - Can't stat to (path:%s)!\n",
 							__FUNCTION__,__LINE__, tDMSInfo->metaInfoDir);
	 			;
 			}
 			else {
 				if(!S_ISDIR(fstat.st_mode)) {
 					PrintError("[DLNA/DMS] %s(%d) Error - It isn't directory stat for (path:%s)!\n",
 								__FUNCTION__,__LINE__, tDMSInfo->metaInfoDir);
 					;
	 			}
	 		}
	 	}

	 	if (VK_strlen(tDMSInfo->extMediaDir) > 1) /* Allow "/" directory */
		{
	 		if(stat((char*)tDMSInfo->extMediaDir, &fstat) < 0) {
 				/* Sometimes, stat() system call cause out of order          */
 				/* Don't trust stat() system call; Just show use warning msg */
 				PrintError("[DLNA/DMS] %s(%d) Error - Can't stat to (path:%s)!\n",
 							__FUNCTION__,__LINE__, tDMSInfo->extMediaDir);
	 			;
 			}
 			else {
 				if(!S_ISDIR(fstat.st_mode)) {
 					PrintError("[DLNA/DMS] %s(%d) Error - It isn't directory stat for (path:%s)!\n",
 								__FUNCTION__,__LINE__, tDMSInfo->extMediaDir);
 					;
	 			}
	 		}
	 	}
	}

	/* Check (tDMSInfo->pDeviceInfo) */
	if (tDMSInfo->pDeviceInfo == NULL) {
		PrintError("[DLNA/DMS] %s(%d) Error - Invalid NULL deviceInfo!\n", __FUNCTION__,__LINE__);
		nDiRet = DI_ERR_INVALID_PARAM;
		goto func_error;
	}
	if (tDMSInfo->pDeviceInfo->friendlyName == NULL) {
		PrintError("[DLNA/DMS] %s(%d) Error - Invalid NULL friendlyName!\n", __FUNCTION__,__LINE__);
		nDiRet = DI_ERR_INVALID_PARAM;
		goto func_error;
	}

	/* Check (ptNetworkInfo) */
	if(ptNetworkInfo->networkInterface == NULL) {
		PrintError("[DLNA/DMS] %s(%d) Error - Invalid NULL network interface!\n",__FUNCTION__,__LINE__);
		nDiRet = DI_ERR_INVALID_PARAM;
		goto func_error;
	}
	/*
	if(ptNetworkInfo->ipAddress == NULL) {
		PrintError("[DLNA/DMS] %s(%d) Error - Invalid NULL network ip!\n",__FUNCTION__,__LINE__);
		PrintError("[DLNA/DMS] %s(%d) Error - It's allowed!\n",__FUNCTION__,__LINE__);
		;
	}
	*/
	/* ......... */
	/*****************************************************************************/


	/* DMS Class */
	PrintData("[DLNA/DMS] Digital Media Server Version [%d.%d Build %s %s]\n", DMS_MAJOR_VER, DMS_MINOR_VER, DMS_BUILD_DATE, DMS_BUILD_TIME);
	if (_ghDmsDiHandle != NULL) {
		PrintError("[DLNA/DMS] %s(%d) Error - Already activated DMS handle!\n", __FUNCTION__,__LINE__);
		if ((_ghDmsDiHandle->hStartThread != NULL) &&
			(_ghDmsDiHandle->hStartThread->bThreadError == TRUE))
		{
			// Uninitialize if there was creating error for prev DMS task
			PrintError("[DLNA/DMS] %s(%d) Error - Remained prev error task!\n", __FUNCTION__,__LINE__);
			;
		}
		else if ((_ghDmsDiHandle->hStartThread != NULL) &&
			(_ghDmsDiHandle->hStartThread->bStartTaskRunning == TRUE))
		{
			// Waiting this??
			PrintError("[DLNA/DMS] %s(%d) Error - Remained prev running task!\n", __FUNCTION__,__LINE__);
			;
		}
		else
		{
			/*
			if (_ghDmsDiHandle->pDmsOpenSettings != NULL) {
				PrintError("[DLNA/DMS] %s(%d) Error - non-NULL setting!\n", __FUNCTION__,__LINE__);
				DD_MEM_Free(_ghDmsDiHandle->pDmsOpenSettings);
				_ghDmsDiHandle->pDmsOpenSettings = NULL;
			}

			DD_MEM_Free(_ghDmsDiHandle);
			_ghDmsDiHandle = NULL;
			*/
			nDiRet = DI_ERR_ERROR;
		 	goto func_error;
		}
	}

	// Prepare DMS DI handle
	_ghDmsDiHandle = (DMS_DI_HANDLE *)DD_MEM_Alloc(sizeof(DMS_DI_HANDLE));
	if (_ghDmsDiHandle == NULL) {
		PrintError("[DLNA/DMS] %s(%d) Error - Out of memory!\n",__FUNCTION__,__LINE__);
		nDiRet = DI_ERR_NO_RESOURCE;
		goto func_error;
	}
	VK_MEM_Memset(_ghDmsDiHandle, 0, sizeof(DMS_DI_HANDLE));

	// Prepare DMS settings
	_ghDmsDiHandle->pDmsOpenSettings = (BDlna_Dms_OpenSettings *)DD_MEM_Alloc(sizeof(BDlna_Dms_OpenSettings));
	if (_ghDmsDiHandle->pDmsOpenSettings == NULL) {
		PrintError("[DLNA/DMS] %s(%d) Error - Out of memory!\n",__FUNCTION__,__LINE__);
		nDiRet = DI_ERR_NO_RESOURCE;
		goto func_error;
	}
	VK_MEM_Memset(_ghDmsDiHandle->pDmsOpenSettings, 0, sizeof(BDlna_Dms_OpenSettings));

	// Prepare UPnP settings
	_ghDmsDiHandle->pUPnPOpenSettings = (BUPnPOpenSettings *)DD_MEM_Alloc(sizeof(BUPnPOpenSettings));
	if (_ghDmsDiHandle->pUPnPOpenSettings == NULL) {
		PrintError("[DLNA/DMS] %s(%d) Error - Out of memory!\n",__FUNCTION__,__LINE__);
		nDiRet = DI_ERR_NO_RESOURCE;
		goto func_error;
	}
	VK_MEM_Memset(_ghDmsDiHandle->pUPnPOpenSettings, 0, sizeof(BUPnPOpenSettings));

	if (_ghDmsDiHandle->pUPnPOpenSettings != NULL)
	{
		/* Specify which interface the library will get the IP address from */
		if (ptNetworkInfo->networkInterface != NULL) {
			if (_ghDmsDiHandle->pUPnPOpenSettings->networkInterface != NULL)
			{
				DD_MEM_Free((void *)_ghDmsDiHandle->pUPnPOpenSettings->networkInterface); // VK_MEM_Free??
				_ghDmsDiHandle->pUPnPOpenSettings->networkInterface = NULL;
			}

			_ghDmsDiHandle->pUPnPOpenSettings->networkInterface = VK_StrDup(ptNetworkInfo->networkInterface); // <-- free()
		}

		if(ptNetworkInfo->ipAddress == NULL) {
			/* If NULL, the library will automatically discover the IP address */
			_ghDmsDiHandle->pUPnPOpenSettings->ipAddress = NULL;
		}
		else {
			if(_ghDmsDiHandle->pUPnPOpenSettings->ipAddress != NULL)
			{
				DD_MEM_Free((void *)_ghDmsDiHandle->pUPnPOpenSettings->ipAddress);
				_ghDmsDiHandle->pUPnPOpenSettings->ipAddress = NULL;
			}

			_ghDmsDiHandle->pUPnPOpenSettings->ipAddress = VK_StrDup(ptNetworkInfo->ipAddress);
		}

    	if (ptNetworkInfo->port) {
			_ghDmsDiHandle->pUPnPOpenSettings->portNumber = ptNetworkInfo->port;
		}
		else {
			_ghDmsDiHandle->pUPnPOpenSettings->portNumber = _DI_DLNA_DMS_PORT_NUM;
		}
		_ghDmsDiHandle->pUPnPOpenSettings->ssdpRefreshPeriod = ptNetworkInfo->ssdpRefreshPeriod;
		if ((ptNetworkInfo->ssdpRefreshPeriod < 5) || (ptNetworkInfo->ssdpRefreshPeriod > 5000))
		{
			_ghDmsDiHandle->pUPnPOpenSettings->ssdpRefreshPeriod = _DI_DLNA_DMS_SSDP_REFRESH;
		}

		/* If set, the BDlna_Stop call will wait for all thread to terminate before returning */
		_ghDmsDiHandle->pUPnPOpenSettings->waitOnStop = ptNetworkInfo->waitOnStop;
	}

	if (_ghDmsDiHandle->pDmsOpenSettings != NULL)
	{
		// Added at appLibs_110831, but unused currently
		_ghDmsDiHandle->pDmsOpenSettings->client_auth = 0;
		_ghDmsDiHandle->pDmsOpenSettings->callback_auth_client = NULL;
		_ghDmsDiHandle->pDmsOpenSettings->callback_post_scan = NULL;
		_ghDmsDiHandle->pDmsOpenSettings->post_scan_acl_data = NULL;


		if(tDMSInfo->udn[0])
		{
	#if 0 // Removed VK_strlen() (in case of omitting null char from caller)
			ulStrLen = VK_strlen(tDMSInfo->udn);
			VK_strncpy(_ghDmsDiHandle->pDmsOpenSettings->udn, tDMSInfo->udn, ulStrLen);
			_ghDmsDiHandle->pDmsOpenSettings->udn[ulStrLen] = '\0';
	#else
			VK_strncpy(_ghDmsDiHandle->pDmsOpenSettings->udn, tDMSInfo->udn, sizeof(_ghDmsDiHandle->pDmsOpenSettings->udn)-1);
			_ghDmsDiHandle->pDmsOpenSettings->udn[sizeof(_ghDmsDiHandle->pDmsOpenSettings->udn)-1] = '\0';
	#endif
		}
		else
		{
			// Generating UUID/UDN is needed...
			VK_strncpy(_ghDmsDiHandle->pDmsOpenSettings->udn, "uuid:0E96FF1A-6771-2F44-A0C5-000102030405", sizeof(_ghDmsDiHandle->pDmsOpenSettings->udn)-1);
			_ghDmsDiHandle->pDmsOpenSettings->udn[sizeof(_ghDmsDiHandle->pDmsOpenSettings->udn)-1] = '\0';
		}

		if(tDMSInfo->interfaceName[0])
		{
			VK_strncpy(_ghDmsDiHandle->pDmsOpenSettings->interfaceName, tDMSInfo->interfaceName, sizeof(_ghDmsDiHandle->pDmsOpenSettings->interfaceName)-1);
			_ghDmsDiHandle->pDmsOpenSettings->interfaceName[sizeof(_ghDmsDiHandle->pDmsOpenSettings->interfaceName)-1] = '\0';
		}
		else
		{
			PrintError("[DLNA/DMS] %s(%d) Error - Invalid interfaceName arg!\n",__FUNCTION__,__LINE__);
			VK_strncpy(_ghDmsDiHandle->pDmsOpenSettings->interfaceName, "eth0", sizeof(_ghDmsDiHandle->pDmsOpenSettings->interfaceName)-1);
			_ghDmsDiHandle->pDmsOpenSettings->interfaceName[sizeof(_ghDmsDiHandle->pDmsOpenSettings->interfaceName)-1] = '\0';
		}

		if(tDMSInfo->wanInterfaceName[0])
		{
			VK_strncpy(_ghDmsDiHandle->pDmsOpenSettings->wanInterfaceName, tDMSInfo->wanInterfaceName, sizeof(_ghDmsDiHandle->pDmsOpenSettings->wanInterfaceName)-1);
			_ghDmsDiHandle->pDmsOpenSettings->wanInterfaceName[sizeof(_ghDmsDiHandle->pDmsOpenSettings->wanInterfaceName)-1] = '\0';
		}
		else
		{
			PrintError("[DLNA/DMS] %s(%d) Error - Invalid wanInterfaceName arg!\n",__FUNCTION__,__LINE__);
			VK_strncpy(_ghDmsDiHandle->pDmsOpenSettings->wanInterfaceName, "eth0", sizeof(_ghDmsDiHandle->pDmsOpenSettings->wanInterfaceName)-1);
			_ghDmsDiHandle->pDmsOpenSettings->wanInterfaceName[sizeof(_ghDmsDiHandle->pDmsOpenSettings->wanInterfaceName)-1] = '\0';
		}

		// Set shared directory including DB, info directory
		if(tDMSInfo->metaDbDir[0])
		{
			VK_strncpy(_ghDmsDiHandle->pDmsOpenSettings->metaDbDir, tDMSInfo->metaDbDir, sizeof(_ghDmsDiHandle->pDmsOpenSettings->metaDbDir)-1);
			_ghDmsDiHandle->pDmsOpenSettings->metaDbDir[sizeof(_ghDmsDiHandle->pDmsOpenSettings->metaDbDir)-1] = '\0';
		}
		else
		{
			PrintError("[DLNA/DMS] %s(%d) Error - Invalid metaDbDir arg! Use default configuration(/tmp)\n",__FUNCTION__,__LINE__);
			VK_strncpy(_ghDmsDiHandle->pDmsOpenSettings->metaDbDir, "/tmp", sizeof(_ghDmsDiHandle->pDmsOpenSettings->metaDbDir)-1);
			_ghDmsDiHandle->pDmsOpenSettings->metaDbDir[sizeof(_ghDmsDiHandle->pDmsOpenSettings->metaDbDir)-1] = '\0';
		}

		if(tDMSInfo->metaInfoDir[0])
		{
			VK_strncpy(_ghDmsDiHandle->pDmsOpenSettings->metaInfoDir, tDMSInfo->metaInfoDir, sizeof(_ghDmsDiHandle->pDmsOpenSettings->metaInfoDir)-1);
			_ghDmsDiHandle->pDmsOpenSettings->metaInfoDir[sizeof(_ghDmsDiHandle->pDmsOpenSettings->metaInfoDir)-1] = '\0';
		}
		else
		{
			PrintError("[DLNA/DMS] %s(%d) Error - Invalid metaInfoDir arg!\n",__FUNCTION__,__LINE__);
			VK_strncpy(_ghDmsDiHandle->pDmsOpenSettings->metaInfoDir, "/mnt/hd2", sizeof(_ghDmsDiHandle->pDmsOpenSettings->metaInfoDir)-1);
			_ghDmsDiHandle->pDmsOpenSettings->metaInfoDir[sizeof(_ghDmsDiHandle->pDmsOpenSettings->metaInfoDir)-1] = '\0';
		}

		if(tDMSInfo->contentDir[0])
		{
			VK_strncpy(_ghDmsDiHandle->pDmsOpenSettings->contentDir, tDMSInfo->contentDir, sizeof(_ghDmsDiHandle->pDmsOpenSettings->contentDir)-1);
			_ghDmsDiHandle->pDmsOpenSettings->contentDir[sizeof(_ghDmsDiHandle->pDmsOpenSettings->contentDir)-1] = '\0';
		}
		else
		{
			PrintError("[DLNA/DMS] %s(%d) Error - Invalid contentDir arg!\n",__FUNCTION__,__LINE__);
			VK_strncpy(_ghDmsDiHandle->pDmsOpenSettings->contentDir, "/mnt/hd2", sizeof(_ghDmsDiHandle->pDmsOpenSettings->contentDir)-1);
			_ghDmsDiHandle->pDmsOpenSettings->contentDir[sizeof(_ghDmsDiHandle->pDmsOpenSettings->contentDir)-1] = '\0';
		}

		// recordDir, Don't use if it is undefined.
		if(tDMSInfo->recordDir[0])
		{
			VK_strncpy(_ghDmsDiHandle->pDmsOpenSettings->recordDir, tDMSInfo->recordDir, sizeof(_ghDmsDiHandle->pDmsOpenSettings->recordDir)-1);
			_ghDmsDiHandle->pDmsOpenSettings->recordDir[sizeof(_ghDmsDiHandle->pDmsOpenSettings->recordDir)-1] = '\0';
		}
		else
		{
			// Don't set default value:
			PrintError("[DLNA/DMS] %s(%d) Warning - undesignated recordDir! It's OK...\n",__FUNCTION__,__LINE__);
			VK_MEM_Memset(_ghDmsDiHandle->pDmsOpenSettings->recordDir, 0x0, sizeof(_ghDmsDiHandle->pDmsOpenSettings->recordDir));
			_ghDmsDiHandle->pDmsOpenSettings->recordDir[0] = '\0';
		}

		// extMediaDir, unused if undesignated...
		if ((tDMSInfo->extMediaDir != NULL) && (tDMSInfo->extMediaDir[0] != '\0'))
		{
			VK_strncpy(_ghDmsDiHandle->pDmsOpenSettings->extMediaDir, tDMSInfo->extMediaDir, sizeof(_ghDmsDiHandle->pDmsOpenSettings->extMediaDir)-1);
			_ghDmsDiHandle->pDmsOpenSettings->extMediaDir[sizeof(_ghDmsDiHandle->pDmsOpenSettings->extMediaDir)-1] = '\0';
		}
		else {
			// Don't set default value:
			PrintError("[DLNA/DMS] %s(%d) Warning - undesignated extMediaDir! It's OK...\n",__FUNCTION__,__LINE__);
			VK_MEM_Memset(_ghDmsDiHandle->pDmsOpenSettings->extMediaDir, 0x0, sizeof(_ghDmsDiHandle->pDmsOpenSettings->extMediaDir));
			_ghDmsDiHandle->pDmsOpenSettings->extMediaDir[0] = '\0';
		}
	}

	if(tDMSInfo->pDeviceInfo != NULL)
	{
		if (_ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo != NULL) {
			PrintError("[DLNA/DMS] %s(%d) Error - Invalid polluted deviceInfo!\n",__FUNCTION__,__LINE__);
			DD_MEM_Free(_ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo);
			_ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo = NULL;
		}

		_ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo = (BDlna_DeviceInfo*)DD_MEM_Alloc(sizeof(BDlna_DeviceInfo));
		if (_ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo == NULL) {
			PrintError("[DLNA/DMS] %s(%d) Error! Out of memory!\n", __func__, __LINE__);
			nDiRet = DI_ERR_ERROR;
			/* free() for handle alloc() is needed */
			goto func_error;
		}
		VK_MEM_Memset(_ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo, 0x0, sizeof(BDlna_DeviceInfo));

		// Is (tDMSInfo->pDeviceInfo->friendlyName) var from app null-terminated?
		// ======================================================================
		// 	void *VK_StrDup(void *p)
		// 	{
		// 		char *pstr, *pNewStr;
		// 		unsigned long	strSize;
		//
		// 		if (p == NULL)
		// 			return NULL;
		// 		pstr = p;
		// 		strSize = VK_strlen(p);                 <--
		// 		pNewStr = DD_MEM_Alloc(strSize+1);
		// 		if (pNewStr == NULL)
		// 			return NULL;
		// 		VK_strncpy(pNewStr, pstr, strSize);
		// 		pNewStr[strSize] = 0;                <--
		// 		return pNewStr;
		// 	}

		if (tDMSInfo->pDeviceInfo->friendlyName != NULL) {
			_ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->friendlyName = VK_StrDup(tDMSInfo->pDeviceInfo->friendlyName);
		}
		else {
			_ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->friendlyName = NULL;
		}

		if (tDMSInfo->pDeviceInfo->manufacturer != NULL) {
			_ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->manufacturer = VK_StrDup(tDMSInfo->pDeviceInfo->manufacturer);
		}
		else {
			_ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->manufacturer = NULL;
		}

		if (tDMSInfo->pDeviceInfo->manufacturerUrl != NULL) {
			_ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->manufacturerUrl = VK_StrDup(tDMSInfo->pDeviceInfo->manufacturerUrl);
		}
		else {
			_ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->manufacturerUrl = NULL;
		}

		if (tDMSInfo->pDeviceInfo->modelName != NULL) {
			_ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->modelName = VK_StrDup(tDMSInfo->pDeviceInfo->modelName);
		}
		else {
			_ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->modelName = NULL;
		}

		if (tDMSInfo->pDeviceInfo->modelNumber != NULL) {
			_ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->modelNumber = VK_StrDup(tDMSInfo->pDeviceInfo->modelNumber);
		}
		else {
			_ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->modelNumber = NULL;
		}

		if (tDMSInfo->pDeviceInfo->modelDescription != NULL) {
			_ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->modelDescription = VK_StrDup(tDMSInfo->pDeviceInfo->modelDescription);
		}
		else {
			_ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->modelDescription = NULL;
		}

		if (tDMSInfo->pDeviceInfo->modelUrl != NULL) {
			_ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->modelUrl = VK_StrDup(tDMSInfo->pDeviceInfo->modelUrl);
		}
		else {
			_ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->modelUrl = NULL;
		}

		if (tDMSInfo->pDeviceInfo->serialNumber != NULL) {
			_ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->serialNumber = VK_StrDup(tDMSInfo->pDeviceInfo->serialNumber);
		}
		else {
			_ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->serialNumber = NULL;
		}

		if (tDMSInfo->pDeviceInfo->udn != NULL) {
			_ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->udn = VK_StrDup(tDMSInfo->pDeviceInfo->udn);
		}
		else {
			// _ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->udn = NULL;
			_ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->udn = VK_StrDup("uuid:0E96FF1A-6771-2F44-A0C5-000102030405");
		}

		if (tDMSInfo->pDeviceInfo->upc != NULL) {
			_ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->upc = VK_StrDup(tDMSInfo->pDeviceInfo->upc);
		}
		else {
			_ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->upc = NULL;
		}

		// DmsOpenSettings.friendlyName, not DmsOpenSettings.pDeviceInfo->friendlyName
		if (tDMSInfo->friendlyName != NULL) {
			_ghDmsDiHandle->pDmsOpenSettings->friendlyName = VK_StrDup(tDMSInfo->pDeviceInfo->friendlyName);
		}
		else {
			_ghDmsDiHandle->pDmsOpenSettings->friendlyName = NULL;
		}
	}

	_ghDmsDiHandle->pDmsOpenSettings->DTCPprotected = tDMSInfo->DTCPprotected;
	_ghDmsDiHandle->pDmsOpenSettings->enableThumbnailGen = tDMSInfo->enableThumbnailGen;
	_ghDmsDiHandle->pDmsOpenSettings->SupportXbox = tDMSInfo->SupportXbox;
	// client_auth
	// ssdpRefreshPeriod

	if (tDMSInfo->legalFileExtension != NULL) {
		_ghDmsDiHandle->pDmsOpenSettings->legalFileExtension = VK_StrDup(tDMSInfo->legalFileExtension);
	}
	else {
		// _ghDmsDiHandle->pDmsOpenSettings->legalFileExtension = NULL;
		_ghDmsDiHandle->pDmsOpenSettings->legalFileExtension = VK_StrDup("jpg:jpeg:jpe:png:wav:mp3:wma:m1a:m2a:m4a:mpa:mp4:ts:wmv:mpg:mpv:mpeg:asf:avi:divx:flv:mkv:m2ts:vob:m1v:m2v:m4v");
	}

	if (tDMSInfo->ContentXferPort != 0) 	{
		_ghDmsDiHandle->pDmsOpenSettings->ContentXferPort = tDMSInfo->ContentXferPort;
	}
	else {
		_ghDmsDiHandle->pDmsOpenSettings->ContentXferPort = 12345;
	}

	if (bDebugSetting == TRUE)
	{
		PrintData("[DLNA/DMS] %s(%d) ===============================\n", __FUNCTION__,__LINE__);
		PrintData("[DLNA/DMS] %s(%d) [DMS Info]\n", __FUNCTION__,__LINE__);
		PrintData("[DLNA/DMS] %s(%d) ===============================\n", __FUNCTION__,__LINE__);
		PrintData("[DLNA/DMS] %s(%d) + metaDbDir           = %s\n", __FUNCTION__, __LINE__, _ghDmsDiHandle->pDmsOpenSettings->metaDbDir );
		PrintData("[DLNA/DMS] %s(%d) + metaInfoDir         = %s\n", __FUNCTION__, __LINE__, _ghDmsDiHandle->pDmsOpenSettings->metaInfoDir );
		PrintData("[DLNA/DMS] %s(%d) + contentDir          = %s\n", __FUNCTION__, __LINE__, _ghDmsDiHandle->pDmsOpenSettings->contentDir );
		PrintData("[DLNA/DMS] %s(%d) + extMediaDir         = %s\n", __FUNCTION__, __LINE__, _ghDmsDiHandle->pDmsOpenSettings->extMediaDir );
		PrintData("[DLNA/DMS] %s(%d) + recordDir           = %s\n", __FUNCTION__, __LINE__, _ghDmsDiHandle->pDmsOpenSettings->recordDir );
		PrintData("[DLNA/DMS] %s(%d) + ContentXferPort     = %d\n", __FUNCTION__, __LINE__, _ghDmsDiHandle->pDmsOpenSettings->ContentXferPort );
		PrintData("[DLNA/DMS] %s(%d) + interfaceName       = %s\n", __FUNCTION__, __LINE__, _ghDmsDiHandle->pDmsOpenSettings->interfaceName );
		PrintData("[DLNA/DMS] %s(%d) + wanInterfaceName    = %s\n", __FUNCTION__, __LINE__, _ghDmsDiHandle->pDmsOpenSettings->wanInterfaceName );
		PrintData("[DLNA/DMS] %s(%d) + udn                 = %s\n", __FUNCTION__, __LINE__, _ghDmsDiHandle->pDmsOpenSettings->udn );
		PrintData("[DLNA/DMS] %s(%d) + DTCPprotected       = %d\n", __FUNCTION__, __LINE__, _ghDmsDiHandle->pDmsOpenSettings->DTCPprotected );
		PrintData("[DLNA/DMS] %s(%d) + friendlyName        = %s\n", __FUNCTION__, __LINE__, _ghDmsDiHandle->pDmsOpenSettings->friendlyName );
		PrintData("[DLNA/DMS] %s(%d) + SupportXbox         = %d\n", __FUNCTION__, __LINE__, _ghDmsDiHandle->pDmsOpenSettings->SupportXbox );
		PrintData("[DLNA/DMS] %s(%d) + legalFileExtension  = %s\n", __FUNCTION__, __LINE__, _ghDmsDiHandle->pDmsOpenSettings->legalFileExtension );
		PrintData("[DLNA/DMS] %s(%d) + enableThumbnailGen  = %d\n", __FUNCTION__, __LINE__, _ghDmsDiHandle->pDmsOpenSettings->enableThumbnailGen );
		// Added AppLibs_110831
		PrintData("[DLNA/DMS] %s(%d) + client_auth         = %d\n", __FUNCTION__, __LINE__, _ghDmsDiHandle->pDmsOpenSettings->client_auth );
	  	PrintData("[DLNA/DMS] %s(%d) + ssdpRefreshPeriod   = %d\n", __FUNCTION__, __LINE__, _ghDmsDiHandle->pDmsOpenSettings->ssdpRefreshPeriod );

		PrintData("[DLNA/DMS] %s(%d) ===============================\n", __FUNCTION__,__LINE__);
		PrintData("[DLNA/DMS] %s(%d) [Device Info]\n", __FUNCTION__,__LINE__);
		PrintData("[DLNA/DMS] %s(%d) ===============================\n", __FUNCTION__,__LINE__);
		PrintData("[DLNA/DMS] %s(%d) + friendlyName        = %s\n", __FUNCTION__, __LINE__, _ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->friendlyName );
		PrintData("[DLNA/DMS] %s(%d) + manufacturer        = %s\n", __FUNCTION__, __LINE__, _ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->manufacturer );
		PrintData("[DLNA/DMS] %s(%d) + manufacturerUrl     = %s\n", __FUNCTION__, __LINE__, _ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->manufacturerUrl );
		PrintData("[DLNA/DMS] %s(%d) + modelName           = %s\n", __FUNCTION__, __LINE__, _ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->modelName );
		PrintData("[DLNA/DMS] %s(%d) + modelNumber         = %s\n", __FUNCTION__, __LINE__, _ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->modelNumber );
		PrintData("[DLNA/DMS] %s(%d) + modelDescription    = %s\n", __FUNCTION__, __LINE__, _ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->modelDescription );
		PrintData("[DLNA/DMS] %s(%d) + modelUrl            = %s\n", __FUNCTION__, __LINE__, _ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->modelUrl );
		if (_ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->serialNumber)
		  PrintData("[DLNA/DMS] %s(%d) + serialNumber        = %s\n", __FUNCTION__, __LINE__, _ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->serialNumber);
		if (_ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->udn)
		  PrintData("[DLNA/DMS] %s(%d) + udn                 = %s\n", __FUNCTION__, __LINE__, _ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->udn);
		if (_ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->upc)
		  PrintData("[DLNA/DMS] %s(%d) + upc                 = %s\n", __FUNCTION__, __LINE__, _ghDmsDiHandle->pDmsOpenSettings->pDeviceInfo->upc);
	}

	if (1)
	{
		_ghDmsDiHandle->hStartThread = (struct stDMS_DI_RUNNING_THREAD *)DD_MEM_Alloc(sizeof(struct stDMS_DI_RUNNING_THREAD));
		if (_ghDmsDiHandle->hStartThread == NULL) {
			PrintError("[DLNA/DMS] %s(%d) Error - Out of memory!\n",__FUNCTION__,__LINE__);
			// What TODO??
		}
		VK_MEM_Memset(_ghDmsDiHandle->hStartThread , 0, sizeof(struct stDMS_DI_RUNNING_THREAD));
		_ghDmsDiHandle->hStartThread->bThreadError = FALSE;
		_ghDmsDiHandle->hStartThread->bBrcmScanRunning = FALSE;
		_ghDmsDiHandle->hStartThread->bStartTaskRunning = FALSE;

		nVkRet = VK_TASK_Create(P_DLNA_DMS_StartDmsTask,
								_DI_DLNA_DMS_TASK_PRIORITY,
								_DI_DLNA_DMS_STACK_SIZE,
								"P_DLNA_DMS_StartDmsTask",
	#if 0
								(void *)_ghDmsDiHandle, // or (void *)_ghDmsDiHandle->hStartThread,
	#else
								NULL,
	#endif
								&_ghDmsDiHandle->hStartThread->taskID,
								FALSE); // TRUE means JOINABLE
			if (nVkRet != VK_OK)
			{
				PrintError("[DLNA/DMS] %s(%d) Error! (0x%x) in VK_TASK_Create()\n", __FUNCTION__,__LINE__, nVkRet);
				_ghDmsDiHandle->hStartThread->bThreadError = TRUE;
				/* Must dealloc prev handle??? */
				nDiRet = DI_ERR_ERROR;
				goto func_error;
			}

			if (!_ghDmsDiHandle->hStartThread->taskID) {
				PrintError("[DLNA/DMS] %s(%d) Error! Failed TASK_Start(taskID:0x%x)\n",
							__FUNCTION__,__LINE__, (int)_ghDmsDiHandle->hStartThread->taskID);
				;
			}
			else {
				PrintData("[DLNA/DMS] %s(%d) --> TASK_Start(taskID:0x%x)\n",
							__FUNCTION__,__LINE__, (int)_ghDmsDiHandle->hStartThread->taskID);
				(void)VK_TASK_Start(_ghDmsDiHandle->hStartThread->taskID);
			}

		nDiRet = DI_ERR_OK;
	}
	else
	{
		(void)P_DLNA_DMS_CoreInitialize();
		_ghDmsDiHandle->hBrcmDms = BDlna_Dms_Create(_ghDmsDiHandle->pDmsOpenSettings);
		if (!_ghDmsDiHandle->hBrcmDms)  {
			PrintError("[DLNA/DMS] %s(%d) Error - Failed to Create brDMS!\n", __FUNCTION__,__LINE__);
			nDiRet = DI_ERR_ERROR;
			goto func_error;
		}
		else {
			_ghDmsDiHandle->bBrcmInternalTaskActivated = TRUE;
			(void)P_DLNA_DMS_CoreStart();
			nDiRet = DI_ERR_OK;
		}
	}

	PrintError("[DLNA/DMS] %s(%d) Success! --(nerr:0x%x)\n", __FUNCTION__,__LINE__, (int)nDiRet);
	PrintExit;
	return nDiRet;

func_error:
	/* Deallocate here if there was error */
	PrintError("[DLNA/DMS] %s(%d) Error! --(nerr:0x%x)\n", __FUNCTION__,__LINE__, (int)nDiRet);

	if (_ghDmsDiHandle != NULL)
	{
		if (_ghDmsDiHandle->pUPnPOpenSettings != NULL)
		{
			DD_MEM_Free(_ghDmsDiHandle->pUPnPOpenSettings);
			_ghDmsDiHandle->pUPnPOpenSettings = NULL;
		}

		if (_ghDmsDiHandle->pDmsOpenSettings != NULL)
		{
			DD_MEM_Free(_ghDmsDiHandle->pDmsOpenSettings);
			_ghDmsDiHandle->pDmsOpenSettings = NULL;
		}

		DD_MEM_Free(_ghDmsDiHandle);
		_ghDmsDiHandle = NULL;
	}

	PrintExit;
	return nDiRet;
}

// Need mutex lock...
static void P_DLNA_DMS_StopDmsTask(void *pParam)
{
	DMS_DI_HANDLE *hDms = (DMS_DI_HANDLE *) pParam;

//	lock(hDms->lock);
	PrintEnter;

	if (hDms == NULL) {
		PrintError("[DLNA/DMS] %s(%d) Error! Invalid NULL arg!\n", __FUNCTION__,__LINE__);
		goto func_error;
	}
	if (hDms->pDmsOpenSettings == NULL) {
		PrintError("[DLNA/DMS] %s(%d) Error! Invalid NULL DMS setting!\n", __FUNCTION__,__LINE__);
		goto func_error;
	}
	if (hDms->pUPnPOpenSettings == NULL) {
		PrintError("[DLNA/DMS] %s(%d) Error! Invalid NULL UPnP setting!\n", __FUNCTION__,__LINE__);
		goto func_error;
	}
	if (hDms->hBrcmDms == NULL) {
		PrintError("[DLNA/DMS] %s(%d) Error! Invalid NULL brcm handle!\n", __FUNCTION__,__LINE__);
		goto func_error;
	}

#if 0
	// Consider [UPnP->waitOnStop] var.
	// If set, the BDlna_Stop call will wait for all thread to terminate before returning
	UPnP->waitOnStop = ....;
#endif

	/* release resource */
	if (hDms->hStartThread)
	{
#if 0
		if (hDms->hStartThread)
		if (hDms->hStartThread->bThreadError == TRUE)
		if (hDms->hStartThread->bStartTaskRunning == TRUE)
		if (hDms->hStartThread->bBrcmScanRunning == TRUE)
			// ...
			// ...
			(void)VK_TASK_Sleep (_DI_DLNA_DMS_TASK_START_WATING_TIME); /* wait for starting the task */
#endif

		// Checking for task running is needed.
		VK_TASK_Stop(hDms->hStartThread->taskID);

		// Calling for TASK_Destroy() is needed although task is already finished.
		VK_TASK_Destroy(hDms->hStartThread->taskID);

		DD_MEM_Free(hDms->hStartThread);
		hDms->hStartThread = NULL;
	}

	(void)P_DLNA_DMS_CoreStop();
	(void)BDlna_Dms_Destroy(hDms->hBrcmDms);
	(void)P_DLNA_DMS_CoreUninitialize();
	hDms->bBrcmInternalTaskActivated = FALSE;

	// Free alloced mem(Alloc, StrDup)
	if (1)
	{
		// Free DMS settings
		if (hDms->pDmsOpenSettings != NULL)
		{
			if (hDms->pDmsOpenSettings->pDeviceInfo != NULL) {
				if (hDms->pDmsOpenSettings->pDeviceInfo->friendlyName != NULL) {
					// pDeviceInfo->friendlyName
					DD_MEM_Free(hDms->pDmsOpenSettings->pDeviceInfo->friendlyName);
					hDms->pDmsOpenSettings->pDeviceInfo->friendlyName = NULL;
				}
				if (hDms->pDmsOpenSettings->pDeviceInfo->manufacturer != NULL) {
					DD_MEM_Free(hDms->pDmsOpenSettings->pDeviceInfo->manufacturer);
					hDms->pDmsOpenSettings->pDeviceInfo->manufacturer = NULL;
				}
				if (hDms->pDmsOpenSettings->pDeviceInfo->manufacturerUrl != NULL) {
					DD_MEM_Free(hDms->pDmsOpenSettings->pDeviceInfo->manufacturerUrl);
					hDms->pDmsOpenSettings->pDeviceInfo->manufacturerUrl = NULL;
				}
				if (hDms->pDmsOpenSettings->pDeviceInfo->modelName != NULL) {
					DD_MEM_Free(hDms->pDmsOpenSettings->pDeviceInfo->modelName);
					hDms->pDmsOpenSettings->pDeviceInfo->modelName = NULL;
				}
				if (hDms->pDmsOpenSettings->pDeviceInfo->modelNumber != NULL) {
					DD_MEM_Free(hDms->pDmsOpenSettings->pDeviceInfo->modelNumber);
					hDms->pDmsOpenSettings->pDeviceInfo->modelNumber = NULL;
				}
				if (hDms->pDmsOpenSettings->pDeviceInfo->modelDescription != NULL) {
					DD_MEM_Free(hDms->pDmsOpenSettings->pDeviceInfo->modelDescription);
					hDms->pDmsOpenSettings->pDeviceInfo->modelDescription = NULL;
				}
				if (hDms->pDmsOpenSettings->pDeviceInfo->modelUrl != NULL) {
					DD_MEM_Free(hDms->pDmsOpenSettings->pDeviceInfo->modelUrl);
					hDms->pDmsOpenSettings->pDeviceInfo->modelUrl = NULL;
				}
				if (hDms->pDmsOpenSettings->pDeviceInfo->serialNumber != NULL) {
					DD_MEM_Free(hDms->pDmsOpenSettings->pDeviceInfo->serialNumber);
					hDms->pDmsOpenSettings->pDeviceInfo->serialNumber = NULL;
				}
				if (hDms->pDmsOpenSettings->pDeviceInfo->udn != NULL) {
					DD_MEM_Free(hDms->pDmsOpenSettings->pDeviceInfo->udn);
					hDms->pDmsOpenSettings->pDeviceInfo->udn = NULL;
				}
				if (hDms->pDmsOpenSettings->pDeviceInfo->upc != NULL) {
					DD_MEM_Free(hDms->pDmsOpenSettings->pDeviceInfo->upc);
					hDms->pDmsOpenSettings->pDeviceInfo->upc = NULL;
				}

				DD_MEM_Free(hDms->pDmsOpenSettings->pDeviceInfo);
				hDms->pDmsOpenSettings->pDeviceInfo = NULL;
			}

			if (hDms->pDmsOpenSettings->friendlyName != NULL) { // settings.friendlyName
				DD_MEM_Free(hDms->pDmsOpenSettings->friendlyName);
				hDms->pDmsOpenSettings->friendlyName = NULL;
			}
			if (hDms->pDmsOpenSettings->legalFileExtension != NULL) { // settings.friendlyName
				DD_MEM_Free(hDms->pDmsOpenSettings->legalFileExtension);
				hDms->pDmsOpenSettings->legalFileExtension = NULL;
			}
			if (hDms->pDmsOpenSettings != NULL) {
				DD_MEM_Free(hDms->pDmsOpenSettings);
				hDms->pDmsOpenSettings = NULL;
			}
		} // DMS settings

		// Free UPnP settings
		if (hDms->pUPnPOpenSettings != NULL)
		{
			if (hDms->pUPnPOpenSettings->ipAddress != NULL) { // settings.ipAddress
				DD_MEM_Free((void *)hDms->pUPnPOpenSettings->ipAddress);
				hDms->pUPnPOpenSettings->ipAddress = NULL;
			}
			if (hDms->pUPnPOpenSettings->networkInterface != NULL) { // settings.networkInterface
				DD_MEM_Free((void *)hDms->pUPnPOpenSettings->networkInterface);
				hDms->pUPnPOpenSettings->networkInterface = NULL;
			}
			if (hDms->pUPnPOpenSettings != NULL) {
				DD_MEM_Free(hDms->pUPnPOpenSettings);
				hDms->pUPnPOpenSettings = NULL;
			} // UPnP settings
		}

		if (hDms->hStartThread != NULL) {
			DD_MEM_Free(hDms->hStartThread);
			hDms->hStartThread = NULL;
		}

		/* Delete handle */
		DD_MEM_Free(hDms);
		hDms = NULL;
	}

	PrintExit;
//	unlock(hDms->lock);
	return;

func_error:
	// Do something if error occurs...
	PrintExit;
//	unlock(hDms->lock);
	return;
}

/**
 *	Function: DI_DLNA_StopDMS
 *	Description: DMS를 Stop하는 함수
 *  Param: 	void
 *
 *	Return:  DI_ERR_CODE
 *
 */
DI_ERR_CODE	DI_DLNA_StopDMS(void)
{
	DI_ERR_CODE nDiRet = DI_ERR_OK;
	DMS_DI_HANDLE *hDms = _ghDmsDiHandle;
	//HINT32 nVkRet = 0;

	PrintEnter;

	if (hDms == NULL) { //already die or not starrted
		PrintError("[DMS] %s(%d) Error - Already uninitialized DMS handle!\n", __FUNCTION__, __LINE__);
		nDiRet = DI_ERR_ERROR;
		goto func_error;
	}

	if (hDms->bBrcmInternalTaskActivated == FALSE)
	{
		PrintError("[DMS] %s(%d) Error - Already deactivated brcmDMS!\n", __FUNCTION__, __LINE__);
		nDiRet = DI_ERR_ERROR;
		goto func_error;
	}

	//if (hDms->bBrcmInternalTaskActivated == TRUE)
	//{
	//	PrintError("[DMS] %s(%d) Error - Running bDMS service!\n", __FUNCTION__, __LINE__);
	//	return DI_ERR_ERROR;
	//}

	// Process with function call, not thread
#if 0
	hDms->hStopThread = (struct stDMS_DI_RUNNING_THREAD *)DD_MEM_Alloc(sizeof(struct stDMS_DI_RUNNING_THREAD));
	if (hDms->hStopThread == NULL) {
		PrintError("[DLNA/DMS] %s(%d) Error - Out of memory!\n",__FUNCTION__,__LINE__);
		// What todo??
	}
	VK_MEM_Memset(hDms->hStopThread , 0, sizeof(struct stDMS_DI_RUNNING_THREAD));
	hDms->hStopThread->bThreadError = FALSE;
	hDms->hStopThread->bBrcmScanRunning = FALSE;
	hDms->hStopThread->bStopTaskRunning = FALSE;

	nVkRet = VK_TASK_Create(P_DLNA_DMS_StopDmsTask,
						_DI_DLNA_DMS_TASK_PRIORITY,
						_DI_DLNA_DMS_STACK_SIZE,
						"P_DLNA_DMS_StopDmsTask",
						(void *)hDms, // (void *)hDms->hStopThread,
						&hDms->hStopThread->taskID,
						FALSE); /* TRUE means JOINABLE */
	if (nVkRet != VK_OK)
	{
		PrintError("[DLNA/DMS] %s(%d) Error! (0x%x) in VK_TASK_Create()\n", __FUNCTION__,__LINE__, nVkRet);
		hDms->hStopThread->bThreadError = TRUE;
		/* Must dealloc prev handle??? */
		nDiRet = DI_ERR_ERROR;
		goto func_error;
	}
	PrintError("[DLNA/DMS] %s(%d) --> TASK_Stop(taskID:0x%x)\n", __FUNCTION__,__LINE__, (int)hDms->hStopThread->taskID);
	(void)VK_TASK_Stop(hDms->hStopThread->taskID);
#else
	P_DLNA_DMS_StopDmsTask((void *)hDms);  // hDms->hStartThread,
#endif

func_error:
	PrintExit;
	return nDiRet;
}

// Don't use this function
void DI_DLNA_SetDlnaCore(bool bCoreStart, DI_DLNA_DEVICE_TYPE eDeviceType)
{
	UNUSED_PARAMETER(bCoreStart);
	UNUSED_PARAMETER(eDeviceType);
	//P_DLNA_StartDlnaCore(bCoreStart, eDeviceType);
	return;
}

#if 1 // Dont use this function: BRCM supporting is needed.
DI_ERR_CODE	P_DLNA_DMS_RescanAllDirectory(void)
{
	DI_ERR_CODE nDiRet = DI_ERR_OK;
	DMS_DI_HANDLE *hDms = _ghDmsDiHandle;

	PrintEnter;

	if (hDms == NULL) {
		PrintError("[DMS] %s(%d) Error - Uninitialized DMS handle!\n", __FUNCTION__, __LINE__);
		nDiRet = DI_ERR_ERROR;
		goto func_error;
	}

	if (hDms->pDmsOpenSettings == NULL)
	{
		PrintError("[DMS] %s(%d) Error - Invalid NULL setting!\n", __FUNCTION__, __LINE__);
		nDiRet = DI_ERR_ERROR;
		goto func_error;
	}

	if (hDms->bBrcmInternalTaskActivated == FALSE)
	{
		PrintError("[DMS] %s(%d) Error - Already stopped DMS service!\n", __FUNCTION__, __LINE__);
		nDiRet = DI_ERR_ERROR;
		goto func_error;
	}

	if ((hDms->pDmsOpenSettings->extMediaDir != NULL) &&
		(hDms->pDmsOpenSettings->extMediaDir[0] != '\0'))
	{
		PrintError("[DMS] %s(%d) --> +BDlna_Dms_ReScan[extMediaDir:%s]\n", __FUNCTION__, __LINE__, hDms->pDmsOpenSettings->contentDir);
		BDlna_Dms_ReScan(hDms->pDmsOpenSettings, (char *)hDms->pDmsOpenSettings->extMediaDir);
		PrintError("[DMS] %s(%d) --> -BDlna_Dms_ReScan[extMediaDir:%s]\n", __FUNCTION__, __LINE__, hDms->pDmsOpenSettings->contentDir);
	}
	else {
		PrintError("[DLNA/DMS] %s(%d) Warning - Couldn't find the extMediaDir\n",__FUNCTION__,__LINE__);
	}

	// recordDir??

	PrintError("[DMS] %s(%d) [contentDir:%s]\n", __FUNCTION__, __LINE__, hDms->pDmsOpenSettings->contentDir);
	if (hDms->pDmsOpenSettings->contentDir[0])

	if ((hDms->pDmsOpenSettings->contentDir != NULL) &&
		(hDms->pDmsOpenSettings->contentDir[0] != '\0'))
	{
		PrintError("[DMS] %s(%d) --> +BDlna_Dms_ReScan[contentDir:%s]\n", __FUNCTION__, __LINE__, hDms->pDmsOpenSettings->contentDir);
		BDlna_Dms_ReScan(hDms->pDmsOpenSettings, (char *)hDms->pDmsOpenSettings->contentDir);
		PrintError("[DMS] %s(%d) --> -BDlna_Dms_ReScan[contentDir:%s]\n", __FUNCTION__, __LINE__, hDms->pDmsOpenSettings->contentDir);
	}
	else {
		PrintError("[DLNA/DMS] %s(%d) Warning - Couldn't find the extMediaDir\n",__FUNCTION__,__LINE__);
	}

#if 0
	BDlna_Dms_UpdateMeta();
#endif

func_error:
	PrintExit;
	return nDiRet;
}

DI_ERR_CODE	P_DLNA_DMS_RescanContentDirectory(void)
{
	DI_ERR_CODE nDiRet = DI_ERR_OK;
	DMS_DI_HANDLE *hDms = _ghDmsDiHandle;

	PrintEnter;

	if (hDms == NULL) {
		PrintError("[DMS] %s(%d) Error - Uninitialized DMS handle!\n", __FUNCTION__, __LINE__);
		nDiRet = DI_ERR_ERROR;
		goto func_error;
	}

	if (hDms->pDmsOpenSettings == NULL)
	{
		PrintError("[DMS] %s(%d) Error - Invalid NULL setting!\n", __FUNCTION__, __LINE__);
		nDiRet = DI_ERR_ERROR;
		goto func_error;
	}

	if (hDms->bBrcmInternalTaskActivated == FALSE)
	{
		PrintError("[DMS] %s(%d) Error - Already stopped DMS service!\n", __FUNCTION__, __LINE__);
		nDiRet = DI_ERR_ERROR;
		goto func_error;
	}

	// recordDir[128]??

	PrintError("[DMS] %s(%d) [contentDir:%s]\n", __FUNCTION__, __LINE__, hDms->pDmsOpenSettings->contentDir);
	if (hDms->pDmsOpenSettings->contentDir[0])

	if ((hDms->pDmsOpenSettings->contentDir != NULL) &&
		(hDms->pDmsOpenSettings->contentDir[0] != '\0'))
	{
		PrintError("[DMS] %s(%d) --> +BDlna_Dms_ReScan[contentDir:%s]\n", __FUNCTION__, __LINE__, hDms->pDmsOpenSettings->contentDir);
		BDlna_Dms_ReScan(hDms->pDmsOpenSettings, (char *)hDms->pDmsOpenSettings->contentDir);
		PrintError("[DMS] %s(%d) --> +BDlna_Dms_ReScan[contentDir:%s]\n", __FUNCTION__, __LINE__, hDms->pDmsOpenSettings->contentDir);
	}
	else {
		PrintError("[DLNA/DMS] %s(%d) Warning - Couldn't find the extMediaDir\n",__FUNCTION__,__LINE__);
	}

#if 0
	BDlna_Dms_UpdateMeta();
#endif

func_error:
	PrintExit;
	return nDiRet;
}

DI_ERR_CODE	P_DLNA_DMS_RescanExtDirectory(void)
{
	DI_ERR_CODE nDiRet = DI_ERR_OK;
	DMS_DI_HANDLE *hDms = _ghDmsDiHandle;

	PrintEnter;

	if (hDms == NULL) {
		PrintError("[DMS] %s(%d) Error - Uninitialized DMS handle!\n", __FUNCTION__, __LINE__);
		nDiRet = DI_ERR_ERROR;
		goto func_error;
	}

	if (hDms->pDmsOpenSettings == NULL)
	{
		PrintError("[DMS] %s(%d) Error - Invalid NULL setting!\n", __FUNCTION__, __LINE__);
		nDiRet = DI_ERR_ERROR;
		goto func_error;
	}

	if (hDms->bBrcmInternalTaskActivated == FALSE)
	{
		PrintError("[DMS] %s(%d) Error - Already stopped DMS service!\n", __FUNCTION__, __LINE__);
		nDiRet = DI_ERR_ERROR;
		goto func_error;
	}

	if ((hDms->pDmsOpenSettings->extMediaDir != NULL) &&
		(hDms->pDmsOpenSettings->extMediaDir[0] != '\0'))
	{
		PrintError("[DMS] %s(%d) --> +BDlna_Dms_ReScan[extMediaDir:%s]\n", __FUNCTION__, __LINE__, hDms->pDmsOpenSettings->contentDir);
		BDlna_Dms_ReScan(hDms->pDmsOpenSettings, (char *)hDms->pDmsOpenSettings->extMediaDir);
		PrintError("[DMS] %s(%d) --> +BDlna_Dms_ReScan[extMediaDir:%s]\n", __FUNCTION__, __LINE__, hDms->pDmsOpenSettings->contentDir);
	}
	else {
		PrintError("[DLNA/DMS] %s(%d) Warning - Couldn't find the extMediaDir\n",__FUNCTION__,__LINE__);
	}

#if 0
	BDlna_Dms_UpdateMeta();
#endif

func_error:
	PrintExit;
	return nDiRet;
}

/**
 *	Function: DI_DLNA_RescanDMS
 *	Description: DMS를 Stop하는 함수
 *  Param: 	void
 *
 *	Return:  DI_ERR_CODE
 *
 */
DI_ERR_CODE	DI_DLNA_RescanDMS(void)
{
	return P_DLNA_DMS_RescanAllDirectory();
}
#endif

/*@}*/
