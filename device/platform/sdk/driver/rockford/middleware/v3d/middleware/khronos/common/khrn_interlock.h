/*=============================================================================
Copyright (c) 2008 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  Interlock system

FILE DESCRIPTION
Interlock system to ensure correct ordering of reads/writes.
=============================================================================*/

#ifndef KHRN_INTERLOCK_H
#define KHRN_INTERLOCK_H

#include "middleware/khronos/common/khrn_mem.h"
#include "middleware/khronos/egl/egl_disp.h"

/* should define KHRN_INTERLOCK_USER_T, KHRN_INTERLOCK_USER_NONE,
 * KHRN_INTERLOCK_USER_TEMP, KHRN_INTERLOCK_USER_WRITING, and
 * KHRN_INTERLOCK_EXTRA_T */
#include "middleware/khronos/common/2708/khrn_interlock_filler_4.h"

typedef struct {
   EGL_DISP_IMAGE_HANDLE_T disp_image_handle;
   KHRN_INTERLOCK_USER_T users;
   KHRN_INTERLOCK_EXTRA_T extra;
} KHRN_INTERLOCK_T;

/*
   platform-independent implementations
*/

extern void khrn_interlock_init(KHRN_INTERLOCK_T *interlock);
extern void khrn_interlock_term(KHRN_INTERLOCK_T *interlock);

extern bool khrn_interlock_read(KHRN_INTERLOCK_T *interlock, KHRN_INTERLOCK_USER_T user); /* user allowed to be KHRN_INTERLOCK_USER_NONE */
extern bool khrn_interlock_write(KHRN_INTERLOCK_T *interlock, KHRN_INTERLOCK_USER_T user); /* user allowed to be KHRN_INTERLOCK_USER_NONE */
extern KHRN_INTERLOCK_USER_T khrn_interlock_get_writer(KHRN_INTERLOCK_T *interlock);
extern bool khrn_interlock_release(KHRN_INTERLOCK_T *interlock, KHRN_INTERLOCK_USER_T user);

extern bool khrn_interlock_write_would_block(KHRN_INTERLOCK_T *interlock);

extern void khrn_interlock_invalidate(KHRN_INTERLOCK_T *interlock);
extern bool khrn_interlock_is_invalid(KHRN_INTERLOCK_T *interlock);

/*
   platform-dependent implementations
*/

extern void khrn_interlock_extra_init(KHRN_INTERLOCK_T *interlock);
extern void khrn_interlock_extra_term(KHRN_INTERLOCK_T *interlock);

extern void khrn_interlock_read_immediate(KHRN_INTERLOCK_T *interlock);
extern void khrn_interlock_write_immediate(KHRN_INTERLOCK_T *interlock);

extern void khrn_interlock_flush(KHRN_INTERLOCK_USER_T user);

#endif
