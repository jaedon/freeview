/**************************************************************************************/
/**
 * @file oplscheduler.h
 *
 *
 *
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __OPLRECORDITEM_H__
#define __OPLSCHEDULER_H__

#include	"opltype.h"

#include	"oplscheduledprogramme.h"
#include	"oplrecording.h"

#ifdef __cplusplus
extern "C" {
#endif	//__cplusplus


typedef OPL_BOOL	(* OPL_RecordingItem_EventListener_t) (int state, int Err, OPL_Recording_t *item);

typedef struct
{
	OPL_RecordingItem_EventListener_t	listener;
	HUINT32 							ulMsgId;

} OPL_RecordListHandle_t;



void				OPL_RecordItem_AddEventListener (OPL_RecordingItem_EventListener_t listener);
int					OPL_RecordItem_CountRecordings (void);
int					OPL_RecordItem_CountRecordingsByDiscId (const char *uuid);
OPL_Recording_t *	OPL_RecordItem_GetRecordingAt (int index);
OPL_Recording_t * 	OPL_RecordItem_GetRecordingAtDiscId (int index, const char* discId);
OPL_Recording_t *	OPL_RecordItem_GetRecordingByID (const char *id);
OPL_Collection_t *	OPL_RecordItem_GetRunningRecordings(void);
OPL_Collection_t *	OPL_RecordItem_GetRecordings(void);

#ifdef __cplusplus
};
#endif	//__cplusplus

#endif	//__OPLRECORDITEM_H__

