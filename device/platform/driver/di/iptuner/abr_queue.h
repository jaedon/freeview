/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_mpd.h  $
 * Version:			$Revision:   1.0  $
 * Original Author:	YoungKi Kim
 * Current Author:	$Author: ykkim3@humaxdigital.com $
 * Date:			 
 * File Description:    Adaptive Bitrate Streaming MPD Controller Header File
 * Module:
 * Remarks:	
 */	

/*******************************************************************/ 	
/* Copyright (c) 2011 HUMAX Co., Ltd. 								*/
/* All rights reserved.												*/
/*******************************************************************/

#ifndef __ABR_QUEUE_H__
#define __ABR_QUEUE_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include "htype.h"
#include <sys/time.h> 	 /** for checking delay */
/* End Including Headers*/

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
/* Start Extern variablee */

/* End Extern variable */

/*******************************************************************/
/************************ Constant Definition *************************/
/*******************************************************************/

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

typedef struct ABR_QUEUE_NODE_s
{
	HINT32 nId; /* Segment ID */
	HINT32 nType;
	HINT64 range;
	HINT64 offset; /* end point */ /* start point = offset - range */
	HINT32 header_size;
	HINT64 bufferOffset;
	HUINT64 ullBitrate;
	HBOOL	bComplete;
} ABR_QUEUE_NODE_t;
	
typedef struct ABR_QUEUE_s
{
	HINT32 nCur;
	HINT32 nFront;
	HINT32 nRear;
	HINT32 nSize;
	
	HULONG ulSema;
	ABR_QUEUE_NODE_t *nodeSet;
} ABR_QUEUE_t;

typedef enum {
ABR_OK,
ABR_FAIL,
ABR_INVALID_PARAM,
ABR_NO_RESOURCE,
ABR_NO_ALLOC,
ABR_NO_SEGMENT,
} ABR_ERR;


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */

ABR_ERR 		ABR_QUEUE_GetCurNodeInfo(ABR_QUEUE_t* pQueue, ABR_QUEUE_NODE_t* pNode);
ABR_ERR 		ABR_QUEUE_GetNodeInfoByOffset(ABR_QUEUE_t* pQueue, const HINT64 sz_offset, ABR_QUEUE_NODE_t* pNode);
ABR_ERR 		ABR_QUEUE_GetNodeInfoBySegment(ABR_QUEUE_t* pQueue, HINT32 nId, HINT32 nType,  ABR_QUEUE_NODE_t* pNode);
ABR_ERR 		ABR_QUEUE_GetNodeInfoByBufferOffset(ABR_QUEUE_t* pQueue, const HINT64 sz_BufferOffset, ABR_QUEUE_NODE_t* pNode);
HBOOL 			ABR_QUEUE_GetComplete(ABR_QUEUE_t* pQueue, HINT32 nId);
ABR_ERR			ABR_QUEUE_SetComplete(ABR_QUEUE_t* pQueue, HINT32 nId, HBOOL bComplete);

ABR_ERR 		ABR_QUEUE_PushBack(ABR_QUEUE_t* pQueue, ABR_QUEUE_NODE_t tNode);
ABR_ERR 		ABR_QUEUE_Replace(ABR_QUEUE_t* pQueue, ABR_QUEUE_NODE_t tNode);
ABR_ERR 		ABR_QUEUE_Realign(ABR_QUEUE_t* pQueue, HINT64 sz_Base, ABR_QUEUE_NODE_t tNode);
ABR_ERR 		ABR_QUEUE_Reset(ABR_QUEUE_t* pQueue, HINT64 nBase);

#ifdef  __cplusplus
}
#endif

#endif /* ! __DI_IPTUNER_MPD_H__ */

