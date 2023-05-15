/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk.h
	@brief

	Description:  									\n
	Module: APPKITv2		\n

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

#ifndef ___APK2_H___
#define ___APK2_H___

#define _____INSIDE_APK2_H_____

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#ifdef	__cplusplus
extern "C" {
#endif

#include <hlib.h>
#include <dlib.h>
#include <oapi.h>

#include <apk_deprecated.h>
#include <apk_media_play.h>
#include <apk_media_record.h>
#include <apk_builder.h>
#include <apk_filemgr.h>
#include <apk_filemgr_storage.h>
#include <apk_filemgr_group.h>
#include <apk_filemgr_browse.h>
#include <apk_filemgr_sambaclient.h>
#include <apk_filemgr_sambaserver.h>
#include <apk_filemgr_ftpserver.h>
#include <apk_output_video.h>
#include <apk_output_audio.h>
#include <apk_output_hdmi.h>
#include <apk_output_panel.h>
#include <apk_output_thermal_protection.h>
#include <apk_meta_schedule.h>
#include <apk_sysinfo_tuner.h>
#include <apk_sysinfo_stb.h>
#include <apk_meta_service.h>
#include <apk_meta_antenna.h>
#include <apk_meta_query.h>
#include <apk_meta_record.h>
#include <apk_meta_search.h>
#include <apk_meta_mail.h>
#include <apk_scenario.h>
#include <apk_scenario_pincode.h>
#include <apk_cas_nagra.h>
#include <apk_cas_irdeto.h>
#include <apk_cas_verimatrix.h>
#include <apk_cas_mmi.h>
#include <apk_db.h>
#include <apk_cas_ci.h>
#include <apk_cas_action.h>
#include <apk_swupdate.h>
#include <apk_dsmcc.h>
#include <apk_power.h>
#include <apk_exe.h>
#include <apk_network.h>
#include <apk_dlna.h>
#include <apk_event.h>
#include <apk_airplay.h>
#include <apk_drmagent.h>
#include <apk_datastreamer.h>
#include <apk_extdev.h>
#include <apk_resource.h>
#include <apk_download.h>


#undef _____INSIDE_APK2_H_____

extern HERROR		APK_Init (void);
#ifdef	CONFIG_DEBUG
extern HERROR		APK_CMD_Init (void);
#endif

#ifdef __cplusplus
}
#endif
#endif
/* end of file */
