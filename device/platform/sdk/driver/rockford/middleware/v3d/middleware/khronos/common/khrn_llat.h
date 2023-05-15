/*=============================================================================
Copyright (c) 2008 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  Low-latency thread

FILE DESCRIPTION
A number of interrupts require "immediate" processing that may not possible in
an interrupt context, eg on 2708:
- in response to a frame/flush complete interrupt from the v3d hardware, we want
  to submit the next control list and (possibly) put the just completed frame on
  the display.
- in response to a dispman-x callback informing us that an image is no longer on
  the display, we want to submit the next render control list to the v3d
  hardware if this is all it was waiting for.
- in response to a user program complete interrupt from the v3d hardware, we may
  be in a position to kick off a bin control list.
We have a single "low-latency" high-priority thread to do this processing. The
thread should be idle most of the time as it should only be used for short
processing tasks.
=============================================================================*/

#ifndef KHRN_LLAT_H
#define KHRN_LLAT_H

#include "interface/vcos/vcos.h"

extern bool khrn_llat_init(void);
extern void khrn_llat_term(void);

typedef void (*KHRN_LLAT_CALLBACK_T)(void);
extern uint32_t khrn_llat_register(KHRN_LLAT_CALLBACK_T callback);
extern void khrn_llat_unregister(uint32_t i);
extern void khrn_llat_notify(uint32_t i);

extern void khrn_llat_wait(void);

#endif
