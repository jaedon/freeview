#include <hlib.h>
#include <irldvt_stb_params.h>
#include <irldvt_output.h>
//#include <db_param.h>
//#include <_db_param.h>


#define NO_AV_EVENT_TIMEOUT					6000

//STATIC HdcpBlockMode_e 		s_eHdcpFailBlockMode;
//STATIC HdcpBlockMode_e 		s_eHdcpNoDeviceBlockMode;

#if 0
HERROR IRLDVT_OUT_SetInitParameters(
							HBOOL						bVbiTeletextEnable,
							VideoBlankSpec_e			eVideoBlankSpec,
							DxCompositeStandard_e		eCompositeStandard,
							DxDfcSelect_e 				eDfc,
							DxResolutionSelect_e		eResolutionSelect,
							DxAspectRatio_e 			eTvAspectRatio,
							DxHdmi3DSelect_e			e3DMode,
							HBOOL						bUseEdidAspectRatio,
							HBOOL						bWideSignaloLetterBox,
							HBOOL						bEnableHdmiAfd,
							HdcpBlockMode_e				eHdcpFailBlockMode,
							HdcpBlockMode_e				eHdcpNoDeviceBlockMode,
							DxScartFormat_e				eTvScartFormat,
							DxScartFormat_e				eVcrScartFormat,
							DxDigitalAudioFormat_e		eHdmiAudioOutFormat,
							DxDigitalAudioFormat_e		eSpdifAudioOutFormat
							)
{

	PAL_OUT_SetInitParameters(bVbiTeletextEnable,
							eVideoBlankSpec,
							eHdmiAudioOutFormat,
							eSpdifAudioOutFormat
							);


	PAL_SCALER_SetInitParameters(eCompositeStandard,
							eDfc,
							eResolutionSelect,
							eTvAspectRatio,
							e3DMode,
							bUseEdidAspectRatio,
							bWideSignaloLetterBox,
							bEnableHdmiAfd
							);


	//s_eHdcpFailBlockMode = eHdcpFailBlockMode;
	//s_eHdcpNoDeviceBlockMode = eHdcpNoDeviceBlockMode;

#if defined(CONFIG_MW_OUTPUT_SCART)
	//PAL_SCART_SetTvFormat(eTvScartFormat);
	//PAL_SCART_SetVcrFormat(eVcrScartFormat);
#endif

	return ERR_OK;
}
#endif

void irldvt_output_SetInitialHdmiAudioTransCode(void)
{
	HERROR	hErr;

	hErr = PAL_OUT_SetHdmiAudioTransCode(eDxAUDIO_CODEC_DOLBY_AC3, eDxAUDIO_CODEC_DOLBY_AC3);
	hErr |= PAL_OUT_SetHdmiAudioTransCode(eDxAUDIO_CODEC_DOLBY_AC3P, eDxAUDIO_CODEC_DOLBY_AC3);

	hErr |= PAL_OUT_SetHdmiAudioTransCode(eDxAUDIO_CODEC_AAC, eDxAUDIO_CODEC_DOLBY_AC3);
	hErr |= PAL_OUT_SetHdmiAudioTransCode(eDxAUDIO_CODEC_AAC_LOAS, eDxAUDIO_CODEC_DOLBY_AC3);
	hErr |= PAL_OUT_SetHdmiAudioTransCode(eDxAUDIO_CODEC_AAC_PLUS, eDxAUDIO_CODEC_DOLBY_AC3);
	hErr |= PAL_OUT_SetHdmiAudioTransCode(eDxAUDIO_CODEC_AAC_PLUS_ADTS, eDxAUDIO_CODEC_DOLBY_AC3);

	if (ERR_OK != hErr)
	{
		HxLOG_Warning("IRLDVT_SVC_OUT_SetHdmiAudioTransCode() Error!!! [%d]\n", hErr);
	}
}

void irldvt_output_SetInitialSpdifAudioTransCode(void)
{
	PAL_OUT_SetSpdifAudioTransCode(eDxAUDIO_CODEC_DOLBY_AC3, eDxAUDIO_CODEC_DOLBY_AC3);
	PAL_OUT_SetSpdifAudioTransCode(eDxAUDIO_CODEC_DOLBY_AC3P, eDxAUDIO_CODEC_DOLBY_AC3);

	PAL_OUT_SetSpdifAudioTransCode(eDxAUDIO_CODEC_AAC, eDxAUDIO_CODEC_DOLBY_AC3);
	PAL_OUT_SetSpdifAudioTransCode(eDxAUDIO_CODEC_AAC_LOAS, eDxAUDIO_CODEC_DOLBY_AC3);
	PAL_OUT_SetSpdifAudioTransCode(eDxAUDIO_CODEC_AAC_PLUS, eDxAUDIO_CODEC_DOLBY_AC3);
	PAL_OUT_SetSpdifAudioTransCode(eDxAUDIO_CODEC_AAC_PLUS_ADTS, eDxAUDIO_CODEC_DOLBY_AC3);
}

HERROR IRLDVT_OUT_SetBackgroundColor(HUINT8 ucR, HUINT8 ucG, HUINT8 ucB)
{
	HERROR				hErr;

	hErr = PAL_SCALER_SetBackgroundColor(ucR, ucG, ucB);

	//IRLDVT_DB_PARAM_SetItem(eDB_PARAM_ITEM_BACKGROUND_COLOR_R, (HUINT32)ucR, 0);
	//IRLDVT_DB_PARAM_SetItem(eDB_PARAM_ITEM_BACKGROUND_COLOR_G, (HUINT32)ucG, 0);
	//IRLDVT_DB_PARAM_SetItem(eDB_PARAM_ITEM_BACKGROUND_COLOR_B, (HUINT32)ucB, 0);

	return hErr;
}

HUINT32 irldvt_output_GetInitialVolume(void)
{
	HERROR		hErr;
	HUINT32		ulVolume = 0;

	//hErr = IRLDVT_DB_PARAM_Getitem(eDB_PARAM_ITEM_LAST_VOLUME, (HUINT32 *)&ulVolume, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("IRLDVT_DB_PARAM_Getitem(eDB_PARAM_ITEM_SPDIF_AUDIO_FORMAT) Error!!!\n");
	}

	return ulVolume;
}


void IRLDVT_OUTPUT_Init(void)
{
	//BUS_Result_t	eRes;
	HUINT32			i;
	HUINT32 		ulLipSyncDealy = 0;
/*
	IRLDVT_OUT_SetInitParameters(TRUE,
						eVIDEO_BLANK_SPEC_WSS,
						eDxCOMPOSITE_STANDARD_PAL,
						eDxDFC_SELECT_DEFAULT,
						eDxRESOLUTION_SELECT_1080P,
						eDxASPECTRATIO_16X9,
						eDxHDMI_3DSELECT_AUTO,
						FALSE,
						FALSE,
						FALSE,
						eHDCP_BLOCK_AVMUTE,
						eHDCP_BLOCK_AVMUTE,
						eDxSCART_FORMAT_RGB,
						eDxSCART_FORMAT_RGB,
						eDxDIGITALAUDIO_FORMAT_MULTICHANNEL,
						eDxDIGITALAUDIO_FORMAT_MULTICHANNEL
						);
*/
	/*IRLDVT_OUT_SetInitParameters(TRUE,
						eVIDEO_BLANK_SPEC_WSS,
						eDxCOMPOSITE_STANDARD_PAL,
						irldvt_output_GetInitialDfcSelect(),
						irldvt_output_GetInitialResolutionSelect(),
						irldvt_output_GetInitialTvAspectRatio(),
						irldvt_output_GetInitial3DMode(),
						FALSE,
						FALSE,
						FALSE,
						eHDCP_BLOCK_AVMUTE,
						eHDCP_BLOCK_AVMUTE,
						irldvt_output_GetInitialTvScartFormat(),
						irldvt_output_GetInitialVcrScartFormat(),
						irldvt_output_GetInitialHdmiAudioOutFormat(),
						irldvt_output_GetInitialSpdifAudioOutFormat()
						);*/

	PAL_SCALER_SetInitParameters(eDxCOMPOSITE_STANDARD_PAL,
							eDxDFC_SELECT_DEFAULT,
							eDxRESOLUTION_SELECT_1080P,
							eDxASPECTRATIO_16X9,
							eDxHDMI_3DSELECT_AUTO,
							FALSE,
							FALSE,
							FALSE
							);

	irldvt_output_SetInitialHdmiAudioTransCode();
	irldvt_output_SetInitialSpdifAudioTransCode();

	IRLDVT_OUT_SetBackgroundColor(0, 0, 0);


	PAL_SCALER_SyncVideoSetting();

}
