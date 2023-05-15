

/*
 * hxid.c
 *
 *  Created on: 2012. 8. 6.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "_hlib_mem.h"
#include "_hlib_id.h"
#include "hlib.h"

typedef struct
{
	HINT32	offset;
	HINT32	max;
	HINT32	last;
	HUINT32	flags[1];
} id_mgr_t;


HxID_t	HLIB_ID_New (HUINT16 min, HUINT16 max)
{
	id_mgr_t *mgr;
	HINT32    size;

	if (min == HxID_NULL || min >= max)
		return (HxID_t)NULL;

	size = (HINT32)(max - min);
	mgr  = (id_mgr_t *)HLIB_MEM_Calloc(sizeof(id_mgr_t) + sizeof(HUINT32) * (size / 32));
	if (mgr == NULL)
		return (HxID_t)NULL;

	mgr->offset = (HINT32)min;
	mgr->max    = size;

	return (HxID_t)mgr;
}


void	HLIB_ID_Delete (HxID_t hID)
{
	if (hID)
	{
		HLIB_MEM_Free((void *)hID);
	}
}

HINT32	HLIB_ID_Alloc (HxID_t hID)
{
	id_mgr_t *mgr = (id_mgr_t *)hID;
	HINT32	  i, j, n, c;

	if (mgr == NULL)
		return HxID_NULL;

	mgr = (id_mgr_t *)hID;
	n=c = mgr->max / 32 + 1; // array size
	for (i = mgr->last ; n-- ; i = (i + 1) % c)
	{
		if (mgr->flags[i] == 0xFFFFFFFF)
			continue;
		for (j = 0 ; j < 32 ; j++)
		{
			if (!(mgr->flags[i] & (1 << j)) && i * 32 + j <= mgr->max)
			{
				mgr->last = i;
				i = i * 32 + j;
				mgr->flags[i / 32] |= (1 << (i % 32));
				return i + mgr->offset;
			}
		}
	}
	return HxID_NULL;
}

void	HLIB_ID_Free (HxID_t hID, HINT32 id)
{
	id_mgr_t *mgr = (id_mgr_t *)hID;

	if (mgr == NULL)
		return;

	id -= mgr->offset;
	if (id < 0 || mgr->max < id)
		return;
	mgr->flags[id / 32] &= ~(1 << (id % 32));
}

HINT32	HLIB_ID_AllocWithSpecifiedID (HxID_t hID, HINT32 id)
{
	id_mgr_t *mgr = (id_mgr_t *)hID;

	if (mgr == NULL)
		return HxID_NULL;

	id -= mgr->offset;
	if (id < 0 || mgr->max < id)
		return HxID_NULL;

	if (mgr->flags[id / 32] & (1 << (id % 32)))
		return HxID_NULL;

	mgr->flags[id / 32] |= (1 << (id % 32));
	return id + mgr->offset;
}

HBOOL	HLIB_ID_HasID (HxID_t hID, HINT32 id)
{
	id_mgr_t *mgr = (id_mgr_t *)hID;

	if (mgr == NULL)
		return FALSE;

	id -= mgr->offset;
	if (id < 0 || mgr->max < id)
		return FALSE;

	if (mgr->flags[id / 32] & (1 << (id % 32)))
		return TRUE;
	return FALSE;
}


// 일단 여기에 넣자 ... 파일 만들기 귀찮다.
HUID	HUID_String8(const HCHAR *str)
{
	HUID   uid = 0ULL;
	HINT32 n   = sizeof(HUID);

	while (n-- && *str != '\0')
	{
		uid = uid << sizeof(HUID) | *str++;
	}
	return uid;
}

HCHAR *	HUID_ToString(HCHAR *str, HUID uid)
{
	HINT32 n = sizeof(HUID);
	HCHAR *r = str;
	// 설마 str사이즈 9 안되는거 넣지는 않겠지??
	while (n--)
	{
		HCHAR c = (HCHAR)((uid >> (sizeof(HUID) * n)) & 0xFF);
		if (c)
			*str++ = c;
	}
	*str = '\0';
	return r;
}




