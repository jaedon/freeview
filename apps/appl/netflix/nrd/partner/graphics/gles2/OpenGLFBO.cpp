/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "OpenGLFBO.h"
#include "OpenGLContext.h"
#include "OpenGLState.h"
#include <nrd/AppLog.h>

using namespace netflix;
using namespace netflix::gibbon;

OpenGLFBO::OpenGLFBO(const Surface::SharedPtr& surface)
    : mValid(true)
{
    const GLuint tex = surface->getTexture();
    mRect = surface->getRect();
    init(tex);
}

OpenGLFBO::~OpenGLFBO()
{
    if (!mValid)
        return;

    glDeleteFramebuffers(1, &mFbo);
    glDeleteRenderbuffers(mRb.size(), &mRb[0]);
}

GLenum OpenGLFBO::generate(GLuint tex, Coordinate width, Coordinate height, unsigned int flags)
{
    OpenGLContext::context()->state()->unbindTexture(tex);
    glGenFramebuffers(1, &mFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, mFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

#ifdef GL_DEPTH_STENCIL
    if (flags & DepthStencil) {
        mRb.resize(1);
        glGenRenderbuffers(mRb.size(), &mRb[0]);
        glBindRenderbuffer(GL_RENDERBUFFER, mRb[0]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_STENCIL, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRb[0]);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRb[0]);
    } else
#endif
    {
        if (flags & Depth) {
            assert(mRb.empty());
            mRb.resize(1);
            GLuint& rb = mRb.back();
            glGenRenderbuffers(1, &rb);
            glBindRenderbuffer(GL_RENDERBUFFER, rb);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rb);
        }
        if (flags & Stencil) {
            mRb.resize(mRb.size() + 1);
            GLuint& rb = mRb.back();
            glGenRenderbuffers(1, &rb);
            glBindRenderbuffer(GL_RENDERBUFFER, rb);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, width, height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rb);
        }
    }

    const GLenum fbstatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fbstatus != GL_FRAMEBUFFER_COMPLETE) {
        // flush any error if we got one
        glGetError();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &mFbo);
        glDeleteRenderbuffers(mRb.size(), &mRb[0]);
        mFbo = 0;
        mRb.clear();
    }
    return fbstatus;
}

void OpenGLFBO::init(GLuint tex)
{
    const Coordinate width = mRect.width;
    const Coordinate height = mRect.height;

    static unsigned int tryFlags[] = {
#ifdef GL_DEPTH_STENCIL
        DepthStencil,
#endif
        Depth | Stencil,
        Depth,
        Stencil,
        0
    };

    GL_CHECK;
    GLenum status;
    for (size_t idx = 0; idx < sizeof(tryFlags) / sizeof(tryFlags[0]); ++idx) {
        status = generate(tex, width, height, tryFlags[idx]);
        if (status == GL_FRAMEBUFFER_COMPLETE)
            break;
    }
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        mValid = false;
        std::string message;
        switch (status) {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            message = "Incomplete attachment";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            message = "Missing attachment";
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            message = "Unsupported framebuffer";
            break;
        default:
            message = StringFormatter::sformat("Unknown status: 0x%x", status);
            break;
        }
        Log::error(TRACE_GIBBON_GRAPHICS, "Couldn't create FBO: %s", message.c_str());
        // } else {
        //     Log::error(TRACE_GIBBON_GRAPHICS, "FBO OK!");
    }

    GL_CHECK;
}

