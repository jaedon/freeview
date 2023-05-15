/**************************************************************************************/
/**
 * @file corsair_hama.cpp
 *
 * Corsair Tools
 *
 * @author  IPM-SW1T
 * @date    2013/02/11
 * @attention   (c) 2011-2012 Humax Co., Ltd.

 *	This program is produced by Humax Co., Ltd. (¡°Humax¡±) and
 *	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 *	non-assignable, non-transferable and non-exclusive license to use this Software.
 *	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 *	you agree to the responsibility to take all reasonable efforts to protect the any information
 *	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 *	reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 *	If you have no authorized license, discontinue using this Software immediately.

 *	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 *	IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 *	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 *	 CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 *	This License is effective until terminated. You may terminate this License at any time by destroying all copies
 *	of the Software including all documentation. This License will terminate immediately without notice from Humax
 *	to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 *	of the Software and all documentation.

 *	The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 *	All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 *	in the Republic of Korea.

 *
 **************************************************************************************
 **/

#include "oplvod_extapp.h"  // for DIAL
#include "opldial.h"        //


#include "corsair_hama.h"
#include "corsair.h"
#include "corsair_hbbtv_manager.h"
#include "corsair_gfx.h"
#include "corsair_ime.h"
#include "corsair_postman.h"
#include "corsair_otvs.h"
#include "corsair_tools.h"
#include "corsair_product_env.h"
#include "corsair_ait_manager.h"
#include "corsair_debug.h"

#include "oplvideobroadcasthandle.h"
#include "oplappmgr.h"
#include "opllocalsystem.h"
#include "bplglobal.h"

#include "apk.h"
#include "hapi.h"
#include <apk.h>
#include "oplnetflix.h"


#include "corsair_input.h"

#include "corsair_hbbtv_surface.h"
#include "corsair_hbbtv_manager.h"

#include <directfb_keynames.h>
#include <input_api.h>


using namespace std;

#define HDD_MOUNT_PATH "/mnt/hd2"
#define WIDGET_MOUNT_PATH "/mnt/hd4"

#if defined(CONFIG_OP_JCOM)
#include "opljlabslocalsystem.h"
#endif
extern Corsair* g_corsair;


/**********************************************************************************
 * CorsairHama
 **********************************************************************************/
void CorsairHama::initHamaSignal(void)
{
    HAPI_ConnectSystemSignal("hama:onPanelPriority", "i", NULL, CorsairHama::onPanelPriority);
    HAPI_ConnectSystemSignal("hama:onSystemUserActionIdle", NULL, NULL, CorsairHama::onUserActionIdle);

    HAPI_ConnectSignal("signal:onStartPortal", NULL, CorsairHama::onStartPortal);
    HAPI_ConnectSignal("signal:onStopPortal",  NULL, CorsairHama::onStopPortal);
    HAPI_ConnectSignal("signal:onOSKResult", NULL, CorsairHama::onOSKResultStr);
    HAPI_ConnectSignal("signal:onNetworkConnected", NULL, CorsairHama::onNetworkConnected);
    HAPI_ConnectSignal("signal:onSystemStateChanged", NULL, CorsairHama::onSystemStateChanged);
    HAPI_ConnectSignal("signal:onHWindowEvent", NULL, CorsairHama::onHWindowEvent);
    HAPI_ConnectSignal("signal:onHbbTVAppStart", NULL, CorsairHama::onHbbTVAppStart);
    HAPI_ConnectSignal("signal:onTeletextRun", NULL, CorsairHama::onTeletextRun);
    HAPI_ConnectSignal("signal:onTeletextState", NULL, CorsairHama::onTeletextState);
    HAPI_ConnectSignal("signal:onSystemLanguageChanged", NULL, CorsairHama::onSystemLanguageChanged);
    HAPI_ConnectSignal("signal:onDialStarted", NULL, CorsairHama::onDialStarted);
    HAPI_ConnectSignal("signal:onDialStopped", NULL, CorsairHama::onDialStopped);
    HAPI_ConnectSignal("signal:onStorageMounted", NULL, CorsairHama::onStorageMounted);
    HAPI_ConnectSignal("signal:onStorageUnmounted", NULL, CorsairHama::onStorageUnmounted); // for DIAL
    HAPI_ConnectSignal("signal:onHumaxApplicationCreate", NULL, CorsairHama::onHumaxApplicationCreate);
    HAPI_ConnectSignal("signal:onHumaxApplicationLoaded", NULL, CorsairHama::onHumaxApplicationLoaded);
    HAPI_ConnectSignal("signal:onHumaxApplicationDestroy", NULL, CorsairHama::onHumaxApplicationDestroy);
    HAPI_ConnectSignal("signal:onHumaxApplicationUnloaded", NULL, CorsairHama::onHumaxApplicationUnloaded);
    HAPI_ConnectSignal("signal:onHbbTVEnabled", NULL, CorsairHama::onHbbTVEnabled);
    HAPI_ConnectSignal("signal:onNagraPRMstart", NULL, CorsairHama::onNagraPRMstart);
#if defined(CONFIG_PROD_FVP4000T)
    HAPI_ConnectSignal("signal:onMhegLaunchApplication", NULL, CorsairHama::onMhegLaunchApplication);
    HAPI_ConnectSignal("signal:onMhegNotifyAppStatus", NULL, CorsairHama::onMhegNotifyAppStatus);
    HAPI_ConnectSignal("signal:onDeleteCookies", NULL, CorsairHama::onDeleteCookies);
#endif
    HAPI_ConnectSignal("signal:onNetflixStateChanged", NULL, CorsairHama::onNetflixStateChanged);
    HAPI_ConnectSignal("signal:onHbbTvVodStopped", NULL, CorsairHama::onHbbTvVodStopped);
}

void CorsairHama::shutDown(void *data)
{
#if 1
    if ( !getCorsair() ) return;
    getCorsair()->stop();
#else
    OplVideoBroadcastHandle *vbcastObj = OplVideoBroadcastHandle::create(OplVideoHandle::EHandleOwnerTypeUser);
    if ( vbcastObj )
    {
        vbcastObj->setFullScreen(true);
        OplVideoHandle::unRegist(vbcastObj);
        delete vbcastObj;
    }

    CorsairGfx *corsair_gfx = CorsairGfx::getInstance();
    if (corsair_gfx)
    {
        corsair_gfx->freeSurfaces();
        corsair_gfx->shutdown();
    }

    abort();
    exit(-1);
#endif
}

void CorsairHama::requestShutDown(void *data)
{
    HAPI_DeleteMyApplication(getpid());
}

void CorsairHama::startHbbtvApp(void *data)
{
    if ( !getCorsair() ) return;

    CorsairHbbTVManager *hbbtv_manager = getCorsair()->getHbbTVManager();
    if ( !hbbtv_manager ) return;

    eOplHbbTVApplicationType app_type = (eOplHbbTVApplicationType)((int)data);
    switch( app_type )
    {
        case OPLHBBTVAPPLICATIONTYPE_AUTOSTART:
            hbbtv_manager->action(new CorsairHbbTVStartAction(CorsairHbbTVApp::HBBTV_APPTYPE_BR_AUTOSTART, hbbtv_manager));
            break;

        case OPLHBBTVAPPLICATIONTYPE_INDEPENDENT:
            hbbtv_manager->action(new CorsairHbbTVStartAction(CorsairHbbTVApp::HBBTV_APPTYPE_BI, hbbtv_manager));
            break;

        case OPLHBBTVAPPLICATIONTYPE_UNDEFINED:
        default:
            hbbtv_manager->action(new CorsairHbbTVStopAction(CorsairHbbTVApp::HBBTV_APPTYPE_BR_AUTOSTART, hbbtv_manager));
            break;
    }
}

void CorsairHama::setNetworkState(void *data)
{
    if ( !getCorsair() ) return;

    CorsairHbbTVManager *hbbtv_manager = getCorsair()->getHbbTVManager();
    if ( hbbtv_manager )
        hbbtv_manager->action(new CorsairHbbTVNetworkUpdateAction(hbbtv_manager));

    getCorsair()->initUserAgent();
}

void CorsairHama::startPortal(void *data)
{
    if ( !getCorsair() ) return;

    std::string url = "";
    char buf[1024] = {0,};
    if ( CorsairTools::getUrlfromUsb(buf, sizeof(buf)) )
        url = string(buf);

    if( url != "" )
    {
        CorsairHbbTVManager *hbbtv_manager = getCorsair()->getHbbTVManager();
        if ( hbbtv_manager )
            hbbtv_manager->action(new CorsairHbbTVBIAppStartAction(url.c_str(), hbbtv_manager));
    }
}

void CorsairHama::stopPortal(void *data)
{
    if ( !getCorsair() ) return;

    CorsairHbbTVManager *hbbtv_manager = getCorsair()->getHbbTVManager();
    if ( hbbtv_manager )
        hbbtv_manager->action(new CorsairHbbTVStopAction(CorsairHbbTVApp::HBBTV_APPTYPE_BI, hbbtv_manager));
}

void CorsairHama::setPanelPriority(void *data)
{
    APK_OUTPUT_PANEL_SetAppPriority((HUINT32)data);
}

void CorsairHama::resultKeyboard(void *data)
{
    getCorsair()->getIme()->commit_ext((const char *)data);
}

void CorsairHama::startSimpleKeyboard(void *data)
{
    getCorsair()->getIme()->update_ext((const char *)data);
}

void CorsairHama::startKeyboard(void *data)
{
    getCorsair()->getIme()->update_ext((const char *)data);
}


#if defined(CONFIG_OP_JCOM)
void CorsairHama::cancelKeyboard(void *data)
{
    getCorsair()->getIme()->commit_ext((const char *)NULL);
}

void CorsairHama::shutDownSurfer(void *data)
{
    HCHAR	* appName = strdup("FULLBROWSER");
    OplJlabsLocalSystem::ResidentAppFinishNotifier((HCHAR *)appName);
}
#endif

void CorsairHama::toggleTeletext()
{
    if ( !getCorsair() ) return;

    CorsairHbbTVManager *hbbtv_manager = getCorsair()->getHbbTVManager();
    if ( hbbtv_manager )
        hbbtv_manager->action(new CorsairHbbTVTeletextAction(hbbtv_manager));
}

void CorsairHama::setSystemLanguage(int data)
{
    if ( !getCorsair() || !getCorsair()->getPostman() ) 
        return;

    char *menu_lang = (char*)HLIB_LANG_IdTo639((HxLANG_Id_e)data);

    if ( NULL != menu_lang )
    {
        CorsairMail *mail = new CorsairMailSetSystemLanguage(menu_lang);
        if ( NULL != mail )
        {
            getCorsair()->getPostman()->post(mail);
        }
    }
    return;
}

void CorsairHama::startDragonfly()
{
    if ( !getCorsair() || !getCorsair()->getPostman() ) return;

    CorsairMail *mail = new CorsairMailConnectDragonfly();
    if ( !mail ) return;

    getCorsair()->getPostman()->post(mail);
}

void CorsairHama::initializeWidgets()
{
    if ( !getCorsair() || !getCorsair()->getPostman() ) return;
    /**
        * @todo it need something code to initialize Widgets.  
        *
        */
}

//	i
HERROR	CorsairHama::onPanelPriority(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    LOG_ASSERT(nArgc == 1);

    CorsairHama::setPanelPriority((void*)HxOBJECT_INT(apArgv[0]));

    return ERR_OK;
}

HERROR	CorsairHama::onUserActionIdle(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    OplLocalSystem::userActionIdle();

    return ERR_OK;
}

HERROR	CorsairHama::onStartPortal(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    CorsairHama::startPortal(NULL);

    return ERR_OK;
}

HERROR	CorsairHama::onStopPortal(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    CorsairHama::stopPortal(NULL);

    return ERR_OK;
}

//	i
HERROR	CorsairHama::onNetworkConnected(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    LOG_ASSERT(nArgc == 1);

    CorsairHama::setNetworkState((void*)HxOBJECT_INT(apArgv[0]));

    return ERR_OK;
}

HERROR	CorsairHama::onSystemStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    LOG_ASSERT(nArgc == 1);

    if(HxOBJECT_INT(apArgv[0]) == 3) 			//ePAMA_STATUS_Shutdown
    {
        if ( !getCorsair() )
            return ERR_FAIL;

        CorsairHbbTVManager *hbbtv_manager = getCorsair()->getHbbTVManager();
        if ( !hbbtv_manager )
            return ERR_FAIL;

        if( hbbtv_manager->isRunningApplication() )
        {
            CorsairHbbTVApp *app = hbbtv_manager->getRunningApplication(CorsairHbbTVApp::HBBTV_APPTYPE_BI);
            if ( app )
            {
                app->stop_by_system();
                return ERR_OK;
            }
#if defined(CONFIG_PROD_FVP4000T)
            app = hbbtv_manager->getApplication(CorsairHbbTVApp::HBBTV_APPTYPE_BR_XMLAIT);
            if( app )
            {
                app->stop_by_system();
                return ERR_OK;
            }
#endif
        }

    }

    return ERR_OK;
}

HERROR	CorsairHama::onHWindowEvent(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    return ERR_OK;
}

//	i
HERROR	CorsairHama::onHbbTVAppStart(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    LOG_ASSERT(nArgc == 1);

    CorsairHama::startHbbtvApp((void*)HxOBJECT_INT(apArgv[0]));

    return ERR_OK;
}

HERROR	CorsairHama::onTeletextRun(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    LOG_ASSERT(nArgc == 1);

    CorsairHama::toggleTeletext();

    return ERR_OK;
}

//	i
HERROR	CorsairHama::onTeletextState(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    LOG_ASSERT(nArgc == 1);

    if (HxOBJECT_INT(apArgv[0]) == 0)
        CorsairHama::toggleTeletext();

    return ERR_OK;
}

HERROR	CorsairHama::onSystemLanguageChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    LOG_ASSERT(nArgc == 1);

    CorsairHama::setSystemLanguage(HxOBJECT_INT(apArgv[0]));

    return ERR_OK;
}


//	s
HERROR	CorsairHama::onOSKResultStr(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    LOG_ASSERT(nArgc == 1);

    OplLocalSystem::keyboardResult((char*)HxOBJECT_STR(apArgv[0]));

    return ERR_OK;
}

//	s
HERROR	CorsairHama::onOSKResult(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    LOG_ASSERT(nArgc == 1);

    CorsairHama::resultKeyboard((void*)HxOBJECT_STR(apArgv[0]));

    return ERR_OK;
}

//	s
HERROR	CorsairHama::onOSKCancel(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    LOG_ASSERT(nArgc == 1);

#if defined(CONFIG_OP_JCOM)
    CorsairHama::cancelKeyboard(NULL);
#endif

    return ERR_OK;
}

//	s
HERROR	CorsairHama::onSimpleKeyboardStr(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    LOG_ASSERT(nArgc == 1);

    CorsairHama::startSimpleKeyboard((void*)HxOBJECT_STR(apArgv[0]));

    return ERR_OK;
}

HERROR	CorsairHama::onKeyboardStr(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    LOG_ASSERT(nArgc == 1);
    string argv0 = (const char *)HxOBJECT_STR(apArgv[0]);

    char argvStr[128];
    snprintf(argvStr,127, "%s",argv0.c_str());
    int len=strlen(argvStr);

    CorsairHama::startKeyboard((void*)HxOBJECT_STR(apArgv[0]));


    HInputEvent_t keyEvent;
    memset(&keyEvent, 0x00, sizeof(HInputEvent_t));

    keyEvent.window_id = 0x5;
    keyEvent.request_id = 0x1d;
    keyEvent.eInputReturnType = eHINPUTRET_NONE;
    keyEvent.type = DIET_KEYPRESS;
    keyEvent.device_id = 0;
    keyEvent.flags = 0;

    keyEvent.key_code = argvStr[len-1];
    keyEvent.key_symbol = argvStr[len-1];

    keyEvent.modifiers = 0;


    GOGI_KEYBOARD_EVENT gogi_evt;
    char       key_value[16] = {0,};
    DFBInputEventType   type;
    type = (DFBInputEventType)keyEvent.type;
    dfb_to_gogi_key_symbol( type, keyEvent.key_symbol,
            keyEvent.key_code, keyEvent.key_id,
            keyEvent.modifiers, gogi_evt, key_value, keyEvent.flags);
    gogi_evt.key_repeat = 0;
    gogi_evt.key_value = key_value;

    GogiOperaWindow* hbbtv_window = g_corsair->getHbbTVSurface()->getWindow();
    if (hbbtv_window)
    {
        gogi_bool result=opera_hbbtv_key_input( hbbtv_window, &gogi_evt);
    }

    return ERR_OK;
}


HERROR	CorsairHama::onDialStarted(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    LOG_ASSERT(nArgc == 1);

    if ( !getCorsair() ) return ERR_FAIL;

    CorsairHbbTVManager *hbbtv_manager = getCorsair()->getHbbTVManager();
    if ( !hbbtv_manager ) return ERR_FAIL;

    CorsairHbbTVApp *app = hbbtv_manager->getApplication(CorsairHbbTVApp::HBBTV_APPTYPE_BI);
    if ( app )
    {
        CorsairHbbTVBIApp *bi_app = dynamic_cast<CorsairHbbTVBIApp*>(app);
        if ( NULL!=bi_app && bi_app->isRunning() )
            app->stop();
    }

    return ERR_OK;
}

HERROR	CorsairHama::onDialStopped(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    if ( !getCorsair() ) return ERR_FAIL;

    CorsairHbbTVManager *hbbtv_manager = getCorsair()->getHbbTVManager();
    if ( hbbtv_manager )
        hbbtv_manager->action(new CorsairHbbTVStopAction(CorsairHbbTVApp::HBBTV_APPTYPE_BI, hbbtv_manager));

    return ERR_OK;
}

HERROR	CorsairHama::onStorageMounted(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    std::string cookie;
    if ( OplVodExtApp::getTVAppsCookie(cookie) )
    {
        OPL_DIAL_SetCookie(cookie.c_str());
    }

    if ( strstr( (char*)HxOBJECT_STR(apArgv[0]), "/media/" ) )
    {

#ifdef CONFIG_DEBUG
        CorsairTools::ReloadMainAppFromUSB();
        CorsairHama::startDragonfly();
        getCorsair()->initUserAgent();
#else
        if ( CorsairProductEnv::getEnabledMainapp() )
            CorsairTools::ReloadMainAppFromUSB();

        if ( CorsairProductEnv::getEnabledDragonfly() )
            CorsairHama::startDragonfly();

        getCorsair()->initUserAgent();
#endif

    }
    else if ( strstr( (char*)HxOBJECT_STR(apArgv[0]), HDD_MOUNT_PATH ) )
    {
        /* when HDD is mounted, if there is an application from the end user, stb will start the application. */
#if 0   /* this is blocked temporary. */
        if ( access(USER_INSTALLED_APPLICATION_PATH, 0 ) == 0 )
        {
            if ( !getCorsair() || !getCorsair()->getPostman() ) return ERR_OK;

            CorsairMail *mail = new CorsairMailOpenUrl(USER_INSTALLED_APPLICATION_URI);
            if ( !mail ) return ERR_OK;

            getCorsair()->getPostman()->post(mail);
        }
#endif
        if ( HLIB_DIR_IsMountedPath(WIDGET_MOUNT_PATH) )
        {
            static string s_lastpath;
            if ( (access(WIDGET_MOUNT_PATH, R_OK|W_OK) == 0 ) && strcmp(s_lastpath.c_str(), (char*)HxOBJECT_STR(apArgv[0]) ) != 0 )
            {
                s_lastpath = (char*)HxOBJECT_STR(apArgv[0]);
                CorsairHama::initializeWidgets();
            }
        }
    }
    return ERR_OK;
}

// for DIAL
HERROR	CorsairHama::onStorageUnmounted(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    std::string cookie;
    if ( OplVodExtApp::getTVAppsCookie(cookie) )
    {
        OPL_DIAL_SetCookie(cookie.c_str());
        return ERR_OK;
    }
    else
        return ERR_FAIL;
}
HERROR	CorsairHama::onHumaxApplicationCreate(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    char *name = NULL;
    char *url = NULL;

    if( nArgc == 2 )
    {
        name = (char*)HxOBJECT_STR(apArgv[0]);
        url = (char*)HxOBJECT_STR(apArgv[1]);
    }
    else
    {
        name = (char*)HxOBJECT_STR(apArgv[0]);
    }

    CorsairMail *mail = new CorsairMailCreateHumaxApplication(name, url);
    if ( mail == NULL )
        return ERR_FAIL;

    if ( getCorsair() != NULL && getCorsair()->getPostman() != NULL )
        getCorsair()->getPostman()->post(mail);
    else
    {
        delete mail;
        mail = NULL;
    }
    return ERR_OK;
}

HERROR	CorsairHama::onHumaxApplicationDestroy(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    if( nArgc == 1 )
    {
        char *name = (char*)HxOBJECT_STR(apArgv[0]);

        CorsairMail *mail = new CorsairMailDestroyHumaxApplication(name);

        if ( mail == NULL )
            return ERR_FAIL;

        if ( getCorsair() != NULL && getCorsair()->getPostman() != NULL )
            getCorsair()->getPostman()->post(mail);
        else
        {
            delete mail;
            mail = NULL;
        }
    }

    return ERR_OK;
}

HERROR	CorsairHama::onHumaxApplicationLoaded(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    if( nArgc == 1 )
    {
        std::string app_name = HxOBJECT_STR(apArgv[0]);
        OplOIPFApplicationManager::setHumaxApplicationLoad(OplOIPFApplicationManager::getHumaxApplicationType(app_name.c_str()));
    }
    return ERR_OK;
}

HERROR	CorsairHama::onHumaxApplicationUnloaded(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    if( nArgc == 1 )
    {
        std::string app_name = HxOBJECT_STR(apArgv[0]);
        OplOIPFApplicationManager::setHumaxApplicationUnload(OplOIPFApplicationManager::getHumaxApplicationType(app_name.c_str()));
    }
    return ERR_OK;
}

HERROR	CorsairHama::onHbbTVEnabled(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    if( nArgc == 1 )
    {
        bool enable = HxOBJECT_INT(apArgv[0]);
        CorsairMail *mail = new CorsairMailEnableHbbTV(enable);

        if ( !mail ) return ERR_FAIL;

        if ( getCorsair() != NULL && getCorsair()->getPostman() != NULL )
            getCorsair()->getPostman()->post(mail);
        else
        {
            delete mail;
            mail = NULL;
        }
    }
    return ERR_OK;
}

HERROR	CorsairHama::onNagraPRMstart(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    if ( !getCorsair() ) return ERR_FAIL;

    OplOIPFApplicationManager::setNagraPrmStarted();

    CorsairHbbTVManager *hbbtv_manager = getCorsair()->getHbbTVManager();
    if ( hbbtv_manager )
    {
        hbbtv_manager->setNagraPRMStatus(true);
    }

    return ERR_OK;
}

#if defined(CONFIG_PROD_FVP4000T)
HERROR	CorsairHama::onMhegLaunchApplication(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    if( nArgc == 1 )
    {
        CorsairHbbTVManager *hbbtv_manager = getCorsair()->getHbbTVManager();
        string url = HxOBJECT_STR(apArgv[0]);
        LOG_DEBUG("onMhegLaunchApplication : %s \n",url.c_str());
        if(!url.empty()){
            int pos;
            while( (pos = url.find("%2B")) != std::string::npos){
                url.erase(pos,3);
                url.insert(pos,"+");
            }
        }
        LOG_DEBUG("onMhegLaunchApplication : %s \n",url.c_str());

        if ( hbbtv_manager )
            hbbtv_manager->action(new CorsairHbbTVBIAppStartAction(url.c_str(), hbbtv_manager, CorsairHbbTVBIAppStartAction::HBBTV_BIAPP_START_NOTOFFLIVE));
    }
    return ERR_OK;
}

HERROR	CorsairHama::onMhegNotifyAppStatus(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    if ( !getCorsair() || !getCorsair()->getPostman() ) return ERR_FAIL;
        
    if( nArgc == 1 )
    {
        CorsairMail *mail = NULL;
        int status = HxOBJECT_INT(apArgv[0]);

        LOG_DEBUG("Mheg Status : %d \n",status);
        switch(status)
        {
            case MHEG_STATUS_STARTING:
                mail = new CorsairMailSetMhegStatus(true);
                if ( mail != NULL )
                {
                    getCorsair()->getPostman()->post(mail);
                }
                break;

            case MHEG_STATUS_STOPPING:
                mail = new CorsairMailSetMhegStatus(false);
                if ( mail != NULL )
                {
                    getCorsair()->getPostman()->post(mail);
                }
                mail = new CorsairMailResendAIT();
                if ( mail != NULL )
                {
                    getCorsair()->getPostman()->post(mail);
                }
                break;

            default:
                break;
        }
    }

    return ERR_OK;

}

HERROR	CorsairHama::onDeleteCookies(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    CorsairDebug::debugCookieDelete();

    return ERR_OK;
}
#endif

HERROR CorsairHama::onNetflixStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    if( nArgc == 1 )
    {
        int status = HxOBJECT_INT(apArgv[0]);

        OplNetflix::callStatusChangedCallback(status);
    }
    return ERR_OK;
}

HERROR CorsairHama::onHbbTvVodStopped(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    if ( !getCorsair() ) return ERR_FAIL;
    CorsairHbbTVManager *hbbtv_manager = getCorsair()->getHbbTVManager();
    if ( !hbbtv_manager ) return ERR_FAIL;
    hbbtv_manager->addAction(new CorsairHbbTVVodStopAction());

    return ERR_OK;
}


