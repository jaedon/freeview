/**
 * @file	di_channel.h
 * @brief	Channel module DI layer header file
 * @version $Revision: 1.0.0 $
 * @date 	$Date: 2010/03/27 12:00:00 $
 * @author 	Linus Lee
 */


/** @defgroup	DI_CHANNEL	CHANNEL COMMON IF OF DI LAYER
 *  This is CHANNEL module of DI layer.
 *  @{
 */

#ifndef _DI_CHANNEL_H_
#define _DI_CHANNEL_H_
/******************************************************************************
 * Include Files
 ******************************************************************************/
#include "htype.h"

/******************************************************************************
 * Symbol and Macro Definitions
 ******************************************************************************/

/**
 * Define channel error code.
 */
typedef enum
{
	DI_CH_ERR 				= -1,	/**< Return Error */
	DI_CH_OK				= 0,	/**< Return OK */
	DI_CH_PARAM_ERR			= 1,	/**< Return parameter error */
	DI_CH_NOT_SUPPORTED		= 2,	/**< Return not supported */
	DI_CH_HW_RESTRICTION 	= 3,	/**< Return HW restriction */
	DI_CH_CANCEL_TUNE 		= 4		/**< MW cancel tuning */
}DI_CHANNEL_ERR;


/**
 * Define which type of tuner.
 */
typedef enum
{
	DI_CH_TYPE_SAT = 0x1,			/**< Satellite channel */
	DI_CH_TYPE_TER = 0x2,			/**< Terrestrial channel */
	DI_CH_TYPE_CAB = 0x4,			/**< Cable channel */
	DI_CH_TYPE_ANA = 0x8			/**< Analog channel */
} DI_CH_Type_e;


/**
 * Define signal info which is used for callback param.
 */
typedef enum
{
	DI_CH_SIGNAL_LOCKED			= 0,	/**< Carrier locked */
	DI_CH_SIGNAL_UNLOCK			= 1,	/**< Carrier unlocked */
	DI_CH_SIGNAL_ANTENNA_FAULT	= 2,	/**< LNB fault or Antenna fault */
	DI_CH_SIGNAL_ANTENNA_OK		= 3,	/**< LNB or Antenna fault is recovered */
	DI_CH_SIGNAL_TSMF_GOOD		= 4,	/**< For Japan */
	DI_CH_SIGNAL_TSMF_BAD		= 5,	/**< For Japan */
	DI_CH_SIGNAL_TSMF_EWS_ON	= 6,	/**< For Japan */
	DI_CH_SIGNAL_TSMF_EWS_OFF	= 7		/**< For Japan */
} DI_CH_SignalInfo_e;


/**
 * Antenna power on/off.
 */
typedef enum
{
	DI_CH_ANTENNA_OFF		= 0, 	/**< Power Off */
	DI_CH_ANTENNA_ON		= 1,	/**< Power On */
	DI_CH_ANTENNA_FAULT	= 2		/**< Antenna short circuit */
} DI_CH_AntStatus_e;

/**
 * RF Input Index of multi tuner platform.
 */
typedef enum
{
	DI_CH_RF_INPUT_NONE	= -1,
	DI_CH_RF_INPUT1		= 0,		/**< Input source : 1 */
	DI_CH_RF_INPUT2		= 1,		/**< Input source : 2 */
	DI_CH_RF_INPUT3		= 2,		/**< Input source : 3 */
	DI_CH_RF_INPUT4		= 3,		/**< Input source : 4 */
	DI_CH_RF_INPUTLT	= 4,		/**< Input source : External Loop-Through */
	DI_CH_RF_INPUTMAX
} DI_CH_InputSource_e;

/**
 * Define signal info which is used for callback param.
 */
typedef struct
{
	HUINT32		ulStrength;			/**< Signal strength percentage Unit */
	HUINT32		ulQuality;			/**< Signal quality percentage Unit */
	HUINT32		ulAgc;				/**< AGC - Half NIM */
	float		fBer;				/**< BER */
	float		fSnr;				/**< SNR */

	HUINT8		bEmergencyflag;		/**< bEmergencyflag */

	HUINT32		ulCorrectedNo;		/**< ulCorrectedNo */
	HUINT32		ulUnCorrectedNo;	/**< ulUnCorrectedNo */
	HUINT32		ulUnErroredNo;		/**< ulUnErroredNo */
	HUINT32		ulInbandBer;		/**< ulInbandBer */
	HUINT32		ulUnCorrectedFlag;	/**< ulUnCorrectedFlag */

	HUINT32 	ulRfAgc;			/**< Dual AGC */
	HUINT32 	ulIfAgc;			/**< Dual AGC */

	float		fSignalInputPower;	/**< Uint is Flexible (dBm, dBmV, dBuV) */
	float		fEbNo;				/**< EbNo */
} DI_CH_SignalQuality_t;



typedef struct DI_CH_DeviceStatus
{
    struct {
        HBOOL enabled;     /* If true, AVS(Automatic Voltage Scaling) is enabled on the device. */
        HUINT32 voltage; /* chip voltage in units of millivolts. This is only valid if enabled= true. */
    } avs;
    int temperature;      /* chip temperature in units of 1/1000 degrees Celsius. */
} DI_CH_DeviceStatus;



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
 * MW get total number of logical channels.
 *
 * \param[out]	pulNumInstance	Total number of logical Tuners
 *
 * \return      OK, or other Error on failure
 *
 * \remarks
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_GetNumOfInstance( HUINT32 *pulNumInstance );



/**
 * MW get a capability of a logical channel.
 *
 * \param[in]	ulInstanceId		Logical Tuner ID
 * \param[out]	pulChannelType	Type of channel
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     Before MW call this function, DI layer have to initialize all params.
 *
 * \see
 */
DI_CHANNEL_ERR DI_CH_GetCapability( HUINT32 ulInstanceId, HUINT32 *pulChannelType );



/**
 * Callback function for status of lock.
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 * \param[in]	ulRequestId			sync btw MW and DI
 * \param[in]	eChannelType		type of channel
 * \param[in]	etSignalInfo		event which is send to MW
 *
 * \return
 *
 * \remarks
 *
 * \see         DI_CH_SignalInfo_e
 */
typedef void ( *pfnDI_CH_NotifySignalInfo )( HUINT32 ulInstanceId, HUINT32 ulRequestId, DI_CH_Type_e etChannelType , DI_CH_SignalInfo_e etSignalInfo );



/**
 * Main call. Initialize channel resources.
 *
 * \return      DI_CH_OK, or other Error on failure
 *
 * \remarks
 *
 * \see
 */
DI_CHANNEL_ERR MAIN_CH_InitChannel (void);



/**
 * Main call. Initialize channel resources.
 *
 * \return      DI_CH_OK, or other Error on failure
 *
 * \remarks
 *
 * \see
 */
DI_CHANNEL_ERR MAIN_CH_ReInitChannel (void);



/**
 * Main call. UnInitialize channel resources.
 *
 * \return      DI_CH_OK, or other Error on failure
 *
 * \remarks
 *
 * \see
 */
DI_CHANNEL_ERR MAIN_CH_UnInitChannel (void);




/******************************************************************************
 * Local(static) Function Prototypes
 ******************************************************************************/


#endif /* _DI_CHANNEL_H_ */

/** @} */ // end of DI_CHANNEL
