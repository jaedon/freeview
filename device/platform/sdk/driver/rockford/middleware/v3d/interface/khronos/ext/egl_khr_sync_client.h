/*=============================================================================
Copyright (c) 2008 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  Header file

FILE DESCRIPTION
Client-side implementation of the EGL_KHR_sync extension.
=============================================================================*/


#include "interface/khronos/common/khrn_int_common.h"

#include "interface/khronos/common/khrn_client.h"
#include "interface/khronos/common/khrn_client_rpc.h"
#include "middleware/khronos/egl/egl_server.h"

typedef struct {
   EGLint condition;
   EGLint threshold;
   EGLenum type;

   int sem[3];

   EGL_SYNC_ID_T serversync;

   /*
      we keep one master handle to the named semaphore in existence for the
      lifetime of the sync object, allowing both wait functions and the KHAN
      message handler to "open, post/wait, close".
   */

   PLATFORM_SEMAPHORE_T master;
} EGL_SYNC_T;

void egl_sync_term(EGL_SYNC_T *sync);
