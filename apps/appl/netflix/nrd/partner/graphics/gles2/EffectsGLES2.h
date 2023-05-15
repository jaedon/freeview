/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __EFFECTSGLES_H__
#define __EFFECTSGLES_H__

#include <Effects.h>
#include <OpenGLContext.h>
#include <OpenGLState.h>
#include <OpenGLShaders.h>
#include <nrdbase/tr1.h>

namespace netflix {
namespace gibbon {

class EffectBlurGLES2 : public EffectBlur
{
public:
    EffectBlurGLES2() {}
    virtual std::string describe() const { return "GL" + EffectBlur::describe(); }

    class GLES2Renderer : public DefaultRenderer {
    public:
        GLES2Renderer(EffectBlur::SharedPtr e) : DefaultRenderer(e) {}
        ~GLES2Renderer();
        virtual DisplayList::SharedPtr render(const DisplayList::SharedPtr &dl, const Rect &rect) const;

    private:
        class Shader : public BlitShader
        {
        public:
            enum { Offsets = BlitShader::Last + 1 };

            template<int VertexCount, int FragmentCount>
            Shader(OpenGLContext* ctx, const GLchar *(&vertex)[VertexCount], const GLchar *(&fragment)[FragmentCount])
                : BlitShader(ctx, vertex, fragment)
            {
            }
        };
        mutable shared_ptr<Shader> mShader;
    };
    virtual Renderer::SharedPtr getDefaultRenderer() {
        return Renderer::SharedPtr(new GLES2Renderer(static_pointer_cast<EffectBlur>(shared_from_this())));
    }
private:
};

class EffectTransformGLES2 : public EffectTransform
{
public:
    EffectTransformGLES2() : EffectTransform() { }
    virtual std::string describe() const { return "GL" + EffectTransform::describe(); }

    class GLES2Renderer : public DefaultRenderer {
    public:
        GLES2Renderer(EffectTransform::SharedPtr e) : DefaultRenderer(e) {}
        virtual DisplayList::SharedPtr render(const DisplayList::SharedPtr &dl, const Rect &rect) const;
    };
    virtual Renderer::SharedPtr getDefaultRenderer() {
        return Renderer::SharedPtr(new GLES2Renderer(static_pointer_cast<EffectTransform>(shared_from_this())));
    }
};

class EffectDesaturateGLES2 : public EffectDesaturate
{
public:
    EffectDesaturateGLES2() : EffectDesaturate() { }
    virtual std::string describe() const { return "GL" + EffectDesaturate::describe(); }

    class GLES2Renderer : public DefaultRenderer {
    public:
        GLES2Renderer(EffectDesaturate::SharedPtr e) : DefaultRenderer(e) {}
        ~GLES2Renderer();
        virtual DisplayList::SharedPtr render(const DisplayList::SharedPtr &dl, const Rect &rect) const;

    private:
        class Shader : public BlitShader
        {
        public:
            enum { Color = BlitShader::Last + 1 };

            template<int VertexCount, int FragmentCount>
            Shader(OpenGLContext* ctx, const GLchar *(&vertex)[VertexCount], const GLchar *(&fragment)[FragmentCount])
                : BlitShader(ctx, vertex, fragment)
            {
            }
        };
        mutable shared_ptr<Shader> mShader;
    };
    virtual Renderer::SharedPtr getDefaultRenderer() {
        return Renderer::SharedPtr(new GLES2Renderer(static_pointer_cast<EffectDesaturate>(shared_from_this())));
    }
};

class EffectTintGLES2 : public EffectTint
{
public:
    EffectTintGLES2() : EffectTint() { }
    virtual std::string describe() const { return "GL" + EffectTint::describe(); }

    class GLES2Renderer : public DefaultRenderer {
    public:
        GLES2Renderer(EffectTint::SharedPtr e) : DefaultRenderer(e) {}
        ~GLES2Renderer();
        virtual DisplayList::SharedPtr render(const DisplayList::SharedPtr &dl, const Rect &rect) const;

    private:
        class Shader : public BlitShader
        {
        public:
            enum { Color = BlitShader::Last + 1 };

            template<int VertexCount, int FragmentCount>
            Shader(OpenGLContext* ctx, const GLchar *(&vertex)[VertexCount], const GLchar *(&fragment)[FragmentCount])
                : BlitShader(ctx, vertex, fragment)
            {
            }
        };
        mutable shared_ptr<Shader> mShader;
    };
    virtual Renderer::SharedPtr getDefaultRenderer() {
        return Renderer::SharedPtr(new GLES2Renderer(static_pointer_cast<EffectTint>(shared_from_this())));
    }
};

}} // namespace netflix::gibbon

#endif /* __SURFACEEFFECTSGLES_H__ */
