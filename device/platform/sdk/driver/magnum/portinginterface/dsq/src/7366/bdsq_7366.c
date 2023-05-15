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
#include "bstd.h"
#include "bdsq.h"
#include "bdsq_7366.h"
#include "bdsq_7366_priv.h"


BDBG_MODULE(bdsq_7366);


static const BDSQ_Settings defDevSettings =
{
   {
      BDSQ_7366_P_Open,
      BDSQ_7366_P_Close,
      BDSQ_7366_P_GetTotalChannels,
      BDSQ_7366_GetChannelDefaultSettings,
      BDSQ_7366_P_OpenChannel,
      BDSQ_7366_P_CloseChannel,
      BDSQ_7366_P_GetDevice,
      BDSQ_7366_P_GetVersionInfo,
      BDSQ_7366_P_Reset,
      BDSQ_7366_P_ResetChannel,
      BDSQ_7366_P_PowerDownChannel,
      BDSQ_7366_P_PowerUpChannel,
      BDSQ_7366_P_IsChannelOn,
      BDSQ_7366_P_PowerDownVsense,
      BDSQ_7366_P_PowerUpVsense,
      BDSQ_7366_P_IsVsenseOn,
      BDSQ_7366_P_SetTone,
      BDSQ_7366_P_GetTone,
      BDSQ_7366_P_SetVoltage,
      BDSQ_7366_P_GetVoltage,
      BDSQ_7366_P_EnableVsenseInterrupt,
      NULL, /* BDSQ_7366_P_EnableActivityInterrupt */
      NULL, /* BDSQ_7366_P_EnableIdleInterrupt */
      BDSQ_7366_P_Write,
      BDSQ_7366_P_EnableRx,
      BDSQ_7366_P_Read,
      BDSQ_7366_P_SendACW,
      BDSQ_7366_P_GetStatus,
      NULL, /* BDSQ_7366_P_GetActivityStatus */
      NULL, /* BDSQ_7366_P_ResetActivityStatus */
      BDSQ_7366_P_SetChannelSettings,
      BDSQ_7366_P_GetChannelSettings,
      BDSQ_7366_P_SetConfig,
      BDSQ_7366_P_GetConfig,
      BDSQ_7366_P_SetChannelConfig,
      BDSQ_7366_P_GetChannelConfig,
      BDSQ_7366_P_GetTxEventHandle,
      BDSQ_7366_P_GetRxEventHandle,
      BDSQ_7366_P_GetVsenseEventHandle,
      NULL, /* BDSQ_7366_P_GetActivityEventHandle */
      NULL  /* BDSQ_7366_P_GetIdleEventHandle */
   }
};


static const BDSQ_ChannelSettings defChnSettings =
{
   true,    /* bEnvelope */
   true,    /* bToneAlign */
   false,   /* bDisableRRTO */
   false,   /* bEnableToneburst */
   true,    /* bToneburstB */
   false,   /* bOverrideFraming */
   false,   /* bExpectReply */
   false,   /* TBD unused */
   true     /* bDisableRxOnly */
};


/******************************************************************************
 BDSQ_7366_GetDefaultSettings()
******************************************************************************/
BERR_Code BDSQ_7366_GetDefaultSettings(
   BDSQ_Settings *pDefSettings /* [out] default settings */
)
{
   *pDefSettings = defDevSettings;
   return BERR_SUCCESS;
}


/******************************************************************************
 BDSQ_7366_GetChannelDefaultSettings()
******************************************************************************/
BERR_Code BDSQ_7366_GetChannelDefaultSettings(
   BDSQ_Handle   h,                      /* [in] BDSQ handle */
   uint32_t      chnNo,                  /* [in] channel number */
   BDSQ_ChannelSettings *pChnDefSettings /* [out] default channel settings */
)
{
   BSTD_UNUSED(h);
   BSTD_UNUSED(chnNo);
   *pChnDefSettings = defChnSettings;
   return BERR_SUCCESS;
}

