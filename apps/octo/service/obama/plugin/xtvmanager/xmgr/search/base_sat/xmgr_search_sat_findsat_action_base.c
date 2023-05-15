/**
	@file     ap_sat_search_svc.c
	@brief    file_name & simple comment.

	Description: Applicatuion for searching Services\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2008/01/15		initiated					JK Baek\n
	 2008/03/14		renamed					wjmoh\n
	 2008/09/05		Satellite part is divided		jinh\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <linkedlist.h>

#include <db_common.h>
#include <db_svc.h>
#include <db_param.h>

#include <otl.h>

#include <svc_pipe.h>
#include <svc_resources.h>
#include <svc_ch.h>
#include <svc_si.h>
#include <svc_si.h>

#include <isosvc.h>

#include <bus.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_antenna.h>
#include <mgr_spec.h>
#include <mgr_search.h>
#include <mgr_rscmap.h>
#include <mgr_time.h>
#include <xmgr_tplist.h>
#include "../local_include/_xmgr_search.h"

#include <hlib.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/




#define MAX_SVCNUM_IN_CHECK				16
#define MAX_LENGTH_FOR_VERSION			8

#if !defined(WIN32)
#define FINDSAT_CHECKLIST_FILE			(HUINT8 *)OBAMA_DIR_DATA "/findsat.xml"
#else
#define FINDSAT_CHECKLIST_FILE			(HUINT8 *)"./findsat.xml"
#endif

// Check List를 현재 DB로부터 가져 오는가, proc create 시에 argument로 받는가의 구조
// 현재는 Freesat 만 enable 시키도록 한다
//#ifdef CONFIG_OP_FREESAT
#if !defined(CONFIG_OP_MIDDLE_EAST)
#define	FINDSAT_CHECKLIST_ARGUMENT
#endif

// default 로 적용해야하나..
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE) || defined(CONFIG_PROD_HMS1000T) || defined(CONFIG_PROD_ICORDPRO) || defined(CONFIG_PROD_HDNEO) || defined(CONFIG_PROD_HGS1000S)// || defined(CONFIG_PROD_IR4000HD)
#define SUPPORT_TIME_RECOVERY
#endif


//#endif

// XML TAGs
#define FINDSAT_XML_TAG_VERSION			(HUINT8 *)"data_ver"
#define FINDSAT_XML_ATTR_VERNUM			(HUINT8 *)"vernum"

#define FINDSAT_XML_TAG_SAT				(HUINT8 *)"sat"
#define FINDSAT_XML_TAG_TS				(HUINT8 *)"ts"
#define FINDSAT_XML_TAG_SVC				(HUINT8 *)"svc"

#define FINDSAT_XML_ATTR_SATTYPE		(HUINT8 *)"type"
#define FINDSAT_XML_ATTR_TSFREQ			(HUINT8 *)"freq"
#define FINDSAT_XML_ATTR_TSPOLAR		(HUINT8 *)"pol"
#define FINDSAT_XML_ATTR_TSSR			(HUINT8 *)"symbol"
#define FINDSAT_XML_ATTR_TSFEC			(HUINT8 *)"fec"
#define FINDSAT_XML_ATTR_TSPSK			(HUINT8 *)"psk"
#define FINDSAT_XML_ATTR_TSNETID		(HUINT8 *)"netid"
#define FINDSAT_XML_ATTR_TS_ONID		(HUINT8 *)"onid"
#define FINDSAT_XML_ATTR_TS_TSID		(HUINT8 *)"tsid"
#define FINDSAT_XML_ATTR_SVCID			(HUINT8 *)"svcid"
#define FINDSAT_XML_ATTR_TSTRANS		(HUINT8 *)"trans"



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	eSatFindState_Start		 			= 0,
	eSatFindState_Tuning,
	eSatFindState_WaitingSi,

	eSatFindState_Finish
} SatFind_State_t;

typedef struct
{
	DbSvc_TuningInfo_t	 stTuneInfo;
	HUINT32				 ulNetworkId;
	HUINT32				 ulOnId, ulTsId;
	HUINT32				 ulSvcNum;
	HUINT16				 ausSvcId[MAX_SVCNUM_IN_CHECK];
} TsCheckList_t;

typedef struct
{
	SVC_SatType_t		 eSatType;

	HUINT32				 ulNumTp;
	TsCheckList_t		*pstTsCheckArray;

} SatCheckList_t;

typedef struct tagFindSat_Contents_t
{
	HBOOL					bActionTaken;
	HBOOL					bActionFound;

	HUINT32					ulActionId;
	Handle_t				 	hAction;
	HUINT32					ulTsPortId;

	SatFind_State_t			eState;
	DbSvc_AntInfo_t				stAntInfo;

	// Check Lists
	HUINT32					ulNumCheckSat;
	SatCheckList_t			*pstSatCheckArray;

	// Check Status
	HUINT32					ulCurrSatCnt, ulCurrTsCnt;
	HUINT32					ulSuccessTpCnt;

	// Check Result :
	SVC_SatType_t			aeSatType[4];
} FindSat_Contents_t;

typedef struct {
	char	*szArg;
	HUINT32	 ulValue;
} ArgConvTable_t;


static FindSat_Contents_t		*s_pstFindSat_Contents = NULL;


#if !defined(FINDSAT_CHECKLIST_ARGUMENT)
// SAT Type
static ArgConvTable_t			 s_astFindSat_SatTypeConvTable[] =
{
#if defined(CONFIG_OP_FREESAT)
	{ "ASTRA2", 		eSatType_ASTRA_2 },
#elif defined(CONFIG_OP_MIDDLE_EAST)
	{ "NILESAT",		eSatType_NILESAT_101_102 },
	{ "BADR",			eSatType_BADR_4_6},
	{ "HOTBIRD",		eSatType_HOTBIRD },
	{ "YAHLIVE", 		eSatType_YAHLIVE },
#else
	{ "ASTRA1",			eSatType_ASTRA_1 },
	{ "ASTRA2", 		eSatType_ASTRA_2 },
	{ "HOTBIRD",		eSatType_HOTBIRD },
#endif
	{ NULL,				0 },
};

// TS POLAR
static ArgConvTable_t			 s_astFindSat_TsPolConvTable[] =
{
	{ "H",			eDxSAT_POLAR_HOR },
	{ "V",			eDxSAT_POLAR_VER },
	{ NULL,			0 },
};

// TS FEC
static ArgConvTable_t			 s_astFindSat_TsFecConvTable[] =
{
	{ "1_2",		eDxSAT_CODERATE_1_2 },
	{ "2_3",		eDxSAT_CODERATE_2_3 },
	{ "3_4",		eDxSAT_CODERATE_3_4 },
	{ "5_6",		eDxSAT_CODERATE_5_6 },
	{ "7_8",		eDxSAT_CODERATE_7_8 },
	{ "3_5",		eDxSAT_CODERATE_3_5 },
	{ "4_5",		eDxSAT_CODERATE_4_5 },
	{ "5_11",		eDxSAT_CODERATE_5_11 },
	{ "6_7",		eDxSAT_CODERATE_6_7 },
	{ "8_9",		eDxSAT_CODERATE_8_9 },
	{ "9_10",		eDxSAT_CODERATE_9_10 },
	{ "AUTO",		eDxSAT_CODERATE_AUTO },
	{ NULL, 		0 },
};

// TS PSK
static ArgConvTable_t			 s_astFindSat_TsPskConvTable[] =
{
	{ "QPSK",		eDxSAT_PSK_QPSK },
	{ "8PSK",		eDxSAT_PSK_8PSK },
	{ NULL, 		0 },
};
#endif

// 'findsat.xml' Binary Data

// ===================================================================
// Hard Coding 된 Version
// ===================================================================
#define FINDSAT_VER_HARDCODED			0x0001

#if defined(CONFIG_OP_FREESAT)
const static char s_aucFindSat_DefaultFileData [] =
{
	0x3C,0x3F,0x78,0x6D,0x6C,0x20,0x76,0x65,0x72,0x73,0x69,0x6F,0x6E,0x3D,0x22,0x31,
	0x2E,0x30,0x22,0x20,0x65,0x6E,0x63,0x6F,0x64,0x69,0x6E,0x67,0x3D,0x22,0x49,0x53,
	0x4F,0x2D,0x38,0x38,0x35,0x39,0x2D,0x39,0x22,0x20,0x3F,0x3E,0x0D,0x0A,0x3C,0x63,
	0x68,0x65,0x63,0x6B,0x6C,0x69,0x73,0x74,0x3E,0x0D,0x0A,0x0D,0x0A,0x3C,0x64,0x61,
	0x74,0x61,0x5F,0x76,0x65,0x72,0x20,0x76,0x65,0x72,0x6E,0x75,0x6D,0x3D,0x22,0x31,
	0x22,0x3E,0x0D,0x0A,0x3C,0x2F,0x64,0x61,0x74,0x61,0x5F,0x76,0x65,0x72,0x3E,0x0D,
	0x0A,0x0D,0x0A,0x3C,0x73,0x61,0x74,0x20,0x74,0x79,0x70,0x65,0x3D,0x22,0x61,0x73,
	0x74,0x72,0x61,0x32,0x22,0x3E,0x0D,0x0A,0x09,0x3C,0x74,0x73,0x20,0x66,0x72,
	0x65,0x71,0x3D,0x22,0x31,0x31,0x34,0x32,0x38,0x22,0x20,0x70,0x6F,0x6C,0x3D,0x22,
	0x68,0x22,0x20,0x73,0x79,0x6D,0x62,0x6F,0x6C,0x3D,0x22,0x32,0x37,0x35,0x30,0x30,
	0x22,0x20,0x66,0x65,0x63,0x3D,0x22,0x32,0x5F,0x33,0x22,0x20,0x70,0x73,0x6B,0x3D,
	0x22,0x71,0x70,0x73,0x6B,0x22,0x20,0x6F,0x6E,0x69,0x64,0x3D,0x22,0x35,0x39,0x22,
	0x20,0x74,0x73,0x69,0x64,0x3D,0x22,0x32,0x33,0x31,0x35,0x22,0x3E,0x0D,0x0A,0x09,
	0x3C,0x2F,0x74,0x73,0x3E,0x0D,0x0A,0x09,0x3C,0x74,0x73,0x20,0x66,0x72,0x65,0x71,
	0x3D,0x22,0x31,0x30,0x37,0x37,0x33,0x22,0x20,0x70,0x6F,0x6C,0x3D,0x22,0x68,0x22,
	0x20,0x73,0x79,0x6D,0x62,0x6F,0x6C,0x3D,0x22,0x32,0x32,0x30,0x30,0x30,0x22,0x20,
	0x66,0x65,0x63,0x3D,0x22,0x35,0x5F,0x36,0x22,0x20,0x70,0x73,0x6B,0x3D,0x22,0x71,
	0x70,0x73,0x6B,0x22,0x20,0x6F,0x6E,0x69,0x64,0x3D,0x22,0x32,0x22,0x20,0x74,0x73,
	0x69,0x64,0x3D,0x22,0x32,0x30,0x34,0x35,0x22,0x3E,0x0D,0x0A,0x09,0x3C,0x2F,0x74,
	0x73,0x3E,0x0D,0x0A,0x3C,0x2F,0x73,0x61,0x74,0x3E,0x0D,0x0A,0x0D,0x0A,0x0D,0x0A,
	0x3C,0x2F,0x63,0x68,0x65,0x63,0x6B,0x6C,0x69,0x73,0x74,0x3E,0x0D,0x0A,0x0D,0x0A
};
/* end binary data. size = 532 bytes */

#elif defined(CONFIG_OP_MIDDLE_EAST)
/*
<?xml version="1.0" encoding="ISO-8859-9" ?><checklist><data_ver vernum="1"></data_ver><sat type="nilesat">	<ts freq="10796" pol="v" symbol="27500" fec="3_4" psk="qpsk" onid="2048" tsid="36" trans="0">	</ts>	<ts freq="11373" pol="h" symbol="27500" fec="2_3" psk="8psk" onid="2048" tsid="45" trans="0">	</ts>	<ts freq="11938" pol="v" symbol="27500" fec="3_4" psk="qpsk" onid="2048" tsid="28" trans="0">	</ts>	<ts freq="11766" pol="h" symbol="27500" fec="5_6" psk="qpsk" onid="2048" tsid="5" trans="0">	</ts></sat><sat type="badr">	<ts freq="10730" pol="v" symbol="27500" fec="3_4" psk="qpsk" onid="702" tsid="1" trans="0">	</ts>	<ts freq="10770" pol="h" symbol="27500" fec="3_4" psk="qpsk" onid="1" tsid="1" trans="0">	</ts>	<ts freq="11919" pol="h" symbol="27500" fec="3_4" psk="qpsk" onid="702" tsid="512" trans="0">	</ts>	<ts freq="12476" pol="v" symbol="27500" fec="3_4" psk="qpsk" onid="702" tsid="559" trans="0">	</ts></sat><sat type="hotbird">	<ts freq="11054" pol="h" symbol="27500" fec="5_6" psk="qpsk" onid="318" tsid="12700" trans="0">	</ts>	<ts freq="11240" pol="v" symbol="27500" fec="3_4" psk="qpsk" onid="318" tsid="200" trans="0">	</ts>	<ts freq="11766" pol="v" symbol="27500" fec="2_3" psk="qpsk" onid="318" tsid="5200" trans="0">	</ts>	<ts freq="12015" pol="h" symbol="27500" fec="3_4" psk="qpsk" onid="318" tsid="6500" trans="0">	</ts></sat><sat type="yahlive">	<ts freq="11785" pol="h" symbol="27500" fec="5_6" psk="qpsk" onid="525" tsid="4" trans="1">	</ts>	<ts freq="12034" pol="v" symbol="27500" fec="2_3" psk="8psk" onid="1" tsid="1" trans="1">	</ts></sat></checklist>

*/
const static char s_aucFindSat_DefaultFileData [] =
{
	0x3C, 0x3F, 0x78, 0x6D, 0x6C, 0x20, 0x76, 0x65, 0x72, 0x73, 0x69, 0x6F, 0x6E, 0x3D, 0x22, 0x31,
	0x2E, 0x30, 0x22, 0x20, 0x65, 0x6E, 0x63, 0x6F, 0x64, 0x69, 0x6E, 0x67, 0x3D, 0x22, 0x49, 0x53,
	0x4F, 0x2D, 0x38, 0x38, 0x35, 0x39, 0x2D, 0x39, 0x22, 0x20, 0x3F, 0x3E, 0x0A, 0x3C, 0x63, 0x68,
	0x65, 0x63, 0x6B, 0x6C, 0x69, 0x73, 0x74, 0x3E, 0x0A, 0x0A, 0x3C, 0x64, 0x61, 0x74, 0x61, 0x5F,
	0x76, 0x65, 0x72, 0x20, 0x76, 0x65, 0x72, 0x6E, 0x75, 0x6D, 0x3D, 0x22, 0x31, 0x22, 0x3E, 0x0A,
	0x3C, 0x2F, 0x64, 0x61, 0x74, 0x61, 0x5F, 0x76, 0x65, 0x72, 0x3E, 0x0A, 0x0A, 0x3C, 0x73, 0x61,
	0x74, 0x20, 0x74, 0x79, 0x70, 0x65, 0x3D, 0x22, 0x6E, 0x69, 0x6C, 0x65, 0x73, 0x61, 0x74, 0x22,
	0x3E, 0x0A, 0x09, 0x3C, 0x74, 0x73, 0x20, 0x66, 0x72, 0x65, 0x71, 0x3D, 0x22, 0x31, 0x30, 0x37,
	0x39, 0x36, 0x22, 0x20, 0x70, 0x6F, 0x6C, 0x3D, 0x22, 0x76, 0x22, 0x20, 0x73, 0x79, 0x6D, 0x62,
	0x6F, 0x6C, 0x3D, 0x22, 0x32, 0x37, 0x35, 0x30, 0x30, 0x22, 0x20, 0x66, 0x65, 0x63, 0x3D, 0x22,
	0x33, 0x5F, 0x34, 0x22, 0x20, 0x70, 0x73, 0x6B, 0x3D, 0x22, 0x71, 0x70, 0x73, 0x6B, 0x22, 0x20,
	0x6F, 0x6E, 0x69, 0x64, 0x3D, 0x22, 0x32, 0x30, 0x34, 0x38, 0x22, 0x20, 0x74, 0x73, 0x69, 0x64,
	0x3D, 0x22, 0x33, 0x36, 0x22, 0x20, 0x74, 0x72, 0x61, 0x6E, 0x73, 0x3D, 0x22, 0x30, 0x22, 0x3E,
	0x0A, 0x09, 0x3C, 0x2F, 0x74, 0x73, 0x3E, 0x0A, 0x09, 0x3C, 0x74, 0x73, 0x20, 0x66, 0x72, 0x65,
	0x71, 0x3D, 0x22, 0x31, 0x31, 0x33, 0x37, 0x33, 0x22, 0x20, 0x70, 0x6F, 0x6C, 0x3D, 0x22, 0x68,
	0x22, 0x20, 0x73, 0x79, 0x6D, 0x62, 0x6F, 0x6C, 0x3D, 0x22, 0x32, 0x37, 0x35, 0x30, 0x30, 0x22,
	0x20, 0x66, 0x65, 0x63, 0x3D, 0x22, 0x32, 0x5F, 0x33, 0x22, 0x20, 0x70, 0x73, 0x6B, 0x3D, 0x22,
	0x38, 0x70, 0x73, 0x6B, 0x22, 0x20, 0x6F, 0x6E, 0x69, 0x64, 0x3D, 0x22, 0x32, 0x30, 0x34, 0x38,
	0x22, 0x20, 0x74, 0x73, 0x69, 0x64, 0x3D, 0x22, 0x34, 0x35, 0x22, 0x20, 0x74, 0x72, 0x61, 0x6E,
	0x73, 0x3D, 0x22, 0x30, 0x22, 0x3E, 0x0A, 0x09, 0x3C, 0x2F, 0x74, 0x73, 0x3E, 0x0A, 0x09, 0x3C,
	0x74, 0x73, 0x20, 0x66, 0x72, 0x65, 0x71, 0x3D, 0x22, 0x31, 0x31, 0x39, 0x33, 0x38, 0x22, 0x20,
	0x70, 0x6F, 0x6C, 0x3D, 0x22, 0x76, 0x22, 0x20, 0x73, 0x79, 0x6D, 0x62, 0x6F, 0x6C, 0x3D, 0x22,
	0x32, 0x37, 0x35, 0x30, 0x30, 0x22, 0x20, 0x66, 0x65, 0x63, 0x3D, 0x22, 0x33, 0x5F, 0x34, 0x22,
	0x20, 0x70, 0x73, 0x6B, 0x3D, 0x22, 0x71, 0x70, 0x73, 0x6B, 0x22, 0x20, 0x6F, 0x6E, 0x69, 0x64,
	0x3D, 0x22, 0x32, 0x30, 0x34, 0x38, 0x22, 0x20, 0x74, 0x73, 0x69, 0x64, 0x3D, 0x22, 0x32, 0x38,
	0x22, 0x20, 0x74, 0x72, 0x61, 0x6E, 0x73, 0x3D, 0x22, 0x30, 0x22, 0x3E, 0x0A, 0x09, 0x3C, 0x2F,
	0x74, 0x73, 0x3E, 0x0A, 0x09, 0x3C, 0x74, 0x73, 0x20, 0x66, 0x72, 0x65, 0x71, 0x3D, 0x22, 0x31,
	0x31, 0x37, 0x36, 0x36, 0x22, 0x20, 0x70, 0x6F, 0x6C, 0x3D, 0x22, 0x68, 0x22, 0x20, 0x73, 0x79,
	0x6D, 0x62, 0x6F, 0x6C, 0x3D, 0x22, 0x32, 0x37, 0x35, 0x30, 0x30, 0x22, 0x20, 0x66, 0x65, 0x63,
	0x3D, 0x22, 0x35, 0x5F, 0x36, 0x22, 0x20, 0x70, 0x73, 0x6B, 0x3D, 0x22, 0x71, 0x70, 0x73, 0x6B,
	0x22, 0x20, 0x6F, 0x6E, 0x69, 0x64, 0x3D, 0x22, 0x32, 0x30, 0x34, 0x38, 0x22, 0x20, 0x74, 0x73,
	0x69, 0x64, 0x3D, 0x22, 0x35, 0x22, 0x20, 0x74, 0x72, 0x61, 0x6E, 0x73, 0x3D, 0x22, 0x30, 0x22,
	0x3E, 0x0A, 0x09, 0x3C, 0x2F, 0x74, 0x73, 0x3E, 0x0A, 0x3C, 0x2F, 0x73, 0x61, 0x74, 0x3E, 0x0A,
	0x0A, 0x3C, 0x73, 0x61, 0x74, 0x20, 0x74, 0x79, 0x70, 0x65, 0x3D, 0x22, 0x62, 0x61, 0x64, 0x72,
	0x22, 0x3E, 0x0A, 0x09, 0x3C, 0x74, 0x73, 0x20, 0x66, 0x72, 0x65, 0x71, 0x3D, 0x22, 0x31, 0x30,
	0x37, 0x33, 0x30, 0x22, 0x20, 0x70, 0x6F, 0x6C, 0x3D, 0x22, 0x76, 0x22, 0x20, 0x73, 0x79, 0x6D,
	0x62, 0x6F, 0x6C, 0x3D, 0x22, 0x32, 0x37, 0x35, 0x30, 0x30, 0x22, 0x20, 0x66, 0x65, 0x63, 0x3D,
	0x22, 0x33, 0x5F, 0x34, 0x22, 0x20, 0x70, 0x73, 0x6B, 0x3D, 0x22, 0x71, 0x70, 0x73, 0x6B, 0x22,
	0x20, 0x6F, 0x6E, 0x69, 0x64, 0x3D, 0x22, 0x37, 0x30, 0x32, 0x22, 0x20, 0x74, 0x73, 0x69, 0x64,
	0x3D, 0x22, 0x31, 0x22, 0x20, 0x74, 0x72, 0x61, 0x6E, 0x73, 0x3D, 0x22, 0x30, 0x22, 0x3E, 0x0A,
	0x09, 0x3C, 0x2F, 0x74, 0x73, 0x3E, 0x0A, 0x09, 0x3C, 0x74, 0x73, 0x20, 0x66, 0x72, 0x65, 0x71,
	0x3D, 0x22, 0x31, 0x30, 0x37, 0x37, 0x30, 0x22, 0x20, 0x70, 0x6F, 0x6C, 0x3D, 0x22, 0x68, 0x22,
	0x20, 0x73, 0x79, 0x6D, 0x62, 0x6F, 0x6C, 0x3D, 0x22, 0x32, 0x37, 0x35, 0x30, 0x30, 0x22, 0x20,
	0x66, 0x65, 0x63, 0x3D, 0x22, 0x33, 0x5F, 0x34, 0x22, 0x20, 0x70, 0x73, 0x6B, 0x3D, 0x22, 0x71,
	0x70, 0x73, 0x6B, 0x22, 0x20, 0x6F, 0x6E, 0x69, 0x64, 0x3D, 0x22, 0x31, 0x22, 0x20, 0x74, 0x73,
	0x69, 0x64, 0x3D, 0x22, 0x31, 0x22, 0x20, 0x74, 0x72, 0x61, 0x6E, 0x73, 0x3D, 0x22, 0x30, 0x22,
	0x3E, 0x0A, 0x09, 0x3C, 0x2F, 0x74, 0x73, 0x3E, 0x0A, 0x09, 0x3C, 0x74, 0x73, 0x20, 0x66, 0x72,
	0x65, 0x71, 0x3D, 0x22, 0x31, 0x31, 0x39, 0x31, 0x39, 0x22, 0x20, 0x70, 0x6F, 0x6C, 0x3D, 0x22,
	0x68, 0x22, 0x20, 0x73, 0x79, 0x6D, 0x62, 0x6F, 0x6C, 0x3D, 0x22, 0x32, 0x37, 0x35, 0x30, 0x30,
	0x22, 0x20, 0x66, 0x65, 0x63, 0x3D, 0x22, 0x33, 0x5F, 0x34, 0x22, 0x20, 0x70, 0x73, 0x6B, 0x3D,
	0x22, 0x71, 0x70, 0x73, 0x6B, 0x22, 0x20, 0x6F, 0x6E, 0x69, 0x64, 0x3D, 0x22, 0x37, 0x30, 0x32,
	0x22, 0x20, 0x74, 0x73, 0x69, 0x64, 0x3D, 0x22, 0x35, 0x31, 0x32, 0x22, 0x20, 0x74, 0x72, 0x61,
	0x6E, 0x73, 0x3D, 0x22, 0x30, 0x22, 0x3E, 0x0A, 0x09, 0x3C, 0x2F, 0x74, 0x73, 0x3E, 0x0A, 0x09,
	0x3C, 0x74, 0x73, 0x20, 0x66, 0x72, 0x65, 0x71, 0x3D, 0x22, 0x31, 0x32, 0x34, 0x37, 0x36, 0x22,
	0x20, 0x70, 0x6F, 0x6C, 0x3D, 0x22, 0x76, 0x22, 0x20, 0x73, 0x79, 0x6D, 0x62, 0x6F, 0x6C, 0x3D,
	0x22, 0x32, 0x37, 0x35, 0x30, 0x30, 0x22, 0x20, 0x66, 0x65, 0x63, 0x3D, 0x22, 0x33, 0x5F, 0x34,
	0x22, 0x20, 0x70, 0x73, 0x6B, 0x3D, 0x22, 0x71, 0x70, 0x73, 0x6B, 0x22, 0x20, 0x6F, 0x6E, 0x69,
	0x64, 0x3D, 0x22, 0x37, 0x30, 0x32, 0x22, 0x20, 0x74, 0x73, 0x69, 0x64, 0x3D, 0x22, 0x35, 0x35,
	0x39, 0x22, 0x20, 0x74, 0x72, 0x61, 0x6E, 0x73, 0x3D, 0x22, 0x30, 0x22, 0x3E, 0x0A, 0x09, 0x3C,
	0x2F, 0x74, 0x73, 0x3E, 0x0A, 0x3C, 0x2F, 0x73, 0x61, 0x74, 0x3E, 0x0A, 0x0A, 0x3C, 0x73, 0x61,
	0x74, 0x20, 0x74, 0x79, 0x70, 0x65, 0x3D, 0x22, 0x68, 0x6F, 0x74, 0x62, 0x69, 0x72, 0x64, 0x22,
	0x3E, 0x0A, 0x09, 0x3C, 0x74, 0x73, 0x20, 0x66, 0x72, 0x65, 0x71, 0x3D, 0x22, 0x31, 0x31, 0x30,
	0x35, 0x34, 0x22, 0x20, 0x70, 0x6F, 0x6C, 0x3D, 0x22, 0x68, 0x22, 0x20, 0x73, 0x79, 0x6D, 0x62,
	0x6F, 0x6C, 0x3D, 0x22, 0x32, 0x37, 0x35, 0x30, 0x30, 0x22, 0x20, 0x66, 0x65, 0x63, 0x3D, 0x22,
	0x35, 0x5F, 0x36, 0x22, 0x20, 0x70, 0x73, 0x6B, 0x3D, 0x22, 0x71, 0x70, 0x73, 0x6B, 0x22, 0x20,
	0x6F, 0x6E, 0x69, 0x64, 0x3D, 0x22, 0x33, 0x31, 0x38, 0x22, 0x20, 0x74, 0x73, 0x69, 0x64, 0x3D,
	0x22, 0x31, 0x32, 0x37, 0x30, 0x30, 0x22, 0x20, 0x74, 0x72, 0x61, 0x6E, 0x73, 0x3D, 0x22, 0x30,
	0x22, 0x3E, 0x0A, 0x09, 0x3C, 0x2F, 0x74, 0x73, 0x3E, 0x0A, 0x09, 0x3C, 0x74, 0x73, 0x20, 0x66,
	0x72, 0x65, 0x71, 0x3D, 0x22, 0x31, 0x31, 0x32, 0x34, 0x30, 0x22, 0x20, 0x70, 0x6F, 0x6C, 0x3D,
	0x22, 0x76, 0x22, 0x20, 0x73, 0x79, 0x6D, 0x62, 0x6F, 0x6C, 0x3D, 0x22, 0x32, 0x37, 0x35, 0x30,
	0x30, 0x22, 0x20, 0x66, 0x65, 0x63, 0x3D, 0x22, 0x33, 0x5F, 0x34, 0x22, 0x20, 0x70, 0x73, 0x6B,
	0x3D, 0x22, 0x71, 0x70, 0x73, 0x6B, 0x22, 0x20, 0x6F, 0x6E, 0x69, 0x64, 0x3D, 0x22, 0x33, 0x31,
	0x38, 0x22, 0x20, 0x74, 0x73, 0x69, 0x64, 0x3D, 0x22, 0x32, 0x30, 0x30, 0x22, 0x20, 0x74, 0x72,
	0x61, 0x6E, 0x73, 0x3D, 0x22, 0x30, 0x22, 0x3E, 0x0A, 0x09, 0x3C, 0x2F, 0x74, 0x73, 0x3E, 0x0A,
	0x09, 0x3C, 0x74, 0x73, 0x20, 0x66, 0x72, 0x65, 0x71, 0x3D, 0x22, 0x31, 0x31, 0x37, 0x36, 0x36,
	0x22, 0x20, 0x70, 0x6F, 0x6C, 0x3D, 0x22, 0x76, 0x22, 0x20, 0x73, 0x79, 0x6D, 0x62, 0x6F, 0x6C,
	0x3D, 0x22, 0x32, 0x37, 0x35, 0x30, 0x30, 0x22, 0x20, 0x66, 0x65, 0x63, 0x3D, 0x22, 0x32, 0x5F,
	0x33, 0x22, 0x20, 0x70, 0x73, 0x6B, 0x3D, 0x22, 0x71, 0x70, 0x73, 0x6B, 0x22, 0x20, 0x6F, 0x6E,
	0x69, 0x64, 0x3D, 0x22, 0x33, 0x31, 0x38, 0x22, 0x20, 0x74, 0x73, 0x69, 0x64, 0x3D, 0x22, 0x35,
	0x32, 0x30, 0x30, 0x22, 0x20, 0x74, 0x72, 0x61, 0x6E, 0x73, 0x3D, 0x22, 0x30, 0x22, 0x3E, 0x0A,
	0x09, 0x3C, 0x2F, 0x74, 0x73, 0x3E, 0x0A, 0x09, 0x3C, 0x74, 0x73, 0x20, 0x66, 0x72, 0x65, 0x71,
	0x3D, 0x22, 0x31, 0x32, 0x30, 0x31, 0x35, 0x22, 0x20, 0x70, 0x6F, 0x6C, 0x3D, 0x22, 0x68, 0x22,
	0x20, 0x73, 0x79, 0x6D, 0x62, 0x6F, 0x6C, 0x3D, 0x22, 0x32, 0x37, 0x35, 0x30, 0x30, 0x22, 0x20,
	0x66, 0x65, 0x63, 0x3D, 0x22, 0x33, 0x5F, 0x34, 0x22, 0x20, 0x70, 0x73, 0x6B, 0x3D, 0x22, 0x71,
	0x70, 0x73, 0x6B, 0x22, 0x20, 0x6F, 0x6E, 0x69, 0x64, 0x3D, 0x22, 0x33, 0x31, 0x38, 0x22, 0x20,
	0x74, 0x73, 0x69, 0x64, 0x3D, 0x22, 0x36, 0x35, 0x30, 0x30, 0x22, 0x20, 0x74, 0x72, 0x61, 0x6E,
	0x73, 0x3D, 0x22, 0x30, 0x22, 0x3E, 0x0A, 0x09, 0x3C, 0x2F, 0x74, 0x73, 0x3E, 0x0A, 0x3C, 0x2F,
	0x73, 0x61, 0x74, 0x3E, 0x0A, 0x0A, 0x3C, 0x73, 0x61, 0x74, 0x20, 0x74, 0x79, 0x70, 0x65, 0x3D,
	0x22, 0x79, 0x61, 0x68, 0x6C, 0x69, 0x76, 0x65, 0x22, 0x3E, 0x0A, 0x09, 0x3C, 0x74, 0x73, 0x20,
	0x66, 0x72, 0x65, 0x71, 0x3D, 0x22, 0x31, 0x31, 0x37, 0x38, 0x35, 0x22, 0x20, 0x70, 0x6F, 0x6C,
	0x3D, 0x22, 0x68, 0x22, 0x20, 0x73, 0x79, 0x6D, 0x62, 0x6F, 0x6C, 0x3D, 0x22, 0x32, 0x37, 0x35,
	0x30, 0x30, 0x22, 0x20, 0x66, 0x65, 0x63, 0x3D, 0x22, 0x35, 0x5F, 0x36, 0x22, 0x20, 0x70, 0x73,
	0x6B, 0x3D, 0x22, 0x71, 0x70, 0x73, 0x6B, 0x22, 0x20, 0x6F, 0x6E, 0x69, 0x64, 0x3D, 0x22, 0x35,
	0x32, 0x35, 0x22, 0x20, 0x74, 0x73, 0x69, 0x64, 0x3D, 0x22, 0x34, 0x22, 0x20, 0x74, 0x72, 0x61,
	0x6E, 0x73, 0x3D, 0x22, 0x31, 0x22, 0x3E, 0x0A, 0x09, 0x3C, 0x2F, 0x74, 0x73, 0x3E, 0x0A, 0x09,
	0x3C, 0x74, 0x73, 0x20, 0x66, 0x72, 0x65, 0x71, 0x3D, 0x22, 0x31, 0x32, 0x30, 0x33, 0x34, 0x22,
	0x20, 0x70, 0x6F, 0x6C, 0x3D, 0x22, 0x76, 0x22, 0x20, 0x73, 0x79, 0x6D, 0x62, 0x6F, 0x6C, 0x3D,
	0x22, 0x32, 0x37, 0x35, 0x30, 0x30, 0x22, 0x20, 0x66, 0x65, 0x63, 0x3D, 0x22, 0x32, 0x5F, 0x33,
	0x22, 0x20, 0x70, 0x73, 0x6B, 0x3D, 0x22, 0x38, 0x70, 0x73, 0x6B, 0x22, 0x20, 0x6F, 0x6E, 0x69,
	0x64, 0x3D, 0x22, 0x31, 0x22, 0x20, 0x74, 0x73, 0x69, 0x64, 0x3D, 0x22, 0x31, 0x22, 0x20, 0x74,
	0x72, 0x61, 0x6E, 0x73, 0x3D, 0x22, 0x31, 0x22, 0x3E, 0x0A, 0x09, 0x3C, 0x2F, 0x74, 0x73, 0x3E,
	0x0A, 0x3C, 0x2F, 0x73, 0x61, 0x74, 0x3E, 0x0A, 0x0A, 0x3C, 0x2F, 0x63, 0x68, 0x65, 0x63, 0x6B,
	0x6C, 0x69, 0x73, 0x74, 0x3E, 0x0A, 0x0A, 0x0A
};
/* end binary data. size = 746 bytes */

#else
const static char s_aucFindSat_DefaultFileData [] = /* 532 */
{0x3C,0x3F,0x78,0x6D,0x6C,0x20,0x76,0x65,0x72,0x73,0x69,0x6F,0x6E,0x3D,0x22
,0x31,0x2E,0x30,0x22,0x20,0x65,0x6E,0x63,0x6F,0x64,0x69,0x6E,0x67,0x3D,0x22
,0x49,0x53,0x4F,0x2D,0x38,0x38,0x35,0x39,0x2D,0x39,0x22,0x20,0x3F,0x3E,0x0A
	,0x3C,0x63,0x68,0x65,0x63,0x6B,0x6C,0x69,0x73,0x74,0x3E,0x0A,0x0A,0x3C,0x64
	,0x61,0x74,0x61,0x5F,0x76,0x65,0x72,0x20,0x76,0x65,0x72,0x6E,0x75,0x6D,0x3D
	,0x22,0x31,0x22,0x3E,0x0A,0x3C,0x2F,0x64,0x61,0x74,0x61,0x5F,0x76,0x65,0x72
	,0x3E,0x0A,0x0A,0x3C,0x73,0x61,0x74,0x20,0x74,0x79,0x70,0x65,0x3D,0x22,0x61
	,0x73,0x74,0x72,0x61,0x31,0x22,0x3E,0x0A,0x09,0x3C,0x74,0x73,0x20,0x66,0x72
	,0x65,0x71,0x3D,0x22,0x31,0x31,0x38,0x33,0x36,0x22,0x20,0x70,0x6F,0x6C,0x3D
	,0x22,0x68,0x22,0x20,0x73,0x79,0x6D,0x62,0x6F,0x6C,0x3D,0x22,0x32,0x37,0x35
	,0x30,0x30,0x22,0x20,0x66,0x65,0x63,0x3D,0x22,0x33,0x5F,0x34,0x22,0x20,0x70
	,0x73,0x6B,0x3D,0x22,0x71,0x70,0x73,0x6B,0x22,0x20,0x6F,0x6E,0x69,0x64,0x3D
	,0x22,0x31,0x22,0x20,0x74,0x73,0x69,0x64,0x3D,0x22,0x31,0x31,0x30,0x31,0x22
	,0x3E,0x0A,0x09,0x3C,0x2F,0x74,0x73,0x3E,0x0A,0x09,0x3C,0x74,0x73,0x20,0x66
	,0x72,0x65,0x71,0x3D,0x22,0x31,0x31,0x39,0x35,0x34,0x22,0x20,0x70,0x6F,0x6C
	,0x3D,0x22,0x68,0x22,0x20,0x73,0x79,0x6D,0x62,0x6F,0x6C,0x3D,0x22,0x32,0x37
	,0x35,0x30,0x30,0x22,0x20,0x66,0x65,0x63,0x3D,0x22,0x33,0x5F,0x34,0x22,0x20
	,0x70,0x73,0x6B,0x3D,0x22,0x71,0x70,0x73,0x6B,0x22,0x20,0x6F,0x6E,0x69,0x64
	,0x3D,0x22,0x31,0x22,0x20,0x74,0x73,0x69,0x64,0x3D,0x22,0x31,0x30,0x37,0x39
	,0x22,0x3E,0x0A,0x09,0x3C,0x2F,0x74,0x73,0x3E,0x0A,0x3C,0x2F,0x73,0x61,0x74
	,0x3E,0x0A,0x0A,0x3C,0x73,0x61,0x74,0x20,0x74,0x79,0x70,0x65,0x3D,0x22,0x68
	,0x6F,0x74,0x62,0x69,0x72,0x64,0x22,0x3E,0x0A,0x09,0x3C,0x74,0x73,0x20,0x66
	,0x72,0x65,0x71,0x3D,0x22,0x31,0x31,0x37,0x36,0x36,0x22,0x20,0x70,0x6F,0x6C
	,0x3D,0x22,0x76,0x22,0x20,0x73,0x79,0x6D,0x62,0x6F,0x6C,0x3D,0x22,0x32,0x37
	,0x35,0x30,0x30,0x22,0x20,0x66,0x65,0x63,0x3D,0x22,0x32,0x5F,0x33,0x22,0x20
	,0x70,0x73,0x6B,0x3D,0x22,0x71,0x70,0x73,0x6B,0x22,0x20,0x6F,0x6E,0x69,0x64
	,0x3D,0x22,0x33,0x31,0x38,0x22,0x20,0x74,0x73,0x69,0x64,0x3D,0x22,0x35,0x32
	,0x30,0x30,0x22,0x3E,0x0A,0x09,0x3C,0x2F,0x74,0x73,0x3E,0x0A,0x09,0x3C,0x74
	,0x73,0x20,0x66,0x72,0x65,0x71,0x3D,0x22,0x31,0x31,0x39,0x31,0x39,0x22,0x20
	,0x70,0x6F,0x6C,0x3D,0x22,0x76,0x22,0x20,0x73,0x79,0x6D,0x62,0x6F,0x6C,0x3D
	,0x22,0x32,0x37,0x35,0x30,0x30,0x22,0x20,0x66,0x65,0x63,0x3D,0x22,0x32,0x5F
	,0x33,0x22,0x20,0x70,0x73,0x6B,0x3D,0x22,0x71,0x70,0x73,0x6B,0x22,0x20,0x6F
	,0x6E,0x69,0x64,0x3D,0x22,0x32,0x37,0x32,0x22,0x20,0x74,0x73,0x69,0x64,0x3D
	,0x22,0x36,0x30,0x30,0x30,0x22,0x3E,0x0A,0x09,0x3C,0x2F,0x74,0x73,0x3E,0x0A
	,0x3C,0x2F,0x73,0x61,0x74,0x3E,0x0A,0x0A,0x0A,0x3C,0x2F,0x63,0x68,0x65,0x63
	,0x6B,0x6C,0x69,0x73,0x74,0x3E,0x0A,0x0A};
/* end binary data. size = 532 bytes */
#endif
static FindSat_Contents_t *xmgr_findsat_GetContents (void);
static void xmgr_findsat_FreeContents (void);
static INLINE HERROR xmgr_findsat_CheckActionId (FindSat_Contents_t *pstContents, Handle_t hAction);
static INLINE void xmgr_findsat_IncreaseSessionVersion (FindSat_Contents_t *pstContents);

static HERROR xmgr_findsat_ChangeTpText (FindSat_Contents_t *pstContents);
static HERROR xmgr_findsat_ChangeProgressNumber (FindSat_Contents_t *pstContents);

static HERROR xmgr_findsat_InitContents (FindSat_Contents_t *pstContents);
static HERROR xmgr_findsat_MakeDefaultAntInfo (FindSat_Contents_t *pstContents, DxSAT_DiseqcInput_e eDiseqc);
static HERROR xmgr_findsat_InitCheckListPosition (FindSat_Contents_t *pstContents);

static HERROR xmgr_findsat_DestroyContents (FindSat_Contents_t *pstContents);

static HERROR xmgr_findsat_TakeAction (HUINT32 ulActionId);
static HERROR xmgr_findsat_ReleaseAction (FindSat_Contents_t *pstContents);


static HERROR xmgr_findsat_PrintCheckList (FindSat_Contents_t *pstContents);

static HERROR xmgr_findsat_GetCurrentCheckTuningInfo (FindSat_Contents_t *pstContents, DxTuneParam_t *pstTuneInfo);
static HERROR xmgr_findsat_GetCurrentSatCheckItem (FindSat_Contents_t *pstContents, SatCheckList_t *pstSatItem);
static HERROR xmgr_findsat_GetCurrentTsCheckItem (FindSat_Contents_t *pstContents, TsCheckList_t *pstTsItem);
static HERROR xmgr_findsat_GotoNextCheckList (FindSat_Contents_t *pstContents);
static HERROR xmgr_findsat_SetNextCheckTp (FindSat_Contents_t *pstContents);
static HERROR xmgr_findsat_SetNextCheckSat (FindSat_Contents_t *pstContents);
static HERROR xmgr_findsat_SetNextDiseqc (FindSat_Contents_t *pstContents);
static HERROR xmgr_findsat_SetCheckResult (FindSat_Contents_t *pstContents, SVC_SatType_t eSatType);

static HERROR xmgr_findsat_CheckSiResults (FindSat_Contents_t *pstContents);
static HERROR xmgr_findsat_DetermineSatellites (FindSat_Contents_t *pstContents);

static HERROR xmgr_findsat_SendUpdateDisplayMsg (Handle_t hAction);
static HERROR xmgr_findsat_SendSearchStartedMsg (Handle_t hAction);
static HERROR xmgr_findsat_SendSearchFinishedMsg (Handle_t hAction, HBOOL bSuccess);
static HERROR xmgr_findsat_SendSearchSaveFinishedMsg (Handle_t hAction);
static HERROR xmgr_findsat_SendSearchActionDestroyedMsg (Handle_t hAction);

static HERROR xmgr_findsat_TuneCheckTp (FindSat_Contents_t *pstContents);

static HERROR xmgr_findsat_MsgGwmCreate (FindSat_Contents_t *pstContents, HINT32 nAppHandle, HINT32 nParam);
static HERROR xmgr_findsat_MsgGwmDestroyed (FindSat_Contents_t *pstContents);
static HERROR xmgr_findsat_MsgAppSearchActionStart (FindSat_Contents_t *pstContents, HINT32 nAppHandle);
static HERROR xmgr_findsat_MsgAppSearchSaveResult (FindSat_Contents_t *pstContents, HINT32 nAppHandle);
static HERROR xmgr_findsat_MsgAppSearchCancel (FindSat_Contents_t *pstContents, HINT32 nAppHandle);
static HERROR xmgr_findsat_MsgGwmTimer (FindSat_Contents_t *pstContents, HINT32 nTimerId);
static HERROR xmgr_findsat_EvtChLocked (FindSat_Contents_t *pstContents, Handle_t hAction);
static HERROR xmgr_findsat_EvtChNoSignal (FindSat_Contents_t *pstContents, Handle_t hAction);
static HERROR xmgr_findsat_EvtSiSearchDone (FindSat_Contents_t *pstContents, Handle_t hAction);
static HERROR xmgr_findsat_EvtSiSearchFail (FindSat_Contents_t *pstContents, Handle_t hAction);

#if defined(FINDSAT_CHECKLIST_ARGUMENT)
static HERROR xmgr_findsat_InitCheckList (FindSat_Contents_t *pstContents, HINT32 nAppHandle);
static HERROR xmgr_findsat_AddSatellite(DbSvc_AntInfo_t	*pstAntInfo, HUINT16 	*pusAntIdx);
#else

static HERROR xmgr_findsat_CheckVersion (FindSat_Contents_t *pstContents);
static HERROR xmgr_findsat_LoadCheckList (FindSat_Contents_t *pstContents);
static HERROR xmgr_findsat_PrefareDefaultDataFile (FindSat_Contents_t *pstContents);
static HERROR xmgr_findsat_ParseXmlSatData (HXMLELEMENT hSatElem, SatCheckList_t *pstSatCheck);
static HERROR xmgr_findsat_ParseXmlTsData (HXMLELEMENT hTsElem, TsCheckList_t *pstTsCheck);
static HERROR xmgr_findsat_GetSatAttributes (HXMLELEMENT hSatElem, SatCheckList_t *pstSatCheck);
static HERROR xmgr_findsat_GetTsAttributes (HXMLELEMENT hTsElem, TsCheckList_t *pstTsCheck);
static HERROR xmgr_findsat_GetArgValues (ArgConvTable_t *pstArgTable, HUINT8 *szArg, HUINT32 *pulValue);
#endif


#define _________________________________________________________________________________

static FindSat_Contents_t *xmgr_findsat_GetContents (void)
{
	if (s_pstFindSat_Contents == NULL)
	{
		s_pstFindSat_Contents = (FindSat_Contents_t *)OxAP_Malloc (sizeof(FindSat_Contents_t));
		if (s_pstFindSat_Contents != NULL)
		{
			HxSTD_memset (s_pstFindSat_Contents, 0, sizeof(FindSat_Contents_t));
		}
	}

	return s_pstFindSat_Contents;
}

static void xmgr_findsat_FreeContents (void)
{
	HUINT32					 ulSatCheckIdx;
	SatCheckList_t			*pstSatItem;

	if (s_pstFindSat_Contents != NULL)
	{
		if (s_pstFindSat_Contents->pstSatCheckArray != NULL)
		{
			for (ulSatCheckIdx = 0; ulSatCheckIdx < s_pstFindSat_Contents->ulNumCheckSat; ulSatCheckIdx++)
			{
				pstSatItem = &(s_pstFindSat_Contents->pstSatCheckArray[ulSatCheckIdx]);

				if (pstSatItem->pstTsCheckArray != NULL)
				{
					OxAP_Free (pstSatItem->pstTsCheckArray);
					pstSatItem->pstTsCheckArray = NULL;
				}
			}

			OxAP_Free (s_pstFindSat_Contents->pstSatCheckArray);
			s_pstFindSat_Contents->pstSatCheckArray = NULL;
		}

		OxAP_Free (s_pstFindSat_Contents);
		s_pstFindSat_Contents = NULL;
	}
}

static INLINE HERROR xmgr_findsat_CheckActionId (FindSat_Contents_t *pstContents, Handle_t hAction)
{
	HUINT32			 ulActionId;

	ulActionId = SVC_PIPE_GetActionId (hAction);

	if (pstContents->ulActionId == ulActionId)
	{
		return ERR_BUS_NO_ERROR;
	}

	HxLOG_Info("is not matched actionID(0x%x)! line(%d)\n", ulActionId, __LINE__);
	return MESSAGE_PASS;
}

static INLINE void xmgr_findsat_IncreaseSessionVersion (FindSat_Contents_t *pstContents)
{
	pstContents->hAction = SVC_PIPE_IncreaseActionVersion (pstContents->ulActionId);
}


#define	_____CONTENTS_INITIATION_____

static HERROR xmgr_findsat_InitContents (FindSat_Contents_t *pstContents)
{
	HUINT32				 ulCount;

	// 1. Clear all the contents:
	HxSTD_memset (pstContents, 0, sizeof(FindSat_Contents_t));

	pstContents->eState = eSatFindState_Start;
	for (ulCount = 0; ulCount < 4; ulCount ++)
	{
		pstContents->aeSatType[ulCount] = eSatType_NONE;
	}

	return ERR_BUS_NO_ERROR;
}

static HERROR xmgr_findsat_MakeDefaultAntInfo (FindSat_Contents_t *pstContents, DxSAT_DiseqcInput_e eDiseqc)
{
	HxSTD_memset (&pstContents->stAntInfo, 0, sizeof(DbSvc_AntInfo_t));

	pstContents->stAntInfo.eAntennaType				= eDxANT_TYPE_DISEQC;
	pstContents->stAntInfo.utAntInfo.stDiseqcAnt.ulLnbFreq	= CH_UNIVERSAL_LNB_FREQ;
	pstContents->stAntInfo.utAntInfo.stDiseqcAnt.eLnbVoltage	= eDxLNB_VOLT_STD;
	pstContents->stAntInfo.utAntInfo.stDiseqcAnt.b22kTone		= FALSE;
	pstContents->stAntInfo.utAntInfo.stDiseqcAnt.eDiseqcVer	= eDxDISEQC_VER_1_0;
	pstContents->stAntInfo.utAntInfo.stDiseqcAnt.eDiseqcInput	= eDiseqc;
	pstContents->stAntInfo.utAntInfo.stDiseqcAnt.eToneBurst	= eDxTONE_BURST_NONE;

	return ERR_OK;
}

static HERROR xmgr_findsat_InitCheckListPosition (FindSat_Contents_t *pstContents)
{
	HxLOG_Info("[xmgr_findsat_InitCheckListPosition] \n");
	pstContents->ulCurrSatCnt = 0;
	pstContents->ulCurrTsCnt = 0;

	return ERR_OK;
}



#define	_____CONTENTS_MODIFICATION_____



#define	_____CONTENTS_DESTRUCTION_____

static HERROR xmgr_findsat_DestroyContents (FindSat_Contents_t *pstContents)
{
	(void)pstContents;

	xmgr_findsat_FreeContents();

	return ERR_BUS_NO_ERROR;
}


#define	_____FOR_UI_____

static HERROR xmgr_findsat_ChangeTpText (FindSat_Contents_t *pstContents)
{
	return ERR_OK;
}

static HERROR xmgr_findsat_ChangeProgressNumber (FindSat_Contents_t *pstContents)
{
	HUINT32			 ulDiseqcCnt, ulSatCnt, ulTsCnt;
	HUINT32			 ulCurrDiseqc;
	HUINT32			 ulNumFinishedTp, ulTotalNumTp;
	SatCheckList_t	*pstSatItem;

	ulTotalNumTp = 0;
	ulNumFinishedTp = 0;

	ulCurrDiseqc = (pstContents->stAntInfo.utAntInfo.stDiseqcAnt.eDiseqcInput - eDxDISEQC_INPUT_A);

	for (ulDiseqcCnt = 0; ulDiseqcCnt < 4; ulDiseqcCnt++)
	{
		for (ulSatCnt = 0; ulSatCnt < pstContents->ulNumCheckSat; ulSatCnt++)
		{
			pstSatItem = &(pstContents->pstSatCheckArray[ulSatCnt]);

			for (ulTsCnt = 0; ulTsCnt < pstSatItem->ulNumTp; ulTsCnt++)
			{
				ulTotalNumTp ++;

				if ((ulDiseqcCnt < ulCurrDiseqc) ||
					(ulDiseqcCnt == ulCurrDiseqc && ulSatCnt < pstContents->ulCurrSatCnt) ||
					(ulDiseqcCnt == ulCurrDiseqc && ulSatCnt == pstContents->ulCurrSatCnt && ulTsCnt < pstContents->ulCurrTsCnt))
				{
					ulNumFinishedTp++;
				}
			}
		}
	}


	MGR_SEARCHRESULT_SetNumTpCompleted (pstContents->ulActionId, ulNumFinishedTp);
	MGR_SEARCHRESULT_SetNumTpTotal (pstContents->ulActionId, ulTotalNumTp);

	return ERR_OK;
}


#define	_____DATA_CONVERSION_____


#define	_____ACTION_FUNCTIONS_____

static HERROR xmgr_findsat_TakeAction (HUINT32 ulActionId)
{
	DxTuneParam_t		 stTuneInfo;
	HERROR				 hErr;

	HxSTD_memset (&stTuneInfo, 0, sizeof(DxTuneParam_t));

	stTuneInfo.eDeliType = eDxDELIVERY_TYPE_SAT;

	hErr = MGR_ACTION_SetupSearch (ulActionId, &stTuneInfo, eAmPriority_SEARCH);
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	hErr = MGR_ACTION_Take (ulActionId, eActionType_SEARCH, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR xmgr_findsat_ReleaseAction (FindSat_Contents_t *pstContents)
{
	HERROR			 hErr;

	if (pstContents->bActionTaken == TRUE)
	{
		// Reset the DiSEqC Info
		SVC_CH_ResetDiseqc (pstContents->hAction, eAsyncMode);

		hErr = SVC_CH_StopTune (pstContents->hAction, (HUINT16)-1, eSyncMode);
		if (hErr != ERR_OK)
		{
			HxLOG_Critical("\n\n\n");
		}

		// Release the action :
		MGR_ACTION_Release (pstContents->ulActionId, eActionType_SEARCH);
		pstContents->bActionTaken = FALSE;
	}

	return ERR_OK;
}


#define	_____MESSAGE_FUNCTIONS_____


#define	_____DEFAULT_DATA_FUNCTIONS_____

#define	_____PARSING_FUNCTIONS_____





#if defined(FINDSAT_CHECKLIST_ARGUMENT)
#if defined(CONFIG_OP_FREESAT) // Freesat 아닌 경우에도 FINDSAT_CHECKLIST_ARGUMENT 을 추후에 추가랗 결우를 대비해서 분리 한다.
static HERROR xmgr_findsat_InitCheckList (FindSat_Contents_t *pstContents, HINT32 nAppHandle)
{
	HUINT32				 ulNumSat = 1;
	HERROR				 hErr;
	HUINT32				 ulCntTs, ulNumTs;
	MgrSearch_Option_t 		*pstOption = (MgrSearch_Option_t *)nAppHandle;
	SatCheckList_t		*pstSatCheck;
	HINT32				nValue;
	DbSvc_TuningInfo_t		stNewTuningInfo;
	int					FixedTPNum = 4;


	pstContents->pstSatCheckArray = (SatCheckList_t *)OxAP_Malloc (sizeof(SatCheckList_t) * ulNumSat);
	if (pstContents->pstSatCheckArray == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	HxSTD_memset (pstContents->pstSatCheckArray, 0, sizeof(SatCheckList_t) * ulNumSat);
	pstContents->ulNumCheckSat = ulNumSat;

	pstContents->pstSatCheckArray[0].eSatType = eSatType_ASTRA_2;

	ulNumTs = pstOption->ulTpNum+ FixedTPNum;

	if (ulNumTs == 0)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	pstSatCheck = &(pstContents->pstSatCheckArray[0]);
	pstSatCheck->pstTsCheckArray = (TsCheckList_t *)OxAP_Malloc (sizeof(TsCheckList_t) * ulNumTs);
	if (pstSatCheck->pstTsCheckArray == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	HxSTD_memset (pstSatCheck->pstTsCheckArray, 0, sizeof(TsCheckList_t) * ulNumTs);

// for Freesat //////////////////////////////////////////////////////////////////////////////////////////////////////////
	// HOME TP1 // Hardcoding 된 값을 저장하는 것은 여기서만 하도록 한다.
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_HOME_TP_INFO_1, (HUINT32 *)&stNewTuningInfo.sat, sizeof(DbSvc_SatTuningInfo_t));
	if ((hErr != ERR_OK) || (stNewTuningInfo.sat.ulFrequency == 0))
	{
		XMGR_TPLIST_GetFactoryTuningInfo_Freesat(1, &stNewTuningInfo);
		nValue = sizeof(DbSvc_SatTuningInfo_t);
		DB_PARAM_SetItem(eDB_PARAM_ITEM_HOME_TP_INFO_1, (HUINT32)&stNewTuningInfo.sat, nValue);
	}
	HxSTD_memcpy(&(pstSatCheck->pstTsCheckArray[0].stTuneInfo), &stNewTuningInfo.sat, sizeof(DbSvc_TuningInfo_t));

	// HOME TP2 // 다이젝이 도는 것은 Wizard와 거의 비슷한 상태이므로 HomeTP 1 을 제외하고는 Hardcoding 정보를 무조건 사용한다.
	XMGR_TPLIST_GetFactoryTuningInfo_Freesat(2, &stNewTuningInfo);
	nValue = sizeof(DbSvc_SatTuningInfo_t);
	DB_PARAM_SetItem(eDB_PARAM_ITEM_HOME_TP_INFO_2, (HUINT32)&stNewTuningInfo.sat, nValue);
	HxSTD_memcpy(&(pstSatCheck->pstTsCheckArray[1].stTuneInfo), &stNewTuningInfo.sat, sizeof(DbSvc_TuningInfo_t));

	// HOME TP3
	XMGR_TPLIST_GetFactoryTuningInfo_Freesat(3, &stNewTuningInfo);
	nValue = sizeof(DbSvc_SatTuningInfo_t);
	DB_PARAM_SetItem(eDB_PARAM_ITEM_HOME_TP_INFO_3, (HUINT32)&stNewTuningInfo.sat, nValue);
	HxSTD_memcpy(&(pstSatCheck->pstTsCheckArray[2].stTuneInfo), &stNewTuningInfo.sat, sizeof(DbSvc_TuningInfo_t));

	XMGR_TPLIST_GetFactoryTuningInfo_Freesat(4, &stNewTuningInfo);
	nValue = sizeof(DbSvc_SatTuningInfo_t);
	HxSTD_memcpy(&(pstSatCheck->pstTsCheckArray[3].stTuneInfo), &stNewTuningInfo.sat, sizeof(DbSvc_TuningInfo_t));
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	for(ulCntTs= FixedTPNum; ulCntTs < ulNumTs; ++ulCntTs)
	{
		HxLOG_Print("xmgr_findsat_InitCheckList >>> TP (Freq:%d, SR:%d) ======================\n"
		, pstOption->pstTpArray[ulCntTs - FixedTPNum].stTuningParam.sat.ulFrequency
		, pstOption->pstTpArray[ulCntTs - FixedTPNum].stTuningParam.sat.ulSymbolRate );

		HxSTD_memcpy(&(pstSatCheck->pstTsCheckArray[ulCntTs].stTuneInfo),  &(pstOption->pstTpArray[ulCntTs - FixedTPNum].stTuningParam), sizeof(DbSvc_TuningInfo_t));
	}

	pstSatCheck->ulNumTp = ulNumTs;

	hErr = ERR_OK;

	xmgr_findsat_PrintCheckList (pstContents);

	return hErr;
}
#else
static HERROR xmgr_findsat_InitCheckList (FindSat_Contents_t *pstContents, HINT32 nAppHandle)
{
	HUINT32				 ulNumSat = 1;
	HERROR				 hErr;
	HUINT32				 ulCntTs, ulNumTs;
	MgrSearch_Option_t 		*pstOption = (MgrSearch_Option_t *)nAppHandle;
	SatCheckList_t		*pstSatCheck;


	pstContents->pstSatCheckArray = (SatCheckList_t *)OxAP_Malloc (sizeof(SatCheckList_t) * ulNumSat);
	if (pstContents->pstSatCheckArray == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	HxSTD_memset (pstContents->pstSatCheckArray, 0, sizeof(SatCheckList_t) * ulNumSat);
	pstContents->ulNumCheckSat = ulNumSat;

	pstContents->pstSatCheckArray[0].eSatType = pstContents->stAntInfo.eSatType;
	ulNumTs = pstOption->ulTpNum;

	if (ulNumTs == 0)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	pstSatCheck = &(pstContents->pstSatCheckArray[0]);
	pstSatCheck->pstTsCheckArray = (TsCheckList_t *)OxAP_Malloc (sizeof(TsCheckList_t) * ulNumTs);
	if (pstSatCheck->pstTsCheckArray == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	HxSTD_memset (pstSatCheck->pstTsCheckArray, 0, sizeof(TsCheckList_t) * ulNumTs);

	for(ulCntTs= 0; ulCntTs < ulNumTs; ++ulCntTs)
	{
		HxLOG_Info(">>> TP (Freq:%d, SR:%d) ======================\n"
		, pstOption->pstTpArray[ulCntTs].stTuningParam.sat.ulFrequency
		, pstOption->pstTpArray[ulCntTs].stTuningParam.sat.ulSymbolRate );

		HxSTD_memcpy(&(pstSatCheck->pstTsCheckArray[ulCntTs].stTuneInfo),  &(pstOption->pstTpArray[ulCntTs].stTuningParam), sizeof(DbSvc_TuningInfo_t));
	}

	pstSatCheck->ulNumTp = ulNumTs;

	hErr = ERR_OK;

	xmgr_findsat_PrintCheckList (pstContents);

	return hErr;
}
#endif


#else


static HERROR xmgr_findsat_LoadCheckList (FindSat_Contents_t *pstContents)
{
#if defined(CONFIG_3RD_XML)
	HUINT32 			 ulCntSat, ulNumSat;
	HXMLDOC 			 hXmlDocHandle;
	HXMLELEMENT 		 hRootElem, hSatElem;
	POINTER_LIST_T		*pstSatList, *pstCurrSatItem;
	HERROR				 hErr;

	hXmlDocHandle = (HXMLDOC)NULL;

	pstSatList = NULL;

	hErr = xmgr_findsat_PrefareDefaultDataFile (pstContents);
	if (hErr != ERR_XML_OK)
	{
		HxLOG_Error("xmgr_findsat_PrefareDefaultDataFile() error!");
		hErr = ERR_FAIL;
		goto Error;
	}

	hErr = PAL_XML_InitParser();
	if (hErr != ERR_XML_OK)
	{
		HxLOG_Error("PAL_XML_InitParser() error!");
		hErr = ERR_FAIL;
		goto Error;
	}

	hErr = PAL_XML_ParseFile (FINDSAT_CHECKLIST_FILE, &hXmlDocHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("PAL_XML_ParseFile() error!");
		hErr = ERR_FAIL;
		goto Error;
	}

	hErr = PAL_XML_GetRootElement (hXmlDocHandle, &hRootElem);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("PAL_XML_GetRootElement() error!");
		hErr = ERR_FAIL;
		goto Error;
	}

	hErr = PAL_XML_FindChildElementByName (hRootElem, FINDSAT_XML_TAG_SAT, &pstSatList);
	if (hErr != ERR_OK || pstSatList == NULL)
	{
		HxLOG_Error("PAL_XML_FindChildElementByName() error!");
		hErr = ERR_FAIL;
		goto Error;
	}

	ulNumSat = (HUINT32)UTIL_LINKEDLIST_GetNumListItemsInPointerList (pstSatList, NULL, NULL);
	if (ulNumSat == 0)
	{
		HxLOG_Error("UTIL_LINKEDLIST_GetNumListItemsInPointerList() error!");
		hErr = ERR_FAIL;
		goto Error;
	}

	pstContents->pstSatCheckArray = (SatCheckList_t *)AP_MEM_Alloc (sizeof(SatCheckList_t) * ulNumSat);
	if (pstContents->pstSatCheckArray == NULL)
	{
		HxLOG_Error("AP_MEM_Alloc() error!");
		hErr = ERR_FAIL;
		goto Error;
	}

	HxSTD_memset (pstContents->pstSatCheckArray, 0, sizeof(SatCheckList_t) * ulNumSat);

	pstCurrSatItem = pstSatList;
	ulCntSat = 0;

	while (pstCurrSatItem != NULL)
	{
		hSatElem = (HXMLELEMENT)UTIL_LINKEDLIST_GetContents (pstCurrSatItem);
		if (hSatElem != (HXMLELEMENT)NULL)
		{
			hErr = xmgr_findsat_ParseXmlSatData (hSatElem, &(pstContents->pstSatCheckArray[ulCntSat]));
			if (hErr == ERR_OK)
			{
				ulCntSat ++;
			}
		}

		pstCurrSatItem = UTIL_LINKEDLIST_GetNextPointer (pstCurrSatItem);
		if (pstCurrSatItem == pstSatList)					{ break; }
	}

	pstContents->ulNumCheckSat = ulCntSat;

	xmgr_findsat_PrintCheckList (pstContents);

	hErr = ERR_OK;

Error:
	if (pstSatList != NULL)
	{
		UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstSatList, NULL);
	}

	if (hXmlDocHandle != (HXMLDOC)NULL)
	{
		PAL_XML_FreeDocument (hXmlDocHandle);
	}

	return hErr;
#else
	return ERR_FAIL;
#endif

}




static HERROR xmgr_findsat_CheckVersion (FindSat_Contents_t *pstContents)
{
#if defined(CONFIG_3RD_XML)
	//HUINT32			ulValue = 0, ulVersion;
	HXMLDOC 			 hXmlDocHandle;
	HXMLELEMENT 		 hRootElem;
	HERROR				 hErr;

	hXmlDocHandle = (HXMLDOC)NULL;

	hErr = xmgr_findsat_PrefareDefaultDataFile (pstContents);
	if (hErr != ERR_XML_OK)
	{
		HxLOG_Error("PAL_XML_InitParser() error \n");
		hErr = ERR_FAIL;
		goto Error;
	}

	hErr = PAL_XML_InitParser();
	if (hErr != ERR_XML_OK)
	{
		HxLOG_Error("PAL_XML_InitParser() error \n");
		hErr = ERR_FAIL;
		goto Error;
	}

	hErr = PAL_XML_ParseFile (FINDSAT_CHECKLIST_FILE, &hXmlDocHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("PAL_XML_ParseFile() error \n");
		hErr = ERR_FAIL;
		goto Error;
	}

	hErr = PAL_XML_GetRootElement (hXmlDocHandle, &hRootElem);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("PAL_XML_GetRootElement() error \n");
		hErr = ERR_FAIL;
		goto Error;
	}


Error:
	if (hXmlDocHandle != (HXMLDOC)NULL)
	{
		PAL_XML_FreeDocument (hXmlDocHandle);
	}

	return hErr;
#else
	return ERR_FAIL;
#endif

}


static HERROR xmgr_findsat_PrefareDefaultDataFile (FindSat_Contents_t *pstContents)
{
#if defined(CONFIG_3RD_XML)
	int			 nSize;
	FILE		*pfFile;

#if 1 // 지워버린다.
	if(HLIB_FILE_Exist(FINDSAT_CHECKLIST_FILE))
	{
		HLIB_FILE_Delete((char *)FINDSAT_CHECKLIST_FILE);
	}
#else
	// XML Data File이 존재하는가 확인부터 한다.
	pfFile = fopen ((char *)FINDSAT_CHECKLIST_FILE, "r");
	if (pfFile)
	{
		// File이 이미 존재한다 : 이 파일을 사용하도록 한다.
		fclose (pfFile);
		return ERR_OK;
	}
#endif
	// File이 존재하지 않는다 : 새로 만들어야 한다.
	pfFile = fopen ((char *)FINDSAT_CHECKLIST_FILE, "wt");
	if (!pfFile)
	{
		// Fail to make a new file :
		HxLOG_Error("fopen() %s error \n", FINDSAT_CHECKLIST_FILE);
		return ERR_FAIL;
	}

	nSize = sizeof(s_aucFindSat_DefaultFileData);
	fwrite (s_aucFindSat_DefaultFileData, nSize, 1, pfFile);
	fclose (pfFile);

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}


static HERROR xmgr_findsat_ParseXmlSatData (HXMLELEMENT hSatElem, SatCheckList_t *pstSatCheck)
{
#if defined(CONFIG_3RD_XML)
	HUINT32				 ulCntTs, ulNumTs;
	HXMLELEMENT			 hTsElem;
	POINTER_LIST_T		*pstTsList, *pstCurrTsItem;
	HERROR				 hErr;

	pstTsList = NULL;

	// 1. Get the satellite attributes
	hErr = xmgr_findsat_GetSatAttributes (hSatElem, pstSatCheck);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("xmgr_findsat_GetSatAttributes() error \n");
		hErr = ERR_FAIL;
		goto Error;
	}

	// 2. Get the TS Information
	hErr = PAL_XML_FindChildElementByName (hSatElem, FINDSAT_XML_TAG_TS, &pstTsList);
	if (hErr != ERR_OK || pstTsList == NULL)
	{
		HxLOG_Error("PAL_XML_FindChildElementByName() error \n");
		hErr = ERR_FAIL;
		goto Error;
	}

	ulNumTs = (HUINT32)UTIL_LINKEDLIST_GetNumListItemsInPointerList (pstTsList, NULL, NULL);
	if (ulNumTs == 0)
	{
		HxLOG_Error("UTIL_LINKEDLIST_GetNumListItemsInPointerList() error \n");
		hErr = ERR_FAIL;
		goto Error;
	}

	pstSatCheck->pstTsCheckArray = (TsCheckList_t *)AP_MEM_Alloc (sizeof(TsCheckList_t) * ulNumTs);
	if (pstSatCheck->pstTsCheckArray == NULL)
	{
		HxLOG_Error("AP_MEM_Alloc() error \n");
		hErr = ERR_FAIL;
		goto Error;
	}

	HxSTD_memset (pstSatCheck->pstTsCheckArray, 0, sizeof(TsCheckList_t) * ulNumTs);

	ulCntTs = 0;
	pstCurrTsItem = pstTsList;

	while (pstCurrTsItem != NULL)
	{
		hTsElem = (HXMLELEMENT)UTIL_LINKEDLIST_GetContents (pstCurrTsItem);
		if (hTsElem != (HXMLELEMENT)NULL)
		{
			hErr = xmgr_findsat_ParseXmlTsData (hTsElem, &(pstSatCheck->pstTsCheckArray[ulCntTs]));
			if (hErr == ERR_OK)
			{
				ulCntTs ++;
			}
		}

		pstCurrTsItem = UTIL_LINKEDLIST_GetNextPointer (pstCurrTsItem);
		if (pstCurrTsItem == pstTsList)					{ break; }
	}

	pstSatCheck->ulNumTp = ulCntTs;

	hErr = ERR_OK;

Error:
	if (pstTsList != NULL)
	{
		UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstTsList, NULL);
	}

	return hErr;
#else
	return ERR_FAIL;
#endif
}

static HERROR xmgr_findsat_ParseXmlTsData (HXMLELEMENT hTsElem, TsCheckList_t *pstTsCheck)
{
#if defined(CONFIG_3RD_XML)
	HUINT32				 ulCntSvc, ulNumSvc;
	HUINT32				 ulValue;
	HXMLELEMENT			 hSvcElem;
	HUINT8				*pszArg;
	POINTER_LIST_T		*pstSvcList, *pstCurrSvcItem;
	HERROR				 hErr;

	pstSvcList = NULL;

	HxSTD_memset (pstTsCheck, 0, sizeof(TsCheckList_t));

	// 1. Get the TS Attribute
	hErr = xmgr_findsat_GetTsAttributes (hTsElem, pstTsCheck);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("xmgr_findsat_GetTsAttributes() error \n");
		hErr = ERR_FAIL;
		goto Error;
	}

	// 2. Get the SVC Information
	// This is not mandatory.
	hErr = PAL_XML_FindChildElementByName (hTsElem, FINDSAT_XML_TAG_SVC, &pstSvcList);
	if (hErr != ERR_OK || pstSvcList == NULL)
	{
		return ERR_OK;
	}

	ulNumSvc = (HUINT32)UTIL_LINKEDLIST_GetNumListItemsInPointerList (pstSvcList, NULL, NULL);
	if (ulNumSvc == 0)
	{
		HxLOG_Error("UTIL_LINKEDLIST_GetNumListItemsInPointerList() error \n");
		hErr = ERR_FAIL;
		goto Error;
	}

	if (ulNumSvc > MAX_SVCNUM_IN_CHECK)				{ ulNumSvc = MAX_SVCNUM_IN_CHECK; }

	ulCntSvc = 0;
	pstCurrSvcItem = pstSvcList;
	while (ulCntSvc < ulNumSvc)
	{
		hSvcElem = (HXMLELEMENT)UTIL_LINKEDLIST_GetContents (pstCurrSvcItem);
		if (hSvcElem != (HXMLELEMENT)NULL)
		{
			hErr = PAL_XML_GetAttrContents (hSvcElem, FINDSAT_XML_ATTR_SVCID, &pszArg);
			if (hErr == ERR_OK && pszArg != NULL)
			{
				sscanf ((char *)pszArg, "%u", &ulValue);
				pstTsCheck->ausSvcId[ulCntSvc] = (HUINT16)ulValue;
				PAL_XML_FreeAttrContents (pszArg);
				pszArg = NULL;

				ulCntSvc ++;
			}
		}

		pstCurrSvcItem = UTIL_LINKEDLIST_GetNextPointer (pstCurrSvcItem);
		if ((pstCurrSvcItem == pstSvcList) || (pstCurrSvcItem == NULL))
		{
			break;
		}
	}

	pstTsCheck->ulSvcNum = ulCntSvc;

	hErr = ERR_OK;

Error:
	if (pstSvcList != NULL)
	{
		UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstSvcList, NULL);
	}

	return hErr;
#else
	return ERR_FAIL;
#endif
}

static HERROR xmgr_findsat_GetSatAttributes (HXMLELEMENT hSatElem, SatCheckList_t *pstSatCheck)
{
#if defined(CONFIG_3RD_XML)
	HUINT32			 ulValue;
	HUINT8			*pszArg;
	HERROR			 hErr;

	// 1. Get the satellite attributes : type
	hErr = PAL_XML_GetAttrContents (hSatElem, FINDSAT_XML_ATTR_SATTYPE, &pszArg);
	if (hErr != ERR_OK || pszArg == NULL)
	{
		HxLOG_Error("PAL_XML_GetAttrContents() error \n");
		return ERR_FAIL;
	}

	hErr = xmgr_findsat_GetArgValues (s_astFindSat_SatTypeConvTable, pszArg, &ulValue);
	PAL_XML_FreeAttrContents (pszArg);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("PAL_XML_FreeAttrContents() error \n");
		return ERR_FAIL;
	}

	pstSatCheck->eSatType = (SVC_SatType_t)ulValue;

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}


static HERROR xmgr_findsat_GetTsAttributes (HXMLELEMENT hTsElem, TsCheckList_t *pstTsCheck)
{
#if defined(CONFIG_3RD_XML)
	HUINT32			 ulValue;
	HUINT8			*pszArg;
	HERROR			 hErr;

	// 1. Frequency
	hErr = PAL_XML_GetAttrContents (hTsElem, FINDSAT_XML_ATTR_TSFREQ, &pszArg);
	if (hErr != ERR_OK || pszArg == NULL)
	{
		HxLOG_Error("PAL_XML_FreeAttrContents() error \n");
		return ERR_FAIL;
	}

	sscanf ((char *)pszArg, "%u", &ulValue);
	pstTsCheck->stTuneInfo.sat.ulFrequency = ulValue;
	PAL_XML_FreeAttrContents (pszArg);
	pszArg = NULL;

	// 2. Polar
	hErr = PAL_XML_GetAttrContents (hTsElem, FINDSAT_XML_ATTR_TSPOLAR, &pszArg);
	if (hErr != ERR_OK || pszArg == NULL)
	{
		HxLOG_Error("PAL_XML_FreeAttrContents() error \n");
		return ERR_FAIL;
	}

	hErr = xmgr_findsat_GetArgValues (s_astFindSat_TsPolConvTable, pszArg, &ulValue);
	if (hErr != ERR_OK)
	{
		ulValue = eDxSAT_POLAR_AUTO;
	}

	pstTsCheck->stTuneInfo.sat.ePolarization = ulValue;
	PAL_XML_FreeAttrContents (pszArg);
	pszArg = NULL;

	// 3. Symbol Rate
	hErr = PAL_XML_GetAttrContents (hTsElem, FINDSAT_XML_ATTR_TSSR, &pszArg);
	if (hErr != ERR_OK || pszArg == NULL)
	{
		HxLOG_Error("PAL_XML_GetAttrContents() error \n");
		return ERR_FAIL;
	}

	sscanf ((char *)pszArg, "%u", &ulValue);
	pstTsCheck->stTuneInfo.sat.ulSymbolRate = ulValue;

	PAL_XML_FreeAttrContents (pszArg);
	pszArg = NULL;

	// 4. FEC
	hErr = PAL_XML_GetAttrContents (hTsElem, FINDSAT_XML_ATTR_TSFEC, &pszArg);
	if (hErr != ERR_OK || pszArg == NULL)
	{
		HxLOG_Error("PAL_XML_GetAttrContents() error \n");
		return ERR_FAIL;
	}

	hErr = xmgr_findsat_GetArgValues (s_astFindSat_TsFecConvTable, pszArg, &ulValue);
	if (hErr != ERR_OK)
	{
		ulValue = eDxSAT_CODERATE_AUTO;
	}

	pstTsCheck->stTuneInfo.sat.eFecCodeRate = (DxSat_CodeRate_e)ulValue;
	PAL_XML_FreeAttrContents (pszArg);
	pszArg = NULL;


	// 여기서부터는 Option 사항
	// 5. PSK
	hErr = PAL_XML_GetAttrContents (hTsElem, FINDSAT_XML_ATTR_TSPSK, &pszArg);
	if (hErr == ERR_OK && pszArg != NULL)
	{
		hErr = xmgr_findsat_GetArgValues (s_astFindSat_TsPskConvTable, pszArg, &ulValue);
		if (hErr != ERR_OK)
		{
			ulValue = eDxSAT_PSK_QPSK;
		}

		pstTsCheck->stTuneInfo.sat.ePskMod = (DxSat_CodeRate_e)ulValue;
		PAL_XML_FreeAttrContents (pszArg);
		pszArg = NULL;
	}

	// 6. Network ID
	hErr = PAL_XML_GetAttrContents (hTsElem, FINDSAT_XML_ATTR_TSNETID, &pszArg);
	if (hErr == ERR_OK && pszArg != NULL)
	{
		sscanf ((char *)pszArg, "%u", &ulValue);
		pstTsCheck->ulNetworkId = ulValue;

		PAL_XML_FreeAttrContents (pszArg);
		pszArg = NULL;
	}

	// 7. Original Network ID
	hErr = PAL_XML_GetAttrContents (hTsElem, FINDSAT_XML_ATTR_TS_ONID, &pszArg);
	if (hErr == ERR_OK && pszArg != NULL)
	{
		sscanf ((char *)pszArg, "%u", &ulValue);
		pstTsCheck->ulOnId = ulValue;

		PAL_XML_FreeAttrContents (pszArg);
		pszArg = NULL;
	}

	// 8. TS ID
	hErr = PAL_XML_GetAttrContents (hTsElem, FINDSAT_XML_ATTR_TS_TSID, &pszArg);
	if (hErr == ERR_OK && pszArg != NULL)
	{
		sscanf ((char *)pszArg, "%u", &ulValue);
		pstTsCheck->ulTsId = ulValue;

		PAL_XML_FreeAttrContents (pszArg);
		pszArg = NULL;
	}

#if defined(CONFIG_OP_VTC)||defined(CONFIG_OP_VAST)||defined(CONFIG_OP_MIDDLE_EAST)
	// 9. DVBS2 or NOT (ulValue == 1 : DVBS2 / 0 : DVBS)
	hErr = PAL_XML_GetAttrContents (hTsElem, FINDSAT_XML_ATTR_TSTRANS, &pszArg);
	if (hErr != ERR_OK || pszArg == NULL)
	{
		HxLOG_Error("PAL_XML_GetAttrContents() error \n");
		return ERR_FAIL;
	}
	sscanf ((char *)pszArg, "%u", &ulValue);
	if(ulValue)
	{
		pstTsCheck->stTuneInfo.sat.eTransSpec = eDxSAT_TRANS_DVBS2;
	}
	else
	{
		pstTsCheck->stTuneInfo.sat.eTransSpec = eDxSAT_TRANS_DVBS;
	}
	PAL_XML_FreeAttrContents (pszArg);
	pszArg = NULL;
#endif

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

static HERROR xmgr_findsat_GetArgValues (ArgConvTable_t *pstArgTable, HUINT8 *szArg, HUINT32 *pulValue)
{
	HUINT32		 ulIndex;

	ulIndex = 0;
	MWC_UTIL_ConvertStringUpper (szArg);

	while (pstArgTable[ulIndex].szArg != NULL)
	{
		if (MWC_UTIL_DvbStrcmp ((HUINT8 *)pstArgTable[ulIndex].szArg, szArg) == 0)
		{
			*pulValue = pstArgTable[ulIndex].ulValue;
			return ERR_OK;
		}

		ulIndex++;
	}

	return ERR_FAIL;
}



#endif





static HERROR xmgr_findsat_PrintCheckList (FindSat_Contents_t *pstContents)
{
	HUINT32			 ulSatIdx, ulTsIdx, ulSvcIdx;
	SatCheckList_t	*pstSatCheck;
	TsCheckList_t	*pstTsCheck;

	for (ulSatIdx = 0; ulSatIdx < pstContents->ulNumCheckSat; ulSatIdx++)
	{
		pstSatCheck = &(pstContents->pstSatCheckArray[ulSatIdx]);

		HxLOG_Critical("=== Sat %d [%d] ======================\n", ulSatIdx, pstSatCheck->eSatType);

		for (ulTsIdx = 0; ulTsIdx < pstSatCheck->ulNumTp; ulTsIdx++)
		{
			pstTsCheck = &(pstSatCheck->pstTsCheckArray[ulTsIdx]);

			HxLOG_Critical(" - TS (%02d) freq %d, SR %d, NetId %d, (OnID %04x:TsID %04x)\n", ulTsIdx,
																	pstTsCheck->stTuneInfo.sat.ulFrequency,
																	pstTsCheck->stTuneInfo.sat.ulSymbolRate,
																	pstTsCheck->ulNetworkId,
																	pstTsCheck->ulOnId,
																	pstTsCheck->ulTsId);
			for (ulSvcIdx = 0; ulSvcIdx < pstTsCheck->ulSvcNum; ulSvcIdx++)
			{
				HxLOG_Critical("     (SVC ID : %d)\n", pstTsCheck->ausSvcId[ulSvcIdx]);
			}
		}

		HxLOG_Critical("=====================================\n");

	}

	return ERR_OK;
}


#define	_____CHECKLIST_MANAGEMENT_FUNCTIONS_____

static HERROR xmgr_findsat_GetCurrentCheckTuningInfo (FindSat_Contents_t *pstContents, DxTuneParam_t *pstTuneInfo)
{
	SatCheckList_t	*pstSatCheck;
	TsCheckList_t	*pstTsCheck;

	if (pstContents->ulCurrSatCnt < pstContents->ulNumCheckSat)
	{
		pstSatCheck = &(pstContents->pstSatCheckArray[pstContents->ulCurrSatCnt]);
		if (pstContents->ulCurrTsCnt < pstSatCheck->ulNumTp)
		{
			pstTsCheck = &(pstSatCheck->pstTsCheckArray[pstContents->ulCurrTsCnt]);

			return MGR_SEARCHUTIL_ConvertSatSvcTuningInfoToChTuningParam (pstContents->ulTsPortId,
																		&(pstContents->stAntInfo),
																		&(pstTsCheck->stTuneInfo),
																		pstTuneInfo);
		}
	}

	return ERR_FAIL;
}

static HERROR xmgr_findsat_GetCurrentSatCheckItem (FindSat_Contents_t *pstContents, SatCheckList_t *pstSatItem)
{
	if (pstContents->ulCurrSatCnt < pstContents->ulNumCheckSat)
	{
		*pstSatItem = pstContents->pstSatCheckArray[pstContents->ulCurrSatCnt];
		return ERR_OK;
	}

	return ERR_FAIL;
}

static HERROR xmgr_findsat_GetCurrentTsCheckItem (FindSat_Contents_t *pstContents, TsCheckList_t *pstTsItem)
{
	SatCheckList_t *pstSatItem;

	if (pstContents->ulCurrSatCnt < pstContents->ulNumCheckSat)
	{
		pstSatItem = &(pstContents->pstSatCheckArray[pstContents->ulCurrSatCnt]);

		if (pstContents->ulCurrTsCnt < pstSatItem->ulNumTp)
		{
			*pstTsItem = pstSatItem->pstTsCheckArray[pstContents->ulCurrTsCnt];
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

static HERROR xmgr_findsat_GotoNextCheckList (FindSat_Contents_t *pstContents)
{
	HERROR		 hErr;

	HxLOG_Info("[xmgr_findsat_GotoNextCheckList] \n");

	hErr = xmgr_findsat_SetNextCheckTp (pstContents);
	if (hErr == ERR_OK)
	{
		return ERR_OK;
	}

	// 현재 위성에 해당하는 모든 TP를 넘었다 :
	// 현재 테스트한 Satellite는 해당되지 않는다고 판단됨 : 다음 Check List로 검사해봐야 한다.
	hErr = xmgr_findsat_SetNextCheckSat (pstContents);
	if (hErr == ERR_OK)
	{
		return ERR_OK;
	}

	// 현재 Check List를 다 비교했지만 결과가 나오지 않았다 : 현재 DiSEqC를 SAT_NONE으로 놓고 다음으로 넘어간다.
	xmgr_findsat_SetCheckResult (pstContents, eSatType_NONE);

	xmgr_findsat_InitCheckListPosition (pstContents);
	hErr = xmgr_findsat_SetNextDiseqc (pstContents);
	if (hErr == ERR_OK)
	{
		return ERR_OK;
	}

	// 다음 DiSEqC이 없다 : 마지막까지 다 함.
	HxLOG_Info("All the check case finished\n");

	pstContents->eState = eSatFindState_Finish;
	return ERR_FAIL;
}

static HERROR xmgr_findsat_SetNextCheckTp (FindSat_Contents_t *pstContents)
{
	SatCheckList_t	*pstSatCheck;

	HxLOG_Info("pstContents->ulCurrSatCnt: %d, pstContents->ulNumCheckSat: %d \n", pstContents->ulCurrSatCnt, pstContents->ulNumCheckSat);

	if (pstContents->ulCurrSatCnt < pstContents->ulNumCheckSat)
	{
		pstSatCheck = &(pstContents->pstSatCheckArray[pstContents->ulCurrSatCnt]);
		HxLOG_Info("pstContents->ulCurrTsCnt: %d, pstContents->ulNumCheckSat: %d, pstSatCheck->ulNumTp: %d \n", pstContents->ulCurrTsCnt, pstContents->ulNumCheckSat, pstSatCheck->ulNumTp);
		if (pstContents->ulCurrTsCnt < pstSatCheck->ulNumTp)
		{
			pstContents->ulCurrTsCnt ++;

			if (pstContents->ulCurrTsCnt < pstSatCheck->ulNumTp)
			{
				return ERR_OK;
			}
		}
	}

	return ERR_FAIL;
}

static HERROR xmgr_findsat_SetNextCheckSat (FindSat_Contents_t *pstContents)
{
	HxLOG_Info("pstContents->ulCurrSatCnt: %d, pstContents->ulNumCheckSat: %d \n", pstContents->ulCurrSatCnt, pstContents->ulNumCheckSat);

	if (pstContents->ulCurrSatCnt < pstContents->ulNumCheckSat)
	{
		pstContents->ulCurrSatCnt ++;
		pstContents->ulCurrTsCnt = 0;

		if (pstContents->ulCurrSatCnt < pstContents->ulNumCheckSat)
		{
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

static HERROR xmgr_findsat_SetNextDiseqc (FindSat_Contents_t *pstContents)
{
	DxSAT_DiseqcInput_e	 eNextDiseqc;

	HxLOG_Info("pstContents->stAntInfo.utAntInfo.stDiseqcAnt.eDiseqcInput %d \n", pstContents->stAntInfo.utAntInfo.stDiseqcAnt.eDiseqcInput);

	switch (pstContents->stAntInfo.utAntInfo.stDiseqcAnt.eDiseqcInput)
	{
	case eDxDISEQC_INPUT_A:
		HxLOG_Info("eNextDiseqc = eDxDISEQC_INPUT_B \n");
		eNextDiseqc = eDxDISEQC_INPUT_B;
		break;

	case eDxDISEQC_INPUT_B:
		HxLOG_Info("eNextDiseqc = eDxDISEQC_INPUT_C \n");
		eNextDiseqc = eDxDISEQC_INPUT_C;
		break;

	case eDxDISEQC_INPUT_C:
		HxLOG_Info("eNextDiseqc = eDxDISEQC_INPUT_D \n");
		eNextDiseqc = eDxDISEQC_INPUT_D;
		break;

	case eDxDISEQC_INPUT_D:
	default:
		HxLOG_Info("ther is no eNextDiseqc \n");
		return ERR_FAIL;
	}

	xmgr_findsat_MakeDefaultAntInfo (pstContents, eNextDiseqc);
	return ERR_OK;
}

static HERROR xmgr_findsat_SetCheckResult (FindSat_Contents_t *pstContents, SVC_SatType_t eSatType)
{
	HINT32		 nIndex;

	HxLOG_Info("pstContents->stAntInfo.utAntInfo.stDiseqcAnt.eDiseqcInput: %d, eSatType: %d \n", pstContents->stAntInfo.utAntInfo.stDiseqcAnt.eDiseqcInput, eSatType);

	nIndex = (HINT32)pstContents->stAntInfo.utAntInfo.stDiseqcAnt.eDiseqcInput - eDxDISEQC_INPUT_A;

	if (nIndex < 0 || nIndex >= 4)
	{
		return ERR_FAIL;
	}

	HxLOG_Info("Set the DiSEqC %c as the satellite %d\n", 'A' + nIndex, eSatType);

	pstContents->aeSatType[nIndex] = eSatType;
	return ERR_OK;
}


#define	_____DETERMINE_FUNCTIONS_____

static HERROR xmgr_findsat_CheckSiResults (FindSat_Contents_t *pstContents)
{
	HUINT32			 ulCheckCnt, ulSvcCnt, ulSvcNum;
	SatCheckList_t	 stSatItem;
	TsCheckList_t	 stTsItem;
	DbSvc_NetInfo_t	 stNetInfo;
	DbSvc_TsInfo_t	 stTsInfo;
	DbSvc_Info_t	*pstSvcArray;
	HERROR			 hErr;
	HUINT32			 ulInfoNum = 0;

	stTsItem.ulNetworkId = 0;
	stTsItem.ulSvcNum = 0;

	hErr  = xmgr_findsat_GetCurrentSatCheckItem (pstContents, &stSatItem);
	hErr |= xmgr_findsat_GetCurrentTsCheckItem (pstContents, &stTsItem);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n\n");
		return ERR_FAIL;
	}

	//HxLOG_Print("[xmgr_findsat_CheckSiResults] NID=%d OID=%d TID=%d\n", stTsItem.ulNetworkId, stTsItem.ulOnId, stTsItem.ulTsId);

	// Network ID를 체크한다.
	if (stTsItem.ulNetworkId != 0)
	{
		hErr = SVC_SI_GetSearchedResults(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Network, 1, &ulInfoNum, (void *)&stNetInfo);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("[FINDSAT] No Network info in SI\n");
			return ERR_FAIL;
		}

		if (stTsItem.ulNetworkId != (HUINT32)stNetInfo.usNetId)
		{
			HxLOG_Error("[FINDSAT] Network ID difference (SI:%04x, Check:%04x)\n", stNetInfo.usNetId, stTsItem.ulNetworkId);
			return ERR_FAIL;
		}

		HxLOG_Error("[FINDSAT] Network ID is same! (%d)\n", stTsItem.ulNetworkId);
	}

	// ON ID / TS ID 를 체크한다.
	if (stTsItem.ulOnId != 0 || stTsItem.ulTsId != 0)
	{
		hErr = SVC_SI_GetSearchedResults(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Transponder, 1, &ulInfoNum, (void *)&stTsInfo);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("[FINDSAT] No TsInfo in the TP \n");
			return ERR_FAIL;
		}

		HxLOG_Print("[FINDSAT] Data Ts Info (OnId:0x%04x, TsId:0x%04x)\n", stTsItem.ulOnId, stTsItem.ulTsId);
		HxLOG_Print("[FINDSAT] Live Ts Info (OnId:0x%04x, TsId:0x%04x)\n", stTsInfo.usOnId, stTsInfo.usTsId);

		if ((stTsItem.ulOnId != 0) && (stTsItem.ulOnId != (HUINT32)stTsInfo.usOnId))
		{
			HxLOG_Error("[FINDSAT] OnID Different (Data:%08x, Live:%08x) \n", stTsItem.ulOnId, stTsInfo.usOnId);
			return ERR_FAIL;
		}

		if ((stTsItem.ulTsId != 0) && (stTsItem.ulTsId != (HUINT32)stTsInfo.usTsId))
		{
			HxLOG_Error("[FINDSAT] TsId Different (Data:%08x, Live:%08x) \n", stTsItem.ulTsId, stTsInfo.usTsId);
			return ERR_FAIL;
		}
	}

	// Check 할 Service 가 있다
	if (stTsItem.ulSvcNum > 0)
	{
		hErr = SVC_SI_CountSearchedResult(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Service, &ulSvcNum);
		if (hErr != ERR_OK || ulSvcNum < stTsItem.ulSvcNum)
		{
			HxLOG_Error("[FINDSAT] Service number is not enough in the TP (%d)\n", ulSvcNum);
			return ERR_FAIL;
		}

		pstSvcArray = (DbSvc_Info_t *)OxAP_Malloc (sizeof(DbSvc_Info_t) * ulSvcNum);
		if (pstSvcArray == NULL)
		{
			HxLOG_Critical("\n\n\n");
			return ERR_FAIL;
		}

		hErr = SVC_SI_GetSearchedResults(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Service, ulSvcNum, &ulInfoNum, (void *)pstSvcArray);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("[FINDSAT] No Service in the TP \n");
			OxAP_Free (pstSvcArray);
			return ERR_FAIL;
		}

		for (ulCheckCnt = 0; ulCheckCnt < stTsItem.ulSvcNum; ulCheckCnt ++)
		{
			for (ulSvcCnt = 0; ulSvcCnt < ulSvcNum; ulSvcCnt++)
			{
				if (stTsItem.ausSvcId[ulCheckCnt] == DbSvc_GetSvcId(&pstSvcArray[ulSvcCnt]))
				{
					HxLOG_Print("[FINDSAT]    Service Found (%04x) :\n", stTsItem.ausSvcId[ulCheckCnt]);
					break;
				}
			}

			// 해당되는 Service가  현재 TP 에 없다. >>  잘못된 TP
			if (ulSvcCnt >= ulSvcNum)
			{
				break;
			}
		}

		OxAP_Free (pstSvcArray);

		// Check List 내의 Service 들이 없을 경우 :
		if (ulCheckCnt < stTsItem.ulSvcNum)
		{
			HxLOG_Error("[FINDSAT] Services didn't match :\n");
			return ERR_FAIL;
		}
	}

	// 이 TP는 Check List의 그 TP가 맞다고 판단 :
	return ERR_OK;
}

static HERROR xmgr_findsat_DetermineSatellites (FindSat_Contents_t *pstContents)
{
	HUINT16		 usAntIndex;
	HUINT32		 ulCount;
	DbSvc_AntInfo_t	 stAntInfo;
#if !defined(FINDSAT_CHECKLIST_ARGUMENT)
	HERROR		hErr = ERR_FAIL;
#endif
	HxLOG_Trace();

	// 1. 하나라도 찾지 못했는가?
	for (ulCount = 0; ulCount < 4; ulCount++)
	{
		if (pstContents->aeSatType[ulCount] > eSatType_ALL && pstContents->aeSatType[ulCount] < eSatType_LAST)
		{
			break;
		}
	}

	if (ulCount == 4)
	{
		HxLOG_Info("No satellite found\n");
		return ERR_FAIL;
	}

	// 2. DiSEqC A~D의 내용이 다 동일하다 : LNB 안테나임을 의미
	for (ulCount = 1; ulCount < 4; ulCount++)
	{
		if (pstContents->aeSatType[ulCount] != pstContents->aeSatType[0])
		{
			break;
		}
	}

	if (ulCount == 4)
	{
		HxLOG_Info("Antenna Is LNB.\n");

		HxSTD_memset (&stAntInfo, 0, sizeof(DbSvc_AntInfo_t));

		stAntInfo.eSatType				= pstContents->aeSatType[0];
		MGR_ANTENNA_GetSatelliteShortName (stAntInfo.eSatType, stAntInfo.szName, MAX_ANT_NAME);
		stAntInfo.eAntennaType 				= eDxANT_TYPE_LNB_ONLY;
		stAntInfo.utAntInfo.stLnbAnt.ulLnbFreq		= CH_UNIVERSAL_LNB_FREQ;
		stAntInfo.utAntInfo.stLnbAnt.eLnbVoltage 	= eDxLNB_VOLT_STD;

#if defined(FINDSAT_CHECKLIST_ARGUMENT)
			xmgr_findsat_AddSatellite(&stAntInfo, &usAntIndex);
#else
			DB_SVC_AddAntInfo (&stAntInfo, &usAntIndex);
#endif
		MGR_SEARCHUTIL_SetSatAntennaTypeToDb (eUiAntType_LNB_Only);
		DB_SVC_AntennaSync();		//DB_PARAM_Sync();

		return ERR_OK;
	}

	// 각각 다른 안테나 : DiSEqC로 처리
	HxLOG_Info("Antenna Is DiSEqC.\n");

	// A, C가 같고 B, D가 같다 : DiSEqC A와 B 만으로 구분되는 스위치이다.
	if ((pstContents->aeSatType[0] == pstContents->aeSatType[2]) &&
		(pstContents->aeSatType[1] == pstContents->aeSatType[3]))
	{
		pstContents->aeSatType[2] = eSatType_NONE;
		pstContents->aeSatType[3] = eSatType_NONE;
	}

	for (ulCount = 0; ulCount < 4; ulCount++)
	{

		HxLOG_Info("pstContents->aeSatType[%d]: %d\n", ulCount, pstContents->aeSatType[ulCount]);
		if (pstContents->aeSatType[ulCount] > eSatType_ALL && pstContents->aeSatType[ulCount] < eSatType_LAST)
		{
			stAntInfo.eSatType				= pstContents->aeSatType[ulCount];
			MGR_ANTENNA_GetSatelliteName (stAntInfo.eSatType, stAntInfo.szName, MAX_ANT_NAME);

			stAntInfo.eAntennaType 				= eDxANT_TYPE_DISEQC;
			stAntInfo.utAntInfo.stDiseqcAnt.ulLnbFreq		= CH_UNIVERSAL_LNB_FREQ;
			stAntInfo.utAntInfo.stDiseqcAnt.eLnbVoltage	= eDxLNB_VOLT_STD;
			stAntInfo.utAntInfo.stDiseqcAnt.b22kTone 		= FALSE;
			stAntInfo.utAntInfo.stDiseqcAnt.eDiseqcVer	= eDxDISEQC_VER_1_0;
			stAntInfo.utAntInfo.stDiseqcAnt.eDiseqcInput	= (DxSAT_DiseqcInput_e)(eDxDISEQC_INPUT_A + ulCount);
			stAntInfo.utAntInfo.stDiseqcAnt.eToneBurst	= eDxTONE_BURST_NONE;

#if defined(FINDSAT_CHECKLIST_ARGUMENT)
			xmgr_findsat_AddSatellite(&stAntInfo, &usAntIndex);
#else

			hErr = DB_SVC_AddAntInfo (&stAntInfo, &usAntIndex);
			HxLOG_Info("DB_SVC_AddAntInfo, hErr: %d, usAntIndex: %d \n", hErr, usAntIndex);
#endif
			//break;
		}
	}

	MGR_SEARCHUTIL_SetSatAntennaTypeToDb (eUiAntType_DiSEqC_1_0);
	DB_SVC_AntennaSync();	//DB_PARAM_Sync();

	return ERR_OK;
}

#if defined(FINDSAT_CHECKLIST_ARGUMENT)
static HERROR xmgr_findsat_AddSatellite(DbSvc_AntInfo_t	*pstAntInfo, HUINT16 	*pusAntIdx)
{
	HINT32				 i, cnt;
	HINT32				*idxList = NULL;
	HERROR				 hErr, hResult = ERR_FAIL;

	hErr = DB_SVC_FindAllAntIdxList(&cnt, &idxList);
	if ( (ERR_OK == hErr) && (NULL != idxList))
	{
		DbSvc_AntInfo_t	stAntInfo;

		for (i = 0; i < cnt; i++)
		{
			if (DB_SVC_GetAntInfo(idxList[i], &stAntInfo) == ERR_OK)
			{
				if(pstAntInfo->eSatType == stAntInfo.eSatType)
				{
#if !defined(CONFIG_PROD_HMS1000S) && !defined(CONFIG_PROD_HMSLITE) && !defined(CONFIG_PROD_ICORDPRO) && !defined(CONFIG_PROD_HDNEO) && !defined(CONFIG_PROD_HGS1000S)
					HLIB_STD_StrUtf8NCpy (pstAntInfo->szName, "FreesatAnt", MAX_ANT_NAME);
#endif
					DB_SVC_UpdateAntInfo(idxList[i], pstAntInfo);

					hResult = ERR_OK;
					goto END_FUNC;
				}
			}
		}

		DB_SVC_AddAntInfo (pstAntInfo, pusAntIdx);

		hResult = ERR_OK;
		goto END_FUNC;
	}

END_FUNC:
	if (NULL != idxList)
	{
		DB_SVC_FreeAntList (idxList);
	}

	return hResult;
}
#endif

#define	_____MESSAGE_FUNCTIONS_____

// UI 에게 명령을 내리는 Message
static HERROR xmgr_findsat_SendUpdateDisplayMsg (Handle_t hAction)
{
	return (HERROR)BUS_PostMessage (NULL, eMEVT_SEARCH_STATUS_CHANGED, (Handle_t)hAction, 0, 0, 0);
}

// UI 에게 상태를 알려주는 Message
static HERROR xmgr_findsat_SendSearchStartedMsg (Handle_t hAction)
{
	return (HERROR)BUS_PostMessage (NULL, eMEVT_SEARCH_STARTED, (Handle_t)hAction, (HINT32)MGR_SEARCHUTIL_MakeCurrentAppHandle(), 0, 0);
}

static HERROR xmgr_findsat_SendSearchFinishedMsg (Handle_t hAction, HBOOL bSuccess)
{
	return (HERROR)BUS_PostMessage (NULL, eMEVT_SEARCH_FINISHED, (Handle_t)hAction, (HINT32)MGR_SEARCHUTIL_MakeCurrentAppHandle(), (HINT32)bSuccess, 0);
}

static HERROR xmgr_findsat_SendSearchSaveFinishedMsg (Handle_t hAction)
{
	return (HERROR)BUS_PostMessage (NULL, eMEVT_SEARCH_SAVE_FINISHED, (Handle_t)hAction, (HINT32)MGR_SEARCHUTIL_MakeCurrentAppHandle(), 0, 0);
}

static HERROR xmgr_findsat_SendSearchActionDestroyedMsg (Handle_t hAction)
{
	return (HERROR)BUS_PostMessage (NULL, eMEVT_SEARCH_ACTION_DESTROYED, (Handle_t)hAction, (HINT32)MGR_SEARCHUTIL_MakeCurrentAppHandle(), 0, 0);
}


#define	_____PROCESSING_FUNCTIONS_____

static HERROR xmgr_findsat_TuneCheckTp (FindSat_Contents_t *pstContents)
{
	DxTuneParam_t		 stTuneInfo;
	SvcRsc_Info_t			 stRscInfo;
	HERROR				 hErr;

	MGR_RSC_GetResourceInfo (pstContents->ulActionId, &stRscInfo);

	while (pstContents->eState != eSatFindState_Finish)
	{
		// 현재 해당하는 Check TP가 Tuning 가능한가?
		hErr = xmgr_findsat_GetCurrentCheckTuningInfo (pstContents, &stTuneInfo);
		HxLOG_Info("Tune the TP (Freq:%d, SR:%d) hErr = %X\n", stTuneInfo.sat.tuningInfo.ulFrequency, stTuneInfo.sat.tuningInfo.ulSymbolRate, hErr);
		if (hErr == ERR_OK)
		{
			HxLOG_Info("Tune the TP (Freq:%d, SR:%d)\n", stTuneInfo.sat.tuningInfo.ulFrequency, stTuneInfo.sat.tuningInfo.ulSymbolRate);

			hErr  = SVC_CH_StopTune (pstContents->hAction, (HUINT16)-1, eSyncMode);
			if (hErr == ERR_OK)
			{
				xmgr_findsat_IncreaseSessionVersion (pstContents);

				hErr = SVC_CH_StartTune (pstContents->hAction, &stRscInfo, (HUINT16)-1, &stTuneInfo);
				if (hErr == ERR_OK)
				{
					pstContents->eState = eSatFindState_Tuning;
					return ERR_OK;
				}
			}
		}

		// Tuning 실패 혹은 현재 Check TP를 얻어오는 데 실패
		// 다음 TP Tuning 시도
		hErr = xmgr_findsat_GotoNextCheckList (pstContents);
		if (hErr == ERR_OK)
		{
			continue;
		}

		break;
	}

	return ERR_OK;
}


#define	_____INTERNAL_MSG_PROCESS_FUNCTION_____

//eMEVT_BUS_CREATE
static HERROR xmgr_findsat_MsgGwmCreate (FindSat_Contents_t *pstContents, HINT32 nAppHandle, HINT32 nParam)
{
	HERROR		 hErr;

	HxLOG_Trace();
//	HLIB_LOG_SetLevel(HxLOG_DOMAIN, HxLOG_ALL);
	BUS_MGR_SetMgrName (BUS_MGR_GetThis(), "xproc_search_SatFind");

	// 1. Initialize the contents
	hErr = xmgr_findsat_InitContents (pstContents);
	if (hErr != ERR_OK)
	{
		BUS_MGR_Destroy (NULL);
		return MESSAGE_BREAK;
	}

#if defined(FINDSAT_CHECKLIST_ARGUMENT)

#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE) || defined(CONFIG_PROD_ICORDPRO) || defined(CONFIG_PROD_HDNEO) || defined(CONFIG_PROD_HGS1000S) // MOONSG_sss
	pstContents->stAntInfo.eSatType = nParam;
#endif
	xmgr_findsat_InitCheckList (pstContents, nAppHandle);

#else
	// 2. Load the check list :
	xmgr_findsat_CheckVersion (pstContents);

	hErr = xmgr_findsat_LoadCheckList (pstContents);
	if (hErr != ERR_OK)
	{
		BUS_MGR_Destroy (NULL);
		return MESSAGE_BREAK;
	}
#endif
	// 3. Antenna Setting을 DiSEqc A부터 시작한다.
#if defined(FINDSAT_CHECKLIST_ARGUMENT)
#else
	DB_SVC_DeleteAllAntInfo();
#endif
	xmgr_findsat_MakeDefaultAntInfo (pstContents, eDxDISEQC_INPUT_A);
	xmgr_findsat_InitCheckListPosition (pstContents);

	return ERR_BUS_NO_ERROR;
}

// eMEVT_BUS_DESTROY:
static HERROR xmgr_findsat_MsgGwmDestroyed (FindSat_Contents_t *pstContents)
{
	// Destroy the contents
	if (pstContents->eState == eSatFindState_WaitingSi)
	{
		// Stop SI & Cancel the search :
		SVC_SI_StopSearch(pstContents->hAction, TRUE);
	}

	pstContents->eState = eSatFindState_Finish;
	xmgr_findsat_ReleaseAction (pstContents);

	xmgr_findsat_SendSearchActionDestroyedMsg(pstContents->hAction);

	xmgr_findsat_DestroyContents (pstContents);

	return MESSAGE_BREAK;
}

// eMEVT_SEARCH_ACTION_START:
static HERROR xmgr_findsat_MsgAppSearchActionStart (FindSat_Contents_t *pstContents, HINT32 nAppHandle)
{
	HUINT32	ulTunerGroupId;
	HERROR			hErr;


	(void)nAppHandle;

	// 1. Take the Action
	pstContents->ulActionId	= MGR_ACTION_GetMainActionId();
	pstContents->hAction	= SVC_PIPE_GetActionHandle (pstContents->ulActionId);

	hErr = xmgr_findsat_TakeAction (pstContents->ulActionId);
	if (hErr != ERR_OK)
	{
		BUS_MGR_Destroy (NULL);
		return MESSAGE_BREAK;
	}

	hErr = SVC_PIPE_GetResourceId (pstContents->ulActionId, eRxRSCTYPE_TUNER_GROUP, &ulTunerGroupId);
	if (hErr != ERR_OK)
	{
		MGR_ACTION_Release (pstContents->ulActionId, eActionType_SEARCH);
		BUS_MGR_Destroy (NULL);
		return MESSAGE_BREAK;
	}

	pstContents->ulTsPortId = (HUINT32)ulTunerGroupId;
	pstContents->bActionTaken = TRUE;

	// 2. 첫 Check List 부터 Tuning 시작
	hErr = xmgr_findsat_TuneCheckTp (pstContents);

	// 시작했음을 UI 에게 알려줘야 한다.
	xmgr_findsat_SendSearchStartedMsg(pstContents->hAction);

	if (hErr != ERR_OK)
	{
		// 일반 Search Action 과 달리 끝나자 마자 결과를 저장해야 한다.
		hErr = xmgr_findsat_DetermineSatellites (pstContents);
		xmgr_findsat_ReleaseAction (pstContents);
		if(ERR_OK != hErr)
		{
			xmgr_findsat_SendSearchFinishedMsg (pstContents->hAction, FALSE);
		}
		else
		{
			xmgr_findsat_SendSearchFinishedMsg (pstContents->hAction, TRUE);
		}
	}

	xmgr_findsat_ChangeTpText (pstContents);
	xmgr_findsat_ChangeProgressNumber (pstContents);
	xmgr_findsat_SendUpdateDisplayMsg(pstContents->hAction);

	return ERR_BUS_NO_ERROR;
}


// eMEVT_SEARCH_SAVE_RESULT
static HERROR xmgr_findsat_MsgAppSearchSaveResult (FindSat_Contents_t *pstContents, HINT32 nAppHandle)
{
	// UI -> Search Action 으로 오는 메시지는 다른 Search Action 으로도 갈 수 있는 메시지이기 때문에 (NULL)
	// 절대로 MESSAGE_BREAK 를 사용하면 안된다.
	(void)pstContents;
	(void)nAppHandle;

	// 실제로 Save 하지 않았지만 UI APP에서 이 메시지를 Action이 처리했다는 걸 알리기 위해 메시지를 보낸다.
	xmgr_findsat_SendSearchSaveFinishedMsg(pstContents->hAction);

	return ERR_OK;
}

// eMEVT_SEARCH_CANCEL
static HERROR xmgr_findsat_MsgAppSearchCancel (FindSat_Contents_t *pstContents, HINT32 nAppHandle)
{
	(void)nAppHandle;

	// UI -> Search Action 으로 오는 메시지는 다른 Search Action 으로도 갈 수 있는 메시지이기 때문에 (NULL)
	// 절대로 MESSAGE_BREAK 를 사용하면 안된다.

	if (pstContents->eState == eSatFindState_WaitingSi)
	{
		// Stop SI & Cancel the search :
		SVC_SI_StopSearch(pstContents->hAction, TRUE);
	}

	pstContents->eState = eSatFindState_Finish;
	xmgr_findsat_SendUpdateDisplayMsg(pstContents->hAction);
	xmgr_findsat_ReleaseAction (pstContents);
	xmgr_findsat_SendSearchFinishedMsg(pstContents->hAction, FALSE);
	return ERR_OK;
}


// eMEVT_BUS_TIMER:
static HERROR xmgr_findsat_MsgGwmTimer (FindSat_Contents_t *pstContents, HINT32 nTimerId)
{
	(void)pstContents;
	(void)nTimerId;

	return ERR_BUS_NO_ERROR;
}

// eSEVT_CH_LOCKED:
static HERROR xmgr_findsat_EvtChLocked (FindSat_Contents_t *pstContents, Handle_t hAction)
{
	HERROR		 hErr;

	(void)hAction;

	switch (pstContents->eState)
	{
	case eSatFindState_Tuning:
		// Tuning 성공 : SI를 걸도록 한다.
		// Satellite Auto Detection 에서는 TsUniqId 와 Network ID 외에는 의미없다.
		HxLOG_Info("Start SI\n");

		{
			SvcSi_SearchSpec_t		stSrchSpec;
			SvcSi_SearchOption_t	stSrchOption;

			HxSTD_memset(&stSrchOption, 0x00, sizeof(SvcSi_SearchOption_t));
			stSrchOption.ulSearchType	= (HUINT32)eSVCSI_SEARCHTYPE_Normal;
			stSrchOption.ulSvcType		= (HUINT32)eDxSVC_TYPE_All;
			stSrchOption.ulCasType		= (HUINT32)eDxCAS_TYPE_All;
			stSrchOption.hSession		= (SvcSi_SessionH_t)HANDLE_NULL;	// not use
			stSrchOption.ulNetId		= SvcSi_NETID_SEARCH_NITACT;

			hErr = MGR_SEARCH_GetSiSpec(&stSrchOption, &stSrchSpec);
			if (hErr != ERR_FAIL)
			{
				hErr = SVC_SI_StartSearch(pstContents->hAction,
												&stSrchSpec,
												&stSrchOption,
												TRUE);
			}

		    if (hErr == ERR_OK)
		    {
			    pstContents->eState = eSatFindState_WaitingSi;
		    }
		    else
		    {
			    hErr = xmgr_findsat_GotoNextCheckList (pstContents);
			    if (hErr == ERR_OK)
			    {
				    xmgr_findsat_TuneCheckTp (pstContents);
			    }
		    }
		}
		break;

	default:
		break;
	}

	if (pstContents->eState == eSatFindState_Finish)
	{
		hErr = xmgr_findsat_DetermineSatellites (pstContents);
		xmgr_findsat_ReleaseAction (pstContents);

#if defined(SUPPORT_TIME_RECOVERY)
		pstContents->bActionFound = ((ERR_OK != hErr) ? FALSE : TRUE);
		MGR_TIME_ClockRecoverStart();
#else
		// 일반 Search Action 과 달리 끝나자 마자 결과를 저장해야 한다.
		if(ERR_OK != hErr)
		{
			xmgr_findsat_SendSearchFinishedMsg (pstContents->hAction, FALSE);
		}
		else
		{
			xmgr_findsat_SendSearchFinishedMsg (pstContents->hAction, TRUE);
		}
#endif
	}

	return MESSAGE_BREAK;
}

// eSEVT_CH_NO_SIGNAL:
static HERROR xmgr_findsat_EvtChNoSignal (FindSat_Contents_t *pstContents, Handle_t hAction)
{
	HERROR		 hErr;

	(void)hAction;

	switch (pstContents->eState)
	{
	case eSatFindState_Tuning:
		// Tuning 실패 : 다음 Check List로
		hErr = xmgr_findsat_GotoNextCheckList (pstContents);
		if (hErr == ERR_OK)
		{
			xmgr_findsat_TuneCheckTp (pstContents);
		}
		break;

	default:
		break;
	}

	if (pstContents->eState == eSatFindState_Finish)
	{
		hErr = xmgr_findsat_DetermineSatellites (pstContents);
		xmgr_findsat_ReleaseAction (pstContents);

#if defined(SUPPORT_TIME_RECOVERY)
		pstContents->bActionFound = ((ERR_OK != hErr) ? FALSE : TRUE);
		MGR_TIME_ClockRecoverStart();
#else
		// 일반 Search Action 과 달리 끝나자 마자 결과를 저장해야 한다.
		if(ERR_OK != hErr)
		{
			xmgr_findsat_SendSearchFinishedMsg (pstContents->hAction, FALSE);
		}
		else
		{
			xmgr_findsat_SendSearchFinishedMsg (pstContents->hAction, TRUE);
		}
#endif
	}

	xmgr_findsat_ChangeTpText (pstContents);
	xmgr_findsat_ChangeProgressNumber (pstContents);
	xmgr_findsat_SendUpdateDisplayMsg(pstContents->hAction);

	return MESSAGE_BREAK;
}

// eSEVT_SI_SEARCH_DONE:
static HERROR xmgr_findsat_EvtSiSearchDone (FindSat_Contents_t *pstContents, Handle_t hAction)
{
	SatCheckList_t	 stSatItem;
	HERROR			 hErr;
	HxLOG_Trace();

	(void)hAction;

	switch (pstContents->eState)
	{
	case eSatFindState_WaitingSi:
		// 받은 SI 정보가 해당되는 정보와 동일한가?
		hErr = xmgr_findsat_CheckSiResults (pstContents);
		if (hErr == ERR_OK)
		{
			// 해당 시 성공사례로 등록
			hErr = xmgr_findsat_GetCurrentSatCheckItem (pstContents, &stSatItem);
			if (hErr == ERR_OK)
			{
				xmgr_findsat_SetCheckResult (pstContents, stSatItem.eSatType);
			}

			// SI를 Stop 시킨다.
			SVC_SI_StopSearch(pstContents->hAction, TRUE);

			// 다음 DiSEqC 를 체크해야 함.
			xmgr_findsat_InitCheckListPosition (pstContents);
			hErr = xmgr_findsat_SetNextDiseqc (pstContents);
			if (hErr == ERR_OK)
			{
				xmgr_findsat_TuneCheckTp (pstContents);
			}
			else
			{
				// 다음 DiSEqC이 없다 : 마지막까지 다 함.
				pstContents->eState = eSatFindState_Finish;
			}
		}
		else
		{
			// SI를 Stop 시킨다.
			SVC_SI_StopSearch(pstContents->hAction, TRUE);

			hErr = xmgr_findsat_GotoNextCheckList (pstContents);
			if (hErr == ERR_OK)
			{
				xmgr_findsat_TuneCheckTp (pstContents);
			}
		}
		break;

	default:
		break;
	}

	if (pstContents->eState == eSatFindState_Finish)
	{
		hErr = xmgr_findsat_DetermineSatellites (pstContents);
		xmgr_findsat_ReleaseAction (pstContents);

#if defined(SUPPORT_TIME_RECOVERY)
		pstContents->bActionFound = ((ERR_OK != hErr) ? FALSE : TRUE);
		MGR_TIME_ClockRecoverStart();
#else
		// 일반 Search Action 과 달리 끝나자 마자 결과를 저장해야 한다.
		if(ERR_OK != hErr)
		{
			xmgr_findsat_SendSearchFinishedMsg (pstContents->hAction, FALSE);
		}
		else
		{
			xmgr_findsat_SendSearchFinishedMsg (pstContents->hAction, TRUE);
		}
#endif
	}

	xmgr_findsat_ChangeTpText (pstContents);
	xmgr_findsat_ChangeProgressNumber (pstContents);
	xmgr_findsat_SendUpdateDisplayMsg(pstContents->hAction);

	return MESSAGE_BREAK;
}

//eSEVT_SI_SEARCH_FAIL:
static HERROR xmgr_findsat_EvtSiSearchFail (FindSat_Contents_t *pstContents, Handle_t hAction)
{
	HERROR		 hErr;

	(void)hAction;

	switch (pstContents->eState)
	{
	case eSatFindState_WaitingSi:
		// SI를 Stop 시킨다.
		SVC_SI_StopSearch(pstContents->hAction, TRUE);

		// SI 실패 : 다음 Check List로
		hErr = xmgr_findsat_GotoNextCheckList (pstContents);
		if (hErr == ERR_OK)
		{
			xmgr_findsat_TuneCheckTp (pstContents);
		}

		break;

	default:
		break;
	}

	if (pstContents->eState == eSatFindState_Finish)
	{
		hErr = xmgr_findsat_DetermineSatellites (pstContents);
		xmgr_findsat_ReleaseAction (pstContents);

#if defined(SUPPORT_TIME_RECOVERY)
		pstContents->bActionFound = ((ERR_OK != hErr) ? FALSE : TRUE);
		MGR_TIME_ClockRecoverStart();
#else
		// 일반 Search Action 과 달리 끝나자 마자 결과를 저장해야 한다.
		if(ERR_OK != hErr)
		{
			xmgr_findsat_SendSearchFinishedMsg (pstContents->hAction, FALSE);
		}
		else
		{
			xmgr_findsat_SendSearchFinishedMsg (pstContents->hAction, TRUE);
		}
#endif
	}

	xmgr_findsat_ChangeTpText (pstContents);
	xmgr_findsat_ChangeProgressNumber (pstContents);
	xmgr_findsat_SendUpdateDisplayMsg(pstContents->hAction);

	return MESSAGE_BREAK;
}


#define	_____EXTERNAL_PROCESS_FUNCTION_____

// =============================================================================================
//
//   Antenna에 설정된 Satellite 를 알아내는 Action
//
// =============================================================================================

BUS_Result_t xproc_search_SatFind (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	FindSat_Contents_t	*pstContents;
	HERROR					 hErr;

	pstContents = xmgr_findsat_GetContents();
	if (pstContents == NULL)			{ return MESSAGE_BREAK; }

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		// Argument :
		// p1 : Antenna ID
		HxLOG_Info("\tAP_FindSat_Proc : [eMEVT_BUS_CREATE]\n\n");
		hErr = xmgr_findsat_MsgGwmCreate (pstContents, p1, p2);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_BUS_DESTROY:
		HxLOG_Info("\tAP_FindSat_Proc : [eMEVT_BUS_DESTROY]\n\n");
		hErr = xmgr_findsat_MsgGwmDestroyed (pstContents);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_SEARCH_ACTION_START:
		HxLOG_Info("\tAP_FindSat_Proc : [eMEVT_SEARCH_ACTION_START]\n\n");
		hErr = xmgr_findsat_MsgAppSearchActionStart (pstContents, p1);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_SEARCH_SAVE_RESULT:
		HxLOG_Info("\tAP_FindSat_Proc : [eMEVT_SEARCH_SAVE_RESULT]\n\n");
		hErr = xmgr_findsat_MsgAppSearchSaveResult (pstContents, p1);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_SEARCH_CANCEL:
		HxLOG_Info("\tAP_FindSat_Proc : [eMEVT_SEARCH_CANCEL]\n\n");
		hErr = xmgr_findsat_MsgAppSearchCancel (pstContents, p1);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_BUS_TIMER:
		HxLOG_Info("\tAP_FindSat_Proc : [eMEVT_BUS_TIMER]\n\n");
		hErr = xmgr_findsat_MsgGwmTimer (pstContents, p1);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eSEVT_CH_LOCKED:
		HxLOG_Print("\tAP_FindSat_Proc : [eSEVT_CH_LOCKED]\n\n");

		hErr = xmgr_findsat_CheckActionId (pstContents, hAction);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }

		hErr = xmgr_findsat_EvtChLocked (pstContents, hAction);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eSEVT_CH_NO_SIGNAL:
		HxLOG_Print("\tAP_FindSat_Proc : [eSEVT_CH_NO_SIGNAL]\n\n");

		hErr = xmgr_findsat_CheckActionId (pstContents, hAction);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }

		hErr = xmgr_findsat_EvtChNoSignal (pstContents, hAction);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eSEVT_SI_SEARCH_DONE:
		HxLOG_Print("\tAP_FindSat_Proc : [eSEVT_SI_SEARCH_DONE]\n\n");

		hErr = xmgr_findsat_CheckActionId (pstContents, hAction);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }

		hErr = xmgr_findsat_EvtSiSearchDone (pstContents, hAction);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eSEVT_SI_SEARCH_FAIL:
		HxLOG_Print("\tAP_FindSat_Proc : [eSEVT_SI_SEARCH_FAIL]\n\n");

		hErr = xmgr_findsat_CheckActionId (pstContents, hAction);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }

		hErr = xmgr_findsat_EvtSiSearchFail (pstContents, hAction);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

#if defined(SUPPORT_TIME_RECOVERY)
	case eMEVT_TIME_CLOCKRECOVER_SETUP_DONE:
		// 일반 Search Action 과 달리 끝나자 마자 결과를 저장해야 한다.
		xmgr_findsat_SendSearchFinishedMsg (pstContents->hAction, pstContents->bActionFound);
		break;
#endif

	default:
		break;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


/* End of File. ---------------------------------------------------------- */


