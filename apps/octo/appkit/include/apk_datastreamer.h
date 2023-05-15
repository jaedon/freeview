#ifndef __APK_DATASTREAMER_H__
#define __APK_DATASTREAMER_H__

typedef void (*APK_DATASTREAMER_Notifier_t) (HINT32 size, void *pvData);

HERROR	APK_DATASTREAMER_InitDataStreamer(APK_DATASTREAMER_Notifier_t fnNotifier);
HERROR	APK_DATASTREAMER_ConnectDataStreamer(HCHAR *data_type);
HERROR	APK_DATASTREAMER_DisconnectDataStreamer(HCHAR *data_type);

#endif // __APK_DATASTREAMER_H__


