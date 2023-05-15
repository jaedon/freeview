/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bi2c.h $
 * $brcm_Revision: Hydra_Software_Devel/74 $
 * $brcm_Date: 9/26/12 7:00a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/i2c/include/bi2c.h $
 * 
 * Hydra_Software_Devel/74   9/26/12 7:00a agin
 * SW7584-45:  bi2c.c can't build for 7584 with latest code.
 * 
 * Hydra_Software_Devel/73   9/25/12 10:21a agin
 * SWNOOS-564:  Removed reference to BCHP_CHIP for determining
 * BI2C_MAX_I2C_CHANNELS.  Removed reference to
 * BI2C_MAX_I2C_MSTR_CHANNELS due to redundancy.
 * 
 * Hydra_Software_Devel/72   8/16/12 4:59p agin
 * SWNOOS-564:  Support i2c on 7445.
 * 
 * Hydra_Software_Devel/71   7/23/12 4:35p randyjew
 * SW7563-2: Add 7563  support
 * 
 * Hydra_Software_Devel/70   5/2/12 8:11p agin
 * SW7584-23:  Support 7584.
 * 
 * Hydra_Software_Devel/69   1/3/12 1:15p agin
 * SWNOOS-507:  Improve i2c performance.
 * 
 * Hydra_Software_Devel/68   12/16/11 7:37p bselva
 * SW7360-6: Added appframework support for 7360 platform
 * 
 * Hydra_Software_Devel/67   10/28/11 2:27p mward
 * SW7435-7: Add 7435 like 7425.
 * 
 * Hydra_Software_Devel/66   10/7/11 6:49a agin
 * SW7429-7:  Add support for i2c.
 * 
 * Hydra_Software_Devel/65   9/30/11 9:47a agin
 * SWNOOS-485:  Support 7346 B0.
 * 
 * Hydra_Software_Devel/64   9/12/11 12:41p jzhu
 * SWBLURAY-27236:7640 B0 BRINGUP, change 7640 b0 I2C to 3 channels
 *
 * Hydra_Software_Devel/63   8/29/11 1:51p jrubio
 * SW7346-470: remove 7346A0 wrap over code
 *
 * Hydra_Software_Devel/62   6/23/11 11:13a agin
 * SWNOOS-464:  Add prototype for BI2C_Set4ByteXfrMode and
 * BI2C_Is4ByteXfrMode.
 *
 * Hydra_Software_Devel/61   3/9/11 5:31p etrudeau
 * SWBLURAY-23692: 7640 has 4 i2c channels with BSC Master C not connected
 *
 * Hydra_Software_Devel/60   2/24/11 10:59a agin
 * SW7346-96:  Enable extra sw i2c channel for 7346 by default.
 *
 * Hydra_Software_Devel/59   2/21/11 7:34a agin
 * SW7346-96:  Add ability to use GPIO lines not tied to the BSC I2C for
 * I2C bit bang.
 *
 * Hydra_Software_Devel/58   1/26/11 2:26p etrudeau
 * SWBLURAY-23692: add 7640 support
 *
 * Hydra_Software_Devel/57   11/17/10 7:28p agin
 * SW7231-19:  Add I2C PI support for 7231/7344/7346
 *
 * Hydra_Software_Devel/56   11/1/10 5:08p xhuang
 * SW7552-4: Add 7552 support
 *
 * Hydra_Software_Devel/55   8/27/10 5:17p tdo
 * SW7425-12: Add 7425 I2C support
 *
 * Hydra_Software_Devel/54   8/11/10 4:04p jrubio
 * SW7358-7: fix typo
 *
 * Hydra_Software_Devel/53   8/11/10 12:40p agin
 * SW7358-7:  Add support for 7358A0 in I2C.
 *
 * Hydra_Software_Devel/52   6/25/10 11:58a agin
 * SW7422-12:  Change BI2C_MAX_I2C_CHANNELS and BI2C_MAX_I2C_MSTR_CHANNELS
 * to 5 for 7422.
 *
 * Hydra_Software_Devel/51   6/22/10 11:37a vanessah
 * SW7422-12:  To support appframework. Missing files added:
 * magnum\portinginterface\pwr rockford\appframework\src\board\97422  To
 * do list: 1. in framework_board.c, more initialization to be done.  2.
 * More registers mapping, like clock generation as well as
 * BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL etc
 *
 * Hydra_Software_Devel/50   11/9/09 11:27a gmohile
 * SW7408-1 : Add 7408 support
 *
 * Hydra_Software_Devel/49   10/22/09 1:26p agin
 * SW7405-2686: Change BREG_I2C to abstract SW and HW implementation.
 *
 * Hydra_Software_Devel/48   10/2/09 6:34p lwhite
 * SW7468-6: Add 7468 support
 *
 * Hydra_Software_Devel/47   9/25/09 1:20p agin
 * SW7405-3038:  Add BI2C_SwReset().
 *
 * Hydra_Software_Devel/46   9/15/09 10:40a rpereira
 * SW7630-45: Adding 7630 support
 *
 * Hydra_Software_Devel/45   9/1/09 8:59p rpereira
 * SW7550-30: Adding 7550 support
 *
 * Hydra_Software_Devel/44   7/29/09 12:36p jrubio
 * PR55232: add 7342/7340
 *
 * Hydra_Software_Devel/43   7/21/09 4:39p mward
 * PR 55545: Support 7125.
 *
 * Hydra_Software_Devel/42   7/20/09 10:18a agin
 * PR56687: Change I2C clock enum from Khz to Hz.
 *
 * Hydra_Software_Devel/41   4/27/09 11:16a jhaberf
 * PR53796: Updating i2c build to support BCM35130 DTV chip.
 *
 * Hydra_Software_Devel/40   4/10/09 5:36p rpereira
 * PR52971: added 7635 support
 *
 * Hydra_Software_Devel/39   2/20/09 11:14a agin
 * PR52310: Add i2c sda delay control for 7601.
 *
 * Hydra_Software_Devel/38   2/19/09 3:29p agin
 * PR52310: Add i2c sda delay control for 7601.
 *
 * Hydra_Software_Devel/37   1/31/09 1:15a jrubio
 * PR51629: add 7336 support
 *
 * Hydra_Software_Devel/36   12/8/08 6:39p agin
 * PR49867: Added I2C_MAX_I2C_CHANNELS for 7420.
 *
 * Hydra_Software_Devel/35   12/2/08 4:28p kaushikb
 * PR49867: Adding support for 7420
 *
 * Hydra_Software_Devel/34   8/21/08 8:27a fbasso
 * PR 44540: added support for 7601.
 *
 * Hydra_Software_Devel/33   4/4/08 10:43a farshidf
 * PR39178: add support for 3548 and 3556
 *
 * Hydra_Software_Devel/32   1/13/08 9:02p rpereira
 * PR36162: BI2C_MAX_I2C_CHANNELS and BI2C_MAX_I2C_MSTR_CHANNELS incorrect
 * in bi2c.h for BCM7440 Bx
 *
 * Hydra_Software_Devel/31   11/27/07 6:02p farshidf
 * PR36894: update for 7335
 *
 * Hydra_Software_Devel/30   10/14/07 3:52p jkim
 * PR14344: Add 7325 support
 *
 * Hydra_Software_Devel/29   4/24/07 5:25p jkim
 * PR29538: Add burst mode support
 *
 * Hydra_Software_Devel/28   3/30/07 6:54p katrep
 * PR29073:Added support for 7405
 *
 * Hydra_Software_Devel/27   10/31/06 3:14p erickson
 * PR25108: add 7403 support
 *
 * Hydra_Software_Devel/26   9/18/06 2:27p agin
 * PR23800: Timeout value hardcoded in bi2c.c
 *
 * Hydra_Software_Devel/25   9/6/06 4:13p jkim
 * PR14344: adding 7440 support
 *
 * Hydra_Software_Devel/24   8/30/06 6:04p mward
 * PR21678: 7118 chip 97118 board - has 1 virtual 2 total channels.
 *
 * Hydra_Software_Devel/23   8/30/06 5:05p mward
 * PR21678: 7118 chip 97118 board - has 2 virtual channels.
 *
 * Hydra_Software_Devel/22   8/9/06 10:58a agin
 * PR23362: Add 3563 support.
 *
 * Hydra_Software_Devel/22   8/9/06 10:50a agin
 * PR23362: Add 3563 support.
 *
 * Hydra_Software_Devel/21   6/15/06 5:13p mward
 * PR21678: Add support for 7118 chip 97118 board
 *
 * Hydra_Software_Devel/20   2/14/06 2:58p agin
 * PR19666: 7438, not 7348.
 *
 * Hydra_Software_Devel/19   2/14/06 2:27p agin
 * PR19666: Need to create 7438 I2C porting interface.
 *
 * Hydra_Software_Devel/18   1/23/06 4:58p agin
 * PR19220: Support 5th I2C controller.
 *
 * Hydra_Software_Devel/17   1/14/06 10:19p agin
 * PR19076: Support BCM7400.
 *
 * Hydra_Software_Devel/15   9/9/05 5:05p jkim
 * PR16945:Per conversation with Dave Lwin, change BI2C_Delete2cRegHandle
 * back to BI2C_CloseI2cRegHandle
 *
 * Hydra_Software_Devel/14   9/7/05 3:44p jkim
 * PR16942: Change MAX_I2C_MSTR_CHANNELS to BI2C_MAX_I2C_MSTR_CHANNELS and
 * MAX_I2C_CHANNELS to BI2C_MAX_I2C_CHANNELS
 *
 * Hydra_Software_Devel/13   8/1/05 5:23p vsilyaev
 * PR 16012: Added support for 7401\
 *
 * Hydra_Software_Devel/12   3/16/05 4:04p dlwin
 * PR 14240: More updated for 3560, added code to support sharing of
 * one I2C master.
 *
 * Hydra_Software_Devel/11   3/10/05 8:30a dlwin
 * PR 14240: Added support for 3560 and future chip that use this same
 * I2C core.
 *
 * Hydra_Software_Devel/10   1/28/04 10:21a brianlee
 * PR9499: Fix breg_i2c.h file inclusion.
 *
 * Hydra_Software_Devel/9   11/6/03 9:47a brianlee
 * Added BI2C_CloseI2cRegHandle() function.
 *
 * Hydra_Software_Devel/8   10/15/03 2:33p brianlee
 * Moved #defne MAX_I2C_CHANNELS to bi2c.h so application code can use it.
 *
 * Hydra_Software_Devel/7   9/25/03 11:17a brianlee
 * Added tags for Midas service generator.
 *
 * Hydra_Software_Devel/6   9/18/03 10:16a brianlee
 * Make SetClk and GetClk functions public so the caller can change the
 * clock rate after the channel is opened.
 *
 * Hydra_Software_Devel/5   9/11/03 11:40a brianlee
 * Changed enum values for clock speed.  Also added polling option.
 *
 * Hydra_Software_Devel/4   8/21/03 4:02p brianlee
 * Removed EDDCRead and EDDCWrite functions.  Make them private.
 * Application should go through register interface to perform these
 * functions.
 *
 * Hydra_Software_Devel/3   8/19/03 10:26a brianlee
 * Fixed a typo.
 *
 * Hydra_Software_Devel/2   7/31/03 9:43a brianlee
 * Made some functions private.  Application should go through I2C
 * register handle to perform read/write.
 *
 * Hydra_Software_Devel/1   7/22/03 6:33p brianlee
 * Initial version.
 *
 ***************************************************************************/


/*= Module Overview *********************************************************
<verbatim>

Overview
The I2C PI module controls the 4 I2C master controllers within the BCM7038.
The I2C master controllers within the BCM7038 are independent of each other.
Each controller is capable of the following clock rates: 47KHz, 50KHz, 93KHz,
100KHz, 187KHz, 200KHz, 375KHz, and 400KHz.  The I2C controllers now support
unlimited data transfer sizes through the ability to suppress START and STOP
conditions.

Design
The design for BI2C PI API is broken into two parts.
  Part 1 (open/close/configuration):
    These APIs are used for opening and closing BI2C device/device channel.
    When a device/device channel is opened, the device/device channel can be
    configured for transfer speed.
  Part 2 (command):
    These APIs are sending read and write commands using the I2C master controller.

Usage
The usage of BI2C involves the following:

   * Configure/Open of BI2C
      * Configure BI2C device for the target system
      * Open BI2C device
      * Configure BI2C device channel for the target system
      * Open BI2C device channel
      * Create BI2C register handle

   * Sending commands
      * Send read/write commands using BI2C register handle.

Sample Code
void main( void )
{
    BI2C_Handle       hI2c;
    BI2C_ChannelHandle   hI2cChan;
    BI2C_ChannelSettings defChnSettings;
    BREG_Handle       hReg;
    BCHP_Handle       hChip;
    int chanNo;
    unsigned char      data[10];
    BREG_I2C_Handle      hI2cReg;
    BINT_Handle         hInt;

    // Do other initialization, i.e. for BREG, BCHP, etc

    BI2C_Open( &hI2c, hChip, hReg, hInt, (BI2C_Settings *)NULL );

    chanNo = 0; // example for channel 0
    BI2C_GetChannelDefaultSettings( hI2c, chanNo, &defChnSettings );

    // Make any changes required from the default values
    defChnSettings.clkRate      = BI2C_Clk_eClk400Khz;

    BI2C_OpenChannel( hI2c, &hI2cChan, chanNo, &defChnSettings );

    // Get handle to I2C Reg
    BI2C_CreateI2cRegHandle (hI2cChan, &hI2cReg);

    //
    // Do a read of 2 bytes from register 0x1b of I2C device whose
    // chip ID is 0x9C.
    //
    BREG_I2C_Read (hI2cReg, 0x9c, 0x1b, &data, 2);

    //
    // Do a write of 3 bytes from register 0x1000 of I2C device whose
    // chip ID is 0x8E.
    //
    // Fill in data to send
    data[0]               = 0xb2;
    data[1]               = 0x77;
    data[2]               = 0x20;
    BREG_I2C_WriteA16 (hI2cReg, 0x8e, 0x1000, &data, 3);
}

</verbatim>
***************************************************************************/


#ifndef BI2C_H__
#define BI2C_H__

#include "bchp.h"
#include "breg_mem.h"
#include "breg_i2c.h"
#include "bint.h"
#include "bkni.h"
#include "berr_ids.h"
#include "bchp_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
    Number of I2C channels

Description:

See Also:

****************************************************************************/
#if (BCHP_CHIP==7563) || (BCHP_CHIP==7584) /* 7563 and 7584 don't follow normal convention */
    #define BI2C_MAX_I2C_CHANNELS 3
#elif defined(BCHP_BSCE_REG_START)
    #define BI2C_MAX_I2C_CHANNELS 5
#elif defined (BCHP_BSCD_REG_START)
    #define BI2C_MAX_I2C_CHANNELS 4
#elif defined (BCHP_BSCC_REG_START)
    #define BI2C_MAX_I2C_CHANNELS 3
#elif defined (BCHP_BSCB_REG_START)
    #define BI2C_MAX_I2C_CHANNELS 2
#elif defined (BCHP_BSCA_REG_START)
    #define BI2C_MAX_I2C_CHANNELS 1
#endif

#if (BCHP_CHIP==7346) && (BCHP_VER==BCHP_VER_A0)
    #define EXTRA_SW_I2C_CHANNELS 1
#else
    #define EXTRA_SW_I2C_CHANNELS 0
#endif

/***************************************************************************
Summary:
    Error Codes specific to BI2C

Description:

See Also:39178

****************************************************************************/
#define BI2C_ERR_NOTAVAIL_CHN_NO            BERR_MAKE_CODE(BERR_I2C_ID, 0)
#define BI2C_ERR_NO_ACK                     BERR_MAKE_CODE(BERR_I2C_ID, 1)
#define BI2C_ERR_SINGLE_MSTR_CREATE         BERR_MAKE_CODE(BERR_I2C_ID, 2)



/***************************************************************************
Summary:
    The handles for i2c module.

Description:
    Since BI2C is a device channel, it has main device handle as well
    as a device channel handle.

See Also:
    BI2C_Open(), BI2C_OpenChannel()

****************************************************************************/
typedef struct BI2C_P_Handle                *BI2C_Handle;
typedef struct BI2C_P_ChannelHandle         *BI2C_ChannelHandle;

/***************************************************************************
Summary:
    Enumeration for i2c clock rate

Description:
    This enumeration defines the clock rate for the I2C master

See Also:
    None.

****************************************************************************/
typedef unsigned int BI2C_Clk;
#define BI2C_Clk_eClk47Khz  47000
#define BI2C_Clk_eClk50Khz  50000
#define BI2C_Clk_eClk93Khz  93000
#define BI2C_Clk_eClk100Khz 100000
#define BI2C_Clk_eClk187Khz 187000
#define BI2C_Clk_eClk200Khz 200000
#define BI2C_Clk_eClk375Khz 375000
#define BI2C_Clk_eClk400Khz 400000

/***************************************************************************
Summary:
    Enumeration for i2c clock rate

Description:
    This enumeration defines the clock rate for the I2C master

See Also:
    None.

****************************************************************************/
typedef enum
{
    BI2C_eSdaDelay370ns = 370,
    BI2C_eSdaDelay482ns = 482,
    BI2C_eSdaDelay593ns = 593,
    BI2C_eSdaDelay704ns = 704,
    BI2C_eSdaDelay815ns = 815,
    BI2C_eSdaDelay926ns = 926,
    BI2C_eSdaDelay1037ns = 1037
}  BI2C_SdaDelay;

/***************************************************************************
Summary:
    Enumeration for i2c timeout field

Description:
    This enumeration defines whether to use a timeout value based on i2c
    clk speed or not.

See Also:
    None.

****************************************************************************/
typedef enum
{
   BI2C_TimeoutBasedOnClkSpeed  = 0
}  BI2C_Timeout;

/***************************************************************************
Summary:
    Required default settings structure for I2C module.

Description:
    The default setting structure defines the default configure of
    I2C when the device is opened.  Since BI2C is a device
    channel, it also has default settings for a device channel.
    Currently there are no parameters for device setting.

See Also:
    BI2C_Open(), BI2C_OpenChannel()

****************************************************************************/
typedef void *BI2C_Settings;

typedef struct BI2C_ChannelSettings
{
    BI2C_Clk            clkRate;        /* I2C clock speed */
    bool                intMode;        /* use interrupt flag */
    unsigned int        timeoutMs;      /* Timeout value in milliseconds.  */
                                        /* If equal to BI2C_TimeoutBasedOnClkSpeed, */
                                        /* timeout value is calculated based on the */
                                        /* i2c clk speed */
	bool				burstMode;		/* enable/disable burst mode read */
	bool				softI2c;		/* i2c gpio mode */
	bool				fourByteXferMode; /* use four byte transfer mode */
} BI2C_ChannelSettings;

/***************************************************************************
Summary:
    This function opens I2C module.

Description:
    This function is responsible for opening BI2C module. When BI2C is
    opened, it will create a module handle and configure the module based
    on the default settings. Once the device is opened, it must be closed
    before it can be opened again.

Returns:
    TODO:

See Also:
    BI2C_Close(), BI2C_OpenChannel(), BI2C_CloseChannel(),
    BI2C_GetDefaultSettings()

****************************************************************************/
BERR_Code BI2C_Open(
    BI2C_Handle *pI2C,                  /* [out] Returns handle */
    BCHP_Handle hChip,                  /* [in] Chip handle */
    BREG_Handle hRegister,              /* [in] Register handle */
    BINT_Handle hInterrupt,             /* [in] Interrupt handle */
    const BI2C_Settings *pDefSettings   /* [in] Default settings */
    );

/***************************************************************************
Summary:
    This function closes I2C module.

Description:
    This function is responsible for closing BI2C module. Closing BI2C
    will free main BI2C handle. It is required that all opened
    BI2C channels must be closed before calling this function. If this
    is not done, the results will be unpredicable.

Returns:
    TODO:

See Also:
    BI2C_Open(), BI2C_CloseChannel()

****************************************************************************/
BERR_Code BI2C_Close(
    BI2C_Handle hDev                    /* [in] Device handle */
    );

/***************************************************************************
Summary:
    This function returns the default settings for I2C module.

Description:
    This function is responsible for returns the default setting for
    BI2C module. The returning default setting should be when
    opening the device.

Returns:
    TODO:

See Also:
    BI2C_Open()

****************************************************************************/
BERR_Code BI2C_GetDefaultSettings(
    BI2C_Settings *pDefSettings,        /* [out] Returns default setting */
    BCHP_Handle hChip                   /* [in] Chip handle */
    );

/***************************************************************************
Summary:
    This function returns the total number of channels supported by
    I2C module.

Description:
    This function is responsible for getting total number of channels
    supported by BI2C module, since BI2C device is implemented as a
    device channel.

Returns:
    TODO:

See Also:
    BI2C_OpenChannel(), BI2C_ChannelDefaultSettings()

****************************************************************************/
BERR_Code BI2C_GetTotalChannels(
    BI2C_Handle hDev,                   /* [in] Device handle */
    unsigned int *totalChannels         /* [out] Returns total number downstream channels supported */
    );

/***************************************************************************
Summary:
    This function gets default setting for a I2C module channel.

Description:
    This function is responsible for returning the default setting for
    channel of BI2C. The return default setting is used when opening
    a channel.

Returns:
    TODO:

See Also:
    BI2C_OpenChannel()

****************************************************************************/
BERR_Code BI2C_GetChannelDefaultSettings(
    BI2C_Handle hDev,                   /* [in] Device handle */
    unsigned int channelNo,             /* [in] Channel number to default setting for */
    BI2C_ChannelSettings *pChnDefSettings /* [out] Returns channel default setting */
    );

/***************************************************************************
Summary:
    This function opens I2C module channel.

Description:
    This function is responsible for opening BI2C module channel. When a
    BI2C channel is opened, it will create a module channel handle and
    configure the module based on the channel default settings. Once a
    channel is opened, it must be closed before it can be opened again.

Returns:
    TODO:

See Also:
    BI2C_CloseChannel(), BI2C_GetChannelDefaultSettings()

****************************************************************************/
BERR_Code BI2C_OpenChannel(
    BI2C_Handle hDev,                   /* [in] Device handle */
    BI2C_ChannelHandle *phChn,          /* [out] Returns channel handle */
    unsigned int channelNo,             /* [in] Channel number to open */
    const BI2C_ChannelSettings *pChnDefSettings /* [in] Channel default setting */
    );

/***************************************************************************
Summary:
    This function closes I2C module channel.

Description:
    This function is responsible for closing BI2C module channel. Closing
    BI2C channel it will free BI2C channel handle. It is required that all
    opened BI2C channels must be closed before closing BI2C.

Returns:
    TODO:

See Also:
    BI2C_OpenChannel(), BI2C_CloseChannel()

****************************************************************************/
BERR_Code BI2C_CloseChannel(
    BI2C_ChannelHandle hChn             /* [in] Device channel handle */
    );

/***************************************************************************
Summary:
    This function gets I2C module device handle based on
    the device channel handle.

Description:
    This function is responsible returning BI2C module handle based on the
    BI2C module channel.

Returns:
    TODO:

See Also:

****************************************************************************/
BERR_Code BI2C_GetDevice(
    BI2C_ChannelHandle hChn,            /* [in] Device channel handle */
    BI2C_Handle *pI2C                   /* [out] Returns Device handle */
    );


/***************************************************************************
Summary:
    This function creates an I2C register handle.

Description:
    This function is responsible for creating an I2C register handle for
    master I2C controller.  It fills in the BREG_I2C_Handle structure with
    pointers to the I2C PI functions.  The application can then use this
    I2C register interface to perform read and write operations.

Returns:
    TODO:

See Also:

****************************************************************************/
BERR_Code BI2C_CreateI2cRegHandle(
    BI2C_ChannelHandle hChn,            /* [in] Device channel handle */
    BREG_I2C_Handle *pI2cRegHandle      /* [out] register handle */
    );

/***************************************************************************
Summary:
    This function closes I2C register handle

Description:
    This function is responsible for closing the I2C register handle.
    This function frees BI2C register handle.

Returns:
    TODO:

See Also:
    BI2C_CreateI2cRegHandle()

****************************************************************************/
BERR_Code BI2C_CloseI2cRegHandle(
    BREG_I2C_Handle     hI2cReg             /* [in] I2C register handle */
    );

/***************************************************************************
Summary:
    This function sets the clock rate for an I2C channel

Description:
    This function allows the user to change the I2C clock rate.

Returns:
    TODO:

See Also:

****************************************************************************/
void BI2C_SetClk(
    BI2C_ChannelHandle  hChn,           /* [in] Device channel handle */
    BI2C_Clk            clk             /* [in] clock rate setting */
    );

/***************************************************************************
Summary:
    This function gets the clock rate for an I2C channel

Description:
    This function returns the current clock rate setting for an I2C channel

Returns:
    TODO:

See Also:

****************************************************************************/
BI2C_Clk BI2C_GetClk(
    BI2C_ChannelHandle  hChn            /* [in] Device channel handle */
    );

/***************************************************************************
Summary:
    This function sets the SDA delay for an I2C channel

Description:
    This function allows the user to change the SDS delay.

Returns:
    TODO:

See Also:

****************************************************************************/
void BI2C_SetSdaDelay(
    BI2C_ChannelHandle  hChn,           /* Device channel handle */
    BI2C_SdaDelay       sdaDelay        /* Sda delay value */
    );

/***************************************************************************
Summary:
	This function enable/disable I2C burst mode operations.

Description:

Returns:
	TODO:

See Also:

****************************************************************************/
BERR_Code BI2C_SetBurstMode(
	BI2C_ChannelHandle hChn,	/* [in] I2C channel handle */
	bool bBurstMode				/* [out] Enable or Disable burst mode */
	);

/***************************************************************************
Summary:
	This function will verify if I2C burst mode has been enabled or
	disabled on an I2C channel.

Description:

Returns:
	TODO:

See Also:

****************************************************************************/
BERR_Code BI2C_GetBurstMode(
	BI2C_ChannelHandle hChn,	/* [in] I2C channel handle */
	bool *pbBurstMode			/* [out] current burst mode? */
	);

/***************************************************************************
Summary:
	This function will set the DATA_REG_SIZE field.

Description:

Returns:
	TODO:

See Also:

****************************************************************************/
void BI2C_Set4ByteXfrMode(
    BI2C_ChannelHandle  hChn,           /* Device channel handle */
    bool b4ByteMode                     /* Enable or Disable 4 byte xfr mode */
    );

/***************************************************************************
Summary:
	This function will check if the DATA_REG_SIZE field is set.

Description:

Returns:
	TODO:

See Also:

****************************************************************************/
bool BI2C_Is4ByteXfrMode(
    BI2C_ChannelHandle  hChn           /* Device channel handle */
    );

/***************************************************************************
Summary:
	This function will perform a software reset.

Description:

After an interruption in protocol, power loss or system reset, any 2-wire
part can be protocol reset by following these steps: (a) Create a start bit
condition, (b) clock 9 cycles, (c) create another start bit followed by stop
bit condition. The device is ready for next communication after above steps
have been completed.

Returns:
	TODO:

See Also:

****************************************************************************/
BERR_Code BI2C_SwReset(
    BI2C_ChannelHandle  hChn           /* [in] Device channel handle */
	);

#ifdef __cplusplus
}
#endif

#endif



