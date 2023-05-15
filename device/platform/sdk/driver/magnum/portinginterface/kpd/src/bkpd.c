/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bkpd.c $
 * $brcm_Revision: Hydra_Software_Devel/32 $
 * $brcm_Date: 10/5/12 11:35a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/kpd/src/bkpd.c $
 * 
 * Hydra_Software_Devel/32   10/5/12 11:35a agin
 * SW7445-30:  Fix compiler error for 7445.
 * 
 * Hydra_Software_Devel/31   9/24/12 5:06p randyjew
 * SW7563-2: Add 7563 Support
 * 
 * Hydra_Software_Devel/30   4/28/12 3:36p chengs
 * SW7584-23: Add 7584 support.
 * 
 * Hydra_Software_Devel/29   4/26/12 5:03p bselva
 * SW7360-17: Add support for 7360
 * 
 * Hydra_Software_Devel/28   4/24/12 3:56p erickson
 * SW7346-790: clean up isr content handling
 * 
 * Hydra_Software_Devel/27   4/24/12 3:50p erickson
 * SW7346-790: add keyMask setting
 *
 * Hydra_Software_Devel/26   11/21/11 6:20p mward
 * SW7435-7: Add 7435.
 *
 * Hydra_Software_Devel/25   10/11/11 11:24a katrep
 * SW7429-1:add 7429 support
 *
 * Hydra_Software_Devel/24   2/1/11 7:12a agin
 * SW7335-884:  Need to enable irqen bit in the BCHP_LDK_CONTROL register
 * regardless of using interrupt mode or polling mode.
 *
 * Hydra_Software_Devel/23   1/14/11 8:04p brianlee
 * SW7422-197: Make sure to zero out upper bits of LDK_CONTROL during
 * initialization.
 *
 * Hydra_Software_Devel/22   12/27/10 2:48p xhuang
 * SW7358-29: Add 7358/7552 support
 *
 * Hydra_Software_Devel/21   12/8/10 5:58p katrep
 * SW7231-4:add support for 7231
 *
 * Hydra_Software_Devel/20   12/7/10 6:16p jrubio
 * SW7344-9: add 7344/7346 support
 *
 * Hydra_Software_Devel/19   11/2/10 5:06p hongtaoz
 * SW7425-9: added 7425 support;
 *
 * Hydra_Software_Devel/18   10/7/10 4:51p nickh
 * SW7422-74: Add 7422 support
 *
 * Hydra_Software_Devel/17   7/8/10 5:28p jkim
 * SW7405-751: Bring KPD core out of reset and program  DUTY CYCLE to make
 * it work without opening LED PI.
 *
 * Hydra_Software_Devel/16   12/13/09 6:42p rpereira
 * SW7550-41: Fixed compilation issues for 7550
 *
 * Hydra_Software_Devel/15   4/9/08 10:55a farshidf
 * PR39192: fix compile error
 *
 * Hydra_Software_Devel/14   4/9/08 10:10a farshidf
 * PR39192: add 3548 support
 *
 * Hydra_Software_Devel/13   10/4/07 5:19p brianlee
 * PR35042: Set returned pointer to NULL if error occurs during kpd open.
 *
 * Hydra_Software_Devel/12   10/4/07 4:28p brianlee
 * PR35042: Fixed potential memory leak problem.
 *
 * Hydra_Software_Devel/11   2/2/07 11:21a jkim
 * PR27238: Modify to use the correct IRQ definition
 *
 * Hydra_Software_Devel/10   1/12/07 4:20p jkim
 * PR14344: Add 3563 support
 *
 * Hydra_Software_Devel/9   11/10/05 12:38p erickson
 * PR17936: don't read the L2 interrupt status and enable registers here
 * instead, trust that BINT will only call you if the interrupt actually
 * fired. this allows the L2 to be masked in the usermodedrv and yet
 * still processed by BINT. This is critical because the UPG_CPU_INTR is
 * shared with the linux kernel and cannot be masked.
 *
 * Hydra_Software_Devel/8   9/15/04 11:50a brianlee
 * PR12504: Added function to allow user to install interrupt callback.
 *
 * Hydra_Software_Devel/7   5/27/04 5:15p brianlee
 * PR11238: Fixed code to work with 7038 B0.
 *
 * Hydra_Software_Devel/6   2/3/04 4:02p brianlee
 * PR9598: Make 7038 kpd compile for now, we don't have RDB file for B0
 * yet.
 *
 * Hydra_Software_Devel/5   1/30/04 11:03a brianlee
 * PR9535: Fixed compile error in createcallback function call for now.
 * Must put in int ID when RDB for 7038B0 becomes available.
 *
 * Hydra_Software_Devel/4   12/29/03 3:59p marcusk
 * PR9117: Updated with changes required to support interrupt ids rather
 * than strings.
 *
 * Hydra_Software_Devel/3   11/4/03 6:59p brianlee
 * Get rid of enter/leave macros.
 *
 * Hydra_Software_Devel/2   9/30/03 11:21a brianlee
 * Fixed a warning.
 *
 * Hydra_Software_Devel/1   9/23/03 11:41a brianlee
 * Initial version.  This version contains stubs for now since 7038 A0
 * does not have keypad controller.
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkpd.h"
#include "bchp_ldk.h"
#include "bchp_irq0.h"
#include "bchp_int_id_irq0.h"

#if (BCHP_CHIP==7231) || (BCHP_CHIP==7344) || (BCHP_CHIP==7346) || (BCHP_CHIP==7358) || (BCHP_CHIP==7360) || (BCHP_CHIP==7422) || \
    (BCHP_CHIP==7425) || (BCHP_CHIP==7429) || (BCHP_CHIP==7435) || (BCHP_CHIP==7445) || (BCHP_CHIP==7552) || (BCHP_CHIP==7563) || \
    (BCHP_CHIP==7584)
    #include "bchp_int_id_irq0_aon.h"
    #include "bchp_irq0_aon.h"
#endif

BDBG_MODULE(bkpd);

#define BKPD_CHK_RETCODE( rc, func )        \
do {                                        \
    if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
    {                                       \
        goto done;                          \
    }                                       \
} while(0)

#define KPD_DEFAULT_PRESCALE        0x0055
#define KPD_DEFAULT_DUTYCYCLE_OFF   0x01
#define KPD_DEFAULT_DUTYCYCLE_ON    0xAA
#define KPD_DEFAULT_DEBOUNCE        0x40

#ifdef BCHP_INT_ID_ldk
    #define BCHP_INT_ID_ldk_irqen           BCHP_INT_ID_ldk
#endif

static void BKPD_P_HandleInterrupt_Isr
(
    void *pParam1,                      /* Device channel handle */
    int parm2                           /* not used */
);

/*******************************************************************************
*
*   Private Module Handles
*
*******************************************************************************/

typedef struct BKPD_CallbackInfo
{
    BKPD_CallbackFunc cbFunc ;
    void              *pParm1 ;
    int               parm2 ;
} BKPD_CallbackInfo;

BDBG_OBJECT_ID(BKPD);
typedef struct BKPD_P_Handle
{
    BDBG_OBJECT(BKPD)
    BCHP_Handle         hChip;
    BREG_Handle         hRegister;
    BINT_Handle         hInterrupt;
    BKNI_EventHandle    hEvent;
    BINT_CallbackHandle hCallback;
    bool                intMode;
    BKPD_CallbackInfo   stCallbackInfo;
} BKPD_P_Handle;

/*******************************************************************************
*
*   Default Module Settings
*
*******************************************************************************/
static const BKPD_Settings defKpdSettings =
{
    KPD_DEFAULT_PRESCALE,
    KPD_DEFAULT_DEBOUNCE,
    true,
    0
};

/*******************************************************************************
*
*   Public Module Functions
*
*******************************************************************************/
BERR_Code BKPD_Open(
    BKPD_Handle *pKpd,                  /* [output] Returns handle */
    BCHP_Handle hChip,                  /* Chip handle */
    BREG_Handle hRegister,              /* Register handle */
    BINT_Handle hInterrupt,             /* Interrupt handle */
    const BKPD_Settings *pDefSettings   /* Default settings */
    )
{
    BERR_Code       retCode = BERR_SUCCESS;
    BKPD_Handle     hDev;
    uint32_t        lval;
    uint16_t        sval;
    uint32_t        prescale;

    /* Sanity check on the handles we've been given. */
    BDBG_ASSERT( hChip );
    BDBG_ASSERT( hRegister );
    BDBG_ASSERT( hInterrupt );

    /* Alloc memory from the system heap */
    hDev = (BKPD_Handle) BKNI_Malloc( sizeof( BKPD_P_Handle ) );
    if( hDev == NULL )
    {
        *pKpd = NULL;
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BKPD_Open: BKNI_malloc() failed\n"));
        goto done;
    }

    BKNI_Memset(hDev, 0, sizeof(*hDev));
    BDBG_OBJECT_SET(hDev, BKPD);
    hDev->hChip     = hChip;
    hDev->hRegister = hRegister;
    hDev->hInterrupt = hInterrupt;
    hDev->intMode = pDefSettings->intMode;

    BKPD_CHK_RETCODE( retCode, BKNI_CreateEvent( &(hDev->hEvent) ) );

    prescale = pDefSettings->prescale;
    BREG_Write32 (hDev->hRegister, BCHP_LDK_PRESCHI, (uint8_t)(prescale >> 8));
    BREG_Write32 (hDev->hRegister, BCHP_LDK_PRESCLO, (uint8_t)(prescale & 0xff));

    BREG_Write32 (hDev->hRegister, BCHP_LDK_DEBOUNCE, pDefSettings->debounce);
#ifdef BCHP_LDK_KEY_MASK
    BREG_Write32 (hDev->hRegister, BCHP_LDK_KEY_MASK, pDefSettings->keyMask);
#endif

    /* Set up default duty cycle assuming 100% brightness. */
    BREG_Write32 (hDev->hRegister, BCHP_LDK_DUTYOFF, 0);
    BREG_Write32 (hDev->hRegister, BCHP_LDK_DUTYON, KPD_DEFAULT_DUTYCYCLE_ON+KPD_DEFAULT_DUTYCYCLE_OFF);

    /* Read data to clear any pending interrupt */
    BKPD_Read (hDev, &sval);

    if (hDev->intMode)
    {
        BKPD_CHK_RETCODE( retCode, BINT_CreateCallback(
            &(hDev->hCallback), hDev->hInterrupt, BCHP_INT_ID_ldk_irqen,
            BKPD_P_HandleInterrupt_Isr, (void *) hDev, 0x00 ) );
        BKPD_CHK_RETCODE( retCode, BINT_EnableCallback( hDev->hCallback ) );
    }

    BKNI_EnterCriticalSection();
    /* Enable keypad interrupt in led/keypad controller */
    lval = BREG_Read32 (hDev->hRegister, BCHP_LDK_CONTROL);
    lval |= BCHP_LDK_CONTROL_irqen_MASK;
    BREG_Write32 (hDev->hRegister, BCHP_LDK_CONTROL, lval);

    /* Clear user callback */
    hDev->stCallbackInfo.cbFunc = NULL;

    BKNI_LeaveCriticalSection();

    /* Set the VER bit.  Scan keypad only once at end of ON interval. */
    lval = BREG_Read32 (hDev->hRegister, BCHP_LDK_CONTROL);
    lval &= ~BCHP_LDK_CONTROL_swr_MASK;
    lval |=  BCHP_LDK_CONTROL_ver_MASK;
    lval &= 0x0f;                           /* make sure to zero out the upper bits */
    BREG_Write32 (hDev->hRegister, BCHP_LDK_CONTROL, lval);

    *pKpd = hDev;

done:
    if ((retCode != BERR_SUCCESS) && hDev)
    {
        BKNI_Free( (void *) hDev );
        *pKpd = NULL;
    }
    return( retCode );
}

BERR_Code BKPD_Close(
    BKPD_Handle hDev                    /* Device handle */
    )
{
    BERR_Code   retCode = BERR_SUCCESS;
    uint32_t    lval;

    BDBG_OBJECT_ASSERT(hDev, BKPD);

    if ( hDev->hCallback )
    {
        BKPD_CHK_RETCODE( retCode, BINT_DisableCallback( hDev->hCallback ) );
        BKPD_CHK_RETCODE( retCode, BINT_DestroyCallback( hDev->hCallback ) );
    }

    BKNI_EnterCriticalSection();
    /* Enable keypad interrupt in led/keypad controller */
    lval = BREG_Read32 (hDev->hRegister, BCHP_LDK_CONTROL);
    lval &= ~BCHP_LDK_CONTROL_irqen_MASK;
    BREG_Write32 (hDev->hRegister, BCHP_LDK_CONTROL, lval);
    BKNI_LeaveCriticalSection();

    BKNI_DestroyEvent( hDev->hEvent );
    BDBG_OBJECT_DESTROY(hDev, BKPD);
    BKNI_Free( (void *) hDev );

done:
    return( retCode );
}

BERR_Code BKPD_GetDefaultSettings(
    BKPD_Settings *pDefSettings,        /* [output] Returns default setting */
    BCHP_Handle hChip                   /* Chip handle */
)
{
    BERR_Code retCode = BERR_SUCCESS;
    BSTD_UNUSED(hChip);

    *pDefSettings = defKpdSettings;

    return( retCode );
}

BERR_Code BKPD_GetEventHandle(
    BKPD_Handle     hDev,           /* Device handle */
    BKNI_EventHandle *phEvent       /* [output] Returns event handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hDev, BKPD);

    *phEvent = hDev->hEvent;

    return( retCode );
}

BERR_Code BKPD_IsDataReady (
    BKPD_Handle         hKpd,           /* Device handle */
    bool                *ready          /* flag indicating key is pressed */
)
{
    uint32_t            lval;

    BDBG_OBJECT_ASSERT(hKpd, BKPD);
    lval = BREG_Read32 (hKpd->hRegister, BCHP_IRQ0_IRQSTAT);
    #if defined(BCHP_IRQ0_AON_IRQSTAT_ldkirq_MASK)
        *ready = (lval & BCHP_IRQ0_AON_IRQSTAT_ldkirq_MASK) ? true : false;
    #elif defined(BCHP_IRQ0_IRQSTAT_ldk_MASK)
        *ready = (lval & BCHP_IRQ0_IRQSTAT_ldk_MASK) ? true : false;
    #else
        *ready = (lval & BCHP_IRQ0_IRQSTAT_ldkirq_MASK) ? true : false;
    #endif

    return BERR_SUCCESS;
}

BERR_Code BKPD_Read_isr(
    BKPD_Handle         hKpd,           /* Device handle */
    uint16_t            *pData          /* pointer to data read from keypad */
)
{
    uint32_t            hi, lo;

    BDBG_OBJECT_ASSERT(hKpd, BKPD);
    hi = BREG_Read32 (hKpd->hRegister, BCHP_LDK_KEYROW32);
    lo = BREG_Read32 (hKpd->hRegister, BCHP_LDK_KEYROW10);

    *pData = (((uint16_t)hi) << 8) | ((uint16_t)lo);

    return BERR_SUCCESS;
}

BERR_Code BKPD_InstallInterruptCallback (
    BKPD_Handle         hKpd,           /* Device handle */
    BKPD_CallbackFunc   callback,       /* callback function */
    void                *pParm1,        /* application specified parameter */
    int                 parm2           /* application specified parameter */
)
{
    BDBG_OBJECT_ASSERT(hKpd, BKPD);
    
    BKNI_EnterCriticalSection();
    hKpd->stCallbackInfo.cbFunc = callback;
    hKpd->stCallbackInfo.pParm1 = pParm1 ;
    hKpd->stCallbackInfo.parm2 = parm2 ;
    BKNI_LeaveCriticalSection();

    return BERR_SUCCESS;
}

BERR_Code BKPD_UnInstallInterruptCallback (
    BKPD_Handle         hKpd            /* Device handle */
)
{
    BDBG_OBJECT_ASSERT(hKpd, BKPD);

    BKNI_EnterCriticalSection();
    hKpd->stCallbackInfo.cbFunc = NULL;
    hKpd->stCallbackInfo.pParm1 = NULL ;
    hKpd->stCallbackInfo.parm2 = 0 ;
    BKNI_LeaveCriticalSection();

    return BERR_SUCCESS;
}

/*******************************************************************************
*
*   Private Module Functions
*
*******************************************************************************/
static void BKPD_P_HandleInterrupt_Isr
(
    void *pParam1,                      /* Device channel handle */
    int parm2                           /* not used */
)
{
    BKPD_Handle         hDev;
    uint16_t            data;

    BSTD_UNUSED(parm2);
    hDev = (BKPD_Handle) pParam1;
    BDBG_OBJECT_ASSERT(hDev, BKPD);

    /*
     * Check to see if user has installed a callback
     */
    if(hDev->stCallbackInfo.cbFunc)
    {
        hDev->stCallbackInfo.cbFunc (   hDev->stCallbackInfo.pParm1,
                                        hDev->stCallbackInfo.parm2,
                                        NULL) ;
    }

    /*
     * Clear keypad interrupt by reading the key registers
     */
    BKPD_Read_isr(hDev, &data);

    BKNI_SetEvent( hDev->hEvent );

    return;
}
