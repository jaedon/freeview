/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/******************************************************************************/
/**
 * @file	  		nx_core_shahid.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/
#include <nx_core_shahid.h>
#include <nx_core_curlapi.h>
#include <apk_dlna.h>
#include <nx_core_player.h>
/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef struct stShaidReqInfo {
    HINT32  type;
    void *info;
} Nx_ShahidReqInfo;


typedef struct stShaidInfoDbItem {
    void *dataParam[128];
    void *data[128];
    HINT32  length;
} Nx_ShahidInfoDbItem;

typedef struct stShaidInfoDb {
    Nx_ShahidInfoDbItem arrayOfItem[eSHAHID_REQ_MAX];
} Nx_ShahidInfoDb;

typedef struct tagContentsShahidContext {
    HBOOL   bEnable;
    Nx_ShahidInfoDb StShahidDb;
	HxVector_t	*vastDataList;
} Nx_ShahidContext_t;

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_ShahidContext_t		s_stShahidContent;

/**************************************************************************************************/
#define ___________________Private_prototype________________________________________________
/**************************************************************************************************/

static Nx_ShahidContext_t	*nx_shahid_GetContents(void);
static Nx_ShahidInfoDb		*nx_shahid_GetShahidDb(void);
#if defined(CONFIG_OP_SHAHID_APPS)
static HINT32		nx_shahid_searchingDBAvailableItemIndex(Nx_ShahidInfoDb *stShahidInfoDb, ShahidReqType itemType, void* query);
static void*		nx_shahid_getDbItem(Nx_ShahidInfoDb *stShahidInfoDb, ShahidReqType itemType, HINT32 index);
static HERROR		nx_shahid_saveDbItem(Nx_ShahidInfoDb *stShahidInfoDb, ShahidReqType itemType, void* query, void* data);
static void			nx_shahid_parseShowList(HCHAR* strResData,ShahidShowList* stShowList);
static void			nx_shahid_responseFunc(NxCurlApiRes* res);
static HCHAR*		nx_shahid_getComponentId(ShahidReqType eComponentId);
static HERROR		nx_shahid_findXmlChildElement(HxXMLELEMENT hParent, HUINT8 *pucName, HxList_t **ppElementList);
static HERROR		nx_shahid_getXmlRootElement(HxXMLDOC *hXmlDocHandle, HxXMLELEMENT *hRoot, HUINT8 *pszFilePath);
static HERROR		nx_shahid_parseVASTTrackingEvents(HxXMLELEMENT *pElementTrackingEvents, VASTData **ppstVASTData, HxXMLDOC *pXmlDocHandle);
static HERROR		nx_shahid_parseVASTInline(HxXMLELEMENT *pElementInline, VASTData **ppstVASTData, HxXMLDOC *pXmlDocHandle);
static HERROR		nx_shahid_parseVASTWrapper(HxXMLELEMENT *pElementWrapper, VASTData **ppstVASTData, HxXMLDOC *pXmlDocHandle);
static VASTData* 	nx_shahid_parseVASTXml(void);
static void 		nx_shahid_freeVASTData(void* pstVASTData);
static HERROR		nx_shahid_AddVASTCacheData(VASTData **ppstVASTData);
static HERROR		nx_shahid_RemoveAllVASTCacheData(void);
#endif

/**************************************************************************************************/
#define ___________________Private_functions________________________________________________
/**************************************************************************************************/

static Nx_ShahidContext_t	*nx_shahid_GetContents(void)
{
    return &s_stShahidContent;
}

static Nx_ShahidInfoDb	*nx_shahid_GetShahidDb(void)
{
    return &(s_stShahidContent.StShahidDb);
}

#if defined(CONFIG_OP_SHAHID_APPS)

static HINT32 nx_shahid_searchingDBAvailableItemIndex(Nx_ShahidInfoDb *stShahidInfoDb, ShahidReqType itemType, void* query)
{
    Nx_ShahidInfoDbItem *dbIndexItem;
    HINT32  lDbIndex, lDbLength;

    if(NULL == query)
    {
        return ERR_FAIL;
    }

    dbIndexItem = &stShahidInfoDb->arrayOfItem[itemType];
    lDbLength = dbIndexItem->length;

    if(lDbLength == 0)
    {
        return lDbLength;
    }
    else
    {
        for(lDbIndex=0;lDbIndex<lDbLength;lDbIndex++)
        {
            switch(itemType)
            {
                case eSHAHID_REQ_HOME_6: // Array of Show
                {
                    //check duplicated query
                    ShowListInfo  *stDbInfo;
                    ShowListInfo  *stQueryInfo;

                    stDbInfo = dbIndexItem->dataParam[lDbIndex];
                    stQueryInfo =(ShowListInfo*)query;

                    if( (stDbInfo->pageNumber == stQueryInfo->pageNumber)
                        && (stDbInfo->pageSize == stQueryInfo->pageSize))
                        {
                            return lDbIndex;
                        }
                }
                break;
                default:
                    return -1;//Not Supported
            }
        }
        return lDbLength;
    }
}

static void* nx_shahid_getDbItem(Nx_ShahidInfoDb *stShahidInfoDb, ShahidReqType itemType, HINT32 index)
{
    Nx_ShahidInfoDbItem *dbIndexItem;

    dbIndexItem = &(stShahidInfoDb->arrayOfItem[itemType]);

    if(dbIndexItem->length == 0 || index>=dbIndexItem->length)
    {
        return NULL;
    }

    return dbIndexItem->data[index];
}

static HERROR nx_shahid_saveDbItem(Nx_ShahidInfoDb *stShahidInfoDb, ShahidReqType itemType, void* query, void* data)
{
    Nx_ShahidInfoDbItem *dbIndexItem;
    HINT32  lDbIndex, lDbLength;

    if(NULL == query || NULL == data)
    {
        return ERR_FAIL;
    }

    lDbIndex = nx_shahid_searchingDBAvailableItemIndex(stShahidInfoDb,itemType,query);

    dbIndexItem = &stShahidInfoDb->arrayOfItem[itemType];
    lDbLength = dbIndexItem->length;

    if(lDbIndex == -1)
    {
        return ERR_FAIL;
    }

    if(lDbIndex == 0)
    {
        dbIndexItem->data[lDbLength] = data;
        dbIndexItem->dataParam[lDbLength] = query;
        dbIndexItem->length++;
    }
    else
    {
        switch(itemType)
        {
            case eSHAHID_REQ_HOME_6: // Array of Show
            {
                //check duplicated query
                ShahidShowList *stShowList;
                ShowListInfo  *stDbInfo;
                ShowListInfo  *stQueryInfo;

                stDbInfo = dbIndexItem->dataParam[lDbIndex];
                stQueryInfo =(ShowListInfo*)query;

                if(NULL != stDbInfo)
                {
                    //updating Db INFO
                    stShowList = dbIndexItem->data[lDbIndex];
                    if(NULL != stShowList)
                    {
                        NX_APP_Free(stShowList->stArrayOfShahidShow);
                        NX_APP_Free(stShowList);
                        stShowList = NULL;
                    }
                    NX_APP_Free(stDbInfo);
                }
                //Save Info
                dbIndexItem->dataParam[lDbIndex] = query;
                dbIndexItem->data[lDbIndex] = data;
                return ERR_OK;
            }
                break;
            default:
                break;
        }
    }

    return ERR_FAIL;
}

static void nx_shahid_parseShowList(HCHAR* strResData,ShahidShowList* stShowList)
{
	HxJSON_t		jsonRoot, jsonResultArray, jsonResultItem;
	HxJSON_t		jsonTextObj,jsonProductImagesObj,jsonProductImages_TVObj;
    const HCHAR     *JSON_KEY_RESULT_ARRAY="result";
    const HCHAR     *JSON_KEY_TOTALNUMBERS_INT="totalNumbers";
    const HCHAR     *JSON_KEY_PAGESIZE_INT="pageSize";
    const HCHAR     *JSON_KEY_PAGENUMBER_INT="pageNumber";

    const HCHAR     *JSON_KEY_SHOW_TEXT_OBJECT="text";
    const HCHAR     *JSON_KEY_SHOW_TEXT_NAME_STR="name";
    const HCHAR     *JSON_KEY_SHOW_TEXT_DESCRIPTION_STR="description";
    const HCHAR     *JSON_KEY_SHOW_TEXT_SHORTDESCRIPTION_STR="shortDescription";
    const HCHAR     *JSON_KEY_SHOW_ID_INT = "id";
    const HCHAR     *JSON_KEY_SHOW_PRODUCTIMAGES_OBJ = "productImages";
    const HCHAR     *JSON_KEY_SHOW_PRODUCTIMAGES_TV_OBJ = "TV";
    const HCHAR     *JSON_KEY_SHOW_PRODUCTIMAGES_TV_THUMBNAILURL_STR = "thumbnailUrl";

    HINT32          lTotalNumbers = 0;
    HINT32          lPageSize = 0;
    HINT32          lPageNumber = 0;
    HINT32          lResultLength = 0;
    HINT32          lItemIndex = 0;

    ShahidShow      *stShahidShow = NULL;
    ShahidShow      *stArrayOfShahidShow = NULL;
    const HCHAR     *strJsonString = NULL;

    HxLOG_Error("\n[%s.%d] ----------->> START \n",__FUNCTION__,__LINE__);

   	jsonRoot = HLIB_JSON_Decode(strResData);
    lTotalNumbers = HLIB_JSON_Object_GetInteger(jsonRoot,JSON_KEY_TOTALNUMBERS_INT);
    lPageSize = HLIB_JSON_Object_GetInteger(jsonRoot,JSON_KEY_PAGESIZE_INT);
    lPageNumber = HLIB_JSON_Object_GetInteger(jsonRoot,JSON_KEY_PAGENUMBER_INT);
    jsonResultArray = HLIB_JSON_Object_GetArray(jsonRoot, JSON_KEY_RESULT_ARRAY);
    lResultLength = HLIB_JSON_GetSize(jsonResultArray);

    if(lResultLength>0)
    {
        stArrayOfShahidShow =(ShahidShow*)NX_APP_Calloc(sizeof(ShahidShow)*lResultLength);

    	HxJSON_ARRAY_EACH(jsonResultArray, jsonResultItem, {
            stShahidShow = &(stArrayOfShahidShow[lItemIndex]);
            //ID
            stShahidShow->id = HLIB_JSON_Object_GetInteger(jsonResultItem,JSON_KEY_SHOW_ID_INT);
            //TEXT
            jsonTextObj = HLIB_JSON_Object_GetObject(jsonResultItem,JSON_KEY_SHOW_TEXT_OBJECT);
            strJsonString = HLIB_JSON_Object_GetString(jsonTextObj,JSON_KEY_SHOW_TEXT_NAME_STR);
            if(NULL != strJsonString)
            {
                HxSTD_memcpy(stShahidShow->name,strJsonString,SHAHID_STRING_SHORT_LENGTH);
            }
            strJsonString = HLIB_JSON_Object_GetString(jsonTextObj,JSON_KEY_SHOW_TEXT_DESCRIPTION_STR);
            if(NULL != strJsonString)
            {
                HxSTD_memcpy(stShahidShow->description,strJsonString,SHAHID_STRING_LONG_LENGTH);
            }
            strJsonString = HLIB_JSON_Object_GetString(jsonTextObj,JSON_KEY_SHOW_TEXT_SHORTDESCRIPTION_STR);
            if(NULL != strJsonString)
            {
                HxSTD_memcpy(stShahidShow->shortDescription,strJsonString,SHAHID_STRING_LONG_LENGTH);
            }

            //PRODUCT IMAGES
            jsonProductImagesObj = HLIB_JSON_Object_GetObject(jsonResultItem,JSON_KEY_SHOW_PRODUCTIMAGES_OBJ);
            jsonProductImages_TVObj = HLIB_JSON_Object_GetObject(jsonProductImagesObj,JSON_KEY_SHOW_PRODUCTIMAGES_TV_OBJ);

            strJsonString = HLIB_JSON_Object_GetString(jsonProductImages_TVObj,JSON_KEY_SHOW_PRODUCTIMAGES_TV_THUMBNAILURL_STR);

            if(NULL != strJsonString)
            {
                HxSTD_memcpy(stShahidShow->thumbnailUrl,strJsonString,SHAHID_STRING_SHORT_LENGTH);
            }

            lItemIndex++;
    	});

    }

    stShowList->length = lResultLength;
    stShowList->stArrayOfShahidShow = stArrayOfShahidShow;

    HLIB_JSON_Delete(jsonRoot);
}

static void nx_shahid_responseFunc(NxCurlApiRes* res)
{
    Nx_ShahidReqInfo  *stShahidReqInfo = NULL;
    Nx_ShahidInfoDb * stShahidInfoDb = NULL;

    if(NULL == res)
    {
        HxLOG_Print("\n[%s.%d] ----------->> response NoData \n\n\n",__FUNCTION__,__LINE__);
        return ;
    }

    stShahidInfoDb = nx_shahid_GetShahidDb();

    stShahidReqInfo =(Nx_ShahidReqInfo*)res->reqParam;

    if(NULL == stShahidReqInfo)
    {
        HxLOG_Print("\n[%s.%d] ----------->> stShahidReqInfo NoData \n\n\n",__FUNCTION__,__LINE__);
        return ;
    }

    switch(stShahidReqInfo->type)
    {
		case eSHAHID_REQ_HOME_1:
			{
				VASTData *pstVASTData = NULL;
				pstVASTData = nx_shahid_parseVASTXml();
				nx_shahid_AddVASTCacheData(&pstVASTData);
				nx_shahid_RemoveAllVASTCacheData();
				pstVASTData = NULL;
			}
			break;

        case eSHAHID_REQ_HOME_6:
            {
                ShahidShowList* stShowList= NULL;

                HxLOG_Print("\n[%s.%d] ----------->> response CALL \n\n\n",__FUNCTION__,__LINE__);
                HxLOG_Print("\n[%s.%d] Response Code : %d \n\n\n",__FUNCTION__,__LINE__,res->resStatus);

                stShowList = (ShahidShowList*)NX_APP_Calloc(sizeof(ShahidShowList));

                nx_shahid_parseShowList((HCHAR*)res->resData,stShowList);
                nx_shahid_saveDbItem(stShahidInfoDb, eSHAHID_REQ_HOME_6,stShahidReqInfo->info,stShowList);

				ONDK_GWM_PostMessage(NULL, MSG_CORE_CONTENTS_SHAHID_DATA_DB_UPDATED, 0,stShahidReqInfo->type ,(HINT32)stShahidReqInfo->info, 0);
            }
            break;

        default:
            break;
    }

    return;
}

static HCHAR* nx_shahid_getComponentId(ShahidReqType eComponentId)
{
    HCHAR   *strComponentId = NULL;
    strComponentId = NX_APP_Calloc(sizeof(HCHAR)*32);
    if(NULL == strComponentId)
    {
        return strComponentId;
    }

    if(eComponentId < eSHAHID_REQ_HOME_MAX)
    {
        HINT32  uNum = eComponentId - eSHAHID_REQ_HOME_0;
        HxSTD_snprintf(strComponentId, sizeof(HCHAR)*32,"home-%d",uNum);
    }

    return strComponentId;
}

static HERROR	nx_shahid_findXmlChildElement(HxXMLELEMENT hParent, HUINT8 *pucName, HxList_t **ppElementList)
{
	HERROR eRet = ERR_OK;

	eRet = HLIB_XML_FindChildElementByName(hParent, pucName, ppElementList);
	if( (ERR_OK != eRet) || (NULL == ppElementList) )
	{
		HxLOG_Error("error : have no child element \n");
		if( NULL != ppElementList ) HLIB_LIST_RemoveAll(*ppElementList);
		return ERR_FAIL;
	}

	return eRet;
}
static ShahidCastData* nx_shahid_dial_parseReq(HCHAR* szData)
{
	HxJSON_t		jsonRoot;
    const HCHAR     *JSON_KEY_URL_STR="url";
    const HCHAR     *JSON_KEY_DURATION_SECONDS_INT="durationSeconds";
    const HCHAR     *JSON_KEY_MAIN_TITLE_STR="mainTitle";
    const HCHAR     *strJsonString = NULL;
    HUINT32         ulJsonInt = 0;
    HUINT32         ulszDataLength = 0;
    ShahidCastData  *stCastReqData;

    ulszDataLength = HxSTD_StrLen(szData);

    if(ulszDataLength<=0)
    {
        HxLOG_Error("\n[%s.%d] wrong Json type \n\n\n",__FUNCTION__,__LINE__);
        return NULL;
    }

    if(FALSE == HLIB_JSON_IsJSON(szData, ulszDataLength))
    {
        HxLOG_Error("\n[%s.%d] wrong Json type \n\n\n",__FUNCTION__,__LINE__);
        return NULL;
    }

    jsonRoot = HLIB_JSON_Decode(szData);

    if(!jsonRoot)
    {
        HxLOG_Error("\n[%s.%d] wrong Json type \n\n\n",__FUNCTION__,__LINE__);
        return NULL;
    }

    strJsonString = HLIB_JSON_Object_GetString(jsonRoot,JSON_KEY_URL_STR);
    stCastReqData = (ShahidCastData*)NX_APP_Calloc(sizeof(ShahidCastData));

    if(NULL != strJsonString)
    {
        HxSTD_memcpy(stCastReqData->url,strJsonString,SHAHID_STRING_SHORT_LENGTH);
    }

    ulJsonInt = HLIB_JSON_Object_GetInteger(jsonRoot,JSON_KEY_DURATION_SECONDS_INT);
    stCastReqData->durationSeconds = ulJsonInt;

    strJsonString = HLIB_JSON_Object_GetString(jsonRoot,JSON_KEY_MAIN_TITLE_STR);
    if(NULL != strJsonString)
    {
        HxSTD_memcpy(stCastReqData->mainTitle,strJsonString,SHAHID_STRING_LONG_LENGTH);
    }

    HLIB_JSON_Delete(jsonRoot);
    return stCastReqData;
}

static void nx_shahid_dial_playURL(HCHAR* szUrl)
{
#if 1
	NxPlayerContentInform_t * pPlayerContent = NULL;

	pPlayerContent = NX_PLAYER_MakeContentMediaPlayback(0, 0,szUrl);

	NX_PLAYER_SetContent(NX_PLAYER_INST_01, pPlayerContent);
	NX_APP_Free(pPlayerContent);

	NX_PLAYER_Play(NX_PLAYER_INST_01);
#endif

#if 0
	ONDK_GWM_PostMessage(NULL, MSG_CORE_CONTENTS_SHAHID_CAST_PLAY, 0, szUrl, 0, 0);
#endif
}

static void nx_shahid_dial_callbackEvent(APKE_DIAL_Property_e event,  char *app, void *data)
{
   	HERROR	hErr = 0;

	switch ( event )
	{
		case eDLNA_DIAL_ReqAppLaunch:
            {
                ShahidCastData* stData;
    			HxLOG_Error("[OPL:DIAL] eDLNA_DIAL_ReqAppLaunch - %s / %s \n", app, (char *)data);
                APK_DIAL_SetAppState(SHAHID_DIAL_NAME, SHAHID_DIAL_STATE_RUNNING);

                stData = nx_shahid_dial_parseReq(data);
                if(NULL != stData)
                {
                    nx_shahid_dial_playURL(stData->url);
                    NX_APP_Free(stData);
                }
                else
                {
        			HxLOG_Error("[OPL:DIAL] eDLNA_DIAL_ReqAppLaunch - Shahid Launch FAIL \n");
                }
            }
			break;
		case eDLNA_DIAL_ReqAppStop:
			HxLOG_Error("[OPL:DIAL] eDLNA_DIAL_ReqAppStop- %s\n", (char *)app);
			break;
		case eDLNA_DIAL_ReqAppStatus:
			HxLOG_Error("[OPL:DIAL] eDLNA_DIAL_ReqAppStatus- %s\n", (char *)app);
			break;
		case eDLNA_DIAL_Started:
			HxLOG_Error("[OPL:DIAL] eDLNA_DIAL_Started\n");
            hErr = APK_DIAL_AppRegist(SHAHID_DIAL_NAME);
			break;
		case eDLNA_DIAL_Stopped:
			HxLOG_Error("[OPL:DIAL] eDLNA_DIAL_Stopped\n");
			break;
        default:
            break;
    }
}

static void    nx_shahid_dial_apk_listener( HUINT32 p1, HUINT32 p2, HUINT32 p3 )
{
    nx_shahid_dial_callbackEvent(p1,(HCHAR*)p2,(void*)p3);
}

static void nx_shahid_dial_InitCast(void)
{
	HCHAR	szDefaultFriendlyName[128];
	HCHAR	szFriendlyName[128];
	HINT32	nValue = 0;
   	HERROR	hErr = 0;
    HBOOL   bDialActivated = FALSE;

//Dial Test
    HxSTD_MemSet(szDefaultFriendlyName, 0, 128);
    HxSTD_MemSet(szFriendlyName, 0, 128);

    hErr == APK_SYSINFO_STB_GetModelName(szDefaultFriendlyName, 128);
    if (ERR_OK == hErr)
    {
        HxLOG_Error("####### Shahid szDefaultFriendlyName : %s \n\n",szDefaultFriendlyName);
    }


	hErr = APK_DB_GetInt(USERDB_DLNA_DIAL_ACTIVATION, &nValue);
    hErr = APK_DB_GetStr( USERDB_DLNA_DIAL_FRIENDLYNAME, szFriendlyName, 128);
	if ((hErr != ERR_OK) || HxSTD_StrEmpty(szFriendlyName))
	{
		HxSTD_snprintf( szFriendlyName, sizeof(szFriendlyName)-1, "%s", szDefaultFriendlyName);
		HxSTD_StrNCat(szFriendlyName, " DIAL Server", sizeof(szFriendlyName)-1);

		if (APK_DB_SetStr(USERDB_DLNA_DIAL_FRIENDLYNAME, szFriendlyName)  != ERR_OK)
			HxLOG_Error("BPL_INIT: Error> DB_SetStr(DIAL) failed\n");
	}

    APK_DIAL_Init();
    APK_DIAL_RegisterNotifier((APKS_DLNA_CbHandler) nx_shahid_dial_apk_listener);

    APK_DIAL_GetDialStatus(&bDialActivated);
    if(TRUE == bDialActivated)
    {
        hErr = APK_DIAL_AppRegist(SHAHID_DIAL_NAME);
    }
    else
    {
    	hErr = APK_DIAL_Start(szFriendlyName);
        HxLOG_Error("\n[%s.%d]----------->> APK_DIAL_Start : %d \n\n\n",__FUNCTION__,__LINE__,hErr);
    }


	APK_DB_SetInt(USERDB_DLNA_DIAL_ACTIVATION, (HINT32)TRUE);
}

static HERROR	nx_shahid_getXmlRootElement(HxXMLDOC *hXmlDocHandle, HxXMLELEMENT *hRoot, HUINT8 *pszFilePath)
{
	HERROR eRet = ERR_OK;

	eRet = HLIB_XML_InitParser();
	if( ERR_OK != eRet )
	{
		HxLOG_Error("error : xml parser init\n");
		return ERR_FAIL;
	}

	eRet = HLIB_XML_ParseFile(pszFilePath, hXmlDocHandle);
	if( ERR_OK != eRet )
	{
		HxLOG_Error("error : have no file path\n");
		return ERR_FAIL;
	}

	eRet = HLIB_XML_GetRootElement(*hXmlDocHandle, hRoot);
	if( ERR_OK != eRet )
	{
		HxLOG_Error("error : have no root element \n");
		return ERR_FAIL;
	}

	return eRet;
}

static HERROR	nx_shahid_parseVASTTrackingEvents(HxXMLELEMENT *pElementTrackingEvents, VASTData **ppstVASTData, HxXMLDOC *pXmlDocHandle)
{
	HERROR 			eRet = ERR_OK;
	HUINT8			*eventAttr = NULL;
	HxXMLELEMENT	elTracking = NULL;
	HxList_t		*listTracking = NULL;

	/* Get Tracking */
	eRet = nx_shahid_findXmlChildElement(*pElementTrackingEvents, (HUINT8*)"Tracking", &listTracking);
	if (ERR_OK == eRet)
	{
		listTracking = HLIB_LIST_First(listTracking);
		while(listTracking)
		{
			elTracking = (HxXMLELEMENT)HLIB_LIST_Data(listTracking);
			HLIB_XML_GetAttrContents(elTracking, "event", &eventAttr);

			if (HxSTD_StrCmp(eventAttr, "creativeView") == 0)
			{
				eRet = HLIB_XML_GetElementContents(*pXmlDocHandle, elTracking, &((*ppstVASTData)->stTrackingEvents.evtCreativeView));
			}
			else if (HxSTD_StrCmp(eventAttr, "start") == 0)
			{
				eRet = HLIB_XML_GetElementContents(*pXmlDocHandle, elTracking, &((*ppstVASTData)->stTrackingEvents.evtStart));
			}
			else if (HxSTD_StrCmp(eventAttr, "midpoint") == 0)
			{
				eRet = HLIB_XML_GetElementContents(*pXmlDocHandle, elTracking, &((*ppstVASTData)->stTrackingEvents.evtMidpoint));
			}
			else if (HxSTD_StrCmp(eventAttr, "firstQuartile") == 0)
			{
				eRet = HLIB_XML_GetElementContents(*pXmlDocHandle, elTracking, &((*ppstVASTData)->stTrackingEvents.evtFirstQuartile));
			}

			listTracking = HLIB_LIST_Next(listTracking);
		}
	}

	HLIB_LIST_RemoveAll(listTracking);

	return ERR_OK;
}

static HERROR	nx_shahid_parseVASTInline(HxXMLELEMENT *pElementInline, VASTData **ppstVASTData, HxXMLDOC *pXmlDocHandle)
{
	HERROR 			eRet = ERR_OK;
	HxXMLELEMENT	elErrordata = NULL, elImpression = NULL, elCreatives = NULL, elDuration = NULL, elMediaFiles = NULL,
					elMediaFile = NULL, elTrackingevents = NULL, elCreative = NULL, elLinear = NULL;
	HxList_t		*listErrordata = NULL, *listImpression = NULL, *listCreatives = NULL, *listDuration = NULL, *listMediaFiles = NULL,
					*listMediaFile = NULL, *listCreative = NULL, *listLinear = NULL, *listTrackingevents = NULL;

	/* Get Error Data */
	eRet = nx_shahid_findXmlChildElement(*pElementInline, (HUINT8*)"Error", &listErrordata);
	if (ERR_OK == eRet)
	{
		elErrordata = (HxXMLELEMENT)HLIB_LIST_Data(listErrordata);
		HLIB_XML_GetElementContents(*pXmlDocHandle, elErrordata, &((*ppstVASTData)->errorData));
	}

	/* Get Impression */
	eRet = nx_shahid_findXmlChildElement(*pElementInline, (HUINT8*)"Impression", &listImpression);
	if (ERR_OK == eRet)
	{
		elImpression = (HxXMLELEMENT)HLIB_LIST_Data(listImpression);
		HLIB_XML_GetElementContents(*pXmlDocHandle, elImpression, &((*ppstVASTData)->impression));
	}

	/* Get Creatives */
	eRet = nx_shahid_findXmlChildElement(*pElementInline, (HUINT8*)"Creatives", &listCreatives);
	if (ERR_OK == eRet)
	{
		elCreatives = (HxXMLELEMENT)HLIB_LIST_Data(listCreatives);

		/* Get Creative */
		eRet = nx_shahid_findXmlChildElement(elCreatives, "Creative", &listCreative);
		if (ERR_OK == eRet)
		{
			elCreative = (HxXMLELEMENT)HLIB_LIST_Data(listCreative);

			/* Get Linear */
			eRet = nx_shahid_findXmlChildElement(elCreative, (HUINT8*)"Linear", &listLinear);
			if (ERR_OK == eRet)
			{
				elLinear = (HxXMLELEMENT)HLIB_LIST_Data(listLinear);

				/* Get Duration */
				eRet = nx_shahid_findXmlChildElement(elLinear, (HUINT8*)"Duration", &listDuration);
				if (ERR_OK == eRet)
				{
					elDuration = (HxXMLELEMENT)HLIB_LIST_Data(listDuration);
					HLIB_XML_GetElementContents(*pXmlDocHandle, elDuration, &((*ppstVASTData)->duration));
				}

				/* Get MediaFiles */
				eRet = nx_shahid_findXmlChildElement(elLinear, (HUINT8*)"MediaFiles", &listMediaFiles);
				if (ERR_OK == eRet)
				{
					elMediaFiles = (HxXMLELEMENT)HLIB_LIST_Data(listMediaFiles);

					/* Get MediaFile */
					eRet = nx_shahid_findXmlChildElement(elMediaFiles, (HUINT8*)"MediaFile", &listMediaFile);
					if (ERR_OK == eRet)
					{
						elMediaFile = (HxXMLELEMENT)HLIB_LIST_Data(listMediaFile);
						HLIB_XML_GetElementContents(*pXmlDocHandle, elMediaFile, &((*ppstVASTData)->mediaFile));
					}
				}

				/* Get TrackingEvents */
				eRet = nx_shahid_findXmlChildElement(elLinear, (HUINT8*)"TrackingEvents", &listTrackingevents);
				if (ERR_OK == eRet)
				{
					elTrackingevents = (HxXMLELEMENT)HLIB_LIST_Data(listTrackingevents);
					nx_shahid_parseVASTTrackingEvents(&elTrackingevents, ppstVASTData, pXmlDocHandle);
				}
			}
		}
	}

	HLIB_LIST_RemoveAll(listErrordata);
	HLIB_LIST_RemoveAll(listImpression);
	HLIB_LIST_RemoveAll(listCreatives);
	HLIB_LIST_RemoveAll(listDuration);
	HLIB_LIST_RemoveAll(listMediaFiles);
	HLIB_LIST_RemoveAll(listMediaFile);
	HLIB_LIST_RemoveAll(listCreative);
	HLIB_LIST_RemoveAll(listLinear);
	HLIB_LIST_RemoveAll(listTrackingevents);

	return ERR_OK;
}

static HERROR	nx_shahid_parseVASTWrapper(HxXMLELEMENT *pElementWrapper, VASTData **ppstVASTData, HxXMLDOC *pXmlDocHandle)
{
	HERROR 			eRet = ERR_OK;
	HxXMLELEMENT 	elErrordata = NULL, elImpression = NULL, elAdTagURI = NULL, elCreatives = NULL, elTrackingevents = NULL, elCreative = NULL, elLinear = NULL;
	HxList_t 		*listErrordata = NULL, *listImpression = NULL, *listAdTagURI = NULL, *listCreatives = NULL, *listCreative = NULL, *listLinear = NULL, *listTrackingevents = NULL;

	/* Get Error Data */
	eRet = nx_shahid_findXmlChildElement(*pElementWrapper, (HUINT8*)"Error", &listErrordata);
	if (ERR_OK == eRet)
	{
		elErrordata = (HxXMLELEMENT)HLIB_LIST_Data(listErrordata);
		HLIB_XML_GetElementContents(*pXmlDocHandle, elErrordata, &((*ppstVASTData)->errorData));
	}

	/* Get Impression */
	eRet = nx_shahid_findXmlChildElement(*pElementWrapper, (HUINT8*)"Impression", &listImpression);
	if (ERR_OK == eRet)
	{
		elImpression = (HxXMLELEMENT)HLIB_LIST_Data(listImpression);
		HLIB_XML_GetElementContents(*pXmlDocHandle, elImpression, &((*ppstVASTData)->impression));
	}

	/* Get VASTAdTagURI */
	eRet = nx_shahid_findXmlChildElement(*pElementWrapper, (HUINT8*)"VASTAdTagURI", &listAdTagURI);
	if (ERR_OK == eRet)
	{
		elAdTagURI = (HxXMLELEMENT)HLIB_LIST_Data(listAdTagURI);
		HLIB_XML_GetElementContents(*pXmlDocHandle, elAdTagURI, &((*ppstVASTData)->adTagURI));
	}

	/* Get Creatives */
	eRet = nx_shahid_findXmlChildElement(*pElementWrapper, (HUINT8*)"Creatives", &listCreatives);
	if (ERR_OK == eRet)
	{
		elCreatives = (HxXMLELEMENT)HLIB_LIST_Data(listCreatives);

		/* Get Creative */
		eRet = nx_shahid_findXmlChildElement(elCreatives, "Creative", &listCreative);
		if (ERR_OK == eRet)
		{
			elCreative = (HxXMLELEMENT)HLIB_LIST_Data(listCreative);

			/* Get Linear */
			eRet = nx_shahid_findXmlChildElement(elCreative, (HUINT8*)"Linear", &listLinear);
			if (ERR_OK == eRet)
			{
				elLinear = (HxXMLELEMENT)HLIB_LIST_Data(listLinear);

				/* Get TrackingEvents */
				eRet = nx_shahid_findXmlChildElement(elLinear, (HUINT8*)"TrackingEvents", &listTrackingevents);
				if (ERR_OK == eRet)
				{
					elTrackingevents = (HxXMLELEMENT)HLIB_LIST_Data(listTrackingevents);
					nx_shahid_parseVASTTrackingEvents(&elTrackingevents, ppstVASTData, pXmlDocHandle);
				}
			}
		}
	}

	HLIB_LIST_RemoveAll(listErrordata);
	HLIB_LIST_RemoveAll(listImpression);
	HLIB_LIST_RemoveAll(listAdTagURI);
	HLIB_LIST_RemoveAll(listCreatives);
	HLIB_LIST_RemoveAll(listCreative);
	HLIB_LIST_RemoveAll(listLinear);
	HLIB_LIST_RemoveAll(listTrackingevents);

	return ERR_OK;
}

static VASTData* nx_shahid_parseVASTXml(void)
{
	HERROR 			eRet = ERR_OK;
	HxXMLDOC 		hXmlDocHandle = NULL;
	VASTData 		*pstVASTData = NULL;
	HxXMLELEMENT 	hRoot = NULL, elAd = NULL, elInline = NULL, elWrapper = NULL;
	HxList_t 		*listAd = NULL, *listInline = NULL, *listWrapper = NULL;
	HUINT8     		*XML_VAST_FILE_PATH = "/vast.xml";

	eRet = nx_shahid_getXmlRootElement(&hXmlDocHandle, &hRoot, XML_VAST_FILE_PATH);
	if (ERR_OK == eRet)
	{
		pstVASTData = (VASTData*)NX_APP_Calloc(sizeof(VASTData));
	}
	else
	{
		return NULL;
	}

	/* Get Ad */
	eRet = nx_shahid_findXmlChildElement(hRoot, (HUINT8*)"Ad", &listAd);
	if (ERR_OK == eRet)
	{
		elAd = (HxXMLELEMENT)HLIB_LIST_Data(listAd);
		HLIB_XML_GetAttrContents(elAd, "id", &pstVASTData->adId);
	}
	else
	{
		HLIB_XML_FreeDocument(hXmlDocHandle);
		return pstVASTData;
	}

	/* Get InLine */
	eRet = nx_shahid_findXmlChildElement(elAd, (HUINT8*)"InLine", &listInline);
	if (ERR_OK == eRet)
	{
		elInline = (HxXMLELEMENT)HLIB_LIST_Data(listInline);
		nx_shahid_parseVASTInline(&elInline, &pstVASTData, &hXmlDocHandle);
	}
	else
	{
		/* Get Wrapper */
		eRet = nx_shahid_findXmlChildElement(elAd, (HUINT8*)"Wrapper", &listWrapper);
		if (ERR_OK == eRet)
		{
			elWrapper = (HxXMLELEMENT)HLIB_LIST_Data(listWrapper);
			nx_shahid_parseVASTWrapper(&elWrapper, &pstVASTData, &hXmlDocHandle);
		}
		else
		{
			HLIB_LIST_RemoveAll(listAd);
			HLIB_XML_FreeDocument(hXmlDocHandle);
			return pstVASTData;
		}
	}

	HLIB_LIST_RemoveAll(listAd);
	if (listInline != NULL)
		HLIB_LIST_RemoveAll(listInline);
	if (listWrapper != NULL)
		HLIB_LIST_RemoveAll(listWrapper);
	HLIB_XML_FreeDocument(hXmlDocHandle);

	return pstVASTData;
}

static void nx_shahid_freeVASTData(void* pVASTData)
{
	VASTData *pstVASTData = (VASTData*)pVASTData;

	if (pstVASTData != NULL)
	{
		if (pstVASTData->adId != NULL)
			NX_APP_Free(pstVASTData->adId);

		if (pstVASTData->errorData != NULL)
			NX_APP_Free(pstVASTData->errorData);

		if (pstVASTData->impression != NULL)
			NX_APP_Free(pstVASTData->impression);

		if (pstVASTData->duration != NULL)
			NX_APP_Free(pstVASTData->duration);

		if (pstVASTData->mediaFile != NULL)
			NX_APP_Free(pstVASTData->mediaFile);

		if (pstVASTData->adTagURI != NULL)
			NX_APP_Free(pstVASTData->adTagURI);

		if (pstVASTData->stTrackingEvents.evtCreativeView != NULL)
			NX_APP_Free(pstVASTData->stTrackingEvents.evtCreativeView);

		if (pstVASTData->stTrackingEvents.evtStart != NULL)
			NX_APP_Free(pstVASTData->stTrackingEvents.evtStart);

		if (pstVASTData->stTrackingEvents.evtMidpoint != NULL)
			NX_APP_Free(pstVASTData->stTrackingEvents.evtMidpoint);

		if (pstVASTData->stTrackingEvents.evtFirstQuartile != NULL)
			NX_APP_Free(pstVASTData->stTrackingEvents.evtFirstQuartile);

		NX_APP_Free(pstVASTData);
	}
}

static HERROR	nx_shahid_AddVASTCacheData(VASTData **ppstVASTData)
{
	Nx_ShahidContext_t	*stShahidContext;

    stShahidContext = nx_shahid_GetContents();

	if(stShahidContext == NULL)
	{
		HxLOG_Debug("stShahidContext is NULL!\n");
		return ERR_FAIL;
	}

	HLIB_VECTOR_Add(stShahidContext->vastDataList, (void*)*ppstVASTData);
	return ERR_OK;
}

static HERROR	nx_shahid_RemoveAllVASTCacheData(void)
{
	Nx_ShahidContext_t	*stShahidContext;
	HINT32				lListLen = 0;

	stShahidContext = nx_shahid_GetContents();
	lListLen = HLIB_VECTOR_Length(stShahidContext->vastDataList);
	if (lListLen > 0)
	{
		HLIB_VECTOR_RemoveAll(stShahidContext->vastDataList);
	}
	return ERR_OK;
}

#endif
/**************************************************************************************************/
#define ___________________Public_functions________________________________________________
/**************************************************************************************************/


#if 0
HERROR NX_SHAHID_DoTest(void)
{
    NxCurlApiReq *stReq = NULL;

    HxLOG_Print("\n[%s.%d] ----------->> START \n\n\n",__FUNCTION__,__LINE__);
//	HxSTD_MemSet(&stLastChannel, 0, sizeof(NX_Channel_t));
    stReq = NX_APP_Calloc(sizeof(NxCurlApiReq));

    stReq->eReqMethod = eCurlReqMethodPOST;
	HxSTD_MemCopy(stReq->strUrl, "http://api.shahid.net/api/apiAccess", NX_CORE_CURL_URL_LENGTH_MAX);
    HxSTD_MemCopy(stReq->strPostParam, "{\"apiKey\":\"sh@hid0nlin3\"}", NX_CORE_CURL_URL_POST_PARAM_LENGTH_MAX);

    NX_CORE_CURLAPI_Req(stReq);
    HxLOG_Print("\n[%s.%d] ----------->> END \n\n\n",__FUNCTION__,__LINE__);
}

HERROR NX_SHAHID_DoTest2(void)
{
    NxCurlApiReq *stReq = NULL;
    Nx_ShahidContext_t *stShahidContext;

    HxLOG_Print("\n[%s.%d] ----------->> START \n\n\n",__FUNCTION__,__LINE__);
//	HxSTD_MemSet(&stLastChannel, 0, sizeof(NX_Channel_t));
    stReq = NX_APP_Calloc(sizeof(NxCurlApiReq));
    stReq->eReqMethod = eCurlReqMethodGET;

    stReq->pfnResCallback = nx_shahid_responseFunc;
    stReq->reqParam = (void*)&s_stShahidContent;

//	HxSTD_MemCopy(stReq->strUrl,"http://52.6.255.141:8080/api/v3/component/?component={\"componentId\":\"home-1\"}", NX_CORE_CURL_URL_LENGTH_MAX);
    HxSTD_MemCopy(stReq->strUrl,"http://52.6.255.141:8080/api/v3/component/?component={\"componentId\":\"home-6\",\"pageNumber\":0,\"pageSize\":2}",NX_CORE_CURL_URL_LENGTH_MAX);

    stShahidContext = (Nx_ShahidContext_t*)stReq->reqParam;
    HxLOG_Print("\n[%s.%d] ----------->> stShahidContext %d \n\n\n",__FUNCTION__,__LINE__,stShahidContext->bEnable);

    NX_CORE_CURLAPI_Req(stReq);
    HxLOG_Print("\n[%s.%d] ----------->> END \n\n\n",__FUNCTION__,__LINE__);
}
#endif

//Initialize for Shihid API
HERROR	NX_SHAHID_Init(void)
{
    Nx_ShahidContext_t	*stShahidContext;

    stShahidContext = nx_shahid_GetContents();
#if defined(CONFIG_OP_SHAHID_APPS)
    stShahidContext->bEnable = TRUE;
    HxSTD_MemSet(stShahidContext,0,sizeof(Nx_ShahidContext_t));
	stShahidContext->vastDataList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))nx_shahid_freeVASTData, NULL);
    nx_shahid_dial_InitCast();

#else
    stShahidContext->bEnable = FALSE;
	return ERR_FAIL;
#endif

	return ERR_OK;
}

HERROR   NX_SHAHID_ReqShowList(ShowListInfo  *stShowInfo)
{
#if defined(CONFIG_OP_SHAHID_APPS)
    NxCurlApiReq *stReq = NULL;
    HCHAR   *strComponentId = NULL;
    Nx_ShahidReqInfo  *stShahidReqInfo = NULL;

    HxLOG_Print("\n[%s.%d] ----------->> START \n\n\n",__FUNCTION__,__LINE__);

    strComponentId = nx_shahid_getComponentId(eSHAHID_REQ_HOME_6);

    if(NULL == strComponentId )  return ERR_FAIL;

    stReq = NX_APP_Calloc(sizeof(NxCurlApiReq));

    if(NULL == stReq){
        NX_APP_Free(strComponentId);
        return ERR_FAIL;
    }

    stShahidReqInfo = NX_APP_Calloc(sizeof(Nx_ShahidReqInfo));

    stReq->eReqMethod = eCurlReqMethodGET;
    stReq->pfnResCallback = nx_shahid_responseFunc;

    stShahidReqInfo->info = NX_APP_MemDup(stShowInfo, sizeof(ShowListInfo));
    stShahidReqInfo->type = eSHAHID_REQ_HOME_6;

    HxSTD_snprintf(stReq->strUrl, sizeof(HCHAR)*NX_CORE_CURL_URL_LENGTH_MAX,
        "http://52.6.255.141:8080/api/v3/component/?component={\"componentId\":\"%s\",\"pageNumber\":%d,\"pageSize\":%d}",strComponentId,stShowInfo->pageNumber,stShowInfo->pageSize);


    stReq->reqParam = (void*)stShahidReqInfo;
    NX_CORE_CURLAPI_Req(stReq);

    NX_APP_Free(strComponentId);

    if(stReq != NULL)
    {
        NX_APP_Free(stReq);
    }

    HxLOG_Print("\n[%s.%d] ----------->> END \n\n\n",__FUNCTION__,__LINE__);
    return ERR_OK;
#else
    return ERR_FAIL;
#endif
}

ShahidShowList*   NX_SHAHID_GetShowList(ShowListInfo  *stShowListInfo)
{
#if defined(CONFIG_OP_SHAHID_APPS)
    Nx_ShahidInfoDb * stShahidInfoDb = NULL;
    HINT32  index = 0;
    ShahidShowList* stShowList;

    if(NULL == stShowListInfo)
    {
        return NULL;
    }

    stShahidInfoDb = nx_shahid_GetShahidDb();
    index = nx_shahid_searchingDBAvailableItemIndex(stShahidInfoDb, eSHAHID_REQ_HOME_6, stShowListInfo);
    stShowList = (ShahidShowList*)nx_shahid_getDbItem(stShahidInfoDb,eSHAHID_REQ_HOME_6,index);
    return stShowList;
#else
    return NULL;
#endif
}
