/**
	@file     	xmgr_param_ir4000hd.c
	@brief

	Description:  			\n
	Module: 					 					\n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <hapi.h>
#include <db_svc.h>
#include <db_param.h>

#include <isosvc.h>

#include <bus.h>
#include <mgr_common.h>
#include <mgr_param.h>
#include <pmgr_param.h>
#include "../local_include/_xmgr_param.h"

#include <otl.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	eXmgrParam_UpdatedListType_Menu,
	eXmgrParam_UpdatedListType_Surfer
} XmgrParam_UpdatedListType_e;

typedef enum
{
	eXmgrParam_UpdatedListimgType_New,
	eXmgrParam_UpdatedListimgType_Default,
	eXmgrParam_UpdatedListimgType_Noimg
} XmgrParam_UpdatedListimgType_e;

#define	UPDATED_LIST_MAX_STRING_LENGTH_IN_A_LINE		128
#define	UPDATED_LIST_MAX_LINE							5
#define	MAX_UPDATED_LIST_LEN							1024
#define TOTAL_PAGE_NUM									3

/* OTA version */
#define	OTA_NOTIFICATION_VERSION_FOR_CHECK				0

typedef struct
{
	HUINT32 						ulIndex;
	HCHAR							szString[UPDATED_LIST_MAX_STRING_LENGTH_IN_A_LINE];
} UpdatedList_DataItem_t;

typedef struct
{
	HUINT32 							ulPageNum;
	XmgrParam_UpdatedListimgType_e		eImage;
	HUINT32 							ulLineNum;
	UpdatedList_DataItem_t				stListItem[UPDATED_LIST_MAX_LINE];
} UpdatedList_t;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

static UpdatedList_t		stUpdatedList_ir4000hd_v1[TOTAL_PAGE_NUM] =
{
	{
		0, eXmgrParam_UpdatedListimgType_Default, 1,
		{
			{0, "1. The important function added."},
		}
	},
	{
		1, eXmgrParam_UpdatedListimgType_Noimg, 1,
		{
			{0, "2. IP EPG function added."},
		}
	},
	{
		2, eXmgrParam_UpdatedListimgType_Noimg, 3,
		{
			{0, "3. Updated security problem fix."}
			//{1, "4. Changed loader version."},
			//{2, "5. Updated SW version."}
		}
	}
};


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

/*******************************************************************
{
	"uitype":"updatedlist",
	"uidata":{
		"otaversion": 0,
		"listitem":[
			"pageitem":[
				"pagenum":%d,
				"type":["defaultimage" or "newimage" or "noimage"],
				{
					"index":0,
					"stringlength":%d,
					"string":["serial number"]
				},
				{
					"index":1,
					"string":"base64 encoded data"
				}
			],
			"pageitem":[
				"pagenum":%d,
				"type":["defaultimage" or "newimage" or "noimage"],
				{
					"index":0,
					"stringlength":%d,
					"string":["serial number"]
				},
				{
					"index":1,
					"string":"base64 encoded data"
				}
			]
		]
	}
}

//{"uitype":"updatedlist", "uidata":{"otaversion":1,"listitem":[[{"pagenum":1, "type":["defaultimage"]},{"index":0,"stringlength":2,"string":["serial number1"]}],[{"pagenum":2, "type":["defaultimage"]},{"index":0, "stringlength":2,"string":["serial number2"]}],[{"pagenum":3, "type":["defaultimage"]},{"index":0, "stringlength":2,"string":["serial number3_1"]},{"index":1, "stringlength":2,"string":["serial number3_2"]},{"index":2, "stringlength":2,"string":["serial number3_3"]}]]}}

*******************************************************************/


STATIC HCHAR *xmgr_param_EncodeUpdatedListData(HUINT32 ulOtaVersion, UpdatedList_t *pstUpdatedListEvt, HUINT32 ulTotalPage)
{
	HUINT32 					ulTotalItem = 0, ulItemLoop = 0, ulPageLoop = 0;
	HCHAR						*pzProperties = NULL;
	//HCHAR						*pzProperties2 = NULL;
	HINT32						slStream = HLIB_RWSTREAM_OpenEx(NULL, MAX_UPDATED_LIST_LEN, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	//HINT32						slStream2 = HLIB_RWSTREAM_OpenEx(NULL, MAX_UPDATED_LIST_LEN, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);

	UpdatedList_DataItem_t		*pstListItem = NULL;
	//HUINT32					ulStreamSize = 0, ulNeed = 0;
	//HCHAR 					*endcodedBuffer = NULL;


	HLIB_RWSTREAM_Print(slStream, "{\"uitype\":\"updatedlist\"");
	HLIB_RWSTREAM_Print(slStream, ",\"uidata\":{");
	HLIB_RWSTREAM_Print(slStream, "\"otaversion\":%d,", ulOtaVersion);

	ulPageLoop = 0;

	if(ulTotalPage != 0)
	{
		HxLOG_Info(" ulTotalItem %d\n", ulTotalItem);
		HLIB_RWSTREAM_Print(slStream, "\"listitem\":[");

		do
		{
			//HLIB_RWSTREAM_Print(slStream, "\"pageitem\":[");
			HLIB_RWSTREAM_Print(slStream, "[{\"pagenum\":%d,", pstUpdatedListEvt[ulPageLoop].ulPageNum);
			HLIB_RWSTREAM_Print(slStream, "\"type\":[");
			if(pstUpdatedListEvt[ulPageLoop].eImage == eXmgrParam_UpdatedListimgType_New)
			{
				HLIB_RWSTREAM_Print(slStream, "\"%s\"", "newimage");
			}
			else if(pstUpdatedListEvt[ulPageLoop].eImage == eXmgrParam_UpdatedListimgType_Default)
			{
				HLIB_RWSTREAM_Print(slStream, "\"%s\"", "defaultimage");
			}
			else
			{
				HLIB_RWSTREAM_Print(slStream, "\"%s\"", "noimage");
			}
			HLIB_RWSTREAM_Print(slStream, "]},");

			ulTotalItem = pstUpdatedListEvt[ulPageLoop].ulLineNum;
			ulItemLoop = 0;
			do
			{
				/* UpdatedList_DataItem_t */
				HxLOG_Info("ulItemLoop %d\n", ulItemLoop);

				pstListItem = &(pstUpdatedListEvt[ulPageLoop].stListItem[ulItemLoop]);

				HLIB_RWSTREAM_Print(slStream, "{\"index\":%d", pstListItem->ulIndex);
				HLIB_RWSTREAM_Print(slStream, ",\"stringlength\":%d,\"string\": [\"%s\"]", HxSTD_StrLen(pstListItem->szString), pstListItem->szString);

				ulItemLoop = ulItemLoop + 1;
				if(ulItemLoop == ulTotalItem)
				{
					HLIB_RWSTREAM_Print(slStream, "}");
					break;
				}
				HLIB_RWSTREAM_Print(slStream, "},");
			}while(1);

			ulPageLoop = ulPageLoop + 1;
			if(ulPageLoop == ulTotalPage)
			{
				HLIB_RWSTREAM_Print(slStream, "]");
				break;
			}
			HLIB_RWSTREAM_Print(slStream, "],");
		}while(1);
	}
	HLIB_RWSTREAM_Print(slStream, "]}}");

	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);


#if 1
	return pzProperties;

#else // for encoding
	if(pzProperties != NULL)
	{
		if(pzProperties != NULL)
		{
			ulStreamSize = HxSTD_StrLen(pzProperties);
			ulNeed = HLIB_MATH_EncodeBase64(NULL, (const void *)pzProperties, ulStreamSize);
			endcodedBuffer = HLIB_STD_MemCalloc(ulNeed+1);
			if(endcodedBuffer != NULL)
			{
				HLIB_MATH_EncodeBase64(endcodedBuffer, (const void *)pzProperties, ulStreamSize);
			}
			HLIB_STD_MemFree(pzProperties);
		}
		HLIB_RWSTREAM_Print(slStream, endcodedBuffer);
		HLIB_STD_MemFree(endcodedBuffer);
	}

	pzProperties2 = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream2);

	return pzProperties2;
#endif
}

HERROR xmgr_param_GetUpdatedList_IR4000HD(HUINT8 *pzUpdatedList)
{
	HCHAR	*pzProperty = NULL;

	// JSON 형태로 encode. (HLIB_MATH_EncodeBase64도 사용함. 즉, oapi 단에서 HLIB_MATH_DecodeBase64 하여야 함.)
#if 0
	pzProperty = xmgr_param_EncodeUpdatedListData(&stUpdatedListEvt);
#else
	pzProperty = xmgr_param_EncodeUpdatedListData(OTA_NOTIFICATION_VERSION_FOR_CHECK, stUpdatedList_ir4000hd_v1, TOTAL_PAGE_NUM);
#endif

	if(pzUpdatedList == NULL)
		return ERR_FAIL;

	HLIB_STD_StrUtf8NCpy(pzUpdatedList, pzProperty, MAX_UPDATED_LIST_LEN);

	if(pzProperty)
	{
		HLIB_STD_MemFree(pzProperty);
	}
	return ERR_OK;
}


/*********************** End of File ******************************/
