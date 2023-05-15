/**************************************************************************************/
/**
 * @file oplbookmark.c
 *
 *
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include "oplbookmark.h"

#include <apk.h>
#include <hlib.h>

#include "vkernel.h"

OPL_Bookmark_t*		OPL_Bookmark_New (unsigned int	time, const char *name)
{
	OPL_Bookmark_t	*self;

	HxLOG_Trace();

	self = (OPL_Bookmark_t *)HLIB_STD_MemCalloc(sizeof(OPL_Bookmark_t));
	if (self)
	{
		self->time	= time;
		self->name	= HLIB_STD_StrDup(name);
	}

	return self;
}

void				OPL_Bookmark_Delete (OPL_Bookmark_t *self)
{
	HxLOG_Trace();

	if (self == NULL)
		return;

	if (self->name)
	{
		HLIB_STD_MemFree(self->name);
		self->name = NULL;
	}

	HLIB_STD_MemFree(self);
}

OPL_Bookmark_t *	OPL_Bookmark_Clone (const OPL_Bookmark_t *self)
{
	OPL_Bookmark_t *Clone = NULL;
	if (self)
	{
		Clone = OPL_Bookmark_New(self->time, self->name);
		return Clone;
	}
	return NULL;
}