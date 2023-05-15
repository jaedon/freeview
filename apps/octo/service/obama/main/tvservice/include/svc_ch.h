/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_ch.h
	@brief	  MW의 channel service 헤더 파일

	Description: 위성/지상파/케이블에 대하여 채널 locking을 제공하는 channel service의 헤더 파일
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

	/* - Description : Locking 요청이 접수되었음.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_CH_STARTED,

	/* - Description : Stop을 요청한 action에게 session이 stop되었음을 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_CH_STOPPED,

	/* - Description : Locking을 요청한 action에게 lock이 되었음을 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_CH_LOCKED,

	/* - Description : Locking을 요청한 action에게 signal이 없음을 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_CH_NO_SIGNAL,

	/* - Description : Request Lock 한 정보와 다른 값으로 Tuning 되었음을 알리는 이벤트 메시지
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

	/* - Description : 모터가 움직이고 있음을 알리는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_CH_MOVING_MOTOR,

	/* - Description : 모터 관련 Command가 완료했음을 알리는 이벤트 메시지
	   - Parameters Usage :
		   handle : Action handle
		   p1 : 해당 커맨드
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
	HUINT32			signalLevel;		/**< Percent 환산 값 */
	HUINT32			signalQuality;		/**< Percent 환산 값 */
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
  @remark   Channel service 초기화 API.
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
						HUINT16 			usUniqueId, 	/**< [in] DB_SVC의 TS info에 있는 unique ID. DB_SVC에서 온 정보가 아니면 (HUINT16)-1로 지정. */
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
						HUINT16				usNextUniqueId,	/**< [in] 다음에 start할 TS의 unique ID 예고. 채널 서비스에서는 예고된 unique Id와
																	현재 튜닝중인 unique ID가 같으면 TS를 끊지 않고 유지한다.(locking 속도 개선)
																	-1을 주면 무조건 TS를 끊는다. 예고하지 않으려면 (HUINT16)-1을 지정하라. */
						ApiSyncMode_t		syncMode		/**< [in] API synchronization mode */
						);

/**
  @remark	Tuner resource 를 돌려줌.
  @remark	type : Async/Sync

  @return	ERR_OK\n
  			ERR_FAIL : Invalid actionId.
*/
extern	HERROR	SVC_CH_GetNumberOfTunerGroup(HUINT32 *pulTunerNumber);

/**
  @remark	주어진 id에 대한 Tuner Type를 알려줌.
  @remark	type : Async/Sync

  @return	ERR_OK\n
  			ERR_FAIL : Invalid actionId.
*/
extern	HERROR	SVC_CH_GetTunerType(HUINT32 nTunerId, HUINT32 *pulTunerType);

/**
  @remark	실제 locking된 정보를 읽어가는 API
  @remark	type : Inline

  @return	ERR_OK\n
  			ERR_FAIL : Invalid actionId 또는 no signal 상태임.
*/
extern HERROR	SVC_CH_GetLockedInfo(
						HUINT32				ulActionId,			/**< [in] Action ID */
						DxTuneParam_t		*lockedTuningInfo	/**< [out] 지정된 action의 channel status를 복사할 포인터 */
						);

/**
  @remark	Tuner Group ID에 실제 locking된 정보를 읽어가는 API
  @remark	type : Inline

  @return	ERR_OK\n
  			ERR_FAIL : Invalid actionId 또는 no signal 상태임.
*/
extern HERROR	SVC_CH_GetLockedInfoByTunerGroupID(
						HUINT32				ulTunerGroupId,			/**< [in] Tuner Group ID */
						DxDeliveryType_e	eDeliveryType,			/**< [in] Delivery Type */
						DxTuneParam_t		*lockedTuningInfo		/**< [out] 지정된 action의 channel status를 복사할 포인터 */
						);

/**
  @remark	신호 레벨/품질등의 정보를 읽어가는 API
  @remark	type : Inline

  @return	ERR_OK\n
  			ERR_FAIL : Invalid actionId 또는 no signal 상태임.
*/
extern HERROR SVC_CH_GetSignalInfo(
						HUINT32 			ulActionId, 		/**< [in] Action ID */
						DxDeliveryType_e	*deliveryType,		/**< [out] 지정된 action의 channel delivery type을 복사할 포인터 */
						CH_SignalInfo_t		*signalInfo			/**< [out] 지정된 action의 signal 정보를 복사할 포인터 */
						);

/**
  @remark	Tuner Group ID 신호 레벨/품질등의 정보를 읽어가는 API
  @remark	type : Inline

  @return	ERR_OK\n
  			ERR_FAIL : Invalid actionId 또는 no signal 상태임.
*/
extern HERROR SVC_CH_GetSignalInfoByTunerGroupID(
						HUINT32		 		ulTunerGroupId, 	/**< [in] Tuner Group ID */
						DxDeliveryType_e	*deliveryType,		/**< [out] 지정된 action의 channel delivery type을 복사할 포인터 */
						CH_SignalInfo_t		*signalInfo			/**< [out] 지정된 action의 signal 정보를 복사할 포인터 */
						);

/**
  @remark	지상파 antenna power set \n
  @remark   API Type : Inline \n

  @return   ERR_OK   : 성공\n
  			ERR_FAIL : 실패
*/

void 		SVC_CH_SetTERAntennaPower(HBOOL bOnOff);

/**
  @remark	지상파 antenna power Get \n
  @remark   API Type : Inline \n

  @return   ERR_OK   : 성공\n
  			ERR_FAIL : 실패
*/

HERROR 	SVC_CH_GetTERAntennaPower(HBOOL *pbOnOff);

/**
  @remark	위성의 포지션 정보를 모두 가져옴. \n
  @remark   API Type : Inline \n

  @return   ERR_OK   : 성공\n
  			ERR_FAIL : 실패
*/
HERROR	SVC_CH_GetSatPositionInfo(HINT32 nSatId, SAT_SatPosInfo_t *pstSatPosInfo);

/**
  @remark	[diseqc ver 1.2] 미리 정의된 포지션 넘버를 읽어오는 API \n
  @remark   API Type : Inline \n

  @return   ERR_OK   : 성공\n
  			ERR_FAIL : 실패
*/
HERROR	SVC_CH_GetSatDiseqcPos(HINT32 nSatId, HINT32 *pnPosNum);

/**
  @remark	위성의 각도를 가져옴. motor의 goto satellite input으로 사용됨. \n
  @remark   API Type : Inline \n

  @return   ERR_OK   : 성공\n
  			ERR_FAIL : 실패
*/
HERROR	SVC_CH_GetSatDegree(HINT32 nSatId, HINT16 *psSatDegree);

/**
  @remark	위성의 Antenna 연결 상태를 지정함. \n
  @remark   API Type : Inline \n

  @return   ERR_OK   : 성공\n
  			ERR_FAIL : 실패
*/
HERROR SVC_CH_SetAntennaType(DxAntennaConnection_e eAntType);

/**
  @remark   지정된 Tuning Information으로 서로 loop through 상태로 Tuning 가능한지 알려주는 API
  @remark	type : Inline

  @return	ERR_OK : Tuning 가능함. \n
  			ERR_FAIL : Tuning 불가.
*/
extern HERROR SVC_CH_CheckValidLoopThrough(
							DxTuneParam_t *pstTuningInfo1,	/**< [in]  tuning parameter 1 */
							DxTuneParam_t *pstTuningInfo2		/**< [in]  tuning parameter 2 */
							);
/**
  @remark   해당 TP 두개가 같은 RF-Input으로 들어올 수 있는지 확인.
  @remark	type : Inline

  @return	ERR_OK : Tuning 가능함. \n
  			ERR_FAIL : Tuning 불가.
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


