/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnr_7125_tune.h $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 9/7/12 11:08a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/tnr/src/soc/7125ib/btnr_7125_tune.h $
 * 
 * Hydra_Software_Devel/9   9/7/12 11:08a mward
 * SW7425-3765: restructure TNR directory
 * 
 * Hydra_Software_Devel/8   6/1/11 10:13a mward
 * SW7125-956:  Provide the IFAGC_OD_MODE on 7125, but on the 97125 and
 * 97019 Reference Platforms, that option should be on (y).  Default
 * IFAGC_OD_MODE=y (only) for BCHP_CHIP==7125.  Compile with
 * IFAGC_OD_MODE=n for platforms using push-pull AGC.
 * 
 * Hydra_Software_Devel/7   2/10/11 11:27a mward
 * SW7125-810:  Function to check 65 vs. 60 nm.
 * 
 * Hydra_Software_Devel/6   1/31/11 3:47p mward
 * SW7125-810:  Initial support for 60 nm chips.
 * 
 * Hydra_Software_Devel/5   8/16/10 9:43a mward
 * SW7125-513:  Merge.
 * 
 * Hydra_Software_Devel/SW7125-513/4   8/12/10 6:50p mward
 * SW7125-513:  Default AGC threshold from BNM code, not measured value.
 * Works better.
 * 
 * Hydra_Software_Devel/SW7125-513/3   8/10/10 11:30a mward
 * SW7125-513:  Default AGC ranges from values reported by BNM on an
 * actual 97125 board.
 * 
 * Hydra_Software_Devel/SW7125-513/2   8/5/10 10:43a mward
 * SW7125-513: Reconcile with DOCSIS code from STB_3.0.1alpha1_07302010.
 * Add VGA calibration.
 * 
 * Hydra_Software_Devel/SW7125-513/1   8/3/10 7:06p mward
 * SW7125-513: Reconcile with DOCSIS release STB_3.0.1alpha1_07302010.
 * 
 * Hydra_Software_Devel/4   7/28/10 7:17p mward
 * SW7125-513:  Bring in AlignAGC2() from DOCSIS code.
 * 
 * Hydra_Software_Devel/3   7/22/10 2:52p mward
 * SW7125-513: Add BTNR_7125_P_ReConfig_LNA() from DOCSIS code.
 * 
 * Hydra_Software_Devel/2   7/16/10 2:17p mward
 * SW7125-513: Add back last 7550 history to show origin version.
 * 
 * Hydra_Software_Devel/1   7/12/10 3:21p mward
 * SW7125-513:  Add host control of 7125 internal DS and tuner.  Copy and
 * adapt 7550 implementation.
 * 
 * Hydra_Software_Devel/25   5/18/10 12:04p farshidf
 * SW7550-440: merge to main branch
 * 
 ***************************************************************************/

#ifndef _BTNR_7125_TUNE_H__
#define _BTNR_7125_TUNE_H__           

#ifdef __cplusplus
extern "C" {
#endif

/*DPM Offset that is board dependent, checked with a 0dbmV input @ 207 MHz*/
#define DPM_OFFSET_256dbmV 15384  /*This gain needs to be 256 times larger so 3 db is 768, -5.5 is -1408*/

/*Divider Values for DPM tone*/
#define DPM_8MHZ_OUTDIV_M6 0x20 /*540000000/(4*(0x20+2))= 3970588.235 Hz*/
#define DPM_7MHZ_OUTDIV_M6 0x25 /*540000000/(4*(0x25+2))= 3461538.462 Hz*/
#define DPM_6MHZ_OUTDIV_M6 0x2B /*540000000/(4*(0x2B+2))= 3000000.000 Hz*/
#define DPM_5MHZ_OUTDIV_M6 0x34 /*540000000/(4*(0x34+2))= 2500000.000 Hz*/

/*	 DPM Frequency Control Word:
Frequency Control word may be determined as follows:

    DPMFCW = round{2^24 * (Fdpm in MHz)/(fs/2 in MHz)}
	Add 3 to DPMFCW to offset FCW for better averaging.
*/
#define DPM_8MHZ_DPMFCW 0x22C04D /*2^24 * 3.970588235 / 29.25 */
#define DPM_7MHZ_DPMFCW 0x1E4BC0 /*2^24 * 3.461538462 / 29.25 */
#define DPM_6MHZ_DPMFCW 0x1A41A7 /*2^24 * 3.000000000 / 29.25 */
#define DPM_5MHZ_DPMFCW 0x15E161 /*2^24 * 2.500000000 / 29.25 */

	/*Minimum and Maximum tuner frequencies allowed*/
/*these are also the minimum and maximum checkpoints in the Tuner_Freq_Selection_Table*/
#define MIN_TUNER_FREQ 33437500
#define MAX_TUNER_FREQ 1162500000

/*breakpoint for use of Harmonic Rejection Filter*/
#define TUNER_F_RFTRK  348000000

/*breakpoint between VHF and UHF path when in Terrrestraial Mode*/
#define VHF_UHF_BREAKPOINT 240000000

/*Constants*/
#define POWER2_31 2147483648UL
#define POWER2_29 536870912
#define POWER2_24			 16777216
#define POWER2_14      16384
#define POWER3_3       27
#define POWER5_7       78125

/*Thresholds for the window used to determine fixed gain settings in*/
/*BTNR_7125_P_Tuner_LoopVGAGainRange()*/
#ifdef IFAGC_OD_MODE
	#define MAX_AGC_THRESHOLD 	0x3C000000 /*1.45v*/
	#define MIN_AGC_THRESHOLD 	0x26000000 /*1.20v*/
	#define MAX_TUNER_VGA_LEVEL 0x70000000 /*2.15v*/
	#define MIN_TUNER_VGA_LEVEL 0xD0000000 /*0.50v*/
	#define MAX_GAIN_VGA_LEVEL  0x4F000000 /*1.70v*/
	#define MIN_GAIN_VGA_LEVEL  0x1C000000 /*1.10v*/
#else
	#define MAX_AGC_THRESHOLD 	0x1B000000 /*1.45v*/
	#define MIN_AGC_THRESHOLD 	0x00000000 /*1.20v*/
	#define MAX_TUNER_VGA_LEVEL 0x7F000000 /*2.35v*/
	#define MIN_TUNER_VGA_LEVEL 0xC0000000 /*0.60v*/
	#define MAX_GAIN_VGA_LEVEL  0x33000000 /*1.70v*/
	#define MIN_GAIN_VGA_LEVEL  0xF2000000 /*1.10v*/
#endif
#define AVG_MAXMIN_THRESHOLD (MAX_AGC_THRESHOLD + MIN_AGC_THRESHOLD)/2

/*Initial Filter BW's for Cable Mode*/
#define DS_IFLPF_BWR_SEL_8MHz 0x0F
#define DS_FGA_RC_CNTL_SEL_8MHz 0x37
#define DS_IFLPF_BWR_SEL_7MHz 0x09
#define DS_FGA_RC_CNTL_SEL_7MHz 0x32
#define DS_IFLPF_BWR_SEL_6MHz 0x07
#define DS_FGA_RC_CNTL_SEL_6MHz 0x2F
#define DS_IFLPF_BWR_SEL_5MHz 0x04
#define DS_FGA_RC_CNTL_SEL_5MHz 0x29

/*Breakpoints for BTNR_7125_P_Tuner_FGA_BW_Selection() and BTNR_7125_P_Get_Status_TNR_Core0*/
/*Changes in the number of breakpoints will require changes in both functions*/
#define IFLPF_BWR_BREAKPOINT1 3
#define IFLPF_BWR_BREAKPOINT2 4
#define IFLPF_BWR_BREAKPOINT3 15

/*Iniital Values for BTNR_P_7125_AcquireSettings_t Structure*/
/*These values can also be overwriten by BBS*/
/*BBS needs everything passed as uint32_t*/
#define ACQWORD0 0								/*spare Acquisition Paramater*/ /*Begining of Debugging Words*/
#define ACQWORD1 0								/*spare Acquisition Paramater*/
#define ACQWORD2 0								/*spare Acquisition Paramater*/
#define ACQWORD3 0								/*spare Acquisition Paramater*/

#define FORCE_TUNERACQUISITIONTYPE 0          	/* 0 Auto, 1 InitTune, 2 Tune, 3 MiniTune*/

#define LOOPVGAGAIN_INIT_DWELL 8				/*number BKNI_Delay(500) to let AGC converge*/  
#define LOOPVGAGAIN_VGA_DWELL 8					/*number BKNI_Delay(500) to let AGC converge during trimming when VGA is changing*/
#define LOOPVGAGAIN_NOTVGA_DWELL 4				/*number BKNI_Delay(500) to let AGC converge during trimming when VGA is not changing*/
#define LOOPVGAGAIN_DISPLAY   0					/*print mvarAGCTI during BTNR_7125_P_Tuner_LoopVGAGainRange*/
#define INPUT_SOURCE 0							/*0 Internal Tuner: 1 for external tuner or IF*/
#define RERUN_INIT 0							/*Rerun The BTNR_7125_P_Init_TNR_Core0() Routine from BTNR_7125_P_Tune_TNR_Core0()*/
#define DS_RF_AGC_ACQ_BW 0xd
#define DS_IF_AGC_ACQ_BW 0xd
#define DS_RF_AGC_TRK_BW 0x15
#define DS_IF_AGC_TRK_BW 0x15
#define DS_DCOC_ON	1		 					/* set to "1" to enable the IF DCOC in the AFE */
#define DS_DCOC_ACQ_BW 0x8
#define DS_DCOC_TRK_BW 0xB
#define DS_BCM3410_AUTO	1						/* 0 for fixed, 1 for Auto */
#define DS_BCM3410_GAIN_STATE 31				/* This can range from 1 to 31 (only affects fixed gain mode) */
#define DS_LNA_BOOST_ON 0						/* 0 for Boost off, 1 for Boost on */
#define DS_LNA_TILT_ON 1						/* 0 for Tilt off, 1 for Tilt on */
#define DS_LNA_TILT_FREQ 550000000				/* Tilt Frequency */
#define DEFAULT_TERRESTRIAL_LNA_ADDRESS 0x60  	/*Default used to program initial value in structure, is then scanned for by LNA.c*/
#define DEFAULT_CABLE_LNA_ADDRESS 0x61        	/*Default used to program initial value in structure, is then scanned for by LNA.c*/
#define DS_MIN_AGC_THRESHOLD 0x22BFDB4C	 		/* Lower AGC Threshold default. should be set by VGA calibration BNM cal'd: 0x22c7d248 */
#define DS_MID_AGC_THRESHOLD 0x2B81232A    		/* Mid range of AGC default. should be set by VGA calibration */
#define DS_MAX_AGC_THRESHOLD 0x2D32B55E    		/* 0x34426b08 Upper Threshold default. should be set by VGA calibration was 0x2D32B55E */

#define DS_MIN_AGC_RAIL	0x0						/* AGC Rail defaults. should be set by VGA calibration */
#define DS_MAX_AGC_RAIL	0x7fffffff


#define CABLE_LNA_BASE_ADDRESS 0x60			/*Cable LNA Address on 97125 Reference Platform */

typedef enum IFVGA_BW_s
{
	IFVGA_BW_eAcquisition = 0,
	IFVGA_BW_eTracking = 1
}IFVGA_BW_t;

typedef enum DCOC_Mode_s
{
	DCOC_Mode_eFreezeReset = 0,
	DCOC_Mode_eRun = 1
}DCOC_Mode_t;

/*******************************************************************************************************
*declare table to get Daisy Gain BTNR_7125_P_Tune_TNR_Core0() ******************************************
********************************************************************************************************/
#define  Daisy_Gain_Table_Size 32

typedef struct Daisy_Gain_Selection_Elements_s
{
	uint8_t DS_GAIN_CTR;
}Daisy_Gain_Selection_Elements_t;

static const Daisy_Gain_Selection_Elements_t Daisy_Gain_Selection_Table[Daisy_Gain_Table_Size] =
{ 
	{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, 
	{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, 
	{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, 
	{0}, {0}, {0}, {0}, {1}, {1}, {1}, {1} 
};




/*******************************************************************************************************
*declare table to get LPF Gains for BTNR_7125_P_Tune_TNR_Core0() and BTNR_7125_P_Tuner_AlignAGC2() *****
********************************************************************************************************/
#define  AlignAgc2_Gain_Table_Size 32

typedef struct AlignAgc2_Gain_Selection_Elements_s
{
	uint8_t RF_ATT_Range;
	uint8_t  RFTRK_Gain_Range;
}AlignAgc2_Gain_Selection_Elements_t;

static const AlignAgc2_Gain_Selection_Elements_t AlignAgc2_Gain_Selection_Table[AlignAgc2_Gain_Table_Size] =
{
	{3, 0}, {3, 0}, {3, 0}, {3, 0}, {3, 0}, {3, 0}, {3, 0}, {3, 0},
	{3, 0}, {3, 0}, {3, 0}, {3, 0}, {3, 0}, {3, 0}, {3, 0}, {3, 0},
	{3, 0}, {3, 0}, {3, 0}, {3, 0}, {3, 0}, {3, 0}, {3, 0}, {3, 0},
	{3, 0}, {2, 0}, {2, 0}, {2, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 1}
};

/*******************************************************************************************************
*declare table to get FGA_RC parameters for BTNR_7125_P_Tuner_FGA_BW_Selection() routine ***************
********************************************************************************************************/
#define  FGA_RC_Table_Size 8
#define  FGA_RC_Num_Tables 14

typedef struct FGA_RC_Selection_Elements_s
{
	uint8_t  FGA_RC_CNTL;
}FGA_RC_Selection_Elements_t;

/* declare table to get FGA_RC parameters */
static const FGA_RC_Selection_Elements_t FGA_RC_Selection_Table[FGA_RC_Num_Tables][FGA_RC_Table_Size] = 
{
   {{0},{0},{2},{8},{17},{26},{33},{39}},
   {{0},{1},{11},{21},{29},{36},{41},{46}},
   {{0},{7},{16},{25},{32},{38},{43},{48}},
   {{0},{11},{20},{28},{35},{41},{45},{50}},
   {{4},{15},{23},{31},{37},{43},{47},{51}},
   {{8},{19},{26},{33},{39},{45},{49},{52}},
   {{12},{22},{29},{36},{41},{46},{50},{53}},
   {{15},{25},{31},{38},{43},{48},{51},{54}},
   {{18},{27},{33},{39},{44},{49},{52},{55}},
   {{21},{29},{35},{41},{46},{50},{53},{56}},
   {{23},{31},{37},{42},{47},{51},{54},{57}},
   {{25},{33},{39},{44},{48},{52},{55},{57}},
   {{27},{35},{40},{45},{49},{53},{55},{58}},
   {{29},{36},{41},{46},{50},{53},{56},{58}}
};

/*******************************************************************************************************
* declare table elements for VCO used in the BTNR_7125_P_Set_Tuner_Freq() routine **********************
*the ssval11 and div_sel need to have a 1-1 mapping for the BTNR_7125_P_Get_Status_TNR_Core0() routine *
********************************************************************************************************/
#define Tuner_Freq_Selection_Num_Tables 2 
#define Tuner_Freq_Selection_Table_Size 12
typedef struct Tuner_Freq_Selection_Elements_s
{
	uint32_t tuner_freq_range;
	uint8_t ssval1;
	uint8_t div_sel;
	uint8_t tunctl;
}Tuner_Freq_Selection_Elements_t;

/*declare table to get VCO and DDFS programming parameters */
static const Tuner_Freq_Selection_Elements_t Tuner_Freq_Selection_Table[Tuner_Freq_Selection_Num_Tables][Tuner_Freq_Selection_Table_Size] = 
{
    {
        /* for 65nm */
    	{33437499,   0, 0, 0}, /*error frequency too low (MIN_TUNER_FREQ-1) */
    	{48437500,  64, 6, 1},
    	{72656250,  64, 6, 2},
    	{96875000,  32, 2, 1},
    	{145312500, 32, 2, 2},
    	{193750000, 16, 4, 1},
    	{290625000, 16, 4, 2},
    	{387500000,  8, 0, 1},
    	{581250000,  8, 0, 2},
    	{775000000,  4, 1, 1},
    	{1162500000, 4, 1, 2}, /*MAX_TUNER_FREQ*/
    	{1162500001, 0, 0, 0} /*error frequency too high*/
    },
    {
        /* for 60nm */
        {39218750,   0, 0, 0}, /*error frequency too low (MIN_TUNER_FREQ-1) */
        {58750000,  64, 6, 1},
        {78437500,  64, 6, 2},
        {117500000,  32, 2, 1},
        {156875000, 32, 2, 2},
        {235000000, 16, 4, 1},
        {313750000, 16, 4, 2},
        {470000000,  8, 0, 1},
        {627500000,  8, 0, 2},
        {940000000,  4, 1, 1},
        {1255000000, 4, 1, 2}, /*MAX_TUNER_FREQ*/
        {1255000001, 0, 0, 0} /*error frequency too high*/
    }
};

/*******************************************************************************************************
* declare table for LPF parameters used in the BTNR_7125_P_Set_Tuner_LPF() routine *********************
********************************************************************************************************/
#define LPF_Table_Size 64

typedef struct LPF_Selection_Elements_s
{
	uint32_t freq;
	uint8_t  rftrk1_in;
}LPF_Selection_Elements_t;

/*this is the original table values with each value multiplied by 1000000*4/5*/
static const LPF_Selection_Elements_t LPF_Selection_Table[LPF_Table_Size] = 
{
	{440960000, 7},{368698400, 7},{327020800, 7},{295060800, 7},{267504800, 7},{246078400, 7},{226706400, 7},{211293600, 7},
	{196712000, 7},{184606400, 7},{173451200, 7},{164412800, 7},{155444000, 5},{148614400, 5},{141256000, 5},{135237600, 5},
	{129076800, 5},{121339200, 5},{116498400, 5},{112511200, 5},{108302400, 5},{105183200, 5},{101487200, 5},{ 98428800, 5},
	{ 95168800, 5},{ 92466400, 5},{ 89576800, 5},{ 87198400, 5},{ 84617600, 5},{ 82716800, 5},{ 80396800, 5},{ 78455360, 3},
	{ 76362640, 3},{ 70454720, 3},{ 68856720, 3},{ 67536960, 3},{ 66057680, 3},{ 64992960, 3},{ 63622160, 3},{ 62467840, 3},
	{ 61187200, 3},{ 60129120, 3},{ 58933920, 3},{ 57950560, 3},{ 56825360, 3},{ 56029360, 3},{ 54984240, 3},{ 54108240, 3},
	{ 53133200, 3},{ 52098560, 3},{ 51197600, 3},{ 50451040, 3},{ 49593440, 3},{ 48985840, 3},{ 48188960, 3},{ 47510000, 3},
	{ 46752000, 3},{ 46131360, 3},{ 45410400, 3},{ 44810880, 3},{ 44138560, 3},{ 43649520, 3},{ 43003040, 3},{ 42465200, 3}
};

/*Main functions used in btnr_7125_tune.c*/
void BTNR_7125_P_Set_Tuner_Freq(BTNR_7125_Handle);
void BTNR_7125_P_Set_Tuner_DDFS(BTNR_7125_Handle, uint8_t);
void BTNR_7125_P_Set_Tuner_VCO(BTNR_7125_Handle, uint8_t);
void BTNR_7125_P_Set_Tuner_LPF(BTNR_7125_Handle);
void BTNR_7125_P_Set_Tuner_IFVGA_BW(BTNR_7125_Handle, IFVGA_BW_t);
void BTNR_7125_P_Set_Tuner_DCOC_Mode(BTNR_7125_Handle, DCOC_Mode_t);
void BTNR_7125_P_Tuner_Set_BB_LPF_Filter(BTNR_7125_Handle);
void BTNR_7125_P_Tuner_AlignAGC2(BTNR_7125_Handle, uint8_t);
void BTNR_7125_P_Tuner_LoopVGAGainRange(BTNR_7125_Handle);
void BTNR_7125_P_Tuner_FGA_BW_Selection(BTNR_7125_Handle);

/*Utility functions btnr_7125_utils.c*/
void BTNR_7125_P_Tuner_Init_Buffer(BTNR_7125_Handle);
void BTNR_7125_P_Range_Check(BTNR_7125_Handle);
uint32_t BTNR_7125_P_2560log10(uint32_t);
void BTNR_7125_P_Tuner_MXR_DCO_DAC(BTNR_7125_Handle);
void BTNR_7125_P_Tuner_MXR_DCO_LOOP(BTNR_7125_Handle);				 
void BTNR_7125_P_Tuner_VGA_DCO_LOOP(BTNR_7125_Handle);	
void BTNR_7125_P_Tuner_VGA_CAL(BTNR_7125_Handle h);
void BTNR_7125_P_Tuner_Device_type(BTNR_7125_Handle h);

/*EXTERNAL LNA FUNCTIONS*/
void BTNR_7125_P_Init_LNA(BTNR_7125_Handle);
void BTNR_7125_P_Set_LNA_Boost(BTNR_7125_Handle);
void BTNR_7125_P_Set_LNA_Tilt(BTNR_7125_Handle);
uint8_t BTNR_7125_P_Get_LNA_Gain(BTNR_7125_Handle);
void BTNR_7125_P_Set_LNA_Output(BTNR_7125_Handle);
void BTNR_7125_P_PowerDown_LNA_Output(BTNR_7125_Handle h, bool PowerDown);
void BTNR_7125_P_Probe_LNA(BTNR_7125_Handle);
void BTNR_7125_P_ReConfig_LNA(BTNR_7125_Handle);
#ifdef __cplusplus
}
#endif

#endif /* _BTNR_7125_TUNE_H__ */


