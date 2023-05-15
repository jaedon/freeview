#if !defined(GIBBON_NO_SURFACEDECODER)
#include "SurfaceDecoder.h"
#include "SurfaceCache.h"
#include "GibbonApplication.h"

#include <webp/decode.h>
#include <iomanip>

using namespace netflix;
using namespace netflix::gibbon;

Variant SurfaceDecoderWebP::libraryInfo()
{
    std::ostringstream os;
    Variant version;

    version["name"] = "libwebp";
    //os << std::hex << std::setw(2) << std::setfill('0') << WEBP_DECODER_ABI_VERSION;
    os.str("");
    version["compiled"] = os.str();
    os.str("");
    os << std::hex << std::setw(2) << std::setfill('0') << WebPGetDecoderVersion();
    version["runtime"] = os.str();
    return version;
}

SurfaceDecoder::SharedPtr SurfaceDecoderWebP::decoder(const Resource::SharedPtr &resource)
{
    const DataBuffer data = resource->responseData();
    if(WebPGetInfo(data.constData(), data.length(), 0, 0))
        return SurfaceDecoder::SharedPtr(new SurfaceDecoderWebP(resource));
    return SurfaceDecoder::SharedPtr();
}

bool SurfaceDecoderWebP::decode(DecodeInfo *result) const
{
    assert(result);

    int width, height;
    const DataBuffer data = mResource->responseData();
    if(!WebPGetInfo(data.constData(), data.length(), &width, &height))
        return false;

    if(width <= 0 || height <= 0)
        return false;

    WebPBitstreamFeatures features;
    if(WebPGetFeatures(data.constData(), data.length(), &features) != VP8_STATUS_OK)
        return false;

    if(result->info.size.empty()) {
        result->info.size = Size(width, height);
        if(features.has_alpha)
            result->info.flags |= Surface::Flags_Alpha;
        else
            result->info.flags &= ~Surface::Flags_Alpha;
    }

    if(result->data.isValid()) {
        WebPDecoderConfig config;
        if(!WebPInitDecoderConfig(&config))
            return false;
        config.input = features;

        const Surface::Format format = result->data.format();
        const int stride = result->data.bytesPerLine();

        WebPDecBuffer &output = config.output;
        output.u.RGBA.rgba = result->data.data();
        output.u.RGBA.stride = stride;
        output.u.RGBA.size = stride * height;
        output.is_external_memory = 1;

        int formatMask = Surface::BitDepth32|Surface::BitDepth24|Surface::BGR|Surface::AlphaFirst;
#ifdef HAVE_LIBWEBP_PREMUL
        formatMask |= Surface::Premultiplied;
#endif

        switch(format & formatMask) {
#ifdef HAVE_LIBWEBP_PREMUL
        case Surface::BitDepth32|Surface::BGR|Surface::AlphaFirst|Surface::Premultiplied:
            output.colorspace = MODE_Argb;
            break;
        case Surface::BitDepth32|Surface::BGR|Surface::Premultiplied:
            output.colorspace = MODE_rgbA;
            break;
        case Surface::BitDepth32|Surface::Premultiplied:
            output.colorspace = MODE_bgrA;
            break;
#endif
        case Surface::BitDepth32|Surface::BGR|Surface::AlphaFirst:
            output.colorspace = MODE_ARGB;
            break;
        case Surface::BitDepth32|Surface::BGR:
            output.colorspace = MODE_RGBA;
            break;
        case Surface::BitDepth32:
            output.colorspace = MODE_BGRA;
            break;
        case Surface::BitDepth24|Surface::BGR:
            output.colorspace = MODE_RGB;
            break;
        case Surface::BitDepth24:
            output.colorspace = MODE_BGR;
            break;
        case Surface::BitDepth32|Surface::AlphaFirst:
            // not supported?
        default:
            return false;
        }

        if(WebPDecode(data.constData(), data.length(), &config) != VP8_STATUS_OK)
            return false;
        WebPFreeDecBuffer(&output);

#ifndef HAVE_LIBWEBP_PREMUL
        if(format & Surface::Premultiplied)
            result->data.premultiplyAlpha();
#endif
    }
    return true;
}
#endif
