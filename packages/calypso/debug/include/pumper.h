/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   pumper.h  		$
 * Version:		$Revision:   1.0  			$
 * Original Author:	Dylan Song				$
 * Current Author:	$Author:   Dylan Song  			$
 * Date:		$Date:   16 JUNE 2008 17:04:02  	$
 * File Description:																			         
 * Module: 																							           
 * Remarks:																											
 */


/**
 * @ingroup Pumper
 */

/**
 * @author Dylan Song(dssong@humaxdigital.com), Hyunuk Yang (huyang@humaxdigital.com)
 * @date 22 October 2012
 */

/**
 * @note
 * Copyright (C) 2011 Humax Corporation. All Rights Reserved. <br>
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute this software
 * except in compliance with the terms and conditions of any applicable license
 * agreement in writing between Humax Corporation and you.
 */

/*@{*/

/**
 * @file pumper.h
 */

/*******************************************************************/
/* Copyright (c) 2007 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef __PUMPER_H__
#define __PUMPER_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include <pthread.h>

/* End Including Headers */
/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
/* Start Extern variablee */

/* End Extern variable */
/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
/* Start Macro definition */
//#define PUMPER_CONTEXT_MAX_NUM                    (3)

//#define PUMPER_JOB_SHORT_DELAY_USEC              (50*1000)  /* usleep() time while running feeder task */
#define PUMPER_JOB_SHORT_DELAY_USEC              (10*1000)  /* usleep() time while running feeder task */
#define PUMPER_JOB_DEFAULT_DELAY_USEC            (100*1000) /* usleep() time while running feeder task */
#define PUMPER_JOB_DEFAULT_WAIT_COUNT            (500)      /* 20(msec)*500 */
/* End Macro definition */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

/**
* Pumping Type
*/
typedef enum
{
	PUMPER_PUMPING_UNKOWN_TYPE = 0,	/**< UNKNOWN */
	PUMPER_PUMPING_TYPE_TS,			/**< TS Type */
	PUMPER_PUMPING_TYPE_TS192,		/**< TS 192Packet Type */
	PUMPER_PUMPING_TYPE_PES,		/**< PES Type */
	PUMPER_PUMPING_TYPE_PES_SMP,	/**< PES Type with SMP */
	PUMPER_PUMPING_TYPE_ES,			/**< ES Type */
	PUMPER_PUMPING_TYPE_NONE		/**< NONE Type */
} PUMPER_PUMPING_Type_t;

typedef enum
{   	
	ePUMPER_JOB_STATUS_CANCELED=0, /* Job canceled/terminated                                  */
	ePUMPER_JOB_STATUS_STOPPING,   /* Prepare halt; maybe stop() or release()                  */
	ePUMPER_JOB_STATUS_READY,      /* Meaning of ready and out_of_pumping after pumper start() */
	ePUMPER_JOB_STATUS_RUNNING,    /* While in pumping (by playpump), so don't touch me        */
	ePUMPER_JOB_STATUS_WAITING,    /* Event waiting status like BKNI_Wait(), so don't touch me */	
} PUMPER_JobStatus_e;     

/**
* Pumper Context
*/
typedef struct _PUMPER_Context_t
{
	int										nResourceId;
	void                     				*pPlaypumpHandle;	/**< NEXUS Playpump Handle */
	void                       				*tEvent;				/**< BKNI Event Handle  */
	PUMPER_PUMPING_Type_t                   tPumpingType;		/**< Pumping Type  */
	pthread_mutex_t                         tMutex;
	PUMPER_JobStatus_e                      eJobStatus;
	void									*pSocBuffer;		/**< SOC_BUFFERMGR_HANDLE_t */
} PUMPER_Context_t;
/* End typedef */
/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */

int	PUMPER_Init (void);
void PUMPER_Uninit (void);

PUMPER_Context_t *PUMPER_Create (unsigned int unIndex, PUMPER_PUMPING_Type_t tPumpingType, void * pSocBuffer /*SOC_BUFFERMGR_HANDLE_t*/ /* If needed */);
int PUMPER_Destroy (PUMPER_Context_t *tPumperContext);

unsigned int PUMPER_Pumping (PUMPER_Context_t *tPumperContext, unsigned char *pBuf, unsigned int unInjSize);
int PUMPER_TEST_PCM_Pumping (PUMPER_Context_t *tPumperContext, unsigned char *pBuf, unsigned int unInjSize);

int PUMPER_Flush (PUMPER_Context_t *tPumperContext, int bStopPumper);
int PUMPER_Start (PUMPER_Context_t *tPumperContext);
PUMPER_Context_t *PUMPER_GetContext (unsigned int unIndex);

int PUMPER_SetResourceMap(void* pMap);
int PUMPER_UnsetResourceMap ();


int PUMPER_ConvertToDiCodec (unsigned int FFCodecId);
#ifdef  __cplusplus
}
#endif

/* End global function prototypes */
#endif /* __PUMPER_H__ */

/*@}*/
