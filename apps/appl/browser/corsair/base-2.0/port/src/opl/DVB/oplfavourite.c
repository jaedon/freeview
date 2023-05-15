/**************************************************************************************/
/**
 * @file oplfavourite.c
 *
 * Defines the channel structure.
 *
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include "oplfavourite.h"

#include "oapi.h"
#include "apk.h"
#include <hlib.h>
#include <clib.h>




//#define	_DLOG					printf
#define	_DLOG					HxLOG_Debug
#define	ASSERT(expr)			HxLOG_Assert(expr)
#define	TRACE()					do{_DLOG("OPLFAV::TRACE [%s:%d]\n", __FUNCTION__, __LINE__);}while(0)
#define	RETURN_IF_VOID(expr)	do{if(expr){_DLOG("OPLFAV::[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return;}}while(0)
#define	RETURN_IF(expr, err)	do{if(expr){_DLOG("OPLFAV::[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return (err);}}while(0)
#define	GOTO_IF(expr, pos)		do{if(expr){_DLOG("OPLFAV::[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);goto pos;}}while(0)


#define	FAV_STRBUF_SIZE	(4096 * 2)

#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
//ALL, TV, Radio, HDTV, FreeTV, PayTV
#define CHANNEL_GROUP_COUNT 6
//Favourite1 ~ 5
#define USER_FAVOURITE_COUNT 5
#elif defined(CONFIG_OP_MIDDLE_EAST)
//ALL, TV, Radio, HDTV, FreeTV, PayTV
#define	CHANNEL_GROUP_COUNT			6
//Favourite1 ~ 5
#define	USER_FAVOURITE_COUNT		5
#else
//default
//ALL, TV, Radio, HDTV, FreeTV, PayTV
#define CHANNEL_GROUP_COUNT 6
//Favourite1 ~ 5
#define USER_FAVOURITE_COUNT 5
#endif

typedef struct
{
    HINT32		id;
    HBOOL		ready;
    HCHAR		*idStr;
    HCHAR		*name;
    HCHAR		*extension;
    HxVector_t	*list;	// channel uid list
} Favourite_t;

typedef struct
{
    HINT32		nextId;
    HINT32		mainId;
    HxVector_t	*favList;
} FavouriteManager_t;

HxVector_t *favKindList = NULL;
HxVector_t *allList = NULL;
HxVector_t *tvList = NULL;
HxVector_t *radioList = NULL;
HxVector_t *hdList = NULL;
HxVector_t *freeList = NULL;
HxVector_t *payList = NULL;
HxVector_t *favList[5];

static const HCHAR *
    opl_favourite_GetChannelGroupName (HUINT32 i)
{
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE) || defined(CONFIG_PROD_HMS1000T) || defined(CONFIG_PROD_DIGIMAXT2)
    static HCHAR	*favListName[] = {"ALL", "TV", "RADIO", "HDTV", "FREE", "PAY"};

    if (sizeof(favListName) / sizeof(favListName[0]) > i)
    {
        return favListName[i];
    }
#endif
    return NULL;
}

static const HCHAR *
    opl_favourite_GetUserFavouriteName (HUINT32 i)
{
    static char buffer[128];

    if (i < USER_FAVOURITE_COUNT)
    {
        HxSTD_PrintToStrN(buffer, 128, "Favourite%d", i + 1);
        return buffer;
    }
    return NULL;
}


static const HCHAR *
    opl_favourite_ToIDString (HINT32 id)
{
    static HCHAR s_buf[64];

    HxSTD_PrintToStrN(s_buf, 64, "fav://%X", (int)id);
    return s_buf;
}

static HINT32
    opl_favourite_ToID (const HCHAR *idStr)
{
    RETURN_IF( !HLIB_STD_StrStartWith(idStr, "fav://"), 0 );
    idStr += 6;	// skip 'fav://'

    return HLIB_STD_HexToInt(idStr);
}

static void
    opl_favourite_Delete (Favourite_t *fav)
{
    if (fav)
    {
        if (fav->idStr)
            HLIB_STD_MemFree(fav->idStr);
        if (fav->name)
            HLIB_STD_MemFree(fav->name);
        if (fav->extension)
            HLIB_STD_MemFree(fav->extension);
        if (fav->list)
            HLIB_VECTOR_Delete(fav->list);
        HLIB_STD_MemFree(fav);
    }
}

static Favourite_t *
    opl_favourite_New (HINT32 id, const HCHAR *name, const HCHAR *extension)
{
    Favourite_t	*fav;

    if (id == 0)
        return NULL;

    fav = HLIB_STD_MemAlloc(sizeof(Favourite_t));
    if(fav != NULL) {
        HxSTD_MemSet(fav, 0, sizeof(Favourite_t));
    }

    if (fav)
    {
        fav->id = id;
        fav->idStr = HLIB_STD_StrDup(opl_favourite_ToIDString(id));
        if (name)
            fav->name = HLIB_STD_StrDup(name);

        if (extension)
            fav->extension = HLIB_STD_StrDup(extension);

        fav->list = HLIB_VECTOR_New(NULL, NULL);
        if (fav->idStr == NULL || fav->list == NULL)
        {
            opl_favourite_Delete(fav);
            return NULL;
        }
    }
    return fav;
}

static Favourite_t *
    opl_favourite_NewWithJSON (const HCHAR *json)
{
    Favourite_t		*fav;
    HxJSON_t		root;
    HxJSON_t		list;
    HxJSON_t		item;
    OPL_Channel_t 	channel;
    HINT32			uid;

    TRACE();
    _DLOG("%s(%s)\n", __FUNCTION__, json);
    root = HLIB_JSON_Decode(json);
    RETURN_IF(root == (HxJSON_t)NULL, NULL);

    fav  = opl_favourite_New(
                      HLIB_JSON_Object_GetInteger(root, "id")
                    , HLIB_JSON_Object_GetString(root, "name")
                    , HLIB_JSON_Object_GetString(root, "extension")
                );
    GOTO_IF(fav == NULL, END);
    GOTO_IF(fav->list == NULL, END);

    list = HLIB_JSON_Object_GetArray(root, "list");
    if (list)
    {
        HxJSON_ARRAY_EACH(list, item, {
            if (!HxJSON_IsInteger(item))
                continue;

            uid = HxJSON_Integer_Get(item);
            channel = OPL_Channel_FindChannelByUID(NULL,uid);
            
            if (channel != NULL)
                HLIB_VECTOR_Add(fav->list, (void *)uid);
        });
    }
END:
    HLIB_JSON_Delete(root);
    return fav;
}

static const HCHAR *
    opl_favourite_DumpJSON (Favourite_t *fav)
{
    HCHAR	*s_buf=NULL;
    HINT32		sd;
    HINT32		uid;
    HINT32		count;

    TRACE();

    //if (s_buf == NULL)
    {
        s_buf = (HCHAR *)HLIB_STD_MemCalloc(FAV_STRBUF_SIZE);
        RETURN_IF(s_buf == NULL, NULL);
    }

    sd = HLIB_RWSTREAM_Open(s_buf, FAV_STRBUF_SIZE);
    RETURN_IF(!sd, NULL);

    GOTO_IF( HLIB_RWSTREAM_Print(sd, "{") < 0, ERROR );
    GOTO_IF( HLIB_RWSTREAM_Print(sd, "\"id\":%d,", fav->id) < 0, ERROR );
    if (fav->name)
        GOTO_IF( HLIB_RWSTREAM_Print(sd, "\"name\":\"%s\",", fav->name) < 0, ERROR );

    GOTO_IF( HLIB_RWSTREAM_Print(sd, "\"list\":[") < 0, ERROR );
    count = 0;
    HxVECTOR_EACH(fav->list, HINT32, uid, {
        if (uid == 0)
            continue;
        if (count > 0)
            GOTO_IF( HLIB_RWSTREAM_Print(sd, ",") < 0, ERROR );
        GOTO_IF( HLIB_RWSTREAM_Print(sd, "%d", (int)uid) < 0, ERROR );
        count++;
    });
    GOTO_IF( HLIB_RWSTREAM_Print(sd, "]}") < 0, ERROR );

    return (HCHAR *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
ERROR:
    HLIB_RWSTREAM_Close(sd);
    return NULL;
}

static void
    opl_favourite_SetName (Favourite_t *fav, const HCHAR *name)
{
    if (fav->name)
        HLIB_STD_MemFree(fav->name);
    fav->name = HLIB_STD_StrDup(name);
}

static HINT32
    opl_favourite_Compare (const void *arg1,const void *arg2)
{
    Favourite_t *lhs = *(Favourite_t **)arg1;
    Favourite_t *rhs = *(Favourite_t **)arg2;

    return lhs->id - rhs->id;
}


static void
    opl_favourite_Debug (Favourite_t *fav)
{
    FILE *fp;
    const HCHAR *json;
    int			uid;
    DxService_t	*service;
    HCHAR	buf[256], *p;

    TRACE();

    HxSTD_PrintToStrN(buf, 256, "fav%d-%s.txt", fav->id, fav->name);
    for (p = buf ; *p != '\0' ; p++) {
        if (!isalnum(*p) && *p != '.')
            *p = '_';
    }

    fp = fopen(buf, "w");
    RETURN_IF_VOID(fp == NULL);

    json = opl_favourite_DumpJSON(fav);
    if (json)
    {
        fprintf(fp, "%s\n\n", json);
        HLIB_STD_MemFree((void *)json);
    }

    HxVECTOR_EACH(fav->list, int, uid, {
        service = (DxService_t *)OPL_Channel_FindChannelByUID(NULL, uid);
        if (service)
            fprintf(fp, "%5d: (%4d) %s\n", uid, service->lcn, service->name);
        else
            fprintf(fp, "%5d: NULL\n", uid);
    });
    fclose(fp);
}


static const HCHAR *
    opl_favmanager_LoadJSON (const HCHAR *key)
{
    static HCHAR	*s_buf;
    HCHAR			*dst;
    HUINT32			len;

    TRACE();

    if (s_buf == NULL)
    {
        s_buf = (HCHAR *)HLIB_STD_MemCalloc(FAV_STRBUF_SIZE * 2);
        RETURN_IF(s_buf == NULL, NULL);
    }
    dst = s_buf + FAV_STRBUF_SIZE;

    RETURN_IF( APK_DB_GetStr((const HCHAR *)key, s_buf, FAV_STRBUF_SIZE) != ERR_OK, NULL );
    len = (HUINT32)strlen(s_buf);
    RETURN_IF( len == 0, NULL );
    RETURN_IF( HLIB_MATH_DecodeBase64Ex(dst, FAV_STRBUF_SIZE, s_buf, len) == NULL, NULL );

    return dst;
}

static HERROR
    opl_favmanager_RemoveJSON(const HCHAR *key)
{
    TRACE();
    APK_DB_Remove((const HCHAR *)key);
    return ERR_OK;
}

static HERROR
    opl_favmanager_SaveJSON (const HCHAR *key, const HCHAR *json)
{
    static HCHAR	*s_buf;
    HUINT32			len;

    TRACE();

    if (json == NULL)
    {
        APK_DB_SetStr(key, "");
        return ERR_OK;
    }

    len = (HUINT32)strlen(json);

    s_buf = HLIB_MATH_EncodeBase64Easy((const void *)json, len);

    RETURN_IF(s_buf == NULL, ERR_FAIL);
#if 0
    fflush(NULL);
    HxLog_Dump(json,len,0,1);
    fflush(NULL);
#endif

    APK_DB_SetStr(key, s_buf);

    HLIB_STD_MemFree(s_buf);

    return ERR_OK;
}

static void
    opl_favmanager_Load (FavouriteManager_t *mgr, HBOOL loadAll)
{
#if DONOTSAVE // do not save
printf("\n\n\nDONOTSAVE!!!\n\n\n");
    HINT32      i;
    HINT32      index;
    HINT32      uid;


	TRACE();


    if(loadAll) {
        for(i = 0; i < CHANNEL_GROUP_COUNT; i++) {
            Favourite_t *fav;
            fav = opl_favourite_New(mgr->nextId,opl_favourite_GetChannelGroupName(i),NULL);
            fav->ready = TRUE;
            switch(i) {
            case 0: // ALL
                if(allList == NULL)
                {
                    allList = HLIB_VECTOR_NewEx(NULL, 0, NULL, NULL, NULL);
                    fav->list = allList;
                }
                else
                {
                    index = 0;
                    HxVECTOR_EACH(allList, HINT32, uid, {
                                      OPL_Channel_t channel = OPL_Channel_FindChannelByUID(NULL, uid);
                                      if (channel == NULL)	// check!
                                      {
                                          HLIB_VECTOR_Remove(allList, index);
                                          continue;
                                      }
                                      index++;
                                  });
                    fav->list = allList;
                }
                break;
            case 1: // TV
                if(tvList == NULL)
                {
                    tvList = HLIB_VECTOR_NewEx(NULL, 0, NULL, NULL, NULL);
                    fav->list = tvList;
                }
                else
                {
                    index = 0;
                    HxVECTOR_EACH(tvList, HINT32, uid, {
                                      OPL_Channel_t channel = OPL_Channel_FindChannelByUID(NULL, uid);
                                      if (channel == NULL)	// check!
                                      {
                                          HLIB_VECTOR_Remove(tvList, index);
                                          continue;
                                      }
                                      index++;
                                  });
                    fav->list = tvList;
                }
                break;
            case 2: // RADIO
                if(radioList == NULL)
                {
                    radioList = HLIB_VECTOR_NewEx(NULL, 0, NULL, NULL, NULL);
                    fav->list = radioList;
                }
                else
                {
                    index = 0;
                    HxVECTOR_EACH(radioList, HINT32, uid, {
                                      OPL_Channel_t channel = OPL_Channel_FindChannelByUID(NULL, uid);
                                      if (channel == NULL)	// check!
                                      {
                                          HLIB_VECTOR_Remove(radioList, index);
                                          continue;
                                      }
                                      index++;
                                  });
                    fav->list = radioList;
                }
                break;
            case 3: // HD
                if(hdList == NULL)
                {
                    hdList = HLIB_VECTOR_NewEx(NULL, 0, NULL, NULL, NULL);
                    fav->list = hdList;
                }
                else
                {
                    index = 0;
                    HxVECTOR_EACH(hdList, HINT32, uid, {
                                      OPL_Channel_t channel = OPL_Channel_FindChannelByUID(NULL, uid);
                                      if (channel == NULL)	// check!
                                      {
                                          HLIB_VECTOR_Remove(hdList, index);
                                          continue;
                                      }
                                      index++;
                                  });
                    fav->list = hdList;
                }
                break;
            case 4: // FREE
                if(freeList == NULL)
                {
                    freeList = HLIB_VECTOR_NewEx(NULL, 0, NULL, NULL, NULL);
                    fav->list = freeList;
                }
                else
                {
                    index = 0;
                    HxVECTOR_EACH(freeList, HINT32, uid, {
                                      OPL_Channel_t channel = OPL_Channel_FindChannelByUID(NULL, uid);
                                      if (channel == NULL)	// check!
                                      {
                                          HLIB_VECTOR_Remove(freeList, index);
                                          continue;
                                      }
                                      index++;
                                  });
                    fav->list = freeList;
                }
                break;
            case 5: // PAY
                if(payList == NULL)
                {
                    payList = HLIB_VECTOR_NewEx(NULL, 0, NULL, NULL, NULL);
                    fav->list = payList;
                }
                else
                {
                    index = 0;
                    HxVECTOR_EACH(payList, HINT32, uid, {
                                      OPL_Channel_t channel = OPL_Channel_FindChannelByUID(NULL, uid);
                                      if (channel == NULL)	// check!
                                      {
                                          HLIB_VECTOR_Remove(payList, index);
                                          continue;
                                      }
                                      index++;
                                  });
                    fav->list = payList;
                }
                break;
            default:
                break;
            }
            HLIB_VECTOR_Add(mgr->favList, fav);
            mgr->nextId += 1;
        }
        for(i = 0; i < USER_FAVOURITE_COUNT; i++) {
            Favourite_t *fav;
            fav = opl_favourite_New(mgr->nextId,opl_favourite_GetUserFavouriteName(i),NULL);
            fav->ready = TRUE;
            if(favList[i] == NULL)
            {
                favList[i] = HLIB_VECTOR_NewEx(NULL, 0, NULL, NULL, NULL);
            }
            else
            {
                index = 0;
                HxVECTOR_EACH(favList[i], HINT32, uid, {
                                  OPL_Channel_t channel = OPL_Channel_FindChannelByUID(NULL, uid);
                                  if (channel == NULL)	// check!
                                  {
                                      HLIB_VECTOR_Remove(favList[i], index);
                                      continue;
                                  }
                                  index++;
                              });
                fav->list = payList;
            }
            fav->list = favList[i];
            HLIB_VECTOR_Add(mgr->favList, fav);
            mgr->nextId += 1;
        }
    }
#else

printf("\n\n\nSAVE!!!\n\n\n");

    const HCHAR	*json;
    HxJSON_t	root;
    HxJSON_t	list;
    HxJSON_t	value;
    Favourite_t	*fav;

    TRACE();

    json = opl_favmanager_LoadJSON("favourite");
    RETURN_IF_VOID(json == NULL);
    HxLOG_Debug("%s:%d::favourite:(%s)\n", __FUNCTION__, __LINE__, json);

    root = HLIB_JSON_Decode(json);
    RETURN_IF_VOID(root == (HxJSON_t)NULL);

    mgr->mainId = HLIB_JSON_Object_GetInteger(root, "main");

    list = HLIB_JSON_Object_GetArray(root, "list");
    if (loadAll && list)
    {
        HxJSON_ARRAY_EACH(list, value, {
            if (!HxJSON_IsString(value))
                continue;

            json = opl_favmanager_LoadJSON(HxJSON_String_Get(value));
            if (json == NULL)
                continue;

            fav  = opl_favourite_NewWithJSON(json);
            if (fav == NULL)
                continue;
            fav->ready = TRUE;

            HxLOG_Debug("%s:%d::%s:(%s)\n", __FUNCTION__, __LINE__, fav->idStr, json);
            HLIB_VECTOR_Add(mgr->favList, (void *)fav);
            if (fav->id > mgr->nextId)
            {
                mgr->nextId = fav->id + 1;
            }
        });
    }
    HLIB_JSON_Delete(root);
#endif
}

static HERROR
    opl_favmanager_SaveFavourite (Favourite_t *fav)
{
#if DONOTSAVE // do not save


TRACE();


printf("\n\n\nDONOTSAVE!!!\n\n\n");

    switch(fav->id) {
        case 1: // ALL
        allList = fav->list;
        break;
        case 2: // TV
        tvList = fav->list;
        break;
        case 3: // RADIO
        radioList = fav->list;
        break;
        case 4: // HD
        hdList = fav->list;
        break;
        case 5: // FREE
        freeList = fav->list;
        break;
        case 6: // PAY
        payList = fav->list;
        break;
        default :
        break;
    };
#else
printf("\n\n\nSAVE!!!\n\n\n");
    HERROR	nRet;
    const HCHAR *json;

    TRACE();

    RETURN_IF(fav->ready == FALSE, ERR_FAIL);
    json = opl_favourite_DumpJSON(fav);
    RETURN_IF(json == NULL, ERR_FAIL);

    nRet = opl_favmanager_SaveJSON(fav->idStr, json);

    HLIB_STD_MemFree((void *)json);

    return nRet;
#endif
}

static void
    opl_favmanager_Save (FavouriteManager_t *mgr, HBOOL saveAll)
{
#if DONOTSAVE // do not save
printf("\n\n\nDONOTSAVE!!!\n\n\n");
    Favourite_t *fav;


	TRACE();


    favKindList = mgr->favList;

    HxVECTOR_EACH(mgr->favList, Favourite_t *, fav, {
        if (!fav->ready)
            continue;
        if (saveAll)
            (void) opl_favmanager_SaveFavourite(fav);
    });
#else
printf("\n\n\nSAVE!!!\n\n\n");
    HINT32	count;
    HINT32	sd;
    HERROR	err;
    Favourite_t *fav;
    HCHAR	*pPtr;

    TRACE();

    sd = HLIB_RWSTREAM_Open(NULL, 0);
    RETURN_IF_VOID(!sd);

    HLIB_RWSTREAM_Print(sd, "{\"main\":%d,\"list\":[", mgr->mainId);

    count = 0;
    HxVECTOR_EACH(mgr->favList, Favourite_t *, fav, {
        if (!fav->ready)
            continue;
        if (count > 0)
            HLIB_RWSTREAM_PutChar(sd, ',');
        if (saveAll)
            (void) opl_favmanager_SaveFavourite(fav);
        HLIB_RWSTREAM_Print(sd, "\"%s\"", fav->idStr);
        count++;
    });
    HLIB_RWSTREAM_Print(sd, "]}");
    pPtr = (HCHAR *)HLIB_RWSTREAM_GetBuf(sd);
    err = opl_favmanager_SaveJSON("favourite", pPtr);
#if 0
    fflush(NULL);
    HxLog_Dump(pPtr,strlen(pPtr),0,1);
    fflush(NULL);
#endif
    HLIB_RWSTREAM_Close(sd);
    RETURN_IF_VOID(err != ERR_OK);
#endif
}



static FavouriteManager_t *
    opl_favmanager_GetManager (void)
{
    static FavouriteManager_t	*s_manager;

    if (s_manager == NULL)
    {
        s_manager = (FavouriteManager_t *)HLIB_STD_MemAlloc(sizeof(FavouriteManager_t));
        ASSERT(s_manager);

        s_manager->nextId = 1;
        s_manager->mainId = 0;
        s_manager->favList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))opl_favourite_Delete, opl_favourite_Compare);
        ASSERT(s_manager->favList);

        opl_favmanager_Load(s_manager, TRUE);
    }
    return s_manager;
}

static Favourite_t *
    opl_favmanager_GetFavourite (FavouriteManager_t *mgr, HINT32 id)
{
    Favourite_t *fav;

    if (mgr->favList) {
        HxVECTOR_EACH(mgr->favList, Favourite_t *, fav, {
            if (fav->id == id)
                return fav;
        });
    }
    HxLOG_Warning("Invalid handle!! (%s/favID:%d)\n", __FUNCTION__, (int)id);
    return NULL;
}

static HCHAR *
    opl_favmanager_GetNextName (FavouriteManager_t *mgr, HCHAR *name, HUINT32 n)
{
    Favourite_t *fav;
    HINT32		dup;

	TRACE();

    dup = 0;
    HxVECTOR_EACH(mgr->favList, Favourite_t *, fav, {
        if (fav->name && HLIB_STD_StrStartWith(fav->name, "Favourite"))
        {
            dup++;
        }
    });
    if (dup < 26)
        HxSTD_PrintToStrN(name, n, "Favourite%c", 'A' + dup);
    else
        HxSTD_PrintToStrN(name, n, "Favourite%d", dup);
    return name;
}

#define	____OPL_FAVORITE_PUBLIC_____________________________________________________________________

void		OPL_Favourite_ToUpdate ()
{
	if(allList == NULL && tvList == NULL && radioList == NULL && hdList == NULL && freeList == NULL && payList == NULL)
	{
		OPL_Favourite_Update();
	}
}

OPL_BOOL		OPL_Favourite_Activate (OPL_Favourite_t hFav)
{
    Favourite_t *fav;
    FavouriteManager_t *mgr;

    TRACE();
    RETURN_IF(!hFav, OPL_FALSE);

    mgr = opl_favmanager_GetManager();
    fav = opl_favmanager_GetFavourite(mgr, (HINT32)hFav);
    RETURN_IF(fav == NULL, OPL_FALSE);
    RETURN_IF(fav->ready == FALSE, OPL_FALSE);

    mgr->mainId = fav->id;
    opl_favmanager_Save(mgr, FALSE);
    return OPL_TRUE;
}

OPL_Favourite_t	OPL_Favourite_GetCurrent (void)
{
    Favourite_t *fav;
    FavouriteManager_t *mgr;

    TRACE();

    mgr = opl_favmanager_GetManager();
    fav = opl_favmanager_GetFavourite(mgr, mgr->mainId);
    RETURN_IF(fav == NULL, NULL);

    return (OPL_Favourite_t)fav->id;
}


unsigned int			OPL_Favourite_GetCollection (OPL_Favourite_t **favList)
{
    Favourite_t *fav;
    FavouriteManager_t *mgr;
    OPL_Favourite_t	*list;
    HINT32		i, n;

    TRACE();

    mgr = opl_favmanager_GetManager();
    n   = HLIB_VECTOR_Length(mgr->favList);
    RETURN_IF (n == 0, 0);

    list = (OPL_Favourite_t *)HLIB_STD_MemAlloc(sizeof(OPL_Favourite_t) * n);
    RETURN_IF(list == NULL, 0);
    i = 0;
    HxVECTOR_EACH(mgr->favList, Favourite_t *, fav, {
        if (list)
            list[i] = (OPL_Favourite_t)fav->id;
        i++;
    });

    if (i == 0)
    {
        HLIB_STD_MemFree(list);
        list = NULL;
    }
    *favList = list;

    return i;
}


OPL_Favourite_t	OPL_Favourite_New (const char *name, const char *extension, void *uids)
{
    Favourite_t			*fav;
    FavouriteManager_t	*mgr;
    HCHAR 	buf[64];

    TRACE();
    _DLOG("%s(%s)\n", __FUNCTION__, name);

    mgr = opl_favmanager_GetManager();

    if (name == NULL)
    {
        name = (const char *)opl_favmanager_GetNextName(mgr, buf, 64);
    }

    fav = opl_favourite_New(mgr->nextId++, (const HCHAR *)name, (const HCHAR *)extension);
    RETURN_IF(fav == NULL, NULL);

    if(uids != NULL)
    {
        HLIB_VECTOR_Append(fav->list, (HxVector_t *)uids, NULL);
    }

    HLIB_VECTOR_Add(mgr->favList, (void *)fav);
    return (OPL_Favourite_t)fav->id;
}

OPL_BOOL		OPL_Favourite_Remove (OPL_Favourite_t hFav)
{
    Favourite_t			*fav;
    FavouriteManager_t	*mgr;

    TRACE();

    mgr = opl_favmanager_GetManager();
    fav = opl_favmanager_GetFavourite(mgr, (HINT32)hFav);
    RETURN_IF(fav == NULL, OPL_FALSE);
	#if DONOTSAVE
	#else
    opl_favmanager_RemoveJSON(fav->idStr);
    #endif
    HLIB_VECTOR_Remove(mgr->favList, HLIB_VECTOR_IndexOf(mgr->favList, (void *)fav));
    opl_favmanager_Save(mgr, FALSE);
    return OPL_TRUE;
}


unsigned int			OPL_Favourite_GetFavouriteIDs (int channelUniqueID, char ***favIDs)
{
    Favourite_t			*fav;
    FavouriteManager_t	*mgr;
    HINT32				i, n;

    TRACE();

    mgr = opl_favmanager_GetManager();
    n   = HLIB_VECTOR_Length(mgr->favList);
    RETURN_IF( n == 0, 0 );

    if (favIDs)
    {
        *favIDs = (char **)HLIB_STD_MemAlloc(sizeof(char *) * n);
        RETURN_IF( *favIDs == NULL, 0 );
    }
    i = 0;
    HxVECTOR_EACH(mgr->favList, Favourite_t *, fav, {
        if (!fav->list)
            continue;
        
        if (HLIB_VECTOR_IndexOf(fav->list, (void *)channelUniqueID) < 0)
            continue;

        if (favIDs)
            (*favIDs)[i] = HLIB_STD_StrDup(opl_favourite_ToIDString(fav->id));
        i++;
    });

    if (i == 0 && favIDs)
    {
        HLIB_STD_MemFree(*favIDs);
        *favIDs = NULL;
    }
    return (unsigned int)i;
}

OPL_BOOL		OPL_Favourite_Commit (OPL_Favourite_t hFav, const char **ccids, unsigned int count)
{
    Favourite_t *fav;
    FavouriteManager_t *mgr;

    TRACE();

    mgr = opl_favmanager_GetManager();
    fav = opl_favmanager_GetFavourite(mgr, (HINT32)hFav);
    RETURN_IF(fav == NULL, OPL_FALSE);
    RETURN_IF(fav->list == NULL, OPL_FALSE);

    _DLOG("%s(%s:%s, count:%d)\n", __FUNCTION__, fav->idStr, fav->name, (int)count);

    HLIB_VECTOR_RemoveAll(fav->list);
    fav->ready = TRUE;

    if (ccids && count)
    {
        unsigned int	i;
        int		uid;

        for (i = 0 ; i < count ; i++)
        {
            if (ccids[i] == NULL)
                continue;

            uid = OPL_Channel_CCID2UniqueID(ccids[i]);
            if (uid == 0)
                continue;
            
            HLIB_VECTOR_Add(fav->list, (void *)uid);
        }
    }
    opl_favmanager_SaveFavourite(fav);
    opl_favmanager_Save(mgr, FALSE);

    //opl_favourite_Debug(fav);
    #if DONOTSAVE
	#else
    (void)opl_favourite_Debug;
	#endif

    return OPL_TRUE;
}

OPL_BOOL		OPL_Favourite_CommitALL (OPL_Favourite_t hFav, const char **ccids, unsigned int count)
{
    Favourite_t *fav;
    FavouriteManager_t *mgr;

    TRACE();

    mgr = opl_favmanager_GetManager();
    fav = opl_favmanager_GetFavourite(mgr, (HINT32)hFav);
    RETURN_IF(fav == NULL, OPL_FALSE);
    RETURN_IF(fav->list == NULL, OPL_FALSE);

    _DLOG("%s(%s:%s, count:%d)\n", __FUNCTION__, fav->idStr, fav->name, (int)count);

    fav->ready = TRUE;

    if (ccids && count)
    {
        unsigned int	i;
        int		uid;

        for (i = 0 ; i < count ; i++)
        {
            if (ccids[i] == NULL)
                continue;

            uid = OPL_Channel_CCID2UniqueID(ccids[i]);
            if (uid == 0)
                continue;

            HLIB_VECTOR_Add(fav->list, (void *)uid);
        }
    }
    opl_favmanager_Save(mgr, TRUE);

    //opl_favourite_Debug(fav);
    //(void)opl_favourite_Debug;

    return OPL_TRUE;
}


OPL_Favourite_t	OPL_Favourite_FindByID (const char *favID)
{
    Favourite_t *fav;
    FavouriteManager_t *mgr;

    TRACE();
    RETURN_IF(favID == NULL, NULL);

    mgr = opl_favmanager_GetManager();
    fav = opl_favmanager_GetFavourite(mgr, opl_favourite_ToID((const HCHAR *)favID));
    RETURN_IF(fav == NULL, NULL);

    return (OPL_Favourite_t)fav->id;
}


unsigned int			OPL_Favourite_GetChannelList (OPL_Favourite_t hFav, OPL_Channel_t **list)
{
    Favourite_t 		*fav;
    FavouriteManager_t *mgr;
    OPL_Channel_t		channel;
    int					uid;
    unsigned int				i, n;

    TRACE();

    mgr = opl_favmanager_GetManager();
    fav = opl_favmanager_GetFavourite(mgr, (HINT32)hFav);
    RETURN_IF(fav == NULL, 0);
    RETURN_IF(fav->list == NULL, 0);

    n = (unsigned int)HLIB_VECTOR_Length(fav->list);
    if (n == 0)
        return 0;

    if (list)
    {
        *list = (OPL_Channel_t *)HLIB_STD_MemAlloc(sizeof(OPL_Channel_t) * n);
        RETURN_IF(*list == NULL, 0);
    }

    i = 0;
    HxVECTOR_EACH(fav->list, int, uid, {
        if (uid == 0)
            continue;
        channel = OPL_Channel_FindChannelByUID(NULL, uid);
        if (channel == NULL)	// check!
            continue;
        if (list)
        {
            (*list)[i] = OPL_Channel_Clone(channel);
            if ((*list)[i] == NULL)
                continue;
        }
        i++;
    });
    if (i == 0 && list)
    {
        HLIB_STD_MemFree(*list);
        *list = NULL;
    }
    _DLOG("%s(%s:%u)\n", __FUNCTION__, fav->name, i);
    return i;
}

OPL_Collection_t *OPL_Favourite_GetChannelCollection (OPL_Favourite_t hFav)
{
    OPL_Collection_t	*col;
    Favourite_t 		*fav;
    FavouriteManager_t	*mgr;
    int					uid;

    TRACE();

    mgr = opl_favmanager_GetManager();
    fav = opl_favmanager_GetFavourite(mgr, (HINT32)hFav);
    RETURN_IF(fav == NULL, NULL);
    RETURN_IF(fav->list == NULL, NULL);
    RETURN_IF(HLIB_VECTOR_Length(fav->list) == 0, NULL);

    col = OPL_Collection_New();
    RETURN_IF(col == NULL, NULL);

    HxVECTOR_EACH(fav->list, int, uid, {
        if (uid == 0)
            continue;
        OPL_Collection_Add(col, (void *)uid);
    });

    if (OPL_Collection_Length(col) == 0)
    {
        OPL_Collection_Delete(col);
        return NULL;
    }
    _DLOG("%s(%s:%u)\n", __FUNCTION__, fav->name, OPL_Collection_Length(col));
    return col;
}


const char *	OPL_Favourite_GetID (OPL_Favourite_t hFav)
{
    Favourite_t *fav;
    FavouriteManager_t *mgr;

    TRACE();

    mgr = opl_favmanager_GetManager();
    fav = opl_favmanager_GetFavourite(mgr, (HINT32)hFav);
    RETURN_IF(fav == NULL, NULL);

    return (char *)fav->idStr;
}

const char *	OPL_Favourite_GetName (OPL_Favourite_t hFav)
{
    Favourite_t *fav;
    FavouriteManager_t *mgr;

    TRACE();

    mgr = opl_favmanager_GetManager();
    fav = opl_favmanager_GetFavourite(mgr, (HINT32)hFav);
    RETURN_IF(fav == NULL, NULL);

    return (char *)fav->name;
}


void			OPL_Favourite_SetName (OPL_Favourite_t hFav, const char *name)
{
    Favourite_t *fav;
    FavouriteManager_t *mgr;

    TRACE();

    mgr = opl_favmanager_GetManager();
    fav = opl_favmanager_GetFavourite(mgr, (HINT32)hFav);
    RETURN_IF_VOID(fav == NULL);

    opl_favourite_SetName(fav, (const HCHAR *)name);
}


OPL_BOOL		OPL_Favourite_HasChannel (OPL_Favourite_t hFav, int channelUniqueID)
{
    Favourite_t			*fav;
    FavouriteManager_t	*mgr;

    TRACE();
    mgr = opl_favmanager_GetManager();
    fav = opl_favmanager_GetFavourite(mgr, (HINT32)hFav);
    RETURN_IF(fav == NULL, OPL_FALSE);
    RETURN_IF(fav->list == NULL, OPL_FALSE);

    if (HLIB_VECTOR_IndexOf(fav->list, (void *)channelUniqueID) < 0)
        return OPL_FALSE;
    return OPL_TRUE;
}

#if 0
void		OPL_Favourite_Update ()
{
    OPL_Favourite_t		*favList;
    FavouriteManager_t	*favManager;
    Favourite_t         *fav;
    OPL_Favourite_t		hFav;
    OPL_Channel_t		*chList;
    HxVector_t			**uids;
    DxService_t			*service;
    HxVector_t          *clistItems;
    CListItem_t         *item;
    HxVector_t			*pstSvcList;
	int					n_favListCollection;
    int					n_channel;
    int					uid;
    int                 i, j;

    TRACE();


    // remove ALL channel group
    favManager = opl_favmanager_GetManager();
    i = 0;
    HxVECTOR_EACH(favManager->favList, Favourite_t *, fav, {
                      if(fav)
                      {
                          for(j = 0; j < CHANNEL_GROUP_COUNT; j++)
                          {
                              if(strcmp(fav->name, opl_favourite_GetChannelGroupName(j)) == 0)
                              {
							  	printf("remove [%s] !!\n", fav->name);
                                  HLIB_VECTOR_RemoveAll(fav->list);
                                  break;
                              }
                          }
                      }
                      i++;
                  });
    opl_favmanager_Save(favManager, TRUE);


    // get channel
    n_channel = OPL_Channel_GetChannelList(NULL, &chList);
    pstSvcList = HLIB_VECTOR_NewEasy(1024, HLIB_STD_MemFree, NULL);
    for(i = 0; i < n_channel; i++)
    {
        service = (DxService_t *)chList[i];
        HLIB_VECTOR_Add(pstSvcList, HLIB_STD_MemDup(service, sizeof(DxService_t)));
    }


    uids = (HxVector_t **)HLIB_STD_MemAlloc(sizeof(HxVector_t *) * CHANNEL_GROUP_COUNT);
    RETURN_IF_VOID(uids  == NULL);
    for (i = 0; i < CHANNEL_GROUP_COUNT; i++)
        uids[i] = HLIB_VECTOR_New(NULL, NULL);



    // GET TV LIST!!!!
	printf("GET TV LIST!!!!\n");
    clistItems = CLIB_GenerateList(pstSvcList, NULL, CLIB_TYPE_TV, CLIB_OP_NULL, 0, TRUE);
    HxVECTOR_EACH(clistItems, CListItem_t *, item,{
		printf("uid=[%d] LCN=[%d]\n",item->uid, item->nLogicalNo);
            HLIB_VECTOR_Add(uids[1], (void *)item->uid);
        });
	printf("\n end \n");

    // GET RADIO LIST!!!!
	printf("GET RADIO LIST!!!!\n");
    clistItems = CLIB_GenerateList(pstSvcList, NULL, CLIB_TYPE_RADIO, CLIB_OP_NULL, 0, TRUE);
    HxVECTOR_EACH(clistItems, CListItem_t *, item,{
		printf("uid=[%d] LCN=[%d]\n",item->uid, item->nLogicalNo);
            HLIB_VECTOR_Add(uids[2], (void *)item->uid);
        });
	printf("\n end \n");

    // GET HDTV LIST!!!!
	printf("GET HDTV LIST!!!!\n");
    clistItems = CLIB_GenerateList(pstSvcList, NULL, CLIB_TYPE_HD, CLIB_OP_NULL, 0, TRUE);
    HxVECTOR_EACH(clistItems, CListItem_t *, item,{
		printf("uid=[%d] LCN=[%d]\n",item->uid, item->nLogicalNo);
            HLIB_VECTOR_Add(uids[3], (void *)item->uid);
        });
	printf("\n end \n");


    // GET PAYTV LIST!!!!
	printf("GET PAYTV LIST!!!!\n");
    clistItems = CLIB_GenerateList(pstSvcList, NULL, CLIB_TYPE_CAS, CLIB_OP_NULL, 0, TRUE);
    HxVECTOR_EACH(clistItems, CListItem_t *, item,{
		printf("uid=[%d] LCN=[%d]\n",item->uid, item->nLogicalNo);
            HLIB_VECTOR_Add(uids[5], (void *)item->uid);
        });
	printf("\n end \n");

    for(i = 0; i < n_channel; i++)
    {
        service = (DxService_t *)chList[i];
        if(service == NULL)
            continue;

        uid = service->uid;

        // get ALL uids
        HLIB_VECTOR_Add(uids[0], (void *)uid);

        switch(service->svcType)
        {
            case eDxSVC_TYPE_TV : // get TV uids
                /*
                HLIB_VECTOR_Add(uids[1], (void *)uid);
                if(service->highDefinition == eDxVIDEO_TYPE_HD)
                {
                    HLIB_VECTOR_Add(uids[3], (void *)uid);
                }
                */
                switch(service->casType)
                {
                    // get FreeTV uids
                    case eDxCAS_TYPE_All :
                        break;
                    case eDxCAS_TYPE_FTA :
                        HLIB_VECTOR_Add(uids[4], (void *)uid);
                        break;
                        /*
                    // get PayTV uids
                    case eDxCAS_TYPE_NAGRA :
                    case eDxCAS_TYPE_IRDETO :
                    case eDxCAS_TYPE_NDS :
                    case eDxCAS_TYPE_VIACCESS :
                    case eDxCAS_TYPE_CONAX :
                    case eDxCAS_TYPE_MEDIAGUARD :
                    case eDxCAS_TYPE_CRYPTOWORKS :
                    case eDxCAS_TYPE_JPBCAS :
                    case eDxCAS_TYPE_JPCCAS :
                    case eDxCAS_TYPE_ALLCAS  :
                        HLIB_VECTOR_Add(uids[5], (void *)uid);
                        break;
                        */
                    default:
                        break;
                }
                break;
                /*
            case eDxSVC_TYPE_RADIO : // get Radio uids
                HLIB_VECTOR_Add(uids[2], (void *)uid);
                break;
                */
            default:
                break;
        }
    }



    n_favListCollection = OPL_Favourite_GetCollection(&favList);


	if(n_favListCollection == 0)
	{
	    // create channel group
	    for(i = 0; i < CHANNEL_GROUP_COUNT; i++)
	    {
	        const char *favName;
	        favName = opl_favourite_GetChannelGroupName(i);
	        hFav = OPL_Favourite_New(favName, NULL, uids[i]);
	        OPL_Favourite_CommitALL(hFav, NULL, 0);
	    }

	    // create User favourite
	    for(i = 0; i < USER_FAVOURITE_COUNT; i++)
	    {
	        const char *favName;
	        favName = opl_favourite_GetUserFavouriteName(i);
	        hFav = OPL_Favourite_New(favName, NULL, NULL);
	        OPL_Favourite_Commit(hFav, NULL, 0);
	    }
	}
	else
	{
		favManager = opl_favmanager_GetManager();
    	i = 0;
		HxVECTOR_EACH(favManager->favList, Favourite_t *, fav, {
			if(fav)
			{
				for(j = 0; j < CHANNEL_GROUP_COUNT; j++)
				{
					if(strcmp(fav->name, opl_favourite_GetChannelGroupName(j)) == 0)
					{
						printf("append [%s] !!\n", fav->name);
						HLIB_VECTOR_Append(fav->list, uids[i], NULL);
						break;
					}
				}
			}
			i++;
		});
    	opl_favmanager_Save(favManager, TRUE);
	}

    HLIB_STD_MemFree(uids);

    if (NULL != chList)
    {
        HLIB_STD_MemFree(chList);
    }
    return;
}


#else
void			OPL_Favourite_Update ()
{
	OPL_Favourite_t 	   	*favList;
	FavouriteManager_t  	*favManager;
	Favourite_t		   		*fav;
	OPL_Favourite_t	   		hFav;
	OPL_Channel_t	   		*chList;
	HxVector_t		   		**uids;
	DxService_t		   		*service;
	//HxVector_t		   		*clistItems;
	//CListItem_t		   		*item;
	HxVector_t		   		*pstSvcList;
	int				   		n_favListCollection;
	int				   		n_channel;
	int				  		uid;
	int				   		i, j;

	TRACE();


	// remove ALL channel group
	favManager = opl_favmanager_GetManager();
	i = 0;
	HxVECTOR_EACH(favManager->favList, Favourite_t *, fav, {
					 if(fav)
					 {
						 for(j = 0; j < CHANNEL_GROUP_COUNT; j++)
						 {
							 if(strcmp(fav->name, opl_favourite_GetChannelGroupName(j)) == 0)
							 {
                                                                printf("remove [%s] !!\n", fav->name);
                                                                if(fav->list)
                                                                {
                                                                        HLIB_VECTOR_RemoveAll(fav->list);
                                                                }
								break;
							 }
						 }
					 }
					 i++;
				 });
	opl_favmanager_Save(favManager, TRUE);


	// get channel
	n_channel = OPL_Channel_GetChannelList(NULL, &chList);
	pstSvcList = HLIB_VECTOR_NewEasy(1024, HLIB_STD_MemFree_CB, NULL);
	for(i = 0; i < n_channel; i++)
	{
	   service = (DxService_t *)chList[i];
	   HLIB_VECTOR_Add(pstSvcList, HLIB_STD_MemDup(service, sizeof(DxService_t)));
	}


	uids = (HxVector_t **)HLIB_STD_MemAlloc(sizeof(HxVector_t *) * CHANNEL_GROUP_COUNT);
	RETURN_IF_VOID(uids	== NULL);
	for (i = 0; i < CHANNEL_GROUP_COUNT; i++)
	   uids[i] = HLIB_VECTOR_New(NULL, NULL);


	for(i = 0; i < n_channel; i++)
    {
        service = (DxService_t *)chList[i];
        if(service == NULL)
            continue;

        uid = service->uid;

        // get ALL uids
        HLIB_VECTOR_Add(uids[0], (void *)uid);

        switch(service->svcType)
        {
            case eDxSVC_TYPE_TV : // get TV uids
                HLIB_VECTOR_Add(uids[1], (void *)uid);
                if(service->highDefinition == eDxVIDEO_TYPE_HD)
                {
                    HLIB_VECTOR_Add(uids[3], (void *)uid);
                }
                switch(service->casType)
                {
                    // get FreeTV uids
                    case eDxCAS_TYPE_All :
                        break;
                    case eDxCAS_TYPE_FTA :
                        HLIB_VECTOR_Add(uids[4], (void *)uid);
                        break;
                    // get PayTV uids
                    case eDxCAS_TYPE_NAGRA :
                    case eDxCAS_TYPE_IRDETO :
                    case eDxCAS_TYPE_NDS :
                    case eDxCAS_TYPE_VIACCESS :
                    case eDxCAS_TYPE_CONAX :
                    case eDxCAS_TYPE_MEDIAGUARD :
                    case eDxCAS_TYPE_CRYPTOWORKS :
                    case eDxCAS_TYPE_VERIMATRIX :
                    case eDxCAS_TYPE_JPBCAS :
                    case eDxCAS_TYPE_JPCCAS :
                    case eDxCAS_TYPE_ALLCAS  :
                        HLIB_VECTOR_Add(uids[5], (void *)uid);
                        break;
                    default:
                        break;
                }
                break;
            case eDxSVC_TYPE_RADIO : // get Radio uids
                HLIB_VECTOR_Add(uids[2], (void *)uid);
                break;
            default:
                break;
        }
    }


	n_favListCollection = OPL_Favourite_GetCollection(&favList);


	if(n_favListCollection == 0)
	{
	   // create channel group
	   for(i = 0; i < CHANNEL_GROUP_COUNT; i++)
	   {
		   const char *favName;
		   favName = opl_favourite_GetChannelGroupName(i);
		   hFav = OPL_Favourite_New(favName, NULL, uids[i]);
		   OPL_Favourite_CommitALL(hFav, NULL, 0);
	   }

	   // create User favourite
	   for(i = 0; i < USER_FAVOURITE_COUNT; i++)
	   {
		   const char *favName;
		   favName = opl_favourite_GetUserFavouriteName(i);
		   hFav = OPL_Favourite_New(favName, NULL, NULL);
		   OPL_Favourite_Commit(hFav, NULL, 0);
	   }
	}
	else
	{
	   favManager = opl_favmanager_GetManager();
	   i = 0;
	   HxVECTOR_EACH(favManager->favList, Favourite_t *, fav, {
		   if(fav)
		   {
			   for(j = 0; j < CHANNEL_GROUP_COUNT; j++)
			   {
				   if(strcmp(fav->name, opl_favourite_GetChannelGroupName(j)) == 0)
				   {
					   printf("append [%s] !!\n", fav->name);
                                           if(fav->list)
                                           {
					        HLIB_VECTOR_Append(fav->list, uids[i], NULL);
                                           }
					   break;
				   }
			   }
		   }
		   i++;
	   });
	   opl_favmanager_Save(favManager, TRUE);
	}

	HLIB_STD_MemFree(uids);

	if (NULL != chList)
	{
	   HLIB_STD_MemFree(chList);
	}

	if (NULL != favList)
	{
		HLIB_STD_MemFree(favList);
	}
	return;
}

#endif



