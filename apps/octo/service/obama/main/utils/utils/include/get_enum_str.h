#ifndef	__GET_ENUM_STR_H__
#define	__GET_ENUM_STR_H__

#include <octo_common.h>

const HINT8 *OTL_ENUM2STR_ChDeliveryType(DxDeliveryType_e eDeliveryType);
const HINT8 *OTL_ENUM2STR_VideoCodec(DxVideoCodec_e eVideoCodec);
const HINT8 *OTL_ENUM2STR_VideoFrameRate(VIDEO_FrameRate_t eFrameRate);
const HINT8 *OTL_ENUM2STR_Video3DFormat(DxVideo3DFormat_e e3DFormat);
const HINT8 *OTL_ENUM2STR_Video3DSelect(DxHdmi3DSelect_e e3DSelect);
const HINT8 *OTL_ENUM2STR_VideoPictureRate(DxPictureRate_e ePictureRate);
const HINT8 *OTL_ENUM2STR_Afd(VIDEO_Afd_t eAfd);
const HINT8 *OTL_ENUM2STR_ResolutionSelect(DxResolutionSelect_e eVideoOutputSelect);
const HINT8 *OTL_ENUM2STR_ResolutionStatus(DxResolutionStatus_e eVideoRes);
const HINT8 *OTL_ENUM2STR_AspectRatio(DxAspectRatio_e eAspectRatio);
const HINT8 *OTL_ENUM2STR_DfcSelect(DxDfcSelect_e eDfc);
const HINT8 *OTL_ENUM2STR_Wss(VIDEO_Wss_t eWss);
const HINT8 *OTL_ENUM2STR_ScartFormat(DxScartFormat_e eScartFormat);
const HINT8 *OTL_ENUM2STR_ScartPath(DxScartPath_e eScartPath);
const HINT8 *OTL_ENUM2STR_HdcpStatus(DxHdcpStatus_e eHdcpStatus);
const HINT8 *OTL_ENUM2STR_CompositeStandard(DxCompositeStandard_e eCompositeStandard);
const HINT8 *OTL_ENUM2STR_VideoBlankSpec(VideoBlankSpec_e eVideoBlankSpec);
const HINT8 *OTL_ENUM2STR_AudioDecType(AudioDecType_t eAudioDecType);
const HINT8 *OTL_ENUM2STR_AudioCodec(DxAudioCodec_e eAudioCodec);
const HINT8 *OTL_ENUM2STR_DigitalAudioOutput(DxDigitalAudioFormat_e eDAudioOutMode);
const HINT8 *OTL_ENUM2STR_StereoFormat(DxStereoFormat_e eStereoType);
const HINT8 *OTL_ENUM2STR_StereoSelect(DxStereoSelect_e eStereoSelect);
const HINT8 *OTL_ENUM2STR_DualMonoSelect(DxDualMonoSelect_e eDualMonoSelect);
const HINT8 *OTL_ENUM2STR_Macrovision(Macrovision_t eMacrovision);
const HINT8 *OTL_ENUM2STR_CopyRight(CopyRight_t eCopyRight);
const HINT8 *OTL_ENUM2STR_HdcpBlockMode(HdcpBlockMode_e eHdcpBlockMode);
const HINT8 *OTL_ENUM2STR_DeliveryType(DxDeliveryType_e eDeliveryType);
const HINT8 *OTL_ENUM2STR_TrickModeCtrl(eDxCopyrightControl_SesTrickModeCtrl_e eTrickModeCtrl);
const HINT8 *OTL_ENUM2STR_StorageRight(eDxCopyrightControl_SesStorageRight_e eStorageRight);
const HINT8 *OTL_ENUM2STR_CompVidCtrl(eDxCopyrightControl_SesComponentCtrl_e eCompVidCtrl);

#endif // __GET_ENUM_STR_H__
