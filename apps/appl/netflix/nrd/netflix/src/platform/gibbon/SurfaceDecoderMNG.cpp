/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#if !defined(GIBBON_NO_SURFACEDECODER)
#include "SurfaceDecoder.h"
#include "SurfaceCache.h"
#include <GibbonApplication.h>
#include "Screen.h"

#include <nrdbase/Log.h>
#include <nrdbase/PerformanceMarker.h>
#include <nrd/AppLog.h>

#define MNG_USE_SO
#include <libmng.h>

using namespace netflix;
using namespace netflix::gibbon;

Variant SurfaceDecoderMNG::libraryInfo()
{
    Variant version;

    version["name"] = "libmng";
    version["compiled"] = MNG_VERSION_TEXT;
    version["runtime"] = mng_version_text();
    return version;
}

class MNGTimer;

namespace MNG {
static inline mng_ptr alloc(mng_size_t size) { return static_cast<mng_ptr>(calloc(1, size)); }
static inline void free(mng_ptr ptr, mng_size_t) { ::free(ptr); }
static inline mng_bool processTerm(mng_handle, mng_uint8, mng_uint8, mng_uint32, mng_uint32) { return MNG_TRUE; }
static mng_bool openStream(mng_handle) { return MNG_TRUE; }
static inline mng_bool refresh(mng_handle, mng_uint32, mng_uint32, mng_uint32, mng_uint32) { return MNG_TRUE; }
static inline mng_bool trace(mng_handle, mng_int32 funcnr, mng_int32 funcseq, mng_pchar zFuncname)
{
    (void)funcnr;
    (void)funcseq;
    (void)zFuncname;
    NTRACE(TRACE_UI_ENGINE, "mng trace: funcnr: %d funcseq: %d zFuncname: %s", funcnr, funcseq, zFuncname);
    return MNG_TRUE;
}
};

class SurfaceControllerMNG : public Surface::Controller
{
public:
    SurfaceControllerMNG(Surface::SharedPtr surface, const DataBuffer &buf, const Size &size);
    ~SurfaceControllerMNG();

    virtual void stop();
    virtual void start();
    virtual int getCurrentFrame() const;
    virtual bool isRunning() const {
        ScopedMutex _lock(mMutex);
        return mRunning;
    }

    const Size mSize;
    int mRunning;
    int mCurrentFrame;

    DataBuffer mEncodedData;
    shared_ptr<MNGTimer> mTimer;

    friend class MNGTimer;
};

class MNGTimer : public SurfaceDecoders::Timer
{
public:
    MNGTimer(const shared_ptr<SurfaceControllerMNG> &controller,
             const DataBuffer &data,
             const Size &size);
    ~MNGTimer();
    virtual void execute();

    int nextImage(const shared_ptr<SurfaceControllerMNG> &controller);
    static mng_bool readData(mng_handle mng, mng_ptr buf, mng_uint32 size, mng_uint32p read);
    static mng_bool processHeader(mng_handle mng, mng_uint32 width, mng_uint32 height);
    static mng_bool closeStream(mng_handle);
    static mng_ptr getCanvasLine(mng_handle, mng_uint32);
    static mng_bool setTimer(mng_handle, mng_uint32);
    static mng_uint32 getTickCount(mng_handle);
    static mng_bool error(mng_handle, mng_int32, mng_int8, mng_chunkid, mng_uint32, mng_int32, mng_int32, mng_pchar);
    static mng_bool refresh(mng_handle mng, mng_uint32, mng_uint32, mng_uint32, mng_uint32);

    weak_ptr<SurfaceControllerMNG> mController;
    DataBuffer mMNGData;
    const Size mSize;

    mng_handle mMNG;

    int mEncodedDataPos, mFrameCount, mCurrentFrame;
    uint32_t mClock;
    int mDelay;
    Rect mRefreshRect;
    DataBuffer mPixelData;
    Surface::Format mFormat;

    struct FrameInfo {
        inline FrameInfo(int _delay, const Rect &_refreshRect, const DataBuffer &_data) : delay(_delay),
                                                                                          refreshRect(_refreshRect), data(_data) { }
        int delay;
        Rect refreshRect;
        DataBuffer data;
    };
    std::vector<FrameInfo> mFrames;
    enum { FramesStateAborted, FramesStateCollecting, FramesStateFull } mFramesState;
};

// ============================ MNGTimer ============================

MNGTimer::MNGTimer(const shared_ptr<SurfaceControllerMNG> &controller, const DataBuffer &data, const Size &size)
    : mController(controller), mMNGData(data), mSize(size),
      mMNG(0), mEncodedDataPos(0), mFrameCount(0), mCurrentFrame(0), mClock(0),
      mDelay(-1), mRefreshRect(0, 0, size.width, size.height),
      mFormat(Surface::Format_None), mFramesState(FramesStateCollecting)
{}

MNGTimer::~MNGTimer()
{
    if(mMNG)
        mng_cleanup(&mMNG);
}

void MNGTimer::execute()
{
    const int delay = nextImage(mController.lock());
    if(delay != -1) {
        if(SurfaceDecoders::SharedPtr decoders = SurfaceDecoders::instance())
            decoders->setTimeout(delay, shared_from_this());
    }
}

int MNGTimer::nextImage(const shared_ptr<SurfaceControllerMNG> &controller)
{
    if(!controller)
        return -1;
    {
        ScopedMutex lock(controller->mMutex);
        if(controller->mTimer.get() != this)
            return -1;
    }

    mng_retcode ret = MNG_INVALIDHANDLE;
    int delay;
    const bool collectFrame = mFramesState == FramesStateCollecting;
    if(mFramesState == FramesStateFull) {
        if(++mCurrentFrame == mFrameCount-1)
            mCurrentFrame = 0;
        const FrameInfo &info = mFrames[mCurrentFrame];
        mPixelData = info.data;
        delay = info.delay;
    } else {
        if(!mMNG) {
            mFrames.clear();
            assert(mSize.width > 0);
            assert(mSize.height > 0);
            mMNG = mng_initialize(this, MNG::alloc, MNG::free, MNG::trace);
            if(mMNG) {
                mng_set_doprogressive(mMNG, MNG_FALSE);
                mng_set_suspensionmode(mMNG, MNG_TRUE);

                mng_setcb_openstream(mMNG, MNG::openStream);
                mng_setcb_processterm(mMNG, MNG::processTerm);

                mng_setcb_closestream(mMNG, MNGTimer::closeStream);
                mng_setcb_errorproc(mMNG, MNGTimer::error);
                mng_setcb_getcanvasline(mMNG, MNGTimer::getCanvasLine);
                mng_setcb_gettickcount(mMNG, MNGTimer::getTickCount);
                mng_setcb_processheader(mMNG, MNGTimer::processHeader);
                mng_setcb_readdata(mMNG, MNGTimer::readData);
                mng_setcb_refresh(mMNG, MNGTimer::refresh);
                mng_setcb_settimer(mMNG, MNGTimer::setTimer);
                ret = mng_readdisplay(mMNG);
            }
        } else if(mCurrentFrame) {
            ret = mng_display_resume(mMNG);
        } else {
            ret = mng_display_reset(mMNG);
            if(ret == MNG_NOERROR)
                ret = mng_display(mMNG);
        }
        if(ret != MNG_NOERROR && ret != MNG_NEEDTIMERWAIT) {
            Log::error(TRACE_UI_ENGINE, "Got error from nextImage %d %d %d", ret, mFrameCount, mCurrentFrame);
            return -1;
        }
        delay = mDelay;
        if(mFrameCount != 1) {
            assert(mDelay != -1);

            if(++mCurrentFrame == mFrameCount) {
                mDelay = -1;
                mCurrentFrame = 0;
                mClock = 0;
            }
        }
    }
    {
        Surface::SharedPtr surface = controller->getSurface();
        if(!surface || !surface->isValid())
            return -1;

        Surface::Data lock(surface, Surface::Data::WriteLock);
        if(!lock.data())
            return -1;
        if(lock.format() != mFormat)
            return -1;
        const int bpl = int(mSize.width) * 4;
        const unsigned char *data = mPixelData.constData();
        if(bpl == lock.bytesPerLine()) {
            memcpy(lock.scanline(0), data, bpl * int(mSize.height));
        } else {
            for(int y=0; y<mSize.height; ++y)
                memcpy(lock.scanline(y), data + (bpl * y), bpl);
        }
        controller->onSurfaceChanged(mRefreshRect);
        if(collectFrame) {
            if(mCurrentFrame * mPixelData.size() > 5*1024*1024) {
                mFrames.clear();
                mFramesState = FramesStateAborted;
            } else {
                mFrames.push_back(FrameInfo(delay, mRefreshRect, mPixelData));
            }
        }
    }
    {
        ScopedMutex lock(controller->mMutex);
        controller->mCurrentFrame = mCurrentFrame;
        if(controller->mTimer.get() != this)
            return -1;
    }

    return delay;
}

mng_bool MNGTimer::refresh(mng_handle mng, mng_uint32 x, mng_uint32 y, mng_uint32 width, mng_uint32 height)
{
    MNGTimer *p = reinterpret_cast<MNGTimer*>(mng_get_userdata(mng));
    assert(p);
    p->mRefreshRect = Rect(x, y, width, height);
    return MNG_TRUE;
}


mng_bool MNGTimer::error(mng_handle mng, mng_int32 errorcode, mng_int8 /*severity*/, mng_chunkid chunkname,
                         mng_uint32 /*chunkseq*/, mng_int32 extra1, mng_int32 extra2, mng_pchar zErrortext)
{
    MNGTimer *p = reinterpret_cast<MNGTimer*>(mng_get_userdata(mng));
    assert(p);
    shared_ptr<SurfaceControllerMNG> controller = p->mController.lock();
    Log::error(TRACE_UI_ENGINE, "MNG Error: %d: %s chunk: %c%c%c%c: subcode: %d:%d %s",
               errorcode, zErrortext, (chunkname >> 24) & 0xff, (chunkname >> 16) & 0xff, (chunkname >> 8) & 0xff,
               (chunkname >> 0) & 0xff, extra1, extra2, controller ? controller->getSurface()->describe().c_str() : "(no loader)");
    return MNG_TRUE;
}

mng_bool MNGTimer::readData(mng_handle mng, mng_ptr buf, mng_uint32 size, mng_uint32p read)
{
    MNGTimer *p = reinterpret_cast<MNGTimer*>(mng_get_userdata(mng));
    assert(p);
    if(p->mEncodedDataPos < p->mMNGData.size()) {
        const int wanted = std::min<int>(size, p->mMNGData.size() - p->mEncodedDataPos);
        memcpy(buf, p->mMNGData.data() + p->mEncodedDataPos, wanted);
        p->mEncodedDataPos += wanted;
        *read = wanted;
        return MNG_TRUE;
    }
    return MNG_FALSE;
}

mng_bool MNGTimer::processHeader(mng_handle mng, mng_uint32 width, mng_uint32 height)
{
    MNGTimer *p = reinterpret_cast<MNGTimer*>(mng_get_userdata(mng));
    assert(p);

    int format;
    p->mFormat = GibbonApplication::instance()->getScreen()->getFormat(Screen::SurfaceFormat_Alpha);
    switch(p->mFormat) {
    case Surface::Format_RGBA32:
        format = MNG_CANVAS_BGRA8;
        break;
    case Surface::Format_RGBA32_Premultiplied:
        format = MNG_CANVAS_BGRA8_PM;
        break;
    case Surface::Format_BGRA32:
        format = MNG_CANVAS_RGBA8;
        break;
    case Surface::Format_BGRA32_Premultiplied:
        format = MNG_CANVAS_RGBA8_PM;
        break;
    case Surface::Format_ABGR32_Premultiplied:
        format = MNG_CANVAS_ARGB8_PM;
        break;
    case Surface::Format_ABGR32:
        format = MNG_CANVAS_ARGB8;
        break;
    default:
        Log::error(TRACE_UI_ENGINE, "Invalid format %d for mng", p->mFormat);
        return MNG_FALSE;
    }

    if(mng_set_canvasstyle(mng, format) != MNG_NOERROR) {
        Log::error(TRACE_UI_ENGINE, "Couldn't set canvasstyle to %d", format);
        return MNG_FALSE;
    }
    shared_ptr<SurfaceControllerMNG> loader = p->mController.lock();
    if(!loader || static_cast<int>(width) != p->mSize.width || static_cast<int>(height) != p->mSize.height)
        return MNG_FALSE;
    assert(p->mPixelData.isEmpty());
    p->mPixelData.resize(static_cast<int>(p->mSize.width) * static_cast<int>(p->mSize.height) * 4);
    return MNG_TRUE;
}

mng_ptr MNGTimer::getCanvasLine(mng_handle mng, mng_uint32 y)
{
    MNGTimer *p = reinterpret_cast<MNGTimer*>(mng_get_userdata(mng));
    assert(p);
    assert(!p->mPixelData.isEmpty());
    return reinterpret_cast<mng_ptr>(p->mPixelData.data() + (static_cast<int>(p->mSize.width) * 4 * y));
}

mng_uint32 MNGTimer::getTickCount(mng_handle mng)
{
    MNGTimer *p = reinterpret_cast<MNGTimer*>(mng_get_userdata(mng));
    assert(p);
    static unsigned last = 0;
    if(last != p->mClock)
        last = p->mClock;
    return p->mClock;
}

mng_bool MNGTimer::setTimer(mng_handle mng, mng_uint32 msecs)
{
    MNGTimer *p = reinterpret_cast<MNGTimer*>(mng_get_userdata(mng));
    assert(p);
    p->mClock += msecs;
    p->mDelay = msecs;
    return MNG_TRUE;
}

mng_bool MNGTimer::closeStream(mng_handle mng)
{
    MNGTimer *p = reinterpret_cast<MNGTimer*>(mng_get_userdata(mng));
    assert(p);
    if(p->mFramesState == FramesStateCollecting)
        p->mFramesState = FramesStateFull;
    p->mFrameCount = p->mCurrentFrame + 1;
    return MNG_TRUE;
}

// ============================ SurfaceControllerMNG ============================

SurfaceControllerMNG::SurfaceControllerMNG(Surface::SharedPtr surface, const DataBuffer &data, const Size &s)
    : Surface::Controller(surface), mSize(s), mRunning(0), mCurrentFrame(-1), mEncodedData(data)
{

}

SurfaceControllerMNG::~SurfaceControllerMNG()
{
    ScopedMutex lock(mMutex);
    if(mTimer)
        mTimer->stop();
}

int SurfaceControllerMNG::getCurrentFrame() const
{
    return mCurrentFrame;
}

void SurfaceControllerMNG::start()
{
    PERFORMANCE_MARKER_SCOPED("surfacecontrollermng.start");
    Surface::SharedPtr surface = getSurface();
    bool started = false;
    {
        ScopedMutex lock(mMutex);
        if(!mRunning++) {
            assert(!mTimer);
            mTimer.reset(new MNGTimer(static_pointer_cast<SurfaceControllerMNG>(shared_from_this()), mEncodedData, mSize));
            started = true;
        }
    }
    if(started && surface) {
        {
            Surface::Data lock(surface, Surface::Data::WriteLock);
            if(lock.data()) {
                const int bpl = int(lock.bytesPerLine());
                memset(lock.data(), 0, int(lock.getHeight()*bpl));
            }
        }
        if(SurfaceDecoders::SharedPtr decoders = SurfaceDecoders::instance())
            decoders->setTimeout(0, mTimer);
        onSurfaceStarted(true);
    }
}

void SurfaceControllerMNG::stop()
{
    bool stopped = false;
    {
        ScopedMutex lock(mMutex);
        assert(mRunning > 0);
        if(!--mRunning) {
            assert(mTimer);
            mTimer->stop();
            mTimer.reset();
            stopped = true;
        }
    }
    if(stopped)
        onSurfaceStarted(false);
}

// ============================ MNGReadSizeUserData ============================

struct MNGReadSize
{
    const DataBuffer buffer;
    int pos;
    Size size;
    static mng_bool readData(mng_handle mng, mng_ptr buf, mng_uint32 size, mng_uint32p read)
    {
        MNGReadSize *p = reinterpret_cast<MNGReadSize*>(mng_get_userdata(mng));
        assert(p);
        if(p->pos < p->buffer.size()) {
            const int wanted = std::min<int>(size, p->buffer.size() - p->pos);
            memcpy(buf, p->buffer.data() + p->pos, wanted);
            p->pos += wanted;
            *read = wanted;
            return MNG_TRUE;
        }
        return MNG_FALSE;
    }
    static mng_bool processHeader(mng_handle mng, mng_uint32 width, mng_uint32 height)
    {
        MNGReadSize *p = reinterpret_cast<MNGReadSize*>(mng_get_userdata(mng));
        assert(p);
        p->size = Size(width, height);
        return MNG_FALSE;
    }
    static mng_bool closeStream(mng_handle) { return MNG_TRUE; }
    static mng_ptr getCanvasLine(mng_handle, mng_uint32) { return 0; }
    static mng_bool setTimer(mng_handle, mng_uint32) { return MNG_TRUE; }
    static mng_uint32 getTickCount(mng_handle) { return 0; }
    static mng_bool error(mng_handle, mng_int32, mng_int8, mng_chunkid, mng_uint32, mng_int32, mng_int32, mng_pchar) { return MNG_TRUE; }

    static inline Size readSize(const DataBuffer &buffer)
    {
        MNGReadSize userData = { buffer, 0, Size() };
        mng_handle mng = mng_initialize(&userData, MNG::alloc, MNG::free, MNG::trace);
        if(!mng)
            return Size();

        mng_set_doprogressive(mng, MNG_FALSE);
        mng_set_suspensionmode(mng, MNG_TRUE);

        mng_setcb_openstream(mng, MNG::openStream);
        mng_setcb_processterm(mng, MNG::processTerm);
        mng_setcb_refresh(mng, MNG::refresh);

        mng_setcb_closestream(mng, MNGReadSize::closeStream);
        mng_setcb_errorproc(mng, MNGReadSize::error);
        mng_setcb_getcanvasline(mng, MNGReadSize::getCanvasLine);
        mng_setcb_gettickcount(mng, MNGReadSize::getTickCount);
        mng_setcb_processheader(mng, MNGReadSize::processHeader);
        mng_setcb_readdata(mng, MNGReadSize::readData);
        mng_setcb_settimer(mng, MNGReadSize::setTimer);

        mng_readdisplay(mng);
        mng_cleanup(&mng);
        return userData.size;
    }
};

SurfaceDecoder::SharedPtr SurfaceDecoderMNG::decoder(const Resource::SharedPtr &resource)
{
    const DataBuffer data = resource->responseData();
    const unsigned char sig[] = { 0x8A, 0x4D, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
    if(data.size() >= 8 && !memcmp(data.data(), sig, 8))
        return SurfaceDecoder::SharedPtr(new SurfaceDecoderMNG(resource));
    return SurfaceDecoder::SharedPtr();
}

bool SurfaceDecoderMNG::decode(DecodeInfo *result) const
{
    assert(result);
    const DataBuffer data = mResource->responseData();
    const Size mngSize(MNGReadSize::readSize(data));
    if(mngSize.empty())
        return false;
    if(result->info.size.empty()) {
        result->info.size = mngSize;
        result->info.flags |= Surface::Flags_Alpha;
    }
    if(result->data.isValid()) {
        Surface::SharedPtr surface = result->data.getSurface();
        assert(surface);
        if(!surface || !surface->isValid())
            return false;
        surface->setController(Surface::Controller::SharedPtr(new SurfaceControllerMNG(surface, data, mngSize)));
    }
    return true;
}
#endif
