/******************************************************************************
 *    (c)2008-2010 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: cube_composited.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 3/17/11 4:23p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/applications/opengles_v3d/v3d/nexus/cube_composited/cube_composited.c $
 * 
 * Hydra_Software_Devel/2   3/17/11 4:23p erickson
 * SWVC4-133: merge
 * 
 * hauxwell_35230_v3d/5   12/14/10 3:50p gsweet
 * Correctly handle finish display signal
 * 
 * hauxwell_35230_v3d/4   12/13/10 10:26a gsweet
 * Fixed runtime warnings when display is larger than NTSC
 * 
 * hauxwell_35230_v3d/3   12/10/10 11:02a hills
 * Fixed compile errors on 35230
 * 
 * hauxwell_35230_v3d/2   12/10/10 10:22a gsweet
 * Updating cube_composited example for optimal performance using the
 * abstract display API. Added pixmap based example.
 * 
 * hauxwell_35230_v3d/1   12/6/10 12:53p hauxwell
 * SWVC4-101: organizing examples
 * 
 * hauxwell_35230_v3d/1   12/6/10 12:23p gsweet
 * SWVC4-101: Allow abstract platform support for V3D driver.
 * 
 * hauxwell_35230_v3d/1   12/6/10 12:07p gsweet
 * SWVC4-101: Allow abstract platform support for V3D driver.
 * 
 * hauxwell_35230_v3d/10   11/19/10 3:04p gsweet
 * Changed 3D examples to default to 720x480 resolution
 * 
 * hauxwell_35230_v3d/9   11/3/10 3:42p hauxwell
 * add 7422 support
 * 
 * hauxwell_35230_v3d/8   10/19/10 3:02p hauxwell
 * update to use BRCM_RegisterDisplay() API
 * 
 * hauxwell_35230_v3d/7   10/12/10 1:26p hauxwell
 * update so frame divider doesn't half the max framerate
 * 
 * hauxwell_35230_v3d/6   10/8/10 12:01p hauxwell
 * add 60Hz
 * 
 * hauxwell_35230_v3d/5   10/1/10 1:28p gsweet
 * Added heap arg when destroying pixmap surface
 * 
 * hauxwell_35230_v3d/4   9/27/10 9:25a gsweet
 * Added discard framebuffer extension for better performance/resource
 * usage.
 * 
 * hauxwell_35230_v3d/3   9/21/10 11:58a gsweet
 * Allow BRCM_CreateCompatiblePixmapSurface() to be called when no 3D
 * stack is initialized. SWVC4-35
 * 
 * hauxwell_35230_v3d/2   8/23/10 1:22p gsweet
 * Changed PIXMAP_* to BRCM_PIXMAP_* for pixmap rendering
 * 
 * hauxwell_35230_v3d/1   8/18/10 3:21p gsweet
 * New OpenGLES example
 * 
 * hauxwell_35230_v3d/1   7/19/10 6:02p hauxwell
 * new example
 *
 *****************************************************************************/

#include <malloc.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <errno.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_surface.h"
#include "nexus_graphics2d.h"
#include "bkni.h"

#include "esutil.h"
#include "default_nexus.h"

#include "../common/init.h"

#ifndef SINGLE_PROCESS
#error This example will not work in multi-process mode
#endif

#define WIDTH_2D  1280
#define HEIGHT_2D 720

#define GL_WIDTH  256
#define GL_HEIGHT 256

#define MAX_OPERATIONS 100

#define RED   8
#define GREEN 8
#define BLUE  8
#define ALPHA 8

#define NUM_3D_THREADS 2

typedef struct
{
   pthread_t            thread;
   bool                 done;
   void                 *current_3d_context;
   NEXUS_SurfaceHandle  current_3d_surface;
   void                 *native_window;
   float                px, py;
   float                vx, vy;

   ESMatrix             projection_matrix;
   ESMatrix             modelview_matrix;
   ESMatrix             mvp_matrix;

   GLint                mvp_matrix_loc;
   GLint                position_loc;
   GLint                color_loc;
   GLint                alpha_loc;

   GLint                program_object;

   GLuint               vbo[2];

} ThreedWindowThread;

NEXUS_DisplayHandle     nexus_display;
NEXUS_SurfaceHandle     nexus_framebuffer[2]; /* Double buffering */
NEXUS_Graphics2DHandle  nexus_gfx = 0;
BKNI_EventHandle        blit_fill_event = 0;
BKNI_EventHandle        vsync_event = 0;
BKNI_MutexHandle        context_mutex = 0;
BKNI_MutexHandle        surface_mutex = 0;
void                    *heap = 0;
CompBufferOnDisplayFunc bufferOnDisplayFunc = 0;

NXPL_PlatformHandle     nxpl_handle = 0;

bool                    fillOnTop = false;
ThreedWindowThread      windowThread[NUM_3D_THREADS];

bool                    threedDone = false;

static const GLfloat cube[] = {
   /*          POSITION                            COLOR                */
   1.000000f, 1.000000f, -1.000000f,     1.000000f, 0.000000f, 0.000000f,
   1.000000f, -1.000000f, -1.000000f,    1.000000f, 0.000000f, 0.000000f,
   -1.000000f, -1.000000f, -1.000000f,   1.000000f, 0.000000f, 0.000000f, 
   -1.000000f, 1.000000f, -1.000000f,    1.000000f, 0.000000f, 0.000000f, 

   -1.000000f, -1.000000f, 1.000000f,    1.000000f, 1.000000f, 0.000000f,
   -1.000000f, 1.000000f, 1.000000f,     1.000000f, 1.000000f, 0.000000f,
   -1.000000f, 1.000000f, -1.000000f,    1.000000f, 1.000000f, 0.000000f, 
   -1.000000f, -1.000000f, -1.000000f,   1.000000f, 1.000000f, 0.000000f, 

   1.000000f, -1.000000f, 1.000000f,     0.000000f, 0.000000f, 1.000000f,
   1.000000f, 1.000000f, 1.000001f,      0.000000f, 0.000000f, 1.000000f,
   -1.000000f, -1.000000f, 1.000000f,    0.000000f, 0.000000f, 1.000000f, 
   -1.000000f, 1.000000f, 1.000000f,     0.000000f, 0.000000f, 1.000000f, 

   1.000000f, -1.000000f, -1.000000f,    1.000000f, 0.000000f, 1.000000f,
   1.000000f, 1.000000f, -1.000000f,     1.000000f, 0.000000f, 1.000000f,
   1.000000f, -1.000000f, 1.000000f,     1.000000f, 0.000000f, 1.000000f, 
   1.000000f, 1.000000f, 1.000001f,      1.000000f, 0.000000f, 1.000000f, 

   1.000000f, 1.000000f, -1.000000f,     0.000000f, 1.000000f, 0.000000f,
   -1.000000f, 1.000000f, -1.000000f,    0.000000f, 1.000000f, 0.000000f,
   1.000000f, 1.000000f, 1.000001f,      0.000000f, 1.000000f, 0.000000f, 
   -1.000000f, 1.000000f, 1.000000f,     0.000000f, 1.000000f, 0.000000f, 

   1.000000f, -1.000000f, -1.000000f,    0.000000f, 1.000000f, 1.000000f,
   1.000000f, -1.000000f, 1.000000f,     0.000000f, 1.000000f, 1.000000f,
   -1.000000f, -1.000000f, 1.000000f,    0.000000f, 1.000000f, 1.000000f, 
   -1.000000f, -1.000000f, -1.000000f,   0.000000f, 1.000000f, 1.000000f, 
};

static const GLushort cube_idx[] = {
   0, 1, 2,
   3, 0, 2,
   4, 5, 6,
   7, 4, 6,
   8, 9, 10,
   9, 11, 10,
   12, 13, 14,
   13, 15, 14,
   16, 17, 18,
   17, 19, 18,
   20, 21, 22,
   23, 20, 22,
};

void InitGLState(ThreedWindowThread *window)
{
   /* The shaders */
   const char vShaderStr[] =
      "uniform mat4   u_mvpMatrix;    \n"
      "uniform float  u_alpha;        \n"
      "attribute vec4 a_position;     \n"
      "attribute vec4 a_color;        \n"
      "varying vec4   v_color;        \n"
      "                               \n"
      "void main()                    \n"
      "{                              \n"
      "  gl_Position = u_mvpMatrix *  \n"
      "                a_position;    \n"
      "  v_color = a_color;           \n"
      "  v_color.a = u_alpha;         \n"
      "}                              \n";

   const char fShaderStr[] =
      "precision mediump float;    \n"
      "varying vec4 v_color;       \n"
      "                            \n"
      "void main()                 \n"
      "{                           \n"
      "  gl_FragColor = v_color;   \n"
      "}                           \n";

   GLuint     v, f;
   GLint      ret;
   const char *ff;
   const char *vv;
   char       *p, *q;

   glClearDepthf(1.0f);
   glClearColor(1.0f, 1.0f, 1.0f, 1);  /* White background */

   glEnable(GL_DEPTH_TEST);
   glDisable(GL_CULL_FACE);

   /* Create vertex buffer objects */
   glGenBuffers(2, window->vbo);
   glBindBuffer(GL_ARRAY_BUFFER, window->vbo[0]);
   glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, window->vbo[1]);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_idx), cube_idx, GL_STATIC_DRAW);

   v = glCreateShader(GL_VERTEX_SHADER);
   f = glCreateShader(GL_FRAGMENT_SHADER);

   ff = fShaderStr;
   vv = vShaderStr;
   glShaderSource(v, 1, &vv, NULL);
   glShaderSource(f, 1, &ff, NULL);

   /* Compile the shaders */
   glCompileShader(v);
   
   glGetShaderiv(v, GL_COMPILE_STATUS, &ret);
   if (ret == GL_FALSE)
   {
      glGetShaderiv(v, GL_INFO_LOG_LENGTH, &ret);
      p = (char *)alloca(ret);
      glGetShaderInfoLog(v, ret, NULL, p);
      assert(0);
   }
   glCompileShader(f);
   glGetShaderiv(f, GL_COMPILE_STATUS, &ret);
   if (ret == GL_FALSE)
   {
      glGetShaderiv(f, GL_INFO_LOG_LENGTH, &ret);
      q = (char *)alloca(ret);
      glGetShaderInfoLog(f, ret, NULL, q);
      assert(0);
   }

   window->program_object = glCreateProgram();
   glAttachShader(window->program_object, v);
   glAttachShader(window->program_object, f);

   /* Link the program */
   glLinkProgram(window->program_object);

   /* Get the attribute locations */
   window->position_loc = glGetAttribLocation(window->program_object, "a_position");
   window->color_loc = glGetAttribLocation(window->program_object, "a_color");

   /* Get the uniform locations */
   window->mvp_matrix_loc = glGetUniformLocation(window->program_object, "u_mvpMatrix");
   window->alpha_loc = glGetUniformLocation(window->program_object, "u_alpha");

   esMatrixLoadIdentity(&window->projection_matrix);
   esMatrixLoadIdentity(&window->modelview_matrix);
   esMatrixLoadIdentity(&window->mvp_matrix);
}

void TerminateGLState(ThreedWindowThread *window)
{
   glDeleteProgram(window->program_object);
   glDeleteBuffers(2, window->vbo);
}

void InitGLViewPort(ThreedWindowThread *window, unsigned int width, unsigned int height)
{
   glViewport(0, 0, width, height);

   esPerspective(&window->projection_matrix, 45.0f, (float)width / (float)height, 100, 1000);

   esMatrixLoadIdentity(&window->modelview_matrix);
   esTranslate(&window->modelview_matrix, 0, 0, -500);
   esScale(&window->modelview_matrix, 100, 100, 100);
}

int InitEGL(NativeWindowType egl_win)
{
   EGLDisplay egl_display     = 0;
   EGLSurface egl_surface     = 0;
   EGLContext egl_context     = 0;
   EGLConfig *egl_config;
   EGLint     config_attribs[128];
   EGLint     major_version;
   EGLint     minor_version;
   int        i = 0;
   int        configs;
   EGLint     ctx_attrib_list[3] = {EGL_CONTEXT_CLIENT_VERSION, 0, EGL_NONE};

   ctx_attrib_list[1] = 2; /* For ES2 */

   /*
      Step 1 - Get the default display.
   */
   egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

   /*
      Step 2 - Initialize EGL.
      EGL has to be initialized with the display obtained in the
      previous step. We cannot use other EGL functions except
      eglGetDisplay and eglGetError before eglInitialize has been
      called.
   */
   if (!eglInitialize(egl_display, &major_version, &minor_version))
   {
      printf("eglInitialize() failed");
      return 0;
   }

   /*
      Step 3 - Specify the required configuration attributes.
      An EGL "configuration" describes the pixel format and type of
      surfaces that can be used for drawing.
      For now we just want to use a 16 bit RGB surface that is a
      Window surface, i.e. it will be visible on screen. The list
      has to contain key/value pairs, terminated with EGL_NONE.
      */
   config_attribs[i++] = EGL_RED_SIZE;
   config_attribs[i++] = RED;

   config_attribs[i++] = EGL_GREEN_SIZE;
   config_attribs[i++] = GREEN;

   config_attribs[i++] = EGL_BLUE_SIZE;
   config_attribs[i++] = BLUE;

   config_attribs[i++] = EGL_ALPHA_SIZE;
   config_attribs[i++] = ALPHA;

   config_attribs[i++] = EGL_DEPTH_SIZE;
   config_attribs[i++] = 24;

   config_attribs[i++] = EGL_STENCIL_SIZE;
   config_attribs[i++] = 8;

   config_attribs[i++] = EGL_SURFACE_TYPE;
   config_attribs[i++] = EGL_PIXMAP_BIT;

   config_attribs[i++] = EGL_RENDERABLE_TYPE;
   config_attribs[i++] = EGL_OPENGL_ES2_BIT;

   /* For multisample
   config_attribs[i++] = EGL_SAMPLE_BUFFERS;
   config_attribs[i++] = 1;
   config_attribs[i++] = EGL_SAMPLES;
   config_attribs[i++] = 4;
   */

   /* Terminate the list by adding EGL_NONE */
   config_attribs[i++] = EGL_NONE;

   /*
      Step 3.5 - Get the number of configurations to correctly size the array
      used in step 4
   */
   if (!eglGetConfigs(egl_display, NULL, 0, &configs))
   {
      printf("eglGetConfigs() failed");
      return 0;
   }

   egl_config = (EGLConfig *)alloca(configs * sizeof(EGLConfig));

   /*
      Step 4 - Find a config that matches all requirements.
      eglChooseConfig provides a list of all available configurations
      that meet or exceed the requirements given as the second
      argument.
   */
   if (!eglChooseConfig(egl_display, config_attribs, egl_config, configs, &configs) || (configs == 0))
   {
      printf("eglChooseConfig() failed");
      return 0;
   }

   for (i = 0; i < configs; i++)
   {
      /*
         Configs with deeper color buffers get returned first by eglChooseConfig.
         Applications may find this counterintuitive, and need to perform additional processing on the list of
         configs to find one best matching their requirements. For example, specifying RGBA depths of 565
         could return a list whose first config has a depth of 888.
      */

      /* If we asked for a 565, but all the 8888 match first, we need to pick an actual 565 buffer */
      EGLint red_size, green_size, blue_size;
      eglGetConfigAttrib(egl_display, egl_config[i], EGL_RED_SIZE, &red_size);
      eglGetConfigAttrib(egl_display, egl_config[i], EGL_GREEN_SIZE, &green_size);
      eglGetConfigAttrib(egl_display, egl_config[i], EGL_BLUE_SIZE, &blue_size);

      if (RED == 5 && GREEN == 6 && BLUE == 5)
      {
         if ((red_size == 5) && (green_size == 6) && (blue_size == 5))
            break;
      }
      else
      {
         if ((red_size == 8) && (green_size == 8) && (blue_size == 8))
            break;
      }
   }
   configs = i;

   /*
      Step 5 - Create a surface to draw to.
      Use the config picked in the previous step and the native window
      handle to create a window surface. 
   */
   egl_surface = eglCreateWindowSurface(egl_display, egl_config[configs], egl_win, NULL);
   if (egl_surface == EGL_NO_SURFACE)
   {
      printf("eglCreateWindowSurface() failed\n");
      return false;
   }

   /*
      Step 6 - Create a context.
      EGL has to create a context for OpenGL ES. Our OpenGL ES resources
      like textures will only be valid inside this context (or shared contexts)
   */
   egl_context = eglCreateContext(egl_display, egl_config[configs], EGL_NO_CONTEXT, ctx_attrib_list);
   if (egl_context == EGL_NO_CONTEXT)
   {
      printf("eglCreateContext() failed");
      return 0;
   }

   /*
      Step 7 - Bind the context to the current thread and use our
      window surface for drawing and reading.
      We need to specify a surface that will be the target of all
      subsequent drawing operations, and one that will be the source
      of read operations. They can be the same surface.
   */
   eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context);

   eglSwapInterval(egl_display, 1);

   return 1;
}

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

void blit_or_fill_finished(void *data, int unused)
{
   uint32_t i;

   /* current_3d_context buffer is now on the display, so signal that to V3D */
   BKNI_AcquireMutex(context_mutex);

   for (i = 0; i < NUM_3D_THREADS; i++)
   {
      if (windowThread[i].current_3d_context && bufferOnDisplayFunc)
      {
         bufferOnDisplayFunc(windowThread[i].current_3d_context);
         windowThread[i].current_3d_context = NULL;
      }
   }

   BKNI_ReleaseMutex(context_mutex);

   BSTD_UNUSED(unused);
   BKNI_SetEvent((BKNI_EventHandle)data);
}

void vsync(void *data, int unused)
{
   BSTD_UNUSED(unused);
   BKNI_SetEvent((BKNI_EventHandle)data);
}

void Render3DFrame(ThreedWindowThread *window, float cube_alpha, float bg_alpha)
{
   /* Rotate the cube */
   esRotate(&window->modelview_matrix, 1.0f, 1, 0, 0);
   esRotate(&window->modelview_matrix, 0.5f, 0, 1, 0);

   /* Compute the final MVP by multiplying the model-view and perspective matrices together */
   esMatrixMultiply(&window->mvp_matrix, &window->modelview_matrix, &window->projection_matrix);

   /* Clear all the buffers we asked for during config to ensure fast-path */
   glClearColor(1.0f, 1.0f, 1.0f, bg_alpha);
   glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glUseProgram(window->program_object);

   /* Enable cube array */
   glBindBuffer(GL_ARRAY_BUFFER, window->vbo[0]);
   glVertexAttribPointer(window->position_loc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), BUFFER_OFFSET(0));
   glVertexAttribPointer(window->color_loc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), BUFFER_OFFSET(3 * sizeof(GLfloat)));
   glEnableVertexAttribArray(window->position_loc);
   glEnableVertexAttribArray(window->color_loc);

   /* Load the MVP matrix */
   glUniformMatrix4fv(window->mvp_matrix_loc, 1, GL_FALSE, (GLfloat*)&window->mvp_matrix.m[0][0]);

   /* Set the overall alpha for the cube */
   glUniform1f(window->alpha_loc, cube_alpha);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, window->vbo[1]);

   /* Finally draw the elements */
   glDrawElements(GL_TRIANGLES, sizeof(cube_idx) / sizeof(GLushort), GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));

   /* Finish 3D rendering - does not block */
   eglSwapBuffers(eglGetCurrentDisplay(), eglGetCurrentSurface(EGL_READ));

   /* Help the scheduler */
   pthread_yield();
}

void Render2DFrame(void)
{
   NEXUS_Error rc;
   NEXUS_Graphics2DBlitSettings blit_settings;
   NEXUS_Graphics2DFillSettings fillSettings;
   uint32_t                     i;

   static int cur_fb = 0;

   /* Wait for previous vsync to trigger before drawing into the buffer again */
   BKNI_WaitForEvent(vsync_event, 0xffffffff);

   /* Fill dest surface with something slightly more interesting than a flat color */
   /* Should run in parallel with 3D rendering */
   NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
   fillSettings.surface = nexus_framebuffer[cur_fb];
   fillSettings.rect.width = WIDTH_2D;
   fillSettings.rect.height = HEIGHT_2D;
   fillSettings.color = 0xFF101010;
   NEXUS_Graphics2D_Fill(nexus_gfx, &fillSettings);

   fillSettings.rect.x = WIDTH_2D / 6;
   fillSettings.rect.y = HEIGHT_2D / 6;
   fillSettings.rect.width = WIDTH_2D - (WIDTH_2D / 3);
   fillSettings.rect.height = HEIGHT_2D - (HEIGHT_2D / 3);
   fillSettings.color = 0xFF303030;
   NEXUS_Graphics2D_Fill(nexus_gfx, &fillSettings);

   fillSettings.rect.x = WIDTH_2D / 4;
   fillSettings.rect.y = HEIGHT_2D / 4;
   fillSettings.rect.width = WIDTH_2D - (WIDTH_2D / 2);
   fillSettings.rect.height = HEIGHT_2D - (HEIGHT_2D / 2);
   fillSettings.color = 0xFF404040;
   NEXUS_Graphics2D_Fill(nexus_gfx, &fillSettings);

   for (i = 0; i < NUM_3D_THREADS; i++)
   {
      BKNI_AcquireMutex(surface_mutex);

      if (windowThread[i].current_3d_surface)
      {
         /* Blit last 3D frame into current framebuffer */
         NEXUS_Graphics2D_GetDefaultBlitSettings(&blit_settings);
         blit_settings.colorOp = NEXUS_BlitColorOp_eUseSourceAlpha;
         blit_settings.alphaOp = NEXUS_BlitAlphaOp_eCopyDest;
         blit_settings.source.surface = windowThread[i].current_3d_surface;
         blit_settings.output.surface = nexus_framebuffer[cur_fb];
         blit_settings.output.rect.x = windowThread[i].px;
         blit_settings.output.rect.y = windowThread[i].py;
         blit_settings.output.rect.width = GL_WIDTH;
         blit_settings.output.rect.height = GL_HEIGHT;
         blit_settings.dest.surface = nexus_framebuffer[cur_fb];
         blit_settings.dest.rect.x = windowThread[i].px;
         blit_settings.dest.rect.y = windowThread[i].py;
         blit_settings.dest.rect.width = GL_WIDTH;
         blit_settings.dest.rect.height = GL_HEIGHT;

         NEXUS_Graphics2D_Blit(nexus_gfx, &blit_settings);

         if (fillOnTop)
         {
            /* Blit something on top of the 3D, just to show that it works */
            NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
            fillSettings.surface = nexus_framebuffer[cur_fb];
            fillSettings.colorOp = NEXUS_FillOp_eBlend;
            fillSettings.rect.x = WIDTH_2D / 8;
            fillSettings.rect.y = HEIGHT_2D / 8;
            fillSettings.rect.width = WIDTH_2D - (WIDTH_2D / 4);
            fillSettings.rect.height = HEIGHT_2D - (HEIGHT_2D / 4);
            fillSettings.color = 0x80A00000;
            NEXUS_Graphics2D_Fill(nexus_gfx, &fillSettings);
         }

         rc = NEXUS_Graphics2D_Checkpoint(nexus_gfx, NULL);
         if (rc == NEXUS_GRAPHICS2D_QUEUED) 
         {
            /* Wait for the blit to complete before continuing */
            BKNI_WaitForEvent(blit_fill_event, 0xffffffff);
         }
      }

      BKNI_ReleaseMutex(surface_mutex);
   }

   /* Switch the display to use this framebuffer */
   NEXUS_Display_SetGraphicsFramebuffer(nexus_display, nexus_framebuffer[cur_fb]);

   cur_fb = (cur_fb + 1) & 1;
}


/* A new 3D frame is ready to be displayed. This will be called asynchronously. */
static void DisplayBuffer(void *context, NEXUS_SurfaceHandle surface, void *window)
{
   uint32_t i;

   /* We will simply remember the last buffer that was requested to be displayed, and
      will blit it when we next render a 2D frame.
      Note: it is possible (when swapInterval is 0) that this might be called multiple times
      before the next 2D render. We just need to use the last one (swap 0 is unsynchronized anyway).
      For all other non-zero swapIntervals, we shouldn't get another DisplayBuffer call until
      we've called the OnDisplay callback during the vsync handler.
    */

   for (i = 0; i < NUM_3D_THREADS; i++)
   {
      if (windowThread[i].native_window == window)
      {
         BKNI_AcquireMutex(context_mutex);
         windowThread[i].current_3d_context = context;
         BKNI_ReleaseMutex(context_mutex);

         BKNI_AcquireMutex(surface_mutex);
         windowThread[i].current_3d_surface = surface;
         BKNI_ReleaseMutex(surface_mutex);

         break;
      }
   }

}

static void WindowUndisplay(void *context, void *window)
{
   uint32_t i;

   BSTD_UNUSED(context);
   
   for (i = 0; i < NUM_3D_THREADS; i++)
   {
      if (windowThread[i].native_window == window)
      {
         BKNI_AcquireMutex(context_mutex);
         windowThread[i].current_3d_context = NULL;
         BKNI_ReleaseMutex(context_mutex);

         BKNI_AcquireMutex(surface_mutex);
         windowThread[i].current_3d_surface = NULL;
         BKNI_ReleaseMutex(surface_mutex);

         break;
      }
   }

}

void InitDisplay(unsigned int width, unsigned int height)
{
   NEXUS_SurfaceCreateSettings   create_settings;
   NEXUS_Graphics2DOpenSettings  openSettings;
   NEXUS_GraphicsSettings        graphicsSettings;
   NEXUS_Graphics2DSettings      gfxSettings;
   NXPL_CompositingData          compData;
   float                         aspect;

   eInitResult res = InitPlatformAndDefaultDisplay((NEXUS_DISPLAYHANDLE*)&nexus_display, &aspect, width, height);
   if (res != eInitSuccess)
      return;

   memset(&compData, 0, sizeof(NXPL_CompositingData));
   compData.bufferBlit = DisplayBuffer;
   compData.windowUndisplay = WindowUndisplay;
   NXPL_RegisterNexusCompositedDisplayPlatform(&nxpl_handle, &compData);
   bufferOnDisplayFunc = compData.bufferOnDisplay;


   heap = NEXUS_Platform_GetFramebufferHeap(0);

   /* Create a base surface to use as the framebuffer */
   NEXUS_Surface_GetDefaultCreateSettings(&create_settings);
   create_settings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
   create_settings.width = width;
   create_settings.height = height;
   create_settings.heap = heap;
   nexus_framebuffer[0] = NEXUS_Surface_Create(&create_settings);

   NEXUS_Surface_GetDefaultCreateSettings(&create_settings);
   create_settings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
   create_settings.width = width;
   create_settings.height = height;
   create_settings.heap = heap;
   nexus_framebuffer[1] = NEXUS_Surface_Create(&create_settings);

   /* Open Nexus 2D */
   NEXUS_Graphics2D_GetDefaultOpenSettings(&openSettings);
   openSettings.preAllocPacketMemory = true;
   openSettings.maxOperations = MAX_OPERATIONS;
   nexus_gfx = NEXUS_Graphics2D_Open(0, &openSettings);

   /* Set the vsync initially triggered to prevent block */
   BKNI_CreateEvent(&vsync_event);
   BKNI_SetEvent(vsync_event);

   NEXUS_Display_GetGraphicsSettings(nexus_display, &graphicsSettings);
   graphicsSettings.enabled = true;
   graphicsSettings.position.width = width;
   graphicsSettings.position.height = height;
   graphicsSettings.clip.width = width;
   graphicsSettings.clip.height = height;
   graphicsSettings.frameBufferCallback.callback = vsync;
   graphicsSettings.frameBufferCallback.context = vsync_event;
   NEXUS_Display_SetGraphicsSettings(nexus_display, &graphicsSettings);

   /* Setup a blit finished callback */
   BKNI_CreateEvent(&blit_fill_event);

   NEXUS_Graphics2D_GetSettings(nexus_gfx, &gfxSettings);
   gfxSettings.checkpointCallback.callback = blit_or_fill_finished;
   gfxSettings.checkpointCallback.context = blit_fill_event;
   NEXUS_Graphics2D_SetSettings(nexus_gfx, &gfxSettings);
}

void MoveWindows(void)
{
   uint32_t i;
   for (i = 0; i < NUM_3D_THREADS; i++)
   {
      float *px = &windowThread[i].px;
      float *py = &windowThread[i].py;
      float *vx = &windowThread[i].vx;
      float *vy = &windowThread[i].vy;

      *px += *vx;
      *py += *vy;

      if (*px + GL_WIDTH >= WIDTH_2D)
      {
         *vx = -*vx;
         *px = WIDTH_2D - GL_WIDTH - 1;
      }
      else if (*px <= 0.0f)
      {
         *vx = -*vx;
         *px = 0.0f;
      }

      if (*py + GL_HEIGHT >= HEIGHT_2D)
      {
         *vy = -*vy;
         *py = HEIGHT_2D - GL_HEIGHT - 1;
      }
      else if (*py <= 0.0f)
      {
         *vy = -*vy;
         *py = 0.0f;
      }
   }
}

/* Runs in its own thread. Nothing in this thread blocks, so the 3D core can run ahead up to 3 frames.
 * This is the most efficient way to run the core. The thread will eventually block if the frames can't actually be displayed
 * quickly enough (the 2d vsync will cause this). */
void *Run3D(void *p)
{
   uint32_t                frame;
   NXPL_NativeWindowInfo   win_info;
   EGLDisplay              eglDisplay;
   uint32_t                instance = (uint32_t)p;
   uint32_t                i;
   bool                    allDone;
   uint32_t                numFrames = 255;

   fillOnTop = false;

   win_info.x = 0; 
   win_info.y = 0;
   win_info.width = GL_WIDTH;
   win_info.height = GL_HEIGHT;
   win_info.stretch = false;
   win_info.clientID = 0;
   windowThread[instance].native_window = NXPL_CreateNativeWindow(&win_info);

   /* Initialise EGL with a smaller 3D window */
   InitEGL(windowThread[instance].native_window);

   /* Setup the local state for this demo */
   InitGLState(&windowThread[instance]);
   InitGLViewPort(&windowThread[instance], GL_WIDTH, GL_HEIGHT);

   printf("OpenGL-ES composited on Nexus fills (background alpha = 1.0, cube alpha = 1.0) ...\n");
   for (frame = 0; frame < numFrames; frame++)
      Render3DFrame(&windowThread[instance], 1.0f, 1.0f);

   printf("OpenGL-ES composited on Nexus fills (background alpha = 0.0, cube alpha = 1.0) ...\n");
   for (frame = 0; frame < numFrames; frame++)
      Render3DFrame(&windowThread[instance], 1.0f, 0.0f);

   printf("OpenGL-ES composited on Nexus fills (background alpha = 0.0, cube alpha = varying) ...\n");
   for (frame = 0; frame < numFrames; frame++)
      Render3DFrame(&windowThread[instance], (float)frame / (float)numFrames, 0.0f);

   printf("OpenGL-ES composited on Nexus fills (background alpha = 1.0, cube alpha = 0.0) ...\n");
   for (frame = 0; frame < numFrames; frame++)
      Render3DFrame(&windowThread[instance], 0.0f, 1.0f);

   printf("OpenGL-ES composited on Nexus fills, filled on top with Nexus ...\n");
   fillOnTop = true;
   for (frame = 0; frame < numFrames; frame++)
      Render3DFrame(&windowThread[instance], 1.0f, 0.0f);

   /* Close the local state for this demo */
   TerminateGLState(&windowThread[instance]);

   eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
   eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

   eglTerminate(eglDisplay);
   eglReleaseThread();

   windowThread[instance].done = true;

   allDone = true;
   for (i = 0; i < NUM_3D_THREADS; i++)
   {
      if (!windowThread[i].done)
         allDone = false;
   }
   threedDone = allDone;

   return NULL;
}

void Start3DFrames(void)
{
   uint32_t i;
   for (i = 0; i < NUM_3D_THREADS; i++)
   {
      pthread_attr_t    attr;

      /* Start a thread to render a number of 3D frames. Nothing in this thread blocks, so the 3D core can run ahead up to 3 frames. */
      /* This is the most efficient way to run the core. The thread will eventually block if the frames can't actually be displayed */
      /* quickly enough (the 2d vsync will cause this). */
      pthread_attr_init(&attr);
      pthread_create(&windowThread[i].thread, &attr, &Run3D, (void*)i);
      pthread_attr_destroy(&attr);
   }
}

void WaitFor3DFramesToFinish(void)
{
   uint32_t i;
   for (i = 0; i < NUM_3D_THREADS; i++)
      pthread_join(windowThread[i].thread, NULL);
}

int main(int argc, char** argv)
{
   NEXUS_GraphicsSettings graphicsSettings;
   uint32_t               i;

   BSTD_UNUSED(argc);
   BSTD_UNUSED(argv);

   memset(&windowThread, 0, sizeof(windowThread));
   for (i = 0; i < NUM_3D_THREADS; i++)
   {
      windowThread[i].px = 200.0f * i;
      windowThread[i].py = 200.0f * i;
      windowThread[i].vx = 1.0f + i;
      windowThread[i].vy = 1.0f + i * 2;
   }

   /* Setup the display and EGL */
   InitDisplay(WIDTH_2D, HEIGHT_2D);

   BKNI_CreateMutex(&context_mutex);
   BKNI_CreateMutex(&surface_mutex);

   printf("Press CTRL+C to terminate early\n");
   
   /* We'll generate 3D frames in separate threads, which don't have to block, for best performance */
   Start3DFrames();

   /* Whilst the 3D frames are still being generated, we'll update the 2D display continuously */
   while (!threedDone)
   {
      MoveWindows();
      Render2DFrame();
   }

   /* And wait for the 3D thread to exit */
   WaitFor3DFramesToFinish();

   /* Remove the framebuffer surface so we can delete it */
   NEXUS_Display_GetGraphicsSettings(nexus_display, &graphicsSettings);
   graphicsSettings.enabled = false;
   NEXUS_Display_SetGraphicsSettings(nexus_display, &graphicsSettings);

   /* Close the graphics 2D */
   NEXUS_Graphics2D_Close(nexus_gfx);

   /* Destroy the nexus surfaces */
   NEXUS_Surface_Destroy(nexus_framebuffer[0]);
   NEXUS_Surface_Destroy(nexus_framebuffer[1]);
   
   NXPL_UnregisterNexusCompositedDisplayPlatform(nxpl_handle);

   /* Close the Nexus display */
   NEXUS_Display_Close(nexus_display);

   /* Free resources */
   BKNI_DestroyEvent(blit_fill_event);
   BKNI_DestroyEvent(vsync_event);
   BKNI_DestroyMutex(context_mutex);
   BKNI_DestroyMutex(surface_mutex);

   /* Close the platform */
   NEXUS_Platform_Uninit();

   return 0;
}
