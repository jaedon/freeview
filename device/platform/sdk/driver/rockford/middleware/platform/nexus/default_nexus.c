/*=============================================================================
Copyright (c) 2010 Broadcom Europe Limited.
All rights reserved.

Project  :  Default Nexus platform API for EGL driver
Module   :  Nexus platform 

FILE DESCRIPTION
DESC
=============================================================================*/

#include "default_nexus.h"

#include "display_nexus.h"

#include <EGL/egl.h>

#include <malloc.h>
#include <memory.h>

#define UNUSED(X) (void)X

/*****************************************************************************
 * Registration interface
 *****************************************************************************/
/* Register a display for exclusive use. The client application should not use the display until 
 * calling NXPL_UnregisterNexusDisplayPlatform. 
 * It will register its own memory, h/w and display APIs suitable for basic exclusive mode rendering on 
 * a Nexus display. */
#ifdef SINGLE_PROCESS
void NXPL_RegisterNexusDisplayPlatform(NXPL_PlatformHandle *handle, NEXUS_DISPLAYHANDLE display)
{
   BEGL_DriverInterfaces *data = (BEGL_DriverInterfaces*)malloc(sizeof(BEGL_DriverInterfaces));
   memset(data, 0, sizeof(BEGL_DriverInterfaces));

   if (data != NULL)
   {
      BEGL_GetDefaultDriverInterfaces(data);

      data->hwInterface      = NXPL_CreateHWInterface();
      data->memInterface     = NXPL_CreateMemInterface(data->hwInterface);
      data->displayInterface = NXPL_CreateDisplayInterface(data->memInterface, display, 
                                                           &data->displayCallbacks);

      *handle = (NXPL_PlatformHandle)data;

      BEGL_RegisterDriverInterfaces(data);
   }
}

#if !defined(BLURAY_PLATFORM) || defined(NULL_DISPLAY)
/* Register for composited mode usage. */
void NXPL_RegisterNexusCompositedDisplayPlatform(NXPL_PlatformHandle *handle, NXPL_CompositingData *compData)
{
   BEGL_DriverInterfaces *data = (BEGL_DriverInterfaces*)malloc(sizeof(BEGL_DriverInterfaces));
   memset(data, 0, sizeof(BEGL_DriverInterfaces));

   if (data != NULL)
   {
      BEGL_GetDefaultDriverInterfaces(data);

      data->hwInterface      = NXPL_CreateHWInterface();
      data->memInterface     = NXPL_CreateMemInterface(data->hwInterface);
      data->displayInterface = NXPL_CreateCompositedDisplayInterface(data->memInterface, compData, 
                                                                     &data->displayCallbacks);

      *handle = (NXPL_PlatformHandle)data;

      BEGL_RegisterDriverInterfaces(data);
   }
}
#endif /* BLURAY_PLATFORM */
#else
/* MULTI PROCESS */
void NXPL_RegisterNexusDisplayPlatform(NXPL_PlatformHandle *handle, NEXUS_DISPLAYHANDLE display /*ignored*/)
{
   BEGL_DriverInterfaces *data = (BEGL_DriverInterfaces*)malloc(sizeof(BEGL_DriverInterfaces));

   UNUSED(display);

   memset(data, 0, sizeof(BEGL_DriverInterfaces));

   if (data != NULL)
   {
      BEGL_GetDefaultDriverInterfaces(data);

      data->hwInterface      = NXPL_CreateHWInterface();
      data->memInterface     = NXPL_CreateMemInterface(data->hwInterface);
      data->displayInterface = NXPL_CreateDisplayInterface(data->memInterface, &data->displayCallbacks);

      *handle = (NXPL_PlatformHandle)data;

      BEGL_RegisterDriverInterfaces(data);
   }
}
#endif

/* Unregister a display for exclusive use. The client application can then use the Nexus display again. */
void NXPL_UnregisterNexusDisplayPlatform(NXPL_PlatformHandle handle)
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

      NXPL_DestroyDisplayInterface(data->displayInterface);
      NXPL_DestroyMemInterface(data->memInterface);
      NXPL_DestroyHWInterface(data->hwInterface);

      memset(data, 0, sizeof(BEGL_DriverInterfaces));
      free(data);
   }
}

#ifndef NULL_DISPLAY
/* Unregister a display. The client application can then use the Nexus display again. */
void NXPL_UnregisterNexusCompositedDisplayPlatform(NXPL_PlatformHandle handle)
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

      NXPL_DestroyDisplayInterface(data->displayInterface);
      NXPL_DestroyMemInterface(data->memInterface);
      NXPL_DestroyHWInterface(data->hwInterface);

      memset(data, 0, sizeof(BEGL_DriverInterfaces));
      free(data);
   }
}
#endif /* NULL_DISPLAY */

