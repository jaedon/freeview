/*=============================================================================
Copyright (c) 2010 Broadcom Europe Limited.
All rights reserved.

Project  :  Default Nexus platform API for EGL driver
Module   :  Nexus platform 

FILE DESCRIPTION
DESC
=============================================================================*/

#include "default_carbon.h"

#include <malloc.h>
#include <memory.h>

BEGL_MemoryInterface *NXPL_CreateMemInterface(void);
BEGL_HWInterface *WINPL_CreateHWInterface(void);
BEGL_DisplayInterface *NXPL_CreateDisplayInterface(BEGL_MemoryInterface *memIface, NEXUS_DisplayHandle display, BEGL_DisplayCallbacks *displayCallbacks);

void NXPL_DestroyMemInterface(BEGL_MemoryInterface *mem);
void WINPL_DestroyHWInterface(BEGL_HWInterface *hw);
void NXPL_DestroyDisplayInterface(BEGL_DisplayInterface *disp);


/*****************************************************************************
 * Registration interface
 *****************************************************************************/
KHAPI void CARPL_RegisterDisplayPlatform(NXPL_PlatformHandle *handle, NEXUS_DisplayHandle display)
{
   BEGL_DriverInterfaces *data = (BEGL_DriverInterfaces*)malloc(sizeof(BEGL_DriverInterfaces));

   if (data != NULL)
   {
      BEGL_GetDefaultDriverInterfaces(data);

      data->memInterface = NXPL_CreateMemInterface();
      data->hwInterface = WINPL_CreateHWInterface();
      data->displayInterface = NXPL_CreateDisplayInterface(data->memInterface, display, &data->displayCallbacks);

      *handle = (NXPL_PlatformHandle)data;

      BEGL_RegisterDriverInterfaces(data);
   }
}

KHAPI void CARPL_UnregisterDisplayPlatform(NXPL_PlatformHandle handle)
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
      WINPL_DestroyHWInterface(data->hwInterface);
      NXPL_DestroyDisplayInterface(data->displayInterface);
   }
}
