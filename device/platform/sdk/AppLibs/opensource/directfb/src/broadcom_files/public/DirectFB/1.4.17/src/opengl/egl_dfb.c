/*
   (c) Copyright 2001-2008  The world wide DirectFB Open Source Community (directfb.org)
   (c) Copyright 2000-2004  Convergence (integrated media) GmbH

   All rights reserved.

   Written by Denis Oliver Kropp <dok@directfb.org>,
              Andreas Hundt <andi@fischlustig.de>,
              Sven Neumann <neo@directfb.org>,
              Ville Syrjälä <syrjala@sci.fi> and
              Claudio Ciccani <klan@users.sf.net>.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the
   Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <config.h>

#include <directfb.h>
#include <directfbgl.h>

#include <direct/debug.h>

#include <opengl/egl_dfb.h>
#include <opengl/opengl_api.h>

#include <EGL/egl.h>

#include <pthread.h>

D_DEBUG_DOMAIN( EGL_DFB, "EGL/DFB", "EGL on DirectFB" );

/**********************************************************************************************************************/

typedef struct {
    EGLint            egl_error;

    IDirectFB        *dfb;

    IDirectFBSurface *surface;
    IDirectFBGL      *gl;


} EGLDFBContext;

/**********************************************************************************************************************/

#define EGL_RETURN(error, val) { ctx->egl_error = error; return val; }

/**********************************************************************************************************************/

static pthread_once_t egldfb_once = PTHREAD_ONCE_INIT;
static pthread_key_t  egldfb_tls;

static void
egldfb_destructor( void *context )
{
    EGLDFBContext *ctx = context;

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );

    if (ctx)
    {
        if (ctx->gl)
            ctx->gl->Release( ctx->gl );

        if (ctx->surface)
            ctx->surface->Release( ctx->surface );

        if (ctx->dfb)
            ctx->dfb->Release( ctx->dfb );

        D_FREE( ctx );
    }
}

static void
egldfb_once_init( void )
{
    pthread_key_create( &egldfb_tls, egldfb_destructor );
}

/**********************************************************************************************************************/

static EGLDFBContext *
get_egl_dfb_context( void )
{
    EGLDFBContext *ctx;

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );

    pthread_once( &egldfb_once, egldfb_once_init );

    ctx = pthread_getspecific( egldfb_tls );
    if (!ctx) {
        ctx = D_CALLOC( 1, sizeof(EGLDFBContext) );
        if (!ctx) {
            D_OOM();
            return NULL;
        }

        pthread_setspecific( egldfb_tls, ctx );
    }

    return ctx;
}

/**********************************************************************************************************************/

EGLint eglGetError (void)
{
    EGLint         error;
    EGLDFBContext *ctx = get_egl_dfb_context();

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );

    error = ctx->egl_error;
    ctx->egl_error = EGL_SUCCESS;

    return error;
}

EGLDisplay eglGetDisplay (EGLNativeDisplayType display)
{
    EGLDFBContext *ctx = get_egl_dfb_context();

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );

    if (!display) {
        IDirectFB *dfb;
        
        DirectFBInit (NULL, NULL);
        if (DirectFBCreate (&dfb))
            EGL_RETURN (EGL_BAD_ACCESS, EGL_NO_DISPLAY);

        dfb->SetCooperativeLevel( dfb, DFSCL_FULLSCREEN );

        display = dfb;
    }

    ctx->dfb = display;
    
    EGL_RETURN (EGL_SUCCESS, display);
}

EGLBoolean eglInitialize (EGLDisplay dpy, EGLint *major, EGLint *minor)
{
    EGLDFBContext *ctx = get_egl_dfb_context();

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );

    if (!dpy)
        EGL_RETURN (EGL_BAD_DISPLAY, EGL_FALSE);
    
    if (major)
        *major = 1;
    if (minor)
        *minor = 3;
        
    EGL_RETURN (EGL_SUCCESS, EGL_TRUE);
}

EGLBoolean eglTerminate (EGLDisplay dpy)
{
    EGLDFBContext *ctx = get_egl_dfb_context();

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );

    if (!dpy)
        EGL_RETURN (EGL_BAD_DISPLAY, EGL_FALSE);
    
    EGL_RETURN (EGL_SUCCESS, EGL_TRUE);
}

const char* eglQueryString (EGLDisplay dpy, EGLint name)
{
    EGLDFBContext *ctx = get_egl_dfb_context();

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );

    if (!dpy)
        EGL_RETURN (EGL_BAD_DISPLAY, NULL);
    
    switch (name) {
        case EGL_CLIENT_APIS:
            EGL_RETURN (EGL_SUCCESS, "OpenGL");
        case EGL_EXTENSIONS:
            EGL_RETURN (EGL_SUCCESS, "EGL_KHR_image");
        case EGL_VENDOR:
            EGL_RETURN (EGL_SUCCESS, "Denis Oliver Kropp");
        case EGL_VERSION:
            EGL_RETURN (EGL_SUCCESS, "1.3");
        default:
            break;
    }
    
    EGL_RETURN (EGL_BAD_PARAMETER, NULL);
}

EGLBoolean eglGetConfigs (EGLDisplay dpy,
                          EGLConfig *configs, EGLint config_size, EGLint *num_configs)
{
    EGLDFBContext *ctx = get_egl_dfb_context();

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );

    if (!dpy)
        EGL_RETURN (EGL_BAD_DISPLAY, EGL_FALSE);

    D_UNIMPLEMENTED ();

    EGL_RETURN (EGL_SUCCESS, EGL_TRUE);
}

EGLBoolean eglChooseConfig (EGLDisplay dpy, const EGLint *attrib_list,
                            EGLConfig *configs, EGLint config_size, EGLint *num_config)
{
    EGLDFBContext *ctx = get_egl_dfb_context();

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );

    if (!dpy)
        EGL_RETURN (EGL_BAD_DISPLAY, EGL_FALSE);

    D_UNIMPLEMENTED ();

    EGL_RETURN (EGL_SUCCESS, EGL_TRUE);
}

EGLBoolean eglGetConfigAttrib (EGLDisplay dpy, EGLConfig config,
                               EGLint attribute, EGLint *value)
{
    EGLDFBContext *ctx = get_egl_dfb_context();

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );

    if (!dpy)
        EGL_RETURN (EGL_BAD_DISPLAY, EGL_FALSE);

    D_UNIMPLEMENTED ();

    EGL_RETURN (EGL_SUCCESS, EGL_TRUE);
}

EGLSurface eglCreateWindowSurface (EGLDisplay dpy, EGLConfig config,
                                   EGLNativeWindowType win, const EGLint *attrib)
{
    EGLDFBContext    *ctx = get_egl_dfb_context();
    IDirectFB        *dfb = dpy;
    IDirectFBSurface *surface;

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );
    
    if (!dpy)
        EGL_RETURN (EGL_BAD_DISPLAY, EGL_NO_SURFACE);
        
    if (win) {
        if (win->GetSurface (win, &surface))
            EGL_RETURN (EGL_BAD_ALLOC, EGL_NO_SURFACE);
    }
    else {
        DFBResult             ret;
        DFBSurfaceDescription desc;

        desc.flags = DSDESC_CAPS;
        desc.caps  = DSCAPS_PRIMARY | DSCAPS_FLIPPING;

        ret = dfb->CreateSurface( dfb, &desc, &surface );
        if (ret) {
            D_DERROR( ret, "EGL/DFB: Could not create primary surface!\n" );
            EGL_RETURN (EGL_BAD_ALLOC, EGL_NO_SURFACE);
        }
    }

    EGL_RETURN (EGL_SUCCESS, surface);
}

EGLSurface eglCreatePbufferSurface (EGLDisplay dpy, EGLConfig config, const EGLint *attrib)
{
    EGLDFBContext         *ctx = get_egl_dfb_context();
    IDirectFB             *dfb = (IDirectFB*)dpy;
    IDirectFBSurface      *surface;
    DFBSurfaceDescription  dsc;

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );
    
    if (!dpy)
        EGL_RETURN (EGL_BAD_DISPLAY, EGL_NO_SURFACE);
 
    dsc.flags = DSDESC_NONE;
    
    if (attrib) {
        while (*attrib != EGL_NONE) {
            switch (*attrib) {
                case EGL_WIDTH:
                    dsc.flags |= DSDESC_WIDTH;
                    dsc.width  = attrib[1];
                    break;
                case EGL_HEIGHT:
                    dsc.flags |= DSDESC_HEIGHT;
                    dsc.height = attrib[1];
                    break;
                case EGL_LARGEST_PBUFFER:
                case EGL_VG_COLORSPACE:
                    break;
                case EGL_VG_ALPHA_FORMAT:
                    if (attrib[1] != EGL_ALPHA_FORMAT_NONPRE) {
                        dsc.flags |= DSDESC_CAPS;
                        dsc.caps   = DSCAPS_PREMULTIPLIED;
                    }
                    break;
                default:
                    EGL_RETURN (EGL_BAD_ATTRIBUTE, EGL_NO_SURFACE);
            }
            attrib += 2;
        }
    }
    
    if (dfb->CreateSurface (dfb, &dsc, &surface))
        EGL_RETURN (EGL_BAD_ALLOC, EGL_NO_SURFACE);
        
    EGL_RETURN (EGL_SUCCESS, (EGLSurface)surface);
}

EGLSurface eglCreatePbufferFromClientBuffer (EGLDisplay dpy, 
                                             EGLenum buftype, EGLClientBuffer buffer,
                                             EGLConfig config, const EGLint *attrib)
{
    EGLDFBContext *ctx = get_egl_dfb_context();

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );

    D_UNIMPLEMENTED ();

    EGL_RETURN (EGL_BAD_DISPLAY, EGL_NO_SURFACE);
}


EGLSurface eglCreatePixmapSurface (EGLDisplay dpy, EGLConfig config,
                                   EGLNativePixmapType pixmap, const EGLint *attrib)
{
    EGLDFBContext *ctx = get_egl_dfb_context();

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );

    if (!dpy)
        EGL_RETURN (EGL_BAD_DISPLAY, EGL_NO_SURFACE);
        
    if (!pixmap)
        EGL_RETURN (EGL_BAD_NATIVE_PIXMAP, EGL_NO_SURFACE);
 
    pixmap->AddRef (pixmap);
        
    EGL_RETURN (EGL_SUCCESS, pixmap);
} 

EGLBoolean eglDestroySurface (EGLDisplay dpy, EGLSurface surface)
{
    EGLDFBContext *ctx = get_egl_dfb_context();

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );

    if (!dpy)
        EGL_RETURN (EGL_BAD_DISPLAY, EGL_FALSE);
        
    if (!surface)
        EGL_RETURN (EGL_BAD_SURFACE, EGL_FALSE);
 
    ((IDirectFBSurface*)surface)->Release (surface);    
        
    EGL_RETURN (EGL_SUCCESS, EGL_TRUE);
}

EGLBoolean eglSurfaceAttrib (EGLDisplay dpy, EGLSurface surface,
                             EGLint attribute, EGLint value)
{
    EGLDFBContext *ctx = get_egl_dfb_context();

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );

    if (!dpy)
        EGL_RETURN (EGL_BAD_DISPLAY, EGL_FALSE);
        
    if (!surface)
        EGL_RETURN (EGL_BAD_SURFACE, EGL_FALSE);
        
    EGL_RETURN (EGL_SUCCESS, EGL_TRUE);
}

EGLBoolean eglQuerySurface (EGLDisplay dpy, EGLSurface surface,
                            EGLint attribute, EGLint *value)
{
    EGLDFBContext          *ctx = get_egl_dfb_context();
    IDirectFBSurface       *s = (IDirectFBSurface*)surface;
    DFBSurfaceCapabilities  caps;

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );
    
    if (!dpy)
        EGL_RETURN (EGL_BAD_DISPLAY, EGL_FALSE);
        
    if (!surface)
        EGL_RETURN (EGL_BAD_SURFACE, EGL_FALSE);
        
    if (!value)
        EGL_RETURN (EGL_BAD_PARAMETER, EGL_FALSE);
        
    switch (attribute) {
        case EGL_WIDTH:
            s->GetSize (s, value, NULL);
            break;
        case EGL_HEIGHT:
            s->GetSize (s, NULL, value);
            break;
        case EGL_LARGEST_PBUFFER:
            *value = EGL_FALSE;
            break;
        case EGL_VG_COLORSPACE:
            *value = EGL_COLORSPACE_LINEAR;
            break;
        case EGL_VG_ALPHA_FORMAT:
            s->GetCapabilities (s, &caps);
            *value = (caps & DSCAPS_PREMULTIPLIED) ? EGL_ALPHA_FORMAT_PRE : EGL_ALPHA_FORMAT_NONPRE;
            break;
        default:
            *value = EGL_UNKNOWN;
            EGL_RETURN (EGL_BAD_PARAMETER, EGL_FALSE);
    }     
        
    EGL_RETURN (EGL_SUCCESS, EGL_TRUE);
}

EGLBoolean eglBindTexImage (EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
    EGLDFBContext *ctx = get_egl_dfb_context();

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );

    EGL_RETURN (EGL_BAD_PARAMETER, EGL_FALSE);
}

EGLBoolean eglReleaseTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
    EGLDFBContext *ctx = get_egl_dfb_context();

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );

    EGL_RETURN (EGL_BAD_PARAMETER, EGL_FALSE);
}


EGLBoolean eglBindAPI (EGLenum api)
{
    EGLDFBContext *ctx = get_egl_dfb_context();

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );

    if (api != EGL_OPENVG_API)
        EGL_RETURN (EGL_BAD_PARAMETER, EGL_FALSE);
       
    EGL_RETURN (EGL_SUCCESS, EGL_TRUE);
}

EGLenum eglQueryAPI (void)
{
    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );

    return EGL_OPENGL_ES_API;
}

EGLContext eglCreateContext (EGLDisplay dpy, 
                             EGLConfig config, EGLContext share_context,
                             const EGLint *attrib_list)
{
    EGLDFBContext *ctx = get_egl_dfb_context();

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );
    
    if (!dpy)
        EGL_RETURN (EGL_BAD_DISPLAY, EGL_NO_CONTEXT);
        
    EGL_RETURN (EGL_SUCCESS, ctx);
}

EGLBoolean eglDestroyContext (EGLDisplay dpy, EGLContext context)
{
    EGLDFBContext *ctx = get_egl_dfb_context();

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );

    if (!dpy)
        EGL_RETURN (EGL_BAD_DISPLAY, EGL_FALSE);
        
    if (!context)
        EGL_RETURN (EGL_BAD_CONTEXT, EGL_FALSE);

    if (ctx->gl) {
        ctx->gl->Release( ctx->gl );
        ctx->gl = NULL;
    }

    if (ctx->surface) {
        ctx->surface->Release( ctx->surface );
        ctx->surface = NULL;
    }

    EGL_RETURN (EGL_SUCCESS, EGL_TRUE);
}

EGLBoolean eglMakeCurrent (EGLDisplay dpy,
                           EGLSurface draw, EGLSurface read, EGLContext context)
{
    DFBResult         ret;
    EGLDFBContext    *ctx     = get_egl_dfb_context();
    IDirectFBSurface *surface = draw;
    IDirectFBGL      *gl      = NULL;

    D_DEBUG_AT( EGL_DFB, "%s( %p, %p, %p, %p )\n", __FUNCTION__, dpy, draw, read, context );

    if (!dpy)
        EGL_RETURN (EGL_BAD_DISPLAY, EGL_FALSE);
        
    if ((context != NULL) != (draw != NULL))
        EGL_RETURN (EGL_BAD_MATCH, EGL_FALSE);
        
    if (draw != read)
        EGL_RETURN (EGL_BAD_MATCH, EGL_FALSE);

    if (surface) {
        ret = surface->AddRef( surface );
        if (ret) {
            D_DERROR( ret, "EGL/DFB: Could not AddRef() draw surface!\n" );
            EGL_RETURN (EGL_BAD_SURFACE, EGL_FALSE);
        }

        ret = surface->GetGL( surface, &gl );
        if (ret) {
            D_DERROR( ret, "EGL/DFB: Could not GetGL() from draw surface!\n" );
            surface->Release( surface );
            EGL_RETURN (EGL_BAD_SURFACE, EGL_FALSE);
        }
    }
    
    if (ctx->gl)
        ctx->gl->Release( ctx->gl );

    if (ctx->surface)
        ctx->surface->Release( ctx->surface );

    ctx->surface = surface;
    ctx->gl      = gl;

    if (gl)
        gl->Lock( gl );

    EGL_RETURN (EGL_SUCCESS, EGL_TRUE);
}

EGLContext eglGetCurrentContext (void)
{
    EGLDFBContext *ctx = get_egl_dfb_context();

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );

    EGL_RETURN (EGL_SUCCESS, pthread_getspecific( egldfb_tls ));
}

EGLSurface eglGetCurrentSurface (EGLint which)
{
    EGLDFBContext *ctx = get_egl_dfb_context();

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );
    
    if (which != EGL_DRAW && which != EGL_READ)
        EGL_RETURN (EGL_BAD_PARAMETER, EGL_NO_SURFACE);
    
    EGL_RETURN (EGL_BAD_CONTEXT, EGL_NO_SURFACE);
}

EGLDisplay eglGetCurrentDisplay (void)
{
    EGLDFBContext *ctx = get_egl_dfb_context();

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );
    
    EGL_RETURN (EGL_SUCCESS, ctx->dfb);
}

EGLBoolean eglQueryContext (EGLDisplay dpy, 
                            EGLContext context, EGLint attribute, EGLint *value)
{
    EGLDFBContext *ctx = get_egl_dfb_context();

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );

    if (!dpy)
        EGL_RETURN (EGL_BAD_DISPLAY, EGL_FALSE);
        
    if (!context)
        EGL_RETURN (EGL_BAD_CONTEXT, EGL_FALSE);
        
    if (!value)
        EGL_RETURN (EGL_BAD_PARAMETER, EGL_FALSE);
        
    switch (attribute) {
        case EGL_CONTEXT_CLIENT_TYPE:
            *value = EGL_OPENGL_ES_API;
            EGL_RETURN (EGL_SUCCESS, EGL_TRUE);
        default:
            break;
    }

    EGL_RETURN (EGL_SUCCESS, EGL_FALSE);
}

EGLBoolean eglWaitClient (void)
{
    EGLDFBContext *ctx = get_egl_dfb_context();

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );

    EGL_RETURN (EGL_SUCCESS, EGL_TRUE);
}

EGLBoolean eglWaitGL (void)
{
    EGLDFBContext *ctx = get_egl_dfb_context();

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );

    EGL_RETURN (EGL_SUCCESS, EGL_TRUE);
}

EGLBoolean eglWaitNative (EGLint engine)
{
    EGLDFBContext *ctx = get_egl_dfb_context();

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );

    EGL_RETURN (EGL_SUCCESS, EGL_TRUE);
}

EGLBoolean eglSwapBuffers (EGLDisplay dpy, EGLSurface surface)
{
    EGLDFBContext *ctx = get_egl_dfb_context();

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );

    if (!dpy)
        EGL_RETURN (EGL_BAD_DISPLAY, EGL_FALSE);

    if (!surface)
        EGL_RETURN (EGL_BAD_SURFACE, EGL_FALSE);
    
    if (ctx->gl)
        ctx->gl->Unlock( ctx->gl );
        
    ((IDirectFBSurface*)surface)->Flip (surface, NULL, 0);
    
    if (ctx->gl)
        ctx->gl->Lock( ctx->gl );

    EGL_RETURN (EGL_SUCCESS, EGL_TRUE);
}

EGLBoolean eglCopyBuffers (EGLDisplay dpy, 
                           EGLSurface source, EGLNativePixmapType dest)
{
    EGLDFBContext *ctx = get_egl_dfb_context();

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );

    if (!dpy)
        EGL_RETURN (EGL_BAD_DISPLAY, EGL_FALSE);

    if (!source)
        EGL_RETURN (EGL_BAD_SURFACE, EGL_FALSE);
        
    if (!dest)
        EGL_RETURN (EGL_BAD_NATIVE_PIXMAP, EGL_FALSE);
        
    dest->SetClip (dest, NULL);
    dest->SetBlittingFlags (dest, DSBLIT_NOFX);
    dest->SetRenderOptions (dest, DSRO_NONE);
    dest->Blit (dest, source, NULL, 0, 0);
    
    EGL_RETURN (EGL_SUCCESS, EGL_TRUE);
}

EGLBoolean eglSwapInterval (EGLDisplay dpy, EGLint interval)
{
    EGLDFBContext *ctx = get_egl_dfb_context();

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );

    if (!dpy)
        EGL_RETURN (EGL_BAD_DISPLAY, EGL_FALSE);
        
    EGL_RETURN (EGL_SUCCESS, EGL_TRUE);
}

EGLBoolean eglReleaseThread (void)
{
    EGLDFBContext *ctx = get_egl_dfb_context();

    D_DEBUG_AT( EGL_DFB, "%s()\n", __FUNCTION__ );

    EGL_RETURN (EGL_SUCCESS, EGL_TRUE);
}

