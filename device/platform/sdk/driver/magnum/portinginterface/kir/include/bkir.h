/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bkir.h $
 * $brcm_Revision: Hydra_Software_Devel/64 $
 * $brcm_Date: 4/29/13 6:36p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/kir/include/bkir.h $
 * 
 * Hydra_Software_Devel/64   4/29/13 6:36p agin
 * SW7231-1184:  Unexpected result of NEXUS_IrInput_EnableDataFilter
 * function use.
 * 
 * Hydra_Software_Devel/63   10/8/12 12:41a agin
 * SWNOOS-576:  Fixed compile error for 7550.
 * 
 * Hydra_Software_Devel/62   9/21/12 6:49p agin
 * SW7445-29:  add kir PI support for 7445.
 * 
 * Hydra_Software_Devel/61   8/10/12 10:05a gmohile
 * SW7435-292 : enable 7435 Data Filter
 * 
 * Hydra_Software_Devel/60   7/26/12 7:41p xhuang
 * SW7552-299: Correct IR number for 7552
 * 
 * Hydra_Software_Devel/59   4/28/12 3:28p chengs
 * SW7584-23: Add 7584 support.
 * 
 * Hydra_Software_Devel/58   4/26/12 4:56p bselva
 * SW7360-17: Add support for 7360
 * 
 * Hydra_Software_Devel/57   2/21/12 6:34p jkim
 * SW7425-2283: Add API to enable/disable filter1
 * 
 * Hydra_Software_Devel/56   11/3/11 5:54p mward
 * SW7435-7:  Add 7435.
 * 
 * Hydra_Software_Devel/55   10/27/11 2:06p xhuang
 * SW7552-9: Add 7552 power management support
 * 
 * Hydra_Software_Devel/54   10/20/11 3:51p bselva
 * SW7358-169: Added the changes required for power managment support
 * 
 * Hydra_Software_Devel/53   9/20/11 2:11p jkim
 * SW7429-10: Add 7429 support
 * 
 * Hydra_Software_Devel/52   9/20/11 1:37p jkim
 * SWBLURAY-27433: Add Toshiba IR support
 * 
 * Hydra_Software_Devel/52   9/20/11 1:35p jkim
 * SWBLURAY-27433: Add Toshiba IR support
 * 
 * Hydra_Software_Devel/51   6/22/11 6:10p randyjew
 * SW7344-14: Enable data filter for 7344/7346
 * 
 * Hydra_Software_Devel/50   6/9/11 7:02p nickh
 * SW7425-533: Enable data filter for 7425
 * 
 * Hydra_Software_Devel/49   6/6/11 5:14p etrudeau
 * SWBLURAY-24213: identify Quick chip 7630 B2 as having IR Filter support
 * 
 * Hydra_Software_Devel/48   5/13/11 5:15p gmohile
 * SW7231-128 : Enable data filter for 7231
 * 
 * Hydra_Software_Devel/47   4/12/11 5:47p mward
 * SW7125-905:  Add 7125 to BKIR_HAS_DATA_FILTER.
 * 
 * Hydra_Software_Devel/46   2/11/11 9:54a mward
 * SW7125-820:  Enable 2nd KIR on 7125 all revs.
 * 
 * Hydra_Software_Devel/45   2/11/11 9:21a gmohile
 * SW7408-210 : Enable data filter for 7420
 * 
 * Hydra_Software_Devel/44   2/4/11 5:10p mward
 * SW7125-820:  Enable 2nd KIR on 7125 c0.
 * 
 * Hydra_Software_Devel/43   1/26/11 2:26p etrudeau
 * SWBLURAY-23692: add 7640 support
 * 
 * Hydra_Software_Devel/42   1/20/11 5:36p gmohile
 * SW7408-1 : define BKIR_HAS_DATA_FILTER for 7408
 * 
 * Hydra_Software_Devel/41   12/27/10 2:19p xhuang
 * SW7358-29: Add 7358/7552 support
 * 
 * Hydra_Software_Devel/40   12/7/10 2:46p jrubio
 * SW7344-9: add 7346/7344 support
 * 
 * Hydra_Software_Devel/39   12/2/10 10:31a katrep
 * SW7231-4:add support for 7231
 * 
 * Hydra_Software_Devel/38   10/13/10 2:49p jkim
 * SW7630-98: add support for RCA remote
 * 
 * Hydra_Software_Devel/37   10/7/10 4:20p nickh
 * SW7422-74: Add 7422 support
 * 
 * Hydra_Software_Devel/36   9/14/10 5:46p hongtaoz
 * SW7425-9: adding 7425 support;
 * 
 * Hydra_Software_Devel/35   7/23/10 10:49a jkim
 * SW7550-471: define BKIR_HAS_DATA_FILTER for 7550
 * 
 * Hydra_Software_Devel/34   6/25/10 11:27a jkim
 * SW7550-471:Add support for pattern matching and 48 bit data
 * 
 * Hydra_Software_Devel/33   3/24/10 2:18p jkim
 * SW7405-4100: make BKIR_KirDevice_eNumKirDevice the highest value
 * 
 * Hydra_Software_Devel/32   12/13/09 6:41p rpereira
 * SW7550-41: Fixed compilation issues for 7550
 *
 * Hydra_Software_Devel/31   12/1/09 6:16p randyjew
 * SW7468-6: Add 7468 support
 *
 * Hydra_Software_Devel/30   11/23/09 6:33p mward
 * SW7125-97: Use BKIR_N_CHANNELS 1, no bchp_kbd2.h for 7125b0
 *
 * Hydra_Software_Devel/29   11/6/09 4:18p gmohile
 * SW7408-1 : Add 7408 support
 *
 * Hydra_Software_Devel/28   9/24/09 10:43a mward
 * SW7125-4: Changing BKIR_N_CHANNELS 2 for 7125 after RDB header update
 * removed both bchp_kbd3.h and bchp_kbd4.h.
 *
 * Hydra_Software_Devel/27   9/15/09 10:33a rpereira
 * SW7630-45: Adding support for 7630
 *
 * Hydra_Software_Devel/26   9/1/09 9:22p rpereira
 * SW7550-30: Adding 7550 support
 *
 * Hydra_Software_Devel/25   8/24/09 1:50p jkim
 * SW7405-340: Add RC6 Mode 0 support
 *
 * Hydra_Software_Devel/24   8/20/09 4:23p mward
 * PR55545: Support 7125.
 *
 * Hydra_Software_Devel/23   8/19/09 4:03p mward
 * PR55545: Support for 7125.
 *
 * Hydra_Software_Devel/22   8/10/09 4:09p jrubio
 * PR55232: add 7342/7340 support
 *
 * Hydra_Software_Devel/21   7/27/09 7:05p jkim
 * PR56525: add a function to set custom device type
 *
 * Hydra_Software_Devel/20   7/22/09 3:03p jkim
 * PR37963: Add functions to check preamble status
 *
 * Hydra_Software_Devel/19   7/14/09 6:09p jkim
 * PR56525: Add API to get default cir paramters so that it can be
 * modified and copied to cutom cir
 *
 * Hydra_Software_Devel/18   4/17/09 11:15a agin
 * PR54338: Moved BKIR_N_CHANNELS from bkir.c because other files need
 * this.
 *
 * Hydra_Software_Devel/17   2/9/09 8:24p jkim
 * PR37963: create device type for xmp-2 ack
 *
 * Hydra_Software_Devel/16   9/22/08 4:16p jkim
 * PR37963: add xmp-2 cir param
 *
 * Hydra_Software_Devel/15   7/28/08 1:43p jkim
 * PR45065: add R-step remote support
 *
 * Hydra_Software_Devel/14   4/1/08 4:19p brianlee
 * PR23894: Added functions to register user callback with KIR PI.
 *
 * Hydra_Software_Devel/13   4/9/07 4:51p jkim
 * PR28061: Add RCMM support
 *
 * Hydra_Software_Devel/12   11/28/06 6:03p bandrews
 * PR25664: Changed all c++ comments to C comments
 *
 * Hydra_Software_Devel/11   11/28/06 11:40a jkim
 * PR25664: Added RC6 remote support. Simultaneous support for two remote
 * should be working but can not be tested using broadcom reference
 * board.
 *
 * Hydra_Software_Devel/10   10/1/05 6:17p jkim
 * PR14344: Add UHFR legacy support
 *
 * Hydra_Software_Devel/9   4/4/05 2:40p agin
 * PR14558:  Add pointer to array for BKIR_Read function.
 *
 * Hydra_Software_Devel/8   2/28/05 2:27p brianlee
 * PR14284: Add support for UEI RF remote.
 *
 * Hydra_Software_Devel/7   3/4/04 5:36p brianlee
 * PR9958: Added BKIR_KirDevice_eCirCustom type and the function
 * BKIR_SetCustomCir() to allow user to set custom KIR settings.
 *
 * Hydra_Software_Devel/6   10/17/03 10:40a brianlee
 * Added a function to check for repeat key.
 *
 * Hydra_Software_Devel/5   10/2/03 10:40a brianlee
 * BKIR_Read should output interrupt device type, not IR device type.
 *
 * Hydra_Software_Devel/4   9/25/03 11:59a brianlee
 * Fixed tags for Midas service generator.
 *
 * Hydra_Software_Devel/3   9/15/03 10:28a brianlee
 * Added enums for port and interrupt device.
 *
 * Hydra_Software_Devel/2   7/31/03 1:58p brianlee
 * Removed blocking option from IsDataReady function.  Provide a GetEvent
 * function so caller can block on the event himself.
 *
 * Hydra_Software_Devel/1   7/24/03 4:24p brianlee
 * Initial version.
 *
 ***************************************************************************/


/*= Module Overview *********************************************************
<verbatim>

Overview
The KIR PI module controls the keyboard/IR receiver core within the BCM7038.
There are two separate keyboard/IR receivers within the 7038.  Each KIR core
can support 2 ports, i.e. 2 separate input pins.  There is built-in
hardware support for several IR devices, such as Sejin, TWIRP, GI Remote A and B.
In addition, there is a consumer IR decoder that is programmable by the user.

Design
The design for BKIR PI API is broken into two parts.
  Part 1 (open/close/configuration):
    These APIs are used for opening and closing BKIR device/device channel.
  Part 2 (enable device):
    These APIs are used to enable a KIR device type.
  Part 3 (get receiver data):
    These APIs are used to check for data received, read it, and return it
    to the caller.

Usage
The usage of BKIR involves the following:
   * Configure/Open of BKIR
      * Configure BKIR device for the target system
      * Open BKIR device
      * Configure BKIR device channel for the target system
      * Open BKIR device channel
   * Enable device
      * Enable the IR device type.
   * Check to see if data is received.
   * Get the data received.

Sample Code
void main( void )
{
    BKIR_Handle         hKir;
    BKIR_ChannelHandle  hKirChan;
    BKIR_ChannelSettings defChnSettings;
    BREG_Handle         hReg;
    BCHP_Handle         hChip;
    BINT_Handle         hInt;
    bool                readyFlag = 0;
    uint32_t            data;
    int                 chanNo;
    BKIR_KirInterruptDevice interruptDevice;

    // Do other initialization, i.e. for BREG, BCHP, etc

    // Make any changes required from the default values
    BKIR_Open (&hKir, hChip, hReg, hInt, (BKIR_Settings *)NULL);

    chanNo = 0; //example for channel 0
    BKIR_GetChannelDefaultSettings( hKir, chanNo, &defChnSettings );

    defChnSettings.irPort = 2;      // using port 2

    BKIR_OpenChannel( hKir, &hKirChan, chanNo, &defChnSettings );

    // Enable TWIRP device
    BKIR_EnableIrDevice (hKirChan, BKIR_KirDevice_eTwirpKbd);

    interruptDevice = BKIR_KirInterruptDevice_eNone;
    do
    {
        // Using polling
        BKIR_IsDataReady (hKirChan, &readyFlag);
        if (readyFlag)
            BKIR_Read (hKirChan, &interruptDevice, &data);
    } while (interruptDevice != BKIR_KirInterruptDevice_eTwirpKbd)
}

</verbatim>
***************************************************************************/


#ifndef BKIR_H__
#define BKIR_H__

#include "bchp.h"
#include "breg_mem.h"
#include "bint.h"
#include "bkni.h"
#include "berr_ids.h"
#include "bchp_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (BCHP_CHIP==7550)
    #define BKIR_N_CHANNELS 1 /* 7550 does not follow normal convention */
#else
    #if defined(BCHP_KBD3_REG_START)
        #define BKIR_N_CHANNELS 3
    #elif defined(BCHP_KBD2_REG_START)
        #define BKIR_N_CHANNELS 2
    #elif defined(BCHP_KBD1_REG_START)
        #define BKIR_N_CHANNELS 1
    #endif
#endif

/***************************************************************************
Summary:
    Error Codes specific to BKIR

Description:

See Also:

****************************************************************************/
#define BKIR_ERR_NOTAVAIL_CHN_NO            BERR_MAKE_CODE(BERR_KIR_ID, 0)

/***************************************************************************
Summary:
    The handles for kir module.

Description:
    Since BKIR is a device channel, it has main device handle as well
    as a device channel handle.

See Also:
    BKIR_Open(), BKIR_OpenChannel()

****************************************************************************/
typedef struct BKIR_P_Handle                *BKIR_Handle;
typedef struct BKIR_P_ChannelHandle         *BKIR_ChannelHandle;

/***************************************************************************
Summary:
    Enumeration for supported KIR devices

Description:
    This enumeration defines the devices that the KIR PI supports.

See Also:
    None.

****************************************************************************/
typedef enum
{
    BKIR_KirDevice_eTwirpKbd = 1,       /* TWIRP */
    BKIR_KirDevice_eSejin38KhzKbd,      /* Sejin IR keyboard (38.4KHz) */
    BKIR_KirDevice_eSejin56KhzKbd,      /* Sejin IR keyboard (56.0KHz) */
    BKIR_KirDevice_eRemoteA,            /* remote A */
    BKIR_KirDevice_eRemoteB,            /* remote B */
    BKIR_KirDevice_eCirGI,              /* Consumer GI */
    BKIR_KirDevice_eCirSaE2050,         /* Consumer SA E2050 */
    BKIR_KirDevice_eCirTwirp,           /* Consumer Twirp */
    BKIR_KirDevice_eCirSony,            /* Consumer Sony */
    BKIR_KirDevice_eCirRecs80,          /* Consumer Rec580 */
    BKIR_KirDevice_eCirRc5,             /* Consumer Rc5 */
    BKIR_KirDevice_eCirUei,             /* Consumer UEI */
    BKIR_KirDevice_eCirRfUei,           /* Consumer RF UEI */
    BKIR_KirDevice_eCirEchoStar,        /* Consumer EchoRemote */
    BKIR_KirDevice_eSonySejin,          /* Sony Sejin keyboard using UART D */
    BKIR_KirDevice_eCirNec,             /* Consumer NEC */
    BKIR_KirDevice_eCirRC6,             /* Consumer RC6 */
    BKIR_KirDevice_eCirGISat,           /* Consumer GI Satellite */
    BKIR_KirDevice_eCirCustom,          /* Customer specific type */
    BKIR_KirDevice_eCirDirectvUhfr,     /* DIRECTV uhfr */
    BKIR_KirDevice_eCirEchostarUhfr,    /* Echostar uhfr */
    BKIR_KirDevice_eCirRcmmRcu,         /* RCMM Remote Control Unit */
    BKIR_KirDevice_eCirRstep,           /* R-step Remote Control Unit */
    BKIR_KirDevice_eCirXmp2,            /* XMP-2 4 byte data*/
    BKIR_KirDevice_eCirXmp2Ack,         /* XMP-2 Ack/Nak*/
    BKIR_KirDevice_eCirRC6Mode0,        /* Consumer RC6 Mode 0 */
    BKIR_KirDevice_eCirRca,             /* Consumer RCA */
    BKIR_KirDevice_eCirToshibaTC9012,   /* Consumer Toshiba */
    BKIR_KirDevice_eNumKirDevice        /* number of keyboard devices */
} BKIR_KirDevice;

/***************************************************************************
Summary:
    Enumeration for IR device type that generated the interrupt

Description:
    This enumeration defines the device type that generated the interrupt

See Also:
    None.

****************************************************************************/
typedef enum
{
    BKIR_KirInterruptDevice_eNone = 0,
    BKIR_KirInterruptDevice_eTwirpKbd,      /* TWIRP */
    BKIR_KirInterruptDevice_eSejinKbd,      /* SEJIN */
    BKIR_KirInterruptDevice_eRemoteA,
    BKIR_KirInterruptDevice_eRemoteB,
    BKIR_KirInterruptDevice_eCir

} BKIR_KirInterruptDevice;

/***************************************************************************
Summary:
    Enumeration for IR port selection

Description:
    This enumeration defines the port selection for each IR channel.

See Also:
    None.

****************************************************************************/
typedef enum
{
    BKIR_KirPort1,
    BKIR_KirPort2,
    BKIR_KirPortAuto
} BKIR_KirPort;

/***************************************************************************
Summary:
    Typedef for CIR parameters

Description:
    This structure typedef is designed for CIR parameters

See Also:
    None.

****************************************************************************/
typedef struct tspec {
    unsigned val;   /* value */
    unsigned char tol;  /* tolerance select code */
} tspec;

typedef struct CIR_Param { /* CIR decoder configuration */
    unsigned countDivisor;  /* count clock divisor */
    tspec pa[4],        /* preamble A pulse sequence */
          pb[4];        /* preamble B pulse sequence */
    unsigned char paCount;  /* number of entries in pa[] */
    unsigned char pbCount;  /* number of entries in pb[] */
    int measurePreamblePulse;/* false => even counts specifies pulse off period */
                /* true => even counts specifies cycle period */
    int pbRepeat;       /* if true, pb[] matches a repeat sequence */
    unsigned pulseTol;      /* pulse tolerance */
    unsigned t0;        /* T0 */
    unsigned delT;      /* delta T */
    int fixSymPulseLast;    /* false => fix-width symbol pulse between */
                    /*   edges 0 & 1 */
                /* true => fix-width symbol pulse between */
                /*   edges 1 & 2 */
    int measureSymPulse;    /* false => measure spacing for complete cycle */
            /* true => measure spacing between 2 consecutive edges */
    tspec symPulseWidth;    /* data symbol fix-width pulse period */
    tspec spacingTol;       /* spacing tolerance value and select code */
    unsigned nSymA,     /* no. of symbols for sequence with preamble A */
             nSymB;     /* no. of symbols for sequence with preamble B */
    unsigned bitsPerSym;    /* no. of data bits per symbol */
    int mostSignifSymRecvFirst;/* true => most significant symbol received */
                    /*   first */
                /* false => least significant symbol received */
                /*   first */
    int leftAdjustRecvData; /* true => resulting received data is left */
                    /* adjusted using '0' to pad LSBs */
                    /* false => resulting received data is right */
                    /* adjusted using '0' to pad MSBs */
    int biphaseCoded;       /* true => the input signal is bi-phase coded */
                /* false => the input signal is pulse spacing */
                /*   coded */
    int twoSymPerCy;        /* two symbols per cycle, 1 or 0 */
    int chkStopSym;         /* check stop symbol, 1 or 0 */
    int varLenData;         /* variable length data, 1 or 0. */
    unsigned timeoutDivisor;    /* time-out clock divisor */
    unsigned frameTimeout;  /* frame time-out */
    unsigned edgeTimeout;   /* edge time-out */
    unsigned faultDeadTime; /* mininmum dead-time after fault */
    tspec    stop;          /* stop */
    unsigned dataSymTimeout;    /* data symbol timeout */

    unsigned repeatTimeout;     /* repeat timer timeout */
    int stopParamUnit;      /* stop parameter unit selector: */
                    /*  0: stop has count units */
                    /*  1: stop has timout units */
    int dataSymClkTickUnit;     /* data symbol timer clock tick and */
                    /*  dataSymTimeout units selector: */
                    /*  0: dataSymTimeout has count units */
                    /*  1: dataSymTimeout has timout units */
    int ignoreDataSymTimerEdge1;    /* ignore data symbol timer expiration */
                    /* while waiting for Edge 1; 0 or 1. */
    int dataSymTimerExpStatEn;  /* enable data symbol time-out */
                    /*   expiration flag to lflag bit in */
                    /*   status register */
    int enHavePreambleAftStop;  /* enable havePreambleAftStop parameter */
                    /* for non-biphase decoding, 0 or 1 */
    int havePreambleAftStop;    /* have preamble after stop symbol */
    int restrictiveDecode;      /* restrictive decoding enabled */
    int rc6;            /* RC6 encoded. Requires biphaseCoded=1 */
    int dontValidateMode;       /* don't validate RC6 mode bits */
    unsigned modeBits;      /* RC6 mode bits (3 bits), typically 6 */
    int dontValidateTrailer;    /* Reserved: don't validate RC6 trailer: */
                    /*  0:false, 1:true */
    unsigned trailer;       /* Reserved: RC6 trailer (1 bit): */
                    /*  0 for Mode 6A */
    int dontValidateCustCode;   /* don't validate customer code bits */
    unsigned custCode;      /* RC6 customer code bits (16 bits) */
    unsigned nCustCodeBits;     /* number of RC6 customer code bits: */
    unsigned passModeCustCodePass;
            /* RC6 mode bits and customer code pass-through control. */
            /* 0: Exclude mode bits and customer code from */
            /*    received data. The nccb field determines */
            /*    the size of the customer code. */
            /* 1: Exclude mode bits from the received data, but, */
            /*    include customer code. */
            /* 2: Not allowed. */
            /* 3: Include both mode bits and customer code in */
            /*    the received data. */
} CIR_Param;


/***************************************************************************
Summary:
    KIR user callback function

Description:
    The is the user callback function.  It allows a user to register a
    callback function with the KIR PI.  When a KIR interrupt happens,
    this callback function gets called if it's registered.

See Also:
    BKIR_RegisterCallback(), BKIR_UnregisterCallback()

****************************************************************************/
typedef BERR_Code (*BKIR_Callback)(BKIR_ChannelHandle hChn, void *pData);

/***************************************************************************
Summary:
    Required default settings structure for KIR module.

Description:
    The default setting structure defines the default configure of
    KIR when the device is opened.  Since BKIR is a device
    channel, it also has default settings for a device channel.
    Currently there are no parameters for device setting.

See Also:
    BKIR_Open(), BKIR_OpenChannel()

****************************************************************************/
typedef void *BKIR_Settings;

typedef struct BKIR_ChannelSettings
{
    BKIR_KirPort        irPort;         /* IR port */
    bool                intMode;        /* interrupt enable flag */
    BKIR_KirDevice      customDevice;   /* device that this custom cir is used for */
} BKIR_ChannelSettings;

/***************************************************************************
Summary:
    This function opens KIR module.

Description:
    This function is responsible for opening BKIR module. When BKIR is
    opened, it will create a module handle and configure the module based
    on the default settings. Once the device is opened, it must be closed
    before it can be opened again.

Returns:
    TODO:

See Also:
    BKIR_Close(), BKIR_OpenChannel(), BKIR_CloseChannel(),
    BKIR_GetDefaultSettings()

****************************************************************************/
BERR_Code BKIR_Open(
    BKIR_Handle *pKIR,                  /* [out] Returns handle */
    BCHP_Handle hChip,                  /* [in] Chip handle */
    BREG_Handle hRegister,              /* [in] Register handle */
    BINT_Handle hInterrupt,             /* [in] Interrupt handle */
    const BKIR_Settings *pDefSettings   /* [in] Default settings */
    );

/***************************************************************************
Summary:
    This function closes KIR module.

Description:
    This function is responsible for closing BKIR module. Closing BKIR
    will free main BKIR handle. It is required that all opened
    BKIR channels must be closed before calling this function. If this
    is not done, the results will be unpredicable.

Returns:
    TODO:

See Also:
    BKIR_Open(), BKIR_CloseChannel()

****************************************************************************/
BERR_Code BKIR_Close(
    BKIR_Handle hDev                    /* [in] Device handle */
    );

/***************************************************************************
Summary:
    This function returns the default settings for KIR module.

Description:
    This function is responsible for returns the default setting for
    BKIR module. The returning default setting should be when
    opening the device.

Returns:
    TODO:

See Also:
    BKIR_Open()

****************************************************************************/
BERR_Code BKIR_GetDefaultSettings(
    BKIR_Settings *pDefSettings,        /* [out] Returns default setting */
    BCHP_Handle hChip                   /* [in] Chip handle */
    );

/***************************************************************************
Summary:
    This function returns the total number of channels supported by
    KIR module.

Description:
    This function is responsible for getting total number of channels
    supported by BKIR module, since BKIR device is implemented as a
    device channel.

Returns:
    TODO:

See Also:
    BKIR_OpenChannel(), BKIR_ChannelDefaultSettings()

****************************************************************************/
BERR_Code BKIR_GetTotalChannels(
    BKIR_Handle hDev,                   /* [in] Device handle */
    unsigned int *totalChannels         /* [out] Returns total number downstream channels supported */
    );

/***************************************************************************
Summary:
    This function gets default setting for a KIR module channel.

Description:
    This function is responsible for returning the default setting for
    channel of BKIR. The return default setting is used when opening
    a channel.

Returns:
    TODO:

See Also:
    BKIR_OpenChannel()

****************************************************************************/
BERR_Code BKIR_GetChannelDefaultSettings(
    BKIR_Handle hDev,                   /* [in] Device handle */
    unsigned int channelNo,             /* [in] Channel number to default setting for */
    BKIR_ChannelSettings *pChnDefSettings /* [out] Returns channel default setting */
    );

/***************************************************************************
Summary:
    This function gets default CIR setting for a KIR module channel and specified device.

Description:
    This function is responsible for returning the default setting for
    channel of BKIR and the specified device. The return default setting is
    used when setting custom CIR.

Returns:
    TODO:

See Also:
    BKIR_SetCustomCir()

****************************************************************************/
BERR_Code BKIR_GetDefaultCirParam (
    BKIR_ChannelHandle  hChn,            /* Device channel handle */
    BKIR_KirDevice      device,          /* device type to enable */
    CIR_Param           *pCustomCirParam /* [output] Returns default setting */
);

/***************************************************************************
Summary:
    This function opens KIR module channel.

Description:
    This function is responsible for opening BKIR module channel. When a
    BKIR channel is opened, it will create a module channel handle and
    configure the module based on the channel default settings. Once a
    channel is opened, it must be closed before it can be opened again.

Returns:
    TODO:

See Also:
    BKIR_CloseChannel(), BKIR_GetChannelDefaultSettings()

****************************************************************************/
BERR_Code BKIR_OpenChannel(
    BKIR_Handle hDev,                   /* [in] Device handle */
    BKIR_ChannelHandle *phChn,          /* [out] Returns channel handle */
    unsigned int channelNo,             /* [in] Channel number to open */
    const BKIR_ChannelSettings *pChnDefSettings /* [in] Channel default setting */
    );

/***************************************************************************
Summary:
    This function closes KIR module channel.

Description:
    This function is responsible for closing BKIR module channel. Closing
    BKIR channel it will free BKIR channel handle. It is required that all
    opened BKIR channels must be closed before closing BKIR.

Returns:
    TODO:

See Also:
    BKIR_OpenChannel(), BKIR_CloseChannel()

****************************************************************************/
BERR_Code BKIR_CloseChannel(
    BKIR_ChannelHandle hChn             /* [in] Device channel handle */
    );

/***************************************************************************
Summary:
    This function gets KIR module device handle based on
    the device channel handle.

Description:
    This function is responsible returning BKIR module handle based on the
    BKIR module channel.

Returns:
    TODO:

See Also:

****************************************************************************/
BERR_Code BKIR_GetDevice(
    BKIR_ChannelHandle hChn,            /* [in] Device channel handle */
    BKIR_Handle *pKIR                   /* [out] Returns Device handle */
    );

/***************************************************************************
Summary:
    This function checks to see if data is received.

Description:
    This function is used to check to see if data is received by the KIR receiver.

Returns:
    TODO:

See Also:


****************************************************************************/
BERR_Code BKIR_IsDataReady (
    BKIR_ChannelHandle  hChn,           /* [in] Device channel handle */
    bool                *dataReady      /* [out] flag to indicate if data is ready */
    );

/***************************************************************************
Summary:
    This function reads the received KIR data

Description:
    This function is used to read the data received by the KIR receiver.

Returns:
    TODO:

See Also:


****************************************************************************/
BERR_Code BKIR_Read(
    BKIR_ChannelHandle      hChn,           /* [in] Device channel handle */
    BKIR_KirInterruptDevice *pDevice,       /* [out] pointer to IR device type that generated the key */
    unsigned char           *data           /* [out] pointer to data received */
    );

/***************************************************************************
Summary:
    This function reads the received KIR data.
    Intended to be called from ISR.

Description:
    This function is used to read the data received by the KIR receiver.

Returns:
    TODO:

See Also:


****************************************************************************/
BERR_Code BKIR_Read_Isr(
    BKIR_ChannelHandle      hChn,           /* [in] Device channel handle */
    BKIR_KirInterruptDevice *pDevice,       /* [out] pointer to IR device type that generated the key */
    unsigned char           *data           /* [out] pointer to data received */
    );

/***************************************************************************
Summary:
    This function checks to see if remote A repeat condition occurs.

Description:
    This function is used to check if the remote A repeat condition occurs.

Returns:
    TODO:

See Also:


****************************************************************************/
BERR_Code BKIR_IsRepeated(
    BKIR_ChannelHandle      hChn,           /* [in] Device channel handle */
    bool                    *repeatFlag     /* [out] flag to remote A repeat condition */
    );

/***************************************************************************
Summary:
    This function checks to see if preamble A is detected.

Description:
    This function is used to check if the preamble A is detected.

Returns:
    TODO:

See Also:


****************************************************************************/
BERR_Code BKIR_IsPreambleA(
    BKIR_ChannelHandle      hChn,           /* [in] Device channel handle */
    bool                    *preambleFlag   /* [out] flag for preamble A */
    );

/***************************************************************************
Summary:
    This function checks to see if preamble B is detected.

Description:
    This function is used to check if the preamble B is detected.

Returns:
    TODO:

See Also:


****************************************************************************/
BERR_Code BKIR_IsPreambleB(
    BKIR_ChannelHandle      hChn,           /* [in] Device channel handle */
    bool                    *preambleFlag   /* [out] flag for preamble B */
    );

/***************************************************************************
Summary:
    This function enables a KIR device.

Description:
    This function enables a KIR device type.

Returns:
    TODO:

See Also:

****************************************************************************/
BERR_Code BKIR_EnableIrDevice (
    BKIR_ChannelHandle  hChn,           /* [in] Device channel handle */
    BKIR_KirDevice      device          /* [in] device type to enable */
    );

/***************************************************************************
Summary:
    This function disables a KIR device.

Description:
    This function disables a KIR device type.

Returns:
    TODO:

See Also:

****************************************************************************/
BERR_Code BKIR_DisableIrDevice (
    BKIR_ChannelHandle  hChn,           /* [in] Device channel handle */
    BKIR_KirDevice      device          /* [in] device type to disable */
    );

/***************************************************************************
Summary:
    This function enables a data filter.

Description:
    This function enables a data filter based on the pattern.

Returns:
    TODO:

See Also:

****************************************************************************/
BERR_Code BKIR_EnableDataFilter (
    BKIR_ChannelHandle  hChn,       /* [in] Device channel handle */
    uint64_t            pat0,       /* [in] pattern0 to match (only least significant 48bits are used) */
    uint64_t            pat1,       /* [in] pattern1 to match (only least significant 48bits are used) */
    uint64_t            mask0,      /* [in] don't care bits in pattern0 (only least significant 48bits are used) */
    uint64_t            mask1       /* [in] don't care bits in pattern1 (only least significant 48bits are used) */
    );

/***************************************************************************
Summary:
    This function disables a data filter.

Description:
    This function disables a data filter based on the pattern.

Returns:
    TODO:

See Also:

****************************************************************************/
BERR_Code BKIR_DisableDataFilter (
    BKIR_ChannelHandle  hChn        /* [in] Device channel handle */
    );

/***************************************************************************
Summary:
    This function enables a filter1.

Description:
    This function enables a filter1 using the filter width.
	Any pulse smaller than (28*filter_width+2)/27) microseconds	will be rejected
Returns:
    TODO:

See Also:

****************************************************************************/
BERR_Code BKIR_EnableFilter1 (
    BKIR_ChannelHandle  hChn,       	/* [in] Device channel handle */
    unsigned int        filter_width 	/* filter width if smaller than this to be rejected */
    );

/***************************************************************************
Summary:
    This function disables a data filter.

Description:
    This function disables a data filter based on the pattern.

Returns:
    TODO:

See Also:

****************************************************************************/
BERR_Code BKIR_DisableFilter1 (
    BKIR_ChannelHandle  hChn        /* [in] Device channel handle */
    );

/***************************************************************************
Summary:
    This function disables all KIR devices.

Description:
    This function disables all KIR devices.

Returns:
    TODO:

See Also:

****************************************************************************/
BERR_Code BKIR_DisableAllIrDevices (
    BKIR_ChannelHandle  hChn            /* [in] Device channel handle */
    );

/***************************************************************************
Summary:
    This function gets the event handle for BKIR module channel.

Description:
    This function is responsible for getting the event handle. The
    application code should use this function get BKIR's event handle,
    which the application should use to pend on.  The KIR ISR will
    set the event.

Returns:
    TODO:

See Also:

****************************************************************************/
BERR_Code BKIR_GetEventHandle(
    BKIR_ChannelHandle hChn,            /* [in] Device channel handle */
    BKNI_EventHandle *phEvent           /* [out] Returns event handle */
    );

/***************************************************************************
Summary:
    This function sets the custom device type.

Description:
    This function is called to set the custom device type.
    This function should be called before calling BKIR_EnableIrDevice function
    since custom device type is used in BKIR_EnableIrDevice function

Returns:
    TODO:

See Also:

****************************************************************************/
void BKIR_SetCustomDeviceType (
    BKIR_ChannelHandle  hChn,           /* Device channel handle */
    BKIR_KirDevice      customDevice    /* device that this custom cir is used for */
);

/***************************************************************************
Summary:
    This function sets the custom CIR's parameters.

Description:
    This function is called to set the custom CIR parameters to the ones
    pointed to by pCirParam.  This allows the user to change the CIR
    parameters on the fly.

Returns:
    TODO:

See Also:

****************************************************************************/
void BKIR_SetCustomCir (
    BKIR_ChannelHandle  hChn,           /* [in] Device channel handle */
    CIR_Param           *pCirParam      /* [in] Pointer to custom CIR parameters */
);

/***************************************************************************
Summary:
    This function registers a callback function with KIR PI

Description:
    This function is used to register a callback function with KIR PI.
    When a KIR interrupt happens, if a callback function has been registered
    for this channel, it will call that function.

Returns:
    TODO:

See Also:

****************************************************************************/
void BKIR_RegisterCallback (
    BKIR_ChannelHandle  hChn,       /* Device channel handle */
    BKIR_Callback       callback,   /* Callback function to register */
    void                *pData      /* Data passed to callback function */
);

/***************************************************************************
Summary:
    This function unregisters a callback function with KIR PI

Description:
    This function is used to unregister a callback function for a
    particular KIR channel.

Returns:
    TODO:

See Also:

****************************************************************************/
void BKIR_UnregisterCallback (
    BKIR_ChannelHandle  hChn        /* Device channel handle */
);

#ifdef __cplusplus
}
#endif

#endif



