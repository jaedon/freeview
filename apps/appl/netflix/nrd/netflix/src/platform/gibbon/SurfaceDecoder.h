/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __SURFACEDECODER_H__
#define __SURFACEDECODER_H__

#include <gibbon/config.h>

#include "Surface.h"
#include "Animation.h"

#include <nrdbase/tr1.h>
#include <nrdbase/DataBuffer.h>
#include <nrdbase/ObjectCount.h>

#include <nrdbase/Variant.h>
#include <nrdbase/Mutex.h>

#include <vector>

NRDP_DECLARE_OBJECTCOUNT(GibbonSurfaceDecodersTimer);

class SurfaceDecodersThread;
namespace netflix {
namespace gibbon {

class SurfaceDecoder : public enable_shared_from_this<SurfaceDecoder>
{
public:
    typedef shared_ptr<SurfaceDecoder> SharedPtr;
    typedef weak_ptr<SurfaceDecoder> WeakPtr;

    virtual ~SurfaceDecoder() { }
    virtual std::string description() const = 0;

    enum {
        Flag_None        = 0x00,
        Flag_NeedSurface = 0x01
    };
    inline uint8_t flags() const { return mFlags; }

    struct SurfaceInfo
    {
        inline SurfaceInfo() : flags(0), size(0, 0) { }
        uint8_t flags;
        Size size;
    };
    struct DecodeInfo
    {
        SurfaceInfo info;
        Surface::Data data;
    };
    virtual bool decode(DecodeInfo *result) const = 0;

protected:
    friend class SurfaceDecoders;
    inline SurfaceDecoder(const Resource::SharedPtr &resource, uint8_t flags=Flag_None) : mResource(resource), mFlags(flags) { }
    const Resource::SharedPtr mResource;
    uint8_t mFlags;
};

#if defined(HAVE_LIBJPEG) && !defined(GIBBON_NO_SURFACEDECODER_JPEG) && !defined(GIBBON_NO_SURFACEDECODER)
#define GIBBON_SURFACEDECODER_JPEG
class SurfaceDecoderJPEG : public SurfaceDecoder
{
public:
    static SurfaceDecoder::SharedPtr decoder(const Resource::SharedPtr &resource);
    static std::string extension() { return "jpg"; }
    static Variant libraryInfo();

    virtual std::string description() const { return extension(); }
    virtual bool decode(DecodeInfo *result) const;

private:
    inline SurfaceDecoderJPEG(const Resource::SharedPtr &resource) : SurfaceDecoder(resource) { }
};
#endif

#if defined(HAVE_LIBPNG) && !defined(GIBBON_NO_SURFACEDECODER_PNG) && !defined(GIBBON_NO_SURFACEDECODER)
#define GIBBON_SURFACEDECODER_PNG
class SurfaceDecoderPNG : public SurfaceDecoder
{
public:
    static SurfaceDecoder::SharedPtr decoder(const Resource::SharedPtr &resource);
    static DataBuffer encode(const Surface::SharedPtr &surface);
    static std::string extension() { return "png"; }
    static Variant libraryInfo();

    virtual std::string description() const { return extension(); }
    virtual bool decode(DecodeInfo *result) const;

private:
    inline SurfaceDecoderPNG(const Resource::SharedPtr &resource) : SurfaceDecoder(resource) { }
};
#endif

#if defined(HAVE_LIBMNG) && !defined(GIBBON_NO_SURFACEDECODER_MNG) && !defined(GIBBON_NO_SURFACEDECODER)
#define GIBBON_SURFACEDECODER_MNG
class SurfaceDecoderMNG : public SurfaceDecoder
{
public:
    static SurfaceDecoder::SharedPtr decoder(const Resource::SharedPtr &resource);
    static std::string extension() { return "mng"; }
    static Variant libraryInfo();

    virtual std::string description() const { return extension(); }
    virtual bool decode(DecodeInfo *result) const;

private:
    inline SurfaceDecoderMNG(const Resource::SharedPtr &resource) : SurfaceDecoder(resource, Flag_NeedSurface) { }
};
#endif

#if defined(HAVE_LIBWEBP) && !defined(GIBBON_NO_SURFACEDECODER_WEBP) && !defined(GIBBON_NO_SURFACEDECODER)
#define GIBBON_SURFACEDECODER_WEBP
class SurfaceDecoderWebP : public SurfaceDecoder
{
public:
    static SurfaceDecoder::SharedPtr decoder(const Resource::SharedPtr &resource);
    static std::string extension() { return "webp"; }
    static Variant libraryInfo();

    virtual std::string description() const { return extension(); }
    virtual bool decode(DecodeInfo *result) const;

private:
    inline SurfaceDecoderWebP(const Resource::SharedPtr &resource) : SurfaceDecoder(resource) { }
};
#endif

#if defined(GIBBON_SURFACEDECODER_PLATFORM)
class SurfaceDecoderPlatform : public SurfaceDecoder
{
public:
    ~SurfaceDecoderPlatform();

    static SurfaceDecoder::SharedPtr decoder(const Resource::SharedPtr &resource);

    virtual std::string description() const { return "platform"; }
    virtual bool decode(DecodeInfo *result) const;

private:
#if defined(GIBBON_GRAPHICSENGINE_DIRECTFB)
    SurfaceDecoderPlatform(const Resource::SharedPtr &resource, IDirectFBImageProvider *provider);
    IDirectFBImageProvider *mDFBProvider;
#endif
};
#endif

class SurfaceDecoders
{
public:
    typedef shared_ptr<SurfaceDecoders> SharedPtr;

    ~SurfaceDecoders();

    static SurfaceDecoders::SharedPtr instance();
    static void cleanup();

    std::vector<std::string> formats() const;

    SurfaceDecoder::SharedPtr decoder(const Resource::SharedPtr &resource) const;

    bool encode(const Surface::SharedPtr &surface, const std::string &file) const;
    DataBuffer encode(const Surface::SharedPtr &surface) const;

    void dump();
    Variant libraryInfo() const;

    class Timer : public enable_shared_from_this<Timer>
    {
    public:
        Timer() : mStopped(false) { NRDP_OBJECTCOUNT_REF(GibbonSurfaceDecodersTimer, this); }
        virtual ~Timer() { NRDP_OBJECTCOUNT_DEREF(GibbonSurfaceDecodersTimer, this); }
        virtual void execute() = 0;
        void stop();
        inline bool isStopped() const { return mStopped; } // always called with lock held
    private:
        bool mStopped;
    };
    void setTimeout(int ms, const shared_ptr<Timer> &timer);
protected:
    SurfaceDecodersThread *mThread;
    friend class SurfaceDecoder;
    SurfaceDecoders();
};

}} // namespace netflix::gibbon

#endif /* __SURFACEDECODER_H__ */
