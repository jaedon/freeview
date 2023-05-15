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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>

#include <direct/clock.h>
#include <direct/log.h>

#include <GLES/gl.h>
#include <EGL/egl.h>


static void
triangle( void )
{
     float vertices[] = {
          0, 0, 1,
          0, 8, 1,
          8, 8, 1,
     };

     float color[] = {
          1.0f, 1.0f, 1.0f, 1.0f,
          1.0f, 0.0f, 1.0f, 1.0f,
          0.0f, 0.0f, 1.0f, 1.0f
     };

     glDisable( GL_CULL_FACE );
     glDisable( GL_DEPTH_TEST );
     glDisable( GL_TEXTURE_2D );
     glDisable( GL_LIGHTING );

     glShadeModel( GL_SMOOTH );

     glEnableClientState( GL_VERTEX_ARRAY );
     glEnableClientState( GL_COLOR_ARRAY );

     glVertexPointer( 3, GL_FLOAT, sizeof (float) * 3, vertices );
     glColorPointer( 4, GL_FLOAT, sizeof (float) * 4, color );

     glDrawArrays( GL_TRIANGLES, 0, 3 );
}

void *main_test(void *arg)
{
     EGLDisplay display;
     EGLContext context;
     EGLSurface surface;
     int        screen_width, screen_height;

     (void)arg;

     /*
      * EGL Initialization
      */
     display = eglGetDisplay( NULL );

     context = eglCreateContext( display, NULL, NULL, 0 );

     /* Let EGL create a new fullscreen EGLSurface */
     surface = eglCreateWindowSurface( display, NULL, NULL, NULL );

     eglQuerySurface( display, surface, EGL_WIDTH, &screen_width );
     eglQuerySurface( display, surface, EGL_HEIGHT, &screen_height );

     eglMakeCurrent( display, surface, surface, context );


     /*
      * View
      */
     GLfloat h = (GLfloat) screen_height / (GLfloat) screen_width;

     glViewport(0, 0, (GLint) screen_width, (GLint) screen_height);
     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();
     glFrustumf(-1.0, 1.0, -h, h, 5.0, 60.0);
     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
     glTranslatef(0.0, 0.0, -40.0);


     /*
      * Main Loop
      */
     int       quit      = 0;
     long long T0        = 0;
     GLint     Frames    = 0;
     GLfloat   fps       = 0;
     GLfloat   view_rotx = 20.0, view_roty = 30.0, view_rotz = 0.0;
     GLfloat   angle     = 0.0;

     T0 = direct_clock_get_millis();


     while (!quit) {
          long long t;

          glClear(GL_COLOR_BUFFER_BIT);

          glPushMatrix();
          glRotatef(view_rotx, 1.0, 0.0, 0.0);
          glRotatef(view_roty, 0.0, 1.0, 0.0);
          glRotatef(view_rotz, 0.0, 0.0, 1.0);

          glPushMatrix();
          glTranslatef(-3.0, -2.0, 0.0);
          glRotatef(angle, 0.0, 0.0, 1.0);
          triangle();
          glPopMatrix();

          glPushMatrix();
          glTranslatef(3.1, -2.0, 0.0);
          glRotatef(-2.0 * angle - 9.0, 0.0, 0.0, 1.0);
          triangle();
          glPopMatrix();

          glPushMatrix();
          glTranslatef(-3.1, 4.2, 0.0);
          glRotatef(-2.0 * angle - 25.0, 0.0, 0.0, 1.0);
          triangle();
          glPopMatrix();

          glPopMatrix();

          glFinish();

          eglSwapBuffers( display, surface );

          Frames++;


          t = direct_clock_get_millis();
          if (t - T0 >= 5000) {
               GLfloat seconds = (t - T0) / 1000.0;

               fps = Frames / seconds;

               printf("%4.1f FPS\n", fps);

               T0 = t;
               Frames = 0;
          }

          angle += 2.0;

          view_rotx += 0.1;
          view_roty += 0.1;
          view_rotz += 0.1;
     }
     glFinish();

     return NULL;
}

int
main( int argc, char *argv[] )
{
     GLint i;
     pthread_t egl_thread;

     for ( i=1; i<argc; i++ ) {
          if (strcmp(argv[i], "-info")==0) {
               printf("GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
               printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
               printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
               printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));
          }
     }

     pthread_create(&egl_thread, NULL, main_test, NULL);
     pthread_join(egl_thread, NULL);

     return 0;
}

