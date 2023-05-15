#include <octo_common.h>

#include <db_param.h>

#include <pmgr_output.h>

#include "./local_include/_xmgr_output.h"

HBOOL pmgr_output_GetDefaultHdcp(void)
{
#if defined(CONFIG_PROD_HMX4KBBC)
	return FALSE;
#else
	return TRUE;
#endif
}

HdcpBlockMode_e pmgr_output_GetHdcpFailBlockSpec(void)
{
	return eHDCP_BLOCK_AVMUTE;
}

HdcpBlockMode_e pmgr_output_GetHdcpNoDeviceBlockSpec(void)
{
	return eHDCP_BLOCK_AVMUTE;
}

HBOOL pmgr_output_GetEdidAspectRatioSpec(void)
{
	return FALSE;
}

HBOOL pmgr_output_GetWideSignalLetterBoxSpec(void)
{
	return FALSE;
}

HBOOL pmgr_output_GetVbiTeletextSpec(void)
{
	return TRUE;
}

VideoBlankSpec_e pmgr_output_GetVideoBlankSpec(void)
{
	return eVIDEO_BLANK_SPEC_WSS;
}

DxCompositeStandard_e pmgr_output_GetCompositeSpec(void)
{
	DxCompositeStandard_e eComSpec = eDxCOMPOSITE_STANDARD_UNKNOWN;

#if defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
	(void)xmgr_model_GetCompositeSpec_ROCKYPOM(&eComSpec);
#elif defined(CONFIG_PROD_HA7100S)
	(void)xmgr_model_GetCompositeSpec_HA7100S(&eComSpec);
#else
	eComSpec = eDxCOMPOSITE_STANDARD_PAL;
#endif

	return eComSpec;
}

HBOOL pmgr_output_GetHdmiAfdEnableSpec(void)
{
#if defined(CONFIG_OP_UK_DTT)
	return xmgr_output_GetHdmiAfdEnableSpec_UkDtt();
#else
	return FALSE;
#endif
}

HBOOL pmgr_output_SupportTvScart(void)
{
	return xmgr_output_SupportTvScart_BASE();
}

HBOOL pmgr_output_SupportVcrScart(void)
{
	return FALSE;
}

HBOOL pmgr_output_SupportSpdif(void)
{
	return TRUE;
}

HBOOL pmgr_output_SupportCec(void)
{
	return TRUE;
}

DxScartFormat_b	pmgr_output_GetSupportTvScartFormat(void)
{
	DxScartFormat_b ulScartFormatCapability = eDxSCART_FORMAT_UNKNOWN;
#if defined(CONFIG_OP_TDC)
	(void) xmgr_output_GetSupportTvScartFormat_Tdc(&ulScartFormatCapability);
#endif
	return ulScartFormatCapability;
}

DxScartFormat_b	pmgr_output_GetSupportVcrScartFormat(void)
{
	return eDxSCART_FORMAT_UNKNOWN;
}

DxDfcSelect_e pmgr_output_GetInitialDfcSelect(void)
{
	return xmgr_output_GetInitialDfcSelect_BASE();
}

DxResolutionSelect_e pmgr_output_GetInitialResolutionSelect(void)
{
	return xmgr_output_GetInitialResolutionSelect_BASE();
}

DxAspectRatio_e pmgr_output_GetInitialTvAspectRatio(void)
{
	return xmgr_output_GetInitialTvAspectRatio_BASE();
}

DxHdmi3DSelect_e pmgr_output_GetInitial3DMode(void)
{
	return xmgr_output_GetInitial3DMode_BASE();
}

DxScartFormat_e pmgr_output_GetInitialTvScartFormat(void)
{
	return xmgr_output_GetInitialTvScartFormat_BASE();
}

DxScartFormat_e pmgr_output_GetInitialVcrScartFormat(void)
{
	return xmgr_output_GetInitialVcrScartFormat_BASE();
}

HUINT32 pmgr_output_GetInitialVolume(void)
{
	return xmgr_output_GetInitialVolume_BASE();
}

HUINT32 pmgr_output_GetInitialDecoderVolume(void)
{
	return xmgr_output_GetInitialDecoderVolume_BASE();
}

void pmgr_output_SetInitialHdmiAudioTransCode(void)
{
	xmgr_output_SetInitialHdmiAudioTransCode_BASE();
}

DxDigitalAudioFormat_e pmgr_output_GetInitialHdmiAudioOutFormat(void)
{
	return xmgr_output_GetInitialHdmiAudioOutFormat_BASE();
}

void pmgr_output_SetInitialSpdifAudioTransCode(void)
{
	xmgr_output_SetInitialSpdifAudioTransCode_BASE();
}

DxDigitalAudioFormat_e pmgr_output_GetInitialSpdifAudioOutFormat(void)
{
	return xmgr_output_GetInitialSpdifAudioOutFormat_BASE();
}


