/**
	@file     ap_actionmanager.c
	@brief    Action application들의 creation/destroy 를 관리하고 리소스를 할당하는 관리 application.

	Description: WatchTV, Recording, TSR, search 등 action application들을 시나리오와 리소스 사용 상황에 따라\n
				생성 시키고 없애기도 한다. Action application이 리소스를 할당 받는 API를 제공한다.
	Module: AP/AM			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <db_param.h>
#include <db_svc.h>

#include <pal_pipe.h>
//#include <mwc_util.h>
#include <svc_resources.h>

#include <svc_ch.h>

#include <pal_channel.h>
#include <pal_pipe.h>

#include <get_enum_str.h>

#if defined(CONFIG_MW_LOGCAPTURE)
#include <svc_logcapture.h>
#endif

#include <otl.h>

//#define CONFIG_MW_CH_AUTOMATIC_RETUNE			1

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



#define MOTOR_MOVING_TIMEOUT					(30*1000)	/* 30 sec */

#if defined(CONFIG_MW_CH_CAB_AUTO_SR)
// 개수가 너무 많으면 Unlock Message가 늦게 올라온다.. 증가시키지 말것...
#define	MAX_AUTO_SR_NUM			8
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eSTATE_IDLE,
	eSTATE_LOCKING,
	eSTATE_MOTOR_MOVING,
	eSTATE_LOCKED,
	eSTATE_NO_SIGNAL
} ChannelState_t;

/**@brief : Auto Symbol-Rate 구조체 */
#if defined(CONFIG_MW_CH_CAB_AUTO_SR)
typedef enum
{
	eSymbolRateMode_Normal,
	eSymbolRateMode_Auto,
} SymbolRate_Mode_t;

typedef struct
{
	SymbolRate_Mode_t	eSrMode;
	HUINT32				ulCurrIdx;
	ChannelState_t		eChState;
} AUO_SymbolRateContext_t;

typedef struct
{
	HUINT32						ulNumSymbolRate;
	HUINT32						aulSymbolRateList[MAX_AUTO_SR_NUM];
	AUO_SymbolRateContext_t		stContext[NUM_MAX_ACTION];
} AUTO_SymbolRateInfo_t;
#endif

/**@brief : Actin 관리 context 구조체 */
typedef struct
{
	Handle_t			hAction;			/**< 이 action context에 등록된 action handle */
	ChannelState_t		eChState;			/**< 이 action context가 처리한 튜너 state */
	HUINT32				ulTunerGroupId;		/**< 이 action context가 사용하고 있는 tuner ID */
	HUINT32				ulDemuxId;			/**< 이 action context가 사용하고 있는 demux ID */

	SvcRsc_Info_t		stRscInfo;
} CH_ActionContext_t;


/**@brief : Tuner 관리 context 구조체 */
typedef struct
{
	HINT32				supportDeliveryType;	/**< Delivery type의 combination. ChDeliveryType의 OR된 형태.
														(예를 들어 TER|CAB|ANALOG). PAL로부터 초기화 됨. 변경 불가. */

	DxDeliveryType_e	workingDeliveryType;	/**< 현재 context가 working하고 있는 delivery type */
	HUINT16				usUniqueId;				/**< 현재 context가 locking하고 있는 uniqueId */
	HUINT16				usNextUniqueId;				/**< 현재 context가 locking하고 있는 uniqueId */

	ChannelState_t		eChState;					/**< 이 튜너의 locking state */
	DxTuneParam_t		tuningInfo;				/**< 이 튜너로 요청된 튜닝 파라미터 */
	HINT32				nActionCount;			/**< 이 tuner에 걸린 action의 갯수 */

	/* Motor antenna 관련 파라미터 */
	DxSAT_DiseqcVer_e	lastMotorDiseqcVer;		/**< 현재 동작중인 모터 diseqc version */
	HUINT8				ucLastMotorPosition;	/**< 현재 설정된 motor positon for DiSEqC 1.2 */
	HINT16				sLastSatLonggitude; 	/**< 현재 설정된 motor longgitude for DiSEqC 1.3 */
	HUINT32				ulMotorMovingStartTime;		/**< Motor moving time */

	HUINT32				ulRequestId;			/**< PAL event session synchronizing ID */

	HBOOL				bRetuneToAuto;

	HUINT32				ulLastReqTick;
} CH_TunerContext_t;


/**@brief : CMD_CH_LOCK 을 요청하는 메시지 */
typedef struct {
	HUINT32				ulCmd;			/**< Command. union상에서의 공통 위치. */
	Handle_t			hAction;		/**< Command를 발행한 action의 handle */
	HUINT16				usUniqueId;		/**< DB_SVC의 TS info에 있는 unique ID. DB_SVC에서 온 정보가 아니면 (HUINT16)-1로 지정. */
	DxTuneParam_t		tuningInfo;		/**< 위성/지상파/케이블을 union한 튜닝 정보 구조체 */
	SvcRsc_Info_t		stRscInfo;
} CH_LockMsg_t;

/**@brief : CMD_CH_STOP 을 요청하는 메시지 */
typedef struct
{
	HUINT32				ulCmd;			/**< Command. union상에서의 공통 위치. */
	Handle_t			hAction;		/**< Command를 발행한 action의 handle */
	HUINT32				usNextUniqueId;	/**< 다음에 Lock할 TP의 unique ID를 미리 알려줘서 TP가 변경되는 경우에는 stop시에 TS out을 끊거나
											demod stop하도록 하여 이전 TP가 섞이지 않도록 한다. (HUINT16)-1 로 주면 무조건 TS out을 끊는다.*/
	ApiSyncMode_t		syncMode;		/**< Command를 발행한 API의 리턴 형식을 sync/async로 지정 */
} CH_StopMsg_t;

/**@brief : CMD_CH_TUNER_EVENT를 요청하는 메시지 */
typedef struct
{
	HUINT32				ulCmd;			/**< Command. union상에서의 공통 위치. */
	HUINT32		ulTunerGroupId;		/**< Check할 tuner ID */
	HUINT32				ulRequestId;	/**< Request synchronizing ID */
	PalCh_Event_e		event;			/**< Event from tuner instance */
} CH_EventMsg_t;

typedef struct
{
	HUINT32				ulCmd;			/**< Command. union상에서의 공통 위치. */
	Handle_t			hAction;		/**< Command를 발행한 action의 handle */
	HINT32				nArg1, nArg2, nArg3;
	ApiSyncMode_t		syncMode;		/**< Command를 발행한 API의 리턴 형식을 sync/async로 지정 */
} CH_MotorMsg_t;


/**@brief : CMD_CH_* 을 통합한 union. 제일 긴 메시지 type에 맞춰진다. */
typedef union
{
	HUINT32				ulCmd;				/**< Command. union상에서의 공통 위치. */
	CH_LockMsg_t		lockMsg;			/**< 채널 lock 메시지 */
	CH_StopMsg_t		stopMsg;			/**< 채널 stop 메시지 */
	CH_EventMsg_t		eventMsg;			/**< Tuner event 메시지 */
	CH_MotorMsg_t		motorCmdMsg;		/**< Motor command 메시지 */
} CH_Msg_t;

/**@brief : Channel service command */
enum
{
	CMD_CH_LOCK = 100,			/**< Tuner locking을 요청하는 명령 */
	CMD_CH_STOP,				/**< Tuner stop을 요청하는 명령 */
	CMD_CH_TUNER_EVENT,			/**< Tuner에 변경이 발생하였으니 각 action들은 각각 필요한 조치를 하라... */

	// Motor Messages
	CMD_CH_MOTOR_STOP,
	CMD_CH_MOTOR_GOTO_POS,
	CMD_CH_MOTOR_STORE_POS,
	CMD_CH_MOTOR_DRIVE,
	CMD_CH_MOTOR_SET_LIMIT,
	CMD_CH_MOTOR_DISABLE_LIMIT,
	CMD_CH_MOTOR_ENABLE_LIMIT,
	CMD_CH_MOTOR_RECALC,
	CMD_CH_MOTOR_GOTO_DEGREE,
	CMD_CH_MOTOR_GOTO_SAT,

	// Reset DiSEqC
	CMD_CH_RESET_DISEQC,

	CMD_CH_END					/**< enum 마지막 표시. 명령 아님. */
};

#if defined(CONFIG_DEBUG)
#define MAX_SWAP_TP									32

typedef struct
{
	DxDeliveryType_e	 eDeliType;

	HINT32				 nSrcFreq;			// 실제로 SI에서 받은 Frequency
	HINT32				 nRemapFreq;		// Test 환경에서 고쳐야 하는 Frequency
} TEST_TP_SWAP_t;

#endif


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static CH_TunerContext_t	*s_TunerContext;

/* Action context는 action당 하나씩 갖는다 */
static CH_ActionContext_t	s_ActionContext[NUM_MAX_ACTION];

static HULONG				s_ulChMsgQId;
static HULONG				s_ulChTaskQid;
static HULONG 				s_ulChSyncSema; 	/**< Sync API 처리용 세마포 */

static const SAT_SatPosInfo_t s_stSatPosList[] =
{
	{		eSatType_AGILA_2,       				0xff,     	 1460,			"146.0E"	},
	{		eSatType_AMOS_1,        				25,       	-40,      		"4.0W"		},
	{		eSatType_ANIK_F1,       				0xff,     	-1073,			"107.3W"	},
	{		eSatType_APSTAR_1,      				0xff,     	 765,			"76.5E"		},
	{		eSatType_APSTAR_1A,     				0xff,     	 765,			"76.5E"		},
	{		eSatType_APSTAR_2R,     				0xff,     	 765,			"76.5E"		},
	{		eSatType_ARABSAT_2A3A,					17, 	  	 260,			"26.0E" 	},
	{		eSatType_ARABSAT_2B,    				20,       	 305,      		"30.5E"		},
	{		eSatType_ASIASAT_2,     				0xff,     	 1005,			"100.5E"	},
	{		eSatType_ASIASAT_3S,    				0xff,     	 1055,			"105.5E"	},
	{		eSatType_ASTRA_1,       				2,        	 192,			"19.2E"		},
	{		eSatType_ASTRA_2,       				19,       	 282,			"28.2E"		},
	{		eSatType_ASTRA_3,       				0xff,     	 235,			"23.5E"		},
	{		eSatType_ATLANTA_BIRD_1,				28,       	-125,			"12.5W"		},
	{		eSatType_BRASILSAT_B1,  				0xff,     	-700,			"70.0W"		},
	{		eSatType_BRASILSAT_B2,  				0xff,     	-650,			"65.0W"		},
	{		eSatType_BRASILSAT_B3,  				0xff,     	-840,			"84.0W"		},
	{		eSatType_BRASILSAT_B4,  				0xff,     	-840,			"84.0W"		},
	{		eSatType_EURASIASAT_1,  				0xff,     	 420,			"42.0E"		},
	{		eSatType_EUTELSAT_II_F2,				0xff,     	 480,			"48.0E"		},
	{		eSatType_EUTELSAT_W1,   				4,        	 100,			"10.0E"		},
	{		eSatType_EUTELSAT_W2,   				3,        	 160,			"16.0E"		},
	{		eSatType_EUTELSAT_W3,   				5,        	 70,			"7.0E"		},
	{		eSatType_EXPRESS_3A,    				0xff,     	-110,			"11.0W"		},
	{		eSatType_EXPRESS_6A,    				0xff,     	 800,			"80.0E"		},
	{		eSatType_GORIZONT_26,   				0xff,     	 1800,			"???"       },
	{		eSatType_GORIZONT_33,   				0xff,     	 1450,			"145.0E"	},
	{		eSatType_HISPASAT,      				14,       	-300,			"30.0W"		},
	{		eSatType_HOTBIRD,       				1,        	 130,			"13.0E"		},
	{		eSatType_INSAT_2DT,     				0xff,     	 1800,			"???"       },
	{		eSatType_INSAT_2E3B,    				0xff,     	 830,			"83.0E"		},
	{		eSatType_INSAT_3C,      				0xff,     	 830,			"83.0E"		},
	{		eSatType_INTELSAT_605,  				0xff,     	-275,			"27.5W"		},
	{		eSatType_INTELSAT_704,  				0xff,     	 660,			"66.0E"		},
	{		eSatType_INTELSAT_706,  				0xff,     	 1800,			"???"       },
	{		eSatType_INTELSAT_707,  				0xff,     	-10,			"1.0W"		},
	{		eSatType_INTELSAT_709,  				0xff,     	 1800,			"???"       },
	{		eSatType_INTELSAT_805,  				0xff,     	-555,			"55.5W"		},
	{		eSatType_INTELSAT_901,  				11,       	-180,			"18.0W"		},
	{		eSatType_INTELSAT_902,  				0xff,     	 620,			"62.0E"		},
	{		eSatType_INTELSAT_904,  				0xff,     	 600,			"60.0E"		},
	{		eSatType_INTELSAT_906,  				0xff,     	 640,			"64.0E"		},
	{		eSatType_JCSAT_3,       				0xff,     	 1280,			"128.0E"	},
	{		eSatType_KOREASAT_3,    				0xff,     	 1160,			"116.0E"	},
	{		eSatType_LMI_1,         				0xff,     	 750,			"75.0E"		},
	{		eSatType_MEASAT_1,      				0xff,     	 915,			"91.5E"		},
	{		eSatType_MEASAT_2,      				0xff,     	 1480,			"148.0E"	},
	{		eSatType_NAHUEL_1,      				0xff,     	-720,			"72.0W"		},
	{		eSatType_NILESAT,       				27,       	-70,			"7.0 W"		},
	{		eSatType_NSS_703,						0xff,     	 570,			"57.0E"		},
	{		eSatType_NSS_803,						0xff,     	 1800,			"???"       },
	{		eSatType_NSS_806,       				0xff,     	-405,			"40.5W"		},
	{		eSatType_OPTUS_B1,      				0xff,     	 1600,			"160.0E"	},
	{		eSatType_OPTUS_B3,      				0xff,     	 1560,			"156.0E"	},
	{		eSatType_PALAPA_C2,     				0xff,     	 1130,			"113.0E"	},
	{		eSatType_PAS_1R,        				0xff,     	-450,			"45.0W"		},
	{		eSatType_PAS_2,         				0xff,     	 1690,			"169.0E"	},
	{		eSatType_PAS_3R_6_6B,   				0xff,     	-430,			"43.0W"		},
	{		eSatType_PAS_7_10,      				0xff,     	 685,			"68.5E"		},
	{		eSatType_PAS_8,         				0xff,     	 1660,			"166.0E"	},
	{		eSatType_PAS_9,         				0xff,     	-580,			"58.0W"		},
	{		eSatType_SATMEX_5,      				0xff,     	-1168,			"116.8W"	},
	{		eSatType_SINOSAT_1,     				0xff,     	 1105,			"110.5E"	},
	{		eSatType_SIRIUS,        				6,        	 50,			"5.0E"		},
	{		eSatType_ST_1,          				0xff,     	 880,			"88.0E"		},
	{		eSatType_TELECOM_2A,    				0xff,     	 30,			"3.0E"		},
	{		eSatType_TELECOM_2C,    				0xff,     	-50,			"5.0W"		},
	{		eSatType_TELECOM_2D,    				10,       	-80,			"8.0W"		},
	{		eSatType_TELKOM_1,      				0xff,     	 1080,			"108.0E"	},
	{		eSatType_TELSTAR_12,    				0xff,     	-150,			"15.0W"		},
	{		eSatType_THAICOM_1A,    				0xff,     	 1200,			"120.0E"	},
	{		eSatType_THAICOM_2_3,   				0xff,     	 785,			"78.5E"		},
	{		eSatType_THOR,							26,       	-8,     		"0.8W"		},
	{		eSatType_TURKSAT_1B,    				0xff,     	 313,			"31.3E"		},
	{		eSatType_TURKSAT_1C,    				22,       	 420,			"42.0E"		},
	{		eSatType_YAMAL_102,     				0xff,     	 898,			"89.8E"		},
	{		eSatType_AMC_1,         				0xff,     	-1030,			"103.0W"	},
	{		eSatType_AMC_3,         				0xff,     	-870,			"87.0W"		},
	{		eSatType_AMC_4,         				0xff,     	-1010,			"101.0W"	},
	{		eSatType_AMC_7,         				0xff,     	-1370,			"137.0W"	},
	{		eSatType_ANIK_E2,       				0xff,     	-1111,			"111.1W"	},
	{		eSatType_GALAXY_10R,    				0xff,     	-1230,			"123.0W"	},
	{		eSatType_GALAXY_11,     				0xff,     	-910,			"91.0W"		},
	{		eSatType_GALAXY_1R,     				0xff,     	-1330,			"133.0W"	},
	{		eSatType_GALAXY_3C,     				0xff,     	-950,			"95.0W"		},
	{		eSatType_GALAXY_4R,     				0xff,     	-990,			"99.0W"		},
	{		eSatType_GALAXY_9,      				0xff,     	-1270,			"127.0W"	},
	{		eSatType_SATCOM_C3,     				0xff,     	-1310,			"131.0W"	},
	{		eSatType_SATCOM_C4,     				0xff,     	-1350,			"135.0W"	},
	{		eSatType_SOLIDARIDAD_2, 				0xff,     	-1130,			"113.0W"	},
	{		eSatType_TELSTAR_4,     				0xff,     	-890,			"89.0W"		},
	{		eSatType_TELSTAR_5,     				0xff,     	-970,			"97.0W"		},
	{		eSatType_TELSTAR_7,     				0xff,     	-1290,			"129.0W"	},
	{		eSatType_EUROBIRD_1,					0xff,	  	 285,			"28.5E"		},
	{		eSatType_EUROBIRD_3,					0xff,	  	 330,			"33.0E" 	},
	{		eSatType_EUROBIRD_9,					0xff,	  	 90,			"9.0E" 		},
	{		eSatType_EUTELSAT_W4,					0xff,	  	 360,			"36.0E"		},
	{		eSatType_EUTELSAT_W6,					0xff,	  	 216,			"21.6E" 	},
	{		eSatType_EXPRESS_AM1,					0xff,	  	 400,			"40.0E" 	},
	{		eSatType_EXPRESS_AM22,					0xff,	  	 530,			"53.0E" 	},
	{		eSatType_HELLAS_SAT2,					0xff,	  	 390,			"39.0E" 	},
	{		eSatType_INTELSAT_10_02,				0xff,	  	-10,			"1.0W" 		},
	{		eSatType_INTELSAT_12,					0xff,	  	 450,			"45.0E"		},
	{		eSatType_INTELSAT_7_10,					0xff,	  	 685,			"68.5E"		},
	{		eSatType_INTELSAT_802,					0xff,	  	 330,			"33.0E" 	},
	{		eSatType_INTELSAT_907,					13,	 	  	-275,			"27.5W" 	},
	{		eSatType_NSS_7,							0xff,	  	-220,			"22.0W" 	},
	{		eSatType_ATLANTA_BIRD_2,				0xff,	  	-80,			"8.0W"	 	},
	{		eSatType_ATLANTA_BIRD_3,				9,		  	-50,			"5.0W"	 	},
	{		eSatType_THOR_3_5,						26,		  	-10,			"1.0W" 		},
	{		eSatType_TURKSAT_2A,					22,		  	 420,			"42.0E"		},
	{		eSatType_NILESAT_101_102,  				0xff,	  	-70,			"7.0W"		},
	{		eSatType_BADR_4_6,						0xff,	  	 260,			"26.0E"		},
	{		eSatType_YAHLIVE, 						0xff,	  	 525,			"52.5E"		},
	{		eSatType_HISPASAT_1D, 					0xff,	  	-300,			"30.0W"		},
	{		eSatType_HISPASAT_1E,					0xff,	  	-300,			"30.0W" 	},
	{		eSatType_SES_2, 						0xff,	  	-870, 			"87.0W" 	},
	{		eSatType_SES_3, 						0xff,	  	-1030,			"103.0W"	},
	{		eSatType_SES_6,							0xff,	  	-405,			"40.5W" 	},
	{		eSatType_EUTELSAT_5_WEST_A, 			0xff,	  	-50,			"5.0W"	},
	{		eSatType_EUTELSAT_12_WEST_B,			0xff,	  	-125, 			"12.5W" 	},
	{		eSatType_EUTELSAT_113_WEST_A,			0xff,	  	-1130,			"113.0W"	},
	{		eSatType_EUTELSAT_117_WEST_A,			0xff,	  	-1168,			"116.8W" 	},
	{		eSatType_EUTELSAT_HOTBIRD_13B,			0xff,	  	 130,			"13.0E" 	},
	{		eSatType_EUTELSAT_HOTBIRD_13C,			0xff,	  	 130,			"13.0E" 	},
	{		eSatType_EUTELSAT_HOTBIRD_13D,			0xff,	  	 130,			"13.0E" 	},
	{		eSatType_INTELSAT_1R,					0xff,	  	-500, 			"50.0W" 	},
	{		eSatType_INTELSAT_11,					0xff,	  	-431, 			"43.1W" 	},
	{		eSatType_INTELSAT_14,					0xff,	  	-450,			"45.0W" 	},
	{		eSatType_INTELSAT_21,					0xff,	  	-580, 			"58.0W" 	},
	{		eSatType_INTELSAT_23,					0xff,	  	-530, 			"53.0W" 	},
	{		eSatType_INTELSAT_34,					0xff,	  	-555,			"55.5W" 	},
	{		eSatType_INTELSAT_903,					0xff,	  	-345, 			"34.5W" 	},
	{		eSatType_GALAXY_18, 					0xff,	  	-1230,			"123.0W"	},
	{		eSatType_GALAXY_19,						0xff,	  	-970,			"97.0W" 	},
	{		eSatType_GALAXY_28, 					0xff,	  	-890, 			"89.0W" 	},
	{		eSatType_SPACEWAY_1_AND_DIRECTV_10_02,	0xff,	  	-1030,			"103.0W" 	},
	{		eSatType_AMAZONAS_2,					0xff,	  	-610,			"61.0W" 	},
	{		eSatType_AMAZONAS_3,					0xff,	  	-610,			"61.0W" 	},
	{		eSatType_AMC_6, 						0xff,	  	-719, 			"71.9W" 	},
	{		eSatType_AMC_8,							0xff,	  	-1390,			"139.0W" 	},
	{		eSatType_AMC_15,						0xff,	  	-1050,			"105.0W" 	},
	{		eSatType_AMC_18,						0xff,	  	-1050,			"105.0W" 	},
	{		eSatType_AMC_21,						0xff,	  	-1249,			"124.9W" 	},
	{		eSatType_BSAT_3C_JCSAT_110R,			0xff,	  	 285,			"28.5E" 	},
	{		eSatType_N_SAT_110,						0xff,	  	-1010,			"101.0W" 	},
	{		eSatType_TELSTAR_12_VANTAGE,			0xff,	  	-150,			"15.0W" 	},
	{		eSatType_TELSTAR_14R,					0xff,	  	-630, 			"63.0W" 	},
	{		eSatType_ST_2,							0xff,	  	 880,			"88.0E" 	},
	{		eSatType_SIMON_BOLIVAR,					0xff,	  	-780,			"78.0W" 	},
	{		eSatType_STAR_ONE_C1,					0xff,	  	-650, 			"65.0W" 	},
	{		eSatType_STAR_ONE_C2,					0xff,	  	-700,			"70.0W" 	},
	{		eSatType_STAR_ONE_C4,					0xff,	  	-700,			"70.0W" 	},
	{		eSatType_NSS_10,						0xff,	  	-374,			"37.4W" 	}
};

#if defined(CONFIG_MW_CH_CAB_AUTO_SR)
static AUTO_SymbolRateInfo_t	s_stAutoSrInfo;
#endif

STATIC PostMsgToMgrCb_t		s_fnSvcChPostMsgCb = NULL;

/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/
#if defined(CONFIG_MW_CH_SATELLITE)
static HBOOL svc_ch_isMotorMovingCondition(HUINT32 ulTunerGroupId, DxTuneParam_t *tuningInfo);
#endif
static void svc_ch_handleMsg_LOCK(CH_LockMsg_t *lockMsg);
static void svc_ch_handleMsg_STOP(CH_StopMsg_t *stopMsg);
static void svc_ch_handleMsg_MotorCmd (CH_MotorMsg_t *pstMotorMsg);
#if defined(CONFIG_MW_CH_SATELLITE)
static void svc_ch_resetDiseqc (CH_MotorMsg_t *pstMotorMsg);
#endif
static void svc_ch_handleMsg_EVENT(CH_EventMsg_t *eventMsg);
static void svc_ch_tunerEventCallback(HUINT32 ulTunerGroupId, HUINT32 ulRequestId, PalCh_Event_e event);
#if defined(CONFIG_MW_CH_CAB_AUTO_SR)
static HERROR svc_ch_autoSrUpdateState(HUINT32 ulActionId, ChannelState_t eActionState, ChannelState_t eTunerChState, AUTO_SymbolRateInfo_t *pstAutoSrInfo, ChannelState_t *peAutoSrState);
static void svc_ch_autoSrAction(HUINT32 ulActionId, ChannelState_t eAutoSrState, AUTO_SymbolRateInfo_t *pstAutoSrInfo, DxTuneParam_t *pTuningInfo);
#endif
static void svc_ch_updateState(HUINT32 ulActionId, HUINT32 ulTunerGroupId, ChannelState_t *peAutoSrState);
static HERROR svc_ch_startTune(HUINT32 ulTunerGroupId, HUINT32 ulDemuxId, HUINT16 usUniqueId, DxTuneParam_t *tuningInfo);
static void svc_ch_stopTune (HUINT32 ulActionId, HUINT32 ulTunerGroupId, HUINT32 ulDemuxId, HUINT16 usNextUniqueId, HBOOL bSkipTunerStop);
static HERROR svc_ch_startTuneMsgSend(Handle_t hAction, SvcRsc_Info_t *pstRscInfo, HUINT16 usUniqueId, DxTuneParam_t *tuningParam, HBOOL bUseAutoSrMode);
INLINE static void svc_ch_tunerStart (HUINT32 ulTunerGroupId, DxTuneParam_t *tuningInfo, HUINT16 usUniqueId);
INLINE static void svc_ch_tunerStop (HUINT32 ulTunerGroupId);
#if defined(CONFIG_DEBUG)
static HERROR svc_ch_dbg_GetRemappedFreq (DxDeliveryType_e eDeliType, HUINT32 ulSrcFreq, HUINT32 *pulRemapFreq);
static HERROR svc_ch_dbg_GetOrgFreqFromRemappedFreq (DxDeliveryType_e eDeliType, HUINT32 ulRemapFreq, HUINT32 *pulSrcFreq);
#endif
#if defined(CONFIG_MW_CH_AUTOMATIC_RETUNE)
static HBOOL	svc_ch_needReTuneWithAutoParam (HUINT32 ulTunerGroupId);
static HERROR	svc_ch_reTuneWithAutoParam (HUINT32 ulTunerGroupId);
static HERROR	svc_ch_checkTuningInfoChanged (HUINT32 ulTunerGroupId);
#endif

#if defined(CONFIG_MW_CH_SATELLITE)
static HERROR svc_ch_getLockedInfo_SAT(HUINT32 ulTunerGroupId, DxTuneParam_t *lockedTuningInfo);
#endif
#if defined(CONFIG_MW_CH_TERRESTRIAL)
static HERROR svc_ch_getLockedInfo_TER(HUINT32 ulTunerGroupId, DxTuneParam_t *lockedTuningInfo);
#endif
#if defined(CONFIG_MW_CH_CABLE)
static HERROR svc_ch_getLockedInfo_CAB(HUINT32 ulTunerGroupId, DxTuneParam_t *lockedTuningInfo);
#endif

STATIC HINT32 svc_ch_PostMsgToMgr(HINT32 nMsg, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	if (s_fnSvcChPostMsgCb)
	{
		return (* s_fnSvcChPostMsgCb)(nMsg, hAction, nParam1, nParam2, nParam3);
	}

	return 0;
}

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
static void svc_ch_task(void *param)
{
	HUINT32 		ulRet;
	CH_Msg_t 		chMsg;

	while (1)
	{
		ulRet = VK_MSG_Receive(s_ulChMsgQId, &chMsg, sizeof(CH_Msg_t));
		if (ulRet != VK_OK)
		{
			HxLOG_Error("VK_MSG_Receive failed. s_ulChMsgQId(%d) \n",s_ulChMsgQId);
		}

		switch(chMsg.ulCmd)
		{
			case CMD_CH_LOCK :
				svc_ch_handleMsg_LOCK((CH_LockMsg_t *)&chMsg);
				break;

			case CMD_CH_STOP :
				svc_ch_handleMsg_STOP((CH_StopMsg_t *)&chMsg);
				break;

			case CMD_CH_TUNER_EVENT :
				svc_ch_handleMsg_EVENT((CH_EventMsg_t *)&chMsg);
				break;

			// Motor Messages
			case CMD_CH_MOTOR_STOP:
			case CMD_CH_MOTOR_GOTO_POS:
			case CMD_CH_MOTOR_STORE_POS:
			case CMD_CH_MOTOR_DRIVE:
			case CMD_CH_MOTOR_SET_LIMIT:
			case CMD_CH_MOTOR_DISABLE_LIMIT:
			case CMD_CH_MOTOR_ENABLE_LIMIT:
			case CMD_CH_MOTOR_RECALC:
			case CMD_CH_MOTOR_GOTO_DEGREE:
			case CMD_CH_MOTOR_GOTO_SAT:
				svc_ch_handleMsg_MotorCmd ((CH_MotorMsg_t *)&chMsg);
				break;

#if defined(CONFIG_MW_CH_SATELLITE)
			// Reset DiSEqC
			case CMD_CH_RESET_DISEQC:
				svc_ch_resetDiseqc ((CH_MotorMsg_t *)&chMsg);
				break;
#endif

			default :
				HxLOG_Critical("\n\n");
				break;
		}
	}
}

HBOOL svc_ch_isMotorAntenna(DxTuneParam_t *tuningInfo)
{
	if(tuningInfo->eDeliType != eDxDELIVERY_TYPE_SAT)
	{
		return FALSE;
	}

	if(tuningInfo->sat.antennaType != eDxANT_TYPE_DISEQC)
	{
		return FALSE;
	}

	if( (tuningInfo->sat.antInfo.info.diseqc.diseqcVersion == eDxDISEQC_VER_1_2) ||
		(tuningInfo->sat.antInfo.info.diseqc.diseqcVersion == eDxDISEQC_VER_1_3) )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

#if defined(CONFIG_MW_CH_SATELLITE)
HBOOL svc_ch_isMotorMovingCondition(HUINT32 ulTunerGroupId, DxTuneParam_t *tuningInfo)
{
	if(svc_ch_isMotorAntenna(tuningInfo) != TRUE)
	{
		return FALSE;
	}
	else if(tuningInfo->sat.antInfo.info.diseqc.diseqcVersion == eDxDISEQC_VER_1_2)
	{
		if(s_TunerContext[ulTunerGroupId].lastMotorDiseqcVer != eDxDISEQC_VER_1_2)
		{
			return TRUE;
		}
		else if(tuningInfo->sat.antInfo.info.diseqc.ucMotorPosition != s_TunerContext[ulTunerGroupId].ucLastMotorPosition)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else if(tuningInfo->sat.antInfo.info.diseqc.diseqcVersion == eDxDISEQC_VER_1_3)
	{
		if(s_TunerContext[ulTunerGroupId].lastMotorDiseqcVer != eDxDISEQC_VER_1_3)
		{
			return TRUE;
		}
		else if(tuningInfo->sat.antInfo.info.diseqc.sSatLonggitude != s_TunerContext[ulTunerGroupId].sLastSatLonggitude)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}
#endif

STATIC HUINT32 svc_ch_countTunedAction (HUINT32 ulTunerGroupId)
{
	HUINT32				 ulActCnt, ulLockNum;

	for (ulActCnt = 0, ulLockNum = 0; ulActCnt < NUM_MAX_ACTION; ulActCnt++)
	{
		CH_ActionContext_t			*pstActionCtxt = &(s_ActionContext[ulActCnt]);

		if ((pstActionCtxt->eChState != eSTATE_IDLE) &&
			(pstActionCtxt->ulTunerGroupId == ulTunerGroupId))
		{
			ulLockNum++;
		}
	}

	return ulLockNum;
}

void svc_ch_handleMsg_LOCK(CH_LockMsg_t *lockMsg)
{
	HUINT32			 ulActionId;
	HUINT32	 ulRfInputId, ulTunerGroupId, ulDemuxId;
	SvcRsc_Item_t 	*pstRfInput = &(lockMsg->stRscInfo.astRscItem[eRxRSCTYPE_RFINPUT]);
	SvcRsc_Item_t		*pstTuner = &(lockMsg->stRscInfo.astRscItem[eRxRSCTYPE_TUNER_GROUP]);
	SvcRsc_Item_t		*pstDemux = &(lockMsg->stRscInfo.astRscItem[eRxRSCTYPE_DEMUX]);
	HERROR			 err;

	ulActionId = 0;
	ulTunerGroupId = ulDemuxId = RxRSCID_NULL;

	/* Action ID 찾고... */
	ulActionId = PAL_PIPE_GetActionId(lockMsg->hAction);

	/* RF-Input ID */
	ulRfInputId = (pstRfInput->bAllocated == TRUE) ? pstRfInput->ulResourceId : RxRSCID_NULL;
	if (ulRfInputId == RxRSCID_NULL)
	{
#if defined(CONFIG_MW_CH_SAT_RFINPUT_SELECTABLE)
		HxLOG_Error("ulRfInputId:%u\n",ulRfInputId);
#endif
	}

	/* Tuner ID 찾고... */
	ulTunerGroupId = (pstTuner->bAllocated == TRUE) ? pstTuner->ulResourceId : RxRSCID_NULL;
	if (ulTunerGroupId == RxRSCID_NULL)
	{
		HxLOG_Error("ulTunerGroupId:%u\n",ulTunerGroupId);
	}

	/* Demux ID 찾고... */
	ulDemuxId = (pstDemux->bAllocated == TRUE) ? pstDemux->ulResourceId : RxRSCID_NULL;
	if (ulDemuxId == RxRSCID_NULL)
	{
		HxLOG_Error("ulDemuxId:%u\n",ulDemuxId);
	}

	HxLOG_Print("MESSAGE : CMD_CH_LOCK, hAction:0x%x, Tuner ID:%d, Demux ID:%d\n", lockMsg->hAction, ulTunerGroupId, ulDemuxId);
	HxLOG_Print("[svc_ch_handleMsg_LOCK] ulFrequency [%d], ulSymbolRate [%d]\n", lockMsg->tuningInfo.cab.ulFrequency, lockMsg->tuningInfo.cab.ulSymbolRate);

	/* Session expire 검사하고... */
	if(PAL_PIPE_IsActionHandleLatest(lockMsg->hAction) == ERR_OK)
	{
		CH_ActionContext_t			*pstActionCtxt = &(s_ActionContext[ulActionId]);

		pstActionCtxt->hAction = lockMsg->hAction;

		if(pstActionCtxt->eChState != eSTATE_IDLE)
		{
			pstActionCtxt->eChState			= eSTATE_IDLE;
			pstActionCtxt->ulTunerGroupId	= (HUINT32)-1;
			pstActionCtxt->ulDemuxId		= (HUINT32)-1;
		}

#if defined(CONFIG_MW_CH_SAT_RFINPUT_SELECTABLE)
		if (eDxDELIVERY_TYPE_SAT == lockMsg->tuningInfo.eDeliType)
		{
			PAL_CH_SAT_SetRfInputConnection (ulTunerGroupId, ulRfInputId);
		}
#endif

		HxLOG_Print("[svc_ch_handleMsg_LOCK] ulFrequency [%d], ulSymbolRate [%d]\n", lockMsg->tuningInfo.cab.ulFrequency, lockMsg->tuningInfo.cab.ulSymbolRate);
		err = svc_ch_startTune(ulTunerGroupId, ulDemuxId, lockMsg->usUniqueId, &(lockMsg->tuningInfo));
		if (ERR_OK == err)
		{
			PAL_PIPE_SetDemux (ulActionId, ulDemuxId);
			PAL_PIPE_SetInputResource (ulActionId, ePAL_PIPE_INPUT_TUNER, (HUINT32)ulTunerGroupId);

			pstActionCtxt->eChState = eSTATE_LOCKING;
			pstActionCtxt->ulTunerGroupId = ulTunerGroupId;
			pstActionCtxt->ulDemuxId = ulDemuxId;
			pstActionCtxt->stRscInfo = lockMsg->stRscInfo;

			svc_ch_PostMsgToMgr(eSEVT_CH_STARTED, pstActionCtxt->hAction, 0, 0, 0);

#if defined(CONFIG_MW_LOGCAPTURE)
			// 수신감도 측정을 위해 Tunuer Group ID, Constellation 전달
			svc_ch_PostMsgToMgr(eSEVT_LOGCAPTURE_CHANNEL_INFO, pstActionCtxt->hAction, (HINT32)ulTunerGroupId, lockMsg->tuningInfo.cab.constellation, 0);
#endif

			/* 이미 동작중인 튜너에 등록했을 경우에는 바로 EVT_CH_LOCKED나 EVT_CH_NO_SIGNAL이 올라가도록 해야 한다. */
			svc_ch_updateState(ulActionId, ulTunerGroupId, NULL);
		}
	}
	else
	{
		HxLOG_Print("\tAction handle 0x%x is already expired\n", lockMsg->hAction);
	}
}

void svc_ch_handleMsg_STOP(CH_StopMsg_t *stopMsg)
{
	HUINT32			ulActionId;
	HUINT32			ulTunerGroupId, ulDemuxId;

	/* Action ID 찾고... */
	ulActionId = PAL_PIPE_GetActionId(stopMsg->hAction);
	HxLOG_Print("MESSAGE : CMD_CH_STOP, hAction:0x%x\n", stopMsg->hAction);

	/* Session expire 검사하고... */
	if(PAL_PIPE_IsActionHandleLatest(stopMsg->hAction) == ERR_OK)
	{
		CH_ActionContext_t			*pstActionCtxt = &(s_ActionContext[ulActionId]);

		pstActionCtxt->hAction	= stopMsg->hAction;

		ulTunerGroupId			= pstActionCtxt->ulTunerGroupId;
		ulDemuxId				= pstActionCtxt->ulDemuxId;

		if(pstActionCtxt->eChState != eSTATE_IDLE)
		{
			/* 돌고 있는 action context임. stop 처리하고 통보한다. */
			/* 해당 튜너를 stop한다. Demux path를 끊어야 하므로 연결되어 있는 demux ID를 같이 알려준다. */
			pstActionCtxt->eChState			= eSTATE_IDLE;
			pstActionCtxt->ulTunerGroupId	= (HUINT32)-1;
			pstActionCtxt->ulDemuxId		= (HUINT32)-1;

			svc_ch_stopTune(ulActionId, ulTunerGroupId, ulDemuxId, stopMsg->usNextUniqueId, FALSE);
#if defined(CONFIG_MW_CH_CAB_AUTO_SR)
			s_stAutoSrInfo.stContext[ulActionId].eSrMode = eSymbolRateMode_Normal;
#endif
		}

#if defined(CONFIG_MW_CH_SAT_RFINPUT_SELECTABLE)
		if (svc_ch_countTunedAction(ulTunerGroupId) == 0)
		{
			HUINT32			 ulDeliType = 0;

			PAL_CH_GetSupportDeliveryType(ulTunerGroupId, &ulDeliType);
			if (0 != (ulDeliType & eDxDELIVERY_TYPE_SAT))
			{
				PAL_CH_SAT_ResetRfInputConnection(ulTunerGroupId);
			}

		}
#endif
		/* Async API 모드인 경우는 이벤트 통보해야 한다. */
		if(stopMsg->syncMode == eAsyncMode)
		{
			svc_ch_PostMsgToMgr(eSEVT_CH_STOPPED, pstActionCtxt->hAction, 0, 0, 0);
		}
	}
	else
	{
		HxLOG_Print("\tAction handle 0x%x is already expired\n", stopMsg->hAction);
	}

	/* Sync API 모드인 경우는 expire된 action handle 이라도 반드시 sema4를 릴리즈 해야 한다. 안그러면 시스템 서버린다. */
	if(stopMsg->syncMode == eSyncMode)
	{
		VK_SEM_Release(s_ulChSyncSema);
	}
}

STATIC void svc_ch_handleMsg_MotorCmd (CH_MotorMsg_t *pstMotorMsg)
{
	HUINT32			ulActionId;
	HUINT32	ulTunerGroupId, ulDemuxId;
	HERROR			hErr;

	 ulActionId = 0;
	 ulTunerGroupId = ulDemuxId = RxRSCID_NULL;

	/* Action ID 찾고... */
	ulActionId = PAL_PIPE_GetActionId (pstMotorMsg->hAction);

	/* Tuner ID 찾고... */
	hErr = PAL_PIPE_GetResourceId(ulActionId, eRxRSCTYPE_TUNER_GROUP, &ulTunerGroupId);
	if (hErr)
	{
		HxLOG_Error("PAL_PIPE_GetResourceId ret:0x%X\n",hErr);
	}

	hErr = PAL_PIPE_GetResourceId(ulActionId, eRxRSCTYPE_DEMUX, &ulDemuxId);
	if (hErr)
	{
		HxLOG_Error("PAL_PIPE_GetResourceId ret:0x%X\n",hErr);
	}

	HxLOG_Print("MESSAGE : MOTORMSG(%04x), hAction:0x%x, Tuner ID:%d, Demux ID:%d\n", pstMotorMsg->ulCmd, pstMotorMsg->hAction, ulTunerGroupId, ulDemuxId);

	/* Session expire 검사하고... */
	if(PAL_PIPE_IsActionHandleLatest(pstMotorMsg->hAction) == ERR_OK)
	{
		s_ActionContext[ulActionId].hAction = pstMotorMsg->hAction;

		if (ulTunerGroupId == RxRSCID_NULL)
		{
			HxLOG_Error("ulTunerGroupId is RxRSCID_NULL\n");
		} else
		{
			switch (pstMotorMsg->ulCmd)
			{
#if defined(CONFIG_MW_CH_SATELLITE)
			case CMD_CH_MOTOR_STOP:		// Stop
				hErr = PAL_CH_MOTOR_Stop(ulTunerGroupId);
				break;

			case CMD_CH_MOTOR_GOTO_POS:
				hErr = PAL_CH_MOTOR_GotoPosition(ulTunerGroupId, (HUINT8)pstMotorMsg->nArg1);
				break;

			case CMD_CH_MOTOR_STORE_POS:
				hErr = PAL_CH_MOTOR_StorePosition(ulTunerGroupId, (HUINT8)pstMotorMsg->nArg1);
				break;

			case CMD_CH_MOTOR_DRIVE:
				hErr = PAL_CH_MOTOR_Drive(ulTunerGroupId, (CH_MotorDirection_t)pstMotorMsg->nArg1, (CH_MotorDriveMode_t)pstMotorMsg->nArg2, (HUINT8)pstMotorMsg->nArg3);
				break;

			case CMD_CH_MOTOR_SET_LIMIT:
				hErr = PAL_CH_MOTOR_SetLimit(ulTunerGroupId, (CH_MotorDirection_t)pstMotorMsg->nArg1);
				break;

			case CMD_CH_MOTOR_DISABLE_LIMIT:
				hErr = PAL_CH_MOTOR_DisableLimit(ulTunerGroupId);
				break;

			case CMD_CH_MOTOR_ENABLE_LIMIT:
				hErr = PAL_CH_MOTOR_EnableLimit(ulTunerGroupId);
				break;

			case CMD_CH_MOTOR_RECALC:
				hErr = PAL_CH_MOTOR_Recalculate(ulTunerGroupId, (HUINT8)pstMotorMsg->nArg1);
				break;

			case CMD_CH_MOTOR_GOTO_DEGREE:
				hErr = PAL_CH_MOTOR_GotoDegree(ulTunerGroupId, (HINT16)pstMotorMsg->nArg1);
				break;

			case CMD_CH_MOTOR_GOTO_SAT:
				hErr = PAL_CH_MOTOR_GotoSatellite(ulTunerGroupId, (HINT16)pstMotorMsg->nArg1);
				break;
#endif

			default:
				hErr = ERR_FAIL;
				break;
			}
		}
	}
	else
	{
		HxLOG_Print("\tAction handle 0x%x is already expired\n", pstMotorMsg->hAction);
	}

	if (pstMotorMsg->syncMode == eSyncMode)
	{
		VK_SEM_Release(s_ulChSyncSema);
	}
	else
	{
		svc_ch_PostMsgToMgr(eSEVT_CH_MOTOR_CMD_COMPLETED, pstMotorMsg->hAction, eDxDELIVERY_TYPE_SAT, pstMotorMsg->ulCmd, 0);
	}
}

#if defined(CONFIG_MW_CH_SATELLITE)
static void svc_ch_resetDiseqc (CH_MotorMsg_t *pstMotorMsg)
{
	HUINT32			ulActionId;
	HUINT32	ulTunerGroupId;
	HUINT8			szClearCommand[3] = {0xE0, 0x10, 0x00};
	HERROR			hErr;

	/* Action ID 찾고... */
	ulActionId = PAL_PIPE_GetActionId (pstMotorMsg->hAction);

	hErr = PAL_PIPE_GetResourceId (ulActionId, eRxRSCTYPE_TUNER_GROUP, &ulTunerGroupId);
	if (hErr)
	{
		HxLOG_Error("PAL_PIPE_GetResourceId ret:0x%X\n",hErr);
	}

	PAL_CH_SAT_SendDiseqcCommand((HUINT32)ulTunerGroupId, szClearCommand, 3);

	if (pstMotorMsg->syncMode == eSyncMode)
	{
		VK_SEM_Release(s_ulChSyncSema);
	}
}
#endif

HINT8 *svc_ch_getTunerEventStr(PalCh_Event_e eTunerEvent)
{
	switch(eTunerEvent)
	{
		ENUM_TO_STR(ePAL_CH_EVENT_LOCKED);
		ENUM_TO_STR(ePAL_CH_EVENT_NO_SIGNAL);
		ENUM_TO_STR(ePAL_CH_EVENT_ANTENNA_FAULT);
		ENUM_TO_STR(ePAL_CH_EVENT_ANTENNA_OK);
		ENUM_TO_STR(ePAL_CH_EVENT_MOTOR_MOVING);
		ENUM_TO_STR(ePAL_CH_EVENT_TSMF_GOOD);
		ENUM_TO_STR(ePAL_CH_EVENT_TSMF_BAD);
		ENUM_TO_STR(ePAL_CH_EVENT_EWS_ON);
		ENUM_TO_STR(ePAL_CH_EVENT_EWS_OFF);
	}

	HxLOG_Print("Unknown tuner event (%d)\n", eTunerEvent);
	return "Unknown tuner event";
}

void svc_ch_handleMsg_EVENT(CH_EventMsg_t *eventMsg)
{
	HUINT32			i;
	HUINT32	ulTunerGroupId = eventMsg->ulTunerGroupId;
	HUINT32			ulRequestId = eventMsg->ulRequestId;
	PalCh_Event_e	tunerEvent = eventMsg->event;

	HxLOG_Print("MESSAGE : CMD_CH_TUNER_EVENT ==> %s\n", svc_ch_getTunerEventStr(tunerEvent));

	/* Update state to tuner context */
	switch(tunerEvent)
	{
		case ePAL_CH_EVENT_LOCKED:
			if(ulRequestId == s_TunerContext[ulTunerGroupId].ulRequestId)
			{
				s_TunerContext[ulTunerGroupId].eChState = eSTATE_LOCKED;

				HxLOG_Print(HxANSI_COLOR_YELLOW("Lock Time [%d tick]\n"), (HLIB_STD_GetSystemTick() - s_TunerContext[ulTunerGroupId].ulLastReqTick));

				/* Update each state of action context and send event to AP if changed. */
				for(i = 0; i < NUM_MAX_ACTION; i++)
				{
					if(s_ActionContext[i].eChState != eSTATE_IDLE)
					{
						svc_ch_updateState(i, ulTunerGroupId, NULL);
					}
				}
			}

#if defined(CONFIG_MW_CH_AUTOMATIC_RETUNE)
			// Re-tuning 해서 성공했을 경우 실제 Tuning 정보를 보아 달라졌으면 AP에 해당 정보를 알려 줘야 한다.
			if (s_TunerContext[ulTunerGroupId].bRetuneToAuto == TRUE)
			{
				svc_ch_checkTuningInfoChanged (ulTunerGroupId);
				s_TunerContext[ulTunerGroupId].bRetuneToAuto = FALSE;
			}
#endif
			break;

		case ePAL_CH_EVENT_NO_SIGNAL:
		case ePAL_CH_EVENT_MOTOR_MOVING:
			if(ulRequestId == s_TunerContext[ulTunerGroupId].ulRequestId)
			{
				ChannelState_t	eAutoSrState;

				HxLOG_Print(HxANSI_COLOR_YELLOW("Unlock Time [%d tick]\n"), (HLIB_STD_GetSystemTick() - s_TunerContext[ulTunerGroupId].ulLastReqTick));

				if( (tunerEvent == ePAL_CH_EVENT_MOTOR_MOVING) && (s_TunerContext[ulTunerGroupId].eChState == eSTATE_LOCKED) )
				{ /* MOTOR_MOVING 인데 이미 locked 로 갔으면 더 이상 돌지 말자... */
					break;
				}
				else if( (svc_ch_isMotorAntenna(&s_TunerContext[ulTunerGroupId].tuningInfo) == TRUE) &&
					( (VK_TIMER_GetSystemTick() - s_TunerContext[ulTunerGroupId].ulMotorMovingStartTime) < MOTOR_MOVING_TIMEOUT)
				)
				{ /* Motor의 경우 motor moving time expire이전 no signal은 motor moving으로 줘야 한다. */
					s_TunerContext[ulTunerGroupId].eChState = eSTATE_MOTOR_MOVING;
				}
				else
				{
					s_TunerContext[ulTunerGroupId].eChState = eSTATE_NO_SIGNAL;
				}

#if defined(CONFIG_MW_CH_AUTOMATIC_RETUNE)
				// Tuning Fail 하고 Re-tuning 한 경우가 아니라면 Re-tuning을 시도해야 한다.
				if (s_TunerContext[ulTunerGroupId].bRetuneToAuto == FALSE &&
					s_TunerContext[ulTunerGroupId].eChState == eSTATE_NO_SIGNAL)
				{
					if (svc_ch_needReTuneWithAutoParam(ulTunerGroupId) == TRUE)
					{
						svc_ch_reTuneWithAutoParam (ulTunerGroupId);
					}
				}
				else if (s_TunerContext[ulTunerGroupId].bRetuneToAuto == TRUE)
				{
					// 이미 이전에 Fail 를 올리고 처리한 이상, 더 이상 또 No Signal을 올릴 필요가 없다.
					return;
				}
#endif

				/* Update each state of action context and send event to AP if changed. */
				for(i = 0; i < NUM_MAX_ACTION; i++)
				{
					if(s_ActionContext[i].eChState != eSTATE_IDLE)
					{
						svc_ch_updateState(i, ulTunerGroupId, &eAutoSrState);
#if defined(CONFIG_MW_CH_CAB_AUTO_SR)
						svc_ch_autoSrAction(i, eAutoSrState, &s_stAutoSrInfo, &s_TunerContext[ulTunerGroupId].tuningInfo);
#endif
#if 0 // defined(CONFIG_MW_CH_CAB_AUTO_SR)
						if (s_stAutoSrInfo.stContext[i].eSrMode == eSymbolRateMode_Auto)
						{
							if (eChState == eSTATE_NO_SIGNAL)
							{
								// The Last Symbol-rate
								HxLOG_Print("\t==> Sending eSEVT_CH_NO_SIGNAL\n");

								s_stAutoSrInfo.stContext[i].eSrMode = eSymbolRateMode_Normal;
								s_stAutoSrInfo.stContext[i].ulCurrIdx = 0;
								s_stAutoSrInfo.stContext[i].eChState = eSTATE_NO_SIGNAL;

								svc_ch_PostMsgToMgr(eSEVT_CH_NO_SIGNAL, s_ActionContext[i].hAction, 0, 0, 0);
							}
							else if (eChState == eSTATE_LOCKING)
							{
								DxTuneParam_t	stTuningInfo;

								HxSTD_memcpy(&stTuningInfo, &s_TunerContext[ulTunerGroupId].tuningInfo, sizeof(DxTuneParam_t));
								stTuningInfo.cab.ulSymbolRate = s_stAutoSrInfo.aulSymbolRateList[s_stAutoSrInfo.stContext[i].ulCurrIdx + 1];
								s_stAutoSrInfo.stContext[i].ulCurrIdx++;
								s_stAutoSrInfo.stContext[i].eChState = eSTATE_LOCKING;

								HxLOG_Print("\t==> eSEVT_CH_NO_SIGNAL : tune to next Symbol-rate\n");
								HxLOG_Print("\t==> eSEVT_CH_NO_SIGNAL : stTuningInfo.cab.ulSymbolRate [%d]\n", stTuningInfo.cab.ulSymbolRate);
								svc_ch_startTuneMsgSend(s_ActionContext[i].hAction, -1, &stTuningInfo, FALSE);
							}
							else if (eChState == eSTATE_LOCKED)
							{ /* Tuner의 state가 LOCKED로 바뀌고, action의 state가 LOCKED가 아니므로 AP에 notice 해줘야 한다. */
								HxLOG_Print("\t==> Sending eSEVT_CH_LOCKED\n");
								HxLOG_Print("\t==> Sending eSEVT_CH_LOCKED for action[%d]\n", i);

								s_stAutoSrInfo.stContext[i].eSrMode = eSymbolRateMode_Normal;
								s_stAutoSrInfo.stContext[i].ulCurrIdx = 0;
								s_stAutoSrInfo.stContext[i].eChState = eSTATE_LOCKED;

								svc_ch_PostMsgToMgr(eSEVT_CH_LOCKED, s_ActionContext[i].hAction, 0, 0, 0);
							}
						}
#endif
					}
				}

				if(s_TunerContext[ulTunerGroupId].eChState == eSTATE_MOTOR_MOVING)
				{
					int					ulRet;

					VK_TASK_Sleep(1000); /* 1초 기다린 후... NO_SIGNAL을 MOTOR_MOVING으로만 바꿔서 다시 돌린다. */

					eventMsg->event = ePAL_CH_EVENT_MOTOR_MOVING;
					ulRet = VK_MSG_Send(s_ulChMsgQId, eventMsg, sizeof(CH_Msg_t));
					if(ulRet != ERR_OK)
					{
						//ChErrorPrint(0, -1, ("Error from VK_MSG_Send() for eSTATE_MOTOR_MOVING.\n"));
					}
				}
			}
			break;

		case ePAL_CH_EVENT_ANTENNA_FAULT:
			if(s_TunerContext[ulTunerGroupId].workingDeliveryType == eDxDELIVERY_TYPE_SAT)
			{
				svc_ch_PostMsgToMgr(eSEVT_CH_ANTENNA_FAULT, 0, eDxDELIVERY_TYPE_SAT, 0, 0);
			}
			else if(s_TunerContext[ulTunerGroupId].workingDeliveryType == eDxDELIVERY_TYPE_TER)
			{
				svc_ch_PostMsgToMgr(eSEVT_CH_ANTENNA_FAULT, 0, eDxDELIVERY_TYPE_TER, 0, 0);
			}
			else
			{
				//HxLOG_Critical("\n\n");
				HxLOG_Critical("\n\n");
			}
			break;

		case ePAL_CH_EVENT_ANTENNA_OK:
			if(s_TunerContext[ulTunerGroupId].workingDeliveryType == eDxDELIVERY_TYPE_SAT)
			{
				svc_ch_PostMsgToMgr(eSEVT_CH_ANTENNA_OK, 0, eDxDELIVERY_TYPE_SAT, 0, 0);
			}
			else if(s_TunerContext[ulTunerGroupId].workingDeliveryType == eDxDELIVERY_TYPE_TER)
			{
				svc_ch_PostMsgToMgr(eSEVT_CH_ANTENNA_OK, 0, eDxDELIVERY_TYPE_TER, 0, 0);
			}
			else
			{
				HxLOG_Critical("\n\n");
			}
			break;

		default:
			HxLOG_Critical("\n\n");
			break;
	}
}


#if defined(CONFIG_MW_CH_CAB_AUTO_SR)
HERROR svc_ch_autoSrUpdateState(HUINT32 ulActionId, ChannelState_t eActionState, ChannelState_t eTunerChState, AUTO_SymbolRateInfo_t *pstAutoSrInfo, ChannelState_t *peAutoSrState)
{
	if (peAutoSrState == NULL)
	{
		return ERR_FAIL;
	}

	if (pstAutoSrInfo == NULL)
	{
		return ERR_FAIL;
	}

	if (pstAutoSrInfo->stContext[ulActionId].eSrMode != eSymbolRateMode_Auto)
	{
		return ERR_FAIL;
	}

	if (pstAutoSrInfo->stContext[ulActionId].ulCurrIdx == 0)
	{
		pstAutoSrInfo->stContext[ulActionId].eChState = eActionState;
	}
	else
	{
		eActionState = pstAutoSrInfo->stContext[ulActionId].eChState;
	}

	if (peAutoSrState)
	{
		HxLOG_Print(" eActionState [%d], eTunerChState [%d]\n", eActionState, eTunerChState);
		*peAutoSrState = eSTATE_IDLE;
		// 제일 마지막 SR에서도 No Signal일 경우만 eSEVT_CH_NO_SIGNAL Message를 보낸다.
		if( (eActionState != eSTATE_NO_SIGNAL) && (eTunerChState == eSTATE_NO_SIGNAL) )
		{
			HxLOG_Print(" AutoSrIndex [%d/%d]\n", pstAutoSrInfo->stContext[ulActionId].ulCurrIdx, pstAutoSrInfo->ulNumSymbolRate);
			HxLOG_Print("\t==> eSTATE_NO_SIGNAL : pstAutoSrInfo->ulCurrIdx [%d], pstAutoSrInfo->ulNumSymbolRate [%d]\n", pstAutoSrInfo->stContext[ulActionId].ulCurrIdx, pstAutoSrInfo->ulNumSymbolRate);
			*peAutoSrState = (pstAutoSrInfo->stContext[ulActionId].ulCurrIdx + 1) >= (pstAutoSrInfo->ulNumSymbolRate) ? eSTATE_NO_SIGNAL : eSTATE_LOCKING;
		}
		else if( (eActionState != eSTATE_LOCKED) && (eTunerChState == eSTATE_LOCKED) )
		{
			HxLOG_Print("\t==> Sending eSEVT_CH_LOCKED for action[%d]\n", ulActionId);
			*peAutoSrState = eSTATE_LOCKED;
		}
		HxLOG_Print(" New eAutoSrState [%d]\n", *peAutoSrState);
	}

	return ERR_OK;
}

void svc_ch_autoSrAction(HUINT32 ulActionId, ChannelState_t eAutoSrState, AUTO_SymbolRateInfo_t *pstAutoSrInfo, DxTuneParam_t *pTuningInfo)
{
	Handle_t	hAction;

	if (pstAutoSrInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		return;
	}

	hAction = s_ActionContext[ulActionId].hAction;

	if (pstAutoSrInfo->stContext[ulActionId].eSrMode == eSymbolRateMode_Auto)
	{
		// 제일 마지막 SR에서도 No Signal일 경우만 eSEVT_CH_NO_SIGNAL Message를 보낸다.
		HxLOG_Print("\t==> Sending eSEVT_CH_NO_SIGNAL for action[%d]\n", ulActionId);
		HxLOG_Print("\t==> s_stAutoSrInfo.ulCurrIdx [%d], pstAutoSrInfo->ulNumSymbolRate [%d]\n", pstAutoSrInfo->stContext[ulActionId].ulCurrIdx, pstAutoSrInfo->ulNumSymbolRate);

		switch (eAutoSrState)
		{
			case eSTATE_NO_SIGNAL:
				HxLOG_Print("\t==> Sending eSEVT_CH_NO_SIGNAL\n");

				pstAutoSrInfo->stContext[ulActionId].eSrMode = eSymbolRateMode_Normal;
				pstAutoSrInfo->stContext[ulActionId].ulCurrIdx = 0;
				pstAutoSrInfo->stContext[ulActionId].eChState = eSTATE_NO_SIGNAL;

				svc_ch_PostMsgToMgr(eSEVT_CH_NO_SIGNAL, hAction, 0, 0, 0);
				break;

			case eSTATE_LOCKING:
				if (pTuningInfo)
				{
					DxTuneParam_t stTuningInfo;

					HxSTD_memcpy(&stTuningInfo, pTuningInfo, sizeof(DxTuneParam_t));
					stTuningInfo.cab.ulSymbolRate = pstAutoSrInfo->aulSymbolRateList[pstAutoSrInfo->stContext[ulActionId].ulCurrIdx + 1];
					pstAutoSrInfo->stContext[ulActionId].ulCurrIdx++;
					pstAutoSrInfo->stContext[ulActionId].eChState = eSTATE_LOCKING;

					HxLOG_Print("\t==> tune to next Symbol-rate\n");
					HxLOG_Print("\t==> stTuningInfo.cab.ulSymbolRate [%d]\n", stTuningInfo.cab.ulSymbolRate);
					svc_ch_startTuneMsgSend(hAction, &(s_ActionContext[ulActionId].stRscInfo), -1, &stTuningInfo, FALSE);
				}
				else
				{
					HxLOG_Critical("\n\n");
				}
				break;

			case eSTATE_LOCKED:
				HxLOG_Print("\t==> Sending eSEVT_CH_LOCKED\n");
				HxLOG_Print("\t==> Sending eSEVT_CH_LOCKED for action[%d]\n", ulActionId);

				pstAutoSrInfo->stContext[ulActionId].eSrMode = eSymbolRateMode_Normal;
				pstAutoSrInfo->stContext[ulActionId].ulCurrIdx = 0;
				pstAutoSrInfo->stContext[ulActionId].eChState = eSTATE_LOCKED;

				svc_ch_PostMsgToMgr(eSEVT_CH_LOCKED, hAction, 0, 0, 0);
				break;
			default:
				HxLOG_Critical("\n\n");
				break;
		}
	}
}
#endif

#if defined(CONFIG_MW_CH_AUTOMATIC_RETUNE)
static HBOOL svc_ch_needReTuneWithAutoParam (HUINT32 ulTunerGroupId)
{
	DxTuneParam_t		*pstTuneInfo;

	pstTuneInfo = &(s_TunerContext[ulTunerGroupId].tuningInfo);

	switch (s_TunerContext[ulTunerGroupId].workingDeliveryType)
	{
#if defined(CONFIG_MW_CH_TERRESTRIAL)
	case eDxDELIVERY_TYPE_TER:
		if ((pstTuneInfo->ter.constellation != eDxTER_CONSTELLATION_AUTO) ||
			(pstTuneInfo->ter.hierachy != eDxTER_HIERACHY_AUTO) ||
			(pstTuneInfo->ter.codeRate != eDxTER_CODERATE_AUTO) ||
			(pstTuneInfo->ter.transMode != eDxTER_TRANSMODE_AUTO) ||
			(pstTuneInfo->ter.guardInterval != eDxTER_GUARDINTERVAL_AUTO) ||
			(pstTuneInfo->ter.offset != eDxTER_OFFSET_AUTO) ||
			(pstTuneInfo->ter.t2param.pilotPattern != eDxTER_PP_AUTO))
		{
			return TRUE;
		}

		break;
#endif

#if defined(CONFIG_MW_CH_SATELLITE)
	case eDxDELIVERY_TYPE_SAT:				// !!! Satellite 는 절대로 Re-Tune 기능을 활용하면 안된다. !!!!
		return FALSE;
#endif

	default:
		break;
	}

	return FALSE;
}

static HERROR svc_ch_reTuneWithAutoParam (HUINT32 ulTunerGroupId)
{
//#if defined(CONFIG_MW_CH_SATELLITE)
#if 0
	HUINT32				 ulScdUserBandNum, ulScdUserBandFreq;
#endif
	DxTuneParam_t		*pstTuneInfo;
	HERROR				 hErr;

	if (s_TunerContext[ulTunerGroupId].bRetuneToAuto == FALSE)
	{
		pstTuneInfo = &(s_TunerContext[ulTunerGroupId].tuningInfo);

		HxLOG_Print("[MW_CH] Re-tuning with the auto parameters:\n");
		switch (s_TunerContext[ulTunerGroupId].workingDeliveryType)
		{
#if 0
//#if defined(CONFIG_MW_CH_SATELLITE)
		case eDxDELIVERY_TYPE_SAT :
			PAL_CH_SAT_StopTune ((HUINT32)ulTunerGroupId);

			switch(pstTuneInfo->sat.antennaType)
			{
			case eDxANT_TYPE_LNB_ONLY:
				hErr = PAL_CH_LNB_StartTune(
									(HUINT32)ulTunerGroupId,
									++s_TunerContext[ulTunerGroupId].ulRequestId,
									pstTuneInfo->sat.ulFrequency, // MHz
									pstTuneInfo->sat.ulSymbolRate,
									eDxSAT_CODERATE_AUTO,
									eDxSAT_POLAR_AUTO,
									pstTuneInfo->sat.transSpec,
									eDxSAT_PSK_AUTO,
									eDxSAT_PILOT_AUTO,
									pstTuneInfo->sat.antInfo.lnb.ulLnbFreq,
									pstTuneInfo->sat.antInfo.lnb.lnbVoltage
									);
				if (hErr)
				{
					HxLOG_Error("PAL_CH_LNB_StartTune ret:0x%X\n",hErr);
				}
				break;

			case eDxANT_TYPE_DISEQC:
				hErr = PAL_CH_DISEQC_StartTune(
									(HUINT32)ulTunerGroupId,
									++s_TunerContext[ulTunerGroupId].ulRequestId,
									pstTuneInfo->sat.ulFrequency, // MHz
									pstTuneInfo->sat.ulSymbolRate,
									eDxSAT_CODERATE_AUTO,
									eDxSAT_POLAR_AUTO,
									pstTuneInfo->sat.transSpec,
									eDxSAT_PSK_AUTO,
									eDxSAT_PILOT_AUTO,
									pstTuneInfo->sat.antInfo.diseqc.ulLnbFreq,
									pstTuneInfo->sat.antInfo.diseqc.lnbVoltage,
									pstTuneInfo->sat.antInfo.diseqc.b22kTone,
									pstTuneInfo->sat.antInfo.diseqc.diseqcVersion,
									pstTuneInfo->sat.antInfo.diseqc.diseqcInput,
									pstTuneInfo->sat.antInfo.diseqc.toneBurst,
									pstTuneInfo->sat.antInfo.diseqc.ucMotorPosition,
									pstTuneInfo->sat.antInfo.diseqc.sSatLonggitude
									);
				if (hErr)
				{
					HxLOG_Error("PAL_CH_DISEQC_StartTune ret:0x%X\n",hErr);
				}
				break;

			case eDxANT_TYPE_SCD:
				HxLOG_Print("\t\t==> eDxDELIVERY_TYPE_SAT/eDxANT_TYPE_SCD\n");

				if (ulTunerGroupId == 0)
				{
					ulScdUserBandNum	= pstTuneInfo->sat.antInfo.scd.ulScdUserBandNum;
					ulScdUserBandFreq 	= pstTuneInfo->sat.antInfo.scd.ulScdUserBandFreq;
				}
				else if (ulTunerGroupId == 1)
				{
					ulScdUserBandNum	= pstTuneInfo->sat.antInfo.scd.ulScdUserBandNum1;
					ulScdUserBandFreq		= pstTuneInfo->sat.antInfo.scd.ulScdUserBandFreq1;
				}
				else if (ulTunerGroupId == 2)
				{
					ulScdUserBandNum	= pstTuneInfo->sat.antInfo.scd.ulScdUserBandNum2;
					ulScdUserBandFreq		= pstTuneInfo->sat.antInfo.scd.ulScdUserBandFreq2;
				}
				else
				{
					ulScdUserBandNum	= pstTuneInfo->sat.antInfo.scd.ulScdUserBandNum3;
					ulScdUserBandFreq		= pstTuneInfo->sat.antInfo.scd.ulScdUserBandFreq3;
				}

				hErr = PAL_CH_SCD_StartTune(
									(HUINT32)ulTunerGroupId,
									++s_TunerContext[ulTunerGroupId].ulRequestId,
									pstTuneInfo->sat.ulFrequency, // MHz
									pstTuneInfo->sat.ulSymbolRate,
									eDxSAT_CODERATE_AUTO,
									eDxSAT_POLAR_AUTO,
									pstTuneInfo->sat.transSpec,
									eDxSAT_PSK_AUTO,
									eDxSAT_PILOT_AUTO,
									pstTuneInfo->sat.antInfo.scd.ulLnbFreq,
									pstTuneInfo->sat.antInfo.scd.lnbVoltage,
									pstTuneInfo->sat.antInfo.scd.b22kTone,
									pstTuneInfo->sat.antInfo.scd.scdInput,
									ulScdUserBandNum,
									ulScdUserBandFreq
									);
				if (hErr)
				{
					HxLOG_Error("PAL_CH_SCD_StartTune ret:0x%X\n",hErr);
				}
				break;

			case eDxANT_TYPE_SMATV:
				hErr = PAL_CH_SMATV_StartTune(
									(HUINT32)ulTunerGroupId,
									++s_TunerContext[ulTunerGroupId].ulRequestId,
									pstTuneInfo->sat.ulFrequency, // MHz
									pstTuneInfo->sat.ulSymbolRate,
									eDxSAT_CODERATE_AUTO,
									eDxSAT_POLAR_AUTO,
									pstTuneInfo->sat.transSpec,
									eDxSAT_PSK_AUTO,
									eDxSAT_PILOT_AUTO
									);
				if (hErr)
				{
					HxLOG_Error("PAL_CH_SMATV_StartTune ret:0x%X\n",hErr);
				}
				break;

			default:
				break;
			}

			break;
#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
		case eDxDELIVERY_TYPE_TER :
			PAL_CH_TER_StopTune((HUINT32)ulTunerGroupId);

			hErr = PAL_CH_TER_StartTune(
							(HUINT32)ulTunerGroupId,
							++s_TunerContext[ulTunerGroupId].ulRequestId,
							pstTuneInfo->ter.ulFrequency, // KHz
							pstTuneInfo->ter.bandWidth,
							eDxTER_CONSTELLATION_AUTO,
							eDxTER_HIERACHY_AUTO,
							eDxTER_CODERATE_AUTO,
							eDxTER_TRANSMODE_AUTO,
							eDxTER_GUARDINTERVAL_AUTO,
							pstTuneInfo->ter.stream,
							eDxTER_OFFSET_AUTO,
							pstTuneInfo->ter.system,
							/* +++++ T2 Parameters +++++ */
							pstTuneInfo->ter.t2param.preambleFormat,
							pstTuneInfo->ter.t2param.mixed,
							eDxTER_PP_AUTO,
							pstTuneInfo->ter.t2param.extenedCarrier,
							pstTuneInfo->ter.t2param.PAPRreduction,
							pstTuneInfo->ter.t2param.ulPlpId,/* MPLP : change order of param */
							pstTuneInfo->ter.t2param.numPlpBlocks
							);

			if (hErr)
			{
				HxLOG_Error("PAL_CH_TER_StartTune ret:0x%X\n",hErr);
			}
			break;
#endif
		default :
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}

		s_TunerContext[ulTunerGroupId].bRetuneToAuto = TRUE;
	}

	return ERR_OK;
}

static HERROR svc_ch_checkTuningInfoChanged (HUINT32 ulTunerGroupId)
{
	HUINT32				 ulActionId;
	DxTuneParam_t 	 stLockedInfo;
	DbSvc_TuningInfo_t	 stSvcTuneInfo1, stSvcTuneInfo2;
	HERROR				 hErr;

	if (s_TunerContext[ulTunerGroupId].bRetuneToAuto == FALSE && s_TunerContext[ulTunerGroupId].eChState == eSTATE_LOCKED)
	{
		stLockedInfo.deliveryType = s_TunerContext[ulTunerGroupId].workingDeliveryType;
		switch(s_TunerContext[ulTunerGroupId].workingDeliveryType)
		{
#if defined(CONFIG_MW_CH_SATELLITE)
		case eDxDELIVERY_TYPE_SAT :
			hErr = svc_ch_getLockedInfo_SAT (ulTunerGroupId, &stLockedInfo);
			break;
#endif
#if defined(CONFIG_MW_CH_TERRESTRIAL)
		case eDxDELIVERY_TYPE_TER :
			hErr = svc_ch_getLockedInfo_TER (ulTunerGroupId, &stLockedInfo);
			break;
#endif
#if defined(CONFIG_MW_CH_CABLE)
		case eDxDELIVERY_TYPE_CAB :
			hErr = svc_ch_getLockedInfo_CAB (ulTunerGroupId, &stLockedInfo);
			break;
#endif
		default :
			HxLOG_Critical("\n\n");
			break;
		}

		HxSTD_memset (&stSvcTuneInfo1, 0, sizeof(DbSvc_TuningInfo_t));
		HxSTD_memset (&stSvcTuneInfo2, 0, sizeof(DbSvc_TuningInfo_t));


		OTL_CONV_DxTuneParam2DbTuningInfo((const DxTuneParam_t *)&(s_TunerContext[ulTunerGroupId].tuningInfo), &stSvcTuneInfo1);
		OTL_CONV_DxTuneParam2DbTuningInfo((const DxTuneParam_t *)&stLockedInfo, &stSvcTuneInfo2);
//		MWC_UTIL_ConvCh2DbTuningParam (&(s_TunerContext[ulTunerGroupId].tuningInfo), &stSvcTuneInfo1);
//		MWC_UTIL_ConvCh2DbTuningParam (&stLockedInfo, &stSvcTuneInfo2);

//		if (MWC_UTIL_CompareDbTuningParam (s_TunerContext[ulTunerGroupId].workingDeliveryType, &stSvcTuneInfo1, &stSvcTuneInfo2) != 0)
		if (DB_SVC_UtilCompareDbTuningParam(s_TunerContext[ulTunerGroupId].workingDeliveryType, &stSvcTuneInfo1, &stSvcTuneInfo2) != 0)
		{
			HxLOG_Print("[MW CH] Send the message that the Tp locked with the other tuning parameters\n");

			for (ulActionId = 0; ulActionId < NUM_MAX_ACTION; ulActionId++)
			{
				if (s_ActionContext[ulActionId].ulTunerGroupId == ulTunerGroupId)
				{
					svc_ch_PostMsgToMgr (eSEVT_CH_RELOCKED_BY_AUTO, s_ActionContext[ulActionId].hAction, 0, 0, 0);
				}
			}
		}
	}

	return ERR_OK;
}
#endif


void svc_ch_updateState(HUINT32 ulActionId, HUINT32 ulTunerGroupId, ChannelState_t *peAutoSrState)
{
	HINT32		nRelocked;
	ChannelState_t	curState, nextState;

	if( (s_ActionContext[ulActionId].eChState != eSTATE_IDLE) && (s_ActionContext[ulActionId].ulTunerGroupId == ulTunerGroupId) )
	{ /* Active action이 걸린 tuner에 대해서만 update 하고 AP로 이벤트 보냄. */
		HxLOG_Print("Update state for action[%d]\n", ulActionId);

		curState = s_ActionContext[ulActionId].eChState; /* Current eChState in action context */
		nextState = s_TunerContext[ulTunerGroupId].eChState; /* New arrived state in tuner context */

#if defined(CONFIG_MW_CH_CAB_AUTO_SR)
		if ( svc_ch_autoSrUpdateState(ulActionId, curState, nextState, &s_stAutoSrInfo, peAutoSrState) != ERR_OK)
#endif
		{
			if( (curState != eSTATE_NO_SIGNAL) && (nextState == eSTATE_NO_SIGNAL) )
			{ /* Tuner의 state가 NO_SIGNAL로 바뀌고, action의 state가 NO_SIGNAL이 아니므로 AP에 notice 해줘야 한다. */
				HxLOG_Print("\t==> Sending eSEVT_CH_NO_SIGNAL for action[%d]\n", ulActionId);
				svc_ch_PostMsgToMgr(eSEVT_CH_NO_SIGNAL, s_ActionContext[ulActionId].hAction, 0, 0, 0);
			}
			else if( (curState != eSTATE_LOCKED) && (nextState == eSTATE_LOCKED) )
			{ /* Tuner의 state가 LOCKED로 바뀌고, action의 state가 LOCKED가 아니므로 AP에 notice 해줘야 한다. */
				HxLOG_Print("\t==> Sending eSEVT_CH_LOCKED for action[%d]\n", ulActionId);

#if defined(CONFIG_MW_CH_AUTOMATIC_RETUNE)
				// Re-tuning 해서 성공했을 경우 AP에 해당 정보를 알려 줘야 한다.
				nRelocked = (s_TunerContext[ulTunerGroupId].bRetuneToAuto == TRUE) ? TRUE : FALSE;
#else
				nRelocked = FALSE;
#endif
				svc_ch_PostMsgToMgr(eSEVT_CH_LOCKED, s_ActionContext[ulActionId].hAction, nRelocked, 0, 0);
			}
			else if( (curState != eSTATE_MOTOR_MOVING) && (nextState == eSTATE_MOTOR_MOVING) )
			{ /* Tuner의 state가 LOCKED로 바뀌고, action의 state가 LOCKED가 아니므로 AP에 notice 해줘야 한다. */
				HxLOG_Print("\t==> Sending eSEVT_CH_MOVING_MOTOR for action[%d]\n", ulActionId);
				svc_ch_PostMsgToMgr(eSEVT_CH_MOVING_MOTOR, s_ActionContext[ulActionId].hAction, 0, 0, 0);
			}
		}

		s_ActionContext[ulActionId].eChState = nextState; /* 변경을 추적하기 위해 action context 에 등록한다. */
	}
	else
	{
		/* Action이 active되어 있지 않거나 이 action이 걸린 튜너가 아니네... */
	}
}

void svc_ch_tunerEventCallback(HUINT32 ulTunerGroupId, HUINT32 ulRequestId, PalCh_Event_e event)
{
	CH_Msg_t	chMsg;

	chMsg.eventMsg.ulCmd = CMD_CH_TUNER_EVENT;
	chMsg.eventMsg.ulTunerGroupId = ulTunerGroupId;
	chMsg.eventMsg.ulRequestId = ulRequestId;
	chMsg.eventMsg.event = event;

	VK_MSG_Send(s_ulChMsgQId, &chMsg, sizeof(CH_Msg_t));
}

HERROR svc_ch_startTune(HUINT32 ulTunerGroupId, HUINT32 ulDemuxId, HUINT16 usUniqueId, DxTuneParam_t *tuningInfo)
{
	HINT32					nDiff;
	DxDeliveryType_e		deliveryType;
	CH_TunerContext_t		*pstTunerCtxt;
#if defined(CONFIG_DEBUG)
	HERROR					err = ERR_OK;
	HUINT32					ulRemapFreq;
#endif

	HxLOG_Info("svc_ch_startTune()\n");
	if (NULL == tuningInfo)
	{
		HxLOG_Error("tuningInfo is NULL \n");
		return ERR_FAIL;
	}

	if (ulTunerGroupId >= PAL_CH_GetNumOfTunerGroup())
	{
		HxLOG_Error("ulTunerGroupId:%u\n",ulTunerGroupId);
		return ERR_FAIL;
	}

	pstTunerCtxt = &(s_TunerContext[ulTunerGroupId]);

	deliveryType = tuningInfo->eDeliType;

	if( (pstTunerCtxt->supportDeliveryType & deliveryType) == 0) /* supportDeliveryType은 여러개가 OR 되었으므로 반드시 |로 찾아봐야 함. */
	{ /* 지원하지 않는 delivery type으로 resource가 지정됨. Action manager 잘못임... */
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	HxLOG_Print("[svc_ch_startTune] tuningInfo->cab.ulSymbolRate [%d]\n", tuningInfo->cab.ulSymbolRate);

	pstTunerCtxt->tuningInfo.eDeliType = deliveryType;

	HxLOG_Print("received start tune. reset nextUniqueId(%d)->(%d) \n",pstTunerCtxt->usNextUniqueId,(HUINT16)-1);
	pstTunerCtxt->usNextUniqueId = (HUINT16)-1;

	pstTunerCtxt->ulLastReqTick = HLIB_STD_GetSystemTick();

	if (svc_ch_countTunedAction(ulTunerGroupId) == 0)
	{
		/* 빈 튜너임. 맘 대로 써라. */
		HxLOG_Print("Tuner[%d] is empty. Start tuning ... \n", ulTunerGroupId);

#if defined(CONFIG_DEBUG)
		switch(deliveryType)
		{
			case eDxDELIVERY_TYPE_SAT :
				err = svc_ch_dbg_GetRemappedFreq (deliveryType, tuningInfo->sat.tuningInfo.ulFrequency, &ulRemapFreq);
				if (err == ERR_OK)
				{
					tuningInfo->sat.tuningInfo.ulFrequency = ulRemapFreq;
				}
				break;

			case eDxDELIVERY_TYPE_CAB :
				err = svc_ch_dbg_GetRemappedFreq (deliveryType, tuningInfo->cab.ulFrequency, &ulRemapFreq);
				if (err == ERR_OK)
				{
					tuningInfo->cab.ulFrequency = ulRemapFreq;
				}
				break;

			case eDxDELIVERY_TYPE_TER :
				err = svc_ch_dbg_GetRemappedFreq (deliveryType, tuningInfo->ter.ulFrequency, &ulRemapFreq);
				if (err == ERR_OK)
				{
					tuningInfo->ter.ulFrequency = ulRemapFreq;
				}
				break;

			default:
				break;
		}
#endif

		if (pstTunerCtxt->eChState == eSTATE_LOCKED ||
			pstTunerCtxt->eChState == eSTATE_LOCKING)
		{
			nDiff = DB_SVC_UtilCompareChTuningParam(tuningInfo, &(pstTunerCtxt->tuningInfo));
			if (nDiff == 0)
			{
				HxLOG_Print("\t=> Already the tuner tuned to this TP");
				return ERR_OK;
			}

			// Something Wrong! Stop the tuner!
			svc_ch_tunerStop(ulTunerGroupId);
		}

#if defined(CONFIG_MW_LOGCAPTURE)
		svc_ch_PostMsgToMgr(eSEVT_LOGCAPTURE_CHANNEL_CHANGE, HANDLE_NULL, 0, 0, 0);
#endif

		svc_ch_tunerStart (ulTunerGroupId, tuningInfo, usUniqueId);
	}
	else
	{ /* 이미 동작중인 튜너일때는 같은 Unique ID만 받아 줄수 있다. 같은 ID이면 그냥 통과시켜서 아래에서 reference count만 올린다. */
		HxLOG_Print("Tuner %d is already running. Check for the same tuning parameter ...\n", ulTunerGroupId);

		nDiff = DB_SVC_UtilCompareChTuningParam(tuningInfo, &(pstTunerCtxt->tuningInfo));
		if (nDiff != 0)
		{
			HxLOG_Error("Already Tuner (%d) has Tuned with the different tuner parameter!!!\n", ulTunerGroupId);
			return ERR_FAIL;
		}
	}

	return ERR_OK;
}

void svc_ch_stopTune (HUINT32 ulActionId, HUINT32 ulTunerGroupId, HUINT32 ulDemuxId, HUINT16 usNextUniqueId, HBOOL bSkipTunerStop)
{
	HUINT32					 ulLockNum;
	HUINT32					 ulNextTunerGroupId = ulTunerGroupId;
	HBOOL					 bRealStopTune;
	CH_TunerContext_t		*pstTunerCtxt;
	HERROR					 hErr;

	HxLOG_Info("svc_ch_stopTune()\n");
	if(ulTunerGroupId >= PAL_CH_GetNumOfTunerGroup())
	{
		HxLOG_Critical("\n\n");
		return;
	}

	pstTunerCtxt = &(s_TunerContext[ulTunerGroupId]);

	ulLockNum = svc_ch_countTunedAction(ulTunerGroupId);

	// Action이 Tuner자체를 바꿀 수 있다.
	// 그 경우에는 Next Unique ID가 전혀 의미가 없어진다.
	hErr = PAL_PIPE_GetResourceId(ulActionId, eRxRSCTYPE_TUNER_GROUP, &ulNextTunerGroupId);
	if (ERR_OK != hErr)
	{
		ulNextTunerGroupId = ulTunerGroupId;
	}

	if (ulNextTunerGroupId != ulTunerGroupId)
	{
		usNextUniqueId = (HUINT16)-1;
	}

	if (((HUINT16)-1 != usNextUniqueId) && ((HUINT16)-1 == pstTunerCtxt->usNextUniqueId))
	{
		pstTunerCtxt->usNextUniqueId = usNextUniqueId;
		HxLOG_Print("Set nextUniqueId(%d). Do not stop channel until received startTune for uniqudId(%d) \n"
												,pstTunerCtxt->usNextUniqueId,pstTunerCtxt->usNextUniqueId);
	}

	bRealStopTune = FALSE;

	if (TRUE != bSkipTunerStop)
	{
		if (ulLockNum == 0)
		{
			if (((HUINT16)-1 == pstTunerCtxt->usNextUniqueId) ||
				(pstTunerCtxt->usUniqueId != pstTunerCtxt->usNextUniqueId))
			{
				bRealStopTune = TRUE;
			}
		}
	}

	/* Demux Disconnection은 바로 끊어서 Demux에 stream 공급이 안되도록 한다. */
	if (ulDemuxId != RxRSCID_NULL)
	{
		PAL_PIPE_SetCi(ulActionId, -1);
		PAL_PIPE_SetInputResource (ulActionId, ePAL_PIPE_INPUT_NONE, 0);
	}

	if (bRealStopTune == TRUE)
	{
		HxLOG_Print("\t=> Cur UniqueID(%d) => Next UniqueID(%d). Stop TS out !!!\n",
												pstTunerCtxt->usUniqueId, usNextUniqueId);

		svc_ch_tunerStop(ulTunerGroupId);
	}
	else
	{
		HxLOG_Print("\t=> Cur UniqueID(%d) => Next UniqueID(%d). Not real stop !!!\n",
												pstTunerCtxt->usUniqueId, usNextUniqueId);
	}
}

static HERROR svc_ch_startTuneMsgSend(Handle_t hAction, SvcRsc_Info_t *pstRscInfo, HUINT16 usUniqueId, DxTuneParam_t *tuningParam, HBOOL bUseAutoSrMode)
{
	CH_Msg_t	chMsg;
	HUINT32 	ulRet;
	HUINT32		frq;
#if defined(CONFIG_MW_CH_CAB_AUTO_SR)
	HUINT32		ulActionId;
#endif

	chMsg.lockMsg.ulCmd = CMD_CH_LOCK;
	chMsg.lockMsg.hAction = hAction;
	chMsg.lockMsg.usUniqueId = usUniqueId;
	chMsg.lockMsg.tuningInfo = *tuningParam;
	chMsg.lockMsg.stRscInfo = *pstRscInfo;

	switch(tuningParam->eDeliType)
	{
		case eDxDELIVERY_TYPE_SAT:
			frq = tuningParam->sat.tuningInfo.ulFrequency;
			HxLOG_Info("SVC_CH_StartTune(hAction:0x%x, deliveryType:SAT, Frq:%d MHz)\n", hAction, frq);
			break;

		case eDxDELIVERY_TYPE_TER:
			frq = tuningParam->ter.ulFrequency;
			HxLOG_Info("SVC_CH_StartTune(hAction:0x%x, deliveryType:TER, Frq:%d KHz)\n", hAction, frq);
			break;

		case eDxDELIVERY_TYPE_CAB:
			frq = tuningParam->cab.ulFrequency;

#if defined(CONFIG_MW_CH_CAB_AUTO_SR)
			ulActionId = PAL_PIPE_GetActionId(hAction);
			if (bUseAutoSrMode)
			{
				HxLOG_Info("[svc_ch_startTuneMsgSend] tuningParam->cab.ulSymbolRate [%d]\n", tuningParam->cab.ulSymbolRate);
				if (chMsg.lockMsg.tuningInfo.cab.ulSymbolRate == CAB_SYMBOLRATE_AUTO)
				{
					s_stAutoSrInfo.stContext[ulActionId].eSrMode = eSymbolRateMode_Auto;
					s_stAutoSrInfo.stContext[ulActionId].ulCurrIdx = 0;
					chMsg.lockMsg.tuningInfo.cab.ulSymbolRate = s_stAutoSrInfo.aulSymbolRateList[0];
				}
				else
				{
					s_stAutoSrInfo.stContext[ulActionId].eSrMode = eSymbolRateMode_Normal;
				}
				HxLOG_Info("[svc_ch_startTuneMsgSend] tuningParam->cab.ulSymbolRate [%d]\n", chMsg.lockMsg.tuningInfo.cab.ulSymbolRate);
				HxLOG_Info("[svc_ch_startTuneMsgSend] s_stAutoSrInfo.stContext[%d].eSrMode [%d]\n", ulActionId, s_stAutoSrInfo.stContext[ulActionId].eSrMode);
			}
#endif
			HxLOG_Info("SVC_CH_StartTune(hAction:0x%x, deliveryType:CAB, Frq:%d KHz, constel:%d)\n", hAction, frq, tuningParam->cab.constellation);
			break;

		default :
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
	}

	ulRet = VK_MSG_Send(s_ulChMsgQId, &chMsg, sizeof(CH_Msg_t));
	if(ulRet != ERR_OK)
	{
		HxLOG_Error("Error from VK_MSG_Send() in SVC_CH_StartTune().\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

/*
	실제로 Tuner를 PAL 단에 Start / Stop 시키도록 내리는 명령들
*/
INLINE static void svc_ch_tunerStart (HUINT32 ulTunerGroupId, DxTuneParam_t *tuningInfo, HUINT16 usUniqueId)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	HUINT32				 ulScdUserBandNum, ulScdUserBandFreq;
#endif
	CH_TunerContext_t	*pstTunerCtxt = &(s_TunerContext[ulTunerGroupId]);
	HERROR				 err = ERR_FAIL;

	/* support delivery type 중 하나를 실제 working delivery type으로 지정. */
	if(tuningInfo->eDeliType != pstTunerCtxt->workingDeliveryType)
	{ /* 튜너의 용도(delivery type)가 바뀜. */
		HxLOG_Print("Change Tuner[%d]'s delivery type : [%s] => [%s]\n",
											ulTunerGroupId,
											(HCHAR*)OTL_ENUM2STR_ChDeliveryType(pstTunerCtxt->workingDeliveryType),
											(HCHAR*)OTL_ENUM2STR_ChDeliveryType(tuningInfo->eDeliType));

		PAL_CH_DeactivateDemod(ulTunerGroupId, pstTunerCtxt->workingDeliveryType);
		PAL_CH_SetTunerGroupDeliveryType(ulTunerGroupId, tuningInfo->eDeliType);
		PAL_CH_ActivateDemod(ulTunerGroupId, tuningInfo->eDeliType);

		pstTunerCtxt->workingDeliveryType = tuningInfo->eDeliType;
	}

	switch (tuningInfo->eDeliType)
	{
#if defined(CONFIG_MW_CH_SATELLITE)
		case eDxDELIVERY_TYPE_SAT :
		{
			switch(tuningInfo->sat.antennaType)
			{
				case eDxANT_TYPE_LNB_ONLY:
					HxLOG_Print("\t\t==> eDxDELIVERY_TYPE_SAT/eDxANT_TYPE_LNB_ONLY\n");

					err = PAL_CH_LNB_StartTune(
										(HUINT32)ulTunerGroupId,
										++(pstTunerCtxt->ulRequestId),
										tuningInfo->sat.tuningInfo.ulFrequency, // MHz
										tuningInfo->sat.tuningInfo.ulSymbolRate,
										tuningInfo->sat.tuningInfo.eFecCodeRate,
										tuningInfo->sat.tuningInfo.ePolarization,
										tuningInfo->sat.tuningInfo.eTransSpec,
										tuningInfo->sat.tuningInfo.ePskMod,
										tuningInfo->sat.tuningInfo.ePilot,
										tuningInfo->sat.antInfo.info.lnb.ulLnbFreq,
										tuningInfo->sat.antInfo.info.lnb.lnbVoltage
										);
					if (err)
						HxLOG_Error("PAL_CH_LNB_StartTune ret error:0x%X\n",err);
					break;

				case eDxANT_TYPE_DISEQC:
					HxLOG_Print("\t\t==> eDxDELIVERY_TYPE_SAT/eDxANT_TYPE_DISEQC\n");

					if(svc_ch_isMotorMovingCondition(ulTunerGroupId, tuningInfo) == TRUE)
					{ /* Motor 안테나에서 move 조건일 경우 */
						if( (tuningInfo->sat.antInfo.info.diseqc.diseqcVersion == eDxDISEQC_VER_1_2) &&
							(tuningInfo->sat.antInfo.info.diseqc.ucMotorPosition > 0) )
						{
							HxLOG_Print("\t\tChange motor to Diseqc 1.2(pos:%d)\n",
																tuningInfo->sat.antInfo.info.diseqc.ucMotorPosition);

							err = PAL_CH_MOTOR_GotoPosition((HUINT32)ulTunerGroupId, tuningInfo->sat.antInfo.info.diseqc.ucMotorPosition);
							if (err)
							{
								HxLOG_Error("PAL_CH_MOTOR_GotoPosition ret:0x%X\n",err);
							}

							/* 매번 move 하지 않도록 backup */
							pstTunerCtxt->lastMotorDiseqcVer = eDxDISEQC_VER_1_2;
							pstTunerCtxt->ucLastMotorPosition = tuningInfo->sat.antInfo.info.diseqc.ucMotorPosition;
							pstTunerCtxt->ulMotorMovingStartTime = VK_TIMER_GetSystemTick();
						}
						else if( (tuningInfo->sat.antInfo.info.diseqc.diseqcVersion == eDxDISEQC_VER_1_3) &&
							(tuningInfo->sat.antInfo.info.diseqc.sSatLonggitude != 0) )
						{
							HxLOG_Print("\t\tChange motor to Diseqc 1.3(longgitude:%d)\n",
																tuningInfo->sat.antInfo.info.diseqc.sSatLonggitude);

							err = PAL_CH_MOTOR_GotoSatellite((HUINT32)ulTunerGroupId, tuningInfo->sat.antInfo.info.diseqc.sSatLonggitude);
							if (err)
							{
								HxLOG_Error("PAL_CH_MOTOR_GotoSatellite ret:0x%X\n",err);
							}

							/* 매번 move 하지 않도록 backup */
							pstTunerCtxt->lastMotorDiseqcVer = eDxDISEQC_VER_1_3;
							pstTunerCtxt->sLastSatLonggitude = tuningInfo->sat.antInfo.info.diseqc.sSatLonggitude;
							pstTunerCtxt->ulMotorMovingStartTime = VK_TIMER_GetSystemTick();
						}
						else
						{
							HxLOG_Critical("\n\n");
						}
					}
					else
					{
						/* Motor move 조건이 아니면 그냥 tuning... */
					}

					err = PAL_CH_DISEQC_StartTune(
										(HUINT32)ulTunerGroupId,
										++(pstTunerCtxt->ulRequestId),
										tuningInfo->sat.tuningInfo.ulFrequency, // MHz
										tuningInfo->sat.tuningInfo.ulSymbolRate,
										tuningInfo->sat.tuningInfo.eFecCodeRate,
										tuningInfo->sat.tuningInfo.ePolarization,
										tuningInfo->sat.tuningInfo.eTransSpec,
										tuningInfo->sat.tuningInfo.ePskMod,
										tuningInfo->sat.tuningInfo.ePilot,
										tuningInfo->sat.antInfo.info.diseqc.ulLnbFreq,
										tuningInfo->sat.antInfo.info.diseqc.lnbVoltage,
										tuningInfo->sat.antInfo.info.diseqc.b22kTone,
										tuningInfo->sat.antInfo.info.diseqc.diseqcVersion,
										tuningInfo->sat.antInfo.info.diseqc.diseqcInput,
										tuningInfo->sat.antInfo.info.diseqc.toneBurst,
										tuningInfo->sat.antInfo.info.diseqc.ucMotorPosition,
										tuningInfo->sat.antInfo.info.diseqc.sSatLonggitude
										);
					if (err)
						HxLOG_Error("PAL_CH_DISEQC_StartTune ret error:0x%X\n",err);
					break;

				case eDxANT_TYPE_SCD:
					HxLOG_Print("\t\t==> eDxDELIVERY_TYPE_SAT/eDxANT_TYPE_SCD\n");

					if (ulTunerGroupId == 0)
					{
						ulScdUserBandNum	= tuningInfo->sat.antInfo.info.scd.ulScdUserBandNum;
						ulScdUserBandFreq 	= tuningInfo->sat.antInfo.info.scd.ulScdUserBandFreq;
					}
					else if (ulTunerGroupId == 1)
					{
						ulScdUserBandNum	= tuningInfo->sat.antInfo.info.scd.ulScdUserBandNum1;
						ulScdUserBandFreq 	= tuningInfo->sat.antInfo.info.scd.ulScdUserBandFreq1;
					}
					else if (ulTunerGroupId == 2)
					{
						ulScdUserBandNum	= tuningInfo->sat.antInfo.info.scd.ulScdUserBandNum2;
						ulScdUserBandFreq 	= tuningInfo->sat.antInfo.info.scd.ulScdUserBandFreq2;
					}
					else
					{
						ulScdUserBandNum	= tuningInfo->sat.antInfo.info.scd.ulScdUserBandNum3;
						ulScdUserBandFreq		= tuningInfo->sat.antInfo.info.scd.ulScdUserBandFreq3;
					}

					err = PAL_CH_SCD_StartTune(
										(HUINT32)ulTunerGroupId,
										++(pstTunerCtxt->ulRequestId),
										tuningInfo->sat.tuningInfo.ulFrequency, // MHz
										tuningInfo->sat.tuningInfo.ulSymbolRate,
										tuningInfo->sat.tuningInfo.eFecCodeRate,
										tuningInfo->sat.tuningInfo.ePolarization,
										tuningInfo->sat.tuningInfo.eTransSpec,
										tuningInfo->sat.tuningInfo.ePskMod,
										tuningInfo->sat.tuningInfo.ePilot,
										tuningInfo->sat.antInfo.info.scd.ulLnbFreq,
										tuningInfo->sat.antInfo.info.scd.lnbVoltage,
										tuningInfo->sat.antInfo.info.scd.b22kTone,
										tuningInfo->sat.antInfo.info.scd.scdInput,
										ulScdUserBandNum,
										ulScdUserBandFreq
										);
					if (err)
						HxLOG_Error("PAL_CH_SCD_StartTune ret error:0x%X\n",err);
					break;

				case eDxANT_TYPE_SMATV:
					HxLOG_Print("\t\t==> eDxDELIVERY_TYPE_SAT/eDxANT_TYPE_SMATV\n");

					err = PAL_CH_SMATV_StartTune(
										(HUINT32)ulTunerGroupId,
										++(pstTunerCtxt->ulRequestId),
										tuningInfo->sat.tuningInfo.ulFrequency, // MHz
										tuningInfo->sat.tuningInfo.ulSymbolRate,
										tuningInfo->sat.tuningInfo.eFecCodeRate,
										tuningInfo->sat.tuningInfo.ePolarization,
										tuningInfo->sat.tuningInfo.eTransSpec,
										tuningInfo->sat.tuningInfo.ePskMod,
										tuningInfo->sat.tuningInfo.ePilot
										);
					if (err)
						HxLOG_Error("PAL_CH_SMATV_StartTune ret error:0x%X\n",err);
					break;

				default:
					HxLOG_Print("\t\t==> ERROR in eDxDELIVERY_t [%d] / antenna type[%d]\n", tuningInfo->eDeliType, tuningInfo->sat.antennaType);
					break;
			}

			break;
		}
#endif
#if defined(CONFIG_MW_CH_TERRESTRIAL)
		case eDxDELIVERY_TYPE_TER :
		{
			HxLOG_Print("\t\t==> Terrestrial\n");

 			err = PAL_CH_TER_StartTune(
							(HUINT32)ulTunerGroupId,
							++(pstTunerCtxt->ulRequestId),
							tuningInfo->ter.ulFrequency, // KHz
							tuningInfo->ter.eBandWidth,
							tuningInfo->ter.eConstellation,
							tuningInfo->ter.eHierachy,
							tuningInfo->ter.eCodeRate,
							tuningInfo->ter.eTransMode,
							tuningInfo->ter.eGuardInterval,
							tuningInfo->ter.eStream,
							tuningInfo->ter.eOffset,
							tuningInfo->ter.eSystem,
							tuningInfo->ter.stT2Param.preambleFormat,
							tuningInfo->ter.stT2Param.mixed,
							tuningInfo->ter.stT2Param.pilotPattern,
							tuningInfo->ter.stT2Param.extenedCarrier,
							tuningInfo->ter.stT2Param.PAPRreduction,
							tuningInfo->ter.stT2Param.ulPlpId,/* MPLP : change order of param */
							tuningInfo->ter.stT2Param.numPlpBlocks
						);
			if (err)
				HxLOG_Error("PAL_CH_TER_StartTune ret error:0x%X\n",err);
			break;
		}
#endif

#if defined(CONFIG_MW_CH_CABLE)
		case eDxDELIVERY_TYPE_CAB :
		{
			HxLOG_Print("\t\t==> Cable\n");
			HxLOG_Print("\t\t Freq [%d], SR [%d] \n", tuningInfo->cab.ulFrequency, tuningInfo->cab.ulSymbolRate);

			err = PAL_CH_CAB_StartTune(
							(HUINT32)ulTunerGroupId,
							++(pstTunerCtxt->ulRequestId),
							tuningInfo->cab.ulFrequency, // KHz
							tuningInfo->cab.ulSymbolRate,
							tuningInfo->cab.constellation,
							tuningInfo->cab.spectrum,
							tuningInfo->cab.nTsId,
							tuningInfo->cab.nNetId
							);
			if (err)
				HxLOG_Error("PAL_CH_CAB_StartTune ret error:0x%X\n",err);
			break;
		}
#endif

		default :
			HxLOG_Critical("\n\n");
			err = ERR_FAIL;
			break;
	}

	if (ERR_OK == err)
	{
		pstTunerCtxt->eChState = eSTATE_LOCKING;
		pstTunerCtxt->tuningInfo = *tuningInfo;
		pstTunerCtxt->ulMotorMovingStartTime = 0;
#if defined(CONFIG_MW_CH_AUTOMATIC_RETUNE)
		pstTunerCtxt->bRetuneToAuto = FALSE;
#endif

				/* Unique ID 복사 등록한다. 이 후 ref count가 0가 아니면 이 unique ID가 아닌 request는 받아 주지 않는다. */
		pstTunerCtxt->usUniqueId = usUniqueId;
		pstTunerCtxt->usNextUniqueId = (HUINT16)-1;
	}
	else
	{ /* Force tuner context state to NO_SIGNAL if tuning was failed */
		pstTunerCtxt->eChState = eSTATE_NO_SIGNAL;
	}
}

INLINE static void svc_ch_tunerStop (HUINT32 ulTunerGroupId)
{
	CH_TunerContext_t	*pstTunerCtxt = &(s_TunerContext[ulTunerGroupId]);
	HERROR				 err = ERR_FAIL;

#if defined(CONFIG_MW_CH_AUTOMATIC_RETUNE)
	pstTunerCtxt->bRetuneToAuto = FALSE;
#endif

	switch(pstTunerCtxt->workingDeliveryType)
	{
#if defined(CONFIG_MW_CH_SATELLITE)
		case eDxDELIVERY_TYPE_SAT :
			err = PAL_CH_SAT_StopTune((HUINT32)ulTunerGroupId);
			break;
#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
		case eDxDELIVERY_TYPE_TER :
			err = PAL_CH_TER_StopTune((HUINT32)ulTunerGroupId);
			break;
#endif

#if defined(CONFIG_MW_CH_CABLE)
		case eDxDELIVERY_TYPE_CAB :
			err = PAL_CH_CAB_StopTune((HUINT32)ulTunerGroupId);
			break;
#endif

		default :
			HxLOG_Critical("\n\n");
			break;
	}

	if (ERR_OK == err)
	{
		pstTunerCtxt->eChState = eSTATE_IDLE;
		pstTunerCtxt->usUniqueId = (HUINT16)-1;
		pstTunerCtxt->usNextUniqueId = (HUINT16)-1;
	}
}

/*******************************************************************
************************* Debug / Command **************************
********************************************************************/
#if defined(CONFIG_DEBUG)
static HUINT32						 s_ulNumTesterTp = 0;
static TEST_TP_SWAP_t				 s_astSwapTp[MAX_SWAP_TP];

static HERROR svc_ch_dbg_GetRemapIndexFromSrcFreq (DxDeliveryType_e eDeliType, HUINT32 ulSrcFreq, HUINT32 *pulIndex)
{
	HINT32		 nDiff;
	HUINT32 	 ulIndex;

	for (ulIndex = 0; ulIndex < s_ulNumTesterTp; ulIndex++)
	{
		if (s_astSwapTp[ulIndex].eDeliType != eDeliType)			{ continue; }

		switch (eDeliType)
		{
		case eDxDELIVERY_TYPE_SAT:
		case eDxDELIVERY_TYPE_CAB:
		case eDxDELIVERY_TYPE_TER:
			nDiff = (HINT32)ulSrcFreq - s_astSwapTp[ulIndex].nSrcFreq;
			if (nDiff <= 1 && nDiff >= -1)			// +-1 MHz
			{
				*pulIndex = ulIndex;
				return ERR_OK;
			}
			break;
		default:
			break;
		}
	}

	return ERR_FAIL;
}

static HERROR svc_ch_dbg_GetRemapIndexFromRemappedFreq (DxDeliveryType_e eDeliType, HUINT32 ulRemapFreq, HUINT32 *pulIndex)
{
	HINT32		 nDiff;
	HUINT32 	 ulIndex;

	for (ulIndex = 0; ulIndex < s_ulNumTesterTp; ulIndex++)
	{
		if (s_astSwapTp[ulIndex].eDeliType != eDeliType)			{ continue; }

		switch (eDeliType)
		{
		case eDxDELIVERY_TYPE_SAT:
		case eDxDELIVERY_TYPE_CAB:
		case eDxDELIVERY_TYPE_TER:
			nDiff = (HINT32)ulRemapFreq - s_astSwapTp[ulIndex].nRemapFreq;
			if (nDiff <= 1 && nDiff >= -1)			// +-1 MHz
			{
				*pulIndex = ulIndex;
				return ERR_OK;
			}
			break;
		default:
			break;
		}
	}

	return ERR_FAIL;
}

static HERROR svc_ch_dbg_GetRemappedFreq (DxDeliveryType_e eDeliType, HUINT32 ulSrcFreq, HUINT32 *pulRemapFreq)
{
	HUINT32 	 ulIndex;
	HERROR		 hErr;

	hErr = svc_ch_dbg_GetRemapIndexFromSrcFreq (eDeliType, ulSrcFreq, &ulIndex);
	if (hErr == ERR_OK)
	{
		*pulRemapFreq = (HUINT32)s_astSwapTp[ulIndex].nRemapFreq;
	}

	return hErr;
}

static HERROR svc_ch_dbg_GetOrgFreqFromRemappedFreq (DxDeliveryType_e eDeliType, HUINT32 ulRemapFreq, HUINT32 *pulSrcFreq)
{
	HUINT32 	 ulIndex;
	HERROR		 hErr;

	hErr = svc_ch_dbg_GetRemapIndexFromRemappedFreq (eDeliType, ulRemapFreq, &ulIndex);
	if (hErr == ERR_OK)
	{
		*pulSrcFreq = (HUINT32)s_astSwapTp[ulIndex].nSrcFreq;
	}

	return hErr;
}


STATIC HCHAR *svc_ch_dbg_GetStringOfChannelState (ChannelState_t eState)
{
	switch (eState)
	{
	ENUM_TO_STR (eSTATE_IDLE);
	ENUM_TO_STR (eSTATE_LOCKING);
	ENUM_TO_STR (eSTATE_MOTOR_MOVING);
	ENUM_TO_STR (eSTATE_LOCKED);
	ENUM_TO_STR (eSTATE_NO_SIGNAL);
	default:
		break;
	}

	return "Unknown";
}

STATIC HCHAR *svc_ch_dbg_GetStringOfAntType (DxAntennaType_e eAntType)
{
	switch (eAntType)
	{
	ENUM_TO_STR (eDxANT_TYPE_LNB_ONLY);
	ENUM_TO_STR (eDxANT_TYPE_DISEQC);
	ENUM_TO_STR (eDxANT_TYPE_SCD);
	ENUM_TO_STR (eDxANT_TYPE_SMATV);
	ENUM_TO_STR (eDxANT_TYPE_MONOBLOCK_LNB);
	default:
		break;
	}

	return "Unknown";
}

STATIC void svc_ch_dbg_PrintLnbAntInfo (DxLnbAntenna_t *pstAnt)
{
	if (CH_UNIVERSAL_LNB_FREQ == pstAnt->ulLnbFreq)
	{
		HLIB_CMD_Printf ("	LNB Freq			 (UNIVERSAL)\n");
	}
	else
	{
		HLIB_CMD_Printf ("	LNB Freq			 (%d)\n", pstAnt->ulLnbFreq);
	}

	HLIB_CMD_Printf ("	LNB Voltage 		 (%d)\n", pstAnt->lnbVoltage);
}

STATIC void svc_ch_dbg_PrintDiseqcAntInfo (DxDiseqcAntenna_t *pstAnt)
{
	if (CH_UNIVERSAL_LNB_FREQ == pstAnt->ulLnbFreq)
	{
		HLIB_CMD_Printf ("	LNB Freq			 (UNIVERSAL)\n");
	}
	else
	{
		HLIB_CMD_Printf ("	LNB Freq			 (%d)\n", pstAnt->ulLnbFreq);
	}

	HLIB_CMD_Printf ("	LNB Voltage 		 (%d)\n", pstAnt->lnbVoltage);
	HLIB_CMD_Printf ("	22K Tone			 (%d)\n", pstAnt->b22kTone);
	HLIB_CMD_Printf ("	DiSEqC Version		 (%d)\n", pstAnt->diseqcVersion);
	HLIB_CMD_Printf ("	DiSEqC Input		 (%d)\n", pstAnt->diseqcInput);
	HLIB_CMD_Printf ("	Tone Burst			 (%d)\n", pstAnt->toneBurst);
}

STATIC void svc_ch_dbg_PrintScdAntInfo (DxScdAntenna_t *pstAnt)
{
	if (CH_UNIVERSAL_LNB_FREQ == pstAnt->ulLnbFreq)
	{
		HLIB_CMD_Printf ("	LNB Freq			 (UNIVERSAL)\n");
	}
	else
	{
		HLIB_CMD_Printf ("	LNB Freq			 (%d)\n", pstAnt->ulLnbFreq);
	}

	HLIB_CMD_Printf ("	LNB Voltage 		 (%d)\n", pstAnt->lnbVoltage);
	HLIB_CMD_Printf ("	22K Tone			 (%d)\n", pstAnt->b22kTone);
	HLIB_CMD_Printf ("	SCD Input			 (%d)\n", pstAnt->scdInput);
}

STATIC void svc_ch_dbg_PrintSatTuningInfo (DxSatTuningInfo_t *pstTuneInfo)
{
	HLIB_CMD_Printf ("	TP Type 			 (SAT)\n");
	HLIB_CMD_Printf ("	Antenna Type		 (%s)\n", svc_ch_dbg_GetStringOfAntType (pstTuneInfo->antennaType));

	switch (pstTuneInfo->antennaType)
	{
	case eDxANT_TYPE_LNB_ONLY:
		svc_ch_dbg_PrintLnbAntInfo (&(pstTuneInfo->antInfo.info.lnb));
		break;

	case eDxANT_TYPE_DISEQC:
		svc_ch_dbg_PrintDiseqcAntInfo (&(pstTuneInfo->antInfo.info.diseqc));
		break;

	case eDxANT_TYPE_SCD:
		svc_ch_dbg_PrintScdAntInfo (&(pstTuneInfo->antInfo.info.scd));
		break;

	default:
		break;
	}

	HLIB_CMD_Printf ("	Frequency			 (%d)\n", pstTuneInfo->tuningInfo.ulFrequency);
	HLIB_CMD_Printf ("	Symbol Rate 		 (%d)\n", pstTuneInfo->tuningInfo.ulSymbolRate);
	HLIB_CMD_Printf ("	FEC 				 (%d)\n", pstTuneInfo->tuningInfo.eFecCodeRate);
	HLIB_CMD_Printf ("	Polarization		 (%d)\n", pstTuneInfo->tuningInfo.ePolarization);
}

STATIC void svc_ch_dbg_PrintCabTuningInfo (DxCabTuningInfo_t *pstTuneInfo)
{
	HLIB_CMD_Printf ("	TP Type 			 (CAB)\n");
	HLIB_CMD_Printf ("	Frequency			 (%d)\n", pstTuneInfo->ulFrequency);
	HLIB_CMD_Printf ("	Symbol Rate 		 (%d)\n", pstTuneInfo->ulSymbolRate);
	HLIB_CMD_Printf ("	Constellation		 (%d)\n", pstTuneInfo->constellation);
	HLIB_CMD_Printf ("	Spectrum			 (%d)\n", pstTuneInfo->spectrum);
}

STATIC void svc_ch_dbg_PrintTerTuningInfo (DxTerTuningInfo_t *pstTuneInfo)
{
	HLIB_CMD_Printf ("	TP Type 			 (TER)\n");
	HLIB_CMD_Printf ("	Frequency			 (%d)\n", pstTuneInfo->ulFrequency);
	HLIB_CMD_Printf ("	Bandwidth			 (%d)\n", pstTuneInfo->eBandWidth);
	HLIB_CMD_Printf ("	Constellation		 (%d)\n", pstTuneInfo->eConstellation);
	HLIB_CMD_Printf ("	Hierarchy			 (%d)\n", pstTuneInfo->eHierachy);
	HLIB_CMD_Printf ("	Code Rate			 (%d)\n", pstTuneInfo->eCodeRate);
}

HERROR svc_ch_dbg_AddRemapFreq (DxDeliveryType_e eDeliType, HUINT32 ulSrcFreq, HUINT32 ulRemapFreq)
{
	HUINT32 	 ulIndex;
	HERROR		 hErr;

	hErr = svc_ch_dbg_GetRemapIndexFromSrcFreq (eDeliType, ulSrcFreq, &ulIndex);
	if (hErr == ERR_OK)
	{
		// Modify the remapping data :
		s_astSwapTp[ulIndex].nRemapFreq = (HINT32)ulRemapFreq;
		return ERR_OK;
	}

	if (s_ulNumTesterTp >= MAX_SWAP_TP)
	{
		return ERR_FAIL;
	}

	s_astSwapTp[s_ulNumTesterTp].eDeliType = eDeliType;
	s_astSwapTp[s_ulNumTesterTp].nSrcFreq = (HINT32)ulSrcFreq;
	s_astSwapTp[s_ulNumTesterTp].nRemapFreq = (HINT32)ulRemapFreq;
	s_ulNumTesterTp ++;

	return ERR_OK;
}

HERROR svc_ch_dbg_DeleteRemapFreq (DxDeliveryType_e eDeliType, HUINT32 ulSrcFreq)
{
	HUINT32 	 ulIndex;
	HERROR		 hErr;

	hErr = svc_ch_dbg_GetRemapIndexFromSrcFreq (eDeliType, ulSrcFreq, &ulIndex);
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	for (; ulIndex < s_ulNumTesterTp - 1; ulIndex++)
	{
		s_astSwapTp[ulIndex] = s_astSwapTp[ulIndex + 1];
	}

	s_ulNumTesterTp --;

	return ERR_OK;
}

HERROR svc_ch_dbg_ClearAllRemapFreq (void)
{
	s_ulNumTesterTp = 0;
	return ERR_OK;
}

HUINT32 svc_ch_dbg_CountRemapFreqInfo (void)
{
	return s_ulNumTesterTp;
}

HERROR svc_ch_dbg_GetRemapFreqInfo (HUINT32 ulIndex, DxDeliveryType_e *peDeliType, HUINT32 *pulSrcFreq, HUINT32 *pulRemapFreq)
{
	if (ulIndex >= s_ulNumTesterTp) 		{ return ERR_FAIL; }

	if (peDeliType != NULL)
	{
		*peDeliType = s_astSwapTp[ulIndex].eDeliType;
	}

	if (pulSrcFreq != NULL)
	{
		*pulSrcFreq = (HUINT32)s_astSwapTp[ulIndex].nSrcFreq;
	}

	if (pulRemapFreq != NULL)
	{
		*pulRemapFreq = (HUINT32)s_astSwapTp[ulIndex].nRemapFreq;
	}

	return ERR_OK;
}

static void svc_ch_dbg_PrintTunerStatus (void)
{
	HUINT32 			 ulCnt, ulTunerNum = PAL_CH_GetNumOfTunerGroup();

	HLIB_CMD_Printf ("\n ----- Tuner Status -----\n");

	for (ulCnt = 0; ulCnt < ulTunerNum; ulCnt++)
	{
		CH_TunerContext_t		*pstTunerContext = &(s_TunerContext[ulCnt]);

		HLIB_CMD_Printf ("Tuner (%d):\n", ulCnt);
		HLIB_CMD_Printf ("	DeliveryType		 (0x%08x)\n", pstTunerContext->supportDeliveryType);
		HLIB_CMD_Printf ("	WorkingDeliveryType  (0x%08x)\n", pstTunerContext->workingDeliveryType);
		HLIB_CMD_Printf ("	Channel State		 (%s)\n", svc_ch_dbg_GetStringOfChannelState (pstTunerContext->eChState));
		HLIB_CMD_Printf ("	Action Count		 (%d)\n", svc_ch_countTunedAction(ulCnt));
		HLIB_CMD_Printf ("	Ts(%d)		Next Ts	 (%d)\n", pstTunerContext->usUniqueId,pstTunerContext->usNextUniqueId);

		if (eSTATE_IDLE != pstTunerContext->eChState)
		{
			switch (pstTunerContext->tuningInfo.eDeliType)
			{
			case eDxDELIVERY_TYPE_SAT:
				svc_ch_dbg_PrintSatTuningInfo (&(pstTunerContext->tuningInfo.sat));
				break;

			case eDxDELIVERY_TYPE_CAB:
				svc_ch_dbg_PrintCabTuningInfo (&(pstTunerContext->tuningInfo.cab));
				break;

			case eDxDELIVERY_TYPE_TER:
				svc_ch_dbg_PrintTerTuningInfo (&(pstTunerContext->tuningInfo.ter));
				break;
			default:
				break;

			}
		}

		HLIB_CMD_Printf ("\n\n");
	}

	HLIB_CMD_Printf (" ------------------------\n");
}

static int svc_ch_cmd_MapSatTp (void *szArgList)
{
	HCHAR			*aucArg_1=NULL, *aucArg_2=NULL, *aucArg_3=NULL;
	HCHAR			aucCmd[20], aucStr2[20], aucStr3[20];
	unsigned long		arg2 = 0;
	unsigned long		arg3 = 0;
	HUINT32 			ulRet=0;
	HUINT32 			ulIndex, ulTpNum;
	HUINT32 			ulSrcFreq, ulRemapFreq;
	DxDeliveryType_e	eDeliType;

	/* 1st argument */
	aucArg_1 = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg_1 == NULL )
		return HxCMD_ERR;
	if( sscanf(aucArg_1, "%s", aucCmd) == 0 )
		return HxCMD_ERR;

	HLIB_STD_StrUpper(aucCmd);

	/* 2nd argument */
	aucArg_2 = HLIB_CMD_GetNextArg( (HCHAR**)&szArgList );
	if( aucArg_2 != NULL )
	{
		if( sscanf(aucArg_2, "%s", aucStr2) == 0 )
			return HxCMD_ERR;

		arg2 = atoi(aucStr2);
	}

	/* 3nd argument */
	aucArg_3 = HLIB_CMD_GetNextArg( (HCHAR**)&szArgList );
	if( aucArg_3 != NULL )
	{
		if( sscanf(aucArg_3, "%s", aucStr3) == 0 )
			return HxCMD_ERR;

		arg3 = atoi(aucStr3);
	}

	/* action */
	if (strcmp(aucCmd, "ADD") == 0)
	{
		if ( aucArg_2 == NULL || aucArg_3 == NULL)
		{
			return HxCMD_ERR;
		}
		else
		{
			svc_ch_dbg_AddRemapFreq (eDxDELIVERY_TYPE_SAT, arg2, arg3);
		}
	}
	else if (strcmp(aucCmd, "DEL") == 0)
	{
		if ( aucArg_2 == NULL )
		{
			return HxCMD_ERR;
		}
		else
		{
			svc_ch_dbg_DeleteRemapFreq (eDxDELIVERY_TYPE_SAT, arg2);
		}
	}
	else if(strcmp(aucCmd, "VIEW") == 0)
	{
		ulTpNum = svc_ch_dbg_CountRemapFreqInfo();
		HLIB_CMD_Printf ("--- Remapped TPs : %d ---------------------\n", ulTpNum);
		HLIB_CMD_Printf ("------------------------------------------\n");
		for (ulIndex = 0; ulIndex < ulTpNum; ulIndex++)
		{
			ulRet = svc_ch_dbg_GetRemapFreqInfo (ulIndex, &eDeliType, &ulSrcFreq, &ulRemapFreq);
			if (ulRet == ERR_OK)
			{
				HLIB_CMD_Printf (" [%d] : (%s) %d -> %d\n", ulIndex, OTL_ENUM2STR_DeliveryType(eDeliType), ulSrcFreq, ulRemapFreq);
			}
		}
		HLIB_CMD_Printf ("------------------------------------------\n");
	}
	else if(strcmp(aucCmd, "CLEAR") == 0)
	{
		svc_ch_dbg_ClearAllRemapFreq();
	}
	else
	{
		return HxCMD_ERR;
	}

	return HxCMD_OK;
}

static int svc_ch_cmd_MapCabTp (void *szArgList)
{
	HCHAR			*aucArg_1=NULL, *aucArg_2=NULL, *aucArg_3=NULL;
	HCHAR			aucCmd[20], aucStr2[20], aucStr3[20];
	unsigned long		arg2 = 0;
	unsigned long		arg3 = 0;
	HUINT32 			ulRet=0;
	HUINT32 			ulIndex, ulTpNum;
	HUINT32 			ulSrcFreq, ulRemapFreq;
	DxDeliveryType_e	eDeliType;

	/* 1st argument */
	aucArg_1 = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg_1 == NULL )
		return HxCMD_ERR;
	if( sscanf(aucArg_1, "%s", aucCmd) == 0 )
		return HxCMD_ERR;

	HLIB_STD_StrUpper(aucCmd);

	/* 2nd argument */
	aucArg_2 = HLIB_CMD_GetNextArg( (HCHAR**)&szArgList );
	if( aucArg_2 != NULL )
	{
		if( sscanf(aucArg_2, "%s", aucStr2) == 0 )
			return HxCMD_ERR;

		arg2 = atoi(aucStr2);
	}

	/* 3nd argument */
	aucArg_3 = HLIB_CMD_GetNextArg( (HCHAR**)&szArgList );
	if( aucArg_3 != NULL )
	{
		if( sscanf(aucArg_3, "%s", aucStr3) == 0 )
			return HxCMD_ERR;

		arg3 = atoi(aucStr3);
	}

	/* action */
	if (strcmp(aucCmd, "ADD") == 0)
	{
		if ( aucArg_2 == NULL || aucArg_3 == NULL)
		{
			return HxCMD_ERR;
		}
		else
		{
			svc_ch_dbg_AddRemapFreq (eDxDELIVERY_TYPE_CAB, arg2, arg3);
		}
	}
	else if (strcmp(aucCmd, "DEL") == 0)
	{
		if ( aucArg_2 == NULL )
		{
			return HxCMD_ERR;
		}
		else
		{
			svc_ch_dbg_DeleteRemapFreq (eDxDELIVERY_TYPE_CAB, arg2);
		}
	}
	else if(strcmp(aucCmd, "VIEW") == 0)
	{
		ulTpNum = svc_ch_dbg_CountRemapFreqInfo();
		HLIB_CMD_Printf ("--- Remapped TPs : %d ---------------------\n", ulTpNum);
		HLIB_CMD_Printf ("------------------------------------------\n");
		for (ulIndex = 0; ulIndex < ulTpNum; ulIndex++)
		{
			ulRet = svc_ch_dbg_GetRemapFreqInfo (ulIndex, &eDeliType, &ulSrcFreq, &ulRemapFreq);
			if (ulRet == ERR_OK)
			{
				HLIB_CMD_Printf (" [%d] : (%s) %d -> %d\n", ulIndex, OTL_ENUM2STR_DeliveryType(eDeliType), ulSrcFreq, ulRemapFreq);
			}
		}
		HLIB_CMD_Printf ("------------------------------------------\n");
	}
	else if(strcmp(aucCmd, "CLEAR") == 0)
	{
		svc_ch_dbg_ClearAllRemapFreq();
	}
	else
	{
		return HxCMD_ERR;
	}

	return HxCMD_OK;
}

static int svc_ch_cmd_MapTerTp (void *szArgList)
{
	HCHAR			*aucArg_1=NULL, *aucArg_2=NULL, *aucArg_3=NULL;
	HCHAR			aucCmd[20], aucStr2[20], aucStr3[20];
	unsigned long		arg2 = 0;
	unsigned long		arg3 = 0;
	HUINT32 			ulRet=0;
	HUINT32 			ulIndex, ulTpNum;
	HUINT32 			ulSrcFreq, ulRemapFreq;
	DxDeliveryType_e	eDeliType;

	/* 1st argument */
	aucArg_1 = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg_1 == NULL )
		return HxCMD_ERR;
	if( sscanf(aucArg_1, "%s", aucCmd) == 0 )
		return HxCMD_ERR;

	HLIB_STD_StrUpper(aucCmd);

	/* 2nd argument */
	aucArg_2 = HLIB_CMD_GetNextArg( (HCHAR**)&szArgList );
	if( aucArg_2 != NULL )
	{
		if( sscanf(aucArg_2, "%s", aucStr2) == 0 )
			return HxCMD_ERR;

		arg2 = atoi(aucStr2);
	}

	/* 3nd argument */
	aucArg_3 = HLIB_CMD_GetNextArg( (HCHAR**)&szArgList );
	if( aucArg_3 != NULL )
	{
		if( sscanf(aucArg_3, "%s", aucStr3) == 0 )
			return HxCMD_ERR;

		arg3 = atoi(aucStr3);
	}

	/* action */
	if (strcmp(aucCmd, "ADD") == 0)
	{
		if ( aucArg_2 == NULL || aucArg_3 == NULL)
		{
			return HxCMD_ERR;
		}
		else
		{
			svc_ch_dbg_AddRemapFreq (eDxDELIVERY_TYPE_TER, arg2, arg3);
		}
	}
	else if (strcmp(aucCmd, "DEL") == 0)
	{
		if ( aucArg_2 == NULL )
		{
			return HxCMD_ERR;
		}
		else
		{
			svc_ch_dbg_DeleteRemapFreq (eDxDELIVERY_TYPE_TER, arg2);
		}
	}
	else if(strcmp(aucCmd, "VIEW") == 0)
	{
		ulTpNum = svc_ch_dbg_CountRemapFreqInfo();
		HLIB_CMD_Printf ("--- Remapped TPs : %d ---------------------\n", ulTpNum);
		HLIB_CMD_Printf ("------------------------------------------\n");
		for (ulIndex = 0; ulIndex < ulTpNum; ulIndex++)
		{
			ulRet = svc_ch_dbg_GetRemapFreqInfo (ulIndex, &eDeliType, &ulSrcFreq, &ulRemapFreq);
			if (ulRet == ERR_OK)
			{
				OTL_ENUM2STR_DeliveryType(eDeliType);
				HLIB_CMD_Printf (" [%d] : (%s) %d -> %d\n", ulIndex, eDeliType, ulSrcFreq, ulRemapFreq);
			}
		}
		HLIB_CMD_Printf ("------------------------------------------\n");
	}
	else if(strcmp(aucCmd, "CLEAR") == 0)
	{
		svc_ch_dbg_ClearAllRemapFreq();
	}
	else
	{
		return HxCMD_ERR;
	}

	return HxCMD_OK;
}

static int svc_ch_cmd_PrintTunerStat (void *szArgList)
{
	svc_ch_dbg_PrintTunerStatus();

	return HxCMD_OK;
}

/**********************************************
	Command 수행 함수를 등록하는 통합 등록 함수.
***********************************************/
void svc_ch_RegisterCommand(void)
{
#define	hCmdHandle		"SVC"

	HLIB_CMD_RegisterWord(hCmdHandle,svc_ch_cmd_MapSatTp,
			"mapsattp", 			/* keyword */
			"Remap the satellite TP for the test",		/*	 help  */
			"mapsattp [add/del/view/clear] [src_freq] [remapped_freq]");		/*	usage  */

	HLIB_CMD_RegisterWord(hCmdHandle,svc_ch_cmd_MapCabTp,
			"mapcabtp", 			/* keyword */
			"Remap the cable TP for the test",		/*	 help  */
			"mapcabtp [add/del/view/clear] [src_freq] [remapped_freq]");		/*	usage  */

	HLIB_CMD_RegisterWord(hCmdHandle,svc_ch_cmd_MapTerTp,
			"maptertp", 			/* keyword */
			"Remap the terrestrial TP for the test",		/*	 help  */
			"maptertp [add/del/view/clear] [src_freq] [remapped_freq]");		/*	usage  */

	HLIB_CMD_RegisterWord(hCmdHandle,svc_ch_cmd_PrintTunerStat,
			"tuner_status", 			/* keyword */
			"Print the MW CH Tuner Context Status", 	/*	 help  */
			"tuner_status");		/*	usage  */
}

#endif // CONFIG_DEBUG


/*******************************************************************
**********************		API 정의	****************************
********************************************************************/
void SVC_CH_Init(PostMsgToMgrCb_t fnPostMsgToMgr)
{
	HUINT32 	i;
	HUINT32		ulRet;

	s_fnSvcChPostMsgCb = fnPostMsgToMgr;

	/* Channel service message queue 생성 */
	ulRet = (HUINT32)VK_MSG_Create(SVC_CH_MSGQ_SIZE, sizeof(CH_Msg_t), "ChMsgQ", &s_ulChMsgQId, VK_SUSPENDTYPE_FIFO);
	if (ulRet)
	{
		HxLOG_Error("VK_MSG_Create ret:0x%X\n",ulRet);
	}

	/* Channel service task 생성 */
	ulRet = (HUINT32)VK_TASK_Create(svc_ch_task, SVC_CH_TASK_PRIORITY, SVC_CH_TASK_STACK_SIZE, "tMwCh", NULL, &s_ulChTaskQid, 0);
	if (ulRet)
	{
		HxLOG_Error("VK_TASK_Create ret:0x%X\n",ulRet);
	}

	/* Sync API용 semaphore 생성. */
	ulRet = VK_SEM_Create(&s_ulChSyncSema, "ChSyncSema", VK_SUSPENDTYPE_PRIORITY);
	if (ulRet != ERR_OK)
	{
		HxLOG_Critical("\n\n");
	}
	else
	{
		VK_SEM_Get(s_ulChSyncSema); /* 세마포를 get 해 놓아야 sync API가 동작한다. */
	}

	/* Action context 초기화 */
	for(i = 0; i  < NUM_MAX_ACTION; i++)
	{
		/* Active flag 초기화 */
		s_ActionContext[i].eChState = eSTATE_IDLE;
	}

	/* Tuner context 초기화 */
	s_TunerContext = NULL;
	if (PAL_CH_GetNumOfTunerGroup() > 0)
	{
		s_TunerContext = (CH_TunerContext_t *)OxMW_Malloc (sizeof(CH_TunerContext_t) * PAL_CH_GetNumOfTunerGroup());
		if (s_TunerContext != NULL)
		{
			HxSTD_memset (s_TunerContext, 0, sizeof(CH_TunerContext_t) * PAL_CH_GetNumOfTunerGroup());

			for(i = 0; i < PAL_CH_GetNumOfTunerGroup(); i++)
			{
				PAL_CH_GetSupportDeliveryType(i, &s_TunerContext[i].supportDeliveryType);
				s_TunerContext[i].workingDeliveryType = 0;
				s_TunerContext[i].usUniqueId = -1;
				s_TunerContext[i].usNextUniqueId = -1;
				s_TunerContext[i].eChState = eSTATE_IDLE;
				s_TunerContext[i].ulRequestId = 0;
			}
		}
	}

	/* Tuner 상태 변경을 통보할 callback을 등록한다. */
	PAL_CH_RegisterCheckTunerCallback(svc_ch_tunerEventCallback);

#if defined(CONFIG_MW_CH_CAB_AUTO_SR)
	HxSTD_memset(&s_stAutoSrInfo, 0, sizeof(AUTO_SymbolRateInfo_t));
	for(i = 0; i < NUM_MAX_ACTION; i++)
	{
		s_stAutoSrInfo.stContext[i].eSrMode = eSymbolRateMode_Normal;
		s_stAutoSrInfo.stContext[i].ulCurrIdx = 0;
		s_stAutoSrInfo.stContext[i].eChState = eSTATE_IDLE;
	}
#endif

	VK_TASK_Start(s_ulChTaskQid);

#if defined(CONFIG_DEBUG)
	svc_ch_RegisterCommand();
#endif
}

HERROR SVC_CH_StartTune(Handle_t hAction, SvcRsc_Info_t *pstRscInfo, HUINT16 usUniqueId, DxTuneParam_t *tuningParam)
{
	HUINT32 ulDemuxId;
	HUINT32 ulTunerId;

	ulTunerId = pstRscInfo->astRscItem[eRxRSCTYPE_TUNER_GROUP].ulResourceId;
	ulDemuxId = pstRscInfo->astRscItem[eRxRSCTYPE_DEMUX].ulResourceId;

	PAL_PIPE_SetTunerResource(ulDemuxId, ePAL_PIPE_INPUT_TUNER, ulTunerId);	/* for Demux Optimize */

	return svc_ch_startTuneMsgSend(hAction, pstRscInfo, usUniqueId, tuningParam, TRUE);
}

HERROR SVC_CH_StopTune(Handle_t hAction, HUINT16 usNextUniqueId, ApiSyncMode_t syncMode)
{
	HUINT32		ulActionId = PAL_PIPE_GetActionId(hAction);
	CH_Msg_t	chMsg;
	HUINT32 	ulRet;

	HxLOG_Info("SVC_CH_StopTune(hAction:0x%x, syncMode:%d)\n", hAction, syncMode);

	if(ulActionId >= NUM_MAX_ACTION)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	chMsg.stopMsg.ulCmd = CMD_CH_STOP;
	chMsg.stopMsg.hAction = hAction;
	chMsg.stopMsg.usNextUniqueId = usNextUniqueId;
	chMsg.stopMsg.syncMode = syncMode;

	ulRet = VK_MSG_Send(s_ulChMsgQId, &chMsg, sizeof(CH_Msg_t));
	if(ulRet != ERR_OK)
	{
		//ChErrorPrint(0, ulActionId, ("Error from VK_MSG_Send() in SVC_CH_StopTune().\n"));
		return ERR_FAIL;
	}

	if(syncMode == eSyncMode)
	{
		VK_SEM_Get(s_ulChSyncSema);
	}

	return ERR_OK;
}

#if defined(CONFIG_MW_CH_SATELLITE)
static HERROR svc_ch_getLockedInfo_SAT(HUINT32 ulTunerGroupId, DxTuneParam_t *lockedTuningInfo)
{
	HERROR				err;
	HUINT32 			ulActualTpFreq = 0;
	DxSat_Polarization_e	actualPolar = eDxSAT_POLAR_AUTO;
	DxSat_CodeRate_e		actualCodeRate = eDxSAT_CODERATE_AUTO;
	DxSat_PskModulation_e	actualPskMod = eDxSAT_PSK_AUTO;
	DxSat_Pilot_e 		actualPilot = eDxSAT_PILOT_AUTO;

	/* 위성은 중간 계산이 복잡하므로 일단 내린 파라미터를 복사하고, AUTO로 내려서 바뀌었을 가능성이 있는 파라미터들만 PAL에서 다시 읽어서 준다. */
	*lockedTuningInfo = s_TunerContext[ulTunerGroupId].tuningInfo;

	/* device에서 변경된 부분 update */
	switch (lockedTuningInfo->sat.antennaType)
	{
		case eDxANT_TYPE_LNB_ONLY:
		case eDxANT_TYPE_DISEQC:
		case eDxANT_TYPE_SCD:
			err = PAL_CH_LNB_GetLockedInfo(ulTunerGroupId,
										lockedTuningInfo->sat.tuningInfo.ulFrequency,
										lockedTuningInfo->sat.antInfo.info.lnb.ulLnbFreq, /* 세 안테나의 ulLnbFreq 위치가 같음. */
										&ulActualTpFreq,
										&actualPolar,
										&actualCodeRate,
										&actualPskMod,
										&actualPilot);
			if (err)
			{
				HxLOG_Error("PAL_CH_LNB_GetLockedInfo ret:0x%X\n",err);
			}
			break;

		case eDxANT_TYPE_SMATV:
			err = PAL_CH_SMATV_GetLockedInfo(ulTunerGroupId,
										&ulActualTpFreq,
										&actualPolar,
										&actualCodeRate,
										&actualPskMod,
										&actualPilot);
			if (err)
			{
				HxLOG_Error("PAL_CH_SMATV_GetLockedInfo ret:0x%X\n",err);
			}
			break;

		default:
			err = ERR_FAIL;
			break;
	}

	lockedTuningInfo->sat.tuningInfo.ulFrequency = ulActualTpFreq;
	lockedTuningInfo->sat.tuningInfo.ePolarization = actualPolar;
	lockedTuningInfo->sat.tuningInfo.eFecCodeRate = actualCodeRate;
	lockedTuningInfo->sat.tuningInfo.ePskMod = actualPskMod;
	lockedTuningInfo->sat.tuningInfo.ePilot = actualPilot;
	lockedTuningInfo->sat.tuningInfo.eRollOff = eDxSAT_ROLL_035;

	return err;
}
#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
static HERROR svc_ch_getLockedInfo_TER(HUINT32 ulTunerGroupId, DxTuneParam_t *lockedTuningInfo)
{
	HERROR					err;
	HUINT32 				ulActualFreq;
	DxTer_BandWidth_e 		actualBandWidth;
	DxTer_Constellation_e 	actualConstellation;
	DxTer_Hierachy_e			actualHierachy;
	DxTer_CodeRate_e			actualCodeRate;
	DxTer_TransMode_e 		actualTransMode;
	DxTer_GuardInterval_e 	actualGuardInterval;
	DxTer_Offset_e			actualOffset;
	DxTer_System_e					system;
	DxTer_PreambleFormat_e			preambleFormat;
	DxTer_Mixed_e						mixed;
	DxTer_Pilot_Pattern_e				pilotPattern;
	DxTer_ExtendedCarrier_e			extenedCarrier;
	DxTer_PAPR_Reduction_e			PAPRreduction;
	HUINT32							numPlpBlocks;
	HUINT32							ulPlpId;

	/* 지상파는 대부분의 파라미터가 AUTO를 지원. 그냥 다 DI에서 읽어준다. */
	err = PAL_CH_TER_GetLockedInfo(
							(HUINT32)ulTunerGroupId,
							&ulActualFreq,
							&actualBandWidth,
							&actualConstellation,
							&actualHierachy,
							&actualCodeRate,
							&actualTransMode,
							&actualGuardInterval,
							&actualOffset,
							&system,
							&preambleFormat,
							&mixed,
							&pilotPattern,
							&extenedCarrier,
							&PAPRreduction,
							&ulPlpId,
							&numPlpBlocks
					);
	if(err == ERR_OK)
	{ /* Locked state */
		/* 기본으로 내린 값 반영 */
		*lockedTuningInfo = s_TunerContext[ulTunerGroupId].tuningInfo;

		/* device에서 변경된 부분 update */
		lockedTuningInfo->ter.ulFrequency	= ulActualFreq;
		lockedTuningInfo->ter.eConstellation= actualConstellation;
		lockedTuningInfo->ter.eHierachy		= actualHierachy;
		lockedTuningInfo->ter.eCodeRate		= actualCodeRate;
		lockedTuningInfo->ter.eTransMode	= actualTransMode;
		lockedTuningInfo->ter.eGuardInterval= actualGuardInterval;
		lockedTuningInfo->ter.eOffset		= actualOffset;
		lockedTuningInfo->ter.eBandWidth	= actualBandWidth;
		lockedTuningInfo->ter.eSystem		= system;
		lockedTuningInfo->ter.stT2Param.preambleFormat		= preambleFormat;
		lockedTuningInfo->ter.stT2Param.mixed					= mixed;
		lockedTuningInfo->ter.stT2Param.pilotPattern			= pilotPattern;
		lockedTuningInfo->ter.stT2Param.extenedCarrier		= extenedCarrier;
		lockedTuningInfo->ter.stT2Param.PAPRreduction			= PAPRreduction;
		lockedTuningInfo->ter.stT2Param.numPlpBlocks			= numPlpBlocks;
		//MPLP : Do not update lockedTuningInfo->ter.t2param.ulPlpId				= ulPlpId;

#if 0
		HxLOG_Print("[GetLockedInfo_TER()] ulFrequency 		: %d\n",lockedTuningInfo->ter.ulFrequency);
		HxLOG_Print("[GetLockedInfo_TER()] constellation 		: %d\n",lockedTuningInfo->ter.constellation);
		HxLOG_Print("[GetLockedInfo_TER()] hierachy 			: %d\n",lockedTuningInfo->ter.hierachy);
		HxLOG_Print("[GetLockedInfo_TER()] codeRate 			: %d\n",lockedTuningInfo->ter.codeRate);
		HxLOG_Print("[GetLockedInfo_TER()] transMode 		: %d\n",lockedTuningInfo->ter.transMode);
		HxLOG_Print("[GetLockedInfo_TER()] guardInterval 		: %d\n",lockedTuningInfo->ter.guardInterval);
		HxLOG_Print("[GetLockedInfo_TER()] offset 			: %d\n",lockedTuningInfo->ter.offset);
		HxLOG_Print("[GetLockedInfo_TER()] bandWidth 		: %d\n",lockedTuningInfo->ter.bandWidth);
		HxLOG_Print("[GetLockedInfo_TER()] system 			: %d\n",lockedTuningInfo->ter.system);
		HxLOG_Print("[GetLockedInfo_TER()] preambleFormat 	: %d\n",lockedTuningInfo->ter.t2param.preambleFormat);
		HxLOG_Print("[GetLockedInfo_TER()] mixed 			: %d\n",lockedTuningInfo->ter.t2param.mixed);
		HxLOG_Print("[GetLockedInfo_TER()] pilotPattern 		: %d\n",lockedTuningInfo->ter.t2param.pilotPattern);
		HxLOG_Print("[GetLockedInfo_TER()] extenedCarrier 	: %d\n",lockedTuningInfo->ter.t2param.extenedCarrier);
		HxLOG_Print("[GetLockedInfo_TER()] PAPRreduction 	: %d\n",lockedTuningInfo->ter.t2param.PAPRreduction);
		HxLOG_Print("[GetLockedInfo_TER()] numPlpBlocks 		: %d\n",lockedTuningInfo->ter.t2param.numPlpBlocks);
		HxLOG_Print("[GetLockedInfo_TER()] ulPlpId 			: %d\n",lockedTuningInfo->ter.t2param.ulPlpId);
#endif
	}

	return err;
}

#if defined(CONFIG_MW_CH_DVB_T2)
static HERROR svc_ch_getDataPlpInfo(HUINT32 ulTunerGroupId,  HUINT8 *pucPlpIds, HUINT8 *pucNumPlps)
{
	HERROR					err;

	err = PAL_CH_TER_GetDataPlpInfo(ulTunerGroupId, pucPlpIds,pucNumPlps);
	if(err != ERR_OK)
	{
		HxLOG_Error(0, ulTunerGroupId, ("Error from svc_ch_getDataPlpInfo() : %d\n", err));
		return err;
	}

	return err;
}

static HERROR svc_ch_getDataPlpCount(HUINT32 ulTunerGroupId,  HUINT8 *pucNumPlps)
{
	HERROR					err;

	err = PAL_CH_TER_GetDataPlpCount(ulTunerGroupId, pucNumPlps);
	if(err != ERR_OK)
	{
		HxLOG_Error(0, ulTunerGroupId, ("Error from svc_ch_getDataPlpCount() : %d\n", err));
		return err;
	}

	return err;
}

#endif /* defined(CONFIG_MW_CH_DVB_T2) */
#endif /* defined(CONFIG_MW_CH_TERRESTRIAL) */

#if defined(CONFIG_MW_CH_CABLE)
static HERROR svc_ch_getLockedInfo_CAB(HUINT32 ulTunerGroupId, DxTuneParam_t *lockedTuningInfo)
{
	HERROR				err;
	HUINT32 			actualSymbolRate;
	DxCab_Constellation_e	actualConstellation;
	DxCab_Spectrum_e		actualSpectrum;

	err = PAL_CH_CAB_GetLockedInfo(
								(HUINT32)ulTunerGroupId,
								&actualConstellation,
								&actualSpectrum,
								&actualSymbolRate
								);
	if(err == ERR_OK)
	{ /* Locked state */
		/* 기본으로 내린 값 반영 */
		*lockedTuningInfo = s_TunerContext[ulTunerGroupId].tuningInfo;

		/* device에서 변경된 부분 update */
		lockedTuningInfo->cab.constellation	= actualConstellation;
		lockedTuningInfo->cab.spectrum		= actualSpectrum;
		lockedTuningInfo->cab.ulSymbolRate	= actualSymbolRate;
	}

	return err;
}
#endif

HERROR	SVC_CH_GetNumberOfTunerGroup(HUINT32 *pulTunerNumber)
{
	*pulTunerNumber = PAL_CH_GetNumOfTunerGroup();

	return ERR_OK;
}

HERROR	SVC_CH_GetTunerType(HUINT32 nTunerId, HUINT32 *pulTunerType)
{
	PAL_CH_GetSupportDeliveryType(nTunerId, pulTunerType);

	return ERR_OK;
}

HERROR	SVC_CH_GetLockedInfo(HUINT32 ulActionId, DxTuneParam_t *lockedTuningInfo)
{
	HUINT32	ulTunerGroupId;
	HERROR			err;
#if defined(CONFIG_DEBUG)
	HUINT32			ulSrcFreq;
#endif

	if(ulActionId >= NUM_MAX_ACTION)
	{
		return ERR_FAIL;
	}

	if(s_ActionContext[ulActionId].eChState != eSTATE_IDLE)
	{
		ulTunerGroupId = s_ActionContext[ulActionId].ulTunerGroupId;
		if(ulTunerGroupId >= PAL_CH_GetNumOfTunerGroup())
		{
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}

		if(s_TunerContext[ulTunerGroupId].eChState == eSTATE_LOCKED)
		{
			lockedTuningInfo->eDeliType = s_TunerContext[ulTunerGroupId].workingDeliveryType;

			switch(s_TunerContext[ulTunerGroupId].workingDeliveryType)
			{
#if defined(CONFIG_MW_CH_SATELLITE)
				case eDxDELIVERY_TYPE_SAT :
					err = svc_ch_getLockedInfo_SAT(ulTunerGroupId, lockedTuningInfo);
#if defined(CONFIG_DEBUG)
					if (err == ERR_OK)
					{
						err = svc_ch_dbg_GetOrgFreqFromRemappedFreq (eDxDELIVERY_TYPE_SAT, lockedTuningInfo->sat.tuningInfo.ulFrequency, &ulSrcFreq);
						if (err == ERR_OK)
						{
							lockedTuningInfo->sat.tuningInfo.ulFrequency = ulSrcFreq;
						}

						err = ERR_OK;
					}
#endif


					break;
#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
				case eDxDELIVERY_TYPE_TER :
					err = svc_ch_getLockedInfo_TER(ulTunerGroupId, lockedTuningInfo);
#if defined(CONFIG_DEBUG)
					if (err == ERR_OK)
					{
						err = svc_ch_dbg_GetOrgFreqFromRemappedFreq (eDxDELIVERY_TYPE_TER, lockedTuningInfo->ter.ulFrequency, &ulSrcFreq);
						if (err == ERR_OK)
						{
							lockedTuningInfo->ter.ulFrequency = ulSrcFreq;
						}

						err = ERR_OK;
					}
#endif
					break;
#endif

#if defined(CONFIG_MW_CH_CABLE)
				case eDxDELIVERY_TYPE_CAB :
					err = svc_ch_getLockedInfo_CAB(ulTunerGroupId, lockedTuningInfo);
#if defined(CONFIG_DEBUG)
					if (err == ERR_OK)
					{
						err = svc_ch_dbg_GetOrgFreqFromRemappedFreq (eDxDELIVERY_TYPE_CAB, lockedTuningInfo->cab.ulFrequency, &ulSrcFreq);
						if (err == ERR_OK)
						{
							lockedTuningInfo->cab.ulFrequency = ulSrcFreq;
						}

						err = ERR_OK;
					}
#endif
					break;
#endif

				default :
					HxLOG_Critical("\n\n");
					break;
			}

			return ERR_OK;
		}
		else
		{
			return ERR_FAIL;
		}
	}
	else
	{
		return ERR_FAIL;
	}
}


HERROR	SVC_CH_GetLockedInfoByTunerGroupID(HUINT32 ulTunerGroupId, DxDeliveryType_e eDeliveryType, DxTuneParam_t *lockedTuningInfo)
{
	HERROR		err = ERR_FAIL;
#if defined(CONFIG_DEBUG) && defined(CONFIG_MW_CH_SATELLITE)
	HUINT32		ulSrcFreq;
#endif

	if ((ulTunerGroupId >= PAL_CH_GetNumOfTunerGroup()) || (NULL == lockedTuningInfo))
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	HxSTD_memset(lockedTuningInfo,0,sizeof(DxTuneParam_t));
	if (s_TunerContext[ulTunerGroupId].eChState == eSTATE_LOCKED)
	{
		lockedTuningInfo->eDeliType = s_TunerContext[ulTunerGroupId].workingDeliveryType;

		switch(s_TunerContext[ulTunerGroupId].workingDeliveryType)
		{
#if defined(CONFIG_MW_CH_SATELLITE)
			case eDxDELIVERY_TYPE_SAT :
				err = svc_ch_getLockedInfo_SAT((HUINT32)ulTunerGroupId, lockedTuningInfo);
#if defined(CONFIG_DEBUG)
				if (err == ERR_OK)
				{
					err = svc_ch_dbg_GetOrgFreqFromRemappedFreq (eDxDELIVERY_TYPE_SAT, lockedTuningInfo->sat.tuningInfo.ulFrequency, &ulSrcFreq);
					if (err == ERR_OK)
					{
						lockedTuningInfo->sat.tuningInfo.ulFrequency = ulSrcFreq;
					}

					err = ERR_OK;
				}
#endif


				break;
#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
			case eDxDELIVERY_TYPE_TER :
				err = svc_ch_getLockedInfo_TER((HUINT32)ulTunerGroupId, lockedTuningInfo);
				break;
#endif

#if defined(CONFIG_MW_CH_CABLE)
			case eDxDELIVERY_TYPE_CAB :
				err = svc_ch_getLockedInfo_CAB((HUINT32)ulTunerGroupId, lockedTuningInfo);
				break;
#endif

			default :
				HxLOG_Critical("\n\n");
				break;
		}
		if (err != ERR_OK)
		{
			HxLOG_Error("hErr(%d) ulTunerGroupId(%d) \n",err,ulTunerGroupId);
		}

		return ERR_OK;
	}
	else
	{
		return ERR_FAIL;
	}

}

HERROR SVC_CH_IsIdleTuner (HUINT32 ulTunerGroupId, HBOOL *pbIsIdle)
{

	if ((ulTunerGroupId >= PAL_CH_GetNumOfTunerGroup()) || (NULL == pbIsIdle) || (s_TunerContext == NULL))
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (s_TunerContext[ulTunerGroupId].eChState == eSTATE_IDLE)
	{
		*pbIsIdle = TRUE;
	}
	else
	{
		*pbIsIdle = FALSE;
	}

	return ERR_OK;
}

#if	defined(CONFIG_MW_CH_DVB_T2)
HERROR SVC_CH_GetDataPLPInfo (HUINT32 ulActionId,  HUINT8 *pucPlpIds, HUINT8 *pucNumPlps )
{
	HUINT32	ulTunerGroupId;
	HERROR	hError = ERR_OK;

	if(ulActionId >= NUM_MAX_ACTION)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if(s_ActionContext[ulActionId].eChState != eSTATE_IDLE)
	{
		ulTunerGroupId = s_ActionContext[ulActionId].ulTunerGroupId;
		if(ulTunerGroupId >= PAL_CH_GetNumOfTunerGroup())
		{
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}

		if(s_TunerContext[ulTunerGroupId].eChState == eSTATE_LOCKED)
		{
			hError = svc_ch_getDataPlpInfo(ulTunerGroupId, pucPlpIds,pucNumPlps);
			if(hError != ERR_OK)
			{
				return ERR_FAIL;
			}
		}
	}

	return ERR_OK;
}

HERROR SVC_CH_GetPLPCount (HUINT32 ulActionId,   HUINT8 *pucNumPlps )
{
	HUINT32	ulTunerGroupId;
	HERROR	hError =  ERR_OK;

	if(ulActionId >= NUM_MAX_ACTION)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if(s_ActionContext[ulActionId].eChState != eSTATE_IDLE)
	{
		ulTunerGroupId = s_ActionContext[ulActionId].ulTunerGroupId;
		if(ulTunerGroupId >= PAL_CH_GetNumOfTunerGroup())
		{
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}

		if(s_TunerContext[ulTunerGroupId].eChState == eSTATE_LOCKED)
		{
			hError = svc_ch_getDataPlpCount(ulTunerGroupId, pucNumPlps);
			if(hError != ERR_OK)
			{
				return ERR_FAIL;
			}
		}
	}

	return ERR_OK;
}

#endif

HERROR local_MwCh_GetSignalInfo(HUINT32 ulTunerGroupId, DxDeliveryType_e *deliveryType, CH_SignalInfo_t *signalInfo)
{
	HERROR		err;

	if(ulTunerGroupId >= PAL_CH_GetNumOfTunerGroup())
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if(s_TunerContext[ulTunerGroupId].eChState == eSTATE_LOCKED)
	{
		*deliveryType = s_TunerContext[ulTunerGroupId].workingDeliveryType;

		switch(s_TunerContext[ulTunerGroupId].workingDeliveryType)
		{
#if defined(CONFIG_MW_CH_SATELLITE)
			case eDxDELIVERY_TYPE_SAT :
				err = PAL_CH_SAT_GetSignalInfo(
											(HUINT32)ulTunerGroupId,
											&(signalInfo->signalLevel),
											&(signalInfo->signalQuality),
											&(signalInfo->ulAgc),
											&(signalInfo->fBer),
											&(signalInfo->fSnr),
											&(signalInfo->fSignalInputPower)
											);
				if (err)
				{
					HxLOG_Error("PAL_CH_SAT_GetSignalInfo ret:0x%X\n",err);
				}
				break;
#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
			case eDxDELIVERY_TYPE_TER :
				err = PAL_CH_TER_GetSignalInfo(
											(HUINT32)ulTunerGroupId,
											&(signalInfo->signalLevel),
											&(signalInfo->signalQuality),
											&(signalInfo->ulAgc),
											&(signalInfo->fBer),
											&(signalInfo->fSnr),
											&(signalInfo->fSignalInputPower)
											);
				if (err)
				{
					HxLOG_Error("PAL_CH_TER_GetSignalInfo ret:0x%X\n",err);
				}
				break;
#endif

#if defined(CONFIG_MW_CH_CABLE)
			case eDxDELIVERY_TYPE_CAB :
				err = PAL_CH_CAB_GetSignalInfo(
											(HUINT32)ulTunerGroupId,
											&(signalInfo->signalLevel),
											&(signalInfo->signalQuality),
											&(signalInfo->ulAgc),
											&(signalInfo->fBer),
											&(signalInfo->fSnr),
											&(signalInfo->fSignalInputPower)
											);
				if (err)
				{
					HxLOG_Error("PAL_CH_CAB_GetSignalInfo ret:0x%X\n",err);
				}
				break;
#endif

			default :
				HxLOG_Critical("\n\n");
				break;
		}

		return ERR_OK;
	}
	else
	{
		HxSTD_memset(signalInfo, 0, sizeof(CH_SignalInfo_t));

		return ERR_FAIL;
	}
}


HERROR SVC_CH_GetSignalInfo(HUINT32 ulActionId, DxDeliveryType_e *deliveryType, CH_SignalInfo_t *signalInfo)
{
	HUINT32		ulTunerGroupId;

	if(ulActionId >= NUM_MAX_ACTION)
	{
		HxLOG_Critical("\n\n");
		//ChErrorPrint(ERR_FAIL,ulActionId,("[%s:%d] Invalid Action Id:0x%X!!\n",__FUNCTION__,__LINE__,ulActionId));
		return ERR_FAIL;
	}

	if(s_ActionContext[ulActionId].eChState != eSTATE_IDLE)
	{
		ulTunerGroupId = s_ActionContext[ulActionId].ulTunerGroupId;
		return local_MwCh_GetSignalInfo(ulTunerGroupId, deliveryType, signalInfo);
	}
	else
	{
		//ChErrorPrint(ERR_FAIL,ulActionId,("[%s:%d] s_ActionContext[%d].eChState:0x%X and return ERR!!!!!\n",__FUNCTION__,__LINE__,
//				ulActionId,s_ActionContext[ulActionId].eChState));
		return ERR_FAIL;
	}
}


HERROR SVC_CH_GetSignalInfoByTunerGroupID(HUINT32 ulTunerGroupId, DxDeliveryType_e *deliveryType, CH_SignalInfo_t *signalInfo)
{
	return local_MwCh_GetSignalInfo((HUINT32)ulTunerGroupId, deliveryType, signalInfo);
}

void SVC_CH_SetTERAntennaPower(HBOOL bOnOff)
{
#if defined(CONFIG_MW_CH_TERRESTRIAL)
	HUINT32		i;
	HBOOL		bTerFound = FALSE;

	HxLOG_Info("SVC_CH_SetTERAntennaPower(bOnOff:%d)\n", bOnOff);

	for(i = 0; i < PAL_CH_GetNumOfTunerGroup(); i++)
	{
		if(s_TunerContext[i].supportDeliveryType & eDxDELIVERY_TYPE_TER)
		{
			bTerFound = TRUE;
			break;
		}
	}

	if(bTerFound == TRUE)
	{
		PAL_CH_TER_SetAntennaPower(i, bOnOff);
	}
#endif
}

HERROR SVC_CH_GetTERAntennaPower(HBOOL *pbOnOff)
{
#if defined(CONFIG_MW_CH_TERRESTRIAL)
	HUINT32		i;
	HBOOL		bTerFound = FALSE;

	HxLOG_Info("SVC_CH_GetTERAntennaPower\n");

	for(i = 0; i < PAL_CH_GetNumOfTunerGroup(); i++)
	{
		if(s_TunerContext[i].supportDeliveryType & eDxDELIVERY_TYPE_TER)
		{
			bTerFound = TRUE;
			break;
		}
	}

	if(bTerFound == TRUE)
	{
		return PAL_CH_TER_GetAntennaPower(i, pbOnOff);
	}

	return ERR_FAIL;
#else
	*pbOnOff = FALSE;

	return ERR_FAIL;
#endif

}


#if defined(CONFIG_MW_CH_SATELLITE)
STATIC HERROR svc_ch_checkOneTunner(DxSatTuningInfo_t *pstTuningInfo1, DxSatTuningInfo_t *pstTuningInfo2)
{
	HxLOG_Print("\t[svc_ch_checkOneTunner] (+) \n");

	// 1 tunner 일 경우 TP가 다르면 confilict
	if(pstTuningInfo1->tuningInfo.ulFrequency != pstTuningInfo2->tuningInfo.ulFrequency)
	{
		HxLOG_Print("\t ONE tuner conflict \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_ch_checkLnbOnlyLoopThrough(DxSatTuningInfo_t *pstTuningInfo1, DxSatTuningInfo_t *pstTuningInfo2)
{
	HUINT32		ulLnbFreq1, ulLnbFreq2;

	HxLOG_Info("svc_ch_checkLnbOnlyLoopThrough()\n");

	if(pstTuningInfo1->antInfo.info.lnb.ulLnbFreq == CH_UNIVERSAL_LNB_FREQ)
	{
		ulLnbFreq1 = PAL_CH_LNB_GetActualLnbFreq(pstTuningInfo1->tuningInfo.ulFrequency);
	}
	else
	{
		ulLnbFreq1 = pstTuningInfo1->antInfo.info.lnb.ulLnbFreq;
	}

	if(pstTuningInfo2->antInfo.info.lnb.ulLnbFreq == CH_UNIVERSAL_LNB_FREQ)
	{
		ulLnbFreq2 = PAL_CH_LNB_GetActualLnbFreq(pstTuningInfo2->tuningInfo.ulFrequency);
	}
	else
	{
		ulLnbFreq2 = pstTuningInfo2->antInfo.info.lnb.ulLnbFreq;
	}

	if(	(ulLnbFreq1 == ulLnbFreq2) && /* LNB frequency 같고... */
		(pstTuningInfo1->tuningInfo.ePolarization == pstTuningInfo2->tuningInfo.ePolarization) /* polar가 같으면 diseqc version별로 더 체크해 보자. */
	)
	{
		return ERR_OK;
	}
	else
	{
		HxLOG_Print("\t<LNB Only> Loop through conflict ! ulLnbFreq[%d/%d], polar[%d/%d]\n",
					ulLnbFreq1, ulLnbFreq2, pstTuningInfo1->tuningInfo.ePolarization, pstTuningInfo2->tuningInfo.ePolarization);

		return ERR_FAIL;
	}
}

HERROR svc_ch_checkDiseqcLoopThrough(DxSatTuningInfo_t *pstTuningInfo1, DxSatTuningInfo_t *pstTuningInfo2)
{
	HUINT32		ulLnbFreq1, ulLnbFreq2;
	HBOOL		b22kTone1, b22kTone2;

	HxLOG_Info("svc_ch_checkDiseqcLoopThrough()\n");

	if(pstTuningInfo1->antInfo.info.diseqc.ulLnbFreq == CH_UNIVERSAL_LNB_FREQ)
	{
		ulLnbFreq1 = PAL_CH_LNB_GetActualLnbFreq(pstTuningInfo1->tuningInfo.ulFrequency);
		b22kTone1 = PAL_CH_LNB_GetActual22KTone(pstTuningInfo1->tuningInfo.ulFrequency);
	}
	else if(pstTuningInfo1->antInfo.info.diseqc.ulLnbFreq >= (HUINT32)CH_MDU4_LNB_FREQ && pstTuningInfo1->antInfo.info.diseqc.ulLnbFreq <= (HUINT32)CH_MDU1_LNB_FREQ)
	{
		ulLnbFreq1 = PAL_CH_LNB_GetActualMduLnbFreq(pstTuningInfo1->antInfo.info.diseqc.ulLnbFreq, pstTuningInfo1->tuningInfo.ePolarization, pstTuningInfo1->tuningInfo.ulFrequency);
		b22kTone1 = pstTuningInfo1->antInfo.info.diseqc.b22kTone;
	}
	else
	{
		ulLnbFreq1 = pstTuningInfo1->antInfo.info.diseqc.ulLnbFreq;
		b22kTone1 = pstTuningInfo1->antInfo.info.diseqc.b22kTone;
	}

	if(pstTuningInfo2->antInfo.info.diseqc.ulLnbFreq == CH_UNIVERSAL_LNB_FREQ)
	{
		ulLnbFreq2 = PAL_CH_LNB_GetActualLnbFreq(pstTuningInfo2->tuningInfo.ulFrequency);
		b22kTone2 = PAL_CH_LNB_GetActual22KTone(pstTuningInfo2->tuningInfo.ulFrequency);
	}
	else if(pstTuningInfo2->antInfo.info.diseqc.ulLnbFreq >= (HUINT32)CH_MDU4_LNB_FREQ && pstTuningInfo2->antInfo.info.diseqc.ulLnbFreq <= (HUINT32)CH_MDU1_LNB_FREQ)
	{
		ulLnbFreq2 = PAL_CH_LNB_GetActualMduLnbFreq(pstTuningInfo2->antInfo.info.diseqc.ulLnbFreq, pstTuningInfo2->tuningInfo.ePolarization, pstTuningInfo2->tuningInfo.ulFrequency);
		b22kTone2 = pstTuningInfo2->antInfo.info.diseqc.b22kTone;
	}
	else
	{
		ulLnbFreq2 = pstTuningInfo2->antInfo.info.diseqc.ulLnbFreq;
		b22kTone2 = pstTuningInfo2->antInfo.info.diseqc.b22kTone;
	}

	if( (pstTuningInfo1->antInfo.info.diseqc.diseqcVersion == pstTuningInfo2->antInfo.info.diseqc.diseqcVersion) && /* Diseqc version이 같고... */
		(ulLnbFreq1 == ulLnbFreq2) && /* LNB frequency 같고... */
		(b22kTone1 == b22kTone2) && /* 22kTone 같고... */
		(pstTuningInfo1->tuningInfo.ePolarization == pstTuningInfo2->tuningInfo.ePolarization) /* polar가 같으면 diseqc version별로 더 체크해 보자. */
	)
	{
		switch(pstTuningInfo1->antInfo.info.diseqc.diseqcVersion)
		{
			case eDxDISEQC_VER_1_0 :
			case eDxDISEQC_VER_2_0 :
				if(pstTuningInfo1->antInfo.info.diseqc.diseqcInput != pstTuningInfo2->antInfo.info.diseqc.diseqcInput)
				{
					HxLOG_Print("\t<DiSEqC> Loop through conflict ! Diseqc input[%d/%d]\n",
								pstTuningInfo1->antInfo.info.diseqc.diseqcInput, pstTuningInfo2->antInfo.info.diseqc.diseqcInput);
					return ERR_FAIL;
				}
				break;

			case eDxDISEQC_VER_1_2 :
				if(pstTuningInfo1->antInfo.info.diseqc.ucMotorPosition != pstTuningInfo2->antInfo.info.diseqc.ucMotorPosition)
				{
					HxLOG_Print("\t<DiSEqC> Loop through conflict ! Motor position[%d/%d]\n",
								pstTuningInfo1->antInfo.info.diseqc.ucMotorPosition, pstTuningInfo2->antInfo.info.diseqc.ucMotorPosition);
					return ERR_FAIL;
				}
				break;

			case eDxDISEQC_VER_1_3 :
				if(pstTuningInfo1->antInfo.info.diseqc.sSatLonggitude != pstTuningInfo2->antInfo.info.diseqc.sSatLonggitude)
				{
					HxLOG_Print("\t<DiSEqC> Loop through conflict ! Longgitude[%d/%d]\n",
								pstTuningInfo1->antInfo.info.diseqc.sSatLonggitude, pstTuningInfo2->antInfo.info.diseqc.sSatLonggitude);
					return ERR_FAIL;
				}
				break;

			default :
				HxLOG_Critical("\n\n");
				return ERR_FAIL;
		}
	}
	else
	{ /* Diseqc version 더 볼 필요없이 FAIL */
		HxLOG_Print("\t<Diseqc> Loop through conflict ! Diseqc ver[%d/%d], ulLnbFreq[%d/%d], 22k[%d/%d], polar[%d/%d]\n",
					pstTuningInfo1->antInfo.info.diseqc.diseqcVersion, pstTuningInfo2->antInfo.info.diseqc.diseqcVersion,
					ulLnbFreq1, ulLnbFreq2,
					b22kTone1, b22kTone2,
					pstTuningInfo1->tuningInfo.ePolarization, pstTuningInfo2->tuningInfo.ePolarization);

		return ERR_FAIL;
	}

	return ERR_OK;
}
#endif

HERROR SVC_CH_CheckValidLoopThrough(DxTuneParam_t *pstTuningInfo1, DxTuneParam_t *pstTuningInfo2)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	DxAntennaConnection_e	eAntConnectType;
	HUINT32					ulMaxRfInput = 1, ulMaxTunerNumber = 1;
	HERROR					hErr;
#endif

	HxLOG_Info("(+) \n");

#if defined(CONFIG_MW_CH_SATELLITE)
	if( pstTuningInfo1 == NULL || pstTuningInfo2 == NULL )
		return ERR_FAIL;

	if( (pstTuningInfo1->eDeliType != eDxDELIVERY_TYPE_SAT) || (pstTuningInfo2->eDeliType != eDxDELIVERY_TYPE_SAT) )
	{
		HxLOG_Print("[%s:%d] deliveryType(0x%X, 0x%X)\n",
					__FUNCTION__, __LINE__, pstTuningInfo1->eDeliType, pstTuningInfo2->eDeliType);
		HxLOG_Info("(-) \n");
		return ERR_OK;
	}

	if( pstTuningInfo1->sat.antennaType != pstTuningInfo2->sat.antennaType )
	{
		HxLOG_Print("[%s:%d] antennaType(0x%X, 0x%X)\n",
					__FUNCTION__, __LINE__,  pstTuningInfo1->sat.antennaType,pstTuningInfo2->sat.antennaType);
		HxLOG_Info("(-) \n");
		return ERR_FAIL;
	}

	/* Get Antenna Connection Type */
//	eAntConnectType = MWC_PARAM_GetAntennaConnectionType();
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_ANT_CONN_TYPE, (HUINT32 *)&eAntConnectType, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_ANT_CONN_TYPE) Error!!!\n");
	}

	ulMaxRfInput = SVC_CH_CountSatelliteRfInput();
	SVC_CH_GetNumberOfTunerGroup( &ulMaxTunerNumber );

	if( (PAL_CH_GetNumOfTunerGroup() == 1) || (eAntConnectType == eDxANTCONN_TWOSAME && ulMaxRfInput == ulMaxTunerNumber) )
	{ /* Tuner group이 1개이거나 antenna connection type이 two same이면 항상 OK ==> tuner 개수와 RF Input이 같은 경우만 OK로 변경 */
		HxLOG_Print("\tONE tuner group or TWO-SAME connection. Loop-through OK.\n");

		// 1 tunner 일 경우 체크
		if(PAL_CH_GetNumOfTunerGroup() == 1)
		{
			HxLOG_Info("(-) \n");
			return svc_ch_checkOneTunner(&(pstTuningInfo1->sat), &(pstTuningInfo2->sat));
		}

		HxLOG_Info("(-) \n");
		return ERR_OK;
	}

	if( pstTuningInfo1->sat.antennaType == eDxANT_TYPE_LNB_ONLY )
	{
		HxLOG_Info("(-) \n");
		return svc_ch_checkLnbOnlyLoopThrough(&(pstTuningInfo1->sat), &(pstTuningInfo2->sat));
	}
	else if ( pstTuningInfo1->sat.antennaType == eDxANT_TYPE_DISEQC )
	{
		HxLOG_Info("(-) \n");
		return svc_ch_checkDiseqcLoopThrough(&(pstTuningInfo1->sat), &(pstTuningInfo2->sat));
	}
	else
	{
		/* SCD, SMATV 는 항상 valid함. */
		HxLOG_Print("\tSCD or SMATV is always valid for loop through check.\n");
	}
#endif

	HxLOG_Info("(-) \n");

	return ERR_OK;
}

HERROR SVC_CH_CheckTpConflict (DxTuneParam_t *pstTuningInfo1, DxTuneParam_t *pstTuningInfo2)
{
	HxLOG_Info("SVC_CH_CheckValidLoopThrough()\n");

	if( pstTuningInfo1 == NULL || pstTuningInfo2 == NULL )
		return ERR_FAIL;

	if ((NULL == pstTuningInfo1) || (NULL == pstTuningInfo2))
	{
		return ERR_FAIL;
	}

	if (pstTuningInfo1->eDeliType != pstTuningInfo2->eDeliType)
	{
		return ERR_FAIL;
	}

	if (pstTuningInfo1->eDeliType != eDxDELIVERY_TYPE_SAT)
	{
		HxLOG_Print ("[%s:%d] Delivery type not Satellite: no conflict\n", __FUNCTION__, __LINE__);
		return ERR_OK;
	}

#if defined(CONFIG_MW_CH_SATELLITE)
	if( pstTuningInfo1->sat.antennaType != pstTuningInfo2->sat.antennaType )
	{
		return ERR_FAIL;
	}

	if( pstTuningInfo1->sat.antennaType == eDxANT_TYPE_LNB_ONLY )
	{
		return svc_ch_checkLnbOnlyLoopThrough(&(pstTuningInfo1->sat), &(pstTuningInfo2->sat));
	}
	else if ( pstTuningInfo1->sat.antennaType == eDxANT_TYPE_DISEQC )
	{
		return svc_ch_checkDiseqcLoopThrough(&(pstTuningInfo1->sat), &(pstTuningInfo2->sat));
	}
	else
	{
		/* SCD, SMATV 는 항상 valid함. */
		HxLOG_Print("\tSCD or SMATV is always valid for loop through check.\n");
	}
#endif

	return ERR_OK;
}

#if defined(CONFIG_MW_CH_SATELLITE)
HERROR	SVC_CH_GetSatPositionInfo(HINT32 nSatId, SAT_SatPosInfo_t *pstSatPosInfo)
{
	HINT32	i;
	HINT32	numSat;

	numSat = sizeof(s_stSatPosList) / sizeof(SAT_SatPosInfo_t);

	for(i = 0; i < numSat; i++)
	{
		if(s_stSatPosList[i].nSatId == nSatId)
		{
			pstSatPosInfo->nSatId		= s_stSatPosList[i].nSatId;
			pstSatPosInfo->nPosNum		= s_stSatPosList[i].nPosNum;
			pstSatPosInfo->sSatDegree	= s_stSatPosList[i].sSatDegree;
			pstSatPosInfo->szLongitude	= s_stSatPosList[i].szLongitude;

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

HERROR	SVC_CH_GetSatDiseqcPos(HINT32 nSatId, HINT32 *pnPosNum)
{
	HINT32	i;
	HINT32	numSat;

	numSat = sizeof(s_stSatPosList) / sizeof(SAT_SatPosInfo_t);

	for(i=0; i<numSat; i++)
	{
		if(s_stSatPosList[i].nSatId == nSatId)
		{
			*pnPosNum = s_stSatPosList[i].nPosNum;

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

HERROR	SVC_CH_GetSatDegree(HINT32 nSatId, HINT16 *psSatDegree)
{
	HINT32	i;
	HINT32	numSat;

	numSat = sizeof(s_stSatPosList) / sizeof(SAT_SatPosInfo_t);

	for(i=0; i<numSat; i++)
	{
		if(s_stSatPosList[i].nSatId == nSatId)
		{
			*psSatDegree = s_stSatPosList[i].sSatDegree;

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}
#endif

HERROR SVC_CH_SetAntennaType(DxAntennaConnection_e eAntType)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	return PAL_CH_SAT_SetAntennaType(0, eAntType);
#else
	return ERR_FAIL;
#endif
}

// Motor Functions
HERROR SVC_CH_MOTOR_StopMotor (Handle_t hAction, ApiSyncMode_t syncMode)
{
	CH_Msg_t	chMsg;
	HUINT32 	ulRet;

	chMsg.motorCmdMsg.ulCmd = CMD_CH_MOTOR_STOP;
	chMsg.motorCmdMsg.hAction = hAction;
	chMsg.motorCmdMsg.syncMode = syncMode;

	ulRet = VK_MSG_Send(s_ulChMsgQId, &chMsg, sizeof(CH_Msg_t));
	if(ulRet != ERR_OK)
	{
		HxLOG_Error("VK_MSG_Send ret:0x%X\n",ulRet);
		return ERR_FAIL;
	}

	if(syncMode == eSyncMode)
	{
		VK_SEM_Get(s_ulChSyncSema);
	}

	return ERR_OK;
}

HERROR SVC_CH_MOTOR_GotoPosition (Handle_t hAction, HUINT8 ucPosition, ApiSyncMode_t syncMode)
{
	CH_Msg_t	chMsg;
	HUINT32 	ulRet;

	chMsg.motorCmdMsg.ulCmd = CMD_CH_MOTOR_GOTO_POS;
	chMsg.motorCmdMsg.hAction = hAction;
	chMsg.motorCmdMsg.nArg1 = (HINT32)ucPosition;
	chMsg.motorCmdMsg.syncMode = syncMode;

	ulRet = VK_MSG_Send(s_ulChMsgQId, &chMsg, sizeof(CH_Msg_t));
	if(ulRet != ERR_OK)
	{
		HxLOG_Error("VK_MSG_Send ret:0x%X\n",ulRet);
		return ERR_FAIL;
	}

	if(syncMode == eSyncMode)
	{
		VK_SEM_Get(s_ulChSyncSema);
	}

	return ERR_OK;
}

HERROR SVC_CH_MOTOR_StorePosition (Handle_t hAction, HUINT8 ucPosition, ApiSyncMode_t syncMode)
{
	CH_Msg_t	chMsg;
	HUINT32 	ulRet;

	chMsg.motorCmdMsg.ulCmd = CMD_CH_MOTOR_STORE_POS;
	chMsg.motorCmdMsg.hAction = hAction;
	chMsg.motorCmdMsg.nArg1 = (HINT32)ucPosition;
	chMsg.motorCmdMsg.syncMode = syncMode;

	ulRet = VK_MSG_Send(s_ulChMsgQId, &chMsg, sizeof(CH_Msg_t));
	if(ulRet != ERR_OK)
	{
		HxLOG_Error("VK_MSG_Send ret:0x%X\n",ulRet);
		return ERR_FAIL;
	}

	if(syncMode == eSyncMode)
	{
		VK_SEM_Get(s_ulChSyncSema);
	}

	return ERR_OK;
}

HERROR SVC_CH_MOTOR_Drive (Handle_t hAction, CH_MotorDirection_t direction, CH_MotorDriveMode_t driveMode, HUINT8 ucValue, ApiSyncMode_t syncMode)
{
	CH_Msg_t	chMsg;
	HUINT32 	ulRet;

	chMsg.motorCmdMsg.ulCmd = CMD_CH_MOTOR_DRIVE;
	chMsg.motorCmdMsg.hAction = hAction;
	chMsg.motorCmdMsg.nArg1 = (HINT32)direction;
	chMsg.motorCmdMsg.nArg2 = (HINT32)driveMode;
	chMsg.motorCmdMsg.nArg3 = (HINT32)ucValue;
	chMsg.motorCmdMsg.syncMode = syncMode;

	ulRet = VK_MSG_Send(s_ulChMsgQId, &chMsg, sizeof(CH_Msg_t));
	if(ulRet != ERR_OK)
	{
		HxLOG_Error("VK_MSG_Send ret:0x%X\n",ulRet);
		return ERR_FAIL;
	}

	if(syncMode == eSyncMode)
	{
		VK_SEM_Get(s_ulChSyncSema);
	}

	return ERR_OK;
}

HERROR SVC_CH_MOTOR_SetLimit (Handle_t hAction, CH_MotorDirection_t direction, ApiSyncMode_t syncMode)
{
	CH_Msg_t	chMsg;
	HUINT32 	ulRet;

	chMsg.motorCmdMsg.ulCmd = CMD_CH_MOTOR_SET_LIMIT;
	chMsg.motorCmdMsg.hAction = hAction;
	chMsg.motorCmdMsg.nArg1 = (HINT32)direction;
	chMsg.motorCmdMsg.syncMode = syncMode;

	ulRet = VK_MSG_Send(s_ulChMsgQId, &chMsg, sizeof(CH_Msg_t));
	if(ulRet != ERR_OK)
	{
		HxLOG_Error("VK_MSG_Send ret:0x%X\n",ulRet);
		return ERR_FAIL;
	}

	if(syncMode == eSyncMode)
	{
		VK_SEM_Get(s_ulChSyncSema);
	}

	return ERR_OK;
}

HERROR SVC_CH_MOTOR_DisableLimit (Handle_t hAction, ApiSyncMode_t syncMode)
{
	CH_Msg_t	chMsg;
	HUINT32 	ulRet;

	chMsg.motorCmdMsg.ulCmd = CMD_CH_MOTOR_DISABLE_LIMIT;
	chMsg.motorCmdMsg.hAction = hAction;
	chMsg.motorCmdMsg.syncMode = syncMode;

	ulRet = VK_MSG_Send(s_ulChMsgQId, &chMsg, sizeof(CH_Msg_t));
	if(ulRet != ERR_OK)
	{
		HxLOG_Error("VK_MSG_Send ret:0x%X\n",ulRet);
		return ERR_FAIL;
	}

	if(syncMode == eSyncMode)
	{
		VK_SEM_Get(s_ulChSyncSema);
	}

	return ERR_OK;
}

HERROR SVC_CH_MOTOR_EnableLimit (Handle_t hAction, ApiSyncMode_t syncMode)
{
	CH_Msg_t	chMsg;
	HUINT32 	ulRet;

	chMsg.motorCmdMsg.ulCmd = CMD_CH_MOTOR_ENABLE_LIMIT;
	chMsg.motorCmdMsg.hAction = hAction;
	chMsg.motorCmdMsg.syncMode = syncMode;

	ulRet = VK_MSG_Send(s_ulChMsgQId, &chMsg, sizeof(CH_Msg_t));
	if(ulRet != ERR_OK)
	{
		HxLOG_Error("VK_MSG_Send ret:0x%X\n",ulRet);
		return ERR_FAIL;
	}

	if(syncMode == eSyncMode)
	{
		VK_SEM_Get(s_ulChSyncSema);
	}

	return ERR_OK;
}

HERROR SVC_CH_MOTOR_Recalculate (Handle_t hAction, HUINT8 ucPosition, ApiSyncMode_t syncMode)
{
	CH_Msg_t	chMsg;
	HUINT32 	ulRet;

	chMsg.motorCmdMsg.ulCmd = CMD_CH_MOTOR_RECALC;
	chMsg.motorCmdMsg.hAction = hAction;
	chMsg.motorCmdMsg.nArg1 = (HINT32)ucPosition;
	chMsg.motorCmdMsg.syncMode = syncMode;

	ulRet = VK_MSG_Send(s_ulChMsgQId, &chMsg, sizeof(CH_Msg_t));
	if(ulRet != ERR_OK)
	{
		HxLOG_Error("VK_MSG_Send ret:0x%X\n",ulRet);
		return ERR_FAIL;
	}

	if(syncMode == eSyncMode)
	{
		VK_SEM_Get(s_ulChSyncSema);
	}

	return ERR_OK;
}

HERROR SVC_CH_MOTOR_GotoDegree (Handle_t hAction, HINT16 sDegree, ApiSyncMode_t syncMode)
{
	CH_Msg_t	chMsg;
	HUINT32 	ulRet;

	chMsg.motorCmdMsg.ulCmd = CMD_CH_MOTOR_GOTO_DEGREE;
	chMsg.motorCmdMsg.hAction = hAction;
	chMsg.motorCmdMsg.nArg1 = (HINT32)sDegree;
	chMsg.motorCmdMsg.syncMode = syncMode;

	ulRet = VK_MSG_Send(s_ulChMsgQId, &chMsg, sizeof(CH_Msg_t));
	if(ulRet != ERR_OK)
	{
		HxLOG_Error("VK_MSG_Send ret:0x%X\n",ulRet);
		return ERR_FAIL;
	}

	if(syncMode == eSyncMode)
	{
		VK_SEM_Get(s_ulChSyncSema);
	}

	return ERR_OK;
}

HERROR SVC_CH_MOTOR_GotoSatellite (Handle_t hAction, HINT16 sSatPos, ApiSyncMode_t syncMode)
{
	CH_Msg_t	chMsg;
	HUINT32 	ulRet;

	chMsg.motorCmdMsg.ulCmd = CMD_CH_MOTOR_GOTO_SAT;
	chMsg.motorCmdMsg.hAction = hAction;
	chMsg.motorCmdMsg.nArg1 = (HINT32)sSatPos;
	chMsg.motorCmdMsg.syncMode = syncMode;

	ulRet = VK_MSG_Send(s_ulChMsgQId, &chMsg, sizeof(CH_Msg_t));
	if(ulRet != ERR_OK)
	{
		HxLOG_Error("VK_MSG_Send ret:0x%X\n",ulRet);
		return ERR_FAIL;
	}

	if(syncMode == eSyncMode)
	{
		VK_SEM_Get(s_ulChSyncSema);
	}

	return ERR_OK;
}

HERROR SVC_CH_ResetDiseqc (Handle_t hAction, ApiSyncMode_t syncMode)
{
	CH_Msg_t	chMsg;
	HUINT32 	ulRet;

	chMsg.motorCmdMsg.ulCmd = CMD_CH_RESET_DISEQC;
	chMsg.motorCmdMsg.hAction = hAction;
	chMsg.motorCmdMsg.syncMode = syncMode;

	ulRet = VK_MSG_Send(s_ulChMsgQId, &chMsg, sizeof(CH_Msg_t));
	if(ulRet != ERR_OK)
	{
		HxLOG_Error("VK_MSG_Send ret:0x%X\n",ulRet);
		return ERR_FAIL;
	}

	if(syncMode == eSyncMode)
	{
		VK_SEM_Get(s_ulChSyncSema);
	}

	return ERR_OK;
}

#define _____TUNING_PARAM_CONVERTION_____

#if defined(CONFIG_OTA_IRDETO)
HERROR SVC_CH_MakeDiTuningDataByChTuningParam (DxTuneParam_t *pstChTuneParam,
																	HUINT32 *pulDiDataSize, void **ppvDiParam)
{
	HUINT32			 ulFrequency;
	HERROR			 hErr;

	if (pstChTuneParam == NULL)								{ return ERR_FAIL; }
	if (pulDiDataSize == NULL || ppvDiParam == NULL)		{ return ERR_FAIL; }

	switch (pstChTuneParam->eDeliType)
	{
#if defined(CONFIG_MW_CH_SATELLITE)
	case eDxDELIVERY_TYPE_SAT :
	{
#if defined(CONFIG_DEBUG)
		hErr = svc_ch_dbg_GetRemappedFreq (eDxDELIVERY_TYPE_SAT, pstChTuneParam->sat.tuningInfo.ulFrequency, &ulFrequency);
		if (hErr != ERR_OK)
		{
			ulFrequency = pstChTuneParam->sat.tuningInfo.ulFrequency;
		}
#else
		ulFrequency = pstChTuneParam->sat.tuningInfo.ulFrequency;
#endif

		switch(pstChTuneParam->sat.antennaType)
		{
			case eDxANT_TYPE_LNB_ONLY:
				hErr = PAL_CH_LNB_MakeSatTuneParam(
									ulFrequency,
									pstChTuneParam->sat.tuningInfo.ulSymbolRate,
									pstChTuneParam->sat.tuningInfo.eFecCodeRate,
									pstChTuneParam->sat.tuningInfo.ePolarization,
									pstChTuneParam->sat.tuningInfo.eTransSpec,
									pstChTuneParam->sat.tuningInfo.ePskMod,
									pstChTuneParam->sat.tuningInfo.ePilot,
									pstChTuneParam->sat.antInfo.info.lnb.ulLnbFreq,
									pstChTuneParam->sat.antInfo.info.lnb.lnbVoltage,
									pulDiDataSize,
									ppvDiParam
									);
				if (hErr)
					HxLOG_Error("PAL_CH_LNB_MakeSatTuneParam ret:0x%X\n",hErr);
				break;

			case eDxANT_TYPE_DISEQC:
				hErr = PAL_CH_DISEQC_MakeSatTuneParam(
									ulFrequency,
									pstChTuneParam->sat.tuningInfo.ulSymbolRate,
									pstChTuneParam->sat.tuningInfo.eFecCodeRate,
									pstChTuneParam->sat.tuningInfo.ePolarization,
									pstChTuneParam->sat.tuningInfo.eTransSpec,
									pstChTuneParam->sat.tuningInfo.ePskMod,
									pstChTuneParam->sat.tuningInfo.ePilot,
									pstChTuneParam->sat.antInfo.info.diseqc.ulLnbFreq,
									pstChTuneParam->sat.antInfo.info.diseqc.lnbVoltage,
									pstChTuneParam->sat.antInfo.info.diseqc.b22kTone,
									pstChTuneParam->sat.antInfo.info.diseqc.diseqcVersion,
									pstChTuneParam->sat.antInfo.info.diseqc.diseqcInput,
									pstChTuneParam->sat.antInfo.info.diseqc.toneBurst,
									pstChTuneParam->sat.antInfo.info.diseqc.ucMotorPosition,
									pstChTuneParam->sat.antInfo.info.diseqc.sSatLonggitude,
									pulDiDataSize,
									ppvDiParam
									);
				if (hErr)
					HxLOG_Error("PAL_CH_DISEQC_MakeSatTuneParam ret:0x%X\n",hErr);
				break;

			case eDxANT_TYPE_SCD:
				hErr = PAL_CH_SCD_MakeSatTuneParam(
									ulFrequency,
									pstChTuneParam->sat.tuningInfo.ulSymbolRate,
									pstChTuneParam->sat.tuningInfo.eFecCodeRate,
									pstChTuneParam->sat.tuningInfo.ePolarization,
									pstChTuneParam->sat.tuningInfo.eTransSpec,
									pstChTuneParam->sat.tuningInfo.ePskMod,
									pstChTuneParam->sat.tuningInfo.ePilot,
									pstChTuneParam->sat.antInfo.info.scd.ulLnbFreq,
									pstChTuneParam->sat.antInfo.info.scd.lnbVoltage,
									pstChTuneParam->sat.antInfo.info.scd.b22kTone,
									pstChTuneParam->sat.antInfo.info.scd.scdInput,
									pstChTuneParam->sat.antInfo.info.scd.ulScdUserBandNum,
									pstChTuneParam->sat.antInfo.info.scd.ulScdUserBandFreq,
									pulDiDataSize,
									ppvDiParam
									);
				if (hErr)
					HxLOG_Error("PAL_CH_SCD_MakeSatTuneParam ret:0x%X\n",hErr);
				break;

			case eDxANT_TYPE_SMATV:
				hErr = PAL_CH_SMATV_MakeSatTuneParam(
									ulFrequency,
									pstChTuneParam->sat.tuningInfo.ulSymbolRate,
									pstChTuneParam->sat.tuningInfo.eFecCodeRate,
									pstChTuneParam->sat.tuningInfo.ePolarization,
									pstChTuneParam->sat.tuningInfo.eTransSpec,
									pstChTuneParam->sat.tuningInfo.ePskMod,
									pstChTuneParam->sat.tuningInfo.ePilot,
									pulDiDataSize,
									ppvDiParam
									);
				if (hErr)
					HxLOG_Error("PAL_CH_SMATV_MakeSatTuneParam ret:0x%X\n",hErr);
				break;

			default:
				HxLOG_Error("pstChTuneParam->sat.antennaType is unknown:0x%X\n",pstChTuneParam->sat.antennaType);
				hErr = ERR_FAIL;
				break;
		}

		break;
	}
#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
	case eDxDELIVERY_TYPE_TER :
	{
#if defined(CONFIG_DEBUG)
		hErr = svc_ch_dbg_GetRemappedFreq (eDxDELIVERY_TYPE_TER, pstChTuneParam->ter.ulFrequency, &ulFrequency);
		if (hErr != ERR_OK)
		{
			ulFrequency = pstChTuneParam->ter.ulFrequency;
		}
#else
		ulFrequency = pstChTuneParam->ter.ulFrequency;
#endif

		hErr = PAL_CH_TER_MakeTerTuneParam(
						ulFrequency,
						pstChTuneParam->ter.bandWidth,
						pstChTuneParam->ter.constellation,
						pstChTuneParam->ter.hierachy,
						pstChTuneParam->ter.codeRate,
						pstChTuneParam->ter.transMode,
						pstChTuneParam->ter.guardInterval,
						pstChTuneParam->ter.stream,
						pstChTuneParam->ter.offset,
						pstChTuneParam->ter.system,
						pstChTuneParam->ter.t2param.ulPlpId,
						pstChTuneParam->ter.t2param.preambleFormat,
						pstChTuneParam->ter.t2param.mixed,
						pstChTuneParam->ter.t2param.pilotPattern,
						pstChTuneParam->ter.t2param.extenedCarrier,
						pstChTuneParam->ter.t2param.PAPRreduction,
						pstChTuneParam->ter.t2param.numPlpBlocks,
						pulDiDataSize,
						ppvDiParam
						);
		if (hErr)
			HxLOG_Error("PAL_CH_TER_MakeTerTuneParam ret:0x%X\n",hErr);
		break;
	}
#endif

#if defined(CONFIG_MW_CH_CABLE)
	case eDxDELIVERY_TYPE_CAB :
	{
#if defined(CONFIG_DEBUG)
		hErr = svc_ch_dbg_GetRemappedFreq (eDxDELIVERY_TYPE_CAB, pstChTuneParam->cab.ulFrequency, &ulFrequency);
		if (hErr != ERR_OK)
		{
			ulFrequency = pstChTuneParam->cab.ulFrequency;
		}
#else
		ulFrequency = pstChTuneParam->cab.ulFrequency;
#endif

		hErr = PAL_CH_CAB_MakeCabTuneParam(
						ulFrequency,
						pstChTuneParam->cab.ulSymbolRate,
						pstChTuneParam->cab.constellation,
						pstChTuneParam->cab.spectrum,
						pulDiDataSize,
						ppvDiParam
						);
		if (hErr)
			HxLOG_Error("PAL_CH_CAB_MakeCabTuneParam ret:0x%X\n",hErr);
		break;
	}
#endif

	default :
		HxLOG_Critical("\n\n");
		hErr = ERR_FAIL;
		break;
	}

	return hErr;
}

HERROR SVC_CH_FreeDiTuningData (void *pvData)
{
	if (pvData != NULL)
	{
		OxMW_Free (pvData);
	}

	return ERR_OK;
}
#endif

HERROR SVC_CH_GetRequestedTuneInfo(HUINT32 ulTunerGroup, DxTuneParam_t* pstTuneInfo)
{
	if(pstTuneInfo == NULL)
		return ERR_FAIL;

	if(ulTunerGroup >= PAL_CH_GetNumOfTunerGroup())
	{
		HxLOG_Info("[SVC_CH_GetRequestedTuneInfo] ERROR : invalid TunerGroup(%d) \n", ulTunerGroup);
		return ERR_FAIL;
	}

	HxSTD_memcpy(pstTuneInfo, &s_TunerContext[ulTunerGroup].tuningInfo, sizeof(DxTuneParam_t));

	return ERR_OK;
}


HERROR SVC_CH_RegisterAutoSRList(HUINT32 ulSymbolRateNum, HUINT32 *aulSymbolRateList)
{
#if defined(CONFIG_MW_CH_CAB_AUTO_SR)
	if (ulSymbolRateNum == 0 || ulSymbolRateNum > MAX_AUTO_SR_NUM)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	HxLOG_Info("[SVC_CH_RegisterAutoSRList] ulSymbolRateNum [%d], aulSymbolRateList[0] : [%d]\n", ulSymbolRateNum, aulSymbolRateList[0]);

	HxSTD_memset(&s_stAutoSrInfo, 0, sizeof(AUTO_SymbolRateInfo_t));
	s_stAutoSrInfo.ulNumSymbolRate = ulSymbolRateNum;
	HxSTD_memcpy(s_stAutoSrInfo.aulSymbolRateList, aulSymbolRateList, sizeof(HUINT32)*ulSymbolRateNum);

#if defined(CONFIG_DEBUG)
	{
		HUINT32 i;
		HxLOG_Info("[SVC_CH_RegisterAutoSRList] Auto Symbol-Rate List \n");
		for (i = 0; i < s_stAutoSrInfo.ulNumSymbolRate; i++)
		{
			HxLOG_Info("Symbol-Rate [%02d] : [%d ms]\n", i, s_stAutoSrInfo.aulSymbolRateList[i]);
		}
	}
#endif
	return ERR_OK;
#else
	HxLOG_Critical("\n\n");
	return ERR_FAIL;
#endif
}


#define _____SATELLITE_RF_INPUT_____
HUINT32 SVC_CH_CountSatelliteRfInput (void)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	return PAL_CH_SAT_CountRfInput();
#else
	return 0;
#endif
}

HERROR SVC_CH_GetSatelliteRfInput (HUINT32 ulRfInputIdx, CH_SatRfInput_t *pstRfInput)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	PalCh_SatRfInput_t	 stPalSatRfInput;
	HERROR				 hErr;

	if (NULL == pstRfInput)						{ return ERR_FAIL; }
	HxSTD_MemSet (pstRfInput, 0, sizeof(CH_SatRfInput_t));

	hErr = PAL_CH_SAT_GetRfInput (ulRfInputIdx, &stPalSatRfInput);
	if (ERR_OK == hErr)
	{
		pstRfInput->bAvailable				= stPalSatRfInput.bAvailable;
		pstRfInput->ulConnectableTunerNum	= (stPalSatRfInput.ulConnectableTunerNum > SVC_CH_MAX_TUNER_NUM) ? SVC_CH_MAX_TUNER_NUM : stPalSatRfInput.ulConnectableTunerNum;
		HxSTD_MemCopy (pstRfInput->aulConnectableTunerId, stPalSatRfInput.aulConnectableTunerId, sizeof(HUINT32) * pstRfInput->ulConnectableTunerNum);
		pstRfInput->ulConnectedTunerNum		= (stPalSatRfInput.ulConnectedTunerNum > SVC_CH_MAX_TUNER_NUM) ? SVC_CH_MAX_TUNER_NUM : stPalSatRfInput.ulConnectedTunerNum;
		HxSTD_MemCopy (pstRfInput->aulConnectableTunerId, stPalSatRfInput.aulConnectedTunerId, sizeof(HUINT32) * pstRfInput->ulConnectedTunerNum);

		return ERR_OK;
	}
#endif

	return ERR_FAIL;
}


