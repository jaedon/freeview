/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_dlna.h  $
 * Version:			$Revision:   1.0  $
 * Original Author:	Dongho Kim
 * Current Author:	$Author: kimdh1@humaxdigital.com $
 * Date:			$Date: 7 15 10:20:48 KST 2009 $
 * File Description:	DLNA PLAY
 * Module: 		
 * Remarks:	
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef	__DI_IPTUNER_DLNA_H__
#define	__DI_IPTUNER_DLNA_H__

#ifdef	__cplusplus
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
DI_ERR_CODE DI_IPTUNER_OpenDLNA (DI_IPTUNER_Index_e nTunerIndex, HINT8 *pUrl, DI_MEDIA_INFO_t *pstMediaInfo);
DI_ERR_CODE DI_IPTUNER_CloseDLNA (DI_IPTUNER_Index_e nTunerIndex);
DI_ERR_CODE DI_IPTUNER_ControlDLNA (DI_IPTUNER_Index_e nTunerIndex,  DI_IPTUNER_CMD_e cmd, HINT32 val);

DI_ERR_CODE DI_IPTUNER_DLNA_InstallDLNAFunction (DLNA_FileFunction_t stDLNAFileFn);
/* End global function prototypes */


#ifdef	__cplusplus
}
#endif

#endif /* !__DI_IPTUNER_DLNA_H__ */


