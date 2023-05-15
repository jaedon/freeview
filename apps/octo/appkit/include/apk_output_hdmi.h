#ifndef __APK_OUTPUT_HDMI_H__
#define __APK_OUTPUT_HDMI_H__

typedef void (*APLK_OUTPUT_HDMI_Notifier_t) (void);


HERROR APK_OUTPUT_HDMI_Init(void);
HERROR APK_OUTPUT_HDMI_RegisterNotifier(APLK_OUTPUT_HDMI_Notifier_t fnNotifier);
HERROR APK_OUTPUT_HDMI_UnregisterNotifier(void);
HERROR APK_OUTPUT_HDMI_DeInit(void);
HERROR APK_OUTPUT_HDMI_SupportCec(HBOOL *pbSupport);
HERROR APK_OUTPUT_HDMI_GetHdcpVersion(DxHdcpVersion_e *peVersion);
HERROR APK_OUTPUT_HDMI_GetHdcpStatus(DxHdcpStatus_e *peHdcpStatus);
HERROR APK_OUTPUT_HDMI_GetNetflixDrmStatus(DxNflixDrmStatus_e *peNflixDrmStatus);
HERROR APK_OUTPUT_HDMI_SetNetflixDrmStatus(DxNflixDrmStatus_e eNflixDrmStatus);
HERROR APK_OUTPUT_HDMI_IsPlugIn(HBOOL *pbPlugInOut);
HERROR APK_OUTPUT_HDMI_Set3DSelect(DxHdmi3DSelect_e eHdmi3DSelect);
HERROR APK_OUTPUT_HDMI_Get3DSelect(DxHdmi3DSelect_e *peHdmi3DSelect);
HERROR APK_OUTPUT_HDMI_GetEdid3DFormatCapability(DxVideo3DFormat_b *pul3DFormatCapability);
HERROR APK_OUTPUT_HDMI_Get3DStatus(DxVideo3DFormat_e *peHdmi3DStatus);
HERROR APK_OUTPUT_HDMI_GetEdidResolutionCapability(DxResolutionStatus_b *pulResolutionCapability);
HERROR APK_OUTPUT_HDMI_SetCecEnable(HBOOL bEnable);
HERROR APK_OUTPUT_HDMI_GetCecEnable(HBOOL *pbEnable);
HERROR APK_OUTPUT_HDMI_SetCecVolumeCtrlEnable(HBOOL bEnable);
HERROR APK_OUTPUT_HDMI_GetCecVolumeCtrlEnable(HBOOL *pbEnable);
HERROR APK_OUTPUT_HDMI_CecUserCtrlPressed(DxHdmiUserCtrlPressed_e eParam);
HERROR APK_OUTPUT_HDMI_GetEdidInfo(HCHAR *pszEdidInfo);
HERROR APK_OUTPUT_HDMI_GetEdidDisplayResolution(HUINT32 *pulDisplayResX, HUINT32 *pulDisplayResY);
#endif // __APK_OUTPUT_HDMI_H__

