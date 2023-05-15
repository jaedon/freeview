/*
   (c) Copyright 2000-2002  convergence integrated media GmbH.
   All rights reserved.

   Written by Denis Oliver Kropp <dok@directfb.org>,
              Andreas Hundt <andi@fischlustig.de> and
              Sven Neumann <neo@directfb.org>.

   This file is subject to the terms and conditions of the MIT License:

   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated documentation
   files (the "Software"), to deal in the Software without restriction,
   including without limitation the rights to use, copy, modify, merge,
   publish, distribute, sublicense, and/or sell copies of the Software,
   and to permit persons to whom the Software is furnished to do so,
   subject to the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/* Define this to use hardware accelerated video memory */

#include "platform_init.h"
#include <config.h>

#ifdef USE_ZLIB
#include <zlib.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <directfb.h>
#include <directfb_util.h>

#include <direct/util.h>

#define DIRECTFB_ROTATE

#ifdef USE_ZLIB
#define DFBLOGO_FILENAME "/tmp/dfblogo_0000.raw.gz"
#else
#define DFBLOGO_FILENAME "/tmp/dfblogo.raw"
#endif

/* macro for a safe call to DirectFB functions */
#define DFBCHECK(x) \
     {                                                                \
          err = x;                                                    \
          if (err != DFB_OK) {                                        \
               fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ ); \
               DirectFBErrorFatal( #x, err );                         \
          }                                                           \
     }

static inline long myclock(void)
{
    struct timeval tv;

    gettimeofday (&tv, NULL);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

static DFB_PlatformNexusHandle w_getOffScreenHeapHandle(void)
{
    DFB_PlatformSettings platformSettings;

    DFB_Platform_GetSettings(&platformSettings);
    return platformSettings.offscreenHeapHandle;
}

static void *w_malloc(int size, bool nexus)
{
    void *ptr;

    if (nexus)
    {
        NEXUS_MemoryAllocationSettings settings;

        NEXUS_Memory_GetDefaultAllocationSettings(&settings);
        settings.alignment = 4096;  /* Surface needs to be aligned on a 4K boundary */
        settings.heap = w_getOffScreenHeapHandle();
        NEXUS_Memory_Allocate(size, &settings, &ptr);
    }
    else
        ptr = malloc(size);

    return ptr;
}

static void w_free(void *ptr, bool nexus)
{
    if (nexus)
        NEXUS_Memory_Free(ptr);
    else
        free(ptr);
}

static int
print_usage( const char *prg )
{
     fprintf (stderr, "\n");
     fprintf (stderr, "== DirectFB Window Pre-Allocation Memory Test ==\n");
     fprintf (stderr, "\n");

     fprintf (stderr, "Usage: %s [options]\n", prg);
     fprintf (stderr, "\n");
     fprintf (stderr, "Options:\n");
     fprintf (stderr, "  -h, --help                        Show this help message\n");
     fprintf (stderr, "  -v, --version                     Print version information\n");
     fprintf (stderr, "  -s, --static                      Use DSCAPS_STATIC_ALLOC\n");
     fprintf (stderr, "  -n, --nexus                       Use Nexus video-only memory (default is system memory)\n");
     return -1;
}

int main( int argc, char *argv[] )
{
     IDirectFB                    *dfb;
     IDirectFBDisplayLayer        *layer;

     IDirectFBImageProvider       *provider;
     IDirectFBVideoProvider       *video_provider;

     IDirectFBSurface             *bgsurface;
     IDirectFBSurface             *cursurface;
     IDirectFBSurface             *cursurface2;

     IDirectFBWindow              *window1;
     IDirectFBWindow              *window2;
     IDirectFBSurface             *window_surface1;
     IDirectFBSurface             *window_surface2;

     IDirectFBEventBuffer         *buffer;

     IDirectFBFont                *font;

     DFBDisplayLayerConfig         layer_config;
     DFBGraphicsDeviceDescription  gdesc;
     IDirectFBWindow*              upper;
     DFBWindowID                   id1;

     int                           i;
     int                           fontheight;
     int                           rotation;
     int                           err;
     int                           quit = 0;

     IDirectFBWindow*              active = NULL;
     int                           grabbed = 0;
     int                           startx = 0;
     int                           starty = 0;
     int                           endx = 0;
     int                           endy = 0;
     int                           winupdate = 0, winx = 0, winy = 0;

     bool                          static_alloc = false;
     bool                          nexus_video_memory = false;

     DFBCHECK(DirectFBInit( &argc, &argv ));

     /* Parse arguments. */
     for (i=1; i<argc; i++) {
          const char *arg = argv[i];

          if (strcmp( arg, "-h" ) == 0 || strcmp (arg, "--help") == 0)
               return print_usage( argv[0] );
          else if (strcmp (arg, "-v") == 0 || strcmp (arg, "--version") == 0) {
               fprintf (stderr, "df_window_prealloc version\n");
               return false;
          }
          else if (strcmp (arg, "-s") == 0 || strcmp (arg, "--static") == 0) {
              static_alloc = true;
          }
          else if (strcmp (arg, "-n") == 0 || strcmp (arg, "--nexus") == 0) {
              nexus_video_memory = true;
          }
          else
               return print_usage( argv[0] );
     }

     DFBCHECK(DirectFBCreate( &dfb ));

     dfb->GetDeviceDescription( dfb, &gdesc );

     DFBCHECK(dfb->GetDisplayLayer( dfb, DLID_PRIMARY, &layer ));

     layer->SetCooperativeLevel( layer, DLSCL_ADMINISTRATIVE );

     layer->GetConfiguration( layer, &layer_config );
#ifdef DIRECTFB_ROTATE
     layer->GetRotation( layer, &rotation );
#endif

     {
          DFBFontDescription desc;

          desc.flags = DFDESC_HEIGHT;
          desc.height = layer_config.width/42;

          DFBCHECK(dfb->CreateFont( dfb, FONT, &desc, &font ));
          font->GetHeight( font, &fontheight );
     }

     if (argc < 2 ||
         dfb->CreateVideoProvider( dfb, argv[1], &video_provider ) != DFB_OK)
     {
          video_provider = NULL;
     }

     {
          DFBSurfaceDescription desc;

          DFBCHECK(dfb->CreateImageProvider( dfb,
                                             DATADIR"/desktop.png",
                                             &provider ));

          desc.flags  = DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_CAPS;
          desc.width  = layer_config.width;
          desc.height = layer_config.height;
          desc.caps   = DSCAPS_SHARED;

#ifdef DIRECTFB_ROTATE
          if (rotation == 90 || rotation == 270)
          {
              unsigned int tmp;
              tmp = desc.width;
              desc.width = desc.height;
              desc.height = tmp;
          }
#endif

          DFBCHECK(dfb->CreateSurface( dfb, &desc, &bgsurface ) );

          provider->RenderTo( provider, bgsurface, NULL );
          provider->Release( provider );

          DFBCHECK(dfb->CreateImageProvider( dfb,
                             DATADIR"/cursor.png",
                             &provider ));

          desc.flags  = DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_CAPS;
          desc.width  = 32;
          desc.height = 32;
          desc.caps   = DSCAPS_NONE;

          DFBCHECK(dfb->CreateSurface( dfb, &desc, &cursurface ) );

          DFBCHECK(provider->RenderTo( provider, cursurface, NULL ) );
          provider->Release( provider );

          DFBCHECK(layer->SetCursorShape(layer,cursurface,0,0));

          layer->EnableCursor ( layer, 1 );

          DFBCHECK(bgsurface->SetFont( bgsurface, font ));

          bgsurface->SetColor( bgsurface, 0xCF, 0xCF, 0xFF, 0xFF );
          bgsurface->DrawString( bgsurface,
                                 "Move the mouse over a window to activate it.",
                                 -1, 0, 0, DSTF_LEFT | DSTF_TOP );

          bgsurface->SetColor( bgsurface, 0xCF, 0xDF, 0xCF, 0xFF );
          bgsurface->DrawString( bgsurface,
                                 "Press left mouse button and drag to move the window.",
                                 -1, 0, fontheight, DSTF_LEFT | DSTF_TOP );

          bgsurface->SetColor( bgsurface, 0xCF, 0xEF, 0x9F, 0xFF );
          bgsurface->DrawString( bgsurface,
                                 "Press middle mouse button to raise/lower the window.",
                                 -1, 0, fontheight * 2, DSTF_LEFT | DSTF_TOP );

          bgsurface->SetColor( bgsurface, 0xCF, 0xFF, 0x6F, 0xFF );
          bgsurface->DrawString( bgsurface,
                                 "Hold right mouse button to fade in/out the window.", -1,
                                 0, fontheight * 3,
                                 DSTF_LEFT | DSTF_TOP );

          layer->SetBackgroundImage( layer, bgsurface );
          layer->SetBackgroundMode( layer, DLBM_IMAGE );
     }

     {
          DFBSurfaceDescription sdsc;
          DFBWindowDescription  desc;

          desc.flags = ( DWDESC_POSX | DWDESC_POSY |
                         DWDESC_WIDTH | DWDESC_HEIGHT );

          if (!video_provider) {
               desc.caps = DWCAPS_ALPHACHANNEL;
               desc.flags |= DWDESC_CAPS;

               sdsc.width  = 300;
               sdsc.height = 200;
          }
          else {
               video_provider->GetSurfaceDescription( video_provider, &sdsc );

               if (sdsc.flags & DSDESC_CAPS) {
                    desc.flags       |= DWDESC_SURFACE_CAPS;
                    desc.surface_caps = sdsc.caps;
               }
          }

          desc.posx   = 20;
          desc.posy   = 120;
          desc.width  = sdsc.width;
          desc.height = sdsc.height;

          DFBCHECK( layer->CreateWindow( layer, &desc, &window2 ) );
          window2->GetSurface( window2, &window_surface2 );

          window2->SetOpacity( window2, 0xFF );

          window2->CreateEventBuffer( window2, &buffer );

          if (video_provider) {
               video_provider->PlayTo( video_provider, window_surface2,
                                       NULL, NULL, NULL );
          }
          else
          {
               window_surface2->SetColor( window_surface2,
                                          0x00, 0x30, 0x10, 0xc0 );
               window_surface2->DrawRectangle( window_surface2,
                                               0, 0, desc.width, desc.height );
               window_surface2->SetColor( window_surface2,
                                          0x80, 0xa0, 0x00, 0x90 );
               window_surface2->FillRectangle( window_surface2,
                                               1, 1,
                                               desc.width-2, desc.height-2 );
          }


          DFBCHECK(window2->SetCursorShape(window2, cursurface, 0, 0));

          window_surface2->Flip( window_surface2, NULL, 0 );
     }

     {
          DFBWindowDescription desc;

          desc.flags  = ( DWDESC_POSX | DWDESC_POSY |
                          DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_CAPS );
          desc.posx   = 200;
          desc.posy   = 200;
          desc.width  = 512;
          desc.height = 145;
          desc.caps   = DWCAPS_ALPHACHANNEL;

          DFBCHECK(layer->CreateWindow( layer, &desc, &window1 ) );
          window1->GetSurface( window1, &window_surface1 );
          window_surface1->SetFont( window_surface1, font );

          {
               FILE *fd;
               unsigned char *ptr;
               size_t size;
               DFBSurfaceDescription dfblogo_desc;
               IDirectFBSurface *surf;
               void *ptr2;
               int pitch;
#ifdef USE_ZLIB
               gzFile gz_dfblogo = NULL;
               int fd_p;
#endif
               DFBCHECK(dfb->CreateImageProvider( dfb,
                                                  DATADIR"/dfblogo.png",
                                                  &provider ));

               provider->GetSurfaceDescription(provider, &dfblogo_desc);

               size = dfblogo_desc.width * DFB_BYTES_PER_PIXEL(dfblogo_desc.pixelformat) * dfblogo_desc.height;

               fd = fopen(DFBLOGO_FILENAME, "r");
               if (fd == NULL)
               {
                   provider->RenderTo( provider, window_surface1, NULL );
                   window_surface1->DumpRaw( window_surface1, "/tmp", "dfblogo" );
                   fd = fopen(DFBLOGO_FILENAME, "r");
                   if (fd == NULL)
                   {
                       fprintf( stderr, "%s <%d>: Cannot open \"%s\"!\n", __FILE__, __LINE__, DFBLOGO_FILENAME );
                       DirectFBErrorFatal("Cannot read " DFBLOGO_FILENAME "!\n", DFB_FAILURE);
                   }
               }

               ptr = (unsigned char *)w_malloc(size, nexus_video_memory);
               if (ptr == NULL)
               {
                   fprintf( stderr, "%s <%d>: Cannot allocate %d bytes of memory!\n", __FILE__, __LINE__, size );
                   DirectFBErrorFatal("Cannot allocate memory!\n", DFB_FAILURE);
               }

#ifdef USE_ZLIB
               fd_p = fileno(fd);
               gz_dfblogo = gzdopen( fd_p, "rb" );
               gzread(gz_dfblogo, ptr, size);
               gzclose(gz_dfblogo);
#else
               if (fread(ptr, size, 1, fd) != 1)
               {
                   fprintf( stderr, "%s <%d>: Cannot read %d bytes from file \"%s\"!\n", __FILE__, __LINE__, size, DFBLOGO_FILENAME );
                   DirectFBErrorFatal("Cannot read from file!\n", DFB_FAILURE);
               }
#endif
               fclose(fd);

               dfblogo_desc.flags |= DSDESC_PREALLOCATED;
               dfblogo_desc.flags |= DSDESC_CAPS;
               dfblogo_desc.caps = DSCAPS_NONE;

               if (static_alloc)
                   dfblogo_desc.caps |= DSCAPS_STATIC_ALLOC;

               if (nexus_video_memory)
               {
                   dfblogo_desc.caps |= DSCAPS_VIDEOONLY;
#if (DIRECTFB_MAJOR_VERSION == 1 && DIRECTFB_MINOR_VERSION == 4 && DIRECTFB_MICRO_VERSION <= 5)
                   dfblogo_desc.preallocated[0].handle = (unsigned long)w_getOffScreenHeapHandle();
#endif
               }
               else
               {
                   dfblogo_desc.caps |= DSCAPS_SYSTEMONLY;
               }
               dfblogo_desc.preallocated[0].data = ptr;
               dfblogo_desc.preallocated[0].pitch = dfblogo_desc.width * DFB_BYTES_PER_PIXEL(dfblogo_desc.pixelformat);
               DFBCHECK(dfb->CreateSurface(dfb, &dfblogo_desc, &surf));
               window_surface1->StretchBlit(window_surface1, surf, NULL, NULL);
               surf->Lock(surf, DSLF_WRITE, &ptr2, &pitch);
               printf("Prealloc %s%s buffer: %p Locked buffer: %p\n",
                       (static_alloc) ? "static " : "",
                       (nexus_video_memory) ? "video" : "system", ptr, ptr2);
               surf->Unlock(surf);
               surf->Release( surf );
               w_free(ptr, nexus_video_memory);
          }

          window_surface1->SetColor( window_surface1, 0xFF, 0x20, 0x20, 0x90 );
          window_surface1->DrawRectangle( window_surface1,
                                          0, 0, desc.width, desc.height );

          window_surface1->Flip( window_surface1, NULL, 0 );

          provider->Release( provider );

          window1->AttachEventBuffer( window1, buffer );

          window1->SetOpacity( window1, 0xFF );

          window1->GetID( window1, &id1 );

          {
               DFBSurfaceDescription desc;

               DFBCHECK(dfb->CreateImageProvider( dfb,
                             DATADIR"/cursor_red.png",
                             &provider ));

               desc.flags  = DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_CAPS;
               desc.width  = 32;
               desc.height = 32;
               desc.caps   = DSCAPS_NONE;

               DFBCHECK(dfb->CreateSurface( dfb, &desc, &cursurface2 ) );

               DFBCHECK(provider->RenderTo( provider, cursurface2, NULL ) );
               provider->Release( provider );
          }
              DFBCHECK(window1->SetCursorShape(window1, cursurface2, 0, 0));
     }

     window1->RequestFocus( window1 );
     window1->RaiseToTop( window1 );
     upper = window1;

     rotation = 0;

     while (!quit) {
          DFBWindowEvent evt;

          buffer->WaitForEventWithTimeout( buffer, 0, 10 );

          while (buffer->GetEvent( buffer, DFB_EVENT(&evt) ) == DFB_OK) {
               IDirectFBWindow* window;

               if (evt.window_id == id1)
                    window = window1;
               else
                    window = window2;

               if (evt.type == DWET_GOTFOCUS) {
                    active = window;
               }
               else if (active) {
                    switch (evt.type) {

                    case DWET_BUTTONDOWN:
                         if (!grabbed) {
                              grabbed = evt.buttons;
                              startx  = evt.cx;
                              starty  = evt.cy;
                              window->GrabPointer( window );
                         }
                         break;

                    case DWET_BUTTONUP:
                         switch (evt.button) {
                              case DIBI_LEFT:
                                   if (grabbed && !evt.buttons) {
                                        window->UngrabPointer( window );
                                        grabbed = 0;
                                   }
                                   break;
                              case DIBI_MIDDLE:
                                   upper->LowerToBottom( upper );
                                   upper =
                                     (upper == window1) ? window2 : window1;
                                   break;
                              case DIBI_RIGHT:
                                   quit = DIKS_DOWN;
                                   break;
                              default:
                                   break;
                         }
                         break;

                    case DWET_KEYDOWN:
                         if (grabbed)
                              break;
                         switch (evt.key_id) {
                              case DIKI_RIGHT:
                                   active->Move (active, 1, 0);
                                   break;
                              case DIKI_LEFT:
                                   active->Move (active, -1, 0);
                                   break;
                              case DIKI_UP:
                                   active->Move (active, 0, -1);
                                   break;
                              case DIKI_DOWN:
                                   active->Move (active, 0, 1);
                                   break;
                              default:
                                   break;
                         }
                         break;

                    case DWET_LOSTFOCUS:
                         if (!grabbed && active == window)
                              active = NULL;
                         break;

                    default:
                         break;

                    }
               }

               switch (evt.type) {

               case DWET_MOTION:
                    endx = evt.cx;
                    endy = evt.cy;
                    winx = evt.x;
                    winy = evt.y;
                    winupdate = 1;
                    break;

               case DWET_KEYDOWN:
                    switch (evt.key_symbol) {
                    case DIKS_ESCAPE:
                    case DIKS_SMALL_Q:
                    case DIKS_CAPITAL_Q:
                    case DIKS_BACK:
                    case DIKS_STOP:
                         quit = 1;
                         break;
                    case DIKS_SMALL_R:
                         if (active) {
                              rotation = (rotation + 90) % 360;

#ifdef DIRECTFB_ROTATE
                              active->SetRotation( active, rotation );
#endif
                         }
                         break;
                    default:
                         break;
                    }
                    break;

               default:
                    break;
               }
          }

          if (video_provider)
               window_surface2->Flip( window_surface2, NULL, 0 );

          if (active) {
               if (grabbed == 1) {
                    active->Move( active, endx - startx, endy - starty);
                    startx = endx;
                    starty = endy;
               }
               else if (grabbed == 2) {
                    active->SetOpacity( active, (myclock() % 2551) /10 );
               }
               else if (winupdate) {
                    char buf[32];
                    DFBRectangle rect;
                    DFBRegion    region;

                    snprintf( buf, sizeof(buf), "x/y: %4d,%4d", winx, winy );

                    font->GetStringExtents( font, buf, -1, &rect, NULL );

                    rect.x = 1;
                    rect.y = 1;

                    rect.w += rect.w / 3;
                    rect.h += 10;

                    window_surface1->SetColor( window_surface1, 0x10, 0x10, 0x10, 0x77 );
                    window_surface1->FillRectangles( window_surface1, &rect, 1 );

                    window_surface1->SetColor( window_surface1, 0x88, 0xCC, 0xFF, 0xAA );
                    window_surface1->DrawString( window_surface1, buf, -1, rect.h/4, 5, DSTF_TOPLEFT );


                    region = DFB_REGION_INIT_FROM_RECTANGLE( &rect );

                    window_surface1->Flip( window_surface1, &region, 0 );

                    winupdate = 0;
               }
          }
     }

     if (video_provider)
          video_provider->Release( video_provider );

     buffer->Release( buffer );
     font->Release( font );
     window_surface2->Release( window_surface2 );
     window_surface1->Release( window_surface1 );
     window2->Release( window2 );
     window1->Release( window1 );
     layer->Release( layer );
     bgsurface->Release( bgsurface );
     dfb->Release( dfb );

     return 42;
}
