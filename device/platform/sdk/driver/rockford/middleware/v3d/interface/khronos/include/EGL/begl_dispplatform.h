/*=============================================================================
Copyright (c) 2010 Broadcom Europe Limited.
All rights reserved.

Project  :  EGL driver
Module   :  Abstract Display Interface

FILE DESCRIPTION
Defines an abstract interface that will be used to interact with platform display.
=============================================================================*/


#ifndef _BEGL_DISPPLATFORM_H__
#define _BEGL_DISPPLATFORM_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
   /* These formats are render target formats, but cannot be textured from */
   BEGL_BufferFormat_eA8B8G8R8,
   BEGL_BufferFormat_eR8G8B8A8,           /* For big-endian platforms */
   BEGL_BufferFormat_eX8B8G8R8,
   BEGL_BufferFormat_eR8G8B8X8,           /* For big-endian platforms */
   BEGL_BufferFormat_eR5G6B5,
   /* These formats can be textured from, but cannot be rendered into */
   BEGL_BufferFormat_eYUV422_Texture,
   BEGL_BufferFormat_eVUY224_Texture,     /* For big-endian platforms */
   BEGL_BufferFormat_eA8B8G8R8_Texture,
   BEGL_BufferFormat_eA8R8G8B8_Texture,   /* same format as above, but has RB swap in the shader compiler */
   BEGL_BufferFormat_eR8G8B8A8_Texture,   /* For big-endian platforms */
   BEGL_BufferFormat_eB8G8R8A8_Texture,   /* same format as above, but has RB swap in the shader compiler */
   BEGL_BufferFormat_eR5G6B5_Texture,
   /* These formats are TFormat variants */
   BEGL_BufferFormat_eA8B8G8R8_TFormat,
   BEGL_BufferFormat_eX8B8G8R8_TFormat,
   BEGL_BufferFormat_eR5G6B5_TFormat,
   BEGL_BufferFormat_eR5G5B5A1_TFormat,
   BEGL_BufferFormat_eR4G4B4A4_TFormat,
   /* These are the LT versions for small textures */
   BEGL_BufferFormat_eA8B8G8R8_LTFormat,
   BEGL_BufferFormat_eX8B8G8R8_LTFormat,
   BEGL_BufferFormat_eR5G6B5_LTFormat,
   BEGL_BufferFormat_eR5G5B5A1_LTFormat,
   BEGL_BufferFormat_eR4G4B4A4_LTFormat,
   /* Can be used to return back an invalid format */
   BEGL_BufferFormat_INVALID
} BEGL_BufferFormat;

typedef enum
{
   BEGL_Success = 0,
   BEGL_Fail
} BEGL_Error;

typedef enum
{
   BEGL_Lock = 0,
   BEGL_Unlock
} BEGL_LockType;

typedef enum
{
   BEGL_BufferUsage_ePixmap,
   BEGL_BufferUsage_eSwapChain0,
   BEGL_BufferUsage_eSwapChain1,
   BEGL_BufferUsage_eSwapChain2
} BEGL_BufferUsage;

typedef void  *BEGL_BufferHandle;     /* Opaque as far as driver is concerned. Only the app/lib knows the actual type. */
typedef void  *BEGL_DisplayHandle;    /* Opaque 'display' handle (required by EGL) */
typedef void  *BEGL_WindowHandle;     /* Opaque 'window' handle (required by EGL) */

typedef struct
{
   void                *platformState;/* abstract state created by the platform layer */
   BEGL_WindowHandle   window;        /* The native window handle */
} BEGL_WindowState;

typedef struct
{
   void               *cachedAddr;    /* Cached address of buffer memory. Set to NULL during create to tell app to allocate. */
   uint32_t            physOffset;    /* Physical memory offset of buffer memory. Set to 0 during create. */
   uint32_t            width;         /* Visible width of buffer in pixels */
   uint32_t            height;        /* Visible height of buffer in pixels */
   uint32_t            pitchBytes;    /* Actual bytes per row of the buffer, including padding */
   uint32_t            totalByteSize; /* Actual bytes allocated for the entire image */
   uint32_t            alignment;     /* Buffer alignment specified as a power of 2. 1 = 2 bytes, 2 = 4 bytes etc. */
   BEGL_BufferFormat   format;        /* Pixel format of the buffer */
   BEGL_BufferUsage    usage;         /* States whether the buffer will be used as a pixmap or swap-chain buffer */
   BEGL_WindowState    windowState;   /* State relating to the window to which this buffer is related */
} BEGL_BufferSettings;

typedef enum
{
   BEGL_WindowInfoWidth = 1,
   BEGL_WindowInfoHeight = 2,
   BEGL_WindowInfoFormat = 4,
   BEGL_WindowInfoSwapChainCount = 8
} BEGL_WindowInfoFlags;

typedef struct
{
   uint32_t            width;             /* Visible width of window in pixels */
   uint32_t            height;            /* Visible height of window in pixels */
   uint32_t            swapchain_count;   /* Number of buffers in the swap chain, or 0 to take defaults from egl */
   BEGL_BufferFormat   format;            /* Pixel format of the buffer */
} BEGL_WindowInfo;

typedef struct
{
   uint32_t            width;         /* Visible width of pixmap in pixels */
   uint32_t            height;        /* Visible height of pixmap in pixels */
   BEGL_BufferFormat   format;        /* Pixel format of the pixmap */
} BEGL_PixmapInfo;

typedef struct
{
   BEGL_BufferHandle   buffer;         /* The buffer to be/having been displayed */
   uint32_t            cb_arg;         /* handle used by the driver */
   BEGL_WindowState    windowState;    /* State for this buffer's window */
} BEGL_BufferDisplayState;

typedef struct
{
   BEGL_BufferHandle   buffer;         /* The buffer to be/having been displayed */
   BEGL_LockType       lockType;       /* lock or unlock */
   BEGL_WindowState    windowState;    /* State for this buffer's window */
} BEGL_BufferAccessState;

typedef struct
{
   /* Context pointer - opaque to the 3d driver code, but passed out in all function pointer calls.
    * Prevents the client code needing to perform context lookups. */
   void *context;

   /* Request that the given state->buffer be displayed or added to a display queue.
    *
    * When the buffer has actually been displayed (which implies that the previous one is free to be reused)
    * you must call BEGL_Buffer_OnDisplay() with the same BEGL_BufferDisplayState. Normally, you would call 
    * BEGL_Buffer_OnDisplay() on the vsync signal following a SetFramebuffer call for example.
    *
    * Notes:
    * 1) You may get multiple calls to BufferDisplay() with the same buffer. You should call BEGL_Buffer_OnDisplay()
    *    each time (again, during the vsync normally). This is essentially a 'keep on display/still on display' sequence.
    * 2) Normally, you will only receive a new BufferDisplay() after you have called BEGL_Buffer_OnDisplay() for the
    *    previous frame. However, if the user sets SwapInterval(0) in EGL, BufferDisplay() will be called as quickly
    *    as frames are available. Just call BEGL_Buffer_OnDisplay() at vsync time as usual in this case. This mode
    *    of operation is not recommended (it's a performance measuring tool). It should not be used at all when 
    *    compositing, only when setting the framebuffer.
    *
    * You do not need to, and for performance reasons should not block waiting for a vsync before returning from this call. 
    * The BEGL_Buffer_OnDisplay() callback should be called asynchronously. */
   BEGL_Error (*BufferDisplay)(void *context, BEGL_BufferDisplayState *state);

   /* Flush the display of the given window's buffers and wait until they are no longer in use before returning.
    * The window's buffers are about to be destroyed, so don't be using them after this function returns. */
   BEGL_Error (*WindowUndisplay)(void *context, BEGL_WindowState *windowState);

   /* Request creation of an appropriate display buffer. settings->totalByteSize is the size of the memory that the driver needs.
    * We could have just requested a block of memory using the memory interface, but by having the platform layer create a 'buffer'
    * it can actually create whatever type it desires directly, and then only have to deal with that type. For example, in a Nexus
    * platform layer, this function might be implemented to create a NEXUS_Surface (with the correct memory constraints of course).
    * When the buffer handle is passed out during BufferDisplay, the platform layer can simply use it as a NEXUS_Surface. It
    * doesn't have to wrap the memory each time, or perform any lookups. Since the buffer handle is opaque to the 3d driver, the
    * platform layer has complete freedom.
    */
   BEGL_BufferHandle (*BufferCreate)(void *context, BEGL_BufferSettings *settings);

   /* Request a buffer from a pre-allocated swap chain.  The length of the chain is returned via WindowGetInfo() and the
    * swap chain must contain the correct number of buffers to match the swap chain length.  
    *
    * This function will be called instead of BufferCreate() for window buffers if WindowGetInfo() sets the swapchain count
    * to anything greater than 0. If swapchain count is set to 0, this function will never be called.
    * Note: BufferCreate() may still be called for other buffer types such as pixmaps even if swapchain count > 0.
    */
   BEGL_BufferHandle (*BufferGet)(void *context, BEGL_BufferSettings *settings);

   /* Called prior to a swapchain being created for the given window, if you need to store any global state (per window/swapchain)
    * then do it in this structure. You can allocate a structure of your own creation which will be associated with the window/swapchain. */
   void * (*WindowPlatformStateCreate)(void *context, BEGL_WindowHandle window);

   /* Called when a swapchain is destroyed */
   BEGL_Error (*WindowPlatformStateDestroy)(void *platformState);

   /* Destroy a buffer previously created with BufferCreate */
   BEGL_Error (*BufferDestroy)(void *context, BEGL_BufferDisplayState *bufferState);

   /* Get information about a created window buffer */
   BEGL_Error (*BufferGetCreateSettings)(void *context, BEGL_BufferHandle buffer, BEGL_BufferSettings *outSettings);

   /* Called to determine current size of the window referenced by the opaque window handle.
    * Also fills in the number of pre-allocated swap-chain buffers. Set to 0 to have the driver create all buffers via BufferCreate().
    * Set to the number of buffers in your pre-allocated chain if you have one. See BufferGet() if this is the case.
    * This is needed by EGL in order to know the size of a native 'window'. */
   BEGL_Error (*WindowGetInfo)(void *context, BEGL_WindowHandle window, BEGL_WindowInfoFlags flags, BEGL_WindowInfo *outInfo);

   /* Called when the buffer is locked, can be used to underlying surface manager that it is in use */
   BEGL_Error (*BufferAccess)(void *context, BEGL_BufferAccessState *bufferAccess);

   /* Check if the platform can support any additional formats */
   BEGL_Error (*IsSupported)(void *context, BEGL_BufferFormat bufferFormat);

   /* used for EGL_NATIVE_VISUAL_ID, returns a platform dependent visual ID from the config */
   BEGL_Error (*GetNativeFormat)(void *context, BEGL_BufferFormat bufferFormat, unsigned int *outNativeFormat);

   /* called when eglGetDisplay() is called.  Used in cases such as X11, so the X display can be passed into the
      driver.  Could also be used in NEXUS to remove the need for an app to explicitly call it's platform
      registration */
   BEGL_Error (*SetDisplayID)(void *context, unsigned int displayID, unsigned int *outEglDisplayID);

   /* DEPRECATED - no longer called. See WindowUndisplay() instead. */
   BEGL_Error (*BufferDisplayFinish)(void *context);  

   /* DEPRECATED - no longer called */
   BEGL_Error (*BufferGetWin)(void * context, BEGL_BufferHandle buffer, BEGL_WindowState *outWindowState);

} BEGL_DisplayInterface;

/* The client application, or default platform library must register valid versions of each 
   of these interfaces before any EGL or GL functions are invoked, using the following functions 
   provided by the 3D driver. 
*/
typedef struct
{
   /* Called by app/lib to create a pixmap buffer with the correct alignment/size constraints for EGL to use.
    * This will call back into the application's BufferCreate() function to actually allocate the buffer, after
    * calculating the appropriate alignment/size constraints.
    *
    * There should be no real need to use pixmap rendering in EGL, since the swap chain is exposed via this API anyway.
    * However, we must still keep pixmap rendering functional, and thus need this API call.
    *
    * The buffer should be destroyed using BEGL_DisplayInterface->BufferDestroy() */
   BEGL_BufferHandle (*PixmapCreateCompatiblePixmap)(BEGL_PixmapInfo *pixmapInfo);

   /* Signal that a buffer that was requested for display has actually hit the display.
    * You would normally call this in response to a vsync signal for the displayed buffer, or after you
    * have blitted it if compositing.
    * The buffer will not be re-used until another buffer has been signaled as on-display using this function.
    * See also the commentary on BufferDisplay(). */
   void (*BufferOnDisplay)(uint32_t cb_arg);

   /* Function to return the requirements of a given buffer size, so the application can make its own swap
    * chain and provide it back to GL */
   void (*BufferGetRequirements)(BEGL_PixmapInfo *bufferRequirements, BEGL_BufferSettings * bufferConstrainedRequirements);

} BEGL_DisplayCallbacks;

#ifdef __cplusplus
}
#endif

#endif /* _BEGL_DISPPLATFORM_H__ */
