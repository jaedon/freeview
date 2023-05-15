#ifndef __APK_OUTPUT_THERMAL_H__
#define __APK_OUTPUT_THERMAL_H__

typedef void (*APLK_OUTPUT_THERMAL_Notifier_t) (void);


HERROR APK_OUTPUT_THERMAL_Init(void);
HERROR APK_OUTPUT_THERMAL_RegisterNotifier(APLK_OUTPUT_THERMAL_Notifier_t fnNotifier);
HERROR APK_OUTPUT_THERMAL_UnregisterNotifier(void);
HERROR APK_OUTPUT_THERMAL_Shutdown(void);

#endif // __APK_OUTPUT_THERMAL_H__

