/** **********************************************************************************************************
	@file 		mw_mp_utils.c

	@date		2012/01/03
	@author		IPM-SW1T (tipmsw1@humaxdigital.com)
	@breif

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. ("Humax") and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*********************************************************************************************************** */


#include "pal_media.h"
#include "pal_video.h"
#include "stb_params.h"

#include "vkernel.h"
#include <string.h>

#include "_svc_mp_utils.h"

#if defined(CONFIG_PACKAGE_LIBPNG)
#include <png.h>
#endif

STATIC PostMsgToMgrCb_t		s_fnSvcMpPostMsgToMgr = NULL;

/*---------------------------------------------------------------------------------
 * Interfaces
 *---------------------------------------------------------------------------------*/
HINT32 media_player_utils_PostMsgToMgr(HINT32 nMsg, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	if (s_fnSvcMpPostMsgToMgr)
	{
		return (* s_fnSvcMpPostMsgToMgr)(nMsg, hAction, nParam1, nParam2, nParam3);
	}

	return ERR_OK;
}

void media_player_utils_RegisterPostMsgCallback(PostMsgToMgrCb_t fnPostMsgToMgr)
{
	if(fnPostMsgToMgr == NULL)
	{
		HxLOG_Critical("Critical Eror!!!\n");
	}

	s_fnSvcMpPostMsgToMgr = fnPostMsgToMgr;

}

HERROR  media_player_utils_GetFileNameWithoutExt(HUINT8 *pucPathName, HUINT8 *pucPathFileName)
{
	HUINT32             i, ulLen = 0;

	if(pucPathName == NULL || pucPathFileName == NULL)
	{
		return ERR_FAIL;
	}

	ulLen = strlen(pucPathName);
	if(ulLen > 3)
	{
		for(i = ulLen - 1; i > 0; i--)
		{
			if(pucPathName[i] == '.')
			{
				strncpy(pucPathFileName, pucPathName, i);
				pucPathFileName[i] = '\0';
				return ERR_OK;
			}
		}
	}

	return ERR_FAIL;
}

HERROR media_player_utils_make_png(void *pFrameBuffer, const HINT8 *pucPathFileName)
{
#if defined(CONFIG_PACKAGE_LIBPNG)
	FILE *fp = fopen(pucPathFileName, "wb");
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	size_t x, y;
	png_uint_32 bytes_per_row;
	png_byte **row_pointers = NULL;
	PalVideo_FrameBuffer_t *pBuffer = (PalVideo_FrameBuffer_t *)pFrameBuffer;

	if ( fp == NULL)
	{
		goto error_ret;
	}

	if (pBuffer == NULL )
	{
		goto error_ret;
	}

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if ( png_ptr == NULL )
	{
		goto error_ret;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if ( info_ptr == NULL )
	{
		png_destroy_write_struct(&png_ptr, NULL);
		goto error_ret;
	}

	if ( setjmp(png_jmpbuf(png_ptr)) )
	{
		png_destroy_write_struct(&png_ptr, &info_ptr);
		goto error_ret;
	}

	png_set_IHDR(png_ptr,
			info_ptr,
			pBuffer->ulWidth,
			pBuffer->ulHeight,
			8,
			PNG_COLOR_TYPE_RGB_ALPHA ,
			PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT,
			PNG_FILTER_TYPE_DEFAULT);

	bytes_per_row = pBuffer->ulWidth * 4;
	row_pointers = png_malloc(png_ptr, pBuffer->ulHeight * sizeof(png_byte *));
	for ( y = 0; y < pBuffer->ulHeight; ++y )
	{
		HUINT8 *row = png_malloc(png_ptr, pBuffer->ulWidth * sizeof(HUINT8) * 4);
		row_pointers[y] = (png_byte *)row;
		for (x = 0; x < pBuffer->ulWidth; ++x)
		{
			*row++ = (pBuffer->pFrameBuffer[x + (y * pBuffer->ulWidth)] >> 16) & 0xFF;
			*row++ = (pBuffer->pFrameBuffer[x + (y * pBuffer->ulWidth)] >> 8) & 0xFF;
			*row++ = (pBuffer->pFrameBuffer[x + (y * pBuffer->ulWidth)] >> 0) & 0xFF;
			*row++ = (pBuffer->pFrameBuffer[x + (y * pBuffer->ulWidth)] >> 24) & 0xFF;
		}
	}

	png_init_io(png_ptr, fp);
	png_set_rows(png_ptr, info_ptr, row_pointers);
	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	for ( y = 0; y < pBuffer->ulHeight; y++ )
	{
		png_free(png_ptr, row_pointers[y]);
	}
	png_free(png_ptr, row_pointers);

	png_destroy_write_struct(&png_ptr, &info_ptr);

	fclose(fp);

	return ERR_OK;

error_ret:
	if (fp)
	{
		fclose(fp);
	}
	return ERR_FAIL;
#else
	return ERR_FAIL;
#endif
}

#if 0
typedef struct
{
	HBOOL       sbtl_start;

	HINT32      sbtl_shm_id;
	HINT32      sbtl_shm_size;
	HINT32      sbtl_shm_offset;
} _SBTL_INFO_t;
static  _SBTL_INFO_t    *_sbtl_info = NULL;

HERROR media_player_utils_subtitle_create_shmdata(HUINT32 *pulid)
{
	HUINT8          *base, *p;
	HINT32  shm_id;

	if (_sbtl_info == NULL)
	{
		if (HLIB_SHM_Create(MEDIA_TEST_SHM_KEY_ID, MEDIA_TEST_SHM_MEM_SIZE, &shm_id) != ERR_OK)
		{
			return ERR_FAIL;
		}

		_sbtl_info = (_SBTL_INFO_t*)OxMW_Malloc(sizeof(_SBTL_INFO_t));
		memset(_sbtl_info, 0, sizeof(_SBTL_INFO_t));

		_sbtl_info->sbtl_shm_id = shm_id;
		_sbtl_info->sbtl_shm_size = MEDIA_TEST_SHM_MEM_SIZE;
	}

	_sbtl_info->sbtl_shm_offset = 0;
	_sbtl_info->sbtl_start = TRUE;

	if (HLIB_SHM_Attach(_sbtl_info->sbtl_shm_id, (void **)&base) != ERR_OK)
	{
		return ERR_FAIL;
	}

	p = base;

	*(p    ) = 0xB0; 						            //  start
	*(p + 1) = 0xF0;
	*(p + 2) = 0x00;                                    //  size
	*(p + 3) = 0x00;
	*(p + 4) = 0x00;
	*(p + 5) = 0x0D;
	*(p + 6) = 0x03;                                    //  subtitle_type --> SMI Subtitle is 3

	*(p + 7) = 0xFF; 							        //  not used
	*(p + 8) = 0xFF;  							        //  not used
	*(p + 9) = 0xFF;       						  	    //  not used
	*(p + 10) = 0xFF;            						//  not used

	*(p + 11) = 0xE0;
	*(p + 12) = 0xF0;

	HLIB_SHM_Detach(base);

	_sbtl_info->sbtl_shm_offset = 13;

	*pulid = shm_id;

	return ERR_OK;
}

HERROR media_player_utils_subtitle_send_data(HUINT8 *buffer, HINT32 len, HUINT32 *pulStartOffset)
{
	HUINT8  *base, *p;
	HINT32  start_offset;
	HUINT8  *tmpBuffer;
	HINT32  new_start_offset;
	PAL_MEDIA_SUB_Display_Packet_t aa;

	len += 13;

	tmpBuffer = HLIB_STD_MemCalloc(len);
	if (NULL == tmpBuffer)
	{
		return ERR_FAIL;
	}

	start_offset = _sbtl_info->sbtl_shm_offset;

	if (HLIB_SHM_Attach(_sbtl_info->sbtl_shm_id, (void **)&base) != ERR_OK)
	{
		HLIB_STD_MemFree(tmpBuffer);
		return ERR_FAIL;
	}

	p = tmpBuffer;
	*(p    ) = 0xB0;
	*(p + 1) = 0xF1;

	*(p + 2) = (len >> 24) & 0xFF;
	*(p + 3) = (len >> 16) & 0xFF;
	*(p + 4) = (len >>  8) & 0xFF;
	*(p + 5) = (len      ) & 0xFF;

	*(p + 6) = 0x03;          //  subtitle_type --> SMI Subtitle is 3

	*(p + 7) =  0xFF;         //  not used
	*(p + 8) =  0xFF;         //  not used
	*(p + 9) =  0xFF;         //  not used
	*(p + 10) = 0xFF;         //  not used

	if(buffer != NULL)
		memcpy(p + 11, buffer, len - 13);
	else
	{
		memset(&aa, 0xff, sizeof(PAL_MEDIA_SUB_Display_Packet_t));
		memcpy(p + 11, (HUINT8 *)&aa, len - 13);
	}

	*(p + len - 2) = 0xE0;
	*(p + len - 1) = 0xF1;

	new_start_offset = HLIB_STD_WriteCircularBuffer(base, _sbtl_info->sbtl_shm_size, start_offset, len, tmpBuffer);

	HLIB_STD_MemFree(tmpBuffer);

	_sbtl_info->sbtl_shm_offset = new_start_offset;

	HLIB_SHM_Detach(base);

	*pulStartOffset = start_offset;

	return ERR_OK;
}

HERROR media_player_utils_subtitle_destroy_shmdata(HINT32 ulid)
{
	HUINT32 ret_err;
	if(ulid == 0)
		return ERR_FAIL;

	ret_err = HLIB_SHM_Destroy(ulid);
	if(ret_err != ERR_OK)
		return ERR_FAIL;

	return ERR_OK;
}
#endif
