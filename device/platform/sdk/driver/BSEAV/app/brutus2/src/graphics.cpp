/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: graphics.cpp $
* $brcm_Revision: 5 $
* $brcm_Date: 10/19/12 10:36a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/graphics.cpp $
* 
* 5   10/19/12 10:36a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 4   10/7/12 3:35p tokushig
* SW7231-749: add support for auto format change
* 
* 3   9/18/12 12:06p tokushig
* SW7231-749: remove workaround code for simple compositor bug
* 
* 2   9/17/12 4:02p tokushig
* SW7231-749: use simple compositor for graphics
* 
* 1   9/15/12 1:11a tokushig
* SW7231-749: add graphics class
* 
* 
*********************************************************************************/

#include "brutus.h"
#include "graphics.h"
#include "display.h"
#include "nexus_surface_client.h"
#include "nexus_surface_compositor.h"

BDBG_MODULE(graphics);


CGraphics::CGraphics(const char * name, const uint16_t number, CConfiguration * pCfg) :
    CResource(name, number, eBoardResource_graphics, pCfg),
    _pDisplayPrimary(NULL),
    _pDisplaySecondary(NULL),
    _blitter(NULL),
    _checkpointEvent(NULL),
    _inactiveEvent(NULL),
    _surface(NULL),
    _graphicsWidth(0),
    _graphicsHeight(0),
    _itrBackSurface(&_backSurfaceList),
    _bwinEngine(NULL),
    _font10(NULL),
    _font12(NULL),
    _font14(NULL),
    _font17(NULL)
{
    _backSurfaceList.clear();
}

eRet CGraphics::open()
{
    eRet                  ret       = eRet_Ok;
    NEXUS_Error           nerror    = NEXUS_SUCCESS;

error:
    return ret;
}

void CGraphics::close()
{
    uninitGraphics();
}

void CGraphics::graphicsCheckpoint()
{
    NEXUS_Error nerror = NEXUS_SUCCESS;
    BDBG_ASSERT(NULL != _blitter);

    nerror = NEXUS_Graphics2D_Checkpoint(_blitter, NULL);
    if (nerror == NEXUS_GRAPHICS2D_QUEUED) 
    {
        B_Event_Wait(_checkpointEvent, B_WAIT_FOREVER);
    }
}

void CGraphics::setActive(bool bActive)
{
    NEXUS_SurfaceCompositorSettings settings;
    BDBG_ASSERT(NULL != _surfaceCompositor);

    NEXUS_SurfaceCompositor_GetSettings(_surfaceCompositor, &settings);
    settings.enabled = bActive;
    NEXUS_SurfaceCompositor_SetSettings(_surfaceCompositor, &settings);

    if (false == bActive)
    {
        B_Event_Wait(_inactiveEvent, B_WAIT_FOREVER);
    }
}

bool CGraphics::isActive()
{
    NEXUS_SurfaceCompositorSettings settings;
    BDBG_ASSERT(NULL != _surfaceCompositor);

    NEXUS_SurfaceCompositor_GetSettings(_surfaceCompositor, &settings);
    return settings.enabled;
}

static void setEventCallback(void *context, int param)
{
    BSTD_UNUSED(param);
    B_Event_Set((B_EventHandle)context);
}

static void do_sync(bwin_framebuffer_t fb)
{
    eRet                            ret            = eRet_Ok;
    NEXUS_Error                     nerror         = NEXUS_SUCCESS;
    CGraphics                     * pGraphics      = NULL;
    bwin_framebuffer_settings       settings;
    NEXUS_SurfaceHandle             outputSurface;
    NEXUS_Graphics2DBlitSettings    blitSettings;
    
    BDBG_MSG(("TTTTTTTTTTTTTTTTTT DO_SYNC - pGraphics:%p", pGraphics));

    bwin_get_framebuffer_settings(fb, &settings);
    pGraphics = (CGraphics *)settings.data;
    pGraphics->sync(fb);

error:
    return;
}

void CGraphics::sync(bwin_framebuffer_t framebuffer)
{
    NEXUS_Error nerror = NEXUS_SUCCESS;
    B_Error     berror = B_ERROR_SUCCESS;
    eRet        ret    = eRet_Ok;

    NEXUS_Surface_Flush(getSurface());

    nerror = NEXUS_SurfaceClient_UpdateSurface(getDesktopClient(), NULL);
    CHECK_NEXUS_ERROR_GOTO("surface compositor set surface error", ret, nerror, error);
 
    berror = B_Event_Wait(_displayedEvent, 5000);
    CHECK_BOS_ERROR_GOTO("problem while waiting for set surface to complete", ret, nerror, error);

error:
    return;
}

eRet CGraphics::initGraphics(uint16_t width, uint16_t height, uint16_t backWidth, uint16_t backHeight)
{
    eRet                         ret                = eRet_Ok;
    NEXUS_Error                  nerror             = NEXUS_SUCCESS;
    NEXUS_Graphics2DSettings     graphicsSettings;
    NEXUS_SurfaceCreateSettings  surfaceSettings;
    NEXUS_Graphics2DFillSettings fillSettings;

    _blitter = NEXUS_Graphics2D_Open(0, NULL);

    _checkpointEvent = B_Event_Create(NULL);
    CHECK_PTR_GOTO("graphics checkpoint event create failed", _checkpointEvent, ret, eRet_ExternalError, error);
    _inactiveEvent = B_Event_Create(NULL);
    CHECK_PTR_GOTO("graphics inactive event create failed", _inactiveEvent, ret, eRet_ExternalError, error);

    NEXUS_Graphics2D_GetSettings(_blitter, &graphicsSettings);
    graphicsSettings.checkpointCallback.callback = setEventCallback;
    graphicsSettings.checkpointCallback.context  = _checkpointEvent;
    nerror = NEXUS_Graphics2D_SetSettings(_blitter, &graphicsSettings);
    CHECK_NEXUS_ERROR_GOTO("unable to set graphics2D settings", ret, nerror, error);

    /* create framebuffer surface */
    NEXUS_Surface_GetDefaultCreateSettings(&surfaceSettings);
    surfaceSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    surfaceSettings.width       = width;
    surfaceSettings.height      = height;
    _surface = NEXUS_Surface_Create(&surfaceSettings);
    CHECK_PTR_GOTO("surface create failed.", _surface, ret, eRet_ExternalError, error);

    /* fill surface with black */
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = _surface;
    fillSettings.color   = 0xFF000000;
    nerror = NEXUS_Graphics2D_Fill(_blitter, &fillSettings);
    CHECK_NEXUS_ERROR_GOTO("graphics fill error", ret, nerror, error);

    graphicsCheckpoint();

    /* create surface compositor */
    {
        NEXUS_SurfaceCompositorSettings settings;

        _surfaceCompositor = NEXUS_SurfaceCompositor_Create(0);
        NEXUS_SurfaceCompositor_GetSettings(_surfaceCompositor, &settings);
        settings.inactiveCallback.callback = setEventCallback;
        settings.inactiveCallback.context  = _inactiveEvent;

        NEXUS_Display_GetGraphicsSettings(_pDisplayPrimary->getDisplay(), &settings.display[0].graphicsSettings);
        settings.display[0].graphicsSettings.enabled    = true;
        settings.display[0].display                     = _pDisplayPrimary->getDisplay();
        settings.display[0].framebuffer.number          = 2;
        settings.display[0].framebuffer.width           = backWidth;
        settings.display[0].framebuffer.height          = backHeight;
        settings.display[0].framebuffer.backgroundColor = 0xFF000000; /* black background */
        settings.display[0].framebuffer.heap            = NEXUS_Platform_GetFramebufferHeap(0);

        if (NULL != _pDisplaySecondary) 
        {
            NEXUS_Display_GetGraphicsSettings(_pDisplaySecondary->getDisplay(), &settings.display[1].graphicsSettings);
            settings.display[1].graphicsSettings.enabled    = true;
            settings.display[1].display                     = _pDisplaySecondary->getDisplay();
            settings.display[1].framebuffer.width           = 720;
            settings.display[1].framebuffer.height          = 480;
            settings.display[1].framebuffer.backgroundColor = 0xFF000000; /* black background */
            settings.display[1].framebuffer.heap            = NEXUS_Platform_GetFramebufferHeap(1);
        }

        /* 
        settings.frameBufferCallback.callback = framebuffer_callback;
        settings.frameBufferCallback.context  = _surfaceCompositor;
        */
        nerror = NEXUS_SurfaceCompositor_SetSettings(_surfaceCompositor, &settings);
        CHECK_NEXUS_ERROR_GOTO("surface compositor set settings error", ret, nerror, error);
    }

    /* create desktopClient - brutus2 is a surface server but also draws like a client */
    /*      default settings make it fullscreen, zorder=0 */
    {
        NEXUS_SurfaceClientSettings             settingsClient;
        NEXUS_SurfaceCompositorClientSettings   settingsCompositor;

        _displayedEvent = B_Event_Create(NULL);
        CHECK_PTR_GOTO("graphics displayed event create failed", _displayedEvent, ret, eRet_ExternalError, error);

        _desktopClient = NEXUS_SurfaceCompositor_CreateClient(_surfaceCompositor, 100);
        CHECK_NEXUS_ERROR_GOTO("surface compositor create failed", ret, nerror, error);
        NEXUS_SurfaceClient_GetSettings(_desktopClient, &settingsClient);
        settingsClient.displayed.callback = setEventCallback;
        settingsClient.displayed.context  = _displayedEvent;
        nerror = NEXUS_SurfaceClient_SetSettings(_desktopClient, &settingsClient);
        CHECK_NEXUS_ERROR_GOTO("surface client set settings failed", ret, nerror, error);

        nerror = NEXUS_SurfaceClient_SetSurface(_desktopClient, _surface);
        CHECK_NEXUS_ERROR_GOTO("surface compositor set surface error", ret, nerror, error);
    }
    
    /* TODO: for multiprocess support create clients for client apps to use. */

    /* start bwin and bwidgets */
    {
        bwin_engine_settings bwinSettings;
        bwin_framebuffer_settings fbSettings;
        NEXUS_SurfaceMemory  surfaceMem;

        bwin_engine_settings_init(&bwinSettings);
        _bwinEngine = bwin_open_engine(&bwinSettings);
        CHECK_PTR_GOTO("Unable to open bwin engine", _bwinEngine, ret, eRet_ExternalError, error);


        _font10 = bwin_open_font(_bwinEngine, "fonts/verdana_10.bwin_font", -1, true);
        CHECK_PTR_GOTO("Font creation failed", _font10, ret, eRet_ExternalError, error);
        _font12 = bwin_open_font(_bwinEngine, "fonts/verdana_12.bwin_font", -1, true);
        CHECK_PTR_GOTO("Font creation failed", _font12, ret, eRet_ExternalError, error);
        _font14 = bwin_open_font(_bwinEngine, "fonts/verdana_14.bwin_font", -1, true);
        CHECK_PTR_GOTO("Font creation failed", _font14, ret, eRet_ExternalError, error);
        _font17 = bwin_open_font(_bwinEngine, "fonts/verdana_17.bwin_font", -1, true);
        CHECK_PTR_GOTO("Font creation failed", _font17, ret, eRet_ExternalError, error);

        NEXUS_Surface_GetMemory(_surface, &surfaceMem);

        BDBG_WRN(("bwin offscreen: framebuffer %p, %dx%d, pitch %d\n", surfaceMem.buffer, width, height, surfaceMem.pitch));
        bwin_framebuffer_settings_init(&fbSettings);
        fbSettings.width         = width;
        fbSettings.height        = height;
        fbSettings.pitch         = surfaceMem.pitch;
        fbSettings.buffer        = surfaceMem.buffer;
        fbSettings.second_buffer = NULL;
        fbSettings.pixel_format  = bwin_pixel_format_a8_r8_g8_b8;
        fbSettings.drawops.sync  = do_sync;
        fbSettings.data          = this;
        _framebuffer             = bwin_open_framebuffer(_bwinEngine, &fbSettings);
        CHECK_PTR_GOTO("bwin framebuffer failed to open", _framebuffer, ret, eRet_ExternalError, error);

        bwin_get_framebuffer_settings(_framebuffer, &fbSettings);
        _winFramebuffer = fbSettings.window;
    }


    return ret;

error:
    uninitGraphics();
    return ret;
}

void CGraphics::uninitGraphics()
{
    _winFramebuffer = NULL;

    if (NULL != _framebuffer)
    {
        bwin_close_framebuffer(_framebuffer);
        _framebuffer = NULL;
    }

    if (NULL != _font10)
    {
        bwin_close_font(_font10);
        _font10 = NULL;
    }

    if (NULL != _font12)
    {
        bwin_close_font(_font12);
        _font12 = NULL;
    }

    if (NULL != _font14)
    {
        bwin_close_font(_font14);
        _font14 = NULL;
    }

    if (NULL != _font17)
    {
        bwin_close_font(_font17);
        _font17 = NULL;
    }

    if (NULL != _bwinEngine)
    {
        bwin_close_engine(_bwinEngine);
        _bwinEngine = NULL;
    }

    if (NULL != _desktopClient)
    {
        NEXUS_SurfaceCompositorSettings settings;

        setActive(false);
        NEXUS_SurfaceCompositor_GetSettings(_surfaceCompositor, &settings);
        settings.display[0].display = NULL;
        NEXUS_SurfaceCompositor_SetSettings(_surfaceCompositor, &settings);
        setActive(true);

        NEXUS_SurfaceClient_Clear(_desktopClient);
        NEXUS_SurfaceCompositor_DestroyClient(_desktopClient);
        _desktopClient = NULL;
    }

    {
        NEXUS_SurfaceHandle            * pSurface;
        MListItr <NEXUS_SurfaceHandle>   itr(&_backSurfaceList);

        /* destory all back buffer surfaces */
        for (pSurface = itr.first(); pSurface; pSurface = itr.next())
        {
            if (NULL != pSurface)
            {
                NEXUS_Surface_Destroy(*pSurface);
                delete pSurface;
                pSurface = NULL;
            }
        }
        _backSurfaceList.clear();
    }

    if (NULL != _displayedEvent)
    {
        B_Event_Destroy(_displayedEvent);
        _displayedEvent = NULL;
    }

    if (NULL != _surfaceCompositor)
    {
        NEXUS_SurfaceCompositor_Destroy(_surfaceCompositor);
        _surfaceCompositor = NULL;
    }

    if (NULL != _surface)
    {
        NEXUS_Surface_Destroy(_surface);
        _surface = NULL;
    }

    if (NULL != _blitter)
    {
        NEXUS_Graphics2D_Close(_blitter);
        _blitter = NULL;
    }

    if (NULL != _checkpointEvent)
    {
        B_Event_Destroy(_checkpointEvent);
        _checkpointEvent = NULL;
    }

    if (NULL != _inactiveEvent)
    {
        B_Event_Destroy(_inactiveEvent);
        _inactiveEvent = NULL;
    }
}

CGraphics::~CGraphics()
{
    close();
}

void CGraphics::dump()
{
}

bwin_font_t CGraphics::getFont(uint8_t size)
{
    bwin_font_t font = NULL;

    switch (size)
    {
    case 10:
        font = _font10;
        break;
    case 12:
        font = _font12;
        break;
    case 14:
        font = _font14;
        break;
    case 17:
        font = _font17;
        break;
    default:
        BDBG_ERR(("invalid font size given"));
        break;
    }

    return font;
}


eRet CGraphics::setFramebufferSize(uint16_t width, uint16_t height)
{
    NEXUS_SurfaceCompositorSettings settings;
    eRet                            ret        = eRet_InvalidState;
    NEXUS_Error                     nerror     = NEXUS_SUCCESS;

    BDBG_ASSERT(NULL != getDisplay());
    BDBG_ASSERT(NULL != _surfaceCompositor);

    NEXUS_SurfaceCompositor_GetSettings(_surfaceCompositor, &settings);
    BDBG_ASSERT(false == settings.enabled);

    for (int i = 0; i < NEXUS_MAX_DISPLAYS; i++)
    {
        if (getDisplay()->getDisplay() == settings.display[i].display)
        {
            BDBG_MSG(("set framebuffer size w:%d h:%d", width, height));

            /* set simple compositor framebuffer size */
            settings.display[i].framebuffer.width  = width;
            settings.display[i].framebuffer.height = height;
            settings.display[i].graphicsSettings.clip.width  = width;
            settings.display[i].graphicsSettings.clip.height = height;
            nerror = NEXUS_SurfaceCompositor_SetSettings(_surfaceCompositor, &settings);
            CHECK_NEXUS_ERROR_GOTO("unable to set simple compositor framebuffer size", ret, nerror, error);

            ret = eRet_Ok;
            break;
        }
    }

error:
    return ret;
}

