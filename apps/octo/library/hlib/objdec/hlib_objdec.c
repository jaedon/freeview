/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  hxobjdec.c
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <htype.h>

#include <_hlib_stdlib.h>
#include <_hlib_math.h>
#include <_hlib_cbuf.h>
#include <_hlib_json.h>
#include <_hlib_rwstream.h>
#include <_hlib_objdec.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	HxOBJDEC_ENCODE_LEN		2048

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
HxOBJDEC_TAG2ENUM_t		HLIB_OBJDEC_BOOL_Table =
{
	HxOBJDEC_TAG_BOOL,
	{
		{	NULL,		0	}
	}
};

HxOBJDEC_TAG2ENUM_t		HLIB_OBJDEC_INT_Table =
{
	HxOBJDEC_TAG_INTEGER,
	{
		{	NULL,		0	}
	}
};

HxOBJDEC_TAG2ENUM_t		HLIB_OBJDEC_STRING_Table =
{
	HxOBJDEC_TAG_STRING,
	{
		{	NULL,		0	}
	}
};

HxOBJDEC_TAG2ENUM_t		HLIB_OBJDEC_FLOAT_Table =
{
	HxOBJDEC_TAG_FLOAT,
	{
		{	NULL,		0	}
	}
};

HxOBJDEC_TAG2ENUM_t		HLIB_OBJDEC_BINARY_Table =
{
	HxOBJDEC_TAG_BINARY,
	{
		{	NULL,		0	}
	}
};

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static HxCBUF_t		s_nCBuf = NULL;


/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
static	void	hlib_objdec_EncodeProperty(HINT32 stream, HxOBJDEC_PROP2ENUM_t *pstPropTable, HUINT32 base_addr)
{
	HBOOL		bFirst = TRUE;
	HBOOL		bBufAlloc = FALSE,bEncodeAlloc = FALSE;
	HCHAR		*buf, *temp,*encodebuf;
	HUINT32		data;
	HUINT32		read_addr,Len;

	if(pstPropTable == NULL)
		return;

	if (s_nCBuf == NULL)
		s_nCBuf = HLIB_CBUF_New(8192);

	temp = HLIB_CBUF_GetBuffer(s_nCBuf, HxOBJDEC_ENCODE_LEN);
	if(temp == NULL)
		return;

	while (pstPropTable->property)
	{
		*temp = 0;

		read_addr = (base_addr + pstPropTable->offset);
		data = *((HUINT32*)read_addr);
		if (pstPropTable->tagtable)
		{
			switch (pstPropTable->tagtable->tag_type)
			{
				case	HxOBJDEC_TAG_PAIR:
					buf = HLIB_OBJDEC_EncodeEnum(pstPropTable->tagtable->taglist, data);
					if (buf)
						HxSTD_snprintf(temp, HxOBJDEC_ENCODE_LEN, "\"%s\":\"%s\"", pstPropTable->property, buf);
					break;
				case	HxOBJDEC_TAG_INTEGER:
					HxSTD_snprintf(temp, HxOBJDEC_ENCODE_LEN, "\"%s\":%d", pstPropTable->property, data);
					break;
				case	HxOBJDEC_TAG_FLOAT:
					HxSTD_snprintf(temp, HxOBJDEC_ENCODE_LEN, "\"%s\":%f", pstPropTable->property, *(float *)read_addr);
					break;
				case	HxOBJDEC_TAG_STRING:
                        Len = HLIB_STD_URLStrLen( (HCHAR*)read_addr) + 1;
                        encodebuf = HLIB_CBUF_GetBuffer(s_nCBuf, Len);
                        HLIB_STD_URLEncode(encodebuf, (HCHAR*)read_addr,Len);
                        HxSTD_snprintf(temp, HxOBJDEC_ENCODE_LEN, "\"%s\":\"%s\"", pstPropTable->property, encodebuf);
                        if(bEncodeAlloc)
                        {
                            bEncodeAlloc = FALSE;
                            HLIB_MEM_Free(encodebuf);
                        }
                      break;
				case	HxOBJDEC_TAG_BOOL:
					HxSTD_snprintf(temp, HxOBJDEC_ENCODE_LEN, "\"%s\":%s", pstPropTable->property, (data ? "true" : "false"));
					break;
				case	HxOBJDEC_TAG_BINARY:
					buf = HLIB_CBUF_GetBuffer(s_nCBuf, HxOBJDEC_ENCODE_LEN);
					if (HLIB_MATH_EncodeBase64Ex(buf, HxOBJDEC_ENCODE_LEN, (void *)read_addr, pstPropTable->sizeofdata))
					{
						HxSTD_snprintf(temp, HxOBJDEC_ENCODE_LEN, "\"%s\":\"%s\"", pstPropTable->property, buf);
					}

					if(bBufAlloc)
					{
						bBufAlloc = FALSE;
						HLIB_MEM_Free(buf);
					}
					break;
				default:
					break;
			}
		}

		if (HxSTD_StrLen(temp) > 0)
		{
			if (bFirst)
			{
				bFirst = FALSE;
				HLIB_RWSTREAM_Print(stream, "%s", temp);
    		} else
			{
				HLIB_RWSTREAM_Print(stream, ",%s", temp);
    		}
		}

		pstPropTable++;
	}
}

/**************************************************************
	public functions
 **************************************************************/
HCHAR*	HLIB_OBJDEC_EncodeEnum(HxOBJDEC_TAGLIST_t *taglist, HUINT32 data)
{
	while (taglist->tag)
	{
		if (taglist->ulValue == data)
			return taglist->tag;

		taglist++;
	}

	return NULL;
}

void	HLIB_OBJDEC_DecodeEnum(HxOBJDEC_TAG2ENUM_t *table, HxJSON_t object, HUINT32 write_addr, HUINT32 size)
{
	HxOBJDEC_TAGLIST_t	*taglist = table->taglist;

	if (size == 0)
		size = 4;

	switch (table->tag_type)
	{
		case	HxOBJDEC_TAG_PAIR:
			if (HxJSON_IsString(object))
			{
				const HCHAR	*value = HxJSON_String_Get(object);
				while (taglist->tag)
				{
					if (HLIB_STD_StrCaseCmp(taglist->tag, value) == 0)
					{
						HxSTD_MemCopy((HUINT32*)write_addr, &taglist->ulValue, size);
						return;
					}

					taglist++;
				}
			}
			break;
		case	HxOBJDEC_TAG_INTEGER:
			if (HxJSON_IsInteger(object))
			{
				HUINT32	integer = HxJSON_Integer_Get(object);
				HxSTD_MemCopy((HUINT32*)write_addr, &integer, size);
			}
			break;
		case	HxOBJDEC_TAG_FLOAT:
			if (HxJSON_IsReal(object))
			{
				float	value = HxJSON_Real_Get(object);
				HxSTD_MemCopy((HUINT32*)write_addr, &value, size);
			}
			break;
		case	HxOBJDEC_TAG_STRING:
			if (HxJSON_IsString(object))
			{
				const HCHAR	*value = HxJSON_String_Get(object);
				HxSTD_StrNCpy((HCHAR*)write_addr, value, size);
			}
			break;
		case	HxOBJDEC_TAG_BOOL:
			if (HxJSON_IsBoolean(object))
			{
				HBOOL	value = HxJSON_Boolean_Get(object);
				HxSTD_MemCopy((HBOOL*)write_addr, &value, size);
			}
			break;
		case	HxOBJDEC_TAG_BINARY:
			if (HxJSON_IsString(object))
			{
				const HCHAR	*value = HxJSON_String_Get(object);
				HLIB_MATH_DecodeBase64Ex((void *)write_addr, size, value, strlen(value));
			}
			break;
		default:
			break;
	}
}

void	HLIB_OBJDEC_DecodeObject(void *pstInfo, HxJSON_t root, HxOBJDEC_PROP2ENUM_t *pstPropTable)
{
	HxJSON_t	object;

	HUINT32		write_addr, base_addr = (HUINT32)pstInfo;

	while (pstPropTable->property)
	{
		write_addr = (base_addr + pstPropTable->offset);
		object = HxJSON_Object_Get(root, pstPropTable->property);
		if (object)
		{
			if (pstPropTable->tagtable)
				HLIB_OBJDEC_DecodeEnum(pstPropTable->tagtable, object, write_addr, pstPropTable->sizeofdata);
		}

		pstPropTable++;
	}
}


HERROR	HLIB_OBJDEC_EncodeProperty(void *pstInfo, HxOBJDEC_PROP2ENUM_t *pstPropTable, HINT32 stream)
{
	HUINT32		base_addr = (HUINT32)pstInfo;

	if (0 == base_addr || NULL == pstPropTable)
	{
		return ERR_FAIL;
	}

	hlib_objdec_EncodeProperty(stream, pstPropTable, base_addr);

	return ERR_OK;
}


HCHAR*	HLIB_OBJDEC_EncodeObjectIncludeObject(void *pstInfo, HCHAR *object_name, HxOBJDEC_PROP2ENUM_t *pstPropTable, ...)
{
	HINT32		stream;
	HUINT32		base_addr = (HUINT32)pstInfo;

	va_list		ap;
	HCHAR		*object_container;

	stream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);

	if (object_name)
		HLIB_RWSTREAM_Print(stream, "\"%s\":{", object_name);
	else
		HLIB_RWSTREAM_Print(stream, "{");

	hlib_objdec_EncodeProperty(stream, pstPropTable, base_addr);

	//
	va_start(ap, pstPropTable);
	object_container = va_arg(ap, HCHAR *);
	while (object_container)
	{
		HLIB_RWSTREAM_Print(stream, ",%s", object_container);
		object_container = va_arg(ap, HCHAR *);
	}
	va_end(ap);
	//

	HLIB_RWSTREAM_Print(stream, "}");

	return (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(stream);
}

HCHAR*	HLIB_OBJDEC_EncodeObject(void *pstInfo, HCHAR *object_name, HxOBJDEC_PROP2ENUM_t *pstPropTable)
{
	HINT32		stream;
	HUINT32		base_addr = (HUINT32)pstInfo;

	stream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);

	if (object_name)
		HLIB_RWSTREAM_Print(stream, "\"%s\":{", object_name);
	else
		HLIB_RWSTREAM_Print(stream, "{");

	hlib_objdec_EncodeProperty(stream, pstPropTable, base_addr);

	HLIB_RWSTREAM_Print(stream, "}");

	return (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(stream);
}

