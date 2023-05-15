/***************************************************************
* $Workfile:   ci_cc_x509_asn_lib.h  $
* $Modtime:
* Auther : hmkim / dslee
*
* Desc : Content Control (CI+) Addon for X.509 parsing
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
****************************************************************/

/*-
 * Copyright (c) 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef ASN_LIB_H
#define ASN_LIB_H

/* Copyright (C) 1997, 1998, 1999, 2000, 2001 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

/*
 *	ISO C99: 7.18 Integer types <stdint.h>
 */

#ifndef _STDINT_H
#define _STDINT_H	1

#define __WORDSIZE 32

/* Exact integral types.  */

/* Signed.  */

/* There is some amount of overlap with <sys/types.h> as known by inet code */
#ifndef __int8_t_defined
# define __int8_t_defined
typedef signed char		int8_t;
typedef short int		int16_t;
typedef int			int32_t;
# if __WORDSIZE == 64
typedef long int		int64_t;
# else
typedef long long int		int64_t;
# endif
#endif

/* Unsigned.  */
typedef unsigned char		uint8_t;
typedef unsigned short int	uint16_t;
#ifndef __uint32_t_defined
typedef unsigned int		uint32_t;
# define __uint32_t_defined
#endif
#if __WORDSIZE == 64
typedef unsigned long int	uint64_t;
#else
typedef unsigned long long int	uint64_t;
#endif

/* Small types.  */

/* Signed.  */
typedef signed char		int_least8_t;
typedef short int		int_least16_t;
typedef int			int_least32_t;
#if __WORDSIZE == 64
typedef long int		int_least64_t;
#else
typedef long long int		int_least64_t;
#endif

/* Unsigned.  */
typedef unsigned char		uint_least8_t;
typedef unsigned short int	uint_least16_t;
typedef unsigned int		uint_least32_t;
#if __WORDSIZE == 64
typedef unsigned long int	uint_least64_t;
#else
typedef unsigned long long int	uint_least64_t;
#endif

/* Fast types.  */

/* Signed.  */
typedef signed char		int_fast8_t;
#if __WORDSIZE == 64
typedef long int		int_fast16_t;
typedef long int		int_fast32_t;
typedef long int		int_fast64_t;
#else
typedef int			int_fast16_t;
typedef int			int_fast32_t;
typedef long long int		int_fast64_t;
#endif

/* Unsigned.  */
typedef unsigned char		uint_fast8_t;
#if __WORDSIZE == 64
typedef unsigned long int	uint_fast16_t;
typedef unsigned long int	uint_fast32_t;
typedef unsigned long int	uint_fast64_t;
#else
typedef unsigned int		uint_fast16_t;
typedef unsigned int		uint_fast32_t;
typedef unsigned long long int	uint_fast64_t;
#endif

/* Types for `void *' pointers.  */
#if __WORDSIZE == 64
# ifndef __intptr_t_defined
typedef long int		intptr_t;
#  define __intptr_t_defined
# endif
typedef unsigned long int	uintptr_t;
#else
# ifndef __intptr_t_defined
typedef int			intptr_t;
#  define __intptr_t_defined
# endif
typedef unsigned int		uintptr_t;
#endif

/* Largest integral types.  */
#if __WORDSIZE == 64
typedef long int		intmax_t;
typedef unsigned long int	uintmax_t;
#else

typedef long long int		intmax_t;

typedef unsigned long long int	uintmax_t;
#endif

/* The ISO C99 standard specifies that in C++ implementations these
   macros should only be defined if explicitly requested.  */
#if !defined __cplusplus || defined __STDC_LIMIT_MACROS

# if __WORDSIZE == 64
#  define __INT64_C(c)	c ## L
#  define __UINT64_C(c)	c ## UL
# else
#  define __INT64_C(c)	c ## LL
#  define __UINT64_C(c)	c ## ULL
# endif

/* Limits of integral types.  */

/* Minimum of signed integral types.  */
# define INT8_MIN		(-128)
# define INT16_MIN		(-32767-1)
# define INT32_MIN		(-2147483647-1)
# define INT64_MIN		(-__INT64_C(9223372036854775807)-1)

/* Maximum of signed integral types.  */
# define INT8_MAX		(127)
# define INT16_MAX		(32767)
# define INT32_MAX		(2147483647)
# define INT64_MAX		(__INT64_C(9223372036854775807))

/* Maximum of unsigned integral types.  */
# define UINT8_MAX		(255)
# define UINT16_MAX		(65535)
# define UINT32_MAX		(4294967295U)
# define UINT64_MAX		(__UINT64_C(18446744073709551615))

/* Minimum of signed integral types having a minimum size.  */
# define INT_LEAST8_MIN		(-128)
# define INT_LEAST16_MIN	(-32767-1)
# define INT_LEAST32_MIN	(-2147483647-1)
# define INT_LEAST64_MIN	(-__INT64_C(9223372036854775807)-1)

/* Maximum of signed integral types having a minimum size.  */
# define INT_LEAST8_MAX		(127)
# define INT_LEAST16_MAX	(32767)
# define INT_LEAST32_MAX	(2147483647)
# define INT_LEAST64_MAX	(__INT64_C(9223372036854775807))

/* Maximum of unsigned integral types having a minimum size.  */
# define UINT_LEAST8_MAX	(255)
# define UINT_LEAST16_MAX	(65535)
# define UINT_LEAST32_MAX	(4294967295U)
# define UINT_LEAST64_MAX	(__UINT64_C(18446744073709551615))

/* Minimum of fast signed integral types having a minimum size.  */
# define INT_FAST8_MIN		(-128)
# if __WORDSIZE == 64
#  define INT_FAST16_MIN	(-9223372036854775807L-1)
#  define INT_FAST32_MIN	(-9223372036854775807L-1)
# else
#  define INT_FAST16_MIN	(-2147483647-1)
#  define INT_FAST32_MIN	(-2147483647-1)
# endif
# define INT_FAST64_MIN		(-__INT64_C(9223372036854775807)-1)

/* Maximum of fast signed integral types having a minimum size.  */
# define INT_FAST8_MAX		(127)
# if __WORDSIZE == 64
#  define INT_FAST16_MAX	(9223372036854775807L)
#  define INT_FAST32_MAX	(9223372036854775807L)
# else
#  define INT_FAST16_MAX	(2147483647)
#  define INT_FAST32_MAX	(2147483647)
# endif
# define INT_FAST64_MAX		(__INT64_C(9223372036854775807))

/* Maximum of fast unsigned integral types having a minimum size.  */
# define UINT_FAST8_MAX		(255)
# if __WORDSIZE == 64
#  define UINT_FAST16_MAX	(18446744073709551615UL)
#  define UINT_FAST32_MAX	(18446744073709551615UL)
# else
#  define UINT_FAST16_MAX	(4294967295U)
#  define UINT_FAST32_MAX	(4294967295U)
# endif
# define UINT_FAST64_MAX	(__UINT64_C(18446744073709551615))

/* Values to test for integral types holding `void *' pointer.  */
# if __WORDSIZE == 64
#  define INTPTR_MIN		(-9223372036854775807L-1)
#  define INTPTR_MAX		(9223372036854775807L)
#  define UINTPTR_MAX		(18446744073709551615UL)
# else
#  define INTPTR_MIN		(-2147483647-1)
#  define INTPTR_MAX		(2147483647)
#  define UINTPTR_MAX		(4294967295U)
# endif

/* Minimum for largest signed integral type.  */
# define INTMAX_MIN		(-__INT64_C(9223372036854775807)-1)

/* Maximum for largest signed integral type.  */
# define INTMAX_MAX		(__INT64_C(9223372036854775807))

/* Maximum for largest unsigned integral type.  */
# define UINTMAX_MAX		(__UINT64_C(18446744073709551615))

/* Limits of other integer types.  */

/* Limits of `ptrdiff_t' type.  */
# if __WORDSIZE == 64
#  define PTRDIFF_MIN		(-9223372036854775807L-1)
#  define PTRDIFF_MAX		(9223372036854775807L)
# else
#  define PTRDIFF_MIN		(-2147483647-1)
#  define PTRDIFF_MAX		(2147483647)
# endif

/* Limits of `sig_atomic_t'.  */
# define SIG_ATOMIC_MIN		(-2147483647-1)
# define SIG_ATOMIC_MAX		(2147483647)

/* Limit of `size_t' type.  */

/* Limits of `wchar_t'.  */
# ifndef WCHAR_MIN
/* These constants might also be defined in <wchar.h>.  */
#  define WCHAR_MIN		__WCHAR_MIN
#  define WCHAR_MAX		__WCHAR_MAX
# endif

/* Limits of `wint_t'.  */
# define WINT_MIN		(0u)
# define WINT_MAX		(4294967295u)

#endif	/* C++ && limit macros */

/* The ISO C99 standard specifies that in C++ implementations these
   should only be defined if explicitly requested.  */
#if !defined __cplusplus || defined __STDC_CONSTANT_MACROS

/* Signed.  */
# define INT8_C(c)	c
# define INT16_C(c)	c
# define INT32_C(c)	c
# if __WORDSIZE == 64
#  define INT64_C(c)	c ## L
# else
#  define INT64_C(c)	c ## LL
# endif

/* Unsigned.  */
# define UINT8_C(c)	c ## U
# define UINT16_C(c)	c ## U
# define UINT32_C(c)	c ## U
# if __WORDSIZE == 64
#  define UINT64_C(c)	c ## UL
# else
#  define UINT64_C(c)	c ## ULL
# endif

/* Maximal type.  */
# if __WORDSIZE == 64
#  define INTMAX_C(c)	c ## L
#  define UINTMAX_C(c)	c ## UL
# else
#  define INTMAX_C(c)	c ## LL
#  define UINTMAX_C(c)	c ## ULL
# endif

#endif	/* C++ && constant macros */

#endif /* stdint.h */

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
/*
 * Miscellaneous system-dependent types.
 */
#ifndef	_ASN_SYSTEM_H_
#define	_ASN_SYSTEM_H_

#ifdef	HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>	/* For snprintf(3) */
#include <stdlib.h>	/* For *alloc(3) */
#include <string.h>	/* For memcpy(3) */
#include <sys/types.h>	/* For size_t */
#include <stdarg.h>	/* For va_start */
#include <stddef.h>	/* for offsetof and ptrdiff_t */

#ifdef	WIN32

#include <malloc.h>
//#include <stdint.h>
#define	 snprintf	_snprintf
#define	 vsnprintf	_vsnprintf

#ifdef _MSC_VER			/* MSVS.Net */
#ifndef __cplusplus
#define inline __inline
#endif
#define	ssize_t		SSIZE_T
//typedef	char		int8_t;
typedef	short		int16_t;
typedef	int		int32_t;
typedef	unsigned char	uint8_t;
typedef	unsigned short	uint16_t;
typedef	unsigned int	uint32_t;
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <float.h>
#define isnan _isnan
#define finite _finite
#define copysign _copysign
#define	ilogb	_logb
#endif	/* _MSC_VER */

#else	/* !WIN32 */

#if defined(__vxworks)
#include <types/vxTypes.h>
#else	/* !defined(__vxworks) */

#include <inttypes.h>	/* C99 specifies this file */
/*
 * 1. Earlier FreeBSD version didn't have <stdint.h>,
 * but <inttypes.h> was present.
 * 2. Sun Solaris requires <alloca.h> for alloca(3),
 * but does not have <stdint.h>.
 */
#if	(!defined(__FreeBSD__) || !defined(_SYS_INTTYPES_H_))
#if	defined(sun)
#include <alloca.h>	/* For alloca(3) */
#include <ieeefp.h>	/* for finite(3) */
#elif	defined(__hpux)
#ifdef	__GNUC__
#include <alloca.h>	/* For alloca(3) */
#else	/* !__GNUC__ */
#define inline
#endif	/* __GNUC__ */
#else
#include <stdint.h>	/* SUSv2+ and C99 specify this file, for uintXX_t */
#endif	/* defined(sun) */
#endif

#endif	/* defined(__vxworks) */

#endif	/* WIN32 */

#if	__GNUC__ >= 3
#ifndef	GCC_PRINTFLIKE
#define	GCC_PRINTFLIKE(fmt,var)	__attribute__((format(printf,fmt,var)))
#endif
#else
#ifndef	GCC_PRINTFLIKE
#define	GCC_PRINTFLIKE(fmt,var)	/* nothing */
#endif
#endif

#ifndef	offsetof	/* If not defined by <stddef.h> */
#define	offsetof(s, m)	((ptrdiff_t)&(((s *)0)->m) - (ptrdiff_t)((s *)0))
#endif	/* offsetof */

#ifndef	MIN		/* Suitable for comparing primitive types (integers) */
#if defined(__GNUC__)
#define	MIN(a,b)	({ __typeof a _a = a; __typeof b _b = b;	\
	((_a)<(_b)?(_a):(_b)); })
#else	/* !__GNUC__ */
#define	MIN(a,b)	((a)<(b)?(a):(b))	/* Unsafe variant */
#endif /* __GNUC__ */
#endif	/* MIN */

#endif	/* _ASN_SYSTEM_H_ */

/*-
 * Copyright (c) 2003, 2004, 2005 Lev Walkin <vlm@lionet.info>.
 * All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_ASN_CODECS_H_
#define	_ASN_CODECS_H_

#ifdef __cplusplus
extern "C" {
#endif

struct asn_TYPE_descriptor_s;	/* Forward declaration */

/*
 * This structure defines a set of parameters that may be passed
 * to every ASN.1 encoder or decoder function.
 * WARNING: if max_stack_size member is set, and you are calling the
 *   function pointers of the asn_TYPE_descriptor_t directly,
 *   this structure must be ALLOCATED ON THE STACK!
 *   If you can't always satisfy this requirement, use ber_decode(),
 *   xer_decode() and uper_decode() functions instead.
 */
typedef struct asn_codec_ctx_s {
	/*
	 * Limit the decoder routines to use no (much) more stack than a given
	 * number of bytes. Most of decoders are stack-based, and this
	 * would protect against stack overflows if the number of nested
	 * encodings is high.
	 * The OCTET STRING, BIT STRING and ANY BER decoders are heap-based,
	 * and are safe from this kind of overflow.
	 * A value from getrlimit(RLIMIT_STACK) may be used to initialize
	 * this variable. Be careful in multithreaded environments, as the
	 * stack size is rather limited.
	 */
	size_t  max_stack_size; /* 0 disables stack bounds checking */
} asn_codec_ctx_t;

/*
 * Type of the return value of the encoding functions (der_encode, xer_encode).
 */
typedef struct asn_enc_rval_s {
	/*
	 * Number of bytes encoded.
	 * -1 indicates failure to encode the structure.
	 * In this case, the members below this one are meaningful.
	 */
	ssize_t encoded;

	/*
	 * Members meaningful when (encoded == -1), for post mortem analysis.
	 */

	/* Type which cannot be encoded */
	struct asn_TYPE_descriptor_s *failed_type;

	/* Pointer to the structure of that type */
	void *structure_ptr;
} asn_enc_rval_t;
#define	_ASN_ENCODE_FAILED do {					\
	asn_enc_rval_t tmp_error;				\
	tmp_error.encoded = -1;					\
	tmp_error.failed_type = td;				\
	tmp_error.structure_ptr = sptr;				\
	ASN_DEBUG("Failed to encode element %s", td->name);	\
	return tmp_error;					\
} while(0)
#define	_ASN_ENCODED_OK(rval) do {				\
	rval.structure_ptr = 0;					\
	rval.failed_type = 0;					\
	return rval;						\
} while(0)

/*
 * Type of the return value of the decoding functions (ber_decode, xer_decode)
 *
 * Please note that the number of consumed bytes is ALWAYS meaningful,
 * even if code==RC_FAIL. This is to indicate the number of successfully
 * decoded bytes, hence providing a possibility to fail with more diagnostics
 * (i.e., print the offending remainder of the buffer).
 */
enum asn_dec_rval_code_e {
	RC_OK,		/* Decoded successfully */
	RC_WMORE,	/* More data expected, call again */
	RC_FAIL		/* Failure to decode data */
};
typedef struct asn_dec_rval_s {
	enum asn_dec_rval_code_e code;	/* Result code */
	size_t consumed;		/* Number of bytes consumed */
} asn_dec_rval_t;
#define	_ASN_DECODE_FAILED do {					\
	asn_dec_rval_t tmp_error;				\
	tmp_error.code = RC_FAIL;				\
	tmp_error.consumed = 0;					\
	ASN_DEBUG("Failed to decode element %s", td->name);	\
	return tmp_error;					\
} while(0)
#define	_ASN_DECODE_STARVED do {				\
	asn_dec_rval_t tmp_error;				\
	tmp_error.code = RC_WMORE;				\
	tmp_error.consumed = 0;					\
	return tmp_error;					\
} while(0)

#ifdef __cplusplus
}
#endif

#endif	/* _ASN_CODECS_H_ */

/*-
 * Copyright (c) 2004, 2006 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
/*
 * Application-level ASN.1 callbacks.
 */
#ifndef	_ASN_APPLICATION_H_
#define	_ASN_APPLICATION_H_

//#include "asn_system.h"		/* for platform-dependent types */
//#include "asn_codecs.h"		/* for ASN.1 codecs specifics */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Generic type of an application-defined callback to return various
 * types of data to the application.
 * EXPECTED RETURN VALUES:
 *  -1: Failed to consume bytes. Abort the mission.
 * Non-negative return values indicate success, and ignored.
 */
typedef int (asn_app_consume_bytes_f)(const void *buffer, size_t size,
	void *application_specific_key);

/*
 * A callback of this type is called whenever constraint validation fails
 * on some ASN.1 type. See "constraints.h" for more details on constraint
 * validation.
 * This callback specifies a descriptor of the ASN.1 type which failed
 * the constraint check, as well as human readable message on what
 * particular constraint has failed.
 */
typedef void (asn_app_constraint_failed_f)(void *application_specific_key,
	struct asn_TYPE_descriptor_s *type_descriptor_which_failed,
	const void *structure_which_failed_ptr,
	const char *error_message_format, ...) GCC_PRINTFLIKE(4, 5);

#ifdef __cplusplus
}
#endif

//#include "constr_TYPE.h"	/* for asn_TYPE_descriptor_t */

#endif	/* _ASN_APPLICATION_H_ */

/*-
 * Copyright (c) 2003, 2004, 2005 Lev Walkin <vlm@lionet.info>.
 * All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
/*
 * Declarations internally useful for the ASN.1 support code.
 */
#ifndef	_ASN_INTERNAL_H_
#define	_ASN_INTERNAL_H_

//#include "asn_application.h"	/* Application-visible API */

#ifndef	__NO_ASSERT_H__		/* Include assert.h only for internal use. */
#include <assert.h>		/* for assert() macro */
#endif

#ifdef	__cplusplus
extern "C" {
#endif

/* Environment version might be used to avoid running with the old library */
#define	ASN1C_ENVIRONMENT_VERSION	920	/* Compile-time version */
int get_asn1c_environment_version(void);	/* Run-time version */

#include <vkernel.h>
#define	CALLOC(nmemb, size)	CA_MEM_Calloc(nmemb * size) // calloc(nmemb, size)
#define	MALLOC(size)			CA_MEM_Alloc(size) // malloc(size)
#define	REALLOC(oldptr, size)	CA_MEM_Realloc(oldptr, size) // realloc(oldptr, size)
#define	FREEMEM(ptr)			CA_MEM_Free(ptr) // free(ptr)

/*
 * A macro for debugging the ASN.1 internals.
 * You may enable or override it.
 */

//#define dslee_debug
#ifdef dslee_debug
#undef ASN_DEBUG
#define EMIT_ASN_DEBUG 1
#endif

#ifndef	ASN_DEBUG	/* If debugging code is not defined elsewhere... */
#if	EMIT_ASN_DEBUG == 1	/* And it was asked to emit this code... */
#ifdef	__GNUC__
#define	ASN_DEBUG(fmt, args...)	do {		\
		fprintf(stderr, fmt, ##args);	\
		fprintf(stderr, " (%s:%d)\n",	\
			__FILE__, __LINE__);	\
	} while(0)
#else	/* !__GNUC__ */
void ASN_DEBUG_f(const char *fmt, ...);
#define	ASN_DEBUG	ASN_DEBUG_f
#endif	/* __GNUC__ */
#else	/* EMIT_ASN_DEBUG != 1 */

#ifdef dslee_debug
static inline void ASN_DEBUG(const char *fmt, args...)
{
	VK_Print(fmt, ##args);
	//fprintf(stderr, " (%s:%d)\n");
}
#else
static inline void ASN_DEBUG(const char *fmt, ...) { (void)fmt; }
#endif //
#endif	/* EMIT_ASN_DEBUG */
#endif	/* ASN_DEBUG */

/*
 * Invoke the application-supplied callback and fail, if something is wrong.
 */
#define	__ASN_E_cbc(buf, size)	(cb((buf), (size), app_key) < 0)
#define	_ASN_E_CALLBACK(foo)	do {					\
		if(foo)	goto cb_failed;					\
	} while(0)
#define	_ASN_CALLBACK(buf, size)					\
	_ASN_E_CALLBACK(__ASN_E_cbc(buf, size))
#define	_ASN_CALLBACK2(buf1, size1, buf2, size2)			\
	_ASN_E_CALLBACK(__ASN_E_cbc(buf1, size1) || __ASN_E_cbc(buf2, size2))
#define	_ASN_CALLBACK3(buf1, size1, buf2, size2, buf3, size3)		\
	_ASN_E_CALLBACK(__ASN_E_cbc(buf1, size1)			\
		|| __ASN_E_cbc(buf2, size2)				\
		|| __ASN_E_cbc(buf3, size3))

#define	_i_ASN_TEXT_INDENT(nl, level) do {				\
	int __level = (level);						\
	int __nl = ((nl) != 0);						\
	int __i;							\
	if(__nl) _ASN_CALLBACK("\n", 1);				\
	for(__i = 0; __i < __level; __i++)				\
		_ASN_CALLBACK("    ", 4);				\
	er.encoded += __nl + 4 * __level;				\
} while(0)

#define	_i_INDENT(nl)	do {						\
	int __i;							\
	if((nl) && cb("\n", 1, app_key) < 0) return -1;			\
	for(__i = 0; __i < ilevel; __i++)				\
		if(cb("    ", 4, app_key) < 0) return -1;		\
} while(0)

/*
 * Check stack against overflow, if limit is set.
 */
#define	_ASN_DEFAULT_STACK_MAX	(30000)
static inline int
_ASN_STACK_OVERFLOW_CHECK(asn_codec_ctx_t *ctx) {
	if(ctx && ctx->max_stack_size) {

		/* ctx MUST be allocated on the stack */
		ptrdiff_t usedstack = ((char *)ctx - (char *)&ctx);
		if(usedstack > 0) usedstack = -usedstack; /* grows up! */

		/* double negative required to avoid int wrap-around */
		if(usedstack < -(ptrdiff_t)ctx->max_stack_size) {
			ASN_DEBUG("Stack limit %ld reached",
				(long)ctx->max_stack_size);
			return -1;
		}
	}
	return 0;
}

#ifdef	__cplusplus
}
#endif

#endif	/* _ASN_INTERNAL_H_ */

/*-
 * Copyright (c) 2003 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_BER_TLV_LENGTH_H_
#define	_BER_TLV_LENGTH_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef ssize_t ber_tlv_len_t;

/*
 * This function tries to fetch the length of the BER TLV value and place it
 * in *len_r.
 * RETURN VALUES:
 *	 0:	More data expected than bufptr contains.
 *	-1:	Fatal error deciphering length.
 *	>0:	Number of bytes used from bufptr.
 * On return with >0, len_r is constrained as -1..MAX, where -1 mean
 * that the value is of indefinite length.
 */
ssize_t ber_fetch_length(int _is_constructed, const void *bufptr, size_t size,
	ber_tlv_len_t *len_r);

/*
 * This function expects bufptr to be positioned over L in TLV.
 * It returns number of bytes occupied by L and V together, suitable
 * for skipping. The function properly handles indefinite length.
 * RETURN VALUES:
 * 	Standard {-1,0,>0} convention.
 */
ssize_t ber_skip_length(
	struct asn_codec_ctx_s *opt_codec_ctx,	/* optional context */
	int _is_constructed, const void *bufptr, size_t size);

/*
 * This function serializes the length (L from TLV) in DER format.
 * It always returns number of bytes necessary to represent the length,
 * it is a caller's responsibility to check the return value
 * against the supplied buffer's size.
 */
size_t der_tlv_length_serialize(ber_tlv_len_t len, void *bufptr, size_t size);

#ifdef __cplusplus
}
#endif

#endif	/* _BER_TLV_LENGTH_H_ */

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_BER_TLV_TAG_H_
#define	_BER_TLV_TAG_H_

#ifdef __cplusplus
extern "C" {
#endif

enum asn_tag_class {
	ASN_TAG_CLASS_UNIVERSAL		= 0,	/* 0b00 */
	ASN_TAG_CLASS_APPLICATION	= 1,	/* 0b01 */
	ASN_TAG_CLASS_CONTEXT		= 2,	/* 0b10 */
	ASN_TAG_CLASS_PRIVATE		= 3	/* 0b11 */
};
typedef unsigned ber_tlv_tag_t;	/* BER TAG from Tag-Length-Value */

/*
 * Tag class is encoded together with tag value for optimization purposes.
 */
#define	BER_TAG_CLASS(tag)	((tag) & 0x3)
#define	BER_TAG_VALUE(tag)	((tag) >> 2)
#define	BER_TLV_CONSTRUCTED(tagptr)	(((*(const uint8_t *)tagptr)&0x20)?1:0)

#define	BER_TAGS_EQUAL(tag1, tag2)	((tag1) == (tag2))

/*
 * Several functions for printing the TAG in the canonical form
 * (i.e. "[PRIVATE 0]").
 * Return values correspond to their libc counterparts (if any).
 */
ssize_t ber_tlv_tag_snprint(ber_tlv_tag_t tag, char *buf, size_t buflen);
ssize_t ber_tlv_tag_fwrite(ber_tlv_tag_t tag, FILE *);
char *ber_tlv_tag_string(ber_tlv_tag_t tag);

/*
 * This function tries to fetch the tag from the input stream.
 * RETURN VALUES:
 * 	 0:	More data expected than bufptr contains.
 * 	-1:	Fatal error deciphering tag.
 *	>0:	Number of bytes used from bufptr. tag_r will contain the tag.
 */
ssize_t ber_fetch_tag(const void *bufptr, size_t size, ber_tlv_tag_t *tag_r);

/*
 * This function serializes the tag (T from TLV) in BER format.
 * It always returns number of bytes necessary to represent the tag,
 * it is a caller's responsibility to check the return value
 * against the supplied buffer's size.
 */
size_t ber_tlv_tag_serialize(ber_tlv_tag_t tag, void *bufptr, size_t size);

#ifdef __cplusplus
}
#endif

#endif	/* _BER_TLV_TAG_H_ */

// dslee - for build
struct asn_struct_ctx_t;	/* Forward declaration */
typedef struct asn_struct_ctx_s {
	short phase;		/* Decoding phase */
	short step;		/* Elementary step of a phase */
	int context;		/* Other context information */
	void *ptr;		/* Decoder-specific stuff (stack elements) */
	ber_tlv_len_t left;	/* Number of bytes left, -1 for indefinite */
} asn_struct_ctx_t;

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_BER_DECODER_H_
#define	_BER_DECODER_H_

//#include <asn_application.h>

#ifdef __cplusplus
extern "C" {
#endif

struct asn_TYPE_descriptor_s;	/* Forward declaration */
struct asn_codec_ctx_s;		/* Forward declaration */

/*
 * The BER decoder of any type.
 * This function may be invoked directly from the application.
 */
asn_dec_rval_t ber_decode(struct asn_codec_ctx_s *opt_codec_ctx,
	struct asn_TYPE_descriptor_s *type_descriptor,
	void **struct_ptr,	/* Pointer to a target structure's pointer */
	const void *buffer,	/* Data to be decoded */
	size_t size		/* Size of that buffer */
	);

/*
 * Type of generic function which decodes the byte stream into the structure.
 */
typedef asn_dec_rval_t (ber_type_decoder_f)(
		struct asn_codec_ctx_s *opt_codec_ctx,
		struct asn_TYPE_descriptor_s *type_descriptor,
		void **struct_ptr, const void *buf_ptr, size_t size,
		int tag_mode);

/*******************************
 * INTERNALLY USEFUL FUNCTIONS *
 *******************************/

/*
 * Check that all tags correspond to the type definition (as given in head).
 * On return, last_length would contain either a non-negative length of the
 * value part of the last TLV, or the negative number of expected
 * "end of content" sequences. The number may only be negative if the
 * head->last_tag_form is non-zero.
 */
asn_dec_rval_t ber_check_tags(
		struct asn_codec_ctx_s *opt_codec_ctx,	/* codec options */
		struct asn_TYPE_descriptor_s *type_descriptor,
		asn_struct_ctx_t *opt_ctx,	/* saved decoding context */
		const void *ptr, size_t size,
		int tag_mode,		/* {-1,0,1}: IMPLICIT, no, EXPLICIT */
		int last_tag_form,	/* {-1,0:1}: any, primitive, constr */
		ber_tlv_len_t *last_length,
		int *opt_tlv_form	/* optional tag form */
	);

#ifdef __cplusplus
}
#endif

#endif	/* _BER_DECODER_H_ */

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_DER_ENCODER_H_
#define	_DER_ENCODER_H_

//#include <asn_application.h>

#ifdef __cplusplus
extern "C" {
#endif

struct asn_TYPE_descriptor_s;	/* Forward declaration */

/*
 * The DER encoder of any type. May be invoked by the application.
 */
asn_enc_rval_t der_encode(struct asn_TYPE_descriptor_s *type_descriptor,
		void *struct_ptr,	/* Structure to be encoded */
		asn_app_consume_bytes_f *consume_bytes_cb,
		void *app_key		/* Arbitrary callback argument */
	);

/* A variant of der_encode() which encodes data into the pre-allocated buffer */
asn_enc_rval_t der_encode_to_buffer(
		struct asn_TYPE_descriptor_s *type_descriptor,
		void *struct_ptr,	/* Structure to be encoded */
		void *buffer,		/* Pre-allocated buffer */
		size_t buffer_size	/* Initial buffer size (maximum) */
	);

/*
 * Type of the generic DER encoder.
 */
typedef asn_enc_rval_t (der_type_encoder_f)(
		struct asn_TYPE_descriptor_s *type_descriptor,
		void *struct_ptr,	/* Structure to be encoded */
		int tag_mode,		/* {-1,0,1}: IMPLICIT, no, EXPLICIT */
		ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *consume_bytes_cb,	/* Callback */
		void *app_key		/* Arbitrary callback argument */
	);

/*******************************
 * INTERNALLY USEFUL FUNCTIONS *
 *******************************/

/*
 * Write out leading TL[v] sequence according to the type definition.
 */
ssize_t der_write_tags(
		struct asn_TYPE_descriptor_s *type_descriptor,
		size_t struct_length,
		int tag_mode,		/* {-1,0,1}: IMPLICIT, no, EXPLICIT */
		int last_tag_form,	/* {0,!0}: prim, constructed */
		ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *consume_bytes_cb,
		void *app_key
	);

#ifdef __cplusplus
}
#endif

#endif	/* _DER_ENCODER_H_ */

/*-
 * Copyright (c) 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_XER_DECODER_H_
#define	_XER_DECODER_H_

//#include <asn_application.h>

#ifdef __cplusplus
extern "C" {
#endif

struct asn_TYPE_descriptor_s;	/* Forward declaration */

/*
 * The XER decoder of any ASN.1 type. May be invoked by the application.
 */
asn_dec_rval_t xer_decode(struct asn_codec_ctx_s *opt_codec_ctx,
	struct asn_TYPE_descriptor_s *type_descriptor,
	void **struct_ptr,	/* Pointer to a target structure's pointer */
	const void *buffer,	/* Data to be decoded */
	size_t size		/* Size of data buffer */
	);

/*
 * Type of the type-specific XER decoder function.
 */
typedef asn_dec_rval_t (xer_type_decoder_f)(asn_codec_ctx_t *opt_codec_ctx,
		struct asn_TYPE_descriptor_s *type_descriptor,
		void **struct_ptr,
		const char *opt_mname,	/* Member name */
		const void *buf_ptr, size_t size
	);

/*******************************
 * INTERNALLY USEFUL FUNCTIONS *
 *******************************/

/*
 * Generalized function for decoding the primitive values.
 * Used by more specialized functions, such as OCTET_STRING_decode_xer_utf8
 * and others. This function should not be used by applications, as its API
 * is subject to changes.
 */
asn_dec_rval_t xer_decode_general(asn_codec_ctx_t *opt_codec_ctx,
	asn_struct_ctx_t *ctx,	/* Type decoder context */
	void *struct_key,	/* Treated as opaque pointer */
	const char *xml_tag,	/* Expected XML tag name */
	const void *buf_ptr, size_t size,
	int (*opt_unexpected_tag_decoder)
		(void *struct_key, const void *chunk_buf, size_t chunk_size),
	ssize_t (*body_receiver)
		(void *struct_key, const void *chunk_buf, size_t chunk_size,
			int have_more)
	);

/*
 * Fetch the next XER (XML) token from the stream.
 * The function returns the number of bytes occupied by the chunk type,
 * returned in the _ch_type. The _ch_type is only set (and valid) when
 * the return value is greater than 0.
 */
  typedef enum pxer_chunk_type {
	PXER_TAG,	/* Complete XER tag */
	PXER_TEXT,	/* Plain text between XER tags */
	PXER_COMMENT	/* A comment, may be part of */
  } pxer_chunk_type_e;
ssize_t xer_next_token(int *stateContext,
	const void *buffer, size_t size, pxer_chunk_type_e *_ch_type);

/*
 * This function checks the buffer against the tag name is expected to occur.
 */
  typedef enum xer_check_tag {
	XCT_BROKEN	= 0,	/* The tag is broken */
	XCT_OPENING	= 1,	/* This is the <opening> tag */
	XCT_CLOSING	= 2,	/* This is the </closing> tag */
	XCT_BOTH	= 3,	/* This is the <modified/> tag */
	XCT__UNK__MASK	= 4,	/* Mask of everything unexpected */
	XCT_UNKNOWN_OP	= 5,	/* Unexpected <opening> tag */
	XCT_UNKNOWN_CL	= 6,	/* Unexpected </closing> tag */
	XCT_UNKNOWN_BO	= 7	/* Unexpected <modified/> tag */
  } xer_check_tag_e;
xer_check_tag_e xer_check_tag(const void *buf_ptr, int size,
		const char *need_tag);

/*
 * Check whether this buffer consists of entirely XER whitespace characters.
 * RETURN VALUES:
 * 1:	Whitespace or empty string
 * 0:	Non-whitespace
 */
int xer_is_whitespace(const void *chunk_buf, size_t chunk_size);

/*
 * Skip the series of anticipated extensions.
 */
int xer_skip_unknown(xer_check_tag_e tcv, ber_tlv_len_t *depth);

#ifdef __cplusplus
}
#endif

#endif	/* _XER_DECODER_H_ */

/*-
 * Copyright (c) 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_XER_ENCODER_H_
#define	_XER_ENCODER_H_

//#include <asn_application.h>

#ifdef __cplusplus
extern "C" {
#endif

struct asn_TYPE_descriptor_s;	/* Forward declaration */

/* Flags used by the xer_encode() and (*xer_type_encoder_f), defined below */
enum xer_encoder_flags_e {
	/* Mode of encoding */
	XER_F_BASIC	= 0x01,	/* BASIC-XER (pretty-printing) */
	XER_F_CANONICAL	= 0x02	/* Canonical XER (strict rules) */
};

/*
 * The XER encoder of any type. May be invoked by the application.
 */
asn_enc_rval_t xer_encode(struct asn_TYPE_descriptor_s *type_descriptor,
		void *struct_ptr,	/* Structure to be encoded */
		enum xer_encoder_flags_e xer_flags,
		asn_app_consume_bytes_f *consume_bytes_cb,
		void *app_key		/* Arbitrary callback argument */
	);

/*
 * The variant of the above function which dumps the BASIC-XER (XER_F_BASIC)
 * output into the chosen file pointer.
 * RETURN VALUES:
 * 	 0: The structure is printed.
 * 	-1: Problem printing the structure.
 * WARNING: No sensible errno value is returned.
 */
int xer_fprint(FILE *stream, struct asn_TYPE_descriptor_s *td, void *sptr);

/*
 * Type of the generic XER encoder.
 */
typedef asn_enc_rval_t (xer_type_encoder_f)(
		struct asn_TYPE_descriptor_s *type_descriptor,
		void *struct_ptr,	/* Structure to be encoded */
		int ilevel,		/* Level of indentation */
		enum xer_encoder_flags_e xer_flags,
		asn_app_consume_bytes_f *consume_bytes_cb,	/* Callback */
		void *app_key		/* Arbitrary callback argument */
	);

#ifdef __cplusplus
}
#endif

#endif	/* _XER_ENCODER_H_ */

/*
 * Copyright (c) 2005, 2006 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_PER_SUPPORT_H_
#define	_PER_SUPPORT_H_

//#include <asn_system.h>		/* Platform-specific types */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Pre-computed PER constraints.
 */
typedef struct asn_per_constraint_s {
	enum asn_per_constraint_flags {
		APC_UNCONSTRAINED	= 0x0,	/* No PER visible constraints */
		APC_SEMI_CONSTRAINED	= 0x1,	/* Constrained at "lb" */
		APC_CONSTRAINED		= 0x2,	/* Fully constrained */
		APC_EXTENSIBLE		= 0x4	/* May have extension */
	} flags;
	int  range_bits;		/* Full number of bits in the range */
	int  effective_bits;		/* Effective bits */
	long lower_bound;		/* "lb" value */
	long upper_bound;		/* "ub" value */
} asn_per_constraint_t;
typedef struct asn_per_constraints_s {
	asn_per_constraint_t value;
	asn_per_constraint_t size;
} asn_per_constraints_t;

/*
 * This structure describes a position inside an incoming PER bit stream.
 */
typedef struct asn_per_data_s {
 const uint8_t *buffer;	/* Pointer to the octet stream */
        size_t  nboff;	/* Bit offset to the meaningful bit */
        size_t  nbits;	/* Number of bits in the stream */
} asn_per_data_t;

/*
 * Extract a small number of bits (<= 31) from the specified PER data pointer.
 * This function returns -1 if the specified number of bits could not be
 * extracted due to EOD or other conditions.
 */
int32_t per_get_few_bits(asn_per_data_t *per_data, int get_nbits);

/*
 * Extract a large number of bits from the specified PER data pointer.
 * This function returns -1 if the specified number of bits could not be
 * extracted due to EOD or other conditions.
 */
int per_get_many_bits(asn_per_data_t *pd, uint8_t *dst, int right_align,
			int get_nbits);

/*
 * Get the length "n" from the Unaligned PER stream.
 */
ssize_t uper_get_length(asn_per_data_t *pd,
			int effective_bound_bits,
			int *repeat);

/*
 * Get the normally small non-negative whole number.
 */
ssize_t uper_get_nsnnwn(asn_per_data_t *pd);

/*
 * This structure supports forming PER output.
 */
typedef struct asn_per_outp_s {
	uint8_t *buffer;	/* Pointer into the (tmpspace) */
	size_t nboff;		/* Bit offset to the meaningful bit */
	size_t nbits;		/* Number of bits left in (tmpspace) */
	uint8_t tmpspace[32];	/* Preliminary storage to hold data */
	int (*outper)(const void *data, size_t size, void *op_key);
	void *op_key;		/* Key for (outper) data callback */
	size_t flushed_bytes;	/* Bytes already flushed through (outper) */
} asn_per_outp_t;

/* Output a small number of bits (<= 31) */
int per_put_few_bits(asn_per_outp_t *per_data, uint32_t bits, int obits);

/* Output a large number of bits */
int per_put_many_bits(asn_per_outp_t *po, const uint8_t *src, int put_nbits);

/*
 * Put the length "n" to the Unaligned PER stream.
 * This function returns the number of units which may be flushed
 * in the next units saving iteration.
 */
ssize_t uper_put_length(asn_per_outp_t *po, size_t whole_length);

/*
 * Put the normally small non-negative whole number.
 */
int uper_put_nsnnwn(asn_per_outp_t *po, int n);

#ifdef __cplusplus
}
#endif

#endif	/* _PER_SUPPORT_H_ */

/*-
 * Copyright (c) 2005 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_PER_DECODER_H_
#define	_PER_DECODER_H_

//#include <asn_application.h>
//#include <per_support.h>

#ifdef __cplusplus
extern "C" {
#endif

struct asn_TYPE_descriptor_s;	/* Forward declaration */

/*
 * Unaligned PER decoder of any ASN.1 type. May be invoked by the application.
 */
asn_dec_rval_t uper_decode(struct asn_codec_ctx_s *opt_codec_ctx,
	struct asn_TYPE_descriptor_s *type_descriptor,	/* Type to decode */
	void **struct_ptr,	/* Pointer to a target structure's pointer */
	const void *buffer,	/* Data to be decoded */
	size_t size,		/* Size of data buffer */
	int skip_bits,		/* Number of unused leading bits, 0..7 */
	int unused_bits		/* Number of unused tailing bits, 0..7 */
	);

/*
 * Type of the type-specific PER decoder function.
 */
typedef asn_dec_rval_t (per_type_decoder_f)(asn_codec_ctx_t *opt_codec_ctx,
		struct asn_TYPE_descriptor_s *type_descriptor,
		asn_per_constraints_t *constraints,
		void **struct_ptr,
		asn_per_data_t *per_data
	);

#ifdef __cplusplus
}
#endif

#endif	/* _PER_DECODER_H_ */

/*-
 * Copyright (c) 2006 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_PER_ENCODER_H_
#define	_PER_ENCODER_H_

//#include <asn_application.h>
//#include <per_support.h>

#ifdef __cplusplus
extern "C" {
#endif

struct asn_TYPE_descriptor_s;	/* Forward declaration */

/*
 * Unaligned PER encoder of any ASN.1 type. May be invoked by the application.
 */
asn_enc_rval_t uper_encode(struct asn_TYPE_descriptor_s *type_descriptor,
	void *struct_ptr,	/* Structure to be encoded */
	asn_app_consume_bytes_f *consume_bytes_cb,	/* Data collector */
	void *app_key		/* Arbitrary callback argument */
);

/* A variant of uper_encode() which encodes data into the existing buffer */
asn_enc_rval_t uper_encode_to_buffer(
	struct asn_TYPE_descriptor_s *type_descriptor,
	void *struct_ptr,	/* Structure to be encoded */
	void *buffer,		/* Pre-allocated buffer */
	size_t buffer_size	/* Initial buffer size (max) */
);

/*
 * Type of the generic PER encoder function.
 */
typedef asn_enc_rval_t (per_type_encoder_f)(
	struct asn_TYPE_descriptor_s *type_descriptor,
	asn_per_constraints_t *constraints,
	void *struct_ptr,
	asn_per_outp_t *per_output
);

#ifdef __cplusplus
}
#endif

#endif	/* _PER_ENCODER_H_ */

/*-
 * Copyright (c) 2004, 2006 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_ASN1_CONSTRAINTS_VALIDATOR_H_
#define	_ASN1_CONSTRAINTS_VALIDATOR_H_

//#include <asn_system.h>		/* Platform-dependent types */

#ifdef __cplusplus
extern "C" {
#endif

struct asn_TYPE_descriptor_s;		/* Forward declaration */

/*
 * Validate the structure according to the ASN.1 constraints.
 * If errbuf and errlen are given, they shall be pointing to the appropriate
 * buffer space and its length before calling this function. Alternatively,
 * they could be passed as NULL's. If constraints validation fails,
 * errlen will contain the actual number of bytes taken from the errbuf
 * to encode an error message (properly 0-terminated).
 *
 * RETURN VALUES:
 * This function returns 0 in case all ASN.1 constraints are met
 * and -1 if one or more constraints were failed.
 */
int
asn_check_constraints(struct asn_TYPE_descriptor_s *type_descriptor,
	const void *struct_ptr,	/* Target language's structure */
	char *errbuf,		/* Returned error description */
	size_t *errlen		/* Length of the error description */
	);

/*
 * Generic type for constraint checking callback,
 * associated with every type descriptor.
 */
typedef int (asn_constr_check_f)(
	struct asn_TYPE_descriptor_s *type_descriptor,
	const void *struct_ptr,
	asn_app_constraint_failed_f *optional_callback,	/* Log the error */
	void *optional_app_key		/* Opaque key passed to a callback */
	);

/*******************************
 * INTERNALLY USEFUL FUNCTIONS *
 *******************************/

asn_constr_check_f asn_generic_no_constraint;	/* No constraint whatsoever */
asn_constr_check_f asn_generic_unknown_constraint; /* Not fully supported */

/*
 * Invoke the callback with a complete error message.
 */
#define	_ASN_CTFAIL	if(ctfailcb) ctfailcb

#ifdef __cplusplus
}
#endif

#endif	/* _ASN1_CONSTRAINTS_VALIDATOR_H_ */

/*-
 * Copyright (c) 2003, 2004, 2005, 2006 Lev Walkin <vlm@lionet.info>.
 * All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
/*
 * This file contains the declaration structure called "ASN.1 Type Definition",
 * which holds all information necessary for encoding and decoding routines.
 * This structure even contains pointer to these encoding and decoding routines
 * for each defined ASN.1 type.
 */
#ifndef	_CONSTR_TYPE_H_
#define	_CONSTR_TYPE_H_

//#include <ber_tlv_length.h>
//#include <ber_tlv_tag.h>

#ifdef __cplusplus
extern "C" {
#endif

struct asn_TYPE_descriptor_s;	/* Forward declaration */
struct asn_TYPE_member_s;	/* Forward declaration */

/*
 * This type provides the context information for various ASN.1 routines,
 * primarily ones doing decoding. A member _asn_ctx of this type must be
 * included into certain target language's structures, such as compound types.
 */
//typedef struct asn_struct_ctx_s {
//	short phase;		/* Decoding phase */
//	short step;		/* Elementary step of a phase */
//	int context;		/* Other context information */
//	void *ptr;		/* Decoder-specific stuff (stack elements) */
//	ber_tlv_len_t left;	/* Number of bytes left, -1 for indefinite */
//} asn_struct_ctx_t;

//#include <ber_decoder.h>	/* Basic Encoding Rules decoder */
//#include <der_encoder.h>	/* Distinguished Encoding Rules encoder */
//#include <xer_decoder.h>	/* Decoder of XER (XML, text) */
//#include <xer_encoder.h>	/* Encoder into XER (XML, text) */
//#include <per_decoder.h>	/* Packet Encoding Rules decoder */
//#include <per_encoder.h>	/* Packet Encoding Rules encoder */
//#include <constraints.h>	/* Subtype constraints support */

/*
 * Free the structure according to its specification.
 * If (free_contents_only) is set, the wrapper structure itself (struct_ptr)
 * will not be freed. (It may be useful in case the structure is allocated
 * statically or arranged on the stack, yet its elements are allocated
 * dynamically.)
 */
typedef void (asn_struct_free_f)(
		struct asn_TYPE_descriptor_s *type_descriptor,
		void *struct_ptr, int free_contents_only);
#define	ASN_STRUCT_FREE(asn_DEF, ptr)	(asn_DEF).free_struct(&(asn_DEF),ptr,0)
#define	ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF, ptr)	\
					(asn_DEF).free_struct(&(asn_DEF),ptr,1)

/*
 * Print the structure according to its specification.
 */
typedef int (asn_struct_print_f)(
		struct asn_TYPE_descriptor_s *type_descriptor,
		const void *struct_ptr,
		int level,	/* Indentation level */
		asn_app_consume_bytes_f *callback, void *app_key);

/*
 * Return the outmost tag of the type.
 * If the type is untagged CHOICE, the dynamic operation is performed.
 * NOTE: This function pointer type is only useful internally.
 * Do not use it in your application.
 */
typedef ber_tlv_tag_t (asn_outmost_tag_f)(
		struct asn_TYPE_descriptor_s *type_descriptor,
		const void *struct_ptr, int tag_mode, ber_tlv_tag_t tag);
/* The instance of the above function type; used internally. */
asn_outmost_tag_f asn_TYPE_outmost_tag;

/*
 * The definitive description of the destination language's structure.
 */
typedef struct asn_TYPE_descriptor_s {
	char *name;	/* A name of the ASN.1 type. "" in some cases. */
	char *xml_tag;	/* Name used in XML tag */

	/*
	 * Generalized functions for dealing with the specific type.
	 * May be directly invoked by applications.
	 */
	asn_struct_free_f  *free_struct;	/* Free the structure */
	asn_struct_print_f *print_struct;	/* Human readable output */
	asn_constr_check_f *check_constraints;	/* Constraints validator */
	ber_type_decoder_f *ber_decoder;	/* Generic BER decoder */
	der_type_encoder_f *der_encoder;	/* Canonical DER encoder */
	xer_type_decoder_f *xer_decoder;	/* Generic XER decoder */
	xer_type_encoder_f *xer_encoder;	/* [Canonical] XER encoder */
	per_type_decoder_f *uper_decoder;	/* Unaligned PER decoder */
	per_type_encoder_f *uper_encoder;	/* Unaligned PER encoder */

	/***********************************************************************
	 * Internally useful members. Not to be used by applications directly. *
	 **********************************************************************/

	/*
	 * Tags that are expected to occur.
	 */
	asn_outmost_tag_f  *outmost_tag;	/* <optional, internal> */
	ber_tlv_tag_t *tags;	/* Effective tags sequence for this type */
	int tags_count;		/* Number of tags which are expected */
	ber_tlv_tag_t *all_tags;/* Every tag for BER/containment */
	int all_tags_count;	/* Number of tags */

	asn_per_constraints_t *per_constraints;	/* PER compiled constraints */

	/*
	 * An ASN.1 production type members (members of SEQUENCE, SET, CHOICE).
	 */
	struct asn_TYPE_member_s *elements;
	int elements_count;

	/*
	 * Additional information describing the type, used by appropriate
	 * functions above.
	 */
	void *specifics;
} asn_TYPE_descriptor_t;

/*
 * This type describes an element of the constructed type,
 * i.e. SEQUENCE, SET, CHOICE, etc.
 */
  enum asn_TYPE_flags_e {
	ATF_NOFLAGS,
	ATF_POINTER	= 0x01,	/* Represented by the pointer */
	ATF_OPEN_TYPE	= 0x02	/* ANY type, without meaningful tag */
  };
typedef struct asn_TYPE_member_s {
	enum asn_TYPE_flags_e flags;	/* Element's presentation flags */
	int optional;	/* Following optional members, including current */
	int memb_offset;		/* Offset of the element */
	ber_tlv_tag_t tag;		/* Outmost (most immediate) tag */
	int tag_mode;		/* IMPLICIT/no/EXPLICIT tag at current level */
	asn_TYPE_descriptor_t *type;	/* Member type descriptor */
	asn_constr_check_f *memb_constraints;	/* Constraints validator */
	asn_per_constraints_t *per_constraints;	/* PER compiled constraints */
	int (*default_value)(int setval, void **sptr);	/* DEFAULT <value> */
	char *name;			/* ASN.1 identifier of the element */
} asn_TYPE_member_t;

/*
 * BER tag to element number mapping.
 */
typedef struct asn_TYPE_tag2member_s {
	ber_tlv_tag_t el_tag;	/* Outmost tag of the member */
	int el_no;		/* Index of the associated member, base 0 */
	int toff_first;		/* First occurence of the el_tag, relative */
	int toff_last;		/* Last occurence of the el_tag, relatvie */
} asn_TYPE_tag2member_t;

/*
 * This function is a wrapper around (td)->print_struct, which prints out
 * the contents of the target language's structure (struct_ptr) into the
 * file pointer (stream) in human readable form.
 * RETURN VALUES:
 * 	 0: The structure is printed.
 * 	-1: Problem dumping the structure.
 * (See also xer_fprint() in xer_encoder.h)
 */
int asn_fprint(FILE *stream,		/* Destination stream descriptor */
	asn_TYPE_descriptor_t *td,	/* ASN.1 type descriptor */
	const void *struct_ptr);	/* Structure to be printed */

#ifdef __cplusplus
}
#endif

#endif	/* _CONSTR_TYPE_H_ */

/*-
 * Copyright (c) 2003 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_OCTET_STRING_H_
#define	_OCTET_STRING_H_

//#include <asn_application.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OCTET_STRING {
	uint8_t *buf;	/* Buffer with consecutive OCTET_STRING bits */
	int size;	/* Size of the buffer */

	asn_struct_ctx_t _asn_ctx;	/* Parsing across buffer boundaries */
} OCTET_STRING_t;

extern asn_TYPE_descriptor_t asn_DEF_OCTET_STRING;

asn_struct_free_f OCTET_STRING_free;
asn_struct_print_f OCTET_STRING_print;
asn_struct_print_f OCTET_STRING_print_utf8;
ber_type_decoder_f OCTET_STRING_decode_ber;
der_type_encoder_f OCTET_STRING_encode_der;
xer_type_decoder_f OCTET_STRING_decode_xer_hex;		/* Hexadecimal */
xer_type_decoder_f OCTET_STRING_decode_xer_binary;	/* 01010111010 */
xer_type_decoder_f OCTET_STRING_decode_xer_utf8;	/* ASCII/UTF-8 */
xer_type_encoder_f OCTET_STRING_encode_xer;
xer_type_encoder_f OCTET_STRING_encode_xer_utf8;
per_type_decoder_f OCTET_STRING_decode_uper;
per_type_encoder_f OCTET_STRING_encode_uper;

/******************************
 * Handy conversion routines. *
 ******************************/

/*
 * This function clears the previous value of the OCTET STRING (if any)
 * and then allocates a new memory with the specified content (str/size).
 * If size = -1, the size of the original string will be determined
 * using strlen(str).
 * If str equals to NULL, the function will silently clear the
 * current contents of the OCTET STRING.
 * Returns 0 if it was possible to perform operation, -1 otherwise.
 */
int OCTET_STRING_fromBuf(OCTET_STRING_t *s, const char *str, int size);

/* Handy conversion from the C string into the OCTET STRING. */
#define	OCTET_STRING_fromString(s, str)	OCTET_STRING_fromBuf(s, str, -1)

/*
 * Allocate and fill the new OCTET STRING and return a pointer to the newly
 * allocated object. NULL is permitted in str: the function will just allocate
 * empty OCTET STRING.
 */
OCTET_STRING_t *OCTET_STRING_new_fromBuf(asn_TYPE_descriptor_t *td,
	const char *str, int size);

/****************************
 * Internally useful stuff. *
 ****************************/

typedef struct asn_OCTET_STRING_specifics_s {
	/*
	 * Target structure description.
	 */
	int struct_size;	/* Size of the structure */
	int ctx_offset;		/* Offset of the asn_struct_ctx_t member */

	int subvariant;		/* {0,1,2} for O-S, BIT STRING or ANY */
} asn_OCTET_STRING_specifics_t;

#ifdef __cplusplus
}
#endif

#endif	/* _OCTET_STRING_H_ */

/*-
 * Copyright (c) 2003 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_BIT_STRING_H_
#define	_BIT_STRING_H_

//#include <OCTET_STRING.h>	/* Some help from OCTET STRING */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BIT_STRING_s {
	uint8_t *buf;	/* BIT STRING body */
	int size;	/* Size of the above buffer */

	int bits_unused;/* Unused trailing bits in the last octet (0..7) */

	asn_struct_ctx_t _asn_ctx;	/* Parsing across buffer boundaries */
} BIT_STRING_t;

extern asn_TYPE_descriptor_t asn_DEF_BIT_STRING;

asn_struct_print_f BIT_STRING_print;	/* Human-readable output */
asn_constr_check_f BIT_STRING_constraint;
xer_type_encoder_f BIT_STRING_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _BIT_STRING_H_ */

/*-
 * Copyright (c) 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	ASN_CODECS_PRIM_H
#define	ASN_CODECS_PRIM_H

//#include <asn_application.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ASN__PRIMITIVE_TYPE_s {
	uint8_t *buf;	/* Buffer with consecutive primitive encoding bytes */
	int size;	/* Size of the buffer */
} ASN__PRIMITIVE_TYPE_t;	/* Do not use this type directly! */

asn_struct_free_f ASN__PRIMITIVE_TYPE_free;
ber_type_decoder_f ber_decode_primitive;
der_type_encoder_f der_encode_primitive;

/*
 * A callback specification for the xer_decode_primitive() function below.
 */
enum xer_pbd_rval {
	XPBD_SYSTEM_FAILURE,	/* System failure (memory shortage, etc) */
	XPBD_DECODER_LIMIT,	/* Hit some decoder limitation or deficiency */
	XPBD_BROKEN_ENCODING,	/* Encoding of a primitive body is broken */
	XPBD_NOT_BODY_IGNORE,	/* Not a body format, but safe to ignore */
	XPBD_BODY_CONSUMED	/* Body is recognized and consumed */
};
typedef enum xer_pbd_rval (xer_primitive_body_decoder_f)
	(asn_TYPE_descriptor_t *td, void *struct_ptr,
		const void *chunk_buf, size_t chunk_size);

/*
 * Specific function to decode simple primitive types.
 * Also see xer_decode_general() in xer_decoder.h
 */
asn_dec_rval_t xer_decode_primitive(asn_codec_ctx_t *opt_codec_ctx,
	asn_TYPE_descriptor_t *type_descriptor,
	void **struct_ptr, size_t struct_size,
	const char *opt_mname,
	const void *buf_ptr, size_t size,
	xer_primitive_body_decoder_f *prim_body_decoder
);

#ifdef __cplusplus
}
#endif

#endif	/* ASN_CODECS_PRIM_H */

/*
 * Copyright (c) 2003, 2004 X/IO Labs, xiolabs.com.
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_XER_SUPPORT_H_
#define	_XER_SUPPORT_H_

//#include <asn_system.h>		/* Platform-specific types */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Types of data transferred to the application.
 */
typedef enum {
	PXML_TEXT,	/* Plain text between XML tags. */
	PXML_TAG,	/* A tag, starting with '<'. */
	PXML_COMMENT,	/* An XML comment, including "<!--" and "-->". */
	/*
	 * The following chunk types are reported if the chunk
	 * terminates the specified XML element.
	 */
	PXML_TAG_END,		/* Tag ended */
	PXML_COMMENT_END	/* Comment ended */
} pxml_chunk_type_e;

/*
 * Callback function that is called by the parser when parsed data is
 * available. The _opaque is the pointer to a field containing opaque user
 * data specified in pxml_create() call. The chunk type is _type and the text
 * data is the piece of buffer identified by _bufid (as supplied to
 * pxml_feed() call) starting at offset _offset and of _size bytes size.
 * The chunk is NOT '\0'-terminated.
 */
typedef int (pxml_callback_f)(pxml_chunk_type_e _type,
	const void *_chunk_data, size_t _chunk_size, void *_key);

/*
 * Parse the given buffer as it were a chunk of XML data.
 * Invoke the specified callback each time the meaninful data is found.
 * This function returns number of bytes consumed from the bufer.
 * It will always be lesser than or equal to the specified _size.
 * The next invocation of this function must account the difference.
 */
ssize_t pxml_parse(int *_stateContext, const void *_buf, size_t _size,
	pxml_callback_f *cb, void *_key);

#ifdef __cplusplus
}
#endif

#endif	/* _XER_SUPPORT_H_ */

/*-
 * Copyright (c) 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef ASN_TYPE_ANY_H
#define ASN_TYPE_ANY_H

//#include <OCTET_STRING.h>	/* Implemented via OCTET STRING type */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ANY {
	uint8_t *buf;	/* BER-encoded ANY contents */
	int size;	/* Size of the above buffer */

	asn_struct_ctx_t _asn_ctx;	/* Parsing across buffer boundaries */
} ANY_t;

extern asn_TYPE_descriptor_t asn_DEF_ANY;

asn_struct_free_f ANY_free;
asn_struct_print_f ANY_print;
ber_type_decoder_f ANY_decode_ber;
der_type_encoder_f ANY_encode_der;
xer_type_encoder_f ANY_encode_xer;

/******************************
 * Handy conversion routines. *
 ******************************/

/* Convert another ASN.1 type into the ANY. This implies DER encoding. */
int ANY_fromType(ANY_t *, asn_TYPE_descriptor_t *td, void *struct_ptr);
ANY_t *ANY_new_fromType(asn_TYPE_descriptor_t *td, void *struct_ptr);

/* Convert the contents of the ANY type into the specified type. */
int ANY_to_type(ANY_t *, asn_TYPE_descriptor_t *td, void **struct_ptr);

#define	ANY_fromBuf(s, buf, size)	OCTET_STRING_fromBuf((s), (buf), (size))
#define	ANY_new_fromBuf(buf, size)	OCTET_STRING_new_fromBuf(	\
						&asn_DEF_ANY, (buf), (size))

#ifdef __cplusplus
}
#endif

#endif	/* ASN_TYPE_ANY_H */

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_UTF8String_H_
#define	_UTF8String_H_

//#include <OCTET_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef OCTET_STRING_t UTF8String_t;	/* Implemented via OCTET STRING */

extern asn_TYPE_descriptor_t asn_DEF_UTF8String;

asn_struct_print_f UTF8String_print;
asn_constr_check_f UTF8String_constraint;

/*
 * Returns length of the given UTF-8 string in characters,
 * or a negative error code:
 * -1:	UTF-8 sequence truncated
 * -2:	Illegal UTF-8 sequence start
 * -3:	Continuation expectation failed
 * -4:	Not minimal length encoding
 * -5:	Invalid arguments
 */
ssize_t UTF8String_length(const UTF8String_t *st);

/*
 * Convert the UTF-8 string into a sequence of wide characters.
 * Returns the number of characters necessary.
 * Returned value might be greater than dstlen.
 * In case of conversion error, 0 is returned.
 *
 * If st points to a valid UTF-8 string, calling
 * 	UTF8String_to_wcs(st, 0, 0);
 * is equivalent to
 * 	UTF8String_length(const UTF8String_t *st);
 */
size_t UTF8String_to_wcs(const UTF8String_t *st, uint32_t *dst, size_t dstlen);

#ifdef __cplusplus
}
#endif

#endif	/* _UTF8String_H_ */

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_BMPString_H_
#define	_BMPString_H_

//#include <OCTET_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef OCTET_STRING_t BMPString_t;  /* Implemented via OCTET STRING */

extern asn_TYPE_descriptor_t asn_DEF_BMPString;

asn_struct_print_f BMPString_print;	/* Human-readable output */
xer_type_decoder_f BMPString_decode_xer;
xer_type_encoder_f BMPString_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _BMPString_H_ */

/*-
 * Copyright (c) 2003 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_BOOLEAN_H_
#define	_BOOLEAN_H_

//#include <asn_application.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The underlying integer may contain various values, but everything
 * non-zero is capped to 0xff by the DER encoder. The BER decoder may
 * yield non-zero values different from 1, beware.
 */
typedef int BOOLEAN_t;

extern asn_TYPE_descriptor_t asn_DEF_BOOLEAN;

asn_struct_free_f BOOLEAN_free;
asn_struct_print_f BOOLEAN_print;
ber_type_decoder_f BOOLEAN_decode_ber;
der_type_encoder_f BOOLEAN_encode_der;
xer_type_decoder_f BOOLEAN_decode_xer;
xer_type_encoder_f BOOLEAN_encode_xer;
per_type_decoder_f BOOLEAN_decode_uper;
per_type_encoder_f BOOLEAN_encode_uper;

#ifdef __cplusplus
}
#endif

#endif	/* _BOOLEAN_H_ */

/*-
 * Copyright (c) 2003, 2005 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_INTEGER_H_
#define	_INTEGER_H_

//#include <asn_application.h>
//#include <asn_codecs_prim.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef ASN__PRIMITIVE_TYPE_t INTEGER_t;

extern asn_TYPE_descriptor_t asn_DEF_INTEGER;

/* Map with <tag> to integer value association */
typedef struct asn_INTEGER_enum_map_s {
	long		 nat_value;	/* associated native integer value */
	size_t		 enum_len;	/* strlen("tag") */
	const char	*enum_name;	/* "tag" */
} asn_INTEGER_enum_map_t;

/* This type describes an enumeration for INTEGER and ENUMERATED types */
typedef struct asn_INTEGER_specifics_s {
	asn_INTEGER_enum_map_t *value2enum;	/* N -> "tag"; sorted by N */
	unsigned int *enum2value;		/* "tag" => N; sorted by tag */
	int map_count;				/* Elements in either map */
	int extension;				/* This map is extensible */
	int strict_enumeration;			/* Enumeration set is fixed */
} asn_INTEGER_specifics_t;

asn_struct_print_f INTEGER_print;
ber_type_decoder_f INTEGER_decode_ber;
der_type_encoder_f INTEGER_encode_der;
xer_type_decoder_f INTEGER_decode_xer;
xer_type_encoder_f INTEGER_encode_xer;
per_type_decoder_f INTEGER_decode_uper;
per_type_encoder_f INTEGER_encode_uper;

/***********************************
 * Some handy conversion routines. *
 ***********************************/

/*
 * Returns 0 if it was possible to convert, -1 otherwise.
 * -1/EINVAL: Mandatory argument missing
 * -1/ERANGE: Value encoded is out of range for long representation
 * -1/ENOMEM: Memory allocation failed (in asn_long2INTEGER()).
 */
int asn_INTEGER2long(const INTEGER_t *i, long *l);
int asn_long2INTEGER(INTEGER_t *i, long l);

/*
 * Convert the integer value into the corresponding enumeration map entry.
 */
const asn_INTEGER_enum_map_t *INTEGER_map_value2enum(asn_INTEGER_specifics_t *specs, long value);

#ifdef __cplusplus
}
#endif

#endif	/* _INTEGER_H_ */

/*-
 * Copyright (c) 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
/*
 * This type differs from the standard INTEGER in that it is modelled using
 * the fixed machine type (long, int, short), so it can hold only values of
 * limited length. There is no type (i.e., NativeInteger_t, any integer type
 * will do).
 * This type may be used when integer range is limited by subtype constraints.
 */
#ifndef	_NativeInteger_H_
#define	_NativeInteger_H_

//#include <asn_application.h>
//#include <INTEGER.h>

#ifdef __cplusplus
extern "C" {
#endif

extern asn_TYPE_descriptor_t asn_DEF_NativeInteger;

asn_struct_free_f  NativeInteger_free;
asn_struct_print_f NativeInteger_print;
ber_type_decoder_f NativeInteger_decode_ber;
der_type_encoder_f NativeInteger_encode_der;
xer_type_decoder_f NativeInteger_decode_xer;
xer_type_encoder_f NativeInteger_encode_xer;
per_type_decoder_f NativeInteger_decode_uper;
per_type_encoder_f NativeInteger_encode_uper;

#ifdef __cplusplus
}
#endif

#endif	/* _NativeInteger_H_ */

/*-
 * Copyright (c) 2004, 2005, 2006 Lev Walkin <vlm@lionet.info>.
 * All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
/*
 * This type differs from the standard ENUMERATED in that it is modelled using
 * the fixed machine type (long, int, short), so it can hold only values of
 * limited length. There is no type (i.e., NativeEnumerated_t, any integer type
 * will do).
 * This type may be used when integer range is limited by subtype constraints.
 */
#ifndef	_NativeEnumerated_H_
#define	_NativeEnumerated_H_

//#include <NativeInteger.h>

#ifdef __cplusplus
extern "C" {
#endif

extern asn_TYPE_descriptor_t asn_DEF_NativeEnumerated;

xer_type_encoder_f NativeEnumerated_encode_xer;
per_type_decoder_f NativeEnumerated_decode_uper;
per_type_encoder_f NativeEnumerated_encode_uper;

#ifdef __cplusplus
}
#endif

#endif	/* _NativeEnumerated_H_ */

/*-
 * Copyright (c) 2003, 2005 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_ENUMERATED_H_
#define	_ENUMERATED_H_

//#include <INTEGER.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef INTEGER_t ENUMERATED_t;		/* Implemented via INTEGER */

extern asn_TYPE_descriptor_t asn_DEF_ENUMERATED;

per_type_decoder_f ENUMERATED_decode_uper;
per_type_encoder_f ENUMERATED_encode_uper;

#ifdef __cplusplus
}
#endif

#endif	/* _ENUMERATED_H_ */

/*-
 * Copyright (c) 2003 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_GeneralString_H_
#define	_GeneralString_H_

//#include <OCTET_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef OCTET_STRING_t GeneralString_t;	/* Implemented via OCTET STRING */

extern asn_TYPE_descriptor_t asn_DEF_GeneralString;

#ifdef __cplusplus
}
#endif

#endif	/* _GeneralString_H_ */

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_GeneralizedTime_H_
#define	_GeneralizedTime_H_

//#include <OCTET_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef OCTET_STRING_t GeneralizedTime_t;  /* Implemented via OCTET STRING */

extern asn_TYPE_descriptor_t asn_DEF_GeneralizedTime;

asn_struct_print_f GeneralizedTime_print;
asn_constr_check_f GeneralizedTime_constraint;
der_type_encoder_f GeneralizedTime_encode_der;
xer_type_encoder_f GeneralizedTime_encode_xer;

/***********************
 * Some handy helpers. *
 ***********************/

struct tm;	/* <time.h> */

/*struct tm
{
 int tm_sec; //초 0초(0)~59초(59)
 int tm_min; //분 0분(0)~59분(59)
 int tm_hour; //시 0시(0)~23시(23)
 int tm_mday; //일 1일(1)~31일(31)
 int tm_mon; //월 1월(0)~12월(11)
 int tm_year; //년 1970년(0)~2038년(138)
 int tm_wday; //요일 일(0) 월(1) 화(2) 수(3) 목(4) 금(5) 토(6)
 int tm_yday; //1년의 경과 일 수 1월 1일은 0, 2일은 1,...
 int tm_isdst; //써머타임이 적용되면 0이 아닌 값, 그 밖에는 0
};
//*/

/*
 * Convert a GeneralizedTime structure into time_t
 * and optionally into struct tm.
 * If as_gmt is given, the resulting _optional_tm4fill will have a GMT zone,
 * instead of default local one.
 * On error returns -1 and errno set to EINVAL
 */
time_t asn_GT2time(const GeneralizedTime_t *, struct tm *_optional_tm4fill,
	int as_gmt);

/* A version of the above function also returning the fractions of seconds */
time_t asn_GT2time_frac(const GeneralizedTime_t *,
	int *frac_value, int *frac_digits,	/* (value / (10 ^ digits)) */
	struct tm *_optional_tm4fill, int as_gmt);

/*
 * Another version returning fractions with defined precision
 * For example, parsing of the time ending with ".1" seconds
 * with frac_digits=3 (msec) would yield frac_value = 100.
 */
time_t asn_GT2time_prec(const GeneralizedTime_t *,
	int *frac_value, int frac_digits,
	struct tm *_optional_tm4fill, int as_gmt);

/*
 * Convert a struct tm into GeneralizedTime.
 * If _optional_gt is not given, this function will try to allocate one.
 * If force_gmt is given, the resulting GeneralizedTime will be forced
 * into a GMT time zone (encoding ends with a "Z").
 * On error, this function returns 0 and sets errno.
 */
GeneralizedTime_t *asn_time2GT(GeneralizedTime_t *_optional_gt,
	const struct tm *, int force_gmt);
GeneralizedTime_t *asn_time2GT_frac(GeneralizedTime_t *_optional_gt,
	const struct tm *, int frac_value, int frac_digits, int force_gmt);

#ifdef __cplusplus
}
#endif

#endif	/* _GeneralizedTime_H_ */

/*-
 * Copyright (c) 2003 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_GraphicString_H_
#define	_GraphicString_H_

//#include <OCTET_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef OCTET_STRING_t GraphicString_t;	/* Implemented via OCTET STRING */

extern asn_TYPE_descriptor_t asn_DEF_GraphicString;

#ifdef __cplusplus
}
#endif

#endif	/* _GraphicString_H_ */

/*-
 * Copyright (c) 2003 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_IA5String_H_
#define	_IA5String_H_

//#include <OCTET_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef OCTET_STRING_t IA5String_t;  /* Implemented via OCTET STRING */

/*
 * IA5String ASN.1 type definition.
 */
extern asn_TYPE_descriptor_t asn_DEF_IA5String;

asn_constr_check_f IA5String_constraint;

#ifdef __cplusplus
}
#endif

#endif	/* _IA5String_H_ */

/*-
 * Copyright (c) 2003 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_VisibleString_H_
#define	_VisibleString_H_

//#include <OCTET_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef OCTET_STRING_t VisibleString_t;  /* Implemented via OCTET STRING */

extern asn_TYPE_descriptor_t asn_DEF_VisibleString;

asn_constr_check_f VisibleString_constraint;

#ifdef __cplusplus
}
#endif

#endif	/* _VisibleString_H_ */

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_ISO646String_H_
#define	_ISO646String_H_

//#include <asn_application.h>
//#include <VisibleString.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef VisibleString_t ISO646String_t;	/* Implemented using VisibleString */

extern asn_TYPE_descriptor_t asn_DEF_ISO646String;

#ifdef __cplusplus
}
#endif

#endif	/* _ISO646String_H_ */

/*-
 * Copyright (c) 2003 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	ASN_TYPE_NULL_H
#define	ASN_TYPE_NULL_H

//#include <asn_application.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The value of the NULL type is meaningless: see BOOLEAN if you want to
 * carry true/false semantics.
 */
typedef int NULL_t;

extern asn_TYPE_descriptor_t asn_DEF_NULL;

asn_struct_print_f NULL_print;
der_type_encoder_f NULL_encode_der;
xer_type_decoder_f NULL_decode_xer;
xer_type_encoder_f NULL_encode_xer;
per_type_decoder_f NULL_decode_uper;
per_type_encoder_f NULL_encode_uper;

#ifdef __cplusplus
}
#endif

#endif	/* NULL_H */

/*-
 * Copyright (c) 2003 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_BOOLEAN_H_
#define	_BOOLEAN_H_

//#include <asn_application.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The underlying integer may contain various values, but everything
 * non-zero is capped to 0xff by the DER encoder. The BER decoder may
 * yield non-zero values different from 1, beware.
 */
typedef int BOOLEAN_t;

extern asn_TYPE_descriptor_t asn_DEF_BOOLEAN;

asn_struct_free_f BOOLEAN_free;
asn_struct_print_f BOOLEAN_print;
ber_type_decoder_f BOOLEAN_decode_ber;
der_type_encoder_f BOOLEAN_encode_der;
xer_type_decoder_f BOOLEAN_decode_xer;
xer_type_encoder_f BOOLEAN_encode_xer;
per_type_decoder_f BOOLEAN_decode_uper;
per_type_encoder_f BOOLEAN_encode_uper;

#ifdef __cplusplus
}
#endif

#endif	/* _BOOLEAN_H_ */

/*-
 * Copyright (c) 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
/*
 * This type differs from the standard REAL in that it is modelled using
 * the fixed machine type (double), so it can hold only values of
 * limited precision. There is no explicit type (i.e., NativeReal_t).
 * Use of this type is normally enabled by -fnative-integers.
 */
#ifndef	ASN_TYPE_NativeReal_H
#define	ASN_TYPE_NativeReal_H

//#include <asn_application.h>

#ifdef __cplusplus
extern "C" {
#endif

extern asn_TYPE_descriptor_t asn_DEF_NativeReal;

asn_struct_free_f  NativeReal_free;
asn_struct_print_f NativeReal_print;
ber_type_decoder_f NativeReal_decode_ber;
der_type_encoder_f NativeReal_encode_der;
xer_type_decoder_f NativeReal_decode_xer;
xer_type_encoder_f NativeReal_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* ASN_TYPE_NativeReal_H */

/*-
 * Copyright (c) 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	ASN_TYPE_REAL_H
#define	ASN_TYPE_REAL_H

//#include <asn_application.h>
//#include <asn_codecs_prim.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef ASN__PRIMITIVE_TYPE_t REAL_t;

extern asn_TYPE_descriptor_t asn_DEF_REAL;

asn_struct_print_f REAL_print;
xer_type_decoder_f REAL_decode_xer;
xer_type_encoder_f REAL_encode_xer;

/***********************************
 * Some handy conversion routines. *
 ***********************************/

ssize_t REAL__dump(double d, int canonical, asn_app_consume_bytes_f *cb, void *app_key);

/*
 * Convert between native double type and REAL representation (DER).
 * RETURN VALUES:
 *  0: Value converted successfully
 * -1: An error occured while converting the value: invalid format.
 */
int asn_REAL2double(const REAL_t *real_ptr, double *d);
int asn_double2REAL(REAL_t *real_ptr, double d);

#ifdef __cplusplus
}
#endif

#endif	/* ASN_TYPE_REAL_H */

/*-
 * Copyright (c) 2003 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_NumericString_H_
#define	_NumericString_H_

//#include <OCTET_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef OCTET_STRING_t NumericString_t;	/* Implemented via OCTET STRING */

extern asn_TYPE_descriptor_t asn_DEF_NumericString;

asn_constr_check_f NumericString_constraint;

#ifdef __cplusplus
}
#endif

#endif	/* _NumericString_H_ */

/*-
 * Copyright (c) 2003, 2004, 2005 Lev Walkin <vlm@lionet.info>.
 * 	All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_OBJECT_IDENTIFIER_H_
#define	_OBJECT_IDENTIFIER_H_

//#include <asn_application.h>
//#include <asn_codecs_prim.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef ASN__PRIMITIVE_TYPE_t OBJECT_IDENTIFIER_t;

extern asn_TYPE_descriptor_t asn_DEF_OBJECT_IDENTIFIER;

asn_struct_print_f OBJECT_IDENTIFIER_print;
asn_constr_check_f OBJECT_IDENTIFIER_constraint;
der_type_encoder_f OBJECT_IDENTIFIER_encode_der;
xer_type_decoder_f OBJECT_IDENTIFIER_decode_xer;
xer_type_encoder_f OBJECT_IDENTIFIER_encode_xer;

/**********************************
 * Some handy conversion routines *
 **********************************/

/*
 * This function fills an (_arcs) array with OBJECT IDENTIFIER arcs
 * up to specified (_arc_slots) elements.
 *
 * EXAMPLE:
 * 	void print_arcs(OBJECT_IDENTIFIER_t *oid) {
 * 		unsigned long fixed_arcs[10];	// Try with fixed space first
 * 		unsigned long *arcs = fixed_arcs;
 * 		int arc_type_size = sizeof(fixed_arcs[0]);	// sizeof(long)
 * 		int arc_slots = sizeof(fixed_arcs)/sizeof(fixed_arcs[0]); // 10
 * 		int count;	// Real number of arcs.
 * 		int i;
 *
 * 		count = OBJECT_IDENTIFIER_get_arcs(oid, arcs,
 * 			arc_type_size, arc_slots);
 * 		// If necessary, reallocate arcs array and try again.
 * 		if(count > arc_slots) {
 * 			arc_slots = count;
 * 			arcs = malloc(arc_type_size * arc_slots);
 * 			if(!arcs) return;
 * 			count = OBJECT_IDENTIFIER_get_arcs(oid, arcs,
 * 				arc_type_size, arc_slots);
 * 			assert(count == arc_slots);
 * 		}
 *
 * 		// Print the contents of the arcs array.
 * 		for(i = 0; i < count; i++)
 * 			printf("%d\n", arcs[i]);
 *
 * 		// Avoid memory leak.
 * 		if(arcs != fixed_arcs) free(arcs);
 * 	}
 *
 * RETURN VALUES:
 * -1/EINVAL:	Invalid arguments (oid is missing)
 * -1/ERANGE:	One or more arcs have value out of array cell type range.
 * >=0:		Number of arcs contained in the OBJECT IDENTIFIER
 *
 * WARNING: The function always returns the real number of arcs,
 * even if there is no sufficient (_arc_slots) provided.
 */
int OBJECT_IDENTIFIER_get_arcs(OBJECT_IDENTIFIER_t *_oid,
	void *_arcs,			/* e.g., unsigned int arcs[N] */
	unsigned int _arc_type_size,	/* e.g., sizeof(arcs[0]) */
	unsigned int _arc_slots		/* e.g., N */);

/*
 * This functions initializes the OBJECT IDENTIFIER object with
 * the given set of arcs.
 * The minimum of two arcs must be present; some restrictions apply.
 * RETURN VALUES:
 * -1/EINVAL:	Invalid arguments
 * -1/ERANGE:	The first two arcs do not conform to ASN.1 restrictions.
 * -1/ENOMEM:	Memory allocation failed
 * 0:		The object was initialized with new arcs.
 */
int OBJECT_IDENTIFIER_set_arcs(OBJECT_IDENTIFIER_t *_oid,
	const void *_arcs,		/* e.g., unsigned int arcs[N] */
	unsigned int _arc_type_size,	/* e.g., sizeof(arcs[0]) */
	unsigned int _arc_slots		/* e.g., N */);

/*
 * Print the specified OBJECT IDENTIFIER arc.
 */
int OBJECT_IDENTIFIER_print_arc(uint8_t *arcbuf, int arclen,
	int add, /* Arbitrary offset, required to process the first two arcs */
	asn_app_consume_bytes_f *cb, void *app_key);

/* Same as above, but returns the number of written digits, instead of 0 */
ssize_t OBJECT_IDENTIFIER__dump_arc(uint8_t *arcbuf, int arclen, int add,
	asn_app_consume_bytes_f *cb, void *app_key);

/*
 * Parse the OBJECT IDENTIFIER textual representation ("1.3.6.1.4.1.9363").
 * No arc can exceed the (0..signed_long_max) range (typically, 0..2G if L32).
 * This function is not specific to OBJECT IDENTIFIER, it may be used to parse
 * the RELATIVE-OID data, or any other data consisting of dot-separated
 * series of numeric values.
 *
 * If (oid_txt_length == -1), the strlen() will be invoked to determine the
 * size of the (oid_text) string.
 *
 * After return, the optional (opt_oid_text_end) is set to the character after
 * the last parsed one. (opt_oid_text_end) is never less than (oid_text).
 *
 * RETURN VALUES:
 *   -1:	Parse error.
 * >= 0:	Number of arcs contained in the OBJECT IDENTIFIER.
 *
 * WARNING: The function always returns the real number of arcs,
 * even if there is no sufficient (_arc_slots) provided.
 * This is useful for (_arc_slots) value estimation.
 */
int OBJECT_IDENTIFIER_parse_arcs(const char *oid_text, ssize_t oid_txt_length,
	long arcs[], unsigned int arcs_slots, const char **opt_oid_text_end);

/*
 * Internal functions.
 * Used by RELATIVE-OID implementation in particular.
 */
int OBJECT_IDENTIFIER_get_single_arc(uint8_t *arcbuf, unsigned int arclen,
	signed int add, void *value, unsigned int value_size);
int OBJECT_IDENTIFIER_set_single_arc(uint8_t *arcbuf,
	const void *arcval, unsigned int arcval_size, int _prepared_order);

#ifdef __cplusplus
}
#endif

#endif	/* _OBJECT_IDENTIFIER_H_ */

/*-
 * Copyright (c) 2003 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_PrintableString_H_
#define	_PrintableString_H_

//#include <OCTET_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef OCTET_STRING_t PrintableString_t;  /* Implemented via OCTET STRING */

extern asn_TYPE_descriptor_t asn_DEF_PrintableString;

asn_constr_check_f PrintableString_constraint;

#ifdef __cplusplus
}
#endif

#endif	/* _PrintableString_H_ */

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_ObjectDescriptor_H_
#define	_ObjectDescriptor_H_

//#include <GraphicString.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef GraphicString_t ObjectDescriptor_t;  /* Implemented via GraphicString */

extern asn_TYPE_descriptor_t asn_DEF_ObjectDescriptor;

#ifdef __cplusplus
}
#endif

#endif	/* _ObjectDescriptor_H_ */

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_RELATIVE_OID_H_
#define	_RELATIVE_OID_H_

//#include <OBJECT_IDENTIFIER.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Implemented via OBJECT IDENTIFIER */
typedef OBJECT_IDENTIFIER_t RELATIVE_OID_t;

extern asn_TYPE_descriptor_t asn_DEF_RELATIVE_OID;

asn_struct_print_f RELATIVE_OID_print;
xer_type_decoder_f RELATIVE_OID_decode_xer;
xer_type_encoder_f RELATIVE_OID_encode_xer;

/**********************************
 * Some handy conversion routines *
 **********************************/

/* See OBJECT_IDENTIFIER_get_arcs() function in OBJECT_IDENTIFIER.h */
int RELATIVE_OID_get_arcs(RELATIVE_OID_t *_roid,
	void *arcs, unsigned int arc_type_size, unsigned int arc_slots);

/* See OBJECT_IDENTIFIER_set_arcs() function in OBJECT_IDENTIFIER.h */
int RELATIVE_OID_set_arcs(RELATIVE_OID_t *_roid,
	void *arcs, unsigned int arc_type_size, unsigned int arcs_slots);

#ifdef __cplusplus
}
#endif

#endif	/* _RELATIVE_OID_H_ */

/*-
 * Copyright (c) 2003 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_T61String_H_
#define	_T61String_H_

//#include <OCTET_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef OCTET_STRING_t T61String_t;	/* Implemented via OCTET STRING */

extern asn_TYPE_descriptor_t asn_DEF_T61String;

#ifdef __cplusplus
}
#endif

#endif	/* _T61String_H_ */

/*-
 * Copyright (c) 2003 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_TeletexString_H_
#define	_TeletexString_H_

//#include <OCTET_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef OCTET_STRING_t TeletexString_t;	/* Implemented via OCTET STRING */

extern asn_TYPE_descriptor_t asn_DEF_TeletexString;

#ifdef __cplusplus
}
#endif

#endif	/* _TeletexString_H_ */

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_UTCTime_H_
#define	_UTCTime_H_

//#include <OCTET_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef OCTET_STRING_t UTCTime_t;  /* Implemented via OCTET STRING */

extern asn_TYPE_descriptor_t asn_DEF_UTCTime;

asn_struct_print_f UTCTime_print;
asn_constr_check_f UTCTime_constraint;
xer_type_encoder_f UTCTime_encode_xer;

/***********************
 * Some handy helpers. *
 ***********************/

struct tm;	/* <time.h> */

/* See asn_GT2time() in GeneralizedTime.h */
time_t asn_UT2time(const UTCTime_t *, struct tm *_optional_tm4fill, int as_gmt);

/* See asn_time2GT() in GeneralizedTime.h */
UTCTime_t *asn_time2UT(UTCTime_t *__opt_ut, const struct tm *, int force_gmt);

#ifdef __cplusplus
}
#endif

#endif	/* _UTCTime_H_ */

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_UTF8String_H_
#define	_UTF8String_H_

//#include <OCTET_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef OCTET_STRING_t UTF8String_t;	/* Implemented via OCTET STRING */

extern asn_TYPE_descriptor_t asn_DEF_UTF8String;

asn_struct_print_f UTF8String_print;
asn_constr_check_f UTF8String_constraint;

/*
 * Returns length of the given UTF-8 string in characters,
 * or a negative error code:
 * -1:	UTF-8 sequence truncated
 * -2:	Illegal UTF-8 sequence start
 * -3:	Continuation expectation failed
 * -4:	Not minimal length encoding
 * -5:	Invalid arguments
 */
ssize_t UTF8String_length(const UTF8String_t *st);

/*
 * Convert the UTF-8 string into a sequence of wide characters.
 * Returns the number of characters necessary.
 * Returned value might be greater than dstlen.
 * In case of conversion error, 0 is returned.
 *
 * If st points to a valid UTF-8 string, calling
 * 	UTF8String_to_wcs(st, 0, 0);
 * is equivalent to
 * 	UTF8String_length(const UTF8String_t *st);
 */
size_t UTF8String_to_wcs(const UTF8String_t *st, uint32_t *dst, size_t dstlen);

#ifdef __cplusplus
}
#endif

#endif	/* _UTF8String_H_ */

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_UniversalString_H_
#define	_UniversalString_H_

//#include <OCTET_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef OCTET_STRING_t UniversalString_t;  /* Implemented via OCTET STRING */

extern asn_TYPE_descriptor_t asn_DEF_UniversalString;

asn_struct_print_f UniversalString_print;	/* Human-readable output */
xer_type_decoder_f UniversalString_decode_xer;
xer_type_encoder_f UniversalString_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _UniversalString_H_ */

/*-
 * Copyright (c) 2003 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_VisibleString_H_
#define	_VisibleString_H_

//#include <OCTET_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef OCTET_STRING_t VisibleString_t;  /* Implemented via OCTET STRING */

extern asn_TYPE_descriptor_t asn_DEF_VisibleString;

asn_constr_check_f VisibleString_constraint;

#ifdef __cplusplus
}
#endif

#endif	/* _VisibleString_H_ */

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	ASN_SET_OF_H
#define	ASN_SET_OF_H

#ifdef __cplusplus
extern "C" {
#endif

#define	A_SET_OF(type)					\
	struct {					\
		type **array;				\
		int count;	/* Meaningful size */	\
		int size;	/* Allocated size */	\
		void (*free)(type *);			\
	}

#define	ASN_SET_ADD(headptr, ptr)		\
	asn_set_add((headptr), (ptr))

/*******************************************
 * Implementation of the SET OF structure.
 */

/*
 * Add another structure into the set by its pointer.
 * RETURN VALUES:
 * 0 for success and -1/errno for failure.
 */
int  asn_set_add(void *asn_set_of_x, void *ptr);

/*
 * Delete the element from the set by its number (base 0).
 * This is a constant-time operation. The order of elements before the
 * deleted ones is guaranteed, the order of elements after the deleted
 * one is NOT guaranteed.
 * If _do_free is given AND the (*free) is initialized, the element
 * will be freed using the custom (*free) function as well.
 */
void asn_set_del(void *asn_set_of_x, int number, int _do_free);

/*
 * Empty the contents of the set. Will free the elements, if (*free) is given.
 * Will NOT free the set itself.
 */
void asn_set_empty(void *asn_set_of_x);

/*
 * Cope with different conversions requirements to/from void in C and C++.
 * This is mostly useful for support library.
 */
typedef A_SET_OF(void) asn_anonymous_set_;
#define _A_SET_FROM_VOID(ptr)		((asn_anonymous_set_ *)(ptr))
#define _A_CSET_FROM_VOID(ptr)		((const asn_anonymous_set_ *)(ptr))

#ifdef __cplusplus
}
#endif

#endif	/* ASN_SET_OF_H */

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	ASN_SEQUENCE_OF_H
#define	ASN_SEQUENCE_OF_H

//#include <asn_SET_OF.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SEQUENCE OF is the same as SET OF with a tiny difference:
 * the delete operation preserves the initial order of elements
 * and thus MAY operate in non-constant time.
 */
#define	A_SEQUENCE_OF(type)	A_SET_OF(type)

#define	ASN_SEQUENCE_ADD(headptr, ptr)		\
	asn_sequence_add((headptr), (ptr))

/***********************************************
 * Implementation of the SEQUENCE OF structure.
 */

#define	asn_sequence_add	asn_set_add
#define	asn_sequence_empty	asn_set_empty

/*
 * Delete the element from the set by its number (base 0).
 * This is NOT a constant-time operation.
 * The order of elements is preserved.
 * If _do_free is given AND the (*free) is initialized, the element
 * will be freed using the custom (*free) function as well.
 */
void asn_sequence_del(void *asn_sequence_of_x, int number, int _do_free);

/*
 * Cope with different conversions requirements to/from void in C and C++.
 * This is mostly useful for support library.
 */
typedef A_SEQUENCE_OF(void) asn_anonymous_sequence_;
#define _A_SEQUENCE_FROM_VOID(ptr)	((asn_anonymous_sequence_ *)(ptr))
#define _A_CSEQUENCE_FROM_VOID(ptr) 	((const asn_anonymous_sequence_ *)(ptr))

#ifdef __cplusplus
}
#endif

#endif	/* ASN_SEQUENCE_OF_H */

/*-
 * Copyright (c) 2003, 2004, 2005 Lev Walkin <vlm@lionet.info>.
 * All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_CONSTR_CHOICE_H_
#define	_CONSTR_CHOICE_H_

//#include <asn_application.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct asn_CHOICE_specifics_s {
	/*
	 * Target structure description.
	 */
	int struct_size;	/* Size of the target structure. */
	int ctx_offset;		/* Offset of the asn_codec_ctx_t member */
	int pres_offset;	/* Identifier of the present member */
	int pres_size;		/* Size of the identifier (enum) */

	/*
	 * Tags to members mapping table.
	 */
	asn_TYPE_tag2member_t *tag2el;
	int tag2el_count;

	/* Canonical ordering of CHOICE elements, for PER */
	int *canonical_order;

	/*
	 * Extensions-related stuff.
	 */
	int ext_start;		/* First member of extensions, or -1 */
} asn_CHOICE_specifics_t;

/*
 * A set specialized functions dealing with the CHOICE type.
 */
asn_struct_free_f CHOICE_free;
asn_struct_print_f CHOICE_print;
asn_constr_check_f CHOICE_constraint;
ber_type_decoder_f CHOICE_decode_ber;
der_type_encoder_f CHOICE_encode_der;
xer_type_decoder_f CHOICE_decode_xer;
xer_type_encoder_f CHOICE_encode_xer;
per_type_decoder_f CHOICE_decode_uper;
per_type_encoder_f CHOICE_encode_uper;
asn_outmost_tag_f CHOICE_outmost_tag;

#ifdef __cplusplus
}
#endif

#endif	/* _CONSTR_CHOICE_H_ */

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_CONSTR_SEQUENCE_H_
#define	_CONSTR_SEQUENCE_H_

//#include <asn_application.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct asn_SEQUENCE_specifics_s {
	/*
	 * Target structure description.
	 */
	int struct_size;	/* Size of the target structure. */
	int ctx_offset;		/* Offset of the asn_struct_ctx_t member */

	/*
	 * Tags to members mapping table (sorted).
	 */
	asn_TYPE_tag2member_t *tag2el;
	int tag2el_count;

	/*
	 * Optional members of the extensions root (roms) or additions (aoms).
	 * Meaningful for PER.
	 */
	int *oms;		/* Optional MemberS */
	int  roms_count;	/* Root optional members count */
	int  aoms_count;	/* Additions optional members count */

	/*
	 * Description of an extensions group.
	 */
	int ext_after;		/* Extensions start after this member */
	int ext_before;		/* Extensions stop before this member */
} asn_SEQUENCE_specifics_t;

/*
 * A set specialized functions dealing with the SEQUENCE type.
 */
asn_struct_free_f SEQUENCE_free;
asn_struct_print_f SEQUENCE_print;
asn_constr_check_f SEQUENCE_constraint;
ber_type_decoder_f SEQUENCE_decode_ber;
der_type_encoder_f SEQUENCE_encode_der;
xer_type_decoder_f SEQUENCE_decode_xer;
xer_type_encoder_f SEQUENCE_encode_xer;
per_type_decoder_f SEQUENCE_decode_uper;
per_type_encoder_f SEQUENCE_encode_uper;

#ifdef __cplusplus
}
#endif

#endif	/* _CONSTR_SEQUENCE_H_ */

/*-
 * Copyright (c) 2003, 2005 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_CONSTR_SEQUENCE_OF_H_
#define	_CONSTR_SEQUENCE_OF_H_

//#include <asn_application.h>
//#include <constr_SET_OF.h>		/* Implemented using SET OF */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * A set specialized functions dealing with the SEQUENCE OF type.
 * Generally implemented using SET OF.
 */
#define	SEQUENCE_OF_free	SET_OF_free
#define	SEQUENCE_OF_print	SET_OF_print
#define	SEQUENCE_OF_constraint	SET_OF_constraint
#define	SEQUENCE_OF_decode_ber	SET_OF_decode_ber
#define	SEQUENCE_OF_decode_xer	SET_OF_decode_xer
#define	SEQUENCE_OF_decode_uper	SET_OF_decode_uper
der_type_encoder_f SEQUENCE_OF_encode_der;
xer_type_encoder_f SEQUENCE_OF_encode_xer;
per_type_encoder_f SEQUENCE_OF_encode_uper;

#ifdef __cplusplus
}
#endif

#endif	/* _CONSTR_SET_OF_H_ */

/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_CONSTR_SET_H_
#define	_CONSTR_SET_H_

//#include <asn_application.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct asn_SET_specifics_s {
	/*
	 * Target structure description.
	 */
	int struct_size;	/* Size of the target structure. */
	int ctx_offset;		/* Offset of the asn_struct_ctx_t member */
	int pres_offset;	/* Offset of _presence_map member */

	/*
	 * Tags to members mapping table (sorted).
	 * Sometimes suitable for DER encoding (untagged CHOICE is present);
	 * if so, tag2el_count will be greater than td->elements_count.
	 */
	asn_TYPE_tag2member_t *tag2el;
	int tag2el_count;

	/*
	 * Tags to members mapping table, second edition.
	 * Suitable for CANONICAL-XER encoding.
	 */
	asn_TYPE_tag2member_t *tag2el_cxer;
	int tag2el_cxer_count;

	/*
	 * Extensions-related stuff.
	 */
	int extensible;				/* Whether SET is extensible */
	unsigned int *_mandatory_elements;	/* Bitmask of mandatory ones */
} asn_SET_specifics_t;

/*
 * A set specialized functions dealing with the SET type.
 */
asn_struct_free_f SET_free;
asn_struct_print_f SET_print;
asn_constr_check_f SET_constraint;
ber_type_decoder_f SET_decode_ber;
der_type_encoder_f SET_encode_der;
xer_type_decoder_f SET_decode_xer;
xer_type_encoder_f SET_encode_xer;
per_type_decoder_f SET_decode_uper;
per_type_encoder_f SET_encode_uper;

/***********************
 * Some handy helpers. *
 ***********************/

/*
 * Figure out whether the SET member indicated by PR_x has already been decoded.
 * It is very simple bitfield test, despite its visual complexity.
 */
#define	ASN_SET_ISPRESENT(set_ptr, PR_x)				\
	ASN_SET_ISPRESENT2(&((set_ptr)->_presence_map), PR_x)
#define	ASN_SET_ISPRESENT2(map_ptr, PR_x)				\
	(((unsigned int *)(map_ptr))					\
		[(PR_x)	/ (8 * sizeof(unsigned int))]			\
		& (1 << ((8 * sizeof(unsigned int)) - 1			\
		- ((PR_x) % (8 * sizeof(unsigned int))))))

#define	ASN_SET_MKPRESENT(map_ptr, PR_x)				\
	(((unsigned int *)(map_ptr))					\
		[(PR_x)	/ (8 * sizeof(unsigned int))]			\
		|= (1 << ((8 * sizeof(unsigned int)) - 1		\
		- ((PR_x) % (8 * sizeof(unsigned int))))))

#ifdef __cplusplus
}
#endif

#endif	/* _CONSTR_SET_H_ */

/*-
 * Copyright (c) 2003 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_CONSTR_SET_OF_H_
#define	_CONSTR_SET_OF_H_

//#include <asn_application.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct asn_SET_OF_specifics_s {
	/*
	 * Target structure description.
	 */
	int struct_size;	/* Size of the target structure. */
	int ctx_offset;		/* Offset of the asn_struct_ctx_t member */

	/* XER-specific stuff */
	int as_XMLValueList;	/* The member type must be encoded like this */
} asn_SET_OF_specifics_t;

/*
 * A set specialized functions dealing with the SET OF type.
 */
asn_struct_free_f SET_OF_free;
asn_struct_print_f SET_OF_print;
asn_constr_check_f SET_OF_constraint;
ber_type_decoder_f SET_OF_decode_ber;
der_type_encoder_f SET_OF_encode_der;
xer_type_decoder_f SET_OF_decode_xer;
xer_type_encoder_f SET_OF_encode_xer;
per_type_decoder_f SET_OF_decode_uper;
per_type_encoder_f SET_OF_encode_uper;

#ifdef __cplusplus
}
#endif

#endif	/* _CONSTR_SET_OF_H_ */

#endif	/* ASN_LIB_H */
