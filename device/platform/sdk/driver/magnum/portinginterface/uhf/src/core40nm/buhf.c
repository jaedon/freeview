/*************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: buhf.c $
 * $brcm_Revision: Hydra_Software_Devel/16 $
 * $brcm_Date: 5/24/12 4:20p $
 *
 * Module Description: Porting Interface for the UHF Receiver Module.
 * Refer http://twiki-01.broadcom.com/bin/view/Bseavsw/UHFPI
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/uhf/7422/buhf.c $
 * 
 * Hydra_Software_Devel/16   5/24/12 4:20p agin
 * SW7346-843:  Change bcm7346 UHF PI to use UHFR_LPCOEFADDR bit [7]
 * LPF_COEF_ADDR_MEM_ACC_SEL to enable auto-increment for LPF filter
 * coeff loading.
 * 
 * Hydra_Software_Devel/15   5/3/12 11:02a agin
 * SW7346-810:  Change UHF correlator threshold COR3[31:16] to 0x0940 and
 * only check BCH error for preamble type 1 packet.
 * 
 * Hydra_Software_Devel/14   3/26/12 6:32p agin
 * SW7346-739:  BUHF_P_InitReg was independently updating the interrupt
 * mask.  Rely on BINT_EnableCallback instead.
 * 
 * Hydra_Software_Devel/13   11/4/11 2:58p agin
 * SWNOOS-495:  Fixed message reporting Correctable BCH error detected,
 * when there was no BCH error to begin with.
 * 
 * Hydra_Software_Devel/12   11/4/11 11:30a agin
 * SWNOOS-495:  Removed BSTD_UNUSED() in BUHF_P_Isr.
 * 
 * Hydra_Software_Devel/11   11/4/11 11:21a agin
 * SWNOOS-495:  Make some messages BDBG_MSG instead of BDBG_ERR.
 * 
 * Hydra_Software_Devel/10   11/2/11 2:13p agin
 * SWNOOS-495:  Enable high side injection.
 * 
 * Hydra_Software_Devel/9   10/31/11 8:11p agin
 * SWNOOS-495:  Problem with UHF advanced mode, channels 1 to 8.
 * 
 * Hydra_Software_Devel/8   10/20/11 9:30a speter
 * SW7346-511: [7346] Add support for Echostar UHF remote
 * 
 * Hydra_Software_Devel/7   5/23/11 3:17p gautamk
 * SW7346-161: [7346] Implementing follwing suggestion If bit 31 is high
 * and bit 8 is high, declare checksum error; If bit 31 is low and bit 8
 * is high, good data. If bit 10 is high, declare short frame error;
 * 
 * Hydra_Software_Devel/6   5/4/11 1:56p gautamk
 * SW7346-161: [7346] If Cus2_Isr is called it means that ALT_END packet
 * was set.
 * 
 * Hydra_Software_Devel/5   5/2/11 2:09p gautamk
 * SW7346-161: [7346] Correcting programming for ANADIR and RST
 * 
 * Hydra_Software_Devel/4   4/26/11 2:19p gautamk
 * SW7422-440: [7420] Adding support for PM in UHF for 40nm chips.
 * 
 * Hydra_Software_Devel/3   9/23/10 4:01p gautamk
 * SW7425-37: [7425] Adding uhf support for 7425
 * 
 * Hydra_Software_Devel/2   9/23/10 3:55p gautamk
 * SW7422-56: [7422] modifying UHF programming
 * 
 * Hydra_Software_Devel/1   7/30/10 6:01p gautamk
 * SW7422-56: [7422] Adding buhf.c
 * 
 * Hydra_Software_Devel/67   5/27/10 1:24p gautamk
 * SW7340-172: [7340] Lowering the Error Level from ERR to WRN
 * 
 * Hydra_Software_Devel/66   4/8/10 11:32p gautamk
 * SW7325-707: [7325] For Cust2, Only 3 interrupts i.e. ALT_PACKET_END,
 * ALT_CHSM_ERR and ALT_SFR should be taken care.
 * 
 * Hydra_Software_Devel/65   4/8/10 12:19a gautamk
 * SW7325-707: [7325] Setting BCHP_UHFR_1_IMSK to 0xF0000500 for Channel9
 * 
 * Hydra_Software_Devel/64   4/7/10 7:04p gautamk
 * SW7325-707: [7325] If error interrupt is set Data packet is dropped and
 * DataReadyEvent is not raised.
 * 
 * Hydra_Software_Devel/63   4/1/10 4:22p gautamk
 * SW7405-3063:[7405] Programming change for COR1, COR2 for Cust2.
 * 
 * Hydra_Software_Devel/62   3/26/10 4:08p gautamk
 * SW7325-705: [7325] Adding paramter to hold the satus of Interrupt.
 * 
 * Hydra_Software_Devel/61   3/26/10 3:57p gautamk
 * SW7405-3063:[7405] Programming change for OR1, COR2, COR5, and COR6 for
 * Cust2.
 * 
 * Hydra_Software_Devel/60   3/18/10 3:51p gautamk
 * SW7400-2701: [7400] Correcting FCW programming.
 * 
 * Hydra_Software_Devel/59   3/17/10 4:22p gautamk
 * SW7400-2701: [7400] removing reference of customer name from file.
 * 
 * Hydra_Software_Devel/58   3/9/10 4:27p gautamk
 * SW7400-2701: [7400] Modifying Ana1, Ana2 reg . And Ana3 Table.
 * 
 * Hydra_Software_Devel/57   1/21/10 1:39p gautamk
 * SW7408-1 : Adding UHF programming for 7408
 * 
 * Hydra_Software_Devel/56   11/9/09 11:49a gmohile
 * SW7408-1 : Add 7408 support
 * 
 * Hydra_Software_Devel/55   8/10/09 5:37p jrubio
 * PR55232: 7342/7340
 * 
 * Hydra_Software_Devel/54   5/7/09 4:02p gautamk
 * PR47759: change for the cust2 pll setting
 * 
 * Hydra_Software_Devel/53   2/3/09 1:43p katrep
 * PR51619: Fixed compiler warning
 * 
 * Hydra_Software_Devel/52   2/2/09 4:02p gautamk
 * PR51619: [7420] Changing the global variables as per following rule.
 * 1) make them static
 * 2) if you cannot make them static because they must be shared between
 * multiple .c files in your module, you must rename them to include the
 * BXXX_ magnum prefix for your module.
 * 
 * Hydra_Software_Devel/51   1/31/09 1:10a jrubio
 * PR51629: add 7336 support
 * 
 * Hydra_Software_Devel/50   12/18/08 12:13p gautamk
 * PR47759: [7413] Adding support for 7413 UHF PI.
 * 
 * Hydra_Software_Devel/49   12/9/08 10:43a gautamk
 * PR47759: [7420] Fixing Compile error for 7420
 * 
 * Hydra_Software_Devel/48   10/18/08 12:23a gautamk
 * PR47759: [7420] Programming BCHP_UHFR_1_FCW and  BCHP_UHFR_1_FILT (CIC
 * Filter) for 54MHZ domain for 7420 platform.
 * 
 * Hydra_Software_Devel/47   6/5/08 3:53p rjain
 * PR43090: fixing FCW programming for cases when shift is disabled.
 * 
 * Hydra_Software_Devel/46   4/30/08 11:23a rjain
 * PR41011: fixing compilation warnings
 * 
 * Hydra_Software_Devel/45   4/29/08 1:06p rjain
 * PR41011: more changes and code clean up
 * 
 * Hydra_Software_Devel/44   4/29/08 11:53a rjain
 * PR41011: Making shift value programmable. NOte:
 * 1. Shift should be enabled ONLY when both UHF devices are in use
 * 2. Shift value MUST be same for both UHF devices.
 * 3. Shift value should preferably be an even number, to minimise
 * calculation errors.
 * 4. 160KHz < shift < 240 KHz
 * 
 * Hydra_Software_Devel/43   3/28/08 6:43p rjain
 * PR41011: Adding "shift_if" to fix cross channel interference.
 * 
 * Hydra_Software_Devel/42   3/7/08 11:27a rjain
 * PR39231: More clean up
 * 
 * Hydra_Software_Devel/41   2/1/08 5:04p rjain
 * PR 39231: Clean up comments and remove redundant code
 * 
 * Hydra_Software_Devel/40   1/23/08 4:28p rjain
 * PR 38908: The low pass filter is too narrow. Changing its coeffs
 * 
 * Hydra_Software_Devel/39   1/15/08 4:08p farshidf
 * PR36869: Change the setting for 54Mhz
 * 
 * Hydra_Software_Devel/38   12/4/07 12:02p katrep
 * PR36895: Added 7335 support.
 * 
 * Hydra_Software_Devel/37   11/13/07 9:53a mward
 * PR36207: Fix typo = s/b ==
 * 
 * Hydra_Software_Devel/36   11/13/07 12:19p rjain
 * PR 36207: 7325 uses a 27MHZ crystal. Programming ANA1 accordingly.
 * 
 * Hydra_Software_Devel/35   10/30/07 12:17p rjain
 * PR 36207: adding UHF support for 7325
 * 
 * Hydra_Software_Devel/34   10/10/07 4:25p rjain
 * PR35887: Modifying programming of ANA1 to handle 2 recievers properly
 * for 65nm and non-65nm chips
 * 
 * Hydra_Software_Devel/33   8/10/07 4:25p rjain
 * PR 33896: Fixing issue reported by Coverity
 * 
 * Hydra_Software_Devel/32   6/27/07 1:32p rjain
 * PR 32523: adding support for 7400 C0 which is  65nm chip
 * 
 * Hydra_Software_Devel/31   6/6/07 5:16p rjain
 * PR 26620: Fixing warning
 * 
 * Hydra_Software_Devel/30   6/4/07 3:22p rjain
 * PR 27871: removing customer names
 * 
 * Hydra_Software_Devel/29   6/1/07 1:52p rjain
 * PR 26620: Adding correct changes to make device 2 work.
 * 
 * Hydra_Software_Devel/28   5/31/07 5:21p rjain
 * PR 26620: undoing changes done in version 27 because the changes for 2
 * devices for 7400 B0  do not work
 * 
 * Hydra_Software_Devel/27   5/21/07 7:29p rjain
 * PR 26220: UHF device 1 and 2 share some registers. Changing those
 * regsiters so that both devices can work independently
 * 
 * Hydra_Software_Devel/26   5/10/07 2:34p rjain
 * PR 30624: 7405 has only 1 UHF device. It's at 65 nm.
 * 
 * Hydra_Software_Devel/25   5/8/07 6:50p rjain
 * PR 26220: adding support for 7400 B0 (65nm)
 * 
 * Hydra_Software_Devel/24   2/14/07 7:20p rjain
 * PR 27720: Free memory on returning with errors. Remove #if 0's and #if
 * 1's
 * 
 * Hydra_Software_Devel/23   11/28/06 3:16p rjain
 * PR 25937 : increasing BKNI_DELAY() to work on all environments.
 * 
 * Hydra_Software_Devel/22   9/26/06 10:29a agin
 * PR24562: Increase delay for setting up ana3 registers.
 * 
 * Hydra_Software_Devel/21   9/22/06 1:21p rjain
 * PR22119: Updating the hi & Low injection freq values for cust2 on
 * 7401 C0 and 7400 also
 * 
 * Hydra_Software_Devel/20   8/14/06 3:49p jrubio
 * PR22420: Took out for loop and put in BKNI_Delay. In Linux it is being
 * optimized out.
 * 
 * Hydra_Software_Devel/19   7/19/06 1:59p rjain
 * PR22119: changing values for cust2 frequencies on 7401 B0
 * 
 * Hydra_Software_Devel/18   6/12/06 10:03a rjain
 * PR 21581: removed calls to 'abs()' with calls to "ABSOLUTE_VALUE"
 * 
 * Hydra_Software_Devel/17   6/6/06 11:56a rjain
 * PR 21581: removing warnings
 * 
 * Hydra_Software_Devel/16   6/5/06 1:57p rjain
 * PR 21581: updating for UHF 2 interrupts
 * 
 * Hydra_Software_Devel/15   5/16/06 1:42p rjain
 * PR 21462: fixing release build issue
 * 
 * Hydra_Software_Devel/14   5/15/06 4:44p rjain
 * PR 21462: fixed support for UHF2 for 7400
 * 
 * Hydra_Software_Devel/13   5/10/06 11:45a rjain
 * PR 19103: set the UHF event for cust2 data also
 * 
 * Hydra_Software_Devel/12   5/8/06 12:35p rjain
 * PR 21462:  corrected typo
 * 
 * Hydra_Software_Devel/11   5/8/06 12:15p rjain
 * PR 21462: adding support for 7400 in the UHF PI
 * 
 * Hydra_Software_Devel/10   5/8/06 11:36a rjain
 * PR 21462: adding support for 7400 in the UHF PI
 * 
 * Hydra_Software_Devel/9   4/24/06 2:43p rjain
 * PR16923:  fixing compilation warnings
 * 
 * Hydra_Software_Devel/8   2/10/06 2:02p rjain
 * PR 19103: adding event handling. The event gets set when data is ready
 * in advanced mode.
 * 
 * Hydra_Software_Devel/7   10/21/05 2:10p jkim
 * PR14344: Use new include file in RDB
 * 
 * Hydra_Software_Devel/6   10/17/05 4:27p rjain
 * PR 16923: made changes to support cust2 packets in legacy mode also.
 * 
 * Hydra_Software_Devel/5   10/11/05 5:50p jkim
 * PR16923: temp fix until RDB and source is in sync
 * 
 * Hydra_Software_Devel/4   10/11/05 2:58p jkim
 * PR16923: modify according to new RDB definitions
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

#include "buhf.h"

#ifdef BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif


BDBG_MODULE (uhf);


#define ABSOLUTE_VALUE(x)    ((x) < 0 ? -(x) : (x))


/* If you want to the Frequency adjustment algo to print values,
 * use 
 *      #define UHFR_DEBUG_FREQ_ADJUSTMENT printf x 
 * else use
 *      #define UHFR_DEBUG_FREQ_ADJUSTMENT  */

#define UHFR_DEBUG_FREQ_ADJUSTMENT(x) BDBG_MSG(x)


/***************************************************************************
Summary:  Maintains status values of different fields required for the Freq 
offset adjustment calculcations
****************************************************************************/
typedef struct BUHF_P_MovingAvg {
    unsigned int    logCount;       /* Current no. of entries in the log */     
    unsigned char   overflowFlag;   /* Whether log has overflown */ 
    int             prDcOffset[BUHF_MAX_LOG_SIZE];    /* Preamble DC offset: 
                                                         UHFR_COR13.dc_level */
    int             prDcOffsetSum;  /* Sum of DC offset values */
    int             prDcOffsetAvg;  /* Running Average of DC offset values */
} BUHF_P_MovingAvg;


/* Maintains the configuration of the UHFR device */
typedef struct BUHF_P_Device {
    BCHP_Handle         hChip;
    BREG_Handle         hRegister;
    BINT_Handle         hInterrupt;
    BKNI_EventHandle    hDataReadyEvent; /* Event to be set by the UHF module when data is ready */
    BINT_CallbackHandle hCallback1; /* Callback for BCHP_INT_ID_CORR_DECODE_PR1_END */
    BINT_CallbackHandle hCallback2; /* Callback for BCHP_INT_ID_CORR_DECODE_PR2_END */
    BINT_CallbackHandle hCallback3; /* Callback for BCHP_INT_ID_ALT_PACKET_END */
    BINT_CallbackHandle hCallback4; /* Callback for BCHP_INT_ID_ALT_SFR */
    unsigned int        uiIndex;    /* UHFR device 0 or 1 */
    uint32_t            ui32Offset; /* Register offset of this UHFR device base
                                       address from UHFR 0 base address */
    BUHF_Settings       settings;   /* Current settings of the UHFR device */
   
    /* The following are used ONLY IF freqAdjust is TRUE */
    unsigned int        origFcw;        /* Original FCW in first iteration of 
                                           Freq Adjust algo */
    unsigned int        adjustedFcw;    /* New FCW in final iteration of Freq 
                                           Adjust algo ie after the freq has 
                                           converged.*/
    BUHF_P_MovingAvg      *pMovingAvg;    /* Data structure used to calculate 
                                           moving average of the DC level */
    
    /* Used internally */
    volatile bool       intFlag;        /* 1= Data waiting to be processed. */ 
    BUHF_Data           data;           /* value in the COR12 register and the preamble type */

    /* Temporary: only for diagnostic purposes. Remove later */
    unsigned int        totalPkt;       /* No. of packets entering the system */
    unsigned int        errPkt;         /* No. of incoming packets that have
                                           BCH error */
    unsigned int        correctedErrPkt;    /* No. of packets with BCH error 
                                               that have been corrected */
    uint32_t            ui32IntStatus;  /*Value of UHFR_INTR2_CPU_STATUS is saved into it*/                             
    bool                bStandbyState;  /* True if in Standby mode, false if in normal mode */
    bool                bFullPowerdown; /* True if the device is completely powered down */                                               
} BUHF_P_Device;



/* The interrupt handler checks which UHFR intrpt has been received and services
 * it accordingly. If an input key has been received, it calls the fsk input
 * handler.
 */
static void BUHF_P_Isr
(
    void * hUhfr ,      /* [in] Handle to UHFR device */
    int    iParam2      /* Not used */
    
);

static void BUHF_P_Cust2Isr1
(
    void * hUhfr ,      /* [in] Handle to UHFR device */
    int    iParam2      /* Not used */
    
);

static void BUHF_P_Cust2Isr2
(
    void * hUhfr ,      /* [in] Handle to UHFR device */
    int    iParam2      /* Not used */
    
);

/*******************************************************************************
*
*   Default Module Settings
*
*******************************************************************************/
static const BUHF_Settings defUhfrSettings = 
{
    BUHF_Mode_eAdvanced,     /* mode = Advanced */      
    BUHF_ChanNum_eChan1,     /* channel = BUHF_ChanNum_eChan1; */
    BUHF_InjType_eLow,       /* injection = Low side */
    BUHF_eLPF,               /* filter = BUHF_eLPF */
    false,                   /* BCH error correction is disabled */
    false,                   /* Frequency adjustment is disabled */
    false,                   /* Shift IF is disabled */
    0                        /* No shift */
};

static const BUHF_StandbySettings defStandbySettings = 
{
    true,       /* bEnableWakeup */
    true,       /* bHwKeyFiltering */
    0x17700BB8  /* ui32KeyPatternValue */
};


/* Low pass filter coeffs for 107 taps for Cust2 ie Channel 9 */
static unsigned int lowPassFilterCoeffCust2[BUHF_NUM_FILTER_COEFF] = {
           0x00010000,    
           0xFFFF0000,
           0x00030000,
           0xFFFB0000,
           0x00080000,
           0xFFF30000,
           0x00130000,
           0xFFE50000,
           0x00250000,
           0xFFCF0000,
           0x00420000,
           0xFFAB0000,
           0x006E0000,
           0xFF740000,
           0x00B00000,
           0xFF240000,
           0x01110000,
           0xFEAF0000,
           0x01A00000,
           0xFDFD0000,
           0x02810000,
           0xFCD80000,
           0x04110000,
           0xFA8B0000,
           0x07E20000,
           0xF2940000,
           0x28B04000,
           0x00000000,
           0x00000000,
           0x00000000,
           0x00000000,
           0x00000000,
           0x00000000,
           0x00000000,
           0x00000000,
           0x00000000,
           0x00000000,
           0x00000000,
           0x00000000,
           0x00000000,
           0x00000000
};

/* Low pass filter coeffs for 107 taps for Cust1 ie Channel 1-8 */
static unsigned int lowPassFilterCoeffCust1[BUHF_NUM_FILTER_COEFF] = {
           0xFFFE0004,
           0x00050000,
           0xFFF9FFFA,
           0x0007000E,
           0x0000FFEC,
           0xFFF20010,
           0x001F0000,
           0xFFD7FFE3,
           0x0021003D,
           0x0000FFB2,
           0xFFCA003C,
           0x006D0000,
           0xFF7AFFA4,
           0x006500B6,
           0x0000FF23,
           0xFF6900A5,
           0x01270000,
           0xFE9DFF0E,
           0x010901D9,
           0x0000FDC0,
           0xFE7401B6,
           0x031B0000,
           0xFC0BFD33,
           0x033F0639,
           0x0000F679,
           0xF8170BEB,
           0x26B53334,
           0x00000000,
           0x00000000,
           0x00000000,
           0x00000000,
           0x00000000,
           0x00000000,
           0x00000000,
           0x00000000,
           0x00000000,
           0x00000000,
           0x00000000,
           0x00000000,
           0x00000000,
           0x00000000
};

/* Tables with values of various registers depending on Frequency
 ANA 1 values for different frequencies = 0x40402204 for all freq*/
    
   #define BUHF_ANA0_VALUE 0x0000003f
   #define BUHF_ANA1_VALUE 0x00040000
   #define BUHF_ANA3_VALUE 0x01ba0000
   #define BUHF_ANA4_VALUE 0x00800000
   #define BUHF_ANADIR_VALUE 0x00002789

/* ANA 2 values for different frequencies */ /*change for the cust2 pll setting*/
static unsigned int ana2Table[2][BUHF_NUM_CHANNELS] = {
                             /* Values for low side injection */
                             {0x01a93E94, /* 358.8 Mhz VCO 1*/ 
                              0x01ab2408 , /* 360.4 Mhz VCO 2*/
                              0x01b01E57 , /* 364.6 Mhz VCO 3*/
                              0x01b203cb , /* 366.2 Mhz VCO 4*/
                              0x01bF86a4 , /* 377.6 Mhz VCO 5*/
                              0x01c3518a , /* 380.8 Mhz VCO 6*/
                              0x01c6a315 , /* 383.6 Mhz VCO 7*/
                              0x01c88889 , /* 385.2 Mhz VCO 8*/
                              0x01f59815   /* 423.22 Mhz : for 
Cust2*/                           
                              },
                             {0x01c29B7F , /* 380.2 Mhz VCO 1*/  /* Values for high side injection */
                              0x01c480F2 , /* 381.8 Mhz VCO 2*/
                              0x01c97B42 , /* 386.0 Mhz VCO 3*/
                              0x01cB60B6 , /* 387.6 Mhz VCO 4*/
                              0x01d8E38E , /* 399.0 Mhz VCO 5*/
                              0x01dCAE75 , /* 402.2 Mhz VCO 6*/
                              0x01e00000 , /* 405.0 Mhz VCO 7*/
                              0x01e1E573 , /* 406.6 Mhz VCO 8*/ 
                              0x020EF500   /* 444.62 Mhz : For Cust2 */
                             }
                           };

/* BCH Error Correction related macros and arrays */
#define GENERATOR_CODE (0x00000769)
#define CRC_MASK (0x000003ff)
#define CRC_SIZE (10)
#define KEY_DATA_MASK (0x03FFFC00)
#define KEY_DATA_SIZE (16)

static const unsigned long syndrome_table[] = {
0x00000000, 0x00000001, 0x00000002, 0x00000003, 0x00000004, 
0x00000005, 0x00000006, 0x00000000, 0x00000008, 0x00000009, 
0x0000000a, 0x00000000, 0x0000000c, 0x00000000, 0x00000000, 
0x00000000, 0x00000010, 0x00000011, 0x00000012, 0x00480000, 
0x00000014, 0x00000000, 0x00000000, 0x00000000, 0x00000018, 
0x01000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00001400, 0x00000020, 0x00000021, 0x00000022, 
0x00000000, 0x00000024, 0x00000000, 0x00900000, 0x01002000, 
0x00000028, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x02000400, 0x00000000, 0x00000000, 0x00000030, 0x00000000, 
0x02001000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00002800, 0x00000000, 0x00000040, 
0x00000041, 0x00000042, 0x00000000, 0x00000044, 0x00000000, 
0x00000000, 0x00000000, 0x00000048, 0x00000000, 0x00000000, 
0x00000000, 0x01200000, 0x00000000, 0x02004000, 0x00000000, 
0x00000050, 0x00800080, 0x00000000, 0x00040200, 0x00000000, 
0x00200800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000060, 0x00000000, 0x00000000, 0x00004400, 
0x00000000, 0x00000000, 0x00000000, 0x00030000, 0x00000000, 
0x00000000, 0x00000000, 0x00202000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00100080, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00005000, 
0x00008100, 0x00000000, 0x00000000, 0x00000080, 0x00000081, 
0x00000082, 0x00000000, 0x00000084, 0x00002100, 0x00000000, 
0x00000000, 0x00000088, 0x00000000, 0x00000000, 0x02080000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000090, 
0x00800040, 0x00000000, 0x00208000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x02400000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00020200, 0x00000000, 0x00000000, 
0x000000a0, 0x00000000, 0x01000100, 0x00000000, 0x00000000, 
0x00000000, 0x00080400, 0x00000000, 0x00000000, 0x00050000, 
0x00401000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00400400, 0x00000000, 0x00100040, 0x00000000, 
0x00081000, 0x00000000, 0x00000900, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x000000c0, 0x00800010, 0x00000000, 
0x00000000, 0x00000000, 0x00084000, 0x00008800, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00060000, 0x00000000, 0x00800001, 0x00800000, 
0x00000000, 0x00800002, 0x00000000, 0x00800004, 0x00404000, 
0x00100020, 0x00000000, 0x00800008, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x01008000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00100010, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00200100, 0x00000000, 
0x00000000, 0x00800020, 0x00000000, 0x00100004, 0x00000000, 
0x00100002, 0x00100001, 0x00100000, 0x0000a000, 0x00000000, 
0x00010200, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00100008, 0x00000100, 0x00000101, 0x00000102, 0x00000000, 
0x00000104, 0x00002080, 0x00000000, 0x00000000, 0x00000108, 
0x00000000, 0x00004200, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000110, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x02040000, 
0x00000000, 0x00300000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000120, 0x00000000, 
0x01000080, 0x00000000, 0x00000000, 0x00041000, 0x00410000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00090000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00040400, 
0x00000880, 0x00000000, 0x00008040, 0x00000000, 0x00a00000, 
0x00000140, 0x00420000, 0x00000000, 0x00000000, 0x02000200, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00100800, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x000a0000, 0x00000000, 
0x00802000, 0x01100000, 0x00000000, 0x00000000, 0x00000000, 
0x00044000, 0x00000000, 0x00000000, 0x00000000, 0x00008020, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000600, 0x00800800, 0x00000000, 0x00000000, 
0x00008010, 0x00200080, 0x00000000, 0x00000000, 0x00000000, 
0x00102000, 0x01800000, 0x00000000, 0x00008008, 0x00001200, 
0x00000000, 0x00000000, 0x00008004, 0x00000000, 0x00000000, 
0x00008001, 0x00008000, 0x00000000, 0x00008002, 0x00000180, 
0x00002004, 0x01000020, 0x00000000, 0x00002001, 0x00002000, 
0x00000000, 0x00002002, 0x00000000, 0x00000000, 0x00108000, 
0x00000000, 0x00011000, 0x00002008, 0x00000000, 0x00440000, 
0x00000000, 0x00000000, 0x00000000, 0x00010400, 0x00000000, 
0x00002010, 0x00000000, 0x00024000, 0x00000000, 0x00000000, 
0x00000000, 0x00000820, 0x000c0000, 0x00000000, 0x00000000, 
0x00000000, 0x01000002, 0x00000000, 0x01000000, 0x01000001, 
0x00000000, 0x00002020, 0x01000004, 0x00000000, 0x00000000, 
0x00000000, 0x01000008, 0x00000810, 0x00808000, 0x00000000, 
0x00200040, 0x00000000, 0x00000000, 0x00000000, 0x01000010, 
0x00000808, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000802, 0x00000801, 0x00000800, 0x00000000, 
0x00000000, 0x02010000, 0x00000804, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00002040, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00200020, 0x00080200, 0x00000000, 
0x00800100, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x02020000, 0x00000000, 
0x00000000, 0x00400200, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x01000040, 0x00000000, 0x00000000, 
0x00000000, 0x00200008, 0x00000000, 0x00000000, 0x00000000, 
0x00200004, 0x00021000, 0x00200002, 0x00000000, 0x00200000, 
0x00200001, 0x00014000, 0x00000000, 0x00000000, 0x00000000, 
0x00020400, 0x00000000, 0x00000000, 0x00100100, 0x00000000, 
0x00000000, 0x00000000, 0x00000840, 0x00000000, 0x00008080, 
0x00200010, 0x00000000, 0x00000200, 0x00000201, 0x00000202, 
0x00000000, 0x00000204, 0x00000000, 0x00000000, 0x00000000, 
0x00000208, 0x00000000, 0x00004100, 0x00009000, 0x00000000, 
0x00110000, 0x00000000, 0x00000000, 0x00000210, 0x00000000, 
0x00000000, 0x00040040, 0x00008400, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00020080, 0x00000000, 0x00000000, 0x00000220, 
0x00280000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00810000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00600000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x02008000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000240, 0x00000000, 0x00000000, 0x00040010, 
0x02000100, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00082000, 0x00000000, 0x00820000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00040002, 0x00040001, 
0x00040000, 0x00000000, 0x00000000, 0x00000000, 0x00040004, 
0x00000000, 0x00402000, 0x00000000, 0x00040008, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00400800, 0x00000000, 0x00000500, 0x00120000, 0x00000000, 
0x00000000, 0x01080000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00040020, 0x00080800, 0x00000000, 
0x00001100, 0x0000c000, 0x00000000, 0x00000000, 0x00010080, 
0x00000000, 0x00000000, 0x00000000, 0x01400000, 0x00000000, 
0x00000280, 0x00000000, 0x00840000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00200400, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00020010, 0x00000000, 
0x00006000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00020008, 0x00000000, 0x00000000, 0x00201000, 
0x00020004, 0x00000000, 0x00000000, 0x00020001, 0x00020000, 
0x00000000, 0x00020002, 0x00000000, 0x00408000, 0x00000000, 
0x00000000, 0x00140000, 0x00000000, 0x00000000, 0x00000000, 
0x01004000, 0x00000000, 0x02200000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00004800, 
0x00088000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00010040, 0x00000000, 
0x00000000, 0x00020020, 0x00000000, 0x00000000, 0x00000000, 
0x02002000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x01000400, 0x00000000, 0x00001800, 0x00000000, 0x00080100, 
0x00000000, 0x00800200, 0x00000c00, 0x00040080, 0x01001000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00010020, 0x00000000, 0x00400100, 0x00020040, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00204000, 0x00000000, 0x03000000, 0x00000000, 0x00000000, 
0x00000000, 0x00010010, 0x00000000, 0x00002400, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00010008, 
0x00003000, 0x00000000, 0x00000000, 0x00000000, 0x00100200, 
0x00010002, 0x00000000, 0x00010000, 0x00010001, 0x00000000, 
0x00000000, 0x00010004, 0x02000800, 0x00000300, 0x00000000, 
0x00004008, 0x00000000, 0x02000040, 0x00000000, 0x00000000, 
0x00000000, 0x00004002, 0x00000000, 0x00004000, 0x00004001, 
0x00000000, 0x00c00000, 0x00004004, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00210000, 0x00000000, 
0x00000000, 0x00000000, 0x00022000, 0x00000000, 0x00004010, 
0x00000000, 0x00000000, 0x00000000, 0x00880000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00020800, 0x00000000, 0x00000000, 0x00000440, 
0x00004020, 0x00500000, 0x00000000, 0x00000000, 0x00048000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00001040, 0x00000000, 0x00180000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x01020000, 0x02000004, 0x00010800, 0x00000000, 
0x00000000, 0x02000000, 0x02000001, 0x02000002, 0x00000000, 
0x00000000, 0x00000420, 0x00004040, 0x00000000, 0x02000008, 
0x00000000, 0x00000000, 0x00080080, 0x00000000, 0x00000000, 
0x00000000, 0x00040100, 0x02000010, 0x00000000, 0x00001020, 
0x00000000, 0x01010000, 0x00000000, 0x00000000, 0x00000000, 
0x00400080, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000408, 0x00000000, 0x00000000, 0x02000020, 0x00000000, 
0x00001010, 0x00000000, 0x00000401, 0x00000400, 0x00000000, 
0x00000402, 0x00000000, 0x00000404, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00001004, 0x00220000, 0x00001002, 
0x00000000, 0x00001000, 0x00001001, 0x00000000, 0x00000410, 
0x00000000, 0x00000000, 0x00000000, 0x00008200, 0x00001008, 
0x00012000, 0x00000000, 0x00101000, 0x00000000, 0x00000000, 
0x00000000, 0x00002200, 0x00000000, 0x00018000, 0x00000000, 
0x00000000, 0x00004080, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00080040, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x02800000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00400040, 
0x00020100, 0x00100400, 0x00000000, 0x00000000, 0x00000000, 
0x01000200, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00801000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x02100000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00800400, 0x00000000, 0x00000000, 
0x00000a00, 0x00000000, 0x00240000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x02000080, 
0x00000000, 0x00000000, 0x00080008, 0x00000000, 0x00000000, 
0x00000000, 0x00080004, 0x00400010, 0x00080002, 0x00080001, 
0x00080000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00400008, 0x00000000, 0x00042000, 0x00000000, 0x00400004, 
0x00000000, 0x00000000, 0x00804000, 0x00400000, 0x00400001, 
0x00400002, 0x00080010, 0x00028000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00040800, 0x00000480, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00200200, 0x00080020, 0x00000000, 0x01040000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00001080, 
0x00000000, 0x00000000, 0x00000000, 0x00010100, 0x00000000, 
0x00400020, 0x00104000};

#define SYNDROME_TABLE_SIZE (sizeof(syndrome_table) / 4)

static BERR_Code BUHF_P_BchErrorCorrection (BUHF_Handle hUhfr, unsigned int * packet);

/* BCH Error Correction related additions - end */

/* Static functions ie local functions not visible to user */
static BERR_Code BUHF_P_InitReg (BUHF_Handle hUhfr);
static BERR_Code BUHF_P_SetFilterCoeff (BUHF_Handle hUhfr, BUHF_Filter filter);

/******************************************************************************
 * Summary: 
 *      Sets the filter coefficients.        
 *
 * Input Parmameters:
 *      BUHF_Handle hUhfr : Pointer to UHFR device 
 *      BUHF_Filter filter   : Which filter to select.  
 *                             Values = BUHF_eLPF or BUHF_eBPF
 *
 * Returns:
 *      BERR_SUCCESS on Success
 *      Error value on failure.
 *
 * Implementation:
 *      Filter coefficients for Low Pass and High Pass filter
 *      are taken from the array filterCoeff[][].
 *
 *****************************************************************************/
static BERR_Code BUHF_P_SetFilterCoeff 
(
    BUHF_Handle hUhfr,   
    BUHF_Filter filter
)
{
    BERR_Code ret = BERR_SUCCESS;
    uint32_t val;
    unsigned int i;

    /* Check parameters passed to this function */
    BDBG_ASSERT(hUhfr);

    hUhfr->settings.filter = filter;
    
    /* Enable auto increment */
    val = BCHP_UHFR_LPCOEFADDR_LPF_COEF_ADDR_MEM_ACC_SEL_MASK; 
    BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_LPCOEFADDR + hUhfr->ui32Offset), val);         

    if (filter == BUHF_eLPF) 
    {
        if (hUhfr->settings.channel == BUHF_ChanNum_eChan9)
        { /* Load LPF coeffs for Cust 2 */
            for (i=0; i<BUHF_NUM_FILTER_COEFF; i ++) 
            {
                val = lowPassFilterCoeffCust2[i]; 
                BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COEF + hUhfr->ui32Offset), val);            
            }
        }        
        else 
        {   /* Load LPF coeffs for Channels 1- 8 ie Cust 1 */
            for (i=0; i<BUHF_NUM_FILTER_COEFF; i ++) 
            {
                val = lowPassFilterCoeffCust1[i]; 
                BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COEF + hUhfr->ui32Offset), val);            
            }
        }
    }
    else if (filter == BUHF_eBPF)
    { /* For Band Pass Filter */
        BDBG_ERR(("BandPass Filter is not supported"));
    }   
    val = BREG_Read32(hUhfr->hRegister, (BCHP_UHFR_LPCOEFADDR + hUhfr->ui32Offset));
    val &=~BCHP_UHFR_LPCOEFADDR_LPF_COEF_ADDR_MEM_ACC_SEL_MASK;
    BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_LPCOEFADDR + hUhfr->ui32Offset), val);
    return ret;
}

/******************************************************************************
 * static BERR_Code BUHF_P_InitReg (BUHF_Handle hUhfr, BUHF_Mode mode)
 *
 * Function:
 *      Initializes all UHFR registers according to mode of 
 *      operation (Legacy or Advanced)
 *
 * Input Parmameters:
 *      SBcmDevice *pDevice: Pointer to UHFR device
 *
 * Returns:
 *      BERR_SUCCESS on Success
 *      Error on failure
 *
 * Implementation:
 *      ANA3 is not set here. To set it, BUHF_SelectRfChan() has to be called 
 *      explictly by the user. 
 *      In Legacy mode, disable all UHFR intrpts. 
 *      In Advanced mode, enable only BCHP_UHFR_INTR2_CPU_STATUS_CORR_BCH_ERR_MASK,
 *      CORR_DECODE_PR1_END_MASK and CORR_DECODE_PR2_END_MASK 
 *
 *****************************************************************************/
static BERR_Code BUHF_P_InitReg (
        BUHF_Handle hUhfr
)
{
    BERR_Code ret = BERR_SUCCESS;
    unsigned int val;
    unsigned int offset = 0,uiRegval=0;
    offset = hUhfr->ui32Offset;

    /* Check parameters passed to this function */
    BDBG_ASSERT(hUhfr);     

    BDBG_MSG(("BUHF_P_InitReg"));
#ifndef BCHP_PWR_RESOURCE_UHF_INPUT
        /* always program BCHP_UHFR_AUTOPD1 for UHF1 or UHF2. only needed if BCHP_PWR-based PM is not enabled */
    BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_AUTOPD1), 0x00000000);         
#endif
    
    /* reset all blocks */
    BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_RST + offset), 0xffffffff);         
    BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_RST + offset), 0);             

    /* 7420 are decimated by 108 */
    BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_FILT + offset), 0x006a0104);   
    val = (hUhfr->settings.channel == BUHF_ChanNum_eChan9)? 0x00cd462a: 0x0032b9d6; /* ~(0x0032b9d6)+1 = FFCD462A  , But programming 0x00CD462A*/

    if ((hUhfr->settings.bShiftIf==true) && (hUhfr->settings.channel != BUHF_ChanNum_eChan9))
    {  /* If SHIFT IF is selected, use different FCW values for UHF1 and UHF 2 */
        if (hUhfr->uiIndex == 0)  
        {
            val = 0x00322833; /*in 81 MHZ domain its value is 0x00217022 And 81*(0x00217022/2^24)=10.58MHz . 
                                        So in 54Mhz domain (10.58/54)*2^24=0x00322833*/
        }
        else if (hUhfr->uiIndex == 1)  
        {
            val = 0x00334B78; /*in 81 MHZ domain its value is 0x00223251 And 81*(0x00223251/2^24)=10.58MHz . 
                                        So in 54Mhz domain (10.58/54)*2^24=0x00334B78*/
        }
    }

    BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_FCW + offset), val);         

    BUHF_P_SetFilterCoeff (hUhfr, hUhfr->settings.filter); 

    val = (hUhfr->settings.mode == BUHF_Mode_eLegacy)? 0x00010a04: 0x00060a05;
    BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_IRPH + offset), val);         
    BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_IRDC + offset), 0x260b0f0a);         
    BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_FRZMM + offset), 0x02020202);         
    
    val = (hUhfr->settings.mode == BUHF_Mode_eLegacy)? 0x00100100: 0x00000100;
    BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_FRZDC + offset), val);         
    BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_DCINT+ offset), 0x00000000);         

    val = (hUhfr->settings.mode == BUHF_Mode_eLegacy)? 0xf2010b0a: 0xf2010c0a;
    BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_PDAS + offset), val);         

    val = (hUhfr->settings.mode == BUHF_Mode_eLegacy)? 0x00048000: 0x00010000;
    BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_THRAS + offset), val);         
    BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_THHYS + offset), 0x00000020);         

    if ((hUhfr->settings.channel == BUHF_ChanNum_eChan9))
    {
        /* For Cust2 in Legacy/Advanced Mode*/
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_ALTCFG + offset), 0x20f40160);         
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_ALTCFG2 + offset), 0xff10785f);         
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR1 + offset), 0x005b0c8e);       
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR4 + offset), 0x00000d6a);         
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR5 + offset), 0x0001ff6b);         
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR6 + offset), 0x0001ff6b);  
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR7 + offset), 0x003FD879);         
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR8 + offset), 0);         
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR9 + offset), 0);         
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR10 + offset), 0x00001919);         
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR16 + offset), 0x000FFFFF);         
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR11 + offset), 0x0708010F);         
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR3 + offset), 0x0c001f00);         
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR2 + offset), 0x00140801);  
        
        uiRegval = BREG_Read32 (hUhfr->hRegister, (BCHP_UHFR_INTR2_CPU_MASK_STATUS + offset));            
        BDBG_MSG(("BCHP_UHFR_INTR2_CPU_MASK_STATUS=%d",uiRegval));   
    }

    else if (hUhfr->settings.mode == BUHF_Mode_eAdvanced)
    {
        /* For Cust1 in Advanced mode */
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR1 + offset), 0x00000D8e);         
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR4 + offset), 0x003FD861);         
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR5 + offset), 0x0000FF7B);         
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR6 + offset), 0x0000FFE7);         
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR7 + offset), 0x003FD879);         
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR8 + offset), 0x00007fdf);         
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR9 + offset), 0x00003FFF);         
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR10 + offset), 0x19191919);         
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR16 + offset), 0x000FFFFF);         
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR11 + offset), 0x0708010F);         
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR3 + offset), 0x09401f00);         
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR2 + offset), 0x00151400);     

        uiRegval = BREG_Read32 (hUhfr->hRegister, (BCHP_UHFR_INTR2_CPU_MASK_STATUS + offset));            
        BDBG_MSG(("BCHP_UHFR_INTR2_CPU_MASK_STATUS=%d",uiRegval));   
    }

    BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_CTL1 + offset), 0x00000401);         
    BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_RST + offset), 0x00010000);         
    BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_RST + offset), 0);                 

    if (hUhfr->settings.mode == BUHF_Mode_eLegacy)
    {
        /* For Cust1/Cust2 in legacy mode */
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR5 + offset), 0x003FFF7B);         
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR8 + offset), 0x000FFFFF);         
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR3 + offset), 0x26000E07);         
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_INTR2_CPU_MASK_SET + offset), 0xFFFFFFFF);    
        uiRegval = BREG_Read32 (hUhfr->hRegister, (BCHP_UHFR_INTR2_CPU_MASK_STATUS + offset));            
        BDBG_MSG(("BCHP_UHFR_INTR2_CPU_MASK_STATUS=%d",uiRegval));            
    }
    else
    {
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_CTL1 + offset), 0x40000401);         
    }

    if(hUhfr->settings.channel == BUHF_ChanNum_eChan9)
    {
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_AUTOPD4),0x1E240107);        
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_AUTOPD3),0x20D01068);        
    }
    else
    {
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_AUTOPD4),0x0880DCC0);
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_AUTOPD3),0x17700BB8);                
    } 
    val = BREG_Read32 (hUhfr->hRegister,  (BCHP_UHFR_AUTOPD5));
    val &= ~( (BCHP_MASK (UHFR_AUTOPD5, REPEAT_PKT_TIMEOUT)));
    val |= (BCHP_FIELD_DATA (UHFR_AUTOPD5, REPEAT_PKT_TIMEOUT,0xFA0));                         
    BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_AUTOPD5), val );       

    return ret;  
}  
    
/* For BCH error Correction.  */
/*
 *  This function strips the transmitted 10 bit
 *  crc off the 26 bit packet and replaces the crc with zeros.  
 *  The new crc is calculated and compared with the transmitted crc.  
 *  If the crc's don't match, bit errors have occured.  Using the 
 *  difference between the crc's and the syndrome look up table, up
 *  to two bit errors can be corrected.  After an attempt to correct
 *  errors has been made, the crc will be calculated again to verify
 *  the packet has been repaired properly.
 *  
 *
 *  \param input - 26 bit fsk packet
 *
 *  \return  eDeviceExtError for error, BERR_SUCCESS for OK
 */
static BERR_Code BUHF_P_BchErrorCorrection 
(   
    BUHF_Handle hUhfr, 
    unsigned int *packet
)
{
    unsigned long calc_crc;
    unsigned long syndrome;
    unsigned long generator = GENERATOR_CODE << KEY_DATA_SIZE;
    int the_index = KEY_DATA_SIZE + CRC_SIZE;
    unsigned int temp_pkt;
    unsigned int offset=0, mask=0, regVal=0, shift=0;
       
    BDBG_ASSERT(hUhfr);
    offset = hUhfr->ui32Offset;
    
    temp_pkt = *packet;
    
    /* Get syndrome from COR14 */
    regVal = BREG_Read32 (hUhfr->hRegister,  (BCHP_UHFR_COR14+offset));
    mask = BCHP_UHFR_COR14_bch_synd_MASK;
    shift = BCHP_UHFR_COR14_bch_synd_SHIFT; 
    syndrome = (regVal & mask) >> shift ; 

    if (syndrome) /* error detected */
    {

        hUhfr->errPkt ++;
        
        if (syndrome < SYNDROME_TABLE_SIZE)
        {
            /* Right shift packet by 6 */
            temp_pkt = ((temp_pkt) >> 6);
      
            temp_pkt ^= syndrome_table[syndrome]; /* attempt repair */
         
            the_index = KEY_DATA_SIZE + CRC_SIZE;
            generator = GENERATOR_CODE << KEY_DATA_SIZE;
            calc_crc = temp_pkt;    /* Don't strip off transmitted crc */

            /* re - calculate crc - result should be zero if packet is fixed */
            while (the_index >= CRC_SIZE)    
            {
                if (calc_crc & (1 << the_index)) 
                {
                    calc_crc ^= generator;
                }
                generator >>= 1;
                the_index--;
            }
         
            if (calc_crc &= CRC_MASK)
            {
                BDBG_ERR(("BUHF_P_BchErrorCorrection: BCH error corrections failed.\n"));
                return BERR_TRACE(BERR_UNKNOWN);
            }
            else
            {   
                /* Left shift packet by 6 */
                temp_pkt = ((temp_pkt) << 6);              

                /* Copy back to orignal packet memory */
                *packet = temp_pkt;
        
                hUhfr->correctedErrPkt ++;
                
                BDBG_MSG(("BUHF_P_BchErrorCorrection: BCH error corrected.\n"));
                return BERR_SUCCESS;
            }
        
        }
        BDBG_ERR(("BUHF_P_BchErrorCorrection: BCH error corrections failed. syndrome > SYNDROME_TABLE_SIZE \n"));
        return BERR_TRACE(BERR_UNKNOWN);
   }
   return BERR_SUCCESS;
}

BERR_Code BUHF_Open(
    BUHF_Handle *phUhfr,                /* [out] Returns Device handle */
    BCHP_Handle hChip,                  /* [in] Chip handle */
    BREG_Handle hRegister,              /* [in] Register handle */
    BINT_Handle hInterrupt,             /* [in] Interrupt handle */
    unsigned int devNum,                /* [in] UHFR no. 0 or 1 */
    const BUHF_Settings *pSettings      /* [in] Device settings */
)
{
    BERR_Code ret = BERR_SUCCESS;
    unsigned int i;
    BUHF_Handle hUhfr;    

    /* Check parameters passed to this function */
    BDBG_ASSERT(phUhfr);
    BDBG_ASSERT(hChip);
    BDBG_ASSERT(hRegister);
    BDBG_ASSERT(hInterrupt);
    BDBG_ASSERT(pSettings);

    BDBG_MSG(("devNum=%d", devNum));
    BDBG_MSG(("mode=%d", pSettings->mode));
    BDBG_MSG(("channel=%d", pSettings->channel));
    BDBG_MSG(("injection=%d", pSettings->injection));
    BDBG_MSG(("filter=%d", pSettings->filter));
    BDBG_MSG(("bchErrorCorrection=%d", pSettings->bchErrorCorrection));
    BDBG_MSG(("bFreqAdjust=%d", pSettings->bFreqAdjust));
    BDBG_MSG(("bShiftIf=%d", pSettings->bShiftIf));
    BDBG_MSG(("uiFreqShift=%d", pSettings->uiFreqShift));
    
    /* Make sure device number is valid, and get offset */
    if (devNum >= BUHF_MAX_DEVICES) 
    {
        BDBG_ERR(("BUHF_Open: Invalid Device number.\n"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Shift IF can be enabled only for 65nm, Dual UHFR chips. */
#if !((BUHF_MAX_DEVICES==2))
    if (pSettings->bShiftIf == true)
    {
        BDBG_ERR(("%d is not a 65nm chip with 2 UHF devices. Please disable shift IF. ", BCHP_CHIP));
        return BERR_INVALID_PARAMETER;        
    }
#endif
    if ((pSettings->bShiftIf == true) && (pSettings->channel==BUHF_ChanNum_eChan9))
    {
        BDBG_ERR(("Shift IF is not valid for Channel9. Please disable shift IF. "));
        return BERR_INVALID_PARAMETER;        
    }

    /* Allocate the memory for the Device handle */
    hUhfr = (BUHF_Handle) BKNI_Malloc( sizeof(BUHF_P_Device));
    if(hUhfr == NULL)
    {
        ret = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        return ret;
    }
    /* Reset the entire structure */
    BKNI_Memset (hUhfr, 0, sizeof(BUHF_P_Device));
    *phUhfr = hUhfr;
  
    /* Initialise UHFR Handle */
    hUhfr->hChip = hChip;
    hUhfr->hRegister = hRegister;
    hUhfr->hInterrupt = hInterrupt;
    hUhfr->settings = *pSettings;
    hUhfr->uiIndex = devNum;
    hUhfr->bStandbyState = false;    
    hUhfr->bFullPowerdown = false;           

#if (BUHF_MAX_DEVICES > 1)
    /* If htere are more than 1 UHF devices, get correct offset for all*/
    hUhfr->ui32Offset = devNum *(BCHP_UHFR_2_RST- BCHP_UHFR_RST); 
#else
    hUhfr->ui32Offset = 0; /* Since only 1 UHFR device*/
#endif    

    hUhfr->origFcw = 0;
    hUhfr->adjustedFcw = 0;
    hUhfr->intFlag = 0;
    hUhfr->data.value = 0;
    hUhfr->data.prType = BUHF_PrType_eNone;
    hUhfr->totalPkt = 0;
    hUhfr->errPkt = 0;
    hUhfr->correctedErrPkt = 0;

    /* Create event handler */
    ret = BKNI_CreateEvent( &(hUhfr->hDataReadyEvent));
    if (ret != BERR_SUCCESS)
    {
         BDBG_ERR(("Could not install event handler"));
         BKNI_Free(hUhfr); /* free the memory allocated for handle before returning here*/         
         return BERR_TRACE(ret);
    }

#ifdef BCHP_PWR_RESOURCE_UHF_INPUT
    BCHP_PWR_AcquireResource(hUhfr->hChip, BCHP_PWR_RESOURCE_UHF_INPUT);
#endif

    /* Install ISR */

    if (hUhfr->settings.mode == BUHF_Mode_eAdvanced)    
    {
        if (hUhfr->settings.channel == BUHF_ChanNum_eChan9) 
        {
            ret = BINT_CreateCallback (
                &hUhfr->hCallback1,
                hUhfr->hInterrupt,
                BCHP_INT_ID_ALT_PACKET_END, 
                BUHF_P_Cust2Isr1,
                (void*)hUhfr,
                0 /* Not used */);
            ret = BINT_EnableCallback (hUhfr->hCallback1);

            ret = BINT_CreateCallback (
                &hUhfr->hCallback2,
                hUhfr->hInterrupt,
                BCHP_INT_ID_ALT_SFR, 
                BUHF_P_Cust2Isr2,
                (void*)hUhfr,
                0 /* Not used */);
            ret = BINT_EnableCallback (hUhfr->hCallback2);  
        }
        else    
        {
            ret = BINT_CreateCallback (
                &hUhfr->hCallback1,
                hUhfr->hInterrupt,
                BCHP_INT_ID_CORR_DECODE_PR1_END, 
                BUHF_P_Isr,
                (void*)hUhfr,
                BCHP_INT_ID_CORR_DECODE_PR1_END);
            ret = BINT_EnableCallback (hUhfr->hCallback1);
    
            ret = BINT_CreateCallback (
                &hUhfr->hCallback2,
                hUhfr->hInterrupt,
                BCHP_INT_ID_CORR_DECODE_PR2_END, 
                BUHF_P_Isr,
                (void*)hUhfr,
                BCHP_INT_ID_CORR_DECODE_PR2_END);
            ret = BINT_EnableCallback (hUhfr->hCallback2);
        }
    }

    BDBG_MSG(("UHF interrupt callbacks installed for UHF device %d", hUhfr->uiIndex)); 
 
    hUhfr->pMovingAvg =  (BUHF_P_MovingAvg *)BKNI_Malloc (sizeof(BUHF_P_MovingAvg)); 
    if (hUhfr->pMovingAvg == NULL)
    {
        BDBG_ERR(("BUHF_Open: Memory allocation for UHFR Moving Avg data structure failed.\n"));
        ret = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BKNI_Free(hUhfr); /* free the memory allocated for handle before returning here*/
        return ret; 
    }
        
    hUhfr->pMovingAvg->logCount = 0;
    hUhfr->pMovingAvg->overflowFlag = 0;
    hUhfr->pMovingAvg->prDcOffsetAvg = 0;
    hUhfr->pMovingAvg->prDcOffsetSum = 0;
    for (i=0; i < BUHF_MAX_LOG_SIZE; i ++) 
    {
        hUhfr->pMovingAvg->prDcOffset[i] = 0;
    }
        
    /* Initialise UHFR Registers */
    ret = BUHF_P_InitReg (hUhfr);
    if (ret != BERR_SUCCESS)
    {
        BDBG_ERR(("BUHF_Open: Failed to initialise UHFR registers correctly.\n"));
        BKNI_Free(hUhfr->pMovingAvg); 
        BKNI_Free(hUhfr); /* free the memory allocated for handle before returning here*/
        return BERR_TRACE(ret);
    }


    BUHF_SelectRfChan (hUhfr);

    return ret;
}

BERR_Code BUHF_Close
(
    BUHF_Handle hUhfr   
)
{
    /* Check parameters passed to this function */
    BDBG_ASSERT(hUhfr); 
  
    if (hUhfr->hCallback1 != 0)
    {
        BINT_DisableCallback (hUhfr->hCallback1);
        BINT_DestroyCallback (hUhfr->hCallback1);
    }   
    if (hUhfr->hCallback2 != 0)
    {
        BINT_DisableCallback (hUhfr->hCallback2);
        BINT_DestroyCallback (hUhfr->hCallback2);
    }

#ifdef BCHP_PWR_RESOURCE_UHF_INPUT
        BCHP_PWR_ReleaseResource(hUhfr->hChip, BCHP_PWR_RESOURCE_UHF_INPUT);
#endif   

    BKNI_DestroyEvent(hUhfr->hDataReadyEvent); 
    
    BKNI_Free(hUhfr->pMovingAvg);
    BKNI_Free(hUhfr);
    
    return BERR_SUCCESS;
}

BERR_Code BUHF_GetEventHandle(
    BUHF_Handle hUhfr,          /* [in] Device  handle */
    BKNI_EventHandle *phEvent   /* [out] Returns event handle */
)
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ASSERT(hUhfr);
    BDBG_ASSERT(phEvent);

    *phEvent = hUhfr->hDataReadyEvent;

    return( retCode );
}

/******************************************************************************
 *
 * Function:
 *      The interrupt handler checks which UHFR interrupt has been received 
 *      and services it accordingly. The interrupt bit is cleared by writing 
 *      a 1 to it. 
 * 
 * Returns:
 *      BERR_SUCCESS on Success
 *      Error value on other errors.
 *
 * Implementation:
 *      When a eUhfrIntCorrDecodePr1End or eUhfrIntCorrDecodePr2End 
 *      UHFR interrupt has occurred => a packet has been recieved. 
 *      On recieving a packet, the interrupt flag in UHFR configuration 
 *      data structure is set to indicate that there is data pending to be read; 
 *      this data is saved in pUhfrConfig->data to prevent loss on reset (soft
 *      resets are done by the frequency offset adjustment algorithm), and the 
 *      frequency offset adjustment algorithm is invoked.
 * 
 *****************************************************************************/
/* TODO: need to clean up this ISR */
static void BUHF_P_Isr
(
    void * pUhfr ,      /* [in] Handle to UHFR device */
    int    iParam2      /* Not used */
)
{
    unsigned int irq_status, mask, temp,status_clear=0;
    unsigned int offset = 0;
    BUHF_Handle hUhfr;
    unsigned int bchError;
 
    BDBG_ASSERT(pUhfr);

    hUhfr = (BUHF_Handle) pUhfr;
    offset=hUhfr->ui32Offset;
       
    irq_status = BREG_Read32 (hUhfr->hRegister,  (BCHP_UHFR_INTR2_CPU_STATUS+offset));
    hUhfr->ui32IntStatus = irq_status;

    mask = BREG_Read32 (hUhfr->hRegister,  (BCHP_UHFR_INTR2_CPU_MASK_STATUS+offset));
    
    BDBG_MSG(("UHFR_INTR2_CPU_STATUS=0x%08x, BCHP_UHFR_INTR2_CPU_MASK_STATUS=0x%08x", irq_status, mask));    
    
    /* If the interrupt is enabled and is set in the interrupt status register,
     * clear it and set the flag */
    temp = irq_status & ~mask;
    
    if (iParam2 == BCHP_INT_ID_CORR_DECODE_PR1_END)
    {
        BDBG_MSG(("bcmUhfrIsr: CORR_DECODE_PR1_END interrupt.\n"));
        status_clear |= (BCHP_UHFR_INTR2_CPU_STATUS_CORR_DECODE_PR1_END_MASK); /* Clear the intrpt status bit by writing 1 to it */
        hUhfr->intFlag = 1; 
        hUhfr->data.prType = BUHF_PrType_e1;
    }

    if (iParam2 == BCHP_INT_ID_CORR_DECODE_PR2_END)
    {
        BDBG_MSG(("bcmUhfrIsr: CORR_DECODE_PR2_END interrupt.\n"));
        status_clear |= (BCHP_UHFR_INTR2_CPU_STATUS_CORR_DECODE_PR2_END_MASK); /* Clear the intrpt status bit by writing 1 to it */
        hUhfr->intFlag = 1; 
        hUhfr->data.prType = BUHF_PrType_e2;
    }

    if (hUhfr->intFlag == 1)
    {
        /* The PLL workaround and the freq offset adjustment algo etc require a soft
         * reset. Soft reset also clears out COR12 => data is lost on a soft reset. 
         * Therefore, in the ISR, we save the data into hUhfr->data and read
         * it from there always. */
        /* Save the packet */
        hUhfr->data.value = BREG_Read32 (hUhfr->hRegister,  (BCHP_UHFR_COR12+offset));

        BDBG_MSG(("data.value = %x",hUhfr->data.value));

        /* Increment pkt count */
        hUhfr->totalPkt ++;
        
        bchError = (irq_status & BCHP_UHFR_INTR2_CPU_STATUS_CORR_BCH_ERR_MASK) >> BCHP_UHFR_INTR2_CPU_STATUS_CORR_BCH_ERR_SHIFT;
       
        if (bchError == 1) 
        { 
            if (hUhfr->settings.bchErrorCorrection == 1) 
            {  
               /* If enabled do BCH error correction on the recieved data*/
               temp = BUHF_P_BchErrorCorrection (hUhfr, &(hUhfr->data.value));
            }
            else
            {
                if (hUhfr->data.prType == BUHF_PrType_e1)
                    BDBG_MSG(("BCH error detected, but error correction has not been enabled\n"));
            }
            status_clear |= (BCHP_UHFR_INTR2_CPU_STATUS_CORR_BCH_ERR_MASK); /* Clear the intrpt status bit by writing 1 to it */
        }

        BKNI_SetEvent (hUhfr->hDataReadyEvent);
    }

    BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_INTR2_CPU_CLEAR+ offset), status_clear);

    return;
}

static void BUHF_P_Cust2Isr1
(
    void * pUhfr ,      /* [in] Handle to UHFR device */
    int    iParam2      /* Not used */
)
{
    unsigned int irq_status, mask, temp,status_clear=0;
    unsigned int offset = 0;
    BUHF_Handle hUhfr;
    bool    bError=false;

    BDBG_ASSERT(pUhfr);
    BSTD_UNUSED(iParam2);
    
    hUhfr = (BUHF_Handle) pUhfr;
    offset = hUhfr->ui32Offset;

    irq_status = BREG_Read32 (hUhfr->hRegister,  (BCHP_UHFR_INTR2_CPU_STATUS+offset));
    
    hUhfr->ui32IntStatus = irq_status | BCHP_UHFR_INTR2_CPU_STATUS_ALT_PACKET_END_MASK;

    mask = BREG_Read32 (hUhfr->hRegister,  (BCHP_UHFR_INTR2_CPU_MASK_STATUS+offset));
    
    BDBG_MSG(("irq_status = %x mask =%x",irq_status,mask));
    /* If the interrupt is enabled and is set in the interrupt status register,
     * clear it and set the flag */
    temp = irq_status & ~mask;
    
    if (irq_status & BCHP_UHFR_INTR2_CPU_STATUS_CORR_ALT_CHSM_ERR_MASK)
    {
        bError = true;
        BDBG_MSG(("BUHF_P_Cust2Isr1: Checksum error interrupt occured.\n"));
    }
    
    if(bError ==false)    
    {
        hUhfr->intFlag = 1; 
        hUhfr->data.value = BREG_Read32 (hUhfr->hRegister,  (BCHP_UHFR_ALTDTM+offset));
        hUhfr->data.prType = BUHF_PrType_eNone;
        BDBG_MSG(("hUhfr->data.value = %x",hUhfr->data.value));
    }

    status_clear |= (BCHP_UHFR_INTR2_CPU_STATUS_ALT_PACKET_END_MASK); /* Clear the alt_end intrpt status bit by writing 1 to it */
    status_clear |= (BCHP_UHFR_INTR2_CPU_STATUS_CORR_ALT_CHSM_ERR_MASK); /* Clear the Checksum intrpt status bit by writing 1 to it */
    
    BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_INTR2_CPU_CLEAR+ offset), status_clear);

    if(bError ==false)
        BKNI_SetEvent (hUhfr->hDataReadyEvent);

}


static void BUHF_P_Cust2Isr2
(
    void * pUhfr ,      /* [in] Handle to UHFR device */
    int    iParam2      /* Not used */
)
{
    unsigned int irq_status,status_clear=0;
    unsigned int offset = 0;
    BUHF_Handle hUhfr;

    BDBG_ASSERT(pUhfr);
    BSTD_UNUSED(iParam2);
    
    hUhfr = (BUHF_Handle) pUhfr;
    offset = hUhfr->ui32Offset;    

    irq_status = BREG_Read32 (hUhfr->hRegister,  (BCHP_UHFR_INTR2_CPU_STATUS+offset));
    
    hUhfr->ui32IntStatus = irq_status | BCHP_UHFR_INTR2_CPU_STATUS_ALT_SFR_MASK;

    BDBG_MSG(("BUHF_P_Cust2Isr2: ALT_SFR interrupt occured. Decode aborted early. Data not valid.\n"));

    status_clear |= (BCHP_UHFR_INTR2_CPU_STATUS_ALT_SFR_MASK); /* Clear the intrpt status bit by writing 1 to it */
    BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_INTR2_CPU_CLEAR + offset), status_clear);
}

BERR_Code BUHF_IsDataReady
(
    BUHF_Handle hUhfr,                  /* [in] UHFR Device handle */
    unsigned char    *pFlag             /* [out] flag indicating data is available */
)
{
    BERR_Code   ret = BERR_SUCCESS;
    unsigned int offset = 0;
    
    BDBG_ASSERT(hUhfr);
    BDBG_ASSERT(pFlag);

    /* This function is available only in advanced mode */
    if (hUhfr->settings.mode == BUHF_Mode_eLegacy)
    {
        BDBG_ERR(("BUHF_IsDataReady: not Available in UHFR legacy mode.\n"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
     
    BDBG_MSG(("BUHF_IsDataReady: Checking if there's data to be read ...\n")); 

    offset = hUhfr->ui32Offset;
    *pFlag = 0; /* Assume no data is ready */

    if (hUhfr->intFlag == 1)
    {
        *pFlag = 1;
    }
    return ret;
}


BERR_Code BUHF_Read
(
    BUHF_Handle hUhfr,              /* [in] UHFR Device handle */
    BUHF_Data   *pData              /* [out] ptr to where data is to be saved */
)
{
    BERR_Code   ret = BERR_SUCCESS;
    
    BDBG_ASSERT(hUhfr);
    BDBG_ASSERT(pData);

    /* This function is available only in advanced mode */
    if (hUhfr->settings.mode == BUHF_Mode_eLegacy)
    {
        BDBG_ERR(("BUHF_IsDataReady: not Available in UHFR legacy mode.\n"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    BDBG_MSG(("bcmUhfrRead: Reading Data...\n")); 

    /* The PLL workaround and the freq offset adjustment algo etc require a soft
     * reset. Soft reset also clears out COR12 => data is lost on a soft reset. 
     * Therefore, in the ISR, we save the data into pUhfrConfig->data and read
     * it from there always. */
    *pData = hUhfr->data;

    /* Clear the intrpt flag and data */
    hUhfr->intFlag = 0;
    hUhfr->data.value = 0;
    hUhfr->data.prType = BUHF_PrType_eNone;

    return ret;
}  

BERR_Code BUHF_GetDefaultSettings (
    BUHF_Settings *pDefSettings     /* [out] Returns default settings */
)
{
    *pDefSettings = defUhfrSettings;
    return BERR_SUCCESS;
}

BERR_Code BUHF_GetCurrentSettings (
    BUHF_Handle     hUhfr,          /* [in] UHFR Device handle */
    BUHF_Settings   *pSettings      /* [out] Returns current settings */
)
{
    BDBG_ASSERT(hUhfr);
    BDBG_ASSERT(pSettings);

    *pSettings = hUhfr->settings;
    return BERR_SUCCESS;
}


BERR_Code BUHF_GetStatus 
(
    BUHF_Handle   hUhfr,    /* [in] UHFR Device handle */
    BUHF_Status * pStatus   /* [out] ptr to where Status is to be saved */
)
{
    BERR_Code   ret = BERR_SUCCESS;
    unsigned int offset =0, mask=0, regVal=0, shift=0;    
    
    BDBG_ASSERT(hUhfr);
    BDBG_ASSERT(pStatus);
 
    offset = hUhfr->ui32Offset;
     
    /* This function is available only in advanced mode */
    if (hUhfr->settings.mode == BUHF_Mode_eLegacy)
    {
        BDBG_ERR(("BUHF_IsDataReady: not Available in UHFR legacy mode.\n"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    BDBG_MSG(("BUHF_GetStatus: Reading Status...\n")); 

    /* Get BCH error status bit */
    regVal = BREG_Read32 (hUhfr->hRegister, (BCHP_UHFR_INTR2_CPU_STATUS+offset));
    mask = BREG_Read32 (hUhfr->hRegister, (BCHP_UHFR_INTR2_CPU_MASK_STATUS+offset));

    regVal = regVal & ~mask; /* Read the bit only if this interrupt has been unmasked */
    mask = BCHP_UHFR_INTR2_CPU_STATUS_CORR_BCH_ERR_MASK;
    shift = BCHP_UHFR_INTR2_CPU_STATUS_CORR_BCH_ERR_SHIFT; 
    
    pStatus->bchError = (regVal & mask) >> shift;

    /* Read COR 13 */
    regVal = BREG_Read32 (hUhfr->hRegister,  (BCHP_UHFR_COR13+offset));

    /* Get Preamble Peak */
    mask = BCHP_UHFR_COR13_pr_corr_peak_MASK;
    shift = BCHP_UHFR_COR13_pr_corr_peak_SHIFT; 
    pStatus->prCorrPeak = (regVal & mask) >> shift ;

    /* Get Preamble DC offset */
    mask = BCHP_UHFR_COR13_dc_level_MASK;    
    shift = BCHP_UHFR_COR13_dc_level_SHIFT; 
    pStatus->dcLevel = (int) ((short int) (regVal & mask) >> shift); /* Sign extend */

    BDBG_MSG(("BCH Error=%d, Preamble Peak=0x%x, DC Level=%d, Slow RSSI out=%d \n", pStatus->bchError, pStatus->prCorrPeak, pStatus->dcLevel, pStatus->slowRssiOut)); 

    pStatus->totalPkt = hUhfr->totalPkt;
    pStatus->errPkt = hUhfr->errPkt;
    pStatus->correctedErrPkt = hUhfr->correctedErrPkt;
    pStatus->ui32IntStatus = hUhfr->ui32IntStatus;

    return ret;
}

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


/* For 65 nm chips */
BERR_Code BUHF_SelectRfChan
(
    BUHF_Handle     hUhfr           /* [in] UHFR Device handle */
)
{
    unsigned int offset=0;
    uint32_t value, fcw, ana2_val;
    BDBG_ASSERT(hUhfr);
    offset = hUhfr->ui32Offset;

    BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_ANA0 + offset), BUHF_ANA0_VALUE);       
    BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_ANA1 + offset), BUHF_ANA1_VALUE);       
    BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_ANA3 + offset), BUHF_ANA3_VALUE);       
    BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_ANA4 + offset), BUHF_ANA4_VALUE);       
    BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_ANADIR + offset), BUHF_ANADIR_VALUE);       
    ana2_val = ana2Table[hUhfr->settings.injection][hUhfr->settings.channel];
    BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_ANA2 + offset), ana2_val);       

    fcw = 0x032b9d5;
    if (hUhfr->settings.channel == BUHF_ChanNum_eChan9) {
      fcw = ~fcw + 1;
    }
    BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_FCW + offset), fcw);
       
    BKNI_Delay(20); /* TODO: get correct value */
    
    if (hUhfr->settings.injection == BUHF_InjType_eHigh) 
    {
        if (hUhfr->settings.mode == BUHF_Mode_eLegacy) /* legacy mode regardless of Cust1 or Cust2 */
        {   
            value = BREG_Read32 (hUhfr->hRegister, (BCHP_UHFR_FCW + offset));
            value = ~(value) + 1; 
            BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_FCW + offset), value);
        }
        /* COR1, COR4, and COR7 are changed in advanced mode regardless of packet type;
           these 3 registers do not affect legacy mode*/
        value = BREG_Read32 (hUhfr->hRegister, (BCHP_UHFR_COR1 + offset));         
        value ^= 0x00000100; 
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR1 + offset), value);         
        
        value = BREG_Read32 (hUhfr->hRegister, (BCHP_UHFR_COR4 + offset));         
        value = ~(value); 
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR4 + offset), value);   
       
        value = BREG_Read32 (hUhfr->hRegister, (BCHP_UHFR_COR7 + offset));
        value = ~(value);
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_COR7 + offset), value);         
    }

    return BERR_SUCCESS;
}

BERR_Code BUHF_GetDefaultStandbySettings(
    BUHF_Handle hUhf,
    BUHF_StandbySettings *pSettings
    )
{
    BDBG_ASSERT(hUhf);
    BDBG_ASSERT(pSettings);
    if(hUhf->settings.channel == BUHF_ChanNum_eChan9)
    {
        *pSettings = defStandbySettings;
        pSettings->ui32KeyPatternValue = 0x20D01068;
    }
    else
    {
        *pSettings = defStandbySettings;
    }
    return BERR_SUCCESS;
}

BERR_Code BUHF_Standby(
    BUHF_Handle hUhfr,
    const BUHF_StandbySettings *pSettings
    )
{
    BERR_Code   ret = BERR_SUCCESS;
    uint32_t    ui32RegVal;

    BDBG_ENTER(BUHF_Standby);
    
    BDBG_ASSERT(hUhfr);
    BDBG_ASSERT(pSettings);

    if(hUhfr->bStandbyState == true)
    {
        BDBG_WRN(("Already in Standby State. Doing nothing"));
        ret = BERR_SUCCESS;
        goto end;
    }

    if (!pSettings->bEnableWakeup) {
        /* power down everything */
        hUhfr->bFullPowerdown = true;       
#ifdef BCHP_PWR_RESOURCE_UHF_INPUT
        BCHP_PWR_ReleaseResource(hUhfr->hChip, BCHP_PWR_RESOURCE_UHF_INPUT);
#endif
    }
    else {
        /* otherwise, we must leave everything running, but lower the power duty cycle */
        hUhfr->bFullPowerdown = false;

        /* Enable automatic analog power down and long duty cycle power down for standby with wakeup 
           Note, this is not a separate BCHP_PWR node because analog power is still on, but managed automatically by the HW */
        ui32RegVal = BREG_Read32 (hUhfr->hRegister,  (BCHP_UHFR_AUTOPD1));
        ui32RegVal &= ~( (BCHP_MASK (UHFR_AUTOPD1, AUTO_PD_EN)) 
                       | (BCHP_MASK (UHFR_AUTOPD1, LDC_EN_AUTO_PD)));
        ui32RegVal |= (BCHP_FIELD_DATA (UHFR_AUTOPD1, AUTO_PD_EN, 1));    
        ui32RegVal |= (BCHP_FIELD_DATA (UHFR_AUTOPD1, LDC_EN_AUTO_PD, 1));
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_AUTOPD1), ui32RegVal );

        if (pSettings->bHwKeyFiltering == true)
        {
            ui32RegVal = BREG_Read32 (hUhfr->hRegister,  (BCHP_UHFR_AUTOPD5));
            ui32RegVal &= ~( (BCHP_MASK (UHFR_AUTOPD5, DIS_PKT_FILTER))
                                        |(BCHP_MASK (UHFR_AUTOPD5, EN_REPEAT_EN_INT)));
            ui32RegVal |= (BCHP_FIELD_DATA (UHFR_AUTOPD5, DIS_PKT_FILTER, 0));
            ui32RegVal |= (BCHP_FIELD_DATA (UHFR_AUTOPD5, EN_REPEAT_EN_INT, 0));
            BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_AUTOPD5), ui32RegVal );

            BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_AUTOPD4), pSettings->ui32KeyPatternValue);

        }
        else
        {
            ui32RegVal = BREG_Read32 (hUhfr->hRegister,  (BCHP_UHFR_AUTOPD5));
            ui32RegVal &= ~( (BCHP_MASK (UHFR_AUTOPD5, DIS_PKT_FILTER))
                            |(BCHP_MASK (UHFR_AUTOPD5, EN_REPEAT_EN_INT)));
            ui32RegVal |= (BCHP_FIELD_DATA (UHFR_AUTOPD5, DIS_PKT_FILTER, 1));
            ui32RegVal |= (BCHP_FIELD_DATA (UHFR_AUTOPD5, EN_REPEAT_EN_INT, 0));
            BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_AUTOPD5), ui32RegVal );   
        }
    }

    hUhfr->bStandbyState = true;
end:
    BDBG_LEAVE(BUHF_Standby);
    return ret;
}

BERR_Code BUHF_Resume(BUHF_Handle hUhfr)
{
    BERR_Code   ret = BERR_SUCCESS;
    uint32_t    ui32RegVal;

    BDBG_ENTER(BUHF_Resume);
    
    BDBG_ASSERT(hUhfr);

    if(hUhfr->bStandbyState == false)
    {
        BDBG_WRN(("Already in Resume State. Doing nothing"));
        ret = BERR_SUCCESS;
        goto end;
    }

    if (hUhfr->bFullPowerdown) {
        /* power up everything */
        hUhfr->bFullPowerdown = false;
#ifdef BCHP_PWR_RESOURCE_UHF_INPUT
        BCHP_PWR_AcquireResource(hUhfr->hChip, BCHP_PWR_RESOURCE_UHF_INPUT);
#endif
    }
    else {
        /* otherwise, just turn off automatic analog power down, which restores full power state */
        ui32RegVal = BREG_Read32 (hUhfr->hRegister,  (BCHP_UHFR_AUTOPD1));
        ui32RegVal &= ~( (BCHP_MASK (UHFR_AUTOPD1, AUTO_PD_EN)) 
                       | (BCHP_MASK (UHFR_AUTOPD1, LDC_EN_AUTO_PD)));
        ui32RegVal |= (BCHP_FIELD_DATA (UHFR_AUTOPD1, AUTO_PD_EN, 0));
        ui32RegVal |= (BCHP_FIELD_DATA (UHFR_AUTOPD1, LDC_EN_AUTO_PD, 0));
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_AUTOPD1), ui32RegVal );

        ui32RegVal = BREG_Read32 (hUhfr->hRegister,  (BCHP_UHFR_AUTOPD5));
        ui32RegVal &= ~( (BCHP_MASK (UHFR_AUTOPD5, DIS_PKT_FILTER))
                            |(BCHP_MASK (UHFR_AUTOPD5, EN_REPEAT_EN_INT)));
        ui32RegVal |= (BCHP_FIELD_DATA (UHFR_AUTOPD5, DIS_PKT_FILTER, 1));
        ui32RegVal |= (BCHP_FIELD_DATA (UHFR_AUTOPD5, EN_REPEAT_EN_INT, 1));        
        BREG_Write32 (hUhfr->hRegister, (BCHP_UHFR_AUTOPD5), ui32RegVal );
    }

    hUhfr->bStandbyState = false;
end:
    BDBG_LEAVE(BUHF_Resume);
    return ret;
}
