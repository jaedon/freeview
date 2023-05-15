/*
  Copyright (C) 2006-2009 NXP B.V., All Rights Reserved.
  This source code and any compilation or derivative thereof is the proprietary
  information of NXP B.V. and is confidential in nature. Under no circumstances
  is this software to be  exposed to or placed under an Open Source License of
  any type without the expressed written permission of NXP B.V.
 *
 * \file          tmbslTDA18260_Local.h
 *
 *                1
 *
 * \date          %modify_time%
 *
 * \brief         Describe briefly the purpose of this file.
 *
 * REFERENCE DOCUMENTS :
 *                TDA18260_Driver_User_Guide.pdf
 *
 * Detailed description may be added here.
 *
 * \section info Change Information
 *
*/

#ifndef _TMBSL_TDA18260_LOCAL_H
#define _TMBSL_TDA18260_LOCAL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*/
/* Types and defines:                                                         */
/*============================================================================*/

/* Maximum number of devices supported by driver */
#define TDA18260_MAX_UNITS                      (6)
#define TDA18260_MAX_IDENTITIES                 (3)

/* Driver version definition */
#define TMBSL_TDA18260_COMP_NUM                 3  /* Major protocol change - Specification change required */
#define TMBSL_TDA18260_MAJOR_VER                2  /* Minor protocol change - Specification change required */
#define TMBSL_TDA18260_MINOR_VER                1  /* Software update - No protocol change - No specification change required */

#define TMBSL_TDA18260_COMPONENT_NAME_STR       "TDA18260"

/* Instance macros */
#define P_OBJ_VALID                             (pObj != Null)

/* I/O Functions macros */
#define P_SIO                                   pObj->sIo
#define P_SIO_READ_VALID                        (P_OBJ_VALID && (P_SIO.Read != Null))
#define P_SIO_WRITE_VALID                       (P_OBJ_VALID && (P_SIO.Write != Null))

/* I/O sub-address macros */
#define TMBSL_TDA18260_READ_SUB_ADDR_SZ         1
#define TMBSL_TDA18260_WRITE_SUB_ADDR_SZ        1

/* Time Functions macros */
#define P_STIME                                 pObj->sTime
#define P_STIME_WAIT_VALID                      (P_OBJ_VALID && (P_STIME.Wait != Null))

/* Debug Functions macros */
#define P_SDEBUG                                pObj->sDebug
#define P_DBGPRINTVALID                         (P_OBJ_VALID && (P_SDEBUG.Print != Null))
#define P_DBGPRINTEx                            pObj->sDebug.Print

/* Mutex Functions macros */
#define P_SMUTEX                                pObj->sMutex
#define P_SMUTEX_INIT_VALID                     (P_OBJ_VALID && (P_SMUTEX.Init != Null))
#define P_SMUTEX_DEINIT_VALID                   (P_OBJ_VALID && (P_SMUTEX.DeInit != Null))
#define P_SMUTEX_ACQUIRE_VALID                  (P_OBJ_VALID && (P_SMUTEX.Acquire != Null))
#define P_SMUTEX_RELEASE_VALID                  (P_OBJ_VALID && (P_SMUTEX.Release != Null))

/* Driver Mutex macros */
#define TDA18260_MUTEX_TIMEOUT                  (5000)
#define P_MUTEX                                 pObj->pMutex
#define P_MUTEX_VALID                           (P_MUTEX != Null)

#define tmTDA18260SHARED_I2C_MAP_NB_BYTES        (0x24)
#define tmTDA18260STREAM_I2C_MAP_NB_BYTES        (0x29)

#define tmTDA18260_IR_FREQ1 123000000
#define tmTDA18260_IR_FREQ2 355000000
#define tmTDA18260_IR_FREQ3 785000000

#define tmTDA18260_IND_RFAGC_Top_Max 6

#define tmTDA18260_RSSI_CODE_MAX 200
#define tmTDA18260_RSSI_CODE_MIN 10

#define TDA18260_POWER_LEVEL_ERR       -10000     /* dBmV */
#define TDA18260_POWER_LEVEL_LOW        -5000     /* dBmV */
#define TDA18260_POWER_LEVEL_HIGH       +5000     /* dBmV */

#define TDA18260_SIGNAL_CALIB_WAIT    5   /* ms */
#define TDA18260_SIGNAL_CALIB_TIMEOUT 2   /* nb of occurences */
#define TDA18260_IMAGE_CALIB_WAIT     5   /* ms */
#define TDA18260_IMAGE_CALIB_TIMEOUT  8   /* nb of occurences */

#define TDA18260_AGC2_GAIN_0DB 60

#define TDA18260_SHARED_PART_ACCESS_TIMEOUT 100
#define TDA18260_SHARED_PART_ACCESS_COUNT   20

#define TDA18260_TUNIT_OFFSET 0x10

/* IR Calibration target settings */
#define NB_IR_CAL_STEP 2
#define IR_CAL_TARGET_62DBC 0x7
#define IR_CAL_TARGET_59DBC 0x4

/* TDA18260 xtal_xtout_mode */
typedef enum _tmTDA18260_xtal_xtout_mode_t
{
    tmTDA18260_xtal_xtout_mode_Off = 0,
    tmTDA18260_xtal_xtout_mode_Normal = 1,
    tmTDA18260_xtal_xtout_mode_Max = 2
} tmTDA18260_xtal_xtout_mode_t, *ptmTDA18260_xtal_xtout_mode_t;

/* TDA18260 xtal_xtout_drive */
typedef enum _tmTDA18260_xtal_xtout_drive_t
{
    tmTDA18260_xtal_xtout_drive_Off = 0,
    tmTDA18260_xtal_xtout_drive_400mV = 1,
    tmTDA18260_xtal_xtout_drive_800mV = 3,
    tmTDA18260_xtal_xtout_drive_1200mV = 7,
    tmTDA18260_xtal_xtout_drive_Max = 8
} tmTDA18260_xtal_xtout_drive_t, *ptmTDA18260_xtal_xtout_drive_t;


/* TDA18260 Gud Up/Down selection */
typedef enum _tmTDA18260_Gud_t
{
    tmTDA18260_GUD_DOWN = 0,
    tmTDA18260_GUD_UP = 1,
    tmTDA18260_GUD_Max = 2
} tmTDA18260_Gud_t, *ptmTDA18260_Gud_t;

/* TDA18260 Aud Up/Down selection */
typedef enum _tmTDA18260_Aud_t
{
    tmTDA18260_AUD_UP = 0,
    tmTDA18260_AUD_DOWN = 1,
    tmTDA18260_AUD_Max = 2
} tmTDA18260_Aud_t, *ptmTDA18260_Aud_t;

typedef enum _tmTDA18260HcMode_t
{
    tmTDA18260_HcModeNormal = 0,
    tmTDA18260_HcModeSaIRcalWanted = 1,
    tmTDA18260_HcModeSaIRcalImage = 2,
    tmTDA18260_HcModeSbIRcalWanted = 3,
    tmTDA18260_HcModeSbIRcalImage = 4,
    tmTDA18260_HcMode_MAX = 5
} tmTDA18260HcMode_t, *ptmTDA18260HcMode_t;

/* RFcal_modeOFF switch State */
typedef enum _tmTDA18260_SW_RFcal_modeOFF_t
{
    tmTDA18260_RFcal_modeOFF_CLOSE = 0,
    tmTDA18260_RFcal_modeOFF_OPEN = 1,
    tmTDA18260_RFcal_modeOFF_Max = 2
} tmTDA18260_SW_RFcal_modeOFF_t, *ptmTDA18260_SW_RFcal_modeOFF_t;

/* AGC1 Gain State */
typedef enum _tmTDA18260_AGC1_Gain_t
{
    tmTDA18260_AGC1_Gain_Free = 0,
#ifndef TDA18260A
    tmTDA18260_AGC1_Gain_Fixed_0dB = 2,
    tmTDA18260_AGC1_Gain_Fixed_3dB = 4,
#else
    tmTDA18260_AGC1_Gain_Fixed_minus2dB = 2,
    tmTDA18260_AGC1_Gain_Fixed_2dB = 4,
#endif
    tmTDA18260_AGC1_Gain_Fixed_4_875dB = 5,
    tmTDA18260_AGC1_Gain_Fixed_6_75dB = 6,
    tmTDA18260_AGC1_Gain_Fixed_8_625dB = 7,
    tmTDA18260_AGC1_Gain_Fixed_10_5dB = 8,
    tmTDA18260_AGC1_Gain_Fixed_12_375dB = 9,
    tmTDA18260_AGC1_Gain_Fixed_14_25dB = 10,
    tmTDA18260_AGC1_Gain_Fixed_16_125dB = 11,
    tmTDA18260_AGC1_Gain_Fixed_18dB = 12,
    tmTDA18260_AGC1_Gain_Max = 13
} tmTDA18260_AGC1_Gain_t, *ptmTDA18260_AGC1_Gain_t;
/* AGC1 Gain State */

typedef enum _tmTDA18260_IF_Level_t
{
    tmTDA18260_IF_Level_2V   = 0,
    tmTDA18260_IF_Level_1V   = 2,
    tmTDA18260_IF_Level_0_8V = 3,
    tmTDA18260_IF_Level_0_5V = 7,
    tmTDA18260_IF_Level_MAX = 8,
} tmTDA18260_IF_Level_t, *ptmTDA18260_IF_Level_t;

typedef enum _tmTDA18260_HPF_Freq_t
{
    tmTDA18260_HPF_Freq_400KHz = 0,
    tmTDA18260_HPF_Freq_850KHz = 1,
    tmTDA18260_HPF_Freq_1MHz = 2,
    tmTDA18260_HPF_Freq_1_5MHz = 3,
    tmTDA18260_HPF_Freq_MAX = 4,
} tmTDA18260_HPF_Freq_t, *ptmTDA18260_HPF_Freq_t;

typedef enum _tmTDA18260_StreamType_t
{
    tmTDA18260_StreamA = 0,
    tmTDA18260_StreamB = 1,
    tmTDA18260_StreamInvalid,
} tmTDA18260_StreamType_t, *ptmTDA18260_StreamType_t;

/* TDA18260 specific Standard parameters: */
typedef struct _tmTDA18260_StdParameters_t
{
    UInt32              IF;
    tmTDA18260_LP_Fc_t  lpFc;
    tmTDA18260_LP_Fc_Offset_t lpFcOffset;
    tmTDA18260_PD_t     hpBypass;
    UInt8               ifHpf;
} tmTDA18260_StdParameters_t, *ptmTDA18260_StdParameters_t;

/* AGC1 type for Power Level contribution */
typedef enum _tmTDA18260_AGC1_Type_t
{
    tmTDA18260_AGC1_MasterMulti = 0,
    tmTDA18260_AGC1_MasterSingle = 1,
    tmTDA18260_AGC1_Slave = 2,
    tmTDA18260_AGC1_Max = 3
} tmTDA18260_AGC1_Type_t, *ptmTDA18260_AGC1_Type_t;

/* TDA18260 Blocks Path configuration regarding channel Analog/Digital path */
typedef struct _tmTDA18260BlocksPath_t
{
    tmTDA18260_PD_t  pdAGC2a;
    tmTDA18260_PD_t  pdAGC2b;
    tmTDA18260_PD_t  PD_RFAGC_Det;
    tmTDA18260_PD_t  pdAGC2_DETECT2;
    tmTDA18260_PD_t  pdAGC2_DETECT3;
    tmTDA18260_PD_t  pdAGC2_DETECT4;
    tmTDA18260_PD_t  PD_RFAGC_Ifout;
    tmTDA18260_PD_t  PDBUF1b;
} tmTDA18260BlocksPath_t, *ptmTDA18260BlocksPath_t;


/* Power Level */
typedef enum _tmTDA18260Lvl_RSSI_t
{
    tmTDA18260_RSSI_X0 = 0,
    tmTDA18260_RSSI_X1 = 1,
    tmTDA18260_RSSI_Max = 2
} tmTDA18260Lvl_RSSI_t, *ptmTDA18260Lvl_RSSI_t;


typedef enum _tmTDA18260Lvl_RfFilter_t
{
    tmTDA18260_RfFilter_X0 = 0,
    tmTDA18260_RfFilter_X1 = 1,
    tmTDA18260_RfFilter_X2 = 2,
    tmTDA18260_RfFilter_RfMax = 3,
    tmTDA18260_RfFilter_X_Max = 4
} tmTDA18260Lvl_RfFilter_t, *ptmTDA18260Lvl_RfFilter_t;


typedef enum _tmTDA18260Lvl_AGC1_t
{
    tmTDA18260_AGC1_X0 = 0,
    tmTDA18260_AGC1_X1 = 1,
    tmTDA18260_AGC1_X2 = 2,
    tmTDA18260_AGC1_Code = 3,
    tmTDA18260_AGC1_X_Max = 4
} tmTDA18260Lvl_AGC1_t, *ptmTDA18260Lvl_AGC1_t;


typedef enum _tmTDA18260Lvl_AGC2b_t
{
    tmTDA18260_AGC2b_X0 = 0,
    tmTDA18260_AGC2b_X1 = 1,
    tmTDA18260_AGC2b_X2 = 2,
    tmTDA18260_AGC2b_Code = 3,
    tmTDA18260_AGC2b_X_Max = 4
} tmTDA18260Lvl_AGC2b_t, *ptmTDA18260Lvl_AGC2b_t;


typedef enum _tmTDA18260Lvl_Offset_t
{
    tmTDA18260_Offset_X0 = 0,
    tmTDA18260_Offset_X1 = 1,
    tmTDA18260_Offset_X_Max = 2
} tmTDA18260Lvl_Offset_t, *ptmTDA18260Lvl_Offset_t;

typedef enum _tmTDA18260Lvl_Xdiv10_t
{
    tmTDA18260_XDIV10_X0 = 0,
    tmTDA18260_XDIV10_X1 = 1,
    tmTDA18260_XDIV10_X2 = 2,
    tmTDA18260_XDIV10_SUM = 3,
    tmTDA18260_XDIV10_X_Max = 4
} tmTDA18260Lvl_Xdiv10_t, *ptmTDA18260Lvl_Xdiv10_t;

typedef enum _tmTDA18260Lvl_IndexDiv10_t
{
    tmTDA18260_DIV10_RF = 0,
    tmTDA18260_XDIV10_AGC2 = 1,
    tmTDA18260_XDIV10_AGC1 = 2,
    tmTDA18260_XDIV10_AGC1Multi = 3,
    tmTDA18260_XDIV10_OffsetSlave = 4,
    tmTDA18260_XDIV10_Index_Max = 5
} tmTDA18260Lvl_IndexDiv10_t, *ptmTDA18260Lvl_IndexDiv10_t;


/* TDA18260 specific Power State: */
typedef enum _tmTDA18260SharedPowerState_t
{
    tmTDA18260Shared_PowerOn = 0,                       /* TDA18260 powered on      (D0 state) */
    tmTDA18260Shared_PowerStandby,                      /* TDA18260 power standby   (D1 state) */
    tmTDA18260Shared_PowerSuspend,                      /* TDA18260 power suspended (D2 state) */
    tmTDA18260Shared_PowerOff,                          /* TDA18260 powered off     (D3 state) */
    tmTDA18260Shared_PowerMax
} tmTDA18260SharedPowerState_t, *ptmTDA18260SharedPowerState_t;


typedef enum _tmTDA18260StreamPowerState_t
{
    tmTDA18260Stream_PowerOn = 0,                       /* TDA18260 powered on      (D0 state) */
    tmTDA18260Stream_PowerStandby,                      /* TDA18260 power standby   (D1 state) */
    tmTDA18260Stream_PowerSuspend,                      /* TDA18260 power suspended (D2 state) */
    tmTDA18260Stream_PowerOff,                          /* TDA18260 powered off     (D3 state) */
    tmTDA18260Stream_PowerMax
} tmTDA18260StreamPowerState_t, *ptmTDA18260StreamPowerState_t;


/* Driver Instance definition */
typedef struct _tmbslTDA18260_Object_t {
    tmUnitSelect_t                  tUnit;
    tmUnitSelect_t                  tUnitW;
    ptmbslFrontEndMutexHandle       pMutex;
    Bool                            init;
    tmbslFrontEndIoFunc_t           sIo;
    tmbslFrontEndTimeFunc_t         sTime;
    tmbslFrontEndDebugFunc_t        sDebug;
    tmbslFrontEndMutexFunc_t        sMutex;
    tmTDA18260_SwitchState_t        lpfSwitchState;
    tmTDA18260_SwitchState_t        hpfSwitchState;
    tmTDA18260_SwitchState_t        bypassSwitchState;
    UInt32                          rf;
    UInt32                          freqCut;
    Bool                            resetCalledPreviously;
    tmTDA18260_PowerTransitionMode_t ePowerTransitionMode;
    tmUnitSelect_t                  tUnitOtherStream;
    tmTDA18260_StreamType_t         StreamType;
    /* Device specific part: */
    UInt16                          uIdentity[TDA18260_MAX_IDENTITIES];
    tmTDA18260SharedPowerState_t    curSharedPowerState;
    tmTDA18260StreamPowerState_t    curStreamPowerState;
    tmTDA18260_PowerState_t         minPowerState;
    tmTDA18260_DeviceType_t         deviceType;
    tmTDA18260_xtal_xtout_drive_t   xtout_drive;
    tmTDA18260_standardMode_t       standardMode;
    tmTDA18260_AGC1_Gain_t          agc1Gain;
    tmTDA18260_TimeCst_t            agc1TimeCst;
    tmTDA18260_TimeCst_t            ltTimeCst;
    tmTDA18260_IF_Level_t           ifLevel;
    tmTDA18260_State_t              MtoState;
    UInt8                           agc1Gud[tmTDA18260_AGC1_Gud_Max];
    UInt8                           agc1Aup[tmTDA18260_AGC1_Aup_Max];
    UInt8                           agc1Ado[tmTDA18260_AGC1_Ado_Max];
    UInt8                           agc2GdownDetector[tmTDA18260_AGC2_Detector4];
    tmTDA18260_StdParameters_t      stdArray[tmTDA18260_standardMode_Max];
    tmTDA18260_BandIndex_t          bandIndex;
    tmTDA18260_BandParameters_t     bandParam[tmTDA18260_IND_Band_Max];
    Bool                            bLoopThroughAlwaysOn;
    Bool                            bLockSharedPartAccess;
} tmbslTDA18260_Object_t, *ptmbslTDA18260_Object_t, **pptmbslTDA18260_Object_t;

/*============================================================================*/
/* Internal Prototypes:                                                       */
/*============================================================================*/
extern tmErrorCode_t
TDA18260_LockSharedPartAccess
(
    tmUnitSelect_t  tUnit     /* I: Unit number */
); 
   
extern tmErrorCode_t
TDA18260_ReleaseSharedPartAccess
(
    tmUnitSelect_t  tUnit     /* I: Unit number */
);

extern tmErrorCode_t
TDA18260_Reset
(
    tmUnitSelect_t  tUnit     /* I: Unit number */
);

extern tmErrorCode_t
TDA18260_IRCalibration
(
    tmUnitSelect_t tUnit        /* I: Unit number */
);

extern tmErrorCode_t
TDA18260_IRCAL_InitRestore
(
    tmUnitSelect_t          tUnit,       /* I: Unit number */
    tmTDA18260_Step_t       uIRStep      /* I: IRCAL Step  */
);

extern tmErrorCode_t
TDA18260_SignalCalibration
(
    tmUnitSelect_t          tUnit,       /* I: Unit number */
    UInt8                   uIR_Freq,    /* I: Freq step   */
    UInt32                  uLO          /* I: LO          */
);

extern tmErrorCode_t
TDA18260_ImageCalibration
(
    tmUnitSelect_t          tUnit        /* I: Unit number */
);

extern tmErrorCode_t
TDA18260_ResetAgc1
(
    tmUnitSelect_t   tUnit      /* I: Unit number */
);

extern tmErrorCode_t
TDA18260_ResetAgc2Stream
(
    tmUnitSelect_t   tUnit    /* I: Unit number */
);

extern tmErrorCode_t
TDA18260_WriteAgc1Top
(
    tmUnitSelect_t   tUnit      /* I: Unit number */
);

extern tmErrorCode_t
TDA18260_WriteAgc2Top
(
    tmUnitSelect_t   tUnit    /* I: Unit number */
);

extern tmErrorCode_t
TDA18260_SetLOFreq
(
    tmUnitSelect_t  tUnit,      /* I: Unit number                       */
    UInt32          uLO         /* I: local oscillator frequency in Hz  */
);

extern tmErrorCode_t
TDA18260_RF2BandIndex
(
    tmUnitSelect_t         tUnit,       /* I: Unit number           */
    UInt32                 uRF,         /* I: RF frequency in hertz */
    tmTDA18260_BandIndex_t *pBandIndex  /* O: Band Index            */
);

extern tmErrorCode_t
TDA18260_WriteAgc2To0dB
(
    tmUnitSelect_t          tUnit,        /* I: Unit number       */
    tmTDA18260_AGC2Step3dB  agc2Step3dB   /* I: Switch Smooth used */
);

extern tmErrorCode_t
TDA18260_SetAGC1_loop_off
(
    tmUnitSelect_t          tUnit,      /*  I: Unit number */
    tmTDA18260_State_t      uState      /* I: State value */
);

extern tmErrorCode_t
TDA18260_SetSwitchOpen
(
    tmUnitSelect_t  tUnit          /* I: Unit number */
);

extern tmErrorCode_t
TDA18260_SetSwitchClose
(
    tmUnitSelect_t            tUnit,         /* I: Unit number        */
    tmTDA18260_BandIndex_t    uNewBandIndex  /* I: Band Index         */
);

extern tmErrorCode_t
TDA18260_SwitchDirectAccess
(
    tmUnitSelect_t           tUnit,        /* I: Unit number  */
    tmTDA18260_Switch_t      Switch,       /* I: Switch Name  */
    tmTDA18260_SwitchState_t SwitchState   /* I: Switch State */
);

extern tmErrorCode_t
TDA18260_SwitchSmoothAccess
(
    tmUnitSelect_t           tUnit,       /* I: Unit number  */
    tmTDA18260_Switch_t      Switch,      /* I: Switch Name  */
    tmTDA18260_SwitchState_t SwitchState  /* I: Switch State */
);

extern tmErrorCode_t
TDA18260_WriteRF_Filters
(
    tmUnitSelect_t         tUnit,         /* I: Unit number */
    tmTDA18260_BandIndex_t uNewBandIndex  /* I: Band Index  */
);

extern tmErrorCode_t
TDA18260_SetStandardMode
(
    tmUnitSelect_t tUnit        /* I: Unit number */
);

extern tmErrorCode_t
TDA18260_SetPath
(
    tmUnitSelect_t         tUnit,        /* I: Unit number */
    tmTDA18260_BandIndex_t NewBandIndex  /* I: Band Index  */
);

extern tmErrorCode_t
TDA18260_GetRssi
(
    tmUnitSelect_t  tUnit,      /* I: Unit number */
    UInt8*          pRSSI       /* O: RSSI value  */
);

extern Int32
TDA18260_CalcPow
(
    Int32 uX,     /* I: value  */
    Int32 uY      /* I: value  */
);

extern tmErrorCode_t
TDA18260_LvlAgc2Contribution
(
    UInt8       uAGC2b,         /* I: AGC2 gain          */
    UInt32      uRF,            /* I: RF frequency (MHz) */
    Int32*      pPowerLevel     /* I/O: PowerLevel       */
);

extern tmErrorCode_t
TDA18260_LvlAgc1Contribution
(
    tmTDA18260_AGC1_Gain_t uAGC1,           /* I: AGC1 gain          */
    UInt32                   uRF,           /* I: RF frequency (MHz) */
    Int32*                   pPowerLevel    /* I/O: PowerLevel       */
);

extern tmErrorCode_t
TDA18260_LvlAgc1MasterMultiContribution
(
    tmTDA18260_AGC1_Gain_t uAGC1,           /* I: AGC1 gain          */
    UInt32                   uRF,           /* I: RF frequency (MHz) */
    Int32*                   pPowerLevel    /* I/O: PowerLevel       */
);

extern tmErrorCode_t
TDA18260_GetPowerLevel
(
   tmUnitSelect_t  tUnit,        /* I: Unit number                                  */
   UInt32          IF,           /* I: Intermediate Frequency                       */
   UInt8           uRSSIValue,   /* I: RSSI value reading previously                */
   Int32*          pRFEVAL       /* O: Address of the variable to output item value */
);

extern tmErrorCode_t
TDA18260Shared_SetPowerState
(
    tmUnitSelect_t                tUnit,      /* I: Unit number             */
    tmTDA18260SharedPowerState_t  powerState  /* I: Power state of TDA18260 */
);

extern tmErrorCode_t
TDA18260Shared_GetPowerState
(
    tmUnitSelect_t                  tUnit,      /* I: Unit number             */
    tmTDA18260SharedPowerState_t*   pPowerState /* O: Power state of TDA18260 */
);

extern tmErrorCode_t
TDA18260Stream_SetPowerState
(
    tmUnitSelect_t                tUnit,      /* I: Unit number             */
    tmTDA18260StreamPowerState_t  powerState  /* I: Power state of TDA18260 */
);

extern tmErrorCode_t
TDA18260Stream_GetPowerState
(
    tmUnitSelect_t                  tUnit,      /* I: Unit number             */
    tmTDA18260StreamPowerState_t*   pPowerState /* O: Power state of TDA18260 */
);

extern tmTDA18260_StreamType_t 
TDA18260_GetStreamType
( 
    tmUnitSelect_t tUnit
);

tmErrorCode_t
TDA18260Stream_SetSmOn
(
    tmUnitSelect_t tUnit    /* I: Unit number */
);

tmErrorCode_t
TDA18260Stream_SetSmOff
(
    tmUnitSelect_t tUnit    /* I: Unit number */
);

extern tmErrorCode_t
TDA18260Stream_SetLoFreq
(
    tmUnitSelect_t  tUnit,      /* I: Unit number                       */
    UInt32          uLO,          /* I: local oscillator frequency in Hz  */
    Bool            bForceIcpLow  /* I: indicate whether the Icp is forced to low */
);

extern tmErrorCode_t
TDA18260Stream_GetLoFreq
(
    tmUnitSelect_t  tUnit,      /* I: Unit number                       */
    UInt32          *puLO       /* O: local oscillator frequency in Hz  */
);

extern tmErrorCode_t
TDA18260_WriteAGC2To0dB
(
    tmUnitSelect_t          tUnit,        /* I: Unit number        */
    tmTDA18260_AGC2Step3dB  eAGC2Step3dB  /* I: Switch Smooth used */
);

extern tmTDA18260_StreamType_t GetStreamType( tmUnitSelect_t tUnit );

extern tmErrorCode_t TDA18260_Write(ptmbslTDA18260_Object_t pObj, UInt8 uSubAddress, UInt8 uNbData, UInt8* pData);
extern tmErrorCode_t TDA18260_Read(ptmbslTDA18260_Object_t pObj, UInt8 uSubAddress, UInt8 uNbData, UInt8* pData);
extern tmErrorCode_t TDA18260_Wait(ptmbslTDA18260_Object_t pObj, UInt32 Time);

extern tmErrorCode_t TDA18260_MutexAcquire(ptmbslTDA18260_Object_t pObj, UInt32 timeOut);
extern tmErrorCode_t TDA18260_MutexRelease(ptmbslTDA18260_Object_t pObj);

#ifdef __cplusplus
}
#endif

#endif /* _TMBSL_TDA18260_LOCAL_H */

