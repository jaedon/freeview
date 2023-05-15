/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bspi.h $
 * $brcm_Revision: Hydra_Software_Devel/24 $
 * $brcm_Date: 9/28/12 10:59a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/spi/include/bspi.h $
 * 
 * Hydra_Software_Devel/24   9/28/12 10:59a brianlee
 * SW7208-202: Added support for 7208.
 * 
 * Hydra_Software_Devel/23   9/27/12 2:04p brianlee
 * SW7425-3873: Added an API to turn on and off the last byte continue
 * flag.
 * 
 * Hydra_Software_Devel/22   7/26/12 7:32p brianlee
 * SW7468-406: Added support for 7468, which doesn't have UPG SPI.
 * 
 * Hydra_Software_Devel/21   7/23/12 3:01p brianlee
 * SW7358-345: Added APIs to handle HIF SPI get channels and explicitly
 * check for channel type (UPG SPI vs. HIF SPI). SW7584-40: Added 32-bit
 * and 64-bit transfers and support for 7584 SPI changes.
 * 
 * Hydra_Software_Devel/20   3/30/12 4:50p brianlee
 * SW7358-257: Added support for HIF SPI.  For now only enable it for
 * 7358.  Add more chips as necessary.
 * 
 * Hydra_Software_Devel/19   5/5/11 2:14p agin
 * SW7346-170:  Update bpi.h to reflect 3 channels for 7346.
 * 
 * Hydra_Software_Devel/18   5/13/10 3:32p jkim
 * CR3548-909: change the prototyp to match the function definitions in .c
 * file
 * 
 * Hydra_Software_Devel/17   5/7/10 6:41p jkim
 * CR3548-909: implement get function for SetDTLConfig() and Set
 * RDSCLKConfig()
 * 
 * Hydra_Software_Devel/16   10/24/08 3:40p farshidf
 * PR47943: update the SPI mac channel for 3549/3556
 * 
 * Hydra_Software_Devel/15   10/22/08 2:28p farshidf
 * PR47943: add the default settings for channel 2
 * 
 * Hydra_Software_Devel/14   10/13/08 3:36p jkim
 * PR46713: modify function prototype per magnum coding guideline
 * 
 * Hydra_Software_Devel/13   5/21/07 4:14p jkim
 * PR14344: add flag to use Dt and DSCK
 * 
 * Hydra_Software_Devel/12   5/18/07 9:49a erickson
 * PR14344: fix prototype
 * 
 * Hydra_Software_Devel/11   5/17/07 10:29a jkim
 * PR14344: modified to support slower devices. DTL and RDSCLK are now
 * settable.
 * 
 * Hydra_Software_Devel/10   3/18/05 6:50p agin
 * PR14520: keypad for 97398
 * 
 * Hydra_Software_Devel/9   2/9/04 6:42p brianlee
 * PR242: Added support for 16-bit transfer mode.
 * 
 * Hydra_Software_Devel/8   1/28/04 10:22a brianlee
 * PR9499: Fixed breg_spi.h file inclusion.
 * 
 * Hydra_Software_Devel/7   11/6/03 9:51a brianlee
 * Added the function BSPI_CloseSpiRegHandle().
 * 
 * Hydra_Software_Devel/6   10/15/03 2:42p brianlee
 * Moved #define MAX_SPI_CHANNELS here so application can use it.
 * 
 * Hydra_Software_Devel/5   9/25/03 12:17p brianlee
 * Fixed tags for Midas service generator.
 * 
 * Hydra_Software_Devel/4   9/17/03 4:48p lseverin
 * fixed a few documentation warnings
 * 
 * Hydra_Software_Devel/3   9/11/03 6:24p brianlee
 * Modified default setting structure.  Also fixed sample code.
 * 
 * Hydra_Software_Devel/2   7/31/03 10:04a brianlee
 * Removed read/write/setclk functions.  These are now private.  The
 * application should use the register handle to access read/write
 * functions.
 * 
 * Hydra_Software_Devel/1   7/23/03 11:46a brianlee
 * Initial version.
 * 
 ***************************************************************************/


/*= Module Overview *********************************************************
<verbatim>

Overview
The SPI PI module controls the SPI master core within the BCM7038.  
The SPI master core supports several slave devices as selected by the
slave select signals.  For the BCM7038, the SPI core supports 2 slave
selects.  The SPI clock is configurable upon channel opening.  It can 
be set from 53 Khz to 1.6875 Mhz.  By default, the SPI PI will set the
baud rate to the maximum value, 1.6875 Mhz.

Design
The design for BSPI PI API is broken into two parts.

o Part 1 (open/close/configuration):

  These APIs are used for opening and closing BSPI device/device channel.
  When a device/device channel is opened, the device/device channel can be
  configured for transfer speed.

o Part 2 (command):

  These APIs are sending read and write commands using the SPI master controller.

Usage
The usage of BSPI involves the following:

* Configure/Open of BSPI

  * Configure BSPI device for the target system
  * Open BSPI device
  * Configure BSPI device channel for the target system
  * Open BSPI device channel
  * Create BSPI register handle

* Sending commands

  * Send read/write commands using BSPI register handle.

Sample Code
void main( void )
{
    BSPI_Handle       	hSpi;
    BSPI_ChannelHandle   hSpiChan;
    BSPI_ChannelSettings defChnSettings;
    BREG_Handle       	hReg;
    BCHP_Handle       	hChip;
    BINT_Handle			hInt;
    unsigned int 		chanNo;
    uint8_t      		writeBuf[8];
	uint8_t				readBuf[8];
	BREG_SPI_Handle		hSpiReg;

    // Do other initialization, i.e. for BREG, BCHP, etc.
    BSPI_Open( &hSpi, hChip, hReg, hInt, (BSPI_Settings *)NULL );
   
    chanNo= 0; 	// example for channel 0
    BSPI_GetChannelDefaultSettings( hSpi, chanNo, &defChnSettings );

    // Make any changes required from the default values
    defChnSettings.baud = 1200000;		// set clock to 1.2 Mhz
    defChnSettings.clkConfig = (SPI_SPCR0_MSB_CPOL | SPI_SPCR0_MSB_CPHA);
    defChnSettings.intMode = TRUE;		// enable interrupt

    BSPI_OpenChannel( hSpi, &hSpiChan, chanNo, &defChnSettings );

    BSPI_CreateSpiRegHandle (hSpiChan, &hSpiReg);

    //
    // Do a read of 2 bytes from register 0x04 of SPI device whose
    // chip address is 0x28.
    //
    writeBuf[0] = 0x28;								// chip address
    writeBuf[1] = 0x04;								// register offset
    BREG_SPI_Read (hSpiReg, writeBuf, readBuf, 4);   // results in readBuf[2] and readBuf[3]

	printf("Data = %x %x \n", readBuf[2], readBuf[3]);   
    //
    // Do a write of 1 byte from register 0x1C of SPI device whose
    // chip ID is 0x28.
    //
   writeBuf[0] = 0x28;
   writeBuf[1] = 0x1C;
   writeBuf[2] = 0x30;               // value to write
   BREG_SPI_Write (hSpiReg,  writeBuf, 3);
}


</verbatim>
***************************************************************************/


#ifndef BSPI_H__
#define BSPI_H__

#include "bchp.h"
#include "breg_mem.h"
#include "breg_spi.h"
#include "bint.h"
#include "bkni.h"
#include "berr_ids.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
	Number of SPI channels in BCM7038

Description:

See Also:

****************************************************************************/
#if (BCHP_CHIP==3548) || (BCHP_CHIP==3556) || (BCHP_CHIP==7346)
#define MAX_SPI_CHANNELS	3
#else
#define MAX_SPI_CHANNELS	2
#endif

#if (BCHP_CHIP==7358) || (BCHP_CHIP==7468) || (BCHP_CHIP==7208)
#define HAS_HIF_MSPI			1
#define HIF_MSPI_MAX_CHANNELS	2
#else
#define HAS_HIF_MSPI			0
#define HIF_MSPI_MAX_CHANNELS	0
#endif

#if (BCHP_CHIP==7468) || (BCHP_CHIP==7208)
#define HAS_UPG_MSPI			0
#else
#define HAS_UPG_MSPI			1
#endif

#if (HAS_UPG_MSPI==0)
#define SPI_REG(x)				BCHP_HIF_MSPI_##x
#else
#define SPI_REG(x)				BCHP_MSPI_##x
#endif

/***************************************************************************
Summary:
	Error Codes specific to BSPI

Description:

See Also:

****************************************************************************/
#define BSPI_ERR_NOTAVAIL_CHN_NO			BERR_MAKE_CODE(BERR_SPI_ID, 0)

/***************************************************************************
Summary:
	The handles for spi module.

Description:
	Since BSPI is a device channel, it has main device handle as well
	as a device channel handle.

See Also:
	BSPI_Open(), BSPI_OpenChannel()

****************************************************************************/
typedef struct BSPI_P_Handle				*BSPI_Handle;

/***************************************************************************
Summary:
	The handles for spi module.

Description:
	Since BSPI is a device channel, it has main device handle as well
	as a device channel handle.

See Also:
	BSPI_Open(), BSPI_OpenChannel()

****************************************************************************/
typedef struct BSPI_P_ChannelHandle			*BSPI_ChannelHandle;

/***************************************************************************
Summary:
	Enumeration for slave selects

Description:
	This enumeration defines the slave select setting for a SPI channel

See Also:
	None.

****************************************************************************/
typedef enum BSPI_Pcs
{
   BSPI_Pcs_eUpgSpiPcs0 = 0,
   BSPI_Pcs_eUpgSpiPcs1,
   BSPI_Pcs_eUpgSpiPcs2
} BSPI_Pcs;

/***************************************************************************
Summary:
	Required default settings structure for SPI module.

Description:
	The default setting structure defines the default configure of
	SPI when the device is opened.  Since BSPI is a device
	channel, it also has default settings for a device channel.
	Currently there are no parameters for device setting.

See Also:
	BSPI_Open(), BSPI_OpenChannel()

****************************************************************************/
typedef void *BSPI_Settings;

/***************************************************************************
Summary:
	Required default settings structure for SPI module.

Description:
	The default setting structure defines the default configure of
	SPI when the device is opened.  Since BSPI is a device
	channel, it also has default settings for a device channel.
	Currently there are no parameters for device setting.

See Also:
	BSPI_Open(), BSPI_OpenChannel()

****************************************************************************/
typedef enum BSPI_SpiCore
{
	BSPI_SpiCore_Upg = 0,					/* UPG SPI core */
	BSPI_SpiCore_Hif						/* HIF SPI core */
} BSPI_SpiCore;

typedef enum BSPI_EbiCs
{
	BSPI_EbiCs_0 = 0,
	BSPI_EbiCs_1,
	BSPI_EbiCs_2,
	BSPI_EbiCs_3,
	BSPI_EbiCs_4,
	BSPI_EbiCs_5,
	BSPI_EbiCs_6,
	BSPI_EbiCs_7,
	BSPI_EbiCs_unused
} BSPI_EbiCs;

typedef struct BSPI_ChannelSettings
{
	uint32_t	baud;						/* SPI baud rate */
	uint8_t		clkConfig;					/* SPI clock configuration */
	bool		intMode;					/* interrupt enable flag */
	uint8_t		bitsPerTxfr;				/* number of bits per transfer */
	bool 		lastByteContinueEnable; 	/* Last Byte Contiue Enable Flag */ 
	bool 		useUserDtlAndDsclk; 		/* Use User specified DTL and DSCLK */ 
	BSPI_SpiCore spiCore;					/* SPI core to use */
	BSPI_EbiCs	ebiCs;						/* EBI CS line to use as slave select */
} BSPI_ChannelSettings;

typedef void (*BSPI_AssertSSFunc)(void);
typedef void (*BSPI_DeassertSSFunc)(void);

typedef struct BSPI_ChannelInfo
{
	BSPI_SpiCore	spiCore;				/* SPI core to use */
	uint32_t		channelNo;				/* Channel number */
} BSPI_ChannelInfo;


/***************************************************************************
Summary:
	This function opens SPI module.

Description:
	This function is responsible for opening BSPI module. When BSPI is
	opened, it will create a module handle and configure the module based
	on the default settings. Once the device is opened, it must be closed
	before it can be opened again.

Returns:
	TODO:

See Also:
	BSPI_Close(), BSPI_OpenChannel(), BSPI_CloseChannel(),
	BSPI_GetDefaultSettings()

****************************************************************************/
BERR_Code BSPI_Open(
	BSPI_Handle *pSPI,					/* [out] Returns handle */
	BCHP_Handle hChip,					/* [in] Chip handle */
	BREG_Handle hRegister,				/* [in] Register handle */
	BINT_Handle hInterrupt,				/* [in] Interrupt handle */
	const BSPI_Settings *pDefSettings	/* [in] Default settings */
	);

/***************************************************************************
Summary:
	This function closes SPI module.

Description:
	This function is responsible for closing BSPI module. Closing BSPI 
	will free main BSPI handle. It is required that all opened 
	BSPI channels must be closed before calling this function. If this 
	is not done, the results will be unpredicable.

Returns:
	TODO:

See Also:
	BSPI_Open(), BSPI_CloseChannel()

****************************************************************************/
BERR_Code BSPI_Close(
	BSPI_Handle hDev					/* [in] Device handle */
	);

/***************************************************************************
Summary:
	This function returns the default settings for SPI module.

Description:
	This function is responsible for returns the default setting for 
	BSPI module. The returning default setting should be when
	opening the device.

Returns:
	TODO:

See Also:
	BSPI_Open()

****************************************************************************/
BERR_Code BSPI_GetDefaultSettings(
	BSPI_Settings *pDefSettings,		/* [out] Returns default setting */
	BCHP_Handle hChip					/* [in] Chip handle */
	);

/***************************************************************************
Summary:
	This function returns the total number of channels supported by 
	SPI module.

Description:
	This function is responsible for getting total number of channels
	supported by BSPI module, since BSPI device is implemented as a
	device channel.

Returns:
	TODO:

See Also:
	BSPI_OpenChannel(), BSPI_ChannelDefaultSettings()

****************************************************************************/
BERR_Code BSPI_GetTotalChannels(
	BSPI_Handle hDev,					/* [in] Device handle */
	unsigned int *totalChannels			/* [out] Returns total number downstream channels supported */
	);

/***************************************************************************
Summary:
	This function returns the total number of UPG channels supported by 
	SPI module.

Description:
	This function is responsible for getting total number of UPG channels
	supported by BSPI module, since BSPI device is implemented as a
	device channel.

Returns:
	TODO:

See Also:
	BSPI_OpenChannel(), BSPI_ChannelDefaultSettings()

****************************************************************************/
BERR_Code BSPI_GetTotalUpgSpiChannels(
    BSPI_Handle hDev,                   /* Device handle */
    unsigned int *totalUpgChannels      /* [output] Returns total number of UPG SPI hannels supported */
    );

/***************************************************************************
Summary:
	This function returns the total number of HIF channels supported by 
	SPI module.

Description:
	This function is responsible for getting total number of HIF channels
	supported by BSPI module, since BSPI device is implemented as a
	device channel.

Returns:
	TODO:

See Also:
	BSPI_OpenChannel(), BSPI_ChannelDefaultSettings()

****************************************************************************/
BERR_Code BSPI_GetTotalHifSpiChannels(
    BSPI_Handle hDev,                   /* Device handle */
    unsigned int *totalHifChannels      /* [output] Returns total number of HIF SPI hannels supported */
    );

/***************************************************************************
Summary:
	This function gets default setting for a SPI module channel.

Description:
	This function is responsible for returning the default setting for
	channel of BSPI. The return default setting is used when opening
	a channel.

Returns:
	TODO:

See Also:
	BSPI_OpenChannel()

****************************************************************************/
BERR_Code BSPI_GetChannelDefaultSettings(
	BSPI_Handle hDev,					/* [in] Device handle */
	unsigned int channelNo,				/* [in] Channel number to default setting for */
    BSPI_ChannelSettings *pChnDefSettings /* [out] Returns channel default setting */
    );

/***************************************************************************
Summary:
	This function gets default setting for a SPI module channel.

Description:
	This function is responsible for returning the default setting for
	channel of BSPI. The caller sets the BSPI_ChannelInfo input information.

Returns:
	TODO:

See Also:
	BSPI_OpenChannel()

****************************************************************************/
BERR_Code BSPI_GetChannelDefaultSettings_Ext(
    BSPI_Handle 		hDev,                   /* Device handle */
    BSPI_ChannelInfo	channel,				/* Channel number info */
    BSPI_ChannelSettings *pChnDefSettings		/* [output] Returns channel default setting */
    );

/***************************************************************************
Summary:
	This function opens SPI module channel.

Description:
	This function is responsible for opening BSPI module channel. When a
	BSPI channel is	opened, it will create a module channel handle and
	configure the module based on the channel default settings. Once a 
	channel is opened, it must be closed before it can be opened again.

Returns:
	TODO:

See Also:
	BSPI_CloseChannel(), BSPI_GetChannelDefaultSettings()

****************************************************************************/
BERR_Code BSPI_OpenChannel(
	BSPI_Handle hDev,					/* [in] Device handle */
	BSPI_ChannelHandle *phChn,			/* [out] Returns channel handle */
	unsigned int channelNo,				/* [in] Channel number to open */
	const BSPI_ChannelSettings *pChnDefSettings /* [in] Channel default setting */
	);

/***************************************************************************
Summary:
	This function closes SPI module channel.

Description:
	This function is responsible for closing BSPI module channel. Closing
	BSPI channel it will free BSPI channel handle. It is required that all
	opened BSPI channels must be closed before closing BSPI.

Returns:
	TODO:

See Also:
	BSPI_OpenChannel(), BSPI_CloseChannel()

****************************************************************************/
BERR_Code BSPI_CloseChannel(
	BSPI_ChannelHandle hChn				/* [in] Device channel handle */
	);

/***************************************************************************
Summary:
	This function gets SPI module device handle based on
	the device channel handle.

Description:
	This function is responsible returning BSPI module handle based on the
	BSPI module channel.

Returns:
	TODO:

See Also:

****************************************************************************/
BERR_Code BSPI_GetDevice(
	BSPI_ChannelHandle hChn,			/* [in] Device channel handle */
	BSPI_Handle *pSPI					/* [out] Returns Device handle */
	);


/***************************************************************************
Summary:
	This function creates an SPI register handle.

Description:
	This function is responsible for creating an SPI register handle for 
	master SPI controller.  It fills in the BREG_SPI_Handle structure with
	pointers to the SPI PI functions.  The application can then use this
	SPI register interface to perform read and write operations.
	
Returns:
	TODO:

See Also:

****************************************************************************/
BERR_Code BSPI_CreateSpiRegHandle(
	BSPI_ChannelHandle hChn,			/* [in] Device channel handle */
	BREG_SPI_Handle *pSpiRegHandle		/* [out] register handle */
	);

/***************************************************************************
Summary:
	This function closes SPI register handle

Description:
	This function is responsible for closing the SPI register handle.
	This function frees BSPI register handle.

Returns:
	TODO:

See Also:
	BSPI_CreateSpiRegHandle()

****************************************************************************/
BERR_Code BSPI_CloseSpiRegHandle(
	BREG_SPI_Handle		hSpiReg				/* [in] SPI register handle */
	);

/***************************************************************************
Summary:
	This function aborts the current SPI transfer

Description:
	This function allows the user to abort an on-going SPI transfer.
		
Returns:
	TODO:

See Also:

****************************************************************************/
BERR_Code BSPI_AbortTxfr (
	BSPI_ChannelHandle channel			/* [in] Device channel handle */
	);

/***************************************************************************
Summary:
	This function sets the SPI clock config

Description:
		
Returns:
	TODO:

See Also:

****************************************************************************/
BERR_Code BSPI_GetClkConfig(
	BSPI_ChannelHandle hChn,			/* Device channel handle */
	uint8_t *pClkConfig					/* pointer to clock config */
	);

/***************************************************************************
Summary:
	This function gets the SPI clock clock config

Description:
		
Returns:
	TODO:

See Also:

****************************************************************************/
BERR_Code BSPI_SetClkConfig(
	BSPI_ChannelHandle hChn,			/* Device channel handle */
	uint8_t clkConfig					/* clock config */
	);

/***************************************************************************
Summary:
	This function gets length of delay after transfer

Description:
		
Returns:
	TODO:

See Also:

****************************************************************************/
BERR_Code BSPI_GetDTLConfig (
    BSPI_ChannelHandle hChn,    			/* Device channel handle */
    uint32_t *pDTLConfig        			/* pointer to DTLConfig */
	);

/***************************************************************************
Summary:
	This function sets length of delay after transfer

Description:
		
Returns:
	TODO:

See Also:

****************************************************************************/
BERR_Code BSPI_SetDTLConfig (
	BSPI_ChannelHandle context,				/* Device channel handle */
	const uint32_t data						/* data to write */
	);

/***************************************************************************
Summary:
	This function gets the length of delay from PCS valid to SCK transition

Description:
		
Returns:
	TODO:

See Also:

****************************************************************************/
BERR_Code BSPI_GetRDSCLKConfig(
    BSPI_ChannelHandle hChn,    			/* Device channel handle */
    uint32_t *pRDSCLKConfig     			/* pointer to RDSCLKConfig */
	);

/***************************************************************************
Summary:
	This function sets the length of delay from PCS valid to SCK transition

Description:
		
Returns:
	TODO:

See Also:

****************************************************************************/
BERR_Code BSPI_SetRDSCLKConfig(
	BSPI_ChannelHandle context,				/* Device channel handle */
	const uint32_t data						/* data to write */
	);

/***************************************************************************
Summary:
	This function sets the functions to assert SS and deassert SS for HIF MSPI

Description:
		
Returns:
	TODO:

See Also:

****************************************************************************/
void BSPI_RegisterSSFunctions (
	BSPI_ChannelHandle hChn, 				/* Device channel handle */
	BSPI_AssertSSFunc assertFunc, 			/* Assert SS function */
	BSPI_DeassertSSFunc deassertFunc		/* Deassert SS function */
	);


/***************************************************************************
Summary:
	This function sets the flag which controls whether to continue driving
	SS after the last byte of transfer.

Description:
		
Returns:
	TODO:

See Also:

****************************************************************************/
void BSPI_SetLastByteContinueEnable(
    BSPI_ChannelHandle hChn,				/* Device channel handle */
    bool bEnable							/* Enable or disable last byte cont flag */
);

#ifdef __cplusplus
}
#endif
 
#endif



