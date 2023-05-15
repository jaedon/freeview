#ifndef OPENGLFLOATS_H
#define OPENGLFLOATS_H

#include <Surface.h>
#include <stdlib.h>

struct OpenGLFloats
{
public:
    OpenGLFloats()
        : f(0), idx(0), size(0)
    {
        reserve(5000);
    }
    ~OpenGLFloats()
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

    GLfloat* ptr() { return f; }

    void reset()
    {
        idx = f;
    }

private:
    GLfloat* f;
    GLfloat* idx;
    int size;
};

#endif
