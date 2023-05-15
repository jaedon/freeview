/*=============================================================================
Copyright (c) 2010 Broadcom Europe Limited.
All rights reserved.

Project  :  Default RSO Android platform API for EGL driver
Module   :  RSO Android platform on NEXUS

FILE DESCRIPTION
DESC
=============================================================================*/

#include "default_RSO_android.h"

#include <EGL/egl.h>
#include <malloc.h>
#include <string.h>

/* declare external interface */
typedef void (*BufferOnDisplayFunc)(uint32_t);

BEGL_MemoryInterface  *NXPL_CreateMemInterface(BEGL_HWInterface *hwIface);
BEGL_HWInterface      *NXPL_CreateHWInterface(void);

BEGL_DisplayInterface *RSOANPL_CreateDisplayInterface(BEGL_MemoryInterface *mem,
                                                      BEGL_HWInterface *hw,
                                                      BEGL_DisplayCallbacks *displayCallbacks);

void NXPL_DestroyMemInterface(BEGL_MemoryInterface *iface);
void NXPL_DestroyHWInterface(BEGL_HWInterface *iface);
void RSOANPL_DestroyDisplayInterface(BEGL_DisplayInterface * disp);

/*****************************************************************************
 * Registration interface
 *****************************************************************************/
/* Register Android with the egl platform */
void RSOANPL_RegisterAndroidDisplayPlatform(RSOANPL_PlatformHandle *handle, android_native_window_t *awin)
{
   BEGL_DriverInterfaces *installed_data = BEGL_GetDriverInterfaces();
   BEGL_DriverInterfaces *data = (BEGL_DriverInterfaces*)malloc(sizeof(BEGL_DriverInterfaces));
   memset(data, 0, sizeof(BEGL_DriverInterfaces));
   LOGD("%s: Enter \n",__FUNCTION__);

   if (data != NULL)
   {
      LOGD("%s-%d: Calling BEGL_GetDefaultDriverInterfaces \n", __FUNCTION__, __LINE__);
      BEGL_GetDefaultDriverInterfaces(data);

      if (installed_data)
      {
         if (installed_data->hwInterfaceFn == NXPL_CreateHWInterface)
         {
            LOGD("%s-%d: We already Have HW Interfaces....Just reusing them\n", __FUNCTION__, __LINE__);
            data->hwInterface = installed_data->hwInterface;
            data->hwInterfaceFn = installed_data->hwInterfaceFn;
            data->hwInterfaceCloned = 1;
         }
         else
         {
            LOGD("%s-%d: Calling NXPL_CreateHWInterface\n", __FUNCTION__, __LINE__);
            data->hwInterface = NXPL_CreateHWInterface();
            /* keep a copy of the function we called, so we don't need to call again later */
            data->hwInterfaceFn = NXPL_CreateHWInterface;
            data->hwInterfaceCloned = 0;
         }

         if (installed_data->memInterfaceFn == NXPL_CreateMemInterface)
         {
            LOGD("%s-%d: We already Have Mem Interfaces....Just reusing them\n", __FUNCTION__, __LINE__);
            data->memInterface = installed_data->memInterface;
            data->memInterfaceFn = installed_data->memInterfaceFn;
            data->memInterfaceCloned = 1;
         }
         else
         {
            LOGD("%s-%d: Calling NXPL_CreateMemInterface\n", __FUNCTION__, __LINE__);
            data->memInterface = NXPL_CreateMemInterface(data->hwInterface);
            /* keep a copy of the function we called, so we don't need to call again later */
            data->memInterfaceFn = NXPL_CreateMemInterface;
            data->memInterfaceCloned = 0;
         }

      }
      else
      {
         LOGD("%s-%d: Calling NXPL_CreateHWInterface\n", __FUNCTION__, __LINE__);
         data->hwInterface = NXPL_CreateHWInterface();
         /* keep a copy of the function we called, so we don't need to call again later */
         data->hwInterfaceFn = NXPL_CreateHWInterface;
         data->memInterfaceCloned = 0;

         LOGD("%s-%d: Calling NXPL_CreateMemInterface\n", __FUNCTION__, __LINE__);
         data->memInterface = NXPL_CreateMemInterface(data->hwInterface);
         /* keep a copy of the function we called, so we don't need to call again later */
         data->memInterfaceFn = NXPL_CreateMemInterface;
         data->memInterfaceCloned = 0;
      }

      LOGD("%s-%d: Calling RSOANPL_CreateDisplayInterface\n", __FUNCTION__, __LINE__);
      data->displayInterface = RSOANPL_CreateDisplayInterface(data->memInterface, data->hwInterface, &data->displayCallbacks);

      *handle = (RSOANPL_PlatformHandle)data;

      LOGD("%s-%d: Calling BEGL_RegisterDriverInterfaces\n", __FUNCTION__, __LINE__);
      BEGL_RegisterDriverInterfaces(data);
   }

   LOGD("%s: Exit \n",__FUNCTION__);
}

/* Unregister Android */
void RSOANPL_UnregisterAndroidDisplayPlatform(RSOANPL_PlatformHandle handle)
{
   BEGL_DriverInterfaces *data = (BEGL_DriverInterfaces*)handle;
   LOGD("%s: Enter \n",__FUNCTION__);
   if (data != NULL)
   {
      /* Clear out all of our interface pointers and register the empty ones.
       * Do this before destroying the interfaces in case the driver needs to use them
       * to shutdown correctly */
      BEGL_DriverInterfaces nulliface;
      memset(&nulliface, 0, sizeof(BEGL_DriverInterfaces));
      BEGL_GetDefaultDriverInterfaces(&nulliface);
      BEGL_RegisterDriverInterfaces(&nulliface);

      /* don't delete cloned interfaces */
      if (data->memInterfaceCloned == 0)
         NXPL_DestroyMemInterface(data->memInterface);

      /* don't delete cloned interfaces */
      if (data->hwInterfaceCloned == 0)
         NXPL_DestroyHWInterface(data->hwInterface);
      RSOANPL_DestroyDisplayInterface(data->displayInterface);

      memset(data, 0, sizeof(BEGL_DriverInterfaces));
      free(data);
   }

   LOGD("%s: Exit \n",__FUNCTION__);
}
