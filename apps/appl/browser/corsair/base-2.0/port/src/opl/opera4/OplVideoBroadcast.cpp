/**************************************************************************************/
/**
 * @file bplbroadcast.cpp
 *
 * Video Broadcast
 *
 **************************************************************************************
 **/

#include "OplVideoBroadcast.h"
#include "oplfavourite.h"
#include "oplmetadatasearch.h"
#include "oplavcomponent.h"

#include "oplscheduler.h"
#include "oplscheduledprogramme.h"
#include "oplmetadatasearch.h"
#include "oplchannel.h"
#include "opltuner.h"

#if defined( CONFIG_OP_JAPAN)
#include "oplparser_japan.h"
#include "opljlabslocalsystem.h"
#endif

#include "oapi.h"
#include "apk.h"
#include "bpldb.h"
#include <hlib.h>
#include <hapi.h>
#include <thapi.h>
#include <vkernel.h>
#include <stdlib.h>
#include <string.h>


#if defined(CONFIG_OP_UK_DTT)
static HUINT32      ulTsrMaxOffset = 0;
#endif

OplVBroadcast *g_instance = NULL; /* this is only workaround cus of fucking static routine */
HxList_t *OplVBroadcast::s_pstBcastObjList = NULL;
HUINT32 OplVBroadcast::s_ulOplBroadcast_LastMasterSvcUid = 0;

/**
 * @warning OplVBroadcast will be offerd by singleton
 * So, DO NOT destory OplVBroadcast unless you met killing process.
 */
OplVBroadcast* OplVBroadcast::getInstance()
{
    if ( NULL == g_instance )
    {
        g_instance = new OplVBroadcast();
    }
    return g_instance;
}

void OplVBroadcast::destroyInstance()
{
    if ( g_instance )
    {
        delete g_instance;
        g_instance = NULL;
    }
}

bool OplVBroadcast::create(void* aVideoHandle, int aResourceId)
{
    OPL_HANDLE handle = aVideoHandle ? aVideoHandle: this;
//    m_vbcastHandle = OPL_Broadcast_Create(handle, aResourceId);

    static HBOOL s_bInited = FALSE;
    m_vbcastHandle = (OplBroadcast_t*)HLIB_STD_MemAlloc(sizeof(OplBroadcast_t));


#if defined(CONFIG_OP_JAPAN)
    APK_HANDLE                      hCasuiBcHandle;
#endif
    if (TRUE != s_bInited)
    {
        opl_broadcast_initialize();
        s_bInited = TRUE;
    }

    if ( m_vbcastHandle == NULL )
    {
        HxLOG_Error("Insufficient system memory.\n");
        return false;
    }

    m_vbcastHandle->hVbcastHandle = handle;
    m_vbcastHandle->ulViewId = aResourceId;
    m_vbcastHandle->liveEventListener       = NULL;
    m_vbcastHandle->tsrEventListener        = NULL;
    m_vbcastHandle->casEventListener        = NULL;
    m_vbcastHandle->recEventListener        = NULL;
    m_vbcastHandle->mailEventListener       = NULL;
    m_vbcastHandle->bmlEventListener        = NULL;
    m_vbcastHandle->mhegEventListener       = NULL;

    m_vbcastHandle->recorder    = NULL;
    m_vbcastHandle->ePlayState  = OPLBROADCAST_PLAY_STATE_UNREALIZED;
    m_vbcastHandle->eSessionState = OPLBROADCAST_SESSION_STATE_INITIALIZED;
    HxSTD_MemSet(&(m_vbcastHandle->stParentalRatingStatus), 0x00, sizeof( OplBroadcastParentalRatingStatus_t));

    m_vbcastHandle->bFullScreen = TRUE;

    m_vbcastHandle->nLastWindowPositionX = 0;
    m_vbcastHandle->nLastWindowPositionY = 0;
    m_vbcastHandle->nLastWindowPositionWidth = 0;
    m_vbcastHandle->nLastWindowPositionHeight = 0;

    /*clean TSR value*/
    m_vbcastHandle->nTsrPlaytime        =   0;
    m_vbcastHandle->nTsrPlaytimeTick    =   0;
    m_vbcastHandle->nTsrStarttime       =   0;
    m_vbcastHandle->nTsrStarttimeTick   =   0;

#if defined(CONFIG_OP_JAPAN)
    hCasuiBcHandle = APK_CAS_BC_GetHandle (eCasBcCtrlType_Drm, m_vbcastHandle->ulViewId);
    APK_CAS_BC_RegisterNotifier (hCasuiBcHandle, opl_broadcast_notifyDrmFunc);
    hCasuiBcHandle = APK_CAS_BC_GetHandle (eCasBcCtrlType_BsAutoMsg, m_vbcastHandle->ulViewId);
    APK_CAS_BC_RegisterNotifier (hCasuiBcHandle, opl_broadcast_notifyBsAutoMsgFunc);
    hCasuiBcHandle = APK_CAS_BC_GetHandle (eCasBcCtrlType_CaLinkMsg, m_vbcastHandle->ulViewId);
    APK_CAS_BC_RegisterNotifier (hCasuiBcHandle, opl_broadcast_notifyCaLinkMsgFunc);

    (void)opl_broadcast_notifyCasMessageFuncASync;
    (void)opl_broadcast_notifyCasEventFuncASync;
#else

    if (NULL == s_pstBcastObjList)
    {
        HERROR	hErr = APK_MEDIA_PLAY_RegisterNotifier (m_vbcastHandle->ulViewId, OplVBroadcast::opl_broadcast_notifyFuncFromAppkit, OplVBroadcast::opl_broadcast_eventFilter, this);
	if( ERR_OK != hErr )
		HxLOG_Error("[%s:%d] APK_MEDIA_PLAY_RegisterNotifier Error: \n", __FUNCTION__, __LINE__);
        APK_CAS_MMI_RegisterSessionNotifier(eDxCAS_SESSIONTYPE_BANNER, OplVBroadcast::opl_broadcast_notifyCasMessageFuncFromAppkit);
        APK_META_MAIL_RegisterNotifier( (APK_META_MAIL_Notifier_t)OplVBroadcast::opl_broadcast_notifyMailMessage, NULL );
    }
#endif
    s_pstBcastObjList = HLIB_LIST_Append (s_pstBcastObjList, (void *)m_vbcastHandle);

    m_vbcastHandle->hQueryHandle = OPL_METADATASEARCH_New(eOPL_SEARCH_TARGET_SCHEDULED);
    m_vbcastHandle->nPFLen  = 0;

#if defined(CONFIG_OP_JAPAN)
//  APK_EXE_BML_RegisterNotifier(opl_broadcast_bmlVod, hBcast);
#endif
    UpdateInitialState(m_vbcastHandle);

    HxLOG_Print("Creating the broadcast player instance is successful.\n");

    HAPI_ConnectSignal("signal:MhegEventNotify",      NULL, OplVBroadcast::opl_broadcase_NotifyMhegEventFromAppkit);

    return true;
}

void OplVBroadcast::destroy()
{
    if ( m_vbcastHandle == NULL )
    {
        HxLOG_Error("The handle of broadcast player is NULL.\n");
        return;
    }

    if ( m_vbcastHandle->recorder )
    {
        OPL_Recorder_Destroy(m_vbcastHandle->recorder);
        m_vbcastHandle->recorder = NULL;
    }

    s_pstBcastObjList = HLIB_LIST_Remove (s_pstBcastObjList, (void *)m_vbcastHandle);
    if ( NULL == s_pstBcastObjList )
    {
        APK_MEDIA_PLAY_UnregisterNotifier (m_vbcastHandle->ulViewId, OplVBroadcast::opl_broadcast_notifyFuncFromAppkit, this);
        APK_CAS_MMI_UnRegisterSessionNotifier(eDxCAS_SESSIONTYPE_BANNER);
        APK_META_MAIL_UnRegisterNotifier();
    }

    if (m_vbcastHandle->hQueryHandle)
        OPL_METADATASEARCH_Delete(m_vbcastHandle->hQueryHandle);

    while (m_vbcastHandle->nPFLen--)
    {
        OPL_Programme_Delete(m_vbcastHandle->ahPF[m_vbcastHandle->nPFLen]);
    }

    OPL_Broadcast_SetFullScreen(m_vbcastHandle, TRUE);

    HLIB_STD_MemFree(m_vbcastHandle);
    m_vbcastHandle = NULL;

    HxLOG_Print("Destroying the broadcast player instance is successful.\n");


}

OplVBroadcast::OplVBroadcast(void* aVideoHandle, int aResourceId)
    : s_ulOplBroadcast_LastSuppleSvcUid(0)
      ,s_chHandleOfSetchannel(NULL)
      ,s_bOplBcTvMemorized(FALSE)
      ,s_nOplBcTvMemorizedUid(0)
{
    create(aVideoHandle, aResourceId);
    m_recorder = NULL;
    m_videoHandle = aVideoHandle;
}

OplVBroadcast::~OplVBroadcast()
{
    if ( m_recorder ) delete m_recorder;
    m_recorder = NULL;

    destroy();
    m_vbcastHandle = NULL;
}

bool OplVBroadcast::getResourceId()
{
    if ( m_vbcastHandle == NULL )
    {
        HxLOG_Error("The handle of broadcast player is NULL.\n");
        return 0;
    }
    return (int)m_vbcastHandle->ulViewId;
}

void OplVBroadcast::setLiveEventListener(OplBroadcastListener_t listener)
{
    m_vbcastHandle->liveEventListener = listener;
}

void OplVBroadcast::setTsrEventListener(OplBroadcastListener_t listener)
{
    m_vbcastHandle->tsrEventListener = listener;
}

void OplVBroadcast::setCasEventListener(OplBroadcastListener_t listener)
{
    m_vbcastHandle->casEventListener = listener;
}

void OplVBroadcast::setRecEventListener(OplBroadcastListener_t listener)
{
    m_vbcastHandle->recEventListener = listener;
}

void OplVBroadcast::setMailEventListener(OplBroadcastListener_t listener)
{
    m_vbcastHandle->mailEventListener = listener;
}

void OplVBroadcast::setBmlEventListener(OplBroadcastListener_t listener)
{
    m_vbcastHandle->bmlEventListener = listener;
}

void OplVBroadcast::setMhegEventListener(OplBroadcastListener_t listener)
{
    m_vbcastHandle->mhegEventListener = listener;
}

bool OplVBroadcast::getCasMessage(unsigned int *pzModule, unsigned int *pzSlot, unsigned int *pulSession, char **ppszMessage, unsigned int *pulMessageLength)
{
    bool                eStatus = true;
    DxCAS_MMI_Event_t   *pstEvent = NULL;

    if (NULL == m_vbcastHandle)
    {
        HxLOG_Error("hBcast:0x%08x\n", (HUINT32)m_vbcastHandle);
        return false;
    }

    (*pzModule) = 0;
    (*pzSlot) = 0;
    (*pulSession) = (unsigned int)m_vbcastHandle->hCasMsgSession;

    APK_CAS_MMI_GetUiEvent(eDxCAS_SESSIONTYPE_BANNER, m_vbcastHandle->hCasMsgSession, &pstEvent);
    if(pstEvent == NULL)
    {
        eStatus = false;
        (*ppszMessage) = NULL;
    }
    else
    {
        (*pzModule) = pstEvent->eModule;
        (*pzSlot) = pstEvent->nSlot;

        if(pstEvent->pszEncodedUiEvent == NULL)
        {
            (*ppszMessage) = NULL;
            (*pulMessageLength) = 0;
        }
        else
        {
            (*ppszMessage) = (char*)HLIB_STD_MemDup(pstEvent->pszEncodedUiEvent, pstEvent->nEncodedMessageLength);
            (*pulMessageLength) = pstEvent->nEncodedMessageLength;
        }
        
        APK_CAS_MMI_RemoveUiEvent(eDxCAS_SESSIONTYPE_BANNER, pstEvent);
    }

    return eStatus;
}

bool OplVBroadcast::getCasUiEvtInfo(unsigned int *pzEvt, unsigned int *pzModule, unsigned int *pzSlot, unsigned int *pulSession)
{
    DxCAS_MMI_Event_t           *pstEvent = NULL;

    if (NULL == m_vbcastHandle)
    {
        HxLOG_Error("hBcast:0x%08x\n", (HUINT32)m_vbcastHandle);
        return false;
    }

    if((pzEvt == NULL) || (pzModule == NULL) || (pzSlot == NULL) || (pulSession == NULL))
    {
        return false;
    }

    APK_CAS_MMI_GetEvtInfo(&pstEvent);
    if(pstEvent != NULL)
    {
        (*pzEvt) = pstEvent->eventType;
        (*pzModule) = pstEvent->eModule;
        (*pzSlot) = pstEvent->nSlot;
        (*pulSession) = pstEvent->hSession;
        APK_CAS_MMI_RemoveEvtInfo(pstEvent);
    }

    return true;
}

bool OplVBroadcast::releaseCasMessage(char *pszMessage)
{
    if (NULL == m_vbcastHandle)
    {
        HxLOG_Error("hBcast:0x%08x\n", (HUINT32)m_vbcastHandle);
        return BPL_STATUS_ERROR;
    }

    if(pszMessage != NULL)
    {
        HLIB_STD_MemFree(pszMessage);
    }

    return BPL_STATUS_OK;
}

bool OplVBroadcast::getDrmError(unsigned int *pulDrmError, char **ppszCaSystemId)
{
#if defined(CONFIG_OP_JAPAN)
    APKS_CASBC_DrmData_t    *pstEvent = NULL;
    HERROR                      hErr = ERR_FAIL;
    BPLStatus                   bplStatus = BPL_STATUS_ERROR;
    HxLOG_Debug("+ [%s:%d]\n", __FUNCTION__, __LINE__);
    if ((NULL == m_vbcastHandle) || (pulDrmError == NULL) || (ppszCaSystemId == NULL))
    {
        HxLOG_Debug("-[%s:%d]\n", __FUNCTION__, __LINE__);
        return BPL_STATUS_ERROR;
    }

    (*ppszCaSystemId) = NULL;

    hErr = APK_CAS_BC_GetDrm(m_vbcastHandle->ulViewId, &pstEvent);
    if((hErr == ERR_OK) && (pstEvent != NULL))
    {
        *pulDrmError = pstEvent->ulError;
        if(pstEvent->pszCaSystemId != NULL)
        {
            (*ppszCaSystemId) = HLIB_STD_StrDup((HCHAR *)pstEvent->pszCaSystemId);
        }
        HxLOG_Print("ppszCaSystemId %s [%s:%d]\n",*ppszCaSystemId,__FUNCTION__, __LINE__);
        HxLOG_Print("*pulDrmError 0x%x [%s:%d]\n", *pulDrmError,__FUNCTION__, __LINE__);
        bplStatus = BPL_STATUS_OK;
    }
    else
    {

        bplStatus = BPL_STATUS_ERROR;
    }
    if(pstEvent != NULL)
    {
        APK_CAS_BC_FreeDrm(&pstEvent);
    }
    HxLOG_Debug("-[%s:%d]\n", __FUNCTION__, __LINE__);
    return bplStatus;
#else
    return BPL_STATUS_OK;
#endif
}

bool OplVBroadcast::releaseDrmError(char *pszCaSystemId)
{
#if defined(CONFIG_OP_JAPAN)
    HxLOG_Debug("+ [%s:%d] \n",__FUNCTION__,__LINE__);
    if (NULL == m_vbcastHandle)
    {
        HxLOG_Error("hBcast:0x%08x [%s:%d] \n", (HUINT32)hBcast,__FUNCTION__,__LINE__ );
        return BPL_STATUS_ERROR;
    }

    if(pszCaSystemId != NULL)
    {
        HLIB_STD_MemFree(pszCaSystemId);
    }
    HxLOG_Debug("- [%s:%d] \n",__FUNCTION__,__LINE__);
#endif
    return BPL_STATUS_OK;
}

bool OplVBroadcast::getDrmRightError(unsigned int *pulErrorState, char ** ppszContentId, char **ppszCaSystemId, char **ppszRightsIssuerURL)
{
#if defined(CONFIG_OP_JAPAN)
{
    APKS_CASBC_DrmData_t    *pstEvent = NULL;
    HERROR                      hErr = ERR_FAIL;
    BPLStatus                   bplStatus = BPL_STATUS_ERROR;
    HxLOG_Debug("+ [%s:%d]\n", __FUNCTION__, __LINE__);
    if ((NULL == m_vbcastHandle) || (pulErrorState == NULL) || (ppszCaSystemId == NULL)
    ||(ppszContentId == NULL) ||(ppszRightsIssuerURL == NULL))
    {
        HxLOG_Debug("-[%s:%d]\n", __FUNCTION__, __LINE__);
        return BPL_STATUS_ERROR;
    }

    (*ppszCaSystemId) = NULL;
    (*ppszContentId) = NULL;
    (*ppszRightsIssuerURL) = NULL;

    hErr = APK_CAS_BC_GetDrm(m_vbcastHandle->ulViewId, &pstEvent);
    if((hErr == ERR_OK) && (pstEvent != NULL))
    {
        *pulErrorState = pstEvent->ulErrorState;
        if(pstEvent->pszCaSystemId != NULL)
        {
            (*ppszCaSystemId) = HLIB_STD_StrDup((HCHAR *)pstEvent->pszCaSystemId);
        }
        if(pstEvent->pszContentId != NULL)
        {
            (*ppszContentId) = HLIB_STD_StrDup((HCHAR *)pstEvent->pszContentId);
        }
        if(pstEvent->pszRightsIssuerURL != NULL)
        {
            (*ppszRightsIssuerURL) = HLIB_STD_StrDup((HCHAR *)pstEvent->pszRightsIssuerURL);
        }
        HxLOG_Print("ppszCaSystemId %s [%s:%d]\n",*ppszCaSystemId,__FUNCTION__, __LINE__);
        HxLOG_Print("ppszContentId %s [%s:%d]\n",*ppszContentId,__FUNCTION__, __LINE__);
        HxLOG_Print("ppszRightsIssuerURL %s [%s:%d]\n",*ppszRightsIssuerURL,__FUNCTION__, __LINE__);
        HxLOG_Print("*pulErrorState %d [%s:%d]\n", *pulErrorState,__FUNCTION__, __LINE__);
        bplStatus = BPL_STATUS_OK;
    }
    else
    {

        bplStatus = BPL_STATUS_ERROR;
    }
    if(pstEvent != NULL)
    {
        APK_CAS_BC_FreeDrm(&pstEvent);
    }
    HxLOG_Debug("-[%s:%d]\n", __FUNCTION__, __LINE__);
    return bplStatus;
}
#elif  defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE) || defined(CONFIG_PROD_HMS1000T) || defined(CONFIG_PROD_DIGIMAXT2)
{
    if ((NULL == m_vbcastHandle) || (pulErrorState == NULL) || (ppszCaSystemId == NULL)
    ||(ppszContentId == NULL) ||(ppszRightsIssuerURL == NULL))
    {
        HxLOG_Debug("-[%s:%d]\n", __FUNCTION__, __LINE__);
        return BPL_STATUS_ERROR;
    }
    /*contentID�� � ���·� ������ �𸣰���. �ʿ��� ��ｿ����������*/
    *ppszContentId = NULL;
    *pulErrorState = m_vbcastHandle->stDrmRightsStatus.errorState;
    if(m_vbcastHandle->stDrmRightsStatus.drmSystemId != NULL)
    {
        (*ppszCaSystemId) = HLIB_STD_StrDup((HCHAR *)m_vbcastHandle->stDrmRightsStatus.drmSystemId);
    }
    if(m_vbcastHandle->stDrmRightsStatus.contentId != NULL)
    {
        (*ppszContentId) = HLIB_STD_StrDup((HCHAR *)m_vbcastHandle->stDrmRightsStatus.contentId);
    }
    if(m_vbcastHandle->stDrmRightsStatus.rightsIssuerUrl != NULL)
    {
        (*ppszRightsIssuerURL) = HLIB_STD_StrDup((HCHAR *)m_vbcastHandle->stDrmRightsStatus.rightsIssuerUrl);
    }
    HxLOG_Print("ppszCaSystemId %s [%s:%d]\n",*ppszCaSystemId,__FUNCTION__, __LINE__);
    HxLOG_Print("ppszContentId %s [%s:%d]\n",*ppszContentId,__FUNCTION__, __LINE__);
    HxLOG_Print("ppszRightsIssuerURL %s [%s:%d]\n",*ppszRightsIssuerURL,__FUNCTION__, __LINE__);
    HxLOG_Print("*pulErrorState %d [%s:%d]\n", *pulErrorState,__FUNCTION__, __LINE__);
    return BPL_STATUS_OK;
}
#else
    return BPL_STATUS_OK;
#endif
}

bool OplVBroadcast::releaseDrmRightError(char *pszContentId, char *pszCaSystemId, char *pszRightsIssuerURL)
{
#if defined(CONFIG_OP_JAPAN) || defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE) || defined(CONFIG_PROD_HMS1000T) || defined(CONFIG_PROD_DIGIMAXT2)
    HxLOG_Debug("+ [%s:%d] \n",__FUNCTION__,__LINE__);

    if (NULL == m_vbcastHandle)
    {
        HxLOG_Error("hBcast:0x%08x [%s:%d] \n", (HUINT32)m_vbcastHandle,__FUNCTION__,__LINE__ );
        return BPL_STATUS_ERROR;
    }

    if(pszContentId != NULL)
    {
        HLIB_STD_MemFree(pszContentId);
    }
    if(pszCaSystemId != NULL)
    {
        HLIB_STD_MemFree(pszCaSystemId);
    }
    if(pszRightsIssuerURL != NULL)
    {
        HLIB_STD_MemFree(pszRightsIssuerURL);
    }
    HxLOG_Debug("- [%s:%d] \n",__FUNCTION__,__LINE__);
#endif
    return BPL_STATUS_OK;
}

bool OplVBroadcast::getAutoDispMsg(OPL_AutoDispMsg_t **pphAutoDispMsg)
{
#if defined(CONFIG_OP_JAPAN)
    APKS_CASBC_BsAutoMsgData_t  *pstAutoDispMsg = NULL;
    HERROR                      hErr = ERR_FAIL;
    BPLStatus                   bplStatus = BPL_STATUS_ERROR;
    HxLOG_Debug("+ [%s:%d]\n", __FUNCTION__, __LINE__);
    if ((NULL == m_vbcastHandle) || (pphAutoDispMsg == NULL))
    {
        HxLOG_Debug("-[%s:%d]\n", __FUNCTION__, __LINE__);
        bplStatus = BPL_STATUS_ERROR;
        goto funcExit;
    }

    hErr = APK_CAS_BC_GetAutoDispMsg(m_vbcastHandle->ulViewId, &pstAutoDispMsg);
    if((hErr == ERR_OK) && (pstAutoDispMsg != NULL))
    {
        *pphAutoDispMsg = (OPL_AutoDispMsg_t *)pstAutoDispMsg;
        bplStatus = BPL_STATUS_OK;
    }
    else
    {
        bplStatus = BPL_STATUS_ERROR;
    }
funcExit:
    HxLOG_Debug("-[%s:%d]\n", __FUNCTION__, __LINE__);
    return bplStatus;
#else
    return BPL_STATUS_OK;
#endif
}

bool OplVBroadcast::getNewMailMsg(OPL_MailHandle *hMail)
{
    BPLStatus                       bplStatus = BPL_STATUS_ERROR;

    if( m_vbcastHandle && m_vbcastHandle->hMail )
    {
        *hMail = m_vbcastHandle->hMail;
        bplStatus = BPL_STATUS_OK;
    }

    return bplStatus;
}

bool OplVBroadcast::getCaLinkMsg(HUINT32 *pOnId, HUINT32 *pTsId, HUINT32 *pSvcId, HUINT8 **ppszCaLinkMsg)
{
#if defined(CONFIG_OP_JAPAN)
    HERROR                      hErr = ERR_FAIL;
    BPLStatus                   bplStatus = BPL_STATUS_ERROR;
    HxLOG_Debug("+ [%s:%d]\n", __FUNCTION__, __LINE__);
    if ((NULL == m_vbcastHandle) || (pOnId == NULL) || (pTsId == NULL) || (pSvcId == NULL) || (ppszCaLinkMsg == NULL))
    {
        HxLOG_Error("invalid param -[%s:%d]\n", __FUNCTION__, __LINE__);
        bplStatus = BPL_STATUS_ERROR;
        goto funcExit;
    }

    hErr = APK_CAS_BC_GetCaLinkMsg(m_vbcastHandle->ulViewId, pOnId, pTsId, pSvcId, ppszCaLinkMsg);
    if(hErr == ERR_OK)
    {
        bplStatus = BPL_STATUS_OK;
    }
    else
    {
        bplStatus = BPL_STATUS_ERROR;
    }
funcExit:
    HxLOG_Debug("-[%s:%d]\n", __FUNCTION__, __LINE__);
    return bplStatus;
#else
    return BPL_STATUS_OK;
#endif
}

bool OplVBroadcast::getFullScreen(void)
{
    if ( m_vbcastHandle == NULL )
    {
        HxLOG_Error("The handle of broadcast player is NULL.\n");
        return BPL_STATUS_ERROR;
    }
    return m_vbcastHandle->bFullScreen == TRUE;
}

bool OplVBroadcast::setFullScreen(BPL_BOOL bFullScreen)
{
    HERROR hErr = ERR_FAIL;
    if ( m_vbcastHandle == NULL )
    {
        HxLOG_Error("The handle of broadcast player is NULL.\n");
        return false;
    }

    if ( bFullScreen == TRUE )
    {
        // Width/Height 0 may trigger full screen from OCTO size
        hErr = APK_MEDIA_PLAY_SetVideoSize(m_vbcastHandle->ulViewId, 0, 0, 0, 0);
        m_vbcastHandle->bFullScreen = bFullScreen;
    }
    else
    {
        hErr = APK_MEDIA_PLAY_SetVideoSize(m_vbcastHandle->ulViewId,
                                            m_vbcastHandle->nLastWindowPositionX,
                                            m_vbcastHandle->nLastWindowPositionY,
                                            m_vbcastHandle->nLastWindowPositionWidth,
                                            m_vbcastHandle->nLastWindowPositionHeight );
    }

    if( hErr == ERR_OK )
    {
        m_vbcastHandle->bFullScreen = bFullScreen;
    }
    return hErr == ERR_OK ? true : false;
}

bool OplVBroadcast::bindToCurrentChannel(void)
{
    DxService_t                  stService;
    HERROR                        hErr;

    if (NULL == m_vbcastHandle)
    {
        HxLOG_Error("hBcast NULL\n");
        return false;
    }

    OxMediaPlay_MediaType_e eType;
    APK_MEDIA_PLAY_GetPlayType(m_vbcastHandle->ulViewId, &eType);

    switch (eType)
    {
    case eOxMP_MEDIATYPE_LIVE:
        /* this case is to avoid when js code call vb.stop() and as soon as vb.bindtoCurrentChannel() */
        if ( m_vbcastHandle->ePlayState == OPLBROADCAST_PLAY_STATE_CONNECTING ||
                m_vbcastHandle->ePlayState == OPLBROADCAST_PLAY_STATE_PRESENTING )
        {
            HxLOG_Error("OPL_Broadcast_BindToCurrentChannel Error invalid playState (hBcast->ePlayState:0x%X)\n", m_vbcastHandle->ePlayState);
            break;
        }
    case eOxMP_MEDIATYPE_NONE:
    default:
        hErr = opl_broadcast_getLastService (m_vbcastHandle->ulViewId, &stService);
        if (BPL_STATUS_OK != hErr)
        {
            HxLOG_Error("opl_broadcast_getCurrentMasterService Error\n");
            return false;
        }

        /* The last channel is invalid when the conflict by scheduled record */
        HBOOL tunable = FALSE;
        hErr = opl_broadcast_CheckChannelChangable(m_vbcastHandle, (OPL_Channel_t)&stService, &tunable);
        if ( hErr == ERR_OK && tunable )
        {
            hErr = opl_broadcast_BindChannel (m_vbcastHandle, stService.uid, 0);
            if (BPL_STATUS_OK != hErr)
            {
                HxLOG_Error("opl_broadcast_BindChannel Error\n");
                return false;
            }
        }
        else
        {
            OPL_Channel_t channel = opl_broadcast_getChangbleChannel_inTheRecording(m_vbcastHandle);
            if ( !channel )
            {
                HxLOG_Error("opl_broadcast_getChangbleChannel_inTheRecording: there is no channel in the recording\n");
                break;
            }

            /* start LIVE */
            {
                HUINT32  ulSessionId = 0;
                hErr = opl_broadcast_startLive(m_vbcastHandle->ulViewId, ((DxService_t *)channel)->uid, &ulSessionId);
                if (ERR_OK != hErr)
                {
                    HxLOG_Error ("[%s:%d] opl_broadcast_startLive err: ViewId=%d\n", __FUNCTION__, __LINE__, m_vbcastHandle->ulViewId);
                    return false;
                }
            }

        }
        break;
    }

    return true;
}

bool OplVBroadcast::prevChannel(void)
{
    HUINT32          ulSvcUid;

    if (NULL == m_vbcastHandle)
    {
        HxLOG_Error("The handle of broadcast player is NULL.\n");
        return false;
    }
#if defined(CONFIG_OP_JAPAN)
    ulSvcUid = opl_broadcast_getAvailableSvcUidByCurrOffset(m_vbcastHandle->ulViewId, -1);
#else
    ulSvcUid = opl_broadcast_getSvcUidByCurrOffset (m_vbcastHandle->ulViewId, -1);
#endif
    if (0 == ulSvcUid)
    {
        HxLOG_Error("opl_broadcast_getChannelByCurrOffset Error.\n");
        return false;
    }

    if (BPL_STATUS_OK == opl_broadcast_BindChannel(m_vbcastHandle, ulSvcUid, 0))
    {
        HUINT32 ulMainViewId = 0;
        if(APK_MEDIA_PLAY_GetMainViewId(&ulMainViewId) == ERR_OK)
        {
            if(m_vbcastHandle->ulViewId == ulMainViewId)
            {
                opl_broadcast_memorizeCurrentChannel (ulSvcUid);
            }
        }
        return BPL_STATUS_OK;
    }

    return true;
}

bool OplVBroadcast::nextChannel(void)
{
    HUINT32          ulSvcUid;

    if (NULL == m_vbcastHandle)
    {
        HxLOG_Error("The handle of broadcast player is NULL.\n");
        return false;
    }
#if defined(CONFIG_OP_JAPAN)
    ulSvcUid = opl_broadcast_getAvailableSvcUidByCurrOffset(m_vbcastHandle->ulViewId, 1);
#else
    ulSvcUid = opl_broadcast_getSvcUidByCurrOffset (m_vbcastHandle->ulViewId, 1);
#endif
    if (0 == ulSvcUid)
    {
        HxLOG_Error("opl_broadcast_getChannelByCurrOffset Error.\n");
        return false;
    }

    if (BPL_STATUS_OK == opl_broadcast_BindChannel(m_vbcastHandle, ulSvcUid, 0))
    {
        HUINT32 ulMainViewId = 0;
        if(APK_MEDIA_PLAY_GetMainViewId(&ulMainViewId) == ERR_OK)
        {
            if(m_vbcastHandle->ulViewId == ulMainViewId)
            {
                opl_broadcast_memorizeCurrentChannel (ulSvcUid);
            }
        }
        return true;
    }

    return false;
}

bool OplVBroadcast::setChannelHandle(OPL_Channel_t channel)
{
    s_chHandleOfSetchannel = (void *)channel;
    return true;
}

bool OplVBroadcast::setChannel(unsigned short usServiceID, unsigned short usTSID, unsigned short usNetID)
{
    OPL_Channel_t    hChannel;
    if ( m_vbcastHandle == NULL )
    {
        HxLOG_Error("The handle of broadcast player is NULL.\n");
        return false;
    }

    hChannel = OPL_Channel_FindChannelByTriplet(OPL_Channel_GetManager(), (int)usNetID, (int)usTSID, (int)usServiceID);
    if (hChannel == NULL)
    {
        HxLOG_Error("Cannot set channel by triplet\n");
        return false;
    }

    return OPL_Broadcast_SetChannelByChannelObject(m_vbcastHandle, hChannel) == BPL_STATUS_OK;
}

bool OplVBroadcast::stop(void)
{
    HERROR               hErr;
    if ( m_vbcastHandle == NULL )
    {
        HxLOG_Error("The handle of broadcast player is NULL.\n");
        return false;
    }

    OxMediaPlay_MediaType_e eType;
    APK_MEDIA_PLAY_GetPlayType(m_vbcastHandle->ulViewId, &eType);

    switch (eType)
    {
    case eOxMP_MEDIATYPE_LIVE:
    case eOxMP_MEDIATYPE_TSRPLAYBACK:
        hErr = APK_MEDIA_PLAY_Stop (m_vbcastHandle->ulViewId);
        break;

    default:
        hErr = ERR_OK;
        break;
    }

    if (ERR_OK != hErr)
    {
        HxLOG_Error("Stop Action Failed.\n");
    }

    switch (m_vbcastHandle->ePlayState)
    {
    case OPLBROADCAST_PLAY_STATE_CONNECTING:
    case OPLBROADCAST_PLAY_STATE_PRESENTING:
        m_vbcastHandle->ePlayState = OPLBROADCAST_PLAY_STATE_STOPPED;
        break;

    default:
        break;
    }

    return true;
}

bool OplVBroadcast::setChannel(OPL_Channel_t channel)
{
    BPLStatus       status;
    int             uid;

    if ( m_vbcastHandle == NULL )
    {
        HxLOG_Error("The handle of broadcast player is NULL.\n");
        return false;
    }

    uid    = OPL_Channel_GetUniqueID(channel);
    status = opl_broadcast_BindChannel(m_vbcastHandle, uid, 0);
    if (status == BPL_STATUS_OK)
    {
        HUINT32 ulMainViewId = 0;
        if(APK_MEDIA_PLAY_GetMainViewId(&ulMainViewId) == ERR_OK)
        {
            if(m_vbcastHandle->ulViewId == ulMainViewId)
            {
                opl_broadcast_memorizeCurrentChannel (uid);
            }
        }
        return true;
    }

    return false;
}

bool OplVBroadcast::setChannel(OPL_Channel_t channel, unsigned int tuneInfo)
{
    BPLStatus       status;
    int             uid;

    if ( m_vbcastHandle == NULL )
    {
        HxLOG_Error("The handle of broadcast player is NULL.\n");
        return false;
    }

    uid    = OPL_Channel_GetUniqueID(channel);
    HxLOG_Debug("[%d:%s] ##bglee## channel: %p, uid: %d, tuneinfo: %x\n", __LINE__, __FUNCTION__,  channel, uid, tuneInfo);
    status = opl_broadcast_BindChannel(m_vbcastHandle, uid, tuneInfo);
    if (status == BPL_STATUS_OK)
    {
        HUINT32 ulMainViewId = 0;
        if(APK_MEDIA_PLAY_GetMainViewId(&ulMainViewId) == ERR_OK)
        {
            if(m_vbcastHandle->ulViewId == ulMainViewId)
            {
                opl_broadcast_memorizeCurrentChannel (uid);
            }
        }
        return true;
    }

    return false;
}

bool OplVBroadcast::OplVBroadcast::setChannelFCC(OPL_Channel_t channel, unsigned int ulViewID)
{
    int             uid;
    if ( m_vbcastHandle == NULL )
    {
        HxLOG_Error("The handle of broadcast player is NULL.\n");
        return false;
    }

    uid = OPL_Channel_GetUniqueID(channel);
    if(uid != 0)
    {
        HUINT32 ulSessionId = 0;
        HERROR  hErr = opl_broadcast_startLiveFcc(ulViewID, uid, &ulSessionId);
        if (ERR_OK != hErr)
        {
            HxLOG_Error ("[%s:%d] opl_broadcast_startLiveFcc err: ViewId=%d\n", __FUNCTION__, __LINE__, ulViewID);
//          return BPL_STATUS_ERROR;
        }
    }

    return true;
}

bool OplVBroadcast::stopByViewID(unsigned int ulViewID)
{
    HERROR  hErr = ERR_FAIL;
    // ȣ��Ǵｿ����?����, FCC�� ���ؼ� ������ｿä�η�?����..�׷��� View ID�� 0�� �ƴ� ���� ó�� ?
    if(ulViewID == 0)
    {
        HxLOG_Error("not allow Video broad cast ID.\n");
        return false;
    }

    if ( m_vbcastHandle == NULL )
    {
        HxLOG_Error("The handle of broadcast player is NULL.\n");
        return false;
    }

    hErr = APK_MEDIA_PLAY_Stop(ulViewID);
    if (ERR_OK != hErr)
    {
        HxLOG_Error ("[%s:%d] OPL_Broadcast_StopByViewID Err. ViewId=%d\n", __FUNCTION__, __LINE__, ulViewID);
//          return BPL_STATUS_ERROR;
    }

    return true;
}

bool OplVBroadcast::setVisibility(bool bVisible)
{
    HERROR  hErr = ERR_FAIL;
    if ( m_vbcastHandle == NULL )
    {
        HxLOG_Error("The handle of broadcast player is NULL.\n");
        return false;
    }

    HxLOG_Critical("setVisibility resourceId=%d bVisible=%d\n", m_vbcastHandle->ulViewId, bVisible);
    hErr = APK_MEDIA_PLAY_SetVideoHide(m_vbcastHandle->ulViewId, !bVisible);
    if (ERR_OK != hErr)
    {
        HxLOG_Error ("[%s:%d] OPL_Broadcast_SetVisibility Err. ViewId=%d\n", __FUNCTION__, __LINE__, m_vbcastHandle->ulViewId);
        return false;
    }
    return true;
}

bool OplVBroadcast::getVolume(unsigned int *pulVolume)
{
    if ( m_vbcastHandle == NULL )
    {
        HxLOG_Error("The handle of broadcast player is NULL.\n");
        return false;
    }

    if ( pulVolume == NULL )
    {
        HxLOG_Error("The buffer is invalid.\n");
        return false;
    }

    {
        HUINT32 nVolume;

        APK_OUTPUT_AUDIO_GetVolume(&nVolume);
        *pulVolume = nVolume;
    }

    return true;
}

bool OplVBroadcast::setVolume(unsigned int ulVolume)
{
    if ( APK_OUTPUT_AUDIO_SetVolume(ulVolume) != ERR_OK )
    {
        HxLOG_Error("The handle of broadcast player is NULL.\n");
        return false;
    }
    return true;
}

bool OplVBroadcast::getMute(int *pnMute)
{
    if ( pnMute == NULL )
    {
        HxLOG_Error("Invalid Parameter\n");
        return false;
    }

    if ( APK_OUTPUT_AUDIO_GetMasterMute(pnMute) != ERR_OK )
    {
        HxLOG_Error("failed to calling function \n");
        return false;
    }

    return true;
}

bool OplVBroadcast::setMute(int nMute)
{
    if ( APK_OUTPUT_AUDIO_SetMasterMute(nMute) != ERR_OK )
    {
        return false;
    }

    return true;
}

bool OplVBroadcast::getWindow(int *pX, int *pY, int *pWidth, int *pHeight, BPL_BOOL *pbIsFullScreen)
{
    if ( m_vbcastHandle == NULL )
    {
        HxLOG_Error("The handle of broadcast player is NULL.\n");
        return false;
    }
    if(pX == NULL || pY == NULL || pWidth == NULL ||pHeight == NULL ||pbIsFullScreen == NULL)
    {
        HxLOG_Error("params are NULL.\n");
        return false;
    }
    if (ERR_OK != APK_MEDIA_PLAY_GetVideoSize(m_vbcastHandle->ulViewId, pX, pY, pWidth, pHeight))
    {
        HxLOG_Error("APK_MEDIA_PLAY_GetVideoSize ret Error!!!!\n");
        return false;
    }
    HxLOG_Print(" hBcast->bFullScreen %d [%s : %d] ", m_vbcastHandle->bFullScreen,__FUNCTION__,__LINE__);
    *pbIsFullScreen = m_vbcastHandle->bFullScreen;
    return true;
}

bool OplVBroadcast::setVideoPosition(int x, int y, int width, int height)
{
    if ( m_vbcastHandle == NULL )
    {
        HxLOG_Error("The handle of broadcast player is NULL.\n");
        return false;
    }

    if (ERR_OK != APK_MEDIA_PLAY_SetVideoSize(m_vbcastHandle->ulViewId, x, y, width, height))
    {
        HxLOG_Error("APK_MEDIA_PLAY_SetVideoSize ret Error!!!!\n");
        return false;
    }
    m_vbcastHandle->nLastWindowPositionX = x;
    m_vbcastHandle->nLastWindowPositionY = y;
    m_vbcastHandle->nLastWindowPositionWidth = width;
    m_vbcastHandle->nLastWindowPositionHeight = height;
    m_vbcastHandle->bFullScreen = FALSE;

    return true;
}

bool OplVBroadcast::tvOn(void)
{
    if (NULL == m_vbcastHandle) { return false; }
    HERROR hErr = opl_broadcast_tvOn (m_vbcastHandle);
    return (ERR_OK == hErr) ? true : false;
}

bool OplVBroadcast::tvOff(void)
{
    if (NULL == m_vbcastHandle) { return false; }
    HERROR hErr = opl_broadcast_tvOff (m_vbcastHandle);
    return (ERR_OK == hErr) ? true : false;
}

bool OplVBroadcast::getWidth (unsigned int *pulWidth)
{
    HINT32 nWidth = 0;
    HERROR hErr;

    if ((NULL != m_vbcastHandle) && (NULL != pulWidth))
    {
        hErr = APK_MEDIA_PLAY_GetVideoSize (m_vbcastHandle->ulViewId, NULL, NULL, &nWidth, NULL);
        if (ERR_OK == hErr)
        {
            *pulWidth = (HUINT32)nWidth;
            return true;
        }
    }

    return false;
}

bool OplVBroadcast::getHeight (unsigned int *pulHeight)
{
    HINT32 nHeight = 0;
    HERROR hErr;

    if ((NULL != m_vbcastHandle) && (NULL != pulHeight))
    {
        hErr = APK_MEDIA_PLAY_GetVideoSize (m_vbcastHandle->ulViewId, NULL, NULL, NULL, &nHeight);
        if (ERR_OK == hErr)
        {
            *pulHeight = (HUINT32)nHeight;
            return true;
        }
    }

    return false;
}

bool OplVBroadcast::getTimeShiftMode (unsigned int *pulTimeShiftMode)
{
    BPL_BOOL bTsr;
    if ( m_vbcastHandle == NULL )
    {
        HxLOG_Error("The handle of broadcast player is NULL.\n");
        return false;
    }

    if ( pulTimeShiftMode == NULL )
    {
        HxLOG_Error("The buffer is invalid.\n");
        return false;
    }

    APK_DB_GetInt(USERDB_RECORDING_TSR_ONOFF, (HINT32*)&bTsr);
    if (FALSE != bTsr)
    {
        *pulTimeShiftMode = 1;           // 1 : Timeshift shall use "local resource"
    }
    else
    {
        *pulTimeShiftMode = 0;           // 0 : Timeshift is turned off
    }

    return true;
}

bool OplVBroadcast::setTimeShiftMode (unsigned int ulTimeShiftMode)
{
    if ( m_vbcastHandle == NULL )
    {
        HxLOG_Error("The handle of broadcast player is NULL.\n");
        return false;
    }

    switch (ulTimeShiftMode)
    {
    case 0:
        APK_DB_SetInt(USERDB_RECORDING_TSR_ONOFF, FALSE);
        APK_MEDIA_PLAY_SetTSREnable (FALSE);
        break;

    case 1:
        APK_DB_SetInt(USERDB_RECORDING_TSR_ONOFF, TRUE);
        APK_MEDIA_PLAY_SetTSREnable (TRUE);
        break;

    default:
        HxLOG_Error ("Other mode is not implemented yet.\n");
        break;
    }

    return true;
}

bool OplVBroadcast::getCurrentTimeShiftMode (unsigned int *pulTimeShiftMode)
{
    DxService_t svcInfo;
    HERROR hErr;
    HBOOL bTSREnable;

    if ((NULL == m_vbcastHandle) || (NULL == pulTimeShiftMode))
    {
        HxLOG_Error("hBcast:0x%08x, pulMode:0x%08x\n", (HUINT32)m_vbcastHandle, (HUINT32)pulTimeShiftMode);
        return false;
    }

    *pulTimeShiftMode = 0;

    hErr = opl_broadcast_getCurrentMasterService (m_vbcastHandle->ulViewId, &svcInfo);
    if (BPL_STATUS_OK != hErr)
    {
        HxLOG_Error ("opl_broadcast_getCurrentMasterService Err.\n");
        return true;
    }

    // TSR �� �����ϰų� ���� ä���� ��ȭ���̸� Time Shift �����ϴ�.
    hErr = APK_MEDIA_PLAY_GetTSREnable( &bTSREnable );
    if( (ERR_OK == hErr) && (TRUE == bTSREnable) )
    {
        *pulTimeShiftMode = 1;       // Timeshift using "local resource"
    }

    return true;
}

bool OplVBroadcast::getPlaybackOffset (unsigned int *pulPlaybackOffset)
{
    HUINT32 ulTime = 0;
#if defined(CONFIG_TIMESTAMP_64)
    HUINT64 ullPlaybackTime = 0;
#endif
    HUINT32 ulRsvSlot = 0;
    DxService_t stSvcInfo;
    BPLStatus hErr;
    OxMediaPlay_MediaType_e eType;

    if ((NULL == m_vbcastHandle) || (NULL == pulPlaybackOffset))
    {
        HxLOG_Error("hBcast:0x%08x, pulPbOffset:0x%08x\n", (HUINT32)m_vbcastHandle, (HUINT32)pulPlaybackOffset);
        return false;
    }

    APK_MEDIA_PLAY_GetPlayType(m_vbcastHandle->ulViewId, &eType);

    switch (eType)
    {
    case eOxMP_MEDIATYPE_LIVE:
        *pulPlaybackOffset = 0;
        hErr = opl_broadcast_getCurrentMasterService (m_vbcastHandle->ulViewId, &stSvcInfo);
        if (BPL_STATUS_OK == hErr)
        {
            HBOOL bIsRecording;
            APK_MEDIA_RECORD_IsRecording (stSvcInfo.uid, &bIsRecording);
            if (TRUE == bIsRecording )
            {
                ulTime = 0;

                // TBR Recording ��  hBcast->ulRecsessionId�� ã�� �� ���? APK_MEDIA_RECORD_Start�� �̿����� �ʾұ� ����)
                // svc uid�� ���� Sama�� slot id�� ������ APK_MEDIA_RECORD_GetRecTime�� �־��ش�
                if ( ERR_OK == APK_MEDIA_RECORD_GetRsvSlotByUID(stSvcInfo.uid, &ulRsvSlot))
                {
                    if (ERR_OK == APK_MEDIA_RECORD_GetRecTime (ulRsvSlot, &ulTime))
                    {
                        *pulPlaybackOffset = ulTime - m_vbcastHandle->nTsrStarttime;
                        m_vbcastHandle->nTsrPlaytime = ulTime;
                    }
                }
                else
                {
                    HxLOG_Error("[%s] [%d] APK_MEDIA_RECORD_GetRsvSlotByUID Error:\n", __FUNCTION__, __LINE__);
                    *pulPlaybackOffset = 0;
                }
            }
            else
            {
                if (ERR_OK == APK_MEDIA_PLAY_GetTsrRecTime (m_vbcastHandle->ulViewId, &ulTime))
                {
                    *pulPlaybackOffset = ulTime;
                }
            }
        }
        break;

    case eOxMP_MEDIATYPE_TSRPLAYBACK:
        *pulPlaybackOffset = 0;

        {
            HINT32 nPlayTime = 0;
            HUINT32 nStartTime = 0;
            HBOOL bGetStartTime = FALSE;
            HBOOL bIsRecording = FALSE;

            // ���� �ֱ��� TsrStartTime �� �о� �´�.
            hErr = opl_broadcast_getCurrentMasterService (m_vbcastHandle->ulViewId, &stSvcInfo);
            if (BPL_STATUS_OK == hErr)
            {
                APK_MEDIA_RECORD_IsRecording (stSvcInfo.uid, &bIsRecording);
                if (TRUE == bIsRecording )
                {
                    if ( ERR_OK == APK_MEDIA_RECORD_GetRsvSlotByUID(stSvcInfo.uid, &ulRsvSlot))
                    {
                        if (ERR_OK == APK_MEDIA_RECORD_GetStartTime (ulRsvSlot, &ulTime))
                        {
                            HxLOG_Debug("[%s] [%d] succeeded to get Recording Start Time\n", __FUNCTION__, __LINE__);
                            m_vbcastHandle->nTsrStarttime = ulTime;
                            bGetStartTime = TRUE;
                        }
                    }
                }
            }

            if(bGetStartTime == FALSE)
            {
                if(APK_MEDIA_PLAY_GetTsrStartTime(m_vbcastHandle->ulViewId, &nStartTime) == ERR_OK)
                {
                    HxLOG_Debug("[%s][%d] [TSR_PB]hBcast->nTsrStarttime=%d, nStartTime=%d\n",
                        __FUNCTION__, __LINE__, m_vbcastHandle->nTsrStarttime, nStartTime);
                    m_vbcastHandle->nTsrStarttime = nStartTime;
                }
            }
#if defined(CONFIG_TIMESTAMP_64)
            if(APK_MEDIA_PLAY_GetPlayPosition (m_vbcastHandle->ulViewId, &ullPlaybackTime)==ERR_OK)
            {
                m_vbcastHandle->nTsrPlaytime        = (HINT32)ullPlaybackTime;
            }
#else
            if(APK_MEDIA_PLAY_GetPlayPosition (m_vbcastHandle->ulViewId, &ulTime)==ERR_OK)
            {
                m_vbcastHandle->nTsrPlaytime        = (HINT32)ulTime;
            }
#endif
            else
            {
                m_vbcastHandle->nTsrPlaytime ++;
            }

            nPlayTime = m_vbcastHandle->nTsrPlaytime - m_vbcastHandle->nTsrStarttime;
            /* exception */
            {
                HUINT32 ulDurationTime = 0;
                if(bIsRecording == TRUE)
                {
                    APK_MEDIA_RECORD_GetRecTime(ulRsvSlot, &ulDurationTime);
                }
                else
                {
#if defined(CONFIG_TIMESTAMP_64)
                    HUINT64 ullDurationTime = 0;
                    APK_MEDIA_PLAY_GetDurationTime (m_vbcastHandle->ulViewId, &ullDurationTime);
                    ulDurationTime = (HUINT32)ullDurationTime;
#else
                    APK_MEDIA_PLAY_GetDurationTime (m_vbcastHandle->ulViewId, &ulDurationTime);
#endif
                }

                if (nPlayTime > (HINT32)ulDurationTime)
                    nPlayTime = (HINT32)ulDurationTime;
                // nTsrPlaytime, nTsrStarttime �� �ٷ� �ٷ� update �� ���� �ʾ� ���� ����Ǵｿ����?�߻�
                // nPlayTime �� 0 ���� Ŀ��
                else if ( nPlayTime < 1 )
                    nPlayTime = 1;
                HxLOG_Debug("[%s][%d] [TSR_PB]ulTsrStartTime=%d, nTsrPlaytime=%d, ulDurationTime=%d, ulPosition(playtime)=%d\n",
                    __FUNCTION__, __LINE__, m_vbcastHandle->nTsrStarttime, m_vbcastHandle->nTsrPlaytime, ulDurationTime, nPlayTime);
            }

            *pulPlaybackOffset = nPlayTime;
        }
        break;

    default:
        HxLOG_Error ("[%s:%d] Current MediaPlay Type(%d) is invalid\n", __FUNCTION__, __LINE__, eType);
        *pulPlaybackOffset = 0;
    }
    return true;
}

bool OplVBroadcast::getPlayPosition (unsigned int *pulPlayPosition)
{
#if defined(CONFIG_TIMESTAMP_64)
    HUINT64 ullTime = 0;
#else
    HUINT32 ulTime = 0;
#endif
    OxMediaPlay_MediaType_e eType;

    if ((NULL == m_vbcastHandle) || (NULL == pulPlayPosition))
    {
        HxLOG_Error("hBcast:0x%08x, pulPbOffset:0x%08x\n", (HUINT32)m_vbcastHandle, (HUINT32)pulPlayPosition);
        return false;
    }

    APK_MEDIA_PLAY_GetPlayType(m_vbcastHandle->ulViewId, &eType);

    switch (eType)
    {
    case eOxMP_MEDIATYPE_LIVE:
        *pulPlayPosition = 0;
        break;

    case eOxMP_MEDIATYPE_TSRPLAYBACK:
#if defined(CONFIG_TIMESTAMP_64)
        if(APK_MEDIA_PLAY_GetPlayPosition (m_vbcastHandle->ulViewId, &ullTime)==ERR_OK)
            *pulPlayPosition = (HUINT32)(ullTime * 1000);
#else
        if(APK_MEDIA_PLAY_GetPlayPosition (m_vbcastHandle->ulViewId, &ulTime)==ERR_OK)
            *pulPlayPosition = ulTime * 1000;
#endif
        else
            *pulPlayPosition = 0;
        break;

    default:
        *pulPlayPosition = 0;
    }

    return true;
}

bool OplVBroadcast::getMaxOffset (unsigned int *pulMaxOffset)
{
    HUINT32 ulTime = 0;
    xOplBroadcastBufferingType_e eBufType = eOplBroadcastBufType_None;
    BPLStatus hErr;

    if ((NULL == m_vbcastHandle) || (NULL == pulMaxOffset))
    {
        HxLOG_Error("hBcast:0x%08x, pulPbOffset:0x%08x\n", (HUINT32)m_vbcastHandle, (HUINT32)pulMaxOffset);
        return false;
    }

    hErr = opl_broadcast_getBufferingTypeAndTime (m_vbcastHandle, &eBufType, &ulTime);
    if ((BPL_STATUS_OK != hErr) || (eOplBroadcastBufType_None == eBufType))
    {
        HxLOG_Error("No buffering...\n");
        *pulMaxOffset = 0;
        return true;
    }

    *pulMaxOffset = ulTime;
    return true;
}

bool OplVBroadcast::getStartOffset (unsigned int *pulStartOffset)
{
    HUINT32 ulTime = 0;
    BPLStatus hErr;

    if ((NULL == m_vbcastHandle) || (NULL == pulStartOffset))
    {
        HxLOG_Error("hBcast:0x%08x, pulPbOffset:0x%08x\n", (HUINT32)m_vbcastHandle, (HUINT32)pulStartOffset);
        return false;
    }

    hErr = opl_broadcast_getStartTime (m_vbcastHandle, &ulTime);
    if ((BPL_STATUS_OK != hErr))
    {
        HxLOG_Error("No buffering...\n");
        *pulStartOffset = 0;
        return true;
    }

    *pulStartOffset = ulTime;
    return true;
}

bool OplVBroadcast::seek (int nOffset, unsigned int ulReference)
{
    HBOOL bPlayBuffer = FALSE;
    HINT32 nAbsoluteOffset = 0;
    HUINT32 ulCurrOffset = 0, ulMaxOffset = 0;
    xOplBroadcastBufferingType_e eBufType = eOplBroadcastBufType_None;
    DxService_t stMasterSvc, stSuppleSvc;
    BPLStatus hErr;
    OxMediaPlay_MediaType_e eType;

    if (NULL == m_vbcastHandle)
    {
        HxLOG_Error("hBcast NULL\n");
        return false;
    }

    HxSTD_MemSet (&stMasterSvc, 0, sizeof(DxService_t));

    hErr = opl_broadcast_getCurrentMasterService (m_vbcastHandle->ulViewId, &stMasterSvc);
    if (BPL_STATUS_OK != hErr)
    {
        HxLOG_Error("Current Master Service None:\n");
        return false;
    }

    hErr = opl_broadcast_getCurrentSupplementaryService (m_vbcastHandle->ulViewId, &stSuppleSvc);
    if (BPL_STATUS_OK != hErr)
    {
        HxSTD_MemCopy (&stSuppleSvc, &stMasterSvc, sizeof(DxService_t));
    }

    hErr = opl_broadcast_getBufferingTypeAndTime (m_vbcastHandle, &eBufType, &ulMaxOffset);
    if ((BPL_STATUS_OK != hErr) || (eOplBroadcastBufType_None == eBufType))
    {
        HxLOG_Error("No buffering...\n");
        return false;
    }

    // nOffset �� ���� Position ���� ���?.������ ���� ������ �ȴ�...
    switch (ulReference)
    {
    case 1:                 // POSITION_CURRENT
        hErr = OPL_Broadcast_GetPlaybackOffset (m_vbcastHandle, &ulCurrOffset);
        if (BPL_STATUS_OK != hErr)
        {
            HxLOG_Error ("cannot get the max offset:\n");
            return false;
        }

        nAbsoluteOffset = (HINT32)ulCurrOffset + nOffset;
        break;

    case 2:                 // POSITION_END
        nAbsoluteOffset = (HINT32)ulMaxOffset - nOffset;
        break;

    case 0:                 // POSITION_START
    default:
        nAbsoluteOffset = nOffset;
        break;
    }

    if (nAbsoluteOffset < 0)                                { nAbsoluteOffset = 0; }
    else if (nAbsoluteOffset > (HINT32)ulMaxOffset)         { nAbsoluteOffset = (HINT32)ulMaxOffset; }

    // ������ ���� Absolute Offset�� MAX���� 2 �� ���� ������ Buffering Play�� �� �ϵ��� �Ѵ�.
    bPlayBuffer = ((HUINT32)nAbsoluteOffset + 2 < ulMaxOffset) ? TRUE : FALSE;

    APK_MEDIA_PLAY_GetPlayType(m_vbcastHandle->ulViewId, &eType);

    switch (eType)
    {
    case eOxMP_MEDIATYPE_LIVE:
        // ���� Live ���� : Play Buffer �� ������ ��������.
        if (TRUE == bPlayBuffer)
        {
            switch (eBufType)
            {
                case eOplBroadcastBufType_Usr:
                case eOplBroadcastBufType_Tsr:
                    {
                        HUINT32                      ulSessionId;
                        OxMediaPlay_StartInfo_t      unStart;
                        memset(&unStart, 0x00, sizeof(OxMediaPlay_StartInfo_t));
                        unStart.stTsrPb.eType           = eOxMP_MEDIATYPE_TSRPLAYBACK;
                        unStart.stTsrPb.ulMasterSvcUid  = stMasterSvc.uid;
                        unStart.stTsrPb.ulSuppleSvcUid  = stMasterSvc.uid;
                        unStart.stTsrPb.ulStartTime     = nAbsoluteOffset;

                        HERROR hErr = APK_MEDIA_PLAY_Start (m_vbcastHandle->ulViewId, &unStart, &ulSessionId);
                        if( ERR_OK != hErr )
                        {
                            HxLOG_Error("[%s:%d] APK_MEDIA_PLAY_Start Error :\n", __FUNCTION__, __LINE__ );
                            return false;
                        }
                    }
                    break;

                default:
                    return false;
            }
        }

        break;

    case eOxMP_MEDIATYPE_TSRPLAYBACK:
        if (TRUE == bPlayBuffer)
        {
            switch (eBufType)
            {
            case eOplBroadcastBufType_Tsr:
                m_vbcastHandle->nTsrPlaytime    = nAbsoluteOffset + m_vbcastHandle->nTsrStarttime;
#if defined(CONFIG_TIMESTAMP_64)
                APK_MEDIA_PLAY_SetPlayPosition (m_vbcastHandle->ulViewId, (HUINT64)m_vbcastHandle->nTsrPlaytime);
#else
                APK_MEDIA_PLAY_SetPlayPosition (m_vbcastHandle->ulViewId, m_vbcastHandle->nTsrPlaytime);
#endif
                break;
            case eOplBroadcastBufType_Usr:
                m_vbcastHandle->nTsrPlaytime    = nAbsoluteOffset;
#if defined(CONFIG_TIMESTAMP_64)
                APK_MEDIA_PLAY_SetPlayPosition (m_vbcastHandle->ulViewId, (HUINT64)m_vbcastHandle->nTsrPlaytime);
#else
                APK_MEDIA_PLAY_SetPlayPosition (m_vbcastHandle->ulViewId, m_vbcastHandle->nTsrPlaytime);
#endif
                break;

            // TSR Playback �� TSR ���� Buffering�� �����Ѵٴ� �� ��ü�� ���� �� ���
            default:
                return false;
            }
        }
        else
        {
            APK_MEDIA_PLAY_Stop (m_vbcastHandle->ulViewId);

            /* start LIVE */ // StartSupplementaryService ���� �ʿ�
            {
                HUINT32  ulSessionId = 0;
                HERROR hErr = opl_broadcast_startLive(m_vbcastHandle->ulViewId, stMasterSvc.uid, &ulSessionId);
                if (ERR_OK != hErr)
                {
                    HxLOG_Error ("[%s:%d] opl_broadcast_startLive err: ViewId=%d\n", __FUNCTION__, __LINE__, m_vbcastHandle->ulViewId);
                    return false;
                }
            }
        }

        break;

    case eOxMP_MEDIATYPE_NONE:
    default:
        if (TRUE != bPlayBuffer)
        {
            /* start LIVE */
            {
                HUINT32  ulSessionId = 0;
                HERROR hErr = opl_broadcast_startLive(m_vbcastHandle->ulViewId, stMasterSvc.uid, &ulSessionId);
                if (ERR_OK != hErr)
                {
                    HxLOG_Error ("[%s:%d] opl_broadcast_startLive err: ViewId=%d\n", __FUNCTION__, __LINE__, m_vbcastHandle->ulViewId);
                    return false;
                }
            }
        }

        break;
    }

    return true;
}

bool OplVBroadcast::stopTimeshift (void)
{
    DxService_t stMasterSvc, stSuppleSvc;
    BPLStatus hErr;
    OxMediaPlay_MediaType_e eType;

    if (NULL == m_vbcastHandle)
    {
        HxLOG_Error ("hBcast:NULL\n");
        return false;
    }

    APK_MEDIA_PLAY_GetPlayType(m_vbcastHandle->ulViewId, &eType);

    if (eOxMP_MEDIATYPE_TSRPLAYBACK != eType)
    {
        HxLOG_Error ("Current MediaType not TSRPB\n");
        return false;
    }

    HxSTD_MemSet (&stMasterSvc, 0, sizeof(DxService_t));

    hErr = opl_broadcast_getCurrentMasterService (m_vbcastHandle->ulViewId, &stMasterSvc);
    if (BPL_STATUS_OK != hErr)
    {
        HxLOG_Error("Current Master Service None:\n");
        stMasterSvc.uid = s_ulOplBroadcast_LastMasterSvcUid;
    }

    hErr = opl_broadcast_getCurrentSupplementaryService (m_vbcastHandle->ulViewId, &stSuppleSvc);
    if (BPL_STATUS_OK != hErr)
    {
        HxSTD_MemCopy (&stSuppleSvc, &stMasterSvc, sizeof(DxService_t));
    }

    if (ERR_OK != APK_MEDIA_PLAY_Stop (m_vbcastHandle->ulViewId))
    {
        HxLOG_Error ("Error in [APK_MEDIA_PLAY_Stop]\n");
        return false;
    }

    /* start LIVE */ // StartSupplementaryService ���� �ʿ�
    {
        HUINT32  ulSessionId = 0;
        HERROR hErr = opl_broadcast_startLive(m_vbcastHandle->ulViewId, stMasterSvc.uid, &ulSessionId);
        if (ERR_OK != hErr)
        {
            HxLOG_Error ("[%s:%d] opl_broadcast_startLive err: ViewId=%d\n", __FUNCTION__, __LINE__, m_vbcastHandle->ulViewId);
            return false;
        }
    }

    return true;
}

bool OplVBroadcast::recordNow(OPL_Channel_t channel, unsigned int ulDurationSec, char *recordID)
{
    DxService_t stService = {0, };
    DxService_t stMasterSvc = {0, };
    HERROR hErr;

    // tuning �� ���Ŀ��� ��ｿ���
       if((m_vbcastHandle->eSessionState == OPLBROADCAST_SESSION_STATE_STARTED) &&
       ((m_vbcastHandle->ePlayState == OPLBROADCAST_PLAY_STATE_CONNECTING) || (m_vbcastHandle->ePlayState == OPLBROADCAST_PLAY_STATE_PRESENTING)))
    {
        if (NULL != recordID)
        {
            recordID[0] = '\0';
        }

        HxSTD_MemSet (&stService, 0, sizeof(DxService_t));

        stService.uid = OPL_Channel_GetUniqueID(channel);

        hErr = opl_broadcast_getCurrentMasterService (m_vbcastHandle->ulViewId, &stMasterSvc);
        if (BPL_STATUS_OK != hErr)
        {
            HxLOG_Error("Fail to Get Current Master Service.\n");
            HxSTD_MemSet (&stMasterSvc, 0, sizeof(DxService_t));
        }

        hErr = opl_broadcast_getCurrentSupplementaryService (m_vbcastHandle->ulViewId, &stService);
        if (BPL_STATUS_OK != hErr)
        {
            HxLOG_Error("Fail to Get Current Supplement Service.\n");
            HxSTD_MemCopy (&stService, &stMasterSvc, sizeof(DxService_t));
        }

        hErr = opl_broadcast_StartRec( m_vbcastHandle, &stService, 0, ulDurationSec, eOxMR_TargetStorage_IntHdd, NULL, &(m_vbcastHandle->ulRecSessionId) , recordID);
        if (ERR_OK != hErr)
        {
            HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
            return false;
        }

        m_vbcastHandle->eRecState = OPLRECORDER_REC_STATE_REC_PRESTART;
    }
    else
    {
        HxLOG_Error ("[%s:%d] Not Record!!! eSessionState=%d ePlayState=%d\n", __FUNCTION__, __LINE__, m_vbcastHandle->eSessionState, m_vbcastHandle->ePlayState);
        return false;
    }

    return true;
}

bool OplVBroadcast::recordNowAt(OPL_Channel_t channel, unsigned int ulDurationSec, unsigned int ultargetID, char *recordID)
{
    DxService_t stService;
    HERROR hErr;

    if (NULL != recordID)
    {
        recordID[0] = '\0';
    }

    HxSTD_MemSet (&stService, 0, sizeof(DxService_t));
    stService.uid = OPL_Channel_GetUniqueID(channel);

    hErr = opl_broadcast_StartRec( m_vbcastHandle, &stService, 0, ulDurationSec, \
                        (OxMediaRec_TargetStorage_e)ultargetID, NULL, &(m_vbcastHandle->ulRecSessionId) , recordID);
    if (ERR_OK != hErr)
    {
        HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
        return false;
    }

    m_vbcastHandle->eRecState = OPLRECORDER_REC_STATE_REC_PRESTART;

    return true;
}

bool OplVBroadcast::recordDelayed(OPL_Channel_t channel, unsigned int ulStartPos, unsigned int ulDurationSec, char *recordID)
{
    DxService_t stService;
    HERROR hErr;
    HUINT32 ulMaxOffset = 0;

    if (NULL != recordID)
    {
        recordID[0] = '\0';
    }

    HxSTD_MemSet (&stService, 0, sizeof(DxService_t));
    stService.uid = OPL_Channel_GetUniqueID(channel);

    if(ulStartPos == 0)
    {
        //TSR �� ���� Offset ������....
        OPL_Broadcast_GetMaxOffset(m_vbcastHandle, &ulMaxOffset);
        // TSR Full�� �ƴ϶�ｿ�׻�?0�̴�....
        if(ulMaxOffset>=10)
        {
            // TSR �� 10�� �̻� ����ｿ���¸�?..Delayed ... Web���� ó����������...�ѹ� ��...
            ulStartPos = 1;
        }
    }

    hErr = opl_broadcast_StartRec( m_vbcastHandle, &stService, (HINT32)ulStartPos, ulDurationSec, eOxMR_TargetStorage_IntHdd, NULL, &(m_vbcastHandle->ulRecSessionId) , recordID);
    if (ERR_OK != hErr)
    {
        HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
        return false;
    }

    m_vbcastHandle->eRecState = OPLRECORDER_REC_STATE_REC_PRESTART;

    return true;
}

bool OplVBroadcast::recordDelayedAt(OPL_Channel_t channel, unsigned int ulStartPos, unsigned int ulDurationSec, unsigned int ultargetID, char *recordID)
{
    DxService_t stService;
    HERROR hErr;
    HUINT32 ulMaxOffset = 0;

    if (NULL != recordID)
    {
        recordID[0] = '\0';
    }

    HxSTD_MemSet (&stService, 0, sizeof(DxService_t));
    stService.uid = OPL_Channel_GetUniqueID(channel);

    //TSR �� ���� Offset ������....
    OPL_Broadcast_GetMaxOffset(m_vbcastHandle, &ulMaxOffset);
    if(ulStartPos == 0)
    {
        // TSR Full�� �ƴ϶�ｿ�׻�?�̴�....
        if(ulMaxOffset>=10)
        {
            // TSR �� 10�� �̻� ����ｿ���¸�?..Delayed ... Web���� ó����������...�ѹ� ��...
            ulStartPos = 1;
        }
    }

    hErr = opl_broadcast_StartRec( m_vbcastHandle, &stService, (HINT32)ulStartPos, ulDurationSec, \
                    (OxMediaRec_TargetStorage_e)ultargetID, NULL, &(m_vbcastHandle->ulRecSessionId) , recordID);
    if (ERR_OK != hErr)
    {
        HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
        return false;
    }

    m_vbcastHandle->eRecState = OPLRECORDER_REC_STATE_REC_PRESTART;

    return true;
}

bool OplVBroadcast::stopRecording(OPL_Channel_t channel)
{
    DxService_t stService;
    HERROR hErr;

    HxSTD_MemSet (&stService, 0, sizeof(DxService_t));
    stService.uid = OPL_Channel_GetUniqueID(channel);

    hErr = APK_MEDIA_RECORD_Stop (m_vbcastHandle->ulRecSessionId);
    if (ERR_OK != hErr)
    {
        HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
        return false;
    }

    return true;
}

bool OplVBroadcast::getRecordId (char *recordID)
{
    DxService_t stService;
    HERROR hErr;

    if (NULL == recordID)
    {
        HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
        return false;
    }

    if (BPL_STATUS_OK != opl_broadcast_getCurrentMasterService (m_vbcastHandle->ulViewId, &stService))
    {
        HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
        return false;
    }

    hErr = APK_MEDIA_RECORD_GetURL (m_vbcastHandle->ulRecSessionId, (HCHAR *)recordID);
    if (ERR_OK != hErr)
    {
        HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
        return false;
    }

    return true;
}

bool OplVBroadcast::getRecordingState(eOplRecorderState *peState)
{
    if ((NULL == m_vbcastHandle) || (NULL == peState))
    {
        HxLOG_Error("hBcast:0x%08x, peState:0x%08x\n", (HUINT32)m_vbcastHandle, (HUINT32)peState);
        return false;
    }

    *peState = m_vbcastHandle->eRecState;
    return true;
}

bool OplVBroadcast::getPlayState (OplBroadcastPlayState_e *peState)
{
    if ((NULL == m_vbcastHandle) || (NULL == peState))
    {
        HxLOG_Error("hBcast:0x%08x, peState:0x%08x\n", (HUINT32)m_vbcastHandle, (HUINT32)peState);
        return false;
    }

    *peState = m_vbcastHandle->ePlayState;
    return true;
}

bool OplVBroadcast::getPlaySpeed (signed int *pnSpeed)
{
    HERROR hErr;

    if ((NULL == m_vbcastHandle) || (NULL == pnSpeed))
    {
        HxLOG_Error("hBcast:0x%08x, pulSpeed:0x%08x\n", (HUINT32)m_vbcastHandle, (HUINT32)pnSpeed);
        return false;
    }

    *pnSpeed = 0;

    hErr = APK_MEDIA_PLAY_GetPlaySpeed (m_vbcastHandle->ulViewId, pnSpeed);
    if (hErr != ERR_OK)
    {
        HxLOG_Error ("APK_MEDIA_PLAY_GetPlaySpeed Err.\n");
        return false;
    }
    return true;
}

bool OplVBroadcast::setPlaySpeed (signed int nSpeed)
{
    BPLStatus hBPLErr;
    OxMediaPlay_MediaType_e eType;
	HBOOL				bStartPaused = FALSE;

    if (NULL == m_vbcastHandle)
    {
        HxLOG_Error("hBcast:0x%08x\n", (HUINT32)m_vbcastHandle);
        return false;
    }

    APK_MEDIA_PLAY_GetPlayType(m_vbcastHandle->ulViewId, &eType);

    if( eOxMP_MEDIATYPE_LIVE == eType )
    {
        // ���簡 Live ���̶�ｿFF X1 �̻��� �����̰�, �ݵ�ｿTrick���� ���� ��....
        if (nSpeed >= 100)
        {
            HxLOG_Error ("SetSpeed (%d) impossible for the Live Mode\n", nSpeed);
            return false;
        }

#if defined (CONFIG_TSR_IMMEDIATELY_PAUSE)
		if(nSpeed <= 0)
		{
			// Support Immediately Start&Pause and Start&FR
			bStartPaused = TRUE;
		}
#endif

        hBPLErr = OPL_Broadcast_SetOffset (m_vbcastHandle, 3, 2, bStartPaused, TRUE);

		if(nSpeed != 0)
		{
			// for start with FR
			bStartPaused = FALSE;
		}

        if (BPL_STATUS_OK != hBPLErr)
        {
            HxLOG_Error ("OPL_Broadcast_SetOffset Error\n");
            return false;
        }
    }

	if( bStartPaused == FALSE )
	{
	    if ( ERR_OK != APK_MEDIA_PLAY_SetPlaySpeed(m_vbcastHandle->ulViewId, nSpeed) )
	    {

	    }
	}
    return true;
}

bool OplVBroadcast::countComponent (eOplAVComponentType eType, unsigned int *pulCompNum)
{
    HERROR hErr = ERR_FAIL;

    OxMediaPlay_Component_e eApkCompType = eOxMP_COMPONENT_NONE;

    if ((NULL == m_vbcastHandle) || (NULL == pulCompNum))               { return false; }

    /* take a appkit component-type */
    eApkCompType = opl_broadcast_cvtComponentType (eType);

    /* get a count of component */
    if (eOxMP_COMPONENT_NONE != eApkCompType)
    {
        HUINT32 ulApkCompNum = 0;
        hErr = APK_MEDIA_PLAY_GetComponentNum (m_vbcastHandle->ulViewId, eApkCompType, &ulApkCompNum);
        if( ERR_OK != hErr )
        {
            HxLOG_Error("[%s:%d] APK_MEDIA_PLAY_GetComponentNum Error : \n", __FUNCTION__, __LINE__ );
            return false;
        }

        /* OK */
        *pulCompNum = ulApkCompNum;
        HxLOG_Debug("[%s:%d] Component-Type : (%d), Count : (%d)\n", __FUNCTION__, __LINE__,  eApkCompType, ulApkCompNum);
    }

    return (ERR_OK == hErr) ? true : false;
}

bool OplVBroadcast::countMultiViewComponent (unsigned int *pulCompNum)
{
#if defined( CONFIG_OP_JAPAN)
    HERROR hErr = ERR_FAIL;
    *pulCompNum = 0;
    {
        OPL_ProgrammeHandle                 programme= NULL;
        HxVector_t                      *descList = NULL;
        HINT32                          count = 0;
        APKS_EventComponentGroupDes_t       *multiViewComp = NULL;

        hErr = opl_broadcast_getCurrentProgramme( m_vbcastHandle ,&programme );
        if( hErr == ERR_OK  && programme != NULL )
        {
            hErr = OPL_Programme_GetMultiViewComponent( programme ,     descList );
            if( hErr == ERR_OK && descList != NULL )
            {
                count = HLIB_VECTOR_Length(descList);
                if(  count > 0 )
                {/* 1�� ���� ���?*/
                    multiViewComp = (APKS_EventComponentGroupDes_t  *)HLIB_VECTOR_ItemAt( descList , 0 );

                    if( multiViewComp != NULL )
                    {
                        *pulCompNum = multiViewComp->ucNumOfGroup;
                        hErr = ERR_OK;
                    }
                }
            }

            if( descList )
            {
                HLIB_VECTOR_Delete( descList );
            }
        }

        if( programme != NULL )
        {
            OPL_Programme_Delete( programme );
        }
    }

    return (ERR_OK == hErr) ? true : false;
#else
    return true;
#endif
}

bool OplVBroadcast::getComponent (eOplAVComponentType eType, unsigned int ulTrackIndex, OplAVComponent_t *pstComponent)
{
    HERROR hErr   = ERR_FAIL;
    OxMediaPlay_Component_e eApkCompType = eOxMP_COMPONENT_NONE;

    if ((NULL == m_vbcastHandle) || (NULL == pstComponent))             { return false; }

    /* take a appkit component-type */
    eApkCompType = opl_broadcast_cvtComponentType (eType);

    /* get a component by index */
    hErr = opl_broadcast_getComponent (m_vbcastHandle->ulViewId, eApkCompType, ulTrackIndex, pstComponent);
    if( ERR_OK != hErr )
    {
        HxLOG_Error("[%s:%d] opl_broadcast_getComponent Error : \n", __FUNCTION__, __LINE__ );
        return false;
    }

    return true;
}

bool OplVBroadcast::getMultiViewComponent (unsigned int ulIndex, OplMultiViewComponent_t *pstComponent)
{
#if defined( CONFIG_OP_JAPAN)
    HERROR               hErr = ERR_FAIL;
    {
        OPL_ProgrammeHandle programme= NULL;
        HxVector_t *descList = NULL;
        HINT32 i =0, j=0, k =0, compLoop =0, n =0, count = 0;
        APKS_EventComponentGroupDes_t *multiViewComp = NULL;
        HINT32 arrnComponntCount[OPLAVCOMPONENT_TYPE_MAX];
        OplAVComponent_t arrstComponents[OPLAVCOMPONENT_TYPE_MAX][OPL_MULTI_VIEW_MAX_COMPONENT];

        hErr = opl_broadcast_getCurrentProgramme( m_vbcastHandle ,&programme );
        if( hErr == ERR_OK  && programme != NULL )
        {
            hErr = OPL_Programme_GetMultiViewComponent( programme ,     descList );
            if( hErr == ERR_OK && descList != NULL )
            {
                count = HLIB_VECTOR_Length(descList);
                if(  count > 0 )
                {/* 1�� ���� ���?*/
                    multiViewComp = (APKS_EventComponentGroupDes_t  *)HLIB_VECTOR_ItemAt( descList , 0 );
                    if( (multiViewComp != NULL) && (multiViewComp->ucNumOfGroup > 0))
                    {
                        /* ���� ������Ʈ �� �ҷ��´�. */
                        for( compLoop =0; compLoop<OPLAVCOMPONENT_TYPE_MAX; compLoop++)
                        {
                            arrnComponntCount[compLoop] = 0;
                            OPL_Broadcast_CountComponent( hBroadcast , compLoop, (HUINT32*)&arrnComponntCount[compLoop] );
                            for( n =0; n < arrnComponntCount[compLoop]; n++ )
                            {
                                OPL_Broadcast_GetComponent( m_vbcastHandle, compLoop, n, &arrstComponents[compLoop][n] );
                            }
                        }

                        for( i =0; i< multiViewComp->ucNumOfGroup; i++ )
                        {
                            /* copy */
                            if( i != ulIndex )
                                continue;

                            pstComponent->index =  ulIndex;
                            HxSTD_StrNCpy( pstComponent->szName, (HCHAR*)multiViewComp->pstGroup[i].pucTextChar, OPL_MULTI_VIEW_COMPONENT_TEXT_SIZE );

                            for( j =0; j< multiViewComp->pstGroup[i].ucNumOfCaUnit; j++ )
                            {
                                for( k =0; k< multiViewComp->pstGroup[i].pstCaUnit[j].ucNumOfComponent; k++)
                                {
                                    for( compLoop =0; compLoop<OPLAVCOMPONENT_TYPE_MAX; compLoop++)
                                    {
                                        for( n =0; n < arrnComponntCount[compLoop]; n++ )
                                        {
                                            /* ������ ������ �߰��ص� */
                                            if( arrstComponents[compLoop][n].ulComponentTag  == multiViewComp->pstGroup[i].pstCaUnit[j].pucComponentTag[k] )
                                            {
                                                HxSTD_MemCopy( &(pstComponent->hComponents[pstComponent->compNum++]), &arrstComponents[compLoop][n], sizeof( OplAVComponent_t ));
                                            }
                                        }
                                    }
                                }

                                if( k < multiViewComp->pstGroup[i].pstCaUnit[j].ucNumOfComponent )
                                {
                                    break;
                                }
                            }

                            if( j < multiViewComp->pstGroup[i].ucNumOfCaUnit )
                            {
                                break;
                            }
                        }

                        if( i >= multiViewComp->ucNumOfGroup )
                        {/* not found */
                            hErr = ERR_FAIL;
                        }
                    }

                }
            }

            if( descList )
            {
                HLIB_VECTOR_Delete( descList );
            }
        }

        if( programme != NULL )
        {
            OPL_Programme_Delete( programme );
        }
    }
    return (ERR_OK == hErr) ? true : false;
#else
    return false;
#endif
}

bool OplVBroadcast::getCurrentComponent (eOplAVComponentType eType, OplAVComponent_t *pstComponent)
{
    HERROR hErr = ERR_FAIL;
    HINT32 nApkCompIndex = 0,nApkSubCompIndex = -1;
    OxMediaPlay_Component_e  eApkCompType = eOxMP_COMPONENT_NONE;

    if ((NULL == m_vbcastHandle) || (NULL == pstComponent))             { return false; }

    /* take a appkit component-type */
    eApkCompType = opl_broadcast_cvtComponentType (eType);

    /* get a current component-index */
    hErr = APK_MEDIA_PLAY_GetComponentIndex (m_vbcastHandle->ulViewId, eApkCompType, &nApkCompIndex, &nApkSubCompIndex);
    if( ERR_OK != hErr )
    {
        HxLOG_Error("[%s:%d] APK_MEDIA_PLAY_GetComponentIndex Error : \n", __FUNCTION__, __LINE__ );
        return false;
    }


#if defined(CONFIG_OP_UK_DTT)
    if(nApkSubCompIndex != -1 && eApkCompType == eOxMP_COMPONENT_AUDIO)
    {
        /* get a component by index */
        hErr = opl_broadcast_getComponent (m_vbcastHandle->ulViewId, eApkCompType, nApkSubCompIndex, pstComponent);
        if( ERR_OK != hErr )
        {
            HxLOG_Error("[%s:%d] opl_broadcast_getComponent Error : \n", __FUNCTION__, __LINE__ );
            return false;
        }
    }
    else
#endif
    {
        /* get a component by index */
        hErr = opl_broadcast_getComponent (m_vbcastHandle->ulViewId, eApkCompType, nApkCompIndex, pstComponent);
        if( ERR_OK != hErr )
        {
            HxLOG_Error("[%s:%d] opl_broadcast_getComponent Error : \n", __FUNCTION__, __LINE__ );
            return false;
        }
    }
    return true;
}

bool OplVBroadcast::setCurrentComponent (eOplAVComponentType eType, HUINT32 ulPid, HUINT8 *pszLang)
{
    HERROR hErr = ERR_FAIL;

    OxMediaPlay_Component_e eApkCompType = eOxMP_COMPONENT_NONE;
    HUINT32 ulCompNum = 0;

    if (NULL == m_vbcastHandle)                             { return false; }

    /* take a appkit component-type */
    eApkCompType = opl_broadcast_cvtComponentType (eType);

    /* get a count of component */
    hErr = APK_MEDIA_PLAY_GetComponentNum (m_vbcastHandle->ulViewId, eApkCompType, &ulCompNum);
    if( ERR_OK != hErr )
    {
        HxLOG_Error("[%s:%d] APK_MEDIA_PLAY_GetComponentNum Error: \n", __FUNCTION__, __LINE__ );
        return false;
    }

    /* find */
    {
        HUINT32 i;
        OplAVComponent_t stComponent;

        for (i=0 ; i<ulCompNum ; i++ )
        {
            hErr =  opl_broadcast_getComponent(m_vbcastHandle->ulViewId, eApkCompType, i, &stComponent);
            if( ERR_OK != hErr )
            {
                HxLOG_Error("[%s:%d] opl_broadcast_getComponent (%d) Error : \n", __FUNCTION__, __LINE__, i);
                return false;
            }

            /* compare */
            if (stComponent.ulPid == ulPid)
            {
                /* ext compare */
                if ((eOxMP_COMPONENT_AUDIO      == eApkCompType) ||
                    (eOxMP_COMPONENT_SUBTITLE   == eApkCompType) )
                {
                    if( NULL != pszLang )
                    {
                        if(HxSTD_StrNCmp((HCHAR*)stComponent.szLanguage, (HCHAR*)pszLang, 3) != 0)
                            continue;
                    }
                }

#if defined(CONFIG_OP_UK_DTT)

                if(eApkCompType == eOxMP_COMPONENT_SUBTITLE)
                {
                    HBOOL bEnabled = FALSE;

                    APK_DB_GetInt(USERDB_SUBTITLE_DISPLAY, (HINT32*)&bEnabled);
                    if(bEnabled != TRUE)
                    {
                        hErr = APK_SCENARIO_SetSubtitleSetting(TRUE);
                        if (ERR_OK != hErr)
                        {
                            HxLOG_Error(" APK_SCENARIO_SetSubtitleSetting ret Error:0x%X.\n",hErr);
                        }

                        APK_DB_SetInt(USERDB_SUBTITLE_DISPLAY, (HUINT32)TRUE);
                    }
                }
#endif
                hErr = APK_MEDIA_PLAY_SetComponentIndex (m_vbcastHandle->ulViewId, eApkCompType, i);
                if( ERR_OK != hErr )
                {
                    HxLOG_Error("[%s:%d] APK_MEDIA_PLAY_SetComponentIndex Error : \n", __FUNCTION__, __LINE__);
                    return false;
                }

                break;
            }
        }
    }
    return true;
}

bool OplVBroadcast::offComponent (eOplAVComponentType eType, HUINT32 ulP1)
{
    HERROR hErr = ERR_OK;

    OxMediaPlay_Component_e eApkCompType = eOxMP_COMPONENT_NONE;

    if (NULL == m_vbcastHandle)                             { return false; }

    /* take a appkit component-type */
    eApkCompType = opl_broadcast_cvtComponentType (eType);

    /* set */
    hErr = APK_MEDIA_PLAY_SetComponentIndex (m_vbcastHandle->ulViewId, eApkCompType, -1);

    /* ext action */
    switch (eType)
    {
        case OPLAVCOMPONENT_TYPE_SUBTITLE:
            if(ERR_OK == hErr)
            {
#if defined(CONFIG_OP_UK_DTT)
                hErr = APK_SCENARIO_SetSubtitleSetting(FALSE);
                if (ERR_OK != hErr)
                {
                    HxLOG_Error(" APK_SCENARIO_SetSubtitleSetting ret Error:0x%X.\n",hErr);
                    return false;
                }

                APK_DB_SetInt(USERDB_SUBTITLE_DISPLAY, (HUINT32)FALSE);
#endif
                HAPI_BroadcastSignal("signal:onSubtitleState", 1, "i", 0);
            }
            break;

        default:
            return false;
    }
    return true;
}

const char *OplVBroadcast::requestThumbnail(const char* url, HUINT32 time)
{
    static HCHAR stUrl[1024] = {0,};
    HCHAR *p = NULL;
    if (NULL == m_vbcastHandle)                             { return p; }

    p = (HCHAR*)THAPI_GetThumbnailURL(url, time,NULL, TRUE);
    HxLOG_Debug("[%s:%d]url[%s],time[%d], p[%s]\n",__FUNCTION__,__LINE__,url,time,p);
    if(p)
    {
        HxSTD_memset(stUrl, 0x00, sizeof(stUrl));
        HxSTD_PrintToStrN(stUrl, 1024, "file:///%s", p);
        return stUrl;
    }

    return NULL;
}

eOplPlaybackError OplVBroadcast::getErrorType(void)
{
    eOplPlaybackError errorType = OPLBROADCAST_CHANNEL_ERROR_UNDEFINED;

    if (NULL != m_vbcastHandle)
    {
        errorType = m_vbcastHandle->ePlaybackError;
    }

#if defined(CONFIG_OP_JAPAN)
    switch(errorType)
    {
        case OPLBROADCAST_CHANNEL_ERROR_EMPTY_CHANNEL:
            errorType = OPLBROADCAST_CHANNEL_ERROR_PMT_TIMEOUT;
            break;
        case OPLBROADCAST_CHANNEL_ERROR_CAS_BLOCK:
            errorType = OPLBROADCAST_CHANNEL_ERROR_ENCRYPTED_CHANNEL;
            break;
        case OPLBROADCAST_CHANNEL_ERROR_CHANNEL_NOT_SUPPORTED:
        case OPLBROADCAST_CHANNEL_ERROR_CANNOT_TUNE:
        case OPLBROADCAST_CHANNEL_ERROR_TUNER_LOCK:
        case OPLBROADCAST_CHANNEL_ERROR_PARENTAL_LOCK:
        case OPLBROADCAST_CHANNEL_ERROR_ENCRYPTED_CHANNEL:
        case OPLBROADCAST_CHANNEL_ERROR_UNKNOWN_CHANNEL:
        case OPLBROADCAST_CHANNEL_ERROR_CHANNEL_SWITCH_INTERRUPTED:
        case OPLBROADCAST_CHANNEL_ERROR_CANNOT_TUNE_DUETO_RECORD:
        case OPLBROADCAST_CHANNEL_ERROR_CANNOT_RESOLVE_IP_CHANNEL:
        case OPLBROADCAST_CHANNEL_ERROR_BANDWIDTH:
        case OPLBROADCAST_CHANNEL_ERROR_CANNOT_SUPPORT_CHANNEL_LIST:
        case OPLBROADCAST_CHANNEL_ERROR_INSUFFICIENT_RESOURCE:
        case OPLBROADCAST_CHANNEL_ERROR_NOT_FOUND_IN_TRANSPORT:
        case OPLBROADCAST_CHANNEL_ERROR_NO_ERROR:
            break;
        case OPLBROADCAST_CHANNEL_ERROR_PIN_DENIED:
            errorType = OPLBROADCAST_CHANNEL_ERROR_PARENTAL_LOCK;
            break;
        default:
            errorType = OPLBROADCAST_CHANNEL_ERROR_UNDEFINED;
            break;
    }
#endif
    return errorType;
}

bool OplVBroadcast::controlLive(bool bControl)
{
    HERROR hErr = (!bControl) ? APK_MEDIA_PLAY_MemorizeMediaState (m_vbcastHandle->ulViewId, TRUE)\
        : APK_MEDIA_PLAY_RestoreMediaState(m_vbcastHandle->ulViewId);

    return (hErr == ERR_OK ) ? true : false;
}

bool OplVBroadcast::getProgrammes (int *count, OPL_ProgrammeHandle **programmes)
{
    int i, c;

    if (m_vbcastHandle == NULL)
        return false;

#if defined( CONFIG_OP_JAPAN )
    if (m_vbcastHandle->nPFLen < 1)       /* �Ϻ��� ���� �̶�ｿ��ｿtype ������ following�� ���� �� �� �ִ�. */
#else
    if (m_vbcastHandle->nPFLen < 2)
#endif
    {
        DxService_t service;
        if (opl_broadcast_getCurrentMasterService(m_vbcastHandle->ulViewId, &service) == BPL_STATUS_OK)
        {
            OPL_METADATASEARCH_SearchPF(NULL, service.uid, (OPL_SearchPfMonitor_t)OplVBroadcast::opl_broadcast_PfMonitor, (void *)m_vbcastHandle);
            if (m_vbcastHandle->nPFLen == 0)
                return false;
            }
        }

    *programmes = (OPL_ProgrammeHandle *)HLIB_STD_MemAlloc(sizeof(OPL_ProgrammeHandle) * m_vbcastHandle->nPFLen);
    if (*programmes == NULL)
        return false;

    for (i = 0, c = 0 ; i < m_vbcastHandle->nPFLen ; i++)
    {
        (*programmes)[c] = OPL_Programme_Clone(m_vbcastHandle->ahPF[i]);
        if ((*programmes)[c])
            c++;
    }
    if (c == 0)
    {
        HLIB_STD_MemFree(*programmes);
        return false;
    }
    *count = c;
    return true;
}

bool OplVBroadcast::CheckAitSection (void)
{
    HxLOG_Trace();

    if (NULL == m_vbcastHandle)
    {
        HxLOG_Error ("The handle of broadcast player is NULL.\n");
        return false;
    }

    APK_MEDIA_PLAY_CheckAitSection(m_vbcastHandle->ulViewId);
    return true;
}

bool OplVBroadcast::checkChannelChangable (OPL_Channel_t channel, HBOOL *pbChangable)
{
    return (ERR_OK == opl_broadcast_CheckChannelChangable(m_vbcastHandle, channel, pbChangable))
                ? true : false;
}

const char* OplVBroadcast::getPlayingThumbnail (unsigned int pos)
{
    char *thumbnail = NULL;
    HUINT32 capturedTime = 0;
    if (NULL == m_vbcastHandle)
    {
        HxLOG_Error("hBcast NULL\n");
        return NULL;
    }
    thumbnail = APK_MEDIA_PLAY_GetThumbnail(m_vbcastHandle->ulViewId, pos, &capturedTime);
    return thumbnail;
}

bool OplVBroadcast::resultPincode (HBOOL permit, int *retVal)
{
    NOT_USED_PARAM(m_vbcastHandle);

    APK_MEDIA_PLAY_ResultPincode(permit);
    *retVal = 0;
    return true;
}

bool OplVBroadcast::getMaxDuration (unsigned int *pulMaxDuration)
{
    HERROR hErr;
#if defined(CONFIG_TIMESTAMP_64)
    HUINT64 ullDuration = 0;
#endif

    if ((NULL == m_vbcastHandle) || (NULL == pulMaxDuration))
    {
        HxLOG_Error("hBcast:0x%08x, pulMaxDuration:0x%08x\n", (HUINT32)m_vbcastHandle, (HUINT32)pulMaxDuration);
        return false;
    }

    *pulMaxDuration = 0;

#if defined(CONFIG_TIMESTAMP_64)
    hErr = APK_MEDIA_PLAY_GetDurationTime(m_vbcastHandle->ulViewId, &ullDuration);
    if(pulMaxDuration)
    {
        *pulMaxDuration = (HUINT32)ullDuration;
    }
#else
    hErr = APK_MEDIA_PLAY_GetDurationTime(m_vbcastHandle->ulViewId, pulMaxDuration);
#endif

    if (hErr != ERR_OK)
    {
        HxLOG_Error ("APK_MEDIA_PLAY_GetDurationTime Err.\n");
        return false;
    }
    return true;
}

bool OplVBroadcast::getTrickRestrictMode (OplBroadcastTrickRestrictMode_e *peMode)
{
    HERROR hErr;

    if ((NULL == m_vbcastHandle) || (NULL == peMode))
    {
        HxLOG_Error("hBcast:0x%08x\n", (HUINT32)m_vbcastHandle);
        return false;
    }

    hErr = APK_MEDIA_PLAY_GetTrickRestrictMode(m_vbcastHandle->ulViewId, (HUINT32*)peMode);
    if (ERR_OK != hErr)
    {
        HxLOG_Error ("APK_MEDIA_PLAY_GetTrickRestrictMode err: ulViewId(0x%04x)\n", m_vbcastHandle->ulViewId);
        return false;
    }

    return true;
}

bool OplVBroadcast::getTsrPausedPoints (HUINT32 *pulNum, OPL_Bookmark_t ***pppstBookmarks)
{
    HERROR hErr;
    HUINT32 ulNum = 0;
    OxMediaPlay_Bookmark_t *pstBookmarks = NULL;
    HUINT32 i;

    if ((NULL == m_vbcastHandle) || (NULL == pulNum) || (NULL == pppstBookmarks))
    {
        HxLOG_Error("hBcast:0x%08x\n", (HUINT32)m_vbcastHandle);
        return false;
    }

    hErr = APK_MEDIA_PLAY_GetTsrPausedPoints(m_vbcastHandle->ulViewId, &ulNum, &pstBookmarks);
    if (ERR_OK != hErr)
    {
        HxLOG_Error ("APK_MEDIA_PLAY_GetTsrPausedPoints err: ulViewId(0x%04x)\n", m_vbcastHandle->ulViewId);
        *pulNum = 0;
        *pppstBookmarks = NULL;
        return false;
    }

    if (ulNum != 0)
    {
        *pppstBookmarks = (OPL_Bookmark_t **)HLIB_STD_MemCalloc(ulNum*sizeof(OPL_Bookmark_t *));
        if (*pppstBookmarks == NULL)
        {
            *pulNum = 0;
            *pppstBookmarks = NULL;
            if (pstBookmarks != NULL)
            {
                HLIB_STD_MemFree(pstBookmarks);
            }

            return false;
        }

        *pulNum = ulNum;
        for (i=0; i<ulNum; i++)
        {
            (*pppstBookmarks)[i] = OPL_Bookmark_New(pstBookmarks[i].ulPos, pstBookmarks[i].szName);
        }
    }
    else
    {
        *pulNum = 0;
        *pppstBookmarks = NULL;
    }

    if (pstBookmarks != NULL)
    {
        HLIB_STD_MemFree(pstBookmarks);
    }

    return true;
}

bool OplVBroadcast::getTsrPaused (HBOOL *pbIsPaused)
{
    HERROR hErr;

    if ((NULL == m_vbcastHandle) || (NULL == pbIsPaused))
    {
        HxLOG_Error("hBcast:0x%08x\n", (HUINT32)m_vbcastHandle);
        return false;
    }

    hErr = APK_MEDIA_PLAY_GetTsrPaused(m_vbcastHandle->ulViewId, pbIsPaused);
    if (ERR_OK != hErr)
    {
        HxLOG_Error ("APK_MEDIA_PLAY_GetTsrPaused err: ulViewId(0x%04x)\n", m_vbcastHandle->ulViewId);
        return false;
    }

    return true;
}

//JLABS - start
bool OplVBroadcast::getEventRelayChannel(OPL_Channel_t *eventRelayChannel)
{
    OPL_Channel_t relayChannel = NULL;

    relayChannel = OPL_Channel_FindChannelByUID( NULL , m_vbcastHandle->eventRelaySvcUid );

    if (!relayChannel)
    {
        return false;
    }
    else
    {
        *eventRelayChannel = (OPL_Channel_t)OPL_Channel_Clone(relayChannel);
    }
    return true;
}

bool OplVBroadcast::getEwsChannel(OPL_Channel_t *ewsChannel)
{
    OPL_Channel_t oplEwsChannel = NULL;

    oplEwsChannel = OPL_Channel_FindChannelByUID( NULL , m_vbcastHandle->ewsSvcUid );

    if (!oplEwsChannel)
    {
        return false;
    }
    else
    {
        *ewsChannel = (OPL_Channel_t)OPL_Channel_Clone(oplEwsChannel);
    }
    return true;
}

bool OplVBroadcast::getEwsCurTuner(HINT32 *tunerId)
{
    OPL_Channel_t oplCurChannel = NULL;
    BPLStatus result = BPL_STATUS_ERROR;
    HINT32 onId = 0, tsId = 0, svcId = 0;

    oplCurChannel = OPL_Channel_FindChannelByUID( NULL , m_vbcastHandle->ewsCurSvcUid);

    if (oplCurChannel)
    {
        onId = OPL_Channel_GetOrgNetworkID(oplCurChannel);
        tsId = OPL_Channel_GetTransponderID(oplCurChannel);
        svcId = OPL_Channel_GetServiceID(oplCurChannel);

        *tunerId = -1;
        result = OPL_Tuner_GetTunerIdByTripleId(onId, tsId, svcId, tunerId);
    }

    return result == BPL_STATUS_OK ? true : false;
}

bool OplVBroadcast::getBmlStartFlag(HBOOL *pbAutoFlag)
{
    *pbAutoFlag = 0;
    NOT_USED_PARAM(m_vbcastHandle);
    return true;
}

bool OplVBroadcast::getBmlEndParam(int *pnCause, char *pcParam)
{
    *pnCause = 0;
    *pcParam = '\0';
    NOT_USED_PARAM(m_vbcastHandle);
    return true;
}

bool OplVBroadcast::showVideoMessage (int type)
{
    if ( m_vbcastHandle == NULL )
    {
        HxLOG_Error("The handle of broadcast player is NULL.\n");
        return false;
    }

    if (ERR_OK != APK_MEDIA_PLAY_SetVideoHide (m_vbcastHandle->ulViewId, TRUE))
    {
        HxLOG_Error("APK_MEDIA_PLAY_SetVideoHide ret Error!!!!\n");
        return false;
    }

    return true;
}

bool OplVBroadcast::hideVideoMessage (int type)
{
    if ( m_vbcastHandle == NULL )
    {
        HxLOG_Error("The handle of broadcast player is NULL.\n");
        return false;
    }

    if (ERR_OK != APK_MEDIA_PLAY_SetVideoHide (m_vbcastHandle->ulViewId, FALSE))
    {
        HxLOG_Error("APK_MEDIA_PLAY_SetVideoHide ret Error!!!!\n");
        return false;
    }

    return true;
}

bool OplVBroadcast::startBML ()
{
    NOT_USED_PARAM(m_vbcastHandle);
    return true;
}

bool OplVBroadcast::endBML ()
{
    NOT_USED_PARAM(m_vbcastHandle);
    return true;
}

bool OplVBroadcast::setBMLKeyControlMode (HBOOL permit, int *retVal)
{
    NOT_USED_PARAM(m_vbcastHandle);
#ifndef EMUL
//  APK_EXE_BML_Set(permit);
#endif
    *retVal = 0; // ������ ������ ���?octo �� �׾?
    return true;
}

bool OplVBroadcast::getBmlVodHandle(OPL_HANDLE *hBml)
{
    if ( m_vbcastHandle == NULL )
    {
        HxLOG_Error("The handle of broadcast player is NULL.\n");
        return false;
    }

    if( m_vbcastHandle && m_vbcastHandle->pcBMLName )
    {
        *hBml = m_vbcastHandle->pcBMLName;  // BML Hnadle��  BMLNAME���� �� ��.
        return true;
    }

    return false;
}

bool OplVBroadcast::getBmlVodName(char **appName)
{
#ifndef EMUL
    *appName = m_vbcastHandle->pcBMLName;
#endif
    return true;
}

bool OplVBroadcast::getBmlVodParam(char **param)
{
#ifndef EMUL
    *param = m_vbcastHandle->pcBMLParam;
#endif
    return true;
}

//JLABS - end

bool OplVBroadcast::getParentalRatingChangeStatus(char **ppszContentId, HBOOL *pbBlocked, unsigned int *pRating, char **ppszSystemId)
{
    if((ppszContentId == NULL) || (pbBlocked == NULL) || (pRating == NULL) ||(ppszSystemId == NULL))
    {
        return false;
    }
    *ppszContentId = NULL;
    *pbBlocked = m_vbcastHandle->stParentalRatingStatus.bBlocked;
    *pRating = m_vbcastHandle->stParentalRatingStatus.ratingValue;
#if defined(CONFIG_OP_JAPAN)
    /*ppszSystemId will be free*/
    APK_CAS_BC_MakeDrmSystemIdStr(ppszSystemId, m_vbcastHandle->stParentalRatingStatus.caSystemId);
#else
#if defined(CONFIG_OP_SES)
	if (m_vbcastHandle->stParentalRatingStatus.bBlockedByCas == TRUE)
	{
		*ppszSystemId = HLIB_STD_StrDup("BlockedByCas");
	}
	else
#endif
	{
		*ppszSystemId = HLIB_STD_StrDup("urn:dvb:casystemid:19188");
	}
#endif
    return true;
}

bool OplVBroadcast::releaseParentalRatingChangeStatus(char *pszContentId, char *pszSystemId)
{
    if(pszContentId != NULL)
        HLIB_STD_MemFree(pszContentId);
    if(pszSystemId != NULL)
        HLIB_STD_MemFree(pszSystemId);
    return true;
}

bool OplVBroadcast::getRctInfoCount(HUINT32 *totalLinkCount, HUINT32 *imgDataByteSize)
{
    if((NULL == m_vbcastHandle) || (NULL == totalLinkCount) || (NULL == imgDataByteSize))
    {
        HxLOG_Error("hBcast:0x%08x, totalLinkCount:0x%08x, imgDataByteSize:0x%08x\n", (HUINT32)m_vbcastHandle, (HUINT32)totalLinkCount, (HUINT32)imgDataByteSize);
        return false;
    }

    *totalLinkCount = m_vbcastHandle->nLinkCount;
    *imgDataByteSize = m_vbcastHandle->nImgSize;
    return true;
}

bool OplVBroadcast::getRctInfo(void **rctInfo)
{
    HERROR hErr;

    HxLOG_Debug("##### sh ##### [%s][%d] nLinkCount=[%d]\n",__FUNCTION__,__LINE__,m_vbcastHandle->nLinkCount);

    if(m_vbcastHandle->nLinkCount)
    {
        *rctInfo = (DxService_RctInfo_t *)HLIB_STD_MemAlloc(sizeof(DxService_RctInfo_t));
        ((DxService_RctInfo_t *)(*rctInfo))->pstLinkInfoList = (DxService_RctLinkInfo_t *)HLIB_STD_MemAlloc(sizeof(DxService_RctLinkInfo_t) * m_vbcastHandle->nLinkCount);
        ((DxService_RctInfo_t *)(*rctInfo))->stImageIconInfo.pucDatabyte = (HUINT8 *)HLIB_STD_MemAlloc(sizeof(HUINT8) * m_vbcastHandle->nImgSize);

        hErr = APK_MEDIA_PLAY_GetRctInfo(m_vbcastHandle->ulViewId, (DxService_RctInfo_t*)*rctInfo);
        if(hErr != ERR_OK)
        {
            HxLOG_Error ("APK_MEDIA_PLAY_GetRctInfo Err.\n");
            return false;
        }
        HxLOG_Debug("##### sh ##### [%s][%d] ulTotalLinkCnt=[%d]\n",__FUNCTION__,__LINE__,((DxService_RctInfo_t *)(*rctInfo))->ulTotalLinkCnt);
    }
    else
    {
        *rctInfo = NULL;
    }
    return true;
}

bool OplVBroadcast::releaseRctInfo(void *rctInfo)
{
    HERROR hErr = BPL_STATUS_ERROR;
    hErr = APK_MEDIA_PLAY_FreeRctInfo((DxService_RctInfo_t*)rctInfo);
    if(hErr != ERR_OK)
    {
        HxLOG_Error ("APK_MEDIA_PLAY_GetRctInfo Err.\n");
        return false;
    }
    return true;
}

bool OplVBroadcast::getRequestPincodeMsg(const char **msg)
{
    if(msg == NULL)
    {
        return false;
    }
    *msg = HLIB_STD_StrDup((HCHAR*)m_vbcastHandle->pszPincodeMsg);
    return true;
}

bool OplVBroadcast::releaseRequestPincodeMsg(const char *msg)
{
    if(msg != NULL)
        HLIB_STD_MemFree((char*)msg);
    return true;
}

bool OplVBroadcast::getNcdChangedMsg(const char **msg,int *pStarttime, int *pDuration)
{
    if(msg == NULL)
    {
        return false;
    }
    if((pStarttime == NULL) || (pDuration == NULL) )
    {
        return false;
    }

    *msg = HLIB_STD_StrDup((HCHAR*)m_vbcastHandle->pszNetworkMsg);
    *pStarttime = (int)m_vbcastHandle->nStartTimeofChange;
    *pDuration = (int)m_vbcastHandle->nASODuration;

    return true;
}

bool OplVBroadcast::releaseNcdChangedMsg(const char *msg)
{
    if(msg != NULL)
        HLIB_STD_MemFree((char *)msg);
    return true;
}

bool OplVBroadcast::getChannelChangeError (int *pnError)
{
    //return OPL_Broadcast_GetChannelChangeError (m_vbcastHandle, pnError) == BPL_STATUS_OK;
    return true;
}

bool OplVBroadcast::getTuneInfoByMheg(HUINT32 *MhegSvcUid, HUINT32 *TuneMode, void**hMhegChannel)
{
    return OPL_Broadcast_GetTuneInfoByMheg(m_vbcastHandle, MhegSvcUid,TuneMode, hMhegChannel) == BPL_STATUS_OK;
}

bool OplVBroadcast::getMhegInterruptFlag(HUINT32 *ulInterruptFlag)
{
    return OPL_Broadcast_GetMhegInterruptFlag(m_vbcastHandle, ulInterruptFlag) == BPL_STATUS_OK;
}

/******************************************************************
	class OplVBroadcast
******************************************************************/
OPL_Channel_t	OplVBroadcast::getCurrentChannel()
{
	return OPL_Broadcast_GetCurrentChannel(m_vbcastHandle);
}

OPL_Channel_t	OplVBroadcast::getLatestChannel()
{
	return OPL_Broadcast_GetLatestChannel(m_vbcastHandle);
}


int OplVBroadcast::getAVOutputsCount(void)
{
	return 0;
}

OplAVOutput *OplVBroadcast::getAVOutputByIndex(int nIndex)
{
	OplAVOutput *output = new OplAVOutput();
	if ( !output )
	{
		return NULL;
	}

	/* TODO: */

	return output;
}

OplRecorder *OplVBroadcast::getRecorder()
{
	if ( m_recorder ) return m_recorder;

	m_recorder = new OplRecorder();
	return m_recorder;
}

int OplVBroadcast::getPlaySpeedCount()
{
	int count = 0;
	int *speeds = NULL;
	if ( OPL_Broadcast_GetPlaySpeeds(m_vbcastHandle, &speeds, &count) != BPL_STATUS_OK )
	{
		return -1;
	}

	HLIB_STD_MemFree(speeds);
	return count;
}

bool OplVBroadcast::getPlaySpeeds(int **speeds_out)
{
	int count = 0;
	int *speeds = NULL;
	if ( OPL_Broadcast_GetPlaySpeeds(m_vbcastHandle, &speeds, &count) != BPL_STATUS_OK )
	{
		return false;
	}

	*speeds_out = new int[count];
	if ( !*speeds_out ) return false;

	for ( int i=0; i<count; ++i )
		(*speeds_out)[i] = speeds[i];

	HLIB_STD_MemFree(speeds);
	return true;
}
OPL_Channel_t	OplVBroadcast::createChannelObject(unsigned short  nIdType, int onid, int tsid, int sid)
{
	return OPL_Broadcast_CreateChannelObjectByTripleID(m_vbcastHandle, onid, tsid, sid);
}
#if 1 // HUMAX EXTENSION
OPL_Channel_t	OplVBroadcast::createChannelObject(unsigned short  nIdType, unsigned short usMajorChannel, unsigned short usMinorChannel)
{
	return OPL_Broadcast_CreateChannelObject(m_vbcastHandle, usMajorChannel, usMinorChannel);
}
#endif

OPL_Channel_t OplVBroadcast::OPL_Broadcast_CreateChannelObject(OplBroadcast_t *hBroadcast, unsigned short usMajorChannel, \
        unsigned short usMinorChannel)
{
    if ( hBroadcast == NULL )
    {
        HxLOG_Error("The handle of broadcast player is NULL.\n");
        return 0;
    }

    return OPL_Channel_FindChannelByNumber(OPL_Channel_GetManager(), (int)usMajorChannel, (int)usMinorChannel);
}

OPL_Channel_t OplVBroadcast::OPL_Broadcast_CreateChannelObjectByTripleID(OplBroadcast_t *hBroadcast, int onid, int tsid, int sid)
{
    if ( hBroadcast == NULL )
    {
        HxLOG_Error("The handle of broadcast player is NULL.\n");
        return 0;
    }

    return OPL_Channel_FindChannelByTriplet(OPL_Channel_GetManager(), onid, tsid, sid);
}

BPLStatus OplVBroadcast::OPL_Broadcast_SetFullScreen(OplBroadcast_t *hBroadcast, BPL_BOOL bFullScreen)
{
    BPLStatus hErr = BPL_STATUS_ERROR;
    HERROR aErr = ERR_OK;
    if ( hBroadcast == NULL )
    {
        HxLOG_Error("The handle of broadcast player is NULL.\n");
        return hErr;
    }

    if ( bFullScreen == TRUE )
    {
        // Width/Height 0 may trigger full screen from OCTO size
        aErr = APK_MEDIA_PLAY_SetVideoSize(hBroadcast->ulViewId, 0, 0, 0, 0);
        hBroadcast->bFullScreen = bFullScreen;
    }
    else
    {
        aErr = APK_MEDIA_PLAY_SetVideoSize(hBroadcast->ulViewId,
                                            hBroadcast->nLastWindowPositionX,
                                            hBroadcast->nLastWindowPositionY,
                                            hBroadcast->nLastWindowPositionWidth,
                                            hBroadcast->nLastWindowPositionHeight );
    }

    if( aErr == ERR_OK )
    {
        hBroadcast->bFullScreen = bFullScreen;
    }
    return aErr == ERR_OK ? BPL_STATUS_OK : BPL_STATUS_ERROR;
}

BPLStatus OplVBroadcast::OPL_Broadcast_GetMhegInterruptFlag(OplBroadcast_t *hBroadcast, HUINT32 *ulInterruptibleFlag)
{
    if(NULL == hBroadcast)
    {
        HxLOG_Debug("hBcast:0x%08x\n", (HUINT32)hBroadcast);
        return BPL_STATUS_ERROR;
    }

    *ulInterruptibleFlag = (HUINT32)hBroadcast->bInterruptible;

    HxLOG_Debug("## ysmoon ## [%s, %d] *InterruptibleFlag:%d \n", __FUNCTION__, __LINE__, *ulInterruptibleFlag);

    return BPL_STATUS_OK;
}

BPLStatus OplVBroadcast::OPL_Broadcast_GetTuneInfoByMheg (OplBroadcast_t *hBroadcast, HUINT32 *ulMhegSvcUid, \
        HUINT32 *ulTuneMode, void **hMhegChannel )
{
    if(NULL == hBroadcast)
    {
        HxLOG_Debug("hBcast:0x%08x\n", (HUINT32)hBroadcast);
        return BPL_STATUS_ERROR;
    }

    *ulMhegSvcUid = hBroadcast->nMhegSvcUid;
    *ulTuneMode = hBroadcast->eMhegTuneMode;
    *hMhegChannel = hBroadcast->hMhegChannel;

    HxLOG_Debug("## ysmoon ## [%s, %d] MhegSvcUid:%d, TuneMode:0x%x, hMhegChannel: %p, %p\n", \
            __FUNCTION__, __LINE__, *ulMhegSvcUid, *ulTuneMode, hBroadcast->hMhegChannel , *hMhegChannel);

    return BPL_STATUS_OK;
}

BPLStatus OplVBroadcast::OPL_Broadcast_SetChannelByChannelObject(OplBroadcast_t *hBroadcast, OPL_Channel_t channel)
{
    BPLStatus status;
    int uid;

    if ( hBroadcast == NULL )
    {
        HxLOG_Error("The handle of broadcast player is NULL.\n");
        return BPL_STATUS_ERROR;
    }

    uid    = OPL_Channel_GetUniqueID(channel);
    status = opl_broadcast_BindChannel(hBroadcast, uid, 0);
    if (status == BPL_STATUS_OK)
    {
        HUINT32 ulMainViewId = 0;
        if(APK_MEDIA_PLAY_GetMainViewId(&ulMainViewId) == ERR_OK)
        {
            if(hBroadcast->ulViewId == ulMainViewId)
            {
                opl_broadcast_memorizeCurrentChannel (uid);
            }
        }
        return BPL_STATUS_OK;
    }

    return BPL_STATUS_ERROR;
}

BPLStatus OplVBroadcast::OPL_Broadcast_GetPlaybackOffset (OplBroadcast_t *hBroadcast, unsigned int *pulPbOffset)
{
    HUINT32 ulTime = 0;
#if defined(CONFIG_TIMESTAMP_64)
    HUINT64 ullPlaybackTime = 0;
#endif
    HUINT32 ulRsvSlot = 0;
    DxService_t stSvcInfo;
    BPLStatus hErr;
    OxMediaPlay_MediaType_e eType;

    if ((NULL == hBroadcast) || (NULL == pulPbOffset))
    {
        HxLOG_Error("hBcast:0x%08x, pulPbOffset:0x%08x\n", (HUINT32)hBroadcast, (HUINT32)pulPbOffset);
        return BPL_STATUS_ERROR;
    }

    APK_MEDIA_PLAY_GetPlayType(hBroadcast->ulViewId, &eType);

    switch (eType)
    {
    case eOxMP_MEDIATYPE_LIVE:
        *pulPbOffset = 0;
        hErr = opl_broadcast_getCurrentMasterService (hBroadcast->ulViewId, &stSvcInfo);
        if (BPL_STATUS_OK == hErr)
        {
            HBOOL bIsRecording;
            APK_MEDIA_RECORD_IsRecording (stSvcInfo.uid, &bIsRecording);
            if (TRUE == bIsRecording )
            {
                ulTime = 0;

                // TBR Recording ��  hBcast->ulRecsessionId�� ã�� �� ���? APK_MEDIA_RECORD_Start�� �̿����� �ʾұ� ����)
                // svc uid�� ���� Sama�� slot id�� ������ APK_MEDIA_RECORD_GetRecTime�� �־��ش�
                if ( ERR_OK == APK_MEDIA_RECORD_GetRsvSlotByUID(stSvcInfo.uid, &ulRsvSlot))
                {
                    if (ERR_OK == APK_MEDIA_RECORD_GetRecTime (ulRsvSlot, &ulTime))
                    {
                        *pulPbOffset = ulTime - hBroadcast->nTsrStarttime;
                        hBroadcast->nTsrPlaytime = ulTime;
                    }
                }
                else
                {
                    HxLOG_Error("[%s] [%d] APK_MEDIA_RECORD_GetRsvSlotByUID Error:\n", __FUNCTION__, __LINE__);
                    *pulPbOffset = 0;
                }
            }
            else
            {
                if (ERR_OK == APK_MEDIA_PLAY_GetTsrRecTime (hBroadcast->ulViewId, &ulTime))
                {
                    *pulPbOffset = ulTime;
                }
            }
        }
        break;

    case eOxMP_MEDIATYPE_TSRPLAYBACK:
        *pulPbOffset = 0;

        {
            HINT32 nPlayTime = 0;
            HUINT32 nStartTime = 0;
            HBOOL bGetStartTime = FALSE;
            HBOOL bIsRecording = FALSE;

            // ���� �ֱ��� TsrStartTime �� �о� �´�.
            hErr = opl_broadcast_getCurrentMasterService (hBroadcast->ulViewId, &stSvcInfo);
            if (BPL_STATUS_OK == hErr)
            {
                APK_MEDIA_RECORD_IsRecording (stSvcInfo.uid, &bIsRecording);
                if (TRUE == bIsRecording )
                {
                    if ( ERR_OK == APK_MEDIA_RECORD_GetRsvSlotByUID(stSvcInfo.uid, &ulRsvSlot))
                    {
                        if (ERR_OK == APK_MEDIA_RECORD_GetStartTime (ulRsvSlot, &ulTime))
                        {
                            HxLOG_Debug("[%s] [%d] succeeded to get Recording Start Time\n", __FUNCTION__, __LINE__);
                            hBroadcast->nTsrStarttime = ulTime;
                            bGetStartTime = TRUE;
                        }
                    }
                }
            }

            if(bGetStartTime == FALSE)
            {
                if(APK_MEDIA_PLAY_GetTsrStartTime(hBroadcast->ulViewId, &nStartTime) == ERR_OK)
                {
                    HxLOG_Debug("[%s][%d] [TSR_PB]hBcast->nTsrStarttime=%d, nStartTime=%d\n",
                        __FUNCTION__, __LINE__, hBroadcast->nTsrStarttime, nStartTime);
                    hBroadcast->nTsrStarttime = nStartTime;
                }
            }
#if defined(CONFIG_TIMESTAMP_64)
            if(APK_MEDIA_PLAY_GetPlayPosition (hBroadcast->ulViewId, &ullPlaybackTime)==ERR_OK)
            {
                hBroadcast->nTsrPlaytime        = (HINT32)ullPlaybackTime;
            }
#else
            if(APK_MEDIA_PLAY_GetPlayPosition (hBroadcast->ulViewId, &ulTime)==ERR_OK)
            {
                hBroadcast->nTsrPlaytime        = (HINT32)ulTime;
            }
#endif
            else
            {
                hBroadcast->nTsrPlaytime ++;
            }

            nPlayTime = hBroadcast->nTsrPlaytime - hBroadcast->nTsrStarttime;
            /* exception */
            {
                HUINT32 ulDurationTime = 0;
                if(bIsRecording == TRUE)
                {
                    APK_MEDIA_RECORD_GetRecTime(ulRsvSlot, &ulDurationTime);
                }
                else
                {
#if defined(CONFIG_TIMESTAMP_64)
                    HUINT64 ullDurationTime = 0;
                    APK_MEDIA_PLAY_GetDurationTime (hBroadcast->ulViewId, &ullDurationTime);
                    ulDurationTime = (HUINT32)ullDurationTime;
#else
                    APK_MEDIA_PLAY_GetDurationTime (hBroadcast->ulViewId, &ulDurationTime);
#endif
                }

                if (nPlayTime > (HINT32)ulDurationTime)
                    nPlayTime = (HINT32)ulDurationTime;
                // nTsrPlaytime, nTsrStarttime �� �ٷ� �ٷ� update �� ���� �ʾ� ���� ����Ǵｿ����?�߻�
                // nPlayTime �� 0 ���� Ŀ��
                else if ( nPlayTime < 1 )
                    nPlayTime = 1;
                HxLOG_Debug("[%s][%d] [TSR_PB]ulTsrStartTime=%d, nTsrPlaytime=%d, ulDurationTime=%d, ulPosition(playtime)=%d\n",
                    __FUNCTION__, __LINE__, hBroadcast->nTsrStarttime, hBroadcast->nTsrPlaytime, ulDurationTime, nPlayTime);
            }

            *pulPbOffset = nPlayTime;
        }
        break;

    default:
        HxLOG_Error ("[%s:%d] Current MediaPlay Type(%d) is invalid\n", __FUNCTION__, __LINE__, eType);
        *pulPbOffset = 0;
    }

    return BPL_STATUS_OK;
}

BPLStatus OplVBroadcast::OPL_Broadcast_GetMaxOffset (OplBroadcast_t *hBroadcast, unsigned int *pulPbOffset)
{
    HUINT32 ulTime = 0;
    xOplBroadcastBufferingType_e eBufType = eOplBroadcastBufType_None;
    BPLStatus hErr;

    if ((NULL == hBroadcast) || (NULL == pulPbOffset))
    {
        HxLOG_Error("hBcast:0x%08x, pulPbOffset:0x%08x\n", (HUINT32)hBroadcast, (HUINT32)pulPbOffset);
        return BPL_STATUS_ERROR;
    }

    hErr = opl_broadcast_getBufferingTypeAndTime (hBroadcast, &eBufType, &ulTime);
    if ((BPL_STATUS_OK != hErr) || (eOplBroadcastBufType_None == eBufType))
    {
        HxLOG_Error("No buffering...\n");
        *pulPbOffset = 0;
        return BPL_STATUS_OK;
    }

    *pulPbOffset = ulTime;
    return BPL_STATUS_OK;
}

BPLStatus OplVBroadcast::OPL_Broadcast_SetOffset (OplBroadcast_t *hBroadcast, int nOffset, unsigned int ulReference, int bStartPaused, int bUseLatestPbTime)
{
    HBOOL bPlayBuffer = FALSE;
    HINT32 nAbsoluteOffset = 0;
    HUINT32 ulCurrOffset = 0, ulMaxOffset = 0;
    xOplBroadcastBufferingType_e eBufType = eOplBroadcastBufType_None;
    DxService_t stMasterSvc, stSuppleSvc;
    BPLStatus hErr;
    OxMediaPlay_MediaType_e eType;

    if (NULL == hBroadcast)
    {
        HxLOG_Error("hBcast NULL\n");
        return BPL_STATUS_ERROR;
    }

    HxSTD_MemSet (&stMasterSvc, 0, sizeof(DxService_t));

    hErr = opl_broadcast_getCurrentMasterService (hBroadcast->ulViewId, &stMasterSvc);
    if (BPL_STATUS_OK != hErr)
    {
        HxLOG_Error("Current Master Service None:\n");
        return BPL_STATUS_ERROR;
    }

    hErr = opl_broadcast_getCurrentSupplementaryService (hBroadcast->ulViewId, &stSuppleSvc);
    if (BPL_STATUS_OK != hErr)
    {
        HxSTD_MemCopy (&stSuppleSvc, &stMasterSvc, sizeof(DxService_t));
    }

    hErr = opl_broadcast_getBufferingTypeAndTime (hBroadcast, &eBufType, &ulMaxOffset);
    if ((BPL_STATUS_OK != hErr) || (eOplBroadcastBufType_None == eBufType))
    {
        HxLOG_Error("No buffering...\n");
        return BPL_STATUS_ERROR;
    }

    // nOffset �� ���� Position ���� ���?.������ ���� ������ �ȴ�...
    switch (ulReference)
    {
    case 1:                 // POSITION_CURRENT
        hErr = OPL_Broadcast_GetPlaybackOffset (hBroadcast, &ulCurrOffset);
        if (BPL_STATUS_OK != hErr)
        {
            HxLOG_Error ("cannot get the max offset:\n");
            return BPL_STATUS_ERROR;
        }

        nAbsoluteOffset = (HINT32)ulCurrOffset + nOffset;
        break;

    case 2:                 // POSITION_END
        nAbsoluteOffset = (HINT32)ulMaxOffset - nOffset;
        break;

    case 0:                 // POSITION_START
    default:
        nAbsoluteOffset = nOffset;
        break;
    }

    if (nAbsoluteOffset < 0)                                { nAbsoluteOffset = 0; }
    else if (nAbsoluteOffset > (HINT32)ulMaxOffset)         { nAbsoluteOffset = (HINT32)ulMaxOffset; }

    // ������ ���� Absolute Offset�� MAX���� 2 �� ���� ������ Buffering Play�� �� �ϵ��� �Ѵ�.
    bPlayBuffer = ((HUINT32)nAbsoluteOffset + 2 < ulMaxOffset) ? TRUE : FALSE;

    APK_MEDIA_PLAY_GetPlayType(hBroadcast->ulViewId, &eType);

    switch (eType)
    {
    case eOxMP_MEDIATYPE_LIVE:
        // ���� Live ���� : Play Buffer �� ������ ��������.
        if (TRUE == bPlayBuffer)
        {
            switch (eBufType)
            {
                case eOplBroadcastBufType_Usr:
                case eOplBroadcastBufType_Tsr:
                    {
                        HUINT32                      ulSessionId;
                        OxMediaPlay_StartInfo_t      unStart;
                        memset(&unStart, 0x00, sizeof(OxMediaPlay_StartInfo_t));
                        unStart.stTsrPb.eType           = eOxMP_MEDIATYPE_TSRPLAYBACK;
                        unStart.stTsrPb.ulMasterSvcUid  = stMasterSvc.uid;
                        unStart.stTsrPb.ulSuppleSvcUid  = stMasterSvc.uid;
                        unStart.stTsrPb.ulStartTime     = nAbsoluteOffset;
						unStart.stTsrPb.bStartPaused	= bStartPaused;
						unStart.stTsrPb.bUseLatestPbTime	= bUseLatestPbTime;

                        HERROR hErr = APK_MEDIA_PLAY_Start (hBroadcast->ulViewId, &unStart, &ulSessionId);
                        if( ERR_OK != hErr )
                        {
                            HxLOG_Error("[%s:%d] APK_MEDIA_PLAY_Start Error :\n", __FUNCTION__, __LINE__ );
                            return BPL_STATUS_ERROR;
                        }
                    }
                    break;

                default:
                    return BPL_STATUS_ERROR;
            }
        }

        break;

    case eOxMP_MEDIATYPE_TSRPLAYBACK:
        if (TRUE == bPlayBuffer)
        {
            switch (eBufType)
            {
            case eOplBroadcastBufType_Tsr:
                hBroadcast->nTsrPlaytime    = nAbsoluteOffset + hBroadcast->nTsrStarttime;
#if defined(CONFIG_TIMESTAMP_64)
                APK_MEDIA_PLAY_SetPlayPosition (hBroadcast->ulViewId, (HUINT64)hBroadcast->nTsrPlaytime);
#else
                APK_MEDIA_PLAY_SetPlayPosition (hBroadcast->ulViewId, hBroadcast->nTsrPlaytime);
#endif
                break;
            case eOplBroadcastBufType_Usr:
                hBroadcast->nTsrPlaytime    = nAbsoluteOffset;
#if defined(CONFIG_TIMESTAMP_64)
                APK_MEDIA_PLAY_SetPlayPosition (hBroadcast->ulViewId, (HUINT64)hBroadcast->nTsrPlaytime);
#else
                APK_MEDIA_PLAY_SetPlayPosition (hBroadcast->ulViewId, hBroadcast->nTsrPlaytime);
#endif
                break;

            // TSR Playback �� TSR ���� Buffering�� �����Ѵٴ� �� ��ü�� ���� �� ���
            default:
                return BPL_STATUS_ERROR;
            }
        }
        else
        {
            APK_MEDIA_PLAY_Stop (hBroadcast->ulViewId);

            /* start LIVE */ // StartSupplementaryService ���� �ʿ�
            {
                HUINT32  ulSessionId = 0;
                HERROR hErr = opl_broadcast_startLive(hBroadcast->ulViewId, stMasterSvc.uid, &ulSessionId);
                if (ERR_OK != hErr)
                {
                    HxLOG_Error ("[%s:%d] opl_broadcast_startLive err: ViewId=%d\n", __FUNCTION__, __LINE__, hBroadcast->ulViewId);
                    return BPL_STATUS_ERROR;
                }
            }
        }

        break;

    case eOxMP_MEDIATYPE_NONE:
    default:
        if (TRUE != bPlayBuffer)
        {
            /* start LIVE */
            {
                HUINT32  ulSessionId = 0;
                HERROR hErr = opl_broadcast_startLive(hBroadcast->ulViewId, stMasterSvc.uid, &ulSessionId);
                if (ERR_OK != hErr)
                {
                    HxLOG_Error ("[%s:%d] opl_broadcast_startLive err: ViewId=%d\n", __FUNCTION__, __LINE__, hBroadcast->ulViewId);
                    return BPL_STATUS_ERROR;
                }
            }
        }

        break;
    }

    return BPL_STATUS_OK;
}

OPL_Channel_t OplVBroadcast::OPL_Broadcast_GetCurrentChannel(OplBroadcast_t *hBroadcast)
{
    DxService_t stService;
    BPLStatus hErr;

    if ( hBroadcast == NULL )
    {
        HxLOG_Error ("The handle of broadcast player is NULL.\n");
        return 0;
    }
    hErr = opl_broadcast_getCurrentMasterService (hBroadcast->ulViewId, &stService);
    if (BPL_STATUS_OK != hErr)
    {
        HxLOG_Error ("opl_broadcast_getCurrentMasterService Err.\n");
        return 0;
    }
    return OPL_Channel_FindChannelByUID(NULL, (int)stService.uid);
}

OPL_Channel_t OplVBroadcast::OPL_Broadcast_GetLatestChannel(OplBroadcast_t *hBroadcast)
{
    HCHAR szCcid[32] = { 0, };
    OPL_ChannelManager_t chManager;
    OPL_Channel_t hChannel = NULL;
    HERROR hErr;

    chManager = OPL_Channel_GetManager();
    if (NULL == chManager)
    {
        return NULL;
    }

    hErr = APK_DB_GetStr((HCHAR *)USERDB_SVC_LAST_CHANNEL, szCcid, 32);
    if (ERR_OK == hErr)
    {
        hChannel = OPL_Channel_FindChannel (chManager, szCcid);
    }

    if (NULL == hChannel)
    {
        hChannel = OPL_Channel_FindChannelAt (chManager, 0);
        if (NULL == hChannel)
        {
            return NULL;
        }
    }
    return hChannel;
}

BPLStatus OplVBroadcast::OPL_Broadcast_GetPlaySpeeds(OplBroadcast_t *hBroadcast, int **speeds_out, int *count_out)
{
    int i = 0;
    int speeds[MAX_SPEED_NUM] = {0, };

    if ( !count_out ) return BPL_STATUS_ERROR;

    HERROR hErr = APK_MEDIA_PLAY_GetSupportedSpeeds(hBroadcast->ulViewId, (HUINT32*)count_out, MAX_SPEED_NUM, speeds);
    if ( hErr != ERR_OK ) return BPL_STATUS_ERROR;

    *speeds_out = (int*)HLIB_STD_MemCalloc(sizeof(int)*(*count_out));
    if ( !*speeds_out ) return BPL_STATUS_ERROR;

    if ( *count_out >= MAX_SPEED_NUM ) *count_out = MAX_SPEED_NUM;

    for ( i=0; i<*count_out; ++i ) {
        (*speeds_out)[i] = speeds[i]/100;
    }

    return BPL_STATUS_OK;
}

/** Private **/
void OplVBroadcast::opl_broadcast_initialize(void)
{
    HINT32 lastSvcUid;
    HCHAR szCcid[32] = { 0, };
    OPL_ChannelManager_t chManager;
    OPL_Channel_t hChannel = NULL;
    HERROR hErr;

    HxSTD_MemSet (s_szOplBroadcast_CurrSvcCcid, 0, 32);

    chManager = OPL_Channel_GetManager();
    if (NULL == chManager)
    {
        return;
    }

    //  OCTO�� �հ��� Play�ϰ� ������ OPL���� Last Channel�� �����Ѵ�.
    APK_MEDIA_PLAY_GetRequestId( 0 /*AV0 default*/, eOxMP_MEDIATYPE_LIVE, (HUINT32*)&lastSvcUid);
    if (lastSvcUid >= 0)
    {
        OPL_Channel_t   tChannel;
        tChannel = OPL_Channel_FindChannelByUID(OPL_Channel_GetManager(), (int)lastSvcUid);
        if (tChannel)
        {
            OPL_Channel_GetCCID(tChannel, szCcid, 32);
            APK_DB_SetStr(USERDB_SVC_LAST_CHANNEL, szCcid);
            opl_broadcast_memorizeLastChannelByNetwork(tChannel);
        }
    }

    hErr = APK_DB_GetStr((const HCHAR*)USERDB_SVC_LAST_CHANNEL, (HCHAR*)szCcid, 32);
    if (ERR_OK == hErr)
    {
        hChannel = OPL_Channel_FindChannel (chManager, szCcid);
    }

    if (NULL == hChannel)
    {
        hChannel = OPL_Channel_FindChannelAt (chManager, 0);
        if (NULL == hChannel)
        {
            return;
        }
    }

    OPL_Channel_GetCCID (hChannel, s_szOplBroadcast_CurrSvcCcid, 32);
    s_ulOplBroadcast_LastMasterSvcUid = s_ulOplBroadcast_LastSuppleSvcUid = OPL_Channel_GetUniqueID (hChannel);
}

void OplVBroadcast::UpdateInitialState(OplBroadcast_t* hBcast)
{
    APK_MEDIA_PLAY_GetViewState(hBcast->ulViewId, &(hBcast->eViewState));
    APK_MEDIA_PLAY_GetLockState(hBcast->ulViewId, &(hBcast->eLockState));
    if ((eOxMP_LiveViewState_NONE == hBcast->eViewState) || (eOxMP_LiveLockState_NONE == hBcast->eLockState))
    {
        hBcast->eSessionState = OPLBROADCAST_SESSION_STATE_STARTED;
        hBcast->ePlayState      = OPLBROADCAST_PLAY_STATE_CONNECTING;
    }
    else if ((eOxMP_LiveViewState_OK == hBcast->eViewState) && (eOxMP_LiveLockState_OK == hBcast->eLockState))
    {
        hBcast->eSessionState = OPLBROADCAST_SESSION_STATE_STARTED;
        hBcast->ePlayState      = OPLBROADCAST_PLAY_STATE_PRESENTING;
    }
}

void OplVBroadcast::opl_broadcast_notifyCasMessageFuncFromAppkit(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
    HxList_t *pstListItem = NULL;
    OplBroadcast_t *hBcast = NULL;
    DxCAS_MMI_SessionEventType_e eEventType = eDxCASMMI_EVENTTYPE_NONE;
    DxCAS_MmiSessionType_e  eApkSessionType = eDxCAS_SESSIONTYPE_NONE;
    Handle_t hSession;

    eEventType = (DxCAS_MMI_SessionEventType_e)ulParam1;
    eApkSessionType = (DxCAS_MmiSessionType_e)ulParam2;
    hSession = (Handle_t)ulParam3;

    switch(eEventType)
    {
        case eDxCASMMI_EVENTTYPE_DISPLAYUI:
            switch(eApkSessionType)
            {
                case eDxCAS_SESSIONTYPE_BANNER:
                    if(s_pstBcastObjList == NULL)
                    {
                        HxLOG_Error("Bcast Object List NULL\n");
                        return;
                    }
                    pstListItem = s_pstBcastObjList;

                    /* broadcast cas system message */
                    do
                    {
                        hBcast = (OplBroadcast_t *)HLIB_LIST_Data(pstListItem);

                        if((hBcast != NULL) && (hBcast->casEventListener != NULL))
                        {
                            hBcast->hCasMsgSession = hSession;
                            (*(hBcast->casEventListener))(hBcast->hVbcastHandle, OPLBROADCAST_EVENT_TYPE_CAS_MESSAGE_EVENT);
                        }

                        pstListItem = pstListItem->next;
                    }while(pstListItem != NULL);
                    break;

                default:
                    break;
            }
            break;
        case eDxCASMMI_EVENTTYPE_REQUESTSESSIONSTART:
            switch(eApkSessionType)
            {
                case eDxCAS_SESSIONTYPE_EXTENSION:
                    if(s_pstBcastObjList == NULL)
                    {
                        HxLOG_Error("Bcast Object List NULL\n");
                        return;
                    }

                    pstListItem = s_pstBcastObjList;
                    do
                    {
                        hBcast = (OplBroadcast_t *)HLIB_LIST_Data(pstListItem);

                        if((hBcast != NULL) && (hBcast->casEventListener != NULL))
                        {
                            hBcast->hCasMsgSession = hSession;
                            (*(hBcast->casEventListener))(hBcast->hVbcastHandle, OPLBROADCAST_EVENT_TYPE_CAS_UI_TRIGGERED);
                        }
                        pstListItem = pstListItem->next;
                    }while(pstListItem != NULL);
                    break;

                default:
                    break;
            }
            break;
#if 0// TODO: banner�� ���⼭ �޴µ�, banner�� �ѹ� ���� �� ������ ����ｿ������ｿ..
        case eDxCASMMI_EVENTTYPE_REQUESTSESSIONSTOP:
            break;
#endif

#if 0// TODO: �ӽ�
        case eDxCASMMI_EVENTTYPE_SESSIONSTARTED:
            s_hSessionOplBroadCast[eApkSessionType] = hSession;
            break;

#endif
        default:
            break;
    }

    return;
}

void OplVBroadcast::opl_broadcast_notifyFuncFromAppkit ( HUINT32 ulViewId, \
                    HUINT32 ulSessionId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, \
                    OxMediaPlay_NotifierData_t *pstNotifyData, void *pvUserData )
{
    HUINT32 ulParam1 = 0;
    HUINT32 ulParam2 = 0;
    HUINT32 ulParam3 = 0;

    if (NULL != pstNotifyData)
    {
#if defined(CONFIG_TIMESTAMP_64)    
        ulParam1 = (HUINT32)pstNotifyData->stEventNotify.ullParam1;
        ulParam2 = (HUINT32)pstNotifyData->stEventNotify.ullParam2;
        ulParam3 = (HUINT32)pstNotifyData->stEventNotify.ullParam3;
#else
        ulParam1 = pstNotifyData->stEventNotify.ulParam1;
        ulParam2 = pstNotifyData->stEventNotify.ulParam2;
        ulParam3 = pstNotifyData->stEventNotify.ulParam3;
#endif
    }
    switch( eType )
    {
        case eOxMP_MEDIATYPE_LIVE:
        {
            OplVBroadcast::live_proc(ulViewId, ulSessionId, eEvent, ulParam1, ulParam2, ulParam3);
            break;
        }
        case eOxMP_MEDIATYPE_TSRPLAYBACK:
        {
            // Chase�� PVR PLAYBACK���� �����ϸ�...���⵵..
            OplVBroadcast::tsr_proc(ulViewId, ulSessionId, eEvent, ulParam1, ulParam2, ulParam3);
            break;
        }
        case eOxMP_MEDIATYPE_MEDIAPLAY:
        default :
            break;
    }
}
HBOOL OplVBroadcast::opl_broadcast_eventFilter (HUINT32 ulViewId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, HBOOL *updated)
{
	*updated = FALSE;

	if ((eOxMP_MEDIATYPE_LIVE == eType) || (eOxMP_MEDIATYPE_TSRPLAYBACK == eType) || (eOxMP_MEDIATYPE_PVRPLAYBACK == eType))
	{
		*updated = TRUE;
	}
	return TRUE;
}

void OplVBroadcast::opl_broadcast_notifyMailMessage(APK_HANDLE handle, HINT32 event, const HINT32 *args, void *userdata)
{
    OplBroadcast_t *hBcast     = NULL;
    HxList_t *pstListItem;
    DxMail_t *pstSrcMail = (DxMail_t *)args, *pstMail = NULL;

    /* for warning */
    handle = handle;
    event = event;
    userdata = userdata;

    if( pstSrcMail == NULL )
    {
        HxLOG_Error ("[%s,%d] add mail is null\n", __FUNCTION__, __LINE__);
        return;
    }

    // �� Broadcast Object�� ������ �ϴ� �͵�
    for (pstListItem = s_pstBcastObjList; NULL != pstListItem; pstListItem = pstListItem->next)
    {
        hBcast = (OplBroadcast_t *)HLIB_LIST_Data (pstListItem);
        if (NULL != hBcast)
        {
            pstMail = NULL;
            pstMail = (DxMail_t *)APK_META_MAIL_Clone(pstSrcMail);
            /* ���⼭ ����ｿ������?web���� garbage collection�� �Ҹ��鼭 ���� �ȴ�. */

            if (NULL == pstMail)
            {
                HxLOG_Error ("[%s,%d] pstMail (0x%08x)\n", __FUNCTION__, __LINE__, pstMail);
            }
            else
            {
                HxLOG_Print("[%s,%d] handle : %d \n", __FUNCTION__ , __LINE__ , pstMail);
                HxLOG_Print("[%s,%d] id : %d \n", __FUNCTION__ , __LINE__, pstMail->ulId);
                HxLOG_Print("[%s,%d] type : %d \n", __FUNCTION__ , __LINE__, (HINT32)pstMail->eMailType);

                hBcast->hMail = (OPL_MailHandle)pstMail;

                if( hBcast->hVbcastHandle != NULL && hBcast->mailEventListener != NULL )
                {
                    hBcast->mailEventListener(hBcast->hVbcastHandle, OPLBROADCAST_EVENT_TYPE_MAIL_NEW_MEESAGE_EVENT);
                }
            }
        }
    }

    if( pstSrcMail )
    {
        APK_META_MAIL_Delete( pstSrcMail );
    }
}

HERROR OplVBroadcast::opl_broadcase_NotifyMhegEventFromAppkit(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    HxList_t *pstListItem= NULL;
    OplBroadcast_t *hBcast = NULL;
    HUINT32 eMhegTuneMode, ulMhegEvnet, ulInterruptible;
    HINT32 nMhegSvcUid;
    eOplBroadcastEventType eOplBroadCastEvent;

    if(s_pstBcastObjList == NULL)
    {
        HxLOG_Error("Bcast Object List NULL\n");
        return ERR_FAIL;
    }

    ulMhegEvnet = HxOBJECT_INT(apArgv[0]);

    HxLOG_Debug("ulMhegEvnet(0x%x) \n", ulMhegEvnet);

    switch(ulMhegEvnet)
    {
    case 30: // OPLBROADCAST_EVENT_TYPE_MHEG_REQUEST_TUNE;
        nMhegSvcUid = HxOBJECT_INT(apArgv[1]);
        eMhegTuneMode = HxOBJECT_INT(apArgv[2]);
        eOplBroadCastEvent= OPLBROADCAST_EVENT_TYPE_MHEG_REQUEST_TUNE;

        HxLOG_Debug("nMhegSvcUid(0x%x) eMhegTuneMode(0x%x)\n", nMhegSvcUid, eMhegTuneMode);

        break;

    case 31:// OPLBROADCAST_EVENT_TYPE_MHEG_INTERRUPTIBLE
        ulInterruptible = HxOBJECT_INT(apArgv[1]);
        eOplBroadCastEvent= OPLBROADCAST_EVENT_TYPE_MHEG_INTERRUPTIBLE;

        HxLOG_Debug("ulInterruptible(0x%x) \n", ulInterruptible);

        break;

    default:
        return ERR_FAIL;
    }

    pstListItem = s_pstBcastObjList;
    do
    {
        hBcast = (OplBroadcast_t *)HLIB_LIST_Data(pstListItem);

        if((hBcast != NULL) && (hBcast->mhegEventListener != NULL))
        {
            if(eOplBroadCastEvent == OPLBROADCAST_EVENT_TYPE_MHEG_REQUEST_TUNE)
            {
                OPL_Channel_t hChannel = NULL;

                hChannel = OPL_Channel_FindChannelByUID(NULL, nMhegSvcUid);
                if(hChannel != NULL)
                {
                    hBcast->nMhegSvcUid= nMhegSvcUid;
                    hBcast->eMhegTuneMode = (OxMediaPlay_TuningMode_e) eMhegTuneMode;
                    hBcast->hMhegChannel = hChannel;

                    HxLOG_Debug(" Send OPLBROADCAST_EVENT_TYPE_MHEG_REQUEST_TUNE :: nSvcUid(0x%x) , eTuneMode(0x%x) hMhegChannel(%p)\n",
                        hBcast->nMhegSvcUid, hBcast->eMhegTuneMode, hBcast->hMhegChannel);

                    (*(hBcast->mhegEventListener))(hBcast->hVbcastHandle,eOplBroadCastEvent);
                }
            }
            else if(eOplBroadCastEvent == OPLBROADCAST_EVENT_TYPE_MHEG_INTERRUPTIBLE)
            {
                hBcast->bInterruptible= ulInterruptible;

                HxLOG_Debug(" Send OPLBROADCAST_EVENT_TYPE_MHEG_INTERRUPTIBLE :: bInterruptible(%d) \n",hBcast->bInterruptible);

                (*(hBcast->mhegEventListener))(hBcast->hVbcastHandle,eOplBroadCastEvent);

            }
            else
            {

            }
        }
        pstListItem = pstListItem->next;
    }while(pstListItem != NULL);

    return ERR_OK;
}

BPLStatus OplVBroadcast::opl_broadcast_getLastService (HUINT32 ulViewId, DxService_t *pstService)
{
    HUINT32 ulSvcUid;

    HxSTD_MemSet (pstService, 0, sizeof(DxService_t));

    ulSvcUid = opl_broadcast_getSvcUidByCurrOffset (ulViewId, 0);
    if (0 != ulSvcUid)
    {
        OPL_Channel_t hChannel = OPL_Channel_FindChannelByUID(OPL_Channel_GetManager(), (int)ulSvcUid);

        pstService->uid = ulSvcUid;
        if (NULL != hChannel)
        {
            pstService->onid  = OPL_Channel_GetOrgNetworkID (hChannel);
            pstService->tsid  = OPL_Channel_GetTransponderID (hChannel);
            pstService->svcid = OPL_Channel_GetServiceID (hChannel);
        }

        return BPL_STATUS_OK;
    }

    return BPL_STATUS_ERROR;
}

HUINT32 OplVBroadcast::opl_broadcast_getSvcUidByCurrOffset (HUINT32 ulViewId, HINT32 nOffset)
{
#if 1
{
    HINT32 nSvcIndex = 0, nChanCount = 0;
    OPL_Channel_t hChannel;
    OPL_ChannelManager_t chManager;

    chManager = OPL_Channel_GetManager();
    if (NULL == chManager)
    {
        return 0;
    }

    hChannel = OPL_Channel_FindChannel (chManager, s_szOplBroadcast_CurrSvcCcid);
    if (NULL == hChannel)
    {
        return 0;
    }

    nSvcIndex = OPL_Channel_IndexOf (chManager, hChannel);
    if (nSvcIndex < 0)
    {
        return 0;
    }

    nChanCount = OPL_Channel_Count(chManager);
    if (nChanCount < 0)
    {
        return 0;
    }

    nSvcIndex += nOffset;

    while (nSvcIndex < 0)
    {
        nSvcIndex += nChanCount;
    }

    while (nSvcIndex >= nChanCount)
    {
        nSvcIndex -= nChanCount;
    }

    hChannel = OPL_Channel_FindChannelAt (chManager, nSvcIndex);
    if (NULL == hChannel)
    {
        return 0;
    }
    return (HUINT32)OPL_Channel_GetUniqueID(hChannel);
}
#else
{
    HUINT32                  ulSvcUid = 0;
    OPL_ChannelManager_t     chManager;
    OPL_FAVLIST_t            favList;

    chManager = OPL_Channel_GetManager();
    if (NULL == chManager)
    {
        return 0;
    }

    ulSvcUid = OPL_Channel_FindChannel (chManager, s_szOplBroadcast_CurrSvcCcid);
    if (0 == ulSvcUid)
    {
        return 0;
    }

    // ���� activate�� favourite list(Channel Group)�� �������� �翊
    favList = OPL_FAVMGR_GetCurrentList();
    if (NULL == favList)
    {
        return 0;
    }
    return (HUINT32)OPL_FAVLIST_FindChannel(favList, (int)ulSvcUid, (int)nOffset);
}
#endif
}


HERROR OplVBroadcast::opl_broadcast_CheckChannelChangable (OplBroadcast_t *hBroadcast, OPL_Channel_t channel, HBOOL *pbChangable)
{
    DxService_t *pstSvc = (DxService_t *)channel;
    OxMediaPlay_StartInfo_t unStart;
    memset(&unStart, 0x00, sizeof(OxMediaPlay_StartInfo_t));

    HERROR                       hErr;

    if ((NULL == hBroadcast) || (NULL == pstSvc) || (NULL == pbChangable))
    {
        HxLOG_Error ("The handle of broadcast player is NULL.\n");
        return ERR_FAIL;
    }

    *pbChangable = FALSE;

    unStart.eType = eOxMP_MEDIATYPE_LIVE;
    unStart.stLive.ulMasterSvcUid = pstSvc->uid;
    unStart.stLive.ulSuppleSvcUid = pstSvc->uid;

    hErr = APK_MEDIA_PLAY_CheckPlayChangable (hBroadcast->ulViewId, eOxMP_MEDIATYPE_LIVE, &unStart, pbChangable);
    if (ERR_OK != hErr)
    {
        HxLOG_Error ("APK_MEDIA_PLAY_CheckPlayChangable err: ulViewId(0x%04x), svcuid(%d).\n", hBroadcast->ulViewId, pstSvc->uid);
        return ERR_FAIL;
    }
    return ERR_OK;
}

OPL_Channel_t    OplVBroadcast::opl_broadcast_getChangbleChannel_inTheRecording(OplBroadcast_t *hBcast)
{
    HBOOL bChangable          = FALSE;
    HUINT32 ulRunningRecordings = 0, i;
    OPL_Collection_t *runningRecordings  = OPL_Scheduler_GetCurrentRecordings(OPL_Scheduler_GetInstance());
    HERROR hErr = ERR_FAIL;

    if (runningRecordings)
    {
        ulRunningRecordings = OPL_Collection_Length(runningRecordings);
        for (i = 0; i < ulRunningRecordings; ++i)
        {
            OPL_ScheduledItem_t *item = (OPL_ScheduledItem_t*)OPL_Collection_ItemAt(runningRecordings, i);
            OPL_Channel_t   recChannel= OPL_Channel_FindChannel(NULL, item->channelID);
            hErr = opl_broadcast_CheckChannelChangable(hBcast, recChannel, &bChangable);
            {
                if (ERR_OK == hErr && bChangable)
                {   // can channel change
                    OPL_Collection_Delete(runningRecordings);
                    return recChannel;
                }
            }
        }
    }

    OPL_Collection_Delete(runningRecordings);
    return NULL;
}


HERROR OplVBroadcast::opl_broadcast_startLive(HUINT32 ulViewId, HUINT32 ulUID, HUINT32 *pulSessionId)
{
    HERROR hErr = ERR_FAIL;
    OPL_Channel_t pChannel = NULL;

    if( NULL == pulSessionId )
        return ERR_FAIL;

    pChannel = OPL_Channel_FindChannelByUID(NULL, ulUID);

    if( NULL != pChannel )
    {

        OxMediaPlay_StartInfo_t unStart;
        memset(&unStart, 0x00, sizeof(OxMediaPlay_StartInfo_t));

        unStart.stLive.eType = eOxMP_MEDIATYPE_LIVE;
        unStart.stLive.eLiveType = eOxMP_LIVETYPE_MAIN;
        unStart.stLive.ulMasterSvcUid = ulUID;
        unStart.stLive.ulSuppleSvcUid = ulUID;
        unStart.stLive.eSuppSvcType = eOxMP_SUPPLESVCTYPE_Normal;
        unStart.stLive.ulMajorCHNum = OPL_Channel_GetMajorChannel(pChannel);
        unStart.stLive.eTuneMode = (OxMediaPlay_TuningMode_e)0x00;

        hErr = APK_MEDIA_PLAY_Start (ulViewId, &unStart, pulSessionId);
        if (ERR_OK != hErr)
        {
            HxLOG_Error ("[%s:%d] APK_MEDIA_PLAY_Start err: ViewId=%d\n", __FUNCTION__, __LINE__, ulViewId);
        }
    }
    else
    {
        HxLOG_Error ("[%s:%d] OPL_Channel_FindChannelByUID err: ViewId=%d\n", __FUNCTION__, __LINE__, ulViewId);
    }

    return hErr;
}

BPLStatus OplVBroadcast::opl_broadcast_BindChannel (OplBroadcast_t *self, int uid, unsigned int tuneInfo)
{
    HINT32 nMasterSvcUid, nSuppleSvcUid;
    OPL_Channel_t channel = OPL_Channel_FindChannelByUID(NULL, uid);
    HERROR hErr;
    NOT_USED_PARAM(nSuppleSvcUid);

    HxLOG_Assert(channel);
    if (NULL == channel)
    {
        HxLOG_Error ("channel (0x%08x)\n", channel);
        return BPL_STATUS_ERROR;
    }

    OxMediaPlay_PlayState_e eState;
    OxMediaPlay_MediaType_e eOapiType;

    APK_MEDIA_PLAY_GetPlayType( self->ulViewId, &eOapiType );

    switch ( eOapiType )
    {
        case eOxMP_MEDIATYPE_LIVE:
        {
            APK_MEDIA_PLAY_GetRequestId ( self->ulViewId, eOxMP_MEDIATYPE_LIVE, (HUINT32*)&nMasterSvcUid);
            // ���� ä���� ���� ������ ����ｿ������?�ʴ´�.
            if ( uid == nMasterSvcUid )
            {
                /*  this case means that stop() is called at the last time.
                    but the Appkit status still has PLAYING.
                    so we must have to wait until changing the status to STOP.
                    to fix this issue. Mediactrl of Appkit shall be fixed
                 */

                APK_MEDIA_PLAY_GetPlayState( self->ulViewId, &eState );
                if ( eState == eOxMP_PLAYSTATE_PLAYING && self->ePlayState == OPLBROADCAST_PLAY_STATE_STOPPED )
                {
                    int retry_cnt = 0;
                    while( eState != eOxMP_PLAYSTATE_STOPPED && retry_cnt < 10 )
                    {
                        VK_TASK_Sleep(200);
                        APK_MEDIA_PLAY_GetPlayState( self->ulViewId, &eState );
                        retry_cnt++;
                    }
                }
                else
                {
#if defined(CONFIG_SUPPORT_MHEGPLAYER_APP)
                    if((eOxMP_TUNEMODE_FORCE_TUNE & tuneInfo) == 0)
                    {
                        HxLOG_Error ("Current Service is same...(0x%x) \n", eOxMP_TUNEMODE_FORCE_TUNE & tuneInfo);
                        return BPL_STATUS_OK;
                    }
                    else
                    {
                        HxLOG_Error ("Current Service is same...But Forced Tune (0x%x) \n", eOxMP_TUNEMODE_FORCE_TUNE & tuneInfo);
                    }
#else
                    HxLOG_Error ("Current Service is same...\n");

#if !defined(CONFIG_PRISM_ZAPPING)
                    return BPL_STATUS_OK;
#endif

#endif
                }
            }
            // Channel Zapping �ӵ��� ���� Stop�� ������ �ʴ´�.
            hErr = ERR_OK;

            break;
        }
        case eOxMP_MEDIATYPE_PVRPLAYBACK:
        case eOxMP_MEDIATYPE_TSRPLAYBACK:
        case eOxMP_MEDIATYPE_MEDIAPLAY:
        {
            hErr = APK_MEDIA_PLAY_Stop (self->ulViewId);
            break;
        }
        default:
            hErr = ERR_OK;
            break;
    }

    if (ERR_OK != hErr)
    {
        HxLOG_Error ("Cannot stop the service : ulViewId=0x%08x, SVCUID=%d\n", self->ulViewId, uid);
        return BPL_STATUS_ERROR;
    }

    opl_broadcast_disableRecNotifier (self);

    if (tuneInfo > 0)
    {
        HUINT32  ulSessionId = 0;

        hErr = opl_broadcast_startLiveByMheg(self->ulViewId, ((DxService_t *)channel)->uid, tuneInfo, &ulSessionId);
        if (ERR_OK != hErr)
        {
            HxLOG_Error ("[%s:%d] opl_broadcast_startLive err: ViewId=%d\n", __FUNCTION__, __LINE__, self->ulViewId);
            return BPL_STATUS_ERROR;
        }
    }
    else {
        HUINT32  ulSessionId = 0;

        hErr = opl_broadcast_startLive(self->ulViewId, ((DxService_t *)channel)->uid, &ulSessionId);
        if (ERR_OK != hErr)
        {
            HxLOG_Error ("[%s:%d] opl_broadcast_startLive err: ViewId=%d\n", __FUNCTION__, __LINE__, self->ulViewId);
            return BPL_STATUS_ERROR;
        }
    }


#if !defined( CONFIG_OP_JAPAN )
    /* �Ϻ����� eit  changed �޽��� ���Ŀ� �۾��Ѵ�. */
    if (self->hQueryHandle)
    {
        while (self->nPFLen--)
        {
            OPL_Programme_Delete(self->ahPF[self->nPFLen]);
        }
        self->nPFLen = 0;

        OPL_METADATASEARCH_SearchPF(self->hQueryHandle, uid, (OPL_SearchPfMonitor_t)OplVBroadcast::opl_broadcast_PfMonitor, (void *)self);
    }
#endif

    self->ePlayState = OPLBROADCAST_PLAY_STATE_CONNECTING;

    opl_broadcast_enableRecNotifier (self, (DxService_t *)channel);

#if defined(CONFIG_OP_UK_DTT)
    ulTsrMaxOffset = 0;
#endif

    return BPL_STATUS_OK;
}

HUINT32 OplVBroadcast::opl_broadcast_getAvailableSvcUidByCurrOffset (HUINT32 ulViewId, HINT32 nOffset)
{
#if defined(CONFIG_OP_JAPAN)
    HINT32 nSvcIndex = 0, nChanCount = 0, nCurrIndex;
    OPL_Channel_t hChannel, hCurrentChannel;
    OPL_ChannelManager_t chManager;
    HBOOL bPossible = FALSE;
    HINT32 tuningRange = eTuningRange_All;
    HBOOL bSeamless = TRUE;
    chManager = OPL_Channel_GetManager();
    if (NULL == chManager)
    {
        return 0;
    }

    hChannel = OPL_Channel_FindChannel (chManager, s_szOplBroadcast_CurrSvcCcid);
    if (NULL == hChannel)
    {
        return 0;
    }

    nSvcIndex = OPL_Channel_IndexOf (chManager, hChannel);
    if (nSvcIndex < 0)
    {
        return 0;
    }

    nCurrIndex = nSvcIndex;

    nChanCount = OPL_Channel_Count(chManager);
    if (nChanCount < 0)
    {
        return 0;
    }
    hCurrentChannel = hChannel;
    APK_DB_GetInt(USERDB_JLABS_TUNING_RANGE, &tuningRange);
    OPL_JlabsLocalSystem_GetSeamlessEnable(&bSeamless);

    do
    {
        nSvcIndex += nOffset;
        while (nSvcIndex < 0)
        {
            nSvcIndex += nChanCount;
        }

        while (nSvcIndex >= nChanCount)
        {
            nSvcIndex -= nChanCount;
        }

        hChannel = OPL_Channel_FindChannelAt (chManager, nSvcIndex);
        if (NULL == hChannel)
        {
            return 0;
        }
        if(bSeamless == FALSE)
        {
            if(OPL_Channel_IsSameNetwork(hCurrentChannel, hChannel) == FALSE)
            {
                bPossible = FALSE;
                continue;
            }
        }

        if (eTuningRange_All >= tuningRange && eTuningRange_TV <=tuningRange)
        {
            bPossible = OPL_Channel_IsPossibleToTuneByUpDown(hChannel, tuningRange);
        }
        else
        {
            bPossible = OPL_Channel_IsPossibleToTuneByUpDown(hChannel, eTuningRange_All);
        }
    }while((bPossible == FALSE) && (nCurrIndex != nSvcIndex));

    return (HUINT32)OPL_Channel_GetUniqueID(hChannel);
#else
    return 0;
#endif
}

BPLStatus OplVBroadcast::opl_broadcast_memorizeCurrentChannel (HUINT32 ulSvcUid)
{
    HCHAR szCcid[32] = { 0, };
    OPL_Channel_t hChannel;

    hChannel = OPL_Channel_FindChannelByUID (OPL_Channel_GetManager(), (int)ulSvcUid);
    if (NULL == hChannel)                   { return BPL_STATUS_ERROR; }

    OPL_Channel_GetCCID (hChannel, szCcid, 32);

    if ((HxSTD_StrNCmp (s_szOplBroadcast_CurrSvcCcid, szCcid, 32) != 0) &&
        ('\0' != szCcid))
    {
        HxSTD_MemCopy(s_szOplBroadcast_CurrSvcCcid, szCcid, 32);
        APK_DB_SetStr(USERDB_SVC_LAST_CHANNEL, s_szOplBroadcast_CurrSvcCcid);
        opl_broadcast_memorizeLastChannelByNetwork(hChannel);
        return BPL_STATUS_OK;
    }

    return BPL_STATUS_ERROR;
}

HERROR OplVBroadcast::opl_broadcast_tvOn (OplBroadcast_t *hBcast)
{
    HERROR hErr;
    HBOOL bChangable = FALSE;

    OxMediaPlay_MediaType_e eType;
    hErr = APK_MEDIA_PLAY_GetPlayType(hBcast->ulViewId, &eType);


    // TV Off (LIVE, TSRPB �� �ƴ� ���?�� ��쿡�ｿ����?�Ѵ�.
    // TV ���°� �ƴϰ� None�� �ƴϸ� (MediaPB, USRPB) �ϴ� �� ���� !
    switch (eType)
    {
    case eOxMP_MEDIATYPE_LIVE:
    case eOxMP_MEDIATYPE_TSRPLAYBACK:
        return ERR_OK;

    case eOxMP_MEDIATYPE_PVRPLAYBACK:
    case eOxMP_MEDIATYPE_MEDIAPLAY:
        APK_MEDIA_PLAY_Stop (hBcast->ulViewId);
        break;

    default:
        break;
    }

    HxLOG_Debug("[%s:%s:%d] Memorized : %d \n", __FILE__, __FUNCTION__, __LINE__, s_bOplBcTvMemorized);
    HxLOG_Debug("[%s:%s:%d] Uid : %d \n", __FILE__, __FUNCTION__, __LINE__, s_nOplBcTvMemorizedUid);

    if (TRUE == s_bOplBcTvMemorized)
    {
        if (s_nOplBcTvMemorizedUid > 1)
        {
            OPL_Channel_t   channel = OPL_Channel_FindChannelByUID (NULL, s_nOplBcTvMemorizedUid);

            hErr = opl_broadcast_CheckChannelChangable(hBcast, channel, &bChangable);
            if (ERR_OK == hErr && bChangable)
            {   // can channel change

                /* start LIVE */
                {
                    HUINT32  ulSessionId = 0;
                    hErr = opl_broadcast_startLive(hBcast->ulViewId, ((DxService_t *)channel)->uid, &ulSessionId);
                    if (ERR_OK != hErr)
                    {
                        HxLOG_Error ("[%s:%d] opl_broadcast_startLive err: ViewId=%d\n", __FUNCTION__, __LINE__, hBcast->ulViewId);
                        return BPL_STATUS_ERROR;
                    }
                }
            }
            else
            {   // check the recording channel
                OPL_Channel_t channel = opl_broadcast_getChangbleChannel_inTheRecording(hBcast);

                HxLOG_Print ("[%s:%d] stored channel can't changable! get the alt channel 0x(%x) in the recording channels \n"
                    , __FUNCTION__, __LINE__, channel);

                /* start LIVE */
                {
                    HUINT32  ulSessionId = 0;
                    hErr = opl_broadcast_startLive(hBcast->ulViewId, ((DxService_t *)channel)->uid, &ulSessionId);
                    if (ERR_OK != hErr)
                    {
                        HxLOG_Error ("[%s:%d] opl_broadcast_startLive err: ViewId=%d\n", __FUNCTION__, __LINE__, hBcast->ulViewId);
                        return BPL_STATUS_ERROR;
                    }
                }
            }
        }
        else
        {
            HxLOG_Error ("[%s:%d] when tvOff, there was no TV Live....\n", __FUNCTION__, __LINE__);
        }
    }
    else
    {
        DxService_t              stService;

        if (opl_broadcast_getLastService (hBcast->ulViewId, &stService) == BPL_STATUS_OK)
        {
            OPL_Channel_t   channel = (OPL_Channel_t)&stService;
            hErr = opl_broadcast_CheckChannelChangable(hBcast, channel, &bChangable);
            if (ERR_OK == hErr && bChangable)
            {
                /* start LIVE */
                {
                    HUINT32  ulSessionId = 0;
                    hErr = opl_broadcast_startLive(hBcast->ulViewId, ((DxService_t *)channel)->uid, &ulSessionId);
                    if (ERR_OK != hErr)
                    {
                        HxLOG_Error ("[%s:%d] opl_broadcast_startLive err: ViewId=%d\n", __FUNCTION__, __LINE__, hBcast->ulViewId);
                        return BPL_STATUS_ERROR;
                    }
                }
            }
            else
            {   // check the recording channel
                OPL_Channel_t channel = opl_broadcast_getChangbleChannel_inTheRecording(hBcast);
                HxLOG_Print ("[%s:%d] stored channel can't changable! get the alt channel 0x(%x) in the recording channels \n"
                    , __FUNCTION__, __LINE__, channel);

                /* start LIVE */
                {
                    HUINT32  ulSessionId = 0;
                    if ( channel )
                    {
                        hErr = opl_broadcast_startLive(hBcast->ulViewId, ((DxService_t *)channel)->uid, &ulSessionId);
                        if (ERR_OK != hErr)
                        {
                            HxLOG_Error ("[%s:%d] opl_broadcast_startLive err: ViewId=%d\n", __FUNCTION__, __LINE__, hBcast->ulViewId);
                            return BPL_STATUS_ERROR;
                        }
                    }
                }
            }
        }
        else
        {
            HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
        }
    }

    s_bOplBcTvMemorized = FALSE;
    s_nOplBcTvMemorizedUid = -1;

    return ERR_OK;
}


HERROR OplVBroadcast::opl_broadcast_tvOff (OplBroadcast_t *hBcast)
{
    HINT32 nSvcUid;
    OxMediaPlay_MediaType_e eType;

    APK_MEDIA_PLAY_GetPlayType(hBcast->ulViewId, &eType);
    // TV On (LIVE, TSRPB) �� ��쿡�ｿ����?�Ѵ�.
    switch (eType)
    {
    case eOxMP_MEDIATYPE_LIVE:
    case eOxMP_MEDIATYPE_TSRPLAYBACK:
        APK_MEDIA_PLAY_GetRequestId( hBcast->ulViewId, eOxMP_MEDIATYPE_LIVE, (HUINT32*)&nSvcUid);
        APK_MEDIA_PLAY_Stop (hBcast->ulViewId);
        break;
    default:
        nSvcUid = -1;
        break;
    }

    if ((FALSE == s_bOplBcTvMemorized) || (nSvcUid > 0))
    {
        s_bOplBcTvMemorized = TRUE;
        s_nOplBcTvMemorizedUid = nSvcUid;
    }
    HxLOG_Debug("[%s:%s:%d] Memorized : %d \n", __FILE__, __FUNCTION__, __LINE__, s_bOplBcTvMemorized);
    HxLOG_Debug("[%s:%s:%d] Uid : %d \n", __FILE__, __FUNCTION__, __LINE__, s_nOplBcTvMemorizedUid);

    return ERR_OK;
}

BPLStatus OplVBroadcast::opl_broadcast_getCurrentMasterService (HUINT32    ulViewId, DxService_t *pstService)
{
// queit tune ��ｿ�ڵ�?���� �ӽ� block.
    HERROR  hErr;
    HINT32  nSvcUid = -1;

    hErr = APK_MEDIA_PLAY_GetRequestId (ulViewId, eOxMP_MEDIATYPE_LIVE, (HUINT32*)&nSvcUid);

    if ( (ERR_OK == hErr) && (nSvcUid > 0) )
    {
        OPL_Channel_t hChannel = OPL_Channel_FindChannelByUID (OPL_Channel_GetManager(), (int)nSvcUid);

        pstService->uid = nSvcUid;
        if (NULL != hChannel)
        {
            pstService->onid  = OPL_Channel_GetOrgNetworkID (hChannel);
            pstService->tsid  = OPL_Channel_GetTransponderID (hChannel);
            pstService->svcid = OPL_Channel_GetServiceID (hChannel);
        }

        return BPL_STATUS_OK;
    }

    return BPL_STATUS_ERROR;
}

BPLStatus OplVBroadcast::opl_broadcast_getBufferingTypeAndTime (OplBroadcast_t *hBroadcast, \
        xOplBroadcastBufferingType_e *peType, HUINT32 *pulBufTime)
{
    /*
     * �� Live ä�ο��� Buffer�� ChasePlayback, TSRPlayback ���� �Ǵ�.
     */

    HUINT32 ulTime = 0;
    HUINT32 ulRsvSlot = 0;

    HERROR  hErr;
    HBOOL   bRecording;
    DxService_t stSvcInfo={0,};

    if( (NULL == pulBufTime) || (NULL == peType) )
        return BPL_STATUS_ERROR;

    *pulBufTime = 0;
    *peType     = eOplBroadcastBufType_None;

    hErr = opl_broadcast_getCurrentMasterService (hBroadcast->ulViewId, &stSvcInfo);
    if( ERR_OK != hErr )
    {
        HxLOG_Error("[%s:%d] opl_broadcast_getCurrentMasterService Error: \n", __FUNCTION__, __LINE__);
        goto EXIT_PROC;
    }

    hErr = APK_MEDIA_RECORD_IsRecording(stSvcInfo.uid ,&bRecording);
    if( ERR_OK != hErr )
    {
        HxLOG_Error("[%s:%d] APK_MEDIA_RECORD_IsRecording Error: \n", __FUNCTION__, __LINE__);
        goto EXIT_PROC;
    }

    if (TRUE == bRecording)
    {
        ulTime = 0;
      // TBR Recording ��  hBcast->ulRecsessionId�� ã�� �� ���? APK_MEDIA_RECORD_Start�� �̿����� �ʾұ� ����)
    // svc uid�� ���� Sama�� slot id�� ������ APK_MEDIA_RECORD_GetRecTime�� �־��ش�
        hErr = APK_MEDIA_RECORD_GetRsvSlotByUID(stSvcInfo.uid, &ulRsvSlot);

        if( ERR_OK != hErr )
        {
            HxLOG_Error("[%s:%d] APK_MEDIA_RECORD_GetRsvSlotByUID: \n", __FUNCTION__, __LINE__);
            goto EXIT_PROC;
        }

        hErr = APK_MEDIA_RECORD_GetRecTime (ulRsvSlot, &ulTime);

        if( ERR_OK != hErr )
        {
            HxLOG_Error("[%s:%d] APK_MEDIA_RECORD_GetRecTime Error: \n", __FUNCTION__, __LINE__);
            goto EXIT_PROC;
        }

        *peType = eOplBroadcastBufType_Usr;
    }
    else
    {
        hErr = APK_MEDIA_PLAY_GetTsrRecTime (hBroadcast->ulViewId, &ulTime);
        if( ERR_OK != hErr )
        {
            HxLOG_Error("[%s:%d] APK_MEDIA_PLAY_GetDurationTime Error: \n", __FUNCTION__, __LINE__);
            goto EXIT_PROC;
        }

        *peType = eOplBroadcastBufType_Tsr;
    }

    *pulBufTime = ulTime;


EXIT_PROC:
    return BPL_STATUS_OK;
}

BPLStatus OplVBroadcast::opl_broadcast_getStartTime (OplBroadcast_t *hBroadcast, HUINT32 *pulBufTime)
{
    if (0 != hBroadcast->nTsrStarttime)
    {
        if ( (HLIB_STD_GetSystemTick() - hBroadcast->nTsrStarttimeTick) >= 1000 )
        {
            hBroadcast->nTsrStarttime ++;
            hBroadcast->nTsrStarttimeTick   = HLIB_STD_GetSystemTick();
        }
    }
    else
    {
        HUINT32                  ulTime = 0;
            APK_MEDIA_PLAY_GetTsrStartTime (hBroadcast->ulViewId, &ulTime);
            hBroadcast->nTsrStarttime       = (HINT32)ulTime;
            hBroadcast->nTsrStarttimeTick   = HLIB_STD_GetSystemTick();
    }

    *pulBufTime = hBroadcast->nTsrStarttime;

    if (1 < hBroadcast->nTsrStarttime)
        *pulBufTime -= 1;

    return BPL_STATUS_OK;
}

BPLStatus OplVBroadcast::opl_broadcast_getCurrentSupplementaryService (HUINT32 ulViewId, DxService_t *pstService)
{
// queit tune ��ｿ�ڵ�?���� �ӽ� block.
    HINT32 nSvcUid = -1;
    OxMediaPlay_MediaType_e eType;

    APK_MEDIA_PLAY_GetPlayType (ulViewId, &eType);

    HxSTD_MemSet (pstService, 0, sizeof(DxService_t));

    switch (eType)
    {
    case eOxMP_MEDIATYPE_LIVE:
    case eOxMP_MEDIATYPE_TSRPLAYBACK:
        nSvcUid = APK_MEDIA_PLAY_GetRequestId( ulViewId, eOxMP_MEDIATYPE_LIVE, (HUINT32*)&nSvcUid);
        break;

    default:
        break;
    }

    if (nSvcUid > 0)
    {
        OPL_Channel_t hChannel = OPL_Channel_FindChannelByUID (OPL_Channel_GetManager(), (int)nSvcUid);

        pstService->uid = nSvcUid;
        if (NULL != hChannel)
        {
            pstService->onid  = OPL_Channel_GetOrgNetworkID (hChannel);
            pstService->tsid  = OPL_Channel_GetTransponderID (hChannel);
            pstService->svcid = OPL_Channel_GetServiceID (hChannel);
        }

        return BPL_STATUS_OK;
    }

    return BPL_STATUS_ERROR;
}

HERROR OplVBroadcast::opl_broadcast_StartRec(OplBroadcast_t *hBcast, DxService_t *pstService, \
        HINT32 nStartOffset, HUINT32 ulDuration, OxMediaRec_TargetStorage_e eStorageType, \
        HUINT8 *szStorageId, HUINT32 *pulSessionId, char *pszRecordID)
{
    HERROR hErr;
    HUINT32 ulCurrTime = 0, ulStartTime = 0, ulRecTime = 0;
    OxMediaRec_StartInfo_t stStart;
    HCHAR szName[256];
    HINT32 eventId = 0;

    HxLOG_Trace();

    if( (NULL == pstService) || (NULL == pulSessionId) )
        return ERR_FAIL;

    HxSTD_MemSet( &stStart, 0, sizeof(OxMediaRec_StartInfo_t) );

    stStart.ulMasterSvcUid  =    pstService->uid;
    stStart.ulSuppleSvcUid  =    pstService->uid;
    stStart.eSuppSvcType    =    eOxMP_SUPPLESVCTYPE_Normal;
    stStart.nStartOffset        =    nStartOffset;
    stStart.eStorageType    =    eStorageType;
    ulCurrTime          = HLIB_STD_GetSystemTime();

    /* Setup starttime*/
    if(nStartOffset<=0)
    {
        // Instant Rec ...
        (void)HLIB_DATETIME_ConvertUnixTimeToDateTime(ulCurrTime, &(stStart.stStartTime));
        ulStartTime = ulCurrTime;
    }
    else
    {
        // Delayed Rec ...
        OPL_Broadcast_GetMaxOffset((OplBroadcast_t *)hBcast, &ulRecTime);
        ulStartTime         = ulCurrTime - ulRecTime + nStartOffset;
        HxLOG_Debug("[%s:%d] ulRecTime(%d), nStartOffset(%d), ulStartTime(%d)\n", __FUNCTION__, __LINE__, ulRecTime, nStartOffset, ulStartTime);
        (void)HLIB_DATETIME_ConvertUnixTimeToDateTime(ulStartTime, &(stStart.stStartTime));
    }

    HxSTD_MemSet(stStart.szStorageId, 0, sizeof(stStart.szStorageId));

    if( NULL != szStorageId )
        HLIB_STD_StrNCpySafe(stStart.szStorageId, (HCHAR*)szStorageId, sizeof(stStart.szStorageId));

    stStart.ulDuration        =    ulDuration;

#if 1
    HxSTD_MemSet(szName, 0, 128);
    hErr = APK_META_QUERY_FindEventName(pstService->uid, ulStartTime, ulStartTime + ulDuration, szName, eventId);
    if(hErr == ERR_OK)
    {
        if(HxSTD_StrLen(szName)>0)
        {
            HxLOG_Error ("[%s:%d] APK_META_QUERY_GetEventNameByStartTime (Event Name: %s) \n", __FUNCTION__, __LINE__, szName);
            HLIB_STD_StrNCpySafe(stStart.szName, szName, sizeof(stStart.szName));
            stStart.nEvtId = eventId;
        }
        else
        {
            HxLOG_Error ("[%s:%d] APK_META_QUERY_GetEventNameByStartTime Error (Svcname: %s) \n", __FUNCTION__, __LINE__, pstService->name);
            HLIB_STD_StrNCpySafe(stStart.szName, pstService->name, sizeof(stStart.szName));
        }
    }
    else
    {
        HxLOG_Error ("[%s:%d] APK_META_QUERY_GetEventNameByStartTime Error (Svcname: %s) \n", __FUNCTION__, __LINE__, pstService->name);
        HLIB_STD_StrNCpySafe(stStart.szName, pstService->name, sizeof(stStart.szName));
    }

#else
    {
        const char *pszName = NULL;
        OPL_ProgrammeHandle programme = NULL;

    if( BPL_STATUS_OK == opl_broadcast_getCurrentProgramme( hBcast, &programme) )
    {
        if( NULL != programme )
        {
            pszName = OPL_Programme_GetName( programme );
            if (NULL != pszName)
            HLIB_STD_StrNCpySafe(stStart.szName, pszName, sizeof(stStart.szName));

            stStart.nEvtId = OPL_Programme_GetEventId( programme );
        }
    }
    else
    {
        HxLOG_Error ("[%s:%d] opl_broadcast_getCurrentProgramme err: \n", __FUNCTION__, __LINE__);
        HLIB_STD_StrNCpySafe(stStart.szName, pstService->name, sizeof(stStart.szName));
    }

        if( NULL != programme )
            OPL_Programme_Delete (programme);

    }
#endif


#if defined(CONFIG_PROD_IR4000HD)
    /*
     *  Note : IR4000HD�� Delayed Recoding �� �������� �ʴ´�.
     */
    stStart.nStartOffset    =   -1;
#endif

    hErr = APK_MEDIA_RECORD_Start ( &stStart, &(hBcast->ulRecSessionId), pszRecordID);
    if( ERR_OK != hErr )
    {
        HxLOG_Error ("[%s:%d] APK_MEDIA_RECORD_Start err: \n", __FUNCTION__, __LINE__);
        return ERR_FAIL;
    }

    if (NULL != pulSessionId)
    {
        *pulSessionId = hBcast->ulRecSessionId;
    }

    return ERR_OK;
}

BPLStatus OplVBroadcast::opl_broadcast_getCurrentProgramme (OplBroadcast_t *hBroadcast, OPL_ProgrammeHandle *programme)
{
    OplBroadcast_t *self = hBroadcast;
    int i =0, isUndefined =0, curIndex =0, tempIndex =0;
    unsigned int ulStartTime =0, ulDuration =0, ulCurTime = 0, ulTempStartTime = 0, ulCurStartTime =0;

    if (self == NULL)
        return BPL_STATUS_ERROR;

    if (self->nPFLen < 1)
    {
        DxService_t service;
        if (opl_broadcast_getCurrentMasterService(self->ulViewId, &service) == BPL_STATUS_OK)
        {
            OPL_METADATASEARCH_SearchPF(NULL, service.uid, (OPL_SearchPfMonitor_t)OplVBroadcast::opl_broadcast_PfMonitor, (void *)self);
            if (self->nPFLen == 0)
                return BPL_STATUS_ERROR;
        }
        else
        {
            return BPL_STATUS_ERROR;
        }
    }

    ulCurTime = HLIB_STD_GetSystemTime();

    for (i = 0; i < self->nPFLen ; i++)
    {
        ulStartTime = OPL_Programme_GetStartTime( self->ahPF[i] );
        ulDuration = OPL_Programme_GetDuration( self->ahPF[i] );

        if( ulStartTime > ulCurTime ) /* skil futuer event.*/
            continue;

        if( (ulStartTime <= ulCurTime )&& (ulDuration == 0 || ulDuration == 0xffffff) )
        {/*������ ������ ���� �̺�Ʈ�� */
            isUndefined = 1;
            tempIndex = i;
            ulTempStartTime = ulStartTime;
            continue;
        }
        else if( (ulStartTime <= ulCurTime )&& ( (ulStartTime + ulDuration) > ulCurTime ) )
        {/* current event */
            curIndex = i;
            ulCurStartTime = ulStartTime;
        }
        else { /* do nothing */ }
    }

    if( ( isUndefined == 1 ) && ( ulTempStartTime >= ulCurStartTime ))
    {/* ������ ������ ���� current event */
        curIndex = tempIndex;
    }

    if( curIndex < self->nPFLen )
    {
        *programme = OPL_Programme_Clone(self->ahPF[curIndex]);
    }

    return BPL_STATUS_OK;
}

OxMediaPlay_Component_e OplVBroadcast::opl_broadcast_cvtComponentType(eOplAVComponentType eType)
{
    OxMediaPlay_Component_e eApkCompType = eOxMP_COMPONENT_NONE;

    switch (eType)
    {
        case OPLAVCOMPONENT_TYPE_VIDEO:     eApkCompType    =   eOxMP_COMPONENT_VIDEO;      break;
        case OPLAVCOMPONENT_TYPE_AUDIO:     eApkCompType    =   eOxMP_COMPONENT_AUDIO;      break;
        case OPLAVCOMPONENT_TYPE_SUBTITLE:  eApkCompType    =   eOxMP_COMPONENT_SUBTITLE;   break;
        case OPLAVCOMPONENT_TYPE_DATA:      eApkCompType    =   eOxMP_COMPONENT_DATA;       break;

        default:
            break;
    }

    return eApkCompType;
}

HERROR OplVBroadcast::opl_broadcast_getComponent(HUINT32 ulViewId, OxMediaPlay_Component_e  eApkCompType, \
        HINT32 nApkCompIndex, OplAVComponent_t *pstComponent)
{
    HERROR hErr = ERR_FAIL;
    OxMediaPlay_Component_t stApkComponent;

    if (NULL == pstComponent)
        return ERR_FAIL;

    HxSTD_MemSet (&stApkComponent, 0, sizeof(OxMediaPlay_Component_t));

    hErr = APK_MEDIA_PLAY_GetComponent (ulViewId, eApkCompType, (HINT32)nApkCompIndex, &stApkComponent);
    if( ERR_OK != hErr )
    {
        HxLOG_Error("[%s:%d] APK_MEDIA_PLAY_GetComponent Error : \n", __FUNCTION__, __LINE__ );
        return ERR_FAIL;
    }

    /* OK */
    HxSTD_MemSet (pstComponent, 0, sizeof(OplAVComponent_t));

    switch (eApkCompType)
    {
        case eOxMP_COMPONENT_VIDEO:     hErr = opl_broadcast_cvtVideoComponent(&(stApkComponent.stVideo), pstComponent);    break;
        case eOxMP_COMPONENT_AUDIO:     hErr = opl_broadcast_cvtAudioComponent(&(stApkComponent.stAudio), pstComponent);    break;
        case eOxMP_COMPONENT_SUBTITLE:  hErr = opl_broadcast_cvtSubtitleComponent(&(stApkComponent.stSubtitle), pstComponent);  break;
        case eOxMP_COMPONENT_DATA:      hErr = opl_broadcast_cvtDataComponent(&(stApkComponent.stData), pstComponent);  break;
        default:
            return ERR_FAIL;
    }

    if( ERR_OK != hErr )
    {
        HxLOG_Error("[%s:%d] can't convert component (APK->OPL) Error : \n", __FUNCTION__, __LINE__ );
        return ERR_FAIL;
    }

    return ERR_OK;
}

BPLStatus OplVBroadcast::opl_broadcast_disableRecNotifier (OplBroadcast_t *hBcast)
{
    DxService_t stService;
    HERROR hErr;

    hErr = opl_broadcast_getCurrentMasterService (hBcast->ulViewId, &stService);
    if (BPL_STATUS_OK != hErr)
    {
        HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
        return BPL_STATUS_ERROR;
    }
    hErr = APK_MEDIA_RECORD_UnregisterNotifier(stService.uid, (OxMediaRec_Notifier_t)OplVBroadcast::opl_broadcast_notifyRecFunc, NULL);
    if (ERR_OK != hErr)
    {
        HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
        return BPL_STATUS_ERROR;
    }

    return BPL_STATUS_OK;
}

void OplVBroadcast::opl_broadcast_notifyRecFunc ( HUINT32 ulSvcUid, HUINT32 ulSessionId, OxMediaPlay_Event_e eEvent, \
        OxMediaPlay_NotifierData_t *pstNotifyData, void *pvUserData )
{
    static HxList_t *pstListItem = NULL;
    eOplRecorderState eRecState = (eOplRecorderState)-1;
    eOplBroadcastEventType eOipfEvent = (eOplBroadcastEventType)-1;

    // common scenario(follow OIPF Scenario) have to added in here
    switch (eEvent)
    {
    case eOxMR_EVENT_REC_STARTED:
        eRecState   = OPLRECORDER_REC_STATE_REC_STARTED;
        eOipfEvent  = OPLBROADCAST_EVENT_TYPE_RECORDING_EVENT;
        break;

    case eOxMR_EVENT_REC_STOPPED:
        eRecState   = OPLRECORDER_REC_STATE_REC_COMPLETED;
        eOipfEvent  = OPLBROADCAST_EVENT_TYPE_RECORDING_EVENT;
        break;
    case eOxMR_EVENT_REC_TSRBUFFER_COPYEND:
        eRecState   = OPLRECORDER_REC_STATE_TSRBUFFER_COPYEND;
        eOipfEvent  = OPLBROADCAST_EVENT_TYPE_RECORDING_EVENT;
        break;
    default:
        return;
    }

    // this have to notify to each Broadcast Objects
    for (pstListItem = s_pstBcastObjList; NULL != pstListItem; pstListItem = pstListItem->next)
    {
        OplBroadcast_t *hBcast = (OplBroadcast_t *)HLIB_LIST_Data (pstListItem);

        if (NULL != hBcast)
        {
            HUINT32          ulLiveSvcUid = 0;

            APK_MEDIA_PLAY_GetRequestId(hBcast->ulViewId, eOxMP_MEDIATYPE_LIVE, (HUINT32*)&ulLiveSvcUid);
            if(ulSvcUid == ulLiveSvcUid)
            {
                hBcast->ulRecSessionId = ulSessionId;
                if ((-1 != eRecState) && (hBcast->eRecState != eRecState))
                {
                    hBcast->eRecState = eRecState;

                    if ((NULL != hBcast->recEventListener) && (-1 != eOipfEvent))
                    {
                        (*(hBcast->recEventListener))(hBcast->hVbcastHandle, eOipfEvent);           // _broadcast_onRecChange
                    }
                }
            }
        }
    }
}

HERROR OplVBroadcast::opl_broadcast_startLiveFcc(HUINT32 ulViewId, HUINT32 ulUID, HUINT32 *pulSessionId)
{
    HERROR hErr = ERR_FAIL;
    OPL_Channel_t pChannel = NULL;

    if( NULL == pulSessionId )
    {
        return ERR_FAIL;
    }

    pChannel = OPL_Channel_FindChannelByUID(NULL, ulUID);

    if( NULL != pChannel )
    {

        OxMediaPlay_StartInfo_t      unStart;
        memset(&unStart, 0x00, sizeof(OxMediaPlay_StartInfo_t));

        unStart.stLive.eType            = eOxMP_MEDIATYPE_LIVE;
        unStart.stLive.eLiveType        = eOxMP_LIVETYPE_FCC;
        unStart.stLive.eTuneMode        = (OxMediaPlay_TuningMode_e)0x00;
        unStart.stLive.ulMasterSvcUid   = ulUID;
        unStart.stLive.ulSuppleSvcUid   = ulUID;
        unStart.stLive.eSuppSvcType     = eOxMP_SUPPLESVCTYPE_Normal;
        unStart.stLive.ulMajorCHNum     = OPL_Channel_GetMajorChannel(pChannel);

        hErr = APK_MEDIA_PLAY_Start (ulViewId, &unStart, pulSessionId);
        if (ERR_OK != hErr)
        {
            HxLOG_Error ("[%s:%d] APK_MEDIA_PLAY_Start err: ViewId=%d\n", __FUNCTION__, __LINE__, ulViewId);
        }
    }
    else
    {
        HxLOG_Error ("[%s:%d] OPL_Channel_FindChannelByUID err: ViewId=%d\n", __FUNCTION__, __LINE__, ulViewId);
    }

    return hErr;
}

void OplVBroadcast::opl_broadcast_PfMonitor (OPL_HANDLE search, OPL_ProgrammeHandle *pf, HINT32 n, OplBroadcast_t *self)
{
    HINT32  i, c, startindex =0;

    if (n > 0 && self->nPFLen == n)
    {
        for (i = 0 ; i < n ; i++)
        {
            if (!OPL_Programme_Equals(self->ahPF[i], pf[i]))
                break;
        }
        if (i == n)
        {
            HxLOG_Print("\t%s duplicated!!\n\n", __FUNCTION__);
#if !defined( CONFIG_OP_JAPAN )
            return;
#endif
        }
    }

    for (i = 0 ; i < self->nPFLen ; i++)
    {
        OPL_Programme_Delete(self->ahPF[i]);
    }
    self->nPFLen = 0;

#if defined( CONFIG_OP_JAPAN )
{
    /* �Ϻ��� ������ �ɷ������ϰ� ���� �̺�Ʈ�� ������ pf[0]�� ��ġ ��Ű�� ���ؼ� */
    HUINT32         ulStartTime =0, ulDuration =0, ulCurTime =0, curIndex =0, tempIndex =0, ulTempStartTime =0, ulCurStartTime =0;
    HBOOL       bIsUndefined = FALSE;
    HINT32      nMasterSvcUid = 0, nPfSvcUid =0;
    OPL_Channel_t   pstChannel = NULL;

    APK_MEDIA_PLAY_GetRequestId (self->ulViewId, eOxMP_MEDIATYPE_LIVE, &nMasterSvcUid);

    ulCurTime = HLIB_STD_GetSystemTime();

    for (i = 0; i < n ; i++)
    {
        pstChannel = OPL_Programme_GetChannel( pf[i] );
        if( pstChannel )
            nPfSvcUid = OPL_Channel_GetUniqueID( pstChannel );

        // ���� ä���� �ƴϸ� ���� �ʿ䰡 ���? �� ���̿� ����ｿ���̴�?
        if (nPfSvcUid != nMasterSvcUid)
        {
            HxLOG_Error ("Current Service is not same...!!!\n");
            return;
        }

        ulStartTime = OPL_Programme_GetStartTime( pf[i] );
        ulDuration = OPL_Programme_GetDuration( pf[i] );

        if( ulStartTime > ulCurTime ) /* �̷� �̺�Ʈ�� �ǳ� �ڴ�. */
            continue;

        if( (ulStartTime <= ulCurTime )&& (ulDuration == 0 || ulDuration == 0xffffff) )
        {/*������ ������ ���� �̺�Ʈ�� */
            bIsUndefined = TRUE;
            tempIndex = i;
            ulTempStartTime = ulStartTime;
            continue;
        }
        else if( (ulStartTime <= ulCurTime )&& ( (ulStartTime + ulDuration) > ulCurTime ) )
        {/* current event */
            curIndex = i;
            ulCurStartTime = ulStartTime;
        }
        else
        { /* do nothing */ }
    }

    if( ( bIsUndefined == TRUE ) && ( ulTempStartTime >= ulCurStartTime ))
    {/* ������ ������ ���� current event */
        curIndex = tempIndex;
    }

    startindex = curIndex;
    i =0;
}
#endif

    for (i = startindex, c = 0 ; c < 12 && i < n ; i++)
    {
        self->ahPF[c] = OPL_Programme_Clone(pf[i]);
        if (self->ahPF[c])
        {
            HxLOG_Print("\t%s() PF[%d] %s\n", __FUNCTION__, c, OPL_Programme_GetName(self->ahPF[c]));
            c++;
        }
    }
    self->nPFLen = c;

    if (search && self->liveEventListener)
    {
        self->liveEventListener(self->hVbcastHandle, OPLBROADCAST_EVENT_TYPE_PROGRAMMES_CHANGED);
    }
}

BPLStatus OplVBroadcast::opl_broadcast_memorizeLastChannelByNetwork (OPL_Channel_t hChannel)
{
    HCHAR szCcid[32] = { 0, };
    OPL_NetworkType_e netType = eOPL_NETWORK_START;
    if(hChannel == NULL)
        return BPL_STATUS_ERROR;
    netType = OPL_Channel_GetNetTypeByOnID(OPL_Channel_GetOrgNetworkID(hChannel));
    OPL_Channel_GetCCID (hChannel, szCcid, 32);
    if(szCcid[0] != '\0')
    {
        switch(netType)
        {
#if defined(CONFIG_OP_JAPAN)
            case eOPL_DTT_NETWORK:
                APK_DB_SetStr(USERDB_JLABS_DTT_LAST_CHANNEL, szCcid);
                break;
            case eOPL_BS_NETWORK:
                APK_DB_SetStr(USERDB_JLABS_BS_LAST_CHANNEL, szCcid);
                break;
            case eOPL_CATV_NETWORK:
                APK_DB_SetStr(USERDB_JLABS_CATV_LAST_CHANNEL, szCcid);
                break;
            case eOPL_CS1_NETWORK:
                APK_DB_SetStr(USERDB_JLABS_CS1_LAST_CHANNEL, szCcid);
                break;
            case eOPL_CS2_NETWORK:
                APK_DB_SetStr(USERDB_JLABS_CS2_LAST_CHANNEL, szCcid);
                break;
            case eOPL_JCHITS_NETWORK:
                APK_DB_SetStr(USERDB_JLABS_JCHITS_LAST_CHANNEL, szCcid);
                break;
#endif
            default:
                break;
        }
    }
    return BPL_STATUS_OK;
}

void OplVBroadcast::live_proc(unsigned int ulViewId, unsigned int session, OxMediaPlay_Event_e eEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
    HBOOL bStateChanged;
    HxList_t *pstListItem;
    OxMediaPlay_LiveViewState_e eViewState;
    OxMediaPlay_LiveLockState_e eLockState;
    OplBroadcastPlayState_e eLastPlayState;
    eOplBroadcastEventType eOipfEvent = (eOplBroadcastEventType)-1, \
            eOipfAdditionalEvent = (eOplBroadcastEventType)-1;
    HUINT32 nSvcUid=0;
#if defined (CONFIG_PROD_HMS1000T) || defined(CONFIG_PROD_DIGIMAXT2)
    static HBOOL bNoSignal = FALSE;
#endif
    static HBOOL bNokAntenna = FALSE;

    // oplbroadcast ������ status ��ｿó��?:
    switch (eEvent)
    {
    case eOxMP_EVENT_LIVE_SvcChanged:
        {
            HUINT32 ulMainViewId = 0;
            APK_MEDIA_PLAY_GetMainViewId(&ulMainViewId);

            if( g_instance )  g_instance->opl_broadcast_getSvcUidByCurrOffset (ulViewId, 0);

            if( ulMainViewId == ulViewId )
            {
                APK_MEDIA_PLAY_GetRequestId (ulViewId, eOxMP_MEDIATYPE_LIVE, (HUINT32*)&nSvcUid);
                if (nSvcUid > 0)
                {
                    if( g_instance ) g_instance->opl_broadcast_memorizeCurrentChannel ((HUINT32)nSvcUid);
                    s_ulOplBroadcast_LastMasterSvcUid = (HUINT32)nSvcUid;
                }
/*
                APK_MEDIA_PLAY_GetRequestId (ulViewId, &nSvcUid); // supple change
                if (nSvcUid > 0)
                {
                    s_ulOplBroadcast_LastSuppleSvcUid = (HUINT32)nSvcUid;
                }
                else
                {
                    s_ulOplBroadcast_LastSuppleSvcUid = s_ulOplBroadcast_LastMasterSvcUid;
                }
*/
            }
        }
        break;

/*
    case eOxMP_EVENT_TUNE_Locked:
        eOipfEvent = OPLBROADCAST_EVENT_TYPE_CHANNEL_CHANGE_SUCCEED;
        break;

    case eOxMP_EVENT_TUNE_NoSignal:
        eOipfEvent = OPLBROADCAST_EVENT_TYPE_CHANNEL_CHANGE_ERROR;
        break;
*/
    case eOxMP_EVENT_SI_EIT_CHANGED:
        /* sbkim, do nothing - PF ǥ�� Timing ������ Ȥ�� ����ｿ�д�?
        eOipfEvent = OPLBROADCAST_EVENT_TYPE_PROGRAMMES_CHANGED;*/
        break;

    case eOxMP_EVENT_LIVE_EventRelay:
        eOipfEvent = OPLBROADCAST_EVENT_TYPE_EVENT_RELAY_NOTIFY;
        break;

    case eOxMP_EVENT_LIVE_Ews:
        eOipfEvent = OPLBROADCAST_EVENT_TYPE_EWS_NOTIFY;
        break;

    case eOxMP_EVENT_PinCtrlChanged:
    case eOxMP_EVENT_LIVE_PinCtrlChangedByBCAS:
        eOipfEvent = OPLBROADCAST_EVENT_TYPE_PARENTAL_RATING_CHANGE;
        break;

    case eOxMP_EVENT_RequestPincode:
        eOipfEvent = OPLBROADCAST_EVENT_TYPE_REQUEST_PINCODE;
        break;

    case eOxMP_EVENT_TrickRestrictModeChanged:
        eOipfEvent = OPLBROADCAST_EVENT_TYPE_TRICK_RESTRICT_MODE_CHANGED;
        break;

    case eOxMP_EVENT_COMPONENT_SELECTED:
        {
            if (eOxMP_COMPONENT_SUBTITLE == ulParam1)
            {
                HINT32  nTrackIndex = ulParam2;

                if (nTrackIndex >= 0)
                {
                    OxMediaPlay_Component_t stComponent;

                    if (APK_MEDIA_PLAY_GetComponent(ulViewId, eOxMP_COMPONENT_SUBTITLE, nTrackIndex, &stComponent) == ERR_OK)
                    {
                        HAPI_BroadcastSignal("signal:onSubtitleTypeChanged", TRUE, "i", stComponent.stSubtitle.eSubtitleType);
                        HAPI_BroadcastSignal("signal:onSubtitleState", TRUE, "i", 1);
						#if defined(CONFIG_OP_UK_DTT)
                        HAPI_BroadcastSignal("signal:onSubtitleStateChekForMhegIcs", TRUE, "i", 1); /* work around: MHE ICS023 scene5: no video ���� ���� subtitle�� ���´�. */
						#endif
                    }
                }
        		eOipfEvent = OPLBROADCAST_EVENT_TYPE_SELECTED_COMPONENT_CHANGE_SUBTITLE;
            }
			else if (eOxMP_COMPONENT_VIDEO == ulParam1)
			{
        		eOipfEvent = OPLBROADCAST_EVENT_TYPE_SELECTED_COMPONENT_CHANGE_VIDEO;
			}
			else if (eOxMP_COMPONENT_AUDIO == ulParam1)
			{
        		eOipfEvent = OPLBROADCAST_EVENT_TYPE_SELECTED_COMPONENT_CHANGE_AUDIO;
			}
        }
        break;
    default:
        break;
    }

    // �� Broadcast Object�� ������ �ϴ� �͵�
    for (pstListItem = s_pstBcastObjList; NULL != pstListItem; pstListItem = pstListItem->next)
    {
        OplBroadcast_t     *hBcast = (OplBroadcast_t *)HLIB_LIST_Data (pstListItem);

        if ((NULL != hBcast) && (ulViewId == hBcast->ulViewId))
        {
            switch (eEvent)
            {
/*
 * note : UI Playbar �� position ���� polling �ϴ� ����ｿ������
 *        position�� �ٷ� �÷��ִ� ������ｿ��?��ｿLive�� Playbar�� ���� �ڵ���
 *        ���� UI�� polling����ｿ��������ｿ�Ʒ� �ڵ�ｿSkip�Ѵ�.
 */
#if 0
                case eOxMP_EVENT_LIVE_TSRTimecodeChanged:
                    /*
                     * LIVE ��û�߿� PLAYBAR ������ �÷��ֱ� ���� Event ó��
                     */
                    eOipfEvent = OPLBROADCAST_EVENT_TYPE_PLAY_POSITION_CHANGED;
                    break;
#endif
                case eOxMP_EVENT_STARTED:
                    /*clean TSR value*/
                    //hBcast->nTsrPlaytime      =   0;
                    hBcast->nTsrPlaytimeTick    =   0;
                    //hBcast->nTsrStarttime     =   0;
                    hBcast->nTsrStarttimeTick   =   0;
                    break;

                case eOxMP_EVENT_COMPONENT_AUDIO_CHANGED:
                    //  TODO: something or nothing.
                    break;

                case eOxMP_EVENT_LIVE_SvcChanged:
                    hBcast->eSessionState = OPLBROADCAST_SESSION_STATE_INITIALIZED;
                    hBcast->ulSessionHandle = 0;

                    hBcast->eViewState  = hBcast->eLastViewState    = eOxMP_LiveViewState_NONE;
                    hBcast->eLockState  = hBcast->eLastLockState    = eOxMP_LiveLockState_NONE;
                    hBcast->ePlaybackError  = OPLBROADCAST_CHANNEL_ERROR_CHANNEL_NOT_SUPPORTED;

                    /*state�� �ٲٵ� play state change event�� ������ �ʴ´�.*/
                    hBcast->ePlayState = OPLBROADCAST_PLAY_STATE_CONNECTING;

                    /*clean TSR value*/
                    hBcast->nTsrPlaytime        =   0;
                    hBcast->nTsrPlaytimeTick    =   0;
                    hBcast->nTsrStarttime       =   0;
                    hBcast->nTsrStarttimeTick   =   0;
                    break;

                case eOxMP_EVENT_SI_EIT_CHANGED:
#if defined( CONFIG_OP_JAPAN )
                    {
                        BPLStatus   hErr = BPL_STATUS_ERROR;
                        DxService_t     stSvcInfo;

                        HxSTD_MemSet( &stSvcInfo, 0x00, sizeof( DxService_t ));

                        hErr = opl_broadcast_getCurrentMasterService (hBcast->ulViewId, &stSvcInfo);
                        if (BPL_STATUS_OK == hErr)
                        {
                                if (hBcast->hQueryHandle)
                                {
                                    while (hBcast->nPFLen--)
                                    {
                                        OPL_Programme_Delete(hBcast->ahPF[hBcast->nPFLen]);
                                    }
                                    hBcast->nPFLen = 0;
                                    OPL_METADATASEARCH_SearchPF(hBcast->hQueryHandle, stSvcInfo.uid, (OPL_SearchPfMonitor_t)OplVBroadcast::opl_broadcast_PfMonitor, (void *)hBcast);
                                }
                        }
                    }
#endif
                    break;

                case eOxMP_EVENT_SI_RCT_CHANGED:
                    {
                        hBcast->nLinkCount = ulParam1;
                        hBcast->nImgSize = ulParam2;
                        HxLOG_Debug("\n##### sh ##### [%s][%d] hBcast->nLinkCount=[%d], hBcast->nImgSize=[%d]\n",__FUNCTION__,__LINE__,hBcast->nLinkCount,hBcast->nImgSize);
                        eOipfEvent = OPLBROADCAST_EVENT_TYPE_SI_RCT_CHANGED;
                    }
                    break;
                case eOxMP_EVENT_SI_NETWORK_CHANGED:
                    hBcast->nStartTimeofChange = ulParam1;
                    hBcast->nASODuration = ulParam2;
                    hBcast->pszNetworkMsg = (HUINT8*)ulParam3;
                    eOipfEvent = OPLBROADCAST_EVENT_TYPE_NCD_CHANGED;
                    break;
                case eOxMP_EVENT_LIVE_EventRelay:
                    hBcast->eventRelaySvcUid = ulParam1;
                    break;

                case eOxMP_EVENT_LIVE_Ews:
                    hBcast->ewsSvcUid    = ulParam1;
                    hBcast->ewsCurSvcUid = ulParam2;
                    break;

                case eOxMP_EVENT_PinCtrlChanged:
                    hBcast->stParentalRatingStatus.contentId    = 0;
                    hBcast->stParentalRatingStatus.bBlocked     = ulParam1;
                    hBcast->stParentalRatingStatus.ratingValue  = ulParam2;
#if defined(CONFIG_OP_SES)
					hBcast->stParentalRatingStatus.bBlockedByCas = ulParam3;
#else
					hBcast->stParentalRatingStatus.caSystemId	= ulParam3;
#endif


#if defined( CONFIG_OP_JAPAN )
                    /*cas ������ ��쿡�ｿeMctrlEvent_LIVE_PinCtrlChangedByBCAS �� ���� rating �� control�ȴ�.*/
                    if(eOxMP_LiveViewState_CAS == hBcast->eViewState)
#else
                    if ((eOxMP_LiveViewState_CAS != hBcast->eViewState) && (eOxMP_LiveViewState_OK != hBcast->eViewState))
#endif
                    {
                        continue;
                    }
                    break;

                case eOxMP_EVENT_RequestPincode:
                    hBcast->pszPincodeMsg = (HUINT8*)HLIB_STD_StrDup((HCHAR*)ulParam1);
                    break;

                case eOxMP_EVENT_LIVE_PinCtrlChangedByBCAS:
                    hBcast->stParentalRatingStatus.contentId = 0;
                    hBcast->stParentalRatingStatus.bBlocked     = ulParam1;
                    hBcast->stParentalRatingStatus.ratingValue  = ulParam2;
                    hBcast->stParentalRatingStatus.caSystemId   = ulParam3;

                    if ((eOxMP_LiveViewState_CAS != hBcast->eViewState) && (eOxMP_LiveViewState_OK != hBcast->eViewState))
                    {
                        continue;
                    }
                    break;

                case eOxMP_EVENT_TUNE_Locked:
                    // � ����̰ｿTune Locked/Fail �� ù �����̾�ｿ�Ѵ�?
                    // View / Lock State�� ��ｿ�����ؾ�?�Ѵ�.

#if defined (CONFIG_PROD_HMS1000T) || defined(CONFIG_PROD_DIGIMAXT2)
                    bNoSignal = FALSE;
#endif
                    eLastPlayState = hBcast->ePlayState;

                    if (bNokAntenna == TRUE)
                        break;

                    if (OPLBROADCAST_SESSION_STATE_INITIALIZED == hBcast->eSessionState)
                    {
                        hBcast->eSessionState = OPLBROADCAST_SESSION_STATE_STARTED;

                        if ((eOxMP_LiveViewState_NONE == hBcast->eViewState) || (eOxMP_LiveLockState_NONE == hBcast->eLockState))
                        {
                            eOipfEvent = OPLBROADCAST_EVENT_TYPE_CHANNEL_CHANGE_SUCCEED;
                            hBcast->ePlayState      = OPLBROADCAST_PLAY_STATE_CONNECTING;
                            hBcast->ePlaybackError  = OPLBROADCAST_CHANNEL_ERROR_NO_ERROR;
                        }
                        else if ((eOxMP_LiveViewState_OK == hBcast->eViewState) && (eOxMP_LiveLockState_OK == hBcast->eLockState))
                        {
                            eOipfEvent = OPLBROADCAST_EVENT_TYPE_CHANNEL_CHANGE_SUCCEED;
                            hBcast->ePlayState      = OPLBROADCAST_PLAY_STATE_PRESENTING;
                            hBcast->ePlaybackError  = OPLBROADCAST_CHANNEL_ERROR_NO_ERROR;
                        }
                        else
                        {
                            eOipfEvent = OPLBROADCAST_EVENT_TYPE_CHANNEL_CHANGE_ERROR;
                            hBcast->ePlayState      = OPLBROADCAST_PLAY_STATE_CONNECTING;
                            hBcast->ePlaybackError  = opl_broadcast_getNextErrorByStates (hBcast, eOxMP_EVENT_TUNE_Locked);
                        }

                            eOipfAdditionalEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
                        }
                    else
                    {
                        // Presenting �߿� �� Tune Locked�� �´ٰ� �ϸ� �װ��� �����ص� �ȴ�.
                        if (OPLBROADCAST_PLAY_STATE_PRESENTING != hBcast->ePlayState)
                        {
                            /*  CATAL TEST Error TC #122 */
                            //hBcast->ePlayState = OPLBROADCAST_PLAY_STATE_CONNECTING;
                            hBcast->ePlayState = OPLBROADCAST_PLAY_STATE_PRESENTING;
                        }

                        hBcast->ePlaybackError = OPLBROADCAST_CHANNEL_ERROR_NO_ERROR;

                        if (eLastPlayState != hBcast->ePlayState)
                        {
                            eOipfEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
                        }
                    }
                    break;

                case eOxMP_EVENT_TUNE_NoSignal:
#if defined (CONFIG_PROD_HMS1000T) || defined(CONFIG_PROD_DIGIMAXT2)
                    bNoSignal = TRUE;
#endif
                    eLastPlayState = hBcast->ePlayState;

                    if (bNokAntenna == TRUE)
                        break;

#if defined (CONFIG_PROD_FVP4000T)
                    //if (OPLBROADCAST_PLAY_STATE_CONNECTING != hBcast->ePlayState) /*  BBC Catal TC#122 - when signal is unpluged, playstate is 0.  */
                    {
                        //hBcast->ePlayState = OPLBROADCAST_PLAY_STATE_CONNECTING;
                        hBcast->ePlayState = OPLBROADCAST_PLAY_STATE_UNREALIZED;
                    }
#else
                    if (OPLBROADCAST_PLAY_STATE_CONNECTING != hBcast->ePlayState) /*  OIPF Spec  */
                    {
                        hBcast->ePlayState = OPLBROADCAST_PLAY_STATE_CONNECTING;
                    }
#endif
                    if (OPLBROADCAST_SESSION_STATE_INITIALIZED == hBcast->eSessionState)
                    {
                        eOipfEvent = OPLBROADCAST_EVENT_TYPE_CHANNEL_CHANGE_ERROR;
                        hBcast->eSessionState = OPLBROADCAST_SESSION_STATE_STARTED;
                    }

                    /*state�� connecting->connecting �̾ error�� �޶����ٸ� event�� �߻��Ų��?/ */
                    if ((eLastPlayState != hBcast->ePlayState)
                        ||((eLastPlayState == OPLBROADCAST_PLAY_STATE_CONNECTING)
                        &&(hBcast->ePlaybackError != OPLBROADCAST_CHANNEL_ERROR_CANNOT_TUNE)))
                    {
                        hBcast->ePlaybackError = OPLBROADCAST_CHANNEL_ERROR_CANNOT_TUNE;
                        if (-1 == eOipfEvent)
                        {
                            eOipfEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
                        }
                        else
                        {
                            eOipfAdditionalEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
                        }
                    }
                    break;

#if defined (CONFIG_PROD_HMS1000T) || defined(CONFIG_PROD_DIGIMAXT2)
                case eOxMP_EVENT_TUNE_AntennaOk:
                    bNokAntenna = FALSE;
                    eLastPlayState = hBcast->ePlayState;

                    if (bNoSignal == TRUE)
                    {
                        hBcast->ePlaybackError = OPLBROADCAST_CHANNEL_ERROR_CANNOT_TUNE;
                        eOipfEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
                        eOipfAdditionalEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
                    }
                    break;

                    if (OPLBROADCAST_SESSION_STATE_INITIALIZED == hBcast->eSessionState)
                    {
                        hBcast->eSessionState = OPLBROADCAST_SESSION_STATE_STARTED;

                        if ((eOxMP_LiveViewState_NONE == hBcast->eViewState) || (eOxMP_LiveLockState_NONE == hBcast->eLockState))
                        {
                            eOipfEvent = OPLBROADCAST_EVENT_TYPE_CHANNEL_CHANGE_SUCCEED;
                            hBcast->ePlayState      = OPLBROADCAST_PLAY_STATE_CONNECTING;
                            hBcast->ePlaybackError  = OPLBROADCAST_CHANNEL_ERROR_NO_ERROR;
                        }
                        else if ((eOxMP_LiveViewState_OK == hBcast->eViewState) && (eOxMP_LiveLockState_OK == hBcast->eLockState))
                        {
                            eOipfEvent = OPLBROADCAST_EVENT_TYPE_CHANNEL_CHANGE_SUCCEED;
                            hBcast->ePlayState      = OPLBROADCAST_PLAY_STATE_PRESENTING;
                            hBcast->ePlaybackError  = OPLBROADCAST_CHANNEL_ERROR_NO_ERROR;
                        }
                        else
                        {
                            eOipfEvent = OPLBROADCAST_EVENT_TYPE_CHANNEL_CHANGE_ERROR;
                            hBcast->ePlayState      = OPLBROADCAST_PLAY_STATE_CONNECTING;
                            hBcast->ePlaybackError  = OplVBroadcast::opl_broadcast_getNextErrorByStates (hBcast, eOxMP_EVENT_TUNE_AntennaOk);
                        }

                        if (-1 == eOipfEvent)
                        {
                            eOipfEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
                        }
                        else
                        {
                            eOipfAdditionalEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
                        }
                    }
                    else
                    {
                        // Presenting �߿� �� Tune Locked�� �´ٰ� �ϸ� �װ��� �����ص� �ȴ�.
                        if (OPLBROADCAST_PLAY_STATE_PRESENTING != hBcast->ePlayState)
                        {
                            hBcast->ePlayState = OPLBROADCAST_PLAY_STATE_CONNECTING;
                        }

                        hBcast->ePlaybackError = OPLBROADCAST_CHANNEL_ERROR_NO_ERROR;

                        if (eLastPlayState != hBcast->ePlayState)
                        {
                            eOipfEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
                        }
                    }
                    break;

                case eOxMP_EVENT_TUNE_AntennaNok:
                    bNokAntenna = TRUE;
                    eLastPlayState = hBcast->ePlayState;
                    if (OPLBROADCAST_PLAY_STATE_CONNECTING != hBcast->ePlayState)
                    {
                        hBcast->ePlayState = OPLBROADCAST_PLAY_STATE_CONNECTING;
                    }

                    if (OPLBROADCAST_SESSION_STATE_INITIALIZED == hBcast->eSessionState)
                    {
                        eOipfEvent = OPLBROADCAST_EVENT_TYPE_CHANNEL_CHANGE_ERROR;
                        hBcast->eSessionState = OPLBROADCAST_SESSION_STATE_STARTED;
                    }

                    /*state�� connecting->connecting �̾ error�� �޶����ٸ� event�� �߻��Ų��?/ */
                    if ((eLastPlayState != hBcast->ePlayState)
                        ||((eLastPlayState == OPLBROADCAST_PLAY_STATE_CONNECTING)
                        &&(hBcast->ePlaybackError != OPLBROADCAST_CHANNEL_ERROR_NOK_ANTENNA)))
                    {
                        hBcast->ePlaybackError = OPLBROADCAST_CHANNEL_ERROR_NOK_ANTENNA;

                        if (-1 == eOipfEvent)
                        {
                            eOipfEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
                        }
                        else
                        {
                            eOipfAdditionalEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
                        }
                    }
                    break;
#endif

                case eOxMP_EVENT_LIVE_ViewStateChanged:
                    eOipfEvent = eOipfAdditionalEvent = (eOplBroadcastEventType)-1;
                    eViewState = (OxMediaPlay_LiveViewState_e)ulParam1;

                    if (hBcast->eViewState == eViewState)
                    {
                        HxLOG_Error ("[%s:%d] ViewState not changed: \n", __FUNCTION__, __LINE__);
                        break;
                    }

                    hBcast->eLastViewState  = hBcast->eViewState;
                    hBcast->eViewState      = eViewState;

                    if (OPLBROADCAST_SESSION_STATE_INITIALIZED == hBcast->eSessionState)
                    {
                        break;
                    }

                    bStateChanged = FALSE;

                    OplVBroadcast::opl_broadcast_getErrorFromNotifier (hBcast, eEvent, &bStateChanged);
                    if (TRUE == bStateChanged)
                    {
                        eOipfEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
                        /*  DIKIM - CATAL TEST  */
                    //  hBcast->ePlayState = OPLBROADCAST_PLAY_STATE_UNREALIZED;
                    }
                    break;

                case eOxMP_EVENT_LIVE_LockStateChanged:
                    eOipfEvent = eOipfAdditionalEvent = (eOplBroadcastEventType)-1;
                    eLockState = (OxMediaPlay_LiveLockState_e)ulParam1;

                    if (hBcast->eLockState == eLockState)
                    {
                        HxLOG_Error ("[%s:%d] LockState not changed: \n", __FUNCTION__, __LINE__);
                        break;
                    }

                    hBcast->eLastLockState  = hBcast->eLockState;
                    hBcast->eLockState      = eLockState;

                    if (OPLBROADCAST_SESSION_STATE_INITIALIZED == hBcast->eSessionState)
                    {
                        break;
                    }

                    bStateChanged = FALSE;

                    OplVBroadcast::opl_broadcast_getErrorFromNotifier (hBcast, eEvent, &bStateChanged);
                    if (TRUE == bStateChanged)
                    {
                        eOipfEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
                    }
                    break;

                case eOxMP_EVENT_LIVE_SvcStopped:
                    eLastPlayState = hBcast->ePlayState;

                    if (OPLBROADCAST_PLAY_STATE_STOPPED != hBcast->ePlayState)
                    {
                        hBcast->ePlayState = OPLBROADCAST_PLAY_STATE_STOPPED;
                    }
                    eOipfEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
                    if (OPLBROADCAST_SESSION_STATE_INITIALIZED == hBcast->eSessionState)
                    {
                        hBcast->eSessionState = OPLBROADCAST_SESSION_STATE_STARTED;
                    }
                    hBcast->ePlaybackError = OPLBROADCAST_CHANNEL_ERROR_NO_ERROR;
                    break;

                case eOxMP_EVENT_TSR_PAUSED:
                case eOxMP_EVENT_TSR_RESUMED:
                    eOipfEvent = OPLBROADCAST_EVENT_TYPE_TSR_STATE_CHANGED;
                    break;

                default:
                    break;
            }

            if (NULL != hBcast->liveEventListener)
            {
                if (eOipfEvent != -1)
                {
                    (*(hBcast->liveEventListener))(hBcast->hVbcastHandle, eOipfEvent);          // _broadcast_onLiveChange
                }

                if(eOipfAdditionalEvent != -1)
                {
                    (*(hBcast->liveEventListener))(hBcast->hVbcastHandle, eOipfAdditionalEvent);            // _broadcast_onLiveChange
                }
            }
        }
    }
}

void OplVBroadcast::tsr_proc(unsigned int ulViewId, unsigned int session, OxMediaPlay_Event_e eEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
   HxList_t *pstListItem;
   HERROR hErr;
   eOplBroadcastEventType eOipfEvent = (eOplBroadcastEventType)-1;
   eOplRecorderState eRecState = (eOplRecorderState)-1;


   // OIPF 2.1 - 192 page, 7.13.2.2
   /*  If the playback reaches the beginning of the time-shift buffer at rewind playback speed, then the play
       state is changed to 2 (��paused��) and a PlaySpeedChanged event with a speed of 0 is generated. If the
       playback reaches the end of the time-shift buffer at fast-forward playback speed, then the play speed is
       set to 1.0 and a PlaySpeedChanged event is generated. */
   //  ==> RW : start pos -> play state = paused, generate the PlaySpeedChanged event (speed = 0)
   //  ==> FF : end pos   -> generate the PlaySpeedChanged event, play speed = 1

   switch (eEvent)
   {
   case eOxMP_EVENT_PLAYSTATE:
       {
           OxMediaPlay_PlayState_e eState;

           APK_MEDIA_PLAY_GetPlayState (ulViewId, &eState);
           switch( eState )
           {
               case eOxMP_PLAYSTATE_STOPPED:
                   eRecState = OPLRECORDER_REC_STATE_UNREALIZED;

                   HxLOG_Error ("HSSEO : OPLRECORDER_REC_STATE_UNREALIZED Error\n");
                   break;
               default :
                   break;
           }
           break;
       }
   case eOxMP_EVENT_STARTED:
       eRecState = OPLRECORDER_REC_STATE_TS_STARTED;
       break;

   case eOxMP_EVENT_STOPPED:
       eRecState = OPLRECORDER_REC_STATE_UNREALIZED;
       break;

   case eOxMP_EVENT_BOF:
       /* HMS1000S, IR4000HD : SPEED 100 , After BOF*/
#if 1
       hErr = APK_MEDIA_PLAY_SetPlaySpeed (ulViewId, 100);
       if (ERR_OK != hErr)
       {
           HxLOG_Error ("APK_MEDIA_PLAY_SetPlaySpeed Error\n");
       }
#else
       hErr = APK_MEDIA_PLAY_SetPlaySpeed (ulViewId, 0);
       if (ERR_OK != hErr)
       {
           HxLOG_Error ("APK_MEDIA_PLAY_SetPlaySpeed Error\n");
       }
#endif
       break;

   case eOxMP_EVENT_EOF:
       /* HMS1000S, IR4000HD : SPEED 100 , After EOF*/
#if 1
       eRecState = OPLRECORDER_REC_STATE_UNREALIZED;
#else
       hErr = APK_MEDIA_PLAY_SetPlaySpeed (ulViewId, 100);
       if (ERR_OK != hErr)
       {
           HxLOG_Error ("APK_MEDIA_PLAY_SetPlaySpeed Error\n");
       }
#endif
       break;

   case eOxMP_EVENT_SPEED_CHANGED:
       eOipfEvent = OPLBROADCAST_EVENT_TYPE_PLAY_SPEED_CHANGED;
       break;
   case eOxMP_EVENT_PLAYTIME:
       eOipfEvent = OPLBROADCAST_EVENT_TYPE_PLAY_POSITION_CHANGED;
       break;
   case eOxMP_EVENT_PinCtrlChanged:
       eOipfEvent = OPLBROADCAST_EVENT_TYPE_PARENTAL_RATING_CHANGE;
       break;
   case eOxMP_EVENT_DRM_BLOCK:
       eOipfEvent = OPLBROADCAST_EVENT_TYPE_DRM_RIGHTS_ERROR_EVENT;
       break;
   case eOxMP_EVENT_SupportedSpeedChanged:
       eOipfEvent = OPLBROADCAST_EVENT_TYPE_SUPPORTED_PLAY_SPEED_CHANGED;
       break;
   case eOxMP_EVENT_TrickRestrictModeChanged:
       eOipfEvent = OPLBROADCAST_EVENT_TYPE_TRICK_RESTRICT_MODE_CHANGED;
       break;

   case eOxMP_EVENT_PLAYERROR:
       eOipfEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
       break;

   case eOxMP_EVENT_COMPONENT_SELECTED:
       {
           if (eOxMP_COMPONENT_SUBTITLE == ulParam1)
           {
               HINT32  nTrackIndex = ulParam2;

               if (nTrackIndex >= 0)
               {
                   OxMediaPlay_Component_t stComponent;

                   if (APK_MEDIA_PLAY_GetComponent(ulViewId, eOxMP_COMPONENT_SUBTITLE, nTrackIndex, &stComponent) == ERR_OK)
                   {
                       HAPI_BroadcastSignal("signal:onSubtitleTypeChanged", TRUE, "i", stComponent.stSubtitle.eSubtitleType);
                       HAPI_BroadcastSignal("signal:onSubtitleState", TRUE, "i", 1);
                   }
               }
           }
       }
       break;

   default:
       break;
   }

   // �� Broadcast Object�� ������ �ϴ� �͵�
   for (pstListItem = s_pstBcastObjList; NULL != pstListItem; pstListItem = pstListItem->next)
   {
       OplBroadcast_t *hBcast = (OplBroadcast_t *)HLIB_LIST_Data (pstListItem);
       if ((NULL != hBcast) && (ulViewId == hBcast->ulViewId))
       {
           switch (eEvent)
           {
               case eOxMP_EVENT_PinCtrlChanged:
                   hBcast->stParentalRatingStatus.contentId = 0;
                   hBcast->stParentalRatingStatus.bBlocked     = ulParam1;
                   hBcast->stParentalRatingStatus.ratingValue  = ulParam2;
#if defined(CONFIG_OP_SES)
                   hBcast->stParentalRatingStatus.bBlockedByCas = ulParam3;
#else
                   hBcast->stParentalRatingStatus.caSystemId	= ulParam3;
#endif

                   if ((eOxMP_LiveViewState_CAS != hBcast->eViewState) && (eOxMP_LiveViewState_OK != hBcast->eViewState))
                   {
                       continue;
                   }
                   break;
               case eOxMP_EVENT_DRM_BLOCK:
                   if(OplVBroadcast::opl_broadcast_getDrmRightsStatusFromJson(&(hBcast->stDrmRightsStatus), ulParam1) == BPL_STATUS_ERROR)
                   {
                       continue;
                   }
                   if ((eOxMP_LiveViewState_CAS != hBcast->eViewState) && (eOxMP_LiveViewState_OK != hBcast->eViewState))
                   {
                       continue;
                   }
                   break;

               case eOxMP_EVENT_PLAYERROR:
                   hBcast->ePlaybackError = OPLBROADCAST_CHANNEL_ERROR_INSUFFICIENT_RESOURCE;
                   break;

               default:
                   break;
           }
           // Playback related
           if ((NULL != hBcast->tsrEventListener) && (-1 != eOipfEvent))
           {
               (*(hBcast->tsrEventListener))(hBcast->hVbcastHandle, eOipfEvent);           // _broadcast_onTsrChange
           }

           // Recording related
           if ((hBcast->eRecState < OPLRECORDER_REC_STATE_SCHEDULED) ||
               (hBcast->eRecState > OPLRECORDER_REC_STATE_REC_UPDATED) ||
               (eRecState == OPLRECORDER_REC_STATE_TS_STARTED) )
           {
               if ((-1 != eRecState) && (hBcast->eRecState != eRecState))
               {
                   hBcast->eRecState = eRecState;

                   if (NULL != hBcast->recEventListener)
                   {
                       (*(hBcast->recEventListener))(hBcast->hVbcastHandle, OPLBROADCAST_EVENT_TYPE_RECORDING_EVENT);          // _broadcast_onTsrChange
                   }

               }
           }
       }
   }
}

HERROR OplVBroadcast::opl_broadcast_startLiveByMheg(HUINT32 ulViewId, HUINT32 ulUID, HUINT32 TuneMode, HUINT32 *pulSessionId)
{
    HERROR hErr = ERR_FAIL;
    OPL_Channel_t pChannel = NULL;

    if( NULL == pulSessionId )
        return ERR_FAIL;


    pChannel = OPL_Channel_FindChannelByUID(NULL, ulUID);
    if( NULL != pChannel )
    {

        OxMediaPlay_StartInfo_t unStart;
        memset(&unStart, 0x00, sizeof(OxMediaPlay_StartInfo_t));

        unStart.stLive.eType = eOxMP_MEDIATYPE_LIVE;
        unStart.stLive.eLiveType = eOxMP_LIVETYPE_MAIN;
        unStart.stLive.ulMasterSvcUid = ulUID;
        unStart.stLive.ulSuppleSvcUid = ulUID;
        unStart.stLive.eSuppSvcType = eOxMP_SUPPLESVCTYPE_Normal;
        unStart.stLive.ulMajorCHNum = OPL_Channel_GetMajorChannel(pChannel);
        unStart.stLive.eTuneMode = (OxMediaPlay_TuningMode_e)TuneMode;

        hErr = APK_MEDIA_PLAY_Start (ulViewId, &unStart, pulSessionId);
        if (ERR_OK != hErr)
        {
            HxLOG_Error ("[%s:%d] APK_MEDIA_PLAY_Start err: ViewId=%d\n", __FUNCTION__, __LINE__, ulViewId);
        }
    }
    else
    {
        HxLOG_Error ("[%s:%d] OPL_Channel_FindChannelByUID err: ViewId=%d\n", __FUNCTION__, __LINE__, ulViewId);
    }

    return hErr;
}

BPLStatus OplVBroadcast::opl_broadcast_enableRecNotifier (OplBroadcast_t *hBcast, DxService_t *pstService)
{
    HERROR hErr;

    hErr = APK_MEDIA_RECORD_RegisterNotifier (pstService->uid, (OxMediaRec_Notifier_t)OplVBroadcast::opl_broadcast_notifyRecFunc, NULL);
    if (ERR_OK != hErr)
    {
        HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
        return BPL_STATUS_ERROR;
    }

    return BPL_STATUS_OK;
}

HERROR OplVBroadcast::opl_broadcast_cvtVideoComponent (OxMediaPlay_VideoComp_t *pstDstVideo, OplAVComponent_t *pstSrcComp)
{
    if ( (NULL == pstDstVideo) || (NULL == pstSrcComp) )
        return ERR_FAIL;

    {
        pstSrcComp->ulComponentTag  = (HUINT32)pstDstVideo->nComponentTag;
        pstSrcComp->ulPid = (HUINT32)pstDstVideo->nPid;
        pstSrcComp->eType = OPLAVCOMPONENT_TYPE_VIDEO;

#if defined( CONFIG_OP_JAPAN)
        {
            /* �Ϻ��� ����ｿPMT ����Ÿ�� �������� EIT ������ �־�ｿ�Ѵ�? */
            OplAVComponent_t        *tempAVComponent = NULL;
            OPL_ProgrammeHandle     programme= NULL;
            OPL_Collection_t        *coll = NULL;
            HINT32              i =0, j =0, collLen =0 ;

            hErr = opl_broadcast_getCurrentProgramme( hBcast ,&programme );
            if( ERR_OK == hErr && programme != NULL )
            {
                coll = OPL_Programme_GetComponents (programme , OPLAVCOMPONENT_TYPE_VIDEO );
                if( coll != NULL )
                {
                    collLen = OPL_Collection_Length(coll);
                    for( i =0; i<collLen ; i++ )
                    {
                        tempAVComponent = (OplAVComponent_t *)OPL_Collection_ItemAt(coll, i);
                        if(  (tempAVComponent != NULL) && (tempAVComponent->ulComponentTag == pstSrcComp->ulComponentTag) )
                        {/* ���ڶ� ������ EIT���� ü�� �ش�. */
                            pstSrcComp->ulComponentType = tempAVComponent->ulComponentType;
                            pstSrcComp->fAspectRatio = tempAVComponent->fAspectRatio;

                            if( tempAVComponent->szText )
                            {
                                HLIB_STD_StrNCpySafe( pstSrcComp->szText, tempAVComponent->szText , OPLAVCOMPONENT_TEXT_SIZE );
                            }

                            if( tempAVComponent->nMultiViewNum > 0 )
                            {
                                for( j=0; j< tempAVComponent->nMultiViewNum; j++ )
                                {
                                    HLIB_STD_StrNCpySafe( pstSrcComp->szMultiView[j], tempAVComponent->szMultiView[j], OPLAVCOMPONENT_MULTIVIEW_SIZE );
                                }
                            }
                            break;
                        }
                    }

                    if( i >= collLen )
                    {
                        //������ PMT �켱 ó�� , hResult = ERR_FAIL;
                    }
                }
                else
                {
                    //������ PMT �켱 ó�� , hResult = BPL_STATUS_ERROR;
                }
            }
            else
            {
                //������ PMT �켱 ó�� , hResult = BPL_STATUS_ERROR;
            }

            if( programme != NULL )
            {
                OPL_Programme_Delete( programme );
            }
        }
#endif
    }

    return ERR_OK;
}

HERROR OplVBroadcast::opl_broadcast_cvtAudioComponent (OxMediaPlay_AudioComp_t *pstDstAudio, OplAVComponent_t *pstSrcComp)
{
#if defined( CONFIG_OP_JAPAN)||defined(CONFIG_OP_FREESAT)
    HERROR                   hErr, hResult = BPL_STATUS_ERROR;
#endif

    if ( (NULL == pstDstAudio) || (NULL == pstSrcComp) )
        return ERR_FAIL;

    {
        HINT32 i;
        HCHAR *encoding = NULL;

        pstSrcComp->ulComponentTag = (HUINT32)pstDstAudio->nComponentTag;
        pstSrcComp->ulPid = (HUINT32)pstDstAudio->nPid;
        pstSrcComp->eType = OPLAVCOMPONENT_TYPE_AUDIO;
        pstSrcComp->eSubType = opl_broadcast_convertAudioComponentTypeToOplSubType (pstDstAudio->eMode);
        encoding = opl_broadcast_convertAudioCodecToOplEncoding(pstDstAudio->eCodec);

        HLIB_STD_StrNCpySafe (pstSrcComp->encoding, (HCHAR*)encoding, HxSTD_StrLen(encoding) + 1);

        for( i=0 ; i<(OxMP_AUDIO_LANG_NUM-1) ; i++ )
        {
#if defined(BPLBROADCAST_AUDIO_DUAL_MONO)
            if (pstDstAudio->astLangInfo[i].szLangCode[0] != '\0' && pstDstAudio->astLangInfo[i+1].szLangCode[0] != '\0')
            {
                HLIB_STD_StrNCpySafe (pstSrcComp->szLanguage, (HCHAR*)pstDstAudio->astLangInfo[i].szLangCode, 4);
                HLIB_STD_StrNCpySafe (pstSrcComp->szLanguage2, (HCHAR*)pstDstAudio->astLangInfo[i+1].szLangCode, 4);
                pstSrcComp->eSubType = OPLAVCOMPONENT_SUBTYPE_AUDIO_DUALMONO;
                break;
            }
            else
            {
                if (pstDstAudio->astLangInfo[i].szLangCode[0] != '\0')
                {
                    HLIB_STD_StrNCpySafe (pstSrcComp->szLanguage, (HCHAR*)pstDstAudio->astLangInfo[i].szLangCode, 4);
                    break;
                }
            }
#else
            if (pstDstAudio->astLangInfo[i].szLangCode[0] != '\0')
            {
                HLIB_STD_StrNCpySafe (pstSrcComp->szLanguage, (HCHAR*)pstDstAudio->astLangInfo[i].szLangCode, 4);
                break;
            }
#endif
        }

        pstSrcComp->bAudioDescription =  OAPI_AUDIO_IS_AD_BM_TYPE(pstDstAudio->astLangInfo[0].eAudType, pstDstAudio->ucMixType, pstDstAudio->eSupAudType);
        pstSrcComp->bAudioDescription |= OAPI_AUDIO_IS_AD_BM_TYPE(pstDstAudio->astLangInfo[1].eAudType, pstDstAudio->ucMixType, pstDstAudio->eSupAudType);
        pstSrcComp->bAudioDescription |= OAPI_AUDIO_IS_AD_RM_TYPE(pstDstAudio->astLangInfo[0].eAudType, pstDstAudio->ucMixType, pstDstAudio->eSupAudType);
        pstSrcComp->bAudioDescription |= OAPI_AUDIO_IS_AD_RM_TYPE(pstDstAudio->astLangInfo[1].eAudType, pstDstAudio->ucMixType, pstDstAudio->eSupAudType);

        //HxLOG_Debug(C_CODE_F_BLACK_B_GREEN" ## ysmoon ## pstSrcComp->szLanguage(%s) \n"C_CODE_RESET"\n", pstSrcComp->szLanguage);
        //HxLOG_Debug(C_CODE_F_BLACK_B_GREEN" ## ysmoon ## pstSrcComp->bAudioDescription(%d) \n"C_CODE_RESET"\n", pstDstAudio->astLangInfo[0].eAudType, pstDstAudio->astLangInfo[1].eAudType, pstDstAudio->ucMixType, pstDstAudio->eSupAudType);
        //HxLOG_Debug(C_CODE_F_BLACK_B_GREEN" ## ysmoon ## pstSrcComp->bAudioDescription(%d) \n"C_CODE_RESET"\n", pstSrcComp->bAudioDescription);
        //HxLOG_Debug(C_CODE_F_BLACK_B_GREEN" ## ysmoon ## pstDstAudio->nPid(0x%x) pstSrcComp->bAudioDescription(%d) \n"C_CODE_RESET"\n", pstDstAudio->nPid, pstSrcComp->bAudioDescription);

#if defined( CONFIG_OP_JAPAN)
        OxMediaPlay_MediaType_e eType;
        APK_MEDIA_PLAY_GetPlayType(hBcast->ulViewId, &eType);

        switch (eType)
        {
            case eOxMP_MEDIATYPE_LIVE:
            {
                /* �Ϻ��� ����ｿPMT ����Ÿ�� �������� EIT ������ �־�ｿ�Ѵ�? */
                OplAVComponent_t        *tempAVComponent = NULL;
                OPL_ProgrammeHandle     programme= NULL;
                OPL_Collection_t        *coll = NULL;
                HINT32              i =0, j =0, collLen =0 ;

                hErr = opl_broadcast_getCurrentProgramme( hBcast , &programme );
                if( ERR_OK == hErr && programme != NULL )
                {
                    coll = OPL_Programme_GetComponents (programme , OPLAVCOMPONENT_TYPE_AUDIO );
                    if( coll != NULL )
                    {
                        collLen = OPL_Collection_Length(coll);
                        for( i =0; i<collLen ; i++ )
                        {
                            tempAVComponent = (OplAVComponent_t *)OPL_Collection_ItemAt(coll, i);
                            if(  (tempAVComponent != NULL) && (tempAVComponent->ulComponentTag == pstSrcComp->ulComponentTag) )
                            {/* ���ڶ� ������ EIT���� ü�� �ش�. */
                                pstSrcComp->ulComponentType = tempAVComponent->ulComponentType;
                                pstSrcComp->ulStreamType = tempAVComponent->ulStreamType;
                                pstSrcComp->bAudioDescription = tempAVComponent->bAudioDescription;
                                pstSrcComp->ulAudioChannels = tempAVComponent->ulAudioChannels;

                                if( tempAVComponent->szLanguage)
                                {
                                    HLIB_STD_StrNCpySafe( pstSrcComp->szLanguage, tempAVComponent->szLanguage , OPLAVCOMPONENT_LANG_SIZE );
                                }

                                if( tempAVComponent->szLanguage2)
                                {
                                    HLIB_STD_StrNCpySafe( pstSrcComp->szLanguage2, tempAVComponent->szLanguage2 , OPLAVCOMPONENT_LANG_SIZE );
                                }

                                if( tempAVComponent->szText)
                                {
                                    HLIB_STD_StrNCpySafe( pstSrcComp->szText, tempAVComponent->szText , OPLAVCOMPONENT_TEXT_SIZE );
                                }

                                if( tempAVComponent->nMultiViewNum > 0 )
                                {
                                    for( j=0; j< tempAVComponent->nMultiViewNum; j++ )
                                    {
                                        HLIB_STD_StrNCpySafe( pstSrcComp->szMultiView[j], tempAVComponent->szMultiView[j], OPLAVCOMPONENT_MULTIVIEW_SIZE );
                                    }
                                }
                                break;
                            }
                        }

                        if( i >= collLen )
                        {
                            //������ PMT �켱 ó�� , hResult = ERR_FAIL;
                        }
                    }
                    else
                    {
                        //������ PMT �켱 ó�� ,hResult = BPL_STATUS_ERROR;
                    }
                }
                else
                {
                    //������ PMT �켱 ó�� ,hResult = BPL_STATUS_ERROR;
                }

                if( programme != NULL )
                {
                    OPL_Programme_Delete( programme );
                }

            }
            break;

            case eOxMP_MEDIATYPE_TSRPLAYBACK:
                if (pstDstAudio->astLangInfo[1].szLangCode )
                    HLIB_STD_StrNCpySafe(pstSrcComp->szLanguage2, (HCHAR *)pstDstAudio->astLangInfo[1].szLangCode, OPLAVCOMPONENT_LANG_SIZE);
                if (pstDstAudio->aucStreamName )
                    HLIB_STD_StrNCpySafe (pstSrcComp->szText, (HCHAR *)pstDstAudio->aucStreamName, OPLAVCOMPONENT_TEXT_SIZE);
                break;

            default:
                hErr = ERR_FAIL;
                break;
        }
#elif defined(CONFIG_OP_FREESAT)
        {
            /* PMT ����Ÿ�� �������� EIT ������ �־�ｿ�Ѵ�?*/
            DxEventComponent_t      *comp = NULL;
            OPL_ProgrammeHandle     programme= NULL;
            OPL_Collection_t        *coll = NULL;
            HINT32              i =0, collLen =0 ;

            if(pstDstAudio->eCodec == eDxAUDIO_CODEC_DOLBY_AC3 || pstDstAudio.eCodec == eDxAUDIO_CODEC_DOLBY_AC3P)
            {
                pstSrcComp->eSubType = OPLAVCOMPONENT_SUBTYPE_AUDIO_DOLBY;
            }

            hErr = opl_broadcast_getCurrentProgramme( hBcast , &programme );
            if( ERR_OK == hErr && programme != NULL )
            {
                coll = OPL_Programme_GetAudioComponent (programme);
                if( coll != NULL )
                {
                    collLen = OPL_Collection_Length(coll);

                    for( i =0; i<collLen ; i++ )
                    {
                        comp = (DxEventComponent_t *)OPL_Collection_ItemAt(coll, i);
                        if((comp != NULL) && (comp->tag == pstSrcComp->ulComponentTag) )
                        {

                            /* ETSI EN 300 468 - Stream_content and component_type */
                            pstSrcComp->eSubType = OPLAVCOMPONENT_SUBTYPE_AUDIO_STEREO;
                            switch (comp->content)
                            {
                            case 0x02:
                                switch (comp->type)
                                {
                                case 0x01:
                                    pstSrcComp->eSubType = OPLAVCOMPONENT_SUBTYPE_AUDIO_SINGLEMONO;
                                    break;

                                case 0x02:
                                    pstSrcComp->eSubType = OPLAVCOMPONENT_SUBTYPE_AUDIO_DUALMONO;
                                    break;

                                case 0x03:
                                    pstSrcComp->eSubType = OPLAVCOMPONENT_SUBTYPE_AUDIO_STEREO;
                                    break;

                                case 0x04:
                                    pstSrcComp->eSubType = OPLAVCOMPONENT_SUBTYPE_AUDIO_MULTILINGUAL;
                                    break;

                                case 0x05:
                                    pstSrcComp->eSubType = OPLAVCOMPONENT_SUBTYPE_AUDIO_SURROUND;
                                    break;

                                default:
                                    break;
                                }
                                break;

                            case 0x04:
                                pstSrcComp->eSubType = OPLAVCOMPONENT_SUBTYPE_AUDIO_DOLBY;
                                break;

                            default:
                                break;
                            }
                            break;
                        }
                    }
                }
                else
                {
                    hResult = BPL_STATUS_ERROR;
                }
            }
            else
            {
                hResult = BPL_STATUS_ERROR;
            }

            if( programme != NULL )
            {
                OPL_Programme_Delete( programme );
            }
            //coll �� ���� �����̾ collection delete�� �ϸ� �ȵȴ�.
            if(coll)
                HLIB_STD_MemFree(coll);
        }

#endif
    }

    return ERR_OK;
}


HERROR OplVBroadcast::opl_broadcast_cvtSubtitleComponent (OxMediaPlay_SubtitleComp_t *pstDstSubtitle, OplAVComponent_t *pstSrcComp)
{
    if ( (NULL == pstDstSubtitle) || (NULL == pstSrcComp) )
        return ERR_FAIL;

    {
        /* common (DVB, EBU)*/
        pstSrcComp->eType = OPLAVCOMPONENT_TYPE_SUBTITLE;
        pstSrcComp->ulPid = (HUINT32)pstDstSubtitle->nPid;

        switch (pstDstSubtitle->eSubtitleType)
        {
            case eOxMP_SUBTITLETYPE_Dvb:
                pstSrcComp->eSubType            = OPLAVCOMPONENT_SUBTYPE_SUBTITLE_DVB;
                pstSrcComp->ulComponentTag  = (HUINT32)pstDstSubtitle->utArg.stDvbInfo.nComponentTag;

                pstSrcComp->bHardOfHearing_Impaired = IsHardOfHearingSubtitle((HUINT32)pstDstSubtitle->utArg.stDvbInfo.nTypeValue);
                HLIB_STD_StrNCpySafe (pstSrcComp->szLanguage, pstDstSubtitle->utArg.stDvbInfo.szLangCode, OPLAVCOMPONENT_LANG_SIZE);

#if defined( CONFIG_OP_JAPAN )
                {
                    /* �Ϻ��� ����ｿPMT ����Ÿ�� �������� EIT ������ �־�ｿ�Ѵ�?*/
                    OplAVComponent_t        *tempAVComponent = NULL;
                    OPL_ProgrammeHandle     programme= NULL;
                    OPL_Collection_t        *coll = NULL;
                    HINT32              collLen = 0, i =0;

                    hResult = BPL_STATUS_OK;

                    if( pstDstSubtitle->utArg.stDvbInfo.nComponentTag < 0x30 || pstDstSubtitle->utArg.stDvbInfo.nComponentTag > 0x37 )
                    {
                        hResult = ERR_FAIL; /* caption ��.. */
                    }
                    else
                    {

                        hErr = opl_broadcast_getCurrentProgramme( hBcast , &programme );
                        if( ERR_OK == hErr && programme != NULL )
                        {/* EIT�� ������ ������ PMT ������ ������ �ȵȴ�. ������ caption �ö���ｿ�ｿ�ִ�? */
                            coll = OPL_Programme_GetComponents (programme , OPLAVCOMPONENT_TYPE_SUBTITLE );
                            if( coll )
                                collLen = OPL_Collection_Length(coll);

                            if( collLen > 0 )
                            {
                                for( i =0; i<collLen ; i++ )
                                {
                                    tempAVComponent = (OplAVComponent_t *)OPL_Collection_ItemAt(coll, i);
                                    if(  (tempAVComponent != NULL) && (tempAVComponent->ulComponentTag == pstSrcComp->ulComponentTag) )
                                    {/* ���ڶ� ������ EIT���� ü�� �ش�. */
                                        pstSrcComp->ulComponentType = tempAVComponent->ulComponentType;

                                        if( tempAVComponent->szLanguage)
                                        {
                                            HLIB_STD_StrNCpySafe( pstSrcComp->szLanguage, tempAVComponent->szLanguage , OPLAVCOMPONENT_LANG_SIZE );
                                        }
                                        break;
                                    }
                                }

                                if( i >= collLen )
                                {
                                    hResult = ERR_FAIL;
                                }
                            }
                            else
                            {
                                hResult = ERR_FAIL;
                            }
                        }
                        else
                        {
                            hResult = ERR_FAIL;
                        }

                        if( programme != NULL )
                        {
                            OPL_Programme_Delete( programme );
                        }
                    }
                }
#endif
                break;

            case eOxMP_SUBTITLETYPE_Ebu:
                pstSrcComp->eSubType = OPLAVCOMPONENT_SUBTYPE_SUBTITLE_EBU;
                pstSrcComp->ulComponentTag = (HUINT32)pstDstSubtitle->utArg.stEbuInfo.nComponentTag;

                HLIB_STD_StrNCpySafe (pstSrcComp->szLanguage, pstDstSubtitle->utArg.stEbuInfo.szLangCode, 4);
                break;

        default:
            break;
        }
    }

    return ERR_OK;
}

HERROR OplVBroadcast::opl_broadcast_cvtDataComponent (OxMediaPlay_DataComp_t *pstDstData, OplAVComponent_t *pstSrcComp)
{
    if ( (NULL == pstDstData) || (NULL == pstSrcComp) )
        return ERR_FAIL;

    {
        pstSrcComp->eType = OPLAVCOMPONENT_TYPE_DATA;
        pstSrcComp->ulComponentTag = (HUINT32)pstDstData->nComponentTag;
        pstSrcComp->ulPid = (HUINT32)pstDstData->nPid;

#if defined( CONFIG_OP_JAPAN)
        {
            /* �Ϻ��� ����ｿPMT ����Ÿ�� �������� EIT ������ �־�ｿ�Ѵ�?*/
            OplAVComponent_t *tempAVComponent = NULL;
            OPL_ProgrammeHandle programme= NULL;
            OPL_Collection_t *coll = NULL;
            HINT32 i =0, j =0, collLen =0 ;

            /* EIT �� ���� �Ǿ��� ���� TRUE */
            pstSrcComp->bDataEntryComponentFlag = FALSE;

            if( (pstSrcComp->ulComponentTag < 0x40) || (pstSrcComp->ulComponentTag  > 0x4f) )
            {/* �� 40 ~ 7f �����ε�,, ���� �� JC �ø�� 4f������ �ؼ� �����ϰ� �Ѵ�. */
                hResult = BPL_STATUS_ERROR;
            }
            else
            {
                hErr = opl_broadcast_getCurrentProgramme( hBcast , &programme );
                if( ERR_OK == hErr && programme != NULL )
                {
                    /* component �� COMPONENT TAG�� �Ѿ�� VIDEO������ �����Ѵ�. */
                    coll = OPL_Programme_GetComponents (programme , OPLAVCOMPONENT_TYPE_DATA );
                    if( coll != NULL )
                    {
                        collLen = OPL_Collection_Length(coll);
                        for( i =0; i<collLen ; i++ )
                        {
                            tempAVComponent = (OplAVComponent_t *)OPL_Collection_ItemAt(coll, i);
                            if(  (tempAVComponent != NULL) && (tempAVComponent->ulComponentTag == pstSrcComp->ulComponentTag) )
                            {/* ���ڶ� ������ EIT���� ü�� �ش�. */
                                pstSrcComp->ulComponentTag = tempAVComponent->ulComponentTag;
                                pstSrcComp->bAssociatedContentFlag = tempAVComponent->bAssociatedContentFlag;
                                pstSrcComp->bDataEntryComponentFlag = tempAVComponent->bDataEntryComponentFlag;
                                pstSrcComp->nDigitalCopyControl = tempAVComponent->nDigitalCopyControl;

                                if( tempAVComponent->szText )
                                {
                                    HLIB_STD_StrNCpySafe( pstSrcComp->szText, tempAVComponent->szText , OPLAVCOMPONENT_TEXT_SIZE );
                                }

                                if( tempAVComponent->nMultiViewNum > 0 )
                                {
                                    for( j=0; j< tempAVComponent->nMultiViewNum; j++ )
                                    {
                                        HLIB_STD_StrNCpySafe( pstSrcComp->szMultiView[j], tempAVComponent->szMultiView[j], OPLAVCOMPONENT_MULTIVIEW_SIZE );
                                    }
                                }
                                break;
                            }
                        }

                        if( i >= collLen )
                        {
                            //������ PMT �켱 ó�� ,hResult = BPL_STATUS_ERROR;
                        }
                    }
                    else
                    {
                        //������ PMT �켱 ó�� ,hResult = BPL_STATUS_ERROR;
                    }
                }
                else
                {
                    //������ PMT �켱 ó�� ,hResult = BPL_STATUS_ERROR;
                }

                if( programme != NULL )
                {
                    OPL_Programme_Delete( programme );
                }
            }
        }
#endif


    }

    return ERR_OK;
}

eOplPlaybackError OplVBroadcast::opl_broadcast_getNextErrorByStates (OplBroadcast_t *hBcast, OxMediaPlay_Event_e eCurrEvent)
{
    // ������ ��ｿantenna short-circuit �� 0 ���� ���� : MOON
    if (eOxMP_LiveViewState_NOK_ANTENNA == hBcast->eViewState)
    {
        return OPLBROADCAST_CHANNEL_ERROR_NOK_ANTENNA;
    }

    // �켱���� 1: Channel Unlock
    if ((eOxMP_LiveViewState_NO_SIGNAL == hBcast->eViewState) ||
        (eOxMP_LiveViewState_NO_SIGNAL_TIMEOUT == hBcast->eViewState))
    {
        return OPLBROADCAST_CHANNEL_ERROR_CANNOT_TUNE;
    }

    if (eOxMP_LiveViewState_MOTOR_MOVING == hBcast->eViewState)
    {
        return OPLBROADCAST_CHANNEL_ERROR_MOTOR_MOVING;
    }

    // �켱���� 2: Empty Service
    if (eOxMP_LiveViewState_EMPTY_SVC == hBcast->eViewState)
    {
        return OPLBROADCAST_CHANNEL_ERROR_EMPTY_CHANNEL;
    }

    // �켱���� 3: User Lock ���?  
    if (eOxMP_LiveLockState_LOCK_PIN == hBcast->eLockState)
    {
        return OPLBROADCAST_CHANNEL_ERROR_PARENTAL_LOCK;
    }


    if (eOxMP_LiveLockState_LOCK_PIN_DENIED == hBcast->eLockState)
    {
        return OPLBROADCAST_CHANNEL_ERROR_PIN_DENIED;
    }

    // eOxMP_LiveViewState_REMOVE_CH ����ｿ��ｿ�켱������ �ִ��� ������ �ֽʽÿ�.
    /*
    if (eOxMP_LiveViewState_REMOVE_CH == hBcast->eViewState)
    {
    }
    */

    // �켱���� 4: CAS
    if (eOxMP_LiveViewState_CAS == hBcast->eViewState)
    {
        return OPLBROADCAST_CHANNEL_ERROR_CAS_BLOCK;
    }

    // �켱���� 5: SVC Lock ���� Lock State��:
    switch (hBcast->eLockState)
    {
    case eOxMP_LiveLockState_RATING_PIN:
    case eOxMP_LiveLockState_SC_PIN:
        return OPLBROADCAST_CHANNEL_ERROR_PARENTAL_LOCK;

    case eOxMP_LiveLockState_RATING_PIN_DENIED:
    case eOxMP_LiveLockState_SC_PIN_DENIED:
        return OPLBROADCAST_CHANNEL_ERROR_PIN_DENIED;

    case eOxMP_LiveLockState_NO_EIT:
        return OPLBROADCAST_CHANNEL_ERROR_NO_EIT;

    default:
        break;
    }

    //
    switch(hBcast->eViewState)
    {
    case eOxMP_LiveViewState_AV_UNDER_RUN:
        return OPLBROADCAST_CHANNEL_ERROR_AV_UNDERRUN;

    case eOxMP_LiveViewState_HDMI_BLOCKED:
        return OPLBROADCAST_CHANNEL_ERROR_HDMI_BLOCK;

    case eOxMP_LiveViewState_NO_VIDEO:
        return OPLBROADCAST_CHANNEL_ERROR_NO_VIDEO;

    case eOxMP_LiveViewState_NO_AV:
        return OPLBROADCAST_CHANNEL_ERROR_NO_AV;

    case eOxMP_LiveViewState_NOT_RUNNING:
        return OPLBROADCAST_CHANNEL_ERROR_NOT_RUNNING;

    default:
        break;
    }

    if (((eOxMP_LiveViewState_OK == hBcast->eViewState) || (eOxMP_LiveViewState_NONE == hBcast->eViewState)) &&
        ((eOxMP_LiveLockState_OK == hBcast->eLockState) || (eOxMP_LiveLockState_NONE == hBcast->eLockState)))
    {
        return OPLBROADCAST_CHANNEL_ERROR_NO_ERROR;
    }

    return OPLBROADCAST_CHANNEL_ERROR_UNDEFINED;
}

BPLStatus OplVBroadcast::opl_broadcast_getErrorFromNotifier (OplBroadcast_t *hBcast, OxMediaPlay_Event_e eCurrEvent, HBOOL *pbStateChanged)
{
    //HBOOL bChanged = FALSE;
    HBOOL bPlayStateChanged = FALSE;
    eOplPlaybackError eNextError;
    OplBroadcastPlayState_e eNextPlayState;

    /*�������� ��쿡�ｿlock state�� �׻� none�� ���̱� ������ �������� ���� lock state�� ��ｿ���ｿ��ｿcheck�Ѵ�.*/
    if ((eOxMP_LiveViewState_NONE == hBcast->eViewState) || (eOxMP_LiveLockState_NONE == hBcast->eLockState))
    {
        if(eOxMP_LiveViewState_EMPTY_SVC == hBcast->eViewState)
        {
            eNextPlayState  = OPLBROADCAST_PLAY_STATE_CONNECTING;
            eNextError = OplVBroadcast::opl_broadcast_getNextErrorByStates (hBcast, eCurrEvent);
        }
        else
        {
            eNextPlayState  = OPLBROADCAST_PLAY_STATE_CONNECTING;
            eNextError = OPLBROADCAST_CHANNEL_ERROR_NO_ERROR;
        }
    }
    else if ((eOxMP_LiveViewState_OK == hBcast->eViewState) && (eOxMP_LiveLockState_OK == hBcast->eLockState))
    {
        eNextPlayState = OPLBROADCAST_PLAY_STATE_PRESENTING;
        eNextError = OPLBROADCAST_CHANNEL_ERROR_NO_ERROR;
    }
    else
    {
        eNextPlayState = OPLBROADCAST_PLAY_STATE_CONNECTING;
        eNextError = OplVBroadcast::opl_broadcast_getNextErrorByStates (hBcast, eCurrEvent);
    }

    if (hBcast->ePlayState != eNextPlayState)
    {
        bPlayStateChanged = TRUE;
    }
    else
    {
        bPlayStateChanged = FALSE;

        // State�� Connecting�����, Error�� NO_ERROR <-> ERROR ��
        // Tune OK -> Error �� ���� ���̱� ������ ���⼭ ERROR ��쳿üũ����ｿ�Ѵ�.
        if (OPLBROADCAST_PLAY_STATE_CONNECTING == eNextPlayState)
        {
            //if ((OPLBROADCAST_CHANNEL_ERROR_NO_ERROR == hBcast->ePlaybackError) && (OPLBROADCAST_CHANNEL_ERROR_NO_ERROR != eNextError))
            if (hBcast->ePlaybackError != eNextError)
            {
                bPlayStateChanged = TRUE;
            }
            /*
            else if ((OPLBROADCAST_CHANNEL_ERROR_NO_ERROR != hBcast->ePlaybackError) && (OPLBROADCAST_CHANNEL_ERROR_NO_ERROR == eNextError))
            {
                bPlayStateChanged = TRUE;
            }
            */
        }
    }
    /*rating pin�� ���� error�� parentalRatingChanged event�� �÷�����Ѵ*/
    if(((eNextError == OPLBROADCAST_CHANNEL_ERROR_PARENTAL_LOCK)||(eNextError == OPLBROADCAST_CHANNEL_ERROR_PIN_DENIED))
    &&((hBcast->eLockState == eOxMP_LiveLockState_RATING_PIN)   ||(hBcast->eLockState == eOxMP_LiveLockState_RATING_PIN_DENIED)))
    {
        bPlayStateChanged = FALSE;
    }
    else
    {
        hBcast->ePlayState = eNextPlayState;
        hBcast->ePlaybackError = eNextError;
    }

    if (NULL != pbStateChanged)                     { *pbStateChanged = bPlayStateChanged; }
    return BPL_STATUS_OK;
}

eOplAVComponentSubType OplVBroadcast::opl_broadcast_convertAudioComponentTypeToOplSubType (OxMediaPlay_AudioMode_e eAudioComp)
{
    switch (eAudioComp)
    {
    case eOxMP_AUDMODE_SINGLEMONO:
        return OPLAVCOMPONENT_SUBTYPE_AUDIO_SINGLEMONO;

    case eOxMP_AUDMODE_DUALMONO:
        return OPLAVCOMPONENT_SUBTYPE_AUDIO_DUALMONO;

    case eOxMP_AUDMODE_STEREO:
        return OPLAVCOMPONENT_SUBTYPE_AUDIO_STEREO;

    case eOxMP_AUDMODE_MULTILINGUAL:
        return OPLAVCOMPONENT_SUBTYPE_AUDIO_MULTILINGUAL;

    case eOxMP_AUDMODE_SURROUND:
        return OPLAVCOMPONENT_SUBTYPE_AUDIO_SURROUND;

    case eOxMP_AUDMODE_DOLBY:
        return OPLAVCOMPONENT_SUBTYPE_AUDIO_DOLBY;

    default:
        break;
    }

    return OPLAVCOMPONENT_SUBTYPE_AUDIO_STEREO;
}

HCHAR *OplVBroadcast::opl_broadcast_convertAudioCodecToOplEncoding (DxAudioCodec_e eCodec)
{
    switch (eCodec)
    {
    case eDxAUDIO_CODEC_MPEG:
        return (HCHAR*)"MPEG1_L2";

    case eDxAUDIO_CODEC_MP3:
        return (HCHAR*)"MPEG1_L3";

    case eDxAUDIO_CODEC_DOLBY_AC3:
        return (HCHAR*)"AC3";

    case eDxAUDIO_CODEC_DOLBY_AC3P:
        return (HCHAR*)"E-AC3";

    case eDxAUDIO_CODEC_PCM:
        return (HCHAR*)"WAV";

    case eDxAUDIO_CODEC_AAC:
    case eDxAUDIO_CODEC_AAC_LOAS:
    case eDxAUDIO_CODEC_AAC_PLUS:
    case eDxAUDIO_CODEC_AAC_PLUS_ADTS:
        return (HCHAR*)"HEAAC";

    case eDxAUDIO_CODEC_DTS:
    case eDxAUDIO_CODEC_DTS_HD:
        return (HCHAR*)"DTS";

    default: break;
    }

    return (HCHAR*)"UNKNOWN";
}

BPLStatus OplVBroadcast::opl_broadcast_getDrmRightsStatusFromJson(OplBroadcastDrmRightsStatus_t *pstStatus, HUINT32 error_status)
{
    if( pstStatus == NULL )
        return BPL_STATUS_ERROR;

    eOplBroadcastDrmRightsError error;
    switch ( error_status )
    {
    case eOxMP_DRMERROR_ValidLicense:
        error = OPLBROADCAST_DRM_RIGHTS_VALID_LICENSE;
        break;
    case eOxMP_DRMERROR_InvalidLicense:
        error = OPLBROADCAST_DRM_RIGHTS_INVALID_LICENSE;
        break;
    case eOxMP_DRMERROR_NoLicense:
        error = OPLBROADCAST_DRM_RIGHTS_NO_LICENSE;
    default:
        return BPL_STATUS_ERROR;
    }
    pstStatus->errorState = (HUINT32)error;
    pstStatus->contentId = NULL;
    pstStatus->drmSystemId = NULL;
    pstStatus->rightsIssuerUrl = NULL;
    return BPL_STATUS_OK;
}

BPLStatus OplVBroadcast::OPL_Broadcast_ReleaseRctInfo(void *rctInfo)
{
    HERROR hErr = BPL_STATUS_ERROR;
    hErr = APK_MEDIA_PLAY_FreeRctInfo((DxService_RctInfo_t*)rctInfo);
    if(hErr != ERR_OK)
    {
        HxLOG_Error ("APK_MEDIA_PLAY_GetRctInfo Err.\n");
        return BPL_STATUS_ERROR;
    }
    return BPL_STATUS_OK;
}

BPLStatus OplVBroadcast::OPL_Broadcast_HasSDTeletext()
{
    HUINT32 ulViewId =0;
    HINT32 nSvcUid =0;
    DxService_t *pService = NULL;
    DxRecListData_t *pstRecord = NULL;
    OxMediaPlay_MediaType_e eMediaType;
    BPLStatus hasTTX = BPL_STATUS_ERROR;

    if(APK_MEDIA_PLAY_GetMainViewId( &ulViewId ) == ERR_OK)
    {
        if( APK_MEDIA_PLAY_GetPlayType(ulViewId, &eMediaType) == ERR_OK)
        {
            if(APK_MEDIA_PLAY_GetRequestId ( ulViewId, eMediaType, (HUINT32*)&nSvcUid) == ERR_OK)
            {
                if(eMediaType == eOxMP_MEDIATYPE_PVRPLAYBACK)
                {
                    pstRecord = APK_META_RECORD_GetByCID(nSvcUid);

                    if(pstRecord != NULL)
                    {
                        if(pstRecord->stMeta.usTtxPid != 0x2000)
                        {
                            hasTTX = BPL_STATUS_OK;
                        }

                        APK_META_RECORD_Release(pstRecord);
                    }
                }
                else // eOxMP_MEDIATYPE_LIVE, eOxMP_MEDIATYPE_TSRPLAYBACK
                {
                    pService = APK_META_SVC_GetService (nSvcUid);

                    if(pService != NULL)
                    {
                        if(pService->ttxPid != 0x2000)
                        {
                            hasTTX = BPL_STATUS_OK;
                        }

                        APK_META_SVC_Delete (pService);
                    }
                }
            }
        }
    }

    return hasTTX;
}
