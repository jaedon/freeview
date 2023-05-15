/***************************************************************************
*     Copyright (c) 2005-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
 *
 * $brcm_Workfile: buhf.h $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 9/7/12 8:57a $
 *
 * Module Description: Porting Interface for the UHF Receiver Module.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/uhf/include/buhf.h $
 * 
 * Hydra_Software_Devel/9   9/7/12 8:57a jrubio
 * SWNOOS-563: add 7422 UHF family to if else statement
 * 
 * Hydra_Software_Devel/8   9/6/12 6:05p agin
 * SWNOOS-563:  Merge buhf from both 7401 and 7422.
 * 
 * Hydra_Software_Devel/7   11/21/11 6:21p mward
 * SW7435-7: Add 7435.
 * 
 * Hydra_Software_Devel/6   10/20/11 9:31a speter
 * SW7346-511: [7346] Add support for Echostar UHF remote
 * 
 * Hydra_Software_Devel/5   4/26/11 2:19p gautamk
 * SW7422-440: [7420] Adding support for PM in UHF for 40nm chips.
 * 
 * Hydra_Software_Devel/4   12/8/10 4:46a gautamk
 * SW7405-4713:[7344] UHF support for 7344 and 7346
 * 
 * Hydra_Software_Devel/3   9/23/10 4:01p gautamk
 * SW7425-37: [7425] Adding uhf support for 7425
 * 
 * Hydra_Software_Devel/2   9/23/10 3:55p gautamk
 * SW7422-56: [7422] modifying UHF programming
 * 
 * Hydra_Software_Devel/1   7/30/10 6:06p gautamk
 * SW7422-56: [7422] Adding buhf.h
 * 
 * Hydra_Software_Devel/25   3/26/10 4:08p gautamk
 * SW7325-705: [7325] Adding paramter to hold the satus of Interrupt.
 * 
 * Hydra_Software_Devel/24   3/17/10 5:09p gautamk
 * SW7400-2701: [7400] removing reference of customer name from file.
 * 
 * Hydra_Software_Devel/24   3/17/10 4:37p gautamk
 * SW7400-2701: [7400] removing reference of customer name from file.
 * 
 * Hydra_Software_Devel/24   3/17/10 4:35p gautamk
 * SW7400-2701: [7400] removing reference of customer name from file.
 * 
 * Hydra_Software_Devel/23   11/9/09 11:49a gmohile
 * SW7408-1 : Add 7408 support
 * 
 * Hydra_Software_Devel/22   8/10/09 4:10p jrubio
 * PR55232: add 7340/7342 support
 * 
 * Hydra_Software_Devel/21   1/31/09 1:03a jrubio
 * PR51629: add 7336 support
 * 
 * Hydra_Software_Devel/20   1/16/09 11:50a mward
 * PR39231: Eliminate compiler warning for c++ comment.
 * 
 * Hydra_Software_Devel/19   12/18/08 12:13p gautamk
 * PR47759: [7413] Adding support for 7413 UHF PI.
 * 
 * Hydra_Software_Devel/18   12/9/08 10:43a gautamk
 * PR47759: [7420] Fixing Compile error for 7420
 * 
 * Hydra_Software_Devel/17   4/29/08 11:53a rjain
 * PR41011: Making shift value programmable. NOte:
 * 1. Shift should be enabled ONLY when both UHF devices are in use
 * 2. Shift value MUST be same for both UHF devices.
 * 3. Shift value should preferably be an even number, to minimise
 * calculation errors.
 * 4. 160KHz < shift < 240 KHz
 * 
 * Hydra_Software_Devel/16   3/31/08 11:11a rjain
 * PR41011: Adding "shift_if" to fix cross channel interference.
 * 
 * Hydra_Software_Devel/15   2/1/08 5:05p rjain
 * PR 39231: Clean up comments and remove redundant code
 * 
 * Hydra_Software_Devel/14   1/23/08 5:39p rjain
 * PR 27871: removing customer names
 * 
 * Hydra_Software_Devel/13   11/28/07 4:01p farshidf
 * PR36874: Add 7335 support
 * 
 * Hydra_Software_Devel/12   10/14/07 1:45p jkim
 * PR14344: Add 7325  support
 * 
 * Hydra_Software_Devel/11   6/4/07 3:22p rjain
 * PR 27871: removing customer names
 * 
 * Hydra_Software_Devel/10   5/8/07 6:50p rjain
 * PR 26220: adding support for 7400 B0 (65nm)
 * 
 * Hydra_Software_Devel/9   5/7/07 8:19p katrep
 * PR29073: Added support for 7405 chip
 * 
 * Hydra_Software_Devel/8   12/4/06 6:13p rjain
 * PR25108: adding support for 7403 UHF PI
 * 
 * Hydra_Software_Devel/7   5/8/06 11:36a rjain
 * PR 21462 : adding support for 7400 in the UHF PI
 * 
 * Hydra_Software_Devel/6   2/10/06 4:45p rjain
 * PR 19103: adding event handler. Event gets set when data is ready in
 * advanced mode.
 * 
 * Hydra_Software_Devel/5   1/14/06 11:14p agin
 * PR19076: Support BCM7400.
 * 
 * Hydra_Software_Devel/4   10/17/05 4:26p rjain
 * PR 16923: made changes to support cust2 packets in legacy mode also.
 * 
 * Hydra_Software_Devel/3   9/29/05 3:34p rjain
 * PR 16923: We now pass preamble information to the application and in
 * the application process packet differently based on packet type &
 * preamble
 * 
 * Hydra_Software_Devel/2   9/6/05 4:20p rjain
 * PR 16923: Added code for cust2 packets.
 * 
 * Hydra_Software_Devel/1   8/30/05 4:43p rjain
 * PR 16923: Add PI for UHF reciever
 * 
 * ***************************************************************************/


#ifndef _BUHF_H_
#define _BUHF_H_


/*=************************ Module Overview ********************************

Overview
--------


***************************************************************************/

#include "bstd.h"
#include "bchp.h"
#include "breg_mem.h"
#include "bint.h"
#include "bkni.h"
#include "berr_ids.h"
#if BCHP_CHIP==7400
#include "bchp_7400.h"
#include "bchp_uhfr_2.h"
#elif BCHP_CHIP==7401
#include "bchp_7401.h"
#elif BCHP_CHIP==7403
#include "bchp_7403.h"
#elif BCHP_CHIP==7405
#include "bchp_7405.h"
#elif BCHP_CHIP==7325
#include "bchp_7325.h"
#elif BCHP_CHIP==7340
#include "bchp_7340.h"
#elif BCHP_CHIP==7342
#include "bchp_7342.h"
#elif BCHP_CHIP==7335
#include "bchp_7335.h"
#elif BCHP_CHIP==7420
#include "bchp_7420.h"
#elif BCHP_CHIP==7413
#include "bchp_7413.h"
#elif BCHP_CHIP==7408
#include "bchp_7408.h"
#elif ((BCHP_CHIP == 7422) || (BCHP_CHIP == 7425) || (BCHP_CHIP == 7344) || (BCHP_CHIP == 7346) || (BCHP_CHIP == 7435))
#define BUHF_7422_FAMILY 1
#else
#error "Not supported"
#endif

#if (BUHF_7422_FAMILY==1)
#include "bchp_uhfr.h"
#include "bchp_uhfr_intr2.h"
#include "bchp_int_id_uhfr_intr2.h"
#else
#include "bchp_uhfr_1.h"
#include "bchp_uhfr_glbl.h"
#endif
#ifdef __cplusplus
extern "C" {
#endif

    
/*{{{ Defines */


#if ((BCHP_CHIP == 7400) || (BCHP_CHIP == 7420))
#define BUHF_MAX_DEVICES  2 /* Number of UHF devices on this chip. */
#else
#define BUHF_MAX_DEVICES  1 /* Number of UHF devices on this chip. */
#endif

#define BUHF_MAX_LOG_SIZE         10    /* No. of entries to be maintained for moving average */  
#define BUHF_NUM_FILTER_COEFF     41    /* The number of filter coefficients */
#define BUHF_NUM_FILTERS          2     /* The number of filters. Only 2 at present : LPF and BPF */
#define BUHF_NUM_CHANNELS         9     /* The number of channels supported */  
#define BUHF_PR_CORR_PEAK_THRES   0xa00 /* If DC offset estimate is greater than this value, take it for averaging, else discard */
#define BUHF_FREQ_RATIO_NUM       976   /* (976 / 660) used to calcluate freqOffset from average DC level */     
#define BUHF_FREQ_RATIO_DEN       660   /* (976 / 660) used to calcluate freqOffset from average DC level */     
#define BUHF_FREQ_OFFSET_THRES    1000  /*  Frequency offset threshold */     
#define BUHF_MIN_STEP_FACTOR      5149  /* 27*divN*10**6/2**19 */
      
  
/*}}}*/

/*{{{ Typedefs */

/***************************************************************************
Summary:  
    Indicates UHFR Modes - Advanced and Legacy
    
Description: 
    Part of BUHF_Settings. Set while calling BUHF_Open(). 
    Cannot be changed later.
****************************************************************************/
typedef enum BUHF_Mode
{
    BUHF_Mode_eLegacy = 0,     /* Packet is recieved thru IR path */
    BUHF_Mode_eAdvanced        /* Packet is recieved by UHFR. */
} BUHF_Mode;

  
/***************************************************************************
Summary:   
    Lists UHFR Filter types.
    
Description: 
    Part of BUHF_Settings. Set while calling BUHF_Open(). 
    Cannot be changed later.

****************************************************************************/
typedef enum BUHF_Filter
{
    BUHF_eLPF = 0,  /* Low pass filter */
    BUHF_eBPF       /* Band pass filter */
} BUHF_Filter;


/***************************************************************************
Summary:   
    Preamble types of the recieved data packet

Description: 
    Part of BUHF_Data. Obtained on calling BUHF_Read.
    
****************************************************************************/
typedef enum BUHF_PrType
{
    BUHF_PrType_eNone = 0,   /* Dont know yet. unitialised value */
    BUHF_PrType_e1,          /* Preamble 1 */
    BUHF_PrType_e2           /* Preamble 2 */
} BUHF_PrType;


/***************************************************************************
Summary:  
    Enum for UHFR Channel numbers

Description: 
Channels 1 through 8  are defined for Cust1 UHF remote usage
Channel 9 is for Cust2 remote usage.

    Part of BUHF_Settings. Set while calling BUHF_Open(). 
    Cannot be changed later.    
****************************************************************************/
typedef enum BUHF_ChanNum
{
    BUHF_ChanNum_eChan1 = 0,    /* 369.5 Mhz Channel 1 */
    BUHF_ChanNum_eChan2,        /* 371.1 Mhz Channel 2 */
    BUHF_ChanNum_eChan3,        /* 375.3 Mhz Channel 3 */
    BUHF_ChanNum_eChan4,        /* 376.9 Mhz Channel 4 */
    BUHF_ChanNum_eChan5,        /* 388.3 Mhz Channel 5 */
    BUHF_ChanNum_eChan6,        /* 391.5 Mhz Channel 6 */
    BUHF_ChanNum_eChan7,        /* 394.3 Mhz Channel 7 */
    BUHF_ChanNum_eChan8,        /* 395.9 Mhz Channel 8 */
    BUHF_ChanNum_eChan9         /* 433.92 Mhz Channel 9 */
} BUHF_ChanNum;


/***************************************************************************
Summary:  
    Enum for injection types
    
Description: 
    Part of BUHF_Settings. Set while calling BUHF_Open(). 
    Cannot be changed later.     
****************************************************************************/
typedef enum BUHF_InjType
{
    BUHF_InjType_eLow = 0,    /* Low side injection */
    BUHF_InjType_eHigh        /* High Side injection */
} BUHF_InjType;



#if 0
/* rj: TODO: does it have to be public?? */
/***************************************************************************
Summary:  Maintains status values of different fields required for the Freq 
offset adjustment calculcations
****************************************************************************/
typedef struct BUHF_MovingAvg {
    unsigned int    logCount;       /* Current no. of entries in the log */     
    unsigned char   overflowFlag;   /* Whether log has overflown */ 
    int             prDcOffset[BUHF_MAX_LOG_SIZE];    /* Preamble DC offset: 
                                                         UHFR_COR13.dc_level */
    int             prDcOffsetSum;  /* Sum of DC offset values */
    int             prDcOffsetAvg;  /* Running Average of DC offset values */
} BUHF_MovingAvg;
#endif



/***************************************************************************
Summary:  
    Maintains status of some registers that user may need

Description:
    Status can be obtained by calling BUHF_GetStatus()
    
****************************************************************************/
typedef struct BUHF_Status {
    unsigned int bchError;    /* 0=> no BCH error. 1=> BCH error. UHFR_ISTS[23] */
    unsigned int prCorrPeak;  /* UHCOR13[31:16] */
    int          dcLevel;     /* UHCOR13[15:0] */
    int          slowRssiOut; /* UHLDAS[31:0] */
    
    /* Temporary: only for diagnostic purposes. Remove later */
    unsigned int totalPkt;        /* No. of packets entering the system */
    unsigned int errPkt;          /* No. of incoming packets that have BCH error */
    unsigned int correctedErrPkt; /* No. of packets with BCH error that have been corrected */
    uint32_t            ui32IntStatus;  /*Value of BCHP_UHFR_1_ISTS is saved into it*/                    
} BUHF_Status;

    

/***************************************************************************
Summary:  
    Options to be taken from the user at Device Open
    
Description: 
    Set while calling BUHF_Open().     
****************************************************************************/
typedef struct BUHF_Settings {
    BUHF_Mode     mode;           /* Legacy mode or Advanced mode. Default = Advanced mode. */      
    BUHF_ChanNum  channel;        /* Which channel is the UHFR currently set for. Default = Channel 1*/
    BUHF_InjType  injection;      /* High Side injection or Low side. Default = Low side injection */
    BUHF_Filter   filter;         /* LPF or BPF. Default = LPF */
    volatile bool bchErrorCorrection; /* TRUE: BCH error correction is enabled,
                                         FALSE: Disabled 
                                         Default = False */
    volatile bool bFreqAdjust;    /* TRUE: Frequency adjustment is enabled, 
                                     FALSE: Disabled 
                                     Default = False */
    volatile bool bShiftIf;       /* TRUE: Shift IF to avoid cross-channel 
                                     interference between two receivers.
                                     Can be set to 1 only for 65 nm 
                                     chips with 2 UHF recievers, when both 
                                     recievers are in use. If shift is enabled
                                     when only one UHF reciever is in use, it
                                     may result in invalid behaviour.
                                     bShiftIf is valid only for channels 1 to 8.
                                     FALSE: Disable shift IF 
                                     Default = False */
    unsigned int uiFreqShift;     /* How much to shift the frequency by. 
                                     Please note:
                                     - Used only if bShiftIf is true.
                                     - Both UHF devices MUST be given the 
                                     same shift vaule for proper functioning.
                                     - uiFreqShift should preferably be even 
                                     numbers to ensure calculations are 
                                     accurate down to +/- 1LSB 
                                     - Shift value should be between 160 & 240 KHz*/
} BUHF_Settings;   

/***************************************************************************
Summary:  The data and preamble type

Description: 
    Obtained on calling BUHF_Read()    
****************************************************************************/
typedef struct BUHF_Data {
    unsigned int        value;          /* Value of the COR12 register.
                                           Need to store the data internally 
                                           otherwise it is lost on a Soft Reset*/
    BUHF_PrType         prType;         /* The Preamble type for this data packet: 1 or 2 */

} BUHF_Data;

/***************************************************************************
Summary:  
    Handle to the UHFR device
Description: 
    Obtained on calling BUHF_Open()  
****************************************************************************/
typedef struct BUHF_P_Device * BUHF_Handle;



/*}}}*/


/*{{{ Function prototypes */

/***************************************************************************
Summary:
    Initializes & starts the UHFR Device.

Description:
    Allocates memory for the device handle, installs the UHFR interrupt 
    callback, sets up all registers etc required as per the given user settings.

Returns:
    BERR_SUCCESS - if open is successful.
    Error value  - if not successful.

See Also:
    BUHF_Close, BUHF_GetDefaultSettings, BUHF_IsDataReady, BUHF_Read
***************************************************************************/
BERR_Code BUHF_Open (
    BUHF_Handle *phUhfr,                /* [out] Allocated Device handle */
    BCHP_Handle hChip,                  /* [in] Chip handle */
    BREG_Handle hRegister,              /* [in] Register handle */
    BINT_Handle hInterrupt,             /* [in] Interrupt handle */
    unsigned int devNum,                /* [in] UHFR 0 or 1 */
    const BUHF_Settings *pSettings      /* [in] Device settings */
);


/***************************************************************************
Summary:
    Stops and shuts down the UHFR Device.

Description:
    Frees device handle and associated memory. Also uninstalls the UHFR
    interrupt handler.

Returns:
    BERR_SUCCESS - if successful.
    Error value  - if not successful.

See Also:
    BUHF_Open
***************************************************************************/
BERR_Code BUHF_Close(
    BUHF_Handle hUhfr                   /* [in] UHFR Device handle */
);




/***************************************************************************
Summary:
    Check whether the UHFR has data to read. If yes, set flag to 1, else 0
    This PI is avilable only in Advanced mode

Description:
    This function checks the UHFR intertrupt status register bits 
    ISTS_CORR_DECODE_PR1_END and ISTS_CORR_DECODE_PR2_END along with the
    intFlag in the UHFR device's uhfrConfig structure. If any of these is
    true => there is data waiting to be read.   
    
     
Returns:
    BERR_SUCCESS - if successful.
    Error value  - if not successful.

See Also:
    BUHF_Read
***************************************************************************/
BERR_Code BUHF_IsDataReady
(
    BUHF_Handle     hUhfr,      /* [in] UHFR Device handle */
    unsigned char   *pFlag      /* [out] Flag indicating data is available or not 
                                         1=> data available
                                         0=> no data available */
);


/***************************************************************************
Summary:
    Read data from the UHFR device.
    This PI is avilable only in Advanced mode

Description:
    This function gets data from the UHFR decoder. It also clears the 
    interrupt flag in UHFR configuration data structure to indicate that 
    data has been read.

    Rather than reading data from COR12, this function reads data from 
    hUhfr->data; since COR12 gets cleared on a soft reset and may loose
    data.   

Returns:
    BERR_SUCCESS - if successful.
    Error value  - if not successful.

See Also:
    BUHF_IsDataReady
***************************************************************************/
BERR_Code BUHF_Read
(
    BUHF_Handle hUhfr,              /* [in] UHFR Device handle */
    BUHF_Data   *pData              /* [out] ptr to where data is to be saved */

);


/***************************************************************************
Summary:
    Get default settings for the UHFR device. Can be called before the device
    is opened.

Returns:
    BERR_SUCCESS - if successful.
    Error value  - if not successful.

See Also:
    BUHF_Open, BUHF_GetCurrentSettings
***************************************************************************/
BERR_Code BUHF_GetDefaultSettings(
    BUHF_Settings   *pDefSettings       /* [out] Returns default settings */
);



/***************************************************************************
Summary:
    Get current settings for the UHFR device. Can be called only if the device
    is already open.

Returns:
    BERR_SUCCESS - if successful.
    Error value  - if not successful.

See Also:
    BUHF_GetDefaultSettings
***************************************************************************/

BERR_Code BUHF_GetCurrentSettings(
    BUHF_Handle     hUhfr,          /* [in] UHFR Device handle */
    BUHF_Settings   *pSettings      /* [out] Returns current settings */
);




/******************************************************************************
 Summary:
       This function returns the Status of certain registers. 
       This function is available only in Advanced mode.
 
Description:
        This PI fills in the following values in the BUHF_Status structure 
        provided by the user.
          unsigned int bchError;
          unsigned int prCorrPeak;  
          int          dcLevel;    
          int          slowRssiOut;

Returns:
    BERR_SUCCESS - if successful.
    Error value  - if not successful.

*****************************************************************************/
BERR_Code BUHF_GetStatus 
(
    BUHF_Handle   hUhfr,    /* [in] UHFR Device handle */
    BUHF_Status * pStatus   /* [out] ptr to where Status is to be saved */
);

/***************************************************************************
Summary:
    This function gets the event handle for UHF device

Description:
    This function is responsible for getting the event handle. The
    application code should use this function get UHF's event handle, 
    which the application should use to wait for data ready.  The UHF ISR will 
    set the event when it recieves valid data.
    
Returns:
    BERR_SUCCESS
****************************************************************************/
BERR_Code BUHF_GetEventHandle(
    BUHF_Handle hUhfr,          /* [in] Device  handle */
    BKNI_EventHandle *phEvent   /* [out] Returns event handle */
);

typedef struct BUHF_StandbySettings
{
    bool bEnableWakeup;   /* If true, then allows wakeup from standby using UHF. 
                             If false, the device is completely shut down */

    bool bHwKeyFiltering; /* Enables HW key filtering. If disabled, then SW key filtering must be used.
                             This boolean is ignored if bEnableWakeup is false */
                             
    uint32_t ui32KeyPatternValue; /* If bHwKeyFiltering is true then UHF will respond to this
                                     pattern value. The default pattern value corresponds to the Power 
                                     button. Different remotes will have different pattern values. */
} BUHF_StandbySettings;

/***************************************************************************
Summary:
    This function will return default Standby settings for the UHF device.

Returns:
    BERR_SUCCESS
****************************************************************************/

BERR_Code BUHF_GetDefaultStandbySettings(
    BUHF_Handle hUhf,
    BUHF_StandbySettings *pSettings
    );



/***************************************************************************
Summary:
    This function will put the UHF device in Standby mode.

Returns:
    BERR_SUCCESS
****************************************************************************/

BERR_Code BUHF_Standby(
    BUHF_Handle hUhf,
    const BUHF_StandbySettings *pSettings
    );

/***************************************************************************
Summary:
    This function takes UHF device out from Standby mode.

Returns:
    BERR_SUCCESS
****************************************************************************/
BERR_Code BUHF_Resume(BUHF_Handle hUhf);

/******************************************************************************
Summary:
      Configures to one of the 9 RF channels defined .

 Description:
      Depending on the channel selected, we set the values for UHFR Analog 
      Control Register 3 ie fRF, fLO, fVCO, fPI, vco_div, div2, rot_dir, 
      divN[7:0] and fcw[13:0]. 

 Returns:
      BERR_SUCCESS on Success
      eDeviceBadDevice if invalid device number is given
      eDeviceBadParam if invalid channel number or injection value is given
      Error value on other errors.

Returns:
    BERR_SUCCESS - if open is successful.
    Error value  - if not successful. *
*****************************************************************************/
BERR_Code BUHF_SelectRfChan
(
    BUHF_Handle     hUhfr           /* [in] UHFR Device handle */
);

/*}}}*/

#ifdef __cplusplus
}
#endif

#endif  /* _UHFRDEV_H_ */
