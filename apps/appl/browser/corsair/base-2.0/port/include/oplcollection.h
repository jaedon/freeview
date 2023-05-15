/**************************************************************************************/
/**
 * @file oplcollection.h
 *
 *
 *
 * @attention   Copyright (c) 2012 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __OPLCOLLECTION_H__
#define __OPLCOLLECTION_H__


#ifdef __cplusplus
extern "C" {
#endif	//__cplusplus


typedef struct
{
	void *	collection;
	void    (* removeNotify)(void *);
	void *	(* clone) (void *);
} OPL_Collection_t;

OPL_Collection_t *	OPL_Collection_New (void);
OPL_Collection_t *	OPL_Collection_NewEx  (void (*removeNotify)(void *), void * (*clone)(void *));
OPL_Collection_t *	OPL_Collection_Clone  (OPL_Collection_t *self, void * (*clone)(void *));
void		OPL_Collection_Delete (OPL_Collection_t * self);
int			OPL_Collection_Add (OPL_Collection_t * self, void *data);
int			OPL_Collection_Insert (OPL_Collection_t * self, int n, void *data);
int			OPL_Collection_Remove (OPL_Collection_t * self, int n);
void		OPL_Collection_RemoveAll (OPL_Collection_t * self);
int			OPL_Collection_Length (OPL_Collection_t * self);
void *		OPL_Collection_ItemAt	(OPL_Collection_t * self, int n);
int			OPL_Collection_IndexOf	(OPL_Collection_t * self, const void *data);
int			OPL_Collection_Find (OPL_Collection_t *self, void *key, int (* comp) (const void *data, void *key));
void **		OPL_Collection_Dump (OPL_Collection_t * self, int *length, void *(* clone)(void *));
char **		OPL_Collection_DumpString (OPL_Collection_t * self, int *length);
char *		OPL_Collection_ToJSON (const char *object_name, OPL_Collection_t *self, char *(* toJSON)(void *));


#ifdef __cplusplus
};
#endif	//__cplusplus

#endif	//__OPLCOLLECTION_H__

