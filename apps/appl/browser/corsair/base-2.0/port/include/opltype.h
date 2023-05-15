/**************************************************************************************/
/**
 * @file opltype.h
 *
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __OPLTYPE_H__
#define __OPLTYPE_H__

#ifdef __cplusplus
	extern "C" {
#endif

#if defined(_ICC)
#define OPL_INLINE __inline
#elif defined(__GNUC__)
#define OPL_INLINE	inline
#else
#define OPL_INLINE
#endif
#define	OPL_STATIC	static

typedef enum
{
	OPL_FALSE,
	OPL_TRUE
} OPL_BOOL;

typedef void *	OPL_HANDLE;

#include	"oplcollection.h"
#include	"oplevent.h"


#ifdef __cplusplus
};
#endif

#endif

