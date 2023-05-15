/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhab.h $
 * $brcm_Revision: Hydra_Software_Devel/48 $
 * $brcm_Date: 10/25/12 12:28p $
 *
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/hab/include/common/bhab.h $
 * 
 * Hydra_Software_Devel/48   10/25/12 12:28p atanugul
 * SW3461-252: Report internalGainDaisy, internalGainLoopThrough,
 * externalGainBypassable and externalGainTotal in units of 1/100th of a
 * dB
 * 
 * Hydra_Software_Devel/47   10/24/12 5:06p atanugul
 * SW3461-252: rename externalGainBypassable to externalGainBypassed in
 * BHAB_InternalGainSettings struture
 * 
 * Hydra_Software_Devel/46   10/24/12 3:00p atanugul
 * SW3461-252: Add externalGainBypassable to BHAB_InternalGainSettings
 * struture
 * 
 * Hydra_Software_Devel/45   10/18/12 5:15p atanugul
 * SW3461-269: Merge to main
 * 
 * Hydra_Software_Devel/44   10/18/12 3:42p atanugul
 * SW3128-228: Add API Support for Reading New Front-End 312X, 314X, 346X
 * F/W Version IDs
 *
 * Hydra_Software_Devel/SW3461-252/2   10/17/12 12:02p vishk
 * SW3461-270: Add API Support for Reading New Front-End 312X, 314X, 346X
 * F/W Version IDs
 * 
 * Hydra_Software_Devel/SW3461-252/1   9/28/12 5:49p atanugul
 * SW3461-269: Add support for BHAB_GetVersionInfo(); Add tnrApplication
 * and rfInputMode to BHAB_ConfigSettings; Add BHAB_GetInternalGain(),
 * BHAB_GetExternalGain() and BHAB_SetExternalGain() API
 *
 * Hydra_Software_Devel/42   9/28/12 1:37p atanugul
 * SW3128-228: Add support for BHAB_GetVersionInfo()
 * 
 * Hydra_Software_Devel/41   7/19/12 12:07p atanugul
 * SW3472-8: Add EWS support to 3472; power on LEAP memories before FW
 * download for 3472
 * 
 * Hydra_Software_Devel/40   6/30/12 5:46p atanugul
 * SW3128-174: Propogate error through HAB return value
 * 
 * Hydra_Software_Devel/39   6/13/12 4:36p vishk
 * SW7405-3392: Update 'berr_ids.h' for HAB error code
 * 
 * Hydra_Software_Devel/38   4/10/12 3:06p atanugul
 * SW3462-4: Add HAB support for 3462
 * 
 * Hydra_Software_Devel/37   3/30/12 6:08p atanugul
 * SW3128-140: Add Spectrum Analyzer feature to the TNR PI
 * 
 * Hydra_Software_Devel/36   1/18/12 4:36p atanugul
 * SW3128-93: Update BHAB_CHK_RETCODE() macro to use BERR_TRACE()
 * 
 * Hydra_Software_Devel/35   1/11/12 6:25p atanugul
 * SW3128-104: Fix BHAB_312x_P_CheckHab() and BHAB_312x_P_DecodeError() to
 * return the proper error codes.
 * 
 * Hydra_Software_Devel/34   1/10/12 4:57p atanugul
 * SW3128-107:Enable HAB CRC Check in PI
 * 
 * Hydra_Software_Devel/33   12/8/11 3:12p atanugul
 * SW3128-89: Add PI support for Spectrum Analyzer Data retrieval
 * 
 * Hydra_Software_Devel/32   11/9/11 2:57p atanugul
 * SW3128-68: Add BHAB_ReadSlave() and BHAB_ReadSlave() API to 3128
 * 
 * Hydra_Software_Devel/31   11/2/11 4:42p atanugul
 * SW3461-3: Update BHAB_GetApVersion to include familyId, AP major
 * version, minor version and remove script version and configuration
 * version
 * 
 * Hydra_Software_Devel/30   10/18/11 3:20p atanugul
 * SW3461-67: change write_len, read_len and command_len in
 * BHAB_SendHabCommand() from type uint8_t to uint16_t
 * 
 * Hydra_Software_Devel/29   8/11/11 4:18p enavarro
 * SWSATFE-86: added more BHAB error codes
 * 
 * Hydra_Software_Devel/28   8/11/11 10:22a enavarro
 * SWSATFE-86: added AST device to BHAB_DevId list
 * 
 * Hydra_Software_Devel/27   8/11/11 9:24a enavarro
 * SW7425-883: added BHAB_DevId for diseqc
 * 
 * Hydra_Software_Devel/26   7/28/11 10:03a enavarro
 * SW7425-883: added BHAB_DevId for satellite front end cores
 * 
 * Hydra_Software_Devel/25   7/28/11 9:58a enavarro
 * SW7425-883: added interrupt types for satellite front end
 * 
 * Hydra_Software_Devel/24   6/30/11 3:58p atanugul
 * SW3128-25: Add BHAB_ReadRegister() and BHAB_WriteRegister() prototypes
 * 
 * Hydra_Software_Devel/23   6/20/11 11:46a atanugul
 * SW3461-5: Add LoopThrough Support for 3461
 * 
 * Hydra_Software_Devel/22   6/1/11 3:43p atanugul
 * SW3461-5: Add THD support to 3461
 * 
 * Hydra_Software_Devel/21   5/27/11 4:28p atanugul
 * SW3461-3: Added BHAB_GetConfigSettings() and BHAB_SetConfigSettings()
 * 
 * Hydra_Software_Devel/20   5/6/11 3:48p atanugul
 * SW3128-2: Added support to configure TM_MTSIF_CTRL - Transport Control
 * Register for BCM3128
 * 
 * Hydra_Software_Devel/19   4/29/11 10:16a atanugul
 * SW3128-8: Add SPI support for 3128
 * 
 * Hydra_Software_Devel/18   3/28/11 12:13p atanugul
 * SW3128-2: Add Support for 8 cores and code cleanup
 * 
 * Hydra_Software_Devel/17   3/16/11 4:52p atanugul
 * SW3128-2: Add Support for BCM3128
 * 
 * Hydra_Software_Devel/16   3/2/11 6:08p atanugul
 * SW3128-2: Add Support for BCM3128
 * 
 * Hydra_Software_Devel/15   2/1/11 5:41p VISHK
 * SW7422-171: Nexus support for the 3128 (8 ADS core + tuner on daughter
 * board) plug on 7422 board  Read and write memory modified to 32 bit
 * addressing to support 3128 chipset as it has 32 bit memory addressing.
 * 
 * Hydra_Software_Devel/14   12/2/10 7:17p VISHK
 * SW7405-5037: To make the i2c address programmable for the slave
 * device(LNA) of 31xx frontend chip.
 * 
 * Hydra_Software_Devel/13   11/2/10 5:59p VISHK
 * SW7420-1020: Request for non-blocking call support in Nexus frontend
 * 
 * Hydra_Software_Devel/12   10/21/10 6:48p vishk
 * SW7420-1020: Request for non-blocking call support in Nexus frontend
 * 
 * Hydra_Software_Devel/11   7/2/10 4:23p vishk
 * SW7420-873: Add support to inform application to update frontend tuner
 * gain and to inform that there is no signal to tune upon multiple tries
 * to acquire
 * 
 * Hydra_Software_Devel/10   6/22/10 6:02p vishk
 * SW7420-839: Add support to program AGC gain to 31xx TNR PI
 * 
 * Hydra_Software_Devel/9   10/5/09 10:43a vishk
 * SW7405-2851: 31xx: Add support to configure the Watchdog feature of
 * 31xx.
 * 
 * Hydra_Software_Devel/8   5/12/09 10:54a vishk
 * PR 46431: ADS PI: Add support for BCM3117, BCM3114, and BCM3112.
 * PR 46436: AOB PI: Add support for BCM3117 and BCM3114
 * 
 * Hydra_Software_Devel/7   4/10/09 11:51a vishk
 * PR 46431: ADS PI: Add support for BCM3117, BCM3114, and BCM3112.
 * PR 46436: AOB PI: Add support for BCM3117 and BCM3114
 * 
 * Hydra_Software_Devel/6   3/3/09 1:08p vishk
 * PR 46431: ADS PI: Add support for BCM3117, BCM3114, and BCM3112.
 * PR 46436: AOB PI: Add support for BCM3117 and BCM3114
 * 
 * Hydra_Software_Devel/5   1/29/09 7:17p vishk
 * PR 46431: ADS PI: Add support for BCM3117, BCM3114, and BCM3112.
 * PR 46436: AOB PI: Add support for BCM3117 and BCM3114
 * 
 * Removed GetLockStateChangeEventHandle.
 * 
 * Hydra_Software_Devel/4   1/17/09 2:37p vishk
 * PR 46431: ADS PI: Add support for BCM3117, BCM3114, and BCM3112.
 * PR 46436: AOB PI: Add support for BCM3117 and BCM3114
 * 
 * Added comment.
 * 
 * Hydra_Software_Devel/3   1/16/09 7:12p vishk
 * PR 46431: ADS PI: Add support for BCM3117, BCM3114, and BCM3112.
 * PR 46436: AOB PI: Add support for BCM3117 and BCM3114
 * 
 * Hydra_Software_Devel/2   10/27/08 11:11a gmohile
 * PR 47386 : Add temination byte for send hab command
 * 
 * Hydra_Software_Devel/1   10/20/08 2:52p gmohile
 * PR 47386 : Checkin initial version
 * 
 ***************************************************************************/

#ifndef BHAB_H
#define BHAB_H

#ifdef __cplusplus
extern "C" {
#endif


#include "berr_ids.h"
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "bfec.h"


#define BHAB_CHK_RETCODE(x) \
   { if ((retCode = (x)) != BERR_SUCCESS) goto done; }   

/* bit definitions for BHAB_ApStatus */
#define BHAB_APSTATUS_RESET      0x00000001 /* AP is in reset state */
#define BHAB_APSTATUS_IDLE       0x00000002 /* AP is in idle state */
#define BHAB_APSTATUS_RUN        0x00000000 /* AP is running */
#define BHAB_APSTATUS_RUN_MASK   0x00000003 /* AP execution state mask */
#define BHAB_APSTATUS_HAB_DONE   0x00000400 /* HAB done */
#define BHAB_APSTATUS_HAB_ERR    0x00001000 /* HAB access violation */
#define BHAB_APSTATUS_MEM_ERR    0x00002000 /* memory access violation */
#define BHAB_APSTATUS_IOMB_ERR   0x00004000 /* io_mbox isb transfer error */
#define BHAB_APSTATUS_H_ERR      0x00008000 /* host transfer error */
#define BHAB_APSTATUS_INIT_DONE  0x00800000 /* AP init done */
#define BHAB_APSTATUS_LOCK       0x00400000 /* inband lock status */
#define BHAB_APSTATUS_HAB_MASK   0x00800403 /* HAB available condition mask */
#define BHAB_APSTATUS_HAB_READY  0x00800400 /* HAB available condition */
#define BHAB_APSTATUS_HABCMD_ERR 0x00080000 /* HAB command syntax error */
#define BHAB_APSTATUS_AP_ERR     0x00040000 /* internal AP error */


/***************************************************************************
Summary:
	BHAB error codes

Description:

See Also:

****************************************************************************/
#define BHAB_ERR_AP_FAIL            BERR_MAKE_CODE(BERR_HAB_ID, 0)  /* This is a BERR_Code to indicate that host was unable to reset/idle/run the AP. */
#define BHAB_ERR_HOST_XFER          BERR_MAKE_CODE(BERR_HAB_ID, 1)  /* This is a BERR_Code to indicate a host transfer error (underrun/overrun). */
#define BHAB_ERR_IOMB_XFER          BERR_MAKE_CODE(BERR_HAB_ID, 2)  /* This is a BERR_Code to indicate an IO Mailbox ISB transfer error. */
#define BHAB_ERR_MEMAV              BERR_MAKE_CODE(BERR_HAB_ID, 3)  /* This is a BERR_Code to indicate a memory access violation. */
#define BHAB_ERR_AP_EEPROM          BERR_MAKE_CODE(BERR_HAB_ID, 4)  /* This is a BERR_Code to indicate no i2c ack from EEPROM. */
#define BHAB_ERR_AP_NOT_INIT        BERR_MAKE_CODE(BERR_HAB_ID, 5)  /* This is a BERR_Code to indicate that the AP has failed to initialize. */
#define BHAB_ERR_HABAV              BERR_MAKE_CODE(BERR_HAB_ID, 6)  /* This is a BERR_Code to indicate an HAB access violation. */
#define BHAB_ERR_HAB_NOT_AVAIL      BERR_MAKE_CODE(BERR_HAB_ID, 7)  /* This is a BERR_Code to indicate that the HAB is busy. */
#define BHAB_ERR_HAB_ERR            BERR_MAKE_CODE(BERR_HAB_ID, 8)  /* This is a BERR_Code to indicate an HAB command format error. */
#define BHAB_ERR_HAB_NO_ACK         BERR_MAKE_CODE(BERR_HAB_ID, 9)  /* This is a BERR_Code to indicate that the HAB command was not processed by AP. */
#define BHAB_ERR_HAB_FAIL           BERR_MAKE_CODE(BERR_HAB_ID, 10) /* This is a BERR_Code to indicate bad/unexpected HAB data read. */
#define BHAB_ERR_AP_HABAV           BERR_MAKE_CODE(BERR_HAB_ID, 11) /* This is a BERR_Code to indicate an AP-internal error. */
#define BHAB_ERR_AP_COPY            BERR_MAKE_CODE(BERR_HAB_ID, 12) /* This is a BERR_Code to indicate an AP-internal error. */
#define BHAB_ERR_AP_IRQ             BERR_MAKE_CODE(BERR_HAB_ID, 13) /* This is a BERR_Code to indicate an AP-internal error. */
#define BHAB_ERR_AP_SCR             BERR_MAKE_CODE(BERR_HAB_ID, 14) /* This is a BERR_Code to indicate an AP-internal error. */
#define BHAB_ERR_AP_ISB             BERR_MAKE_CODE(BERR_HAB_ID, 15) /* This is a BERR_Code to indicate an AP-internal error. */
#define BHAB_ERR_AP_WD              BERR_MAKE_CODE(BERR_HAB_ID, 16) /* This is a BERR_Code to indicate an AP-internal error. */
#define BHAB_ERR_AP_STACK           BERR_MAKE_CODE(BERR_HAB_ID, 17) /* This is a BERR_Code to indicate an AP-internal error. */
#define BHAB_ERR_AP_BSC             BERR_MAKE_CODE(BERR_HAB_ID, 18) /* This is a BERR_Code to indicate an AP-internal error. */
#define BHAB_ERR_IOMB_BUSY          BERR_MAKE_CODE(BERR_HAB_ID, 19) /* This is a BERR_Code to indicate the IO MBOX was busy. */
#define BHAB_ERR_HAB_TIMEOUT        BERR_MAKE_CODE(BERR_HAB_ID, 20) /* This is a BERR_Code to indicate an HAB timeout. */
#define BHAB_ERR_AP_UNKNOWN         BERR_MAKE_CODE(BERR_HAB_ID, 21) /* This is a BERR_Code to indicate an AP-internal error. */
#define BHAB_ERR_MI2C_NO_ACK        BERR_MAKE_CODE(BERR_HAB_ID, 22) /* This is a BERR_Code to indicate no ack from i2c slave device (e.g. tuner). */
#define BHAB_ERR_HAB_CHECKSUM       BERR_MAKE_CODE(BERR_HAB_ID, 23) /* This is a BERR_Code to indicate bad checksum in HAB */
#define BHAB_ERR_HAB_BAD_PARAM      BERR_MAKE_CODE(BERR_HAB_ID, 24) /* This is a BERR_Code to indicate invalid parameter specified in HAB command */
#define BHAB_ERR_HAB_NOT_SUPP       BERR_MAKE_CODE(BERR_HAB_ID, 25) /* This is a BERR_Code to indicate AP does not support this HAB command */
#define BHAB_ERR_INVALID_CRC        BERR_MAKE_CODE(BERR_HAB_ID, 26) /* This is a BERR_Code to indicate that the CRC check failed for the HAB command */
#define BHAB_ERR_INVALID_OPCODE     BERR_MAKE_CODE(BERR_HAB_ID, 26) /* This is a BERR_Code to indicate that the Opcode for the HAB command sent is Invalid */
#define BHAB_ERR_CMD_PROCESSING_ERR BERR_MAKE_CODE(BERR_HAB_ID, 26) /* This is a BERR_Code to indicate AP HAB command processing error */


/***************************************************************************
Summary:
	Enum for HAB interrupt tyes

Description:
	This enum lists the different types of interrupts from the device

See Also:
	None.

****************************************************************************/
typedef enum BHAB_InterruptType{
	BHAB_Interrupt_eHabDone,
	BHAB_Interrupt_eInitDone,
	BHAB_Interrupt_eLockChange,
	BHAB_Interrupt_eUpdateGain,
	BHAB_Interrupt_eNoSignal,
	BHAB_Interrupt_eQamAsyncStatusReady,
	BHAB_Interrupt_eOobAsyncStatusReady,
	BHAB_Interrupt_eTc2AsyncStatusReady,
	BHAB_Interrupt_eThdAsyncStatusReady,
	BHAB_Interrupt_eOdsAsyncStatusReady,
	BHAB_Interrupt_eFskReady,
	BHAB_Interrupt_eDiseqcDone,
	BHAB_Interrupt_eDiseqcOverVoltage,
	BHAB_Interrupt_eDiseqcUnderVoltage,
	BHAB_Interrupt_eStatus,
	BHAB_Interrupt_ePeakScanDone,
	BHAB_Interrupt_eSpectrumAnalyzerDataReady,
	BHAB_Interrupt_eEmergencyWarningSystem,
	BHAB_Interrupt_eMax
} BHAB_InterruptType;


typedef enum BHAB_DevId{
	BHAB_DevId_eADS0 = 0,
	BHAB_DevId_eADS1,
	BHAB_DevId_eADS2,
	BHAB_DevId_eADS3,
	BHAB_DevId_eADS4,
	BHAB_DevId_eADS5,
	BHAB_DevId_eADS6,
	BHAB_DevId_eADS7,
	BHAB_DevId_eAOB0 = 10,
	BHAB_DevId_eAOB1,
	BHAB_DevId_eAUS0 = 20,
	BHAB_DevId_eAUS1,
	BHAB_DevId_eVSB0 = 30,	
	BHAB_DevId_eVSB1,
	BHAB_DevId_eTC20 = 40,	
	BHAB_DevId_eTC21,    
	BHAB_DevId_eTHD0 = 50,    
	BHAB_DevId_eTHD1,
	BHAB_DevId_eTNR0 = 60,    
	BHAB_DevId_eTNR1,
	BHAB_DevId_eODS0 = 70,
	BHAB_DevId_eODS1,
	BHAB_DevId_eAST,
	BHAB_DevId_eMax
}BHAB_DevId;

/***************************************************************************
Summary:
    Enumeration for Daisy Chain
    
Description:
	This enum lists the different types of Daisy Chain outputs from the 
    device.

See Also:
	BHAB_GetConfigSettings(), BHAB_SetConfigSettings()

****************************************************************************/
typedef enum BHAB_RfDaisyChain
{
    BHAB_RfDaisyChain_eOff,         /* Daisychaining is tunrned off. */
    BHAB_RfDaisyChain_eExternalLna, /* Daisychaining through UHF path. This Rf daisychaining path does not use internal LNA. */
    BHAB_RfDaisyChain_eInternalLna, /* Daisychaining through VHF path. This Rf daisychaining path uses internal LNA. */    
    BHAB_RfDaisyChain_eMax
} BHAB_RfDaisyChain;

/***************************************************************************
Summary:
	The handle for hab module.

Description:
	This is an opaque handle that is used to access HAB.

See Also:
	BHAB_Open()

****************************************************************************/
typedef struct BHAB_P_Handle *BHAB_Handle;


/***************************************************************************
Summary:
	Typedef for status information

Description:
	This typedef contains AP status information

See Also:
	None.

****************************************************************************/
typedef uint32_t BHAB_ApStatus;

/***************************************************************************
Summary:	

Description:

See Also:
	
****************************************************************************/
typedef struct BHAB_NmiGpioSettings
{
	bool		polarity;
	bool		enable;
	uint8_t		select;
} BHAB_NmiGpioSettings;

/***************************************************************************
Summary:	

Description:

See Also:
	
****************************************************************************/
typedef struct BHAB_WatchDogTimerSettings
{
	bool					start;
	bool					oneShot;
	bool					nmiMode;
	uint32_t				timeout;
	BHAB_NmiGpioSettings	clearWatchDog;
} BHAB_WatchDogTimerSettings;

/***************************************************************************
Summary:	

Description:

See Also:
	
****************************************************************************/
typedef struct BHAB_NmiSettings
{
	BHAB_NmiGpioSettings	powerDownDetect;
	BHAB_NmiGpioSettings	spare1;
	BHAB_NmiGpioSettings	spare2;
	BHAB_NmiGpioSettings	nmi;			/*Here the polarity is always high. */	
} BHAB_NmiSettings;

/***************************************************************************
Summary:
    Enumeration for RF Input Mode

Description:
    This field specifies the Input mode.

See Also:
    BHAB_GetConfigSettings(), BHAB_SetConfigSettings()

****************************************************************************/
typedef enum BHAB_RfInputMode
{
  BHAB_RfInputMode_eOff,            /* Tuner is off. */
  BHAB_RfInputMode_eExternalLna,    /* Tuner Rf input through UHF path. This Rf path does not use internal LNA. */
  BHAB_RfInputMode_eInternalLna,    /* Tuner Rf input through VHF path. This Rf path uses internal LNA. */
  BHAB_RfInputMode_eStandardIf,     /* 44 MHz or 36 MHz */
  BHAB_RfInputMode_eLowIf,          /*4 MHz to 5 MHz. */
  BHAB_RfInputMode_eBaseband,
  BHAB_RfInputMode_eLast
} BHAB_RfInputMode;

/***************************************************************************
Summary:
    Enumeration for Tuner Application

Description:
    This field controls the tuner application. This setting has to be match
    the TunerApplication parameters in tuner parameters.

See Also:
    BTNR_SetSettings(), BTNR_GetSettings()

****************************************************************************/
typedef enum BHAB_TunerApplication
{
  BHAB_TunerApplication_eCable,
  BHAB_TunerApplication_eTerrestrial,
  BHAB_TunerApplication_eLast
} BHAB_TunerApplication;

/***************************************************************************
Summary:
     Structure to set HAB config settings.

Description:
    Defines the structure to set HAB config settings.

See Also:
	BHAB_GetConfigSettings(), BHAB_SetConfigSettings()

****************************************************************************/
typedef struct BHAB_ConfigSettings
{
    BHAB_RfDaisyChain       daisyChain;         /* daisy chain output*/
    bool                    enableLoopThrough;  /* loop through */
    BHAB_RfInputMode        rfInputMode;        /* RF input mode */   
    BHAB_TunerApplication   tnrApplication;     /* TNR application */
} BHAB_ConfigSettings;

/***************************************************************************
Summary:
    Structure for External Gain Settings

Description:
    Structure for External Gain Settings

See Also:
    BHAB_GetExternalGain(), BHAB_GetInternallGain()
    
****************************************************************************/
typedef struct BHAB_ExternalGainSettings
{
    int16_t externalGainBypassable; /* External Gain bypassable in units of 1/100th of a dB */
    int16_t externalGainTotal; /* External Gain Total in units of 1/100th of a dB */
} BHAB_ExternalGainSettings;

/***************************************************************************
Summary:
    Structure for Internal Gain Settings

Description:
    Structure for Internal Gain Settings

See Also:
    BHAB_GetInternalGain()
    
****************************************************************************/
typedef struct BHAB_InternalGainSettings
{
    uint32_t    frequency;                  /* frequency */ 
    int16_t     internalGainLoopThrough;    /* Internal Gain to Loop Through in units of 1/100th of a dB */
    int16_t     internalGainDaisy;          /* Internal Gain to Daisy in units of 1/100th of a dB */
    bool        externalGainBypassed;       /* This flag indicates whether the external gain into the tuner is bypass-able or not */
} BHAB_InternalGainSettings;

/***************************************************************************
Summary:
    Structure for External Gain Settings

Description:
    Structure for External Gain Settings

See Also:
    BHAB_GetInternalGain()

****************************************************************************/
typedef struct BHAB_InternalGainInputParams
{
    uint32_t frequency;              /* frequency */ 
} BHAB_InternalGainInputParams;

/***************************************************************************
Summary:
	This defines the function prototype that is used for callbacks.

Description:
	None.

See Also:
	None.

****************************************************************************/
typedef BERR_Code (*BHAB_IntCallbackFunc)( void * pParm1, int parm2 );


typedef struct BHAB_Settings BHAB_Settings;


/***************************************************************************
Summary:
	Structure for API function table

Description:
   This structure contains pointers to all public BHAB functions.

See Also:
	None.

****************************************************************************/
typedef struct BHAB_ApiFuncTable
{	  
    BERR_Code (*Open)(BHAB_Handle *, void*, const BHAB_Settings *);
    BERR_Code (*Close)(BHAB_Handle);
    BERR_Code (*InitAp)(BHAB_Handle, const uint8_t *);
    BERR_Code (*GetApStatus)(BHAB_Handle, BHAB_ApStatus *);
    BERR_Code (*GetApVersion)(BHAB_Handle, uint32_t*, uint32_t*, uint16_t*, uint8_t*, uint8_t*);
    BERR_Code (*GetVersionInfo)(BHAB_Handle, BFEC_SystemVersionInfo*);
    BERR_Code (*ReadRegister)(BHAB_Handle, uint32_t, uint32_t*);
    BERR_Code (*WriteRegister)(BHAB_Handle, uint32_t, uint32_t*);	  
    BERR_Code (*ReadMemory)(BHAB_Handle, uint32_t, uint8_t*, uint32_t);
    BERR_Code (*WriteMemory)(BHAB_Handle, uint32_t, const uint8_t*, uint32_t);
    BERR_Code (*ReadMbox)(BHAB_Handle, uint32_t, uint32_t*);
    BERR_Code (*WriteMbox)(BHAB_Handle, uint32_t, uint32_t*);
    BERR_Code (*HandleInterrupt_isr)(BHAB_Handle);
    BERR_Code (*ProcessInterruptEvent)(BHAB_Handle);
    BERR_Code (*EnableLockInterrupt)(BHAB_Handle, BHAB_DevId, bool);
    BERR_Code (*InstallInterruptCallback)(BHAB_Handle, BHAB_DevId, BHAB_IntCallbackFunc, void *, int);
    BERR_Code (*UnInstallInterruptCallback)(BHAB_Handle, BHAB_DevId);
    BERR_Code (*SendHabCommand)(BHAB_Handle, uint8_t *, uint16_t, uint8_t *, uint16_t, bool, bool, uint16_t);
    BERR_Code (*GetInterruptEventHandle)(BHAB_Handle, BKNI_EventHandle*);
    BERR_Code (*GetWatchDogTimer)(BHAB_Handle, BHAB_WatchDogTimerSettings *);
    BERR_Code (*SetWatchDogTimer)(BHAB_Handle, const BHAB_WatchDogTimerSettings *);
    BERR_Code (*GetNmiConfig)(BHAB_Handle, BHAB_NmiSettings *);
    BERR_Code (*SetNmiConfig)(BHAB_Handle, const BHAB_NmiSettings *);
    BERR_Code (*GetConfigSettings)(BHAB_Handle, BHAB_ConfigSettings *);
    BERR_Code (*SetConfigSettings)(BHAB_Handle, const BHAB_ConfigSettings *);
    BERR_Code (*ReadSlave)(BHAB_Handle, uint8_t, uint32_t, uint32_t *, uint8_t);
    BERR_Code (*WriteSlave)(BHAB_Handle, uint8_t, uint32_t, uint32_t *, uint8_t);
    BERR_Code (*GetInternalGain)(BHAB_Handle, const BHAB_InternalGainInputParams *, BHAB_InternalGainSettings *);
    BERR_Code (*GetExternalGain)(BHAB_Handle, BHAB_ExternalGainSettings *);    
    BERR_Code (*SetExternalGain)(BHAB_Handle, const BHAB_ExternalGainSettings *);    
} BHAB_ApiFuncTable;


/***************************************************************************
Summary:
	Structure for HAB settings

Description:
	This structure contains the settings for HAB.

See Also:
	None.

****************************************************************************/
struct BHAB_Settings
{
    uint16_t chipAddr;  /* i2c chip address (i2c) */
    void     (*interruptEnableFunc)(bool, void*); /* callback function for enabling/disabling L1 interrupt */
    void     *interruptEnableFuncParam; /* parameter passed to interruptEnableFunc */
    BHAB_ApiFuncTable api;
    uint8_t slaveChipAddr;  /* i2c chip address for the 31xx's i2c  slave device */
    bool    isSpi;
    bool    isMtsif;    
};


/******************************************************************************
Summary:
   Initializes the BHAB API.
Description:
   This function must be called first to get a BHAB_Handle.  This handle is 
   used by all other function calls in the BHAB API.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_Open(
	BHAB_Handle *handle,     /* [out] BHAB handle */
	void        *pReg,       /* [in] pointer ot i2c or spi handle */
	const BHAB_Settings *pDefSettings /* [in] Default Settings */
);


/******************************************************************************
Summary:
   Closes the BHAB API.
Description:
   This function releases all the resources allocated by BHAB API and disables 
   host interrupts.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_Close(
	BHAB_Handle h   /* [in] BHAB handle */
);


/******************************************************************************
Summary:
   Downloads the microcode and runs the AP.
Description:
   This function downloads the microcode to the AP RAM and then starts running 
   the AP.
Returns:
   BERR_Code : BERR_SUCCESS = AP is running and successfully initialized
******************************************************************************/
BERR_Code BHAB_InitAp(
	BHAB_Handle   h,       /* [in] BHAB handle */
	const uint8_t *pImage  /* [in] pointer to AP microcode image */
);


/******************************************************************************
Summary:
   Returns AP version information.
Description:
   This function returns the Family ID, chip ID, chip revision, AP microcode 
   version.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_GetApVersion(
    BHAB_Handle    handle,     /* [in]  BHAB handle */
    uint32_t       *pFamilyId, /* [out] Chip Family id */
    uint32_t       *pChipId,   /* [out] chip id */
    uint16_t       *pChipVer,  /* [out] chip revision number */
    uint8_t        *pMajApVer,    /* [out] AP microcode major version */
    uint8_t        *pMinApVer     /* [out] AP microcode minor version */
);


/******************************************************************************
Summary:
   Returns AP version information.
Description:
   This function returns the familyId, chipId, chipVersion, fwMajorVersion
    fwMinorVersion, fwBuildType and fwBuildId
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_GetVersionInfo(
    BHAB_Handle             handle,         /* [in]  BHAB handle */
    BFEC_SystemVersionInfo   *pVersionInfo   /* [out]  Returns FW version information */
);  


/******************************************************************************
Summary:
   Reads from an RBUS register.
Description:
   This function is normally used to read a specific RBUS register.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_ReadRegister(
	BHAB_Handle handle,     /* [in] BHAB handle */
	uint32_t    reg,        /* [in] address of register to read */
	uint32_t    *val        /* [in] contains data that was read */
);


/******************************************************************************
Summary:
   Writes to an RBUS register.
Description:
   This function is normally used to write to a specific RBUS register.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_WriteRegister(
	BHAB_Handle handle,     /* [in] BHAB handle */
	uint32_t    reg,        /* [in] address of register to read */
	uint32_t    *val        /* [in] contains data that was read */
);


/******************************************************************************
Summary:
   Reads from the AP RAM/ROM space.
Description:
   This function is normally used for verifying the contents of the AP RAM 
   after microcode download.  This function assumes that the AP is already in 
   idle or reset state.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_ReadMemory(
	BHAB_Handle h,  /* [in] BHAB PI Handle */
	uint32_t addr,  /* [in] starting address */
	uint8_t *buf,   /* [out] holds the data read */
	uint32_t n      /* [in] number of bytes to read */
);


/******************************************************************************
Summary:
   Writes to the AP RAM space.
Description:
   This function is normally used for downloading the microcode.  This function
   assumes that the AP is already in idle or reset state.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_WriteMemory(
	BHAB_Handle handle, /* [in] BHAB PI Handle */ 
	uint32_t addr, /* [in] starting address in AP RAM */
	uint8_t *buf,  /* [in] specifies the data to write */
	uint32_t n     /* [in] number of bytes to write */
);

/******************************************************************************
Summary:
   Writes to the mailbox space.
Description:
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_ReadMbox(
	BHAB_Handle handle,    /* [in] BHAB PI Handle */
	uint32_t    reg,  /* [in] RBUS register address */
	uint32_t    *val  /* [out] value read from register */
);

/******************************************************************************
Summary:
   Writes to the mailbox space.
Description:
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_WriteMbox(
	BHAB_Handle handle,    /* [in] BHAB PI Handle */
	uint32_t    reg,  /* [in] RBUS register address */
	uint32_t    *val  /* [in] value to write */
);

/******************************************************************************
Summary:
   Handle BHAB interrupts.
Description:
   This function must be called when the host receives an interrupt from the 
   BHAB device.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_HandleInterrupt_isr(
	BHAB_Handle handle   /* [in] BHAB handle */
);


/******************************************************************************
Summary:
   This function processes BHAB interrupt events.
Description:
   This function is used for an i2c-controlled BHAB device only.  The 
   application calls this function when a BHAB interrupt event has been 
   signalled.  This function will decode the interrupt and set approriate 
   events.  For memory-mapped BHAB devices, this function will return
   BERR_NOT_SUPPORTED.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_ProcessInterruptEvent(
	BHAB_Handle handle  /* [in] HAB device handle */
);


/******************************************************************************
Summary:
   Enables/Disables the inband channel lock interrupt.
Description:
   This function enables or disables an interrupt to be generated upon change 
   in inband channel lock status.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_EnableLockInterrupt(
	BHAB_Handle handle, /* [in] BHAB Handle */
	BHAB_DevId eDevId,    /* [in] Device ID */
	bool bEnable   /* [in] true = enable lock interrupts, false = disables lock interrupts */
);


/******************************************************************************
Summary:
   Used to install a callback for a given Interrupt.
Description:
   This function install a callback for an interrupt 
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_InstallInterruptCallback(
	BHAB_Handle handle,                /* [in] BHAB handle */
	BHAB_DevId eDevId,    /* [in] Device ID */
	BHAB_IntCallbackFunc fCallBack,    /* [in] Callback function pointer */
	void * pParm1,                     /* [in] Paramater1 for callback function*/
	int parm2                          /* [in] Paramater2 for callback function*/
); 


/******************************************************************************
Summary:
   Used to uninstall a callback for a given Interrupt.
Description:
   This function uninstall a callback for an interrupt 
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_UnInstallInterruptCallback(
	BHAB_Handle handle,  /* [in] BHAB handle */
	BHAB_DevId eDevId    /* [in] Device ID */
);


/******************************************************************************
Summary:
   This function sends an HAB command.
Description:
   This function writes a command to the HAB, asserts HABR, waits for the AP to 
   release the HAB, then optionally checks for a service acknowlegement from 
   the AP.  If bCheckForAck is true, then read_len must be greater than 0.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_SendHabCommand(
	BHAB_Handle h,      /* [in] BHAB PI Handle */
	uint8_t *write_buf, /* [in] specifies the HAB command to send */
	uint16_t write_len,  /* [in] number of bytes of the HAB command to be written using either I2c or SPI. */ 
	uint8_t *read_buf,  /* [out] holds the data read from the HAB */ 
	uint16_t read_len,   /* [in] number of bytes to read from the HAB */
	bool bCheckForAck,  /* [in] true = determine if the AP has serviced the command */
	bool bInsertTermination, /* [in] true = insert termination byte 0x00 in write buffer at read_len position */
	uint16_t command_len /* [in] Total number of bytes in the HAB command so that the termination byte is inserted at the right HAB location. */
);


/******************************************************************************
Summary:
   Returns status of the AP.
Description:
   The state of the AP and the HAB are returned in this function.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_GetApStatus(
	BHAB_Handle   handle,    /* [in] HAB device handle */
	BHAB_ApStatus *pStatus   /* [out] AP status */
);

/******************************************************************************
Summary:
   Returns status of the AP.
Description:
   The state of the AP and the HAB are returned in this function.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_GetInterruptEventHandle(
	BHAB_Handle handle,	 
	BKNI_EventHandle *event /* [out] Interrupt event handle */
);

/******************************************************************************
Summary:
   Gets the watch dog timer configuration.
   
Description:

Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_GetWatchDogTimer(BHAB_Handle h, 
	BHAB_WatchDogTimerSettings *wdtSettings);

/******************************************************************************
Summary:
   Sets the watch dog timer configuration.
   
Description:
   
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_SetWatchDogTimer(BHAB_Handle h,      /* [in] BHAB PI Handle */
	const BHAB_WatchDogTimerSettings *wdtSettings);

/******************************************************************************
Summary:
   Gets the 31xx non maskable interrupt configuration.

Description:
   
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_GetNmiConfig(BHAB_Handle h,      /* [in] BHAB PI Handle */
	BHAB_NmiSettings *nmiSettings);

/******************************************************************************
Summary:
   Sets the 31xx non maskable interrupt configuration.

Description:

Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_SetNmiConfig(BHAB_Handle h,      /* [in] BHAB PI Handle */
	const BHAB_NmiSettings *nmiSettings);

/***************************************************************************
Summary:
    This function gets HAB settings.

Description:
    This function returns the current HAB Settings.
    
Returns:
    TODO:

See Also:
    BHAB_SetConfigSettings()

****************************************************************************/
BERR_Code BHAB_GetConfigSettings(
    BHAB_Handle handle,           /* [in] Device handle */
    BHAB_ConfigSettings *settings     /* [out] HAB config settings. */
    );
    
/***************************************************************************
Summary:
    This function sets HAB settings.

Description:
    This function sets the current HAB Settings.
    
Returns:
    TODO:

See Also:
    BHAB_GetConfigSettings()
****************************************************************************/
BERR_Code BHAB_SetConfigSettings(
    BHAB_Handle handle,           /* [in] Device handle */
    const BHAB_ConfigSettings *settings     /* [in] HAB config settings. */
    );

/******************************************************************************
Summary:
   This function allows the reading from a slave device attached to the demod chip.
Description:
  
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_ReadSlave(
    BHAB_Handle handle,     /* [in] Device channel handle */
	uint8_t chipAddr,       /* [in] chip addr of the i2c slave device */
	uint32_t subAddr,       /* [in] sub addr of the register to read from the slave device */
	uint32_t *data,         /* [out] ptr to the data that we will read from the slave device */
	uint8_t dataLen         /* [in] how many bytes are we going to read?, for 3412 LNA, this should be in multiples of 4-bytes,
                                for 3128, dataLen can only be 4 bytes */
);

/******************************************************************************
Summary:
   This function allows the writing to a slave device attached to the demod chip.
Description:
  
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BHAB_WriteSlave(
    BHAB_Handle handle,     /* [in] Device channel handle */
	uint8_t chipAddr,       /* [in] chip addr of the i2c slave device */
	uint32_t subAddr,       /* [in] sub addr of the register to read from the slave device */
	uint32_t *data,         /* [out] ptr to the data that we will read from the slave device */
	uint8_t dataLen        /* [in] how many bytes are we going to read?, for 3412 LNA, this should be in multiples of 4-bytes,
                                for 3128, dataLen can only be 4 bytes */    
);
    
/******************************************************************************
Summary:
   This function retrieves the internal gain settings of a demod chip
Description:
  
Returns:
   BERR_Code
******************************************************************************/    
BERR_Code BHAB_GetInternalGain(
    BHAB_Handle handle,                                 /* [in] Device handle */
    const BHAB_InternalGainInputParams *inputParams,  /* [in] frequency */    
    BHAB_InternalGainSettings *internalGainSettings     /* [out] internal gain settings. */    
);
    
/******************************************************************************
Summary:
   This function sets the external gain settings to a demod chip
Description:
  
Returns:
   BERR_Code
******************************************************************************/     
BERR_Code BHAB_GetExternalGain(
    BHAB_Handle handle,                                   /* [in] Device handle */
    BHAB_ExternalGainSettings *externalGainSettings     /* [in] external gain settings. */
    );
    
/******************************************************************************
Summary:
   This function retrieves the external gain settings of a demod chip
Description:
  
Returns:
   BERR_Code
******************************************************************************/     
BERR_Code BHAB_SetExternalGain(
    BHAB_Handle handle,                                   /* [in] Device handle */
    const BHAB_ExternalGainSettings *externalGainSettings     /* [in] external gain settings. */
    );
        
#ifdef __cplusplus
}
#endif


#endif
