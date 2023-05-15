/**
 * @file	di_channel_s.h
 * @brief	Channel SAT module DI layer header file
 * @version $Revision: 1.0.0 $
 * @date 	$Date: 2010/03/27 12:00:00 $
 * @author 	Linus Lee
 */

/** @defgroup CH_SAT SATELLITE SYSTEM
 *  This is Satellite System of DI layer.
 *  @{
 */

#ifndef _DI_CHANNEL_S_H_
#define _DI_CHANNEL_S_H_
/******************************************************************************
 * Include Files
 ******************************************************************************/
#include "di_channel.h"

/******************************************************************************
 * Symbol and Macro Definitions
 ******************************************************************************/

/******************************************************************************
 * Data Types
 ******************************************************************************/

/**
 * DiSEqC Switch type.
 */
typedef enum
{
	DI_CH_ANT_TYPE_LNB		= 0,	/**< LNB only */
	DI_CH_ANT_TYPE_DISEQC	= 1,	/**< DiSEqC switch */
	DI_CH_ANT_TYPE_SCD		= 2,	/**< Single Cable Distribution */
	DI_CH_ANT_TYPE_SMATV	= 3		/**< SMATV */
} CH_SAT_AntennaType_e;


/**
 * FEC code rate.
 */
typedef enum
{
	DI_CH_SCODERATE_AUTO	= 0,	/**< Code rate : Auto detect */

	DI_CH_SCODERATE_1_2		= 1,	/**< Code rate : 1/2  DVB-S 	DVB-S2:QPSK 				*/
	DI_CH_SCODERATE_2_3		= 2,	/**< Code rate : 2/3  DVB-S 	DVB-S2:QPSK 	DVB-S2:8PSK */
	DI_CH_SCODERATE_3_4		= 3,	/**< Code rate : 3/4  DVB-S 	DVB-S2:QPSK 	DVB-S2:8PSK */
	DI_CH_SCODERATE_5_6		= 4,	/**< Code rate : 5/6  DVB-S 	DVB-S2:QPSK 	DVB-S2:8PSK */
	DI_CH_SCODERATE_7_8		= 5,	/**< Code rate : 7/8  DVB-S 								*/

	DI_CH_S2CODERATE_3_5	= 6,	/**< Code rate : 3/5  			DVB-S2:QPSK 	DVB-S2:8PSK */
	DI_CH_S2CODERATE_4_5	= 7,	/**< Code rate : 4/5 			DVB-S2:QPSK 				*/
	DI_CH_S2CODERATE_8_9	= 8,	/**< Code rate : 8/9  			DVB-S2:QPSK 	DVB-S2:8PSK */
	DI_CH_S2CODERATE_9_10	= 9,	/**< Code rate : 9/10 			DVB-S2:QPSK 	DVB-S2:8PSK */

	DI_CH_SCODERATE_5_11	= 10,	/**< Code rate : 5/11 										*/
	DI_CH_SCODERATE_6_7		= 11	/**< Code rate : 6/7 										*/

} CH_SAT_CodeRate_e;


/**
 * Polarization.
 */
typedef enum
{
	DI_CH_POLAR_AUTO	= 0,		/**< Auto */
	DI_CH_POLAR_HOR		= 1,		/**< Horizontal */
	DI_CH_POLAR_VER		= 2,		/**< Vertical */
	DI_CH_POLAR_LEFT	= 3,		/**< Left */
	DI_CH_POLAR_RIGHT	= 4			/**< Right */
} CH_SAT_Polarization_e;


/**
 * Transport Sepc.
 */
typedef enum
{
	DI_CH_TRANS_DVBS	= 0,		/**< DVB-S */
	DI_CH_TRANS_DVBS2	= 1			/**< DVB-S2 */
} CH_SAT_TransportSpec_e;


/**
 * SAT modulation.
 */
typedef enum
{
	DI_CH_PSK_AUTO = 0,				/**< Auto */
	DI_CH_PSK_QPSK = 1,				/**< QPSK */
	DI_CH_PSK_8PSK = 2				/**< 8PSK */
} CH_SAT_Modulation_e;


/**
 * DVB-S2 pilot on/off.
 */
typedef enum
{
    DI_CH_PILOT_AUTO	= 0,		/**< Pilot : Auto detect */
    DI_CH_PILOT_OFF		= 1,		/**< Pilot : Off */
    DI_CH_PILOT_ON		= 2			/**< Pilot : On */
} CH_SAT_Pilot_e;


/**
 * DVB-S2 Roll off.
 */
typedef enum
{
	DI_CH_ROLL_020 = 0,				/**< Roll off 20 */
	DI_CH_ROLL_025 = 1,				/**< Roll off 25 */
	DI_CH_ROLL_035 = 2				/**< Roll off 35 */
}CH_SAT_RollOff_e;


/**
 * LNB voltage.
 */
typedef enum
{
	DI_CH_LNB_VOLT_STD	= 0,		/**< Vertical : 11v, Horizontal 15v */
	DI_CH_LNB_VOLT_HIGH	= 1			/**< Vertical : 12v, Horizontal 16v */
}CH_SAT_LnbVoltage_e;


/**
 * DiSEqC version.
 */
typedef enum
{
	DI_CH_DISEQC_VER_1_0 = 0,		/**< which allows switching between up to 4 satellite sources */
	DI_CH_DISEQC_VER_1_1 = 1,		/**< which allows switching between up to 16 sources */
	DI_CH_DISEQC_VER_1_2 = 2,		/**< which allows switching between up to 16 sources, and control of a simple horizontal-panning satellite motor */
	DI_CH_DISEQC_VER_1_3 = 3,		/**< Universal Satellites Automatic Location System (USALS) */
	DI_CH_DISEQC_VER_2_0 = 4		/**< which adds bi-directional communications to DiSEqC 1.0 */
} CH_SAT_DiseqcVer_e;


/**
 * SAT antenna position.
 */
typedef enum
{
	DI_CH_DISEQC_INPUT_NONE		= 0,		/**< position : None */
	DI_CH_DISEQC_INPUT_A		= 1,		/**< position : A */
	DI_CH_DISEQC_INPUT_B		= 2,		/**< position : B */
	DI_CH_DISEQC_INPUT_C		= 3,		/**< position : C */
	DI_CH_DISEQC_INPUT_D		= 4,		/**< position : D */
	DI_CH_DISEQC_INPUT_TB_A		= 5,		/**< position : TB_A */
	DI_CH_DISEQC_INPUT_TB_B		= 6,		/**< position : TB_B */
	DI_CH_DISEQC_INPUT_SCD_A 	= 7,		/**< position : SCD A */
	DI_CH_DISEQC_INPUT_SCD_B	= 8			/**< position : SCD B */
} CH_SAT_DiseqcInput_e;


/**
 * SAT Tone burst.
 */
typedef enum
{
	DI_CH_TONE_BURST_NONE 	= 0,			/**< Tone burst : None */
	DI_CH_TONE_BURST_A		= 1,			/**< Tone burst : A */
	DI_CH_TONE_BURST_B		= 2				/**< Tone burst : B */
} CH_SAT_ToneBurst_e;


/**
 * SAT Input mode of multi tuner platform.
 */
typedef enum
{
	DI_CH_INPUT_ONE_SAME 	= 0, 			/**< Input mode : One Same, In case of internal loop through */
	DI_CH_INPUT_TWO_DIFF 	= 1, 			/**< Input mode : Two Different */
	DI_CH_INPUT_TWO_SAME 	= 2,  			/**< Input mode : Two Same, In case of external loop through */
	DI_CH_INPUT_MAX
} CH_SAT_InputMode_e;


/**
 * SAT tuning parameters.
 */
typedef struct
{
	CH_SAT_AntennaType_e				etAntennaType;				/**< DiSEqC Switch type */

	/* DVBS parameters */
	HUINT32								ulFrequency;				/**< tuning frequency : Unit [MHz] */
	HUINT32								ulSymbolRate;				/**< Symbol rate */
	CH_SAT_CodeRate_e					etFecCodeRate;				/**< FEC code rate */
	CH_SAT_Polarization_e				etPolarization;				/**< Polarization */

	/* DVBS2 parameters */
	CH_SAT_TransportSpec_e				etTransSpec;				/**< DBV S/S2 */
	CH_SAT_Modulation_e 				etModulation;				/**< Modulation */
	CH_SAT_Pilot_e						etPilot;					/**< S2 Pilot on/off */
	CH_SAT_RollOff_e					etRollOff;					/**< S2 Roll off */

	/* LNB parameters */
	CH_SAT_LnbVoltage_e 				etLnbVoltage;				/**< LNB voltage level */

	/* Diseqc parameters */
	CH_SAT_DiseqcVer_e					etDiseqcVersion;			/**< DiSEqC version */
	CH_SAT_DiseqcInput_e				etDiseqcInput;				/**< Input position */
	CH_SAT_ToneBurst_e					etToneBurst;				/**< Control tone-burst sw */
	HBOOL								b22kTone;					/**< 22KHz tone on/off */

	/* SCD patameters */
	HUINT32 							ulScdMasterUnitId;			/**< SCD Master Unit Id which is used for multi tuner */
	HUINT32 							ulScdUserBandNum;			/**< SCD Band number */
	HUINT32 							ulScdUserBandFreq;			/**< SCD Band Freq. */

	HUINT32 							ulSelectedAdc;

} CH_SAT_TuneParam_t;

/**
 * Structure of capacity of tuner
 */
typedef struct
{
	HINT32		anInputId[DI_CH_RF_INPUTMAX-1];		/**< RF Input Index array */
	HUINT32		ulNumOfInput;						/**< Number of RF input */
	HBOOL 		bLoopThroughOut;					/**< Existence of external Loop through out */

} DI_CH_TunerCapacity_t;

/******************************************************************************
 * Local(static) Data Types
 ******************************************************************************/

/******************************************************************************
 * Extern Variables
 ******************************************************************************/

/******************************************************************************
 * Global Variables
 ******************************************************************************/

/******************************************************************************
 * Local(static) Variables
 ******************************************************************************/

/******************************************************************************
 * Extern Function Prototypes
 ******************************************************************************/

/**
 * MW register callback function which inform of signal status.
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 *
 * \param[in]	pfnNotify			Callback function which is registered.
 *
 * \return      OK, or other Error on failure
 *
 * \remarks
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_S_RegisterCallback( HUINT32 ulInstanceId, pfnDI_CH_NotifySignalInfo pfnNotify);



/**
 * MW activate a logical channel and assign a handler associated.
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     Before tuning, activate a channel first.
 *
 * \see         DI_CH_S_DeactivateChannel
 */
DI_CHANNEL_ERR DI_CH_S_ActivateChannel( HUINT32 ulInstanceId );



/**
 * MW deactivate a logical channel and release a handler associated.
 *
 * \param[in]	ulInstanceId		Logical Tuner ID
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     When the connection is not effective, MW has to deactivate the channel.
 *
 * \see         DI_CH_S_ActivateChannel
 */
DI_CHANNEL_ERR DI_CH_S_DeactivateChannel( HUINT32 ulInstanceId );



/**
 * MW cancel for tuner to lock.
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     If possible, cut ts output off.
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_S_CancelLock( HUINT32 ulInstanceId );



/**
 * MW ask SAT tuner to lock.
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 * \param[in]	ulRequestId			When send lock event to MW, it is managed by DI.
 * \param[in]	pstParam			tuning parameters
 *
 * \return      OK, or other Error on failure
 *
 * \remarks
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_S_RequestLock( HUINT32 ulInstanceId, HUINT32 ulRequestId, CH_SAT_TuneParam_t *pstParam );



/**
 * MW take the tune param locked.
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 * \param[out]	pstParam			tuning parameters
 *
 * \return      OK, or other Error on failure
 *
 * \remarks
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_S_GetLockedInfo( HUINT32 ulInstanceId, CH_SAT_TuneParam_t *pstParam );



/**
 * MW get various signal information( AGC, BER, SNR etc. ).
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 * \param[out]	pstSignalInfo			Information of the Signal quality
 *
 * \return      OK, or other Error on failure
 *
 * \remarks
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_S_GetSignalInfo ( HUINT32 ulInstanceId, DI_CH_SignalQuality_t *pstSignalInfo );

 /**
  * MW get device status( AVS voltage, temperature ).
  *
  * \param[in]	 ulInstanceId			 Logical Tuner ID
  * \param[out]  pstDeviceStatus		 Information of the device status
  *
  * \return 	 OK, or other Error on failure
  *
  * \remarks
  *
  * \see
  */
 DI_CHANNEL_ERR DI_CH_S_GetDeviceStatus ( HUINT32 ulInstanceId, DI_CH_DeviceStatus *pstDeviceStatus );


 /**
 * MW send the message of DiSEqC.
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 * \param[in]	pucData				the pointer of message sending
 * \param[in]	ucLength			the length of message
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     SAT only
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_S_SendDiseqcMsg( HUINT32 ulInstanceId, HUINT8 * pucData, HUINT8 ucLength);


 /**
 * MW receive the message of DiSEqC.
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 * \param[in]	pucData				the pointer of message receiving
 * \param[in]	ucLength				the length of message
 * \param[out]  pucReplyLength		the length of real reply
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     SAT only
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_S_ReceiveDiseqcMsg( HUINT32 ulInstanceId, HUINT8 * pucData, HUINT8 ucLength, HUINT8 *pucReplyLength);


/******************************************************************************
 * TO PROVIDE API FOR HW TEST APP & FACTORY APP
 ******************************************************************************/

/**
 * MW set LNB Output.
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 * \param[in]	bEnable					On or Off
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     SAT only
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_S_SetLnbOutput( HUINT32 ulInstanceId, HBOOL bEnable );



/**
 * MW set LNB 22KHz tone.
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 * \param[in]	b22KhzTone				On or Off
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     SAT only
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_S_SetLnb22KhzTone( HUINT32 ulInstanceId, HBOOL b22KhzTone );



/**
 * MW set LNB Polarisation.
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 * \param[in]	etPolar					Polarisation
 * \param[in]	etVoltage				standard or high
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     SAT only, Horizontal=18V, Vertical=13V
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_S_SetLnbPolarisation( HUINT32 ulInstanceId, CH_SAT_Polarization_e etPolar, CH_SAT_LnbVoltage_e etVoltage );



/**
 * get Channel Status.
 *
 * \param[in]		ulInstanceId		Logical Tuner ID
 * \param[out]	pulBer			pulBer		<- Actually SNR
 * \param[out]	pulRfAgc			pulRfAgc
 * \param[out]	pulIfAgc			pulIfAgc
 * \param[out]	pulUncorrect		pulUncorrect
 * \param[out]	pfPower			pfPower
 *
 * \return      OK, or other Error on failure
 *
 * \remarks	 SAT only, Hw Test Sw	/	It will be deprecated
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_S_GetChannelStatus (HUINT32 ulInstanceId, HINT32 *pulBer, HUINT32 *pulRfAgc, HUINT32 *pulIfAgc, HUINT32 *pulUncorrect, float *pfPower);



DI_CHANNEL_ERR DI_CH_S_SetLoopthGpio(HBOOL bHigh);


/**
 * Octo get a capability of HW configuration of tuner.
 *
 * \param[in]		ulInstanceId		Logical Tuner ID
 * \param[out]	pstTunerCap		refer to DI_CH_TunerCapacity_t
 *
 * \return      OK, or other Error on failure
 *
 * \remarks
 *
 * \see		DI_CH_TunerCapacity_t
 */
DI_CHANNEL_ERR DI_CH_S_GetTunerCapability( HUINT32 ulInstanceId, DI_CH_TunerCapacity_t *pstTunerCap );



/**
 * Octo set input source of multi tuner platform.
 *
 * \param[in]		ulInstanceId			Logical Tuner ID
 * \param[in]		etInputSource			refer to DI_CH_InputSource_e
 *
 * \return      OK, or other Error on failure
 *
 * \remarks    For multi tuner platform
 *
 * \see		DI_CH_S_GetTunerCapability
 */
DI_CHANNEL_ERR DI_CH_S_SetInputSource( HUINT32 ulInstanceId, DI_CH_InputSource_e etInputSource );


/**
 * MW set input mode of multi tuner platform.
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 * \param[in]	etInputMode			refer to CH_SAT_InputMode_e
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     SAT multi tuner platform only
 *
 * \see		CH_SAT_InputMode_e
 */
DI_CHANNEL_ERR DI_CH_S_SetInputMode( HUINT32 ulInstanceId, CH_SAT_InputMode_e etInputMode );



/**
 * DD reinitialize device in case of S3 warm boot.
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     If possible, cut ts output off.
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_S_ReInitDevice( HUINT32 ulInstanceId );


/******************************************************************************
 * Local(static) Function Prototypes
 ******************************************************************************/


#endif /* _DI_CHANNEL_S_H_ */

/** @} */ // end of DI_CH_SAT

