/**************************************************************
 *	@file		apk_dsmcc.h
 *	appkit sqc internal header
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2011/11/30
 *	@author			humax
 *	@brief			appkit dsmcc header
 **************************************************************/
#ifndef	__DSMCC_INIT_H__
#define	__DSMCC_INIT_H__

#include <dlib_dsmcc.h>

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef	void	(*APK_DSMCC_Notifiler_t)(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);


/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

void	APK_DSMCC_Init(void);
void	APK_DSMCC_DeInit(void);
HERROR	APK_DSMCC_Create(DxDSMCC_Type_e eType, Handle_t *phDsmcc, HCHAR **ppszCachePath);
HERROR 	APK_DSMCC_Destroy(Handle_t hDsmcc);
void	APK_DSMCC_FreePath(HCHAR *ppszCachePath);
void	APK_DSMCC_RegisterNotifier(Handle_t hDsmcc, APK_DSMCC_Notifiler_t fnNotifier);
void	APK_DSMCC_UnRegisterNotifier(Handle_t hDsmcc, APK_DSMCC_Notifiler_t fnNotifier);
HERROR	APK_DSMCC_GetCarouselList(Handle_t hDsmcc, DxDSMCC_CarouselList_t *pstList);
void	APK_DSMCC_FreeCarouselInfo(DxDSMCC_CarouselInfo_t *pList);
HERROR	APK_DSMCC_Start(Handle_t hDsmcc, Handle_t hCarousel, const HCHAR *pszCachePath);
HERROR	APK_DSMCC_Restart(Handle_t hDsmcc, Handle_t hCarousel);
HERROR	APK_DSMCC_Stop(Handle_t hDsmcc, Handle_t hCarousel);
HERROR	APK_DSMCC_RequestEvent(Handle_t hDsmcc, HBOOL bAll);
HERROR	APK_DSMCC_Open(Handle_t hDsmcc, Handle_t hSession);
HERROR	APK_DSMCC_Close(Handle_t hDsmcc, Handle_t hSession);

#endif //__DSMCC_INIT_H__

