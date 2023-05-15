/*
   (c) Copyright 2001  convergence integrated media GmbH.
   All rights reserved.

   Written by Denis Oliver Kropp <dok@convergence.de> and
              Andreas Hundt <andi@convergence.de>.

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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <misc/conf.h>

#include <direct/thread.h>

#include <GLES/gl.h>

#include <directfb.h>
#include <directfbgl.h>


//extern int df_andi_main( int argc, char *argv[] );


/* the super interface */
IDirectFB *dfb;

/* the primary surface (surface of primary layer) */
IDirectFBSurface *primary;

/* the GL context */
IDirectFBGL *primary_gl;

/* our font */
IDirectFBFont *font;

/* event buffer */
IDirectFBEventBuffer *events;

/* macro for a safe call to DirectFB functions */
#define DFBCHECK(x) \
        {                                                                      \
           err = x;                                                            \
           if (err != DFB_OK) {                                                \
              fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ );           \
              DirectFBErrorFatal( #x, err );                                   \
           }                                                                   \
        }

static int screen_width, screen_height;

static long long T0 = 0;
static GLint Frames = 0;
static GLfloat fps = 0;
static GLuint texture_id = 0;


static void
rectangle( void )
{
     float vertices[] = {
          0, 0, 1,
          0, 5, 1,
          5, 5, 1,
          5, 0, 1,
     };

     float texcoords[] = {
          0, 1,
          0, 0,
          1, 0,
          1, 1,
     };

     float color[] = {
          1.0f, 1.0f, 1.0f, 1.0f,
          1.0f, 1.0f, 1.0f, 1.0f,
          1.0f, 1.0f, 1.0f, 1.0f,
          1.0f, 1.0f, 1.0f, 1.0f,
     };

     glShadeModel( GL_FLAT );

     glDisable( GL_CULL_FACE );
     glDisable( GL_LIGHTING );
     glDisable( GL_DEPTH_TEST );

     glEnable( GL_TEXTURE_2D );

     glEnableClientState( GL_VERTEX_ARRAY );
     glEnableClientState( GL_COLOR_ARRAY );
     glEnableClientState( GL_TEXTURE_COORD_ARRAY );

     glVertexPointer( 3, GL_FLOAT, sizeof (float) * 3, vertices );
     glColorPointer( 4, GL_FLOAT, sizeof (float) * 4, color );
     glTexCoordPointer( 2, GL_FLOAT, sizeof (float) * 2, texcoords );

     glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
}

static GLfloat view_rotx = 20.0, view_roty = 30.0, view_rotz = 0.0;
static GLfloat inc_rotx = 0, inc_roty = 0, inc_rotz = 0;
static GLfloat angle = 0.0;

static void
draw( void )
{
     glClear(GL_COLOR_BUFFER_BIT);

     glPushMatrix();
     glRotatef(view_rotx, 1.0, 0.0, 0.0);
     glRotatef(view_roty, 0.0, 1.0, 0.0);
     glRotatef(view_rotz, 0.0, 0.0, 1.0);

     glPushMatrix();
     glTranslatef(-3.0, -2.0, 0.0);
     glRotatef(angle, 0.0, 0.0, 1.0);
     rectangle();
     glPopMatrix();

     glPushMatrix();
     glTranslatef(3.1, -2.0, 0.0);
     glRotatef(-2.0 * angle - 9.0, 0.0, 0.0, 1.0);
     rectangle();
     glPopMatrix();

     glPushMatrix();
     glTranslatef(-3.1, 4.2, 0.0);
     glRotatef(-2.0 * angle - 25.0, 0.0, 0.0, 1.0);
     rectangle();
     glPopMatrix();

     glPopMatrix();
}

/* new window size or exposure */
static void
reshape(int width, int height)
{
     GLfloat h = (GLfloat) height / (GLfloat) width;

     glViewport(0, 0, (GLint) width, (GLint) height);
     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();
     glFrustumf(-1.0, 1.0, -h, h, 5.0, 60.0);
     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
     glTranslatef(-1.0, -1.0, -40.0);
}

static void
init(int argc, char *argv[])
{
     GLint i;

     for ( i=1; i<argc; i++ ) {
          if (strcmp(argv[i], "-info")==0) {
               printf("GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
               printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
               printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
               printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));
          }
     }
}

static void //DFBSurfaceID
init_texture( void )
{
     DFBResult              err;
     DFBSurfaceDescription  desc;
//     DFBSurfaceID           surface_id;
     IDirectFBSurface      *surface;

     glGenTextures( 1, &texture_id );
     glBindTexture( GL_TEXTURE_2D, texture_id );

     glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
     glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

     glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
     glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );


     desc.flags       = DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT | DSDESC_CAPS;
     desc.width       = 512;
     desc.height      = 512;
     desc.pixelformat = DSPF_ARGB;
     desc.caps        = DSCAPS_NONE; //DOUBLE;

     DFBCHECK(dfb->CreateSurface( dfb, &desc, &surface ));


     surface->Clear( surface, 0xff, 0xff, 0xff, 0xff );

     surface->SetColor( surface, 0x00, 0x00, 0xff, 0xff );
     surface->FillRectangle( surface, 10, 10, 10, 10 );

     surface->SetColor( surface, 0x00, 0xff, 0x00, 0xff );
     surface->FillRectangle( surface, 50, 50, 200, 200 );


     primary_gl->TextureSurface( primary_gl, GL_TEXTURE_2D, 0, surface );


//     surface->GetID( surface, &surface_id );

//     printf( ">>>>>>>  Surface ID = %u\n", surface_id );


     surface->Release( surface );

//     return surface_id;

     dfb->WaitIdle( dfb );
}

/*
static void
df_andi_thread( DirectThread *thread, void *ctx )
{
     (void)ctx;
     (void)thread;
     char *argv[] = { "df_andi", NULL };

     df_andi_main( 1, argv );
}
*/

extern void platform_init(void);

int
main( int argc, char *argv[] )
{
     int quit = 0;
     DFBResult err;
     DFBSurfaceDescription dsc;
//     DFBSurfaceID          surface_id;

     DFBCHECK(DirectFBInit( &argc, &argv ));

     /* create the super interface */
     DFBCHECK(DirectFBCreate( &dfb ));

     /* create an event buffer for all devices with these caps */
     DFBCHECK(dfb->CreateInputEventBuffer( dfb, DICAPS_KEYS | DICAPS_AXES,
                                           DFB_FALSE, &events ));

     /* set our cooperative level to DFSCL_FULLSCREEN
        for exclusive access to the primary layer */
     dfb->SetCooperativeLevel( dfb, DFSCL_FULLSCREEN );

     /* get the primary surface, i.e. the surface of the
        primary layer we have exclusive access to */
     dsc.flags = DSDESC_CAPS;
     dsc.caps  = DSCAPS_PRIMARY | DSCAPS_DOUBLE;

     DFBCHECK(dfb->CreateSurface( dfb, &dsc, &primary ));

     /* get the size of the surface and fill it */
     DFBCHECK(primary->GetSize( primary, &screen_width, &screen_height ));

     primary->Clear( primary, 0, 0, 0, 0 );
     primary->Flip( primary, NULL, 0 );

     /* load font */
     {
          DFBFontDescription desc;

          desc.flags = DFDESC_HEIGHT;
          desc.height = 24;

          /* create the default font and set it */
          DFBCHECK(dfb->CreateFont( dfb, FONT, &desc, &font ));
          DFBCHECK(primary->SetFont( primary, font ));
     }

     /* get the GL context */
#if 0
     DFBRectangle rect = {
          100, 100, 100, 100
     };
     IDirectFBSurface *sub;
     primary->GetSubSurface( primary, &rect, &sub );
     DFBCHECK(sub->GetGL( sub, &primary_gl ));
#else
     DFBCHECK(primary->GetGL( primary, &primary_gl ));
#endif

     DFBCHECK(primary_gl->Lock( primary_gl ));

     init(argc, argv);
     reshape(screen_width, screen_height);

     /*surface_id =*/ init_texture();

     DFBCHECK(primary_gl->Unlock( primary_gl ));



//     DirectThread *df_andi;

//     dfb_config->primary_id = surface_id;

//     df_andi = direct_thread_create( DTT_DEFAULT, df_andi_thread, NULL, "df_andi" );


     long long t0 = T0 = direct_clock_get_millis();

     while (!quit) {
          DFBInputEvent evt;
          long long t;
//          int i=0;

//          for (i=0; i<200; i++) {

//               primary->Clear( primary, i, i, i, i );
//          }
//          usleep(100000);

          DFBCHECK(primary_gl->Lock( primary_gl ));

          draw();

          DFBCHECK(primary_gl->Unlock( primary_gl ));

          if (fps) {
               char buf[64];

               snprintf(buf, 64, "%4.1f FPS", fps);

               primary->SetColor( primary, 0xff, 0, 0, 0xff );
//               primary->DrawString( primary, buf, -1, screen_width - 5, 5, DSTF_TOPRIGHT );
          }

          glFinish();
//          usleep(5000000);

          primary->Flip( primary, NULL, 0 );
//          usleep(100000);
          Frames++;

          //primary->Dump( primary, ".", "dfbtest_gl" );


          t = direct_clock_get_millis();
          if (t - T0 >= 5000) {
               GLfloat seconds = (t - T0) / 1000.0;

               fps = Frames / seconds;

               printf("%4.1f FPS\n", fps);
               //fps = 0;

               T0 = t;
               Frames = 0;
          }


          while (events->GetEvent( events, DFB_EVENT(&evt) ) == DFB_OK) {
               switch (evt.type) {
                    case DIET_KEYPRESS:
                         switch (evt.key_symbol) {
                              case DIKS_ESCAPE:
                                   quit = 1;
                                   break;
                              case DIKS_CURSOR_UP:
                                   inc_rotx = 5.0;
                                   break;
                              case DIKS_CURSOR_DOWN:
                                   inc_rotx = -5.0;
                                   break;
                              case DIKS_CURSOR_LEFT:
                                   inc_roty = 5.0;
                                   break;
                              case DIKS_CURSOR_RIGHT:
                                   inc_roty = -5.0;
                                   break;
                              case DIKS_PAGE_UP:
                                   inc_rotz = 5.0;
                                   break;
                              case DIKS_PAGE_DOWN:
                                   inc_rotz = -5.0;
                                   break;
                              default:
                                   ;
                         }
                         break;
                    case DIET_KEYRELEASE:
                         switch (evt.key_symbol) {
                              case DIKS_CURSOR_UP:
                                   inc_rotx = 0;
                                   break;
                              case DIKS_CURSOR_DOWN:
                                   inc_rotx = 0;
                                   break;
                              case DIKS_CURSOR_LEFT:
                                   inc_roty = 0;
                                   break;
                              case DIKS_CURSOR_RIGHT:
                                   inc_roty = 0;
                                   break;
                              case DIKS_PAGE_UP:
                                   inc_rotz = 0;
                                   break;
                              case DIKS_PAGE_DOWN:
                                   inc_rotz = 0;
                                   break;
                              default:
                                   ;
                         }
                         break;
                    case DIET_AXISMOTION:
                         if (evt.flags & DIEF_AXISREL) {
                              switch (evt.axis) {
                                   case DIAI_X:
                                        view_roty += evt.axisrel / 2.0;
                                        break;
                                   case DIAI_Y:
                                        view_rotx += evt.axisrel / 2.0;
                                        break;
                                   case DIAI_Z:
                                        view_rotz += evt.axisrel / 2.0;
                                        break;
                                   default:
                                        ;
                              }
                         }
                         break;
                    default:
                         ;
               }
          }


//          inc_rotx = (t - t0) / 10.0f;
//          inc_roty = (t - t0) / 13.0f;
          inc_rotz = (t - t0) / 17.0f;

          t0 = t;


          angle += 0.01;

          view_rotx += inc_rotx;
          view_roty += inc_roty;
          view_rotz += inc_rotz;
     }

     /* release our interfaces to shutdown DirectFB */
     primary_gl->Release( primary_gl );
     primary->Release( primary );
     font->Release( font );
     events->Release( events );
     dfb->Release( dfb );

     return 0;
}

