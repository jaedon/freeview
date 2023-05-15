/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef OPENGLFBO_H
#define OPENGLFBO_H

#include <vector>
#include <Surface.h>
#include <nrdbase/tr1.h>

namespace netflix {
namespace gibbon {

class OpenGLFBO : public enable_shared_from_this<OpenGLFBO>
{
public:
    typedef shared_ptr<OpenGLFBO> SharedPtr;
    typedef weak_ptr<OpenGLFBO> WeakPtr;

    OpenGLFBO(const Surface::SharedPtr& surface);
    ~OpenGLFBO();

    bool isValid() const { return mValid; }
    GLuint fbo() const { return mFbo; }

private:
    void init(GLuint tex);

    enum GenerateFlags {
#ifdef GL_DEPTH_STENCIL
        DepthStencil = 0x1,
#endif
        Depth = 0x2,
        Stencil = 0x4
    };
    GLenum generate(GLuint tex, Coordinate width, Coordinate height, unsigned int flags);

private:
    friend class OpenGLContext;

    GLuint mFbo;
    std::vector<GLuint> mRb;
    Rect mRect;
    bool mValid;
};

}} // namespace netflix::gibbon

#endif
