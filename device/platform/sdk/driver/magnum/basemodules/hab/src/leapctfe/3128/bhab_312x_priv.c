/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhab_312x_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/58 $
 * $brcm_Date: 10/23/12 11:30a $
 *
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/hab/src/leapctfe/3128/bhab_312x_priv.c $
 * 
 * Hydra_Software_Devel/58   10/23/12 11:30a atanugul
 * SW3128-228: clean up
 * 
 * Hydra_Software_Devel/57   10/18/12 3:42p atanugul
 * SW3128-228: Add API Support for Reading New Front-End 312X, 314X, 346X
 * F/W Version IDs
 * 
 * Hydra_Software_Devel/56   10/12/12 3:33p vishk
 * SW7425-4007: Release mutex before exiting on error condition.
 * 
 * Hydra_Software_Devel/55   9/28/12 1:38p atanugul
 * SW3128-228: Add support for BHAB_GetVersionInfo()
 * 
 * Hydra_Software_Devel/54   9/7/12 11:40a atanugul
 * SW7425-3788: restructure HAB directory
 * 
 * Hydra_Software_Devel/53   7/11/12 12:23p atanugul
 * SW3128-174: Add mutex protection to BHAB_312x_ReadRegister() and
 * BHAB_312x_WriteRegister()
 * 
 * Hydra_Software_Devel/52   6/30/12 5:46p atanugul
 * SW3128-174: Propogate error through HAB return value
 * 
 * Hydra_Software_Devel/51   6/29/12 8:09p atanugul
 * SW3128-174: Add Mutex protection to BHAB_DecodeInterrupt() inside
 * BHAB_ProcessInterruptEvent()
 * 
 * Hydra_Software_Devel/50   4/27/12 12:04p atanugul
 * SW7435-145: Fix Coverity Defect
 * 
 * Hydra_Software_Devel/49   4/19/12 4:48p atanugul
 * SW3128-144: Reset 3128 Completely before downloading the FW; cleanup
 * 
 * Hydra_Software_Devel/48   4/13/12 6:02p vishk
 * SW7358-217: Not allow insertCard.callback & removeCard.callback to be
 * registered at NEXUS_Smartcard_Open()
 * 
 * Hydra_Software_Devel/47   4/9/12 3:00p atanugul
 * SW3128-141: Handle IRQ_STATUS1 register in DecodeInterrupt(); Fix HAB
 * timeout issue.
 * 
 * Hydra_Software_Devel/46   3/30/12 6:08p atanugul
 * SW3128-140: Add Spectrum Analyzer feature to the TNR PI
 * 
 * Hydra_Software_Devel/45   3/30/12 12:08p atanugul
 * SW3128-140: Add Spectrum Analyzer feature to the TNR PI
 * 
 * Hydra_Software_Devel/44   3/21/12 11:00a atanugul
 * SW7418-83: Fix BHAB_P_SWAP32 macro
 * 
 * Hydra_Software_Devel/43   2/28/12 11:35a atanugul
 * SW7231-68: Fix CRC errors
 * 
 * Hydra_Software_Devel/42   2/27/12 5:00p atanugul
 * SW7231-68: Add SPI support
 * 
 * Hydra_Software_Devel/41   2/24/12 5:52p atanugul
 * SW7231-68: Add API support
 * 
 * Hydra_Software_Devel/40   2/14/12 5:16p atanugul
 * SW3128-122: Fix build error on 3128 A0
 * 
 * Hydra_Software_Devel/39   2/14/12 3:26p atanugul
 * SW3128-121: Add OOB No Signal IRQ support to 3128
 * 
 * Hydra_Software_Devel/38   2/8/12 5:30p atanugul
 * SW3128-119: Implement new apis for SetAcquireParams, GetAcquireParams
 * and GetDefaultAcquireParams; dump 3128 registers in case of HAB
 * timeout
 * 
 * Hydra_Software_Devel/37   1/18/12 4:41p atanugul
 * SW3128-93: HAB cleanup
 * 
 * Hydra_Software_Devel/36   1/18/12 11:41a atanugul
 * SW3128-89: Enable reentry for Spectrum Data Ready callback
 * 
 * Hydra_Software_Devel/35   1/17/12 3:32p atanugul
 * SW3128-104: HAB cleanup
 * 
 * Hydra_Software_Devel/34   1/16/12 4:17p atanugul
 * SW3128-107: Cleanup
 * 
 * Hydra_Software_Devel/33   1/11/12 6:25p atanugul
 * SW3128-104: Fix BHAB_312x_P_CheckHab() and BHAB_312x_P_DecodeError() to
 * return the proper error codes.
 * 
 * Hydra_Software_Devel/32   1/11/12 5:41p atanugul
 * SW3128-107: Enable HAB CRC check
 * 
 * Hydra_Software_Devel/31   1/10/12 4:57p atanugul
 * SW3128-107:Enable HAB CRC Check in PI
 * 
 * Hydra_Software_Devel/30   12/22/11 2:21p atanugul
 * SW3128-97: Check for callback type installed before executing the
 * callback
 * 
 * Hydra_Software_Devel/29   12/13/11 12:53p atanugul
 * SW3128-85: Increase HAB timeout to 10.1 seconds
 * 
 * Hydra_Software_Devel/28   12/8/11 5:57p atanugul
 * SW7425-1860: Fix coverity issue
 * 
 * Hydra_Software_Devel/27   12/8/11 3:13p atanugul
 * SW3128-89: Add PI support for Spectrum Analyzer Data retrieval
 * 
 * Hydra_Software_Devel/26   11/29/11 5:24p atanugul
 * SW7425-1722: move bchp_leap_ctrl.h bchp_leap_host_irq.h includes to
 * bhab_312x_priv.h
 * 
 * Hydra_Software_Devel/25   11/9/11 2:58p atanugul
 * SW3128-68: Add BHAB_ReadSlave() and BHAB_ReadSlave() API to 3128
 * 
 * Hydra_Software_Devel/24   11/8/11 5:06p atanugul
 * SW3128-64: Add BHAB_312x_GetConfigSettings BHAB_312x_SetConfigSettings
 * support for 312x
 * 
 * Hydra_Software_Devel/23   11/2/11 6:01p atanugul
 * SW3128-2: Update BHAB_GetApVersion()
 * 
 * Hydra_Software_Devel/22   10/18/11 3:21p atanugul
 * SW3461-67: change write_len, read_len and command_len in
 * BHAB_SendHabCommand() from type uint8_t to uint16_t
 * 
 * Hydra_Software_Devel/21   10/12/11 4:36p atanugul
 * SW3128-2:  Use combined I2C read transaction for B0
 * 
 * Hydra_Software_Devel/20   10/4/11 4:34p atanugul
 * SW3128-53: Add support for 3128 B0
 * 
 * Hydra_Software_Devel/19   10/4/11 4:09p atanugul
 * SW3128-53: Add support for 3128 B0
 * 
 * Hydra_Software_Devel/18   10/4/11 3:53p atanugul
 * SW3128-53: Add support for 3128 B0
 * 
 * Hydra_Software_Devel/17   10/4/11 12:20p atanugul
 * SW3128-2: code cleanup
 * 
 * Hydra_Software_Devel/16   9/20/11 5:12p atanugul
 * SW3128-2: code cleanup
 * 
 * Hydra_Software_Devel/15   8/30/11 12:09p atanugul
 * SW3128-47: Add Support for BCM3123
 * 
 * Hydra_Software_Devel/14   8/25/11 11:04a atanugul
 * SW3128-40: Add No Signal support for 3128
 * 
 * Hydra_Software_Devel/13   8/2/11 4:47p atanugul
 * SW3128-2: Fix BADS_3128_GetSoftDecision() and  rxSymbolRate in
 * BADS_3128_GetAsyncStatus(); code cleanup
 * 
 * Hydra_Software_Devel/12   7/12/11 6:04p vishk
 * SW7425-881: OOB issues on BCM3128
 * 
 * Hydra_Software_Devel/12   7/12/11 6:01p vishk
 * SW7425-881: OOB issues on BCM3128
 * 
 * Hydra_Software_Devel/11   6/27/11 10:29a atanugul
 * SW3128-21: Add FW version for 3128
 * 
 * Hydra_Software_Devel/10   6/20/11 3:47p atanugul
 * SW3128-2: Fix BADS_SetScanParam(); code cleanup
 * 
 * Hydra_Software_Devel/9   5/11/11 2:45p vishk
 * SW3128-2: Added support to configure TM_MTSIF_CTRL - Transport Control.
 * Move the MTSIF settings from BHAB_Open() to BHAB_ResetAp().
 * 
 * Hydra_Software_Devel/8   5/6/11 3:49p atanugul
 * SW3128-2: Added support to configure TM_MTSIF_CTRL - Transport Control
 * Register for BCM3128
 * 
 * Hydra_Software_Devel/7   5/6/11 12:46p atanugul
 * SW3128-2: Fix AOB status and BHAB_3128_SendHabCommand()
 * 
 * Hydra_Software_Devel/6   4/29/11 10:16a atanugul
 * SW3128-8: Add SPI support for 3128
 * 
 * Hydra_Software_Devel/5   4/15/11 4:41p atanugul
 * SW3128-6: Fixed ADS Reset Status and Get Status
 * 
 * Hydra_Software_Devel/4   4/13/11 7:18p vishk
 * SW7425-328: Hab timeout seen upon NEXUS_Frontend_Untune()
 * 
 * Hydra_Software_Devel/3   4/8/11 4:37p vishk
 * SW3128-4: Bus Error due to channel callbacks called for the ones not
 * opened too.
 * 
 * Hydra_Software_Devel/2   3/28/11 12:13p atanugul
 * SW3128-2: Add Support for 8 cores and code cleanup
 * 
 * Hydra_Software_Devel/1   3/4/11 3:10p vishk
 * SW7422-171: Nexus support for the 3128 (8 ADS core + tuner on daughter
 * board) plug on 7422 board
 * 
 * Hydra_Software_Devel/3   3/2/11 3:24p atanugul
 * SW3128-2: Add Support for BCM3128
 * 
 * Hydra_Software_Devel/2   2/2/11 12:46p atanugul
 * SW3128-2: Add Support for BCM3128
 * 
 * Hydra_Software_Devel/1   2/1/11 10:44a atanugul
 * SW3128-2: Add Support for BCM3128
 *
 ***************************************************************************/
#include "bhab_312x_priv.h"
#include "bchp_hsi.h"
#include "../../c0/bchp_leap_hab_mem.h"
#include "../../c0/bchp_tm.h"

#ifndef BHAB_312X_VER
#error "BHAB_312X_VER needs to be defined"
#endif

#if BSTD_CPU_ENDIAN == BSTD_ENDIAN_LITTLE
#define BHAB_P_SWAP32(a) (((a&0xFF)<<24)|((a&0xFF00)<<8)|((a&0xFF0000)>>8)|((a&0xFF000000)>>24))
#else
#define BHAB_P_SWAP32(a) a
#endif

/* define BHAB_DEBUG */

BDBG_MODULE(bhab_312x_priv);

static uint8_t BHAB_312x_P_CRC8Block(uint8_t crc, uint8_t *pDataBlock, uint32_t dataLength);
    
/******************************************************************************
 BHAB_312x_Open()
******************************************************************************/
BERR_Code BHAB_312x_Open(
    BHAB_Handle *handle,     /* [out] BHAB handle */
    void        *pReg,       /* [in] pointer ot i2c or spi handle */
    const BHAB_Settings *pDefSettings /* [in] Default Settings */
)
{
    BERR_Code retCode = BERR_SUCCESS;
    BHAB_Handle hDev;
    BHAB_312x_P_Handle *h312xDev = NULL;
    unsigned i;
    
    BDBG_ASSERT(pDefSettings->interruptEnableFunc);

    hDev = (BHAB_Handle)BKNI_Malloc(sizeof(BHAB_P_Handle));
    
    if(!hDev)
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    else
    {
        h312xDev = (BHAB_312x_P_Handle *)BKNI_Malloc(sizeof(BHAB_312x_P_Handle));
        if(h312xDev)
        {
            BKNI_Memset( h312xDev, 0x00, sizeof(BHAB_312x_P_Handle));
            hDev->pImpl = (void*)h312xDev;

            if(pDefSettings->isSpi) {
               h312xDev->hSpiRegister = (BREG_SPI_Handle)pReg;
            }
            else {
               h312xDev->hI2cRegister = (BREG_I2C_Handle)pReg;
            }

            BKNI_Memcpy((void*)(&(hDev->settings)), (void*)pDefSettings, sizeof(BHAB_Settings));

            /* create events */
            BHAB_CHK_RETCODE(BKNI_CreateEvent(&(h312xDev->hInterruptEvent)));
            BHAB_CHK_RETCODE(BKNI_CreateEvent(&(h312xDev->hApiEvent)));
            BHAB_CHK_RETCODE(BKNI_CreateEvent(&(h312xDev->hInitDoneEvent)));
            BHAB_CHK_RETCODE(BKNI_CreateEvent(&(h312xDev->hHabDoneEvent)));
            BHAB_CHK_RETCODE(BKNI_CreateMutex(&(h312xDev->hMutex)));
            BHAB_CHK_RETCODE(BKNI_CreateMutex(&(h312xDev->hRegAccessMutex)));

            for(i=0; i<BHAB_DevId_eMax; i++){
                h312xDev->InterruptCallbackInfo[i].func = NULL;
                h312xDev->InterruptCallbackInfo[i].pParm1 = NULL;
                h312xDev->InterruptCallbackInfo[i].parm2 = (int)NULL;
            }

            BKNI_Memset( &h312xDev->nmiSettings, 0x00, sizeof(BHAB_NmiSettings));
            BKNI_Memset( &h312xDev->wdtSettings, 0x00, sizeof(BHAB_WatchDogTimerSettings));

            h312xDev->isSpi = pDefSettings->isSpi;
            h312xDev->isMtsif = pDefSettings->isMtsif;

            retCode = BHAB_312x_P_DisableInterrupts(hDev);
        }
        else
            retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
done:
    if(retCode == BERR_SUCCESS)
        *handle = hDev;
    else
    {
        if(h312xDev)
        {
            BKNI_Free(h312xDev);
            h312xDev = NULL; 
        }
        if(hDev)
        {
            BKNI_Free(hDev);              
            hDev = NULL;
        }
        *handle = NULL;
    }

    return retCode;
}


/******************************************************************************
 BHAB_312x_Close()
******************************************************************************/
BERR_Code BHAB_312x_Close(BHAB_Handle handle)
{
    BERR_Code retCode = BERR_SUCCESS;
    BHAB_312x_P_Handle *p312x;
    uint32_t sb, buf;

    BDBG_ASSERT(handle);

    p312x = (BHAB_312x_P_Handle *)(handle->pImpl);
    BDBG_ASSERT(p312x);    
    
    /* If we didn't load the AP when starting, don't reset it when closing */
    if (p312x->loadAP)
    {   
        /* reset all interrupt status */
        buf = 0xFFFFFFFF;
        BHAB_CHK_RETCODE(BHAB_312x_WriteRegister(handle, BCHP_LEAP_HOST_IRQ_CLEAR0, &buf));
        BHAB_CHK_RETCODE(BHAB_312x_WriteRegister(handle, BCHP_LEAP_HOST_IRQ_CLEAR1, &buf));
        /* reset the AP */
        sb = 0x01;
        retCode = BHAB_312x_WriteRegister(handle, BCHP_LEAP_CTRL_CTRL, &sb);

        if(retCode)
            BDBG_WRN(("Ap not in reset."));
    }

    retCode = BHAB_312x_P_DisableInterrupts(handle);
done:
    if (p312x->hInterruptEvent) {BKNI_DestroyEvent(p312x->hInterruptEvent);}
    if (p312x->hApiEvent){BKNI_DestroyEvent(p312x->hApiEvent);}
    if (p312x->hInitDoneEvent){BKNI_DestroyEvent(p312x->hInitDoneEvent);}
    if (p312x->hHabDoneEvent){BKNI_DestroyEvent(p312x->hHabDoneEvent);}
    if (p312x->hMutex){BKNI_DestroyMutex(p312x->hMutex);}
    if (p312x->hRegAccessMutex){BKNI_DestroyMutex(p312x->hRegAccessMutex);}

    BKNI_Free((void*)p312x);
    BKNI_Free((void*)handle);

    return retCode;
}


/******************************************************************************
 BHAB_312x_P_InitAp()
******************************************************************************/
BERR_Code BHAB_312x_InitAp(
    BHAB_Handle handle,       /* [in] BHAB handle */
    const uint8_t *pHexImage  /* [in] pointer to BCM312x microcode image */
)
{
    BERR_Code retCode = BERR_SUCCESS;
    BHAB_312x_P_Handle *p312x;
    uint32_t n, instlen, datalen, instaddr, dataaddr;
    const uint8_t *pImage;
    uint8_t retries=0, count=0;

    BDBG_ASSERT(handle);

    p312x = (BHAB_312x_P_Handle *)(handle->pImpl);
    BDBG_ASSERT(p312x);
    
    /* We are loading AP code, it is appropriate to reset the AP when closing */
    p312x->loadAP = true;

    /* Disable host interrupt */    
    BHAB_CHK_RETCODE(BHAB_312x_P_EnableHostInterrupt(handle, false));

    /* reset the AP before downloading the microcode */
    BHAB_CHK_RETCODE(BHAB_312x_P_ResetAp(handle));

    if (pHexImage)
    {
        /* download to RAM */
        pImage = pHexImage;

        instlen = (pImage[10] << 16) | (pImage[11] << 8) | pImage[12];
        datalen = (pImage[16] << 16) | (pImage[17] << 8) | pImage[18];
        n = instlen + datalen;
        if (n != 0)
        {
            instaddr = (pImage[7] << 16) | (pImage[8] << 8) | pImage[9];
            dataaddr = (pImage[13] << 16) | (pImage[14] << 8) | pImage[15];
            
            for (retries = 0; retries < 3; retries++)
            {
                retCode = BHAB_312x_WriteMemory(handle, instaddr, &pImage[19], instlen);            
                if (retCode == BERR_SUCCESS)
                    break;
                else
                    BDBG_WRN(("BHAB_312x_WriteMemory() retCode 0x%X", retCode));
            }
        }

    }

    if (retries == 3)
    {
        BDBG_ERR(("unable to download FW"));
        BERR_TRACE(retCode = BHAB_ERR_HOST_XFER);
    }
    else
    {
        /* enable init done interrupt */
        BHAB_CHK_RETCODE(BHAB_312x_P_EnableInitDoneInterrupt(handle));

        /* start running the AP */
        if ((retCode = BHAB_312x_P_RunAp(handle)) == BERR_SUCCESS)
        {
            for (count = 0; count < BHAB_INIT_RETRIES; count++)
            {
                /* wait for init done interrupt */
                if ((retCode = BHAB_312x_P_WaitForEvent(handle, p312x->hInitDoneEvent, 150)) == BERR_SUCCESS)
                    break;
                /* Disable host interrupt */ 
                if ((retCode = BHAB_312x_P_EnableHostInterrupt(handle, false)) != BERR_SUCCESS)
                    BDBG_WRN(("Failed disabling Host Interrupt"));              
            }
            
            if (count < BHAB_INIT_RETRIES)
                retCode = BERR_SUCCESS;
            else
            {
                /* reset the AP before exiting */
                BHAB_CHK_RETCODE(BHAB_312x_P_ResetAp(handle));
                BKNI_Sleep(10);
                BDBG_ERR(("AP initialization timeout. Starting over %d times", count));
                BERR_TRACE(retCode = BHAB_ERR_AP_NOT_INIT);            
            }
        }
    }

done:
    if(retCode != BERR_SUCCESS)
        BHAB_312x_P_EnableHostInterrupt(handle, true);
    return retCode;
}


/******************************************************************************
 BHAB_312x_GetApStatus()
******************************************************************************/
BERR_Code BHAB_312x_GetApStatus(
   BHAB_Handle handle,      /* [in] HAB device handle */
   BHAB_ApStatus *pStatus   /* [out] AP status */
)
{
    BERR_Code retCode = BERR_SUCCESS;
    uint32_t sb;

    BDBG_ASSERT(handle);

    *pStatus = 0;
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_LEAP_CTRL_SW_SPARE0, &sb)); 
    *pStatus = sb;
    
done:
    return retCode;
}


/******************************************************************************
 BHAB_312x_GetApVersion()
******************************************************************************/
BERR_Code BHAB_312x_GetApVersion(
    BHAB_Handle handle,     /* [in] BHAB handle */
    uint32_t    *pFamilyId, /* [out] Chip Family id */    
    uint32_t    *pChipId,   /* [out] BHAB chip ID */
    uint16_t    *pChipVer,  /* [out] chip revision number */
    uint8_t     *pMajApVer, /* [out] AP microcode major version */
    uint8_t     *pMinApVer  /* [out] AP microcode minor version */
)
{
    BERR_Code retCode = BERR_SUCCESS;
    uint8_t buf[21];

    BDBG_ASSERT(handle);

    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_TM_FAMILY_ID, pFamilyId));
    
    buf[0] = ((BHAB_GETVERSION >> 2) & 0xFF);
    buf[1] = (((BHAB_GETVERSION & 0x3) << 6) | ((PAYLOAD_LEN >> 4) & 0x3F));
    buf[2] = ((PAYLOAD_LEN & 0xF) << 4);
    buf[3] = BHAB_CORE_ID;
    buf[4] = 0;    /* CRC */ 
    BHAB_CHK_RETCODE(BHAB_312x_SendHabCommand(handle, buf, 5, buf, 21, false, true, 21)); 
    *pChipId = ((buf[4] << 8) | buf[5]);
    *pChipVer = ((buf[6] << 8) | buf[7]);
    *pMajApVer = buf[8];
    *pMinApVer = buf[9];

done:
    if(retCode != BERR_SUCCESS)
    {
        *pChipId = 0;
        *pChipVer = 0;
        *pMajApVer = 0;
        *pMinApVer = 0;
    }
    
    return retCode;
}


/******************************************************************************
 BHAB_3461_GetVersionInfo()
******************************************************************************/
BERR_Code BHAB_312x_GetVersionInfo(
    BHAB_Handle             handle,         /* [in]  BHAB handle */
    BFEC_SystemVersionInfo  *pVersionInfo   /* [out]  Returns FW version information */
)
{
    BERR_Code retCode = BERR_SUCCESS;
    uint8_t buf[21] = HAB_MSG_HDR(BHAB_GETVERSION, 0, BHAB_GLOBAL_CORE_TYPE, BHAB_CORE_ID);
    uint8_t sysVersion[73] = HAB_MSG_HDR(BHAB_GET_SYS_VERSION, 0, BHAB_GLOBAL_CORE_TYPE, BHAB_CORE_ID);    

    BDBG_ASSERT(handle);

    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_TM_FAMILY_ID, &pVersionInfo->familyId));

    BHAB_CHK_RETCODE(BHAB_312x_SendHabCommand(handle, buf, 5, buf, 21, false, true, 21));
    pVersionInfo->chipId = ((buf[4] << 8) | buf[5]);
    pVersionInfo->chipVersion = ((buf[6] << 8) | buf[7]);
    pVersionInfo->firmware.majorVersion = buf[8];
    pVersionInfo->firmware.minorVersion = buf[9];
    
    if((pVersionInfo->firmware.majorVersion >= 6) || ((pVersionInfo->firmware.majorVersion >= 5) & ( pVersionInfo->firmware.minorVersion>= 7)))
    {
        BHAB_CHK_RETCODE(BHAB_312x_SendHabCommand(handle, sysVersion, 5, sysVersion, 73, false, true, 73));
        pVersionInfo->familyId = (sysVersion[4] << 24) | (sysVersion[5] << 16) | (sysVersion[6] << 8) | sysVersion[7]; 
        pVersionInfo->chipId = (sysVersion[8] << 24) | (sysVersion[9] << 16) | (sysVersion[10] << 8) | sysVersion[11];
        pVersionInfo->chipVersion = (sysVersion[12] << 24) | (sysVersion[13] << 16) | (sysVersion[14] << 8) | sysVersion[15];
        pVersionInfo->firmware.majorVersion = ((sysVersion[16] << 8) | sysVersion[17]);
        pVersionInfo->firmware.minorVersion = ((sysVersion[18] << 8) | sysVersion[19]);       
        pVersionInfo->firmware.buildType = ((sysVersion[20] << 8) | sysVersion[21]);       
        pVersionInfo->firmware.buildId = ((sysVersion[22] << 8) | sysVersion[23]);
        pVersionInfo->bondoutOptions[0] = (sysVersion[40] << 24) | (sysVersion[41] << 16) | (sysVersion[42] << 8) | sysVersion[43];
        pVersionInfo->bondoutOptions[1] = (sysVersion[44] << 24) | (sysVersion[45] << 16) | (sysVersion[46] << 8) | sysVersion[47];        
    }
    else
    {
        pVersionInfo->firmware.buildType = 0;
        pVersionInfo->firmware.buildId = 0;
        pVersionInfo->bondoutOptions[0] = 0;
        pVersionInfo->bondoutOptions[1] = 0;
    }

done:
    return retCode;
}


/******************************************************************************
 BHAB_312x_ReadMemory()
******************************************************************************/
BERR_Code BHAB_312x_ReadMemory(BHAB_Handle handle, uint32_t addr, uint8_t *buf, uint32_t n)
{   
    BERR_Code retCode = BERR_SUCCESS;
    BHAB_312x_P_Handle *p312x;
    uint8_t sb, data, readbuf[8], writebuf[8];
    uint32_t sb1;
    uint16_t bytes_left, offset;
    uint8_t i=0;

    BDBG_ASSERT(handle);
    p312x = (BHAB_312x_P_Handle *)(handle->pImpl);

    if (((uint32_t)addr + (uint32_t)n) > (BCHP_LEAP_HAB_MEM_WORDi_ARRAY_BASE + BHAB_HAB_SIZE))
        return BERR_TRACE(BERR_INVALID_PARAMETER);  
        
    sb1 = BHAB_P_SWAP32(addr);
    if(p312x->isSpi){       
        /* enable read mode and speculative read */    
        writebuf[0] = (handle->settings.chipAddr << 1) | 0x1;
        writebuf[1] = CSR_CONFIG;
        writebuf[2] = CSR_CONFIG_READ_RBUS_READ | (CSR_CONFIG_SPECULATIVE_READ_EN_BITS << CSR_CONFIG_SPECULATIVE_READ_EN_SHIFT);
        BHAB_CHK_RETCODE(BREG_SPI_Write(p312x->hSpiRegister,  writebuf, 3));

        /* set CSR pointer to point to ADDR0 and set RBUS address to read */        
        writebuf[1] = CSR_RBUS_ADDR0;
        writebuf[2] = sb1;
        writebuf[3] = (sb1 >> 8);
        writebuf[4] = (sb1 >> 16);
        writebuf[5] = (sb1 >> 24);
        BHAB_CHK_RETCODE(BREG_SPI_Write(p312x->hSpiRegister,  writebuf, 6));

        writebuf[0] = (handle->settings.chipAddr << 1);      
        for (offset = 0; offset < n/4 * 4; offset+=4) {
            writebuf[1] = CSR_RBUS_DATA0;
            BHAB_CHK_RETCODE(BREG_SPI_Read(p312x->hSpiRegister, writebuf, readbuf, 6));
            *buf++ = readbuf[2];
            *buf++ = readbuf[3];
            *buf++ = readbuf[4];
            *buf++ = readbuf[5];

            for(i=0; i < 5; i++){ 
                writebuf[1] = CSR_STATUS;
                BHAB_CHK_RETCODE(BREG_SPI_Read(p312x->hSpiRegister, writebuf, readbuf, 3));              
                if (readbuf[2] == BHAB_CPU_RUNNIG)
                    break;
            }
            if(i==5)    
                BDBG_WRN(("Write transaction not finished"));                
        }
        
        /* Read the bytes left */
        bytes_left = n%4;        
        if(bytes_left)
        {
            writebuf[1] = CSR_RBUS_DATA0;        
            BHAB_CHK_RETCODE(BREG_SPI_Read(p312x->hSpiRegister, writebuf, readbuf, 2+bytes_left));        
            for(i=0; i<bytes_left; i++) {
                *buf++ = readbuf[2+i];           
            }
        }
        
        /* set it back to the default write mode */   
        writebuf[0] = (handle->settings.chipAddr << 1) | 0x1;
        writebuf[1] = CSR_CONFIG;
        writebuf[2] = 0;
        BHAB_CHK_RETCODE(BREG_SPI_Write(p312x->hSpiRegister,  writebuf, 3));
    }
    else {
        /* enable read mode and speculative read*/
        data = CSR_CONFIG_READ_RBUS_READ | (CSR_CONFIG_SPECULATIVE_READ_EN_BITS << CSR_CONFIG_SPECULATIVE_READ_EN_SHIFT);
        BHAB_CHK_RETCODE(BREG_I2C_Write(p312x->hI2cRegister, handle->settings.chipAddr, CSR_CONFIG, &data, 1));  

        /* set CSR pointer to point to ADDR0 and set RBUS address to read */
        BHAB_CHK_RETCODE(BREG_I2C_Write(p312x->hI2cRegister, handle->settings.chipAddr, CSR_RBUS_ADDR0, (uint8_t *)&sb1, 4));    

        for (offset = 0; offset < n; offset+=4) {
            BHAB_CHK_RETCODE(BREG_I2C_P_Read312x(p312x->hI2cRegister, handle->settings.chipAddr, CSR_RBUS_DATA0, buf+offset, 4)); 
            for(i=0; i < 5; i++){
                BHAB_CHK_RETCODE(BREG_I2C_P_Read312x(p312x->hI2cRegister, handle->settings.chipAddr, CSR_STATUS, &sb, 1));
                if(sb == BHAB_CPU_RUNNIG)
                    break;
            }
            if(i==5)    
                BDBG_WRN(("Write transaction not finished"));              
        }

        data =0;    
        BHAB_CHK_RETCODE(BREG_I2C_Write(p312x->hI2cRegister, handle->settings.chipAddr, CSR_CONFIG, &data, 1));      
    }
done:
    return retCode;
}


/******************************************************************************
 BHAB_312x_WriteMemory()
******************************************************************************/
BERR_Code BHAB_312x_WriteMemory(BHAB_Handle handle, uint32_t addr, const uint8_t *buf, uint32_t n)
{   
    BERR_Code retCode = BERR_SUCCESS;
    BHAB_312x_P_Handle *p312x;
    uint8_t pad, i=0, k=0;
    uint16_t bytes_left;
    uint32_t sb1, curr_addr, j=0;
    uint8_t readbuf[8], writebuf[16];

    BDBG_ASSERT(handle);

    p312x = (BHAB_312x_P_Handle *)(handle->pImpl);
    BDBG_ASSERT(p312x);
    
    sb1 = BHAB_P_SWAP32(addr);

    if ((addr + n) >= (BCHP_LEAP_HAB_MEM_WORDi_ARRAY_BASE +128))
        return BERR_TRACE(BERR_INVALID_PARAMETER);

    if(p312x->isSpi)
    {
        /* set CSR pointer to point to ADDR0 and set RBUS address to zero */      
        writebuf[0] = (handle->settings.chipAddr << 1) | 0x1;
        writebuf[1] = CSR_CONFIG;
        writebuf[2] = 0;
        BHAB_CHK_RETCODE(BREG_SPI_Write(p312x->hSpiRegister,  writebuf, 3));       

        writebuf[1] = CSR_RBUS_ADDR0;
        writebuf[2] = sb1;
        writebuf[3] = (sb1 >> 8);
        writebuf[4] = (sb1 >> 16);
        writebuf[5] = (sb1 >> 24);
        BHAB_CHK_RETCODE(BREG_SPI_Write(p312x->hSpiRegister,  writebuf, 6));   
        
        writebuf[1] = CSR_RBUS_DATA0;            
        for (j = 0; j < n; j+=14) {
            if(j%4)
                writebuf[1] = CSR_RBUS_DATA2;
            else
                writebuf[1] = CSR_RBUS_DATA0;            
            for (i = 0; i < 14; i++) {
                if(i+j == n)
                    break;
                writebuf[2+i] = (*((uint8_t *)buf+j+i));
                }
            BHAB_CHK_RETCODE(BREG_SPI_Write(p312x->hSpiRegister,  writebuf, 16));     
        }
        
        /* pad with zeros at the end, so that an RBUS write can be triggered for the last word */
        bytes_left = n%4;               
        if(bytes_left)
        {   
            writebuf[0] = (handle->settings.chipAddr << 1) | 0x1;
            writebuf[1] = CSR_RBUS_DATA0+bytes_left;
            for(k=0; k<(4-bytes_left); k++)
            {
                writebuf[2+bytes_left-1+k] = 0;
            }
            BHAB_CHK_RETCODE(BREG_SPI_Write(p312x->hSpiRegister,  writebuf, 6)); 
        } 
        /* check for host transfer error */
        writebuf[0] = (handle->settings.chipAddr << 1);
        writebuf[1] = CSR_STATUS;        
        for(i=0; i < 5; i++){
            BHAB_CHK_RETCODE(BREG_SPI_Read(p312x->hSpiRegister,  writebuf, readbuf, 3));           
            if ((readbuf[2] & CSR_STATUS_ERROR_BITS) == 0)
                break;
        }  
        if(i==5)    
            BDBG_WRN(("Write transaction not finished"));          
    }
    else    
    {
        /* set CSR pointer to point to ADDR0 and set RBUS address to zero */
        BHAB_CHK_RETCODE(BREG_I2C_Write(p312x->hI2cRegister, handle->settings.chipAddr, CSR_RBUS_ADDR0, (uint8_t *)&sb1, 4));               

        /* set CSR pointer to point to DATA0 and provide the data to be downloaded */
        BHAB_CHK_RETCODE(BREG_I2C_Write(p312x->hI2cRegister, handle->settings.chipAddr, CSR_RBUS_DATA0, buf, n));

        /* pad with zeros at the end, so that an RBUS write can be triggered for the last word */
        bytes_left = n%4;
        if(bytes_left)
        {    
            pad = 0;
            curr_addr = CSR_RBUS_DATA0 + bytes_left;
            for(i=0; i<(4-bytes_left); i++)
            {              
                BHAB_CHK_RETCODE(BREG_I2C_Write(p312x->hI2cRegister, handle->settings.chipAddr, curr_addr++, &pad, 1));
            }          
        }
    }
done:
    return retCode;
}


/******************************************************************************
 BHAB_312x_ReadMbox()
******************************************************************************/
BERR_Code BHAB_312x_ReadMbox(
    BHAB_Handle handle,    /* [in] BHAB PI Handle */
    uint32_t    reg,  /* [in] RBUS register address */
    uint32_t    *val  /* [out] value read from register */
)
{
    BERR_Code retCode = BERR_SUCCESS;
    BSTD_UNUSED(handle);
    BSTD_UNUSED(reg);
    BSTD_UNUSED(val);

    return retCode;
}


/******************************************************************************
 BHAB_312x_WriteMbox()
******************************************************************************/
BERR_Code BHAB_312x_WriteMbox(
    BHAB_Handle handle,    /* [in] BHAB PI Handle */
    uint32_t    reg,  /* [in] RBUS register address */
    uint32_t    *val  /* [in] value to write */
)
{
    BERR_Code retCode = BERR_SUCCESS;
    BSTD_UNUSED(handle);
    BSTD_UNUSED(reg);
    BSTD_UNUSED(val);

    return retCode;
}


/******************************************************************************
 BHAB_312x_ReadRegister()
******************************************************************************/
BERR_Code BHAB_312x_ReadRegister(
    BHAB_Handle handle,    /* [in] BHAB PI Handle */
    uint32_t    reg,  /* [in] RBUS register address */
    uint32_t    *val  /* [out] value read from register */
)
{   
    BERR_Code retCode = BERR_SUCCESS;
    BHAB_312x_P_Handle *p312x;
    uint8_t data, sb, i=0;
    uint32_t sb1, buf;
    uint8_t readbuf[8], writebuf[8];

    BDBG_ASSERT(handle);
    BHAB_P_ACQUIRE_REG_ACCESS_MUTEX(handle); 
    p312x = (BHAB_312x_P_Handle *)(handle->pImpl);
    BDBG_ASSERT(p312x);
    
    sb1 = BHAB_P_SWAP32(reg);
  
    if(p312x->isSpi)
    {
        readbuf[2] = 0;
        
        writebuf[0] = (handle->settings.chipAddr << 1) | 0x1;
        writebuf[1] = CSR_CONFIG;
        writebuf[2] = 0;
        BHAB_CHK_RETCODE(BREG_SPI_Write (p312x->hSpiRegister, writebuf, 3));
        
        /* enable read mode and speculative read */    
        writebuf[0] = (handle->settings.chipAddr << 1);
        writebuf[1] = CSR_CONFIG;
        BHAB_CHK_RETCODE(BREG_SPI_Read(p312x->hSpiRegister, writebuf, readbuf, 3));
          
        writebuf[0] = (handle->settings.chipAddr << 1) | 0x1;          
        readbuf[2] &= ~CSR_CONFIG_READ_RBUS_MASK; 
        readbuf[2] |=  ((CSR_CONFIG_READ_RBUS_READ << CSR_CONFIG_READ_RBUS_SHIFT) | (1 << CSR_CONFIG_NO_RBUS_ADDR_INC_SHIFT));     
        writebuf[2] = readbuf[2];
        BHAB_CHK_RETCODE(BREG_SPI_Write(p312x->hSpiRegister,  writebuf, 3));
        
        /* set CSR pointer to point to ADDR0 and set RBUS address to write */        
        writebuf[1] = CSR_RBUS_ADDR0;
        writebuf[2] = sb1;
        writebuf[3] = (sb1 >> 8);
        writebuf[4] = (sb1 >> 16);
        writebuf[5] = (sb1 >> 24);  
        BHAB_CHK_RETCODE(BREG_SPI_Write(p312x->hSpiRegister,  writebuf, 6));   
        
        /* poll the busy bit to make sure the transaction is completed */       
        writebuf[0] = (handle->settings.chipAddr << 1);
        writebuf[1] = CSR_STATUS;          
        for(i=0; i < 5; i++){
            BHAB_CHK_RETCODE(BREG_SPI_Read(p312x->hSpiRegister,  writebuf, readbuf, 3));             
             if ((readbuf[2] & (1 << CSR_STATUS_BUSY_SHIFT)) == 0)
                break;
        }  
        if(i==5)    
            BDBG_WRN(("Read transaction not finished"));  

        /* read data*/   
        writebuf[0] = (handle->settings.chipAddr << 1);        
        writebuf[1] = CSR_RBUS_DATA0;
        BHAB_CHK_RETCODE(BREG_SPI_Read(p312x->hSpiRegister, writebuf, readbuf, 6));  
       
        *val = BHAB_P_SWAP32(*(uint32_t *)&readbuf[2]);        
    
        /* set READ_RBUS to the reset value for write mode */        
        writebuf[0] = (handle->settings.chipAddr << 1) | 0x1;         
        writebuf[1] = CSR_CONFIG;
        writebuf[2] = CSR_CONFIG_READ_RBUS_WRITE;
        BHAB_CHK_RETCODE(BREG_SPI_Write(p312x->hSpiRegister,  writebuf, 3));     
    }    
    else
    { 
        /* set READ_RBUS for read mode */
        data = ((CSR_CONFIG_READ_RBUS_READ << CSR_CONFIG_READ_RBUS_SHIFT) | (1 << CSR_CONFIG_NO_RBUS_ADDR_INC_SHIFT));
        BHAB_CHK_RETCODE(BREG_I2C_Write(p312x->hI2cRegister, handle->settings.chipAddr, CSR_CONFIG, &data, 1));  
               
        BHAB_CHK_RETCODE(BREG_I2C_Write(p312x->hI2cRegister, handle->settings.chipAddr, CSR_RBUS_ADDR0, (uint8_t *)&sb1, 4));

        /* poll the busy bit to make sure the transaction is completed */  
        for(i=0; i < 5; i++){
            retCode = BREG_I2C_P_Read312x(p312x->hI2cRegister, handle->settings.chipAddr, CSR_STATUS, &sb, 1);
            if ((sb & (1 << CSR_STATUS_BUSY_SHIFT)) == 0)
                break;
        }
        if(i==5)
            BDBG_WRN(("Read transaction not finished\n"));    

        /* read the data */
        BHAB_CHK_RETCODE(BREG_I2C_P_Read312x(p312x->hI2cRegister, handle->settings.chipAddr, CSR_RBUS_DATA0, (uint8_t *)&buf, 4));
        *val = (uint32_t)BHAB_P_SWAP32(buf);  

        /* set READ_RBUS to the reset value for write mode */
        data = CSR_CONFIG_READ_RBUS_WRITE;
        BHAB_CHK_RETCODE(BREG_I2C_Write(p312x->hI2cRegister, handle->settings.chipAddr, CSR_CONFIG, &data, 1));
    }
done:
    BHAB_P_RELEASE_REG_ACCESS_MUTEX(handle);
    return retCode;
}


/******************************************************************************
 BHAB_312x_WriteRegister()
******************************************************************************/
BERR_Code BHAB_312x_WriteRegister(
    BHAB_Handle handle,    /* [in] BHAB PI Handle */
    uint32_t    reg,  /* [in] RBUS register address */
    uint32_t    *val  /* [in] value to write */
)
{   
    BERR_Code retCode = BERR_SUCCESS;
    BHAB_312x_P_Handle *p312x;
    uint8_t sb, buf[4], i=0;
    uint32_t sb1;
    uint8_t readbuf[8], writebuf[8];
    
    BDBG_ASSERT(handle);
    BHAB_P_ACQUIRE_REG_ACCESS_MUTEX(handle);
    p312x = (BHAB_312x_P_Handle *)(handle->pImpl);
    BDBG_ASSERT(p312x);    
    BSTD_UNUSED(reg);
    
    sb1 = BHAB_P_SWAP32(reg);
    if(p312x->isSpi)
    {
        /* enable write mode */    
        writebuf[0] = (handle->settings.chipAddr << 1);
        writebuf[1] = CSR_CONFIG;
        BHAB_CHK_RETCODE(BREG_SPI_Read (p312x->hSpiRegister, writebuf, readbuf, 3));
           
        writebuf[0] = (handle->settings.chipAddr << 1) | 0x1;        
        readbuf[2] &= ~CSR_CONFIG_READ_RBUS_MASK; 
        readbuf[2] |= (CSR_CONFIG_READ_RBUS_WRITE << CSR_CONFIG_READ_RBUS_SHIFT);        
        writebuf[2] = readbuf[2];
        BHAB_CHK_RETCODE(BREG_SPI_Write (p312x->hSpiRegister,  writebuf, 3));   
        
        /* set CSR pointer to point to ADDR0 and set RBUS address to write */        
        writebuf[1] = CSR_RBUS_ADDR0;
        writebuf[2] = sb1;
        writebuf[3] = (sb1 >> 8);
        writebuf[4] = (sb1 >> 16);
        writebuf[5] = (sb1 >> 24);
        BHAB_CHK_RETCODE(BREG_SPI_Write (p312x->hSpiRegister,  writebuf, 6));       
        
        /* write to RBUS*/
        writebuf[1] = CSR_RBUS_DATA0;
        *(uint32_t *)&writebuf[2] = BHAB_P_SWAP32(*(uint32_t *)val); 
        BHAB_CHK_RETCODE(BREG_SPI_Write (p312x->hSpiRegister,  writebuf, 6));     
        
        /* poll the busy bit to make sure the transaction is completed */
        writebuf[0] = (handle->settings.chipAddr << 1);
        writebuf[1] = CSR_STATUS;        
        for(i=0; i < 5; i++){
            BHAB_CHK_RETCODE(BREG_SPI_Read(p312x->hSpiRegister,  writebuf, readbuf, 3));           
            if ((readbuf[2] & (1 << CSR_STATUS_BUSY_SHIFT)) == 0)
                break;
        }  
        if(i==5)    
            BDBG_WRN(("Write transaction not finished"));       
    }
    else {
        /* set CSR pointer to point to ADDR0 and set RBUS address to write */
        BHAB_CHK_RETCODE(BREG_I2C_Write(p312x->hI2cRegister, handle->settings.chipAddr, CSR_RBUS_ADDR0, (uint8_t *)&sb1, 4));
        sb1 = BHAB_P_SWAP32(*(uint32_t *)val);
        /* write to RBUS*/
        BHAB_CHK_RETCODE(BREG_I2C_Write(p312x->hI2cRegister, handle->settings.chipAddr, CSR_RBUS_DATA0, (uint8_t *)&sb1, 4));
        
        BHAB_CHK_RETCODE(BREG_I2C_P_Read312x(p312x->hI2cRegister, handle->settings.chipAddr, CSR_RBUS_DATA0, buf, 4));
     
        /* poll the busy bit to make sure the transaction is completed */
        for(i=0; i < 5; i++){
            retCode = BREG_I2C_P_Read312x(p312x->hI2cRegister, handle->settings.chipAddr, CSR_STATUS, &sb, 1);
            if ((sb & (1 << CSR_STATUS_BUSY_SHIFT)) == 0)
                break;
        }
        if(i==5)
            BDBG_WRN(("Write transaction not finished\n"));

    }
 
done:
    BHAB_P_RELEASE_REG_ACCESS_MUTEX(handle); 
    return retCode;
}


/******************************************************************************
 BHAB_312x_HandleInterrupt_isr()
******************************************************************************/
BERR_Code BHAB_312x_HandleInterrupt_isr(
    BHAB_Handle handle /* [in] BHAB handle */
)
{
    BHAB_312x_P_Handle *p312x;
    BKNI_ASSERT_ISR_CONTEXT();

    BDBG_ASSERT(handle);

    p312x = (BHAB_312x_P_Handle *)(handle->pImpl);
    BDBG_ASSERT(p312x);
    
    handle->settings.interruptEnableFunc(false, handle->settings.interruptEnableFuncParam);
      
    BKNI_SetEvent_isr(p312x->hApiEvent);   
    BKNI_SetEvent_isr(p312x->hInterruptEvent);
    return BERR_SUCCESS;
}


/******************************************************************************
 BHAB_312x_ProcessInterruptEvent()
******************************************************************************/
BERR_Code BHAB_312x_ProcessInterruptEvent(
    BHAB_Handle handle  /* [in] BHAB handle */
)
{
    BERR_Code retCode = BERR_SUCCESS;
    BDBG_ASSERT(handle);

    BHAB_P_ACQUIRE_MUTEX(handle);     
    BHAB_312x_P_EnableHostInterrupt(handle, false);
    BHAB_CHK_RETCODE(BHAB_312x_P_DecodeInterrupt(handle));
    BHAB_312x_P_EnableHostInterrupt(handle, true);

done:
    if(retCode != BERR_SUCCESS)
        BHAB_312x_P_EnableHostInterrupt(handle, true);
    BHAB_P_RELEASE_MUTEX(handle);
    return retCode;
}


/******************************************************************************
 BHAB_312x_EnableLockInterrupt()
******************************************************************************/ 
BERR_Code BHAB_312x_EnableLockInterrupt(
    BHAB_Handle handle,  /* [in] BHAB handle */
    BHAB_DevId eDevId,    /* [in] Device ID */
    bool bEnable   /* [in] true = enable Lock  interrupt */
)
{
    BERR_Code retCode = BERR_SUCCESS;
    BSTD_UNUSED(handle);
    BSTD_UNUSED(eDevId);
    BSTD_UNUSED(bEnable);
    
    return retCode;   
}
    

/******************************************************************************
 BHAB_312x_InstallInterruptCallback()
******************************************************************************/ 
BERR_Code BHAB_312x_InstallInterruptCallback(
    BHAB_Handle handle,  /* [in] BHAB handle */
    BHAB_DevId eDevId,    /* [in] Device ID */
    BHAB_IntCallbackFunc fCallBack,
    void * pParm1, 
    int parm2
)
{
    BERR_Code retCode = BERR_SUCCESS;
    BHAB_P_CallbackInfo *callback;
    BHAB_312x_P_Handle *p312x;

    BDBG_ASSERT(handle);

    p312x = (BHAB_312x_P_Handle *)(handle->pImpl);
    BDBG_ASSERT(p312x);

    if (eDevId >= BHAB_DevId_eMax) {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    callback = &p312x->InterruptCallbackInfo[eDevId];

    BKNI_EnterCriticalSection();
    callback->pParm1 = pParm1;
    callback->parm2 = parm2;
    callback->func = fCallBack;
    BKNI_LeaveCriticalSection();

    switch(parm2)
    {
        case 0:
            callback->callbackType |= 0x1;
            break;
        case 1:
            callback->callbackType |= 0x2;
            break;
        case 2:
            callback->callbackType |= 0x4;
            break;
        case 3:
            callback->callbackType |= 0x8;
            break;
        case 5:
            callback->callbackType |= 0x10;
            break;            
        default:
            retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
            break;            
    }

    return retCode;
}


/******************************************************************************
 BHAB_312x_UnInstallInterruptCallback()
******************************************************************************/ 
BERR_Code BHAB_312x_UnInstallInterruptCallback(
    BHAB_Handle handle,  /* [in] BHAB handle */
    BHAB_DevId eDevId    /* [in] Device ID */
)
{
    BERR_Code retCode = BERR_SUCCESS;
    BHAB_P_CallbackInfo *callback;
    BHAB_312x_P_Handle *p312x;

    BDBG_ASSERT(handle);

    p312x = (BHAB_312x_P_Handle *)(handle->pImpl);
    BDBG_ASSERT(p312x);
    
    if (eDevId >= BHAB_DevId_eMax) {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    callback = &p312x->InterruptCallbackInfo[eDevId];


    BKNI_EnterCriticalSection();
    callback->func = NULL;
    callback->pParm1 = NULL;
    callback->parm2 = (int)NULL;
    BKNI_LeaveCriticalSection();

    return retCode;
}   
    

/******************************************************************************
 BHAB_312x_SendHabCommand()
******************************************************************************/
BERR_Code BHAB_312x_SendHabCommand(
    BHAB_Handle handle, /* [in] BHAB PI Handle */
    uint8_t *write_buf, /* [in] specifies the HAB command to send */
    uint16_t write_len,  /* [in] number of bytes in the HAB command */ 
    uint8_t *read_buf,  /* [out] holds the data read from the HAB */ 
    uint16_t read_len,   /* [in] number of bytes to read from the HAB */
    bool bCheckForAck,  /* [in] true = determine if the AP has serviced the command */
    bool bInsertTermination, /* [in] true = insert termination byte 0x00 in write buffer at read_len position */
    uint16_t command_len
)
{
    BERR_Code retCode = BERR_SUCCESS;
    uint32_t sb, datalength = 0;
    uint8_t crc = 0;
    BSTD_UNUSED(command_len);

    BDBG_ASSERT(handle);

    BHAB_P_ACQUIRE_MUTEX(handle);
    
    if ((write_len > BHAB_MEM_SIZE) || (read_len > BHAB_MEM_SIZE) || (write_len == 0)){
        retCode = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }
    
    BHAB_312x_P_EnableHostInterrupt(handle, false);

    *(write_buf+write_len-1) = BHAB_312x_P_CRC8Block(crc, write_buf, write_len-1);
    
    /* write the command to the HAB */
    BHAB_CHK_RETCODE(BHAB_312x_P_WriteHab(handle, 0, write_buf, write_len));

    if(bInsertTermination){
        sb=0x1;
        BHAB_CHK_RETCODE(BHAB_312x_WriteRegister(handle, BCHP_LEAP_CTRL_HAB_REQUEST, &sb));
    }
    /* wait for the AP to service the HAB, and then read any return data */
    BHAB_CHK_RETCODE(BHAB_312x_P_ServiceHab(handle, read_buf, read_len, bCheckForAck, write_buf[0] | 0x80));
    crc = 0;
    datalength = (((*(read_buf+1) & 0x3F) << 4) | (*(read_buf+2) >> 4) ) + 4;
  
    if((read_len != 0) && (*(read_buf+datalength) != BHAB_312x_P_CRC8Block(crc, read_buf, datalength))) {
        BDBG_WRN(("Invalid CRC"));       
        retCode = BERR_TRACE(BHAB_ERR_INVALID_CRC);
    }

    BHAB_CHK_RETCODE(BHAB_312x_P_CheckHab(handle));
    
done:
    if(retCode != BERR_SUCCESS)
        BHAB_312x_P_EnableHostInterrupt(handle, true);
    BHAB_P_RELEASE_MUTEX(handle);
     
    return retCode;
}

/******************************************************************************
 BHAB_312x_GetInterruptEventHandle()
******************************************************************************/
BERR_Code BHAB_312x_GetInterruptEventHandle(
    BHAB_Handle handle,            /* [in] BHAB handle */
    BKNI_EventHandle *hEvent       /* [out] interrupt event handle */
)
{
    BDBG_ASSERT(handle);

    *hEvent = ((BHAB_312x_P_Handle *)(handle->pImpl))->hInterruptEvent;
    return BERR_SUCCESS;
}


/****************************Private Functions*********************************/

static uint8_t BHAB_312x_P_CRC8Byte(
    uint8_t inCRC,
    uint8_t inData
    )
{
    int i;
    unsigned short data;

    data = inCRC ^ inData;
    data <<= 8;
  
    for ( i = 0; i < 8; i++ ) 
    {
        if (( data & 0x8000 ) != 0 )
        {
            data = data ^ (0x1070U << 3);
        }        
        data = data << 1;
    }

    return (uint8_t)(data >> 8 );
}

static uint8_t BHAB_312x_P_CRC8Block(
    uint8_t crc,
    uint8_t *pDataBlock,
    uint32_t dataLength
    )
{
    uint32_t i;

    for ( i = 0; i < dataLength; i++ ) 
    {
        crc = BHAB_312x_P_CRC8Byte(crc, *pDataBlock++ );
    }
   
    return crc;
}

/******************************************************************************
 BHAB_312x_P_DumpRegisters()
******************************************************************************/
BERR_Code BHAB_312x_P_DumpRegisters(
    BHAB_Handle handle
)
{
    BERR_Code retCode = BERR_SUCCESS;
    uint32_t i, buf=0;

    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_LEAP_HOST_IRQ_STATUS0, &buf));
    BDBG_WRN(("BCHP_LEAP_HOST_IRQ_STATUS0 %x", buf));
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_LEAP_HOST_IRQ_STATUS1, &buf));
    BDBG_WRN(("BCHP_LEAP_HOST_IRQ_STATUS1 %x", buf));
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_LEAP_HOST_IRQ_SET0, &buf));
    BDBG_WRN(("BCHP_LEAP_HOST_IRQ_SET0 %x", buf));    
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_LEAP_HOST_IRQ_CLEAR0, &buf));
    BDBG_WRN(("BCHP_LEAP_HOST_IRQ_CLEAR0 %x", buf));    
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_LEAP_HOST_IRQ_MASK_STATUS0, &buf));
    BDBG_WRN(("BCHP_LEAP_HOST_IRQ_MASK_STATUS0 %x", buf));
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_LEAP_HOST_IRQ_MASK_SET0, &buf));
    BDBG_WRN(("BCHP_LEAP_HOST_IRQ_MASK_SET0 %x", buf));    
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_LEAP_HOST_IRQ_MASK_CLEAR0, &buf));
    BDBG_WRN(("BCHP_LEAP_HOST_IRQ_MASK_CLEAR0 %x", buf));   
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_TM_SFT7, &buf));
    BDBG_WRN(("BCHP_TM_SFT7 %x", buf));    
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_TM_SFT6, &buf));
    BDBG_WRN(("BCHP_TM_SFT6 %x", buf));   
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_TM_SFT5, &buf));
    BDBG_WRN(("BCHP_TM_SFT5 %x", buf));   
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_TM_SFT4, &buf));
    BDBG_WRN(("BCHP_TM_SFT4 %x", buf));   
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_TM_SFT3, &buf));
    BDBG_WRN(("BCHP_TM_SFT3 %x", buf));   
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_TM_SFT2, &buf));
    BDBG_WRN(("BCHP_TM_SFT2 %x", buf));   
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_TM_SFT1, &buf));
    BDBG_WRN(("BCHP_TM_SFT1 %x", buf));   
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_TM_SFT0, &buf));
    BDBG_WRN(("BCHP_TM_SFT0 %x", buf));
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_TM_MISC0, &buf));
    BDBG_WRN(("BCHP_TM_MISC0 %x", buf));  
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_TM_MISC1, &buf));
    BDBG_WRN(("BCHP_TM_MISC1 %x", buf));  
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_TM_MISC2, &buf));
    BDBG_WRN(("BCHP_TM_MISC2 %x", buf));      
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_TM_MISC3, &buf));
    BDBG_WRN(("BCHP_TM_MISC3 %x", buf));      
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, 0x78000, &buf));
    BDBG_WRN(("HAB first word %x", buf));    
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, 0x78004, &buf));
    BDBG_WRN(("HAB second word %x", buf));    
#ifdef BHAB_DEBUG   
    for(i=0; i < 20; i++) {     
#else
    for(i=0; i < 1; i++) {
#endif    
    buf = 0;
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_LEAP_CTRL_SW_SPARE0, &buf));
    BDBG_WRN(("BCHP_LEAP_CTRL_SW_SPARE0 %x", buf));
    buf = 0;
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_LEAP_CTRL_SW_SPARE1, &buf));
    BDBG_WRN(("BCHP_LEAP_CTRL_SW_SPARE1 %x", buf));    
    buf = 0;
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_LEAP_CTRL_SW_SPARE2, &buf));
    BDBG_WRN(("BCHP_LEAP_CTRL_SW_SPARE2 %x", buf));    
    buf = 0;
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_LEAP_CTRL_SW_SPARE3, &buf));
    BDBG_WRN(("BCHP_LEAP_CTRL_SW_SPARE3 %x", buf));   
    BKNI_Sleep(250);
    }
 
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_LEAP_HOST_IRQ_STATUS0, &buf));
    BDBG_WRN(("BCHP_LEAP_HOST_IRQ_STATUS0 %x", buf));
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_LEAP_HOST_IRQ_STATUS1, &buf));
    BDBG_WRN(("BCHP_LEAP_HOST_IRQ_STATUS1 %x", buf));    
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_LEAP_HOST_IRQ_SET0, &buf));
    BDBG_WRN(("BCHP_LEAP_HOST_IRQ_SET0 %x", buf));    
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_LEAP_HOST_IRQ_CLEAR0, &buf));
    BDBG_WRN(("BCHP_LEAP_HOST_IRQ_CLEAR0 %x", buf));
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_LEAP_HOST_IRQ_MASK_STATUS0, &buf));
    BDBG_WRN(("BCHP_LEAP_HOST_IRQ_MASK_STATUS0 %x", buf));
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_LEAP_HOST_IRQ_MASK_SET0, &buf));
    BDBG_WRN(("BCHP_LEAP_HOST_IRQ_MASK_SET0 %x", buf));    
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_LEAP_HOST_IRQ_MASK_CLEAR0, &buf));
    BDBG_WRN(("BCHP_LEAP_HOST_IRQ_MASK_CLEAR0 %x", buf));
    
done:
    return retCode;
}

/******************************************************************************
 BREG_I2C_P_Read312x()
******************************************************************************/
BERR_Code BREG_I2C_P_Read312x(
    BREG_I2C_Handle i2cHandle,    /* [in] BREG_I2C Handle */
    uint16_t chipAddr, 
    uint8_t subAddr, 
    uint8_t *pData, 
    size_t length
)
{
    BERR_Code retCode = BERR_SUCCESS;
    BDBG_ASSERT(i2cHandle);

    BHAB_CHK_RETCODE(BREG_I2C_Read(i2cHandle, chipAddr, subAddr, pData, length));   
  
done:  
    return retCode;
}


/******************************************************************************
 BHAB_312x_P_EnableHostInterrupt() 
******************************************************************************/
BERR_Code BHAB_312x_P_EnableHostInterrupt(
    BHAB_Handle handle, /* [in] HAB handle */
    bool bEnable        /* [in] true=enables the L1 interrupt on the host processor */
)
{
    BDBG_ASSERT(handle);
   
    BKNI_EnterCriticalSection();
    handle->settings.interruptEnableFunc(bEnable, handle->settings.interruptEnableFuncParam);
    BKNI_LeaveCriticalSection();   

    return BERR_SUCCESS;
}


/******************************************************************************
 BHAB_312x_P_DisableInterrupts()
******************************************************************************/ 
BERR_Code BHAB_312x_P_DisableInterrupts(
    BHAB_Handle handle   /* [in] BHAB Handle */
)
{   
    BERR_Code retCode = BERR_SUCCESS;  
    BSTD_UNUSED(handle);
    
    return retCode;  
}


/******************************************************************************
 BHAB_312x_P_EnableHabDoneInterrupt()
******************************************************************************/ 
BERR_Code BHAB_312x_P_EnableHabDoneInterrupt(
    BHAB_Handle handle /* [in] BHAB PI Handle */
)
{  
    BERR_Code retCode = BERR_SUCCESS;  
    BSTD_UNUSED(handle);
    
    return retCode;
}


/******************************************************************************
 BHAB_312x_P_EnableInitDoneInterrupt()
******************************************************************************/ 
BERR_Code BHAB_312x_P_EnableInitDoneInterrupt(
    BHAB_Handle handle /* [in] BHAB PI Handle */
)
{  
    BERR_Code retCode = BERR_SUCCESS;
    BSTD_UNUSED(handle);
    
    return retCode;
}


/******************************************************************************
 BERR_312x_Code BHAB_P_WaitForEvent()
******************************************************************************/
BERR_Code BHAB_312x_P_WaitForEvent(
    BHAB_Handle handle,             /* [in] BHAB PI Handle */
    BKNI_EventHandle hEvent,   /* [in] event to wait on */
    int timeoutMsec            /* [in] timeout in milliseconds */
)
{
    BERR_Code retCode = BERR_SUCCESS;
    BHAB_312x_P_Handle *p312x;

    BDBG_ASSERT(handle);

    p312x = (BHAB_312x_P_Handle *)(handle->pImpl);
    BDBG_ASSERT(p312x);
    
    /* Temporary fix till B0 312x chip as that provides maskable Host IRQ register. */
    while(1) {
        BHAB_312x_P_EnableHostInterrupt(handle, true);
        retCode = BKNI_WaitForEvent(p312x->hApiEvent, timeoutMsec);
        if ( retCode != BERR_SUCCESS ) { break; }
        BHAB_312x_P_EnableHostInterrupt(handle, false);
        BHAB_CHK_RETCODE(BHAB_312x_P_DecodeInterrupt(handle));
        retCode = BKNI_WaitForEvent(hEvent, 0);
        if (retCode == BERR_SUCCESS ) { break; }
    }

    BHAB_312x_P_EnableHostInterrupt(handle, true);

done:    
    return retCode;
}


/******************************************************************************
 BHAB_312x_P_RunAp()
******************************************************************************/
BERR_Code BHAB_312x_P_RunAp(BHAB_Handle handle)
{
    BERR_Code retCode = BERR_SUCCESS;
    uint32_t buf;
    
    BDBG_ASSERT(handle);  
    
    BHAB_312x_P_EnableHostInterrupt(handle, true);

    /* start running the AP */
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_LEAP_CTRL_CTRL, &buf));
    buf &= ~0x0F; 
    BHAB_CHK_RETCODE(BHAB_312x_WriteRegister(handle, BCHP_LEAP_CTRL_CTRL, &buf));  

done:
    return retCode;
}


/******************************************************************************
 BHAB_312x_P_ResetAp()
******************************************************************************/
BERR_Code BHAB_312x_P_ResetAp(BHAB_Handle handle)
{   
    BERR_Code retCode = BERR_SUCCESS;
    uint32_t buf;
    uint8_t buf1;
    uint8_t writebuf[8];    
    BHAB_312x_P_Handle *p312x;

    BDBG_ASSERT(handle);

    p312x = (BHAB_312x_P_Handle *)(handle->pImpl);
    BDBG_ASSERT(p312x);        
    /* clear LEAP_HOST_IRQ and put LEAP in reset */
    buf=0xFFFFFFFF;
    BHAB_CHK_RETCODE(BHAB_312x_WriteRegister(handle, BCHP_LEAP_HOST_IRQ_CLEAR0, &buf));
    BHAB_CHK_RETCODE(BHAB_312x_WriteRegister(handle, BCHP_LEAP_HOST_IRQ_CLEAR1, &buf));  

    if(p312x->isSpi){
        writebuf[0] = (handle->settings.chipAddr << 1) | 0x1;
        writebuf[1] = HIF_SFT_RST;
        writebuf[2] = 0x1;
        BHAB_CHK_RETCODE(BREG_SPI_Write (p312x->hSpiRegister,  writebuf, 3));
        writebuf[2] = 0x0;
        BHAB_CHK_RETCODE(BREG_SPI_Write (p312x->hSpiRegister,  writebuf, 3));   
    }   
    else {	
        buf1 = 0xFF;
        BHAB_CHK_RETCODE(BREG_I2C_Write(p312x->hI2cRegister, handle->settings.chipAddr, HIF_SFT_RST, (uint8_t *)&buf1, 1));
        buf1 = 0;
        BHAB_CHK_RETCODE(BREG_I2C_Write(p312x->hI2cRegister, handle->settings.chipAddr, HIF_SFT_RST, (uint8_t *)&buf1, 1));
    }
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_LEAP_CTRL_CTRL, &buf));
    buf &= ~0x0F;
    buf |= 0x0F;
    BHAB_CHK_RETCODE(BHAB_312x_WriteRegister(handle, BCHP_LEAP_CTRL_CTRL, &buf));

    buf =0;
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_TM_MTSIF_CTRL, &buf)); 
    if(p312x->isMtsif)
       buf &= ~0x01;
    else
       buf |= 0x1; 
    BHAB_CHK_RETCODE(BHAB_312x_WriteRegister(handle, BCHP_TM_MTSIF_CTRL, &buf));
done:
    return retCode;
}


/******************************************************************************
 BHAB_312x_P_ReadHab()
******************************************************************************/
BERR_Code BHAB_312x_P_ReadHab(BHAB_Handle handle, uint8_t addr, uint8_t *buf, uint16_t n)
{
   
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ASSERT(handle);
     
    if ((addr & BHAB_HAB_SIZE) || (n > BHAB_HAB_SIZE))
        return BERR_TRACE(BERR_INVALID_PARAMETER);

    if ((addr + n) > BHAB_HAB_SIZE)
        return BERR_TRACE(BERR_INVALID_PARAMETER);

    BHAB_CHK_RETCODE(BHAB_312x_ReadMemory(handle, BCHP_LEAP_HAB_MEM_WORDi_ARRAY_BASE, buf, n));

done:
    return retCode;
}


/******************************************************************************
 BHAB_312x_P_WriteHab()
******************************************************************************/
BERR_Code BHAB_312x_P_WriteHab(BHAB_Handle handle, uint8_t addr, uint8_t *buf, uint8_t n)
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ASSERT(handle);

    if ((addr & BHAB_HAB_SIZE) || (n & BHAB_HAB_SIZE))
        return BERR_TRACE(BERR_INVALID_PARAMETER);

    if ((addr + n) > BHAB_HAB_SIZE)
        return BERR_TRACE(BERR_INVALID_PARAMETER);
     
    BHAB_CHK_RETCODE(BHAB_312x_WriteMemory(handle, (BCHP_LEAP_HAB_MEM_WORDi_ARRAY_BASE + addr), buf, n));

done:
    return retCode;
}

/******************************************************************************
 BHAB_312x_P_ServiceHab()
******************************************************************************/
BERR_Code BHAB_312x_P_ServiceHab(
    BHAB_Handle handle,   /* [in] BHAB PI Handle */
    uint8_t *read_buf,  /* [out] holds the data read from the HAB */ 
    uint16_t read_len,   /* [in] number of bytes to read from the HAB */
    bool bCheckForAck,  /* [in] true = determine if the AP has serviced the command */
    uint8_t ack_byte    /* [in] value of the ack byte to expect */
)
{   
    BERR_Code retCode = BERR_SUCCESS;   

    BHAB_312x_P_Handle *p312x;
    BDBG_ASSERT(handle);

    p312x = (BHAB_312x_P_Handle *)(handle->pImpl);
    BDBG_ASSERT(p312x);
    
    if (BHAB_312x_P_WaitForEvent(handle, p312x->hHabDoneEvent, 10100) == BERR_TIMEOUT)
    {
        BDBG_ERR(("HAB timeout\n"));   
        BDBG_WRN(("Dumping 312x Registers\n"));
        BHAB_312x_P_DumpRegisters(handle);
#ifdef BHAB_DEBUG         
        BDBG_ASSERT(false);        
#endif        
        BERR_TRACE(retCode = BHAB_ERR_HAB_TIMEOUT);     
        goto done;
    }
    
    if (read_len > 0)
    {
        BHAB_CHK_RETCODE(BHAB_312x_P_ReadHab(handle, 0, read_buf, read_len));
        if (bCheckForAck)
        {
            if (ack_byte != read_buf[0])
            {
                BDBG_ERR(("HAB command not serviced!\n"));
                BERR_TRACE(retCode = BHAB_ERR_HAB_NO_ACK);
            }
        }
    }

done:
    return retCode;
}

/******************************************************************************
 BHAB_312x_P_DecodeInterrupt()
******************************************************************************/
BERR_Code BHAB_312x_P_DecodeInterrupt(BHAB_Handle handle)
{  
    BERR_Code retCode = BERR_SUCCESS;
    BHAB_312x_P_Handle *p312x;
    BHAB_P_CallbackInfo *callback;    
    uint32_t   buf, buf1, sb;
    unsigned i;
    
    BDBG_ASSERT(handle);
    
    p312x = (BHAB_312x_P_Handle *)(handle->pImpl);
    BDBG_ASSERT(p312x);
    
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_LEAP_HOST_IRQ_STATUS0, &buf));
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_LEAP_HOST_IRQ_STATUS1, &buf1));

    if((buf == 0) &&  (buf1 == 0))
        return retCode;

    /*HAB DONE INTERRUPT*/
    if (buf & BHAB_HAB_DONE)
    {
        BDBG_MSG(("HAB_DONE"));      
        BKNI_SetEvent_isr(p312x->hHabDoneEvent);
    }
   
    /*INIT DONE INTERRUPT*/
    if (buf & BHAB_AP_INIT_DONE)
    {
        BDBG_MSG(("AP INIT DONE"));    
        BKNI_SetEvent_isr(p312x->hInitDoneEvent);
    }

    /* ADS interrupts */   
    for(i = 0; i < 8; i++)
    {
        if((buf & (BHAB_ADS_CHN0_NO_SIGNAL << 2*i)) == (uint32_t)(BHAB_ADS_CHN0_IN_LOCK << 2*i))
        {
            if((p312x->InterruptCallbackInfo[BHAB_DevId_eADS0 + i].func) && (p312x->InterruptCallbackInfo[BHAB_DevId_eADS0 + i].callbackType & 0x1)){
                BDBG_MSG(("Channel %d Locked", i));
                callback = &p312x->InterruptCallbackInfo[BHAB_DevId_eADS0 + i];
                callback->parm2 = (int) BHAB_Interrupt_eLockChange;        
                BKNI_EnterCriticalSection();
                callback->func(callback->pParm1, callback->parm2);
                BKNI_LeaveCriticalSection();
            }
            else {
                BDBG_WRN(("Channel %d LockChange callback not installed. ", i));
            }
        }
            
        if((buf & (BHAB_ADS_CHN0_NO_SIGNAL << 2*i)) == (uint32_t)(BHAB_ADS_CHN0_OUT_OF_LOCK << 2*i))
        {
            if((p312x->InterruptCallbackInfo[BHAB_DevId_eADS0 + i].func) && (p312x->InterruptCallbackInfo[BHAB_DevId_eADS0 + i].callbackType & 0x1)){
                BDBG_MSG(("Channel %d Unlocked", i));             
                callback = &p312x->InterruptCallbackInfo[BHAB_DevId_eADS0 + i];
                callback->parm2 = (int) BHAB_Interrupt_eLockChange;        
                BKNI_EnterCriticalSection();
                callback->func(callback->pParm1, callback->parm2);
                BKNI_LeaveCriticalSection();
            }
            else {
                BDBG_WRN(("Channel %d LockChange callback not installed. ", i));
            }
        }

        if((buf & (BHAB_ADS_CHN0_NO_SIGNAL << 2*i)) == (uint32_t)(BHAB_ADS_CHN0_NO_SIGNAL << 2*i))            

        {
            if((p312x->InterruptCallbackInfo[BHAB_DevId_eADS0 + i].func) && (p312x->InterruptCallbackInfo[BHAB_DevId_eADS0 + i].callbackType & 0x4)){
                BDBG_MSG(("Channel %d found No Signal", i));               
                callback = &p312x->InterruptCallbackInfo[BHAB_DevId_eADS0 + i];
                callback->parm2 = (int) BHAB_Interrupt_eNoSignal;        
                BKNI_EnterCriticalSection();
                callback->func(callback->pParm1, callback->parm2);
                BKNI_LeaveCriticalSection();
            }
            else {
                BDBG_WRN(("Channel %d No Signal callback not installed. ", i));
            }
        }        
                
    }
        
    if (buf & BHAB_ADS_STATUS_RDY)
    {        
        BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_LEAP_CTRL_SW_SPARE0, &sb));
    
        for(i = 0; i < 8; i++)
        {
            if (sb & (BHAB_ADS_CHN0_STATUS_RDY << i))
            {
                if((p312x->InterruptCallbackInfo[BHAB_DevId_eADS0 + i].func) && (p312x->InterruptCallbackInfo[BHAB_DevId_eADS0 + i].callbackType & 0x8)){
                    BDBG_MSG(("Channel %d Status Rdy", i));                 
                    callback = &p312x->InterruptCallbackInfo[BHAB_DevId_eADS0 + i];
                    callback->parm2 = (int) BHAB_Interrupt_eQamAsyncStatusReady;     
                    BKNI_EnterCriticalSection();
                    callback->func(callback->pParm1, callback->parm2);
                    BKNI_LeaveCriticalSection();
                }
                else
                    BDBG_WRN(("Channel %d Status Ready callback not installed. ", i));
            }
        }
    }

    /* Out of band interrupts */
    if((buf & BHAB_OOB_NO_SIGNAL) == BHAB_OOB_OUT_OF_LOCK)
    {
        if(p312x->InterruptCallbackInfo[BHAB_DevId_eAOB0].func){
                BDBG_MSG(("OOB out of Lock"));        
                callback = &p312x->InterruptCallbackInfo[BHAB_DevId_eAOB0];      
                callback->parm2 = (int) BHAB_Interrupt_eLockChange;        
                BKNI_EnterCriticalSection();
                callback->func(callback->pParm1, callback->parm2);
                BKNI_LeaveCriticalSection();
        }
        else {
            BDBG_WRN(("OOB LockChange callback not installed. "));
        }
    }

    if((buf & BHAB_OOB_NO_SIGNAL) == BHAB_OOB_IN_LOCK)    
    {
        if(p312x->InterruptCallbackInfo[BHAB_DevId_eAOB0].func){    
            BDBG_MSG(("OOB In Lock"));        
            callback = &p312x->InterruptCallbackInfo[BHAB_DevId_eAOB0];      
            callback->parm2 = (int) BHAB_Interrupt_eLockChange;        
            BKNI_EnterCriticalSection();
            callback->func(callback->pParm1, callback->parm2);
            BKNI_LeaveCriticalSection();
        }
        else {
            BDBG_WRN(("OOB LockChange callback not installed. "));
        }            
    }    
    
    if((buf & BHAB_OOB_NO_SIGNAL) == BHAB_OOB_NO_SIGNAL)    
    {
        if(p312x->InterruptCallbackInfo[BHAB_DevId_eAOB0].func){    
            BDBG_MSG(("OOB No Signal"));        
            callback = &p312x->InterruptCallbackInfo[BHAB_DevId_eAOB0];      
            callback->parm2 = (int) BHAB_Interrupt_eNoSignal;        
            BKNI_EnterCriticalSection();
            callback->func(callback->pParm1, callback->parm2);
            BKNI_LeaveCriticalSection();
        }
        else {
            BDBG_WRN(("OOB No Signal callback not installed. "));
        }            
    }    
        
    if (buf & BHAB_OOB_STATUS_RDY)
    {
        if(p312x->InterruptCallbackInfo[BHAB_DevId_eAOB0].func){   
            BDBG_MSG(("OOB Status Ready"));          
            callback = &p312x->InterruptCallbackInfo[BHAB_DevId_eAOB0];      
            callback->parm2 = (int) BHAB_Interrupt_eOobAsyncStatusReady; 
            BKNI_EnterCriticalSection();
            callback->func(callback->pParm1, callback->parm2);
            BKNI_LeaveCriticalSection();
        }
        else {
            BDBG_WRN(("OOB callback not installed. "));
        }            
    }
        
    if ((buf1 & BHAB_TNR_SPECTRUM_DATA_RDY) == BHAB_TNR_SPECTRUM_DATA_RDY)
    {   
        if(p312x->InterruptCallbackInfo[BHAB_DevId_eTNR0].func){
            BDBG_MSG(("Spectrum Data Rdy"));                 
            callback = &p312x->InterruptCallbackInfo[BHAB_DevId_eTNR0];
            callback->parm2 = (int) BHAB_Interrupt_eSpectrumAnalyzerDataReady;
            callback->func(callback->pParm1, callback->parm2);
        }
        else
            BDBG_WRN(("Spectrum data ready callback not installed. "));                
    }      
      
        /* clear LEAP_HOST_IRQ and LEAP_CTRL_SPARE */
    if(buf != 0)
        BHAB_CHK_RETCODE(BHAB_312x_WriteRegister(handle, BCHP_LEAP_HOST_IRQ_CLEAR0, &buf));
    if(buf1 != 0)
        BHAB_CHK_RETCODE(BHAB_312x_WriteRegister(handle, BCHP_LEAP_HOST_IRQ_CLEAR1, &buf1));
  
done:
    return retCode;
}


/******************************************************************************
 BHAB_312x_P_DecodeError()
******************************************************************************/
BERR_Code BHAB_312x_P_DecodeError(
    BHAB_Handle handle,           /* [in] BHAB PI Handle */
    BHAB_ApStatus *pApStatus /* [in] AP status returned by BHAB_GetApStatus */
)
{
    BERR_Code retCode = BERR_SUCCESS;
    uint8_t sb;
    BSTD_UNUSED(handle);

    if (*pApStatus & 0xF)
    {
        sb = *pApStatus & 0XF;
        
        switch (sb)
        {
            case 1:
              break;
              
            case 2:
                BDBG_ERR(("Invalid Opcode"));
                retCode = BHAB_ERR_INVALID_OPCODE;
                break;          

            case 3:
                BDBG_ERR(("HAB CRC/length error"));        
                retCode = BHAB_ERR_INVALID_CRC;
                break;

            case 5:
                BDBG_ERR(("LEAP HAB command processing timeout error"));        
                retCode = BHAB_ERR_HAB_TIMEOUT;
                break;

            case 6:
                BDBG_ERR(("LEAP HAB command processing error"));        
                retCode = BHAB_ERR_CMD_PROCESSING_ERR;
                break;
              
            default:         
                retCode = BHAB_ERR_AP_UNKNOWN;
                break;
        }
    }
    
    return retCode;
}


/******************************************************************************
 BHAB_312x_P_CheckHab()
******************************************************************************/
BERR_Code BHAB_312x_P_CheckHab(
    BHAB_Handle handle    /* [in] BHAB Handle */
)
{
    BERR_Code retCode = BERR_SUCCESS;
    BHAB_ApStatus status = 0;
    uint32_t buf;

    BDBG_ASSERT(handle);
  
    BHAB_CHK_RETCODE(BHAB_312x_GetApStatus(handle, &status));   
    BHAB_CHK_RETCODE(BHAB_312x_ReadRegister(handle, BCHP_LEAP_HOST_IRQ_STATUS0, &buf));   
    BDBG_MSG(("BHAB_312x_P_CheckHab status = 0x%x", status ));
    BDBG_MSG(("BHAB_312x_P_CheckHab IRQ register =  0x%x", buf ));

    if ((status & 0xF) == 0x1) {
        retCode = BERR_SUCCESS;
    }
    else
    {
        BDBG_MSG(("BHAB_312x_P_DecodeError called. 0x%x", status ));
        retCode = BHAB_312x_P_DecodeError(handle, &status);
    }
   
done:
    return retCode;
}


/******************************************************************************
 BHAB_312x_GetWatchDogTimer()
******************************************************************************/
BERR_Code BHAB_312x_GetWatchDogTimer(
    BHAB_Handle handle,    /* [in] BHAB Handle */
    BHAB_WatchDogTimerSettings *wdtSettings
)
{
    BERR_Code retCode = BERR_SUCCESS;
    BSTD_UNUSED(handle);
    BSTD_UNUSED(wdtSettings);
    
     return retCode;
}


/******************************************************************************
 BHAB_312x_SetWatchDogTimer()
******************************************************************************/
BERR_Code BHAB_312x_SetWatchDogTimer(
    BHAB_Handle handle,    /* [in] BHAB Handle */
    const BHAB_WatchDogTimerSettings *wdtSettings
)
{
    BERR_Code retCode = BERR_SUCCESS;
    BSTD_UNUSED(handle);
    BSTD_UNUSED(wdtSettings);
    
    return retCode;
}


/******************************************************************************
 BHAB_312x_GetNmiConfig()
******************************************************************************/
BERR_Code BHAB_312x_GetNmiConfig(
    BHAB_Handle handle,    /* [in] BHAB Handle */
    BHAB_NmiSettings *nmiSettings
)
{
    BERR_Code retCode = BERR_SUCCESS;
    BSTD_UNUSED(handle);
    BSTD_UNUSED(nmiSettings);
    
    return retCode;
}


/******************************************************************************
 BHAB_312x_SetNmiConfig()
******************************************************************************/
BERR_Code BHAB_312x_SetNmiConfig(
    BHAB_Handle handle,    /* [in] BHAB Handle */
    const BHAB_NmiSettings *nmiSettings
)
{
    BERR_Code retCode = BERR_SUCCESS;
    BSTD_UNUSED(handle);
    BSTD_UNUSED(nmiSettings);
    
    return retCode;
}

/***************************************************************************
 BHAB_312x_GetConfigSettings()
****************************************************************************/
BERR_Code BHAB_312x_GetConfigSettings(
    BHAB_Handle handle,           /* [in] Device handle */
    BHAB_ConfigSettings *settings)     /* [out] HAB config settings. */
{
    BERR_Code retCode = BERR_SUCCESS;
    uint8_t buf[9] = HAB_MSG_HDR(BHAB_LOOP_THROUGH_READ, 0, BHAB_CORE_TYPE, BHAB_CORE_ID);

    BDBG_ASSERT(handle);         
    BHAB_CHK_RETCODE(BHAB_312x_SendHabCommand(handle, buf, 5, buf, 9, false, true, 9));
    settings->enableLoopThrough = buf[4] & 0x1;

done:     
    return retCode;    
}
    
/***************************************************************************
 BHAB_312x_SetConfigSettings()
****************************************************************************/
BERR_Code BHAB_312x_SetConfigSettings(
    BHAB_Handle handle,           /* [in] Device handle */
    const BHAB_ConfigSettings *settings)     /* [in] HAB config settings. */
{
    BERR_Code retCode = BERR_SUCCESS;
    uint8_t buf[9] = HAB_MSG_HDR(BHAB_LOOP_THROUGH_WRITE, 0x4, BHAB_CORE_TYPE, BHAB_CORE_ID);
    
    BDBG_ASSERT(handle);  
    if(settings->enableLoopThrough)
    {    
        buf[4] = 0x1;
        BHAB_CHK_RETCODE(BHAB_312x_SendHabCommand(handle, buf, 9, buf, 0, false, true, 9));
    }        

done:    
    return retCode;    
}

/******************************************************************************
 BHAB_312x_ReadSlave()
******************************************************************************/
BERR_Code BHAB_312x_ReadSlave(
    BHAB_Handle handle,     /* [in] Device channel handle */
    uint8_t chipAddr,       /* [in] chip addr of the i2c slave device */
    uint32_t subAddr,       /* [in] sub addr of the register to read from the slave device */
    uint32_t *data,         /* [out] ptr to the data that we will read from the slave device */
    uint8_t dataLen)        /* [in] how many bytes are we going to read? one to four, For 3412 LNA, this should be in multiples of 4-bytes*/
{
    BERR_Code retCode = BERR_SUCCESS;
    uint8_t hab[13] = HAB_MSG_HDR(BLNA_READ_REGISTER, 0x4, CORE_TYPE_GLOBAL, BLNA_CORE_ID);
    
    BDBG_ENTER(BHAB_312x_ReadSlave);
    BDBG_ASSERT( handle );
    if(dataLen != 4)
    {
        BDBG_ERR(("dataLen can only be 4 for 3128"));
        BERR_TRACE(retCode = BERR_NOT_SUPPORTED);      
    }
    else
    {
        hab[4] = chipAddr;
        hab[5] = subAddr >> 2;
        hab[7] = dataLen;
     
        BHAB_CHK_RETCODE(BHAB_SendHabCommand(handle, hab, 9, hab, 13, false, true, 13));
            *data = (hab[11] << 24) | (hab[10] << 16) | (hab[9] << 8) | hab[8];   
    }
    
done:    
    BDBG_LEAVE(BHAB_312x_ReadSlave);              
    return retCode; 
}

/******************************************************************************
 BHAB_312x_WriteSlave()
******************************************************************************/
BERR_Code BHAB_312x_WriteSlave(
    BHAB_Handle handle,     /* [in] Device channel handle */
    uint8_t chipAddr,       /* [in] chip addr of the i2c slave device */
    uint32_t subAddr,       /* [in] sub addr of the register to read from the slave device */
    uint32_t *data,         /* [in] ptr to the data that we will write to the slave device */
    uint8_t dataLen)        /* [in] how many bytes are we going to write? one to four. For 3412 LNA, this should be in multiples of 4-bytes*/
{
    BERR_Code retCode = BERR_SUCCESS;
    uint8_t hab[13] = HAB_MSG_HDR(BLNA_WRITE_REGISTER, 0x8, CORE_TYPE_GLOBAL, BLNA_CORE_ID);    
    
    BDBG_ENTER(BHAB_312x_WriteSlave);
    BDBG_ASSERT( handle );
    if(dataLen != 4)
    {
        BDBG_ERR(("dataLen can only be 4 for 3128"));
        BERR_TRACE(retCode = BERR_NOT_SUPPORTED);      
    }
    else
    {
        hab[4] = chipAddr;
        hab[5] = subAddr >> 2;
        hab[7] = dataLen;
        hab[8] = *data & 0xFF;
        hab[9] = (*data >> 8) & 0xFF;
        hab[10] = (*data >> 16) & 0xFF;
        hab[11] = (*data >> 24) & 0xFF;      
        BHAB_CHK_RETCODE(BHAB_SendHabCommand(handle, hab, 13, hab, 0, false, true, 13));
    }

done:    
    BDBG_LEAVE(BLNA_3412_WriteRegister);              
    return retCode;
}
