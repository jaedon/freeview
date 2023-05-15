/**************************************************************
 *	@file		apk_swupdate.h
 *	appkit sqc internal header
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2011/11/30
 *	@author			humax
 *	@brief			appkit swupdate
 **************************************************************/
#ifndef	__APK_SWUPDATE_INT_H__
#define	__APK_SWUPDATE_INT_H__


typedef void (*APK_SWUPDATE_Notifier_t) (DxSwUpdate_EventData_t *pstData);

void	APK_SWUPDATE_Init(void);
void	APK_SWUPDATE_DeInit(void);
HERROR  APK_SWUPDATE_Create(DxSwUpdate_Spec_e eSwUpdateSpec, Handle_t *phSwupdate, void *pUserData);
HERROR  APK_SWUPDATE_CreateManualConnect(DxSwUpdate_Spec_e eSwUpdateSpec, Handle_t *phSwupdate, DxTransponder_t *pTunningInfo);
HERROR 	APK_SWUPDATE_SwupdateInit(APK_SWUPDATE_Notifier_t fnNotifier);
HERROR	APK_SWUPDATE_StartDetect(APK_SWUPDATE_Notifier_t fnNotifier, Handle_t hSwUpdate);
HERROR	APK_SWUPDATE_StopDetect(Handle_t hSwUpdate);
HERROR	APK_SWUPDATE_StartDownload(APK_SWUPDATE_Notifier_t fnNotifier, Handle_t hSwUpdate);
HERROR	APK_SWUPDATE_StopDownload(Handle_t hSwUpdate);
HERROR	APK_SWUPDATE_StartInstall(APK_SWUPDATE_Notifier_t fnNotifier, Handle_t hSwUpdate);
HERROR	APK_SWUPDATE_StopInstall(Handle_t hSwUpdate);
HERROR	APK_SWUPDATE_Cancel(Handle_t hSwUpdate);


#endif //__APK_SWUPDATE_INT_H__

