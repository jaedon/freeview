/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */

/* chip manufacture's  header files */

/* humax header files */
#include "fdi_common.h"
#include "fdi_channel.h"

#include "di_err.h"
#include "di_channel.h"
#include "di_channel_t.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */

static unsigned long s_ulRequestId;


void INTRN_FDI_CHANNEL_T_ConvFDItoDI(FDI_CH_TER_TUNE_PARAM_t *ptFDIParam, CH_TER_TuneParam_t *ptDIParam)
{

	ptDIParam->ulFrequency = ptFDIParam->ulFrequency; /*unit KHz*/

	/*  etBandWidth  */
	switch( ptFDIParam->etBandWidth )
	{
		case FDI_CH_BW_8MHZ :
			ptDIParam->etBandWidth = DI_CH_BW_8MHZ;
			break;
		case FDI_CH_BW_7MHZ :
			ptDIParam->etBandWidth = DI_CH_BW_7MHZ;
			break;
		case FDI_CH_BW_6MHZ :
			ptDIParam->etBandWidth = DI_CH_BW_6MHZ;
			break;
		case FDI_CH_BW_5MHZ :
			ptDIParam->etBandWidth = DI_CH_BW_5MHZ;
			break;
		case FDI_CH_BW_10MHZ :
			ptDIParam->etBandWidth = DI_CH_BW_10MHZ;
			break;
		case FDI_CH_BW_1P7MHZ :
			ptDIParam->etBandWidth = DI_CH_BW_1P7MHZ;
			break;
		default :
			ptDIParam->etBandWidth = DI_CH_BW_8MHZ;
	}

	/*  etCodeRate  */
	switch( ptFDIParam->etCodeRate )
	{
		case FDI_CH_TCODERATE_AUTO :
			ptDIParam->etCodeRate = DI_CH_TCODERATE_AUTO;
			break;
		case FDI_CH_TCODERATE_1_2 :
			ptDIParam->etCodeRate = DI_CH_TCODERATE_1_2;
			break;
		case FDI_CH_TCODERATE_2_3 :
			ptDIParam->etCodeRate = DI_CH_TCODERATE_2_3;
			break;
		case FDI_CH_TCODERATE_3_4 :
			ptDIParam->etCodeRate = DI_CH_TCODERATE_3_4;
			break;
		case FDI_CH_TCODERATE_5_6 :
			ptDIParam->etCodeRate = DI_CH_TCODERATE_5_6;
			break;
		case FDI_CH_TCODERATE_7_8 :
			ptDIParam->etCodeRate = DI_CH_TCODERATE_7_8;
			break;
		case FDI_CH_T2CODERATE_3_5 :
			ptDIParam->etCodeRate = DI_CH_T2CODERATE_3_5;
			break;
		case FDI_CH_T2CODERATE_4_5 :
			ptDIParam->etCodeRate = DI_CH_T2CODERATE_4_5;
			break;
		default :
			ptDIParam->etCodeRate = DI_CH_TCODERATE_AUTO;
	}
	
	/*  etGuardInterval  */
	switch( ptFDIParam->etGuardInterval )
	{
		case FDI_CH_GUARDINTERVAL_AUTO :
			ptDIParam->etGuardInterval = DI_CH_GUARDINTERVAL_AUTO;
			break;
		case FDI_CH_GUARDINTERVAL_1_32 :
			ptDIParam->etGuardInterval = DI_CH_GUARDINTERVAL_1_32;
			break;
		case FDI_CH_GUARDINTERVAL_1_16 :
			ptDIParam->etGuardInterval = DI_CH_GUARDINTERVAL_1_16;
			break;
		case FDI_CH_GUARDINTERVAL_1_8 :
			ptDIParam->etGuardInterval = DI_CH_GUARDINTERVAL_1_8;
			break;
		case FDI_CH_GUARDINTERVAL_1_4 :
			ptDIParam->etGuardInterval = DI_CH_GUARDINTERVAL_1_4;
			break;
		default :
			ptDIParam->etGuardInterval = DI_CH_GUARDINTERVAL_AUTO;
	}

	/*  etHierarchy  */
	switch( ptFDIParam->etHierarchy )
	{
		case FDI_CH_HIERARCHY_AUTO :
			ptDIParam->etHierarchy = DI_CH_HIERARCHY_AUTO;
			break;
		case FDI_CH_HIERARCHY_NONE :
			ptDIParam->etHierarchy = DI_CH_HIERARCHY_NONE;
			break;
		case FDI_CH_HIERARCHY_1 :
			ptDIParam->etHierarchy = DI_CH_HIERARCHY_1;
			break;
		case FDI_CH_HIERARCHY_2 :
			ptDIParam->etHierarchy = DI_CH_HIERARCHY_2;
			break;
		case FDI_CH_HIERARCHY_4 :
			ptDIParam->etHierarchy = DI_CH_HIERARCHY_4;
			break;
		default :
			ptDIParam->etHierarchy = DI_CH_HIERARCHY_AUTO;
	}

	/*  etModulation  */
	switch( ptFDIParam->etModulation )
	{
		case FDI_CH_TMOD_AUTO :
			ptDIParam->etModulation = DI_CH_TMOD_AUTO;
			break;
		case FDI_CH_TMOD_QPSK :
			ptDIParam->etModulation = DI_CH_TMOD_QPSK;
			break;
		case FDI_CH_TMOD_16QAM :
			ptDIParam->etModulation = DI_CH_TMOD_16QAM;
			break;
		case FDI_CH_TMOD_64QAM :
			ptDIParam->etModulation = DI_CH_TMOD_64QAM;
			break;
		case FDI_CH_TMOD_256QAM :
			ptDIParam->etModulation = DI_CH_TMOD_256QAM;
			break;
		default :
			ptDIParam->etModulation = DI_CH_TMOD_AUTO;
	}

	/*  etOffset  */
	switch( ptFDIParam->etOffset )
	{
		case FDI_CH_OFFSET_AUTO :
			ptDIParam->etOffset = DI_CH_OFFSET_AUTO;
			break;
		case FDI_CH_OFFSET_NONE :
			ptDIParam->etOffset = DI_CH_OFFSET_NONE;
			break;
		case FDI_CH_OFFSET_1_6_P :
			ptDIParam->etOffset = DI_CH_OFFSET_1_6_P;
			break;
		case FDI_CH_OFFSET_1_6_N :
			ptDIParam->etOffset = DI_CH_OFFSET_1_6_N;
			break;
		case FDI_CH_OFFSET_2_6_P :
			ptDIParam->etOffset = DI_CH_OFFSET_2_6_P;
			break;
		case FDI_CH_OFFSET_2_6_N :
			ptDIParam->etOffset = DI_CH_OFFSET_2_6_N;
			break;
		case FDI_CH_OFFSET_3_6_P :
			ptDIParam->etOffset = DI_CH_OFFSET_3_6_P;
			break;
		case FDI_CH_OFFSET_3_6_N :
			ptDIParam->etOffset = DI_CH_OFFSET_3_6_N;
			break;
		default :
			ptDIParam->etOffset = DI_CH_OFFSET_AUTO;
	}
	
	/*  etStream  */
	switch( ptFDIParam->etStream )
	{
		case FDI_CH_STREAM_HP :
			ptDIParam->etStream = DI_CH_STREAM_HP;
			break;
		case FDI_CH_STREAM_LP :
			ptDIParam->etStream = DI_CH_STREAM_LP;
			break;
		default :
			ptDIParam->etStream = DI_CH_STREAM_HP;
	}

	/*  etTransMode  */
	switch( ptFDIParam->etTransMode )
	{
		case FDI_CH_TRANSMODE_AUTO :
			ptDIParam->etTransMode = DI_CH_TRANSMODE_AUTO;
			break;
		case FDI_CH_TRANSMODE_2K :
			ptDIParam->etTransMode = DI_CH_TRANSMODE_2K;
			break;
		case FDI_CH_TRANSMODE_8K :
			ptDIParam->etTransMode = DI_CH_TRANSMODE_8K;
			break;
		default :
			ptDIParam->etTransMode = DI_CH_TRANSMODE_AUTO;
	}

	/* for T2 : etTerSystem */
	if( ptFDIParam->etTerSystem == FDI_CH_DVBT2 )
	{
		ptDIParam->etTerSystem = DI_CH_DVBT2;
		ptDIParam->stT2Param.ulPlpId = ptFDIParam->stT2Param.ulPlpId;
		ptDIParam->stT2Param.ulPlpNumBlocks = ptFDIParam->stT2Param.ulPlpNumBlocks;
		
		/* etPreambleFormat  */
		switch( ptFDIParam->stT2Param.etPreambleFormat )
		{
			case FDI_CH_T2SISO :
				ptDIParam->stT2Param.etPreambleFormat = DI_CH_T2SISO;
				break;
			case FDI_CH_T2MISO : 
				ptDIParam->stT2Param.etPreambleFormat = DI_CH_T2MISO;
				break;
			default :
				ptDIParam->stT2Param.etPreambleFormat = DI_CH_T2SISO;
		}

		/* etPreambleMixed  */
		switch( ptFDIParam->stT2Param.etPreambleMixed )
		{
			case FDI_CH_T2NOT_MIXED :
				ptDIParam->stT2Param.etPreambleMixed = DI_CH_T2NOT_MIXED;
				break;
			case FDI_CH_T2MIXED :
				ptDIParam->stT2Param.etPreambleMixed = DI_CH_T2MIXED;
				break;
			default :
				ptDIParam->stT2Param.etPreambleMixed = DI_CH_T2NOT_MIXED;
		}

		/* etPilotPattern  */
		switch( ptFDIParam->stT2Param.etPilotPattern )
		{
			case FDI_CH_T2_PPAUTO :
				ptDIParam->stT2Param.etPilotPattern = DI_CH_T2_PPAUTO;
				break;
			case FDI_CH_T2_PP1 :
				ptDIParam->stT2Param.etPilotPattern = DI_CH_T2_PP1;
				break;
			case FDI_CH_T2_PP2 :
				ptDIParam->stT2Param.etPilotPattern = DI_CH_T2_PP2;
				break;
			case FDI_CH_T2_PP3 :
				ptDIParam->stT2Param.etPilotPattern = DI_CH_T2_PP3;
				break;
			case FDI_CH_T2_PP4 :
				ptDIParam->stT2Param.etPilotPattern = DI_CH_T2_PP4;
				break;
			case FDI_CH_T2_PP5 :
				ptDIParam->stT2Param.etPilotPattern = DI_CH_T2_PP5;
				break;
			case FDI_CH_T2_PP6 :
				ptDIParam->stT2Param.etPilotPattern = DI_CH_T2_PP6;
				break;
			case FDI_CH_T2_PP7 :
				ptDIParam->stT2Param.etPilotPattern = DI_CH_T2_PP7;
				break;
			case FDI_CH_T2_PP8 :
				ptDIParam->stT2Param.etPilotPattern = DI_CH_T2_PP8;
				break;
			default :
				ptDIParam->stT2Param.etPilotPattern = DI_CH_T2_PPAUTO;
		}

		/* etExtendedCarrier  */
		switch( ptFDIParam->stT2Param.etExtendedCarrier )
		{
			case FDI_CH_T2NOT_USED :
				ptDIParam->stT2Param.etExtendedCarrier = DI_CH_T2NOT_USED;
				break;
			case FDI_CH_T2USED :
				ptDIParam->stT2Param.etExtendedCarrier = DI_CH_T2USED;
				break;
			default :
				ptDIParam->stT2Param.etExtendedCarrier = DI_CH_T2NOT_USED;
		}
		
		/* etPaprMode  */
		switch( ptFDIParam->stT2Param.etPaprMode )
		{
			case FDI_CH_NONE_PAPR :
				ptDIParam->stT2Param.etPaprMode = DI_CH_NONE_PAPR;
				break;
			case FDI_CH_ACE_PAPR :
				ptDIParam->stT2Param.etPaprMode = DI_CH_ACE_PAPR;
				break;
			case FDI_CH_TR_PAPR :
				ptDIParam->stT2Param.etPaprMode = DI_CH_TR_PAPR;
				break;
			case FDI_CH_ACETR_PAPR :
				ptDIParam->stT2Param.etPaprMode = DI_CH_ACETR_PAPR;
				break;
			default :
				ptDIParam->stT2Param.etPaprMode = DI_CH_NONE_PAPR;
		}
	}
	else
	{
		ptDIParam->etTerSystem = DI_CH_DVBT;

	}
}

void INTRN_FDI_CHANNEL_T_ConvDItoFDI(CH_TER_TuneParam_t *ptDIParam, FDI_CH_TER_TUNE_PARAM_t *ptFDIParam)
{

	ptFDIParam->ulFrequency = ptDIParam->ulFrequency; /*unit KHz*/

	/*  etBandWidth  */
	switch( ptDIParam->etBandWidth )
	{
		case DI_CH_BW_8MHZ :
			ptFDIParam->etBandWidth = FDI_CH_BW_8MHZ;
			break;
		case DI_CH_BW_7MHZ :
			ptFDIParam->etBandWidth = FDI_CH_BW_7MHZ;
			break;
		case DI_CH_BW_6MHZ :
			ptFDIParam->etBandWidth = FDI_CH_BW_6MHZ;
			break;
		case DI_CH_BW_5MHZ :
			ptFDIParam->etBandWidth = FDI_CH_BW_5MHZ;
			break;
		case DI_CH_BW_10MHZ :
			ptFDIParam->etBandWidth = FDI_CH_BW_10MHZ;
			break;
		case DI_CH_BW_1P7MHZ :
			ptFDIParam->etBandWidth = FDI_CH_BW_1P7MHZ;
			break;
		default :
			ptFDIParam->etBandWidth = FDI_CH_BW_8MHZ;
	}

	/*  etCodeRate  */
	switch( ptDIParam->etCodeRate )
	{
		case DI_CH_TCODERATE_AUTO :
			ptFDIParam->etCodeRate = FDI_CH_TCODERATE_AUTO;
			break;
		case DI_CH_TCODERATE_1_2 :
			ptFDIParam->etCodeRate = FDI_CH_TCODERATE_1_2;
			break;
		case DI_CH_TCODERATE_2_3 :
			ptFDIParam->etCodeRate = FDI_CH_TCODERATE_2_3;
			break;
		case DI_CH_TCODERATE_3_4 :
			ptFDIParam->etCodeRate = FDI_CH_TCODERATE_3_4;
			break;
		case DI_CH_TCODERATE_5_6 :
			ptFDIParam->etCodeRate = FDI_CH_TCODERATE_5_6;
			break;
		case DI_CH_TCODERATE_7_8 :
			ptFDIParam->etCodeRate = FDI_CH_TCODERATE_7_8;
			break;
		case DI_CH_T2CODERATE_3_5 :
			ptFDIParam->etCodeRate = FDI_CH_T2CODERATE_3_5;
			break;
		case DI_CH_T2CODERATE_4_5  :
			ptFDIParam->etCodeRate = FDI_CH_T2CODERATE_4_5;
			break;
		default :
			ptFDIParam->etCodeRate = FDI_CH_TCODERATE_AUTO;
	}
	
	/*  etGuardInterval  */
	switch( ptDIParam->etGuardInterval )
	{
		case DI_CH_GUARDINTERVAL_AUTO :
			ptFDIParam->etGuardInterval = FDI_CH_GUARDINTERVAL_AUTO;
			break;
		case DI_CH_GUARDINTERVAL_1_32 :
			ptFDIParam->etGuardInterval = FDI_CH_GUARDINTERVAL_1_32;
			break;
		case DI_CH_GUARDINTERVAL_1_16 :
			ptFDIParam->etGuardInterval = FDI_CH_GUARDINTERVAL_1_16;
			break;
		case DI_CH_GUARDINTERVAL_1_8 :
			ptFDIParam->etGuardInterval = FDI_CH_GUARDINTERVAL_1_8;
			break;
		case DI_CH_GUARDINTERVAL_1_4 :
			ptFDIParam->etGuardInterval = FDI_CH_GUARDINTERVAL_1_4;
			break;
		default :
			ptFDIParam->etGuardInterval = FDI_CH_GUARDINTERVAL_AUTO;
	}

	/*  etHierarchy  */
	switch( ptDIParam->etHierarchy )
	{
		case DI_CH_HIERARCHY_AUTO :
			ptFDIParam->etHierarchy = FDI_CH_HIERARCHY_AUTO;
			break;
		case DI_CH_HIERARCHY_NONE :
			ptFDIParam->etHierarchy = FDI_CH_HIERARCHY_NONE;
			break;
		case DI_CH_HIERARCHY_1 :
			ptFDIParam->etHierarchy = FDI_CH_HIERARCHY_1;
			break;
		case DI_CH_HIERARCHY_2 :
			ptFDIParam->etHierarchy = FDI_CH_HIERARCHY_2;
			break;
		case DI_CH_HIERARCHY_4 :
			ptFDIParam->etHierarchy = FDI_CH_HIERARCHY_4;
			break;
		default :
			ptFDIParam->etHierarchy = FDI_CH_HIERARCHY_AUTO;
	}

	/*  etModulation  */
	switch( ptDIParam->etModulation )
	{
		case DI_CH_TMOD_AUTO :
			ptFDIParam->etModulation = FDI_CH_TMOD_AUTO;
			break;
		case DI_CH_TMOD_QPSK :
			ptFDIParam->etModulation = FDI_CH_TMOD_QPSK;
			break;
		case DI_CH_TMOD_16QAM :
			ptFDIParam->etModulation = FDI_CH_TMOD_16QAM;
			break;
		case DI_CH_TMOD_64QAM :
			ptFDIParam->etModulation = FDI_CH_TMOD_64QAM;
			break;
		case DI_CH_TMOD_256QAM :
			ptFDIParam->etModulation = FDI_CH_TMOD_256QAM;
			break;
		default :
			ptFDIParam->etModulation = FDI_CH_TMOD_AUTO;
	}

	/*  etOffset  */
	switch( ptDIParam->etOffset )
	{
		case DI_CH_OFFSET_AUTO :
			ptFDIParam->etOffset = FDI_CH_OFFSET_AUTO;
			break;
		case DI_CH_OFFSET_NONE :
			ptFDIParam->etOffset = FDI_CH_OFFSET_NONE;
			break;
		case DI_CH_OFFSET_1_6_P :
			ptFDIParam->etOffset = FDI_CH_OFFSET_1_6_P;
			break;
		case DI_CH_OFFSET_1_6_N :
			ptFDIParam->etOffset = FDI_CH_OFFSET_1_6_N;
			break;
		case DI_CH_OFFSET_2_6_P :
			ptFDIParam->etOffset = FDI_CH_OFFSET_2_6_P;
			break;
		case DI_CH_OFFSET_2_6_N :
			ptFDIParam->etOffset = FDI_CH_OFFSET_2_6_N;
			break;
		case DI_CH_OFFSET_3_6_P :
			ptFDIParam->etOffset = FDI_CH_OFFSET_3_6_P;
			break;
		case DI_CH_OFFSET_3_6_N :
			ptFDIParam->etOffset = FDI_CH_OFFSET_3_6_N;
			break;
		default :
			ptFDIParam->etOffset = FDI_CH_OFFSET_AUTO;
	}
	
	/*  etStream  */
	switch( ptDIParam->etStream )
	{
		case DI_CH_STREAM_HP :
			ptFDIParam->etStream = FDI_CH_STREAM_HP;
			break;
		case DI_CH_STREAM_LP :
			ptFDIParam->etStream = FDI_CH_STREAM_LP;
			break;
		default :
			ptFDIParam->etStream = FDI_CH_STREAM_HP;
	}

	/*  etTransMode  */
	switch( ptDIParam->etTransMode )
	{
		case DI_CH_TRANSMODE_AUTO :
			ptFDIParam->etTransMode = FDI_CH_TRANSMODE_AUTO;
			break;
		case DI_CH_TRANSMODE_2K :
			ptFDIParam->etTransMode = FDI_CH_TRANSMODE_2K;
			break;
		case DI_CH_TRANSMODE_8K :
			ptFDIParam->etTransMode = FDI_CH_TRANSMODE_8K;
			break;
		default :
			ptFDIParam->etTransMode = FDI_CH_TRANSMODE_AUTO;
	}

	/* for T2 : etTerSystem */
	if( ptDIParam->etTerSystem == DI_CH_DVBT2 )
	{
		ptFDIParam->etTerSystem = FDI_CH_DVBT;
		ptFDIParam->stT2Param.ulPlpId = ptDIParam->stT2Param.ulPlpId;
		ptFDIParam->stT2Param.ulPlpNumBlocks = ptDIParam->stT2Param.ulPlpNumBlocks;
		
		/* etPreambleFormat  */
		switch( ptDIParam->stT2Param.etPreambleFormat )
		{
			case DI_CH_T2SISO :
				ptFDIParam->stT2Param.etPreambleFormat = FDI_CH_T2SISO;
				break;
			case DI_CH_T2MISO : 
				ptFDIParam->stT2Param.etPreambleFormat = FDI_CH_T2MISO;
				break;
			default :
				ptFDIParam->stT2Param.etPreambleFormat = FDI_CH_T2SISO;
		}

		/* etPreambleMixed  */
		switch( ptDIParam->stT2Param.etPreambleMixed )
		{
			case DI_CH_T2NOT_MIXED :
				ptFDIParam->stT2Param.etPreambleMixed = FDI_CH_T2NOT_MIXED;
				break;
			case DI_CH_T2MIXED :
				ptFDIParam->stT2Param.etPreambleMixed = FDI_CH_T2MIXED;
				break;
			default :
				ptFDIParam->stT2Param.etPreambleMixed = FDI_CH_T2NOT_MIXED;
		}

		/* etPilotPattern  */
		switch( ptDIParam->stT2Param.etPilotPattern )
		{
			case DI_CH_T2_PPAUTO :
				ptFDIParam->stT2Param.etPilotPattern = FDI_CH_T2_PPAUTO;
				break;
			case DI_CH_T2_PP1 :
				ptFDIParam->stT2Param.etPilotPattern = FDI_CH_T2_PP1;
				break;
			case DI_CH_T2_PP2 :
				ptFDIParam->stT2Param.etPilotPattern = FDI_CH_T2_PP2;
				break;
			case DI_CH_T2_PP3 :
				ptFDIParam->stT2Param.etPilotPattern = FDI_CH_T2_PP3;
				break;
			case DI_CH_T2_PP4 :
				ptFDIParam->stT2Param.etPilotPattern = FDI_CH_T2_PP4;
				break;
			case DI_CH_T2_PP5 :
				ptFDIParam->stT2Param.etPilotPattern = FDI_CH_T2_PP5;
				break;
			case DI_CH_T2_PP6 :
				ptFDIParam->stT2Param.etPilotPattern = FDI_CH_T2_PP6;
				break;
			case DI_CH_T2_PP7 :
				ptFDIParam->stT2Param.etPilotPattern = FDI_CH_T2_PP7;
				break;
			case DI_CH_T2_PP8 :
				ptFDIParam->stT2Param.etPilotPattern = FDI_CH_T2_PP8;
				break;
			default :
				ptFDIParam->stT2Param.etPilotPattern = FDI_CH_T2_PPAUTO;
		}

		/* etExtendedCarrier  */
		switch( ptDIParam->stT2Param.etExtendedCarrier )
		{
			case DI_CH_T2NOT_USED :
				ptFDIParam->stT2Param.etExtendedCarrier = FDI_CH_T2NOT_USED;
				break;
			case DI_CH_T2USED :
				ptFDIParam->stT2Param.etExtendedCarrier = FDI_CH_T2USED;
				break;
			default :
				ptFDIParam->stT2Param.etExtendedCarrier = FDI_CH_T2NOT_USED;
		}
		
		/* etPaprMode  */
		switch( ptDIParam->stT2Param.etPaprMode )
		{
			case DI_CH_NONE_PAPR :
				ptFDIParam->stT2Param.etPaprMode = FDI_CH_NONE_PAPR;
				break;
			case DI_CH_ACE_PAPR :
				ptFDIParam->stT2Param.etPaprMode = FDI_CH_ACE_PAPR;
				break;
			case DI_CH_TR_PAPR :
				ptFDIParam->stT2Param.etPaprMode = FDI_CH_TR_PAPR;
				break;
			case DI_CH_ACETR_PAPR :
				ptFDIParam->stT2Param.etPaprMode = FDI_CH_ACETR_PAPR;
				break;
			default :
				ptFDIParam->stT2Param.etPaprMode = FDI_CH_NONE_PAPR;
		}
	}
	else
	{
		ptFDIParam->etTerSystem = FDI_CH_DVBT;
	}
	
}


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_T_Open(unsigned long ulDeviceId)
{
	DI_CHANNEL_ERR eDiErr;

	FDI_PRINT(FDI_PRT_DBGINFO, ("[FDI_CHANNEL_T_Open] : ++\n"));
	eDiErr = DI_CH_T_ActivateChannel((unsigned char)ulDeviceId);
	if(eDiErr != DI_CH_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_T_Open] FDI_CHANNEL_S_Open(%d) Error : %d\n", ulDeviceId, eDiErr));
		return FDI_ERR;
	}
	FDI_PRINT(FDI_PRT_DBGINFO, ("[FDI_CHANNEL_T_Open] : --\n"));

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_T_RequestLock(unsigned long ulDeviceId, FDI_CH_TER_TUNE_PARAM_t *ptParam)
{
	DI_CHANNEL_ERR eDiErr;
	CH_TER_TuneParam_t tDiParam;

	FDI_PRINT(FDI_PRT_DBGINFO, ("[FDI_CHANNEL_T_RequestLock] : ++\n"));

	if( ptParam == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_T_RequestLock] Invalid Parameter. ptParams==NULL\n"));
		return FDI_ERR;
	}

	s_ulRequestId++;

	FVK_MEM_Set(&tDiParam, 0, sizeof(CH_TER_TuneParam_t));
	INTRN_FDI_CHANNEL_T_ConvFDItoDI(ptParam, &tDiParam);

#if (FACTORY_USE_CHANNEL_CAB == YES)
	DI_CH_C_CancelLock(ulDeviceId);
	DI_CH_C_DeactivateChannel(ulDeviceId);
#endif
#if (FACTORY_USE_CHANNEL_SAT == YES)
	DI_CH_S_CancelLock(ulDeviceId);
	DI_CH_S_DeactivateChannel(ulDeviceId);
#endif
		
	DI_CH_T_DeactivateChannel(ulDeviceId);
	DI_DEMUX_BindChannel(ulDeviceId, DI_CH_TYPE_TER);
	DI_CH_T_ActivateChannel(ulDeviceId);
	
	/* request lock  */
	eDiErr = DI_CH_T_RequestLock(ulDeviceId, s_ulRequestId, &tDiParam);
	if(eDiErr != DI_CH_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_T_RequestLock] DI_CH_S_RequestLock Error : %d\n", eDiErr));
		return FDI_ERR;
	}

	FDI_PRINT(FDI_PRT_DBGINFO, ("[FDI_CHANNEL_T_RequestLock] : --\n"));

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_T_GetLockedInfo(unsigned long ulDeviceId,
											FDI_CH_TER_TUNE_PARAM_t *ptParam)
{
	DI_CHANNEL_ERR eDiErr;
	CH_TER_TuneParam_t tDiParam;

	if( ptParam == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_T_GetLockedInfo] Invalid Parameter. ptParams==NULL\n"));
		return FDI_ERR;
	}

	eDiErr = DI_CH_T_GetLockedInfo(ulDeviceId, &tDiParam);
	if(eDiErr != DI_CH_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_T_GetLockedInfo] DI_CH_S_GetLockedInfo Error : %d\n", eDiErr));
		return FDI_ERR;
	}

	INTRN_FDI_CHANNEL_T_ConvDItoFDI(&tDiParam, ptParam);

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_T_IsLocked(unsigned long ulDeviceId, BOOL *pbIsLocked)
{
	DI_CHANNEL_ERR eDiErr;
	CH_TER_TuneParam_t tDiParam; // fixed by shinms

	if( pbIsLocked == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_T_IsLocked] Invalid Parameter. pbIsLocked==NULL\n"));
		return FDI_ERR;
	}

	eDiErr = DI_CH_T_GetLockedInfo(ulDeviceId, &tDiParam);
	if(eDiErr != DI_CH_OK)
	{
		*pbIsLocked = FALSE;
	}
	else
	{
		*pbIsLocked = TRUE;
	}

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_T_CancelLock(unsigned long ulDeviceId)
{
	DI_CHANNEL_ERR eDiErr;

	eDiErr = DI_CH_T_CancelLock(ulDeviceId);
	if(eDiErr != DI_CH_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_T_CancelLock] DI_CH_T_CancelLock Error : %d\n", eDiErr));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_T_SetAntennaPower(unsigned long ulDeviceId, BOOL bOn_nOff)
{
	DI_CHANNEL_ERR eDiErr;

	if (bOn_nOff)
	{
		eDiErr = DI_CH_T_SetAntennaPower(ulDeviceId, DI_CH_ANT_POWER_ON);
	}
	else
	{
		eDiErr = DI_CH_T_SetAntennaPower(ulDeviceId, DI_CH_ANT_POWER_OFF);
	}
	if(eDiErr != DI_CH_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_T_SetAntennaPower] DI_CH_T_SetAntennaPower Error : eDiErr(%d), bOn_nOff(%d)\n", eDiErr, bOn_nOff));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_T_GetSignalInfo(unsigned long ulDeviceId, FDI_CH_TER_SIGNAL_QUALITY_t *ptSignalInfo)
{
	DI_CHANNEL_ERR eDiErr;
	DI_CH_SignalQuality_t tDiParam;

	if( ptSignalInfo == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_T_GetSignalInfo] Invalid Parameter. ptSignalInfo==NULL\n"));
		return FDI_ERR;
	}

	eDiErr = DI_CH_T_GetSignalInfo(ulDeviceId, &tDiParam);
	if(eDiErr != DI_CH_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_T_GetSignalInfo] DI_CH_S_GetSignalInfo Error : %d\n", eDiErr));
		return FDI_ERR;
	}

	FDI_PRINT(0, ("[FDI_CHANNEL_S_GetSignalInfo] AGC:%d\n", tDiParam.ulAgc));
	FDI_PRINT(0, ("[FDI_CHANNEL_S_GetSignalInfo] BER:%f\n", tDiParam.fBer));
	FDI_PRINT(0, ("[FDI_CHANNEL_S_GetSignalInfo] SNR:%f\n", tDiParam.fSnr));
	FDI_PRINT(0, ("[FDI_CHANNEL_S_GetSignalInfo] strength:%d\n", tDiParam.ulStrength));
	FDI_PRINT(0, ("[FDI_CHANNEL_S_GetSignalInfo] quality:%d\n", tDiParam.ulQuality));

	ptSignalInfo->unAgc = tDiParam.ulAgc;
	ptSignalInfo->fBer = tDiParam.fBer;
	ptSignalInfo->fSnr = tDiParam.fSnr;
	ptSignalInfo->ulStrength = tDiParam.ulStrength;
	ptSignalInfo->ulQuality = tDiParam.ulQuality;

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_T_RegisterCallback(unsigned long ulDeviceId, pfnFDI_CHANNEL_NotifySignalInfo pfnNotify)
{
	DI_CHANNEL_ERR eDiErr;
	pfnDI_CH_NotifySignalInfo pfnDiNotify;

	pfnDiNotify = (pfnDI_CH_NotifySignalInfo)pfnNotify;
	eDiErr = DI_CH_T_RegisterCallback(ulDeviceId, pfnDiNotify);
	if(eDiErr != DI_CH_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_T_RegisterCallback] DI_CH_S_RegisterCallback Error : %d\n", eDiErr));
		return FDI_ERR;
	}
	return FDI_NO_ERR;
}


