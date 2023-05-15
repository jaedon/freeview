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
 * @file	  		nx_prism_installwizard.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_PRISM_INSTALL_SUB_NETWORK_H__
#define __NX_PRISM_INSTALL_SUB_NETWORK_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <ondk.h>
#include <ondk_type.h>
#include <ondk_gfx.h>
#include <hlib.h>

#include <nx_common.h>

#include <nx_prism_app.h>
#include <nx_prism_ui.h>
#include <nx_prism_component.h>

#include <nx_prism_install_main_ui.h>
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_PRISM_INSTALL_NETWORK_ITEM_ID			    ((NX_PRISM_INSTALL_ITEM_ID)+1200)
#define	NX_PRISM_INSTALL_NETWORK_HIDDEN_ITEM_ID			((NX_PRISM_INSTALL_NETWORK_ITEM_ID)+500)

#define	NX_PRISM_INSTALL_NETWORK_BTN_ID			        ((NX_PRISM_INSTALL_NETWORK_ITEM_ID)+10) //BUTTON

#define	NX_PRISM_INSTALL_NETWORK_BTN_PREVIOUS_ID		((NX_PRISM_INSTALL_NETWORK_BTN_ID)+1)
#define	NX_PRISM_INSTALL_NETWORK_BTN_NEXT_ID			((NX_PRISM_INSTALL_NETWORK_BTN_ID)+2)

#define NX_PRISM_INSTALL_NETWORK_BTN_X                    380
#define NX_PRISM_INSTALL_NETWORK_BTN_Y                    615
#define NX_PRISM_INSTALL_NETWORK_BTN_W                    252
#define NX_PRISM_INSTALL_NETWORK_BTN_H                    55
#define NX_PRISM_INSTALL_NETWORK_BTN_GAP                  17

#define NX_INSTALL_NETWORK_HIDDEN_INPUT_MAX                     48
/******************************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum {
	eInstall_Network_INIT = 0,
    eInstall_Network_LAN_TRYING,
	eInstall_Network_LAN_CONNECTED,
	eInstall_Network_LAN_DISCONNECTED,

	eInstall_Network_WIFI_SEARCHING,
	eInstall_Network_WIFI_POPUP,
	eInstall_Network_WIFI_RESPONSE_POPUP,
	eInstall_Network_WIFI_HIDDEN_NETWORK,
	eInstall_Network_WIFI_HIDDEN_KEYBOARD_SECURITY_OPTION,
	eInstall_Network_WIFI_HIDDEN_KEYBOARD_NAME,
	eInstall_Network_WIFI_HIDDEN_KEYBOARD_PW,
	eInstall_Network_MAX
} NxInstall_NetworkState_t;

typedef enum {
	eInstall_Network_WIFI_IMG_F_0 = 0,
	eInstall_Network_WIFI_IMG_F_1,
	eInstall_Network_WIFI_IMG_F_2,
	eInstall_Network_WIFI_IMG_F_3,
	eInstall_Network_WIFI_IMG_F_4,
	eInstall_Network_WIFI_IMG_N_0,
	eInstall_Network_WIFI_IMG_N_1,
	eInstall_Network_WIFI_IMG_N_2,
	eInstall_Network_WIFI_IMG_N_3,
	eInstall_Network_WIFI_IMG_N_4,
	eInstall_Network_WIFI_IMG_LOCK,
	eInstall_Network_WIFI_IMG_MAX
} NxInstall_Network_WIFI_IMG_ID_t;

typedef struct tagInstall_Network_Wifi_Ap {
    HCHAR*  apName;
    HINT32  lSignalPower; //0~99
    HBOOL   bLocked;
} Nx_Install_Network_WIFI_AP_t;

typedef struct tagInstall_Network_Context {
	HINT32		lCurFocus;
	HUINT32		ulNumOfMainItem, ulMaxOfMainItem;
    Nx_InstallSubPage_t	*pstSubItems;

	HINT32		lDefaultFocusIdx;
	HUINT8		ucTitle[NX_PRISM_INSTALL_MAX_ELEMENT_TEXT_LENGTH];
    HUINT8      ulNumOfSubPage;// Nx_InstallSubPage_t sub page number

    NxInstallPage_t ePage;
    HINT32      lLoadingIndex;
    APKE_NET_STATUS_e connectionStatus;
    NxInstall_NetworkState_t eUiState;

//WIFI
    ONDKImageHandle hWifiImgHandleList[eInstall_Network_WIFI_IMG_MAX];
    HUINT32     ulNumOfAp;
	APKS_NETAP_Info_t *apList;
	HINT32 lSelectedItemIndex;

//HIDDEN
	APKS_NETAP_Info_t *apValue;
    HCHAR       strPassword[NX_INSTALL_NETWORK_HIDDEN_INPUT_MAX];
} Nx_Install_Network_Context_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

#ifdef CONFIG_MW_INET
extern ONDK_Result_t	NX_PRISM_Install_Network_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
#ifdef CONFIG_MW_INET_WLAN
extern ONDK_Result_t	NX_PRISM_Install_Network_Wifi_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_Install_Network_Hidden_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
#endif //CONFIG_MW_INET_WLAN
#endif //CONFIG_MW_INET

#endif // __NX_PRISM_INSTALL_SUB_NETWORK_H__