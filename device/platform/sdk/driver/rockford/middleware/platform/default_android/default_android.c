/*=============================================================================
Copyright (c) 2010 Broadcom Europe Limited.
All rights reserved.

Project  :  Default RSO Android platform API for EGL driver
Module   :  RSO Android platform on NEXUS

FILE DESCRIPTION
DESC
=============================================================================*/

#include "default_android.h"

#include <EGL/egl.h>
#include <malloc.h>
#include <string.h>

/* declare external interface */
typedef void (*BufferOnDisplayFunc)(uint32_t);

BEGL_MemoryInterface  *NXPL_CreateMemInterface(BEGL_HWInterface *hwIface);
BEGL_HWInterface      *NXPL_CreateHWInterface(void);
BEGL_DisplayInterface *ANPL_CreateDisplayInterface(BEGL_MemoryInterface *mem, BEGL_HWInterface *hw, BEGL_DisplayCallbacks *displayCallbacks);

void NXPL_DestroyMemInterface(BEGL_MemoryInterface *iface);
void NXPL_DestroyHWInterface(BEGL_HWInterface *iface);
void ANPL_DestroyDisplayInterface(BEGL_DisplayInterface * disp);

/*****************************************************************************
 * Registration interface
 *****************************************************************************/
/* Register Android with the egl platform */
void ANPL_RegisterAndroidDisplayPlatform(ANPL_PlatformHandle *handle, android_native_window_t *awin)
{
   BEGL_DriverInterfaces *data = (BEGL_DriverInterfaces*)malloc(sizeof(BEGL_DriverInterfaces));
   memset(data, 0, sizeof(BEGL_DriverInterfaces));

   if (data != NULL)
   {
      BEGL_GetDefaultDriverInterfaces(data);

      data->hwInterface = NXPL_CreateHWInterface();
      data->hwInterfaceFn = (void *) NXPL_CreateHWInterface;

      data->memInterface = NXPL_CreateMemInterface(data->hwInterface);
      data->memInterfaceFn = (void *) NXPL_CreateMemInterface;

      data->displayInterface = ANPL_CreateDisplayInterface(data->memInterface, data->hwInterface, &data->displayCallbacks);

      *handle = (ANPL_PlatformHandle)data;

      BEGL_RegisterDriverInterfaces(data);
   }
}

/* Unregister Android */
void ANPL_UnregisterAndroidDisplayPlatform(ANPL_PlatformHandle handle)
{
   BEGL_DriverInterfaces *data = (BEGL_DriverInterfaces*)handle;

   if (data != NULL)
   {
      /* Clear out all of our interface pointers and register the empty ones.
       * Do this before destroying the interfaces in case the driver needs to use them
       * to shutdown correctly */
      BEGL_DriverInterfaces nulliface;
      memset(&nulliface, 0, sizeof(BEGL_DriverInterfaces));
      BEGL_GetDefaultDriverInterfaces(&nulliface);
      BEGL_RegisterDriverInterfaces(&nulliface);

      NXPL_DestroyMemInterface(data->memInterface);
      NXPL_DestroyHWInterface(data->hwInterface);
      ANPL_DestroyDisplayInterface(data->displayInterface);

      memset(data, 0, sizeof(BEGL_DriverInterfaces));
      free(data);
   }
}
