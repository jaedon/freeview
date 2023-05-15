/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bspi.c $
 * $brcm_Revision: Hydra_Software_Devel/99 $
 * $brcm_Date: 9/25/13 4:22p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/spi/src/bspi.c $
 * 
 * Hydra_Software_Devel/99   9/25/13 4:22p vishk
 * SW7231-1268: Remove stdio.h reference
 * 
 * Hydra_Software_Devel/98   9/25/13 1:53p vishk
 * SW7231-1268: Add support for 16-bit transfer for 97429/97241 platforms.
 * 
 * Hydra_Software_Devel/97   9/19/13 10:42a vishk
 * SW7231-1268: Enable support for the second 3461 frontend. This is done
 * by mutexing access to SPI module as a whole due to shared MOSI, MISO
 * and CLK lines.
 * 
 * Hydra_Software_Devel/96   9/18/13 8:35p vishk
 * SW7231-1268: Add support for spi interface on 97241T2SFF board for only
 * one 3461 frontend. Also, speed up the spi interface clock to 13.5Mhz
 * and optimize spi for faster firmware download.
 * 
 * Hydra_Software_Devel/95   9/18/13 8:35p vishk
 * SW7231-1268: Add support for spi interface on 97241T2SFF board for only
 * one 3461 frontend. Also, speed up the spi interface clock to 13.5Mhz
 * and optimize spi for faster firmware download.
 * 
 * Hydra_Software_Devel/94   8/13/13 2:55p jrubio
 * SW7439-20: add 7439 support
 * 
 * Hydra_Software_Devel/93   8/12/13 12:12p bselva
 * SW7362-15: Add support for 7362 platform
 * 
 * Hydra_Software_Devel/92   6/25/13 12:00p agin
 * SW7366-23:  Add SPI support for 7366.
 * 
 * Hydra_Software_Devel/91   3/4/13 10:35a agin
 * SW7435-639:  Fix warning with HIF_MSPI_MAX_CHANNELS.
 * 
 * Hydra_Software_Devel/90   3/2/13 7:04a agin
 * SW7435-639:  Add warnings at open channel time, if opening an HIF_MSPI
 * channel.
 * 
 * Hydra_Software_Devel/89   2/8/13 10:40a brianlee
 * SW7358-475: Get rid of compiler warnings.
 * 
 * Hydra_Software_Devel/88   2/7/13 10:40a brianlee
 * SW7358-475: For HIF SPI channels, enable MSPI control only when reading
 * or writing.  Otherwise, let BSPI have control.
 * 
 * Hydra_Software_Devel/87   1/31/13 4:52p alexc
 * SW7145-27: Add 7145 support
 * 
 * Hydra_Software_Devel/86   12/12/12 11:34a brianlee
 * SW7346-1119: Get rid of a compilation warning for unused variable.
 * 
 * Hydra_Software_Devel/85   10/4/12 7:38p brianlee
 * SW7445-35: Added 7445 support.
 * 
 * Hydra_Software_Devel/84   9/28/12 1:05p brianlee
 * SW7425-3873: Added conditional compile flag for older chips which don't
 * have cont after cmd bit.
 * 
 * Hydra_Software_Devel/83   9/27/12 2:12p brianlee
 * SW7425-3873: Added an API to turn on and off the last byte continue
 * flag.
 * 
 * Hydra_Software_Devel/82   9/24/12 5:16p randyjew
 * SW7563-2: Add 7563 support
 * 
 * Hydra_Software_Devel/81   8/10/12 2:37p brianlee
 * SW7584-23: Added coverity comment.
 * 
 * Hydra_Software_Devel/80   8/10/12 10:59a brianlee
 * SW7584-23: Fixed a typo.
 * 
 * Hydra_Software_Devel/79   7/27/12 11:44a brianlee
 * SW7468-406: Fixed int ID for chip that doesn't have UPG SPI.
 * 
 * Hydra_Software_Devel/78   7/26/12 7:40p brianlee
 * SW7468-406: Use conditional compile for upg spi check instead of chip
 * name.
 * 
 * Hydra_Software_Devel/77   7/26/12 7:32p brianlee
 * SW7468-406: Added support for 7468, which doesn't have UPG SPI.
 * 
 * Hydra_Software_Devel/76   7/24/12 9:53a enavarro
 * SWSATFE-97: dont include bchp_ebi.h on chips that dont have an EBI bus
 * 
 * Hydra_Software_Devel/75   7/23/12 4:03p brianlee
 * SW7358-345: Fixed compile errors.
 * 
 * Hydra_Software_Devel/74   7/23/12 3:46p brianlee
 * SW7358-345: Fixed a compile issue.
 * 
 * Hydra_Software_Devel/73   7/23/12 3:01p brianlee
 * SW7358-345: Added APIs to handle HIF SPI get channels and explicitly
 * check for channel type (UPG SPI vs. HIF SPI). SW7584-40: Added 32-bit
 * and 64-bit transfers and support for 7584 SPI changes.
 * 
 * Hydra_Software_Devel/72   6/27/12 5:11p enavarro
 * SWSATFE-174: added 4538 support
 * 
 * Hydra_Software_Devel/71   4/28/12 4:09p chengs
 * SW7584-23: Add 7584 support.
 * 
 * Hydra_Software_Devel/70   4/26/12 9:30a jrubio
 * SW7360-17: fix typo
 * 
 * Hydra_Software_Devel/69   4/26/12 5:21p bselva
 * SW7360-17: Add support for 7360
 * 
 * Hydra_Software_Devel/68   3/30/12 7:17p brianlee
 * SW7358-257: Fixed a compilation error for non HIF SPI case.
 * 
 * Hydra_Software_Devel/67   3/30/12 4:48p brianlee
 * SW7358-257: Added support for HIF SPI.
 * 
 * Hydra_Software_Devel/66   11/21/11 6:20p mward
 * SW7435-7: Add 7435.
 * 
 * Hydra_Software_Devel/65   10/11/11 2:03p agin
 * SW7429-14:  Added support for 7429.
 * 
 * Hydra_Software_Devel/64   9/13/11 5:20p enavarro
 * SWSATFE-86: added BCM4528 support
 * 
 * Hydra_Software_Devel/63   5/5/11 3:58p agin
 * SW7346-170:  Use MAX_SPI_CHANNELS from bspi.h.
 * 
 * Hydra_Software_Devel/62   5/2/11 2:36p agin
 * SW7346-170:  Change MAX_SPI_CHANNELS for 7346 to 3.
 * 
 * Hydra_Software_Devel/61   3/21/11 7:17a enavarro
 * SWSATFE-97: add support for 4550
 * 
 * Hydra_Software_Devel/60   12/27/10 2:50p xhuang
 * SW7358-29: Add 7358/7552 support
 * 
 * Hydra_Software_Devel/59   12/8/10 6:05p katrep
 * SW7231-4:add support for 7231
 * 
 * Hydra_Software_Devel/58   12/7/10 6:18p jrubio
 * SW7344-9: add 7344/7346 support
 * 
 * Hydra_Software_Devel/57   11/2/10 5:22p hongtaoz
 * SW7425-9: added 7425 support;
 * 
 * Hydra_Software_Devel/56   10/7/10 4:48p nickh
 * SW7422-74: Add 7422 support
 * 
 * Hydra_Software_Devel/55   5/7/10 6:42p jkim
 * CR3548-909: implement get function for SetDTLConfig() and Set
 * RDSCLKConfig()
 * 
 * Hydra_Software_Devel/54   4/23/10 2:29p erickson
 * SW3548-2904: don't write directly to BCHP_IRQ0_SPI_IRQEN because of
 * possible linux SPI flash collision, clean up BCHP_CHIP lists, add _isr
 * to two priv functions
 *
 * Hydra_Software_Devel/53   12/13/09 6:44p rpereira
 * SW7550-41: Fixed compilation issues for 7550
 *
 * Hydra_Software_Devel/52   9/15/09 10:46a rpereira
 * SW7630-45: Adding 7630 support
 *
 * Hydra_Software_Devel/51   9/1/09 9:10p rpereira
 * SW7550-30: Adding 7550 support
 *
 * Hydra_Software_Devel/50   8/20/09 4:28p mward
 * PR55545: Support 7125.
 *
 * Hydra_Software_Devel/49   8/17/09 3:46p mward
 * PR55232: Typo'd '7400' to '7401'.
 *
 * Hydra_Software_Devel/48   8/10/09 5:25p jrubio
 * PR55232: add 7340/7342 support
 *
 * Hydra_Software_Devel/47   4/9/09 5:26p rpereira
 * PR52971: added 7635 support
 *
 * Hydra_Software_Devel/46   3/9/09 4:49p jkim
 * PR50132: remove the dead code
 *
 * Hydra_Software_Devel/45   1/31/09 1:31a jrubio
 * PR51629: add 7336 support
 *
 * Hydra_Software_Devel/44   12/5/08 10:47a farshidf
 * PR47734: add the hChnDev->chnNo  initiialization in BSPI_OpenChannel
 *
 * Hydra_Software_Devel/43   12/3/08 10:22a kaushikb
 * PR49867: Adding support for 7420
 *
 * Hydra_Software_Devel/42   10/22/08 2:28p farshidf
 * PR47943: add the default settings for channel 2
 *
 * Hydra_Software_Devel/41   10/16/08 2:52p farshidf
 * PR47943: Extend magnum SPI PI to allow control of BSPI pins via MSPI
 * core
 *
 * Hydra_Software_Devel/40   10/13/08 3:35p jkim
 * PR46713: modify function prototype per magnum coding guideline
 *
 * Hydra_Software_Devel/39   8/21/08 9:14a fbasso
 * PR 44544: added support for 7601
 *
 * Hydra_Software_Devel/38   6/26/08 11:04a farshidf
 * PR44170:add BKNI_CriticalSection around a register write
 *
 * Hydra_Software_Devel/37   6/23/08 3:53p farshidf
 * PR43831: add 3549/3556 flags around the new code
 *
 * Hydra_Software_Devel/36   6/23/08 3:27p farshidf
 * PR43831: PR43831: remove the space the include name
 * PR43831: move the Master enable/disable  bit to open and close spi
 * PR43831: change the register read/write to BREG
 * PR43831: fix the SPI interface for 3549/3556 by adding new bit settings
 *
 * Hydra_Software_Devel/PR43831/4   6/23/08 3:25p farshidf
 * PR43831: remove the space the include name
 *
 * Hydra_Software_Devel/PR43831/3   6/23/08 3:19p farshidf
 * PR43831: move the Master enable/disable  bit to open and close spi
 *
 * Hydra_Software_Devel/PR43831/2   6/23/08 10:04a farshidf
 * PR43831: change the register read/write to BREG
 *
 * Hydra_Software_Devel/PR43831/1   6/20/08 5:18p farshidf
 * PR43831: fix the SPI interface for 3549/3556 by adding new bit settings
 *
 * Hydra_Software_Devel/35   4/4/08 2:44p farshidf
 * PR39199: Add support for 3548/3556
 *
 * Hydra_Software_Devel/34   11/28/07 11:55a farshidf
 * PR36894: add 7335 support
 *
 * Hydra_Software_Devel/33   10/31/07 2:20p jkim
 * PR14344: Add support for 7325
 *
 * Hydra_Software_Devel/33   10/25/07 11:27a jkim
 * PR14344: Add support for 7325
 *
 * Hydra_Software_Devel/33   10/14/07 3:54p jkim
 * PR14344: add 7325 support
 *
 * Hydra_Software_Devel/32   5/21/07 4:09p jkim
 * PR30844: Add 7405 support
 *
 * Hydra_Software_Devel/31   5/17/07 10:28a jkim
 * PR14344: modified to support slower devices. DTL and RDSCLK are now
 * settable.
 *
 * Hydra_Software_Devel/30   2/16/07 11:34a jkim
 * PR14344: Added 7440 support
 *
 * Hydra_Software_Devel/29   2/2/07 11:21a jkim
 * PR27238: Modify to use the correct IRQ definition
 *
 * Hydra_Software_Devel/28   1/12/07 4:19p jkim
 * PR14344: Add 3563 support
 *
 * Hydra_Software_Devel/27   11/9/06 11:36a jkim
 * PR14344: added 7403 support
 *
 * Hydra_Software_Devel/26   9/19/06 5:20p agin
 * PR24339: Resolve compiler warning for DEBUG=n builds for UPG modules.
 *
 * Hydra_Software_Devel/25   8/9/06 11:08a agin
 * PR23362: Add 3563 support.
 *
 * Hydra_Software_Devel/25   8/9/06 11:08a agin
 * PR23362: Add 3563 support.
 *
 * Hydra_Software_Devel/24   6/15/06 5:13p mward
 * PR21684: Add support for 7118 chip 97118 board
 *
 * Hydra_Software_Devel/23   3/21/06 3:04p jkim
 * PR20326: Add support for 7438
 *
 * Hydra_Software_Devel/22   1/14/06 11:36p agin
 * PR19076: Support BCM7400.
 *
 * Hydra_Software_Devel/21   8/23/05 5:08p jkim
 * PR14344: Adding 7401 support
 *
 * Hydra_Software_Devel/20   5/9/05 11:41a dlwin
 * PR 14698: Resolve "unused ..." warnings.
 *
 * Hydra_Software_Devel/19   3/18/05 6:50p agin
 * PR14520: keypad for 97398
 *
 * Hydra_Software_Devel/18   3/17/05 5:38p dlwin
 * PR 14240: Added support for 3560.
 *
 * Hydra_Software_Devel/17   3/10/05 9:37a dlwin
 * PR 14240: Added support for 3560
 *
 * Hydra_Software_Devel/16   7/12/04 2:15p brianlee
 * PR11723: Get rid of floating point calculation.
 *
 * Hydra_Software_Devel/15   3/26/04 4:32p brianlee
 * PR8971: Remove BDBG_ASSERT() for malloc failure.
 *
 * Hydra_Software_Devel/14   2/9/04 6:42p brianlee
 * PR242: Added support for 16-bit transfer mode.
 *
 * Hydra_Software_Devel/13   1/28/04 10:22a brianlee
 * PR9499: Fixed breg_spi.h file inclusion.
 *
 * Hydra_Software_Devel/12   1/22/04 10:20a brianlee
 * PR242: In BSPI_CloseChannel(), test to see if the callback is NULL
 * before destroying it.
 *
 * Hydra_Software_Devel/11   1/13/04 5:02p brianlee
 * PR9268: Make write structures constant.
 *
 * Hydra_Software_Devel/10   12/29/03 3:59p marcusk
 * PR9117: Updated with changes required to support interrupt ids rather
 * than strings.
 *
 * Hydra_Software_Devel/9   12/5/03 2:45p brianlee
 * PR8865: Check for maximum SPI transfer size of 16.
 *
 * Hydra_Software_Devel/8   11/6/03 9:51a brianlee
 * Added the function BSPI_CloseSpiRegHandle().
 *
 * Hydra_Software_Devel/7   11/4/03 6:56p brianlee
 * Fixed a compilation error.
 *
 * Hydra_Software_Devel/6   11/4/03 6:54p brianlee
 * Get rid of enter/leave macros.
 *
 * Hydra_Software_Devel/5   10/15/03 2:53p brianlee
 * Fixed problem with SPI read and write.
 *
 * Hydra_Software_Devel/4   9/24/03 2:10p brianlee
 * Changed the names of header files.
 *
 * Hydra_Software_Devel/3   9/19/03 1:53p brianlee
 * Fixed warnings from Midas build.
 *
 * Hydra_Software_Devel/2   9/15/03 10:26a brianlee
 * Changed TRUE/FALSE to lower case.
 *
 * Hydra_Software_Devel/1   9/11/03 6:25p brianlee
 * Initial version.
 *
 ***************************************************************************/
#include "bstd.h"

#include "breg_spi_priv.h"
#include "bspi.h"
#include "bspi_priv.h"

#if (HAS_UPG_MSPI==1)
#include "bchp_mspi.h"
#endif

#if (BCHP_CHIP!=4550) && (BCHP_CHIP!=4528) && (BCHP_CHIP!=4538)
#include "bchp_irq0.h"
#endif
#include "bchp_int_id_irq0.h"
#if (BCHP_CHIP==3548) || (BCHP_CHIP==3556) || (BCHP_CHIP==4550) || (BCHP_CHIP==4528) || (BCHP_CHIP==4538) ||HAS_HIF_MSPI
#include "bchp_bspi.h"
#endif
#if (BCHP_CHIP==7422) || (BCHP_CHIP==7425) || (BCHP_CHIP==7429) || (BCHP_CHIP==7344) || \
    (BCHP_CHIP==7346) || (BCHP_CHIP==7231) || (BCHP_CHIP==7358) || (BCHP_CHIP==7366) || (BCHP_CHIP==7552) || \
    (BCHP_CHIP==7435) || (BCHP_CHIP==7360) || (BCHP_CHIP==7584) || (BCHP_CHIP==7563) || (BCHP_CHIP==7445) || \
    (BCHP_CHIP==7145) || (BCHP_CHIP==7362) || (BCHP_CHIP == 7439) 
#include "bchp_int_id_irq0_aon.h"
#endif
#if HAS_HIF_MSPI
#include "bchp_hif_mspi.h"
#include "bchp_int_id_hif_spi_intr2.h"
#endif
#if (BCHP_CHIP!=4550) && (BCHP_CHIP!=4528) && (BCHP_CHIP!=4538)
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

#define SPI_POLLING_INTERVAL        10      /* in usecs */

#define BSPI_P_ACQUIRE_UPG_MUTEX(handle) BKNI_AcquireMutex((handle)->hUpgMutex)
#define BSPI_P_RELEASE_UPG_MUTEX(handle) BKNI_ReleaseMutex((handle)->hUpgMutex)

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
            BSPI_P_ACQUIRE_UPG_MUTEX( hDev );
            BSPI_P_SetClk (hChnDev, pChnDefSettings->baud, pChnDefSettings->clkConfig);
            BSPI_P_RELEASE_UPG_MUTEX( hDev );

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
            BSPI_P_ACQUIRE_UPG_MUTEX( hDev );
            lval = BREG_Read32( hDev->hRegister, (hChnDev->coreOffset + SPI_REG(SPCR2)));
            lval |= SPI_REG(SPCR2_cont_after_cmd_MASK);
            BREG_Write32( hDev->hRegister, (hChnDev->coreOffset + SPI_REG(SPCR2)), lval);
            BSPI_P_RELEASE_UPG_MUTEX( hDev );
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
    BSPI_P_ACQUIRE_UPG_MUTEX( hDev );
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
    BKNI_Free( hChn );
    hDev->hSpiChn[chnNo] = NULL;
    BSPI_P_RELEASE_UPG_MUTEX( hDev );

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

    (*pSpiReg)->context                       = (void *)hChn;
    (*pSpiReg)->BREG_SPI_Write_Func           = BSPI_P_Write;
    (*pSpiReg)->BREG_SPI_Read_Func            = BSPI_P_Read;

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
    BSPI_P_ACQUIRE_UPG_MUTEX( hDev );
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
    BSPI_P_RELEASE_UPG_MUTEX( hDev );
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
    BSPI_P_ACQUIRE_UPG_MUTEX( hDev );
    data = BREG_Read32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR1_LSB)));
    *pDTLConfig = (data & SPI_REG(SPCR1_LSB_DTL_MASK));
    BSPI_P_RELEASE_UPG_MUTEX( hDev );

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
    BSPI_P_ACQUIRE_UPG_MUTEX( hDev );
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR1_LSB)), data );

done:
    BSPI_P_RELEASE_UPG_MUTEX( hDev );

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
    BSPI_P_ACQUIRE_UPG_MUTEX( hDev );
    data = BREG_Read32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR1_MSB)));
    *pRDSCLKConfig = (data & SPI_REG(SPCR1_MSB_RDSCLK_MASK));
    BSPI_P_RELEASE_UPG_MUTEX( hDev );

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
    BSPI_P_ACQUIRE_UPG_MUTEX( hDev );
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR1_MSB)), data );

done:
    BSPI_P_RELEASE_UPG_MUTEX( hDev );

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
    hChn->lastByteContinueEnable = bEnable;
}

/*******************************************************************************
*
*   Private Module Functions
*
*******************************************************************************/
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
    BSPI_P_ACQUIRE_UPG_MUTEX( hDev );

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
    BSPI_P_RELEASE_UPG_MUTEX( hDev );

    return retCode;
}

#if UPG_MSPI_SUPPORTS_16BIT_TRANSFER

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
    uint32_t            lval = 0, i;
    bool isOdd;

    hChn = (BSPI_ChannelHandle)context;
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hSpi;
    BSPI_P_ACQUIRE_UPG_MUTEX( hDev );

    isOdd = length & 0x01;


    /* 16-bit transfer mode. For odd sizes, the last byte is sent on 8-bit mode. */
    lval = BREG_Read32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR0_MSB)));
    lval &= ~SPI_REG(SPCR0_MSB_BitS_MASK);
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR0_MSB)), lval);

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

    if(isOdd){
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

    /* Skip for slower SPI device  */
    if(!isOdd)
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

    if(length==1)
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(ENDQP)), 0 );
    else if(isOdd)
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(ENDQP)), (uint32_t)(length/2) );
    else
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(ENDQP)), (uint32_t)((length/2) - 1) );



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
    BSPI_P_RELEASE_UPG_MUTEX( hDev );
    return retCode;

}

#else

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
    uint32_t            lval = 0, i;
#if (BCHP_CHIP==7584)
    uint32_t            j;
#endif

    hChn = (BSPI_ChannelHandle)context;
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hSpi;
    BSPI_P_ACQUIRE_UPG_MUTEX( hDev );

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

    if ((hChn->bitsPerTxfr == 16) && (length & 0x01))       /* if the length is odd, reject it */
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
        /*
         * 16-bit transfer mode
         */
        lval = BREG_Read32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR0_MSB)));
        lval &= ~SPI_REG(SPCR0_MSB_BitS_MASK);
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(SPCR0_MSB)), lval);

        for (i=0; i<length; i+=2)
        {
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(TXRAM00) + (i * 4)),     (uint32_t)(pData[i]) );
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + SPI_REG(TXRAM00) + (i * 4) + 4), (uint32_t)(pData[i+1]) );

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
    BSPI_P_RELEASE_UPG_MUTEX( hDev );

    return retCode;
}


#endif
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
    uint32_t            lval, timeoutMs, loopCnt;

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
        loopCnt = ((timeoutMs * 1000) / SPI_POLLING_INTERVAL) + 1;
        while (1)
        {
            lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + SPI_REG(MSPI_STATUS)));
            if (lval & SPI_REG(MSPI_STATUS_SPIF_MASK))
                break;

            if (loopCnt == 0)
            {
                retCode = BERR_TIMEOUT;
                goto done;
            }
            BKNI_Delay(SPI_POLLING_INTERVAL);
            loopCnt--;
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

