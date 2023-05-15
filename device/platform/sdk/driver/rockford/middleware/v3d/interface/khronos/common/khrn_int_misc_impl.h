/*=============================================================================
Copyright (c) 2008 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  External interface
File     :  $RCSfile$
Revision :  $Revision$

FILE DESCRIPTION
Miscellaneous server-side implementation functions.
=============================================================================*/
#if defined(KHRN_IMPL_STRUCT)
#define FN(type, name, args) type (*name) args;
#elif defined(KHRN_IMPL_STRUCT_INIT)
#define FN(type, name, args) name,
#else
#define FN(type, name, args) extern type name args;
#endif

FN(void, khrn_misc_fifo_finish_impl, (void))
FN(void, khrn_misc_rpc_flush_impl, (void))

#undef FN
