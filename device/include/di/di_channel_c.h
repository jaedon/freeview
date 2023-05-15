/**
 * @file	di_channel_c.h
 * @brief	Channel CAB module DI layer header file
 * @version $Revision: 1.0.0 $
 * @date 	$Date: 2010/03/27 12:00:00 $
 * @author 	Linus Lee
 */

/** @defgroup CH_CAB CABLE SYSTEM
 *  This is Cable system of DI layer.
 *  @{
 */

#ifndef _DI_CHANNEL_C_H_
#define _DI_CHANNEL_C_H_
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
 * CAB modulation.
 */
typedef enum
{
	DI_CH_CMOD_AUTO		= 0, 		/**<  Auto search order : 64-> 256-> 128-> 32-> 16 */
	DI_CH_CMOD_16QAM	= 1,		/**<  16 QAM */
	DI_CH_CMOD_32QAM	= 2,		/**<  32 QAM */
	DI_CH_CMOD_64QAM	= 3,		/**<  64 QAM */
	DI_CH_CMOD_128QAM	= 4,		/**<  128 QAM */
	DI_CH_CMOD_256QAM	= 5			/**<  256 QAM */
} CH_CAB_Modulation_e;


/**
 * CAB spectral inversion.
 */
typedef enum
{
	DI_CH_INVERSION_AUTO		= 0,	/**<  Spectral Auto */
	DI_CH_INVERSION_NORMAL		= 1,	/**<  Spectral Normal */
	DI_CH_INVERSION_INVERTED	= 2		/**<  Spectral Inversion */
} CH_CAB_Spectrum_e;


/**
 * CAB tuning parameters.
 */
typedef struct
{
	HUINT32 						ulFrequency;	/**< tuning frequency : Unit [KHz] */
	HUINT32  						ulSymbolRate;	/**< Symbol rate */
	CH_CAB_Modulation_e  			etModulation;	/**< Modulation */
	CH_CAB_Spectrum_e   			etSpectrum;		/**< spectral inversion */

	int 							nNetId;			/**< for Japan project */
	int								nTsId;			/**< for Japan project */
} CH_CAB_TuneParam_t;

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
DI_CHANNEL_ERR DI_CH_C_RegisterCallback( HUINT32 ulInstanceId, pfnDI_CH_NotifySignalInfo pfnNotify);



/**
 * MW activate a logical channel and assign a handler associated.
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     Before tuning, activate a channel first.
 *
 * \see         DI_CH_C_DeactivateChannel
 */
DI_CHANNEL_ERR DI_CH_C_ActivateChannel( HUINT32 ulInstanceId );



/**
 * MW deactivate a logical channel and release a handler associated.
 *
 * \param[in]	ulInstanceId		Logical Tuner ID
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     When the connection is not effective, MW has to deactivate the channel.
 *
 * \see         DI_CH_C_ActivateChannel
 */
DI_CHANNEL_ERR DI_CH_C_DeactivateChannel( HUINT32 ulInstanceId );



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
DI_CHANNEL_ERR DI_CH_C_CancelLock( HUINT32 ulInstanceId );



/**
 * MW ask CAB tuner to lock.
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
DI_CHANNEL_ERR DI_CH_C_RequestLock( HUINT32 ulInstanceId, HUINT32 ulRequestId, CH_CAB_TuneParam_t *pstParam );



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
DI_CHANNEL_ERR DI_CH_C_GetLockedInfo( HUINT32 ulInstanceId, CH_CAB_TuneParam_t * pstParam );



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
DI_CHANNEL_ERR DI_CH_C_GetSignalInfo ( HUINT32 ulInstanceId, DI_CH_SignalQuality_t *pstSignalInfo );



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
DI_CHANNEL_ERR DI_CH_C_GetDeviceStatus ( HUINT32 ulInstanceId, DI_CH_DeviceStatus *pstDeviceStatus );



/******************************************************************************
 * JAPAN SPECIFIC API
 ******************************************************************************/

/**
 * MW get various signal information( AGC, BER, SNR etc. ).
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 * \param[in]	ulFrequency			ulFrequency
 *
 * \return      OK, or other Error on failure
 *
 * \remarks
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_C_GetStatus( HUINT32 ulInstanceId, HUINT32 ulFrequency);



/**
 * MW get various signal information( AGC, BER, SNR etc. ).
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 * \param[out]	ucTsNo			ucTsNo
 * \param[out]	nTsId			nTsId
 * \param[out]	nNetId			nNetId
 *
 * \return      OK, or other Error on failure
 *
 * \remarks	Japanese transport stream multiplex frame(TSMF)
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_C_GetTsmfInfo ( HUINT32 ulInstanceId, HUINT16 *ucTsNo, HINT32 *nTsId, HINT32 *nNetId);



/**
 * MW get various signal information( AGC, BER, SNR etc. ).
 *
 * \param[in]		ulInstanceId			Logical Tuner ID
 * \param[in] 		ulFrequency			ulFrequency
 *
 * \return      OK, or other Error on failure
 *
 * \remarks	Japanese transport stream multiplex frame(TSMF)
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_C_CheckTsmfLock( HUINT32 ulInstanceId, HUINT32 ulFrequency );



/**
 * MW get various signal information( AGC, BER, SNR etc. ).
 *
 * \param[in]		ulInstanceId			Logical Tuner ID
 * \param[out]	ucIsEws				ucIsEws
 *
 * \return      OK, or other Error on failure
 *
 * \remarks	Japan api only, Emergency Warning Signal system. (EWS)
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_C_GetEwsInfo( HUINT32 ulInstanceId,  HUINT8 *ucIsEws );



/**
 * MW get various signal information( AGC, BER, SNR etc. ).
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 *
 * \return      OK, or other Error on failure
 *
 * \remarks
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_C_Init( HUINT32 ulInstanceId );



/**
 * MW get various signal information( AGC, BER, SNR etc. ).
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 *
 * \return      OK, or other Error on failure
 *
 * \remarks
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_C_Stop( HUINT32 ulInstanceId );



/**
 * MW get various signal information( AGC, BER, SNR etc. ).
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 *
 * \return      OK, or other Error on failure
 *
 * \remarks
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_C_ResetGpio( HUINT32 ulInstanceId );



/**
 * MW get various signal information( AGC, BER, SNR etc. ).
 *
 * \param[in]		ulInstanceId			Logical Tuner ID
 * \param[out]	ucRfAgc				ucRfAgc
 * \param[out]	ucIfAgc				ucIfAgc
 *
 * \return      OK, or other Error on failure
 *
 * \remarks
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_C_GetAgc( HUINT32 ulInstanceId,  HUINT8 *ucRfAgc, HUINT8 *ucIfAgc);



/******************************************************************************
 * TO PROVIDE API FOR HW TEST APP & FACTORY APP
 ******************************************************************************/

/**
 * MW get various signal information( AGC, BER, SNR etc. ).
 *
 * \param[in]		ulInstanceId			Logical Tuner ID
 * \param[out]	nSnr					nSnr
 * \param[out]	nber_decimal			nber_decimal
 * \param[out]	nber_float			nber_float
 * \param[out]	nber_exp				nber_exp
 * \param[out]	nUncorrected			nUncorrected
 * \param[out]	nSignallevel			nSignallevel
 *
 * \return      OK, or other Error on failure
 *
 * \remarks	It will be deprecated
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_C_GetLockInfo ( HUINT32 ulInstanceId, HINT32 *nSnr, HINT32 *nber_decimal, HINT32 *nber_float, HINT32 *nber_exp, HINT32 *nUncorrected, HINT32 *nSignallevel);



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
DI_CHANNEL_ERR DI_CH_C_ReInitDevice( HUINT32 ulInstanceId );


/******************************************************************************
 * Local(static) Function Prototypes
 ******************************************************************************/


#endif /* _DI_CHANNEL_C_H_ */

/** @} */ // end of DI_CH_CAB
