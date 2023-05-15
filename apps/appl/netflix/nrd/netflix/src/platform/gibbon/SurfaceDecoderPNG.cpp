#if !defined(GIBBON_NO_SURFACEDECODER)
#include "GibbonApplication.h"
#include "SurfaceDecoder.h"
#include "SurfaceCache.h"
#include "Screen.h"

#include <nrdbase/Log.h>
#include <nrd/AppLog.h>


extern "C" {
#define PNG_SKIP_SETJMP_CHECK
#include <png.h>
}

using namespace netflix::gibbon;
using namespace netflix;

Variant SurfaceDecoderPNG::libraryInfo()
{
    std::ostringstream os;
    Variant version;

    version["name"] = "libpng";
    os << PNG_LIBPNG_VER;
    version["compiled"] = os.str();

    png_uint_32 libpng_vn = png_access_version_number();
    os.str("");
    os << libpng_vn;
    version["runtime"] = os.str();
    return version;
}

struct Context
{
    Context(const DataBuffer &d, std::string const & u)
        : pos(0)
        , length(d.size())
        , data(d.constData())
        , url(u)
    {
    }

    static inline void readCallback(png_struct* png, png_byte* data, png_size_t length)
    {
        Context *context = static_cast<Context*>(png_get_io_ptr(png));
        if(context->length - context->pos < static_cast<int>(length)) {
            png_error(png, "Read Error");
        } else {
            memcpy(data, context->data + context->pos, length);
            context->pos += length;
        }
    }

    static inline void warningCallback(png_struct* png, png_const_charp message)
    {
        Context *context = static_cast<Context*>(png_get_error_ptr(png));
        Log::error(TRACE_UI_ENGINE, "PNG Error[%s]: %s", context->url.c_str(), static_cast<const char*>(message));
    }

    int pos, length;
    const unsigned char *data;
    std::string url;
};

SurfaceDecoder::SharedPtr SurfaceDecoderPNG::decoder(const Resource::SharedPtr &resource)
{
    const DataBuffer data = resource->responseData();
    if(data.size() >= 8 && !png_sig_cmp((unsigned char*)data.data(), 0, 8))
        return SurfaceDecoder::SharedPtr(new SurfaceDecoderPNG(resource));
    return SurfaceDecoder::SharedPtr();
}

// Note: setjmp/longjmp creates havoc with C++ destruction. The doDecode function isolates the setjmp/longjmp. -- jjb 2015/1/21
static bool doDecode(Context * context, SurfaceDecoderPNG::DecodeInfo *result);
bool SurfaceDecoderPNG::decode(DecodeInfo *result) const
{
    Context context(mResource->responseData(), mResource->url().str());

    return doDecode(&context, result);
}

static bool doDecode(Context * context, SurfaceDecoderPNG::DecodeInfo *result)
{
    png_struct *png = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (!png) {
        return false;
    }

    png_info *info = png_create_info_struct(png);
    png_info *end = png_create_info_struct(png);
    if(!info || !end) {
        png_destroy_read_struct(&png, &info, &end);
        return false;
    }

    png_set_error_fn(png, context, 0, Context::warningCallback);
    png_set_read_fn(png, context, Context::readCallback);

    if(setjmp(png_jmpbuf(png))) {
        png_destroy_read_struct(&png, &info, &end);
        return false;
    }

    png_set_sig_bytes(png, 8);
    context->pos += 8;

    png_read_info(png, info);

    int bitDepth, colorType, interlace;
    png_uint_32 width, height;

    png_get_IHDR(png, info, &width, &height, &bitDepth, &colorType, &interlace, 0, 0);

    if(!width || !height) {
        png_destroy_read_struct(&png, &info, &end);
        return false;
    }

    bool hasAlpha = colorType & PNG_COLOR_MASK_ALPHA;
    if(png_get_valid(png, info, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png);
        hasAlpha = true;
    }

    if(result->info.size.empty()) {
        result->info.size = Size(width, height);
        if(hasAlpha)
            result->info.flags |= Surface::Flags_Alpha;
        else
            result->info.flags &= ~Surface::Flags_Alpha;
    }

    if(result->data.isValid()) {
        switch (colorType) {
        case PNG_COLOR_TYPE_PALETTE:
            png_set_palette_to_rgb(png);
            break;
        case PNG_COLOR_TYPE_GRAY:
#if PNG_LIBPNG_VER >= 10209
            png_set_expand_gray_1_2_4_to_8(png);
#else
            png_set_gray_1_2_4_to_8(png);
#endif
            break;
        }

        const Surface::Format format = result->data.format();
        const bool alphaOnly = (format & Surface::AlphaOnly);
        assert(!alphaOnly || !(format & Surface::Premultiplied));

        if(bitDepth == 16)
            png_set_strip_16(png);
        else if(bitDepth < 8)
            png_set_packing(png);

        if(!alphaOnly && (colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_GRAY_ALPHA))
            png_set_gray_to_rgb(png);

        if(interlace != PNG_INTERLACE_NONE)
            png_set_interlace_handling(png);

        if(!alphaOnly)
            png_set_filler(png, 0xff, (format & Surface::AlphaFirst) ? PNG_FILLER_BEFORE : PNG_FILLER_AFTER);
        png_read_update_info(png, info);

        if(format & Surface::AlphaFirst)
            png_set_swap_alpha(png);

        if(!(format & Surface::BGR))
            png_set_bgr(png);

        png_get_IHDR(png, info, &width, &height, &bitDepth, &colorType, 0, 0, 0);

        if(!alphaOnly && bitDepth != 8) {
            png_destroy_read_struct(&png, &info, &end);
            return false;
        }

        switch (colorType) {
        case PNG_COLOR_TYPE_RGB:
        case PNG_COLOR_TYPE_RGB_ALPHA:
        case PNG_COLOR_TYPE_GRAY_ALPHA:
        case PNG_COLOR_TYPE_GRAY:
            break;
        default:
            png_destroy_read_struct(&png, &info, &end);
            return false;
        }

#ifdef PNG_READ_INTERLACING_SUPPORTED
        int number_passes = png_set_interlace_handling(png);
#else
        int number_passes = 1;
#endif
        while(number_passes--) {
            for (unsigned y=0; y<height; ++y)
                png_read_row(png, reinterpret_cast<png_byte*>(result->data.scanline(y)), 0);
        }

        png_read_end(png, end);
        if(format & Surface::Premultiplied)
            result->data.premultiplyAlpha();
    }

    png_destroy_read_struct(&png, &info, &end);
    return true;
}

struct WriteUserDataPNG
{
    WriteUserDataPNG() : scanLines(0), png(0), info(0) {}
    ~WriteUserDataPNG()
    {
        if(scanLines)
            png_free(png, scanLines);
        if(png)
            png_destroy_write_struct(&png, &info);
    }
    DataBuffer png_data;
    png_byte **scanLines;
    png_struct *png;
    png_info *info;
};

static void pngWriteFunc(png_struct *png, png_byte *data, png_size_t length)
{
    WriteUserDataPNG *writeData = reinterpret_cast<WriteUserDataPNG*>(png_get_io_ptr(png));
    writeData->png_data.append(data, length);
}

static void pngFlushFunc(png_struct *)
{
}

DataBuffer SurfaceDecoderPNG::encode(const Surface::SharedPtr &surface)
{
    WriteUserDataPNG p;
    DataBuffer result;
    if(!surface || !surface->isValid())
        return p.png_data;
    const Coordinate width = surface->getWidth();
    const Coordinate height = surface->getHeight();
    if(width <= 0 || height <= 0)
        return p.png_data;

    p.png = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if(!p.png)
        return p.png_data;
    p.info = png_create_info_struct(p.png);
    if(!p.info)
        return p.png_data;
    if(setjmp(png_jmpbuf(p.png)))
        return p.png_data;

    Surface::Data data;
    data.lock(surface, Surface::Data::ReadLock);
    if(!data.data())
        return p.png_data;

    const Surface::Format format = data.format();
    if(format & Surface::Premultiplied)
        data.demultiplyAlpha();
    if(!(format & Surface::BGR))
        png_set_bgr(p.png);
    if(format & Surface::AlphaFirst)
        png_set_swap_alpha(p.png);

    int colorType = 0;
    switch(format & (Surface::BitDepth32|Surface::BitDepth24|Surface::AlphaOnly)) {
    case Surface::BitDepth32:
        colorType = PNG_COLOR_TYPE_RGBA;
        break;
    case Surface::BitDepth24:
        colorType = PNG_COLOR_TYPE_RGB;
        break;
    case Surface::AlphaOnly:
        colorType = PNG_COLOR_TYPE_GRAY;
        break;
    default:
        return p.png_data;
    }

    p.scanLines = reinterpret_cast<png_byte**>(png_malloc(p.png, int(height * sizeof(png_byte *))));
    for (int y=0; y<height; ++y)
        p.scanLines[y] = reinterpret_cast<png_byte*>(data.scanline(y));

    png_set_IHDR(p.png, p.info, int(width), int(height), 8, colorType,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);

    png_set_write_fn(p.png, &p, pngWriteFunc, pngFlushFunc);

    png_set_rows(p.png, p.info, p.scanLines);
    png_write_png(p.png, p.info, PNG_TRANSFORM_IDENTITY, 0);
    return p.png_data;
}
#endif
