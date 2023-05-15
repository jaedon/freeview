/******************************************************************************
*    (c)2011-2013 Broadcom Corporation
*
* This program is the proprietary software of Broadcom Corporation and/or its licensors,
* and may only be used, duplicated, modified or distributed pursuant to the terms and
* conditions of a separate, written license agreement executed between you and Broadcom
* (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
* no license (express or implied), right to use, or waiver of any kind with respect to the
* Software, and Broadcom expressly reserves all rights in and to the Software and all
* intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
* HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
* NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
*
* Except as expressly set forth in the Authorized License,
*
* 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
* secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
* and to use this information only in connection with your use of Broadcom integrated circuit products.
*
* 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
* AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
* WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
* THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
* OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
* LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
* OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
* USE OR PERFORMANCE OF THE SOFTWARE.
*
* 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
* LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
* EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
* USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
* ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
* LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
* ANY LIMITED REMEDY.
*
* $brcm_Workfile: $
* $brcm_Revision: $
* $brcm_Date: $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: $
*
*****************************************************************************/

#include "bdsq.h"
#include "bdsq_priv.h"
#include "bdsq_g1_priv.h"
BDBG_MODULE(bdsq_g1_priv);


/******************************************************************************
 BDSQ_g1_P_Open()
******************************************************************************/
BERR_Code BDSQ_g1_P_Open(BDSQ_Handle *h, BCHP_Handle hChip, void *pReg, BINT_Handle hInterrupt, const BDSQ_Settings *pSettings)
{
   BERR_Code retCode = BERR_SUCCESS;
   BDSQ_Handle hDev;
   BDSQ_g1_P_Handle *hG1Dev;
   uint8_t i;

   BSTD_UNUSED(hChip);

   BDBG_ENTER(BDSQ_g1_P_Open);

   /* allocate heap memory for device handle */
   hDev = (BDSQ_Handle)BKNI_Malloc(sizeof(BDSQ_P_Handle));
   BDBG_ASSERT(hDev);
   BKNI_Memset((void*)hDev, 0, sizeof(BDSQ_P_Handle));
   hG1Dev = (BDSQ_g1_P_Handle *)BKNI_Malloc(sizeof(BDSQ_g1_P_Handle));
   BDBG_ASSERT(hG1Dev);
   BKNI_Memset((void*)hG1Dev, 0, sizeof(BDSQ_g1_P_Handle));
   hDev->pImpl = (void*)hG1Dev;

   /* allocate heap memory for channel handle pointer */
   hDev->pChannels = (BDSQ_P_ChannelHandle **)BKNI_Malloc(BDSQ_NUM_CHANNELS * sizeof(BDSQ_P_ChannelHandle *));
   BDBG_ASSERT(hDev->pChannels);
   BKNI_Memset((void*)hDev->pChannels, 0, BDSQ_NUM_CHANNELS * sizeof(BDSQ_P_ChannelHandle *));

   /* initialize device handle */
   BKNI_Memcpy((void*)(&(hDev->settings)), (void*)pSettings, sizeof(BDSQ_Settings));
   hG1Dev->hRegister = (BREG_Handle)pReg;
   hG1Dev->hInterrupt = hInterrupt;

   /* create events */
   /* TBD... */

   hDev->totalChannels = BDSQ_NUM_CHANNELS;
   for (i = 0; i < BDSQ_NUM_CHANNELS; i++)
      hDev->pChannels[i] = NULL;

   *h = hDev;
   BDBG_LEAVE(BDSQ_g1_P_Open);
   return retCode;
}


/******************************************************************************
 BDSQ_g1_P_Close()
******************************************************************************/
BERR_Code BDSQ_g1_P_Close(BDSQ_Handle h)
{
   BDBG_ENTER(BDSQ_g1_P_Close);

   /* free handles */
   BKNI_Free((void*)h->pChannels);
   BKNI_Free((void*)h->pImpl);
   BKNI_Free((void*)h);
   h = NULL;

   BDBG_LEAVE(BDSQ_g1_P_Close);
   return BERR_SUCCESS;
}


/******************************************************************************
 BDSQ_g1_P_GetTotalChannels()
******************************************************************************/
BERR_Code BDSQ_g1_P_GetTotalChannels(BDSQ_Handle h, uint32_t *totalChannels)
{
   BSTD_UNUSED(h);
   BDBG_ENTER(BDSQ_g1_P_GetTotalChannels);

   *totalChannels = BDSQ_NUM_CHANNELS;

   BDBG_LEAVE(BDSQ_g1_P_GetTotalChannels);
   return BERR_SUCCESS;
}


/******************************************************************************
 BDSQ_g1_P_InitChannelConfig()
******************************************************************************/
BERR_Code BDSQ_g1_P_InitChannelConfig(BDSQ_ChannelHandle h)
{
   BDSQ_g1_P_ChannelHandle *hChn = (BDSQ_g1_P_ChannelHandle *)h->pImpl;

   hChn->configParam[BDSQ_g1_CONFIG_RRTO_US] = 210000;            /* rrtoUsec */
   hChn->configParam[BDSQ_g1_CONFIG_BIT_THRESHOLD_US] = 723;      /* bitThreshold in us */
   hChn->configParam[BDSQ_g1_CONFIG_TONE_THRESHOLD] = 110;        /* toneThreshold 687.5mV * 0.16 */
   hChn->configParam[BDSQ_g1_CONFIG_PRETX_DELAY_MS] = 15;         /* preTxDelay in ms */
   hChn->configParam[BDSQ_g1_CONFIG_VSENSE_THRESHOLD_HI] = 0x35C; /* vsenseThresholdHi ~22V */
   hChn->configParam[BDSQ_g1_CONFIG_VSENSE_THRESHOLD_LO] = 0x17C; /* vsenseThresholdLo ~10V */
   hChn->configParam[BDSQ_g1_CONFIG_PGA_GAIN] = 0xD;              /* default pga gain 5.33 (14.5dB) */
   hChn->configParam[BDSQ_g1_CONFIG_ENABLE_LNBPU] = 0;            /* bEnableLNBPU= false */
   hChn->configParam[BDSQ_g1_CONFIG_RX_TONE_MODE] = 1;            /* default to differential mode */
   hChn->configParam[BDSQ_g1_CONFIG_IDLE_TIMEOUT_US] = 8000;      /* diseqc idle timeout in us */
   hChn->configParam[BDSQ_g1_CONFIG_RX_BIT_TIMING] = 0x320;       /* rx bit timinng in us */
   hChn->configParam[BDSQ_g1_CONFIG_CIC_LEN] = 0x03;              /* fast cic filter length */
   hChn->configParam[BDSQ_g1_CONFIG_CIC_MIN_THRESH] = 0x41;       /* fast cic min threshold */
   hChn->configParam[BDSQ_g1_CONFIG_CIC_DELTA_THRESH] = 0x00;     /* fast cic delta threshold */

   return BERR_SUCCESS;
}


/******************************************************************************
 BDSQ_g1_P_OpenChannel()
******************************************************************************/
BERR_Code BDSQ_g1_P_OpenChannel(BDSQ_Handle h, BDSQ_ChannelHandle *pChannelHandle, uint32_t chan, const BDSQ_ChannelSettings *pSettings)
{
   extern const uint32_t BDSQ_g1_ChannelIntrID[BDSQ_NUM_CHANNELS][BDSQ_g1_MaxIntID];

   BERR_Code retCode = BERR_SUCCESS;
   BDSQ_ChannelSettings chnSettings;
   BDSQ_ChannelHandle chnHandle;
   BDSQ_g1_P_ChannelHandle *chG1;
   BDSQ_g1_P_Handle *hDev = h->pImpl;

   BDBG_ENTER(BDSQ_g1_P_OpenChannel);

   /* set channel settings */
   if (pSettings == NULL)
      BDSQ_GetChannelDefaultSettings(h, chan, &chnSettings);
   else
      chnSettings = *pSettings;

   /* check channel index */
   if (chan >= BDSQ_NUM_CHANNELS)
      return BERR_INVALID_PARAMETER;

   /* allocate heap memory for the channel handle */
   chnHandle = (BDSQ_ChannelHandle)BKNI_Malloc(sizeof(BDSQ_P_ChannelHandle));
   BDBG_ASSERT(chnHandle);
   BKNI_Memset((void*)chnHandle, 0, sizeof(BDSQ_P_ChannelHandle));
   chG1 = (BDSQ_g1_P_ChannelHandle *)BKNI_Malloc(sizeof(BDSQ_g1_P_ChannelHandle));
   BDBG_ASSERT(chG1);
   BKNI_Memset((void*)chG1, 0, sizeof(BDSQ_g1_P_ChannelHandle));

   /* initialize channel handle */
   BKNI_Memcpy((void*)(&(chnHandle->settings)), (void*)&chnSettings, sizeof(BDSQ_ChannelSettings));
   chnHandle->pDevice = h;
   chnHandle->channel = chan;
   chnHandle->bEnabled = false;
   chG1->timer1Isr = NULL;
   chG1->timer2Isr = NULL;
   chG1->dsecDoneIsr = NULL;
   chG1->bVsenseEnabled = false;
   chnHandle->pImpl = (void*)chG1;
   BDSQ_g1_P_InitChannelConfig(chnHandle);

   /* create callbacks */
   retCode = BINT_CreateCallback(&(chG1->hTimer1Cb), hDev->hInterrupt, BDSQ_g1_ChannelIntrID[chan][BDSQ_g1_IntID_eTimer1],
                     BDSQ_g1_P_Timer1_isr, (void*)chnHandle, 0);
   BDBG_ASSERT(retCode == BERR_SUCCESS);

   retCode = BINT_CreateCallback(&(chG1->hTimer2Cb), hDev->hInterrupt, BDSQ_g1_ChannelIntrID[chan][BDSQ_g1_IntID_eTimer2],
                     BDSQ_g1_P_Timer2_isr, (void*)chnHandle, 0);
   BDBG_ASSERT(retCode == BERR_SUCCESS);

   retCode = BINT_CreateCallback(&(chG1->hDiseqcDoneCb), hDev->hInterrupt, BDSQ_g1_ChannelIntrID[chan][BDSQ_g1_IntID_eDiseqcDone],
                     BDSQ_g1_Txn_P_DiseqcDone_isr, (void*)chnHandle, 0);
   BDBG_ASSERT(retCode == BERR_SUCCESS);

   retCode = BINT_CreateCallback(&(chG1->hTxAlmostEmptyCb), hDev->hInterrupt, BDSQ_g1_ChannelIntrID[chan][BDSQ_g1_IntID_eTxAlmostEmpty],
                     NULL, (void*)chnHandle, 0);
   BDBG_ASSERT(retCode == BERR_SUCCESS);

   retCode = BINT_CreateCallback(&(chG1->hRxAlmostEmptyCb), hDev->hInterrupt, BDSQ_g1_ChannelIntrID[chan][BDSQ_g1_IntID_eRxAlmostEmpty],
                     NULL, (void*)chnHandle, 0);
   BDBG_ASSERT(retCode == BERR_SUCCESS);

   retCode = BINT_CreateCallback(&(chG1->hAcwDoneCb), hDev->hInterrupt, BDSQ_g1_ChannelIntrID[chan][BDSQ_g1_IntID_eAcwDone],
                     NULL, (void*)chnHandle, 0);
   BDBG_ASSERT(retCode == BERR_SUCCESS);

   retCode = BINT_CreateCallback(&(chG1->hUnderVoltageCb), hDev->hInterrupt, BDSQ_g1_ChannelIntrID[chan][BDSQ_g1_IntID_eUnderVoltage],
                     BDSQ_g1_P_UnderVoltage_isr, (void*)chnHandle, 0);
   BDBG_ASSERT(retCode == BERR_SUCCESS);

   retCode = BINT_CreateCallback(&(chG1->hOverVoltageCb), hDev->hInterrupt, BDSQ_g1_ChannelIntrID[chan][BDSQ_g1_IntID_eOverVoltage],
                     BDSQ_g1_P_OverVoltage_isr, (void*)chnHandle, 0);
   BDBG_ASSERT(retCode == BERR_SUCCESS);

   retCode = BINT_CreateCallback(&(chG1->hActivityCb), hDev->hInterrupt, BDSQ_g1_ChannelIntrID[chan][BDSQ_g1_IntID_eActivity],
                     BDSQ_g1_P_Activity_isr, (void*)chnHandle, 0);
   BDBG_ASSERT(retCode == BERR_SUCCESS);

   retCode = BINT_CreateCallback(&(chG1->hIdleTimeoutCb), hDev->hInterrupt, BDSQ_g1_ChannelIntrID[chan][BDSQ_g1_IntID_eIdleTimeout],
                     BDSQ_g1_P_IdleTimeout_isr, (void*)chnHandle, 0);
   BDBG_ASSERT(retCode == BERR_SUCCESS);

   /* create events */
   retCode = BKNI_CreateEvent(&(chG1->hTxEvent));
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   retCode = BKNI_CreateEvent(&(chG1->hRxEvent));
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   retCode = BKNI_CreateEvent(&(chG1->hVsenseEvent));
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   retCode = BKNI_CreateEvent(&(chG1->hActivityEvent));
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   retCode = BKNI_CreateEvent(&(chG1->hIdleEvent));
   BDBG_ASSERT(retCode == BERR_SUCCESS);

   h->pChannels[chan] = chnHandle;

   *pChannelHandle = chnHandle;
   BDBG_LEAVE(BDSQ_g1_P_OpenChannel);
   return retCode;
}


/******************************************************************************
 BDSQ_g1_P_CloseChannel()
******************************************************************************/
BERR_Code BDSQ_g1_P_CloseChannel(BDSQ_ChannelHandle h)
{
   BDSQ_g1_P_ChannelHandle *chG1;
   BERR_Code retCode = BERR_SUCCESS;

   BDBG_ENTER(BDSQ_g1_P_CloseChannel);

   /* clean up events */
   chG1 = (BDSQ_g1_P_ChannelHandle *)(h->pImpl);
   BKNI_DestroyEvent(chG1->hTxEvent);
   BKNI_DestroyEvent(chG1->hRxEvent);
   BKNI_DestroyEvent(chG1->hVsenseEvent);
   BKNI_DestroyEvent(chG1->hActivityEvent);
   BKNI_DestroyEvent(chG1->hIdleEvent);

   /* clean up callbacks */
   BINT_DestroyCallback(chG1->hTimer1Cb);
   BINT_DestroyCallback(chG1->hTimer2Cb);
   BINT_DestroyCallback(chG1->hDiseqcDoneCb);
   BINT_DestroyCallback(chG1->hTxAlmostEmptyCb);
   BINT_DestroyCallback(chG1->hRxAlmostEmptyCb);
   BINT_DestroyCallback(chG1->hAcwDoneCb);
   BINT_DestroyCallback(chG1->hUnderVoltageCb);
   BINT_DestroyCallback(chG1->hOverVoltageCb);

   /* free channel handle */
   BKNI_Free((void*)chG1);
   BKNI_Free((void*)h);
   h = NULL;

   BDBG_LEAVE(BDSQ_g1_P_CloseChannel);
   return retCode;
}


/******************************************************************************
 BDSQ_g1_P_GetDevice()
******************************************************************************/
BERR_Code BDSQ_g1_P_GetDevice(BDSQ_ChannelHandle h, BDSQ_Handle *pDev)
{
   BDBG_ENTER(BDSQ_g1_P_GetDevice);

   *pDev = h->pDevice;

   BDBG_LEAVE(BDSQ_g1_P_GetDevice);
   return BERR_SUCCESS;
}


/******************************************************************************
 BDSQ_g1_P_GetVersionInfo()
******************************************************************************/
BERR_Code BDSQ_g1_P_GetVersionInfo(BDSQ_Handle h, BFEC_VersionInfo *pVersion)
{
   BSTD_UNUSED(h);

   pVersion->majorVersion = BDSQ_API_VERSION;
   pVersion->minorVersion = BDSQ_G1_RELEASE_VERSION;
   pVersion->buildId = BDSQ_G1_BUILD_VERSION;
   pVersion->buildType = 0;
   return BERR_SUCCESS;
}


/******************************************************************************
 BDSQ_g1_P_ResetChannel()
******************************************************************************/
BERR_Code BDSQ_g1_P_ResetChannel(BDSQ_ChannelHandle h)
{
   BDSQ_g1_P_ChannelHandle *hChn = (BDSQ_g1_P_ChannelHandle *)(h->pImpl);
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t val;
   uint8_t i;

   static const uint32_t firCoeff[] =
   {
      0x0FFC0FF6, 0x0FF70FF8, 0x0FF90FFB, 0x0FFC0FFE, 0x00000002, 0x00030003, 0x00030001, 0x0FFF0FFC,
      0x0FF70FF2, 0x0FED0FE7, 0x0FE00FDA, 0x0FD50FD0, 0x0FCC0FCA, 0x0FCA0FCB, 0x0FCF0FD5, 0x0FDE0FE8,
      0x0FF40002, 0x00110020, 0x00300040, 0x004F005D, 0x00690073, 0x007A007F, 0x00800000, 0x00000158
   };

   BDBG_ENTER(BDSQ_g1_P_ResetChannel);

   /* initialize phy block */
   h->bEnabled = true;
   BDSQ_P_PowerUpDsecPhy(h);

   /* reset diseqc datapath */
   BDSQ_P_ToggleBit(h, BCHP_SDS_DSEC_DSRST, 0x00000001);

   /* program fir filter */
   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_DSCMEMADR, 0x00000000);
   for (i = 0; i < 24; i++)
      BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_DSCMEMDAT, firCoeff[i]);
   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_DSFIRCTL, 0x00000158);

   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_DSCTL00, 0x00091000);    /* release control word reset, release state machine reset */
   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_DSCTL01, 0x00000013);    /* set ddfs gain 630mV */
   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_DSCTL02, 0x1B810000);    /* set dac1 trim to 0V, power down diseqc rx by default, neg edge adc interface */
   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_DSCTL03, 0x00314060);    /* noise estimator decimate by 8, slow CIC, differential mode output */

   /* override rx tone mode */
   val = hChn->configParam[BDSQ_g1_CONFIG_RX_TONE_MODE] & 0x7;
   BDSQ_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DSCTL03, ~0x07070000, (val << 24) | (val << 16));

   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_RERT, 0x1770660D);       /* decrease vtop and vbot level for dac1 */
   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_TPWC, 0x3C210A15);       /* decrease tone absent timing */
   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_DSCT, 0x08000000);       /* misc diseqc controls */
   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_SLEW, 0x06060600);       /* adjust TOA to TOD timing */
   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_ADCTL, 0x02000000);      /* TBD required? */
   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_RTDC2, 0x00000704);      /* adjust noise_lpf_alpha and min_noise_int */

   /* set cic settings based on rx tone mode */
   if (hChn->configParam[BDSQ_g1_CONFIG_RX_TONE_MODE] == 3)
   {
      /* enable maximum threshold mode for non-confirm mode */
      BDSQ_P_OrRegister(h, BCHP_SDS_DSEC_DSCTL03, 0x00800000);
   }

   /* set fast and slow cic filter lengths, adjust fast cic delta and min threshold */
   val = (hChn->configParam[BDSQ_g1_CONFIG_CIC_LEN] & 0x3F) << 24;
   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_CICC, 0x001F4100 | val);    /* cic lpf alpha, max_thresh_ctl=0 */
   val = ((hChn->configParam[BDSQ_g1_CONFIG_CIC_DELTA_THRESH] & 0xFF) << 24) | ((hChn->configParam[BDSQ_g1_CONFIG_CIC_MIN_THRESH] & 0xFF) << 16);
   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_FCIC, 0x00000307 | val);    /* m/n majority vote for fast cic */

   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_SCIC, 0x00410307);       /* adjust slow cic */
   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_TCTL, 0x1FD40010);       /* set toa_conf_timeout */

   /* set rx low duty timing and bit timing */
   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_RBDT, 0x012C0000 | (hChn->configParam[BDSQ_g1_CONFIG_RX_BIT_TIMING] & 0xFFF));

   /* diseqc resets, toggle integrators, clear status */
   BDSQ_P_ToggleBit(h, BCHP_SDS_DSEC_DSCTL00, 0x7F006301);

   /* reset buffer status and diseqc status */
   hChn->bufStatus = BDSQ_BUF_ALL_EMPTY;
   hChn->dsecStatus.status = BDSQ_SendStatus_eSuccess;
   hChn->dsecStatus.bRxExpected = false;
   hChn->dsecStatus.nRxBytes = 0;

   /* update diseqc settings */
   retCode = BDSQ_g1_Txn_P_UpdateSettings(h);

   BDBG_LEAVE(BDSQ_g1_P_ResetChannel);
   return retCode;
}


/******************************************************************************
 BDSQ_g1_P_Reset()
******************************************************************************/
BERR_Code BDSQ_g1_P_Reset(BDSQ_Handle h)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint8_t i;

   BDBG_ENTER(BDSQ_g1_P_Reset);

   for (i = 0; i < h->totalChannels; i++)
   {
      if (h->pChannels[i] != NULL)
      {
         /* power down vsense and diseqc by default */
         BDSQ_g1_P_PowerDownVsense(h->pChannels[i]);
         BDSQ_g1_P_PowerDownChannel(h->pChannels[i]);
      }
   }

   BDBG_LEAVE(BDSQ_g1_P_Reset);
   return retCode;
}


/******************************************************************************
 BDSQ_g1_P_PowerDownChannel()
******************************************************************************/
BERR_Code BDSQ_g1_P_PowerDownChannel(BDSQ_ChannelHandle h)
{
   /* power off diseqc rx */
   BDSQ_P_OrRegister(h, BCHP_SDS_DSEC_DSCTL02, 0x00800000);

   /* power down rx phy */
   BDSQ_P_PowerDownDsecPhy(h);

   h->bEnabled = false;
   return BERR_SUCCESS;
}


/******************************************************************************
 BDSQ_g1_P_PowerUpChannel()
******************************************************************************/
BERR_Code BDSQ_g1_P_PowerUpChannel(BDSQ_ChannelHandle h)
{
   /* re-initialize diseqc */
   BDSQ_g1_P_ResetChannel(h);

   h->bEnabled = true;
   return BERR_SUCCESS;
}


/******************************************************************************
 BDSQ_g1_P_IsChannelOn()
******************************************************************************/
BERR_Code BDSQ_g1_P_IsChannelOn(BDSQ_ChannelHandle h, bool *pOn)
{
   BDBG_ENTER(BDSQ_g1_P_IsChannelOn);

   *pOn = h->bEnabled;

   BDBG_LEAVE(BDSQ_g1_P_IsChannelOn);
   return BERR_SUCCESS;
}


/******************************************************************************
 BDSQ_g1_P_PowerDownVsense()
******************************************************************************/
BERR_Code BDSQ_g1_P_PowerDownVsense(BDSQ_ChannelHandle h)
{
   BERR_Code retCode = BERR_SUCCESS;
   BDBG_ENTER(BDSQ_g1_P_PowerDownVsense);

   /* power down vsense phy */
   retCode = BDSQ_P_PowerDownVsensePhy(h);

   BDBG_LEAVE(BDSQ_g1_P_PowerDownVsense);
   return retCode;
}


/******************************************************************************
 BDSQ_g1_P_PowerUpVsense()
******************************************************************************/
BERR_Code BDSQ_g1_P_PowerUpVsense(BDSQ_ChannelHandle h)
{
   BERR_Code retCode = BERR_SUCCESS;
   BDBG_ENTER(BDSQ_g1_P_PowerUpVsense);

   /* power up vsense phy */
   retCode = BDSQ_P_PowerUpVsensePhy(h);

   BDBG_LEAVE(BDSQ_g1_P_PowerUpVsense);
   return retCode;
}


/******************************************************************************
 BDSQ_g1_P_IsVsenseOn()
******************************************************************************/
BERR_Code BDSQ_g1_P_IsVsenseOn(BDSQ_ChannelHandle h, bool *pOn)
{
   BDSQ_g1_P_ChannelHandle *hChn = (BDSQ_g1_P_ChannelHandle *)h->pImpl;

   BDBG_ENTER(BDSQ_g1_P_IsVsenseOn);

   *pOn = hChn->bVsenseEnabled;

   BDBG_LEAVE(BDSQ_g1_P_IsVsenseOn);
   return BERR_SUCCESS;
}


/******************************************************************************
 BDSQ_g1_P_SetTone()
******************************************************************************/
BERR_Code BDSQ_g1_P_SetTone(BDSQ_ChannelHandle h, bool bEnable)
{
   BERR_Code retCode = BERR_SUCCESS;
   BDSQ_g1_P_ChannelHandle *hChn = (BDSQ_g1_P_ChannelHandle *)h->pImpl;
   uint32_t val;

   if (!h->bEnabled)
      return BDSQ_ERR_POWERED_DOWN;

   BDBG_ENTER(BDSQ_g1_P_SetTone);
   hChn->bDiseqcToneOn = bEnable;

   BDSQ_P_ReadRegister(h, BCHP_SDS_DSEC_DSCTL00, &val);
   if (bEnable)
      val |= 0x30;    /* tone on */
   else
      val &= ~0x30;   /* tone off */
   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_DSCTL00, val);

   BDBG_LEAVE(BDSQ_g1_P_SetTone);
   return retCode;
}


/******************************************************************************
 BDSQ_g1_P_GetTone()
******************************************************************************/
BERR_Code BDSQ_g1_P_GetTone(BDSQ_ChannelHandle h, bool *pbTone)
{
   BERR_Code retCode = BERR_SUCCESS;
   BDSQ_g1_P_ChannelHandle *hChn = (BDSQ_g1_P_ChannelHandle *)h->pImpl;
   uint32_t adctl, rtdc2, dsctl3, val;

   *pbTone = 0;

   if (!h->bEnabled)
      return BDSQ_ERR_POWERED_DOWN;

   /* TBD */
   /* if (hChn->diseqc->diseqcStatus.status == BAST_DiseqcSendStatus_eBusy)
         return BAST_ERR_DISEQC_BUSY; */

   BDBG_ENTER(BDSQ_g1_P_GetTone);

   /* enable diseqc rx for tone detect */
   BDSQ_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DSCTL02, ~0x00800000, 0x00000000);

   /* configure adc for diseqc rx */
   BKNI_Sleep(45);   /* use bkni sleep because task level function */

   /* save registers we are going to modify */
   BDSQ_P_ReadRegister(h, BCHP_SDS_DSEC_ADCTL, &adctl);
   BDSQ_P_ReadRegister(h, BCHP_SDS_DSEC_RTDC2, &rtdc2);
   BDSQ_P_ReadRegister(h, BCHP_SDS_DSEC_DSCTL03, &dsctl3);

#ifdef BDSQ_TONE_DETECT_LEGACY
   /* direct amplitude comparsion method */
   BDSQ_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DSCTL03, ~0x00070000, 0x00080000);  /* legacy mode, use abs value of ADC output */
   BDSQ_P_OrRegister(h, BCHP_SDS_DSEC_DSFIRCTL, 0x00001000);   /* bypass FIR filter */

   /* set thresholds for 300mV */
   BDSQ_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DSCTL03, ~0x000000FF, 0x00000048);  /* (300mV)/(1060mV/2^8) = 72 = 0x48 */
   BDSQ_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DSCTL03, ~0x0000FF00, 0x00002400);  /* set this lower for hysterisis */

   BDSQ_P_ToggleBit(h, BCHP_SDS_DSEC_DSCTL01, 0x00008000);     /* clear tone status */
   BDSQ_P_OrRegister(h, BCHP_SDS_DSEC_DSCTL01, 0x00004000);    /* enable tone detection */

   /* reset integrators and diseqc */
   BDSQ_P_ToggleBit(h, BCHP_SDS_DSEC_DSCTL00, 0x7F000301);
   BKNI_Sleep(5);

   /* check if tone present */
   BDSQ_P_ReadRegister(h, BCHP_SDS_DSEC_DST2, &val);

   if (val & 0x01)
      *pbTone = true;
   else
      *pbTone = false;

   BDSQ_P_AndRegister(h, BCHP_SDS_DSEC_DSCTL01, ~0x00004000);     /* disable tone detection */
   BDSQ_P_ToggleBit(h, BCHP_SDS_DSEC_DSCTL01, 0x00008000);        /* clear tone status */
   BDSQ_P_AndRegister(h, BCHP_SDS_DSEC_DSFIRCTL, ~0x00001000);    /* re-enable FIR filter */

#else
   /* energy detect method */
   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_ADCTL, 0x06000000);      /* diseqc AFE PGA setting */
   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_RTDC2, 0x0000060F);      /* set lpf alpha, always integrate */
   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_DSCTL03, 0x003F4060);    /* noise estimator decimate by 8, slow CIC, soft demod mode for tone detect */
   BDSQ_P_OrRegister(h, BCHP_SDS_DSEC_DSCTL00, 0x7F000300);       /* diseqc resets, freeze integrators, clear status */
   BDSQ_P_AndRegister(h, BCHP_SDS_DSEC_DSCTL00, ~0x7F000300);     /* release resets, unfreeze integrators, unclear status */

   BKNI_Sleep(5);    /* use bkni sleep since task level function */

   BDSQ_P_ReadRegister(h, BCHP_SDS_DSEC_RTDC2, &val);
   val = val >> 23;
   if (val > hChn->configParam[BDSQ_g1_CONFIG_TONE_THRESHOLD])
      *pbTone = true;
   else
      *pbTone = false;
#endif

   /* restore registers */
   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_ADCTL, adctl);
   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_RTDC2, rtdc2);
   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_DSCTL03, dsctl3);

   /* reset diseqc and packet pointers */
   BDSQ_P_ToggleBit(h, BCHP_SDS_DSEC_DSCTL00, 0x00000300);

   /* disable diseqc rx after tone detect */
   BDSQ_P_OrRegister(h, BCHP_SDS_DSEC_DSCTL02, 0x00800000);

   BDBG_LEAVE(BDSQ_g1_P_GetTone);
   return retCode;
}


/******************************************************************************
 BDSQ_g1_P_SetVoltage()
******************************************************************************/
BERR_Code BDSQ_g1_P_SetVoltage(BDSQ_ChannelHandle h, bool bVtop)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t val;

   if (!h->bEnabled)
      return BDSQ_ERR_POWERED_DOWN;

   BDBG_ENTER(BDSQ_g1_P_SetVoltage);

   BDSQ_P_ReadRegister(h, BCHP_SDS_DSEC_DSCTL00, &val);
   if (bVtop)
      val |= 0x00100400;    /* vtop */
   else
      val &= ~0x00100400;   /* vbot */
   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_DSCTL00, val);

   BDBG_LEAVE(BDSQ_g1_P_SetVoltage);
   return retCode;
}


/******************************************************************************
 BDSQ_g1_P_GetVoltage()
******************************************************************************/
BERR_Code BDSQ_g1_P_GetVoltage(BDSQ_ChannelHandle h, uint16_t *pVoltage)
{
   BDSQ_g1_P_ChannelHandle *hChn = (BDSQ_g1_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;

   *pVoltage = 0;

   if (!hChn->bVsenseEnabled)
      return BDSQ_ERR_POWERED_DOWN;

   BDBG_ENTER(BDSQ_g1_P_GetVoltage);

   /* read sar adc */
   retCode = BDSQ_P_ReadVsenseAdc(h, pVoltage);

   BDBG_LEAVE(BDSQ_g1_P_GetVoltage);
   return retCode;
}


/******************************************************************************
 BDSQ_g1_P_EnableVsenseInterrupt()
******************************************************************************/
BERR_Code BDSQ_g1_P_EnableVsenseInterrupt(BDSQ_ChannelHandle h, bool bEnable)
{
   BDSQ_g1_P_ChannelHandle *hChn = (BDSQ_g1_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;

   BDBG_ENTER(BDSQ_g1_P_EnableVsenseInterrupt);

   if (bEnable)
   {
      /* enable status output */
      BDSQ_P_OrRegister(h, BCHP_SDS_DSEC_DS_SAR_THRSH, 0x00010000);

      retCode = BINT_ClearCallback(hChn->hOverVoltageCb);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      retCode = BINT_ClearCallback(hChn->hUnderVoltageCb);
      BDBG_ASSERT(retCode == BERR_SUCCESS);

      retCode = BINT_EnableCallback(hChn->hOverVoltageCb);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      retCode = BINT_EnableCallback(hChn->hUnderVoltageCb);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
   }
   else
   {
      /* disable status output */
      BDSQ_P_AndRegister(h, BCHP_SDS_DSEC_DS_SAR_THRSH, ~0x00010000);

      retCode = BINT_DisableCallback(hChn->hOverVoltageCb);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      retCode = BINT_DisableCallback(hChn->hUnderVoltageCb);
      BDBG_ASSERT(retCode == BERR_SUCCESS);

      retCode = BINT_ClearCallback(hChn->hOverVoltageCb);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      retCode = BINT_ClearCallback(hChn->hUnderVoltageCb);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
   }

   BDBG_LEAVE(BDSQ_g1_P_EnableVsenseInterrupt);
   return BERR_SUCCESS;
}


/******************************************************************************
 BDSQ_g1_P_EnableActivityInterrupt()
******************************************************************************/
BERR_Code BDSQ_g1_P_EnableActivityInterrupt(BDSQ_ChannelHandle h, bool bEnable)
{
   BDSQ_g1_P_ChannelHandle *hChn = (BDSQ_g1_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;

   BDBG_ENTER(BDSQ_g1_P_EnableActivityInterrupt);

   if (bEnable)
   {
      retCode = BINT_ClearCallback(hChn->hActivityCb);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      retCode = BINT_EnableCallback(hChn->hActivityCb);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
   }
   else
   {
      retCode = BINT_DisableCallback(hChn->hActivityCb);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      retCode = BINT_ClearCallback(hChn->hActivityCb);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
   }

   BDBG_LEAVE(BDSQ_g1_P_EnableActivityInterrupt);
   return BERR_SUCCESS;
}


/******************************************************************************
 BDSQ_g1_P_EnableIdleInterrupt()
******************************************************************************/
BERR_Code BDSQ_g1_P_EnableIdleInterrupt(BDSQ_ChannelHandle h, bool bEnable)
{
   BDSQ_g1_P_ChannelHandle *hChn = (BDSQ_g1_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;

   BDBG_ENTER(BDSQ_g1_P_EnableIdleTimeoutInterrupt);

   /* clear idle timer flags */
   BDSQ_P_AndRegister(h, BCHP_SDS_DSEC_DS_IDLE_TIMEOUT_CTL, ~0x00300000);

   if (bEnable)
   {
      /* monitor tone present activities */
      BDSQ_P_OrRegister(h, BCHP_SDS_DSEC_DS_IDLE_TIMEOUT_CTL, 0x00800000);

      /* enable idle timer */
      BDSQ_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DS_IDLE_TIMEOUT_CTL, ~0x00300000, 0x00100000);

      retCode = BINT_ClearCallback(hChn->hIdleTimeoutCb);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      retCode = BINT_EnableCallback(hChn->hIdleTimeoutCb);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
   }
   else
   {
      retCode = BINT_DisableCallback(hChn->hIdleTimeoutCb);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      retCode = BINT_ClearCallback(hChn->hIdleTimeoutCb);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
   }

   BDBG_LEAVE(BDSQ_g1_P_EnableIdleTimeoutInterrupt);
   return BERR_SUCCESS;
}


/******************************************************************************
 BDSQ_g1_P_Write()
******************************************************************************/
BERR_Code BDSQ_g1_P_Write(BDSQ_ChannelHandle h, const uint8_t *pBuf, uint8_t n)
{
   BDSQ_g1_P_ChannelHandle *hChn = (BDSQ_g1_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
   uint8_t i;

   if (!h->bEnabled)
      return BDSQ_ERR_POWERED_DOWN;

   if (n > 128)
      return BERR_INVALID_PARAMETER;

   BDBG_ENTER(BDSQ_g1_P_Write);

   /* reset statuses */
   hChn->dsecStatus.status = BDSQ_SendStatus_eBusy;
   hChn->dsecStatus.bRxExpected = false;  /* by default don't expect reply */
   hChn->dsecStatus.posParityErr = 0;
   hChn->dsecStatus.nRxBytes = 0;

   /* update diseqc settings before transmit */
   retCode = BDSQ_g1_Txn_P_UpdateSettings(h);

   /* copy diseqc message to channel handle */
   hChn->txCount = 0;
   hChn->txLen = n;
   for (i = 0; i < n; i++)
      hChn->txBuf[i] = pBuf[i];

   /* delay since h/w does not insert idle time for tone off case */
   BKNI_EnterCriticalSection();
   if ((hChn->bDiseqcToneOn == false) && hChn->configParam[BDSQ_g1_CONFIG_PRETX_DELAY_MS])
      retCode = BDSQ_g1_P_EnableTimer(h, BDSQ_g1_TimerSelect_e1, hChn->configParam[BDSQ_g1_CONFIG_PRETX_DELAY_MS] * 1000, BDSQ_g1_Txn_P_Transmit);
   else
      retCode = BDSQ_g1_P_EnableTimer(h, BDSQ_g1_TimerSelect_e1, 50, BDSQ_g1_Txn_P_Transmit);
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BDSQ_g1_P_Write);
   return retCode;
}


/******************************************************************************
 BDSQ_g1_P_EnableRx()
******************************************************************************/
BERR_Code BDSQ_g1_P_EnableRx(BDSQ_ChannelHandle h, bool bEnable)
{
   BDSQ_g1_P_ChannelHandle *hChn = (BDSQ_g1_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;

   if (!h->bEnabled)
      return BDSQ_ERR_POWERED_DOWN;

   BDBG_ENTER(BDSQ_g1_P_EnableRx);

   /* rx-only mode */
   if (bEnable)
   {
      /* update diseqc settings before receive */
      retCode = BDSQ_g1_Txn_P_UpdateSettings(h);

      /* reset statuses */
      hChn->dsecStatus.status = BDSQ_SendStatus_eBusy;
      hChn->dsecStatus.bRxExpected = true;
      hChn->dsecStatus.posParityErr = 0;
      hChn->dsecStatus.nRxBytes = 0;

      /* expect reply */
      h->settings.bExpectReply = true;
      hChn->txLen = 0;  /* zero tx length */

      /* enter rx only mode */
      BKNI_EnterCriticalSection();
      retCode = BDSQ_g1_P_EnableTimer(h, BDSQ_g1_TimerSelect_e1, 50, BDSQ_g1_Txn_P_Transmit);
      BKNI_LeaveCriticalSection();
   }
   else
   {
      /* reset to exit rx only mode */
      BINT_ClearCallback_isr(hChn->hDiseqcDoneCb); /* clear and disable interrupt */
      BINT_DisableCallback_isr(hChn->hDiseqcDoneCb);
      hChn->dsecStatus.status = BDSQ_SendStatus_eSuccess;   /* clear diseqc busy flag */

      /* reset the FIFO, memory, noise integrator, etc. */
      BDSQ_P_ToggleBit(h, BCHP_SDS_DSEC_DSCTL00, 0x7F002201);

      /* power off diseqc rx */
      BDSQ_P_OrRegister(h, BCHP_SDS_DSEC_DSCTL02, 0x00800000);

      /* do not expect reply */
      h->settings.bExpectReply = false;
   }

   BDBG_LEAVE(BDSQ_g1_P_EnableRx);
   return retCode;
}


/******************************************************************************
 BDSQ_g1_P_Read()
******************************************************************************/
BERR_Code BDSQ_g1_P_Read(BDSQ_ChannelHandle h, uint8_t *pBuf, uint8_t nBufMax, uint8_t *n, uint8_t *nNotRead)
{
   BERR_Code retCode = BERR_SUCCESS;
   BDSQ_g1_P_ChannelHandle *hChn = (BDSQ_g1_P_ChannelHandle *)h->pImpl;
   *nNotRead = 0;
   *n = 0;

   BDBG_ENTER(BDSQ_g1_P_Read);

   /* case if nothing to read */
   if ((hChn->bufStatus & BDSQ_BUF_RX_NOT_EMPTY) == 0)
      return BERR_SUCCESS;

   if (hChn->dsecStatus.status == BDSQ_SendStatus_eRxParityError)
      retCode = BDSQ_ERR_RX_PARITY;
   else if (hChn->dsecStatus.status == BDSQ_SendStatus_eRxIncomplete)
      retCode = BDSQ_ERR_RX_INCOMPLETE;

   if (hChn->dsecStatus.nRxBytes > nBufMax)
   {
      *n = nBufMax;
      *nNotRead = hChn->dsecStatus.nRxBytes - nBufMax;
      BKNI_Memcpy(pBuf, &(hChn->rxBuf[hChn->rxIdx]), *n);

      /* prepare for subsequent read */
      hChn->rxIdx += *n;
      hChn->dsecStatus.nRxBytes -= *n;
   }
   else
   {
      *n = hChn->dsecStatus.nRxBytes;
      *nNotRead = 0;
      BKNI_Memcpy(pBuf, &(hChn->rxBuf[hChn->rxIdx]), *n);

      /* indicate rx buffer has been read */
      hChn->bufStatus &= ~BDSQ_BUF_RX_NOT_EMPTY;
   }

   BDBG_LEAVE(BDSQ_g1_P_Read);
   return retCode;
}


/******************************************************************************
 BDSQ_g1_P_SendACW()
******************************************************************************/
BERR_Code BDSQ_g1_P_SendACW(BDSQ_ChannelHandle h, uint8_t acw)
{
   BSTD_UNUSED(h);
   BSTD_UNUSED(acw);
   return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BDSQ_g1_P_GetStatus()
******************************************************************************/
BERR_Code BDSQ_g1_P_GetStatus(BDSQ_ChannelHandle h, BDSQ_Status *pStatus)
{
   BDSQ_g1_P_ChannelHandle *hChn = (BDSQ_g1_P_ChannelHandle *)h->pImpl;

   *pStatus = hChn->dsecStatus;
   return BERR_SUCCESS;
}


/******************************************************************************
 BDSQ_g1_P_GetActivityStatus()
******************************************************************************/
BERR_Code BDSQ_g1_P_GetActivityStatus(BDSQ_ChannelHandle h, BDSQ_ActivityStatus *pActivityStatus)
{
   uint32_t val;

   /* read tone detection bits */
   BDSQ_P_ReadRegister(h, BCHP_SDS_DSEC_DST2, &val);
   pActivityStatus->bToneDetected = (val & 0x1) ? true : false;
   pActivityStatus->bToneActive = (val & 0x4) ? true : false;

   return BERR_SUCCESS;
}


/******************************************************************************
 BDSQ_g1_P_ResetActivityStatus()
******************************************************************************/
BERR_Code BDSQ_g1_P_ResetActivityStatus(BDSQ_ChannelHandle h)
{
   /* reset tone detection bits */
   BDSQ_P_ToggleBit(h, BCHP_SDS_DSEC_DSCTL01, 0x00008000);

   return BERR_SUCCESS;
}


/******************************************************************************
 BDSQ_g1_P_SetChannelSettings()
******************************************************************************/
BERR_Code BDSQ_g1_P_SetChannelSettings(BDSQ_ChannelHandle h, BDSQ_ChannelSettings *pSettings)
{
   h->settings = *pSettings;

   /* apply diseqc settings */
   return BDSQ_g1_Txn_P_UpdateSettings(h);
}


/******************************************************************************
 BDSQ_g1_P_GetChannelSettings()
******************************************************************************/
BERR_Code BDSQ_g1_P_GetChannelSettings(BDSQ_ChannelHandle h, BDSQ_ChannelSettings *pSettings)
{
   *pSettings = h->settings;
   return BERR_SUCCESS;
}


/******************************************************************************
 BDSQ_g1_P_SetConfig()
******************************************************************************/
BERR_Code BDSQ_g1_P_SetConfig(BDSQ_Handle h, uint32_t addr, uint32_t val)
{
   BSTD_UNUSED(h);
   BSTD_UNUSED(addr);
   BSTD_UNUSED(val);
   return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BDSQ_g1_P_GetConfig()
******************************************************************************/
BERR_Code BDSQ_g1_P_GetConfig(BDSQ_Handle h, uint32_t addr, uint32_t *pVal)
{
   BSTD_UNUSED(h);
   BSTD_UNUSED(addr);
   BSTD_UNUSED(pVal);
   return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BDSQ_g1_P_SetChannelConfig()
******************************************************************************/
BERR_Code BDSQ_g1_P_SetChannelConfig(BDSQ_ChannelHandle h, uint32_t addr, uint32_t val)
{
   BDSQ_g1_P_ChannelHandle *hChn = (BDSQ_g1_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t tmp;

   BDBG_ASSERT(h);
   BDBG_ENTER(BDSQ_g1_P_SetChannelConfig);

   if (addr >= BDSQ_g1_CONFIG_MAX)
      retCode = BERR_INVALID_PARAMETER;

   switch (addr)
   {
      case BDSQ_g1_CONFIG_RRTO_US:
         val &= 0xFFFFF;
         BDSQ_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_RXRT, ~0x000FFFFF, val);
         break;
      case BDSQ_g1_CONFIG_VSENSE_THRESHOLD_HI:
         val &= 0x3FF;              /* 10-bit binary offset */
         tmp = val + 0x200;         /* convert to 2's comp */
         tmp = (tmp >> 2) & 0xFF;   /* convert to 8-bit value */
         BDSQ_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DS_SAR_THRSH, ~0x0000FF00, tmp << 8);
         break;
      case BDSQ_g1_CONFIG_VSENSE_THRESHOLD_LO:
         val &= 0x3FF;              /* 10-bit binary offset */
         tmp = val + 0x200;         /* convert to 2's comp */
         tmp = (tmp >> 2) & 0xFF;   /* convert to 8-bit value */
         BDSQ_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DS_SAR_THRSH, ~0x000000FF, tmp);
         break;
      case BDSQ_g1_CONFIG_ENABLE_LNBPU:
         val = val ? 1 : 0;
         break;
      case BDSQ_g1_CONFIG_RX_TONE_MODE:
         val &= 0x7;
         break;
      case BDSQ_g1_CONFIG_IDLE_TIMEOUT_US:
         val &= 0xFFFFF;
         BDSQ_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DS_IDLE_TIMEOUT_CTL, ~0x000FFFFF, val);
         break;
      case BDSQ_g1_CONFIG_RX_BIT_TIMING:
         val &= 0xFFF;
         BDSQ_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_RBDT, ~0x00000FFF, val);
         break;
      case BDSQ_g1_CONFIG_CIC_LEN:
         val &= 0x3F;
         BDSQ_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_CICC, ~0x3F000000, val << 24);
         break;
      case BDSQ_g1_CONFIG_CIC_MIN_THRESH:
         val &= 0xFF;
         BDSQ_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_FCIC, ~0x00FF0000, val << 16);
         break;
      case BDSQ_g1_CONFIG_CIC_DELTA_THRESH:
         val &= 0xFF;
         BDSQ_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_FCIC, ~0xFF000000, val << 24);
         break;
   }

   if (retCode == BERR_SUCCESS)
      hChn->configParam[addr] = val;

   BDBG_LEAVE(BSCS_g2_P_SetChannelConfig);
   return retCode;
}


/******************************************************************************
 BDSQ_g1_P_GetChannelConfig()
******************************************************************************/
BERR_Code BDSQ_g1_P_GetChannelConfig(BDSQ_ChannelHandle h, uint32_t addr, uint32_t *pVal)
{
   BDSQ_g1_P_ChannelHandle *hChn = (BDSQ_g1_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;

   BDBG_ENTER(BDSQ_g1_P_GetChannelConfig);

   if (addr >= BDSQ_g1_CONFIG_MAX)
      retCode = BERR_INVALID_PARAMETER;
   else
      *pVal = hChn->configParam[addr];

   BDBG_LEAVE(BDSQ_g1_P_GetChannelConfig);
   return retCode;
}


/******************************************************************************
 BDSQ_g1_P_GetTxEventHandle()
******************************************************************************/
BERR_Code BDSQ_g1_P_GetTxEventHandle(BDSQ_ChannelHandle h, BKNI_EventHandle *hEvent)
{
   BDBG_ENTER(BDSQ_g1_P_GetTxEventHandle);
   *hEvent = ((BDSQ_g1_P_ChannelHandle *)h->pImpl)->hTxEvent;
   BDBG_LEAVE(BDSQ_g1_P_GetTxEventHandle);

   return BERR_SUCCESS;
}


/******************************************************************************
 BDSQ_g1_P_GetRxEventHandle()
******************************************************************************/
BERR_Code BDSQ_g1_P_GetRxEventHandle(BDSQ_ChannelHandle h, BKNI_EventHandle *hEvent)
{
   BDBG_ENTER(BDSQ_g1_P_GetRxEventHandle);
   *hEvent = ((BDSQ_g1_P_ChannelHandle *)h->pImpl)->hRxEvent;
   BDBG_LEAVE(BDSQ_g1_P_GetRxEventHandle);

   return BERR_SUCCESS;
}


/******************************************************************************
 BDSQ_g1_P_GetVsenseEventHandle()
******************************************************************************/
BERR_Code BDSQ_g1_P_GetVsenseEventHandle(BDSQ_ChannelHandle h, BKNI_EventHandle *hVsenseEvent)
{
   BDBG_ENTER(BDSQ_g1_P_GetVsenseEventHandle);
   *hVsenseEvent = ((BDSQ_g1_P_ChannelHandle *)h->pImpl)->hVsenseEvent;
   BDBG_LEAVE(BDSQ_g1_P_GetVsenseEventHandle);

   return BERR_SUCCESS;
}


/******************************************************************************
 BDSQ_g1_P_GetActivityEventHandle()
******************************************************************************/
BERR_Code BDSQ_g1_P_GetActivityEventHandle(BDSQ_ChannelHandle h, BKNI_EventHandle *hActivityEvent)
{
   BDBG_ENTER(BDSQ_g1_P_GetActivityEventHandle);
   *hActivityEvent = ((BDSQ_g1_P_ChannelHandle *)h->pImpl)->hActivityEvent;
   BDBG_LEAVE(BDSQ_g1_P_GetActivityEventHandle);

   return BERR_SUCCESS;
}


/******************************************************************************
 BDSQ_g1_P_GetIdleEventHandle()
******************************************************************************/
BERR_Code BDSQ_g1_P_GetIdleEventHandle(BDSQ_ChannelHandle h, BKNI_EventHandle *hIdleEvent)
{
   BDBG_ENTER(BDSQ_g1_P_GetIdleEventHandle);
   *hIdleEvent = ((BDSQ_g1_P_ChannelHandle *)h->pImpl)->hIdleEvent;
   BDBG_LEAVE(BDSQ_g1_P_GetIdleEventHandle);

   return BERR_SUCCESS;
}


/******************************************************************************
 BDSQ_g1_P_DisableTimer() - ISR context
******************************************************************************/
BERR_Code BDSQ_g1_P_DisableTimer(BDSQ_ChannelHandle h, BDSQ_g1_TimerSelect t)
{
   BDSQ_g1_P_ChannelHandle *hChn = (BDSQ_g1_P_ChannelHandle *)h->pImpl;
   BINT_CallbackHandle hCb = NULL;

   switch (t)
   {
      case BDSQ_g1_TimerSelect_e1:
         hCb = hChn->hTimer1Cb;
         hChn->timer1Isr = NULL;
         break;

      case BDSQ_g1_TimerSelect_e2:
         hCb = hChn->hTimer2Cb;
         hChn->timer2Isr = NULL;
         break;

      default:
         BDBG_ERR(("invalid timer!"));
         return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   BINT_DisableCallback_isr(hCb);
   BINT_ClearCallback_isr(hCb);

   /* disable specified timer */
   BDSQ_P_AndRegister(h, BCHP_SDS_DSEC_DSTMRCTL, ~(1 << t));
   return BERR_SUCCESS;
}


/******************************************************************************
 BDSQ_g1_P_EnableTimer() - ISR context
******************************************************************************/
BERR_Code BDSQ_g1_P_EnableTimer(BDSQ_ChannelHandle h, BDSQ_g1_TimerSelect t, uint32_t delay_usec, BDSQ_FUNCT func)
{
   BDSQ_g1_P_ChannelHandle *hChn = (BDSQ_g1_P_ChannelHandle *)h->pImpl;
   BINT_CallbackHandle hCb = NULL;
   uint32_t reg;

   if (delay_usec == 0)
      return BERR_INVALID_PARAMETER;

   BDSQ_g1_P_DisableTimer(h, t);

   switch (t)
   {
      case BDSQ_g1_TimerSelect_e1:
         reg = BCHP_SDS_DSEC_DSGENTMR1;
         if (func)
         {
            hCb = hChn->hTimer1Cb;
            hChn->timer1Isr = func;
         }
         break;

      case BDSQ_g1_TimerSelect_e2:
         reg = BCHP_SDS_DSEC_DSGENTMR2;
         if (func)
         {
            hCb = hChn->hTimer2Cb;
            hChn->timer2Isr = func;
         }
         break;

      default:
         BDBG_ERR(("invalid timer!"));
         return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   if (func)
   {
      /* clear interrupt and enable */
      BINT_ClearCallback_isr(hCb);
      BINT_EnableCallback_isr(hCb);
   }

   /* configure and start specified timer */
   BDSQ_P_WriteRegister(h, reg, delay_usec);
   BDSQ_P_OrRegister(h, BCHP_SDS_DSEC_DSTMRCTL, (1 << t));
   return BERR_SUCCESS;
}


/******************************************************************************
 BDSQ_g1_P_Timer1_isr() - ISR context
******************************************************************************/
void BDSQ_g1_P_Timer1_isr(void *p, int param)
{
   BDSQ_ChannelHandle h = (BDSQ_ChannelHandle)p;
   BDSQ_g1_P_ChannelHandle *hChn = (BDSQ_g1_P_ChannelHandle *)h->pImpl;
   BDSQ_FUNCT funct;

   BSTD_UNUSED(param);

   /* BDBG_MSG(("in BDSQ_g1_P_Timer1_isr()")); */
   funct = hChn->timer1Isr;
   BDSQ_g1_P_DisableTimer(h, BDSQ_g1_TimerSelect_e1);
   if (funct)
      funct(h);
}


/******************************************************************************
 BDSQ_g1_P_Timer2_isr() - ISR context
******************************************************************************/
void BDSQ_g1_P_Timer2_isr(void *p, int param)
{
   BDSQ_ChannelHandle h = (BDSQ_ChannelHandle)p;
   BDSQ_g1_P_ChannelHandle *hChn = (BDSQ_g1_P_ChannelHandle *)h->pImpl;
   BDSQ_FUNCT funct;

   BSTD_UNUSED(param);

   /* BDBG_MSG(("in BDSQ_g1_P_Timer2_isr()")); */
   funct = hChn->timer2Isr;
   BDSQ_g1_P_DisableTimer(h, BDSQ_g1_TimerSelect_e2);
   if (funct)
      funct(h);
}


/******************************************************************************
 BDSQ_g1_P_UnderVoltage_isr()
******************************************************************************/
void BDSQ_g1_P_UnderVoltage_isr(void *p, int param)
{
   BDSQ_ChannelHandle h = (BDSQ_ChannelHandle)p;
   BDSQ_g1_P_ChannelHandle *hChn = (BDSQ_g1_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;

   BSTD_UNUSED(param);
   /* BDBG_MSG(("DSQ%d voltage BELOW threshold(%d)", h->channel, hChn->configParam[BDSQ_g1_CONFIG_VSENSE_THRESHOLD_LO])); */

   retCode = BINT_ClearCallback_isr(hChn->hUnderVoltageCb);
   BDBG_ASSERT(retCode == BERR_SUCCESS);

   BKNI_SetEvent(hChn->hVsenseEvent);
}


/******************************************************************************
 BDSQ_g1_P_OverVoltage_isr()
******************************************************************************/
void BDSQ_g1_P_OverVoltage_isr(void *p, int param)
{
   BDSQ_ChannelHandle h = (BDSQ_ChannelHandle)p;
   BDSQ_g1_P_ChannelHandle *hChn = (BDSQ_g1_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;

   BSTD_UNUSED(param);
   /* BDBG_MSG(("DSQ%d voltage ABOVE threshold(%d)", h->channel, hChn->configParam[BDSQ_g1_CONFIG_VSENSE_THRESHOLD_HI])); */

   retCode = BINT_ClearCallback_isr(hChn->hOverVoltageCb);
   BDBG_ASSERT(retCode == BERR_SUCCESS);

   BKNI_SetEvent(hChn->hVsenseEvent);
}


/******************************************************************************
 BDSQ_g1_P_Activity_isr()
******************************************************************************/
void BDSQ_g1_P_Activity_isr(void *p, int param)
{
   BDSQ_ChannelHandle h = (BDSQ_ChannelHandle)p;
   BDSQ_g1_P_ChannelHandle *hChn = (BDSQ_g1_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;

   BSTD_UNUSED(param);
   /* BDBG_MSG(("in BDSQ_g1_P_Activity_isr()")); */

   retCode = BINT_DisableCallback(hChn->hActivityCb);
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   retCode = BINT_ClearCallback_isr(hChn->hActivityCb);
   BDBG_ASSERT(retCode == BERR_SUCCESS);

   BKNI_SetEvent(hChn->hActivityEvent);
}


/******************************************************************************
 BDSQ_g1_P_IdleTimeout_isr()
******************************************************************************/
void BDSQ_g1_P_IdleTimeout_isr(void *p, int param)
{
   BDSQ_ChannelHandle h = (BDSQ_ChannelHandle)p;
   BDSQ_g1_P_ChannelHandle *hChn = (BDSQ_g1_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;

   BSTD_UNUSED(param);
   /* BDBG_MSG(("in BDSQ_g1_P_IdleTimeout_isr()")); */

   /* clear idle timer flags */
   BDSQ_P_AndRegister(h, BCHP_SDS_DSEC_DS_IDLE_TIMEOUT_CTL, ~0x00300000);

   retCode = BINT_DisableCallback(hChn->hIdleTimeoutCb);
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   retCode = BINT_ClearCallback_isr(hChn->hIdleTimeoutCb);
   BDBG_ASSERT(retCode == BERR_SUCCESS);

   BKNI_SetEvent(hChn->hIdleEvent);
}
