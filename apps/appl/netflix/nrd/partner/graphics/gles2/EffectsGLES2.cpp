/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "EffectsGLES2.h"
#include "Animation.h"
#include "Screen.h"
#include "SurfaceCache.h"

using namespace netflix;
using namespace netflix::gibbon;

class ProgramDeleteEvent : public Application::Event
{
public:
    ProgramDeleteEvent(GLuint program) : mProgram(program) { }

    void eventFired()
    {
        glDeleteProgram(mProgram);
    }
private:
    GLuint mProgram;
};

static inline void effectDeleteProgram(GLuint program)
{
#if !defined(GIBBON_NO_THREADANIMATION)
    if (!Animation::isAnimationThread()) {
        shared_ptr<ProgramDeleteEvent> event(new ProgramDeleteEvent(program));
        Animation::postEvent(event);
        return;
    }
#else
    if (!Application::isAppThread()) {
        shared_ptr<ProgramDeleteEvent> event(new ProgramDeleteEvent(program));
        app()->postEvent(event);
        return;
    }
#endif
    glDeleteProgram(program);
}

EffectBlurGLES2::GLES2Renderer::~GLES2Renderer()
{
    if (mShader)
        effectDeleteProgram(mShader->takeProgram());
}

DisplayList::SharedPtr EffectBlurGLES2::GLES2Renderer::render(const DisplayList::SharedPtr &dl, const Rect &area) const
{
    const int radius = mEffect->getRadius();
    if (!radius)
        return dl;

    Surface::SharedPtr surface = dl->flatten();
    OpenGLContext* context = OpenGLContext::context();

    const Rect dstRect = getRect(area);

    OpenGLState* state = context->state();

    if(!mShader) {
        const GLchar* vertex[] = {
            "uniform mat4 u_projection;\n"
            "attribute vec4 a_position;\n"
            "attribute vec2 a_texCoord;\n"
            "varying vec2   v_texCoord;\n"
            "void main()\n"
            "{\n"
            "  gl_Position = u_projection * a_position;\n"
            "  v_texCoord = a_texCoord;\n"
            "}\n"
        };

        const GLchar* fragment[] = {
            "#ifdef GL_ES\n"
            "precision highp float;\n"
            "#endif\n"
            "uniform sampler2D s_texture;\n"
            "varying vec2 v_texCoord;\n"
            "uniform vec3 u_offsets;\n"
            "void main(void)\n"
            "{\n"
            "   vec4 sum = vec4(0.0);\n"
            "   sum += texture2D(s_texture, vec2(v_texCoord.x - u_offsets.x, v_texCoord.y - u_offsets.y));\n"
            "   sum += texture2D(s_texture, vec2(v_texCoord.x              , v_texCoord.y - u_offsets.y)) * 2.0;\n"
            "   sum += texture2D(s_texture, vec2(v_texCoord.x + u_offsets.x, v_texCoord.y - u_offsets.y));\n"
            "   sum += texture2D(s_texture, vec2(v_texCoord.x - u_offsets.x, v_texCoord.y              )) * 2.0;\n"
            "   sum += texture2D(s_texture, vec2(v_texCoord.x              , v_texCoord.y              ));\n"
            "   sum += texture2D(s_texture, vec2(v_texCoord.x + u_offsets.x, v_texCoord.y              )) * 2.0;\n"
            "   sum += texture2D(s_texture, vec2(v_texCoord.x - u_offsets.x, v_texCoord.y + u_offsets.y));\n"
            "   sum += texture2D(s_texture, vec2(v_texCoord.x              , v_texCoord.y + u_offsets.y)) * 2.0;\n"
            "   sum += texture2D(s_texture, vec2(v_texCoord.x + u_offsets.x, v_texCoord.y + u_offsets.y));\n"
            "   gl_FragColor = sum / 13.0;\n"
            "}\n"
        };
        mShader.reset(new Shader(context, vertex, fragment));
        mShader->use();
        mShader->defineAttribute(Shader::Position, "a_position");
        mShader->defineAttribute(Shader::TexCoord, "a_texCoord");
        mShader->defineUniform(Shader::Offsets, "u_offsets");
        mShader->defineUniform(Shader::ProjMatrix, "u_projection");

        glUniform1i(glGetUniformLocation(mShader->program(), "s_texture"), 0);
    }

    Surface::SharedPtr surface1 = requestSurface(dstRect.width, dstRect.height);
    const Rect surface1Rect(0, 0, surface1->getWidth(), surface1->getHeight());

    GraphicsEngine engine1(surface1);

    // populate result with an unblurred version of the source, centered
    GLuint texture = surface->getTexture();
    if (!texture)
        Log::warn(TRACE_GIBBON_GRAPHICS, "Bind failed %fx%f", surface->getWidth(), surface->getHeight());
    {
        Transform modelViewMatrix;
        const float dx = (surface1Rect.width - dstRect.width) - (surface->getWidth() - area.width);
        const float dy = (surface1Rect.height - dstRect.height) - (surface->getHeight() - area.height);
        modelViewMatrix.translate(-dx / surface1Rect.width, dy / surface1Rect.height, 0.);
        modelViewMatrix.scale(surface->getWidth() / surface1Rect.width, surface->getHeight() / surface1Rect.height, 1.);
        const bool hasClip = engine1.getState().hasClip;
        const Rect clipRect = engine1.clip();
        engine1.setClip(surface1Rect);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        engine1.setClip(Rect(area.x + radius, area.y + radius, area.width, area.height));
        static GLfloat texTransform[4] = { 0, 0, 1, 1 };
        context->engine()->blit(texture, modelViewMatrix, texTransform);
        if (hasClip)
            engine1.setClip(clipRect);
        else
            engine1.clearClip();
    }

    if (radius) {
        // Time to blur! The goal is to perform a series of box filters, approximating a Gaussian blur

        // Grab a second surface so we can pingpong between surface1 and surface2 during the passes.
        Surface::SharedPtr surface2 = GibbonApplication::instance()->getSurfaceCache()->requestSurface(surface1Rect.width, surface1Rect.height,
                                                                                                       Surface::Flags_Alpha|Surface::Flags_RenderTarget,
                                                                                                       true);
        GraphicsEngine engine2(surface2);
        GLuint texture1 = surface1->getTexture();
        GLuint texture2 = surface2->getTexture();

        OpenGLShader::Scope<Shader> scope(mShader);

        OpenGLState::Scope stateScope(state);

        mShader->bindVertexBuffer();
        glVertexAttribPointer(mShader->variable(Shader::Position), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
        stateScope.enableVAA(mShader->variable(Shader::Position));

        mShader->bindTextureBuffer();
        glVertexAttribPointer(mShader->variable(Shader::TexCoord), 2, GL_UNSIGNED_BYTE, GL_FALSE, 2 * sizeof(GLubyte), 0);
        stateScope.enableVAA(mShader->variable(Shader::TexCoord));

        const GLfloat blurOffsets[3] = { static_cast<float>(1. / area.width), static_cast<float>(1. / area.height), 0. };
        glUniform3fv(mShader->variable(Shader::Offsets), 1, blurOffsets);

        Transform projectionMatrix;
        projectionMatrix.scale(1, -1, 1);
        glUniformMatrix4fv(mShader->variable(Shader::ProjMatrix), 1, GL_FALSE, (GLfloat*)projectionMatrix.matrix());

        stateScope.flush();

        bool srcSurface1 = true;
        for (int i = 0; i < radius; ++i) {
            state->bindTexture(srcSurface1 ? texture1 : texture2);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
            if (srcSurface1) {
                context->popEngine();
            } else {
                context->pushEngine(&engine2);
            }
            srcSurface1 = !srcSurface1;
        }

        if (!srcSurface1)
            surface1.swap(surface2);

        if (context->engine() != &engine2) {
            assert(context->engine() == &engine1);
            context->pushEngine(&engine2);
        }
    }

    return DisplayList::SharedPtr(new DisplayList(surface1));
}

DisplayList::SharedPtr EffectTransformGLES2::GLES2Renderer::render(const DisplayList::SharedPtr &dl, const Rect &area) const
{
    Surface::SharedPtr surface = dl->flatten();
    OpenGLContext* context = OpenGLContext::context();

    const Rect resultRect(getRect(area));
    Surface::SharedPtr result = requestSurface(resultRect.width, resultRect.height);

    GLuint texture = surface->getTexture();
    if (!texture)
        Log::warn(TRACE_GIBBON_GRAPHICS, "Bind failed %fx%f", surface->getWidth(), surface->getHeight());

    GraphicsEngine engine(result);
    engine.clear(resultRect);

    const Rect surfaceRect = surface->getRect();
    GLfloat textureTransform[4] = { 0, 0, 1, 1 };
    if (area != surfaceRect) {
        textureTransform[0] = area.x / (float) surfaceRect.width;
        textureTransform[1] = area.y / (float) surfaceRect.height;
        textureTransform[2] = area.width / (float) surfaceRect.width;
        textureTransform[3] = area.height / (float) surfaceRect.height;
    }

    Transform transform = getTransform();

    if(!mEffect->getShowBackFace()) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
    }
    context->engine()->blit(texture, transform, textureTransform);
    glDisable(GL_CULL_FACE);

    return DisplayList::SharedPtr(new DisplayList(result));
}

EffectDesaturateGLES2::GLES2Renderer::~GLES2Renderer()
{
    if (mShader)
        effectDeleteProgram(mShader->takeProgram());
}

DisplayList::SharedPtr EffectDesaturateGLES2::GLES2Renderer::render(const DisplayList::SharedPtr &dl, const Rect &area) const
{
    Surface::SharedPtr surface = dl->flatten();
    OpenGLContext* context = OpenGLContext::context();
    OpenGLState* state = context->state();

    if(!mShader) {
        const GLchar* vertex[] = {
            "uniform mat4 u_projectionMatrix;\n"
            "attribute vec4 a_position;\n"
            "attribute vec2 a_texCoord;\n"
            "varying vec2   v_texCoord;\n"
            "void main()\n"
            "{\n"
            "  gl_Position = u_projectionMatrix * a_position;\n"
            "  v_texCoord = a_texCoord;\n"
            "}\n"
        };

        const GLchar* fragment[] = {
            "#ifdef GL_ES\n"
            "precision highp float;\n"
            "#endif\n"
            "uniform sampler2D s_texture;\n"
            "uniform vec3 u_color;\n"
            "varying vec2 v_texCoord;\n"
            "void main(void)\n"
            "{\n"
            "   vec3 weights = vec3(0.3, 0.59, 0.11);\n"
            "   vec4 src = texture2D(s_texture, vec2(v_texCoord.x, v_texCoord.y));\n"
            "   float luminance = dot(src.rgb, weights);\n"
            "   float dr = clamp(luminance * u_color.r + src.r * (1.0 - u_color.r), 0.0, 1.0);\n"
            "   float dg = clamp(luminance * u_color.g + src.g * (1.0 - u_color.g), 0.0, 1.0);\n"
            "   float db = clamp(luminance * u_color.b + src.b * (1.0 - u_color.b), 0.0, 1.0);\n"
            "   gl_FragColor = vec4(dr, dg, db, src.a);\n"
            "}\n"
        };

        mShader.reset(new Shader(context, vertex, fragment));
        mShader->use();
        mShader->defineUniform(Shader::ProjMatrix, "u_projectionMatrix");
        mShader->defineAttribute(Shader::Position, "a_position");
        mShader->defineAttribute(Shader::TexCoord, "a_texCoord");
        mShader->defineUniform(Shader::Color, "u_color");
        glUniform1i(glGetUniformLocation(mShader->program(), "s_texture"), 0);
    }

    OpenGLState::Scope stateScope(state);

    const Rect resultRect(getRect(area));
    Surface::SharedPtr result = requestSurface(resultRect.width, resultRect.height);
    const Rect surfaceRect = Rect(0, 0, result->getWidth(), result->getHeight());

    GraphicsEngine engine(result);
    engine.clear(surfaceRect);

    OpenGLShader::Scope<Shader> scope(mShader);

    Transform modelViewMatrix;
    modelViewMatrix.scale(1, -1, 1);

    const float dx = (surfaceRect.width - resultRect.width) - (surface->getWidth() - area.width);
    const float dy = (surfaceRect.height - resultRect.height) - (surface->getHeight() - area.height);
    modelViewMatrix.translate(-dx / surfaceRect.width, dy / surfaceRect.height, 0.);
    modelViewMatrix.scale(surface->getWidth() / surfaceRect.width, surface->getHeight() / surfaceRect.height, 1.);

    glUniformMatrix4fv(mShader->variable(Shader::ProjMatrix), 1, GL_FALSE, (GLfloat*)modelViewMatrix.matrix());

    mShader->bindVertexBuffer();
    glVertexAttribPointer(mShader->variable(Shader::Position), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    stateScope.enableVAA(mShader->variable(Shader::Position));

    const float c[3] = { mEffect->getRed(), mEffect->getGreen(), mEffect->getBlue() };
    glUniform3fv(mShader->variable(Shader::Color), 1, c);

    mShader->bindTextureBuffer();
    glVertexAttribPointer(mShader->variable(Shader::TexCoord), 2, GL_UNSIGNED_BYTE, GL_FALSE, 2 * sizeof(GLubyte), 0);
    stateScope.enableVAA(mShader->variable(Shader::TexCoord));

    GLuint texture = surface->getTexture();
    if (!texture)
        Log::warn(TRACE_GIBBON_GRAPHICS, "Bind failed %fx%f", surface->getWidth(), surface->getHeight());
    state->bindTexture(texture);

    stateScope.flush();

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);

    return DisplayList::SharedPtr(new DisplayList(result));
}

EffectTintGLES2::GLES2Renderer::~GLES2Renderer()
{
    if (mShader)
        effectDeleteProgram(mShader->takeProgram());
}

DisplayList::SharedPtr EffectTintGLES2::GLES2Renderer::render(const DisplayList::SharedPtr &dl, const Rect &area) const
{
    Surface::SharedPtr surface = dl->flatten();
    OpenGLContext* context = OpenGLContext::context();
    OpenGLState* state = context->state();

    if(!mShader) {
        const GLchar* vertex[] = {
            "uniform mat4 u_projectionMatrix;\n"
            "attribute vec4 a_position;\n"
            "attribute vec2 a_texCoord;\n"
            "varying vec2   v_texCoord;\n"
            "void main()\n"
            "{\n"
            "  gl_Position = u_projectionMatrix * a_position;\n"
            "  v_texCoord = a_texCoord;\n"
            "}\n"
        };

        const GLchar* fragment[] = {
            "#ifdef GL_ES\n"
            "precision highp float;\n"
            "#endif\n"
            "uniform sampler2D s_texture;\n"
            "uniform vec3 u_color;\n"
            "varying vec2 v_texCoord;\n"
            "void main(void)\n"
            "{\n"
            "   vec4 src = texture2D(s_texture, vec2(v_texCoord.x, v_texCoord.y));\n"
            "   gl_FragColor = src * vec4(u_color, 1);\n"
            "}\n"
        };

        mShader.reset(new Shader(context, vertex, fragment));
        mShader->use();
        mShader->defineUniform(Shader::ProjMatrix, "u_projectionMatrix");
        mShader->defineAttribute(Shader::Position, "a_position");
        mShader->defineAttribute(Shader::TexCoord, "a_texCoord");
        mShader->defineUniform(Shader::Color, "u_color");
        glUniform1i(glGetUniformLocation(mShader->program(), "s_texture"), 0);
    }

    OpenGLState::Scope stateScope(state);

    const Rect resultRect(getRect(area));
    Surface::SharedPtr result = requestSurface(resultRect.width, resultRect.height);
    const Rect surfaceRect = Rect(0, 0, result->getWidth(), result->getHeight());

    GraphicsEngine engine(result);
    engine.clear(surfaceRect);

    OpenGLShader::Scope<Shader> scope(mShader);

    Transform modelViewMatrix;
    modelViewMatrix.scale(1, -1, 1);

    const float dx = (surfaceRect.width - resultRect.width) - (surface->getWidth() - area.width);
    const float dy = (surfaceRect.height - resultRect.height) - (surface->getHeight() - area.height);
    modelViewMatrix.translate(-dx / surfaceRect.width, dy / surfaceRect.height, 0.);
    modelViewMatrix.scale(surface->getWidth() / surfaceRect.width, surface->getHeight() / surfaceRect.height, 1.);

    glUniformMatrix4fv(mShader->variable(Shader::ProjMatrix), 1, GL_FALSE, (GLfloat*)modelViewMatrix.matrix());

    mShader->bindVertexBuffer();
    glVertexAttribPointer(mShader->variable(Shader::Position), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    stateScope.enableVAA(mShader->variable(Shader::Position));

    const float c[3] = { mEffect->getRed(), mEffect->getGreen(), mEffect->getBlue() };
    glUniform3fv(mShader->variable(Shader::Color), 1, c);

    mShader->bindTextureBuffer();
    glVertexAttribPointer(mShader->variable(Shader::TexCoord), 2, GL_UNSIGNED_BYTE, GL_FALSE, 2 * sizeof(GLubyte), 0);
    stateScope.enableVAA(mShader->variable(Shader::TexCoord));

    GLuint texture = surface->getTexture();
    if (!texture)
        Log::warn(TRACE_GIBBON_GRAPHICS, "Bind failed %fx%f", surface->getWidth(), surface->getHeight());
    state->bindTexture(texture);

    stateScope.flush();

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);

    return DisplayList::SharedPtr(new DisplayList(result));
}
