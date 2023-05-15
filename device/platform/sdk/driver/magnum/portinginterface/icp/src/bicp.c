/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bicp.c $
 * $brcm_Revision: Hydra_Software_Devel/54 $
 * $brcm_Date: 10/5/12 12:21p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/icp/src/bicp.c $
 * 
 * Hydra_Software_Devel/54   10/5/12 12:21p agin
 * SW7445-52:  add icp pi support.
 * 
 * Hydra_Software_Devel/53   9/26/12 11:37a randyjew
 * SW7563-2: Add 7563 support
 * 
 * Hydra_Software_Devel/52   4/28/12 3:02p chengs
 * SW7584-23: Add 7584 support.
 * 
 * Hydra_Software_Devel/51   4/26/12 4:07p bselva
 * SW7360-17 : Add support for 7360
 * 
 * Hydra_Software_Devel/50   11/21/11 6:19p mward
 * SW7435-7: Add 7435.
 * 
 * Hydra_Software_Devel/49   10/11/11 2:08p agin
 * SW7429-27:  Add support for 7429.
 * 
 * Hydra_Software_Devel/48   1/17/11 11:04a mward
 * SW7344-9: warning: unused variable 'lval'.
 * 
 * Hydra_Software_Devel/47   1/16/11 3:40p agin
 * SW7344-9:  For 7346, need to write to BCHP_IRQ0_AON_IRQEN, instead of
 * BCHP_IRQ0_IRQEN.  However, we don't need to do this in general as it
 * is handled in BINT_EnableCallback.
 * 
 * Hydra_Software_Devel/46   12/27/10 1:56p xhuang
 * SW7358-29: Add 7358/7552 support
 * 
 * Hydra_Software_Devel/45   12/8/10 9:45a jrubio
 * SW7344-9: add 40nm chip support
 * 
 * Hydra_Software_Devel/44   11/2/10 4:55p hongtaoz
 * SW7425-9: added 7425 support;
 * 
 * Hydra_Software_Devel/43   10/7/10 4:38p nickh
 * SW7422-74: Add 7422 support
 * 
 * Hydra_Software_Devel/42   12/13/09 6:39p rpereira
 * SW7550-41: Fixed compilation issues for 7550
 *
 * Hydra_Software_Devel/41   12/4/09 6:54p randyjew
 * SW7468-6: Add 7468 support
 *
 * Hydra_Software_Devel/40   11/6/09 3:57p gmohile
 * SW7408-1 : Add 7408 support
 *
 * Hydra_Software_Devel/39   9/1/09 9:14p rpereira
 * SW7550-30: Adding 7550 support
 *
 * Hydra_Software_Devel/38   8/20/09 4:05p mward
 * PR55545: Support 7125.
 *
 * Hydra_Software_Devel/37   8/10/09 4:24p jrubio
 * PR55232: add 7340/7342
 *
 * Hydra_Software_Devel/36   3/6/09 2:06p jkim
 * PR52508: remove global variable and move it inside the handle
 *
 * Hydra_Software_Devel/35   1/31/09 1:11a jrubio
 * PR51629:add 7336 support
 *
 * Hydra_Software_Devel/34   12/2/08 4:21p kaushikb
 * PR49867: Adding support for 7420
 *
 * Hydra_Software_Devel/33   6/9/08 10:34a jkim
 * PR42188: Close the channel only if channel is not closed yet during
 * BICP_Close
 *
 * Hydra_Software_Devel/32   4/4/08 11:03a farshidf
 * PR39179: Add 3548/3556 support
 *
 * Hydra_Software_Devel/31   11/27/07 6:02p farshidf
 * PR36887: Add 7335 support
 *
 * Hydra_Software_Devel/30   10/14/07 3:52p jkim
 * PR14344: add 7325 support
 *
 * Hydra_Software_Devel/29   5/18/07 10:59a jkim
 * PR14344: Add 7405 support
 *
 * Hydra_Software_Devel/28   2/16/07 11:36a jkim
 * PR14344: Added 7440 support
 *
 * Hydra_Software_Devel/27   2/2/07 11:22a jkim
 * PR27238: Modify to use the correct IRQ definition
 *
 * Hydra_Software_Devel/26   1/12/07 4:22p jkim
 * PR14344: Add 3563 support
 *
 * Hydra_Software_Devel/25   11/9/06 11:04a jkim
 * PR14344: added 7403 support
 *
 * Hydra_Software_Devel/24   9/19/06 5:20p agin
 * PR24339: Resolve compiler warning for DEBUG=n builds for UPG modules.
 *
 * Hydra_Software_Devel/23   8/9/06 11:00a agin
 * PR23362: Add 3563 support.
 *
 * Hydra_Software_Devel/22   7/25/06 2:45p ltokuda
 * PR21692: Add BCM7118 support.
 *
 * Hydra_Software_Devel/21   3/21/06 3:03p jkim
 * PR20326: Add support for 7438
 *
 * Hydra_Software_Devel/20   1/14/06 11:00p agin
 * PR19076: Support BCM7400.
 *
 * Hydra_Software_Devel/19   1/3/06 12:10p agin
 * PR18689: Infinite recursive call in BICP_DisableInt (bicp.c)
 *
 * Hydra_Software_Devel/18   9/8/05 10:58a brianlee
 * PR16410: Make rcPinMask a variable that supports multiple ICAP pins.
 *
 * Hydra_Software_Devel/17   8/23/05 5:09p jkim
 * PR14344: Adding 7401 support
 *
 * Hydra_Software_Devel/16   7/26/05 6:33p brianlee
 * PR16410: Fixed RC6 remote missing key detection.
 *
 * Hydra_Software_Devel/15   4/7/05 10:44a agin
 * PR14698: unused parameters.
 *
 * Hydra_Software_Devel/14   3/10/05 8:51a dlwin
 * PR 14240: Added support for 3560
 *
 * Hydra_Software_Devel/13   3/7/05 9:06a dlwin
 * PR 13212: Replaced C++ comment with C version
 *
 * Hydra_Software_Devel/12   2/24/05 5:27p agin
 * PR13212:  Fixed RC6 state machine for when receiving a bad waveform.
 *
 * Hydra_Software_Devel/11   2/4/05 11:18a agin
 * PR13212:  added callback for RC6 processing.  Improved ISR.
 *
 * Hydra_Software_Devel/10   2/3/05 11:36a agin
 * PR13212: RC6 support
 *
 * Hydra_Software_Devel/9   2/1/05 7:05p agin
 * PR13987:  don't use globals in portinginterface.
 *
 * Hydra_Software_Devel/8   1/31/05 5:56p agin
 * PR13987:  interrupt frequency count for icap.
 *
 * Hydra_Software_Devel/7   3/26/04 4:32p brianlee
 * PR8971: Remove BDBG_ASSERT() for malloc failure.
 *
 * Hydra_Software_Devel/6   12/29/03 3:59p marcusk
 * PR9117: Updated with changes required to support interrupt ids rather
 * than strings.
 *
 * Hydra_Software_Devel/5   11/4/03 6:50p brianlee
 * Get rid of enter/leave macros.
 *
 * Hydra_Software_Devel/4   10/24/03 4:34p brianlee
 * Remove define for max icap channel.  This has been moved to bicp.h.
 *
 * Hydra_Software_Devel/3   9/24/03 11:57a brianlee
 * Changed the names of header files.
 *
 * Hydra_Software_Devel/2   9/19/03 12:27p brianlee
 * Fixed warnings from Midas build.
 *
 * Hydra_Software_Devel/1   9/16/03 7:03p brianlee
 * Initial version.
 *
 ***************************************************************************/
#include "bstd.h"
#include "bicp.h"
#include "bicp_priv.h"
#include "bchp_icap.h"
#include "bchp_irq0.h"
#include "bchp_int_id_irq0.h"
#if (BCHP_CHIP==7231) || (BCHP_CHIP==7344) || (BCHP_CHIP==7346) || (BCHP_CHIP==7358) || \
    (BCHP_CHIP==7360) || (BCHP_CHIP==7422) || (BCHP_CHIP==7425) || (BCHP_CHIP==7429) || \
    (BCHP_CHIP==7435) || (BCHP_CHIP==7445) || (BCHP_CHIP==7552) || (BCHP_CHIP==7584) || \
    (BCHP_CHIP==7563)
#include "bchp_int_id_irq0_aon.h"
#include "bchp_irq0_aon.h"
#endif

BDBG_MODULE(bicp);

#define DEV_MAGIC_ID            ((BERR_ICP_ID<<16) | 0xFACE)

#define BICP_CHK_RETCODE( rc, func )        \
do {                                        \
    if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
    {                                       \
        goto done;                          \
    }                                       \
} while(0)

#define MAX_ICAP_RCNT               15

/*******************************************************************************
*
*   Private Module Handles
*
*******************************************************************************/

typedef struct BICP_P_Handle
{
    uint32_t        magicId;                    /* Used to check if structure is corrupt */
    BCHP_Handle     hChip;
    BREG_Handle     hRegister;
    BINT_Handle     hInterrupt;
    unsigned int    maxChnNo;
    unsigned int    uiRCPinMask;
    BICP_ChannelHandle hIcpChn[MAX_ICP_CHANNELS];
} BICP_P_Handle;

typedef struct BICP_P_RC6_KEYBIT {
    unsigned char ve;       /* 1: ve+, 0: ve */
    unsigned char used;     /* counted or not */
    unsigned char len;      /* in T unit */
} BICP_P_RC6_KEYBIT;

typedef struct BICP_P_RC6
{
    uint8_t lead;
    uint8_t start;
    uint8_t trailer;
    uint8_t mode;
    uint8_t modebits;
    uint8_t ctrl;
    uint16_t ctrlbits;
    uint8_t total_ctrl_bits;
    uint8_t info;
    uint16_t data;
    uint8_t edge;   /*0: 1->0, 1: 0->1 */
    uint16_t last;
    int                 keycnt;
    BICP_P_RC6_KEYBIT   keybits[314];
} BICP_P_RC6;

typedef struct BICP_P_ChannelHandle
{
    uint32_t            magicId;                    /* Used to check if structure is corrupt */
    BICP_Handle         hIcp;
    uint32_t            chnNo;
    uint32_t            isrCount;
    unsigned int        handleRC6;
    BICP_P_RC6          rc6;
    BKNI_EventHandle    hChnEvent;
    BINT_CallbackHandle hChnCallback;
    BICP_Callback       pInterruptEventUserCallback;
} BICP_P_ChannelHandle;

#if 0
/* move inside handle */
static unsigned int uiRCPinMask = 0;
#endif

#define RCCOUNTER   (1<<16)
#define TUNIT       (27000*16/36)
#define TUNIT14     (TUNIT>>2)
#define TUNIT34     ((TUNIT*3)>>2)
#define INFOBITS    12      /* only support 12 bits info now */

/*******************************************************************************
*
*   Default Module Settings
*
*******************************************************************************/
static const BICP_Settings defIcpSettings = NULL;

static const BICP_ChannelSettings defIcpChn0Settings =
{
    MAX_ICAP_RCNT,
    true
};

static const BICP_ChannelSettings defIcpChn1Settings =
{
    MAX_ICAP_RCNT,
    true
};

static const BICP_ChannelSettings defIcpChn2Settings =
{
    MAX_ICAP_RCNT,
    true
};

static const BICP_ChannelSettings defIcpChn3Settings =
{
    MAX_ICAP_RCNT,
    true
};

/*******************************************************************************
*
*   Public Module Functions
*
*******************************************************************************/
BERR_Code BICP_Open(
    BICP_Handle *pIcp,                  /* [output] Returns handle */
    BCHP_Handle hChip,                  /* Chip handle */
    BREG_Handle hRegister,              /* Register handle */
    BINT_Handle hInterrupt,             /* Interrupt handle */
    const BICP_Settings *pDefSettings   /* Default settings */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BICP_Handle hDev;
    unsigned int chnIdx;
    BSTD_UNUSED(pDefSettings);

    /* Sanity check on the handles we've been given. */
    BDBG_ASSERT( hChip );
    BDBG_ASSERT( hRegister );
    BDBG_ASSERT( hInterrupt );

    /* Alloc memory from the system heap */
    hDev = (BICP_Handle) BKNI_Malloc( sizeof( BICP_P_Handle ) );
    if( hDev == NULL )
    {
        *pIcp = NULL;
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BICP_Open: BKNI_malloc() failed\n"));
        goto done;
    }

    hDev->magicId   = DEV_MAGIC_ID;
    hDev->hChip     = hChip;
    hDev->hRegister = hRegister;
    hDev->hInterrupt = hInterrupt;
    hDev->maxChnNo  = MAX_ICP_CHANNELS;
    hDev->uiRCPinMask = 0;
    for( chnIdx = 0; chnIdx < hDev->maxChnNo; chnIdx++ )
    {
        hDev->hIcpChn[chnIdx] = NULL;
    }


    *pIcp = hDev;

done:
    return( retCode );
}

BERR_Code BICP_Close(
    BICP_Handle hDev                    /* Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    unsigned int chnIdx;


    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    /* If channel is not closed, close the channel to free up the memory */
    for( chnIdx = 0; chnIdx < hDev->maxChnNo; chnIdx++ )
    {
        if (hDev->hIcpChn[chnIdx])
        {
            BICP_CloseChannel(hDev->hIcpChn[chnIdx]);
        }
    }

    BKNI_Free( (void *) hDev );

    return( retCode );
}

BERR_Code BICP_GetDefaultSettings(
    BICP_Settings *pDefSettings,        /* [output] Returns default setting */
    BCHP_Handle hChip                   /* Chip handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BSTD_UNUSED(hChip);

    *pDefSettings = defIcpSettings;

    return( retCode );
}

BERR_Code BICP_GetTotalChannels(
    BICP_Handle hDev,                   /* Device handle */
    unsigned int *totalChannels         /* [output] Returns total number downstream channels supported */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    *totalChannels = hDev->maxChnNo;

    return( retCode );
}

BERR_Code BICP_GetChannelDefaultSettings(
    BICP_Handle hDev,                   /* Device handle */
    unsigned int channelNo,             /* Channel number to default setting for */
    BICP_ChannelSettings *pChnDefSettings /* [output] Returns channel default setting */
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
            *pChnDefSettings = defIcpChn0Settings;
            break;

        case 1:
            *pChnDefSettings = defIcpChn1Settings;
            break;

        case 2:
            *pChnDefSettings = defIcpChn2Settings;
            break;

        case 3:
            *pChnDefSettings = defIcpChn3Settings;
            break;

        default:
            retCode = BERR_INVALID_PARAMETER;
            break;

    }

    return( retCode );
}

BERR_Code BICP_OpenChannel(
    BICP_Handle hDev,                   /* Device handle */
    BICP_ChannelHandle *phChn,          /* [output] Returns channel handle */
    unsigned int channelNo,             /* Channel number to open */
    const BICP_ChannelSettings *pChnDefSettings /* Channel default setting */
    )
{
    BERR_Code           retCode = BERR_SUCCESS;
    BICP_ChannelHandle  hChnDev;

    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    hChnDev = NULL;

    if( channelNo < hDev->maxChnNo )
    {
        if( hDev->hIcpChn[channelNo] == NULL )
        {
            /* Alloc memory from the system heap */
            hChnDev = (BICP_ChannelHandle) BKNI_Malloc( sizeof( BICP_P_ChannelHandle ) );
            if( hChnDev == NULL )
            {
                *phChn = NULL;
                retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
                BDBG_ERR(("BICP_OpenChannel: BKNI_malloc() failed\n"));
                goto done;
            }

            BICP_CHK_RETCODE( retCode, BKNI_CreateEvent( &(hChnDev->hChnEvent) ) );
            hChnDev->magicId    = DEV_MAGIC_ID;
            hChnDev->hIcp       = hDev;
            hChnDev->chnNo      = channelNo;
            hChnDev->isrCount   = 0;
            hChnDev->rc6.lead   = 0;
            hChnDev->rc6.start  = 0;
            hChnDev->rc6.trailer    = 0;
            hChnDev->rc6.mode   = 0;
            hChnDev->rc6.modebits   = 0;
            hChnDev->rc6.ctrl   = 0;
            hChnDev->rc6.ctrlbits   = 0;
            hChnDev->rc6.total_ctrl_bits    = 8;
            hChnDev->rc6.info   = 0;
            hChnDev->rc6.data   = 0;
            hChnDev->rc6.edge   = 0;
            hChnDev->rc6.last   = 0;
            hChnDev->rc6.keycnt = 0;
            hChnDev->pInterruptEventUserCallback = NULL;
            hDev->hIcpChn[channelNo] = hChnDev;

            /* Program glitch rejector count */
            BICP_CHK_RETCODE( retCode, BICP_P_SetRejectCnt (hChnDev, pChnDefSettings->rejectCnt) );

            /*
             * Enable interrupt for this channel
             */
            if (pChnDefSettings->intMode == true)
            {
                /*
                 * Register and enable L2 interrupt.
                 */
                #if defined(BCHP_INT_ID_icap_irqen)
                    BICP_CHK_RETCODE( retCode, BINT_CreateCallback(
                        &(hChnDev->hChnCallback), hDev->hInterrupt, BCHP_INT_ID_icap_irqen,
                        BICP_P_HandleInterrupt_Isr, (void *) hChnDev, (1 << channelNo) ) );
                #elif defined(BCHP_INT_ID_icap)
                    BICP_CHK_RETCODE( retCode, BINT_CreateCallback(
                        &(hChnDev->hChnCallback), hDev->hInterrupt, BCHP_INT_ID_icap,
                        BICP_P_HandleInterrupt_Isr, (void *) hChnDev, (1 << channelNo) ) );
                #else
                    #error BCHP_CHIP set to unsupport chip.
                #endif
                BICP_CHK_RETCODE( retCode, BINT_EnableCallback( hChnDev->hChnCallback ) );

                /*
                 * Enable ICP interrupt in ICP
                 */
                BKNI_EnterCriticalSection();
                BICP_P_EnableInt (hChnDev);
                BKNI_LeaveCriticalSection();
            }

            *phChn = hChnDev;
        }
        else
        {
            retCode = BICP_ERR_NOTAVAIL_CHN_NO;
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
            hDev->hIcpChn[channelNo] = NULL;
            *phChn = NULL;
        }
    }
    return( retCode );
}

BERR_Code BICP_CloseChannel(
    BICP_ChannelHandle hChn         /* Device channel handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BICP_Handle hDev;
    unsigned int chnNo;


    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hIcp;

    /* Reset uiRCPinMask if channel is RC6 */
    if (hChn->handleRC6)
    {
        hDev->uiRCPinMask &= ~(1<<hChn->chnNo);
    }

    /*
     * Disable interrupt for this channel
     */
    BKNI_EnterCriticalSection();
    BICP_P_DisableInt (hChn);
    BKNI_LeaveCriticalSection();

    BICP_CHK_RETCODE( retCode, BINT_DisableCallback( hChn->hChnCallback ) );
    BICP_CHK_RETCODE( retCode, BINT_DestroyCallback( hChn->hChnCallback ) );
    BKNI_DestroyEvent( hChn->hChnEvent );
    chnNo = hChn->chnNo;
    BKNI_Free( hChn );
    hDev->hIcpChn[chnNo] = NULL;

done:
    return( retCode );
}

BERR_Code BICP_GetDevice(
    BICP_ChannelHandle hChn,            /* Device channel handle */
    BICP_Handle *phDev                  /* [output] Returns Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    *phDev = hChn->hIcp;

    return( retCode );
}


BERR_Code BICP_GetEventHandle(
    BICP_ChannelHandle hChn,            /* Device channel handle */
    BKNI_EventHandle *phEvent           /* [output] Returns event handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );


    *phEvent = hChn->hChnEvent;
    return( retCode );
}

BERR_Code BICP_EnableEdge(
    BICP_ChannelHandle  hChn,           /* Device channel handle */
    BICP_EdgeConfig     edge            /* edge config */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    uint32_t lval, mask;
    BICP_Handle hDev;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hIcp;

    lval = BREG_Read32(hDev->hRegister, BCHP_ICAP_INEDGE);
    mask = 1 << hChn->chnNo;
    if (edge == BICP_EdgeConfig_ePositive || edge == BICP_EdgeConfig_eBoth)
        lval |= (mask << BCHP_ICAP_INEDGE_icap_pedgedet_SHIFT);
    if (edge == BICP_EdgeConfig_eNegative || edge == BICP_EdgeConfig_eBoth)
        lval |= mask;

    BREG_Write32(hDev->hRegister, BCHP_ICAP_INEDGE, lval);

    return( retCode );
}

BERR_Code BICP_DisableEdge(
    BICP_ChannelHandle  hChn,           /* Device channel handle */
    BICP_EdgeConfig     edge            /* edge config */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    uint32_t lval, mask;
    BICP_Handle hDev;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hIcp;

    lval = BREG_Read32(hDev->hRegister, BCHP_ICAP_INEDGE);
    mask = 1 << hChn->chnNo;
    if (edge == BICP_EdgeConfig_ePositive || edge == BICP_EdgeConfig_eBoth)
        lval &= ~(mask << BCHP_ICAP_INEDGE_icap_pedgedet_SHIFT);
    if (edge == BICP_EdgeConfig_eNegative || edge == BICP_EdgeConfig_eBoth)
        lval &= ~mask;
    BREG_Write32(hDev->hRegister, BCHP_ICAP_INEDGE, lval);

    return( retCode );
}

BERR_Code BICP_GetTimerCnt(
    BICP_ChannelHandle  hChn,           /* Device channel handle */
    uint16_t            *timerCnt       /* pointer to count */
    )
{
    BERR_Code       retCode = BERR_SUCCESS;
    uint32_t        msb = 0, lsb = 0;
    BICP_Handle     hDev;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hIcp;

    switch (hChn->chnNo)
    {
        case 0:
            msb = BREG_Read32(hDev->hRegister, BCHP_ICAP_TCNT0MSB);
            lsb = BREG_Read32(hDev->hRegister, BCHP_ICAP_TCNT0LSB);
            break;

        case 1:
            msb = BREG_Read32(hDev->hRegister, BCHP_ICAP_TCNT1MSB);
            lsb = BREG_Read32(hDev->hRegister, BCHP_ICAP_TCNT1LSB);
            break;

        case 2:
            msb = BREG_Read32(hDev->hRegister, BCHP_ICAP_TCNT2MSB);
            lsb = BREG_Read32(hDev->hRegister, BCHP_ICAP_TCNT2LSB);
            break;

        case 3:
            msb = BREG_Read32(hDev->hRegister, BCHP_ICAP_TCNT3MSB);
            lsb = BREG_Read32(hDev->hRegister, BCHP_ICAP_TCNT3LSB);
            break;

    }
    *timerCnt = ((uint16_t)msb << 8) | ((uint16_t)lsb);

    return( retCode );
}

BERR_Code BICP_PollTimer(
    BICP_ChannelHandle  hChn,           /* Device channel handle */
    bool                *triggered,     /* status of trigger */
    uint16_t            *timerCnt       /* pointer to count */
    )
{
    BERR_Code       retCode = BERR_SUCCESS;
    uint32_t        mask, lval;
    BICP_Handle     hDev;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hIcp;
    lval = BREG_Read32(hDev->hRegister, BCHP_ICAP_INSTATUS);
    mask = 1 << hChn->chnNo;
    *triggered =  (lval & mask) ? true : false;
    if (*triggered)
    {
        /* Reset interrupt pins */
        BREG_Write32(hDev->hRegister, BCHP_ICAP_RST, mask);
        BREG_Write32(hDev->hRegister, BCHP_ICAP_RST, 0);

        /* Get the count for caller */
        BICP_CHK_RETCODE (retCode, BICP_GetTimerCnt (hChn, timerCnt) );
    }

done:
    return( retCode );
}

void BICP_EnableInt(
    BICP_ChannelHandle  hChn            /* Device channel handle */
    )
{
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );
    BICP_P_EnableInt( hChn );
}

void BICP_DisableInt(
    BICP_ChannelHandle  hChn            /* Device channel handle */
    )
{
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );
    BICP_P_DisableInt( hChn );
}

void BICP_EnableRC6(
    BICP_ChannelHandle  hChn,           /* Device channel handle */
    BICP_Callback pCallback             /* Pointer to completion callback. */
    )
{
    BICP_Handle hDev;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );
    hChn->handleRC6 = 1;
    hChn->pInterruptEventUserCallback = pCallback;
    hDev = hChn->hIcp;
    hDev->uiRCPinMask |= (1<<hChn->chnNo);
}

void BICP_DisableRC6(
    BICP_ChannelHandle  hChn            /* Device channel handle */
    )
{
    BICP_Handle hDev;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );
    hChn->handleRC6 = 0;

    hDev = hChn->hIcp;
    hDev->uiRCPinMask &= ~(1<<hChn->chnNo);
}

void BICP_ResetIntCount(
    BICP_ChannelHandle  hChn            /* Device channel handle */
    )
{
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );
    hChn->isrCount = 0;
}

void BICP_GetIntCount(
    BICP_ChannelHandle  hChn,           /* Device channel handle */
    uint32_t            *data
    )
{
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );
    *data = hChn->isrCount;
}

/*******************************************************************************
*
*   Private Module Functions
*
*******************************************************************************/
BERR_Code BICP_P_SetRejectCnt(
    BICP_ChannelHandle  hChn,           /* Device channel handle */
    uint8_t             clks            /* number of clocks to reject */
    )
{
    uint32_t    lval;
    BICP_Handle hDev;
    BERR_Code   retCode = BERR_SUCCESS;

    hDev = hChn->hIcp;

    lval = clks - 1;
    if (lval > MAX_ICAP_RCNT)
    {
        retCode = BERR_INVALID_PARAMETER;
        goto done;
    }

    switch (hChn->chnNo)
    {
        case 0:
            BREG_Write32(hDev->hRegister, BCHP_ICAP_RCNT0, lval);
            break;

        case 1:
            BREG_Write32(hDev->hRegister, BCHP_ICAP_RCNT1, lval);
            break;

        case 2:
            BREG_Write32(hDev->hRegister, BCHP_ICAP_RCNT2, lval);
            break;

        case 3:
            BREG_Write32(hDev->hRegister, BCHP_ICAP_RCNT3, lval);
            break;

        default:
            retCode = BERR_INVALID_PARAMETER;
            goto done;
    }

done:
    return retCode;
}

void BICP_P_EnableInt(
    BICP_ChannelHandle  hChn            /* Device channel handle */
    )
{
    uint32_t    lval;
    BICP_Handle hDev;

    hDev = hChn->hIcp;

    lval = BREG_Read32(hDev->hRegister, BCHP_ICAP_MASK);
    lval |= (1 << hChn->chnNo);
    BREG_Write32(hDev->hRegister, BCHP_ICAP_MASK, lval);

}

void BICP_P_DisableInt(
    BICP_ChannelHandle  hChn            /* Device channel handle */
    )
{
    uint32_t    lval;
    BICP_Handle hDev;

    hDev = hChn->hIcp;

    lval = BREG_Read32(hDev->hRegister, BCHP_ICAP_MASK);
    lval &= ~(1 << hChn->chnNo);
    BREG_Write32(hDev->hRegister, BCHP_ICAP_MASK, lval);

}

void BICP_P_RC6Handle(BICP_ChannelHandle hIcpChan, uint8_t reg)
{
    BICP_Handle hDev;
    uint16_t cur=0;
    uint32_t len, len1, adj_count;
    uint8_t ve1, ve2;
    uint8_t used1, used2;
    uint8_t found_key = 0;

/* T = 1/36KHZ * 16 = 444.44us, 7115 counter is 27MHz, so T = 12000 ticks
   1bit data will use 2T with mancherster coding.
   RC6 data sequence is:
   Leader: 8T (6T ve+, 2T ve-)
   Start : 2T (ve+, ve-)
   Mode  : 6T (3 bits)
   Trailer:4T (2T ve+, 2T ve-)
   Control:16T/32T(8bits/16bits, depends on the first bit, 0: 8, 1: 16)
   Info  : 16T-256T(8bits-128bits)
*/

    BDBG_ASSERT( hIcpChan );
    hDev = hIcpChan->hIcp;

    if (reg & hDev->uiRCPinMask){
        BICP_GetTimerCnt(hIcpChan, &cur);

        hIcpChan->rc6.keybits[hIcpChan->rc6.keycnt].ve = 1-hIcpChan->rc6.edge;  /* edge 0: 1->0, ve 1: ve+ */
        hIcpChan->rc6.keybits[hIcpChan->rc6.keycnt].used = 0;
        if (hIcpChan->rc6.keycnt){  /* now we can tell last ve len */
            /* counter is 16 bits, there is chance of wrap around */
            if (cur > hIcpChan->rc6.last)
                len = cur - hIcpChan->rc6.last;
            else{
                len = RCCOUNTER - hIcpChan->rc6.last + cur;
            }

            if ((hIcpChan->rc6.keycnt == 1) && (len < 9800 ))/*Fudge delta after a key is found */
                len += RCCOUNTER;

            if (len < TUNIT34)              /*I bet it is leader bit 6T, wrap around */
                len += RCCOUNTER;

            adj_count = len;
            len = adj_count / TUNIT;
            if ((adj_count % TUNIT) > (TUNIT/2))
                len++;                      /* round up */

            /*  Check for invalid lengths and cross check len with state;  */
            if ((len == 5) || ((len == 6) && (hIcpChan->rc6.lead) )) {
                hIcpChan->rc6.keycnt=0;     /* Start over */
                hIcpChan->rc6.edge = 0;
                hIcpChan->rc6.last = cur;

                hIcpChan->rc6.lead = hIcpChan->rc6.start = hIcpChan->rc6.mode = hIcpChan->rc6.trailer = 0;
                hIcpChan->rc6.ctrl = hIcpChan->rc6.info = 0;
                hIcpChan->rc6.ctrlbits = hIcpChan->rc6.modebits = 0;
                return;
            }




            if ((len == 6) && hIcpChan->rc6.keybits[hIcpChan->rc6.keycnt-1].ve)     /* assume found lead bit */
            {
                hIcpChan->rc6.keybits[0].ve = hIcpChan->rc6.keybits[hIcpChan->rc6.keycnt-1].ve;
                hIcpChan->rc6.keybits[0].used = hIcpChan->rc6.keybits[hIcpChan->rc6.keycnt-1].used;
                hIcpChan->rc6.keycnt = 1;
            }

            hIcpChan->rc6.keybits[hIcpChan->rc6.keycnt-1].len = len;

            if (hIcpChan->rc6.keycnt > 1){
                ve1 = hIcpChan->rc6.keybits[hIcpChan->rc6.keycnt-2].ve;
                len1 = hIcpChan->rc6.keybits[hIcpChan->rc6.keycnt-2].len;
                used1 = hIcpChan->rc6.keybits[hIcpChan->rc6.keycnt-2].used;
                used2 = hIcpChan->rc6.keybits[hIcpChan->rc6.keycnt-1].used;
                ve2 = hIcpChan->rc6.keybits[hIcpChan->rc6.keycnt-1].ve;

                if (!used1 && !used2){  /* make sure 2 states captured */

                    if (!hIcpChan->rc6.lead){
                        if ((len1 == 6) && ve1 && (len>1) && !ve2){ /* found lead bit */
                            hIcpChan->rc6.lead = 1;
                            hIcpChan->rc6.keybits[hIcpChan->rc6.keycnt-2].used = 1;
                            hIcpChan->rc6.keybits[hIcpChan->rc6.keycnt-1].used = 1;
                        }
                    }
                    else if (!hIcpChan->rc6.start){
                        if ((len1 == 1) && ve1 && !ve2){
                            hIcpChan->rc6.start = 1;
                            hIcpChan->rc6.keybits[hIcpChan->rc6.keycnt-2].used = 1;
                            if (len==1)             /* next dat is same as current one */
                                hIcpChan->rc6.keybits[hIcpChan->rc6.keycnt-1].used = 1;
                        }
                    }
                    else if (hIcpChan->rc6.mode != 3){
                        hIcpChan->rc6.modebits <<= 1;
                        if (ve1 && !ve2)
                            hIcpChan->rc6.modebits |= 1;
                        hIcpChan->rc6.keybits[hIcpChan->rc6.keycnt-2].used = 1;
                        if (len==1)             /* next dat is same as current one */
                            hIcpChan->rc6.keybits[hIcpChan->rc6.keycnt-1].used = 1;
                        hIcpChan->rc6.mode=hIcpChan->rc6.mode+1;
                    }
                    else if (!hIcpChan->rc6.trailer){
                        if ((len1>=2) && (len>=2)){
                            hIcpChan->rc6.trailer = 1;
                            hIcpChan->rc6.keybits[hIcpChan->rc6.keycnt-2].used = 1;
                            if (len == 2)   /* next dat is same as current one */
                                hIcpChan->rc6.keybits[hIcpChan->rc6.keycnt-1].used = 1;
                        }
                    }
                    else if(hIcpChan->rc6.ctrl != hIcpChan->rc6.total_ctrl_bits){
                        hIcpChan->rc6.ctrlbits <<= 1;
                        if (ve1 && !ve2)
                            hIcpChan->rc6.ctrlbits |= 1;

                        hIcpChan->rc6.keybits[hIcpChan->rc6.keycnt-2].used = 1;
                        if (len==1)             /* next dat is same as current one */
                            hIcpChan->rc6.keybits[hIcpChan->rc6.keycnt-1].used = 1;

                        if (!hIcpChan->rc6.ctrl){
                            if (hIcpChan->rc6.ctrlbits)
                                hIcpChan->rc6.total_ctrl_bits = 16;
                            else
                                hIcpChan->rc6.total_ctrl_bits = 8;
                        }

                        hIcpChan->rc6.ctrl++;
                    }
                    else if( hIcpChan->rc6.lead && hIcpChan->rc6.start && hIcpChan->rc6.mode && hIcpChan->rc6.trailer && hIcpChan->rc6.ctrl){   /* it is data now */
                        hIcpChan->rc6.data <<= 1;
                        if (ve1 && !ve2){   /* it is 1 */
                            hIcpChan->rc6.data |= 1;
                        }
                        if (!ve1 && ve2){   /* it is 0 */
                        }
                        hIcpChan->rc6.keybits[hIcpChan->rc6.keycnt-2].used = 1;
                        if (len==1)             /* next dat is same as current one */
                            hIcpChan->rc6.keybits[hIcpChan->rc6.keycnt-1].used = 1;
                        hIcpChan->rc6.info++;
                        if (hIcpChan->rc6.info == INFOBITS){    /* got data */
                            found_key = 1;
                        }
                    }
                    else{
                        BDBG_ERR(("RC6 key something wrong, redo\n"));
                        hIcpChan->rc6.lead = hIcpChan->rc6.start = hIcpChan->rc6.mode = hIcpChan->rc6.trailer = hIcpChan->rc6.ctrl = hIcpChan->rc6.info = 0;
                        hIcpChan->rc6.keycnt = hIcpChan->rc6.ctrlbits = hIcpChan->rc6.modebits = 0;
                        hIcpChan->rc6.edge = 0;
                    }
                }

                /* for last bit == 1, it is ve+ and ve- */
                if ((hIcpChan->rc6.info == INFOBITS-1) && !hIcpChan->rc6.keybits[hIcpChan->rc6.keycnt-1].used){     /* this is fake */
                    if (ve2){
                        hIcpChan->rc6.data <<= 1;
                        hIcpChan->rc6.data |= 1;
                        hIcpChan->rc6.keybits[hIcpChan->rc6.keycnt-1].used = 1;
                        hIcpChan->rc6.keybits[hIcpChan->rc6.keycnt].used = 1;
                        found_key = 1;
                    }
                }

            }
        }
        hIcpChan->rc6.keycnt++;
        if ((hIcpChan->rc6.keycnt>4) && !hIcpChan->rc6.lead)
        {
            hIcpChan->rc6.keycnt=0;     /* Start over */
            hIcpChan->rc6.edge = 0;
            hIcpChan->rc6.last = 0;

            hIcpChan->rc6.lead = hIcpChan->rc6.start = hIcpChan->rc6.mode = hIcpChan->rc6.trailer = 0;
            hIcpChan->rc6.ctrl = hIcpChan->rc6.info = 0;
            hIcpChan->rc6.ctrlbits = hIcpChan->rc6.modebits = 0;
        }
        else
        {
            hIcpChan->rc6.edge = 1-hIcpChan->rc6.edge;
            hIcpChan->rc6.last = cur;
        }
    }
    if (found_key){
        /* Invoke callback */
        if( hIcpChan->pInterruptEventUserCallback != NULL )
        {
            hIcpChan->pInterruptEventUserCallback( hIcpChan->rc6.ctrlbits, hIcpChan->rc6.modebits, hIcpChan->rc6.data );
        }
        hIcpChan->rc6.lead = hIcpChan->rc6.start = hIcpChan->rc6.mode = hIcpChan->rc6.trailer = hIcpChan->rc6.ctrl = hIcpChan->rc6.info = 0;
        hIcpChan->rc6.keycnt = hIcpChan->rc6.ctrlbits = hIcpChan->rc6.modebits = 0;
        hIcpChan->rc6.edge = 0;
        hIcpChan->rc6.data = 0;
        found_key = 0;
    }
}

static void BICP_P_HandleInterrupt_Isr
(
    void *pParam1,                      /* Device channel handle */
    int parm2                           /* not used */
)
{
    BICP_ChannelHandle  hChn;
    BICP_Handle         hDev;
    uint32_t            lval;
    uint32_t            mask;

    hChn = (BICP_ChannelHandle) pParam1;
    BDBG_ASSERT( hChn );

    hDev = hChn->hIcp;

    /* Read interrupt status register */
    lval = BREG_Read32(hDev->hRegister, BCHP_ICAP_INSTATUS);

    /* Is this interrupt for this channel? */
    mask = lval & parm2;

    if (!mask)
        return;

    hChn->isrCount++;

    /* Reset interrupt pins */
    BREG_Write32(hDev->hRegister, BCHP_ICAP_RST, mask);
    BREG_Write32(hDev->hRegister, BCHP_ICAP_RST, 0);

    if (hChn->handleRC6)
    {
        BICP_P_RC6Handle(hChn, (unsigned char)(lval & 0xf));
    }

    BKNI_SetEvent( hChn->hChnEvent );
    return;
}

