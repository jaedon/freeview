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
 * @file	  		nx_prism_db_default_ha7100s.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/


/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_port.h>
#include <nx_core.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_PORT_DB_DEFAULT_INSTALL_PIN_COCE						"1354"


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
HERROR	NX_PORT_DB_Default(void)
{
	HCHAR		szInstallPinCode[NX_PORT_PIN_DIGIT_MAX] = { 0, };

	NX_PORT_DB_SetInt(USERDB_SYSTEM_B_FIRSTTIME_BOOT, TRUE);

	// Base
	NX_PORT_DB_SetInt(USERDB_SUBTITLE_DISPLAY, 0);
	NX_PORT_DB_SetInt(USERDB_SUBTITLE_FONT, 1);
	NX_PORT_DB_SetInt(USERDB_RECORDING_TSR_ONOFF, FALSE);
	NX_PORT_DB_SetInt(USERDB_SCREENDISP_INFO_DISP_TIME, 5);
	NX_PORT_DB_SetInt(USERDB_TVTVEPG_LASTUPDATETIME, 0);

	NX_PORT_DB_SetInt(USERDB_SEARCH_OPT_NETWORK_SEARCH, FALSE);
	NX_PORT_DB_SetInt(USERDB_SEARCH_OPT_SCRAMBLE_TYPE, eDxCAS_TYPE_All);
	NX_PORT_DB_SetInt(USERDB_SEARCH_OPT_CHANNEL_TYPE, eDxSVC_TYPE_All);
	NX_PORT_DB_SetInt(USERDB_SYSTEM_AUTOMATIC_POWER_DOWN, TRUE);

	// PVR
#if defined(CONFIG_MW_MM_PVR)
	NX_PORT_DB_SetInt(USERDB_SERIES_PLAYBACK, FALSE);
	NX_PORT_DB_SetInt(USERDB_RECORDING_AUTODELETE_ONOFF, FALSE);
	NX_PORT_DB_SetInt(USERDB_RECORDING_DESCRAMBLE_ONOFF, FALSE);
#endif

	// Media
#if defined(CONFIG_MW_MM_MEDIA)
	NX_PORT_DB_SetInt(USERDB_VIDEO_REPEAT_OPTION, eNxMediaPlay_Video_RepeatOnce);
#endif

	// Network
#if defined (CONFIG_MW_INET)
	NX_PORT_DB_SetInt(USERDB_HBBTV_ENABLE, FALSE);

	NX_PORT_DB_SetInt(USERDB_IPEPG_ONOFF, FALSE);
	NX_PORT_DB_SetInt(USERDB_RP_ONOFF, FALSE);
	NX_PORT_DB_SetInt(USERDB_RP_PERIOD_START, 28800);
	NX_PORT_DB_SetInt(USERDB_RP_PERIOD_END, 79200);
	NX_PORT_DB_SetInt(USERDB_RP_INTERVAL, 1800);
	NX_PORT_DB_SetInt(USERDB_RP_REG_CODE, 0);

	NX_PORT_DB_SetInt(USERDB_DLNA_DMS_ACTIVATION, TRUE);
	NX_PORT_DB_SetInt(USERDB_DLNA_DMR_ACTIVATION, TRUE);
	NX_PORT_DB_SetInt(USERDB_DLNA_DIAL_ACTIVATION, FALSE);   /* HMS PH2 base: default disabled(by webapp), because the "youtube" app registration process */
	NX_PORT_DB_SetInt(USERDB_DLNA_SATIP_ACTIVATION, TRUE);
	NX_PORT_DB_SetInt(USERDB_AIRPLAY_ACTIVATION, TRUE);      /* HMS PH2 base: always enabled, regardless of the network server on/off state */
	NX_PORT_DB_SetInt(USERDB_WOONSERVER_ACTIVATION, TRUE);
	NX_PORT_DB_SetInt(USERDB_DLNA_SATIP_ACTIVATION, TRUE);
	NX_PORT_DB_SetInt(USERDB_SAMBACLIENT_ACTIVATION, FALSE); /* unsupported feature */
	NX_PORT_DB_SetInt(USERDB_SAMBASERVER_ACTIVATION, FALSE); /* HMS PH2 base: default-off */
	NX_PORT_DB_SetInt(USERDB_FTPSERVER_ACTIVATION, FALSE);   /* HMS PH2 base: default-off */
#endif

	// Satellite
	NX_PORT_DB_SetInt(USERDB_ANTSET_ANTENNATYPE_ID, eNxSat_AntType_LNB_Only);
	NX_PORT_DB_SetInt(USERDB_FIRST_ANTENNA_ID, NX_ANT_IDX_NULL);
	NX_PORT_DB_SetInt(USERDB_SECOND_ANTENNA_ID, NX_ANT_IDX_NULL);
	NX_PORT_DB_SetInt(USERDB_THIRD_ANTENNA_ID, NX_ANT_IDX_NULL);
	NX_PORT_DB_SetInt(USERDB_FOURTH_ANTENNA_ID, NX_ANT_IDX_NULL);
	NX_PORT_DB_SetInt(USERDB_FIFTH_ANTENNA_ID, NX_ANT_IDX_NULL);
	NX_PORT_DB_SetInt(USERDB_SIXTH_ANTENNA_ID, NX_ANT_IDX_NULL);
	NX_PORT_DB_SetInt(USERDB_SEVENTH_ANTENNA_ID, NX_ANT_IDX_NULL);
	NX_PORT_DB_SetInt(USERDB_EIGHTH_ANTENNA_ID, NX_ANT_IDX_NULL);

	// Favourite name
	NX_PORT_DB_SetInt(USERDB_FAVOURITE1_NAME_UPDATE_FLAG_ID, FALSE);
	NX_PORT_DB_SetInt(USERDB_FAVOURITE2_NAME_UPDATE_FLAG_ID, FALSE);
	NX_PORT_DB_SetInt(USERDB_FAVOURITE3_NAME_UPDATE_FLAG_ID, FALSE);
	NX_PORT_DB_SetInt(USERDB_FAVOURITE4_NAME_UPDATE_FLAG_ID, FALSE);
	NX_PORT_DB_SetInt(USERDB_FAVOURITE5_NAME_UPDATE_FLAG_ID, FALSE);

	HxSTD_memset(szInstallPinCode, '0', sizeof(HUINT8) * (NX_PORT_PIN_DIGIT_MAX));
	HxSTD_snprintf(szInstallPinCode, NX_PORT_PIN_DIGIT_MAX, "%s", (char*)NX_PORT_DB_DEFAULT_INSTALL_PIN_COCE);
	szInstallPinCode[NX_PORT_PIN_DIGIT_MAX-1] = '\0';
	APK_DB_SetStr(USERDB_INSTALLTION_PINCOCE, szInstallPinCode);

	return	ERR_OK;
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

