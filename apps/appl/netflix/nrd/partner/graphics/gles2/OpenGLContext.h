/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef OPENGLCONTEXT_H
#define OPENGLCONTEXT_H

#include "GraphicsEngine.h"
#include "OpenGLFloats.h"
#include <Surface.h>
#include <Debug.h>
#include <Color.h>
#include <Rect.h>
#include <Transform.h>
#include <nrd/AppLog.h>
#include <nrdbase/tr1.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/LinkedList.h>
#include <nrdbase/ThreadLocalStore.h>
#include <vector>

#include "OpenGLExtensions.h"

namespace netflix {
inline void _glcheck(const char* function, const char* file, int line)
{
    if(!netflix::gibbon::Debug::DebugGLCheck)
        return;
    const GLenum e = glGetError();
    if (e != GL_NO_ERROR) {
        netflix::Log::fatal(TRACE_GIBBON_GRAPHICS)
            << ">>> GL ERROR " << std::hex << e
            << " AT " << function << "\n" << file << ":" << std::dec << line;
        abort();
    }
}
#define GL_CHECK _glcheck(__PRETTY_FUNCTION__, __FILE__, __LINE__)

namespace gibbon {

class OpenGLContextPrivate;
class FillShader;
class LineShader;
class BlitShader;
class TextShader;
class OpenGLState;
class ContextData;

struct Floats
{
public:
    Floats()
        : f(0), idx(0), size(0)
    {
        reserve(5000);
    }
    ~Floats()
    {
        free(f);
    }

    void reserve(int sz)
    {
        if (size >= sz)
            return;
        f = static_cast<GLfloat*>(realloc(f, sz * sizeof(GLfloat)));
        idx = f;
        size = sz;
    }

    GLfloat* take(int size)
    {
        GLfloat* cur = idx;
        idx += size;
        return cur;
    }

    void reset()
    {
        idx = f;
    }

private:
    GLfloat* f;
    GLfloat* idx;
    int size;
};

struct GLVertex
{
    float x, y, z;    // position
    float s, t;       // texture
    float r, g, b, a; // color
};

class OpenGLContext
{
public:
    ~OpenGLContext();

    static void setScreenSize(Coordinate width, Coordinate height);
    static bool hasContext();
    static OpenGLContext* context();
    static OpenGLContext* primary();
    static void ensureContext() { (void)context(); }
    static void shutdown();

    std::string version() const;

    bool renderVideo();

    void pushEngine(GraphicsEngine* engine);
    void popEngine();
    GraphicsEngine* engine();

    void flip();

    OpenGLState* state() { return mState; }
    const OpenGLState* state() const { return mState; }

    Coordinate screenWidth() const { return mWidth; }
    Coordinate screenHeight() const { return mHeight; }

    shared_ptr<FillShader> fillShader() const { return mFillShader; }
    shared_ptr<LineShader> lineShader() const { return mLineShader; }
    shared_ptr<BlitShader> blitShader() const { return mBlitShader; }
    shared_ptr<TextShader> textShader() const { return mTextShader; }

    GLuint createTexture(GLenum format, Coordinate width, Coordinate height);
    bool readPixels(const Surface::SharedPtr& surface, unsigned char* data);

    GLuint createVBO(GLfloat* floats, int per, int size);
    void readVBO(GLuint vbo, GLfloat* floats, int per, int size, int* bytes);

    void pad(const Surface::SharedPtr& surface, GLint x, GLint y, GLint width, GLint height);

    OpenGLFloats& floats() { return mFloats; }

    class Sync
    {
    public:
        typedef shared_ptr<Sync> SharedPtr;
        typedef weak_ptr<Sync> WeakPtr;

        Sync();
        ~Sync();

        void wait();

    private:
        enum Type {
            ARB_sync,
            KHR_fence_sync,
            GL_finish
        };
        static Type sType;

        struct Callbacks {
#ifdef GIBBON_GRAPHICSENGINE_GLES2_EGL
            PFNEGLCREATESYNCKHRPROC eglCreate;
            PFNEGLDESTROYSYNCKHRPROC eglDestroy;
            PFNEGLCLIENTWAITSYNCKHRPROC eglClientWait;
#endif
            PFNGLFENCESYNCPROC glFenceSync;
            PFNGLISSYNCPROC glIsSync;
            PFNGLDELETESYNCPROC glDeleteSync;
            PFNGLCLIENTWAITSYNCPROC glClientWaitSync;
            PFNGLWAITSYNCPROC glWaitSync;
            PFNGLGETSYNCIVPROC glGetSync;
        };
        static Callbacks sCallbacks;

        static void init();

        struct {
            union {
                void* mEglKhr;
                GLsync mGLSync;
            };
        } mData;

        friend class OpenGLContext;
    };

    void* display();
    void *createEGLVideoImage(unsigned width, unsigned height);

    static void addTextureToDelete(GLuint tex);
    static void deleteTextures();

    inline GLint getIntegerv(GLenum e) const {
        GLint result;
        glGetIntegerv(e, &result);
        if(glGetError() != GL_NO_ERROR)
            result = 0;
        return result;
    }

    static std::vector<std::string> extensions();
    static bool supportsExtension(const std::string& ext);
    static void* procAddr(const std::string& proc);
    static bool supportsPlatformExtension(const std::string& ext);

private:
    bool init(OpenGLContext* share);
    void cleanup();
    void loadEngineState(GraphicsEngine* engine);

    const GraphicsEngine::State& engineState() const { assert(!mEngines.empty()); return mEngines.back()->getState(); }

private:
    OpenGLContext(OpenGLContext* share = 0);

    std::list<GraphicsEngine*> mEngines;

    OpenGLContextPrivate* mPrivate;
    shared_ptr<FillShader> mFillShader;
    shared_ptr<LineShader> mLineShader;
    shared_ptr<BlitShader> mBlitShader;
    shared_ptr<TextShader> mTextShader;
    OpenGLState* mState;
    Coordinate mWidth, mHeight;
    OpenGLFloats mFloats;
    bool mShared;

    static Mutex sMutex;
    static std::set<shared_ptr<OpenGLContext> > sContexts;
    static shared_ptr<OpenGLContext> sPrimary;
    static Coordinate sWidth, sHeight;
    static std::vector<GLuint> sTexToDelete;
    static bool sShutdown;
    static ThreadLocalStore<ContextData> sContextDatas;

    friend class ContextData;
};

inline void OpenGLContext::pushEngine(GraphicsEngine* engine)
{
    mEngines.push_back(engine);
    loadEngineState(engine);
}

inline void OpenGLContext::popEngine()
{
    assert(!mEngines.empty());
    mEngines.pop_back();
    if (!mEngines.empty())
        loadEngineState(mEngines.back());
}

inline GraphicsEngine* OpenGLContext::engine()
{
    assert(!mEngines.empty());
    return mEngines.back();
}

}} // netflix::gibbon

#endif
