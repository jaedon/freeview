/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef OPENGLSTATE_H
#define OPENGLSTATE_H

#include <map>
#include <set>
#include <algorithm>
#include <gibbon/config.h>

#if defined(GIBBON_GRAPHICSENGINE_GLES2_EGL)
#  include <GLES2/gl2.h>
#elif defined(GIBBON_GRAPHICSENGINE_GLES2_IOS)
#  include <OpenGLES/ES2/gl.h>
#elif defined(GIBBON_GRAPHICSENGINE_GLES2_GLX)
#ifndef GL_GLEXT_PROTOTYPES
#    define GL_GLEXT_PROTOTYPES 1
#  endif
#  include <GL/gl.h>
#endif

namespace netflix {
namespace gibbon {

class OpenGLState
{
public:
    OpenGLState();
    ~OpenGLState();

    void useProgram(GLuint program);
    void bindBuffer(GLenum target, GLuint buffer);
    void deleteBuffer(GLenum target, GLuint buffer);
    void bindTexture(GLuint texture, bool force = false);
    void unbindTexture(GLuint texture);
    void updateTexture(GLuint texture);
    void blendFunc(GLenum sfactor, GLenum dfactor);
    void enableCap(GLenum cap, bool enable = true);
    void disableCap(GLenum cap);
    void viewport(GLint x, GLint y, GLsizei width, GLsizei height);
    void scissor(GLfloat x, GLfloat y, GLfloat width, GLfloat height);

    // per texture
    void setSmoothScale(bool smooth);

    class Scope
    {
    public:
        Scope(OpenGLState* state) : s(state) { s->startScope(this); }
        ~Scope() { s->endScope(); }

        void enableCap(GLenum cap, bool enable = true);
        void disableCap(GLenum cap);

        void enableVAA(GLuint index);
        void disableVAA(GLuint index);

        void flush() { s->flushScope(); }

    private:
        OpenGLState* s;

        struct State
        {
            std::set<GLenum> caps;
            std::set<GLuint> vaas;
        };
        State state;

        friend class OpenGLState;
    };

    struct GLRect
    {
        GLRect() : x(0), y(0), width(-1), height(-1) { }

        GLint x, y;
        GLsizei width, height;
    };

    GLRect clipRect() const { return clip; }

private:
    void startScope(Scope* scope);
    void flushScope();
    void endScope();

private:
    Scope* scope;
    Scope::State state;
    GLuint program, texture;
    GLenum blendS, blendD;
    std::map<GLenum, GLuint> buffers;

    struct TexParams
    {
        TexParams() : smooth(false) { }

        bool smooth;
    };
    std::map<GLuint, TexParams> texParams;

    GLRect view, clip;

    friend class Scope;
};

inline OpenGLState::OpenGLState()
    : scope(0), program(0), texture(0), blendS(GL_INVALID_ENUM), blendD(GL_INVALID_ENUM)
{
}

inline OpenGLState::~OpenGLState()
{
}

inline void OpenGLState::Scope::enableCap(GLenum cap, bool enable)
{
    if (enable)
        state.caps.insert(cap);
    else
        state.caps.erase(cap);
}

inline void OpenGLState::Scope::disableCap(GLenum cap)
{
    state.caps.erase(cap);
}

inline void OpenGLState::Scope::enableVAA(GLuint index)
{
    state.vaas.insert(index);
}

inline void OpenGLState::Scope::disableVAA(GLuint index)
{
    state.vaas.erase(index);
}

inline void OpenGLState::viewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    if (view.x == x && view.y == y && view.width == width && view.height == height)
        return;
    view.x = x;
    view.y = y;
    view.width = width;
    view.height = height;
    glViewport(x, y, width, height);
}

inline void OpenGLState::scissor(GLfloat x, GLfloat y, GLfloat width, GLfloat height)
{
    const float xx = floorf(x);
    const float yy = floorf(y);
    const float ww = ceilf(width + (x - xx));
    const float hh = ceilf(height + (y - yy));

    if (clip.x == xx && clip.y == yy && clip.width == ww && clip.height == hh)
        return;
    clip.x = xx;
    clip.y = yy;
    clip.width = ww;
    clip.height = hh;
    glScissor(clip.x, clip.y, clip.width, clip.height);
}

inline void OpenGLState::useProgram(GLuint prog)
{
    if (prog == program)
        return;
    program = prog;
    glUseProgram(program);
}

inline void OpenGLState::bindTexture(GLuint tex, bool force)
{
    if (!force && tex == texture)
        return;
    texture = tex;
    glBindTexture(GL_TEXTURE_2D, texture);
}

inline void OpenGLState::unbindTexture(GLuint tex)
{
    if (tex != texture)
        return;
    texture = 0;
    glBindTexture(GL_TEXTURE_2D, 0);
}

inline void OpenGLState::setSmoothScale(bool smooth)
{
    TexParams& params = texParams[texture];
    if (params.smooth != smooth) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (smooth ? GL_LINEAR : GL_NEAREST));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (smooth ? GL_LINEAR : GL_NEAREST));
        params.smooth = smooth;
    }
}

inline void OpenGLState::updateTexture(GLuint tex)
{
    texture = tex;
}

inline void OpenGLState::blendFunc(GLenum sfactor, GLenum dfactor)
{
    if (blendS == sfactor && blendD == dfactor)
        return;
    blendS = sfactor;
    blendD = dfactor;
    glBlendFunc(sfactor, dfactor);
}

inline void OpenGLState::bindBuffer(GLenum target, GLuint buffer)
{
    std::map<GLenum, GLuint>::iterator entry = buffers.find(target);
    if (entry != buffers.end()) {
        if (entry->second == buffer)
            return;
        entry->second = buffer;
    } else {
        buffers[target] = buffer;
    }
    glBindBuffer(target, buffer);
}

inline void OpenGLState::deleteBuffer(GLenum target, GLuint buffer)
{
    glDeleteBuffers(1, &buffer);
    buffers[target] = 0;
}

inline void OpenGLState::startScope(Scope* s)
{
    scope = s;
}

inline void OpenGLState::enableCap(GLenum cap, bool enable)
{
    if (enable && state.caps.insert(cap).second)
        glEnable(cap);
    else if (!enable && state.caps.erase(cap))
        glDisable(cap);
}

inline void OpenGLState::disableCap(GLenum cap)
{
    if (state.caps.erase(cap))
        glDisable(cap);
}

template<typename T>
inline void flushEntries(const std::set<T>& s1, const std::set<T>& s2, void (*func)(T))
{
    std::set<T> result;
    std::set_difference(s1.begin(), s1.end(), s2.begin(), s2.end(), std::inserter(result, result.end()));
    typename std::set<T>::const_iterator r = result.begin();
    const typename std::set<T>::const_iterator e = result.end();
    while (r != e) {
        func(*r);
        ++r;
    }
}

inline void OpenGLState::flushScope()
{
    flushEntries<GLenum>(state.caps, scope->state.caps, glDisable);
    flushEntries<GLenum>(scope->state.caps, state.caps, glEnable);
    flushEntries<GLuint>(state.vaas, scope->state.vaas, glDisableVertexAttribArray);
    flushEntries<GLuint>(scope->state.vaas, state.vaas, glEnableVertexAttribArray);

    state = scope->state;
}

inline void OpenGLState::endScope()
{
    flushScope();
    scope = 0;
}

}
}

#endif
