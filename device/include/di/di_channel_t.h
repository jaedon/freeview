/**
 * @file	di_channel_t.h
 * @brief	Channel TER module DI layer header file
 * @version $Revision: 1.0.0 $
 * @date 	$Date: 2010/03/27 12:00:00 $
 * @author 	Linus Lee
 */

/** @defgroup CH_TER TERRESTRIAL SYSTEM
 *  This is Terrestrial System of DI layer.
 *  @{
 */

#ifndef _DI_CHANNEL_T_H_
#define _DI_CHANNEL_T_H_
/******************************************************************************
 * Include Files
 ******************************************************************************/

/******************************************************************************
 * Symbol and Macro Definitions
 ******************************************************************************/

/******************************************************************************
 * Data Types
 ******************************************************************************/

/**
 * TER bandwidth.
 */
typedef enum
{
	DI_CH_BW_8MHZ 	= 0,	/**<  Bandwidth 8MHz */
	DI_CH_BW_7MHZ 	= 1,	/**<  Bandwidth 7MHz */
	DI_CH_BW_6MHZ 	= 2,	/**<  Bandwidth 6MHz */
	DI_CH_BW_5MHZ 	= 3,	/**<  Bandwidth 5MHz */
	DI_CH_BW_10MHZ 	= 4,	/**<  Bandwidth 10MHz */
	DI_CH_BW_1P7MHZ = 5		/**<  Bandwidth 1.7MHz */
} CH_TER_BandWidth_e;


/**
 * TER modulation.
 */
typedef enum
{
	DI_CH_TMOD_AUTO		= 0,	/**< Modulation Auto */
	DI_CH_TMOD_QPSK		= 1,	/**< Modulation QPSK */
	DI_CH_TMOD_16QAM	= 2,	/**< Modulation 16 QAM */
	DI_CH_TMOD_64QAM	= 3,	/**< Modulation 64 QAM */
	DI_CH_TMOD_256QAM	= 4		/**< Modulation 256 QAM */
} CH_TER_Moudulation_e;


/**
 * TER Hierarchy information.
 */
typedef enum
{
	DI_CH_HIERARCHY_AUTO 	= 0,	/**< hierarchical auto */
	DI_CH_HIERARCHY_NONE	= 1,	/**< Non hierarchical */
	DI_CH_HIERARCHY_1		= 2,	/**< alpha = 1 */
	DI_CH_HIERARCHY_2		= 3,	/**< alpha = 2 */
	DI_CH_HIERARCHY_4		= 4		/**< alpha = 4 */
} CH_TER_Hierarchy_e;


/**
 * TER Code rates.
 */
typedef enum
{
	DI_CH_TCODERATE_AUTO 	= 0,	/**< Code rate = auto */
	DI_CH_TCODERATE_1_2		= 1,	/**< Code rate = 1/2 */
	DI_CH_TCODERATE_2_3		= 2,	/**< Code rate = 2/3 */
	DI_CH_TCODERATE_3_4		= 3,	/**< Code rate = 3/4 */
	DI_CH_TCODERATE_5_6		= 4,	/**< Code rate = 5/6 */
	DI_CH_TCODERATE_7_8		= 5,	/**< Code rate = 7/8 */
	DI_CH_T2CODERATE_3_5	= 6,	/**< Code rate = 3/5 */
	DI_CH_T2CODERATE_4_5	= 7		/**< Code rate = 4/5 */
} CH_TER_CodeRate_e;


/**
 * TER Transmission mode.
 */
typedef enum
{
	DI_CH_TRANSMODE_AUTO	= 0,	/**< Transmission mode auto */
	DI_CH_TRANSMODE_2K		= 1,	/**< Transmission mode 2K */
	DI_CH_TRANSMODE_8K		= 2,	/**< Transmission mode 8K */
	DI_CH_T2TRANSMODE_4K	= 3,	/**< Transmission mode 4K */
	DI_CH_T2TRANSMODE_1K	= 4,	/**< Transmission mode 1K */
	DI_CH_T2TRANSMODE_16K	= 5,	/**< Transmission mode 16K */
	DI_CH_T2TRANSMODE_32K	= 6		/**< Transmission mode 32K */
} CH_TER_TransMode_e;


/**
 * TER Guard Intervals.
 */
typedef enum
{
	DI_CH_GUARDINTERVAL_AUTO		= 0,	/**< Guard Interval auto */
	DI_CH_GUARDINTERVAL_1_32		= 1,	/**< Guard Interval 1/32 */
	DI_CH_GUARDINTERVAL_1_16		= 2,	/**< Guard Interval 1/16 */
	DI_CH_GUARDINTERVAL_1_8			= 3,	/**< Guard Interval 1/8 */
	DI_CH_GUARDINTERVAL_1_4			= 4,	/**< Guard Interval 1/4 */
	DI_CH_T2GUARDINTERVAL_1_128		= 5,	/**< Guard Interval 1/128 */
	DI_CH_T2GUARDINTERVAL_19_128	= 6,	/**< Guard Interval 19/128 */
	DI_CH_T2GUARDINTERVAL_19_256	= 7		/**< Guard Interval 19/256 */
} CH_TER_GuardInterval_e;


/**
 * TER HP/LP stream.
 */
typedef enum
{
	DI_CH_STREAM_HP = 0,	/**< High Priority bit stream */
	DI_CH_STREAM_LP = 1		/**< Low Priority bit stream */
} CH_TER_Stream_e;


/**
 * TER AFC : Auto frequency control.
 */
typedef enum
{
	DI_CH_OFFSET_AUTO	= 0,	/**< frequency offset auto */
	DI_CH_OFFSET_NONE	= 1,	/**< frequency offset = 0 */
	DI_CH_OFFSET_1_6_P	= 2,	/**< frequency offset = +167KHz */
	DI_CH_OFFSET_1_6_N	= 3,	/**< frequency offset = -167KHz */
	DI_CH_OFFSET_2_6_P	= 4,	/**< frequency offset = +(167*2)KHz */
	DI_CH_OFFSET_2_6_N	= 5,	/**< frequency offset = -(167*2)KHz */
	DI_CH_OFFSET_3_6_P	= 6,	/**< frequency offset = +(167*3)KHz */
	DI_CH_OFFSET_3_6_N	= 7		/**< frequency offset = -(167*3)KHz */
} CH_TER_Offset_e;


/**
* TER Antenna power on/off
**/
typedef enum
{
	DI_CH_ANT_POWER_OFF 	= 0,	/**< default value */
	DI_CH_ANT_POWER_ON		= 1,	/**< 5 Volt On */
	DI_CH_ANT_POWER_FAULT	= 2 	/**< Antenna short circuit */
} CH_TER_AntStatus_e;


/**
 * TER system.
 */
typedef enum
{
	DI_CH_DVBT 	= 0,	/**< DVB-T */
	DI_CH_DVBT2 = 1		/**< DVB-T2 */
} CH_TER_System_e;


typedef enum
{
	DI_CH_T2SISO = 0,
	DI_CH_T2MISO = 1
} CH_TER_PreambleFormat_e;


typedef enum
{
	DI_CH_T2NOT_MIXED 	= 0,
	DI_CH_T2MIXED 		= 1
} CH_TER_PreambleMixed_e;


typedef enum
{
	DI_CH_T2NOT_USED 	= 0,
	DI_CH_T2USED 		= 1
} CH_TER_ExtendedCarrier_e;


typedef enum
{
	DI_CH_NONE_PAPR		= 0,
	DI_CH_ACE_PAPR		= 1,
	DI_CH_TR_PAPR		= 2,
	DI_CH_ACETR_PAPR	= 3
} CH_TER_PaprMode_e;


typedef enum
{
	DI_CH_T2_PPAUTO	= 0,
	DI_CH_T2_PP1	= 1,
	DI_CH_T2_PP2	= 2,
	DI_CH_T2_PP3	= 3,
	DI_CH_T2_PP4	= 4,
	DI_CH_T2_PP5	= 5,
	DI_CH_T2_PP6	= 6,
	DI_CH_T2_PP7	= 7,
	DI_CH_T2_PP8	= 8
} CH_TER_PilotPattern_e;


/**
 * TER DVB-T2 specific tuning parameters.
 */
typedef struct
{
	HUINT32							ulPlpId;			/**< DVB-T2 PLP Index */
	CH_TER_PreambleFormat_e			etPreambleFormat;
	CH_TER_PreambleMixed_e			etPreambleMixed;
	CH_TER_PilotPattern_e			etPilotPattern;
	CH_TER_ExtendedCarrier_e		etExtendedCarrier;
	CH_TER_PaprMode_e				etPaprMode;
	HUINT32							ulPlpNumBlocks;
}CH_TER_T2Param_t;


/**
 * TER tuning parameters.
 */
typedef struct
{
	HUINT32							ulFrequency;		/**< tuning frequency : Unit [KHz] */
	CH_TER_BandWidth_e				etBandWidth;		/**< 7MHz or 8 MHz */
	CH_TER_Moudulation_e			etModulation;		/**< Modulation, TPS : Transmission Parameter Signalling */
	CH_TER_Hierarchy_e				etHierarchy;		/**< TPS : Transmission Parameter Signalling */
	CH_TER_CodeRate_e				etCodeRate;			/**< TPS : Transmission Parameter Signalling */
	CH_TER_TransMode_e				etTransMode;		/**< TPS : Transmission Parameter Signalling */
	CH_TER_GuardInterval_e			etGuardInterval;	/**< TPS : Transmission Parameter Signalling */
	CH_TER_Stream_e					etStream;			/**< HP or LP */
	CH_TER_Offset_e					etOffset;			/**< AFC, frequency offset */
	CH_TER_System_e					etTerSystem;		/**< DVB-T or T2 */
	CH_TER_T2Param_t				stT2Param;			/**< DVB-T2 specific parameters */

} CH_TER_TuneParam_t;
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
DI_CHANNEL_ERR DI_CH_T_RegisterCallback( HUINT32 ulInstanceId, pfnDI_CH_NotifySignalInfo pfnNotify);



/**
 * MW activate a logical channel and assign a handler associated.
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     Before tuning, activate a channel first.
 *
 * \see         DI_CH_T_DeactivateChannel
 */
DI_CHANNEL_ERR DI_CH_T_ActivateChannel( HUINT32 ulInstanceId );



/**
 * MW deactivate a logical channel and release a handler associated.
 *
 * \param[in]	ulInstanceId		Logical Tuner ID
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     When the connection is not effective, MW has to deactivate the channel.
 *
 * \see         DI_CH_T_ActivateChannel
 */
DI_CHANNEL_ERR DI_CH_T_DeactivateChannel( HUINT32 ulInstanceId );



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
DI_CHANNEL_ERR DI_CH_T_CancelLock( HUINT32 ulInstanceId );



/**
 * MW ask TER tuner to lock.
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 * \param[in]	unRequestId			When send lock event to MW, it is managed by DI.
 * \param[in]	pstParam			tuning parameters
 *
 * \return      OK, or other Error on failure
 *
 * \remarks
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_T_RequestLock( HUINT32 ulInstanceId, HUINT32 unRequestId, CH_TER_TuneParam_t *pstParam );



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
DI_CHANNEL_ERR DI_CH_T_GetLockedInfo( HUINT32 ulInstanceId, CH_TER_TuneParam_t *pstParam );



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
DI_CHANNEL_ERR DI_CH_T_GetSignalInfo ( HUINT32 ulInstanceId, DI_CH_SignalQuality_t *pstSignalInfo );



/******************************************************************************
 * DVB-T2 SYSTEM API
 ******************************************************************************/

/**
 * MW get the number of multiple PLPId.
 *
 * \param[in]		ulInstanceId	Logical Tuner ID
 * \param[out]	pucNumPlps		the number of PLPs.
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     DVB-T2 only. Physical Layer Pipes.(PLP)
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_T_GetDataPlpCount( HUINT32 ulInstanceId, HUINT8 *pucNumPlps );



/**
 * MW get multiple PLP info.
 *
 * \param[in]		ulInstanceId	Logical Tuner ID
 * \param[out]	pucPlpIds		a data pointer that contain PLP info.
 * \param[out]	pucNumPlps		the number of PLPs.
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     DVB-T2 only.  Physical Layer Pipes.(PLP)
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_T_GetDataPlpInfo( HUINT32 ulInstanceId,  HUINT8 *pucPlpIds, HUINT8 *pucNumPlps );


/**
 * MW get Cell Id info.
 *
 * \param[in]		ulInstanceId	Logical Tuner ID
 * \param[out]	pusCellId		a data pointer that contain 16bit Cell Id info.
 *
 * \return      OK, or other Error on failure
 *
 * \remarks
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_T_GetCellId( HUINT32 ulInstanceId, HUINT16 *pusCellId );


/******************************************************************************
 * TER ANTENNA POWER API (OPTIONAL)
 ******************************************************************************/

/**
 * MW set the power of antenna.
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 * \param[in]	etAntState			the value of power
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     TER only
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_T_SetAntennaPower( HUINT32 ulInstanceId, DI_CH_AntStatus_e etAntState );



/**
 * MW get the power of antenna.
 *
 * \param[in]		ulInstanceId			Logical Tuner ID
 * \param[out]	petAntState			return current value of power status
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     TER only
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_T_GetAntennaPower( HUINT32 ulInstanceId, DI_CH_AntStatus_e *petAntState );



/******************************************************************************
 * JAPAN SPECIFIC API
 ******************************************************************************/

/**
 * MW get the tsmf Status.
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     Japan api only
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_T_GetStatus( HUINT32 ulInstanceId);



/**
 * MW get the tsmf Status.
 *
 * \param[in]		ulInstanceId		Logical Tuner ID
 * \param[out]	tsmfStatus		the status of Tsmf
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     Japanese transport stream multiplex frame(TSMF)
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_T_GetTsmfStatus ( HUINT32 ulInstanceId,  int *tsmfStatus);



/**
 * MW get terrestrial EwsInfo.
 *
 * \param[in]		ulInstanceId		Logical Tuner ID
 * \param[out]	ucIsEws			ucIsEws
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     Japan api only, Emergency Warning Signal system. (EWS)
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_T_GetEwsInfo( HUINT32 ulInstanceId,  HUINT8 *ucIsEws );



/**
 * MW init the terrestrial.
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     Japan api only
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_T_Init( HUINT32 ulInstanceId );



/**
 * MW stop the terrestrial.
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     Japan api only
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_T_StopTER( HUINT32 ulInstanceId );



/**
 * MW reset the terrestrial.
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     Japan api only
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_T_ResetGpio( HUINT32 ulInstanceId );



/**
 * MW get terrestrial Agc.
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 * \param[out]	ucRfAgc			ucRfAgc
 * \param[out]	ucIfAgc			ucIfAgc
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     Japan api only
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_T_GetAgc( HUINT32 ulInstanceId,  HUINT8 *ucRfAgc, HUINT8 *ucIfAgc);



/**
 * MW set the terrestrial BER calculate time.
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 * \param[in]	berCalMode			berCalMode
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     Japan api only
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_T_SetBERCalTime( HUINT32 ulInstanceId, HUINT32 berCalMode);



/******************************************************************************
 * TO PROVIDE API FOR HW TEST APP & FACTORY APP
 ******************************************************************************/

/**
 * MW get various signal information( AGC, BER, SNR etc. ).
 *
 * \param[in]	ulInstanceId	Logical Tuner ID
 * \param[out]	nSignalQuality			nSignalQuality
 * \param[out]	nBer					nBer
 * \param[out]	nSignallevel			nSignallevel
 *
 * \return      OK, or other Error on failure
 *
 * \remarks
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_T_GetChannelStatus(HUINT32 ulInstanceId, HUINT32 *nSignalQuality, HUINT32 *nBer, HINT32 *nSignallevel);

/**
 * MW get device status( AVS voltage, temperature ).
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 * \param[out]	pstDeviceStatus			Information of the device status
 *
 * \return      OK, or other Error on failure
 *
 * \remarks
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_T_GetDeviceStatus ( HUINT32 ulInstanceId, DI_CH_DeviceStatus *pstDeviceStatus );


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
DI_CHANNEL_ERR DI_CH_T_ReInitDevice( HUINT32 ulInstanceId );

//JAMES
DI_CHANNEL_ERR DI_CH_T_Requestdump(HUINT32 ulInstanceId);
DI_CHANNEL_ERR DI_CH_T_READ(HUINT32 ulInstanceId, HUINT32 uAddr );
DI_CHANNEL_ERR DI_CH_T_WRITE(HUINT32 ulInstanceId, HUINT32 uAddr, HUINT32 uData);

/**
 * DD enable or disable demod power to control loopthrough during stadby state
 *
 * \param[in]	bHigh			Enable or disable power(loopthrough)
 *
 * \return      OK, or other Error on failure
 *
 * \remarks
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_T_SetLoopthGpio(HBOOL bHigh);

/******************************************************************************
 * Local(static) Function Prototypes
 ******************************************************************************/


#endif /* _DI_CHANNEL_T_H_ */

/** @} */ // end of DI_CH_TER

