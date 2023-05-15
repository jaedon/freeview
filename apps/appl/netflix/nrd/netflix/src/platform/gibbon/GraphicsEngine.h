/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __GRAPHICSENGINE_H__
#define __GRAPHICSENGINE_H__

#include <gibbon/config.h>

#include <Animation.h>
#include <GibbonApplication.h>
#include <Color.h>
#include <Surface.h>
#include <DisplayList.h>
#include <nrdbase/Stopwatch.h>
#if defined(GIBBON_GRAPHICSENGINE_GLES2)
#include <OpenGLFBO.h>
#include <Transform.h>
#endif
#if (defined(GIBBON_GRAPHICSENGINE_DIRECTFB) && defined(GIBBON_DFBSCREEN_VSYNC)) || defined(GIBBON_SCREEN_FLIPTHREAD)
# define GIBBON_SCREEN_FLIPVSYNC
#endif

#include <nrdbase/Compiler.h>
#include <nrdbase/Instrumentation.h>

#include <nrd/AppLog.h>

//=============================================================================
// If you turn this on, the GL graphics engine will dump out the number of each
// type of operation it performed between the time it was created against the
// screen and when flip was called.

//#define GIBBON_GL_LOG_OPERATIONS 1

//=============================================================================

namespace netflix {
namespace gibbon {

class DrawVBO;
class OpenGLContext;
class GraphicsTextData;
class GraphicsGLTextData;

class GraphicsEngine
{
public:
    enum CompositionMode
    {
        CompositionMode_Source,
        CompositionMode_SourceOver
    };

    struct State
    {
        inline State(const Rect &rect=Rect()) { reset(rect); }
        inline void reset(const Rect &rect) {
            compositionMode = CompositionMode_SourceOver;
            smoothScale = GraphicsEngine::defaultSmoothScale();
            opacity = 1.0;
            clip = rect;
            scale = 1.0;
            hasClip = false;
        }
        CompositionMode compositionMode;
        bool smoothScale;
        float opacity;
        float scale;
        Rect clip;
        bool hasClip;
    };

    struct Operation
    {
        inline Operation() : name(0), engine(0), duration(false) { }
        inline Operation(const char *_name, const Variant &_data=Variant(), GraphicsEngine *_engine=0) {
            start(_name, _data, _engine);
        }
        inline ~Operation() {
            if(engine) {
                engine->flush();
                stop();
                engine->appendOperation(*this);
            }
        }
        inline Operation(const Operation &operation) : engine(0) {
            name = operation.name;
            data = operation.variant();
            duration = operation.duration;
            duration.stop();
        }
        inline void stop() { duration.stop(); }
        inline void start(const char *_name, GraphicsEngine *_engine) {
            start(_name, Variant(), _engine);
        }
        inline void start(const char *_name, const Variant &_data=Variant(), GraphicsEngine *_engine=0) {
            name = _name;
            data = _data;
            engine = _engine;
            if(engine) {
                duration.start();
                state = engine->getState();
            }
        }
        Variant variant() const {
            Variant result = data;
            if(engine) {
                result["compositionMode"] = state.compositionMode;
                result["smoothScale"] = state.smoothScale;
                if(state.hasClip)
                    result["clip"] = state.clip.toString();
                result["opacity"] = state.opacity;
                result["scale"] = state.scale;
            }
            return result;
        }

        State state;
        const char *name;
        GraphicsEngine *engine;
        Variant data;
        Stopwatch duration;
    };

    static void initCommands();

    inline GraphicsEngine() : mInstrument(false), mTextData(0) {  }
    inline GraphicsEngine(Surface::SharedPtr surface) : mInstrument(false), mTextData(0) { begin(surface); }
    inline GraphicsEngine(DisplayList::SharedPtr displayList) : mInstrument(false), mTextData(0) { begin(displayList); }
    inline ~GraphicsEngine() { end(); }

    inline bool isDisplayList() const { return static_cast<bool>(mDisplayList); }

    static inline bool defaultSmoothScale() { return sDefaultSmoothScale; }
    static inline void setDefaultSmoothScale(bool b) { sDefaultSmoothScale = b; }

    inline void setCompositionMode(CompositionMode mode)
    {
        if(mState.compositionMode != mode)
            setCompositionMode_internal(mode);
    }
    inline CompositionMode getCompositionMode() const { return mState.compositionMode; }

    inline void setSmoothScale(bool smooth)
    {
        if(mState.smoothScale != smooth)
            setSmoothScale_internal(smooth);
    }
    inline bool smoothScale() const { return mState.smoothScale; }

    inline void setInstrument(bool instrument)
    {
        if(mInstrument)
            sendInstrumentations();
        if(instrument)
            INSTRUMENTATION_INTERVAL_SWITCHED_START("gibbon.graphics.render");
        mInstrument = instrument;
    }
    inline bool instrument() const { return mInstrument; }

    inline void begin(const DisplayList::SharedPtr &displayList)
    {
        assert(!mDisplayList && !mSurface);
        GibbonApplication::checkRenderThread();
        assert(displayList);
        if(Surface::SharedPtr surface = displayList->getSurface()) {
            begin(surface);
        } else {
            mSurface.reset();
            mDisplayList = displayList;
            mState.reset(Rect(0, 0, displayList->getSize()));
            begin_sys();
        }
    }

    inline void begin(const Surface::SharedPtr &surface, bool modify=true)
    {
        if(modify)
            surface->modify();
        assert(!mDisplayList && !mSurface);
        GibbonApplication::checkRenderThread();
        assert(surface && surface->isValid() && surface->isRenderTarget());
        mSurface = surface;
        mDisplayList.reset();
        mState.reset(surface->getRect());
        begin_sys();
        setCompositionMode_internal(mState.compositionMode);
        setClip_internal(mState.clip, true);
        setSmoothScale_internal(mState.smoothScale);
        setOpacity_internal(mState.opacity);
        if(NRDP_UNLIKELY(mInstrument))
            flush();
    }
    inline void end()
    {
        if(NRDP_UNLIKELY(mInstrument))
            sendInstrumentations();
        if(isDisplayList()) {
            end_sys();
            mDisplayList.reset();
        } else if(mSurface) {
            end_sys();
            mSurface.reset();
        }
        assert(!mTextData);
    }

    inline const State &getState() const { return mState; }
    inline void setState(const State &state)
    {
        setCompositionMode(state.compositionMode);
        setClip(state.clip, !state.hasClip);
        setSmoothScale(state.smoothScale);
        setOpacity(state.opacity);
        setScale(state.scale);
    }

    inline Surface::SharedPtr getSurface() const { return mSurface; }
    inline DisplayList::SharedPtr getDisplayList() const { return mDisplayList; }

    inline void push(const Rect &rect)
    {
        if(isDisplayList())
            mDisplayList->push(rect);
        if(NRDP_UNLIKELY(mInstrument))
            appendOperation(Operation("push", Variant::StringMap("rect", rect.toString())));
    }
    inline void pop()
    {
        if(isDisplayList())
            mDisplayList->pop();
        if(NRDP_UNLIKELY(mInstrument))
            appendOperation(Operation("pop"));
    }
    inline void comment(const std::string &msg)
    {
        if(isDisplayList())
            mDisplayList->appendComment(msg);
        else
            Log::info(TRACE_GIBBON_GRAPHICS, "Comment: %s", msg.c_str());
        if(NRDP_UNLIKELY(mInstrument))
            appendOperation(Operation("comment", msg));
    }

    inline void apply(const DisplayList::SharedPtr &displayList, const Rect &srcRect, const Rect &dstRect, int steps = -1)
    {
        assert(displayList);
        displayList->apply(this, srcRect, dstRect, steps);
    }
    inline void blit(const Surface::SharedPtr &srcSurface, const Rect &srcRect, const Rect &dstRect, const Color &color)
    {
        if(!srcSurface || !srcSurface->isValid())
            return;

        Operation op;
        if(NRDP_UNLIKELY(mInstrument))
            op.start("blit", Variant::StringMap("srcRect", srcRect.toString())
                     ("pixels", dstRect.width*dstRect.height)
                     ("dstRect", dstRect.toString())("color", color.toString())("src", srcSurface->describe()), this);
        if(isDisplayList()) {
            blit_dl(srcSurface, srcRect, dstRect, &color);
        } else if(mSurface) {
#if defined(GIBBON_GRAPHICSENGINE_GLES2) && 0
            if(shared_ptr<DisplayList> displayList = srcSurface->displayList()) {
                apply(displayList, srcRect, dstRect);
                return;
            }
#else
            if(srcSurface->displayList())
                srcSurface->applyDisplayList();
#endif
            blit_sys(srcSurface, srcRect, dstRect, &color);
        }
    }
    inline void blit(const Surface::SharedPtr &srcSurface, const Rect &srcRect, const Rect &dstRect)
    {
        if(!srcSurface || !srcSurface->isValid())
            return;

        Operation op;
        if(NRDP_UNLIKELY(mInstrument))
            op.start("blit", Variant::StringMap("srcRect", srcRect.toString())
                     ("pixels", dstRect.width*dstRect.height)
                     ("dstRect", dstRect.toString())("src", srcSurface->describe()), this);
        if(isDisplayList()) {
            blit_dl(srcSurface, srcRect, dstRect, 0);
        } else if(mSurface) {
#if defined(GIBBON_GRAPHICSENGINE_GLES2) && 0
            if(shared_ptr<DisplayList> displayList = srcSurface->displayList()) {
                apply(displayList, srcRect, dstRect);
                return;
            }
#else
            if(srcSurface->displayList())
                srcSurface->applyDisplayList();
#endif
            blit_sys(srcSurface, srcRect, dstRect, 0);
        }
    }

    inline void batchBlit(const Surface::SharedPtr &srcSurface, const std::vector<Rect>& src, const std::vector<Rect>& dst)
    {
        if(!srcSurface || !srcSurface->isValid())
            return;

        Operation op;
        if(NRDP_UNLIKELY(mInstrument))
            op.start("batchBlit", this);
        if(isDisplayList()) {
            batchBlit_dl(srcSurface, src, dst, 0);
        } else if(mSurface) {
            if(srcSurface->displayList())
                srcSurface->applyDisplayList();
            batchBlit_sys(srcSurface, src, dst, 0);
        }
    }
    inline void batchBlit(const Surface::SharedPtr &srcSurface, const std::vector<Rect> &src, const std::vector<Rect>& dst, const Color &color)
    {
        if(!srcSurface || !srcSurface->isValid())
            return;

        Operation op;
        if(NRDP_UNLIKELY(mInstrument))
            op.start("batchBlit", this);
        if(isDisplayList()) {
            batchBlit_dl(srcSurface, src, dst, &color);
        } else if(mSurface) {
            if(srcSurface->displayList())
                srcSurface->applyDisplayList();
            batchBlit_sys(srcSurface, src, dst, &color);
        }
    }
    inline void clear(const Rect &rect)
    {
        Operation op;
        if(NRDP_UNLIKELY(mInstrument))
            op.start("clear", Variant::StringMap("rect", rect.toString())
                     ("pixels", rect.width*rect.height), this);
        if(isDisplayList())
            clear_dl(rect);
        else if(mSurface)
            clear_sys(rect);
    }
    inline void fill(const Rect &rect, const Color &color)
    {
        Operation op;
        if(NRDP_UNLIKELY(mInstrument))
            op.start("fill", Variant::StringMap("rect", rect.toString())("color", color.toString())
                     ("pixels", rect.width*rect.height), this);
        if(isDisplayList())
            fill_dl(rect, color);
        else if(mSurface)
            fill_sys(rect, color);
    }
    inline void drawRect(const Rect &rect, const Color &color)
    {
        Operation op;
        if(NRDP_UNLIKELY(mInstrument))
            op.start("drawRect", Variant::StringMap("rect", rect.toString())("color", color.toString())("pixels", rect.width*rect.height), this);
        if(isDisplayList())
            drawRect_dl(rect, color);
        else if(mSurface)
            drawRect_sys(rect, color);
    }

    inline void setScale(float scale)
    {
        if(mState.scale != scale)
            setScale_internal(scale);
    }
    inline float scale() const { return mState.scale; }

    inline void setOpacity(float opacity)
    {
        if(mState.opacity != opacity)
            setOpacity_internal(opacity);
    }
    inline float opacity() const { return mState.opacity; }

    inline Rect clip() const { return mState.clip; }
    inline void setClip(const Rect &clipRect) { setClip(clipRect, false);  }
    inline void clearClip()
    {
        if(isDisplayList())
            setClip(Rect(0, 0, mDisplayList->getSize()), true);
        else if(mSurface)
            setClip(mSurface->getRect(), true);
    }

    inline void flush()
    {
        if(mSurface)
            flush_sys();
    }

    void startText() { if (!startText_sys()) startText_internal(); }
    inline void drawText(hb_buffer_t *text, int start, int end,
                         const Rect &rect, const Rect &dstRect, const Style::SharedPtr &style)
    {
        Operation op;
        if(NRDP_UNLIKELY(mInstrument))
            op.start("drawText", Variant::StringMap("pixels", rect.width*rect.height), this);
        if(isDisplayList())
            drawText_dl(text, start, end, rect, dstRect, style);
        else if (mSurface)
            drawText_sys(text, start, end, rect, dstRect, style);
    }
    void drawText(const char* text, const Rect &rect, const Rect &dstRect, const Style::SharedPtr &style);
    void endText() { if (!endText_sys()) endText_internal(); }

#ifdef GIBBON_GRAPHICSENGINE_GLES2
    static void freeSurfaces();
    bool isScreen() const { return !mFbo; }
    OpenGLFBO::SharedPtr fbo() const { return mFbo; }
    float scaleX() const { return mXScale; }
    float scaleY() const { return mYScale; }
    Rect targetRect() const { return mTargetRect; }

    enum RectMode { RectFill, RectLine };
    void drawRect(RectMode mode, const Rect &rect, const Color &color);

    Rect makeScissor(const Rect& rect);
    Transform makeTransform(const Rect& dstRect, const Rect& srcRect = Rect());
    void blit(GLuint tex, const Transform& modelView, const GLfloat texTransform[4]);

    void drawVBO(const Surface::SharedPtr& surface, const Color& color, const Transform& modelview, GLuint vbo, int size);
#endif

protected:
    static void initCommands_sys();

    bool startText_sys();
    bool endText_sys();
    void startText_internal();
    void endText_internal();

    inline void setCompositionMode_internal(CompositionMode mode)
    {
        mState.compositionMode = mode;
        if(isDisplayList())
            setCompositionMode_dl(mode);
        else if(mSurface)
            setCompositionMode_sys(mode);
    }
    inline void setOpacity_internal(float opacity)
    {
        mState.opacity = opacity;
        if(isDisplayList())
            setOpacity_dl(opacity);
        else if(mSurface)
            setOpacity_sys(opacity);
    }
    inline void setScale_internal(float scale)
    {
        mState.scale = scale;
        if(isDisplayList())
            setScale_dl(scale);
        else if(mSurface)
            setScale_sys(scale);
    }
    inline void setClip_internal(const Rect &rect, bool clear)
    {
        mState.hasClip = !clear;
        mState.clip = rect;
        if(isDisplayList())
            setClip_dl(clear ? 0 : &mState.clip);
        else if(mSurface)
            setClip_sys(clear ? 0 : &mState.clip);
    }
    inline void setSmoothScale_internal(bool smooth)
    {
        mState.smoothScale = smooth;
        if(isDisplayList())
            setSmoothScale_dl(smooth);
        else if(mSurface)
            setSmoothScale_sys(smooth);
    }
    void drawText_internal(hb_buffer_t *text, int start, int end, const Rect &rect, const Rect &dstRect, const Style::SharedPtr &style);
    void drawText_dl_internal(hb_buffer_t *text, int start, int end, const Rect &rect, const Rect &dstRect, const Style::SharedPtr &style);

    void setCompositionMode_dl(CompositionMode mode);
    void setOpacity_dl(float opacity);
    void setScale_dl(float scale);
    void setClip_dl(const Rect *rect);
    void setSmoothScale_dl(bool smooth);
    void clear_dl(const Rect &rect);
    void fill_dl(const Rect &rect, const Color &color);
    void drawRect_dl(const Rect &rect, const Color &color);
    void blit_dl(const Surface::SharedPtr &srcSurface, const Rect &srcRect, const Rect &dstRect, const Color *color);
    void batchBlit_dl(const Surface::SharedPtr &surface, const std::vector<Rect> &src, const std::vector<Rect> &dst, const Color *color);
#ifdef GIBBON_GRAPHICSENGINE_DRAWTEXT_DL
    void drawText_dl(hb_buffer_t *text, int start, int end, const Rect &rect, const Rect &dstRect, const Style::SharedPtr &style);
#else
    inline void drawText_dl(hb_buffer_t *text, int start, int end, const Rect &rect, const Rect &dstRect, const Style::SharedPtr &style)
    { drawText_dl_internal(text, start, end, rect, dstRect, style); }
#endif

    void begin_sys();
    void end_sys();
    void flush_sys();
    void setCompositionMode_sys(CompositionMode mode);
    void setOpacity_sys(float opacity);
    void setScale_sys(float scale);
    void setClip_sys(const Rect *rect);
    void setSmoothScale_sys(bool smooth);
    void clear_sys(const Rect &rect);
    void fill_sys(const Rect &rect, const Color &color);
    void drawRect_sys(const Rect &rect, const Color &color);
    void blit_sys(const Surface::SharedPtr &srcSurface, const Rect &srcRect, const Rect &dstRect, const Color *color);
    void batchBlit_sys(const Surface::SharedPtr &surface, const std::vector<Rect> &src, const std::vector<Rect> &dst, const Color *color);
    void drawText_sys(hb_buffer_t *text, int start, int end, const Rect &rect, const Rect &dstRect, const Style::SharedPtr &style);

    inline void setClip(const Rect &rect, bool clear) //in graphics engine coordinates, not scaled
    {
        if(mState.clip != rect || mState.hasClip != !clear)
            setClip_internal(rect, clear);
    }

    inline void appendOperation(const Operation &operation) {
        mOperations.push_back(operation);
    }
    inline void sendInstrumentations()
    {
        assert(mInstrument);
        if(!mOperations.empty()) {
            Variant tags;
            Variant operations;
            Stopwatch::ms last = NAN;
            for(std::list<Operation>::const_iterator it = mOperations.begin(); it != mOperations.end(); ++it) {
                Variant &count = tags[(*it).name];
                count = count.integer()+1;

                Variant tag;
                tag["op"] = (*it).name;
                tag["args"] = (*it).variant();
                tag["started"] = isnan(last) ? 0 : (*it).duration.started()-last;
                tag["duration"] = (*it).duration.elapsed();
                operations.push_back(tag);
                last = (*it).duration.started();
            }
            tags["operations"] = operations;
            INSTRUMENTATION_INTERVAL_SWITCHED_TAG("gibbon.graphics.render", tags);
            mOperations.clear();
        }
        mInstrument = false;
        INSTRUMENTATION_INTERVAL_SWITCHED_END("gibbon.graphics.render");
    }

protected:

    DisplayList::SharedPtr mDisplayList;
    Surface::SharedPtr mSurface;

    State mState;
    bool mInstrument;
    static bool sDefaultSmoothScale;
    std::list<Operation> mOperations;
    GraphicsTextData* mTextData;

#if defined(GIBBON_GRAPHICSENGINE_DIRECTFB)
    void setColor(const Color &color);
    int mBlittingFlags;
#elif defined(GIBBON_GRAPHICSENGINE_GLES2)
    void blit_internal(GLuint tex, const Transform& modelView, const GLfloat texTransform[4],
                       bool blend, float opacity, bool smooth);

    GraphicsGLTextData* mGLTextData;
    OpenGLContext* mGLContext;
    OpenGLFBO::SharedPtr mFbo;
    Rect mTargetRect;
    float mXScale, mYScale;
    friend class DrawVBO;
#endif
};

SERIALIZE_ALIAS(GraphicsEngine::CompositionMode, uint8_t)

#if !defined(GIBBON_GRAPHICSENGINE_GLES2)
inline bool GraphicsEngine::startText_sys()
{
    return false;
}

inline bool GraphicsEngine::endText_sys()
{
    return false;
}
#endif // !GIBBON_GRAPHICSENGINE_GLES2

}} // namespace netflix::gibbon

#endif /* __GRAPHICSENGINE_H__ */
