/* $Header: $ */

#ifndef DLIB_ANTINFO_H__
#define DLIB_ANTINFO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <hlib.h>

#define DxSAT_UNIVERSAL_LNB_FREQ		0xFFFFFFFF
#define DxSAT_MDU1_LNB_FREQ				(DxSAT_UNIVERSAL_LNB_FREQ - 1)			// for turkish LNB types MDU1 using in digitiruk OP
#define DxSAT_MDU2_LNB_FREQ				(DxSAT_UNIVERSAL_LNB_FREQ - 2)			// for turkish LNB types MDU2 using in digitiruk OP
#define DxSAT_MDU3_LNB_FREQ				(DxSAT_UNIVERSAL_LNB_FREQ - 3)			// for turkish LNB types MDU3 using in digitiruk OP
#define DxSAT_MDU4_LNB_FREQ				(DxSAT_UNIVERSAL_LNB_FREQ - 4)			// for turkish LNB types MDU4 using in digitiruk OP
#define DxSAT_MDU5_LNB_FREQ				(DxSAT_UNIVERSAL_LNB_FREQ - 5)			// for turkish LNB types MDU5 using in digitiruk OP
#define DxSAT_MDU_USER_DEFINE			(DxSAT_UNIVERSAL_LNB_FREQ - 100)		// for turkish LNB types User define using in digitiruk OP

typedef enum
{
  eDxLNB_VOLT_STD			= 0,			   /**< Vertical : 11v, Horizontal 15v */
  eDxLNB_VOLT_HIGH			= 1 			   /**< Vertical : 12v, Horizontal 16v */
} DxSAT_LnbVoltage_e;

typedef struct
{
	/* LNB Parameters */
	HUINT32 					ulLnbFreq;				/**< LNB frequency �Ǵ� CH_UNIVERSAL_LNB_FREQ(0xffffffff) */
	DxSAT_LnbVoltage_e			lnbVoltage; 			/**< Voltage drop compensation */
} DxLnbAntenna_t;

typedef enum
{
	eDxDISEQC_VER_1_0 		= 0,
	eDxDISEQC_VER_1_1 		= 1,
	eDxDISEQC_VER_1_2 		= 2,
	eDxDISEQC_VER_1_3 		= 3,
	eDxDISEQC_VER_2_0 		= 4
} DxSAT_DiseqcVer_e;

typedef enum
{
	eDxDISEQC_INPUT_NONE		= 0,
	eDxDISEQC_INPUT_A 		= 1,
	eDxDISEQC_INPUT_B 		= 2,
	eDxDISEQC_INPUT_C 		= 3,
	eDxDISEQC_INPUT_D 		= 4
} DxSAT_DiseqcInput_e;

typedef enum
{
	eDxSCD_INPUT_NONE 		= 0,
	eDxSCD_INPUT_SCD_A		= 1,
	eDxSCD_INPUT_SCD_B		= 2
} DxSAT_ScdInput_e;

typedef enum
{
	eDxTONE_BURST_NONE		= 0,
	eDxTONE_BURST_A			= 1,
	eDxTONE_BURST_B			= 2
} DxSAT_ToneBurst_e;

typedef struct
{
	/* LNB Parameters */
	HUINT32 						ulLnbFreq;				/**< LNB frequency CH_UNIVERSAL_LNB_FREQ(0xffffffff) */
	DxSAT_LnbVoltage_e				lnbVoltage; 			/**< Voltage drop compensation */
	HBOOL							b22kTone;				/**< 22KHz Tone : On/Off */

	/* Diseqc Switch Parameters */
	DxSAT_DiseqcVer_e				diseqcVersion;			/**< Diseqc Ver. 1.0, 1.1, 1.2, 1.3, 2.0 */
	DxSAT_DiseqcInput_e				diseqcInput;			/**< Diseqc A, B, C, D */
	DxSAT_ToneBurst_e				toneBurst;				/**< NONE, A, B */

	/* Diseqc 1.2, 1.3 motor parameters */
	HUINT32 							ucMotorPosition;		/**< for DiSEqC 1.2 */
	HINT32							sSatLonggitude; 		/**< for DiSEqC 1.3 */
} DxDiseqcAntenna_t;

typedef struct
{
	/* LNB Parameters */
	HUINT32 							ulLnbFreq;				/**< LNB frequency CH_UNIVERSAL_LNB_FREQ(0xffffffff) */
	DxSAT_LnbVoltage_e				lnbVoltage; 			/**< Voltage drop compensation */
	HBOOL							b22kTone;				/**< 22KHz tone ON/OFF */

	/* SCD Switch Parameters */
	DxSAT_ScdInput_e 				scdInput;				/**< SCD diseqc input A, B */
	HUINT32 							ulScdUserBandNum;
	HUINT32 							ulScdUserBandFreq;
	HUINT32 							ulScdUserBandNum1;
	HUINT32 							ulScdUserBandFreq1;
	HUINT32 							ulScdUserBandNum2;		// MOON_SCD
	HUINT32 							ulScdUserBandFreq2;
	HUINT32 							ulScdUserBandNum3;
	HUINT32 							ulScdUserBandFreq3;
} DxScdAntenna_t;

typedef struct
{
	HINT32						uid;
	HCHAR						satType[DxANTINFO_NAME_LEN];
	HCHAR						satName[DxANTINFO_NAME_LEN];
	DxAntennaType_e				antType;

	union {
		DxLnbAntenna_t			lnb;
		DxDiseqcAntenna_t 		diseqc;
		DxScdAntenna_t			scd;
	} info;
} DxAntInfo_t;

#ifdef __cplusplus
}
#endif

#endif // DLIB_EPG_H__


