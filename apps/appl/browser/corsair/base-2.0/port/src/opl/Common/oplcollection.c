/**************************************************************************************/
/**
 * @file oplcollection.c
 *
 * Defines the collection structure.
 *
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include "oplcollection.h"

#include <hlib.h>
#include <apk.h>





#define	____OPL_COLLECTION_PRIVATE_____________________________________________________________________

#define	____OPL_COLLECTION_PUBLIC______________________________________________________________________


OPL_Collection_t *	OPL_Collection_New (void)
{
	return OPL_Collection_NewEx(NULL, NULL);
}

OPL_Collection_t *	OPL_Collection_NewEx  (void (*removeNotify)(void *), void * (*clone)(void *))
{
	OPL_Collection_t *self;

	HxLOG_Trace();

	self = (OPL_Collection_t *)HLIB_STD_MemAlloc(sizeof(OPL_Collection_t));
	if (self == NULL)
		return NULL;

	self->collection = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))removeNotify, NULL);
	if (self->collection == NULL)
	{
		HLIB_STD_MemFree(self);
		return NULL;
	}
	self->clone = clone;
	self->removeNotify = removeNotify;

	return self;
}


OPL_Collection_t *	OPL_Collection_Clone  (OPL_Collection_t *self, void * (*clone)(void *))
{
        OPL_Collection_t *thiz;
        int		i, n;
        void	*data;

        HxLOG_Trace();
        HxLOG_Assert(self);

        if (self == NULL)
                return NULL;

        if (clone == NULL)
        	clone = self->clone;

        thiz = OPL_Collection_NewEx(self->removeNotify, self->clone);
        if (thiz == NULL)
        	return NULL;

        n = OPL_Collection_Length(self);
        for (i = 0 ; i < n ; i++)
        {
        	data = OPL_Collection_ItemAt(self, i);
        	if (clone)
        		data = clone(data);
        	OPL_Collection_Add(thiz, data);
        }
        return thiz;
}


void		OPL_Collection_Delete (OPL_Collection_t * self)
{
	HxLOG_Trace();

	if (self)
	{
		if (self->collection)
			HLIB_VECTOR_Delete(self->collection);
		HLIB_STD_MemFree(self);
	}
}

int			OPL_Collection_Add (OPL_Collection_t * self, void *data)
{
	HxLOG_Trace();
	HxLOG_Assert(self && self->collection);

        if (self == NULL || self->collection == NULL)
                return -1;

	if (HLIB_VECTOR_Add(self->collection, data) != ERR_OK)
		return -1;
	return (int)HLIB_VECTOR_Length(self->collection) - 1;
}

int			OPL_Collection_Insert (OPL_Collection_t * self, int n, void *data)
{
	HxLOG_Trace();
	HxLOG_Assert(self && self->collection);
    
        if (self == NULL || self->collection == NULL)
                return -1;

	if (HLIB_VECTOR_Insert(self->collection, (HINT32)n, data) != ERR_OK)
		return -1;
	return n;
}

int			OPL_Collection_Remove (OPL_Collection_t * self, int n)
{
	HxLOG_Trace();
	HxLOG_Assert(self && self->collection);

        if (self == NULL || self->collection == NULL)
                return -1;

	if (HLIB_VECTOR_Remove(self->collection, (HINT32)n) != ERR_OK)
		return -1;
	return n;
}

void		OPL_Collection_RemoveAll (OPL_Collection_t * self)
{
	HxLOG_Trace();
	HxLOG_Assert(self && self->collection);

        if (self == NULL || self->collection == NULL)
                return;

	(void)HLIB_VECTOR_RemoveAll(self->collection);
}

int			OPL_Collection_Length (OPL_Collection_t * self)
{
	HxLOG_Trace();
	HxLOG_Assert(self && self->collection);

        if (self == NULL || self->collection == NULL)
                return 0;

	return (int)HLIB_VECTOR_Length(self->collection);
}

void *		OPL_Collection_ItemAt	(OPL_Collection_t * self, int n)
{
	HxLOG_Trace();
	HxLOG_Assert(self && self->collection);

        if (self == NULL || self->collection == NULL)
                return NULL;

	return HLIB_VECTOR_ItemAt(self->collection, n);
}

int			OPL_Collection_IndexOf	(OPL_Collection_t * self, const void *data)
{
	HxLOG_Trace();
	HxLOG_Assert(self && self->collection);

        if (self == NULL || self->collection == NULL)
                return -1;

	return (int)HLIB_VECTOR_IndexOf(self->collection, data);
}

int			OPL_Collection_Find (OPL_Collection_t *self, void *key, int (* comp) (const void *data, void *key))
{
	int		i, n;
	void *	data;

	HxLOG_Trace();


	n = OPL_Collection_Length(self);
	for (i = 0 ; i < n ; i++)
	{
		data = OPL_Collection_ItemAt(self, i);

		if ((comp && comp(data, key)) || (data == key))
		{
			return i;
		}
	}
	return -1;
}

void **		OPL_Collection_Dump (OPL_Collection_t * self, int *length, void *(* clone)(void *))
{
	int		i, n;
	void	**list;
	void	*data;

	HxLOG_Trace();

	n = OPL_Collection_Length(self);
	if (n == 0)
		return NULL;

	list = (void **)HLIB_STD_MemAlloc(n * sizeof(void *));
	if (list == NULL)
		return NULL;

	if (clone == NULL)
		clone = self->clone;

	for (i = 0 ; i < n ; i++)
	{
		data = OPL_Collection_ItemAt(self, i);

		if (clone)
			list[i] = clone(data);
		else
			list[i] = data;
	}

	if (length)
		*length = n;
	return list;
}

char **		OPL_Collection_DumpString (OPL_Collection_t * self, int *length)
{
	return (char **)OPL_Collection_Dump(self, length, (void *(*)(void *))HLIB_STD_StrDup_CB);
}

char *		OPL_Collection_ToJSON (const char *object_name, OPL_Collection_t *self, char *(* toJSON)(void *))
{
	int		i, n, c;
	char	*json;
	void	*data;
	HINT32	sd;

	HxLOG_Trace();

	sd = HLIB_RWSTREAM_Open(NULL, 0);
	if (!sd)
		return NULL;

	HLIB_RWSTREAM_Print(sd, "{");
	HLIB_RWSTREAM_Print(sd, "\"%s\":", object_name);
	HLIB_RWSTREAM_Print(sd, "[");
	if(self && self->collection)
	{
		n = OPL_Collection_Length(self);
		for (i = 0, c = 0 ; i < n ; i++)
		{
			data = OPL_Collection_ItemAt(self, i);
			if (toJSON)
				json = toJSON(data);
			else if (data)
				json = data;
			else
				json = NULL;

			if (c++ > 0)
				HLIB_RWSTREAM_PutChar(sd, ',');

			if (json == NULL)
				HLIB_RWSTREAM_Print(sd, "null");
			else
				HLIB_RWSTREAM_Print(sd, "%s", json);
		}
	}
	HLIB_RWSTREAM_Print(sd, "]");
	HLIB_RWSTREAM_Print(sd, "}");

	json = (char *)HLIB_RWSTREAM_GetBuf(sd);
	json = HLIB_STD_StrDup(json);

	HLIB_RWSTREAM_Close(sd);

	return json;
}



