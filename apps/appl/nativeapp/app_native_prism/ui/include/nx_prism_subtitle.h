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

/******************************************************************************/
/**
 * @file	  		nx_prism_subtitle.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef ____NX_PRISM_SUBTITLE_H____
#define ____NX_PRISM_SUBTITLE_H____
#include <ondk.h>
#include <oapi.h>

typedef	struct	_DVBData
{
	HINT32		page_timeout;
	HUINT16		usCompPageId;
	HUINT16		usAncPageId;

	HxList_t	*page_segment;
	HxList_t	*region_segment;
	HxList_t	*clut_segment;
	HxList_t	*object_segment;
	void		*display_segment;
	HBOOL		end_of_segment;
} DVBData_t;

ONDK_Result_t				NX_PRISM_Subtitle_Proc(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
OxMediaPlay_SubtitleType_e 	NX_PRISM_SubtitleGetType(void);
void                 		NX_PRISM_Subtitle_deliveryPacket(HCHAR* datatype, HINT32 len,HUINT8 *p);
void	            		NX_PRISM_Subtitle_CMD_Register_PESEmul(HUINT8 *cmdname);

#endif	// ____NX_PRISM_SUBTITLE_H____