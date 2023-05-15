//-----------------------------------------------------------------------------
// $Header:
// (C) Copyright 2001 NXP Semiconductors, All rights reserved
//
// This source code and any compilation or derivative thereof is the sole
// property of NXP Corporation and is provided pursuant to a Software
// License Agreement.  This code is the proprietary information of NXP
// Corporation and is confidential in nature.  Its use and dissemination by
// any party other than NXP Corporation is strictly limited by the
// confidential information provisions of the Agreement referenced above.
//-----------------------------------------------------------------------------
// FILE NAME:    tmbslTDA182I2Instance.c
//
// DESCRIPTION:  define the static Objects
//
// DOCUMENT REF: DVP Software Coding Guidelines v1.14
//               DVP Board Support Library Architecture Specification v0.5
//
// NOTES:
//-----------------------------------------------------------------------------
//
//#include"Channel.h"
#include"di_channel.h"
#include"di_channel_priv.h"
#include"TDA182I2.h"
#include"TDA182I2Instance.h"


/********************************************************************
	Print Option
 ********************************************************************/
#include "di_err.h"

#define CH_PRINT				// All Print Enable /Disable
#define CH_PRINT_ERROR			// Error Print Enable / Disable
//#define CH_PRINT_WARNING
//#define CH_PRINT_DEBUG
//#define CH_PRINT_DATA

//#include "ch_print.h"
/********************************************************************
	Global Variable Definition
 ********************************************************************/
extern HINT32 show_di_ch;


/* default instance */
TDA182I2Object_t gTDA182I2Instance[] =
{
	{
		0,														//tUnit
		FALSE,													//initDone
		0,														//resetDone
		TDA182I2_ADD,											//uHwAddress
		tmTDA182I2_PowerStandby,								//PowerState
		tmPowerStandby,											//curPowerState
		0,														//uRF
		tmTDA182I2_QAM_8MHz,									//StandardMode
		{														//Std_Array
			{													/* DVB-T 6MHz */
				3250000,										// IF
				0,												// CF_Offset
				TDA182I2_LPF_6MHz,							  	// LPF
				TDA182I2_LPFOffset_0pc, 					  	// LPF_Offset
				TDA182I2_IF_AGC_Gain_1Vpp_min_6_24dB,		  	// IF_Gain
				TDA182I2_IF_Notch_Enabled,					  	// IF_Notch
				TDA182I2_IF_HPF_0_4MHz, 					  	// IF_HPF
				TDA182I2_DC_Notch_Enabled,					  	// DC_Notch
				TDA182I2_AGC1_LNA_TOP_d95_u89dBuV,			 	// AGC1_LNA_TOP
				TDA182I2_AGC2_RF_Attenuator_TOP_d90_u84dBuV,  // AGC2_RF_Attenuator_TOP
				TDA182I2_AGC3_RF_AGC_TOP_100dBuV,			  	// AGC3_RF_AGC_TOP_Low_band
				TDA182I2_AGC3_RF_AGC_TOP_102dBuV,			  	// AGC3_RF_AGC_TOP_High_band
				TDA182I2_AGC4_IR_Mixer_TOP_d110_u105dBuV,		// AGC4_IR_Mixer_TOP
				TDA182I2_AGC5_IF_AGC_TOP_d110_u105dBuV, 		// AGC5_IF_AGC_TOP
				TDA182I2_AGC5_Detector_HPF_Disabled,		  	// AGC5_Detector_HPF
				TDA182I2_AGC3_Adapt_Enabled,				  	// AGC3_Adapt
				TDA182I2_AGC3_Adapt_TOP_98dBuV, 			  	// AGC3_Adapt_TOP
				TDA182I2_AGC5_Atten_3dB_Enabled,			  	// AGC5_Atten_3dB
				0x02,											// GSK : settings V2.0.0
				TDA182I2_H3H5_VHF_Filter6_Enabled,			  	// H3H5_VHF_Filter6
				TDA182I2_LPF_Gain_Free						  	// LPF_Gain
			},
			{													/* DVB-T 7MHz */
				3500000,										// IF
				0,												// CF_Offset
				TDA182I2_LPF_7MHz,							  	// LPF
				TDA182I2_LPFOffset_min_8pc, 				  	// LPF_Offset
				TDA182I2_IF_AGC_Gain_1Vpp_min_6_24dB,		  	// IF_Gain
				TDA182I2_IF_Notch_Enabled,					  	// IF_Notch
				TDA182I2_IF_HPF_Disabled,					  	// IF_HPF
				TDA182I2_DC_Notch_Enabled,					 	// DC_Notch
				TDA182I2_AGC1_LNA_TOP_d95_u89dBuV,			 	// AGC1_LNA_TOP
				TDA182I2_AGC2_RF_Attenuator_TOP_d90_u84dBuV,  // AGC2_RF_Attenuator_TOP
				TDA182I2_AGC3_RF_AGC_TOP_100dBuV,			  	// AGC3_RF_AGC_TOP_Low_band
				TDA182I2_AGC3_RF_AGC_TOP_102dBuV,			  	// AGC3_RF_AGC_TOP_High_band
				TDA182I2_AGC4_IR_Mixer_TOP_d110_u105dBuV,	  	// AGC4_IR_Mixer_TOP
				TDA182I2_AGC5_IF_AGC_TOP_d110_u105dBuV, 	  	// AGC5_IF_AGC_TOP
				TDA182I2_AGC5_Detector_HPF_Disabled,		  	// AGC5_Detector_HPF
				TDA182I2_AGC3_Adapt_Enabled,				  	// AGC3_Adapt
				TDA182I2_AGC3_Adapt_TOP_98dBuV, 			  	// AGC3_Adapt_TOP
				TDA182I2_AGC5_Atten_3dB_Enabled,			  	// AGC5_Atten_3dB
				0x02,											// GSK : settings V2.0.0
				TDA182I2_H3H5_VHF_Filter6_Enabled,			  	// H3H5_VHF_Filter6
				TDA182I2_LPF_Gain_Free						  	// LPF_Gain
			},
			{													/* DVB-T 8MHz */
				4000000,										// IF
				0,												// CF_Offset
				TDA182I2_LPF_8MHz,							  	// LPF
				TDA182I2_LPFOffset_0pc, 					  	// LPF_Offset
				TDA182I2_IF_AGC_Gain_1Vpp_min_6_24dB,		  	// IF_Gain
				TDA182I2_IF_Notch_Enabled,					  	// IF_Notch
				TDA182I2_IF_HPF_Disabled,					  	// IF_HPF
				TDA182I2_DC_Notch_Enabled,					  	// DC_Notch
				TDA182I2_AGC1_LNA_TOP_d95_u89dBuV,			  	// AGC1_LNA_TOP
				TDA182I2_AGC2_RF_Attenuator_TOP_d90_u84dBuV,  // AGC2_RF_Attenuator_TOP
				TDA182I2_AGC3_RF_AGC_TOP_100dBuV,			  	// AGC3_RF_AGC_TOP_Low_band
				TDA182I2_AGC3_RF_AGC_TOP_102dBuV,			  	// AGC3_RF_AGC_TOP_High_band
				TDA182I2_AGC4_IR_Mixer_TOP_d110_u105dBuV,	  	// AGC4_IR_Mixer_TOP
				TDA182I2_AGC5_IF_AGC_TOP_d110_u105dBuV, 	  	// AGC5_IF_AGC_TOP
				TDA182I2_AGC5_Detector_HPF_Disabled,		  	// AGC5_Detector_HPF
				TDA182I2_AGC3_Adapt_Enabled,				  	// AGC3_Adapt
				TDA182I2_AGC3_Adapt_TOP_98dBuV, 			  	// AGC3_Adapt_TOP
				TDA182I2_AGC5_Atten_3dB_Enabled,			  	// AGC5_Atten_3dB
				0x02,											// GSK : settings V2.0.0
				TDA182I2_H3H5_VHF_Filter6_Enabled,			  	// H3H5_VHF_Filter6
				TDA182I2_LPF_Gain_Free						  	// LPF_Gain
			},
			{													/* QAM 6MHz */
				3600000,										// IF
				0,												// CF_Offset
				TDA182I2_LPF_6MHz,							  	// LPF
				TDA182I2_LPFOffset_min_8pc, 				  	// LPF_Offset
				TDA182I2_IF_AGC_Gain_2Vpp_0_30dB,		  		// IF_Gain
				TDA182I2_IF_Notch_Disabled, 				  	// IF_Notch
				TDA182I2_IF_HPF_Disabled,					  	// IF_HPF
				TDA182I2_DC_Notch_Enabled,					  	// DC_Notch
				TDA182I2_AGC1_LNA_TOP_d95_u89dBuV,			  	// AGC1_LNA_TOP
				TDA182I2_AGC2_RF_Attenuator_TOP_d90_u84dBuV,  // AGC2_RF_Attenuator_TOP
				TDA182I2_AGC3_RF_AGC_TOP_100dBuV,			  	// AGC3_RF_AGC_TOP_Low_band
				TDA182I2_AGC3_RF_AGC_TOP_100dBuV,			  	// AGC3_RF_AGC_TOP_High_band
				TDA182I2_AGC4_IR_Mixer_TOP_d110_u105dBuV,		// AGC4_IR_Mixer_TOP
				TDA182I2_AGC5_IF_AGC_TOP_d110_u105dBuV, 	 	// AGC5_IF_AGC_TOP
				TDA182I2_AGC5_Detector_HPF_Disabled,		  	// AGC5_Detector_HPF
				TDA182I2_AGC3_Adapt_Disabled,				  	// AGC3_Adapt
				TDA182I2_AGC3_Adapt_TOP_102dBuV,			  	// AGC3_Adapt_TOP
				TDA182I2_AGC5_Atten_3dB_Disabled,			  	// AGC5_Atten_3dB
				0x02,											// GSK : settings V2.0.0
				TDA182I2_H3H5_VHF_Filter6_Disabled, 		  	// H3H5_VHF_Filter6
				TDA182I2_LPF_Gain_Free						  	// LPF_Gain
			},
			{													/* QAM 8MHz */
				5000000,											/* IF */
				0,													/* CF_Offset */
				TDA182I2_LPF_9MHz,									/* LPF */
			#if defined(CONFIG_TVT_SIGNAL)
				TDA182I2_LPFOffset_0pc,
			#else
				TDA182I2_LPFOffset_min_8pc,							/* LPF_Offset */
			#endif
				TDA182I2_IF_AGC_Gain_1Vpp_min_6_24dB, 				/* IF_Gain */
				TDA182I2_IF_Notch_Disabled,							/* IF_Notch */
				TDA182I2_IF_HPF_0_85MHz,							/* IF_HPF */
			#if defined(CONFIG_TVT_SIGNAL)
				TDA182I2_DC_Notch_Disabled,
			#else
				TDA182I2_DC_Notch_Enabled,							/* DC_Notch */
			#endif
				TDA182I2_AGC1_LNA_TOP_d95_u89dBuV,					/* AGC1_LNA_TOP */
				TDA182I2_AGC2_RF_Attenuator_TOP_d90_u84dBuV,		/* AGC2_RF_Attenuator_TOP */
				TDA182I2_AGC3_RF_AGC_TOP_100dBuV, 					/* AGC3_RF_AGC_TOP_Low_band */
				TDA182I2_AGC3_RF_AGC_TOP_100dBuV, 					/* AGC3_RF_AGC_TOP_High_band */
				TDA182I2_AGC4_IR_Mixer_TOP_d110_u105dBuV, 			/* AGC4_IR_Mixer_TOP */
				TDA182I2_AGC5_IF_AGC_TOP_d110_u105dBuV,				/* AGC5_IF_AGC_TOP */
				TDA182I2_AGC5_Detector_HPF_Disabled,				/* AGC5_Detector_HPF */
				TDA182I2_AGC3_Adapt_Disabled, 						/* AGC3_Adapt */
				TDA182I2_AGC3_Adapt_TOP_102dBuV,					/* AGC3_Adapt_TOP */
				TDA182I2_AGC5_Atten_3dB_Disabled, 					/* AGC5_Atten_3dB */
				0x02,												/* GSK : settings V2.0.0  */
				TDA182I2_H3H5_VHF_Filter6_Disabled,					/* H3H5_VHF_Filter6 */
				TDA182I2_LPF_Gain_Free								/* LPF_Gain */
			},
			{													/* ISDBT 6MHz */
				3250000,										// IF
				0,												// CF_Offset
				TDA182I2_LPF_6MHz,							  	// LPF
				TDA182I2_LPFOffset_0pc, 					  	// LPF_Offset
				TDA182I2_IF_AGC_Gain_0_6Vpp_min_10_3_19_7dB,  // IF_Gain
				TDA182I2_IF_Notch_Enabled,					  	// IF_Notch
				TDA182I2_IF_HPF_0_4MHz, 					  	// IF_HPF
				TDA182I2_DC_Notch_Enabled,					  	// DC_Notch
				TDA182I2_AGC1_LNA_TOP_d95_u89dBuV,			  	// AGC1_LNA_TOP
				TDA182I2_AGC2_RF_Attenuator_TOP_d90_u84dBuV,  // AGC2_RF_Attenuator_TOP
				TDA182I2_AGC3_RF_AGC_TOP_100dBuV,			  	// AGC3_RF_AGC_TOP_Low_band
				TDA182I2_AGC3_RF_AGC_TOP_102dBuV,			  	// AGC3_RF_AGC_TOP_High_band
				TDA182I2_AGC4_IR_Mixer_TOP_d110_u105dBuV,	  	// AGC4_IR_Mixer_TOP
				TDA182I2_AGC5_IF_AGC_TOP_d110_u105dBuV, 	  	// AGC5_IF_AGC_TOP
				TDA182I2_AGC5_Detector_HPF_Disabled,		  	// AGC5_Detector_HPF
				TDA182I2_AGC3_Adapt_Enabled,				  	// AGC3_Adapt
				TDA182I2_AGC3_Adapt_TOP_98dBuV, 			  	// AGC3_Adapt_TOP
				TDA182I2_AGC5_Atten_3dB_Enabled,			  	// AGC5_Atten_3dB
				0x02,											// GSK : settings V2.0.0
				TDA182I2_H3H5_VHF_Filter6_Enabled,			  	// H3H5_VHF_Filter6
				TDA182I2_LPF_Gain_Free						  	// LPF_Gain
			},
			{													/* ATSC 6MHz */
				3250000,										// IF
				0,												// CF_Offset
				TDA182I2_LPF_6MHz,							  	// LPF
				TDA182I2_LPFOffset_0pc, 					  	// LPF_Offset
				TDA182I2_IF_AGC_Gain_0_6Vpp_min_10_3_19_7dB,  // IF_Gain
				TDA182I2_IF_Notch_Enabled,					  	// IF_Notch
				TDA182I2_IF_HPF_0_4MHz, 					  	// IF_HPF
				TDA182I2_DC_Notch_Enabled,					  	// DC_Notch
				TDA182I2_AGC1_LNA_TOP_d100_u94dBuV, 		  	// AGC1_LNA_TOP
				TDA182I2_AGC2_RF_Attenuator_TOP_d90_u84dBuV,  // AGC2_RF_Attenuator_TOP
				TDA182I2_AGC3_RF_AGC_TOP_104dBuV,			  	// AGC3_RF_AGC_TOP_Low_band
				TDA182I2_AGC3_RF_AGC_TOP_107dBuV,			  	// AGC3_RF_AGC_TOP_High_band
				TDA182I2_AGC4_IR_Mixer_TOP_d112_u107dBuV,	  	// AGC4_IR_Mixer_TOP
				TDA182I2_AGC5_IF_AGC_TOP_d112_u107dBuV, 	 	// AGC5_IF_AGC_TOP
				TDA182I2_AGC5_Detector_HPF_Disabled,		  	// AGC5_Detector_HPF
				TDA182I2_AGC3_Adapt_Enabled,				  	// AGC3_Adapt
				TDA182I2_AGC3_Adapt_TOP_96dBuV, 			  	// AGC3_Adapt_TOP
				TDA182I2_AGC5_Atten_3dB_Enabled,			  	// AGC5_Atten_3dB
				0x02,											// GSK : settings V2.0.0
				TDA182I2_H3H5_VHF_Filter6_Enabled,			  	// H3H5_VHF_Filter6
				TDA182I2_LPF_Gain_Free						  	// LPF_Gain
			},
			{													/* DMB-T 8MHz */
				4000000,										// IF
				0,												// CF_Offset
				TDA182I2_LPF_8MHz,							  	// LPF
				TDA182I2_LPFOffset_0pc, 					  	// LPF_Offset
				TDA182I2_IF_AGC_Gain_1Vpp_min_6_24dB,		  	// IF_Gain
				TDA182I2_IF_Notch_Enabled,					  	// IF_Notch
				TDA182I2_IF_HPF_Disabled,					  	// IF_HPF
				TDA182I2_DC_Notch_Enabled,					  	// DC_Notch
				TDA182I2_AGC1_LNA_TOP_d95_u89dBuV,			  	// AGC1_LNA_TOP
				TDA182I2_AGC2_RF_Attenuator_TOP_d90_u84dBuV,  // AGC2_RF_Attenuator_TOP
				TDA182I2_AGC3_RF_AGC_TOP_100dBuV,			  	// AGC3_RF_AGC_TOP_Low_band
				TDA182I2_AGC3_RF_AGC_TOP_102dBuV,			  	// AGC3_RF_AGC_TOP_High_band
				TDA182I2_AGC4_IR_Mixer_TOP_d110_u105dBuV,	  	// AGC4_IR_Mixer_TOP
				TDA182I2_AGC5_IF_AGC_TOP_d110_u105dBuV, 	  	// AGC5_IF_AGC_TOP
				TDA182I2_AGC5_Detector_HPF_Disabled,		  	// AGC5_Detector_HPF
				TDA182I2_AGC3_Adapt_Enabled,				  	// AGC3_Adapt
				TDA182I2_AGC3_Adapt_TOP_98dBuV, 			  	// AGC3_Adapt_TOP
				TDA182I2_AGC5_Atten_3dB_Enabled,			  	// AGC5_Atten_3dB
				0x02,											// GSK : settings V2.0.0
				TDA182I2_H3H5_VHF_Filter6_Enabled,			  	// H3H5_VHF_Filter6
				TDA182I2_LPF_Gain_Free						  	// LPF_Gain
			}
		},
		TRT_UNKNOWN,											//lastTuneReqType
		{
			{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},							// I2CMap
			{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},
			{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},
			{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},
			{0},{0},{0},{0}
		},
		0,                                  					// bSoftReset
		1,														// IRQ wait True
	        1,                                  					// bRFCAL_SW_Algo_Enable
	        {                                      				 	// RFCAL_Prog
			{ 0, 0, 0 },
			{ 1, 0, 0 },
			{ 2, 0, 0 },
			{ 3, 0, 0 },
			{ 4, 0, 0 },
			{ 5, 0, 0 },
			{ 6, 0, 0 },
			{ 7, 0, 0 },
			{ 8, 0, 0 },
			{ 9, 0, 0 },
			{ 10, 0, 0 },
			{ 11, 0, 0 }
	        },
	        {                                       				// RFCAL_Coeffs
			{ 0, 0, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 2, 3, 0, 0 },
			{ 3, 4, 0, 0 },
			{ 4, 6, 0, 0 },
			{ 5, 7, 0, 0 },
			{ 6, 9, 0, 0 },
			{ 7, 10, 0, 0 }
		}
	},

	{
		0,														//tUnit
		FALSE,													//initDone
		0,														//resetDone
		TDA182I2_ADD,											//uHwAddress
		tmTDA182I2_PowerStandby,								//PowerState
		tmPowerStandby,											//curPowerState
		0,														//uRF
		tmTDA182I2_QAM_8MHz,									//StandardMode
		{														//Std_Array
			{													/* DVB-T 6MHz */
				3250000,										// IF
				0,												// CF_Offset
				TDA182I2_LPF_6MHz,								// LPF
				TDA182I2_LPFOffset_0pc, 						// LPF_Offset
				TDA182I2_IF_AGC_Gain_1Vpp_min_6_24dB,			// IF_Gain
				TDA182I2_IF_Notch_Enabled,						// IF_Notch
				TDA182I2_IF_HPF_0_4MHz, 						// IF_HPF
				TDA182I2_DC_Notch_Enabled,						// DC_Notch
				TDA182I2_AGC1_LNA_TOP_d95_u89dBuV,				// AGC1_LNA_TOP
				TDA182I2_AGC2_RF_Attenuator_TOP_d90_u84dBuV,  // AGC2_RF_Attenuator_TOP
				TDA182I2_AGC3_RF_AGC_TOP_100dBuV,				// AGC3_RF_AGC_TOP_Low_band
				TDA182I2_AGC3_RF_AGC_TOP_102dBuV,				// AGC3_RF_AGC_TOP_High_band
				TDA182I2_AGC4_IR_Mixer_TOP_d110_u105dBuV,		// AGC4_IR_Mixer_TOP
				TDA182I2_AGC5_IF_AGC_TOP_d110_u105dBuV, 		// AGC5_IF_AGC_TOP
				TDA182I2_AGC5_Detector_HPF_Disabled,			// AGC5_Detector_HPF
				TDA182I2_AGC3_Adapt_Enabled,					// AGC3_Adapt
				TDA182I2_AGC3_Adapt_TOP_98dBuV, 				// AGC3_Adapt_TOP
				TDA182I2_AGC5_Atten_3dB_Enabled,				// AGC5_Atten_3dB
				0x02,											// GSK : settings V2.0.0
				TDA182I2_H3H5_VHF_Filter6_Enabled,				// H3H5_VHF_Filter6
				TDA182I2_LPF_Gain_Free							// LPF_Gain
			},
			{													/* DVB-T 7MHz */
				3500000,										// IF
				0,												// CF_Offset
				TDA182I2_LPF_7MHz,								// LPF
				TDA182I2_LPFOffset_min_8pc, 					// LPF_Offset
				TDA182I2_IF_AGC_Gain_1Vpp_min_6_24dB,			// IF_Gain
				TDA182I2_IF_Notch_Enabled,						// IF_Notch
				TDA182I2_IF_HPF_Disabled,						// IF_HPF
				TDA182I2_DC_Notch_Enabled,						// DC_Notch
				TDA182I2_AGC1_LNA_TOP_d95_u89dBuV,				// AGC1_LNA_TOP
				TDA182I2_AGC2_RF_Attenuator_TOP_d90_u84dBuV,  // AGC2_RF_Attenuator_TOP
				TDA182I2_AGC3_RF_AGC_TOP_100dBuV,				// AGC3_RF_AGC_TOP_Low_band
				TDA182I2_AGC3_RF_AGC_TOP_102dBuV,				// AGC3_RF_AGC_TOP_High_band
				TDA182I2_AGC4_IR_Mixer_TOP_d110_u105dBuV,		// AGC4_IR_Mixer_TOP
				TDA182I2_AGC5_IF_AGC_TOP_d110_u105dBuV, 		// AGC5_IF_AGC_TOP
				TDA182I2_AGC5_Detector_HPF_Disabled,			// AGC5_Detector_HPF
				TDA182I2_AGC3_Adapt_Enabled,					// AGC3_Adapt
				TDA182I2_AGC3_Adapt_TOP_98dBuV, 				// AGC3_Adapt_TOP
				TDA182I2_AGC5_Atten_3dB_Enabled,				// AGC5_Atten_3dB
				0x02,											// GSK : settings V2.0.0
				TDA182I2_H3H5_VHF_Filter6_Enabled,				// H3H5_VHF_Filter6
				TDA182I2_LPF_Gain_Free							// LPF_Gain
			},
			{													/* DVB-T 8MHz */
				4000000,										// IF
				0,												// CF_Offset
				TDA182I2_LPF_8MHz,								// LPF
				TDA182I2_LPFOffset_0pc, 						// LPF_Offset
				TDA182I2_IF_AGC_Gain_1Vpp_min_6_24dB,			// IF_Gain
				TDA182I2_IF_Notch_Enabled,						// IF_Notch
				TDA182I2_IF_HPF_Disabled,						// IF_HPF
				TDA182I2_DC_Notch_Enabled,						// DC_Notch
				TDA182I2_AGC1_LNA_TOP_d95_u89dBuV,				// AGC1_LNA_TOP
				TDA182I2_AGC2_RF_Attenuator_TOP_d90_u84dBuV,  // AGC2_RF_Attenuator_TOP
				TDA182I2_AGC3_RF_AGC_TOP_100dBuV,				// AGC3_RF_AGC_TOP_Low_band
				TDA182I2_AGC3_RF_AGC_TOP_102dBuV,				// AGC3_RF_AGC_TOP_High_band
				TDA182I2_AGC4_IR_Mixer_TOP_d110_u105dBuV,		// AGC4_IR_Mixer_TOP
				TDA182I2_AGC5_IF_AGC_TOP_d110_u105dBuV, 		// AGC5_IF_AGC_TOP
				TDA182I2_AGC5_Detector_HPF_Disabled,			// AGC5_Detector_HPF
				TDA182I2_AGC3_Adapt_Enabled,					// AGC3_Adapt
				TDA182I2_AGC3_Adapt_TOP_98dBuV, 				// AGC3_Adapt_TOP
				TDA182I2_AGC5_Atten_3dB_Enabled,				// AGC5_Atten_3dB
				0x02,											// GSK : settings V2.0.0
				TDA182I2_H3H5_VHF_Filter6_Enabled,				// H3H5_VHF_Filter6
				TDA182I2_LPF_Gain_Free							// LPF_Gain
			},
			{													/* QAM 6MHz */
				3600000,										// IF
				0,												// CF_Offset
				TDA182I2_LPF_6MHz,								// LPF
				TDA182I2_LPFOffset_min_8pc, 					// LPF_Offset
				TDA182I2_IF_AGC_Gain_1Vpp_min_6_24dB,			// IF_Gain
				TDA182I2_IF_Notch_Disabled, 					// IF_Notch
				TDA182I2_IF_HPF_Disabled,						// IF_HPF
				TDA182I2_DC_Notch_Enabled,						// DC_Notch
				TDA182I2_AGC1_LNA_TOP_d95_u89dBuV,				// AGC1_LNA_TOP
				TDA182I2_AGC2_RF_Attenuator_TOP_d90_u84dBuV,  // AGC2_RF_Attenuator_TOP
				TDA182I2_AGC3_RF_AGC_TOP_100dBuV,				// AGC3_RF_AGC_TOP_Low_band
				TDA182I2_AGC3_RF_AGC_TOP_100dBuV,				// AGC3_RF_AGC_TOP_High_band
				TDA182I2_AGC4_IR_Mixer_TOP_d110_u105dBuV,		// AGC4_IR_Mixer_TOP
				TDA182I2_AGC5_IF_AGC_TOP_d110_u105dBuV, 		// AGC5_IF_AGC_TOP
				TDA182I2_AGC5_Detector_HPF_Disabled,			// AGC5_Detector_HPF
				TDA182I2_AGC3_Adapt_Disabled,					// AGC3_Adapt
				TDA182I2_AGC3_Adapt_TOP_102dBuV,				// AGC3_Adapt_TOP
				TDA182I2_AGC5_Atten_3dB_Disabled,				// AGC5_Atten_3dB
				0x02,											// GSK : settings V2.0.0
				TDA182I2_H3H5_VHF_Filter6_Disabled, 			// H3H5_VHF_Filter6
				TDA182I2_LPF_Gain_Free							// LPF_Gain
			},
			{													/* QAM 8MHz */
				5000000,										/* IF */
				0,												/* CF_Offset */
				TDA182I2_LPF_9MHz,							/* LPF */
				TDA182I2_LPFOffset_min_8pc,					/* LPF_Offset */
				TDA182I2_IF_AGC_Gain_1Vpp_min_6_24dB, 		/* IF_Gain */
				TDA182I2_IF_Notch_Disabled,					/* IF_Notch */
				TDA182I2_IF_HPF_0_85MHz,						/* IF_HPF */
				TDA182I2_DC_Notch_Enabled,					/* DC_Notch */
				TDA182I2_AGC1_LNA_TOP_d95_u89dBuV,			/* AGC1_LNA_TOP */
				TDA182I2_AGC2_RF_Attenuator_TOP_d90_u84dBuV,	/* AGC2_RF_Attenuator_TOP */
				TDA182I2_AGC3_RF_AGC_TOP_100dBuV, 			/* AGC3_RF_AGC_TOP_Low_band */
				TDA182I2_AGC3_RF_AGC_TOP_100dBuV, 			/* AGC3_RF_AGC_TOP_High_band */
				TDA182I2_AGC4_IR_Mixer_TOP_d110_u105dBuV, 	/* AGC4_IR_Mixer_TOP */
				TDA182I2_AGC5_IF_AGC_TOP_d110_u105dBuV,		/* AGC5_IF_AGC_TOP */
				TDA182I2_AGC5_Detector_HPF_Disabled,			/* AGC5_Detector_HPF */
				TDA182I2_AGC3_Adapt_Disabled, 				/* AGC3_Adapt */
				TDA182I2_AGC3_Adapt_TOP_102dBuV,				/* AGC3_Adapt_TOP */
				TDA182I2_AGC5_Atten_3dB_Disabled, 			/* AGC5_Atten_3dB */
				0x02,											/* GSK : settings V2.0.0  */
				TDA182I2_H3H5_VHF_Filter6_Disabled,			/* H3H5_VHF_Filter6 */
				TDA182I2_LPF_Gain_Free						/* LPF_Gain */
			},
			{													/* ISDBT 6MHz */
				3250000,										// IF
				0,												// CF_Offset
				TDA182I2_LPF_6MHz,								// LPF
				TDA182I2_LPFOffset_0pc, 						// LPF_Offset
				TDA182I2_IF_AGC_Gain_0_6Vpp_min_10_3_19_7dB,  // IF_Gain
				TDA182I2_IF_Notch_Enabled,						// IF_Notch
				TDA182I2_IF_HPF_0_4MHz, 						// IF_HPF
				TDA182I2_DC_Notch_Enabled,						// DC_Notch
				TDA182I2_AGC1_LNA_TOP_d95_u89dBuV,				// AGC1_LNA_TOP
				TDA182I2_AGC2_RF_Attenuator_TOP_d90_u84dBuV,  // AGC2_RF_Attenuator_TOP
				TDA182I2_AGC3_RF_AGC_TOP_100dBuV,				// AGC3_RF_AGC_TOP_Low_band
				TDA182I2_AGC3_RF_AGC_TOP_102dBuV,				// AGC3_RF_AGC_TOP_High_band
				TDA182I2_AGC4_IR_Mixer_TOP_d110_u105dBuV,		// AGC4_IR_Mixer_TOP
				TDA182I2_AGC5_IF_AGC_TOP_d110_u105dBuV, 		// AGC5_IF_AGC_TOP
				TDA182I2_AGC5_Detector_HPF_Disabled,			// AGC5_Detector_HPF
				TDA182I2_AGC3_Adapt_Enabled,					// AGC3_Adapt
				TDA182I2_AGC3_Adapt_TOP_98dBuV, 				// AGC3_Adapt_TOP
				TDA182I2_AGC5_Atten_3dB_Enabled,				// AGC5_Atten_3dB
				0x02,											// GSK : settings V2.0.0
				TDA182I2_H3H5_VHF_Filter6_Enabled,				// H3H5_VHF_Filter6
				TDA182I2_LPF_Gain_Free							// LPF_Gain
			},
			{													/* ATSC 6MHz */
				3250000,										// IF
				0,												// CF_Offset
				TDA182I2_LPF_6MHz,								// LPF
				TDA182I2_LPFOffset_0pc, 						// LPF_Offset
				TDA182I2_IF_AGC_Gain_0_6Vpp_min_10_3_19_7dB,  // IF_Gain
				TDA182I2_IF_Notch_Enabled,						// IF_Notch
				TDA182I2_IF_HPF_0_4MHz, 						// IF_HPF
				TDA182I2_DC_Notch_Enabled,						// DC_Notch
				TDA182I2_AGC1_LNA_TOP_d100_u94dBuV, 			// AGC1_LNA_TOP
				TDA182I2_AGC2_RF_Attenuator_TOP_d90_u84dBuV,  // AGC2_RF_Attenuator_TOP
				TDA182I2_AGC3_RF_AGC_TOP_104dBuV,				// AGC3_RF_AGC_TOP_Low_band
				TDA182I2_AGC3_RF_AGC_TOP_107dBuV,				// AGC3_RF_AGC_TOP_High_band
				TDA182I2_AGC4_IR_Mixer_TOP_d112_u107dBuV,		// AGC4_IR_Mixer_TOP
				TDA182I2_AGC5_IF_AGC_TOP_d112_u107dBuV, 		// AGC5_IF_AGC_TOP
				TDA182I2_AGC5_Detector_HPF_Disabled,			// AGC5_Detector_HPF
				TDA182I2_AGC3_Adapt_Enabled,					// AGC3_Adapt
				TDA182I2_AGC3_Adapt_TOP_96dBuV, 				// AGC3_Adapt_TOP
				TDA182I2_AGC5_Atten_3dB_Enabled,				// AGC5_Atten_3dB
				0x02,											// GSK : settings V2.0.0
				TDA182I2_H3H5_VHF_Filter6_Enabled,				// H3H5_VHF_Filter6
				TDA182I2_LPF_Gain_Free							// LPF_Gain
			},
			{													/* DMB-T 8MHz */
				4000000,										// IF
				0,												// CF_Offset
				TDA182I2_LPF_8MHz,								// LPF
				TDA182I2_LPFOffset_0pc, 						// LPF_Offset
				TDA182I2_IF_AGC_Gain_1Vpp_min_6_24dB,			// IF_Gain
				TDA182I2_IF_Notch_Enabled,						// IF_Notch
				TDA182I2_IF_HPF_Disabled,						// IF_HPF
				TDA182I2_DC_Notch_Enabled,						// DC_Notch
				TDA182I2_AGC1_LNA_TOP_d95_u89dBuV,				// AGC1_LNA_TOP
				TDA182I2_AGC2_RF_Attenuator_TOP_d90_u84dBuV,  // AGC2_RF_Attenuator_TOP
				TDA182I2_AGC3_RF_AGC_TOP_100dBuV,				// AGC3_RF_AGC_TOP_Low_band
				TDA182I2_AGC3_RF_AGC_TOP_102dBuV,				// AGC3_RF_AGC_TOP_High_band
				TDA182I2_AGC4_IR_Mixer_TOP_d110_u105dBuV,		// AGC4_IR_Mixer_TOP
				TDA182I2_AGC5_IF_AGC_TOP_d110_u105dBuV, 		// AGC5_IF_AGC_TOP
				TDA182I2_AGC5_Detector_HPF_Disabled,			// AGC5_Detector_HPF
				TDA182I2_AGC3_Adapt_Enabled,					// AGC3_Adapt
				TDA182I2_AGC3_Adapt_TOP_98dBuV, 				// AGC3_Adapt_TOP
				TDA182I2_AGC5_Atten_3dB_Enabled,				// AGC5_Atten_3dB
				0x02,											// GSK : settings V2.0.0
				TDA182I2_H3H5_VHF_Filter6_Enabled,				// H3H5_VHF_Filter6
				TDA182I2_LPF_Gain_Free							// LPF_Gain
			}
		},
		TRT_UNKNOWN,											//lastTuneReqType
		{
			{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},							// I2CMap
			{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},
			{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},
			{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},
			{0},{0},{0},{0}
		},
		0,														// bSoftReset
		1,														// IRQ wait True
		1,														// bRFCAL_SW_Algo_Enable
		{														// RFCAL_Prog
			{ 0, 0, 0 },
			{ 1, 0, 0 },
			{ 2, 0, 0 },
			{ 3, 0, 0 },
			{ 4, 0, 0 },
			{ 5, 0, 0 },
			{ 6, 0, 0 },
			{ 7, 0, 0 },
			{ 8, 0, 0 },
			{ 9, 0, 0 },
			{ 10, 0, 0 },
			{ 11, 0, 0 }
		},
		{														// RFCAL_Coeffs
			{ 0, 0, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 2, 3, 0, 0 },
			{ 3, 4, 0, 0 },
			{ 4, 6, 0, 0 },
			{ 5, 7, 0, 0 },
			{ 6, 9, 0, 0 },
			{ 7, 10, 0, 0 }
		},
	}
};

/********************************************************************
	Static Function
 ********************************************************************/
/********************************************************************
 * FUNCTION:	TDA182I2AllocInstance
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2AllocInstance( HUINT32 tUnit, ppTDA182I2Object_t ppDrvObject)
{
    TDA182I2_ERR_CODE   reterr = tmERR_BAD_UNIT_NUMBER;
    pTDA182I2Object_t 	pObj = NULL;

	if( tUnit >= NUM_OF_CHANNEL )
	{
		CH_DI_Print(0, ("[TDA182I2AllocInstance]BAD_PARAMETER\n"));
		return reterr;
	}

    /* Find a free instance */
    pObj = &gTDA182I2Instance[tUnit];

    if (pObj != NULL)
    {
    	if (pObj->initDone == FALSE)
		{
			CH_DI_Print(0, ("[TDA182I2AllocInstance]InitDone : %d\n", pObj->initDone ));

	        pObj->tUnit = tUnit;

	        *ppDrvObject = pObj;
	    }
		else
		{
			CH_DI_Print(0, ("[TDA182I2AllocInstance]Already InitDone\n"));
			*ppDrvObject = pObj;
		}
		reterr = tmNOERR;
	}

	pObj->initDone = TRUE;

    return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2AllocInstance
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2DeAllocInstance( HUINT32 tUnit)
{

    TDA182I2_ERR_CODE	reterr = tmERR_BAD_UNIT_NUMBER;
    pTDA182I2Object_t	pObj = NULL;

	if( tUnit >= NUM_OF_CHANNEL)
	{
		CH_DI_Print(0, ("[TDA182I2Init]BAD_PARAMETER\n"));
		return reterr;
	}

    /* check input parameters */
    reterr = TDA182I2GetInstance(tUnit, &pObj);

    /* check driver state */
    if (reterr == tmNOERR)
    {
        if (pObj == NULL || pObj->initDone== FALSE)
        {
            reterr = tmERR_NOT_INITIALIZED;
        }
    }

    if ((reterr == tmNOERR) && (pObj != NULL))
    {
        pObj->initDone = FALSE;
    }

    return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2AllocInstance
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2GetInstance( HUINT32 tUnit, ppTDA182I2Object_t ppDrvObject)
{

    TDA182I2_ERR_CODE  	reterr = tmERR_NOT_INITIALIZED;
    pTDA182I2Object_t 	pObj = NULL;

	if( tUnit >= NUM_OF_CHANNEL )
	{
		CH_DI_Print(0, ("[TDA182I2GetInstance]BAD_PARAMETER\n"));
		return reterr;
	}

    /* get instance */
    pObj = &gTDA182I2Instance[tUnit];
    if(pObj->initDone == TRUE && pObj->tUnit == tUnit)
    {
        *ppDrvObject = pObj;
        reterr= tmNOERR;
    }

    return reterr;
}

