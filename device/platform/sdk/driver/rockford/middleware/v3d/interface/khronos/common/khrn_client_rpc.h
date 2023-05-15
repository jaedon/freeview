/*=============================================================================
Copyright (c) 2008 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  RPC

FILE DESCRIPTION
Khronos RPC API. When RPC_DIRECT is defined, all RPC_CALLs are converted
directly into function calls. Otherwise, we convert them via helper functions
to core API calls. The core API implementation is platform-specific.
=============================================================================*/

#ifndef KHRN_CLIENT_RPC_H
#define KHRN_CLIENT_RPC_H

#define RPC_DELAYED_USE_OF_POINTERS

#include "interface/khronos/common/khrn_int_util.h"
#include "interface/khronos/common/khrn_client.h"

#include <stdlib.h> /* for size_t */

#ifdef __cplusplus
extern "C" {
#endif

# if defined(__SYMBIAN32__)  //use functions defined in khrpc.cpp
#include "rpc_platform.h"
#endif

#include "middleware/khronos/egl/egl_server.h" /* for egl_server_unlock_states() */

/******************************************************************************
type packing/unpacking macros
******************************************************************************/

#define RPC_FLOAT(f)        (f)
#define RPC_ENUM(e)         (e)
#define RPC_INT(i)          (i)
#define RPC_INTPTR(p)       (p)
#define RPC_UINT(u)         (u)
#define RPC_SIZEI(s)        (s)
#define RPC_SIZEIPTR(p)     (p)
#define RPC_BOOLEAN(b)      (b)
#define RPC_BITFIELD(b)     (b)
#define RPC_FIXED(f)        (f)
#define RPC_HANDLE(h)       (h)
#define RPC_EGLID(i)        (i)
#define RPC_FLOAT_RES(f)    (f)
#define RPC_ENUM_RES(e)     (e)
#define RPC_INT_RES(i)      (i)
#define RPC_UINT_RES(u)     (u)
#define RPC_BOOLEAN_RES(b)  (b)
#define RPC_BITFIELD_RES(b) (b)
#define RPC_HANDLE_RES(h)   (h)

/******************************************************************************
rpc call macros
******************************************************************************/

#define RPC_INIT() true
#define RPC_TERM()

#define RPC_FLUSH(thread) RPC_CALL0(khrn_misc_rpc_flush_impl, thread, no_id)
#define RPC_HIGH_PRIORITY_BEGIN(thread)
#define RPC_HIGH_PRIORITY_END(thread)

#define RPC_DO(fn, args) fn args
#define RPC_DO_RES(fn, args) fn args
/*
   RPC_CALL[n](fn, id, RPC_THING(p0), RPC_THING(p1), ...)

   Implementation notes:

   In direct mode, fn is called directly and id is ignored.
   Otherwise fn is ignored and an RPC message is constructed based on id.

   Preconditions:

   p0, p1, etc. satisfy the precondition to fn
   id matches fn
   Server is up (except for a special initialise call)

   Postconditions:

   We promise to call fn on the server at some point in the future. The RPC calls for this
   thread will occur in the same order they are made on the client.
   All postconditions of fn will be satisfied (on the server)

   Invariants preserved:

   -

   Invariants used:

   -
*/
#define RPC_CALL8_MAKECURRENT(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7)                                             RPC_CALL8(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7)

#define RPC_CALL0(fn, thread, id)                                          RPC_DO(fn, ())
#define RPC_CALL1(fn, thread, id, p0)                                      RPC_DO(fn, (p0))
#define RPC_CALL2(fn, thread, id, p0, p1)                                  RPC_DO(fn, (p0, p1))
#define RPC_CALL3(fn, thread, id, p0, p1, p2)                              RPC_DO(fn, (p0, p1, p2))
#define RPC_CALL4(fn, thread, id, p0, p1, p2, p3)                          RPC_DO(fn, (p0, p1, p2, p3))
#define RPC_CALL5(fn, thread, id, p0, p1, p2, p3, p4)                      RPC_DO(fn, (p0, p1, p2, p3, p4))
#define RPC_CALL6(fn, thread, id, p0, p1, p2, p3, p4, p5)                  RPC_DO(fn, (p0, p1, p2, p3, p4, p5))
#define RPC_CALL7(fn, thread, id, p0, p1, p2, p3, p4, p5, p6)              RPC_DO(fn, (p0, p1, p2, p3, p4, p5, p6))
#define RPC_CALL8(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7)          RPC_DO(fn, (p0, p1, p2, p3, p4, p5, p6, p7))
#define RPC_CALL9(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7, p8)      RPC_DO(fn, (p0, p1, p2, p3, p4, p5, p6, p7, p8))
#define RPC_CALL10(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9) RPC_DO(fn, (p0, p1, p2, p3, p4, p5, p6, p7, p8, p9))
#define RPC_CALL16(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15) \
   RPC_DO(fn, (p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15))

/*
   RPC_THING_RES(RPC_CALL[n]_RES(fn, id, RPC_THING(p0), RPC_THING(p1), ...))

   Implementation notes:

   In direct mode, fn is called directly and id is ignored.
   Otherwise fn is ignored and an RPC message is constructed based on id.

   Preconditions:

   p0, p1, etc. satisfy the precondition to fn
   id matches fn
   Server is up (except for a special initialise call)

   Postconditions:

   The call to fn on the server has completed
   We return the return value of fn, and all postconditions of fn are satisfied (on the server)

   Invariants preserved:

   -

   Invariants used:

   -
*/

#define RPC_CALL0_RES(fn, thread, id)                                                    RPC_DO_RES(fn, ())
#define RPC_CALL1_RES(fn, thread, id, p0)                                                RPC_DO_RES(fn, (p0))
#define RPC_CALL2_RES(fn, thread, id, p0, p1)                                            RPC_DO_RES(fn, (p0, p1))
#define RPC_CALL3_RES(fn, thread, id, p0, p1, p2)                                        RPC_DO_RES(fn, (p0, p1, p2))
#define RPC_CALL4_RES(fn, thread, id, p0, p1, p2, p3)                                    RPC_DO_RES(fn, (p0, p1, p2, p3))
#define RPC_CALL5_RES(fn, thread, id, p0, p1, p2, p3, p4)                                RPC_DO_RES(fn, (p0, p1, p2, p3, p4))
#define RPC_CALL6_RES(fn, thread, id, p0, p1, p2, p3, p4, p5)                            RPC_DO_RES(fn, (p0, p1, p2, p3, p4, p5))
#define RPC_CALL7_RES(fn, thread, id, p0, p1, p2, p3, p4, p5, p6)                        RPC_DO_RES(fn, (p0, p1, p2, p3, p4, p5, p6))
#define RPC_CALL8_RES(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7)                    RPC_DO_RES(fn, (p0, p1, p2, p3, p4, p5, p6, p7))
#define RPC_CALL9_RES(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7, p8)                RPC_DO_RES(fn, (p0, p1, p2, p3, p4, p5, p6, p7, p8))
#define RPC_CALL10_RES(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9)           RPC_DO_RES(fn, (p0, p1, p2, p3, p4, p5, p6, p7, p8, p9))
#define RPC_CALL11_RES(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)      RPC_DO_RES(fn, (p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10))
#define RPC_CALL12_RES(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11) RPC_DO_RES(fn, (p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11))

/*
   message with data in/out via control channel
*/

#define RPC_CALL1_IN_CTRL(fn, thread, id, in, len)                                      RPC_DO(fn, (in))
#define RPC_CALL2_IN_CTRL(fn, thread, id, p0, in, len)                                  RPC_DO(fn, (p0, in))
#define RPC_CALL3_IN_CTRL(fn, thread, id, p0, p1, in, len)                              RPC_DO(fn, (p0, p1, in))
#define RPC_CALL4_IN_CTRL(fn, thread, id, p0, p1, p2, in, len)                          RPC_DO(fn, (p0, p1, p2, in))
#define RPC_CALL5_IN_CTRL(fn, thread, id, p0, p1, p2, p3, in, len)                      RPC_DO(fn, (p0, p1, p2, p3, in))
#define RPC_CALL6_IN_CTRL(fn, thread, id, p0, p1, p2, p3, p4, in, len)                  RPC_DO(fn, (p0, p1, p2, p3, p4, in))
#define RPC_CALL7_IN_CTRL(fn, thread, id, p0, p1, p2, p3, p4, p5, in, len)              RPC_DO(fn, (p0, p1, p2, p3, p4, p5, in))
#define RPC_CALL8_IN_CTRL(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, in, len)          RPC_DO(fn, (p0, p1, p2, p3, p4, p5, p6, in))
#define RPC_CALL9_IN_CTRL(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7, in, len)      RPC_DO(fn, (p0, p1, p2, p3, p4, p5, p6, p7, in))
#define RPC_CALL10_IN_CTRL(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7, p8, in, len) RPC_DO(fn, (p0, p1, p2, p3, p4, p5, p6, p7, p8, in))

/*
   RPC_CALL[n]_OUT_CTRL(fn, id, RPC_THING(p0), RPC_THING(p1), ..., out)

   Implementation notes:

   In direct mode, fn is called directly and id is ignored.
   Otherwise fn is ignored and an RPC message is constructed based on id.
   The dispatch code is responsible for calculating the length of the returned data.

   Preconditions:

   p0, p1, ..., out satisfy the precondition to fn
   id matches fn
   Server is up (except for a special initialise call)

   Postconditions:

   The call to fn on the server has completed
   We return whatever fn returned in out

   Invariants preserved:

   -

   Invariants used:

   -
*/

#define RPC_CALL1_OUT_CTRL(fn, thread, id, out)                                               RPC_DO(fn, (out))
#define RPC_CALL2_OUT_CTRL(fn, thread, id, p0, out)                                           RPC_DO(fn, (p0, out))
#define RPC_CALL3_OUT_CTRL(fn, thread, id, p0, p1, out)                                       RPC_DO(fn, (p0, p1, out))
#define RPC_CALL4_OUT_CTRL(fn, thread, id, p0, p1, p2, out)                                   RPC_DO(fn, (p0, p1, p2, out))
#define RPC_CALL5_OUT_CTRL(fn, thread, id, p0, p1, p2, p3, out)                               RPC_DO(fn, (p0, p1, p2, p3, out))
#define RPC_CALL6_OUT_CTRL(fn, thread, id, p0, p1, p2, p3, p4, out)                           RPC_DO(fn, (p0, p1, p2, p3, p4, out))
#define RPC_CALL7_OUT_CTRL(fn, thread, id, p0, p1, p2, p3, p4, p5, out)                       RPC_DO(fn, (p0, p1, p2, p3, p4, p5, out))
#define RPC_CALL8_OUT_CTRL(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, out)                   RPC_DO(fn, (p0, p1, p2, p3, p4, p5, p6, out))
#define RPC_CALL9_OUT_CTRL(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7, out)               RPC_DO(fn, (p0, p1, p2, p3, p4, p5, p6, p7, out))
#define RPC_CALL10_OUT_CTRL(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7, p8, out)          RPC_DO(fn, (p0, p1, p2, p3, p4, p5, p6, p7, p8, out))
#define RPC_CALL11_OUT_CTRL(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, out)      RPC_DO(fn, (p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, out))
#define RPC_CALL12_OUT_CTRL(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, out) RPC_DO(fn, (p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, out))
#define RPC_CALL13_OUT_CTRL(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, out)      RPC_DO(fn, (p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, out))
#define RPC_CALL14_OUT_CTRL(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, out) RPC_DO(fn, (p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, out))
#define RPC_CALL15_OUT_CTRL(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, out) RPC_DO(fn, (p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, out))
#define RPC_CALL16_OUT_CTRL(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, out) RPC_DO(fn, (p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, out))

#define RPC_CALL1_OUT_CTRL_RES(fn, thread, id, out)                                      RPC_DO_RES(fn, (out))
#define RPC_CALL2_OUT_CTRL_RES(fn, thread, id, p0, out)                                  RPC_DO_RES(fn, (p0, out))
#define RPC_CALL3_OUT_CTRL_RES(fn, thread, id, p0, p1, out)                              RPC_DO_RES(fn, (p0, p1, out))
#define RPC_CALL4_OUT_CTRL_RES(fn, thread, id, p0, p1, p2, out)                          RPC_DO_RES(fn, (p0, p1, p2, out))
#define RPC_CALL5_OUT_CTRL_RES(fn, thread, id, p0, p1, p2, p3, out)                      RPC_DO_RES(fn, (p0, p1, p2, p3, out))
#define RPC_CALL6_OUT_CTRL_RES(fn, thread, id, p0, p1, p2, p3, p4, out)                  RPC_DO_RES(fn, (p0, p1, p2, p3, p4, out))
#define RPC_CALL7_OUT_CTRL_RES(fn, thread, id, p0, p1, p2, p3, p4, p5, out)              RPC_DO_RES(fn, (p0, p1, p2, p3, p4, p5, out))
#define RPC_CALL8_OUT_CTRL_RES(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, out)          RPC_DO_RES(fn, (p0, p1, p2, p3, p4, p5, p6, out))
#define RPC_CALL9_OUT_CTRL_RES(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7, out)      RPC_DO_RES(fn, (p0, p1, p2, p3, p4, p5, p6, p7, out))
#define RPC_CALL10_OUT_CTRL_RES(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7, p8, out) RPC_DO_RES(fn, (p0, p1, p2, p3, p4, p5, p6, p7, p8, out))

/*
   message with data in/out via bulk channel
*/

#define RPC_CALL1_IN_BULK(fn, thread, id, in, len)                                      RPC_DO(fn, (in))
#define RPC_CALL2_IN_BULK(fn, thread, id, p0, in, len)                                  RPC_DO(fn, (p0, in))
#define RPC_CALL3_IN_BULK(fn, thread, id, p0, p1, in, len)                              RPC_DO(fn, (p0, p1, in))
#define RPC_CALL4_IN_BULK(fn, thread, id, p0, p1, p2, in, len)                          RPC_DO(fn, (p0, p1, p2, in))
#define RPC_CALL5_IN_BULK(fn, thread, id, p0, p1, p2, p3, in, len)                      RPC_DO(fn, (p0, p1, p2, p3, in))
#define RPC_CALL6_IN_BULK(fn, thread, id, p0, p1, p2, p3, p4, in, len)                  RPC_DO(fn, (p0, p1, p2, p3, p4, in))
#define RPC_CALL7_IN_BULK(fn, thread, id, p0, p1, p2, p3, p4, p5, in, len)              RPC_DO(fn, (p0, p1, p2, p3, p4, p5, in))
#define RPC_CALL8_IN_BULK(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, in, len)          RPC_DO(fn, (p0, p1, p2, p3, p4, p5, p6, in))
#define RPC_CALL9_IN_BULK(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7, in, len)      RPC_DO(fn, (p0, p1, p2, p3, p4, p5, p6, p7, in))
#define RPC_CALL10_IN_BULK(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7, p8, in, len) RPC_DO(fn, (p0, p1, p2, p3, p4, p5, p6, p7, p8, in))

#define RPC_CALL1_OUT_BULK(fn, thread, id, out)                                      RPC_DO(fn, (out))
#define RPC_CALL2_OUT_BULK(fn, thread, id, p0, out)                                  RPC_DO(fn, (p0, out))
#define RPC_CALL3_OUT_BULK(fn, thread, id, p0, p1, out)                              RPC_DO(fn, (p0, p1, out))
#define RPC_CALL4_OUT_BULK(fn, thread, id, p0, p1, p2, out)                          RPC_DO(fn, (p0, p1, p2, out))
#define RPC_CALL5_OUT_BULK(fn, thread, id, p0, p1, p2, p3, out)                      RPC_DO(fn, (p0, p1, p2, p3, out))
#define RPC_CALL6_OUT_BULK(fn, thread, id, p0, p1, p2, p3, p4, out)                  RPC_DO(fn, (p0, p1, p2, p3, p4, out))
#define RPC_CALL7_OUT_BULK(fn, thread, id, p0, p1, p2, p3, p4, p5, out)              RPC_DO(fn, (p0, p1, p2, p3, p4, p5, out))
#define RPC_CALL8_OUT_BULK(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, out)          RPC_DO(fn, (p0, p1, p2, p3, p4, p5, p6, out))
#define RPC_CALL9_OUT_BULK(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7, out)      RPC_DO(fn, (p0, p1, p2, p3, p4, p5, p6, p7, out))
#define RPC_CALL10_OUT_BULK(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7, p8, out) RPC_DO(fn, (p0, p1, p2, p3, p4, p5, p6, p7, p8, out))

#define RPC_CALL1_IN_BULK_RES(fn, thread, id, in, len)                                      RPC_DO_RES(fn, (in))
#define RPC_CALL2_IN_BULK_RES(fn, thread, id, p0, in, len)                                  RPC_DO_RES(fn, (p0, in))
#define RPC_CALL3_IN_BULK_RES(fn, thread, id, p0, p1, in, len)                              RPC_DO_RES(fn, (p0, p1, in))
#define RPC_CALL4_IN_BULK_RES(fn, thread, id, p0, p1, p2, in, len)                          RPC_DO_RES(fn, (p0, p1, p2, in))
#define RPC_CALL5_IN_BULK_RES(fn, thread, id, p0, p1, p2, p3, in, len)                      RPC_DO_RES(fn, (p0, p1, p2, p3, in))
#define RPC_CALL6_IN_BULK_RES(fn, thread, id, p0, p1, p2, p3, p4, in, len)                  RPC_DO_RES(fn, (p0, p1, p2, p3, p4, in))
#define RPC_CALL7_IN_BULK_RES(fn, thread, id, p0, p1, p2, p3, p4, p5, in, len)              RPC_DO_RES(fn, (p0, p1, p2, p3, p4, p5, in))
#define RPC_CALL8_IN_BULK_RES(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, in, len)          RPC_DO_RES(fn, (p0, p1, p2, p3, p4, p5, p6, in))
#define RPC_CALL9_IN_BULK_RES(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7, in, len)      RPC_DO_RES(fn, (p0, p1, p2, p3, p4, p5, p6, p7, in))
#define RPC_CALL10_IN_BULK_RES(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7, p8, in, len) RPC_DO_RES(fn, (p0, p1, p2, p3, p4, p5, p6, p7, p8, in))

#define RPC_CALL1_OUT_BULK_RES(fn, thread, id, out)                                      RPC_DO_RES(fn, (out))
#define RPC_CALL2_OUT_BULK_RES(fn, thread, id, p0, out)                                  RPC_DO_RES(fn, (p0, out))
#define RPC_CALL3_OUT_BULK_RES(fn, thread, id, p0, p1, out)                              RPC_DO_RES(fn, (p0, p1, out))
#define RPC_CALL4_OUT_BULK_RES(fn, thread, id, p0, p1, p2, out)                          RPC_DO_RES(fn, (p0, p1, p2, out))
#define RPC_CALL5_OUT_BULK_RES(fn, thread, id, p0, p1, p2, p3, out)                      RPC_DO_RES(fn, (p0, p1, p2, p3, out))
#define RPC_CALL6_OUT_BULK_RES(fn, thread, id, p0, p1, p2, p3, p4, out)                  RPC_DO_RES(fn, (p0, p1, p2, p3, p4, out))
#define RPC_CALL7_OUT_BULK_RES(fn, thread, id, p0, p1, p2, p3, p4, p5, out)              RPC_DO_RES(fn, (p0, p1, p2, p3, p4, p5, out))
#define RPC_CALL8_OUT_BULK_RES(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, out)          RPC_DO_RES(fn, (p0, p1, p2, p3, p4, p5, p6, out))
#define RPC_CALL9_OUT_BULK_RES(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7, out)      RPC_DO_RES(fn, (p0, p1, p2, p3, p4, p5, p6, p7, out))
#define RPC_CALL10_OUT_BULK_RES(fn, thread, id, p0, p1, p2, p3, p4, p5, p6, p7, p8, out) RPC_DO_RES(fn, (p0, p1, p2, p3, p4, p5, p6, p7, p8, out))

#ifdef __cplusplus
 }
#endif
#endif
