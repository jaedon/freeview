#ifndef __egltypes_h_
#define __egltypes_h_

#ifdef __cplusplus
extern "C" {
#endif

typedef int EGLBoolean;
typedef int EGLint;
typedef void *EGLDisplay;
typedef void *EGLConfig;
typedef void *EGLSurface;
typedef void *EGLContext;

#define EGL_DEFAULT_DISPLAY ((NativeDisplayType)0)
#define EGL_NO_CONTEXT ((EGLContext)0)
#define EGL_NO_DISPLAY ((EGLDisplay)0)
#define EGL_NO_SURFACE ((EGLSurface)0)

#ifdef __cplusplus
}
#endif

#endif /* ___egltypes_h_ */
