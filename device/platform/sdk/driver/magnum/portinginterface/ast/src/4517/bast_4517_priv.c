/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_4517_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/21 $
 * $brcm_Date: 10/16/12 9:20a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/4517/bast_4517_priv.c $
 * 
 * Hydra_Software_Devel/21   10/16/12 9:20a enavarro
 * SWSATFE-220: add BAST_GetVersionInfo()
 * 
 * Hydra_Software_Devel/20   7/24/12 10:05a enavarro
 * SWSATFE-140: use hirq/spare register definitions from bhab
 * 
 * Hydra_Software_Devel/19   7/17/12 4:27p enavarro
 * SWSATFE-140: changed tuning range for 4517-B0
 * 
 * Hydra_Software_Devel/18   7/2/12 2:27p enavarro
 * SWSATFE-140: add B0 support
 * 
 * Hydra_Software_Devel/17   2/1/12 5:03p enavarro
 * SWSATFE-140: wait 5 msecs after BAST_ResetChannel()
 * 
 * Hydra_Software_Devel/16   2/1/12 2:41p enavarro
 * SWSATFE-140: added BAST_4517_P_GetTunerLnaStatus()
 * 
 * Hydra_Software_Devel/15   1/17/12 3:20p enavarro
 * SWSATFE-140: got symbol rate scan to work
 * 
 * Hydra_Software_Devel/14   1/16/12 9:41a enavarro
 * SWSATFE-140: fixed bug in BAST_4517_P_EnableStatusInterrupts()
 * 
 * Hydra_Software_Devel/13   1/13/12 11:25a enavarro
 * SWSATFE-140: fixed pointer bug in BAST_4517_P_SetTunerFilter()
 * 
 * Hydra_Software_Devel/12   1/13/12 10:47a enavarro
 * SWSATFE-140: added G3 config params
 * 
 * Hydra_Software_Devel/11   1/11/12 4:02p ronchan
 * SWSATFE-140: add support for fsk send and receive
 * 
 * Hydra_Software_Devel/10   1/11/12 10:56a ronchan
 * SWSATFE-140: fixed diseqc done interrupt mask
 * 
 * Hydra_Software_Devel/9   1/9/12 4:34p enavarro
 * SWSATFE-140: added tuner freq range checking
 * 
 * Hydra_Software_Devel/8   1/9/12 10:21a enavarro
 * SWSATFE-140: added BAST_4517_P_GetFskEventHandle()
 * 
 * Hydra_Software_Devel/7   1/6/12 4:40p enavarro
 * SWSATFE-140: added implementation for BAST_TunerConfigLna(); fixed
 * input validation in BAST_4517_TuneAcquire()
 * 
 * Hydra_Software_Devel/6   12/14/11 3:42p enavarro
 * SWSATFE-140: fixed compiler warnings
 * 
 * Hydra_Software_Devel/5   11/3/11 9:49a enavarro
 * SWSATFE-140: implemented VERSION command
 * 
 * Hydra_Software_Devel/4   10/26/11 2:28p enavarro
 * SWSATFE-140: added tuner filer, agc, and peak scan functions
 * 
 * Hydra_Software_Devel/3   10/25/11 4:22p enavarro
 * SWSATFE-140: added tunerFilter and AGC to channel status; implemented
 * BAST_FreezeEq(), BAST_FreezeAgc(), and BAST_SendACW()
 * 
 * Hydra_Software_Devel/2   10/25/11 11:17a enavarro
 * SWSATFE-140: added stub functions
 * 
 * Hydra_Software_Devel/1   10/24/11 3:38p enavarro
 * SWSATFE-140: initial check-in
 *
 ***************************************************************************/
#include "bstd.h"
#include "bchp_hif.h"
#include "bchp_leap_host_irq.h"
#include "bchp_leap_ctrl.h"
#include "bchp_leap_hab_mem.h"
#include "bchp_leap_l2.h"
#include "bi2c.h"
#include "bkni_multi.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_4517_priv.h"
#include "bhab.h"
#include "bhab_4517.h"

#ifndef BAST_4517_VER
#error "BAST_4517_VER needs to be defined"
#endif

#if (BAST_4517_VER!=BCHP_VER_A0) && (BAST_4517_VER!=BCHP_VER_B0)
#error "unsupported version specified in BAST_4517_VER"
#endif


BDBG_MODULE(bast_4517_priv);

/* local functions */
BERR_Code BAST_4517_P_EnableLockInterrupt(BAST_ChannelHandle h, bool bEnable);
BERR_Code BAST_4517_P_EnableFskInterrupt(BAST_Handle h, bool bEnable);
void BAST_4517_P_GetRegisterAddress(BAST_ChannelHandle h, uint32_t reg, uint32_t *pAddr);
BERR_Code BAST_4517_P_InterruptCallback(void *pParm1, int parm2);


/******************************************************************************
 BAST_4517_P_Open()
******************************************************************************/
BERR_Code BAST_4517_P_Open(
   BAST_Handle *h,                   /* [out] BAST handle */
   BCHP_Handle hChip,                /* [in] chip handle */
   void        *pReg,                /* [in] pointer to register or i2c handle */
   BINT_Handle hInterrupt,           /* [in] Interrupt handle */   
   const BAST_Settings *pDefSettings /* [in] default settings */
)
{
   BERR_Code retCode;
   BAST_Handle hDev;
   BAST_4517_P_Handle *hImplDev;
   uint8_t i;
   BHAB_Handle hHab;

   BSTD_UNUSED(hInterrupt);
   BSTD_UNUSED(hChip);

   BDBG_ASSERT(pReg);
   hHab = (BHAB_Handle)pReg;

   /* allocate memory for the handle */
   hDev = (BAST_Handle)BKNI_Malloc(sizeof(BAST_P_Handle));
   BDBG_ASSERT(hDev);
   hImplDev = (BAST_4517_P_Handle *)BKNI_Malloc(sizeof(BAST_4517_P_Handle));
   BDBG_ASSERT(hImplDev);
   hDev->pImpl = (void*)hImplDev;

   hDev->pChannels = (BAST_P_ChannelHandle **)BKNI_Malloc(BAST_4517_MAX_CHANNELS * sizeof(BAST_P_ChannelHandle *));
   BDBG_ASSERT(hDev->pChannels);

   /* initialize our handle */
   hImplDev->hHab = (BHAB_Handle)pReg;
   BKNI_Memcpy((void*)(&(hDev->settings)), (void*)pDefSettings, sizeof(BAST_Settings));
   hDev->totalChannels = BAST_4517_MAX_CHANNELS;
   for (i = 0; i < hDev->totalChannels; i++)
      hDev->pChannels[i] = NULL;

   /* create events */
   retCode = BKNI_CreateEvent(&(hImplDev->hDiseqcEvent));
   if (retCode != BERR_SUCCESS)
      goto done_0;
   retCode = BKNI_CreateEvent(&(hImplDev->hDiseqcOverVoltageEvent));
   if (retCode != BERR_SUCCESS)
      goto done_1;
   retCode = BKNI_CreateEvent(&(hImplDev->hDiseqcUnderVoltageEvent));
   if (retCode != BERR_SUCCESS)
      goto done_2;
   retCode = BKNI_CreateEvent(&(hImplDev->hFskEvent));
   if (retCode != BERR_SUCCESS)
   {
      BKNI_DestroyEvent(hImplDev->hDiseqcUnderVoltageEvent);
      
      done_2:
      BKNI_DestroyEvent(hImplDev->hDiseqcOverVoltageEvent);
      
      done_1:
      BKNI_DestroyEvent(hImplDev->hDiseqcEvent);
      
      done_0:
      BKNI_Free((void*)hDev->pChannels);
      BKNI_Free((void*)hImplDev);
      BKNI_Free((void*)hDev);      
      
      *h = NULL;
      return retCode;
   }

   BHAB_InstallInterruptCallback(hHab, BHAB_DevId_eAST, BAST_4517_P_InterruptCallback, (void*)hDev, 0);

   *h = hDev;
   return retCode;
}


/******************************************************************************
 BAST_4517_P_Close()
******************************************************************************/
BERR_Code BAST_4517_P_Close(BAST_Handle h)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pImpl);
   uint32_t mask;
   BERR_Code retCode;

   BDBG_ASSERT(h);

   mask = 0xFFFFFFFF;
   BHAB_CHK_RETCODE(BHAB_WriteRegister(pDevImpl->hHab, BCHP_LEAP_HOST_IRQ_MASK_SET0, &mask));
   BHAB_CHK_RETCODE(BHAB_WriteRegister(pDevImpl->hHab, BCHP_LEAP_HOST_IRQ_MASK_SET1, &mask));

   retCode = BHAB_UnInstallInterruptCallback(pDevImpl->hHab, BHAB_DevId_eAST);      
   BKNI_DestroyEvent(((BAST_4517_P_Handle *)(h->pImpl))->hDiseqcEvent);
   BKNI_DestroyEvent(((BAST_4517_P_Handle *)(h->pImpl))->hDiseqcOverVoltageEvent);
   BKNI_DestroyEvent(((BAST_4517_P_Handle *)(h->pImpl))->hDiseqcUnderVoltageEvent);
   BKNI_DestroyEvent(((BAST_4517_P_Handle *)(h->pImpl))->hFskEvent);
   BKNI_Free((void*)h->pChannels);
   BKNI_Free((void*)h->pImpl);
   BKNI_Free((void*)h);
   h = NULL;

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_GetTotalChannels()
******************************************************************************/
BERR_Code BAST_4517_P_GetTotalChannels(
   BAST_Handle  h,             /* [in] BAST handle */
   uint32_t     *totalChannels /* [out] number of channels supported */
)
{
   BSTD_UNUSED(h);
   *totalChannels = BAST_4517_MAX_CHANNELS;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_4517_P_OpenChannel()
******************************************************************************/
BERR_Code BAST_4517_P_OpenChannel(
   BAST_Handle                 h,              /* [in] BAST handle */
   BAST_ChannelHandle         *pChannelHandle, /* [out] BAST channel handle */
   uint32_t                   chnNo,           /* [in] channel number */
   const BAST_ChannelSettings *pSettings       /* [in] channel settings */
)
{
   BERR_Code retCode;
   BAST_ChannelSettings cs;
   BAST_P_ChannelHandle *ch;
   BAST_4517_P_ChannelHandle *chImpl;

   BDBG_ASSERT(chnNo < BAST_4517_MAX_CHANNELS);

   if (pSettings == NULL)
      BAST_4517_GetChannelDefaultSettings(h, chnNo, &cs);
   else
      cs = *pSettings;

   /* allocate memory for the channel handle */
   ch = (BAST_P_ChannelHandle *)BKNI_Malloc(sizeof(BAST_P_ChannelHandle));
   BDBG_ASSERT(ch);
   BKNI_Memcpy((void*)(&ch->settings), (void*)&cs, sizeof(BAST_ChannelSettings)); 
   ch->channel = (uint8_t)chnNo;
   ch->pDevice = h;
   h->pChannels[chnNo] = ch;

   chImpl = (BAST_4517_P_ChannelHandle *)BKNI_Malloc(sizeof(BAST_4517_P_ChannelHandle));
   BDBG_ASSERT(chImpl);

   chImpl->lastMode = BAST_Mode_eUnknown;
   retCode = BKNI_CreateEvent(&(chImpl->hLockChangeEvent));
   if (retCode != BERR_SUCCESS)
      goto done_0;
   retCode = BKNI_CreateEvent(&(chImpl->hStatusEvent));
   if (retCode != BERR_SUCCESS)
      goto done_1;
   retCode = BKNI_CreateEvent(&(chImpl->hPeakScanEvent));
   if (retCode != BERR_SUCCESS)
   {
      BKNI_DestroyEvent(chImpl->hStatusEvent);

      done_1:
      BKNI_DestroyEvent(chImpl->hLockChangeEvent);

      done_0:
      BKNI_Free((void*)chImpl);
      BKNI_Free((void*)ch);

      *pChannelHandle = NULL;
      return retCode;
   }
   
   ch->pImpl = (void*)chImpl;
   *pChannelHandle = ch;

   return retCode;
}


/******************************************************************************
 BAST_4517_P_CloseChannel()
******************************************************************************/
BERR_Code BAST_4517_P_CloseChannel(
   BAST_ChannelHandle h  /* [in] BAST channel handle */
)
{
   BAST_4517_P_ChannelHandle *chImpl;

   chImpl = (BAST_4517_P_ChannelHandle *)(h->pImpl);
   BKNI_DestroyEvent(chImpl->hLockChangeEvent);
   BKNI_DestroyEvent(chImpl->hStatusEvent);
   BKNI_DestroyEvent(chImpl->hPeakScanEvent);
   BKNI_Free((void*)chImpl);
   BKNI_Free((void*)h);
   h = NULL;

   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_4517_P_GetDevice()
******************************************************************************/
BERR_Code BAST_4517_P_GetDevice(
   BAST_ChannelHandle h,  /* [in] BAST channel handle */
   BAST_Handle *pDev      /* [out] BAST handle */
)
{
   *pDev = h->pDevice;
   return BERR_SUCCESS; 
}


/******************************************************************************
 BAST_4517_P_InitAp()
******************************************************************************/
BERR_Code BAST_4517_P_InitAp(
   BAST_Handle h,            /* [in] BAST handle */
   const uint8_t *pHexImage  /* [in] pointer to BCM4517 firmware image */
)
{
   BAST_4517_P_Handle *pImpl = (BAST_4517_P_Handle *)(h->pImpl);
   
   return BHAB_InitAp(pImpl->hHab, pHexImage);
}


/******************************************************************************
 BAST_4517_P_GetApStatus()
******************************************************************************/
BERR_Code BAST_4517_P_GetApStatus(
   BAST_Handle h,           /* [in] BAST device handle */
   BAST_ApStatus *pStatus   /* [out] AP status */
)
{
   BAST_4517_P_Handle *pImpl = (BAST_4517_P_Handle *)(h->pImpl);
   return BHAB_GetApStatus(pImpl->hHab, pStatus);
}


/******************************************************************************
 BAST_4517_P_SoftReset()
******************************************************************************/
BERR_Code BAST_4517_P_SoftReset(
   BAST_Handle h           /* [in] BAST device handle */
)
{
   BAST_4517_P_Handle *pImpl = (BAST_4517_P_Handle *)(h->pImpl);
   BERR_Code retCode;
   uint8_t buf[3];
   
   buf[0] = 0x05;
   buf[1] = 0x00;
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pImpl->hHab, buf, 3, buf, 3, true, true, 3));

   BKNI_Sleep(10); /* wait for soft reset to complete */

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_GetApVersion()
******************************************************************************/
BERR_Code BAST_4517_P_GetApVersion(
   BAST_Handle h,          /* [in] BAST handle */
   uint16_t    *pChipId,   /* [out] BAST chip ID */
   uint8_t     *pChipVer,  /* [out] chip revision number */
   uint32_t    *pBondId,   /* [out] chip bonding option */
   uint8_t     *pApFwMajVer,   /* [out] AP firmware major version */
   uint8_t     *pApFwMinVer    /* [out] AP firmware minor version */
)
{
   BAST_4517_P_Handle *pImpl = (BAST_4517_P_Handle *)(h->pImpl);
   uint32_t familyId, chipId;
   BERR_Code retCode;
   uint16_t chipVer;
   uint8_t majVer, minVer;

   BAST_4517_CHK_RETCODE(BHAB_GetApVersion(pImpl->hHab, &familyId, &chipId, &chipVer, &majVer, &minVer));

   *pChipId = (uint16_t)chipId;
   *pChipVer = (uint8_t)chipVer;
   *pBondId = 0;
   *pApFwMajVer = majVer;
   *pApFwMinVer = minVer;

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_GetVersionInfo()
******************************************************************************/
BERR_Code BAST_4517_P_GetVersionInfo(BAST_Handle h, FEC_DeviceVersionInfo *pVersionInfo)
{
   pVersionInfo->majorVersion = BAST_API_VERSION;
   pVersionInfo->minorVersion = BAST_4517_RELEASE_VERSION;
   pVersionInfo->buildType = 0;
   pVersionInfo->buildId = 0;
   return BERR_SUCCESS;
}


/******************************************************************************
 BERR_Code BAST_4517_P_ConfigGpio(
******************************************************************************/
BERR_Code BAST_4517_P_ConfigGpio(
   BAST_Handle h,           /* [in] BAST handle */
   uint32_t    write_mask,  /* [in] selects which GPIO pins are output */
   uint32_t    read_mask    /* [in] selects which GPIO pins are input */
)
{
   BAST_4517_P_Handle *pImpl = (BAST_4517_P_Handle *)(h->pImpl);
   BERR_Code retCode;
   uint8_t buf[10];
   
   buf[0] = 0x23;
   buf[1] = (uint8_t)((write_mask >> 24) & 0xFF);
   buf[2] = (uint8_t)((write_mask >> 16) & 0xFF);
   buf[3] = (uint8_t)((write_mask >> 8) & 0xFF);
   buf[4] = (uint8_t)(write_mask & 0xFF);
   buf[5] = (uint8_t)((read_mask >> 24) & 0xFF);
   buf[6] = (uint8_t)((read_mask >> 16) & 0xFF);
   buf[7] = (uint8_t)((read_mask >> 8) & 0xFF);
   buf[8] = (uint8_t)(read_mask & 0xFF);
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pImpl->hHab, buf, 10, buf, 10, true, true, 10));

   done:
   return retCode;
}


/******************************************************************************
 BERR_Code BAST_4517_P_SetGpio(
******************************************************************************/
BERR_Code BAST_4517_P_SetGpio(
   BAST_Handle h,     /* [in] BAST handle */
   uint32_t    mask,  /* [in] selects which GPIO pins to set */
   uint32_t    state  /* [in] state of GPIO pins */
)
{
   BAST_4517_P_Handle *pImpl = (BAST_4517_P_Handle *)(h->pImpl);
   BERR_Code retCode;
   uint8_t buf[10];
   
   buf[0] = 0x24;
   buf[1] = (uint8_t)((mask >> 24) & 0xFF);
   buf[2] = (uint8_t)((mask >> 16) & 0xFF);
   buf[3] = (uint8_t)((mask >> 8) & 0xFF);
   buf[4] = (uint8_t)(mask & 0xFF);
   buf[5] = (uint8_t)((state >> 24) & 0xFF);
   buf[6] = (uint8_t)((state >> 16) & 0xFF);
   buf[7] = (uint8_t)((state >> 8) & 0xFF);
   buf[8] = (uint8_t)(state & 0xFF);
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pImpl->hHab, buf, 10, buf, 10, true, true, 10));

   done:
   return retCode;
}


/******************************************************************************
 BERR_Code BAST_4517_P_GetGpio(
******************************************************************************/
BERR_Code BAST_4517_P_GetGpio(
   BAST_Handle h,     /* [in] BAST handle */
   uint32_t    mask,  /* [in] selects which GPIO pins to read */
   uint32_t    *state  /* [out] state of GPIO pins */
)
{
   BAST_4517_P_Handle *pImpl = (BAST_4517_P_Handle *)(h->pImpl);
   BERR_Code retCode;
   uint8_t buf[10];
   
   buf[0] = 0x25;
   buf[1] = (uint8_t)((mask >> 24) & 0xFF);
   buf[2] = (uint8_t)((mask >> 16) & 0xFF);
   buf[3] = (uint8_t)((mask >> 8) & 0xFF);
   buf[4] = (uint8_t)(mask & 0xFF);
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pImpl->hHab, buf, 10, buf, 10, true, true, 10));

   *state = (buf[5] << 24) | (buf[6] << 16) | (buf[7] << 8) | buf[8];

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_TuneAcquire()
******************************************************************************/
BERR_Code BAST_4517_P_TuneAcquire(
   BAST_ChannelHandle h,            /* [in] BAST channel handle */
   const uint32_t     freq,         /* [in] RF tuner frequency in Hz */
   const BAST_AcqSettings *pParams  /* [in] acquisition parameters */
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BAST_4517_P_ChannelHandle *pImpl = (BAST_4517_P_ChannelHandle *)(h->pImpl);   
   BERR_Code retCode;
   BAST_Mode mode;
   uint8_t buf[20];

   if ((pParams->symbolRate < 256000UL) || (pParams->symbolRate > 45000000UL))
      return (BERR_TRACE(BERR_INVALID_PARAMETER));

#if (BAST_4517_VER==BCHP_VER_A0)
   if (freq < 478000000UL)
#else
   if (freq < 250000000UL)
#endif
      goto freq_out_of_range;

   if (h->channel == 0)
   {
      if (freq > 2150000000UL)
         goto freq_out_of_range;
   }
   else if (freq > 3000000000UL)
   {
      freq_out_of_range:
      BDBG_WRN(("frequency out of range"));
      return (BERR_TRACE(BERR_INVALID_PARAMETER));
   }

   mode = pParams->mode;
   if ((mode == BAST_Mode_eUnknown) ||
       ((mode > BAST_Mode_eDcii_4_5) && (mode < BAST_Mode_eTurbo_scan)) ||
       (mode > BAST_Mode_eBlindScan) ||
       ((mode > BAST_Mode_eTurbo_8psk_8_9) && (mode < BAST_Mode_eBlindScan)))
      return (BERR_TRACE(BERR_INVALID_PARAMETER));

   BAST_4517_CHK_RETCODE(BAST_4517_P_EnableLockInterrupt(h, false));

   buf[0] = 0x0A;
   buf[1] = h->channel; 
   buf[2] = (uint8_t)mode;
   buf[3] = (uint8_t)((freq >> 24) & 0xFF); 
   buf[4] = (uint8_t)((freq >> 16) & 0xFF); 
   buf[5] = (uint8_t)((freq >> 8) & 0xFF); 
   buf[6] = (uint8_t)(freq & 0xFF); 
   buf[7] = (uint8_t)((pParams->symbolRate >> 24) & 0xFF);
   buf[8] = (uint8_t)((pParams->symbolRate >> 16) & 0xFF);
   buf[9] = (uint8_t)((pParams->symbolRate >> 8) & 0xFF);
   buf[10] = (uint8_t)(pParams->symbolRate & 0xFF);
   buf[11] = (uint8_t)((pParams->carrierFreqOffset >> 24) & 0xFF); 
   buf[12] = (uint8_t)((pParams->carrierFreqOffset >> 16) & 0xFF); 
   buf[13] = (uint8_t)((pParams->carrierFreqOffset >> 8) & 0xFF); 
   buf[14] = (uint8_t)(pParams->carrierFreqOffset & 0xFF); 
   buf[15] = (uint8_t)((pParams->acq_ctl >> 24) & 0xFF);
   buf[16] = (uint8_t)((pParams->acq_ctl >> 16) & 0xFF);
   buf[17] = (uint8_t)((pParams->acq_ctl >> 8) & 0xFF);
   buf[18] = (uint8_t)(pParams->acq_ctl & 0xFF);
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 20, buf, 20, true, true, 20));

   pImpl->lastMode = mode;
   BAST_4517_CHK_RETCODE(BAST_4517_P_EnableLockInterrupt(h, true));

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_GetChannelStatus()
******************************************************************************/
BERR_Code BAST_4517_P_GetChannelStatus(
   BAST_ChannelHandle h,        /* [in] BAST handle */
   BAST_ChannelStatus *pStatus  /* [out] pointer to status structure */
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BAST_4517_P_ChannelHandle *pImpl = (BAST_4517_P_ChannelHandle *)(h->pImpl);
   BERR_Code retCode;
   BAST_Mode mode;
   bool bTurboStatus, bRetry = false;
   uint8_t buf[128];
   
   if (BAST_MODE_IS_TURBO(pImpl->lastMode))
   {
      /* get turbo status */
      get_turbo_status:
      bTurboStatus = true;
      buf[0] = 0x0D;
      buf[1] = h->channel;
      BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 87, buf, 87, true, true, 87));
      mode = (BAST_Mode)buf[3];
      if (BAST_MODE_IS_LEGACY_QPSK(mode) && !bRetry)
      {
         bRetry = true;
         goto get_qpsk_status;
      }
   }
   else
   {
      /* get qpsk status */
      get_qpsk_status:
      bTurboStatus = false;
      buf[0] = 0x0B;
      buf[1] = h->channel;
      BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 81, buf, 81, true, true, 81));
      mode = (BAST_Mode)buf[3];
      if (BAST_MODE_IS_TURBO(mode) && !bRetry)
      {
         bRetry = true;
         goto get_turbo_status;
      }
   }

   pStatus->bTunerLocked = (buf[2] & 0x01) ? true : false;
   pStatus->bDemodLocked = (buf[2] & 0x02) ? true : false;
   pStatus->bBertLocked = (buf[2] & 0x04) ? true : false;
   pStatus->mode = mode;
   pStatus->symbolRate = (buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7];
   pStatus->tunerFreq = (buf[8] << 24) | (buf[9] << 16) | (buf[10] << 8) | buf[11];
   pStatus->carrierOffset = (int32_t)((buf[12] << 24) | (buf[13] << 16) | (buf[14] << 8) | buf[15]);
   pStatus->carrierError = (int32_t)((buf[16] << 24) | (buf[17] << 16) | (buf[18] << 8) | buf[19]);
   pStatus->symbolRateError = (int32_t)((buf[20] << 24) | (buf[21] << 16) | (buf[22] << 8) | buf[23]);
   pStatus->snrEstimate = (buf[24] << 8) | buf[25];
   pStatus->mpegCount = (buf[26] << 24) | (buf[27] << 16) | (buf[28] << 8) | buf[29];
   pStatus->mpegErrors = (buf[30] << 24) | (buf[31] << 16) | (buf[32] << 8) | buf[33];
   pStatus->berErrors = (buf[34] << 24) | (buf[35] << 16) | (buf[36] << 8) | buf[37];
   pStatus->outputBitrate = (buf[38] << 24) | (buf[39] << 16) | (buf[40] << 8) | buf[41];
   pStatus->reacqCount = (buf[42] << 24) | (buf[43] << 16) | (buf[44] << 8) | buf[45];
   pStatus->sample_clock = (buf[46] << 24) | (buf[47] << 16) | (buf[48] << 8) | buf[49];
   pStatus->tunerFilter = (buf[50] << 24) | (buf[51] << 16) | (buf[52] << 8) | buf[53];
   pStatus->IFagc = (buf[58] << 24) | (buf[59] << 16) | (buf[60] << 8) | buf[61];
   pStatus->RFagc = (buf[54] << 24) | (buf[55] << 16) | (buf[56] << 8) | buf[57];
   pStatus->agf = (buf[62] << 24) | (buf[63] << 16) | (buf[64] << 8) | buf[65];

   if (bTurboStatus && (BAST_MODE_IS_TURBO(mode)))
   {
      pStatus->modeStatus.turbo.totalBlocks = (buf[66] << 24) | (buf[67] << 16) | (buf[68] << 8) | buf[69];
      pStatus->modeStatus.turbo.corrBlocks = (buf[70] << 24) | (buf[71] << 16) | (buf[72] << 8) | buf[73];
      pStatus->modeStatus.turbo.badBlocks = (buf[74] << 24) | (buf[75] << 16) | (buf[76] << 8) | buf[77];
      pStatus->modeStatus.turbo.hpstatus = (buf[78] << 24) | (buf[79] << 16) | (buf[80] << 8) | buf[81];
      pStatus->modeStatus.turbo.fec_clock = (buf[82] << 24) | (buf[83] << 16) | (buf[84] << 8) | buf[85];
   }
   else if (!bTurboStatus && (BAST_MODE_IS_LEGACY_QPSK(mode)))
   {
      pStatus->modeStatus.legacy.rsCorrCount = (buf[66] << 24) | (buf[67] << 16) | (buf[68] << 8) | buf[69];
      pStatus->modeStatus.legacy.rsUncorrCount = (buf[70] << 24) | (buf[71] << 16) | (buf[72] << 8) | buf[73];
      pStatus->modeStatus.legacy.preVitErrCount = (buf[74] << 24) | (buf[75] << 16) | (buf[76] << 8) | buf[77];
      pStatus->modeStatus.legacy.spinv = (BAST_Spinv)buf[78];
      pStatus->modeStatus.legacy.phase = (BAST_PhaseRotation)buf[79];
   }
   else
   {
      BKNI_Memset(&(pStatus->modeStatus.turbo), 0, sizeof(BAST_TurboStatus));
   }

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_GetLockStatus()
******************************************************************************/
BERR_Code BAST_4517_P_GetLockStatus(
   BAST_ChannelHandle h,  /* [in] BAST handle */
   bool *bLocked          /* [out] pointer to status structure */
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode;
   uint32_t spare0, mask;

   mask = 1 << h->channel; 
   BAST_4517_CHK_RETCODE(BHAB_ReadRegister(pDevImpl->hHab,  BCHP_LEAP_CTRL_SW_SPARE0, &spare0));
   if (spare0 & mask) 
      *bLocked = true;
   else
      *bLocked = false; 

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_ResetStatus()
******************************************************************************/
BERR_Code BAST_4517_P_ResetStatus(
   BAST_ChannelHandle h  /* [in] BAST channel handle */
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode;
   uint8_t buf[4];
   
   buf[0] = 0x0E;
   buf[1] = h->channel;
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 3, buf, 3, true, true, 3));

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_SetDiseqcTone()
******************************************************************************/
BERR_Code BAST_4517_P_SetDiseqcTone(
   BAST_ChannelHandle h,  /* [in] BAST channel handle */
   bool bTone             /* [in] true = tone on, false = tone off */
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode;
   uint8_t buf[3];
   
   buf[0] = 0x12;
   buf[1] = bTone ? 1 : 0;
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 3, buf, 3, true, true, 3));

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_GetDiseqcTone()
******************************************************************************/
BERR_Code BAST_4517_P_GetDiseqcTone(
   BAST_ChannelHandle h,  /* [in] BAST channel handle */
   bool *bTone            /* [out] true = tone present, false = tone absent */
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode;
   uint8_t buf[3];
   
   buf[0] = 0x13;
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 3, buf, 3, true, true, 3));

   *bTone = (buf[1] & 0x01) ? true : false;

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_SetDiseqcVoltage()
******************************************************************************/
BERR_Code BAST_4517_P_SetDiseqcVoltage(
   BAST_ChannelHandle h, /* [in] BAST channel handle */
   bool bVtop            /* [in] true = VTOP, false = VBOT */
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode;
   uint8_t buf[3];
   
   buf[0] = 0x14;
   buf[1] = bVtop ? 1 : 0;
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 3, buf, 3, true, true, 3));

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_GetDiseqcVoltage()
******************************************************************************/
BERR_Code BAST_4517_P_GetDiseqcVoltage(
   BAST_ChannelHandle h, /* [in] BAST channel handle */
   uint8_t *pVoltage     /* [out] voltage estimation */
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode;
   uint8_t buf[3];
   
   buf[0] = 0x15;
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 3, buf, 3, true, true, 3));

   *pVoltage = buf[1];
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_EnableVsenseInterrupts()
******************************************************************************/
BERR_Code BAST_4517_P_EnableVsenseInterrupts(
   BAST_ChannelHandle h, 
   bool bEnable
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode;
   uint32_t val32;
   uint8_t buf[3];
   
   val32 = BHAB_4517_HIRQ0_DISEQC_OVERVOLTAGE | BHAB_4517_HIRQ0_DISEQC_UNDERVOLTAGE; 
   BAST_4517_CHK_RETCODE(BHAB_WriteRegister(pDevImpl->hHab, BCHP_LEAP_HOST_IRQ_CLEAR0, &val32));  
   if (bEnable)
   {
      BAST_4517_CHK_RETCODE(BHAB_WriteRegister(pDevImpl->hHab, BCHP_LEAP_HOST_IRQ_MASK_CLEAR0, &val32));  
   }
   else
   {
      BAST_4517_CHK_RETCODE(BHAB_WriteRegister(pDevImpl->hHab, BCHP_LEAP_HOST_IRQ_MASK_SET0, &val32));  
   }

   /* enable/disable vsense interrupts in the LEAP */
   buf[0] = 0x1F;
   buf[1] = bEnable ? 1 : 0;
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 3, buf, 3, true, true, 3));

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_SendDiseqcCommand()
******************************************************************************/
BERR_Code BAST_4517_P_SendDiseqcCommand(
   BAST_ChannelHandle h,    /* [in] BAST channel handle */
   const uint8_t *pSendBuf, /* [in] contains data to send */
   uint8_t sendBufLen       /* [in] number of bytes to send */
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode = BERR_SUCCESS;
   uint8_t buf[128],i,len;
   uint32_t mask = BHAB_4517_HIRQ0_DISEQC_DONE;

   buf[0] = 0x16;
   len=sendBufLen+3;
   if(sendBufLen > 124)
   {
      retCode = BERR_INVALID_PARAMETER;
      goto done;
   }
   buf[1] = sendBufLen & 0xFF;
   for(i=0;i<sendBufLen;i++)
   {
      buf[2+i]=pSendBuf[i];
   }
   buf[len-1] = 0x00;
   
   /* set up diseqc irq */
   BAST_4517_CHK_RETCODE(BAST_4517_P_WriteRegister(h, BCHP_LEAP_HOST_IRQ_MASK_CLEAR0, &mask));     
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, len, buf, len, true, true, len));

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_SetDiseqcSettings()
******************************************************************************/
BERR_Code BAST_4517_P_SetDiseqcSettings(BAST_ChannelHandle h, BAST_DiseqcSettings *pSettings)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode=BERR_SUCCESS;
   uint8_t buf[128], len;
   
   buf[0] = 0x1B;
   buf[1] = 0x00;
   buf[1] |= pSettings->bEnvelope?0x01:0;
   buf[1] |= pSettings->bToneAlign?0x02:0;
   buf[1] |= pSettings->bDisableRRTO?0x04:0;
   buf[1] |= pSettings->bEnableToneburst?0x08:0;
   buf[1] |= pSettings->bToneburstB?0x10:0;
   buf[1] |= pSettings->bOverrideFraming?0x20:0;
   buf[1] |= pSettings->bExpectReply?0x40:0;
   buf[1] |= pSettings->bEnableLNBPU?0x80:0;
   buf[2] = 0x00;
   buf[2] |= pSettings->bDisableRxOnly?0x01:0;
   buf[3] = (uint8_t)((pSettings->rrtoUsec >> 24) & 0xFF);
   buf[4] = (uint8_t)((pSettings->rrtoUsec >> 16) & 0xFF);
   buf[5] = (uint8_t)((pSettings->rrtoUsec >> 8) & 0xFF);
   buf[6] = (uint8_t)((pSettings->rrtoUsec) & 0xFF);
   buf[7] = (uint8_t)((pSettings->bitThreshold >> 8) & 0xFF);
   buf[8] = (uint8_t)((pSettings->bitThreshold) & 0xFF);
   buf[9] = (uint8_t)((pSettings->toneThreshold) & 0xFF);
   buf[10] = (uint8_t)((pSettings->preTxDelay) & 0xFF);
   buf[11] = (uint8_t)((pSettings->vsenseThresholdHi) & 0xFF);
   buf[12] = (uint8_t)((pSettings->vsenseThresholdLo) & 0xFF);
   buf[13] = 0x00;
   len=14;

   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, len, buf, len, true, true, len));

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_GetDiseqcSettings()
******************************************************************************/
BERR_Code BAST_4517_P_GetDiseqcSettings(BAST_ChannelHandle h, BAST_DiseqcSettings *pSettings)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode = BERR_SUCCESS;
   uint8_t buf[128], len;
   
   buf[0] = 0x1C;
   len = 14;

   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, len, buf, len, true, true, len));

   pSettings->bEnvelope=(buf[1] & 0x01)?true:false;
   pSettings->bToneAlign=(buf[1] & 0x02)?true:false;
   pSettings->bDisableRRTO=(buf[1] & 0x04)?true:false;
   pSettings->bEnableToneburst=(buf[1] & 0x08)?true:false;
   pSettings->bToneburstB=(buf[1] & 0x18)?true:false;
   pSettings->bOverrideFraming=(buf[1] & 0x20)?true:false;
   pSettings->bExpectReply=(buf[1] & 0x60)?true:false;
   pSettings->bEnableLNBPU=(buf[1] & 0x80)?true:false;
   pSettings->bDisableRxOnly=(buf[2] & 0x01)?true:false;

   pSettings->rrtoUsec=0;
   pSettings->rrtoUsec|=buf[3];
   pSettings->rrtoUsec<<=8;
   pSettings->rrtoUsec|=buf[4];
   pSettings->rrtoUsec<<=8;
   pSettings->rrtoUsec|=buf[5];
   pSettings->rrtoUsec<<=8;
   pSettings->rrtoUsec|=buf[6];
   pSettings->bitThreshold=0;
   pSettings->bitThreshold|=buf[7];
   pSettings->bitThreshold<<=8;
   pSettings->bitThreshold|=buf[8];
   pSettings->toneThreshold=buf[9];
   pSettings->preTxDelay=buf[10];
   pSettings->vsenseThresholdHi=buf[11];
   pSettings->vsenseThresholdLo=buf[12];
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_GetDiseqcStatus()
******************************************************************************/
BERR_Code BAST_4517_P_GetDiseqcStatus(
   BAST_ChannelHandle h,       /* [in] BAST channel handle */
   BAST_DiseqcStatus  *pStatus /* [out] status of last transaction */
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode;
   uint8_t buf[32], i;

   /* send DISEQC_READ_RECEIVE_BUFFER */
   buf[0] = 0x17;
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 21, buf, 21, true, true, 21));

   pStatus->status = (BAST_DiseqcSendStatus)buf[1];
   pStatus->bReplyExpected = buf[2] ? true : false;
   pStatus->nReplyBytes = buf[3];
   if (pStatus->nReplyBytes > 16)
      pStatus->nReplyBytes = 16;
   for (i = 0; i < 16; i++)
      pStatus->replyBuffer[i] = buf[4+i];

   done:
   return retCode;
}


/******************************************************************************
 BAST_ResetDiseqc()
******************************************************************************/ 
BERR_Code BAST_4517_P_ResetDiseqc(
   BAST_ChannelHandle h,    /* [in] BAST channel handle */
   uint8_t options          /* [in] reset options */
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode;
   uint8_t buf[4];

   BSTD_UNUSED(options);

   buf[0] = 0x11;
   buf[1] = 0;
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 3, buf, 3, true, true, 3));

   done:
   return retCode;
}


/******************************************************************************
 BAST_ResetFsk()
******************************************************************************/ 
BERR_Code BAST_4517_P_ResetFsk(
   BAST_Handle h    /* [in] BAST channel handle */
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pImpl);
   BERR_Code retCode;
   uint8_t buf[2];

   buf[0] = 0x18;
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 2, buf, 2, true, true, 2));

   /* turn on fsk interrupts */
   BAST_4517_CHK_RETCODE(BAST_4517_P_EnableFskInterrupt(h, true));

   done:
   return retCode;
}


/******************************************************************************
 BERR_Code BAST_4517_P_ReadFsk()
******************************************************************************/
BERR_Code BAST_4517_P_ReadFsk(
   BAST_Handle h,  /* [in] BAST handle */
   uint8_t *pBuf,  /* [out] data read */
   uint8_t *n      /* [out] length of data read */
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pImpl);                             
   BERR_Code retCode;
   uint8_t buf[130], i, len, checksum;

   *n = 0;

   buf[0] = 0x19;
   buf[1] = 0x19;
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 2, buf, 2, true, false, 2));
   checksum = buf[0] + buf[1];
   len = buf[1] & 0x7F;

   if (len)
   {
      /* read the next len bytes in the HAB */
      BAST_4517_CHK_RETCODE(BHAB_ReadMemory(pDevImpl->hHab, BCHP_LEAP_HAB_MEM_WORDi_ARRAY_BASE+2, &buf[2], (uint32_t)(len+1)));

      for (i = 0; i < len; i++)
         checksum += buf[2+i];

      if (checksum != buf[2+len])
      {
         BDBG_ERR(("Bad HAB checksum!\n"));
         BERR_TRACE(retCode = BAST_ERR_HAB_CHECKSUM);
         goto done;
      }

      *n = len + 1;
      for (i = 0; i < *n; i++)
         pBuf[i] = buf[i+2]; 
   }

   done:
   return retCode;
}


/******************************************************************************
 BERR_Code BAST_4517_P_WriteFsk()
******************************************************************************/
BERR_Code BAST_4517_P_WriteFsk(
   BAST_Handle h,  /* [in] BAST handle */
   uint8_t *pBuf,  /* [in] data to write */
   uint8_t n       /* [in] length of data to write */
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pImpl);                               
   BERR_Code retCode;
   uint8_t buf[128], i, len;

#ifndef BAST_SWM_DEBUG
   /* Per DirecTV, dont allow debug options in production code */
   if (n == 4)
   {
      if ((pBuf[0] == 0x83) && (pBuf[1] == 0x04))
      {
         /* this is the Set Options local message */
         /* dont allow option bits 1, 3, 4, and 14 */
         if ((pBuf[2] & 0x40) || (pBuf[3] & 0x1A))
         {
            BDBG_ERR(("specified SWM option bit(s) are not allowed in production code"));
            return BERR_NOT_SUPPORTED;
         }
      }
      else if ((pBuf[0] == 0x83) && (pBuf[1] == 0x02))
      {
         /* this is a "Set Rx Bit Mask" message */
         BDBG_ERR(("Set Rx Bit Mask SWM command not allowed in production code"));
         return BERR_NOT_SUPPORTED;
      }
   }
#endif

   buf[0] = 0x1A;
   for (i = 0; i < n; i++)
   {
      /* BDBG_MSG(("WriteFsk(): 0x%02X", pBuf[i])); */
      buf[1+i] = pBuf[i];
   }
   len = (uint8_t)(2+n);
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, len, buf, len, true, true, len));

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_PowerDownFsk()
******************************************************************************/
BERR_Code BAST_4517_P_PowerDownFsk(
   BAST_Handle h  /* [in] BAST handle */
)
{
   /* TBD */
   BSTD_UNUSED(h);
   return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_4517_P_PowerUpFsk()
******************************************************************************/
BERR_Code BAST_4517_P_PowerUpFsk(
   BAST_Handle h  /* [in] BAST handle */
)
{
   /* TBD */
   BSTD_UNUSED(h);
   return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_4517_P_WriteMi2c()
******************************************************************************/ 
BERR_Code BAST_4517_P_WriteMi2c(
   BAST_ChannelHandle h, /* [in] BAST channel handle */
   uint8_t slave_addr,   /* [in] address of the i2c slave device */
   uint8_t *i2c_buf,     /* [in] specifies the data to transmit */
   uint8_t n             /* [in] number of bytes to transmit after the i2c slave address */
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode;
   uint8_t buf[128], i, len;
   
   if ((n == 0) || (n > 8))
      return BERR_INVALID_PARAMETER;

   buf[0] = 0x26;
   buf[1] = h->channel;
   buf[2] = 0;
   buf[3] = n;
   buf[4] = slave_addr & 0xFE;
   for (i = 0; i < n; i++)
      buf[5+i] = i2c_buf[i];
   len = (uint8_t)(6+n);
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, len, buf, len, true, true, len));

   switch (buf[2])
   {
      case 0:
         retCode = BERR_SUCCESS;
         break;

      case 1:
         retCode = BAST_ERR_MI2C_NO_ACK;
         break;

      case 2:
         retCode = BAST_ERR_MI2C_BUSY;
         break;

      default:
         retCode = BAST_ERR_HAB_FAIL;
         break;
   }

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_ReadMi2c()
******************************************************************************/
BERR_Code BAST_4517_P_ReadMi2c(
   BAST_ChannelHandle h, /* [in] BAST channel handle */
   uint8_t slave_addr,   /* [in] address of the i2c slave device */
   uint8_t *out_buf,     /* [in] specifies the data to transmit before the i2c restart condition */
   uint8_t out_n,        /* [in] number of bytes to transmit before the i2c restart condition not including the i2c slave address */
   uint8_t *in_buf,      /* [out] holds the data read */
   uint8_t in_n          /* [in] number of bytes to read after the i2c restart condition not including the i2c slave address */
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode;
   uint8_t buf[128], i, len;
   
   if ((out_n > 8) || (in_n > 8) || (in_n == 0))
      return BERR_INVALID_PARAMETER;

   buf[0] = 0x27;
   buf[1] = h->channel;
   buf[2] = 0;
   buf[3] = out_n;
   buf[4] = in_n;
   buf[5] = slave_addr & 0xFE;
   for (i = 0; i < out_n; i++)
      buf[6+i] = out_buf[i];

   len = 7 + out_n + in_n;
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, len, buf, len, true, true, len));

   for (i = 0; i < in_n; i++)
      in_buf[i] = buf[6+out_n+i];

   switch (buf[2])
   {
      case 0:
         retCode = BERR_SUCCESS;
         break;

      case 1:
         retCode = BAST_ERR_MI2C_NO_ACK;
         break;

      case 2:
         retCode = BAST_ERR_MI2C_BUSY;
         break;

      default:
         retCode = BAST_ERR_HAB_FAIL;
         break;
   }
   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_GetSoftDecisionBuf()
******************************************************************************/
BERR_Code BAST_4517_P_GetSoftDecisionBuf(
   BAST_ChannelHandle h,  /* [in] BAST channel handle */
   int16_t *pI,           /* [out] 15 I-values */
   int16_t *pQ            /* [out] 15 Q-values */
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode;
   uint32_t i;
   uint8_t buf[128];

   buf[0] = 0x20;
   buf[1] = h->channel;
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 63, buf, 63, true, true, 63));

   for (i = 0; i < 15; i++)
   {
      pI[i] = (int16_t)((buf[i*4+2] << 8) | buf[i*4+3]);
      pQ[i] = (int16_t)((buf[i*4+4] << 8) | buf[i*4+5]);
   }
   done:
   return retCode;
}


/******************************************************************************
 BERR_Code BAST_4517_P_FreezeEq()
******************************************************************************/
BERR_Code BAST_4517_P_FreezeEq(
   BAST_ChannelHandle h,  /* [in] BAST handle */
   bool bFreeze           /* [in] true = freeze, false = unfreeze */
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode;
   uint8_t buf[4];

   buf[0] = 0x2B;
   buf[1] = h->channel;
   buf[2] = bFreeze ? 1 : 0;
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 4, buf, 4, true, true, 4));

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_DeepSleep()
******************************************************************************/
BERR_Code BAST_4517_P_DeepSleep(
   BAST_Handle h  /* [in] BAST handle */
)
{
   /* TBD */
   BSTD_UNUSED(h);
   return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_4517_P_PowerDown()
******************************************************************************/
BERR_Code BAST_4517_P_PowerDown(
   BAST_ChannelHandle h,  /* [in] BAST handle */
   uint32_t options       /* [in] see BAST_CORE_* macros */
)
{
   /* TBD */
   BSTD_UNUSED(h);
   BSTD_UNUSED(options);
   return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_4517_P_PowerUp()
******************************************************************************/
BERR_Code BAST_4517_P_PowerUp(
   BAST_ChannelHandle h,  /* [in] BAST channel handle */
   uint32_t options       /* [in] see BAST_CORE_* macros */
)
{
   /* TBD */
   BSTD_UNUSED(h);
   BSTD_UNUSED(options);
   return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_4517_P_GetLockStateChangeEventHandle()
******************************************************************************/
BERR_Code BAST_4517_P_GetLockStateChangeEventHandle(
   BAST_ChannelHandle h,       /* [in] BAST channel handle */
   BKNI_EventHandle   *hEvent  /* [out] lock event handle */
)
{
   *hEvent = ((BAST_4517_P_ChannelHandle *)(h->pImpl))->hLockChangeEvent;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_4517_P_GetDiseqcEventHandle()
******************************************************************************/
BERR_Code BAST_4517_P_GetDiseqcEventHandle(
   BAST_ChannelHandle h,     /* [in] BAST channel handle */
   BKNI_EventHandle *hEvent  /* [out] lock event handle */
)
{
   *hEvent = ((BAST_4517_P_Handle *)(h->pDevice->pImpl))->hDiseqcEvent;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_4517_P_GetDiseqcVsenseEventHandle()
******************************************************************************/
BERR_Code BAST_4517_P_GetDiseqcVsenseEventHandles(
   BAST_ChannelHandle h, 
   BKNI_EventHandle *hDiseqcOverVoltageEvent, 
   BKNI_EventHandle *hDiseqcUnderVoltageEvent)
{
   *hDiseqcOverVoltageEvent = ((BAST_4517_P_Handle *)(h->pDevice->pImpl))->hDiseqcOverVoltageEvent;
   *hDiseqcUnderVoltageEvent = ((BAST_4517_P_Handle *)(h->pDevice->pImpl))->hDiseqcUnderVoltageEvent;   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_4517_P_GetFskEventHandle()
******************************************************************************/
BERR_Code BAST_4517_P_GetFskEventHandle(
   BAST_Handle h,            /* [in] BAST handle */
   BKNI_EventHandle *hEvent  /* [out] lock event handle */
)
{
   *hEvent = ((BAST_4517_P_Handle *)(h->pImpl))->hFskEvent;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_4517_P_ReadConfig()
******************************************************************************/
BERR_Code BAST_4517_P_ReadConfig(
   BAST_ChannelHandle h,   /* [in] BAST channel handle */
   uint16_t id,            /* [in] configuration parameter ID */
   uint8_t *p,             /* [out] buffer to hold the read data */
   uint8_t len             /* [in] number of bytes to read */
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode;
   uint8_t   hab[32], i, n;

   if ((len != 1) && (len != 2) && (len != 4))
   {
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   hab[0] = 0x06;
   hab[1] = h->channel;
   hab[2] = (uint8_t)((id >> 8) & 0xFF);
   hab[3] = (uint8_t)(id & 0xFF);
   hab[4] = len;
   n = (uint8_t)(6+len);
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, hab, n, hab, n, true, true, n));

   for (i = 0; i < len; i++)
      p[i] = hab[i+5];

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_WriteConfig()
******************************************************************************/
BERR_Code BAST_4517_P_WriteConfig(
   BAST_ChannelHandle h,   /* [in] BAST channel handle */
   uint16_t id,            /* [in] configuration parameter ID */
   uint8_t *p,             /* [in] data to write */
   uint8_t len             /* [in] number of bytes to write */
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode;
   uint8_t   i, hab[32], n;

   if ((len != 1) && (len != 2) && (len != 4))
   {
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   hab[0] = 0x07;
   hab[1] = h->channel;
   hab[2] = (uint8_t)((id >> 8) & 0xFF);
   hab[3] = (uint8_t)(id & 0xFF);
   hab[4] = len;
   for (i = 0; i < len; i++)
      hab[i+5] = p[i];

   n = (uint8_t)(6+len);
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, hab, n, hab, n, true, true, n));

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_AbortAcq()
******************************************************************************/
BERR_Code BAST_4517_P_AbortAcq(
   BAST_ChannelHandle h  /* [in] BAST channel handle */
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   uint8_t buf[4];

   buf[0] = 0x0F;
   buf[1] = h->channel;
   return BHAB_SendHabCommand(pDevImpl->hHab, buf, 3, buf, 3, true, true, 3);
}


/******************************************************************************
 BAST_4517_P_PeakScan()
******************************************************************************/
BERR_Code BAST_4517_P_PeakScan(
   BAST_ChannelHandle h,           /* [in] BAST channel handle */
   uint32_t           tunerFreq    /* [in] frequency in Hz */
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode;
   uint32_t mask, bit;
   uint8_t buf[8];
   
   bit = ((h->channel * 4) + 3);
   mask = 1 << bit;

   /* clear the peak scan irq */
   BAST_4517_CHK_RETCODE(BHAB_WriteRegister(pDevImpl->hHab, BCHP_LEAP_HOST_IRQ_CLEAR1, &mask));

   buf[0] = 0x31;
   buf[1] = h->channel;
   buf[2] = (uint8_t)((tunerFreq >> 24) & 0xFF);
   buf[3] = (uint8_t)((tunerFreq >> 16) & 0xFF);
   buf[4] = (uint8_t)((tunerFreq >> 8) & 0xFF); 
   buf[5] = (uint8_t)(tunerFreq & 0xFF);
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 7, buf, 7, true, true, 7));

   /* enable peak scan interrupts */
   BAST_4517_CHK_RETCODE(BHAB_WriteRegister(pDevImpl->hHab, BCHP_LEAP_HOST_IRQ_MASK_CLEAR1, &mask));

   /* NOTE: the peak_scan irq gets disabled in bhab_4517_priv.c::BHAB_4517_P_DecodeInterrupt() */

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_GetPeakScanStatus()
******************************************************************************/
BERR_Code BAST_4517_P_GetPeakScanStatus(
   BAST_ChannelHandle  h,          /* [in] BAST channel handle */
   BAST_PeakScanStatus *pStatus    /* [out] Diseqc event handle */
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode;
   uint8_t buf[24];
   
   buf[0] = 0x32;
   buf[1] = h->channel;
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 19, buf, 19, true, true, 19));

   pStatus->status = (uint32_t)((buf[2] << 24) | (buf[3] << 16) | (buf[4] << 8) | buf[5]);
   pStatus->tunerFreq = (uint32_t)((buf[6] << 24) | (buf[7] << 16) | (buf[8] << 8) | buf[9]);
   pStatus->out = (uint32_t)((buf[10] << 24) | (buf[11] << 16) | (buf[12] << 8) | buf[13]);
   pStatus->peakPower = (uint32_t)((buf[14] << 24) | (buf[15] << 16) | (buf[16] << 8) | buf[17]);

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_GetPeakScanEventHandle()
******************************************************************************/
BERR_Code BAST_4517_P_GetPeakScanEventHandle(
   BAST_ChannelHandle h,                /* [in] BAST channel handle */
   BKNI_EventHandle  *hPeakScanEvent    /* [out] Peak scan done event handle */
)
{
   *hPeakScanEvent = ((BAST_4517_P_ChannelHandle *)(h->pImpl))->hPeakScanEvent;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_4517_P_SetPeakScanSymbolRateRange()
******************************************************************************/
BERR_Code BAST_4517_P_SetPeakScanSymbolRateRange(BAST_ChannelHandle h, uint32_t minSymRate, uint32_t maxSymRate)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);

   BERR_Code retCode;
   uint8_t buf[12];
   
   buf[0] = 0x33;
   buf[1] = h->channel;
   buf[2] = (uint8_t)((minSymRate >> 24) & 0xFF);
   buf[3] = (uint8_t)((minSymRate >> 16) & 0xFF);
   buf[4] = (uint8_t)((minSymRate >> 8) & 0xFF); 
   buf[5] = (uint8_t)(minSymRate & 0xFF);
   buf[6] = (uint8_t)((maxSymRate >> 24) & 0xFF);
   buf[7] = (uint8_t)((maxSymRate >> 16) & 0xFF);
   buf[8] = (uint8_t)((maxSymRate >> 8) & 0xFF); 
   buf[9] = (uint8_t)(maxSymRate & 0xFF);
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 11, buf, 11, true, true, 11));

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_GetPeakScanSymbolRateRange()
******************************************************************************/
BERR_Code BAST_4517_P_GetPeakScanSymbolRateRange(BAST_ChannelHandle h, uint32_t *pMinSymRate, uint32_t *pMaxSymRate)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode;
   uint8_t buf[24];
   
   buf[0] = 0x34;
   buf[1] = h->channel;
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 11, buf, 11, true, true, 11));

   *pMinSymRate = (uint32_t)((buf[2] << 24) | (buf[3] << 16) | (buf[4] << 8) | buf[5]);
   *pMaxSymRate = (uint32_t)((buf[6] << 24) | (buf[7] << 16) | (buf[8] << 8) | buf[9]);

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_EnableStatusInterrupts()
******************************************************************************/
BERR_Code BAST_4517_P_EnableStatusInterrupts(
   BAST_ChannelHandle h, 
   bool bEnable
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode;
   uint32_t mask, bit;
   uint8_t buf[4];

   bit = ((h->channel * 4) + 1);
   mask = 1 << bit;

   BAST_4517_CHK_RETCODE(BHAB_WriteRegister(pDevImpl->hHab, BCHP_LEAP_HOST_IRQ_CLEAR1, &mask));

   /* tell the LEAP to call BAST_EnableStatusInterrupts() */
   buf[0] = 0x37;
   buf[1] = h->channel;
   buf[2] = bEnable ? 1 : 0;
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 4, buf, 4, true, true, 4));

   if (bEnable)
   {
      BAST_4517_CHK_RETCODE(BHAB_WriteRegister(pDevImpl->hHab, BCHP_LEAP_HOST_IRQ_MASK_CLEAR1, &mask));
   }
   else
   {
      BAST_4517_CHK_RETCODE(BHAB_WriteRegister(pDevImpl->hHab, BCHP_LEAP_HOST_IRQ_MASK_SET1, &mask));
   }

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_GetStatusEventHandle()
******************************************************************************/
BERR_Code BAST_4517_P_GetStatusEventHandle(
   BAST_ChannelHandle h,        /* [in] BAST channel handle */
   BKNI_EventHandle  *hEvent    /* [out] status event handle */
)
{
   *hEvent = ((BAST_4517_P_ChannelHandle *)(h->pImpl))->hStatusEvent;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_4517_P_ConfigBcm3445()
******************************************************************************/
BERR_Code BAST_4517_P_ConfigBcm3445(
   BAST_Handle h,                  /* [in] BAST device handle */
   BAST_Bcm3445Settings *pSettings /* [in] BCM3445 configuration settings */
)
{
   /* TBD */
   BSTD_UNUSED(h);
   BSTD_UNUSED(pSettings);
   return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_4517_P_MapBcm3445ToTuner()
******************************************************************************/
BERR_Code BAST_4517_P_MapBcm3445ToTuner(
   BAST_ChannelHandle        h,    /* [in] BAST channel handle */
   BAST_Mi2cChannel          mi2c, /* [in] specifies which BCM3445 */
   BAST_Bcm3445OutputChannel out   /* [in] BCM3445 output channel */
)
{
   /* TBD */
   BSTD_UNUSED(h);
   BSTD_UNUSED(mi2c);
   BSTD_UNUSED(out);
   return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_4517_P_GetBcm3445Status()
******************************************************************************/
BERR_Code BAST_4517_P_GetBcm3445Status(
   BAST_ChannelHandle h,       /* [in] BAST channel handle */
   BAST_Bcm3445Status *pStatus /* [out] BCM3445 status struct */
)
{
   /* TBD */
   BSTD_UNUSED(h);
   BSTD_UNUSED(pStatus);
   return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_4517_P_SetSearchRange()
******************************************************************************/
BERR_Code BAST_4517_P_SetSearchRange(BAST_Handle h, uint32_t searchRange)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pImpl);
   BERR_Code retCode;
   uint8_t buf[12];
   
   buf[0] = 0x07;
   buf[1] = 0;
   buf[2] = (uint8_t)((BAST_4517_CONFIG_SEARCH_RANGE >> 8) & 0xFF);
   buf[3] = (uint8_t)(BAST_4517_CONFIG_SEARCH_RANGE & 0xFF);
   buf[4] = BAST_4517_CONFIG_LEN_SEARCH_RANGE;
   buf[5] = (uint8_t)((searchRange >> 24) & 0xFF);
   buf[6] = (uint8_t)((searchRange >> 16) & 0xFF);
   buf[7] = (uint8_t)((searchRange >> 8) & 0xFF); 
   buf[8] = (uint8_t)(searchRange & 0xFF);
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 10, buf, 10, true, true, 10));

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_GetSearchRange()
******************************************************************************/
BERR_Code BAST_4517_P_GetSearchRange(BAST_Handle h, uint32_t *pSearchRange)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pImpl);
   BERR_Code retCode;
   uint8_t buf[12];

   *pSearchRange = 0;

   BKNI_Memset(buf, 0, 12);
   buf[0] = 0x06;
   buf[1] = 0;
   buf[2] = (uint8_t)((BAST_4517_CONFIG_SEARCH_RANGE >> 8) & 0xFF);
   buf[3] = (uint8_t)(BAST_4517_CONFIG_SEARCH_RANGE & 0xFF);
   buf[4] = BAST_4517_CONFIG_LEN_SEARCH_RANGE;
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 10, buf, 10, true, true, 10));

   *pSearchRange = (uint32_t)(buf[5] << 24) | (uint32_t)(buf[6] << 16) | (uint32_t)(buf[7] << 8) | (uint32_t)buf[8];

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_SetTunerFilter()
******************************************************************************/
BERR_Code BAST_4517_P_SetTunerFilter(BAST_ChannelHandle h, uint32_t cutoffHz)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode;
   uint8_t buf[8];
   
   buf[0] = 0x30;
   buf[1] = h->channel;
   buf[2] = (uint8_t)((cutoffHz >> 24) & 0xFF);
   buf[3] = (uint8_t)((cutoffHz >> 16) & 0xFF);
   buf[4] = (uint8_t)((cutoffHz >> 8) & 0xFF); 
   buf[5] = (uint8_t)(cutoffHz & 0xFF);
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 7, buf, 7, true, true, 7));

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_GetSignalDetectStatus()
******************************************************************************/
BERR_Code BAST_4517_P_GetSignalDetectStatus(BAST_ChannelHandle h, BAST_SignalDetectStatus *pStatus)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode;
   uint8_t buf[4];

   buf[0] = 0x38;
   buf[1] = h->channel;
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 4, buf, 4, true, true, 10));

   pStatus->bEnabled = (buf[2] & 1) ? true : false;
   pStatus->bDone = (buf[2] & 2) ? true : false;
   pStatus->bTimingLoopLocked = (buf[2] & 4) ? true : false;

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_ResetChannel()
******************************************************************************/
BERR_Code BAST_4517_P_ResetChannel(BAST_ChannelHandle h, bool bForceReset)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode;
   uint8_t buf[4];

   buf[0] = 0x1D;
   buf[1] = h->channel;
   buf[2] = bForceReset ? 1: 0;
   retCode = BHAB_SendHabCommand(pDevImpl->hHab, buf, 4, buf, 4, true, true, 4);
   if (retCode == BERR_SUCCESS)
      BKNI_Sleep(5); /* wait for channel to reset */
   return retCode;
}


/******************************************************************************
 BAST_4517_P_EnableDiseqcLnb()
******************************************************************************/
BERR_Code BAST_4517_P_EnableDiseqcLnb(BAST_ChannelHandle h, bool bEnable)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   uint8_t buf[4];

   buf[0] = 0x14;
   buf[1] = bEnable ? 1 : 0;
   return BHAB_SendHabCommand(pDevImpl->hHab, buf, 3, buf, 3, true, true, 3);
}


/******************************************************************************
 BAST_4517_P_SetOutputTransportSettings()
******************************************************************************/
BERR_Code BAST_4517_P_SetOutputTransportSettings(BAST_ChannelHandle h, BAST_OutputTransportSettings *pSettings)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode;
   uint8_t buf[16];
   
   buf[0] = 0x07;
   buf[1] = h->channel;
   buf[2] = (uint8_t)((BAST_4517_CONFIG_XPORT_CTL >> 8) & 0xFF);
   buf[3] = (uint8_t)(BAST_4517_CONFIG_XPORT_CTL & 0xFF);
   buf[4] = BAST_4517_CONFIG_LEN_XPORT_CTL;
   buf[5] = buf[6] = buf[7] = buf[8] = 0;

   if (pSettings->bSerial)
      buf[8] |= 0x01;      
   if (pSettings->bClkInv)
      buf[8] |= 0x02; 
   if (pSettings->bClkSup)
      buf[8] |= 0x04; 
   if (pSettings->bVldInv)
      buf[8] |= 0x08;
   if (pSettings->bSyncInv)
      buf[8] |= 0x10;
   if (pSettings->bErrInv)
      buf[8] |= 0x20;
   if (pSettings->bXbert)
      buf[8] |= 0x40;
   if (pSettings->bTei)
      buf[8] |= 0x80;
   if (pSettings->bDelay)
      buf[7] |= 0x01;
   if (pSettings->bSync1)
      buf[7] |= 0x02;
   if (pSettings->bHead4)
      buf[7] |= 0x04;
   if (pSettings->bDelHeader)
      buf[7] |= 0x08;     
   if (pSettings->bOpllBypass)
      buf[7] |= 0x10;    
   buf[6] = (uint8_t)pSettings->bchMpegErrorMode;
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 10, buf, 10, true, true, 10));

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_GetOutputTransportSettings()
******************************************************************************/
BERR_Code BAST_4517_P_GetOutputTransportSettings(BAST_ChannelHandle h, BAST_OutputTransportSettings *pSettings)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode;
   uint8_t buf[16];

   buf[0] = 0x06;
   buf[1] = h->channel;
   buf[2] = (uint8_t)((BAST_4517_CONFIG_XPORT_CTL >> 8) & 0xFF);
   buf[3] = (uint8_t)(BAST_4517_CONFIG_XPORT_CTL & 0xFF);
   buf[4] = BAST_4517_CONFIG_LEN_XPORT_CTL;
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 10, buf, 10, true, true, 10));

   if (buf[8] & 0x01)  
      pSettings->bSerial = true;
   else
      pSettings->bSerial = false;   
   if (buf[8] & 0x02) 
      pSettings->bClkInv = true; 
   else
      pSettings->bClkInv = false;
   if (buf[8] & 0x04)
      pSettings->bClkSup = true;
   else
      pSettings->bClkSup = false;   
   if (buf[8] & 0x08)
      pSettings->bVldInv = true;
   else
      pSettings->bVldInv = false;   
   if (buf[8] & 0x10)
      pSettings->bSyncInv = true;
   else
      pSettings->bSyncInv = false;   
   if (buf[8] & 0x20)
      pSettings->bErrInv = true;
   else
      pSettings->bErrInv = false;   
   if (buf[8] & 0x40)
      pSettings->bXbert = true;
   else
      pSettings->bXbert = false;   
   if (buf[8] & 0x80)
      pSettings->bTei = true;
   else
      pSettings->bTei = false;   
   if (buf[7] & 0x01)
      pSettings->bDelay = true;
   else
      pSettings->bDelay = false;   
   if (buf[7] & 0x02)
      pSettings->bSync1 = true;
   else
      pSettings->bSync1 = false;   
   if (buf[7] & 0x04)
      pSettings->bHead4 = true;
   else
      pSettings->bHead4 = false;   
   if (buf[7] & 0x08)
      pSettings->bDelHeader = true;    
   else
      pSettings->bDelHeader = false;   
   if (buf[7] & 0x10)
      pSettings->bOpllBypass = true;
   else
      pSettings->bOpllBypass = false; 

   buf[6] &= 0x03;
   pSettings->bchMpegErrorMode = (BAST_BchMpegErrorMode)buf[6];

   done:
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_4517_P_SetNetworkSpec()
******************************************************************************/
BERR_Code BAST_4517_P_SetNetworkSpec(BAST_Handle h, BAST_NetworkSpec s)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pImpl);
   BERR_Code retCode;
   uint8_t buf[8];
   
   buf[0] = 0x07;
   buf[1] = 0;
   buf[2] = (uint8_t)((BAST_4517_CONFIG_NETWORK_SPEC >> 8) & 0xFF);
   buf[3] = (uint8_t)(BAST_4517_CONFIG_NETWORK_SPEC & 0xFF);
   buf[4] = BAST_4517_CONFIG_LEN_NETWORK_SPEC;
   buf[5] = (uint8_t)s;
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 7, buf, 7, true, true, 7));

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_GetNetworkSpec()
******************************************************************************/
BERR_Code BAST_4517_P_GetNetworkSpec(BAST_Handle h, BAST_NetworkSpec *p)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pImpl);
   BERR_Code retCode;
   uint8_t buf[8];
   
   buf[0] = 0x06;
   buf[1] = 0;
   buf[2] = (uint8_t)((BAST_4517_CONFIG_NETWORK_SPEC >> 8) & 0xFF);
   buf[3] = (uint8_t)(BAST_4517_CONFIG_NETWORK_SPEC & 0xFF);
   buf[4] = BAST_4517_CONFIG_LEN_NETWORK_SPEC;
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 7, buf, 7, true, true, 7));

   *p = (BAST_NetworkSpec)buf[5];

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_EnableLockInterrupt()
******************************************************************************/ 
BERR_Code BAST_4517_P_EnableLockInterrupt(
   BAST_ChannelHandle h, /* [in] BAST channel handle */
   bool bEnable          /* [in] true = enable lock interrupts, false = disables lock interrupts */
)
{
   BAST_4517_P_Handle *pImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode;
   uint32_t mask, bit;

   bit = h->channel * 4;
   mask = 1 << bit;

   /* clear the interrupt status */
   BAST_4517_CHK_RETCODE(BHAB_WriteRegister(pImpl->hHab, BCHP_LEAP_HOST_IRQ_CLEAR1, &mask));

   if (bEnable)
   {
      BAST_4517_CHK_RETCODE(BHAB_WriteRegister(pImpl->hHab, BCHP_LEAP_HOST_IRQ_MASK_CLEAR1, &mask));
   }
   else
   {
      BAST_4517_CHK_RETCODE(BHAB_WriteRegister(pImpl->hHab, BCHP_LEAP_HOST_IRQ_MASK_SET1, &mask));
   }

   done:
   return retCode;   
}



/******************************************************************************
 BAST_4517_P_GetRegisterAddress()
******************************************************************************/
void BAST_4517_P_GetRegisterAddress(BAST_ChannelHandle h, uint32_t reg, uint32_t *pAddr)
{
   *pAddr = reg;
   
   if (h->channel > 0)
   {
      if (((reg >= 0x000A8000) && (reg < 0x000A8D00)) || ((reg >= 0x000AC000) && (reg < 0x000AC400)))
      {
         /* SDS or TFEC register access */
         *pAddr = reg + (uint32_t)((uint32_t)h->channel * 0x8000);
      }
   }
}


/******************************************************************************
 BAST_4517_P_ReadRegister()
******************************************************************************/
BERR_Code BAST_4517_P_ReadRegister(
   BAST_ChannelHandle h,     /* [in] BAST channel handle */
   uint32_t           reg,   /* [in] address of register to read */
   uint32_t           *val   /* [in] contains data that was read */
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   uint32_t addr;

   if (reg & 0x03)
      return BERR_INVALID_PARAMETER;
      
   BAST_4517_P_GetRegisterAddress(h, reg, &addr);
   return BHAB_ReadRegister(pDevImpl->hHab, addr, val);
}


/******************************************************************************
 BAST_4517_P_WriteRegister()
******************************************************************************/
BERR_Code BAST_4517_P_WriteRegister(
   BAST_ChannelHandle h,     /* [in] BAST channel handle */
   uint32_t           reg,   /* [in] address of register to read */
   uint32_t           *val   /* [in] contains data that was read */
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   uint32_t addr;

   if (reg & 0x03)
      return BERR_INVALID_PARAMETER;

   BAST_4517_P_GetRegisterAddress(h, reg, &addr);
   return BHAB_WriteRegister(pDevImpl->hHab, addr, val);
}


/******************************************************************************
 BAST_4517_P_InterruptCallback()
******************************************************************************/
BERR_Code BAST_4517_P_InterruptCallback(void *pParm1, int parm2)
{
   BHAB_4517_IrqStatus *pParams = (BHAB_4517_IrqStatus *)pParm1;
   BAST_Handle h = (BAST_Handle)(pParams->pParm1);
   BAST_4517_P_Handle *pImpl = (BAST_4517_P_Handle *)(h->pImpl);   
   uint32_t fstatus0 = pParams->status0;
   uint32_t fstatus1 = pParams->status1;
   uint32_t channel, val32;
   BAST_4517_P_ChannelHandle *pChn;

   BSTD_UNUSED(parm2);

   if (fstatus0)
   {
      if (fstatus0 & BHAB_4517_HIRQ0_DISEQC_DONE)
         BKNI_SetEvent(pImpl->hDiseqcEvent);
      
      if (fstatus0 & BHAB_4517_HIRQ0_DISEQC_OVERVOLTAGE)
         BKNI_SetEvent(pImpl->hDiseqcOverVoltageEvent);
      
      if (fstatus0 & BHAB_4517_HIRQ0_DISEQC_UNDERVOLTAGE)
         BKNI_SetEvent(pImpl->hDiseqcUnderVoltageEvent);
      
      if (fstatus0 & BHAB_4517_HIRQ0_FSK_MSG_READY)
         BKNI_SetEvent(pImpl->hFskEvent);
   }

   if (fstatus1)
   {
      for (channel = 0; channel < BAST_4517_MAX_CHANNELS; channel++)
      {
         pChn = (BAST_4517_P_ChannelHandle *)(h->pChannels[channel]->pImpl);
         val32 = (fstatus1 >> (channel*4)) & 0x0F;
         if (val32 & BHAB_4517_HIRQ1_CHAN0_LOCK_CHANGE) 
            BKNI_SetEvent(pChn->hLockChangeEvent);
         if (val32 & BHAB_4517_HIRQ1_CHAN0_STATUS_EVENT) 
            BKNI_SetEvent(pChn->hStatusEvent);
         if (val32 & BHAB_4517_HIRQ1_CHAN0_PEAK_SCAN_DONE) 
            BKNI_SetEvent(pChn->hPeakScanEvent);
      }
   }
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_4517_P_ReadAgc()
******************************************************************************/
BERR_Code BAST_4517_P_ReadAgc(
   BAST_ChannelHandle h,  /* [in] BAST handle */
   BAST_Agc agcSelect,    /* [in] AGC select */
   uint32_t *pAgc         /* [out] integrator value */
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pImpl);
   BERR_Code retCode;
   uint8_t buf[8];
   
   buf[0] = 0x2D;
   buf[1] = h->channel;
   buf[2] = (uint8_t)agcSelect;
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 8, buf, 8, true, true, 8));

   *pAgc = (uint32_t)((buf[3] << 24) | (buf[4] << 16) | (buf[5] << 8) | buf[6]);

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_WriteAgc()
******************************************************************************/
BERR_Code BAST_4517_P_WriteAgc(
   BAST_ChannelHandle h,  /* [in] BAST handle */
   BAST_Agc agcSelect,    /* [in] AGC select */
   uint32_t *pAgc         /* [in] integrator value */
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pImpl);
   BERR_Code retCode;
   uint8_t buf[8];
   
   buf[0] = 0x2E;
   buf[1] = h->channel;
   buf[2] = (uint8_t)agcSelect;
   buf[3] = (uint8_t)((*pAgc >> 24) & 0xFF);
   buf[4] = (uint8_t)((*pAgc >> 16) & 0xFF);
   buf[5] = (uint8_t)((*pAgc >> 8) & 0xFF);
   buf[6] = (uint8_t)(*pAgc & 0xFF);
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 8, buf, 8, true, true, 8));

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_FreezeAgc()
******************************************************************************/
BERR_Code BAST_4517_P_FreezeAgc(
   BAST_ChannelHandle h,  /* [in] BAST handle */
   BAST_Agc agcSelect,    /* [in] AGC select */
   bool bFreeze           /* [in] true = freeze, false = unfreeze */
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode;
   uint8_t buf[8];

   buf[0] = 0x2C;
   buf[1] = h->channel;
   buf[2] = (uint8_t)agcSelect;
   buf[3] = bFreeze ? 1 : 0;
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 5, buf, 5, true, true, 5));

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_EnableSpurCanceller()
******************************************************************************/
BERR_Code BAST_4517_P_EnableSpurCanceller(
   BAST_ChannelHandle h,             /* [in] BAST channel handle */
   uint8_t            n,             /* [in] number of spurs to cancel (i.e. size of pConfig array), maximum is 6 */
   BAST_SpurCancellerConfig *pConfig /* [in] array of spur settings */
)
{
   /* TBD */
   BSTD_UNUSED(h);
   BSTD_UNUSED(n);
   BSTD_UNUSED(pConfig);
   return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_4517_P_SendACW()
******************************************************************************/
BERR_Code BAST_4517_P_SendACW(
   BAST_ChannelHandle h, /* [in] BAST channel handle */
   uint8_t acw           /* [in] auto control word to send */
)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode;
   uint8_t buf[4];

   buf[0] = 0x2F;
   buf[1] = acw;
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 3, buf, 3, true, true, 3));

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_EnableFskInterrupt()
******************************************************************************/
BERR_Code BAST_4517_P_EnableFskInterrupt(
   BAST_Handle h, /* [in] BAST handle */
   bool bEnable   /* [in] true = enable FTM interrupts, false = disables FTM interrupts */
)
{
   BAST_4517_P_Handle *pImpl = (BAST_4517_P_Handle *)(h->pImpl);
   BERR_Code retCode;
   uint32_t mask = BHAB_4517_HIRQ0_FSK_MSG_READY;
   
   BAST_4517_CHK_RETCODE(BHAB_WriteRegister(pImpl->hHab, BCHP_LEAP_HOST_IRQ_CLEAR0, &mask));
   if (bEnable)
   {
      BAST_4517_CHK_RETCODE(BHAB_WriteRegister(pImpl->hHab, BCHP_LEAP_HOST_IRQ_MASK_CLEAR0, &mask));
   }
   else
   {
      BAST_4517_CHK_RETCODE(BHAB_WriteRegister(pImpl->hHab, BCHP_LEAP_HOST_IRQ_MASK_SET0, &mask));
   }

   done:
   return retCode; 
}


/******************************************************************************
 BAST_4517_P_TunerConfigLna()
******************************************************************************/
BERR_Code BAST_4517_P_TunerConfigLna(BAST_Handle h, BAST_TunerLnaSettings *pSettings)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pImpl);
   BERR_Code retCode;
   uint8_t buf[8];
   
   buf[0] = 0x35;
   buf[1] = (uint8_t)pSettings->out0;
   buf[2] = (uint8_t)pSettings->out1;
   buf[3] = (uint8_t)pSettings->daisy;
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 5, buf, 5, true, true, 5));

   done:
   return retCode;
}


/******************************************************************************
 BAST_4517_P_GetTunerLnaStatus()
******************************************************************************/
BERR_Code BAST_4517_P_GetTunerLnaStatus(BAST_ChannelHandle h, BAST_TunerLnaStatus *pStatus)
{
   BAST_4517_P_Handle *pDevImpl = (BAST_4517_P_Handle *)(h->pDevice->pImpl);
   BERR_Code retCode;
   uint8_t buf[16];
   
   buf[0] = 0x28;
   buf[1] = h->channel;
   BAST_4517_CHK_RETCODE(BHAB_SendHabCommand(pDevImpl->hHab, buf, 13, buf, 13, true, true, 13));
   pStatus->status = buf[2];
   pStatus->outCfg = (BAST_TunerLnaOutputConfig)buf[3];
   pStatus->lnaAgc = (uint32_t)((buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7]);
   pStatus->bbAgc = (uint32_t)((buf[8] << 24) | (buf[9] << 16) | (buf[10] << 8) | buf[11]);

   done:
   return retCode;
}


