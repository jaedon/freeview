#ifndef __APK_OUTPUT_AUDIO_H__
#define __APK_OUTPUT_AUDIO_H__

typedef void (*APK_OUTPUT_AUDIO_Notifier_t) (void);

HERROR APK_OUTPUT_AUDIO_Init(void);
HERROR APK_OUTPUT_AUDIO_RegisterNotifier(APK_OUTPUT_AUDIO_Notifier_t fnNotifier);
HERROR APK_OUTPUT_AUDIO_UnregisterNotifier(void);
HERROR APK_OUTPUT_AUDIO_DeInit(void);
HERROR APK_OUTPUT_AUDIO_SupportSpdif(HBOOL *pbSupport);
HERROR APK_OUTPUT_AUDIO_GetMasterMute(HBOOL *pbMute);
HERROR APK_OUTPUT_AUDIO_SetMasterMute(HBOOL bMute);
HERROR APK_OUTPUT_AUDIO_GetVolumeMax(HUINT32 *pulMaxVolume);
HERROR APK_OUTPUT_AUDIO_GetVolume(HUINT32 *pulVolume);
HERROR APK_OUTPUT_AUDIO_GetDecorderVolume(HUINT32 *pulVolume);
HERROR APK_OUTPUT_AUDIO_SetVolume(HUINT32 ulVolume);
HERROR APK_OUTPUT_AUDIO_SetDecoderVolume(HUINT32 ulVolume);
HERROR APK_OUTPUT_AUDIO_GetLipSyncDelay(HUINT32 *pulmsec);
HERROR APK_OUTPUT_AUDIO_SetLipSyncDelay(HUINT32 ulmsec);
HERROR APK_OUTPUT_AUDIO_SetHdmiFormat(DxDigitalAudioFormat_e eDigitalAudioFormat);
HERROR APK_OUTPUT_AUDIO_GetHdmiFormat(DxDigitalAudioFormat_e *peDigitalAudioFormat);
HERROR APK_OUTPUT_AUDIO_SetSpdifFormat(DxDigitalAudioFormat_e eDigitalAudioFormat);
HERROR APK_OUTPUT_AUDIO_GetSpdifFormat(DxDigitalAudioFormat_e *peDigitalAudioFormat);
HERROR APK_OUTPUT_AUDIO_GetAudioDescription(HBOOL *pbEnable);
HERROR APK_OUTPUT_AUDIO_SetAudioDescription(HBOOL bEnable);
HERROR APK_OUTPUT_AUDIO_SetStereoSelect(DxStereoSelect_e eStereoSelect);
HERROR APK_OUTPUT_AUDIO_GetStereoSelect(DxStereoSelect_e *peStereoSelect);
HERROR APK_OUTPUT_AUDIO_SetDualMonoSelect(DxDualMonoSelect_e eDualMonoSelect);
HERROR APK_OUTPUT_AUDIO_GetDualMonoSelect(DxDualMonoSelect_e *peDualMonoSelect);

#endif // __APK_OUTPUT_AUDIO_H__

