/**
 * @file	di_cm.h
 * @brief	CM module DI layer header file
 * @version $Revision: 0.9.0 $
 * @date 	$Date: 2008/06/24 12:00:00 $
 * @author 	nhlee
 */

/** @defgroup	DI_CM	CM COMMON IF OF DI LAYER
 *  This is CM module of DI layer.
 *  @{
 */

#ifndef _DI_CM_H_
#define _DI_CM_H_
/******************************************************************************
 * Include Files
 ******************************************************************************/
#include "htype.h"

/******************************************************************************
 * Symbol and Macro Definitions
 ******************************************************************************/
#define NUM_OF_DS_CHANNEL	CONFIG_CABLE_MODEM_MAX_DS
#define NUM_OF_US_CHANNEL	CONFIG_CABLE_MODEM_MAX_US

/******************************************************************************
 * Local(static) Data Types
 ******************************************************************************/

typedef enum _DI_CM_SNMP
{
	SNMP_eCmMacInfo=1,
	SNMP_eCmIpInfo=2,
	SNMP_eCmCmtsIpInfo=3,
	SNMP_eCmDSFreq=4,
	SNMP_eCmDSPower=5,
	SNMP_eCmDSSnr=6,
	SNMP_eCmUSFreq=7,
	SNMP_eCmUSPower=8,
	SNMP_eCmUSMod=9,
	SNMP_eCmUSSymRate=10,
	SNMP_eCmSerialNum=11,
	SNMP_eCmGlueVer=12,
	SNMP_eCmMwVer=13,
	SNMP_eCmCmVer=14,
	SNMP_eCmHwVer=15,
	SNMP_eCmLoaderVer=16,
	SNMP_eCmCurrentFreq=17,
	SNMP_eCmModulation=18,
	SNMP_eCmPowerLevel=19,
	SNMP_eCmInbandSnr=20,
	SNMP_eCmCurrentState=21,
	SNMP_eCmTotalWorkingTime=22,
	SNMP_eCmCcManufacturer=23,
	SNMP_eCmCcMacAddress=24,
	SNMP_eCmCcIpAddress=25,
	SNMP_eCmCcCertiState=26,
	SNMP_eCmSysSerialNumInfo=27,
	SNMP_eCmSysSystemIdInfo=28,
	SNMP_eCmSysHostIdInfo=29,
	SNMP_eCmSysCmMacInfo=30,
	SNMP_eCmSysHostMacInfo=31,
	SNMP_eCmSysCcMacInfo=32,
	SNMP_eCmSysCmIpInfo=33,
	SNMP_eCmSysHostIpInfo=34,
	SNMP_eCmSysCcIpInfo=35,
	SNMP_eCmVideoCurrentChannelNum=36,
	SNMP_eCmVideoWatchingTime=37,
	SNMP_eCmVideoBitStreamError=38,
	SNMP_eCmAudioFormat=39,
	SNMP_eCmAudioSyncStatus=40,
	SNMP_eCmAudioSampleRate=41,
	SNMP_eCmCmReset=42,
	SNMP_eCmStbReset=43,
	SNMP_eCmCcReset=44,
	SNMP_eCmFactoryReset=45,
	SNMP_eCmStbChangeState=46,
	SNMP_eCmSystemId=47,
	SNMP_eCmAppVersion=48,
	SNMP_eCmMwVersion=49,
	SNMP_eCmBrowserVersion=50,
	SNMP_eCmStbId=51,
	SNMP_eCmBcasId=52,
	SNMP_eCmCcasId=53,
	SNMP_eCmMacAddress=54,
	SNMP_eCmIpaddress=55,
	SNMP_eCmNetMask=56,
	SNMP_eCmDsPower=57,
	SNMP_eCmDsFrq=58,
	SNMP_eCmServiceId=59,
	SNMP_eCmNetworkId=60,
	SNMP_eCmUnerroredNo=61,
	SNMP_eCmCorrectedNo=62,
	SNMP_eCmUncorrectedNo=63,
	SNMP_eCmInbandBer=64,
	SNMP_eCmPowerStatus=65,
	SNMP_eCmPlayStatus=66,
	SNMP_eCmTuner1ChId=67,
	SNMP_eCmTuner1NetId=68,
	SNMP_eCmTuner1Frq=69,
	SNMP_eCmTuner2ChId=70,
	SNMP_eCmTuner2NetId=71,
	SNMP_eCmTuner2Frq=72
}DI_CM_Snmp_t;

typedef enum _DI_CM_FDC_MODULATION
{
   DI_CM_FDC_MOD_ERROR = 0,
   DI_CM_FDC_MOD_UNKNOWN,
   DI_CM_FDC_MOD_OTHER,
   DI_CM_FDC_MOD_QAM64,
   DI_CM_FDC_MOD_QAM256
} DI_CM_FdcModulation_e;

typedef enum _DI_CM_RDC_MODULATION
{
   DI_CM_RDC_MOD_ERROR = -1,
   DI_CM_RDC_MOD_UNKNOWN = 0,
   DI_CM_RDC_MOD_TDMA,
   DI_CM_RDC_MOD_ATDMA,
   DI_CM_RDC_MOD_SCDMA,
   DI_CM_RDC_MOD_TDMAANDATDMA
} DI_CM_RdcModulation_e;

typedef enum _DI_CM_RDC_RANGE
{
   DI_CM_RDC_RANGE_ERROR = 0,
   DI_CM_RDC_RANGE_OTHER = 1,
   DI_CM_RDC_RANGE_ABORTED,
   DI_CM_RDC_RANGE_RETRIESEXCEEDED,
   DI_CM_RDC_RANGE_SUCCESS,
   DI_CM_RDC_RANGE_CONTINUE,
   DI_CM_RDC_RANGE_TIMEOUTT4
} DI_CM_RdcRanging_e;

typedef enum _DI_CM_FAN_CONTROL
{
   DI_CM_FAN_OFF = 0,
   DI_CM_FAN_LOW_SPEED = 1,
   DI_CM_FAN_HIGH_SPEED,
} DI_CM_FanControl_e;

typedef enum _DI_CM_STANDBY_MODE
{
   DI_CM_LED_TURN_ON = 0,
   DI_CM_LED_TURN_OFF = 1,
} DI_CM_LEDControl_e;

enum
{
	Event_CDFilename = 40			// equal to kEventCDFilename
	,Event_CDIpAddress				// equal to kEventCDIpAddress
	,Event_eCMSNMPGetRequest		// equal to KEventeCMSNMPGetRequest
	,Event_eCMSNMPSetRequest		// equal to KEventeCMSNMPSetRequest
	,Event_CmSerialNum				// equal to kEventCmSerialNum
	,Event_MpegMacAddr				// equal to kEventMpegMacAddr
};


enum
{
	Api_DsgMwVersion = 50			// equal to kDsgMwVersion
	,Api_DsgDocsisCDStart			// equal to kDsgDocsisCDStart
	,Api_DsgSetScanFreq				// equal to kDsgSetScanFreq
	,Api_eCMSNMPGetResponse			// equal to KEventeCMSNMPGetResponse
	,Api_eCMSNMPSetResponse			// equal to KEventeCMSNMPSetResponse
	,Api_StbStandByMode				// equal to kStbStandByMode
	,Api_ReceivedCmSerialNumber		// equal to kReceivedCmSerialNumber
	,Api_ReceivedMpegMacAddr		// equal to kReceivedMpegMacAddr
	,Api_ReceivedCmSavedTemp		// equal to kReceivedCmSavedTemp
	,Api_kReceivedCmMacAddr
	,Api_kDsgSetFanControl
	,Api_kDsgSetDefaultTemp
	,Api_kDsgSetDefaultSnmp
	,Api_kDsgSetDefaultSavedFrq
	,Api_kReceivedCmNetmask
	,Api_kReceivedCmGateway
};


typedef enum
{
	DI_CM_EVENT_ONLINE 	= 0,
	DI_CM_EVENT_OFFLINE	=1
} DI_CM_LinkInfo_e;


typedef struct
{
	HUINT32		ulDsStrength;			/**< DS Signal strength (%) */
	HUINT32		ulDsQuality;			/**< Signal quality (%) */

	HFLOAT32	fDsBer;					/**< BER */
	HFLOAT32	fDsSnr;					/**< SNR */

	HUINT32		ulDsCorrectedNo;		/**< ulCorrectedNo */
	HUINT32		ulDsUnCorrectedNo;		/**< ulUnCorrectedNo */

	HFLOAT32	fDsInputPower;			/**< Downstream Power Level  (dBuV) */
	HUINT32		ulDsFrequency;			/* Downstream Frequency (Hz) */
	HUINT32 	ulDsModulation;			/* Downstream Modulation */
} DI_CM_DsSignalQuality_t;

typedef struct
{
	HFLOAT32	fUsInputPower;			/* Upstream Power level (dBuV) */
	HUINT32		ulUsFrequency;			/* Upstream Frequency (Hz) */
	HUINT32 	ulUsModulation;			/* Upstream Modulation */
	HUINT32		ulUsBandwidth;			/* Upstream Bandwidth (Hz) */
	HUINT32		ulUsChannelId;			/* Upstream Channel ID */
	HUINT32		ulUsStrength;			/* Upstream Strength (%) */
} DI_CM_UsSignalQuality_t;

typedef void ( *pfnDI_CM_NotifyLinkInfo )(DI_CM_LinkInfo_e etLinkInfo );

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
DI_ERR_CODE DI_CM_GetCmIp(HUINT32 *pulCmIp);
DI_ERR_CODE DI_CM_GetCmMac(HUINT8 *pucCmMac);
DI_ERR_CODE DI_CM_SetCmMac(HUINT8 *pucCmMac);
DI_ERR_CODE DI_CM_GetCmVer(HCHAR *pcCmVer);
DI_ERR_CODE DI_CM_GetNetmask(HUINT32 *pulNetmask);
DI_ERR_CODE DI_CM_GetGateway(HUINT32 *pulGateway);

HINT32 DI_CM_CmStatus(void);

DI_ERR_CODE DI_CM_GetFDCFreq(HINT32 *pnFdcFreq);
DI_ERR_CODE DI_CM_GetFDCPower(HINT32 *pnFdcPwr);
DI_ERR_CODE DI_CM_GetFDCSnr(HINT32 *pnFdcSnr);
DI_ERR_CODE DI_CM_GetFDCMod(HINT32 *pnFdcMod);

DI_ERR_CODE DI_CM_GetRDCFreq(HINT32 *pnRdcFreq);
DI_ERR_CODE DI_CM_GetRDCPower(HUINT32 *pulRdcPwr);
DI_ERR_CODE DI_CM_GetRDCChannelId(HINT32 *pnRdcChId);

DI_ERR_CODE DI_CM_GetFDCFreq30(HINT32 nFdcId, HINT32 *pnFdcFreq);
DI_ERR_CODE DI_CM_GetFDCPower30(HINT32 nFdcId, HINT32 *pnFdcPwr);
DI_ERR_CODE DI_CM_GetFDCSnr30(HINT32 nFdcId, HINT32 *pnFdcSnr);
DI_ERR_CODE DI_CM_GetFDCBer30(HINT32 nFdcId, HUINT32 *pnFdcBer);
DI_ERR_CODE DI_CM_GetFDCChannelId30(HINT32 nFdcId, HINT32 *pnFdcChId);
DI_ERR_CODE DI_CM_GetFDCMod30(HINT32 nFdcId, HINT32 *pnFdcMod);
DI_ERR_CODE DI_CM_GetFDCCorrected30(HINT32 nFdcId, HUINT32 *pulFdcCorr);
DI_ERR_CODE DI_CM_GetFDCUncorrected30(HINT32 nFdcId, HUINT32 *pulFdcUncorr);
DI_ERR_CODE DI_CM_GetDsSignalInfo(HUINT32 ulFdcId, DI_CM_DsSignalQuality_t *pstDsSignalInfo);

DI_ERR_CODE DI_CM_GetRDCFreq30(HINT32 nRdcId, HINT32 *pnRdcFreq);
DI_ERR_CODE DI_CM_GetRDCPower30(HINT32 nRdcId, HUINT32 *pulRdcPower);
DI_ERR_CODE DI_CM_GetRDCRange30(HINT32 nRdcId, HINT32 *pnRdcRange);
DI_ERR_CODE DI_CM_GetRDCChannelId30(HINT32 nRdcId, HINT32 *pnRdcChId);
DI_ERR_CODE DI_CM_GetRDCMod30(HINT32 nRdcId, HINT32 *pnRdcMod);
DI_ERR_CODE DI_CM_GetRDCBandWidth30(HINT32 nRdcId, HINT32 *pnRdcBw);
DI_ERR_CODE DI_CM_GetRDCSymbolRate30(HINT32 nRdcId, HUINT32 *pulRdcSymbolRate);
DI_ERR_CODE DI_CM_GetUsSignalInfo(HUINT32 ulRdcId, DI_CM_UsSignalQuality_t *pstUsSignalInfo);

DI_ERR_CODE DI_CM_GetScanDSFreq(HCHAR *pszDsFreq);
DI_ERR_CODE DI_CM_SetScanDSFreq(HUINT32 *pulDsFreq);


DI_ERR_CODE DI_CM_GetCmSavedTemp(HINT32 *pnCmSavedTemp, HINT32 *pnCmCurrentTemp);
DI_ERR_CODE DI_CM_SetTemp(HINT32 nTempMode);

DI_ERR_CODE DI_CM_SetStandbyMode(HINT32 nStandbyCmLed);

DI_ERR_CODE DI_CM_SetFan(HINT32 nFanMode);

DI_ERR_CODE DI_CM_SetCmDefaults(void);

DI_ERR_CODE DI_CM_GetCmSerial( HCHAR *pszCmSerialNo);
DI_ERR_CODE DI_CM_SetCmSerial( HCHAR *pszCmSerialNo);

DI_ERR_CODE DI_CM_SwUpgrade(void);

DI_ERR_CODE DI_CM_GetMpegIp(HCHAR *pcMpegIp);

DI_ERR_CODE DI_CM_RegisterCallback(pfnDI_CM_NotifyLinkInfo pfnNotify);

void DI_CM_HwGpioReset(void);

DI_ERR_CODE DI_CM_Init(void);


/******************************************************************************
 * Local(static) Function Prototypes
 ******************************************************************************/


#endif /* _DI_CM_H_ */

/** @} */ // end of DI_CM
