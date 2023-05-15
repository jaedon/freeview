/**************************************************************************
 *     (c)2005-2011 Broadcom Corporation
 *  
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *   
 *  Except as expressly set forth in the Authorized License,
 *   
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *   
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *  
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
 *  ANY LIMITED REMEDY.
 * 
 * $brcm_Workfile: bads_api.c $
 * $brcm_Revision: Hydra_Software_Devel/45 $
 * $brcm_Date: 11/16/11 6:04p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ads/35233/bads_api.c $
 * 
 * Hydra_Software_Devel/45   11/16/11 6:04p atanugul
 * SW3461-79: Add No Signal to GetLockStatus()
 * 
 * Hydra_Software_Devel/44   10/10/11 1:51p farshidf
 * SW7552-134: update to match V2_0 of 3461
 * 
 * Hydra_Software_Devel/43   8/22/11 11:08a farshidf
 * SWDTV-7671: update power call according to Adam
 * 
 * Hydra_Software_Devel/42   8/16/11 4:31p farshidf
 * SW7552-105: update the frontend with 3461_V1_0_RC0 label
 * 
 * Hydra_Software_Devel/41   8/16/11 4:23p farshidf
 * SW7552-105: rebase ro 3461_V1_0_RC0 label
 * 
 * Hydra_Software_Devel/40   7/28/11 4:11p farshidf
 * SWDTV-8113: update the close
 * 
 * Hydra_Software_Devel/39   7/27/11 5:45p farshidf
 * SWDTV-8113: clean up
 * 
 * Hydra_Software_Devel/38   7/20/11 2:14p farshidf
 * SW7552-60: stop timer before acquire
 * 
 * Hydra_Software_Devel/37   7/18/11 4:22p farshidf
 * SWDTV-7869: compile fix
 * 
 * Hydra_Software_Devel/36   7/18/11 3:53p farshidf
 * SWDTV-7869: compile fix
 * 
 * Hydra_Software_Devel/35   7/15/11 5:47p farshidf
 * SWDTV-7869: add the 35126Bo changes
 * 
 * Hydra_Software_Devel/34   7/15/11 10:27a farshidf
 * SWDTV-7671: correction
 * 
 * Hydra_Software_Devel/33   7/15/11 9:47a farshidf
 * SWDTV-7671: add new header file and move back the resource free
 * 
 * Hydra_Software_Devel/32   7/14/11 4:42p farshidf
 * SW3128-28: update to fix the FFt IRQ
 * 
 * Hydra_Software_Devel/31   7/13/11 5:30p farshidf
 * SWDTV-7671: fix the Power Management 2.0 code
 * 
 * Hydra_Software_Devel/30   7/12/11 4:39p farshidf
 * SWDTV-7671: merge
 * 
 * Hydra_Software_Devel/SWDTV-7327/2   7/11/11 10:52a farshidf
 * SWDTV-7671: add the power managment 2.0 for DS
 * 
 * Hydra_Software_Devel/SWDTV-7327/1   6/2/11 5:43p shyi
 * SWDTV-7237: Added unit conversion from Hz to 256Hz for carrier range
 * 
 * Hydra_Software_Devel/28   6/3/11 5:39p farshidf
 * SW7552-36: update header file
 * 
 * Hydra_Software_Devel/27   6/1/11 11:43a farshidf
 * SW7552-36: clean up the code
 * 
 * Hydra_Software_Devel/26   5/31/11 3:54p farshidf
 * SW7552-36: add disable power saver
 * 
 * Hydra_Software_Devel/25   5/31/11 2:06p farshidf
 * SW7552-36: fix the timer
 * 
 * Hydra_Software_Devel/24   5/27/11 1:57p vishk
 * SW7231-180: Remove duplicate member variables in bads.h and convert
 * BADS_AutomaticFrequencyOffset and NEXUS_FrontendQamFrequencyOffset
 * enum types to unsigned integer.
 * 
 * Hydra_Software_Devel/23   5/27/11 10:50a farshidf
 * SW7552-36: compiel fix
 * 
 * Hydra_Software_Devel/22   5/26/11 5:19p vishk
 * SWDTV-7035: adapt to latest bads.h file
 * 
 * Hydra_Software_Devel/21   5/16/11 4:26p farshidf
 * SWDTV-7035: adapt to latest bads.h file
 * 
 * Hydra_Software_Devel/20   5/16/11 12:51p farshidf
 * SWDTV-7035:merg main
 * 
 * Hydra_Software_Devel/SWDTV-7035/5   5/16/11 12:50p farshidf
 * SWDTV-7035: update naming convention
 * 
 * Hydra_Software_Devel/19   5/10/11 11:07p farshidf
 * SWDTV-7035: merge main
 * 
 * Hydra_Software_Devel/SWDTV-7035/3   5/10/11 6:30p cbrooks
 * SWDTV-7035:Fixed MIPS timer
 * 
 * Hydra_Software_Devel/SWDTV-7035/2   5/10/11 5:51p farshidf
 * SWDTV-7035: put back Nexus status
 * 
 * Hydra_Software_Devel/SWDTV-7035/1   5/10/11 11:54a farshidf
 * SWDTV-7035: update the naming
 * 
 * Hydra_Software_Devel/18   5/9/11 6:42p farshidf
 * SWDTV-6857: fix teh BBS lock info
 * 
 * Hydra_Software_Devel/17   5/9/11 5:16p farshidf
 * SWDTV-6857: fix compil issue
 * 
 * Hydra_Software_Devel/16   5/9/11 5:09p farshidf
 * SWDTV-6857: remove teh Nexus line check
 * 
 * Hydra_Software_Devel/15   5/6/11 5:09p farshidf
 * SWDTV-6857: update Nexus Status
 * 
 * Hydra_Software_Devel/14   5/6/11 4:26p farshidf
 * SWDTV-6857: waring fix
 * 
 * Hydra_Software_Devel/13   5/6/11 3:50p farshidf
 * SWDTV-6857: fix some of the status to Nexus
 * 
 * Hydra_Software_Devel/12   5/6/11 2:04p farshidf
 * SWDTV-6587: add scan structure and calls
 * 
 * Hydra_Software_Devel/11   5/6/11 9:07a farshidf
 * SWDTV-6857: merge main
 * 
 * Hydra_Software_Devel/SWDTV-6857/1   5/5/11 9:18p cbrooks
 * SWDTV-6857:Cleaned up Code
 * 
 * Hydra_Software_Devel/10   4/28/11 9:08p farshidf
 * SWDTV-6190: reset the reac count
 * 
 * Hydra_Software_Devel/9   4/28/11 8:20p farshidf
 * SWDTV-6190: fix the lockstatus call
 * 
 * Hydra_Software_Devel/8   4/28/11 11:42a farshidf
 * SWDTV-6190: fix the channel handle
 * 
 * Hydra_Software_Devel/7   4/27/11 11:47p farshidf
 * SWDTV-6190: move the L3 interrupt processing inside the PI
 * 
 * Hydra_Software_Devel/6   4/27/11 3:42p farshidf
 * SWDTV-6190: bug fix
 * 
 * Hydra_Software_Devel/5   4/27/11 12:01a farshidf
 * SWDTV-6190: fix compile
 * 
 * Hydra_Software_Devel/4   4/26/11 4:09p farshidf
 * SWDTV-6191: update the param
 * 
 * Hydra_Software_Devel/3   4/26/11 2:26p farshidf
 * SWDTV-6191: fix the DS compile
 * 
 * Hydra_Software_Devel/2   4/25/11 6:22p farshidf
 * SWDTV-6191: update the ADS callback
 * 
 * Hydra_Software_Devel/1   4/20/11 12:40p farshidf
 * SWDTV-6190: add intial DS files
 *  
 ***************************************************************************/
#include "bstd.h"
#include "bads.h"
#include "bads_priv.h"
#include "btmr.h"
#include "bmth.h"
#include "bkni.h"
#include "bads_def.h"
#include "bads_api.h"
#include "bads_acquire.h"
#include "bads_status.h"
#include "bads_utils.h"
#include "bchp_ds_topm.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_int_id_ds.h"
#include "bads_global_clk.h"
#include "bads_mth.h"
#if ((BCHP_CHIP==7552) || (BCHP_CHIP==35230)  || (BCHP_CHIP==35233))
#include "bchp_int_id_ds_topm.h"
#endif
#ifdef BCHP_PWR_SUPPORT
#include "bchp_pwr_resources.h" 
#include "bchp_pwr.h"
#endif

BDBG_MODULE(bads_api);

/*******************************************************************************
*   BADS_P_3x7x_Isr
*******************************************************************************/

void BADS_P_3x7x_Isr(void *p, int param)
{
  BADS_3x7x_ChannelHandle pHandle = (BADS_3x7x_ChannelHandle)p;
  uint32_t RegStatus, RegMaskStatus;
  BSTD_UNUSED(param);

  RegStatus = BREG_Read32(pHandle->hRegister, BCHP_DS_IRQSTS2);
  RegMaskStatus = BREG_Read32(pHandle->hRegister, BCHP_DS_IRQMSK2);

	
  if ((RegStatus)& ~(RegMaskStatus))
  {
	RegStatus &= ~RegMaskStatus;
#if BCHP_DS_CORE_V_9_1 
	if (RegStatus & BCHP_DS_IRQSTS2_FCA_IRQ_MASK)
	{	
		BREG_Write32(pHandle->hRegister, BCHP_DS_IRQCLR2, BCHP_DS_IRQCLR2_FCA_ICLR_MASK);
		BKNI_SetEvent(pHandle->hFcaEvent);
	}
	else if (RegStatus & BCHP_DS_IRQSTS2_CHSCN_IRQ_MASK)
	{
		BREG_Write32(pHandle->hRegister, BCHP_DS_IRQCLR2, BCHP_DS_IRQCLR2_CHSCN_ICLR_MASK);
		BKNI_SetEvent(pHandle->hChscnEvent);
	}
#elif ((BCHP_DS_CORE_V_9_2) || (BCHP_DS_CORE_V_9_4))
	if (RegStatus & BCHP_DS_IRQCLR2_FFT_ICLR_MASK)
	{	
		BKNI_SetEvent(pHandle->hFftEvent);
		BREG_Write32(pHandle->hRegister, BCHP_DS_IRQCLR2, BCHP_DS_IRQCLR2_FFT_ICLR_MASK);
	}
#endif  
  }
}


/*******************************************************************************
*   BADS_P_TimerFunc
*******************************************************************************/
BERR_Code BADS_P_TimerFunc(void *myParam1, int myParam2)
{

	BERR_Code retCode = BERR_SUCCESS;
    BADS_3x7x_Handle hImplDev;
	BADS_3x7x_ChannelHandle h;
    BSTD_UNUSED(myParam2);

    BDBG_ENTER(BADS_P_TimerFunc);
	hImplDev = (BADS_3x7x_Handle) myParam1;
	BDBG_ASSERT( hImplDev );
	h = (BADS_3x7x_ChannelHandle)(hImplDev->hAdsChn[0]);
    BDBG_ASSERT( h );
  
	BKNI_ResetEvent(hImplDev->hIntEvent);
	BADS_P_ChnLockStatus(h);

    if ( h->pCallback[BADS_Callback_eLockChange] != NULL )
	{
            if (h->pChnLockStatus->FLK == 1)
            {
                if (h->pChnAcqParam->BADS_Local_Params.LockUpdate)
                {
					h->pChnAcqParam->BADS_Local_Params.LockUpdate = false;
					(h->pCallback[BADS_Callback_eLockChange])(h->pCallbackParam[BADS_Callback_eLockChange] );
                }
            }
            else
            {
                if (h->pChnAcqParam->BADS_Local_Params.lock_status != h->pChnLockStatus->FLK )
                {	                   
					h->pChnAcqParam->BADS_Local_Params.LockUpdate = true;
		           (h->pCallback[BADS_Callback_eLockChange])(h->pCallbackParam[BADS_Callback_eLockChange] );
				   h->pChnLockStatus->ReAck_Count = 0;
                }
            }
	}
	h->pChnAcqParam->BADS_Local_Params.lock_status = h->pChnLockStatus->FLK;

	if (h->pChnAcqParam->BADS_Acquire_Params.NexusStatusMode & BADS_NexusStatusMode_EnableStatusForNexus)
	{
  		BADS_P_ChnStatus(h);
	    h->pChnAcqParam->BADS_Acquire_Params.NexusStatusMode &= (~BADS_NexusStatusMode_EnableStatusForNexus);
	}

	if (h->pChnAcqParam->BADS_Acquire_Params.AcquireStartMode & BADS_AcquireStartMode_ResetStatus)
	{
		BDBG_WRN(("call reset"));
		BADS_P_ChnStatusReset(h);
	    h->pChnAcqParam->BADS_Acquire_Params.AcquireStartMode &= (~BADS_AcquireStartMode_ResetStatus);
	}

	if (h->pChnAcqParam->BADS_Acquire_Params.AcquireStartMode & BADS_AcquireStartMode_Acquire)
	{
		h->pChnAcqParam->BADS_Acquire_Params.AcquireStartMode &= ~BADS_AcquireStartMode_Acquire;
		h->pChnLockStatus->ReAck_Count = 0;
		BDBG_WRN(("call1 BADS_3x7x_ProcessInterruptEvent"));
		BKNI_SetEvent(hImplDev->hIntEvent);
		return BERR_SUCCESS;
	}
	else
	{
		if (h->pChnAcqParam->BADS_Acquire_Params.Auto == 1)
		{
			if (h->pChnAcqParam->BADS_Local_Params.lock_status == 0)
			{
				BDBG_MSG(("call  BADS_3x7x_ProcessInterruptEvent in AUTO mode"));
				BKNI_SetEvent(hImplDev->hIntEvent);
			}
		}
	}

	BDBG_LEAVE(BADS_P_TimerFunc);
	return retCode;
}


/*******************************************************************************
*   BADS_3x7x_Open
*******************************************************************************/
BERR_Code BADS_3x7x_Open(
    BADS_Handle *pAds,                          /* [out] Returns handle */
    BCHP_Handle hChip,                          /* [in] Chip handle */
    BREG_Handle hRegister,                      /* [in] Register handle */
    BINT_Handle hInterrupt,                     /* [in] Interrupt handle */
    const struct BADS_Settings *pDefSettings    /* [in] Default settings */
    )
{

    BERR_Code retCode = BERR_SUCCESS;
	BADS_Handle hDev;
    uint8_t chnIdx;
	BTMR_Settings sTimerSettings;
	BADS_3x7x_Handle hImplDev = NULL;;
    BDBG_ENTER(BADS_3x7x_Open);

    /* Alloc memory from the system heap */
    hDev = (BADS_Handle) BKNI_Malloc( sizeof( BADS_P_Handle ) );

    if( hDev == NULL )
    {
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BADS_Open: BKNI_malloc() failed"));
        goto done;
    }
   
    BKNI_Memset( hDev, 0x00, sizeof( BADS_P_Handle ) );
    hDev->magicId = DEV_MAGIC_ID;
	BKNI_Memcpy(&hDev->settings, pDefSettings, sizeof(*pDefSettings));

    /* Alloc memory from the system heap */
    hImplDev = (BADS_3x7x_Handle) BKNI_Malloc( sizeof( BADS_P_3x7x_Handle ) );
    if( hImplDev == NULL )
    {
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BADS_Open: BKNI_malloc() failed"));
        goto done;
    }
    
    BKNI_Memset( hImplDev, 0x00, sizeof( BADS_P_3x7x_Handle ) );
    hImplDev->magicId = DEV_MAGIC_ID;
	hImplDev->hChip = hChip;
    hImplDev->mxChnNo = MX_ADS_CHANNELS;
    hImplDev->hRegister = hRegister;
    hImplDev->hInterrupt = hInterrupt;
#ifdef BCHP_PWR_RESOURCE_DS_TOP
    BCHP_PWR_AcquireResource(hImplDev->hChip, BCHP_PWR_RESOURCE_DS_TOP);
#endif   
    
	BKNI_CreateEvent(&(hImplDev->hIntEvent));

	/* Create timer for status lock check */

	sTimerSettings.type = BTMR_Type_ePeriodic;
	sTimerSettings.cb_isr = (BTMR_CallbackFunc)BADS_P_TimerFunc;
	sTimerSettings.pParm1 = (void*)hImplDev;
	sTimerSettings.parm2 = 0;
    sTimerSettings.exclusive = false;
	retCode = BTMR_CreateTimer (hDev->settings.hTmr, &hImplDev->hTimer, &sTimerSettings);
	if ( retCode != BERR_SUCCESS )
	{
	   BDBG_ERR(("BTHD_Open: Create Timer Failed"));
	   goto done;
    }	

	sTimerSettings.type = BTMR_Type_eStopWatch;
	sTimerSettings.cb_isr = 0;
	sTimerSettings.pParm1 = 0;
	sTimerSettings.parm2 = 0;
	sTimerSettings.exclusive = false;
	retCode = BTMR_CreateTimer (hDev->settings.hTmr, &hImplDev->hStatusTimer, &sTimerSettings);
	if ( retCode != BERR_SUCCESS )
	{
	   BDBG_ERR(("BTHD_Open: Create Timer Failed"));
	   goto done;
    }
	BTMR_StartTimer(hImplDev->hStatusTimer, POWER2_30/MIPS_TIMER);   /* the timer is in Micro second */;


    for( chnIdx = 0; chnIdx < MX_ADS_CHANNELS; chnIdx++ )
    {
        hImplDev->hAdsChn[chnIdx] = NULL;
    }

    hDev->pImpl = hImplDev;

    *pAds = hDev;
done:
#ifdef BCHP_PWR_RESOURCE_DS_TOP
		BCHP_PWR_ReleaseResource(hImplDev->hChip, BCHP_PWR_RESOURCE_DS_TOP);
#endif 
    if( retCode != BERR_SUCCESS )
    {
		if( hImplDev != NULL )
        {
            BKNI_Free( hImplDev );
        }  
		if( hDev != NULL )
        {
            BKNI_Free( hDev );
        }
        *pAds = NULL;
    }

    return( retCode );
}


/*******************************************************************************
*   BADS_3x7x_Close
*******************************************************************************/
BERR_Code BADS_3x7x_Close(
    BADS_Handle hDev                    /* [in] Device handle */
    )
{
	BERR_Code retCode = BERR_SUCCESS;
	BADS_3x7x_Handle hImplDev = (BADS_3x7x_Handle) hDev->pImpl;
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    BDBG_ENTER(BADS_3x7x_Close);
	BTMR_DestroyTimer(hImplDev->hTimer);
	BTMR_DestroyTimer(hImplDev->hStatusTimer);
	BKNI_DestroyEvent(hImplDev->hIntEvent);

    hDev->magicId = 0x00;       /* clear it to catch improper use */
	BKNI_Free( (void *) hDev->pImpl  );
	BKNI_Free( (void *) hDev );

    BDBG_LEAVE(BADS_3x7x_Close);
    return( retCode );
}

/*******************************************************************************
*   BADS_3x7x_Init
*******************************************************************************/
BERR_Code BADS_3x7x_Init(
    BADS_Handle hDev                    /* [in] Device handle */
    )
{ 
    BERR_Code retCode = BERR_SUCCESS;
	BDBG_ENTER(BADS_3x7x_Init);
    BSTD_UNUSED(hDev);
    BDBG_LEAVE(BADS_3x7x_Init);
    return( retCode );
}

/*******************************************************************************
*   BADS_3x7x_Init
*******************************************************************************/

BERR_Code BADS_3x7x_GetVersion(
    BADS_Handle hDev,                   /* [in] Device handle */
    BADS_Version *pVersion              /* [out] Returns version */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_3x7x_Handle hImplDev;
	uint32_t chipVer;
	BADS_Version Version;

    BDBG_ENTER(BADS_3x7x_GetVersion);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    hImplDev = (BADS_3x7x_Handle) hDev->pImpl;
    BDBG_ASSERT( hImplDev );

	chipVer = BREG_Read32(hImplDev->hRegister, BCHP_SUN_TOP_CTRL_PRODUCT_ID);
	Version.majVer = chipVer >> 8;
    Version.minVer = chipVer & 0xFF;

    *pVersion = Version;     

    BDBG_LEAVE(BADS_3x7x_GetVersion);
    return( retCode );
}


/*******************************************************************************
*   BADS_3x7x_GetTotalChannels
*******************************************************************************/
BERR_Code BADS_3x7x_GetTotalChannels(
    BADS_Handle hDev,                   /* [in] Device handle */
    unsigned int *totalChannels         /* [out] Returns total number downstream channels supported */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_3x7x_Handle hImplDev;

    BDBG_ENTER(BADS_3x7x_GetTotalChannels);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    hImplDev = (BADS_3x7x_Handle) hDev->pImpl;
    BDBG_ASSERT( hImplDev );

    *totalChannels = hImplDev->mxChnNo; /* use saved data */

    BDBG_LEAVE(BADS_3x7x_GetTotalChannels);
    return( retCode );
}


/*******************************************************************************
*  Channel Managment level
*  BADS_3x7x_OpenChannel
*******************************************************************************/

BERR_Code BADS_3x7x_OpenChannel(
    BADS_Handle hDev,                                   /* [in] Device handle */
    BADS_ChannelHandle *phChn,                          /* [out] Returns channel handle */
    unsigned int channelNo,                             /* [in] Channel number to open */
    const struct BADS_ChannelSettings *pChnDefSettings  /* [in] Channel default setting */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_3x7x_Handle hImplDev;
    BADS_3x7x_ChannelHandle hImplChnDev = NULL;
    BADS_ChannelHandle hChnDev;
	uint32_t BufSrc;

    BDBG_ENTER(BADS_3x7x_OpenChannel);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    hImplDev = (BADS_3x7x_Handle) hDev->pImpl;
    BDBG_ASSERT( hImplDev );
    hChnDev = NULL;
    if( channelNo < hImplDev->mxChnNo )
    {
        if( hImplDev->hAdsChn[channelNo] == NULL )
        {
		    BDBG_MSG(("BADS_3x7x_OpenChannel"));

			/* Alloc memory from the system heap */
            hChnDev = (BADS_ChannelHandle) BKNI_Malloc( sizeof( BADS_P_ChannelHandle ) );
            if( hChnDev == NULL )
            {
                retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
                BDBG_ERR(("BADS_OpenChannel: BKNI_malloc() failed"));
                goto done;
            }
            
            BKNI_Memset( hChnDev, 0x00, sizeof( BADS_P_ChannelHandle ) );
            hChnDev->magicId = DEV_MAGIC_ID;
            hChnDev->hAds = hDev;

			hImplChnDev = (BADS_3x7x_ChannelHandle) BKNI_Malloc( sizeof( BADS_P_3x7x_ChannelHandle ) );
            if( hImplChnDev == NULL )
            {
                retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
                BDBG_ERR(("BADS_OpenChannel: BKNI_malloc() failed, impl"));
                goto done;
            }
            
            hImplChnDev->chnNo = channelNo;
			hImplChnDev->hAds = hImplDev;
			hImplChnDev->magicId = DEV_MAGIC_ID;
			hImplChnDev->hAds = hChnDev;
#if BCHP_DS_CORE_V_9_1 
			BKNI_CreateEvent(&(hImplChnDev->hChscnEvent));
			BKNI_CreateEvent(&(hImplChnDev->hFcaEvent));
#elif ((BCHP_DS_CORE_V_9_2) || (BCHP_DS_CORE_V_9_4))
			BKNI_CreateEvent(&(hImplChnDev->hFftEvent));
#endif
			BINT_CreateCallback( &(hImplChnDev->hDs2Callback), hImplDev->hInterrupt, BCHP_INT_ID_DS0_2_IS, BADS_P_3x7x_Isr, (void*)hImplChnDev, BADS_DS0_2_EVENT );
			BINT_EnableCallback(hImplChnDev->hDs2Callback);	
			hImplChnDev->hRegister = hImplDev->hRegister;

			hImplChnDev->pChnStatus = (BADS_3x7x_ChnStatus_t *)BMEM_AllocAligned(hDev->settings.hHeap, sizeof(BADS_3x7x_ChnStatus_t), 0, 0 );
			if( hImplChnDev->pChnStatus == NULL )
			{
				retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
				BDBG_ERR(("BADS_Open: BKNI_malloc() failed"));
				goto done;
			}
			BMEM_ConvertAddressToOffset(hDev->settings.hHeap, hImplChnDev->pChnStatus, &BufSrc );
			BKNI_Memset(hImplChnDev->pChnStatus, 0x00, sizeof( BADS_3x7x_ChnStatus_t ) );
			BREG_Write32(hImplDev->hRegister, BCHP_DS_TOPM_SW_SPARE1, BufSrc);
         
			hImplChnDev->pChnAcqParam = (BADS_3x7x_AcqParams_t *)BMEM_AllocAligned(hDev->settings.hHeap, sizeof(BADS_3x7x_AcqParams_t), 0, 0 );
			 if( hImplChnDev->pChnAcqParam == NULL )
            {
                retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
                BDBG_ERR(("BADS_OpenChannel: BMEM_AllocAligned() failed"));
                goto done;
            }
			BMEM_ConvertAddressToOffset(hDev->settings.hHeap, hImplChnDev->pChnAcqParam, &BufSrc );
			BKNI_Memset(hImplChnDev->pChnAcqParam, 0x00, sizeof( BADS_3x7x_AcqParams_t ) );
			BREG_Write32(hImplDev->hRegister, BCHP_DS_TOPM_SW_SPARE2, BufSrc);
			hImplChnDev->pChnLockStatus = (BADS_3x7x_ChnLockStatus_t *)BMEM_AllocAligned(hDev->settings.hHeap, sizeof(BADS_3x7x_ChnLockStatus_t), 0, 0 );
			if( hImplChnDev->pChnLockStatus == NULL )
            {
                retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
                BDBG_ERR(("BADS_OpenChannel: BMEM_AllocAligned() failed"));
                goto done;
            }
			BMEM_ConvertAddressToOffset(hDev->settings.hHeap, hImplChnDev->pChnLockStatus, &BufSrc );
			BKNI_Memset( hImplChnDev->pChnLockStatus, 0x00, sizeof( BADS_3x7x_ChnLockStatus_t ) );
			BREG_Write32(hImplDev->hRegister, BCHP_DS_TOPM_SPARE1, BufSrc);
			hImplChnDev->pChnAcqParam->BADS_Local_Params.lock_status = 2;   /* init to a value different than 1 and 0 */
			
	
			if (pChnDefSettings->autoAcquire == true) 
			{
				hImplChnDev->pChnAcqParam->BADS_Acquire_Params.Auto = BADS_Acquire_Params_eEnable;
			}
			else
			{
				hImplChnDev->pChnAcqParam->BADS_Acquire_Params.Auto = BADS_Acquire_Params_eDisable;
			}

            hImplDev->hAdsChn[channelNo] = hImplChnDev;
            hChnDev->pImpl = hImplChnDev;

            *phChn = hChnDev;

        }
        else
        {
            retCode = BERR_TRACE(BADS_ERR_NOTAVAIL_CHN_NO);
        }
    }
    else
    {
        retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
    }

done:
    if( retCode != BERR_SUCCESS )
    {
        if( hImplChnDev->pChnStatus != NULL )
			BMEM_Free(hDev->settings.hHeap,(void *)(hImplChnDev->pChnStatus) );
		if( hImplChnDev->pChnAcqParam != NULL )
			BMEM_Free(hDev->settings.hHeap ,(void *)(hImplChnDev->pChnAcqParam) );
		if( hImplChnDev->pChnLockStatus != NULL )
			BMEM_Free(hDev->settings.hHeap,(void *)(hImplChnDev->pChnLockStatus) );
		if( hImplChnDev != NULL )
			BKNI_Free( hImplChnDev );
		if( hChnDev != NULL )
            BKNI_Free( hChnDev );
        *phChn = NULL;
    }
    BDBG_LEAVE(BADS_3x7x_OpenChannel);
    return( retCode );
}

/*******************************************************************************
*   BADS_3x7x_CloseChannel
*******************************************************************************/
BERR_Code BADS_3x7x_CloseChannel(
    BADS_ChannelHandle hChn  
	)
{
    BADS_Handle hDev;
	    BADS_3x7x_Handle hImplDev;

    BADS_3x7x_ChannelHandle hImplChnDev = (BADS_3x7x_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );
    hDev = hChn->hAds;
	hImplDev = (BADS_3x7x_Handle) hDev->pImpl;

    BINT_DisableCallback(hImplChnDev->hDs2Callback);
#if BCHP_DS_CORE_V_9_1 
		BKNI_DestroyEvent(hImplChnDev->hFcaEvent);
		BKNI_DestroyEvent(hImplChnDev->hChscnEvent);
#elif ((BCHP_DS_CORE_V_9_2) || (BCHP_DS_CORE_V_9_4))
		BKNI_DestroyEvent(hImplChnDev->hFftEvent);
#endif

    hChn->magicId = 0x00;       /* clear it to catch inproper use */
	BMEM_Free(hDev->settings.hHeap ,(void *)(hImplChnDev->pChnAcqParam) );
	BMEM_Free(hDev->settings.hHeap,(void *)(hImplChnDev->pChnLockStatus) );
	BMEM_Free(hDev->settings.hHeap,(void *)(hImplChnDev->pChnStatus) );
	hImplDev->hAdsChn[hImplChnDev->chnNo] = NULL;
    BKNI_Free( hChn->pImpl );
    BKNI_Free( hChn );

	return BERR_SUCCESS;
}


/*******************************************************************************
*   BADS_3x7x_GetDevice
*******************************************************************************/
BERR_Code BADS_3x7x_GetDevice(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_Handle *phDev                  /* [out] Returns Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BADS_3x7x_GetDevice);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    *phDev = hChn->hAds;

    BDBG_LEAVE(BADS_3x7x_GetDevice);
    return( retCode );
}


/*******************************************************************************
*   BADS_3x7x_SetScanParam
*******************************************************************************/
BERR_Code BADS_3x7x_SetScanParam(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_ChannelScanSettings *pChnScanSettings                 
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_3x7x_ChannelHandle hImplChnDev;
	
    BDBG_ENTER(BADS_3x7x_SetScanParam);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );
    hImplChnDev = (BADS_3x7x_ChannelHandle) hChn->pImpl;

    hImplChnDev->pChnAcqParam->BADS_Scan_Params.AI = pChnScanSettings->AI;
	hImplChnDev->pChnAcqParam->BADS_Scan_Params.QM = pChnScanSettings->QM;
	hImplChnDev->pChnAcqParam->BADS_Scan_Params.CO = pChnScanSettings->CO;
	hImplChnDev->pChnAcqParam->BADS_Scan_Params.TO = pChnScanSettings->TO;
	hImplChnDev->pChnAcqParam->BADS_Scan_Params.B1024 = pChnScanSettings->B1024;
	hImplChnDev->pChnAcqParam->BADS_Scan_Params.B256 = pChnScanSettings->B256;
	hImplChnDev->pChnAcqParam->BADS_Scan_Params.B64 = pChnScanSettings->B64;
	hImplChnDev->pChnAcqParam->BADS_Scan_Params.A1024 = pChnScanSettings->A1024;
	hImplChnDev->pChnAcqParam->BADS_Scan_Params.A512 = pChnScanSettings->A512;
	hImplChnDev->pChnAcqParam->BADS_Scan_Params.A256 = pChnScanSettings->A256;
	hImplChnDev->pChnAcqParam->BADS_Scan_Params.A128 = pChnScanSettings->A128;
	hImplChnDev->pChnAcqParam->BADS_Scan_Params.A64 = pChnScanSettings->A64;
	hImplChnDev->pChnAcqParam->BADS_Scan_Params.A32 = pChnScanSettings->A32;
	hImplChnDev->pChnAcqParam->BADS_Scan_Params.A16 = pChnScanSettings->A16;
	hImplChnDev->pChnAcqParam->BADS_Scan_Params.Carrier_Search = pChnScanSettings->carrierSearch;
	hImplChnDev->pChnAcqParam->BADS_Scan_Params.Upper_Baud_Search = pChnScanSettings->upperBaudSearch;
	hImplChnDev->pChnAcqParam->BADS_Scan_Params.Lower_Baud_Search = pChnScanSettings->lowerBaudSearch;

    BDBG_LEAVE(BADS_3x7x_SetScanParam);
    return( retCode );
}



/*******************************************************************************
*   BADS_3x7x_GetScanParam
*******************************************************************************/
BERR_Code BADS_3x7x_GetScanParam(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_ChannelScanSettings *pChnScanSettings                 
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_3x7x_ChannelHandle hImplChnDev;
	
    BDBG_ENTER(BADS_3x7x_GetScanParam);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );
    hImplChnDev = (BADS_3x7x_ChannelHandle) hChn->pImpl;

	pChnScanSettings->AI = hImplChnDev->pChnAcqParam->BADS_Scan_Params.AI ;
	pChnScanSettings->QM = hImplChnDev->pChnAcqParam->BADS_Scan_Params.QM ;
	pChnScanSettings->CO = hImplChnDev->pChnAcqParam->BADS_Scan_Params.CO ;
	pChnScanSettings->TO = hImplChnDev->pChnAcqParam->BADS_Scan_Params.TO ;
	pChnScanSettings->B1024 = hImplChnDev->pChnAcqParam->BADS_Scan_Params.B1024 ;
	pChnScanSettings->B256 = hImplChnDev->pChnAcqParam->BADS_Scan_Params.B256 ;
	pChnScanSettings->B64 = hImplChnDev->pChnAcqParam->BADS_Scan_Params.B64 ;
	pChnScanSettings->A1024 = hImplChnDev->pChnAcqParam->BADS_Scan_Params.A1024 ;
	pChnScanSettings->A512 = hImplChnDev->pChnAcqParam->BADS_Scan_Params.A512 ;
	pChnScanSettings->A256 = hImplChnDev->pChnAcqParam->BADS_Scan_Params.A256 ;
	pChnScanSettings->A128 = hImplChnDev->pChnAcqParam->BADS_Scan_Params.A128 ;
	pChnScanSettings->A64 = hImplChnDev->pChnAcqParam->BADS_Scan_Params.A64 ;
	pChnScanSettings->A32 = hImplChnDev->pChnAcqParam->BADS_Scan_Params.A32 ;
	pChnScanSettings->A16 = hImplChnDev->pChnAcqParam->BADS_Scan_Params.A16 ;
	pChnScanSettings->carrierSearch = hImplChnDev->pChnAcqParam->BADS_Scan_Params.Carrier_Search ;
	pChnScanSettings->upperBaudSearch = hImplChnDev->pChnAcqParam->BADS_Scan_Params.Upper_Baud_Search ;
	pChnScanSettings->lowerBaudSearch = hImplChnDev->pChnAcqParam->BADS_Scan_Params.Lower_Baud_Search ;

    BDBG_LEAVE(BADS_3x7x_GetScanParam);
    return( retCode );
}
 


/*******************************************************************************
*   BADS_3x7x_Acquire
*******************************************************************************/
BERR_Code BADS_3x7x_Acquire(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_InbandParam *ibParam           /* [in] Inband Parameters to use */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_3x7x_ChannelHandle hImplChnDev;
	BADS_Handle hDev;
	BADS_3x7x_Handle hImplDev;

    BDBG_ENTER(BADS_3x7x_Acquire);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BADS_3x7x_ChannelHandle) hChn->pImpl;
	BDBG_ASSERT( hImplChnDev );
	hDev = (BADS_Handle)(hChn->hAds);
	BDBG_ASSERT( hDev );
	hImplDev = (BADS_3x7x_Handle) hDev->pImpl;
	BDBG_ASSERT( hImplDev );
	BDBG_MSG(("BADS_3x7x_Acquire"));

	switch(ibParam->modType)
	{
		case BADS_ModulationType_eAnnexAQam16:
			 hImplChnDev->pChnAcqParam->BADS_Acquire_Params.Annex = BADS_Acquire_Params_Annex_eAnnexA;
			 hImplChnDev->pChnAcqParam->BADS_Acquire_Params.Qam_Mode = BADS_Acquire_Params_BPS_eQam16;
			 break;
		case BADS_ModulationType_eAnnexAQam32:
			 hImplChnDev->pChnAcqParam->BADS_Acquire_Params.Annex = BADS_Acquire_Params_Annex_eAnnexA;
			 hImplChnDev->pChnAcqParam->BADS_Acquire_Params.Qam_Mode = BADS_Acquire_Params_BPS_eQam32;
			 break;
		case BADS_ModulationType_eAnnexAQam64:
			 hImplChnDev->pChnAcqParam->BADS_Acquire_Params.Annex = BADS_Acquire_Params_Annex_eAnnexA;
			 hImplChnDev->pChnAcqParam->BADS_Acquire_Params.Qam_Mode = BADS_Acquire_Params_BPS_eQam64;
			 break;
		case BADS_ModulationType_eAnnexAQam128:
			 hImplChnDev->pChnAcqParam->BADS_Acquire_Params.Annex = BADS_Acquire_Params_Annex_eAnnexA;
			 hImplChnDev->pChnAcqParam->BADS_Acquire_Params.Qam_Mode = BADS_Acquire_Params_BPS_eQam128;
			 break;
		case BADS_ModulationType_eAnnexAQam256:
			 hImplChnDev->pChnAcqParam->BADS_Acquire_Params.Annex = BADS_Acquire_Params_Annex_eAnnexA;
			 hImplChnDev->pChnAcqParam->BADS_Acquire_Params.Qam_Mode  = BADS_Acquire_Params_BPS_eQam256;
			 break;
		case BADS_ModulationType_eAnnexAQam1024:
			 hImplChnDev->pChnAcqParam->BADS_Acquire_Params.Annex = BADS_Acquire_Params_Annex_eAnnexA;
			 hImplChnDev->pChnAcqParam->BADS_Acquire_Params.Qam_Mode = BADS_Acquire_Params_BPS_eQam1024;
			 break;
		case BADS_ModulationType_eAnnexBQam16:
			 hImplChnDev->pChnAcqParam->BADS_Acquire_Params.Annex = BADS_Acquire_Params_Annex_eAnnexA;
			 hImplChnDev->pChnAcqParam->BADS_Acquire_Params.Qam_Mode = BADS_Acquire_Params_BPS_eQam16;
			 break;
		case BADS_ModulationType_eAnnexBQam32:
			 hImplChnDev->pChnAcqParam->BADS_Acquire_Params.Annex = BADS_Acquire_Params_Annex_eAnnexB;
			 hImplChnDev->pChnAcqParam->BADS_Acquire_Params.Qam_Mode = BADS_Acquire_Params_BPS_eQam32;
			 break;
		case BADS_ModulationType_eAnnexBQam64:
			 hImplChnDev->pChnAcqParam->BADS_Acquire_Params.Annex = BADS_Acquire_Params_Annex_eAnnexB;
			 hImplChnDev->pChnAcqParam->BADS_Acquire_Params.Qam_Mode = BADS_Acquire_Params_BPS_eQam64;
			 break;
		case BADS_ModulationType_eAnnexBQam128:
			 hImplChnDev->pChnAcqParam->BADS_Acquire_Params.Annex = BADS_Acquire_Params_Annex_eAnnexB;
			 hImplChnDev->pChnAcqParam->BADS_Acquire_Params.Qam_Mode = BADS_Acquire_Params_BPS_eQam128;
			 break;
		case BADS_ModulationType_eAnnexBQam256:
			 hImplChnDev->pChnAcqParam->BADS_Acquire_Params.Annex = BADS_Acquire_Params_Annex_eAnnexB;
			 hImplChnDev->pChnAcqParam->BADS_Acquire_Params.Qam_Mode = BADS_Acquire_Params_BPS_eQam256;
			 break;
		case BADS_ModulationType_eAnnexBQam1024:
			 hImplChnDev->pChnAcqParam->BADS_Acquire_Params.Annex = BADS_Acquire_Params_Annex_eAnnexB;
			 hImplChnDev->pChnAcqParam->BADS_Acquire_Params.Qam_Mode = BADS_Acquire_Params_BPS_eQam1024;
			 break;
		default:
			BDBG_WRN(("BADS_3x7x_Acquire: QAM mode not supported"));
			break;

	}
	hImplChnDev->pChnAcqParam->BADS_Acquire_Params.AnnexA_Sym_Rate = ibParam->symbolRate;			
	hImplChnDev->pChnAcqParam->BADS_Acquire_Params.IS = ibParam->invertSpectrum;
	hImplChnDev->pChnAcqParam->BADS_Acquire_Params.Auto = ibParam->autoAcquire;
	hImplChnDev->pChnAcqParam->BADS_Acquire_Params.AcqType = ibParam->acquisitionType;
	hImplChnDev->pChnAcqParam->BADS_Acquire_Params.Carrier_Range = ibParam->frequencyOffset / 256; /* from Hz to 256Hz */
	BKNI_Memset(hImplChnDev->pChnLockStatus, 0x00, sizeof( BADS_3x7x_ChnLockStatus_t ) ); /* Clear status structure upon new TuneAcquire command from host */
	hImplChnDev->pChnAcqParam->BADS_Local_Params.LockUpdate = true;
	hImplChnDev->pChnLockStatus->ReAck_Count = 0;

    BTMR_StopTimer(hImplDev->hTimer);  /* the timer is in Micro second */
	BADS_P_Acquire(hImplChnDev);
    BTMR_StartTimer(hImplDev->hTimer, 25000);   /* the timer is in Micro second */

    BDBG_LEAVE(BADS_3x7x_Acquire);
    return( retCode );
}


/*******************************************************************************
*   BADS_3x7x_GetStatus
*******************************************************************************/
BERR_Code BADS_3x7x_GetStatus(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_Status *pStatus                /* [out] Returns status */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_3x7x_ChannelHandle hImplChnDev;
    BADS_3x7x_Handle hImplDev;

    BDBG_ENTER(BADS_3x7x_GetStatus);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BADS_3x7x_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
	hImplDev = hImplChnDev->hAds;

	BADS_P_ChnStatus(hImplChnDev);
  

	pStatus->isPowerSaverEnabled = 0;													/* Enable=1, Disable=0 */

	pStatus->isFecLock = (hImplChnDev->pChnStatus->FLK==BADS_3x7x_ChnStatus_eLock)?1:0;	/* lock=1, unlock=0 */
	pStatus->isQamLock = (hImplChnDev->pChnStatus->QLK==BADS_3x7x_ChnStatus_eLock)?1:0;	/* lock=1, unlock=0 */
	pStatus->symbolRate = hImplChnDev->pChnStatus->Symbol_Rate;								/* in Baud */

	if (hImplChnDev->pChnStatus->AB == BADS_3x7x_ChnStatus_Annex_eAnnexA)
	{
		switch(hImplChnDev->pChnStatus->BPS)
		{
			case BADS_Acquire_Params_BPS_eQam16:
				pStatus->modType = BADS_ModulationType_eAnnexAQam16;
				break;
			case BADS_Acquire_Params_BPS_eQam32:
				pStatus->modType = BADS_ModulationType_eAnnexAQam32;
				break;
			case BADS_Acquire_Params_BPS_eQam64:
				pStatus->modType = BADS_ModulationType_eAnnexAQam64;
				break;
			case BADS_Acquire_Params_BPS_eQam128:
				pStatus->modType = BADS_ModulationType_eAnnexAQam128;
				break;
			case BADS_Acquire_Params_BPS_eQam256:
				pStatus->modType = BADS_ModulationType_eAnnexAQam256;
				break;
	 		case BADS_Acquire_Params_BPS_eQam512:
				pStatus->modType = BADS_ModulationType_eAnnexAQam512;
				break;
			case BADS_Acquire_Params_BPS_eQam1024:
				pStatus->modType = BADS_ModulationType_eAnnexAQam1024;
				break;
		}
	}
	else if (hImplChnDev->pChnStatus->AB == BADS_3x7x_ChnStatus_Annex_eAnnexB)
	{
		switch(hImplChnDev->pChnStatus->BPS)
		{
			case BADS_Acquire_Params_BPS_eQam16:
				pStatus->modType = BADS_ModulationType_eAnnexBQam16;
				break;
			case BADS_Acquire_Params_BPS_eQam32:
				pStatus->modType = BADS_ModulationType_eAnnexBQam32;
				break;
			case BADS_Acquire_Params_BPS_eQam64:
				pStatus->modType = BADS_ModulationType_eAnnexBQam64;
				break;
			case BADS_Acquire_Params_BPS_eQam128:
				pStatus->modType = BADS_ModulationType_eAnnexBQam128;
				break;
			case BADS_Acquire_Params_BPS_eQam256:
				pStatus->modType = BADS_ModulationType_eAnnexBQam256;
				break;
	 		case BADS_Acquire_Params_BPS_eQam512:
				pStatus->modType = BADS_ModulationType_eAnnexBQam512;
				break;
			case BADS_Acquire_Params_BPS_eQam1024:
				pStatus->modType = BADS_ModulationType_eAnnexBQam1024;
				break;
		}
	}
	pStatus->carrierFreqOffset = hImplChnDev->pChnStatus->Carrier_Error/1000;		/* in 1/1000 Hz */
	pStatus->carrierPhaseOffset = hImplChnDev->pChnStatus->Phase_Error/1000;		/* in 1/1000 Hz */
	pStatus->symbolRateError = hImplChnDev->pChnStatus->Symbol_Error;
    pStatus->rxSymbolRate = hImplChnDev->pChnStatus->Symbol_Error + hImplChnDev->pChnStatus->Symbol_Rate;
	pStatus->snrEstimate = hImplChnDev->pChnStatus->SNRAVG_db;
	pStatus->correctedBits = hImplChnDev->pChnStatus->FEC_Corr_RS_Bits;				
	pStatus->uncorrectedCount = hImplChnDev->pChnStatus->FEC_UCorr_RS_Blocks;					
	pStatus->correctedCount = hImplChnDev->pChnStatus->FEC_Corr_RS_Blocks;		
	pStatus->goodRsBlockCount = hImplChnDev->pChnStatus->FEC_Clean_RS_Blocks;	
	pStatus->berRawCount = hImplChnDev->pChnStatus->BERT;
    pStatus->isSpectrumInverted  = hImplChnDev->pChnStatus->SI;


#if 0 
	pStatus->modType = hImplChnDev->pChnStatus->BPS;							/* Modulation type */
	pStatus->ifFreq = 0;								/* in Hertz, IF freq. */
	pStatus->symbolRate = hImplChnDev->pChnStatus->Symbol_Rate;								/* in Baud */
	pStatus->symbolRateError = 0;	/* in Baud */
	pStatus->isFecLock = (hImplChnDev->pChnStatus->FLK==BADS_3x7x_ChnStatus_eLock)?1:0;	/* lock=1, unlock=0 */
	pStatus->isQamLock = (hImplChnDev->pChnStatus->QLK==BADS_3x7x_ChnStatus_eLock)?1:0;	/* lock=1, unlock=0 */
	pStatus->correctedCount = 0;							/* reset on every call to BADS_3x7x_P_Reset_Status() */
	pStatus->uncorrectedCount = 0;						/* reset on every call to BADS_3x7x_P_Reset_Status() */
	pStatus->snrEstimate = hImplChnDev->pChnStatus->SNRAVG;									/* in 1/256 dB */
	pStatus->agcIntLevel = 0;																					/* returns 0, moved to tuner*/
	pStatus->agcExtLevel = 0;																					/* returns 0, moved to tuner*/
	pStatus->carrierFreqOffset = 0;		/* in 1/1000 Hz */
	pStatus->carrierPhaseOffset = 0;			/* in 1/1000 Hz */
	pStatus->rxSymbolRate = 0;							/* in Baud, received symbol rate */
	pStatus->interleaveDepth = 0;		/* use in Docsis */
	pStatus->goodRsBlockCount = 0;							/* reset on every read */
	pStatus->berRawCount = 0;   							/* reset on every read */
	pStatus->dsChannelPower = 0;							/* in 10s of dBmV unit; OCAP DPM support for video channels */
	pStatus->mainTap = 0;									/* Channel main tap coefficient */
	pStatus->equalizerGain = 0;								/* Channel equalizer gain value in dBm */
	/* OCAP required postRsBER for all DS channels. postRsBER and elapsedTimeSec will be reset on every channel change*/
	/* fpostRsBER normally between xxxe-6 to xxxe-12 float value, to send this small float number over rMagnum to host, we convert it to uint32 using the formula*/
	pStatus->postRsBER = 0;						  /* returns 0, NO FLOAT in 7550*/
	                                    /* For Annex A  postRsBER = (11.224*uncorrectedCount)/(correctedCount+uncorrectedCount+cleanCount)*187*8)*/
	                                    /* For Annex B  postRsBER = (11.224*uncorrectedCount)/(correctedCount+uncorrectedCount+cleanCount)*122*7)*/

	pStatus->elapsedTimeSec = 0;							  /* returns 0, not needed*/
	pStatus->isSpectrumInverted = 0;				/* If 1, Spectrum is inverted relative to the AP bootup. */
	
	pStatus->cleanCount = 0;							/* reset on every call to BADS_3x7x_P_Reset_Status() */
	pStatus->agcAGFLevel = 0;										  /* in 1/10 percent */
	pStatus->agcAGF2Level = 0;										/* in 1/10 percent */
	pStatus->correctedBits = 0;							/* reset on every call to BADS_3x7x_P_Reset_Status() */

#endif
    BDBG_LEAVE(BADS_3x7x_GetStatus);
    return( retCode );
}


/*******************************************************************************
*   BADS_3x7x_GetLockStatus
*******************************************************************************/
BERR_Code BADS_3x7x_GetLockStatus(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_LockStatus *pLockStatus         /* [out] Returns lock status */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_3x7x_ChannelHandle hImplChnDev;

    BDBG_ENTER(BADS_3x7x_GetLockStatus);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BADS_3x7x_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );

	BADS_P_ChnStatus(hImplChnDev);
	*pLockStatus = hImplChnDev->pChnStatus->FLK;

    BDBG_LEAVE(BADS_3x7x_GetLockStatus);
    return( retCode );

}

/*******************************************************************************
*   BADS_3x7x_GetSoftDecision
*******************************************************************************/
BERR_Code BADS_3x7x_GetSoftDecision(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    int16_t nbrToGet,                   /* [in] Number values to get */
    int16_t *iVal,                      /* [out] Ptr to array to store output I soft decision */
    int16_t *qVal,                      /* [out] Ptr to array to store output Q soft decision */
    int16_t *nbrGotten                  /* [out] Number of values gotten/read */
    )
{
	BADS_3x7x_ChannelHandle hImplChnDev;
	BERR_Code retCode = BERR_SUCCESS;
	BSTD_UNUSED(nbrToGet);
    BDBG_ENTER(BADS_3x7x_GetSoftDecision);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BADS_3x7x_ChannelHandle) hChn->pImpl;
	BDBG_ASSERT( hImplChnDev );

	BADS_P_Read_Constellation(hImplChnDev, BADS_IQ_CONSTELLATION_VECTORS, iVal, qVal, nbrGotten); 

    BDBG_LEAVE(BADS_3x7x_GetSoftDecision);
    return( retCode );
}

 
/*******************************************************************************
*   BADS_3x7x_EnablePowerSaver
*******************************************************************************/
BERR_Code BADS_3x7x_EnablePowerSaver(
    BADS_ChannelHandle hChn             /* [in] Device channel handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_3x7x_ChannelHandle hImplChnDev;


    BDBG_ENTER(BADS_3x7x_EnablePowerSaver);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BADS_3x7x_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );

	/*BADS_P_PowerDown(hImplChnDev);*/

    BDBG_LEAVE(BADS_3x7x_EnablePowerSaver);
    return( retCode );
}

/*******************************************************************************
*   BADS_3x7x_DisablePowerSaver
*******************************************************************************/
BERR_Code BADS_3x7x_DisablePowerSaver(
    BADS_ChannelHandle hChn             /* [in] Device channel handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_3x7x_ChannelHandle hImplChnDev;


    BDBG_ENTER(BADS_3x7x_DisablePowerSaver);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BADS_3x7x_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );

	/*BADS_P_PowerUp(hImplChnDev);*/

    BDBG_LEAVE(BADS_3x7x_DisablePowerSaver);
    return( retCode );
}

/*******************************************************************************
*   BADS_3x7x_EnablePowerSaver
*******************************************************************************/
BERR_Code BADS_3x7x_ProcessNotification(
    BADS_ChannelHandle hChn,                /* [in] Device channel handle */
    unsigned int event                      /* [in] Event code and event data*/
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BSTD_UNUSED(hChn);
    BSTD_UNUSED(event);

    BDBG_WRN(("Not supported for this frontend chip."));
    return( retCode );
}



/*******************************************************************************
*   BADS_3x7x_InstallCallback
*******************************************************************************/
BERR_Code BADS_3x7x_InstallCallback(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_Callback callbackType,         /* [in] Type of callback */
    BADS_CallbackFunc pCallback,        /* [in] Function Ptr to callback */
    void *pParam                        /* [in] Generic parameter send on callback */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

	BADS_3x7x_ChannelHandle hImplChnDev = (BADS_3x7x_ChannelHandle) hChn->pImpl;
		BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );
   BDBG_ENTER(BADS_3x7x_InstallCallback);

   
    switch( callbackType )
    {
        case BADS_Callback_eLockChange:
            hImplChnDev->pCallback[callbackType] = pCallback;
            hImplChnDev->pCallbackParam[callbackType] = pParam;
            break;
		case BADS_Callback_eTuner:
            hImplChnDev->pCallback[callbackType] = pCallback;
            hImplChnDev->pCallbackParam[callbackType] = pParam;
            break;

        default:
            retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
            break;
    }

    BDBG_LEAVE(BADS_3x7x_InstallCallback);
    return( retCode );
}

/*******************************************************************************
*   BADS_3x7x_SetDaisyChain
*******************************************************************************/
BERR_Code BADS_3x7x_SetDaisyChain(
    BADS_Handle hDev,       /* [in] Returns handle */
    bool enableDaisyChain   /* [in] Eanble/disable daisy chain. */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_3x7x_Handle hImplDev;


    BDBG_ENTER(BADS_3x7x_SetDaisyChain);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    hImplDev = (BADS_3x7x_Handle) hDev->pImpl;  
    BDBG_ASSERT( hImplDev );
    
    hImplDev->isDaisyChain = enableDaisyChain;

    BDBG_LEAVE(BADS_3x7x_SetDaisyChain);
    return( retCode );
}
/*******************************************************************************
*   BADS_3x7x_GetDaisyChain
*******************************************************************************/
BERR_Code BADS_3x7x_GetDaisyChain(
    BADS_Handle hDev,           /* [in] Returns handle */
    bool *isEnableDaisyChain    /* [in] Eanble/disable daisy chain. */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_3x7x_Handle hImplDev;
    
    BDBG_ENTER(BADS_3x7x_GetDaisyChain);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    hImplDev = (BADS_3x7x_Handle) hDev->pImpl;  
    BDBG_ASSERT( hImplDev );
    
    *isEnableDaisyChain = hImplDev->isDaisyChain;

    BDBG_LEAVE(BADS_3x7x_GetDaisyChain);
    return( retCode );
}

/*******************************************************************************
*   BADS_3x7x_ResetStatus
*******************************************************************************/
BERR_Code BADS_3x7x_ResetStatus(
    BADS_ChannelHandle hChannel           /* [in] Returns handle */
    )
{
	BERR_Code retCode = BERR_SUCCESS;
	BADS_3x7x_ChannelHandle hImplChnDev = (BADS_3x7x_ChannelHandle) hChannel->pImpl;
	BDBG_ASSERT( hChannel );
    BDBG_ASSERT( hChannel->magicId == DEV_MAGIC_ID );

    
    BDBG_ENTER(BADS_3x7x_ResetStatus);
	BADS_P_ChnStatusReset(hImplChnDev);

    BDBG_LEAVE(BADS_3x7x_ResetStatus);
    return( retCode );
}


/*******************************************************************************
*   BADS_3x7x_GetInterruptEventHandle
*******************************************************************************/
BERR_Code BADS_3x7x_GetInterruptEventHandle(
   BADS_Handle hDev,            /* [in] BADS handle */
   BKNI_EventHandle *phEvent /* [out] event handle */
)
{

	BERR_Code retCode = BERR_SUCCESS;
    BADS_3x7x_Handle hImplDev;
    
    BDBG_ENTER(BADS_3x7x_GetInterruptEventHandle);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    hImplDev = (BADS_3x7x_Handle) hDev->pImpl;  
    BDBG_ASSERT( hImplDev );
	*phEvent = hImplDev->hIntEvent;


    BDBG_LEAVE(BADS_3x7x_GetInterruptEventHandle);
    return( retCode );
}

/*******************************************************************************
*   BADS_3x7x_ProcessInterruptEvent
*******************************************************************************/
BERR_Code BADS_3x7x_ProcessInterruptEvent(
    BADS_Handle hDev           /* [in] Returns handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_3x7x_Handle hImplDev;
	BADS_3x7x_ChannelHandle hImplChnDev;

    
    BDBG_ENTER(BADS_3x7x_ProcessInterruptEvent);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    hImplDev = (BADS_3x7x_Handle) hDev->pImpl;  
    BDBG_ASSERT( hImplDev );
	hImplChnDev = (BADS_3x7x_ChannelHandle)(hImplDev->hAdsChn[0]);

	BDBG_MSG(("BADS_3x7x_ProcessInterruptEvent"));
	BTMR_StopTimer(hImplDev->hTimer);  /* the timer is in Micro second */
	BKNI_ResetEvent(hImplDev->hIntEvent);
	BADS_P_Acquire(hImplChnDev);
	BTMR_StartTimer(hImplDev->hTimer, 25000);   /* the timer is in Micro second */


    BDBG_LEAVE(BADS_3x7x_ProcessInterruptEvent);
    return( retCode );
}

/*******************************************************************************
*   BADS_3x7x_GetChannelDefaultSettings
*******************************************************************************/
BERR_Code BADS_3x7x_GetChannelDefaultSettings(
    BADS_Handle hDev,                       /* [in] Device handle */
    unsigned int channelNo,                 /* [in] Channel number to default setting for */
    BADS_ChannelSettings *pChnDefSettings   /* [out] Returns channel default setting */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_3x7x_Handle hImplDev;

    BDBG_ENTER(BADS_3x7x_GetChannelDefaultSettings);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    hImplDev = (BADS_3x7x_Handle) hDev->pImpl;
    BDBG_ASSERT( hImplDev );

    switch( channelNo )
    {
        case 0:
        if( channelNo < hImplDev->mxChnNo )
        {
            pChnDefSettings->ifFreq = 0;    /*The if freq for 3x7x is set to 0 as the internal tuner does not spit out seperate if frequency. */
            pChnDefSettings->autoAcquire = true;
            pChnDefSettings->fastAcquire = false;
            break;
        }
        default:
            retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
            break;
    }

    BDBG_LEAVE(BADS_3x7x_GetChannelDefaultSettings);
    return( retCode );
}

/*******************************************************************************
*   BADS_3x7x_Untune
*******************************************************************************/
BERR_Code BADS_3x7x_Untune(
    BADS_Handle hDev           /* [in] Returns handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_3x7x_Handle hImplDev;

    BDBG_ENTER(BADS_3x7x_Untune);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    hImplDev = (BADS_3x7x_Handle) hDev->pImpl;  
    BDBG_ASSERT( hImplDev );

    BTMR_StopTimer(hImplDev->hTimer);  /* the timer is in Micro second */
    hImplDev->timerStopped = true;

    BDBG_LEAVE(BADS_3x7x_Untune);
    return( retCode );
}













