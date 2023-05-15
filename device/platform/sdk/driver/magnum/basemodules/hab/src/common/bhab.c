/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhab.c $
 * $brcm_Revision: Hydra_Software_Devel/18 $
 * $brcm_Date: 10/18/12 5:15p $
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/hab/src/common/bhab.c $
 * 
 * Hydra_Software_Devel/18   10/18/12 5:15p atanugul
 * SW3461-269: Merge to main
 * 
 * Hydra_Software_Devel/17   10/18/12 3:42p atanugul
 * SW3128-228: Add API Support for Reading New Front-End 312X, 314X, 346X
 * F/W Version IDs
 * 
 * Hydra_Software_Devel/15   9/28/12 1:38p atanugul
 * SW3128-228: Add support for BHAB_GetVersionInfo()
 * 
 * Hydra_Software_Devel/14   11/9/11 2:57p atanugul
 * SW3128-68: Add BHAB_ReadSlave() and BHAB_ReadSlave() API to 3128
 * 
 * Hydra_Software_Devel/13   11/2/11 4:42p atanugul
 * SW3461-3: Update BHAB_GetApVersion to include familyId, AP major
 * version, minor version and remove script version and configuration
 * version
 * 
 * Hydra_Software_Devel/12   10/18/11 3:20p atanugul
 * SW3461-67: change write_len, read_len and command_len in
 * BHAB_SendHabCommand() from type uint8_t to uint16_t
 * 
 * Hydra_Software_Devel/11   5/27/11 4:27p atanugul
 * SW3461-3: Added BHAB_GetConfigSettings() and BHAB_SetConfigSettings()
 * 
 * Hydra_Software_Devel/10   3/2/11 6:07p atanugul
 * SW3128-2: Add Support for BCM3128
 * 
 * Hydra_Software_Devel/9   2/1/11 5:40p VISHK
 * SW7422-171: Nexus support for the 3128 (8 ADS core + tuner on daughter
 * board) plug on 7422 board  Read and write memory modified to 32 bit
 * addressing to support 3128 chipset as it has 32 bit memory addressing.
 * 
 * Hydra_Software_Devel/8   10/11/10 5:23p jgarrett
 * SW7125-630: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7125-630/1   10/1/10 5:47p jgarrett
 * SW7125-630: Adding debug checks
 * 
 * Hydra_Software_Devel/7   6/22/10 6:02p vishk
 * SW7420-839: Add support to program AGC gain to 31xx TNR PI
 * 
 * Hydra_Software_Devel/6   10/5/09 10:44a vishk
 * SW7405-2851: 31xx: Add support to configure the Watchdog feature of
 * 31xx.
 * 
 * Hydra_Software_Devel/5   1/29/09 7:16p vishk
 * PR 46431: ADS PI: Add support for BCM3117, BCM3114, and BCM3112.
 * PR 46436: AOB PI: Add support for BCM3117 and BCM3114
 * 
 * Hydra_Software_Devel/4   1/17/09 2:35p vishk
 * PR 46431: ADS PI: Add support for BCM3117, BCM3114, and BCM3112.
 * PR 46436: AOB PI: Add support for BCM3117 and BCM3114.
 * 
 * Added comment.
 * 
 * Hydra_Software_Devel/3   1/16/09 7:11p vishk
 * PR 46431: ADS PI: Add support for BCM3117, BCM3114, and BCM3112.
 * PR 46436: AOB PI: Add support for BCM3117 and BCM3114
 * 
 * Hydra_Software_Devel/2   10/27/08 11:11a gmohile
 * PR 47386 : Add temination byte for send hab command
 * 
 * Hydra_Software_Devel/1   10/20/08 2:52p gmohile
 * PR 47386 : Checkin initial version
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bi2c.h"
#include "bhab.h"
#include "bhab_priv.h"

BDBG_MODULE(BHAB);

/******************************************************************************
 BHAB_Open()
******************************************************************************/
BERR_Code BHAB_Open(
	BHAB_Handle *handle,     /* [out] BHAB handle */
	void        *pReg,       /* [in] pointer ot i2c or spi handle */
	const BHAB_Settings *pDefSettings /* [in] Default Settings */
)
{

	BDBG_ASSERT(handle);
	BDBG_ASSERT(pReg);
	BDBG_ASSERT(pDefSettings);

	return (pDefSettings->api.Open(handle, pReg, pDefSettings));	
}


/******************************************************************************
 BHAB_Close()
******************************************************************************/
BERR_Code BHAB_Close(BHAB_Handle handle)
{

	BDBG_ASSERT(handle);
	return (handle->settings.api.Close(handle));  	
}


/******************************************************************************
 BHAB_3520_P_InitAp()
******************************************************************************/
BERR_Code BHAB_InitAp(
	BHAB_Handle handle,       /* [in] BHAB handle */
	const uint8_t *pHexImage  /* [in] pointer to BCM3520 microcode image */
)
{
	BDBG_ASSERT(handle);
	return (handle->settings.api.InitAp(handle, pHexImage)); 	
}


/******************************************************************************
 BHAB_GetApStatus()
******************************************************************************/
BERR_Code BHAB_GetApStatus(
	BHAB_Handle handle,      /* [in] HAB device handle */
	BHAB_ApStatus *pStatus   /* [out] AP status */
)
{
	BDBG_ASSERT(handle);
	return (handle->settings.api.GetApStatus(handle, pStatus));	
}


/******************************************************************************
 BHAB_GetApVersion()
******************************************************************************/
BERR_Code BHAB_GetApVersion(
    BHAB_Handle handle,     /* [in] BHAB handle */
    uint32_t    *pFamilyId, /* [out] Chip Family id */    
    uint32_t    *pChipId,   /* [out] BHAB chip ID */
    uint16_t    *pChipVer,  /* [out] chip revision number */
    uint8_t     *pMajApVer, /* [out] AP microcode major version */
    uint8_t     *pMinApVer  /* [out] AP microcode minor version */    
)
{
	BDBG_ASSERT(handle);
	return (handle->settings.api.GetApVersion(handle, pFamilyId, pChipId, pChipVer, pMajApVer, pMinApVer));	
}

/******************************************************************************
 BHAB_GetVersionInfo()
******************************************************************************/
BERR_Code BHAB_GetVersionInfo(
    BHAB_Handle             handle,         /* [in]  BHAB handle */
    BFEC_SystemVersionInfo  *pVersionInfo   /* [out]  Returns FW version information */
)
{
	BDBG_ASSERT(handle);
	return (handle->settings.api.GetVersionInfo(handle, pVersionInfo));	
}

/******************************************************************************
 BHAB_ReadRegister()
******************************************************************************/
BERR_Code BHAB_ReadRegister(
	BHAB_Handle handle,     /* [in] BHAB handle */
	uint32_t    reg,   /* [in] address of register to read */
	uint32_t    *val   /* [in] contains data that was read */
)
{
	BDBG_ASSERT(handle);
	return (handle->settings.api.ReadRegister(handle, reg, val));   
}


/******************************************************************************
 BHAB_WriteRegister()
******************************************************************************/
BERR_Code BHAB_WriteRegister(
	BHAB_Handle handle,     /* [in] BHAB handle */
	uint32_t    reg,   /* [in] address of register to read */
	uint32_t    *val   /* [in] contains data that was read */
)
{
	BDBG_ASSERT(handle);
	return (handle->settings.api.WriteRegister(handle, reg, val));   
}


/******************************************************************************
 BHAB_ReadMemory()
******************************************************************************/
BERR_Code BHAB_ReadMemory(BHAB_Handle handle, uint32_t addr, uint8_t *buf, uint32_t n)
{   
	BDBG_ASSERT(handle);
	return (handle->settings.api.ReadMemory(handle, addr, buf, n));   	
}


/******************************************************************************
 BHAB_WriteMemory()
******************************************************************************/
BERR_Code BHAB_WriteMemory(BHAB_Handle handle, uint32_t addr, uint8_t *buf, uint32_t n)
{   
	BDBG_ASSERT(handle);
	return (handle->settings.api.WriteMemory(handle, addr, buf, n));   
}


/******************************************************************************
 BHAB_ReadMbox()
******************************************************************************/
BERR_Code BHAB_ReadMbox(
	BHAB_Handle handle,    /* [in] BHAB PI Handle */
	uint32_t    reg,  /* [in] RBUS register address */
	uint32_t    *val  /* [out] value read from register */
)
{   
	BDBG_ASSERT(handle);
	return (handle->settings.api.ReadMbox(handle, reg, val)); 
}


/******************************************************************************
 BHAB_WriteMbox()
******************************************************************************/
BERR_Code BHAB_WriteMbox(
	BHAB_Handle handle,    /* [in] BHAB PI Handle */
	uint32_t    reg,  /* [in] RBUS register address */
	uint32_t    *val  /* [in] value to write */
)
{	
	BDBG_ASSERT(handle);
	return (handle->settings.api.WriteMbox(handle, reg, val)); 
}


/******************************************************************************
 BHAB_HandleInterrupt_isr()
******************************************************************************/
BERR_Code BHAB_HandleInterrupt_isr(
	BHAB_Handle handle /* [in] BHAB handle */
)
{
	BDBG_ASSERT(handle);
    BKNI_ASSERT_ISR_CONTEXT();
	return (handle->settings.api.HandleInterrupt_isr(handle)); 
}


/******************************************************************************
 BHAB_ProcessInterruptEvent()
******************************************************************************/
BERR_Code BHAB_ProcessInterruptEvent(
	BHAB_Handle handle  /* [in] BHAB handle */
)
{
	BDBG_ASSERT(handle);
	return (handle->settings.api.ProcessInterruptEvent(handle)); 
}


/******************************************************************************
 BHAB_EnableLockInterrupt()
******************************************************************************/ 
BERR_Code BHAB_EnableLockInterrupt(
	BHAB_Handle handle,  /* [in] BHAB handle */
	BHAB_DevId eDevId,    /* [in] Device ID */
	bool bEnable   /* [in] true = enable Lock  interrupt */
)
{
	BDBG_ASSERT(handle);
	return (handle->settings.api.EnableLockInterrupt(handle, eDevId, bEnable)); 
}
	

/******************************************************************************
 BHAB_InstallInterruptCallback()
******************************************************************************/ 
BERR_Code BHAB_InstallInterruptCallback(
	BHAB_Handle handle,  /* [in] BHAB handle */
	BHAB_DevId eDevId,    /* [in] Device ID */
	BHAB_IntCallbackFunc fCallBack,
	void * pParm1, 
	int parm2
)
{
	BDBG_ASSERT(handle);
	return (handle->settings.api.InstallInterruptCallback(handle, eDevId, fCallBack, pParm1, parm2)); 
}

/******************************************************************************
 BHAB_UnInstallInterruptCallback()
******************************************************************************/ 
BERR_Code BHAB_UnInstallInterruptCallback(
	BHAB_Handle handle,  /* [in] BHAB handle */
	BHAB_DevId eDevId    /* [in] Device ID */
)
{
	BDBG_ASSERT(handle);
	return (handle->settings.api.UnInstallInterruptCallback(handle, eDevId)); 
}	
	

/******************************************************************************
 BHAB_SendHabCommand()
******************************************************************************/
BERR_Code BHAB_SendHabCommand(
	BHAB_Handle handle, /* [in] BHAB PI Handle */
	uint8_t *write_buf, /* [in] specifies the HAB command to send */
	uint16_t write_len,  /* [in] number of bytes of the HAB command to be written using either I2c or SPI. */ 
	uint8_t *read_buf,  /* [out] holds the data read from the HAB */ 
	uint16_t read_len,   /* [in] number of bytes to read from the HAB */
	bool bCheckForAck,  /* [in] true = determine if the AP has serviced the command */
	bool bInsertTermination, /* [in] true = insert termination byte 0x00 in write buffer at read_len position */
	uint16_t command_len /* [in] Total number of bytes in the HAB command so that the termination byte is inserted at the right HAB location. */
)
{
	BDBG_ASSERT(handle);
	return (handle->settings.api.SendHabCommand(handle, write_buf, write_len, read_buf, read_len, bCheckForAck, bInsertTermination, command_len)); 
}
/******************************************************************************
 BHAB_GetInterruptEventHandle()
******************************************************************************/
BERR_Code BHAB_GetInterruptEventHandle(
	BHAB_Handle handle,     /* [out] BHAB handle */
	BKNI_EventHandle *event
)
{

	BDBG_ASSERT(handle);

	return (handle->settings.api.GetInterruptEventHandle(handle, event));		
}

/******************************************************************************
Summary:
   Gets the watch dog timer configuration.
   
Description:

Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_GetWatchDogTimer(BHAB_Handle handle,      /* [in] BHAB PI Handle */ 
	BHAB_WatchDogTimerSettings *wdtSettings)
{
	BDBG_ASSERT(handle);
	
	return (handle->settings.api.GetWatchDogTimer(handle, wdtSettings));
}

/******************************************************************************
Summary:
   Sets the watch dog timer configuration.
   
Description:
   
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_SetWatchDogTimer(BHAB_Handle handle,      /* [in] BHAB PI Handle */
	const BHAB_WatchDogTimerSettings *wdtSettings)
{
	BDBG_ASSERT(handle);
	
	return (handle->settings.api.SetWatchDogTimer(handle, wdtSettings));
}


/******************************************************************************
Summary:
   Gets the 31xx non maskable interrupt configuration.

Description:
   
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_GetNmiConfig(BHAB_Handle handle,      /* [in] BHAB PI Handle */
	BHAB_NmiSettings *nmiSettings)
{
	BDBG_ASSERT(handle);
	
	return (handle->settings.api.GetNmiConfig(handle, nmiSettings));
}

/******************************************************************************
Summary:
   Sets the 31xx non maskable interrupt configuration.

Description:

Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_SetNmiConfig(BHAB_Handle handle,      /* [in] BHAB PI Handle */
	const BHAB_NmiSettings *nmiSettings)
{
	BDBG_ASSERT(handle);
	
	return (handle->settings.api.SetNmiConfig(handle, nmiSettings));
}

/***************************************************************************
Summary:
    This function gets HAB settings.

Description:
    This function returns the current HAB Settings.
    
Returns:
    BERR_Code

****************************************************************************/
BERR_Code BHAB_GetConfigSettings(
    BHAB_Handle handle,           /* [in] Device handle */
    BHAB_ConfigSettings *settings)     /* [out] HAB config settings. */
{
	BDBG_ASSERT(handle);
	
	return (handle->settings.api.GetConfigSettings(handle, settings));
}
    
/***************************************************************************
Summary:
    This function sets HAB settings.

Description:
    This function sets the current HAB Settings.
    
Returns:
    BERR_Code

****************************************************************************/
BERR_Code BHAB_SetConfigSettings(
    BHAB_Handle handle,           /* [in] Device handle */
    const BHAB_ConfigSettings *settings)     /* [in] HAB config settings. */
{
	BDBG_ASSERT(handle);
	
	return (handle->settings.api.SetConfigSettings(handle, settings));
}
        
/******************************************************************************
Summary:
   This function allows the reading from a slave device attached to the demod chip.
Description:
  
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_ReadSlave(
    BHAB_Handle handle,     /* [in] Device channel handle */
	uint8_t chipAddr,       /* [in] chip addr of the i2c slave device */
	uint32_t subAddr,       /* [in] sub addr of the register to read from the slave device */
	uint32_t *data,         /* [out] ptr to the data that we will read from the slave device */
	uint8_t dataLen)        /* [in] how many bytes are we going to read? one to four, For 3412 LNA, this should be in multiples of 4-bytes*/
{
	BDBG_ASSERT(handle);
	
	return (handle->settings.api.ReadSlave(handle, chipAddr, subAddr, data, dataLen));
}

/******************************************************************************
Summary:
   This function allows the writing to a slave device attached to the demod chip.
Description:
  
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_WriteSlave(
    BHAB_Handle handle,     /* [in] Device channel handle */
	uint8_t chipAddr,       /* [in] chip addr of the i2c slave device */
	uint32_t subAddr,       /* [in] sub addr of the register to read from the slave device */
	uint32_t *data,         /* [out] ptr to the data that we will read from the slave device */
	uint8_t dataLen)        /* [in] how many bytes are we going to read? one to four, For 3412 LNA, this should be in multiples of 4-bytes*/
{
	BDBG_ASSERT(handle);
	
	return (handle->settings.api.WriteSlave(handle, chipAddr, subAddr, data, dataLen));
}

/******************************************************************************
Summary:
   This function retrieves the internal gain settings of a demod chip
Description:
  
Returns:
   BERR_Code
******************************************************************************/    
BERR_Code BHAB_GetInternalGain(
    BHAB_Handle handle,                                 /* [in] Device handle */
    const BHAB_InternalGainInputParams *inputParams,  /* [in] frequency */    
    BHAB_InternalGainSettings *internalGainSettings)    /* [out] internal gain settings. */    
{
	BDBG_ASSERT(handle);
	
	return (handle->settings.api.GetInternalGain(handle, inputParams, internalGainSettings));
}

/******************************************************************************
Summary:
   This function sets the external gain settings to a demod chip
Description:
  
Returns:
   BERR_Code
******************************************************************************/   
BERR_Code BHAB_GetExternalGain(
    BHAB_Handle handle,                                   /* [in] Device handle */
    BHAB_ExternalGainSettings *externalGainSettings)    /* [in] external gain settings. */
{
	BDBG_ASSERT(handle);
	
	return (handle->settings.api.GetExternalGain(handle, externalGainSettings));
}
/******************************************************************************
Summary:
   This function retrieves the external gain settings of a demod chip
Description:
  
Returns:
   BERR_Code
******************************************************************************/     
BERR_Code BHAB_SetExternalGain(
    BHAB_Handle handle,                                       /* [in] Device handle */
    const BHAB_ExternalGainSettings *externalGainSettings)  /* [in] external gain settings. */
{
	BDBG_ASSERT(handle);
	
	return (handle->settings.api.SetExternalGain(handle, externalGainSettings));
}
    