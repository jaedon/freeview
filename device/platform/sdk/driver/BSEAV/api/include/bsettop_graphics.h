/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_graphics.h $
 * $brcm_Revision: 23 $
 * $brcm_Date: 3/1/10 10:42a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/include/bsettop_graphics.h $
 * 
 * 23   3/1/10 10:42a jgarrett
 * SW7550-272: Adding 32-bit YCbCr graphics format
 *
 * 22   11/10/09 3:42p jgarrett
 * SW7420-166: Adding graphics heap index
 *
 * 21   10/5/09 11:06a jgarrett
 * SW7325-612: Fixing bitmasks for certain blending operations
 *
 * 20   10/21/08 10:26a vsilyaev
 * PR 46638: Added  blit opearations
 *
 * 19   6/26/08 11:11a jgarrett
 * PR 36285: Fixing deep copy inside blit_params
 *
 * 18   12/18/07 10:49a vsilyaev
 * PR 38269: Added new parameters to change scan-order
 *
 * 17   6/20/07 10:26a jgarrett
 * PR 31505: Unifying name of graphics handle in prototypes
 *
 * 16   4/13/07 3:51p vsilyaev
 * PR 29840: Added API to render graphics surface as the decoder output
 *
 * 15   3/22/07 1:35p vsilyaev
 * PR 28558: Added bsurface_extended_blit function
 *
 * 14   1/30/07 11:22a vsilyaev
 * PR 25685: Improve documentation and operation types for
 * bsurface_blended_fill
 *
 * 13   12/14/06 3:15p ahulse
 * PR25685: Add function bsurface_blended_fill to allow a blended fill of
 * a surface
 *
 * 12   8/29/06 1:43p erickson
 * PR23964: added BSURFACE_SELECT_PALETTE_WITH_COLORKEY support
 *
 * 11   7/7/06 10:17a erickson
 * PR22272: Add ability to specify alignment for
 * bgraphics_create_framebuffer()
 *
 * PR22272/1   6/23/06 1:21p nilesh
 * PR22272: Add ability to specify alignment for
 * bgraphics_create_framebuffer()
 *
 * 10   6/8/06 10:27a erickson
 * PR20908: added double buffering API changes
 *
 * 9   5/4/06 12:38p erickson
 * PR19334: added bsurface_settings.disable_automatic_cache_flush to
 * eliminate unnecessary cache flushes if app wants to handle manual
 * flushing
 *
 * 8   4/28/06 2:26p jgarrett
 * PR 21279: Adding option to create surface using externally allocated
 * memory
 *
 * 7   12/13/05 4:53p erickson
 * PR18439: added bdecode_acquire_capture_buffer et al to return capture
 * buffer to application
 *
 * 6   7/1/05 2:50p erickson
 * PR15351: modified bgraphics_sync to clear entire framebuffer if any
 * surface is visible. This handles all conditions and changes behavior
 * only slightly.
 *
 * 5   6/30/05 1:08p erickson
 * PR15352: change meaning of BSURFACE_SET_DEST_ALPHA_WITH_AVG_ALPHA to
 * blend src1 and src2 alpha, not average. Then use it in bgraphics_sync
 * if not using per-surface alpha and offscreen_framebuffer is false.
 *
 * 4   3/15/05 5:01p erickson
 * PR14343: added condition to bsurface_blit that src2 cannot scale to
 * dest
 *
 * 3   3/8/05 4:39p erickson
 * PR14343: fixed comment
 *
 * 2   3/8/05 9:49a erickson
 * PR14343: added bsurface_blit
 *
 * 1   2/7/05 7:05p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/39   1/17/05 10:33a erickson
 * PR13325: updated docs
 *
 * Irvine_BSEAVSW_Devel/38   12/17/04 9:10a erickson
 * PR13325: added bsurface_sync and updated docs
 *
 * Irvine_BSEAVSW_Devel/37   11/4/04 12:28p erickson
 * PR13101: added a8 pixel_format support, tweaked comments
 *
 * Irvine_BSEAVSW_Devel/36   10/25/04 12:08p erickson
 * PR12934: added async_blits parameter instead of compile-time option
 *
 * Irvine_BSEAVSW_Devel/35   9/22/04 5:02p erickson
 * PR12751: updated example
 *
 * Irvine_BSEAVSW_Devel/34   9/13/04 1:58p erickson
 * PR11081: clarified comment on pixel format related to palettized values
 *
 * Irvine_BSEAVSW_Devel/33   9/2/04 3:40p erickson
 * PR12553: clarified bgraphics_pixel when pixel value is less than 32
 * bits
 *
 * Irvine_BSEAVSW_Devel/32   9/2/04 11:59a erickson
 * PR12555: modified comments for palettized color support
 *
 * Irvine_BSEAVSW_Devel/31   7/13/04 6:31p dlwin
 * PR11578: Added API to load CLUT for clut surfaces.
 *
 * Irvine_BSEAVSW_Devel/30   7/6/04 3:53p erickson
 * PR11771: settop api dataflow redesign
 *
 * Irvine_BSEAVSW_Devel/29   6/30/04 4:26p bandrews
 * PR11578: Moved palette member from bsurface_create_settings to
 * bsurface_settings.
 *
 * Irvine_BSEAVSW_Devel/28   6/21/04 7:53p dlwin
 * PR11432: Adding CLUT2 and CLUT4 support.
 *
 * Irvine_BSEAVSW_Devel/27   6/2/04 2:44p erickson
 * PR11204: changed/removed deprecated settop api functions
 *
 * Irvine_BSEAVSW_Devel/26   5/17/04 11:03a erickson
 * PR11135: renamed prototype parameter names for autoparsing code
 *
 * Irvine_BSEAVSW_Devel/25   4/14/04 5:58p vsilyaev
 * PR 8850: Fixed comments, replaced blit with copy.
 *
 * Irvine_BSEAVSW_Devel/24   3/23/04 9:02a erickson
 * PR9835: removed documentation that says implicit bgraphics_sync's will
 * be called
 *
 * Irvine_BSEAVSW_Devel/23   3/22/04 9:36a erickson
 * PR10166: updated documentation on bgraphics_sync
 *
 * Irvine_BSEAVSW_Devel/22   3/19/04 2:34p erickson
 * PR8850: Changed documentation for bgraphics_pixel in bsurface_fill. It
 * is now consistently ARGB8888 format and not an exception as it was
 * documented.
 *
 * Irvine_BSEAVSW_Devel/21   2/13/04 4:49p marcusk
 * PR9728: Updated to keep API the same (added a new function call)
 *
 * Irvine_BSEAVSW_Devel/20   2/13/04 11:37a marcusk
 * PR9728: Updated code to support different formats and new method of
 * scaling.
 *
 * Irvine_BSEAVSW_Devel/19   2/10/04 4:06p erickson
 * PR9691: updated comments for 7038
 *
 * Irvine_BSEAVSW_Devel/18   2/10/04 11:46a erickson
 * PR9691: bgraphics_update was a bogus name for bgraphics_sync. Renamed
 * it in the documentation. No api change.
 *
 * Irvine_BSEAVSW_Devel/Irvine_BSEAVSW_Devel_7038/2   12/17/03 10:00p vsilyaev
 * Pass pixel by value not by reference.
 *
 * Irvine_BSEAVSW_Devel/Irvine_BSEAVSW_Devel_7038/1   12/5/03 9:17p vsilyaev
 * Changed return type of bsurface_destroy to void.
 *
 * Irvine_BSEAVSW_Devel/16   11/11/03 4:56p erickson
 * PR8600: graphics are disabled when the display format is set because
 * the app must adjust scaling and other
 * parameters, and settop api can't do it automatically.
 *
 * Irvine_BSEAVSW_Devel/15   11/7/03 4:28p erickson
 * PR8583: removing, fixing or implementing TODO's
 *
 * Irvine_BSEAVSW_Devel/14   11/4/03 2:42p erickson
 * documentation changes
 *
 * Irvine_BSEAVSW_Devel/13   10/31/03 11:58a erickson
 * settop api changes after internal review
 *
 * Irvine_BSEAVSW_Devel/12   10/28/03 11:27a erickson
 * settop api reworks after internal design view
 *
 * Irvine_BSEAVSW_Devel/11   10/22/03 11:23a erickson
 * settop api pass-through type conversion
 *
 * Irvine_BSEAVSW_Devel/10   10/1/03 10:16a erickson
 * updated information for docjet
 *
 * Irvine_BSEAVSW_Devel/9   9/30/03 1:13a erickson
 * updated docjet documentation
 *
 * Irvine_BSEAVSW_Devel/8   9/17/03 5:04p erickson
 * updated documentation for DocJet
 *
 * Irvine_BSEAVSW_Devel/7   8/12/03 9:42a erickson
 * changes after pre-review meeting
 *
 * Irvine_BSEAVSW_Devel/6   7/30/03 10:20a erickson
 * enfore the settings.position.width/height are not modified
 *
 * Irvine_BSEAVSW_Devel/5   7/30/03 9:38a erickson
 * added cplusplus wrappers
 *
 * Irvine_BSEAVSW_Devel/4   7/28/03 3:47p erickson
 * removed _impl
 *
 * Irvine_BSEAVSW_Devel/3   7/25/03 4:22p erickson
 * in the middle of rework, does not compile
 *
 * Irvine_BSEAVSW_Devel/2   7/8/03 3:19p erickson
 * added graphics api
 *
 ***************************************************************************/
#ifndef BSETTOP_GRAPHICS_H__
#define BSETTOP_GRAPHICS_H__

#ifdef __cplusplus
extern "C" {
#endif

/*=********************************************
The graphics interface supports Broadcom's newest framebuffer graphics
model as well as the mulitple-surface graphics architecture.

All pixel formats are supported along with chromakey and alphablending
effects.

On some platforms the hardware supports copy and fill, otherwise it is supported
with the CPU.
***********************************************/

/*
Summary:
For non-palettized surfaces, this is a ARGB8888 pixel.
When used, it will be converted into the desired pixel for a surface.
For palettized surfaces, this is the palette index plus possible alpha value.

Description:
For palette indexes, values should be packed into the least significant bits
of bgraphics_pixel. For instance, a8_palette8 uses byte[0] for the palette8 index,
byte[1] for alpha8 value and ignores byte[2] and byte[3].
palette2 uses bits 0 and 1 of byte[0] and ignores the rest.
*/
typedef uint32_t bgraphics_pixel;

/*
Summary:
	Pixel format of a surface.
*/
typedef enum bgraphics_pixel_format {
	bgraphics_pixel_format_r5_g6_b5,         /* 16-bit, no per-pixel alpha */
	bgraphics_pixel_format_a1_r5_g5_b5,      /* 16-bit */
	bgraphics_pixel_format_r5_g5_b5_a1,      /* 16-bit */
	bgraphics_pixel_format_a4_r4_g4_b4,      /* 16-bit */
	bgraphics_pixel_format_r4_g4_b4_a4,      /* 16-bit */
	bgraphics_pixel_format_a8_r8_g8_b8,      /* 32-bit */

	bgraphics_pixel_format_palette8,         /* 8-bit Palette index */
	bgraphics_pixel_format_a8_palette8,      /* A8 and 8-bit Palette index */

	bgraphics_pixel_format_a0,               /* 0-bit surface (constant color, constant alpha) */
	bgraphics_pixel_format_a8,               /* 8-bit alpha-only surface (constant color) */

	bgraphics_pixel_format_y08_cb8_y18_cr8,  /* 32-bit Y0_8 Cb_8 Y1_8 Cr_8 */
	bgraphics_pixel_format_cr8_y18_cb8_y08,  /* 32-bit Cr_8 Y1_8 Cb_8 Y0_8 */
	bgraphics_pixel_format_a8_y8_cb8_cr8,    /* 32-bit A_8 Y_8 Cb_8 Cr_8 */

	bgraphics_pixel_format_video_tunnel,      /* 0-bit tunnel to video. Use to allow video to show through graphics
												without chromakey and without per-pixel alpha. */
	bgraphics_pixel_format_palette2,		  /* 2-bit palette index */
	bgraphics_pixel_format_palette4 		  /* 4-bit palette index */
} bgraphics_pixel_format;

/*
Summary:
A palette of colors to be used with palettized surfaces.
*/
typedef struct bgraphics_palette {
	const bgraphics_pixel *palette;	/* [size_is(length)] */
	unsigned length;
} bgraphics_palette;

/*
Summary:
	Universal settings for a graphics engine.
Description:
	Graphics scaling is calculated by comparing the source width/height
	with the destination width/height.

	Instead of assuming the destination width/height is the width/height of the
	display format, the application must specify it independently. This is because
	the real issue is not the width/height of the display forward, but the x/y/width/height
	of the viewable area of the TV. This can vary depending on the application and
	target platform.

	The source width/height are used to define the scale factor used by graphic
	surfaces.
*/
typedef struct bgraphics_settings{
	bool enabled;						/* Is the graphics engine enabled? If disabled,
											no graphics will appear and no changes will
											be applied until it is reenabled. */
	bool chromakey_enabled;				/* should the chromakey values be applied? */
	bgraphics_pixel lower_chromakey;	/* lower bound of chromakey range */
	bgraphics_pixel upper_chromakey;	/* lower bound of chromakey range */
	bool antiflutter_filter;			/* should the antiflutter filter be enabled? */
	bool async_blits;					/* Performs asynchronous blits and fills if
											possible. If false, then bgraphics_sync does
											not need to be called between fill/blits and
											memory writes. */
	bool offscreen_framebuffer;	/* A common technique is for an application to create
		a single fixed-size surface, then blit the entire surface to the actual
		framebuffer. This handles scaling, and double-buffering. By setting this
		parameter to true, the bgraphics_sync call will assume this is the technique
		you are using. Therefore, it will do two things:
		1) when double-buffering, it will not update the other framebuffer surface
			because it will be completely overwritte
		2) do a copy, not a blend, when writing to the framebuffer.
		If you do not use bgraphics_sync, this parameter has no effect. */
	unsigned source_width; /* The virtual width of the graphics area
								that will be displayed. It used for scaling up to the destination
								width. */
	unsigned source_height; /* The virtual height of the graphics area
								that will be displayed. It used for scaling up to the destination
								height. */
	unsigned destination_width; /* Width of the display. This could be the same as the
								width of the display format, but it might be different
								because the real issue is the viewable area. Adjust this
								for different scaling. */
	unsigned destination_height; /* Width of the display. This could be the same as the
								width of the display format, but it might be different
								because the real issue is the viewable area. Adjust this
								for different scaling. */
	bsettop_callback operation_complete_callback; /* This optional callback notifies the
		user that all asynchronous fills and blits have completed. */
	void *callback_context; /* User defined context which is passed into the
		above callback function. */
} bgraphics_settings;

/**
Summary:
	Open and enable a graphics engine.
Description:
	Each display can have only one graphics engine. A graphics
	engine can only go to one display.

	Graphics can also appear on a second
	display when it is driven by a cloned window (see bdecode_window_close),
	however those graphics are identical to the other display.

	When the display format for the linked display changes, the graphics engine
	will be automatically disabled. See bdisplay_set for details.
**/
bgraphics_t bgraphics_open(
	bobject_t id,
	bdisplay_t display	/* display on which the graphics are displayed */
	);

/*
Summary:
	Close a graphics engine.
Description:
	You should close all surfaces before closing a graphics engine.
	Also, you must close graphics before closing the display to which it is linked.
	Generally, close things in the opposite order in which you brought them up.
*/
void bgraphics_close(
	bgraphics_t graphics	/* handle returned by bgraphics_open */
	);

/**
Summary:
	Get current graphics engine settings.
**/
void bgraphics_get(
	bgraphics_t graphics,	/* handle returned by bgraphics_open */
	bgraphics_settings *settings /* [out] */
	);

/**
Summary:
	Set graphics engine settings.
**/
bresult bgraphics_set(
	bgraphics_t graphics, 	/* handle returned by bgraphics_open */
	const bgraphics_settings *settings
	);

/**
Summary:
Sync all visible surfaces and automatically blit visible surfaces to the
framebuffer on framebuffer-enabled systems.

Description:
This is an optional and a potentially very disagreeable call.
It was designed to provide compatibility betweeen legacy (i.e. pre-7038) real-time graphics
compositor systems and magnum (i.e. 7038 and beyond) framebuffer based system.
If you have complex graphics operations and have no need for legacy support, you probably should
not use this call. Instead, you can draw, copy, blit or fill directly into the framebuffer from any surface.
bgraphics_sync will constrain your scaling and blending capabilities. Performing these operations manually without
bgraphics_sync will give you much more flexibility.

The behavior of bgraphics_sync depends on whether the platform uses a real-time
graphics compositor or not. On legacy systems, visible surfaces are composited
for each field with real-time priority and make up a "virtual framebuffer." In this case,
bgraphics_sync is equivalent to bsurface_sync on all visible surfaces.

The following comments only apply to magnum-based systems:

On magnum systems, surfaces which are marked visible in the
Settop API are not visible in and of themselves. Instead, the visible flag means that bgraphics_sync will
automatically blend or copy the surface to the one and only visible surface (called the framebuffer).
The process is as follows:

1. The framebuffer's cached memory will be flushed.

2. A check is performed to see if any surfaces are marked visible. If not, skip to step 6.
This is equivalent to bsurface_sync(framebuffer).

3. The framebuffer will be cleared with a background color, using either the chromakey value or all 0's
(black with possible alpha 0). All user operations drawn directly to the framebuffer will be wiped out.
This mimics the legacy "virtual framebuffer" where no surface simply meant no graphics.
If bgraphics_settings.offscreen_framebuffer is true, then it is assumed that the visible surface(s) will
completely cover the entire framebuffer. In this case, no clearing or per-pixel blending is performed.

4. Cached memory for every visible surface is flushed.

5. Every visible surface is copied to the framebuffer with possible blending, scaling and translation.
Blending is based on the bsurface_settings.alpha value or alpha-per-pixel values.
Scaling and translation of the surface's x,y,width and height is based on the proportion of
bgraphics_settings.source_width to destination_width and source_height to destination_height.

6. Wait for all async fills and blits to complete.
**/
bresult bgraphics_sync(
	bgraphics_t graphics	/* handle returned by bgraphics_open */
	);

/**
Summary:
Sync a surface by flushing cached memory and waiting for any async blits or fills.

Description:
If bgraphics_settings.async_blits is true, then fills and blits are executed
asynchrously. If you are using these in conjunction with CPU writes, then you must
sync before doing CPU writes.

On magnum platforms, if bsurface_sync is called on the framebuffer, it calls bgraphics_sync(surface->graphics).
If you have visible surfaces, they will be automatically blitted. If you do not have
visible surfaces, it will simply sync the framebuffer's surface like all other surfaces.

bsurface_get_memory returns a pointer to cached memory, and so bsurface_sync
will flush the cache.

bsurface_sync should be called at the boundary between fills/blits and CPU accesses. The following
example illustrates why.

Example:
	s1 = bsurface_create(g, &create_settings);
	bsurface_get_mem(s1, &mem);

	bsurface_fill(s1, &rect, BLACK);
	bsurface_sync(s1, NULL); // this waits until fill is done
	((uin32_t*)mem.buffer)[10] = BLUE; // this blue pixel is now guaranteed to
		// be drawn on top of the black box
	bsurface_sync(); // this guarantees that you can see the BLUE pixel on the screen
		// it also makes sure the cache is flushed before doing the next fill
	bsurface_fill(s1, &rect, BLACK);
	bgraphics_sync(g, NULL);
**/
bresult bsurface_sync(
	bsurface_t surface	/* handle returned by bsurface_create */
	);

/*
Summary:
Switch between framebuffers in double-buffered mode.

Description:
Double-buffered mode is enabled by a parameter to bgraphics_create_framebuffer. Implicitly created
framebuffers are not double-buffered.

bgraphics_flip performs an implicit call to bgraphics_sync.

bgraphics_flip will copy the contents of the currently displayed buffer into the
new off-screen buffer. The app can then make incremental updates to the framebuffer instead of redrawing
the entire framebuffer. However, if bgraphics_settings.offscreen_framebuffer is true, then this copy
will not occur because the app will be redrawing the entire framebuffer.
*/
bresult bgraphics_flip(
	bgraphics_t graphics	/* handle returned by bgraphics_open */
	);

/*
Summary:
Waits for previous call to bgraphics_flip to complete.

Description:
This function waits for the previous call to bgraphics_flip to finish flipping the back
buffer to the front buffer. This function should be called before calling bgraphics_flip
again.
*/
bresult bgraphics_flip_sync(
	bgraphics_t graphics	/* handle returned by bgraphics_open */
	);

/*
Summary:
	Information about the memory used by a surface.
*/
typedef struct bsurface_memory {
	void *buffer; /* pointer to mapped memory. Directly accessible by the application.
						This must be typecasted to the correct pixel format, for example
						unsigned short * for a 16 bit pixel format or unsigned long *
						for a 32 bit pixel format. */
	unsigned int pitch; /* The memory width of the surface in bytes. The visible width
							is <= pitch. Pitch must be used to calculate the next
							line of the surface. See the example in the bsurface_get_memory
							documentation. */
} bsurface_memory;

/*
Summary:
Settings needed to create a surface.
Description:
These cannot be changed after the surface has been created.

For palettized surfaces, a default palette of all black will be created. You must
call bsurface_set or bsurface_load_clut to set your desired palette.
*/
typedef struct bsurface_create_settings {
	bgraphics_pixel_format pixel_format;	/* format of the surface. */
	unsigned int width;						/* visible width of the surface. The pitch
											may be wider. */
	unsigned int height;					/* visible height of the surface. */

	unsigned int alignment; /* optional buffer alignment specified
				 * as a power of 2, measured in bytes.
				 * 0 is default alignment (default), 1
				 * is 2-byte aligned, 2 is 4-byte
				 * aligned, etc. */

	unsigned int pitch; /* optional buffer pitch, measured in
			     * bytes. 0 is the default pitch (width *
			     * sizeof(pixel)). */

	unsigned int heap_index; /* optional heap index setting */
} bsurface_create_settings;

/*
Summary:
	Initialize the bsurface_create_settings structure with defaults.
Description:
	This is required before using a bsurface_create_settings structure.
*/
void bsurface_create_settings_init(
	bsurface_create_settings *create_settings,	/* [out] */
	bgraphics_t graphics		/* required for possible resource-dependent defaults */
	);

/**
Summary:
Creates a frame buffer, or master surface, using the create settings.

Description:
On legacy platforms, this function is optional because visible surfaces will be
composited with video at realtime. However the Settop API will mimic a framebuffer
for API compatibility.

On magnum platforms, a framebuffer is an essential part of the graphics architecture.
If you do not explicity create a framebuffer using this function, a framebuffer
will be implicitly created when you create the first surface.

Care should be taken when specifying the frame buffer size and
pixel format, as all other surfaces will need to be mapped to this frame buffer.
If the width is smaller than the destination_width specified by the bgraphics_set()
function, the horizontal graphics scaler will be used.

The framebuffer can be reconfigured with the bsurface routines, however the
framebuffer will never be reallocated, thus any changes made to the framebuffer
result in a framebuffer no larger than the one created by this routine.
Therefore, if you plan to switch between 480i and 1080i, be sure to create a
1080i-sized framebuffer at first.

Unlike bsurface_create, the framebuffer will default to being visible.

If back_buffer_count is not zero, 1 or 2 back buffers will also be created,
although you will only get one bsurface_t handle. bsurface_get_memory will return
information about the hidden surface. Call bgraphics_flip to hide one or more surfaces
and show the other. The call bsurface_get_memory to get the information about the other
surface.
**/
bsurface_t bgraphics_create_framebuffer(
	bgraphics_t graphics,	/* handle returned by bgraphics_open */
	const bsurface_create_settings *create_settings,
	unsigned int back_buffer_count /* 0, 1 or 2 back buffers */
	);

/**
Summary:
	Creates a new, invisible surface using the create settings.
Description:
	After creating the surface, you can make it visible using get and set_settings.
Example:
	bsurface_create_settings create_settings;
	bsurface_settings settings;
	bsurface_t surface;

	bsurface_init_create_settings(&create_settings);
	create_settings.width = 100;
	create_settings.height = 100;
	surface = bsurface_create(g, &create_settings);
	assert(surface);
	bsurface_get(surface, &settings);
	settings.visible = true;
	bsurface_set(surface, &settings);
**/
bsurface_t bsurface_create(
	bgraphics_t graphics,	/* handle returned by bgraphics_open */
	const bsurface_create_settings *create_settings
	);

/**
Summary:
DEPRECATED: Please use bgraphics_create_framebuffer().
**/
bsurface_t bsurface_get_display(
	bgraphics_t graphics,	/* handle returned by bgraphics_open */
	const bsurface_create_settings *create_settings
	);

/**
Summary:
Destroy a surface.

Description:
Destroying a surface also hides it.

It is invalid to destroy the surface return by bgraphics_create_framebuffer(), even on
legacy architectures.
**/
void bsurface_destroy(
	bsurface_t surface	/* handle returned by bsurface_create */
	);

/**
Summary:
	Returns information needed for direct memory writes.

Description:
	In order for direct memory writes to be successful, you must know the
	format specified by bgraphics_pixel_format.
	You cannot assume that pitch == width * sizeof(pixelformat).

	You cannot cache the value of bsurface_memory->buffer. This value may
	change if the surface is the display buffer and you're using double
	buffering.

	You should use bsurface_sync or bgraphics_sync when interleaving fill/blit/copy
	commands with CPU writes to make your code resilient to asynchronous
	operation.

Example:
	// example of using bsurface_memory's buffer and pitch to do a CPU-based fill
	// fill entire 16 bits/pixel surface, using pitch and knowledge of pixel format
	bsurface_settings settings;
	bsurface_memory mem;
	int bytes_per_pixel = 2;
	bsurface_get(s1, &settings);
	bsurface_get_memory(s1, &mem);
	for (y=0;y<settings.height;y++)
		for (x=0;x<settings.width;x++) {
			unsigned short *ptr = (unsigned short *)mem.buffer;
			ptr[y * mem.pitch/bits_per_pixel + x] = color;
		}

	// After calling flip, you must call bsurface_get_memory again.
	bsurface_t fb = bgraphics_create_framebuffer(graphics, &cs, true);
	...
	bsurface_get_memory(fb, &mem);
	mem.buffer[10] = BLACK;
	bgraphics_flip();
	bsurface_get_memory(fb, &mem);
	mem.buffer[10] = BLACK;	// Correct. mem->buffer will be updated if necessary.
**/
bresult bsurface_get_memory(
	bsurface_t s,
	bsurface_memory *memory /* [out] */
	);

/*
Summary:
Settings for a surface returned by bsurface_get.
Description:
These settings control the appearance of a surface, including visibility,
position, overlapping (zorder), transparency and palette.
*/
typedef struct bsurface_settings {
	bsettop_rect position; /* Position and size of the surface.
		position.width and height are readonly */
	bsettop_rect cliprect; /* The area within the surface that should be shown,
		relative to 0,0. If you want to show the entire surface, cliprect
		should be 0,0,position.width,position.height. If you specify a cliprect
		which is smaller, only that portition will be shown, and it will be
		scaled to fit the rectangle of size position.width and .height. */
	bool visible; /* Defaults to false. On magnum platforms, this causes a
		surface to be automatically blitted to the framebuffer when bgraphics_sync
		is called. On legacy platforms, this causes the surface to be blended into
		the display by the compositor. */
	unsigned zorder; /* 0 is on bottom. If two surfaces have the same zorder value,
		their position relative to each other is undefined. You cannot rely on
		creation order to provide an implicit zorder because the behavior may
		change. */
	uint8_t alpha; /* 0x00 to 0xFF where 0xFF is opaque and 0x00 is transparent.
		On magnum platforms, this is only used when automatically blitting to
		the framebuffer with bgraphics_sync. On legacy platforms, this controls
		the alpha blending with the compositor. */
	bgraphics_pixel color;	/* Constant color for any surface that doesn't have a
		per-pixel color value (e.g. bgraphics_pixel_format_a0 or a8).
		This should be used mutual exclusively with the palette member, if either
		are to be used.*/
	bgraphics_palette palette;	/* For legacy platforms, there is only one palette
		for the entire graphics engine; therefore, the last palette set is the
		global palette for all surfaces. On magnum platforms, each surface can
		have its own palette. */
	bool disable_automatic_cache_flush; /* By default, the settop api will assume that
		every surface's cache must be flushed before performing a HW fill/blit.
		This is only needed if the application is combining CPU writes with HW fill/blits.
		If the app is not, this cache flush is wasteful. If you set this param
		to true, no default cache flush will be performed. The app can call bsurface_sync
		to manually flush the cache when needed. */
} bsurface_settings;

/**
Summary:
Apply bsurface_settings to a surface.

Description:
On magnum platforms, the bsurface_settings.alpha value is applied differently to the framebuffer
and non-framebuffer surfaces. For the framebuffer, this alpha controls blending with
video. For non-framebuffer surfaces, this alpha specifies the level of transparency
when blitting the visible surface to the framebuffer. See bgraphics_sync for details on this process.
**/
bresult bsurface_set(
	bsurface_t s,
	const bsurface_settings *settings
	);

/*
Summary:
	Get the bsurface_settings for a surface.
*/
void bsurface_get(
	bsurface_t s,
	bsurface_settings *settings /* [out] */
	);

/**
Summary:
	Fill rectangle with pixel value.
Description:
	This operation may be asynchronous. You must call bgraphics_sync, or call
	bsurface_set with visible == true in order to synchronize.
	If you write directly to memory before synchronizing, the result is indeterminate.
**/
bresult bsurface_fill(
	bsurface_t surface,		/* Surface to be filled */
	const bsettop_rect *rect,	/* rectangle within the surface that will be filled.
								It will be clipped to the surface's visible area. */
	bgraphics_pixel pixel /*
		For non-palettized surfaces, this is a universal ARBG8888
		value which will be used to fill the surface.
		For YCrCb surfaces, the colorspace will be converted from ARGB8888
		before the fill.
		For palettized surfaces, this is the index value. */
	);


/**
Summary:
	Copy from a rectangle to another rectangle with possible scaling and
	pixel format conversion.
Description:
	This operation may be asynchronous. You must call bgraphics_sync, or call
	bsurface_set with visible == true in order to synchronize.
	If you write directly to memory before synchronizing, the result is indeterminate.

	If the size of the destrect and srcrect differ, the content will be scaled.

	There is no alpha blending with this operation. If the dest has an alpha
	channel, then it will be filled either with the src's alpha channel or with
	0xFF (opaque).
**/
bresult bsurface_copy(
	bsurface_t destsurface, 	/* Destination surface to be copied into */
	const bsettop_rect *destrect,	 /* Rectangle in the destination surface where the
									pixels will be copied. It will be clipped with
									the visible area of the surface. */
	bsurface_t srcsurface, 		/* Source surface to be copied from */
	const bsettop_rect *srcrect	 /* Rectangle in the source surface where the
									pixels will be copied from. It will be clipped with
									the visible area of the surface. */
	);

/**
BSURFACE_BLEND_XXXX are values to be used in the bsurface_blit operation
parameter. You should chose one value.

Alpha values are always relative to src1 and should be between 0x00 and 0xFF.
Therefore alpha of 0xFF means take all of src1, none of src2.
Likewise alpha of 0x00 means take none of src1, all of src2.

There are some special cases which are enforced and optimized in the implementation.
For instance, if you don't want to blend colors, you can set src2 to NULL,
use BSURFACE_BLEND_WITH_SRC1_ALPHA, and set pixel1 == 0xFF, and the color blend
will be bypassed. You can still use the BSURFACE_SET_DEST_ALPHA to manipulate
the alpha-per-pixel.
**/

/* Use the pixel1 parameter to blend the sources. */
#define BSURFACE_BLEND_WITH_PIXEL1			0x0000
/* Copy src1's alpha-per-pixel to blend the sources */
#define BSURFACE_BLEND_WITH_SRC1_ALPHA		0x0001
/* Copy src2's alpha-per-pixel to blend the sources */
#define BSURFACE_BLEND_WITH_SRC2_ALPHA		0x0002
/* Blend two palettized surfaces using colorkey to select per pixel */
#define BSURFACE_SELECT_PALETTE_WITH_COLORKEY 0x0004
/* Copy source (blend with one) */
#define BSURFACE_BLEND_WITH_ONE		        0x0008

/**
BSURFACE_BLIT_DEST_ALPHA_XXX specifies how the alpha-per-pixel values of the dest
surface should be filled. If the dest has no alpha-per-pixel, any setting is ignored.
**/

/* Copy the pixel2 parameter into the dest's alpha-per-pixel. */
#define BSURFACE_SET_DEST_ALPHA_WITH_PIXEL2			0x0000
/* Copy src1's alpha channel into dest's alpha-per-pixel. */
#define BSURFACE_SET_DEST_ALPHA_WITH_SRC1_ALPHA		0x0010
/* Copy src2's alpha channel into dest's alpha-per-pixel. */
#define BSURFACE_SET_DEST_ALPHA_WITH_SRC2_ALPHA		0x0020
/* Blend src1's and src2's alpha-per-pixel into dest's alpha-per-pixel.
The formula is dest = src1 + src2 * (1-src1). */
#define BSURFACE_SET_DEST_ALPHA_WITH_AVG_ALPHA		0x0040
/* Blend pixel2 parameter and src2's alpha-per-pixel into dest's alpha-per-pixel.
The formula is dest = pixel2 + src2 * (1-pixel2). */
#define BSURFACE_SET_DEST_ALPHA_WITH_AVG_ALPHA_PIXEL2   0x0080
 /* Multiply src1's alpha-per-pixel with pixel2 parameter.
The formula is dest = src1 * pixel2. */
#define BSURFACE_SET_DEST_ALPHA_WITH_SRC1_TIMES_PIXEL2  0x0100

/**
Summary:
Blit two surfaces together into a destination surface.

Desription:
Blit is a superset of copy. It gives you more control over how two surfaces
will be blended together and how the destination surface's alpha-per-pixel (or
alpha channel) will be populated if present.

src1 or src2 can be the same as dest.

If src2 is NULL, then you must use BSURFACE_BLEND_WITH_SRC1_ALPHA and
have pixel1 of 0xFF. Anything else implies that you are blending between src1
and src2, which cannot be done. Likewise for src1.

If both src1 and src2 are null, then operation is ignored and pixel1 is regarded
as a fill color.

There are some operations that will internally result as a two-pass operation.
This is hardware specific. In general, the Settop API will fulfill your request
or return an error code if it cannot. Please see the implementation for more
details.

Scaling and clipping are specified by the proportion of the src1, src2 and
dest rectangles. The entire src1_rect is combined with the entire src2_rect and
the result is copied into the entire dest_rect. Some platforms have
scaling limitations. For instance, on 7038 src2 cannot scale, therefore its
width and height must match dest's.

Example:
	// bsurface_copy is equivalent to:
	bsurface_blit(destsurface, destrect,
		BSURFACE_BLEND_WITH_PIXEL1|BSURFACE_SET_DEST_ALPHA_WITH_SRC1_ALPHA,
		src1, src1rect, NULL, NULL, 0xFF, 0xFF);

	// blend two surfaces together using a constant alpha, and set
	// the dest's alpha channel with a constant alpha
	bsurface_blit(destsurface, destrect,
		BSURFACE_BLEND_WITH_PIXEL1|BSURFACE_SET_DEST_ALPHA_WITH_PIXEL2,
		src1, src1rect, src2, src2rect, 0xA0, 0xFF);

	// blend two surfaces together using src1's alpha channel, and set
	// the dest's alpha channel to be src1's alpha. The pixel1 and pixel2
	// values are ignored.
	bsurface_blit(destsurface, destrect,
		BSURFACE_BLEND_WITH_SRC1_ALPHA|BSURFACE_SET_DEST_ALPHA_WITH_SRC1_ALPHA,
		src1, src1rect, src2, src2rect, 0, 0);
**/
bresult bsurface_blit(
	bsurface_t destsurface, 		/* Destination surface to be copied into */
	const bsettop_rect *destrect,	/* Rectangle of destsurface that will be filled.
		If NULL, use the whole destsurface. */
	uint32_t operation,				/* Combination of BSURFACE_BLEND_XXXX and
		BSURFACE_SET_DEST_ALPHA_XXXX values.
		The choice of operation determines the meaning of pixel1 and pixel2 parameters
		and requirements for src1 and src2.
		See documentation above. */
	bsurface_t src1,				/* 1st source surface to be copied from.
		Can be NULL if not used. */
	const bsettop_rect *src1_rect,	/* Rectangle of src1_surface to be used.
		If NULL, use the whole src1 surface. */
	bsurface_t src2,				/* 2nd source surface to be copied from.
		Can be NULL if not used. */
	const bsettop_rect *src2_rect,	/* Rectangle of src2_surface to be used.
		If NULL, use the whole src2 surface. */
	bgraphics_pixel pixel1,			/* 1st constant value specified by some operations */
	bgraphics_pixel pixel2			/* 2nd constant value specified by some operations */
	);


typedef struct bsurface_blit_params {
    uint32_t operation;             /* Combination of BSURFACE_BLEND_XXXX and
        BSURFACE_SET_DEST_ALPHA_XXXX values.
        The choice of operation determines the meaning of pixel1 and pixel2 parameters
        and requirements for src1 and src2.
        See documentation above. */
    bsurface_t src1;                /* 1st source surface to be copied from.
                                       Can be NULL if not used. */
    bsettop_rect src1_rect;    /* Rectangle of src1_surface to be used.
                                  If width == height == 0 (default), use
                                  the whole src1 surface. */
    bsurface_t src2;                /* 2nd source surface to be copied from.
                                       Can be NULL if not used. */
    bsettop_rect src2_rect;    /* Rectangle of src2_surface to be used.
                                  If width == height == 0 (default), use
                                  the whole src2 surface. */
    bgraphics_pixel pixel1;    /* 1st constant value specified by some operations */
    bgraphics_pixel pixel2;    /* 2nd constant value specified by some operations */
    int16_t brightness;  /* Color space conversion for the surface (not the GFD).
                            Ranges between -32768 and 32767. Default is 0. */
    int16_t contrast;       /* Color space conversion for the surface (not the GFD).
                            Ranges between -32768 and 32767. Default is 0. */
    int16_t saturation; /* Color space conversion for the surface (not the GFD).
                            Ranges between -32768 and 32767. Default is 0. */
    int16_t hue;            /* Color space conversion for the surface (not the GFD).
                            Ranges between -32768 and 32767. Default is 0. */

    bool src1_right_to_left;    /* If true, src1 will be inverted horizontally during blit */
    bool src1_bottom_to_top;    /* If true, src1 will be inverted vertically during blit */
    bool src2_right_to_left;    /* If true, src2 will be inverted horizontally during blit */
    bool src2_bottom_to_top;    /* If true, src2 will be inverted vertically during blit */
} bsurface_blit_params;


void bsurface_blit_params_init(bsurface_blit_params *params);

bresult bsurface_extended_blit(
	bsurface_t destsurface,
	const bsettop_rect *destrect,
	const bsurface_blit_params *params
	);



/**
BSURFACE_FILL_XXXX are values to be used in the bsurface_blended_fill operation

**/

/* pick one of the following color operations: */
/* Blend fill color with destsurface using fillcolor alpha */
#define BSURFACE_FILL_BLEND_WITH_PIXEL_ALPHA        0x0000
/* Do not blend the color. Simply copy the color to the surface. This is equivalent to bsurface_fill().
Other BSURFACE_FILL operations are ignored. */
#define BSURFACE_FILL_COPY_COLOR                    0x0001
/* Do not change the color. This is used if you want to change the alpha channel only. */
#define BSURFACE_FILL_BLEND_DONT_CHANGE_COLOR       0x0002

/* pick one of the following alpha operations: */
/* set destsurface alpha channel with fill color alpha */
#define BSURFACE_FILL_SET_ALPHA_WITH_PIXEL_ALPHA    0x0000
/* leave destsurface alpha channel unchanged */
#define BSURFACE_FILL_DONT_CHANGE_ALPHA             0x0010
/* set destsurface alpha channel with average of destsurface and fill color */
#define BSURFACE_FILL_SET_ALPHA_WITH_AVG_ALPHA      0x0020

/**
Summary:
	Blend Fill rectangle with pixel value.
Description:
	This operation may be asynchronous. You must call bgraphics_sync, or call
	bsurface_set with visible == true in order to synchronize.
	If you write directly to memory before synchronizing, the result is indeterminate.
**/
bresult
bsurface_blended_fill(
	bsurface_t surface,         /* Surface to be filled */
	const bsettop_rect *rect,   /* rectangle within the surface that will be filled.
								It will be clipped to the surface's visible area. */
	bgraphics_pixel pixel,      /*
		For non-palettized surfaces, this is a universal ARBG8888
		value which will be used to fill the surface.
		For YCrCb surfaces, the colorspace will be converted from ARGB8888
		before the fill.
		For palettized surfaces, this is the index value. */
	uint32_t operation         /* gfx operation to perform, eg, simple fill or blend with specified pixel color.
                                  see BSURFACE_FILL_BLEND_XXXX above for options. */

	);

/**
Summary:
Load a CLUT for palettized surfaces.

Description:
CLUT means "color look up table" which means the same as "palette."

The palette memory is copied. The number of entries in the palette can be any amount,
however it may be truncated based on the number of palette entries required for
the pixel format.

For legacy devices there can be only one clut per graphics devices. So changing
the palette for one palettized surface will affect all the other palettized surfaces
in the same graphics device.

For magnum platforms, each surface has a separate palette.
**/
bresult bsurface_load_clut(
	bsurface_t s,
	bgraphics_palette palette);

#ifdef __cplusplus
}
#endif

#endif /* BSETTOP_GRAPHICS_H__ */

