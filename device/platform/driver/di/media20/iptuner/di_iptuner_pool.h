/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:       $Workfile:   di_iptuner_pool.h  $
 * Version:         $Revision:   1.0  $
 * Original Author: YoungKi Kim
 * Current Author:  $Author: ykkim3@humaxdigital.com $
 * Date:
 * File Description:	     IP Tuner Pool Header File
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef __DI_IPTUNER_POOL_H__
#define __DI_IPTUNER_POOL_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include "htype.h"
#include "di_iptuner20.h"

/* End Including Headers*/


/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
/* Start Extern variablee */

/* End Extern variable */


/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

typedef enum
{
	DI_IPTUNER_SET_STATUS_SET,
	DI_IPTUNER_SET_STATUS_HEAD,
	DI_IPTUNER_SET_STATUS_TAIL
} DI_IPTUNER_SET_STATUS_e;

typedef struct DI_IPTUNER_NODE_s
{
	DI_IPTUNER_t *pIPTunerHdl;
	struct DI_IPTUNER_NODE_s *prev;
	struct DI_IPTUNER_NODE_s *next;
} DI_IPTUNER_NODE_t;


typedef struct DI_IPTUNER_POOL_s
{
	DI_IPTUNER_NODE_t *head;
	DI_IPTUNER_NODE_t *tail;

	HINT32 size;
	HINT32 capacity;
} DI_IPTUNER_POOL_t;

typedef struct DI_IPTUNER_RESULT_SET_s
{
	HINT32 size;
	DI_IPTUNER_SET_STATUS_e eStatus;
	DI_IPTUNER_POOL_t list;
	DI_IPTUNER_NODE_t *current;
	void (*push)(struct DI_IPTUNER_RESULT_SET_s *this, DI_IPTUNER_t *hdl);
	DI_IPTUNER_t* (*next)(struct DI_IPTUNER_RESULT_SET_s *this);
	DI_IPTUNER_t* (*prev)(struct DI_IPTUNER_RESULT_SET_s *this);
	void (*VK_free)(struct DI_IPTUNER_RESULT_SET_s *this);
} DI_IPTUNER_SET_t;

/* End typedef */

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */

DI_ERR_CODE 	DI_IPTUNER_POOL_Init (void);
DI_ERR_CODE     DI_IPTUNER_POOL_Uninit (void);
DI_IPTUNER_t* 	DI_IPTUNER_POOL_Create (void);
DI_IPTUNER_t* 	DI_IPTUNER_POOL_AllocTuner (void);
DI_ERR_CODE 	DI_IPTUNER_POOL_FreeTuner (DI_IPTUNER_t *iptuner);
DI_ERR_CODE     DI_IPTUNER_POOL_FindByType (DI_IPTUNER_SET_t* resultSet, DI_IPTUNER_ProtocolType_e type);
HBOOL DI_IPTUNER_POOL_IsValidHandle (const DI_IPTUNER_t* pHTuner);
DI_IPTUNER_t*   DI_IPTUNER_POOL_FindById (HUINT32 Id);
DI_IPTUNER_t* DI_IPTUNER_POOL_FindBySaveFileName (HINT8* pName);
DI_IPTUNER_t* DI_IPTUNER_POOL_FindByUrl (const HINT8* pName);
DI_IPTUNER_t* DI_IPTUNER_POOL_FindByVfioHandle (void *pVfio);

HINT32 	DI_IPTUNER_POOL_Size (void);
HINT32 	DI_IPTUNER_POOL_Capacity (void);
/* End global function prototypes */



#ifdef  __cplusplus
}
#endif

#endif /* ! __DI_IPTUNER_POOL_H__ */
