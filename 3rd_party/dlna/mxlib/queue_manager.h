/*******************************************************************************
 * HUMAX R&D Lab., HUMAX INC., SEOUL, KOREA
 * COPYRIGHT(c) 2011 by HUMAX Inc.
 * 
 * Author : Kang, Yonghee.
 *
 * All rights reserved. No part of this work may be reproduced, stored in a
 * retrieval system, or transmitted by any means without prior written
 * permission of HUMAX Inc.
 ******************************************************************************/
#ifndef QUEUEMANAGER_H
#define QUEUEMANAGER_H


#ifdef __cplusplus
extern "C" {
#endif

#if defined(WIN32)
#include "stdafx.h"
#endif


#include "Transcoder_data_struct.h"
#include "transcoderControlConfig.h"
	

#include "mxkernel.h"
#include "transcoder_util.h"

#define TRANSCODER_QUEUE_MAX_ITEM		1000

#if !defined(IDLE_PRIORITY_CLASS)
//todo : define priority policy!
#define IDLE_PRIORITY_CLASS 		0
#define NORMAL_PRIORITY_CLSSS		0
#endif

#ifdef WIN32
#if defined(_DEBUG)
#define TRANSCODER_QUEUE_PRIORITY		NORMAL_PRIORITY_CLASS  
#else
#define TRANSCODER_QUEUE_PRIORITY		IDLE_PRIORITY_CLASS 
#endif
#else
#define TRANSCODER_QUEUE_PRIORITY		0
#endif

/*******************************************************************************/
typedef struct _transcoder_queue_element_tag
{
//	char*		woon_id;

	char*		device_id;
	char*		object_id;

	void*		user_data;
	char*		InFullPathFileName;
	char*		FullPathSubtitleName;//only for Video
	char*		OutputPath;//this is from caller, can be null.
	char*		OutputNameOnly;//this is from caller, can be null.
	char*		OutFullPathFileName;// generated inside and return to caller.
	int			iProfile;
	int         nIsMetadata;
	TRANSCODER_METADATA         sMetadata;
	struct		_transcoder_queue_element_tag	*pNext;
// --------------------------------------------------------//
} TRANSCODER_QUEUE_ELEMENT, *PTRANSCODER_QUEUE_ELEMENT;
// --------------------------------------------------------//

#if defined(WIN32)
typedef int (*TRANSCODER_QUEUE_OPERATOR)(PTRANSCODER_QUEUE_ELEMENT queue_element, PHANDLE phOperatorProcess, PBOOL pbTerminating);
#else
typedef int (*TRANSCODER_QUEUE_OPERATOR)(PTRANSCODER_QUEUE_ELEMENT queue_element, PHANDLE phOperatorProcess, PBOOL pbTerminating);
#endif

typedef struct _queue_list_tag
{
	TRANSCODER_TYPE				type;
	char*						DefaultDirectory;
	char*						Extension;
	int							no_of_item;
	TRANSCODER_QUEUE_ELEMENT	*pFirst;
	TRANSCODER_QUEUE_OPERATOR	fnOperator;//actual conversion operator. fill OutFullPathFileName
	HANDLE						hOperatorThread;
	HANDLE						hOperatorProcess;
	BOOL						terminating;
	BOOL						removeOperating;
	TRANSCODER_FINISH_NOTIFY_CALLBACK	notify_finished;
	TRANSCODER_FINISH_EMPTY_CALLBACK	empty_finished;
	TRANSCODER_CRITICALSECTION	CS;
// --------------------------------------------------------//
} TRANSCODER_QUEUE_LIST, *PTRANSCODER_QUEUE_LIST;
// --------------------------------------------------------//

/*******************************************************************************/
extern TRANSCODER_QUEUE_LIST		  g_ConversionQueue;
extern TRANSCODER_QUEUE_LIST		  g_ThumbnailQueue;
//extern int	g_Thumbnail_width;
//extern int	g_Thumbnail_height;

/*******************************************************************************/
int initQueue(PTRANSCODER_QUEUE_LIST transcoder_queue,
		    char*							DefaultDirectory,
			char*							Extension,
		    TRANSCODER_TYPE					type,
			TRANSCODER_QUEUE_OPERATOR		fnOperator,
			TRANSCODER_FINISH_NOTIFY_CALLBACK	notify_finished,
			TRANSCODER_FINISH_EMPTY_CALLBACK	empty_finished
			);
int addQueue(PTRANSCODER_QUEUE_LIST transcoder_queue, PTRANSCODER_QUEUE_ELEMENT transcoder_item);
int removeQueue(PTRANSCODER_QUEUE_LIST transcoder_queue, const char* woon_id, const char* FullPathFileName);
int terminateQueue(PTRANSCODER_QUEUE_LIST transcoder_queue);

char *getTargetFullPathName(char* InFullPathFileName, char* pOutputFileNameOnly, char* outpath, int iProfile, char* ext);


#ifdef __cplusplus
}
#endif

#endif 
