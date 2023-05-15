/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brpc_3255.h $
 * $brcm_Revision: Hydra_Software_Devel/45 $
 * $brcm_Date: 8/15/12 5:27p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/rpc/brpc_3255.h $
 * 
 * Hydra_Software_Devel/45   8/15/12 5:27p nickh
 * SW7425-3077: Add comments for 3383 Power Management support
 * 
 * Hydra_Software_Devel/44   7/26/12 3:09p jtna
 * SW7425-3514: add frontend GetTemperature APIs
 * 
 * Hydra_Software_Devel/43   7/12/12 2:28p nickh
 * SW7425-3232: Add LNA reconfiguration RPC
 * 
 * Hydra_Software_Devel/42   6/15/12 11:10a nickh
 * SW7425-3247: Add host video channel lock status RPC
 * 
 * Hydra_Software_Devel/41   4/11/12 2:48p nickh
 * SW7425-2512: Add Output1 and Output2 Stage2 Tilt
 * 
 * Hydra_Software_Devel/40   2/24/12 4:56p nickh
 * SW7425-2436: Add RPC member for retrieving last succseffully registered
 * Docsis frequency
 * 
 * Hydra_Software_Devel/39   1/25/12 4:41p jtna
 * SW7425-2259: add new RPC calls for 3383 XPT register R/W
 * 
 * Hydra_Software_Devel/38   12/19/11 5:27p nickh
 * SW7425-2023: Add new SetParserBandId RPC call
 * 
 * Hydra_Software_Devel/37   9/12/11 4:45p jtna
 * SW7425-1238: add TSMF support
 * 
 * Hydra_Software_Devel/36   6/27/11 6:13p nickh
 * SW7425-771: Add support for 8 TNR tuners
 * 
 * Hydra_Software_Devel/35   6/23/11 11:58p nickh
 * SW7425-771: Add support for 8 tuners
 * 
 * Hydra_Software_Devel/34   4/1/11 4:21p haisongw
 * SW7125-772: Adding OOB powerlevel in OOB status
 * 
 * Hydra_Software_Devel/33   2/17/11 5:00p haisongw
 * SW7125-839: CLONE -Provide an API for the host to read LNA info when
 * the LNA is controlled by BNM
 * 
 * Hydra_Software_Devel/32   11/8/10 5:08p haisongw
 * SW7125-717: sync with 3.0.1 beta1 Docsis release (support annex C and
 * docsis scan control)
 * 
 * Hydra_Software_Devel/31   10/1/10 5:14p haisongw
 * SW7125-645: add API to retrieve QAM status ( including DPM)
 * asynchronously
 * 
 * Hydra_Software_Devel/30   6/15/10 2:21p haisongw
 * SW7125-474: minor cleanup
 *
 * Hydra_Software_Devel/29   6/11/10 11:25a haisongw
 * SW7125-474: Add Rmagnum support for BNM Power management and tuner
 * control
 *
 * Hydra_Software_Devel/28   1/19/10 3:28p haisongw
 * SW7420-450:Functionality required to check spectrum inversion status
 * from 3255R ADS
 *
 * Hydra_Software_Devel/27   11/30/09 6:35p mward
 * SW7125-131: Allow 4 Rmagnum tuners.
 *
 * Hydra_Software_Devel/26   9/24/09 5:32p haisongw
 * SW7400-2531: Retrieve eCM status from 325x
 *
 * Hydra_Software_Devel/25   8/25/09 3:12p haisongw
 * SW7400-2477:Add APIs to retrieve 3255 channel bonding status
 *
 * Hydra_Software_Devel/24   3/4/09 5:32p haisongw
 * PR52808:add eCM reset event notification
 *
 * Hydra_Software_Devel/23   10/13/08 2:38p anilmm
 * PR47842: BER calculation software implementation
 *
 * Hydra_Software_Devel/22   5/27/08 4:27p haisongw
 * PR43047: Add two parameters in BADS_GetStatus() to sync up with 325X
 * docsis 2.0.0 release
 *
 * Hydra_Software_Devel/21   5/5/08 6:02p haisongw
 * PR28691: Add power saving operation for OOB upstream since Docsis 2.0.0
 * Beta1 release
 *
 * Hydra_Software_Devel/20   12/20/07 5:51p haisongw
 * PR36061: speed up 325x QAM acqusition time
 *
 * Hydra_Software_Devel/19   10/12/07 1:29p haisongw
 * PR36060: add OCAP DPM support for three 3420 tuners
 *
 * Hydra_Software_Devel/18   5/10/07 6:21p haisongw
 * PR30640: Add an option to enable/disable 325X ADS auto-reacquiring
 *
 * Hydra_Software_Devel/17   4/16/07 6:13p haisongw
 * PR25049: SetTop API function btuner_get_qam_status() returns incomplete
 * information
 *
 * Hydra_Software_Devel/16   3/12/07 5:48p haisongw
 * PR25908: Add BTNR_3255Ib_GetPowerSaver() support
 * PR28647: add DVS178 upstream support
 *
 * Hydra_Software_Devel/15   12/14/06 5:41p haisongw
 * PR26393: add 7401C0/C1 SPOD support on BCM97455MBV10 board
 *
 * Hydra_Software_Devel/14   11/14/06 4:09p haisongw
 * PR25908: eCM_DSG_SW_v4.x/12271: Please add RPC support for tuner
 * standby
 *
 * Hydra_Software_Devel/13   10/24/06 5:00p haisongw
 * PR24619: fix previous compiling error
 *
 * Hydra_Software_Devel/12   10/23/06 2:07p haisongw
 * PR24619: need a way to sync 740x and 3255 after boot up and vendor Gpio
 * control
 *
 * Hydra_Software_Devel/11   10/5/06 2:33p haisongw
 * PR 24762: add two more Annex mode for CableCARD OOB DS
 *
 * Hydra_Software_Devel/10   9/20/06 3:34p haisongw
 * PR24338: add 97455B0 Board support, add POD_ApplyPower_RPC
 *
 * Hydra_Software_Devel/9   9/7/06 3:22p haisongw
 * PR 22368: Add AOB, AUS and TNR3255ob support
 *
 * Hydra_Software_Devel/8   9/6/06 12:20p haisongw
 * PR 22368: Add AOB, AUS and TNR3255ob support
 *
 * Hydra_Software_Devel/7   9/1/06 5:34p haisongw
 * PR24071: add spectral_inversion control API for OOB DS
 *
 * Hydra_Software_Devel/6   8/25/06 4:50p haisongw
 * PR 22368: Add AOB, AUS and TNR3255ob support
 *
 * Hydra_Software_Devel/5   8/15/06 10:55a haisongw
 * PR22318: multiple OS support for BRPC module
 *
 * Hydra_Software_Devel/4   8/14/06 2:17p haisongw
 * PR22318: multiple OS support for BRPC module
 *
 *********************************************************************/
#ifndef __BRPC_3255_H
#define __BRPC_3255_H

#if __cplusplus
extern "C" {
#endif

#define MX_IQ_PER_GET	16		/* max number of QAM/QPSK soft decision results*/
#define MX_PREMABLE_SZ	(0x7f)	/* max size of premeable pattern in Upstream*/

typedef enum BRPC_DevId
{
	BRPC_DevId_3255			= 0,
	BRPC_DevId_3255_US0 	= 1,
	BRPC_DevId_3255_OB0		= 2,
	BRPC_DevId_3255_DS0		= 10,
	BRPC_DevId_3255_DS1		= 11,
	BRPC_DevId_3255_DS2		= 12,
	BRPC_DevId_3255_DS3		= 13,
	BRPC_DevId_3255_DS4		= 14,
	BRPC_DevId_3255_DS5		= 15,
	BRPC_DevId_3255_DS6		= 16,
	BRPC_DevId_3255_DS7		= 17,

	BRPC_DevId_3255_TNR0	= 20,
	BRPC_DevId_3255_TNR1	= 21,
	BRPC_DevId_3255_TNR2	= 22,
	BRPC_DevId_3255_TNR3	= 23,
	BRPC_DevId_3255_TNR4	= 24,
	BRPC_DevId_3255_TNR5	= 25,
	BRPC_DevId_3255_TNR6	= 26,
	BRPC_DevId_3255_TNR7	= 27,

	BRPC_DevId_3255_POD		= 30,
	#if (defined(VENDOR_GPIO_CONTROL) || defined(VENDOR_REQUEST))
	BRPC_DevId_3255_VEN     = 31,
	#endif

	BRPC_DevId_3255_TNR0_OOB= 40,

	BRPC_ProcId_LastDevId
} BRPC_DevId;


typedef enum BRPC_ProcId
{
	BRPC_ProcId_Reserved = 0,
	BRPC_ProcId_InitSession = 1,

	BRPC_ProcId_ADS_GetVersion = 2,
	BRPC_ProcId_ADS_GetTotalChannels = 3,
	BRPC_ProcId_ADS_OpenChannel = 4,
	BRPC_ProcId_ADS_CloseChannel = 5,
	BRPC_ProcId_ADS_GetChannelDefaultSettings = 6,
	BRPC_ProcId_ADS_GetStatus = 7,
	BRPC_ProcId_ADS_Acquire = 8,
	BRPC_ProcId_ADS_EnablePowerSaver = 9,
	BRPC_ProcId_ADS_GetSoftDecision = 10,
	BRPC_ProcId_ADS_GetLockStatus = 11,

	BRPC_ProcId_TNR_Open = 12,
	BRPC_ProcId_TNR_Close = 13,
	BRPC_ProcId_TNR_GetVersion = 14,
	BRPC_ProcId_TNR_SetRfFreq = 15,

	BRPC_ProcId_TNR_Oob_Open = 16,
	BRPC_ProcId_TNR_Oob_Close = 17,
	BRPC_ProcId_TNR_Oob_SetRfFreq = 18,

	BRPC_ProcId_AOB_Open = 19,
	BRPC_ProcId_AOB_Close = 20,
	BRPC_ProcId_AOB_Acquire = 21,
	BRPC_ProcId_AOB_GetStatus = 22,
	BRPC_ProcId_AOB_GetLockStatus = 23,
	BRPC_ProcId_AOB_EnablePowerSaver = 24,
	BRPC_ProcId_AOB_GetSoftDecision = 25,

	BRPC_ProcId_AUS_Open = 26,
	BRPC_ProcId_AUS_Close = 27,
	BRPC_ProcId_AUS_SetOperationMode = 28,
	BRPC_ProcId_AUS_SetSymbolRate = 29,
	BRPC_ProcId_AUS_SetRfFreq = 30,
	BRPC_ProcId_AUS_SetPowerLevel = 31,
	BRPC_ProcId_AUS_GetStatus = 32,
	BRPC_ProcId_AUS_SetPreamblePattern = 34,
	BRPC_ProcId_AUS_SetBurstProfile = 35,
	BRPC_ProcId_AUS_SetRawPowerLevel = 36,
	BRPC_ProcId_AUS_TxStarVuePkt = 37,
	BRPC_ProcId_AUS_SetTransmitMode = 38,

#ifdef VENDOR_GPIO_CONTROL
	BRPC_ProcId_VEN_GpioControl = 39,
#endif
#ifdef VENDOR_REQUEST
	BRPC_ProcId_VEN_Request = 40,
#endif

	BRPC_ProcId_ADS_GetDsChannelPower = 41,
	    
	BRPC_ProcId_POD_CardOut = 43,
	BRPC_ProcId_POD_CardIn = 44,
	BRPC_ProcId_POD_CardInStart = 45,
	BRPC_ProcId_POD_CardApplyPower = 46,

	BRPC_ProcId_AOB_SetSpectrum = 50,
	BRPC_ProcId_TNR_GetAgcVal= 51,
	BRPC_ProcId_TNR_EnablePowerSaver= 52,
	BRPC_ProcId_TNR_GetPowerSaver= 53,
	BRPC_ProcId_AUS_GetTransmitMode = 54,
	BRPC_ProcId_ADS_GetBondingCapability = 55,
	BRPC_ProcId_ADS_ReserveChannel = 56,
	BRPC_ProcId_ECM_GetStatus = 57,
	BRPC_ProcId_ECM_PowerSaver = 58,
	BRPC_ProcId_ECM_Transit_Frontend_Control_to_Host = 59,
	BRPC_ProcId_ECM_Transit_Frontend_Control_to_Bnm = 60,

	BRPC_ProcId_ECM_TSMF_GetFldVerifyConfig = 61,
	BRPC_ProcId_ECM_TSMF_SetFldVerifyConfig = 62,
	BRPC_ProcId_ECM_TSMF_EnableAutoMode = 63,
	BRPC_ProcId_ECM_TSMF_EnableSemiAutoMode = 64,
	BRPC_ProcId_ECM_TSMF_DisableTsmf = 65,
	BRPC_ProcId_ECM_TSMF_SetParserConfig = 66,

	BRPC_ProcId_ADS_SetParserBandId = 67,
	BRPC_ProcId_ECM_ReadXptBlock = 68,
	BRPC_ProcId_ECM_WriteXptBlock = 69,
	BRPC_ProcId_ECM_HostChannelsLockStatus = 70,
	BRPC_ProcId_ECM_DoLnaReConfig = 71,
	BRPC_ProcId_ECM_ReadDieTemperature = 72,
	BRPC_ProcId_LastProcId
} BRPC_ProcId;


typedef struct BRPC_Param_InitSession
{
	uint32_t devId;						/* device id, see BRPC_DevId */
} BRPC_Param_InitSession;


typedef struct BRPC_Param_XXX_Get
{
	uint32_t devId;						/* device id, see BRPC_DevId */
} BRPC_Param_XXX_Get;

/* POD RPC parameters  */
typedef struct BRPC_Param_POD_CardOut
{
	uint32_t devId;						/* device id, see BRPC_DevId */
} BRPC_Param_POD_CardOut;

typedef struct BRPC_Param_POD_CardIn
{
	uint32_t devId;						/* device id, see BRPC_DevId */
} BRPC_Param_POD_CardIn;

typedef struct BRPC_Param_POD_CardInStart
{
	uint32_t devId;					/* device id, see BRPC_DevId */
} BRPC_Param_POD_CardInStart;

#define MCARD_3VOLT             0       /* use only for bcm97456-A0 MBV0002 ref desgin */
#define SCARD_3VOLT             1       /* Use on later MBV100x ref design, all platforms */
#define SCARD_5VOLT             2
#define MCARD_5VOLT             3
#define ENABLE_POD_OUTPINS      4       /* Works only on bcm3255-A3 or later */
#define DISABLE_POD_OUTPINS     5       /* Do nothing for earlier rev chip */
typedef struct BRPC_Param_POD_CardApplyPower
{
	uint32_t devId;					/* device id, see BRPC_DevId */
	uint32_t powerMode;				/* Mcard_3V, Scard_3V and Scard_5V */
} BRPC_Param_POD_CardApplyPower;

typedef struct BRPC_Param_TNR_GetVersion
{
	uint32_t manafactureId;
	uint32_t modelId;
	uint32_t majVer;
	uint32_t minVer;
} BRPC_Param_TNR_GetVersion;

typedef struct BRPC_Param_TNR_Open
{
	uint32_t devId;						/* device id, see BRPC_DevId */
	uint32_t ifFreq;					/* IF Frequency in Hertz */
} BRPC_Param_TNR_Open;


typedef struct BRPC_Param_TNR_Close
{
	uint32_t devId;						/* device id, see BRPC_DevId */
} BRPC_Param_TNR_Close;

typedef struct BRPC_Param_TNR_GetAgcVal
{
    	/* Byte3-Byte2 = 16 bit chipid. Ex: 0x3412
       	Byte1 = T1T2|0SBT.  Out1Tilt(b7b6)/Out2Tilt(b5b4)|0(b3)/SuperBoost(b2)/Boost(b1)/Tilt(b0).  {0=OFF, 1=ON}
       	Byte0 = b000(b7-b5)/AGC gain value 0x0-0x1F(b4-b0)
     	+-------+-------+---------------+-------+
     	|  LnaChipId    | T1/T2|0/S/B/T |  AGC  |
     	+-------+-------+---------------+-------+
            B3      B2          B1         B0
    	*/
	uint32_t AgcVal;					/* AGC value*/
} BRPC_Param_TNR_GetAgcVal;

typedef struct BRPC_Param_TNR_EnablePowerSaver
{
	uint32_t devId;						/* device id, see BRPC_DevId */
} BRPC_Param_TNR_EnablePowerSaver;

typedef struct BRPC_Param_TNR_GetPowerSaver
{
	uint32_t powersaver;      /* 1 : standby mode; 0 : non-standby mode*/
} BRPC_Param_TNR_GetPowerSaver;

/* 
  Added new BTNR_TunerMode_eDocsis. Usage:
  (1) To stop Docsis scanning or terminate Docsis & use DS0 for video channel, call this API to BRPC_DevId_3255_DS0 with BTNR_TunerMode_eDigital & a valid video freq.
  **note: shut down Docsis takes about 1-2sec**
  (2) To terminate DS0 video or start Docsis scanning, call this API to BRPC_DevId_3255_DS0 with BTNR_TunerMode_eDocsis & rfFreq=0.
  (3) To terminate DS0 video or ask Docsis to lock to a specific freq, call this API to BRPC_DevId_3255_DS0 with BTNR_TunerMode_eDocsis & rfFreq=desired_freq.
  **note: Starting Docsis from the Host does not need ADS_Acquire, thus no lock notification will be received by the Host
*/
typedef struct BRPC_Param_TNR_SetRfFreq
{
	uint32_t devId;						/* device id, see BRPC_DevId */
	uint32_t rfFreq;					/* RF Frequency in Hertz */
	uint32_t tunerMode;					/* tunerMode:	0: BTNR_TunerMode_eDigital
														1: BTNR_TunerMode_eAnalog
														2: BTNR_TunerMode_eDocsis(new)	*/
} BRPC_Param_TNR_SetRfFreq;


typedef struct BRPC_Param_TNR_OOB_Open
{
	uint32_t devId;						/* device id, see BRPC_DevId */
	uint32_t ifFreq; 					/* IF Frequency in Hertz: Use 1250000 to indicate using DS0 as OOB receiver.(shared with Docsis tuner) */ 
										/* Other value will be ignored & use external legacy tuner as OOB receiver. IfFreq is fixed during compile time */
} BRPC_Param_TNR_OOB_Open;


typedef struct BRPC_Param_TNR_OOB_Close
{
	uint32_t devId;						/* device id, see BRPC_DevId */
} BRPC_Param_TNR_OOB_Close;


typedef struct BRPC_Param_TNR_OOB_SetRfFreq
{
	uint32_t devId;						/* device id, see BRPC_DevId */
	uint32_t rfFreq;					/* RF Frequency in Hertz */
} BRPC_Param_TNR_OOB_SetRfFreq;


typedef struct BRPC_Param_AOB_Open
{
	uint32_t devId;						/* device id, see BRPC_DevId */
	uint32_t enableFEC;			/* Use OOB FEC or not */
} BRPC_Param_AOB_Open;


typedef struct BRPC_Param_AOB_Close
{
	uint32_t devId;						/* device id, see BRPC_DevId */
} BRPC_Param_AOB_Close;

typedef enum BRPC_AOB_ModulationType {
	BRPC_AOB_ModulationType_eAnnexAQpsk,
	BRPC_AOB_ModulationType_eDvs178Qpsk,
	BRPC_AOB_ModulationType_ePod_AnnexAQpsk,
	BRPC_AOB_ModulationType_ePod_Dvs178Qpsk
} BRPC_AOB_ModulationType;

typedef struct BRPC_Param_AOB_Acquire
{
	uint32_t devId;						/* device id, see BRPC_DevId */
	uint32_t modType;					/* Modulation type */
	uint32_t symbolRate;				/* in Baud */
} BRPC_Param_AOB_Acquire;


typedef struct BRPC_Param_AOB_GetLockStatus
{
	uint32_t isFecLock;					/* lock=1, unlock=0 */
	uint32_t isQamLock;					/* lock=1, unlock=0 */
} BRPC_Param_AOB_GetLockStatus;

typedef struct BRPC_Param_AOB_GetStatus
{
	uint32_t modType;					/* Modulation Type */
	uint32_t ifFreq;					/* in Hertz, IF freq. */
	uint32_t loFreq;					/* in Hertz, LO freq. */
	uint32_t sysXtalFreq;				/* in Hertz, Sys. Xtal freq. */
	uint32_t symbolRate;				/* in Baud */
	uint32_t isFecLock;					/* lock=1, unlock=0 */
	uint32_t isQamLock;					/* lock=1, unlock=0 */
	uint32_t snrEstimate;				/* in 1/256 db */
	uint32_t agcIntLevel;				/* in 1/10 percent */
	uint32_t agcExtLevel;				/* in 1/10 percent */
	uint32_t carrierFreqOffset;			/* in 1/1000 Hz */
	uint32_t carrierPhaseOffset;		/* in 1/1000 Hz */
	uint32_t uncorrectedCount;			/* not self-clearing  */
	uint32_t correctedCount;			/* not self-clearing */
	uint32_t berErrorCount;				/* not self-clearing */
	uint32_t fdcChannelPower;           /* Only valid if using DS0 for OOB! in 1/10 dBmV unit; OCAP DPM support for OOB channel */
} BRPC_Param_AOB_GetStatus;

typedef enum BRPC_BAOB_SpectrumMode
{
	BRPC_BAOB_SpectrumMode_eAuto,
	BRPC_BAOB_SpectrumMode_eNoInverted,
	BRPC_BAOB_SpectrumMode_eInverted
} BRPC_BAOB_SpectrumMode;

typedef struct BRPC_Param_AOB_SetSpectrum
{
	uint32_t devId;						/* device id, see BRPC_DevId */
	uint32_t spectrum;					/* OOB DS specturm setting */
} BRPC_Param_AOB_SetSpectrum;

typedef struct BRPC_Param_AOB_EnablePowerSaver
{
	uint32_t devId;						/* device id, see BRPC_DevId */
} BRPC_Param_AOB_EnablePowerSaver;

typedef struct BRPC_Param_AOB_GetSoftDecisions
{
	uint32_t iVal[MX_IQ_PER_GET];					/* Array of iVal */
	uint32_t qVal[MX_IQ_PER_GET];					/* Array of iVal */
	uint32_t	nbrGotten;							/* number bytes read*/
} BRPC_Param_AOB_GetSoftDecisions;


typedef struct BRPC_Param_AUS_Open
{
	uint32_t devId;					/* device id, see BRPC_DevId */
	uint32_t xtalFreq;				/* Crystal Freqency in Hertz */
} BRPC_Param_AUS_Open;


typedef struct BRPC_Param_AUS_Close
{
	uint32_t devId;						/* device id, see BRPC_DevId */
} BRPC_Param_AUS_Close;

typedef struct BRPC_Param_AUS_SetSetTransmitMode
{
	uint32_t devId;					/* device id, see BRPC_DevId */
	uint32_t transmitMode;			/* 0: disable Transimitter
									   1: enable Transimitter */
} BRPC_Param_AUS_SetSetTransmitMode;

typedef enum BRPC_AUS_Operation_mode {
	BRPC_AUS_OperationMode_eAnnexA = 0,
	BRPC_AUS_OperationMode_eDvs178,
	BRPC_AUS_OperationMode_eDocsis,
	BRPC_AUS_OperationMode_ePod,
	BRPC_AUS_OperationMode_eTestCw,
	BRPC_AUS_OperationMode_eTestPn23,
	BRPC_AUS_OperationMode_ePod_AnnexA,
	BRPC_AUS_OperationMode_ePod_Dvs178
} BRPC_AUS_Operation_mode;

typedef struct BRPC_Param_AUS_SetOperationMode
{
	uint32_t devId;					/* device id, see BRPC_DevId */
	uint32_t operationMode;			/* operation Mode */
} BRPC_Param_AUS_SetOperationMode;

typedef struct BRPC_Param_AUS_SetSymbolRate
{
	uint32_t devId;					/* device id, see BRPC_DevId */
	uint32_t symbolRate;			/* symbol rate in Hz */
} BRPC_Param_AUS_SetSymbolRate;

typedef struct BRPC_Param_AUS_SetRfFreq
{
	uint32_t devId;					/* device id, see BRPC_DevId */
	uint32_t rfFreq;				/* RF Frequency in Hertz */
} BRPC_Param_AUS_SetRfFreq;

typedef struct BRPC_Param_AUS_SetPreamblePattern
{
	uint32_t devId;						/* device id, see BRPC_DevId */
	uint32_t patternSize;				/* preamble Pattern Size */
	uint32_t Pattern[MX_PREMABLE_SZ];	/* preamble Pattern */
} BRPC_Param_AUS_SetPreamblePattern;

typedef struct BRPC_Param_AUS_SetBurstProfile
{
	uint32_t devId;					/* device id, see BRPC_DevId */
	uint32_t burstBank;				/* Burst band number */
	uint32_t modulationType;		/* 1=QPSK, 2=16Qam */
	uint32_t diffEncodingOnOff;		/* 1=On, 2=Off */
	uint32_t preambleLength; 		/* 9:0 bits */
	uint32_t preambleValOffset;		/* 8:0 bits */
	uint32_t fecBytes;				/* 0-10 */
	uint32_t fecCodewordInfoBytes;	/* 0, 16-253 */
	uint32_t scramblerSeed;			/* 14:0 bits */
	uint32_t maxBurstSize;			/* TODO: */
	uint32_t guardTimeSize;			/* TODO: */
	uint32_t lastCodewordLength;	/* 1=fixed, 2=shortened */
	uint32_t scramblerOnOff;		/* 1=on, 2=off */
	uint32_t nbrPreambleDiffEncoding; /* Number of Preamble different encoding */
} BRPC_Param_AUS_SetBurstProfile;

typedef struct BRPC_Param_AUS_SetPowerLevel
{
	uint32_t devId;					/* device id, see BRPC_DevId */
	uint32_t powerLevel;			/* in hundredth of dBmV*/
} BRPC_Param_AUS_SetPowerLevel;


typedef struct BRPC_Param_AUS_SetRawPowerLevel
{
	uint32_t devId;
	uint32_t powerAmpGain;
	uint32_t powerAmpDac;
} BRPC_Param_AUS_SetRawPowerLevel;

typedef struct BRPC_AUS_Status {
	uint32_t powerSaveEnabled;
	BRPC_AUS_Operation_mode operationMode;
	uint32_t xtalFreq;
	uint32_t powerLevel;
	uint32_t rfFreq;
	uint32_t symbolRate;
} BRPC_AUS_Status;

typedef struct BRPC_Param_AUS_TxStarVuePkt
{
	uint32_t devId;					/* device id, see BRPC_DevId */
	uint32_t svBuffer[14];			/* (StarVue format: 1 byte sequence + 53 byte ATM cell) */
									/* only the first 54 bytes are valid data */
} BRPC_Param_AUS_TxStarVuePkt;

typedef struct BRPC_Param_ADS_GetVersion
{
	uint32_t majVer;
	uint32_t minVer;
} BRPC_Param_ADS_GetVersion;

typedef struct BRPC_Param_ADS_ReserveChannel
{
	uint32_t num2Reserve;				/* Number of bonded channels to reserve*/
} BRPC_Param_ADS_ReserveChannel;
typedef struct BRPC_Param_ADS_GetBondingCapability
{
	uint32_t maxNum;				/* Maximum number of bonded channels */
} BRPC_Param_ADS_GetBondingCapability;
typedef struct BRPC_Param_ADS_GetTotalChannels
{
	uint32_t totalChannels;				/* Total number of available channels */
} BRPC_Param_ADS_GetTotalChannels;

typedef struct BRPC_Param_ADS_OpenChannel
{
	uint32_t devId;						/* device id, see BRPC_DevId */
	uint32_t ifFreq; 					/* IF Frequency in Hertz */
} BRPC_Param_ADS_OpenChannel;

typedef struct BRPC_Param_ADS_CloseChannel
{
	uint32_t devId;						/* device id, see BRPC_DevId */
} BRPC_Param_ADS_CloseChannel;

typedef struct BRPC_Param_ADS_GetLockStatus
{
	uint32_t isFecLock;					/* lock=1, unlock=0 */
	uint32_t isQamLock;					/* lock=1, unlock=0 */
} BRPC_Param_ADS_GetLockStatus;

typedef struct BRPC_Param_ADS_GetStatus
{
	uint32_t isPowerSaverEnabled;		/* Eanble=1, Disable=0 */
	uint32_t modType;					/* Modulation type */
	uint32_t ifFreq;					/* in Hertz, IF freq. */
	uint32_t symbolRate;				/* in Baud */
	uint32_t isFecLock;					/* lock=1, unlock=0 */
	uint32_t isQamLock;					/* lock=1, unlock=0 */
	uint32_t correctedCount;			/* reset on every read */
	uint32_t uncorrectedCount;			/* reset on every read */
	uint32_t snrEstimate;				/* in 1/256 dB */
	uint32_t agcIntLevel;				/* in 1/10 percent */
	uint32_t agcExtLevel;				/* in 1/10 percent */
	uint32_t carrierFreqOffset;			/* in 1/1000 Hz */
	uint32_t carrierPhaseOffset;		/* in 1/1000 Hz */
	uint32_t rxSymbolRate;				/* in Baud, received symbol rate */
	uint32_t interleaveDepth;			/* return index of 0-15 instead of i&j, see ITU Recommendation J.83	*/
										/* Table B.2/J.83 - Level 2 interleaving  */
	uint32_t goodRsBlockCount;			/* reset on every read */
	uint32_t berRawCount;   			/* reset on every read !!This is only useful for self-testing PN sequence data!! */
	uint32_t dsChannelPower;			/* Deprecated starting V3.0.1alpha4 release. Will always return 0. Please use new Async API to get channel Power */
	uint32_t mainTap;					/* Channel main tap value */
	uint32_t equalizerGain;				/* Channel equalizer gain value in dBm */
	/* OCAP required postRsBER for all DS channels. postRsBER and elapsedTimeSec will be reset on every channel change*/
	/* fpostRsBER normally between xxxe-6 to xxxe-12 float value, to send this small float number over rMagnum to host, we convert it to uint32 using the formula*/
	uint32_t postRsBER;					/* Converted floating point fpostRsBER --> uint32_t postRsBER for rMagnum transport: */
										/* int ipostRsBER = (int)( log(fpostRsBER) * 1000000.0 ); uint32_t postRsBER = (uint32_t)ipostRsBER; */
										/* Host side will need this to convert it back: int ipostRsBER = (int)postRsBER; float fpostRsBER = exp( (float)ipostRsBER/1000000.0 ); */
	uint32_t elapsedTimeSec;			/* postRsBER over this time */
	uint32_t spectralInversion;			/* spectral inversion, 1 inverted, 0 normal */
} BRPC_Param_ADS_GetStatus;


typedef enum BRPC_ADS_ModulationType{
	/* Most of these are currently not supported, are here for future use */
	BRPC_ADS_ModulationType_eAnnexAQam16,
	BRPC_ADS_ModulationType_eAnnexAQam32,
	BRPC_ADS_ModulationType_eAnnexAQam64,
	BRPC_ADS_ModulationType_eAnnexAQam128,
	BRPC_ADS_ModulationType_eAnnexAQam256,
	BRPC_ADS_ModulationType_eAnnexAQam512,
	BRPC_ADS_ModulationType_eAnnexAQam1024,
	BRPC_ADS_ModulationType_eAnnexAQam2048,
	BRPC_ADS_ModulationType_eAnnexAQam4096,
	BRPC_ADS_ModulationType_eAnnexBQam16,
	BRPC_ADS_ModulationType_eAnnexBQam32,
	BRPC_ADS_ModulationType_eAnnexBQam64,
	BRPC_ADS_ModulationType_eAnnexBQam128,
	BRPC_ADS_ModulationType_eAnnexBQam256,
	BRPC_ADS_ModulationType_eAnnexBQam512,
	BRPC_ADS_ModulationType_eAnnexBQam1024,
	BRPC_ADS_ModulationType_eAnnexBQam2048,
	BRPC_ADS_ModulationType_eAnnexBQam4096,
	BRPC_ADS_ModulationType_eAnnexCQam16,
	BRPC_ADS_ModulationType_eAnnexCQam32,
	BRPC_ADS_ModulationType_eAnnexCQam64,
	BRPC_ADS_ModulationType_eAnnexCQam128,
	BRPC_ADS_ModulationType_eAnnexCQam256,
	BRPC_ADS_ModulationType_eAnnexCQam512,
	BRPC_ADS_ModulationType_eAnnexCQam1024,
	BRPC_ADS_ModulationType_eAnnexCQam2048,
	BRPC_ADS_ModulationType_eAnnexCQam4096
} BRPC_ADS_ModulationType;

typedef struct BRPC_Param_ADS_Acquire
{
	uint32_t devId;			/* device id, see BRPC_DevId */
	uint32_t modType;		/* Modulation type */
	uint32_t symbolRate;	/* in Baud */
	uint32_t autoAcquire;	/* 0 : Acquire once and report lock/no-lock */ 
							/* 1 (not recomended) : Acquire once and report lock/no-lock plus.. */
							/* if no-lock or lost-lock, contineous re-acquire until lock */
							/* ***Auto re-acquire on a non-existing channel is a very resource intentive process */
	/* 71xx: For internal channel only */
	uint32_t fastAcquire;	/* 1: ~80ms faster. Use faster sweep rate & skip DCO loop (may give up some performance?)*/
							/* other value: Normal acquire time. */
} BRPC_Param_ADS_Acquire;

typedef struct BRPC_Param_ADS_EnablePowerSaver
{
	uint32_t devId;						/* device id, see BRPC_DevId */
} BRPC_Param_ADS_EnablePowerSaver;

typedef struct BRPC_Param_ADS_GetSoftDecisions
{
	uint32_t iVal[MX_IQ_PER_GET];					/* Array of iVal */
	uint32_t qVal[MX_IQ_PER_GET];					/* Array of iVal */
	uint32_t	nbrGotten;							/* number bytes read*/
} BRPC_Param_ADS_GetSoftDecisions;

#ifdef VENDOR_GPIO_CONTROL
typedef struct BRPC_Param_VEN_GpioControl
{
	uint32_t devId;					/* device id, see BRPC_DevId */
	uint32_t Operation; 			/* GPIO_OPERATION enum */
	uint32_t RegAddress;			/* 1 of the 4 GPIO registers */
	uint32_t WriteValue;			/* value to be written for Write,OR,AND operation */
									/* not used for Read operation */
} BRPC_Param_VEN_GpioControl;

typedef struct BRPC_Param_VEN_GpioReadReply
{
	uint32_t ReadBackValue;			/* return Read back register Value for all operations */
} BRPC_Param_VEN_GpioReadReply;
#endif

#ifdef VENDOR_REQUEST
#define MAX_VEN_PARAM   32
typedef struct BRPC_Param_VEN_Request
{
	uint32_t devId;					/* device id, see BRPC_DevId */
	uint32_t Operation;				/* OPERATION enum */
	uint32_t NumOfParams;			/* Number of parameters to follows */
	uint32_t Params[MAX_VEN_PARAM];	/* Allow up to 8 paramters */
} BRPC_Param_VEN_Request;

typedef struct BRPC_Param_VEN_Reply
{
	uint32_t NumberOfReply;			/* Number of Reply parameters to follows */
	uint32_t Reply[MAX_VEN_PARAM];	/* allow up to 8 reply parameters */
} BRPC_Param_VEN_Reply;
#endif

typedef struct BRPC_Param_ECM_GetStatus
{
	uint32_t downstreamCenterFreq;                /* Docsis channel DS frequency */
	uint32_t downstreamPowerLevel;                /* Docsis channel DS power */
	                                              /* Deprecated starting V3.0.1alpah4 release. Will always return 0. Please use new Async API to get channel Power */
	uint32_t downstreamCarrierLock;               /* Docsis channel DS lock status */
	uint32_t channelScdmaStatus;                  /* Docsis channel DS scdma status */
	uint32_t upstreamModuType;                    /* US modulation type */
	uint32_t upstreamXmtCenterFreq;               /* US tramsmition frequency */
	uint32_t upstreamPowerLevel;                  /* US power */
	uint32_t upStreamSymbolrate;                  /* US Symbol rate */
	uint32_t ecmMacAddressHi;                     /* eCM Mac address */
	uint32_t ecmMacAddressLo;                     /* eCM mac address */
	uint32_t isEcmIpMode;                         /* eCM IPV6: 0 no IP address yet, 1 IPV4 2 IPV6 */
	uint32_t ecmIpAddress;                        /* eCM IPV4 address */
	uint32_t ecmIpv6Address0;                     /* eCM IPv6 address */
	uint32_t ecmIpv6Address1;                     /* eCM IPv6 address */
	uint32_t ecmIpv6Address2;                     /* eCM IPv6 address */
	uint32_t ecmIpv6Address3;                     /* eCM IPv6 address */
	uint32_t lastKnownGoodFreq;		      /* Last Frequency that Docsis successfully registered on */
}BRPC_Param_ECM_GetStatus;

/*
Host can use this API to put 3383 into low power mode. Currently supported modes:
BRPC_ECM_PowerMode_On: When 3383 is in low powermode, this will put 3383 back to full ON.

BRPC_ECM_PowerMode_Standby1: This mode will turn off the FullBand Capture(WB) block & transistion 
    to 1x1 narrowband data only mode. This will save approx 4 Watts of power.  
    When 3383 receives this mode from the host, it will send up a CM_STATUS msg to CMTS & expect the CMTS 
    to move this modem to a single channel CMTS.  Once the modem has moved to this new 1x1 CMTS, it will 
    switch from WB to NB & turn off WB. This feature requires a bonding CMTS that supports CM_STATUS 
    sent up by the modem.  This switchover will also stop any active video channels that are using 3383 WB.
    
    For testing purposes & if you do not have a bonding CMTS that supports CM_STATUS, there is a way to 
    test the switchover with the following special configuration to the modem.
    (1) set up non_vol to disable bonding: Use commands "CM/NonVol/CM DOCSIS 3.0 NonVol> enable us_bonding 0" & 
    "CM/NonVol/CM DOCSIS 3.0 NonVol> enable ds_bonding 0" & "CM/NonVol/CM DOCSIS 3.0 NonVol> write" & reboot
    (2) Boot up & range & register with a 1x1 CMTS.
    (3) host sends this Standby1 mode.
    (4) 3383 will switch from WB to NB with no loss of data & save approx 4 Watts.
*/
typedef enum BRPC_ECM_PowerMode{
	/* Most of these modes are not supported, except noted */
	BRPC_ECM_PowerMode_Unknown,
	BRPC_ECM_PowerMode_On,		/* Supported. From Standby1 to full on  */
	BRPC_ECM_PowerMode_Off,
	BRPC_ECM_PowerMode_Standby1,	/* Supported. From full on to Standby1 */
	BRPC_ECM_PowerMode_Standby2,
	BRPC_ECM_PowerMode_Standby3,
	BRPC_ECM_PowerMode_Standby4,
	BRPC_ECM_PowerMode_Last
} BRPC_ECM_PowerMode;

typedef struct BRPC_Param_ECM_PowerSaver
{
	uint32_t devId;				/* Set to BRPC_DevId_3255(0), see BRPC_DevId. Currently assume all external channels + LNA */
	uint32_t mode;				/* eCM power mode: BRPC_ECM_PowerMode  */
} BRPC_Param_ECM_PowerSaver;
typedef struct BRPC_Param_ECM_Transit_Frontend_Control_to_Host
{
	uint32_t devId;						/* Set to BRPC_DevId_3255(0), see BRPC_DevId. Currently assume all external channels + LNA */
} BRPC_Param_ECM_Transit_Frontend_Control_to_Host;
typedef struct BRPC_Param_ECM_Transit_Frontend_Control_to_Bnm
{
	uint32_t devId;						/* Set to BRPC_DevId_3255(0), see BRPC_DevId. Currently assume all external channels + LNA */
} BRPC_Param_ECM_Transit_Frontend_Control_to_Bnm;

typedef struct BRPC_Param_TSMF_Get
{
	uint32_t TsmfNum;
} BRPC_Param_TSMF_Get;

typedef struct BRPC_Param_ECM_TSMF_FldVerifyConfig
{
	uint32_t TsmfNum;

	uint32_t CrcChkDis;
	uint32_t RelTsStatusChkDis;
	uint32_t FrameTypeChkDis;
	uint32_t RelTsModeChkDis;
	uint32_t SyncChkFDis;
	uint32_t CCCkDis;
	uint32_t AdapChkDis;
	uint32_t ScChkDis;
	uint32_t TsPriorChkDis;
	uint32_t PusiChkDis;
	uint32_t TeiChkDis;
	uint32_t VersionChgMode;
} BRPC_Param_ECM_TSMF_FldVerifyConfig;

typedef struct BRPC_Param_ECM_TSMF_EnableAutoMode
{
	uint32_t eInputSel;
	uint32_t TSMFNum;
	uint32_t RelativeTsNo;
} BRPC_Param_ECM_TSMF_EnableAutoMode;

typedef struct BRPC_Param_ECM_TSMF_EnableSemiAutoMode
{
	uint32_t eInputSel;
	uint32_t TSMFNum;
	uint32_t SlotMapLo;
	uint32_t SlotMapHi;
	uint32_t RelativeTsNo;
} BRPC_Param_ECM_TSMF_EnableSemiAutoMode;

typedef struct BRPC_Param_ECM_TSMF_DisableTsmf
{
	uint32_t TSMFNum;
} BRPC_Param_ECM_TSMF_DisableTsmf;

typedef struct BRPC_Param_ECM_TSMF_SetParserConfig
{
	uint32_t uiParserBandNum;
	uint32_t TSMFNum;
	uint32_t TsmftoParserEn;
} BRPC_Param_ECM_TSMF_SetParserConfig;

typedef struct BRPC_Param_ADS_SetParserBandId
{
    uint32_t  devId;            /* device id DS0 to DSx, see BRPC_DevId */
    uint32_t  enable;           /* 0 or 1. Enabled bit for each mini parser */
    uint32_t  parserBand;       /* 4bit value. newly mapped parserband ID for devId */
} BRPC_Param_ADS_SetParserBandId;

typedef struct BRPC_Param_ECM_ReadXptBlock
{
    uint32_t startRegisterAddress; /* Offset from the begining of XPT block, 32bit aligned  */
    uint32_t endRegisterAddress;   /* Offset from the begining of XPT block, 32bit aligned */
    uint32_t returnRegisterValues[16]; /* array of uint32 to store read values. Max 16 32-bit word deep */ 
} BRPC_Param_ECM_ReadXptBlock;

typedef struct BRPC_Param_ECM_WriteXptBlock
{
    uint32_t startRegisterAddress; /* Offset from the begining of XPT block, 32bit aligned  */
    uint32_t endRegisterAddress;   /* Offset from the begining of XPT block, 32bit aligned */
    uint32_t writeRegisterValues[16]; /* array of uint32 to be written. Max 16 32-bit word deep */ 
} BRPC_Param_ECM_WriteXptBlock;

/* Host notfify eCM of Host control video channels lock status  */
typedef struct BRPC_Param_ECM_HostChannelsLockStatus
{
	uint32_t NonCmControlledVideoChLockStatus;  /* b31-b0 indicate host control video channels lock status. */
                						   		/* bit set=lock, clear=unlock. b0=ch0, b1=ch1, etc.... upto 32 channels can be reported*/
} BRPC_Param_ECM_HostChannelsLockStatus;

/* When CM receive this API, it will do LNA_reconfig(Doing CPPM) regardless, & it will glitch all locked channels */
/* LNA ReConfig can takes up to 1.5 seconds, so this is an async call, meaning when this API return, DoLnaReConfig is added to the Q and is not completed */
typedef struct BRPC_Param_ECM_DoLnaReConfig
{
    uint32_t devId;                                         /* Not used, just set to BRPC_DevId_3255(0), see BRPC_DevId. */
} BRPC_Param_ECM_DoLnaReConfig;

/* Read the FE chip temperature */
typedef struct BRPC_Param_ECM_ReadDieTemperature
{
    uint32_t TempInDot00DegC;        /* in unit 1/100 degC . Return chip temperature in C degree */
} BRPC_Param_ECM_ReadDieTemperature;

/* 					Async rMangum API calls:                   */
/*  Due to the nature of these API calls take longer time,     */
/*  the result will return in BRPC notification call below     */
/* 					                                           */
typedef struct BRPC_Param_ADS_GetDsChannelPower  /* OCAP DPM support for video channels */
{
	uint32_t devId;						/* device id, see BRPC_DevId */
} BRPC_Param_ADS_GetDsChannelPower;

/* 					BRPC notification data format:                   */
/*  Event code        total          32 bits                         */
/* |bit31 bit30.................. bit16|bit15 bit14...... bit0|      */
/* |BRPC_ADS_Notification_Event        |Event specific data   |      */
/* |BRPC_AOB_Notification_Event        |Event specific data   |      */
/* |BRPC_AUS_Notification_Event        |Event specific data   |      */
/* |BRPC_DS_Channel_Power_Event        | DPM in 1/10 dBbmV    |      */
typedef enum BRPC_Notification_Event
{
	BRPC_Notification_Event_No = 0,
	BRPC_Notification_Event_LockStatusChanged,
	BRPC_Notification_Event_EcmReset,
	BRPC_Notification_Event_EcmOperational,
	BRPC_Notification_Event_EcmRmagnumReady,	/* onetime notification to host from BNM when rMagnum is up during boot up */
	BRPC_Notification_Event_DsChannelPower,
	BRPC_Notification_Event_Last

} BRPC_Notification_Event;

typedef enum {
	BRPC_Qam_Lock = 0x8000,
	BRPC_Fec_Lock = 0x4000
} BRPC_Param_ADS_LockStatus_Notification;


typedef enum {
	BRPC_OOB_Qam_Lock = 0x8000,
	BRPC_OOB_Fec_Lock = 0x4000
} BRPC_Param_AOB_LockStatus_Notification;

#define BRPC_GET_NOTIFICATION_EVENT(x)	(x>>16)
#define BRPC_GET_ADS_QAM_LOCKSTATUS(x)	(x&BRPC_Qam_Lock)
#define BRPC_GET_ADS_FEC_LOCKSTATUS(x)	(x&BRPC_Fec_Lock)
#define BRPC_GET_AOB_QAM_LOCKSTATUS(x)	(x&BRPC_OOB_Qam_Lock)
#define BRPC_GET_AOB_FEC_LOCKSTATUS(x)	(x&BRPC_OOB_Fec_Lock)
#define BRPC_GET_DS_POWER(x)			((int16_t)(x&0xffff))

#if __cplusplus
}
#endif

#endif

