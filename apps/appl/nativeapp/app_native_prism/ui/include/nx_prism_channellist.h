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
 * @file	  		nx_prism_channellist.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_PRISM_CHANNELLIST_H__
#define __NX_PRISM_CHANNELLIST_H__

/**************************************************************************************************/
#define _________PRISM_CHANNELLIST_Private_define_________________________________________________
/**************************************************************************************************/

typedef enum
{
	eNxChlist_OptType_None = 0,
	eNxChlist_OptType_ChannelList,
	eNxChlist_OptType_InfoHub,
	eNxChlist_OptType_Max
} eNxChlist_OptType_t;


typedef enum
{
	eNxChlist_OptItem_All = 0,
	eNxChlist_OptItem_Group,
	eNxChlist_OptItem_Sort,
	eNxChlist_OptItem_AddFav,
	eNxChlist_OptItem_Delete,
	eNxChlist_OptItem_Lock,
	eNxChlist_OptItem_Rename,
	eNxChlist_OptItem_Max
} eNxChlist_OptItem_t;


/**************************************************************************************************/
#define _________PRISM_CHANNELLIST_Private_Macro_________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define _________PRISM_CHANNELLIST_Private_Value_________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define _________PRISM_CHANNELLIST_Private_typedef_________________________________________________
/**************************************************************************************************/


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HUINT32		NX_PRISM_CHANNELLIST_GetCurrentSort(void);


#endif /* !__NX_PRISM_CHANNELLIST_H__ */


