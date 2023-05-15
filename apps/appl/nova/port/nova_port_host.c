/**************************************************************
*	http://www.humaxdigital.com
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

/**************************************************************************************************/
#define _________Nova_Private_Include_________________________________________________
/**************************************************************************************************/
#include <nova_port.h>
#include "tuner_api.h"

/**************************************************************************************************/
#define _________Nova_Private_Macro_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________Nova_Private_Struct_________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define _________Nova_Private_Value_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________Nova_Private_Prototype_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________Nova_Private_functions________________________________________________
/**************************************************************************************************/
static DxSat_CodeRate_e			Convert_eFec		(NovaspreadTTunerCodeRate CodeRate);
static DxSat_Pilot_e			Convert_ePilot		(NovaspreadTTunerPilotTones PilotTones);
static DxSat_Polarization_e		Convert_ePol		(NovaspreadTTunerPolarization Polarization);
static DxSat_PskModulation_e	Convert_ePskMod		(NovaspreadTTunerModulation Modulation);
static DxSat_RollOff_e			Convert_eRollOff	(NovaspreadTTunerRollOff RollOff);
static DxSat_TransportSpec_e	Convert_eTransSpec	(NovaspreadTTunerModulationSystem ModulationSystem);


/**************************************************************************************************/
#define _________Nova_Public_Functions__________________________________________________________
/**************************************************************************************************/
NovaspreadTTunerReleaseRequestListener g_fnReleaseRequestListener = NULL;



PUBLIC NovaspreadTTunerRequestId NovaspreadHostAllocateTuner(const NovaspreadTTunerAllocationParameters * aTunerAllocationParameter, NovaspreadTTunerAllocationFinishedListener  aAllocationFinishedListener )
{
	void* pTuner;
	TUNER_PARAM_t TunerParam;

	if (aTunerAllocationParameter->TunerParameters.Type != NOVASPREAD_TUNER_TYPE_DVB_S)
	{
		printf("Not supported tuner type:%d", aTunerAllocationParameter->TunerParameters.Type);
		return 0;
	}

	memset(&TunerParam,0, sizeof(TunerParam));

	// convert aTunerAllocationParameter to TunerParam
	TunerParam.eFec			= Convert_eFec		(aTunerAllocationParameter->TunerParameters.Value.DvbS.CodeRate);
	TunerParam.ePilot		= Convert_ePilot	(aTunerAllocationParameter->TunerParameters.Value.DvbS.PilotTones);
	TunerParam.ePol			= Convert_ePol		(aTunerAllocationParameter->TunerParameters.Value.DvbS.Polarization);
	TunerParam.ePskMod		= Convert_ePskMod	(aTunerAllocationParameter->TunerParameters.Value.DvbS.Modulation);
	TunerParam.eRollOff		= Convert_eRollOff	(aTunerAllocationParameter->TunerParameters.Value.DvbS.RollOff);
	TunerParam.eTransSpec	= Convert_eTransSpec(aTunerAllocationParameter->TunerParameters.Value.DvbS.ModulationSystem);
	TunerParam.eTunerType	= TT_SAT;
	TunerParam.Freq			= aTunerAllocationParameter->TunerParameters.Value.DvbS.Frequency / 1000;
	TunerParam.Src			= aTunerAllocationParameter->TunerParameters.Value.DvbS.SignalSource;
	TunerParam.SymbolRate	= aTunerAllocationParameter->TunerParameters.Value.DvbS.SymbolRate;

	TunerParam.Mode			= aTunerAllocationParameter->AllocationMode;
	TunerParam.Priority		= aTunerAllocationParameter->Priority;

#ifdef USE_TRANSCODING
	TunerParam.UseTranscoder = aTunerAllocationParameter->AllocateTranscoder;
#else
	TunerParam.UseTranscoder = false;
#endif

	if (TunerParam.UseTranscoder == true)
	{
		memcpy(&TunerParam.Transcoding, &aTunerAllocationParameter->Transcoding, sizeof(aTunerAllocationParameter->Transcoding));
	}

	TunerParam.UseCrypto	 = aTunerAllocationParameter->AllocateTranscryptor;
	memcpy(&TunerParam.DvbId, &aTunerAllocationParameter->DvbId, sizeof(NovaspreadTDvbId));


#ifdef CONFIG_DEBUG
	printf(HxANSI_COLOR_YELLOW("REQ PARAM:\n"));
	printf(HxANSI_COLOR_YELLOW(" - Priority    :%d\n"), aTunerAllocationParameter->Priority);
	printf(HxANSI_COLOR_YELLOW(" - Mode        :%d\n"), aTunerAllocationParameter->AllocationMode);
	printf(HxANSI_COLOR_YELLOW(" - Transcoder  :%d\n"), aTunerAllocationParameter->AllocateTranscoder);
	printf(HxANSI_COLOR_YELLOW(" - Transcryptor:%d\n"), aTunerAllocationParameter->AllocateTranscryptor);

	printf(HxANSI_COLOR_YELLOW("TP PARAM:\n"));
	printf(HxANSI_COLOR_YELLOW(" - CodeRate        :%d\n"), aTunerAllocationParameter->TunerParameters.Value.DvbS.CodeRate);
	printf(HxANSI_COLOR_YELLOW(" - Frequency       :%d\n"), aTunerAllocationParameter->TunerParameters.Value.DvbS.Frequency);
	printf(HxANSI_COLOR_YELLOW(" - FrontendId      :%d\n"), aTunerAllocationParameter->TunerParameters.Value.DvbS.FrontendId);
	printf(HxANSI_COLOR_YELLOW(" - Modulation      :%d\n"), aTunerAllocationParameter->TunerParameters.Value.DvbS.Modulation);
	printf(HxANSI_COLOR_YELLOW(" - ModulationSystem:%d\n"), aTunerAllocationParameter->TunerParameters.Value.DvbS.ModulationSystem);
	printf(HxANSI_COLOR_YELLOW(" - OrbitalPosition :%d\n"), aTunerAllocationParameter->TunerParameters.Value.DvbS.OrbitalPosition);
	printf(HxANSI_COLOR_YELLOW(" - PilotTones      :%d\n"), aTunerAllocationParameter->TunerParameters.Value.DvbS.PilotTones);
	printf(HxANSI_COLOR_YELLOW(" - Polarization    :%d\n"), aTunerAllocationParameter->TunerParameters.Value.DvbS.Polarization);
	printf(HxANSI_COLOR_YELLOW(" - RollOff         :%d\n"), aTunerAllocationParameter->TunerParameters.Value.DvbS.RollOff);
	printf(HxANSI_COLOR_YELLOW(" - SignalSource    :%d\n"), aTunerAllocationParameter->TunerParameters.Value.DvbS.SignalSource);
	printf(HxANSI_COLOR_YELLOW(" - SymbolRate      :%d\n"), aTunerAllocationParameter->TunerParameters.Value.DvbS.SymbolRate);

	printf(HxANSI_COLOR_YELLOW("DVB ID:\n"));
	printf(HxANSI_COLOR_YELLOW(" - OriNetworkId      :%d\n"), aTunerAllocationParameter->DvbId.OriginalNetworkId);
	printf(HxANSI_COLOR_YELLOW(" - ServiceId         :%d\n"), aTunerAllocationParameter->DvbId.ServiceId);
	printf(HxANSI_COLOR_YELLOW(" - TransportStreamId :%d\n"), aTunerAllocationParameter->DvbId.TransportStreamId);

	printf(HxANSI_COLOR_YELLOW("TranscodingParam:\n"));
	printf(HxANSI_COLOR_YELLOW(" - AudioBitrate    :%d\n"), aTunerAllocationParameter->Transcoding.AudioBitrate);
	printf(HxANSI_COLOR_YELLOW(" - AudioCodec      :%d\n"), aTunerAllocationParameter->Transcoding.AudioCodec);
	printf(HxANSI_COLOR_YELLOW(" - VideoBitrate    :%d\n"), aTunerAllocationParameter->Transcoding.VideoBitrate);
	printf(HxANSI_COLOR_YELLOW(" - VideoCodec      :%d\n"), aTunerAllocationParameter->Transcoding.VideoCodec);
	printf(HxANSI_COLOR_YELLOW(" - VideoResolution :%d\n"), aTunerAllocationParameter->Transcoding.VideoResolution);
#endif

	pTuner =  TUNERAPI_AllocateTuner(TunerParam, aAllocationFinishedListener);
	if (pTuner == NULL)
	{
		HxLOG_Error(HxANSI_COLOR_RED("TUNERAPI_AllocateTuner failed\n"));
		return 0;
	}

	return TUNERAPI_GetTunerRequestID(pTuner);
}

PUBLIC NovaspreadTBoolean NovaspreadHostSetTunerReleaseRequestListener(NovaspreadTTunerReleaseRequestListener aReleaseRequestListener )
{
	printf(HxANSI_COLOR_PURPLE("[%s] Register TunerReleaseRequestListener Callback\n"), __FUNCTION__);
	g_fnReleaseRequestListener = aReleaseRequestListener;
	return TRUE;
}

PUBLIC void NovaspreadTunerRelease ( NovaspreadTTuner aTuner )
{
	NOVA_ErrCode_e ErrCode;

	ErrCode = TUNERAPI_ReleaseTuner(aTuner);
	if (ErrCode != NERR_OK)
	{
		HxLOG_Error("TUNERAPI_AllocateTuner failed(Err:%d)\n", ErrCode);
	}
}

PUBLIC void NovaspreadHostCancelAllocation ( NovaspreadTUInt32 aRequestId )
{
	void* pTuner;
	NOVA_ErrCode_e ErrCode;

	if (aRequestId == 0)
	{
		printf("Invalid aRequestId(%d)\n", aRequestId);
		return;
	}

	pTuner = (void*)aRequestId;
	ErrCode = TUNERAPI_ReleaseTuner(pTuner);
	if (ErrCode != NERR_OK)
	{
		HxLOG_Error("TUNERAPI_AllocateTuner failed(Err:%d)\n", ErrCode);
	}
}


// =============== [Private APIs] =======================================================
static DxSat_CodeRate_e Convert_eFec(NovaspreadTTunerCodeRate CodeRate)
{
	switch(CodeRate)
	{
	case NOVASPREAD_TUNER_CODE_RATE_1_2: return eDxSAT_CODERATE_1_2;
	case NOVASPREAD_TUNER_CODE_RATE_2_3: return eDxSAT_CODERATE_2_3;
	case NOVASPREAD_TUNER_CODE_RATE_3_4: return eDxSAT_CODERATE_3_4;
	case NOVASPREAD_TUNER_CODE_RATE_3_5: return eDxSAT_CODERATE_3_5;
	case NOVASPREAD_TUNER_CODE_RATE_4_5: return eDxSAT_CODERATE_4_5;
	case NOVASPREAD_TUNER_CODE_RATE_5_6: return eDxSAT_CODERATE_5_6;
	case NOVASPREAD_TUNER_CODE_RATE_6_7: return eDxSAT_CODERATE_6_7;
	case NOVASPREAD_TUNER_CODE_RATE_7_8: return eDxSAT_CODERATE_7_8;
	case NOVASPREAD_TUNER_CODE_RATE_8_9: return eDxSAT_CODERATE_8_9;
	case NOVASPREAD_TUNER_CODE_RATE_9_10: return eDxSAT_CODERATE_9_10;
	case NOVASPREAD_TUNER_CODE_RATE_AUTO: return eDxSAT_CODERATE_UNKNOWN;
	case NOVASPREAD_TUNER_CODE_RATE_1_3: return eDxSAT_CODERATE_UNKNOWN;
	case NOVASPREAD_TUNER_CODE_RATE_1_4: return eDxSAT_CODERATE_UNKNOWN;
	case NOVASPREAD_TUNER_CODE_RATE_2_5: return eDxSAT_CODERATE_UNKNOWN;
	default:							return eDxSAT_CODERATE_UNKNOWN;
	}
}

static DxSat_Pilot_e Convert_ePilot(NovaspreadTTunerPilotTones PilotTones)
{
	switch(PilotTones)
	{
	case NOVASPREAD_TUNER_PILOT_TONES_ON:		return eDxSAT_PILOT_ON;
	case NOVASPREAD_TUNER_PILOT_TONES_OFF:		return eDxSAT_PILOT_OFF;
	case NOVASPREAD_TUNER_PILOT_TONES_UNKNOWN:	return eDxSAT_PILOT_UNKNOWN;
	default:									return eDxSAT_PILOT_UNKNOWN;
	}
}

static DxSat_Polarization_e Convert_ePol(NovaspreadTTunerPolarization Polarization)
{
	switch(Polarization)
	{
	case NOVASPREAD_TUNER_POLARIZATION_H:	return eDxSAT_POLAR_HOR;
	case NOVASPREAD_TUNER_POLARIZATION_V:	return eDxSAT_POLAR_VER;
	default:								return eDxSAT_POLAR_UNKNOWN;
	}
}

static DxSat_PskModulation_e Convert_ePskMod(NovaspreadTTunerModulation Modulation)
{
	switch(Modulation)
	{
	case NOVASPREAD_TUNER_MODULATION_QPSK:	return eDxSAT_PSK_QPSK;
	case NOVASPREAD_TUNER_MODULATION_8PSK:	return eDxSAT_PSK_8PSK;
	case NOVASPREAD_TUNER_MODULATION_QAM_16:return eDxSAT_PSK_UNKOWN;
	case NOVASPREAD_TUNER_MODULATION_QAM_32:return eDxSAT_PSK_UNKOWN;
	case NOVASPREAD_TUNER_MODULATION_QAM_64:return eDxSAT_PSK_UNKOWN;
	case NOVASPREAD_TUNER_MODULATION_QAM_128:return eDxSAT_PSK_UNKOWN;
	case NOVASPREAD_TUNER_MODULATION_QAM_256:return eDxSAT_PSK_UNKOWN;
	case NOVASPREAD_TUNER_MODULATION_UNKNOWN:return eDxSAT_PSK_UNKOWN;
	case NOVASPREAD_TUNER_MODULATION_AUTO:	return eDxSAT_PSK_UNKOWN;
	default:								return eDxSAT_PSK_UNKOWN;
	}
}

static DxSat_RollOff_e Convert_eRollOff(NovaspreadTTunerRollOff RollOff)
{
	switch(RollOff)
	{
	case NOVASPREAD_TUNER_ROLL_OFF_0_20:	return eDxSAT_ROLL_020;
	case NOVASPREAD_TUNER_ROLL_OFF_0_25:	return eDxSAT_ROLL_025;
	case NOVASPREAD_TUNER_ROLL_OFF_0_35:	return eDxSAT_ROLL_035;
	case NOVASPREAD_TUNER_ROLL_OFF_UNKNOWN:	return eDxSAT_ROLL_UNKNOWN;
	default:								return eDxSAT_ROLL_UNKNOWN;
	}
}

static DxSat_TransportSpec_e Convert_eTransSpec(NovaspreadTTunerModulationSystem ModulationSystem)
{
	switch(ModulationSystem)
	{
	case NOVASPREAD_TUNER_MODULATION_SYSTEM_DVB_S:	return eDxSAT_TRANS_DVBS;
	case NOVASPREAD_TUNER_MODULATION_SYSTEM_DVB_S2:	return eDxSAT_TRANS_DVBS2;
	default:										return eDxSAT_TRANS_UNKNOWN;
	}
}

/**************************************************************************************************/
#define _________Nova__End_Of_File____________________________________________________
/**************************************************************************************************/


