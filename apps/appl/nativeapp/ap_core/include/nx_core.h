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
 * @file	  		nx_core.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_CORE_H__
#define __NX_CORE_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include <ondk.h>
#include <hapi.h>
#include <papi.h>
#include <thapi.h>
#include <hlib.h>
#include <dlib.h>
#include <apk.h>

#include <nx_core_av.h>
#include <nx_core_channel.h>
#include <nx_core_search.h>
#include <nx_core_system.h>
#include <nx_core_swup.h>
#if defined(CONFIG_MW_INET) || defined(CONFIG_MW_INET_WLAN)
#include <nx_core_network.h>
#endif
#if defined(CONFIG_MW_CH_SATELLITE)
#include <nx_core_antenna.h>
#endif
#if defined(CONFIG_PROD_DEVICE_FILE_SYSTEM)
#include <nx_core_filemgr_storage.h>
#include <nx_core_filemgr_browser.h>
#include <nx_core_filemgr_file.h>
#endif
#if defined(CONFIG_MW_MM_PVR)
#include <nx_core_recorder.h>
#endif
#include <nx_core_contents.h>
#include <nx_core_media.h>
#include <nx_core_player.h>
#include <nx_core_message.h>
#include <nx_core_metadata_programmes.h>
#include <nx_core_metadata_search.h>
#include <nx_core_metadata_epg.h>
#include <nx_core_scheduler.h>

#if defined(CONFIG_MW_CAS)
#include <nx_core_cas.h>
#endif

#if defined(CONFIG_MW_MM_DLNA)
#include <nx_core_dlna.h>
#endif


/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/


/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/

#define	NX_CORE_IsHardOfHearingSubtitle(type)			(((type) == 0x05) || (0x20 <= (type) && (type) <= 0x24))
#define	NX_CORE_IsEbuSubtitle(type)						((type) <= 0x05)
#define	NX_CORE_IsDvbSubtitle(type)						((0x10 <= (type) && (type) <= 0x14))


/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/
#define	NX_CORE_PID_NULL								0x2000


/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/


/*******************************************************************/
/********************	global function prototype   ****************/
/*******************************************************************/
HERROR NX_CORE_Init(void);


#endif // __NX_CORE_H__

