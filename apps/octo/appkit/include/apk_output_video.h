#ifndef __APK_OUTPUT_VIDEO_H__
#define __APK_OUTPUT_VIDEO_H__

typedef void (*APK_OUTPUT_VIDEO_Notifier_t) (void);

HERROR APK_OUTPUT_VIDEO_Init (void);
HERROR APK_OUTPUT_VIDEO_RegisterNotifier(APK_OUTPUT_VIDEO_Notifier_t fnNotifier);
HERROR APK_OUTPUT_VIDEO_UnregisterNotifier(void);
HERROR APK_OUTPUT_VIDEO_DeInit(void);
HERROR APK_OUTPUT_VIDEO_SupportComposite(HBOOL *pbSupport);
HERROR APK_OUTPUT_VIDEO_SupportTvScart(HBOOL *pbSupport);
HERROR APK_OUTPUT_VIDEO_SupportVcrScart(HBOOL *pbSupport);
HERROR APK_OUTPUT_VIDEO_GetSupportTvScartFormat(DxScartFormat_b *pulScartFormat);
HERROR APK_OUTPUT_VIDEO_GetSupportVcrScartFormat(DxScartFormat_b *pulScartFormat);
HERROR APK_OUTPUT_VIDEO_SetTvScartFormat(DxScartFormat_e eScartFormat);
HERROR APK_OUTPUT_VIDEO_GetTvScartFormat(DxScartFormat_e *peScartFormat);
HERROR APK_OUTPUT_VIDEO_SetVcrScartFormat(DxScartFormat_e eScartFormat);
HERROR APK_OUTPUT_VIDEO_GetVcrScartFormat(DxScartFormat_e *peScartFormat);
HERROR APK_OUTPUT_VIDEO_SetScartPath(DxScartPath_e eScartPath);
HERROR APK_OUTPUT_VIDEO_GetScartPath(DxScartPath_e *peScartPath);
HERROR APK_OUTPUT_VIDEO_SetResolutionSelect(DxResolutionSelect_e eResolutionSelect);
HERROR APK_OUTPUT_VIDEO_GetResolutionSelect(DxResolutionSelect_e *peResolutionSelect);
HERROR APK_OUTPUT_VIDEO_GetResolutionStatus(DxResolutionStatus_e *peResolutionStatus);
HERROR APK_OUTPUT_VIDEO_SetTvAspectRatio(DxAspectRatio_e eTvAspectRatio);
HERROR APK_OUTPUT_VIDEO_GetTvAspectRatio(DxAspectRatio_e *peTvAspectRatio);
HERROR APK_OUTPUT_VIDEO_GetPictureRate(DxPictureRate_e *pePictureRate);
HERROR APK_OUTPUT_VIDEO_SetDfcSelect(DxDfcSelect_e eDfcSelect);
HERROR APK_OUTPUT_VIDEO_GetDfcSelect(DxDfcSelect_e *peDfcSelect);
HERROR APK_OUTPUT_VIDEO_GetDfcStatus(DxDfcStatus_b *pulDfcStatus);
HERROR APK_OUTPUT_VIDEO_SetBackGroundColor(HUINT8 R, HUINT8 G, HUINT8 B);
HERROR APK_OUTPUT_VIDEO_GetBackGroundColor(HUINT8 *pR, HUINT8 *pG, HUINT8 *pB);
HERROR APK_OUTPUT_VIDEO_SetAvEnable(HBOOL bEnable);
HERROR APK_OUTPUT_VIDEO_GetAvEnable(HBOOL *pbEnable);
HERROR APK_OUTPUT_VIDEO_GetCompositeStandard(DxCompositeStandard_e *peCompositeStandard);
HERROR APK_OUTPUT_VIDEO_SetSupportVideoFrame(HBOOL bEnable);
HERROR APK_OUTPUT_VIDEO_GetVideoBlankCopyRight(DxCopyRight_e *peVideoBlankCopyRight);
HERROR APK_OUTPUT_VIDEO_SetVideoBlankCopyRight(DxCopyRight_e eVideoBlankCopyRight);

#endif // __APK_OUTPUT_VIDEO_H__
