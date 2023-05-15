#include <cutils/log.h>
#include "default_android.h"

void __attribute__ ((constructor)) v3d_android_load(void);
void __attribute__ ((destructor)) v3d_android_unload(void);

/* These functions are defined in nexus_egl_client.cpp (part of libnexuseglclient.so) */
void* EGL_nexus_join(char *client_process_name);
void EGL_nexus_unjoin(void *nexus_client);

static ANPL_PlatformHandle s_platformHandle;
static void *nexus_client = NULL;

void *v3d_get_nexus_client_context(void)
{
   return nexus_client;
}

/* Called when the library is loaded and before dlopen() returns */
void v3d_android_load(void)
   {
   nexus_client = EGL_nexus_join("libegl_nexus");
   if (nexus_client == NULL)
      {
      LOGE("EGL_nexus_join [CONSTRUCTOR] Failed\n");
   }

   /* NOTE! we may need a way to return back the default display */
   ANPL_RegisterAndroidDisplayPlatform(&s_platformHandle, NULL);
}

/* Called when the library is unloaded and before dlclose() */
void v3d_android_unload(void)
{
   ANPL_UnregisterAndroidDisplayPlatform(s_platformHandle);

   EGL_nexus_unjoin(nexus_client);
   nexus_client = NULL;
}
