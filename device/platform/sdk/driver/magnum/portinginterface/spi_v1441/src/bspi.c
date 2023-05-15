/***************************************************************************
 *     Copyright (c) 2003-2014, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
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
 ***************************************************************************/
#include "bstd.h"

#include "breg_spi_priv.h"
#include "bspi.h"
#include "bspi_priv.h"

#if (HAS_UPG_MSPI==1)
#include "bchp_mspi.h"
#endif

#if (BCHP_CHIP!=4550) && (BCHP_CHIP!=4528) && (BCHP_CHIP!=4538) && (BCHP_CHIP!=4552)
#include "bchp_irq0.h"
#endif
#include "bchp_int_id_irq0.h"
#if (BCHP_CHIP==3548) || (BCHP_CHIP==3556) || (BCHP_CHIP==4550) || (BCHP_CHIP==4528) || (BCHP_CHIP==4538) || (BCHP_CHIP==4552) || HAS_HIF_MSPI
#include "bchp_bspi.h"
#endif
#if (BCHP_CHIP==7422) || (BCHP_CHIP==7425) || (BCHP_CHIP==7429) || (BCHP_CHIP==7344) || (BCHP_CHIP==7364) || (BCHP_CHIP==7250) || \
    (BCHP_CHIP==7346) || (BCHP_CHIP==7231) || (BCHP_CHIP==7358) || (BCHP_CHIP==7366) || (BCHP_CHIP==7552) || \
    (BCHP_CHIP==7435) || (BCHP_CHIP==7360) || (BCHP_CHIP==7584) || (BCHP_CHIP==7563) || (BCHP_CHIP==7445) || \
    (BCHP_CHIP==7145) || (BCHP_CHIP==7362) || (BCHP_CHIP==7228) || (BCHP_CHIP == 7439) || (BCHP_CHIP == 74371)
#include "bchp_int_id_irq0_aon.h"
#endif
#if HAS_HIF_MSPI
#include "bchp_hif_mspi.h"
#include "bchp_int_id_hif_spi_intr2.h"
#endif
#if (BCHP_CHIP!=4550) && (BCHP_CHIP!=4528) && (BCHP_CHIP!=4538) && (BCHP_CHIP!=4552)
#include "bchp_ebi.h"
#endif
#include "bkni_multi.h"

BDBG_MODULE(bspi);

#define DEV_MAGIC_ID            ((BERR_SPI_ID<<16) | 0xFACE)

#define BSPI_CHK_RETCODE( rc, func )        \
do {                                        \
    if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
    {                                       \
        goto done;                          \
    }                                       \
} while(0)

#define MAX_SPI_XFER        16          /* maximum number of transfers allowed per transaction */

#define SPI_SYSTEM_CLK      27000000    /* 27 MHz */
#if defined(BCHP_MSPI_SPCR3)
#define MAX_SPI_BAUD        13500000     /* SPBR = 1,  CLK=27MHZ */
#else
#define MAX_SPI_BAUD        1687500     /* SPBR = 8, CLK=27MHZ */
#endif
#define MIN_SPI_BAUD        52734       /* SPBR = 0, 27MHZ */

/* Format of the CDRAM, for some reason, this does not show up in RDB file */
#define SPI_CDRAM_CONT              0x80
#define SPI_CDRAM_BITSE             0x40
#define SPI_CDRAM_DT                0x20
#define SPI_CDRAM_DSCK              0x10
#define SPI_CDRAM_PCS_MASK          0x07

#define SPI_CDRAM_PCS_PCS0          0x01
#define SPI_CDRAM_PCS_PCS1          0x02
#if MAX_SPI_CHANNELS > 2
#define SPI_CDRAM_PCS_PCS2          0x04
#else
#define SPI_CDRAM_PCS_PCS2          0x00
#endif

#define SPI_CDRAM_PCS_DISABLE_ALL   (SPI_CDRAM_PCS_PCS0 | SPI_CDRAM_PCS_PCS1 | SPI_CDRAM_PCS_PCS2)

#define SPI_POLLING_INTERVAL          10      /* in usecs */
#define SPI_MUTEX_POLLING_INTERVAL    5       /* in millisecs */
#define SPI_MUTEX_POLLING_ITERATIONS  48000   /* So, 5ms*480000 = 240,000 msecs = 4 minutes. */

static void BSPI_P_EnableInt_isr ( BSPI_ChannelHandle  hChn );
static void BSPI_P_DisableInt_isr( BSPI_ChannelHandle  hChn );

/*******************************************************************************
*
*   Private Module Handles
*
*******************************************************************************/

typedef struct BSPI_P_Handle
{
    uint32_t        magicId;                    /* Used to check if structure is corrupt */
    BCHP_Handle     hChip;
    BREG_Handle     hRegister;
    BINT_Handle     hInterrupt;
    unsigned int    maxChnNo;
    BSPI_ChannelHandle hSpiChn[MAX_SPI_CHANNELS + HIF_MSPI_MAX_CHANNELS];
    BKNI_MutexHandle hUpgMutex;                    /* UPG spi mutex handle for serialization */
    unsigned int    mutexChnNo;
} BSPI_P_Handle;

typedef struct BSPI_P_ChannelHandle
{
    uint32_t            magicId;                    /* Used to check if structure is corrupt */
    BSPI_Handle         hSpi;
    unsigned int        chnNo;
    uint32_t            coreOffset;
    BKNI_EventHandle    hChnEvent;
    BINT_CallbackHandle hChnCallback;
    BSPI_Pcs            pcs;
    uint32_t            baud;
    uint8_t             clkConfig;
    bool                intMode;
    uint8_t             bitsPerTxfr;
    bool                lastByteContinueEnable;     /* Last Byte Contiue Enable Flag */
    bool                useUserDtlAndDsclk;         /* Use User specified DTL and DSCLK */
    BSPI_AssertSSFunc   assertSSFunc;               /* function to assert SS */
    BSPI_DeassertSSFunc deassertSSFunc;             /* function to deassert SS */
    BSPI_EbiCs          ebiCs;                      /* EBI CS line to use */
} BSPI_P_ChannelHandle;



/*******************************************************************************
*
*   Default Module Settings
*
*******************************************************************************/
static const BSPI_Settings defSpiSettings = NULL;

static const BSPI_ChannelSettings defSpiChn0Settings =
{
    MAX_SPI_BAUD,
    SPI_REG(SPCR0_MSB_CPOL_MASK) | SPI_REG(SPCR0_MSB_CPHA_MASK),
#if defined(BCHP_MSPI_SPCR3)
    false,
#else
    true,
#endif
    8,
    false,
    false,
    BSPI_SpiCore_Upg,
    BSPI_EbiCs_unused
};

static const BSPI_ChannelSettings defSpiChn1Settings =
{
    MAX_SPI_BAUD,
    SPI_REG(SPCR0_MSB_CPOL_MASK) | SPI_REG(SPCR0_MSB_CPHA_MASK),
#if defined(BCHP_MSPI_SPCR3)
    false,
#else
    true,
#endif

    8,
    false,
    false,
    BSPI_SpiCore_Upg,
    BSPI_EbiCs_unused
};

#if MAX_SPI_CHANNELS > 2
static const BSPI_ChannelSettings defSpiChn2Settings =
{
    MAX_SPI_BAUD,
    SPI_REG(SPCR0_MSB_CPOL_MASK) | SPI_REG(SPCR0_MSB_CPHA_MASK),
#if defined(BCHP_MSPI_SPCR3)
    false,
#else
    true,
#endif

    8,
    false,
    false,
    BSPI_SpiCore_Upg,
    BSPI_EbiCs_unused
};
#endif

#if HAS_HIF_MSPI
static const BSPI_ChannelSettings defHifSpiChn0Settings =
{
    MAX_SPI_BAUD,
    SPI_REG(SPCR0_MSB_CPOL_MASK) | SPI_REG(SPCR0_MSB_CPHA_MASK),
    true,
    8,
    false,
    false,
    BSPI_SpiCore_Hif,
    BSPI_EbiCs_2,
};

static const BSPI_ChannelSettings defHifSpiChn1Settings =
{
    MAX_SPI_BAUD,
    SPI_REG(SPCR0_MSB_CPOL_MASK) | SPI_REG(SPCR0_MSB_CPHA_MASK),
    true,
    8,
    false,
    false,
    BSPI_SpiCore_Hif,
    BSPI_EbiCs_2,
};
#endif

#if (BCHP_CHIP==3548) || (BCHP_CHIP==3556)
#define BSPI_USE_MAST_N_BOOT 1
#endif

void BSPI_P_ACQUIRE_UPG_MUTEX(BSPI_ChannelHandle hChn )
{
    uint16_t i=0;
    BSPI_Handle hDev;

    hDev = hChn->hSpi;

    /*Wait for a mutex for a maximum total time of 24,000 mseconds(SPI_MUTEX_POLLING_ITERATIONS * SPI_MUTEX_POLLING_INTERVAL), which is 4 minutes. */
    for(i=0; i<SPI_MUTEX_POLLING_ITERATIONS; i++){
        if(hDev->mutexChnNo != hChn->chnNo){
            if(hDev->mutexChnNo == hDev->maxChnNo){
                BKNI_AcquireMutex(hDev->hUpgMutex);
                hDev->mutexChnNo = hChn->chnNo;
                break;
            }
            else{
                BKNI_Sleep(SPI_MUTEX_POLLING_INTERVAL);
            }
        }
        else
            break;
    }
    if(i == SPI_MUTEX_POLLING_ITERATIONS)
        BDBG_ERR(("SPI mutex previously acquired by channel %d. Hence channel %d can't acquire mutex", hDev->mutexChnNo, hChn->chnNo));
}

void BSPI_P_RELEASE_UPG_MUTEX(BSPI_ChannelHandle hChn )
{
    BSPI_Handle hDev;

    hDev = hChn->hSpi;

    if((hDev->mutexChnNo == hChn->chnNo) && (!hChn->lastByteContinueEnable)){
        hDev->mutexChnNo = hDev->maxChnNo;
        BKNI_ReleaseMutex(hDev->hUpgMutex);
    }
}

/*******************************************************************************
*
*   Public Module Functions
*
*******************************************************************************/
BERR_Code BSPI_Open(
    BSPI_Handle *pSpi,                  /* [output] Returns handle */
    BCHP_Handle hChip,                  /* Chip handle */
    BREG_Handle hRegister,              /* Register handle */
    BINT_Handle hInterrupt,             /* Interrupt handle */
    const BSPI_Settings *pDefSettings   /* Default settings */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BSPI_Handle hDev;
    unsigned int chnIdx;

    /* Sanity check on the handles we've been given. */
    BDBG_ASSERT( hChip );
    BDBG_ASSERT( hRegister );
    BDBG_ASSERT( hInterrupt );
    BSTD_UNUSED( pDefSettings );

    /* Alloc memory from the system heap */
    hDev = (BSPI_Handle) BKNI_Malloc( sizeof( BSPI_P_Handle ) );
    if( hDev == NULL )
    {
        *pSpi = NULL;
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BSPI_Open: BKNI_malloc() failed\n"));
        goto done;
    }

    hDev->magicId   = DEV_MAGIC_ID;
    hDev->hChip     = hChip;
    hDev->hRegister = hRegister;
    hDev->hInterrupt = hInterrupt;
    hDev->maxChnNo  = MAX_SPI_CHANNELS + HIF_MSPI_MAX_CHANNELS;
    hDev->mutexChnNo = hDev->maxChnNo;
    for( chnIdx = 0; chnIdx < hDev->maxChnNo; chnIdx++ )
    {
        hDev->hSpiChn[chnIdx] = NULL;
    }
    retCode = BKNI_CreateMutex( &hDev->hUpgMutex );
    if( retCode != BERR_SUCCESS ) goto done;

    *pSpi = hDev;

done:
    if(retCode){
        BKNI_Free( hDev );
        hDev = NULL;
    }
    return( retCode );
}

BERR_Code BSPI_Close(
    BSPI_Handle hDev                    /* Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    if(hDev->hUpgMutex)BKNI_DestroyMutex(hDev->hUpgMutex);
    hDev->hUpgMutex = NULL;

    BKNI_Free( (void *) hDev );

    return( retCode );
}

BERR_Code BSPI_GetDefaultSettings(
    BSPI_Settings *pDefSettings,        /* [output] Returns default setting */
    BCHP_Handle hChip                   /* Chip handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BSTD_UNUSED(hChip);

    *pDefSettings = defSpiSettings;

    return( retCode );
}

BERR_Code BSPI_GetTotalChannels(
    BSPI_Handle hDev,                   /* Device handle */
    unsigned int *totalChannels         /* [output] Returns total number downstream channels supported */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    *totalChannels = hDev->maxChnNo;

    return( retCode );
}

BERR_Code BSPI_GetTotalUpgSpiChannels(
    BSPI_Handle hDev,                   /* Device handle */
    unsigned int *totalUpgChannels      /* [output] Returns total number of UPG SPI hannels supported */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    *totalUpgChannels = MAX_SPI_CHANNELS;

    return( retCode );
}

BERR_Code BSPI_GetTotalHifSpiChannels(
    BSPI_Handle hDev,                   /* Device handle */
    unsigned int *totalHifChannels      /* [output] Returns total number of HIF SPI hannels supported */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    *totalHifChannels = HIF_MSPI_MAX_CHANNELS;

    return( retCode );
}

BERR_Code BSPI_GetChannelDefaultSettings(
    BSPI_Handle hDev,                   /* Device handle */
    unsigned int channelNo,             /* Channel number to default setting for */
    BSPI_ChannelSettings *pChnDefSettings /* [output] Returns channel default setting */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

#if !BDBG_DEBUG_BUILD
    BSTD_UNUSED(hDev);
#endif

    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    switch (channelNo)
    {
        case 0:
            *pChnDefSettings = defSpiChn0Settings;
            break;

        case 1:
            *pChnDefSettings = defSpiChn1Settings;
            break;
#if MAX_SPI_CHANNELS > 2
        case 2:
            *pChnDefSettings = defSpiChn2Settings;
            break;
#endif

#if HAS_HIF_MSPI
        case MAX_SPI_CHANNELS:
            *pChnDefSettings = defHifSpiChn0Settings;
            break;

        case (MAX_SPI_CHANNELS+1):
            *pChnDefSettings = defHifSpiChn1Settings;
            break;

#endif

        default:
            retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
            break;

    }

    return( retCode );
}

BERR_Code BSPI_GetChannelDefaultSettings_Ext(
    BSPI_Handle         hDev,                   /* Device handle */
    BSPI_ChannelInfo    channel,                /* Channel number info */
    BSPI_ChannelSettings *pChnDefSettings       /* [output] Returns channel default setting */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

#if !BDBG_DEBUG_BUILD
    BSTD_UNUSED(hDev);
#endif

    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    if (channel.spiCore == BSPI_SpiCore_Upg)
    {
        switch (channel.channelNo)
        {
            case 0:
                *pChnDefSettings = defSpiChn0Settings;
                break;

            case 1:
                *pChnDefSettings = defSpiChn1Settings;
                break;
#if MAX_SPI_CHANNELS > 2
            case 2:
                *pChnDefSettings = defSpiChn2Settings;
                break;
#endif
            default:
                retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
                break;
        }
    }
    else
    {
        switch (channel.channelNo)
        {
#if HAS_HIF_MSPI
            case 0:
                *pChnDefSettings = defHifSpiChn0Settings;
                break;

            case 1:
                *pChnDefSettings = defHifSpiChn1Settings;
                break;
#endif
            default:
                retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
                break;
        }
    }

    return( retCode );
}

BERR_Code BSPI_OpenChannel(
    BSPI_Handle hDev,                   /* Device handle */
    BSPI_ChannelHandle *phChn,          /* [output] Returns channel handle */
    unsigned int channelNo,             /* Channel number to open */
    const BSPI_ChannelSettings *pChnDefSettings /* Channel default setting */
    )
{
    BERR_Code           retCode = BERR_SUCCESS;
    BSPI_ChannelHandle  hChnDev;
    uint32_t            lval;

    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    hChnDev = NULL;

    /*
     * Readjust channel number for HIF SPI if necessary.
     * Internally for bookkeeping, HIF SPI channels start after UPG SPI channels end.
     */
    #if HIF_MSPI_MAX_CHANNELS > 0
        if ((pChnDefSettings->spiCore == BSPI_SpiCore_Hif) && (channelNo < HIF_MSPI_MAX_CHANNELS))
        {
            BDBG_WRN(("HIF_MSPI channels are technically not supported for use for with-FLASH devices.  Please use a UPG_MSPI channel instead."));

            #if defined(BCHP_HIF_MSPI_SPCR3_fastbr_MASK)
                BDBG_WRN(("The max HIF_MSPI baud rate supported on this chip is 108 MBaud."));
            #else
                BDBG_WRN(("The max HIF_MSPI baud rate supported on this chip is 13.5 MBaud."));
            #endif

            #if defined(BCHP_MSPI_SPCR3_fastbr_MASK)
                BDBG_WRN(("The max UPG_MSPI baud rate supported on this chip is 108 MBaud."));
            #else
                BDBG_WRN(("The max UPG_MSPI baud rate supported on this chip is 108 MBaud."));
            #endif

            /* coverity[overrun_static] */
            channelNo += MAX_SPI_CHANNELS;
        }
    #endif

    if( channelNo < hDev->maxChnNo )
    {
        /* coverity[overrun_static] */
        if( hDev->hSpiChn[channelNo] == NULL )
        {
#if (BCHP_CHIP==7584)
            if ((pChnDefSettings->bitsPerTxfr != 32) && (pChnDefSettings->bitsPerTxfr != 64))
#endif
            if ((pChnDefSettings->bitsPerTxfr != 8) && (pChnDefSettings->bitsPerTxfr != 16))
            {
                retCode = BERR_INVALID_PARAMETER;
                goto done;
            }
            /* Alloc memory from the system heap */
            hChnDev = (BSPI_ChannelHandle) BKNI_Malloc( sizeof( BSPI_P_ChannelHandle ) );
            if( hChnDev == NULL )
            {
                *phChn = NULL;
                retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
                BDBG_ERR(("BSPI_OpenChannel: BKNI_malloc() failed\n"));
                goto done;
            }
            BKNI_Memset(hChnDev, 0, sizeof(*hChnDev));

            BSPI_CHK_RETCODE( retCode, BKNI_CreateEvent( &(hChnDev->hChnEvent) ) );
            hChnDev->magicId    = DEV_MAGIC_ID;
            hChnDev->hSpi       = hDev;

            /*
             * Set core offset
             */
#if HAS_HIF_MSPI
            if (pChnDefSettings->spiCore == BSPI_SpiCore_Upg)
            {
                hChnDev->coreOffset = 0;
            }
            else
            {
#if (HAS_UPG_MSPI==0)
                hChnDev->coreOffset = 0;
#else
                hChnDev->coreOffset = BCHP_HIF_MSPI_SPCR0_LSB - BCHP_MSPI_SPCR0_LSB;
#endif
            }
#else
            hChnDev->coreOffset = 0;
#endif
            switch (channelNo)
            {
                case 0:
                    hChnDev->pcs = BSPI_Pcs_eUpgSpiPcs0;
                    break;
                case 1:
                    hChnDev->pcs = BSPI_Pcs_eUpgSpiPcs1;
                    break;
            #if MAX_SPI_CHANNELS > 2
                case 2:
                    hChnDev->pcs = BSPI_Pcs_eUpgSpiPcs2;
                    break;
            #endif
            #if HAS_HIF_MSPI
                case MAX_SPI_CHANNELS:
                    hChnDev->pcs = BSPI_Pcs_eUpgSpiPcs0;
                    break;

                case (MAX_SPI_CHANNELS+1):
                    hChnDev->pcs = BSPI_Pcs_eUpgSpiPcs1;
                    break;
            #endif

                default:
                    retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
                    goto done;
            }
            hChnDev->baud       = pChnDefSettings->baud;
            hChnDev->clkConfig  = pChnDefSettings->clkConfig;
            hChnDev->bitsPerTxfr = pChnDefSettings->bitsPerTxfr;
            hChnDev->lastByteContinueEnable = pChnDefSettings->lastByteContinueEnable;
            hChnDev->useUserDtlAndDsclk = pChnDefSettings->useUserDtlAndDsclk;
            hChnDev->chnNo = channelNo;

            hChnDev->assertSSFunc = NULL;
            hChnDev->deassertSSFunc = NULL;

            hChnDev->ebiCs          = pChnDefSettings->ebiCs;

            hDev->hSpiChn[channelNo] = hChnDev;

            /* Program SPI clock setting */
            BSPI_P_ACQUIRE_UPG_MUTEX( hChnDev );
            BSPI_P_SetClk (hChnDev, pChnDefSettings->baud, pChnDefSettings->clkConfig);
            BSPI_P_RELEASE_UPG_MUTEX( hChnDev );

#if HAS_HIF_MSPI
            BSPI_P_SetEbiCs (hChnDev, pChnDefSettings->ebiCs);
#endif
            /*
             * Enable interrupt for this channel
             */
            hChnDev->intMode = pChnDefSettings->intMode;
            if (hChnDev->intMode == true)
            {
                /*
                 * Register and enable L2 interrupt.
                 */
#if !defined(BCHP_INT_ID_spi_irqen)
#if HAS_UPG_MSPI
#define BCHP_INT_ID_spi_irqen           BCHP_INT_ID_spi
#else
#define BCHP_INT_ID_spi_irqen           BCHP_INT_ID_MSPI_DONE
#endif
#endif
                if (channelNo < MAX_SPI_CHANNELS)
                {
                    retCode = BINT_CreateCallback(&(hChnDev->hChnCallback), hDev->hInterrupt,
                                    BCHP_INT_ID_spi_irqen, BSPI_P_HandleInterrupt_Isr, (void *) hChnDev, 0x00);
                }
#if HAS_HIF_MSPI
                else
                {
                    retCode = BINT_CreateCallback(&(hChnDev->hChnCallback), hDev->hInterrupt,
                                    BCHP_INT_ID_MSPI_DONE, BSPI_P_HandleInterrupt_Isr, (void *) hChnDev, 0x00);
                }
#endif

                if (retCode) {
                    BDBG_ERR(("Unable to enable IRQ0_SPI_IRQEN. See bint.inc and the BCHP_DISABLE_IRQ0_SPI option."));
                    retCode = BERR_TRACE(retCode);
                    goto done;
                }

                retCode = BINT_EnableCallback(hChnDev->hChnCallback);
                if (retCode) {
                    retCode = BERR_TRACE(retCode);
                    goto done;
                }

                BKNI_EnterCriticalSection();
                BSPI_P_EnableInt_isr(hChnDev);
                BKNI_LeaveCriticalSection();
            }
            else
            {
                hChnDev->hChnCallback = NULL;
                /* No need to disable BCHP_IRQ0_SPI_IRQEN. No one should have enabled it. Be aware that direct access to BCHP_IRQ0_SPI_IRQEN is not allowed
                in this PI. A potential conflict with the kernel is possible. See bint.inc for a note. */

                BKNI_EnterCriticalSection();
                BSPI_P_DisableInt_isr(hChnDev);
                BKNI_LeaveCriticalSection();
            }

            *phChn = hChnDev;

#if defined(BCHP_MSPI_SPCR2_cont_after_cmd_MASK) || defined(BCHP_HIF_MSPI_SPCR2_cont_after_cmd_MASK)
            /*
             * Set the master CONT after command bit in hardware.
             * The actual flag to control it is the lastByteContinueEnable flag.
             */
            BSPI_P_ACQUIRE_UPG_MUTEX( hChnDev );
            lval = BREG_Read32( hDev->hRegister, (hChnDev->coreOffset + SPI_REG(SPCR2)));
            lval |= SPI_REG(SPCR2_cont_after_cmd_MASK);
            BREG_Write32( hDev->hRegister, (hChnDev->coreOffset + SPI_REG(SPCR2)), lval);
            BSPI_P_RELEASE_UPG_MUTEX( hChnDev );
#else
            BSTD_UNUSED(lval);
#endif
        }
        else
        {
            retCode = BSPI_ERR_NOTAVAIL_CHN_NO;
        }
    }
    else
    {
        retCode = BERR_INVALID_PARAMETER;
    }

done:

    if( retCode != BERR_SUCCESS )
    {
        if( hChnDev != NULL )
        {
            BKNI_DestroyEvent( hChnDev->hChnEvent );
            BKNI_Free( hChnDev );
            hDev->hSpiChn[channelNo] = NULL;
            *phChn = NULL;
        }
    }
    return( retCode );
}

BERR_Code BSPI_CloseChannel(
    BSPI_ChannelHandle hChn         /* Device channel handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BSPI_Handle hDev;
    unsigned int chnNo;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hSpi;
    BSPI_P_ACQUIRE_UPG_MUTEX( hChn );
    /*
     * Disable interrupt for this channel
     */
    BKNI_EnterCriticalSection();
    BSPI_P_DisableInt_isr(hChn);
    BKNI_LeaveCriticalSection();

    if (hChn->hChnCallback != NULL)
    {
        (void)BINT_DestroyCallback( hChn->hChnCallback );
    }
    BKNI_DestroyEvent( hChn->hChnEvent );
    chnNo = hChn->chnNo;
    BSPI_P_RELEASE_UPG_MUTEX( hChn );
    BKNI_Free( hChn );
    hDev->hSpiChn[chnNo] = NULL;

    return( retCode );
}

BERR_Code BSPI_GetDevice(
    BSPI_ChannelHandle hChn,            /* Device channel handle */
    BSPI_Handle *phDev                  /* [output] Returns Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    *phDev = hChn->hSpi;

    return( retCode );
}


BERR_Code BSPI_CreateSpiRegHandle(
    BSPI_ChannelHandle hChn,            /* Device channel handle */
    BREG_SPI_Handle *pSpiReg            /* [output]  */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );


    *pSpiReg = (BREG_SPI_Handle)BKNI_Malloc( sizeof(BREG_SPI_Impl) );
    if( *pSpiReg == NULL )
    {
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BSPI_CreateSpiRegHandle: BKNI_malloc() failed\n"));
        goto done;
    }

    (*pSpiReg)->context                                  = (void *)hChn;
    (*pSpiReg)->BREG_SPI_Get_Bits_Per_Transfer_Func      = BSPI_P_GetBitsPerTransfer;
    (*pSpiReg)->BREG_SPI_Write_All_Func                  = BSPI_P_WriteAll;
    (*pSpiReg)->BREG_SPI_Write_Func                      = BSPI_P_Write;
    (*pSpiReg)->BREG_SPI_Read_Func                       = BSPI_P_Read;
    (*pSpiReg)->BREG_SPI_Read_All_Func                   = BSPI_P_ReadAll;
    (*pSpiReg)->BREG_SPI_Set_Continue_After_Command_Func = BSPI_P_SetContinueAfterCommand;

done:
    return( retCode );
}

BERR_Code BSPI_CloseSpiRegHandle(
    BREG_SPI_Handle     hSpiReg         /* SPI register handle */
    )
{

    BDBG_ASSERT( hSpiReg );
    BKNI_Free( (void *) hSpiReg );

    return BERR_SUCCESS ;
}

BERR_Code BSPI_AbortTxfr(
    BSPI_ChannelHandle hChn         /* Device channel handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    uint32_t lval;
    BSPI_Handle hDev;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hSpi;
    BSPI_P_ACQUIRE_UPG_MUTEX( hChn );
    lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR2)));
    if (!(lval & SPI_REG(SPCR2_spe_MASK)))         /* if no transfer is going on, we're done */
        goto done;

    /* Set the HALT bit to stop the transfer */
    lval |= SPI_REG(SPCR2_halt_MASK);
    BREG_Write32 (hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR2)), lval);

    /* Now wait until the Halt Acknowledge bit is set */
    do
    {
        lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + SPI_REG(MSPI_STATUS)));
    } while (!(lval & SPI_REG(MSPI_STATUS_HALTA_MASK)));

    BKNI_EnterCriticalSection();
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(MSPI_STATUS)), 0);
    BKNI_LeaveCriticalSection();
    BREG_Write32 (hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR2)), 0);

done:
    BSPI_P_RELEASE_UPG_MUTEX( hChn );
    return( retCode );
}

BERR_Code BSPI_GetClkConfig(
    BSPI_ChannelHandle hChn,                /* Device channel handle */
    uint8_t *pClkConfig                     /* Pointer to clock config */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    *pClkConfig = hChn->clkConfig;

    return( retCode );
}

BERR_Code BSPI_SetClkConfig(
    BSPI_ChannelHandle hChn,            /* Device channel handle */
    uint8_t clkConfig                   /* clock config */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hChn->clkConfig = clkConfig;

    return( retCode );
}

BERR_Code BSPI_GetDTLConfig (
    BSPI_ChannelHandle hChn,    /* Device channel handle */
    uint32_t *pDTLConfig        /* pointer to DTLConfig */
    )
{
    BSPI_Handle         hDev;
    uint32_t            data;
    BERR_Code           retCode = BERR_SUCCESS;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hSpi;
    BSPI_P_ACQUIRE_UPG_MUTEX( hChn );
    data = BREG_Read32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR1_LSB)));
    *pDTLConfig = (data & SPI_REG(SPCR1_LSB_DTL_MASK));
    BSPI_P_RELEASE_UPG_MUTEX( hChn );

    return( retCode );
}

BERR_Code BSPI_SetDTLConfig (
    BSPI_ChannelHandle context,             /* Device channel handle */
    const uint32_t data         /* data to write */
    )
{
    BSPI_Handle         hDev=NULL;
    BSPI_ChannelHandle  hChn;
    BERR_Code           retCode = BERR_SUCCESS;

    if (data > 255)
    {
        retCode = BERR_INVALID_PARAMETER;
        goto done;
    }

    hChn = context;
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hSpi;
    BSPI_P_ACQUIRE_UPG_MUTEX( hChn );
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR1_LSB)), data );
    BSPI_P_RELEASE_UPG_MUTEX( hChn );

done:
    return retCode;
}


BERR_Code BSPI_GetRDSCLKConfig (
    BSPI_ChannelHandle hChn,    /* Device channel handle */
    uint32_t *pRDSCLKConfig     /* pointer to RDSCLKConfig */
    )
{
    BSPI_Handle         hDev;
    uint32_t            data;
    BERR_Code           retCode = BERR_SUCCESS;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hSpi;
    BSPI_P_ACQUIRE_UPG_MUTEX( hChn );
    data = BREG_Read32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR1_MSB)));
    *pRDSCLKConfig = (data & SPI_REG(SPCR1_MSB_RDSCLK_MASK));
    BSPI_P_RELEASE_UPG_MUTEX( hChn );

    return( retCode );
}

BERR_Code BSPI_SetRDSCLKConfig(
    BSPI_ChannelHandle context,             /* Device channel handle */
    const uint32_t data         /* data to write */
    )
{
    BSPI_Handle         hDev=NULL;
    BSPI_ChannelHandle  hChn;
    BERR_Code           retCode = BERR_SUCCESS;

    if (data > 255)
    {
        retCode = BERR_INVALID_PARAMETER;
        goto done;
    }

    hChn = context;
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hSpi;
    BSPI_P_ACQUIRE_UPG_MUTEX( hChn );
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR1_MSB)), data );
    BSPI_P_RELEASE_UPG_MUTEX( hChn );

done:
    return retCode;
}

void BSPI_RegisterSSFunctions (
    BSPI_ChannelHandle hChn,                /* Device channel handle */
    BSPI_AssertSSFunc assertFunc,           /* Assert SS function */
    BSPI_DeassertSSFunc deassertFunc        /* Deassert SS function */
    )
{
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hChn->assertSSFunc = assertFunc;
    hChn->deassertSSFunc = deassertFunc;
}


void BSPI_SetLastByteContinueEnable(
    BSPI_ChannelHandle hChn,
    bool bEnable
)
{
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    BSPI_P_ACQUIRE_UPG_MUTEX( hChn );

    hChn->lastByteContinueEnable = bEnable;

    BSPI_P_RELEASE_UPG_MUTEX( hChn );
}

BERR_Code BSPI_P_Start(
    BSPI_ChannelHandle hChn         /* Device channel handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    uint32_t lval=0;
    BSPI_Handle hDev;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hSpi;

    lval = BREG_Read32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR2)));

    lval |= SPI_REG(SPCR2_spe_MASK);
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR2)), lval);

    return( retCode );
}

void BSPI_P_SetContinueAfterCommand(
    void *context,              /* Device channel handle */
    bool bEnable
)
{
    BSPI_Handle         hDev;
    BSPI_ChannelHandle  hChn;
    uint32_t            lval = 0;

    hChn = (BSPI_ChannelHandle)context;
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hSpi;
    BSPI_P_ACQUIRE_UPG_MUTEX( hChn );

    hChn->lastByteContinueEnable = bEnable;

    /* Set cont_after_cmd bit. */
    lval = BREG_Read32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR2)));
    if(bEnable){
        lval |= 0x80;
    }
    else{
        lval &= 0x7f;
    }
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR2)), lval);

    BSPI_P_RELEASE_UPG_MUTEX( hChn );
}


/*******************************************************************************
*
*   Private Module Functions
*
*******************************************************************************/
void BSPI_P_GetBitsPerTransfer
(
    void *context,             /* Device channel handle */
    uint8_t *pBitsPerTransfer  /* Number of bits transferred per command byte */
)
{
    BSPI_Handle         hDev;
    BSPI_ChannelHandle  hChn;

    hChn = (BSPI_ChannelHandle)context;
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hSpi;
    BSPI_P_ACQUIRE_UPG_MUTEX( hChn );

    *pBitsPerTransfer = hChn->bitsPerTxfr;

    BSPI_P_RELEASE_UPG_MUTEX( hChn );
    return;
}


/* Only supported for 8 bit transfer mode for now. */
BERR_Code BSPI_P_ReadAll
(
    void *context,              /* Device channel handle */
    const uint8_t *pWriteData,      /* pointer to memory location where data is to sent  */
    size_t writeLength,             /* size of *pWriteData buffer */
    uint8_t *pReadData,             /* pointer to memory location to store read data  */
    size_t readLength               /* size of *pReadData buffer */
)
{
    BSPI_Handle         hDev;
    BSPI_ChannelHandle  hChn;
    BERR_Code           retCode = BERR_SUCCESS;
    uint32_t            lval_cont = 0, lval_stop = 0, lval = 0, i = 0, j = 0, k = 0, count = 0;

    hChn = (BSPI_ChannelHandle)context;
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hSpi;
    BSPI_P_ACQUIRE_UPG_MUTEX( hChn );

    if (hChn->assertSSFunc)
    {
        (*(hChn->assertSSFunc)) ();
    }

    if( hChn->useUserDtlAndDsclk == true)
    {
        lval_stop = SPI_CDRAM_PCS_DISABLE_ALL | SPI_CDRAM_DT | SPI_CDRAM_DSCK;
    }
    else
    {
        lval_stop = SPI_CDRAM_PCS_DISABLE_ALL;
    }
    lval_stop &= ~(1 << hChn->pcs);

    lval_cont = lval_stop | SPI_CDRAM_CONT;

    for (; i < readLength+writeLength; i++, count++)
    {
        count  %= MAX_SPI_XFER;

        if(i < writeLength)
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(TXRAM00) + (count * 8)), (uint32_t)(pWriteData[i]) );

        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(CDRAM00) + (count * 4)), lval_cont );

        if(i == (readLength+writeLength-1))
        {
            /* Skip for slower SPI device  */
            if( hChn->lastByteContinueEnable == false){
                BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(CDRAM00) + (count * 4)), lval_stop );
            }
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(NEWQP)), 0 );
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(ENDQP)), count );

            BSPI_P_Start(hChn);
            /* Wait for SPI to finish */
            BSPI_CHK_RETCODE( retCode, BSPI_P_WaitForCompletion(hChn, MAX_SPI_XFER));

            if(i>=writeLength){
                for (k=0; k<=count; k++)
                {
                    lval = BREG_Read32( hDev->hRegister, (hChn->coreOffset + SPI_REG(RXRAM00) + (k * 8) + 4));
                    /* j is unsigned and hence pReadData[j++ - writeLength] overwritten till j >= writelength */
                    pReadData[j++ - writeLength] = (uint8_t)lval;
                }
            }
            break;
        }

        if(count == (MAX_SPI_XFER - 1)) {
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(NEWQP)), 0 );
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(ENDQP)), count );

            BSPI_P_Start(hChn);
            /* Wait for SPI to finish */
            BSPI_CHK_RETCODE( retCode, BSPI_P_WaitForCompletion(hChn, MAX_SPI_XFER));

            if(i>=writeLength){
                for (k=0; k<=count; k++)
                {
                    lval = BREG_Read32( hDev->hRegister, (hChn->coreOffset + SPI_REG(RXRAM00) + (k * 8) + 4));
                    /* j is unsigned and hence pReadData[j++ - writeLength] overwritten till j >= writelength */
                    pReadData[j++ - writeLength] = (uint8_t)lval;
                }
            }
        }
    }
done:
    if (hChn->deassertSSFunc)
    {
        (*(hChn->deassertSSFunc)) ();
    }
    BSPI_P_RELEASE_UPG_MUTEX( hChn );
    return retCode;
}

BERR_Code BSPI_P_Read
(
    void *context,              /* Device channel handle */
    const uint8_t *pWriteData,      /* pointer to write data  */
    uint8_t *pReadData,         /* pointer to memory location to store read data  */
    size_t length               /* number of bytes to read + number of bytes to write */
)
{
    BSPI_Handle         hDev;
    BSPI_ChannelHandle  hChn;
    BERR_Code           retCode = BERR_SUCCESS;
    uint32_t            lval = 0, i;
#if (BCHP_CHIP==7584)
    uint32_t            j;
#endif

    hChn = (BSPI_ChannelHandle)context;
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hSpi;
    BSPI_P_ACQUIRE_UPG_MUTEX( hChn );

#if (BCHP_CHIP==7584)
    if (((hChn->bitsPerTxfr == 8) && (length > MAX_SPI_XFER)) ||
        ((hChn->bitsPerTxfr == 16) && (length > (MAX_SPI_XFER * 2))) || 
        ((hChn->bitsPerTxfr == 32) && (length > (MAX_SPI_XFER * 4))) || 
        ((hChn->bitsPerTxfr == 64) && (length > (MAX_SPI_XFER * 8))))
#else
    if (((hChn->bitsPerTxfr == 8) && (length > MAX_SPI_XFER)) ||
        ((hChn->bitsPerTxfr == 16) && (length > (MAX_SPI_XFER * 2))))
#endif
    {
        retCode = BERR_INVALID_PARAMETER;
        goto done;
    }

    if ((hChn->bitsPerTxfr == 16) && (length & 0x01))       /* if the length is odd, round it up */
        length++;
#if (BCHP_CHIP==7584)
    if ((hChn->bitsPerTxfr == 32) && (length & 0x03))       /* round up length */
        length = (length & ~0x03) + 4;
    if ((hChn->bitsPerTxfr == 64) && (length & 0x07))       /* round up length */
        length = (length & ~0x07) + 8;
#endif

#if BSPI_USE_MAST_N_BOOT || HAS_HIF_MSPI
    if (hChn->chnNo >= MAX_SPI_CHANNELS)                    /* for HIF channels */
    {
        lval = BREG_Read32(hDev->hRegister, BCHP_BSPI_MAST_N_BOOT_CTRL);
        lval |= BCHP_BSPI_MAST_N_BOOT_CTRL_mast_n_boot_MASK;        /* give control to HIF_MSPI */
        BREG_Write32( hDev->hRegister, BCHP_BSPI_MAST_N_BOOT_CTRL, lval );
    }
#endif
#if 0
    /* Initialize the SPI clock */
    BSPI_P_SetClk (hChn, hChn->baud, hChn->clkConfig);
#endif
    /* Fill the TX and CMD buffers */
    if (hChn->bitsPerTxfr == 8)
    {
        /*
         * 8-bit transfer mode
         */
        for (i=0; i<length; i++)
        {
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(TXRAM00) + (i * 8)), (uint32_t)(pWriteData[i]) );
            if( hChn->useUserDtlAndDsclk == true)
            {
                lval = SPI_CDRAM_CONT | SPI_CDRAM_PCS_DISABLE_ALL | SPI_CDRAM_DT | SPI_CDRAM_DSCK;
            }
            else
            {
                lval = SPI_CDRAM_CONT | SPI_CDRAM_PCS_DISABLE_ALL;
            }
            lval &= ~(1 << hChn->pcs);
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(CDRAM00) + (i * 4)), lval );
        }

        /* Skip for slower SPI device  */
        if( hChn->lastByteContinueEnable == false)
        {
            /*
             * Last one don't need to assert SS
             */
            if( hChn->useUserDtlAndDsclk == true)
            {
                lval = SPI_CDRAM_PCS_DISABLE_ALL | SPI_CDRAM_DT | SPI_CDRAM_DSCK;
            }
            else
            {
                lval = SPI_CDRAM_PCS_DISABLE_ALL;
            }
            lval &= ~(1 << hChn->pcs);
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(CDRAM00) + ((length - 1) * 4)), lval );
        }

        /* Set queue pointers */
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(NEWQP)), 0 );
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(ENDQP)), (uint32_t)(length - 1) );
    }
    else if (hChn->bitsPerTxfr == 16)
    {
        /*
         * 16-bit transfer mode
         */
        lval = BREG_Read32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR0_MSB)));
        lval &= ~SPI_REG(SPCR0_MSB_BitS_MASK);
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR0_MSB)), lval);

        for (i=0; i<length; i+=2)
        {
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(TXRAM00) + (i * 4)),     (uint32_t)(pWriteData[i]) );
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(TXRAM00) + (i * 4) + 4), (uint32_t)(pWriteData[i+1]) );

            if( hChn->useUserDtlAndDsclk == true)
            {
                lval = SPI_CDRAM_CONT | SPI_CDRAM_PCS_DISABLE_ALL | SPI_CDRAM_DT | SPI_CDRAM_DSCK | SPI_CDRAM_BITSE;
            }
            else
            {
                lval = SPI_CDRAM_CONT | SPI_CDRAM_PCS_DISABLE_ALL | SPI_CDRAM_BITSE;
            }
            lval &= ~(1 << hChn->pcs);
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(CDRAM00) + ((i/2) * 4)), lval );
        }

        /* Skip for slower SPI device  */
        if( hChn->lastByteContinueEnable == false)
        {
            /*
             * Last one don't need to assert SS
             */
            if( hChn->useUserDtlAndDsclk == true)
            {
                lval = SPI_CDRAM_PCS_DISABLE_ALL | SPI_CDRAM_DT | SPI_CDRAM_DSCK | SPI_CDRAM_BITSE;
            }
            else
            {
                lval = SPI_CDRAM_PCS_DISABLE_ALL | SPI_CDRAM_BITSE;
            }
            lval &= ~(1 << hChn->pcs);
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(CDRAM00) + (((length/2) - 1) * 4)), lval );
        }

        /* Set queue pointers */
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(NEWQP)), 0 );
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(ENDQP)), (uint32_t)((length/2) - 1) );
    }
#if (BCHP_CHIP==7584)
    else if (hChn->bitsPerTxfr == 32)
    {
        /*
         * 32-bit transfer mode
         */
        lval = BREG_Read32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR3)));
        lval |= SPI_REG(SPCR3_data_reg_size_MASK);
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR3)), lval);

        lval = BREG_Read32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR0_MSB)));
        lval &= ~SPI_REG(SPCR0_MSB_BitS_MASK);
        lval |= (0x20 << SPI_REG(SPCR0_MSB_BitS_SHIFT));
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR0_MSB)), lval);

        for (i=0, j=0; i<length; i+=4, j+=2)
        {
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(TXRAM00) + (j * 4)),     
                        ((uint32_t)(pWriteData[i])  << 24) | ((uint32_t)(pWriteData[i+1]) << 16) |
                        ((uint32_t)(pWriteData[i+2]) << 8) |  (uint32_t)(pWriteData[i+3]));

            if( hChn->useUserDtlAndDsclk == true)
            {
                lval = SPI_CDRAM_CONT | SPI_CDRAM_PCS_DISABLE_ALL | SPI_CDRAM_DT | SPI_CDRAM_DSCK | SPI_CDRAM_BITSE;
            }
            else
            {
                lval = SPI_CDRAM_CONT | SPI_CDRAM_PCS_DISABLE_ALL | SPI_CDRAM_BITSE;
            }
            lval &= ~(1 << hChn->pcs);
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(CDRAM00) + ((j/2) * 4)), lval );
        }

        /* Skip for slower SPI device  */
        if( hChn->lastByteContinueEnable == false)
        {
            /*
             * Last one don't need to assert SS
             */
            if( hChn->useUserDtlAndDsclk == true)
            {
                lval = SPI_CDRAM_PCS_DISABLE_ALL | SPI_CDRAM_DT | SPI_CDRAM_DSCK | SPI_CDRAM_BITSE;
            }
            else
            {
                lval = SPI_CDRAM_PCS_DISABLE_ALL | SPI_CDRAM_BITSE;
            }
            lval &= ~(1 << hChn->pcs);
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(CDRAM00) + (((length/4) - 1) * 4)), lval );
        }

        /* Set queue pointers */
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(NEWQP)), 0 );
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(ENDQP)), (uint32_t)((length/4) - 1) );
    }
    else if (hChn->bitsPerTxfr == 64)
    {
        /*
         * 32-bit transfer mode
         */
        lval = BREG_Read32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR3)));
        lval |= SPI_REG(SPCR3_data_reg_size_MASK);
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR3)), lval);

        lval = BREG_Read32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR0_MSB)));
        lval &= ~SPI_REG(SPCR0_MSB_BitS_MASK);
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR0_MSB)), lval);

        for (i=0, j=0; i<length; i+=8, j+=2)
        {
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(TXRAM00) + (j * 4)),     
                        ((uint32_t)(pWriteData[i]) << 24)  | ((uint32_t)(pWriteData[i+1]) << 16) |
                        ((uint32_t)(pWriteData[i+2]) << 8) |  (uint32_t)(pWriteData[i+3]));
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(TXRAM00) + (j * 4) + 4),
                        ((uint32_t)(pWriteData[i+4]) << 24) | ((uint32_t)(pWriteData[i+5]) << 16) |
                        ((uint32_t)(pWriteData[i+6]) << 8)  |  (uint32_t)(pWriteData[i+7]));

            if( hChn->useUserDtlAndDsclk == true)
            {
                lval = SPI_CDRAM_CONT | SPI_CDRAM_PCS_DISABLE_ALL | SPI_CDRAM_DT | SPI_CDRAM_DSCK;
            }
            else
            {
                lval = SPI_CDRAM_CONT | SPI_CDRAM_PCS_DISABLE_ALL;
            }
            lval &= ~(1 << hChn->pcs);
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(CDRAM00) + ((j/2) * 4)), lval );
        }

        /* Skip for slower SPI device  */
        if( hChn->lastByteContinueEnable == false)
        {
            /*
             * Last one don't need to assert SS
             */
            if( hChn->useUserDtlAndDsclk == true)
            {
                lval = SPI_CDRAM_PCS_DISABLE_ALL | SPI_CDRAM_DT | SPI_CDRAM_DSCK;
            }
            else
            {
                lval = SPI_CDRAM_PCS_DISABLE_ALL;
            }
            lval &= ~(1 << hChn->pcs);
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(CDRAM00) + (((length/8) - 1) * 4)), lval );
        }

        /* Set queue pointers */
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(NEWQP)), 0 );
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(ENDQP)), (uint32_t)((length/8) - 1) );
    }
#endif

    if (hChn->assertSSFunc)
    {
        (*(hChn->assertSSFunc)) ();
    }

    /* Start SPI transfer */
    lval = BREG_Read32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR2)));
    lval |= SPI_REG(SPCR2_spe_MASK);
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR2)), lval);

    /* Wait for SPI to finish */
    BSPI_CHK_RETCODE( retCode, BSPI_P_WaitForCompletion(hChn, length));

    if (hChn->deassertSSFunc)
    {
        (*(hChn->deassertSSFunc)) ();
    }

    /* Transfer finished, clear SPIF bit */
    BKNI_EnterCriticalSection();
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(MSPI_STATUS)), 0);
    BKNI_LeaveCriticalSection();
    /* Copy data to user buffer */
    if (hChn->bitsPerTxfr == 8)
    {
        /*
         * 8-bit transfer mode
         */
        for (i=0; i<length; i++)
        {
            lval = BREG_Read32( hDev->hRegister, (hChn->coreOffset + SPI_REG(RXRAM00) + (i * 8) + 4));
            pReadData[i] = (uint8_t)lval;
        }
    }
    else if (hChn->bitsPerTxfr == 16)
    {
        /*
         * 16-bit transfer mode
         */
        for (i=0; i<length; i+=2)
        {
            lval = BREG_Read32( hDev->hRegister, (hChn->coreOffset + SPI_REG(RXRAM00) + (i * 4)));
            pReadData[i] = (uint8_t)lval;
            lval = BREG_Read32( hDev->hRegister, (hChn->coreOffset + SPI_REG(RXRAM00) + (i * 4) + 4));
            pReadData[i+1] = (uint8_t)lval;
        }
    }
#if (BCHP_CHIP==7584)
    else if (hChn->bitsPerTxfr == 32)
    {
        /*
         * 32-bit transfer mode
         */
        for (i=0, j=0; i<length; i+=4, j+=2)
        {
            lval = BREG_Read32( hDev->hRegister, (hChn->coreOffset + SPI_REG(RXRAM00) + (j * 4) + 4));
            pReadData[i]   = (uint8_t)(lval >> 24);
            pReadData[i+1] = (uint8_t)(lval >> 16);
            pReadData[i+2] = (uint8_t)(lval >>  8);
            pReadData[i+3] = (uint8_t)(lval & 0xff);
        }
    }
    else if (hChn->bitsPerTxfr == 64)
    {
        /*
         * 64-bit transfer mode
         */
        for (i=0, j=0; i<length; i+=8, j+=2)
        {
            lval = BREG_Read32( hDev->hRegister, (hChn->coreOffset + SPI_REG(RXRAM00) + (j * 4)));
            pReadData[i]   = (uint8_t)(lval >> 24);
            pReadData[i+1] = (uint8_t)(lval >> 16);
            pReadData[i+2] = (uint8_t)(lval >>  8);
            pReadData[i+3] = (uint8_t)lval;
            lval = BREG_Read32( hDev->hRegister, (hChn->coreOffset + SPI_REG(RXRAM00) + (j * 4) + 4));
            pReadData[i+4] = (uint8_t)(lval >> 24);
            pReadData[i+5] = (uint8_t)(lval >> 16);
            pReadData[i+6] = (uint8_t)(lval >>  8);
            pReadData[i+7] = (uint8_t)lval;
        }
    }
#endif

done:
#if BSPI_USE_MAST_N_BOOT || HAS_HIF_MSPI
    if (hChn->chnNo >= MAX_SPI_CHANNELS)                    /* for HIF channels */
    {
        lval = BREG_Read32(hDev->hRegister, BCHP_BSPI_MAST_N_BOOT_CTRL);
        lval &= ~BCHP_BSPI_MAST_N_BOOT_CTRL_mast_n_boot_MASK;   /* give control to BSPI */
        BREG_Write32( hDev->hRegister, BCHP_BSPI_MAST_N_BOOT_CTRL, lval );
    }
#endif
    BSPI_P_RELEASE_UPG_MUTEX( hChn );

    return retCode;
}

BERR_Code BSPI_P_WriteAll
(
    void *context,          /* Device channel handle */
    const uint8_t *pData,   /* pointer to data to write */
    size_t length           /* number of bytes to write */
)
{
    BSPI_Handle         hDev;
    BSPI_ChannelHandle  hChn;
    BERR_Code           retCode = BERR_SUCCESS;
    uint32_t            lval_cont = 0, lval_stop = 0, i = 0, count = 0;

    hChn = (BSPI_ChannelHandle)context;
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hSpi;
    BSPI_P_ACQUIRE_UPG_MUTEX( hChn );

    if (hChn->assertSSFunc)
    {
        (*(hChn->assertSSFunc)) ();
    }

    if( hChn->useUserDtlAndDsclk == true)
    {
        lval_stop = SPI_CDRAM_PCS_DISABLE_ALL | SPI_CDRAM_DT | SPI_CDRAM_DSCK;
    }
    else
    {
        lval_stop = SPI_CDRAM_PCS_DISABLE_ALL;
    }
    lval_stop &= ~(1 << hChn->pcs);

    lval_cont = lval_stop | SPI_CDRAM_CONT;

    for (i=0, count = 0; i<length; i++, count++)
    {
        count  %= MAX_SPI_XFER;

        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(TXRAM00) + (count * 8)), (uint32_t)(pData[i]) );
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(CDRAM00) + (count * 4)), lval_cont );

        if(i == (length - 1))
        {
            /* Skip for slower SPI device  */
            if( hChn->lastByteContinueEnable == false){
                BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(CDRAM00) + (count * 4)), lval_stop );
            }
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(NEWQP)), 0 );
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(ENDQP)), count );

            BSPI_P_Start(hChn);
            /* Wait for SPI to finish */
            BSPI_CHK_RETCODE( retCode, BSPI_P_WaitForCompletion(hChn, MAX_SPI_XFER));
            break;
        }

        if(count == (MAX_SPI_XFER - 1)) {
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(NEWQP)), 0 );
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(ENDQP)), count );

            BSPI_P_Start(hChn);
            /* Wait for SPI to finish */
            BSPI_CHK_RETCODE( retCode, BSPI_P_WaitForCompletion(hChn, MAX_SPI_XFER));
        }
    }
done:
    if (hChn->deassertSSFunc)
    {
        (*(hChn->deassertSSFunc)) ();
    }
    BSPI_P_RELEASE_UPG_MUTEX( hChn );
    return retCode;

}

BERR_Code BSPI_P_Write
(
    void *context,              /* Device channel handle */
    const uint8_t *pData,               /* pointer to data to write */
    size_t length               /* number of bytes to write */
)
{
    BSPI_Handle         hDev;
    BSPI_ChannelHandle  hChn;
    BERR_Code           retCode = BERR_SUCCESS;
    uint32_t            lval = 0, i, num8BitBytes=0;
#if (BCHP_CHIP==7584)
    uint32_t            j;
#endif

    hChn = (BSPI_ChannelHandle)context;
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hSpi;
    BSPI_P_ACQUIRE_UPG_MUTEX( hChn );

#if (BCHP_CHIP==7584)
    if (((hChn->bitsPerTxfr == 8) && (length > MAX_SPI_XFER)) ||
        ((hChn->bitsPerTxfr == 16) && (length > (MAX_SPI_XFER * 2))) || 
        ((hChn->bitsPerTxfr == 32) && (length > (MAX_SPI_XFER * 4))) || 
        ((hChn->bitsPerTxfr == 64) && (length > (MAX_SPI_XFER * 8))))
#else
    if (((hChn->bitsPerTxfr == 8) && (length > MAX_SPI_XFER)) ||
        ((hChn->bitsPerTxfr == 16) && (length > (MAX_SPI_XFER * 2))))
#endif
    {
        retCode = BERR_INVALID_PARAMETER;
        goto done;
    }

#if (BCHP_CHIP==7584)
    if (((hChn->bitsPerTxfr == 32) && (length & 0x03)) || 
        ((hChn->bitsPerTxfr == 64) && (length & 0x07)))
    {
        retCode = BERR_INVALID_PARAMETER;
        goto done;
    }
#endif

#if BSPI_USE_MAST_N_BOOT || HAS_HIF_MSPI
    if (hChn->chnNo >= MAX_SPI_CHANNELS)                    /* for HIF channels */
    {
        lval = BREG_Read32(hDev->hRegister, BCHP_BSPI_MAST_N_BOOT_CTRL);
        lval |= BCHP_BSPI_MAST_N_BOOT_CTRL_mast_n_boot_MASK;    /* give control to HIF_MSPI */
        BREG_Write32( hDev->hRegister, BCHP_BSPI_MAST_N_BOOT_CTRL, lval );
    }
#endif

    /* Fill the TX and CMD buffers */
    if (hChn->bitsPerTxfr == 8)
    {
        /*
         * 8-bit transfer mode
         */
        for (i=0; i<length; i++)
        {
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(TXRAM00) + (i * 8)), (uint32_t)(pData[i]) );
            if( hChn->useUserDtlAndDsclk == true)
            {
                lval = SPI_CDRAM_CONT | SPI_CDRAM_PCS_DISABLE_ALL | SPI_CDRAM_DT | SPI_CDRAM_DSCK;
            }
            else
            {
                lval = SPI_CDRAM_CONT | SPI_CDRAM_PCS_DISABLE_ALL;
            }
            lval &= ~(1 << hChn->pcs);
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(CDRAM00) + (i * 4)), lval );
        }

        /* Skip for slower SPI device  */
        if( hChn->lastByteContinueEnable == false)
        {
            /*
             * Last one don't need to assert SS
             */
            if( hChn->useUserDtlAndDsclk == true)
            {
                lval = SPI_CDRAM_PCS_DISABLE_ALL | SPI_CDRAM_DT | SPI_CDRAM_DSCK;
            }
            else
            {
                lval = SPI_CDRAM_PCS_DISABLE_ALL;
            }
            lval &= ~(1 << hChn->pcs);
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(CDRAM00) + ((length - 1) * 4)), lval );
        }

        /* Set queue pointers */
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(NEWQP)), 0 );
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(ENDQP)), (uint32_t)(length - 1) );
    }
    else if (hChn->bitsPerTxfr == 16)
    {
        num8BitBytes = length % (hChn->bitsPerTxfr/8);
        BDBG_MSG(("length = %d, num8BitBytes = %d", length, num8BitBytes));
        if(num8BitBytes==1){
            /* 16-bit transfer mode */
            lval = BREG_Read32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR0_MSB)));
            lval &= ~SPI_REG(SPCR0_MSB_BitS_MASK);
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR0_MSB)), lval);
        }

        for (i=0; i<length/2; i++)
        {
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(TXRAM00) + (2 * i * 4)),     (uint32_t)(pData[2*i]) );
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(TXRAM00) + (2 * i * 4) + 4), (uint32_t)(pData[(2*i)+1]) );

            if( hChn->useUserDtlAndDsclk == true)
            {
                lval = SPI_CDRAM_CONT | SPI_CDRAM_PCS_DISABLE_ALL | SPI_CDRAM_DT | SPI_CDRAM_DSCK | SPI_CDRAM_BITSE;
            }
            else
            {
                lval = SPI_CDRAM_CONT | SPI_CDRAM_PCS_DISABLE_ALL | SPI_CDRAM_BITSE;
            }
            lval &= ~(1 << hChn->pcs);
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(CDRAM00) + (i * 4)), lval );
        }

        if(num8BitBytes==1){
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(TXRAM00) + ((length - 1) * 4)), (uint32_t)(pData[length-1]));
            if( hChn->useUserDtlAndDsclk == true)
            {
                lval = SPI_CDRAM_PCS_DISABLE_ALL | SPI_CDRAM_DT | SPI_CDRAM_DSCK;
            }
            else
            {
                lval = SPI_CDRAM_PCS_DISABLE_ALL;
            }
            lval &= ~(1 << hChn->pcs);
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(CDRAM00) + ((length/2) * 4)), lval );
        }
        else{
            /* Skip for slower SPI device  */
            if( hChn->lastByteContinueEnable == false)
            {
                /* Last one don't need to assert SS */
                if( hChn->useUserDtlAndDsclk == true)
                {
                    lval = SPI_CDRAM_PCS_DISABLE_ALL | SPI_CDRAM_DT | SPI_CDRAM_DSCK | SPI_CDRAM_BITSE;
                }
                else
                {
                    lval = SPI_CDRAM_PCS_DISABLE_ALL | SPI_CDRAM_BITSE;
                }
                lval &= ~(1 << hChn->pcs);
                BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(CDRAM00) + (((length/2) - 1) * 4)), lval );
            }
        }

        /* Set queue pointers */
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(NEWQP)), 0 );
        if(length==1)
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(ENDQP)), 0 );
        else if(num8BitBytes)
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(ENDQP)), (uint32_t)(length/2) );
        else
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(ENDQP)), (uint32_t)((length/2) - 1) );
    }
#if (BCHP_CHIP==7584)
    else if (hChn->bitsPerTxfr == 32)
    {
        /*
         * 32-bit transfer mode
         */
        lval = BREG_Read32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR3)));
        lval |= SPI_REG(SPCR3_data_reg_size_MASK);
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR3)), lval);

        lval = BREG_Read32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR0_MSB)));
        lval &= ~SPI_REG(SPCR0_MSB_BitS_MASK);
        lval |= (0x20 << SPI_REG(SPCR0_MSB_BitS_SHIFT));
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR0_MSB)), lval);

        for (i=0, j=0; i<length; i+=4, j+=2)
        {
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(TXRAM00) + (j * 4)),     
                        ((uint32_t)(pData[i])  << 24) | ((uint32_t)(pData[i+1]) << 16) |
                        ((uint32_t)(pData[i+2]) << 8) |  (uint32_t)(pData[i+3]));

            if( hChn->useUserDtlAndDsclk == true)
            {
                lval = SPI_CDRAM_CONT | SPI_CDRAM_PCS_DISABLE_ALL | SPI_CDRAM_DT | SPI_CDRAM_DSCK | SPI_CDRAM_BITSE;
            }
            else
            {
                lval = SPI_CDRAM_CONT | SPI_CDRAM_PCS_DISABLE_ALL | SPI_CDRAM_BITSE;
            }
            lval &= ~(1 << hChn->pcs);
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(CDRAM00) + ((j/2) * 4)), lval );
        }

        /* Skip for slower SPI device  */
        if( hChn->lastByteContinueEnable == false)
        {
            /*
             * Last one don't need to assert SS
             */
            if( hChn->useUserDtlAndDsclk == true)
            {
                lval = SPI_CDRAM_PCS_DISABLE_ALL | SPI_CDRAM_DT | SPI_CDRAM_DSCK | SPI_CDRAM_BITSE;
            }
            else
            {
                lval = SPI_CDRAM_PCS_DISABLE_ALL | SPI_CDRAM_BITSE;
            }
            lval &= ~(1 << hChn->pcs);
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(CDRAM00) + (((length/4) - 1) * 4)), lval );
        }

        /* Set queue pointers */
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(NEWQP)), 0 );
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(ENDQP)), (uint32_t)((length/4) - 1) );
    }
    else if (hChn->bitsPerTxfr == 64)
    {
        /*
         * 32-bit transfer mode
         */
        lval = BREG_Read32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR3)));
        lval |= SPI_REG(SPCR3_data_reg_size_MASK);
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR3)), lval);

        lval = BREG_Read32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR0_MSB)));
        lval &= ~SPI_REG(SPCR0_MSB_BitS_MASK);
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR0_MSB)), lval);

        for (i=0, j=0; i<length; i+=8, j+=2)
        {
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(TXRAM00) + (j * 4)),
                        ((uint32_t)(pData[i]) << 24)  | ((uint32_t)(pData[i+1]) << 16) |
                        ((uint32_t)(pData[i+2]) << 8) |  (uint32_t)(pData[i+3]));
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(TXRAM00) + (j * 4) + 4),
                        ((uint32_t)(pData[i+4]) << 24) | ((uint32_t)(pData[i+5]) << 16) |
                        ((uint32_t)(pData[i+6]) << 8)  |  (uint32_t)(pData[i+7]));

            if( hChn->useUserDtlAndDsclk == true)
            {
                lval = SPI_CDRAM_CONT | SPI_CDRAM_PCS_DISABLE_ALL | SPI_CDRAM_DT | SPI_CDRAM_DSCK;
            }
            else
            {
                lval = SPI_CDRAM_CONT | SPI_CDRAM_PCS_DISABLE_ALL;
            }
            lval &= ~(1 << hChn->pcs);
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(CDRAM00) + ((j/2) * 4)), lval );
        }

        /* Skip for slower SPI device  */
        if( hChn->lastByteContinueEnable == false)
        {
            /*
             * Last one don't need to assert SS
             */
            if( hChn->useUserDtlAndDsclk == true)
            {
                lval = SPI_CDRAM_PCS_DISABLE_ALL | SPI_CDRAM_DT | SPI_CDRAM_DSCK;
            }
            else
            {
                lval = SPI_CDRAM_PCS_DISABLE_ALL;
            }
            lval &= ~(1 << hChn->pcs);
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(CDRAM00) + (((length/8) - 1) * 4)), lval );
        }

        /* Set queue pointers */
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(NEWQP)), 0 );
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(ENDQP)), (uint32_t)((length/8) - 1) );
    }
#endif

    if (hChn->assertSSFunc)
    {
        (*(hChn->assertSSFunc)) ();
    }

    /* Start SPI transfer */
    lval = BREG_Read32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR2)));
    lval |= SPI_REG(SPCR2_spe_MASK);
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR2)), lval);

    /* Wait for SPI to finish */
    BSPI_CHK_RETCODE( retCode, BSPI_P_WaitForCompletion(hChn, length));

    if (hChn->deassertSSFunc)
    {
        (*(hChn->deassertSSFunc)) ();
    }

    /* Transfer finished, clear SPIF bit */
    BKNI_EnterCriticalSection();
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(MSPI_STATUS)), 0);
    BKNI_LeaveCriticalSection();

done:

#if BSPI_USE_MAST_N_BOOT || HAS_HIF_MSPI
    if (hChn->chnNo >= MAX_SPI_CHANNELS)                    /* for HIF channels */
    {
        lval = BREG_Read32(hDev->hRegister, BCHP_BSPI_MAST_N_BOOT_CTRL);
        lval &= ~BCHP_BSPI_MAST_N_BOOT_CTRL_mast_n_boot_MASK;   /* give control to BSPI */
        BREG_Write32( hDev->hRegister, BCHP_BSPI_MAST_N_BOOT_CTRL, lval );
    }
#endif
    BSPI_P_RELEASE_UPG_MUTEX( hChn );

    return retCode;
}

void BSPI_P_SetClk(
    BSPI_ChannelHandle  hChn,           /* Device channel handle */
    uint32_t            baud,           /* baud rate */
    uint8_t             clkConfig       /* clock configuration */
    )
{
    BSPI_Handle hDev;
    uint32_t    lval;
    BERR_Code   retCode = BERR_SUCCESS;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hSpi;

    if (baud > MAX_SPI_BAUD)
    {
        retCode = BERR_INVALID_PARAMETER;
        return;
    }

#if defined(BCHP_MSPI_SPCR3)
    lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR3)));
    lval |= 0x3;
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR3)), lval );
#endif

    lval = SPI_SYSTEM_CLK / (2 * baud);
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR0_LSB)), lval );

    lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR0_MSB)));
    lval &= ~(SPI_REG(SPCR0_MSB_CPOL_MASK) | SPI_REG(SPCR0_MSB_CPHA_MASK));

#if defined(BCHP_MSPI_SPCR0_MSB_MSTR_MASK) || defined(BCHP_HIF_MSPI_SPCR0_MSB_MSTR_MASK)
    lval |= (SPI_REG(SPCR0_MSB_MSTR_MASK) | clkConfig);
#else
    lval |= clkConfig;
#endif

    BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR0_MSB)), lval );
}

#if HAS_HIF_MSPI
void BSPI_P_SetEbiCs(
    BSPI_ChannelHandle  hChn,           /* Device channel handle */
    BSPI_EbiCs          ebiCs          /* EBI CS to use */
    )
{
    BSPI_Handle hDev;
    uint32_t    lval;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hSpi;

    if (ebiCs == BSPI_EbiCs_unused)
    {
        return;
    }
    lval = BREG_Read32(hDev->hRegister, BCHP_EBI_CS_SPI_SELECT);
    lval |= (1L << ebiCs);
    BREG_Write32( hDev->hRegister, BCHP_EBI_CS_SPI_SELECT, lval );
}
#endif

static void BSPI_P_EnableInt_isr( BSPI_ChannelHandle  hChn )
{
    uint32_t    lval;
    BSPI_Handle hDev;

    hDev = hChn->hSpi;
    lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR2)));
    lval |= SPI_REG(SPCR2_spifie_MASK);
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR2)), lval );

}

static void BSPI_P_DisableInt_isr( BSPI_ChannelHandle  hChn )
{
    uint32_t    lval;
    BSPI_Handle hDev;

    hDev = hChn->hSpi;
    lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR2)));
    lval &= ~SPI_REG(SPCR2_spifie_MASK);
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR2)), lval );

}

BERR_Code BSPI_P_WaitForCompletion
(
    BSPI_ChannelHandle  hChn,                           /* Device channel handle */
    uint32_t            numBytes                        /* number of bytes to transfer */
)
{
    BSPI_Handle         hDev;
    BERR_Code           retCode = BERR_SUCCESS;
    uint32_t            lval, timeoutMs;

    hDev = hChn->hSpi;

    /* Calculate the timeout value */
    lval = numBytes * 9;                        /* number of clks per byte, assume delay between each byte xfer accounts for 1 extra clock per byte */
    timeoutMs = (lval * 1000) / hChn->baud;     /* get timeout in milliseconds */
    timeoutMs = ((timeoutMs * 110) / 100) + 1;  /* add 10% fudge factor and round up */

    if (hChn->intMode)
    {
        /*
         * Wait for event, set by ISR
         */
        BSPI_CHK_RETCODE (retCode, BKNI_WaitForEvent(hChn->hChnEvent, timeoutMs));
    }
    else
    {
        /*
         * Polling mode
         */
        while (1)
        {
            lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + SPI_REG(MSPI_STATUS)));
            if (lval & SPI_REG(MSPI_STATUS_SPIF_MASK)){
                BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(MSPI_STATUS)), 0);
                break;
            }
        }
    }

done:
    return retCode;
}

void BSPI_P_HandleInterrupt_Isr
(
    void *pParam1,                      /* Device channel handle */
    int parm2                           /* not used */
)
{
    BSPI_ChannelHandle  hChn;
    BSPI_Handle         hDev;
    uint32_t            lval;

    hChn = (BSPI_ChannelHandle) pParam1;
    BDBG_ASSERT( hChn );
    BSTD_UNUSED(parm2);

    hDev = hChn->hSpi;

    /* Clear interrupt */
    lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + SPI_REG(MSPI_STATUS)));
    lval &= ~SPI_REG(MSPI_STATUS_SPIF_MASK);
    BREG_Write32(hDev->hRegister, (hChn->coreOffset + SPI_REG(MSPI_STATUS)), lval);

    BKNI_SetEvent( hChn->hChnEvent );
    return;
}

