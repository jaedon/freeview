/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef OPENGLSHADERS_H
#define OPENGLSHADERS_H

#include <nrd/AppLog.h>

namespace netflix {
namespace gibbon {

class OpenGLShader
{
public:
    OpenGLShader(OpenGLContext* ctx, const GLchar* vertex, const GLchar* fragment)
        : context(ctx)
    {
        const GLchar* v[] = { vertex };
        const GLchar* f[] = { fragment };
        init(v, f);
    }

    template<int VertexCount, int FragmentCount>
    OpenGLShader(OpenGLContext* ctx, const GLchar* (&vertex)[VertexCount], const GLchar* (&fragment)[FragmentCount])
        : context(ctx)
    {
        init(vertex, fragment);
    }

    ~OpenGLShader()
    {
        if (mValid) {
            glDeleteProgram(mProgram);
        }
    }

    template<int VertexCount, int FragmentCount>
    void init(const GLchar* (&vertex)[VertexCount], const GLchar* (&fragment)[FragmentCount])
    {
        const GLuint v = compile(GL_VERTEX_SHADER, vertex, &mValid);
        if (!mValid)
            return;
        const GLuint f = compile(GL_FRAGMENT_SHADER, fragment, &mValid);
        if (!mValid)
            return;
        mProgram = glCreateProgram();
        glAttachShader(mProgram, v);
        glAttachShader(mProgram, f);

        glLinkProgram(mProgram);

        glDeleteShader(v);
        glDeleteShader(f);
    }

    operator GLuint() { return mProgram; }
    GLuint program() const { return mProgram; }

    GLuint takeProgram();

    void operator()() { context->state()->useProgram(mProgram); }
    void use() { context->state()->useProgram(mProgram); }

    bool isValid() const { return mValid; }

    void defineVariable(size_t pos, GLuint location)
    {
        if (mVariables.size() <= pos) {
            mVariables.resize(pos + 1);
        }
        mVariables[pos] = location;
    }
    void defineUniform(size_t pos, const char* name) { defineVariable(pos, glGetUniformLocation(mProgram, name)); }
    void defineAttribute(size_t pos, const char* name) { defineVariable(pos, glGetAttribLocation(mProgram, name)); }
    GLuint variable(size_t pos) const { assert(pos < mVariables.size()); return mVariables[pos]; }

    template<typename T>
    class Scope
    {
    public:
        Scope(const shared_ptr<T>& ptr)
            : shader(ptr)
        {
            shader->use();
            shader->prepare();
        }
        ~Scope()
        {
            shader->clear();
        }
    private:
        shared_ptr<T> shader;
    };

private:
    template<int Count>
    GLuint compile(GLuint type, const GLchar* (&source)[Count], bool* ok)
    {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, Count, source, 0);
        glCompileShader(shader);
        GLint compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            std::string log(length, ' ');
            glGetShaderInfoLog(shader, length, &length, &log[0]);
            Log::error(TRACE_GIBBON_GRAPHICS, "Shader error: %s", log.c_str());
            *ok = false;
            return 0;
        }
        *ok = true;
        return shader;
    }

protected:
    OpenGLContext* context;

private:
    std::vector<GLuint> mVariables;
    GLuint mProgram;
    bool mValid;
};

inline GLuint OpenGLShader::takeProgram()
{
    const GLuint prog = mProgram;
    mProgram = 0;
    mValid = false;
    return prog;
}

class RectShader : public OpenGLShader
{
public:
    template<int VertexCount, int FragmentCount>
    RectShader(OpenGLContext* ctx, const GLchar* (&vertex)[VertexCount], const GLchar* (&fragment)[FragmentCount])
        : OpenGLShader(ctx, vertex, fragment)
    {
        glGenBuffers(2, mVertexBuffer);
        GLfloat vertices[12] = {
            -1.0,  1.0, 0.0,
            -1.0, -1.0, 0.0,
            1.0, -1.0, 0.0,
            1.0,  1.0, 0.0,
        };
        context->state()->bindBuffer(GL_ARRAY_BUFFER, mVertexBuffer[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        GLubyte indices[6] = {0, 1, 2, 0, 2, 3};
        context->state()->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVertexBuffer[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    }

    void bindVertexBuffer()
    {
        context->state()->bindBuffer(GL_ARRAY_BUFFER, mVertexBuffer[0]);
    }

    void prepare()
    {
        context->state()->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVertexBuffer[1]);
    }

    void clear()
    {
    }

protected:
    GLuint mVertexBuffer[2];
};

class FillShader : public RectShader
{
public:
    enum { ProjMatrix, MVMatrix, Position, Color, Last = Color };

    template<int VertexCount, int FragmentCount>
    FillShader(OpenGLContext* ctx, const GLchar* (&vertex)[VertexCount], const GLchar* (&fragment)[FragmentCount])
        : RectShader(ctx, vertex, fragment)
    {
    }
};

class LineShader : public FillShader
{
public:
    template<int VertexCount, int FragmentCount>
    LineShader(OpenGLContext* ctx, const GLchar* (&vertex)[VertexCount], const GLchar* (&fragment)[FragmentCount])
        : FillShader(ctx, vertex, fragment)
    {
        GLubyte indices[8] = {1, 2, 2, 3, 3, 0, 0, 1};
        context->state()->bindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVertexBuffer[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    }
};

class BlitShader : public RectShader
{
public:
    enum { ProjMatrix, MVMatrix, Position, TexCoord, TextureTransform, Opacity, Last = Opacity };

    template<int VertexCount, int FragmentCount>
    BlitShader(OpenGLContext* ctx, const GLchar* (&vertex)[VertexCount], const GLchar* (&fragment)[FragmentCount])
        : RectShader(ctx, vertex, fragment)
    {
        glGenBuffers(1, &mTextureCoordBuffer);
        GLubyte textureCoordinates[8] = {
            0, 0,
            0, 1,
            1, 1,
            1, 0
        };
        context->state()->bindBuffer(GL_ARRAY_BUFFER, mTextureCoordBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinates), textureCoordinates, GL_STATIC_DRAW);
    }

    void bindTextureBuffer()
    {
        context->state()->bindBuffer(GL_ARRAY_BUFFER, mTextureCoordBuffer);
    }

private:
    GLuint mTextureCoordBuffer;
};

class TextShader : public OpenGLShader
{
public:
    enum { ProjMatrix, MVMatrix, Vertex, TexCoord, Color, Last = Color };

    template<int VertexCount, int FragmentCount>
    TextShader(OpenGLContext* ctx, const GLchar* (&vertex)[VertexCount], const GLchar* (&fragment)[FragmentCount])
        : OpenGLShader(ctx, vertex, fragment)
    {
    }

    void prepare()
    {
    }

    void clear()
    {
    }
};

}} // netflix::gibbon

#endif
