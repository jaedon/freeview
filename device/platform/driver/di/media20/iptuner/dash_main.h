/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   dash_main.h  $
 * Version:			$Revision:   1.0  $
 * Original Author: $
 * Current Author:	$Author: parkjh4@humaxdigital.com $
 * Date:			$Date: 2012.07.30
 * File Description:	Dash main module
 * Module:
 * Remarks:
 */	

/**
 * @ingroup DASH_MAIN
 */

/**
 * @author 
 * @date 30 Jul 2012
 */

/**
 * @note
 * Copyright (C) 2012 Humax Corporation. All Rights Reserved. <br>
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute this software
 * except in compliance with the terms and conditions of any applicable license
 * agreement in writing between Humax Corporation and you.
 */

 /*@{*/

/**
 * @file dash_main.h
 */


#ifndef __DASH_MAIN_H__
#define __DASH_MAIN_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include "htype.h"
#include "util_dllist.h"
#include "dash_mpdparser.h"
#include "dash_controller.h"
#include "adp_buffermgr.h"
#include "adp_heuristics.h"
#include "dash_main.h"
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
/* Start typedef */
typedef ADP_BUFFERMGR_t* DASH_BUFFERMGR_HANDLE_t;
typedef ADP_BUFFERMGR_CONFIG_t DASH_BUFFERMGR_CONFIG_t;
typedef ADP_HEURISTICS_CONFIG_t DASH_HEURISTICS_CONFIG_t;
typedef ADP_HEURISTICS_HANDLE_t DASH_HEURISTICS_HANDLE_t;

/** 
 * DASH handle set
 */
typedef struct DASH_HANDLESET_s
{
	DASH_HEURISTICS_HANDLE_t 	hHeuristics; /**< heuristics handle */
	DASH_BUFFERMGR_HANDLE_t 	hBuffermgr; /**< buffer manager handle */
	DASH_PRECONNECTOR_HANDLE_t	hPreconnector; /**< preconnector handle */
	DASH_CONTROLLER_HANDLE_t	hController; /**< controller handle */
	DASH_MPDPARSER_HANDLE_t		hMPDParser; /**< MPD parser handle */
} DASH_HANDLESET_t;

/** 
 * DASH master handle
 */
typedef struct DASH_Essential_s
{
	DASH_HANDLESET_t handle;		/**< handle set */
	DASH_USER_EVENT_e eUserAction;	/**< occured user event */
	DASH_USER_EVENT_e eUserActionForRead; /**< occured user event(especilly seek) for blocking waiting IO in VFIO read */
	HINT64  SeekPos;				/**< occured seek position */
	HULONG	unPreconnectorTaskId;	/**< Preconnector Task ID */
	HULONG 	unSchedulerTaskId;		/**< Scheduler Task ID */
	HULONG 	unBufferingTaskId;		/**< Buffer manager Task ID */
	HULONG 	unMPDTaskId;			/**< MPD Task ID */
	HBOOL	bPreconnectorTaskInit;	/**< Preconnector task init check */
	HBOOL	bSchedulerTaskInit;		/**< Scheduler task init check */
	HBOOL	bBufferingTaskInit;		/**< Buffer manager task init check */
	HBOOL	bMPDTaskInit;			/**< MPD task init init check */
	HULONG	ulSema;					/**< Semaphore */
	HBOOL	bDump;	/* flag for dump */
} DASH_ESSENTIAL_t;
typedef DASH_ESSENTIAL_t*	 DASH_MASTER_HANDLE_t;

/*******************************************************
*          Maindata APIs
*******************************************************/

/*! 
 *	\brief Create DASH master handle
 *	\return DASH master handle
 *	\see 
 */
DASH_MASTER_HANDLE_t	DASH_MAIN_Create(void);

/*! 
 *	\brief Destroy DASH master handle
 *	\param pDASH : DASH master handle
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e		DASH_MAIN_Destroy(DASH_MASTER_HANDLE_t pDASH);

/*! 
 *	\brief Set heuristics handle to master handle
 *	\param pDASH : DASH master handle
 *	\param pHeuristics : Heuristics handle
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e		DASH_MAIN_SetHeuristicsHandle(DASH_MASTER_HANDLE_t pDASH, DASH_HEURISTICS_HANDLE_t pHeuristics);

/*! 
 *	\brief Set buffer manager handle to master handle
 *	\param pDASH : DASH master handle
 *	\param pBufferMgr : Buffer manager handle
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e		DASH_MAIN_SetBufferMgrHandle(DASH_MASTER_HANDLE_t pDASH, DASH_BUFFERMGR_HANDLE_t pBufferMgr);

/*! 
 *	\brief Set controller handle to master handle
 *	\param pDASH : DASH master handle
 *	\param pController : DASH controller handle
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e		DASH_MAIN_SetControllerHandle(DASH_MASTER_HANDLE_t pDASH, DASH_CONTROLLER_HANDLE_t pController);

/*! 
 *	\brief Set preconnector handle to master handle
 *	\param pDASH : DASH master handle
 *	\param pPreconnector : preconnector handle
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e		DASH_MAIN_SetPreconnectorHandle(DASH_MASTER_HANDLE_t pDASH, DASH_PRECONNECTOR_HANDLE_t pPreconnector);

/*! 
 *	\brief Set MPD parser handle to master handle
 *	\param pDASH : DASH master handle
 *	\param pMPD : MPD parser handle
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e		DASH_MAIN_SetMPDParserHandle(DASH_MASTER_HANDLE_t pDASH, DASH_MPDPARSER_HANDLE_t pMPD);

/*! 
 *	\brief Set user event flag
 *	\param pDASH : DASH master handle
 *	\param eUserEvent : User event type
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e		DASH_MAIN_SetOccuredUserEvent(DASH_MASTER_HANDLE_t pDASH, DASH_USER_EVENT_e eUserEvent);

/*! 
 *	\brief Clear user event flag that is for blocking waiting IO in VFIO read
 *	\param pDASH : DASH master handle
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e 		DASH_MAIN_ClearUserEventForRead(DASH_MASTER_HANDLE_t pDASH);

/*! 
 *	\brief Clear user event flag
 *	\param pDASH : DASH master handle
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e 		DASH_MAIN_ClearUserEvent(DASH_MASTER_HANDLE_t pDASH);

/*! 
 *	\brief Get user event flag
 *	\param pDASH : DASH master handle
 *	\return User event
 *	\see 
 */
DASH_USER_EVENT_e	DASH_MAIN_GetOccuredUserEvent(DASH_MASTER_HANDLE_t pDASH);

/*! 
 *	\brief Get user event flag that is for blocking waiting IO in VFIO read
 *	\param pDASH : DASH master handle
 *	\return User event
 *	\see 
 */
DASH_USER_EVENT_e 	DASH_MAIN_GetOccuredUserEventForRead(DASH_MASTER_HANDLE_t pDASH);

/*! 
 *	\brief Get seek position (in ms)
 *	\param pDASH : DASH master handle
 *	\return seek position
 *	\see 
 */
HINT64				DASH_MAIN_GetSeekPosition(DASH_MASTER_HANDLE_t pDASH);

/*! 
 *	\brief Set seek position (in ms)
 *	\param pDASH : DASH master handle
 *	\param ullPosition : Position
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e 		DASH_MAIN_SetSeekPosition(DASH_MASTER_HANDLE_t pDASH, HINT64 ullPosition);

/*! 
 *	\brief Get handle for heuristics
 *	\param pDASH : DASH master handle
 *	\param pHeuristics : Heuristics handle(return)
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e		DASH_MAIN_GetHeuristicsHandle(DASH_MASTER_HANDLE_t pDASH, DASH_HEURISTICS_HANDLE_t* pHeuristics);

/*! 
 *	\brief Get handle for buffer manager
 *	\param pDASH : DASH master handle
 *	\param pBufferMgr : buffer manager handle(return)
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e		DASH_MAIN_GetBufferMgrHandle(DASH_MASTER_HANDLE_t pDASH, DASH_BUFFERMGR_HANDLE_t* pBufferMgr);

/*! 
 *	\brief Get handle for controller
 *	\param pDASH : DASH master handle
 *	\param pController : controller handle (return)
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e		DASH_MAIN_GetControllerHandle(DASH_MASTER_HANDLE_t pDASH, DASH_CONTROLLER_HANDLE_t* pController);

/*! 
 *	\brief Get handle for preconnector
 *	\param pDASH : DASH master handle
 *	\param pPreconnector : preconnector handle (return)
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e		DASH_MAIN_GetPreconnectorHandle(DASH_MASTER_HANDLE_t pDASH, DASH_PRECONNECTOR_HANDLE_t* pPreconnector);

/*! 
 *	\brief Get handle for MPD parser
 *	\param pDASH : DASH master handle
 *	\param pPreconnector : MPD parser handle (return)
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e		DASH_MAIN_GetMPDParserHandle(DASH_MASTER_HANDLE_t pDASH, DASH_MPDPARSER_HANDLE_t* pMPD);


#endif
