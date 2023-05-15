
#include <GibbonApplication.h>
#include <SurfaceDecoder.h>
#include <SurfaceCache.h>
#include <nrdbase/Endian.h>
#include <nrd/AppLog.h>
#include <Surface.h>
#include <Screen.h>
#include <Style.h>
#include <Surface.h>
#include <Rect.h>
#include <iostream>
#include <directfb.h>
#include <directfb_version.h>
#include <list>

#if 1 /* HUMAX_PATCH */
#include <nxlink.h>
#endif

#if 0
# define DEBUG_DFBSURFACE_INFO
#endif


using namespace netflix;
using namespace netflix::gibbon;

namespace netflix { namespace gibbon { int sDFBGraphicsMemory = -1; } }

class PurgeEvent : public Application::Event
{
public:
    PurgeEvent() { }
    ~PurgeEvent() { }
    virtual void eventFired() { gibbonApp()->getSurfaceCache()->purge(); }
};

template <typename T>
class DFBPointer
{
public:
    DFBPointer(T *tt = 0)
        : t(tt)
    {}
    ~DFBPointer()
    {
        if(t)
            t->Release(t);
    }
    operator T*() { return t; }
    T **operator&() { return &t; }
    T *operator->() { return t; }
private:
    T *t;
};
#if 1 /* HUMAX_PATCH */
static int s_surfaceCnt = 0;
#endif

void dumpDFBSurfaceInfo(Surface *surface)
{
    IDirectFBSurface *dfb = surface->getDFBSurface();

    Log::warn(TRACE_GIBBON_SURFACE) << "Surface: " << surface->describe() << std::endl;
    Log::warn(TRACE_GIBBON_SURFACE) << "DirectFB Surface Info -------------------------------------" << dfb << std::endl;

    int width, height;
    if(dfb->GetSize(dfb, &width, &height) == DFB_OK) {
        Log::warn(TRACE_GIBBON_SURFACE) << " width/height: " << width << "x" << height << std::endl;
    }

    DFBSurfacePixelFormat pf;
    if(dfb->GetPixelFormat(dfb, &pf) == DFB_OK) {
        Log::warn(TRACE_GIBBON_SURFACE) << " pixelformat: " << DFB_PIXELFORMAT_INDEX(pf) << std::endl;
    }

    DFBAccelerationMask am;
    if(dfb->GetAccelerationMask(dfb, dfb, &am) == DFB_OK) {
        Log::warn(TRACE_GIBBON_SURFACE) << "    FillRectangle: " << ((am & DFXL_FILLRECTANGLE) != 0 ? "accelerated" : "slow") << std::endl;
        Log::warn(TRACE_GIBBON_SURFACE) << "    DrawRectangle: " << ((am & DFXL_DRAWRECTANGLE) != 0 ? "accelerated" : "slow") << std::endl;
        Log::warn(TRACE_GIBBON_SURFACE) << "    DrawLine: " << ((am & DFXL_DRAWLINE) != 0 ? "accelerated" : "slow") << std::endl;
        Log::warn(TRACE_GIBBON_SURFACE) << "    FillTriangle: " << ((am & DFXL_FILLTRIANGLE) != 0 ? "accelerated" : "slow") << std::endl;
        Log::warn(TRACE_GIBBON_SURFACE) << "    Blit: " << ((am & DFXL_BLIT) != 0 ? "accelerated" : "slow") << std::endl;
        Log::warn(TRACE_GIBBON_SURFACE) << "    StretchBlit: " << ((am & DFXL_STRETCHBLIT) != 0 ? "accelerated" : "slow") << std::endl;
        Log::warn(TRACE_GIBBON_SURFACE) << "    TexTriangles: " << ((am & DFXL_TEXTRIANGLES) != 0 ? "accelerated" : "slow") << std::endl;
//        Log::warn(TRACE_GIBBON_SURFACE) << "    Blit2: " << ((dsc.acceleration_mask & DFXL_BLIT2) != 0 ? "accelerated" : "slow") << std::endl;
        Log::warn(TRACE_GIBBON_SURFACE) << "    DrawString: " << ((am & DFXL_DRAWSTRING) != 0 ? "accelerated" : "slow") << std::endl;
    }
}

static llong estimateScreenSize()
{
    return Surface::estimateSize(GibbonConfiguration::screenWidth(), GibbonConfiguration::screenHeight(),
                                 Surface::Flags_Screen|Surface::Flags_Alpha|Surface::Flags_RenderTarget) * 2;
}

int Surface::getGraphicsMemoryUsed()
{
    return -1;
}

int Surface::getGraphicsMemoryCapacity()
{
    if(sDFBGraphicsMemory != -1) {
        const int screenSize = estimateScreenSize();
        if(sDFBGraphicsMemory > screenSize)
            return sDFBGraphicsMemory - screenSize;
    }
    return -1;
}

int Surface::atlasPadding()
{
    return 0;
}

unsigned int Surface::calculateSize() const
{
    if(mSurface && mode() == Master)
        return estimateSize(getWidth(), getHeight(), mFlags);
    return 0;
}

static inline bool checkSurface(IDirectFBSurface *surface)
{
#ifdef GIBBON_DFB_VALIDATE_SURFACE
    if(surface) {
        void *data = 0;
        int bytesPerLine;
        if(surface->Clear(surface, 0, 0, 0, 0) != DFB_OK
            || surface->Lock(surface, DSLF_READ, &data, &bytesPerLine) != DFB_OK) {
            surface->Release(surface);
            surface = 0;
        } else {
            surface->Unlock(surface);
        }
    }
#endif
    return surface;
}

void Surface::setBlittingFlags(int flags)
{
    if(mBlittingFlags != flags) {
        mBlittingFlags = flags;
        DIRECTFB_CHECK(mSurface->SetBlittingFlags(mSurface, (DFBSurfaceBlittingFlags)flags));
    }
}

void Surface::updateSubsurface_sys()
{
    IDirectFBSurface* master = mMaster->getDFBSurface();
    const DFBRectangle rect = mRect.toDFBRect();
    const DFBResult err = master->GetSubSurface(master, &rect, &mSurface);
    if(mSurface) {
        DFBSurfacePixelFormat dfbpf;
        DIRECTFB_CHECK(mSurface->GetPixelFormat(mSurface, &dfbpf));
        unsigned actualFormat = (mFormat & BGR);
        switch (dfbpf) {
        case DSPF_ARGB: {
            actualFormat |= (BitDepth32|Alpha);
            DFBSurfaceCapabilities caps = DSCAPS_NONE;
            DIRECTFB_CHECK(mSurface->GetCapabilities(mSurface, &caps));
            if(caps & DSCAPS_PREMULTIPLIED)
                actualFormat |= Premultiplied;
            break; }
        case DSPF_RGB24:
            actualFormat |= BitDepth24;
            break;
        case DSPF_RGB32:
            actualFormat |= BitDepth32;
            break;
        case DSPF_A8:
            actualFormat = AlphaOnly;
            break;
        default:
            Log::error(TRACE_GIBBON_SURFACE, "Invalid format 0x%x", dfbpf);
            break;
        }

        mFormat = static_cast<Surface::Format>(actualFormat);
#ifdef DEBUG_DFBSURFACE_INFO
        dumpDFBSurfaceInfo(this);
#endif
    } else {
        Log::error(TRACE_GIBBON_SURFACE, "Failed to create sub DFBSurface! (%s)", DirectFBErrorString(err));
        mFormat = Format_None;
    }
}

void Surface::init()
{
    int	nDestWidth, nDestHeight;

    mBlittingFlags = -1;
    mSurface = 0;
    mVideo = true;

    Format format;
    const bool isText = mFlags & Flags_Text;
    const bool hasAlpha = mFlags & Flags_Alpha;
    if(isText)
        format = GibbonApplication::instance()->getScreen()->getFormat(Screen::SurfaceFormat_Text);
    else if(hasAlpha)
        format = GibbonApplication::instance()->getScreen()->getFormat(Screen::SurfaceFormat_Alpha);
    else
        format = GibbonApplication::instance()->getScreen()->getFormat(Screen::SurfaceFormat_Opaque);
    if(mGroup != -1) {
        mFormat = format;
        return;
    }

    IDirectFB* dfb = GibbonApplication::instance()->getScreen()->getDirectFB();
    if(!mRect.empty() && dfb) {
        DFBResult err = DFB_OK;

        DFBSurfacePixelFormat dfb_format = DSPF_UNKNOWN;
        switch (format & (Alpha|BitDepth32|BitDepth24|AlphaOnly)) {
        case Alpha|BitDepth32:
            dfb_format = DSPF_ARGB;
            break;
        case BitDepth32:
            dfb_format = DSPF_RGB32;
            break;
        case BitDepth24:
            dfb_format = DSPF_RGB24;
            break;
        case AlphaOnly:
            dfb_format = DSPF_A8;
            break;
        default:
            Log::error(TRACE_LOG, "Unhandled DFB format: %s", Screen::describeFormat(format).c_str());
            break;
        }

        if(!mSurface) {
            DFBSurfaceDescription dsc;
            memset(&dsc, 0, sizeof(dsc));
            dsc.flags = DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_CAPS;
            if(mVideo)
                dsc.caps |= DSCAPS_VIDEOONLY;
            if(format & Premultiplied)
                dsc.caps |= DSCAPS_PREMULTIPLIED;
            dsc.width = mRect.width;
            dsc.height = mRect.height;
            if(mFlags & Surface::Flags_Screen) {
#if !defined(GIBBON_SCREEN_FLIPTHREAD)
#if defined(GIBBON_DFBSCREEN_TRIPLE)
                dsc.caps |= DSCAPS_TRIPLE;
#else
                dsc.caps |= DSCAPS_DOUBLE;
#endif
#endif
				nDestWidth = 1280;
				nDestHeight = 720;

#if defined(GIBBON_DFBSCREEN_LAYER)
                const Screen::Configuration &config = GibbonApplication::instance()->getScreen()->getConfiguration();
                DFBPointer<IDirectFBDisplayLayer> dfbLayer;
                err = dfb->GetDisplayLayer(dfb, config.layer, &dfbLayer);
                if(err == DFB_OK) {
#ifdef GIBBON_DFBSCREEN_LAYER_COOPERATIVE
                    dfbLayer->SetCooperativeLevel(dfbLayer, GIBBON_DFBSCREEN_LAYER_COOPERATIVE);
#endif
                    {
                        DFBDisplayLayerConfig layerConfig;
                        dfbLayer->GetConfiguration(dfbLayer, &layerConfig);
#ifdef GIBBON_DFBSCREEN_LAYER_BUFFERMODE
                        layerConfig.flags = layerConfig.flags|DLCONF_BUFFERMODE;
                        layerConfig.buffermode = GIBBON_DFBSCREEN_LAYER_BUFFERMODE;
#endif
                        if(!config.window) {
                            layerConfig.flags = layerConfig.flags|DLCONF_WIDTH|DLCONF_HEIGHT|DLCONF_SURFACE_CAPS;
                            layerConfig.width = mRect.width;
                            layerConfig.height = mRect.height;
                            layerConfig.surface_caps = dsc.caps;
                            if(dfb_format != DSPF_UNKNOWN) {
                                layerConfig.flags = layerConfig.flags|DLCONF_PIXELFORMAT;
                                layerConfig.pixelformat = dfb_format;
                            }
                        }
                        nDestWidth = layerConfig.width;
                        nDestHeight = layerConfig.height;
                        dfbLayer->SetConfiguration(dfbLayer, &layerConfig);
                    }


                    if(config.window) {
                        DFBWindowDescription wdesc;
                        memset(&wdesc, 0, sizeof(DFBWindowDescription));
                        wdesc.flags = DWDESC_CAPS|DWDESC_HEIGHT|DWDESC_WIDTH|DWDESC_POSX|DWDESC_POSY|DWDESC_SURFACE_CAPS|DWDESC_OPTIONS;
                        wdesc.caps = DWCAPS_ALPHACHANNEL;
#if defined(GIBBON_DFBSCREEN_LAYER_DOUBLEBUFFER) || defined(__i386__)
                        // Enable DWCAPS_DOUBLEBUFFER for desktop refapp (__i386__) to avoid tearing for widgets on top of video
                        // See: NRDP-2572
                        wdesc.caps |= DWCAPS_DOUBLEBUFFER;
#endif
                        wdesc.surface_caps = dsc.caps;
#ifdef __i386__
                        // Without this blending of semi-transparent surfaces seems to break.
                        // See: NRDP-1470
                        wdesc.surface_caps |= DSCAPS_PREMULTIPLIED;
#endif
                        wdesc.height = mRect.height;
                        wdesc.width = mRect.width;
						// DWOP_SCALE : For Window/Surface Resize ...
						// DWOP_ALPHACHANNEL : Prevent Dimming by Other Window.
                        wdesc.options = DWOP_SCALE | DWOP_ALPHACHANNEL;
                        wdesc.posx = 0;
                        wdesc.posy = 0;

                        IDirectFBWindow *dfbWindow = GibbonApplication::instance()->getScreen()->getDirectFBWindow();
                        if(dfbWindow) {
                            int w, h;
                            err = dfbWindow->GetSize(dfbWindow, &w, &h);
                            if(err != DFB_OK || w != mRect.width || h != mRect.height
                                || dfbWindow->GetSurface(dfbWindow, &mSurface) != DFB_OK) {
                                dfbWindow->Release(dfbWindow);
                                dfbWindow = 0;
                            }
                        }
                        if(!dfbWindow) {
#if 1 /* HUMAX_PATCH */
                            uint32_t appid = 0;

							if((wdesc.height != nDestHeight) || (wdesc.width != nDestWidth))
							{
								wdesc.surface_caps &= ~DSCAPS_PREMULTIPLIED;
							}
#endif
                            err = dfbLayer->CreateWindow(dfbLayer, &wdesc, &dfbWindow);
                            if(err == DFB_OK) {
                                err = dfbWindow->GetSurface(dfbWindow, &mSurface);
                                if(err != DFB_OK || !mSurface) {
                                    dfbWindow->Release(dfbWindow);
                                    dfbWindow = 0;
                                } else {
#if 1 /* HUMAX_PATCH */
                                    if (1)
                                    {
                                        DIRECTFB_CHECK(mSurface->Clear(mSurface, 0, 0, 0, 0));
                                    }
#endif

                                    /* the Opacity value set 0xFF in ScreenDFB layer when received first OEM rendered event. */
                                    /*dfbWindow->SetOpacity(dfbWindow, 0xFF);*/
                                    dfbWindow->RaiseToTop(dfbWindow);
                                    dfbWindow->RequestFocus(dfbWindow);
                                }
                                GibbonApplication::instance()->getScreen()->setDirectFBWindow(dfbWindow);
#if 1 /* HUMAX_PATCH */
                                NXL_GetApplicationID(&appid);
                               	dfbWindow->SetApplicationID(dfbWindow, appid);
#endif
                            }

                            // resize a window for OSD
                            if((wdesc.height != nDestHeight) || (wdesc.width != nDestWidth))
                            {
                                dfbWindow->Resize(dfbWindow, nDestWidth, nDestHeight);
                            }
                        }
                    } else {
                        dfbLayer->GetSurface(dfbLayer, &mSurface);
                    }
                }
#endif
                dsc.caps |= DSCAPS_PRIMARY;
            } else if(dfb_format != DSPF_UNKNOWN) {
                dsc.flags |= DSDESC_PIXELFORMAT;
                dsc.pixelformat = dfb_format;
            }

            if(!mSurface) {
                if(dfb->CreateSurface(dfb, &dsc, &mSurface) != DFB_OK || !checkSurface(mSurface)) {
                    GibbonApplication::instance()->postEvent(new PurgeEvent); // We should purge, might have fragmentation issues
                    if(mFlags & Flags_Accelerated) { // Must have accelerated
                        mSurface = 0;
                    } else { // Fallback to system memory
                        mVideo = false;
                        dsc.caps &= ~DSCAPS_VIDEOONLY;
                        dsc.caps |= DSCAPS_SYSTEMONLY;
                        if(dfb->CreateSurface(dfb, &dsc, &mSurface) != DFB_OK || !checkSurface(mSurface))
                            mSurface = 0;
                    }
                }
            }
        }

        if(mSurface) {
            DFBSurfacePixelFormat dfbpf;
            DIRECTFB_CHECK(mSurface->GetPixelFormat(mSurface, &dfbpf));
            unsigned actualFormat = (format & BGR);
            switch (dfbpf) {
            case DSPF_ARGB: {
                actualFormat |= (BitDepth32|Alpha);
                DFBSurfaceCapabilities caps = DSCAPS_NONE;
                DIRECTFB_CHECK(mSurface->GetCapabilities(mSurface, &caps));
                if(caps & DSCAPS_PREMULTIPLIED)
                    actualFormat |= Premultiplied;
                break; }
            case DSPF_RGB24:
                actualFormat |= BitDepth24;
                break;
            case DSPF_RGB32:
                actualFormat |= BitDepth32;
                break;
            case DSPF_A8:
                actualFormat = AlphaOnly;
                break;
            default:
                Log::error(TRACE_GIBBON_SURFACE, "Invalid format 0x%x", dfbpf);
                break;
            }

            mFormat = static_cast<Surface::Format>(actualFormat);
#ifdef DEBUG_DFBSURFACE_INFO
            dumpDFBSurfaceInfo(this);
#endif
#if 1 /* HUMAX_PATCH */
            s_surfaceCnt++;
#endif
        } else {
            Log::error(TRACE_GIBBON_SURFACE, "Failed to create DFBSurface! (%s)", DirectFBErrorString(err));
            mFormat = Format_None;
        }
    }
#if 1 /* HUMAX_PATCH */
    //NTRACE(TRACE_DPI, "--SurfaceDFB::init(count:%d)--\n", s_surfaceCnt);
#endif
}

void Surface::cleanup()
{
#if 1 /* HUMAX_PATCH */
    //NTRACE(TRACE_DPI, "++SurfaceDFB::cleanup(count:%d)++\n", s_surfaceCnt);
#endif
    if(mSurface)
    {
        mSurface->Release(mSurface);
        mSurface = 0;
#if 1 /* HUMAX_PATCH */
        s_surfaceCnt--;
#endif
    }
    if(mFlags & Surface::Flags_Screen) {
        if(IDirectFBWindow *window = GibbonApplication::instance()->getScreen()->getDirectFBWindow()) {
            window->Release(window);
            GibbonApplication::instance()->getScreen()->setDirectFBWindow(0);
        }
    }
#if 1 /* HUMAX_PATCH */
    //NTRACE(TRACE_DPI, "--SurfaceDFB::cleanup(count:%d)--\n", s_surfaceCnt);
#endif
}

bool Surface::isValid() const
{
    return (mGroup != -1 || mSurface);
}

bool Surface::isAccelerated() const
{
    return mVideo;
}

bool Surface::matches(uint8_t flags) const
{
    const bool isAlphaOnly = mFormat & Surface::AlphaOnly;
    if(flags & Flags_Text) {
        const bool textIsAlphaOnly = GibbonApplication::instance()->getScreen()->getFormat(Screen::SurfaceFormat_Text) & Surface::AlphaOnly;
        if(textIsAlphaOnly)
            return isAlphaOnly;
    }
    if(isAlphaOnly)
        return false;
    if(flags & Flags_Alpha)
        return (mFormat & Surface::Alpha);
    return !(mFormat & Surface::Alpha) || (GibbonApplication::instance()->getScreen()->getFormat(Screen::SurfaceFormat_Opaque) & Surface::Alpha);
}

void Surface::upload(int x, int y, int width, int height, const unsigned char* data, int pad)
{
    assert(!pad);
    NRDP_UNUSED(pad);

    modify();
    if(!mSurface) {
        assert(mGroup == -1);
        // we were a subsurface at some point but now we're a master.
        // we'll need to create our DFBSurface now.
        init();
    }

#if DIRECTFB_MAJOR_VERSION >= 2 || (DIRECTFB_MAJOR_VERSION == 1 && (DIRECTFB_MINOR_VERSION >= 3 || DIRECTFB_MINOR_VERSION == 2 && DIRECTFB_MICRO_VERSION >= 9))
    const DFBRectangle dfbRect = Rect::toDFBRect(x, y, width, height);
    DIRECTFB_CHECK(mSurface->Write(mSurface, &dfbRect, data, width * Surface::bytesPerPixel(mFormat)));
#else
    IDirectFB *dfb = GibbonApplication::instance()->getScreen()->getDirectFB();
    assert(dfb);

    DFBSurfaceDescription dsc;
    memset(&dsc, 0, sizeof(dsc));
    dsc.flags = DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_CAPS | DSDESC_PREALLOCATED | DSDESC_PIXELFORMAT;
    if(mFormat & Premultiplied)
        dsc.caps |= DSCAPS_PREMULTIPLIED;
    int bpp = 0;
    switch(mFormat & (Alpha|BitDepth32|BitDepth24|AlphaOnly)) {
    case Alpha|BitDepth32:
        bpp = 4;
        dsc.pixelformat = DSPF_ARGB;
        break;
    case BitDepth32:
        bpp = 4;
        dsc.pixelformat = DSPF_RGB32;
        break;
    case BitDepth24:
        bpp = 3;
        dsc.pixelformat = DSPF_RGB24;
        break;
    case AlphaOnly:
        bpp = 1;
        dsc.pixelformat = DSPF_A8;
        break;
    default:
        return;
    }
    dsc.width = width;
    dsc.height = height;
    dsc.preallocated[0].data = (void*)data;
    dsc.preallocated[0].pitch = width * bpp;
    IDirectFBSurface *surface = 0;
    DIRECTFB_CHECK(dfb->CreateSurface(dfb, &dsc, &surface));
    if(surface) {
#if 1 /* HUMAX_PATCH */
        DIRECTFB_CHECK(mSurface->SetColor(mSurface, 0x0, 0x0, 0x0, 0xff));
#endif
        DIRECTFB_CHECK(mSurface->Blit(mSurface, surface, 0, x, y));
        surface->Release(surface);
    }
#endif
}

bool Surface::Data::lockSurface(LockType type)
{
    assert(type & (ReadLock|WriteLock));

    Screen::EndianType endian = GibbonApplication::instance()->getScreen()->getEndian();
    if(endian == Screen::Endian_Default) {
#if defined(NRDP_GPU_LITTLE_ENDIAN)
        endian = Screen::Endian_Little;
#elif defined(NRDP_GPU_BIG_ENDIAN)
        endian = Screen::Endian_Big;
#else
#warning "GIBBON_GRAPHICS_ENDIANNESS not set to a recognizable value"
        endian = Screen::Endian_Little;
#endif // defined(NRDP_GPU_LITTLE_ENDIAN)
    }
    assert(endian != Screen::Endian_Default);

    mFormat = mSurface->mFormat;

    if(endian == Screen::Endian_Big) {
        // flip the BGR and AlphaFirst bits
        mFormat = static_cast<Surface::Format>(mFormat ^ (Surface::BGR|Surface::AlphaFirst));
    }

    if((type == WriteLock && mSurface->mode() == Surface::Subsurface) || type == ReadLock) {
        lock_internal();
        mOwnsData = true;
        mBytesPerLine = mSurface->mRect.width * Surface::bytesPerPixel(mFormat);
        assert(mData != 0);
        if(type == ReadLock) {
            int bpl;
            void *data = 0;
            DIRECTFB_CHECK(mSurface->mSurface->Lock(mSurface->mSurface, DSLF_READ, &data, &bpl));
            for(int y = 0; y < mHeight; ++y)
                memcpy(mData + (y * mBytesPerLine), ((const char*)data) + (y * bpl), mBytesPerLine);
            DIRECTFB_CHECK(mSurface->mSurface->Unlock(mSurface->mSurface));
        }
    } else {
        unsigned dfb_lock = 0;
        if(type & ReadLock)
            dfb_lock |= DSLF_READ;
        if(type & WriteLock)
            dfb_lock |= DSLF_WRITE;
        void *data = 0;
        DIRECTFB_CHECK(mSurface->mSurface->Lock(mSurface->mSurface, static_cast<DFBSurfaceLockFlags>(dfb_lock), &data, &mBytesPerLine));
        mData = static_cast<unsigned char *>(data);
    }
    return true;
}

void Surface::Data::unlockSurface()
{
    if(mData) {
        if(!mOwnsData) {
            DIRECTFB_CHECK(mSurface->mSurface->Unlock(mSurface->mSurface));
            mData = 0;
        } else if(mLockType == ReadLock) {
            unlock_internal();
        }
    }
}

#ifdef GIBBON_SURFACEDECODER_PLATFORM
SurfaceDecoderPlatform::SurfaceDecoderPlatform(const Resource::SharedPtr &resource, IDirectFBImageProvider *provider) : SurfaceDecoder(resource, Flag_NeedSurface), mDFBProvider(provider)
{

}

SurfaceDecoderPlatform::~SurfaceDecoderPlatform()
{
    mDFBProvider->Release(mDFBProvider);
}

SurfaceDecoder::SharedPtr SurfaceDecoderPlatform::decoder(const Resource::SharedPtr &resource)
{
    const DataBuffer data = resource->responseData();
    DFBDataBufferDescription desc;
    desc.flags = DBDESC_MEMORY;
    desc.memory.data = data.constData();
    desc.memory.length = data.size();
    if(IDirectFB *dfb = GibbonApplication::instance()->getScreen()->getDirectFB()) {
        DFBPointer<IDirectFBDataBuffer> buf;
        if(dfb->CreateDataBuffer(dfb, &desc, &buf) == DFB_OK) {
            IDirectFBImageProvider *provider;
            if(buf->CreateImageProvider(buf, &provider) == DFB_OK)
                return SurfaceDecoder::SharedPtr(new SurfaceDecoderPlatform(resource, provider));
        }
    }
    return SurfaceDecoder::SharedPtr();
}

bool SurfaceDecoderPlatform::decode(DecodeInfo *result) const
{
    if(result->info.size.empty()) {
        DFBSurfaceDescription dsc;
        if(mDFBProvider->GetSurfaceDescription(mDFBProvider, &dsc) != DFB_OK)
            return false;
        result->info.size = Size(dsc.width, dsc.height);
        {
            DFBImageDescription img_dsc;
            if(mDFBProvider->GetImageDescription(mDFBProvider, &img_dsc) != DFB_OK)
                return false;
            if(img_dsc.caps & (DICAPS_ALPHACHANNEL|DICAPS_COLORKEY))
                result->info.flags |= Surface::Flags_Alpha;
        }
    }
    if(result->data.isValid()) {
        Surface::SharedPtr surface = result->data.getSurface();
        assert(surface);
        if(result->info.flags & Surface::Flags_Alpha)
            surface->setBlittingFlags(DSBLIT_BLEND_ALPHACHANNEL|DSBLIT_SRC_PREMULTIPLY);
        surface->mSurface->Clear(surface->mSurface, 0, 0, 0, 0);
        if(mDFBProvider->RenderTo(mDFBProvider, surface->mSurface, 0) != DFB_OK)
            return false;
    }
    return true;
}
#endif /* GIBBON_SURFACEDECODER_PLATFORM */
