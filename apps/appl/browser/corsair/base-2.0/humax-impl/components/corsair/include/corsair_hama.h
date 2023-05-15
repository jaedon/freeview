/**************************************************************************************/
/**
 * @file corsair_hama.h
 *
 * Corsair Tools
 *
 * @author  IPM-SW1T
 * @date    2013/02/11
 * @attention   (c) 2011-2012 Humax Co., Ltd.

 *	This program is produced by Humax Co., Ltd. (¡°Humax¡±) and
 *	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 *	non-assignable, non-transferable and non-exclusive license to use this Software.
 *	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 *	you agree to the responsibility to take all reasonable efforts to protect the any information
 *	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 *	reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 *	If you have no authorized license, discontinue using this Software immediately.

 *	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 *	IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 *	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 *	 CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 *	This License is effective until terminated. You may terminate this License at any time by destroying all copies
 *	of the Software including all documentation. This License will terminate immediately without notice from Humax
 *	to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 *	of the Software and all documentation.

 *	The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 *	All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 *	in the Republic of Korea.

 *
 **************************************************************************************
 **/

#ifndef __CORSAIR_HAMA_H__
#define __CORSAIR_HAMA_H__

#include "hapi.h"

/**********************************************************************************
 * CorsairHama
 **********************************************************************************/
class CorsairHama {
public:
#if defined(CONFIG_PROD_FVP4000T)
    typedef enum {
		MHEG_STATUS_NOME = 0,
		MHEG_STATUS_NOT_RUNNING,
		MHEG_STATUS_STARTING,
		MHEG_STATUS_RUNNING,
		MHEG_STATUS_STOPPING
    } eMhegStatus;
#endif

public:
	static HERROR	onPanelPriority(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
	static HERROR	onUserActionIdle(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
	static HERROR	onStartPortal(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
	static HERROR	onStopPortal(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
	static HERROR	onNetworkConnected(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
	static HERROR	onSystemStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
	static HERROR	onHWindowEvent(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
	static HERROR	onHbbTVAppStart(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
	static HERROR	onTeletextRun(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
	static HERROR	onTeletextState(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
	static HERROR	onSystemLanguageChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
	static HERROR	onOSKResultStr(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
	static HERROR	onOSKResult(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
	static HERROR	onOSKCancel(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
	static HERROR	onSimpleKeyboardStr(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
	static HERROR	onKeyboardStr(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
	static HERROR	onDialStarted(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
	static HERROR	onDialStopped(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
	static HERROR 	onStorageMounted(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
	static HERROR	onStorageUnmounted(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser); // for DIAL
	static HERROR 	onHumaxApplicationCreate(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
	static HERROR 	onHumaxApplicationDestroy(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
	static HERROR 	onHumaxApplicationLoaded(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
	static HERROR 	onHumaxApplicationUnloaded(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
	static HERROR 	onHbbTVEnabled(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
	static HERROR	onMhegLaunchApplication(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
	static HERROR	onNagraPRMstart(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
#if defined(CONFIG_PROD_FVP4000T)
	static HERROR	onMhegNotifyAppStatus(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
	static HERROR	onDeleteCookies(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
#endif
	static HERROR 	onNetflixStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
	static HERROR 	onHbbTvVodStopped(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);

	static void initHamaSignal(void);
	static void shutDown(void *data);
	static void requestShutDown(void *data);
	static void startHbbtvApp(void *data);
	static void setNetworkState(void *data);
	static void startPortal(void *data);
	static void stopPortal(void *data);
	static void setPanelPriority(void *data);
	static void resultKeyboard(void *data);
	static void startSimpleKeyboard(void *data);
	static void startKeyboard(void *data);
#if defined(CONFIG_OP_JCOM)
	static void cancelKeyboard(void *data);
	static void shutDownSurfer(void *data);
#endif
	static void startSurfer();
	static void shutdownSurfer();
	static void toggleTeletext();
	static void setSystemLanguage(int data);
	static void startDragonfly();
	static void initializeWidgets();
};

#endif
