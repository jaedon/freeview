#ifndef __DRV_DRM_H__
#define __DRV_DRM_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"
#include "drv_err.h"

#include "di_media20.h"
#include "di_drm.h"

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/

DRV_Error DRV_DRM_CreateSession(SESSION_HANDLE media_session_handle);
DRV_Error DRV_DRM_GetInfo(SESSION_HANDLE media_session_handle, DRM_Info_t *drm_info);
DRV_Error DRV_DRM_ResigterCallback(SESSION_HANDLE media_session_handle, DRM_Info_t *drm_info);
DRV_Error DRV_DRM_GetLicence(SESSION_HANDLE media_session_handle);
DRV_Error DRV_DRM_PlayReady_GetLicenseURL(SESSION_HANDLE media_session_handle, HUINT8* pucWrmHeader, HUINT32 uiWrmHeaderSize, HCHAR *licenseURL);
DRV_Error DRV_DRM_GetKey(SESSION_HANDLE media_session_handle, DRM_Key_Info_t *drm_key_info);
DRV_Error DRV_DRM_GetDRMStatus(SESSION_HANDLE media_session_handle);
DRV_Error DRV_DRM_DestroySession(SESSION_HANDLE media_session_handle);
DRV_Error DRV_DRM_RegisterOutputControlCallback(SESSION_HANDLE media_session_handle, DI_DRM_OutputControlCallback_t OutputControlCallback);
DRV_Error DRV_DRM_UnregisterOutputControlCallback(SESSION_HANDLE media_session_handle);
DRV_Error DRV_DRM_RegisterLicenseDataCallback(SESSION_HANDLE media_session_handle, DI_DRM_LicenseDataCallback_t callback);
DRV_Error DRV_DRM_UnregisterLicenseDataCallback(SESSION_HANDLE media_session_handle);
DRV_Error DRV_DRM_SetLicenseData(SESSION_HANDLE media_session_handle, HUINT8* pucLicenseData);
DRV_Error DRV_DRM_PushPumpBuffer(SESSION_HANDLE media_session_handle, void *pvDst, void *pvSrc, HUINT32 unSize, HBOOL bflush);
void* DRV_DRM_DataMemAlloc(HUINT32 unSize);
DRV_Error DRV_DRM_DataMemFree(void *pvBuffer);

#if (NEXUS_VERSION >= 1290) /* for playready */
#if defined (CONFIG_PLAYREADY)
DRV_Error DRV_DRM_RegisterPostCallback(void);
DRV_Error DRV_DRM_RegisterSecureClockCallback(void);
#endif
#endif

#if defined (CONFIG_PLAYREADY_25_BRCM)
DRV_Error DRV_DRM_SecureClock(SESSION_HANDLE media_session_handle);
DRV_Error DRV_DRM_DoDecrypt(SESSION_HANDLE media_session_handle, HUINT8 *pBuff, HINT32 nBuffSize, HUINT8 *pIv, HINT32 nIvSize);
DRV_Error DRV_DRM_GetStoredLicense(SESSION_HANDLE media_session_handle);
#endif

#endif /* !__DRV_DRM_H__ */


