/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __NATIVE_NJSE_TYPES
#define __NATIVE_NJSE_TYPES

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * This type is used by NJSE to represent booleans.
 */
typedef int NJSEBoolean;
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/**
 * This type is used by NJSE to represent the "number" type of Javascript.
 */
typedef double NJSENumber;

/**
   \defgroup NJSETypes Common types for NJSE native API
   @{
   This are the common types used by the NJSE native interface.
*/

/**
   Return types used by the methods in the NJSE native interface.
*/
typedef enum {
  /** Use this to indicate that the action requested by the method was performed as expected */
  NJSE_RETURN_OK = 0,
  /** Use this to return value 'NULL' */
  NJSE_RETURN_VALUE_NULL = 1,
  /** Use this to return value 'undefined' */
  NJSE_RETURN_VALUE_UNDEFINED = 2,
#ifdef NJSE_ASYNC
  /** Use this to suspend the execution of the javascript engine, if the
   * current function supports it. Use asyncResumeTokenGetter to check if it is
   * supported, and asyncResumer to resume later. */
  NJSE_RETURN_ASYNC = 3,
#endif /* NJSE_ASYNC */
  /** Use this to throw an generic error */
  NJSE_RETURN_ERROR = 4,
  /** Use this in case of an Out Of Memory error */
  NJSE_RETURN_OOM_ERROR = 5,
  /** Use this to throw an error with name 'TypeError' (Used for
	  capabilities check as defined in DAE 7 and DAE 9.3) */
  NJSE_RETURN_TYPE_ERROR = 6,
  /** Use this to throw an error with message 'SecurityError' (Used
	  for security checks as defiend in HBBTV (A.1 table 19) and for
	  OIPF according to definitions in DAE) */
  NJSE_RETURN_SECURITY_ERROR = 7,
  /** Use this for method that has not been implemented */
  NJSE_RETURN_NOT_SUPPORTED = 8
} NJSEReturnCode;

/*@}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __NATIVE_NJSE_TYPES */
