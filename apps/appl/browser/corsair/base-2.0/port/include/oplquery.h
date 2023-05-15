/**************************************************************************************/
/**
 * @file oplquery.h
 *
 *
 *
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __OPLQUERY_H__
#define __OPLQUERY_H__

#include	"opltype.h"

#ifdef __cplusplus
extern "C" {
#endif	//__cplusplus

enum _OPL_QueryComparision
{
	eOPL_QUERY_EQ		= 0,
	eOPL_QUERY_NOT_EQ	= 1,
	eOPL_QUERY_GT		= 2,
	eOPL_QUERY_GT_OR_EQ	= 3,
	eOPL_QUERY_LT		= 4,
	eOPL_QUERY_LT_OR_EQ	= 5,
	eOPL_QUERY_CONTAINS	= 6,
	eOPL_QUERY_EXIST	= 7,

	eOPL_QUERY_UNKNOWN
};

/**
 *
 *
 * @param	type	AND: '&', OR: '|', NOT: '!', QUERY: 0
 */
OPL_HANDLE	OPL_QUERY_New (const char *field, int comparison, const char *value);
void		OPL_QUERY_Delete (OPL_HANDLE q);
OPL_HANDLE	OPL_QUERY_Clone (const OPL_HANDLE q);

OPL_HANDLE	OPL_QUERY_And (const OPL_HANDLE q1, const OPL_HANDLE q2);
OPL_HANDLE	OPL_QUERY_Or  (const OPL_HANDLE q1, const OPL_HANDLE q2);
OPL_HANDLE	OPL_QUERY_Not (const OPL_HANDLE q);



#ifdef __cplusplus
};
#endif	//__cplusplus

#endif	//__OPLQUERY_H__

