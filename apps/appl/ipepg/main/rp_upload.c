/**************************************************************
 *	@file		rp_upload.c
 *	RP Rsvlist/Reclist/HddInfo upload for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/08/01
 *	@author			humax
 *	@brief			RP Rsvlist/Reclist/HddInfo upload for OCTO 2.0 Project
 **************************************************************/
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

#include <vkernel.h>
#include <hlib.h>
#include <hapi.h>
#include <oapi.h>
#include <apk.h>

#include "../include/ipepg.h"
#include "rp_common.h"

#if defined(CFG_RELEASE_DEBUG)
#undef HxLOG_Trace
#define HxLOG_Trace()	printf("[IPEPG] %s %d\n", __FUNCTION__, __LINE__);
#undef HxLOG_Debug
#define HxLOG_Debug		printf
#undef HxLOG_Print
#define HxLOG_Print		printf
#endif

#define RPURL_MAX_LEN		            256
#define EXT_MEDIA_FILE					".ts"

static HUINT8	*s_pstMount[9] =
{
	"sda", "sdb", "sdc", "sdd", "sde", "sdf",  "sdg", "sdh", "sdi"
};

const static STB_ELEMENT_t		s_stElementHdd[] =
{
	{ DEPTH_1, 	(HUINT8 *)"hdd",		PRINT_ATTR3, 	(HUINT8 *)"uuid",		(HUINT8 *)"mount",		(HUINT8 *)"type" 	},
	{ DEPTH_2, 	(HUINT8 *)"partition",	PRINT_ATTR1, 	(HUINT8 *)"format",		NULL,					NULL 				},
	{ DEPTH_3, 	(HUINT8 *)"sizefree",	PRINT_DECIMAL, 	NULL,					NULL,					NULL 				},
	{ DEPTH_3, 	(HUINT8 *)"sizeused",	PRINT_DECIMAL, 	NULL,					NULL,					NULL 				},
	{ DEPTH_3, 	(HUINT8 *)"sizetotal",	PRINT_DECIMAL, 	NULL,					NULL,					NULL 				},
	{ DEPTH_2, 	(HUINT8 *)"partition",	PRINT_CLOSING, 	NULL,					NULL,					NULL 				},
	{ DEPTH_1, 	(HUINT8 *)"hdd",		PRINT_CLOSING, 	NULL,					NULL,					NULL 				},
};

const static STB_ELEMENT_t		s_stElementRp[] =
{
	{ DEPTH_1, 	(HUINT8 *)"rplist",		PRINT_ATTR3, 	(HUINT8 *)"version",	(HUINT8 *)"country",	(HUINT8 *)"count" 	},
	{ DEPTH_2, 	(HUINT8 *)"rpdata",		PRINT_ATTR2, 	(HUINT8 *)"kind",		(HUINT8 *)"action",		NULL 				},
	{ DEPTH_3, 	(HUINT8 *)"rpindex",	PRINT_DECIMAL, 	NULL,					NULL,					NULL 				},
	{ DEPTH_3, 	(HUINT8 *)"result",		PRINT_DECIMAL, 	NULL,					NULL,					NULL 				},
	{ DEPTH_2, 	(HUINT8 *)"rpdata",		PRINT_CLOSING, 	NULL,					NULL,					NULL 				},
	{ DEPTH_1, 	(HUINT8 *)"rplist",		PRINT_CLOSING, 	NULL,					NULL,					NULL 				},
};

const static STB_ELEMENT_t		s_stElementRsv[] =
{
	{ DEPTH_1, 	(HUINT8 *)"rsvlist",	PRINT_ATTR2, 	(HUINT8 *)"version",	(HUINT8 *)"count",		NULL			 	},
	{ DEPTH_2, 	(HUINT8 *)"rsv",		PRINT_ATTR1, 	(HUINT8 *)"kind",		NULL,					NULL 				},
	{ DEPTH_3, 	(HUINT8 *)"chnum",		PRINT_DECIMAL, 	NULL,					NULL,					NULL 				},
	{ DEPTH_3, 	(HUINT8 *)"chname",		PRINT_STRING, 	NULL,					NULL,					NULL 				},
	{ DEPTH_3, 	(HUINT8 *)"hsvc",		PRINT_DECIMAL, 	NULL,					NULL,					NULL 				},
	{ DEPTH_3, 	(HUINT8 *)"uniqueid",	PRINT_DECIMAL, 	NULL,					NULL,					NULL 				},
	{ DEPTH_3, 	(HUINT8 *)"start",		PRINT_DECIMAL, 	NULL,					NULL,					NULL 				},
	{ DEPTH_3, 	(HUINT8 *)"end",		PRINT_DECIMAL, 	NULL,					NULL,					NULL 				},
	{ DEPTH_3, 	(HUINT8 *)"evtname",	PRINT_STRING, 	NULL,					NULL,					NULL 				},
	{ DEPTH_3, 	(HUINT8 *)"rsvtype",	PRINT_DECIMAL, 	NULL,					NULL,					NULL 				},
	{ DEPTH_3, 	(HUINT8 *)"repeat",		PRINT_DECIMAL, 	NULL,					NULL,					NULL 				},
	{ DEPTH_2, 	(HUINT8 *)"rsv",		PRINT_CLOSING, 	NULL,					NULL,					NULL 				},
	{ DEPTH_1, 	(HUINT8 *)"rsvlist",	PRINT_CLOSING, 	NULL,					NULL,					NULL 				},
};

const static STB_ELEMENT_t		s_stElementRec[] =
{
	{ DEPTH_1, 	(HUINT8 *)"reclist",	PRINT_ATTR2, 	(HUINT8 *)"version",	(HUINT8 *)"count",		NULL			 	},
	{ DEPTH_2, 	(HUINT8 *)"rec",		PRINT_OPENING, 	NULL,					NULL,					NULL 				},
	{ DEPTH_3, 	(HUINT8 *)"chnum",		PRINT_DECIMAL, 	NULL,					NULL,					NULL 				},
	{ DEPTH_3, 	(HUINT8 *)"chname",		PRINT_STRING, 	NULL,					NULL,					NULL 				},
	{ DEPTH_3, 	(HUINT8 *)"start",		PRINT_DECIMAL, 	NULL,					NULL,					NULL 				},
	{ DEPTH_3, 	(HUINT8 *)"end",		PRINT_DECIMAL, 	NULL,					NULL,					NULL 				},
	{ DEPTH_3, 	(HUINT8 *)"evtname",	PRINT_STRING, 	NULL,					NULL,					NULL 				},
	{ DEPTH_3, 	(HUINT8 *)"repeat",		PRINT_DECIMAL, 	NULL,					NULL,					NULL 				},
	{ DEPTH_3, 	(HUINT8 *)"size",		PRINT_DECIMAL, 	NULL,					NULL,					NULL 				},
	{ DEPTH_3, 	(HUINT8 *)"played",		PRINT_DECIMAL, 	NULL,					NULL,					NULL 				},
	{ DEPTH_3, 	(HUINT8 *)"eventid",	PRINT_DECIMAL, 	NULL,					NULL,					NULL 				},
	{ DEPTH_2, 	(HUINT8 *)"rec",		PRINT_CLOSING, 	NULL,					NULL,					NULL 				},
	{ DEPTH_1, 	(HUINT8 *)"reclist",	PRINT_CLOSING, 	NULL,					NULL,					NULL 				},
};

const static STB_ELEMENT_t		s_stElementAllowedList[] =
{
	{ DEPTH_1, 	(HUINT8 *)"allowedlist",	PRINT_OPENING, 	NULL,					NULL,					NULL			 	},
	{ DEPTH_2, 	(HUINT8 *)"deviceid",		PRINT_STRING, 	NULL,					NULL,					NULL 				},
	{ DEPTH_1, 	(HUINT8 *)"allowedlist",	PRINT_CLOSING, 	NULL,					NULL,					NULL 				},
};

const static STB_ELEMENT_t		s_stElementNogify[] =
{
	{ DEPTH_ROOT, 	(HUINT8 *)"notify",		PRINT_OPENING, 	NULL,					NULL,					NULL 				},
	{ DEPTH_ROOT, 	(HUINT8 *)"notify",		PRINT_CLOSING, 	NULL,					NULL,					NULL 				},
};

const static STB_ELEMENT_t		s_stElementTimeZone[] =
{
	{ DEPTH_1, 	(HUINT8 *)"timezone",		PRINT_DECIMAL, 	NULL,					NULL,					NULL 				},
};


const static STB_ELEMENTLIST_t		s_stElementListHdd 		= { STB_ELEMENT_LIST_HDD, 		(STB_ELEMENT_t *)s_stElementHdd	 	};
const static STB_ELEMENTLIST_t		s_stElementListRp 		= { STB_ELEMENT_LIST_RPLIST,	(STB_ELEMENT_t *)s_stElementRp	 	};
const static STB_ELEMENTLIST_t		s_stElementListRsv 		= { STB_ELEMENT_LIST_RSVLIST,	(STB_ELEMENT_t *)s_stElementRsv	 	};
const static STB_ELEMENTLIST_t		s_stElementListRec  	= { STB_ELEMENT_LIST_RECLIST,	(STB_ELEMENT_t *)s_stElementRec		};
const static STB_ELEMENTLIST_t		s_stElementListNotify 	= { STB_ELEMENT_LIST_NOTIFY,	(STB_ELEMENT_t *)s_stElementNogify	};
const static STB_ELEMENTLIST_t		s_stElementListTimezone = { STB_ELEMENT_LIST_TIMEZONE,	(STB_ELEMENT_t *)s_stElementTimeZone};
const static STB_ELEMENTLIST_t		s_stElementListAllowedList  = { STB_ELEMENT_LIST_ALLOWEDLIST,	(STB_ELEMENT_t *)s_stElementAllowedList};


static RP_REPLACESTR_t		s_astRpReplaceString[] =
{
		{	5,		FALSE,	{'&', 'a', 'm', 'p', ';', '\0', 0, },		0x26	},
		{	6,		TRUE,	{'&', 'q', 'u', 'o', 't', ';', '\0', 0, },	0x22	},
		{	4,		TRUE,	{'&', 'g', 't', ';', '\0', 0, },			0x3C	},
		{	4,		TRUE,	{'&', 'l', 't', ';', '\0', 0, },			0x3E	},

		// system specification 2.4.2
		// 5.3 programme data
		{	2, 		TRUE, 	{'\\', 'n', '\0', 0, }, 				0x0A	},
		{	2, 		TRUE, 	{'\\', 't', '\0', 0, }, 				0x09	},
		{	1, 		TRUE, 	{'\\', '\0', 0, },		 			0x5C	},
		{	0, 		FALSE, 	{0, }, 						0x00	}
};

static HERROR rp_client_process(RPList_t *pstRpList);
static HERROR 	rp_client_loadElement(HUINT32 uiMode, STB_ELEMENT_t **ppstElement);
static HERROR 	rp_client_common(STB_ELEMENT_t *pstElement, HUINT32 uiTagType, STB_ATTR_ELEMENT_t *pstAttr, STB_DataType_t *pstData);
static HERROR	rp_client_SetReplaceString(HUINT8 *pucSrcPtr, HUINT8 *pucDesPtr, HUINT32 *pucSaveSize, HUINT32 ulDeslen);

static HERROR rp_client_CreateAllowedList (HxVector_t **ppstAllowedList)
{
	HxVector_t		*pstAllowedList = NULL;
	HERROR			hErr = ERR_FAIL;
	HERROR			hResult = ERR_FAIL;
	HxXMLDOC 		hXmlDocHandle = NULL;
	HxXMLELEMENT	hRootElement = NULL;

	HxList_t		*pstDeviceIdHead = NULL;
	HxList_t		*pstDeviceIdIter = NULL;
	HxXMLELEMENT 	hDeviceId = NULL;

	HUINT8			*pszContent= NULL;

	pstAllowedList = HLIB_VECTOR_NewEasy(0, HLIB_MEM_Free_CB, NULL);
	IPEPG_GOTO_IF(pstAllowedList == NULL, END_FUNC);

	hErr = HLIB_XML_InitParser();
	IPEPG_GOTO_IF(hErr != ERR_OK, END_FUNC);

	hErr = HLIB_XML_ParseFile(RP_ALLOWED_DEVICE_FILE, &hXmlDocHandle);
	IPEPG_GOTO_IF(hErr != ERR_OK, END_FUNC);

	hErr = HLIB_XML_GetRootElement(hXmlDocHandle, &hRootElement);
	IPEPG_GOTO_IF(hErr != ERR_OK, END_FUNC);

	hErr = HLIB_XML_FindChildElementByName(hRootElement, "deviceid", &pstDeviceIdHead);
	IPEPG_GOTO_IF(hErr != ERR_OK, END_FUNC);

	for (pstDeviceIdIter = HLIB_LIST_First(pstDeviceIdHead); pstDeviceIdIter != NULL; pstDeviceIdIter = HLIB_LIST_Next(pstDeviceIdIter))
	{
		pszContent = NULL;

		hDeviceId = (HxXMLELEMENT)HLIB_LIST_Data(pstDeviceIdIter);
		if (hDeviceId == NULL)
		{
			HxLOG_Warning("null data \n");
			continue;
		}

		hErr = HLIB_XML_GetElementContents(hXmlDocHandle, hDeviceId, &pszContent);
		if (hErr != ERR_OK)
		{
			HxLOG_Warning("HLIB_XML_GetElementContents failed \n");
			continue;
		}

		if (pszContent != NULL)
		{
			HLIB_VECTOR_Add(pstAllowedList,HLIB_STD_StrDup(pszContent));
			HLIB_XML_FreeAttrContents(pszContent);
		}
	}

	hResult = ERR_OK;
	*ppstAllowedList = pstAllowedList;

END_FUNC:
	if(hXmlDocHandle != NULL)
	{
		HLIB_XML_FreeDocument(hXmlDocHandle);
	}

	if (pstDeviceIdHead != NULL)
	{
		HLIB_LIST_RemoveAll(pstDeviceIdHead);
	}
	return hResult;
}

static void     rp_client_TimeZoneProcess(void)
{
    HERROR              hErr = ERR_OK;
    STB_ELEMENT_t		*pstElement = NULL;
    HINT32				SysOffsetHour = 0, SysOffsetMin = 0;
    STB_DataType_t		stData;
	HxLOG_Trace();

    /* TimeZone */
	hErr = HLIB_DATETIME_GetOffsetTime (&SysOffsetHour, &SysOffsetMin);
	if(hErr == ERR_OK)
	{
        /* element load */
    	hErr = rp_client_loadElement(STB_ELEMENT_LIST_TIMEZONE, &pstElement);
    	if( hErr == ERR_OK )
    	{
    		stData.uiDecimal = SysOffsetHour * 60 + SysOffsetMin;
			hErr = rp_client_common(pstElement, STB_TIMEZONE, NULL, &stData);
    	}
	}
    else
    {
        HxLOG_Warning("[rp_client_TimeZoneProcess] HLIB_DATETIME_GetOffsetTime fail\n");
    }
}

static void rp_client_HardDiskProcess(void)
{
	HERROR			 		hErr = ERR_OK;
	STB_ELEMENT_t			*pstElement = NULL;
	HUINT8					szAttrBuf[10];
	STB_ATTR_ELEMENT_t		stAttr;
	STB_DataType_t			stData;
	HxLOG_Trace();

	/* element load */
	hErr = rp_client_loadElement(STB_ELEMENT_LIST_HDD, &pstElement);
	if( hErr == ERR_OK )
	{

		DxStorage_Info_t* deviceInfoList;
		HUINT32 	deviceInfoListCount, i, j;

		hErr = APK_FILEMGR_STORAGE_GetList(eDxSTORAGE_FILTER_Hdd,  &deviceInfoList, &deviceInfoListCount);


		if (deviceInfoList)
		{
			HxLOG_Debug("[rp_client_HardDiskProcess] 1. deviceInfoListCount=%d\n", deviceInfoListCount);
			for (i=0; i<deviceInfoListCount; i++)
			{
				DxStorage_Info_t* devInfo = &(deviceInfoList[i]);

				/* <hdd uuid="" mount="" type="" > */
				HxSTD_MemSet(&stAttr, 0x00, sizeof(STB_ATTR_ELEMENT_t));
				stAttr.pucAttr1 = devInfo->szUUID;
				stAttr.pucAttr2 = (devInfo->ulFlags & eDxSTORAGE_FLAG_MOUNTED ? s_pstMount[i] : NULL);

				switch(devInfo->eDevType)
				{
					case eDxSTORAGE_DEVTYPE_NONE:
						snprintf(szAttrBuf, 10, "%d", eDxSTORAGE_DEVTYPE_NONE);
						break;
					case eDxSTORAGE_DEVTYPE_INTHDD:
						snprintf(szAttrBuf, 10, "%d", eDxSTORAGE_DEVTYPE_INTHDD);
						break;
					case eDxSTORAGE_DEVTYPE_DETACHABLE:
					case eDxSTORAGE_DEVTYPE_ESATAHDD:
					case eDxSTORAGE_DEVTYPE_USBHDD:
						snprintf(szAttrBuf, 10, "%d", eDxSTORAGE_DEVTYPE_DETACHABLE);
						break;
					default:
						break;
				}

				stAttr.pucAttr3 = szAttrBuf;
				rp_client_common(pstElement, STB_HDD_OPEN, &stAttr, NULL);

				HxLOG_Debug("  ulDeviceId: 0x%08x\n", devInfo->ulDeviceId);
				HxLOG_Debug("	 ulFlags: 0x%x\n", devInfo->ulFlags);
				HxLOG_Debug("	 eDevType: %u\n", devInfo->eDevType);
				HxLOG_Debug("	 szUUID: %s\n", devInfo->szUUID);
				HxLOG_Debug("	 szDevName: %s\n", devInfo->szDevName);
				HxLOG_Debug("	 ulPartitionNum: %d\n", devInfo->nPartitionNum);

				for (j=0; j<devInfo->nPartitionNum; j++)
				{
					if ((devInfo->astPartitionInfos[j].bAvailable == TRUE) && (devInfo->astPartitionInfos[j].ulFlags & eDxSTORAGE_FLAG_MOUNTED) && ((devInfo->astPartitionInfos[j].ulFlags & eDxSTORAGE_FLAG_PVRDEVICE) || (devInfo->astPartitionInfos[j].ulFlags & eDxSTORAGE_FLAG_SUBPVRDEVICE)))
					{
						HUINT64 ullTotalSize = 0;
						HUINT64 ullUsedSize = 0;
						HUINT64 ullAvailSize = 0;
						HUINT64 ullTSRUsedSize = 0;

						if(HLIB_DIR_IsMountedPath(devInfo->astPartitionInfos[j].szMountPath) != TRUE)
						{
							HxLOG_Critical("Critical Error!!! This is not mounted path : %s\n", devInfo->astPartitionInfos[j].szMountPath);
							continue;
						}

						hErr = HLIB_STORAGE_GetPartitionSize((HCHAR *)devInfo->astPartitionInfos[j].szMountPath, &ullTotalSize, &ullUsedSize, NULL);
						hErr |= HLIB_STORAGE_GetPathSize(devInfo->astPartitionInfos[j].szTsrPath, &ullTSRUsedSize);

						if (hErr == ERR_OK)
						{
							if (ullTSRUsedSize > 0)
							{
								ullTSRUsedSize /= SIZE_1K;
								// TSR은 reserved 영역이므로 도출된 used size에서 빼준다.
								if(ullUsedSize >= ullTSRUsedSize)
									ullUsedSize -=	ullTSRUsedSize;
							}

							if((ullTotalSize - ullUsedSize) > DxSVC_FS_PVR_RESERVED_SIZE_KB )
								ullAvailSize = ((ullTotalSize - ullUsedSize) - DxSVC_FS_PVR_RESERVED_SIZE_KB);
							else
								ullAvailSize = 0;
							/* Partition File System Type */
							/* <partition format=""> */
							HxSTD_MemSet(&stAttr, 0x00, sizeof(STB_ATTR_ELEMENT_t));
							snprintf(szAttrBuf, 10, "%d", devInfo->astPartitionInfos[j].eFsType);
							stAttr.pucAttr1 = szAttrBuf;
							rp_client_common(pstElement, STB_HDD_PARTITION_OPEN, &stAttr, NULL);

							/* <sizefree> </sizefree> */
							stData.uiDecimal = (HUINT32)(ullAvailSize*1.073741);
							rp_client_common(pstElement, STB_HDD_SIZEFREE, NULL, &stData);

							/* <sizeused> </sizeused> */
							stData.uiDecimal = (HUINT32)(ullUsedSize*1.073741); // 1.073 ~~ (1.024*1.024*1.024)
							rp_client_common(pstElement, STB_HDD_SIZEUSED, NULL, &stData);

							/* <sizetotal> </sizetotal> */
							stData.uiDecimal = (HUINT32)(ullTotalSize*1.073741); // 1.073 ~~ (1.024*1.024*1.024)
							rp_client_common(pstElement, STB_HDD_SIZETOTAL, NULL, &stData);
							rp_client_common(pstElement, STB_HDD_PARTITION_CLOSE, NULL, NULL);

							HxLOG_Debug("	   ulTotalSizeKb: %d\n", ullTotalSize);
							HxLOG_Debug("	   ulTotalSizeGb: %d\n", ullTotalSize /(1024*1024));
						}
					}
					HxLOG_Debug("	 ulPartitionIdx: %d\n", j);
					HxLOG_Debug("	   bAvailable: %d\n", devInfo->astPartitionInfos[j].bAvailable);
					HxLOG_Debug("	   eFsType: %u\n", devInfo->astPartitionInfos[j].eFsType);
					HxLOG_Debug("	   bMounted: %d\n", (devInfo->astPartitionInfos[j].ulFlags & eDxSTORAGE_FLAG_MOUNTED));
					HxLOG_Debug("	   bPvrFlag: %d\n", ((devInfo->astPartitionInfos[j].ulFlags & eDxSTORAGE_FLAG_PVRDEVICE) || (devInfo->astPartitionInfos[j].ulFlags & eDxSTORAGE_FLAG_SUBPVRDEVICE)));
					HxLOG_Debug("	   szMountPath: %s\n", devInfo->astPartitionInfos[j].szMountPath);
				}
				rp_client_common(pstElement, STB_HDD_CLOSE, NULL, NULL);

			}
		}
		else
		{
			HxLOG_Warning("[rp_client_HardDiskProcess] APK_SQCMS_INFO_GetList Error!!! \n");
		}

		APK_FILEMGR_STORAGE_FreeList(deviceInfoList, deviceInfoListCount);
	}
}

static void rp_client_RpListProcess(RPList_t *pstRpList)
{
	HERROR			 		hErr = ERR_FAIL;
	STB_ELEMENT_t			*pstElement = NULL;
	HUINT32					uiIndex;
	STB_ATTR_ELEMENT_t		stAttr;
	STB_DataType_t			stData;
	HUINT8					szAttrBuf[3][10];
	HxLOG_Trace();

    if(pstRpList->pstRpData != NULL)
    {
        /* element load */
    	hErr = rp_client_loadElement(STB_ELEMENT_LIST_RPLIST, &pstElement);
    	if( hErr == ERR_OK )
    	{
            /* RPLIST ATTLIST */
       		HxSTD_MemSet(&stAttr, 0x00, sizeof(STB_ATTR_ELEMENT_t));
        	snprintf(szAttrBuf[0], 10, "%d", pstRpList->stAttr.ucVersion);
        	snprintf(szAttrBuf[1], 10, "%d", pstRpList->stAttr.ucCountry);
        	snprintf(szAttrBuf[2], 10, "%d", pstRpList->stAttr.ucCount);
        	stAttr.pucAttr1 = szAttrBuf[0];
        	stAttr.pucAttr2 = szAttrBuf[1];
        	stAttr.pucAttr3 = szAttrBuf[2];

        	/* <rplist version ="" country="" count=""> */
			rp_client_common(pstElement, STB_RPLIST_OPEN, &stAttr, NULL);

        	for(uiIndex=0; uiIndex<pstRpList->stAttr.ucCount; uiIndex++)
        	{
        		/* <rpdata kind="" action=""> */
        		HxSTD_MemSet(&stAttr, 0x00, sizeof(STB_ATTR_ELEMENT_t));
        		snprintf(szAttrBuf[0], 10, "%d", pstRpList->pstRpData[uiIndex].usKind);
        		snprintf(szAttrBuf[1], 10, "%d", pstRpList->pstRpData[uiIndex].usAction);
        		stAttr.pucAttr1 = szAttrBuf[0];
        		stAttr.pucAttr2 = szAttrBuf[1];
        		stAttr.pucAttr3 = NULL;
				rp_client_common(pstElement, STB_RPLIST_RPDATA_OPEN, &stAttr, NULL);

        		/* <rpindex></rpindex> */
        		HxSTD_MemSet(&stData, 0x00, sizeof(STB_DataType_t));
        		stData.uiDecimal = pstRpList->pstRpData[uiIndex].usRpindex;
				rp_client_common(pstElement, STB_RPLIST_RPINDEX, NULL, &stData);

        		/* <rpresult></rpresult> */
        		VK_memset(&stData, 0x00, sizeof(STB_DataType_t));
        		stData.uiDecimal = pstRpList->pstRpData[uiIndex].usRsvResult;
				rp_client_common(pstElement, STB_RPLIST_RESULT, NULL, &stData);

				/* </rpdata> */
				rp_client_common(pstElement, STB_RPLIST_RPDATA_CLOSE, NULL, NULL);
        	}

            /* </rplist> */
			rp_client_common(pstElement, STB_RPLIST_CLOSE, NULL, NULL);
    	}

    }
}


static void rp_client_RsvListProcess(void)
{
	HBOOL					bSeriesRec = FALSE;
	HERROR			 		hErr = ERR_OK;
	STB_ELEMENT_t			*pstElement = NULL;
	HUINT32 				ulRsvCount = 0;
	HUINT32					uinRsv = 0, uiIndex = 0, i;
    HUINT32                 uiEventRsvCount = 0;
	HUINT8					szAttrBuf[3][10];
	STB_ATTR_ELEMENT_t		stAttr;
	STB_DataType_t			stData;

    RPLIST_RSVTYPE_e 	    eRsvType = RP_RSVTYPE_RESERVED;
	RPLIST_REPEAT_e 	    eRepeat = RP_REPEAT_ONCE;
	DxSchedule_t			stDxSched;
	HUINT32					ulIpEpgUid = 0;
	UNIXTIME				utStartTime = 0;
	HCHAR					*szEvtName = NULL;

	/* element load */
	HxLOG_Trace();
	hErr = rp_client_loadElement(STB_ELEMENT_LIST_RSVLIST, &pstElement);
	if( hErr != ERR_OK )
	{
		HxLOG_Debug("[rp_client_RsvListProcess] rp_client_loadElement ERROR \n");
		return ;
	}

    /* 모든 RSV Item의 개수를 얻어 온다. */
	(void)APK_META_SCHEDULE_CountSchedule(&ulRsvCount);
	HxLOG_Debug("[rp_client_RsvListProcess] Cnt = %d\n", ulRsvCount);
	for (i = 0 ; i < ulRsvCount ; i++)
	{
		hErr = APK_META_SCHEDULE_GetScheduleByIndex(i, &stDxSched);
		if (ERR_OK != hErr)					{ continue; }

		HxLOG_Debug("[rp_client_RsvListProcess] rsvType = %d svcUId=%d\n", stDxSched.eRsvType, stDxSched.ulSvcUid);
		switch (stDxSched.eRsvType)
		{
		case DxRSVTYPE_WATCHING_EBR:
		case DxRSVTYPE_WATCHING_TBR:
		case DxRSVTYPE_RECORD_EBR:
		case DxRSVTYPE_RECORD_TBR:
		case DxRSVTYPE_RECORD_SERIES_EBR:
		case DxRSVTYPE_RECORD_SATRECORD_TBR:
			uiEventRsvCount++;
			break;

		default:
			break;
		}
	}

	HxSTD_MemSet(&stAttr, 0x00, sizeof(STB_ATTR_ELEMENT_t));
    snprintf(szAttrBuf[0], 10, "%d", 100);
	snprintf(szAttrBuf[1], 10, "%d", uiEventRsvCount);
	stAttr.pucAttr1 = szAttrBuf[0];
	stAttr.pucAttr2 = szAttrBuf[1];

	/* <rsvlist version="" count="">  */
	rp_client_common(pstElement, STB_RSVLIST_OPEN, &stAttr, NULL);
	for ( uinRsv = 0; uinRsv < ulRsvCount; uinRsv++ )
	{
		HINT32				 nEvtId = -1;
		IPEPG_SvcInfo_t		*svcInfo = NULL;

		eRsvType = 0;
        eRepeat = 0;
		bSeriesRec = FALSE;

		hErr = APK_META_SCHEDULE_GetScheduleByIndex(uinRsv, &stDxSched);
		if (ERR_OK != hErr)					{ continue; }

		HLIB_DATETIME_ConvertDateTimeToUnixTime(&(stDxSched.stStartTime), &utStartTime);

		switch (stDxSched.eRsvType)
		{
		case DxRSVTYPE_WATCHING_EBR:
		case DxRSVTYPE_WATCHING_TBR:
			nEvtId = stDxSched.uExtInfo.stWch.nEvtId;
			eRsvType = RP_RSVTYPE_WATCHING;
			szEvtName = stDxSched.uExtInfo.stWch.szEvtName;
			break;

		case DxRSVTYPE_RECORD_EBR:
			nEvtId = stDxSched.uExtInfo.stRec.nEvtId;
			eRsvType = RP_RSVTYPE_REC_EBR;
			szEvtName = stDxSched.uExtInfo.stRec.szEventName;
			break;

		case DxRSVTYPE_RECORD_SERIES_EBR:
			nEvtId = stDxSched.uExtInfo.stRec.nEvtId;
			eRsvType = RP_RSVTYPE_REC_EBR;
			bSeriesRec = TRUE;
			szEvtName = stDxSched.uExtInfo.stRec.szEventName;
			break;


		case DxRSVTYPE_RECORD_TBR:
		case DxRSVTYPE_RECORD_SATRECORD_TBR:
			nEvtId = stDxSched.uExtInfo.stRec.nEvtId;
			eRsvType = RP_RSVTYPE_REC_TBR;
			bSeriesRec = (stDxSched.uExtInfo.stRec.ulSeriesId > 0);
			szEvtName = stDxSched.uExtInfo.stRec.szEventName;
			break;

		default:
			nEvtId = -1;
			break;
		}

		if (nEvtId < 0)
		{
			continue;
		}

#if !defined(CONFIG_OP_UK_DTT)
		if (eDxEPG_TYPE_IP == stDxSched.eEpgType)
#endif
		{
			ulIpEpgUid = nEvtId;
		}

		svcInfo = IPEPG_GetSvc(stDxSched.ulSvcUid);

		if (svcInfo == NULL)
		{
			continue;
		}

	        HxSTD_MemSet(&stAttr, 0x00, sizeof(STB_ATTR_ELEMENT_t));
	        snprintf(szAttrBuf[0] , 10, "2");
	   		stAttr.pucAttr1 = szAttrBuf[0];

        /* <rsv kind=""> */
		rp_client_common(pstElement, STB_RSVLIST_RSV_OPEN, &stAttr, NULL);

		switch (stDxSched.eRepeat)
		{
		case DxRSVREPEAT_DAILY:
			eRepeat = RP_REPEAT_DAILY;
			break;

		case DxRSVREPEAT_WEEKLY:
			eRepeat = RP_REPEAT_WEEKLY;
			break;

		default:
			eRepeat = (bSeriesRec) ? RP_REPEAT_SERIES : RP_REPEAT_ONCE;
			break;
		}

        for(uiIndex = STB_RSVLIST_CHNUM; uiIndex<STB_RSVLIST_RSV_CLOSE; uiIndex++)
        {
        	HUINT8 *cpName = NULL;

            switch(uiIndex)
            {
                case STB_RSVLIST_CHNUM      : stData.uiDecimal = svcInfo->ulLcn;						break;
                case STB_RSVLIST_SVCHANDLE  : stData.uiDecimal = stDxSched.ulSvcUid;					break;
                case STB_RSVLIST_RSVTYPE    : stData.uiDecimal = eRsvType;								break;
                case STB_RSVLIST_REPEAT     : stData.uiDecimal = eRepeat;								break;
                case STB_RSVLIST_UNIQUEID   : stData.uiDecimal = ulIpEpgUid;							break;
                case STB_RSVLIST_START      : stData.uiDecimal = utStartTime;							break;
                case STB_RSVLIST_END        : stData.uiDecimal = utStartTime + stDxSched.ulDuration;	break;
                case STB_RSVLIST_CHNAME     :
					if(svcInfo->ucName)
					{
						cpName = (HUINT8 *)IPEPG_MemAlloc(DxNAME_LEN);
			        	HxSTD_MemSet(cpName, 0x00, DxNAME_LEN);
						rp_client_SetReplaceString(svcInfo->ucName, cpName, NULL, DxNAME_LEN);
						stData.pString = cpName;
					}
					else
					{
						stData.pString = NULL;
					}
					break;

                case STB_RSVLIST_EVTNAME    :
					if(szEvtName)
					{
						cpName = (HUINT8 *)IPEPG_MemAlloc(DxNAME_LEN);
						HxSTD_MemSet(cpName, 0x00, DxNAME_LEN);
						rp_client_SetReplaceString(szEvtName, cpName, NULL, 256);
						stData.pString = cpName;
					}
					else
					{
						stData.pString = NULL;
					}
                	break;
            }

			rp_client_common(pstElement, uiIndex, NULL, &stData);
			IPEPG_MemFree(cpName);
        }

        /* </rsv> */
		rp_client_common(pstElement, STB_RSVLIST_RSV_CLOSE, NULL, NULL);

		IPEPG_MemFree(svcInfo);
	}

	rp_client_common(pstElement, STB_RSVLIST_CLOSE, NULL, NULL);
}

static HERROR   rp_client_RecListProcess(void)
{
	HERROR                  hErr = ERR_FAIL;
	STB_ELEMENT_t		    *pstElement = NULL;
	HUINT8				    szAttrBuf[3][10];
	HUINT64                 ullRecSize = 0;
	HUINT32					nTryCount = 0;
	STB_ATTR_ELEMENT_t		stAttr;
	STB_DataType_t			stData;
	HUINT32					i = 0;
	HxVector_t				*pstRecVector = NULL;
	HxVector_t				*pstNewRecVector = HLIB_VECTOR_NewEasy(0, (HxFreeFunc_t)DxRECLISTDATA_Delete, NULL);

	if(pstNewRecVector == NULL)
	{
		HxLOG_Warning("[rp_client_RsvListProcess] Fail to HLIB_VECTOR_New()\n");
		return ERR_FAIL;
	}

	// Wait for first scan( max 30 sec )
	while(APK_META_RECORD_IsFirstScanDone() == FALSE)
	{
		HLIB_STD_TaskSleep(200);
		++nTryCount;
		if(nTryCount == 150)	// 200ms * 150 = 30 sec
		{
			HxLOG_Warning("Time Out : Wait for first scan\n");
			return ERR_FAIL;
		}
		HxLOG_Debug("Wait for First Recording Scan[%d]\n", nTryCount);
	}

	// Step 1 : Get all recordings
	pstRecVector = APK_META_RECORD_GetItems();
	if(pstRecVector != NULL)
	{
		// Step 2 : Make a new vector that recording finished
		for(i = 0; i < HLIB_VECTOR_Length(pstRecVector); ++i)
		{
			DxRecListData_t *pstTempRec = NULL;
			DxRecListData_t *clone = NULL;
			pstTempRec = HLIB_VECTOR_ItemAt(pstRecVector, (HINT32)i);
			if(pstTempRec)
			{
				if(pstTempRec->eRecStatus == eDxREC_STS_RECORD_START || pstTempRec->eRecStatus == eDxREC_STS_RECORDING)
				{
					// Recording that don't finish is discard.
					HxLOG_Debug("REC[%s] is not finished\n", pstTempRec->szName);
					continue;
				}

				clone = DxRECLISTDATA_Clone(pstTempRec);
				if (clone)
				{
					(void)HLIB_VECTOR_Add(pstNewRecVector, clone);
				}
			}
		}
		HLIB_VECTOR_Delete(pstRecVector);
	}


	/* element load */
	HxLOG_Trace();
	hErr = rp_client_loadElement(STB_ELEMENT_LIST_RECLIST, &pstElement);
	if(hErr == ERR_OK)
	{
		HUINT32 	uiRecNum=0, uiIndex = 0;

		// Step 3 : Use new vector
		uiRecNum	= HLIB_VECTOR_Length(pstNewRecVector);
		HxLOG_Debug("[rp_client_RecListProcess] Cnt = %d\n", uiRecNum);

		HxSTD_MemSet(&stAttr, 0x00, sizeof(STB_ATTR_ELEMENT_t));
		snprintf(szAttrBuf[0], 10, "%d", 100);
		snprintf(szAttrBuf[1], 10, "%d", uiRecNum);
		stAttr.pucAttr1 = szAttrBuf[0];
		stAttr.pucAttr2 = szAttrBuf[1];

		/* <reclist version="" count=""> */
		rp_client_common(pstElement, STB_RECLIST_OPEN, &stAttr, NULL);

		for (i = 0; i < uiRecNum; i++)
		{
			DxRecListData_t *pstRecData = NULL;

			pstRecData = HLIB_VECTOR_ItemAt(pstNewRecVector, (HINT32)i);
			if (pstRecData == NULL)	continue;

			/* <rec> */
			rp_client_common(pstElement, STB_RECLIST_REC_OPEN, NULL, NULL);

			for(uiIndex = STB_RECLIST_CHNUM ; uiIndex < STB_RECLIST_REC_CLOSE ; uiIndex++)
			{
				HUINT8 * cpName = NULL;
				switch(uiIndex)
				{
					case STB_RECLIST_CHNUM		: stData.uiDecimal	= pstRecData->nSvcNo;		break;
					case STB_RECLIST_START		: stData.uiDecimal	= pstRecData->ulRecStartTime;	break;
					case STB_RECLIST_END		: stData.uiDecimal	= pstRecData->ulRecStartTime + pstRecData->ulRecDuration;		break;
					case STB_RECLIST_REPEAT 	: stData.uiDecimal	= 1; break;

					case STB_RECLIST_CHNAME 	:
						if(pstRecData->szSvcName)
						{
							cpName = (HUINT8 *)IPEPG_MemAlloc(DxRECLIST_NAME_LEN+1);
							HxSTD_MemSet(cpName, 0x00, DxRECLIST_NAME_LEN+1);
							rp_client_SetReplaceString(pstRecData->szSvcName, cpName, NULL, DxRECLIST_NAME_LEN+1);
							stData.pString = cpName;
						}
						else
						{
							stData.pString = NULL;
						}

						break;

					case STB_RECLIST_EVTNAME	:
						if(pstRecData->szName)
						{
							cpName = (HUINT8 *)IPEPG_MemAlloc(DxRECLIST_NAME_LEN+1);
							HxSTD_MemSet(cpName, 0x00, DxRECLIST_NAME_LEN+1);
							rp_client_SetReplaceString(pstRecData->szName, cpName, NULL, DxRECLIST_NAME_LEN+1);
							stData.pString = cpName;
						}
						else
						{
							stData.pString = NULL;
						}

						break;

					case STB_RECLIST_SIZE		:
						{
							char *szRecFile = IPEPG_MemAlloc(1024);
							HxSTD_PrintToStrN(szRecFile, 1024, "%s%s%s"
								, pstRecData->stMeta.szPath
								, pstRecData->stMeta.szFileName
								, EXT_MEDIA_FILE
							);
							HLIB_STORAGE_GetPathSize(szRecFile, &ullRecSize);
							HxLOG_Debug("filesize %lld %u\n", ullRecSize, (HUINT32)(ullRecSize/1024));
							IPEPG_MemFree(szRecFile);
						}
						ullRecSize /= SIZE_1K;
						stData.uiDecimal  = (HUINT32)ullRecSize; break;
					case STB_RECLIST_PLAYED		:
						stData.uiDecimal = pstRecData->bPlayed;
						break;
					case STB_RECLIST_EVTID		:
						stData.uiDecimal = pstRecData->nEventId;
						break;
					default : break;
				}
				rp_client_common(pstElement, uiIndex, NULL, &stData);
				IPEPG_MemFree(cpName);
			}
			/* </rec> */
			rp_client_common(pstElement, STB_RECLIST_REC_CLOSE, NULL, NULL);
		}
		HLIB_VECTOR_Delete(pstNewRecVector);

		/* </reclist> */
		rp_client_common(pstElement, STB_RECLIST_CLOSE, NULL, NULL);
	}

	return ERR_OK;
}


static HERROR   rp_client_AllowedListProcess(void)
{
	HERROR				hResult = ERR_FAIL;
	HERROR 				hErr = ERR_FAIL;
	HUINT32				i;
	STB_ELEMENT_t		*pstElement = NULL;
	HxVector_t			*pstAllowedList = NULL;
	HUINT32				ulListLength;
	STB_DataType_t		stData;

	hErr = rp_client_loadElement(STB_ELEMENT_LIST_ALLOWEDLIST, &pstElement);
	IPEPG_GOTO_IF(hErr != ERR_OK, END_FUNC);

	hErr = rp_client_CreateAllowedList(&pstAllowedList);
	IPEPG_GOTO_IF(hErr != ERR_OK, END_FUNC);
	IPEPG_GOTO_IF(pstAllowedList == NULL, END_FUNC);
	IPEPG_GOTO_IF(HLIB_VECTOR_Length(pstAllowedList) == 0, END_FUNC);

	rp_client_common(pstElement, STB_ALLOWEDLIST_OPEN, NULL, NULL);

	ulListLength = HLIB_VECTOR_Length(pstAllowedList);
	for (i=0; i<ulListLength; i++)
	{
		HUINT8 *pszDeviceId = NULL;

		pszDeviceId = HLIB_VECTOR_ItemAt(pstAllowedList, (HINT32)i);
		if (pszDeviceId == NULL)
		{
			continue;
		}

		stData.pString = pszDeviceId;
		rp_client_common(pstElement, STB_ALLOWEDLIST_DEVICEID, NULL, &stData);
	}

	rp_client_common(pstElement, STB_ALLOWEDLIST_CLOSE, NULL, NULL);

	hResult = ERR_OK;
END_FUNC:
	if (pstAllowedList != NULL)
	{
		HLIB_VECTOR_Delete(pstAllowedList);
	}

	return hResult;
}

static HERROR rp_client_process(RPList_t *pstRpList)
{
	HERROR 				hErr = ERR_FAIL;
	HUINT32				uiIndex;
	STB_ELEMENT_t		*pstElement = NULL;
	HxLOG_Trace();

	/* element load */
	hErr = rp_client_loadElement(STB_ELEMENT_LIST_NOTIFY, &pstElement);
	if(pstElement == NULL)	return ERR_FAIL;

	if(HLIB_FILE_Exist(RP_UPLOAD_FILE))	HLIB_FILE_Delete(RP_UPLOAD_FILE);

	hErr = rp_client_common(pstElement, STB_NOTIFY_OPEN, NULL, NULL);

	for(uiIndex=STB_ELEMENT_LIST_TIMEZONE; uiIndex<STB_ELEMENT_LIST_MAX; uiIndex++)
	{
        switch(uiIndex)
		{
			case STB_ELEMENT_LIST_TIMEZONE :
				rp_client_TimeZoneProcess();
				break;
			case STB_ELEMENT_LIST_HDD :
				rp_client_HardDiskProcess();
				break;
			case STB_ELEMENT_LIST_RPLIST :
				rp_client_RpListProcess(pstRpList);
				break;
			case STB_ELEMENT_LIST_RSVLIST :
				rp_client_RsvListProcess();
				break;
			case STB_ELEMENT_LIST_RECLIST :
				if(rp_client_RecListProcess() != ERR_OK)
				{
					HxLOG_Warning("Fail to rp_client_RecListProcess()\n");
					return ERR_FAIL;
				}
				break;
			case STB_ELEMENT_LIST_ALLOWEDLIST :
				rp_client_AllowedListProcess();
				break;
			default:
				HxLOG_Debug("[rp_client_process] Parm ERROR \n");
		}
	}

	hErr = rp_client_common(pstElement, STB_NOTIFY_CLOSE, NULL, NULL);
	return hErr;

}

static HERROR 	rp_client_common(STB_ELEMENT_t *pstElement, HUINT32 uiTagType, STB_ATTR_ELEMENT_t *pstAttr, STB_DataType_t *pstData)
{
	HINT8	pStrBuf[XML_TAG_MAX_LEN]="";
	HxFILE_t 			pFile = NULL;

	HxSTD_MemSet(pStrBuf, 0, XML_TAG_MAX_LEN);

	switch(pstElement[uiTagType].uiPrintType)
	{
		case PRINT_OPENING:
			snprintf(pStrBuf, XML_TAG_MAX_LEN, "<%s>", pstElement[uiTagType].pucTagName);
			break;
		case PRINT_CLOSING:
			snprintf(pStrBuf, XML_TAG_MAX_LEN, "</%s>", pstElement[uiTagType].pucTagName);
			break;
		case PRINT_DECIMAL:
			snprintf(pStrBuf, XML_TAG_MAX_LEN, "<%s>%d</%s>", pstElement[uiTagType].pucTagName, pstData ? pstData->uiDecimal:0, pstElement[uiTagType].pucTagName);
			break;
		case PRINT_STRING :
			snprintf(pStrBuf, XML_TAG_MAX_LEN, "<%s>%s</%s>", pstElement[uiTagType].pucTagName, pstData ? pstData->pString:NULL, pstElement[uiTagType].pucTagName);
			break;
		case PRINT_ATTR1 :
			snprintf(pStrBuf, XML_TAG_MAX_LEN, "<%s %s=\"%s\">", pstElement[uiTagType].pucTagName, pstElement[uiTagType].pucAttr0Name, pstAttr ? pstAttr->pucAttr1:NULL);
			break;
		case PRINT_ATTR2 :
			snprintf(pStrBuf, XML_TAG_MAX_LEN, "<%s %s=\"%s\" %s=\"%s\">",   pstElement[uiTagType].pucTagName,
																		  pstElement[uiTagType].pucAttr0Name, pstAttr ? pstAttr->pucAttr1:NULL,
																		  pstElement[uiTagType].pucAttr1Name, pstAttr ? pstAttr->pucAttr2:NULL);

			break;
		case PRINT_ATTR3 :
			snprintf(pStrBuf, XML_TAG_MAX_LEN, "<%s %s=\"%s\" %s=\"%s\" %s=\"%s\">", pstElement[uiTagType].pucTagName,
																			pstElement[uiTagType].pucAttr0Name, pstAttr ? pstAttr->pucAttr1:NULL,
																			pstElement[uiTagType].pucAttr1Name, pstAttr ? pstAttr->pucAttr2:NULL,
																			pstElement[uiTagType].pucAttr2Name, pstAttr ? pstAttr->pucAttr3:NULL);
			break;

		case PRINT_ATTR_DECIMAL :
			snprintf(pStrBuf, XML_TAG_MAX_LEN, "<%s %s=%d>", pstElement[uiTagType].pucTagName, pstElement[uiTagType].pucAttr0Name, pstAttr ? pstAttr->ucDecimal:0);
			break;
		default :
			break;

	}

	HxLOG_Debug("rp_client_common :%s \n", pStrBuf);

	pFile = HLIB_FILE_Open(RP_UPLOAD_FILE,"a");
	if(pFile != NULL)	HLIB_FILE_Write(pFile,pStrBuf, 1,HxSTD_StrLen(pStrBuf));
	if(pFile != NULL)	HLIB_FILE_Close(pFile);
	return ERR_OK;
}



static HERROR		rp_client_SetReplaceString(HUINT8 *pucSrcPtr, HUINT8 *pucDesPtr, HUINT32 *pucSaveSize, HUINT32 ulDeslen)
{
	HERROR					hError = ERR_OK;
	HUINT32					ulSrcLen = 0;
	HUINT32					ulDesPos = 0;
	HUINT32					ulSrcPos = 0;
	HUINT32					ulReplaceCharPos = 0;
	HUINT8					pucTempPtr[1024] = {0,};

	if(pucSrcPtr == NULL || pucDesPtr == NULL || ulDeslen == 0)
	{
		HxLOG_Debug("[local_Rp_SetReplaceString] Param NULL!!\n");
		return ERR_FAIL;
	}

	ulSrcLen = HxSTD_StrLen(pucSrcPtr);

	if(ulSrcLen >= ulDeslen)
	{
		HxLOG_Debug("[local_Rp_SetReplaceString] Destnation Str Len is shorter than Source Str Len!!\n");
		return ERR_FAIL;
	}

	for( ulSrcPos = 0, ulDesPos = 0 ; ulSrcPos <  ulSrcLen ; ulSrcPos++)
	{
		HUINT32 ulReplaceCharIdx = 0;
		for(ulReplaceCharIdx = 0 ; ulReplaceCharIdx < 7 ; ulReplaceCharIdx++)
		{
			if(pucSrcPtr[ulSrcPos] == s_astRpReplaceString[ulReplaceCharIdx].ucReplaceCode)
			{
				break;
			}
		}

		if(ulReplaceCharIdx == 7)
		{
			pucTempPtr[ulDesPos++] = pucSrcPtr[ulSrcPos];
			continue;
		}


		for(ulReplaceCharPos = 0; ulReplaceCharPos < s_astRpReplaceString[ulReplaceCharIdx].ucOrgStrLen ; ulReplaceCharPos++)
		{
			pucTempPtr[ulDesPos++] = s_astRpReplaceString[ulReplaceCharIdx].szOrgStr[ulReplaceCharPos];
		}
	}

	ulSrcLen = HxSTD_StrLen(pucTempPtr);

	for( ulSrcPos = 0, ulDesPos = 0 ; ulSrcPos <  ulSrcLen ; ulSrcPos++)
	{
		if(pucTempPtr[ulSrcPos] == 0xee || pucTempPtr[ulSrcPos] == 0x82 || pucTempPtr[ulSrcPos] == 0x86 || pucTempPtr[ulSrcPos] == 0x87)
		{
			continue;
		}

		// 근본원인은 record info의 aucDisplayEvtName 저장시 이미 UTF8 변환중 문제가 생김
		// 여기서는 일단 문제 character는 제거하는 것으로 처리
		if(pucTempPtr[ulSrcPos] < 0x20 || pucTempPtr[ulSrcPos] >= 0xE0)
		{
			continue;
		}

		pucDesPtr[ulDesPos++] = pucTempPtr[ulSrcPos];
	}

	hError = ERR_OK;

	return hError;
}

static HERROR rp_client_loadElement(HUINT32 uiMode, STB_ELEMENT_t **ppstElement)
{
	if(ppstElement == NULL)
	{
		HxLOG_Debug("[rp_client_loadElement] Param NULL !!\n");
		return ERR_FAIL;
	}

	*ppstElement = NULL;

	switch(uiMode)
	{
		case STB_ELEMENT_LIST_NOTIFY:
			*ppstElement = (STB_ELEMENT_t *)s_stElementListNotify.pstElement;
			break;
        case STB_ELEMENT_LIST_TIMEZONE:
			*ppstElement = (STB_ELEMENT_t *)s_stElementListTimezone.pstElement;
			break;
		case STB_ELEMENT_LIST_HDD :
			*ppstElement = (STB_ELEMENT_t *)s_stElementListHdd.pstElement;
			break;
		case STB_ELEMENT_LIST_RPLIST :
			*ppstElement = (STB_ELEMENT_t *)s_stElementListRp.pstElement;
			break;
		case STB_ELEMENT_LIST_RSVLIST :
			*ppstElement = (STB_ELEMENT_t *)s_stElementListRsv.pstElement;
			break;
		case STB_ELEMENT_LIST_RECLIST :
			*ppstElement = (STB_ELEMENT_t *)s_stElementListRec.pstElement;
			break;
		case STB_ELEMENT_LIST_ALLOWEDLIST :
			*ppstElement = (STB_ELEMENT_t *)s_stElementListAllowedList.pstElement;
			break;

		default :
			*ppstElement = NULL;
			return ERR_FAIL;
			HxLOG_Debug("[rp_client_loadElement] invalid mode !!\n");
	}

	return ERR_OK;
}

static HERROR RP_Network_Upload_Process(void)
{
	HERROR						hErr = ERR_FAIL;
    HUINT8						ucRpUploadUrl[RPURL_MAX_LEN] = {0,};
	HFLOAT64					uiUploadSize = 0;
	HxTRANSPORT_Http_t			*pHttp = NULL;
	HxTRANSPORT_Httpheader_t	*pHttpHeader = NULL;
	HUINT8 						*pucMacAddress = NULL;
	HUINT8						*pServerURL = NULL;



    /* Compare Previous Upload Buffer to Present Buffer */
	if(HLIB_FILE_Exist(HUMAX_UPLOADFILE_PATH) == TRUE && HLIB_FILE_Exist(RP_UPLOAD_FILE) == TRUE)
	{
		HUINT32	ulOldFileSize=0, ulNewFileSize=0;
		HUINT8 *pOldData = NULL, * pNewData = NULL;

		HLIB_FILE_ReadFile(HUMAX_UPLOADFILE_PATH, (void **)&pOldData, &ulOldFileSize);
		HLIB_FILE_ReadFile(RP_UPLOAD_FILE, (void **)&pNewData, &ulNewFileSize);

		HxLOG_Debug("OLD[%s][%d]\n", pOldData, ulOldFileSize);
		HxLOG_Debug("NEW[%s][%d]\n", pNewData, ulNewFileSize);

		if(ulOldFileSize != 0 && ulNewFileSize != 0
			&& ulOldFileSize == ulNewFileSize)
		{
			if(HxSTD_MemCmp(pOldData, pNewData, ulOldFileSize) == 0)
			{
				IPEPG_MemFree(pOldData);
				IPEPG_MemFree(pNewData);
				return ERR_OK;
			}
		}

		IPEPG_MemFree(pOldData);
		IPEPG_MemFree(pNewData);
	}



	if(HLIB_FILE_Exist(HUMAX_UPLOADFILE_PATH) == TRUE) HLIB_FILE_Delete(HUMAX_UPLOADFILE_PATH);
	if(HLIB_FILE_Exist(RP_UPLOAD_FILE) == TRUE)
	{
		HLIB_FILE_Copy(RP_UPLOAD_FILE, HUMAX_UPLOADFILE_PATH);
		HLIB_FILE_Delete(RP_UPLOAD_FILE);
	}

	pucMacAddress = IPEPG_GetItemPoint(eIPEPG_ITEM_MAC);
	pServerURL = (HUINT8 *)IPEPG_GetItemPoint(eIPEPG_ITEM_SERVERURL);
	snprintf(ucRpUploadUrl, RPURL_MAX_LEN, "%s/%s%s", pServerURL, HUMAX_SETRPLIST_URL, pucMacAddress);
	HxLOG_Debug("RP SERVER URL [%s]\n", ucRpUploadUrl);
	IPEPG_MemFree(pucMacAddress);
	IPEPG_MemFree(pServerURL);


    HxLOG_Debug("[RP_Network_Upload_Process] Upload Start!  \n");
    pHttp = HLIB_TRANSPORT_HttpOpen();
    if ( pHttp )
    {
#if defined(TVPORTAL_ROOT_CA) && defined(TVPORTAL_CLIENT_CA_KEY) 
		if(!strncmp(ucRpUploadUrl, HTTPS_PREFIX, HTTPS_PREFIX_URL_LENGTH))
		{
			hErr = HLIB_TRANSPORT_HttpSetCACertificateFile(pHttp, TVPORTAL_ROOT_CA);
			if(hErr != ERR_OK)
			{
				HxLOG_Debug("[RP_Network_Upload_Process.1] ##ERROR: Error code = %x\n", HxTransport_GetLastError());
				HLIB_TRANSPORT_HttpClose(pHttp);
				return hErr;
			}
			hErr = HLIB_TRANSPORT_HttpSetCertificateKeyFile(pHttp, TVPORTAL_CLIENT_CA_KEY, eHxTP_HTTP_CERTIFICATE_P12);
			if(hErr != ERR_OK)
			{
				HxLOG_Debug("[RP_Network_Upload_Process.2] ##ERROR: Error code = %x\n", HxTransport_GetLastError());
				HLIB_TRANSPORT_HttpClose(pHttp);
				return hErr;
			}
			hErr = HLIB_TRANSPORT_HttpSetPeerVeryfication(pHttp, TRUE);
			if(hErr != ERR_OK)
			{
				HxLOG_Debug("[RP_Network_Upload_Process.3] ##ERROR: Error code = %x\n", HxTransport_GetLastError());
				HLIB_TRANSPORT_HttpClose(pHttp);
				return hErr;
			}
			hErr = HLIB_TRANSPORT_HttpSetHostVeryfication(pHttp, FALSE);
			if(hErr != ERR_OK)
			{
				HxLOG_Debug("[RP_Network_Upload_Process.4] ##ERROR: Error code = %x\n", HxTransport_GetLastError());
				HLIB_TRANSPORT_HttpClose(pHttp);
				return hErr;
			}
		}
#endif
		pHttpHeader = HLIB_TRANSPORT_HttpAppendHeader(pHttpHeader, "Content-Type: text/xml");
		if ( pHttpHeader )
		{
			hErr = ERR_OK;
			hErr |= HLIB_TRANSPORT_HttpSetHeaderToTP(pHttp, pHttpHeader);
			hErr |= HLIB_TRANSPORT_HttpPut(pHttp, ucRpUploadUrl, HUMAX_UPLOADFILE_PATH, &uiUploadSize);
			hErr |= HLIB_TRANSPORT_HttpFreeHeader(pHttpHeader);
			if(hErr != ERR_OK)
			{
				HxLOG_Debug("[RP_Network_Upload_Process] ##ERROR: Error code = %x\n", HxTransport_GetLastError());
			}

		}
		HLIB_TRANSPORT_HttpClose(pHttp);
    }

	return hErr;
}


/*
Rpserver로 현재 STB의 상태를 Upload 해 준다.
upload data
1. timezone
2. harddisk info
3. Rplist result
4. rsvlist
5. reclist
*/
static HERROR	rp_UploadData(void)
{
	HERROR				ret = ERR_OK;
	RPList_t 			*pstRpList = Rp_GetRpLists();

	HxLOG_Trace();

	/* Make XML */
	ret = rp_client_process(pstRpList);
	if (ret != ERR_OK)
	{
		HxLOG_Warning("rp_client_process failed. \n");
		goto END_FUNC;
	}

	/* Upload */
	ret = RP_Network_Upload_Process();
	if (ret != ERR_OK)
	{
		HxLOG_Warning("RP_Network_Upload_Process failed. \n");
		goto END_FUNC;
	}

	ret = ERR_OK;
END_FUNC:

	IPEPG_MemFree(pstRpList->pstRpData);
	pstRpList->pstRpData = NULL;
	return ret;
}

HERROR	Rp_UploadData(void)
{
	return	rp_UploadData();
}

/*********************** End of File ******************************/

