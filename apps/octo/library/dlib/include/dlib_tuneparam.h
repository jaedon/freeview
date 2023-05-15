/* $Header: $ */

#ifndef DLIB_TUNEPARAM_H__
#define DLIB_TUNEPARAM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <hlib.h>

typedef enum
{
	eDxSAT_CODERATE_UNKNOWN			= -1,
	eDxSAT_CODERATE_AUTO			= 0,		/* Code rate : Auto detect */
	eDxSAT_CODERATE_NONE			= 1,		/* DVBS Code Rate */
	eDxSAT_CODERATE_1_2				= 2,		/* Code rate : 1/2 */
	eDxSAT_CODERATE_2_3				= 3,		/* Code rate : 2/3 */
	eDxSAT_CODERATE_3_4				= 4,		/* Code rate : 3/4 */
	eDxSAT_CODERATE_5_6				= 5,		/* Code rate : 5/6 */
	eDxSAT_CODERATE_7_8				= 6,		/* Code rate : 7/8 */

	/* DVBS2 Code Rate */
	eDxSAT_CODERATE_3_5				= 7,		/* Code rate : 3/5 */
	eDxSAT_CODERATE_4_5				= 8,		/* Code rate : 4/5 */
	eDxSAT_CODERATE_5_11			= 9,		/* Code rate : 5/11 */
	eDxSAT_CODERATE_6_7				= 10,		/* Code rate : 6/7 */
	eDxSAT_CODERATE_8_9				= 11,		/* Code rate : 8/9 */
	eDxSAT_CODERATE_9_10			= 12		/* Code rate : 9/10 */

} DxSat_CodeRate_e;

typedef enum
{
	eDxSAT_POLAR_UNKNOWN			= -1,
	eDxSAT_POLAR_AUTO				= 0,
	eDxSAT_POLAR_HOR				= 1,
	eDxSAT_POLAR_VER				= 2,
	eDxSAT_POLAR_LEFT				= 3,
	eDxSAT_POLAR_RIGHT 				= 4
}  DxSat_Polarization_e;

typedef enum
{
	eDxSAT_TRANS_UNKNOWN			= -1,
	eDxSAT_TRANS_DVBS				= 0,
	eDxSAT_TRANS_DVBS2				= 1
}  DxSat_TransportSpec_e;

typedef enum
{
	eDxSAT_PSK_UNKOWN				= -1,
	eDxSAT_PSK_AUTO					= 0,
	eDxSAT_PSK_QPSK					= 1,
	eDxSAT_PSK_8PSK					= 2
} DxSat_PskModulation_e;

typedef enum
{
	eDxSAT_PILOT_UNKNOWN			= -1,
	eDxSAT_PILOT_AUTO				= 0,			/**< Pilot : Auto detect */
	eDxSAT_PILOT_OFF				= 1,			/**< Pilot : Off */
	eDxSAT_PILOT_ON					= 2 			/**< Pilot : On */
} DxSat_Pilot_e;

typedef enum
{
	eDxSAT_ROLL_UNKNOWN				= -1,
	eDxSAT_ROLL_020					= 0,		/**< Roll off : 0.20 */
	eDxSAT_ROLL_025					= 1,		/**< Roll off : 0.25 */
	eDxSAT_ROLL_035					= 2 		/**< Roll off : 0.35 */
} DxSat_RollOff_e;




typedef struct
{
	/* DVBS Parameters */
	HUINT32 							ulFrequency;			/**< Tuning Frequency, MHz(Unit) */
	HUINT32 							ulSymbolRate;			/**< Symbol Rate, KSymbol/sec */
	DxSat_CodeRate_e 					eFecCodeRate;			/**< FEC Code Rate */
	DxSat_Polarization_e 				ePolarization;			/**< Polarity : Vertical/Horizontal*/

	/* DVBS2 Parameters */
	DxSat_TransportSpec_e				eTransSpec;				/**< DVBS, DVBS2 */
	DxSat_PskModulation_e				ePskMod; 				/**< Modulation Type : QPSK, 8PSK, AUTO*/
	DxSat_Pilot_e						ePilot;					/**< Pilot On/Off/Auto */
	DxSat_RollOff_e						eRollOff;				/**< Roll off : 0.20, 0.25, 0.35 */

	HCHAR								satType[DxANTINFO_NAME_LEN];
	HINT32								antuid;
} DxSat_TuningTPInfo_t;


typedef struct
{
	DxDeliveryType_e				deliveryType;
	HUINT32 						ulTunerGroupId;
	HUINT32 						ulRfInputId;

	DxAntennaType_e					antennaType;
	DxSat_TuningTPInfo_t 			tuningInfo;
	DxAntInfo_t						antInfo;
#if	0
	union
	{
		DxLnbAntenna_t				lnb;
		DxDiseqcAntenna_t 			diseqc;
		DxScdAntenna_t				scd;
	} antInfo;
#endif
} DxSatTuningInfo_t;

typedef enum
{
	eDxCAB_CONSTELLATION_AUTO		= 0,
	eDxCAB_CONSTELLATION_16QAM 		= 1,
	eDxCAB_CONSTELLATION_32QAM 		= 2,
	eDxCAB_CONSTELLATION_64QAM 		= 3,
	eDxCAB_CONSTELLATION_128QAM		= 4,
	eDxCAB_CONSTELLATION_256QAM		= 5,
	eDxCAB_CONSTELLATION_1024QAM	= 6,
	eDxCAB_CONSTELLATION_4096QAM	= 7
} DxCab_Constellation_e;

typedef enum
{
	eDxCAB_SPECTRUM_AUTO			= 0,
	eDxCAB_SPECTRUM_NORMAL 			= 1,
	eDxCAB_SPECTRUM_INVERTED		= 2
} DxCab_Spectrum_e;



typedef struct
{
	DxDeliveryType_e				deliveryType;

	HUINT32 						ulTunerGroupId;


	HUINT32 						ulFrequency;
	HUINT32 						ulSymbolRate;
	DxCab_Constellation_e			constellation;
	DxCab_Spectrum_e 				spectrum;
	HINT32							nTsId;
	HINT32							nNetId;
} DxCabTuningInfo_t;

typedef enum
{
	eDxTER_BANDWIDTH_8M			= 0,	/**< 8MHz (DVB-T/DVB-H/DMB-T/H). ï¿½ï¿½ï¿? DAB Eureka 147(1.712MHz), IEEE 802.11a(20MHz) */
	eDxTER_BANDWIDTH_7M			= 1,	/**< 7MHz */
	eDxTER_BANDWIDTH_6M			= 2,	/**< 6MHz */
	eDxTER_T2BANDWIDTH_5MHZ		= 3,
	eDxTER_T2BANDWIDTH_10MHZ		= 4,
	eDxTER_T2BANDWIDTH_1P7MHZ		= 5
} DxTer_BandWidth_e;

/* Modulation Constellation */
typedef enum
{
	eDxTER_CONSTELLATION_AUTO	= 0,
	eDxTER_CONSTELLATION_QPSK	= 1,
	eDxTER_CONSTELLATION_16QAM = 2,
	eDxTER_CONSTELLATION_64QAM = 3,
	eDxTER_CONSTELLATION_256QAM= 4

} DxTer_Constellation_e;

typedef enum
{
	eDxTER_HIERACHY_AUTO		= 0,
	eDxTER_HIERACHY_NONE		= 1,
	eDxTER_HIERACHY_1			= 2,
	eDxTER_HIERACHY_2			= 3,
	eDxTER_HIERACHY_4			= 4
} DxTer_Hierachy_e;


/* Inner FEC Code Rate */
typedef enum
{
	eDxTER_CODERATE_AUTO	= 0,
	eDxTER_CODERATE_1_2		= 1,	/**< 1/2 */
	eDxTER_CODERATE_2_3		= 2,	/**< 2/3 */
	eDxTER_CODERATE_3_4		= 3,	/**< 3/4 */
	eDxTER_CODERATE_5_6		= 4,	/**< 5/6 */
	eDxTER_CODERATE_7_8		= 5 	/**< 7/8 */
} DxTer_CodeRate_e;

/* Number of Subcarriers. (Number of FFT input data values.) */
typedef enum
{
	eDxTER_TRANSMODE_AUTO		= 0,
	eDxTER_TRANSMODE_2K			= 1,	/**< FFT 2048. 1705 subcarriers */
	eDxTER_TRANSMODE_8K			= 2,	/**< FFT 8192. 6817 subcarriers */
	eDxTER_T2TRANSMODE_1K		= 3,
	eDxTER_T2TRANSMODE_4K		= 4,
	eDxTER_T2TRANSMODE_16K 		= 5,
	eDxTER_T2TRANSMODE_32K 		= 6
} DxTer_TransMode_e;

/**< Additional guard interval */
typedef enum
{
	eDxTER_GUARDINTERVAL_AUTO	= 0,
	eDxTER_GUARDINTERVAL_1_4	=	1,	/**< 1/4 */
	eDxTER_GUARDINTERVAL_1_8	= 2,	/**< 1/8 */
	eDxTER_GUARDINTERVAL_1_16	= 3,	/**< 1/16 */
	eDxTER_GUARDINTERVAL_1_32	= 4,	/**< 1/32 */
	eDxTER_T2GUARDINTERVAL_1_128	= 5,	/**< 1/128 */
	eDxTER_T2GUARDINTERVAL_19_128	= 6,	/**< 19/128 */
	eDxTER_T2GUARDINTERVAL_19_256	= 7 /**< 19/256 */

} DxTer_GuardInterval_e;

typedef enum
{
	eDxTER_STREAM_HP			= 0,	/**< HP stream */
	eDxTER_STREAM_LP			= 1 	/**< LP stream */
} DxTer_Stream_e;

typedef enum
{
	eDxTER_OFFSET_AUTO 		= 0,
	eDxTER_OFFSET_NONE 		= 1,	/**< No offset */
	eDxTER_OFFSET_1_6_P		= 2,	/**< +1/6MHz */
	eDxTER_OFFSET_1_6_N		= 3,	/**< -1/6MHz */
	eDxTER_OFFSET_2_6_P		= 4,	/**< +2/6MHz */
	eDxTER_OFFSET_2_6_N		= 5,	/**< -2/6MHz */
	eDxTER_OFFSET_3_6_P		= 6,	/**< +3/6MHz */
	eDxTER_OFFSET_3_6_N		= 7 	/**< -3/6MHz */
} DxTer_Offset_e;
typedef enum
{
	eDxTER_DVB_T			= 0,		/**< T2 stream */
	eDxTER_DVB_T2			= 1 		/**< T stream */
} DxTer_System_e;

typedef enum
{
	eDxTER_SISO			= 0,
	eDxTER_MISO			= 1
} DxTer_PreambleFormat_e;
typedef enum
{
	eDxTER_NOT_MIXED		= 0,
	eDxTER_MIXED			= 1
} DxTer_Mixed_e;
typedef enum
{
	eDxTER_PP_AUTO 	= 0,
	eDxTER_PP1 		= 1,
	eDxTER_PP2 		= 2,
	eDxTER_PP3 		= 3,
	eDxTER_PP4 		= 4,
	eDxTER_PP5 		= 5,
	eDxTER_PP6 		= 6,
	eDxTER_PP7 		= 7,
	eDxTER_PP8 		= 8
} DxTer_Pilot_Pattern_e;

typedef enum
{
	eDxTER_T2NOT_USED		= 0,
	eDxTER_T2USED			= 1
} DxTer_ExtendedCarrier_e;

typedef enum
{
	eDxTER_REDUCTION_NONE			= 0,
	eDxTER_ACE 						= 1,
	eDxTER_TR						= 2,
	eDxTER_AVE_TR					= 3
} DxTer_PAPR_Reduction_e;

typedef struct
{
	HUINT32 					ulPlpId;
	DxTer_PreambleFormat_e		preambleFormat;
	DxTer_Mixed_e				mixed;
	DxTer_Pilot_Pattern_e		pilotPattern;
	DxTer_ExtendedCarrier_e		extenedCarrier;
	DxTer_PAPR_Reduction_e		PAPRreduction;
	HUINT32 					numPlpBlocks;
}DxTer_T2_Param_t;

typedef struct
{
	DxDeliveryType_e				deliveryType;
	HUINT32							ulTunerGroupId;

	HUINT32							ulFrequency;
	DxTer_BandWidth_e				eBandWidth;
	DxTer_Constellation_e			eConstellation;
	DxTer_Hierachy_e				eHierachy;
	DxTer_CodeRate_e				eCodeRate;
	DxTer_TransMode_e				eTransMode;
	DxTer_GuardInterval_e			eGuardInterval;
	DxTer_Stream_e					eStream;
	DxTer_Offset_e					eOffset;
	/* DVB-T2 */
	DxTer_System_e					eSystem;
	DxTer_T2_Param_t				stT2Param;
} DxTerTuningInfo_t;

typedef struct
{
	DxDeliveryType_e		eDeliType;
	HUINT32					ulTunerId;

	// TODO: we have to define..
} DxNetTuneInfo_t;

typedef union
{
	DxDeliveryType_e		eDeliType;
	DxSatTuningInfo_t		sat;
	DxTerTuningInfo_t		ter;
	DxCabTuningInfo_t		cab;
	DxNetTuneInfo_t			net;
} DxTuneParam_t;

typedef enum
{
	eDxSat_MOTOR_DIRECTION_EAST = 0,
	eDxSat_MOTOR_DIRECTION_WEST = 1
} DxSat_MOTOR_DIRECTION_e;	// same as CH_MotorDirection_t

typedef enum
{
	eDxSat_MOTOR_DRIVEMODE_CONTINUOUS 	= 0,
	eDxSat_MOTOR_DRIVEMODE_TIMEOUT 		= 1,
	eDxSat_MOTOR_DRIVEMODE_STEP 		= 2
} DxSat_MOTOR_DRIVEMODE_e;	// same as CH_MotorDriveMode_t

#ifdef __cplusplus
}
#endif

#endif // DLIB_TUNEPARAM_H__


