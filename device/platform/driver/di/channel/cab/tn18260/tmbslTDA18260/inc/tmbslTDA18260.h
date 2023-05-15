/*
  Copyright (C) 2006-2009 NXP B.V., All Rights Reserved.
  This source code and any compilation or derivative thereof is the proprietary
  information of NXP B.V. and is confidential in nature. Under no circumstances
  is this software to be  exposed to or placed under an Open Source License of
  any type without the expressed written permission of NXP B.V.
 *
 * \file          tmbslTDA18260.h
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

/* File generated automatically from register description file */

#ifndef _TMBSL_TDA18260_H
#define _TMBSL_TDA18260_H

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*/
/* TDA18260 Error Codes                                                       */
/*============================================================================*/

#define TDA18260_ERR_BASE                       (CID_COMP_TUNER | CID_LAYER_BSL)
#define TDA18260_ERR_COMP                       (CID_COMP_TUNER | CID_LAYER_BSL | TM_ERR_COMP_UNIQUE_START)

#define TDA18260_ERR_BAD_UNIT_NUMBER            (TDA18260_ERR_BASE + TM_ERR_BAD_UNIT_NUMBER)
#define TDA18260_ERR_NOT_INITIALIZED            (TDA18260_ERR_BASE + TM_ERR_NOT_INITIALIZED)
#define TDA18260_ERR_ALREADY_SETUP              (TDA18260_ERR_BASE + TM_ERR_ALREADY_SETUP)
#define TDA18260_ERR_INIT_FAILED                (TDA18260_ERR_BASE + TM_ERR_INIT_FAILED)
#define TDA18260_ERR_BAD_PARAMETER              (TDA18260_ERR_BASE + TM_ERR_BAD_PARAMETER)
#define TDA18260_ERR_NOT_SUPPORTED              (TDA18260_ERR_BASE + TM_ERR_NOT_SUPPORTED)
#define TDA18260_ERR_HW_FAILED                  (TDA18260_ERR_COMP + 0x0001)
#define TDA18260_ERR_NOT_READY                  (TDA18260_ERR_COMP + 0x0002)
#define TDA18260_ERR_BAD_VERSION                (TDA18260_ERR_COMP + 0x0003)
#define TDA18260_ERR_POWER_LEVEL_TOO_LOW        (TDA18260_ERR_COMP + 0x0004)
#define TDA18260_ERR_POWER_LEVEL_TOO_HIGH       (TDA18260_ERR_COMP + 0x0005)

/*============================================================================*/
/* Types and defines:                                                         */
/*============================================================================*/

/* TDA18260 specific Power State: */
typedef enum _tmTDA18260_PowerState_t
{
    tmTDA18260_PowerOn = 0,                         /* TDA18260 powered on      (D0 state) */
    tmTDA18260_PowerStandby = 1,                    /* TDA18260 power standby   (D1 state) - loop through enable */
    tmTDA18260_PowerSuspend = 2,                    /* TDA18260 power suspended (D2 state) - standby + loop through disable */
    tmTDA18260_PowerOff = 3,                        /* TDA18260 powered off     (D3 state) */
    tmTDA18260_PowerMax = 4
} tmTDA18260_PowerState_t, *ptmTDA18260_PowerState_t;

/* TDA18260 specific Power State: */
typedef enum _tmTDA18260_PowerTransitionMode_t
{
    tmTDA18260_PowerTransitionMode_Regular = 0,
    tmTDA18260_PowerTransitionMode_Smooth,
    tmTDA18260_PowerTransitionMode_Invalid
} tmTDA18260_PowerTransitionMode_t, *ptmTDA18260_PowerTransitionMode_t;

/* TDA18260 specific Standard modes: */
typedef enum _tmTDA18260_standardMode_t
{
    tmTDA18260_DIG_6MHz = 0,                        /* Digital TV DVB-C 6MHz */
    tmTDA18260_DIG_7MHz = 1,                        /* Digital TV DVB-C 7MHz */
    tmTDA18260_DIG_8MHz = 2,                        /* Digital TV DVB-C 8MHz */
    tmTDA18260_DIG_9MHz = 3,                        /* Digital TV DVB-C 9MHz */
    tmTDA18260_standardMode_Max
} tmTDA18260_standardMode_t, *ptmTDA18260_standardMode_t;

/* TDA18260 specific LPFc: */
typedef enum _tmTDA18260_LP_Fc_t
{
    tmTDA18260_LPF_6MHz = 0,    /* 6MHz LPFc */
    tmTDA18260_LPF_7MHz = 1,    /* 7MHz LPFc */
    tmTDA18260_LPF_8MHz = 2,    /* 8MHz LPFc */
    tmTDA18260_LPF_9MHz = 3,    /* 9MHz LPFc */
    tmTDA18260_LPF_Max = 4
} tmTDA18260_LP_Fc_t, *ptmTDA18260_LP_Fc_t;

/* TDA18260 specific LPFc offset: */
typedef enum _tmTDA18260_LP_Fc_Offset_t
{
    tmTDA18260_LPF_Offset_Minus3 = 0, /* -3 */
    tmTDA18260_LPF_Offset_Minus2,     /* -2 */
    tmTDA18260_LPF_Offset_Minus1,     /* -1 */
    tmTDA18260_LPF_Offset_0,          /*  0 */
    tmTDA18260_LPF_Offset_Plus1,      /*  1 */
    tmTDA18260_LPF_Offset_Plus2,      /*  2 */
    tmTDA18260_LPF_Offset_Plus3,      /*  3 */
    tmTDA18260_LPF_Offset_Invalid,    /* Invalid Value */
} tmTDA18260_LP_Fc_Offset_t, *ptmTDA18260_LP_Fc_Offset_t;

typedef enum _tmTDA18260_AGC1_Gud_t
{
    tmTDA18260_AGC1_Gud_Up = 0,
    tmTDA18260_AGC1_Gud_Down = 1,
    tmTDA18260_AGC1_Gud_Max = 2
} tmTDA18260_AGC1_Gud_t, *ptmTDA18260_AGC1_Gud_t;

typedef enum _tmTDA18260_AGC1_Aup_t
{
    tmTDA18260_AGC1_Aup_0dB = 0,
    tmTDA18260_AGC1_Aup_3dB = 1,
    tmTDA18260_AGC1_Aup_4_5dB = 2,
    tmTDA18260_AGC1_Aup_6dB = 3,
    tmTDA18260_AGC1_Aup_7_5dB = 4,
    tmTDA18260_AGC1_Aup_9dB = 5,
    tmTDA18260_AGC1_Aup_10_5dB = 6,
    tmTDA18260_AGC1_Aup_12dB = 7,
    tmTDA18260_AGC1_Aup_13_5dB = 8,
    tmTDA18260_AGC1_Aup_Max = 9
} tmTDA18260_AGC1_Aup_t, *ptmTDA18260_AGC1_Aup_t;

typedef enum _tmTDA18260_AGC1_Ado_t
{
    tmTDA18260_AGC1_Ado_3dB = 0,
    tmTDA18260_AGC1_Ado_4_5dB = 1,
    tmTDA18260_AGC1_Ado_6dB = 2,
    tmTDA18260_AGC1_Ado_7_5dB = 3,
    tmTDA18260_AGC1_Ado_9dB = 4,
    tmTDA18260_AGC1_Ado_10_5dB = 5,
    tmTDA18260_AGC1_Ado_12dB = 6,
    tmTDA18260_AGC1_Ado_13_5dB = 7,
    tmTDA18260_AGC1_Ado_15dB = 8,
    tmTDA18260_AGC1_Ado_Max = 9
} tmTDA18260_AGC1_Ado_t, *ptmTDA18260_AGC1_Ado_t;

typedef enum _tmTDA18260_BpFilter_t
{
    tmTDA18260_BP_Filter0 = 0,
    tmTDA18260_BP_Filter1 = 1,
    tmTDA18260_BP_Filter2 = 2,
    tmTDA18260_BP_Filter3 = 3,
    tmTDA18260_BP_Filter4 = 4,
    tmTDA18260_BP_Filter5 = 5,
    tmTDA18260_BP_Filter6 = 6,
    tmTDA18260_BP_Filter7 = 7,
    tmTDA18260_BP_Filter_Max = 8
} tmTDA18260_BpFilter_t, *ptmTDA18260_BpFilter_t;

typedef enum _tmTDA18260_AGC2_Detector_t
{
    tmTDA18260_AGC2_Detector2 = 1,
    tmTDA18260_AGC2_Detector3 = 2,
    tmTDA18260_AGC2_Detector4 = 3,
    tmTDA18260_AGC2_Detector_Max = 4
} tmTDA18260_AGC2_Detector_t, *ptmTDA18260_AGC2_Detector_t;

/* TDA18260 specific device type: */
typedef enum _tmTDA18260_DeviceType_t {
    tmTDA18260_DeviceTypeMaster = 0,                /* TDA18260 Master Device */
    tmTDA18260_DeviceTypeMasterWithoutXTout = 1,    /* TDA18260 Master Device Without external clock */
    tmTDA18260_DeviceTypeSlave = 2,                 /* TDA18260 Slave Device */
    tmTDA18260_DeviceTypeSlaveWithXTout = 3,        /* TDA18260 Slave Device With external clock */
    tmTDA18260_DeviceTypeMax = 4
} tmTDA18260_DeviceType_t, *ptmTDA18260_DeviceType_t;

/* TDA18260 Step */
typedef enum _tmTDA18260_Step_t
{
    tmTDA18260_INIT = 0,
    tmTDA18260_RESTORE = 1,
    tmTDA18260_Step_Max = 2
} tmTDA18260_Step_t, *ptmTDA18260_Step_t;

/* Switch Name */
typedef enum _tmTDA18260_Switch_t
{
    tmTDA18260_RFSW_LP = 1,
    tmTDA18260_RFSW_HP = 2,
    tmTDA18260_RFSW_BYPASS = 3,
    tmTDA18260_SW_Type_Max = 4
} tmTDA18260_Switch_t, *ptmTDA18260_Switch_t;

typedef enum _tmTDA18260_AGC2Step3dB
{
    tmTDA18260_AGC2Step3dB_OFF = 0,
    tmTDA18260_AGC2Step3dB_ON = 1,
    tmTDA18260_AGC2Step3dB_Max = 2
} tmTDA18260_AGC2Step3dB, *ptmTDA18260_AGC2Step3dB;

/* TDA18260 State */
typedef enum _tmTDA18260_State_t
{
    tmTDA18260_OFF = 0,
    tmTDA18260_ON = 1,
    tmTDA18260_State_Max = 2
} tmTDA18260_State_t, *ptmTDA18260_State_t;

/* Power Down State */
typedef enum _tmTDA18260_PD_t
{
    tmTDA18260_PD_NotActivated = 0,                               
    tmTDA18260_PD_Activated = 1,              
    tmTDA18260_PD_Max = 2                
} tmTDA18260_PD_t, *ptmTDA18260_PD_t;

/* Time transition */
/* NOTES:       000 : no smooth transition      100 : 8   */
/*              001: 1                          101 : 16  */
/*              010: 2                          110 : 32  */
/*              011: 4                          111 : 200 */
typedef enum _tmTDA18260_TimeCst_t
{
    tmTDA18260_TCST_0 = 0,
    tmTDA18260_TCST_1 = 1,
    tmTDA18260_TCST_2 = 2,
    tmTDA18260_TCST_4 = 3,
    tmTDA18260_TCST_8 = 4,
    tmTDA18260_TCST_16 = 5,
    tmTDA18260_TCST_32 = 6,
    tmTDA18260_TCST_200 = 7,
    tmTDA18260_TCST_MAX =8
} tmTDA18260_TimeCst_t, *ptmTDA18260_TimeCst_t;


/* Switch State */
typedef enum _tmTDA18260_SwitchState_t
{
    tmTDA18260_SW_OPEN  = 0,
    tmTDA18260_SW_CLOSE = 1,
    tmTDA18260_SW_State_Max = 2
} tmTDA18260_SwitchState_t, *ptmTDA18260_SwitchState_t;

typedef enum _tmTDA18260_BlockType_t
{
    tmTDA18260_BlockShared = 0,
    tmTDA18260_BlockStream = 1,
    tmTDA18260_BlockMax = 2
} tmTDA18260_BlockType_t, *ptmTDA18260_BlockType_t;

typedef struct _tmTDA18260_BandP_t
{
    UInt32                  RfMax;          /* LoMax-IF (Hz) */
    tmTDA18260_Switch_t     Switch;
    tmTDA18260_SwitchState_t SwitchState;
    UInt8                   Ind_Div2;
    
    UInt8                   BP_Ind1;
    UInt8                   Cap_HP_Left;
    UInt8                   Cap_HP_Right;
    
    UInt8                   Cap_HP_Mid;
    UInt8                   Cap_L10n;
    UInt8                   Cap_L6n;
    UInt8                   BP_Ind2;
    
    UInt8                   DTA_notMOCA;
    
    tmTDA18260_PD_t         pd_AGC1bis;
    UInt8                   AGC1bis_gain;
    
    UInt8                   BP_Filter;
} tmTDA18260_BandP_t, *ptmTDA18260_BandP_t;

typedef enum _tmTDA18260_FilterType_t
{
    tmTDA18260_FILTER_BYPASS = 0,
    tmTDA18260_FILTER_LPF = 1,
    tmTDA18260_FILTER_BPF = 2,
    tmTDA18260_FILTER_HPF = 3,
    tmTDA18260_FILTER_ANLG = 4,
    tmTDA18260_FILTER_Max = 5
} tmTDA18260_FilterType_t, *ptmTDA18260_FilterType_t;

typedef struct _tmTDA18260_Filter_t
{
    tmTDA18260_FilterType_t type;
    UInt8                   number;
} tmTDA18260_Filter_t, *ptmTDA18260_Filter_t;

typedef struct _tmTDA18260_BandParameters_t
{
    tmTDA18260_Filter_t       filter;
    UInt32                    rfMax;          /* LoMax-IF (Hz) */
    tmTDA18260_Switch_t       Switch;
    UInt8                     indDiv2;
    UInt8                     capHpLeft;
    UInt8                     capHpMid;
    UInt8                     capHpRight;
    UInt8                     bpInd1;
    UInt8                     bpInd2;
    UInt8                     capL6n;
    UInt8                     capL10n;
    UInt8                     rfFilterSwitch;
    tmTDA18260_PD_t           pdAgc1Bis;
    UInt8                     agc1BisGain;
    UInt8                     bpFilter;
    UInt8                     stobGain;
    UInt8                     stobDC;
    UInt8                     psmStob;
    UInt8                     psmBuf1b;
} tmTDA18260_BandParameters_t, *ptmTDA18260_BandParameters_t;


typedef enum _tmTDA18260_Stream_t
{
    tmTDA18260_Stream_A,
    tmTDA18260_Stream_B,
    tmTDA18260_Stream_Invalid,
} tmTDA18260_Stream_t, *ptmTDA18260_Stream_t;

typedef enum _tmTDA18260_AGC_Mode_t
{
    tmTDA18260_AGC_Mode_Free = 0,              
    tmTDA18260_AGC_Mode_forced = 1,              
    tmTDA18260_AGC_Mode_Max = 2
} tmTDA18260_AGC_Mode_t, *ptmTDA18260_AGC_Mode_t;

/*============================================================================*/
/* Exported functions:                                                        */
/*============================================================================*/
tmErrorCode_t
tmbslTDA18260_Init
(
    tmUnitSelect_t              tUnit,      /* I: Unit number      */
    tmbslFrontEndDependency_t*  psSrvFunc   /* I: setup parameters */
);

tmErrorCode_t 
tmbslTDA18260_DeInit
(
    tmUnitSelect_t  tUnit   /* I: Unit number */
);

tmErrorCode_t
tmbslTDA18260_GetSWVersion
(
    ptmSWVersion_t  pSWVersion  /* O: Receives SW Version */
);

tmErrorCode_t
tmbslTDA18260_GetLLSWVersion
(
    ptmSWVersion_t  pSWVersion  /* O: Receives SW Version */
);

tmErrorCode_t
tmbslTDA18260_GetLL2SWVersion
(
    ptmSWVersion_t  pSWVersion  /* O: Receives SW Version */
);

tmErrorCode_t
tmbslTDA18260_GetSWSettingsVersion
(
    ptmSWSettingsVersion_t pSWSettingsVersion   /* O: Receives SW Settings Version */
);

tmErrorCode_t
tmbslTDA18260_GetLLSWSettingsVersion
(
    ptmSWSettingsVersion_t pSWSettingsVersion   /* O: Receives SW Settings Version */
);

tmErrorCode_t
tmbslTDA18260_GetLL2SWSettingsVersion
(
    ptmSWSettingsVersion_t pSWSettingsVersion   /* O: Receives SW Settings Version */
);

tmErrorCode_t
tmbslTDA18260_CheckHWVersion
(
    tmUnitSelect_t  tUnit   /* I: Unit number */
);

tmErrorCode_t
tmbslTDA18260_SetPowerTransitionMode
(
    tmUnitSelect_t                tUnit,      /* I: Unit number                   */
    tmTDA18260_PowerTransitionMode_t  ePowerTransitionMode  /* I: Power state mode              */
);

tmErrorCode_t
tmbslTDA18260_SetPowerState
(
    tmUnitSelect_t          tUnit,      /* I: Unit number                   */
    tmTDA18260_PowerState_t powerState  /* I: Power state of TDA18260       */
);

tmErrorCode_t
tmbslTDA18260_GetPowerState
(
    tmUnitSelect_t           tUnit,      /* I: Unit number */
    tmTDA18260_BlockType_t   blockType,  /* I: Block Type: Shaed, Stream A/B */
    tmTDA18260_PowerState_t* pPowerState /* O: Power state of TDA18260 */
);

tmErrorCode_t
tmbslTDA18260_Reset
(
    tmUnitSelect_t  tUnit   /* I: Unit number */
);

tmErrorCode_t
tmbslTDA18260_SetStandardMode
(
    tmUnitSelect_t              tUnit,          /* I: Unit number                  */
    tmTDA18260_standardMode_t   standardMode    /* I: Standard mode of this device */
);

tmErrorCode_t
tmbslTDA18260_GetStandardMode
(
    tmUnitSelect_t               tUnit,          /* I: Unit number */
    tmTDA18260_standardMode_t    *pStandardMode  /* O: Standard mode of this device */
);

tmErrorCode_t
tmbslTDA18260_SetRf
(
    tmUnitSelect_t  tUnit,       /* I: Unit number           */
    UInt32          uRF          /* I: RF frequency in hertz */
);

tmErrorCode_t
tmbslTDA18260_GetRf
(
    tmUnitSelect_t  tUnit,  /* I: Unit number           */
    UInt32*         puRF    /* O: IF Frequency in hertz */
);

tmErrorCode_t
tmbslTDA18260_GetIF
(
    tmUnitSelect_t  tUnit,  /* I: Unit number           */
    UInt32*         puIF    /* O: IF Frequency in hertz */
);

tmErrorCode_t
tmbslTDA18260_GetLockStatus
(
    tmUnitSelect_t          tUnit,          /* I: Unit number     */
    tmbslFrontEndState_t*   pLockStatus     /* O: PLL Lock status */
);

tmErrorCode_t
tmbslTDA18260_GetPowerLevel
(
    tmUnitSelect_t  tUnit,      /* I: Unit number         */
    Int32*          pPowerLevel /* O: Power Level in dBmV */
);

tmErrorCode_t
tmbslTDA18260_SetXtOutOn
(
    tmUnitSelect_t   tUnit,    /* I: Unit number */
    Bool             bOn       /* I: false -> XtOut off, true -> XtOut on */
);

tmErrorCode_t
tmbslTDA18260_AGC1Lock
(
    tmUnitSelect_t         tUnit,    /* I: Unit number */
    tmTDA18260_AGC_Mode_t  bAgcMode  /* I: true -> AGC1 is locked to current value, false -> AGC1 is not locked (default configuration) */
);

#ifdef __cplusplus
}
#endif

#endif /* _TMBSL_TDA18260_H */

