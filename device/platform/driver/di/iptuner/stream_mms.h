/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:       $Workfile:   stream_mms.h  $
 * Version:         $Revision:   1.0  $
 * Original Author: SungYoung Jeon
 * Current Author:  $Author: junsy@humaxdigital.com $
 * Date:            $Date:  Apr 17 03:08:21 KST 2009 $
 * File Description:	     MMS Stream Controller (MMSTU/MMSH)
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef __STREAM_MMS_H__
#define __STREAM_MMS_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include <unistd.h>
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include "htype.h"
#include "transport_io.h"
#include "di_iptuner_common.h"
#include "util_url.h"
#include "stream_global.h"
#include "di_iptuner.h"
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
int STREAM_MMS_Open ( STREAM_ACCESS_t *pAccess );
void STREAM_MMS_Close ( STREAM_ACCESS_t *pAccess );
ssize_t STREAM_MMS_Read ( STREAM_ACCESS_t *pAccess, HUINT8 *p_buffer, size_t i_len );
int STREAM_MMS_Seek ( STREAM_ACCESS_t *pAccess, HINT64 i_pos );
int STREAM_MMS_Control ( STREAM_ACCESS_t *pAccess, DI_IPTUNER_CMD_e cmd, HUINT64 val ,HUINT64 val2);

/* End global function prototypes */

#ifdef  __cplusplus
}
#endif

#endif /* ! __STREAM_MMS_H__ */
