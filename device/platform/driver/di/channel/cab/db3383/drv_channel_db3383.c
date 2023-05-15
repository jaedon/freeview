/*************************************************************************************
 *
 *	DB3383.c
 *
 *	Copyright (c) 2011 by Humax	Corp.
 *
 *
 *************************************************************************************/
#include "di_channel.h"
#include "di_channel_priv.h"
#include "di_channel_c.h"
#include "di_channel_attr.h"

#include "drv_err.h"
//#include "drv_i2c.h"
#include "drv_gpio.h"

#include "drv_channel_c.h"
#include "drv_channel_db3383.h"
#include "nexus_frontend.h"
#include "nexus_platform.h"

#if defined (CONFIG_DEMOD_TSMF)
#include "nexus_tsmf.h"
#endif

#include <string.h>

#if (NEXUS_VERSION >= 1290)	/* modify for getting inband power level */
#include "bkni.h"
#endif

/****************************************************
	Define
******************************************************/
#define	WAIT_QAM_LOCKING		(100) //(50)//(100)
#define	LOCK_TRIAL_COUNT		(10) // (4)

#if defined (CONFIG_DEMOD_TSMF)
//#define 	DEBUG_FEC_LOCK
#if defined(DEBUG_FEC_LOCK)
#define 	TSMF_RETRY_CNT			(20)
#else
#define 	TSMF_RETRY_CNT			(6)
#endif

#define		NOT_MATCHED_TSINFO_RETRY_CNT	(10)
#endif

/****************************************************
	Extern  Variable
******************************************************/
extern HINT32 show_di_ch;

/****************************************************
	Groval Variable
******************************************************/
typedef struct
{
	HINT32			nStrength;			/**< Signal strength percentage Unit */
	HULONG			ulQuality;			/**< Signal quality percentage Unit */
	HUINT8			ucAgc;				/**< AGC */
	HULONG			ulBer;				/**< BER */
	HULONG			ulSnr;				/**< SNR */
	HUINT8			ucRfAgc;			/**< RFAGC */
	HUINT8			ucIfAgc;			/**< IFAGC */
	HUINT32			ulCorrectedNo;		/**< ulCorrectedNo */
	HUINT32			ulUnCorrectedNo;	/**< ulUnCorrectedNo */
	HUINT32			ulUnErroredNo;		/**< ulUnErroredNo */
	HFLOAT32		fSignalPower;		/**< Signal strength dBm or dBmV or dBuV */
	HINT32			netId;
	HINT32			tsId;
#if defined (CONFIG_DEMOD_TSMF)
	HUINT8			tsmfStatus;
#endif
	HUINT8			emergencyFlag;
#if (NEXUS_VERSION >= 1290)	/* modify for getting inband power level */
	BKNI_EventHandle	hStatusEvent;
#endif
}DB3383_SignalQuality_t;

#if defined (CONFIG_DEMOD_TSMF)
DB3383_TsmfInfo_t			gDB3383TsmfInfo[NUM_OF_CHANNEL];
HINT32 gRelNo[NUM_OF_CHANNEL];
HINT32 gTsmfNo, gTsmfexist;

typedef struct INTRN_TSMF_TSID
{
	HUINT8	ts_id0;
	HUINT8	ts_id1;
	HUINT8	original_network_id0;
	HUINT8	original_network_id1;
}INTRN_TSMF_TSID_t;


typedef struct INTRN_TSMF_TS_STATUS
{
#if 1	// Little Endian
__extension__ 	HUINT8	ts_status8:1,
			ts_status7:1,
			ts_status6:1,
			ts_status5:1,
			ts_status4:1,
			ts_status3:1,
			ts_status2:1,
			ts_status1:1;
__extension__	HUINT8	reserved:1,
			ts_status15:1,
			ts_status14:1,
			ts_status13:1,
			ts_status12:1,
			ts_status11:1,
			ts_status10:1,
			ts_status9:1;
#else	//Big Endian
	HUINT8	ts_status1:1,
			ts_status2:1,
			ts_status3:1,
			ts_status4:1,
			ts_status5:1,
			ts_status6:1,
			ts_status7:1,
			ts_status8:1;
	HUINT8	ts_status9:1,
			ts_status10:1,
			ts_status11:1,
			ts_status12:1,
			ts_status13:1,
			ts_status14:1,
			ts_status15:1,
			reserved:1;
#endif /* 1 */

}INTRN_TSMF_TS_STATUS_t;

typedef struct INTRN_TSMF_RECEIVE_STATUS
{
#if 1 // Little Endian
__extension__	HUINT8	receive_status4:2,
			receive_status3:2,
			receive_status2:2,
			receive_status1:2;
__extension__	HUINT8	receive_status8:2,
			receive_status7:2,
			receive_status6:2,
			receive_status5:2;
__extension__	HUINT8	receive_status12:2,
			receive_status11:2,
			receive_status10:2,
			receive_status9:2;
__extension__	HUINT8	emergency_indicator:1,
			reserved:1,
			receive_status15:2,
			receive_status14:2,
			receive_status13:2;

#else	//Big Endian
	HUINT8	receive_status1:2,
			receive_status2:2,
			receive_status3:2,
			receive_status4:2;
	HUINT8	receive_status5:2,
			receive_status6:2,
			receive_status7:2,
			receive_status8:2;
	HUINT8	receive_status9:2,
			receive_status10:2,
			receive_status11:2,
			receive_status12:2;
	HUINT8	receive_status13:2,
			receive_status14:2,
			receive_status15:2,
			reserved:1,
			emergency_indicator:1;
#endif /* 1 */
}INTRN_TSMF_RECEIVE_STATUS_t;

typedef struct INTRN_TSMF_RELATIVE_TS_NUMBER
{
__extension__	HUINT8	relative_ts_number2nd:4,
			relative_ts_number1st:4;
}INTRN_TSMF_RELATIVE_TS_NUMBER_t;

typedef struct INTRN_CH_TSMF_HEADER
{
	HUINT8 TSHdr0;
	HUINT8 TSHdr1;
	HUINT8 TSHdr2;
	HUINT8 TSHdr3;
#if 1	//Little Endian
__extension__	HUINT8 frame_sync0:5,
		   reserved0:3;
	HUINT8 frame_sync1;
__extension__	HUINT8 frame_type:4,
		   relative_ts_number_mode:1,
		   version_number:3;
#else	// Big Endian
	HUINT8 reserved0:3,
		   frame_sync0:5;
	HUINT8 frame_sync1;
	HUINT8 version_number:3,
		   relative_ts_number_mode:1,
		   frame_type:4;

#endif /* 1 */

	INTRN_TSMF_TS_STATUS_t ts_status;
	INTRN_TSMF_TSID_t aTSID[15];
	INTRN_TSMF_RECEIVE_STATUS_t receive_status;

	INTRN_TSMF_RELATIVE_TS_NUMBER_t relative_ts_number[26];
}INTRN_CH_TSMF_HEADER_t;

static INTRN_CH_TSMF_HEADER_t sTSMF_HEADER;
#endif

/****************************************************
	Static Variable
******************************************************/
static NEXUS_FrontendHandle s_st3383Frontend[NUM_OF_CHANNEL];

#if defined (CONFIG_DEMOD_TSMF)
#if (NEXUS_VERSION >= 1290)
static NEXUS_TsmfHandle s_st3383Tsmf[NUM_OF_CHANNEL];
#endif
#endif
static HUINT8	s_ucIsDb3383Lock[NUM_OF_CHANNEL];
static DB3383_SignalQuality_t		s_stDb3383SignalStatus[NUM_OF_CHANNEL];

#if defined (CONFIG_DEMOD_TSMF)
static HBOOL		sTsmfGoodFlag[NUM_OF_CHANNEL];
static HBOOL		sTsmfBadFlag[NUM_OF_CHANNEL];
static HBOOL		sTsmfEmgOnFlag[NUM_OF_CHANNEL];
static HBOOL		sTsmfEmgOffFlag[NUM_OF_CHANNEL];
static HUINT32		BadRetry[NUM_OF_CHANNEL], GoodRetry[NUM_OF_CHANNEL];
static HUINT32		beforeId1[NUM_OF_CHANNEL], beforeId2[NUM_OF_CHANNEL];
static HUINT32		gCurrentId[BUS_NUM];
#endif

/****************************************************
	Extern Function
******************************************************/
extern	void DRV_CH_CallNotifyCallback ( int nChannelID, HUINT32 unRequestId, DI_CH_SignalInfo_e etSigalInfo );

#if defined (CONFIG_DEMOD_TSMF)
extern DRV_Error DRV_CH_TSMF_RegisterTSMFCallback (pfnCH_TSMFNotify pfnTSMFNotify);
void DB3383_TsmfData(int nChannelId, HUINT16 *tsNo, HINT32 *tsID, HINT32 *netID);
void DB3383_CB_CH_TSMF_Notify(HUINT32 ulDemodId, HUINT8 *pucBuf);
int DB8883_FindRelativeNumber(int nChannelId, int tsid, int netid);
extern DRV_Error DRV_CH_TSMF_EnableMonitor(void);
#endif

#if (NEXUS_VERSION >= 1290)	/* modify for getting inband power level */
static void P_async_status_ready_callback(void *context, int param)
{
	CH_UNUSED(param);
	BKNI_SetEvent((BKNI_EventHandle)context);
}

static HUINT32 P_ConvertStrengthLevel(HINT32 nChannelId, HFLOAT32 fRfInputLevel)
{
	HFLOAT32 fStrength = 0;
	HINT32 nTmpRfLevel = 0;

	nTmpRfLevel = fRfInputLevel * 100.0;

	/*
		Strength table
		dBuV	/ %
		x > 75 		/ 100
		65 < x < 75	/ 90
		60 < x < 65	/ 80
		57 < x < 60	/ 70
		54 < x < 57	/ 60
		52 < x < 54	/ 50
		50 < x < 52	/ 40
		48 < x < 50	/ 30
		46 < x < 48	/ 20
		44 < x < 46	/ 10
		x < 44		/ 0
	*/

	if (nTmpRfLevel >= 7500)
	{
		fStrength = 100;
	}
	else if (nTmpRfLevel >= 6500)
	{
		fStrength = 90 + (10.0 * (nTmpRfLevel - 6500)/(7500 - 6500));
	}
	else if (nTmpRfLevel >= 6000)
	{
		fStrength = 80 + (10.0 * (nTmpRfLevel - 6000)/(6500 - 6000));
	}
	else if (nTmpRfLevel >= 5700)
	{
		fStrength = 70 + (10.0 * (nTmpRfLevel - 5700)/(6000 - 5700));
	}
	else if (nTmpRfLevel >= 5400)
	{
		fStrength = 60 + (10.0 * (nTmpRfLevel - 5400)/(5700 - 5400));
	}
	else if (nTmpRfLevel >= 5200)
	{
		fStrength = 50 + (10.0 * (nTmpRfLevel - 5200)/(5400 - 5200));
	}
	else if (nTmpRfLevel >= 5000)
	{
		fStrength = 40 + (10.0 * (nTmpRfLevel - 5000)/(5200 - 5000));
	}
	else if (nTmpRfLevel >= 4800)
	{
		fStrength = 30 + (10.0 * (nTmpRfLevel - 4800)/(5000 - 4800));
	}
	else if (nTmpRfLevel >= 4600)
	{
		fStrength = 20 + (10.0 * (nTmpRfLevel - 4600)/(4800 - 4600));
	}
	else if (nTmpRfLevel >= 4400)
	{
		fStrength = 10 + (10.0 * (nTmpRfLevel - 4400)/(4600 - 4400));
	}
	else
	{
		fStrength = 0;
	}

	CH_DI_Print(CH_LOG_SIGINFO, ("[%s] : fStrength [%d] = %d\n",__FUNCTION__, nChannelId, (HUINT32)fStrength));

	return (HUINT32)fStrength;
}


static HUINT32 P_ConvertQualityLevel(HINT32 nChannelId, HUINT32 ulSnr)
{
	HUINT32 ulQuality = 0;

	ulQuality = ulSnr/10 - 230;
	if (ulQuality >= 100)
	{
		ulQuality = 100;
	}
	else if (ulQuality <= 0)
	{
		ulQuality = 0;
	}

	CH_DI_Print(CH_LOG_SIGINFO, ("[%s] : fQuality [%d] = %d\n",__FUNCTION__,nChannelId, ulQuality));

	return ulQuality;
}
#endif

static HBOOL WaitQamLocked(int nChannelId)
{
	NEXUS_FrontendQamStatus stQamStatus;
	HUINT8 usLock = FALSE;
	int nUnitId=0, nCount=0;

	nUnitId = GetDeviceUnitId( nChannelId );

	while(usLock != TRUE)
	{
		NEXUS_Frontend_GetQamStatus(s_st3383Frontend[nUnitId], &stQamStatus);

		if ( (stQamStatus.fecLock == 1 )&&(stQamStatus.receiverLock == 1))
		{
			usLock = TRUE;
		}
		else
		{
			usLock = FALSE;
		}

		if(usLock == TRUE)
		{
			s_ucIsDb3383Lock[nUnitId] = TRUE;
			CH_DI_Print(CH_LOG_STATUS, ("WaitQamLocked[%d] : Locked!(nCount:%d) fec(%d)\n", nUnitId, nCount, stQamStatus.fecLock));
			return TRUE;
		}
		else
		{
			VK_TASK_Sleep(WAIT_QAM_LOCKING);
			if(nCount == LOCK_TRIAL_COUNT)
			{
				s_ucIsDb3383Lock[nUnitId] = FALSE;
				CH_DI_Print(CH_LOG_DEFAULT, ("WaitQamLocked[%d] : Lock trial expired fec(%d)\n", nUnitId, stQamStatus.fecLock));
				return FALSE;
			}
			else
			{
				nCount++;
			}
		}
	}

	return TRUE;
}

void DRV_C_InstallApi( void )
{
	CAB_InitDevice 			= &DB3383_InitDevice;
	CAB_SetTune 			= &DB3383_SetTune;
	CAB_CheckLock 			= &DB3383_CheckLock;
	CAB_GetStrengthQuality 	= &DB3383_GetStrengthQuality;
	CAB_SetPowerState 		= &DB3383_SetPowerState;
	CAB_EnableTsOutput 		= &DB3383_EnableTsOutput;
	CAB_DisableTsOutput 	= &DB3383_DisableTsOutput;
	CAB_ResetGpio = &DB3383_ResetGpio;
#if defined(CONFIG_DEMOD_TSMF)
	CAB_GetPathInfo = &DB3383_GetPathInfo;
#endif
	CH_DI_Print(CH_LOG_ARGS, ("DRV_C_InstallApi()  OK! \n"));

	return;
}

int DB3383_InitDevice(int nChannelId)
{
	 int nUnitId;
	 NEXUS_PlatformConfiguration platformConfig;

#if defined (CONFIG_DEMOD_TSMF)
#if (NEXUS_VERSION >= 1290)
	DRV_Error nRet = DRV_ERR;
	NEXUS_TsmfSettings tsmfSettings;
#endif
#endif

	 nUnitId = GetDeviceUnitId( nChannelId );

	 NEXUS_Platform_GetConfiguration(&platformConfig);

	 s_st3383Frontend[nUnitId] = platformConfig.frontend[nUnitId];

#if defined (CONFIG_DEMOD_TSMF)
	 sTsmfGoodFlag[nUnitId] = 1;
	 sTsmfEmgOffFlag[nUnitId] = 1;
#endif

	 if (NULL == s_st3383Frontend[nUnitId] )
	 {
		 CH_DI_Print(CH_LOG_DEFAULT, ("[DB3383_InitDevice] ERR! Unable to find CABLE capable frontend\n"));
		 return DI_CH_ERR;
	 }

#if defined (CONFIG_DEMOD_TSMF)
#if (NEXUS_VERSION >= 1290)
	s_st3383Tsmf[nUnitId] = NEXUS_Tsmf_Open(nUnitId, NULL);
	if (NULL == s_st3383Tsmf[nUnitId] )
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DB3383_InitDevice] ERR! Unable to Open CABLE TSMF\n"));
		return DI_CH_ERR;
	}

	nRet = DRV_CH_TSMF_SetHandle(nUnitId, (HUINT32)s_st3383Tsmf[nUnitId]);
	if (nRet != DRV_OK)
	{
		CH_DI_Print(CH_LOG_DEFAULT, ("[DB3383_InitDevice] ERR! Unable to Save TSMF Handle for other modules\n"));
		return DI_CH_ERR;
	}

	tsmfSettings.sourceType = NEXUS_TsmfSourceType_eMtsif;
	tsmfSettings.sourceTypeSettings.mtsif = NEXUS_Frontend_GetConnector(s_st3383Frontend[nUnitId]);
	tsmfSettings.semiAutomaticMode = false;
	tsmfSettings.relativeTsNum = 0xFF;
	tsmfSettings.enabled = false;
	tsmfSettings.parserforcesetting = false;
	NEXUS_Tsmf_SetSettings(s_st3383Tsmf[nUnitId], &tsmfSettings);
#endif
#endif

#if (NEXUS_VERSION >= 1290)	/* modify for getting inband power level */
	BKNI_CreateEvent(&(s_stDb3383SignalStatus[nChannelId].hStatusEvent));
#endif


	 return DI_CH_OK;
 }

#if defined (CONFIG_DEMOD_TSMF)
void DB3383_CB_CH_TSMF_Notify(HUINT32 ulDemodId, HUINT8 *pucBuf)
{
	int i, number=0, relidx=0;

	CH_DI_Print(CH_LOG_ARGS,("[%s] Demod ID : (%d)", __FUNCTION__, ulDemodId));

	VK_memcpy(&sTSMF_HEADER, pucBuf, sizeof(INTRN_CH_TSMF_HEADER_t));

	DD_MEM_Free(pucBuf);

	gTsmfexist = 1;
	CH_DI_Print(CH_LOG_OPT1, ("TS_HDR : %02x %02x %02x %02x\n", sTSMF_HEADER.TSHdr0,sTSMF_HEADER.TSHdr1, sTSMF_HEADER.TSHdr2, sTSMF_HEADER.TSHdr3));
	CH_DI_Print(CH_LOG_OPT1, ("reserved : 0x%x\n", sTSMF_HEADER.reserved0));
	CH_DI_Print(CH_LOG_OPT1, ("FRAME_SYNC : 0x%x, 0x%x\n", sTSMF_HEADER.frame_sync0, sTSMF_HEADER.frame_sync1));
	CH_DI_Print(CH_LOG_TSINFO, ("VERSION_NUM : 0x%x\n", sTSMF_HEADER.version_number));
	CH_DI_Print(CH_LOG_OPT1, ("relative_ts_number_mode : %02x\n", sTSMF_HEADER.relative_ts_number_mode));
	CH_DI_Print(CH_LOG_OPT1, ("frame_type : 0x%x\n", sTSMF_HEADER.frame_type));

	if ( ((sTSMF_HEADER.ts_status.ts_status1) &0x01) == 0x01)
		number += 1;
	if ( ((sTSMF_HEADER.ts_status.ts_status2) &0x01) == 0x01)
		number += 1;
	if ( ((sTSMF_HEADER.ts_status.ts_status3) &0x01) == 0x01)
		number += 1;
	if ( ((sTSMF_HEADER.ts_status.ts_status4) &0x01) == 0x01)
		number += 1;
	if ( ((sTSMF_HEADER.ts_status.ts_status5) &0x01) == 0x01)
		number += 1;
	if ( ((sTSMF_HEADER.ts_status.ts_status6) &0x01) == 0x01)
		number += 1;
	if ( ((sTSMF_HEADER.ts_status.ts_status7) &0x01) == 0x01)
		number += 1;
	if ( ((sTSMF_HEADER.ts_status.ts_status8) &0x01) == 0x01)
		number += 1;
	if ( ((sTSMF_HEADER.ts_status.ts_status9) &0x01) == 0x01)
		number += 1;
	if ( ((sTSMF_HEADER.ts_status.ts_status10) &0x01) == 0x01)
		number += 1;
	if ( ((sTSMF_HEADER.ts_status.ts_status11) &0x01) == 0x01)
		number += 1;
	if ( ((sTSMF_HEADER.ts_status.ts_status12) &0x01) == 0x01)
		number += 1;
	if ( ((sTSMF_HEADER.ts_status.ts_status13) &0x01) == 0x01)
		number += 1;
	if ( ((sTSMF_HEADER.ts_status.ts_status14) &0x01) == 0x01)
		number += 1;
	if ( ((sTSMF_HEADER.ts_status.ts_status15) &0x01) == 0x01)
		number += 1;

	gTsmfNo = number;
	gDB3383TsmfInfo[ulDemodId].tsNo = gTsmfNo;
	CH_DI_Print(CH_LOG_TSINFO, ("TSMF Number : %d\n", gTsmfNo));

	CH_DI_Print(CH_LOG_OPT1, ("ts_status[1] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status1));
	CH_DI_Print(CH_LOG_OPT1, ("ts_status[2] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status2));
	CH_DI_Print(CH_LOG_OPT1, ("ts_status[3] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status3));
	CH_DI_Print(CH_LOG_OPT1, ("ts_status[4] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status4));
	CH_DI_Print(CH_LOG_OPT1, ("ts_status[5] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status5));
	CH_DI_Print(CH_LOG_OPT1, ("ts_status[6] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status6));
	CH_DI_Print(CH_LOG_OPT1, ("ts_status[7] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status7));
	CH_DI_Print(CH_LOG_OPT1, ("ts_status[8] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status8));
	CH_DI_Print(CH_LOG_OPT1, ("ts_status[9] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status9));
	CH_DI_Print(CH_LOG_OPT1, ("ts_status[10] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status10));
	CH_DI_Print(CH_LOG_OPT1, ("ts_status[11] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status11));
	CH_DI_Print(CH_LOG_OPT1, ("ts_status[12] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status12));
	CH_DI_Print(CH_LOG_OPT1, ("ts_status[13] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status13));
	CH_DI_Print(CH_LOG_OPT1, ("ts_status[14] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status14));
	CH_DI_Print(CH_LOG_OPT1, ("ts_status[15] : 0x%x\n", sTSMF_HEADER.ts_status.ts_status15));
	CH_DI_Print(CH_LOG_OPT1, ("ts_status[reserved] : 0x%x\n", sTSMF_HEADER.ts_status.reserved));

	for(i=0; i<15; i++)
	{
		gDB3383TsmfInfo[ulDemodId].tsID[i] = ((sTSMF_HEADER.aTSID[i].ts_id0<<8) | sTSMF_HEADER.aTSID[i].ts_id1);
		gDB3383TsmfInfo[ulDemodId].netID[i] = ((sTSMF_HEADER.aTSID[i].original_network_id0<<8) | sTSMF_HEADER.aTSID[i].original_network_id1);
		CH_DI_Print(CH_LOG_OPT1, ("ts_id[%d] : 0x%x, original_network_id[i+1] : 0x%x \n", i+1, gDB3383TsmfInfo[ulDemodId].tsID[i], gDB3383TsmfInfo[ulDemodId].netID[i] ));
	}

	CH_DI_Print(CH_LOG_TSINFO, ("ts_id[%d] : 0x%x, original_network_id[1] : 0x%x \n", 1, gDB3383TsmfInfo[ulDemodId].tsID[0], gDB3383TsmfInfo[ulDemodId].netID[0] ));

	gDB3383TsmfInfo[ulDemodId].uItsmfStatus[0] = sTSMF_HEADER.receive_status.receive_status1 &0x03;
	gDB3383TsmfInfo[ulDemodId].uItsmfStatus[1] = sTSMF_HEADER.receive_status.receive_status2 &0x03;
	gDB3383TsmfInfo[ulDemodId].uItsmfStatus[2] = sTSMF_HEADER.receive_status.receive_status3 &0x03;
	gDB3383TsmfInfo[ulDemodId].uItsmfStatus[3] = sTSMF_HEADER.receive_status.receive_status4 &0x03;
	gDB3383TsmfInfo[ulDemodId].uItsmfStatus[4] = sTSMF_HEADER.receive_status.receive_status5 &0x03;
	gDB3383TsmfInfo[ulDemodId].uItsmfStatus[5] = sTSMF_HEADER.receive_status.receive_status6 &0x03;
	gDB3383TsmfInfo[ulDemodId].uItsmfStatus[6] = sTSMF_HEADER.receive_status.receive_status7 &0x03;
	gDB3383TsmfInfo[ulDemodId].uItsmfStatus[7] = sTSMF_HEADER.receive_status.receive_status8 &0x03;
	gDB3383TsmfInfo[ulDemodId].uItsmfStatus[8] = sTSMF_HEADER.receive_status.receive_status9 &0x03;
	gDB3383TsmfInfo[ulDemodId].uItsmfStatus[9] = sTSMF_HEADER.receive_status.receive_status10 &0x03;
	gDB3383TsmfInfo[ulDemodId].uItsmfStatus[10] = sTSMF_HEADER.receive_status.receive_status11 &0x03;
	gDB3383TsmfInfo[ulDemodId].uItsmfStatus[11] = sTSMF_HEADER.receive_status.receive_status12 &0x03;
	gDB3383TsmfInfo[ulDemodId].uItsmfStatus[12] = sTSMF_HEADER.receive_status.receive_status13 &0x03;
	gDB3383TsmfInfo[ulDemodId].uItsmfStatus[13] = sTSMF_HEADER.receive_status.receive_status14 &0x03;
	gDB3383TsmfInfo[ulDemodId].uItsmfStatus[14] = sTSMF_HEADER.receive_status.receive_status15 &0x03;

	if (gRelNo[ulDemodId] == 0)
	{
		relidx = 0;
	}
	else
	{
		relidx = gRelNo[ulDemodId] - 1;
	}

	s_stDb3383SignalStatus[ulDemodId].tsmfStatus = gDB3383TsmfInfo[ulDemodId].uItsmfStatus[relidx];
	s_stDb3383SignalStatus[ulDemodId].emergencyFlag = sTSMF_HEADER.receive_status.emergency_indicator;

	CH_DI_Print(CH_LOG_TSINFO, ("receive_status[emergency_indicator] : 0x%x, status(%d)\n", s_stDb3383SignalStatus[ulDemodId].emergencyFlag, s_stDb3383SignalStatus[ulDemodId].tsmfStatus));

	for(i=0; i<26; i++)
	{
		CH_DI_Print(CH_LOG_OPT1, ("relative_ts_number[slot-%d] : 0x%x\n", i*2, sTSMF_HEADER.relative_ts_number[i].relative_ts_number1st));
		CH_DI_Print(CH_LOG_OPT1, ("relative_ts_number[slot-%d] : 0x%x\n", i*2+1, sTSMF_HEADER.relative_ts_number[i].relative_ts_number2nd));
	}
}
#endif

int  DB3383_ResetGpio(int nChannelId)
{
	HUINT32 	nUnitId = 0;

	nUnitId = GetDeviceUnitId( nChannelId );

	CH_DI_Print(CH_LOG_ARGS,("nChannelID[%d] / nUnitId[%d] \n", nChannelId, nUnitId));

	return DI_CH_OK;
}

int DB3383_SetTune(int nChannelId, HUINT32 ulWorkingId, CH_CAB_TuneParam_t *pstCabTuneParam )
{
	HINT32 nUnitId = 0, nRet = DI_CH_OK, nResult = DI_CH_OK;
#if defined(DEBUG_FEC_LOCK)
	NEXUS_FrontendQamStatus	stQamStatus;
#endif
	NEXUS_FrontendQamSettings qamSettings;

#if defined (CONFIG_DEMOD_TSMF)
	HINT32 tsid=0, netid=0, nRelNo=0;
	HUINT8 i=0, nDelay = 0, nCnt = 0;
	NEXUS_TsmfSettings tsmfSettings;
	DRV_CH_TSMF_STATUS_e eStatus;
	HUINT32 ulNoMatchedTsmfInfoCnt = 0;
#endif

	CH_UNUSED(ulWorkingId);
	nUnitId = GetDeviceUnitId( nChannelId );

#if defined (CONFIG_DEMOD_TSMF)
	gTsmfexist = 0;

	beforeId1[nChannelId] = 0;
	beforeId2[nChannelId] = 0;
	gCurrentId[nChannelId] = 0;

	s_stDb3383SignalStatus[nChannelId].tsmfStatus = 0;
	BadRetry[nChannelId] = 0;
	GoodRetry[nChannelId] = 0;

	VK_memset(&gDB3383TsmfInfo[0], 0, sizeof(gDB3383TsmfInfo[NUM_OF_CHANNEL]));
#endif

	CH_DI_Print(CH_LOG_TUNE,("DB3383_SetTune[%d][%dMHz]\n", nChannelId,  pstCabTuneParam->ulFrequency/1000));

#if defined (CONFIG_DEMOD_TSMF)
	DRV_CH_TSMF_RegisterTSMFCallback(DB3383_CB_CH_TSMF_Notify);
#endif

	s_stDb3383SignalStatus[nChannelId].tsId = pstCabTuneParam->nTsId;
	s_stDb3383SignalStatus[nChannelId].netId = pstCabTuneParam->nNetId;

#if defined (CONFIG_DEMOD_TSMF)
	/* Disable TSMF Monitor */
	DRV_CH_TSMF_DisableMonitor();
#endif

	NEXUS_Frontend_GetDefaultQamSettings(&qamSettings);
	qamSettings.frequency = pstCabTuneParam->ulFrequency * KHZ_TO_HZ;
	qamSettings.annex = NEXUS_FrontendQamAnnex_eC;

	switch(pstCabTuneParam->etModulation)
	{
		case DI_CH_CMOD_16QAM:
			qamSettings.mode = NEXUS_FrontendQamMode_e16;
			qamSettings.symbolRate = pstCabTuneParam->ulSymbolRate * KHZ_TO_HZ;
			break;

		case DI_CH_CMOD_32QAM:
			qamSettings.mode = NEXUS_FrontendQamMode_e32;
			qamSettings.symbolRate = pstCabTuneParam->ulSymbolRate * KHZ_TO_HZ;
			break;

		case DI_CH_CMOD_64QAM:
			qamSettings.mode = NEXUS_FrontendQamMode_e64;
			qamSettings.symbolRate =  pstCabTuneParam->ulSymbolRate * KHZ_TO_HZ; // 5274000; // 5056900;
			break;

		case DI_CH_CMOD_256QAM:
			qamSettings.mode = NEXUS_FrontendQamMode_e256;
			qamSettings.symbolRate = pstCabTuneParam->ulSymbolRate * KHZ_TO_HZ; //5274000; //  5360537;
			break;

		default:
			qamSettings.mode = NEXUS_FrontendQamMode_eAuto_64_256;
			qamSettings.symbolRate = pstCabTuneParam->ulSymbolRate * KHZ_TO_HZ;
			break;
	}

	qamSettings.autoAcquire = false; // use NEXUS to auto acquire if loss of lock

#if (NEXUS_VERSION >= 1290)	/* modify for getting inband power level */
	qamSettings.asyncStatusReadyCallback.callback = P_async_status_ready_callback;
	qamSettings.asyncStatusReadyCallback.context = s_stDb3383SignalStatus[nChannelId].hStatusEvent;
#endif

	nRet = NEXUS_Frontend_TuneQam(s_st3383Frontend[nUnitId], &qamSettings);
	if( nRet != DI_CH_OK )
	{
		CH_DI_Print(CH_LOG_DEFAULT,("DB3383_SetTune[%d] :: NEXUS_Frontend_TuneQam.. Fail.. nRet(0x%x)", nUnitId, nRet));
		nResult = DI_CH_ERR;
	}

	nRet = WaitQamLocked( nChannelId );
	if( nRet == TRUE )
	{
#if (NEXUS_VERSION >= 1404)
		NEXUS_Frontend_SetInputBandEnable(s_st3383Frontend[nUnitId], true);
#endif

#if defined (CONFIG_DEMOD_TSMF)
		CH_DI_Print(CH_LOG_TUNE,("\n\n\n-------------------------------------------------------\n"));
		CH_DI_Print(CH_LOG_TUNE,("DB3383_SetTune[%d][%d] :: Locked!!! TSID(0x%x), NETID(0x%x) \n",nUnitId, pstCabTuneParam->ulFrequency/1000 ,s_stDb3383SignalStatus[nChannelId].tsId, s_stDb3383SignalStatus[nChannelId].netId));
		CH_DI_Print(CH_LOG_TUNE,("---------------------------------------------------------\n\n\n"));

		s_ucIsDb3383Lock[nUnitId] = TRUE;
		nResult = DI_CH_OK;

		tsid  =  s_stDb3383SignalStatus[nChannelId].tsId;
		netid = s_stDb3383SignalStatus[nChannelId].netId;

		if ( (tsid == 0 ) && (netid == 0 ))
		{
			nRelNo = 0;
			gDB3383TsmfInfo[nChannelId].tsNo = nRelNo;
			CH_DI_Print(CH_LOG_TUNE,("\n\n\n >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n"));
			CH_DI_Print(CH_LOG_TUNE,(" DB3383_SetTune : Cable Stream Locking TSID(0x%x), NETID(0x%x)\n", tsid, netid));
			CH_DI_Print(CH_LOG_TUNE,(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\n\n"));

#if (NEXUS_VERSION >= 1290)
			tsmfSettings.sourceType = NEXUS_TsmfSourceType_eMtsif;
			tsmfSettings.sourceTypeSettings.mtsif = NEXUS_Frontend_GetConnector(s_st3383Frontend[nUnitId]);
			tsmfSettings.relativeTsNum = 0xFF;
			tsmfSettings.semiAutomaticMode = false;
			tsmfSettings.parserforcesetting = true;
			tsmfSettings.enabled=false;

			NEXUS_Tsmf_SetSettings(s_st3383Tsmf[nUnitId], &tsmfSettings);

			NEXUS_Frontend_ReapplyTSMFSettings(s_st3383Frontend[nUnitId]);
#else
			NEXUS_Frontend_GetTsmfSettings(s_st3383Frontend[nUnitId], &tsmfSettings);

			tsmfSettings.relativeTsNum = 0xFF;
			tsmfSettings.semiAutomaticMode = false;
			tsmfSettings.parserforcesetting = true;
			NEXUS_Frontend_SetTsmfSettings(s_st3383Frontend[nUnitId], &tsmfSettings);
#endif

			/* Enable TSMF monitor */
			DRV_CH_TSMF_EnableMonitor();
			return nResult;
		}

retry_for_not_matched_tsmf_info:

		DRV_CH_TSMF_RequestSearch(nChannelId);

		if ( (tsid == -1) && (netid == -1) )
		{
			nDelay = 10;
			while(1)
			{
				nCnt++;
				VK_TASK_Sleep(nDelay);
				DRV_CH_TSMF_GetStatus(nChannelId, &eStatus);
				if(eStatus != DRV_CH_TSMF_SEARCHING)
				{
					break;
				}
			}

			for(i=0; i<TSMF_RETRY_CNT; i++)
			{
				if(eStatus == DRV_CH_TSMF_TIMEOUT)
				{
#if defined(DEBUG_FEC_LOCK)
					NEXUS_Frontend_GetQamStatus(s_st3383Frontend[nUnitId], &stQamStatus);
					CH_DI_Print(CH_LOG_STATUS, ("Retry[%d] : Locked! Count(%d), Lock(%d) fec(%d) UCB(%d)\n", nUnitId,  i, stQamStatus.receiverLock, stQamStatus.fecLock, stQamStatus.fecUncorrected));
					DRV_CH_TSMF_GetStatus(nChannelId, &eStatus);
					if (eStatus == DRV_CH_TSMF_RECEIVED)
					{
						CH_DI_Print(CH_LOG_STATUS, ("Retry[%d] : DRV_CH_TSMF_RECEIVED!)\n",  i));
						break;
					}
					VK_TASK_Sleep(50);
#else
					CH_DI_Print(CH_LOG_STATUS,(" \n\nDB3383_SetTune : TSMF Retry!!\n\n"));

					NEXUS_Frontend_Untune(s_st3383Frontend[nUnitId]);

					nRet = NEXUS_Frontend_TuneQam(s_st3383Frontend[nUnitId], &qamSettings);
					if( nRet != DI_CH_OK )
					{
						CH_DI_Print(CH_LOG_DEFAULT,("DB3383_SetTune[%d] :: NEXUS_Frontend_TuneQam.. Fail.. nRet(0x%x)", nUnitId, nRet));
						nResult = DI_CH_ERR;
					}

					nRet = WaitQamLocked( nChannelId );
					if( nRet == TRUE )
					{
						DRV_CH_TSMF_RequestSearch(nChannelId);

						while(1)
						{
							nCnt++;
							VK_TASK_Sleep(nDelay);
							DRV_CH_TSMF_GetStatus(nChannelId, &eStatus);
							if(eStatus != DRV_CH_TSMF_SEARCHING)
							{
								break;
							}
						}

						if (eStatus == DRV_CH_TSMF_RECEIVED)
						{
							CH_DI_Print(CH_LOG_STATUS,(" \n\nDB3383_SetTune : TSMF Retry -> Received(%d)!!\n\n", i));
							break;
						}
					}
					else
					{
						CH_DI_Print(CH_LOG_STATUS,(" \n\nDB3383_SetTune : TSMF_Retry ( %d )ms\n\n", 10*nCnt));
					}
#endif
				}
			}
			CH_DI_Print(CH_LOG_STATUS,(" \n\nDB3383_SetTune : TSMF_RECEIVED ( %d )ms\n\n", 10*nCnt));

			/* Disable Search demod PATH */
			DRV_CH_TSMF_CancleSearch(nChannelId);

			return nResult;
		}
		else
		{
			nDelay = 10;
			while(1)
			{
				nCnt++;
				VK_TASK_Sleep(nDelay);
				DRV_CH_TSMF_GetStatus(nChannelId, &eStatus);
				if(eStatus != DRV_CH_TSMF_SEARCHING)
				{
					break;
				}
			}

			for(i=0; i<TSMF_RETRY_CNT; i++)
			{
				if(eStatus == DRV_CH_TSMF_TIMEOUT)
				{
					CH_DI_Print(CH_LOG_STATUS,(" \n\nDB3383_SetTune : TSMF Retry!!\n\n"));
					NEXUS_Frontend_Untune(s_st3383Frontend[nUnitId]);
					nRet = NEXUS_Frontend_TuneQam(s_st3383Frontend[nUnitId], &qamSettings);
					if( nRet != DI_CH_OK )
					{
						CH_DI_Print(CH_LOG_DEFAULT,("DB3383_SetTune[%d] :: NEXUS_Frontend_TuneQam.. Fail.. nRet(0x%x)", nUnitId, nRet));
						nResult = DI_CH_ERR;
					}

					nRet = WaitQamLocked( nChannelId );
					if( nRet == TRUE )
					{
						DRV_CH_TSMF_RequestSearch(nChannelId);

						while(1)
						{
							nCnt++;
							VK_TASK_Sleep(nDelay);
							DRV_CH_TSMF_GetStatus(nChannelId, &eStatus);
							if(eStatus != DRV_CH_TSMF_SEARCHING)
							{
								break;
							}
						}

						if (eStatus == DRV_CH_TSMF_RECEIVED)
						{
							CH_DI_Print(CH_LOG_STATUS,(" \n\nDB3383_SetTune : TSMF Retry -> Received(%d)!!\n\n", i));
							break;
						}
					}
					else
					{
						CH_DI_Print(CH_LOG_STATUS,(" \n\nDB3383_SetTune : TSMF_Retry ( %d )ms\n\n", 10*nCnt));
					}
				}
			}

			if(eStatus == DRV_CH_TSMF_RECEIVED)
			{
				nRelNo = DB8883_FindRelativeNumber(nChannelId, tsid, netid);
				if(nRelNo == 0)
				{
					CH_DI_Print(CH_LOG_TSINFO, ("DB3383_SetTune[%d]:: TS ID and Network ID not matched, try again (%d)\n",  nChannelId, ulNoMatchedTsmfInfoCnt));
					if(ulNoMatchedTsmfInfoCnt < NOT_MATCHED_TSINFO_RETRY_CNT)
					{
						ulNoMatchedTsmfInfoCnt++;
						goto retry_for_not_matched_tsmf_info;
					}
					CH_DI_Print(CH_LOG_DEFAULT, ("DB3383_SetTune[%d]:: give-up for finding matched TSMF info, retry count = %d\n",  nChannelId, ulNoMatchedTsmfInfoCnt));
				}

				gRelNo[nChannelId] = nRelNo;
				CH_DI_Print(CH_LOG_TSINFO, ("DB3383_SetTune[%d][%d]::  tsid [%x], netid[%x], tsno[%d], relNo[%d], cnt[%d]\n",  nChannelId,   pstCabTuneParam->ulFrequency/1000, tsid, netid, gDB3383TsmfInfo[nChannelId].tsNo, nRelNo, i));
				CH_DI_Print(CH_LOG_STATUS,("\n\nDB3383_SetTune : TSMF_RECEIVED ( %d )ms\n\n", 10*nCnt));

				/* Disable Search demod PATH */
				DRV_CH_TSMF_CancleSearch(nChannelId);

				/* Enable TSMF monitor */
				DRV_CH_TSMF_EnableMonitor();
			}
			else
			{
				CH_DI_Print(CH_LOG_TUNE,("\n\n\n >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n"));
				CH_DI_Print(CH_LOG_TUNE,(" DB3383_SetTune : Normal Stream Locking TSID(0x%x)\n", tsid));
				CH_DI_Print(CH_LOG_TUNE,(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\n\n"));

#if (NEXUS_VERSION >= 1290)
				//NEXUS_Tsmf_GetSettings(s_st3383Tsmf[nUnitId], &tsmfSettings);

				tsmfSettings.sourceType = NEXUS_TsmfSourceType_eMtsif;
				tsmfSettings.sourceTypeSettings.mtsif = NEXUS_Frontend_GetConnector(s_st3383Frontend[nUnitId]);
				tsmfSettings.relativeTsNum = 0xFF;
				tsmfSettings.semiAutomaticMode = false;
				tsmfSettings.parserforcesetting = true;
				tsmfSettings.enabled=false;

				NEXUS_Tsmf_SetSettings(s_st3383Tsmf[nUnitId], &tsmfSettings);

				NEXUS_Frontend_ReapplyTSMFSettings(s_st3383Frontend[nUnitId]);
#else
				NEXUS_Frontend_GetTsmfSettings(s_st3383Frontend[nUnitId], &tsmfSettings);

				tsmfSettings.relativeTsNum = 0xFF;
				tsmfSettings.semiAutomaticMode = false;
				tsmfSettings.parserforcesetting = true;
				NEXUS_Frontend_SetTsmfSettings(s_st3383Frontend[nUnitId], &tsmfSettings);
#endif

				/* Disable Search demod PATH */
				DRV_CH_TSMF_CancleSearch(nChannelId);

				/* Enable TSMF monitor */
				DRV_CH_TSMF_EnableMonitor();
				return nResult;
			}
		}

		CH_DI_Print(CH_LOG_TUNE,("\n\n\n >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n"));
		CH_DI_Print(CH_LOG_TUNE,(" DB3383_SetTune :: TSID(0x%x), TSNUM(%d), RELATIVE(%d)\n", tsid, gDB3383TsmfInfo[nChannelId].tsNo, nRelNo));
		CH_DI_Print(CH_LOG_TUNE,(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\n\n"));

#if (NEXUS_VERSION >= 1290)
		tsmfSettings.sourceType = NEXUS_TsmfSourceType_eMtsif;
		tsmfSettings.sourceTypeSettings.mtsif = NEXUS_Frontend_GetConnector(s_st3383Frontend[nUnitId]);
		tsmfSettings.relativeTsNum = nRelNo;
		tsmfSettings.semiAutomaticMode = false;
		tsmfSettings.enabled = false;
		tsmfSettings.parserforcesetting = false;

		NEXUS_Tsmf_SetSettings(s_st3383Tsmf[nUnitId], &tsmfSettings);

		NEXUS_Frontend_ReapplyTSMFSettings(s_st3383Frontend[nUnitId]);
#else
		NEXUS_Frontend_GetTsmfSettings(s_st3383Frontend[nUnitId], &tsmfSettings);

		CH_DI_Print(CH_LOG_TUNE,("DB3383_SetTune[%d] :: semiAutomaticMode[%d], enabled[%d] \n",nUnitId, tsmfSettings.semiAutomaticMode, tsmfSettings.enabled ));

		tsmfSettings.fieldVerifyConfig.crcCheckDisable = 0;
		tsmfSettings.fieldVerifyConfig.relTsStatusCheckDisable = 0;
		tsmfSettings.fieldVerifyConfig.frameTypeCheckDisable = 0;
		tsmfSettings.fieldVerifyConfig.relTsModeCheckDisable = 0;
		tsmfSettings.fieldVerifyConfig.syncCheckDisable = 0;
		tsmfSettings.fieldVerifyConfig.ccCheckDisable = 0;
		tsmfSettings.fieldVerifyConfig.adapCheckDisable = 0;
		tsmfSettings.fieldVerifyConfig.scCheckDisable = 0;
		tsmfSettings.fieldVerifyConfig.tsPriorCheckDisable = 0;
		tsmfSettings.fieldVerifyConfig.pusiCheckDisable = 0;
		tsmfSettings.fieldVerifyConfig.teiCheckDisable = 0;
		tsmfSettings.fieldVerifyConfig.versionChangeMode = 0;
		tsmfSettings.relativeTsNum = nRelNo;
		tsmfSettings.semiAutomaticMode = false;
		tsmfSettings.enabled = false;
		tsmfSettings.parserforcesetting = false;
		nRet = NEXUS_Frontend_SetTsmfSettings(s_st3383Frontend[nUnitId], &tsmfSettings);
#endif

		if (nRelNo != 0 )
		{
#if (NEXUS_VERSION >= 1290)
			tsmfSettings.sourceType = NEXUS_TsmfSourceType_eMtsif;
			tsmfSettings.sourceTypeSettings.mtsif = NEXUS_Frontend_GetConnector(s_st3383Frontend[nUnitId]);

			tsmfSettings.fieldVerifyConfig.crcCheckDisable = 0;
			tsmfSettings.fieldVerifyConfig.relTsStatusCheckDisable = 0;
			tsmfSettings.fieldVerifyConfig.frameTypeCheckDisable = 0;
			tsmfSettings.fieldVerifyConfig.relTsModeCheckDisable = 0;
			tsmfSettings.fieldVerifyConfig.syncCheckDisable = 0;
			tsmfSettings.fieldVerifyConfig.ccCheckDisable = 0;
			tsmfSettings.fieldVerifyConfig.adapCheckDisable = 0;
			tsmfSettings.fieldVerifyConfig.scCheckDisable = 0;
			tsmfSettings.fieldVerifyConfig.tsPriorCheckDisable = 0;
			tsmfSettings.fieldVerifyConfig.pusiCheckDisable = 0;
			tsmfSettings.fieldVerifyConfig.teiCheckDisable = 0;
			tsmfSettings.fieldVerifyConfig.versionChangeMode = 0;
			tsmfSettings.relativeTsNum = nRelNo;
			tsmfSettings.semiAutomaticMode = false;
			tsmfSettings.enabled = true;
			tsmfSettings.parserforcesetting = false;

			NEXUS_Tsmf_SetSettings(s_st3383Tsmf[nUnitId], &tsmfSettings);

			NEXUS_Frontend_ReapplyTSMFSettings(s_st3383Frontend[nUnitId]);
#else
			NEXUS_Frontend_GetTsmfSettings(s_st3383Frontend[nUnitId], &tsmfSettings);

			CH_DI_Print(CH_LOG_TUNE,("DB3383_SetTune[%d] :: semiAutomaticMode[%d], enabled[%d] \n",nUnitId, tsmfSettings.semiAutomaticMode, tsmfSettings.enabled ));

			tsmfSettings.fieldVerifyConfig.crcCheckDisable = 0;
			tsmfSettings.fieldVerifyConfig.relTsStatusCheckDisable = 0;
			tsmfSettings.fieldVerifyConfig.frameTypeCheckDisable = 0;
			tsmfSettings.fieldVerifyConfig.relTsModeCheckDisable = 0;
			tsmfSettings.fieldVerifyConfig.syncCheckDisable = 0;
			tsmfSettings.fieldVerifyConfig.ccCheckDisable = 0;
			tsmfSettings.fieldVerifyConfig.adapCheckDisable = 0;
			tsmfSettings.fieldVerifyConfig.scCheckDisable = 0;
			tsmfSettings.fieldVerifyConfig.tsPriorCheckDisable = 0;
			tsmfSettings.fieldVerifyConfig.pusiCheckDisable = 0;
			tsmfSettings.fieldVerifyConfig.teiCheckDisable = 0;
			tsmfSettings.fieldVerifyConfig.versionChangeMode = 0;
			tsmfSettings.relativeTsNum = nRelNo;
			tsmfSettings.semiAutomaticMode = false;
			tsmfSettings.enabled = true;
			tsmfSettings.parserforcesetting = false;
			nRet = NEXUS_Frontend_SetTsmfSettings(s_st3383Frontend[nUnitId], &tsmfSettings);
#endif
		}
#else
		s_ucIsDb3383Lock[nUnitId] = TRUE;
		nResult = DI_CH_OK;
#endif

		CH_DI_Print(CH_LOG_TUNE,("DB3383_SetTune[%d] :: Locked \n",nUnitId));

		/* update DB values for channel */
		DB3383_CheckLock(nChannelId);
	}
	else
	{
		CH_DI_Print(CH_LOG_TUNE,("DB3383_SetTune[%d] :: Unlocked \n",nUnitId));
		s_ucIsDb3383Lock[nUnitId] = FALSE;
		nResult = DI_CH_ERR;
	}

	return nResult;
}



HBOOL DB3383_CheckLock(int nChannelId)
{

	HINT32 nUnitId = 0;
#if (NEXUS_VERSION >= 1290)	/* modify for getting inband power level */
	NEXUS_Error nResult;
#endif

	NEXUS_FrontendQamStatus stQamStatus;
	HFLOAT32	fSignalPowerdBuV = 0;
	HBOOL	Locked=0;
	HINT32  nQuality=0, nStrength=0;

#if defined (CONFIG_DEMOD_TSMF)
	HUINT32 unRequestId = 0;
#endif

	nUnitId = GetDeviceUnitId( nChannelId );

#if (NEXUS_VERSION >= 1290)	/* modify for getting inband power level */
	nResult = NEXUS_Frontend_RequestQamAsyncStatus(s_st3383Frontend[nUnitId]);
	if(nResult == NEXUS_SUCCESS)
	{
		nResult = BKNI_WaitForEvent(s_stDb3383SignalStatus[nChannelId].hStatusEvent, 1000);
		if (nResult)
		{
			CH_DI_Print(CH_LOG_DEFAULT,("%s[%d] BKNI_WaitForEvent() Status not returned\n",__FUNCTION__, nUnitId));
		}
		NEXUS_Frontend_GetQamAsyncStatus(s_st3383Frontend[nUnitId] , &stQamStatus);
	}
	else
	{
		CH_DI_Print(CH_LOG_DEFAULT,("%s[%d] NEXUS_Frontend_RequestQamAsyncStatus return error, Call NEXUS_Frontend_GetQamStatus \n",__FUNCTION__, nUnitId));
		NEXUS_Frontend_GetQamStatus(s_st3383Frontend[nUnitId], &stQamStatus);
	}
#else
	NEXUS_Frontend_GetQamStatus(s_st3383Frontend[nUnitId], &stQamStatus);
#endif

	Locked = stQamStatus.receiverLock;

	if(Locked)
	{
		CH_DI_Print(CH_LOG_STATUS,("DB3383_CheckLock[%d] receiverLock=%d, fecLock=%d, CB=%d, UCB=%d\n",nChannelId, stQamStatus.receiverLock, stQamStatus.fecLock,stQamStatus.fecCorrected, stQamStatus.fecUncorrected));

#if (NEXUS_VERSION >= 1290)	/* modify for getting inband power level */
		fSignalPowerdBuV = (float)(stQamStatus.dsChannelPower/10.0)+60;
#else
		fSignalPowerdBuV = (float)(stQamStatus.equalizerGain/1000.0);
#endif

		CH_DI_Print(CH_LOG_SIGINFO,("DB3383_CheckLock[%d] : Locked \n",nChannelId));
		CH_DI_Print(CH_LOG_SIGINFO,("====================== DB3383_CheckLock[%d] ======================\n",nChannelId));
		CH_DI_Print(CH_LOG_SIGINFO,("  freq			  = %d [MHz]\n",stQamStatus.settings.frequency / 1000000));
		CH_DI_Print(CH_LOG_SIGINFO,("  receiverLock             = %u\n", stQamStatus.receiverLock));
		CH_DI_Print(CH_LOG_SIGINFO,("  fecLock                  = %u\n", stQamStatus.fecLock));
		CH_DI_Print(CH_LOG_SIGINFO,("  symbolRate               = %u [Baud]\n", stQamStatus.symbolRate));
		CH_DI_Print(CH_LOG_SIGINFO,("  symbolRateError          = %d [Baud]\n", stQamStatus.symbolRateError));
		CH_DI_Print(CH_LOG_SIGINFO,("  ifAgcLevel               = %2.1f [%%]\n", stQamStatus.ifAgcLevel/10.0));
		CH_DI_Print(CH_LOG_SIGINFO,("  snrEstimate              = %2.2f [dB]\n", stQamStatus.snrEstimate/100.0));
		CH_DI_Print(CH_LOG_SIGINFO,("  fecUnerrored         = %u\n", stQamStatus.goodRsBlockCount));
		CH_DI_Print(CH_LOG_SIGINFO,("  fecCorrected             = %u\n", stQamStatus.fecCorrected));
		CH_DI_Print(CH_LOG_SIGINFO,("  fecUncorrected           = %u\n", stQamStatus.fecUncorrected));
		CH_DI_Print(CH_LOG_SIGINFO,("  berEstimate              = %3.2e\n", stQamStatus.postRsBer/1000000000000.0));
		CH_DI_Print(CH_LOG_SIGINFO,("  dsChannelPower           = %3.2f [dBuV]\n", fSignalPowerdBuV));
		CH_DI_Print(CH_LOG_SIGINFO,("=================================================================\n"));

#if (NEXUS_VERSION >= 1290)	/* modify for getting inband power level */
		nStrength = P_ConvertStrengthLevel(nChannelId,fSignalPowerdBuV);
		nQuality = P_ConvertQualityLevel(nChannelId,stQamStatus.snrEstimate);
#else
		nQuality = stQamStatus.snrEstimate/10 - 230;
		if (nQuality >=100)
		{
			nQuality = 100;
		}
		else if (nQuality <= 0)
		{
			nQuality = 0;
		}

		nStrength = 93 - stQamStatus.ifAgcLevel/10;

		if (nStrength >= 80)
		{
			nStrength = 100;
		}
		else if (nStrength <= 10)
		{
			nStrength = 10;
		}
#endif

		s_stDb3383SignalStatus[nChannelId].nStrength = (HUINT32)nStrength;
		s_stDb3383SignalStatus[nChannelId].ulQuality = (HUINT32)nQuality;
		s_stDb3383SignalStatus[nChannelId].ucAgc = 0;		// Ignore
		s_stDb3383SignalStatus[nChannelId].ulBer = stQamStatus.postRsBer;
		s_stDb3383SignalStatus[nChannelId].ulSnr = stQamStatus.snrEstimate;
		s_stDb3383SignalStatus[nChannelId].ucRfAgc = stQamStatus.rfAgcLevel/10;
		s_stDb3383SignalStatus[nChannelId].ucIfAgc = stQamStatus.ifAgcLevel/10;
		s_stDb3383SignalStatus[nChannelId].ulCorrectedNo = stQamStatus.fecCorrected;
		s_stDb3383SignalStatus[nChannelId].ulUnCorrectedNo = stQamStatus.fecUncorrected;
		s_stDb3383SignalStatus[nChannelId].ulUnErroredNo = stQamStatus.goodRsBlockCount ;
		s_stDb3383SignalStatus[nChannelId].fSignalPower = fSignalPowerdBuV;
		s_ucIsDb3383Lock[nUnitId] = TRUE;

#if defined (CONFIG_DEMOD_TSMF)
		gCurrentId[nChannelId] = 1;

		/* 서비스 ID가 바뀔때 마다 메시지 보내고, 동일 service id일경우는 처음 한번만 보냄. */
		if( (s_stDb3383SignalStatus[nChannelId].ulSnr > 25) && (s_stDb3383SignalStatus[nChannelId].tsmfStatus == 2) )
		{
			GoodRetry[nChannelId] = 0;
			CH_DI_Print(CH_LOG_STATUS, ("### BadRetry[%d] = %d\n",nChannelId,BadRetry[nChannelId]));
			if ( BadRetry[nChannelId] > 4)
			{
				sTsmfGoodFlag[nChannelId] = TRUE;

				if((sTsmfBadFlag[nChannelId] == FALSE) || (beforeId1[nChannelId] != gCurrentId[nChannelId]))
				{
					sTsmfBadFlag[nChannelId] = TRUE;
					beforeId1[nChannelId] = gCurrentId[nChannelId];

					CH_DI_Print (CH_LOG_DEFAULT,("DB3383_CheckLock[%d]:: Send TSMF Bad Message .....\n\n",nChannelId));

					//GWM_SendMessage(MSG_CH_TSMF_BAD, versionNumber, CH_MOD_QAM, 0);
					DRV_CH_CallNotifyCallback( nChannelId, unRequestId, DI_CH_SIGNAL_TSMF_BAD);

					BadRetry[nChannelId] = 0;
				}
			}
			else
			{
				BadRetry[nChannelId]++;
			}
		}
		else
		{
			BadRetry[nChannelId] = 0;
			CH_DI_Print (CH_LOG_STATUS, ("### GoodRetry[%d] = %d\n",nChannelId,GoodRetry[nChannelId]));

			if(GoodRetry[nChannelId] > 4)
			{
				sTsmfBadFlag[nChannelId] = FALSE;

				if((sTsmfGoodFlag[nChannelId] == TRUE) || (beforeId1[nChannelId] != gCurrentId[nChannelId]))
				{
					sTsmfGoodFlag[nChannelId] = FALSE;
					beforeId1[nChannelId] = gCurrentId[nChannelId];

					CH_DI_Print (CH_LOG_DEFAULT, ("DB3383_CheckLock[%d]:: Send TSMF Good Message .....\n\n",nChannelId));

					//GWM_SendMessage(MSG_CH_TSMF_GOOD, versionNumber, CH_MOD_QAM, 0);
					DRV_CH_CallNotifyCallback( nChannelId, unRequestId, DI_CH_SIGNAL_TSMF_GOOD);

					GoodRetry[nChannelId] = 0;
				}
			}
			else
			{
				GoodRetry[nChannelId]++;
			}
		}

		if( s_stDb3383SignalStatus[nChannelId].emergencyFlag == 1)
		{
			sTsmfEmgOffFlag[nChannelId] = TRUE;
			if((sTsmfEmgOnFlag[nChannelId] == FALSE) || (beforeId2[nChannelId] != gCurrentId[nChannelId]) )
			{
				sTsmfEmgOnFlag[nChannelId] = TRUE;
				beforeId2[nChannelId] = gCurrentId[nChannelId];

				CH_DI_Print (CH_LOG_DEFAULT, ("DB3383_CheckLock[%d]:: TSMF Emergency On ..... \n\n",nChannelId));

				//GWM_SendMessage(MSG_CH_EWS_ON, versionNumber, CH_MOD_QAM, 0);
				DRV_CH_CallNotifyCallback( nChannelId, unRequestId, DI_CH_SIGNAL_TSMF_EWS_ON);
			}
		}
		else
		{
			sTsmfEmgOnFlag[nChannelId] = FALSE;
			if((sTsmfEmgOffFlag[nChannelId] == TRUE) || (beforeId2[nChannelId] != gCurrentId[nChannelId]) )
			{
				sTsmfEmgOffFlag[nChannelId] = FALSE;
				beforeId2[nChannelId] = gCurrentId[nChannelId];

				CH_DI_Print (CH_LOG_DEFAULT, ("DB3383_CheckLock[%d]:: TSMF Emergency Off .....\n\n",nChannelId));

				//GWM_SendMessage(MSG_CH_EWS_OFF, versionNumber, CH_MOD_QAM, 0);
				DRV_CH_CallNotifyCallback( nChannelId, unRequestId, DI_CH_SIGNAL_TSMF_EWS_OFF);
			}
		}
#endif

		return TRUE;
	}
	else
	{
		s_stDb3383SignalStatus[nChannelId].nStrength = 0;
		s_stDb3383SignalStatus[nChannelId].ulQuality = 0;
		s_stDb3383SignalStatus[nChannelId].ucAgc = 0;		// Ignore

		s_stDb3383SignalStatus[nChannelId].ulBer = 0;
		s_stDb3383SignalStatus[nChannelId].ulSnr = 0;
		s_stDb3383SignalStatus[nChannelId].ucRfAgc = 0;
		s_stDb3383SignalStatus[nChannelId].ucIfAgc = 0;
		s_stDb3383SignalStatus[nChannelId].ulCorrectedNo = 0;
		s_stDb3383SignalStatus[nChannelId].ulUnCorrectedNo = 0;
		s_stDb3383SignalStatus[nChannelId].ulUnErroredNo = 0;
		s_stDb3383SignalStatus[nChannelId].fSignalPower = 0;
		s_ucIsDb3383Lock[nUnitId] = FALSE;

		return FALSE;
	}
}

int  DB3383_GetStrengthQuality(int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo)
{
	if(s_ucIsDb3383Lock[nChannelId])
	{
#if defined(CONFIG_PRODUCT_IMAGE_FACTORY)
		DB3383_CheckLock(nChannelId);
#endif

		pstSignalInfo->ulStrength 			= s_stDb3383SignalStatus[nChannelId].nStrength;
		pstSignalInfo->ulQuality 			= s_stDb3383SignalStatus[nChannelId].ulQuality;
		pstSignalInfo->ulAgc 				= s_stDb3383SignalStatus[nChannelId].ucAgc;

		pstSignalInfo->fBer 				= s_stDb3383SignalStatus[nChannelId].ulBer / 1000000000000.0;
		pstSignalInfo->fSnr 				= s_stDb3383SignalStatus[nChannelId].ulSnr / 100.0;


		pstSignalInfo->ulRfAgc 				= s_stDb3383SignalStatus[nChannelId].ucRfAgc;
		pstSignalInfo->ulIfAgc 				= s_stDb3383SignalStatus[nChannelId].ucIfAgc;

		pstSignalInfo->ulCorrectedNo 		= s_stDb3383SignalStatus[nChannelId].ulCorrectedNo;
		pstSignalInfo->ulUnCorrectedNo 		= s_stDb3383SignalStatus[nChannelId].ulUnCorrectedNo;
		pstSignalInfo->ulUnErroredNo 		= s_stDb3383SignalStatus[nChannelId].ulUnErroredNo;
		pstSignalInfo->fSignalInputPower		= s_stDb3383SignalStatus[nChannelId].fSignalPower;

		CH_DI_Print(CH_LOG_STATUS,("DB3383_GetStrengthQuality[%d] : Locked \n",nChannelId ));
		CH_DI_Print(CH_LOG_STATUS,("=================== DB3383_GetStrengthQuality[%d] ===================\n",nChannelId));
		CH_DI_Print(CH_LOG_STATUS,("  ulStrength               = %d\n", pstSignalInfo->ulStrength));
		CH_DI_Print(CH_LOG_STATUS,("  ulQuality                = %d\n", pstSignalInfo->ulQuality));
		CH_DI_Print(CH_LOG_STATUS,("  fBer                     = %f\n", pstSignalInfo->fBer));
		CH_DI_Print(CH_LOG_STATUS,("  fSnr                     = %3.1f\n", pstSignalInfo->fSnr));
		CH_DI_Print(CH_LOG_STATUS,("  Signal Power [dBuV]      = %4.1f\n", pstSignalInfo->fSignalInputPower));
		CH_DI_Print(CH_LOG_STATUS,("  ulIfAgc                  = %u\n", pstSignalInfo->ulIfAgc));
		CH_DI_Print(CH_LOG_STATUS,("  ulCorrectedNo            = %u\n", pstSignalInfo->ulCorrectedNo));
		CH_DI_Print(CH_LOG_STATUS,("  ulUnCorrectedNo          = %u\n", pstSignalInfo->ulUnCorrectedNo));
		CH_DI_Print(CH_LOG_STATUS,("  ulUnErroredNo            = %u\n", pstSignalInfo->ulUnErroredNo));
		CH_DI_Print(CH_LOG_STATUS,("======================================================================\n"));
	}
	else
	{
		pstSignalInfo->ulStrength			= 0;
		pstSignalInfo->ulQuality			= 0;
		pstSignalInfo->ulAgc				= 0;		// Ignore
		pstSignalInfo->fBer					= 0;
		pstSignalInfo->fSnr					= 0;
		pstSignalInfo->ulCorrectedNo		= 0;
		pstSignalInfo->ulUnCorrectedNo		= 0;
		pstSignalInfo->ulUnErroredNo		= 0;

		CH_DI_Print(CH_LOG_STATUS,("DB3383_GetStrengthQuality[%d] : Unlocked \n",nChannelId));
	}

	return DI_CH_OK;
}


void DB3383_DumpRegister( int nChannelId)
{
	CH_UNUSED(nChannelId);
}


int  DB3383_SetPowerState( int nChannelId, CH_Power_e etPower )
{
	CH_UNUSED(etPower);

	CH_DI_Print(CH_LOG_ARGS, ("[DB3383_SetPowerState[%d]] ----\n",nChannelId));

	return DI_CH_OK;
}


int DB3383_EnableTsOutput ( int nChannelId )
{
	CH_DI_Print(CH_LOG_ARGS, ("[DB3383_EnableTsOutput[%d]] ----\n",nChannelId));

	CH_UNUSED(nChannelId);

	return DI_CH_OK;
}


int DB3383_DisableTsOutput ( int nChannelId )
{
	CH_DI_Print(CH_LOG_ARGS, ("[DB3383_DisableTsOutput[%d]] ----\n",nChannelId));

#if defined (CONFIG_DEMOD_TSMF)
	beforeId1[nChannelId] = 0;
	beforeId2[nChannelId] = 0;
	gCurrentId[nChannelId] = 0;

	int nUnitId;
	NEXUS_TsmfSettings tsmfSettings;
	nUnitId = GetDeviceUnitId( nChannelId );

#if (NEXUS_VERSION >= 1290)
	tsmfSettings.sourceType = NEXUS_TsmfSourceType_eMtsif;
	tsmfSettings.sourceTypeSettings.mtsif = NEXUS_Frontend_GetConnector(s_st3383Frontend[nUnitId]);
	tsmfSettings.relativeTsNum = 0;
	tsmfSettings.semiAutomaticMode = false;
	tsmfSettings.parserforcesetting = true;
	tsmfSettings.enabled=false;

	NEXUS_Tsmf_SetSettings(s_st3383Tsmf[nUnitId], &tsmfSettings);

	NEXUS_Frontend_ReapplyTSMFSettings(s_st3383Frontend[nUnitId]);
#else
	NEXUS_Frontend_GetTsmfSettings(s_st3383Frontend[nUnitId], &tsmfSettings);
	CH_DI_Print(CH_LOG_ARGS,("DB3383_DisableTsOutput[%d] :: semiAutomaticMode[%d], enabled[%d] \n",nUnitId, tsmfSettings.semiAutomaticMode, tsmfSettings.enabled ));

	tsmfSettings.relativeTsNum = 0;
	tsmfSettings.semiAutomaticMode = false;
	tsmfSettings.enabled = false;
	NEXUS_Frontend_SetTsmfSettings(s_st3383Frontend[nUnitId], &tsmfSettings);

	NEXUS_Frontend_GetTsmfSettings(s_st3383Frontend[nUnitId], &tsmfSettings);
	CH_DI_Print(CH_LOG_ARGS,("DB3383_DisableTsOutput[%d] :: semiAutomaticMode[%d], enabled[%d] \n",nUnitId, tsmfSettings.semiAutomaticMode, tsmfSettings.enabled ));

	tsmfSettings.relativeTsNum = 0;
	tsmfSettings.semiAutomaticMode = false;
	tsmfSettings.parserforcesetting = true;
	NEXUS_Frontend_SetTsmfSettings(s_st3383Frontend[nUnitId], &tsmfSettings);
#endif

#if (NEXUS_VERSION >= 1404)
	NEXUS_Frontend_SetInputBandEnable(s_st3383Frontend[nUnitId], false);
#endif
#else
#if (NEXUS_VERSION >= 1404)
	int nUnitId;
	nUnitId = GetDeviceUnitId( nChannelId );

	NEXUS_Frontend_SetInputBandEnable(s_st3383Frontend[nUnitId], false);
#endif
#endif

	return DI_CH_OK;
}


#if defined (CONFIG_DEMOD_TSMF)
void DB3383_TsmfData(int nChannelId, HUINT16 *tsNo, HINT32 *tsID, HINT32 *netID)
{
	int i = 0, j=0;

	*tsNo = gDB3383TsmfInfo[nChannelId].tsNo;
	 CH_DI_Print(CH_LOG_TSINFO,("DB3383_TsmfData[%d] : tsNo[%d]\n", nChannelId,  gDB3383TsmfInfo[nChannelId].tsNo));

	if (gDB3383TsmfInfo[nChannelId].tsNo == 0)
	{
		for(i = 0; i < 15; i++)
		{
			tsID[i] = 0;
			netID[i] = 0;
		}

	}
	else
	{
		for(i = 0; i < 15; i++)
		{

			if ( !((gDB3383TsmfInfo[nChannelId].tsID[i] == 0xffff) &&  (gDB3383TsmfInfo[nChannelId].netID[i] == 0xffff)))
			{
				tsID[j] = gDB3383TsmfInfo[nChannelId].tsID[i];
				netID[j] = gDB3383TsmfInfo[nChannelId].netID[i];
				j++;
			}
		}
	}
}


void DB3383_GetPathInfo(int nChannelId)
{
	HINT32 nUnitId = 0;
	nUnitId = GetDeviceUnitId( nChannelId );

	NEXUS_Frontend_GetPathinfo(s_st3383Frontend[nUnitId]);
}

int DB8883_FindRelativeNumber(int nChannelId, int tsid, int netid)
{
	int relNumber = 0, i=0;

	for (i=0; i< 15; i++)
	{
		if ( (tsid == gDB3383TsmfInfo[nChannelId].tsID[i]) && ( netid == gDB3383TsmfInfo[nChannelId].netID[i]) )
		{
			relNumber = i+1;
			break;
		}
	}

	return relNumber;
}
#endif

