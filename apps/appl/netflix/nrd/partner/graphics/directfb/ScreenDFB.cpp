#include <Debug.h>
#include <Screen.h>
#include <Effects.h>
#include <Surface.h>
#include <Animation.h>
#include "EffectsDFB.h"
#include <SurfaceCache.h>
#include <GibbonApplication.h>

#include <directfb/directfb_version.h>

#include <nrdbase/tr1.h>
#include <nrdbase/ScopedMutex.h>

#include <iostream>

#if 1 /* HUMAX Patch */
#include <GibbonApplication.h>
#endif

#ifndef ENUM_TO_STR
#define ENUM_TO_STR(enumVal)            case enumVal: return #enumVal;
#endif

using namespace netflix::gibbon;
using namespace netflix;
namespace netflix { namespace gibbon { extern int sDFBGraphicsMemory; } }

#if 1 /* HUMAX Patch */
static IDirectFB *s_mFirstDirectFB = NULL; /* static variable for fixing nexus_client IPC error between DFB_init (NEXUS_Platform_Join) and nexus API calling */
static int s_uDFBCreateCnt = 0;
static bool s_dfbCreated = false;
static bool s_screenCleanup = false;
static bool s_needPostCleanup = false;
static bool s_screenFirstRendered = false;
#endif

#ifdef GIBBON_DFBSCREEN_NO_WINDOW
Screen::Configuration Screen::sConfiguration(GIBBON_DFBSCREEN_ID, false);
#else
Screen::Configuration Screen::sConfiguration(GIBBON_DFBSCREEN_ID, true);
#endif

Effect::SharedPtr Screen::createEffect(Effect::Type type)
{
#if 0
    if(type == Effect::Type_Mask)
        return Effect::SharedPtr(new EffectMaskDFB);
    if(type == Effect::Type_Transform)
        return Effect::SharedPtr(new EffectTransformDFB);
#else
    (void)type;
#endif
    return shared_ptr<Effect>();
}

Variant Screen::graphicsInfo() const
{
    Variant result;
    {
        std::ostringstream os;
        Variant dfbLibrary;
        dfbLibrary["name"] = "directfb";
        os << DIRECTFB_MAJOR_VERSION << "." << DIRECTFB_MINOR_VERSION << "." << DIRECTFB_MICRO_VERSION;
        dfbLibrary["compiled"] = os.str();
        os.str("");
        os << directfb_major_version << "." << directfb_minor_version << "." << directfb_micro_version;
        dfbLibrary["runtime"] = os.str();
        result["libraries"] = dfbLibrary;
    }
    result["dfb"]["layer"] = sConfiguration.layer;
    result["dfb"]["window"] = sConfiguration.window;
    return result;
}

void Screen::setConfiguration(const Configuration &config)
{
    shared_ptr<Screen> screen;
    {
        ScopedMutex screenLock(Screen::lock());
        sConfiguration = config;
        if(GibbonApplication::instance()) {
            screen = GibbonApplication::instance()->getScreen();
            if(screen) {
                screen->cleanupSurface();
                screen->initSurface();
            }
        }
    }
    GibbonApplication::instance()->initInputManager();
    if(screen)
        screen->update();
}

#if 1 /* HUMAX Patch */
extern "C" {
/* <---- extern ----> */
static char *getGibbonOemToStr(GibbonOEMEvent event)
{
    switch(event) {
    ENUM_TO_STR(GibbonOEM_Init);
    ENUM_TO_STR(GibbonOEM_Started);
    ENUM_TO_STR(GibbonOEM_Suspended);
    ENUM_TO_STR(GibbonOEM_Cleanup);
    default:
        break;
    }
    return "N/A";
}

void ScreenDFBNotifyOEMEvent(const char *caller, GibbonOEMEvent event) /* To avoid DFB->Release before ~PlayReadyDrmSystem() Oem_Destroy() */
{
    NTRACE(TRACE_DPI, "ScreenDFB::NotifyOEMEvent ++(caller:%s, event:%d)\n", caller, getGibbonOemToStr(event));
    switch(event) {
    case GibbonOEM_Init:
    case GibbonOEM_Started:
        s_needPostCleanup = true;
        break;
    case GibbonOEM_Suspended:
    case GibbonOEM_Cleanup:
        s_needPostCleanup = true;
        break;
    case GibbonOEM_Rendered:
        if(s_screenFirstRendered == false)
        {
            IDirectFBWindow *dfbWindow = GibbonApplication::instance()->getScreen()->getDirectFBWindow();
            dfbWindow->SetOpacity(dfbWindow, 0xFF);
            s_screenFirstRendered = true;
        }
        break;
    }
    NTRACE(TRACE_DPI, "ScreenDFB::NotifyOEMEvent --(cleanup:%d)\n", s_needPostCleanup);
}

void ScreenDFBPostCleanup(const char *caller)  /* static function : Called at ~DeviceLibNative() destroy, not GibbonOEM_Cleanup or ... */
{
    NTRACE(TRACE_DPI, "++ScreenDFB::PostCleanup++ [caller:%s, post:%d, cnt:%d, prevDFB:%p]\n", caller, s_needPostCleanup, s_uDFBCreateCnt, s_mFirstDirectFB);
        
    if (s_needPostCleanup == true)
    {
        if (s_mFirstDirectFB) /* s_dfbCreated */
        {
            NTRACE(TRACE_DPI, ">> DirectFB->Release(prevDFB:%p)\n", s_mFirstDirectFB);
            s_mFirstDirectFB->Release(s_mFirstDirectFB);
            s_mFirstDirectFB = NULL;
            s_uDFBCreateCnt--; 
            s_dfbCreated = false;
         }
        else
        {
            Log::error(TRACE_GIBBON_GRAPHICS, "Error> Already directFB cleanup");
            NTRACE(TRACE_DPI, "ScreenDFB::PostCleanup() Error> Already directFB cleanup[cleanup:%d, DFB:%p]\n", s_needPostCleanup, s_mFirstDirectFB);
        }
    }
    NTRACE(TRACE_DPI, "--ScreenDFB::PostCleanup-- [caller:%s, post:%d, cnt:%d, prevDFB:%p]\n", caller, s_needPostCleanup, s_uDFBCreateCnt, s_mFirstDirectFB);
}

void ScreenDFBPreInit(const char *caller)  /* static function : Called once when the initialization by GibbonOEM_Init event is in progress */
{
    DFBResult ret;
    NTRACE(TRACE_DPI, "++ScreenDFB::PreInit++ [caller:%s, post:%d, cnt:%d, prevDFB:%p]\n", caller, s_needPostCleanup, s_uDFBCreateCnt, s_mFirstDirectFB);
    
    if (s_mFirstDirectFB != NULL)
    {
        Log::error(TRACE_GIBBON_GRAPHICS, "Error> invalid non-NULL directFB");
		return;
    }
    
    /* signal(SIGPIPE, SIG_IGN); */
    
    ret = DirectFBInit(0, NULL); /* (&argc, &argv) */
    if (ret) {
        Log::error(TRACE_GIBBON_GRAPHICS, "Error> DirectFBInit failed");
    }
    
    NTRACE(TRACE_DPI, "--> +DirectFBCreate (mDFB:%p, prevDFB:%p)\n", s_mFirstDirectFB);
    ret = DirectFBCreate(&s_mFirstDirectFB);
    NTRACE(TRACE_DPI, "--> -DirectFBCreate (mDFB:%p, prevDFB:%p)\n", s_mFirstDirectFB);
    if (ret) {
        Log::error(TRACE_GIBBON_GRAPHICS, "Error> DirectFBCreate failed");
    }
    s_uDFBCreateCnt++;
    NTRACE(TRACE_DPI, "--ScreenDFB::PreInit-- [caller:%s, post:%d, cnt:%d, prevDFB:%p]\n", caller, s_needPostCleanup, s_uDFBCreateCnt, s_mFirstDirectFB);
}
/* <---- extern ----> */
}
#endif

void Screen::init()
{
#if 1 /* HUMAX Patch */
    DFBResult     ret;
    NTRACE(TRACE_DPI, "++ScreenDFB::init++ [prevDFB:%p, cnt:%d]\n", s_mFirstDirectFB, s_uDFBCreateCnt);
#endif	
    mDirectFBWindow = 0;

    NTRACE(TRACE_GIBBON_GRAPHICS, "Starting Netflix Application with DirectFB");
#if 0 /* HUMAX Patch */
    int argc = GibbonApplication::getArgc();
    char **argv = GibbonApplication::getArgv();
#endif

    if (s_mFirstDirectFB != NULL)
    {
        /* Replace w/ previous created dfb handle because of nexus_client crash */
        NTRACE(TRACE_DPI, "replaced with (prevDFB:%p, cnt:%d)\n", s_mFirstDirectFB, s_uDFBCreateCnt);
        mDirectFB = s_mFirstDirectFB; /* First DFB creation is replaced by w/ ScreenDFBPreInit() */
    }
    else
    {
        /* signal(SIGPIPE, SIG_IGN); */
        ret = DirectFBInit(0, NULL); /* (&argc, &argv) */
        if (ret) {
            Log::error(TRACE_GIBBON_GRAPHICS, "Error> DirectFBInit failed");
        }

#ifndef BUILD_DEBUG
    DirectFBSetOption("no-cursor", "");
#endif

        NTRACE(TRACE_DPI, "--> +DirectFBCreate (mDFB:%p, prevDFB:%p)++\n", mDirectFB, s_mFirstDirectFB);
        ret = DirectFBCreate(&mDirectFB);
        NTRACE(TRACE_DPI, "--> -DirectFBCreate (mDFB:%p)++\n", mDirectFB);
        if (ret) {
            Log::error(TRACE_GIBBON_GRAPHICS, "Error> DirectFBCreate failed");
        }
        s_mFirstDirectFB = mDirectFB; /* preinit(), postcleanup() based */
        s_uDFBCreateCnt++;
    }

    DFBGraphicsDeviceDescription dsc;
    if(mDirectFB->GetDeviceDescription(mDirectFB, &dsc) == DFB_OK) {
        if(dsc.video_memory) {
            sDFBGraphicsMemory = dsc.video_memory;
        }
#if defined(BUILD_DEBUG) && 0
        std::cout << "DirectFB Device Description -------------------------------------" << std::endl;
        std::cout << "Device Name: " << dsc.name << std::endl;
        std::cout << "Device Vendor: " << dsc.vendor << std::endl;
        std::cout << "Driver Name: " << dsc.driver.name << std::endl;
        std::cout << "Driver Vendor: " << dsc.driver.vendor << std::endl;
        std::cout << "Driver Version: " << dsc.driver.major << "." << dsc.driver.minor << std::endl;
        std::cout << "Video Memory: " << dsc.video_memory << " bytes" << std::endl;
        std::cout << "Acceleration Flags:" << std::endl;
        std::cout << "    FillRectangle: " << ((dsc.acceleration_mask & DFXL_FILLRECTANGLE) != 0 ? "accelerated" : "slow") << std::endl;
        std::cout << "    DrawRectangle: " << ((dsc.acceleration_mask & DFXL_DRAWRECTANGLE) != 0 ? "accelerated" : "slow") << std::endl;
        std::cout << "    DrawLine: " << ((dsc.acceleration_mask & DFXL_DRAWLINE) != 0 ? "accelerated" : "slow") << std::endl;
        std::cout << "    FillTriangle: " << ((dsc.acceleration_mask & DFXL_FILLTRIANGLE) != 0 ? "accelerated" : "slow") << std::endl;
        std::cout << "    Blit: " << ((dsc.acceleration_mask & DFXL_BLIT) != 0 ? "accelerated" : "slow") << std::endl;
        std::cout << "    StretchBlit: " << ((dsc.acceleration_mask & DFXL_STRETCHBLIT) != 0 ? "accelerated" : "slow") << std::endl;
        std::cout << "    TexTriangles: " << ((dsc.acceleration_mask & DFXL_TEXTRIANGLES) != 0 ? "accelerated" : "slow") << std::endl;
//        std::cout << "    Blit2: " << ((dsc.acceleration_mask & DFXL_BLIT2) != 0 ? "accelerated" : "slow") << std::endl;
        std::cout << "    DrawString: " << ((dsc.acceleration_mask & DFXL_DRAWSTRING) != 0 ? "accelerated" : "slow") << std::endl;
        std::cout << "Blitting Flags:" << std::endl;
        std::cout << "    DSBLIT_BLEND_ALPHACHANNEL: " << ((dsc.blitting_flags & DSBLIT_BLEND_ALPHACHANNEL) != 0 ? "supported" : "n/a") << std::endl;
        std::cout << "    DSBLIT_BLEND_COLORALPHA: " << ((dsc.blitting_flags & DSBLIT_BLEND_COLORALPHA) != 0 ? "supported" : "n/a") << std::endl;
        std::cout << "    DSBLIT_COLORIZE: " << ((dsc.blitting_flags & DSBLIT_COLORIZE) != 0 ? "supported" : "n/a") << std::endl;
        std::cout << "    DSBLIT_SRC_COLORKEY: " << ((dsc.blitting_flags & DSBLIT_SRC_COLORKEY) != 0 ? "supported" : "n/a") << std::endl;
        std::cout << "    DSBLIT_DST_COLORKEY: " << ((dsc.blitting_flags & DSBLIT_DST_COLORKEY) != 0 ? "supported" : "n/a") << std::endl;
        std::cout << "    DSBLIT_SRC_PREMULTIPLY: " << ((dsc.blitting_flags & DSBLIT_SRC_PREMULTIPLY) != 0 ? "supported" : "n/a") << std::endl;
        std::cout << "    DSBLIT_DST_PREMULTIPLY: " << ((dsc.blitting_flags & DSBLIT_DST_PREMULTIPLY) != 0 ? "supported" : "n/a") << std::endl;
        std::cout << "    DSBLIT_DEMULTIPLY: " << ((dsc.blitting_flags & DSBLIT_DEMULTIPLY) != 0 ? "supported" : "n/a") << std::endl;
        std::cout << "    DSBLIT_DEINTERLACE: " << ((dsc.blitting_flags & DSBLIT_DEINTERLACE) != 0 ? "supported" : "n/a") << std::endl;
        std::cout << "    DSBLIT_SRC_PREMULTCOLOR: " << ((dsc.blitting_flags & DSBLIT_SRC_PREMULTCOLOR) != 0 ? "supported" : "n/a") << std::endl;
        std::cout << "    DSBLIT_XOR: " << ((dsc.blitting_flags & DSBLIT_XOR) != 0 ? "supported" : "n/a") << std::endl;
//        std::cout << "    DSBLIT_INDEX_TRANSLATION: " << ((dsc.blitting_flags & DSBLIT_INDEX_TRANSLATION) != 0 ? "supported" : "n/a") << std::endl;
//        std::cout << "    DSBLIT_ROTATE90: " << ((dsc.blitting_flags & DSBLIT_ROTATE90) != 0 ? "supported" : "n/a") << std::endl;
//        std::cout << "    DSBLIT_ROTATE180: " << ((dsc.blitting_flags & DSBLIT_ROTATE180) != 0 ? "supported" : "n/a") << std::endl;
//        std::cout << "    DSBLIT_ROTATE270: " << ((dsc.blitting_flags & DSBLIT_ROTATE270) != 0 ? "supported" : "n/a") << std::endl;
//        std::cout << "    DSBLIT_COLORKEY_PROTECT: " << ((dsc.blitting_flags & DSBLIT_COLORKEY_PROTECT) != 0 ? "supported" : "n/a") << std::endl;
//        std::cout << "    DSBLIT_SRC_MASK_ALPHA: " << ((dsc.blitting_flags & DSBLIT_SRC_MASK_ALPHA) != 0 ? "supported" : "n/a") << std::endl;
//        std::cout << "    DSBLIT_SRC_MASK_COLOR: " << ((dsc.blitting_flags & DSBLIT_SRC_MASK_COLOR) != 0 ? "supported" : "n/a") << std::endl;
//        std::cout << "    DSBLIT_SOURCE2: " << ((dsc.blitting_flags & DSBLIT_SOURCE2) != 0 ? "supported" : "n/a") << std::endl;
//        std::cout << "    DSBLIT_FLIP_HORIZONTAL: " << ((dsc.blitting_flags & DSBLIT_FLIP_HORIZONTAL) != 0 ? "supported" : "n/a") << std::endl;
//        std::cout << "    DSBLIT_FLIP_VERTICAL: " << ((dsc.blitting_flags & DSBLIT_FLIP_VERTICAL) != 0 ? "supported" : "n/a") << std::endl;
        std::cout << "Drawing Flags:" << std::endl;
        std::cout << "    DSDRAW_BLEND: " << ((dsc.drawing_flags & DSDRAW_BLEND) != 0 ? "supported" : "n/a") << std::endl;
        std::cout << "    DSDRAW_DST_COLORKEY: " << ((dsc.drawing_flags & DSDRAW_DST_COLORKEY) != 0 ? "supported" : "n/a") << std::endl;
        std::cout << "    DSDRAW_SRC_PREMULTIPLY: " << ((dsc.drawing_flags & DSDRAW_SRC_PREMULTIPLY) != 0 ? "supported" : "n/a") << std::endl;
        std::cout << "    DSDRAW_DST_PREMULTIPLY: " << ((dsc.drawing_flags & DSDRAW_DST_PREMULTIPLY) != 0 ? "supported" : "n/a") << std::endl;
        std::cout << "    DSDRAW_DEMULTIPLY: " << ((dsc.drawing_flags & DSDRAW_DEMULTIPLY) != 0 ? "supported" : "n/a") << std::endl;
        std::cout << "    DSDRAW_XOR: " << ((dsc.drawing_flags & DSDRAW_XOR) != 0 ? "supported" : "n/a") << std::endl;
        std::cout << "-----------------------------------------------------------------" << std::endl;
#endif
    }
    
    s_screenCleanup = false;
    s_screenFirstRendered = false;
#if 1 /* HUMAX Patch */
    NTRACE(TRACE_DPI, "--ScreenDFB::init--\n");
#endif
}

void Screen::cleanup()
{
    bool matchedDFB = false;
    
    NTRACE(TRACE_DPI, "++ScreenDFB::cleanup++ [cnt:%d, cleanup:%d, created:%d, mDFB:%p, prevDFB:%p]\n", s_uDFBCreateCnt, s_screenCleanup, s_dfbCreated, mDirectFB, s_mFirstDirectFB);
    assert(!mDirectFBWindow);
    if(mDirectFB) {
        bool releaseDFB = true;
        if (mDirectFB == s_mFirstDirectFB) matchedDFB = true;
        
#ifdef GIBBON_DFBSCREEN_NO_RELEASE_ON_SUSPEND
        s_dfbCreated = releaseDFB = GibbonApplication::instance()->hasQuit();
#endif
        if (s_needPostCleanup == true)
        {
            /* Don't release directfb here; This cause nexus crash because ~DeviceLibNative() will be done after Screen::cleanup() */
            /* TODO : Chnage with smart avoidance! */
            NTRACE(TRACE_DPI, "Skip DirectFB->Release(mDFB:%p, prevDFB:%p]++\n", mDirectFB, s_mFirstDirectFB);
            //mDirectFB->Release(mDirectFB);
            mDirectFB = 0;
        }
        else
        {
        if(releaseDFB) {
                NTRACE(TRACE_DPI, "--> DirectFB->Release(mDFB:%p, prevDFB:%p]++\n", mDirectFB, s_mFirstDirectFB);
            mDirectFB->Release(mDirectFB);
            mDirectFB = 0;
                if (matchedDFB == true)
                {
                    s_mFirstDirectFB = NULL;
                }
                s_uDFBCreateCnt--; s_dfbCreated = false;
            }
        }
    }
    s_screenCleanup = true;
    NTRACE(TRACE_DPI, "--ScreenDFB::cleanup-- [cnt:%d, cleanup:%d, mDFB:%p, prevDFB:%p]\n", s_uDFBCreateCnt, s_screenCleanup, mDirectFB, s_mFirstDirectFB);
}

void Screen::makeCurrent_sys(bool)
{
}

bool Screen::canScaleDirectly() const
{
    return false;
}

void Screen::handleUpdate(const Rect &)
{
    sendRenderEvent();
}

void Screen::flip_sys(const Rect &rect)
{
    IDirectFBSurface *dfb = mSurface->getDFBSurface();
    if(!dfb)
        return;

#if defined(GIBBON_DFBSCREEN_VREFRESH) && defined(GIBBON_DFBSCREEN_VSYNC)
    static Time syncFlipBase(0);
    DFBSurfaceFlipFlags flags = DSFLIP_WAITFORSYNC;
    if(syncFlipBase.ms()) {
        const int delta = ((Time::mono() - syncFlipBase).ms() % GIBBON_DFBSCREEN_VREFRESH);
        if(delta > (GIBBON_DFBSCREEN_VREFRESH-8))
            flags |= DSFLIP_ONSYNC;
    }
#elif defined(GIBBON_DFBSCREEN_VSYNC) || defined(GIBBON_SCREEN_FLIPTHREAD)
    const DFBSurfaceFlipFlags flags = DSFLIP_WAITFORSYNC;
#else
    const DFBSurfaceFlipFlags flags = DSFLIP_ONSYNC;
#endif

#ifdef GIBBON_SCREEN_FULLRENDER
    NRDP_UNUSED(rect);
    DIRECTFB_CHECK(dfb->Flip(dfb, 0, flags));
#if defined(GRAPHICSENGINE_DFB_DEBUG)
    Log::error(TRACE_GIBBON_GRAPHICS, "DFB::FullFlip(%p): 0x%x", mSurface.get(), flags);
#endif
#else
    const DFBRegion rgn = rect.toDFBRegion();
    DIRECTFB_CHECK(dfb->Flip(dfb, &rgn, flags));
#if defined(GRAPHICSENGINE_DFB_DEBUG)
    Log::error(TRACE_GIBBON_GRAPHICS, "DFB::Flip(%p): %d %d %d %d 0x%x", mSurface.get(), rgn.x1, rgn.y1, rgn.x2, rgn.y2, flags);
#endif
#endif
#if defined(GIBBON_DFBSCREEN_VREFRESH) && defined(GIBBON_DFBSCREEN_VSYNC)
    if(flags == DSFLIP_WAITFORSYNC)
        syncFlipBase = Time::mono();
#endif
}

void Screen::flip_sys(const DisplayList::SharedPtr &displayList, const Rect &rect)
{
    flip_internal(displayList, rect);
}

void Screen::vsync_sys()
{
    vsync_internal();
}
