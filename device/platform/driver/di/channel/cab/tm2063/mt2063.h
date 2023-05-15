/*****************************************************************************
**
**  Name: mt2063.h
**
**  Copyright 2007 Microtune, Inc. All Rights Reserved
**
**  This source code file contains confidential information and/or trade
**  secrets of Microtune, Inc. or its affiliates and is subject to the
**  terms of your confidentiality agreement with Microtune, Inc. or one of
**  its affiliates, as applicable.
**
*****************************************************************************/

/*****************************************************************************
**
**  Name: mt2063.h
**
**  Description:    Microtune MT2063 B0/B1 Tuner software interface
**                  Supports tuners with Part/Rev codes: 0x9B, 0x9C.
**
**  Functions
**  Implemented:    UData_t  MT2063_Open
**                  UData_t  MT2063_Close
**                  UData_t  MT2063_GetGPIO
**                  UData_t  MT2063_GetLocked
**                  UData_t  MT2063_GetParam
**                  UData_t  MT2063_GetPowerMaskBits
**                  UData_t  MT2063_GetReg
**                  UData_t  MT2063_GetTemp
**                  UData_t  MT2063_GetUserData
**                  UData_t  MT2063_ReInit
**                  UData_t  MT2063_SetGPIO
**                  UData_t  MT2063_SetParam
**                  UData_t  MT2063_SetPowerMaskBits
**                  UData_t  MT2063_ClearPowerMaskBits
**                  UData_t  MT2063_SetReg
**                  UData_t  MT2063_Tune
**
**  References:     AN-xxxxx: MT2063 Programming Procedures Application Note
**                  MicroTune, Inc.
**                  AN-00010: MicroTuner Serial Interface Application Note
**                  MicroTune, Inc.
**
**  Enumerations
**  Defined:        MT2063_Ext_SRO
**                  MT2063_Param
**                  MT2063_Mask_Bits
**                  MT2063_GPIO_Attr;
**                  MT2063_Temperature
**
**  Exports:        None
**
**  Dependencies:   MT_ReadSub(hUserData, IC_Addr, subAddress, *pData, cnt);
**                  - Read byte(s) of data from the two-wire bus.
**
**                  MT_WriteSub(hUserData, IC_Addr, subAddress, *pData, cnt);
**                  - Write byte(s) of data to the two-wire bus.
**
**                  MT_Sleep(hUserData, nMinDelayTime);
**                  - Delay execution for nMinDelayTime milliseconds
**
**  CVS ID:         $Id: mt2063.h,v 1.5 2009/04/29 09:58:14 software Exp $
**  CVS Source:     $Source: /export/vol0/cvsroot/software/tuners/MT2063/mt2063.h,v $
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   138   06-19-2007    DAD    Ver 1.00: Initial, derived from mt2067_b.
**   N/A   08-01-2007    PINZ   Changed Analog & DVB-T settings and added
**                              SAW-less receiver mode.
**   155   10-01-2007    DAD    Ver 1.06: Add receiver mode for SECAM positive
**                                        modulation
**                                        (MT2063_ANALOG_TV_POS_NO_RFAGC_MODE)
**   N/A   10-22-2007    PINZ   Ver 1.07: Changed some Registers at init to have
**                                        the same settings as with MT Launcher
**   N/A   10-30-2007    PINZ             Add SetParam VGAGC & VGAOI
**                                        Add SetParam DNC_OUTPUT_ENABLE
**                                        Removed VGAGC from receiver mode,
**                                        default now 1
**   N/A   10-31-2007    PINZ   Ver 1.08: Add SetParam TAGC, removed from rcvr-mode
**                                        Add SetParam AMPGC, removed from rcvr-mode
**                                        Corrected names of GCU values
**                                        Actualized Receiver-Mode values
**   N/A   04-18-2008    PINZ   Ver 1.09: Add SetParam LNA_TGT
**                                        Split SetParam up to ACLNA / ACLNA_MAX
**                                        removed ACLNA_INRC/DECR (+RF & FIF)
**                                        removed GCUAUTO / BYPATNDN/UP
**   175 I 06-06-2008    PINZ   Ver 1.10: Add control to avoid US DECT freqs.
**   175 I 06-19-2008    RSK    Ver 1.17: Refactor DECT control to SpurAvoid.
**   175 I 06-24-2008    PINZ   Ver 1.18: Add Get/SetParam CTFILT_SW
**         07-10-2008    PINZ   Ver 1.19: Documentation Updates, Add a GetParam
**                                        for each SetParam (LNA_RIN, TGTs)
**         01-07-2009    PINZ   Ver 1.27: Changed value of MT2063_ALL_SD in .h
**         02-16-2009    PINZ   Ver 1.29: Several fixes to improve compiler behavior
**   N/A I 03-19-2009    PINZ   Ver 1.32: Add GetParam VERSION (of API)
**                                        Add GetParam IC_REV
*****************************************************************************/
#if !defined( __MT2063_H )
#define __MT2063_H

#include "mt_userdef.h"

#if defined( __cplusplus )
extern "C"                     /* Use "C" external linkage                  */
{
#endif


/*
** Chip revision
**
*/
typedef enum
{
    MT2063_XX = 0,
    MT2063_B0 = 206310,
    MT2063_B1 = 206311,
    MT2063_B3 = 206313
} MT2063_REVCODE;


/*
**  Values returned by the MT2063's on-chip temperature sensor
**  to be read/written.
*/
typedef enum
{
    MT2063_T_0C = 0,                    /*  Temperature approx 0C           */
    MT2063_T_10C,                       /*  Temperature approx 10C          */
    MT2063_T_20C,                       /*  Temperature approx 20C          */
    MT2063_T_30C,                       /*  Temperature approx 30C          */
    MT2063_T_40C,                       /*  Temperature approx 40C          */
    MT2063_T_50C,                       /*  Temperature approx 50C          */
    MT2063_T_60C,                       /*  Temperature approx 60C          */
    MT2063_T_70C,                       /*  Temperature approx 70C          */
    MT2063_T_80C,                       /*  Temperature approx 80C          */
    MT2063_T_90C,                       /*  Temperature approx 90C          */
    MT2063_T_100C,                      /*  Temperature approx 100C         */
    MT2063_T_110C,                      /*  Temperature approx 110C         */
    MT2063_T_120C,                      /*  Temperature approx 120C         */
    MT2063_T_130C,                      /*  Temperature approx 130C         */
    MT2063_T_140C,                      /*  Temperature approx 140C         */
    MT2063_T_150C                       /*  Temperature approx 150C         */
} MT2063_Temperature;


/*
** Parameters for selecting GPIO bits
*/
typedef enum
{
    MT2063_GPIO_IN,
    MT2063_GPIO_DIR,
    MT2063_GPIO_OUT
} MT2063_GPIO_Attr;

typedef enum
{
    MT2063_GPIO0,
    MT2063_GPIO1,
    MT2063_GPIO2
} MT2063_GPIO_ID;


/*
**  Parameter for function MT2063_SetExtSRO that specifies the external
**  SRO drive frequency.
**
**  MT2063_EXT_SRO_OFF is the power-up default value.
*/
typedef enum
{
    MT2063_EXT_SRO_OFF,                 /*  External SRO drive off          */
    MT2063_EXT_SRO_BY_4,                /*  External SRO drive divide by 4  */
    MT2063_EXT_SRO_BY_2,                /*  External SRO drive divide by 2  */
    MT2063_EXT_SRO_BY_1                 /*  External SRO drive divide by 1  */
} MT2063_Ext_SRO;


/*
**  Parameter for function MT2063_SetPowerMask that specifies the power down
**  of various sections of the MT2063.
*/
typedef enum
{
    MT2063_REG_SD   = 0x0040,   /* Shutdown regulator                 */
    MT2063_SRO_SD   = 0x0020,   /* Shutdown SRO                       */
    MT2063_AFC_SD   = 0x0010,   /* Shutdown AFC A/D                   */
    MT2063_PD_SD    = 0x0002,   /* Enable power detector shutdown     */
    MT2063_PDADC_SD = 0x0001,   /* Enable power detector A/D shutdown */
    MT2063_VCO_SD   = 0x8000,   /* Enable VCO shutdown                */
    MT2063_LTX_SD   = 0x4000,   /* Enable LTX shutdown                */
    MT2063_LT1_SD   = 0x2000,   /* Enable LT1 shutdown                */
    MT2063_LNA_SD   = 0x1000,   /* Enable LNA shutdown                */
    MT2063_UPC_SD   = 0x0800,   /* Enable upconverter shutdown        */
    MT2063_DNC_SD   = 0x0400,   /* Enable downconverter shutdown      */
    MT2063_VGA_SD   = 0x0200,   /* Enable VGA shutdown                */
    MT2063_AMP_SD   = 0x0100,   /* Enable AMP shutdown                */
    MT2063_ALL_SD   = 0xFF13,   /* All shutdown bits for this tuner   */
    MT2063_NONE_SD  = 0x0000    /* No shutdown bits                   */
} MT2063_Mask_Bits;


/*
**  Parameter for function MT2063_GetParam & MT2063_SetParam that
**  specifies the tuning algorithm parameter to be read/written.
*/
typedef enum
{
    /*  version of the API, e.g. 10302 = 1.32                               */
    MT2063_VERSION,

    /*  tuner address                                  set by MT2063_Open() */
    MT2063_IC_ADDR,

    /*  tuner revision code  (see enum  MT2063_REVCODE for values)          */
    MT2063_IC_REV,

    /*  max number of MT2063 tuners     set by MT_TUNER_CNT in mt_userdef.h */
    MT2063_MAX_OPEN,

    /*  current number of open MT2063 tuners           set by MT2063_Open() */
    MT2063_NUM_OPEN,

    /*  crystal frequency                            (default: 16000000 Hz) */
    MT2063_SRO_FREQ,

    /*  min tuning step size                            (default: 50000 Hz) */
    MT2063_STEPSIZE,

    /*  input center frequency                         set by MT2063_Tune() */
    MT2063_INPUT_FREQ,

    /*  LO1 Frequency                                  set by MT2063_Tune() */
    MT2063_LO1_FREQ,

    /*  LO1 minimum step size                          (default: 250000 Hz) */
    MT2063_LO1_STEPSIZE,

    /*  LO1 FracN keep-out region                      (default: 999999 Hz) */
    MT2063_LO1_FRACN_AVOID,

    /*  Current 1st IF in use                          set by MT2063_Tune() */
    MT2063_IF1_ACTUAL,

    /*  Requested 1st IF                               set by MT2063_Tune() */
    MT2063_IF1_REQUEST,

    /*  Center of 1st IF SAW filter                (default: 1218000000 Hz) */
    MT2063_IF1_CENTER,

    /*  Bandwidth of 1st IF SAW filter               (default: 20000000 Hz) */
    MT2063_IF1_BW,

    /*  zero-IF bandwidth                             (default: 2000000 Hz) */
    MT2063_ZIF_BW,

    /*  LO2 Frequency                                  set by MT2063_Tune() */
    MT2063_LO2_FREQ,

    /*  LO2 minimum step size                           (default: 50000 Hz) */
    MT2063_LO2_STEPSIZE,

    /*  LO2 FracN keep-out region                      (default: 374999 Hz) */
    MT2063_LO2_FRACN_AVOID,

    /*  output center frequency                        set by MT2063_Tune() */
    MT2063_OUTPUT_FREQ,

    /*  output bandwidth                               set by MT2063_Tune() */
    MT2063_OUTPUT_BW,

    /*  min inter-tuner LO separation                 (default: 1000000 Hz) */
    MT2063_LO_SEPARATION,

    /*  ID of avoid-spurs algorithm in use            compile-time constant */
    MT2063_AS_ALG,

    /*  max # of intra-tuner harmonics                       (default: 15)  */
    MT2063_MAX_HARM1,

    /*  max # of inter-tuner harmonics                        (default: 7)  */
    MT2063_MAX_HARM2,

    /*  # of 1st IF exclusion zones used               set by MT2063_Tune() */
    MT2063_EXCL_ZONES,

    /*  # of spurs found/avoided                       set by MT2063_Tune() */
    MT2063_NUM_SPURS,

    /*  >0 spurs avoided                               set by MT2063_Tune() */
    MT2063_SPUR_AVOIDED,

    /*  >0 spurs in output (mathematically)            set by MT2063_Tune() */
    MT2063_SPUR_PRESENT,

    /* Receiver Mode for some parameters. 1 is DVB-T                        */
    MT2063_RCVR_MODE,

    /* directly set LNA attenuation, parameter is value to set              */
    MT2063_ACLNA,

    /* maximum LNA attenuation, parameter is value to set                   */
    MT2063_ACLNA_MAX,

    /* directly set ATN attenuation.  Paremeter is value to set.            */
    MT2063_ACRF,

    /* maxium ATN attenuation.  Paremeter is value to set.                  */
    MT2063_ACRF_MAX,

    /* directly set FIF attenuation.  Paremeter is value to set.            */
    MT2063_ACFIF,

    /* maxium FIF attenuation.  Paremeter is value to set.                  */
    MT2063_ACFIF_MAX,

    /*  LNA Rin                                                             */
    MT2063_LNA_RIN,

    /*  Power Detector LNA level target                                     */
    MT2063_LNA_TGT,

    /*  Power Detector 1 level                                              */
    MT2063_PD1,

    /*  Power Detector 1 level target                                       */
    MT2063_PD1_TGT,

    /*  Power Detector 2 level                                              */
    MT2063_PD2,

    /*  Power Detector 2 level target                                       */
    MT2063_PD2_TGT,

    /*  Selects, which DNC is activ                                         */
    MT2063_DNC_OUTPUT_ENABLE,

    /*  VGA gain code                                                       */
    MT2063_VGAGC,

    /*  VGA bias current                                                    */
    MT2063_VGAOI,

    /*  TAGC, determins the speed of the AGC                                */
    MT2063_TAGC,

    /*  AMP gain code                                                       */
    MT2063_AMPGC,

    /* Control setting to avoid DECT freqs         (default: MT_AVOID_BOTH) */
    MT2063_AVOID_DECT,

    /* Cleartune filter selection: 0 - by IC (default), 1 - by software     */
    MT2063_CTFILT_SW,

    MT2063_EOP                    /*  last entry in enumerated list         */

} MT2063_Param;


/*
**  Parameter for selecting tuner mode
*/
typedef enum
{
    MT2063_CABLE_QAM = 0,            /* Digital cable              */
    MT2063_CABLE_ANALOG,             /* Analog cable               */
    MT2063_OFFAIR_COFDM,             /* Digital offair             */
    MT2063_OFFAIR_COFDM_SAWLESS,     /* Digital offair without SAW */
    MT2063_OFFAIR_ANALOG,            /* Analog offair              */
    MT2063_OFFAIR_8VSB,              /* Analog offair              */
	MT2063_NUM_RCVR_MODES
} MT2063_RCVR_MODES;


/*
**  Possible values for MT2063_DNC_OUTPUT
*/
typedef enum {
    MT2063_DNC_NONE = 0,
    MT2063_DNC_1,
    MT2063_DNC_2,
    MT2063_DNC_BOTH
} MT2063_DNC_Output_Enable;


/* ====== Functions which are declared in MT2063.c File ======= */

/******************************************************************************
**
**  Name: MT2063_Open
**
**  Description:    Initialize the tuner's register values.
**
**  Usage:          status = MT2063_Open(0xC0, &hMT2063, NULL);
**                  if (MT_IS_ERROR(status))
**                      //  Check error codes for reason
**
**  Parameters:     MT2063_Addr  - Serial bus address of the tuner.
**                  hMT2063      - Tuner handle passed back.
**                  hUserData    - User-defined data, if needed for the
**                                 MT_ReadSub() & MT_WriteSub functions.
**
**  Returns:        status:
**                      MT_OK             - No errors
**                      MT_TUNER_ID_ERR   - Tuner Part/Rev code mismatch
**                      MT_TUNER_INIT_ERR - Tuner initialization failed
**                      MT_COMM_ERR       - Serial bus communications error
**                      MT_ARG_NULL       - Null pointer argument passed
**                      MT_TUNER_CNT_ERR  - Too many tuners open
**
**  Dependencies:   MT_ReadSub  - Read byte(s) of data from the two-wire bus
**                  MT_WriteSub - Write byte(s) of data to the two-wire bus
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   138   06-19-2007    DAD    Ver 1.00: Initial, derived from mt2067_b.
**
******************************************************************************/
UData_t MT2063_Open(UData_t MT2063_Addr,
                    Handle_t* hMT2063,
                    Handle_t hUserData);


/******************************************************************************
**
**  Name: MT2063_Close
**
**  Description:    Release the handle to the tuner.
**
**  Parameters:     hMT2063      - Handle to the MT2063 tuner
**
**  Usage:          status = MT2063_Close(hMT2063);
**
**  Returns:        status:
**                      MT_OK         - No errors
**                      MT_INV_HANDLE - Invalid tuner handle
**
**  Dependencies:   mt_errordef.h - definition of error codes
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   138   06-19-2007    DAD    Ver 1.00: Initial, derived from mt2067_b.
**
******************************************************************************/
UData_t MT2063_Close(Handle_t hMT2063);


/****************************************************************************
**
**  Name: MT2063_Tune
**
**  Description:    Change the tuner's tuned frequency to f_in.
**
**  Parameters:     h           - Tuner handle (returned by MT2063_Open)
**                  f_in        - RF input center frequency (in Hz).
**
**  Usage:          status = MT2063_Tune(hMT2063,
**                                       f_in)
**
**  Returns:        status:
**                      MT_OK            - No errors
**                      MT_INV_HANDLE    - Invalid tuner handle
**                      MT_UPC_UNLOCK    - Upconverter PLL unlocked
**                      MT_DNC_UNLOCK    - Downconverter PLL unlocked
**                      MT_COMM_ERR      - Serial bus communications error
**                      MT_SPUR_CNT_MASK - Count of avoided LO spurs
**                      MT_SPUR_PRESENT  - LO spur possible in output
**                      MT_FIN_RANGE     - Input freq out of range
**                      MT_FOUT_RANGE    - Output freq out of range
**                      MT_UPC_RANGE     - Upconverter freq out of range
**                      MT_DNC_RANGE     - Downconverter freq out of range
**
**  Dependencies:   MUST CALL MT2063_Open BEFORE MT2063_Tune!
**
**                  MT_ReadSub   - Read data from the two-wire serial bus
**                  MT_WriteSub  - Write data to the two-wire serial bus
**                  MT_Sleep     - Delay execution for x milliseconds
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   138   06-19-2007    DAD    Ver 1.00: Initial, derived from mt2067_b.
**
****************************************************************************/
UData_t MT2063_Tune(Handle_t h,
                    UData_t f_in);    /* RF input center frequency   */


/******************************************************************************
**
**  Name: MT2063_GetGPIO
**
**  Description:    Get the current MT2063 GPIO value.
**
**  Parameters:     h            - Open handle to the tuner (from MT2063_Open).
**                  attr         - Selects input readback, I/O direction or
**                                 output value
**                  bit          - Selects GPIO0, GPIO1, or GPIO2
**                  *value       - current setting of GPIO pin
**
**  Usage:          status = MT2063_GetGPIO(hMT2063, MT2063_GPIO2, &value);
**
**  Returns:        status:
**                      MT_OK            - No errors
**                      MT_COMM_ERR      - Serial bus communications error
**                      MT_INV_HANDLE    - Invalid tuner handle
**                      MT_ARG_NULL      - Null pointer argument passed
**
**  Dependencies:   MT_ReadSub  - Read byte(s) of data from the serial bus
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   138   06-19-2007    DAD    Ver 1.00: Initial, derived from mt2067_b.
**
******************************************************************************/
UData_t MT2063_GetGPIO(Handle_t h, MT2063_GPIO_ID gpio_id, MT2063_GPIO_Attr attr, UData_t* value);


/****************************************************************************
**
**  Name: MT2063_GetLocked
**
**  Description:    Checks to see if LO1 and LO2 are locked.
**
**  Parameters:     h            - Open handle to the tuner (from MT2063_Open).
**
**  Usage:          status = MT2063_GetLocked(hMT2063);
**                  if (status & (MT_UPC_UNLOCK | MT_DNC_UNLOCK))
**                      //  error!, one or more PLL's unlocked
**
**  Returns:        status:
**                      MT_OK            - No errors
**                      MT_UPC_UNLOCK    - Upconverter PLL unlocked
**                      MT_DNC_UNLOCK    - Downconverter PLL unlocked
**                      MT_COMM_ERR      - Serial bus communications error
**                      MT_INV_HANDLE    - Invalid tuner handle
**
**  Dependencies:   MT_ReadSub    - Read byte(s) of data from the serial bus
**                  MT_Sleep      - Delay execution for x milliseconds
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   138   06-19-2007    DAD    Ver 1.00: Initial, derived from mt2067_b.
**
****************************************************************************/
UData_t MT2063_GetLocked(Handle_t h);


/****************************************************************************
**
**  Name: MT2063_GetParam
**
**  Description:    Gets a tuning algorithm parameter.
**
**                  This function provides access to the internals of the
**                  tuning algorithm - mostly for testing purposes.
**
**  Parameters:     h           - Tuner handle (returned by MT2063_Open)
**                  param       - Tuning algorithm parameter
**                                (see enum MT2063_Param)
**                  pValue      - ptr to returned value
**
**                  param                     Description
**                  ----------------------    --------------------------------
**                  MT2063_IC_ADDR            Serial Bus address of this tuner
**                  MT2063_MAX_OPEN           Max # of MT2063's allowed open
**                  MT2063_NUM_OPEN           # of MT2063's open
**                  MT2063_SRO_FREQ           crystal frequency
**                  MT2063_STEPSIZE           minimum tuning step size
**                  MT2063_INPUT_FREQ         input center frequency
**                  MT2063_LO1_FREQ           LO1 Frequency
**                  MT2063_LO1_STEPSIZE       LO1 minimum step size
**                  MT2063_LO1_FRACN_AVOID    LO1 FracN keep-out region
**                  MT2063_IF1_ACTUAL         Current 1st IF in use
**                  MT2063_IF1_REQUEST        Requested 1st IF
**                  MT2063_IF1_CENTER         Center of 1st IF SAW filter
**                  MT2063_IF1_BW             Bandwidth of 1st IF SAW filter
**                  MT2063_ZIF_BW             zero-IF bandwidth
**                  MT2063_LO2_FREQ           LO2 Frequency
**                  MT2063_LO2_STEPSIZE       LO2 minimum step size
**                  MT2063_LO2_FRACN_AVOID    LO2 FracN keep-out region
**                  MT2063_OUTPUT_FREQ        output center frequency
**                  MT2063_OUTPUT_BW          output bandwidth
**                  MT2063_LO_SEPARATION      min inter-tuner LO separation
**                  MT2063_AS_ALG             ID of avoid-spurs algorithm in use
**                  MT2063_MAX_HARM1          max # of intra-tuner harmonics
**                  MT2063_MAX_HARM2          max # of inter-tuner harmonics
**                  MT2063_EXCL_ZONES         # of 1st IF exclusion zones
**                  MT2063_NUM_SPURS          # of spurs found/avoided
**                  MT2063_SPUR_AVOIDED       >0 spurs avoided
**                  MT2063_SPUR_PRESENT       >0 spurs in output (mathematically)
**                  MT2063_RCVR_MODE          Predefined modes.
**                  MT2063_LNA_RIN            Get LNA RIN value
**                  MT2063_LNA_TGT            Get target power level at LNA
**                  MT2063_PD1_TGT            Get target power level at PD1
**                  MT2063_PD2_TGT            Get target power level at PD2
**                  MT2063_ACLNA              LNA attenuator gain code
**                  MT2063_ACRF               RF attenuator gain code
**                  MT2063_ACFIF              FIF attenuator gain code
**                  MT2063_ACLNA_MAX          LNA attenuator limit
**                  MT2063_ACRF_MAX           RF attenuator limit
**                  MT2063_ACFIF_MAX          FIF attenuator limit
**                  MT2063_PD1                Actual value of PD1
**                  MT2063_PD2                Actual value of PD2
**                  MT2063_DNC_OUTPUT_ENABLE  DNC output selection
**                  MT2063_VGAGC              VGA gain code
**                  MT2063_VGAOI              VGA output current
**                  MT2063_TAGC               TAGC setting
**                  MT2063_AMPGC              AMP gain code
**                  MT2063_AVOID_DECT         Avoid DECT Frequencies
**                  MT2063_CTFILT_SW          Cleartune filter selection
**
**  Usage:          status |= MT2063_GetParam(hMT2063,
**                                            MT2063_IF1_ACTUAL,
**                                            &f_IF1_Actual);
**
**  Returns:        status:
**                      MT_OK            - No errors
**                      MT_INV_HANDLE    - Invalid tuner handle
**                      MT_ARG_NULL      - Null pointer argument passed
**                      MT_ARG_RANGE     - Invalid parameter requested
**
**  Dependencies:   USERS MUST CALL MT2063_Open() FIRST!
**
**  See Also:       MT2063_SetParam, MT2063_Open
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   138   06-19-2007    DAD    Ver 1.00: Initial, derived from mt2067_b.
**   154   09-13-2007    RSK    Ver 1.05: Get/SetParam changes for LOx_FREQ
**         10-31-2007    PINZ   Ver 1.08: Get/SetParam add VGAGC, VGAOI, AMPGC, TAGC
**   173 M 01-23-2008    RSK    Ver 1.12: Read LO1C and LO2C registers from HW
**                                        in GetParam.
**         04-18-2008    PINZ   Ver 1.15: Add SetParam LNARIN & PDxTGT
**                                        Split SetParam up to ACLNA / ACLNA_MAX
**                                        removed ACLNA_INRC/DECR (+RF & FIF)
**                                        removed GCUAUTO / BYPATNDN/UP
**   175 I 16-06-2008    PINZ   Ver 1.16: Add control to avoid US DECT freqs.
**   175 I 06-19-2008    RSK    Ver 1.17: Refactor DECT control to SpurAvoid.
**         06-24-2008    PINZ   Ver 1.18: Add Get/SetParam CTFILT_SW
**         07-10-2008    PINZ   Ver 1.19: Documentation Updates, Add a GetParam
**                                        for each SetParam (LNA_RIN, TGTs)
**
****************************************************************************/
UData_t MT2063_GetParam(Handle_t     h,
                        MT2063_Param param,
                        UData_t*     pValue);


/****************************************************************************
**
**  Name: MT2063_GetReg
**
**  Description:    Gets an MT2063 register.
**
**  Parameters:     h           - Tuner handle (returned by MT2063_Open)
**                  reg         - MT2063 register/subaddress location
**                  *val        - MT2063 register/subaddress value
**
**  Returns:        status:
**                      MT_OK            - No errors
**                      MT_COMM_ERR      - Serial bus communications error
**                      MT_INV_HANDLE    - Invalid tuner handle
**                      MT_ARG_NULL      - Null pointer argument passed
**                      MT_ARG_RANGE     - Argument out of range
**
**  Dependencies:   USERS MUST CALL MT2063_Open() FIRST!
**
**                  Use this function if you need to read a register from
**                  the MT2063.
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   138   06-19-2007    DAD    Ver 1.00: Initial, derived from mt2067_b.
**
****************************************************************************/
UData_t MT2063_GetReg(Handle_t h,
                      U8Data   reg,
                      U8Data*  val);


/******************************************************************************
**
**  Name: MT2063_GetTemp
**
**  Description:    Get the MT2063 Temperature register.
**
**  Parameters:     h            - Open handle to the tuner (from MT2063_Open).
**                  *value       - value read from the register
**
**                                    Binary
**                  Value Returned    Value    Approx Temp
**                  ---------------------------------------------
**                  MT2063_T_0C       0000         0C
**                  MT2063_T_10C      0001        10C
**                  MT2063_T_20C      0010        20C
**                  MT2063_T_30C      0011        30C
**                  MT2063_T_40C      0100        40C
**                  MT2063_T_50C      0101        50C
**                  MT2063_T_60C      0110        60C
**                  MT2063_T_70C      0111        70C
**                  MT2063_T_80C      1000        80C
**                  MT2063_T_90C      1001        90C
**                  MT2063_T_100C     1010       100C
**                  MT2063_T_110C     1011       110C
**                  MT2063_T_120C     1100       120C
**                  MT2063_T_130C     1101       130C
**                  MT2063_T_140C     1110       140C
**                  MT2063_T_150C     1111       150C
**
**  Returns:        status:
**                      MT_OK            - No errors
**                      MT_COMM_ERR      - Serial bus communications error
**                      MT_INV_HANDLE    - Invalid tuner handle
**                      MT_ARG_NULL      - Null pointer argument passed
**                      MT_ARG_RANGE     - Argument out of range
**
**  Dependencies:   MT_ReadSub  - Read byte(s) of data from the two-wire bus
**                  MT_WriteSub - Write byte(s) of data to the two-wire bus
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   138   06-19-2007    DAD    Ver 1.00: Initial, derived from mt2067_b.
**
******************************************************************************/
UData_t MT2063_GetTemp(Handle_t h, MT2063_Temperature* value);


/****************************************************************************
**
**  Name: MT2063_GetUserData
**
**  Description:    Gets the user-defined data item.
**
**  Parameters:     h           - Tuner handle (returned by MT2063_Open)
**
**  Usage:          status = MT2063_GetUserData(hMT2063, &hUserData);
**
**  Returns:        status:
**                      MT_OK            - No errors
**                      MT_INV_HANDLE    - Invalid tuner handle
**                      MT_ARG_NULL      - Null pointer argument passed
**
**  Dependencies:   USERS MUST CALL MT2063_Open() FIRST!
**
**                  The hUserData parameter is a user-specific argument
**                  that is stored internally with the other tuner-
**                  specific information.
**
**                  For example, if additional arguments are needed
**                  for the user to identify the device communicating
**                  with the tuner, this argument can be used to supply
**                  the necessary information.
**
**                  The hUserData parameter is initialized in the tuner's
**                  Open function to NULL.
**
**  See Also:       MT2063_Open
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   138   06-19-2007    DAD    Ver 1.00: Initial, derived from mt2067_b.
**
****************************************************************************/
UData_t MT2063_GetUserData(Handle_t h,
                           Handle_t* hUserData);


/******************************************************************************
**
**  Name: MT2063_ReInit
**
**  Description:    Initialize the tuner's register values.
**
**  Parameters:     h           - Tuner handle (returned by MT2063_Open)
**
**  Returns:        status:
**                      MT_OK            - No errors
**                      MT_TUNER_ID_ERR   - Tuner Part/Rev code mismatch
**                      MT_INV_HANDLE    - Invalid tuner handle
**                      MT_COMM_ERR      - Serial bus communications error
**
**  Dependencies:   MT_ReadSub  - Read byte(s) of data from the two-wire bus
**                  MT_WriteSub - Write byte(s) of data to the two-wire bus
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   138   06-19-2007    DAD    Ver 1.00: Initial, derived from mt2067_b.
**
******************************************************************************/
UData_t MT2063_ReInit(Handle_t h);


/******************************************************************************
**
**  Name: MT2063_SetGPIO
**
**  Description:    Modify the MT2063 GPIO value.
**
**  Parameters:     h            - Open handle to the tuner (from MT2063_Open).
**                  gpio_id      - Selects GPIO0, GPIO1, or GPIO2
**                  attr         - Selects input readback, I/O direction or
**                                 output value
**                  value        - value to set GPIO pin
**
**  Usage:          status = MT2063_SetGPIO(hMT2063, MT2063_GPIO1, MT2063_GPIO_OUT, 1);
**
**  Returns:        status:
**                      MT_OK            - No errors
**                      MT_COMM_ERR      - Serial bus communications error
**                      MT_INV_HANDLE    - Invalid tuner handle
**
**  Dependencies:   MT_WriteSub - Write byte(s) of data to the two-wire-bus
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   138   06-19-2007    DAD    Ver 1.00: Initial, derived from mt2067_b.
**
******************************************************************************/
UData_t MT2063_SetGPIO(Handle_t h, MT2063_GPIO_ID gpio_id, MT2063_GPIO_Attr attr, UData_t value);


/****************************************************************************
**
**  Name: MT2063_SetParam
**
**  Description:    Sets a tuning algorithm parameter.
**
**                  This function provides access to the internals of the
**                  tuning algorithm.  You can override many of the tuning
**                  algorithm defaults using this function.
**
**  Parameters:     h           - Tuner handle (returned by MT2063_Open)
**                  param       - Tuning algorithm parameter
**                                (see enum MT2063_Param)
**                  nValue      - value to be set
**
**                  param                     Description
**                  ----------------------    --------------------------------
**                  MT2063_SRO_FREQ           crystal frequency
**                  MT2063_STEPSIZE           minimum tuning step size
**                  MT2063_LO1_FREQ           LO1 frequency
**                  MT2063_LO1_STEPSIZE       LO1 minimum step size
**                  MT2063_LO1_FRACN_AVOID    LO1 FracN keep-out region
**                  MT2063_IF1_REQUEST        Requested 1st IF
**                  MT2063_ZIF_BW             zero-IF bandwidth
**                  MT2063_LO2_FREQ           LO2 frequency
**                  MT2063_LO2_STEPSIZE       LO2 minimum step size
**                  MT2063_LO2_FRACN_AVOID    LO2 FracN keep-out region
**                  MT2063_OUTPUT_FREQ        output center frequency
**                  MT2063_OUTPUT_BW          output bandwidth
**                  MT2063_LO_SEPARATION      min inter-tuner LO separation
**                  MT2063_MAX_HARM1          max # of intra-tuner harmonics
**                  MT2063_MAX_HARM2          max # of inter-tuner harmonics
**                  MT2063_RCVR_MODE          Predefined modes
**                  MT2063_LNA_RIN            Set LNA Rin (*)
**                  MT2063_LNA_TGT            Set target power level at LNA (*)
**                  MT2063_PD1_TGT            Set target power level at PD1 (*)
**                  MT2063_PD2_TGT            Set target power level at PD2 (*)
**                  MT2063_ACLNA_MAX          LNA attenuator limit (*)
**                  MT2063_ACRF_MAX           RF attenuator limit (*)
**                  MT2063_ACFIF_MAX          FIF attenuator limit (*)
**                  MT2063_DNC_OUTPUT_ENABLE  DNC output selection
**                  MT2063_VGAGC              VGA gain code
**                  MT2063_VGAOI              VGA output current
**                  MT2063_TAGC               TAGC setting
**                  MT2063_AMPGC              AMP gain code
**                  MT2063_AVOID_DECT         Avoid DECT Frequencies
**                  MT2063_CTFILT_SW          Cleartune filter selection
**
**                  (*) This parameter is set by MT2063_RCVR_MODE, do not call
**                      additionally.
**
**  Usage:          status |= MT2063_SetParam(hMT2063,
**                                            MT2063_STEPSIZE,
**                                            50000);
**
**  Returns:        status:
**                      MT_OK            - No errors
**                      MT_INV_HANDLE    - Invalid tuner handle
**                      MT_ARG_NULL      - Null pointer argument passed
**                      MT_ARG_RANGE     - Invalid parameter requested
**                                         or set value out of range
**                                         or non-writable parameter
**
**  Dependencies:   USERS MUST CALL MT2063_Open() FIRST!
**
**  See Also:       MT2063_GetParam, MT2063_Open
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   138   06-19-2007    DAD    Ver 1.00: Initial, derived from mt2067_b.
**   154   09-13-2007    RSK    Ver 1.05: Get/SetParam changes for LOx_FREQ
**         10-31-2007    PINZ   Ver 1.08: Get/SetParam add VGAGC, VGAOI, AMPGC, TAGC
**         04-18-2008    PINZ   Ver 1.15: Add SetParam LNARIN & PDxTGT
**                                        Split SetParam up to ACLNA / ACLNA_MAX
**                                        removed ACLNA_INRC/DECR (+RF & FIF)
**                                        removed GCUAUTO / BYPATNDN/UP
**   175 I 06-06-2008    PINZ   Ver 1.16: Add control to avoid US DECT freqs.
**   175 I 06-19-2008    RSK    Ver 1.17: Refactor DECT control to SpurAvoid.
**         06-24-2008    PINZ   Ver 1.18: Add Get/SetParam CTFILT_SW
**
****************************************************************************/
UData_t MT2063_SetParam(Handle_t     h,
                        MT2063_Param param,
                        UData_t      nValue);


/****************************************************************************
**
**  Name: MT2063_SetPowerMaskBits
**
**  Description:    Sets the power-down mask bits for various sections of
**                  the MT2063
**
**  Parameters:     h           - Tuner handle (returned by MT2063_Open)
**                  Bits        - Mask bits to be set.
**
**                    MT2063_ALL_SD   All shutdown bits for this tuner
**
**                    MT2063_REG_SD   Shutdown regulator
**                    MT2063_SRO_SD   Shutdown SRO
**                    MT2063_AFC_SD   Shutdown AFC A/D
**                    MT2063_PD_SD    Enable power detector shutdown
**                    MT2063_PDADC_SD Enable power detector A/D shutdown
**                    MT2063_VCO_SD   Enable VCO shutdown VCO
**                    MT2063_UPC_SD   Enable upconverter shutdown
**                    MT2063_DNC_SD   Enable downconverter shutdown
**                    MT2063_VGA_SD   Enable VGA shutdown
**
**  Returns:        status:
**                      MT_OK            - No errors
**                      MT_INV_HANDLE    - Invalid tuner handle
**                      MT_COMM_ERR      - Serial bus communications error
**
**  Dependencies:   USERS MUST CALL MT2063_Open() FIRST!
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   138   06-19-2007    DAD    Ver 1.00: Initial, derived from mt2067_b.
**
****************************************************************************/
UData_t MT2063_SetPowerMaskBits(Handle_t h, MT2063_Mask_Bits Bits);


/****************************************************************************
**
**  Name: MT2063_ClearPowerMaskBits
**
**  Description:    Clears the power-down mask bits for various sections of
**                  the MT2063
**
**  Parameters:     h           - Tuner handle (returned by MT2063_Open)
**                  Bits        - Mask bits to be cleared.
**
**                    MT2063_ALL_SD   All shutdown bits for this tuner
**
**                    MT2063_REG_SD   Shutdown regulator
**                    MT2063_SRO_SD   Shutdown SRO
**                    MT2063_AFC_SD   Shutdown AFC A/D
**                    MT2063_PD_SD    Enable power detector shutdown
**                    MT2063_PDADC_SD Enable power detector A/D shutdown
**                    MT2063_VCO_SD   Enable VCO shutdown VCO
**                    MT2063_UPC_SD   Enable upconverter shutdown
**                    MT2063_DNC_SD   Enable downconverter shutdown
**                    MT2063_VGA_SD   Enable VGA shutdown
**
**  Returns:        status:
**                      MT_OK            - No errors
**                      MT_INV_HANDLE    - Invalid tuner handle
**                      MT_COMM_ERR      - Serial bus communications error
**
**  Dependencies:   USERS MUST CALL MT2063_Open() FIRST!
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   138   06-19-2007    DAD    Ver 1.00: Initial, derived from mt2067_b.
**
****************************************************************************/
UData_t MT2063_ClearPowerMaskBits(Handle_t h, MT2063_Mask_Bits Bits);


/****************************************************************************
**
**  Name: MT2063_GetPowerMaskBits
**
**  Description:    Returns a mask of the enabled power shutdown bits
**
**  Parameters:     h           - Tuner handle (returned by MT2063_Open)
**                  Bits        - Mask bits to currently set.
**
**                    MT2063_REG_SD   Shutdown regulator
**                    MT2063_SRO_SD   Shutdown SRO
**                    MT2063_AFC_SD   Shutdown AFC A/D
**                    MT2063_PD_SD    Enable power detector shutdown
**                    MT2063_PDADC_SD Enable power detector A/D shutdown
**                    MT2063_VCO_SD   Enable VCO shutdown VCO
**                    MT2063_UPC_SD   Enable upconverter shutdown
**                    MT2063_DNC_SD   Enable downconverter shutdown
**                    MT2063_VGA_SD   Enable VGA shutdown
**
**  Returns:        status:
**                      MT_OK            - No errors
**                      MT_INV_HANDLE    - Invalid tuner handle
**                      MT_ARG_NULL      - Output argument is NULL
**                      MT_COMM_ERR      - Serial bus communications error
**
**  Dependencies:   USERS MUST CALL MT2063_Open() FIRST!
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   138   06-19-2007    DAD    Ver 1.00: Initial, derived from mt2067_b.
**
****************************************************************************/
UData_t MT2063_GetPowerMaskBits(Handle_t h, MT2063_Mask_Bits *Bits);


/****************************************************************************
**
**  Name: MT2063_EnableExternalShutdown
**
**  Description:    Enables or disables the operation of the external
**                  shutdown pin
**
**  Parameters:     h           - Tuner handle (returned by MT2063_Open)
**                  Enabled     - 0 = disable the pin, otherwise enable it
**
**  Returns:        status:
**                      MT_OK            - No errors
**                      MT_INV_HANDLE    - Invalid tuner handle
**                      MT_COMM_ERR      - Serial bus communications error
**
**  Dependencies:   USERS MUST CALL MT2063_Open() FIRST!
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   138   06-19-2007    DAD    Ver 1.00: Initial, derived from mt2067_b.
**
****************************************************************************/
UData_t MT2063_EnableExternalShutdown(Handle_t h, U8Data Enabled);


/****************************************************************************
**
**  Name: MT2063_SoftwareShutdown
**
**  Description:    Enables or disables software shutdown function.  When
**                  Shutdown==1, any section whose power mask is set will be
**                  shutdown.
**
**  Parameters:     h           - Tuner handle (returned by MT2063_Open)
**                  Shutdown    - 1 = shutdown the masked sections, otherwise
**                                power all sections on
**
**  Returns:        status:
**                      MT_OK            - No errors
**                      MT_INV_HANDLE    - Invalid tuner handle
**                      MT_COMM_ERR      - Serial bus communications error
**
**  Dependencies:   USERS MUST CALL MT2063_Open() FIRST!
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   138   06-19-2007    DAD    Ver 1.00: Initial, derived from mt2067_b.
**
****************************************************************************/
UData_t MT2063_SoftwareShutdown(Handle_t h, U8Data Shutdown);


/****************************************************************************
**
**  Name: MT2063_SetExtSRO
**
**  Description:    Sets the external SRO driver.
**
**  Parameters:     h           - Tuner handle (returned by MT2063_Open)
**                  Ext_SRO_Setting - external SRO drive setting
**
**       (default)    MT2063_EXT_SRO_OFF  - ext driver off
**                    MT2063_EXT_SRO_BY_1 - ext driver = SRO frequency
**                    MT2063_EXT_SRO_BY_2 - ext driver = SRO/2 frequency
**                    MT2063_EXT_SRO_BY_4 - ext driver = SRO/4 frequency
**
**  Returns:        status:
**                      MT_OK            - No errors
**                      MT_COMM_ERR      - Serial bus communications error
**                      MT_INV_HANDLE    - Invalid tuner handle
**
**  Dependencies:   USERS MUST CALL MT2063_Open() FIRST!
**
**                  The Ext_SRO_Setting settings default to OFF
**                  Use this function if you need to override the default
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   138   06-19-2007    DAD    Ver 1.00: Initial, derived from mt2067_b.
**
****************************************************************************/
UData_t MT2063_SetExtSRO(Handle_t h, MT2063_Ext_SRO Ext_SRO_Setting);


/****************************************************************************
**
**  Name: MT2063_SetReg
**
**  Description:    Sets an MT2063 register.
**
**  Parameters:     h           - Tuner handle (returned by MT2063_Open)
**                  reg         - MT2063 register/subaddress location
**                  val         - MT2063 register/subaddress value
**
**  Returns:        status:
**                      MT_OK            - No errors
**                      MT_COMM_ERR      - Serial bus communications error
**                      MT_INV_HANDLE    - Invalid tuner handle
**                      MT_ARG_RANGE     - Argument out of range
**
**  Dependencies:   USERS MUST CALL MT2063_Open() FIRST!
**
**                  Use this function if you need to override a default
**                  register value
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   138   06-19-2007    DAD    Ver 1.00: Initial, derived from mt2067_b.
**
****************************************************************************/
UData_t MT2063_SetReg(Handle_t h,
                      U8Data   reg,
                      U8Data   val);


#if defined( __cplusplus )
}
#endif

#endif

