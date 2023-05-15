/*************************************************************************************
 *
 *	TDA182I2Instance.h
 *
 *  QAM receiver : 10024(NXP)
 *  Tuner : FJ2207(NuTune)
 *
 *************************************************************************************/

#ifndef _TDA182I2_H_
#define _TDA182I2_H_
#include "htype.h"

// for tmTDA18251Config_t

#define TDA182I2_ADD						0xC0
#define	TUNER_MAX_NUM_OF_DATA		60
#define	TDA182I2_I2C_MAP_NB_BYTES           68
#define TDA182I2_POWER_LEVEL_MIN  			40
#define TDA182I2_POWER_LEVEL_MAX	  		110
#define	tmTDA182I2_AGC3_RF_AGC_TOP_FREQ_LIM		 	291000000

typedef enum TDA182I2_SetPower
{
	tmPowerOn,							// Device powered on	  (D0 state)
	tmPowerStandby, 					// Device power standby   (D1 state)
	tmPowerSuspend, 					// Device power suspended (D2 state)
	tmPowerOff							// Device powered off	  (D3 state)
}TDA182I2_SetPower_t, *pTDA182I2_SetPower_t;

typedef enum {
	tmNOERR,
	tmERR_BAD_UNIT_NUMBER,
	tmERR_NOT_INITIALIZED,
	tmERR_INIT_FAILED,
	tmERR_BAD_PARAMETER,
	tmERR_NOT_SUPPORTED,
	tmERR_SET_FAILED,
	tmERR_NOT_READY,
	tmERR_BAD_VERSION
}TDA182I2_ERR_CODE;

typedef enum _TDA182I2FrontEndState_t
{
	/** status Unknown */
	tmFrontEndStateUnknown = 0,
	/** Channel locked*/
	tmFrontEndStateLocked,
	/** Channel not locked */
	tmFrontEndStateNotLocked,
	/** Channel lock in process */
	tmFrontEndStateSearching,
	tmFrontEndStateMax
} TDA182I2FrontEndState_t, *pTDA182I2FrontEndState_t;

typedef enum _TDA182I2StandardMode_t {
	tmTDA182I2_DVBT_6MHz = 0,						/* Digital TV DVB-T 6MHz */
	tmTDA182I2_DVBT_7MHz,							/* Digital TV DVB-T 7MHz */
	tmTDA182I2_DVBT_8MHz,							/* Digital TV DVB-T 8MHz */
	tmTDA182I2_QAM_6MHz,							/* Digital TV QAM 6MHz */
	tmTDA182I2_QAM_8MHz,							/* Digital TV QAM 8MHz */
	tmTDA182I2_ISDBT_6MHz,							/* Digital TV ISDBT 6MHz */
	tmTDA182I2_ATSC_6MHz,							/* Digital TV ATSC 6MHz */
	tmTDA182I2_DMBT_8MHz,							/* Digital TV DMB-T 8MHz */
	tmTDA182I2_StandardMode_Max
} TDA182I2StandardMode_t, *pTDA182I2StandardMode_t;

typedef struct _TDA182I2Request_t
{
	TDA182I2FrontEndState_t	eTunerLock;
	HUINT32					dwFrequency;
	TDA182I2StandardMode_t	dwStandard;
	HUINT8					dwInputLevel;
}TDA182I2Request_t, *pTDA182I2Request_t;

typedef enum{
	TRT_NONE		= 0x00000000,
	TRT_ATSC		= 0x00000001,
	TRT_DVBT		= 0x00000002,
	TRT_DVBC		= 0x00000004,
	TRT_DVBS		= 0x00000008,
	TRT_ANALOG_FM	= 0x00000010,
	TRT_ANALOG_TV	= 0x00000020,
	TRT_DCABLE		= 0x00000040,
	TRT_ISDBT		= 0x00000080,
	TRT_ISDBC		= 0x00000100,
	TRT_ISDBS		= 0x00000200,
	TRT_TUNER_ONLY	= 0x00000400,
	TRT_UNKNOWN 	= 0x0000FFFF
} TuneReqType_t;

TDA182I2_ERR_CODE TDA182I2Init(   HUINT32 tUnit);
TDA182I2_ERR_CODE TDA182I2SendRequest ( HUINT32 tUnit, pTDA182I2Request_t pTuneRequest, TuneReqType_t tTuneReqType);
TDA182I2_ERR_CODE TDA182I2Reset( HUINT32 tUnit);
TDA182I2_ERR_CODE TDA182I2GetAGC1_Gain_Read( HUINT32 tUnit, HUINT8* puValue);
TDA182I2_ERR_CODE TDA182I2GetAGC4_Gain_Read( HUINT32 tUnit, HUINT8* puValue);


#endif
