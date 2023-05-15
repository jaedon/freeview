/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_multicast.h  $
 * Version:			$Revision:   1.0  $
 * Original Author: $
 * Current Author:	$Author: jlee@humaxdigital.com $
 * Date:			$Date: 2012.07.30
 * File Description:	DI Stream (MULTICAST) Header
 * Module:
 * Remarks:
 */	

/**
 * @ingroup MULTICAST_IPTUNER_CONTROLLER
 */

/**
 * @author Jin Lee(jlee@humaxdigital.com)
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
 * @file di_iptuner_multicast.h
 */

#ifndef __DI_IPTUNER_MULTICAST_H__
#define __DI_IPTUNER_MULTICAST_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include "htype.h"
#include "di_err.h"
#include "di_media20.h"
#include "di_iptuner20.h"
#include "di_iptuner_config.h"
/* End Including Headers*/


/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
/* Start Extern variablee */

/* End Extern variable */


/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
/* Start Macro definition */

/* End Macro definition */


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

/* End typedef */


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */

/*! 
 *	\brief Open ip live channel(internally IGMP join)
 *	\param pHTuner: DI_IPTUNER_t
 *	\return DI_ERR_CODE
 *	\see 
 */
DI_ERR_CODE DI_IPTUNER_MulticastOpen (DI_IPTUNER_t *pHTuner);

/*! 
 *	\brief Close ip live channel(internally IGMP leave)
 *	\param pHTuner: DI_IPTUNER_t
 *	\return DI_ERR_CODE
 *	\see 
 */
DI_ERR_CODE DI_IPTUNER_MulticastClose (DI_IPTUNER_t *pHTuner);
/* End global function prototypes */


#ifdef  __cplusplus
}
#endif

#endif /* ! __DI_IPTUNER_MULTICAST_H__ */


