#if !defined(GIBBON_NO_SURFACEDECODER)
#include "SurfaceDecoder.h"
#include "SurfaceCache.h"
#include "GibbonApplication.h"
#include "Animation.h"

#include <nrdbase/Log.h>
#include <nrd/AppLog.h>

extern "C" {
#include <jpeglib.h>
}
#include <setjmp.h>
#include <unistd.h>
#include <string.h>
#include <memory>

#include <gibbon/config.h>

using namespace netflix;
using namespace netflix::gibbon;

Variant SurfaceDecoderJPEG::libraryInfo()
{
    std::ostringstream os;
    Variant version;

#if defined(JPEG_LIB_VERSION_MAJOR) && defined(JPEG_LIB_VERSION_MINOR)
    version["name"] = "libjpeg";
    os << JPEG_LIB_VERSION_MAJOR << "." << JPEG_LIB_VERSION_MINOR;
    version["compiled"] = os.str();
    os.str("");
    version["runtime"] = os.str();
#endif
    return version;
}

struct JPEGMemorySource : public jpeg_source_mgr
{
    JPEGMemorySource(const DataBuffer &data);

    static void initSource(j_decompress_ptr j)
    {
        JPEGMemorySource *s = reinterpret_cast<JPEGMemorySource*>(j->src);
        s->next_input_byte = 0;
        s->bytes_in_buffer = 0;
        s->eoi[0] = 0xFF;
        s->eoi[1] = JPEG_EOI;
    }

    static boolean fillInputBuffer(j_decompress_ptr j)
    {
        JPEGMemorySource *s = reinterpret_cast<JPEGMemorySource*>(j->src);
        s->next_input_byte = reinterpret_cast<const JOCTET *>(s->mData + s->mPos);
        s->bytes_in_buffer = s->mLength - s->mPos;
        if (s->bytes_in_buffer <= 0) {
            s->next_input_byte = s->eoi;
            s->bytes_in_buffer = 2;
        } else {
            s->mPos += s->bytes_in_buffer;
        }
        return TRUE;
    }

    static void skipInputData(j_decompress_ptr j, long bytes)
    {
        JPEGMemorySource *s = reinterpret_cast<JPEGMemorySource*>(j->src);
        s->mPos += bytes;
        s->next_input_byte += bytes;
        s->bytes_in_buffer -= bytes;
    }

    static void termSource(j_decompress_ptr)
    {
    }

    const unsigned char *mData;
    int mLength;
    int mPos;
    JOCTET eoi[2];
};

JPEGMemorySource::JPEGMemorySource(const DataBuffer &data)
    : mData(data.data()), mLength(data.size()), mPos(0)
{
    jpeg_source_mgr::init_source = initSource;
    jpeg_source_mgr::fill_input_buffer = fillInputBuffer;
    jpeg_source_mgr::skip_input_data = skipInputData;
    jpeg_source_mgr::resync_to_restart = jpeg_resync_to_restart;
    jpeg_source_mgr::term_source = termSource;
}

struct JPEGError
{
    struct jpeg_error_mgr jpegError;
    jmp_buf setJumpBuffer;
    static inline void onJPEGError(j_common_ptr j)
    {
        JPEGError *e = reinterpret_cast<JPEGError*>(j->err);
        longjmp(e->setJumpBuffer, 1);
    }
    static inline void onJPEGErrorMessage(j_common_ptr j)
    {
        char buffer[JMSG_LENGTH_MAX];
        (*j->err->format_message)(j, buffer);
        Log::error(TRACE_UI_ENGINE, "JPEG Error: %s", buffer);
    }
};

static inline void decodeGrayScale(Surface::Format format, int width, unsigned char *row, JSAMPLE *scanLine)
{
    if (format & Surface::BitDepth32) {
        for (int x=0; x<width; ++x) {
            if (format & Surface::AlphaFirst) {
                *row++ = 0xff;
                *row++ = scanLine[x];
                *row++ = scanLine[x];
                *row++ = scanLine[x];
            } else {
                *row++ = scanLine[x];
                *row++ = scanLine[x];
                *row++ = scanLine[x];
                *row++ = 0xff;
            }
        }
    } else {
        for (int x=0; x<width; ++x) {
            *row++ = scanLine[x];
            *row++ = scanLine[x];
            *row++ = scanLine[x];
        }
    }
}

static inline void decodeColor(Surface::Format format, int width, unsigned char *row, JSAMPLE *scanLine)
{
    switch (format & (Surface::BGR|Surface::BitDepth32|Surface::BitDepth24|Surface::AlphaFirst)) {
    case Surface::BGR|Surface::BitDepth32|Surface::AlphaFirst:
        for (int x=0; x<width; ++x) {
            *row++ = 0xff;
            *row++ = scanLine[x * 3];
            *row++ = scanLine[(x * 3) + 1];
            *row++ = scanLine[(x * 3) + 2];
        }
        break;
    case Surface::BGR|Surface::BitDepth32:
        for (int x=0; x<width; ++x) {
            *row++ = scanLine[x * 3];
            *row++ = scanLine[(x * 3) + 1];
            *row++ = scanLine[(x * 3) + 2];
            *row++ = 0xff;
        }
        break;
    case Surface::BitDepth32|Surface::AlphaFirst:
        for (int x=0; x<width; ++x) {
            *row++ = 0xff;
            *row++ = scanLine[(x * 3) + 2];
            *row++ = scanLine[(x * 3) + 1];
            *row++ = scanLine[x * 3];
        }
        break;
    case Surface::BitDepth32:
        for (int x=0; x<width; ++x) {
            *row++ = scanLine[(x * 3) + 2];
            *row++ = scanLine[(x * 3) + 1];
            *row++ = scanLine[x * 3];
            *row++ = 0xff;
        }
        break;
    case Surface::BGR|Surface::BitDepth24:
        memcpy(row, scanLine, width * 3);
        break;
    case Surface::BitDepth24:
        for (int x=0; x<width; ++x) {
            *row++ = scanLine[(x * 3) + 2];
            *row++ = scanLine[(x * 3) + 1];
            *row++ = scanLine[x * 3];
        }
        break;
    }
}

SurfaceDecoder::SharedPtr SurfaceDecoderJPEG::decoder(const Resource::SharedPtr &resource)
{
    const DataBuffer data = resource->responseData();
    static const unsigned char sig[] = { 0xff, 0xd8 };
    if(data.size() > 2 && !memcmp(data.data(), sig, 2))
        return SurfaceDecoder::SharedPtr(new SurfaceDecoderJPEG(resource));
    return SurfaceDecoder::SharedPtr();
}

bool SurfaceDecoderJPEG::decode(DecodeInfo *result) const
{
    JPEGError jpeg_err;
    jpeg_decompress_struct j;
    bool started_decompress = false;
    jpeg_create_decompress(&j);
    j.err = jpeg_std_error(&jpeg_err.jpegError);
    jpeg_err.jpegError.error_exit = JPEGError::onJPEGError;
    jpeg_err.jpegError.output_message = JPEGError::onJPEGErrorMessage;

    enum { StaticScanLineSize = 1920 * 3 };
    unsigned char staticScanLine[StaticScanLineSize];

    JSAMPLE* scanLine = 0;

    if(setjmp(jpeg_err.setJumpBuffer)) {
        if(started_decompress)
            jpeg_finish_decompress(&j);
        jpeg_destroy_decompress(&j);
        if (scanLine != staticScanLine) {
            delete[] scanLine;
            scanLine = 0;
        }
        return false;
    }

    JPEGMemorySource src(mResource->responseData());
    j.src = &src;
    jpeg_read_header(&j, TRUE);

    if(result->info.size.empty()) {
        (void) jpeg_calc_output_dimensions(&j);
        result->info.size = Size(j.output_width, j.output_height);
        result->info.flags &= ~Surface::Flags_Alpha;
    }
    if(result->data.isValid()) {
        started_decompress = true;
#if defined(GIBBON_JPEG_FLOAT)
        j.dct_method = JDCT_FLOAT;
#elif defined(GIBBON_JPEG_SLOW)
        j.dct_method = JDCT_ISLOW;
#else
        j.dct_method = JDCT_IFAST;
#endif
#if defined(GIBBON_JPEG_UPSAMPLE)
        j.do_fancy_upsampling = TRUE;
#else
        j.do_fancy_upsampling = FALSE;
#endif
        jpeg_start_decompress(&j);

        if (j.output_components != 3 && j.output_components != 1) {
            started_decompress = false;
            jpeg_finish_decompress(&j);
            jpeg_destroy_decompress(&j);
            return false;
        }

        const Surface::Format format = result->data.format();
        unsigned char *pixels = result->data.data();
        if (!pixels) {
            started_decompress = false;
            jpeg_finish_decompress(&j);
            jpeg_destroy_decompress(&j);
            return false;
        }
        const int jpegStride = j.output_width * j.output_components;
        const int surfaceStride = result->data.bytesPerLine();
        assert(surfaceStride >= jpegStride);
        if (jpegStride <= StaticScanLineSize)
            scanLine = reinterpret_cast<JSAMPLE*>(staticScanLine);
        else
            scanLine = new JSAMPLE[jpegStride];
        while (j.output_scanline < j.output_height) {
            unsigned char *row = pixels + (j.output_scanline * surfaceStride);
            jpeg_read_scanlines(&j, &scanLine, 1);
            if (j.output_components == 1)
                decodeGrayScale(format, j.output_width, row, scanLine);
            else
                decodeColor(format, j.output_width, row, scanLine);
        }
        if (scanLine != staticScanLine) {
            delete[] scanLine;
            scanLine = 0;
        }
        started_decompress = false;
        jpeg_finish_decompress(&j);
    }
    jpeg_destroy_decompress(&j);
    return true;
}
#endif
