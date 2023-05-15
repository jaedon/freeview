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

#include <config.h>

#include <stdarg.h>

#include <dlfcn.h>

#include <directfbgl.h>

#include <direct/debug.h>

#include <core/surface.h>

#include <display/idirectfbsurface.h>

#include "gl/gl_private.h"

#include "bcmnexus.h"

D_DEBUG_DOMAIN( IDFBGL_BCM, "IDirectFBGL/BCM", "IDirectFBGL BCM Implementation" );

static DFBResult
Probe( void *ctx, ... );

static DFBResult
Construct( void *interface, ... );

#include <direct/interface_implementation.h>

DIRECT_INTERFACE_IMPLEMENTATION( IDirectFBGL, BCM )

/*
 * private data struct of IDirectFBGL
 */
typedef struct {
     int                      ref;       /* reference counter */

     CoreDFB                 *core;

     IDirectFBSurface        *surface;
     CoreSurface             *core_surface;

     DFBGLAttributes          attributes;


     bool                     locked;


     DFBRectangle             view;
     DFBRegion                clip;


     p3dOpenGLContextData     context;
} IDirectFBGL_data;


static void
IDirectFBGL_Destruct( IDirectFBGL *thiz )
{
     IDirectFBGL_data *data = thiz->priv;

     if (data->locked)
          dfb_surface_unlock_buffer( data->core_surface, &data->context.dst );

     data->surface->Release( data->surface );

     DIRECT_DEALLOCATE_INTERFACE( thiz );
}

static DirectResult
IDirectFBGL_AddRef( IDirectFBGL *thiz )
{
     DIRECT_INTERFACE_GET_DATA (IDirectFBGL);

     data->ref++;

     return DFB_OK;
}

static DirectResult
IDirectFBGL_Release( IDirectFBGL *thiz )
{
     DIRECT_INTERFACE_GET_DATA (IDirectFBGL)

     if (--data->ref == 0)
          IDirectFBGL_Destruct( thiz );

     return DFB_OK;
}

static DFBResult
IDirectFBGL_Lock( IDirectFBGL *thiz )
{
     DFBResult             ret;
     p3dOpenGLContextData *p3dctx;
     CoreSurface          *surface;
     BSUR_Surface_Handle   dst_handle = NULL;
     BSUR_Surface_Handle   depth_handle = NULL;

     DIRECT_INTERFACE_GET_DATA (IDirectFBGL);

     D_DEBUG_AT( IDFBGL_BCM, "%s()\n", __FUNCTION__ );

     if (data->locked)
          return DFB_LOCKED;

     p3dctx  = &data->context;

     surface = data->core_surface;
     D_ASSERT( surface != NULL );

     ret = dfb_surface_lock_buffer( data->core_surface, CSBR_BACK, CSAID_GPU, CSAF_READ | CSAF_WRITE, &p3dctx->dst );
     if (ret) {
          D_DERROR( ret, "IDirectFBGL/BCM: Could not lock destination buffer!\n" );
          return ret;
     }

     p3dctx->size   = data->core_surface->config.size;
     p3dctx->format = data->core_surface->config.format;


     D_DEBUG_AT( IDFBGL_BCM, "  -> locked Nexus Surface: %p\n", p3dctx->dst.handle );


     GL_P_SetContext( p3dctx );

     BP3D_Context_SetCurrent( p3dctx->hP3d, p3dctx->hContext );

     DFB_Platform_P_Graphics3D_GetBSUR(p3dctx->dst.handle, &dst_handle);
     if (p3dctx->depth.handle != NULL)
     {
         DFB_Platform_P_Graphics3D_GetBSUR(p3dctx->depth.handle, &depth_handle);
     }

     BP3D_SetDstColorSurface( p3dctx->hP3d, dst_handle );
     BP3D_SetSrcColorSurface( p3dctx->hP3d, dst_handle );
     BP3D_SetDstDepthSurface( p3dctx->hP3d, depth_handle );
     BP3D_SetSrcDepthSurface( p3dctx->hP3d, depth_handle );

     if( !p3dctx->bInitialized )
     {
          GLfloat afZeroColor[4] = { 0.0, 0.0, 0.0, 0.0 };
          GLfloat afFullColor[4] = { 1.0, 1.0, 1.0, 1.0 };
          GLfloat afFullAlpha[4] = { 0.0, 0.0, 0.0, 1.0 };
          GLfloat afAmbColor[4]  = { 0.2, 0.2, 0.2, 1.0 };
          GLfloat afDifColor[4]  = { 0.8, 0.8, 0.8, 1.0 };
          GLfloat afPosition[4]  = { 0.0, 0.0, 1.0, 0.0 };
          GLfloat afDirection[3] = { 0.0, 0.0,-1.0 };
          GLuint uiTextureID;
          GLint ii;

          p3dctx->ModelViewMatrixStack.uiTop = GL_P_MATRIX_STACK_DEPTH - 1;
          p3dctx->ProjectionMatrixStack.uiTop = GL_P_MATRIX_STACK_DEPTH - 1;
          p3dctx->TextureMatrixStack.uiTop = GL_P_MATRIX_STACK_DEPTH - 1;

          glActiveTexture( GL_TEXTURE0 );
          glAlphaFunc( GL_ALWAYS, 0 );
          glBlendFunc( GL_ONE, GL_ZERO );
          glClearColor( 0.0, 0.0, 0.0, 0.0 );
          glClearDepthf( 1.0 );
          glClearStencil( 0 );
          glColor4f( 1.0, 1.0, 1.0, 1.0 );
          glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
          glCullFace( GL_BACK );
          glDepthFunc( GL_LESS );
          glDepthMask( GL_TRUE );
          glDepthRangef( 0.0, 1.0 );
          glFogf( GL_FOG_MODE, GL_EXP );
          glFogf( GL_FOG_DENSITY, 1.0 );
          glFogf( GL_FOG_START, 0.0 );
          glFogf( GL_FOG_END, 1.0 );
          glFogfv( GL_FOG_COLOR, afZeroColor );
          glFrontFace( GL_CCW );
          glLightModelf( GL_LIGHT_MODEL_TWO_SIDE, 0 );
          glLightModelfv( GL_LIGHT_MODEL_AMBIENT, afAmbColor );
          glLineWidth( 1.0 );
          glLogicOp( GL_COPY );
          glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 0.0 );
          glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, afAmbColor );
          glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, afDifColor );
          glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, afFullAlpha );
          glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, afFullAlpha );
          glMultiTexCoord4f( GL_TEXTURE0, 0.0, 0.0, 0.0, 1.0 );
          glNormal3f( 0.0, 0.0, 1.0 );
          glPointSize( 1.0 );
          glPolygonOffset( 0.0, 0.0 );
          glSampleCoverage( 1.0, GL_FALSE );
          glShadeModel( GL_SMOOTH );
          glStencilFunc( GL_ALWAYS, 0, 0xFFFFFFFF );
          glStencilMask( 0xFFFFFFFF );
          glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
//      glViewport( 0, 0, p3dctx->size.w, p3dctx->size.h );

          glGenTextures( 1, &uiTextureID );
          glBindTexture( GL_TEXTURE_2D, uiTextureID );
          glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
          glTexEnvfv( GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, afZeroColor );
          glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR );
          glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
          glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
          glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

          glMatrixMode( GL_TEXTURE );
          glLoadIdentity();
          glMatrixMode( GL_PROJECTION );
          glLoadIdentity();
          glMatrixMode( GL_MODELVIEW );
          glLoadIdentity();

          for( ii = 0; ii < GL_P_LIGHT_MAX; ++ii )
          {
               glDisable( GL_LIGHT0 + ii );
               glLightf( GL_LIGHT0 + ii, GL_SPOT_EXPONENT, 0.0 );
               glLightf( GL_LIGHT0 + ii, GL_SPOT_CUTOFF, 180.0 );
               glLightf( GL_LIGHT0 + ii, GL_CONSTANT_ATTENUATION, 1.0 );
               glLightf( GL_LIGHT0 + ii, GL_LINEAR_ATTENUATION, 0.0 );
               glLightf( GL_LIGHT0 + ii, GL_QUADRATIC_ATTENUATION, 0.0 );
               glLightfv( GL_LIGHT0 + ii, GL_AMBIENT, afFullAlpha );
               glLightfv( GL_LIGHT0 + ii, GL_DIFFUSE, ii ? afZeroColor : afFullColor );
               glLightfv( GL_LIGHT0 + ii, GL_SPECULAR, ii ? afZeroColor : afFullColor );
               glLightfv( GL_LIGHT0 + ii, GL_POSITION, afPosition );
               glLightfv( GL_LIGHT0 + ii, GL_SPOT_DIRECTION, afDirection );
          }

          glDisableClientState( GL_COLOR_ARRAY );
          glDisableClientState( GL_NORMAL_ARRAY );
          glDisableClientState( GL_TEXTURE_COORD_ARRAY );
          glDisableClientState( GL_VERTEX_ARRAY );

          glDisable( GL_ALPHA_TEST );
          glDisable( GL_BLEND );
          glDisable( GL_COLOR_LOGIC_OP );
          glDisable( GL_COLOR_MATERIAL );
          glDisable( GL_CULL_FACE );
          glDisable( GL_DEPTH_TEST );
          glDisable( GL_FOG );
          glDisable( GL_LIGHTING );
          glDisable( GL_LINE_SMOOTH );
          glDisable( GL_MULTISAMPLE );
          glDisable( GL_NORMALIZE );
          glDisable( GL_POINT_SMOOTH );
          glDisable( GL_POLYGON_OFFSET_FILL );
          glDisable( GL_RESCALE_NORMAL );
          glDisable( GL_SAMPLE_ALPHA_TO_COVERAGE );
          glDisable( GL_SAMPLE_ALPHA_TO_ONE );
          glDisable( GL_SAMPLE_COVERAGE );
          glDisable( GL_SCISSOR_TEST );
          glDisable( GL_STENCIL_TEST );
          glDisable( GL_TEXTURE_2D );

          glEnable( GL_DITHER );


          p3dctx->bInitialized = GL_TRUE;
     }


     D_DEBUG_AT( IDFBGL_BCM, "  -> glViewport( %d, %d, %d, %d )\n",
                 data->view.x, p3dctx->size.h - data->view.y - data->view.h, data->view.w, data->view.h );

     glViewport( data->view.x, p3dctx->size.h - data->view.y - data->view.h, data->view.w, data->view.h );


     D_DEBUG_AT( IDFBGL_BCM, "  -> glScissor( %d, %d, %d, %d )\n",
                 data->clip.x1,
                 p3dctx->size.h - data->clip.y2 - 1,
                 data->clip.x2 - data->clip.x1 + 1,
                 data->clip.y2 - data->clip.y1 + 1 );

     glScissor( data->clip.x1,
                p3dctx->size.h - data->clip.y2 - 1,
                data->clip.x2 - data->clip.x1 + 1,
                data->clip.y2 - data->clip.y1 + 1 );

     data->locked = true;

     return DFB_OK;
}

static DFBResult
IDirectFBGL_Unlock( IDirectFBGL *thiz )
{
     DIRECT_INTERFACE_GET_DATA (IDirectFBGL);

     D_DEBUG_AT( IDFBGL_BCM, "%s()\n", __FUNCTION__ );

     if (!data->locked)
          return DFB_BUFFEREMPTY;

     dfb_surface_unlock_buffer( data->core_surface, &data->context.dst );

     data->locked = false;

     return DFB_OK;
}

static DFBResult
IDirectFBGL_GetAttributes( IDirectFBGL     *thiz,
                           DFBGLAttributes *attributes )
{
     DIRECT_INTERFACE_GET_DATA (IDirectFBGL);

     D_DEBUG_AT( IDFBGL_BCM, "%s()\n", __FUNCTION__ );

     if (!attributes)
          return DFB_INVARG;

     *attributes = data->attributes;

     return DFB_OK;
}

static DFBResult
IDirectFBGL_GetProcAddress( IDirectFBGL  *thiz,
                            const char   *name,
                            void        **ret_address )
{
     void *handle;

     DIRECT_INTERFACE_GET_DATA (IDirectFBGL);

     D_DEBUG_AT( IDFBGL_BCM, "%s()\n", __FUNCTION__ );

     if (!name)
          return DFB_INVARG;

     if (!ret_address)
          return DFB_INVARG;

     handle = dlopen( NULL, RTLD_LAZY );
     if (!handle)
          return DFB_FAILURE;

     *ret_address = dlsym( handle, name );

     dlclose( handle );

     return (*ret_address) ? DFB_OK : DFB_UNSUPPORTED;
}

static DFBResult
IDirectFBGL_TextureSurface( IDirectFBGL      *thiz,
                            int               target,
                            int               level,
                            IDirectFBSurface *surface )
{
     int                    format;
     IDirectFBSurface_data *surface_data;
     CoreSurface           *core_surface;

     DIRECT_INTERFACE_GET_DATA (IDirectFBGL);

     D_DEBUG_AT( IDFBGL_BCM, "%s( %d, %d, %p )\n", __FUNCTION__, target, level, (void *)surface );

     if (!target)
          return DFB_INVARG;

     if (target != GL_TEXTURE_2D)
          return DFB_UNSUPPORTED;

     if (level)
          return DFB_UNSUPPORTED;

     if (!data->locked)
          return DFB_UNIMPLEMENTED;

     DIRECT_INTERFACE_GET_DATA_FROM (surface, surface_data, IDirectFBSurface);

     core_surface = surface_data->surface;
     if (!core_surface)
          return DFB_DESTROYED;

     switch (core_surface->config.format) {
          case DSPF_ARGB:
               format = GL_RGBA;
               break;

          case DSPF_RGB32:
               format = GL_RGB;
               break;

          default:
               D_UNIMPLEMENTED();
               return DFB_UNIMPLEMENTED;
     }

     glTexImage2D( GL_TEXTURE_2D_SURFACE_DIRECTFB, level, format,
                   core_surface->config.size.w, core_surface->config.size.h,
                   0, format, GL_UNSIGNED_BYTE, core_surface );

     return DFB_OK;
}

/* exported symbols */

static DFBResult
Probe( void *ctx, ... )
{
     IDirectFBSurface       *surface = ctx;
     IDirectFBSurface_data  *surface_data;
     DFBSurfaceCapabilities  caps;

     D_DEBUG_AT( IDFBGL_BCM, "%s()\n", __FUNCTION__ );

     if (!surface)
          return DFB_UNSUPPORTED;

     surface->GetCapabilities( surface, &caps );

     if (caps & DSCAPS_SYSTEMONLY)
          return DFB_UNSUPPORTED;

     surface_data = surface->priv;
     if (!surface_data)
          return DFB_DEAD;

     /* ... */

     return DFB_OK;
}

static DFBResult
Construct( void *interface, ... )
{
     DFBResult              ret;
     IDirectFBGL           *thiz = interface;
     CoreDFB               *core;
     IDirectFBSurface      *surface;
     IDirectFBSurface_data *surface_data;
     CoreSurface           *core_surface;

     D_DEBUG_AT( IDFBGL_BCM, "%s()\n", __FUNCTION__ );


     va_list tag;
     va_start(tag, interface);
     surface = va_arg(tag, IDirectFBSurface*);
     core = va_arg(tag, CoreDFB *);
     va_end( tag );

     surface_data = surface->priv;
     if (!surface_data) {
          DIRECT_DEALLOCATE_INTERFACE( thiz );
          return DFB_DEAD;
     }

     core_surface = surface_data->surface;
     if (!core_surface) {
          DIRECT_DEALLOCATE_INTERFACE( thiz );
          return DFB_DESTROYED;
     }

     D_DEBUG_AT( IDFBGL_BCM, "  -> increasing surface ref count...\n" );

     /* Increase target reference counter. */
     ret = surface->AddRef( surface );
     if (ret) {
          DIRECT_DEALLOCATE_INTERFACE( thiz );
          return ret;
     }

     /* Allocate interface data. */
     DIRECT_ALLOCATE_INTERFACE_DATA( thiz, IDirectFBGL );

     /* Initialize interface data. */
     data->core         = core;
     data->ref          = 1;
     data->surface      = surface;
     data->core_surface = core_surface;

     /*
      * Setup the OpenGL state
      */
     p3dOpenGLContextData *p3dctx = &data->context;

     D_DEBUG_AT( IDFBGL_BCM, "  -> getting 3D handle...\n" );

     p3dctx->hP3d = DFB_Platform_P_Graphics3D_GetP3D();

     if (!p3dctx->hP3d) {
          D_ERROR( "Broadcom/P3D: Could not get 3D handle!\n" );
          surface->Release( surface );
          DIRECT_DEALLOCATE_INTERFACE( thiz );
          return DFB_INIT;
     }

     D_DEBUG_AT( IDFBGL_BCM, "  -> getting memory handle...\n" );

     p3dctx->hMem = DFB_Platform_P_Graphics3D_GetBMEM();

     D_DEBUG_AT( IDFBGL_BCM, "  -> creating BP3D context...\n" );

     ret = BP3D_Context_Create( p3dctx->hP3d, &p3dctx->hContext );
     if (ret) {
          D_ERROR( "P3DGL/Impl: BP3D_Context_Create() failed with result 0x%04x\n", ret );
          surface->Release( surface );
          DIRECT_DEALLOCATE_INTERFACE( thiz );
          return DFB_INIT;
     }

     p3dctx->uiFreeTexCount              = GL_P_TEXTURE_MAX;
     p3dctx->ModelViewMatrixStack.uiTop  = GL_P_MATRIX_STACK_DEPTH - 1;
     p3dctx->ProjectionMatrixStack.uiTop = GL_P_MATRIX_STACK_DEPTH - 1;
     p3dctx->TextureMatrixStack.uiTop    = GL_P_MATRIX_STACK_DEPTH - 1;


     data->view = surface_data->area.wanted;
     data->clip = DFB_REGION_INIT_FROM_RECTANGLE_VALS( surface_data->area.current.x - surface_data->area.wanted.x,
                                                       surface_data->area.current.y - surface_data->area.wanted.y,
                                                       surface_data->area.current.w, surface_data->area.current.h );


     /* Handle this on our own. */
     data->attributes.double_buffer = !!(core_surface->config.caps & DSCAPS_FLIPPING);
     data->attributes.buffer_size   = DFB_BITS_PER_PIXEL( core_surface->config.format );
     data->attributes.alpha_size    = DFB_ALPHA_BITS_PER_PIXEL( core_surface->config.format );

     switch (core_surface->config.format) {
          case DSPF_ARGB:
          case DSPF_RGB32:
               data->attributes.red_size   = 8;
               data->attributes.green_size = 8;
               data->attributes.blue_size  = 8;
               break;

          default:
               D_UNIMPLEMENTED();
     }

     /* Assign interface pointers. */
     thiz->AddRef         = IDirectFBGL_AddRef;
     thiz->Release        = IDirectFBGL_Release;
     thiz->Lock           = IDirectFBGL_Lock;
     thiz->Unlock         = IDirectFBGL_Unlock;
     thiz->GetAttributes  = IDirectFBGL_GetAttributes;
     thiz->GetProcAddress = IDirectFBGL_GetProcAddress;
     thiz->TextureSurface = IDirectFBGL_TextureSurface;

     return DFB_OK;
}

