/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <DFBSurface.h>
#include <ISurface.h>
#include <Rect.h>
#include <iostream>
#include <directfb.h>




static IDirectFB* sDirectFB;
static IDirectFBFont* sDirectFBFont;

namespace netflix {

//static
void DFBSurface::setDirectFB(IDirectFB* dfb)
{
    sDirectFB = dfb;
}

//static
void DFBSurface::setDirectFBFont(IDirectFBFont* font)
{
    sDirectFBFont = font;
}

//static
IDirectFB* DFBSurface::getDirectFB()
{
    return sDirectFB;
}

using namespace std;


static DFBSurfacePixelFormat sScreenPixelFormat = DSPF_ARGB;

#if 0
static std::string pixelFormatToString(DFBSurfacePixelFormat pf)
{
    switch (pf)
    {
        case DSPF_ARGB1555: return "DSPF_ARGB1555";
        case DSPF_RGB16: return "DSPF_RGB16";
        case DSPF_RGB24: return "DSPF_RGB24";
        case DSPF_RGB32: return "DSPF_RGB32";
        case DSPF_ARGB: return "DSPF_ARGB";
//        case DSPF_A8case: return "DSPF_A8case";
        case DSPF_YUY2: return "DSPF_YUY2";
        case DSPF_RGB332: return "DSPF_RGB332";
        case DSPF_UYVY: return "DSPF_UYVY";
        case DSPF_I420: return "DSPF_I420";
        case DSPF_YV12: return "DSPF_YV12";
        case DSPF_LUT8: return "DSPF_LUT8";
        case DSPF_ALUT44: return "DSPF_ALUT44";
        case DSPF_AiRGB: return "DSPF_AiRGB";
//        case DSPF_A1case: return "DSPF_A1case";
        case DSPF_NV12: return "DSPF_NV12";
        case DSPF_NV16: return "DSPF_NV16";
        case DSPF_ARGB2554: return "DSPF_ARGB2554";
        case DSPF_ARGB4444: return "DSPF_ARGB4444";
//        case DSPF_RGBA4444: return "DSPF_RGBA4444";
        case DSPF_NV21: return "DSPF_NV21";
        case DSPF_AYUV: return "DSPF_AYUV";
//        case DSPF_A4case: return "DSPF_A4case";
        case DSPF_ARGB1666: return "DSPF_ARGB1666";
        case DSPF_ARGB6666: return "DSPF_ARGB6666";
        case DSPF_RGB18: return "DSPF_RGB18";
        case DSPF_LUT2: return "DSPF_LUT2";
        case DSPF_RGB444: return "DSPF_RGB444";
        case DSPF_RGB555: return "DSPF_RGB555";
//        case DSPF_BGR555: return "DSPF_BGR555";
//        case DSPF_RGBA5551: return "DSPF_RGBA5551";
//        case DSPF_YUV444P: return "DSPF_YUV444P";
//        case DSPF_ARGB8565: return "DSPF_ARGB8565";
//        case DSPF_AVYU: return "DSPF_AVYU";
//        case DSPF_VYUcase: return "DSPF_VYUcase";
        default:  break;
    }

    return "unknown";
}

static void dumpSurfaceInfo(IDirectFBSurface* surface)
{
    DFBAccelerationMask am;
    int width, height;
    surface->GetSize(surface, &width, &height);
    DFBResult result = surface->GetAccelerationMask(surface, surface, &am);
    DFBSurfacePixelFormat pf;
    surface->GetPixelFormat(surface, &pf);
    if (result == DFB_OK)
    {
        cout << "DirectFB Surface Info -------------------------------------" << surface << endl;
        cout << " width/height: " << width << "x" << height << endl;
        cout << " pixel format: " << pixelFormatToString(pf) << endl;
        cout << "    FillRectangle: " << ((am & DFXL_FILLRECTANGLE) != 0 ? "accelerated" : "slow") << endl;
        cout << "    DrawRectangle: " << ((am & DFXL_DRAWRECTANGLE) != 0 ? "accelerated" : "slow") << endl;
        cout << "    DrawLine: " << ((am & DFXL_DRAWLINE) != 0 ? "accelerated" : "slow") << endl;
        cout << "    FillTriangle: " << ((am & DFXL_FILLTRIANGLE) != 0 ? "accelerated" : "slow") << endl;
        cout << "    Blit: " << ((am & DFXL_BLIT) != 0 ? "accelerated" : "slow") << endl;
        cout << "    StretchBlit: " << ((am & DFXL_STRETCHBLIT) != 0 ? "accelerated" : "slow") << endl;
        cout << "    TexTriangles: " << ((am & DFXL_TEXTRIANGLES) != 0 ? "accelerated" : "slow") << endl;
//        cout << "    Blit2: " << ((dsc.acceleration_mask & DFXL_BLIT2) != 0 ? "accelerated" : "slow") << endl;
        cout << "    DrawString: " << ((am & DFXL_DRAWSTRING) != 0 ? "accelerated" : "slow") << endl;
    }
}
#endif

static DFBSurfacePixelFormat getDFBPixelFormat(ISurface::PixelFormat pf)
{
    DFBSurfacePixelFormat dfbPixelFormat;
    switch (pf)
    {
    case ISurface::PIXELFORMAT_RGB:
        dfbPixelFormat = DSPF_RGB24;
        break;
    case ISurface::PIXELFORMAT_ARGB:
        dfbPixelFormat = DSPF_ARGB;
        break;
    case ISurface::PIXELFORMAT_DEFAULT:
    default:
        dfbPixelFormat = sScreenPixelFormat;
        break;
    }

    return dfbPixelFormat;
}


// static
ISurface::SharedPtr ISurface::create()
{
    return ISurface::SharedPtr(new DFBSurface());
}


// static
ISurface::SharedPtr ISurface::create(unsigned width, unsigned height, ISurface::PixelFormat pf, unsigned long flags)
{
    return ISurface::SharedPtr(new DFBSurface(width, height, pf, flags));
}

// static
unsigned ISurface::estimateSize(unsigned width, unsigned height, PixelFormat pf)
{
    DFBSurfacePixelFormat dfbPixelFormat = getDFBPixelFormat(pf);
    return width * height * DFB_BYTES_PER_PIXEL(dfbPixelFormat);
}

unsigned DFBSurface::calculateSize()
{
    int w, h;
    mSurface->GetSize(mSurface, &w, &h);
    DFBSurfacePixelFormat pf;
    mSurface->GetPixelFormat(mSurface, &pf);
    return w * h * DFB_BYTES_PER_PIXEL(pf);
}

DFBSurface::DFBSurface() : mSurface(0), mAlpha(255), mScale(1.0)
{
}


DFBSurface::DFBSurface(unsigned width, unsigned height, ISurface::PixelFormat pf, unsigned long flags) : mSurface(0), mAlpha(255), mScale(1.0)
{
    IDirectFB* dfb = DFBSurface::getDirectFB();


#ifdef USE_DIRECTFB_SYSTEM
    if ((flags & ISurface::FLAGS_SCREEN_SURFACE) != 0)
    {
        IDFBSystem               *dfbSystem;
        IDirectFBDisplayLayer    *dfbLayer;
        DFBDisplayLayerConfig     layerConfig;
        DFBRectangle              src_rect,dst_rect;
        DFBColor                  color  = { 0, 0, 0, 0};

        /* Create DirectFB system interface */
        dfb->GetInterface( dfb, "IDFBSystem", NULL, NULL, (void **)&dfbSystem);

        /* Get LOSD Layer */
        dfb->GetDisplayLayer(dfb, DFOSD_ID_LOSD, &dfbLayer);

        /* SetLayerCooperativelevel */
        dfbLayer->SetCooperativeLevel( dfbLayer, DLSCL_ADMINISTRATIVE );

        dfbSystem->OpenLayer( dfbSystem, DFOSD_ID_LOSD);

        layerConfig.flags       = (DFBDisplayLayerConfigFlags)(DLCONF_BUFFERMODE|DLCONF_WIDTH|DLCONF_HEIGHT|DLCONF_PIXELFORMAT);
        layerConfig.buffermode  = DLBM_BACKSYSTEM;
        layerConfig.width       = 1280;
        layerConfig.height      = 720;
        sScreenPixelFormat = layerConfig.pixelformat = DSPF_ARGB;
        dfbLayer->SetConfiguration(dfbLayer, &layerConfig);

        src_rect.x=0;
        src_rect.y=0;
        src_rect.w=1280;
        src_rect.h=720;
//      dst_rect.x=attr[0].val;
//      dst_rect.y=attr[1].val;
//      dst_rect.w=attr[2].val;
//      dst_rect.h=attr[3].val;
        dst_rect.x=0;
        dst_rect.y=0;
        dst_rect.w=960;
        dst_rect.h=540;
        dfbSystem->SetLayerConfiguration( dfbSystem, DFOSD_ID_LOSD, &src_rect, &dst_rect, color, DSPF_ARGB);

        /* Get Layer Surface */
        DFBResult err = dfbLayer->GetSurface(dfbLayer, &mSurface);

        IDirectFBScreen *dfbScreen;
        dfbLayer->GetScreen(dfbLayer, &dfbScreen);
    }
#endif
    if(!mSurface)
    {
        DFBSurfaceDescription dsc;
        DFBResult err = DFB_OK;
        DFBSurfacePixelFormat dfbPixelFormat;

        switch (pf)
        {
//    case ISurface::PIXELFORMAT_RGB:
//        dfbPixelFormat = DSPF_RGB24;
//        break;
//    case ISurface::PIXELFORMAT_ARGB:
//        dfbPixelFormat = DSPF_ARGB;
//        break;
        case ISurface::PIXELFORMAT_DEFAULT:
        default:
            dfbPixelFormat = sScreenPixelFormat;
            break;
        }

        if ((flags & ISurface::FLAGS_SCREEN_SURFACE) != 0)
        {
            dsc.flags = DSDESC_CAPS;
            dsc.caps  = (DFBSurfaceCapabilities)(DSCAPS_PRIMARY | DSCAPS_DOUBLE | DSCAPS_VIDEOONLY);
        }
        else
        {
            dsc.flags = (DFBSurfaceDescriptionFlags)(DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT /*| DSDESC_CAPS*/);
            dsc.width = width;
            dsc.height = height;
            dsc.pixelformat = dfbPixelFormat;
            //dsc.caps = (DFBSurfaceCapabilities)(dsc.caps | DSCAPS_SYSTEMONLY);
        }

        err = dfb->CreateSurface(dfb, &dsc, &mSurface);
    }

    if (mSurface)
    {
        mSurface->Clear(mSurface, 0, 0, 0, 0);
#ifndef USE_DIRECTFB_SYSTEM
        if ((flags & ISurface::FLAGS_SCREEN_SURFACE) != 0)
        {
            DFBSurfacePixelFormat dfbpf;
            mSurface->GetPixelFormat(mSurface, &dfbpf);
            sScreenPixelFormat = dfbpf;
        }
#endif
        //dumpSurfaceInfo(mSurface);
    }
}

DFBSurface::~DFBSurface()
{
    if (mSurface)
    {
        mSurface->Release(mSurface);
    }
}

int DFBSurface::getHeight() const
{
    int w, h;
    mSurface->GetSize(mSurface, &w, &h);
    return h;
}

int DFBSurface::getWidth() const
{
    int w, h;
    mSurface->GetSize(mSurface, &w, &h);
    return w;
}

int DFBSurface::getDepth() const
{
    DFBSurfacePixelFormat pf;
    DFBResult err = mSurface->GetPixelFormat(mSurface, &pf);
    return err == DFB_OK ? DFB_BITS_PER_PIXEL(pf) : 0;
}

ISurface::PixelFormat DFBSurface::getPixelFormat() const
{
    DFBSurfacePixelFormat pf;
    DFBResult err = mSurface->GetPixelFormat(mSurface, &pf);

    if (err == DFB_OK)
    {
        switch (pf)
        {
        case DSPF_RGB32:
            return ISurface::PIXELFORMAT_RGB;
        case DSPF_ARGB:
            return ISurface::PIXELFORMAT_ARGB;
        default:
            break;
        }
    }

    return ISurface::PIXELFORMAT_DEFAULT;
}

void DFBSurface::setAlpha(unsigned alpha)
{
    mAlpha = alpha;
}

unsigned DFBSurface::getAlpha() const
{
    return mAlpha;
}

void DFBSurface::setClip(const Rect& r)
{
    const DFBRegion rgn = r.toDFBRegion();;
    mSurface->SetClip(mSurface, &rgn);
}

void DFBSurface::clearClip()
{
    mSurface->SetClip(mSurface, 0);
}

void DFBSurface::setCompositionMode(ISurface::CompositionMode mode)
{
//     if( mCompMode == mode )
//         return;

    mCompMode = mode;
    if (mode == COMPMODE_BLEND)
    {
        mSurface->SetDrawingFlags(mSurface, DSDRAW_BLEND);
        unsigned blitFlags = DSBLIT_BLEND_ALPHACHANNEL;
        if (mAlpha != 255)
        {
            blitFlags |= DSBLIT_BLEND_COLORALPHA;
        }
        mSurface->SetBlittingFlags(mSurface, (DFBSurfaceBlittingFlags)blitFlags);
    }
    else
    {
        mSurface->SetDrawingFlags(mSurface, DSDRAW_NOFX);
        mSurface->SetBlittingFlags(mSurface, DSBLIT_NOFX);
    }

}

void DFBSurface::blit(ISurface& srcSurface, const Rect& srcRect, const Rect& dstRect)
{
    IDirectFBSurface* srcDFBSurface = static_cast<DFBSurface&>(srcSurface).getDFBSurface();

    DFBResult err = DFB_OK;

    mSurface->SetColor(mSurface, 255, 255, 255, mAlpha);

    if (srcRect.width != dstRect.width || srcRect.height != dstRect.height)
    {
        const DFBRectangle srcDFBRect = srcRect.toDFBRect();
        DFBRectangle dstDFBRect = dstRect.toDFBRect();

            err = mSurface->StretchBlit(mSurface, srcDFBSurface, &srcDFBRect, &dstDFBRect);
    }
    else
    {
       err = mSurface->Blit(mSurface, srcDFBSurface, NULL, dstRect.x, dstRect.y);
    }

    if (err != DFB_OK)
    {
        cout << "DFBSurface::blit error: " << DirectFBErrorString(err) << endl;
    }
}

void DFBSurface::clear()
{
    mSurface->Clear(mSurface, 0, 0, 0, 0);
}

void DFBSurface::fill(const Rect& rect, const Color& c)
{
    mSurface->SetColor(mSurface, c.r, c.g, c.b, c.a);
    mSurface->FillRectangle(mSurface, rect.x, rect.y, rect.width, rect.height);
    mSurface->SetColor(mSurface, 0, 0, 0, 0xff);
}


void DFBSurface::drawRect(const Rect& rect, const Color& c)
{
    mSurface->SetColor(mSurface, c.r, c.g, c.b, c.a);
    mSurface->DrawRectangle(mSurface, rect.x, rect.y, rect.width, rect.height);
    mSurface->SetColor(mSurface, 0, 0, 0, 0xff);
}


void DFBSurface::drawText(const char *text, const Rect &rect)
{
    if (sDirectFBFont) {
        mSurface->SetColor(mSurface, 255, 255, 255, 255);
        mSurface->SetFont(mSurface, sDirectFBFont);
        mSurface->DrawString(mSurface, text, -1, rect.x, rect.y, DSTF_TOP);
        mSurface->SetColor(mSurface, 0, 0, 0, 0xff);
    }
}

void DFBSurface::flip(const Rect& r)
{
    const DFBRegion rgn = r.toDFBRegion();
    mSurface->Flip(mSurface, &rgn, static_cast<DFBSurfaceFlipFlags>(DSFLIP_BLIT));
}

bool DFBSurface::load(const char* path)
{
    IDirectFB* dfb = DFBSurface::getDirectFB();
    DFBResult err = DFB_OK;
    DFBSurfaceDescription dsc;
    IDirectFBSurface* surface = NULL;
    IDirectFBImageProvider* provider = NULL;

    err = dfb->CreateImageProvider(dfb, path, &provider);

    if (err == DFB_OK)
    {
        err = provider->GetSurfaceDescription(provider, &dsc);
    }

    if (err == DFB_OK)
    {
        dsc.flags = (DFBSurfaceDescriptionFlags)(dsc.flags | DSDESC_PIXELFORMAT);
        dsc.pixelformat = sScreenPixelFormat;
        err = dfb->CreateSurface(dfb, &dsc, &surface);
    }

    if (err == DFB_OK)
    {
        err = provider->RenderTo(provider, surface, NULL);
    }

    if (provider)
    {
        provider->Release(provider);
    }

    if (err == DFB_OK)
    {
        mSurface = surface;
        //dumpSurfaceInfo(mSurface);
    }
    else
    {
        cout << "DFBSurface::load error:" << err << endl;
        if (surface)
        {
            surface->Release(surface);
        }
    }

    return err == DFB_OK;
}


bool DFBSurface::load(const char* data, unsigned size)
{
    IDirectFB* dfb = DFBSurface::getDirectFB();
    DFBResult err = DFB_OK;
    DFBSurfaceDescription dsc;
    IDirectFBSurface* surface = NULL;
    IDirectFBImageProvider* provider = NULL;
    IDirectFBDataBuffer* buf = NULL;
    DFBDataBufferDescription dbd;

    dbd.flags = DBDESC_MEMORY;
    dbd.memory.data = data;
    dbd.memory.length = size;
    err = dfb->CreateDataBuffer (dfb, &dbd, &buf);

    if (err == DFB_OK)
    {
        err = buf->CreateImageProvider(buf, &provider);
    }

    if (err == DFB_OK)
    {
        err = provider->GetSurfaceDescription(provider, &dsc);
    }

    if (err == DFB_OK)
    {
        dsc.flags = (DFBSurfaceDescriptionFlags)(dsc.flags | DSDESC_PIXELFORMAT);
        dsc.pixelformat = sScreenPixelFormat;
        err = dfb->CreateSurface(dfb, &dsc, &surface);
    }

    if (err == DFB_OK)
    {
        err = provider->RenderTo(provider, surface, NULL);
    }

    if (provider)
    {
        provider->Release(provider);
    }

    if (buf)
    {
        buf->Release(buf);
    }

    if (err == DFB_OK)
    {
        mSurface = surface;
        //dumpSurfaceInfo(mSurface);
    }
    else
    {
        cout << "DFBSurface::load(data) error:" << err << endl;
        if (surface)
        {
            surface->Release(surface);
        }
    }

    return err == DFB_OK;
}

} // netflix

