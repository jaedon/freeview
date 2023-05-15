#ifndef __APK_EXTDEV_H__
#define __APK_EXTDEV_H__


HERROR	APK_EXTDEV_Init(void);
void	APK_EXTDEV_RegisterNotifier(DxHotplugNotifier_t pfnObjectHandler);
HERROR	APK_EXTDEV_MIC_Start(void);
HERROR	APK_EXTDEV_MIC_Stop(void);
HERROR	APK_EXTDEV_MIC_SetVolume(HINT32 nValue);
HERROR	APK_EXTDEV_MIC_GetVolume(HINT32 *pnValue);
HERROR	APK_EXTDEV_MIC_GetCount(HUINT32 *pulCnt);
HERROR	APK_EXTDEV_MIC_GetStatus(HBOOL* pbStatus);


#endif // __APK_EXTDEV_H__

