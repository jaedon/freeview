/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:       $Workfile:   util_url.h  $
 * Version:         $Revision:   1.0  $
 * Original Author: SungYoung Jeon
 * Current Author:  $Author: junsy@humaxdigital.com $
 * Date:            $Date:  May 20 03:08:21 KST 2009 $
 * File Description:	     UTIL Header (URL Parser)
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef __UTIL_URL_H__
#define __UTIL_URL_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include "htype.h"
#include "transport_io.h"
#include "di_iptuner_common.h"

#include <unistd.h>
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
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
typedef struct
{
    char *psz_protocol;		/** ex> http rtsp mms */
    char *psz_username;		/** user name string */
    char *psz_password;		/** password string */
    char *psz_host;			/** host address or URL*/
    int  i_port;			/** port number */

    char *psz_path;			/** path ( except of host ) */
    char *psz_option;
    char *psz_buffer; /* to be freed */
    char *psz_cookie; /* to be freed */
} URL_t;

/* End typedef */


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */
char *UTIL_EnsureUTF8 ( char *str );
char *UTIL_AllocUniqueURL (unsigned int nTunerId, const char* pUrl);
char* 	UTIL_GetOriginalURL (const char *pUniqueUrl);
void UTIL_UrlClean ( URL_t *url );
void UTIL_UrlParse ( URL_t *url, const char *psz_url, char option );
char* 	UTIL_UrlAllocWithNewName (const char *pOrg, const char *pNewName );
/* End global function prototypes */

#ifdef  __cplusplus
}
#endif

#endif /* ! __UTIL_URL_H__ */
