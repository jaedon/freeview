/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: birb.c $
 * $brcm_Revision: Hydra_Software_Devel/58 $
 * $brcm_Date: 10/5/12 1:14p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/irb/src/birb.c $
 * 
 * Hydra_Software_Devel/58   10/5/12 1:14p agin
 * SWNOOS-576:  Add back PI support for 7550.
 * 
 * Hydra_Software_Devel/57   9/22/12 8:14a agin
 * SW7445-28:  add irb PI support for 7445.
 * 
 * Hydra_Software_Devel/56   4/28/12 3:05p chengs
 * SW7584-23: Add 7584 support.
 * 
 * Hydra_Software_Devel/55   4/26/12 4:21p bselva
 * SW7360-17: Add support for 7360
 * 
 * Hydra_Software_Devel/54   11/21/11 6:19p mward
 * SW7435-7: Add 7435.
 * 
 * Hydra_Software_Devel/53   10/11/11 1:49p jkim
 * SW7429-9: Add 7429 support
 * 
 * Hydra_Software_Devel/53   10/11/11 1:29p jkim
 * SW7429-9: add 7429 support
 * 
 * Hydra_Software_Devel/52   12/27/10 1:59p xhuang
 * SW7358-29: Add 7358/7552 support
 * 
 * Hydra_Software_Devel/51   12/8/10 5:55p katrep
 * SW7231-4:add support for 7231
 * 
 * Hydra_Software_Devel/50   12/7/10 6:14p jrubio
 * SW7344-9: add 7344/7346 support
 * 
 * Hydra_Software_Devel/49   11/2/10 5:00p hongtaoz
 * SW7425-9: added 7425 support;
 * 
 * Hydra_Software_Devel/48   10/13/10 11:53a gmohile
 * SW7408-1 : Add 7408 IR blaster support
 * 
 * Hydra_Software_Devel/47   10/7/10 4:44p nickh
 * SW7422-74: Add 7422 support
 * 
 * Hydra_Software_Devel/46   12/13/09 6:50p rpereira
 * SW7550-41: Fixed compilation issues for 7550
 *
 * Hydra_Software_Devel/45   9/1/09 5:38p rpereira
 * SW7550-30: Added 7550 support
 *
 * Hydra_Software_Devel/44   8/20/09 4:23p mward
 * PR55545: Support 7125.
 *
 * Hydra_Software_Devel/43   8/10/09 4:26p jrubio
 * PR55232: add 7340/7342
 *
 * Hydra_Software_Devel/42   3/6/09 1:23p jkim
 * PR52509: remove global variables
 *
 * Hydra_Software_Devel/41   1/31/09 1:13a jrubio
 * PR51629: add 7336 support
 *
 * Hydra_Software_Devel/40   1/30/09 1:34p jkim
 * PR51617: confrom to the global naming convention and reduce the trailer
 * pulse to 500us on xmp-2 ACK/NAK.
 *
 * Hydra_Software_Devel/39   12/2/08 4:11p kaushikb
 * PR49867: Adding support for 7420
 *
 * Hydra_Software_Devel/38   4/4/08 11:20a farshidf
 * PR39190: add 3548 support
 *
 * Hydra_Software_Devel/37   4/3/08 2:45p brianlee
 * PR35973: Fixed a potential memory resource leak problem in birb_open()
 * function.
 *
 * Hydra_Software_Devel/36   3/27/08 1:10p jkim
 * PR38349: Add trailer pulse for ack/nack
 *
 * Hydra_Software_Devel/35   2/27/08 10:18a jrubio
 * PR39363: fix compile warnings
 *
 * Hydra_Software_Devel/34   2/26/08 10:42a katrep
 * PR29073: Fixed compiler warnings
 *
 * Hydra_Software_Devel/33   2/20/08 1:58p jkim
 * PR37963: Add XMP-2 support
 *
 * Hydra_Software_Devel/32   11/27/07 6:11p farshidf
 * PR36888: add support for 7335
 *
 * Hydra_Software_Devel/31   11/27/07 6:09p farshidf
 * PR36888: Add 7335 support
 *
 * Hydra_Software_Devel/30   10/14/07 3:53p jkim
 * PR14344: add 7325 support
 *
 * Hydra_Software_Devel/29   3/26/07 5:13p katrep
 * PR29073: Added support for 7405
 *
 * Hydra_Software_Devel/28   2/2/07 11:22a jkim
 * PR27238: Modify to use the correct IRQ definition
 *
 * Hydra_Software_Devel/27   1/12/07 4:21p jkim
 * PR14344: Add 3563 support
 *
 * Hydra_Software_Devel/26   12/1/06 4:14p jkim
 * PR14344: fixed incidental error of using 7401 instead 7403
 *
 * Hydra_Software_Devel/25   11/30/06 1:49p jkim
 * PR14344: Add 3563 support
 *
 * Hydra_Software_Devel/24   11/9/06 11:07a jkim
 * PR14344: added 7403 support
 *
 * Hydra_Software_Devel/23   6/15/06 5:13p mward
 * PR21680: Add support for 7118 chip 97118 board
 *
 * Hydra_Software_Devel/22   3/21/06 3:03p jkim
 * PR20326: Add support for 7438
 *
 * Hydra_Software_Devel/21   1/14/06 11:21p agin
 * PR19076: Support BCM7400.
 *
 * Hydra_Software_Devel/20   8/23/05 5:07p jkim
 * PR14344: Adding 7401 support
 *
 * Hydra_Software_Devel/19   4/7/05 10:56a agin
 * PR14698: unused parameters.
 *
 * Hydra_Software_Devel/18   3/10/05 9:39a dlwin
 * PR 14240: Updated to use marco instead of explicit bit set.
 *
 * Hydra_Software_Devel/17   3/10/05 8:51a dlwin
 * PR 14240: Added support for 3560.
 *
 * Hydra_Software_Devel/16   10/7/04 6:35p brianlee
 * PR11333: Modified multiple sequence blast functions to accommodate
 * another case.
 *
 * Hydra_Software_Devel/15   9/22/04 5:59p brianlee
 * PR11333: Fixed various multiple sequence sending functions.
 *
 * Hydra_Software_Devel/14   7/23/04 4:04p brianlee
 * PR11333: Added another type of multiple blast sequences.
 *
 * Hydra_Software_Devel/13   6/14/04 4:04p brianlee
 * PR11386: Fixed test configuration.  Last 4 entries of duration should
 * be doubled because we're going to divide it when writing it to the
 * registers.
 *
 * Hydra_Software_Devel/12   6/7/04 4:13p brianlee
 * PR11386: Make configregisters function public so it can be called by
 * test routines.
 *
 * Hydra_Software_Devel/11   6/3/04 11:17a brianlee
 * PR11333: Added support for sending multiple blast sequences.
 *
 * Hydra_Software_Devel/10   3/26/04 4:32p brianlee
 * PR8971: Remove BDBG_ASSERT() for malloc failure.
 *
 * Hydra_Software_Devel/9   3/4/04 5:34p brianlee
 * PR9958: Added BIRB_Device_eCustom type and the function
 * BIRB_ConfigCustom() to allow user to configure custom IR blaster
 * setting.
 *
 * Hydra_Software_Devel/8   12/29/03 3:59p marcusk
 * PR9117: Updated with changes required to support interrupt ids rather
 * than strings.
 *
 * Hydra_Software_Devel/7   11/4/03 6:51p brianlee
 * Get rid of enter/leave macros.
 *
 * Hydra_Software_Devel/6   10/27/03 10:53a brianlee
 * Fixed long sequence blast mode.
 *
 * Hydra_Software_Devel/5   10/17/03 5:29p brianlee
 * 1.) Fixed writing sequence registers.
 * 2.) In order to use new interrupt, must make sure the irqinh bit is set
 * in blast_control register.  By default, it is 0, which means the old
 * mode is enabled.
 *
 * Hydra_Software_Devel/4   10/3/03 9:53a brianlee
 * Added test configuration.
 *
 * Hydra_Software_Devel/3   9/24/03 11:58a brianlee
 * Changed the names of header files.
 *
 * Hydra_Software_Devel/2   9/19/03 1:42p brianlee
 * Fixed warnings from Midas build.
 *
 * Hydra_Software_Devel/1   9/18/03 4:13p brianlee
 * Initial version.
 *
 *
 ***************************************************************************/
#include "bstd.h"
#include "birb.h"
#include "birb_priv.h"
#include "bchp_irb.h"
#include "bchp_irq0.h"
#include "bchp_int_id_irq0.h"

BDBG_MODULE(birb);

#define DEV_MAGIC_ID            ((BERR_IRB_ID<<16) | 0xFACE)

#define BIRB_CHK_RETCODE( rc, func )        \
do {                                        \
    if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
    {                                       \
        goto done;                          \
    }                                       \
} while(0)

#define MAX_IRB_SEQUENCES           159     /* leave room for 1 start bit */
#define IRB_PAGE_SIZE               40
#define NUMSEQ_GREATER_THAN_80_BIT  0x80

/*******************************************************************************
*
*   Private Data
*
*******************************************************************************/
static const SIrbConfiguration SonyConfiguration =
{
    "sony",                     /* name; */
    MASTER_DIVISOR,             /* masterDivisor */
    CARRIER_CLOCK_DIVISOR,      /* carrierClockDivisor */
    INDEX_CLOCK_DIVISOR,        /* indexClockDivisor */
    0,                          /* noCarrier */
    12/CARRIER_PERIOD,          /* carrierHighCount */
    13/CARRIER_PERIOD,          /* carrierLowCount */
    21,                         /* numberSequences */
    {0,2},                      /* logic0IndexPair */
    {1,2},                      /* logic1IndexPair */
    {                           /* sequenceIndex[] */
        {3,2},                  /* start bit */
        {1,2}, {0,2}, {1,2}, {0,2}, /* data bit 0..3 = 5 */
        {1,2}, {0,2}, {0,2}, {0,2}, /* data bit 4..7 = 1 */
        {1,2}, {0,2}, {1,2}, {1,2}, /* data bit 8..11 = d */
        {1,2}, {0,2}, {0,2}, {1,2}, /* data bit 12..15 = 9*/
        {0,2}, {0,2}, {1,2}, {0,2}  /* data bit 16..19 = 4*/
    },
    {                           /* duration[]; */
        600/INDEX_PERIOD,       /* 0.6ms, '0' ON */
        1200/INDEX_PERIOD,      /* 1.2ms, '1' ON */
        600/INDEX_PERIOD,       /* 0.6ms,  '0', '1', OFF */
        2400/INDEX_PERIOD,      /* 2.4ms,  start bit ON */
        600/INDEX_PERIOD        /* last sequence OFF duration */
                                /* 0.6ms + filler to make 45ms frame duration */
                                /* automatically calculated when sending */
    },
    (FRAME_PERIOD/INDEX_PERIOD),/* frame period A */
    0,                          /* frame period B */
    4,                          /* lastSequenceOffIndex */
    3,                          /* repeatCount */
    0,                           /* repeatStartIndex, repeat whole sequence */
    0                           /* altModCnt */
};

/* Configuration for test64-Bit IR Code */
static const SIrbConfiguration Test64bitConfiguration =
{
    "test64bit",                /* name; */
    MASTER_DIVISOR,             /* masterDivisor */
    CARRIER_CLOCK_DIVISOR,      /* carrierClockDivisor */
    INDEX_CLOCK_DIVISOR,        /* indexClockDivisor */
    0,                          /* noCarrier */
    12/CARRIER_PERIOD,          /* carrierHighCount */
    13/CARRIER_PERIOD,          /* carrierLowCount */
    21,                         /* numberSequences */
    {0,2},                      /* logic0IndexPair */
    {1,2},                      /* logic1IndexPair */
    {                           /* sequenceIndex[] */
        {3,2},                  /* start bit */
        {0,1}, {2,3}, {4,5}, {6,7}, /* data bit 0..3 = 5 */
        {8,9}, {10,11}, {12,13}, {0,2}, /* data bit 4..7 = 1 */
        {0,1}, {2,3}, {4,5}, {6,7}, /* data bit 0..3 = 5 */
        {8,9}, {10,11}, {12,13}, {0,2}, /* data bit 4..7 = 1 */
        {0,1}, {2,3}, {4,5}, {6,7}, /* data bit 0..3 = 5 */
        {8,9}, {10,11}, {12,13}, {0,2}  /* data bit 4..7 = 1 */
    },
    {                           /* duration[]; */
        600/INDEX_PERIOD,       /* 0.6ms, '0' ON */
        1200/INDEX_PERIOD,      /* 1.2ms, '1' ON */
        600/INDEX_PERIOD,       /* 0.6ms,  '0', '1', OFF */
        2400/INDEX_PERIOD,      /* 2.4ms,  start bit ON */
        600/INDEX_PERIOD,       /* last sequence OFF duration */
                                /* 0.6ms + filler to make 45ms frame duration */
                                /* automatically calculated when sending */
        3600/INDEX_PERIOD,      /* 3.4ms */

        1200/INDEX_PERIOD,      /* 1.2ms */
        1200/INDEX_PERIOD,      /* 1.2ms */
        1200/INDEX_PERIOD,      /* 1.2ms */
        2400/INDEX_PERIOD,      /* 2.4ms */

        600/INDEX_PERIOD,       /* 0.6ms (double value) */
        1200/INDEX_PERIOD,      /* 1.2ms (double value) */
        1800/INDEX_PERIOD,      /* 1.8ms (double value) */
        2400/INDEX_PERIOD       /* 2.4ms (double value) */
    },
    (FRAME_PERIOD_64_BIT/INDEX_PERIOD),/* frame period A */
    0,                          /* frame period B */
    4,                          /* lastSequenceOffIndex */
    0,                          /* repeatCount */
    0,                           /* repeatStartIndex, repeat whole sequence */
    0                           /* altModCnt */
};

/* Configuration for GI Remote A IR Code */
static const SIrbConfiguration GIConfiguration =
{
    "GI",                       /* name; */
    MASTER_DIVISOR,             /* masterDivisor */
    CARRIER_CLOCK_DIVISOR,      /* carrierClockDivisor */
    INDEX_CLOCK_DIVISOR,        /* indexClockDivisor */
    0,                          /* noCarrier */
    12/CARRIER_PERIOD,          /* carrierHighCount */
    13/CARRIER_PERIOD,          /* carrierLowCount */
    17,                         /* numberSequences */
    {2,0},                      /* logic0IndexPair */
    {2,1},                      /* logic1IndexPair */
    {                           /* sequenceIndex[] */
        {3,1},                  /* start bit */
        {1,2}, {0,2}, {1,2}, {0,2}, /* data bit 0..3 = 5 */
        {1,2}, {0,2}, {0,2}, {0,2}, /* data bit 4..7 = 1 */
        {1,2}, {0,2}, {1,2}, {1,2}, /* data bit 8..11 = d */
        {1,2}, {0,2}, {0,2}, {1,2}, /* data bit 12..15 = 9*/
        {0,2}, {0,2}, {1,2}, {0,2}  /* data bit 16..19 = 4*/
    },
    {                           /* duration[]; */
        2250/INDEX_PERIOD,      /* 2.25ms, '0' OFF */
        4500/INDEX_PERIOD,      /* 4.50ms, '1' OFF */
        500/INDEX_PERIOD,       /* 0.50ms,  '0', '1', ON */
        9000/INDEX_PERIOD,      /* 9.00ms,  start bit, preamble */
        600/INDEX_PERIOD        /* last sequence OFF duration */
                                /* 0.6ms + filler to make 45ms frame duration */
                                /* automatically calculated when sending */
    },
    (FRAME_PERIOD/INDEX_PERIOD),/* frame period A */
    0,                          /* frame period B */
    4,                          /* lastSequenceOffIndex */
    0,                          /* repeatCount */
    0,                           /* repeatStartIndex, repeat whole sequence */
    0                           /* altModCnt */
};

/* Configuration for Pioneer */
static const SIrbConfiguration PioneerConfiguration =
{
    "pioneer",                  /* name; */
    MASTER_DIVISOR,             /* masterDivisor */
    CARRIER_CLOCK_DIVISOR,      /* carrierClockDivisor */
    INDEX_CLOCK_DIVISOR,        /* indexClockDivisor */
    1,                          /* noCarrier */
    12/CARRIER_PERIOD,          /* carrierHighCount */
    13/CARRIER_PERIOD,          /* carrierLowCount */
    17,                         /* numberSequences */
    {2,0},                      /* logic0IndexPair */
    {2,1},                      /* logic1IndexPair */
    {                           /* sequenceIndex[] */
        {3,4},                  /* start bit */
        {1,2}, {0,2}, {1,2}, {0,2}, /* data bit 0..3 = 5 */
        {1,2}, {0,2}, {0,2}, {0,2}, /* data bit 4..7 = 1 */
        {1,2}, {0,2}, {1,2}, {1,2}, /* data bit 8..11 = d */
        {1,2}, {0,2}, {0,2}, {1,2}, /* data bit 12..15 = 9*/
        {0,2}, {0,2}, {1,2}, {0,2}  /* data bit 16..19 = 4*/
    },
    {                           /* duration[]; */
        /* case 1, no B data bits */
        562/INDEX_PERIOD,       /* 0.562ms, '0' OFF */
        1687/INDEX_PERIOD,      /* 1.687ms, '1' OFF */
        562/INDEX_PERIOD,       /* 0.562ms,  '0', '1', ON */
        9000/INDEX_PERIOD,      /* 9.00ms,  start bit A and B, preamble */
        4500/INDEX_PERIOD,      /* 4.50ms,  start bit A off */
        2250/INDEX_PERIOD,      /* 2.25ms,  start bit B off */
        562/INDEX_PERIOD,       /* last sequence ON duration */
        562/INDEX_PERIOD,       /* last sequence OFF duration for A */
                                /* 0.6ms + filler to make 45ms frame duration */
                                /* automatically calculated when sending */

        /* case 2, B data bits */
        527/INDEX_PERIOD,       /* 0.527ms, '0' OFF */
        1583/INDEX_PERIOD,      /* 1.583ms, '1' OFF */
        528/INDEX_PERIOD,   /* 0.528ms,  '0', '1', ON */
        8440/INDEX_PERIOD,      /* 8.44ms,  start bit A and B, preamble */
        4220/INDEX_PERIOD,      /* 4.22ms,  start bit A off, start bit B off */
        562/INDEX_PERIOD        /* last sequence OFF duration for B */

    },
    (FRAME_PERIOD_PIONEER_ABBB_1/INDEX_PERIOD),     /* frame period A */
    (FRAME_PERIOD_PIONEER_ABBB_1/INDEX_PERIOD),     /* frame period B */
    CASE2_DURATION_INDEX - 1,   /* lastSequenceOffIndex */
    0,                          /* repeatCount */
    0,                           /* repeatStartIndex, repeat whole sequence */
    0                           /* altModCnt */
};

/* Configuration for Pioneer AAAA */
static const SIrbConfiguration PioneerAAAAConfiguration =
{
    "pioneerAAAA",              /* name; */
    MASTER_DIVISOR,             /* masterDivisor */
    CARRIER_CLOCK_DIVISOR,      /* carrierClockDivisor */
    INDEX_CLOCK_DIVISOR,        /* indexClockDivisor */
    1,                          /* noCarrier */
    12/CARRIER_PERIOD,          /* carrierHighCount */
    13/CARRIER_PERIOD,          /* carrierLowCount */
    17,                         /* numberSequences */
    {2,0},                      /* logic0IndexPair */
    {2,1},                      /* logic1IndexPair */
    {                           /* sequenceIndex[] */
        {3,4},                  /* start bit */
        {1,2}, {0,2}, {1,2}, {0,2}, /* data bit 0..3 = 5 */
        {1,2}, {0,2}, {0,2}, {0,2}, /* data bit 4..7 = 1 */
        {1,2}, {0,2}, {1,2}, {1,2}, /* data bit 8..11 = d */
        {1,2}, {0,2}, {0,2}, {1,2}, /* data bit 12..15 = 9*/
        {0,2}, {0,2}, {1,2}, {0,2}  /* data bit 16..19 = 4*/
    },
    {                           /* duration[]; */
        /* case 4, no end pulse, fixed frame time */
        870/INDEX_PERIOD,       /* 0.870ms, '0' OFF, ON */
        900/INDEX_PERIOD,       /* 0.900ms, '1' OFF, ON */

        /* case 5, end pulse, variable frame time */
        305/INDEX_PERIOD,       /* 0.305ms,  '0', OFF, ON, '1' ON */
        2137/INDEX_PERIOD,      /* 2.137ms,  '1', OFF */
        0,                      /* case 4 last sequence OFF duration */
        305/INDEX_PERIOD,       /* 0.305ms, end pulse */
        26000/INDEX_PERIOD      /* case 5 last sequence OFF duration, pause time */
    },
    (FRAME_PERIOD_PIONEER_AAAA_4/INDEX_PERIOD),     /* frame period A */
    (FRAME_PERIOD_PIONEER_AAAA_4/INDEX_PERIOD),     /* frame period B */
    0,                          /* lastSequenceOffIndex */
    0,                          /* repeatCount */
    0,                           /* repeatStartIndex, repeat whole sequence */
    0                           /* altModCnt */
};

#if 0
/* this is moved inside handle */
static SIrbConfiguration CustomConfiguration;
#endif

/* Configuration for XMP-2 Station with 38KHz carrier */
static const SIrbConfiguration Xmp2StationConfiuration = {
    "xmp2_station_38k",         /* name; */
    1,                          /* masterDivisor */
    2,                          /* carrierClockDivisor */
    1,                          /* indexClockDivisor */
    0,                          /* noCarrier */
    118,                        /* carrierHighCount */
    237,                        /* carrierLowCount */
    17,                         /* numberSequences */
    {0,0},                      /* not used, logic0IndexPair */
    {0,0},                      /* not used, logic1IndexPair */
    {   /* sequenceIndex[] */
        {DATA_PULSE,DATA4}, {ZERO,DELTA1}, /* 1st nibble = 5 */
        {DATA_PULSE,DATA2}, {ZERO,ZERO},   /* 2nd nibble = 2 */
        {DATA_PULSE,DATA4}, {ZERO,DELTA1}, /* 1st nibble = 5 */
        {DATA_PULSE,DATA2}, {ZERO,ZERO},   /* 2nd nibble = 2 */
        {DATA_PULSE,DATA4}, {ZERO,DELTA1}, /* 1st nibble = 5 */
        {DATA_PULSE,DATA2}, {ZERO,ZERO},   /* 2nd nibble = 2 */
        {DATA_PULSE,DATA4}, {ZERO,DELTA1}, /* 1st nibble = 5 */
        {DATA_PULSE,DATA2}, {ZERO,ZERO},   /* 2nd nibble = 2 */
        {DATA_PULSE,ZERO}               /* trailer pulse */
    },
    {                           /* duration[] */
        3690,                   /* 0: deltaT-1 */
        3691,                   /* 1: deltaT, 136,71us*/
        3692,                   /* 2: deltaT+1*/
        20509,                  /* 3: D(0), low period for data 0 */
        27892,                  /* 4: D(2), low period for data 2 */
        35274,                  /* 5: D(4), low period for data 4 */
        42656,                  /* 6: D(6), low period for data 6 */
        50039,                  /* 7: D(8), low period for data 8 */
        57421,                  /* 8: D(10), low period for data 10 */
        64803,                  /* 9: D(12), low period for data 12 */
                                /* the remain 4 are double-time values */
        72186,              /* 10: D(14), low period for data 14 */
        5680,                   /* 11: D(DataPulse), high period of data symbol */
        13500,              /* 500us. */ /* 22720,(840us) */              /* 12: D(TrailerPulse), high period of Trailer pulse */
        0                   /* 13: Zero */
    },
    0,                          /* not used, frame period A*/
    0,                          /* not used, frame period B*/
    0,                          /* not used, lastSequenceOffIndex */
    0,                          /* repeatCount */
    0,                          /* repeatStartIndex, repeat whole sequence */
    1                           /* altModCnt */
};

/*******************************************************************************
*
*   Private Module Handles
*
*******************************************************************************/

typedef struct BIRB_P_Handle
{
    uint32_t            magicId;                    /* Used to check if structure is corrupt */
    BCHP_Handle         hChip;
    BREG_Handle         hRegister;
    BINT_Handle         hInterrupt;
    BKNI_EventHandle    hEvent;
    BINT_CallbackHandle hCallback;
    SIrbConfiguration   *pConfig;
    SIrbConfiguration   CustomConfiguration;

    bool                longSequenceReloadPage0;
    bool                longSequenceReloadPage1;
    bool                intMode;
} BIRB_P_Handle;

/*******************************************************************************
*
*   Default Module Settings
*
*******************************************************************************/
static const BIRB_Settings defIrbSettings =
{
    BIRB_Device_eSony,          /* IRB device */
    true                        /* intMode */
};

/*******************************************************************************
*
*   Public Module Functions
*
*******************************************************************************/
BERR_Code BIRB_Open(
    BIRB_Handle *pIrb,                  /* [output] Returns handle */
    BCHP_Handle hChip,                  /* Chip handle */
    BREG_Handle hRegister,              /* Register handle */
    BINT_Handle hInterrupt,             /* Interrupt handle */
    const BIRB_Settings *pDefSettings   /* Default settings */
    )
{
    BERR_Code       retCode = BERR_SUCCESS;
    BIRB_Handle     hDev;
    uint32_t        lval;


    /* Sanity check on the handles we've been given. */
    BDBG_ASSERT( hChip );
    BDBG_ASSERT( hRegister );
    BDBG_ASSERT( hInterrupt );

    /* Alloc memory from the system heap */
    hDev = (BIRB_Handle) BKNI_Malloc( sizeof( BIRB_P_Handle ) );
    if( hDev == NULL )
    {
        *pIrb = NULL;
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BIRB_Open: BKNI_malloc() failed\n"));
        goto done;
    }

    hDev->magicId   = DEV_MAGIC_ID;
    hDev->hChip     = hChip;
    hDev->hRegister = hRegister;
    hDev->hInterrupt = hInterrupt;

    BIRB_CHK_RETCODE( retCode, BKNI_CreateEvent( &(hDev->hEvent) ) );

    BIRB_Reset (hDev);

    hDev->intMode   = pDefSettings->intMode;
    if (pDefSettings->intMode)
    {
        #if defined(BCHP_INT_ID_irb_irqen)
            BIRB_CHK_RETCODE( retCode, BINT_CreateCallback(
                &(hDev->hCallback), hDev->hInterrupt, BCHP_INT_ID_irb_irqen,
                BIRB_P_HandleInterrupt_Isr, (void *) hDev, 0x00 ) );
        #elif defined(BCHP_INT_ID_irb)
            BIRB_CHK_RETCODE( retCode, BINT_CreateCallback(
                &(hDev->hCallback), hDev->hInterrupt, BCHP_INT_ID_irb,
                BIRB_P_HandleInterrupt_Isr, (void *) hDev, 0x00 ) );
        #else
            #error unsupported
        #endif
        BIRB_CHK_RETCODE( retCode, BINT_EnableCallback( hDev->hCallback ) );

        BKNI_EnterCriticalSection();

        /*
         * Enable IRB interrupt in UPG
         */
        lval = BREG_Read32(hDev->hRegister, BCHP_IRQ0_IRQEN);
        #if defined(BCHP_IRQ0_IRQEN_irb_irqen_SHIFT)
            lval |= BCHP_FIELD_DATA(IRQ0_IRQEN, irb_irqen, 1);
        #elif defined(BCHP_IRQ0_IRQEN_irb_SHIFT)
            lval |= BCHP_FIELD_DATA(IRQ0_IRQEN, irb, 1);
        #else
            #error unsupported
        #endif
        BREG_Write32( hDev->hRegister, BCHP_IRQ0_IRQEN, lval );

        /*
         * Enable IRB interrupt in IRB
         */
        BIRB_P_EnableInt (hDev);
        BKNI_LeaveCriticalSection();
    }
    else
        BIRB_P_DisableInt (hDev);

    BIRB_Config (hDev, pDefSettings->irbDev);

    *pIrb = hDev;

done:
    if ((retCode != BERR_SUCCESS) && hDev)
    {
        BKNI_Free( (void *) hDev );
        *pIrb = NULL;
    }
    return( retCode );
}

BERR_Code BIRB_Close(
    BIRB_Handle hDev                    /* Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    BKNI_EnterCriticalSection();
    BIRB_P_DisableInt (hDev);
    BKNI_LeaveCriticalSection();

    BIRB_CHK_RETCODE( retCode, BINT_DisableCallback( hDev->hCallback ) );
    BIRB_CHK_RETCODE( retCode, BINT_DestroyCallback( hDev->hCallback ) );
    BKNI_DestroyEvent( hDev->hEvent );

    BKNI_Free( (void *) hDev );

done:
    return( retCode );
}

BERR_Code BIRB_GetDefaultSettings(
    BIRB_Settings *pDefSettings,        /* [output] Returns default setting */
    BCHP_Handle hChip                   /* Chip handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BSTD_UNUSED(hChip);

    *pDefSettings = defIrbSettings;

    return( retCode );
}

BERR_Code BIRB_GetEventHandle(
    BIRB_Handle     hDev,               /* Device handle */
    BKNI_EventHandle *phEvent           /* [output] Returns event handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    *phEvent = hDev->hEvent;

    return( retCode );
}

void BIRB_ConfigCustom (
    BIRB_Handle         hDev,           /* Device handle */
    SIrbConfiguration   *pConfig        /* Pointer to custom config */
)
{
    BSTD_UNUSED(hDev);
    BKNI_Memcpy ((void *)&(hDev->CustomConfiguration), (void *)pConfig, sizeof(SIrbConfiguration));
}


BERR_Code BIRB_Config (
    BIRB_Handle         hDev,           /* Device handle */
    BIRB_Device         irbDev          /* IR device type */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    switch (irbDev)
    {
        case BIRB_Device_eSony:
            hDev->pConfig = (SIrbConfiguration *)&SonyConfiguration;
            break;
        case BIRB_Device_eGI:
            hDev->pConfig = (SIrbConfiguration *)&GIConfiguration;
            break;
        case BIRB_Device_eCustom:
            hDev->pConfig = &(hDev->CustomConfiguration);
            break;
        case BIRB_Device_eTest:
            hDev->pConfig = (SIrbConfiguration *)&Test64bitConfiguration;
            break;
        case BIRB_Device_ePioneer:
            hDev->pConfig = (SIrbConfiguration *)&PioneerConfiguration;
            break;
        case BIRB_Device_ePioneerAAAA:
            hDev->pConfig = (SIrbConfiguration *)&PioneerAAAAConfiguration;
            break;
        case BIRB_Device_eXmp2:
            hDev->pConfig = (SIrbConfiguration *)&Xmp2StationConfiuration;
            break;
        default:
            retCode = BERR_INVALID_PARAMETER;
    }

    return( retCode );
}

void BIRB_Reset(
    BIRB_Handle     hDev
)
{
    uint32_t    lval;


    lval = BREG_Read32(hDev->hRegister, BCHP_IRB_BLAST_CONTROL);
    lval |= BCHP_IRB_BLAST_CONTROL_blastRST_MASK;           /* assert reset */
    BREG_Write32(hDev->hRegister, BCHP_IRB_BLAST_CONTROL, lval);
    lval &= ~BCHP_IRB_BLAST_CONTROL_blastRST_MASK;          /* de-assert reset */
    BREG_Write32(hDev->hRegister, BCHP_IRB_BLAST_CONTROL, lval);

}

BERR_Code BIRB_Blast (
    BIRB_Handle         hDev            /* Device handle */
)
{
    BERR_Code   retCode = BERR_SUCCESS;
    uint32_t    lval;

    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    lval = BREG_Read32(hDev->hRegister, BCHP_IRB_BLAST_CONTROL);
    lval |= BCHP_IRB_BLAST_CONTROL_blast_MASK;          /* assert reset */
    BREG_Write32(hDev->hRegister, BCHP_IRB_BLAST_CONTROL, lval);

    return( retCode );
}

bool BIRB_IsIrbFinished(
    BIRB_Handle     hDev
)
{
    uint32_t    lval;


    lval = BREG_Read32(hDev->hRegister, BCHP_IRB_BLAST_CONTROL);

    return (lval & BCHP_IRB_BLAST_CONTROL_blast_MASK) ? false : true;
}

BERR_Code BIRB_Send (
    BIRB_Handle         hDev,           /* Device handle */
    uint32_t            *pData,         /* pointer to data to blast */
    uint8_t             bits            /* number of bits to blast */
)
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    if (bits > MAX_IRB_SEQUENCES)
    {
        retCode = BIRB_ERR_TOO_MANY_SEQ;
        goto done;
    }

    BIRB_Reset (hDev);

    BIRB_P_ConfigDataSequence (hDev, pData, bits, true);

    BIRB_ConfigRegisters (hDev);

    BIRB_CHK_RETCODE( retCode, BIRB_Blast (hDev) );

done:
    return( retCode );
}

BERR_Code BIRB_SendWithHeaderOption (
    BIRB_Handle         hDev,           /* Device handle */
    uint32_t            *pData,         /* pointer to data to blast */
    uint8_t             bits,           /* number of bits to blast */
    bool                headerPulse     /* header flag */
)
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    if (bits > MAX_IRB_SEQUENCES)
    {
        retCode = BIRB_ERR_TOO_MANY_SEQ;
        goto done;
    }

    BIRB_Reset(hDev);

    BIRB_P_ConfigDataSequence(hDev, pData, bits, headerPulse);

    BIRB_ConfigRegisters(hDev);

    BIRB_CHK_RETCODE(retCode, BIRB_Blast(hDev));

done:
    return( retCode );
}

BERR_Code BIRB_SendABBB (
    BIRB_Handle         hDev,           /* Device handle */
    uint32_t            *pDataA,        /* pointer to data A to blast */
    uint8_t             bitsA,          /* number of bits in A to blast */
    uint32_t            *pDataB,        /* pointer to data B to blast */
    uint8_t             bitsB,          /* number of bits in B to blast */
    bool                headerA,        /* headerA flag */
    bool                headerB,        /* headerB flag */
    bool                fixedFlag       /* true: frame length fixed */
)
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    if ((bitsA + bitsB) > MAX_IRB_SEQUENCES)
    {
        retCode = BIRB_ERR_TOO_MANY_SEQ;
        goto done;
    }

    BIRB_Reset (hDev);

    BIRB_P_ConfigDataSequenceAB(hDev, pDataA, bitsA, pDataB, bitsB, headerA, headerB, fixedFlag);

    BIRB_ConfigRegisters (hDev);

    BIRB_CHK_RETCODE( retCode, BIRB_Blast (hDev) );

done:
    return( retCode );
}

BERR_Code BIRB_SendAAAA (
    BIRB_Handle         hDev,           /* Device handle */
    uint32_t            *pDataA,        /* pointer to data A to blast */
    uint8_t             bitsA,          /* number of bits in A to blast */
    bool                headerPulse,    /* header flag */
    bool                fixedFlag       /* true: frame length fixed */
)
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    if (bitsA > MAX_IRB_SEQUENCES)
    {
        retCode = BIRB_ERR_TOO_MANY_SEQ;
        goto done;
    }

    BIRB_Reset (hDev);

    BIRB_P_ConfigDataSequenceAA(hDev, pDataA, bitsA, headerPulse, fixedFlag);

    BIRB_ConfigRegisters (hDev);

    BIRB_CHK_RETCODE( retCode, BIRB_Blast (hDev) );

done:
    return( retCode );
}


BERR_Code BIRB_SendXmp2Ack (
    BIRB_Handle         hDev           /* Device handle */
)
{
    uint8_t     ackData;
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    /* build ack data
     * For now, owener code is 0.
     * Data structure is based on page 8 of Packet Layer Spec version 1.3,
     * which is slightly different from page 18 of XMP-2 Remore Demo v1.2.doc
     * ------------------------
     * |C1|C0|0|ACK|N3|N2|N1|N0|
     * ------------------------
     * C1-C0 10 = dual device mode
     * ACK = 1 NAK= 0
     * N3-N0 0000 = accepting all peripherals.
     */
    ackData = 0x90;

    BIRB_Reset (hDev);

    BIRB_P_ConfigDataSequenceXmp2(hDev, &ackData, 1);

    BIRB_ConfigRegisters (hDev);

    BIRB_CHK_RETCODE( retCode, BIRB_Blast (hDev) );

done:
    return( retCode );
}


BERR_Code BIRB_SendXmp2Nack (
    BIRB_Handle         hDev           /* Device handle */
)
{
    uint8_t     ackData;
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    /* build ack data
     * For now, owener code is 0.
     * Data structure is based on page 8 of Packet Layer Spec version 1.3,
     * which is slightly different from page 18 of XMP-2 Remore Demo v1.2.doc
     * ------------------------
     * |C1|C0|0|ACK|N3|N2|N1|N0|
     * ------------------------
     * C1-C0 10 = dual device mode
     * ACK = 1 NAK= 0
     * N3-N0 0000 = accepting all peripherals.
     */
    ackData = 0x80;

    BIRB_Reset (hDev);

    BIRB_P_ConfigDataSequenceXmp2(hDev, &ackData, 1);

    BIRB_ConfigRegisters (hDev);

    BIRB_CHK_RETCODE( retCode, BIRB_Blast (hDev) );

done:
    return( retCode );
}


/****************************************************************
 * This function is used to send generic one or four byte data. *
 ****************************************************************/
BERR_Code BIRB_SendXmp2Bytes (
    BIRB_Handle         hDev,           /* Device handle */
    uint8_t             *pByte,         /* date to send */
    uint8_t             numByte         /* 1 or 4 bye */
)
{
    uint8_t     ackData;
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
    BSTD_UNUSED(ackData);

    if (numByte != 1 && numByte != 4)
    {
        BDBG_ERR(("%s/%d : unsupported bytes", __FILE__, __LINE__)); \
        return BERR_INVALID_PARAMETER;
    }
    BIRB_Reset (hDev);

    BIRB_P_ConfigDataSequenceXmp2(hDev, pByte, numByte);

    BIRB_ConfigRegisters (hDev);

    BIRB_CHK_RETCODE( retCode, BIRB_Blast (hDev) );

done:
    return( retCode );
}


void BIRB_ConfigRegisters(
    BIRB_Handle     hDev
)
{
    uint32_t    i, lval, regIndex, addr, duration;
    SIrbConfiguration *pIrbConfig = hDev->pConfig;


    /* Clear interrupt status */
    lval = BREG_Read32(hDev->hRegister, BCHP_IRB_BLAST_INTRSTATUS);
    lval |= (BCHP_IRB_BLAST_INTRSTATUS_seqPage0Done_MASK |
             BCHP_IRB_BLAST_INTRSTATUS_seqPage1Done_MASK |
             BCHP_IRB_BLAST_INTRSTATUS_blastDone_MASK );
    BREG_Write32(hDev->hRegister, BCHP_IRB_BLAST_INTRSTATUS, lval);

    /*
     * Configure CONTROL register
     */
    lval = BREG_Read32(hDev->hRegister, BCHP_IRB_BLAST_CONTROL);
    lval |= (BCHP_IRB_BLAST_CONTROL_intensify_MASK | (pIrbConfig->noCarrier ? BCHP_IRB_BLAST_CONTROL_carrInh_MASK : 0));
    lval &= ~BCHP_IRB_BLAST_CONTROL_seqPage_MASK;               /* starting loading page 0 */
    BREG_Write32(hDev->hRegister, BCHP_IRB_BLAST_CONTROL, lval);

    /*
     * Configure prescaler registers
     */
    lval = pIrbConfig->masterClockDivisor - 1;
    BREG_Write32(hDev->hRegister, BCHP_IRB_BLAST_PRIMPRE, lval);

    lval  = (pIrbConfig->carrierClockDivisor - 1) << 4;
    lval |= (pIrbConfig->indexClockDivisor - 1);
    BREG_Write32(hDev->hRegister, BCHP_IRB_BLAST_INDXPRE, lval);

    /*
     * Configure repeat registers
     */
    lval = pIrbConfig->repeatCount;
    BREG_Write32(hDev->hRegister, BCHP_IRB_BLAST_REPNUM, lval);

    lval = pIrbConfig->repeatStartIndex;
    BREG_Write32(hDev->hRegister, BCHP_IRB_BLAST_REPINDX, lval);
    /*
     * Configure carrier width registers
     */
    lval = pIrbConfig->carrierHighCount - 1;
/*  if (lval & ~0xf)
        BRCM_DBG_WRN(("irblastr carrierHighCount > 16"));
*/
    BREG_Write32(hDev->hRegister, BCHP_IRB_BLAST_CARRHI, lval);

    lval = pIrbConfig->carrierLowCount - 1;
/*  if (bval & ~0xf)
        BRCM_DBG_WRN(("irblastr carrierLowCount > 16"));
*/
    BREG_Write32(hDev->hRegister, BCHP_IRB_BLAST_CARRLO, lval);

    /*
     * Configure Number of Sequence register
     */
    if (pIrbConfig->numberSequences > (2 * IRB_PAGE_SIZE))
    {
        lval = pIrbConfig->numberSequences - (2 * IRB_PAGE_SIZE) - 1;
        lval |= NUMSEQ_GREATER_THAN_80_BIT;
        BREG_Write32(hDev->hRegister, BCHP_IRB_BLAST_NUMSEQ, lval);

        /*
         * Must set longSeqMode bit
         */
        lval = BREG_Read32(hDev->hRegister, BCHP_IRB_BLAST_INTRENABLE);
        lval |= (BCHP_IRB_BLAST_INTRENABLE_longSeqMode_MASK |
                BCHP_IRB_BLAST_INTRENABLE_seqPage0Done_MASK);

        hDev->longSequenceReloadPage0 = true;

        if (pIrbConfig->numberSequences > (3 * IRB_PAGE_SIZE))
        {
            lval |= BCHP_IRB_BLAST_INTRENABLE_seqPage1Done_MASK;
            hDev->longSequenceReloadPage1 = true;
        }

        BREG_Write32(hDev->hRegister, BCHP_IRB_BLAST_INTRENABLE, lval);

    }
    else
    {
        lval = pIrbConfig->numberSequences - 1;
        BREG_Write32(hDev->hRegister, BCHP_IRB_BLAST_NUMSEQ, lval);
        hDev->longSequenceReloadPage0 = hDev->longSequenceReloadPage1 = false;
    }

    /*
     * Configure Sequence Index registers.
     */
    for (i = 0, regIndex = 0; i < pIrbConfig->numberSequences; i++, regIndex++)
    {
        /*
         * Check to see if we've loaded both pages already
         */
        if (i == (2* IRB_PAGE_SIZE))
            break;

        /*
         * Check to see if we have to switch page
         */
        if (i == IRB_PAGE_SIZE)
        {
            lval = BREG_Read32(hDev->hRegister, BCHP_IRB_BLAST_CONTROL);
            lval |= BCHP_IRB_BLAST_CONTROL_seqPage_MASK;
            BREG_Write32(hDev->hRegister, BCHP_IRB_BLAST_CONTROL, lval);

            regIndex = 0;
        }
        lval = (pIrbConfig->sequenceIndex[i].on << 4) | (pIrbConfig->sequenceIndex[i].off);
        BREG_Write32(hDev->hRegister, (BCHP_IRB_BLAST_SEQ_REGFILE00 + (regIndex * 4)), lval);
    }

    lval = BREG_Read32(hDev->hRegister, BCHP_IRB_BLAST_CONTROL);
    lval &= ~BCHP_IRB_BLAST_CONTROL_seqPage_MASK;
    BREG_Write32(hDev->hRegister, BCHP_IRB_BLAST_CONTROL, lval);

    addr = BCHP_IRB_BLAST_MOD_REGFILE00;    /* address of duration memory */
    for (i = 0; i < irb_durationMemLength; i++)
    {
        if (pIrbConfig->altModCnt == 1)
        {
            /* When altModeCnt is one, actual 0 pulse is used.
             * Do not subtract 1
             */
            duration = pIrbConfig->duration[i];
        }
        else
        {
            duration = pIrbConfig->duration[i] - 1;
        }
        /* The last 4 entries are "double-time", so we need to divide them by 2 */
        if (i >= 10)
            duration /= 2;

        lval = duration >> 8;
        BREG_Write32(hDev->hRegister, addr, lval);
        addr += 4;

        lval = duration & 0xff;
        BREG_Write32(hDev->hRegister, addr, lval);
        addr += 4;
    }

    /* Configure altModCnt bit in INTRSTATUS register. */
    if (pIrbConfig->altModCnt == 1)
    {
        BREG_Write32(hDev->hRegister, BCHP_IRB_BLAST_INTRENABLE, BLAST_INTRENABLE_ALTMODCNT);
    }
    else
    {
        BREG_Write32(hDev->hRegister, BCHP_IRB_BLAST_INTRDISABLE, BLAST_INTRENABLE_ALTMODCNT);
    }
}

/*******************************************************************************
*
*   Private Module Functions
*
*******************************************************************************/
void BIRB_P_EnableInt(
    BIRB_Handle     hDev
)
{
    uint32_t    lval;


    lval = BREG_Read32(hDev->hRegister, BCHP_IRB_BLAST_CONTROL);
    lval |= BCHP_IRB_BLAST_CONTROL_irqInh_MASK;
    BREG_Write32(hDev->hRegister, BCHP_IRB_BLAST_CONTROL, lval);

    lval = BREG_Read32(hDev->hRegister, BCHP_IRB_BLAST_INTRENABLE);
    lval |= (BCHP_IRB_BLAST_INTRENABLE_masterIntrEn_MASK | BCHP_IRB_BLAST_INTRENABLE_blastDone_MASK);
    BREG_Write32(hDev->hRegister, BCHP_IRB_BLAST_INTRENABLE, lval);

}

void BIRB_P_DisableInt(
    BIRB_Handle     hDev
)
{
    uint32_t    lval;


    lval = BREG_Read32(hDev->hRegister, BCHP_IRB_BLAST_CONTROL);
    lval |= BCHP_IRB_BLAST_CONTROL_irqInh_MASK;
    BREG_Write32(hDev->hRegister, BCHP_IRB_BLAST_CONTROL, lval);

    lval = BREG_Read32(hDev->hRegister, BCHP_IRB_BLAST_INTRENABLE);
    lval &= ~(BCHP_IRB_BLAST_INTRENABLE_masterIntrEn_MASK | BCHP_IRB_BLAST_INTRENABLE_blastDone_MASK);
    BREG_Write32(hDev->hRegister, BCHP_IRB_BLAST_INTRENABLE, lval);

}

void BIRB_P_ConfigDataSequence(
    BIRB_Handle     hDev,
    uint32_t        *pData,         /* pointer to data to blast */
    uint8_t         bits,           /* number of bits to blast */
    bool            headerPulse     /* header flag */
)
{
    uint8_t     i, lastSeqIndex, noHeader;
    uint32_t    code, frameLength = 0, lastOffPeriod;
    SIrbConfiguration *pIrbConfig = hDev->pConfig;


    if (headerPulse)
    {
        noHeader = 0;
        pIrbConfig->numberSequences = bits + 1;
    }
    else
    {
        noHeader = 1;
    }

    /*
     * Start bit sequence index already at sequenceIndex[0] if there's a header.
     * In this case, fill in indices for data bits beginning at sequenceIndex[1].
     */
    lastSeqIndex = bits;
    code = *pData++;
    for (i = 1 - noHeader; i <= lastSeqIndex - noHeader; i++)
    {
        pIrbConfig->sequenceIndex[i] = (code & 1) ? pIrbConfig->logic1IndexPair
                                                  : pIrbConfig->logic0IndexPair;
        code >>= 1;
        if (((i + noHeader) % 32) == 0)
        {
            code = *pData++;
        }
    }

    /*
     * Extend OFF period of last pulse to satisfy frame duration requirment.
     */
    for (i = 0; i <= (lastSeqIndex - noHeader); i++)
    {
        irb_IndexPair indexPair = pIrbConfig->sequenceIndex[i];
        frameLength +=  pIrbConfig->duration[indexPair.on] +
                        pIrbConfig->duration[indexPair.off];
    }
    lastOffPeriod = pIrbConfig->framePeriod - frameLength +
                    pIrbConfig->duration[pIrbConfig->sequenceIndex[lastSeqIndex].off];
    pIrbConfig->duration[pIrbConfig->lastSequenceOffIndex] = lastOffPeriod;
    pIrbConfig->sequenceIndex[lastSeqIndex].off = pIrbConfig->lastSequenceOffIndex;

}


void BIRB_P_ConfigDataSequenceAA(
    BIRB_Handle     hDev,
    uint32_t        *pData,         /* pointer to data to blast */
    uint8_t         bits,           /* number of bits to blast */
    bool            headerPulse,    /* header flag */
    bool            fixedFlag       /* fixed frame flag */
)
{
    uint8_t     i;
    uint8_t     noHeader;
    uint32_t    code, frameLength = 0, lastOffPeriod;
    SIrbConfiguration *pIrbConfig = hDev->pConfig;

    /*
     * Start bit sequence index already at sequenceIndex[0].
     * Fill in indices for data bits beginning at sequenceIndex[1].
     */

    if (headerPulse) {
        noHeader = 0;
    } else {
        noHeader = 1;
    }

    code = *pData++;
    for (i = 1 - noHeader; i <= bits - noHeader; i++)
    {
        pIrbConfig->sequenceIndex[i] = (code & 1) ? pIrbConfig->logic1IndexPair
                                                  : pIrbConfig->logic0IndexPair;
        code >>= 1;
        if (((i + noHeader) % 32) == 0)
        {
            code = *pData++;
        }
    }

    if (fixedFlag) {
        for (i = 0; i <= (pIrbConfig->numberSequences - 2); i++)
        {
            irb_IndexPair indexPair = pIrbConfig->sequenceIndex[i];
            frameLength +=  pIrbConfig->duration[indexPair.on] +
                            pIrbConfig->duration[indexPair.off];
        }

        lastOffPeriod = pIrbConfig->framePeriod - (frameLength +
                        pIrbConfig->duration[pIrbConfig->sequenceIndex[pIrbConfig->numberSequences - 1].on]);
        pIrbConfig->duration[pIrbConfig->lastSequenceOffIndex] = lastOffPeriod;
        /* pIrbConfig->sequenceIndex[pIrbConfig->numberSequences - 1].off = pIrbConfig->lastSequenceOffIndex; */
    }
}



void BIRB_P_ConfigDataSequenceAB(
    BIRB_Handle     hDev,
    uint32_t        *pDataA,        /* pointer to data A to blast */
    uint8_t         bitsA,          /* number of bits A to blast */
    uint32_t        *pDataB,        /* pointer to data B to blast */
    uint8_t         bitsB,          /* number of bits B to blast */
    bool            headerA,        /* header A flag */
    bool            headerB,        /* header B flag */
    bool            fixedFlag       /* fixed frame length */
)
{
    uint8_t     i, startSeqIndexB, noHeader;
    uint32_t    code, frameLength = 0, lastOffPeriod;
    SIrbConfiguration *pIrbConfig = hDev->pConfig;

    /****************************************/
    /* Data A                               */
    /****************************************/

    if (headerA) {
        noHeader = 0;
    } else {
        noHeader = 1;
    }

    code = *pDataA++;
    for (i = 1 - noHeader; i <= bitsA - noHeader; i++)
    {
        pIrbConfig->sequenceIndex[i] = (code & 1) ? pIrbConfig->logic1IndexPair
                                                  : pIrbConfig->logic0IndexPair;
        code >>= 1;
        if (((i + noHeader) % 32) == 0)
        {
            code = *pDataA++;
        }
    }

    if (fixedFlag)
    {
        for (i = 0; i <= (bitsA - noHeader); i++)
        {
            irb_IndexPair indexPair = pIrbConfig->sequenceIndex[i];
            frameLength +=  pIrbConfig->duration[indexPair.on] +
                            pIrbConfig->duration[indexPair.off];
        }

        lastOffPeriod = pIrbConfig->framePeriod - (frameLength +
                        pIrbConfig->duration[pIrbConfig->sequenceIndex[bitsA - noHeader + 1].on]);
        pIrbConfig->duration[pIrbConfig->sequenceIndex[bitsA - noHeader + 1].off] = lastOffPeriod;
    }

    /****************************************/
    /* Data B                               */
    /****************************************/
    /*
     * Start bit sequence index already at sequenceIndex[0].
     * Fill in indices for data bits beginning at sequenceIndex[1].
     */
    startSeqIndexB = bitsA + 2 - noHeader;

    if (headerB) {
        noHeader = 0;
    } else {
        noHeader = 1;
    }

    if (bitsB != 0) {
        code = *pDataB++;
        for (i = (startSeqIndexB + 1 - noHeader); i <= (startSeqIndexB + bitsB - noHeader); i++)
        {
            pIrbConfig->sequenceIndex[i] = (code & 1) ? pIrbConfig->logic1IndexPair
                                                      : pIrbConfig->logic0IndexPair;
            code >>= 1;
            if (((i - startSeqIndexB + noHeader) % 32) == 0)
            {
                code = *pDataB++;
            }
        }
    }

    if (fixedFlag)
    {
        /*
         * Calculate time for data and header
         */
        frameLength = 0;
        for (i = startSeqIndexB; i <= (pIrbConfig->numberSequences - 2); i++)
        {
            irb_IndexPair indexPair = pIrbConfig->sequenceIndex[i];
            frameLength +=  pIrbConfig->duration[indexPair.on] +
                            pIrbConfig->duration[indexPair.off];
        }

        /* Now calculate the remaining off time for end pulse */
        lastOffPeriod = pIrbConfig->framePeriodB - (frameLength +
                        pIrbConfig->duration[pIrbConfig->sequenceIndex[pIrbConfig->numberSequences - 1].on]);

        pIrbConfig->duration[pIrbConfig->sequenceIndex[pIrbConfig->numberSequences - 1].off] = lastOffPeriod;
    }
}

/*****************************************************************************/
/* Set Data Sequence (sequenceIndex) in configuration for XMP-2 Station 38K  */
/*****************************************************************************/
void BIRB_P_ConfigDataSequenceXmp2(
    BIRB_Handle     hDev,
    uint8_t         *pData,             /* data */
    uint8_t         numByte             /* 1 or 4 */
    )
{
    SIrbConfiguration *pIrbConfig= hDev->pConfig;
    uint8_t i;
    uint8_t nibble0, nibble1;

    if (numByte == 4)
    {
        pIrbConfig->numberSequences = 17;
        /* sequenceIndex[4] could be set to trailer pulse.
         * set it back to data pulse
         */
        pIrbConfig->sequenceIndex[4].on = DATA_PULSE;
    }
    else
    if (numByte == 1)
    {
        pIrbConfig->numberSequences = 5;
        /* set the trailer pulse */
        pIrbConfig->sequenceIndex[4].on = TRAILER_PULSE;
        pIrbConfig->sequenceIndex[4].off = ZERO;
    }
    else
    {
        BDBG_ERR(("%s/%d : unsupported bytes", __FILE__, __LINE__)); \
        return;
    }

    for (i=0; i<numByte; i++)
    {
        nibble0 = (pData[i] >> 4) & 0x0F;   /* upper nibble */
        nibble1 = pData[i] & 0x0F;          /* lower nibble */
        /* Data High already at sequenceIndex[i].on and
         * Zero already at sequenceIndex[i+1].on,
         * need only determine sequenceIndex[i].off and
         * sequenceIndex[i+1].off from nibble.
         */
        pIrbConfig->sequenceIndex[i*4].off   = DATA0 + (nibble0>>1);
        pIrbConfig->sequenceIndex[i*4+1].off = (nibble0 & 0x01) ? DELTA1 : ZERO;
        pIrbConfig->sequenceIndex[i*4+2].off = DATA0 + (nibble1>>1);
        pIrbConfig->sequenceIndex[i*4+3].off = (nibble1 & 0x01) ? DELTA1 : ZERO;
    }
}


static void BIRB_P_HandleInterrupt_Isr
(
    void *pParam1,                      /* Device channel handle */
    int parm2                           /* not used */
)
{
    BIRB_Handle         hDev;
    uint32_t            lval, i, regIndex;
    SIrbConfiguration   *pIrbConfig;
    BSTD_UNUSED(parm2);

    hDev = (BIRB_Handle) pParam1;
    BDBG_ASSERT( hDev );

    pIrbConfig = hDev->pConfig;

    lval = BREG_Read32(hDev->hRegister, BCHP_IRB_BLAST_INTRSTATUS);
    lval &= BREG_Read32(hDev->hRegister, BCHP_IRB_BLAST_INTRENABLE);

    if (lval & BCHP_IRB_BLAST_INTRSTATUS_blastDone_MASK)
    {
        /* clear interrupt */
        BREG_Write32(hDev->hRegister, BCHP_IRB_BLAST_INTRSTATUS, BCHP_IRB_BLAST_INTRSTATUS_blastDone_MASK);
        BKNI_SetEvent( hDev->hEvent );
        return;
    }

    if (lval & BCHP_IRB_BLAST_INTRSTATUS_seqPage0Done_MASK)
    {
        /* clear interrupt */
        BREG_Write32(hDev->hRegister, BCHP_IRB_BLAST_INTRDISABLE, BCHP_IRB_BLAST_INTRSTATUS_seqPage0Done_MASK);
        BREG_Write32(hDev->hRegister, BCHP_IRB_BLAST_INTRSTATUS, BCHP_IRB_BLAST_INTRSTATUS_seqPage0Done_MASK);

        /*
         * Load sequence 81-120
         */
        if (hDev->longSequenceReloadPage0)
        {
            lval = BREG_Read32(hDev->hRegister, BCHP_IRB_BLAST_CONTROL);
            lval &= ~BCHP_IRB_BLAST_CONTROL_seqPage_MASK;               /* choose page 0 */
            BREG_Write32(hDev->hRegister, BCHP_IRB_BLAST_CONTROL, lval);

            for (i = 80, regIndex = 0; i < pIrbConfig->numberSequences; i++, regIndex++)
            {
                lval = (pIrbConfig->sequenceIndex[i].on << 4) | (pIrbConfig->sequenceIndex[i].off);
                BREG_Write32(hDev->hRegister, (BCHP_IRB_BLAST_SEQ_REGFILE00 + (regIndex * 4)), lval);
                if (regIndex == 39)                                     /* we've filled up this page */
                    break;
            }
            hDev->longSequenceReloadPage0 = false;
        }
        return;
    }

    if (lval & BCHP_IRB_BLAST_INTRSTATUS_seqPage1Done_MASK)
    {
        /* clear interrupt */
        BREG_Write32(hDev->hRegister, BCHP_IRB_BLAST_INTRDISABLE, BCHP_IRB_BLAST_INTRSTATUS_seqPage1Done_MASK);
        BREG_Write32(hDev->hRegister, BCHP_IRB_BLAST_INTRSTATUS, BCHP_IRB_BLAST_INTRSTATUS_seqPage1Done_MASK);

        /*
         * Load sequence 121-160
         */
        if (hDev->longSequenceReloadPage1)
        {
            lval = BREG_Read32(hDev->hRegister, BCHP_IRB_BLAST_CONTROL);
            lval |= BCHP_IRB_BLAST_CONTROL_seqPage_MASK;                /* choose page 1 */
            BREG_Write32(hDev->hRegister, BCHP_IRB_BLAST_CONTROL, lval);

            for (i = 120, regIndex = 0; i < pIrbConfig->numberSequences; i++, regIndex++)
            {
                lval = (pIrbConfig->sequenceIndex[i].on << 4) | (pIrbConfig->sequenceIndex[i].off);
                BREG_Write32(hDev->hRegister, (BCHP_IRB_BLAST_SEQ_REGFILE00 + (regIndex * 4)), lval);
            }
            hDev->longSequenceReloadPage1 = false;
        }
    }
}

