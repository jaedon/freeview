/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_g3_priv_diseqc.c $
 * $brcm_Revision: Hydra_Software_Devel/46 $
 * $brcm_Date: 12/13/12 4:33p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g3/common/bast_g3_priv_diseqc.c $
 * 
 * Hydra_Software_Devel/46   12/13/12 4:33p enavarro
 * SWSATFE-174: disable diseqc rx in PowerDown function
 * 
 * Hydra_Software_Devel/45   10/22/12 4:32p enavarro
 * SWSATFE-174: added BAST_EXCLUDE_ACW build option
 * 
 * Hydra_Software_Devel/44   7/5/12 4:09p enavarro
 * SWSATFE-174: changes for 4538
 * 
 * Hydra_Software_Devel/43   4/26/12 10:50a ronchan
 * SWSATFE-75: set fcw for ddfs tone gen for 1.5ms bit timing
 * 
 * Hydra_Software_Devel/42   4/20/12 2:51p ronchan
 * SWSATFE-75: implement extended diseqc send
 * 
 * Hydra_Software_Devel/41   3/21/12 6:43p ronchan
 * SWSATFE-183: reset adc during initialization
 * 
 * Hydra_Software_Devel/40   3/15/12 2:29p ronchan
 * SWSATFE-180: route selvtop signal to dsec_vctl pin in echo mode
 * 
 * Hydra_Software_Devel/39   3/14/12 4:33p ronchan
 * SWSATFE-180: check for acw_done interrupt on 4517
 * 
 * Hydra_Software_Devel/38   3/13/12 11:14a ronchan
 * SWSATFE-180: toggle ACWDTX for acw transmit
 * 
 * Hydra_Software_Devel/37   3/12/12 3:29p ronchan
 * SWSATFE-180: add support for acw
 * 
 * Hydra_Software_Devel/36   3/2/12 3:14p ronchan
 * SWSATFE-104: bypass initialization of tone alignment mode
 * 
 * Hydra_Software_Devel/35   1/16/12 3:22p ronchan
 * SWSATFE-75: compile out diseqc analog mode
 * 
 * Hydra_Software_Devel/34   1/13/12 2:29p ronchan
 * SWSATFE-75: format vsense debug message
 * 
 * Hydra_Software_Devel/33   11/11/11 10:58a ronchan
 * SWSATFE-156: fixed pretx delay for tone on case
 * 
 * Hydra_Software_Devel/32   11/4/11 10:46a ronchan
 * SWSATFE-75: selecting analog diseqc path essentially powers down
 * digital ftm
 * 
 * Hydra_Software_Devel/31   10/28/11 2:02p ronchan
 * SW7346-524: fixed coverity defect for unchecked return values
 * 
 * Hydra_Software_Devel/30   10/27/11 2:19p ronchan
 * SWSATFE-75: add support for analog diseqc mode
 * 
 * Hydra_Software_Devel/29   10/12/11 6:50p ronchan
 * SWSATFE-75: fixed direct amplitude comparison tone detect
 * 
 * Hydra_Software_Devel/28   10/12/11 5:23p ronchan
 * SWSATFE-75: use direct amplitude comparison method for tone detect
 * 
 * Hydra_Software_Devel/27   10/11/11 2:24p ronchan
 * SWSATFE-75: update legacy tone detect
 * 
 * Hydra_Software_Devel/26   8/1/11 4:21p mathew
 * SWSATFE-86: Removed unwanted comments
 * 
 * Hydra_Software_Devel/25   7/29/11 3:25p guangcai
 * SWSATFE-97: make latest changes apply only to 4528
 * 
 * Hydra_Software_Devel/24   7/29/11 2:28p mathew
 * SWSATFE-86: Diseqc reset update for vsense functionality.Different from
 * 7358.
 * 
 * Hydra_Software_Devel/23   7/6/11 6:35p mathew
 * SWSATFE-75: Implemented vsense adc voltage read initialisation
 * 
 * Hydra_Software_Devel/23   7/6/11 5:53p mathew
 * SWSATFE-75: Implementation update for vsense detection
 * 
 * Hydra_Software_Devel/23   7/6/11 5:20p mathew
 * SWSATFE-75: Diseq adc voltage sense update
 * 
 * Hydra_Software_Devel/22   6/27/11 9:49a ronchan
 * SWSATFE-75: optimized settings for diseqc receive
 * 
 * Hydra_Software_Devel/21   6/20/11 5:28p ronchan
 * SWSATFE-75: fixed adc setting for tone detect, fixed bug in reply
 * expectation
 * 
 * Hydra_Software_Devel/20   5/11/11 1:04p ronchan
 * SWSATFE-75: update to recommended fskphy settings for diseqc
 * 
 * Hydra_Software_Devel/19   5/10/11 2:58p ronchan
 * SWSATFE-75: set adc gain to maximum to receive diseqc
 * 
 * Hydra_Software_Devel/18   5/5/11 5:49p ronchan
 * SWSATFE-75: adc switching for diseqc rx and vsense
 * 
 * Hydra_Software_Devel/17   3/14/11 5:19p ronchan
 * SWSATFE-75: power up phy for vsense
 * 
 * Hydra_Software_Devel/16   3/10/11 3:43p ronchan
 * SWSATFE-75: implement power management for fskphy
 * 
 * Hydra_Software_Devel/15   3/4/11 3:47p enavarro
 * SWSATFE-86: backout previous change for BCM4528
 * 
 * Hydra_Software_Devel/14   3/4/11 8:13a enavarro
 * SWSATFE-75: add support for BCM4528
 * 
 * Hydra_Software_Devel/13   3/3/11 10:52a enavarro
 * SWSATFE-75: put all diseqc state variables in BAST_g3_P_DiseqcDevice
 * struct and allocate this only if the channel has diseqc capability
 * 
 * Hydra_Software_Devel/12   2/25/11 1:03p ronchan
 * SWSATFE-75: implemented diseqc power management
 * 
 * Hydra_Software_Devel/11   1/27/11 2:14p ronchan
 * SWSATFE-75: release DSTCLR before loading DiSEqC fifo
 * 
 * Hydra_Software_Devel/10   10/21/10 10:59a ronchan
 * SWSATFE-75: clear start txn bit after send diseqc command
 * 
 * Hydra_Software_Devel/9   10/21/10 10:47a ronchan
 * SWSATFE-75: use dsecSettings for vsense thresholds
 * 
 * Hydra_Software_Devel/8   10/20/10 6:00p ronchan
 * SWSATFE-75: release all diseqc resets after init
 * 
 * Hydra_Software_Devel/7   10/14/10 5:47p ronchan
 * SWSATFE-75: moved diseqc config parameters to diseqcSettings
 * 
 * Hydra_Software_Devel/6   9/22/10 5:18p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/5   9/21/10 10:36a enavarro
 * SWSATFE-75: fixed compiler warnings
 * 
 * Hydra_Software_Devel/4   9/17/10 5:23p ronchan
 * SWSATFE-75: updated to new dsec registers
 * 
 * Hydra_Software_Devel/3   9/15/10 10:28a enavarro
 * SWSATFE-75: added DSEC_ prefix to register names
 * 
 * Hydra_Software_Devel/2   9/14/10 2:29p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/1   8/31/10 1:56p enavarro
 * SWSATFE-75: initial version
 *
 ***************************************************************************/
#include "bstd.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_g3_priv.h"
#if (BCHP_CHIP==4528) || (BCHP_CHIP==4538)
#include "bchp_ftm_phy_ana.h"
#endif

BDBG_MODULE(bast_g3_priv_diseqc);


/* local routines */
BERR_Code BAST_g3_P_CalibrateDcOffset(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_CalibrateDcOffset1(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_ResetVsense(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_SendDiseqcCommand1(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_Diseqc_UpdateSettings(BAST_ChannelHandle h);


/******************************************************************************
 BAST_g3_P_DiseqcPowerUp() - power up the diseqc core
******************************************************************************/ 
BERR_Code BAST_g3_P_DiseqcPowerUp(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;

   if (!hChn->bHasDiseqc)
      return BERR_NOT_SUPPORTED;
   
   BAST_g3_P_AndRegister(h, BCHP_SDS_DSEC_DS_COMMON_CONTROL, ~0x2);  /* power up adc */
   BAST_g3_P_PowerUpFskphy(h);   /* power up fskphy */
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_DiseqcPowerDown() - power down the diseqc core
******************************************************************************/ 
BERR_Code BAST_g3_P_DiseqcPowerDown(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BAST_g3_P_Handle *hDev = h->pDevice->pImpl;

   if (!hChn->bHasDiseqc)
      return BERR_NOT_SUPPORTED;
   
   /* power down adc */
   BAST_g3_P_OrRegister(h, BCHP_SDS_DSEC_DS_COMMON_CONTROL, 0x2);
   
   /* power off diseqc rx */
   BAST_g3_P_OrRegister(h, BCHP_SDS_DSEC_DSCTL02, 0x00800000);
   
   /* power down fskphy if ftm off */
   if (hDev->bFtmPoweredDown)
      BAST_g3_P_PowerDownFskphy(h);
   
   return BERR_SUCCESS;
}


/* #define BAST_DISEQC_ANALOG_MODE */
/******************************************************************************
 BAST_g3_P_ResetDiseqc() - Non-ISR context
******************************************************************************/ 
BERR_Code BAST_g3_P_ResetDiseqc(
   BAST_ChannelHandle h,    /* [in] BAST channel handle */
   uint8_t options          /* [in] reset options */
)
{
#ifdef BAST_DISEQC_ANALOG_MODE
   BAST_g3_P_Handle *hDev = (BAST_g3_P_Handle *)(h->pDevice->pImpl);
#endif
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)(h->pImpl);
   BERR_Code retCode = BERR_SUCCESS;

   static const uint32_t script_diseqc_init[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DS_MISC_CONTROL, 0x260C7F80), /* ADC and DAC format is offset binary */
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DS_SAR_CONTROL, 0x00500020),  /* offset binary format for SAR ADC */
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DS_COMMON_CONTROL, 0x00000000), /*Initialize DS_COMMON_CONTROL[6]  (for 4528/4550 only */
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSRST, 0x00000001), /* flush unknown caused by DS_COMMON_CONTROL[6]  (for 4528/4550 only) */
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSRST, 0x00000000), /* flush unknown caused by DS_COMMON_CONTROL[6]  (for 4528/4550 only) */
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DS_COMMON_CONTROL, 0x00000003), /* Reset FSKPHY  */
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DS_COMMON_CONTROL, 0x00000000), /* Reset FSKPHY  */
#if (BCHP_CHIP==4528) || (BCHP_CHIP==4538)
      BAST_SCRIPT_WRITE(BCHP_FTM_PHY_ANA_0_0, 0x03007753), /* Setup FSKPHY  */
      BAST_SCRIPT_WRITE(BCHP_FTM_PHY_ANA_0_1, 0x00000000), /* Setup FSKPHY  */
#endif
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCMEMADR, 0x00000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCMEMDAT, 0x0FFC0FF6),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCMEMDAT, 0x0FF70FF8),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCMEMDAT, 0x0FF90FFB),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCMEMDAT, 0x0FFC0FFE),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCMEMDAT, 0x00000002),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCMEMDAT, 0x00030003),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCMEMDAT, 0x00030001),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCMEMDAT, 0x0FFF0FFC),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCMEMDAT, 0x0FF70FF2),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCMEMDAT, 0x0FED0FE7),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCMEMDAT, 0x0FE00FDA),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCMEMDAT, 0x0FD50FD0),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCMEMDAT, 0x0FCC0FCA),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCMEMDAT, 0x0FCA0FCB),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCMEMDAT, 0x0FCF0FD5),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCMEMDAT, 0x0FDE0FE8),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCMEMDAT, 0x0FF40002),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCMEMDAT, 0x00110020),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCMEMDAT, 0x00300040),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCMEMDAT, 0x004F005D),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCMEMDAT, 0x00690073),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCMEMDAT, 0x007A007F),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCMEMDAT, 0x00800000),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSFIRCTL,  0x00000158),
      
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DTCT, 0x0005C91D),       /* set fcw for ddfs tone gen */
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DDIO, 0x25100000),       /* set diseqc i/o muxing */
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCTL00, 0x00091000),    /* enable VCNT, add tone to dac1, LNB power up gpio */
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCTL01, 0x00000013),    /* set ddfs gain 630mV */
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCTL02, 0x1B810000),    /* set dac1 trim to 0V, power down diseqc rx by default to save power */
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCTL03, 0x00314060),    /* noise estimator decimate by 8, slow CIC, differential mode output */
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_RXBT, 0x08340384),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_RERT, 0x1770660D),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_TPWC, 0x3C210A15),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCT, 0x08000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_RXRT, 0x3C033450),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_SLEW, 0x06060600),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_ADCTL, 0x02000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_RTDC1, 0x7F810000),      /* hi and lo clip thresholds */
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_RTDC2, 0x00000704),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_CICC, 0x031F4100),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_FCIC, 0x00410307),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_SCIC, 0x00410307),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_TCTL, 0x1FD40010),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_RBDT, 0x012C0320),
      BAST_SCRIPT_OR(BCHP_SDS_DSEC_DSCTL00, 0x7F006301),       /* diseqc resets, freeze integrators, clear status */
      BAST_SCRIPT_AND(BCHP_SDS_DSEC_DSCTL00, ~0x7F006301),     /* release resets, unfreeze integrators, unclear status */
      BAST_SCRIPT_EXIT
   };
   
   BSTD_UNUSED(options);

   if (((BAST_g3_P_ChannelHandle *)h->pImpl)->bHasDiseqc == false)
      return BERR_NOT_SUPPORTED;
   
   BDBG_ENTER(BAST_g3_P_ResetDiseqc);
   
   /* diseqc initialization */
   hChn->coresPoweredDown &= ~BAST_CORE_DISEQC;
   BAST_g3_P_DiseqcPowerUp(h);
   BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_diseqc_init));
   
   /* JDCHO - Adjust Quiet Time Interval 15ms(default) -> 20ms */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_FRONTEND_ADD)
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_Q15T, ~0xFFFF0000, 0x4E200000);
#endif

   /* disable vsense interrupts for reset */
   BAST_g3_P_EnableVsenseInterrupts(h, false);
   
   /* configure diseqc settings */
   BAST_g3_P_SetDiseqcVoltage(h, false);
   BAST_g3_P_SetDiseqcTone(h, false);
   BAST_g3_P_Diseqc_UpdateSettings(h);
   
   /* clear diseqc busy flag */
   hChn->diseqc->diseqcStatus.status = BAST_DiseqcSendStatus_eSuccess;
#ifndef BAST_EXCLUDE_ACW   
   hChn->diseqc->diseqcAcw = 0;
#endif
   
   /* power up phy for vsense and rx functionality */
   BAST_g3_P_PowerUpFskphy(h);
   if (h->channel == 0)
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_FTM_PHY_ANA0_1, ~0x00780000, 0x00700000);
   else
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_FTM_PHY_ANA1_1, ~0x00780000, 0x00700000);
   
   if (h->pDevice->settings.networkSpec == BAST_NetworkSpec_eEcho)
   {
      /* route selvtop signal to dsec_vctl pin */
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DDIO, ~0x07000000, 0x04000000);
   }
   
#ifdef BAST_DISEQC_ANALOG_MODE
   /* selecting diseqc path to fskphy dac essentially powers down ftm */
   hDev->bFtmPoweredDown = true;
   BAST_g3_P_OrRegister(h, BCHP_FTM_PHY_ANA_MISC, 0x00000010);    /* analog DiSEqC */
#else
   BAST_g3_P_AndRegister(h, BCHP_FTM_PHY_ANA_MISC, ~0x00000010);  /* digital DiSEqC */
#endif
   
   /* initialize vsense by setting thresholds */
   BDBG_MSG(("BAST_g3_P_ResetDiseqc: hi=%02X, lo=%02X", hChn->diseqc->dsecSettings.vsenseThresholdHi, hChn->diseqc->dsecSettings.vsenseThresholdLo));
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DS_SAR_THRSH, ~0xFFFF,
      (hChn->diseqc->dsecSettings.vsenseThresholdHi << 8) | hChn->diseqc->dsecSettings.vsenseThresholdLo);
   
   /* calibrate vsense dc offset */
   retCode = BAST_g3_P_CalibrateDcOffset(h);
   
   done:
   BDBG_LEAVE(BAST_g3_P_ResetDiseqc);
   return retCode;
}


/******************************************************************************
 BAST_g3_P_CalibrateDcOffset() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g3_P_CalibrateDcOffset(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
   
   /* disable vsense interrupts for calibration */
   BINT_DisableCallback(hChn->diseqc->hDiseqcOverVoltageCb);
   BINT_DisableCallback(hChn->diseqc->hDiseqcUnderVoltageCb);
   
   /* set lpf alpha for vsense calibration */
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DS_SAR_CONTROL, ~0xF00, 0x500);
   
#if 0
   /* TBD */
   if (h->channel == 0)
   {
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_FTM_PHY_ANA0_1, ~0x00, 0x08000000);  /* disable input switch for rx input voltage */
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_FTM_PHY_ANA0_0, ~0x700, 0xB00);      /* select bandgap, 2's comp ADC format */
   }
   else
   {
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_FTM_PHY_ANA1_1, ~0x00, 0x08000000);
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_FTM_PHY_ANA1_0, ~0x700, 0xB00);
   }
#endif
   
   /* kick start the SAR adc (self-clearing) */
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DS_SAR_CONTROL, ~0x00, 0x01);
   
   /* wait for filter value to converge */
   BKNI_EnterCriticalSection();
   
#ifdef EMU
   retCode = BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eDiseqc1, 10, BAST_g3_P_CalibrateDcOffset1);
#else
   retCode = BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eDiseqc1, 50, BAST_g3_P_CalibrateDcOffset1);
#endif
   BKNI_LeaveCriticalSection();
   
   
   return retCode;
}


/******************************************************************************
 BAST_g3_P_CalibrateDcOffset1() - ISR context
******************************************************************************/
BERR_Code BAST_g3_P_CalibrateDcOffset1(BAST_ChannelHandle h)
{
   uint32_t mb;
   
   
   /* configure DC offset */
   BAST_g3_P_ReadRegister(h, BCHP_SDS_DSEC_DS_SAR_DATA_OUT, &mb);
   BAST_g3_P_WriteRegister(h, BCHP_SDS_DSEC_DS_SAR_DC_OFFSET, &mb);
   
#if 0
   /* TBD */
   if (h->channel == 0)
   {
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_FTM_PHY_ANA0_0, ~0x700, 0x00);       /* deselect bandgap */
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_FTM_PHY_ANA0_1, ~0x08000000, 0x00);  /* enable input switch for rx input voltage */
   }
   else
   {
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_FTM_PHY_ANA1_0, ~0x700, 0x00);
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_FTM_PHY_ANA1_1, ~0x08000000, 0x00);
   }
#endif
   
   /* set lpf alpha for vsense operation */
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DS_SAR_CONTROL, ~0xF00, 0x0B00);

   /* soft reset after calibration */
   return BAST_g3_P_ResetVsense(h);
}


/******************************************************************************
 BAST_g3_P_ResetVsense() - ISR context
******************************************************************************/
BERR_Code BAST_g3_P_ResetVsense(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t mb;
   
   /* configure adc for vsense */
   BAST_g3_P_AndRegister(h, BCHP_SDS_DSEC_DS_COMMON_CONTROL, ~0x00000028);   /* disable diseqc input, select adc for vsense */
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DSCGDIV, ~0x0000FF00, 0x00002400);  /* vsense mode 3MHz */
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DS_COMMON_CONTROL, ~0x00000080, 0x00000040);   /* start vsense */
   
   /* reset lpf integrator, then clear bits */
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DS_SAR_CONTROL, ~0x00, 0x2000);
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DS_SAR_CONTROL, ~0x2000, 0x00);
   
   /* kick start the SAR adc (self-clearing) */
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DS_SAR_CONTROL, ~0x00, 0x01);
   
   /* reset max/min values, then clear bits */
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DS_SAR_CONTROL, ~0x00, 0xC0);
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DS_SAR_CONTROL, ~0xC0, 0x00);
   
   /* re-enable interrupts if previously on */
   BAST_g3_P_ReadRegister(h, BCHP_SDS_DSEC_DS_SAR_THRSH, &mb);
   if ((mb & 0x10000) == 0x10000)
   {
      BINT_EnableCallback_isr(hChn->diseqc->hDiseqcOverVoltageCb);
      BINT_EnableCallback_isr(hChn->diseqc->hDiseqcUnderVoltageCb);
   }
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_DisableDiseqcInterrupts_isr() - ISR context
******************************************************************************/
void BAST_g3_P_DisableDiseqcInterrupts_isr(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   BAST_g3_P_DisableTimer(h, BAST_TimerSelect_eDiseqc1);
   BAST_g3_P_DisableTimer(h, BAST_TimerSelect_eDiseqc2);  
      
   BINT_DisableCallback_isr(hChn->diseqc->hDiseqcDoneCb);
   BINT_DisableCallback_isr(hChn->diseqc->hDiseqcOverVoltageCb);
   BINT_DisableCallback_isr(hChn->diseqc->hDiseqcUnderVoltageCb);
   
   BINT_ClearCallback_isr(hChn->diseqc->hDiseqcDoneCb);
   BINT_ClearCallback_isr(hChn->diseqc->hDiseqcOverVoltageCb);
   BINT_ClearCallback_isr(hChn->diseqc->hDiseqcUnderVoltageCb);   
}


/******************************************************************************
 BAST_g3_P_SetDiseqcTone() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g3_P_SetDiseqcTone(
   BAST_ChannelHandle h,  /* [in] BAST channel handle */
   bool bTone             /* [in] true = tone on, false = tone off */
)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t mb;
   
   if (hChn->bHasDiseqc == false)
      return BERR_NOT_SUPPORTED;
   
   if (hChn->coresPoweredDown & BAST_CORE_DISEQC)
      return BAST_ERR_POWERED_DOWN;
      
   BDBG_ENTER(BAST_g3_P_SetDiseqcTone);
   hChn->diseqc->bDiseqcToneOn = bTone;
   
   BAST_g3_P_ReadRegister(h, BCHP_SDS_DSEC_DSCTL00, &mb);
   if (bTone)
      mb |= 0x30; /* tone on */
   else
      mb &= ~0x30; /* tone off */
   BAST_g3_P_WriteRegister(h, BCHP_SDS_DSEC_DSCTL00, &mb);
   
   BDBG_LEAVE(BAST_g3_P_SetDiseqcTone);
   return retCode;
}


#define BAST_DISEQC_TONE_DETECT_LEGACY
/******************************************************************************
 BAST_g3_P_GetDiseqcTone() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g3_P_GetDiseqcTone(
   BAST_ChannelHandle h,  /* [in] BAST channel handle */
   bool *bTone            /* [out] true = tone present, false = tone absent */
)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t adctl, rtdc2, dsctl3, mb;
   
#ifndef BAST_DISEQC_TONE_DETECT_LEGACY
   static const uint32_t script_diseqc_detect_tone[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_ADCTL, 0x06000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_RTDC2, 0x0000060F),
      BAST_SCRIPT_WRITE(BCHP_SDS_DSEC_DSCTL03, 0x003F4060),    /* noise estimator decimate by 8, slow CIC, soft demod mode for tone detect */
      BAST_SCRIPT_OR(BCHP_SDS_DSEC_DSCTL00, 0x7F000300),       /* diseqc resets, freeze integrators, clear status */
      BAST_SCRIPT_AND(BCHP_SDS_DSEC_DSCTL00, ~0x7F000300),     /* release resets, unfreeze integrators, unclear status */
      BAST_SCRIPT_EXIT
   };
#endif
   
   if (hChn->bHasDiseqc == false)
      return BERR_NOT_SUPPORTED;
   
   if (hChn->coresPoweredDown & BAST_CORE_DISEQC)
      return BAST_ERR_POWERED_DOWN;
   
   if (hChn->diseqc->diseqcStatus.status == BAST_DiseqcSendStatus_eBusy)
      return BAST_ERR_DISEQC_BUSY;
   
   BDBG_ENTER(BAST_g3_P_GetDiseqcTone);
   
   /* enable diseqc rx for tone detect */
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DSCTL02, ~0x00800000, 0x00000000);
   
   /* configure adc for diseqc rx */
   BAST_g3_P_OrRegister(h, BCHP_SDS_DSEC_DS_COMMON_CONTROL, 0x00000080);   /* stop vsense */
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DSCGDIV, ~0x0000FF00, 0x00000900);  /* AFE mode 12MHz */
   BAST_g3_P_OrRegister(h, BCHP_SDS_DSEC_DS_COMMON_CONTROL, 0x00000028);   /* enable diseqc input, select adc for diseqc rx */
   BKNI_Sleep(45);   /* use bkni sleep because task level function */
   
   /* save registers we are going to modify */
   BAST_g3_P_ReadRegister(h, BCHP_SDS_DSEC_ADCTL, &adctl);
   BAST_g3_P_ReadRegister(h, BCHP_SDS_DSEC_RTDC2, &rtdc2);
   BAST_g3_P_ReadRegister(h, BCHP_SDS_DSEC_DSCTL03, &dsctl3);
   
#ifdef BAST_DISEQC_TONE_DETECT_LEGACY
   /* DIRECT AMPLITUDE COMPARISON method */
   /* set diseqc to legacy mode and use abs value of ADC output */
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DSCTL03, ~0x00070000, 0x00080000);
   BAST_g3_P_OrRegister(h, BCHP_SDS_DSEC_DSFIRCTL, 0x00001000);   /* bypass FIR filter */
   
   /* lower PGA gain control */
   if (h->channel == 0)
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_FTM_PHY_ANA0_1, ~0x00780000, 0x00500000);
   else
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_FTM_PHY_ANA1_1, ~0x00780000, 0x00500000);
   
   /* set thresholds for 300mV */
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DSCTL03, ~0x00FF, 0x0048); /* (300mV)/(1060mV/2^8) = 72 = 0x48 */
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DSCTL03, ~0xFF00, 0x2400); /* set this lower than DSCTL12 for hysterisis */
   
   BAST_g3_P_ToggleBit(h, BCHP_SDS_DSEC_DSCTL01, 0x00008000);     /* clear tone status */
   BAST_g3_P_OrRegister(h, BCHP_SDS_DSEC_DSCTL01, 0x00004000);    /* enable tone detection */
   
   /* reset integrators and diseqc */
   BAST_g3_P_ToggleBit(h, BCHP_SDS_DSEC_DSCTL00, 0x7F000301);
   BKNI_Sleep(5);
   
   /* check if tone present */
   BAST_g3_P_ReadRegister(h, BCHP_SDS_DSEC_DST2, &mb);
   if (mb & 0x01)
      *bTone = true;
   else
      *bTone = false;
   
   /* restore PGA gain control */
   if (h->channel == 0)
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_FTM_PHY_ANA0_1, ~0x00780000, 0x00700000);
   else
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_FTM_PHY_ANA1_1, ~0x00780000, 0x00700000);
   
   BAST_g3_P_AndRegister(h, BCHP_SDS_DSEC_DSCTL01, ~0x00004000);   /* disable tone detection */
   BAST_g3_P_ToggleBit(h, BCHP_SDS_DSEC_DSCTL01, 0x00008000);      /* clear tone status */
   BAST_g3_P_AndRegister(h, BCHP_SDS_DSEC_DSFIRCTL, ~0x00001000);  /* re-enable FIR filter */
#else
   /* ENERGY DETECT method */
   BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_diseqc_detect_tone));
   BKNI_Sleep(5);    /* use bkni sleep because task level function */

   BAST_g3_P_ReadRegister(h, BCHP_SDS_DSEC_RTDC2, &mb);
   mb = mb >> 23;
   if (mb > hChn->diseqc->dsecSettings.toneThreshold)
      *bTone = true;
   else
      *bTone = false;
#endif
   
   /* restore registers */
   BAST_g3_P_WriteRegister(h, BCHP_SDS_DSEC_ADCTL, &adctl);
   BAST_g3_P_WriteRegister(h, BCHP_SDS_DSEC_RTDC2, &rtdc2);
   BAST_g3_P_WriteRegister(h, BCHP_SDS_DSEC_DSCTL03, &dsctl3);
   
   /* reset diseqc and packet pointers */
   BAST_g3_P_ToggleBit(h, BCHP_SDS_DSEC_DSCTL00, 0x00000300);
   
   /* disable diseqc rx after tone detect */
   BAST_g3_P_OrRegister(h, BCHP_SDS_DSEC_DSCTL02, 0x00800000);
   
   /* calibrate vsense dc offset */
   retCode = BAST_g3_P_CalibrateDcOffset(h);

#ifndef BAST_DISEQC_TONE_DETECT_LEGACY
   done:
#endif
   BDBG_LEAVE(BAST_g3_P_GetDiseqcTone);
   return retCode;
}


/******************************************************************************
 BAST_g3_P_SetDiseqcVoltage() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g3_P_SetDiseqcVoltage(
   BAST_ChannelHandle h, /* [in] BAST channel handle */
   bool bVtop            /* [in] true = VTOP, false = VBOT */
)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t mb;

   if (hChn->bHasDiseqc == false)
      return BERR_NOT_SUPPORTED;
   
   if (hChn->coresPoweredDown & BAST_CORE_DISEQC)
      return BAST_ERR_POWERED_DOWN;
   
   BDBG_ENTER(BAST_g3_P_SetDiseqcVoltage);
   
   BAST_g3_P_ReadRegister(h, BCHP_SDS_DSEC_DSCTL00, &mb);
   if (bVtop)
      mb |= 0x00100400;    /* vtop */
   else
      mb &= ~0x00100400;   /* vbot */
   BAST_g3_P_WriteRegister(h, BCHP_SDS_DSEC_DSCTL00, &mb);
   
   BDBG_LEAVE(BAST_g3_P_SetDiseqcVoltage);
   return retCode;
}


/******************************************************************************
 BAST_g3_P_GetDiseqcVoltage() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g3_P_GetDiseqcVoltage(
   BAST_ChannelHandle h, /* [in] BAST channel handle */
   uint8_t *pVoltage     /* [out] voltage estimation */
)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t mb;
   
   if (hChn->bHasDiseqc == false)
      return BERR_NOT_SUPPORTED;
   
   if (hChn->coresPoweredDown & BAST_CORE_DISEQC)
      return BAST_ERR_POWERED_DOWN;
      
   if (hChn->diseqc->diseqcStatus.status == BAST_DiseqcSendStatus_eBusy)
      return BAST_ERR_DISEQC_BUSY;
   
   BDBG_ENTER(BAST_g3_P_GetDiseqcVoltage);
   
   /* read ADC output */
   BAST_g3_P_ReadRegister(h, BCHP_SDS_DSEC_DS_SAR_DATA_OUT, &mb);
   BDBG_MSG(("SDS_DS_SAR_DATA_OUT = %08X", mb));
   *pVoltage = (uint8_t)(mb & 0xFF);
   
   /* re-calibrate vsense dc offset (offset may change due to temperature drift) */
   retCode = BAST_g3_P_CalibrateDcOffset(h);
   
   BDBG_LEAVE(BAST_g3_P_GetDiseqcVoltage);
   return retCode;
}


/******************************************************************************
 BAST_g3_P_EnableDiseqcLnb() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g3_P_EnableDiseqcLnb(
   BAST_ChannelHandle h,  /* [in] BAST channel handle */
   bool bEnable           /* [in] true = LNB on, false = LNB off */
)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t mb;
   
   if (((BAST_g3_P_ChannelHandle *)h->pImpl)->bHasDiseqc == false)
      return BERR_NOT_SUPPORTED;
      
   BDBG_ENTER(BAST_g3_P_EnableDiseqcLnb);
   
   BAST_g3_P_ReadRegister(h, BCHP_SDS_DSEC_DSCTL00, &mb);
   if (bEnable)
      mb |= 0x1000; /* LNB on */
   else
      mb &= ~0x1000; /* LNB off */
   BAST_g3_P_WriteRegister(h, BCHP_SDS_DSEC_DSCTL00, &mb);
   
   BDBG_LEAVE(BAST_g3_P_EnableDiseqcLnb);
   return retCode;
}


/******************************************************************************
 BAST_g3_P_EnableVsenseInterrupts() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g3_P_EnableVsenseInterrupts(BAST_ChannelHandle h, bool bEnable)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
   
   if (hChn->bHasDiseqc == false)
      return BERR_NOT_SUPPORTED;
   
   BDBG_ENTER(BAST_g3_P_EnableVsenseInterrupts);
   
   if (bEnable)
   {
      /* BDBG_MSG(("ENABLE vsense\n")); */
	  /* configure adc for vsense */
      BAST_g3_P_AndRegister(h, BCHP_SDS_DSEC_DS_COMMON_CONTROL, ~0x00000028);   /* disable diseqc input, select adc for vsense */
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DSCGDIV, ~0x0000FF00, 0x00002400);  /* vsense mode 3MHz */
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DS_COMMON_CONTROL, ~0x00000080, 0x00000040);   /* start vsense */

      /* reset lpf integrator, then clear bits */
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DS_SAR_CONTROL, ~0x00, 0x2000);
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DS_SAR_CONTROL, ~0x2000, 0x00);
      /* enable status output */
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DS_SAR_THRSH, 0xFFFFFFFF, 0x10000);
      
      /* enable the diseqc transaction done interrupt */
      retCode = BINT_ClearCallback(hChn->diseqc->hDiseqcOverVoltageCb);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      retCode = BINT_ClearCallback(hChn->diseqc->hDiseqcUnderVoltageCb);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      
      retCode = BINT_EnableCallback(hChn->diseqc->hDiseqcOverVoltageCb);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      retCode = BINT_EnableCallback(hChn->diseqc->hDiseqcUnderVoltageCb);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
   }
   else
   {
      /* BDBG_MSG(("DISABLE vsense\n")); */
      
      /* disable status output */
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DS_SAR_THRSH, ~0x10000, 0x00);
      
      /* disable the diseqc transaction done interrupt */
      retCode = BINT_ClearCallback(hChn->diseqc->hDiseqcOverVoltageCb);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      retCode = BINT_ClearCallback(hChn->diseqc->hDiseqcUnderVoltageCb);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      
      retCode = BINT_DisableCallback(hChn->diseqc->hDiseqcOverVoltageCb);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      retCode = BINT_DisableCallback(hChn->diseqc->hDiseqcUnderVoltageCb);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
   }
   
   BDBG_LEAVE(BAST_g3_P_EnableVsenseInterrupts);
   return retCode;
}

/******************************************************************************
 BAST_g3_P_GetDiseqcVsenseEventHandle() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g3_P_GetDiseqcVsenseEventHandles(
   BAST_ChannelHandle h, 
   BKNI_EventHandle *hDiseqcOverVoltageEvent, 
   BKNI_EventHandle *hDiseqcUnderVoltageEvent)
{
   BERR_Code retCode = BERR_SUCCESS;
   
   if (((BAST_g3_P_ChannelHandle *)h->pImpl)->bHasDiseqc == false)
      return BERR_NOT_SUPPORTED;
   
   *hDiseqcOverVoltageEvent = ((BAST_g3_P_ChannelHandle *)h->pImpl)->diseqc->hDiseqcOverVoltageEvent;
   *hDiseqcUnderVoltageEvent = ((BAST_g3_P_ChannelHandle *)h->pImpl)->diseqc->hDiseqcUnderVoltageEvent;
   return retCode;
}


/******************************************************************************
 BAST_g3_P_DiseqcOverVoltage_isr()
******************************************************************************/
void BAST_g3_P_DiseqcOverVoltage_isr(void *p, int param)
{
   BAST_ChannelHandle h = (BAST_ChannelHandle)p;
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
   BSTD_UNUSED(param);
   
   /* BDBG_MSG(("Diseqc%d voltage ABOVE upper threshold(%02X)", h->channel, hChn->diseqc->dsecSettings.vsenseThresholdHi)); */
   BAST_g3_P_IncrementInterruptCounter(h, BAST_g3_IntID_eDiseqcVoltageGtHiThresh);
   
   retCode = BINT_ClearCallback_isr(hChn->diseqc->hDiseqcOverVoltageCb);
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   
   BKNI_SetEvent(hChn->diseqc->hDiseqcOverVoltageEvent);
}


/******************************************************************************
 BAST_g3_P_DiseqcUnderVoltage_isr()
******************************************************************************/
void BAST_g3_P_DiseqcUnderVoltage_isr(void *p, int param)
{
   BAST_ChannelHandle h = (BAST_ChannelHandle)p;
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
   BSTD_UNUSED(param);
   
   /* BDBG_MSG(("Diseqc%d voltage BELOW lower threshold(%02X)", h->channel, hChn->diseqc->dsecSettings.vsenseThresholdLo)); */
   BAST_g3_P_IncrementInterruptCounter(h, BAST_g3_IntID_eDiseqcVoltageLtLoThresh);
   
   retCode = BINT_ClearCallback_isr(hChn->diseqc->hDiseqcUnderVoltageCb);
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   
   BKNI_SetEvent(hChn->diseqc->hDiseqcUnderVoltageEvent);
}


#ifndef BAST_EXCLUDE_ACW
/******************************************************************************
 BAST_g3_P_SendACW() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g3_P_SendACW(
   BAST_ChannelHandle h,  /* [in] BAST channel handle */
   uint8_t            acw /* [in] auto control word */
)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;

   if (hChn->bHasDiseqc == false)
      return BERR_NOT_SUPPORTED;
   
   if (hChn->coresPoweredDown & BAST_CORE_DISEQC)
      return BAST_ERR_POWERED_DOWN;
   
   BDBG_ENTER(BAST_g3_P_SendACW);
   
   /* clear diseqc status and set send status to busy */
   BAST_g3_P_ToggleBit(h, BCHP_SDS_DSEC_DSCTL00, 0x00000001);
   hChn->diseqc->diseqcStatus.status = BAST_DiseqcSendStatus_eBusy;
   hChn->diseqc->diseqcAcw = acw;
   
   /* set control word */
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DSCTL01, ~0xFF000000, (acw << 24));
   
   /* enable auto control word tx */
   BAST_g3_P_OrRegister(h, BCHP_SDS_DSEC_DSCTL00, 0x00200000);
   
   /* start acw transmission */
   BAST_g3_P_ToggleBit(h, BCHP_SDS_DSEC_DSCTL00, 0x00008000);
   
   /* 32ms setup + 8ms start bit + 32ms control + 24ms error correction + 8ms polarity ~ 104ms */
   /* protect enable timer from interrupts since SDS_MISC_TMRCTL shared */
   BKNI_EnterCriticalSection();
   retCode = BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eDiseqc1, 150000, BAST_g3_P_DoneACW);
   BKNI_LeaveCriticalSection();
   
   BDBG_LEAVE(BAST_g3_P_SendACW);
   return retCode;
}


/******************************************************************************
 BAST_g3_P_DoneACW() - ISR context
******************************************************************************/
BERR_Code BAST_g3_P_DoneACW(
   BAST_ChannelHandle h    /* [in] BAST channel handle */
)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t dst1;

   /* reset status */
   hChn->diseqc->diseqcStatus.status = BAST_DiseqcSendStatus_eSuccess;
   
   /* check for cw done indicator */
#if (BCHP_CHIP==4517)
   BAST_g3_P_ReadRegister(h, BCHP_SDS_DSEC_INTR2_CPU_STATUS, &dst1);
   /* BDBG_MSG(("DST1=%08X", dst1)); */
   if ((dst1 & 0x20) == 0)
      hChn->diseqc->diseqcStatus.status = BAST_DiseqcSendStatus_eAcwTimeout;
#else
   BAST_g3_P_ReadRegister(h, BCHP_SDS_DSEC_DST1, &dst1);
   /* BDBG_MSG(("DST1=%08X", dst1)); */
   if ((dst1 & 0x10000000) == 0)
      hChn->diseqc->diseqcStatus.status = BAST_DiseqcSendStatus_eAcwTimeout;
#endif
   
   /* check polarity bit of acw */
   if (hChn->diseqc->diseqcAcw & 0x80)
      BAST_g3_P_OrRegister(h, BCHP_SDS_DSEC_DSCTL00, 0x00100000);    /* finish with vtop */
   else
      BAST_g3_P_AndRegister(h, BCHP_SDS_DSEC_DSCTL00, ~0x00100000);  /* finish with vbot */
   
   /* revert to non-ACW tx */
   BAST_g3_P_AndRegister(h, BCHP_SDS_DSEC_DSCTL00, ~0x00200000);
   
   /* reset control word and state machine */
   BAST_g3_P_ToggleBit(h, BCHP_SDS_DSEC_DSCTL00, 0x00006000);
   
   /* set diseqc event */
   BKNI_SetEvent(hChn->diseqc->hDiseqcEvent);
   
   return retCode;
}
#endif


/******************************************************************************
 BAST_g3_P_SendDiseqcCommand() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g3_P_SendDiseqcCommand(
   BAST_ChannelHandle h,    /* [in] BAST channel handle */
   const uint8_t *pSendBuf, /* [in] contains data to send */
   uint8_t sendBufLen       /* [in] number of bytes to send */
)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t i;

   if (hChn->bHasDiseqc == false)
      return BERR_NOT_SUPPORTED;
   
   if (hChn->coresPoweredDown & BAST_CORE_DISEQC)
      return BAST_ERR_POWERED_DOWN;
   
   BDBG_ENTER(BAST_g3_P_SendDiseqcCommand);
   
   /* reset statuses */
   hChn->diseqc->diseqcStatus.status = BAST_DiseqcSendStatus_eBusy;
   hChn->diseqc->diseqcStatus.nReplyBytes = 0;
   
   /* update settings based on diseqc ctl parameter */
   retCode = BAST_g3_P_Diseqc_UpdateSettings(h);
   
   /* copy diseqc message to channel handle */
   hChn->diseqc->diseqcSendCount = 0;
   hChn->diseqc->diseqcSendLen = sendBufLen;
   for (i = 0; i < sendBufLen; i++)
      hChn->diseqc->diseqcSendBuffer[i] = pSendBuf[i];
   
   /* delay since h/w does not insert idle time for tone off case */
   /* protect enable timer from interrupts since SDS_MISC_TMRCTL shared */
   BKNI_EnterCriticalSection();
#ifdef EMU
   retCode = BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eDiseqc1, 10, BAST_g3_P_SendDiseqcCommand1);
#else

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_FRONTEND_ADD)
   if ((hChn->diseqc->dsecSettings.bEnableToneburst == false) && hChn->diseqc->dsecSettings.preTxDelay)
      retCode = BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eDiseqc1, hChn->diseqc->dsecSettings.preTxDelay * 1000, BAST_g3_P_SendDiseqcCommand1);
   else
      retCode = BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eDiseqc1, 50, BAST_g3_P_SendDiseqcCommand1);
#else
   if ((hChn->diseqc->bDiseqcToneOn == false) && hChn->diseqc->dsecSettings.preTxDelay)
      retCode = BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eDiseqc1, hChn->diseqc->dsecSettings.preTxDelay * 1000, BAST_g3_P_SendDiseqcCommand1);
   else
      retCode = BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eDiseqc1, 50, BAST_g3_P_SendDiseqcCommand1);
#endif

#endif
   BKNI_LeaveCriticalSection();
   
   BDBG_LEAVE(BAST_g3_P_SendDiseqcCommand);
   return retCode;
}

   
/******************************************************************************
 BAST_g3_P_SendDiseqcCommand1() - ISR context
******************************************************************************/
BERR_Code BAST_g3_P_SendDiseqcCommand1(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t i, mb, dsctl;
   uint8_t nbytes;
   
   /* clear diseqc status */
   BAST_g3_P_ToggleBit(h, BCHP_SDS_DSEC_DSCTL00, 0x00000001);
   BAST_g3_P_ReadRegister(h, BCHP_SDS_DSEC_DSCTL00, &dsctl);
   
   /* don't expect reply by default */
   hChn->diseqc->diseqcStatus.bReplyExpected = false;
   
   if (hChn->diseqc->diseqcSendLen)
   {
      if (hChn->diseqc->dsecSettings.bOverrideFraming)
      {
         if (hChn->diseqc->dsecSettings.bExpectReply)
         {
            /* always expect reply byte */
            hChn->diseqc->diseqcStatus.bReplyExpected = true;
            dsctl |= 0x0000000C;
         }
         else
         {
            /* don't expect reply byte */
            dsctl &= ~0x00000008;
            dsctl |= 0x00000004;
         }
      }
      else
      {
         /* reply expectation depends on framing byte */
         dsctl &= ~0x00000004;
         if (hChn->diseqc->diseqcSendBuffer[0] & 0x02)
            hChn->diseqc->diseqcStatus.bReplyExpected = true;
      }
      
      /* fifo limit is 16 bytes */
      if (hChn->diseqc->diseqcSendLen <= 16)
         nbytes = hChn->diseqc->diseqcSendLen;
      else
         nbytes = 16;
      
      for (i = 0; i < nbytes; i++)
      {
         mb = hChn->diseqc->diseqcSendBuffer[hChn->diseqc->diseqcSendCount++];
         BAST_g3_P_WriteRegister(h, BCHP_SDS_DSEC_DSCMD, &mb);
      }
   }
   else
   {
      if (hChn->diseqc->dsecSettings.bExpectReply)
      {
         /* receive only mode */
         hChn->diseqc->diseqcStatus.bReplyExpected = true;
         dsctl |= 0x0000000C;
      }
      else
      {
         /* don't expect reply byte */
         dsctl &= ~0x00000008;
         dsctl |= 0x00000004;
      }
   }
   
   if (hChn->diseqc->diseqcStatus.bReplyExpected)
   {
      /* power on diseqc rx if reply expected */
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DSCTL02, ~0x00800000, 0x00000000);
      
      /* configure adc for diseqc rx */
      BAST_g3_P_OrRegister(h, BCHP_SDS_DSEC_DS_COMMON_CONTROL, 0x00000080);   /* stop vsense */
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DSCGDIV, ~0x0000FF00, 0x00000900);  /* AFE mode 12MHz */
      BAST_g3_P_OrRegister(h, BCHP_SDS_DSEC_DS_COMMON_CONTROL, 0x00000028);   /* enable diseqc input, select adc for diseqc rx */
   }
   
   /* check for extended send */
   if (hChn->diseqc->diseqcSendLen > 16)
   {
      /* set almost empty levels */
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DSCTL01, ~0x00070000, 0x00020000);
      
      /* set up tx fifo almost empty interrupt */
      BINT_ClearCallback_isr(hChn->diseqc->hDiseqcTxFifoAlmostEmptyCb);
      BINT_EnableCallback_isr(hChn->diseqc->hDiseqcTxFifoAlmostEmptyCb);
   }
   
   /* clear and enable the diseqc transaction done interrupt */
   BINT_ClearCallback_isr(hChn->diseqc->hDiseqcDoneCb);
   BINT_EnableCallback_isr(hChn->diseqc->hDiseqcDoneCb);
   
   /* start the transaction */
   dsctl |= 0x00000002;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_DSEC_DSCTL00, &dsctl);
   BAST_g3_P_AndRegister(h, BCHP_SDS_DSEC_DSCTL00, ~0x00000002);  /* clear start txn */
   
   return retCode;
}


/******************************************************************************
 BAST_g3_P_GetDiseqcStatus()
******************************************************************************/
BERR_Code BAST_g3_P_GetDiseqcStatus(
   BAST_ChannelHandle h,       /* [in] BAST channel handle */
   BAST_DiseqcStatus  *pStatus /* [out] status of last transaction */
)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;

   if (hChn->bHasDiseqc == false)
      return BERR_NOT_SUPPORTED;
   
   BDBG_ENTER(BAST_g3_P_GetDiseqcStatus);
   
   *pStatus = hChn->diseqc->diseqcStatus;
   
   BDBG_LEAVE(BAST_g3_P_GetDiseqcStatus);
   return retCode;
}


/******************************************************************************
 BAST_g3_P_GetDiseqcEventHandle()
******************************************************************************/
BERR_Code BAST_g3_P_GetDiseqcEventHandle(BAST_ChannelHandle h, BKNI_EventHandle *hDiseqcEvent)
{
   BERR_Code retCode = BERR_SUCCESS;
   
   if (((BAST_g3_P_ChannelHandle *)h->pImpl)->bHasDiseqc == false)
      return BERR_NOT_SUPPORTED;
   
   BDBG_ENTER(BAST_g3_P_GetDiseqcEventHandle);
   
   *hDiseqcEvent = ((BAST_g3_P_ChannelHandle *)h->pImpl)->diseqc->hDiseqcEvent;
   
   BDBG_LEAVE(BAST_g3_P_GetDiseqcEventHandle);
   return retCode;
}


/******************************************************************************
 BAST_g3_P_Diseqc_DoneIsr()
******************************************************************************/
void BAST_g3_P_DiseqcDone_isr(void *p, int param)
{
   BAST_ChannelHandle h = (BAST_ChannelHandle)p;
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t i, mb, dst1;
   
   BSTD_UNUSED(param);
   
   BAST_g3_P_IncrementInterruptCounter(h, BAST_g3_IntID_eDiseqcDone);
   BINT_ClearCallback_isr(hChn->diseqc->hDiseqcDoneCb);
   BINT_DisableCallback_isr(hChn->diseqc->hDiseqcDoneCb);
   hChn->diseqc->diseqcStatus.status = BAST_DiseqcSendStatus_eSuccess; /* clear diseqc busy flag */
   
   BAST_g3_P_ReadRegister(h, BCHP_SDS_DSEC_DST1, &dst1);
   if (dst1 & 0x01000000)
   {
      /* error detected */
      if (dst1 & 0x1000)
         hChn->diseqc->diseqcStatus.status = BAST_DiseqcSendStatus_eRxOverflow;
      else if (dst1 & 0x0800)
         hChn->diseqc->diseqcStatus.status = BAST_DiseqcSendStatus_eRxReplyTimeout;
      else if (dst1 & 0x0400)
         hChn->diseqc->diseqcStatus.status = BAST_DiseqcSendStatus_eRxParityError;
   }
   
   /* get size of diseqc reply */
   hChn->diseqc->diseqcStatus.nReplyBytes = (dst1 >> 19) & 0x1F;
   
   /* limit size of reply to maximum buffer size */
   if (hChn->diseqc->diseqcStatus.nReplyBytes > DISEQC_REPLY_BUF_SIZE)
      hChn->diseqc->diseqcStatus.nReplyBytes = DISEQC_REPLY_BUF_SIZE;
      
   if (hChn->diseqc->diseqcStatus.nReplyBytes > 0)
   {
      for (i = 0; i < hChn->diseqc->diseqcStatus.nReplyBytes; i++)
      {
         BAST_g3_P_ReadRegister(h, BCHP_SDS_DSEC_DSRPLY, &mb);
         hChn->diseqc->diseqcStatus.replyBuffer[i] = (uint8_t)(mb & 0xFF);
      }
   }
   
   /* reset the FIFO, memory, noise integrator, etc. */
   BAST_g3_P_ToggleBit(h, BCHP_SDS_DSEC_DSCTL00, 0x7F002201);
   
   /* power off diseqc rx */
   BAST_g3_P_OrRegister(h, BCHP_SDS_DSEC_DSCTL02, 0x00800000);
   
   /* set diseqc event */
   BKNI_SetEvent(hChn->diseqc->hDiseqcEvent);
}


/******************************************************************************
* BAST_g3_P_Diseqc_UpdateSettings()
******************************************************************************/
BERR_Code BAST_g3_P_Diseqc_UpdateSettings(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t mb;
   
   /* set envelope or tone mode */
   BAST_g3_P_ReadRegister(h, BCHP_SDS_DSEC_DDIO, &mb);
   mb &= ~0x700000;
   if (hChn->diseqc->dsecSettings.bEnvelope)
      mb |= 0x100000;
   if (hChn->diseqc->dsecSettings.bEnableLNBPU)
      mb |= 0x10000000;
   else
      mb &= ~0x10000000;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_DSEC_DDIO, &mb);
   
   /* tone align enable or disable */
   BAST_g3_P_ReadRegister(h, BCHP_SDS_DSEC_DSCTL01, &mb);
   if (hChn->diseqc->dsecSettings.bToneAlign)
      mb |= 0x0800;
   else
      mb &= ~0x0800;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_DSEC_DSCTL01, &mb);
   
   /* receive reply timeout enable or disable */
   BAST_g3_P_ReadRegister(h, BCHP_SDS_DSEC_DSCTL01, &mb);
   if (hChn->diseqc->dsecSettings.bDisableRRTO)
      mb |= 0x400000;
   else
      mb &= ~0x400000;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_DSEC_DSCTL01, &mb);
   
   /* receive reply timeout setting */
   BAST_g3_P_ReadRegister(h, BCHP_SDS_DSEC_RXRT, &mb);
   mb &= 0xFFF00000;
   mb |= (hChn->diseqc->dsecSettings.rrtoUsec & 0x000FFFFF);
   BAST_g3_P_WriteRegister(h, BCHP_SDS_DSEC_RXRT, &mb);
   
   /* burst enable or disable */
   BAST_g3_P_ReadRegister(h, BCHP_SDS_DSEC_DSCTL00, &mb);
   if (hChn->diseqc->dsecSettings.bEnableToneburst)
      mb |= 0x40;
   else
      mb &= ~0x40;
   /* tone A or tone B */
   if (hChn->diseqc->dsecSettings.bToneburstB)
      mb |= 0x80;
   else
      mb &= ~0x80;
   /* expect reply enable or disable */
   if ((hChn->diseqc->dsecSettings.bOverrideFraming) && (hChn->diseqc->dsecSettings.bExpectReply == false))
      mb |= 0x04;
   else
      mb &= ~0x04;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_DSEC_DSCTL00, &mb);
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_FRONTEND_ADD)
   
#else
   /* program pretx delay */
   BAST_g3_P_ReadRegister(h, BCHP_SDS_DSEC_Q15T, &mb);
   mb |= (hChn->diseqc->dsecSettings.preTxDelay * 1000) << 16;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_DSEC_Q15T, &mb);
#endif
   return retCode;
}

#if 0
/******************************************************************************
 BAST_g3_P_Diseqc_ReadReply()
******************************************************************************/
BERR_Code BAST_g3_P_Diseqc_ReadReply(BAST_ChannelHandle h, uint8_t *pRcvBuf, uint8_t *pRcvBufLen, BAST_DiseqcStatus *pStatus)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint8_t i;
   
   *pStatus = hChn->diseqc->diseqcStatus;
   *pRcvBufLen = 0;
   if (hChn->diseqc->diseqcStatus.bReplyExpected)
   {
      *pRcvBufLen = hChn->diseqc->diseqcStatus.nReplyBytes;
      for (i = 0; i < hChn->diseqc->diseqcStatus.nReplyBytes; i++)
         pRcvBuf[i] = hChn->diseqc->diseqcStatus.replyBuffer[i];
   }
   return BERR_SUCCESS;
}
#endif


/******************************************************************************
 BAST_g3_P_DiseqcTxFifoAlmostEmpty_isr()
******************************************************************************/
void BAST_g3_P_DiseqcTxFifoAlmostEmpty_isr(void *p, int param)
{
   BAST_ChannelHandle h = (BAST_ChannelHandle)p;
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t mb;
   uint8_t bytes_avail, i, nbytes;
   
   BSTD_UNUSED(param);
   
   BAST_g3_P_IncrementInterruptCounter(h, BAST_g3_IntID_eDiseqcTxFifoAlmostEmpty);
   BINT_ClearCallback_isr(hChn->diseqc->hDiseqcTxFifoAlmostEmptyCb);
   BINT_DisableCallback_isr(hChn->diseqc->hDiseqcTxFifoAlmostEmptyCb);
   
   /* determine number of bytes available in tx fifo */
   BAST_g3_P_ReadRegister(h, BCHP_SDS_DSEC_DST1, &mb);
   bytes_avail = (mb >> 14) & 0x1F;
   
   /* cannot send more bytes than what's available */
   if (hChn->diseqc->diseqcSendLen - hChn->diseqc->diseqcSendCount <= bytes_avail)
      nbytes = hChn->diseqc->diseqcSendLen - hChn->diseqc->diseqcSendCount;
   else
      nbytes = bytes_avail;
   
   for (i = 0; i < nbytes; i++)
   {
      mb = hChn->diseqc->diseqcSendBuffer[hChn->diseqc->diseqcSendCount++];
      BAST_g3_P_WriteRegister(h, BCHP_SDS_DSEC_DSCMD, &mb);
   }
   
   if (hChn->diseqc->diseqcSendCount < hChn->diseqc->diseqcSendLen)
   {
      /* re-arm tx fifo almost empty interrupt if not done */
      BINT_ClearCallback_isr(hChn->diseqc->hDiseqcTxFifoAlmostEmptyCb);
      BINT_EnableCallback_isr(hChn->diseqc->hDiseqcTxFifoAlmostEmptyCb);
   }
}

