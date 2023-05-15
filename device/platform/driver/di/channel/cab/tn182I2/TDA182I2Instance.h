/*************************************************************************************
 *
 *	TDA182I2Instance.h
 *
 *  QAM receiver : 10024(NXP)
 *  Tuner : FJ2207(NuTune)
 *
 *************************************************************************************/

#ifndef _TDA182I2Instance_H_
#define _TDA182I2Instance_H_
#include"htype.h"
#include"TDA182I2.h"

#if 1
typedef struct _TDA182I2_I2C_Map_t
{
	union
	{
		unsigned char ID_byte_1;
		struct
		{
			__extension__ unsigned char Ident_1:7;
			__extension__ unsigned char MS:1;
		}bF;
	}uBx00;

	union
	{
		unsigned char ID_byte_2;
		struct
		{
			__extension__ unsigned char Ident_2:8;
		}bF;
	}uBx01;

	union
	{
		unsigned char ID_byte_3;
		struct
		{
			__extension__ unsigned char Minor_rev:4;
			__extension__ unsigned char Major_rev:4;
		}bF;
	}uBx02;

	union
	{
		unsigned char Thermo_byte_1;
		struct
		{
			__extension__ unsigned char TM_D:7;
			__extension__ unsigned char UNUSED_I0_D0:1;
		}bF;
	}uBx03;

	union
	{
		unsigned char Thermo_byte_2;
		struct
		{
			__extension__ unsigned char TM_ON:1;
			__extension__ unsigned char UNUSED_I0_D0:7;
		}bF;
	}uBx04;

	union
	{
		unsigned char Power_state_byte_1;
		struct
		{
			__extension__ unsigned char LO_Lock:1;
			__extension__ unsigned char POR:1;
			__extension__ unsigned char UNUSED_I0_D0:6;
		}bF;
	}uBx05;

	union
	{
		unsigned char Power_state_byte_2;
		struct
		{
			__extension__ unsigned char SM_XT:1;
			__extension__ unsigned char SM_LT:1;
			__extension__ unsigned char SM_Synthe:1;
			__extension__ unsigned char SM:1;
			__extension__ unsigned char UNUSED_I0_D0:4;
		}bF;
	}uBx06;

	union
	{
		unsigned char Input_Power_Level_byte;
		struct
		{
			__extension__ unsigned char Power_Level:7;
			__extension__ unsigned char UNUSED_I0_D0:1;
		}bF;
	}uBx07;

	union
	{
		unsigned char IRQ_status;
		struct
		{
			__extension__ unsigned char MSM_RCCal_End:1;
			__extension__ unsigned char MSM_IRCAL_End:1;
			__extension__ unsigned char MSM_RFCal_End:1;
			__extension__ unsigned char MSM_LOCalc_End:1;
			__extension__ unsigned char MSM_RSSI_End:1;
			__extension__ unsigned char MSM_XtalCal_End:1;
			__extension__ unsigned char UNUSED_I0_D0:1;
			__extension__ unsigned char IRQ_status:1;
		}bF;
	}uBx08;

	union
	{
		unsigned char IRQ_enable;
		struct
		{
			__extension__ unsigned char MSM_RCCal_Enable:1;
			__extension__ unsigned char MSM_IRCAL_Enable:1;
			__extension__ unsigned char MSM_RFCAL_Enable:1;
			__extension__ unsigned char MSM_LOCalc_Enable:1;
			__extension__ unsigned char MSM_RSSI_Enable:1;
			__extension__ unsigned char XtalCal_Enable:1;
			__extension__ unsigned char UNUSED_I0_D0:1;
			__extension__ unsigned char IRQ_Enable:1;
		}bF;
	}uBx09;

	union
	{
		unsigned char IRQ_clear;
		struct
		{
			__extension__ unsigned char MSM_RCCal_Clear:1;
			__extension__ unsigned char MSM_IRCAL_Clear:1;
			__extension__ unsigned char MSM_RFCal_Clear:1;
			__extension__ unsigned char MSM_LOCalc_Clear:1;
			__extension__ unsigned char MSM_RSSI_Clear:1;
			__extension__ unsigned char XtalCal_Clear:1;
			__extension__ unsigned char UNUSED_I0_D0:1;
			__extension__ unsigned char IRQ_Clear:1;
		}bF;
	}uBx0A;

	union
	{
		unsigned char IRQ_set;
		struct
		{
			__extension__ unsigned char MSM_RCCal_Set:1;
			__extension__ unsigned char MSM_IRCAL_Set:1;
			__extension__ unsigned char MSM_RFCal_Set:1;
			__extension__ unsigned char MSM_LOCalc_Set:1;
			__extension__ unsigned char MSM_RSSI_Set:1;
			__extension__ unsigned char XtalCal_Set:1;
			__extension__ unsigned char UNUSED_I0_D0:1;
			__extension__ unsigned char IRQ_Set:1;
		}bF;
	}uBx0B;

	union
	{
		unsigned char AGC1_byte_1;
		struct
		{
			__extension__ unsigned char AGC1_TOP:4;
			__extension__ unsigned char UNUSED_I0_D0:2;
			__extension__ unsigned char AGC1_6_15dB:1;
			__extension__ unsigned char LT_Enable:1;
		}bF;
	}uBx0C;

	union
	{
		unsigned char AGC2_byte_1;
		struct
		{
			__extension__ unsigned char AGC2_TOP:5;
			__extension__ unsigned char UNUSED_I0_D0:3;
		}bF;
	}uBx0D;

	union
	{
		unsigned char AGCK_byte_1;
		struct
		{
			__extension__ unsigned char AGCK_Mode:2;
			__extension__ unsigned char AGCK_Step:2;
			__extension__ unsigned char Pulse_Shaper_Disable:1;
			__extension__ unsigned char AGCs_Up_Step:1;
			__extension__ unsigned char AGCs_Up_Step_assym:2;	//driver 1.3.2
		}bF;
	}uBx0E;

	union
	{
		unsigned char RF_AGC_byte;
		struct
		{
			__extension__ unsigned char RFAGC_Top:3;
			__extension__ unsigned char RF_Atten_3dB:1;
			__extension__ unsigned char RFAGC_Low_BW:1;
			__extension__ unsigned char RFAGC_Adapt_TOP:2;
			__extension__ unsigned char PD_RFAGC_Adapt:1;
		}bF;
	}uBx0F;

	union
	{
		unsigned char IR_Mixer_byte_1;
		struct
		{
			__extension__ unsigned char IR_Mixer_Top:4;
			__extension__ unsigned char UNUSED_I0_D0:4;
		}bF;
	}uBx10;

	union
	{
		unsigned char AGC5_byte_1;
		struct
		{
			__extension__ unsigned char AGC5_TOP:4;
			__extension__ unsigned char AGC5_Ana:1;
			__extension__ unsigned char AGCs_Do_Step_assym:2;	//driver 1.3.2
			__extension__ unsigned char UNUSED_I0_D0:1;

		}bF;
	}uBx11;

	union
	{
		unsigned char IF_AGC_byte;
		struct
		{
			__extension__ unsigned char IF_level:3;
			__extension__ unsigned char UNUSED_I0_D0:5;
		}bF;
	}uBx12;

	union
	{
		unsigned char IF_Byte_1;
		struct
		{
			__extension__ unsigned char LP_Fc:3;
			__extension__ unsigned char LP_FC_Offset:2;
			__extension__ unsigned char IF_ATSC_Notch:1;
			__extension__ unsigned char IF_HP_Fc:2;
		}bF;
	}uBx13;

	union
	{
		unsigned char Reference_Byte;
		struct
		{
			__extension__ unsigned char XTout:2;
			__extension__ unsigned char UNUSED_I0_D0:2;
			__extension__ unsigned char XtalOsc_AnaReg_En:1;
			__extension__ unsigned char UNUSED_I1_D0:1;
			__extension__ unsigned char Digital_Clock_Mode:1;
			__extension__ unsigned char I2C_Clock_Mode:1;
		}bF;
	}uBx14;

	union
	{
		unsigned char IF_Frequency_byte;
		struct
		{
			__extension__ unsigned char IF_Freq:8;
		}bF;
	}uBx15;

	union
	{
		unsigned char RF_Frequency_byte_1;
		struct
		{
			__extension__ unsigned char RF_Freq_1:4;
			__extension__ unsigned char UNUSED_I0_D0:4;
		}bF;
	}uBx16;

	union
	{
		unsigned char RF_Frequency_byte_2;
		struct
		{
			__extension__ unsigned char RF_Freq_2:8;
		}bF;
	}uBx17;


	union
	{
		unsigned char RF_Frequency_byte_3;
		struct
		{
			__extension__ unsigned char RF_Freq_3:8;
		}bF;
	}uBx18;

	union
	{
		unsigned char MSM_byte_1;
		struct
		{
			__extension__ unsigned char Calc_PLL:1;
			__extension__ unsigned char RC_Cal:1;
			__extension__ unsigned char IR_CAL_Wanted:1;
			__extension__ unsigned char IR_Cal_Image:1;
			__extension__ unsigned char IR_CAL_Loop:1;
			__extension__ unsigned char RF_CAL:1;
			__extension__ unsigned char RF_CAL_AV:1;
			__extension__ unsigned char RSSI_Meas:1;
		}bF;
	}uBx19;

	union
	{
		unsigned char MSM_byte_2;
		struct
		{
			__extension__ unsigned char MSM_Launch:1;
			__extension__ unsigned char XtalCal_Launch:1;
			__extension__ unsigned char UNUSED_I0_D0:6;
		}bF;
	}uBx1A;

	union
	{
		unsigned char PowerSavingMode;
		struct
		{
			__extension__ unsigned char PSM_Lodriver:2;
			__extension__ unsigned char PSM_Ifpoly:1;
			__extension__ unsigned char PSM_Mixer:1;
			__extension__ unsigned char PSMRFpoly:1;
			__extension__ unsigned char PSM_StoB:1;
			__extension__ unsigned char PSM_AGC1:2;
		}bF;
	}uBx1B;

	union
	{
		unsigned char DCC_byte;
		struct
		{
			__extension__ unsigned char UNUSED_I0_D0:5;
			__extension__ unsigned char DCC_psm:1;
			__extension__ unsigned char DCC_Slow:1;
			__extension__ unsigned char DCC_Bypass:1;
		}bF;
	}uBx1C;

	union
	{
		unsigned char FLO_max_byte;
		struct
		{
			__extension__ unsigned char Fmax_Lo:6;
			__extension__ unsigned char UNUSED_I0_D0:2;
		}bF;
	}uBx1D;

	union
	{
		unsigned char IR_Cal_byte_1;
		struct
		{
			__extension__ unsigned char IR_GStep:3;
			__extension__ unsigned char IR_Target:3;
			__extension__ unsigned char IR_Loop:2;
		}bF;
	}uBx1E;

	union
	{
		unsigned char IR_Cal_byte_2;
		struct
		{
			__extension__ unsigned char IR_FreqLow:5;
			__extension__ unsigned char IR_mode_ram_store:1;
			__extension__ unsigned char IR_FreqLow_Sel:1;
			__extension__ unsigned char IR_Corr_Boost:1;
		}bF;
	}uBx1F;

	union
	{
		unsigned char IR_Cal_byte_3;
		struct
		{
			__extension__ unsigned char IR_FreqMid:5;
			__extension__ unsigned char UNUSED_I0_D0:3;
		}bF;
	}uBx20;

	union
	{
		unsigned char IR_Cal_byte_4;
		struct
		{
			__extension__ unsigned char IR_FreqHigh:5;
			__extension__ unsigned char Coarse_IR_FreqHigh:1;
			__extension__ unsigned char UNUSED_I0_D0:2;
		}bF;
	}uBx21;

	union
	{
		unsigned char Vsync_Mgt_byte;
		struct
		{
			__extension__ unsigned char AGC_Ovld_Timer:2;
			__extension__ unsigned char AGC_Ovld_TOP:3;
			__extension__ unsigned char PD_Udld:1;
			__extension__ unsigned char PD_Ovld:1;
			__extension__ unsigned char PD_Vsync_Mgt:1;
		}bF;
	}uBx22;

	union
	{
		unsigned char IR_Mixer_byte_2;
		struct
		{
			__extension__ unsigned char IF_Notch:1;
			__extension__ unsigned char Hi_Pass:1;
			__extension__ unsigned char UNUSED_I0_D0:3;
			__extension__ unsigned char IR_Mixer_Do_step:2;
			__extension__ unsigned char IR_Mixer_loop_off:1;
		}bF;
	}uBx23;

	union
	{
		unsigned char AGC1_byte_2;
		struct
		{
			__extension__ unsigned char AGC1_Gain:4;
			__extension__ unsigned char Force_AGC1_gain:1;
			__extension__ unsigned char AGC1_Do_step:2;
			__extension__ unsigned char AGC1_loop_off:1;
		}bF;
	}uBx24;

	union
	{
		unsigned char AGC5_byte_2;
		struct
		{
			__extension__ unsigned char AGC5_Gain:2;
			__extension__ unsigned char UNUSED_I0_D0:1;
			__extension__ unsigned char Force_AGC5_gain:1;
			__extension__ unsigned char UNUSED_I1_D0:1;
			__extension__ unsigned char AGC5_Do_step:2;
			__extension__ unsigned char AGC5_loop_off:1;
		}bF;
	}uBx25;

	union
	{
		unsigned char RF_Cal_byte_1;
		struct
		{
			__extension__ unsigned char RFCAL_Freq1:2;
			__extension__ unsigned char RFCAL_Offset_Cprog1:2;
			__extension__ unsigned char RFCAL_Freq0:2;
			__extension__ unsigned char RFCAL_Offset_Cprog0:2;
		}bF;
	}uBx26;

	union
	{
		unsigned char RF_Cal_byte_2;
		struct
		{
			__extension__ unsigned char RFCAL_Freq3:2;
			__extension__ unsigned char RFCAL_Offset_Cprog3:2;
			__extension__ unsigned char RFCAL_Freq2:2;
			__extension__ unsigned char RFCAL_Offset_Cprog2:2;
		}bF;
	}uBx27;

	union
	{
		unsigned char RF_Cal_byte_3;
		struct
		{
			__extension__ unsigned char RFCAL_Freq5:2;
			__extension__ unsigned char RFCAL_Offset_Cprog5:2;
			__extension__ unsigned char RFCAL_Freq4:2;
			__extension__ unsigned char RFCAL_Offset_Cprog4:2;
		}bF;
	}uBx28;

	union
	{
		unsigned char RF_Cal_byte_4;
		struct
		{
			__extension__ unsigned char RFCAL_Freq7:2;
			__extension__ unsigned char RFCAL_Offset_Cprog7:2;
			__extension__ unsigned char RFCAL_Freq6:2;
			__extension__ unsigned char RFCAL_Offset_Cprog6:2;
		}bF;
	}uBx29;

	union
	{
		unsigned char RF_Cal_byte_5;
		struct
		{
			__extension__ unsigned char RFCAL_Freq9:2;
			__extension__ unsigned char RFCAL_Offset_Cprog9:2;
			__extension__ unsigned char RFCAL_Freq8:2;
			__extension__ unsigned char RFCAL_Offset_Cprog8:2;
		}bF;
	}uBx2A;

	union
	{
		unsigned char RF_Cal_byte_6;
		struct
		{
			__extension__ unsigned char RFCAL_Freq11:2;
			__extension__ unsigned char RFCAL_Offset_Cprog11:2;
			__extension__ unsigned char RFCAL_Freq10:2;
			__extension__ unsigned char RFCAL_Offset_Cprog10:2;
		}bF;
	}uBx2B;

	union
	{
		unsigned char RF_Filters_byte_1;
		struct
		{
			__extension__ unsigned char RF_Filter_Band:2;
			__extension__ unsigned char RF_Filter_Gv:2;
			__extension__ unsigned char Force_AGC2_gain:1;
			__extension__ unsigned char AGC2_loop_off:1;
			__extension__ unsigned char UNUSED_I0_D0:1;
			__extension__ unsigned char RF_Filter_Bypass:1;
		}bF;
	}uBx2C;

	union
	{
		unsigned char RF_Filters_byte_2;
		struct
		{
			__extension__ unsigned char RF_Filter_Cap:8;
		}bF;
	}uBx2D;

	union
	{
		unsigned char RF_Filters_byte_3;
		struct
		{
			__extension__ unsigned char Gain_Taper:6;
			__extension__ unsigned char AGC2_Do_step:2;
		}bF;
	}uBx2E;

	union
	{
		unsigned char RF_Band_Pass_Filter_byte;
		struct
		{
			__extension__ unsigned char RF_BPF:3;
			__extension__ unsigned char UNUSED_I0_D0:4;
			__extension__ unsigned char RF_BPF_Bypass:1;
		}bF;
	}uBx2F;

	union
	{
		unsigned char CP_Current_byte;
		struct
		{
			__extension__ unsigned char N_CP_Current:7;
			__extension__ unsigned char UNUSED_I0_D0:1;
		}bF;
	}uBx30;

	union
	{
		unsigned char AGCs_DetOut_byte;
		struct
		{
			__extension__ unsigned char Do_AGC1:1;
			__extension__ unsigned char Up_AGC1:1;
			__extension__ unsigned char Do_AGC2:1;
			__extension__ unsigned char Up_AGC2:1;
			__extension__ unsigned char Do_AGC4:1;
			__extension__ unsigned char Up_AGC4:1;
			__extension__ unsigned char Do_AGC5:1;
			__extension__ unsigned char Up_AGC5:1;
		}bF;
	}uBx31;

	union
	{
		unsigned char RFAGCs_Gain_byte_1;
		struct
		{
			__extension__ unsigned char AGC1_Gain_Read:4;
			__extension__ unsigned char AGC2_Gain_Read:2;
			__extension__ unsigned char UNUSED_I0_D0:2;
		}bF;
	}uBx32;

	union
	{
		unsigned char RFAGCs_Gain_byte_2;
		struct
		{
			__extension__ unsigned char TOP_AGC3_Read:3;
			__extension__ unsigned char UNUSED_I0_D0:5;
		}bF;
	}uBx33;

	union
	{
		unsigned char IFAGCs_Gain_byte;
		struct
		{
			__extension__ unsigned char AGC4_Gain_Read:3;
			__extension__ unsigned char AGC5_Gain_Read:2;
			__extension__ unsigned char UNUSED_I0_D0:3;
		}bF;
	}uBx34;

	union
	{
		__extension__ unsigned char RSSI_byte_1;
		__extension__ unsigned char RSSI;
	}uBx35;

	union
	{
		unsigned char RSSI_byte_2;
		struct
		{
			__extension__ unsigned char RSSI_Dicho_not:1;
			__extension__ unsigned char RSSI_Ck_Speed:1;
			__extension__ unsigned char RSSI_Cap_Val:1;
			__extension__ unsigned char RSSI_Cap_Reset_En:1;
			__extension__ unsigned char UNUSED_I0_D0:1;
			__extension__ unsigned char RSSI_AV:1;
			__extension__ unsigned char UNUSED_I1_D0:2;
		}bF;
	}uBx36;

	union
	{
		unsigned char Misc_byte;
		struct
		{
			__extension__ unsigned char IRQ_Polarity:1;
			__extension__ unsigned char RFCAL_DeltaGain:4;
			__extension__ unsigned char DDS_Polarity:1;
			__extension__ unsigned char RFCAL_Phi2:2;
		}bF;
	}uBx37;

	union
	{
		unsigned char rfcal_log_0;
		struct
		{
			__extension__ unsigned char rfcal_log_0:8;
		}bF;
	}uBx38;

	union
	{
		unsigned char rfcal_log_1;
		struct
		{
			__extension__ unsigned char rfcal_log_1:8;
		}bF;
	}uBx39;

	union
	{
		unsigned char rfcal_log_2;
		struct
		{
			__extension__ unsigned char rfcal_log_2:8;
		}bF;
	}uBx3A;

	union
	{
		unsigned char rfcal_log_3;
		struct
		{
			__extension__ unsigned char rfcal_log_3:8;
		}bF;
	}uBx3B;

	union
	{
		unsigned char rfcal_log_4;
		struct
		{
			__extension__ unsigned char rfcal_log_4:8;
		}bF;
	}uBx3C;

	union
	{
		unsigned char rfcal_log_5;
		struct
		{
			__extension__ unsigned char rfcal_log_5:8;
		}bF;
	}uBx3D;

	union
	{
		unsigned char rfcal_log_6;
		struct
		{
			__extension__ unsigned char rfcal_log_6:8;
		}bF;
	}uBx3E;

	union
	{
		unsigned char rfcal_log_7;
		struct
		{
			__extension__ unsigned char rfcal_log_7:8;
		}bF;
	}uBx3F;

	union
	{
		unsigned char rfcal_log_8;
		struct
		{
			__extension__ unsigned char rfcal_log_8:8;
		}bF;
	}uBx40;

	union
	{
		unsigned char rfcal_log_9;
		struct
		{
			__extension__ unsigned char rfcal_log_9:8;
		}bF;
	}uBx41;

	union
	{
		unsigned char rfcal_log_10;
		struct
		{
			__extension__ unsigned char rfcal_log_10:8;
		}bF;
	}uBx42;

	union
	{
		unsigned char rfcal_log_11;
		struct
		{
			__extension__ unsigned char rfcal_log_11:8;
		}bF;
	}uBx43;

} TDA182I2_I2C_Map_t, *pTDA182I2_I2C_Map_t;


#else
typedef struct _TDA182I2_I2C_Map_t
{
	union
	{
		unsigned char ID_byte_1;
		struct
		{
			unsigned char MS:1;
			unsigned char Ident_1:7;
		}bF;
	}uBx00;

	union
	{
		unsigned char ID_byte_2;
		struct
		{
			unsigned char Ident_2:8;
		}bF;
	}uBx01;

	union
	{
		unsigned char ID_byte_3;
		struct
		{
			unsigned char Major_rev:4;
			unsigned char Minor_rev:4;
		}bF;
	}uBx02;

	union
	{
		unsigned char Thermo_byte_1;
		struct
		{
			unsigned char UNUSED_I0_D0:1;
			unsigned char TM_D:7;
		}bF;
	}uBx03;

	union
	{
		unsigned char Thermo_byte_2;
		struct
		{
			unsigned char UNUSED_I0_D0:7;
			unsigned char TM_ON:1;
		}bF;
	}uBx04;

	union
	{
		unsigned char Power_state_byte_1;
		struct
		{
			unsigned char UNUSED_I0_D0:6;
			unsigned char POR:1;
			unsigned char LO_Lock:1;
		}bF;
	}uBx05;

	union
	{
		unsigned char Power_state_byte_2;
		struct
		{
			unsigned char UNUSED_I0_D0:4;
			unsigned char SM:1;
			unsigned char SM_Synthe:1;
			unsigned char SM_LT:1;
			unsigned char SM_XT:1;
		}bF;
	}uBx06;

	union
	{
		unsigned char Input_Power_Level_byte;
		struct
		{
			unsigned char UNUSED_I0_D0:1;
			unsigned char Power_Level:7;
		}bF;
	}uBx07;

	union
	{
		unsigned char IRQ_status;
		struct
		{
			unsigned char IRQ_status:1;
			unsigned char UNUSED_I0_D0:1;
			unsigned char MSM_XtalCal_End:1;
			unsigned char MSM_RSSI_End:1;
			unsigned char MSM_LOCalc_End:1;
			unsigned char MSM_RFCal_End:1;
			unsigned char MSM_IRCAL_End:1;
			unsigned char MSM_RCCal_End:1;
		}bF;
	}uBx08;

	union
	{
		unsigned char IRQ_enable;
		struct
		{
			unsigned char IRQ_Enable:1;
			unsigned char UNUSED_I0_D0:1;
			unsigned char XtalCal_Enable:1;
			unsigned char MSM_RSSI_Enable:1;
			unsigned char MSM_LOCalc_Enable:1;
			unsigned char MSM_RFCAL_Enable:1;
			unsigned char MSM_IRCAL_Enable:1;
			unsigned char MSM_RCCal_Enable:1;
		}bF;
	}uBx09;

	union
	{
		unsigned char IRQ_clear;
		struct
		{
			unsigned char IRQ_Clear:1;
			unsigned char UNUSED_I0_D0:1;
			unsigned char XtalCal_Clear:1;
			unsigned char MSM_RSSI_Clear:1;
			unsigned char MSM_LOCalc_Clear:1;
			unsigned char MSM_RFCal_Clear:1;
			unsigned char MSM_IRCAL_Clear:1;
			unsigned char MSM_RCCal_Clear:1;
		}bF;
	}uBx0A;

	union
	{
		unsigned char IRQ_set;
		struct
		{
			unsigned char IRQ_Set:1;
			unsigned char UNUSED_I0_D0:1;
			unsigned char XtalCal_Set:1;
			unsigned char MSM_RSSI_Set:1;
			unsigned char MSM_LOCalc_Set:1;
			unsigned char MSM_RFCal_Set:1;
			unsigned char MSM_IRCAL_Set:1;
			unsigned char MSM_RCCal_Set:1;
		}bF;
	}uBx0B;

	union
	{
		unsigned char AGC1_byte_1;
		struct
		{
			unsigned char LT_Enable:1;
			unsigned char AGC1_6_15dB:1;
			unsigned char UNUSED_I0_D0:2;
			unsigned char AGC1_TOP:4;
		}bF;
	}uBx0C;

	union
	{
		unsigned char AGC2_byte_1;
		struct
		{
			unsigned char UNUSED_I0_D0:3;
			unsigned char AGC2_TOP:5;
		}bF;
	}uBx0D;

	union
	{
		unsigned char AGCK_byte_1;
		struct
		{
			unsigned char UNUSED_I0_D0:2;
			unsigned char AGCs_Up_Step:1;
			unsigned char Pulse_Shaper_Disable:1;
			unsigned char AGCK_Step:2;
			unsigned char AGCK_Mode:2;
		}bF;
	}uBx0E;

	union
	{
		unsigned char RF_AGC_byte;
		struct
		{
			unsigned char PD_RFAGC_Adapt:1;
			unsigned char RFAGC_Adapt_TOP:2;
			unsigned char RFAGC_Low_BW:1;
			unsigned char RF_Atten_3dB:1;
			unsigned char RFAGC_Top:3;
		}bF;
	}uBx0F;

	union
	{
		unsigned char IR_Mixer_byte_1;
		struct
		{
			unsigned char UNUSED_I0_D0:4;
			unsigned char IR_Mixer_Top:4;
		}bF;
	}uBx10;

	union
	{
		unsigned char AGC5_byte_1;
		struct
		{
			unsigned char UNUSED_I0_D0:3;
			unsigned char AGC5_Ana:1;
			unsigned char AGC5_TOP:4;
		}bF;
	}uBx11;

	union
	{
		unsigned char IF_AGC_byte;
		struct
		{
			unsigned char UNUSED_I0_D0:5;
			unsigned char IF_level:3;
		}bF;
	}uBx12;

	union
	{
		unsigned char IF_Byte_1;
		struct
		{
			unsigned char IF_HP_Fc:2;
			unsigned char IF_ATSC_Notch:1;
			unsigned char LP_FC_Offset:2;
			unsigned char LP_Fc:3;
		}bF;
	}uBx13;

	union
	{
		unsigned char Reference_Byte;
		struct
		{
			unsigned char I2C_Clock_Mode:1;
			unsigned char Digital_Clock_Mode:1;
			unsigned char UNUSED_I1_D0:1;
			unsigned char XtalOsc_AnaReg_En:1;
			unsigned char UNUSED_I0_D0:2;
			unsigned char XTout:2;
		}bF;
	}uBx14;

	union
	{
		unsigned char IF_Frequency_byte;
		struct
		{
			unsigned char IF_Freq:8;
		}bF;
	}uBx15;

	union
	{
		unsigned char RF_Frequency_byte_1;
		struct
		{
			unsigned char UNUSED_I0_D0:4;
			unsigned char RF_Freq_1:4;
		}bF;
	}uBx16;

	union
	{
		unsigned char RF_Frequency_byte_2;
		struct
		{
			unsigned char RF_Freq_2:8;
		}bF;
	}uBx17;


	union
	{
		unsigned char RF_Frequency_byte_3;
		struct
		{
			unsigned char RF_Freq_3:8;
		}bF;
	}uBx18;

	union
	{
		unsigned char MSM_byte_1;
		struct
		{
			unsigned char RSSI_Meas:1;
			unsigned char RF_CAL_AV:1;
			unsigned char RF_CAL:1;
			unsigned char IR_CAL_Loop:1;
			unsigned char IR_Cal_Image:1;
			unsigned char IR_CAL_Wanted:1;
			unsigned char RC_Cal:1;
			unsigned char Calc_PLL:1;
		}bF;
	}uBx19;

	union
	{
		unsigned char MSM_byte_2;
		struct
		{
			unsigned char UNUSED_I0_D0:6;
			unsigned char XtalCal_Launch:1;
			unsigned char MSM_Launch:1;
		}bF;
	}uBx1A;

	union
	{
		unsigned char PowerSavingMode;
		struct
		{
			unsigned char PSM_AGC1:2;
			unsigned char PSM_StoB:1;
			unsigned char PSMRFpoly:1;
			unsigned char PSM_Mixer:1;
			unsigned char PSM_Ifpoly:1;
			unsigned char PSM_Lodriver:2;
		}bF;
	}uBx1B;

	union
	{
		unsigned char DCC_byte;
		struct
		{
			unsigned char DCC_Bypass:1;
			unsigned char DCC_Slow:1;
			unsigned char DCC_psm:1;
			unsigned char UNUSED_I0_D0:5;
		}bF;
	}uBx1C;

	union
	{
		unsigned char FLO_max_byte;
		struct
		{
			unsigned char UNUSED_I0_D0:2;
			unsigned char Fmax_Lo:6;
		}bF;
	}uBx1D;

	union
	{
		unsigned char IR_Cal_byte_1;
		struct
		{
			unsigned char IR_Loop:2;
			unsigned char IR_Target:3;
			unsigned char IR_GStep:3;
		}bF;
	}uBx1E;

	union
	{
		unsigned char IR_Cal_byte_2;
		struct
		{
			unsigned char IR_Corr_Boost:1;
			unsigned char IR_FreqLow_Sel:1;
			unsigned char IR_mode_ram_store:1;
			unsigned char IR_FreqLow:5;
		}bF;
	}uBx1F;

	union
	{
		unsigned char IR_Cal_byte_3;
		struct
		{
			unsigned char UNUSED_I0_D0:3;
			unsigned char IR_FreqMid:5;
		}bF;
	}uBx20;

	union
	{
		unsigned char IR_Cal_byte_4;
		struct
		{
			unsigned char UNUSED_I0_D0:2;
			unsigned char Coarse_IR_FreqHigh:1;
			unsigned char IR_FreqHigh:5;
		}bF;
	}uBx21;

	union
	{
		unsigned char Vsync_Mgt_byte;
		struct
		{
			unsigned char PD_Vsync_Mgt:1;
			unsigned char PD_Ovld:1;
			unsigned char PD_Udld:1;
			unsigned char AGC_Ovld_TOP:3;
			unsigned char AGC_Ovld_Timer:2;
		}bF;
	}uBx22;

	union
	{
		unsigned char IR_Mixer_byte_2;
		struct
		{
			unsigned char IR_Mixer_loop_off:1;
			unsigned char IR_Mixer_Do_step:2;
			unsigned char UNUSED_I0_D0:3;
			unsigned char Hi_Pass:1;
			unsigned char IF_Notch:1;
		}bF;
	}uBx23;

	union
	{
		unsigned char AGC1_byte_2;
		struct
		{
			unsigned char AGC1_loop_off:1;
			unsigned char AGC1_Do_step:2;
			unsigned char Force_AGC1_gain:1;
			unsigned char AGC1_Gain:4;
		}bF;
	}uBx24;

	union
	{
		unsigned char AGC5_byte_2;
		struct
		{
			unsigned char AGC5_loop_off:1;
			unsigned char AGC5_Do_step:2;
			unsigned char UNUSED_I1_D0:1;
			unsigned char Force_AGC5_gain:1;
			unsigned char UNUSED_I0_D0:1;
			unsigned char AGC5_Gain:2;
		}bF;
	}uBx25;

	union
	{
		unsigned char RF_Cal_byte_1;
		struct
		{
			unsigned char RFCAL_Freq0:4;
			unsigned char RFCAL_Freq1:4;
		}bF;
	}uBx26;

	union
	{
		unsigned char RF_Cal_byte_2;
		struct
		{
			unsigned char RFCAL_Freq2:4;
			unsigned char RFCAL_Freq3:4;
		}bF;
	}uBx27;

	union
	{
		unsigned char RF_Cal_byte_3;
		struct
		{
			unsigned char RFCAL_Freq4:4;
			unsigned char RFCAL_Freq5:4;
		}bF;
	}uBx28;

	union
	{
		unsigned char RF_Cal_byte_4;
		struct
		{
			unsigned char RFCAL_Freq6:4;
			unsigned char RFCAL_Freq7:4;
		}bF;
	}uBx29;

	union
	{
		unsigned char RF_Cal_byte_5;
		struct
		{
			unsigned char RFCAL_Freq8:4;
			unsigned char RFCAL_Freq9:4;
		}bF;
	}uBx2A;

	union
	{
		unsigned char RF_Cal_byte_6;
		struct
		{
			unsigned char RFCAL_Freq10:4;
			unsigned char RFCAL_Freq11:4;
		}bF;
	}uBx2B;

	union
	{
		unsigned char RF_Filters_byte_1;
		struct
		{
			unsigned char RF_Filter_Bypass:1;
			unsigned char UNUSED_I0_D0:1;
			unsigned char AGC2_loop_off:1;
			unsigned char Force_AGC2_gain:1;
			unsigned char RF_Filter_Gv:2;
			unsigned char RF_Filter_Band:2;
		}bF;
	}uBx2C;

	union
	{
		unsigned char RF_Filters_byte_2;
		struct
		{
			unsigned char RF_Filter_Cap:8;
		}bF;
	}uBx2D;

	union
	{
		unsigned char RF_Filters_byte_3;
		struct
		{
			unsigned char AGC2_Do_step:2;
			unsigned char Gain_Taper:6;
		}bF;
	}uBx2E;

	union
	{
		unsigned char RF_Band_Pass_Filter_byte;
		struct
		{
			unsigned char RF_BPF_Bypass:1;
			unsigned char UNUSED_I0_D0:4;
			unsigned char RF_BPF:3;
		}bF;
	}uBx2F;

	union
	{
		unsigned char CP_Current_byte;
		struct
		{
			unsigned char UNUSED_I0_D0:1;
			unsigned char N_CP_Current:7;
		}bF;
	}uBx30;

	union
	{
		unsigned char AGCs_DetOut_byte;
		struct
		{
			unsigned char Up_AGC5:1;
			unsigned char Do_AGC5:1;
			unsigned char Up_AGC4:1;
			unsigned char Do_AGC4:1;
			unsigned char Up_AGC2:1;
			unsigned char Do_AGC2:1;
			unsigned char Up_AGC1:1;
			unsigned char Do_AGC1:1;
		}bF;
	}uBx31;

	union
	{
		unsigned char RFAGCs_Gain_byte_1;
		struct
		{
			unsigned char UNUSED_I0_D0:2;
			unsigned char AGC2_Gain_Read:2;
			unsigned char AGC1_Gain_Read:4;
		}bF;
	}uBx32;

	union
	{
		unsigned char RFAGCs_Gain_byte_2;
		struct
		{
			unsigned char UNUSED_I0_D0:5;
			unsigned char TOP_AGC3_Read:3;
		}bF;
	}uBx33;

	union
	{
		unsigned char IFAGCs_Gain_byte;
		struct
		{
			unsigned char UNUSED_I0_D0:3;
			unsigned char AGC5_Gain_Read:2;
			unsigned char AGC4_Gain_Read:3;
		}bF;
	}uBx34;

	union
	{
		unsigned char RSSI_byte_1;
		unsigned char RSSI;
	}uBx35;

	union
	{
		unsigned char RSSI_byte_2;
		struct
		{
			unsigned char UNUSED_I1_D0:2;
			unsigned char RSSI_AV:1;
			unsigned char UNUSED_I0_D0:1;
			unsigned char RSSI_Cap_Reset_En:1;
			unsigned char RSSI_Cap_Val:1;
			unsigned char RSSI_Ck_Speed:1;
			unsigned char RSSI_Dicho_not:1;
		}bF;
	}uBx36;

	union
	{
		unsigned char Misc_byte;
		struct
		{
			unsigned char RFCAL_Phi2:2;
			unsigned char DDS_Polarity:1;
			unsigned char RFCAL_DeltaGain:4;
			unsigned char IRQ_Polarity:1;
		}bF;
	}uBx37;

	union
	{
		unsigned char rfcal_log_0;
		struct
		{
			unsigned char rfcal_log_0:8;
		}bF;
	}uBx38;

	union
	{
		unsigned char rfcal_log_1;
		struct
		{
			unsigned char rfcal_log_1:8;
		}bF;
	}uBx39;

	union
	{
		unsigned char rfcal_log_2;
		struct
		{
			unsigned char rfcal_log_2:8;
		}bF;
	}uBx3A;

	union
	{
		unsigned char rfcal_log_3;
		struct
		{
			unsigned char rfcal_log_3:8;
		}bF;
	}uBx3B;

	union
	{
		unsigned char rfcal_log_4;
		struct
		{
			unsigned char rfcal_log_4:8;
		}bF;
	}uBx3C;

	union
	{
		unsigned char rfcal_log_5;
		struct
		{
			unsigned char rfcal_log_5:8;
		}bF;
	}uBx3D;

	union
	{
		unsigned char rfcal_log_6;
		struct
		{
			unsigned char rfcal_log_6:8;
		}bF;
	}uBx3E;

	union
	{
		unsigned char rfcal_log_7;
		struct
		{
			unsigned char rfcal_log_7:8;
		}bF;
	}uBx3F;

	union
	{
		unsigned char rfcal_log_8;
		struct
		{
			unsigned char rfcal_log_8:8;
		}bF;
	}uBx40;

	union
	{
		unsigned char rfcal_log_9;
		struct
		{
			unsigned char rfcal_log_9:8;
		}bF;
	}uBx41;

	union
	{
		unsigned char rfcal_log_10;
		struct
		{
			unsigned char rfcal_log_10:8;
		}bF;
	}uBx42;

	union
	{
		unsigned char rfcal_log_11;
		struct
		{
			unsigned char rfcal_log_11:8;
		}bF;
	}uBx43;

} TDA182I2_I2C_Map_t, *pTDA182I2_I2C_Map_t;
#endif


//TDA182I2.h
#define TDA182I2_RFCAL_PROG_ROW (12)
#define TDA182I2_RFCAL_COEFFS_ROW (8)
#define TDA182I2_FREQ_ROM_MAP_ROW (565)
#define tmTDA182I2_AGC3_RF_AGC_TOP_FREQ_LIM 291000000		//driver 1.3.2

typedef struct _tmTDA182I2_CAL_Map_t {
    unsigned char	Cal_number;
    unsigned long  	Cal_Freq[4];
    signed char     CprogOffset_Val[4];
} tmTDA182I2_CAL_Map_t, *ptmTDA182I2_CAL_Map_t;

typedef struct _tmTDA182I2_Freq_ROM_Map_t {
    unsigned long  	uRF_Max;
    unsigned char   uCprog;
    unsigned char   uGainTapper;
    unsigned char   uRFBand;
} tmTDA182I2_Freq_ROM_Map_t, *ptmTDA182I2_Freq_ROM_Map_t;


typedef struct _tmTDA182I2_RFCalProg_t {
    unsigned char Cal_number;
    signed char DeltaCprog;
    signed char CprogOffset;
} tmTDA182I2_RFCalProg_t, *ptmTDA182I2_RFCalProg_t;

typedef struct _tmTDA182I2_RFCalCoeffs_t {
    unsigned char Sub_band;
    unsigned char Cal_number;
    signed long RF_A1;
    signed long RF_B1;
} tmTDA182I2_RFCalCoeffs_t, *ptmTDA182I2_RFCalCoeffs_t;

typedef enum _TDA182I2PowerState_t {
	tmTDA182I2_PowerNormalMode, 									/* Device normal mode */
	tmTDA182I2_PowerStandbyWithLNAOnAndWithXtalOnAndSyntheOn, /* Device standby mode with Loop Through and Xtal Output and Synthe */
	tmTDA182I2_PowerStandbyWithLNAOnAndWithXtalOn,			/* Device standby mode with Loop Through and Xtal Output */
	tmTDA182I2_PowerStandbyWithXtalOn,								/* Device standby mode with Xtal Output */
	tmTDA182I2_PowerStandby,										/* Device standby mode */
	tmTDA182I2_PowerMax
} TDA182I2PowerState_t, *pTDA182I2PowerState_t;

typedef enum _TDA182I2LPF_t {
        TDA182I2_LPF_6MHz = 0,                        /* 6MHz LPFc */
        TDA182I2_LPF_7MHz,                            /* 7MHz LPFc */
        TDA182I2_LPF_8MHz,                            /* 8MHz LPFc */
        TDA182I2_LPF_9MHz,                            /* 9MHz LPFc */
        TDA182I2_LPF_1_5MHz,                          /* 1.5MHz LPFc */
        TDA182I2_LPF_Max
} TDA182I2LPF_t, *pTDA182I2LPF_t;

typedef enum _TDA182I2LPFOffset_t {
    TDA182I2_LPFOffset_0pc = 0,                   /* LPFc 0% */
    TDA182I2_LPFOffset_min_4pc,                   /* LPFc -4% */
    TDA182I2_LPFOffset_min_8pc,                   /* LPFc -8% */
    TDA182I2_LPFOffset_min_12pc,                  /* LPFc -12% */
    TDA182I2_LPFOffset_Max
} TDA182I2LPFOffset_t, *pTDA182I2LPFOffset_t;

typedef enum _TDA182I2IF_AGC_Gain_t {
    TDA182I2_IF_AGC_Gain_2Vpp_0_30dB = 0,         /* 2Vpp       0 - 30dB IF AGC Gain */
    TDA182I2_IF_AGC_Gain_1_25Vpp_min_4_26dB,      /* 1.25Vpp   -4 - 26dB IF AGC Gain */
    TDA182I2_IF_AGC_Gain_1Vpp_min_6_24dB,         /* 1Vpp      -6 - 24dB IF AGC Gain */
    TDA182I2_IF_AGC_Gain_0_8Vpp_min_8_22dB,       /* 0.8Vpp    -8 - 22dB IF AGC Gain */
    TDA182I2_IF_AGC_Gain_0_85Vpp_min_7_5_22_5dB,  /* 0.85Vpp   -7.5 - 22.5dB IF AGC Gain */
    TDA182I2_IF_AGC_Gain_0_7Vpp_min_9_21dB,       /* 0.7Vpp    -9 - 21dB IF AGC Gain */
    TDA182I2_IF_AGC_Gain_0_6Vpp_min_10_3_19_7dB,  /* 0.6Vpp    -10.3 - 19.7dB IF AGC Gain */
    TDA182I2_IF_AGC_Gain_0_5Vpp_min_12_18dB,      /* 0.5Vpp    -12 - 18dB IF AGC Gain */
    TDA182I2_IF_AGC_Gain_Max
} TDA182I2IF_AGC_Gain_t, *pTDA182I2IF_AGC_Gain_t;

typedef enum _TDA182I2IF_Notch_t {
    TDA182I2_IF_Notch_Disabled = 0,               /* IF Notch Enabled */
    TDA182I2_IF_Notch_Enabled,                    /* IF Notch Disabled */
    TDA182I2_IF_Notch_Max
} TDA182I2IF_Notch_t, *pTDA182I2IF_Notch_t;

typedef enum _TDA182I2IF_HPF_t {
    TDA182I2_IF_HPF_Disabled = 0,                 /* IF HPF 0.4MHz */
    TDA182I2_IF_HPF_0_4MHz,                       /* IF HPF 0.4MHz */
    TDA182I2_IF_HPF_0_85MHz,                      /* IF HPF 0.85MHz */
    TDA182I2_IF_HPF_1MHz,                         /* IF HPF 1MHz */
    TDA182I2_IF_HPF_1_5MHz,                       /* IF HPF 1.5MHz */
    TDA182I2_IF_HPF_Max
} TDA182I2IF_HPF_t, *pTDA182I2IF_HPF_t;

typedef enum _TDA182I2DC_Notch_t {
    TDA182I2_DC_Notch_Disabled = 0,               /* IF Notch Enabled */
    TDA182I2_DC_Notch_Enabled,                    /* IF Notch Disabled */
    TDA182I2_DC_Notch_Max
} TDA182I2DC_Notch_t, *pTDA182I2DC_Notch_t;

typedef enum _TDA182I2AGC1_LNA_TOP_t {
    TDA182I2_AGC1_LNA_TOP_d95_u89dBuV = 0,            /* AGC1 LNA TOP down 95 up 89 dBuV */
    TDA182I2_AGC1_LNA_TOP_d95_u93dBuV_do_not_use,     /* AGC1 LNA TOP down 95 up 93 dBuV */
    TDA182I2_AGC1_LNA_TOP_d95_u94dBuV_do_not_use,     /* AGC1 LNA TOP down 95 up 94 dBuV */
    TDA182I2_AGC1_LNA_TOP_d95_u95dBuV_do_not_use,     /* AGC1 LNA TOP down 95 up 95 dBuV */
    TDA182I2_AGC1_LNA_TOP_d99_u89dBuV,				  /* AGC1 LNA TOP down 99 up 89 dBuV */  //driver 1.3.2
    TDA182I2_AGC1_LNA_TOP_d99_u93dBuV,                /* AGC1 LNA TOP down 95 up 93 dBuV */
    TDA182I2_AGC1_LNA_TOP_d99_u94dBuV,                /* AGC1 LNA TOP down 99 up 94 dBuV */
    TDA182I2_AGC1_LNA_TOP_d99_u95dBuV,                /* AGC1 LNA TOP down 99 up 95 dBuV */
    TDA182I2_AGC1_LNA_TOP_d99_u9SdBuV,                /* AGC1 LNA TOP down 99 up 95 dBuV */
    TDA182I2_AGC1_LNA_TOP_d100_u93dBuV,               /* AGC1 LNA TOP down 100 up 93 dBuV */
    TDA182I2_AGC1_LNA_TOP_d100_u94dBuV,               /* AGC1 LNA TOP down 100 up 94 dBuV */
    TDA182I2_AGC1_LNA_TOP_d100_u95dBuV,               /* AGC1 LNA TOP down 100 up 95 dBuV */
    TDA182I2_AGC1_LNA_TOP_d100_u9SdBuV,               /* AGC1 LNA TOP down 100 up 95 dBuV */
    TDA182I2_AGC1_LNA_TOP_d101_u93dBuV,               /* AGC1 LNA TOP down 101 up 93 dBuV */
    TDA182I2_AGC1_LNA_TOP_d101_u94dBuV,               /* AGC1 LNA TOP down 101 up 94 dBuV */
    TDA182I2_AGC1_LNA_TOP_d101_u95dBuV,               /* AGC1 LNA TOP down 101 up 95 dBuV */
    TDA182I2_AGC1_LNA_TOP_d101_u9SdBuV,               /* AGC1 LNA TOP down 101 up 95 dBuV */
    TDA182I2_AGC1_LNA_TOP_Max
} TDA182I2AGC1_LNA_TOP_t, *pTDA182I2AGC1_LNA_TOP_t;

typedef enum _TDA182I2AGC2_RF_Attenuator_TOP_t {
    TDA182I2_AGC2_RF_Attenuator_TOP_d89_u81dBuV = 0, /* AGC2 RF Attenuator TOP down 89 up 81 dBuV */
    TDA182I2_AGC2_RF_Attenuator_TOP_d91_u83dBuV,     /* AGC2 RF Attenuator TOP down 91 up 83 dBuV */
    TDA182I2_AGC2_RF_Attenuator_TOP_d93_u85dBuV,     /* AGC2 RF Attenuator TOP down 93 up 85 dBuV */
    TDA182I2_AGC2_RF_Attenuator_TOP_d95_u87dBuV,     /* AGC2 RF Attenuator TOP down 95 up 87 dBuV */
    TDA182I2_AGC2_RF_Attenuator_TOP_d88_u88dBuV,     /* AGC2 RF Attenuator TOP down 88 up 81 dBuV */
    TDA182I2_AGC2_RF_Attenuator_TOP_d89_u82dBuV,     /* AGC2 RF Attenuator TOP down 89 up 82 dBuV */
    TDA182I2_AGC2_RF_Attenuator_TOP_d90_u83dBuV,     /* AGC2 RF Attenuator TOP down 90 up 83 dBuV */
    TDA182I2_AGC2_RF_Attenuator_TOP_d91_u84dBuV,     /* AGC2 RF Attenuator TOP down 91 up 84 dBuV */
    TDA182I2_AGC2_RF_Attenuator_TOP_d92_u85dBuV,     /* AGC2 RF Attenuator TOP down 92 up 85 dBuV */
    TDA182I2_AGC2_RF_Attenuator_TOP_d93_u86dBuV,     /* AGC2 RF Attenuator TOP down 93 up 86 dBuV */
    TDA182I2_AGC2_RF_Attenuator_TOP_d94_u87dBuV,     /* AGC2 RF Attenuator TOP down 94 up 87 dBuV */
    TDA182I2_AGC2_RF_Attenuator_TOP_d95_u88dBuV,     /* AGC2 RF Attenuator TOP down 95 up 88 dBuV */
    TDA182I2_AGC2_RF_Attenuator_TOP_d87_u81dBuV,     /* AGC2 RF Attenuator TOP down 87 up 81 dBuV */
    TDA182I2_AGC2_RF_Attenuator_TOP_d88_u82dBuV,     /* AGC2 RF Attenuator TOP down 88 up 82 dBuV */
    TDA182I2_AGC2_RF_Attenuator_TOP_d89_u83dBuV,     /* AGC2 RF Attenuator TOP down 89 up 83 dBuV */
    TDA182I2_AGC2_RF_Attenuator_TOP_d90_u84dBuV,     /* AGC2 RF Attenuator TOP down 90 up 84 dBuV */
    TDA182I2_AGC2_RF_Attenuator_TOP_d91_u85dBuV,     /* AGC2 RF Attenuator TOP down 91 up 85 dBuV */
    TDA182I2_AGC2_RF_Attenuator_TOP_d92_u86dBuV,     /* AGC2 RF Attenuator TOP down 92 up 86 dBuV */
    TDA182I2_AGC2_RF_Attenuator_TOP_d93_u87dBuV,     /* AGC2 RF Attenuator TOP down 93 up 87 dBuV */
    TDA182I2_AGC2_RF_Attenuator_TOP_d94_u88dBuV,     /* AGC2 RF Attenuator TOP down 94 up 88 dBuV */
    TDA182I2_AGC2_RF_Attenuator_TOP_d95_u89dBuV,     /* AGC2 RF Attenuator TOP down 95 up 89 dBuV */
    TDA182I2_AGC2_RF_Attenuator_TOP_Max
}TDA182I2AGC2_RF_Attenuator_TOP_t, *pTDA182I2AGC2_RF_Attenuator_TOP_t;

typedef enum _TDA182I2AGC3_RF_AGC_TOP_t {
    TDA182I2_AGC3_RF_AGC_TOP_94dBuV = 0, /* AGC3 RF AGC TOP 94 dBuV */
    TDA182I2_AGC3_RF_AGC_TOP_96dBuV,     /* AGC3 RF AGC TOP 96 dBuV */
    TDA182I2_AGC3_RF_AGC_TOP_98dBuV,     /* AGC3 RF AGC TOP 98 dBuV */
    TDA182I2_AGC3_RF_AGC_TOP_100dBuV,    /* AGC3 RF AGC TOP 100 dBuV */
    TDA182I2_AGC3_RF_AGC_TOP_102dBuV,    /* AGC3 RF AGC TOP 102 dBuV */
    TDA182I2_AGC3_RF_AGC_TOP_104dBuV,    /* AGC3 RF AGC TOP 104 dBuV */
    TDA182I2_AGC3_RF_AGC_TOP_106dBuV,    /* AGC3 RF AGC TOP 106 dBuV */
    TDA182I2_AGC3_RF_AGC_TOP_107dBuV,    /* AGC3 RF AGC TOP 107 dBuV */
    TDA182I2_AGC3_RF_AGC_TOP_Max
} TDA182I2AGC3_RF_AGC_TOP_t, *pTDA182I2AGC3_RF_AGC_TOP_t;

typedef enum _TDA182I2AGC4_IR_Mixer_TOP_t {
    TDA182I2_AGC4_IR_Mixer_TOP_d105_u99dBuV = 0,     /* AGC4 IR_Mixer TOP down 105 up 99 dBuV */
    TDA182I2_AGC4_IR_Mixer_TOP_d105_u100dBuV,        /* AGC4 IR_Mixer TOP down 105 up 100 dBuV */
    TDA182I2_AGC4_IR_Mixer_TOP_d105_u101dBuV,        /* AGC4 IR_Mixer TOP down 105 up 101 dBuV */
    TDA182I2_AGC4_IR_Mixer_TOP_d107_u101dBuV,        /* AGC4 IR_Mixer TOP down 107 up 101 dBuV */
    TDA182I2_AGC4_IR_Mixer_TOP_d107_u102dBuV,        /* AGC4 IR_Mixer TOP down 107 up 102 dBuV */
    TDA182I2_AGC4_IR_Mixer_TOP_d107_u103dBuV,        /* AGC4 IR_Mixer TOP down 107 up 103 dBuV */
    TDA182I2_AGC4_IR_Mixer_TOP_d108_u103dBuV,        /* AGC4 IR_Mixer TOP down 108 up 103 dBuV */
    TDA182I2_AGC4_IR_Mixer_TOP_d109_u103dBuV,        /* AGC4 IR_Mixer TOP down 109 up 103 dBuV */
    TDA182I2_AGC4_IR_Mixer_TOP_d109_u104dBuV,        /* AGC4 IR_Mixer TOP down 109 up 104 dBuV */
    TDA182I2_AGC4_IR_Mixer_TOP_d109_u105dBuV,        /* AGC4 IR_Mixer TOP down 109 up 105 dBuV */
    TDA182I2_AGC4_IR_Mixer_TOP_d110_u104dBuV,        /* AGC4 IR_Mixer TOP down 110 up 104 dBuV */
    TDA182I2_AGC4_IR_Mixer_TOP_d110_u105dBuV,        /* AGC4 IR_Mixer TOP down 110 up 105 dBuV */
    TDA182I2_AGC4_IR_Mixer_TOP_d110_u106dBuV,        /* AGC4 IR_Mixer TOP down 110 up 106 dBuV */
    TDA182I2_AGC4_IR_Mixer_TOP_d112_u106dBuV,        /* AGC4 IR_Mixer TOP down 112 up 106 dBuV */
    TDA182I2_AGC4_IR_Mixer_TOP_d112_u107dBuV,        /* AGC4 IR_Mixer TOP down 112 up 107 dBuV */
    TDA182I2_AGC4_IR_Mixer_TOP_d112_u108dBuV,        /* AGC4 IR_Mixer TOP down 112 up 108 dBuV */
    TDA182I2_AGC4_IR_Mixer_TOP_Max
} TDA182I2AGC4_IR_Mixer_TOP_t, *pTDA182I2AGC4_IR_Mixer_TOP_t;

typedef enum _TDA182I2AGC5_IF_AGC_TOP_t {
    TDA182I2_AGC5_IF_AGC_TOP_d105_u99dBuV = 0,        /* AGC5 IF AGC TOP down 105 up 99 dBuV */
    TDA182I2_AGC5_IF_AGC_TOP_d105_u100dBuV,           /* AGC5 IF AGC TOP down 105 up 100 dBuV */
    TDA182I2_AGC5_IF_AGC_TOP_d105_u101dBuV,           /* AGC5 IF AGC TOP down 105 up 101 dBuV */
    TDA182I2_AGC5_IF_AGC_TOP_d107_u101dBuV,           /* AGC5 IF AGC TOP down 107 up 101 dBuV */
    TDA182I2_AGC5_IF_AGC_TOP_d107_u102dBuV,           /* AGC5 IF AGC TOP down 107 up 102 dBuV */
    TDA182I2_AGC5_IF_AGC_TOP_d107_u103dBuV,           /* AGC5 IF AGC TOP down 107 up 103 dBuV */
    TDA182I2_AGC5_IF_AGC_TOP_d108_u103dBuV,           /* AGC5 IF AGC TOP down 108 up 103 dBuV */
    TDA182I2_AGC5_IF_AGC_TOP_d109_u103dBuV,           /* AGC5 IF AGC TOP down 109 up 103 dBuV */
    TDA182I2_AGC5_IF_AGC_TOP_d109_u104dBuV,           /* AGC5 IF AGC TOP down 109 up 104 dBuV */
    TDA182I2_AGC5_IF_AGC_TOP_d109_u105dBuV,           /* AGC5 IF AGC TOP down 109 up 105 dBuV */
    TDA182I2_AGC5_IF_AGC_TOP_d110_u104dBuV,           /* AGC5 IF AGC TOP down 108 up 104 dBuV */
    TDA182I2_AGC5_IF_AGC_TOP_d110_u105dBuV,           /* AGC5 IF AGC TOP down 108 up 105 dBuV */
    TDA182I2_AGC5_IF_AGC_TOP_d110_u106dBuV,           /* AGC5 IF AGC TOP down 108 up 106 dBuV */
    TDA182I2_AGC5_IF_AGC_TOP_d112_u106dBuV,           /* AGC5 IF AGC TOP down 108 up 106 dBuV */
    TDA182I2_AGC5_IF_AGC_TOP_d112_u107dBuV,           /* AGC5 IF AGC TOP down 108 up 107 dBuV */
    TDA182I2_AGC5_IF_AGC_TOP_d112_u108dBuV,           /* AGC5 IF AGC TOP down 108 up 108 dBuV */
    TDA182I2_AGC5_IF_AGC_TOP_Max
} TDA182I2AGC5_IF_AGC_TOP_t, *pTDA182I2AGC5_IF_AGC_TOP_t;

typedef enum _TDA182I2AGC5_Detector_HPF_t {
    TDA182I2_AGC5_Detector_HPF_Disabled = 0,          /* AGC5_Detector_HPF Enabled */
    TDA182I2_AGC5_Detector_HPF_Enabled,               /* IF Notch Disabled */
    TDA182I2_AGC5_Detector_HPF_Max
} TDA182I2AGC5_Detector_HPF_t, *pTDA182I2AGC5_Detector_HPFh_t;

typedef enum _TDA182I2AGC3_Adapt_t {
    TDA182I2_AGC3_Adapt_Enabled = 0,                  /* AGC3_Adapt Enabled */
    TDA182I2_AGC3_Adapt_Disabled,                     /* AGC3_Adapt Disabled */
    TDA182I2_AGC3_Adapt_Max
} TDA182I2AGC3_Adapt_t, *pTDA182I2AGC3_Adapt_t;

typedef enum _TDA182I2AGC3_Adapt_TOP_t {
    TDA182I2_AGC3_Adapt_TOP_102dBuV = 0,              /* AGC3 Adapt TOP 102 dBuV */
    TDA182I2_AGC3_Adapt_TOP_100dBuV,                  /* AGC3 Adapt TOP 100 dBuV */
    TDA182I2_AGC3_Adapt_TOP_98dBuV,                   /* AGC3 Adapt TOP 98 dBuV */
    TDA182I2_AGC3_Adapt_TOP_96dBuV,                   /* AGC3 Adapt TOP 96 dBuV */
} TDA182I2AGC3_Adapt_TOP_t, *pTDA182I2AGC3_Adapt_TOP_t;

typedef enum _TDA182I2AGC5_Atten_3dB_t {
    TDA182I2_AGC5_Atten_3dB_Disabled = 0,             /* AGC5_Atten_3dB Disabled */
    TDA182I2_AGC5_Atten_3dB_Enabled,                  /* AGC5_Atten_3dB Enabled */
    TDA182I2_AGC5_Atten_3dB_Max
} TDA182I2AGC5_Atten_3dB_t, *pTDA182I2AGC5_Atten_3dB_t;

typedef enum _TDA182I2H3H5_VHF_Filter6_t {
    TDA182I2_H3H5_VHF_Filter6_Disabled = 0,           /* H3H5_VHF_Filter6 Disabled */
    TDA182I2_H3H5_VHF_Filter6_Enabled,                /* H3H5_VHF_Filter6 Enabled */
    TDA182I2_H3H5_VHF_Filter6_Max
} TDA182I2H3H5_VHF_Filter6_t, *pTDA182I2H3H5_VHF_Filter6_t;

typedef enum _TDA182I2_LPF_Gain_t {
    TDA182I2_LPF_Gain_Unknown = 0,                    /* LPF_Gain Unknown */
    TDA182I2_LPF_Gain_Frozen,                         /* LPF_Gain Frozen */
    TDA182I2_LPF_Gain_Free                            /* LPF_Gain Free */
} TDA182I2_LPF_Gain_t, *pTDA182I2_LPF_Gain_t;

typedef struct _TDA182I2StdCoefficients {
	unsigned long						IF;
	signed long							CF_Offset;
	TDA182I2LPF_t 						LPF;
	TDA182I2LPFOffset_t					LPF_Offset;
	TDA182I2IF_AGC_Gain_t 				IF_Gain;
	TDA182I2IF_Notch_t					IF_Notch;
	TDA182I2IF_HPF_t					IF_HPF;
	TDA182I2DC_Notch_t					DC_Notch;
	TDA182I2AGC1_LNA_TOP_t				AGC1_LNA_TOP;
	TDA182I2AGC2_RF_Attenuator_TOP_t	AGC2_RF_Attenuator_TOP;
	TDA182I2AGC3_RF_AGC_TOP_t 			AGC3_RF_AGC_TOP_Low_band;		//driver 1.3.2
	TDA182I2AGC3_RF_AGC_TOP_t 			AGC3_RF_AGC_TOP_High_band;		//driver 1.3.2
	TDA182I2AGC4_IR_Mixer_TOP_t			AGC4_IR_Mixer_TOP;
	TDA182I2AGC5_IF_AGC_TOP_t 			AGC5_IF_AGC_TOP;
	TDA182I2AGC5_Detector_HPF_t			AGC5_Detector_HPF;
	TDA182I2AGC3_Adapt_t				AGC3_Adapt;
	TDA182I2AGC3_Adapt_TOP_t			AGC3_Adapt_TOP;
	TDA182I2AGC5_Atten_3dB_t			AGC5_Atten_3dB;
	unsigned char						GSK;
	TDA182I2H3H5_VHF_Filter6_t			H3H5_VHF_Filter6;
	TDA182I2_LPF_Gain_t					LPF_Gain;
}TDA182I2StdCoefficients, *pTDA182I2StdCoefficients;

typedef struct _TDA182I2Object_t{
	HUINT32          			tUnit;
	HUINT8          			initDone;
	HUINT8              		resetDone;
	HUINT8						uHwAddress;
	TDA182I2PowerState_t    	PowerState;
	TDA182I2_SetPower_t    		curPower;
	HUINT32	          			uRF;
	TDA182I2StandardMode_t		StandardMode;
	TDA182I2StdCoefficients   	Std_Array[tmTDA182I2_StandardMode_Max];
	TuneReqType_t 				lastTuneReqType;
	TDA182I2_I2C_Map_t          I2CMap;
	HUINT8						bSoftReset;
	HUINT8						bIRQWait;		//driver 1.3.1
	HUINT8						bRFCAL_SW_Algo_Enable;
	tmTDA182I2_RFCalProg_t		RFCAL_Prog[TDA182I2_RFCAL_PROG_ROW];
	tmTDA182I2_RFCalCoeffs_t	RFCAL_Coeffs[TDA182I2_RFCAL_COEFFS_ROW];
} TDA182I2Object_t, *pTDA182I2Object_t, **ppTDA182I2Object_t;

TDA182I2_ERR_CODE TDA182I2AllocInstance (HUINT32 tUnit, ppTDA182I2Object_t ppDrvObject);
TDA182I2_ERR_CODE TDA182I2DeAllocInstance (HUINT32 tUnit);
TDA182I2_ERR_CODE TDA182I2GetInstance (HUINT32 tUnit, ppTDA182I2Object_t ppDrvObject);

#endif
