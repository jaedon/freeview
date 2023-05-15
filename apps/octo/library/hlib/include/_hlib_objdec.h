/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  hxobjdec.h
	@brief

	Description:  									\n
	Module: RPC module for OCTO 20					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

#ifndef __HLIB_OBJDEC_H__
#define __HLIB_OBJDEC_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	HxOBJDEC_TAG_PAIR,
	HxOBJDEC_TAG_INTEGER,
	HxOBJDEC_TAG_FLOAT,
	HxOBJDEC_TAG_STRING,
	HxOBJDEC_TAG_BOOL,
	HxOBJDEC_TAG_BINARY
} HxOBJDEC_TAG_e;

typedef struct
{
	HCHAR		*tag;
	HUINT32		ulValue;
} HxOBJDEC_TAGLIST_t;

typedef	struct
{
	HxOBJDEC_TAG_e		tag_type;
	HxOBJDEC_TAGLIST_t	taglist[];
} HxOBJDEC_TAG2ENUM_t;

typedef	struct
{
	HCHAR				*property;
	HxOBJDEC_TAG2ENUM_t	*tagtable;
	HUINT32				offset;
	HUINT32				sizeofdata;
} HxOBJDEC_PROP2ENUM_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
extern HxOBJDEC_TAG2ENUM_t		HLIB_OBJDEC_BOOL_Table;
extern HxOBJDEC_TAG2ENUM_t		HLIB_OBJDEC_INT_Table;
extern HxOBJDEC_TAG2ENUM_t		HLIB_OBJDEC_STRING_Table;
extern HxOBJDEC_TAG2ENUM_t		HLIB_OBJDEC_FLOAT_Table;
extern HxOBJDEC_TAG2ENUM_t		HLIB_OBJDEC_BINARY_Table;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
HCHAR*	HLIB_OBJDEC_EncodeEnum(HxOBJDEC_TAGLIST_t *taglist, HUINT32 data);
void	HLIB_OBJDEC_DecodeEnum(HxOBJDEC_TAG2ENUM_t *table, HxJSON_t object, HUINT32 write_addr, HUINT32 size);
void	HLIB_OBJDEC_DecodeObject(void *pstInfo, HxJSON_t root, HxOBJDEC_PROP2ENUM_t *pstPropTable);
HERROR	HLIB_OBJDEC_EncodeProperty(void *pstInfo, HxOBJDEC_PROP2ENUM_t *pstPropTable, HINT32 stream);
HCHAR*	HLIB_OBJDEC_EncodeObjectIncludeObject(void *pstInfo, HCHAR *object_name, HxOBJDEC_PROP2ENUM_t *pstPropTable, ...);
HCHAR*	HLIB_OBJDEC_EncodeObject(void *pstInfo, HCHAR *object_name, HxOBJDEC_PROP2ENUM_t *pstPropTable);

#endif
/* end of file */

