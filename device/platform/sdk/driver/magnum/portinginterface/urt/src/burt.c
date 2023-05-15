/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: burt.c $
 * $brcm_Revision: Hydra_Software_Devel/80 $
 * $brcm_Date: 9/18/12 2:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/urt/7408/burt.c $
 * 
 * Hydra_Software_Devel/80   9/18/12 2:12p agin
 * SW7445-37:  Do not need to specify BCHP_CHIP.
 * 
 * Hydra_Software_Devel/79   9/18/12 1:35p agin
 * SW7445-37:  Cleaned up code.
 * 
 * Hydra_Software_Devel/78   9/18/12 7:33a agin
 * SW7445-37:  Add urt PI support for 7445.
 * 
 * Hydra_Software_Devel/77   4/28/12 4:17p chengs
 * SW7584-23: Add 7584 support.
 * 
 * Hydra_Software_Devel/76   4/26/12 5:31p bselva
 * SW7360-17: Add support for 7360
 * 
 * Hydra_Software_Devel/75   4/14/12 8:13a agin
 * SWNOOS-530:  Add support for 7435.
 * 
 * Hydra_Software_Devel/74   3/18/12 6:17p agin
 * SWNOOS-523:  Add 7429 support.
 * 
 * Hydra_Software_Devel/73   11/7/11 7:40a agin
 * SWNOOS-497:  Relevel 97231 to prerel for B0.
 * 
 * Hydra_Software_Devel/72   6/21/11 6:45a agin
 * SW7346-273:  Use enum values in switch statement.
 * 
 * Hydra_Software_Devel/71   2/14/11 4:11p xhuang
 * SW7358-29: Add 7358/7552 support
 * 
 * Hydra_Software_Devel/70   2/4/11 2:56p jrubio
 * SW7344-9: add 7346 and 7344 support
 * 
 * Hydra_Software_Devel/69   1/26/11 2:26p etrudeau
 * SWBLURAY-23692: add 7640 support
 * 
 * Hydra_Software_Devel/68   12/22/10 5:18p rpereira
 * SW7550-627: Corrected function name of ResetTxRx
 *
 * Hydra_Software_Devel/67   12/22/10 4:59p rpereira
 * SW7550-627: Need a method to flush/reset the RX fifo
 *
 * Hydra_Software_Devel/66   11/2/10 5:29p hongtaoz
 * SW7425-9: added 7425 support;
 *
 * Hydra_Software_Devel/65   10/14/10 4:46p nickh
 * SW7422-74: Add 7422 support
 *
 * Hydra_Software_Devel/64   12/13/09 6:45p rpereira
 * SW7550-41: Fixed compilation issues for 7550
 *
 * Hydra_Software_Devel/63   12/7/09 10:17p randyjew
 * SW7468-6:Add 7468 support
 *
 * Hydra_Software_Devel/62   11/9/09 11:57a gmohile
 * SW7408-1 : Add 7408 support
 *
 * Hydra_Software_Devel/61   9/15/09 11:30a rpereira
 * SW7630-45: Adding 7630 support
 *
 * Hydra_Software_Devel/61   9/15/09 11:06a rpereira
 * SW7630-45: Adding 7630 support
 *
 * Hydra_Software_Devel/61   9/15/09 11:05a rpereira
 * SW7630-45: Adding 7630 support
 *
 * Hydra_Software_Devel/61   9/15/09 10:53a rpereira
 * SW7630-45: Adding 7630 support
 *
 * Hydra_Software_Devel/60   9/1/09 6:46p pntruong
 * SW7550-30: Fixed build errors.
 *
 * Hydra_Software_Devel/59   9/1/09 5:54p rpereira
 * SW7550-30: Added 7550 support
 *
 * Hydra_Software_Devel/58   8/20/09 4:28p mward
 * PR55545: Support 7125.
 *
 * Hydra_Software_Devel/57   8/10/09 6:12p jrubio
 * PR55232: add 7340/73432 support
 *
 * Hydra_Software_Devel/56   7/22/09 1:19p maivu
 * PR 30806: Fix build error with previous checkins for 93549/93556
 *
 *
 * Hydra_Software_Devel/55   7/21/09 5:43p jkim
 * PR30806: Add the function to set stop bits
 *
 * Hydra_Software_Devel/54   4/9/09 4:33p rpereira
 * PR52971: adding 7635 support
 *
 * Hydra_Software_Devel/53   1/31/09 1:08a jrubio
 * PR51629: add 7336 support
 *
 * Hydra_Software_Devel/52   12/2/08 4:17p kaushikb
 * PR49867: Adding support  for 7420
 *
 * Hydra_Software_Devel/51   9/4/08 4:55p jkim
 * PR44823: add a function BURT_Read_Isr()
 *
 * Hydra_Software_Devel/50   9/4/08 4:05p jkim
 * PR45727: remove compiler warning
 *
 * Hydra_Software_Devel/49   8/21/08 10:41a fbasso
 * PR 44545: added support for 7601.
 *
 * Hydra_Software_Devel/48   6/12/08 10:16a jkim
 * PR43200: add break after default label in switch statement and use
 * BCHP_UARTA_LCR instead of BCHP_UARTB_LCR since offset is used UARTB
 * and UARTC
 *
 * Hydra_Software_Devel/47   4/9/08 10:26a farshidf
 * PR39194: add support for3548
 *
 * Hydra_Software_Devel/46   4/4/08 3:12p farshidf
 * PR39483: add 3548/3556 support
 *
 * Hydra_Software_Devel/45   12/13/07 10:14a brianlee
 * PR36744: Fixed a bug for 7440.
 *
 * Hydra_Software_Devel/44   11/28/07 2:24p farshidf
 * PR36894: Add 7335 support
 *
 * Hydra_Software_Devel/43   11/28/07 1:32p brianlee
 * PR36744: Added support for uart D for 7440.
 *
 * Hydra_Software_Devel/42   11/6/07 5:20p brianlee
 * PR36744: Fixed compilation warnings.
 *
 * Hydra_Software_Devel/41   10/14/07 4:01p jkim
 * PR14344: Add 7325 support
 *
 * Hydra_Software_Devel/40   8/22/07 10:45a jkim
 * PR14344: add support 7403
 *
 * Hydra_Software_Devel/39   7/6/07 3:06p jkim
 * PR32436: fix compile error when USE_BURT_INT_CALLBACK is defined
 *
 * Hydra_Software_Devel/38   5/21/07 4:27p jkim
 * PR14344: Add 7405 support
 *
 * Hydra_Software_Devel/37   4/25/07 4:12p jkim
 * PR29625: Add three functions, which can be called within ISR.
 *
 * Hydra_Software_Devel/36   3/1/07 5:02p jkim
 * PR26690: add back intMode so that it is backward compatible. Now,
 * txIntMode and rxIntMode are only used if intMode is set to false.
 *
 * Hydra_Software_Devel/35   2/23/07 11:15a jkim
 * PR26690: remove global variable and make it part of channel handle
 * structure
 *
 * Hydra_Software_Devel/34   1/22/07 11:16a agin
 * PR26997: merge 3 UART support to trunk.
 *
 * Hydra_Software_Devel/33   1/15/07 3:34p jkim
 * PR26690: separate rx/tx interrupt flag in the structure so that each
 * interrupt can be enabled indepent of the other
 *
 * Hydra_Software_Devel/32   1/10/07 6:34p jkim
 * PR26690: Add option to use callback instead of event when rx/tx
 * interrupt is handled
 *
 * Hydra_Software_Devel/31   12/12/06 3:04p jkim
 * PR24640: Preserve error bits in LSR whne LSR is read.
 *
 * Hydra_Software_Devel/30   11/9/06 12:55p jkim
 * PR14344: add support for 7403
 *
 * Hydra_Software_Devel/29   9/19/06 5:20p agin
 * PR24339: Resolve compiler warning for DEBUG=n builds for UPG modules.
 *
 * Hydra_Software_Devel/28   9/19/06 3:33p jkim
 * PR24353: fix compiler error for 7401 C0
 *
 * Hydra_Software_Devel/27   9/11/06 4:50p brianlee
 * PR23187: Added code for 7440.
 *
 * Hydra_Software_Devel/26   9/1/06 12:00p ltokuda
 * PR21692: Update for 7118.
 *
 * Hydra_Software_Devel/25   8/9/06 11:11a agin
 * PR23362: Add 3563 support.
 *
 * Hydra_Software_Devel/24   7/19/06 10:25a jkim
 * PR22662: Cleaned up printf() and asjusted the offset accordingly during
 * BURT_EnableFifo()
 *
 * Hydra_Software_Devel/23   7/18/06 9:24a jkim
 * PR22662: modified to support UARTC for 7401 B0
 *
 * Hydra_Software_Devel/22   6/21/06 9:55a jkim
 * PR14344: Add support for B0
 *
 * Hydra_Software_Devel/21   4/12/06 2:37p brianlee
 * PR20757: Re-enable RX interrupt even if the FIFO is not empty.
 *
 * Hydra_Software_Devel/20   3/21/06 3:06p jkim
 * PR20326: Add support for 7438
 *
 * Hydra_Software_Devel/19   3/14/06 6:06p brianlee
 * PR14344: Fixed another problem with 7401 UART C.
 *
 * Hydra_Software_Devel/18   3/14/06 11:13a brianlee
 * PR14344: Fixed default uart channel settings for 7401.
 *
 * Hydra_Software_Devel/17   3/14/06 10:09a brianlee
 * PR14344: Changed C++ style comments to C.
 *
 * Hydra_Software_Devel/16   3/13/06 11:01a brianlee
 * PR14344: Fixed the number of uart channels for 7401.
 *
 * Hydra_Software_Devel/15   3/1/06 11:28a agin
 * PR19076: PR19919: Fixed baud rate and data bits calculation.
 * PR19919: Update uart porting interface code for BCM7400.
 * PR19076: Support BCM7400.
 *
 * Hydra_Software_Devel/PR19076/3   2/28/06 5:44p agin
 * PR19919: Fixed baud rate and data bits calculation.
 *
 * Hydra_Software_Devel/PR19076/2   2/27/06 5:34p agin
 * PR19919: Update uart porting interface code for BCM7400.
 *
 * Hydra_Software_Devel/PR19076/1   1/16/06 7:25p agin
 * PR19076: Support BCM7400.
 *
 * Hydra_Software_Devel/14   10/28/05 2:37p jkim
 * PR14344: Add 7401 support
 *
 * Hydra_Software_Devel/13   5/9/05 11:42a dlwin
 * PR 14698: Resolve "unused ..." warnings.
 *
 * Hydra_Software_Devel/12   3/10/05 9:38a dlwin
 * PR 14240: Added support for 3560
 *
 * Hydra_Software_Devel/11   12/27/04 11:08a brianlee
 * PR13678: Fixed baudrate register calculation to be more accurate.
 *
 * Hydra_Software_Devel/10   6/11/04 11:05a brianlee
 * PR11179: Set callback to NULL if interrupt is disabled.
 *
 * Hydra_Software_Devel/9   5/24/04 10:10a brianlee
 * PR11179: Check to see if callback is not NULL before destorying it.
 *
 * Hydra_Software_Devel/8   3/26/04 4:32p brianlee
 * PR8971: Remove BDBG_ASSERT() for malloc failure.
 *
 * Hydra_Software_Devel/7   12/29/03 3:59p marcusk
 * PR9117: Updated with changes required to support interrupt ids rather
 * than strings.
 *
 * Hydra_Software_Devel/6   11/4/03 6:54p brianlee
 * Get rid of enter/leave macros.
 *
 * Hydra_Software_Devel/5   9/30/03 11:20a brianlee
 * Fixed a warning.
 *
 * Hydra_Software_Devel/4   9/24/03 2:11p brianlee
 * Changed the names of header files.
 *
 * Hydra_Software_Devel/3   9/19/03 1:58p brianlee
 * Fixed warnings from Midas build.
 *
 * Hydra_Software_Devel/2   9/16/03 6:30p brianlee
 * Writing to IRQ0_IRQEN register does not require coreoffset.
 *
 * Hydra_Software_Devel/1   9/16/03 5:00p brianlee
 * Initial version.
 *
 ***************************************************************************/
#include "bstd.h"
#include "burt.h"
#include "burt_priv.h"
#include "bchp_common.h"
#include "bchp_uarta.h"
#include "bchp_uartb.h" 

#ifdef BCHP_UARTC_REG_START
    #include "bchp_uartc.h"
    #define MAX_URT_CHANNELS 3
#else
    #define MAX_URT_CHANNELS 2
#endif

#include "bchp_irq0.h"
#include "bchp_int_id_irq0.h"

BDBG_MODULE(burt);

#define	DEV_MAGIC_ID			((BERR_URT_ID<<16) | 0xFACE)

#define	BURT_CHK_RETCODE( rc, func )		\
do {										\
	if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
	{										\
		goto done;							\
	}										\
} while(0)

#define UART_CLOCK_FREQ					27000000  /* 27MHz input clock */
#define BITRATE_TO_BAUDRATE( bit )		(((UART_CLOCK_FREQ/8/(bit) + 1)/2) - 1)
#define BAUDRATE_TO_BITRATE( baud )		(UART_CLOCK_FREQ / (16 * (baud + 1)))

#define UART_TX_FIFO_SIZE			32

/*******************************************************************************
*
*	Private Module Handles
*
*******************************************************************************/

typedef struct BURT_P_Handle
{
	uint32_t		magicId;					/* Used to check if structure is corrupt */
	BCHP_Handle 	hChip;
	BREG_Handle		hRegister;
	BINT_Handle 	hInterrupt;
	unsigned int 	maxChnNo;
	BURT_ChannelHandle hUrtChn[MAX_URT_CHANNELS];
} BURT_P_Handle;

typedef struct BURT_P_ChannelHandle
{
	uint32_t 			magicId;					/* Used to check if structure is corrupt */
	BURT_Handle 		hUrt;
	uint32_t	 		chnNo;
	uint32_t 			coreOffset;
	BKNI_EventHandle	hChnTxEvent;
	BKNI_EventHandle	hChnRxEvent;
	BINT_CallbackHandle hChnCallback;
	bool				intMode;					/* If true, rx/txIntMode is not used. */
	bool				rxIntMode;					/* used only if intMode is false */
	bool				txIntMode;					/* used only if intMode is false */
    uint8_t             savedLsr;                   /* OR error bits into here so they are not lost */
	BINT_CallbackFunc 	intCallbackFunc ;
} BURT_P_ChannelHandle;

/* chip has some 16550 style UARTS */
static void BURT_P_EnableFifo(BURT_ChannelHandle hChn);
static uint32_t BURT_P_ReadLsr(BURT_ChannelHandle hChn, uint32_t *pErrLsr);
static uint32_t BURT_P_ReadLsr_Isr(BURT_ChannelHandle hChn, uint32_t *pErrLsr);

/*******************************************************************************
*
*	Default Module Settings
*
*******************************************************************************/
static const BURT_Settings defUrtSettings = NULL;

static const BURT_ChannelSettings defUrt0ChnSettings =
{
	true,
	true,
	115200,
	BURT_DataBits_eDataBits8,
	BURT_Parity_eNone,
	true,
	false,			/* rxIntMode is set to false for backward compatibility. */
	false,			/* txIntMode is set to false for backward compatibility. */
	BURT_StopBits_eOneBit
};

static const BURT_ChannelSettings defUrt1ChnSettings =
{
	true,
	true,
	115200,
	BURT_DataBits_eDataBits8,
	BURT_Parity_eNone,
	true,
	false,			/* rxIntMode is set to false for backward compatibility. */
	false,			/* txIntMode is set to false for backward compatibility. */
	BURT_StopBits_eOneBit
};

#if (MAX_URT_CHANNELS >= 3)
static const BURT_ChannelSettings defUrt2ChnSettings =
{
	true,
	true,
	115200,
	BURT_DataBits_eDataBits8,
	BURT_Parity_eNone,
	true,
	false,			/* rxIntMode is set to false for backward compatibility. */
	false,			/* txIntMode is set to false for backward compatibility. */
	BURT_StopBits_eOneBit
};
#endif

#if (MAX_URT_CHANNELS >= 4)
static const BURT_ChannelSettings defUrt3ChnSettings =
{
	true,
	true,
	115200,
	BURT_DataBits_eDataBits8,
	BURT_Parity_eNone,
	true,
	false,			/* rxIntMode is set to false for backward compatibility. */
	false,			/* txIntMode is set to false for backward compatibility. */
	BURT_StopBits_eOneBit
};
#endif

static const BINT_Id IntId[] =
{
	BCHP_INT_ID_ua_irqen,
#if (MAX_URT_CHANNELS >=2)
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_UART)
	BCHP_INT_ID_uartb_irqen,
#else
	BCHP_INT_ID_ub_irqen,
#endif
#endif
#if (MAX_URT_CHANNELS >=3)
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_UART)
	BCHP_INT_ID_uartc_irqen,
#else
	BCHP_INT_ID_uc_irqen,
#endif
#endif
#if (MAX_URT_CHANNELS >=4)
	BCHP_INT_ID_ud_irqen,
#endif
};
/*******************************************************************************
*
*	Public Module Functions
*
*******************************************************************************/
void BURT_RegisterCallback(
	BURT_ChannelHandle	hChn,			/* Device channel handle */
	BINT_CallbackFunc 	callbackFunc	/* callback function to register */
	)
{
	BDBG_ASSERT( hChn );
	BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

	hChn->intCallbackFunc = callbackFunc;
}

BERR_Code BURT_Open(
	BURT_Handle *pUrt,					/* [output] Returns handle */
	BCHP_Handle hChip,					/* Chip handle */
	BREG_Handle hRegister,				/* Register handle */
	BINT_Handle hInterrupt,				/* Interrupt handle */
	const BURT_Settings *pDefSettings	/* Default settings */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
 	BURT_Handle hDev;
	unsigned int chnIdx;


	/* Sanity check on the handles we've been given. */
	BDBG_ASSERT( hChip );
	BDBG_ASSERT( hRegister );
	BDBG_ASSERT( hInterrupt );
	BSTD_UNUSED( pDefSettings );

	/* Alloc memory from the system heap */
	hDev = (BURT_Handle) BKNI_Malloc( sizeof( BURT_P_Handle ) );
	if( hDev == NULL )
	{
		*pUrt = NULL;
		retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		BDBG_ERR(("BURT_Open: BKNI_malloc() failed\n"));
		goto done;
	}

	hDev->magicId	= DEV_MAGIC_ID;
	hDev->hChip		= hChip;
	hDev->hRegister = hRegister;
	hDev->hInterrupt = hInterrupt;
	hDev->maxChnNo	= MAX_URT_CHANNELS;
	for( chnIdx = 0; chnIdx < hDev->maxChnNo; chnIdx++ )
	{
		hDev->hUrtChn[chnIdx] = NULL;
	}

	*pUrt = hDev;

done:
	return( retCode );
}

BERR_Code BURT_Close(
	BURT_Handle hDev					/* Device handle */
	)
{
	BERR_Code retCode = BERR_SUCCESS;


	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
	BKNI_Free( (void *) hDev );

	return( retCode );
}

BERR_Code BURT_GetDefaultSettings(
	BURT_Settings *pDefSettings,		/* [output] Returns default setting */
	BCHP_Handle hChip					/* Chip handle */
	)
{
	BERR_Code retCode = BERR_SUCCESS;

	BSTD_UNUSED(hChip);


	*pDefSettings = defUrtSettings;

	return( retCode );
}

BERR_Code BURT_GetTotalChannels(
	BURT_Handle hDev,					/* Device handle */
	unsigned int *totalChannels			/* [output] Returns total number downstream channels supported */
	)
{
	BERR_Code retCode = BERR_SUCCESS;


	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

	*totalChannels = hDev->maxChnNo;

	return( retCode );
}

BERR_Code BURT_GetChannelDefaultSettings(
	BURT_Handle hDev,					/* Device handle */
	unsigned int channelNo,				/* Channel number to default setting for */
    BURT_ChannelSettings *pChnDefSettings /* [output] Returns channel default setting */
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
			*pChnDefSettings = defUrt0ChnSettings;
			break;

		case 1:
			*pChnDefSettings = defUrt1ChnSettings;
			break;

#if (MAX_URT_CHANNELS >= 3)
		case 2:
			*pChnDefSettings = defUrt2ChnSettings;
			break;
#endif

#if (MAX_URT_CHANNELS >= 4)
		case 3:
			*pChnDefSettings = defUrt3ChnSettings;
			break;
#endif

		default:
			retCode = BERR_INVALID_PARAMETER;
			break;

	}

	return( retCode );
}

BERR_Code BURT_OpenChannel(
	BURT_Handle hDev,					/* Device handle */
	BURT_ChannelHandle *phChn,			/* [output] Returns channel handle */
	unsigned int channelNo,				/* Channel number to open */
	const BURT_ChannelSettings *pChnDefSettings /* Channel default setting */
	)
{
	BERR_Code 			retCode = BERR_SUCCESS;
 	BURT_ChannelHandle	hChnDev;
	uint32_t			lval;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_UART)
	uint32_t			unIndex;
	uint8_t			ucDummy;
#endif
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

	hChnDev = NULL;

	if( channelNo < hDev->maxChnNo )
	{
		if( hDev->hUrtChn[channelNo] == NULL )
		{
			/* Alloc memory from the system heap */
			hChnDev = (BURT_ChannelHandle) BKNI_Malloc( sizeof( BURT_P_ChannelHandle ) );
			if( hChnDev == NULL )
			{
				*phChn = NULL;
				retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
				BDBG_ERR(("BURT_OpenChannel: BKNI_malloc() failed\n"));
				goto done;
			}

			BURT_CHK_RETCODE( retCode, BKNI_CreateEvent( &(hChnDev->hChnTxEvent) ) );
			BURT_CHK_RETCODE( retCode, BKNI_CreateEvent( &(hChnDev->hChnRxEvent) ) );
			hChnDev->magicId 	= DEV_MAGIC_ID;
			hChnDev->hUrt		= hDev;
			hChnDev->chnNo		= channelNo;
			hDev->hUrtChn[channelNo] = hChnDev;
            switch (channelNo) {
                case 0:
                    hChnDev->coreOffset = 0;
                    break;
                case 1:
                    hChnDev->coreOffset = BCHP_UARTB_RBR - BCHP_UARTA_RBR;
                    break;
#if (MAX_URT_CHANNELS >=3)
                case 2:
                    hChnDev->coreOffset = BCHP_UARTC_RBR - BCHP_UARTA_RBR;
                    break;
#endif
#if (MAX_URT_CHANNELS >=4)
                case 3:
                    hChnDev->coreOffset = BCHP_UARTD_RBR - BCHP_UARTA_RBR;
                    break;
#endif
            }

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_UART)
			/* Work Around:
			* Issue: The boot process stopped when CPU received a data through NEXUS-UART during kernel booting. 
			* Action: Read the dummy data of fifo buffer before uart enabled.
			*/
			for(unIndex=0; unIndex<UART_TX_FIFO_SIZE; unIndex++)
			{
				ucDummy = (uint8_t)(BREG_Read32_isr(hDev->hRegister, hChnDev->coreOffset + BCHP_UARTA_RBR));
			}
#endif

			BURT_EnableTxInt (hChnDev, false);			/* turn off ints */
			BURT_EnableRxInt (hChnDev, false);			/* turn off ints */
            hChnDev->savedLsr = 0;
			BREG_Write32( hDev->hRegister, (hChnDev->coreOffset + BCHP_UARTA_LCR), 0 );
            BURT_P_EnableFifo (hChnDev);
			BURT_P_SetBaudRate (hChnDev, pChnDefSettings->baud);
			BURT_P_SetDataBits (hChnDev, pChnDefSettings->bits);
			BURT_P_SetParity   (hChnDev, pChnDefSettings->parity);
			BURT_P_SetStopBits  (hChnDev, pChnDefSettings->stopBits);
			BURT_P_EnableTxRx  (hChnDev, pChnDefSettings->txEnable, pChnDefSettings->rxEnable);
			/*
			 * Enable interrupt for this channel
			 * Initialize callback function to NULL. This callback is
			 * used instead of posting event during interrupt handling.
			 */
			hChnDev->intMode = pChnDefSettings->intMode;
			hChnDev->rxIntMode = pChnDefSettings->rxIntMode;
			hChnDev->txIntMode = pChnDefSettings->txIntMode;
			hChnDev->intCallbackFunc = NULL;
			if ((hChnDev->intMode == true)  ||
				(hChnDev->rxIntMode == true) ||
				(hChnDev->txIntMode == true))
			{
				/*
				 * Register and enable L2 interrupt.
				 * The same callback is used for tx/rx
				 */
				BURT_CHK_RETCODE( retCode, BINT_CreateCallback(
					&(hChnDev->hChnCallback), hDev->hInterrupt, IntId[channelNo],
					BURT_P_HandleInterrupt_Isr, (void *) hChnDev, 0x00 ) );
				BURT_CHK_RETCODE( retCode, BINT_EnableCallback( hChnDev->hChnCallback ) );

				BKNI_EnterCriticalSection();

				/*
				 * Enable URT interrupt in UPG
				 */
				lval = BREG_Read32(hDev->hRegister, BCHP_IRQ0_IRQEN);
				if (channelNo == 0)
					lval |= BCHP_FIELD_DATA(IRQ0_IRQEN, ua_irqen, 1);
				else if (channelNo == 1)
				{
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_UART)
					lval |= BCHP_FIELD_DATA(IRQ0_IRQEN, uartb_irqen, 1);
#else
					lval |= BCHP_FIELD_DATA(IRQ0_IRQEN, ub_irqen, 1);
#endif
				}
#if (MAX_URT_CHANNELS >= 3)
				else if (channelNo == 2)
				{
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_UART)
					lval |= BCHP_FIELD_DATA(IRQ0_IRQEN, uartc_irqen, 1);
#else
					lval |= BCHP_FIELD_DATA(IRQ0_IRQEN, uc_irqen, 1);
#endif
				}
#endif
#if (MAX_URT_CHANNELS >= 4)
				else if (channelNo == 3)
					lval |= BCHP_FIELD_DATA(IRQ0_IRQEN, ud_irqen, 1);
#endif

				BREG_Write32( hDev->hRegister, BCHP_IRQ0_IRQEN, lval );

				/*
				 * Enable RX URT interrupt in URT
				 * DON'T ENABLE TX INT BECAUSE THE FIFO IS EMPTY AND WE'RE GOING TO GET INTERRUPTED RIGHT AWAY.
				 * The caller should enable the interrupt AFTER he writes to the TX FIFO.
				 */
				BURT_EnableTxInt (hChnDev, false);
				if ((hChnDev->intMode == true) ||
					(hChnDev->rxIntMode == true))
				{
					BURT_EnableRxInt (hChnDev, true);
				}
				BKNI_LeaveCriticalSection();
			}
			else
			{
				hChnDev->hChnCallback = NULL;
			}
			*phChn = hChnDev;
		}
		else
		{
			retCode = BURT_ERR_NOTAVAIL_CHN_NO;
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
			BKNI_DestroyEvent( hChnDev->hChnTxEvent );
			BKNI_DestroyEvent( hChnDev->hChnRxEvent );
			BKNI_Free( hChnDev );
			hDev->hUrtChn[channelNo] = NULL;
			*phChn = NULL;
		}
	}
	return( retCode );
}

BERR_Code BURT_CloseChannel(
	BURT_ChannelHandle hChn			/* Device channel handle */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
	BURT_Handle hDev;
	unsigned int chnNo;


	BDBG_ASSERT( hChn );
	BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

	hDev = hChn->hUrt;
	/*
	 * Disable interrupt for this channel
	 */
	BKNI_EnterCriticalSection();
	BURT_EnableTxInt (hChn, false);			/* turn off ints */
	BURT_EnableRxInt (hChn, false);			/* turn off ints */
	BURT_P_EnableTxRx    (hChn, false, false);
	BKNI_LeaveCriticalSection();

	if (hChn->hChnCallback)
	{
		BURT_CHK_RETCODE( retCode, BINT_DisableCallback( hChn->hChnCallback ) );
		BURT_CHK_RETCODE( retCode, BINT_DestroyCallback( hChn->hChnCallback ) );
	}

	BKNI_DestroyEvent( hChn->hChnTxEvent );
	BKNI_DestroyEvent( hChn->hChnRxEvent );
	chnNo = hChn->chnNo;
	BKNI_Free( hChn );
	hDev->hUrtChn[chnNo] = NULL;

done:
	return( retCode );
}

BERR_Code BURT_GetDevice(
	BURT_ChannelHandle hChn,			/* Device channel handle */
	BURT_Handle *phDev					/* [output] Returns Device handle */
	)
{
	BERR_Code retCode = BERR_SUCCESS;

	BDBG_ASSERT( hChn );
	BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

	*phDev = hChn->hUrt;

	return( retCode );
}

BERR_Code BURT_GetTxEventHandle(
	BURT_ChannelHandle hChn,			/* Device channel handle */
	BKNI_EventHandle *phEvent			/* [output] Returns event handle */
	)
{
	BERR_Code retCode = BERR_SUCCESS;

	BDBG_ASSERT( hChn );
	BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

	*phEvent = hChn->hChnTxEvent;

	return( retCode );
}

BERR_Code BURT_GetRxEventHandle(
	BURT_ChannelHandle hChn,			/* Device channel handle */
	BKNI_EventHandle *phEvent			/* [output] Returns event handle */
	)
{
	BERR_Code retCode = BERR_SUCCESS;

	BDBG_ASSERT( hChn );
	BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

	*phEvent = hChn->hChnRxEvent;

	return( retCode );
}

bool BURT_IsRxDataAvailable(
	BURT_ChannelHandle	hChn			/* Device channel handle */
	)
{
	uint32_t 	lval;
	BURT_Handle	hDev;
	bool		dataAvail;

	BDBG_ASSERT( hChn );
	BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

	hDev = hChn->hUrt;

	lval = BURT_P_ReadLsr(hChn, 0);
	dataAvail = (lval & BCHP_UARTA_LSR_DR_MASK) ? true : false;

	return dataAvail;
}

bool BURT_IsRxDataAvailable_Isr(
	BURT_ChannelHandle	hChn			/* Device channel handle */
	)
{
	uint32_t 	lval;
	BURT_Handle	hDev;
	bool		dataAvail;

	BDBG_ASSERT( hChn );
	BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

	hDev = hChn->hUrt;

	lval = BURT_P_ReadLsr_Isr(hChn, 0);
	dataAvail = (lval & BCHP_UARTA_LSR_DR_MASK) ? true : false;

	return dataAvail;
}

uint32_t BURT_GetAvailTxFifoCnt (
	BURT_ChannelHandle	hChn			/* Device channel handle */
	)
{
	uint32_t 	lval, byteCnt;
	BURT_Handle	hDev;

	BDBG_ASSERT( hChn );
	BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

	hDev = hChn->hUrt;

	lval = BURT_P_ReadLsr(hChn, 0);
	byteCnt = (lval & BCHP_UARTA_LSR_THRE_MASK) ? UART_TX_FIFO_SIZE : 0;

	return byteCnt;
}

uint32_t BURT_GetAvailTxFifoCnt_Isr(
	BURT_ChannelHandle	hChn			/* Device channel handle */
	)
{
	uint32_t 	lval, byteCnt;
	BURT_Handle	hDev;

	BDBG_ASSERT( hChn );
	BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

	hDev = hChn->hUrt;

	lval = BURT_P_ReadLsr_Isr(hChn, 0);
	byteCnt = (lval & BCHP_UARTA_LSR_THRE_MASK) ? UART_TX_FIFO_SIZE : 0;

	return byteCnt;
}

void BURT_EnableTxInt(
	BURT_ChannelHandle 	hChn,			/* Device channel handle */
	bool				enableTxInt		/* enable flag for transmitter interrupt */
	)
{
	uint32_t	lval;
	BURT_Handle	hDev;

	hDev = hChn->hUrt;

	lval = BREG_Read32(hDev->hRegister, hChn->coreOffset + BCHP_UARTA_IER);
	if (enableTxInt)
	{
		/* Enable TX int */
		lval |= BCHP_UARTA_IER_ETBEI_MASK;
	}
	else
	{
		/* Disable TX int */
		lval &= ~BCHP_UARTA_IER_ETBEI_MASK;
	}
	BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_UARTA_IER, lval);
}

void BURT_EnableRxInt(
	BURT_ChannelHandle 	hChn,			/* Device channel handle */
	bool				enableRxInt		/* enable flag for receiver interrupt */
	)
{
	uint32_t	lval;
	BURT_Handle	hDev;

	hDev = hChn->hUrt;


	lval = BREG_Read32(hDev->hRegister, hChn->coreOffset + BCHP_UARTA_IER);
	if (enableRxInt)
	{
		/* Enable RX int */
		lval |= BCHP_UARTA_IER_ERBFI_MASK;
	}
	else
	{
		/* Disable RX int */
		lval &= ~BCHP_UARTA_IER_ERBFI_MASK;
	}
	BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_UARTA_IER, lval);
}

BERR_Code BURT_Read_Isr (
	BURT_ChannelHandle	hChn,			/* Device channel handle */
	uint8_t				*data,			/* pointer to memory to store data */
	uint32_t			numBytes,		/* number of bytes to read */
	uint32_t			*bytesRead,		/* [output] number of actual bytes read */
	BURT_RxError		*rxError		/* [output] receive error code */
	)
{
	BERR_Code 		retCode = BERR_SUCCESS;
	uint32_t 		loopCnt = 0;
	BURT_Handle		hDev;

	BDBG_ASSERT( hChn );
	BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

	hDev = hChn->hUrt;

	while (numBytes && (BURT_IsRxDataAvailable_Isr(hChn)==true))
	{
		*data = (uint8_t)(BREG_Read32_isr(hDev->hRegister, hChn->coreOffset + BCHP_UARTA_RBR));
		data++;
		loopCnt++;
		numBytes--;
	}
	*bytesRead = loopCnt;				/* actual bytes read */

	*rxError = BURT_GetRxError_Isr(hChn);
	if (*rxError != BURT_RxError_eNoError)
	{
		retCode = BURT_ERR_RX_ERROR;
		goto done;
	}

done:
	return( retCode );
}

BERR_Code BURT_Read (
	BURT_ChannelHandle	hChn,			/* Device channel handle */
	uint8_t				*data,			/* pointer to memory to store data */
	uint32_t			numBytes,		/* number of bytes to read */
	uint32_t			*bytesRead,		/* [output] number of actual bytes read */
	BURT_RxError		*rxError		/* [output] receive error code */
	)
{
	BERR_Code 		retCode = BERR_SUCCESS;
	uint32_t 		loopCnt = 0;
	BURT_Handle		hDev;

	BDBG_ASSERT( hChn );
	BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

	hDev = hChn->hUrt;

	while (numBytes && (BURT_IsRxDataAvailable(hChn)==true))
	{
		*data = (uint8_t)(BREG_Read32(hDev->hRegister, hChn->coreOffset + BCHP_UARTA_RBR));
		data++;
		loopCnt++;
		numBytes--;
	}
	*bytesRead = loopCnt;				/* actual bytes read */

	*rxError = BURT_GetRxError (hChn);
	if (*rxError != BURT_RxError_eNoError)
	{
		retCode = BURT_ERR_RX_ERROR;
		goto done;
	}

	/*
	 * Re-enable RX interrupt
	 */
	if ((hChn->intMode) || (hChn->rxIntMode))
		BURT_EnableRxInt (hChn, true);

done:
	return( retCode );
}

BERR_Code BURT_Write (
	BURT_ChannelHandle	hChn,			/* Device channel handle */
	uint8_t			*data,				/* pointers to data to send */
	uint32_t		numBytes			/* number of bytes to write */
	)
{
	BERR_Code 		retCode = BERR_SUCCESS;
	uint32_t 		fifoAvail, lval;
	uint8_t			bval;
	BURT_Handle		hDev;

	BDBG_ASSERT( hChn );
	BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

	hDev = hChn->hUrt;
	if (!numBytes)
		goto done;

	fifoAvail = BURT_GetAvailTxFifoCnt (hChn);
	if (numBytes > fifoAvail)
	{
		retCode = BURT_ERR_TX_FIFO_NOT_AVAIL;
		goto done;
	}

	while (numBytes--)
	{
		bval = *data++;
		lval = (uint32_t)bval;
		BREG_Write32 (hDev->hRegister, (hChn->coreOffset + BCHP_UARTA_RBR), lval);
	}

	/*
	 * Now that we've stuffed the FIFO, enable the TX int
	 */
	if ((hChn->intMode) || (hChn->txIntMode))
		BURT_EnableTxInt (hChn, true);
done:
	return( retCode );
}

BURT_RxError BURT_GetRxError (
	BURT_ChannelHandle	hChn			/* Device channel handle */
	)
{
	uint32_t 		lval;
	BURT_Handle		hDev;
	BURT_RxError 	rxError;

	BDBG_ASSERT( hChn );
	BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

	hDev = hChn->hUrt;

    BURT_P_ReadLsr(hChn, &lval);
	if( lval & BCHP_UARTA_LSR_OE_MASK )
		rxError = BURT_RxError_eReceiverOverRun;
	else if( lval & BCHP_UARTA_LSR_FE_MASK )
		rxError = BURT_RxError_eReceiverFrameError;
	else if( lval & BCHP_UARTA_LSR_PE_MASK )
		rxError = BURT_RxError_eReceiverParityError;
	else
		rxError = BURT_RxError_eNoError;

	return rxError;
}

BURT_RxError BURT_GetRxError_Isr (
	BURT_ChannelHandle	hChn			/* Device channel handle */
	)
{
	uint32_t 		lval;
	BURT_Handle		hDev;
	BURT_RxError 	rxError;

	BDBG_ASSERT( hChn );
	BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

	hDev = hChn->hUrt;

    BURT_P_ReadLsr_Isr(hChn, &lval);
	if( lval & BCHP_UARTA_LSR_OE_MASK )
		rxError = BURT_RxError_eReceiverOverRun;
	else if( lval & BCHP_UARTA_LSR_FE_MASK )
		rxError = BURT_RxError_eReceiverFrameError;
	else if( lval & BCHP_UARTA_LSR_PE_MASK )
		rxError = BURT_RxError_eReceiverParityError;
	else
		rxError = BURT_RxError_eNoError;

	return rxError;
}

/*******************************************************************************
*
*	Private Module Functions
*
*******************************************************************************/
void BURT_ResetTxRx(
	BURT_ChannelHandle 	hChn			/* Device channel handle */
	)
{
	BURT_Handle	hDev;

	hDev = hChn->hUrt;

	/* reset xmit and rcvr fifo's */
	BREG_Write32 (hDev->hRegister, (hChn->coreOffset + BCHP_UARTA_IIR), 7);	/* Note:  BCHP_UARTA_IIR shares same address as BCHP_UARTA_FCR */
}

void BURT_P_EnableTxRx(
	BURT_ChannelHandle 	hChn,			/* Device channel handle */
	bool				enableTx,		/* enable flag for transmitter */
	bool				enableRx		/* enable flag for receiver */
	)
{
	uint32_t	lval;
	BURT_Handle	hDev;

	hDev = hChn->hUrt;

    BSTD_UNUSED(lval);
    BSTD_UNUSED(enableTx);
    BSTD_UNUSED(enableRx);

	/* Enable fifo, reset xmit and rcvr fifo's */
	BREG_Write32 (hDev->hRegister, (hChn->coreOffset + BCHP_UARTA_IIR), 7);	/* Note:  BCHP_UARTA_IIR shares same address as BCHP_UARTA_FCR */
}

void BURT_P_SetBaudRate(
	BURT_ChannelHandle 	hChn,			/* Device channel handle */
	uint32_t			baud
)
{
	uint32_t	lval, bitRate;
	BURT_Handle	hDev;

	hDev = hChn->hUrt;

    BSTD_UNUSED(bitRate);

	/* Set DLAB bit to write DLL and DLH registers. */
	lval = BREG_Read32 (hDev->hRegister, hChn->coreOffset + BCHP_UARTA_LCR);
	lval &= ~BCHP_MASK(UARTA_LCR, DLAB);
	lval |= BCHP_FIELD_DATA(UARTA_LCR, DLAB, 1);
	BREG_Write32 (hDev->hRegister, hChn->coreOffset + BCHP_UARTA_LCR, lval);

	lval = (uint32_t)(5062500 / baud);	/* DLL = 81MHz / (16 * baud) */
	BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_UARTA_RBR, lval & 0xff);		/* Note:  BCHP_UARTA_RBR shares same address as BCHP_UARTA_DLL */
	BREG_Write32(hDev->hRegister, hChn->coreOffset + BCHP_UARTA_IER, lval >> 8);		/* Note:  BCHP_UARTA_IER shares same address as BCHP_UARTA_DLH */

	/* Reset DLAB bit. */
	lval = BREG_Read32 (hDev->hRegister, hChn->coreOffset + BCHP_UARTA_LCR);
	lval &= ~BCHP_MASK(UARTA_LCR, DLAB);
	lval |= BCHP_FIELD_DATA(UARTA_LCR, DLAB, 0);
	BREG_Write32 (hDev->hRegister, hChn->coreOffset + BCHP_UARTA_LCR, lval);
}

void BURT_P_SetDataBits(
	BURT_ChannelHandle 	hChn,			/* Device channel handle */
	BURT_DataBits		bits
)
{
	uint32_t	lval;
	BURT_Handle	hDev;

	hDev = hChn->hUrt;

	lval = BREG_Read32 (hDev->hRegister, hChn->coreOffset + BCHP_UARTA_LCR);
	lval &= ~BCHP_MASK(UARTA_LCR, DLS);
	switch(bits)
	{
		case BURT_DataBits_eDataBits5:
			lval |= BCHP_FIELD_DATA(UARTA_LCR, DLS, 0);
			break;
		case BURT_DataBits_eDataBits6:
			lval |= BCHP_FIELD_DATA(UARTA_LCR, DLS, 1);
			break;
		case BURT_DataBits_eDataBits7:
			lval |= BCHP_FIELD_DATA(UARTA_LCR, DLS, 2);
			break;
		case BURT_DataBits_eDataBits8:
		default:
			lval |= BCHP_FIELD_DATA(UARTA_LCR, DLS, 3);
			break;
	}
	BREG_Write32 (hDev->hRegister, hChn->coreOffset + BCHP_UARTA_LCR, lval);
}

void BURT_P_SetParity(
	BURT_ChannelHandle 	hChn,			/* Device channel handle */
	BURT_Parity			parity
)
{
	uint32_t	lval;
	BURT_Handle	hDev;

	hDev = hChn->hUrt;

	lval = BREG_Read32 (hDev->hRegister, hChn->coreOffset + BCHP_UARTA_LCR);
	switch( parity )
	{
		case BURT_Parity_eNone:
			lval &= ~BCHP_MASK(UARTA_LCR, PEN);
			lval |= BCHP_FIELD_DATA(UARTA_LCR, PEN, 0);
			break;

		case BURT_Parity_eOdd:
			lval &= ~(BCHP_MASK(UARTA_LCR, EPS) |
					BCHP_MASK(UARTA_LCR, PEN));
			lval |= BCHP_FIELD_DATA(UARTA_LCR, EPS, 0) |
					BCHP_FIELD_DATA(UARTA_LCR, PEN, 1);
			break;

		case BURT_Parity_eEven:
			lval &= ~(BCHP_MASK(UARTA_LCR, EPS) |
					BCHP_MASK(UARTA_LCR, PEN));
			lval |= BCHP_FIELD_DATA(UARTA_LCR, EPS, 1) |
					BCHP_FIELD_DATA(UARTA_LCR, PEN, 1);
			break;
	}
	BREG_Write32 (hDev->hRegister, hChn->coreOffset + BCHP_UARTA_LCR, lval);
}

void BURT_P_SetStopBits(
	BURT_ChannelHandle 	hChn,			/* Device channel handle */
	BURT_StopBits		stop_bits
)
{
	uint32_t	lval;
	BURT_Handle	hDev;

	hDev = hChn->hUrt;

	lval = BREG_Read32 (hDev->hRegister, hChn->coreOffset + BCHP_UARTA_LCR);
	lval &= ~BCHP_MASK(UARTA_LCR, STOP);
	switch(stop_bits)
	{
		case BURT_StopBits_eOneBit:
			lval |= BCHP_FIELD_DATA(UARTA_LCR, STOP, 0);
			break;
		case BURT_StopBits_eTwoBit:
			lval |= BCHP_FIELD_DATA(UARTA_LCR, STOP, 1);
			break;
		default:
			lval |= BCHP_FIELD_DATA(UARTA_LCR, STOP, 0);
			break;
	}
	BREG_Write32 (hDev->hRegister, hChn->coreOffset + BCHP_UARTA_LCR, lval);
}

static void BURT_P_HandleInterrupt_Isr
(
	void *pParam1,						/* Device channel handle */
	int parm2							/* not used */
)
{
	BURT_ChannelHandle 	hChn;
	BURT_Handle 		hDev;
	uint32_t			lval;

	hChn = (BURT_ChannelHandle) pParam1;
	BDBG_ASSERT( hChn );
	BSTD_UNUSED( parm2 );

	hDev = hChn->hUrt;

	/* Check for transmit interrupt */
	lval = BURT_P_ReadLsr_Isr(hChn, 0);
	if (lval & BCHP_UARTA_LSR_TEMT_MASK)
	{
		BURT_EnableTxInt (hChn, false);				/* turn off TX interrupt */
#ifndef USE_BURT_INT_CALLBACK
		BKNI_SetEvent( hChn->hChnTxEvent );
#else
		if(hChn->intCallbackFunc == NULL)
		{
			BKNI_SetEvent( hChn->hChnTxEvent );
		}
		else
		{
			hChn->intCallbackFunc(pParam1,1); /* tx interrupt callback */
		}
#endif
	}

	/* Check for receive interrupt */
	lval = BURT_P_ReadLsr_Isr(hChn, 0);
	if (lval & BCHP_UARTA_LSR_DR_MASK)
	{
		BURT_EnableRxInt (hChn, false);				/* turn off RX interrupt */
#ifndef USE_BURT_INT_CALLBACK
		BKNI_SetEvent( hChn->hChnRxEvent );
#else
		if(hChn->intCallbackFunc == NULL)
		{
			BKNI_SetEvent( hChn->hChnRxEvent );
		}
		else
		{
			hChn->intCallbackFunc(pParam1,0); /* rx interrupt callback */
		}
#endif
	}

	return;
}

void BURT_P_EnableFifo(
	BURT_ChannelHandle 	hChn			/* Device channel handle */
)
{
    uint32_t fcraddr;
    fcraddr = hChn->coreOffset + BCHP_UARTA_FCR;
    BREG_Write32(hChn->hUrt->hRegister, fcraddr,
                 BCHP_FIELD_DATA(UARTA_FCR, RT, 0) /* first character */
                 | BCHP_FIELD_DATA(UARTA_FCR, TET, 0) /* empty */
                 | BCHP_FIELD_DATA(UARTA_FCR, FIFOE, 1) /* enable */
                 );
}

static uint32_t
BURT_P_ReadLsr_Isr(
    BURT_ChannelHandle  hChn,
    uint32_t *pErrLsr
)
{
    uint32_t lsraddr;
    uint32_t lval;

    lsraddr = hChn->coreOffset + BCHP_UARTA_LSR;
    lval = BREG_Read32(hChn->hUrt->hRegister, lsraddr);
    hChn->savedLsr |= lval;
    if (pErrLsr) {
        *pErrLsr = hChn->savedLsr;
        hChn->savedLsr = 0;
    }
    return lval;
}

static uint32_t
BURT_P_ReadLsr(
    BURT_ChannelHandle  hChn,
    uint32_t *pErrLsr
)
{
    uint32_t lval;
    BKNI_EnterCriticalSection();
    lval = BURT_P_ReadLsr_Isr(hChn, pErrLsr);
    BKNI_LeaveCriticalSection();
    return lval;
}

/* End of file */
