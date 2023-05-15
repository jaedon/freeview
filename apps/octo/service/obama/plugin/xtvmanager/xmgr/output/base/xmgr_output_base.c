#include <octo_common.h>

#include <db_param.h>

#include <svc_output.h>


DxDfcSelect_e xmgr_output_GetInitialDfcSelect_BASE(void)
{
	HERROR				hErr;
	DxDfcSelect_e		eDfcSelect = eDxDFC_SELECT_MAX_NUM;

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_DFC_SELECT, (HUINT32 *)&eDfcSelect, 0);
	if (ERR_OK != hErr)
	{
		eDfcSelect = eDxDFC_SELECT_MAX_NUM;
	}

	return eDfcSelect;
}

DxResolutionSelect_e xmgr_output_GetInitialResolutionSelect_BASE(void)
{
	HERROR					hErr;
	DxResolutionSelect_e	eResolutionSelect = eDxRESOLUTION_SELECT_MAX_NUM;

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_RESOLUTION_SELECT, (HUINT32 *)&eResolutionSelect, 0);
	if (ERR_OK != hErr)
	{
		eResolutionSelect = eDxRESOLUTION_SELECT_MAX_NUM;
	}
#if defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM) //CVC_DEMO
	eResolutionSelect = eDxRESOLUTION_SELECT_1080P;
#endif
	return eResolutionSelect;
}

DxAspectRatio_e xmgr_output_GetInitialTvAspectRatio_BASE(void)
{
	HERROR				hErr;
	DxAspectRatio_e		eTvAspectRatio = eDxASPECTRATIO_MAX_NUM;

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_TV_ASPECTRATIO, (HUINT32 *)&eTvAspectRatio, 0);
	if (ERR_OK != hErr)
	{
		eTvAspectRatio = eDxASPECTRATIO_MAX_NUM;
	}

	return eTvAspectRatio;
}

DxHdmi3DSelect_e xmgr_output_GetInitial3DMode_BASE(void)
{
	HERROR				hErr;
	DxHdmi3DSelect_e	e3DSelect = eDxHDMI_3DSELECT_OFF;

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_3D_SELECT, (HUINT32 *)&e3DSelect, 0);
	if (ERR_OK != hErr)
	{
		e3DSelect = eDxHDMI_3DSELECT_MAX_NUM;
	}

	return e3DSelect;
}

DxScartFormat_e xmgr_output_GetInitialTvScartFormat_BASE(void)
{
	HERROR				hErr;
	DxScartFormat_e		eTvScartFormat = eDxSCART_FORMAT_UNKNOWN;

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_TVSCART_FORMAT, (HUINT32 *)&eTvScartFormat, 0);
	if (ERR_OK != hErr)
	{
		eTvScartFormat = eDxSCART_FORMAT_UNKNOWN;
	}

	return eTvScartFormat;
}

DxScartFormat_e xmgr_output_GetInitialVcrScartFormat_BASE(void)
{
	HERROR				hErr;
	DxScartFormat_e		eVcrScartFormat = eDxSCART_FORMAT_UNKNOWN;

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_VCRSCART_FORMAT, (HUINT32 *)&eVcrScartFormat, 0);
	if (ERR_OK != hErr)
	{
		eVcrScartFormat = eDxSCART_FORMAT_UNKNOWN;
	}

	return eVcrScartFormat;
}

HUINT32 xmgr_output_GetInitialVolume_BASE(void)
{
	HERROR		hErr;
	HUINT32		ulVolume = 0;

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LAST_VOLUME, (HUINT32 *)&ulVolume, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LAST_VOLUME) Error!!!\n");
	}

	return ulVolume;
}

HUINT32 xmgr_output_GetInitialDecoderVolume_BASE(void)
{
	HERROR		hErr;
	HUINT32		ulVolume = 0;

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LAST_DECODER_VOLUME, (HUINT32 *)&ulVolume, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LAST_DECODER_VOLUME) Error!!!\n");
	}

	return ulVolume;
}

void xmgr_output_SetInitialHdmiAudioTransCode_BASE(void)
{
	HERROR	hErr;

	hErr = SVC_OUT_SetHdmiAudioTransCode(eDxAUDIO_CODEC_DOLBY_AC3, eDxAUDIO_CODEC_DOLBY_AC3);
	hErr |= SVC_OUT_SetHdmiAudioTransCode(eDxAUDIO_CODEC_DOLBY_AC3P, eDxAUDIO_CODEC_DOLBY_AC3);

	hErr |= SVC_OUT_SetHdmiAudioTransCode(eDxAUDIO_CODEC_AAC, eDxAUDIO_CODEC_DOLBY_AC3);
	hErr |= SVC_OUT_SetHdmiAudioTransCode(eDxAUDIO_CODEC_AAC_LOAS, eDxAUDIO_CODEC_DOLBY_AC3);
	hErr |= SVC_OUT_SetHdmiAudioTransCode(eDxAUDIO_CODEC_AAC_PLUS, eDxAUDIO_CODEC_DOLBY_AC3);
	hErr |= SVC_OUT_SetHdmiAudioTransCode(eDxAUDIO_CODEC_AAC_PLUS_ADTS, eDxAUDIO_CODEC_DOLBY_AC3);

	if (ERR_OK != hErr)
	{
		HxLOG_Warning("SVC_OUT_SetHdmiAudioTransCode() Error!!! [%d]\n", hErr);
	}
}

DxDigitalAudioFormat_e xmgr_output_GetInitialHdmiAudioOutFormat_BASE(void)
{
	DxDigitalAudioFormat_e		eDigitalAudioFormat;
	HERROR						hErr;

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_HDMI_AUDIO_FORMAT, (HUINT32 *)&eDigitalAudioFormat, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_HDMI_AUDIO_FORMAT) Error!!!\n");
		eDigitalAudioFormat = eDxDIGITALAUDIO_FORMAT_UNKNOWN;
	}

	return eDigitalAudioFormat;
}

void xmgr_output_SetInitialSpdifAudioTransCode_BASE(void)
{
	SVC_OUT_SetSpdifAudioTransCode(eDxAUDIO_CODEC_DOLBY_AC3, eDxAUDIO_CODEC_DOLBY_AC3);
	SVC_OUT_SetSpdifAudioTransCode(eDxAUDIO_CODEC_DOLBY_AC3P, eDxAUDIO_CODEC_DOLBY_AC3);

	SVC_OUT_SetSpdifAudioTransCode(eDxAUDIO_CODEC_AAC, eDxAUDIO_CODEC_DOLBY_AC3);
	SVC_OUT_SetSpdifAudioTransCode(eDxAUDIO_CODEC_AAC_LOAS, eDxAUDIO_CODEC_DOLBY_AC3);
	SVC_OUT_SetSpdifAudioTransCode(eDxAUDIO_CODEC_AAC_PLUS, eDxAUDIO_CODEC_DOLBY_AC3);
	SVC_OUT_SetSpdifAudioTransCode(eDxAUDIO_CODEC_AAC_PLUS_ADTS, eDxAUDIO_CODEC_DOLBY_AC3);
}

DxDigitalAudioFormat_e xmgr_output_GetInitialSpdifAudioOutFormat_BASE(void)
{
	DxDigitalAudioFormat_e		eDigitalAudioFormat;
	HERROR						hErr;

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_SPDIF_AUDIO_FORMAT, (HUINT32 *)&eDigitalAudioFormat, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_SPDIF_AUDIO_FORMAT) Error!!!\n");
		eDigitalAudioFormat = eDxDIGITALAUDIO_FORMAT_UNKNOWN;
	}

	return eDigitalAudioFormat;
}

HBOOL xmgr_output_SupportTvScart_BASE(void)
{
#if defined (CONFIG_MW_OUTPUT_SCART_TV)
	return TRUE;
#else
	return FALSE;
#endif
}

