/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bthd.c $
 * $brcm_Revision: Hydra_Software_Devel/22 $
 * $brcm_Date: 10/18/12 5:14p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/thd/src/common/bthd.c $
 * 
 * Hydra_Software_Devel/22   10/18/12 5:14p atanugul
 * SW3461-269: Merge to main
 * 
 * Hydra_Software_Devel/SW3461-252/2   10/17/12 12:05p vishk
 * SW3461-270: Add API Support for Reading New Front-End 312X, 314X, 346X
 * F/W Version IDs
 * 
 * Hydra_Software_Devel/SW3461-252/1   9/28/12 5:03p atanugul
 * SW3461-269: Add support for BTHD_GetVersionInfo()
 * 
 * Hydra_Software_Devel/21   2/2/12 6:23p atanugul
 * SW3461-141: Implement new apis for SetAcquireParams, GetAcquireParams
 * and PowerUp
 * 
 * Hydra_Software_Devel/20   11/16/11 12:06p atanugul
 * SW3461-79: Add No Signal to GetThdLockStatus()
 * 
 * Hydra_Software_Devel/19   6/1/11 3:39p atanugul
 * SW3461-5: Add THD support to 3461
 * 
 * Hydra_Software_Devel/18   4/27/11 12:14p farshidf
 * SWDTV-6190: add the Nexus BBS interface
 * 
 * Hydra_Software_Devel/17   5/18/10 3:51p farshidf
 * SW7550-440: add GetThdLockStatus
 * 
 * Hydra_Software_Devel/SW7550-399/1   5/6/10 6:15p farshidf
 * SW7550-399: add the mini tune while we are lock
 * 
 * Hydra_Software_Devel/16   11/6/09 5:53p farshidf
 * SW7550-38: match Nexus changes
 * 
 * Hydra_Software_Devel/15   11/5/09 3:32p farshidf
 * SW7550-38: compile issue
 * 
 * Hydra_Software_Devel/14   11/5/09 2:09p farshidf
 * SW7550-38: add the API changes for 7550
 * PR41450: move down the timer settings
 * PR41450: remove Dynamic settings
 * PR41450: add the update Aquire param settings
 * PR57150: Implement THD under Nexus without the AP
 * 
 * Hydra_Software_Devel/PR57150/5   11/5/09 2:06p farshidf
 * SW7550-38: add the API changes for 7550
 * 
 * Hydra_Software_Devel/PR57150/4   11/1/09 2:13p farshidf
 * PR41450: move down the timer settings
 * 
 * Hydra_Software_Devel/PR57150/3   10/8/09 6:13p farshidf
 * PR41450: remove Dynamic settings
 * 
 * Hydra_Software_Devel/PR57150/2   9/30/09 3:30p farshidf
 * PR41450: add the update Aquire param settings
 * 
 * Hydra_Software_Devel/PR57150/1   7/28/09 2:14p farshidf
 * PR57150: Implement THD under Nexus without the AP
 * 
 * Hydra_Software_Devel/13   9/29/08 9:52a dliu
 * PR46539: Add dynamic power management
 * 
 * Hydra_Software_Devel/12   8/13/08 5:04p dliu
 * PR38944: Change SetIfdAudioSettings to pass in const pointer
 * 
 * Hydra_Software_Devel/11   8/8/08 2:54p dliu
 * PR38944: Change SetIfdAudioSettings function to use pointers
 * 
 * Hydra_Software_Devel/10   8/6/08 4:54p dliu
 * PR38944: Add extran functions and parameters for AASD
 * 
 * Hydra_Software_Devel/9   7/30/08 2:31p dliu
 * PR45192: Change function name
 * 
 * Hydra_Software_Devel/8   7/28/08 4:01p dliu
 * PR45192: Add GetDefault function to api list
 * 
 * Hydra_Software_Devel/7   6/24/08 3:25p dliu
 * PR38944: Change parameters for GetSoftDecisionBuf to signed values
 * 
 * Hydra_Software_Devel/6   5/31/08 12:23p dliu
 * PR38934: Change argument of GetSoftDecisionBuffer to uint_16
 * 
 * Hydra_Software_Devel/5   5/15/08 12:34p dliu
 * PR38934: Add IFD Status function and GetSoftDecisionBuf function
 * 
 * Hydra_Software_Devel/4   2/27/08 10:10a dliu
 * PR36603: Add function to get lockchange event handle
 * 
 * Hydra_Software_Devel/3   11/6/07 5:36p dliu
 * PR36603: Add GetVersion function
 * 
 * Hydra_Software_Devel/2   11/5/07 3:43p dliu
 * PR36603: Change file header
 * 
 * Hydra_Software_Devel/1   11/5/07 3:40p dliu
 * PR36603: Initial version
 *
 ***************************************************************************/
#include "bstd.h"
#include "bthd.h"
#include "bthd_priv.h"

BDBG_MODULE(bTHD);

/******************************************************************************
 BTHD_Open()
******************************************************************************/
BERR_Code BTHD_Open(
   BTHD_Handle *h,         /* [out] BTHD handle */
   BCHP_Handle hChip,      /* [in] chip handle */
   void        *pReg,      /* [in] pointer to register or i2c handle */
   BINT_Handle hInterrupt, /* [in] Interrupt handle */   
   const BTHD_Settings *pDefSettings /* [in] default settings */
)
{ 
   BDBG_ASSERT(h);
   BDBG_ASSERT(pReg);
   BDBG_ASSERT(pDefSettings);
   
   return (pDefSettings->api.Open(h, hChip, pReg, hInterrupt, pDefSettings));
}


/******************************************************************************
 BTHD_Close()
******************************************************************************/
BERR_Code BTHD_Close(
   BTHD_Handle h   /* [in] BTHD handle */
)
{
   BDBG_ASSERT(h);
   return (h->settings.api.Close(h));
}

										    
/******************************************************************************
 BTHD_Init()
******************************************************************************/
BERR_Code BTHD_Init(
   BTHD_Handle h,         /* [in] BTHD handle */
   const uint8_t *pImage, /* [in] pointer to the microcode image. Set to NULL to use default image */
   uint32_t imageLength   /* [in] length of microcode image. Set to 0 when using default image */
)
{
   BDBG_ASSERT(h);
   return (h->settings.api.Init(h, pImage, imageLength));
}

/******************************************************************************
 BTHD_ReadRegister()
******************************************************************************/
BERR_Code BTHD_ReadRegister(
   BTHD_Handle h,    /* [in] BTHD handle */
   uint32_t    reg,  /* [in] address of register to read */
   uint32_t    *val  /* [in] contains data that was read */
)
{
   BDBG_ASSERT(h);
   return (h->settings.api.ReadRegister(h, reg, val));
}


/******************************************************************************
 BTHD_WriteRegister()
******************************************************************************/
BERR_Code BTHD_WriteRegister(
   BTHD_Handle h,    /* [in] BTHD handle */
   uint32_t    reg,  /* [in] address of register to read */
   uint32_t    *val  /* [in] contains data that was read */
)
{
   BDBG_ASSERT(h);
   return (h->settings.api.WriteRegister(h, reg, val));
}


/******************************************************************************
 BTHD_Mi2cWrite()
******************************************************************************/ 
BERR_Code BTHD_Mi2cWrite(
   BTHD_Handle h,      /* [in] BTHD handle */
   uint8_t slave_addr, /* [in] address of the i2c slave device */
   uint8_t *buf,       /* [in] specifies the data to transmit */
   uint8_t n           /* [in] number of bytes to transmit after the i2c slave address */
)
{
   BDBG_ASSERT(h);
   if (h->settings.api.Mi2cWrite)
      return (h->settings.api.Mi2cWrite(h, slave_addr, buf, n));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BTHD_Mi2cRead()
******************************************************************************/ 
BERR_Code BTHD_Mi2cRead(
   BTHD_Handle h,      /* [in] BTHD handle */
   uint8_t slave_addr, /* [in] address of the i2c slave device */
   uint8_t *out_buf,   /* [in] specifies the data to transmit before the i2c restart condition */
   uint8_t out_n,      /* [in] number of bytes to transmit before the i2c restart condition not including the i2c slave address */
   uint8_t *in_buf,    /* [out] holds the data read */
   uint8_t in_n        /* [in] number of bytes to read after the i2c restart condition not including the i2c slave address */
)
{
   BDBG_ASSERT(h);
   if (h->settings.api.Mi2cRead)
      return (h->settings.api.Mi2cRead(h, slave_addr, out_buf, out_n, in_buf, in_n));
   else
      return BERR_NOT_SUPPORTED;  
}

/******************************************************************************
 BTHD_AcquireIfd()
******************************************************************************/
BERR_Code BTHD_AcquireIfd(
	BTHD_Handle h,					/* [in] BTHD handle */
	const BTHD_IfdParams *pParams   /* [in] IFD acqusition parameters */
	)
{
	BDBG_ASSERT(h);
	return (h->settings.api.AcquireIfd(h, pParams));
}

/******************************************************************************
 BTHD_GetIfdStatus()
******************************************************************************/
BERR_Code BTHD_GetIfdStatus(
	BTHD_Handle h,                  /* [in] BTHD handle */
	BTHD_IfdStatus *pIfdStatus		/* [out] IFD Status structure */
	)
{
	BDBG_ASSERT(h);
	return (h->settings.api.GetIfdStatus(h, pIfdStatus));
}

/******************************************************************************
 BTHD_SetAcquireParams()
******************************************************************************/
BERR_Code BTHD_SetAcquireParams(
   BTHD_Handle h,      /* [in] BTHD handle */
   const BTHD_InbandParams *pParams  /* [in] inband acquisition parameters */
)
{   
   BDBG_ASSERT(h);
   return (h->settings.api.SetAcquireParams(h, pParams));
}

/******************************************************************************
 BTHD_GetAcquireParams()
******************************************************************************/
BERR_Code BTHD_GetAcquireParams(
   BTHD_Handle h,      /* [in] BTHD handle */
   BTHD_InbandParams *pParams  /* [out] inband acquisition parameters */  
)
{   
   BDBG_ASSERT(h);
   return (h->settings.api.GetAcquireParams(h, pParams));
}

/******************************************************************************
 BTHD_TuneAcquire()
******************************************************************************/
BERR_Code BTHD_TuneAcquire(
   BTHD_Handle h,                    /* [in] BTHD handle */
   const BTHD_InbandParams *pParams  /* [in] inband acquisition parameters */
)
{   
   BDBG_ASSERT(h);
   return (h->settings.api.TuneAcquire(h, pParams));
}

/******************************************************************************
 BTHD_RequestThdAsyncStatus()
******************************************************************************/
BERR_Code BTHD_RequestThdAsyncStatus(
   BTHD_Handle h /* [in] BTHD handle */
)
{
   BDBG_ASSERT(h);
   return (h->settings.api.RequestThdAsyncStatus(h));
}

/******************************************************************************
 BTHD_GetThdAsyncStatus()
******************************************************************************/
BERR_Code BTHD_GetThdAsyncStatus(
   BTHD_Handle h,           /* [in] BTHD handle */
   BTHD_THDStatus *pStatus  /* [out] THD status */
)
{
   BDBG_ASSERT(h);
   return (h->settings.api.GetThdAsyncStatus(h, pStatus));
}

/******************************************************************************
 BTHD_GetThdStatus()
******************************************************************************/
BERR_Code BTHD_GetThdStatus(
   BTHD_Handle h,           /* [in] BTHD handle */
   BTHD_THDStatus *pStatus  /* [out] THD status */
)
{
   BDBG_ASSERT(h);
   return (h->settings.api.GetThdStatus(h, pStatus));
}

/******************************************************************************
 BTHD_ResetInbandStatus()
******************************************************************************/
BERR_Code BTHD_ResetInbandStatus(
   BTHD_Handle h /* [in] BTHD handle */
)
{
   BDBG_ASSERT(h);
   return (h->settings.api.ResetInbandStatus(h));
}

/******************************************************************************
 BTHD_GetInterruptEventHandle()
******************************************************************************/
BERR_Code BTHD_GetInterruptEventHandle(
   BTHD_Handle h,            /* [in] BTHD handle */
   BKNI_EventHandle *hEvent  /* [out] interrupt event handle */
)
{
   BDBG_ASSERT(h);
   if (h->settings.api.GetInterruptEventHandle)
      return (h->settings.api.GetInterruptEventHandle(h, hEvent));
   else
      return BERR_NOT_SUPPORTED;            
}


/******************************************************************************
 BTHD_GetInterruptEventHandle()
******************************************************************************/
BERR_Code BTHD_GetBBSInterruptEventHandle(
   BTHD_Handle h,            /* [in] BTHD handle */
   BKNI_EventHandle *hEvent  /* [out] interrupt event handle */
)
{
   BDBG_ASSERT(h);
   if (h->settings.api.GetBBSInterruptEventHandle)
      return (h->settings.api.GetBBSInterruptEventHandle(h, hEvent));
   else
      return BERR_NOT_SUPPORTED;            
}

/******************************************************************************
 BTHD_HandleInterrupt_isr()
******************************************************************************/
BERR_Code BTHD_HandleInterrupt_isr(
   BTHD_Handle h /* [in] BTHD handle */
)
{
   BDBG_ASSERT(h);
   return (h->settings.api.HandleInterrupt_isr(h));
}

/******************************************************************************
 BTHD_ProcessInterruptEvent()
******************************************************************************/
BERR_Code BTHD_ProcessInterruptEvent(
   BTHD_Handle h  /* [in] BTHD handle */
)
{
   BDBG_ASSERT(h);
   return (h->settings.api.ProcessInterruptEvent(h));
}


/******************************************************************************
 BTHD_ProcessInterruptEvent()
******************************************************************************/
BERR_Code BTHD_ProcessBBSInterruptEvent(
   BTHD_Handle h  /* [in] BTHD handle */
)
{
   BDBG_ASSERT(h);
   return (h->settings.api.ProcessBBSInterruptEvent(h));
}


/******************************************************************************
 BTHD_GetSettings()
******************************************************************************/
BERR_Code BTHD_GetSettings(
   BTHD_Handle h,           /* [in] BTHD handle */
   BTHD_Settings *pSettings /* [out] current BTHD settings */
)
{
   BDBG_ASSERT(pSettings);
   BKNI_Memcpy((void*)pSettings, (void*)&(h->settings), sizeof(BTHD_Settings));
   return BERR_SUCCESS;
}


/******************************************************************************
   BTHD_SetSettings()
******************************************************************************/
BERR_Code BTHD_SetSettings(
   BTHD_Handle h,           /* [in] BTHD handle */
   BTHD_Settings *pSettings /* [in] new BTHD settings */
)
{
   BDBG_ASSERT(pSettings);
   BKNI_Memcpy((void*)&(h->settings), (void*)pSettings, sizeof(BTHD_Settings));
   return BERR_SUCCESS;
}

/******************************************************************************
 BTHD_GetChipRevision
******************************************************************************/
BERR_Code BTHD_GetChipRevision(
   BTHD_Handle h,           /* [in] BTHD handle */
   uint8_t *revision        /* [out] chip revision*/
)
{
   BDBG_ASSERT(h);
   return (h->settings.api.GetChipRevision(h, revision));
}

/******************************************************************************
 BTHD_GetVersion
******************************************************************************/
BERR_Code BTHD_GetVersion(
   BTHD_Handle h,           /* [in] BTHD handle */
   uint32_t *revision,      /* [out] firmware revision */
   uint32_t *checksum       /* [out] firmware checksum */
)
{
   BDBG_ASSERT(h);
   return (h->settings.api.GetVersion(h, revision, checksum));
}

/******************************************************************************
 BTHD_GetVersionInfo
******************************************************************************/
BERR_Code BTHD_GetVersionInfo(
    BTHD_Handle h,                 /* [in] BTHD handle  */
    BFEC_VersionInfo *pVersionInfo /* [out] Returns version Info */
)
{
   BDBG_ASSERT(h);
   return (h->settings.api.GetVersionInfo(h, pVersionInfo));
}

/******************************************************************************
 BTHD_GetLockStateChangeEventHandle()
******************************************************************************/
BERR_Code BTHD_GetLockStateChangeEventHandle(
   BTHD_Handle h,            /* [in] BTHD handle */
   BKNI_EventHandle *hEvent  /* [out] lock event handle */
)
{
   BDBG_ASSERT(h);
   return (h->settings.api.GetLockStateChangeEventHandle(h, hEvent));
}
/******************************************************************************
 BTHD_GetSoftDecisionBuf()
******************************************************************************/
BERR_Code BTHD_GetSoftDecisionBuf(
   BTHD_Handle h,  /* [in] BTHD handle */
   int16_t *pI,    /* [out] 30 I-values */
   int16_t *pQ     /* [out] 30 Q-values */
)
{
	BDBG_ASSERT(h);
	return (h->settings.api.GetSoftDecisionBuf(h, pI, pQ));
}

/******************************************************************************
 BTHD_GetDefaultInbandParam()
******************************************************************************/
BERR_Code BTHD_GetDefaultInbandParams(
    BTHD_Handle h,            /* [in] BTHD handle */         
    BTHD_InbandParams* pDefInbandParam /* [out] default param */
)
{
    return (h->settings.api.GetDefaultInbandParams(pDefInbandParam));
}

/******************************************************************************
 BTHD_GetDefaultIfdParam()
******************************************************************************/
BERR_Code BTHD_GetDefaultIfdParams(
    BTHD_Handle h,            /* [in] BTHD handle */         
    BTHD_IfdParams* pDefIfdParam /* [out] default param */
)
{
    return (h->settings.api.GetDefaultIfdParams(pDefIfdParam));
}

/******************************************************************************
 BTHD_ResetInbandStatus()
******************************************************************************/
BERR_Code BTHD_ResetIfdStatus(
   BTHD_Handle h /* [in] BTHD handle */
)
{
   BDBG_ASSERT(h);
   return (h->settings.api.ResetIfdStatus(h));
}

/******************************************************************************
 BTHD_GetIfdAudioSettings()
******************************************************************************/
BERR_Code BTHD_GetIfdAudioSettings(
    BTHD_Handle h,                          /* [in] BTHD handle */
    BTHD_IfdAudioSettings* audioSettings    /* [out] Audio settings structure */
)
{
    BDBG_ASSERT(h);
    return (h->settings.api.GetIfdAudioSettings(h, audioSettings));
}

/******************************************************************************
 BTHD_GetIfdAudioSettings()
******************************************************************************/
BERR_Code BTHD_SetIfdAudioSettings(
    BTHD_Handle h,                         /* [in] BTHD handle */
    const BTHD_IfdAudioSettings* audioSettings    /* [in] Audio settings structure */
)
{
    BDBG_ASSERT(h);
    return (h->settings.api.SetIfdAudioSettings(h, audioSettings));
}

/******************************************************************************
 BTHD_PowerEnable()
******************************************************************************/
BERR_Code BTHD_PowerUp(
    BTHD_Handle h                         /* [in] BTHD handle */
)
{
    BDBG_ASSERT(h);
    return (h->settings.api.PowerUp(h));
}

/******************************************************************************
 BTHD_PowerDisable()
******************************************************************************/
BERR_Code BTHD_PowerDown(
    BTHD_Handle h                         /* [in] BTHD handle */
)
{
    BDBG_ASSERT(h);
    return (h->settings.api.PowerDown(h));
}

/******************************************************************************
 BTHD_GetEWSEventHandle()
******************************************************************************/
BERR_Code BTHD_GetEWSEventHandle(
   BTHD_Handle h,            /* [in] BTHD handle */
   BKNI_EventHandle *hEvent  /* [out] lock event handle */
)
{
   BDBG_ASSERT(h);
   return (h->settings.api.GetEWSEventHandle(h, hEvent));
}

/******************************************************************************
 BTHD_GetThdLockStatus()
******************************************************************************/
BERR_Code BTHD_GetThdLockStatus(
   BTHD_Handle h,            /* [in] BTHD handle */
   BTHD_LockStatus *pLockStatus  /* [out] THD status */
)
{
   BDBG_ASSERT(h);
   return (h->settings.api.GetThdLockStatus(h, pLockStatus));
}

/******************************************************************************
 BTHD_InstallCallback()
******************************************************************************/
BERR_Code BTHD_InstallCallback(
    BTHD_Handle h,                      /* [in] Device channel handle */
    BTHD_Callback callbackType,         /* [in] Type of callback */
    BTHD_CallbackFunc pCallback,        /* [in] Function Ptr to callback */
    void *pParam                        /* [in] Generic parameter send on callback */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BTHD_InstallCallback);
    BDBG_ASSERT( h );
    
    if( h->settings.api.InstallCallback != NULL )
    {
        retCode = h->settings.api.InstallCallback( h, callbackType, pCallback, pParam );
    }
    else
    {
        BDBG_WRN(("BTHD_InstallCallback: Funtion Ptr is NULL"));
    }

    BDBG_LEAVE(BTHD_InstallCallback);
    return( retCode );
}
