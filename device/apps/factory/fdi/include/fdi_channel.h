/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Error Code Definition
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FDI_CHANNEL_H_
#define _FDI_CHANNEL_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fdi_err.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
/**
 * Define which type of tuner.
 */
typedef enum
{
	FDI_CH_TYPE_SAT = 0x01,
	FDI_CH_TYPE_TER = 0x02,
	FDI_CH_TYPE_CAB = 0x04,
	FDI_CH_TYPE_ANALOG = 0x08
} FDI_CH_TYPE_e;

/**
 * Define signal info which is used for callback param.
 */
typedef enum
{
	FDI_CH_SIGNAL_LOCKED			= 0,	/**< Carrier locked */
	FDI_CH_SIGNAL_UNLOCK			= 1,	/**< Carrier unlocked */
	FDI_CH_SIGNAL_ANTENNA_FAULT		= 2,	/**< LNB fault or Antenna fault */
	FDI_CH_SIGNAL_ANTENNA_OK		= 3		/**< LNB or Antenna fault is recovered */
} FDI_CH_SIGNAL_INFO_e;

///////////////////////////////////////////////////////////////////////////////
// sat
///////////////////////////////////////////////////////////////////////////////
typedef enum
{
	FDI_CH_ANT_TYPE_LNB		= 0,	/**< LNB only */
	FDI_CH_ANT_TYPE_DISEQC	= 1,	/**< DiSEqC switch */
	FDI_CH_ANT_TYPE_SCD		= 2,	/**< Single Cable Distribution */
	FDI_CH_ANT_TYPE_SMATV	= 3		/**< SMATV */
} FDI_CH_SAT_ANTENNATYPE_e;

/*	Polarization */
typedef enum
{
	FDI_CH_POLAR_AUTO	= 0,
	FDI_CH_POLAR_HOR	= 1,
	FDI_CH_POLAR_VER	= 2,
	FDI_CH_POLAR_LEFT	= 3,
	FDI_CH_POLAR_RIGHT	= 4,
	FDI_CH_POLAR_MAX = 5
} FDI_CH_POLARIZATION_e;

/* Code Rate (FEC ) */
//for DBV-S2
typedef enum
{
	FDI_CH_SCODERATE_AUTO	= 0,
	FDI_CH_SCODERATE_1_2	= 1,
	FDI_CH_SCODERATE_2_3	= 2,
	FDI_CH_SCODERATE_3_4	= 3,
	FDI_CH_SCODERATE_5_6	= 4,
	FDI_CH_SCODERATE_7_8	= 5,
	FDI_CH_S2CODERATE_3_5	= 6,
	FDI_CH_S2CODERATE_4_5	= 7,
	FDI_CH_S2CODERATE_8_9	= 8,
	FDI_CH_S2CODERATE_9_10	= 9,
	FDI_CH_SCODERATE_5_11	= 10,
	FDI_CH_SCODERATE_6_7	= 11
} FDI_CH_CODERATE_e;

/* LNB Setting  */
typedef enum
{
	FDI_CH_LNB_VOLT_STD		= 0,		/**< Vertical : 11v, Horizontal 15v */
	FDI_CH_LNB_VOLT_HIGH	= 1			/**< Vertical : 12v, Horizontal 16v */
} FDI_CH_LNB_VOLTAGE_e;

/* DiSEqC input */
typedef enum
{
	FDI_CH_DISEQC_INPUT_NONE	= 0,
	FDI_CH_DISEQC_INPUT_A		= 1,
	FDI_CH_DISEQC_INPUT_B		= 2,
	FDI_CH_DISEQC_INPUT_C		= 3,
	FDI_CH_DISEQC_INPUT_D		= 4,
	FDI_CH_DISEQC_INPUT_TB_A	= 5,
	FDI_CH_DISEQC_INPUT_TB_B	= 6,
	FDI_CH_DISEQC_INPUT_SCD_A 	= 7,
	FDI_CH_DISEQC_INPUT_SCD_B	= 8
} FDI_CH_DISEQ_INPUT_e;

/* DiSEqC version */
typedef enum
{
	FDI_CH_DISEQC_VER_1_0 = 0,		/**< which allows switching between up to 4 satellite sources */
	FDI_CH_DISEQC_VER_1_1 = 1,		/**< which allows switching between up to 16 sources */
	FDI_CH_DISEQC_VER_1_2 = 2,		/**< which allows switching between up to 16 sources, and control of a simple horizontal-panning satellite motor */
	FDI_CH_DISEQC_VER_1_3 = 3,		/**< Universal Satellites Automatic Location System (USALS) */
	FDI_CH_DISEQC_VER_2_0 = 4		/**< which adds bi-directional communications to DiSEqC 1.0 */
} FDI_CH_DISEQC_VER_e;

/* Transition Spec */
//for DBV-S2
typedef enum
{
	FDI_CH_TRANS_DVBS	= 0,
	FDI_CH_TRANS_DVBS2	= 1
} FDI_CH_TRANSPORT_SPEC_e;

/* Modulation Type */
//for DBV-S2
typedef enum
{
	FDI_CH_PSK_AUTO = 0,
	FDI_CH_PSK_QPSK = 1,
	FDI_CH_PSK_8PSK = 2
} FDI_CH_MODULATION_e;

/* Pilot On-Off */
//for DBV-S2
typedef enum
{
    FDI_CH_PILOT_AUTO	= 0,
    FDI_CH_PILOT_OFF	= 1,
    FDI_CH_PILOT_ON		= 2
} FDI_CH_PILOT_e;

/*ROLL */
typedef enum
{
	FDI_CH_ROLL_020 = 0,
	FDI_CH_ROLL_025 = 1,
	FDI_CH_ROLL_035 = 2
} FDI_CH_ROLLOFF_e;

typedef enum
{
	FDI_CH_TONE_BURST_NONE 	= 0,			/**< Tone burst : None */
	FDI_CH_TONE_BURST_A		= 1,			/**< Tone burst : A */
	FDI_CH_TONE_BURST_B		= 2				/**< Tone burst : B */
} FDI_CH_TONE_BURST_e;

typedef enum
{
	FDI_CH_POWER_OFF = 0,
	FDI_CH_POWER_ON
} FDI_CH_POWER_e;

///////////////////////////////////////////////////////////////////////////////
// cab
///////////////////////////////////////////////////////////////////////////////
typedef enum
{
	FDI_CH_CMOD_AUTO	= 0, 		/* Auto order : 64-> 256-> 128-> 32-> 16 */
	FDI_CH_CMOD_16QAM	= 1,
	FDI_CH_CMOD_32QAM	= 2,
	FDI_CH_CMOD_64QAM	= 3,
	FDI_CH_CMOD_128QAM	= 4,
	FDI_CH_CMOD_256QAM	= 5
} FDI_CH_CAB_MODULATION_e;

typedef enum
{
	FDI_CH_INVERSION_AUTO		= 0,
	FDI_CH_INVERSION_NORMAL		= 1,
	FDI_CH_INVERSION_INVERTED	= 2
} FDI_CH_CAB_SPECTRUM_e;

///////////////////////////////////////////////////////////////////////////////
// ter
///////////////////////////////////////////////////////////////////////////////
typedef enum
{
	FDI_CH_BW_8MHZ = 0,
	FDI_CH_BW_7MHZ = 1,
	FDI_CH_BW_6MHZ = 2,
	FDI_CH_BW_5MHZ = 3,
	FDI_CH_BW_10MHZ = 4,
	FDI_CH_BW_1P7MHZ = 5
} FDI_CH_TER_BANDWIDTH_e;

typedef enum
{
	FDI_CH_TMOD_AUTO		= 0,
	FDI_CH_TMOD_QPSK		= 1,
	FDI_CH_TMOD_16QAM		= 2,
	FDI_CH_TMOD_64QAM		= 3,
	FDI_CH_TMOD_256QAM		= 4,
} FDI_CH_TER_MODULATION_e;

typedef enum
{
	FDI_CH_HIERARCHY_AUTO 	= 0,
	FDI_CH_HIERARCHY_NONE	= 1,
	FDI_CH_HIERARCHY_1		= 2,
	FDI_CH_HIERARCHY_2		= 3,
	FDI_CH_HIERARCHY_4		= 4
} FDI_CH_TER_HIERARCHY_e;

typedef enum
{
	FDI_CH_TCODERATE_AUTO 	= 0,
	FDI_CH_TCODERATE_1_2		= 1,
	FDI_CH_TCODERATE_2_3		= 2,
	FDI_CH_TCODERATE_3_4		= 3,
	FDI_CH_TCODERATE_5_6		= 4,
	FDI_CH_TCODERATE_7_8		= 5,
	FDI_CH_T2CODERATE_3_5		= 6,
	FDI_CH_T2CODERATE_4_5		= 7
} FDI_CH_TER_CODERATE_e;

typedef enum
{
	FDI_CH_TRANSMODE_AUTO	= 0,
	FDI_CH_TRANSMODE_2K		= 1,
	FDI_CH_TRANSMODE_8K		= 2,
} FDI_CH_TER_TRANSMODE_e;

typedef enum
{
	FDI_CH_GUARDINTERVAL_AUTO	= 0,
	FDI_CH_GUARDINTERVAL_1_32	= 1,
	FDI_CH_GUARDINTERVAL_1_16	= 2,
	FDI_CH_GUARDINTERVAL_1_8	= 3,
	FDI_CH_GUARDINTERVAL_1_4	= 4
} FDI_CH_TER_GUARDINTERVAL_e;

typedef enum
{
	FDI_CH_STREAM_HP = 0,
	FDI_CH_STREAM_LP = 1
} FDI_CH_TER_STREAM_e;

typedef enum
{
	FDI_CH_OFFSET_AUTO	= 0,
	FDI_CH_OFFSET_NONE	= 1,
	FDI_CH_OFFSET_1_6_P	= 2,
	FDI_CH_OFFSET_1_6_N	= 3,
	FDI_CH_OFFSET_2_6_P	= 4,
	FDI_CH_OFFSET_2_6_N	= 5,
	FDI_CH_OFFSET_3_6_P	= 6,
	FDI_CH_OFFSET_3_6_N	= 7,
} FDI_CH_TER_OFFSET_e;

typedef enum
{
	FDI_CH_DVBT 	= 0,
	FDI_CH_DVBT2 	= 1
}FDI_CH_TER_System_e;

typedef enum
{
	FDI_CH_T2SISO = 0,
	FDI_CH_T2MISO = 1
} FDI_CH_TER_PreambleFormat_e;

typedef enum
{
	FDI_CH_T2NOT_MIXED 	= 0,
	FDI_CH_T2MIXED 		= 1
} FDI_CH_TER_PreambleMixed_e;

typedef enum
{
	FDI_CH_T2NOT_USED 	= 0,
	FDI_CH_T2USED 		= 1
} FDI_CH_TER_ExtendedCarrier_e;

typedef enum
{
	FDI_CH_NONE_PAPR	= 0,
	FDI_CH_ACE_PAPR		= 1,
	FDI_CH_TR_PAPR		= 2,
	FDI_CH_ACETR_PAPR	= 3
} FDI_CH_TER_PaprMode_e;


typedef enum
{
	FDI_CH_T2_PPAUTO	= 0,
	FDI_CH_T2_PP1	= 1,
	FDI_CH_T2_PP2	= 2,
	FDI_CH_T2_PP3	= 3,
	FDI_CH_T2_PP4	= 4,
	FDI_CH_T2_PP5	= 5,
	FDI_CH_T2_PP6	= 6,
	FDI_CH_T2_PP7	= 7,
	FDI_CH_T2_PP8	= 8
} FDI_CH_TER_PilotPattern_e;

typedef enum
{
	FDI_CH_ANT_POWER_OFF	= 0, 	/**< default value */
	FDI_CH_ANT_POWER_ON		= 1,	/**< 5 Volt On */
	FDI_CH_ANT_POWER_FAULT	= 2		/**< Antenna short circuit */
} FDI_CH_TER_ANT_STATUS_e;

typedef void (*pfnFDI_CHANNEL_NotifySignalInfo)(unsigned long ulInstanceId, unsigned long ulRequestId,
												FDI_CH_TYPE_e eChannelType , FDI_CH_SIGNAL_INFO_e eSignalInfo );

/**
 * Define signal info which is used for callback param.
 */
typedef struct
{
	unsigned long ulStrength;
	unsigned long ulQuality;
	float fAgc;
	int nBer;
	float fSnr;
	unsigned long ulReserved1;
	unsigned long ulReserved2;
	unsigned long ulReserved3;
} FDI_CH_SAT_SIGNAL_QUALITY_t;

typedef struct
{
	unsigned long ulStrength;
	unsigned long ulQuality;
	unsigned int unAgc;
	float fBer;
	float fSnr;
	unsigned long ulReserved1;
	unsigned long ulReserved2;
	unsigned long ulReserved3;
} FDI_CH_CAB_SIGNAL_QUALITY_t;

typedef struct
{
	unsigned long ulStrength;
	unsigned long ulQuality;
	unsigned int unAgc;
	float fBer;
	float fSnr;
	unsigned long ulReserved1;
	unsigned long ulReserved2;
	unsigned long ulReserved3;
} FDI_CH_TER_SIGNAL_QUALITY_t;

/* SAT tuning parameters */
typedef struct
{
	FDI_CH_SAT_ANTENNATYPE_e			etAntennaType;

	/* DVBS parameters */
	unsigned long						ulFrequency;				/**< tuning frequency : Unit [KHz] */
	unsigned long						ulSymbolRate;				/**< Symbol rate */
	FDI_CH_CODERATE_e					etFecCodeRate;				/**< FEC code rate */
	FDI_CH_POLARIZATION_e				etPolarization;				/**< Polarization */

	/* DVBS2 parameters */
	FDI_CH_TRANSPORT_SPEC_e				etTransSpec;				/**< DBV S/S2 */
	FDI_CH_MODULATION_e 				etModulation;				/**< Modulation */
	FDI_CH_PILOT_e						etPilot;					/**< S2 Pilot on/off */
	FDI_CH_ROLLOFF_e					etRollOff;					/**< S2 Roll off */

	/* LNB parameters */
	FDI_CH_LNB_VOLTAGE_e 				etLnbVoltage;				/**< LNB voltage level */

	/* Diseqc parameters */
	FDI_CH_DISEQC_VER_e					etDiseqcVersion;			/**< DiSEqC version */
	FDI_CH_DISEQ_INPUT_e				etDiseqcInput;				/**< Input position */
	FDI_CH_TONE_BURST_e					etToneBurst;
	unsigned char						b22kTone;					/**< 22KHz tone on/off */

	/* SCD patameters */
	unsigned long						ulScdMasterUnitId;			/**< SCD Master Unit Id which is used for multi tuner */
	unsigned long						ulScdUserBandNum;			/**< SCD Band number */
	unsigned long						ulScdUserBandFreq;			/**< SCD Band Freq. */

	unsigned long 							ulSelectedAdc;
} FDI_CH_SAT_TUNE_PARAM_t;

/* CAB tuning parameters */
typedef struct
{
	unsigned long 						ulFrequency;
	unsigned long  						ulSymbolRate;
	FDI_CH_CAB_MODULATION_e  			etModulation;
	FDI_CH_CAB_SPECTRUM_e   			etSpectrum;
} FDI_CH_CAB_TUNE_PARAM_t;

/* TER tuning parameters */
typedef struct
{
	unsigned long					ulPlpId;
	FDI_CH_TER_PreambleFormat_e		etPreambleFormat;
	FDI_CH_TER_PreambleMixed_e		etPreambleMixed;
	FDI_CH_TER_PilotPattern_e		etPilotPattern;
	FDI_CH_TER_ExtendedCarrier_e	etExtendedCarrier;
	FDI_CH_TER_PaprMode_e			etPaprMode;
	unsigned long					ulPlpNumBlocks;
}FDI_CH_TER_T2Param_t;

typedef struct
{
	unsigned long							ulFrequency;
	FDI_CH_TER_BANDWIDTH_e			etBandWidth;
	FDI_CH_TER_MODULATION_e			etModulation;
	FDI_CH_TER_HIERARCHY_e			etHierarchy;
	FDI_CH_TER_CODERATE_e			etCodeRate;
	FDI_CH_TER_TRANSMODE_e			etTransMode;
	FDI_CH_TER_GUARDINTERVAL_e		etGuardInterval;
	FDI_CH_TER_STREAM_e				etStream;
	FDI_CH_TER_OFFSET_e				etOffset;
	FDI_CH_TER_System_e				etTerSystem;
	FDI_CH_TER_T2Param_t			stT2Param;
} FDI_CH_TER_TUNE_PARAM_t;

/******************************************************************************
* Global variables and structures
******************************************************************************/
/* global variables and structures */

/******************************************************************************
* External variables and functions
******************************************************************************/
/* external variables  and functions */

/******************************************************************************
* Static variables and structures
******************************************************************************/
/* static variables and structures */

/******************************************************************************
* Static function prototypes
******************************************************************************/
/* static function prototypes */

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_GetNumOfDevice(unsigned long *pulNumOfDevice);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_GetCapability(unsigned long ulDeviceId, FDI_CH_TYPE_e *peType);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_S_Open(unsigned long ulDeviceId);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_S_RequestLock(unsigned long ulDeviceId, FDI_CH_SAT_TUNE_PARAM_t *ptParam);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_S_GetLockedInfo(unsigned long ulDeviceId, FDI_CH_SAT_TUNE_PARAM_t *ptParam);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_S_IsLocked(unsigned long ulDeviceId, BOOL *pbIsLocked);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_S_CancelLock(unsigned long ulDeviceId);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_S_GetSignalInfo(unsigned long ulDeviceId, FDI_CH_SAT_SIGNAL_QUALITY_t *ptSignalInfo);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_S_SetLnb22KhzTone(unsigned long ulDeviceId, unsigned char uc22KhzTone);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_S_SetLnbPolarisation(unsigned long ulDeviceId,
												FDI_CH_POLARIZATION_e ePolarization,
												FDI_CH_LNB_VOLTAGE_e eVoltage);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_S_SetLnbOutput(unsigned long ulDeviceId, BOOL bOnOff);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_S_RegisterCallback(unsigned long ulDeviceId, pfnFDI_CHANNEL_NotifySignalInfo pfnNotify);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_S_SetLoopthrouth(BOOL bOnOff);


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_S_SetInput(int nUnitDi, int nInputSource);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_S_SendDiseqcMsg( unsigned long ulDeviceId, unsigned char *pucData, unsigned char ucLength);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_S_ReceiveDiseqcMsg( unsigned long ulDeviceId, unsigned char *pucData, unsigned char ucLength, unsigned char *pucReplyLength);


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_C_Open(unsigned long ulDeviceId);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_C_RequestLock(unsigned long ulDeviceId, FDI_CH_CAB_TUNE_PARAM_t *ptParam);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_C_GetLockedInfo(unsigned long ulDeviceId,
											FDI_CH_CAB_TUNE_PARAM_t *ptParam);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_C_IsLocked(unsigned long ulDeviceId, BOOL *pbIsLocked);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_C_CancelLock(unsigned long ulDeviceId);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_C_GetSignalInfo(unsigned long ulDeviceId, FDI_CH_CAB_SIGNAL_QUALITY_t *ptSignalInfo);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_C_RegisterCallback(unsigned long ulDeviceId, pfnFDI_CHANNEL_NotifySignalInfo pfnNotify);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_T_Open(unsigned long ulDeviceId);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_T_RequestLock(unsigned long ulDeviceId, FDI_CH_TER_TUNE_PARAM_t *ptParam);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_T_GetLockedInfo(unsigned long ulDeviceId,
											FDI_CH_TER_TUNE_PARAM_t *ptParam);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_T_IsLocked(unsigned long ulDeviceId, BOOL *pbIsLocked);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_T_CancelLock(unsigned long ulDeviceId);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_T_SetAntennaPower(unsigned long ulDeviceId, BOOL bOn_nOff);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_T_GetSignalInfo(unsigned long ulDeviceId, FDI_CH_TER_SIGNAL_QUALITY_t *ptSignalInfo);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_T_RegisterCallback(unsigned long ulDeviceId, pfnFDI_CHANNEL_NotifySignalInfo pfnNotify);

#endif

