/**
 * @file	di_channel_a.h
 * @brief	Channel Analog module DI layer header file
 * @version $Revision: 0.9.0 $
 * @date 	$Date: 2008/06/24 12:00:00 $
 * @author 	Linus Lee
 */

/** @defgroup DI_CH_ANALOG
 *  This is CHANNEL module of DI layer.
 *  @{
 */

#ifndef _DI_CHANNEL_A_H_
#define _DI_CHANNEL_A_H_
/******************************************************************************
 * Include Files
 ******************************************************************************/
#include "htype.h"
#include "di_channel.h"


/******************************************************************************
 * Symbol and Macro Definitions
 ******************************************************************************/

/******************************************************************************
 * Data Types
 ******************************************************************************/
/**
 * tuner freq. step
 */
#define _FREQ_STEP				(HUINT16)(62500)
#define _FREQ_REG(x)				(HUINT32)(x/_FREQ_STEP)
#define _FREQ_KHz(x)				(HUINT32)(x*1000)
#define _FREQ_MHz(x)				(HUINT32)(x*1000000)
#define _FREQ_AFC(x)				(HINT32)(x*_FREQ_STEP)


/**
 * frequency range
 */
#define DI_CH_FREQ_MIN				_FREQ_MHz(44.25)
#define DI_CH_FREQ_MAX				_FREQ_MHz(864.25)

/**
 * aft step
 */
#define DI_CH_AFT_STEP_MIN			_FREQ_REG(0)
#define DI_CH_AFT_STEP_MID			_FREQ_REG(2000000)
#define DI_CH_AFT_STEP_MAX			_FREQ_REG(4000000)


/**
 * Analog system type.
 */
typedef enum
{
	DI_CH_SYSTEM_BG	= 0,
	DI_CH_SYSTEM_I		= 1,
	DI_CH_SYSTEM_DK	= 2,
	DI_CH_SYSTEM_L		= 3,
	DI_CH_SYSTEM_LP	= 4,
	DI_CH_SYSTEM_NUM	= 5
} CH_ANA_System_e;

/**
 * Tune mode.
 */
typedef enum
{
	DI_CH_A_SET_TUNE		= 0,			/* channel */
	DI_CH_A_SET_FREQ		= 1,			/* finetune */
	DI_CH_A_SET_SYSTEM	= 2,			/* system */
	DI_CH_A_SET_SEARCH		= 3,			/* search */
	DI_CH_RESERVED			= 4
} CH_ANA_TuneMode_e;

/**
 * Analog tuning parameters.
 */
typedef struct
{
	HUINT32					ulFrequency;
	HUINT32					ulFreqOffset;
	CH_ANA_System_e			etAnalogSystem;
	HBOOL					bEnableAft;
	CH_ANA_TuneMode_e		etTuneMode;
} CH_ANA_TuneParam_t;


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
DI_CHANNEL_ERR DI_CH_A_RegisterCallback( HUINT32 ulInstanceId, pfnDI_CH_NotifySignalInfo pfnNotify);


/**
 * MW activate a logical channel and assign a handler associated.
 *
 * \param[in]	ulInstanceId			Logical Tuner ID
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     Before tuning, activate a channel first.
 *
 * \see         DI_CH_A_DeactivateChannel
 */
DI_CHANNEL_ERR DI_CH_A_ActivateChannel( HUINT32 ulInstanceId );


/**
 * MW deactivate a logical channel and release a handler associated.
 *
 * \param[in]	ulInstanceId		Logical Tuner ID
 *
 * \return      OK, or other Error on failure
 *
 * \remarks     When the connection is not effective, MW has to deactivate the channel.
 *
 * \see         DI_CH_A_ActivateChannel
 */
DI_CHANNEL_ERR DI_CH_A_DeactivateChannel( HUINT32 ulInstanceId );


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
DI_CHANNEL_ERR DI_CH_A_CancelLock( HUINT32 ulInstanceId );


/**
 * MW ask Analog tuner to lock.
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
DI_CHANNEL_ERR DI_CH_A_RequestLock( HUINT32 ulInstanceId, HUINT32 ulRequestId, CH_ANA_TuneParam_t * pstParam );


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
DI_CHANNEL_ERR DI_CH_A_GetLockedInfo( HUINT32 ulInstanceId, CH_ANA_TuneParam_t * pstParam);


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
DI_CHANNEL_ERR DI_CH_A_GetSignalInfo ( HUINT32 ulInstanceId, DI_CH_SignalQuality_t *pstSignalInfo );


/******************************************************************************
 * Local(static) Function Prototypes
 ******************************************************************************/

#endif /* _DI_CHANNEL_A_H_ */

/** @} */ // end of DI_CH_ANALOG
