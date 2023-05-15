/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   mms_buffer.h  $
 * Version:			$Revision:   1.0  $
 * Original Author: $
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date: 2012.07.30
 * File Description:	MMS Buffer controller
 * Module:
 * Remarks:
 */	

/**
 * @ingroup MMS_BUFFER
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
 * @file mms_buffer.h
 */

#ifndef __MMS_BUFFER_H__
#define __MMS_BUFFER_H__

#include "htype.h"

typedef struct
{
    HUINT8 *p_data;    // pointer on data
    int     i_data;     // number of bytes set in p_data

    // private
    int    i_size;     // size of p_data memory allocated
} var_buffer_t;

/*****************************************************************************
 * Macro/Function to create/manipulate buffer
 *****************************************************************************/
int  MMS_BUF_Initwrite ( var_buffer_t *p_buf, int i_default_size );
int  MMS_BUF_Reinitwrite ( var_buffer_t *p_buf, int i_default_size );
void MMS_BUF_Add8 ( var_buffer_t *p_buf, HUINT8  i_byte );
void MMS_BUF_Add16 ( var_buffer_t *p_buf, HUINT16 i_word );
void MMS_BUF_Add32 ( var_buffer_t *p_buf, HUINT32 i_word );
void MMS_BUF_Add64 ( var_buffer_t *p_buf, HUINT64 i_word );
void MMS_BUF_Addmemory ( var_buffer_t *p_buf, void *p_mem, int i_mem );
void MMS_BUF_AddUTF16 ( var_buffer_t *p_buf, char *p_str );
void MMS_BUF_Free ( var_buffer_t *p_buf );


void      MMS_BUF_Initread ( var_buffer_t *p_buf, void *p_data, int i_data );
HUINT8   MMS_BUF_Get8 ( var_buffer_t *p_buf );
HUINT16  MMS_BUF_Get16 ( var_buffer_t *p_buf );
HUINT32  MMS_BUF_Get32 ( var_buffer_t *p_buf );
HUINT64  MMS_BUF_Get64 ( var_buffer_t *p_buf );
int       MMS_BUF_Getmemory ( var_buffer_t *p_buf, void *p_mem, HINT64 i_mem );
int       MMS_BUF_Readempty ( var_buffer_t *p_buf );
void      MMS_BUF_Getguid ( var_buffer_t *p_buf, ASF_GUID_t *p_guid );

#endif /* __MMS_BUFFER_H__ */
