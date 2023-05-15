/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bscd.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 10/17/12 12:24p $
 *
 * Module Description:
 *
 * Revision History:
 * $brcm_Log: /magnum/portinginterface/scd/src/core40nm/bscd.c $
 * 
 * Hydra_Software_Devel/3   10/17/12 12:24p qxie
 * SW7346-1066: 1. BSCD_Channel_ResetCard is calling
 * BSCD_Channel_SetVccLevel without checking the channelHandle-
 * >currentChannelSettings.setVcc boolean 2. disbale flow control in set
 * parameter
 * 
 * Hydra_Software_Devel/3   10/17/12 12:22p qxie
 * SW7346-1066: 1. BSCD_Channel_ResetCard is calling
 * BSCD_Channel_SetVccLevel without checking the channelHandle-
 * >currentChannelSettings.setVcc boolean 2. disbale flow control in set
 * parameter
 * 
 * Hydra_Software_Devel/2   9/21/12 9:40a qxie
 * SW7360-34:add support to SCD
 * 
 * Hydra_Software_Devel/1   9/7/12 11:02a qxie
 * SW7429-283:Smartcard: remove symlink in PI code
 * 
 * Hydra_Software_Devel/37   7/5/12 11:11a qxie
 * SW7346-900:fix the order of disable clock and vcc in ResetIFD
 * 
 * Hydra_Software_Devel/36   7/5/12 11:07a qxie
 * SW7346-900:add delay in ChannelResetIFD as well.
 * 
 * Hydra_Software_Devel/35   7/1/12 3:14p qxie
 * SW7346-900:Smartcard: Deactivation function does not have enough delay
 * for TDA8024 to finish
 * 
 * Hydra_Software_Devel/34   6/27/12 5:34p qxie
 * SW7425-3109:fix bugs to support Nordig standard
 * 
 * Hydra_Software_Devel/33   6/1/12 12:23p qxie
 * SW7584-26:add support to 7584
 * 
 * Hydra_Software_Devel/32   5/10/12 4:31p qxie
 * SW7429-109:Add smartcard support to 7429
 * 
 * Hydra_Software_Devel/31   5/9/12 1:26p qxie
 * SW7425-2978:Convert conditional compiler settings to runtime
 * configurable setting in SCD PI
 * 
 * Hydra_Software_Devel/30   5/2/12 2:47p qxie
 * SW7360-15: add support to 7360
 * 
 * Hydra_Software_Devel/29   4/5/12 4:01p qxie
 * SW7429-109:Add smartcard support to 7429
 * 
 * Hydra_Software_Devel/28   3/30/12 2:41p qxie
 * SW7231-622:SMARTCARD: 1.8V Vcc support
 * 
 * Hydra_Software_Devel/27   3/29/12 4:54p qxie
 * SW7429-109:add smartcard support
 * 
 * Hydra_Software_Devel/26   3/21/12 4:11p mward
 * SW7435-18:  Compile error / typo.
 * 
 * Hydra_Software_Devel/25   3/20/12 2:52p qxie
 * SW7435-18: add support to 7435
 * 
 * Hydra_Software_Devel/24   2/10/12 2:32p qxie
 * SW7346-23: add CustomerE
 * 
 * Hydra_Software_Devel/23   12/1/11 4:09p qxie
 * SW7346-23: add support to E* direct connect card
 * 
 * Hydra_Software_Devel/22   10/13/11 5:19p qxie
 * SW7346-23:Fix a bug in BSCD_Channel_Deactivate() which put IO line in
 * an unknow state
 * 
 * Hydra_Software_Devel/21   10/11/11 3:59p qxie
 * SW7231-136: fix a bug for 3V/5V control in 7344
 * 
 * Hydra_Software_Devel/20   9/9/11 11:09a qxie
 * SW7231-136: fix a typo
 * 
 * Hydra_Software_Devel/19   8/19/11 11:28a dlwin
 * SW7346-461: Resolved Coverity: 35203 for 40nm chips.
 * 
 * Hydra_Software_Devel/18   6/30/11 4:47p qxie
 * SW7358-43: add smartcard support to 7358(chnagerst line initial state)
 * 
 * Hydra_Software_Devel/17   6/20/11 6:05p qxie
 * SW7552-39:add smartcard support for 7552
 * 
 * Hydra_Software_Devel/16   5/31/11 6:10p qxie
 * SW7358-43:add smartcard support to 7358
 * 
 * Hydra_Software_Devel/15   5/24/11 11:48a qxie
 * SW7346-218:Need to add support for the smartcard controller chips
 * TDA803x
 * 
 * Hydra_Software_Devel/14   5/23/11 10:03a qxie
 * SW7344-54: add smartcard support to 7344
 * 
 * Hydra_Software_Devel/13   5/20/11 4:56p qxie
 * SW7344-54: add smartcard support to 7344
 * 
 * Hydra_Software_Devel/12   5/10/11 1:47p gmohile
 * SW7231-128 : Add power management support
 * 
 * Hydra_Software_Devel/11   5/6/11 2:22p qxie
 * SW7358-43: add smartcard support to 7358
 * 
 * Hydra_Software_Devel/10   4/29/11 3:57p qxie
 * SW7231-136:add smartcard support to 7231
 * 
 * Hydra_Software_Devel/9   4/1/11 3:41p qxie
 * sw7346-23: smartcard: add 3V/5V support and no TDA8024 support
 * 
 * Hydra_Software_Devel/8   1/31/11 1:39p hongtaoz
 * SW7346-23: fixed compile error for 7425;
 * 
 * Hydra_Software_Devel/7   1/31/11 11:57a qxie
 * SW7346-23:missing "#"
 * 
 * Hydra_Software_Devel/6   1/28/11 5:04p qxie
 * SW7346-23:add support to 7346
 * 
 * Hydra_Software_Devel/5   1/12/11 12:02p katrep
 * SW7231-2:fixed compiler warnings
 * 
 * 
 ************************************************************************/
 
#include "bstd.h"
#include "bkni.h"
#include "bchp_pm.h"
#include "bchp_sca.h"
#include "bchp_scb.h"

#include "bchp_scirq0.h"
#include "bchp_sun_top_ctrl.h"

#include "bscd.h"
#include "bscd_priv.h"
#include "bint.h"
#include "bchp_int_id_scirq0.h"
#include "bchp_gio.h"

#ifdef BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif

#if (BCHP_CHIP==7344)
#include "bchp_aon_pin_ctrl.h"
#include "bchp_gio_aon.h"
#endif

#define BCHP_SCA_SC_CLK_CMD_clk_en_MASK BCHP_SCA_SC_CLK_CMD_1_clk_en_MASK

#if 1
#undef BDBG_ENTER
#undef BDBG_LEAVE
#define BDBG_ENTER(x)
#define BDBG_LEAVE(x)
#endif

BDBG_MODULE(BSCD);


/*******************************************************************************
*   Default Module and Channel Settings.  Note that we could only modify
*   Module settings during BSCD_Open.
*******************************************************************************/
static const BSCD_Settings BSCD_defScdSettings =
{
    /* This attribute indicates the source of clock and the value */
    {BSCD_ClockFreqSrc_eInternalClock, 27000000, false, false},

    /* maximum SCD channels supported */
    BSCD_MAX_SUPPOTED_CHANNELS,
    NULL,
};

static const BSCD_ChannelSettings BSCD_defScdChannelSettings =
{
        /* Smart Card Standard */
        BSCD_Standard_eNDS,

        /* Asynchronous Protocol Types. */
        BSCD_AsyncProtocolType_e0,

        /* This read-only attribute specifies the default
            source clock frequency in Hz. */
        BSCD_INTERNAL_CLOCK_FREQ,

        /* ICC CLK frequency in Hz which is
            source freq / SC_CLK_CMD[etu_clkdiv] / SC_CLK_CMD[sc_clkdiv] */
        BSCD_INTERNAL_CLOCK_FREQ/BSCD_DEFAULT_ETU_CLKDIV/BSCD_DEFAULT_SC_CLKDIV,

        /* ETU in microseconds which is source freq / SC_CLK_CMD[etu_clkdiv] */
            /* (SC_PRESCALE * external_clock_div + (external_clock_div - 1))  */
        BSCD_INTERNAL_CLOCK_FREQ/BSCD_DEFAULT_ETU_CLKDIV/(BSCD_DEFAULT_PRESCALE+1)/BSCD_DEFAULT_BAUD_DIV,

        /* This read-only attribute specifies the maximum IFSD.
            Should be 264. */
        BSCD_MAX_TX_SIZE,

        /* This attribute indicates the current IFSD */
        BSCD_DEFAULT_EMV_INFORMATION_FIELD_SIZE,

        /* Clock Rate Conversion Factor,
            F in 1,2,3,4,5,6,9, 10, 11, 12 or 13.
            Default is 1. */
        BSCD_DEFAULT_F,

        /* Baud Rate Adjustment Factor,
            D in 1,2,3,4,5,6,8 or 9.
            Default is 1. */
        BSCD_DEFAULT_D,

        /*  ETU Clock Divider in
            SC_CLK_CMD register. Valid value is
            from 1 to 8. Default is 6. */
        BSCD_DEFAULT_ETU_CLKDIV,

        /*  SC Clock Divider in
            SC_CLK_CMD register. Valid value is
            1,2,3,4,5,8,10,16. Default is 1. */
        BSCD_DEFAULT_SC_CLKDIV,

        /* Prescale Value */
        BSCD_DEFAULT_PRESCALE,

        /* external clock divisor */
        BSCD_DEFAULT_EXTERNAL_CLOCK_DIVISOR,

        /* Baud Divisor */
        BSCD_DEFAULT_BAUD_DIV,

        /* Number of transmit parity retries per character in
            SC_UART_CMD_2 register. Default is 4 and max is 6.
            7 indicates infinite retries */
        BSCD_DEFAULT_TX_PARITY_RETRIES,

        /* Number of receive parity retries per character in
            SC_UART_CMD_2 register. Default is 4 and max is 6.
            7 indicates infinite retries */
        BSCD_DEFAULT_RX_PARITY_RETRIES,

        /* work waiting time in SC_TIME_CMD register. Other than EMV
            standard, only valid if current protocol is T=0. */
        {BSCD_DEFAULT_WORK_WAITING_TIME,   BSCD_TimerUnit_eETU},

        /* block Wait time in SC_TIME_CMD register. Only valid if
            current protocol is T=1. */
        {BSCD_DEFAULT_BLOCK_WAITING_TIME,   BSCD_TimerUnit_eETU},

        /* Extra Guard Time in SC_TGUARD register. */
        {BSCD_DEFAULT_EXTRA_GUARD_TIME,   BSCD_TimerUnit_eETU},

        /*  block Guard time in SC_BGT register.Other than EMV
            standard, only valid if current protocol is T=1.  */
        {BSCD_DEFAULT_BLOCK_GUARD_TIME,   BSCD_TimerUnit_eETU},

        /* character Wait time in SC_PROTO_CMD register. Only valid
            if current protocol is T=1. */
        BSCD_DEFAULT_CHARACTER_WAIT_TIME_INTEGER,

        /* EDC encoding. Only valid if current protocol is T=1. */
        {BSCD_EDCEncode_eLRC,   false},

        /* arbitrary Time Out value for any synchronous transaction. */
        {BSCD_DEFAULT_TIME_OUT,   BSCD_TimerUnit_eMilliSec},

        /* Specify if we need auto deactivation sequence */
        false,

        /* True if we receive 0x60 in T=0, we will ignore it.  Otherwise, we treat 0x60 as a valid data byte */
        false,

        /* Debounce info for IF_CMD_2 */
        {BSCD_ScPresMode_eMask, true, BSCD_DEFAULT_DB_WIDTH},

        BSCD_ResetCardAction_eReceiveAndDecode,   /* Specify if we want the driver to read, decode and program registers */

        {0,   BSCD_TimerUnit_eETU} , /* block wait time extension */
        true  /* pres is low */     ,
        {BSCD_MAX_ATR_START_IN_CLK_CYCLES, BSCD_TimerUnit_eCLK},
        false  , /* connect to TDA8024 */
        true,   /* pin setting */
        true,   /* VCC setting */
        BSCD_VccLevel_e5V ,      /* 5 Volts */
        BSCD_ClockFreq_e27MHZ,
        BSCD_MAX_RESET_IN_CLK_CYCLES,
        true,
        false
};




/*******************************************************************************
*   Public Module Functions
*******************************************************************************/
BERR_Code BSCD_GetDefaultSettings(
        BSCD_Settings   *outp_sSettings,
        BCHP_Handle     in_chipHandle
)
{
    BERR_Code errCode = BERR_SUCCESS;

    BDBG_ENTER(BSCD_GetDefaultSettings);
    BDBG_ASSERT( in_chipHandle );

    *outp_sSettings = BSCD_defScdSettings;

    BSTD_UNUSED(in_chipHandle);

    BDBG_LEAVE(BSCD_GetDefaultSettings);
    return( errCode );
}


BERR_Code BSCD_Open(
        BSCD_Handle         *outp_handle,
        BREG_Handle         in_regHandle,
        BCHP_Handle         in_chipHandle,
            BINT_Handle         in_interruptHandle,
        const BSCD_Settings *inp_sSettings
)
{
    BERR_Code errCode = BERR_SUCCESS;
    BSCD_Handle moduleHandle;
    unsigned int channelNum;
    uint32_t ulVal = 0;

    BDBG_ENTER(BSCD_Open);
    BDBG_ASSERT( in_chipHandle );
    BDBG_ASSERT( in_regHandle );
    BDBG_ASSERT( in_interruptHandle );

    *outp_handle = NULL;

    /* on many chips, ulVal is actually used, but having an unconditional BSTD_UNUSED makes it easier to maintain this code. */
    BSTD_UNUSED(ulVal);

    /* Alloc memory from the system heap */
    if ((moduleHandle =
                (BSCD_Handle) BKNI_Malloc( sizeof( BSCD_P_Handle)))
                == NULL) {
        /* wrap initially detected error code */
        errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto BSCD_P_DONE_LABEL;
    }

    BKNI_Memset(moduleHandle, 0, sizeof( BSCD_P_Handle ));

    moduleHandle->ulMagicNumber = BSCD_P_HANDLE_MAGIC_NUMBER;
    moduleHandle->chipHandle = in_chipHandle;
    moduleHandle->regHandle = in_regHandle;
    moduleHandle->interruptHandle = in_interruptHandle;

    if (inp_sSettings == NULL)
        moduleHandle->currentSettings = BSCD_defScdSettings;
    else {
        moduleHandle->currentSettings = *inp_sSettings;

    }

    /* Get the chip information for Bcm7038 */
    BCHP_GetChipInfo( moduleHandle->chipHandle, &moduleHandle->chipId, &moduleHandle->chipRev );
    BDBG_MSG(( "chipId=%d, chipRev=%d\n", moduleHandle->chipId, moduleHandle->chipRev ));
/*** move clock setting  to channel OPen */

    /***************************************************************
        Warning:  Note that we have to modify the board to use GPIO and connect it
                 to pin 3 (3V/5V) of TDA chip and run this function.
                 Make sure to disconnect your QAM or    QPSK connection before
                 calling this function or your smartcard will be damaged.
    ***************************************************************/
/** move 3v?5V setting to channel Open */

    /*
        If inp_sSettings->maxChannels == 0, set it to
        BSCD_MAX_SUPPOTED_CHANNELS
    */
    if (moduleHandle->currentSettings.ucMaxChannels == 0)
        moduleHandle->currentSettings.ucMaxChannels =
                            BSCD_MAX_SUPPOTED_CHANNELS;

    for( channelNum = 0;
        channelNum < moduleHandle->currentSettings.ucMaxChannels;
        channelNum++ )
    {
        moduleHandle->channelHandles[channelNum] = NULL;
    }

    *outp_handle = moduleHandle;

    BKNI_EnterCriticalSection();
    moduleHandle->bIsOpen = true;
    BKNI_LeaveCriticalSection();

BSCD_P_DONE_LABEL:

    BDBG_LEAVE(BSCD_Open);
    return( errCode );

}


BERR_Code BSCD_Close(
        BSCD_Handle inout_handle
)
{
    BERR_Code errCode = BERR_SUCCESS;
       /* 12/02/2006 QX: add this to avoid crash during BKNI_Free */
     void *pTemp;


    BDBG_ENTER(BSCD_Close);
    BDBG_ASSERT( inout_handle );

    BKNI_EnterCriticalSection();
    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, (inout_handle ==  NULL) );
    BKNI_LeaveCriticalSection();

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (inout_handle->ulMagicNumber != BSCD_P_HANDLE_MAGIC_NUMBER ) );

    BKNI_EnterCriticalSection();
    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (inout_handle->bIsOpen ==  false) );
    BKNI_LeaveCriticalSection();

BSCD_P_DONE_LABEL:
    BKNI_EnterCriticalSection();
    inout_handle->bIsOpen = false;
    BKNI_LeaveCriticalSection();

 /* 12/02/2006 QX: add this to avoid crash during BKNI_Free */
    BKNI_EnterCriticalSection();
    pTemp = inout_handle;
    inout_handle = NULL;
    BKNI_LeaveCriticalSection();

    BKNI_Free(pTemp);


    BDBG_LEAVE(BSCD_Close);
    return( errCode );
}


BERR_Code BSCD_GetTotalChannels(
        BSCD_Handle     in_handle,
        unsigned char       *outp_ucTotalChannels
)
{
    BERR_Code errCode = BERR_SUCCESS;

    BDBG_ENTER(BSCD_GetTotalChannels);
    BDBG_ASSERT( in_handle );

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (in_handle->ulMagicNumber != BSCD_P_HANDLE_MAGIC_NUMBER ) );

    *outp_ucTotalChannels = in_handle->currentSettings.ucMaxChannels;

BSCD_P_DONE_LABEL:

    BDBG_LEAVE(BSCD_GetTotalChannels);
    return( errCode );
}


BERR_Code BSCD_GetChannelDefaultSettings(
        BSCD_Handle             in_handle,
        unsigned int            in_channelNo,
        BSCD_ChannelSettings    *outp_sSettings
)
{
    BERR_Code errCode = BERR_SUCCESS;


    BDBG_ENTER(BSCD_GetChannelDefaultSettings);
    BDBG_ASSERT( in_handle );

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (in_handle->ulMagicNumber != BSCD_P_HANDLE_MAGIC_NUMBER ) );

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BERR_INVALID_PARAMETER,
        (in_channelNo >= in_handle->currentSettings.ucMaxChannels) );

    *outp_sSettings = BSCD_defScdChannelSettings;

BSCD_P_DONE_LABEL:

    BDBG_LEAVE(BSCD_GetChannelDefaultSettings);
    return( errCode );
}


BERR_Code BSCD_Channel_Open(
        BSCD_Handle                 in_handle,
        BSCD_ChannelHandle          *outp_channelHandle,
        unsigned int                in_channelNo,
        const BSCD_ChannelSettings  *inp_channelDefSettings
)
{
    BERR_Code errCode = BERR_SUCCESS;
    BSCD_ChannelHandle channelHandle = NULL;
    uint32_t            ulVal = 0;
	
#if (BCHP_CHIP == 3548) || (BCHP_CHIP==3556)|| (BCHP_CHIP==35230)
    BINT_Id             intId = BCHP_INT_ID_CREATE(BCHP_SCIRQ0_SCIRQEN, BCHP_SCIRQ0_SCIRQEN_sca_irqen_SHIFT);

#else
    BINT_Id             intId = BCHP_INT_ID_sca_irqen;
#endif
#ifdef BCHP_PWR_RESOURCE_SMARTCARD0
    BCHP_PWR_ResourceId resourceId = BCHP_PWR_RESOURCE_SMARTCARD0;
#ifdef BCHP_PWR_RESOURCE_SMARTCARD1
    if(in_channelNo == 1) {
	resourceId = BCHP_PWR_RESOURCE_SMARTCARD1;
    }
#endif
#endif

    BDBG_ENTER(BSCD_Channel_Open);
    BDBG_ASSERT( in_handle );
	BSTD_UNUSED(ulVal);

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (in_handle->ulMagicNumber != BSCD_P_HANDLE_MAGIC_NUMBER ) );

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BERR_INVALID_PARAMETER,
        (in_channelNo >= in_handle->currentSettings.ucMaxChannels) );

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (in_handle->bIsOpen ==  false) );

    /* channel handle must be NULL.  */
    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (in_handle->channelHandles[in_channelNo]  != NULL) );

    *outp_channelHandle = NULL;

    /* Alloc memory from the system heap */
    if ((channelHandle =
            (BSCD_ChannelHandle) BKNI_Malloc(sizeof(BSCD_P_ChannelHandle)))
            == NULL) {
        /* wrap initially detected error code */
        errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto BSCD_P_DONE_LABEL;
    }

    BKNI_Memset(channelHandle, 0, sizeof( BSCD_P_ChannelHandle ));

    channelHandle->ulMagicNumber = BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER;
    channelHandle->moduleHandle = in_handle;

    channelHandle->ucChannelNumber = in_channelNo;

#ifdef BCHP_PWR_RESOURCE_SMARTCARD0
	/* acquire power */
	BCHP_PWR_AcquireResource(channelHandle->moduleHandle->chipHandle, resourceId);
#endif  

#ifdef BSCD_EMV2000_CWT_PLUS_4
    channelHandle->bIsReceive = false;
#endif
    switch (in_channelNo) {
        case 0:
            channelHandle->ulRegStartAddr = BCHP_SCA_SC_UART_CMD_1;

            break;
        case 1:
            channelHandle->ulRegStartAddr = BCHP_SCB_SC_UART_CMD_1;

            break;
        default:
            channelHandle->ulRegStartAddr = BCHP_SCA_SC_UART_CMD_1;
            break;
    }
    BKNI_EnterCriticalSection();
    BREG_Write32(
        channelHandle->moduleHandle->regHandle,
        (channelHandle->ulRegStartAddr + BSCD_P_INTR_EN_1),
         0);

    BREG_Write32(
        channelHandle->moduleHandle->regHandle,
        (channelHandle->ulRegStartAddr + BSCD_P_INTR_EN_2),
         0);
    channelHandle->ulStatus1 = 0x00;
    channelHandle->ulStatus2 = 0x00;
    channelHandle->ulIntrStatus1 = 0x00;
    channelHandle->ulIntrStatus2 = 0x00;
    BKNI_LeaveCriticalSection();

    /*  Enable smartcard interrupt   */
    switch (in_channelNo) {
        case 0:
        
            intId = BCHP_INT_ID_sca_irqen;
            BREG_Write32( in_handle->regHandle, BCHP_SCIRQ0_SCIRQEN,
                    (BCHP_SCIRQ0_SCIRQEN_sca_irqen_MASK |
                      BREG_Read32(in_handle->regHandle,  BCHP_SCIRQ0_SCIRQEN)) );
            break;
        case 1:
            intId = BCHP_INT_ID_scb_irqen;
            BREG_Write32(  in_handle->regHandle, BCHP_SCIRQ0_SCIRQEN,
                    (BCHP_SCIRQ0_SCIRQEN_scb_irqen_MASK |
                      BREG_Read32(in_handle->regHandle,  BCHP_SCIRQ0_SCIRQEN)) );
            break;

    }


    BSCD_P_CHECK_ERR_CODE_FUNC( errCode, BINT_CreateCallback(
            &(channelHandle->channelIntCallback), in_handle->interruptHandle, intId,
            BSCD_Channel_P_IntHandler_isr, (void *) channelHandle, 0x00 ) );
    BSCD_P_CHECK_ERR_CODE_FUNC( errCode, BINT_EnableCallback(channelHandle->channelIntCallback) );


    BDBG_MSG(("in_channelNo = %d\n", in_channelNo));
    BDBG_MSG(("channelHandle->ulRegStartAddr = 0x%x\n", channelHandle->ulRegStartAddr));

	if (inp_channelDefSettings != NULL){
		BSCD_Channel_P_SetSrcClock(
			channelHandle,
			inp_channelDefSettings);
        BSCD_Channel_SetParameters(
            channelHandle,
            inp_channelDefSettings);
		}
    else {
		BSCD_Channel_P_SetSrcClock(
			channelHandle,
			&BSCD_defScdChannelSettings);
        BSCD_Channel_SetParameters(
            channelHandle,
            &BSCD_defScdChannelSettings);
    }
        /* set pin muxing */
    if( channelHandle->currentChannelSettings.setPinmux){

        switch (in_channelNo) {
            case 0:

            /* Pin muxing */

#if (BCHP_CHIP==7422) || (BCHP_CHIP==7425)
    ulVal = BREG_Read32 (in_handle->regHandle,
                        BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16);
                ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_091_MASK |
                            BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_090_MASK|
                            BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_089_MASK);
                BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16,
                    (ulVal |
                        (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_091_SHIFT) |
                        (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_090_SHIFT) |

                        (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_089_SHIFT)   ));


            ulVal = BREG_Read32 (in_handle->regHandle,
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17);
            ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_093_MASK |
                        BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_092_MASK );
            BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17,
                (ulVal |
                    (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_093_SHIFT) |
                    (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_092_SHIFT)));
            BDBG_MSG(( "7422, need to switch io pin\n" ));

#elif(BCHP_CHIP==7346)
			ulVal = BREG_Read32 (in_handle->regHandle,
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8);
            ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_012_MASK |
                        BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_011_MASK |
                        BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_010_MASK |
                        BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_009_MASK |
                        BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_008_MASK );
            BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8,
                (ulVal |
                    (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_012_SHIFT) |
                    (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_011_SHIFT) |
                    (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_010_SHIFT) |
                    (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_009_SHIFT) |
                    (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_008_SHIFT ) ));
            BDBG_MSG(( "7346, need to switch io pin\n" ));
#elif(BCHP_CHIP==7231)
			ulVal = BREG_Read32 (in_handle->regHandle,
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9);
            ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_81_MASK |
                        BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_80_MASK |
                        BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_79_MASK );
            BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9,
                (ulVal |
                    (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_81_SHIFT) |
                    (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_80_SHIFT) |
                    (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_79_SHIFT) ));
			ulVal = BREG_Read32 (in_handle->regHandle,
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10);
            ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_83_MASK |
                        BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_82_MASK  );
            BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10,
                (ulVal |
                    (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_83_SHIFT) |
                    (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_82_SHIFT) ));
            BDBG_MSG(( "7231, need to switch io pin\n" ));
#elif(BCHP_CHIP==7358)||(BCHP_CHIP==7552)||(BCHP_CHIP==7360)
			ulVal = BREG_Read32 (in_handle->regHandle,
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10);
            ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_83_MASK |
                        BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_82_MASK |
                        BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_81_MASK |
                        BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_80_MASK |
                        BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_79_MASK );
            BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10,
                (ulVal |
                    (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_83_SHIFT) |
                    (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_82_SHIFT) |
                    (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_81_SHIFT) |
                    (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_80_SHIFT) |
                    (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_79_SHIFT ) ));
            BDBG_MSG(( "7358, need to switch io pin\n" ));
#elif(BCHP_CHIP==7344)
						ulVal = BREG_Read32 (in_handle->regHandle,
								BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0);
						ulVal &= ~(BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_02_MASK |
									BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_01_MASK |
									BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_00_MASK  );
						BREG_Write32 (in_handle->regHandle, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0,
							(ulVal |
								(0x00000004 << BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_02_SHIFT) |
								(0x00000004 << BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_01_SHIFT) |
								(0x00000004 << BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_00_SHIFT)  ));
						ulVal = BREG_Read32 (in_handle->regHandle,
								BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1);
						ulVal &= ~(BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_05_MASK |
									BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_04_MASK  );
						BREG_Write32 (in_handle->regHandle, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1,
							(ulVal |
								(0x00000004 << BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_05_SHIFT) |
								(0x00000004 << BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_04_SHIFT)  ));
						BDBG_MSG(( "7344, need to switch io pin\n" ));
#elif(BCHP_CHIP==7435)
			ulVal = BREG_Read32 (in_handle->regHandle,
							BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16);
					ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_091_MASK |
								BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_090_MASK |
								BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_089_MASK  );
					BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16,
						(ulVal |
							(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_091_SHIFT) |
							(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_090_SHIFT) |
							(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_089_SHIFT)  ));
					ulVal = BREG_Read32 (in_handle->regHandle,
							BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17);
					ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_092_MASK |
								BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_093_MASK  );
					BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17,
						(ulVal |
							(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_092_SHIFT) |
							(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_093_SHIFT)  ));
        BDBG_MSG(( "7435, need to switch io pin\n" ));
#elif(BCHP_CHIP==7429)
					ulVal = BREG_Read32 (in_handle->regHandle,
							BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10);
					ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_080_MASK |
								BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_081_MASK |
								BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_082_MASK |
								BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_083_MASK |
								BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_084_MASK |
								BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_085_MASK );
					BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10,
						(ulVal |
							(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_080_SHIFT) |
							(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_081_SHIFT) |
							(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_082_SHIFT) |
							(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_083_SHIFT ) ));
					ulVal = BREG_Read32 (in_handle->regHandle,
							BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9);
					ulVal &= ~BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_079_MASK;
					BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9,
						(ulVal |
							(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_079_SHIFT)));
#if 0
					ulVal = BREG_Read32 (in_handle->regHandle,
							BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7);
					ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_062_MASK |
								BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_063_MASK);
					BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7,ulVal); 
#endif
					BDBG_MSG(( "7429, need to switch io pin\n" ));
#elif(BCHP_CHIP==7584)
			ulVal = BREG_Read32 (in_handle->regHandle,
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10);
            ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_83_MASK |
                        BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_82_MASK |
                        BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_81_MASK |
                        BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_80_MASK |
                        BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_79_MASK );
            BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10,
                (ulVal |
                    (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_83_SHIFT) |
                    (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_82_SHIFT) |
                    (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_81_SHIFT) |
                    (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_80_SHIFT) |
                    (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_79_SHIFT ) ));
            BDBG_MSG(( "7584, need to switch io pin\n" ));


#endif
            break;
        case 1:
            intId = BCHP_INT_ID_scb_irqen;
            BREG_Write32(  in_handle->regHandle, BCHP_SCIRQ0_SCIRQEN,
                    (BCHP_SCIRQ0_SCIRQEN_scb_irqen_MASK |
                      BREG_Read32(in_handle->regHandle,  BCHP_SCIRQ0_SCIRQEN)) );

            /* Pin muxing */
#if (BCHP_CHIP==7422) || (BCHP_CHIP==7425)
            ulVal = BREG_Read32 (in_handle->regHandle,
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17);
            ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_098_MASK |
                        BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_097_MASK |
                        BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_096_MASK |
                        BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_095_MASK |
                        BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_094_MASK );
            BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17,
                (ulVal |
                    (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_098_SHIFT) |
                    (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_097_SHIFT) |
                    (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_096_SHIFT) |
                    (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_095_SHIFT) |
                    (0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_094_SHIFT ) ));
            BDBG_MSG(( "7422, need to switch io pin\n" ));
#elif (BCHP_CHIP==7346)
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10);
			ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_028_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_027_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_026_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_025_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10,
				(ulVal |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_028_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_027_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_026_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_025_SHIFT ) ));
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11);
			ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_029_MASK);
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11,
				(ulVal |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_029_SHIFT)  ));
			BDBG_MSG(( "7346, need to switch io pin\n" ));

#elif(BCHP_CHIP==7231)
			ulVal = BREG_Read32 (in_handle->regHandle,
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4);
            ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_37_MASK |
                        BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_36_MASK |
                        BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_35_MASK |
						 BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_34_MASK );
            BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4,
                (ulVal |
                    (0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_37_SHIFT) |
                    (0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_36_SHIFT) |
                     (0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_35_SHIFT) |
                    (0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_34_SHIFT) ));
			ulVal = BREG_Read32 (in_handle->regHandle,
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3);
            ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3_gpio_33_MASK  );
            BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3,
                (ulVal |
                    (0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3_gpio_33_SHIFT) ));
            BDBG_MSG(( "7231, need to switch io pin\n" ));
#elif(BCHP_CHIP==7358)||(BCHP_CHIP==7552)||(BCHP_CHIP==7360)
			ulVal = BREG_Read32 (in_handle->regHandle,
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4);
            ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_36_MASK |
                        BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_35_MASK |
                        BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_34_MASK |
						 BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_33_MASK );
            BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4,
                (ulVal |
                    (0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_33_SHIFT) |
                    (0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_36_SHIFT) |
                     (0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_35_SHIFT) |
                    (0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_34_SHIFT) ));
			ulVal = BREG_Read32 (in_handle->regHandle,
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5);
            ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_37_MASK  );
            BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5,
                (ulVal |
                    (0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_37_SHIFT) ));
            BDBG_MSG(( "7358, need to switch io pin\n" ));
#elif(BCHP_CHIP==7344)
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12);
			ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_56_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_55_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_54_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_53_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_52_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12,
				(ulVal |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_56_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_55_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_54_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_53_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_52_SHIFT ) ));
			BDBG_MSG(( "7344, need to switch io pin\n" ));
#elif(BCHP_CHIP==7435)
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17);
			ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_094_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_095_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_096_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_097_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_098_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17,
				(ulVal |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_094_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_095_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_096_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_097_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_098_SHIFT ) ));
			BDBG_MSG(( "7435, need to switch io pin\n" ));
#elif(BCHP_CHIP==7429)
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4);
			ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_037_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_036_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_035_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_034_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_033_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4,
				(ulVal |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_037_SHIFT) |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_036_SHIFT) |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_035_SHIFT) |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_034_SHIFT) |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_033_SHIFT ) ));
			BDBG_MSG(( "7429, need to switch io pin\n" ));
#elif(BCHP_CHIP==7584)
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12);
			ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_100_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_99_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_98_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_97_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_96_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12,
				(ulVal |
					(0x00000007 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_100_SHIFT) |
					(0x00000007 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_99_SHIFT) |
					(0x00000007 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_98_SHIFT) |
					(0x00000007 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_97_SHIFT) |
					(0x00000007 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_96_SHIFT ) ));
			BDBG_MSG(( "7584, need to switch io pin\n" ));

#endif

            break;


        default:
            errCode = BERR_INVALID_PARAMETER;
            goto BSCD_P_DONE_LABEL;
    }
    }



    BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
        BKNI_CreateEvent( &(channelHandle->channelWaitEvent.cardWait)));
    BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
        BKNI_CreateEvent( &(channelHandle->channelWaitEvent.tdoneWait)));
    BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
        BKNI_CreateEvent( &(channelHandle->channelWaitEvent.rcvWait)));
    BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
        BKNI_CreateEvent( &(channelHandle->channelWaitEvent.atrStart)));
    BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
        BKNI_CreateEvent( &(channelHandle->channelWaitEvent.timerWait)));

#ifdef BSCD_EMV2000_CWT_PLUS_4_EVENT_INTR
    BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
        BKNI_CreateEvent( &(channelHandle->channelWaitEvent.event1Wait)));
#endif

    BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
        BKNI_CreateEvent( &(channelHandle->channelWaitEvent.event2Wait)));

    BKNI_EnterCriticalSection();
    BSCD_Channel_P_EnableInterrupts_isr(channelHandle);   /* Todo:  Only enable intr for ATR */
    BKNI_LeaveCriticalSection();

    /*Set VCC level to inp_channelDefSettings->vcc*/
    if(  channelHandle->currentChannelSettings.setVcc){
        BSCD_Channel_SetVccLevel( channelHandle,channelHandle->currentChannelSettings.vcc);
    }

    in_handle->channelHandles[in_channelNo] = channelHandle;

    *outp_channelHandle = channelHandle;
    BKNI_EnterCriticalSection();
    channelHandle->bIsOpen = true;
    BKNI_LeaveCriticalSection();


	ulVal = BREG_Read32( 
		
	in_handle->regHandle, 
		(channelHandle->ulRegStartAddr + BSCD_P_STATUS_1));	

     if(ulVal&BCHP_SCA_SC_STATUS_1_card_pres_MASK)
	 	channelHandle->channelStatus.bCardPresent = true;
BSCD_P_DONE_LABEL:
    if( errCode != BERR_SUCCESS )
    {
        if( channelHandle != NULL )
        {
            if (channelHandle->channelWaitEvent.cardWait != NULL)
                BKNI_DestroyEvent( channelHandle->channelWaitEvent.cardWait );
            if (channelHandle->channelWaitEvent.tdoneWait != NULL)
                BKNI_DestroyEvent( channelHandle->channelWaitEvent.tdoneWait );
            if (channelHandle->channelWaitEvent.rcvWait != NULL)
                BKNI_DestroyEvent( channelHandle->channelWaitEvent.rcvWait );
            if (channelHandle->channelWaitEvent.atrStart != NULL)
                BKNI_DestroyEvent( channelHandle->channelWaitEvent.atrStart );
            if (channelHandle->channelWaitEvent.timerWait != NULL)
                BKNI_DestroyEvent( channelHandle->channelWaitEvent.timerWait );
#ifdef BSCD_EMV2000_CWT_PLUS_4_EVENT_INTR
            if (channelHandle->channelWaitEvent.atrStart != NULL)
                BKNI_DestroyEvent( channelHandle->channelWaitEvent.event1Wait );
#endif
            if (channelHandle->channelWaitEvent.timerWait != NULL)
                BKNI_DestroyEvent( channelHandle->channelWaitEvent.event2Wait );

#ifdef BCHP_PWR_RESOURCE_SMARTCARD0
	    /* a failed open releases power */
	    BCHP_PWR_ReleaseResource(channelHandle->moduleHandle->chipHandle, resourceId);
#endif

            BKNI_Free( channelHandle );

        }
    }

    BDBG_LEAVE(BSCD_Channel_Open);
    return( errCode );
}

BERR_Code BSCD_Channel_Close(
        BSCD_ChannelHandle inout_channelHandle
)
{
    BERR_Code errCode = BERR_SUCCESS;
    BSCD_Handle moduleHandle;
    uint32_t ulValue = 0;
       /* 12/02/2006 QX: add this to avoid crash during BKNI_Free */
     void *pTemp;
#ifdef BCHP_PWR_RESOURCE_SMARTCARD0
    BCHP_PWR_ResourceId resourceId = BCHP_PWR_RESOURCE_SMARTCARD0;
#ifdef BCHP_PWR_RESOURCE_SMARTCARD1
    if(inout_channelHandle->ucChannelNumber == 1) {
	resourceId = BCHP_PWR_RESOURCE_SMARTCARD1;
    }
#endif
#endif

    BDBG_ENTER(BSCD_Channel_Close);
    BDBG_ASSERT( inout_channelHandle );

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (inout_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (inout_channelHandle->bIsOpen ==  false) );


BSCD_P_DONE_LABEL:

    inout_channelHandle->bIsOpen = false;

    ulValue = BREG_Read32(inout_channelHandle->moduleHandle->regHandle,  BCHP_SCIRQ0_SCIRQEN);
    /*  Disable smartcard interrupt   */
    switch (inout_channelHandle->ucChannelNumber) {
        case 0:
            ulValue &= ~BCHP_SCIRQ0_SCIRQEN_sca_irqen_MASK;
            break;

        case 1:
            ulValue &= ~BCHP_SCIRQ0_SCIRQEN_scb_irqen_MASK;
            break;


        default:
            errCode = BERR_INVALID_PARAMETER;
            break;
    }


    BREG_Write32(inout_channelHandle->moduleHandle->regHandle,  BCHP_SCIRQ0_SCIRQEN,
                    ulValue);
    BSCD_P_CHECK_ERR_CODE_FUNC( errCode, BINT_DisableCallback( inout_channelHandle->channelIntCallback));
    BSCD_P_CHECK_ERR_CODE_FUNC( errCode, BINT_DestroyCallback( inout_channelHandle->channelIntCallback ) );

    BKNI_DestroyEvent( inout_channelHandle->channelWaitEvent.cardWait );
    BKNI_DestroyEvent( inout_channelHandle->channelWaitEvent.tdoneWait );
    BKNI_DestroyEvent( inout_channelHandle->channelWaitEvent.rcvWait );
    BKNI_DestroyEvent( inout_channelHandle->channelWaitEvent.atrStart );
    BKNI_DestroyEvent( inout_channelHandle->channelWaitEvent.timerWait );
#ifdef BSCD_EMV2000_CWT_PLUS_4_EVENT_INTR
    BKNI_DestroyEvent( inout_channelHandle->channelWaitEvent.event1Wait );
#endif
    BKNI_DestroyEvent( inout_channelHandle->channelWaitEvent.event2Wait );

    BSCD_Channel_Deactivate(inout_channelHandle);

    moduleHandle = inout_channelHandle->moduleHandle;
    moduleHandle->channelHandles[inout_channelHandle->ucChannelNumber] = NULL;

#ifdef BCHP_PWR_RESOURCE_SMARTCARD0
	/* release power */
	BCHP_PWR_ReleaseResource(inout_channelHandle->moduleHandle->chipHandle, resourceId);
#endif

     /* 12/02/2006 QX: add this to avoid crash during BKNI_Free */
    BKNI_EnterCriticalSection();
    pTemp = inout_channelHandle;
    inout_channelHandle = NULL;
    BKNI_LeaveCriticalSection();
    BKNI_Free(pTemp);

    BDBG_LEAVE(BSCD_Channel_Close);
    return( errCode );
}

BERR_Code BSCD_Channel_GetDevice(
        BSCD_ChannelHandle  in_channelHandle,
        BSCD_Handle         *outp_handle
)
{
    BERR_Code errCode = BERR_SUCCESS;

    BDBG_ENTER(BSCD_Channel_GetDevice);
    BDBG_ASSERT( in_channelHandle );

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (in_channelHandle->moduleHandle->bIsOpen ==  false) );

    *outp_handle = NULL;

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

    *outp_handle = in_channelHandle->moduleHandle;

BSCD_P_DONE_LABEL:
    BDBG_LEAVE(BSCD_Channel_GetDevice);
    return( errCode );
}


BERR_Code BSCD_GetChannel(
        BSCD_Handle         in_handle,
        unsigned int        in_channelNo,
        BSCD_ChannelHandle  *outp_channelHandle
)
{
    BERR_Code errCode = BERR_SUCCESS;
    BSCD_ChannelHandle channelHandle = NULL;

    BDBG_ENTER(BSCD_GetChannel);
    BDBG_ASSERT( in_handle );

    *outp_channelHandle = NULL;
    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (in_handle->ulMagicNumber != BSCD_P_HANDLE_MAGIC_NUMBER ) );

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BERR_INVALID_PARAMETER,
        (in_channelNo >= in_handle->currentSettings.ucMaxChannels) );

    channelHandle = in_handle->channelHandles[in_channelNo];

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (channelHandle == NULL ) );

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );


    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (channelHandle->bIsOpen ==  false) );

    *outp_channelHandle = channelHandle;

BSCD_P_DONE_LABEL:

    BDBG_LEAVE(BSCD_GetChannel);
    return( errCode );
}


BERR_Code BSCD_Channel_DetectCard(
        BSCD_ChannelHandle  in_channelHandle,
        BSCD_CardPresent    in_eCardPresent
)
{
    BERR_Code errCode = BERR_SUCCESS;

    BDBG_ENTER(BSCD_Channel_DetectCard);
    BDBG_ASSERT( in_channelHandle );

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (in_channelHandle->bIsOpen ==  false) );

    switch (in_eCardPresent) {
        case BSCD_CardPresent_eInserted:
        {
            BKNI_EnterCriticalSection();

            if ( in_channelHandle->ulStatus1 & BCHP_SCA_SC_STATUS_1_card_pres_MASK) {
                in_channelHandle->channelStatus.bCardPresent = true;
                BKNI_LeaveCriticalSection();
                goto BSCD_P_DONE_LABEL;
            }
            else {
                BDBG_MSG(("SmartCard Not Present"));
                BDBG_MSG(("Please insert the SmartCard"));
            }
            BKNI_LeaveCriticalSection();


            BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
                BSCD_Channel_P_WaitForCardInsertion(in_channelHandle));
        }
        break;

        case BSCD_CardPresent_eRemoved:
        {
            BKNI_EnterCriticalSection();

            if ( !(in_channelHandle->ulStatus1 & BCHP_SCA_SC_STATUS_1_card_pres_MASK)) {
                in_channelHandle->channelStatus.bCardPresent = false;
                BKNI_LeaveCriticalSection();
                goto BSCD_P_DONE_LABEL;
            }
            else {
                BDBG_MSG(("SmartCard Present"));
                BDBG_MSG(("Please remove the SmartCard"));
            }
            BKNI_LeaveCriticalSection();

            BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
                BSCD_Channel_P_WaitForCardRemove(in_channelHandle));
        }
        break;
    }

BSCD_P_DONE_LABEL:

    BDBG_LEAVE(BSCD_Channel_DetectCard);
    return( errCode );

}



BERR_Code BSCD_Channel_SetParameters(
        BSCD_ChannelHandle          in_channelHandle,
        const BSCD_ChannelSettings  *inp_sSettings
)
{
    BERR_Code errCode = BERR_SUCCESS;
    uint32_t        ulValue = 0;

    BDBG_ENTER(BSCD_Channel_SetParameters);
    BDBG_ASSERT( in_channelHandle );

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );


    in_channelHandle->currentChannelSettings.bIsPresHigh = inp_sSettings->bIsPresHigh;
     in_channelHandle->currentChannelSettings.setPinmux = inp_sSettings->setPinmux;
     in_channelHandle->currentChannelSettings.setVcc = inp_sSettings->setVcc;
     in_channelHandle->currentChannelSettings.vcc = inp_sSettings->vcc;
     in_channelHandle->currentChannelSettings.srcClkFreqInHz = inp_sSettings->srcClkFreqInHz;
	 in_channelHandle->currentChannelSettings.eResetCycles =inp_sSettings->eResetCycles;

     in_channelHandle->currentChannelSettings.bDirectRstInverted =inp_sSettings->bDirectRstInverted;
     in_channelHandle->currentChannelSettings.bDirectVccInverted =inp_sSettings->bDirectVccInverted;


	if(inp_sSettings->srcClkFreqInHz !=in_channelHandle->moduleHandle->currentSettings.moduleClkFreq.ulClkFreq){
		BDBG_MSG(("changing the clock frequency...\n")); 
		
     in_channelHandle->moduleHandle->currentSettings.moduleClkFreq.ulClkFreq =  inp_sSettings->srcClkFreqInHz;
    switch(inp_sSettings->srcClkFreqInHz){
        case 27000000:
            in_channelHandle->currentChannelSettings.eSrcClkFreq = BSCD_ClockFreq_e27MHZ;
            break;
        case 36864000:
            in_channelHandle->currentChannelSettings.eSrcClkFreq = BSCD_ClockFreq_e36P864MHZ;
            break;
        case 36000000:
            in_channelHandle->currentChannelSettings.eSrcClkFreq = BSCD_ClockFreq_e36MHZ;
            break;
        case 24000000:
            in_channelHandle->currentChannelSettings.eSrcClkFreq = BSCD_ClockFreq_e24MHZ;
            break;
        default:
            BDBG_ERR(("Unknown SC clock freq %d!\n",inp_sSettings->srcClkFreqInHz ));
            break;

    }
		BSCD_P_CHECK_ERR_CODE_FUNC(errCode, BSCD_Channel_P_SetSrcClock(
			in_channelHandle, inp_sSettings));


	 }

		
    BDBG_MSG(("bIsPresHigh = %d", in_channelHandle->currentChannelSettings.bIsPresHigh));

    if(in_channelHandle->currentChannelSettings.bIsPresHigh)
    {
        ulValue =  BCHP_SCA_SC_IF_CMD_1_pres_pol_MASK | BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1)) ;
        BREG_Write32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1),
             ulValue);
    }


    /*  Smart Card Standard */
    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        ((inp_sSettings->scStandard <= BSCD_Standard_eUnknown)  || (inp_sSettings->scStandard >= BSCD_Standard_eMax)) );
    in_channelHandle->currentChannelSettings.scStandard =   inp_sSettings->scStandard;
    BDBG_MSG(("scStandard = %d", in_channelHandle->currentChannelSettings.scStandard));

    BSCD_P_CHECK_ERR_CODE_FUNC(errCode, BSCD_Channel_P_SetStandard(
        in_channelHandle, inp_sSettings));

    BSCD_P_CHECK_ERR_CODE_FUNC(errCode, BSCD_Channel_P_SetFreq(
        in_channelHandle, inp_sSettings));

    /* Set maximum IFSD */
    in_channelHandle->currentChannelSettings.unMaxIFSD =  BSCD_MAX_TX_SIZE ;
    BDBG_MSG(("unMaxIFSD = %d", in_channelHandle->currentChannelSettings.unMaxIFSD));

    /* Set current IFSD */
    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
                (inp_sSettings->unCurrentIFSD > BSCD_MAX_TX_SIZE));
    if (inp_sSettings->unMaxIFSD == 0) {
        in_channelHandle->currentChannelSettings.unCurrentIFSD =  BSCD_MAX_TX_SIZE ;
    }
    else {
        in_channelHandle->currentChannelSettings.unCurrentIFSD =  inp_sSettings->unCurrentIFSD ;
    }
    BDBG_MSG(("unCurrentIFSD = %d", in_channelHandle->currentChannelSettings.unCurrentIFSD));

    BSCD_P_CHECK_ERR_CODE_FUNC(errCode, BSCD_Channel_P_SetEdcParity(
        in_channelHandle, inp_sSettings));

    BSCD_P_CHECK_ERR_CODE_FUNC(errCode, BSCD_Channel_P_SetWaitTime(
        in_channelHandle, inp_sSettings));

    BSCD_P_CHECK_ERR_CODE_FUNC(errCode, BSCD_Channel_P_SetGuardTime(
        in_channelHandle, inp_sSettings));

    /* Set transaction time out */
    BSCD_P_CHECK_ERR_CODE_FUNC(errCode, BSCD_Channel_P_SetTransactionTimeout(
        in_channelHandle, inp_sSettings));

    /* auto deactivation sequence */
    in_channelHandle->currentChannelSettings.bAutoDeactiveReq =  inp_sSettings->bAutoDeactiveReq;
    BDBG_MSG(("bAutoDeactiveReq = %d", in_channelHandle->currentChannelSettings.bAutoDeactiveReq));

    /* nullFilter */
    in_channelHandle->currentChannelSettings.bNullFilter =  inp_sSettings->bNullFilter;
    BDBG_MSG(("bNullFilter = %d", in_channelHandle->currentChannelSettings.bNullFilter));

    /* connectDirectly */
	in_channelHandle->currentChannelSettings.bConnectDirectly =  inp_sSettings->bConnectDirectly;
	BDBG_MSG(("bConnectDirectly = %d", in_channelHandle->currentChannelSettings.bConnectDirectly));


    /* debounce info */
    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
                (inp_sSettings->scPresDbInfo.ucDbWidth > BSCD_MAX_DB_WIDTH ));
    in_channelHandle->currentChannelSettings.scPresDbInfo =  inp_sSettings->scPresDbInfo;
    BDBG_MSG(("scPresDbInfo.bIsEnabled = %d", in_channelHandle->currentChannelSettings.scPresDbInfo.bIsEnabled));
    BDBG_MSG(("scPresDbInfo.ucDbWidth = %d", in_channelHandle->currentChannelSettings.scPresDbInfo.ucDbWidth));
    BDBG_MSG(("scPresDbInfo.scPresMode = %d", in_channelHandle->currentChannelSettings.scPresDbInfo.scPresMode));

    /* Specify if we want the driver to read, decode and program registers */
    in_channelHandle->currentChannelSettings.resetCardAction = inp_sSettings->resetCardAction;
    BDBG_MSG(("resetCardAction = %d", in_channelHandle->currentChannelSettings.resetCardAction));


    in_channelHandle->currentChannelSettings.ATRRecvTimeInteger =  inp_sSettings->ATRRecvTimeInteger;

    /* Update the BSCD_P_PRESCALE */
    ulValue = BREG_Read32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_PRESCALE));
    BDBG_MSG(("orig BSCD_P_PRESCALE = 0x%x\n", ulValue));



    BREG_Write32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_PRESCALE),
        in_channelHandle->currentChannelSettings.unPrescale);
    BDBG_MSG(("New BSCD_P_PRESCALE = 0x%x\n", in_channelHandle->currentChannelSettings.unPrescale));

#if 1
    /* Don't enable clock here since auto_clk need to be set first in ResetIFD before
         clock enabling for auto_deactivation */
    ulValue = BREG_Read32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_CLK_CMD));
    BDBG_MSG(("orig ucClkCmd = 0x%x\n",  ulValue));

    /* If enabled before, change the the value.  Otherwise leave it intact. */
    ulValue = ulValue & BCHP_SCA_SC_CLK_CMD_clk_en_MASK;
    if (ulValue == BCHP_SCA_SC_CLK_CMD_clk_en_MASK) {

        ulValue = ulValue | (BSCD_P_MapScClkDivToMaskValue(in_channelHandle->currentChannelSettings.ucScClkDiv))  |
                ((in_channelHandle->currentChannelSettings.ucEtuClkDiv - 1) << 1)  |
                ((in_channelHandle->currentChannelSettings.ucBaudDiv == 31) ? 0 : 1);

        BREG_Write32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_CLK_CMD),
            ulValue);
        if(in_channelHandle->currentChannelSettings.ucBaudDiv == 25){
                ulValue = BREG_Read32(
                    in_channelHandle->moduleHandle->regHandle,
                    (in_channelHandle->ulRegStartAddr + BSCD_P_FLOW_CMD));
                ulValue = 0x80 |ulValue;
                BREG_Write32(
                    in_channelHandle->moduleHandle->regHandle,
                    (in_channelHandle->ulRegStartAddr + BSCD_P_FLOW_CMD),
                    ulValue);
        }
        BDBG_MSG(("New SC_CLK_CMD = 0x%x\n", ulValue));
    }
#endif

    BDBG_MSG(("address  = 0x%x\n", in_channelHandle->ulRegStartAddr));
    BDBG_MSG(("BSCD_P_UART_CMD_2 address  = 0x%x\n", (in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_2)));
    /* Update the BSCD_P_UART_CMD_2 */
    ulValue = BREG_Read32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_2));
    BDBG_MSG(("orig BSCD_P_UART_CMD_2 = 0x%x\n",    ulValue));

    ulValue  &=  (BCHP_SCA_SC_UART_CMD_2_convention_MASK);

    if (inp_sSettings->eProtocolType == BSCD_AsyncProtocolType_e0 ) {

        ulValue |= (in_channelHandle->currentChannelSettings.ucRxRetries << BCHP_SCA_SC_UART_CMD_2_rpar_retry_SHIFT) |
                (in_channelHandle->currentChannelSettings.ucTxRetries);
    }
    else if ( (inp_sSettings->eProtocolType == BSCD_AsyncProtocolType_e1 )  ||
            (inp_sSettings->eProtocolType == BSCD_AsyncProtocolType_e14_IRDETO ) ) {
        /* No OP */ ;
    }
    BREG_Write32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_2),
        ulValue);

    BDBG_MSG(("BSCD_P_UART_CMD_2 = 0x%x\n",     ulValue));

    /* Update the BSCD_P_PROTO_CMD */
    ulValue =  BREG_Read32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_PROTO_CMD));
    if ((inp_sSettings->eProtocolType == BSCD_AsyncProtocolType_e1 ) &&
            (in_channelHandle->currentChannelSettings.edcSetting.bIsEnabled))  {
        ulValue =  BCHP_SCA_SC_PROTO_CMD_edc_en_MASK;

        if (in_channelHandle->currentChannelSettings.edcSetting.edcEncode == BSCD_EDCEncode_eLRC ) {
            ulValue &=  ~BCHP_SCA_SC_PROTO_CMD_crc_lrc_MASK;
        }
        else if (in_channelHandle->currentChannelSettings.edcSetting.edcEncode == BSCD_EDCEncode_eCRC) {
            ulValue |=  BCHP_SCA_SC_PROTO_CMD_crc_lrc_MASK;
        }
    }
    else {
        ulValue &=  ~BCHP_SCA_SC_PROTO_CMD_edc_en_MASK;
    }

    ulValue |= in_channelHandle->currentChannelSettings.ulCharacterWaitTimeInteger;

    BREG_Write32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_PROTO_CMD),
        ulValue);

    /* Update the BSCD_P_FLOW_CMD */
    ulValue = 0;
	/* flow control in enabled in TX/RV function, do not enable here */
	/*
    if (in_channelHandle->currentChannelSettings.scStandard == BSCD_Standard_eNDS) {
        ulValue =  BCHP_SCA_SC_FLOW_CMD_flow_en_MASK;
    }
    else {
        ulValue &=  ~BCHP_SCA_SC_FLOW_CMD_flow_en_MASK;
    }

*/
    BREG_Write32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_FLOW_CMD),
        ulValue);


    /* Update the BSCD_P_IF_CMD_2 */
    ulValue = 0;
    if (in_channelHandle->currentChannelSettings.scPresDbInfo.bIsEnabled == true) {
        ulValue =  BCHP_SCA_SC_IF_CMD_2_db_en_MASK;
    }
    else {
        ulValue &=  ~BCHP_SCA_SC_IF_CMD_2_db_en_MASK;
    }

    if (in_channelHandle->currentChannelSettings.scPresDbInfo.scPresMode == BSCD_ScPresMode_eMask) {
        ulValue |= BCHP_SCA_SC_IF_CMD_2_db_mask_MASK;
    }
    else if (in_channelHandle->currentChannelSettings.scPresDbInfo.scPresMode == BSCD_ScPresMode_eDebounce) {
        ulValue &= ~BCHP_SCA_SC_IF_CMD_2_db_mask_MASK;
    }

    ulValue |= in_channelHandle->currentChannelSettings.scPresDbInfo.ucDbWidth;

    BREG_Write32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_2),
        ulValue);

    /* Update the BSCD_P_TGUARD */
    BREG_Write32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_TGUARD),
        in_channelHandle->currentChannelSettings.extraGuardTime.ulValue);

    if( inp_sSettings->setVcc){
        BSCD_Channel_SetVccLevel( in_channelHandle,inp_sSettings->vcc);
    }

BSCD_P_DONE_LABEL:

    BDBG_LEAVE(BSCD_Channel_SetParameters);
    return( errCode );
}


BERR_Code BSCD_Channel_GetParameters(
        BSCD_ChannelHandle  in_channelHandle,
        BSCD_ChannelSettings    *outp_sSettings
)
{
    BERR_Code errCode = BERR_SUCCESS;

    BDBG_ENTER(BSCD_Channel_GetParameters);
    BDBG_ASSERT( in_channelHandle );

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

    *outp_sSettings = in_channelHandle->currentChannelSettings;

BSCD_P_DONE_LABEL:

    BDBG_LEAVE(BSCD_Channel_GetParameters);
    return( errCode );
}

char    BSCD_Channel_GetChannelNumber(
        BSCD_ChannelHandle  in_channelHandle
)
{
    BERR_Code errCode = BERR_SUCCESS;

    BDBG_ENTER(BSCD_Channel_GetChannelNumber);
    BDBG_ASSERT( in_channelHandle );

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

BSCD_P_DONE_LABEL:

    BDBG_LEAVE(BSCD_Channel_GetChannelNumber);
    if (errCode == BERR_SUCCESS)
        return( in_channelHandle->ucChannelNumber );
    else
        return  -1;
}


BERR_Code BSCD_Channel_Deactivate(
        BSCD_ChannelHandle          in_channelHandle
)
{
    BERR_Code errCode = BERR_SUCCESS;
    uint32_t    ulValue;

    BDBG_ENTER(BSCD_Channel_Deactivate);
    BDBG_ASSERT( in_channelHandle );

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

    BDBG_MSG(("In BSCD_Channel_Deactivate\n"));
#if !defined(HUMAX_PLATFORM_BASE)
    /* Disable all interrupts */
    BREG_Write32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_INTR_EN_1), 0);

    BREG_Write32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_INTR_EN_2), 0);
#endif

        if(in_channelHandle->currentChannelSettings.bConnectDirectly == true){
          /* Turn off VCC */
          ulValue =  BREG_Read32(
              in_channelHandle->moduleHandle->regHandle,
              (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1)) ;
	  if(in_channelHandle->currentChannelSettings.bDirectVccInverted == true)
     	ulValue |= BCHP_SCA_SC_IF_CMD_1_vcc_MASK; 
 	  else
          ulValue &= ~BCHP_SCA_SC_IF_CMD_1_vcc_MASK; 
        ulValue |= BCHP_SCA_SC_IF_CMD_1_io_MASK;
    }else{
    /* Turn off VCC */
    ulValue =  BCHP_SCA_SC_IF_CMD_1_vcc_MASK | BREG_Read32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1)) ;
        }
        ulValue &= ~BCHP_SCA_SC_IF_CMD_1_auto_vcc_MASK;
    BREG_Write32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1),
         ulValue);


        if(in_channelHandle->currentChannelSettings.bConnectDirectly == true){
          /* Set RST = 0.     */
		     if(in_channelHandle->currentChannelSettings.bDirectRstInverted ==true)
          ulValue =  BCHP_SCA_SC_IF_CMD_1_rst_MASK | BREG_Read32(
              in_channelHandle->moduleHandle->regHandle,
              (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1)) ;
			 else 
			 	ulValue =  (~BCHP_SCA_SC_IF_CMD_1_rst_MASK) & BREG_Read32(
                in_channelHandle->moduleHandle->regHandle,
                 (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1)) ;
        }
        else
         
        {
    /* Set RST = 0.     */
    ulValue =  (~BCHP_SCA_SC_IF_CMD_1_rst_MASK) & BREG_Read32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1)) ;
        }
       
        ulValue &= ~BCHP_SCA_SC_IF_CMD_1_auto_rst_MASK; 
    BREG_Write32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1),
         ulValue);

    /* Set CLK = 0.      */
    BREG_Write32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_CLK_CMD),
         0);
#if 0
    /* Set IO = 0.     This will cause IO line in an unknown state in wamr reset, comment out temparily */
    ulValue =  (BCHP_SCA_SC_IF_CMD_1_io_MASK) | BREG_Read32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1)) ;
    BREG_Write32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1),
         ulValue);
#endif 
    /* Reset Tx & Rx buffers.   */
    BREG_Write32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1),
         ~BCHP_SCA_SC_UART_CMD_1_io_en_MASK );

    BREG_Write32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_PROTO_CMD),
         BCHP_SCA_SC_PROTO_CMD_rbuf_rst_MASK | BCHP_SCA_SC_PROTO_CMD_tbuf_rst_MASK);

BSCD_P_DONE_LABEL:

    BDBG_LEAVE(BSCD_Channel_Deactivate);
    return( errCode );
}


BERR_Code BSCD_Channel_ResetIFD(
        BSCD_ChannelHandle  in_channelHandle,
        BSCD_ResetType      in_resetType

)
{
    BERR_Code errCode = BERR_SUCCESS;
    uint32_t    ulIFCmdVal = 0, ulValue;
    BSCD_Timer      timer = {BSCD_TimerType_eGPTimer, {BSCD_GPTimerMode_eIMMEDIATE}, true, true};
    BSCD_TimerValue    timeValue= {2, BSCD_TimerUnit_eETU};


    BDBG_ENTER(BSCD_Channel_ResetIFD);
    BDBG_ASSERT( in_channelHandle );

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

    /* Reset all status */
    in_channelHandle->ulStatus1 = 0;
    in_channelHandle->ulStatus2 = 0;
    in_channelHandle->ulIntrStatus1= 0;
    in_channelHandle->ulIntrStatus2= 0;

    in_channelHandle->channelStatus.ulStatus1 = 0;

      if (in_resetType == BSCD_ResetType_eCold) {

    in_channelHandle->channelStatus.bCardPresent = false;

    /* 09/20/05,Allen.C, reset bIsCardRemoved after card removed and reinitialize*/
    in_channelHandle->bIsCardRemoved = false;

       }
    /* Reset some critical registers */
    BREG_Write32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMD),
         0);

    BREG_Write32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_INTR_EN_1),
         0);

    BREG_Write32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_INTR_EN_2),
         0);

    BREG_Write32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1),
         0);

    BREG_Write32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_2),
         0);

    /* Set up debounce filter */
    if (in_channelHandle->currentChannelSettings.scPresDbInfo.bIsEnabled == true) {

        ulValue = BCHP_SCA_SC_IF_CMD_2_db_en_MASK;

        if (in_channelHandle->currentChannelSettings.scPresDbInfo.scPresMode == BSCD_ScPresMode_eMask) {
            ulValue |= BCHP_SCA_SC_IF_CMD_2_db_mask_MASK;
        }

        ulValue |= in_channelHandle->currentChannelSettings.scPresDbInfo.ucDbWidth;

        BREG_Write32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_2), ulValue);
    }
    else {
        BREG_Write32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_2), 0);
    }

    BDBG_MSG(("Inside Reset: Before Cold Reset ulIFCmdVal = 0x%x\n", ulIFCmdVal));
    /* Cold Reset or Warm Reset */
    if (in_resetType == BSCD_ResetType_eCold) {
        BDBG_MSG(("Cold Reset\n"));
        in_channelHandle->resetType = BSCD_ResetType_eCold;  /* Cold Reset */

   

                if(in_channelHandle->currentChannelSettings.bConnectDirectly == true){
#ifdef BSCD_DSS_ICAM
			if(in_channelHandle->currentChannelSettings.bDirectRstInverted ==true)

        	  	ulIFCmdVal = 0; 
			else
                  ulIFCmdVal = BCHP_SCA_SC_IF_CMD_1_rst_MASK; /*VCC L, RST H*/

			if(in_channelHandle->currentChannelSettings.bDirectVccInverted ==true)

        	  	ulIFCmdVal |= BCHP_SCA_SC_IF_CMD_1_vcc_MASK; 
			else
          		ulIFCmdVal &= ~BCHP_SCA_SC_IF_CMD_1_vcc_MASK; /*VCC L, RST H*/
#else
 		if(in_channelHandle->currentChannelSettings.bDirectRstInverted ==true)               
                  ulIFCmdVal = BCHP_SCA_SC_IF_CMD_1_rst_MASK; /*VCC L, RST L*/
		else
                  ulIFCmdVal = 0; /*VCC L, RST L*/
		if(in_channelHandle->currentChannelSettings.bDirectVccInverted ==true)

        	  	ulIFCmdVal |= BCHP_SCA_SC_IF_CMD_1_vcc_MASK; 
			else
          		ulIFCmdVal &= ~BCHP_SCA_SC_IF_CMD_1_vcc_MASK; /*VCC L, RST L*/
        ulIFCmdVal |= BCHP_SCA_SC_IF_CMD_1_io_MASK;
#endif

                }
                else{
#ifdef BSCD_DSS_ICAM
        ulIFCmdVal = BCHP_SCA_SC_IF_CMD_1_vcc_MASK | BCHP_SCA_SC_IF_CMD_1_rst_MASK;
#else
        ulIFCmdVal = BCHP_SCA_SC_IF_CMD_1_vcc_MASK;
#endif
        }
        BREG_Write32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1),
             ulIFCmdVal);
    /* waiting for TDA8024 to fully deactivate*/ 
    if(in_channelHandle->currentChannelSettings.bConnectDirectly == false) BKNI_Delay(20); 

		 /*Disable CLK*/
	BREG_Write32(
	in_channelHandle->moduleHandle->regHandle,
	(in_channelHandle->ulRegStartAddr + BSCD_P_CLK_CMD),
	 0);

    if(in_channelHandle->currentChannelSettings.bIsPresHigh){
        BDBG_MSG(("Change Presence Polarity\n"));

        ulIFCmdVal |= BCHP_SCA_SC_IF_CMD_1_pres_pol_MASK;
        BREG_Write32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1),
             ulIFCmdVal);
            BDBG_MSG(("Finished Change Presence Polarity\n"));
    	}


    }
    else {
        BDBG_MSG(("Warm Reset\n"));
        in_channelHandle->resetType = BSCD_ResetType_eWarm;  /* Warm Reset */

        ulIFCmdVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1));
    }
    BDBG_MSG(("Inside Reset: After Cold Reset ulIFCmdVal = 0x%x\n", ulIFCmdVal));

    /* Use Auto Deactivation instead of TDA8004 */
    if (in_channelHandle->currentChannelSettings.bAutoDeactiveReq == true) {

        BDBG_MSG(("Inside Reset: Before auto clk  BSCD_P_CLK_CMD = 0x%x\n",
                BREG_Read32(
                in_channelHandle->moduleHandle->regHandle,
                (in_channelHandle->ulRegStartAddr + BSCD_P_CLK_CMD))));

        ulIFCmdVal |= BCHP_SCA_SC_IF_CMD_1_auto_clk_MASK;
        BREG_Write32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1),
             ulIFCmdVal);
    }


if(in_channelHandle->currentChannelSettings.bConnectDirectly == true){
    /* wait for voltage on Vcc to completely low */
        BKNI_Sleep(BSCD_RESET_WAIT_TIME);

    ulIFCmdVal = BREG_Read32( in_channelHandle->moduleHandle->regHandle, (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1));
    /* Pull high Vcc, and start activation sequence */
	if(in_channelHandle->currentChannelSettings.bDirectVccInverted ==true)
        ulIFCmdVal &= ~BCHP_SCA_SC_IF_CMD_1_vcc_MASK;
	else
    ulIFCmdVal |= BCHP_SCA_SC_IF_CMD_1_vcc_MASK;

   #if 0
 ulIFCmdVal &= ~BCHP_SCA_SC_IF_CMD_1_io_MASK;
#endif
    if(in_channelHandle->currentChannelSettings.bAutoDeactiveReq == true){
      ulIFCmdVal |= BCHP_SCA_SC_IF_CMD_1_auto_vcc_MASK;
    }
    BREG_Write32( in_channelHandle->moduleHandle->regHandle, (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1), ulIFCmdVal);

    /* From Vcc goes high, TDA8024 expects 50-220 us for CLK start */
    BKNI_Delay(100);
    ulIFCmdVal &= ~BCHP_SCA_SC_IF_CMD_1_io_MASK;
    BREG_Write32( in_channelHandle->moduleHandle->regHandle, (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1), ulIFCmdVal);

}

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
	if(in_resetType == BSCD_ResetType_eCold)
	{
		BKNI_Sleep(20);
	}
#endif

    /* Set Clk cmd */
    ulValue = BCHP_SCA_SC_CLK_CMD_clk_en_MASK |
                (BSCD_P_MapScClkDivToMaskValue(in_channelHandle->currentChannelSettings.ucScClkDiv))  |
                ((in_channelHandle->currentChannelSettings.ucEtuClkDiv - 1) << 1)  |
                ((in_channelHandle->currentChannelSettings.ucBaudDiv == 31) ? 0 : 1);

    BDBG_MSG(("Reset: BCM_SC_CLK_CMD = 0x%lx\n", ulValue));

    BREG_Write32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_CLK_CMD),
         ulValue);

    BREG_Write32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_PRESCALE),
         in_channelHandle->currentChannelSettings.unPrescale);
    BDBG_MSG(("Reset: BSCD_P_PRESCALE = 0x%lx\n", in_channelHandle->currentChannelSettings.unPrescale));

    /* Use Auto Deactivation instead of TDA8004 */
    if (in_channelHandle->currentChannelSettings.bAutoDeactiveReq == true) {

        BDBG_MSG(("Inside Reset: Before auto io ulIFCmdVal = 0x%x\n", ulIFCmdVal));
        ulIFCmdVal |= BCHP_SCA_SC_IF_CMD_1_auto_io_MASK;
        BREG_Write32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1),
             ulIFCmdVal);
        BDBG_MSG(("Inside Reset: after auto io ulIFCmdVal = 0x%x\n", ulIFCmdVal));
    }

#if 0 /*ndef BSCD_DSS_ICAM*/
    if(in_channelHandle->currentChannelSettings.bConnectDirectly == false){

    ulIFCmdVal |= BCHP_SCA_SC_IF_CMD_1_rst_MASK;
    BREG_Write32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1),
             ulIFCmdVal);
        }
#endif


    ulValue = 0;
    BREG_Write32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1),
             ulValue);

    BDBG_MSG(("Inside Reset: Before SmartCardEnableInt\n"));

    /* Enable 2 interrupts with callback */
    BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
                BSCD_Channel_EnableIntrCallback_isr (
                in_channelHandle, BSCD_IntType_eCardInsertInt,
                       BSCD_Channel_P_CardInsertCB_isr));

    BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
                BSCD_Channel_EnableIntrCallback_isr (
                in_channelHandle, BSCD_IntType_eCardRemoveInt,
                       BSCD_Channel_P_CardRemoveCB_isr));

    BREG_Write32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1),
        BCHP_SCA_SC_UART_CMD_1_uart_rst_MASK);

    /******************************************************************
    **
    ** UART Reset should be set within 1 ETU (however, we are generous
    ** to give it 2 etus.
    **
    *****************************************************************/
    BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
        BSCD_Channel_ConfigTimer(in_channelHandle, &timer, &timeValue));

    BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
        BSCD_Channel_P_WaitForTimerEvent(in_channelHandle));

    /* Disable timer */
    timer.bIsTimerInterruptEnable = false;
    timer.bIsTimerEnable = false;
    BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
        BSCD_Channel_EnableDisableTimer_isr(in_channelHandle, &timer));

    ulValue = BREG_Read32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1));

    /* If equal to zero, then UART reset has gone low, so return success */
    if ((ulValue & BCHP_SCA_SC_UART_CMD_1_uart_rst_MASK) == 0) {
        BDBG_MSG(("Reset Success\n"));

        /*
        **   INITIAL_CWI_SC_PROTO_CMD = 0x0f is required so that
        **   CWI does not remain equal to zero, which causes an
        **   erroneous timeout, the CWI is set correctly in the
        **   SmartCardEMVATRDecode procedure
        */
        BREG_Write32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_PROTO_CMD),
             BCHP_SCA_SC_PROTO_CMD_tbuf_rst_MASK | BCHP_SCA_SC_PROTO_CMD_rbuf_rst_MASK);
    }

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
    /* check presence status again if card is already inserted. */
    if(BCHP_SCA_SC_STATUS_1_card_pres_MASK & BREG_Read32( in_channelHandle->moduleHandle->regHandle, (in_channelHandle->ulRegStartAddr + BSCD_P_STATUS_1)) )
        in_channelHandle->channelStatus.bCardPresent = true;
#endif

BSCD_P_DONE_LABEL:

    BDBG_LEAVE(BSCD_Channel_ResetIFD);
    return( errCode );
}


BERR_Code BSCD_Channel_PowerICC(
        BSCD_ChannelHandle          in_channelHandle,
        BSCD_PowerICC               in_iccAction
)
{
    BERR_Code errCode = BERR_SUCCESS;
    uint32_t ulValue;

    BDBG_ENTER(BSCD_Channel_PowerICC);
    BDBG_ASSERT( in_channelHandle );

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

    if(in_channelHandle->currentChannelSettings.bConnectDirectly == false){

        switch (in_iccAction) {
            case BSCD_PowerICC_ePowerDown:
                ulValue =  BCHP_SCA_SC_IF_CMD_1_vcc_MASK | BREG_Read32(
                in_channelHandle->moduleHandle->regHandle,
                (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1)) ;
				ulValue &= ~BCHP_SCA_SC_IF_CMD_1_rst_MASK;
                BREG_Write32(
                    in_channelHandle->moduleHandle->regHandle,
                    (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1),
                    ulValue);
                /* QX delay to let TDA finished */
                BKNI_Delay(20);
            break;
        case BSCD_PowerICC_ePowerUp:
            ulValue =  BREG_Read32(
                in_channelHandle->moduleHandle->regHandle,
                (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1)) ;
            BREG_Write32(
                in_channelHandle->moduleHandle->regHandle,
                (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1),
                 (ulValue & ~BCHP_SCA_SC_IF_CMD_1_vcc_MASK));
            break;

        default:
            BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);
            break;
        }
        }else{
        switch (in_iccAction) {
                    case BSCD_PowerICC_ePowerUp:
                        ulValue =  BCHP_SCA_SC_IF_CMD_1_vcc_MASK | BREG_Read32(
                        in_channelHandle->moduleHandle->regHandle,
                        (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1)) ;
    if(in_channelHandle->currentChannelSettings.bAutoDeactiveReq == true){
      ulValue |= BCHP_SCA_SC_IF_CMD_1_auto_vcc_MASK;
    }
                        BREG_Write32(
                            in_channelHandle->moduleHandle->regHandle,
                            (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1),
                            ulValue);
                    break;
                case BSCD_PowerICC_ePowerDown:
                    ulValue =  BREG_Read32(
                        in_channelHandle->moduleHandle->regHandle,
                        (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1)) ;
                        ulValue &= ~BCHP_SCA_SC_IF_CMD_1_auto_vcc_MASK;                          
                    BREG_Write32(
                        in_channelHandle->moduleHandle->regHandle,
                        (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1),
                         (ulValue & ~BCHP_SCA_SC_IF_CMD_1_vcc_MASK));
                    break;
        
                default:
                    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);
                    break;
                }

        }


BSCD_P_DONE_LABEL:

    BDBG_LEAVE(BSCD_Channel_ResetIFD);
    return( errCode );
}

BERR_Code BSCD_Channel_SetVccLevel(
        BSCD_ChannelHandle          in_channelHandle,
        BSCD_VccLevel                   in_vccLevel
)
{
    BERR_Code errCode = BERR_SUCCESS;
    uint32_t ulValue = 0;

	BSTD_UNUSED(ulValue);
    BDBG_ENTER(BSCD_Channel_SetVccLevel);
    BDBG_ASSERT( in_channelHandle );

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );


    /***************************************************************
        Warning:  Note that we have to modify the board to use VPP pin of NDS
                ICAM smartcard and connect it to pin 3 (3V/5V) of TDA chip
                and run this function.  Make sure to disconnect your QAM or
                QPSK connection before calling this function or your smartcard
                will be damaged.
    ***************************************************************/
    BDBG_MSG(("BSCD_Channel_SetVccLevel: in_vccLevel = 0x%x\n", in_vccLevel));

    BKNI_EnterCriticalSection();
	in_channelHandle->currentChannelSettings.vcc = in_vccLevel;

#if (BCHP_CHIP==7422) || (BCHP_CHIP==7425)
    ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,
                        BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14);
    ulValue &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_074_MASK);

    BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14,ulValue );

    ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_IODIR_EXT); 
                ulValue &=  ~(1<<(74-64+6));     /* include sgpio */
                BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_IODIR_EXT, ulValue );     
#elif (BCHP_CHIP == 7346)
	switch( in_channelHandle->ucChannelNumber){
		case 0:
			ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,
				BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9);
			ulValue &= ~BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_015_MASK;
			BREG_Write32 (in_channelHandle->moduleHandle->regHandle, 
				BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9, ulValue);
	
			ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_IODIR_LO);	
			ulValue &=	~(1<<15);     
	 
			BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_IODIR_LO, ulValue );	
			break;
		case 1:
			ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,
				BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11);
			ulValue &= ~BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_032_MASK;
			BREG_Write32 (in_channelHandle->moduleHandle->regHandle, 
				BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, ulValue);
	
			ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_IODIR_HI);	
			ulValue &=	~(1);     
	 
			BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_IODIR_HI, ulValue );	
			break;
	}
#elif (BCHP_CHIP == 7231)
	switch( in_channelHandle->ucChannelNumber){
		case 0:
			ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,
				BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10);
			ulValue &= ~BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_86_MASK;
			BREG_Write32 (in_channelHandle->moduleHandle->regHandle, 
				BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, ulValue);
	
			ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_IODIR_EXT);	
			ulValue &=	~(1<<(86-64+6));     
			BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_IODIR_EXT, ulValue );	
			break;
		case 1:
			BDBG_WRN(("Slot 1 not supported in BRCM reference board, please use customer callback function to set the correct voltage\n")); 
			break;
	}
#elif (BCHP_CHIP == 7358)||(BCHP_CHIP==7552)||(BCHP_CHIP==7360)
	switch( in_channelHandle->ucChannelNumber){
		case 0:
			ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,
				BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11);
			ulValue &= ~BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_86_MASK;
			BREG_Write32 (in_channelHandle->moduleHandle->regHandle, 
				BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, ulValue);
	
			ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_IODIR_EXT); 
			ulValue &=	~(1<<(86-64));	   
	 
			BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_IODIR_EXT, ulValue ); 
			break;
		case 1:
			BDBG_WRN(("Slot 1 not supported in BRCM reference board, please use customer callback function to set the correct voltage\n")); 
			break;
	}
#elif (BCHP_CHIP == 7344)
	switch( in_channelHandle->ucChannelNumber){
		case 0:
			ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,
					BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1);
			ulValue &= ~BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_08_MASK ;
			
			BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1,ulValue );
			ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_AON_IODIR_LO); 
			ulValue &=	~(1<<(8));	   
			BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_AON_IODIR_LO,ulValue );
			break;
		case 1:
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_MULTI_SMARTCARD)
			ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13);
			ulValue &= ~BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_22_MASK ;

			BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13,ulValue );
			ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_IODIR_LO); 
			ulValue &=	~(1<<(22));	   
			BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_AON_IODIR_LO,ulValue );
#else
			BDBG_WRN(("Slot 1 not supported in BRCM reference board, please use customer callback function to set the correct voltage\n")); 
#endif
			break;
	}

#elif (BCHP_CHIP == 7429)
	switch( in_channelHandle->ucChannelNumber){
		case 0:
			ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10);
			ulValue &= ~BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_086_MASK ;
			
			BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10,ulValue );
			ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_IODIR_EXT); 
			ulValue &=	~(1<<(86-64+4));	   
			BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_IODIR_EXT,ulValue );
			break;
		case 1:
			BDBG_WRN(("Slot 1 not supported in BRCM reference board, please use customer callback function to set the correct voltage\n")); 
			break;
		}

#else
	BDBG_WRN(("Chip is not supported!\n"));
#endif

    switch (in_vccLevel) {
        case BSCD_VccLevel_e3V:

#if (BCHP_CHIP==7422) || (BCHP_CHIP==7425)
            ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_DATA_EXT);
            ulValue &=  ~(1<<(74-64+6)); 
            BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_DATA_EXT, ulValue );
#elif (BCHP_CHIP == 7346)
			switch( in_channelHandle->ucChannelNumber){
				case 0:
					ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_DATA_LO);	
					ulValue &=	~(1<<15);     
			 
					BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_DATA_LO, ulValue );
					break;
				case 1:
					ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_DATA_HI);	
					ulValue &=	~(1);     
			 
					BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_DATA_HI, ulValue );	
					break;
			}
#elif (BCHP_CHIP==7231) 
            ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_DATA_EXT);
            ulValue &=  ~(1<<(86-64+6));
            BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_DATA_EXT, ulValue );
#elif (BCHP_CHIP==7358) ||(BCHP_CHIP==7552)||(BCHP_CHIP==7360)
						ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_DATA_EXT);
						ulValue &=	~(1<<(86-64)); 
						BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_DATA_EXT, ulValue );
#elif (BCHP_CHIP==7429) 
			ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_DATA_EXT);
			ulValue &=	~(1<<(86-64+4)); 
			BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_DATA_EXT, ulValue );
#elif (BCHP_CHIP==7344)
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_MULTI_SMARTCARD)
			switch( in_channelHandle->ucChannelNumber){
			case 0:
				ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_AON_DATA_LO);
				ulValue &=	~(1<<(8)); 
				BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_AON_DATA_LO, ulValue );
				break;
			case 1:				
				ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_DATA_LO);
				ulValue &=	~(1<<(22)); 
				BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_DATA_LO, ulValue );
				break;
			default:
				break;
			}
#else
			ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_AON_DATA_LO);
			ulValue &=	~(1<<(8)); 
			BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_AON_DATA_LO, ulValue );
#endif			
#else
				BDBG_WRN(("Chip is not supported!\n"));
#endif
            break;

        case BSCD_VccLevel_e5V:
#if (BCHP_CHIP==7422) || (BCHP_CHIP==7425)
            ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_DATA_EXT);
            ulValue |=  (1<<(74-64+6)); 
            BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_DATA_EXT, ulValue );
#elif (BCHP_CHIP == 7346)
			switch( in_channelHandle->ucChannelNumber){
				case 0:
					ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_DATA_LO);	
					ulValue |=	1<<15;     
			 
					BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_DATA_LO, ulValue );	
					break;
				case 1:
					ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_DATA_HI);	
					ulValue |=	1;     
			 
					BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_DATA_HI, ulValue );	
					break;
			}
#elif (BCHP_CHIP==7231) 
			ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_DATA_EXT);
			ulValue |=	1<<(86-64+6); 
			BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_DATA_EXT, ulValue );
#elif (BCHP_CHIP==7358) ||(BCHP_CHIP==7552)||(BCHP_CHIP==7360)
									ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_DATA_EXT);
									ulValue |=	1<<(86-64); 
									BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_DATA_EXT, ulValue );
#elif (BCHP_CHIP==7344)
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_MULTI_SMARTCARD)
			switch( in_channelHandle->ucChannelNumber){
			case 0:
				ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_AON_DATA_LO);
				ulValue |=	(1<<(8)); 
				BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_AON_DATA_LO, ulValue );
				break;
			case 1:
				ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_DATA_LO);
				ulValue |=	(1<<(22)); 
				BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_DATA_LO, ulValue );
				break;
			default:
				break;
			}
#else
			ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_AON_DATA_LO);
			ulValue |=	(1<<(8)); 
			BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_AON_DATA_LO, ulValue );
#endif
#elif (BCHP_CHIP==7429) 
            ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_DATA_EXT);
            ulValue |=  (1<<(86-64+4)); 
            BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_DATA_EXT, ulValue );

#else
				BDBG_WRN(("Chip is not supported!\n"));

#endif
            break;

		case  BSCD_VccLevel_e1P8V:
			BDBG_WRN(("1.8V is not supported in BRCM ref board, please add customer code here.\n"));
			break;

		default:
            errCode = BERR_TRACE(BSCD_STATUS_FAILED);
            BDBG_ERR(("BSCD_Channel_SetVccLevel: Do not support VCC Level switch = 0x%x, \n", in_vccLevel));
            goto BSCD_P_DONE_LABEL;
    }






BSCD_P_DONE_LABEL:
    BKNI_LeaveCriticalSection();
    BDBG_LEAVE(BSCD_Channel_SetVccLevel);
    return( errCode );

}

BERR_Code BSCD_Channel_InsCardHwReset(
        BSCD_ChannelHandle          in_channelHandle,
        bool                          in_enableHwRst
)
{
    BERR_Code errCode = BERR_SUCCESS;
    uint32_t ulValue = 0;

    BDBG_ENTER(BSCD_Channel_InsCardHwReset);
    BDBG_ASSERT( in_channelHandle );

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

#if   !defined(LINUX) && ((BCHP_CHIP==7400) )
    ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_SUN_TOP_CTRL_RESET_CTRL);

        switch (in_channelHandle->ucChannelNumber) {
            case 0:
                if (  in_enableHwRst ) /*Enable Inser Card Hardware Reset*/
                    ulValue |= ( 1 << BCHP_SUN_TOP_CTRL_RESET_CTRL_sc_insert_reset_en_SHIFT );
                else
                    ulValue &= ~( 1 << BCHP_SUN_TOP_CTRL_RESET_CTRL_sc_insert_reset_en_SHIFT );

                break;

            case 1:
                if (  in_enableHwRst ) /*Enable Inser Card Hardware Reset*/
                    ulValue |= ( 2 << BCHP_SUN_TOP_CTRL_RESET_CTRL_sc_insert_reset_en_SHIFT );
                else
                    ulValue &= ~( 2 << BCHP_SUN_TOP_CTRL_RESET_CTRL_sc_insert_reset_en_SHIFT );

                break;

            case 2:
                if (  in_enableHwRst ) /*Enable Inser Card Hardware Reset*/
                    ulValue |= ( 4 << BCHP_SUN_TOP_CTRL_RESET_CTRL_sc_insert_reset_en_SHIFT );
                else
                    ulValue &= ~( 4 << BCHP_SUN_TOP_CTRL_RESET_CTRL_sc_insert_reset_en_SHIFT );

                break;

            default:
                return -1;
        }

    BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_RESET_CTRL, ulValue);

#elif   !defined(LINUX) && (BCHP_CHIP==7401)
    ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_SUN_TOP_CTRL_RESET_CTRL);

        switch (in_channelHandle->ucChannelNumber) {
            case 0:
                if (  in_enableHwRst ) /*Enable Inser Card Hardware Reset*/
                    ulValue |= ( 1 << BCHP_SUN_TOP_CTRL_RESET_CTRL_sc_insert_reset_en_SHIFT );
                else
                    ulValue &= ~( 1 << BCHP_SUN_TOP_CTRL_RESET_CTRL_sc_insert_reset_en_SHIFT );

                break;

            case 1:
                if (  in_enableHwRst ) /*Enable Inser Card Hardware Reset*/
                    ulValue |= ( 2 << BCHP_SUN_TOP_CTRL_RESET_CTRL_sc_insert_reset_en_SHIFT );
                else
                    ulValue &= ~( 2 << BCHP_SUN_TOP_CTRL_RESET_CTRL_sc_insert_reset_en_SHIFT );

                break;

            default:
                return -1;
        }

    BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_RESET_CTRL, ulValue);
/*#elif !defined(LINUX) &&  (defined(BCM97038) || defined(BCM7038)) */
#elif !defined(LINUX) &&   ((BCHP_CHIP==7038) || (BCHP_CHIP==7438) )

    ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_SUN_TOP_CTRL_RESET_CTRL);

    switch (in_channelHandle->ucChannelNumber) {
        case 2:
            if (  in_enableHwRst ) /*Enable Inser Card Hardware Reset*/
                ulValue |= BCHP_SUN_TOP_CTRL_RESET_CTRL_sc2_insert_reset_en_MASK;
            else
                ulValue &= ~(BCHP_SUN_TOP_CTRL_RESET_CTRL_sc2_insert_reset_en_MASK);
            break;

        case 1:
            if (  in_enableHwRst ) /*Enable Inser Card Hardware Reset*/
                ulValue |= BCHP_SUN_TOP_CTRL_RESET_CTRL_sc1_insert_reset_en_MASK;
            else
                ulValue &= ~(BCHP_SUN_TOP_CTRL_RESET_CTRL_sc1_insert_reset_en_MASK);
            break;

        case 0:
            if (  in_enableHwRst ) /*Enable Inser Card Hardware Reset*/
                ulValue |= BCHP_SUN_TOP_CTRL_RESET_CTRL_sc0_insert_reset_en_MASK;
            else
                ulValue &= ~(BCHP_SUN_TOP_CTRL_RESET_CTRL_sc0_insert_reset_en_MASK);
            break;

        default:
            return -1;
    }

    BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_RESET_CTRL, ulValue);

#elif !defined(LINUX) &&  (BCHP_CHIP==3560)

    ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_SUN_TOP_CTRL_RESET_CTRL);

    switch (in_channelHandle->ucChannelNumber) {
        case 0:
            if (  in_enableHwRst ) /*Enable Inser Card Hardware Reset*/
                ulValue |= BCHP_SUN_TOP_CTRL_RESET_CTRL_sc0_insert_reset_en_MASK;
            else
                ulValue &= ~(BCHP_SUN_TOP_CTRL_RESET_CTRL_sc0_insert_reset_en_MASK);
            break;

        default:
            return -1;
    }

    BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_RESET_CTRL, ulValue);
#else
    BSTD_UNUSED( ulValue );
    BSTD_UNUSED( in_enableHwRst);

#endif


BSCD_P_DONE_LABEL:

    BDBG_LEAVE(BSCD_Channel_InsCardHwReset);
    return( errCode );

}

BERR_Code BSCD_Channel_ResetCard(
        BSCD_ChannelHandle          in_channelHandle,
        BSCD_ResetCardAction               in_iccAction
)
{
    BERR_Code errCode = BERR_SUCCESS;

    BDBG_ENTER(BSCD_Channel_ResetCard);
    BDBG_ASSERT( in_channelHandle );

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

    if( in_channelHandle->currentChannelSettings.setVcc){

	if(BSCD_Channel_SetVccLevel(in_channelHandle,
				in_channelHandle->currentChannelSettings.vcc)){
				return BERR_UNKNOWN; 
				};
    	}
    switch (in_iccAction) {
        case BSCD_ResetCardAction_eNoAction:
            if (  (errCode = BSCD_Channel_P_Activating(in_channelHandle)) != BERR_SUCCESS) {
                   errCode = BERR_TRACE(errCode);
                goto BSCD_P_DONE_LABEL;
            }
            break;
        case BSCD_ResetCardAction_eReceiveAndDecode:
            BDBG_MSG(("BSCD_ResetCardAction_eReceiveAndDecode \n"));
            if (  ((errCode = BSCD_Channel_P_Activating(in_channelHandle)) != BERR_SUCCESS) ||
                   ((errCode = BSCD_Channel_P_ReceiveAndDecode(in_channelHandle)) != BERR_SUCCESS) ) {
                   errCode = BERR_TRACE(errCode);
                goto BSCD_P_DONE_LABEL;
            }
            break;

        default:
            BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);
            break;
    }


BSCD_P_DONE_LABEL:

    BDBG_LEAVE(BSCD_Channel_ResetCard);
    BDBG_MSG(("Leave ResetCard erroCode = 0x%x\n", errCode));
    return( errCode );
}


BERR_Code BSCD_Channel_GetStatus(
        BSCD_ChannelHandle          in_channelHandle,
        BSCD_Status                 *outp_status
)
{
    BERR_Code errCode = BERR_SUCCESS;

    BDBG_ENTER(BSCD_Channel_GetStatus);
    BDBG_ASSERT( in_channelHandle );

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

    *outp_status = in_channelHandle->channelStatus;

BSCD_P_DONE_LABEL:

    BDBG_LEAVE(BSCD_Channel_GetStatus);
    return( errCode );
}

BERR_Code BSCD_Channel_Transmit(
        BSCD_ChannelHandle          in_channelHandle,
        uint8_t                     *inp_ucXmitData,
        unsigned long                    in_ulNumXmitBytes
)
{
    if (in_channelHandle->currentChannelSettings.scStandard == BSCD_Standard_eIrdeto) {
        return (BSCD_Channel_P_T14IrdetoTransmit(
                in_channelHandle,
                inp_ucXmitData,
                in_ulNumXmitBytes));
    }
    else {
        return (BSCD_Channel_P_T0T1Transmit(
                in_channelHandle,
                inp_ucXmitData,
                in_ulNumXmitBytes));
    }
}


BERR_Code BSCD_Channel_Receive(
        BSCD_ChannelHandle       in_channelHandle,
        uint8_t                         *outp_ucRcvData,
        unsigned long                  *outp_ulNumRcvBytes,
        unsigned long                    in_ulMaxReadBytes
)
{
    BERR_Code       errCode = BSCD_STATUS_READ_SUCCESS;
#ifndef BSCD_DSS_ICAM
    BSCD_Timer      timer = {BSCD_TimerType_eWaitTimer, {BSCD_WaitTimerMode_eWorkWaitTime}, false, false};
#endif

    BDBG_ENTER(BSCD_Channel_Receive);
    BDBG_ASSERT( in_channelHandle );

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

    *outp_ulNumRcvBytes = 0;

    /* Coverity: 35203 */
    if(outp_ucRcvData == NULL) {
        BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);
    }

    if ((in_channelHandle->currentChannelSettings.eProtocolType == BSCD_AsyncProtocolType_e0) ||
        (in_channelHandle->currentChannelSettings.eProtocolType == BSCD_AsyncProtocolType_e14_IRDETO) ) {
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
		if (in_channelHandle->currentChannelSettings.eProtocolType == BSCD_AsyncProtocolType_e0)
		{
        	BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
                    BSCD_Channel_P_T0ReadData(in_channelHandle, outp_ucRcvData, outp_ulNumRcvBytes,
                            in_ulMaxReadBytes));
		}
		else
		{
			BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
						BSCD_Channel_P_T14ReadData(in_channelHandle, outp_ucRcvData, outp_ulNumRcvBytes,
								in_ulMaxReadBytes));
		}
#else
        BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
                    BSCD_Channel_P_T0ReadData(in_channelHandle, outp_ucRcvData, outp_ulNumRcvBytes,
                            in_ulMaxReadBytes));
#endif

        /*
            The Work Wait Timer is enabled in BSCD_Channel_P_T0T1Transmit. We cannot disable
            it in BSCD_Channel_P_T0ReadData since BSCD_Channel_P_T0ReadData is also used
            by reading ATR, which is one byte at a time.
        */

#ifndef BSCD_DSS_ICAM   /* BSYT leave this WWT enabled. We only disable WWT in transmit. */
        /*
            I assume all standards, other than EMV, will read all the bytes in BSCD_Channel_P_T0ReadData,
            therefore we couold safely disable the WWT here.  EMV only read 1 bytes at a time, therefore
            we have to disable WWT in the application
        */
        if ((in_channelHandle->currentChannelSettings.scStandard != BSCD_Standard_eEMV1996) &&
            (in_channelHandle->currentChannelSettings.scStandard != BSCD_Standard_eEMV2000) )
        BSCD_P_CHECK_ERR_CODE_FUNC2(errCode, BSCD_STATUS_READ_FAILED,
            BSCD_Channel_EnableDisableTimer_isr(in_channelHandle, &timer));
#endif


    }/* BSCD_AsyncProtocolType_e0 */

    else { /* BSCD_AsyncProtocolType_e1 */

        BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
                    BSCD_Channel_P_T1ReadData(in_channelHandle, outp_ucRcvData, outp_ulNumRcvBytes,
                            in_ulMaxReadBytes));

    } /* BSCD_AsyncProtocolType_e1 */

    if (*outp_ulNumRcvBytes > 0) {

        /* Ignore the ReadTimeOut error returned by SmartCardByteRead */
        /* BDBG_MSG (("success in SmartCardReadCmd\n")); */
    }

    else {
        BDBG_MSG (("No Response detected...deactivating, scerr = %02x\n",errCode));
        BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);
    }



BSCD_P_DONE_LABEL:

    BDBG_LEAVE(BSCD_Channel_Receive);
    /* BDBG_MSG(("Leave BSCD_Channel_Receive = 0x%x\n", errCode)); */
    return( errCode );
}

BERR_Code BSCD_Channel_ReceiveATR(
        BSCD_ChannelHandle       in_channelHandle,
        uint8_t                         *outp_ucRcvData,
        unsigned long                  *outp_ulNumRcvBytes,
        unsigned long                    in_ulMaxReadBytes
)
{
    BERR_Code       errCode = BSCD_STATUS_READ_SUCCESS;

    BDBG_ENTER(BSCD_Channel_ReceiveATR);
    BDBG_ASSERT( in_channelHandle );

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

    *outp_ulNumRcvBytes = 0;

    if(outp_ucRcvData == NULL) {
        BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);
    }

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_CAS_NA_ISO7816)
	/* james nagra : use T0ReadATR for ATR reading	instead of normal T0read func*/
	BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
				BSCD_Channel_P_T0ReadDataATR(in_channelHandle, outp_ucRcvData, outp_ulNumRcvBytes,
						in_ulMaxReadBytes));
#else
	BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
				BSCD_Channel_P_T0ReadData(in_channelHandle, outp_ucRcvData, outp_ulNumRcvBytes,
						in_ulMaxReadBytes));
#endif


    if (*outp_ulNumRcvBytes > 0) {

        /*
        For T=0, we depend on timeout to
        identify that there is no more byte to be received
        */

        /* Ignore the ReadTimeOut error returned by SmartCardByteRead */
        /* BDBG_MSG (("success in SmartCardReadCmd\n")); */
    }

    else {
        BDBG_MSG (("No Response detected...deactivating, scerr = %02x\n",errCode));
        BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);
    }



BSCD_P_DONE_LABEL:

    BDBG_LEAVE(BSCD_Channel_ReceiveATR);
    /* BDBG_MSG(("Leave BSCD_Channel_ReceiveATR = 0x%x\n", errCode)); */
    return( errCode );
}

BERR_Code BSCD_Channel_ConfigTimer(
        BSCD_ChannelHandle          in_channelHandle,
        BSCD_Timer                  *inp_timer,
        BSCD_TimerValue             *inp_unCount

)
{
    BERR_Code errCode = BERR_SUCCESS;
    uint32_t            ulTimerCmdVal, ulTimerCmpVal;

    BDBG_ENTER(BSCD_Channel_ConfigTimer);
    BDBG_ASSERT( in_channelHandle );

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

    if (inp_timer->eTimerType == BSCD_TimerType_eGPTimer) {

        /* Always disbale timer first before we change timer_cmd */
        ulTimerCmdVal = BREG_Read32(
                    in_channelHandle->moduleHandle->regHandle,
                    (in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMD));

        ulTimerCmdVal &= (~BCHP_SCA_SC_TIMER_CMD_timer_en_MASK);

        BREG_Write32(
                    in_channelHandle->moduleHandle->regHandle,
                    (in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMD),
                    ulTimerCmdVal);

        BKNI_EnterCriticalSection();
        in_channelHandle->ulIntrStatus1  &= ~BCHP_SCA_SC_INTR_STAT_1_timer_intr_MASK;
        BKNI_LeaveCriticalSection();

        /* Set timer_cmp registers */

        ulTimerCmpVal = ((inp_unCount->ulValue & 0xFF00) >> 8);
        BREG_Write32(
                    in_channelHandle->moduleHandle->regHandle,
                    (in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMP_2),
                    ulTimerCmpVal);

        ulTimerCmpVal = inp_unCount->ulValue & 0x00FF;
        BREG_Write32(
                    in_channelHandle->moduleHandle->regHandle,
                    (in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMP_1),
                    ulTimerCmpVal);


        /* Set the timer unit and mode */
        if ( inp_unCount->unit == BSCD_TimerUnit_eCLK) {
            ulTimerCmdVal |= BCHP_SCA_SC_TIMER_CMD_timer_src_MASK;
        }
        else if (inp_unCount->unit  == BSCD_TimerUnit_eETU) {
            ulTimerCmdVal &= (~BCHP_SCA_SC_TIMER_CMD_timer_src_MASK);
        }
        else {
            BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);
        }

        if (inp_timer->timerMode.eGPTimerMode == BSCD_GPTimerMode_eNEXT_START_BIT ) {
            ulTimerCmdVal |= BCHP_SCA_SC_TIMER_CMD_timer_mode_MASK;
        }
        else {  /* BSCD_GPTimerMode_eIMMEDIATE */
            ulTimerCmdVal &= (~BCHP_SCA_SC_TIMER_CMD_timer_mode_MASK);
        }

        /* Check if we need to invoke an interrupt when the time expires */
        if (inp_timer->bIsTimerInterruptEnable == true) {  /* inp_timer->bIsTimerInterruptEnable == true && BSCD_TimerType_eGPTimer */
            BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
                    BSCD_Channel_EnableIntrCallback_isr (
                        in_channelHandle, BSCD_IntType_eTimerInt,
                                BSCD_Channel_P_TimerCB_isr));
        }
        else { /* inp_timer->bIsTimerInterruptEnable == false && BSCD_TimerType_eGPTimer */
            BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
                    BSCD_Channel_DisableIntrCallback_isr (
                        in_channelHandle, BSCD_IntType_eTimerInt));
        }

        if (inp_timer->bIsTimerEnable == true) {
            ulTimerCmdVal    |= BCHP_SCA_SC_TIMER_CMD_timer_en_MASK;
        } /* inp_timer->bIsTimerEnable == true && BSCD_TimerType_eGPTimer */

        else { /* inp_timer->bIsTimerEnable == false && BSCD_TimerType_eGPTimer */
            ulTimerCmdVal    &= ~BCHP_SCA_SC_TIMER_CMD_timer_en_MASK;

        }

    }  /* if (inp_timer->eTimerType == BSCD_TimerType_eGPTimer) */

    else {  /* BSCD_TimerType_eWaitTimer */

        /* Always disable timer first before we change timer_cmd */
        ulTimerCmdVal = BREG_Read32(
                    in_channelHandle->moduleHandle->regHandle,
                    (in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMD));

        ulTimerCmdVal &= (~BCHP_SCA_SC_TIMER_CMD_wait_en_MASK);

        BREG_Write32(
                    in_channelHandle->moduleHandle->regHandle,
                    (in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMD),
                    ulTimerCmdVal);

        BKNI_EnterCriticalSection();
        in_channelHandle->ulIntrStatus2  &= ~BCHP_SCA_SC_INTR_STAT_2_wait_intr_MASK;
        BKNI_LeaveCriticalSection();

        /* Set sc_wait registers */
        ulTimerCmpVal = ((inp_unCount->ulValue  & 0xFF0000) >> 16);
        BREG_Write32(
                    in_channelHandle->moduleHandle->regHandle,
                    (in_channelHandle->ulRegStartAddr + BSCD_P_WAIT_3),
                    ulTimerCmpVal);

        ulTimerCmpVal = ((inp_unCount->ulValue & 0x00FF00) >> 8);
        BREG_Write32(
                    in_channelHandle->moduleHandle->regHandle,
                    (in_channelHandle->ulRegStartAddr + BSCD_P_WAIT_2),
                    ulTimerCmpVal);

        ulTimerCmpVal = (inp_unCount->ulValue & 0x0000FF);
        BREG_Write32(
                    in_channelHandle->moduleHandle->regHandle,
                    (in_channelHandle->ulRegStartAddr + BSCD_P_WAIT_1),
                    ulTimerCmpVal);

        /* Check if we need to invoke an interrupt when the time expires */
        if (inp_timer->bIsTimerInterruptEnable == true) {
            BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
                    BSCD_Channel_EnableIntrCallback_isr (
                        in_channelHandle, BSCD_IntType_eWaitInt,
                                BSCD_Channel_P_WaitCB_isr));
        }
        else {
            BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
                    BSCD_Channel_DisableIntrCallback_isr (
                        in_channelHandle, BSCD_IntType_eWaitInt));
        }

        if (inp_timer->bIsTimerEnable == true) {

            /* Set the wait mode */
            if (inp_timer->eTimerType == BSCD_TimerType_eWaitTimer) {
                if (inp_timer->timerMode.eWaitTimerMode == BSCD_WaitTimerMode_eBlockWaitTime) {
                    ulTimerCmdVal |= BCHP_SCA_SC_TIMER_CMD_wait_mode_MASK;
                }
                else { /* BSCD_WaitTimerMode_eWorkWaitTime */
                    ulTimerCmdVal &= ~ BCHP_SCA_SC_TIMER_CMD_wait_mode_MASK;
                }

                ulTimerCmdVal |= BCHP_SCA_SC_TIMER_CMD_wait_en_MASK;
            }

        }/* BSCD_TimerType_eWaitTimer && inp_timer->bIsTimerEnable == true */

        else { /* inp_timer->bIsTimerEnable == false && BSCD_TimerType_eWaitTimer */
                ulTimerCmdVal &= ~BCHP_SCA_SC_TIMER_CMD_wait_en_MASK;
        }


    }  /* else  BSCD_TimerType_eWaitTimer */
    BREG_Write32(
                    in_channelHandle->moduleHandle->regHandle,
                    (in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMD),
                    ulTimerCmdVal);

    BDBG_MSG (("*** BSCD_Channel_ConfigTimer: Timer cmd = 0x%08x\n", ulTimerCmdVal));

BSCD_P_DONE_LABEL:

    BDBG_LEAVE(BSCD_Channel_ConfigTimer);
    return( errCode );
}

BERR_Code BSCD_Channel_EnableDisableTimer_isr(
        BSCD_ChannelHandle   in_channelHandle,
        BSCD_Timer                  *inp_timer
)
{
    BERR_Code errCode = BERR_SUCCESS;
    uint32_t         ulTimerCmdVal;

    BDBG_ENTER(BSCD_Channel_EnableDisableTimer_isr);
    BDBG_ASSERT( in_channelHandle );

    BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        (in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );


    ulTimerCmdVal = BREG_Read32(
                    in_channelHandle->moduleHandle->regHandle,
                    (in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMD));

    if (inp_timer->eTimerType == BSCD_TimerType_eGPTimer) {

        in_channelHandle->ulIntrStatus1  &= ~BCHP_SCA_SC_INTR_STAT_1_timer_intr_MASK;

        /* Check if we need to invoke an interrupt when the time expires */
        if (inp_timer->bIsTimerInterruptEnable == true) {  /* inp_timer->bIsTimerInterruptEnable == true && BSCD_TimerType_eGPTimer */
            BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
                    BSCD_Channel_EnableIntrCallback_isr (
                        in_channelHandle, BSCD_IntType_eTimerInt,
                                BSCD_Channel_P_TimerCB_isr));
        }
        else { /* inp_timer->bIsTimerInterruptEnable == false && BSCD_TimerType_eGPTimer */
            BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
                    BSCD_Channel_DisableIntrCallback_isr (
                        in_channelHandle, BSCD_IntType_eTimerInt));
        }

        if (inp_timer->bIsTimerEnable == true) {
            ulTimerCmdVal    |= BCHP_SCA_SC_TIMER_CMD_timer_en_MASK;
        } /* inp_timer->bIsTimerEnable == true && BSCD_TimerType_eGPTimer */

        else { /* inp_timer->bIsTimerEnable == false && BSCD_TimerType_eGPTimer */
            ulTimerCmdVal    &= ~BCHP_SCA_SC_TIMER_CMD_timer_en_MASK;
        }


    }  /* if (inp_timer->eTimerType == BSCD_TimerType_eGPTimer) */

    else {  /* BSCD_TimerType_eWaitTimer */

        in_channelHandle->ulIntrStatus2  &= ~BCHP_SCA_SC_INTR_STAT_2_wait_intr_MASK;

        /* Check if we need to invoke an interrupt when the time expires */
        if (inp_timer->bIsTimerInterruptEnable == true) {
            BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
                    BSCD_Channel_EnableIntrCallback_isr (
                        in_channelHandle, BSCD_IntType_eWaitInt,
                                BSCD_Channel_P_WaitCB_isr));
        }
        else {
            BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
                    BSCD_Channel_DisableIntrCallback_isr (
                        in_channelHandle, BSCD_IntType_eWaitInt));
        }

        if (inp_timer->bIsTimerEnable == true) {

            /* Set the wait mode */
            if (inp_timer->eTimerType == BSCD_TimerType_eWaitTimer) {
                if (inp_timer->timerMode.eWaitTimerMode == BSCD_WaitTimerMode_eBlockWaitTime) {
                    ulTimerCmdVal |= BCHP_SCA_SC_TIMER_CMD_wait_mode_MASK;
                }
                else { /* BSCD_WaitTimerMode_eWorkWaitTime */
                    ulTimerCmdVal &= ~ BCHP_SCA_SC_TIMER_CMD_wait_mode_MASK;
                }

                ulTimerCmdVal |= BCHP_SCA_SC_TIMER_CMD_wait_en_MASK;
            }

        }/* BSCD_TimerType_eWaitTimer && inp_timer->bIsTimerEnable == true */

        else { /* inp_timer->bIsTimerEnable == false && BSCD_TimerType_eWaitTimer */
                ulTimerCmdVal &= ~BCHP_SCA_SC_TIMER_CMD_wait_en_MASK;
        }


    }  /* else  BSCD_TimerType_eWaitTimer */

    BREG_Write32(
                    in_channelHandle->moduleHandle->regHandle,
                    (in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMD),
                    ulTimerCmdVal);

    BDBG_MSG (("*** BSCD_Channel_EnableDisableTimer_isr: Timer cmd = 0x%08x\n", ulTimerCmdVal));

BSCD_P_DONE_LABEL:

    BDBG_LEAVE(BSCD_Channel_EnableDisableTimer_isr);
    return( errCode );
}

BERR_Code BSCD_Channel_EnableIntrCallback_isr(
    BSCD_ChannelHandle  in_channelHandle,
    BSCD_IntrType       in_eIntType,
    BSCD_IsrCallbackFunc in_callback
)
{
    uint32_t  ulVal;
    unsigned int  unReg = BSCD_P_INTR_EN_1, i;
    BERR_Code errCode = BERR_SUCCESS;

    if ( (in_eIntType == BSCD_IntType_eTParityInt)    ||
        (in_eIntType == BSCD_IntType_eTimerInt)      ||
        (in_eIntType == BSCD_IntType_eCardInsertInt) ||
        (in_eIntType == BSCD_IntType_eCardRemoveInt) ||
        (in_eIntType == BSCD_IntType_eBGTInt)        ||
        (in_eIntType == BSCD_IntType_eTDoneInt)      ||
        (in_eIntType == BSCD_IntType_eRetryInt)      ||
        (in_eIntType == BSCD_IntType_eTEmptyInt) ||
        (in_eIntType == BSCD_IntType_eEvent1Int)) {
        unReg = BSCD_P_INTR_EN_1;
        /* BDBG_MSG(("BSCD_P_INTR_EN_1: ")); */
    }
    else if ( (in_eIntType == BSCD_IntType_eRParityInt) ||
        (in_eIntType == BSCD_IntType_eATRInt)          ||
        (in_eIntType == BSCD_IntType_eCWTInt)          ||
        (in_eIntType == BSCD_IntType_eRLenInt)         ||
        (in_eIntType == BSCD_IntType_eWaitInt)         ||
        (in_eIntType == BSCD_IntType_eRcvInt)          ||
        (in_eIntType == BSCD_IntType_eRReadyInt) ||
        (in_eIntType == BSCD_IntType_eEvent2Int)) {
        unReg = BSCD_P_INTR_EN_2;
        /* BDBG_MSG(("BSCD_P_INTR_EN_2: ")); */
    }
    else if (in_eIntType == BSCD_IntType_eEDCInt) {
        unReg = BSCD_P_PROTO_CMD;
    }
    else {
        BDBG_ERR(("Interrupt not supported, in_eIntType = %d\n", in_eIntType));
        BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,  true);
    }

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + unReg));

    /* BDBG_MSG(("ulVal = 0x%x", ulVal)); */

    switch (in_eIntType) {

        case BSCD_IntType_eTParityInt:
            for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
                if (in_channelHandle->callBack.tParityIsrCBFunc[i] == NULL) {
                    in_channelHandle->callBack.tParityIsrCBFunc[i] = in_callback;
                    break;
                }
                else if ((in_channelHandle->callBack.tParityIsrCBFunc[i] != NULL) &&
                    (in_channelHandle->callBack.tParityIsrCBFunc[i] == in_callback) ) {
                    break;
                }
            }
            ulVal |=  BCHP_SCA_SC_INTR_STAT_1_tpar_intr_MASK;
            break;

        case BSCD_IntType_eTimerInt:
            for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
                if (in_channelHandle->callBack.timerIsrCBFunc[i] == NULL) {
                    in_channelHandle->callBack.timerIsrCBFunc[i] = in_callback;
                    break;
                }
                else if ((in_channelHandle->callBack.timerIsrCBFunc[i] != NULL) &&
                    (in_channelHandle->callBack.timerIsrCBFunc[i] == in_callback) ) {
                    break;
                }
            }
            ulVal |=  BCHP_SCA_SC_INTR_STAT_1_timer_intr_MASK;
            break;

        case BSCD_IntType_eCardInsertInt:
            for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
                if (in_channelHandle->callBack.cardInsertIsrCBFunc[i] == NULL) {
                    in_channelHandle->callBack.cardInsertIsrCBFunc[i] = in_callback;
                    BDBG_MSG(("new BSCD_IntType_eCardInsertInt  callback "));
                    break;
                }
                else if ((in_channelHandle->callBack.cardInsertIsrCBFunc[i] != NULL) &&
                    (in_channelHandle->callBack.cardInsertIsrCBFunc[i] == in_callback) ) {
                    BDBG_MSG(("BSCD_IntType_eCardInsertInt same callback "));
                    break;
                }
            }
            ulVal |=  BCHP_SCA_SC_INTR_STAT_1_pres_intr_MASK;
            break;

        case BSCD_IntType_eCardRemoveInt:
            for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
                if (in_channelHandle->callBack.cardRemoveIsrCBFunc[i] == NULL) {
                    in_channelHandle->callBack.cardRemoveIsrCBFunc[i] = in_callback;
                    BDBG_MSG(("new BSCD_IntType_eCardRemoveInt  callback "));
                    break;
                }
                else if ((in_channelHandle->callBack.cardRemoveIsrCBFunc[i] != NULL) &&
                    (in_channelHandle->callBack.cardRemoveIsrCBFunc[i] == in_callback) ) {
                    BDBG_MSG(("BSCD_IntType_eCardRemoveInt same callback "));
                    break;
                }
            }
            ulVal |=  BCHP_SCA_SC_INTR_STAT_1_pres_intr_MASK;
            break;

        case BSCD_IntType_eBGTInt:
            for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
                if (in_channelHandle->callBack.bgtIsrCBFunc[i] == NULL) {
                    in_channelHandle->callBack.bgtIsrCBFunc[i] = in_callback;
                    break;
                }
                else if ((in_channelHandle->callBack.bgtIsrCBFunc[i] != NULL) &&
                    (in_channelHandle->callBack.bgtIsrCBFunc[i] == in_callback) ) {
                    break;
                }
            }
            ulVal |=  BCHP_SCA_SC_INTR_STAT_1_bgt_intr_MASK;
            break;

        case BSCD_IntType_eTDoneInt:
            for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
                if (in_channelHandle->callBack.tDoneIsrCBFunc[i] == NULL) {
                    in_channelHandle->callBack.tDoneIsrCBFunc[i] = in_callback;
                    break;
                }
                else if ((in_channelHandle->callBack.tDoneIsrCBFunc[i] != NULL) &&
                    (in_channelHandle->callBack.tDoneIsrCBFunc[i] == in_callback) ) {
                    break;
                }
            }
            ulVal |=  BCHP_SCA_SC_INTR_STAT_1_tdone_intr_MASK;
            break;

        case BSCD_IntType_eRetryInt:
            for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
                if (in_channelHandle->callBack.retryIsrCBFunc[i] == NULL) {
                    in_channelHandle->callBack.retryIsrCBFunc[i] = in_callback;
                    break;
                }
                else if ((in_channelHandle->callBack.retryIsrCBFunc[i] != NULL) &&
                    (in_channelHandle->callBack.retryIsrCBFunc[i] == in_callback) ) {
                    break;
                }
            }
            ulVal |=  BCHP_SCA_SC_INTR_STAT_1_retry_intr_MASK;
            break;

        case BSCD_IntType_eTEmptyInt:
            for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
                if (in_channelHandle->callBack.tEmptyIsrCBFunc[i] == NULL) {
                    in_channelHandle->callBack.tEmptyIsrCBFunc[i] = in_callback;
                    break;
                }
                else if ((in_channelHandle->callBack.tEmptyIsrCBFunc[i] != NULL) &&
                    (in_channelHandle->callBack.tEmptyIsrCBFunc[i] == in_callback) ) {
                    break;
                }
            }
            ulVal |=  BCHP_SCA_SC_INTR_STAT_1_tempty_intr_MASK;
            break;

        case BSCD_IntType_eRParityInt:
            for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
                if (in_channelHandle->callBack.rParityIsrCBFunc[i] == NULL) {
                    in_channelHandle->callBack.rParityIsrCBFunc[i] = in_callback;
                    break;
                }
                else if ((in_channelHandle->callBack.rParityIsrCBFunc[i] != NULL) &&
                    (in_channelHandle->callBack.rParityIsrCBFunc[i] == in_callback) ) {
                    break;
                }
            }
            ulVal |=  BCHP_SCA_SC_INTR_STAT_2_rpar_intr_MASK;
            break;

        case BSCD_IntType_eATRInt:
            for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
                if (in_channelHandle->callBack.atrIsrCBFunc[i] == NULL) {
                    in_channelHandle->callBack.atrIsrCBFunc[i] = in_callback;
                    break;
                }
                else if ((in_channelHandle->callBack.atrIsrCBFunc[i] != NULL) &&
                    (in_channelHandle->callBack.atrIsrCBFunc[i] == in_callback) ) {
                    break;
                }
            }
            ulVal |=  BCHP_SCA_SC_INTR_STAT_2_atrs_intr_MASK;
            break;

        case BSCD_IntType_eCWTInt:
            for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
                if (in_channelHandle->callBack.cwtIsrCBFunc[i] == NULL) {
                    in_channelHandle->callBack.cwtIsrCBFunc[i] = in_callback;
                    break;
                }
                else if ((in_channelHandle->callBack.cwtIsrCBFunc[i] != NULL) &&
                    (in_channelHandle->callBack.cwtIsrCBFunc[i] == in_callback) ) {
                    break;
                }
            }
            ulVal |=  BCHP_SCA_SC_INTR_STAT_2_cwt_intr_MASK;
            break;

        case BSCD_IntType_eRLenInt:
            for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
                if (in_channelHandle->callBack.rLenIsrCBFunc[i] == NULL) {
                    in_channelHandle->callBack.rLenIsrCBFunc[i] = in_callback;
                    break;
                }
                else if ((in_channelHandle->callBack.rLenIsrCBFunc[i] != NULL) &&
                    (in_channelHandle->callBack.rLenIsrCBFunc[i] == in_callback) ) {
                    break;
                }
            }
            ulVal |=  BCHP_SCA_SC_INTR_STAT_2_rlen_intr_MASK;
            break;

        case BSCD_IntType_eWaitInt:
            for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
                if (in_channelHandle->callBack.waitIsrCBFunc[i] == NULL) {
                    in_channelHandle->callBack.waitIsrCBFunc[i] = in_callback;
                    break;
                }
                else if ((in_channelHandle->callBack.waitIsrCBFunc[i] != NULL) &&
                    (in_channelHandle->callBack.waitIsrCBFunc[i] == in_callback) ) {
                    break;
                }
            }
            ulVal |=  BCHP_SCA_SC_INTR_STAT_2_wait_intr_MASK;
            break;

        case BSCD_IntType_eRcvInt:
            for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
                if (in_channelHandle->callBack.rcvIsrCBFunc[i] == NULL) {
                    in_channelHandle->callBack.rcvIsrCBFunc[i] = in_callback;
                    break;
                }
                else if ((in_channelHandle->callBack.rcvIsrCBFunc[i] != NULL) &&
                    (in_channelHandle->callBack.rcvIsrCBFunc[i] == in_callback) ) {
                    break;
                }
            }
            ulVal |=  BCHP_SCA_SC_INTR_STAT_2_rcv_intr_MASK;
            break;

        case BSCD_IntType_eRReadyInt:
            for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
                if (in_channelHandle->callBack.rReadyIsrCBFunc[i] == NULL) {
                    in_channelHandle->callBack.rReadyIsrCBFunc[i] = in_callback;
                    break;
                }
                else if ((in_channelHandle->callBack.rReadyIsrCBFunc[i] != NULL) &&
                    (in_channelHandle->callBack.rReadyIsrCBFunc[i] == in_callback) ) {
                    break;
                }
            }
            ulVal |=  BCHP_SCA_SC_INTR_STAT_2_rready_intr_MASK;
            break;

        case BSCD_IntType_eEDCInt:
            if (in_channelHandle->currentChannelSettings.eProtocolType == BSCD_AsyncProtocolType_e0 ) {
                for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
                    if (in_channelHandle->callBack.edcIsrCBFunc[i] == NULL) {
                        in_channelHandle->callBack.edcIsrCBFunc[i] = in_callback;
                        break;
                    }
                    else if ((in_channelHandle->callBack.edcIsrCBFunc[i] != NULL) &&
                        (in_channelHandle->callBack.edcIsrCBFunc[i] == in_callback) ) {
                        break;
                    }
                }
                ulVal |=  BCHP_SCA_SC_PROTO_CMD_edc_en_MASK;
            }
            break;

        case BSCD_IntType_eEvent1Int:
            for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
                if (in_channelHandle->callBack.event1IsrCBFunc[i] == NULL) {
                    in_channelHandle->callBack.event1IsrCBFunc[i] = in_callback;
                    break;
                }
                else if ((in_channelHandle->callBack.event1IsrCBFunc[i] != NULL) &&
                    (in_channelHandle->callBack.event1IsrCBFunc[i] == in_callback) ) {
                    break;
                }
            }
            ulVal |=  BCHP_SCA_SC_INTR_STAT_1_event1_intr_MASK;
            break;

        case BSCD_IntType_eEvent2Int:
            for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
                if (in_channelHandle->callBack.event2IsrCBFunc[i] == NULL) {
                    in_channelHandle->callBack.event2IsrCBFunc[i] = in_callback;
                    break;
                }
                else if ((in_channelHandle->callBack.event2IsrCBFunc[i] != NULL) &&
                    (in_channelHandle->callBack.event2IsrCBFunc[i] == in_callback) ) {
                    break;
                }
            }
            ulVal |=  BCHP_SCA_SC_INTR_STAT_2_event2_intr_MASK;
            break;

        default:
            BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);
    }

    BREG_Write32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + unReg),
             ulVal);


    /*BDBG_MSG((", final ulVal = 0x%x\n ", ulVal)); */

BSCD_P_DONE_LABEL:

    BDBG_LEAVE(BSCD_Channel_EnableIntrCallback_isr);
    return( errCode );

}

BERR_Code BSCD_Channel_DisableIntrCallback_isr(
    BSCD_ChannelHandle  in_channelHandle,
    BSCD_IntrType    in_eIntType
)
{
    uint32_t ulVal;
    unsigned int  unReg = BSCD_P_INTR_EN_1;
    BERR_Code errCode = BERR_SUCCESS;

    if ( (in_eIntType == BSCD_IntType_eTParityInt)    ||
        (in_eIntType == BSCD_IntType_eTimerInt)      ||
        (in_eIntType == BSCD_IntType_eCardInsertInt) ||
        (in_eIntType == BSCD_IntType_eCardRemoveInt) ||
        (in_eIntType == BSCD_IntType_eBGTInt)        ||
        (in_eIntType == BSCD_IntType_eTDoneInt)      ||
        (in_eIntType == BSCD_IntType_eRetryInt)      ||
        (in_eIntType == BSCD_IntType_eTEmptyInt) ||
            (in_eIntType == BSCD_IntType_eEvent1Int)) {
        unReg = BSCD_P_INTR_EN_1;
        /* BDBG_MSG(("BSCD_P_INTR_EN_1: "));         */
    }
    else if ( (in_eIntType == BSCD_IntType_eRParityInt) ||
        (in_eIntType == BSCD_IntType_eATRInt)          ||
        (in_eIntType == BSCD_IntType_eCWTInt)          ||
        (in_eIntType == BSCD_IntType_eRLenInt)         ||
        (in_eIntType == BSCD_IntType_eWaitInt)         ||
        (in_eIntType == BSCD_IntType_eRcvInt)          ||
        (in_eIntType == BSCD_IntType_eRReadyInt) ||
         (in_eIntType == BSCD_IntType_eEvent2Int)) {
        unReg = BSCD_P_INTR_EN_2;
        /* BDBG_MSG(("BSCD_P_INTR_EN_2: "));         */
    }
    else if (in_eIntType == BSCD_IntType_eEDCInt) {
        unReg = BSCD_P_PROTO_CMD;
    }
    else {
        BDBG_MSG(("Interrupt not supported\n"));
        BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,  true);
    }

    ulVal = BREG_Read32(
        in_channelHandle->moduleHandle->regHandle,
        (in_channelHandle->ulRegStartAddr + unReg));

    /* BDBG_MSG(("ulVal = 0x%x", ulVal)); */

    switch (in_eIntType) {


        case BSCD_IntType_eTParityInt:
             ulVal &= ~BCHP_SCA_SC_INTR_STAT_1_tpar_intr_MASK;
             break;

        case BSCD_IntType_eTimerInt:
            ulVal &= ~BCHP_SCA_SC_INTR_STAT_1_timer_intr_MASK;
            break;

        case BSCD_IntType_eCardInsertInt:
            ulVal &= ~BCHP_SCA_SC_INTR_STAT_1_pres_intr_MASK;
            break;

        case BSCD_IntType_eCardRemoveInt:
            ulVal &= ~BCHP_SCA_SC_INTR_STAT_1_pres_intr_MASK;
            break;

        case BSCD_IntType_eBGTInt:
            ulVal &= ~BCHP_SCA_SC_INTR_STAT_1_bgt_intr_MASK;
            break;

        case BSCD_IntType_eTDoneInt:
            ulVal &= ~BCHP_SCA_SC_INTR_STAT_1_tdone_intr_MASK;
            break;

        case BSCD_IntType_eRetryInt:
            ulVal &= ~BCHP_SCA_SC_INTR_STAT_1_retry_intr_MASK;
            break;

        case BSCD_IntType_eTEmptyInt:
            ulVal &= ~BCHP_SCA_SC_INTR_STAT_1_tempty_intr_MASK;
            break;

        case BSCD_IntType_eRParityInt:
            ulVal &= ~BCHP_SCA_SC_INTR_STAT_2_rpar_intr_MASK;
            break;

        case BSCD_IntType_eATRInt:
            ulVal &= ~BCHP_SCA_SC_INTR_STAT_2_atrs_intr_MASK;
            break;

        case BSCD_IntType_eCWTInt:
            ulVal &= ~BCHP_SCA_SC_INTR_STAT_2_cwt_intr_MASK;
            break;

        case BSCD_IntType_eRLenInt:
            ulVal &= ~BCHP_SCA_SC_INTR_STAT_2_rlen_intr_MASK;
            break;

        case BSCD_IntType_eWaitInt:
            ulVal &= ~BCHP_SCA_SC_INTR_STAT_2_wait_intr_MASK;
            break;

        case BSCD_IntType_eRcvInt:
            ulVal &= ~BCHP_SCA_SC_INTR_STAT_2_rcv_intr_MASK;
            break;

        case BSCD_IntType_eRReadyInt:
            ulVal &= ~BCHP_SCA_SC_INTR_STAT_2_rready_intr_MASK;
            break;

        case BSCD_IntType_eEDCInt:
            ulVal &= ~BCHP_SCA_SC_PROTO_CMD_edc_en_MASK;
            break;

        case BSCD_IntType_eEvent1Int:
            ulVal &= ~BCHP_SCA_SC_INTR_STAT_1_event1_intr_MASK;
            break;

        case BSCD_IntType_eEvent2Int:
            ulVal &=  ~BCHP_SCA_SC_INTR_STAT_2_event2_intr_MASK;
            break;

        default:
            BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);
    }

    BREG_Write32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + unReg),
             ulVal);

    /* BDBG_MSG((", final ulVal = 0x%x\n ", ulVal)); */
BSCD_P_DONE_LABEL:

    BDBG_LEAVE(BSCD_Channel_DisableIntrCallback_isr);
    return( errCode );
}

BERR_Code BSCD_Channel_EnableInterrupts(
    BSCD_ChannelHandle  in_channelHandle
)
{
    BERR_Code errCode = BERR_SUCCESS;

    BDBG_ENTER(BSCD_Channel_EnableInterrupts);
    BDBG_ASSERT( in_channelHandle );

    BKNI_EnterCriticalSection();
    if ( (errCode = BSCD_Channel_P_EnableInterrupts_isr(in_channelHandle)) != BERR_SUCCESS) {
        errCode = BERR_TRACE(errCode);
        goto BSCD_P_DONE_LABEL;
    }
    BKNI_LeaveCriticalSection();

BSCD_P_DONE_LABEL:

    BDBG_LEAVE(BSCD_Channel_EnableInterrupts);
    return( errCode );
}

BERR_Code BSCD_Channel_ResetBlockWaitTimer(
        BSCD_ChannelHandle          in_channelHandle
)
{
    BERR_Code errCode = BERR_SUCCESS;
    BSCD_Timer      timer = {BSCD_TimerType_eWaitTimer, {BSCD_GPTimerMode_eIMMEDIATE},
                            false, true};
    BSCD_TimerValue    timeValue= {BSCD_DEFAULT_BLOCK_WAITING_TIME, BSCD_TimerUnit_eETU};

    BDBG_ENTER(BSCD_Channel_ResetBlockWaitTimer);
    BDBG_ASSERT( in_channelHandle );

    /* Need this for MetroWerks */
    timer.eTimerType = BSCD_TimerType_eWaitTimer;
    timer.timerMode.eWaitTimerMode = BSCD_WaitTimerMode_eBlockWaitTime;

    timeValue.ulValue = in_channelHandle->currentChannelSettings.blockWaitTime.ulValue ;
    BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
            BSCD_Channel_ConfigTimer(in_channelHandle, &timer, &timeValue));

    in_channelHandle->currentChannelSettings.blockWaitTimeExt.ulValue = 0;

BSCD_P_DONE_LABEL:

    BDBG_LEAVE(BSCD_Channel_ResetBlockWaitTimer);
    return( errCode );
}


BERR_Code BSCD_Channel_SetBlockWaitTimeExt(
        BSCD_ChannelHandle          in_channelHandle,
        uint32_t                    in_ulBlockWaitTimeExtInETU
)
{
    BERR_Code errCode = BERR_SUCCESS;

    BDBG_ENTER(BSCD_Channel_SetBlockWaitTimeExt);
    BDBG_ASSERT( in_channelHandle );

    in_channelHandle->currentChannelSettings.blockWaitTimeExt.ulValue = in_ulBlockWaitTimeExtInETU;

    BDBG_LEAVE(BSCD_Channel_SetBlockWaitTimeExt);
    return( errCode );
}

void BSCD_Channel_DumpRegisters(
        BSCD_ChannelHandle       in_channelHandle
)
{
    uint32_t ulVal;

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1));
    BDBG_MSG(("UART_CMD_1 = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_2));
    BDBG_MSG(("UART_CMD_2 = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_PROTO_CMD));
    BDBG_MSG(("PROTO = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_FLOW_CMD));
    BDBG_MSG(("FLOW_CMD = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1));
    BDBG_MSG(("IF_CMD_1 = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_2));
    BDBG_MSG(("IF_CMD_2 = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_INTR_STAT_1));
    BDBG_MSG(("INTR_STAT_1 = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_INTR_STAT_2));
    BDBG_MSG(("INTR_STAT_2 = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_INTR_EN_1));
    BDBG_MSG(("INTR_EN_1 = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_INTR_EN_2));
    BDBG_MSG(("INTR_EN_2 = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_CLK_CMD));
    BDBG_MSG(("CLK_CMD = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_PRESCALE));
    BDBG_MSG(("PRESCALE = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMD));
    BDBG_MSG(("TIMER_CMD = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_BGT));
    BDBG_MSG(("BGT = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CNT_1));
    BDBG_MSG(("TIMER_CNT_1 = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CNT_2));
    BDBG_MSG(("TIMER_CNT_2 = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMP_1));
    BDBG_MSG(("TIMER_CMP_1 = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMP_2));
    BDBG_MSG(("TIMER_CMP_2 = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_WAIT_1));
    BDBG_MSG(("SC_WAIT_1 = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_WAIT_2));
    BDBG_MSG(("SC_WAIT_2 = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_WAIT_3));
    BDBG_MSG(("SC_WAIT_3 = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_TGUARD));
    BDBG_MSG(("TGUARD = %02x \n",ulVal));

#ifdef BSCD_EMV2000_CWT_PLUS_4_EVENT_INTR
    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_EVENT1_CMD_1));
    BDBG_MSG(("BSCD_P_EVENT1_CMD_1 = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_EVENT1_CMD_2));
    BDBG_MSG(("BSCD_P_EVENT1_CMD_2 = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_EVENT1_CMD_3));
    BDBG_MSG(("BSCD_P_EVENT1_CMD_3 = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_EVENT1_CMD_4));
    BDBG_MSG(("BSCD_P_EVENT1_CMD_4 = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_EVENT1_CMP));
    BDBG_MSG(("BSCD_P_EVENT1_CMP = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_EVENT2_CMD_1));
    BDBG_MSG(("BSCD_P_EVENT2_CMD_1 = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_EVENT2_CMD_2));
    BDBG_MSG(("BSCD_P_EVENT2_CMD_2 = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_EVENT2_CMD_3));
    BDBG_MSG(("BSCD_P_EVENT2_CMD_3 = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_EVENT2_CMD_4));
    BDBG_MSG(("BSCD_P_EVENT2_CMD_4 = %02x \n",ulVal));

    ulVal = BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_EVENT2_CMP));
    BDBG_MSG(("BSCD_P_EVENT2_CMP = %02x \n",ulVal));

#endif
    /*
    bcmDeviceRead(inp_device, eSize8, BCM_SC_TRANSMIT, (void *)&ulVal);
    BDBG_MSG(("TRANSMIT = %02x \n",ulVal));
    bcmDeviceRead(inp_device, eSize8, BCM_SC_RECEIVE, (void *)&ulVal);
    BDBG_MSG(("RECEIVE = %02x \n",ulVal));


    bcmDeviceRead(inp_device, eSize8, BCM_SC_TLEN_1, (void *)&ulVal);
    BDBG_MSG(("TLEN_1 = %02x \n",ulVal));
    bcmDeviceRead(inp_device, eSize8, BCM_SC_TLEN_2, (void *)&ulVal);
    BDBG_MSG(("TLEN_2 = %02x \n",ulVal));
    bcmDeviceRead(inp_device, eSize8, BCM_SC_RLEN_1, (void *)&ulVal);
    BDBG_MSG(("RLEN_1 = %02x \n",ulVal));
    bcmDeviceRead(inp_device, eSize8, BCM_SC_RLEN_2, (void *)&ulVal);
    BDBG_MSG(("RLEN_2 = %02x \n",ulVal));

    */
}


