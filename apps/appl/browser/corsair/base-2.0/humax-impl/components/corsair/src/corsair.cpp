/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2012 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 */

#include "dsmcc_api.h"
#include "hbbtv_api.h"

#include <iostream>
#include <sstream>
#include <directfb.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <assert.h>

#ifdef CORSAIR_SEGV_HANDLER_WITH_TRACEBACK
# include <execinfo.h>
#endif

#include "corsair.h"
#ifdef OIPF
#include "asteriks.h"
#endif
#include "corsair_env.h"
#include "corsair_gfx.h"
#include "corsair_utils.h"
#include "corsair_config.h"
#include "corsair_hbbtv_surface.h"
#ifdef OIPF
#include "oipf_surface.h"
#endif
#ifdef ENABLE_OPERA_TVSTORE
#include "corsair_otvs_surface.h"
#include "corsair_otvs.h"
#endif
#include "corsair_web_surface.h"
#include "corsair_security_manager.h"
#include "corsair_dsmcc_manager.h"
#include "corsair_hbbtv_manager.h"
#include "corsair_ait_manager.h"
#include "corsair_hinput.h"
#include "corsair_tools.h"
#if defined(CONFIG_OP_JCOM)
#include "corsair_ime_jlabs.h"
#else
#include "corsair_ime.h"
#endif
#include "corsair_debug.h"
#include "corsair_hama.h"
#include "corsair_postman.h"
#include "corsair_product_env.h"
#ifdef ENABLE_FPS_DISPLAYER
#include "corsair_fps_displayer.h"
#endif

#include "opera_action.h"

#include "hbbtv_api.h"
#include "gogi_opera_api.h"
#include "gogi_mem_api.h"

#include "bplinit.h"
#include "bplglobal.h"
#include "opllocalsystem.h"
#include "oplbroadcast.h"
#include "oplavoutput.h"
#include "oplappmgr.h"
#include "oplevent.h"
// WIDGET_LOCAL_MEDIA
#include "oplvod.h"
#include "oplvod_html5.h"   /* for MSE */
#include "opldial.h"        //
#include "oplvod_extapp.h"
#include "oplappmgr.h"

#include "vkernel.h"
#include "hlib.h"
#include "hapi.h"
#include "oapi.h"
#include "apk.h"
#include <sys/wait.h>

#if defined(CONFIG_OP_JCOM)
#include "opljlabslocalsystem.h"
#endif

#if defined(CONFIG_HUMAX_CRASHREPORT)
#include "hcrlib.h"
#endif

using namespace std;

#define MEGABYTE        1024 * 1024

/***********************************************************************************
 * debugging define.
 ***********************************************************************************/

/***********************************************************************************
 * static variables
 ***********************************************************************************/

Corsair* g_corsair = NULL;

bool Corsair::uses_vega = false;
bool Corsair::do_run = true;
Corsair *Corsair::s_theApp = NULL;

// for testing
int Corsair::m_frame_count = 0;
bool Corsair::m_count_frames = false;

struct timeval fps_start_time;
int time_ms(struct timeval *t) {
    return (t->tv_sec * 1000) + (t->tv_usec / 1000);
}
/***********************************************************************************
 * functions
 ***********************************************************************************/


/**
 * Corsair constructor.
 * @param argc argc from program start
 * @param argv argv from program start
 */
Corsair::Corsair(int argc, char** argv)
    : opera(NULL),
    focused_window(NULL),
    hbbtv_surface(NULL),
    web_surface(NULL),
    hbbtv_window_id(NULL),
    action_queue(NULL),
    m_security_manager(NULL),
#ifdef ENABLE_OPERA_TVSTORE
    m_otvs(NULL),
#endif
#ifdef ENABLE_FPS_DISPLAYER
    m_fps_displayer(NULL),
#endif
    m_hbbtv_manager(NULL)
{
    if ( s_theApp )
    {
        LOG_ERR("Fatal: Corsair was already created. Program exits.\n");
        exit(-1);
    }

    // Initialize logging. Add one output to output all from DEBUG level and up to stdout.
    // note : corsair use hxlog don't use corsair logging system
    //OperaHbbTV::Logging& logging = OperaHbbTV::Logging::getInstance();
    //OperaHbbTV::LoggingOutputStdout* output_stdout = new OperaHbbTV::LoggingOutputStdout(OperaHbbTV::LOG_LEVEL_DEBUG);
    //logging.addOutput(output_stdout);

    // Add another log output that writes warnings (and errors) to file.
    //OperaHbbTV::LoggingOutputFile* output_file = new OperaHbbTV::LoggingOutputFile(OperaHbbTV::LOG_LEVEL_DEBUG);
    //output_file->openFile("/tmp/corsair.log");
    //logging.addOutput(output_file);

    LOG_DEBUG("Starting corsair\n");
    //  setupSignals();

    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = childexit_signal;
    sigaction(SIGCHLD, &action, NULL);

    //Ignore BROKEN PIPE signal.
    signal(SIGPIPE, SIG_IGN);

    CorsairGfx *corsair_gfx = CorsairGfx::getInstance();
    if (corsair_gfx)
        corsair_gfx->init(argc, argv, corsair_flush_buffer);

    LOG_DEBUG("##########################################################\n");
    LOG_DEBUG("CorsairMain:: start to corsair\n");
    LOG_DEBUG("##########################################################\n");

    HLIB_EXTRA_DropRootPrivileges("corsair");

    LOG_DEBUG("##########################################################\n");
    LOG_DEBUG("CorsairMain:: changed to corsair\n");
    LOG_DEBUG("##########################################################\n");

    gettimeofday(&next_run_slice, NULL);

#if defined(CONFIG_OP_JCOM)
    m_Ime = new CorsairImeJlabs();
#else
    m_Ime = new CorsairIme();
#endif

    s_theApp = this;
}


/**
 * Corsair destructor.
 */
Corsair::~Corsair()
{

    deinitInput();

    destroyInternalObjects();

    CorsairGfx *corsair_gfx = CorsairGfx::getInstance();
    if (corsair_gfx)
        corsair_gfx->freeSurfaces();

    if ( action_queue)
    {
        delete action_queue;
        action_queue = NULL;
    }
    deinitHSDK();

    op_terminate_opera(opera);
    gogi_shutdown();

    if (corsair_gfx)
        corsair_gfx->shutdown();

    s_theApp = NULL;
}

/**
 * Initialization of things that must be done after creating the Corsair object.
 * The window creation here will cause a N_REQUEST_WINDOW notification to be sent
 * and the Corsair notification handler (above) needs the g_corsair variable to be
 * defined, which means that the window initialization below can't be done in then
 * Corsair constructor.
 *
 * @returns true if successful.
 */
bool Corsair::init()
{
    if ( !initProductEnv() )
    {
        LOG_ERR("Fatal: Setting Opera build information for product is failed. Program exits.\n");
        return false;
    }

    if ( !initEnv() )
    {
        LOG_ERR("Fatal: Setting Opera environment variabes is failed. Program exits.\n");
        return false;
    }

#ifndef USE_DIRECTFBGL
    initOpera();

    if (!initHSDK(opera))
        return false;
#endif

    action_queue = new OperaActionQueue();
    if ( !action_queue )
    {
        LOG_ERR("Cannot allocate OperaActionQueue\n");
    }

    initUserAgent();

    // init surfaces for hbbtv and open web
    CorsairGfx* corsair_gfx = CorsairGfx::getInstance();
    if (!corsair_gfx)
        return false;

    int screen_width = corsair_gfx->getScreenWidth();
    int screen_height = corsair_gfx->getScreenHeight();

#ifdef ENABLE_OPERA_TVSTORE
    otvs_surface = corsair_gfx->createOTVSSurface(0, 0, screen_width, screen_height);
    if (!otvs_surface)
        return false;
#endif
    hbbtv_surface = corsair_gfx->createHbbTVSurface(0, 0, screen_width, screen_height);
    if (!hbbtv_surface)
        return false;

#if 0
    web_surface = corsair_gfx->createWebSurface(0, 0, screen_width, screen_height);
    if (!web_surface)
        return false;
#endif

#ifdef USE_DIRECTFBGL
    initOpera();

    if (!initHSDK(opera))
        return false;
#endif

    if ( !createInternalObjects() )
    {
        LOG_ERR("Cannot allocate Internal Objects\n");
        return false;
    }

    if ( !initAIT() )
    {
        LOG_ERR("Cannot initialize AIT module\n");
        return false;
    }

    if ( !initInput() )
    {
        LOG_ERR("Cannot initialize Input module\n");
        return false;
    }

    // init opera hbbtv
    hbbtv_window_id = opera_hbbtv_create_window();
    opera_hbbtv_set_broadcast_window(hbbtv_window_id);
    if (!hbbtv_window_id)
        return false;

    showHbbTVSurface();

#if 0
    if ( CorsairProductEnv::getExistedWidget() )
    {
        LOG_DEBUG("This model have Widget\n");
        CorsairHama::initializeWidgets();
    }
#endif

#if 0
    showOpenWebSurface();
#endif
    CorsairTools::setPreferenceLanguage();
    CorsairTools::startDragonFlyWithNfsroot();

#if defined(CONFIG_DEBUG)
    CorsairDebugThread::start();
#endif

    char *ua_data = NULL;
    if(op_get_full_uastring(opera, &ua_data) != GOGI_STATUS_OK)
    {
        LOG_ERR("Cannot getting user agent data\n");
        return false;
    }
    LOG_DEBUG("====================================================================\n");
    LOG_DEBUG("==== OPERA UA : %s\n",ua_data);
    LOG_DEBUG("====================================================================\n");

    /* for MSE */
    OplVodHtmlFive::getResourceMap(NULL);

    return true;
}

bool Corsair::initHSDK(GogiOpera* opera)
{
    opera_hbbtv_init(opera);
    return opera_hbbtv_add_notification_listener(Corsair::notification_listener);
}

void Corsair::deinitHSDK()
{
    opera_hbbtv_shutdown();
}

HBBTV_EventData* Corsair::getUAString()
{
    HBBTV_EventData* ua_data = static_cast<HBBTV_EventData*>(malloc(sizeof(HBBTV_EventData)));
    if (!ua_data)
        return NULL;

    memset(ua_data, 0, sizeof(HBBTV_EventData));
#ifdef CONFIG_PROD_FVP4000T
    ua_data->ua_string.platform_hbbtv_version = PLATFORM_HBBTV_VERSION_1_3_1;
#else
    ua_data->ua_string.platform_hbbtv_version = PLATFORM_HBBTV_VERSION_1_2_1;
#endif
    ua_data->ua_string.vendor_name = strdup("Opera Software");
    ua_data->ua_string.model_name = strdup("HbbTV add-on");
    return ua_data;
}

/** Run Corsair main loop. */
void Corsair::run()
{
    while (do_run)
    {
        operaLoop();
        APK_EVENT_Dispatch();
    }
}

/** Stop running Corsair. */
void Corsair::stop()
{
    Corsair::do_run = false;
}


/** Setup POSIX signals to call our close_signal handler. */
void Corsair::setupSignals()
{
    struct sigaction action;
    action.sa_sigaction = close_signal;
    action.sa_flags = SA_SIGINFO;
    sigemptyset(&action.sa_mask);

    sigaction(SIGHUP, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGKILL, &action, NULL);
    sigaction(SIGQUIT, &action, NULL);

    action.sa_sigaction = abort_signal;
    sigaction(SIGABRT, &action, NULL);

    action.sa_sigaction = pipe_signal;
    sigaction(SIGPIPE, &action, NULL);

    /* The following signal handler enables the segv_handler function (below), which prints
     * debug info on segmentation faults. This can be used to debug segmentation faults.
     * This can be useful if it at some point is impossible or hard to use GDB for debugging. */

    action.sa_sigaction = segv_signal;
    sigaction(SIGSEGV, &action, NULL);
}


/**
 * Scan system for fonts.
 * @param path font path to scan
 */
void Corsair::scanFonts(const char* path)
{
    struct dirent* entry = NULL;
    DIR* dir = opendir(path);

    assert(dir);
    if (!dir)
        exit(-1);

    while ((entry = readdir(dir)) != NULL)
    {
        int len = strlen(entry->d_name);
        if ((strcasecmp(&entry->d_name[len - 4], ".ttf") == 0) ||
                (strcasecmp(&entry->d_name[len - 4], ".otf") == 0) ||
                (strcasecmp(&entry->d_name[len - 4], ".ttc") == 0) ||
                (strcasecmp(&entry->d_name[len - 4], ".ac3") == 0) ||
                (strcasecmp(&entry->d_name[len - 4], ".a3a") == 0) ||
                (strcasecmp(&entry->d_name[len - 4], ".a3b") == 0) ||
                (strcasecmp(&entry->d_name[len - 4], ".a3c") == 0) ||
                (strcasecmp(&entry->d_name[len - 4], ".a3d") == 0) ||
                (strcasecmp(&entry->d_name[len - 4], ".stf") == 0) ||
                (strcasecmp(&entry->d_name[len - 4], ".ccc") == 0))
        {
            ostringstream path_str;
            path_str << path;
            if (*(path_str.str().rbegin()) != '/')
                path_str << "/";
            path_str << entry->d_name;

            int status = gogi_add_font(path_str.str().c_str());
            if (status != GOGI_STATUS_OK)
                LOG_WARN("Warning: no font in file or other error.\n");
            else
                LOG_DEBUG("Added font: %s\n",path_str.str().c_str());
        }
    }

    closedir(dir);
}


void printMemory()
{
    printf("############### corsair Memory info #######################\n");
    printf("heap size : (%d)\n",op_get_heap_size());
    printf("allocated memory size : (%d)\n",op_get_allocated_memory());
    printf("VMRSS :\n"); 

    HCHAR procName[256]={0,};
    HCHAR line[256]={0,};
    FILE *fp;
    HxSTD_snprintf(procName, 256, "/proc/%d/status", HLIB_STD_GetPID());
    if (fp = fopen(procName, "r"), NULL == fp)
    {
        HLIB_CMD_Printf("error to open file : %s\n", procName);
        return;
    }

    while (fgets(line, 255, fp))
    {
        if (HLIB_STD_StrCaseStartWith(line, "Name") || HLIB_STD_StrCaseStartWith(line, "VmRSS") || HLIB_STD_StrCaseStartWith(line, "VmSize"))
        {
            printf("%s",line);
        }
    }
  
    fclose(fp);

    return;
}




void Corsair::operaLoop()
{
    static int delay = 0;

    // Perform the usual run_slicing once.
    struct timeval timeout_time;
    gettimeofday(&timeout_time, NULL);
    CorsairUtils::usec_increase(timeout_time, MAX_OP_RUN_SLICE_TIME);

#if 1
    struct timespec time;
    if ( 0 == clock_gettime( CLOCK_REALTIME, &time)) 
    {
        static int temp_time = time.tv_sec;
         
        if ( time.tv_sec  > temp_time + 60 )
        {
            printMemory();
            temp_time = time.tv_sec;
        }   
    }
#endif 

    while (true)
    {
        delay=op_run_slice(opera);
        if (delay > 0 || CorsairUtils::timed_out(timeout_time))
            break;
    }

    gettimeofday(&next_run_slice, NULL);
    CorsairUtils::usec_increase(next_run_slice, delay*1000);
    opera_hbbtv_tick();
    action_queue->executeAll();
    CorsairGfx::getInstance()->updateScreens();
    do
    {
        readInput(delay);
        m_Ime->imeLoop();
        m_post_man->run();
        m_hbbtv_manager->executeAllActions();

        //  delay == 0 --> corsair has job to do immediately
        //  delay == -1 --> corsair doesn't have any job for running
        //  delay > 0 --> op_slice_run will be called after time delay
        if (delay)
            VK_TASK_Sleep(delay > 10 ? 10 : delay);

        if (!CorsairUtils::timed_out(next_run_slice) && delay)
        {
            struct timeval now;
            gettimeofday(&now, NULL);
            delay = (next_run_slice.tv_sec * 1000)
                    + (next_run_slice.tv_usec / 1000)
                    - (now.tv_sec * 1000)
                    - (now.tv_usec / 1000);
        }
        else
            break;

        opera_hbbtv_tick();
        action_queue->executeAll();
        CorsairGfx::getInstance()->updateScreens();
    } while (!CorsairUtils::timed_out(next_run_slice) && do_run);
}

void Corsair::showHbbTVSurface()
{
    hbbtv_surface->showSurface();
    setFocusedWindow(hbbtv_surface->getWindow());
}

void Corsair::showOpenWebSurface()
{
    web_surface->showSurface();
    setFocusedWindow(web_surface->getWindow());
}

void Corsair::showOipfSurface()
{
}

#ifdef ENABLE_OPERA_TVSTORE
void Corsair::showOTVSSurface()
{
    hbbtv_surface->hideSurface();
    otvs_surface->showSurface();
    setFocusedWindow(otvs_surface->getFocusedWindow());
}
#endif

GogiOperaWindow* Corsair::getFocusedWindow()
{
    return g_corsair->focused_window;
}

void Corsair::setFocusedWindow(GogiOperaWindow* window)
{
    op_set_focus(window);
    focused_window = window;
}
/**
 * Request a Gogi window from integration. This is called by
 * notification handler when HbbTV framework has requested a
 * GogiOperaWindow. The request could be an arbitrary or a specific
 * window, depending on the value of *gogi_window.  If it is NULL,
 * then an arbitrary window is requested. If it is pointing to a
 * specific window, then access to that window is requested.
 *
 * @param gogi_window handle to a window. If it points to NULL an
 *                    arbitrary window is requested. Otherwise a
 *                    specific window is requested.  @param granted
 *                    set to true if access to a window is granted,
 *                    false otherwise.  @return false if request could
 *                    not be done.
 */
bool Corsair::requestGogiWindow(GogiOperaWindow** gogi_window, gogi_bool& granted, HBBTV_WindowIdentifier hbbtv_window_id)
{
    if (!hbbtv_surface)
        return false;

    *gogi_window = hbbtv_surface->requestWindow();

    granted = *gogi_window != NULL;
    if (granted)
    {
        LOG_DEBUG("Granting opera window to HbbTV \n");
        if (hbbtv_surface->isVisible())
            setFocusedWindow(*gogi_window);
    }
    else
        LOG_DEBUG("Rejecting request for opera window to HbbTV, window is already in use.\n");

    return true;
}


/**
 * Release a previously requested Gogi window. This is done when the
 * HbbTV framework has detected that a window no longer contains an
 * HbbTV application.
 *
 * @param gogi_window Handle to GogiOperaWindow to release.
 */
void Corsair::releaseGogiWindow(GogiOperaWindow* gogi_window)
{
    if (!hbbtv_surface->releaseWindow(gogi_window))
        return;

    if (hbbtv_surface->isVisible())
        setFocusedWindow(hbbtv_surface->getWindow());
}


/**
 * Show Gogi window. This is called by notification handler when HbbTV
 * framework has requested the hbbtv window (= hbbtv application) to
 * be shown in the HbbTV screen. We shall update the window visibility
 * state in the hbbtv screen.
 */
void Corsair::showGogiWindow(GogiOperaWindow* gogi_window)
{
    assert(opera_hbbtv_get_window_identifier(gogi_window));

    if (!gogi_window)
        // not hbbtv window, do nothing
        return;

    hbbtv_surface->showWindow(gogi_window);

    if (hbbtv_surface->isVisible())
        setFocusedWindow(hbbtv_surface->getWindow());
}


/**
 * Hide Gogi window. This is called by notification handler when HbbTV
 * framework has requested the hbbtv window (= hbbtv application) to
 * be hidden. We shall update the window visibility state in the hbbtv
 * screen.
 */
void Corsair::hideGogiWindow(GogiOperaWindow* gogi_window)
{
    assert(opera_hbbtv_get_window_identifier(gogi_window));

    if (!gogi_window)
        // not hbbtv window, do nothing
        return;

    hbbtv_surface->hideWindow(gogi_window);
}


// ************ Corsair static functions ***************************

/**
 * (static) signal handler for SIGHUP, SIGTERM and SIGINT.
 * @param signal received signal number.
 */


void Corsair::close_signal(int signal, siginfo_t *info, void *context)
{
    LOG_ERR("Close signal: %d\n",signal);
    g_corsair->stop();
}


void Corsair::abort_signal(int signal, siginfo_t *info, void *context)
{
    LOG_ERR("Abort signal: %d\n",signal);
}

void Corsair::pipe_signal(int signal, siginfo_t *info, void *context)
{
    LOG_ERR("Pipe signal: ingore... %d\n",signal);
}


/* Segmentation fault signal handler that prints debug info on stderr. */
void Corsair::segv_signal(int signal, siginfo_t *info, void *context)
{
    static const char *si_code_strings[3] = {"", "SEGV_MAPERR", "SEGV_ACCERR"};

    LOG_ERR("Segmentation fault:\n");
    LOG_ERR("info.si_signo: %d\n",signal);
    LOG_ERR("info.si_errno: %d\n",info->si_errno);
    LOG_ERR("info.si_code: %d(%s)\n",info->si_code,si_code_strings[info->si_code]);
    LOG_ERR("info.si_addr: 0x%x\n",info->si_addr);

#ifdef CORSAIR_SEGV_HANDLER_WITH_TRACEBACK
    void *buffer[100];
    char **strings;

    int nptrs = backtrace(buffer, 100);
    LOG_ERR("Backtrace (%d addresses):\n",nptrs);

    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL)
    {
        LOG_ERR("Failed making backtrace.\n");
    }
    else
    {
        for (int j = 0; j < nptrs; j++)
        {
            LOG_ERR("[%d] %s\n",j,strings[j]);
        }
        free(strings);
    }
#endif // CORSAIR_SEGV_HANDLER_WITH_TRACEBACK

    // Abort due to this segmentation fault.
    abort();
}

void Corsair::childexit_signal(int signal)
{
    pid_t child;
    int state;

    while((child = waitpid(-1, &state, WNOHANG)) > 0 )
    {
        LOG_ERR("exit child process id : %ld\n",child);
    }
}

void Corsair::corsair_flush_buffer(GogiRect* rects, int num_rects, GogiScreenHandle screen)
{
    void* s;
    op_get_screen_data(screen, &s);
    CorsairSurface* surface = (CorsairSurface*)s;

    if (!surface->isVisible())
        return;
    if (!surface->isOnScreen())
        return;

#ifdef ENABLE_FPS_DISPLAYER
    getCorsair()->m_fps_displayer->update();
#endif

    //LOG_DEBUG("Corsair::corsair_flush_buffer, currCount: " << m_frame_count;
    if (m_count_frames)
    {
        m_frame_count++;
        //LOG_DEBUG("[increase]m_frame_count++: " << m_frame_count;
    }
    CorsairGfx *corsair_gfx = CorsairGfx::getInstance();
    if (!corsair_gfx)
        return;

    int screen_width = corsair_gfx->getScreenWidth();
    int screen_height = corsair_gfx->getScreenHeight();

    DFBRegion update_region;
    update_region.x1 = screen_width;
    update_region.y1 = screen_height;
    update_region.x2 = 0;
    update_region.y2 = 0;

    for (int i = 0; i < num_rects; ++i)
    {
        int currX = surface->getCurrentXPos();
        int currY = surface->getCurrentYPos();

        int rectX1 = rects[i].x+currX;
        int rectX2 = rects[i].x+currX+rects[i].w - 1;
        int rectY1 = rects[i].y+currY;
        int rectY2 = rects[i].y+currY+rects[i].h - 1;

        if (rectX1 < 0)
            rectX1 = 0;

        if (rectY1 < 0)
            rectY1 = 0;

        if (rectX2 > screen_width)
            rectX2 = screen_width;

        if (rectY2 > screen_height)
            rectY2 = screen_height;

        if (rectX1 < update_region.x1)
            update_region.x1 = rectX1;

        if (rectY1 < update_region.y1)
            update_region.y1 = rectY1;

        if (rectX2 > update_region.x2)
            update_region.x2 = rectX2;

        if (rectY2 > update_region.y2)
            update_region.y2 = rectY2;

        DFBRectangle screenRect;
        screenRect.x = rects[i].x+currX;
        screenRect.y = rects[i].y+currY;
        screenRect.w = rects[i].w;
        screenRect.h = rects[i].h;

#if 0
        if (corsair_gfx)
            corsair_gfx->drawIntersection(surface, &screenRect);
#endif
    }

#if 0
    corsair_gfx->flip(&update_region);
#else   /* for multi-dfb_window */
    surface->flip(&update_region);
#endif
}


/**
 * (static) HbbTV library notification listener.
 * @param notification received notification type
 * @param data notification data for received notification
 */
HBBTV_STATUS Corsair::notification_listener(HBBTV_NotificationType notification, HBBTV_NotificationData* data)
{
    if ( g_corsair == NULL || g_corsair->getHbbTVManager() == NULL )
        return HBBTV_OK;
    
    Corsair *corsair = g_corsair;
    CorsairHbbTVManager *hbbtv_manager = g_corsair->getHbbTVManager();
        
    switch (notification)
    {
        case N_IS_TRUSTED:
            LOG_DEBUG("N_IS_TRUSTED:\n");
#if defined(NOTIFICATION_DEBUG)
            LOG_DEBUG(" -- notification_data->application.broadcastRelated = %d\n",data->application.broadcastRelated);
            LOG_DEBUG(" -- notification_data->application.orgId = 0x%x\n",data->application.orgId);
            LOG_DEBUG(" -- notification_data->application.appId = 0x%x\n",data->application.appId);
            LOG_DEBUG(" -- notification_data->application.url = %s\n",data->application.url);
            LOG_DEBUG(" -- notification_data->application.usage_type = 0x%x\n",(void*) data->application.usage_type);
            LOG_DEBUG(" -- notification_data->application.autostart_launching = %d\n",data->application.autostart_launching);
            LOG_DEBUG(" -- notification_data->application.trusted = %d\n",data->application.trusted);
#endif
            data->application.trusted = true;
            break;

        case N_APPLICATION_BROADCAST_RELATED_CHANGE:
        {
            LOG_DEBUG("N_APPLICATION_BROADCAST_RELATED_CHANGE:\n");
#if defined(NOTIFICATION_DEBUG)
            LOG_DEBUG(" -- notification_data->application.broadcastRelated = %d\n",data->application.broadcastRelated);
            LOG_DEBUG(" -- notification_data->application.orgId = 0x%x\n",data->application.orgId);
            LOG_DEBUG(" -- notification_data->application.appId = 0x%x\n",data->application.appId);
            LOG_DEBUG(" -- notification_data->application.url = %s\n",data->application.url);
            LOG_DEBUG(" -- notification_data->application.usage_type = 0x%x\n",(void*) data->application.usage_type);
            LOG_DEBUG(" -- notification_data->application.autostart_launching = %d\n",data->application.autostart_launching);
            LOG_DEBUG(" -- notification_data->application.trusted = %d\n",data->application.trusted);
#endif
            hbbtv_manager->action(new CorsairHbbTVUpdateAction(&data->application, hbbtv_manager));
        }
            break;

        case N_START_APPLICATION:
        {
            LOG_DEBUG("N_START_APPLICATION %s\n",data->application.url);
#if defined(NOTIFICATION_DEBUG)
            LOG_DEBUG(" -- application->hbbtv_window 0x%x\n",data->application.hbbtv_window);
            LOG_DEBUG(" -- window->hbbtv_window 0x%x\n",data->window.hbbtv_window);
            LOG_DEBUG(" -- window->gogi_window 0x%x\n",data->window.gogi_window);
#endif
            hbbtv_manager->action(new CorsairHbbTVStartedAction(&data->application, hbbtv_manager));

            char *ua_string = NULL;
            op_get_full_uastring(g_corsair->getOpera(), &ua_string);
            OplVod::setApplicationUrl(data->application.url);
            OplVod::setUseragent(ua_string);
        }
            LOG_DEBUG("@@ memory check @@ heap  size: %d ( %d MB )\n",op_get_heap_size(),op_get_heap_size()/(1024*1024));
            LOG_DEBUG("@@ memory check @@ allocated memory size: %d ( %d MB )\n",op_get_allocated_memory(),op_get_allocated_memory()/(1024*1024));
            break;

        case N_STOP_APPLICATION:
        {
            LOG_DEBUG("N_STOP_APPLICATION %s\n",data->application.url);
#if defined(NOTIFICATION_DEBUG)
            LOG_DEBUG(" -- application->hbbtv_window 0x%x\n",data->application.hbbtv_window);
            LOG_DEBUG(" -- window->hbbtv_window 0x%x\n",data->window.hbbtv_window);
            LOG_DEBUG(" -- window->gogi_window 0x%x\n",data->window.gogi_window);
#endif
            hbbtv_manager->action(new CorsairHbbTVStoppedAction(&data->application, hbbtv_manager));

            // nagra PRM check
            if( hbbtv_manager->getNagraPRMStatus() )
            {
                OplOIPFApplicationManager::setNagraPrmStopped();
                hbbtv_manager->setNagraPRMStatus(false);
            }
        }
            LOG_DEBUG("@@ memory check @@ heap  size: %d ( %d MB )\n",op_get_heap_size(),op_get_heap_size()/(1024*1024));
            LOG_DEBUG("@@ memory check @@ allocated memory size: %d ( %d MB )\n",op_get_allocated_memory(),op_get_allocated_memory()/(1024*1024));
            break;

        case N_AIT_UPDATED:
        {
            LOG_DEBUG("N_AIT_UPDATED: A new %s has been loaded.\n",(data->ait_updated.xml_ait ? "XML AIT" : "AIT"));
#if defined(CONFIG_PROD_FVP4000T)
            int hbbtv_keyset = 0;
            if( data->ait_updated.xml_ait == true )
            {
                CorsairHbbTVApp *running_bi_app = NULL;
                running_bi_app = hbbtv_manager->getRunningApplication(CorsairHbbTVApp::HBBTV_APPTYPE_BI);
                if( (running_bi_app != NULL) && (running_bi_app->m_is_started_mheg) )
                {
                    if( data->ait_updated.broadcast_related_from_xml_ait )
                    {
                        hbbtv_keyset = 1;
                        *data->ait_updated.broadcast_related_from_xml_ait = hbbtv_manager->isBroadcastRelatedApplicationFromXMLAIT(&data->ait_updated);
                        LOG_DEBUG("Set BR flag = %d\n",*data->ait_updated.broadcast_related_from_xml_ait);
                    }
                }
                else
                {
                     *data->ait_updated.broadcast_related_from_xml_ait = false;
                }
            }
            /*  BBC CRB - for preventng passing 1st back key to OIPF by HbbTV 1.3.1 keyset */
            op_set_pref_int(getCorsair()->getOpera(), "User Prefs", "Exclude HbbTV 1.3.1", hbbtv_keyset);
#endif
            if (data->ait_updated.app_count > 0)
            {
                LOG_DEBUG("The new AIT contains applications:\n");
                for (int i = 0; i < data->ait_updated.app_count; i++)
                {
                    LOG_DEBUG("%d. %s : \n",i+1,data->ait_updated.applications[i].application_name);
#if defined(NOTIFICATION_DEBUG)
                    LOG_DEBUG("\t  - orgId : 0x%x\n",data->ait_updated.applications[i].orgId);
                    LOG_DEBUG("\t  - appId : 0x%x\n",data->ait_updated.applications[i].appId);
                    LOG_DEBUG("\t  - ccontrol_code : %d\n",data->ait_updated.applications[i].control_code);
                    LOG_DEBUG("\t  - usage_type : 0x%x\n",data->ait_updated.applications[i].usage_type);
#endif
                    LOG_DEBUG("-----\n");
                    int y = data->ait_updated.applications[i].url_count;
                    if (y <= 0)
                        LOG_DEBUG("  no urls\n");
                    else
                        while (y-- > 0)
                        {
                            LOG_DEBUG("  url %d : %s\n",data->ait_updated.applications[i].url_count-y,data->ait_updated.applications[i].urls[y]);
                        }
                    LOG_DEBUG("-----\n");
                }

                if ( hbbtv_manager )
                {
                    hbbtv_manager->action(new CorsairHbbTVClearAppListAction(hbbtv_manager));
                    hbbtv_manager->action(new CorsairHbbTVNewAITAction(&data->ait_updated, hbbtv_manager));
                }

            }
            else
            {
                LOG_DEBUG("The new AIT does *not* contain any application.\n");
            }
            return HBBTV_OK;
        }
            break;

        case N_REQUEST_WINDOW:
            // If data->window.gogi_window is given (not null) then that specific window is requested.
            LOG_DEBUG("N_REQUEST_WINDOW\n");
#if defined(NOTIFICATION_DEBUG)
            LOG_DEBUG(" -- application->hbbtv_window 0x%x\n",data->application.hbbtv_window);
            LOG_DEBUG(" -- window->hbbtv_window 0x%x\n",data->window.hbbtv_window);
            LOG_DEBUG(" -- window->gogi_window 0x%x\n",data->window.gogi_window);
#endif
            if (corsair->requestGogiWindow(data->window.gogi_window, data->window.granted, data->window.hbbtv_window))
                return HBBTV_OK;
            else
                return HBBTV_ERROR;
            break;

        case N_RELEASE_WINDOW:
            LOG_DEBUG("N_RELEASE_WINDOW\n");
#if defined(NOTIFICATION_DEBUG)
            LOG_DEBUG(" -- application->hbbtv_window 0x%x\n",data->application.hbbtv_window);
            LOG_DEBUG(" -- window->hbbtv_window 0x%x\n",data->window.hbbtv_window);
            LOG_DEBUG(" -- window->gogi_window 0x%x\n",data->window.gogi_window);
#endif
            corsair->releaseGogiWindow(*data->window.gogi_window);

            return HBBTV_OK;

        case N_SHOW_WINDOW:
            LOG_DEBUG("N_SHOW_WINDOW\n");
            if (!data->window.gogi_window)
                return HBBTV_ERROR;
            corsair->getActionQueue()->add(new ShowHbbTVWindowAction(*data->window.gogi_window, g_corsair));

#if defined(CONFIG_PROD_FVP4000T)
            //  Test DB8-COEX-N0-0430 Application exit MHEG->AIT success
            if ( getCorsair()->hbbtv_surface->findWindow(*data->window.gogi_window) )
            {
                if ( hbbtv_manager && hbbtv_manager->isRunningApplication())
                {
                    CorsairTools::mhegOff();
                    CorsairTools::mhegBlock();
                }
                else{
                    CorsairTools::mhegOn();
                    CorsairTools::mhegUnBlock();
                }
            }
#endif
            break;

        case N_HIDE_WINDOW:
            LOG_DEBUG("N_HIDE_WINDOW\n");
            if (!data->window.gogi_window)
                return HBBTV_ERROR;
            corsair->getActionQueue()->add(new HideHbbTVWindowAction(*data->window.gogi_window, g_corsair));
            break;

        case N_KEYSET_CHANGED:
            LOG_DEBUG("N_KEYSET_CHANGED\n");
            LOG_DEBUG(" -- New keyset: 0x%x\n",data->keyset.key_mask);
            break;

        case N_GET_FREE_MEMORY:
            LOG_DEBUG("N_GET_FREE_MEMORY\n");
            // Using hard coded -1 as response, which according to OOIF spec means that
            // the value was not available. Change this to correct value if available.
            *data->free_memory.free_memory = (long)-1;
            break;

        case N_XML_HTTP_REQUEST:
            LOG_DEBUG("N_XML_HTTP_REQUEST\n");
            break;

        default:
            break;
    }

    return HBBTV_OK;
}


/**
 * (static) Gogi notification handler
 */
int Corsair::opera_handle_notification(GogiOpera* opera, GogiOperaWindow* opera_window, GOGI_OPERA_EVENT event, GogiOperaEventData* data)
{
    if (!Corsair::do_run)
        return GOGI_STATUS_OK;
    if (!g_corsair)
        return GOGI_STATUS_INVALID_PARAM;

    int status = 0;
    bool handled = false;

    if ( event == GOGI_OPERA_EVT_XHR )
    {
#if defined(CONFIG_PROD_FVP4000T)
        if( strstr(data->xhr.target,".bbci.co.uk") || strstr(data->xhr.target,".digitaltv-labs-ssl-test.co.uk") ||
                strstr(data->xhr.target,"hbbtv1.test") || strstr(data->xhr.source,"hbbtv1.test") ||
                strstr(data->xhr.target,"hbbtv2.test") || strstr(data->xhr.source,"hbbtv2.test"))
        {
            data->xhr.permission = GOGI_XHR_AUTO;
            if( strstr(data->xhr.target,"dvb://") )
            {
                data->xhr.permission = GOGI_XHR_ALLOW;
            }
        }
        else
        {
            data->xhr.permission = GOGI_XHR_ALLOW;
        }
#else
        data->xhr.permission = GOGI_XHR_ALLOW;
#endif
        LOG_DEBUG("GOGI_OPERA_EVT_XHR : %s, permission: %s\n",data->xhr.target,(data->xhr.permission==GOGI_XHR_ALLOW?"ALLOW":"AUTO"));
        return GOGI_STATUS_OK;
    }

    handled = opera_hbbtv_handle_notification(opera, opera_window, event, data, &status);
    if (handled)
        return GOGI_STATUS_OK;

#ifdef ENABLE_OPERA_TVSTORE
    handled = g_corsair->getOTVSHandle()->otvs_handle_notification(opera, opera_window, event, data);
    if (handled)
    {
        LOG_DEBUG(" This event is handled on otvs_handle_notification\n");
        return GOGI_STATUS_OK;
    }
#endif

    switch (event)
    {
        case GOGI_OPERA_EVT_SCREEN_INVALID:
            {
                void* tmp;
                op_get_screen_data(data->screen.screen_handle, &tmp);
                if (tmp)
                {
                    CorsairSurface* surface = static_cast<CorsairSurface*>(tmp);
                    surface->setScreenInvalid();
                }
                return GOGI_STATUS_OK;
            }

        case GOGI_OPERA_EVT_YIELD:
            {
                if (data->schedule.next_timeout >= 0)
                {
                    //corsair_read_input(data->schedule.next_timeout);
                    CorsairGfx::getInstance()->updateScreens();
                    return GOGI_STATUS_OK;
                }
                return GOGI_STATUS_INVALID_PARAM;

            }
            return GOGI_STATUS_OK;
        case GOGI_OPERA_EVT_COOKIE_MANAGER_NOTIFICATION:
            LOG_DEBUG("GOGI_OPERA_EVT_COOKIE_MANAGER_NOTIFICATION \n");
#if defined(NOTIFICATION_DEBUG)
            LOG_DEBUG("cookie_man.save_success : %d\n",data->cookie_man.save_success);
            LOG_DEBUG("cookie_man.requested_by_platform : %d\n",data->cookie_man.requested_by_platform);
#endif
            CorsairTools::syncCookie();
            return GOGI_STATUS_OK;

        case GOGI_OPERA_EVT_IMECREATE:
            LOG_DEBUG("GOGI_OPERA_EVT_IMECREATE (text: %s) - x, y = %d, %d \n",data->ime.text,data->ime.rect.x,data->ime.rect.y);
            g_corsair->getIme()->activate(data->ime.imeID, data->ime.password, 0, data->ime.format, data->ime.text, data->ime.maxlength);
            return GOGI_STATUS_OK;

        case GOGI_OPERA_EVT_IMECANCEL:
            LOG_DEBUG("GOGI_OPERA_EVT_IMECANCEL\n");
            g_corsair->getIme()->deactivate();
            return GOGI_STATUS_OK;

        case GOGI_OPERA_EVT_IME_TEXT_UPDATE:
            LOG_DEBUG("GOGI_OPERA_EVT_IME_TEXT_UPDATE\n");
            return GOGI_STATUS_OK;

        case GOGI_OPERA_EVT_ASYNC_DIALOG:
            g_corsair->getActionQueue()->add(new NotificationAction(opera, opera_window, event, data));
            return GOGI_STATUS_OK;

        case GOGI_OPERA_EVT_AUTHENTICATION:
            LOG_DEBUG("GOGI_OPERA_EVT_AUTHENTICATION Server name: %s\n",data->authentication.server_name);
            LOG_DEBUG(" -- Server port: %d\n",data->authentication.server_port);
            LOG_DEBUG(" -- Last username: %s\n",data->authentication.last_username);
            LOG_DEBUG(" -- Message: %s\n",data->authentication.message);
            LOG_DEBUG(" -- Authentication name: %s\n",data->authentication.authentication_name);
            LOG_DEBUG(" -- Security level: %d\n",data->authentication.security_level);
            LOG_DEBUG(" -- Url: %s\n",data->authentication.url);
            LOG_DEBUG(" -- Note: Corsair is just a reference integration which doesn't implement digest authentication. In order to implement it, please see the Opera Linux SDK documentation.\n");
            return GOGI_STATUS_INVALID_PARAM;

        case GOGI_OPERA_EVT_DOCUMENT_TITLE:
            LOG_DEBUG("GOGI_OPERA_EVT_DOCUMENT_TITLE Title: %s\n",data->title.title);
            return GOGI_STATUS_OK;

        case GOGI_OPERA_EVT_DOCUMENT_ICON:
            LOG_DEBUG("GOGI_OPERA_EVT_DOCUMENT_ICON Available: %d\n",data->icon.isAvailable);
            return GOGI_STATUS_OK;

        case GOGI_OPERA_EVT_SOFT_OUT_OF_MEMORY:
            LOG_DEBUG("GOGI_OPERA_EVT_SOFT_OUT_OF_MEMORY\n");
            LOG_DEBUG("heap size: %d ( %d MB )\n",op_get_heap_size(),op_get_heap_size()/(1024*1024));
            LOG_DEBUG("allocated memory size: %d ( %d MB )\n",op_get_allocated_memory(),op_get_allocated_memory()/(1024*1024));
#ifdef ENABLE_ABORT_IF_OUT_OF_MEMORY
            CorsairTools::doAbort();
#endif                     
            return GOGI_STATUS_OK;

        case GOGI_OPERA_EVT_OUT_OF_MEMORY:
            LOG_DEBUG("GOGI_OPERA_EVT_OUT_OF_MEMORY\n");
            LOG_DEBUG("heap size: %d ( %d MB )\n",op_get_heap_size(),op_get_heap_size()/(1024*1024));
            LOG_DEBUG("allocated memory size: %d ( %d MB )\n",op_get_allocated_memory(),op_get_allocated_memory()/(1024*1024));
#ifdef ENABLE_ABORT_IF_OUT_OF_MEMORY
            CorsairTools::doAbort();
#endif            
            return GOGI_STATUS_OK;

        case GOGI_OPERA_EVT_EXTERNAL_ACTION:
            LOG_DEBUG("GOGI_OPERA_EVT_EXTERNAL_ACTION Action: %s\n",data->external_action.action);
            LOG_DEBUG(" -- Parameter: %s\n",data->external_action.parameter);
            LOG_DEBUG(" -- Poll: %d\n",data->external_action.poll);
            return GOGI_STATUS_OK;


        case GOGI_OPERA_EVT_LOADING_FINISHED:
            LOG_DEBUG("GOGI_OPERA_EVT_LOADING_FINISHED Message: %s\n",data->loading.message);
            LOG_DEBUG(" -- http response code: %d\n",data->loading.http_response_code);
            
            LOG_DEBUG("@@ memory check @@ heap  size: %d ( %d MB )\n",op_get_heap_size(),op_get_heap_size()/(1024*1024));
            LOG_DEBUG("@@ memory check @@ allocated memory size: %d ( %d MB )\n",op_get_allocated_memory(),op_get_allocated_memory()/(1024*1024));
            return GOGI_STATUS_OK;


        case GOGI_OPERA_EVT_NEED_RESCHEDULE:
            LOG_DEBUG("GOGI_OPERA_EVT_NEED_RESCHEDULE:\n");
            gettimeofday(&g_corsair->next_run_slice, NULL);
            CorsairUtils::usec_increase(g_corsair->next_run_slice, data->schedule.next_timeout*1000);
            return GOGI_STATUS_OK;


        case GOGI_OPERA_EVT_LOADING_START:
            LOG_DEBUG("GOGI_OPERA_EVT_LOADING_START url: %s\n",data->url.url);
            /* we store cookie file when the last page loaded */
            CorsairTools::syncCookie();

            if ( !g_corsair || !g_corsair->getHbbTVSurface() ) break;

            if ( g_corsair->getHbbTVSurface()->findWindow(opera_window) ){
                OplApplicationManager::setApplicationWindow(OPLWINDOW_HBBTV);
                g_corsair->getHbbTVSurface()->setWindowUrl(opera_window, data->url.url);
            }
            else{
                OplApplicationManager::setApplicationWindow(OPLWINDOW_OIPF);
                if(g_corsair->getOipfSurface())
                	g_corsair->getOipfSurface()->setWindowUrl(opera_window, data->url.url);
            }
            LOG_DEBUG("@@ memory check @@ heap  size: %d ( %d MB )\n",op_get_heap_size(),op_get_heap_size()/(1024*1024));
            LOG_DEBUG("@@ memory check @@ allocated memory size: %d ( %d MB )\n",op_get_allocated_memory(),op_get_allocated_memory()/(1024*1024));

            return GOGI_STATUS_OK;

        case GOGI_OPERA_EVT_START_RENDERING:
            LOG_DEBUG("GOGI_OPERA_EVT_START_RENDERING\n");
            return GOGI_STATUS_OK;

        case GOGI_OPERA_EVT_LOADING_PROGRESS:
#if 0//defined(NOTIFICATION_DEBUG)
            LOG_DEBUG("GOGI_OPERA_EVT_LOADING_PROGRESS message: %s\n",data->loading.message);
            LOG_DEBUG("GOGI_OPERA_EVT_LOADING_PROGRESS byte_count : %d\n",data->loading.byte_count);
            LOG_DEBUG("GOGI_OPERA_EVT_LOADING_PROGRESS loaded_byte_count : %d\n",data->loading.loaded_byte_count);
            LOG_DEBUG("GOGI_OPERA_EVT_LOADING_PROGRESS document_bytes_total : %d\n",data->loading.document_bytes_total);
            LOG_DEBUG("GOGI_OPERA_EVT_LOADING_PROGRESS document_bytes_loaded : %d\n",data->loading.document_bytes_loaded);
            LOG_DEBUG("GOGI_OPERA_EVT_LOADING_PROGRESS has_requested_document : %d\n",data->loading.has_requested_document);
            LOG_DEBUG("\n");
            LOG_DEBUG("\n");
            LOG_DEBUG("GOGI_OPERA_EVT_LOADING_PROGRESS message: %s\n",data->loading.message);
#endif
            return GOGI_STATUS_OK;

        case GOGI_OPERA_EVT_DOCUMENT_SIZE:
            //LOG_DEBUG("GOGI_OPERA_EVT_DOCUMENT_SIZE\n");
            return GOGI_STATUS_OK;

        case GOGI_OPERA_EVT_URL_CHANGE:
            LOG_DEBUG("GOGI_OPERA_EVT_URL_CHANGE\n");
            return GOGI_STATUS_OK;

        case GOGI_OPERA_EVT_LOADING_FAILED:
            LOG_DEBUG("GOGI_OPERA_EVT_LOADING_FAILED\n");
#if defined(NOTIFICATION_DEBUG)
            LOG_DEBUG("    image_count            : %d\n",data->loading.image_count);
            LOG_DEBUG("    loaded_image_count     : %d\n",data->loading.loaded_image_count);
            LOG_DEBUG("    byte_count             : %d\n",data->loading.byte_count);
            LOG_DEBUG("    loaded_byte_count      : %d\n",data->loading.loaded_byte_count);
            LOG_DEBUG("    document_bytes_total   : %d\n",data->loading.document_bytes_total);
            LOG_DEBUG("    document_bytes_loaded  : %d\n",data->loading.document_bytes_loaded);
            LOG_DEBUG("    has_requested_document : %d\n",data->loading.has_requested_document);
            LOG_DEBUG("    message                : %s\n",data->loading.message);
            LOG_DEBUG("    msg_id                 : %d\n",data->loading.msg_id);
            LOG_DEBUG("    uploading              : %d\n",data->loading.uploading);
            LOG_DEBUG("    state                  : %d\n",data->loading.state);
            LOG_DEBUG("    http_response_code     : %d\n",data->loading.http_response_code);
            LOG_DEBUG("    current_percentage     : %d\n",data->loading.current_percentage);
            LOG_DEBUG("    content_type           : %s\n",data->loading.content_type);
#endif
            return GOGI_STATUS_OK;

        case GOGI_OPERA_EVT_ERROR:
            {
                LOG_DEBUG("GOGI_OPERA_EVT_ERROR\n");
#if defined(NOTIFICATION_DEBUG)
                LOG_DEBUG("    document_error      : %d\n",data->error.document_error);
                LOG_DEBUG("    file_error          : %d\n",data->error.file_error);
                LOG_DEBUG("    http_error          : %d\n",data->error.http_error);
                LOG_DEBUG("    dsmcc_error         : %d\n",data->error.dsmcc_error);
                LOG_DEBUG("    tls_error           : %d\n",data->error.tls_error);
                LOG_DEBUG("    network_error       : %d\n",data->error.network_error);
                LOG_DEBUG("    boundary_error      : %d\n",data->error.boundary_error);
                LOG_DEBUG("    redirection_error   : %d\n",data->error.redirection_error);
#endif

                CorsairHbbTVManager *hbbtv_manager = g_corsair->getHbbTVManager();

                /* the error popup should be displayed when TV Portal running */
                if ( data->error.network_error )
                {
                    g_corsair->getActionQueue()->add(new NotificationAction(opera, opera_window, event, data));
                }

                return GOGI_STATUS_OK;
            }

#if defined(NOTIFICATION_DEBUG)
        /* this is to skip print going out to rs232 */
        case GOGI_OPERA_EVT_WINDOW_RESIZE:
            break;
#endif


#if defined(NOTIFICATION_DEBUG) /* belows are ciritical event */
        case GOGI_OPERA_EVT_OUT_OF_DISK:
            LOG_DEBUG("GOGI_OPERA_EVT_OUT_OF_DISK\n");
            break;

        case GOGI_OPERA_EVT_PLUGIN_EVENT:
            LOG_DEBUG("GOGI_OPERA_EVT_PLUGIN_EVENT\n");
            break;

        case GOGI_OPERA_EVT_WIDGET_STARTED:
            LOG_DEBUG("GOGI_OPERA_EVT_WIDGET_STARTED\n");
            break;

        case GOGI_OPERA_EVT_WIDGET_UPDATED:
            LOG_DEBUG("GOGI_OPERA_EVT_WIDGET_UPDATED\n");
            break;

        case GOGI_OPERA_EVT_WIDGET_UNINSTALLED:
            LOG_DEBUG("GOGI_OPERA_EVT_WIDGET_UNINSTALLED\n");
            break;

        case GOGI_OPERA_EVT_WIDGET_INSTALLED:
            LOG_DEBUG("GOGI_OPERA_EVT_WIDGET_INSTALLED\n");
            break;

        case GOGI_OPERA_EVT_WIDGET_START_FAILED:
            LOG_DEBUG("GOGI_OPERA_EVT_WIDGET_START_FAILED\n");
            break;

        case GOGI_OPERA_EVT_WIDGET_STOPPED:
            LOG_DEBUG("GOGI_OPERA_EVT_WIDGET_STOPPED\n");
            break;

        case GOGI_OPERA_EVT_CHECK_URL_ALLOWED:
            LOG_DEBUG("GOGI_OPERA_EVT_CHECK_URL_ALLOWED\n");
            break;

        case GOGI_OPERA_EVT_URL_BLOCKED:
            LOG_DEBUG("GOGI_OPERA_EVT_URL_BLOCKED\n");
            break;

        case GOGI_OPERA_EVT_TOPMOST:
            LOG_DEBUG("GOGI_OPERA_EVT_TOPMOST 0x%x\n",opera_window);
            break;

        case GOGI_OPERA_EVT_NOT_TOPMOST:
            LOG_DEBUG("GOGI_OPERA_EVT_NOT_TOPMOST 0x%x\n",opera_window);
            break;

        case GOGI_OPERA_EVT_OUT_OF_VIDEO_MEMORY:
            LOG_DEBUG("GOGI_OPERA_EVT_OUT_OF_VIDEO_MEMORY\n");
            break;
#endif
        case GOGI_OPERA_EVT_PLATFORM_MESSAGE:
            if(strcmp(data->platform_message.message, "startFPS") == 0)
            {
                LOG_DEBUG("[+]startFPS\n");
                g_corsair->m_frame_count = 0;
                g_corsair->m_count_frames = true;
                gettimeofday(&fps_start_time, NULL);
                data->platform_message.callback(data->platform_message.id, 0, "", "");
                LOG_DEBUG("[-]startFPS\n");
                return GOGI_STATUS_OK;
            }
            else if(strcmp(data->platform_message.message, "stopFPS") == 0)
            {
                char s[10];

                LOG_DEBUG("[+]stopFPS\n");
                struct timeval current_time;
                gettimeofday(&current_time, NULL);
                int nr_frames = g_corsair->m_frame_count;
                g_corsair->m_count_frames = false;
                float fps = (nr_frames*1000.0)/(time_ms(&current_time) - time_ms(&fps_start_time));
                snprintf(s, sizeof(s), "%f", fps);
                LOG_DEBUG("stopFPS FPS: %s\n",s);
                data->platform_message.callback(data->platform_message.id, 0, s, "");
                LOG_DEBUG("[-]stopFPS\n");
                return GOGI_STATUS_OK;
            }
            break;
        default:
#if defined(NOTIFICATION_DEBUG)
        LOG_DEBUG("Unknown Event : %d\n",event);
#endif
            break;
    }
    return GOGI_STATUS_INVALID_PARAM;
}

void Corsair::initOpera()
{
    CorsairGfx* corsair_gfx = CorsairGfx::getInstance();
    if (!corsair_gfx)
        return;

#ifdef USE_VEGA_DFB
    const char* use_vega = getenv("USE_VEGA_DFB");
    if(use_vega && strcmp(use_vega, "YES") == 0)
    {
        LOG_DEBUG("Using accelerated painter.\n");
        if (GOGI_STATUS_OK != gogi_init_dfb(corsair_gfx->getDfbInstance()))
        {
            LOG_ERR("ERROR: Could not init GOGI.\n");
        }
        uses_vega = true;
    }
    else
    {
        LOG_DEBUG("Using software painter.\n");
        int ret = gogi_init();
        if (GOGI_STATUS_OK != ret)
        {
            LOG_ERR("ERROR: Could not init GOGI.[%d]\n",ret);
        }
    }
#elif USE_DIRECTFBGL
    if(GOGI_STATUS_OK != gogi_init_opengl())
    {
        LOG_ERR("ERROR: Could not init GOGI(opengl)\n");
    }
#else
    int ret = gogi_init();
    if (GOGI_STATUS_OK != ret)
    {
        LOG_ERR("ERROR: Could not init GOGI.[%d]\n",ret);
    }
#endif

    char* fontdir = NULL;
    fontdir = getenv("OPERA_FONTS");
    if (fontdir == NULL)
    {
        LOG_ERR("No font directory found.\n");
        LOG_ERR("Please set the environment variable OPERA_FONTS to a directory containing valid fonts.\n");
        exit(-1);
    }
    else
    {
        scanFonts(fontdir);
    }

    struct GOGI_GenericFonts fonts;
    CorsairUtils::set_default_fonts(fonts);

    if (op_create_opera(&opera, opera_handle_notification, &fonts, NULL) != GOGI_STATUS_OK)
    {
        LOG_ERR("Failed to init Opera\n");
        gogi_shutdown();
        exit(-1);
    }

    corsair_gfx->setOpera(opera);

    if (GOGI_STATUS_OK != op_add_protocol(opera, (char*)"dvb", true))
        LOG_ERR("Failed to add custom protocol 'dvb'\n");

    Memory_t *mem = CorsairProductEnv::getMemory();

    /* Limits the heap size for the Opera process */
    op_limit_heap_size(mem->heap_limit_size * MEGABYTE);

    /* The limit makes it possible for the Opera process to ensure that
     * Opera won't fill up the whole heap */
    op_limit_allocated_memory(mem->allocated_limit_size * MEGABYTE);
}

void Corsair::initUserAgent()
{
    char swversion[64], hwversion[64],ui_name[64], extension[64], model_name[64];
    int defaultDev = 0; // 0 = wired, 1 = wireless
    HBBTV_EventData* ua_data = static_cast<HBBTV_EventData*>(malloc(sizeof(HBBTV_EventData)));
    memset(swversion, 0, sizeof(swversion));
    memset(hwversion, 0, sizeof(hwversion));
    memset(extension, 0, sizeof(extension));
    memset(ui_name, 0, sizeof(ui_name));
    memset(model_name, 0, sizeof(model_name));

    OPL_LocalSystem_GetSoftwareVersion(swversion);
    OPL_LocalSystem_GetHardwareVersion(hwversion);
    BPL_GetHumaxUIString(ui_name, sizeof(ui_name));
    OPL_LocalSystem_GetModelName(model_name);
#if defined(CONFIG_PROD_FVP4000T)
    if( OPL_Network_GetDefaultDevIndex(&defaultDev) == BPL_STATUS_OK )
    {
        if( defaultDev == 0 )
            snprintf(extension, sizeof( extension ),"wired");
        else if( defaultDev == 1 )
            snprintf(extension, sizeof( extension ),"wireless");
    } else
#endif
    if( OPL_Network_GetDefaultDevIndex(&defaultDev) == BPL_STATUS_OK )
    {
        if( defaultDev == 0 )
            snprintf(extension, sizeof( extension ),"wired; %s" , ui_name);
        else if( defaultDev == 1 )
            snprintf(extension, sizeof( extension ),"wireless; %s" , ui_name);
    }
    else
    {
        snprintf(extension, sizeof( extension ) ,"%s" , ui_name);
    }
    //snprintf(extension, sizeof( extension ),"FVC/1.0(Sonic; TV14;)");

    ua_data->ua_string.vendor_name = strdup("Humax");
#ifdef CONFIG_PROD_FVP4000T
    ua_data->ua_string.capabilities = NULL;
    ua_data->ua_string.platform_hbbtv_version = PLATFORM_HBBTV_VERSION_1_3_1;    //refer to hbbtv_api.h
#else
    ua_data->ua_string.capabilities = strdup("+PVR");
    ua_data->ua_string.platform_hbbtv_version = PLATFORM_HBBTV_VERSION_1_2_1;    //refer to hbbtv_api.h
#endif

    Information_t *info = CorsairProductEnv::getInformation();

    if ( info->model_name == "" )
        //ua_data->ua_string.model_name = strdup(model_name);
        ua_data->ua_string.model_name = strdup(CONFIG_PRODUCT_NAME);
    else
        ua_data->ua_string.model_name = strdup(info->model_name.c_str());

    ua_data->ua_string.software_version = strdup(swversion);
    ua_data->ua_string.hardware_version = strdup(hwversion);
    ua_data->ua_string.reserved = strdup(extension);
#if defined(CONFIG_PROD_FVP4000T)
    char fvc_ext[512] = {0,};
    snprintf(fvc_ext, sizeof(fvc_ext),"ce-html/1.0 %s (%s; %s;)", OPERA_FVC_EXTENTION, ua_data->ua_string.vendor_name, info->model_name.c_str());
    ua_data->ua_string.suffix = strdup(fvc_ext);
#endif
#if defined(CONFIG_PROD_YS1000) || defined(CONFIG_PROD_YSR2000) || defined(CONFIG_PROD_HGS1000S) /* temporary impl for ysr*/
    ua_data->ua_string.suffix = strdup("ce-html/1.0");
#endif

    opera_hbbtv_send_event(E_SET_UASTRING, ua_data);

#ifdef ENABLE_OPERA_TVSTORE
    {
        string otvs_ext = OPERA_TVSTORE_EXTENTION;
        if ( NULL != CorsairTools::getUsbPath(RUNTIME_ENVFILE_TVSTORE_TESTPAGE) )
            otvs_ext.append(OPERA_TVSTORE_TESTDEVICE);
        else
            otvs_ext.append(ua_data->ua_string.model_name);
        op_set_uastring_suffix( opera, otvs_ext.c_str());
    }
#endif

    HBBTV_EventData* profile_data = static_cast<HBBTV_EventData*>(malloc(sizeof(HBBTV_EventData)));
    memset(profile_data, 0, sizeof(HBBTV_EventData));

    profile_data->profile.supported = PLATFORM_PROFILE_PVR;
    opera_hbbtv_send_event(E_SET_PLATFORM_PROFILE, profile_data);

}

bool Corsair::initProductEnv()
{
    if ( !CorsairProductEnv::initProductEnv() ) return false;

    return true;
}

bool Corsair::initEnv()
{
    if ( !CorsairEnv::initenv() ) return false;
    return CorsairEnv::renew();
}

bool Corsair::initAIT()
{
    return true;
}

bool Corsair::initInput()
{
#ifdef ENABLE_OPERA_TVSTORE
    CorsairInput *otvsInput = new CorsairOTVSInput(eAWINDOW_OTVS);
    if ( otvsInput )
        m_inputs.push_back(otvsInput);
    else
        LOG_DEBUG("OTVSInput instance creation is failed.\n");
#endif
#ifndef REMOVE_HBBTV_INPUT
    CorsairInput *hbbtvInput = new CorsairHbbTVInput(eAWINDOW_HBBTV);
    if ( hbbtvInput )
        m_inputs.push_back(hbbtvInput);
    else
        LOG_DEBUG("HbbTVInput instance creation is failed.\n");
#endif
    return true;
}

bool Corsair::deinitInput()
{
    std::vector<CorsairInput *>::iterator it;
    for ( it = m_inputs.begin(); it != m_inputs.end(); ++it )
        delete *it;
    m_inputs.clear();

    return true;
}

bool Corsair::readInput(int delay)
{
    std::vector<CorsairInput *>::iterator it;
    for ( it = m_inputs.begin(); it != m_inputs.end(); ++it )
    {
        (*it)->readInput(delay);
    }

    return true;
}

bool Corsair::createInternalObjects()
{
    m_security_manager = new CorsairSecurityManager();
    if ( m_security_manager )
    {
        if(0 > m_security_manager->loadCertification( ))
        {
            goto RETURN_ERROR;
        }
    }

    m_hbbtv_manager = new CorsairHbbTVManager();
    if ( !m_hbbtv_manager )
    {
        goto RETURN_ERROR;
    }

    m_post_man = new CorsairPostman();
    if ( !m_post_man )
    {
        LOG_DEBUG("Cannot allocate CorsairPostman\n");
        goto RETURN_ERROR;
    }
    
#ifdef ENABLE_OPERA_TVSTORE
    m_otvs = new CorsairOTVS(otvs_surface);
    if ( !m_otvs )
    {
        LOG_DEBUG("Cannot allocate CorsairPostman\n");
        goto RETURN_ERROR;
    }
#endif

#ifdef ENABLE_FPS_DISPLAYER
    m_fps_displayer = new CorsairFPSDisplayer();
    if ( !m_fps_displayer )
    {
        LOG_DEBUG("Cannot allocate CorsairPostman\n");
        goto RETURN_ERROR;
    }
#endif
    return true;

RETURN_ERROR:
    if ( NULL != m_security_manager )
        delete m_security_manager;

    if ( NULL != m_hbbtv_manager )
        delete m_hbbtv_manager;
        
    if ( NULL != m_post_man )
        delete m_post_man;
        
#ifdef ENABLE_OPERA_TVSTORE
    if ( NULL != m_otvs )
        delete m_otvs;
#endif

#ifdef ENABLE_FPS_DISPLAYER
    if ( NULL != m_fps_displayer )
        delete m_fps_displayer;
#endif

    return false;
}

bool Corsair::destroyInternalObjects()
{
    //CorsairHive::deinit();

    if ( m_post_man )
    {
        delete m_post_man;
        m_post_man = NULL;
    }

    if ( m_hbbtv_manager )
    {
        delete m_hbbtv_manager;
        m_hbbtv_manager = NULL;
    }

    if ( m_security_manager )
    {
        delete m_security_manager;
        m_security_manager = NULL;
    }

    if ( m_Ime )
    {
        delete m_Ime;
        m_Ime = NULL;
    }
#ifdef ENABLE_FPS_DISPLAYER
    if ( m_fps_displayer )
    {
        delete m_fps_displayer;
        m_fps_displayer = NULL;
    }
#endif

    return true;
}

static void printCertificatesFromManager(GogiOpera* opera, GOGI_OPERA_CERT_CATEGORY certificateCategory)
{
    int retval;
    GogiCertificateManager* certman;
    retval = op_create_certificate_manager(opera, &certman, certificateCategory);

    if(certman == NULL)
    {
        LOG_ERR("op_create_certificate_manager(%d) failed with error code == %d \n",certificateCategory,retval);
        return;
    }

    int numberOfCerts = 0;
    retval = op_get_number_of_certificates(certman, &numberOfCerts);

    if(retval != GOGI_STATUS_OK)
    {
        LOG_ERR("op_get_number_of_certificates failed with error code == %d \n",retval);
        return;
    }

    LOG_ERR("Certificates from GogiCertificateManager(%d):\n",certificateCategory);

    for(int i = 0; i < numberOfCerts; ++i)
    {
        GogiCertificate *cert = NULL;
        retval = op_get_certificate(certman, i, &cert);

        if(retval != GOGI_STATUS_OK) LOG_ERR("%d: error code == %d on op_get_certificate\n",i,retval);
        else if(cert == NULL) LOG_ERR("%d: cert is NULL\n",i);
        else if(cert->shortName == NULL) LOG_ERR("%d: shortname is NULL\n",i);
        else LOG_ERR("%d: %s\n",i,cert->shortName);
    }

    return;
}

static void printCertificatesFromDialogData(GOGI_DialogData *data)
{
    LOG_ERR("Certificates from GOGI_OPERA_PERSONAL_CERTIFICATES:\n");

    for(int i = 0; i <  data->certificate.numcerts; ++i)
    {
        LOG_ERR("%d: %s\n",i,data->certificate.cert[i].shortName);
    }

    return;

}

/**
 * Deferred GOGI event handler.
 *
 * Invoked from the action queue whenever we need to call GOGI
 * functions when dealing with a GOGI event, to avoid
 * re-entrancy. Works just like the usual GOGI event handler function.
 */

void Corsair::opera_handle_deferred_notification(GogiOpera* opera, GogiOperaWindow* opera_window, GOGI_OPERA_EVENT event, GogiOperaEventData* data)
{
    if(GOGI_OPERA_EVT_ASYNC_DIALOG == event)
    {
        LOG_DEBUG("GOGI_OPERA_EVT_ASYNC_DIALOG\n");
        LOG_DEBUG("Dialog message: %s\n",data->dialog.message);
        LOG_DEBUG("Dialog type: %d\n",data->dialog.type);
        gogi_dialog_callback_t callback = data->dialog.callback;
        void* id = data->dialog.id;

        switch(data->dialog.type)
        {
            /* Below routine for certification has to get valid date */
            case GOGI_DialogData::GOGI_DIALOG_TYPE_CERT_BROWSE:
            case GOGI_DialogData::GOGI_DIALOG_TYPE_CERT_PASSWD:
                if(true == g_corsair->getSecurityManager()->isAcceptable(data))
                {
                    callback(id, GOGI_DialogData::GOGI_DIALOG_BUTTON_OK, (char*)0, NULL);
                }
                else if(data->dialog.dialog_context == GOGI_DialogData::GOGI_DIALOG_CONTEXT_CERTBROWSE_NO_CHAIN ||
                        data->dialog.dialog_context == GOGI_DialogData::GOGI_DIALOG_CONTEXT_CERTBROWSE_WRONG_NAME ||
                        data->dialog.dialog_context == GOGI_DialogData::GOGI_DIALOG_CONTEXT_CERTBROWSE_EXPIRED)
                {
                    //do nothing
                }
                else
                {
                    callback(id, GOGI_DialogData::GOGI_DIALOG_BUTTON_CANCEL, (char*)0, NULL);
                }
                break;
            case GOGI_DialogData::GOGI_DIALOG_TYPE_CLIENT_CERT_SELECT:
            {
                LOG_DEBUG("Selecting a certificate (the first)\n");
                int cert_index = g_corsair->getSecurityManager()->CanSelect(data);
                if(0 <= cert_index)
                    callback(id, GOGI_DialogData::GOGI_DIALOG_BUTTON_OK, (char*)0, (char*)cert_index);
                else
                    callback(id, GOGI_DialogData::GOGI_DIALOG_BUTTON_CANCEL, (char*)0, (char*)0);
                break;
            }
            case GOGI_DialogData::GOGI_DIALOG_TYPE_JS_ALERT:
                LOG_DEBUG("JS ALERT: %s\n",data->dialog.message);
                callback(data->dialog.id, GOGI_DialogData::GOGI_DIALOG_BUTTON_OK, (char*)"", (char*)"");
                break;
            case GOGI_DialogData::GOGI_DIALOG_TYPE_USERJS_ON_HTTPS:
                LOG_DEBUG("Accepting USERJS on HTTPS.\n");
                callback(data->dialog.id, GOGI_DialogData::GOGI_DIALOG_BUTTON_OK, (char*)"", (char*)"");
                break;
            case GOGI_DialogData::GOGI_DIALOG_TYPE_ASK_COOKIE:
            {
                /* Below is what to be moved into another manager(like Corsair:Tools) */
                LOG_DEBUG("Waiting for user-confirmation of cookie(s)\n");
                callback(id, GOGI_DialogData::GOGI_DIALOG_BUTTON_OK, (char*)GOGI_OPERA_COOKIE_ACCEPT_SHOW_UPDATES, NULL);
                CorsairTools::saveCookie();

                std::string cookie;
                if ( OplVodExtApp::getTVAppsCookie(cookie) )
                {
                    OPL_DIAL_SetCookie(cookie.c_str());
                }

                break;
            }
            default:
                break;
        }
    }
    else if(event == GOGI_OPERA_EVT_ERROR)
    {
        CorsairHbbTVManager *hbbtv_manager = g_corsair->getHbbTVManager();
        hbbtv_manager->action(new CorsairHbbTVErrorAction(opera_window, hbbtv_manager));
    }
    else{
        LOG_DEBUG("Unknown Notification Handler\n");
    }
}

//**********************************************
// Corsair main
//**********************************************

int main(int argc, char** argv)
{
//  setpriority(PRIO_PROCESS, getpid(), -20);
#ifdef  CONFIG_DEBUG
    HLIB_DBG_Init();
#endif

    VK_Init();

    LOG_DEBUG("CorsairMain: launching ...\n");

    if ( BPL_Init() != BPL_STATUS_OK )
    {
        LOG_ERR("FATAL:: Cannot initialize BPL.\n");
        exit(-1);
    }
    LOG_DEBUG("CorsairMain: VK and BPL initialized.\n");

#ifdef OIPF
    g_corsair = new Asteriks(argc, argv);
#else
    g_corsair = new Corsair(argc, argv);
#endif // OIPF

#if defined(CONFIG_HUMAX_CRASHREPORT)
    HCR_Init(NULL);
#endif

    LOG_DEBUG("CorsairMain:: Corsair instance is cateded\n");
    if(!g_corsair || !g_corsair->init())
    {
        delete g_corsair;
        exit(-1);
    }

    CorsairHama::initHamaSignal();

    HAPI_SendReadySignal(HLIB_STD_GetPID());    // to hamalauncher

    g_corsair->run();
    LOG_ERR("Corsair (pid %ld) going down.\n",getpid());
    delete g_corsair;

    LOG_DEBUG("Corsair exited.\n");
    return 0;
}
