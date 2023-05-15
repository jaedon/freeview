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
 * @file	  		nx_prism_live.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_PRISM_LIVE_H__
#define __NX_PRISM_LIVE_H__

#include <nx_core_channel.h>

/**************************************************************************************************/
#define _________PRISM_LIVE_Private_define_________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define _________PRISM_LIVE_Private_Include_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________PRISM_LIVE_Private_Macro_________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define _________PRISM_LIVE_Private_Value_________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define _________PRISM_LIVE_Private_typedef_________________________________________________
/**************************************************************************************************/



/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
NX_Channel_t*	NX_PRISM_LIVE_GetLiveChannels(void);
NX_Channel_t*	NX_PRISM_LIVE_GetMasterChannel(void);
HERROR			NX_PRISM_LIVE_SetPrevChannel(NX_Channel_t *channel);
HERROR 			NX_PRISM_LIVE_SetChannel(NX_Channel_t *channel, HINT32 curChannelIndex);
HERROR			NX_PRISM_LIVE_ServiceStartMainView(HBOOL bSendMsg, HUINT32 ulSvcUid, NX_ChListGroup_t eChListGroup, HUINT32 ulExtraFilter);
HERROR			NX_PRISM_LIVE_ServiceStartByLastService(void);
NX_ChListGroup_t	NX_PRISM_LIVE_GetCurrentChGroupType(void);
HERROR			NX_PRISM_LIVE_GetCurChannelIndex(HINT32 *nChannelIndex);
HERROR 			NX_PRISM_LIVE_GetChannelListNum(HINT32 *nChannelListNum);
HERROR			NX_PRISM_LIVE_GetCurChannelAudioCodec(DxAudioCodec_e *peAudCodec);

ONDK_Result_t 	NX_PRISM_DCN_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);

#endif /* !__NX_PRISM_LIVE_H__ */


