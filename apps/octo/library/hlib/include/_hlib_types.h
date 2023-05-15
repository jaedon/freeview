
#ifndef _HxTYPES_H_
#define _HxTYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/* include generic files */
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#include 	<ctype.h>
#include 	<stddef.h>
#include	<getopt.h>
#include	<errno.h>

/* include di_inc files */
#include 	<htype.h>

typedef HINT64		HOffset_t;
typedef HUINT32		Handle_t;

typedef struct
{
	void *	left;
	void *	right;
} HxPair_t;


typedef HINT32	(* HxCompareFunc_t) (const void *lhs, const void *rhs);
typedef HBOOL	(* HxEqualFunc_t) (const void *lhs, const void *rhs);
typedef void	(* HxFreeFunc_t) (void *element);
typedef void *	(* HxAllocFunc_t) (HUINT32 size);
typedef void *	(* HxCloneFunc_t) (const void *org);
typedef void	(* HxFreeExFunc_t) (void *element, void *userdata);
typedef void *	(* HxAllocExFunc_t) (HUINT32 size, void *userdata);
typedef void *	(* HxCloneExFunc_t) (const void *org, void *userdata);
typedef void	(* HxVoidFunc_t) (void);
typedef HINT32	(* HxIoctlFunc_t) (void *userdata, HINT32 request, ...);
typedef HINT32	(* HxPrintFunc_t) (void *stream, const HCHAR *format, ...);
typedef void	(* HxReturnFunc_t) (HERROR err, void *userdata, ...);

#ifdef __cplusplus
}
#endif


#define TEST_CP // code page 처리하는 부분에 오류가 있어서 수정함. 임시로 Base project에서 확인 후 문제 없을 경우 삭제될 define임.
#endif /* _HxTYPES_H_ */

