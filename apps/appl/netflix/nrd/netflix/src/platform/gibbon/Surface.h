/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef SURFACE_H
#define SURFACE_H

#include <Style.h>
#include <Rect.h>
#include <Color.h>
#include <RectPacker.h>

#include <assert.h>
#include <vector>

#include <nrdbase/tr1.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Noncopyable.h>
#include <nrdbase/WeakPtrSet.h>
#include <nrdbase/MutexStack.h>
#include <nrdbase/AtomicCount.h>

#include <gibbon/config.h>

#include <nrdbase/DataBuffer.h>

#ifdef GIBBON_GRAPHICSENGINE_NOVA
# include "Texture.h"
#endif

#ifdef GIBBON_GRAPHICSENGINE_GLES2_EGL
# include <GLES2/gl2.h>
# include <EGL/egl.h>
# include <EGL/eglext.h>
#endif

#ifdef GIBBON_GRAPHICSENGINE_GLES2_IOS
# include <OpenGLES/ES2/gl.h>
#endif

#ifdef GIBBON_GRAPHICSENGINE_GLES2_OSX
# include <OpenGL/gl.h>
#endif

#ifdef GIBBON_GRAPHICSENGINE_GLES2_GLX
#ifndef GL_GLEXT_PROTOTYPES
#  define GL_GLEXT_PROTOTYPES 1
# endif
# include <GL/gl.h>
# include <GL/glext.h>
#endif

namespace netflix {
namespace gibbon {

class DisplayList;
#if defined(GIBBON_GRAPHICSENGINE_GLES2)
class OpenGLContext;
#endif

class Surface : public enable_shared_from_this<Surface>, public Noncopyable
{
public:
    typedef shared_ptr<Surface> SharedPtr;

    class WeakPtr
    {
    public:
        inline WeakPtr() : id(0)
        {
        }

        inline WeakPtr(const Surface::SharedPtr &shared)
        {
            *this = shared;
        }

        inline Surface::SharedPtr lock() const
        {
            assert(isSurfaceCacheLocked());
            if(Surface::SharedPtr surface = ptr.lock()) {
                if(id == surface->reuseCount())
                    return surface;
            }
            return Surface::SharedPtr();
        }

        inline void reset()
        {
            ptr.reset();
        }

        inline long use_count() const
        {
            assert(isSurfaceCacheLocked());
            return ptr.use_count();
        }

        WeakPtr &operator=(const Surface::SharedPtr &shared)
        {
            ptr = shared;
            if(shared)
                id = shared->reuseCount();
            else
                id = 0;
            return *this;
        }

        bool operator<(const WeakPtr& other) const
        {
            assert(isSurfaceCacheLocked());
#ifdef NRDP_REQUIRE_OWNERLESS
            std::owner_less<weak_ptr<Surface> > less;
            return less(ptr, other.ptr);
#elif defined(_XBOX_VER)
            return ptr.owner_before(other.ptr);
#else
            return ptr < other.ptr;
#endif
        }

#ifndef NDEBUG
        bool isSurfaceCacheLocked() const;
#endif

    private:
        unsigned int id;
        weak_ptr<Surface> ptr;
    };

    enum Flags
    {
        Flags_None           = 0x00,
        Flags_Alpha          = 0x01,
        Flags_Screen         = 0x02,
        Flags_Text           = 0x04,
        Flags_Accelerated    = 0x08,
        Flags_RenderTarget   = 0x10,
        Flags_Master         = 0x20
    };

    enum
    {
        Alpha         = 0x01,
        BitDepth32    = 0x02,
        BitDepth24    = 0x04,
        Premultiplied = 0x10,
        BGR           = 0x20,
        AlphaFirst    = 0x40,
        AlphaOnly     = 0x80
    };
    enum Format
    {
        Format_None = 0,
        Format_AlphaOnly = AlphaOnly,
        Format_RGBA32 = Alpha|BitDepth32,
        Format_RGBA32_Premultiplied = Format_RGBA32|Premultiplied,
        Format_BGRA32 = Format_RGBA32|BGR,
        Format_BGRA32_Premultiplied = Format_RGBA32_Premultiplied|BGR,
        Format_ABGR32 = Format_BGRA32|AlphaFirst,
        Format_ABGR32_Premultiplied = Format_ABGR32|Premultiplied,
        Format_RGB32 = BitDepth32,
        Format_BGR32 = BitDepth32|BGR,
        Format_RGB24 = BitDepth24,
        Format_BGR24 = BitDepth24|BGR
    };

    enum Mode {
        Master,
        Subsurface
    };

    Surface(Coordinate width = 0, Coordinate height = 0, uint8_t flags = Flags_None, int group = -1);
    ~Surface();

    inline static unsigned int estimateSize(Coordinate width, Coordinate height, unsigned int flags) {
        return (unsigned int)(width * height * bytesPerPixel(flags));
    }
    inline int bytesPerPixel() const { return bytesPerPixel(mFlags); }
    static int bytesPerPixel(Format format);
    static int bytesPerPixel(int flags);
    unsigned int calculateSize() const;
    static int atlasPadding();

    static int getGraphicsMemoryUsed();
    static int getGraphicsMemoryCapacity();

    inline void invalidate() { cleanup(); }
    bool isValid() const;

    void offset(Coordinate x, Coordinate y);
    void copy(const Rect &src, const Rect &dst);
    Surface::SharedPtr duplicate(const Rect &rect) const;

    bool matches(uint8_t flags) const;

    inline int group() const { return mGroup; }
    inline Mode mode() const { return (mGroup == -1) ? Master : ((mFlags & Flags_Master) ? Master : Subsurface); }
    void updateSubsurface();
    float masterUtilization() const;

    class Data
    {
    public:
        enum LockType {
            NoLock        = 0x00,
            ReadLock      = 0x01,
            WriteLock     = 0x02,
            ReadWriteLock = ReadLock|WriteLock
        };

        Data();
        Data(unsigned char *data, Coordinate width, Coordinate height, Format format, LockType type=ReadWriteLock);
        Data(const Surface::SharedPtr &surface, LockType type=ReadWriteLock);
        ~Data();

        inline bool isValid() const { return mData; }

        void unlock();
        void lock(const Surface::SharedPtr &surface, LockType type=ReadWriteLock);
        void lock(unsigned char* data, Coordinate width, Coordinate height, Format format, LockType type=ReadWriteLock);

        inline void premultiplyAlpha();
        inline void demultiplyAlpha();

        int bytesPerLine() const { return mBytesPerLine; }
        int bytesPerPixel() const { return Surface::bytesPerPixel(mFormat); }
        unsigned char *scanline(int y) { return mData + (mBytesPerLine * y); }
        const unsigned char *scanline(int y) const { return mData + (mBytesPerLine * y); }
        inline unsigned char *data() { return mData; }
        inline const unsigned char *data() const { return mData; }
        inline Coordinate getWidth() const { return mWidth; }
        inline Coordinate getHeight() const { return mHeight; }

        inline Format format() const { return mFormat; }
        inline Surface::SharedPtr getSurface() const { return mSurface; }

        inline Color getPixel(int x, int y) const;
        inline void setPixel(int x, int y, Color color);
    protected:
        bool lockSurface(LockType type);
        void unlockSurface();

        unsigned int dataSize() const { return (int)(mWidth * mHeight * ((mFormat == Surface::Format_AlphaOnly) ? 1 : 4)); }
        void lock_internal();
        void unlock_internal();

    private:
        Surface::SharedPtr mSurface;
        unsigned char *mData;
        Format mFormat;
        int mBytesPerLine;
        Coordinate mWidth, mHeight;
        LockType mLockType;
#if defined(GIBBON_GRAPHICSENGINE_DIRECTFB)
        bool mOwnsData;
#elif defined(GIBBON_GRAPHICSENGINE_GLES2)
        friend class Surface;
#endif
    };

    class Controller : public enable_shared_from_this<Controller> {
    public:
        typedef shared_ptr<Controller> SharedPtr;
        typedef weak_ptr<Controller> WeakPtr;

        Controller(const Surface::SharedPtr &surface);
        virtual ~Controller() { }

        virtual void stop() { onSurfaceStarted(false); }
        virtual void start() { onSurfaceStarted(true); }
        virtual bool isRunning() const { return false; }
        virtual int getCurrentFrame() const { return 0; }

        Surface::SharedPtr getSurface() const;

        class Listener
        {
        public:
            typedef shared_ptr<Listener> SharedPtr;
            typedef weak_ptr<Listener> WeakPtr;
            Listener() {}
            virtual ~Listener() {}

            virtual void onSurfaceStarted(const Surface::SharedPtr &, bool) { }
            virtual void onSurfaceChanged(const Surface::SharedPtr &, const Rect &) { }
        };
        void addListener(const Listener::SharedPtr &listener);
        void removeListener(const Listener::SharedPtr &listener);
        void onSurfaceChanged(const Rect &rect);
        void onSurfaceStarted(bool);

    protected:
        typedef WeakPtrSet<Listener> ListenerContainer;

        Surface::WeakPtr mSurface;
        mutable Mutex mMutex;
        ListenerContainer mListeners;
    };

    inline Controller::SharedPtr getController() const { return mController; }
    inline void setController(Controller::SharedPtr controller) { mController = controller; }

    inline uint8_t getFlags() const { return mFlags; }
    inline void setRect(const Rect& rect) { mRect = rect; }
    inline const Rect &getRect() const { return mRect; }
    inline Size getSize() const { return mRect.size(); }
    inline Coordinate getWidth() const { return mRect.width; }
    inline Coordinate getHeight() const { return mRect.height; }
    inline const Rect& getMasterRect() const { return mMaster ? mMaster->getRect() : mRect; }
    inline Surface::SharedPtr master() const { return mMaster; }
    inline rectpacker::RectPacker::Node *node() const { return mNode; }
    bool requestMaster();

    inline bool hasAlpha() const { return mFlags & Flags_Alpha; }
    inline bool isRenderTarget() const { return mFlags & Flags_RenderTarget; }

    inline void start() {
        if(mController)
            mController->start();
    }
    inline void stop() {
        if(mController)
            mController->stop();
    }

    bool isAccelerated() const;

    void upload(int x, int y, int width, int height, const unsigned char *data, int pad = 0);
    void upload(const Rect& rect, const unsigned char *data, int pad = 0) { upload((int)rect.x, (int)rect.y, (int)rect.width, (int)rect.height, data, pad); }
    void upload();

    inline void setDescription(const std::string &d) { mDescription = d; }
    inline std::string getDescription() const { return mDescription; }
    std::string describe() const;

    void modify(bool applyDisplayList=true);
    void reuse(uint8_t flags);
    inline unsigned int reuseCount() const { return mReuseCount.count(); }
    inline unsigned int serial() const { return mSerial; }

    inline DataBuffer cacheKey() const {
        if(mCacheKey.empty())
            return StringFormatter::sformat("GENERATED_%08d", serial());
        return mCacheKey;
    }
    inline void setCacheKey(const DataBuffer &cacheKey) { mCacheKey = cacheKey; }

    void applyDisplayList();
    inline shared_ptr<DisplayList> displayList() const { return mDisplayList; }
    inline void setDisplayList(const shared_ptr<DisplayList> &dl) { modify(false); mDisplayList = dl; }

    unsigned int dataSize() const { return (int)(mRect.width * mRect.height * ((mFlags & AlphaOnly) ? 1 : 4)); }
#if defined(GIBBON_GRAPHICSENGINE_DIRECTFB)
    void setBlittingFlags(int flags);
    inline Format format() const { return mFormat; }
    inline IDirectFBSurface *getDFBSurface() const { return mSurface; }
    static bool load(const DFBDataBufferDescription &desc, Surface::SharedPtr *surface, Size *size);
#elif defined(GIBBON_GRAPHICSENGINE_NOVA)
    inline StrongPtr<Texture> getTexture() const { return mSurface; }
#elif defined(GIBBON_GRAPHICSENGINE_GLES2)
    inline void createTexture() {
        if (!mTexture && mValid && !(mFlags & Flags_Screen))
            createTexture_internal();
    }
    inline GLuint getTexture() {
        if (mMaster)
            return mMaster->getTexture();
        createTexture();
        return mTexture;
    }
    friend class OpenGLContext;
#endif

protected:
    void updateSubsurface_sys();
    void cleanup();
    void init();

    static Mutex sMutex;

    Rect mRect;
    Surface::Data *mData;
    uint8_t mFlags;
    std::string mDescription;
    Controller::SharedPtr mController;
    shared_ptr<DisplayList> mDisplayList;

    unsigned int mSerial;
    AtomicCount mReuseCount;

    int mGroup;
    DataBuffer mCacheKey;
    Surface::SharedPtr mMaster;
    rectpacker::RectPacker::Node *mNode;

    unsigned char *mPixels;

#if defined(GIBBON_GRAPHICSENGINE_DIRECTFB)
    friend class SurfaceDecoderPlatform;
    IDirectFBSurface *mSurface;
    bool mVideo;
    Format mFormat;
    int mBlittingFlags;
#elif defined(GIBBON_GRAPHICSENGINE_NOVA)
    StrongPtr<Texture> mSurface;
#elif defined(GIBBON_GRAPHICSENGINE_NULL)
    bool mValid;
#elif defined(GIBBON_GRAPHICSENGINE_GLES2)
    void createTexture_internal();
    GLuint mTexture;
    bool mValid;
#endif
    friend class GraphicsEngine;
    friend class SurfaceCache;
    friend class SurfaceAtlas;
};

inline void Surface::Data::premultiplyAlpha()
{
    const unsigned int alphaOffset = (mFormat & Surface::AlphaFirst) ? 0 : 3;
    for(int y = 0; y < mHeight; ++y) {
        unsigned char *pixel = scanline(y);
        for(int x = 0; x < mWidth; ++x) {
            const unsigned char a = *(pixel + alphaOffset);
            switch (a) {
            case 255:
                pixel += 4;
                break;
            case 0:
                *reinterpret_cast<uint32_t*>(pixel) = 0;
                pixel += 4;
                break;
            default: {
                const float mult = a / 255.0;
                if(!alphaOffset)
                    ++pixel;
                for(int i=0; i<3; ++i, ++pixel)
                    *pixel = char(roundf(*pixel * mult));
                if(alphaOffset)
                    ++pixel;
                break; }
            }
        }
    }
}

inline void Surface::Data::demultiplyAlpha()
{
    const unsigned int alphaOffset = (mFormat & Surface::AlphaFirst) ? 0 : 3;
    for(int y = 0; y < mHeight; ++y) {
        unsigned char *pixel = scanline(y);
        for(int x = 0; x < mWidth; ++x) {
            const unsigned char a = *(pixel + alphaOffset);
            switch (a) {
            case 255:
            case 0:
                pixel += 4;
                break;
            default: {
                const float mult = 255.0 / a;
                if(!alphaOffset)
                    ++pixel;
                for(int i=0; i<3; ++i, ++pixel) {
                    const uint32_t px = static_cast<uint32_t>(roundf(*pixel * mult));
                    *pixel = static_cast<unsigned char>(std::min<uint32_t>(px, 255));
                }
                if(alphaOffset)
                    ++pixel;
                break; }
            }
        }
    }
}



inline Color Surface::Data::getPixel(int x, int y) const
{
    assert(x >= 0 && x < getWidth());
    assert(y >= 0 && y < getHeight());
    const unsigned char *p = scanline(y);
    const Format f = format();
    Color ret;
    assert(sizeof(Color) == 4);
    switch (f & (BitDepth32|BitDepth24|BGR|AlphaFirst|AlphaOnly)) {
    case BitDepth32:
        p += (x * 4);
        ret.b = *p++;
        ret.g = *p++;
        ret.r = *p++;
        ret.a = *p++;
        break;
    case BitDepth32|AlphaFirst:
        p += (x * 4);
        ret.a = *p++;
        ret.b = *p++;
        ret.g = *p++;
        ret.r = *p++;
        break;
    case BitDepth32|BGR:
        p += (x * 4);
        ret.r = *p++;
        ret.g = *p++;
        ret.b = *p++;
        ret.a = *p++;
        break;
    case BitDepth32|BGR|AlphaFirst:
        p += (x * 4);
        ret.a = *p++;
        ret.r = *p++;
        ret.g = *p++;
        ret.b = *p++;
        break;
    case BitDepth24:
        p += (x * 3);
        ret.b = *p++;
        ret.g = *p++;
        ret.r = *p++;
        break;
    case BitDepth24|BGR:
        p += (x * 3);
        ret.r = *p++;
        ret.g = *p++;
        ret.b = *p++;
        break;
    case AlphaOnly:
        p += x;
        ret.r = 255;
        ret.g = 255;
        ret.b = 255;
        ret.a = *p++;
        break;
    default:
        assert(0);
    }
    if ((f & Premultiplied) && (!mSurface || mSurface->hasAlpha())) {
        switch (ret.a) {
        case 255:
        case 0:
            break;
        default: {
            unsigned char *col[] = { &ret.r, &ret.g, &ret.b };
            for (int i=0; i<3; ++i) {
                int tmp = *col[i];
                tmp *= 255;
                tmp /= ret.a;
                *col[i] = static_cast<unsigned char>(tmp);
            }
            break; }
        }
    }
    return ret;
}

inline void Surface::Data::setPixel(int x, int y, Color c)
{
    assert(x >= 0 && x < getWidth());
    assert(y >= 0 && y < getHeight());

    const Format f = format();
    if ((f & Premultiplied) && (!mSurface || mSurface->hasAlpha())) {
        switch (c.a) {
        case 255:
            break;
        case 0:
            memset(&c, 0, sizeof(Color));
            break;
        default: {
            const float mult = c.a / 255.0;
            c.r = int(((float)c.r) * mult);
            c.g = int(((float)c.g) * mult);
            c.b = int(((float)c.b) * mult);
            break; }
        }
    }

    unsigned char *p = scanline(y);
    assert(sizeof(Color) == 4);
    switch (f & (BitDepth32|BitDepth24|BGR|AlphaFirst|AlphaOnly)) {
    case BitDepth32:
        p += (x * 4);
        *p++ = c.b;
        *p++ = c.g;
        *p++ = c.r;
        *p++ = c.a;
        break;
    case BitDepth32|AlphaFirst:
        p += (x * 4);
        *p++ = c.a;
        *p++ = c.b;
        *p++ = c.g;
        *p++ = c.r;
        break;
    case BitDepth32|BGR:
        p += (x * 4);
        *p++ = c.r;
        *p++ = c.g;
        *p++ = c.b;
        *p++ = c.a;
        break;
    case BitDepth32|BGR|AlphaFirst:
        p += (x * 4);
        *p++ = c.a;
        *p++ = c.r;
        *p++ = c.g;
        *p++ = c.b;
        break;
    case BitDepth24:
        p += (x * 3);
        *p++ = c.b;
        *p++ = c.g;
        *p++ = c.r;
        break;
    case BitDepth24|BGR:
        p += (x * 3);
        *p++ = c.r;
        *p++ = c.g;
        *p++ = c.b;
        break;
    case AlphaOnly:
        p += x;
        *p++ = c.a;
        break;
    default:
        assert(0);
        break;
    }
}

inline std::ostream &operator<<(std::ostream &out, const Surface &s)
{
    return out << s.describe();
}

inline int Surface::bytesPerPixel(Format format)
{
    switch (format & (BitDepth32|AlphaOnly)) {
    case AlphaOnly:
        return 1;
    case BitDepth32:
        return 4;
    default:
        break;
    }
    return 3;
}

}} // namespace netflix::gibbon

#endif // SURFACE_H
