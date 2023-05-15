/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "OpenGLContext.h"
#include "OpenGLFBO.h"
#include "OpenGLState.h"
#include "OpenGLShaders.h"
#include "SurfaceCache.h"
#include <Animation.h>
#include <Debug.h>
#include <GibbonConfiguration.h>

#include <nrd/AppLog.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Application.h>
#include <nrdbase/ConditionVariable.h>
#include <nrdbase/ObjectCount.h>

#include <assert.h>

using namespace netflix;
using namespace netflix::gibbon;

NRDP_DEFINE_OBJECTCOUNT(OpenGLContext);

shared_ptr<OpenGLContext> OpenGLContext::sPrimary;
std::set<shared_ptr<OpenGLContext> > OpenGLContext::sContexts;
Mutex OpenGLContext::sMutex(UNTRACKED_MUTEX, "OpenGLContext mutex");
Coordinate OpenGLContext::sWidth;
Coordinate OpenGLContext::sHeight;
std::vector<GLuint> OpenGLContext::sTexToDelete;
bool OpenGLContext::sShutdown = false;
ThreadLocalStore<ContextData> OpenGLContext::sContextDatas;

namespace netflix {
namespace gibbon {
class ContextData
{
public:
    ContextData() { }
    ~ContextData()
    {
        ScopedMutex locker(OpenGLContext::sMutex);
        shared_ptr<OpenGLContext> ctx = context.lock();
        if (ctx) {
            OpenGLContext::sContexts.erase(ctx);
        }
    }

    weak_ptr<OpenGLContext> context;
};
}} // netflix::gibbon

OpenGLContext::OpenGLContext(OpenGLContext* share)
    : mPrivate(0), mState(new OpenGLState), mShared(share != 0)
{
    {
        ScopedMutex locker(sMutex);
        mWidth = sWidth;
        mHeight = sHeight;

        assert(!sShutdown);
    }

    init(share);

    glDisable(GL_BLEND);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    mState->viewport(0, 0, mWidth, mHeight);
    glClearColor(0, 0, 0, 0);
    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_DEPTH_TEST);
    GL_CHECK;

    if (!mShared) {
        GibbonApplication::instance()->getSurfaceCache()->setAtlasPad(1, 1);

        {
            const GLchar* vertex[] = {
                "uniform mat4   u_projectionMatrix;\n"
                "uniform mat4   u_mvMatrix;\n"
                "attribute vec4 a_position;\n"
                "void main()\n"
                "{\n"
                "  gl_Position = (u_projectionMatrix * u_mvMatrix) * a_position;\n"
                "}\n"
            };

            const GLchar* fragment[] = {
                "#ifdef GL_ES\n"
                "precision highp float;\n"
                "#endif\n"
                "uniform vec4 u_color;\n"
                "void main()\n"
                "{\n"
                "  gl_FragColor = u_color;\n"
                "}\n"
            };

            mFillShader.reset(new FillShader(this, vertex, fragment));
            mFillShader->use();
            mFillShader->defineUniform(FillShader::ProjMatrix, "u_projectionMatrix");
            mFillShader->defineUniform(FillShader::MVMatrix, "u_mvMatrix");
            mFillShader->defineAttribute(FillShader::Position, "a_position");
            mFillShader->defineUniform(FillShader::Color, "u_color");

            mLineShader.reset(new LineShader(this, vertex, fragment));
            mLineShader->use();
            mLineShader->defineUniform(LineShader::ProjMatrix, "u_projectionMatrix");
            mLineShader->defineUniform(LineShader::MVMatrix, "u_mvMatrix");
            mLineShader->defineAttribute(LineShader::Position, "a_position");
            mLineShader->defineUniform(LineShader::Color, "u_color");
        }

        {
            const GLchar* vertex[] = {
                "uniform mat4   u_projectionMatrix;\n"
                "uniform mat4   u_mvMatrix;\n"
                "uniform vec4   u_ts;\n"
                "attribute vec4 a_position;\n"
                "attribute vec2 a_texCoord;\n"
                "varying vec2   v_texCoord;\n"
                "void main()\n"
                "{\n"
                "  gl_Position = (u_projectionMatrix * u_mvMatrix) * a_position;\n"
                "  v_texCoord.x = (a_texCoord.x * u_ts.z) + u_ts.x;\n"
                "  v_texCoord.y = (a_texCoord.y * u_ts.w) + u_ts.y;\n"
                "}\n"
            };

            const GLchar* fragment[] = {
                "#ifdef GL_ES\n"
                "precision highp float;\n"
                "#endif\n"
                "varying vec2      v_texCoord;\n"
                "uniform float     u_opacity;\n"
                "uniform sampler2D s_texture;\n"
                "void main()\n"
                "{\n"
                "  gl_FragColor = texture2D(s_texture, v_texCoord) * u_opacity;\n"
                "}\n"
            };

            mBlitShader.reset(new BlitShader(this, vertex, fragment));
            mBlitShader->use();
            mBlitShader->defineUniform(BlitShader::ProjMatrix, "u_projectionMatrix");
            mBlitShader->defineUniform(BlitShader::MVMatrix, "u_mvMatrix");
            mBlitShader->defineUniform(BlitShader::TextureTransform, "u_ts");
            mBlitShader->defineAttribute(BlitShader::Position, "a_position");
            mBlitShader->defineAttribute(BlitShader::TexCoord, "a_texCoord");
            mBlitShader->defineUniform(BlitShader::Opacity, "u_opacity");

            glUniform1i(glGetUniformLocation(mBlitShader->program(), "s_texture"), 0);
        }
        {
            const GLchar* vertex[] = {
                "uniform mat4 projection;\n"
                "uniform mat4 modelview;\n"

                "varying vec2 textureCoordinate;\n"

                "attribute vec4 vertex;\n"
                "attribute vec2 texCoord;\n"

                "void main()\n"
                "{\n"
                "  textureCoordinate = texCoord;\n"
                "  gl_Position       = (modelview * projection) * vertex;\n"
                "}\n"
            };

            const GLchar* fragment[] = {
                "#ifdef GL_ES\n"
                "precision highp float;\n"
                "#endif\n"

                "uniform sampler2D texture;\n"
                "uniform vec4 color;\n"

                "varying vec2 textureCoordinate;\n"

                "void main()\n"
                "{\n"
                "  float a = texture2D(texture, textureCoordinate).a;\n"
                "  gl_FragColor = color.rgba * a;\n"
                "}\n"
            };

            mTextShader.reset(new TextShader(this, vertex, fragment));
            mTextShader->use();
            mTextShader->defineUniform(TextShader::ProjMatrix, "projection");
            mTextShader->defineUniform(TextShader::MVMatrix, "modelview");
            mTextShader->defineAttribute(TextShader::Vertex, "vertex");
            mTextShader->defineAttribute(TextShader::TexCoord, "texCoord");
            mTextShader->defineUniform(TextShader::Color, "color");

            glUniform1i(glGetUniformLocation(mTextShader->program(), "texture"), 0);
        }
    }

    NRDP_OBJECTCOUNT_REF(OpenGLContext, this);
}

OpenGLContext::~OpenGLContext()
{
    mFillShader.reset();
    cleanup();
    delete mState;
    ScopedMutex locker(sMutex);
    if (sPrimary.get() == this)
        sPrimary.reset();
    NRDP_OBJECTCOUNT_DEREF(OpenGLContext, this);
}

void OpenGLContext::shutdown()
{
    ScopedMutex locker(sMutex);
    sShutdown = true;
    sContexts.clear();
    sPrimary.reset();
}

void OpenGLContext::setScreenSize(Coordinate width, Coordinate height)
{
    ScopedMutex locker(sMutex);
    sWidth = width;
    sHeight = height;
    sShutdown = false;
}

OpenGLContext* OpenGLContext::primary()
{
    ScopedMutex locker(sMutex);
    return sPrimary.get();
}

OpenGLContext* OpenGLContext::context()
{
    ContextData* data = sContextDatas.get();
    if (!data) {
        data = new ContextData;
        sContextDatas.set(data);
    }
    ScopedMutex locker(sMutex);
    shared_ptr<OpenGLContext> ctx = data->context.lock();
    if (!ctx) {
        if (!sPrimary) {
            ctx.reset(new OpenGLContext);
            data->context = ctx;
            sPrimary = ctx;
            sContexts.insert(ctx);
        } else {
            ctx.reset(new OpenGLContext(sPrimary.get()));
            data->context = ctx;
            sContexts.insert(ctx);
        }
    }
    assert(ctx);
    return ctx.get();
}

bool OpenGLContext::hasContext()
{
    return (sContextDatas.get() != 0);
}

GLuint OpenGLContext::createTexture(GLenum format, Coordinate width, Coordinate height)
{
    GL_CHECK;
    GLuint texture = 0;
    {
        static Mutex textureMutex(ZERO_MUTEX, "GL texture mutex");
        ScopedMutex locker(textureMutex);
        glGenTextures(1, &texture);
        mState->bindTexture(texture);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, 0);
    GL_CHECK;
    return texture;
}

bool OpenGLContext::readPixels(const Surface::SharedPtr& surface, unsigned char* data)
{
    OpenGLFBO::SharedPtr oldFbo;
    {
        if (!mEngines.empty())
            oldFbo = mEngines.back()->fbo();
    }
    OpenGLFBO fbo(surface);
    if (!fbo.isValid())
        return false;
    glReadPixels(0, 0, surface->getWidth(), surface->getHeight(), (surface->bytesPerPixel() == 1 ? GL_ALPHA : GL_RGBA), GL_UNSIGNED_BYTE, data);
    const GLuint fboid = oldFbo ? oldFbo->fbo() : 0;
    glBindFramebuffer(GL_FRAMEBUFFER, fboid);
    GL_CHECK;
    return true;
}

GLuint OpenGLContext::createVBO(GLfloat* floats, int per, int size)
{
    GL_CHECK;
    GLuint vbo;
    glGenBuffers(1, &vbo);

    mState->bindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, ((per * 6) * sizeof(GLfloat)) * size, floats, GL_STATIC_DRAW);

    GL_CHECK;
    return vbo;
}

void OpenGLContext::readVBO(GLuint vbo, GLfloat* floats, int per, int size, int* bytes)
{
    GL_CHECK;
#ifdef GIBBON_GRAPHICSENGINE_GLES2_EGL
    if(bytes)
        *bytes = 0;
#else
    mState->bindBuffer(GL_ARRAY_BUFFER, vbo);
    glGetBufferSubData(GL_ARRAY_BUFFER, 0, ((per * 6) * sizeof(GLfloat)) * size, floats);
    if (bytes)
        *bytes = ((per * 6) * sizeof(GLfloat)) * size;
#endif
    GL_CHECK;
}

void OpenGLContext::addTextureToDelete(GLuint tex)
{
    ScopedMutex locker(sMutex);
    sTexToDelete.push_back(tex);
}

void OpenGLContext::deleteTextures()
{
    std::vector<GLuint> del;
    {
        ScopedMutex locker(sMutex);
        std::swap(del, sTexToDelete);
    }
    if (del.empty())
        return;
    GL_CHECK;
    OpenGLContext* ctx = OpenGLContext::context();
    for (size_t i = 0; i < del.size(); ++i) {
        ctx->state()->unbindTexture(del[i]);
    }
    glDeleteTextures(del.size(), &del[0]);
    GL_CHECK;
}

static pthread_once_t sGLExtensionOnce = PTHREAD_ONCE_INIT;
static std::set<std::string> sGLExtensions;

static void initGLExtensions()
{
    const std::string all = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
    size_t prev = 0;
    for (size_t pos = all.find(' '); pos != std::string::npos; pos = all.find(' ', pos + 1)) {
        sGLExtensions.insert(all.substr(prev, pos - prev));
        prev = pos + 1;
    }
    if (prev < all.size()) {
        sGLExtensions.insert(all.substr(prev));
    }
}

std::vector<std::string> OpenGLContext::extensions()
{
    pthread_once(&sGLExtensionOnce, initGLExtensions);
    std::vector<std::string> result;
    for(std::set<std::string>::const_iterator it = sGLExtensions.begin(); it != sGLExtensions.end(); ++it)
        result.push_back((*it));
    return result;
}

bool OpenGLContext::supportsExtension(const std::string& ext)
{
    pthread_once(&sGLExtensionOnce, initGLExtensions);
    return (std::find(sGLExtensions.begin(), sGLExtensions.end(), ext) != sGLExtensions.end());
}

static pthread_once_t sSyncOnce = PTHREAD_ONCE_INIT;
OpenGLContext::Sync::Type OpenGLContext::Sync::sType = OpenGLContext::Sync::GL_finish;
OpenGLContext::Sync::Callbacks OpenGLContext::Sync::sCallbacks;

OpenGLContext::Sync::Sync()
{
    pthread_once(&sSyncOnce, init);
    switch (sType) {
    case GL_finish:
        glFinish();
        break;
    case ARB_sync:
        mData.mGLSync = sCallbacks.glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        glFlush();
        break;
    case KHR_fence_sync:
#ifdef GIBBON_GRAPHICSENGINE_GLES2_EGL
        mData.mEglKhr = sCallbacks.eglCreate(static_cast<EGLDisplay>(OpenGLContext::context()->display()),
                                             EGL_SYNC_FENCE_KHR, 0);
        glFlush();
#endif
        break;
    }
}

OpenGLContext::Sync::~Sync()
{
    switch (sType) {
    case GL_finish:
        break;
    case ARB_sync:
        sCallbacks.glDeleteSync(mData.mGLSync);
        break;
    case KHR_fence_sync:
#ifdef GIBBON_GRAPHICSENGINE_GLES2_EGL
        sCallbacks.eglDestroy(static_cast<EGLDisplay>(OpenGLContext::context()->display()), mData.mEglKhr);
#endif
        break;
    }
}

void OpenGLContext::Sync::wait()
{
    switch (sType) {
    case GL_finish:
        break;
    case ARB_sync: {
        GLenum ret;
        do {
            // 500 ms
            ret = sCallbacks.glClientWaitSync(mData.mGLSync, GL_SYNC_FLUSH_COMMANDS_BIT, 500000000);
        } while (ret == GL_TIMEOUT_EXPIRED);
        break; }
    case KHR_fence_sync: {
#ifdef GIBBON_GRAPHICSENGINE_GLES2_EGL
        EGLint ret;
        do {
            ret = sCallbacks.eglClientWait(static_cast<EGLDisplay>(OpenGLContext::context()->display()),
                                           mData.mEglKhr, EGL_SYNC_FLUSH_COMMANDS_BIT_KHR,
                                           500000000);
        } while (ret == EGL_TIMEOUT_EXPIRED_KHR);
#endif
        break; }
    }
}

void OpenGLContext::Sync::init()
{
    if (!GibbonConfiguration::uploadInDecodeThreads())
        return;
#ifdef GIBBON_GRAPHICSENGINE_GLES2_EGL
    if (OpenGLContext::supportsPlatformExtension("EGL_KHR_fence_sync")
        && OpenGLContext::supportsExtension("GL_OES_EGL_sync")) {
        sType = KHR_fence_sync;
        sCallbacks.eglCreate = (PFNEGLCREATESYNCKHRPROC)OpenGLContext::procAddr("eglCreateSyncKHR");
        sCallbacks.eglDestroy = (PFNEGLDESTROYSYNCKHRPROC)OpenGLContext::procAddr("eglDestroySyncKHR");
        sCallbacks.eglClientWait = (PFNEGLCLIENTWAITSYNCKHRPROC)OpenGLContext::procAddr("eglClientWaitSyncKHR");
    } else
#endif
    if (OpenGLContext::supportsExtension("GL_ARB_sync")) {
        sType = ARB_sync;
        sCallbacks.glFenceSync = (PFNGLFENCESYNCPROC)OpenGLContext::procAddr("glFenceSync");
        sCallbacks.glIsSync = (PFNGLISSYNCPROC)OpenGLContext::procAddr("glIsSync");
        sCallbacks.glDeleteSync = (PFNGLDELETESYNCPROC)OpenGLContext::procAddr("glDeleteSync");
        sCallbacks.glClientWaitSync = (PFNGLCLIENTWAITSYNCPROC)OpenGLContext::procAddr("glClientWaitSync");
        sCallbacks.glWaitSync = (PFNGLWAITSYNCPROC)OpenGLContext::procAddr("glWaitSync");
        sCallbacks.glGetSync = (PFNGLGETSYNCIVPROC)OpenGLContext::procAddr("glGetSync");
    }
}

void OpenGLContext::loadEngineState(GraphicsEngine* engine)
{
    const GraphicsEngine::State& state = engine->getState();
    const Rect clip = engine->makeScissor(state.clip);
    mState->scissor(clip.x, clip.y, clip.width, clip.height);
    const OpenGLFBO::SharedPtr& fbo = engine->fbo();
    const GLuint fboid = fbo ? fbo->fbo() : 0;
    glBindFramebuffer(GL_FRAMEBUFFER, fboid);
    const Rect& viewport = engine->targetRect();
    mState->viewport(viewport.x, viewport.y, viewport.width, viewport.height);
}

void OpenGLContext::pad(const Surface::SharedPtr& surface, GLint x, GLint y, GLint width, GLint height)
{
    OpenGLFBO::SharedPtr oldFbo;
    {
        if (!mEngines.empty())
            oldFbo = mEngines.back()->fbo();
    }
    {
        OpenGLFBO::SharedPtr fbo(new OpenGLFBO(surface));
        if (!fbo->isValid()) {
            Log::error(TRACE_GIBBON_GRAPHICS, "Unable to create fbo for clear: %s", surface->describe().c_str());
            return;
        }
        const OpenGLState::GLRect oldClip = mState->clipRect();
        mState->scissor(x, y, width, height);
        mState->enableCap(GL_SCISSOR_TEST);
        glClear(GL_COLOR_BUFFER_BIT);
        if (oldClip.width != -1 && oldClip.height != -1)
            mState->scissor(oldClip.x, oldClip.y, oldClip.width, oldClip.height);
    }
    const GLuint fboid = oldFbo ? oldFbo->fbo() : 0;
    glBindFramebuffer(GL_FRAMEBUFFER, fboid);
}

std::string OpenGLContext::version() const
{
    const char* ver = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    if (ver)
        return std::string(ver);
    return std::string();
}

#if !defined(__APPLE__)
#if !defined(GIBBON_NO_THREADANIMATION)
void *createEGLVideoImage(unsigned width, unsigned height);
class CreateVideoImage : public Application::Event
{
public:
    CreateVideoImage(unsigned w, unsigned h)
        : width(w), height(h), mutex(ZERO_MUTEX, "CreateVideoImage::Mutex"), image(0), finished(false)
    {
    }

    virtual void eventFired()
    {
        ScopedMutex lock(mutex);
        image = createEGLVideoImage(width, height);
        finished = true;
        cond.signal();
    }

    void *wait()
    {
        ScopedMutex lock(mutex);
        while (!finished)
            cond.wait(&mutex);
        return image;
    }

private:
    unsigned width, height;
    Mutex mutex;
    void *image;
    bool finished;
    ConditionVariable cond;
};
#endif

void *createEGLVideoImage(unsigned width, unsigned height)
{
#if !defined(GIBBON_NO_THREADANIMATION)
    if (!Animation::isAnimationThread()) {
        shared_ptr<CreateVideoImage> event(new CreateVideoImage(width, height));
        if (Animation::postEvent(event))
            return event->wait();
    }
#endif
    if (void* image = OpenGLContext::context()->createEGLVideoImage(width, height)) {
        Debug::setProperty("testAnimationNoIdle", true);
        return image;
    }
    return 0;
}
#endif // __APPLE__

