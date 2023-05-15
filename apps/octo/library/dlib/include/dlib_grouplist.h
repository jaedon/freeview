/* $Header: $ */

#ifndef DLIB_FAVORITE_H__
#define DLIB_FAVORITE_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <hlib.h>

typedef struct
{
	HCHAR	szGroupName[DxNAME_LEN];
	HINT32	nSize;
	HUINT8	pucData[4];
} DxGroupList_t;

#ifdef __cplusplus
}
#endif

#endif // DLIB_FAVORITE_H__
