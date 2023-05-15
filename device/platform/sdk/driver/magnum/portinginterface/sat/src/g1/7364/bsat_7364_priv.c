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
#include "bsat.h"
#include "bsat_priv.h"
#include "bsat_g1_priv.h"
#include "bsat_7364_priv.h"
#include "bchp_clkgen.h"
#include "bchp_stb_chan_ctrl.h"


BDBG_MODULE(bsat_7364_priv);

#define BSAT_DEBUG_POWERDOWN(x) /* x */


const uint32_t BSAT_g1_ChannelIntrID[BSAT_G1_MAX_CHANNELS][BSAT_g1_MaxIntID] =
{
   /* channel 0 interrupts */
   {
      BCHP_INT_ID_SDS_LOCK_0,
      BCHP_INT_ID_SDS_NOT_LOCK_0,
      BCHP_INT_ID_SDS_BTM_0,
      BCHP_INT_ID_SDS_BRTM_0,
      BCHP_INT_ID_SDS_GENTM1_0,
      BCHP_INT_ID_SDS_GENTM2_0,
      BCHP_INT_ID_SDS_GENTM3_0,
      BCHP_INT_ID_MI2C_0,
      BCHP_INT_ID_HP_FRAME_BOUNDARY_0,
      BCHP_INT_ID_SDS_HP_STATE_MATCH_0,
      BCHP_INT_ID_SDS_HP_STATE_CHANGE_0,
      BCHP_INT_ID_SDS_DFT_DONE_0,
      BCHP_INT_ID_AFEC_LOCK_0,
      BCHP_INT_ID_AFEC_NOT_LOCK_0,
      BCHP_INT_ID_AFEC_MP_LOCK_0,
      BCHP_INT_ID_AFEC_MP_NOT_LOCK_0,
      BCHP_INT_ID_TFEC_LOCK_0,
      BCHP_INT_ID_TFEC_NOT_LOCK_0,
      BCHP_INT_ID_TFEC_SYNC_0
   },
   /* channel 1 interrupts */
   {
      BCHP_INT_ID_SDS_LOCK_1,
      BCHP_INT_ID_SDS_NOT_LOCK_1,
      BCHP_INT_ID_SDS_BTM_1,
      BCHP_INT_ID_SDS_BRTM_1,
      BCHP_INT_ID_SDS_GENTM1_1,
      BCHP_INT_ID_SDS_GENTM2_1,
      BCHP_INT_ID_SDS_GENTM3_1,
      BCHP_INT_ID_MI2C_1,
      BCHP_INT_ID_HP_FRAME_BOUNDARY_1,
      BCHP_INT_ID_SDS_HP_STATE_MATCH_1,
      BCHP_INT_ID_SDS_HP_STATE_CHANGE_1,
      BCHP_INT_ID_SDS_DFT_DONE_1,
      BCHP_INT_ID_AFEC_LOCK_1,
      BCHP_INT_ID_AFEC_NOT_LOCK_1,
      BCHP_INT_ID_AFEC_MP_LOCK_1,
      BCHP_INT_ID_AFEC_MP_NOT_LOCK_1,
      BCHP_INT_ID_TFEC_LOCK_1,
      BCHP_INT_ID_TFEC_NOT_LOCK_1,
      BCHP_INT_ID_TFEC_SYNC_1
   }
};


/******************************************************************************
 BSAT_g1_P_GetTotalChannels()
******************************************************************************/
BERR_Code BSAT_g1_P_GetTotalChannels(BSAT_Handle h, uint32_t *totalChannels)
{
   BSTD_UNUSED(h);

   /* just do this for now */
   *totalChannels = BSAT_G1_MAX_CHANNELS;
   return BERR_SUCCESS;
}


/******************************************************************************
 BSAT_g1_P_GetRegisterAddress_isrsafe()
******************************************************************************/
void BSAT_g1_P_GetRegisterAddress_isrsafe(BSAT_ChannelHandle h, uint32_t reg, uint32_t *pAddr)
{
   *pAddr = reg;

   if (h->channel > 0)
   {
      if (((reg >= BCHP_SDS_CG_0_RSTCTL) && (reg <= BCHP_SDS_GR_BRIDGE_0_SW_INIT_1)) ||
          ((reg >= BCHP_TFEC_0_TFECTL) && (reg <= BCHP_TFEC_GR_BRIDGE_0_SW_INIT_1)))
      {
         /* SDS and TFEC register access */
         *pAddr += ((uint32_t)(h->channel) * 0x1000);
      }
      else if ((reg >= BCHP_STB_CHAN_CHx_DEC_FCW) && (reg <= BCHP_STB_CHAN_CHx_SW_SPARE1))
      {
         /* STB_CHAN_CHx register access */
         *pAddr += ((uint32_t)(h->channel) * 0x100);
      }
      else if ((reg >= BCHP_AFEC_RST) && (reg <= BCHP_AFEC_FAKEFRM_PARAM))
      {
         /* AFEC register access */
         if (h->channel & 1)
            *pAddr += 0x1000;
         *pAddr += ((uint32_t)(h->channel >> 1) * 0x8000);
      }
      else if ((reg >= BCHP_AFEC_INTR_CPU_STATUS) && (reg <= BCHP_AFEC_INTR_PCI_MASK_CLEAR))
      {
         /* AFEC_INTR register access */
         if (h->channel & 1)
            *pAddr += 0x400;
#if 0 /* not needed since there is only 1 dual AFEC */
         *pAddr += ((uint32_t)(h->channel >> 1) * 0x8000);
#endif
      }
#if 0 /* not needed since there is only 1 dual AFEC */
      else if (((reg >= BCHP_AFECNX_GLOBAL_CLK_CNTRL) && (reg <= BCHP_AFECNX_COMB_SMTH_FIFO_MAX)) ||
               ((reg >= BCHP_AFEC_GLOBAL_INTR_CPU_STATUS) && (reg <= BCHP_AFEC_GR_BRIDGE_SW_RESET_1)))
      {
         /* AFECNX_GLOBAL, AFECNX, AFEC_GLOBAL_INTR, AFEC_BR_BRIDGE register access */
         *pAddr += ((uint32_t)(h->channel >> 1) * 0x8000);
      }
#endif
   }
}


/******************************************************************************
 BSAT_g1_P_ReadRegister_isrsafe() - returns register value
******************************************************************************/
uint32_t BSAT_g1_P_ReadRegister_isrsafe(BSAT_ChannelHandle h, uint32_t reg)
{
   uint32_t addr;
   BSAT_g1_P_Handle *pDev = (BSAT_g1_P_Handle*)(h->pDevice->pImpl);

   BSAT_g1_P_GetRegisterAddress_isrsafe(h, reg, &addr);
/*BDBG_ERR(("read 0x%X", addr));*/
   return BREG_Read32(pDev->hRegister, addr);
}


/******************************************************************************
 BSAT_g1_P_WriteRegister_isrsafe()
******************************************************************************/
void BSAT_g1_P_WriteRegister_isrsafe(BSAT_ChannelHandle h, uint32_t reg, uint32_t val)
{
   uint32_t addr, wait_time;
   BSAT_g1_P_Handle *pDev = (BSAT_g1_P_Handle*)(h->pDevice->pImpl);

   BSAT_g1_P_GetRegisterWriteWaitTime_isrsafe(h, reg, &wait_time);
   BSAT_g1_P_GetRegisterAddress_isrsafe(h, reg, &addr);
   BREG_Write32(pDev->hRegister, addr, val);
/*BDBG_ERR(("write 0x%X=0x%X", addr, val));*/
   if (wait_time > 0)
      BKNI_Delay(wait_time);
}


/******************************************************************************
 BSAT_g1_P_InitHandleExt() - chip-specific initial settings in BSAT device handle
******************************************************************************/
BERR_Code BSAT_g1_P_InitHandleExt(BSAT_Handle h)
{
   BSAT_g1_P_Handle *hDev;

   hDev = (BSAT_g1_P_Handle *)(h->pImpl);
   hDev->xtalFreq = BSAT_G1_XTAL_FREQ;

   return BERR_SUCCESS;
}


/******************************************************************************
 BSAT_g1_P_InitChannelHandleExt() - chip-specific initial settings in BSAT
                                    channel device handle
******************************************************************************/
BERR_Code BSAT_g1_P_InitChannelHandleExt(BSAT_ChannelHandle h)
{
   BSAT_g1_P_ChannelHandle *hChn = (BSAT_g1_P_ChannelHandle *)h->pImpl;

   hChn->bHasAfec = true;
   hChn->bHasTfec = true;
   hChn->xportSettings.bOpllBypass = true;
   hChn->xportSettings.bSerial = false;
   return BERR_SUCCESS;
}


/******************************************************************************
 BSAT_g1_P_TunerPowerUp() - power up the channelizer
******************************************************************************/
BERR_Code BSAT_g1_P_TunerPowerUp(BSAT_ChannelHandle h)
{
   uint32_t mask = (1 << h->channel);

   BSAT_g1_P_AndRegister_isrsafe(h, BCHP_STB_CHAN_CTRL_PWRDN, ~mask);
   return BERR_SUCCESS;
}


/******************************************************************************
 BSAT_g1_P_TunerPowerDown() - power down the channelizer
******************************************************************************/
BERR_Code BSAT_g1_P_TunerPowerDown(BSAT_ChannelHandle h)
{
   BSAT_g1_P_OrRegister_isrsafe(h, BCHP_STB_CHAN_CTRL_PWRDN, (1 << h->channel));
   return BERR_SUCCESS;
}


/******************************************************************************
 BSAT_g1_P_IsSdsOn()
******************************************************************************/
bool BSAT_g1_P_IsSdsOn(BSAT_ChannelHandle h)
{
   BSAT_g1_P_Handle *pDev = (BSAT_g1_P_Handle*)(h->pDevice->pImpl);
   uint32_t val;

   val = BREG_Read32(pDev->hRegister, BCHP_CLKGEN_ONOFF_DUALSDS_INST_PLL_CLOCK);
   if (h->channel)
      val = val >> 1;
   return (val & 0x01) ? true : false;
}


/******************************************************************************
 BSAT_g1_P_SdsPowerUp() - power up the sds core
******************************************************************************/
BERR_Code BSAT_g1_P_SdsPowerUp(BSAT_ChannelHandle h)
{
   BSAT_g1_P_Handle *pDev = (BSAT_g1_P_Handle*)(h->pDevice->pImpl);
   uint32_t val, mask;

   val = BREG_Read32(pDev->hRegister, BCHP_CLKGEN_ONOFF_DUALSDS_INST_PLL_CLOCK);
   mask = h->channel ? 0x02 : 0x01;
   val |= mask;
   BREG_Write32(pDev->hRegister, BCHP_CLKGEN_ONOFF_DUALSDS_INST_PLL_CLOCK, val);
   return BSAT_g1_P_IsSdsOn(h) ? BERR_SUCCESS : BSAT_ERR_POWERUP_FAILED;
}


/******************************************************************************
 BSAT_g1_P_SdsPowerDown() - power down the sds core
******************************************************************************/
BERR_Code BSAT_g1_P_SdsPowerDown(BSAT_ChannelHandle h)
{
   BSAT_g1_P_Handle *pDev = (BSAT_g1_P_Handle*)(h->pDevice->pImpl);
   uint32_t val, mask;

   val = BREG_Read32(pDev->hRegister, BCHP_CLKGEN_ONOFF_DUALSDS_INST_PLL_CLOCK);
   mask = h->channel ? 0x02 : 0x01;
   val &= ~mask;
   BREG_Write32(pDev->hRegister, BCHP_CLKGEN_ONOFF_DUALSDS_INST_PLL_CLOCK, val);

   return BSAT_g1_P_IsSdsOn(h) ? BSAT_ERR_POWERUP_FAILED : BERR_SUCCESS;
}


/******************************************************************************
 BSAT_g1_P_GetAgcStatus()
 agc[0] = {bit 0: lna_agc valid}, {bit 1: chan_agc valid}
 agc[1] = lna_agc
 agc[2] = chan_agc
 agc[3] = (not used)

******************************************************************************/
BERR_Code BSAT_g1_P_GetAgcStatus(BSAT_ChannelHandle h, BSAT_AgcStatus *pStatus)
{
   pStatus->flags = 0x02; /* indicate channelizer AGC value is valid */
   pStatus->value[1] = BSAT_g1_P_ReadRegister_isrsafe(h, BCHP_STB_CHAN_CHx_AGC_LF_INT);
   return BERR_SUCCESS;
}


#ifndef BSAT_EXCLUDE_AFEC
/******************************************************************************
 BSAT_g1_P_IsAfecOn_isrsafe() - true if afec global is on
******************************************************************************/
bool BSAT_g1_P_IsAfecOn_isrsafe(BSAT_ChannelHandle h)
{
   BSAT_g1_P_Handle *pDev = (BSAT_g1_P_Handle*)(h->pDevice->pImpl);
   uint32_t val;

   val = BREG_Read32(pDev->hRegister, BCHP_CLKGEN_ONOFF_SDS_AFEC2X_TOP_INST_CLOCK_ENABLE);
   if ((val & 0x7) != 0x7)
      return false;

   val = BSAT_g1_P_ReadRegister_isrsafe(h, BCHP_AFECNX_GLOBAL_CLK_CNTRL);
   if (val & BCHP_AFECNX_GLOBAL_0_CLK_CNTRL_LDPC_CLK_ENABLEB_MASK)
      return false;

   /* afec global is enabled */
   return true;
}


/******************************************************************************
 BSAT_g1_P_AfecPowerUp_isr() - power up the afec core
******************************************************************************/
BERR_Code BSAT_g1_P_AfecPowerUp_isr(BSAT_ChannelHandle h)
{
   BSAT_g1_P_Handle *pDev = (BSAT_g1_P_Handle*)(h->pDevice->pImpl);

   if (BSAT_g1_P_IsAfecOn_isrsafe(h) == false)
   {
      BSAT_DEBUG_POWERDOWN(BDBG_MSG(("AFEC%d Power Up", h->channel)));

      /* turn on afec in clkgen */
      /* AFEC is currently powered down */
      /* turn on the 54/108/pll clocks in clkgen */
      BREG_Write32(pDev->hRegister, BCHP_CLKGEN_ONOFF_SDS_AFEC2X_TOP_INST_CLOCK_ENABLE, 0x7);

      BSAT_g1_P_AndRegister_isrsafe(h, BCHP_AFECNX_GLOBAL_CLK_CNTRL, ~BCHP_AFECNX_GLOBAL_0_CLK_CNTRL_LDPC_CLK_ENABLEB_MASK);
      BSAT_g1_P_WriteRegister_isrsafe(h, BCHP_AFECNX_GLOBAL_CFG_RST, 1); /* full datapath reset */
      BSAT_g1_P_WriteRegister_isrsafe(h, BCHP_AFECNX_GLOBAL_CFG_RST, 0);
      BSAT_g1_P_OrRegister_isrsafe(h, BCHP_AFECNX_GLOBAL_CONFIG, BCHP_AFECNX_0_GLOBAL_CONFIG_LDPC_ENA_MASK); /* enable LDPC decoder */
      BSAT_g1_P_OrRegister_isrsafe(h, BCHP_AFECNX_GLOBAL_RESET, BCHP_AFECNX_0_GLOBAL_RESET_AFECNX_DP_RST_MASK);
      BSAT_g1_P_WriteRegister_isrsafe(h, BCHP_AFEC_RST, 0);
      BSAT_g1_P_WriteRegister_isrsafe(h, BCHP_AFEC_GLOBAL_INTR_CPU_CLEAR, 0xFFFFFFFF);
      BSAT_g1_P_AndRegister_isrsafe(h, BCHP_AFECNX_GLOBAL_RESET, ~BCHP_AFECNX_0_GLOBAL_RESET_AFECNX_DP_RST_MASK);
   }

   return BERR_SUCCESS;
}


/******************************************************************************
 BSAT_g1_P_AfecPowerDown_isrsafe() - power down the afec global core
******************************************************************************/
BERR_Code BSAT_g1_P_AfecPowerDown_isrsafe(BSAT_ChannelHandle h)
{
   BSAT_g1_P_ChannelHandle *hChn = (BSAT_g1_P_ChannelHandle *)h->pImpl;
   BSAT_g1_P_Handle *pDev = (BSAT_g1_P_Handle*)(h->pDevice->pImpl);
   BSAT_ChannelHandle hOtherChan = BSAT_g1_P_AfecGetOtherChannelHandle_isrsafe(h);
   BSAT_g1_P_ChannelHandle *hOtherChanImpl;

   BSAT_DEBUG_POWERDOWN(BDBG_MSG(("AFEC%d Power Down", h->channel)));

   hChn->bAfecFlushFailed = false;

   if (hOtherChan)
   {
      hOtherChanImpl = (BSAT_g1_P_ChannelHandle *)(hOtherChan->pImpl);
      hOtherChanImpl->bAfecFlushFailed = false;
   }

   if (BSAT_g1_P_IsAfecOn_isrsafe(h) == false)
   {
      /* BDBG_MSG(("BSAT_g1_P_AfecPowerDown_isrsafe(%d): AFEC already powered down", h->channel)); */
      goto done;
   }

   /* first do a datapath reset */
   BSAT_g1_P_WriteRegister_isrsafe(h, BCHP_AFECNX_GLOBAL_CFG_RST, 1);
   BSAT_g1_P_WriteRegister_isrsafe(h, BCHP_AFECNX_GLOBAL_CFG_RST, 0);

   /* global power down */
   BSAT_g1_P_OrRegister_isrsafe(h, BCHP_AFECNX_GLOBAL_CLK_CNTRL, BCHP_AFECNX_GLOBAL_0_CLK_CNTRL_LDPC_CLK_ENABLEB_MASK);

   /* turn off afec in clkgen */
   /* turn off the 54/108/pll clocks in clkgen */
   BREG_Write32(pDev->hRegister, BCHP_CLKGEN_ONOFF_SDS_AFEC2X_TOP_INST_CLOCK_ENABLE, 0x0);

   done:
   return BERR_SUCCESS;
}


/******************************************************************************
 BSAT_g1_P_GetAfecClock_isrsafe()
******************************************************************************/
BERR_Code BSAT_g1_P_GetAfecClock_isrsafe(BSAT_ChannelHandle h, uint32_t *pFreq)
{
   BSAT_g1_P_Handle *hDevImpl = (BSAT_g1_P_Handle *)(h->pDevice->pImpl);
   uint32_t val = BREG_Read32(hDevImpl->hRegister, BCHP_CLKGEN_PLL_SYS0_PLL_DIV);
   uint32_t pdiv = (val >> 10) & 0x0F;
   uint32_t ndiv = (val & 0x3FF);
   uint32_t vco = (hDevImpl->xtalFreq / pdiv) * ndiv; /* default is 3.888GHz */
   uint32_t div = ((BREG_Read32(hDevImpl->hRegister, BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_4)) >> 1) & 0xFF;
   *pFreq = vco / div; /* should be 432MHz */
   return BERR_SUCCESS;
}
#endif /* BSAT_EXCLUDE_AFEC */


#ifndef BSAT_EXCLUDE_TFEC
/******************************************************************************
 BSAT_g1_P_TfecPowerUp_isr() - power up the tfec core
******************************************************************************/
BERR_Code BSAT_g1_P_TfecPowerUp_isr(BSAT_ChannelHandle h)
{
   BSAT_g1_P_Handle *pDev = (BSAT_g1_P_Handle*)(h->pDevice->pImpl);
   uint32_t val, mask;

   val = BREG_Read32(pDev->hRegister, BCHP_CLKGEN_ONOFF_DUALSDS_INST_PLL_CLOCK);
   mask = h->channel ? 0x08 : 0x04;
   val |= mask;
   BREG_Write32(pDev->hRegister, BCHP_CLKGEN_ONOFF_DUALSDS_INST_PLL_CLOCK, val);
   return BSAT_g1_P_IsTfecOn_isrsafe(h) ? BERR_SUCCESS : BSAT_ERR_POWERUP_FAILED;
}


/******************************************************************************
 BSAT_g1_P_TfecPowerDown_isrsafe() - power down the tfec core
******************************************************************************/
BERR_Code BSAT_g1_P_TfecPowerDown_isrsafe(BSAT_ChannelHandle h)
{
   BSAT_g1_P_Handle *pDev = (BSAT_g1_P_Handle*)(h->pDevice->pImpl);
   uint32_t val, mask;

   val = BREG_Read32(pDev->hRegister, BCHP_CLKGEN_ONOFF_DUALSDS_INST_PLL_CLOCK);
   mask = h->channel ? 0x08 : 0x04;
   val &= ~mask;
   BREG_Write32(pDev->hRegister, BCHP_CLKGEN_ONOFF_DUALSDS_INST_PLL_CLOCK, val);
   return BSAT_g1_P_IsTfecOn_isrsafe(h) ? BSAT_ERR_POWERUP_FAILED : BERR_SUCCESS;
}


/******************************************************************************
 BSAT_g1_P_IsTfecOn_isrsafe()
******************************************************************************/
bool BSAT_g1_P_IsTfecOn_isrsafe(BSAT_ChannelHandle h)
{
   BSAT_g1_P_Handle *pDev = (BSAT_g1_P_Handle*)(h->pDevice->pImpl);
   uint32_t val;

   val = BREG_Read32(pDev->hRegister, BCHP_CLKGEN_ONOFF_DUALSDS_INST_PLL_CLOCK);
   val = val >> 2;
   if (h->channel)
      val = val >> 1;
   return (val & 0x01) ? true : false;
}


/******************************************************************************
 BSAT_g1_P_GetTfecClock_isrsafe()
******************************************************************************/
BERR_Code BSAT_g1_P_GetTfecClock_isrsafe(BSAT_ChannelHandle h, uint32_t *pFreq)
{
   BSAT_g1_P_Handle *hDevImpl = (BSAT_g1_P_Handle *)(h->pDevice->pImpl);
   uint32_t val = BREG_Read32(hDevImpl->hRegister, BCHP_CLKGEN_PLL_NETWORK_PLL_DIV);
   uint32_t pdiv = (val >> 10) & 0x0F;
   uint32_t vco = (hDevImpl->xtalFreq / pdiv) * (val & 0x3FF); /* default is 2250MHz */
   uint32_t div = ((BREG_Read32(hDevImpl->hRegister, BCHP_CLKGEN_PLL_NETWORK_PLL_CHANNEL_CTRL_CH_1)) >> 1) & 0xFF;
   *pFreq = vco / div; /* should be 250MHz */
   return BERR_SUCCESS;
}
#endif /* BSAT_EXCLUDE_TFEC */
