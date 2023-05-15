/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_ch_ter.c
	@brief	  Octo pal channel digital terrestrial part

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <pal_channel.h>

#include <di_channel.h>
#include <di_channel_t.h>
#include <di_err.h>

/*******************************************************************/
/************************     Definition   * ***********************/
/*******************************************************************/



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/

#define ______________ConvDI2PAL__________________
static DxTer_CodeRate_e pal_ch_ter_ConvDI2PAL_CodeRate(CH_TER_CodeRate_e etCodeRate)
{
	switch (etCodeRate)
	{
		case DI_CH_TCODERATE_1_2:	return eDxTER_CODERATE_1_2;
		case DI_CH_TCODERATE_2_3:	return eDxTER_CODERATE_2_3;
		case DI_CH_TCODERATE_3_4:	return eDxTER_CODERATE_3_4;
		case DI_CH_TCODERATE_5_6:	return eDxTER_CODERATE_5_6;
		case DI_CH_TCODERATE_7_8:	return eDxTER_CODERATE_7_8;
		case DI_CH_TCODERATE_AUTO:	return eDxTER_CODERATE_AUTO;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
}

static DxTer_TransMode_e pal_ch_ter_ConvDI2PAL_TransMode(CH_TER_TransMode_e etTransMode)
{
	switch (etTransMode)
	{
		case DI_CH_TRANSMODE_2K:		return eDxTER_TRANSMODE_2K;
		case DI_CH_TRANSMODE_8K:		return eDxTER_TRANSMODE_8K;
		case DI_CH_T2TRANSMODE_1K:		return eDxTER_T2TRANSMODE_1K;
		case DI_CH_T2TRANSMODE_4K:		return eDxTER_T2TRANSMODE_4K;
		case DI_CH_T2TRANSMODE_16K:		return eDxTER_T2TRANSMODE_16K;
		case DI_CH_T2TRANSMODE_32K:		return eDxTER_T2TRANSMODE_32K;
		case DI_CH_TRANSMODE_AUTO:		return eDxTER_TRANSMODE_AUTO;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
}

static DxTer_GuardInterval_e pal_ch_ter_ConvDI2PAL_GuardInterval(CH_TER_GuardInterval_e etGuardInterval)
{
	switch (etGuardInterval)
	{
		case DI_CH_GUARDINTERVAL_AUTO:		return eDxTER_GUARDINTERVAL_AUTO;
		case DI_CH_GUARDINTERVAL_1_4:		return eDxTER_GUARDINTERVAL_1_4;
		case DI_CH_GUARDINTERVAL_1_8:		return eDxTER_GUARDINTERVAL_1_8;
		case DI_CH_GUARDINTERVAL_1_16:		return eDxTER_GUARDINTERVAL_1_16;
		case DI_CH_GUARDINTERVAL_1_32:		return eDxTER_GUARDINTERVAL_1_32;
		case DI_CH_T2GUARDINTERVAL_1_128:		return eDxTER_T2GUARDINTERVAL_1_128;
		case DI_CH_T2GUARDINTERVAL_19_128:		return eDxTER_T2GUARDINTERVAL_19_128;
		case DI_CH_T2GUARDINTERVAL_19_256:		return eDxTER_T2GUARDINTERVAL_19_256;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
}

static DxTer_Constellation_e pal_ch_ter_ConvDI2PAL_Constellation(CH_TER_Moudulation_e etModulation)
{
	switch (etModulation)
	{
		case DI_CH_TMOD_QPSK:		return eDxTER_CONSTELLATION_QPSK;
		case DI_CH_TMOD_16QAM:		return eDxTER_CONSTELLATION_16QAM;
		case DI_CH_TMOD_64QAM:		return eDxTER_CONSTELLATION_64QAM;
		case DI_CH_TMOD_256QAM: 	return eDxTER_CONSTELLATION_256QAM;
		case DI_CH_TMOD_AUTO:		return eDxTER_CONSTELLATION_AUTO;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
}

static DxTer_Hierachy_e pal_ch_ter_ConvDI2PAL_Hierachy(CH_TER_Hierarchy_e etHierarchy)
{
	switch (etHierarchy)
	{
		case DI_CH_HIERARCHY_NONE:		return eDxTER_HIERACHY_NONE;
		case DI_CH_HIERARCHY_1:			return eDxTER_HIERACHY_1;
		case DI_CH_HIERARCHY_2:			return eDxTER_HIERACHY_2;
		case DI_CH_HIERARCHY_4:			return eDxTER_HIERACHY_4;
		case DI_CH_HIERARCHY_AUTO:		return eDxTER_HIERACHY_AUTO;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
}

static DxTer_Offset_e pal_ch_ter_ConvDI2PAL_Offset(CH_TER_Offset_e etOffset)
{
	switch (etOffset)
	{
		case DI_CH_OFFSET_NONE:			return eDxTER_OFFSET_NONE;
		case DI_CH_OFFSET_1_6_P:		return eDxTER_OFFSET_1_6_P;
		case DI_CH_OFFSET_1_6_N:		return eDxTER_OFFSET_1_6_N;
		case DI_CH_OFFSET_2_6_P:		return eDxTER_OFFSET_2_6_P;
		case DI_CH_OFFSET_2_6_N:		return eDxTER_OFFSET_2_6_N;
		case DI_CH_OFFSET_3_6_P:		return eDxTER_OFFSET_3_6_P;
		case DI_CH_OFFSET_3_6_N:		return eDxTER_OFFSET_3_6_N;
		case DI_CH_OFFSET_AUTO:			return eDxTER_OFFSET_AUTO;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
}


static DxTer_BandWidth_e pal_ch_ter_ConvDI2PAL_BandWidth(CH_TER_BandWidth_e etBandWidth)
{
	switch (etBandWidth)
	{
		case DI_CH_BW_6MHZ:		return eDxTER_BANDWIDTH_6M;
		case DI_CH_BW_7MHZ:		return eDxTER_BANDWIDTH_7M;
		case DI_CH_BW_8MHZ:		return eDxTER_BANDWIDTH_8M;
		case DI_CH_BW_5MHZ:		return eDxTER_T2BANDWIDTH_5MHZ;
		case DI_CH_BW_10MHZ:	return eDxTER_T2BANDWIDTH_10MHZ;
		case DI_CH_BW_1P7MHZ:	return eDxTER_T2BANDWIDTH_1P7MHZ;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
}


static DxTer_System_e pal_ch_ter_ConvDI2PAL_System(CH_TER_System_e system)
{
#if defined(CONFIG_MW_CH_DVB_T2)
	switch (system)
	{
		case DI_CH_DVBT:		return eDxTER_DVB_T;
		case DI_CH_DVBT2:		return eDxTER_DVB_T2;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
#else
	return eDxTER_DVB_T;
#endif
}

static DxTer_PreambleFormat_e pal_ch_ter_ConvDI2PAL_PreambleFormat(CH_TER_PreambleFormat_e preambleFormat)
{
	switch (preambleFormat)
	{
		case DI_CH_T2SISO:	return eDxTER_SISO;
		case DI_CH_T2MISO:	return eDxTER_MISO;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
}

static DxTer_Mixed_e pal_ch_ter_ConvDI2PAL_PreambleMixed(CH_TER_PreambleMixed_e mixed)
{
	switch (mixed)
	{
		case DI_CH_T2NOT_MIXED:			return eDxTER_NOT_MIXED;
		case DI_CH_T2MIXED:				return eDxTER_MIXED;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
}

static DxTer_Pilot_Pattern_e pal_ch_ter_ConvDI2PAL_PilotPattern(CH_TER_PilotPattern_e pilotPattern)
{
	switch (pilotPattern)
	{
		case DI_CH_T2_PP1:		return eDxTER_PP1;
		case DI_CH_T2_PP2:		return eDxTER_PP2;
		case DI_CH_T2_PP3:		return eDxTER_PP3;
		case DI_CH_T2_PP4:		return eDxTER_PP4;
		case DI_CH_T2_PP5:		return eDxTER_PP5;
		case DI_CH_T2_PP6:		return eDxTER_PP6;
		case DI_CH_T2_PP7:		return eDxTER_PP7;
		case DI_CH_T2_PP8:		return eDxTER_PP8;
		case DI_CH_T2_PPAUTO:	return eDxTER_PP_AUTO;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
}

static DxTer_ExtendedCarrier_e pal_ch_ter_ConvDI2PAL_ExtendedCarrier(CH_TER_ExtendedCarrier_e extendedCarrier)
{
	switch (extendedCarrier)
	{
		case DI_CH_T2NOT_USED:		return eDxTER_T2NOT_USED;
		case DI_CH_T2USED:			return eDxTER_T2USED;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
}

static DxTer_PAPR_Reduction_e pal_ch_ter_ConvDI2PAL_PAPRreduction(CH_TER_PaprMode_e PAPRreduction)
{
	switch (PAPRreduction)
	{
		case DI_CH_NONE_PAPR:		return eDxTER_REDUCTION_NONE;
		case DI_CH_ACE_PAPR:		return eDxTER_ACE;
		case DI_CH_TR_PAPR:			return eDxTER_TR;
		case DI_CH_ACETR_PAPR:		return eDxTER_AVE_TR;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
}

#define _____________ConvPAL2DI__________________
static CH_TER_CodeRate_e pal_ch_ter_ConvPAL2DI_CodeRate(DxTer_CodeRate_e codeRate)
{
#if defined(CONFIG_MW_CH_ISDB_T)
	return DI_CH_TCODERATE_AUTO;
#else
	switch (codeRate)
	{
		case eDxTER_CODERATE_1_2:	return DI_CH_TCODERATE_1_2;
		case eDxTER_CODERATE_2_3:	return DI_CH_TCODERATE_2_3;
		case eDxTER_CODERATE_3_4:	return DI_CH_TCODERATE_3_4;
		case eDxTER_CODERATE_5_6:	return DI_CH_TCODERATE_5_6;
		case eDxTER_CODERATE_7_8:	return DI_CH_TCODERATE_7_8;
		case eDxTER_CODERATE_AUTO:	return DI_CH_TCODERATE_AUTO;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
#endif
}

static CH_TER_TransMode_e pal_ch_ter_ConvPAL2DI_TransMode(DxTer_TransMode_e transMode)
{
#if defined(CONFIG_MW_CH_ISDB_T)
	return DI_CH_TRANSMODE_AUTO;
#else
	switch (transMode)
	{
		case eDxTER_TRANSMODE_AUTO:			return DI_CH_TRANSMODE_AUTO;
		case eDxTER_TRANSMODE_2K:				return DI_CH_TRANSMODE_2K;
		case eDxTER_TRANSMODE_8K:				return DI_CH_TRANSMODE_8K;
		case eDxTER_T2TRANSMODE_1K:			return DI_CH_T2TRANSMODE_1K;
		case eDxTER_T2TRANSMODE_4K:			return DI_CH_T2TRANSMODE_4K;
		case eDxTER_T2TRANSMODE_16K:			return DI_CH_T2TRANSMODE_16K;
		case eDxTER_T2TRANSMODE_32K:			return DI_CH_T2TRANSMODE_32K;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
#endif
}

static CH_TER_GuardInterval_e pal_ch_ter_ConvPAL2DI_GuardInterval(DxTer_GuardInterval_e guardInterval)
{
#if defined(CONFIG_MW_CH_ISDB_T)
	return DI_CH_GUARDINTERVAL_AUTO;
#else
	switch (guardInterval)
	{
		case eDxTER_GUARDINTERVAL_AUTO:			return DI_CH_GUARDINTERVAL_AUTO;
		case eDxTER_GUARDINTERVAL_1_4:				return DI_CH_GUARDINTERVAL_1_4;
		case eDxTER_GUARDINTERVAL_1_8:				return DI_CH_GUARDINTERVAL_1_8;
		case eDxTER_GUARDINTERVAL_1_16:			return DI_CH_GUARDINTERVAL_1_16;
		case eDxTER_GUARDINTERVAL_1_32:			return DI_CH_GUARDINTERVAL_1_32;
		case eDxTER_T2GUARDINTERVAL_1_128:			return DI_CH_T2GUARDINTERVAL_1_128;
		case eDxTER_T2GUARDINTERVAL_19_128:		return DI_CH_T2GUARDINTERVAL_19_128;
		case eDxTER_T2GUARDINTERVAL_19_256:		return DI_CH_T2GUARDINTERVAL_19_256;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
#endif
}

static CH_TER_Moudulation_e pal_ch_ter_ConvPAL2DI_Constellation(DxTer_Constellation_e constellation)
{
#if defined(CONFIG_MW_CH_ISDB_T)
	return DI_CH_TMOD_64QAM;
#else
	switch (constellation)
	{
		case eDxTER_CONSTELLATION_QPSK:	return DI_CH_TMOD_QPSK;
		case eDxTER_CONSTELLATION_16QAM:	return DI_CH_TMOD_16QAM;
		case eDxTER_CONSTELLATION_64QAM:	return DI_CH_TMOD_64QAM;
		case eDxTER_CONSTELLATION_256QAM:	return DI_CH_TMOD_256QAM;
		case eDxTER_CONSTELLATION_AUTO:	return DI_CH_TMOD_AUTO;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
#endif
}

static CH_TER_Hierarchy_e pal_ch_ter_ConvPAL2DI_Hierachy(DxTer_Hierachy_e hierachy)
{
#if defined(CONFIG_MW_CH_ISDB_T)
	return DI_CH_HIERARCHY_AUTO;
#else
	switch (hierachy)
	{
		case eDxTER_HIERACHY_NONE:		return DI_CH_HIERARCHY_NONE;
		case eDxTER_HIERACHY_1:		return DI_CH_HIERARCHY_1;
		case eDxTER_HIERACHY_2:		return DI_CH_HIERARCHY_2;
		case eDxTER_HIERACHY_4:		return DI_CH_HIERARCHY_4;
		case eDxTER_HIERACHY_AUTO:		return DI_CH_HIERARCHY_AUTO;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
#endif
}

static CH_TER_Offset_e pal_ch_ter_ConvPAL2DI_Offset(DxTer_Offset_e offset)
{
#if defined(CONFIG_MW_CH_ISDB_T)
	return DI_CH_OFFSET_AUTO;
#else
	switch (offset)
	{
		case eDxTER_OFFSET_NONE:	return DI_CH_OFFSET_NONE;
		case eDxTER_OFFSET_1_6_P:	return DI_CH_OFFSET_1_6_P;
		case eDxTER_OFFSET_1_6_N:	return DI_CH_OFFSET_1_6_N;
		case eDxTER_OFFSET_2_6_P:	return DI_CH_OFFSET_2_6_P;
		case eDxTER_OFFSET_2_6_N:	return DI_CH_OFFSET_2_6_N;
		case eDxTER_OFFSET_3_6_P:	return DI_CH_OFFSET_3_6_P;
		case eDxTER_OFFSET_3_6_N:	return DI_CH_OFFSET_3_6_N;
		case eDxTER_OFFSET_AUTO:	return DI_CH_OFFSET_AUTO;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
#endif
}

static CH_TER_Stream_e pal_ch_ter_ConvPAL2DI_Stream(DxTer_Stream_e stream)
{
#if defined(CONFIG_MW_CH_ISDB_T)
	return DI_CH_STREAM_HP;
#else
	switch (stream)
	{
		case eDxTER_STREAM_HP:	return DI_CH_STREAM_HP;
		case eDxTER_STREAM_LP:	return DI_CH_STREAM_LP;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
#endif
}

static CH_TER_BandWidth_e pal_ch_ter_ConvPAL2DI_BandWidth(DxTer_BandWidth_e bandWidth)
{
#if defined(CONFIG_MW_CH_ISDB_T)
	return DI_CH_BW_6MHZ;
#else
	switch (bandWidth)
	{
		case eDxTER_BANDWIDTH_6M:	return DI_CH_BW_6MHZ;
		case eDxTER_BANDWIDTH_7M:	return DI_CH_BW_7MHZ;
		case eDxTER_BANDWIDTH_8M:	return DI_CH_BW_8MHZ;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
#endif
}


static CH_TER_System_e pal_ch_ter_ConvPAL2DI_System(DxTer_System_e system)
{
#if defined(CONFIG_MW_CH_DVB_T2)
	switch (system)
	{
		case eDxTER_DVB_T:		return DI_CH_DVBT;
		case eDxTER_DVB_T2:	return DI_CH_DVBT2;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
#else
	return DI_CH_DVBT;
#endif
}

static CH_TER_PreambleFormat_e pal_ch_ter_ConvPAL2DI_PreambleFormat(DxTer_PreambleFormat_e preambleFormat)
{
	switch (preambleFormat)
	{
		case eDxTER_SISO:	return DI_CH_T2SISO;
		case eDxTER_MISO:	return DI_CH_T2MISO;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
}

static CH_TER_PreambleMixed_e pal_ch_ter_ConvPAL2DI_PreambleMixed(DxTer_Mixed_e mixed)
{
	switch (mixed)
	{
		case eDxTER_NOT_MIXED:			return DI_CH_T2NOT_MIXED;
		case eDxTER_MIXED:				return DI_CH_T2MIXED;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
}


static CH_TER_PilotPattern_e pal_ch_ter_ConvPAL2DI_PilotPattern(DxTer_Pilot_Pattern_e pilotPattern)
{
	switch (pilotPattern)
	{
		case eDxTER_PP1:		return DI_CH_T2_PP1;
		case eDxTER_PP2:		return DI_CH_T2_PP2;
		case eDxTER_PP3:		return DI_CH_T2_PP3;
		case eDxTER_PP4:		return DI_CH_T2_PP4;
		case eDxTER_PP5:		return DI_CH_T2_PP5;
		case eDxTER_PP6:		return DI_CH_T2_PP6;
		case eDxTER_PP7:		return DI_CH_T2_PP7;
		case eDxTER_PP8:		return DI_CH_T2_PP8;
		case eDxTER_PP_AUTO:	return DI_CH_T2_PPAUTO;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
}


static CH_TER_ExtendedCarrier_e pal_ch_ter_ConvPAL2DI_ExtendedCarrier(DxTer_ExtendedCarrier_e extendedCarrier)
{
	switch (extendedCarrier)
	{
		case eDxTER_T2NOT_USED:		return DI_CH_T2NOT_USED;
		case eDxTER_T2USED:			return DI_CH_T2USED;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
}

static CH_TER_PaprMode_e pal_ch_ter_ConvPAL2DI_PAPRreduction(DxTer_PAPR_Reduction_e PAPRreduction)
{
	switch (PAPRreduction)
	{
		case eDxTER_REDUCTION_NONE:	return DI_CH_NONE_PAPR;
		case eDxTER_ACE:				return DI_CH_ACE_PAPR;
		case eDxTER_TR:				return DI_CH_TR_PAPR;
		case eDxTER_AVE_TR:			return DI_CH_ACETR_PAPR;
		default:
			HxLOG_Critical("\n\n");
			return 0xff;
	}
}


#define __________________EXTERN_API___________________

#if defined(CONFIG_DEBUG)

HUINT8 *pal_ch_ter_getBandWidthStr(CH_TER_BandWidth_e etBandWidth)
{
	switch (etBandWidth)
	{
		case DI_CH_BW_8MHZ: return "DI_CH_BW_8MHZ";
		case DI_CH_BW_7MHZ: return "DI_CH_BW_7MHZ";
		case DI_CH_BW_6MHZ: return "DI_CH_BW_6MHZ";
		default: return "UNKNOWN VALUE";
	}
}

HUINT8 *pal_ch_ter_getModulationStr(CH_TER_Moudulation_e etModulation)
{
	switch (etModulation)
	{
		case DI_CH_TMOD_AUTO: return "DI_CH_TMOD_AUTO";
		case DI_CH_TMOD_QPSK: return "DI_CH_TMOD_QPSK";
		case DI_CH_TMOD_16QAM: return "DI_CH_TMOD_16QAM";
		case DI_CH_TMOD_64QAM: return "DI_CH_TMOD_64QAM";
		default: return "UNKNOWN VALUE";
	}
}
HUINT8 *pal_ch_ter_getHierachyStr(CH_TER_Hierarchy_e etHierarchy)
{
	switch (etHierarchy)
	{
		case DI_CH_HIERARCHY_AUTO: return "DI_CH_HIERARCHY_AUTO";
		case DI_CH_HIERARCHY_NONE: return "DI_CH_HIERARCHY_NONE";
		case DI_CH_HIERARCHY_1: return "DI_CH_HIERARCHY_1";
		case DI_CH_HIERARCHY_2: return "DI_CH_HIERARCHY_2";
		case DI_CH_HIERARCHY_4: return "DI_CH_HIERARCHY_4";
		default: return "UNKNOWN VALUE";
	}
}

HUINT8 *pal_ch_ter_getCodeRateStr(CH_TER_CodeRate_e etCodeRate)
{
	switch (etCodeRate)
	{
		case DI_CH_TCODERATE_AUTO: return "DI_CH_TCODERATE_AUTO";
		case DI_CH_TCODERATE_1_2: return "DI_CH_TCODERATE_1_2";
		case DI_CH_TCODERATE_2_3: return "DI_CH_TCODERATE_2_3";
		case DI_CH_TCODERATE_3_4: return "DI_CH_TCODERATE_3_4";
		case DI_CH_TCODERATE_5_6: return "DI_CH_TCODERATE_5_6";
		case DI_CH_TCODERATE_7_8: return "DI_CH_TCODERATE_7_8";
		default: return "UNKNOWN VALUE";
	}
}

HUINT8 *pal_ch_ter_getTransModeStr(CH_TER_TransMode_e etTransMode)
{
	switch (etTransMode)
	{
		case DI_CH_TRANSMODE_AUTO: return "DI_CH_TRANSMODE_AUTO";
		case DI_CH_TRANSMODE_2K: return "DI_CH_TRANSMODE_2K";
		case DI_CH_TRANSMODE_8K: return "DI_CH_TRANSMODE_8K";
		default: return "UNKNOWN VALUE";
	}
}

HUINT8 *pal_ch_ter_getGuardIntervalStr(CH_TER_GuardInterval_e etGuardInterval)
{
	switch (etGuardInterval)
	{
		case DI_CH_GUARDINTERVAL_AUTO: return "DI_CH_GUARDINTERVAL_AUTO";
		case DI_CH_GUARDINTERVAL_1_32: return "DI_CH_GUARDINTERVAL_1_32";
		case DI_CH_GUARDINTERVAL_1_16: return "DI_CH_GUARDINTERVAL_1_16";
		case DI_CH_GUARDINTERVAL_1_8: return "DI_CH_GUARDINTERVAL_1_8";
		case DI_CH_GUARDINTERVAL_1_4: return "DI_CH_GUARDINTERVAL_1_4";
		default: return "UNKNOWN VALUE";
	}
}

HUINT8 *pal_ch_ter_getStreamStr(CH_TER_Stream_e etStream)
{
	switch (etStream)
	{
		case DI_CH_STREAM_HP: return "DI_CH_STREAM_HP";
		case DI_CH_STREAM_LP: return "DI_CH_STREAM_LP";
		default: return "UNKNOWN VALUE";
	}
}

HUINT8 *pal_ch_ter_getOffSetStr(CH_TER_Offset_e etOffset)
{
	switch (etOffset)
	{
		case DI_CH_OFFSET_AUTO: return "DI_CH_OFFSET_AUTO";
		case DI_CH_OFFSET_NONE: return "DI_CH_OFFSET_NONE";
		case DI_CH_OFFSET_1_6_P: return "DI_CH_OFFSET_1_6_P";
		case DI_CH_OFFSET_1_6_N: return "DI_CH_OFFSET_1_6_N";
		case DI_CH_OFFSET_2_6_P: return "DI_CH_OFFSET_2_6_P";
		case DI_CH_OFFSET_2_6_N: return "DI_CH_OFFSET_2_6_N";
		case DI_CH_OFFSET_3_6_P: return "DI_CH_OFFSET_3_6_P";
		case DI_CH_OFFSET_3_6_N: return "DI_CH_OFFSET_3_6_N";
		default: return "UNKNOWN VALUE";
	}
}


HUINT8 *pal_ch_ter_getTerSystem(CH_TER_System_e etTerSystem)
{
	switch (etTerSystem)
	{
		case DI_CH_DVBT: return "DI_CH_DVBT";
		case DI_CH_DVBT2: return "DI_CH_DVBT2";
		default: return "UNKNOWN VALUE";
	}
}

HUINT8 *pal_ch_ter_getPreambleFormat(CH_TER_PreambleFormat_e etPreambleFormat)
{
	switch (etPreambleFormat)
	{
		case DI_CH_T2SISO: return "DI_CH_T2SISO";
		case DI_CH_T2MISO: return "DI_CH_T2MISO";
		default: return "UNKNOWN VALUE";
	}
}


HUINT8 *pal_ch_ter_getPreambleMixed(CH_TER_PreambleMixed_e etPreambleMixed)
{
	switch (etPreambleMixed)
	{
		case DI_CH_T2NOT_MIXED: return "DI_CH_T2NOT_MIXED";
		case DI_CH_T2MIXED: return "DI_CH_T2MIXED";
		default: return "UNKNOWN VALUE";
	}
}

HUINT8 *pal_ch_ter_getPilotPattern(CH_TER_PilotPattern_e etPilotPattern)
{
	switch (etPilotPattern)
	{
		case DI_CH_T2_PPAUTO: return "DI_CH_T2_PPAUTO";
		case DI_CH_T2_PP1: return "DI_CH_T2_PP1";
		case DI_CH_T2_PP2: return "DI_CH_T2_PP2";
		case DI_CH_T2_PP3: return "DI_CH_T2_PP3";
		case DI_CH_T2_PP4: return "DI_CH_T2_PP4";
		case DI_CH_T2_PP5: return "DI_CH_T2_PP5";
		case DI_CH_T2_PP6: return "DI_CH_T2_PP6";
		case DI_CH_T2_PP7: return "DI_CH_T2_PP7";
		case DI_CH_T2_PP8: return "DI_CH_T2_PP8";
		default: return "UNKNOWN VALUE";
	}
}


HUINT8 *pal_ch_ter_getExtendedCarrier(CH_TER_ExtendedCarrier_e etExtendedCarrier)
{
	switch (etExtendedCarrier)
	{
		case DI_CH_T2NOT_USED: return "DI_CH_T2NOT_USED";
		case DI_CH_T2USED: return "DI_CH_T2USED";
		default: return "UNKNOWN VALUE";
	}
}

HUINT8 *pal_ch_ter_getPaprMode(CH_TER_PaprMode_e etPaprMode)
{
	switch (etPaprMode)
	{
		case DI_CH_NONE_PAPR: 	return "DI_CH_NONE_PAPR";
		case DI_CH_ACE_PAPR:		return "DI_CH_ACE_PAPR";
		case DI_CH_TR_PAPR:		return "DI_CH_TR_PAPR";
		case DI_CH_ACETR_PAPR:	return "DI_CH_ACETR_PAPR";
		default: return "UNKNOWN VALUE";
	}
}

#endif
/****************************************************************************
********************************* Extern API's *********************************
****************************************************************************/
HERROR PAL_CH_TER_StartTune(
							HUINT32					ulTunerGroupId,
							HUINT32					ulRequestId,
							HUINT32					ulFrequency,
							DxTer_BandWidth_e			bandWidth,
							DxTer_Constellation_e		constellation,
							DxTer_Hierachy_e			hierachy,
							DxTer_CodeRate_e			codeRate,
							DxTer_TransMode_e			transMode,
							DxTer_GuardInterval_e		guardInterval,
							DxTer_Stream_e			stream,
							DxTer_Offset_e			offset,
							DxTer_System_e			system,
							DxTer_PreambleFormat_e	preambleFormat,
							DxTer_Mixed_e				mixed,
							DxTer_Pilot_Pattern_e		pilotPattern,
							DxTer_ExtendedCarrier_e	extenedCarrier,
							DxTer_PAPR_Reduction_e	PAPRreduction,
							HUINT32					ulPlpId,
							HUINT32					numPlpBlocks
						)
{
	CH_TER_TuneParam_t		stDiParam;
	DI_CHANNEL_ERR			err;

	HxLOG_Info("PAL_CH_TER_StartTune()\n");

	VK_MEM_Memset(&stDiParam, 0, sizeof(CH_TER_TuneParam_t)); /* 이거 안하면 garbage 때문에 다른 튜닝 파라미터로 인식함. */

/* 튜닝 파라미터를 DI용으로 변역하고... */
	stDiParam.ulFrequency		= ulFrequency; /* KHz */
	stDiParam.etBandWidth		= pal_ch_ter_ConvPAL2DI_BandWidth(bandWidth);
	stDiParam.etModulation 		= pal_ch_ter_ConvPAL2DI_Constellation(constellation);
	stDiParam.etHierarchy		= pal_ch_ter_ConvPAL2DI_Hierachy(hierachy);
	stDiParam.etCodeRate		= pal_ch_ter_ConvPAL2DI_CodeRate(codeRate);
	stDiParam.etTransMode		= pal_ch_ter_ConvPAL2DI_TransMode(transMode);
	stDiParam.etGuardInterval	= pal_ch_ter_ConvPAL2DI_GuardInterval(guardInterval);
	stDiParam.etStream 			= pal_ch_ter_ConvPAL2DI_Stream(stream);
	stDiParam.etOffset 			= pal_ch_ter_ConvPAL2DI_Offset(offset);
	stDiParam.etTerSystem					= pal_ch_ter_ConvPAL2DI_System(system);
	stDiParam.stT2Param.etPreambleFormat	= pal_ch_ter_ConvPAL2DI_PreambleFormat(preambleFormat);
	stDiParam.stT2Param.etPreambleMixed		= pal_ch_ter_ConvPAL2DI_PreambleMixed(mixed);
	stDiParam.stT2Param.etPilotPattern		= pal_ch_ter_ConvPAL2DI_PilotPattern(pilotPattern);
	stDiParam.stT2Param.etExtendedCarrier	= pal_ch_ter_ConvPAL2DI_ExtendedCarrier(extenedCarrier);
	stDiParam.stT2Param.etPaprMode			= pal_ch_ter_ConvPAL2DI_PAPRreduction(PAPRreduction);
	stDiParam.stT2Param.ulPlpId				= ulPlpId;
	stDiParam.stT2Param.ulPlpNumBlocks		= numPlpBlocks;

#ifdef CONFIG_DEBUG
	{
		HxLOG_Print("===================================================\n");
		HxLOG_Print("   TER T2 TUNE PARAM\n");
		HxLOG_Print("===================================================\n");
		HxLOG_Print("   ulFrequency      [%d]\n", stDiParam.ulFrequency);
		HxLOG_Print("   Transmission     [%d]\n", system);
		HxLOG_Print("   etBandWidth      [%s]\n", pal_ch_ter_getBandWidthStr(stDiParam.etBandWidth));
		HxLOG_Print("   etModulation     [%s]\n", pal_ch_ter_getModulationStr(stDiParam.etModulation));
		HxLOG_Print("   etHierarchy      [%s]\n", pal_ch_ter_getHierachyStr(stDiParam.etHierarchy));
		HxLOG_Print("   etCodeRate       [%s]\n", pal_ch_ter_getCodeRateStr(stDiParam.etCodeRate));
		HxLOG_Print("   etTransMode      [%s]\n", pal_ch_ter_getTransModeStr(stDiParam.etTransMode));
		HxLOG_Print("   etGuardInterval  [%s]\n", pal_ch_ter_getGuardIntervalStr(stDiParam.etGuardInterval));
		HxLOG_Print("   etStream         [%s]\n", pal_ch_ter_getStreamStr(stDiParam.etStream));
		HxLOG_Print("   etOffset         [%s]\n", pal_ch_ter_getOffSetStr(stDiParam.etOffset));

		HxLOG_Print("   etTerSystem         [%s]\n", pal_ch_ter_getTerSystem(stDiParam.etTerSystem));
		HxLOG_Print("   ulPlpId         	[%d]\n", stDiParam.stT2Param.ulPlpId);
		HxLOG_Print("   etPreambleFormat 	[%s]\n", pal_ch_ter_getPreambleFormat(stDiParam.stT2Param.etPreambleFormat));
		HxLOG_Print("   etPreambleMixed     [%s]\n", pal_ch_ter_getPreambleMixed(stDiParam.stT2Param.etPreambleMixed));
		HxLOG_Print("   etPilotPattern        [%s]\n", pal_ch_ter_getPilotPattern(stDiParam.stT2Param.etPilotPattern));
		HxLOG_Print("   etExtendedCarrier    [%s]\n", pal_ch_ter_getExtendedCarrier(stDiParam.stT2Param.etExtendedCarrier));
		HxLOG_Print("   etPaprMode         [%s]\n", pal_ch_ter_getPaprMode(stDiParam.stT2Param.etPaprMode));
		HxLOG_Print("   ulPlpNumBlocks      [%d]\n", stDiParam.stT2Param.ulPlpNumBlocks);

		HxLOG_Print("===================================================\n");
	}
#endif

/* DI를 부른다... */
	err = DI_CH_T_RequestLock(ulTunerGroupId, ulRequestId, &stDiParam);
	if(err != DI_CH_OK)
	{
		HxLOG_Error("Error from DI_CH_T_RequestLock() : %d\n", err);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR PAL_CH_TER_StopTune(HUINT32 ulTunerGroupId)
{
	DI_CHANNEL_ERR	err;

	HxLOG_Info("PAL_CH_TER_StopTune()\n");

	err = DI_CH_T_CancelLock(ulTunerGroupId);
	if(err != DI_CH_OK)
	{
		HxLOG_Error("Error from DI_CH_T_CancelLock() : %d\n", err);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR PAL_CH_TER_GetLockedInfo(
								HUINT32					ulTunerGroupId,
								HUINT32 				*pulActualFreq, // KHz
								DxTer_BandWidth_e 		*pActualBandWidth,
								DxTer_Constellation_e 	*pActualConstellation,
								DxTer_Hierachy_e			*pActualHierachy,
								DxTer_CodeRate_e			*pActualCodeRate,
								DxTer_TransMode_e 		*pActualTransMode,
								DxTer_GuardInterval_e 	*pActualGuardInterval,
								DxTer_Offset_e			*pActualOffset,
								DxTer_System_e			*pSystem,
								DxTer_PreambleFormat_e	*pPreambleFormat,
								DxTer_Mixed_e				*pMixed,
								DxTer_Pilot_Pattern_e		*pPilotPattern,
								DxTer_ExtendedCarrier_e	*pExtenedCarrier,
								DxTer_PAPR_Reduction_e	*pPAPRreduction,
								HUINT32					*pPlpId,
								HUINT32					*pNumPlpBlocks
								)
{
	CH_TER_TuneParam_t		stDiParam;
	DI_CHANNEL_ERR			err;

	err = DI_CH_T_GetLockedInfo(ulTunerGroupId, &stDiParam);
	if(err != DI_CH_OK)
	{
		HxLOG_Error("Error from DI_CH_T_GetLockedInfo() : %d\n", err);
		return ERR_FAIL;
	}

	*pulActualFreq			= stDiParam.ulFrequency; /* kHz */
	*pActualBandWidth		= pal_ch_ter_ConvDI2PAL_BandWidth(stDiParam.etBandWidth);
	*pActualConstellation	= pal_ch_ter_ConvDI2PAL_Constellation(stDiParam.etModulation);
	*pActualHierachy		= pal_ch_ter_ConvDI2PAL_Hierachy(stDiParam.etHierarchy);
	*pActualCodeRate		= pal_ch_ter_ConvDI2PAL_CodeRate(stDiParam.etCodeRate);
	*pActualTransMode		= pal_ch_ter_ConvDI2PAL_TransMode(stDiParam.etTransMode);
	*pActualGuardInterval	= pal_ch_ter_ConvDI2PAL_GuardInterval(stDiParam.etGuardInterval);
	*pActualOffset			= pal_ch_ter_ConvDI2PAL_Offset(stDiParam.etOffset);
	*pSystem				= pal_ch_ter_ConvDI2PAL_System(stDiParam.etTerSystem);
	*pPreambleFormat		= pal_ch_ter_ConvDI2PAL_PreambleFormat(stDiParam.stT2Param.etPreambleFormat);
	*pMixed					= pal_ch_ter_ConvDI2PAL_PreambleMixed(stDiParam.stT2Param.etPreambleMixed);
	*pPilotPattern			= pal_ch_ter_ConvDI2PAL_PilotPattern(stDiParam.stT2Param.etPilotPattern);
	*pExtenedCarrier		= pal_ch_ter_ConvDI2PAL_ExtendedCarrier(stDiParam.stT2Param.etExtendedCarrier);
	*pPAPRreduction			= pal_ch_ter_ConvDI2PAL_PAPRreduction(stDiParam.stT2Param.etPaprMode);
	//MPLP : Do not update *pPlpId					= stDiParam.stT2Param.ulPlpId;
	*pNumPlpBlocks			= stDiParam.stT2Param.ulPlpNumBlocks;

	return ERR_OK;
}

HERROR PAL_CH_TER_GetDataPlpInfo( HUINT32 ulTunerGroupId,  HUINT8 *pucPlpIds, HUINT8 *pucNumPlps )
{
	DI_CHANNEL_ERR			err;
	HUINT8					ucMPLPCnt;

	err = DI_CH_T_GetDataPlpInfo(ulTunerGroupId, pucPlpIds,&ucMPLPCnt);
	if(err != DI_CH_OK)
	{
		return ERR_FAIL;
	}

	if( ucMPLPCnt != (*pucNumPlps))
		return ERR_FAIL;

	HxLOG_Print("\n\n######## PAL_CH_TER_GetDataPlpInfo()  (*pucNumPlps) : %d\n",(*pucNumPlps));
{
	HUINT8 i;
	for(i=0; i < (*pucNumPlps); i++)
	{
		HxLOG_Print("#### [PAL_CH_TER_GetDataPlpInfo()]  (%d) pucPlpIds [%d] \n",i,pucPlpIds[i]);
	}

}
	return ERR_OK;
}

HERROR PAL_CH_TER_GetDataPlpCount( HUINT32 ulTunerGroupId,  HUINT8 *pucNumPlps )
{
	DI_CHANNEL_ERR			err;

	err = DI_CH_T_GetDataPlpCount(ulTunerGroupId, pucNumPlps);
	if(err != DI_CH_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}


HERROR PAL_CH_TER_GetSignalInfo(
								HUINT32			ulTunerGroupId,
								HUINT32			*pulSignalLevel,
								HUINT32			*pulSignalQuality,
								HUINT32			*pulAgc,
								float			*pfBer,
								float			*pfSnr,
								float			*pfSignalInputPower
								)
{
	DI_CHANNEL_ERR			err_DI;
	DI_CH_SignalQuality_t 	stSignalInfo;

	err_DI = DI_CH_T_GetSignalInfo(ulTunerGroupId, &stSignalInfo);
	if(err_DI == DI_CH_OK)
	{
		*pulSignalLevel		= stSignalInfo.ulStrength;
		*pulSignalQuality	= stSignalInfo.ulQuality;
		*pulAgc				= stSignalInfo.ulAgc;
		*pfBer				= stSignalInfo.fBer;
		*pfSnr				= stSignalInfo.fSnr;
		*pfSignalInputPower	= stSignalInfo.fSignalInputPower;

		return ERR_OK;
	}
	else
	{
		*pulSignalLevel		= 0;
		*pulSignalQuality	= 0;
		*pulAgc				= 0;
		*pfBer				= 0;
		*pfSnr				= 0;
		*pfSignalInputPower	= 0;

		return ERR_FAIL;
	}
}

void PAL_CH_TER_SetAntennaPower(HUINT32 ulTunerGroupId, HBOOL bOnOff)
{
	DI_CHANNEL_ERR		err;

	HxLOG_Info("PAL_CH_TER_SetAntennaPower(%d)\n", bOnOff);

	if(bOnOff == TRUE)
	{
		err = DI_CH_T_SetAntennaPower(ulTunerGroupId, DI_CH_ANT_POWER_ON);
		if(err != DI_CH_OK)			HxLOG_Critical("Critical Eror!!!\n");
	}
	else
	{
		err = DI_CH_T_SetAntennaPower(ulTunerGroupId, DI_CH_ANT_POWER_OFF);
		if(err != DI_CH_OK)			HxLOG_Critical("Critical Eror!!!\n");
	}
}

HERROR PAL_CH_TER_GetAntennaPower(HUINT32 ulTunerGroupId, HBOOL *pbOnOff)
{
	DI_CHANNEL_ERR		err;
	DI_CH_AntStatus_e    eAntStatus;

	HxLOG_Info("PAL_CH_TER_GetAntennaPower\n");

	if (pbOnOff == NULL)
		return ERR_FAIL;

	*pbOnOff = FALSE;

	err = DI_CH_T_GetAntennaPower(ulTunerGroupId, &eAntStatus);
	if(err != DI_CH_OK)
	{
		HxLOG_Critical("Critical Eror!!!\n");
		return ERR_FAIL;
	}

	if (eAntStatus == DI_CH_ANTENNA_ON)
		*pbOnOff = TRUE;

	return ERR_OK;
}

static HERROR pal_ch_ter_MakeTerTuneParam(
							HUINT32					ulFrequency,
							DxTer_BandWidth_e			bandWidth,
							DxTer_Constellation_e		constellation,
							DxTer_Hierachy_e			hierachy,
							DxTer_CodeRate_e			codeRate,
							DxTer_TransMode_e			transMode,
							DxTer_GuardInterval_e		guardInterval,
							DxTer_Stream_e			stream,
							DxTer_Offset_e			offset,
							DxTer_System_e			system,
							DxTer_PreambleFormat_e	preambleFormat,
							DxTer_Mixed_e				mixed,
							DxTer_Pilot_Pattern_e		pilotPattern,
							DxTer_ExtendedCarrier_e	extenedCarrier,
							DxTer_PAPR_Reduction_e	PAPRreduction,
							HUINT32					ulPlpId,
							HUINT32					numPlpBlocks,
							CH_TER_TuneParam_t		*pstDiParam
							)
{
	pstDiParam->ulFrequency			= ulFrequency;
	pstDiParam->etBandWidth			= pal_ch_ter_ConvPAL2DI_BandWidth(bandWidth);
	pstDiParam->etModulation 		= pal_ch_ter_ConvPAL2DI_Constellation(constellation);
	pstDiParam->etHierarchy			= pal_ch_ter_ConvPAL2DI_Hierachy(hierachy);
	pstDiParam->etCodeRate			= pal_ch_ter_ConvPAL2DI_CodeRate(codeRate);
	pstDiParam->etTransMode			= pal_ch_ter_ConvPAL2DI_TransMode(transMode);
	pstDiParam->etGuardInterval		= pal_ch_ter_ConvPAL2DI_GuardInterval(guardInterval);
	pstDiParam->etStream 			= pal_ch_ter_ConvPAL2DI_Stream(stream);
	pstDiParam->etOffset 			= pal_ch_ter_ConvPAL2DI_Offset(offset);

	pstDiParam->etTerSystem						= pal_ch_ter_ConvPAL2DI_System(system);
	pstDiParam->stT2Param.etPreambleFormat		= pal_ch_ter_ConvPAL2DI_PreambleFormat(preambleFormat);
	pstDiParam->stT2Param.etPreambleMixed		= pal_ch_ter_ConvPAL2DI_PreambleMixed(mixed);
	pstDiParam->stT2Param.etPilotPattern		= pal_ch_ter_ConvPAL2DI_PilotPattern(pilotPattern);
	pstDiParam->stT2Param.etExtendedCarrier		= pal_ch_ter_ConvPAL2DI_ExtendedCarrier(extenedCarrier);
	pstDiParam->stT2Param.etPaprMode			= pal_ch_ter_ConvPAL2DI_PAPRreduction(PAPRreduction);
	pstDiParam->stT2Param.ulPlpId				= ulPlpId;
	pstDiParam->stT2Param.ulPlpNumBlocks		= numPlpBlocks;

	return ERR_OK;
}

HERROR PAL_CH_TER_MakeTerTuneParam(
							HUINT32					ulFrequency,
							DxTer_BandWidth_e			bandWidth,
							DxTer_Constellation_e		constellation,
							DxTer_Hierachy_e			hierachy,
							DxTer_CodeRate_e			codeRate,
							DxTer_TransMode_e			transMode,
							DxTer_GuardInterval_e		guardInterval,
							DxTer_Stream_e			stream,
							DxTer_Offset_e			offset,
							DxTer_System_e			system,
							HUINT32 				plpId,
							DxTer_PreambleFormat_e	preambleFormat,
							DxTer_Mixed_e				mixed,
							DxTer_Pilot_Pattern_e		pilotPattern,
							DxTer_ExtendedCarrier_e	extenedCarrier,
							DxTer_PAPR_Reduction_e	pAPRreduction,
							HUINT32					numPlpBlocks,
							HUINT32					*pulDataSize,
							void					**ppvDiParam
							)
{
	CH_TER_TuneParam_t	*pstDiParam;
	HERROR				 hErr;

	if (pulDataSize == NULL || ppvDiParam == NULL)
	{
		return ERR_FAIL;
	}

	pstDiParam = (CH_TER_TuneParam_t *)OxMW_Malloc (sizeof(CH_TER_TuneParam_t));
	if (pstDiParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = pal_ch_ter_MakeTerTuneParam (ulFrequency, bandWidth, constellation, hierachy, codeRate, transMode,
										guardInterval, stream, offset, system, preambleFormat, mixed,
										pilotPattern,extenedCarrier,pAPRreduction,plpId,numPlpBlocks,
										pstDiParam);

	if (hErr != ERR_OK)
	{
		OxMW_Free (pstDiParam);
		return ERR_FAIL;
	}

	*pulDataSize = (HUINT32)sizeof(CH_TER_TuneParam_t);
	*ppvDiParam  = (void *)pstDiParam;

	return ERR_OK;
}

HERROR PAL_CH_TER_GetTunerCellId(HUINT32 ulTunerGroupId, HUINT16 *pCellId)
{
	DI_CHANNEL_ERR	err = DI_CH_OK;
	unsigned short	usCellId;

	if (pCellId == NULL)
	{
		return ERR_FAIL;
	}

	*pCellId = 0;

	err = DI_CH_T_GetCellId(ulTunerGroupId, &usCellId);
	if (err != DI_CH_OK)
	{
		HxLOG_Critical("Critical Eror!!!\n");
		return ERR_FAIL;
	}

	*pCellId = (HUINT16) usCellId;

	return ERR_OK;

}

