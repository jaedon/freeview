/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_ch.h
	@brief	  MW�� channel service ��� ����

	Description: ����/������/���̺� ���Ͽ� ä�� locking�� �����ϴ� channel service�� ��� ����
	Module: MW/CH			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	_SVC_CH_H_
#define	_SVC_CH_H_

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
//#include <pal_pipe.h>
#include <svc_resources.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define SVC_CH_MAX_RFINPUT_NUM			8
#define SVC_CH_MAX_TUNER_NUM				8


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
enum
{
	eSEVT_CH_ = eSEVT_CH_START,

	/* - Description : Locking ��û�� �����Ǿ���.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_CH_STARTED,

	/* - Description : Stop�� ��û�� action���� session�� stop�Ǿ����� �˸��� �̺�Ʈ �޽���.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_CH_STOPPED,

	/* - Description : Locking�� ��û�� action���� lock�� �Ǿ����� �˸��� �̺�Ʈ �޽���.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_CH_LOCKED,

	/* - Description : Locking�� ��û�� action���� signal�� ������ �˸��� �̺�Ʈ �޽���.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_CH_NO_SIGNAL,

	/* - Description : Request Lock �� ������ �ٸ� ������ Tuning �Ǿ����� �˸��� �̺�Ʈ �޽���
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_CH_RELOCKED_BY_AUTO,

	/* - Description : Antenna shortage in LNB or terrestrial antenna.
	   - Parameters Usage :
		   handle : N.A.
		   p1 : DxDeliveryType_e - Indicate SAT or TER antenna
		   p2 : TBD
		   p3 : TBD */
	eSEVT_CH_ANTENNA_FAULT,

	/* - Description : Antenna shortage in LNB or terrestrial antenna was resolved.
	   - Parameters Usage :
		   handle : N.A.
		   p1 : DxDeliveryType_e - Indicate SAT or TER antenna
		   p2 : TBD
		   p3 : TBD */
	eSEVT_CH_ANTENNA_OK,

	/* - Description : ���Ͱ� �����̰� ������ �˸��� �̺�Ʈ �޽���.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_CH_MOVING_MOTOR,

	/* - Description : ���� ���� Command�� �Ϸ������� �˸��� �̺�Ʈ �޽���
	   - Parameters Usage :
		   handle : Action handle
		   p1 : �ش� Ŀ�ǵ�
		   p2 : TBD
		   p3 : TBD */
	eSEVT_CH_MOTOR_CMD_COMPLETED,

	eSEVT_CH_END_
};

#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define ___________SATELLITE_Tuning_Params____________
#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
typedef	union
{
	DxLnbAntenna_t				lnb;
	DxDiseqcAntenna_t			diseqc;
	DxScdAntenna_t				scd;
} CH_AntInfo_t;

typedef	struct
{
	// MOON-BUILDER
#if (1)
	HUINT32							ulAntId;					/**< antenna id */
#endif

	/* DVBS Parameters */
	HUINT32							ulFrequency;			/**< Tuning Frequency, MHz(Unit) */
	HUINT32							ulSymbolRate;			/**< Symbol Rate, KSymbol/sec */
	DxSat_CodeRate_e					fecCodeRate;			/**< FEC Code Rate */
	DxSat_Polarization_e				polarization;			/**< Polarity : Vertical/Horizontal*/

	/* DVBS2 Parameters */
	DxSat_TransportSpec_e				transSpec;				/**< DVBS, DVBS2 */
	DxSat_PskModulation_e 			pskMod;					/**< Modulation Type : QPSK, 8PSK, AUTO*/
	DxSat_Pilot_e						pilot;					/**< Pilot On/Off/Auto */
	DxSat_RollOff_e					rollOff;				/**< Roll off : 0.20, 0.25, 0.35 */
} SAT_TuningTPInfo_t;

typedef	struct
{
	HINT32		nSatId;
	HINT32		nPosNum;		/**< predefined satellite diseqc position in positioner */
	HINT16		sSatDegree;		/**< satellite's degree, ex) 120.5E : 1205, 55.0W : -550 ==> East is a positive value, West a negetive */
	HUINT8		*szLongitude;   /**< sSatDegree's text */
} SAT_SatPosInfo_t;


#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define ___________Terrestrial_Tuning_Params____________
#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

typedef struct
{
	HUINT32							ulPlpId;
	DxTer_PreambleFormat_e			preambleFormat;
	DxTer_Mixed_e						mixed;
	DxTer_Pilot_Pattern_e				pilotPattern;
	DxTer_ExtendedCarrier_e			extenedCarrier;
	DxTer_PAPR_Reduction_e			PAPRreduction;
	HUINT32							numPlpBlocks;
}TER_T2_Param_t;

#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define ___________Analog_Tuning_Params____________
#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define ___________Cable_Tuning_Params____________
#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#define TSMF_ALL_TS_ID		(-1)
#define TSMF_ALL_NET_ID		(-1)

/* Japanese JCL channel spec. for multi TS. */
typedef struct
{
	HUINT32							ulTsNum;
	HUINT16							usTsId[15];
	HUINT16							usNetId[15];
} CAB_TsmfInfo_t;

#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define ___________Common_Params____________
#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

typedef struct
{
	HUINT32			signalLevel;		/**< Percent ȯ�� �� */
	HUINT32			signalQuality;		/**< Percent ȯ�� �� */
	HUINT32			ulAgc;				/**< AGC */
	float			fBer;				/**< BER */
	float			fSnr;				/**< SNR */
	float			fSignalInputPower;	/**< dBuV */
} CH_SignalInfo_t;


#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define ___________SATELLITE_RF_INPUT____________
#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

typedef struct
{
	HBOOL						 bAvailable;

	// RF-Input Property

	// TP Tuning State:
	HBOOL						 bTpAllocated;
	DxTuneParam_t				 stAllocatedTpInfo;

	// Connectable Tuner:
	HUINT32						 ulConnectableTunerNum;
	HUINT32						 aulConnectableTunerId[SVC_CH_MAX_TUNER_NUM];

	// Connected Tuner:
	HUINT32						 ulConnectedTunerNum;
	HUINT32						 aulConnectedTunerId[SVC_CH_MAX_TUNER_NUM];
} CH_SatRfInput_t;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

/**
  @remark   Channel service �ʱ�ȭ API.
  @remark	type : Inline

  @return	NONE
*/
void SVC_CH_Init(PostMsgToMgrCb_t fnPostMsgToMgr);

/**
  @remark
  @remark	type : Async

  @return	ERR_OK\n
  			ERR_FAIL : Invalid actionId.
*/
extern HERROR SVC_CH_StartTune(
						Handle_t			hAction,		/**< [in] Action handle */
						SvcRsc_Info_t			*pstRscInfo,
						HUINT16 			usUniqueId, 	/**< [in] DB_SVC�� TS info�� �ִ� unique ID. DB_SVC���� �� ������ �ƴϸ� (HUINT16)-1�� ����. */
						DxTuneParam_t		*tuningParam	/**< [in] Tuning parameter struct pointer */
						);

/**
  @remark
  @remark	type : Async/Sync

  @return	ERR_OK\n
  			ERR_FAIL : Invalid actionId.
*/
extern HERROR SVC_CH_StopTune(
						Handle_t			hAction,		/**< [in] Action handle */
						HUINT16				usNextUniqueId,	/**< [in] ������ start�� TS�� unique ID ����. ä�� ���񽺿����� ����� unique Id��
																	���� Ʃ������ unique ID�� ������ TS�� ���� �ʰ� �����Ѵ�.(locking �ӵ� ����)
																	-1�� �ָ� ������ TS�� ���´�. �������� �������� (HUINT16)-1�� �����϶�. */
						ApiSyncMode_t		syncMode		/**< [in] API synchronization mode */
						);

/**
  @remark	Tuner resource �� ������.
  @remark	type : Async/Sync

  @return	ERR_OK\n
  			ERR_FAIL : Invalid actionId.
*/
extern	HERROR	SVC_CH_GetNumberOfTunerGroup(HUINT32 *pulTunerNumber);

/**
  @remark	�־��� id�� ���� Tuner Type�� �˷���.
  @remark	type : Async/Sync

  @return	ERR_OK\n
  			ERR_FAIL : Invalid actionId.
*/
extern	HERROR	SVC_CH_GetTunerType(HUINT32 nTunerId, HUINT32 *pulTunerType);

/**
  @remark	���� locking�� ������ �о�� API
  @remark	type : Inline

  @return	ERR_OK\n
  			ERR_FAIL : Invalid actionId �Ǵ� no signal ������.
*/
extern HERROR	SVC_CH_GetLockedInfo(
						HUINT32				ulActionId,			/**< [in] Action ID */
						DxTuneParam_t		*lockedTuningInfo	/**< [out] ������ action�� channel status�� ������ ������ */
						);

/**
  @remark	Tuner Group ID�� ���� locking�� ������ �о�� API
  @remark	type : Inline

  @return	ERR_OK\n
  			ERR_FAIL : Invalid actionId �Ǵ� no signal ������.
*/
extern HERROR	SVC_CH_GetLockedInfoByTunerGroupID(
						HUINT32				ulTunerGroupId,			/**< [in] Tuner Group ID */
						DxDeliveryType_e	eDeliveryType,			/**< [in] Delivery Type */
						DxTuneParam_t		*lockedTuningInfo		/**< [out] ������ action�� channel status�� ������ ������ */
						);

/**
  @remark	��ȣ ����/ǰ������ ������ �о�� API
  @remark	type : Inline

  @return	ERR_OK\n
  			ERR_FAIL : Invalid actionId �Ǵ� no signal ������.
*/
extern HERROR SVC_CH_GetSignalInfo(
						HUINT32 			ulActionId, 		/**< [in] Action ID */
						DxDeliveryType_e	*deliveryType,		/**< [out] ������ action�� channel delivery type�� ������ ������ */
						CH_SignalInfo_t		*signalInfo			/**< [out] ������ action�� signal ������ ������ ������ */
						);

/**
  @remark	Tuner Group ID ��ȣ ����/ǰ������ ������ �о�� API
  @remark	type : Inline

  @return	ERR_OK\n
  			ERR_FAIL : Invalid actionId �Ǵ� no signal ������.
*/
extern HERROR SVC_CH_GetSignalInfoByTunerGroupID(
						HUINT32		 		ulTunerGroupId, 	/**< [in] Tuner Group ID */
						DxDeliveryType_e	*deliveryType,		/**< [out] ������ action�� channel delivery type�� ������ ������ */
						CH_SignalInfo_t		*signalInfo			/**< [out] ������ action�� signal ������ ������ ������ */
						);

/**
  @remark	������ antenna power set \n
  @remark   API Type : Inline \n

  @return   ERR_OK   : ����\n
  			ERR_FAIL : ����
*/

void 		SVC_CH_SetTERAntennaPower(HBOOL bOnOff);

/**
  @remark	������ antenna power Get \n
  @remark   API Type : Inline \n

  @return   ERR_OK   : ����\n
  			ERR_FAIL : ����
*/

HERROR 	SVC_CH_GetTERAntennaPower(HBOOL *pbOnOff);

/**
  @remark	������ ������ ������ ��� ������. \n
  @remark   API Type : Inline \n

  @return   ERR_OK   : ����\n
  			ERR_FAIL : ����
*/
HERROR	SVC_CH_GetSatPositionInfo(HINT32 nSatId, SAT_SatPosInfo_t *pstSatPosInfo);

/**
  @remark	[diseqc ver 1.2] �̸� ���ǵ� ������ �ѹ��� �о���� API \n
  @remark   API Type : Inline \n

  @return   ERR_OK   : ����\n
  			ERR_FAIL : ����
*/
HERROR	SVC_CH_GetSatDiseqcPos(HINT32 nSatId, HINT32 *pnPosNum);

/**
  @remark	������ ������ ������. motor�� goto satellite input���� ����. \n
  @remark   API Type : Inline \n

  @return   ERR_OK   : ����\n
  			ERR_FAIL : ����
*/
HERROR	SVC_CH_GetSatDegree(HINT32 nSatId, HINT16 *psSatDegree);

/**
  @remark	������ Antenna ���� ���¸� ������. \n
  @remark   API Type : Inline \n

  @return   ERR_OK   : ����\n
  			ERR_FAIL : ����
*/
HERROR SVC_CH_SetAntennaType(DxAntennaConnection_e eAntType);

/**
  @remark   ������ Tuning Information���� ���� loop through ���·� Tuning �������� �˷��ִ� API
  @remark	type : Inline

  @return	ERR_OK : Tuning ������. \n
  			ERR_FAIL : Tuning �Ұ�.
*/
extern HERROR SVC_CH_CheckValidLoopThrough(
							DxTuneParam_t *pstTuningInfo1,	/**< [in]  tuning parameter 1 */
							DxTuneParam_t *pstTuningInfo2		/**< [in]  tuning parameter 2 */
							);
/**
  @remark   �ش� TP �ΰ��� ���� RF-Input���� ���� �� �ִ��� Ȯ��.
  @remark	type : Inline

  @return	ERR_OK : Tuning ������. \n
  			ERR_FAIL : Tuning �Ұ�.
*/
extern HERROR SVC_CH_CheckTpConflict (DxTuneParam_t *pstTuningInfo1, DxTuneParam_t *pstTuningInfo2);
HERROR SVC_CH_MOTOR_StopMotor (Handle_t hAction, ApiSyncMode_t syncMode);
HERROR SVC_CH_MOTOR_GotoPosition (Handle_t hAction, HUINT8 ucPosition, ApiSyncMode_t syncMode);
HERROR SVC_CH_MOTOR_StorePosition (Handle_t hAction, HUINT8 ucPosition, ApiSyncMode_t syncMode);
HERROR SVC_CH_MOTOR_Drive (Handle_t hAction, CH_MotorDirection_t direction, CH_MotorDriveMode_t driveMode, HUINT8 ucValue, ApiSyncMode_t syncMode);
HERROR SVC_CH_MOTOR_SetLimit (Handle_t hAction, CH_MotorDirection_t direction, ApiSyncMode_t syncMode);
HERROR SVC_CH_MOTOR_DisableLimit (Handle_t hAction, ApiSyncMode_t syncMode);
HERROR SVC_CH_MOTOR_EnableLimit (Handle_t hAction, ApiSyncMode_t syncMode);
HERROR SVC_CH_MOTOR_Recalculate (Handle_t hAction, HUINT8 ucPosition, ApiSyncMode_t syncMode);
HERROR SVC_CH_MOTOR_GotoDegree (Handle_t hAction, HINT16 sDegree, ApiSyncMode_t syncMode);
HERROR SVC_CH_MOTOR_GotoSatellite (Handle_t hAction, HINT16 sSatPos, ApiSyncMode_t syncMode);
HERROR SVC_CH_ResetDiseqc (Handle_t hAction, ApiSyncMode_t syncMode);
#if defined(CONFIG_OTA_IRDETO)
HERROR SVC_CH_MakeDiTuningDataByChTuningParam (DxTuneParam_t *pstChTuneParam, HUINT32 *pulDiDataSize, void **ppvDiParam);
HERROR SVC_CH_FreeDiTuningData (void *pvData);
#endif
HERROR SVC_CH_RegisterAutoSRList(HUINT32 ulSymbolRateNum, HUINT32 *aulSymbolRateList);
HERROR SVC_CH_GetRequestedTuneInfo(HUINT32 ulTunerGroup, DxTuneParam_t* pstTuneInfo);
HERROR SVC_CH_IsIdleTuner (HUINT32 ulTunerGroupId,HBOOL *pbIsIdle);

#if	defined(CONFIG_MW_CH_DVB_T2)
HERROR SVC_CH_GetDataPLPInfo (HUINT32 ulActionId,  HUINT8 *pucPlpIds, HUINT8 *pucNumPlps );
HERROR SVC_CH_GetPLPCount (HUINT32 ulActionId,   HUINT8 *pucNumPlps );
#endif

// Satellite RF-Input
HUINT32		SVC_CH_CountSatelliteRfInput (void);
HERROR		SVC_CH_GetSatelliteRfInput (HUINT32 ulRfInputIdx, CH_SatRfInput_t *pstRfInput);

#endif /* _SVC_CH_H_ */


