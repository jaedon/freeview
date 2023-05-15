/**************************************************************************************/
/**
 * @file oplairplay_x.cpp
 *
 * Oplairplay
 *
 * @author  Jeehune Park(parkjh4@humaxdigital.com)
 * @date    2013/9/05
 * @attention   Copyright (c) 2013 Humax - All rights reserved.
 *
 **************************************************************************************
 **/
#include "bpldb.h"
#include "oplairplay.h"
#include "oapi.h"
#include "apk.h"
#include <hlib.h>
#include <vkernel.h>

static OPL_AIRPLAY_EventCallback eventListener     = NULL;

#define ________________Typedef____________________________________
typedef struct
{
    HBOOL                        bIsRunning;
} OPL_AIRPLAY_Context_t;

#define OPL_PBSTATUS_PAUSE    "PAUSED"
#define OPL_PBSTATUS_LOADING    "LOADING"
#define OPL_PBSTATUS_PLAYING    "PLAYING"
#define OPL_PBSTATUS_EOF        "EOF"
#define OPL_PBSTATUS_STOPPED    "STOPPED"
#define OPL_PBSTATUS_ERROR    "ERROR"


#define ________________Static_Variables___________________________
static OPL_AIRPLAY_Context_t s_AIRPLAY_Context = {0,};

HBOOL                        g_bSendPlaybackInfo = FALSE;
HBOOL                        g_bExitPlaybackInfo = FALSE;
pthread_t                    g_pPlaybackInfoThread;
pthread_mutex_t                        g_mutex;
unsigned long               airplay_TimerID = 0;
static Opl_AIRPLAY_PlaybackInfo_t   g_pbinfo = { eOPL_AIRPLAY_PBSTATUS_Stopped, 0, 0, 0, 0, 0, 0 };

#define ____________INTERNEL_OPL_AIRPLAY_______________

static OPL_AIRPLAY_Context_t *__get_context( void )
{
    return &s_AIRPLAY_Context;
}

static BPLStatus __airplay_listener(HUINT32 p1, HUINT32 p2, HUINT32 p3)
{
    HUINT32 event = eOPL_AIRPLAY_REQ_UNKNOWN;
    HINT64  decimalVal = 0;
    HCHAR   *pUrl = NULL;
    HCHAR   *pTitle = NULL;
    HCHAR   *pAlbum = NULL;
    HCHAR   *pArtist = NULL;
    HCHAR   *pPath = NULL;
    
    HxLOG_Debug("[ APK_AIRPLAY ] received event [%d] \n", p1);
    switch ( p1 )
    {
        case eAPK_AIRPLAY_RequestPlay :
            event = eOPL_AIRPLAY_REQ_PLAY;
            {
                APKS_AIRPLAY_ReqPlayParam_t *pParam = (APKS_AIRPLAY_ReqPlayParam_t*)p2;
                decimalVal = pParam->nPlayPosition;
                pUrl = pParam->contentsURL;
                HxLOG_Debug("[ APK_AIRPLAY ] PLAY with url [%s] start from [%lld] \n", pUrl, decimalVal);
            }
            break;
        case eAPK_AIRPLAY_RequestShowPhoto :
            event = eOPL_AIRPLAY_REQ_SHOWPHOTO;
            {
                APKS_AIRTUNES_ReqSetAlbumart_t * pParam = (APKS_AIRTUNES_ReqSetAlbumart_t*)p2;
                pPath = pParam->szAlbumArtPath;
            }
            break;
        case eAPK_AIRPLAY_RequestStop :
            event = eOPL_AIRPLAY_REQ_STOP;
            decimalVal = 0;
            pUrl = NULL;
            HxLOG_Debug("[ APK_AIRPLAY ] Stops \n");
            break;
        case eAPK_AIRPLAY_RequestSetPosition:         
            event = eOPL_AIRPLAY_REQ_SETPOSITION;
            {
                APKS_AIRPLAY_ReqSetPositionParam_t *pParam = (APKS_AIRPLAY_ReqSetPositionParam_t*)p2;
                decimalVal = pParam->nPlayPosition;
            }
            HxLOG_Debug("[ APK_AIRPLAY ] SetPosition with val [%lld] \n", decimalVal);
            break;
        case eAPK_AIRPLAY_RequestSetRate:
            event = eOPL_AIRPLAY_REQ_SETRATE;
            {
                APKS_AIRPLAY_ReqSetRateParam_t *pParam = (APKS_AIRPLAY_ReqSetRateParam_t*)p2;
                decimalVal = pParam->nPlaySpeed;
            }
            HxLOG_Debug("[ APK_AIRPLAY ] SetSpeed with val [%lld] \n", decimalVal);
            break;
        case eAPK_AIRPLAY_RequestGetPlaybackInfo:            
            event = eOPL_AIRPLAY_REQ_GET_PLAYBACK_INFO;
            decimalVal = 0;
            pUrl = NULL;
             pthread_mutex_lock(&g_mutex);
            g_bSendPlaybackInfo = TRUE;
             pthread_mutex_unlock(&g_mutex);
            HxLOG_Debug("[ APK_AIRPLAY ] GetPlaybackInfo\n");
            break;
        case eAPK_AIRTUNES_RequestPlay :
            event = eOPL_AIRTUNES_REQ_PLAY;
            {
                APKS_AIRTUNES_ReqPlay_t * pParam = (APKS_AIRTUNES_ReqPlay_t*)p2;
                pUrl = (HCHAR *)pParam->contentURL;
            }
            HxLOG_Debug("[ APK_AIRPLAY ] Airtunes Play [%s]\n", pUrl);
            break;
        case eAPK_AIRTUNES_RequestStop :
            event = eOPL_AIRTUNES_REQ_STOP;
            HxLOG_Debug("[ APK_AIRPLAY ] Airtunes Stop\n");
            break;
        case eAPK_AIRTUNES_RequestSetAlbumart :
            event = eOPL_AIRTUNES_REQ_SETALBUMART;
            {
                APKS_AIRTUNES_ReqSetAlbumart_t * pParam = (APKS_AIRTUNES_ReqSetAlbumart_t*)p2;
                pPath = pParam->szAlbumArtPath;
            }
            HxLOG_Debug("[ APK_AIRPLAY ] Airtunes Set Albumart [%s]\n", pPath);
            break;
        case eAPK_AIRTUNES_RequestSetMetadata :
            event = eOPL_AIRTUNESY_REQ_SETMETADATA;
            {
                APKS_AIRTUNES_ReqSetMetadata_t * pParam = (APKS_AIRTUNES_ReqSetMetadata_t*)p2;
                pTitle = pParam->szTitle;
                pAlbum = pParam->szAlbum;
                pArtist = pParam->szArtist;
            }
            HxLOG_Debug("[ APK_AIRPLAY ] Airtunes Set Metadata Title [%s]\n", pTitle);
            break;
        case eAPK_AIRTUNES_RequestFlush :
            event = eOPL_AIRTUNES_REQ_FLUSH;
            HxLOG_Debug("[ APK_AIRPLAY ] Airtunes Flush\n");
            break;
        default:
            event = eOPL_AIRPLAY_REQ_UNKNOWN;
            break;
    }

    if(NULL != eventListener) {
        eventListener(event, decimalVal, pUrl?(char*)pUrl:NULL, pTitle?(char*)pTitle:NULL, pAlbum?(char*)pAlbum:NULL, pArtist?(char*)pArtist:NULL, pPath?(char*)pPath:NULL );
    }
    return BPL_STATUS_OK;
}

static void* __send_playback_info_task(void* args)
{
    while(!g_bExitPlaybackInfo)
    {
        pthread_mutex_lock(&g_mutex);
        if(g_bSendPlaybackInfo)
        {
            g_bSendPlaybackInfo = FALSE;
            HxLOG_Debug("[NativeHAirplay] Send playback info\n");
            HxLOG_Debug("Playback status : %d\n", g_pbinfo.ePlaybackStatus);
            HxLOG_Debug("Position : %lld\n", g_pbinfo.ullPosition);
            HxLOG_Debug("Rate : %d\n", g_pbinfo.unRate);
            OPL_AIRPLAY_SetPlaybackInfo(g_pbinfo);  /* send cached pbinfo */
        }
        pthread_mutex_unlock(&g_mutex);
        usleep(100000);
    }
    return NULL;
}

int __create_async_send_playbackinfo_task()
{
    pthread_mutex_init(&g_mutex, NULL);

    pthread_mutex_lock(&g_mutex);
    g_bExitPlaybackInfo = false;
    pthread_mutex_unlock(&g_mutex);
    
    int r = pthread_create(&g_pPlaybackInfoThread, NULL, __send_playback_info_task, (void *)NULL);
    if ( r != 0 )
    {
        HxLOG_Debug("Cannot create threadFunc task.\n");
    }
    return r;
}
static void _OPL_AIRPLAY_TimerCallback(unsigned long timerID, void *param)
{
    /*
    @ note : this function is called on different thread, becuase of vk_timer
    @ TODO : have to test on supporting airplay evn. don't test this function yet
    */
    HxLOG_Debug("_OPL_AIRPLAY_TimerCallback!!! %d\n",HLIB_STD_GetTID());
    if (timerID == airplay_TimerID)
    {
      
        pthread_mutex_lock(&g_mutex);
        g_bExitPlaybackInfo = true;
        pthread_mutex_unlock(&g_mutex);

        int status = 0;       
        pthread_join(g_pPlaybackInfoThread, reinterpret_cast<void **>(&status));
        g_pPlaybackInfoThread = 0;
        pthread_mutex_destroy(&g_mutex);

        OPL_AIRPLAY_Context_t * pAirplay = __get_context();
        if(pAirplay->bIsRunning == FALSE)
        {
            APK_DB_SetInt(USERDB_AIRPLAY_ACTIVATION, (HUINT32)FALSE );
            return;
        }
        
        if ( APK_AIRPLAY_Stop() == ERR_OK )
        {
            APK_DB_SetInt(USERDB_AIRPLAY_ACTIVATION, (HUINT32)FALSE );
        
            pAirplay->bIsRunning = FALSE;
        }  
        
    }
}
#define ____________PUBLIC_OPL_AIRPLAY_______________
BPLStatus    OPL_AIRPLAY_Start( void )
{
    OPL_AIRPLAY_Context_t *pAirplay = __get_context();

    if(pAirplay->bIsRunning == TRUE)
    {
        APK_DB_SetInt(USERDB_AIRPLAY_ACTIVATION, (HUINT32)TRUE );    
        return BPL_STATUS_OK;
    }

    if ( APK_AIRPLAY_Start() == ERR_OK )
    {
        APK_DB_SetInt(USERDB_AIRPLAY_ACTIVATION, (HUINT32)TRUE );

         if ( __create_async_send_playbackinfo_task() == 0 )
         {
            pAirplay->bIsRunning = TRUE;
            HxLOG_Debug("Create airplay thread \n");         
            return BPL_STATUS_OK;
         }
    }
    APK_DB_SetInt(USERDB_AIRPLAY_ACTIVATION, (HUINT32)FALSE );
    return BPL_STATUS_ERROR;
}

BPLStatus    OPL_AIRPLAY_Stop( void )
{
    HxLOG_Debug("OPL_AIRPLAY_Stop!!! %d\n",HLIB_STD_GetTID());

    const int AIRPLAY_STOP_TIMEOUT = 5*1000;
    HERROR re = ERR_OK;

    if ( airplay_TimerID != 0 )  
    {
        re = VK_TIMER_Cancel(airplay_TimerID);
        airplay_TimerID = 0;            
    }
    if ( re == ERR_OK )
    {
        HxLOG_Debug("Create airplay stop timer \n");
        re = VK_TIMER_EventAfter(AIRPLAY_STOP_TIMEOUT, _OPL_AIRPLAY_TimerCallback, (void *)NULL, (int)0, &airplay_TimerID);
        return BPL_STATUS_OK;
    }   
    return BPL_STATUS_ERROR;
    
}

BPLStatus    OPL_AIRPLAY_RegisterListener( OPL_AIRPLAY_EventCallback listener )
{
    eventListener = listener ;
    APK_AIRPLAY_RegisterNotifier((APKS_AIRPLAY_CbHandler) __airplay_listener );
    return BPL_STATUS_OK;
}

BPLStatus    OPL_AIRPLAY_UnRegisterListener(void)
{
    APK_AIRPLAY_UnRegisterNotifier((APKS_AIRPLAY_CbHandler) __airplay_listener);
    eventListener = NULL;
    return BPL_STATUS_OK;
}

BPLStatus    OPL_AIRPLAY_SetPlaybackInfo (Opl_AIRPLAY_PlaybackInfo_t tPbinfo)
{
    APKS_AIRPLAY_PlaybackInfo_t tPlaybackInfo;
    switch(tPbinfo.ePlaybackStatus)
    {
        case eOPL_AIRPLAY_PBSTATUS_Pause:
            tPlaybackInfo.ePlaybackStatus = eAPK_AIRPLAY_PBSTATUS_Pause;
            break;
        case eOPL_AIRPLAY_PBSTATUS_Playing:
            tPlaybackInfo.ePlaybackStatus = eAPK_AIRPLAY_PBSTATUS_Playing;
            break;
        case eOPL_AIRPLAY_PBSTATUS_ReachedEOF:
            tPlaybackInfo.ePlaybackStatus = eAPK_AIRPLAY_PBSTATUS_ReachedEOF;
            break;
        case eOPL_AIRPLAY_PBSTATUS_Stopped:
            tPlaybackInfo.ePlaybackStatus = eAPK_AIRPLAY_PBSTATUS_Stopped;
            break;
        case eOPL_AIRPLAY_PBSTATUS_Error:
            tPlaybackInfo.ePlaybackStatus = eAPK_AIRPLAY_PBSTATUS_Error;
            break;
        case eOPL_AIRPLAY_PBSTATUS_Loading:
            tPlaybackInfo.ePlaybackStatus = eAPK_AIRPLAY_PBSTATUS_Loading;
            break;            
        case eOPL_AIRPLAY_PBSTATUS_Unknown:
            tPlaybackInfo.ePlaybackStatus = eAPK_AIRPLAY_PBSTATUS_Unknown;
        default:
            break;
    }
    tPlaybackInfo.ullBufferedRange  = tPbinfo.ullBufferedRange;
    tPlaybackInfo.ullDuration       = tPbinfo.ullduration;
    tPlaybackInfo.ullPosition       = tPbinfo.ullPosition;
    tPlaybackInfo.ullSeekableRange  = tPbinfo.ullSeekableRange;
    tPlaybackInfo.ullStartPosition  = tPbinfo.ullStartPosition;
    tPlaybackInfo.unRate            = tPbinfo.unRate;

    HxLOG_Debug("[OPL_AIRPLAY] Send Playbackinfo\n");
    APK_AIRPLAY_SetPlaybackInfo(tPlaybackInfo);
    return BPL_STATUS_OK;
}

BPLStatus     OPL_AIRPLAY_IsActivated(HBOOL *bEnable)
{
    APK_DB_GetInt(USERDB_AIRPLAY_ACTIVATION, (HINT32*)bEnable);
    return BPL_STATUS_OK;
}
BPLStatus  OPL_AIRPLAY_SetPlaybackStatusInfo(const char* status)
{
    if ( status == NULL )
        return BPL_STATUS_ERROR;
    
    if(!strcasecmp (status, OPL_PBSTATUS_PAUSE))
    {
        g_pbinfo.ePlaybackStatus = eOPL_AIRPLAY_PBSTATUS_Pause;
    }
    else if(!strcasecmp (status, OPL_PBSTATUS_PLAYING))
    {
        g_pbinfo.ePlaybackStatus = eOPL_AIRPLAY_PBSTATUS_Playing;   
    }
    else if(!strcasecmp (status, OPL_PBSTATUS_EOF))
    {
        g_pbinfo.ePlaybackStatus = eOPL_AIRPLAY_PBSTATUS_ReachedEOF;   
    }
    else if(!strcasecmp (status, OPL_PBSTATUS_STOPPED))
    {
        g_pbinfo.ePlaybackStatus = eOPL_AIRPLAY_PBSTATUS_Stopped;
    }
    else if(!strcasecmp (status, OPL_PBSTATUS_ERROR))
    {
        g_pbinfo.ePlaybackStatus = eOPL_AIRPLAY_PBSTATUS_Error;   
    }
    else if(!strcasecmp (status, OPL_PBSTATUS_LOADING))
    {
        g_pbinfo.ePlaybackStatus = eOPL_AIRPLAY_PBSTATUS_Loading;   
    }
    else
    {
        g_pbinfo.ePlaybackStatus = eOPL_AIRPLAY_PBSTATUS_Unknown;
    }
    return BPL_STATUS_OK;
}

BPLStatus  OPL_AIRPLAY_SetBufferedRangeInfo(long long int buffered_range)
{
    g_pbinfo.ullBufferedRange = buffered_range;
    return BPL_STATUS_OK;
}

BPLStatus  OPL_AIRPLAY_SetSeekableRangeInfo(long long int seekable_range)
{
    g_pbinfo.ullSeekableRange = seekable_range;
    return BPL_STATUS_OK;
}

BPLStatus  OPL_AIRPLAY_SetDurationInfo(long long int duration)
{
    g_pbinfo.ullduration = duration;
    return BPL_STATUS_OK;
}

BPLStatus  OPL_AIRPLAY_SetPositionInfo(long long int position)
{
    g_pbinfo.ullPosition = position;
    return BPL_STATUS_OK;
}

BPLStatus  OPL_AIRPLAY_SetStartPositionInfo(long long int start_position)
{
    g_pbinfo.ullStartPosition = start_position;
    return BPL_STATUS_OK;
}

BPLStatus  OPL_AIRPLAY_SetRateInfo(int rate)
{
    g_pbinfo.unRate = rate;
    return BPL_STATUS_OK;
}
