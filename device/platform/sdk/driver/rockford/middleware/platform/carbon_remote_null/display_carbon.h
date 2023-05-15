/*=============================================================================
Copyright (c) 2010 Broadcom Europe Limited.
All rights reserved.

Project  :  Default Nexus platform API for EGL driver
Module   :  Nexus platform 

FILE DESCRIPTION
This is a default implementation of a Nexus platform layer used by V3D.
This illustrates one way in which the abstract display interface might be
implemented. You can replace this with your own custom version if preferred.
=============================================================================*/

#ifndef __DISPLAY_CARBON_H__
#define __DISPLAY_CARBON_H__

#include "default_carbon.h"
#include <EGL/egl.h>

BEGL_MemoryInterface  *CARPL_CreateMemInterface(void);
BEGL_HWInterface      *CARPL_CreateHWInterface(void);
BEGL_DisplayInterface *CARPL_CreateDisplayInterface(BEGL_DisplayCallbacks *displayCallbacks);

void CARPL_DestroyMemInterface(BEGL_MemoryInterface *iface);
void CARPL_DestroyHWInterface(BEGL_HWInterface *iface);
void CARPL_DestroyDisplayInterface(BEGL_DisplayInterface *iface);

typedef struct
{
   BEGL_BufferSettings     settings;
} CARPL_BufferData;

#endif
