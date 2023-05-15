/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_g2_priv_tuner.c $
 * $brcm_Revision: Hydra_Software_Devel/77 $
 * $brcm_Date: 9/6/12 6:18p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g2/common/bast_g2_priv_tuner.c $
 * 
 * Hydra_Software_Devel/77   9/6/12 6:18p ronchan
 * SWSATFE-214: rename 73xx to g2
 * 
 * Hydra_Software_Devel/76   7/9/12 11:55a ronchan
 * SWSATFE-205: label functions in isr context
 * 
 * Hydra_Software_Devel/75   5/2/12 3:42p ckpark
 * SWSATFE-191: changes for oqpsk
 * 
 * Hydra_Software_Devel/74   3/9/12 5:09p ronchan
 * SW7335-1402: initialize dacpll on ch0 only
 * 
 * Hydra_Software_Devel/73   1/21/11 4:07p enavarro
 * SW7340-246: fixed calculation of normalized I/Q in DCO convergence
 * algorithm
 * 
 * Hydra_Software_Devel/72   1/20/10 10:53a ckpark
 * SWSATFE-15: fixed overflow bug involving LO divider in TunerRampMb2()
 * 
 * Hydra_Software_Devel/71   12/3/09 3:59p enavarro
 * SWSATFE-16: fixed compiler warning
 * 
 * Hydra_Software_Devel/70   11/30/09 2:23p ckpark
 * SWSATFE-15: fixed overflow bug in TUNER_MB2 when LO divider is 16
 * 
 * Hydra_Software_Devel/69   11/23/09 3:10p ckpark
 * SWSATFE-15: increased settling delay after quick tune; SWSATFE-16:
 * changed LO divider at 1701 MHz to avoid spur in BCM7335
 * 
 * Hydra_Software_Devel/68   9/25/09 3:10p enavarro
 * SW7340-40: added code to use dac/mix pll state machines
 * 
 * Hydra_Software_Devel/67   9/24/09 5:06p enavarro
 * SW7340-40: add code to use DACPLL/MIXPLL state machines for BCM734x
 * 
 * Hydra_Software_Devel/66   9/10/09 9:29a enavarro
 * SW7325-594: moved functions used for fixed point arithmetic to common
 * location
 * 
 * Hydra_Software_Devel/65   8/27/09 5:03p enavarro
 * HW4506-78: use common function to set default sample freq
 * 
 * Hydra_Software_Devel/64   8/10/09 3:50p ronchan
 * PR 55235: configure vco based on xtal freq instead of chip
 * 
 * Hydra_Software_Devel/63   8/10/09 11:15a ronchan
 * PR 55235: add support for 7340 and 7342
 * 
 * Hydra_Software_Devel/62   6/30/09 5:25p ckpark
 * PR56263: Ramp time is set 5usec
 * 
 * Hydra_Software_Devel/61   6/26/09 11:10a ckpark
 * PR 56263: updated BAST_73xx_P_TunerRampMb2()
 * 
 * Hydra_Software_Devel/60   6/24/09 4:01p ckpark
 * PR 56263: added FLI compensation in MB2 ramp
 * 
 * Hydra_Software_Devel/59   6/23/09 3:51p ckpark
 * PR 56263: added BAST_73xx_P_TunerRampMb2() and
 * BAST_73xx_P_ComputeActualTunerFreq(); add tunerOffset to
 * actualTunerFreq
 * 
 * Hydra_Software_Devel/58   5/26/09 3:41p enavarro
 * PR 53391: calibrate manual LPF setting in SET_FILTER_MANUAL mode
 * 
 * Hydra_Software_Devel/57   4/15/09 10:37a enavarro
 * PR 51665: reset mixpll loselock sticky bit 20 usecs after
 * TunerSetNotchFilter() to allow settling after writing qpbiascnt value;
 * do image filter calibration during tune/acquire only if BG=4; if image
 * filter calibration was successful, redo tuneMix(); reset mixpll
 * loselock sticky bit at end of QuickTune(); wait 60 usecs before
 * resetting mixpll loselock bit to start monitoring for lost lock for up
 * to 7 msecs; do 7 msecs stable mixpll lock checking for BG=7 always
 * 
 * Hydra_Software_Devel/56   3/12/09 4:22p enavarro
 * PR 51665: update to latest tuner mixpll retry algorithm
 * 
 * Hydra_Software_Devel/55   3/6/09 11:32a enavarro
 * PR 52796: changed programming of Rcntl and QPbiasCNT2 in
 * BAST_73xx_P_TunerSetRcntlQpbiascnt2()
 * 
 * Hydra_Software_Devel/54   3/2/09 4:03p enavarro
 * PR 51665: reworked tuner mixpll lock retries; set tune mixpll delay to
 * 60 usecs
 * 
 * Hydra_Software_Devel/53   2/26/09 2:22p enavarro
 * PR 51665: implemented revised algorithm for tuner mixpll locking
 * 
 * Hydra_Software_Devel/52   2/13/09 11:10a ronchan
 * PR 37769: fixed compile warning
 * 
 * Hydra_Software_Devel/51   2/13/09 10:47a enavarro
 * PR 51665: added gmx adjustment
 * 
 * Hydra_Software_Devel/50   2/12/09 4:33p enavarro
 * PR 51665: dont retry if mixer pll not locked at end of
 * BAST_73xx_P_TunerInit(); adjust tuner image filter after the first
 * successful LPF calibration
 * 
 * Hydra_Software_Devel/49   2/12/09 2:14p enavarro
 * PR 51665: added dynamic bandgap setting; added dco convergence check
 * 
 * Hydra_Software_Devel/48   2/10/09 9:55a ronchan
 * PR 51665: added high bandgap option with 30ms DCO delay
 * 
 * Hydra_Software_Devel/47   1/31/09 1:29a jrubio
 * PR51629: add 7336 support
 * 
 * Hydra_Software_Devel/46   1/29/09 6:32p ronchan
 * PR 51629: added support for 7336
 * 
 * Hydra_Software_Devel/45   10/31/08 3:15p enavarro
 * PR 37769: removed TONE_SEARCH_MODE code
 * 
 * Hydra_Software_Devel/44   10/1/08 2:24p enavarro
 * PR 37769: disable notch filter settings from previous acquisition
 * 
 * Hydra_Software_Devel/43   8/22/08 3:28p enavarro
 * PR 37769: removed debug message
 * 
 * Hydra_Software_Devel/42   8/14/08 4:03p enavarro
 * PR 37769: reset ADC data capture FIFO before LPF cal; set
 * ANACTL8.DEC_PRE_QP_I='001' for B0 and later
 * 
 * Hydra_Software_Devel/41   7/18/08 4:31p enavarro
 * PR 37769: added IMF tuning
 * 
 * Hydra_Software_Devel/40   7/16/08 4:28p enavarro
 * PR 37769: dont do LPF cal for Fb < 6Msym/sec
 * 
 * Hydra_Software_Devel/39   7/7/08 3:10p ckpark
 * PR 37769: changed pll bw to improve dac pulling & crosstalk, eliminate
 * fddfs avoidance from splitter mode
 * 
 * Hydra_Software_Devel/38   7/3/08 9:55a enavarro
 * PR 37769: changes to LPF cal for peak scan mode
 * 
 * Hydra_Software_Devel/37   7/1/08 12:07p enavarro
 * PR 37769: enable manual setting of tuner LPF
 * 
 * Hydra_Software_Devel/36   6/26/08 2:28p enavarro
 * PR 37881: modified BAST_73xx_P_TunerQuickTune() to support splitter
 * mode; splitter mode changes
 * 
 * Hydra_Software_Devel/35   6/20/08 12:46p enavarro
 * PR 37769: fixed compiler warning; use alternate VCO selection
 * boundaries for all versions of 7335
 * 
 * Hydra_Software_Devel/34   6/17/08 2:55p enavarro
 * PR 37881: changed VCO select boundaries for 7335-Ax
 * 
 * Hydra_Software_Devel/33   6/13/08 5:20p enavarro
 * PR 37769: updated actualTunerFreq calculation
 * 
 * Hydra_Software_Devel/32   6/13/08 11:31a enavarro
 * PR 37769: make alternate Fddfs to be 1134MHz
 * 
 * Hydra_Software_Devel/31   6/9/08 2:23p enavarro
 * PR 37769: added tuner LPF cal failsafe
 * 
 * Hydra_Software_Devel/30   6/3/08 3:37p enavarro
 * PR 37769: do LPF cal for peak scan
 * 
 * Hydra_Software_Devel/29   6/2/08 3:45p enavarro
 * PR 37769: removed debug message
 * 
 * Hydra_Software_Devel/28   5/27/08 11:31a ronchan
 * PR 37769: return vco in BAST_73xx_P_TunerGetExpectedVco()
 * 
 * Hydra_Software_Devel/27   5/23/08 10:59a ronchan
 * PR 37769: added option to compile out splitter mode
 * 
 * Hydra_Software_Devel/25   5/22/08 3:09p enavarro
 * PR 37881: added Fddfs avoidance algorithm in splitter mode
 * 
 * Hydra_Software_Devel/24   5/21/08 2:24p ckpark
 * PR 37881: dont snap to other vco in splitter mode
 * 
 * Hydra_Software_Devel/23   5/20/08 10:59a enavarro
 * PR 37881: changes to splitter mode
 * 
 * Hydra_Software_Devel/21   5/16/08 4:26p enavarro
 * PR 37881: changes for splitter mode
 * 
 * Hydra_Software_Devel/20   5/14/08 2:25p enavarro
 * PR 37769: adjusted tuner filter for non-LO tracking mode; added
 * failsafe logic in LPF calibration
 * 
 * Hydra_Software_Devel/19   5/13/08 2:05p enavarro
 * PR 37769: do Fddfs switch for chips that only have 1 internal tuner
 * 
 * Hydra_Software_Devel/18   5/9/08 6:02p enavarro
 * PR 37769: added support for Fddfs switching; widen tuner filter for
 * splitter mode
 * 
 * Hydra_Software_Devel/17   5/1/08 12:54p enavarro
 * PR 37769: put back BAST_73xx_P_TunerFsSmartTune()
 * 
 * Hydra_Software_Devel/16   4/30/08 4:00p enavarro
 * PR 37769: dont reprogram sample clock in BAST_73xx_P_TunerFsSmartTune()
 * 
 * Hydra_Software_Devel/15   4/30/08 3:09p enavarro
 * PR 37769: put ANACTL8 at the high setting for first DCO initialization
 * 
 * Hydra_Software_Devel/14   4/30/08 2:52p enavarro
 * PR 37769: when resetting the DCOs, keep ANACTL20[3:2] high for 150 uS
 * instead of 100 uS; increase tuner bw by 3MHz if LO tracking is
 * disabled
 * 
 * Hydra_Software_Devel/13   4/28/08 2:31p ronchan
 * PR 37881: fixed dac div ratio for 7335
 * 
 * Hydra_Software_Devel/12   3/12/08 1:34p enavarro
 * PR 37769: set TUNER_ANACTL8.DEC_PRE_QP_I='111' during initialization
 * and during tuning, set to '011' elsewhere
 * 
 * Hydra_Software_Devel/11   3/5/08 10:29a enavarro
 * PR 37769: changed BAST_73xx_P_TunerSetDigctl7() per changes in 4506
 * 
 * Hydra_Software_Devel/10   2/13/08 4:00p ronchan
 * PR 37769: added null check for post tuner init function pointer
 * 
 * Hydra_Software_Devel/9   2/13/08 9:32a enavarro
 * PR 37769: changed tuneDAC popcap delay and VcREF setting per James
 * Chang
 * 
 * Hydra_Software_Devel/8   2/4/08 4:11p enavarro
 * PR 37769: renamed BAST_73xx_P_LdpcSetDigctl7() to
 * BAST_73xx_P_TunerSetDigctl7()
 * 
 * Hydra_Software_Devel/7   1/18/08 4:09p enavarro
 * PR 37769: add debug msg whenever tune_mix fails
 * 
 * Hydra_Software_Devel/6   1/18/08 11:33a enavarro
 * PR 37769: make BAST_73xx_P_TunerSetFreq() return BERR_Code
 * 
 * Hydra_Software_Devel/5   12/19/07 3:09p enavarro
 * PR 37769: set popcap delay to 60us in tune_dac; set Fs=135Mhz with
 * VCO=1080MHz
 * 
 * Hydra_Software_Devel/4   12/14/07 5:31p ronchan
 * PR 37881: added tuner dac div ratio
 * 
 * Hydra_Software_Devel/3   12/12/07 2:34p enavarro
 * PR 37769: add support for 54MHz xtal
 * 
 * Hydra_Software_Devel/2   11/29/07 2:11p ronchan
 * PR 37769: fixed problem with tuner not tuning below 1130 MHz
 * 
 * Hydra_Software_Devel/1   11/21/07 11:33a ronchan
 * PR 32395: initial version
 *
 ***************************************************************************/
#include "bstd.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_g2_priv.h"

BDBG_MODULE(bast_g2_priv_tuner);

/* #define BAST_TUNE_MIX_DEBUG */
/* #define BAST_USE_TUNER_PLL_STATE_MACHINE */
/* #define BAST_DEBUG_TUNER_PLL_STATE_MACHINE */

/* local routines */
BERR_Code BAST_g2_P_TunerTuneDac(BAST_ChannelHandle h, BAST_g2_FUNCT nextFunct);
BERR_Code BAST_g2_P_TunerTuneDac1(BAST_ChannelHandle h);
void BAST_g2_P_TunerGetFddfs(BAST_ChannelHandle h);
void BAST_g2_P_TunerSetMb2(BAST_ChannelHandle h);
void BAST_g2_P_TunerSetRcntlQpbiascnt2(BAST_ChannelHandle h);
void BAST_g2_P_TunerFsSmartTune(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_TunerTuneMix(BAST_ChannelHandle h, BAST_g2_FUNCT next_funct);
BERR_Code BAST_g2_P_TunerTuneMix0(BAST_ChannelHandle h);
void BAST_g2_P_TunerTuneMix1(BAST_ChannelHandle h);
void BAST_g2_P_TunerSelectVco(BAST_ChannelHandle h);
void BAST_g2_P_TunerSetNotchFilter(BAST_ChannelHandle h);
void BAST_g2_P_TunerGetLoDivider(BAST_ChannelHandle h);
void BAST_g2_P_TunerComputeCutoff(BAST_ChannelHandle h);
void BAST_g2_P_TunerSetTestTone(BAST_ChannelHandle h, uint32_t hz);
uint32_t BAST_g2_P_TunerGetAvePower(BAST_ChannelHandle h);
void BAST_g2_P_TunerSetFilter(BAST_ChannelHandle h, BAST_g2_FUNCT next_funct);
BERR_Code BAST_g2_P_TunerSetFreq1(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_TunerInit0(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_TunerInit1(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_TunerInit2(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_TunerSetFilter1(BAST_ChannelHandle h);
uint32_t BAST_g2_P_TunerGetExpectedVco(BAST_ChannelHandle h);
uint32_t BAST_g2_P_TunerGetActualVco(BAST_ChannelHandle h);
uint32_t BAST_g2_P_TunerCalculateVco(BAST_ChannelHandle h, uint32_t fcw);
uint32_t BAST_g2_P_TunerCalculateFcw(BAST_ChannelHandle h);
void BAST_g2_P_TunerUpdateActualTunerFreq(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_TunerSetBandgap(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_TunerQuickTune1(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_TunerQuickTune2(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_TunerSetFddfs1(BAST_ChannelHandle h);


/******************************************************************************
 BAST_g2_P_TunerInit() - ISR context
******************************************************************************/
void BAST_g2_P_TunerInit(BAST_ChannelHandle h, BAST_g2_FUNCT nextFunct)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;
   bool bDacLocked, bMixerLocked;

   static const uint32_t script_tuner_init[] =
   {
      BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL2, 0x10), /* notch/LO/mixer */
      BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL3, 0x83), /* mixer bias */
      BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL4, 0xB0), /* LPF */
      BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL5, 0x88), /* LPF/IF amp */
      BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL6, 0xA6), /* output buffer; A4 normally, must set to 0xA6 to probe I/Q outputs */
      BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL7, 0x08), /* LPF cal/DCO power down */
      BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL8, 0x3B), /* DCO increase post QP current for faster LPF cal, DCO_PRE_QP_I='111' */
      BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL10, 0x2A), /* LO */
      BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL11, 0x5A), /* LO */
      BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL12, 0x40), /* LO */
      BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL13, 0x38), /* LO: bit 7 = 0 for A0, 1 for B0 */
      BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL14, 0xA4), /* LO */
      /* BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL15, 0x2D), */ /* LO */
      BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL15, 0x3F), /* SEK 7/3/08 higher LO PLL BW for improved pulling  */
      BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL16, 0x0C), /* LO */
      BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL17, 0x63), /* LO */
      BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL18, 0x61), /* DAC image filter */
      BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL19, 0xC8), /* DAC */
      BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL20, 0x01), /* DAC PLL */
      BAST_SCRIPT_EXIT
   };

   static const uint32_t script_tuner_dac_pll_init[] =
   {
      BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL21, 0x50), 
      BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL22, 0x18), 
      BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL23, BAST_G2_TUNER_DAC_DIV_RATIO), 
      BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL24, 0x19), 
      BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL25, 0x4A), 
      BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL26, 0x00),
      /* BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL27, 0x0D), */
      BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL27, 0x1F), /* SEK 7/3/08 higher DAC PLL BW for improved pulling */
      BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL28, 0x0C), 
      BAST_SCRIPT_EXIT
   };

   static const uint32_t script_tuner_digital_init[] =
   {
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL4, 0x02),
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL11, 0x00), /* Deassert tuner block reset */
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL12, 0x20),
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL13, 0x04),
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL14, 0x18), /* Power down LPF cal DDFS/DAC clocks */
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL15, 0xFF), /* set tuner pre-DCO bw */
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL16, 0xFF), /* set tuner post-DCO bw */
      BAST_SCRIPT_EXIT
   };

   BDBG_ASSERT(hChn->bExternalTuner == false);

   val = 0x01;
   BAST_g2_P_WriteRegister(h, BCHP_TUNER_DIGCTL11, &val);
   BAST_g2_P_TunerSetBandgap(h);

   hChn->postTunerInitFunct = nextFunct;
   BAST_g2_P_ProcessScript(h, script_tuner_init);
   BAST_g2_P_TunerGetLockStatus(h, &bDacLocked, &bMixerLocked);
   if (!bDacLocked)
      BAST_g2_P_ProcessScript(h->pDevice->pChannels[0], script_tuner_dac_pll_init);
   BAST_g2_P_ProcessScript(h, script_tuner_digital_init);
   
   if (bDacLocked == false)
   {
      BAST_g2_P_TunerTuneDac(h, BAST_g2_P_TunerInit0);
      return;
   }

   BAST_g2_P_TunerInit0(h);
}


/******************************************************************************
 BAST_g2_P_TunerInit0() - ISR context
******************************************************************************/
BERR_Code BAST_g2_P_TunerInit0(BAST_ChannelHandle h)
{   
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;
   
   BAST_g2_P_TunerGetFddfs(h);

   /* get initial values of gmx and dac_ictl */
   BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL18, &val);
   hChn->tunerGmx = (uint8_t)(val & 0x3C);
   BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL19, &val);
   hChn->tunerDacIctl = (uint8_t)(val & 0x0F);
   
   
   hChn->tunerFreq = 2180000000UL;
   hChn->tunerIfStep = 0;
   hChn->acqParams.carrierFreqOffset = 0;
   return BAST_g2_P_TunerSetFreq(h, BAST_g2_P_TunerInit1);
}


/******************************************************************************
 BAST_g2_P_TunerInit1() - ISR context
******************************************************************************/
BERR_Code BAST_g2_P_TunerInit1(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;

   BAST_g2_P_OrRegister(h, BCHP_TUNER_ANACTL8, 0x38); /* DEC_PRE_QP_I='111' */  
   
   hChn->funct_state = 0;
   return BAST_g2_P_TunerInit2(h);
}


/******************************************************************************
 BAST_g2_P_TunerInit2() - ISR context
******************************************************************************/
BERR_Code BAST_g2_P_TunerInit2(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;
   BERR_Code retCode;

   switch (hChn->funct_state)
   {
      case 0:
         hChn->funct_state = 1;
         return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 100, BAST_g2_P_TunerInit2); 

      case 1:
         val = 0x0D;
         BAST_g2_P_WriteRegister(h, BCHP_TUNER_ANACTL20, &val);
         hChn->funct_state = 2;
         return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 150, BAST_g2_P_TunerInit2); 

      case 2:
         val = 0x01;
         BAST_g2_P_WriteRegister(h, BCHP_TUNER_ANACTL20, &val);
        
         if (hChn->postTunerInitFunct != NULL)
            return hChn->postTunerInitFunct(h);
         else
            return BERR_SUCCESS;
   }

   BERR_TRACE(retCode = BERR_INVALID_PARAMETER);
   return retCode;
}


/******************************************************************************
 BAST_g2_P_TunerGetLockStatus()
******************************************************************************/
void BAST_g2_P_TunerGetLockStatus(BAST_ChannelHandle h, bool *bDacLocked, bool *bMixerLocked)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;
   
   if (hChn->bHasTunerDacPll)
      BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL29, &val);
   else
      val = BREG_Read32(((BAST_g2_P_Handle *)(h->pDevice->pImpl))->hRegister, BCHP_TUNER_ANACTL29);

   *bMixerLocked = false;
   if (val & 0x02)
   {
      *bDacLocked = true;
      BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL29, &val);
      if (val & 0x08)
         *bMixerLocked = true;
   }
   else
      *bDacLocked = false;
      
#if 0      
   if ((*bMixerLocked == false) || (*bDacLocked == false))
   {
      BDBG_MSG(("dacLock=%d, MixLock=%d", *bDacLocked, *bMixerLocked));
   }
#endif   
}


/******************************************************************************
 BAST_g2_P_TunerGetFddfs()
******************************************************************************/
void BAST_g2_P_TunerGetFddfs(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   BAST_g2_P_Handle *hDev = (BAST_g2_P_Handle *)h->pDevice->pImpl;
   uint32_t val;

   if (hChn->bHasTunerDacPll)
      BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL23, &val);
   else
      val = BREG_Read32(hDev->hRegister, BCHP_TUNER_ANACTL23); /* TBD: this temp hack assumes dac pll on tuner0 */
   
   hChn->tunerFddfs = hDev->xtalFreq * ((val & 0x7F) + 2);
   BDBG_MSG(("Fddfs=%d", hChn->tunerFddfs));
}


/******************************************************************************
 BAST_g2_P_TunerSetFddfs()
******************************************************************************/
BERR_Code BAST_g2_P_TunerSetFddfs(BAST_ChannelHandle h, BAST_g2_FUNCT nextFunct)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   BAST_g2_P_Handle *hDev = (BAST_g2_P_Handle *)(h->pDevice->pImpl);
   uint32_t lo_vco = 1107000000UL;
   uint32_t diff, desired_Fddfs;   
   uint8_t tune_dac_div_ratio;
   
   hChn->postSetFddfsFunct = nextFunct;
   if ((hChn->bHasTunerDacPll) && (hDev->numInternalTuners == 1))
   {
      if (hChn->actualTunerFreq > lo_vco)
         diff = hChn->actualTunerFreq - lo_vco;
      else
         diff = lo_vco - hChn->actualTunerFreq;
      if (diff < 20000000)
      {
         desired_Fddfs = 1134000000UL;
         tune_dac_div_ratio = BAST_G2_TUNER_DAC_DIV_RATIO + 1;       
      }
      else
      {
         desired_Fddfs = 1107000000UL;
         tune_dac_div_ratio = BAST_G2_TUNER_DAC_DIV_RATIO;
      }
         
      if (hChn->tunerFddfs != desired_Fddfs)
      {     
         BDBG_MSG(("setting Fddfs=%d", desired_Fddfs));      
         hChn->tuneDacDivRatio = tune_dac_div_ratio & 0x7F;
         return BAST_g2_P_TunerTuneDac(h, BAST_g2_P_TunerSetFddfs1);               
      }
   }  

   return hChn->postSetFddfsFunct(h);; 
}


/******************************************************************************
 BAST_g2_P_TunerSetFddfs1()
******************************************************************************/
BERR_Code BAST_g2_P_TunerSetFddfs1(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   
   BAST_g2_P_TunerGetFddfs(h);
   return hChn->postSetFddfsFunct(h);
}


/******************************************************************************
 BAST_g2_P_TunerSetMb2()
******************************************************************************/
void BAST_g2_P_TunerSetMb2(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t Q_hi, Q_lo, P_hi, P_lo;

   /* BDBG_MSG(("BAST_g2_P_TunerSetMb2(): %u Hz, LO divider=%d, Fddfs=%d", hChn->actualTunerFreq, hChn->tunerLoDivider >> 1, hChn->tunerFddfs)); */
   BAST_MultU32U32(hChn->actualTunerFreq, 67108864, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, hChn->tunerFddfs, &Q_hi, &Q_lo);  
   BAST_DivU64U32(Q_hi, Q_lo, hChn->tunerLoDivider, &Q_hi, &Q_lo);   
   Q_lo = (Q_lo + 1) >> 1; 
   Q_lo = Q_lo << 8;
   Q_lo &= 0xFFFFFF00;
   BAST_g2_P_WriteRegister(h, BCHP_TUNER_MB2, &Q_lo);
   /* BDBG_MSG(("MB2=0x%08X", Q_lo)); */
}


/******************************************************************************
 BAST_g2_P_TunerFsSmartTune()
******************************************************************************/
void BAST_g2_P_TunerFsSmartTune(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;

   if (hChn->bExternalTuner == false)
   {
      /* just do this for now */
      BAST_g2_P_SetDefaultSampleFreq(h);
   }
}


/******************************************************************************
 BAST_g2_P_TunerSetRcntlQpbiascnt2()
******************************************************************************/
void BAST_g2_P_TunerSetRcntlQpbiascnt2(BAST_ChannelHandle h)
{
   uint32_t rcntl, qpbiascnt2, anactl11, anactl12, anactl16, anactl17;

   static const uint32_t tune_mix_qpbiascnt2[] =
   {
      13, 12, 8, 4
   };

   BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL11, &anactl11);
   BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL12, &anactl12);
   BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL16, &anactl16);
   BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL17, &anactl17);
   anactl12 &= 0x3F;

   if ((anactl11 & 0xC0) == 0x40)
   {
      /* VCO_sel = 1 */
      if (anactl12 >= 32)
         rcntl = 0;
      else if (anactl12 >= 11)
         rcntl = 1;
      else if (anactl12 >= 5)
         rcntl = 2;
      else
         rcntl = 3;
   }
   else
   {
      /* VCO_sel = 2 */
      if (anactl12 >= 47)
         rcntl = 0;
      else if (anactl12 >= 15)
         rcntl = 1;
      else if (anactl12 >= 5)
         rcntl = 2;
      else
         rcntl = 3;
   }
   
   qpbiascnt2 = tune_mix_qpbiascnt2[rcntl];
#ifdef BAST_DEBUG_TUNER_PLL_STATE_MACHINE   
   BDBG_MSG(("qpbiascnt2=%d, rcntl=%d", qpbiascnt2, rcntl));
#endif   
   BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL17, 0xFFFFFFF3, (rcntl << 2) & 0x0C);
   BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL16, 0xFFFFFFE0, qpbiascnt2);
}


/******************************************************************************
 BAST_g2_P_TunerTuneDac() - ISR context
******************************************************************************/
BERR_Code BAST_g2_P_TunerTuneDac(BAST_ChannelHandle h, BAST_g2_FUNCT nextFunct)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;

   hChn->postTuneMixFunct = nextFunct;
   
   if (hChn->bHasTunerDacPll == false)
      hChn->tuneDacState = 0xFF;   
   else   
      hChn->tuneDacState = 0;
   return BAST_g2_P_TunerTuneDac1(h);
}


/******************************************************************************
 BAST_g2_P_TunerTuneDac1() - ISR context
******************************************************************************/
BERR_Code BAST_g2_P_TunerTuneDac1(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;
   bool bDacLocked, bMixerLocked;
   const uint32_t tune_dac_qp2_cnt = 0x0B;
   const uint32_t tune_dac_vcref = 0x18;
   const uint32_t popcap_delay = 100; /* delay in usecs */
   
#if (((BCHP_CHIP==7340) || (BCHP_CHIP==7342)) && defined(BAST_USE_TUNER_PLL_STATE_MACHINE))
   uint32_t delay;
#else      
   uint32_t val2, tuner_bit;
   
   static const uint32_t script_tuner_dac[] =
   {
      BAST_SCRIPT_OR(BCHP_TUNER_ANACTL21, 0x04), /* Reset_LF = 1 */
      BAST_SCRIPT_OR(BCHP_TUNER_ANACTL22, 0x80), /* Div_reset = 1 */
      BAST_SCRIPT_AND(BCHP_TUNER_ANACTL21, 0xFB), /* Reset_LF = 0 */
      BAST_SCRIPT_AND(BCHP_TUNER_ANACTL22, 0x7F), /* Div_reset = 0 */
      BAST_SCRIPT_EXIT
   };
#endif   
   
   while (hChn->tuneDacState != 0xFF)
   {
      switch (hChn->tuneDacState)
      {
         case 0:
            BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL28, 0xE0, tune_dac_qp2_cnt & 0x1F);
            BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL23, 0x80, hChn->tuneDacDivRatio & 0x7F);   
            BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL22, 0xE0, tune_dac_vcref & 0x1F);
            
#if (((BCHP_CHIP==7340) || (BCHP_CHIP==7342)) && defined(BAST_USE_TUNER_PLL_STATE_MACHINE))
            val = (((popcap_delay * 200000) / 2963) + 1) >> 1;
            val |= 0x30000;           
            BAST_g2_P_WriteRegister(h, BCHP_TUNER_DIGCTL18, &val);
            hChn->tuneDacState = 2;
            BAST_g2_P_ReadRegister(h, BCHP_TUNER_DIGCTL21, &val);            
            delay = (((val & 0x0007FFFF) * 3375) / 10000) + (popcap_delay * 8);
            return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, delay, BAST_g2_P_TunerTuneDac1);              
#else
            val = 0x80;
            BAST_g2_P_WriteRegister(h, BCHP_TUNER_ANACTL26, &val);
 
            tuner_bit = 7;
            
            start_pll_locking:
            BAST_g2_P_ProcessScript(h, script_tuner_dac);
            BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, popcap_delay, NULL); 
            
            /* read popcap */
            BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL29, &val);
            val &= 1;
            BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL26, &val2);
            if (val)
            {
               /* region high */
               val2 &= ~(1 << tuner_bit);
               BAST_g2_P_WriteRegister(h, BCHP_TUNER_ANACTL26, &val2);
            }
            if (tuner_bit)
            {
               tuner_bit--;
               BAST_g2_P_OrRegister(h, BCHP_TUNER_ANACTL26, (1 << tuner_bit));
               goto start_pll_locking;
            }
            
            BAST_g2_P_ProcessScript(h, script_tuner_dac);
            hChn->tuneDacState = 1;
            return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, popcap_delay, BAST_g2_P_TunerTuneDac1);       
#endif
            
#if (((BCHP_CHIP==7340) || (BCHP_CHIP==7342)) && defined(BAST_USE_TUNER_PLL_STATE_MACHINE))
         case 2:
            BAST_g2_P_ReadRegister(h, BCHP_TUNER_DIGCTL20, &val);
            if (val & 0x40000)
               hChn->tuneDacState = 1;
            else
            {
               BDBG_ERR(("DACPLL state machine busy"));
               break;
            }            
#endif
            
         case 1:     
            BAST_g2_P_TunerGetLockStatus(h, &bDacLocked, &bMixerLocked);
            hChn->tuneDacState = 0xFF;            
            if (!bDacLocked)
            {
               if ((hChn->tunerCtl & BAST_G2_TUNER_CTL_DISABLE_RETRIES) == 0)
               {
                  BDBG_WRN(("retrying to lock tuner DAC PLL"));
                  hChn->tuneDacState = 0;
               }
               else
               {
                  BDBG_ERR(("unable to lock tuner DAC PLL"));
               }
            }
#ifdef BAST_DEBUG_TUNER_PLL_STATE_MACHINE               
            else
            {
#if (((BCHP_CHIP==7340) || (BCHP_CHIP==7342)) && defined(BAST_USE_TUNER_PLL_STATE_MACHINE))
               BAST_g2_P_ReadRegister(h, BCHP_TUNER_DIGCTL20, &val);
               val &= 0xFF;
#else
               BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL26, &val);
#endif               
               BDBG_MSG(("tuner dac locked (cap_cntl=0x%X)", val));
            }
#endif            
            break;
            
         default:
            BDBG_ERR(("BAST_g2_P_TunerTuneDac1(): invalid state %d", hChn->tuneDacState));
            return BERR_TRACE(BAST_ERR_AP_IRQ);         
      }
   }
   
   return hChn->postTuneMixFunct(h);   
}


/******************************************************************************
 BAST_g2_P_TunerMixpllLostLock()
******************************************************************************/
bool BAST_g2_P_TunerMixpllLostLock(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val = 0;
   bool bDacLocked, bMixerLocked;
   
   BAST_g2_P_TunerGetLockStatus(h, &bDacLocked, &bMixerLocked);
   if (!bMixerLocked || !bDacLocked)
      goto lost_lock;
   
   BAST_g2_P_ReadRegister(h, BCHP_SDS_IRQSTS5, &val);  
   if (val & 0x00180000)
   {
      lost_lock:
#ifdef BAST_TUNE_MIX_DEBUG      
      BDBG_MSG(("Mixpll lost lock: irqsts5=0x%X", val));
#endif      
      hChn->tuneMixStatus &= ~BAST_TUNE_MIX_LOCKED;  
       
      return true;
   } 
   return (((hChn->tuneMixStatus & BAST_TUNE_MIX_LOCKED) == 0) ? true : false);
}


/******************************************************************************
 BAST_g2_P_TunerResetMixpllLoseLockBit()
******************************************************************************/
void BAST_g2_P_TunerResetMixpllLoseLockBit(BAST_ChannelHandle h)
{
   uint32_t val;
   
   /* reset mixpll lose lock bit */
   val = 0x00180000; /* clear bits 20:19 */
   BAST_g2_P_WriteRegister(h, BCHP_SDS_IRQSTS5, &val);   
}

            
/******************************************************************************
 BAST_g2_P_TunerTuneMix()
******************************************************************************/
BERR_Code BAST_g2_P_TunerTuneMix(BAST_ChannelHandle h, BAST_g2_FUNCT nextFunct)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;

   hChn->postTuneMixFunct = nextFunct;
   hChn->fullTuneFreq = hChn->actualTunerFreq;
   
   if ((hChn->tuneMixStatus & BAST_TUNE_MIX_RESUME_RETRY) == 0)
      hChn->tuneMixState = 2;
   else if ((hChn->tuneMixStatus & BAST_TUNE_MIX_NEXT_RETRY) && ((hChn->tuneMixStatus & BAST_TUNE_MIX_LOCKED) == 0))
      hChn->tuneMixState = 4;
   else
      hChn->tuneMixState = 0;      

#ifdef BAST_TUNE_MIX_DEBUG
   BDBG_MSG(("BAST_g2_P_TunerTuneMix(): tuneMixStatus=0x%X", hChn->tuneMixStatus));
#endif

   return BAST_g2_P_TunerTuneMix0(h);
}


/******************************************************************************
 BAST_g2_P_TunerTuneMix0()
******************************************************************************/
BERR_Code BAST_g2_P_TunerTuneMix0(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
   const uint32_t tune_mix_vcref = 0x18;
   uint32_t val, val2;
#ifdef BAST_TUNE_MIX_DEBUG   
   uint32_t anactl9, anactl18, anactl19; 
#endif        
   bool bDacLocked, bMixerLocked;
   uint8_t gmx = 0;
#if (((BCHP_CHIP==7340) || (BCHP_CHIP==7342)) && defined(BAST_USE_TUNER_PLL_STATE_MACHINE))   
   const uint32_t popcap_delay = 60; /* delay in usecs */
#else
   uint32_t tuner_bit;   
#endif
   
   while (hChn->tuneMixState != 0xFF)
   {
#ifdef BAST_TUNE_MIX_DEBUG   
      /* BDBG_MSG(("BAST_g2_P_TunerTuneMix0(): state %d", hChn->tuneMixState)); */
#endif      
      switch (hChn->tuneMixState)
      {
         case 2:
            /* restore gmx */
            BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL18, &val);
            val = (val & ~0x3C) | hChn->tunerGmx;
            BAST_g2_P_WriteRegister(h, BCHP_TUNER_ANACTL18, &val);
            
            /* restore dac_ictl */
            BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL19, &val);
            val = (val & 0xF0) | hChn->tunerDacIctl;
            BAST_g2_P_WriteRegister(h, BCHP_TUNER_ANACTL19, &val);      
            hChn->tunerGmxIdx = 0xFE;   
            hChn->tuneMixState = 0;      
            break;
            
         case 0: /* retry_tune_mix */
#ifdef BAST_TUNE_MIX_DEBUG
            BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL9, &anactl9);
            BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL18, &anactl18);   
            BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL19, &anactl19);   
            BDBG_MSG(("tune_mix: anactl9=0x%X, anactl18=0x%X, anactl19=%X", anactl9, anactl18, anactl19));
#endif            

            BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL13, 0xFFFFFFE0, tune_mix_vcref);
            
#if (((BCHP_CHIP==7340) || (BCHP_CHIP==7342)) && defined(BAST_USE_TUNER_PLL_STATE_MACHINE))
            val = (((popcap_delay * 200000) / 2963) + 1) >> 1;
            val |= 0x60000;           
            BAST_g2_P_WriteRegister(h, BCHP_TUNER_DIGCTL19, &val);
            val |= 0x10000;
            BAST_g2_P_WriteRegister(h, BCHP_TUNER_DIGCTL19, &val);            
            BAST_g2_P_ReadRegister(h, BCHP_TUNER_DIGCTL22, &val);            
            val2 = (((val & 0x0007FFFF) * 3375) / 10000) + (popcap_delay * 8);
            hChn->tuneMixState = 1;
            return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, val2, BAST_g2_P_TunerTuneMix0);     
#else       
              BAST_g2_P_OrRegister(h, BCHP_TUNER_ANACTL17, 0x0C); /* Rcntl[1:0] = 11 */
            BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL16, 0xFFFFFFE0, 0x04); /* QPbiasCNT2[4:0] = 00100 */
            BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL12, 0xFFFFFFC0, 0x20); /* cap_cntl[5:0]=100000 */

            tuner_bit = 5;  

            start_pll_locking: 
            BAST_g2_P_TunerTuneMix1(h);
      
            /* read popcap */
            BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL29, &val);
            val &= 0x04;
            BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL12, &val2);
            if (val)
            {
               /* region high */
               val2 &= ~(1 << tuner_bit);
               BAST_g2_P_WriteRegister(h, BCHP_TUNER_ANACTL12, &val2);
            }
            if (tuner_bit)
            {
               tuner_bit--;
               BAST_g2_P_OrRegister(h, BCHP_TUNER_ANACTL12, (1 << tuner_bit));
               goto start_pll_locking;
            }
           
            BAST_g2_P_TunerTuneMix1(h);
            hChn->tuneMixState = 1;
#endif            
            break;
            
         case 5: 
            BAST_g2_P_TunerResetMixpllLoseLockBit(h);   
            hChn->tuneMixLockStableCount = 0;
            hChn->tuneMixState = 3;         
            break;
            
         case 1: /* check for mixpll lock */
#if (((BCHP_CHIP==7340) || (BCHP_CHIP==7342)) && defined(BAST_USE_TUNER_PLL_STATE_MACHINE))
            BAST_g2_P_ReadRegister(h, BCHP_TUNER_DIGCTL20, &val);
            if ((val & 0x00080000) == 0)
            {
               BDBG_ERR(("MIXPLL state machine busy!"));
               break;
            }
#endif
         
#ifdef BAST_DEBUG_TUNER_PLL_STATE_MACHINE            
#if (((BCHP_CHIP==7340) || (BCHP_CHIP==7342)) && defined(BAST_USE_TUNER_PLL_STATE_MACHINE))         
            BAST_g2_P_ReadRegister(h, BCHP_TUNER_DIGCTL20, &val);
            val = val >> 8;
#else
            BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL12, &val);
#endif         
            val = val & 0x3F;
            BDBG_MSG(("tuneMix cap_cntl = 0x%X", val));
#endif            
            
            hChn->tuneMixStatus &= ~BAST_TUNE_MIX_LOCKED;   
            if (hChn->tunerBandgap == 0x1C)
            {
               hChn->tuneMixState = 5;
               return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 60, BAST_g2_P_TunerTuneMix0);               
               break;
            }
            else
            {
               BAST_g2_P_TunerGetLockStatus(h, &bDacLocked, &bMixerLocked);
               if (!bDacLocked)
               {
                  BDBG_ERR(("tuner DAC PLL not locked!"));
                  hChn->tuneMixState = 0xFF;
                  break;
               }
               if (bMixerLocked)
                  hChn->tuneMixStatus |= BAST_TUNE_MIX_LOCKED;
               else
               {
                  BDBG_WRN(("tuner MIX PLL not locked!"));
               }
               hChn->tuneMixState = 4;
            }
            break;
                     
         case 3: /* check for stable mixpll lock */
            BAST_g2_P_TunerGetLockStatus(h, &bDacLocked, &bMixerLocked);
            if (bMixerLocked)
            {
               BAST_g2_P_ReadRegister(h, BCHP_SDS_IRQSTS5, &val);  
               if ((val & 0x00180000) == 0)
               {
                  hChn->tuneMixLockStableCount++;
                  /* BDBG_MSG(("mixpll stable lock check: %d", hChn->tuneMixLockStableCount)); */
                  if (hChn->tuneMixLockStableCount > 7)
                  {
                     hChn->tuneMixStatus |= BAST_TUNE_MIX_LOCKED;
#ifdef BAST_TUNE_MIX_DEBUG
                     BDBG_MSG(("mixpll stable lock"));
#endif                     
                  }
                  else
                     return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 1000, BAST_g2_P_TunerTuneMix0);
               }
            }
            hChn->tuneMixState = 4;
            break;
            
         case 4:
            if (hChn->tuneMixStatus & BAST_TUNE_MIX_LOCKED)
            {
               hChn->tuneMixStatus |= BAST_TUNE_MIX_RESUME_RETRY;
               
               /* reset mixpll lose lock bit */
               BAST_g2_P_TunerResetMixpllLoseLockBit(h);
                
               hChn->tuneMixState = 0xFF; /* exit tune_mix */
            }
            else if ((hChn->tuneMixStatus & BAST_TUNE_MIX_GIVE_UP) == 0)
            {
               BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL9, &val);  
               if ((val & 0x1C) != 0x1C)               
               {
                  hChn->tunerBandgap = 0x1C; /* increase bandgap when tune_mix fails */
                  BAST_g2_P_TunerSetBandgap(h);
               }
               else
               {                          
                  if (hChn->tuneMixStatus & BAST_TUNE_MIX_RESET_IF_PREVIOUS_SETTING_FAILS)
                  {
                     hChn->tuneMixStatus &= ~BAST_TUNE_MIX_RESET_IF_PREVIOUS_SETTING_FAILS;
                     hChn->tuneMixState = 2;
#ifdef BAST_TUNE_MIX_DEBUG                     
                     BDBG_MSG(("previous setting failed, reset gmx"));
#endif                     
                     break;
                  }
                  
                  /* set dac_ictl to 0x0F */
                  BAST_g2_P_OrRegister(h, BCHP_TUNER_ANACTL19, 0x0F);
                  
                  if (((hChn->actualTunerFreq > 283000000UL) && (hChn->actualTunerFreq <= 400000000UL)) ||
                      ((hChn->actualTunerFreq > 565000000UL) && (hChn->actualTunerFreq <= 800000000UL)) ||
                      ((hChn->actualTunerFreq > 1130000000UL) && (hChn->actualTunerFreq <= 1600000000UL)))
                  {
                     if (hChn->tunerGmxIdx & 0x80)
                        hChn->tunerGmxIdx = 0;
                     else
                        hChn->tunerGmxIdx++;
                  }
                  else
                  {
                     if (hChn->tunerGmxIdx == 0xFE)
                     {
                        gmx = (hChn->tunerGmx) >> 2; /* try default value first */
                        hChn->tunerGmxIdx = 0xFF;
                        goto write_gmx;
                     }
                 
                     hChn->tunerGmxIdx++;
                  }
                  gmx = hChn->tunerGmxIdx << 2;
                  
                  if (gmx > 0x3C)
                  {
                     hChn->tunerGmxIdx = 0x0F;
                     hChn->tuneMixStatus |= BAST_TUNE_MIX_GIVE_UP;
                     hChn->tuneMixState = 0xFF;
                     break;
                  }
                  
                  write_gmx:
                  BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL18, ~0x3C, gmx);                  
               }

               if ((hChn->tunerCtl & BAST_G2_TUNER_CTL_DISABLE_RETRIES) == 0)
                  hChn->tuneMixState = 0;
               else
                  hChn->tuneMixState = 0xFF;
            }         
            else
               hChn->tuneMixState = 0xFF;         
            break;            
            
         default:
            BDBG_ERR(("BAST_g2_P_TunerTuneMix0(): invalid state %d", hChn->tuneMixState));
            BERR_TRACE(retCode = BAST_ERR_AP_IRQ);
            hChn->tuneMixState = 0xFF;
            break;         
      }
   }
   
#ifdef BAST_TUNE_MIX_DEBUG   
   BDBG_MSG(("tune_mix done"));
#endif   
   return hChn->postTuneMixFunct(h);
}


#if (((BCHP_CHIP==7340) || (BCHP_CHIP==7342)) && defined(BAST_USE_TUNER_PLL_STATE_MACHINE))
#else
/******************************************************************************
 BAST_g2_P_TunerTuneMix1()
******************************************************************************/
void BAST_g2_P_TunerTuneMix1(BAST_ChannelHandle h)
{
   static const uint32_t script_tuner_mix[] =
   {
      BAST_SCRIPT_OR(BCHP_TUNER_ANACTL10, 0x08),  /* cmlDIVRST = 1 */
      BAST_SCRIPT_OR(BCHP_TUNER_ANACTL17, 0x02),  /* manRSTClf = 1 */
      BAST_SCRIPT_OR(BCHP_TUNER_ANACTL9, 0x40),   /* rstCMOSbb = 1 */
      BAST_SCRIPT_AND(BCHP_TUNER_ANACTL10, 0xF7), /* cmlDIVRST = 0 */
      BAST_SCRIPT_AND(BCHP_TUNER_ANACTL17, 0xFD), /* manRSTClf = 0 */
      BAST_SCRIPT_EXIT
   };

   BAST_g2_P_ProcessScript(h, script_tuner_mix);
   BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 60, NULL); 
   BAST_g2_P_AndRegister(h, BCHP_TUNER_ANACTL9, 0xFFFFFFBF); /* rstCMOSbb = 0 */
}
#endif


/******************************************************************************
 BAST_g2_P_TunerSelectVco()
******************************************************************************/
void BAST_g2_P_TunerSelectVco(BAST_ChannelHandle h)
{
   static const uint32_t tuner_range[7] =
   {
#if (BAST_G2_XTAL_FREQ==54000000)
      1700000000UL, 1140000000UL, 850000000UL, 570000000UL, 425000000UL, 285000000UL, 0  
#else
      1700000000UL, 1130000000UL, 850000000UL, 565000000UL, 425000000UL, 283000000UL, 0
#endif      
   };

   static const uint32_t tune_mix_tank[] =
   {
      0x80, 0x40, 0x80, 0x40, 0x80, 0x40, 0x80
   };

   static const uint32_t tune_mix_sel_mux[] =
   {
      2, 2, 5, 5, 4, 4, 6
   };

   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   int i;
   uint32_t tank, sel_mux;

   i = 0;
   while (1)
   {
      if (hChn->actualTunerFreq > tuner_range[i])
         break;
      i++;
   }

   tank = tune_mix_tank[i];
   sel_mux = tune_mix_sel_mux[i];

   BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL11, 0xFFFFFF3F, tank);
   BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL10, ~0x07, sel_mux);
}


/******************************************************************************
 BAST_g2_P_TunerSetNotchFilter()
******************************************************************************/
void BAST_g2_P_TunerSetNotchFilter(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;

   /* if actual_tuner_freq < 1100 MHz, enable tuner notch filter */
   if (hChn->actualTunerFreq < 1100000000UL)
      val = 0x40; /* enable notch */
   else
      val = 0x00; /* disable notch */
   BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL2, 0xFFFFFF1F, val);
}


/******************************************************************************
 BAST_g2_P_TunerGetLoDivider() - tunerLoDivider scaled 2^1
******************************************************************************/
void BAST_g2_P_TunerGetLoDivider(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;

   BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL14, &val);
   if (val & 0x01)
      hChn->tunerLoDivider = 16 * 2;
   else
      hChn->tunerLoDivider = 12 * 2;

   BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL10, &val);
   val &= 0x07;
   if (val == 5)
      hChn->tunerLoDivider = hChn->tunerLoDivider >> 1;
   else if (val == 4)
      hChn->tunerLoDivider = hChn->tunerLoDivider >> 2;
   else if (val == 6)
      hChn->tunerLoDivider = hChn->tunerLoDivider >> 3;
}


/******************************************************************************
 BAST_g2_P_TunerUpdateActualTunerFreq()
******************************************************************************/
void BAST_g2_P_TunerUpdateActualTunerFreq(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   
   hChn->actualTunerFreq = hChn->tunerFreq + hChn->tunerIfStep + hChn->tunerOffset;
   
   if (hChn->tunerCtl & BAST_G2_TUNER_CTL_ENABLE_SPLITTER_MODE)
      hChn->actualTunerFreq += hChn->splitterModeAdj;
}


/******************************************************************************
 BAST_g2_P_TunerQuickTune()
******************************************************************************/
BERR_Code BAST_g2_P_TunerQuickTune(BAST_ChannelHandle h, BAST_g2_FUNCT nextFunct)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t quick_tune_retries, diff, delay;

#ifdef BAST_TUNE_MIX_DEBUG
   BDBG_MSG(("in BAST_g2_P_TunerQuickTune()"));
#endif

   hChn->postTuneFunct = nextFunct;
   
   BAST_g2_P_TunerUpdateActualTunerFreq(h);
   quick_tune_retries = 0;
   
   hChn->tuneMixStatus |= BAST_TUNE_MIX_RESET_IF_PREVIOUS_SETTING_FAILS;
   
   if (hChn->actualTunerFreq > hChn->fullTuneFreq)
      diff = hChn->actualTunerFreq - hChn->fullTuneFreq;
   else 
      diff = hChn->fullTuneFreq - hChn->actualTunerFreq;
   
   if (hChn->splitterModeAdj)
      goto set_mb2;
   else if ((hChn->actualTunerFreq < 750000000UL) || (diff > 6000000) || (hChn->tunerBandgap == 0x1C))
   {
      /* do full tune */
      BAST_g2_P_TunerSelectVco(h);
      BAST_g2_P_TunerGetLoDivider(h); 
      
      set_mb2: 
      BAST_g2_P_TunerSetMb2(h); 
      BAST_g2_P_TunerTuneMix(h, BAST_g2_P_TunerQuickTune1);
   }
   else
      BAST_g2_P_TunerSetMb2(h);

   delay = 750;      
   return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, delay, BAST_g2_P_TunerQuickTune2);  
}


/******************************************************************************
 BAST_g2_P_TunerQuickTune1()
******************************************************************************/
BERR_Code BAST_g2_P_TunerQuickTune1(BAST_ChannelHandle h)
{
   BAST_g2_P_TunerSetRcntlQpbiascnt2(h); 
   return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 30, BAST_g2_P_TunerQuickTune2); 
}


/******************************************************************************
 BAST_g2_P_TunerQuickTune2()
******************************************************************************/
BERR_Code BAST_g2_P_TunerQuickTune2(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;   
   hChn->tunerVco = BAST_g2_P_TunerGetActualVco(h); 
   BAST_g2_P_TunerResetMixpllLoseLockBit(h);
   return hChn->postTuneFunct(h);   
}


/******************************************************************************
 BAST_g2_P_TunerComputeCutoff()
******************************************************************************/
void BAST_g2_P_TunerComputeCutoff(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t P_hi, P_lo, Q_hi, Q_lo, val, Fb;

   if (hChn->acqState == BAST_AcqState_ePeakScan)
      Fb = hChn->peakScanSymRateMax;
   else
      Fb = hChn->acqParams.symbolRate;
      
   if (Fb >= 6000000)
   {
      /* nyquist + (1 + 8% Fb) per Steve */
      if (((hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_NYQUIST_20) == 0) && (h->pDevice->settings.networkSpec != BAST_NetworkSpec_eEuro))
         val = 60;    /* 0.755*0.80 */
      else
         val = 54;  /* 0.68*0.80, to get better SNR for ACI since V70 does not get good SNR, may be temporary, ckp */
      BAST_MultU32U32(Fb, val, &P_hi, &P_lo);
      BAST_DivU64U32(P_hi, P_lo, 100000000, &Q_hi, &Q_lo);
      Q_lo++;
      hChn->tunerLpfToCalibrate = (uint8_t)(Q_lo + 1);
   }
   else if (Fb <= 1500000)
      hChn->tunerLpfToCalibrate = 0x01;
   else if (Fb <= 2500000)
      hChn->tunerLpfToCalibrate = 0x02;
   else if (Fb <= 3000000)
      hChn->tunerLpfToCalibrate = 0x03;
   else if (Fb <= 4000000)
      hChn->tunerLpfToCalibrate = 0x04;
   else
      hChn->tunerLpfToCalibrate = 0x05; /* 4MHz < Fb < 6MHz */
      
#ifndef BAST_EXCLUDE_SPLITTER_MODE
   if (hChn->tunerCtl & BAST_G2_TUNER_CTL_ENABLE_SPLITTER_MODE)
   {
      val = hChn->splitterModeAdj;
      if (hChn->splitterModeAdj < 0)
         val = -(hChn->splitterModeAdj);
      val = (val + 500000) / 1000000;    
      BDBG_MSG(("cutoff=%d + %d MHz for splitterModeAdj", hChn->tunerLpfToCalibrate, val));         
      hChn->tunerLpfToCalibrate += val;
   }
#endif
}


/******************************************************************************
 BAST_g2_P_TunerSetTestTone()
******************************************************************************/
void BAST_g2_P_TunerSetTestTone(BAST_ChannelHandle h, uint32_t hz)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t P_hi, P_lo, Q_hi, Q_lo;

   /* write the FCW to TUNER_MB3[31:16] */
   BAST_MultU32U32(hz, 524288, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, hChn->tunerFddfs, &Q_hi, &Q_lo);
   BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_MB3, 0x0000FFFF, Q_lo << 16);
}


/******************************************************************************
 BAST_g2_P_TunerGetAvePower()
******************************************************************************/
uint32_t BAST_g2_P_TunerGetAvePower(BAST_ChannelHandle h)
{
   uint32_t i, sum, val;

   sum = 0;  
   for (i = 0; i < 64; i++)
   {
      BAST_g2_P_ReadRegister(h, BCHP_SDS_AGCLI, &val);   /* read AGC power estimate */
      sum += (val >> 12);
   }
   return ((sum + 32) >> 6);  /* round and average power over 64 samples */
}


/******************************************************************************
 BAST_g2_P_TunerSetFreq()
******************************************************************************/
BERR_Code BAST_g2_P_TunerSetFreq(BAST_ChannelHandle h, BAST_g2_FUNCT nextFunct)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;

   hChn->postTuneFunct = nextFunct;
   hChn->splitterModeAdj = 0;
   hChn->tunerOffset = 0;
   BAST_g2_P_TunerUpdateActualTunerFreq(h);

   hChn->funct_state = 0;

   /* power up */
   BAST_g2_P_AndRegister(h, BCHP_TUNER_ANACTL9, ~0x20);
   BAST_g2_P_OrRegister(h, BCHP_TUNER_ANACTL8, 0x38); /* DEC_PRE_QP_I='111' */
   return BAST_g2_P_TunerSetFreq1(h);
}


/******************************************************************************
 BAST_g2_P_TunerCalculateFcw()
******************************************************************************/
uint32_t BAST_g2_P_TunerCalculateFcw(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t P_hi, P_lo, Q_hi, Q_lo;

   BAST_MultU32U32(hChn->actualTunerFreq, 16777216, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, hChn->tunerFddfs, &Q_hi, &Q_lo);
   BAST_DivU64U32(Q_hi, Q_lo, hChn->tunerLoDivider >> 1, &Q_hi, &Q_lo);
   /* BDBG_MSG(("BAST_g2_P_TunerCalculateFcw(): Fc=%u, Fddfs=%u, L=%d, fcw=%d", hChn->actualTunerFreq, hChn->tunerFddfs, hChn->tunerLoDivider >> 1, Q_lo)); */
   return Q_lo;
}


/******************************************************************************
 BAST_g2_P_TunerCalculateVco()
******************************************************************************/
uint32_t BAST_g2_P_TunerCalculateVco(BAST_ChannelHandle h, uint32_t fcw)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t P_hi, P_lo, val;

   BAST_MultU32U32(hChn->tunerFddfs, fcw, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, 16777216, &P_hi, &P_lo);
   BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL14, &val);
   if (val & 1)
      P_lo *= 16;
   else
      P_lo *= 12;
   /* BDBG_MSG(("BAST_g2_P_TunerCalculateVco(): fcw=%d, Fddfs=%u, vco=%u", fcw, hChn->tunerFddfs, P_lo)); */
   return P_lo;
}


/******************************************************************************
 BAST_g2_P_TunerGetExpectedVco()
******************************************************************************/
uint32_t BAST_g2_P_TunerGetExpectedVco(BAST_ChannelHandle h)
{
   uint32_t fcw;

   fcw = BAST_g2_P_TunerCalculateFcw(h);
   return BAST_g2_P_TunerCalculateVco(h, fcw);
}


/******************************************************************************
 BAST_g2_P_TunerGetActualVco()
******************************************************************************/
uint32_t BAST_g2_P_TunerGetActualVco(BAST_ChannelHandle h)
{
   uint32_t fcw, vco, val;

   BAST_g2_P_ReadRegister(h, BCHP_TUNER_MB2, &val);
   fcw = (val >> 8);
   vco = BAST_g2_P_TunerCalculateVco(h, fcw);
    /* BDBG_MSG(("VCO=%u", vco)); */
   return vco;
}


/******************************************************************************
 BAST_g2_P_TunerSetFreq1() - ISR context
******************************************************************************/
BERR_Code BAST_g2_P_TunerSetFreq1(BAST_ChannelHandle h)
{
   BAST_g2_P_Handle *hDev = h->pDevice->pImpl;
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t anactl4, val, P_hi, P_lo, Q_hi, Q_lo, anactl18, data0, data1, data2;
   BERR_Code retCode = BERR_SUCCESS;

   static const uint32_t script_tuner_tune_1[] =
   {
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL3, 0x80), /* disable tuner post-DCO */
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL0, 0x80), /* disable tuner pre-DCO */
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL6, 0x00), /* disable agc gain limit */
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL5, 0x00), /* disable pga hysteresis */
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL7, 0x00), /* disable LO carrier loop */
      BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL4, 0xFC), /* open up LPF bw so post-DCO's work properly */
      BAST_SCRIPT_AND_OR(BCHP_SDS_AGICTL, 0x28, 0x22),  /* freeze IF AGC integrator */
      BAST_SCRIPT_AND_OR(BCHP_SDS_AGTCTL, 0x28, 0x02),  /* freeze RF AGC integrator */
      BAST_SCRIPT_WRITE(BCHP_SDS_AIT, 0x00000000),  /* set RF AGC integrator to min gain to kill RF for LPF cal */
      BAST_SCRIPT_WRITE(BCHP_SDS_AII, 0xA0000000),  /* set IF AGC integrator to max gain to pass DDFS tone for LPF cal */
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL11, 0x01), /* assert sds_tuner digital reset */
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL11, 0x00), /* deassert sds_tuner digital reset */
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL15, 0xFF), /* set pre-DCO at high bw (charge pump) */
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL16, 0xFF), /* set post-DCO at high bw */
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL0, 0x00),  /* enable pre-DCO */
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL3, 0x00),  /* enable post-DCO */
      BAST_SCRIPT_EXIT
   };

   while (hChn->funct_state != 0xFF)
   {
      /* BDBG_MSG(("BAST_g2_P_TunerSetFreq1(): state %d", hChn->funct_state)); */
      switch (hChn->funct_state)
      {
         case 0: /* do Fddfs switching */
#if (BCHP_CHIP==7335)         
            BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL14, &val);
            data0 = (hDev->searchRange * 120) / 100;
            if (((hChn->tunerFreq - data0) <= 1701000000UL) &&
                ((hChn->tunerFreq + data0) >= 1701000000UL))
            {
               /* set LO divider to 16 */
               val |= 1;
            }
            else
            {
               /* set LO divider to 12 */
               val &= ~1;
            }
            BAST_g2_P_WriteRegister(h, BCHP_TUNER_ANACTL14, &val);
            BAST_g2_P_TunerGetLoDivider(h);            
#endif            
            
            hChn->funct_state = 1;         
            if ((hChn->acqState == BAST_AcqState_eTuning) &&
                (hChn->tunerCtl & BAST_G2_TUNER_CTL_DISABLE_FDDFS_SWITCHING) == 0)
               return BAST_g2_P_TunerSetFddfs(h, BAST_g2_P_TunerSetFreq1);  
            break;

         case 1: /* do Fs smart tune */
            if (hChn->acqState != BAST_AcqState_eTunerGainCalibration)
            {
               /* disable any notch filter that may be on from the previous acquisition */
               val = (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_OQPSK) ? 0x00000F70 : 0x00000F30;
               BAST_g2_P_WriteRegister(h, BCHP_SDS_NTCH_CTRL, &val); 
            }
            
            if ((hChn->acqState == BAST_AcqState_eTuning) &&
                ((hChn->tunerCtl & BAST_G2_TUNER_CTL_DISABLE_FS_SMART_TUNING) == 0))
            {
               /* Fs smart tuning is enabled */
               if (hChn->diseqcStatus.status == BAST_DiseqcSendStatus_eBusy)
               {
                  /* wait until diseqc completes before smart tuning (check again after 1 msec) */
                  return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 1000, BAST_g2_P_TunerSetFreq1); 
               }
               else
                  BAST_g2_P_TunerFsSmartTune(h);
            }
            hChn->funct_state = 2;         
            break;
            
         case 2:
            BAST_g2_P_ProcessScript(h, script_tuner_tune_1);
            BAST_g2_P_TunerSelectVco(h);
            BAST_g2_P_TunerGetLoDivider(h);

            hChn->splitterModeAdj = 0;
#ifndef BAST_EXCLUDE_SPLITTER_MODE
            if (hChn->tunerCtl & BAST_G2_TUNER_CTL_ENABLE_SPLITTER_MODE)
               hChn->funct_state = 3;
            else
#endif
               hChn->funct_state = 4;
            break;
            
#ifndef BAST_EXCLUDE_SPLITTER_MODE
         case 3: /* splitter mode support */
            BAST_g2_P_UpdateSplitterModeAdj(h);
            hChn->funct_state = 4;
            break;
#endif
         case 4:
            BAST_g2_P_TunerSetMb2(h);
            hChn->funct_state = 14;
            break;
            
         case 14:
            hChn->funct_state = 15;
            return BAST_g2_P_TunerTuneMix(h, BAST_g2_P_TunerSetFreq1); 
            
         case 15:
            if (hChn->tuneMixStatus & BAST_TUNE_MIX_LOCKED)
               hChn->funct_state = 12;
            else if (hChn->acqState == BAST_AcqState_eIdle)
               hChn->funct_state = 16;
            else
            {
               hChn->funct_state = 14;             
               if (hChn->tuneMixStatus & BAST_TUNE_MIX_GIVE_UP)
               {
                  hChn->acqCount++;
                  hChn->tuneMixStatus = 0;
                  return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 100000, BAST_g2_P_TunerSetFreq1);                   
               }
            }
            break;
            
         case 12:          
#if (((BCHP_CHIP==7340) || (BCHP_CHIP==7342)) && defined(BAST_USE_TUNER_PLL_STATE_MACHINE))
            /* rcntl and qpbiascnt2 already set by the mixpll state machine */ 
#ifdef BAST_DEBUG_TUNER_PLL_STATE_MACHINE            
            BAST_g2_P_ReadRegister(h, BCHP_TUNER_DIGCTL24, &val);
            BDBG_MSG(("DIGCTL24=0x%X: QPbiasCNT2=%d, rcntl=%d", val, (val >> 8) & 0x1F, val & 0x03));
#endif            
#else         
            BAST_g2_P_TunerSetRcntlQpbiascnt2(h);
#endif            
            BAST_g2_P_TunerSetNotchFilter(h);
            hChn->funct_state = 5;
            return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 20, BAST_g2_P_TunerSetFreq1);          
                       
         case 5: /* LPF calibration */
            BAST_g2_P_TunerResetMixpllLoseLockBit(h);
            hChn->tunerVco = BAST_g2_P_TunerGetActualVco(h);        
            hChn->bSetLpf = false;
            if (hChn->acqState != BAST_AcqState_eIdle)
            {
               if (hChn->bTunerAdjImageFilter)
               {                     
                  if (hChn->tunerBandgap != 0x1C)
                  {                  
                     hChn->bSetLpf = true;
                     goto do_imf_cal;
                  }
                  else
                     hChn->bTunerAdjImageFilter = false;
               }
               hChn->funct_state = 11;
            }
            else
            {
               /* IMF calibration */
               hChn->bTunerAdjImageFilter = true;    
               
               do_imf_cal:           
               hChn->tunerLpfToCalibrate = 36; /* set filter to 36MHz for IMF calibration */
               hChn->count2 = 0;
               hChn->funct_state = 9;            
            }
            break;

         case 11:
            BAST_g2_P_TunerComputeCutoff(h);       
            if (hChn->tunerCtl & BAST_G2_TUNER_CTL_SET_FILTER_MANUAL)
            {
               hChn->trackingAnactl4 = (uint32_t)((((uint32_t)hChn->tunerLpfToCalibrate * 10) + 5) / 9) << 2;
               hChn->tunerLpfToCalibrate = hChn->tunerCutoff;
            }
                                   
            /* reset the ADC data capture FIFO */
            val = 0x01;
            BAST_g2_P_WriteRegister(h, BCHP_SDS_ADCPCTL, &val);
            
            hChn->funct_state = 6;               
            BAST_g2_P_TunerSetFilter(h, BAST_g2_P_TunerSetFreq1);
            return BERR_SUCCESS;         
            
         case 9:
            /* BDBG_MSG(("IMF: do LPF cal")); */
            hChn->funct_state = 10;
            BAST_g2_P_TunerSetFilter(h, BAST_g2_P_TunerSetFreq1);
            return BERR_SUCCESS;
            
         case 10:
            if ((hChn->tuneMixStatus & BAST_TUNE_MIX_LOCKED) == 0)
            {
               hChn->tuneMixStatus |= BAST_TUNE_MIX_NEXT_RETRY;                
               hChn->funct_state = 14; /* retry tune_mix */
               break;
            }
            
            hChn->funct_state = 9;
            hChn->count2++;
            if (hChn->bTunerCalFailed)
            {
               BDBG_MSG(("IMF calibration failed"));            
               if (hChn->count2 > 4)
                  hChn->funct_state = 7;
               break;
            }
              
            BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL4, &anactl4);
            /* BDBG_MSG(("IMF: anactl4=0x%X", anactl4)); */
            if (hChn->count2 == 1)    
            {            
               hChn->trackingAnactl4 = anactl4;
               break;
            }
            else if (hChn->trackingAnactl4 > anactl4)
               data0 = hChn->trackingAnactl4 - anactl4;
            else
               data0 = anactl4 - hChn->trackingAnactl4;
            hChn->trackingAnactl4 = anactl4;
            data0 = data0 >> 2;
            if (data0 <= 1)
            {
               data0 = anactl4 >> 2;
               if (data0 < 44)
                  data1 = 44 - data0;
               else
                  data1 = data0 - 44;
               data1 = data1 >> 1;
               if (data1 > 7)
                  data1 = 7;
               if (data0 < 44)
                  data2 = 8 - data1;
               else
                  data2 = 8 + data1;
               data2 = data2 << 2;
               BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL18, &anactl18);
               anactl18 &= 0xC3;
               anactl18 |= data2;
               BAST_g2_P_WriteRegister(h, BCHP_TUNER_ANACTL18, &anactl18);
               hChn->tunerGmx = anactl18 & 0x3C;
               hChn->bTunerAdjImageFilter = false;
               /* BDBG_MSG(("IMF: data0=%d, data1=%d, data2=%d, anactl18=0x%X", data0, data1, data2, anactl18)); */
               hChn->funct_state = 7;
            }
            else if (hChn->count2 > 4)
            {
               BDBG_MSG(("IMF calibration failed"));
               hChn->funct_state = 7;
            }
            break;

         case 6:
            if ((hChn->tuneMixStatus & BAST_TUNE_MIX_LOCKED) == 0)
            {          
               hChn->tuneMixStatus |= BAST_TUNE_MIX_NEXT_RETRY; 
               hChn->funct_state = 14; /* retry tune_mix */
               break;
            }
         
            BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL4, &anactl4);
            if (hChn->tunerCtl & BAST_G2_TUNER_CTL_SET_FILTER_MANUAL)
            {
               if ((hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_TUNER_TEST_MODE) == 0)
               {
                  /* put in nominal estimate for LPF bandwidth */
                  val = hChn->trackingAnactl4;
                  BAST_g2_P_WriteRegister(h, BCHP_TUNER_ANACTL4, &val);
               }
            }

            hChn->trackingAnactl4 = anactl4;

            if ((hChn->tunerCtl & BAST_G2_TUNER_CTL_SET_FILTER_MANUAL) == 0)
            {
               /* set LPF wide setting to 10% of nyquist */
               if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_NYQUIST_20)
                  val = 20;
               else
                  val = 35;
               BAST_MultU32U32(val * 2, hChn->acqParams.symbolRate, &P_hi, &P_lo);
               BAST_DivU64U32(P_hi, P_lo, 1000000000UL, &Q_hi, &Q_lo);
               hChn->tunerCutoffWide = (unsigned char)((Q_lo + 1) >> 1);
               
               if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_OQPSK)
                  hChn->tunerCutoffWide += (hDev->searchRange / 1000000);
                  
               anactl4 = hChn->trackingAnactl4 + (hChn->tunerCutoffWide << 2);              
               BAST_g2_P_WriteRegister(h, BCHP_TUNER_ANACTL4, &anactl4);
            }
            else
               hChn->tunerCutoffWide = 0;
            hChn->funct_state = 7;
            break;

         case 7:
            if (hChn->bSetLpf)
            {
               hChn->bSetLpf = false;            
               hChn->funct_state = 11;
            }
            else
               hChn->funct_state = 16;
            break;
            
         case 16: /* label_0 */
            if (hChn->acqState != BAST_AcqState_eIdle)
               BAST_g2_P_SetSymbolRate(h);

#ifdef BAST_DISABLE_TUNER_CAL
            val = 0;
            BAST_g2_P_WriteRegister(h, BCHP_TUNER_DIGCTL0, &val);
            BAST_g2_P_WriteRegister(h, BCHP_TUNER_DIGCTL3, &val);
            val = 0x0D;
            BAST_g2_P_WriteRegister(h, BCHP_TUNER_ANACTL20, &val);
            hChn->funct_state = 8;
            return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 150, BAST_g2_P_TunerSetFreq1); 
#else
            hChn->funct_state = 0xFF;
            break;
#endif

#ifdef BAST_DISABLE_TUNER_CAL
         case 8:
            val = 0x01;
            BAST_g2_P_WriteRegister(h, BCHP_TUNER_ANACTL20, &val);
            hChn->funct_state = 0xFF;
            return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 3000, BAST_g2_P_TunerSetFreq1); 
            break;
#endif            

         default:
            BDBG_ERR(("invalid state in BAST_g2_P_TunerSetFreq1()"));
            BERR_TRACE(retCode = BAST_ERR_AP_IRQ);      
            return retCode;
      }
   }
   
#if ((BCHP_CHIP==7325) || (BCHP_CHIP==7335)) && (BCHP_VER < BCHP_VER_B0)
   BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL8, 0xC7 ,0x18); /* DEC_PRE_QP_I='011' */
#else
   BAST_g2_P_ReadModifyWriteRegister(h, BCHP_TUNER_ANACTL8, 0xC7 ,0x08); /* DEC_PRE_QP_I='001' */
#endif
   retCode = hChn->postTuneFunct(h);
   return retCode;
}


/******************************************************************************
 BAST_g2_P_TunerSetFilter()
******************************************************************************/
void BAST_g2_P_TunerSetFilter(BAST_ChannelHandle h, BAST_g2_FUNCT next_funct)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;

#ifndef BAST_DISABLE_TUNER_CAL
   static const uint32_t script_tuner_filter_1[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_ADCPCTL, 0x01), /* reset ADC data capture FIFO */
      BAST_SCRIPT_OR(BCHP_TUNER_ANACTL2, 0x01),  /* set bit 0 =1 to kill LO */
      BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL7, 0x80),  /* enable filter cal mode, DAC enabled, offset binary */
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL14, 0x00), /* Power up LPF cal DDFS/DAC clocks */
      BAST_SCRIPT_WRITE(BCHP_TUNER_DIGCTL17, 0x10), /* DDFS offset binary, shift 2; 0x10 is larger than 0x30 */
      BAST_SCRIPT_WRITE(BCHP_TUNER_ANACTL20, 0x0D),  /* reset DCO charge pumps */
      BAST_SCRIPT_EXIT
   };
#endif

   val = 0x01;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_ADCPCTL, &val); /* reset the ADC data capture FIFO */
               
#ifndef BAST_DISABLE_TUNER_CAL
   /* BDBG_MSG(("LPF cal, cutoff=%d", hChn->tunerLpfToCalibrate)); */
   hChn->passFunct = next_funct;
   BAST_g2_P_ProcessScript(h, script_tuner_filter_1);
   hChn->count1 = 0;
   BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 150, BAST_g2_P_TunerSetFilter1);
#else
   /* for now, disable tuner lpf cal */
   val = hChn->tunerLpfToCalibrate << 2;
   BAST_g2_P_WriteRegister(h, BCHP_TUNER_ANACTL4, &val);
   next_funct(h);
#endif
}


/******************************************************************************
 BAST_g2_P_TunerSetFilter1()
******************************************************************************/
BERR_Code BAST_g2_P_TunerSetFilter1(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val, p, p_min_diff, i, norm_i, norm_q, diff_i, diff_q;
   uint16_t *pI, *pQ;
   int8_t bw, bw_select, dir;
   uint8_t diff, nominal_bw;

   while (hChn->count1 != 0xFF)
   {
      if (BAST_g2_P_TunerMixpllLostLock(h))
         break;
         
      switch (hChn->count1)
      {
         case 0:
            val = 0xFF; /* set tuner pre-DCO bw */
            BAST_g2_P_WriteRegister(h, BCHP_TUNER_DIGCTL15, &val);
            val = 0x01; /* un-reset DCO charge pumps */
            BAST_g2_P_WriteRegister(h, BCHP_TUNER_ANACTL20, &val);            

            hChn->mb0_i = 0;
            hChn->mb0_q = 0;
            hChn->mb1_i = 0;
            hChn->mb1_q = 0;
            hChn->count3 = 0;    /* count3=DCO stable time */
            hChn->maxCount1 = 0; /* maxCount1 = total DCO converge time */
            hChn->count1 = 10;
            break;

         case 10: /* wait for dco to converge */
            for (i = 0; i < 2; i++)
            {
               if (i == 0)
               {
                  BAST_g2_P_ReadRegister(h, BCHP_TUNER_MB0, &val);
                  pI = &(hChn->mb0_i);
                  pQ = &(hChn->mb0_q);
               }
               else
               {
                  BAST_g2_P_ReadRegister(h, BCHP_TUNER_MB1, &val);
                  pI = &(hChn->mb1_i);
                  pQ = &(hChn->mb1_q);                  
               }
               
               norm_i = ((val & 0x0000FFFF) - 0x8000) & 0x0000FFFF;
               norm_q = (((val >> 16) & 0x0000FFFF) - 0x8000) & 0x0000FFFF;
               
               /* check DCO I/Q range within [-5000,2000] */
               if ((norm_i < 0x6C78) || (norm_i > 0x87D0) || 
                   (norm_q < 0x6C78) || (norm_q > 0x87D0))
               {
                  /* I/Q out of range */
                  hChn->mb0_i = 0;
                  hChn->mb0_q = 0;
                  hChn->mb1_i = 0;
                  hChn->mb1_q = 0;
                  hChn->count3 = 0;
                  break;
               }
               
               /* check if I/Q falls outside current window */
               if (norm_i > *pI)
                  diff_i = norm_i - *pI;
               else
                  diff_i = *pI - norm_i;
               if (norm_q > *pQ)
                  diff_q = norm_q - *pQ;
               else
                  diff_q = *pQ - norm_q;
               if ((diff_i > 255) || (diff_q > 255))
               {
                  *pI = norm_i;
                  *pQ = norm_q;
                  hChn->count3 = 0;
               }  
               /* BDBG_MSG(("DCO converge: MB%d=%08X, norm=%04X%04X, diff=%04X%04X, save=%04X%04X", i, val, norm_q, norm_i, diff_q, diff_i, *pQ, *pI)); */
            }
            
            hChn->count3++;
            if (hChn->count3 <= 4)
            {
               hChn->maxCount1++;
               if (hChn->maxCount1 < 50)
               {
                  /* delay 1 msec */
                  return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 1000, BAST_g2_P_TunerSetFilter1);                  
               }
            }
            
            /* BDBG_MSG(("DCO converge: %d msecs", hChn->maxCount1)); */
            hChn->count1 = 1;
            break;
            
         case 1:
            if (hChn->tunerLpfToCalibrate >= 63)
            {
               bw_select = 63;
               goto write_anactl4;
            }
            
            if ((hChn->tunerCtl & BAST_G2_TUNER_CTL_SET_FILTER_MANUAL) ||
                (hChn->acqState == BAST_AcqState_ePeakScan) ||
                (hChn->acqParams.symbolRate < 6000000))
            {
               bw_select = hChn->tunerLpfToCalibrate;
               goto write_anactl4;
            }
            
            BAST_g2_P_TunerSetTestTone(h, ((hChn->tunerLpfToCalibrate * 3) >> 2) * 1000000);
            val = 0xFC; /* set filter bw to max */
            BAST_g2_P_WriteRegister(h, BCHP_TUNER_ANACTL4, &val);
            hChn->count1 = 2;
            break;

         case 2:
            p = BAST_g2_P_TunerGetAvePower(h);
            hChn->p_6db = p >> 2;
            
            BAST_g2_P_TunerSetTestTone(h, hChn->tunerLpfToCalibrate * 1000000);
            val = hChn->tunerLpfToCalibrate << 2;
            BAST_g2_P_WriteRegister(h, BCHP_TUNER_ANACTL4, &val);
            hChn->count1 = 3;
            break;

         case 3: 
            p = BAST_g2_P_TunerGetAvePower(h);
            if (p < hChn->p_6db)
            {
               dir = 1;
               p_min_diff = hChn->p_6db - p;
            }
            else
            {
               dir = -1;
               p_min_diff = p - hChn->p_6db;
            }
            bw = bw_select = hChn->tunerLpfToCalibrate;

            while (1)
            {
               /* set tuner bw */
               bw += dir;
               if ((bw < 1) || (bw > 0x3F))
                  break;
               val = bw << 2;
               BAST_g2_P_WriteRegister(h, BCHP_TUNER_ANACTL4, &val);

               /* get power */
               p = BAST_g2_P_TunerGetAvePower(h);
               
               if (p < p_min_diff)
               {
                  p_min_diff = p;
                  bw_select = bw;
               }
               if (dir > 0)
               {
                  if (p > hChn->p_6db)
                     break;
               }
               else if (p < hChn->p_6db)
                  break;
            }

            /* add failsafe */
            nominal_bw = (uint8_t)(((uint32_t)hChn->tunerLpfToCalibrate * 10) / 9);
            if ((uint8_t)bw_select > nominal_bw)
               diff = bw_select - nominal_bw;
            else
               diff = nominal_bw - bw_select;
            if (diff > 5)
            {
               bw_select = nominal_bw;
               hChn->bTunerCalFailed = true;
               BDBG_MSG(("detected bad tuner LPF cal, use nominal bw instead"));
            }    
            else
               hChn->bTunerCalFailed = false;          

            write_anactl4:
            val = bw_select << 2;
            BAST_g2_P_WriteRegister(h, BCHP_TUNER_ANACTL4, &val);
            /* BDBG_MSG(("select bw %d, anactl4=%X", bw_select, val)); */

            hChn->count1 = 4;
            break;

         case 4:
            val = 0x08;
            BAST_g2_P_WriteRegister(h, BCHP_TUNER_ANACTL7, &val);
            BAST_g2_P_AndRegister(h, BCHP_TUNER_ANACTL2, 0xFFFFFFFE); /* clear bit 0 to re-enable LO */
            val = 0x18;
            BAST_g2_P_WriteRegister(h, BCHP_TUNER_DIGCTL14, &val);
            hChn->count1 = 0xFF;
            break;

         default:
            BDBG_ASSERT(0); /* should never get here */
      }
   }

   return hChn->passFunct(h);
}


/******************************************************************************
 BAST_g2_P_TunerSetDigctl7()
******************************************************************************/
void BAST_g2_P_TunerSetDigctl7(BAST_ChannelHandle h, uint32_t or_mask)
{
   uint32_t anactl10, val;

   BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL10, &anactl10);
   anactl10 &= 0x07;
   if (anactl10 == 5)
      val = 0x30; /* divide by 2 */
   else if ((anactl10 == 4) || (anactl10 == 6))
      val = 0x40;
   else
      val = 0x20; /* divide by 1 */
      
   val |= or_mask;
   BAST_g2_P_WriteRegister(h, BCHP_TUNER_DIGCTL7, &val); 
}


#ifndef BAST_EXCLUDE_SPLITTER_MODE
/******************************************************************************
 BAST_g2_P_UpdateSplitterModeAdj()
******************************************************************************/
void BAST_g2_P_UpdateSplitterModeAdj(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   BAST_g2_P_Handle *hDev = (BAST_g2_P_Handle *)(h->pDevice->pImpl);
   BAST_g2_P_ChannelHandle *hOtherChn;
   uint32_t myVco, otherVco, vcoDiff, absDiff;
#if 0   
   int32_t diff;
#endif   
   
   hChn->splitterModeAdj = 0;
   BAST_g2_P_TunerUpdateActualTunerFreq(h);   
   
 #if 0    /* Fddfs avoidance is eliminated per Steve's test and recommendation, ckp 070708 */
   /* Fddfs avoidance */
   myVco = BAST_g2_P_TunerGetExpectedVco(h);            
   diff = hChn->tunerFddfs - myVco;
   BDBG_MSG(("SplitterMode: Fddfs=%u,vco=%u", hChn->tunerFddfs, myVco));
   if (diff < 0)
      vcoDiff = -diff;
   else
      vcoDiff = diff;
   if (vcoDiff < hDev->vcoSeparation)
   {
      /* move VCO away from Fddfs */
      hChn->splitterModeAdj = hDev->vcoSeparation - vcoDiff;
      if (diff > 0)
         hChn->splitterModeAdj = -(hChn->splitterModeAdj);

      BDBG_MSG(("SplitterMode: move VCO away from Fddfs, adj=%d", hChn->splitterModeAdj));
      BAST_g2_P_TunerUpdateActualTunerFreq(h);   
      BAST_g2_P_TunerSelectVco(h);
      BAST_g2_P_TunerGetLoDivider(h);
   }
 #endif
   /* VCO avoidance */
   hOtherChn = (BAST_g2_P_ChannelHandle *)(h->pDevice->pChannels[h->channel ^ 1]->pImpl);
   otherVco = hOtherChn->tunerVco;
   myVco = BAST_g2_P_TunerGetExpectedVco(h);
   if (myVco > otherVco)
      vcoDiff = myVco - otherVco;
   else
      vcoDiff = otherVco - myVco;
BDBG_MSG(("SplitterMode: myVco=%u, otherVco=%u, vcoDiff=%u", myVco, otherVco, vcoDiff)); 
   if (vcoDiff < hDev->vcoSeparation)               
   {
      /* the 2 VCOs are too close to each other */             
      /* move this tuner's vco away from the the other vco */
      if (hChn->splitterModeAdj)
      {               
         if (hChn->splitterModeAdj > 0)
         {
            if ((uint32_t)(hChn->splitterModeAdj) > (uint32_t)(hDev->vcoSeparation >> 1))
               hChn->splitterModeAdj = hChn->tunerFddfs - hDev->vcoSeparation - hChn->tunerFreq;
            else
               hChn->splitterModeAdj = otherVco + hDev->vcoSeparation - hChn->tunerFreq;
         }
         else
         {
            absDiff = -(hChn->splitterModeAdj);
            if (absDiff > (hDev->vcoSeparation >> 1))
               hChn->splitterModeAdj = hChn->tunerFddfs + hDev->vcoSeparation - hChn->tunerFreq;
            else
               hChn->splitterModeAdj = otherVco - hDev->vcoSeparation - hChn->tunerFreq;
         }
      }
      else
      {
         hChn->splitterModeAdj = otherVco - myVco;
         if (hChn->splitterModeAdj < 0)
            hChn->splitterModeAdj += hDev->vcoSeparation;
         else
            hChn->splitterModeAdj -= hDev->vcoSeparation;                             
      }  
BDBG_MSG(("SplitterMode: move VCO away from other VCO: adj=%d", hChn->splitterModeAdj));    
   }
   
   if (hChn->splitterModeAdj)
   {           
      BAST_g2_P_TunerUpdateActualTunerFreq(h);  
BDBG_MSG(("final splitterModeAdj=%d, if_step=%d, actual_tuner_freq=%u", hChn->splitterModeAdj, hChn->tunerIfStep, hChn->actualTunerFreq));          
      BAST_g2_P_TunerSelectVco(h);
      BAST_g2_P_TunerGetLoDivider(h);               
   }   
}
#endif


/******************************************************************************
 BAST_g2_P_TunerSetBandgap()
******************************************************************************/
BERR_Code BAST_g2_P_TunerSetBandgap(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;
    
   BAST_g2_P_ReadRegister(h, BCHP_TUNER_ANACTL9, &val);
   val &= ~0x1C;
   val |= (uint32_t)(hChn->tunerBandgap);
   BAST_g2_P_WriteRegister(h, BCHP_TUNER_ANACTL9, &val);
   
   return BERR_SUCCESS;   
}


/******************************************************************************
 BAST_g2_P_ComputeActualTunerFreq()
******************************************************************************/
void BAST_g2_P_ComputeActualTunerFreq(BAST_ChannelHandle h, uint32_t fcw, uint32_t *pFreq)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t P_hi, P_lo, Q_hi, Q_lo;
    
   BAST_MultU32U32(hChn->tunerFddfs, fcw * hChn->tunerLoDivider, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, 16777216 << 1, &Q_hi, &Q_lo);
   *pFreq = Q_lo;
}


/******************************************************************************
 BAST_g2_P_TunerRampMb2()
******************************************************************************/
void BAST_g2_P_TunerRampMb2(BAST_ChannelHandle h, bool bPos, uint32_t len)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t mb2_step_freq, freq, mb2, curr_mb2, end_freq, Q_hi, Q_lo, P_hi, P_lo, end_mb2, val, afectl1;
   int32_t fli, fli_step = 0, frf, frf_step = 0, div;  
   bool bSpinv = false, bHpEnabled = false;   
     
   BAST_g2_P_ReadRegister(h, BCHP_TUNER_MB2, &mb2);        
   BAST_g2_P_ComputeActualTunerFreq(h, mb2 >> 8, &freq);
   BAST_g2_P_ComputeActualTunerFreq(h, 1, &mb2_step_freq);   
   
   BAST_g2_P_ReadRegister(h, BCHP_SDS_HPCTRL1, &val);
   if (val & 0x02) 
   {
      /* HP is enabled */  
      bHpEnabled = true;
       
      /* for turbo or LDPC modes only, compensate MB2 offset with FRF */   
      BAST_g2_P_ReadRegister(h, BCHP_SDS_STAT, &val);
      if (val & 0x00020000)
         bSpinv = true;
         
      BAST_g2_P_ComputeActualTunerFreq(h, 1, &mb2_step_freq);   
      BAST_MultU32U32(mb2_step_freq, 16777216, &P_hi, &P_lo);
      BAST_DivU64U32(P_hi, P_lo, hChn->sampleFreq, &Q_hi, &Q_lo);
      
      if (bPos)
      {
         end_freq = freq + len;
         frf_step = (int32_t)Q_lo;
      }
      else
      {
         end_freq = freq - len;
         frf_step = (int32_t)-Q_lo;
      }
      if (bSpinv)
         frf_step = -frf_step;
   }
   else  
   {
      /* legacy mode, compensate MB2 offset with FLI */   
      BAST_g2_P_ReadRegister(h, BCHP_SDS_AFECTL1, &afectl1);
      if (afectl1 & 0x02)
        div = 16;
      else
        div = 8;

      BAST_MultU32U32(mb2_step_freq * div, 1073741824, &P_hi, &P_lo);
      BAST_DivU64U32(P_hi, P_lo, hChn->sampleFreq, &Q_hi, &Q_lo);

      if (bPos)
      {
         end_freq = freq + len;
         fli_step = Q_lo;
      }
      else
      {
         end_freq = freq - len;
         fli_step = -Q_lo;
      }
   }

   /* determine what the final MB2 will be */
   BAST_MultU32U32(end_freq, 1073741824, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, (hChn->tunerFddfs >> 4), &Q_hi, &Q_lo);
   BAST_DivU64U32(Q_hi, Q_lo, hChn->tunerLoDivider, &Q_hi, &Q_lo);
   Q_lo = (Q_lo + 1) >> 1; 
   end_mb2 = Q_lo & 0xFFFFFF00;     
      
   curr_mb2 = mb2;
/* BDBG_MSG(("freq=%u, end_freq=%u, mb2_step_freq=%u, mb2=0x%X, end_mb2=0x%X, len=%d, Fddfs=%u", freq, end_freq, mb2_step_freq, mb2, end_mb2, len, hChn->tunerFddfs)); */
   while (1)
   {
      /* increment/decrement 1 LSBit of MB2 */
      if (end_mb2 > curr_mb2)
         curr_mb2 += 0x00000100;
      else if (end_mb2 < curr_mb2)
         curr_mb2 -= 0x00000100;
      else
         break;
      BAST_g2_P_WriteRegister(h, BCHP_TUNER_MB2, &curr_mb2);
      
      if (bHpEnabled) /* turbo and LDPC mode */
      {
         BAST_g2_P_ReadRegister(h, BCHP_SDS_FRF, &val);  
         frf = (int32_t)val + frf_step*256;    
         BAST_g2_P_WriteRegister(h, BCHP_SDS_FRF, (uint32_t*)&frf);
      }
      else /* legacy mode */
      {
         BAST_g2_P_ReadRegister(h, BCHP_SDS_FLI, &val);  
         fli = (int32_t)val + fli_step;    
         BAST_g2_P_WriteRegister(h, BCHP_SDS_FLI, (uint32_t*)&fli);
      }
      
      BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 5, NULL);
   }
}


