/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:       $Workfile:   util_index_parser.h  $
 * Version:         $Revision:   1.0  $
 * Original Author: SungYoung Jeon
 * Current Author:  $Author: junsy@humaxdigital.com $
 * Date:            $Date:  Sept 29 03:08:21 KST 2011 $
 * File Description:	    UTIL Header (URL index parser)
 * 					This file specified youview iptuner to support index-file timeline.
 * 					Please refer to belows.
 * 					
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2011 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef __UTIL_INDEX_PARSER_H__
#define __UTIL_INDEX_PARSER_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include "htype.h"
#include "di_iptuner_common.h"
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
HINT32 UTIL_Index_GetOffsetByPosition (HUINT8 *pIndex, HUINT32 nPos, HUINT64 *pOffset);
HINT32 UTIL_Index_GetDuration (HUINT8 *pIndex , HUINT32 *pPos);
HINT32 UTIL_Index_GetNumOfSample (HUINT8 *pIndex);
HINT32 UTIL_Index_GetFileSize (HUINT8 *pIndex, HUINT64 *pOffset);
/* End global function prototypes */

#ifdef  __cplusplus
}
#endif

#endif /* ! __UTIL_INDEX_PARSER_H__ */
