/*
   (c) Copyright 2006-2007  directfb.org

   All rights reserved.

   Written by Denis Oliver Kropp <dok@directfb.org>.

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

//#define DIRECT_ENABLE_DEBUG

#include <config.h>

#include <unistd.h>

#include <direct/debug.h>
#include <direct/list.h>

#include <fusion/conf.h>
#include <fusion/fusion.h>
#include <fusion/shmalloc.h>

#include <core/layer_context.h>
#include <core/layer_control.h>
#include <core/layer_region.h>
#include <core/palette.h>
#include <core/screen.h>
#include <core/windows_internal.h>
#include <core/windowstack.h>
#include <core/wm.h>

#include <gfx/clip.h>
#include <gfx/convert.h>
#include <gfx/util.h>

#include <misc/conf.h>

#include <sawman.h>

#include "sawman_config.h"
#include "sawman_draw.h"
#include "sawman_window.h"

#include "isawman.h"

#include "region.h"


D_DEBUG_DOMAIN( SaWMan_Auto,     "SaWMan/Auto",     "SaWMan auto configuration" );
D_DEBUG_DOMAIN( SaWMan_Update,   "SaWMan/Update",   "SaWMan window manager updates" );
D_DEBUG_DOMAIN( SaWMan_FlipOnce, "SaWMan/FlipOnce", "SaWMan window manager flip once" );
D_DEBUG_DOMAIN( SaWMan_Surface,  "SaWMan/Surface",  "SaWMan window manager surface" );
D_DEBUG_DOMAIN( SaWMan_Focus,    "SaWMan/Focus",    "SaWMan window manager focus" );

/**********************************************************************************************************************/

typedef struct {
     DFBRegion     region;
     int           cap;
     int           size;
     int           curr;
     SaWManWindow *windows;
     /* hidden array of SaWManWindow* at the end of this struct */
} DFBUpdateBin;

static inline DFBUpdateBin *dfb_update_bin_get( const DFBUpdateBin *src, SaWManWindow *window, const int x1, const int y1, const int x2, const int y2, const int max)
{
    int           cap  = max;
    int           size = 0;
    DFBUpdateBin *bin;


    if (src) {
         cap = src->cap;
         size = src->size;
    }

     D_ASSERT(cap > 0);

     bin = malloc( sizeof(DFBUpdateBin) + (cap - 1) * sizeof(SaWManWindow*) );
     D_ASSERT(NULL != bin);

     bin->region = (DFBRegion){ x1, y1, x2, y2 };
     bin->size = size;
     bin->cap = cap;
     bin->curr = size;

     if (src && size > 0)
          memcpy(&bin->windows, &src->windows, size * sizeof(SaWManWindow*));

     if (window) {
          D_ASSERT(size < cap);

          memcpy(&bin->windows + size, &window, sizeof(SaWManWindow*));
          bin->size++;
          bin->curr++;
     }




     return bin;
}

static inline SaWManWindow *dfb_update_bin_window_get( const DFBUpdateBin *bin, const int index )
{
     D_ASSERT(NULL != bin);
     D_ASSERT(index >= 0);
     D_ASSERT(index < bin->size);


     return *(&bin->windows + index);
}

static void
update_region4_r( SaWMan          *sawman,
                  SaWManTier      *tier,
                  CardState       *state,
                  int              start,
                  bool             right_eye,
                  DFBUpdateBin    *bin )
{
     int           i      = start;
     CoreWindow   *window = NULL;
     SaWManWindow *sawwin = NULL;
     DFBRegion     region;
     int           offset;


     D_DEBUG_AT( SaWMan_Update, "%s( %p, %d, %d,%d - %d,%d )\n", __FUNCTION__, tier, start, region.x1, region.y1, region.x2, region.y2 );


     D_MAGIC_ASSERT( sawman, SaWMan );
     D_MAGIC_ASSERT( tier, SaWManTier );
     D_MAGIC_ASSERT( state, CardState );
     D_ASSERT( start < fusion_vector_size( &sawman->layout ) );

     /* find next intersecting window */
     while (i >= 0) {
          sawwin = fusion_vector_at( &sawman->layout, i );

          D_MAGIC_ASSERT( sawwin, SaWManWindow );

          window = sawwin->window;
          D_MAGIC_COREWINDOW_ASSERT( window );

          /* modify stereo offset */
          offset = window->config.z;
          offset *= right_eye ? -1 : 1;
          region = DFB_REGION_INIT_FROM_RECTANGLE(&sawwin->bounds);
          region.x1 += offset;
          region.x2 += offset;

          if (SAWMAN_VISIBLE_WINDOW( window ) && (tier->classes & (1 << window->config.stacking))) {
              if (dfb_region_intersect( &region, bin->region.x1, bin->region.y1, bin->region.x2, bin->region.y2))
                    break;
          }

               i--;
     }

         /* intersecting window found? */
     if (i >= 0) {
          D_MAGIC_ASSERT( sawwin, SaWManWindow );
          D_MAGIC_COREWINDOW_ASSERT( window );

               /* continue recursion with left intersection? */
          if (bin->region.x1 != region.x1)
               update_region4_r( sawman, tier, state, i - 1, right_eye, dfb_update_bin_get( bin, NULL, bin->region.x1, region.y1, region.x1 - 1, region.y2, i + 1) );

               /* continue recursion with upper intersection? */
          if (bin->region.y1 != region.y1)
               update_region4_r( sawman, tier, state, i - 1, right_eye, dfb_update_bin_get( bin, NULL, bin->region.x1, bin->region.y1, bin->region.x2, region.y1 - 1, i + 1) );


               /* continue recursion with right intersection? */
          if (bin->region.x2 != region.x2)
               update_region4_r( sawman, tier, state, i - 1, right_eye, dfb_update_bin_get( bin, NULL, region.x2 + 1, region.y1, bin->region.x2, region.y2, i + 1) );

               /* continue recursion with lower intersection? */
          if (bin->region.y2 != region.y2)
               update_region4_r( sawman, tier, state, i - 1, right_eye, dfb_update_bin_get( bin, NULL, bin->region.x1, region.y2 + 1, bin->region.x2, bin->region.y2, i + 1) );


          if (D_FLAGS_ARE_SET( window->config.options, DWOP_OPAQUE_REGION )) {
               DFBRegion opaque = DFB_REGION_INIT_TRANSLATED( &window->config.opaque, sawwin->bounds.x, sawwin->bounds.y );

               /* continue recursion with left inner window intersection? */
          if (opaque.x1 != region.x1)
               update_region4_r( sawman, tier, state, i - 1, right_eye, dfb_update_bin_get( bin, sawwin, region.x1, opaque.y1, opaque.x1 - 1, opaque.y2, i + 1) );

              /* continue recursion with upper inner window intersection? */
          if (opaque.y1 != region.y1)
               update_region4_r( sawman, tier, state, i - 1, right_eye, dfb_update_bin_get( bin, sawwin, region.x1, region.y1, region.x2, opaque.y1 - 1, i + 1) );


             /* continue recursion with right inner window intersection? */
          if (opaque.x2 != region.x2)
                update_region4_r( sawman, tier, state, i - 1, right_eye, dfb_update_bin_get( bin, sawwin, opaque.x2 + 1, opaque.y1, region.x2, opaque.y2, i + 1) );


            /* continue recursion with lower inner window intersection? */
          if (opaque.y2 != region.y2)
               update_region4_r( sawman, tier, state, i - 1, right_eye, dfb_update_bin_get( bin, sawwin, region.x1, opaque.y2 + 1, region.x2, region.y2, i + 1) );

              /* recursion ends on opaque inner window */
              update_region4_r( sawman, tier, state, -1, right_eye, dfb_update_bin_get( bin, sawwin, opaque.x1, opaque.y1, opaque.x2, opaque.y2, i + 1) );
         }
         else if (SAWMAN_TRANSLUCENT_WINDOW( window )) {
              /* continue recursion on window */
              update_region4_r( sawman, tier, state, i - 1, right_eye, dfb_update_bin_get( bin, sawwin, region.x1, region.y1, region.x2, region.y2, i + 1) );
         }
         else {
              /* recursion ends on opaque window */
              update_region4_r( sawman, tier, state, -1, right_eye, dfb_update_bin_get( bin, sawwin, region.x1, region.y1, region.x2, region.y2, i + 1) );
          }

         free( bin );
     }
     else {
          /* recursion already ended */
          if (bin->size < 1) {
               sawman_draw_background( tier, state, &bin->region );

               free( bin );
          }

          else {
              CoreWindow      *window  = dfb_update_bin_window_get(bin, bin->curr - 1)->window;
              CoreWindowStack *stack   = tier->stack;
              bool             trans   = SAWMAN_TRANSLUCENT_WINDOW(window);
              bool             premult = (window->config.opacity == 0xff && window->surface &&
                                          (window->surface->config.caps & DSCAPS_PREMULTIPLIED) &&
                                          (window->config.options & DWOP_ALPHACHANNEL) && !(window->config.options & DWOP_COLORKEYING) &&
                                          (window->config.dst_geometry.mode == DWGM_DEFAULT) && stack->bg.mode == DLBM_COLOR &&
                                          !stack->bg.color.a && !stack->bg.color.r && !stack->bg.color.g && !stack->bg.color.b);
              D_ASSERT(bin->curr > 0);
              D_ASSERT(bin->size > 0);

              /* draw background behind translucient windows */
              if (bin->curr == bin->size && SAWMAN_TRANSLUCENT_WINDOW( window )) {
                   sawman_draw_background( tier, state, &bin->region );

               }
               /* current window opaque or premultiplied with black background? */
               if (!trans || (premult && (bin->size == bin->curr))) {
                    int next = bin->curr - 1;
                    if (next > 0) {
                         /* there is another window on top so can blit 2 windows at once*/
                         sawman_draw_two_windows( tier, dfb_update_bin_window_get(bin, bin->curr - 1), dfb_update_bin_window_get(bin, bin->curr - 2), state, &bin->region, right_eye );

                         bin->curr -= 2;

                    }
                    else {
                         /* single not blended blit */
                         sawman_draw_window( tier, dfb_update_bin_window_get(bin, bin->curr - 1), state, &bin->region, false, right_eye );

                         bin->curr--;
                    }
                    /* continue with next window on top? */
                    if (bin->curr >= 1)
                         update_region4_r( sawman, tier, state, -1, right_eye, bin );
                    else
                        free( bin );
               }
               else {
                   /* single blend */
                   sawman_draw_window( tier, dfb_update_bin_window_get(bin, bin->curr - 1), state, &bin->region, true, right_eye );

               /* continue with window on top? */
                   if (bin->curr > 1) {
                        bin->curr--;
                        update_region4_r( sawman, tier, state, -1, right_eye, bin );
                   }
                   else {
                        free( bin );
                   }
              }
         }
     }
}

static void
update_region4( SaWMan          *sawman,
                SaWManTier      *tier,
                CardState       *state,
                int              start,
                int              x1,
                int              y1,
                int              x2,
                int              y2,
                bool             right_eye )
{
     D_MAGIC_ASSERT( sawman, SaWMan );
     D_MAGIC_ASSERT( tier, SaWManTier );
     D_MAGIC_ASSERT( state, CardState );
     D_ASSERT( start < fusion_vector_size( &sawman->layout ) );
     D_ASSERT( x1 <= x2 );
     D_ASSERT( y1 <= y2 );


          if (start < 0) {
             DFBRegion region = { x1, y1, x2, y2 };


          sawman_draw_background( tier, state, &region);
     }
     else {
          update_region4_r( sawman, tier, state, start, right_eye, dfb_update_bin_get( NULL, NULL, x1, y1, x2, y2, start + 1) );
     }

}

void
sawman_flush_updating( SaWMan     *sawman,
                       SaWManTier *tier )
{
     int                 i;
     DFBSurfaceStereoEye old_eye;
     int                 left_num_regions  = 0;
     int                 right_num_regions = 0;

     D_DEBUG_AT( SaWMan_Surface, "%s( %p, %p )\n", __FUNCTION__, sawman, tier );

     D_MAGIC_ASSERT( sawman, SaWMan );
     D_MAGIC_ASSERT( tier, SaWManTier );

     old_eye = dfb_surface_get_stereo_eye( tier->region->surface );



     D_ASSUME( tier->left.updating.num_regions > 0 || tier->right.updating.num_regions > 0 );
     D_ASSUME( tier->left.updated.num_regions == 0 && tier->right.updated.num_regions == 0 );

     if (tier->left.updating.num_regions) {
          /*
           * save here as it might get reset in case surface reaction
           * is called synchronously during dfb_layer_region_flip_update()
           */
          left_num_regions = tier->left.updating.num_regions;

          D_DEBUG_AT( SaWMan_Surface, "  -> making updated = updating\n" );

          /* Make updated = updating */
          direct_memcpy( &tier->left.updated, &tier->left.updating, sizeof(DFBUpdates) );
          direct_memcpy( &tier->left.updated_regions[0], &tier->left.updating_regions[0], sizeof(DFBRegion) * tier->left.updating.num_regions );
          tier->left.updated.regions = &tier->left.updated_regions[0];

          D_DEBUG_AT( SaWMan_Surface, "  -> clearing updating\n" );

          /* Clear updating */
          dfb_updates_reset( &tier->left.updating );
     }

     if (tier->right.updating.num_regions) {
          /*
           * save here as it might get reset in case surface reaction
           * is called synchronously during dfb_layer_region_flip_update()
           */
          right_num_regions = tier->right.updating.num_regions;

          D_DEBUG_AT( SaWMan_Surface, "  -> making updated = updating\n" );

          /* Make updated = updating */
          direct_memcpy( &tier->right.updated, &tier->right.updating, sizeof(DFBUpdates) );
          direct_memcpy( &tier->right.updated_regions[0], &tier->right.updating_regions[0], sizeof(DFBRegion) * tier->right.updating.num_regions );
          tier->right.updated.regions = &tier->right.updated_regions[0];

          D_DEBUG_AT( SaWMan_Surface, "  -> clearing updating\n" );

          /* Clear updating */
          dfb_updates_reset( &tier->right.updating );
     }


     D_DEBUG_AT( SaWMan_Surface, "  -> flipping the region\n" );

     /* Flip the whole layer. */
     if (tier->region->config.options & DLOP_STEREO)
          dfb_layer_region_flip_update_stereo( tier->region,
                                               &tier->left.updated.bounding,
                                               &tier->right.updated.bounding, DSFLIP_ONSYNC | DSFLIP_SWAP );
     else
          dfb_layer_region_flip_update( tier->region, &tier->left.updated.bounding, DSFLIP_ONSYNC | DSFLIP_SWAP );


     if (left_num_regions) {
          D_DEBUG_AT( SaWMan_Surface, "  -> copying %d updated regions (F->B)\n", left_num_regions );

          for (i=0; i<left_num_regions; i++) {
               D_DEBUG_AT( SaWMan_Surface, "    -> %4d,%4d - %4dx%4d  [%d]\n",
                           DFB_RECTANGLE_VALS_FROM_REGION( &tier->left.updated.regions[i] ), i );
          }

          /* Copy back the updated region .*/
          dfb_surface_set_stereo_eye( tier->region->surface, DSSE_LEFT );

          dfb_gfx_copy_regions( tier->surface, CSBR_FRONT, tier->surface, CSBR_BACK,
                                tier->left.updated.regions, left_num_regions, 0, 0 );
     }

     if (right_num_regions) {
          D_DEBUG_AT( SaWMan_Surface, "  -> copying %d updated regions (F->B)\n", right_num_regions );

          for (i=0; i<right_num_regions; i++) {
               D_DEBUG_AT( SaWMan_Surface, "    -> %4d,%4d - %4dx%4d  [%d]\n",
                           DFB_RECTANGLE_VALS_FROM_REGION( &tier->right.updated.regions[i] ), i );
          }

          /* Copy back the updated region .*/
          dfb_surface_set_stereo_eye( tier->region->surface, DSSE_RIGHT );

          dfb_gfx_copy_regions( tier->surface, CSBR_FRONT, tier->surface, CSBR_BACK,
                                tier->right.updated.regions, right_num_regions, 0, 0 );
     }


     dfb_surface_set_stereo_eye( tier->region->surface, old_eye );
}

static void
repaint_tier( SaWMan              *sawman,
              SaWManTier          *tier,
              const DFBRegion     *updates,
              int                  num_updates,
              DFBSurfaceFlipFlags  flags,
              bool                 right_eye )
{
     int                  i;
     CoreLayer           *layer;
     CoreLayerRegion     *region;
     CardState           *state;
     CoreSurface         *surface;
     DFBRegion            cursor_inter;
     CoreWindowStack     *stack;
     DFBSurfaceStereoEye  old_eye, eye;

     D_MAGIC_ASSERT( sawman, SaWMan );
     D_MAGIC_ASSERT( tier, SaWManTier );
     D_ASSERT( updates != NULL );
//     D_ASSERT( num_updates > 0 );
     FUSION_SKIRMISH_ASSERT( sawman->lock );

     if (num_updates == 0)
          return;

     stack = tier->stack;
     D_ASSERT( stack != NULL );

     region = tier->region;
     D_ASSERT( region != NULL );

     layer   = dfb_layer_at( tier->layer_id );
     state   = &layer->state;
     surface = region->surface;

     if (/*!data->active ||*/ !surface)
          return;

     D_DEBUG_AT( SaWMan_Update, "%s( %p, %p )\n", __FUNCTION__, sawman, tier );

     {
          SaWManListenerCallData data;

          data.call        = SWMLC_TIER_UPDATE;
          data.stereo_eye  = right_eye ? DSSE_RIGHT : DSSE_LEFT;
          data.layer_id    = tier->layer_id;
          data.num_updates = num_updates;

          D_ASSERT( num_updates <= D_ARRAY_SIZE(data.updates) );

          direct_memcpy( data.updates, updates, num_updates * sizeof(DFBRegion) );

          fusion_reactor_dispatch( sawman->reactor, &data, true, NULL );
     }

     /* Set stereo eye */
     old_eye = dfb_surface_get_stereo_eye( surface );
     eye = right_eye ? DSSE_RIGHT : DSSE_LEFT;
     dfb_surface_set_stereo_eye( surface, eye );

     for (i=0; i<num_updates; i++) {
          const DFBRegion *update = &updates[i];

          DFB_REGION_ASSERT( update );

          D_DEBUG_AT( SaWMan_Update, "  -> %d, %d - %dx%d  (%d)\n",
                      DFB_RECTANGLE_VALS_FROM_REGION( update ), i );

          /* Set destination. */
          state->destination  = surface;
          state->modified    |= SMF_DESTINATION;

          if (!DFB_PLANAR_PIXELFORMAT(region->config.format))
               dfb_state_set_dst_colorkey( state, dfb_color_to_pixel( region->config.format,
                                                                      region->config.src_key.r,
                                                                      region->config.src_key.g,
                                                                      region->config.src_key.b ) );
          else
               dfb_state_set_dst_colorkey( state, 0 );

          /* Set clipping region. */
          dfb_state_set_clip( state, update );

          /* Compose updated region. */
          update_region4( sawman, tier, state,
                          fusion_vector_size( &sawman->layout ) - 1,
                          update->x1, update->y1, update->x2, update->y2,
                          right_eye );

          /* Update cursor? */
          cursor_inter = tier->cursor_region;
          if (tier->cursor_drawn && dfb_region_region_intersect( &cursor_inter, update )) {
               int          x, y;
               DFBRectangle rect = DFB_RECTANGLE_INIT_FROM_REGION( &cursor_inter );

               D_ASSUME( tier->cursor_bs_valid );

               dfb_gfx_copy_to( surface, tier->cursor_bs, &rect,
                                rect.x - tier->cursor_region.x1,
                                rect.y - tier->cursor_region.y1, true );

               x = (s64) stack->cursor.x * (s64) tier->size.w / (s64) sawman->resolution.w;
               y = (s64) stack->cursor.y * (s64) tier->size.h / (s64) sawman->resolution.h;

               sawman_draw_cursor( stack, state, surface, &cursor_inter, x, y );
          }
     }

     /* Reset destination. */
     state->destination  = NULL;
     state->modified    |= SMF_DESTINATION;

     /* Restore stereo eye */
     dfb_surface_set_stereo_eye( surface, old_eye );
}

static SaWManWindow *
get_single_window( SaWMan     *sawman,
                   SaWManTier *tier,
                   bool       *ret_none )
{
     int           n;
     SaWManWindow *sawwin;
     SaWManWindow *single = NULL;
     SaWManTier   *tier0  = (SaWManTier*) sawman->tiers;

     D_MAGIC_ASSERT( sawman, SaWMan );
     D_MAGIC_ASSERT( tier, SaWManTier );
     FUSION_SKIRMISH_ASSERT( sawman->lock );

     D_MAGIC_ASSERT( tier0, SaWManTier );

     if (tier0->stack->cursor.enabled && tier0->stack->cursor.opacity) {
          *ret_none = false;

          return NULL;
     }

     fusion_vector_foreach_reverse (sawwin, n, sawman->layout) {
          CoreWindow *window;

          D_MAGIC_ASSERT( sawwin, SaWManWindow );

          window = sawwin->window;
          D_MAGIC_COREWINDOW_ASSERT( window );

          if (SAWMAN_VISIBLE_WINDOW(window) && (tier->classes & (1 << window->config.stacking))) {
               if (sawman_config->static_layer) {
                    *ret_none = false;
                    return NULL;
               }

               if (      single
                    || ( window->caps & (DWCAPS_INPUTONLY | DWCAPS_COLOR) )
                    || ( window->config.options & DWOP_INPUTONLY ) )
                    return NULL;

               single = sawwin;

               if (single->dst.x == 0 &&
                   single->dst.y == 0 &&
                   single->dst.w == tier->size.w &&
                   single->dst.h == tier->size.h &&
                   !SAWMAN_TRANSLUCENT_WINDOW(window))
                    break;
          }
     }

     if (ret_none && !single)
          *ret_none = true;

     return single;
}

static bool
get_border_only( SaWMan     *sawman,
                 SaWManTier *tier )
{
     int           n;
     SaWManWindow *sawwin;
     bool          none = true;

     D_MAGIC_ASSERT( sawman, SaWMan );
     D_MAGIC_ASSERT( tier, SaWManTier );
     FUSION_SKIRMISH_ASSERT( sawman->lock );

     fusion_vector_foreach_reverse (sawwin, n, sawman->layout) {
          CoreWindow *window;

          D_MAGIC_ASSERT( sawwin, SaWManWindow );

          window = sawwin->window;
          D_MAGIC_COREWINDOW_ASSERT( window );

          none = false;

          if (     SAWMAN_VISIBLE_WINDOW(window)
              && !(window->caps & DWCAPS_INPUTONLY)
              && !(window->config.options & DWOP_INPUTONLY) )
               return false;
     }

     return !none;
}

static bool
windows_updating( SaWMan     *sawman,
                  SaWManTier *tier )
{
     int           i;
     SaWManWindow *window;

     D_MAGIC_ASSERT( sawman, SaWMan );
     D_MAGIC_ASSERT( tier, SaWManTier );

     fusion_vector_foreach (window, i, sawman->layout) {
          D_MAGIC_ASSERT( window, SaWManWindow );

          if (window->flags & SWMWF_UPDATING) {
               long long diff = direct_clock_get_millis() - window->update_ms;

               if (!sawman_config->flip_once_timeout || diff < sawman_config->flip_once_timeout) {
                    D_DEBUG_AT( SaWMan_FlipOnce, "  -> update blocking on window id %u (%lld ms, flags 0x%08x)\n",
                                window->id, diff, window->flags );

                    return true;
               }

               D_DEBUG_AT( SaWMan_FlipOnce, "  -> ignoring blocking of window id %u (%lld ms, flags 0x%08x)\n",
                           window->id, diff, window->flags );
          }
     }

     D_DEBUG_AT( SaWMan_FlipOnce, "  -> update not blocked by any window\n" );

     return false;
}

static DFBResult
process_single( SaWMan              *sawman,
                SaWManTier          *tier,
                SaWManWindow        *single,
                DFBSurfaceFlipFlags  flags )
{
     DFBResult                   ret;
     CoreWindow                 *window;
     CoreSurface                *surface;
     DFBDisplayLayerOptions      options = DLOP_NONE;
     DFBRectangle                dst  = single->dst;
     DFBRectangle                src  = single->src;
     DFBRegion                   clip = DFB_REGION_INIT_FROM_DIMENSION( &tier->size );
     CoreLayer                  *layer;
     CoreLayerShared            *shared;
     CoreLayerRegion            *region;
     int                         screen_width;
     int                         screen_height;
     DFBColorKey                 single_key;
     const DisplayLayerFuncs    *funcs;
     CoreLayerRegionConfigFlags  failed;

     D_MAGIC_ASSERT( sawman, SaWMan );
     D_MAGIC_ASSERT( tier, SaWManTier );
     D_MAGIC_ASSERT( single, SaWManWindow );

     layer = dfb_layer_at( tier->layer_id );
     D_ASSERT( layer != NULL );
     D_ASSERT( layer->funcs != NULL );
     D_ASSERT( layer->funcs->TestRegion != NULL );

     funcs = layer->funcs;

     shared = layer->shared;
     D_ASSERT( shared != NULL );

     region = tier->region;
     D_ASSERT( region != NULL );

     dfb_screen_get_screen_size( layer->screen, &screen_width, &screen_height );

     if (shared->description.caps & DLCAPS_SCREEN_LOCATION) {
          dst.x = dst.x * screen_width  / tier->size.w;
          dst.y = dst.y * screen_height / tier->size.h;
          dst.w = dst.w * screen_width  / tier->size.w;
          dst.h = dst.h * screen_height / tier->size.h;
     }
     else {
          if (dst.w != src.w || dst.h != src.h)
               return DFB_UNSUPPORTED;

          if (shared->description.caps & DLCAPS_SCREEN_POSITION) {
               dfb_rectangle_intersect_by_region( &dst, &clip );

               src.x += dst.x - single->dst.x;
               src.y += dst.y - single->dst.y;
               src.w  = dst.w;
               src.h  = dst.h;

               dst.x += (screen_width  - tier->size.w) / 2;
               dst.y += (screen_height - tier->size.h) / 2;
          }
     }

#ifdef SAWMAN_NO_LAYER_DOWNSCALE
     if (dst.w < src.w)
          return DFB_UNSUPPORTED;
#endif

#ifdef SAWMAN_NO_LAYER_DST_WINDOW
     if (dst.x != 0 || dst.y != 0 || dst.w != screen_width || dst.h != screen_height)
          return DFB_UNSUPPORTED;
#endif


     window = single->window;
     D_MAGIC_COREWINDOW_ASSERT( window );

     surface = window->surface;
     D_ASSERT( surface != NULL );

     if (window->config.options & DWOP_ALPHACHANNEL)
          options |= DLOP_ALPHACHANNEL;

     if (window->caps & DWCAPS_LR_MONO ||
         window->caps & DWCAPS_STEREO)
          options |= DLOP_STEREO;

     if (window->config.options & DWOP_COLORKEYING)
          options |= DLOP_SRC_COLORKEY;

     single_key = tier->single_key;

     if (DFB_PIXELFORMAT_IS_INDEXED( surface->config.format )) {
          CorePalette *palette = surface->palette;

          D_ASSERT( palette != NULL );
          D_ASSERT( palette->num_entries > 0 );

          dfb_surface_set_palette( tier->region->surface, surface->palette );

          if (options & DLOP_SRC_COLORKEY) {
               int index = window->config.color_key % palette->num_entries;

               single_key.r     = palette->entries[index].r;
               single_key.g     = palette->entries[index].g;
               single_key.b     = palette->entries[index].b;
               single_key.index = index;
          }
     }
     else {
          DFBColor color;

          dfb_pixel_to_color( surface->config.format, window->config.color_key, &color );

          single_key.r     = color.r;
          single_key.g     = color.g;
          single_key.b     = color.b;
          single_key.index = window->config.color_key;
     }

     /* Complete reconfig? */
     if (tier->single_window  != single ||
         !DFB_RECTANGLE_EQUAL( tier->single_src, src ) ||
         tier->single_format  != surface->config.format ||
         tier->single_options != options)
     {
          DFBDisplayLayerConfig  config;
          DFBSurfaceStereoEye    window_eye, region_eye;
          CoreLayerRegionConfig  region_config;

          D_DEBUG_AT( SaWMan_Auto, "  -> Switching to %dx%d [%dx%d] %s single mode for %p on %p...\n",
                      single->src.w, single->src.h, src.w, src.h,
                      dfb_pixelformat_name( surface->config.format ), single, tier );

          config.flags        = DLCONF_WIDTH | DLCONF_HEIGHT | DLCONF_PIXELFORMAT | DLCONF_OPTIONS |
                                DLCONF_BUFFERMODE | DLCONF_SURFACE_CAPS;
          config.width        = src.w;
          config.height       = src.h;
          config.pixelformat  = surface->config.format;
          config.options      = options;
          config.buffermode   = DLBM_FRONTONLY;
          config.surface_caps = tier->context->config.surface_caps | (options & DLOP_STEREO ? DSCAPS_STEREO : 0);

          sawman->callback.layer_reconfig.layer_id = tier->layer_id;
          sawman->callback.layer_reconfig.single   = (SaWManWindowHandle) single;
          sawman->callback.layer_reconfig.config   = config;

          switch (sawman_call( sawman, SWMCID_LAYER_RECONFIG,
                               &sawman->callback.layer_reconfig, sizeof(sawman->callback.layer_reconfig), true ))
          {
               case DFB_OK:
                    config = sawman->callback.layer_reconfig.config;
               case DFB_NOIMPL:
                    /* continue, no change demanded */
                    break;

               default:
                    return DFB_UNSUPPORTED;
          }

          config = sawman->callback.layer_reconfig.config;

          region_config = tier->region->config;

          region_config.width = config.width;
          region_config.height = config.height;
          region_config.format = config.pixelformat;
          region_config.options = config.options;
          region_config.buffermode = config.buffermode;
          region_config.surface_caps = config.surface_caps;
          region_config.src_key = tier->single_key;
          region_config.dest = dst;
          region_config.source = src;

          /* Let the driver examine the modified configuration. */
          ret = funcs->TestRegion( layer, layer->driver_data, layer->layer_data,
                                   &region_config, &failed );
          if (ret)
               return ret;

          tier->single_mode     = true;
          tier->single_window   = single;
          tier->single_width    = src.w;
          tier->single_height   = src.h;
          tier->single_src      = src;
          tier->single_dst      = dst;
          tier->single_format   = surface->config.format;
          tier->single_options  = options;
          tier->single_key      = single_key;

          tier->active          = false;
          tier->region->state  |= CLRSF_FROZEN;

          dfb_updates_reset( &tier->left.updates );
          dfb_updates_reset( &tier->right.updates );


          /* Lock the context. */
          if (dfb_layer_context_lock( tier->context ))
               return DFB_FUSION;

          dfb_layer_region_lock(region);

          if (region->surface) {
               if (D_FLAGS_IS_SET( region->state, CLRSF_ENABLED ))
                    dfb_layer_region_disable(region);

               ret = dfb_layer_context_reallocate_surface( layer, region, &region_config );
               if (ret)
                    D_DERROR( ret, "Core/Layers: Reallocation of layer surface failed!\n" );
          }
          else {
               ret = dfb_layer_context_allocate_surface( layer, region, &region_config );
               if (ret)
                    D_DERROR( ret, "Core/Layers: Allocation of layer surface failed!\n" );
          }

          dfb_layer_region_set_configuration( region, &region_config,
                                              CLRCF_WIDTH | CLRCF_HEIGHT | CLRCF_FORMAT | CLRCF_OPTIONS | CLRCF_FREEZE | CLRCF_SURFACE |
                                              CLRCF_BUFFERMODE | CLRCF_SURFACE_CAPS | CLRCF_SRCKEY | CLRCF_DEST | CLRCF_SOURCE );

          /* Enable the primary region. */
          if (! D_FLAGS_IS_SET( region->state, CLRSF_ENABLED ))
               dfb_layer_region_enable( region );

          dfb_layer_region_unlock(region);

          tier->context->primary.config = region_config;

          /* Unlock the context. */
          dfb_layer_context_unlock( tier->context );


          sawman_dispatch_blit( sawman, single, false, &single->src, &single->dst, NULL );

          // FIXME: put in function, same as below
          if (tier->single_options & DLOP_STEREO) {
               sawman_dispatch_blit( sawman, single, false, &single->src, &single->dst, NULL );


               window_eye = dfb_surface_get_stereo_eye(surface);
               region_eye = dfb_surface_get_stereo_eye(tier->region->surface);
               dfb_surface_set_stereo_eye(surface, DSSE_LEFT);
               dfb_surface_set_stereo_eye(tier->region->surface, DSSE_LEFT);
               dfb_gfx_copy_to( surface, tier->region->surface, &src, window->config.z, 0, false );

               if (window->caps & DWCAPS_STEREO) {
                    dfb_surface_set_stereo_eye(surface, DSSE_RIGHT);
               }
               dfb_surface_set_stereo_eye(tier->region->surface, DSSE_RIGHT);
               dfb_gfx_copy_to( surface, tier->region->surface, &src, -window->config.z, 0, false );

               dfb_surface_set_stereo_eye(surface, window_eye);
               dfb_surface_set_stereo_eye(tier->region->surface, region_eye);
          }
          else {
               dfb_gfx_copy_to( surface, tier->region->surface, &src, 0, 0, false );
          }


          tier->active = true;

          if (sawman->cursor.context && tier->context->layer_id == sawman->cursor.context->layer_id)
               dfb_layer_activate_context( dfb_layer_at(tier->context->layer_id), tier->context );

          if (tier->single_options & DLOP_STEREO) {
               dfb_layer_region_flip_update_stereo( tier->region, NULL, NULL, flags );
          }
          else {
               dfb_layer_region_flip_update( tier->region, NULL, flags );
          }

          dfb_updates_reset( &tier->left.updates );
          dfb_updates_reset( &tier->right.updates );

          if (1) {
               SaWManTierUpdate update;

               update.regions[0].x1 = 0;
               update.regions[0].y1 = 0;
               update.regions[0].x2 = tier->single_width  - 1;
               update.regions[0].y2 = tier->single_height - 1;

               update.num_regions   = 1;
               update.classes       = tier->classes;

               fusion_reactor_dispatch_channel( tier->reactor, SAWMAN_TIER_UPDATE, &update, sizeof(update), true, NULL );
          }

          return DFB_OK;
     }

     /* Update destination window */
     if (!DFB_RECTANGLE_EQUAL( tier->single_dst, dst )) {
          CoreLayerRegionConfig region_config;

          region_config.dest = dst;

          tier->single_dst = dst;

          D_DEBUG_AT( SaWMan_Auto, "  -> Changing single destination to %d,%d-%dx%d.\n",
                      DFB_RECTANGLE_VALS(&dst) );

          dfb_layer_region_set_configuration( region, &region_config, CLRCF_DEST );
     }
     else {
          if (tier->single_options & DLOP_STEREO) {
               DFBSurfaceStereoEye    window_eye, region_eye;

               window_eye = dfb_surface_get_stereo_eye(surface);
               region_eye = dfb_surface_get_stereo_eye(tier->region->surface);
               dfb_surface_set_stereo_eye(surface, DSSE_LEFT);
               dfb_surface_set_stereo_eye(tier->region->surface, DSSE_LEFT);
               dfb_gfx_copy_to( surface, tier->region->surface, &src, window->config.z, 0, false );

               if (window->caps & DWCAPS_STEREO) {
                    dfb_surface_set_stereo_eye(surface, DSSE_RIGHT);
               }
               dfb_surface_set_stereo_eye(tier->region->surface, DSSE_RIGHT);
               dfb_gfx_copy_to( surface, tier->region->surface, &src, -window->config.z, 0, false );

               dfb_surface_set_stereo_eye(surface, window_eye);
               dfb_surface_set_stereo_eye(tier->region->surface, region_eye);
          }
          else {
               dfb_gfx_copy_to( surface, tier->region->surface, &src, 0, 0, false );
          }
     }

     /* Update color key */
     if (!DFB_COLORKEY_EQUAL( single_key, tier->single_key )) {
          CoreLayerRegionConfig region_config;

          region_config.src_key = single_key;

          tier->single_key = single_key;

          D_DEBUG_AT( SaWMan_Auto, "  -> Changing single color key.\n" );

          dfb_layer_region_set_configuration( region, &region_config, CLRCF_SRCKEY );
     }

     tier->active = true;

     if (tier->single_options & DLOP_STEREO) {
          dfb_layer_region_flip_update_stereo( tier->region, NULL, NULL, flags );
     }
     else {
          dfb_layer_region_flip_update( tier->region, NULL, flags );
     }

     dfb_updates_reset( &tier->left.updates );
     dfb_updates_reset( &tier->right.updates );

     fusion_skirmish_notify( sawman->lock );

     return DFB_OK;
}

static DFBResult
set_config( SaWMan     *sawman,
            SaWManTier *tier )
{
     DFBResult              ret;
     DFBDisplayLayerConfig *config;
     CoreLayer             *layer;
     CoreLayerShared       *shared;
     int                    screen_width;
     int                    screen_height;

     D_MAGIC_ASSERT( sawman, SaWMan );
     D_MAGIC_ASSERT( tier, SaWManTier );

     layer = dfb_layer_at( tier->layer_id );
     D_ASSERT( layer != NULL );

     shared = layer->shared;
     D_ASSERT( shared != NULL );

     dfb_screen_get_screen_size( layer->screen, &screen_width, &screen_height );


     if (tier->border_only)
          config = &tier->border_config;
     else
          config = &tier->config;

     D_DEBUG_AT( SaWMan_Auto, "  -> Switching to %dx%d %s %s mode.\n", config->width, config->height,
                 dfb_pixelformat_name( config->pixelformat ), tier->border_only ? "border" : "standard" );

     sawman->callback.layer_reconfig.layer_id = tier->layer_id;
     sawman->callback.layer_reconfig.single   = SAWMAN_WINDOW_NONE;
     sawman->callback.layer_reconfig.config   = *config;
     ret = sawman_call( sawman, SWMCID_LAYER_RECONFIG,
                        &sawman->callback.layer_reconfig, sizeof(sawman->callback.layer_reconfig), true );

     /* on DFB_OK we try to overrule the default configuration */
     if ( !ret && !dfb_layer_context_test_configuration( tier->context, &(sawman->callback.layer_reconfig.config), NULL ) ) {
          *config = sawman->callback.layer_reconfig.config;
          D_DEBUG_AT( SaWMan_Auto, "  -> Overruled to %dx%d %s %s mode.\n", config->width, config->height,
                      dfb_pixelformat_name( config->pixelformat ), tier->border_only ? "border" : "standard" );
     }

     tier->active         = false;
     tier->region->state |= CLRSF_FROZEN;

     dfb_updates_reset( &tier->left.updates );
     dfb_updates_reset( &tier->right.updates );

     /* Temporarily to avoid configuration errors. */
     dfb_layer_context_set_screenposition( tier->context, 0, 0 );

     ret = dfb_layer_context_set_configuration( tier->context, config );
     if (ret) {
          D_DERROR( ret, "SaWMan/Auto: Switching to standard mode failed!\n" );
          /* fixme */
     }

     tier->size.w = config->width;
     tier->size.h = config->height;

     /* Notify application manager about new tier size if previous mode was single. */
     if (tier->single_mode)
          sawman_call( sawman, SWMCID_STACK_RESIZED, &tier->size, sizeof(tier->size), false );

     if (shared->description.caps & DLCAPS_SCREEN_LOCATION) {
          DFBRectangle full = { 0, 0, screen_width, screen_height };

          dfb_layer_context_set_screenrectangle( tier->context, &full );
     }
     else if (shared->description.caps & DLCAPS_SCREEN_POSITION) {
          dfb_layer_context_set_screenposition( tier->context,
                                                (screen_width  - config->width)  / 2,
                                                (screen_height - config->height) / 2 );
     }

     if (config->options & DLOP_SRC_COLORKEY) {
          if (DFB_PIXELFORMAT_IS_INDEXED( config->pixelformat )) {
               int          index;
               CoreSurface *surface;
               CorePalette *palette;

               surface = tier->region->surface;
               D_MAGIC_ASSERT( surface, CoreSurface );

               palette = surface->palette;
               D_ASSERT( palette != NULL );
               D_ASSERT( palette->num_entries > 0 );

               index = tier->key.index % palette->num_entries;

               dfb_layer_context_set_src_colorkey( tier->context,
                                                   palette->entries[index].r,
                                                   palette->entries[index].g,
                                                   palette->entries[index].b,
                                                   index );
          }
          else
               dfb_layer_context_set_src_colorkey( tier->context,
                                                   tier->key.r, tier->key.g, tier->key.b, tier->key.index );
     }

     return DFB_OK;
}

static DFBResult
process_updates( SaWMan              *sawman,
                 SaWManTier          *tier,
                 DFBSurfaceFlipFlags  flags )
{
     int              i, n, d;
     int              total;
     int              bounding;
     const DFBRegion *left_updates  = NULL;
     const DFBRegion *right_updates = NULL;
     unsigned int     left_num  = 0;
     unsigned int     right_num = 0;
     DFBRegion        full_tier_region = { 0, 0, tier->size.w - 1, tier->size.h - 1 };
     DFBRegion        left_united;
     DFBRegion        right_united;

     D_MAGIC_ASSERT( sawman, SaWMan );
     D_MAGIC_ASSERT( tier, SaWManTier );

     dfb_updates_stat( &tier->left.updates, &total, &bounding );
     dfb_updates_stat( &tier->right.updates, &n, &d );
     total += n;
     bounding += d;

     n = tier->left.updates.max_regions - tier->left.updates.num_regions + 1;
     n += tier->right.updates.max_regions - tier->right.updates.num_regions + 1;
     d = n + 1;

     /* Try to optimize updates. In buffer swapping modes we can save the copy by updating everything. */
     if ((total > tier->size.w * tier->size.h) ||
         (total > tier->size.w * tier->size.h * 3 / 5 && (tier->context->config.buffermode == DLBM_BACKVIDEO ||
                                                          tier->context->config.buffermode == DLBM_TRIPLE)))
     {
          left_updates = &full_tier_region;
          left_num     = 1;

          if (tier->region->config.options & DLOP_STEREO) {
               right_updates = &full_tier_region;
               right_num     = 1;
          }
     }
     else if (tier->left.updates.num_regions + tier->right.updates.num_regions < 2 || total < bounding * n / d) {
          left_updates = tier->left.updates.regions;
          left_num     = tier->left.updates.num_regions;

          if (tier->region->config.options & DLOP_STEREO) {
               right_updates = tier->right.updates.regions;
               right_num     = tier->right.updates.num_regions;
          }
     }
     else {
          left_updates = &tier->left.updates.bounding;
          left_num     = 1;

          if (tier->region->config.options & DLOP_STEREO) {
               right_updates = &tier->right.updates.bounding;
               right_num     = 1;
          }
     }

     if (left_num) {
          dfb_regions_unite( &left_united, left_updates, left_num );

          repaint_tier( sawman, tier, left_updates, left_num, flags, false );
     }

     if (right_num) {
          dfb_regions_unite( &right_united, right_updates, right_num );

          repaint_tier( sawman, tier, right_updates, right_num, flags, true );
     }


     switch (tier->region->config.buffermode) {
          case DLBM_TRIPLE:
               /* Add the updated region. */
               for (i=0; i<left_num; i++) {
                    const DFBRegion *update = &left_updates[i];

                    DFB_REGION_ASSERT( update );

                    D_DEBUG_AT( SaWMan_Surface, "  -> adding %d, %d - %dx%d  (%d) to updating (left)\n",
                                DFB_RECTANGLE_VALS_FROM_REGION( update ), i );

                    dfb_updates_add( &tier->left.updating, update );
               }

               for (i=0; i<right_num; i++) {
                    const DFBRegion *update = &right_updates[i];

                    DFB_REGION_ASSERT( update );

                    D_DEBUG_AT( SaWMan_Surface, "  -> adding %d, %d - %dx%d  (%d) to updating (right)\n",
                                DFB_RECTANGLE_VALS_FROM_REGION( update ), i );

                    dfb_updates_add( &tier->right.updating, update );
               }

               if (!tier->left.updated.num_regions && !tier->right.updated.num_regions)
                    sawman_flush_updating( sawman, tier );
               break;

          case DLBM_BACKVIDEO:
               if (tier->region->config.options & DLOP_STEREO) {
                    DFBSurfaceStereoEye old_eye = dfb_surface_get_stereo_eye( tier->region->surface );

                    /* Flip the whole region. */
                    dfb_layer_region_flip_update_stereo( tier->region, &left_united, &right_united, flags | DSFLIP_WAITFORSYNC | DSFLIP_SWAP );

                    /* Copy back the updated region. */
                    if (left_num) {
                         dfb_surface_set_stereo_eye( tier->region->surface, DSSE_LEFT );

                         dfb_gfx_copy_regions( tier->region->surface, CSBR_FRONT, tier->region->surface, CSBR_BACK, left_updates, left_num, 0, 0 );
                    }

                    if (right_num) {
                         dfb_surface_set_stereo_eye( tier->region->surface, DSSE_RIGHT );

                         dfb_gfx_copy_regions( tier->region->surface, CSBR_FRONT, tier->region->surface, CSBR_BACK, right_updates, right_num, 0, 0 );
                    }

                    dfb_surface_set_stereo_eye( tier->region->surface, old_eye );
               }
               else {
                    /* Flip the whole region. */
                    dfb_layer_region_flip_update( tier->region, &left_united, flags | DSFLIP_WAITFORSYNC | DSFLIP_SWAP );

                    /* Copy back the updated region. */
                    dfb_gfx_copy_regions( tier->region->surface, CSBR_FRONT, tier->region->surface, CSBR_BACK, left_updates, left_num, 0, 0 );
               }
               break;

          default:
               if (tier->region->config.options & DLOP_STEREO) {
                    DFBRegion left, right;

                    if (left_num) {
                         left = left_updates[0];

                         for (i=1; i< (int)left_updates; ++i)
                              dfb_region_region_union( &left, &left_updates[i] );
                    }

                    if (right_num) {
                         right = right_updates[0];

                         for (i=1; i< (int)right_updates; ++i)
                              dfb_region_region_union( &right, &right_updates[i] );
                    }

                    /* Flip the whole region. */
                    dfb_layer_region_flip_update_stereo( tier->region,
                                                         left_num  ? &left  : NULL,
                                                         right_num ? &right : NULL, flags | DSFLIP_WAITFORSYNC );
               }
               else {
                    /* Flip the updated region .*/
                    for (i=0; i<left_num; i++) {
                         const DFBRegion *update = &left_updates[i];

                         DFB_REGION_ASSERT( update );

                         dfb_layer_region_flip_update( tier->region, update, flags );
                    }
               }
               break;
     }

#ifdef SAWMAN_DUMP_TIER_FRAMES
     {
          DFBResult          ret;
          CoreSurfaceBuffer *buffer;

          D_MAGIC_ASSERT( tier->region->surface, CoreSurface );

          if (fusion_skirmish_prevail( &tier->region->surface->lock ) == DFB_OK) {
               if (tier->region->config.options & DLOP_STEREO) {
                    DFBSurfaceStereoEye eye;

                    eye = dfb_surface_get_stereo_eye( tier->region->surface );

                    dfb_surface_set_stereo_eye( tier->region->surface, DSSE_LEFT );
                    buffer = dfb_surface_get_buffer( tier->region->surface, CSBR_FRONT );
                    D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer );
                    ret = dfb_surface_buffer_dump( buffer, ".", "tier_left" );

                    dfb_surface_set_stereo_eye( tier->region->surface, DSSE_RIGHT );
                    buffer = dfb_surface_get_buffer( tier->region->surface, CSBR_FRONT );
                    D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer );
                    ret = dfb_surface_buffer_dump( buffer, ".", "tier_right" );

                    dfb_surface_set_stereo_eye( tier->region->surface, eye );
               }
               else {
                    buffer = dfb_surface_get_buffer( tier->region->surface, CSBR_FRONT );
                    D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer );

                    ret = dfb_surface_buffer_dump( buffer, ".", "tier" );
               }

               fusion_skirmish_dismiss( &tier->region->surface->lock );
          }
     }
#endif


     if (1) {
          SaWManTierUpdate update;

          direct_memcpy( &update.regions[0], left_updates, sizeof(DFBRegion) * left_num );

          update.num_regions = left_num;
          update.classes     = tier->classes;

          fusion_reactor_dispatch_channel( tier->reactor, SAWMAN_TIER_UPDATE, &update, sizeof(update), true, NULL );
     }


     dfb_updates_reset( &tier->left.updates );
     dfb_updates_reset( &tier->right.updates );

     fusion_skirmish_notify( sawman->lock );

     return DFB_OK;
}

/* FIXME: Split up in smaller functions and clean up things like forcing reconfiguration. */
DirectResult
sawman_process_updates( SaWMan              *sawman,
                        DFBSurfaceFlipFlags  flags )
{
     DirectResult  ret;
     int           idx = -1;
     SaWManTier   *tier;

     D_MAGIC_ASSERT( sawman, SaWMan );
     FUSION_SKIRMISH_ASSERT( sawman->lock );

     D_DEBUG_AT( SaWMan_Update, "%s( %p, 0x%08x )\n", __FUNCTION__, sawman, flags );

     if (sawman->focused_window_switched) {
          SaWManWindow *to = sawman->focused_window_to;

          D_DEBUG_AT( SaWMan_Focus, "  -> focused window switched to %p\n", to );

          if (to) {
               CoreWindow *window;

               window = to->window;
               D_MAGIC_ASSERT( window, CoreWindow );

               if (window->config.cursor_flags & DWCF_INVISIBLE) {
                    /* Update cursor */
                    sawman_window_apply_cursor_flags( sawman, to );
               }

               D_DEBUG_AT( SaWMan_Focus, "  -> cursor %p\n", window->cursor.surface );

               if (window->cursor.surface) {
                    D_DEBUG_AT( SaWMan_Focus, "  -> switching to window's cursor shape\n" );

                    dfb_windowstack_cursor_set_shape( window->stack, window->cursor.surface, window->cursor.hot_x, window->cursor.hot_y );
               }

               if (!(window->config.cursor_flags & DWCF_INVISIBLE)) {
                    /* Update cursor */
                    sawman_window_apply_cursor_flags( sawman, to );
               }
          }
          else if (dfb_config->no_focus_hides_cursor) {
               SaWManTier *tier = (SaWManTier*) sawman->tiers;

               D_MAGIC_ASSERT( tier, SaWManTier );

               if (tier->stack->cursor.opacity) {
                    D_DEBUG_AT( SaWMan_Focus, "  -> hiding cursor...\n" );

                    tier->stack->cursor.opacity = 0;

                    dfb_wm_update_cursor( tier->stack, CCUF_OPACITY );
               }
               else
                    D_DEBUG_AT( SaWMan_Focus, "  -> cursor already hidden\n" );
          }
          else
               D_DEBUG_AT( SaWMan_Focus, "  -> keeping cursor of previously hidden window\n" );

          sawman->focused_window_switched = false;
     }

     direct_list_foreach (tier, sawman->tiers) {
          bool          none = false;
          bool          border_only;
          SaWManWindow *single;

          idx++;

          D_MAGIC_ASSERT( tier, SaWManTier );

          if (!tier->left.updates.num_regions && !tier->right.updates.num_regions)
                continue;

          if (tier->update_once) {
               if (windows_updating( sawman, tier ))
                    continue;

               tier->update_once = false;
          }

          D_DEBUG_AT( SaWMan_Update, "  -> %d left_updates, %d right_updates (tier %d, layer %d)\n",
                      tier->left.updates.num_regions, tier->right.updates.num_regions,
                      idx, tier->layer_id );

          D_ASSERT( tier->region != NULL );

          D_DEBUG_AT( SaWMan_Update, "  -> [%d] %d left_updates, bounding %dx%d\n",
                      tier->layer_id, tier->left.updates.num_regions,
                      tier->left.updates.bounding.x2 - tier->left.updates.bounding.x1 + 1,
                      tier->left.updates.bounding.y2 - tier->left.updates.bounding.y1 + 1 );
          D_DEBUG_AT( SaWMan_Update, "  -> [%d] %d right_updates, bounding %dx%d\n",
                      tier->layer_id, tier->right.updates.num_regions,
                      tier->right.updates.bounding.x2 - tier->right.updates.bounding.x1 + 1,
                      tier->right.updates.bounding.y2 - tier->right.updates.bounding.y1 + 1 );

          if (!tier->config.width || !tier->config.height)
               continue;

          single = get_single_window( sawman, tier, &none );

          if (none && !sawman_config->show_empty) {
               if (tier->active) {
                    D_DEBUG_AT( SaWMan_Auto, "  -> Disabling region...\n" );

                    tier->active        = false;
                    tier->single_window = NULL;  /* enforce configuration to reallocate buffers */

                    dfb_layer_region_disable( tier->region );

                    if (sawman->cursor.context && tier->context->layer_id == sawman->cursor.context->layer_id) {
                         dfb_layer_activate_context( dfb_layer_at(sawman->cursor.context->layer_id), sawman->cursor.context );

                         dfb_layer_region_flip_update( sawman->cursor.region, NULL, DSFLIP_NONE );
                    }
               }

               dfb_updates_reset( &tier->left.updates );
               dfb_updates_reset( &tier->right.updates );

               continue;
          }

          border_only = get_border_only( sawman, tier );

          /* Remember color key before single mode is activated. */
          if (!tier->single_mode)
               tier->key = tier->context->primary.config.src_key;


          /* If the first mode after turning off the layer is not single, then we need
             this to force a reconfiguration to reallocate the buffers. */
          if (!tier->active) {
               tier->single_mode = true;               /* avoid endless loop */
               tier->border_only = !border_only;       /* enforce configuration to reallocate buffers */
          }

          if (single && !border_only) {
               ret = process_single( sawman, tier, single, flags );
               if (ret == DR_OK)
                    continue;
          }

          if (tier->single_mode) {
               D_DEBUG_AT( SaWMan_Auto, "  -> Switching back from single mode...\n" );

               tier->border_only = !border_only;       /* enforce switch */
          }

          /* Switch border/default config? */
          if (tier->border_only != border_only) {
               tier->border_only = border_only;

               set_config( sawman, tier );
          }

          if (!tier->active) {
               DFBRegion region = { 0, 0, tier->size.w - 1, tier->size.h - 1 };

               D_DEBUG_AT( SaWMan_Auto, "  -> Activating tier...\n" );

               tier->active = true;

               dfb_updates_add( &tier->left.updates, &region );

               if (tier->region->config.options & DLOP_STEREO)
                    dfb_updates_add( &tier->right.updates, &region );

               if (sawman->cursor.context && tier->context->layer_id == sawman->cursor.context->layer_id)
                    dfb_layer_activate_context( dfb_layer_at(tier->context->layer_id), tier->context );
          }

          tier->single_mode   = false;
          tier->single_window = NULL;

          if (tier->left.updates.num_regions || tier->right.updates.num_regions)
               process_updates( sawman, tier, flags );
     }

     return DFB_OK;
}

