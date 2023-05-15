/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_video_image_input.h $
 * $brcm_Revision: 14 $
 * $brcm_Date: 10/11/12 12:29p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/include/nexus_video_image_input.h $
 * 
 * 14   10/11/12 12:29p vsilyaev
 * SW7435-329: Converted NEXUS_VideoInput to NEXUS_VideoInputHandle and
 *  added object verification
 * 
 * 13   2/20/12 5:28p mphillip
 * SW7425-1511: Merge pushing/recycling API changes to main
 * 
 * SW7425-1511/1   2/15/12 6:03p ahulse
 * SW7425-1511: Extend image input to use new API for Pushing and
 *  recycling surfaces as well as allow vsync count, pts and jump to head
 *  of queue for submitted image.
 * 
 * 12   9/29/11 2:04p erickson
 * SW7425-1270: rename to progressiveScan1080
 * 
 * 11   9/9/11 4:45p erickson
 * SW7425-1270: add NEXUS_VideoImageInputSettings.is1080p60Capable
 * 
 * 10   5/13/11 12:17p erickson
 * SW7420-1873: remove shutdown attribute
 * 
 * 9   2/17/11 2:41p erickson
 * SW7420-1380: Refactor ImageInput to use a queue and be non-blocking.
 *  Add xx_SetNextSurface to be non-blocking. xx_SetSurface maintains
 *  blocking and is deprecated
 *
 * 8   12/14/10 4:37p erickson
 * SW7420-1285: add null_allowed attribute
 *
 * 7   12/30/09 12:23p erickson
 * SWDEPRECATED-2425: add comments for NEXUS_VideoImageInput_SetSurface
 *
 * 6   11/7/08 12:30p erickson
 * PR48745: added NEXUS_VideoImageInputSettings.heap
 *
 * 5   7/22/08 10:40a vsilyaev
 * PR 44727: Added functions to change settings of the video image
 *
 * 4   7/14/08 2:33p vsilyaev
 * PR 44727: Added imageCallback
 *
 * 3   4/7/08 5:03p erickson
 * PR39986: added #include
 *
 * 2   3/25/08 1:01p vsilyaev
 * PR 39986: Fixed destructor attribute
 *
 * 1   3/21/08 12:09p vsilyaev
 * PR 39986: Added VideoImageInput
 *
 **************************************************************************/
#ifndef NEXUS_VIDEO_IMAGE_INPUT_H__
#define NEXUS_VIDEO_IMAGE_INPUT_H__

#include "nexus_types.h"
#include "nexus_display_types.h"
#include "nexus_surface.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=*
The VideoImageInput interface provides a way to feed graphics into the system as a video source.
The VideoImageInput takes a surface and converts in into a VideoInput. Then this VideoInput can
be connected to a VideoWindow and the graphics will be displayed on the screen.

In the current implementation, VideoImageInput shares hardware resources with the VideoDecoder module (the MFD).
As a consequence, the user shall carefully choose the index parameter that is passed into NEXUS_VideoImageInput_Open
function. As a rule, the output of a VideoDecoder and a VideoImageInput with a matching 'index' can't be rendered
simultaneously. And before switching video between VideoDecoder and VideoImageInput, the user must call
NEXUS_VideoInput_Shutdown on unused instance of VideoInput.
**/

#define NEXUS_IMAGEINPUT_BUSY       NEXUS_MAKE_ERR_CODE(0x10A, 1)  /* image is in use and can't be disturbed */
#define NEXUS_IMAGEINPUT_QUEUE_FULL NEXUS_MAKE_ERR_CODE(0x10A, 2)  /* queue is full, can't add more          */

/**
Summary:
Handle for the VideoImageInput interface
**/
typedef struct NEXUS_VideoImageInput *NEXUS_VideoImageInputHandle;

/**
Summary:
Settings for VideoImageInput input
**/
typedef struct NEXUS_VideoImageInputSettings {
    NEXUS_CallbackDesc imageCallback;  /* Callback called when image has been released from for display, NULL or a new image must be passed in for this to happen. */
    unsigned qScale; /* Q-Scale factor used in DNR */
    bool duplicate;  /* should be set to true if next frame is indentical to the previous frame */
    NEXUS_HeapHandle heap; /* If non-NULL, the video path will allocate buffers from this heap based on NEXUS_DisplayHeapSettings.
                              If NULL, the video path will allocate buffers from the Display Module heap based on NEXUS_DisplayModuleSettings. */
    bool progressiveScan1080; /* if true, 1920x1080 stills will be scanned out as a single progressive frame.
                              you should only set this if you know that your MFD HW and RTS allows for it. for live decode, this is 1080p60 capability.
                              if false, 1920x1080 stills will be scanned out as two interlaced fields. */
    unsigned fifoSize;        /* size of image Input queue */
} NEXUS_VideoImageInputSettings;

/**
Summary:
Settings for VideoImageInputSurfaceSettings 
**/
typedef struct NEXUS_VideoImageInputSurfaceSettings {
    unsigned displayVsyncs;     /* Number of times this picture must be displayed before next picture in queue is displayed */
    bool     infront;           /* This picture will be the next picture displayed. Any pictures already queued will be purged. Default is false */
    uint32_t pts;               /* OPTIONAL pts passed into BVN. Default is ZERO. NOTE: true pts's are 33bit, so this is a rounded down 32 bit pts */
} NEXUS_VideoImageInputSurfaceSettings;

/**
Summary:
Get default settings for the Surface structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.
**/
void NEXUS_VideoImageInput_GetDefaultSurfaceSettings(
    NEXUS_VideoImageInputSurfaceSettings *pSettings /* [out] */
    );



/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.
**/
void NEXUS_VideoImageInput_GetDefaultSettings(
    NEXUS_VideoImageInputSettings *pSettings /* [out] */
    );

/**
Summary:
Creates a new VideoImageInput interface
**/
NEXUS_VideoImageInputHandle NEXUS_VideoImageInput_Open( /* attr{destructor=NEXUS_VideoImageInput_Close}  */
    unsigned index,
    const NEXUS_VideoImageInputSettings *pSettings /* attr{null_allowed=y} */
    );

/**
Summary:
Close the interface
**/
void NEXUS_VideoImageInput_Close(
    NEXUS_VideoImageInputHandle videoImage
    );

/**
Summary:
Push (submit) a new surface to be added to the image input display queue to be displayed by VDC.

Description:
Call this function to add the passed in surface to the image input display queue. Passed in surface
settings (pSettings) will dictate how many times the given surface is displayed ( displayVsyncs ), whether it
will be put at the head of the queue ( infront ) and all preceding surfaces flushed, and what pts
value is associated with this surface (pts).

**/
NEXUS_Error NEXUS_VideoImageInput_PushSurface(
    NEXUS_VideoImageInputHandle imageInput, 
    NEXUS_SurfaceHandle image, 
    const NEXUS_VideoImageInputSurfaceSettings *pSettings /* attr{null_allowed=y} */
    );

/**
Summary:
Recycle any surfaces that have been dequeued ( and no longer displayed ) from image input queue.

Description:
Call this function to get back surfaces that have been dequeued ( and no longer displayed ) from
the image input queue. 

To return more than one surface, pass in a pointer to an array of surfaces, with number of array 
elements specified in num_entries.

Number of surfaces freeable will be indicated by the value of num_returned.

**/
NEXUS_Error NEXUS_VideoImageInput_RecycleSurface( 
    NEXUS_VideoImageInputHandle imageInput,
    NEXUS_SurfaceHandle *recycled,          /* attr{nelem=num_entries;nelem_out=num_returned} */
    size_t num_entries,
    size_t *num_returned 
    );


/**
Summary:
Check that 'image' is unused by nexus and can therefore be reused or freed by the application.

Description:
Before checking an image, nexus cleans up any freed surfaces ( a surface which has transitioned from being displayed to not being displayed ).

Returns NEXUS_SUCCESS if 'image' is now unused by nexus
Returns NEXUS_IMAGEINPUT_BUSY if 'image' is still used by nexus
Returns another non-zero return if image clean up had a problem.
**/
NEXUS_Error NEXUS_VideoImageInput_CheckSurfaceCompletion(
    NEXUS_VideoImageInputHandle imageInput,
    NEXUS_SurfaceHandle image
    );

/**
Summary:
Set a new surface without blocking.

Description:
Setting the next surface does not mean that a previously set surface is now unused by nexus.
Nexus keeps a pipeline of surfaces. You must call NEXUS_VideoImageInput_CheckSurfaceCompletion to verify that a surface
is unused.

To avoid tearing, be aware of the following types of synchronization when setting a new surface:
1) Before calling NEXUS_VideoImageInput_SetNextSurface, ensure all blits into the new surface are complete using NEXUS_Graphics2D_Checkpoint.
2) Before calling NEXUS_VideoImageInput_SetNextSurface, call NEXUS_Surface_Flush(image) if you did any CPU writes to the new surface.
3) Before blitting into the surface and calling NEXUS_VideoImageInput_SetSurface, call NEXUS_VideoImageInput_CheckCompletion to
   check the new surface is available for re-use ( if the surface was previously used ).

Return code must be checked to see if space was available to accept the new surface and the new surface was added to the display queue.

Returns NEXUS_SUCCESS if 'image' is queued ready for display by nexus
Returns NEXUS_IMAGEINPUT_QUEUE_FULL if queue is full and image couldn't be added.
**/
NEXUS_Error NEXUS_VideoImageInput_SetNextSurface(
    NEXUS_VideoImageInputHandle imageInput,
    NEXUS_SurfaceHandle image /* new surface for video input, NULL is used to clear surface and flush the queue */
    );

/**
Summary:
Deprecated: Set new surface and block until any previously displayed surface is unused (freed)

Description:
Use NEXUS_VideoImageInput_SetNextSurface and NEXUS_VideoImageInput_CheckSurfaceCompletion instead.
**/
NEXUS_Error NEXUS_VideoImageInput_SetSurface(
    NEXUS_VideoImageInputHandle imageInput,
    NEXUS_SurfaceHandle image /* new surface for video input, NULL is used to clear surface and flush the queue */
    );

/**
Summary:
Get currenct settings of the ImageInput.
**/
void NEXUS_VideoImageInput_GetSettings(
    NEXUS_VideoImageInputHandle imageInput,
    NEXUS_VideoImageInputSettings *pSettings /* [out] */
    );

/**
Summary:
Set new settings for the ImageInput.
**/
NEXUS_Error NEXUS_VideoImageInput_SetSettings(
    NEXUS_VideoImageInputHandle imageInput,
    const NEXUS_VideoImageInputSettings *pSettings
    );


/**
Summary:
Returns the abstract NEXUS_VideoInput connector for the video input.
This connector is passed to a VideoWindow to display the image video.

Description:
See Also: NEXUS_VideoWindow_AddInput
**/
NEXUS_VideoInputHandle NEXUS_VideoImageInput_GetConnector(
    NEXUS_VideoImageInputHandle imageInput
    );

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_VIDEO_IMAGE_INPUT_H__ */

