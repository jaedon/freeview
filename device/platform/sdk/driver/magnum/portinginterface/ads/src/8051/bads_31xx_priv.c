/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bads_31xx_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 10/10/12 4:31p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ads/src/8051/bads_31xx_priv.c $
 * 
 * Hydra_Software_Devel/2   10/10/12 4:31p mward
 * SW7435-403:  Don't get more than nbrToGet in
 * BADS_31xx_GetSoftDecision().
 * 
 * Hydra_Software_Devel/1   6/21/12 3:34p vishk
 * SW7425-2983: Remove symbolic links and flatten out ADS and AOB
 * directory structure
 * 
 * Hydra_Software_Devel/47   2/6/12 11:48a vishk
 * SW7231-589: Extend NEXUS_Frontend_GetFastStatus() for 3109 frontends
 * 
 * Hydra_Software_Devel/46   11/16/11 6:04p atanugul
 * SW3461-79: Add No Signal to GetLockStatus()
 * 
 * Hydra_Software_Devel/45   11/3/11 3:59p vishk
 * SW3461-80: Means of returning BCM31xx chip revsion and firmware
 * revision via NEXUS i/f
 * 
 * Hydra_Software_Devel/44   10/19/11 4:55p vishk
 * SW7425-1501: Numberous bads_31xx_priv: BADS No Signal messages after
 * doing stream playback or PIP docode.
 * 
 * Hydra_Software_Devel/43   8/8/11 12:22p mward
 * SW7125-1060: SW7405-5021: increase hab[] buffer in
 * BADS_31xx_ReadSlave().
 * 
 * Hydra_Software_Devel/42   8/8/11 12:04p mward
 * SW7125-1059: SW7405-5021: increase hab[] buffer in
 * BADS_31xx_WriteSlave().
 * 
 * Hydra_Software_Devel/41   7/6/11 3:22p vishk
 * SW7125-983: SW7125-1017: [Kbro 7019] Tuner 3109 AGC Return Value Not
 * Accurate Accumulated counters for 7125 & 3255 frontends
 * 
 * Hydra_Software_Devel/40   6/8/11 12:14p mward
 * SW7231-180:  warning: unused variable 'freqOffset'
 * 
 * Hydra_Software_Devel/39   5/27/11 1:56p vishk
 * SW7231-180: Remove duplicate member variables in bads.h and convert
 * BADS_AutomaticFrequencyOffset and NEXUS_FrontendQamFrequencyOffset
 * enum types to unsigned integer.
 * 
 * Hydra_Software_Devel/38   3/2/11 3:17p VISHK
 * SW7405-5166: Need a new function to return the FEC RS block error
 * counters on bcm3117
 * 
 * Hydra_Software_Devel/37   12/7/10 1:08p VISHK
 * SW7422-119: BCM3112 tuners' initialization fails on BCM97422SVMB board
 * 
 * Hydra_Software_Devel/36   11/24/10 7:19p agin
 * SW7405-5021:  Allow 7405 host to control LNA via 31xx tuner/demod
 * chips.
 * 
 * Hydra_Software_Devel/35   10/21/10 7:21p VISHK
 * SW7420-1020: Request for non-blocking call support in Nexus frontend
 * 
 * Hydra_Software_Devel/34   10/21/10 7:14p VISHK
 * SW7420-1020: Request for non-blocking call support in Nexus frontend
 * 
 * Hydra_Software_Devel/33   10/21/10 7:12p VISHK
 * SW7420-1020: Request for non-blocking call support in Nexus frontend
 * 
 * Hydra_Software_Devel/32   10/21/10 6:52p vishk
 * SW7420-1020: Request for non-blocking call support in Nexus frontend
 * 
 * Hydra_Software_Devel/31   10/5/10 7:29p VISHK
 * SW7405-4915: Add nexus api to control power saver mode in downstream
 * demod and tuner, out of banddemod and tuner and upstream. Also add
 * nexus api to control daisy chaining of the frontends.
 * 
 * Hydra_Software_Devel/30   8/24/10 11:28a VISHK
 * SW7405-4808 : Connect reset status api for ADS and AOB to nexus api.
 * 
 * Hydra_Software_Devel/29   8/5/10 4:43p VISHK
 * SW7405-4722: Add support for "Frequency offset" and "Null packets" for
 * ADS and expose "auto acquire" in nexus for AOB
 * 
 * Hydra_Software_Devel/28   7/2/10 4:26p vishk
 * SW7420-873: Add support to inform application to update frontend tuner
 * gain and to inform that there is no signal to tune upon multiple tries
 * to acquire
 * 
 * Hydra_Software_Devel/27   3/8/10 8:44p VISHK
 * SW7405-3995: Support spectrum mode and spectrum inversion control from
 * nexus.
 * 
 * Hydra_Software_Devel/26   1/13/10 6:54p VISHK
 * SW7405-3722: BCM3109: Signal power reported is not correct
 * 
 * Hydra_Software_Devel/25   12/18/09 2:32p VISHK
 * SW7405-3580: BADS_ResetStatus checks the wrong function pointer.
 * 
 * Hydra_Software_Devel/24   11/23/09 10:03a VISHK
 * SW7405-3210: Commented: BADS_31xx_GetStatus not returning values for
 * correctedCount, uncorrectedCount  and  berRawCount
 * 
 * Hydra_Software_Devel/23   11/17/09 6:36p VISHK
 * SW7405-3210: BADS_31xx_GetStatus not returning values for
 * correctedCount, uncorrectedCount and berRawCount
 * 
 * Hydra_Software_Devel/22   11/6/09 3:22p VISHK
 * SW7405-3210: BADS_31xx_GetStatus not returning values for
 * correctedCount, uncorrectedCount and berRawCount
 * 
 * Hydra_Software_Devel/21   11/5/09 1:49p VISHK
 * SW7405-3210: BADS_31xx_GetStatus not returning values for
 * correctedCount, uncorrectedCount and berRawCount
 * 
 * Hydra_Software_Devel/20   9/18/09 9:50a vishk
 * SW7405-2852, SW7405-3009: Removed debug messages.
 * 
 * Hydra_Software_Devel/19   9/14/09 2:30p vishk
 * SW7405-2852: 31xx: ADS add support configure of Parallel or Serial
 * Downstream interface.
 * 
 * SW7405-3009:Need to be able to shut down bcm3117 GPIO_00 to reduce
 * potential FCC concern.
 * 
 * Hydra_Software_Devel/18   9/2/09 5:28p vishk
 * SW7405-2839: Bcm31xx: Add Power Management support for ADS
 * 
 * Hydra_Software_Devel/17   8/25/09 11:21a vishk
 * SW7405-2839: Bcm31xx: Add Power Management support for ADS
 * 
 * Hydra_Software_Devel/16   8/21/09 6:22p vishk
 * PR 57482: Bcm31xx: Add Power Management support for ADS
 * 
 * Hydra_Software_Devel/15   8/20/09 11:58a vishk
 * PR 57482: Bcm31xx: Add Power Management support for ADS
 * 
 * Hydra_Software_Devel/14   8/17/09 4:24p vishk
 * PR 57767: add support for constellation for 31xx frontends.
 * 
 * Hydra_Software_Devel/13   7/22/09 3:58p vishk
 * PR 56899: 0907-0447 DCX700 - OOB AGC and SNR values are not implemented
 * in HAL 6.4.6.2
 * 
 * Hydra_Software_Devel/12   6/29/09 5:25p vishk
 * PR 56070: BCM31xx: Need to read dsChannelPower
 * 
 * Hydra_Software_Devel/11   6/16/09 4:37p vishk
 * PR 55960: Need to query spectrumInverted setting for BCM31xx
 * 
 * Hydra_Software_Devel/10   6/9/09 7:13p vishk
 * PR 55807: BCM3109 won't lock to AnnexA QAM16, QAM32 or QAM128
 * 
 * Hydra_Software_Devel/9   6/2/09 1:26p vishk
 * PR 53208: 3117 AP does not initialize on firs attemp consistently
 * 
 * Hydra_Software_Devel/8   6/1/09 3:31p vishk
 * PR 53208: 3117 AP does not initialize on firs attemp consistently
 * 
 * Hydra_Software_Devel/7   5/13/09 1:40p vishk
 * PR 46431: Add support for BCM3117, BCM3114, and BCM3112
 * 
 * Hydra_Software_Devel/6   5/12/09 2:24p vishk
 * PR 46431: Add support for BCM3117, BCM3114, and BCM3112
 * 
 * Hydra_Software_Devel/5   5/12/09 12:40p vishk
 * PR 46431: Add support for BCM3117, BCM3114, and BCM3112
 * 
 * Hydra_Software_Devel/4   5/7/09 2:42p vishk
 * PR 46431: Add support for BCM3117, BCM3114, and BCM3112
 * 
 * Hydra_Software_Devel/3   5/7/09 2:12p vishk
 * PR 46431: Add support for BCM3117, BCM3114, and BCM3112
 * 
 * Hydra_Software_Devel/2   4/17/09 5:00p vishk
 * PR 46431: Add support for BCM3117, BCM3114, and BCM3112
 * 
 * Hydra_Software_Devel/1   3/10/09 3:47p vishk
 * PR 46431: Add support for BCM3117, BCM3114, and BCM3112
 * 
 ***************************************************************************/
#include "bstd.h"
#include "bads.h"
#include "bhab.h"
#include "bads_priv.h"
#include "bads_31xx_priv.h"

BDBG_MODULE(bads_31xx_priv);

#define CHK_RETCODE( rc, func )             \
do {                                        \
    if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
    {                                       \
        goto done;                          \
    }                                       \
} while(0)

#define MX_ADS_CHANNELS         (1)
#define DEV_MAGIC_ID            ((BERR_ADS_ID<<16) | 0xFACE)

/*******************************************************************************
*
*   Private Module Handles
*
*******************************************************************************/
typedef struct BADS_P_31xx_Handle               *BADS_31xx_Handle;
typedef struct BADS_P_31xx_Handle
{
    uint32_t magicId;                   /* Used to check if structure is corrupt */
    BCHP_Handle hChip;
    BREG_Handle hRegister;
    BINT_Handle hInterrupt;
    BHAB_DevId devId;
    BHAB_Handle hHab;
    BADS_Version verInfo;
    unsigned int mxChnNo;
    BADS_ChannelHandle hAdsChn[MX_ADS_CHANNELS];
    bool isDaisyChain;
} BADS_P_31xx_Handle;

typedef struct BADS_P_31xx_ChannelHandle            *BADS_31xx_ChannelHandle;
typedef struct BADS_P_31xx_ChannelHandle
{
    uint32_t magicId;                   /* Used to check if structure is corrupt */
    BADS_Handle hAds;
    unsigned int chnNo;
    BHAB_DevId devId;
    BHAB_Handle hHab;
    BADS_CallbackFunc pCallback[BADS_Callback_eLast];
    void *pCallbackParam[BADS_Callback_eLast];
    BADS_ChannelSettings settings;
    bool isLock;                        /* current lock status */
    BKNI_MutexHandle mutex;             /* mutex to protect lock status*/
    BHAB_InterruptType event;
} BADS_P_31xx_ChannelHandle;

/*******************************************************************************
*
*   Private Module Functions
*
*******************************************************************************/
BERR_Code BADS_31xx_P_EventCallback_isr(
    void * pParam1, int param2
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_ChannelHandle hChn = (BADS_ChannelHandle) pParam1;
    BADS_31xx_ChannelHandle hImplChnDev;
    BHAB_InterruptType event = (BHAB_InterruptType) param2;

    BDBG_ENTER(BADS_31xx_ProcessNotification);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BADS_31xx_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab);

    switch (event) {
        case BHAB_Interrupt_eLockChange:
            {
                if( hImplChnDev->pCallback[BADS_Callback_eLockChange] != NULL )
                {
                    (hImplChnDev->pCallback[BADS_Callback_eLockChange])(hImplChnDev->pCallbackParam[BADS_Callback_eLockChange] );
                }
            }
            break;
        case BHAB_Interrupt_eUpdateGain:
            {
                if( hImplChnDev->pCallback[BADS_Callback_eUpdateGain] != NULL )
                {
                    (hImplChnDev->pCallback[BADS_Callback_eUpdateGain])(hImplChnDev->pCallbackParam[BADS_Callback_eUpdateGain] );
                }
            }
            break;
        case BHAB_Interrupt_eNoSignal:
            {
                if( hImplChnDev->pCallback[BADS_Callback_eNoSignal] != NULL )
                {
                    (hImplChnDev->pCallback[BADS_Callback_eNoSignal])(hImplChnDev->pCallbackParam[BADS_Callback_eNoSignal] );
                }
            }
            break;
        case BHAB_Interrupt_eQamAsyncStatusReady:
            {
				BDBG_WRN(("BHAB_Interrupt_eQamAsyncStatusReady received."));
            
                if( hImplChnDev->pCallback[BADS_Callback_eAsyncStatusReady] != NULL )
                {
					BDBG_WRN(("BADS_Callback_eAsyncStatusReady callback called."));
                    (hImplChnDev->pCallback[BADS_Callback_eAsyncStatusReady])(hImplChnDev->pCallbackParam[BADS_Callback_eAsyncStatusReady] );
                }
            }
            break;
        default:
            BDBG_WRN((" unknown event code from 31xx"));
            break;
    }

    BDBG_LEAVE(BADS_31xx_P_EventCallback_isr);
    return( retCode );
}

/*******************************************************************************
*
*   Public Module Functions
*
*******************************************************************************/
BERR_Code BADS_31xx_Open(
    BADS_Handle *pAds,                          /* [out] Returns handle */
    BCHP_Handle hChip,                          /* [in] Chip handle */
    BREG_Handle hRegister,                      /* [in] Register handle */
    BINT_Handle hInterrupt,                     /* [in] Interrupt handle */
    const struct BADS_Settings *pDefSettings    /* [in] Default settings */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_Handle hDev;
    unsigned int chnIdx;
    BADS_31xx_Handle hImplDev = NULL;
    uint8_t   hab[8];

    BDBG_ENTER(BADS_31xx_Open);

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
    hDev->settings = *pDefSettings;

    hImplDev = (BADS_31xx_Handle) BKNI_Malloc( sizeof( BADS_P_31xx_Handle ) );
    if( hImplDev == NULL )
    {
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BADS_Open: BKNI_malloc() failed, impl"));
        goto done;
    }

    BKNI_Memset( hImplDev, 0x00, sizeof( BADS_P_31xx_Handle ) );
    hImplDev->magicId = DEV_MAGIC_ID;
    hImplDev->hChip = hChip;
    hImplDev->hRegister = hRegister;
    hImplDev->hInterrupt = hInterrupt;
    hImplDev->hHab = (BHAB_Handle) pDefSettings->hGeneric;    /* For this device, we need the HAB handle */
    hImplDev->devId = BHAB_DevId_eADS0; /* Here the device id is always defaulted to channel 0. */
    hImplDev->mxChnNo = MX_ADS_CHANNELS;
   
    hab[0] = 0x25;
    hab[1] = 0x00;
    CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplDev->hHab, hab, 1, hab, 2, true, true, 2));
    hImplDev->isDaisyChain = hab[1] & 0x2;

    hab[0] = 0x12;
    hab[1] = 0;
    hab[2] = 0x1a;
    hab[3] = 0x04;
    hab[4] = 0x00;
    CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplDev->hHab, hab, 4, hab, 8, true, true, 8));

    hab[0] = 0x0b;
    hab[1] = 0;
    if(pDefSettings->isOpenDrain ) {
        hab[1] = 0x2;
    }
        
    if(pDefSettings->transportConfig < BADS_TransportData_eMax) {
        hab[1] |= (pDefSettings->transportConfig << 2);
    }
    else {
        BDBG_ERR(("ERROR"));
    }
    hab[1] |= (hab[4] & 0x1);
    
    hab[2] = 0x00;
    hab[3] = 0x00;
    hab[4] = 0x00;  
    BHAB_CHK_RETCODE(BHAB_SendHabCommand(hImplDev->hHab, hab, 5, hab, 1, true, true, 5));
    
    for( chnIdx = 0; chnIdx < MX_ADS_CHANNELS; chnIdx++ )
    {
        hImplDev->hAdsChn[chnIdx] = NULL;
    }
    hDev->pImpl = hImplDev;
    *pAds = hDev;

done:
    if( retCode != BERR_SUCCESS )
    {
        if( hDev != NULL )
        {
            BKNI_Free( hDev );
        }
        if( hImplDev != NULL )
        {
            BKNI_Free( hImplDev );
        }       
        *pAds = NULL;
    }
    BDBG_LEAVE(BADS_31xx_Open);
    return( retCode );
}

BERR_Code BADS_31xx_Close(
    BADS_Handle hDev                    /* [in] Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BADS_31xx_Close);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    BKNI_Free( (void *) hDev->pImpl );
    hDev->magicId = 0x00;       /* clear it to catch improper use */
    BKNI_Free( (void *) hDev );

    BDBG_LEAVE(BADS_31xx_Close);
    return( retCode );
}

BERR_Code BADS_31xx_Init(
    BADS_Handle hDev                    /* [in] Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_31xx_Handle hImplDev;
    uint32_t       familyId, chipId;
    uint16_t       chipVer;
    uint8_t        majApVer, minApVer;
    
    BDBG_ENTER(BADS_31xx_Init);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    hImplDev = (BADS_31xx_Handle) hDev->pImpl;
    BDBG_ASSERT( hImplDev );
    BDBG_ASSERT( hImplDev->hHab );

    retCode = BHAB_GetApVersion(hImplDev->hHab, &familyId, &chipId, &chipVer, &majApVer, &minApVer);
    hImplDev->verInfo.familyId = familyId;
    hImplDev->verInfo.chipId = chipId;
    hImplDev->verInfo.apVer = majApVer;
    hImplDev->verInfo.minApVer = minApVer;
    hImplDev->verInfo.majVer = chipVer >> 8;
    hImplDev->verInfo.minVer = chipVer & 0xFF;

    BDBG_LEAVE(BADS_31xx_Init);
    return( retCode );
}

BERR_Code BADS_31xx_GetVersion(
    BADS_Handle hDev,                   /* [in] Device handle */
    BADS_Version *pVersion              /* [out] Returns version */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_31xx_Handle hImplDev;

    BDBG_ENTER(BADS_31xx_GetVersion);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    hImplDev = (BADS_31xx_Handle) hDev->pImpl;
    BDBG_ASSERT( hImplDev );

    *pVersion = hImplDev->verInfo;      /* use saved data */

    BDBG_LEAVE(BADS_31xx_GetVersion);
    return( retCode );
}

BERR_Code BADS_31xx_GetTotalChannels(
    BADS_Handle hDev,                   /* [in] Device handle */
    unsigned int *totalChannels         /* [out] Returns total number downstream channels supported */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_31xx_Handle hImplDev;

    BDBG_ENTER(BADS_31xx_GetTotalChannels);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    hImplDev = (BADS_31xx_Handle) hDev->pImpl;
    BDBG_ASSERT( hImplDev );

    *totalChannels = hImplDev->mxChnNo; /* use saved data */

    BDBG_LEAVE(BADS_31xx_GetTotalChannels);
    return( retCode );
}

BERR_Code BADS_31xx_GetChannelDefaultSettings(
    BADS_Handle hDev,                       /* [in] Device handle */
    unsigned int channelNo,                 /* [in] Channel number to default setting for */
    BADS_ChannelSettings *pChnDefSettings   /* [out] Returns channel default setting */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_31xx_Handle hImplDev;

    BDBG_ENTER(BADS_31xx_GetChannelDefaultSettings);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    hImplDev = (BADS_31xx_Handle) hDev->pImpl;
    BDBG_ASSERT( hImplDev );
    BDBG_ASSERT( hImplDev->hHab );

    switch( channelNo )
    {
        case 0:
        if( channelNo < hImplDev->mxChnNo )
        {
            pChnDefSettings->ifFreq = 0;    /*The if freq for 31xx is set to 0 as the internal tuner does not spit out seperate if frequency. */
            pChnDefSettings->fastAcquire = false;
            break;
        }
        default:
            retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
            break;
    }

    BDBG_LEAVE(BADS_31xx_GetChannelDefaultSettings);
    return( retCode );
}

BERR_Code BADS_31xx_OpenChannel(
    BADS_Handle hDev,                                   /* [in] Device handle */
    BADS_ChannelHandle *phChn,                          /* [out] Returns channel handle */
    unsigned int channelNo,                             /* [in] Channel number to open */
    const struct BADS_ChannelSettings *pChnDefSettings  /* [in] Channel default setting */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_31xx_Handle hImplDev;
    BADS_31xx_ChannelHandle hImplChnDev = NULL;
    BADS_ChannelHandle hChnDev;
    unsigned int event=0;

    BDBG_ENTER(BADS_31xx_OpenChannel);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    hImplDev = (BADS_31xx_Handle) hDev->pImpl;
    BDBG_ASSERT( hImplDev );
    BDBG_ASSERT( hImplDev->hHab);
    
    hChnDev = NULL;
    if( channelNo < hImplDev->mxChnNo )
    {
        if( hImplDev->hAdsChn[channelNo] == NULL )
        {
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

            hImplChnDev = (BADS_31xx_ChannelHandle) BKNI_Malloc( sizeof( BADS_P_31xx_ChannelHandle ) );
            if( hImplChnDev == NULL )
            {
                retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
                BDBG_ERR(("BADS_OpenChannel: BKNI_malloc() failed, impl"));
                goto done;
            }
            
            hImplChnDev->chnNo = channelNo;
            hImplChnDev->devId = (BHAB_DevId) channelNo;

            BHAB_InstallInterruptCallback( hImplDev->hHab,  hImplChnDev->devId, BADS_31xx_P_EventCallback_isr , (void *)hChnDev, event);
            
            if (pChnDefSettings) hImplChnDev->settings = *pChnDefSettings;
            hImplChnDev->hHab = hImplDev->hHab;
            CHK_RETCODE(retCode, BKNI_CreateMutex(&hImplChnDev->mutex));
            hImplDev->hAdsChn[channelNo] = hChnDev;
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
        if( hChnDev != NULL )
        {
            BKNI_Free( hChnDev );
            hImplDev->hAdsChn[channelNo] = NULL;
        }
        if( hImplChnDev != NULL )
        {
            BKNI_Free( hImplChnDev );
        }
        *phChn = NULL;
    }
    BDBG_LEAVE(BADS_31xx_OpenChannel);
    return( retCode );
}

BERR_Code BADS_31xx_CloseChannel(
    BADS_ChannelHandle hChn             /* [in] Device channel handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_31xx_Handle hImplDev;
    BADS_31xx_ChannelHandle hImplChnDev;
    BADS_Handle hAds;
    unsigned int chnNo;
    
    BDBG_ENTER(BADS_31xx_CloseChannel);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BADS_31xx_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );

    hAds = hChn->hAds;
    hImplDev = (BADS_31xx_Handle) hAds->pImpl;
    BDBG_ASSERT( hImplDev );
    
    BHAB_UnInstallInterruptCallback(hImplChnDev->hHab, hImplChnDev->devId);

    BKNI_DestroyMutex(hImplChnDev->mutex);
    chnNo = hImplChnDev->chnNo;
    hChn->magicId = 0x00;       /* clear it to catch inproper use */
    BKNI_Free( hChn->pImpl );
    BKNI_Free( hChn );
    hImplDev->hAdsChn[chnNo] = NULL;

    BDBG_LEAVE(BADS_31xx_CloseChannel);
    return( retCode );
}

BERR_Code BADS_31xx_GetDevice(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_Handle *phDev                  /* [out] Returns Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BADS_31xx_GetDevice);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    *phDev = hChn->hAds;

    BDBG_LEAVE(BADS_31xx_GetDevice);
    return( retCode );
}

BERR_Code BADS_31xx_Acquire(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_InbandParam *ibParam           /* [in] Inband Parameters to use */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_31xx_ChannelHandle hImplChnDev;
    uint8_t hab[4], hab2[8];

    BDBG_ENTER(BADS_31xx_Acquire);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BADS_31xx_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );

    BKNI_AcquireMutex(hImplChnDev->mutex);
    hImplChnDev->isLock = false;
    BKNI_ReleaseMutex(hImplChnDev->mutex);

    BDBG_MSG(("%s: modType=%d, symbolRate=%d", __FUNCTION__, ibParam->modType, ibParam->symbolRate));

    hab[0] = 0x22;
    hab[1] = 0x04;
    CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, hab, 2, hab, 1, true, true, 2));

    BHAB_CHK_RETCODE(BHAB_EnableLockInterrupt(hImplChnDev->hHab, hImplChnDev->devId, false));

    hab[0] = 0xa;
    hab[1] = 0;
    hab[1] = ((ibParam->autoAcquire) ? 0x30 : 0x10);
    hab[1] |= ((ibParam->invertSpectrum) ? 0x08 : 0x00);
    hab[1] |= ((ibParam->spectrum) ? 0x10 : 0x00);
    hab[2] = 0x0;
    hab[3] = 0x0;
    
    if (ibParam->modType < BADS_ModulationType_eAnnexAQam512) {
        hab[1] |= ((ibParam->enableNullPackets) ? 0x40 : 0x00);
        switch ( ibParam->frequencyOffset )
        {
        case BADS_AutomaticFrequencyOffset_eAnnexAQam200:
            hab[2] |= 0x0;
            break;
        case BADS_AutomaticFrequencyOffset_eAnnexAQam250:
            hab[2] |= 0x1;
            break;
        case BADS_AutomaticFrequencyOffset_eAnnexAQam125:
             hab[2] |= 0x2;
            break;        
        default:
             hab[2] |= 0x0;
             BDBG_WRN(("Defaulting the frequency offset to 0x%x", BADS_AutomaticFrequencyOffset_eAnnexAQam200));
        }
        
        hab[2] |= (ibParam->modType << 4) + 0x10;

        hab2[0] = 0x13;
        hab2[1] = 0x00;
        hab2[2] = 0x37;
        hab2[3] = 0x04;
        hab2[4] = ibParam->symbolRate >> 24;
        hab2[5] = ibParam->symbolRate >> 16;
        hab2[6] = ibParam->symbolRate >> 8;
        hab2[7] = ibParam->symbolRate;
        CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, hab2, 8, hab2, 1, true, true, 8 ));
    }
    else {
        switch ( ibParam->modType )
        {
        case BADS_ModulationType_eAnnexBQam64:
            hab[2] |= 0x38;
            break;
        case BADS_ModulationType_eAnnexBQam256:
             hab[2] |= 0x58;
            break;
        case BADS_ModulationType_eAnnexBQam1024:
            hab[2] |= 0x78;
            break;        
        default:
            retCode = BERR_INVALID_PARAMETER;
            goto done;
        }
        switch ( ibParam->frequencyOffset )
        {
        case BADS_AutomaticFrequencyOffset_eAnnexBQam180:
            hab[2] |= 0x0;
            break;
        case BADS_AutomaticFrequencyOffset_eAnnexBQam250:
            hab[2] |= 0x1;
            break;
        case BADS_AutomaticFrequencyOffset_eAnnexBQam125:
             hab[2] |= 0x2;
            break;
        default:
             hab[2] |= 0x0;
             BDBG_WRN(("Defaulting the frequency offset to 0x%x", BADS_AutomaticFrequencyOffset_eAnnexBQam180));
        }
    }
    
    if(ibParam->enableDpm) {
        hab[2] |= 0x80;
    }
    
    CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, hab, 4, hab, 1, true, true, 4 ));
    CHK_RETCODE(retCode, BHAB_EnableLockInterrupt(hImplChnDev->hHab, hImplChnDev->devId, true));

done:
    BDBG_LEAVE(BADS_31xx_Acquire);
    return( retCode );
}

BERR_Code BADS_31xx_GetStatus(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_Status *pStatus                /* [out] Returns status */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_31xx_ChannelHandle hImplChnDev;
    uint8_t   buf[65];
    uint8_t val = 0;
    
    BDBG_ENTER(BADS_31xx_GetStatus);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BADS_31xx_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );
    
    buf[0] = 0x15;
    CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 1, buf, 0x3D, true, true, 0x3D));
    
    pStatus->isPowerSaverEnabled = true;

    val = (buf[2] & 0x70)>>4;
    
    if(buf[2] & 0x8) {
        if( val == 3) pStatus->modType = BADS_ModulationType_eAnnexBQam64;
        else if( val == 5) pStatus->modType = BADS_ModulationType_eAnnexBQam256;
        else if( val == 7) pStatus->modType = BADS_ModulationType_eAnnexBQam1024;
        else {
            retCode = BERR_NOT_SUPPORTED;
            goto done;
        }       
    }
    else {
        if ((val > 0) && (val < 6)) {
            pStatus->modType = (BADS_ModulationType) (val - 1);
        }
        else {
            retCode = BERR_NOT_SUPPORTED;
            goto done;
        }
    }

    pStatus->ifFreq = 0; /* Not supported */
    pStatus->symbolRate = (int32_t)((buf[0x04] << 24) + (buf[0x05] << 16) + (buf[0x06] << 8) + buf[0x07]);
    pStatus->isFecLock = buf[8] & 0x4;
    pStatus->isQamLock = buf[8] & 0x8;
    pStatus->correctedCount = 0; /* Not supported */
    pStatus->uncorrectedCount = 0;/* Not supported */
    pStatus->snrEstimate = (buf[0x9] << 8) + buf[0xa];
    pStatus->agcIntLevel = (((buf[0x13] << 8) + buf[0x14]) * 1000) / 65535;
    pStatus->agcExtLevel =  0;/* Not supported */
    pStatus->carrierFreqOffset = (int32_t)((buf[0xb] << 24) + (buf[0xc] << 16) + (buf[0xd] << 8) + buf[0xe]);
    pStatus->carrierPhaseOffset = (int32_t)((buf[0x33] << 24) + (buf[0x34] << 16) + (buf[0x35] << 8) + buf[0x36]);
    pStatus->rxSymbolRate = (int32_t)((buf[0x4] << 24) + (buf[0x5] << 16) + (buf[0x6] << 8) + buf[0x7]);
    pStatus->interleaveDepth = 0; /* Not supported */
    pStatus->goodRsBlockCount = 0; /* Not supported */
    pStatus->berRawCount = (int32_t)((buf[0x17] << 24) + (buf[0x18] << 16) + (buf[0x19] << 8) + buf[0x1a]); 
    pStatus->correctedBits = (int32_t)((buf[0x1b] << 24) + (buf[0x1c] << 16) + (buf[0x1d] << 8) + buf[0x1e]);
    pStatus->dsChannelPower = ((int32_t)(((int8_t)buf[0x3b] << 8) + buf[0x3c]))/10;
    pStatus->isSpectrumInverted = buf[8] & 0x1;
    pStatus->mainTap = 0; /* Not supported */
    pStatus->equalizerGain = 0; /* Not supported */
    pStatus->postRsBER = 0; /* Not supported */
    pStatus->elapsedTimeSec = 0; /* Not supported */
    pStatus->accCorrectedCount = (int32_t)((buf[0x1f] << 24) + (buf[0x20] << 16) + (buf[0x21] << 8) + buf[0x22]);
    pStatus->accUncorrectedCount = (int32_t)((buf[0x23] << 24) + (buf[0x24] << 16) + (buf[0x25] << 8) + buf[0x26]);
    pStatus->cleanCount = (int32_t)((buf[0x27] << 24) + (buf[0x28] << 16) + (buf[0x29] << 8) + buf[0x2a]);

done:
    BDBG_LEAVE(BADS_31xx_GetStatus);
    return( retCode );
}

BERR_Code BADS_31xx_RequestAsyncStatus(
    BADS_ChannelHandle hChn            /* [in] Device channel handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_31xx_ChannelHandle hImplChnDev;
    uint8_t   buf[1];
    
    BDBG_ENTER(BADS_31xx_RequestAsyncStatus);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BADS_31xx_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );
    
    buf[0] = 0x26;
    CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 1, buf, 0x1, true, true, 0x1));
    
done:
    BDBG_LEAVE(BADS_31xx_RequestAsyncStatus);
    return( retCode );
}

BERR_Code BADS_31xx_GetAsyncStatus(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_Status *pStatus                /* [out] Returns status */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_31xx_ChannelHandle hImplChnDev;
    uint8_t   buf[65];
    uint8_t val = 0;
    
    BDBG_ENTER(BADS_31xx_GetAsyncStatus);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BADS_31xx_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );
    
    buf[0] = 0x27;
    CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 1, buf, 0x3D, true, true, 0x3D));
    
    pStatus->isPowerSaverEnabled = true;

    val = (buf[2] & 0x70)>>4;
    
    if(buf[2] & 0x8) {
        if( val == 3) pStatus->modType = BADS_ModulationType_eAnnexBQam64;
        else if( val == 5) pStatus->modType = BADS_ModulationType_eAnnexBQam256;
        else if( val == 7) pStatus->modType = BADS_ModulationType_eAnnexBQam1024;
        else {
            retCode = BERR_NOT_SUPPORTED;
            goto done;
        }       
    }
    else {
        if ((val > 0) && (val < 6)) {
            pStatus->modType = (BADS_ModulationType) (val - 1);
        }
        else {
            retCode = BERR_NOT_SUPPORTED;
            goto done;
        }
    }

    

    pStatus->ifFreq = 0; /* Not supported */
    pStatus->symbolRate = (int32_t)((buf[0x04] << 24) + (buf[0x05] << 16) + (buf[0x06] << 8) + buf[0x07]);
    pStatus->isFecLock = buf[8] & 0x4;
    pStatus->isQamLock = buf[8] & 0x8;
    pStatus->correctedCount = (int32_t)((buf[0x1f] << 24) + (buf[0x20] << 16) + (buf[0x21] << 8) + buf[0x22]);
    pStatus->uncorrectedCount = (int32_t)((buf[0x23] << 24) + (buf[0x24] << 16) + (buf[0x25] << 8) + buf[0x26]);
    pStatus->snrEstimate = (buf[0x9] << 8) + buf[0xa];
    pStatus->agcIntLevel = (((buf[0x13] << 8) + buf[0x14]) * 1000) / 65535;
    pStatus->agcExtLevel =  0;/* Not supported */
    pStatus->carrierFreqOffset = (int32_t)((buf[0xb] << 24) + (buf[0xc] << 16) + (buf[0xd] << 8) + buf[0xe]);
    pStatus->carrierPhaseOffset = (int32_t)((buf[0x33] << 24) + (buf[0x34] << 16) + (buf[0x35] << 8) + buf[0x36]);
    pStatus->rxSymbolRate = (int32_t)((buf[0x4] << 24) + (buf[0x5] << 16) + (buf[0x6] << 8) + buf[0x7]);
    pStatus->interleaveDepth = 0; /* Not supported */
    pStatus->goodRsBlockCount = (int32_t)((buf[0x27] << 24) + (buf[0x28] << 16) + (buf[0x29] << 8) + buf[0x2a]);
    pStatus->berRawCount = (int32_t)((buf[0x17] << 24) + (buf[0x18] << 16) + (buf[0x19] << 8) + buf[0x1a]);
    pStatus->dsChannelPower = ((int32_t)(((int8_t)buf[0x3b] << 8) + buf[0x3c]))/10;
    pStatus->isSpectrumInverted = buf[8] & 0x1;
    pStatus->mainTap = 0; /* Not supported */
    pStatus->equalizerGain = 0; /* Not supported */
    pStatus->postRsBER = 0; /* Not supported */
    pStatus->elapsedTimeSec = 0; /* Not supported */

done:
    BDBG_LEAVE(BADS_31xx_GetAsyncStatus);
    return( retCode );
}

BERR_Code BADS_31xx_GetLockStatus(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_LockStatus *pLockStatus         /* [out] Returns lock status */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_31xx_ChannelHandle hImplChnDev;
    uint8_t   buf[65];
    bool lock=false;

    BDBG_ENTER(BADS_31xx_GetLockStatus);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BADS_31xx_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab);

    buf[0] = 0x15;
    CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, buf, 1, buf, 0x3D, true, true, 0x3D));

    lock = buf[8] & 0x4;
    if(lock) 
        *pLockStatus = BADS_LockStatus_eLocked;
    else
        *pLockStatus = BADS_LockStatus_eUnlocked;
done:
    BDBG_LEAVE(BADS_31xx_GetLockStatus);
    return( retCode );

}

BERR_Code BADS_31xx_GetSoftDecision(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    int16_t nbrToGet,                   /* [in] Number values to get */
    int16_t *iVal,                      /* [out] Ptr to array to store output I soft decision */
    int16_t *qVal,                      /* [out] Ptr to array to store output Q soft decision */
    int16_t *nbrGotten                  /* [out] Number of values gotten/read */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_31xx_ChannelHandle hImplChnDev;
    uint8_t hab[121], i;

    BDBG_ENTER(BADS_31xx_GetSoftDecision);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BADS_31xx_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );

    hab[0] = 0x23;

    CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, hab, 1, hab, 0x79, true, true, 0x79));
    
	for (i = 0; i < 30 && i < nbrToGet; i++)
    {
        iVal[i] = (hab[1+(4*i)] << 4) | ((hab[2+(4*i)] >> 4) & 0x0F);
        qVal[i] = (hab[3+(4*i)] << 4) | ((hab[4+(4*i)] >> 4) & 0x0F);
      
        if (iVal[i] & 0x800)
            iVal[i] -= 0x1000;
        if (qVal[i] & 0x800)
            qVal[i] -= 0x1000;
    }

	*nbrGotten = i;
done:   


    BDBG_LEAVE(BADS_31xx_GetSoftDecision);
    return( retCode );
}


BERR_Code BADS_31xx_EnablePowerSaver(
    BADS_ChannelHandle hChn             /* [in] Device channel handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_31xx_ChannelHandle hImplChnDev;
    uint8_t   hab[2];

    BDBG_ENTER(BADS_31xx_EnablePowerSaver);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BADS_31xx_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );

    hab[0] = 0x21;
    hab[1] = 0x04;
    CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, hab, 2, hab, 1, true, true, 2));
done:
    BDBG_LEAVE(BADS_31xx_EnablePowerSaver);
    return( retCode );
}

BERR_Code BADS_31xx_DisablePowerSaver(
    BADS_ChannelHandle hChn             /* [in] Device channel handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_31xx_ChannelHandle hImplChnDev;
    uint8_t   hab[2];

    BDBG_ENTER(BADS_31xx_DisablePowerSaver);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BADS_31xx_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );

    hab[0] = 0x22;
    hab[1] = 0x04;
    CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, hab, 2, hab, 1, true, true, 2));
done:
    BDBG_LEAVE(BADS_31xx_DisablePowerSaver);
    return( retCode );
}

BERR_Code BADS_31xx_ProcessNotification(
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

BERR_Code BADS_31xx_InstallCallback(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_Callback callbackType,         /* [in] Type of callback */
    BADS_CallbackFunc pCallback,        /* [in] Function Ptr to callback */
    void *pParam                        /* [in] Generic parameter send on callback */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_31xx_ChannelHandle hImplChnDev;

    BDBG_ENTER(BADS_31xx_InstallCallback);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BADS_31xx_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );

    switch( callbackType )
    {
        case BADS_Callback_eLockChange:
            hImplChnDev->pCallback[callbackType] = pCallback;
            hImplChnDev->pCallbackParam[callbackType] = pParam;
            break;
        case BADS_Callback_eUpdateGain:
            hImplChnDev->pCallback[callbackType] = pCallback;
            hImplChnDev->pCallbackParam[callbackType] = pParam;
            break;
        case BADS_Callback_eNoSignal:
            hImplChnDev->pCallback[callbackType] = pCallback;
            hImplChnDev->pCallbackParam[callbackType] = pParam;
            break;
        case BADS_Callback_eAsyncStatusReady:
            hImplChnDev->pCallback[callbackType] = pCallback;
            hImplChnDev->pCallbackParam[callbackType] = pParam;
            break;
        default:
            retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
            break;
    }

    BDBG_LEAVE(BADS_31xx_InstallCallback);
    return( retCode );
}

BERR_Code BADS_31xx_SetDaisyChain(
    BADS_Handle hDev,       /* [in] Returns handle */
    bool enableDaisyChain   /* [in] Eanble/disable daisy chain. */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_31xx_Handle hImplDev;
    uint8_t   hab[2];

    BDBG_ENTER(BADS_31xx_SetDaisyChain);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    hImplDev = (BADS_31xx_Handle) hDev->pImpl;  
    BDBG_ASSERT( hImplDev );
    BDBG_ASSERT( hImplDev->hHab );
    
    if(enableDaisyChain) {
        hab[0] = 0x22;
    }
    else {
        hab[0] = 0x21;
    }
    hab[1] = 0x02;
    CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplDev->hHab, hab, 2, hab, 1, true, true, 2));

    hImplDev->isDaisyChain = enableDaisyChain;

done:
    BDBG_LEAVE(BADS_31xx_SetDaisyChain);
    return( retCode );
}

BERR_Code BADS_31xx_GetDaisyChain(
    BADS_Handle hDev,           /* [in] Returns handle */
    bool *isEnableDaisyChain    /* [in] Eanble/disable daisy chain. */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_31xx_Handle hImplDev;
    
    BDBG_ENTER(BADS_31xx_GetDaisyChain);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    hImplDev = (BADS_31xx_Handle) hDev->pImpl;  
    BDBG_ASSERT( hImplDev );
    BDBG_ASSERT( hImplDev->hHab );
    
    *isEnableDaisyChain = hImplDev->isDaisyChain;

    BDBG_LEAVE(BADS_31xx_GetDaisyChain);
    return( retCode );
}

BERR_Code BADS_31xx_ResetStatus(
    BADS_ChannelHandle hChn     /* [in] Device channel handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_31xx_ChannelHandle hImplChnDev;
    uint8_t   hab[1];
    
    BDBG_ENTER(BADS_31xx_ResetStatus);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BADS_31xx_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );
    
    hab[0] = 0xD;
    CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, hab, 1, hab, 1, true, true, 1));

done:
    BDBG_LEAVE(BADS_31xx_ResetStatus);
    return( retCode );
}

BERR_Code BADS_31xx_WriteSlave(
    BADS_ChannelHandle hChn,     /* [in] Device channel handle */
    uint8_t chipAddr,            /* [in] chip addr of the i2c slave device */
    uint32_t subAddr,            /* [in] sub addr of the register to read from the slave device */
    uint8_t subAddrLen,          /* [in] how many bytes is the sub addr? one to four*/
    uint32_t *data,              /* [in] ptr to the data that we will write to the slave device */
    uint8_t dataLen              /* [in] how many bytes are we going to write? one to four*/
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_31xx_ChannelHandle hImplChnDev;
    uint8_t   hab[12];
    
    BDBG_ENTER(BADS_31xx_WriteSlave);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BADS_31xx_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );
    
    hab[0] = 0x2A;
    hab[1] = chipAddr; /* slave addr */
    hab[2] = subAddrLen;    /* sub addr len */
    switch (subAddrLen)
    {
        case 4:
            hab[6] = (subAddr >> 24) & 0xff;
        case 3:
            hab[5] = (subAddr >> 16) & 0xff;
        case 2:
            hab[4] = (subAddr >> 8) & 0xff;
        case 1:
            hab[3] = (subAddr >> 0) & 0xff;
    }

    hab[7] = dataLen;
    switch (dataLen)
    {
        case 4:
            hab[11] = (*data >> 24) & 0xff;
        case 3:
            hab[10] = (*data >> 16) & 0xff;
        case 2:
            hab[9] = (*data >> 8) & 0xff;
        case 1:
            hab[8] = (*data >> 0) & 0xff;
    }

    CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, hab, 0xc, hab, 1, true, true, 0xc));

done:
    BDBG_LEAVE(BADS_31xx_ResetStatus);
    return( retCode );
}

BERR_Code BADS_31xx_ReadSlave(
    BADS_ChannelHandle hChn,     /* [in] Device channel handle */
    uint8_t chipAddr,            /* [in] chip addr of the i2c slave device */
    uint32_t subAddr,            /* [in] sub addr of the register to read from the slave device */
    uint8_t subAddrLen,          /* [in] how many bytes is the sub addr? one to four*/
    uint32_t *data,              /* [out] ptr to the data that we will read from the slave device */
    uint8_t dataLen              /* [in] how many bytes are we going to read? one to four*/
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BADS_31xx_ChannelHandle hImplChnDev;
    uint8_t   hab[12];
    
    BDBG_ENTER(BADS_31xx_ReadSlave);
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hImplChnDev = (BADS_31xx_ChannelHandle) hChn->pImpl;
    BDBG_ASSERT( hImplChnDev );
    BDBG_ASSERT( hImplChnDev->hHab );
    
    hab[0] = 0x2B;
    hab[1] = chipAddr; /* slave addr */
    hab[2] = subAddrLen;    /* sub addr len */
    switch (subAddrLen)
    {
        case 4:
            hab[6] = (subAddr >> 24) & 0xff;
        case 3:
            hab[5] = (subAddr >> 16) & 0xff;
        case 2:
            hab[4] = (subAddr >> 8) & 0xff;
        case 1:
            hab[3] = (subAddr >> 0) & 0xff;
    }

    hab[7] = dataLen;
    CHK_RETCODE(retCode, BHAB_SendHabCommand(hImplChnDev->hHab, hab, 8, hab, 0xc, true, true, 0xc));

    *data = 0;
    switch (dataLen)
    {
        case 4:
            *data |= (hab[11] << 24);
        case 3:
            *data |= (hab[10] << 16);
        case 2:
            *data |= (hab[9] << 8);
        case 1:
            *data |= (hab[8] << 0);
    }

done:
    BDBG_LEAVE(BADS_31xx_ResetStatus);
    return( retCode );
}
