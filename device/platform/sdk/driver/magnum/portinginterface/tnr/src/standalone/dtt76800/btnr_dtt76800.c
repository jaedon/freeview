/***************************************************************************
 *     Copyright (c) 2005-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnr_dtt76800.c $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 6/22/10 6:16p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/tnr/dtt76800/btnr_dtt76800.c $
 * 
 * Hydra_Software_Devel/9   6/22/10 6:16p vishk
 * SW7420-839: Add support to program AGC gain to 31xx TNR PI
 * 
 * Hydra_Software_Devel/8   10/8/09 3:54p erickson
 * SW3548-2522: add 2nd stage to RF tuning for digital signals. change ATC
 * bit. also, modified the timing of the 2nd stage of RF tuning for
 * analog signals as requested by frontend team.
 *
 * Hydra_Software_Devel/7   9/30/09 3:51p erickson
 * SW3548-2507: for analog channels, we have to first tune with internal
 * wideband AGC on, wait 5 msec, then tune with internal wideband AGC off
 *
 * Hydra_Software_Devel/6   8/25/09 9:42a vishk
 * SW7405-2841: Bcm31xx: Add Power Management support for TNR
 *
 * Hydra_Software_Devel/5   8/24/09 6:03p vishk
 * SW7405-2841: Bcm31xx: Add Power Management support for TNR
 *
 * Hydra_Software_Devel/4   3/9/09 3:27p erickson
 * PR52989: Thomson DTT7680x register change
 *
 * Hydra_Software_Devel/3   6/25/08 12:40p dliu
 * PR43811: Change AGC settings for analog video
 *
 * Hydra_Software_Devel/2   6/10/08 10:22p vishk
 * PR 41821: Develop TNR PI for DTT76800
 *
 * Hydra_Software_Devel/1   4/30/08 7:50p vishk
 * PR 39716: TNR PI support for DTT76800.
 *
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "btnr.h"
#include "btnr_priv.h"
#include "btnr_dtt76800.h"

BDBG_MODULE(btnr_dtt76800);

#define DEV_MAGIC_ID            ((BERR_TNR_ID<<16) | 0xFACE)

#define CHK_RETCODE( rc, func )         \
do {                                        \
    if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
    {                                       \
        goto done;                          \
    }                                       \
} while(0)


/*******************************************************************************
*
*   Private Module Handles
*
*******************************************************************************/


typedef struct BTNR_P_DTT76800_Settings
{
    unsigned long ifFreq;
    BREG_I2C_Handle hI2CReg;
    unsigned short i2cAddr;

    unsigned long rfFreq;
    BTNR_TunerMode tunerMode;
    bool isInitialized;
} BTNR_P_DTT76800_Settings;

typedef struct BTNR_P_DTT76800_Handle       *BTNR_DTT76800_Handle;

typedef struct BTNR_P_DTT76800_Handle
{
    uint32_t magicId;                   /* Used to check if structure is corrupt */

    BTNR_P_DTT76800_Settings settings;
} BTNR_P_DTT76800_Handle;



/*******************************************************************************
*
*   Default Module Settings
*
*******************************************************************************/
static const BTNR_DTT76800_Settings defDevSettings =
{
    BTNR_DTT76800_SETTINGS_IFFREQ,
    BTNR_DTT76800_SETTINGS_I2CADDR
};

/*******************************************************************************
*
*   Private Module Data
*
*******************************************************************************/
static BERR_Code BTNR_DTT76800_Close(
    BTNR_Handle hDev                    /* [in] Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BTNR_DTT76800_Close);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    hDev->magicId = 0x00;       /* clear it to catch inproper use */
    BKNI_Free( (void *) hDev->hDevImpl );
    BKNI_Free( (void *) hDev );

    BDBG_LEAVE(BTNR_DTT76800_Close);
    return( retCode );
}

static BERR_Code BTNR_DTT76800_SetRfFreq(
    BTNR_DTT76800_Handle hDev,              /* [in] Device handle */
    uint32_t rfFreq,                    /* [in] Requested tuner freq., in Hertz */
    BTNR_TunerMode tunerMode            /* [in] Requested tuner mode */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTNR_P_DTT76800_Settings *pTnrImplData;
    uint8_t tnr[6];
    unsigned long n_cnt;

    BDBG_ENTER(BTNR_DTT76800_SetRfFreq);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    pTnrImplData = &hDev->settings;
    if( pTnrImplData->isInitialized == false )
    {
        /* If required */
        pTnrImplData->isInitialized = true;
    }

    if( rfFreq < 136250001 )
    {
        tnr[3] = 0x39;
    }
    else if( rfFreq < 417250001 )
    {
        tnr[3] = 0x7c;
    }
    else
    {
        tnr[3] = 0xb5;
    }

    n_cnt = (rfFreq + pTnrImplData->ifFreq) / (1000000 / 16);

    tnr[0] = 0x7F & (n_cnt >> 8);
    tnr[1] = 0xFF & n_cnt;
    tnr[2] = 0x85;
    tnr[4] = 0x03;
    tnr[5] = 0x80;

    CHK_RETCODE( retCode, BREG_I2C_WriteNoAddr( pTnrImplData->hI2CReg, pTnrImplData->i2cAddr, tnr, sizeof(tnr) ) );

    if( tunerMode == BTNR_TunerMode_eAnalog )
    {
        BKNI_Sleep(20);
        tnr[5] = 0xA0; /* AGD (bit 5) = 1 to disable internal wideband AGC */
        CHK_RETCODE( retCode, BREG_I2C_WriteNoAddr( pTnrImplData->hI2CReg, pTnrImplData->i2cAddr, tnr, sizeof(tnr) ) );
    }
    else
    {
        BKNI_Sleep(25);
        tnr[4] = 0x23; /* set ATC=1 25msec later, for slow AGC time constant */
        CHK_RETCODE( retCode, BREG_I2C_WriteNoAddr( pTnrImplData->hI2CReg, pTnrImplData->i2cAddr, tnr, sizeof(tnr) ) );
    }

    pTnrImplData->rfFreq = rfFreq;
    pTnrImplData->tunerMode = tunerMode;

done:
    BDBG_LEAVE(BTNR_DTT76800_SetRfFreq);
    return( retCode );
}

static BERR_Code BTNR_DTT76800_GetRfFreq(
    BTNR_DTT76800_Handle hDev,              /* [in] Device handle */
    uint32_t *rfFreq,                   /* [output] Returns tuner freq., in Hertz */
    BTNR_TunerMode *tunerMode           /* [output] Returns tuner mode */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTNR_P_DTT76800_Settings *pTnrImplData;


    BDBG_ENTER(BTNR_DTT76800_GetRfFreq);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    pTnrImplData = &hDev->settings;

    *rfFreq = pTnrImplData->rfFreq;
    *tunerMode = pTnrImplData->tunerMode;

    BDBG_LEAVE(BTNR_DTT76800_GetRfFreq);
    return( retCode );
}

/*******************************************************************************
*
*   Public Module Functions
*
*******************************************************************************/

BERR_Code BTNR_DTT76800_Open(
    BTNR_Handle *phDev,                 /* [output] Returns handle */
    BCHP_Handle hChip,                  /* [in] Chip handle */
    BREG_I2C_Handle hI2CReg,            /* [in] I2C Register handle */
    const BTNR_DTT76800_Settings *pDefSettings /* [in] Default settings */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTNR_DTT76800_Handle hDTT76800Dev;
    BTNR_P_DTT76800_Settings *pTnrImplData;
    BTNR_Handle hDev;


    BDBG_ENTER(BTNR_DTT76800_Open);
    BDBG_ASSERT( hI2CReg );
    BSTD_UNUSED(hChip);

    hDev = NULL;
    /* Alloc memory from the system heap */
    hDTT76800Dev = (BTNR_DTT76800_Handle) BKNI_Malloc( sizeof( BTNR_P_DTT76800_Handle ) );
    if( hDTT76800Dev == NULL )
    {
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BTNR_DTT76800_Open: BKNI_malloc() failed\n"));
        goto done;
    }
    BKNI_Memset( hDTT76800Dev, 0x00, sizeof( BTNR_P_DTT76800_Handle ) );

    hDev = (BTNR_Handle) BKNI_Malloc( sizeof( BTNR_P_Handle ) );
    if( hDev == NULL )
    {
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BTNR_DTT76800_Open: BKNI_malloc() failed\n"));
        BKNI_Free( hDTT76800Dev );
        goto done;
    }
    BKNI_Memset( hDev, 0x00, sizeof( BTNR_P_Handle ) );

    hDTT76800Dev->magicId = DEV_MAGIC_ID;
    pTnrImplData = &hDTT76800Dev->settings;
    pTnrImplData->hI2CReg = hI2CReg;
    pTnrImplData->ifFreq = pDefSettings->ifFreq;
    pTnrImplData->i2cAddr = pDefSettings->i2cAddr;
    if( (pDefSettings->i2cAddr & 0x80) )
    {
        BDBG_ERR(("I2C Address must be 7 bit format"));
        BDBG_ASSERT( !(pDefSettings->i2cAddr & 0x80) );
    }

    pTnrImplData->rfFreq = 0;
    pTnrImplData->tunerMode = BTNR_TunerMode_eDigital;
    pTnrImplData->isInitialized = false;

    hDev->hDevImpl = (void *) hDTT76800Dev;
    hDev->magicId = DEV_MAGIC_ID;
    hDev->pSetRfFreq = (BTNR_SetRfFreqFunc) BTNR_DTT76800_SetRfFreq;
    hDev->pGetRfFreq = (BTNR_GetRfFreqFunc) BTNR_DTT76800_GetRfFreq;
    hDev->pGetAgcRegVal = (BTNR_GetAgcRegValFunc) NULL;
    hDev->pSetAgcRegVal = (BTNR_SetAgcRegValFunc) NULL;
    hDev->pClose = (BTNR_CloseFunc) BTNR_DTT76800_Close;
    hDev->pGetPowerSaver = (BTNR_GetPowerSaverFunc)NULL;
    hDev->pSetPowerSaver = (BTNR_SetPowerSaverFunc)NULL;


done:
    *phDev = hDev;
    BDBG_LEAVE(BTNR_DTT76800_Open);
    return( retCode );
}

BERR_Code BTNR_DTT76800_GetDefaultSettings(
    BTNR_DTT76800_Settings *pDefSettings,   /* [output] Returns default setting */
    BCHP_Handle hChip                   /* [in] Chip handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BTNR_DTT76800_GetDefaultSettings);
    BSTD_UNUSED(hChip);

    *pDefSettings = defDevSettings;

    BDBG_LEAVE(BTNR_DTT76800_GetDefaultSettings);
    return( retCode );
}

