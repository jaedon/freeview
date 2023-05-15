/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
/** @brief global header file  */
#include <octo_common.h>

#include <util.h>
#include <treeutil.h>
#include <util_task.h>

#include <db_svc.h>
#include <db_param.h>

#include <pal_sef.h>
#include <pal_pipe.h>
#include <pal_dsmcc.h>
#include <rlib.h>
#include <dapi.h>
#include <pal_pb.h>

#include <svc_si.h>
#include <linkedlist.h>

#include <svc_epg.h>
#include <isosvc.h>

#include "_svc_epg_common.h"
#include "_svc_epg_common_lib.h"
#include "_svc_epg_si_lib.h"
#include "_svc_epg_raw_mgr.h"
#include "_svc_epg_context_mgr.h"
#include "_svc_epg_eit_lib.h"

#include "../../local_include/_xsvc_epg.h"

#include "_xsvc_epg_tvtv_lib.h"

#include "_svc_epg_db_raw_lib.h"

#include "_xsvc_epg_tvtv_db_lib.h"
#include "_xsvc_epg_tvtv_data.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

//								 TODO : 아래 define 정리
//============================================================================
#define CONFIG_TVTVEPG_LIVEDOWNLOAD

#define CONFIG_TVTV_CONTROL
#define CONFIG_TVTVLOG

#define TVTV_COMPONENT_TAG			0x01

#define SECTION_NUM_MAX				256

#define TVTV_DSI_TABLE_VERSION_1_0		0x00010000
#define TVTV_TABLEFILE_VERSION_1_0		0x00010000
#define TVTV_DSI_COMPLETE_MASK			1


#define TVTV_DSI_TABLE_CURRENT_VERSION	TVTV_DSI_TABLE_VERSION_1_0
#define TVTV_TABLEFILE_CURRENT_VERSION	TVTV_TABLEFILE_VERSION_1_0


#if defined(CONFIG_FUNC_EPG_USE_SVCUID)
#define TVTV_USE_SVCUID					// 각 channel 고유의 svcuid를 이용
#else
#define TVTV_USE_TRIPLEID					// tripleId를 이용
#endif
#define TVTV_EPGTBL_NOT_SAVE_IN_HDD		// xxx.tbl을 hdd에 저장하지 않는다. 바로 db에 저장됨으로 저장하지 않아도 된다.

#define SVC_TRIPLEID_COMPARE			// 현재 search된 channel list와 상관없이 모든 channel에 대한 tvtv epg data를 수신하도록 한다.

#define TVTV_ADD_USR_TP			// 특정 TP만 TVTV롤 add한다.

#ifdef CONFIG_DEBUG
#define TEMP_DBG_RPINT
#endif

#define TVTV_NEW_ONE					// new 구조로 dama에 data를 전달한다..확인 완료시 define 제거

#define TVTV_MAXNUM_OF_TRIPLE_CHANNELIST	5000

//#define TVTV_SUPPORT_DOWNLOAD_INTERVAL_SIZE			// tvtv download시 99%에서 넘어가지 않는 문제 발생함
#if defined(TVTV_SUPPORT_DOWNLOAD_INTERVAL_SIZE)
#define TVTV_INTERVAL_SIZE 	80000
#endif

//============================================================================



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#if defined(TEMP_DBG_RPINT)
typedef enum
{
	eTVTVMAP_PARSE = 0,
	eTVTVMAP_NOT_FOUND,
	eTVTVMAP_SET,
	eTVTVMAP_SENDTODAMA,
	eTVTVMAP_VALIDEVENT,
	eTVTVMAP_VALIDEVENT2,
	eTVTVMAP_SENDEVENT,
	eTVTVMAP_MAX
}Print_TvtvMap_t;
#endif

typedef enum
{
	EPG_CNXTTVTV_EMPTY					= 0,
	EPG_CNXTTVTV_NOTUSED,
	EPG_CNXTTVTV_ONCE,
	EPG_CNXTTVTV_CONTINUE,
	EPG_CNXTTVTV_MONITOR,
	EPG_CNXTTVTV_COMPLETE

} xsvcEpg_ContextTvtvState_e;

typedef enum
{
	EPGCNXT_FITER_SLOT_TVTV_PAT				= 0,
	EPGCNXT_FITER_SLOT_TVTV_PMT,
	EPGCNXT_FITER_SLOT_TVTV_DSI,
	EPGCNXT_FITER_SLOT_TVTV_DDB,

	EPGCNXT_FITER_SLOT_TVTV_MAX
} xsvcEpg_ContextTvtvFilterSlot_e;


typedef enum
{
	TvtvProcess_Nothing							= 0,
	TvtvProcess_DownLoading,
	TvtvProcess_DataLoading,
	TvtvProcess_DataUnloading,

	TvtvProcess_Max
} xsvcEpg_TvtvProcessState_e;

// 32 byte
typedef struct tagTvtvDsiFileTable
{
	HUINT32							ulTableVersion;
	HUINT32							ulDsiTransactioinId;
	HUINT32							ulDsiCrc;
	HUINT32							ulRegisteredTime;
	HUINT32							ulComplete;
	HUINT32							aulReserved[3];
} xsvcEpg_TvtvDsiTable_t;


typedef struct tagTvtvProcessState
{
	HUINT32							ulSemId;
	xsvcEpg_TvtvProcessState_e		eCurrentProcess;
	Handle_t						hAction;
	HUINT8							ucHead;
} xsvcEpg_TvtvProcess_t;

#if defined(TVTV_USE_SVCUID)
typedef struct
{
	DbSvc_TripleId_t		stTripleId;
	HINT32				ulUid;
	HUINT32				ulTvtvId;
	HUINT32				ulLcn;
	HUINT8				ucName[DxNAME_LEN];

} xsvcEpg_TvtvSvcInfo_t;

typedef struct
{
	HUINT16	channel_cnt;
	xsvcEpg_TvtvSvcInfo_t	stTvtvSvcInfo_Temp[TVTV_MAXNUM_OF_TRIPLE_CHANNELIST]; // TODO : total channel을 read해서 malloc으로 change해야함.
} xsvcEpg_TvtvSvcInfoList_t;

STATIC xsvcEpg_TvtvSvcInfoList_t stTvtvSvcInfoList_Temp;
#endif

#if defined(TVTV_USE_TRIPLEID)
#define MAX_TVTV_MULTI_TS_NUM		(10)			// tvtv가 같더라도 서로다른 tripleId가 존재함.
#define MAX_TVTV_TS_NUM				(2000)		// tvtv가 존재한는 total ts number 현재 사내 스트림에서는 203개의 ts(in tsmt)가 존재함.
typedef struct
{
	HUINT32				ulTvtvId;
	HINT32				ulSvcUid;
	DbSvc_TripleId_t		stTripleId;
} xsvcEpg_TvtvSvcInfo_t;

typedef struct
{
	HUINT16	usTsCnt;
	HUINT8   ucMultiTsIdx[MAX_TVTV_MULTI_TS_NUM];
	xsvcEpg_TvtvSvcInfo_t	stTvtvSvcInfo[MAX_TVTV_TS_NUM];
} xsvcEpg_TvtvSvcInfoList_t;

STATIC xsvcEpg_TvtvSvcInfoList_t stTvtvSvcInfoList;
#endif

typedef struct
{
	HUINT32 		ulOrgDeliType;
	HUINT8			ucAntId;
} xsvcEpg_AstraTvTv_Info_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC xsvcEpg_AstraTvTv_Info_t 	stAstraTvtv_Info;
STATIC xsvcEpg_TvtvProcess_t		s_stTvtvProcess = {0, TvtvProcess_Nothing};

#if defined(TVTV_ADD_USR_TP)
#define TVTV_TRIPLE_ID_NUM		130//135 230 133 100

STATIC DbSvc_TripleId_t				s_stTvtv_TripleId[TVTV_TRIPLE_ID_NUM] =			// Default TPs :
{
	{0x0001, 0x03FB, 0x283D}, // [1] Das Erste HD
	{0x0001, 0x03F3, 0x2B66}, // [2] ZDF HD
	{0x0001, 0x0421, 0xEF10}, // [3] RTL HD
	{0x0001, 0x03F9, 0xEF74}, // [4] SAT.1 HD
	{0x0001, 0x0421, 0xEF11}, // [5] VOX HD
	{0x0001, 0x03F9, 0xEF75}, // [6] ProSieben HD
	{0x0001, 0x03F9, 0xEF76}, // [7] kabel eins HD
	{0x0001, 0x03FB, 0x283E}, // [8] arte HD
	{0x0001, 0x0421, 0xEF15}, // [9] RTLII HD
	{0x0001, 0x0411, 0x2E9B}, // [10] SUPER RTL HD
	{0x0001, 0x03F2, 0x2B8E}, // [11] 3sat HD
	{0x0001, 0x04B1, 0x6EA5}, // [12] WDR Koln HD
	{0x0001, 0x0401, 0x2855}, // [13] BR Sud HD
	{0x0001, 0x03FB, 0x283F}, // [14] SWR BW HD
	{0x0001, 0x0401, 0x2859}, // [15] NDR FS HH HD
	{0x0001, 0x0425, 0x2873}, // [16] hr-fernsehen HD
	{0x0001, 0x0425, 0x2871}, // [17] MDR S-Anhalt HD
	{0x0001, 0x0425, 0x286F}, // [18] rbb Berlin HD
	{0x0001, 0x04B1, 0x6EE1}, // [19] Radio Bremen TV
	{0x0001, 0x0445, 0x6F46}, // [20] SR Fernsehen
	{0x0001, 0x040F, 0x2888}, // [21] EinsFestival HD
	{0x0001, 0x040F, 0x2889}, // [22] EinsPlus HD
	{0x0001, 0x040F, 0x2887}, // [23] tagesschau24 HD
	{0x0001, 0x03F3, 0x2B7A}, // [24] zdf_neo HD
	{0x0001, 0x03F3, 0x2B84}, // [25] zdf.kultur HD
	{0x0001, 0x03F2, 0x2BA2}, // [26] ZDFinfo HD
	{0x0001, 0x0401, 0x285B}, // [27] PHOENIX HD
	{0x0001, 0x03EF, 0x1332}, // [28] ServusTV HD Deutschland
	{0x0001, 0x03F2, 0x2B98}, // [29] KIKA HD
	{0x0001, 0x041D, 0x5273}, // [30] NICK/CC HD
	{0x0001, 0x041F, 0x157C}, // [31] Disney Channel HD
	{0x0001, 0x041D, 0x526C}, // [32] ANIXE HD
	{0x0085, 0x0005, 0x0701}, // [33] DAS VIERTE
	{0x0001, 0x0455, 0x1519}, // [34] TELE 5 HD
	{0x0001, 0x03F9, 0xEF77}, // [35] SIXX HD
	{0x0001, 0x0455, 0x151A}, // [36] DMAX HD
	{0x0001, 0x0409, 0x2774}, // [37] TLC HD
	{0x0001, 0x0453, 0x4460}, // [38] SAT.1 Gold
	{0x0001, 0x03F9, 0xEF78}, // [39] ProSieben MAXX HD
	{0x0001, 0x0411, 0x2EAF}, // [40] RTLNITRO HD
	{0x0001, 0x045B, 0x332F}, // [41] Welt der Wunder
	{0x0001, 0x041F, 0x1581}, // [42] SPORT1 HD
	{0x0001, 0x0443, 0x79E0}, // [43] Eurosport Deutschland
	{0x0001, 0x041F, 0x157F}, // [44] Deluxe Music HD
	{0x0001, 0x0436, 0x7004}, // [45] VIVA Germany
	{0x0001, 0x041D, 0x5278}, // [46] iM1
	{0x0001, 0x045B, 0x332E}, // [47] GoTV
	{0x0085, 0x0005, 0x004A}, // [48] joiz
	{0x0001, 0x03EB, 0x33AD}, // [49] MELODIE TV
	{0x0001, 0x045D, 0x32DA}, // [50] Folx TV
	{0x0001, 0x0459, 0x313C}, // [51] Deutsches Musik Fernsehen
	{0x0001, 0x0421, 0xEF14}, // [61] n-tv HD
	{0x0001, 0x041D, 0x5274}, // [62] N24 HD
	{0x0001, 0x0402, 0x2742}, // [63] BBC World
	{0x0001, 0x042C, 0x6F6A}, // [64] CNN Int.
	{0x0001, 0x0455, 0x1523}, // [69] TecTime TV
	{0x0001, 0x0441, 0x2EE3}, // [3] RTL Television
	{0x0001, 0x0453, 0x445C}, // [4] SAT.1
	{0x0001, 0x0441, 0x2F1C}, // [5] VOX
	{0x0001, 0x0453, 0x445D}, // [6] ProSieben
	{0x0001, 0x0453, 0x445E}, // [7] kabel eins
	{0x0001, 0x0441, 0x2EF4}, // [9] RTL2
	{0x0001, 0x0441, 0x2F08}, // [10] Super RTL
	{0x0001, 0x0005, 0x0308}, // [27] SIXX
	{0x0001, 0x041B, 0x7033}, // [62] EinsPlus
	{0x0085, 0x0007, 0x003D}, // [79] NICK/CC AUT
	{0x0085, 0x0007, 0x003C}, // [80] VIVA Austria
	{0x0001, 0x0443, 0x7080}, // [81] RTL Austria
	{0x0001, 0x0443, 0x7085}, // [82] VOX Austria
	{0x0001, 0x0443, 0x708A}, // [83] RTL2 Austria
	{0x0001, 0x0443, 0x708F}, // [84] Super RTL A
	{0x0001, 0x043A, 0x4E22}, // [86] ProSieben Austria
	{0x0001, 0x043A, 0x4E24}, // [88] Kabel 1 Austria
	{0x0001, 0x0441, 0x2F1D},	// [305] RTLNITRO
	{0x0001, 0x043A, 0x4E27},   	// [405] PULS 4 Austria
	{0x0085, 0x0002, 0x000B},	// [501] Sky Cinema +1
	{0x0085, 0x0002, 0x002B},	// [502] Sky Cinema +24
	{0x0085, 0x0002, 0x0008},	// [504] Sky Comedy
	{0x0085, 0x0002, 0x0014},	// [505] Sky Emotion
	{0x0085, 0x0003, 0x0204},	// [506] Sky Nostalgie
	{0x0085, 0x0004, 0x0034},	// [583] Spiegel Geschichte
	{0x0085, 0x0001, 0x00A8},   	// [585] Motorvision TV
	{0x0085, 0x0001, 0x0017},	// [586] Sky Krimi
	{0x0085, 0x0001, 0x001B},	// [588] RTL Crime
	{0x0085, 0x0011, 0x001C},	// [598] Disney XD
	{0x0085, 0x0006, 0x0083},	// [601] Sky Cinema HD
	{0x0085, 0x0006, 0x0082},	// [602] Discovery HD
	{0x0085, 0x0006, 0x0084},	// [603]]Eurosport HD
	{0x0085, 0x0006, 0x0081},	// [604] Sky Sport HD 1
	{0x0085, 0x000D, 0x0072},	// [605] Sky Sport HD 2
	{0x0085, 0x000D, 0x006F},	// [606] Disney Cinemagic HD
	{0x0085, 0x000D, 0x0071},	// [608] History HD
	{0x0085, 0x000C, 0x006A},	// [610] Sky Action HD
	{0x0085, 0x000C, 0x006C},	// [613] Sky Sport News HD
	{0x0085, 0x000B, 0x0075},	// [614] Sky 3D
	{0x0085, 0x000B, 0x0076},	// [615] Nat Geo Wild HD
	{0x0001, 0x000B, 0x0074},	// [617] Disney Channel HD
	{0x0001, 0x0453, 0x4461},	// [660] SAT.1 emotions
	{0x0001, 0x0453, 0x4462},	// [661] kabel eins classics
	{0x0001, 0x0441, 0x2EFE},	// [662] RTL Living
	{0x0001, 0x03EF, 0x132F},	// [916] ORF1 HD
	{0x0001, 0x03EF, 0x1330},	// [917] ORF2 HD
	{0x0085, 0x0021, 0x003F},	// [1220] DMAX
	{0x0085, 0x000D, 0x006E}, //[Sky Atlantic HD] Logo[http://az341951.vo.msecnd.net/tvtvchannellogo/4978.png]
	{0x0085, 0x000C, 0x006B}, //[Sky Hits HD] Logo[http://az341951.vo.msecnd.net/tvtvchannellogo/4638.png]
	{0x0085, 0x000A, 0x007C}, //[Fox HD] Logo[http://az341951.vo.msecnd.net/tvtvchannellogo/4278.png]
	{0x0085, 0x000B, 0x007B}, //[TNT Serie HD] Logo[http://az341951.vo.msecnd.net/tvtvchannellogo/4276.png]
	{0x0085, 0x000C, 0x007E}, //[Syfy HD] Logo[http://az341951.vo.msecnd.net/tvtvchannellogo/4279.png]
	{0x0085, 0x000D, 0x007F}, //[13th Street HD] Logo[http://az341951.vo.msecnd.net/tvtvchannellogo/4670.png]
	{0x0085, 0x000A, 0x007D}, //[AXN HD] Logo[http://az341951.vo.msecnd.net/tvtvchannellogo/4631.png]
	{0x0085, 0x000E, 0x0065}, //[Universal HD] Logo[http://az341951.vo.msecnd.net/tvtvchannellogo/9164.png]
	{0x0085, 0x000E, 0x0080}, //[E! Entertainm. HD] Logo[http://az341951.vo.msecnd.net/tvtvchannellogo/4731.png]
	{0x0085, 0x0001, 0x001D}, //[RTL Passion] Logo[http://az341951.vo.msecnd.net/tvtvchannellogo/720.png]
	{0x0085, 0x0011, 0x0015}, //[Beate-Uhse.TV] Logo[http://az341951.vo.msecnd.net/tvtvchannellogo/944.png]
	{0x0085, 0x0011, 0x0016}, //[Heimatkanal] Logo[http://az341951.vo.msecnd.net/tvtvchannellogo/916.png]
	{0x0085, 0x0011, 0x0018}, //[Classica] Logo[http://az341951.vo.msecnd.net/tvtvchannellogo/942.png]
	{0x0085, 0x0011, 0x001A}, //[Disney Junior] Logo[http://az341951.vo.msecnd.net/tvtvchannellogo/853.png]
	{0x0085, 0x0011, 0x0013}, //[Junior] Logo[http://az341951.vo.msecnd.net/tvtvchannellogo/930.png]
	{0x0085, 0x0004, 0x000D}, //[National Geographic] Logo[http://az341951.vo.msecnd.net/tvtvchannellogo/845.png]
	{0x0085, 0x0001, 0x001E}, //[Sky Sport Austria] Logo[http://az341951.vo.msecnd.net/tvtvchannellogo/4105.png]
	{0x0085, 0x000F, 0x0026}, //[Romance TV] Logo[http://az341951.vo.msecnd.net/tvtvchannellogo/3805.png]
	{0x0035, 0x0451, 0x0FDA}, //[Animax] Logo[http://az341951.vo.msecnd.net/tvtvchannellogo/3798.png]
	{0x0001, 0x03F7, 0x125D}, //[SAT.1 emotions] Logo[http://az341951.vo.msecnd.net/tvtvchannellogo/702.png]
	{0x0085, 0x000F, 0x0023}, //[TNT Film] Logo[http://az341951.vo.msecnd.net/tvtvchannellogo/717.png]
	{0x0085, 0x000F, 0x0021}, //[Kinowelt TV] Logo[http://az341951.vo.msecnd.net/tvtvchannellogo/844.png]
	{0x0085, 0x000F, 0x0039}, //[Biography Channel] Logo[http://az341951.vo.msecnd.net/tvtvchannellogo/721.png]
	{0x0085, 0x000F, 0x0038}, //[Boomerang] Logo[http://az341951.vo.msecnd.net/tvtvchannellogo/1511.png]
	{0x0085, 0x000F, 0x0027}, //[Cartoon Network (S)] Logo[http://az341951.vo.msecnd.net/tvtvchannellogo/902.png]
	{0x0085, 0x000E, 0x0078}, //[Sky Select HD] Logo[http://az341951.vo.msecnd.net/tvtvchannellogo/1544.png]
	{0x0001, 0x03EF, 0x1331} //[ServusTV HD Oesterreich] Logo[http://az341951.vo.msecnd.net/tvtvchannellogo/9044.png]
};
#endif

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

STATIC HERROR				xsvc_epgcontext_ProcessPAT_Tvtv(svcEpg_ContextData_t *pCnxtData);
STATIC HERROR				xsvc_epgcontext_ProcessPMT_Tvtv(svcEpg_ContextData_t *pCnxtData);
STATIC HERROR				xsvc_epgcontext_InitDownloadInfo_Tvtv(svcEpg_ContextData_t *pCnxtData);
STATIC HERROR				xsvc_epgcontext_StopAllFilter_Tvtv(svcEpg_ContextData_t *pCnxtData);
STATIC HERROR				xsvc_epgcontext_StopFilter_Tvtv(svcEpg_Filter_t *pstFilterInfo);
STATIC HERROR				xsvc_epgcontext_StartFilterForPmtSignaling_Tvtv(svcEpg_ContextData_t *pCnxtData);
STATIC HERROR				xsvc_epgcontext_StartPmtFilter_Tvtv(svcEpg_ContextData_t *pCnxtData);

STATIC HERROR				xsvc_epgcontext_GetDsmccPid_Tvtv(HUINT16 usTsUniqueId, HUINT16 usProgNum, HUINT16 usAssociationTag, HUINT16 *pusDsmccPid);
STATIC HERROR				xsvc_epgcontext_GetPmtPid_Tvtv(HUINT16 usTsUniqueId, HUINT16 usSvcId, HBOOL *pbFound, HUINT16 *pusPmtPid);

STATIC HERROR				xsvc_epgcontext_GetContextInfo_Tvtv(Handle_t hAction, HUINT32 ulIndex, Handle_t *phSvcHandle, HUINT32 *pulOrgDeliType, HUINT8 *pucAntId, HUINT32 *pulEpgTuneInfo, svcEpg_Filter_t *pEpgContextFilterInfo);
STATIC HERROR				xsvc_epgcontext_SetContextFilterInfo_Tvtv(Handle_t hAction, HUINT32 ulIndex, svcEpg_Filter_t *pEpgContextFilterInfo);
STATIC HERROR				xsvc_epgcontext_GetDownloadInfo_Tvtv(Handle_t hAction, svcEpg_DownloadInfo_t *pDownloadInfo);
STATIC HERROR				xsvc_epgcontext_SetDownloadInfo_Tvtv(Handle_t hAction, svcEpg_DownloadInfo_t *pDownloadInfo);
STATIC HERROR				xsvc_epgcontext_GetPostprocessInfo_Tvtv(Handle_t hAction, svcEpg_PostProcessInfo_t *pPostprocessInfo);
STATIC HERROR				xsvc_epgcontext_SetPostprocessInfo_Tvtv(Handle_t hAction, svcEpg_PostProcessInfo_t *pPostprocessInfo);
STATIC HERROR				xsvc_epgcontext_GetXMLParseInfo_Tvtv(Handle_t hAction, svcEpg_XmlParseInfo_t *pXMLParseInfo);


STATIC void					xsvc_epgcontext_PostPAT_Tvtv(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction);
STATIC void					xsvc_epgcontext_FreePAT_Tvtv(HUINT32 ulTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2);
STATIC void					xsvc_epgcontext_PostPMT_Tvtv(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction);
STATIC void					xsvc_epgcontext_FreePMT_Tvtv(HUINT32 ulTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2);
STATIC void					xsvc_epgcontext_PostDsi_Tvtv(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction);
STATIC void					xsvc_epgcontext_PostDdb_Tvtv(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction);
STATIC HERROR				xsvc_epgcontext_ProcessDsi_Tvtv(HUINT32 ulParam1, HUINT32 ulParam2);
STATIC HERROR				xsvc_epgcontext_ProcessDdb_Tvtv(HUINT32 ulParam1, HUINT32 ulParam2);



STATIC HERROR				xsvc_epgcontext_RegisterEvent_Tvtv(HTREEMGR hTree, Handle_t hSvcHandle, EPG_SpecType_e eSpec, HUINT32 ulOrgDeliType, HUINT8 ucAntId);
STATIC HERROR				xsvc_epgcontext_InsertTvtvEventListByTvtvMap_Tvtv(HTREEMGR hTree, HUINT32 ulOrgDeliType, HUINT8 ucAntId, POINTER_LIST_T *pTvtvMapList);
STATIC HERROR				xsvc_epgcontext_InsertTvtvEventListByEvent_Tvtv(HTREEMGR hTree, HUINT32 ulTableId, HUINT16 usFileIndex, POINTER_LIST_T *pEventList, POINTER_LIST_T *pExtList);
STATIC HERROR				xsvc_epgcontext_FindTvtvItem_Tvtv(HTREEMGR hTree, HUINT32 usTvtvId, HTREEITEM *phItem);
STATIC void					xsvc_epgcontext_FreeTvtvEventList_Tvtv(void *pvContents);
STATIC void					xsvc_epgcontext_FreeTvtvMapList_Tvtv(void *pvContents);
STATIC void					xsvc_epgcontext_FreeFileTableList_Tvtv(void *pvContents);
STATIC HINT8				xsvc_epgcontext_RankFileByFileNumber_Tvtv(void *pvItem1, void *pvItem2);
STATIC HERROR				xsvc_epgcontext_AddDownloadedFile_Tvtv(HBOOL bFirst, HUINT8 ucIsComplete, PalDsmcc_File_t *pDsmccFile, svcEpg_DownloadInfo_t *pDownloadInfo, PalDsmcc_File_t **ppDsmccFileList);
STATIC HERROR				xsvc_epgcontext_FreeDownloadedFile_Tvtv(void);
STATIC HUINT32				xsvc_epgcontext_GetFileCount_Tvtv(PalDsmcc_File_t *pRoot);
STATIC PalDsmcc_File_t*		xsvc_epgcontext_AppendFile_Tvtv(PalDsmcc_File_t *pRoot, PalDsmcc_File_t *pFile);
STATIC HERROR				xsvc_epgcontext_Postprosess_Tvtv(HUINT32 ulParam1, HUINT32 ulParam2);
STATIC HERROR				xsvc_epgcontext_PostprosessBuffer_Tvtv(HUINT32 ulParam1, HUINT32 ulParam2);
STATIC HERROR				xsvc_epgcontext_PostprosessFile_Tvtv(HUINT32 ulParam1, HUINT32 ulParam2);
#if defined(CONFIG_TVTVEPG_LIVEDOWNLOAD)
STATIC HERROR				xsvc_epgcontext_SetXMLParseInfo_Tvtv(Handle_t hAction, svcEpg_XmlParseInfo_t *pXMLParseInfo);
STATIC HERROR				xsvc_epgcontext_ProcessXMLParse_Tvtv(HUINT32 ulParam1, HUINT32 ulParam2);
STATIC HERROR				xsvc_epgcontext_ProcessXMLParseFile_Tvtv(HUINT32 ulParam1, HUINT32 ulParam2);
STATIC HERROR				xsvc_epgcontext_ProcessXMLParseBuffer_Tvtv(HUINT32 ulParam1, HUINT32 ulParam2);
#endif

#if	defined(CONFIG_TVTV_CONTROL)
STATIC HERROR				xsvc_epgcontext_InitControlProcess_Tvtv(void);
STATIC HERROR				xsvc_epgcontext_GetControlProcess_Tvtv(xsvcEpg_TvtvProcessState_e *pCurrentState);
STATIC HERROR				xsvc_epgcontext_ControlAction_Tvtv(HUINT32 ulCommand, HBOOL bProgressing, HBOOL *pbPass);
#endif

#if defined(TVTV_ADD_USR_TP)
STATIC HERROR				 xsvc_epgcontext_IsExistTVTVEPG(HUINT16 usTsId, HUINT16 usSvcId);
#endif

STATIC HUINT32			xsvc_epgcontext_GetSystemTime_Tvtv(void)
{
	HULONG		ulSysTime = 0;

	if(VK_CLOCK_GetTime((HULONG*)&ulSysTime) != VK_OK)
	{
		HxLOG_Error(": Time was not set \n");
	}
	return (HUINT32)ulSysTime;
}


#define	______TEMP_FILEIO_START______
#include <stdio.h>

#define	TVTV_FILEIO_READONLY			"rb"
#define	TVTV_FILEIO_WRITEONLY			"wb"


STATIC HERROR		xsvc_epgcontext_Fopen_Tvtv(HUINT8 *pszFileName, Handle_t *phFileHandle);
STATIC HERROR		xsvc_epgcontext_Fread_Tvtv(void *pBuffer, HUINT32 ulUnitSize, Handle_t hFileHandle, PalPb_SeekPosition_e eSeekMode, HUINT32 *pulReadSize);
STATIC HERROR		xsvc_epgcontext_CurrentposFread_Tvtv(void *pBuffer, HUINT32 ulUnitSize, Handle_t hFileHandle, HUINT32 *pulReadSize);
STATIC HERROR		xsvc_epgcontext_Fwrite_Tvtv(void *pBuffer, HUINT32 ulUnitSize, Handle_t hFileHandle, PalPb_SeekPosition_e eSeekMode, HUINT32 *pulWriteSize);
STATIC HERROR		xsvc_epgcontext_Fclose_Tvtv(Handle_t hFileHandle);
STATIC HERROR		xsvc_epgcontext_FileLength_Tvtv(HUINT8 *pucFileName, HUINT32 *pulFileLength);
STATIC HERROR		xsvc_epgcontext_ReadFullFileToBuffer_Tvtv(HUINT8 *pszFileName, HUINT8 **ppBuffer, HUINT32 *pulReadSize);
STATIC HERROR		xsvc_epgcontext_ReadFileToBuffer_Tvtv(Handle_t hFileHandle, HUINT8 **ppBuffer, HUINT32 ulReadSize, HUINT32 *pulReadSize);
STATIC HERROR		xsvc_epgcontext_InitDirectory_Tvtv(void);

STATIC HERROR		xsvc_epgcontext_SavePostprocessedFile_Tvtv(xsvcEpg_PostprocFile_t *pPostprocFile);



STATIC HERROR		xsvc_epgcontext_Fopen_Tvtv(HUINT8 *pszFileName, Handle_t *phFileHandle)
{
	HERROR			hError = ERR_FAIL;
	HxFILE_t	hFile = NULL;

	if(pszFileName == NULL || phFileHandle == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_Fopen_Tvtv] Param NULL!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	*phFileHandle = (Handle_t)NULL;

	if (HLIB_FILE_Exist(pszFileName) == TRUE)
	{
		hFile = HLIB_FILE_Open(pszFileName, "r+");
	}
	else
	{
		hFile = HLIB_FILE_Open(pszFileName, "w+");
	}
	if (hFile == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_Fopen_Tvtv] HLIB_FILE_Open fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}
	*phFileHandle =(Handle_t)hFile ;
	hError = ERR_OK;

EXIT_CNXTTVTV_FUNC :

	return hError;

}

STATIC HERROR		xsvc_epgcontext_Fread_Tvtv(void *pBuffer, HUINT32 ulUnitSize, Handle_t hFileHandle, PalPb_SeekPosition_e eSeekMode, HUINT32 *pulReadSize)
{
	HERROR			hError = ERR_FAIL;
	HUINT32			ulReadSize;
	HxFILE_t	hFile = NULL;
	HxFILE_Seek_e	eMode;
	HUINT64 	ullPosOffset;

	if(pBuffer == NULL || hFileHandle == (Handle_t)NULL || pulReadSize == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_Fread_Tvtv] Param NULL!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}
	*pulReadSize = 0;

	hFile = (HxFILE_t)hFileHandle;

	switch(eSeekMode)
	{
		case	ePAL_PB_SEEKPOSITION_START:
			eMode = eHxFILE_SEEK_SET;
			break;

		case	ePAL_PB_SEEKPOSITION_CURRENT:
			eMode = eHxFILE_SEEK_CUR;
			break;

		case	ePAL_PB_SEEKPOSITION_END:
			eMode = eHxFILE_SEEK_END;
			break;

		default:
			eMode = eHxFILE_SEEK_SET;
			break;
	}

	hError = HLIB_FILE_Seek(hFile, 0, eMode, &ullPosOffset);
	if (hError != ERR_OK )
	{
		HxLOG_Error("[xsvc_epgcontext_Fread_Tvtv] HLIB_FILE_Seek fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	ulReadSize = HLIB_FILE_Read(hFile, (void *)pBuffer, 1, ulUnitSize);
	if (ulReadSize <= 0)
	{
		HxLOG_Error("[xsvc_epgcontext_Fread_Tvtv] HLIB_FILE_Read fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	*pulReadSize = ulReadSize;

	hError = ERR_OK;

EXIT_CNXTTVTV_FUNC :

	return hError;

}

STATIC HERROR		xsvc_epgcontext_CurrentposFread_Tvtv(void *pBuffer, HUINT32 ulUnitSize, Handle_t hFileHandle, HUINT32 *pulReadSize)
{
	HERROR			hError = ERR_FAIL;
	HUINT32			ulReadSize;
	HxFILE_t			hFile = NULL;

	if(pBuffer == NULL || hFileHandle == (Handle_t)NULL || pulReadSize == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_CurrentposFread_Tvtv] Param NULL!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}
	*pulReadSize = 0;

	hFile = (HxFILE_t)hFileHandle;

	ulReadSize = HLIB_FILE_Read(hFile, (void *)pBuffer, 1, ulUnitSize);
	if (ulReadSize <= 0)
	{
		HxLOG_Error("[xsvc_epgcontext_Fread_Tvtv] HLIB_FILE_Read fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	*pulReadSize = ulReadSize;

	hError = ERR_OK;

EXIT_CNXTTVTV_FUNC :

	return hError;

}

STATIC HERROR		xsvc_epgcontext_Fwrite_Tvtv(void *pBuffer, HUINT32 ulUnitSize, Handle_t hFileHandle, PalPb_SeekPosition_e eSeekMode, HUINT32 *pulWriteSize)
{
	HERROR			hError = ERR_FAIL;
	HUINT32			ulWriteSize = 0;
	HxFILE_t	hFile = NULL;
	HxFILE_Seek_e	eMode;
	HUINT64 	ullPosOffset;

	if(pBuffer == NULL || hFileHandle == (Handle_t)NULL || pulWriteSize == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_Fwrite_Tvtv] Param NULL!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}
	*pulWriteSize = 0;

	hFile = (HxFILE_t)hFileHandle;

	switch(eSeekMode)
	{
		case	ePAL_PB_SEEKPOSITION_START:
			eMode = eHxFILE_SEEK_SET;
			break;

		case	ePAL_PB_SEEKPOSITION_CURRENT:
			eMode = eHxFILE_SEEK_CUR;
			break;

		case	ePAL_PB_SEEKPOSITION_END:
			eMode = eHxFILE_SEEK_END;
			break;

		default:
			eMode = eHxFILE_SEEK_SET;
			break;
	}
	hError = HLIB_FILE_Seek(hFile, 0, eMode, &ullPosOffset);
	if (hError != ERR_OK )
	{
		HxLOG_Error("[xsvc_epgcontext_Fread_Tvtv] HLIB_FILE_Seek fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	ulWriteSize = HLIB_FILE_Write(hFile, (void *)pBuffer, 1, ulUnitSize);
	if (ulWriteSize <= 0)
	{
		HxLOG_Error("[xsvc_epgcontext_Fread_Tvtv] HLIB_FILE_Write fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	*pulWriteSize = ulWriteSize;
	hError = ERR_OK;

EXIT_CNXTTVTV_FUNC :

	return hError;

}

STATIC HERROR		xsvc_epgcontext_Fclose_Tvtv(Handle_t hFileHandle)
{
	HERROR			hError = ERR_FAIL;

	if( hFileHandle == (Handle_t)0)
	{
		HxLOG_Error("[xsvc_epgcontext_Fclose_Tvtv] Param NULL!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	HLIB_FILE_Close((HxFILE_t)hFileHandle);

	hError = ERR_OK;

EXIT_CNXTTVTV_FUNC :

	return hError;
}

STATIC HERROR		xsvc_epgcontext_FileLength_Tvtv(HUINT8 *pucFileName, HUINT32 *pulFileLength)
{
	HxFILE_t		hFile;
	HERROR		hError = ERR_FAIL;
	HUINT32		ulFileSize;

	hFile = HLIB_FILE_Open(pucFileName,"r");
	if (hFile == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_Fread_Tvtv] HLIB_FILE_Open fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	hError = HLIB_FILE_Size32(hFile, &ulFileSize);
	if (hError != ERR_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_Fread_Tvtv] HLIB_FILE_Size32 fail!!\n");
		HLIB_FILE_Close(hFile);
		goto EXIT_CNXTTVTV_FUNC;
	}
	*pulFileLength = ulFileSize;

	HLIB_FILE_Close(hFile);
	hError = ERR_OK;

EXIT_CNXTTVTV_FUNC :

	return hError;

}

STATIC HERROR		xsvc_epgcontext_ReadFullFileToBuffer_Tvtv(HUINT8 *pszFileName, HUINT8 **ppBuffer, HUINT32 *pulReadSize)
{
	HERROR			hError = ERR_FAIL;
	Handle_t			hFileHandle = (Handle_t)NULL;
	HUINT32			ulFileSize = 0;
	HUINT32			ulSizeToRead = 0;
	HUINT8			*pucFileBuffer = NULL;
	HUINT8			*pucTempBuffer = NULL;
	HUINT32			ulIndex = 0;
	HUINT32			ulBlockCount = 0;
	HUINT32			ulRestSize = 0;

	if( pszFileName == NULL || ppBuffer == NULL || pulReadSize == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_ReadFullFileToBuffer_Tvtv] Param NULL!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}
	*ppBuffer = NULL;
	*pulReadSize = 0;

	hError = xsvc_epgcontext_FileLength_Tvtv(pszFileName, &ulFileSize);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_ReadFullFileToBuffer_Tvtv] filelength read fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	hError = xsvc_epgcontext_Fopen_Tvtv(pszFileName, &hFileHandle);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_ReadFullFileToBuffer_Tvtv] file open fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	pucFileBuffer = (HUINT8*)OxEPG_Malloc(sizeof(HUINT8)*(ulFileSize+1));
	if(pucFileBuffer == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_ReadFullFileToBuffer_Tvtv] OxEPG_Malloc fail!!\n");
		(void)xsvc_epgcontext_Fclose_Tvtv(hFileHandle);
		goto EXIT_CNXTTVTV_FUNC;
	}

	ulBlockCount = ulFileSize / TVTVEPG_FILE_READ_UNIT ;
	ulRestSize = ulFileSize % TVTVEPG_FILE_READ_UNIT;

	pucTempBuffer = pucFileBuffer;

	for(ulIndex = 0; ulIndex < ulBlockCount; ulIndex++)
	{
		hError = xsvc_epgcontext_Fread_Tvtv(pucTempBuffer, TVTVEPG_FILE_READ_UNIT, hFileHandle, ePAL_PB_SEEKPOSITION_CURRENT, &ulSizeToRead);
		if(hError != ERR_OK || ulSizeToRead == 0)
		{
			break;
		}
		pucTempBuffer += ulSizeToRead;
	}
	if(ulRestSize)
	{
		hError = xsvc_epgcontext_Fread_Tvtv(pucTempBuffer, ulRestSize, hFileHandle, ePAL_PB_SEEKPOSITION_CURRENT, &ulSizeToRead);
		if(hError != ERR_OK)
		{
			HxLOG_Print("[xsvc_epgcontext_ReadFullFileToBuffer_Tvtv] xsvc_epgcontext_Fread_Tvtv fail!!\n");
		}
		pucTempBuffer += ulSizeToRead;
	}
	if(pucTempBuffer - pucFileBuffer != ulFileSize)
	{
		(void)xsvc_epgcontext_Fclose_Tvtv(hFileHandle);
		HxLOG_Error("[xsvc_epgcontext_ReadFullFileToBuffer_Tvtv] file read fail !!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	///////////////////////////////////////////////////////////////////
	//
	// for LIBXML parser
	//
	///////////////////////////////////////////////////////////////////
	pucFileBuffer[ulFileSize] = '\0';

	hError = xsvc_epgcontext_Fclose_Tvtv(hFileHandle);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_ReadFullFileToBuffer_Tvtv] file close fail !!\n");
		(void)xsvc_epgcontext_Fclose_Tvtv(hFileHandle);
		goto EXIT_CNXTTVTV_FUNC;
	}

	*ppBuffer = pucFileBuffer;
	*pulReadSize = ulFileSize;
	pucFileBuffer = NULL;
	pucTempBuffer = NULL;

	hError = ERR_OK;

EXIT_CNXTTVTV_FUNC :
	if(pucFileBuffer)
	{
		OxEPG_Free(pucFileBuffer);
		pucFileBuffer = NULL;
		pucTempBuffer = NULL;
	}
	return hError;

}

STATIC HERROR		xsvc_epgcontext_ReadFileToBuffer_Tvtv(Handle_t hFileHandle, HUINT8 **ppBuffer, HUINT32 ulReadSize, HUINT32 *pulReadSize)
{
	HERROR			hError = ERR_FAIL;
	HUINT32			ulSizeToRead = 0;
	HUINT8			*pucFileBuffer = NULL;
	HUINT8			*pucTempBuffer = NULL;
	HUINT32			ulIndex = 0;
	HUINT32			ulBlockCount = 0;
	HUINT32			ulRestSize = 0;

	if( hFileHandle == 0 || ppBuffer == NULL || pulReadSize == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_ReadFileToBuffer_Tvtv] Param NULL!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}
	*ppBuffer = NULL;
	*pulReadSize = 0;

	pucFileBuffer = (HUINT8*)OxEPG_Malloc(sizeof(HUINT8)*(ulReadSize+1));
	if(pucFileBuffer == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_ReadFileToBuffer_Tvtv] OxEPG_Malloc fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	ulBlockCount = ulReadSize / TVTVEPG_FILE_READ_UNIT ;
	ulRestSize = ulReadSize % TVTVEPG_FILE_READ_UNIT;

	pucTempBuffer = pucFileBuffer;

	for(ulIndex = 0; ulIndex < ulBlockCount; ulIndex++)
	{
		hError = xsvc_epgcontext_CurrentposFread_Tvtv(pucTempBuffer, TVTVEPG_FILE_READ_UNIT, hFileHandle, &ulSizeToRead);
		if(hError != ERR_OK || ulSizeToRead == 0)
		{
			break;
		}
		pucTempBuffer += ulSizeToRead;
	}
	if(ulRestSize)
	{
		hError = xsvc_epgcontext_CurrentposFread_Tvtv(pucTempBuffer, ulRestSize, hFileHandle, &ulSizeToRead);
		if(hError != ERR_OK)
		{
			HxLOG_Print("[xsvc_epgcontext_ReadFileToBuffer_Tvtv] xsvc_epgcontext_Fread_Tvtv fail!!\n");
		}
		pucTempBuffer += ulSizeToRead;
	}
	if(pucTempBuffer - pucFileBuffer != ulReadSize)
	{
		OxEPG_Free(pucFileBuffer);
		pucFileBuffer = NULL;
		pucTempBuffer = NULL;

		HxLOG_Error("[xsvc_epgcontext_ReadFileToBuffer_Tvtv] file read fail !!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	///////////////////////////////////////////////////////////////////
	//
	// for LIBXML parser
	//
	///////////////////////////////////////////////////////////////////
	pucFileBuffer[ulReadSize] = '\0';

	*ppBuffer = pucFileBuffer;
	*pulReadSize = ulReadSize;
	hError = ERR_OK;

EXIT_CNXTTVTV_FUNC :

	return hError;

}

STATIC HERROR		xsvc_epgcontext_RemoveDirectory_Tvtv(void)
{
	HCHAR 			szCmd[128] = { 0 };
	HBOOL			bCurrentTvtvExisted = FALSE;

	bCurrentTvtvExisted = HLIB_DIR_IsExist(EPG_FS_TVTV_DATA_PATH);

	if(bCurrentTvtvExisted == FALSE)
	{
		return ERR_FAIL;
	}

	HxLOG_Trace();

	HxSTD_memset(szCmd, 0, sizeof(szCmd));
	HxSTD_snprintf( szCmd, sizeof(szCmd), "rm -rf %s", EPG_FS_TVTV_DATA_PATH);
	VK_SYSTEM_Command (szCmd);

	return ERR_OK;
}

HERROR XSVC_EPG_RemoveInfoDirectory_Tvtv(void);

STATIC HERROR		xsvc_epgcontext_InitDirectory_Tvtv(void)
{
	HERROR			hError = ERR_FAIL;
	HBOOL			bCurrentTvtvExisted = FALSE;

	XSVC_EPG_RemoveInfoDirectory_Tvtv();
	DAPI_ResetTable(DxNAMEOF(eDxEPG_TYPE_TVTV));
	DAPI_ResetTable(DxNAMEOF(DxTvtvService_t));

	bCurrentTvtvExisted = HLIB_DIR_IsExist(EPG_FS_TVTV_DATA_PATH);
	if(bCurrentTvtvExisted == TRUE)
	{
		goto EXIT_CNXTTVTV_FUNC;
	}

	// create directory
	hError = HLIB_DIR_Make(EPG_FS_TVTV_DATA_PATH);
	if (hError != ERR_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_InitDirectory_Tvtv] EPG_FS_TVTV_PATH fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	hError = ERR_OK;

EXIT_CNXTTVTV_FUNC :

	return hError;

}

STATIC HERROR		xsvc_epgcontext_SavePostprocessedFile_Tvtv(xsvcEpg_PostprocFile_t *pPostprocFile)
{
	HERROR					hError = ERR_FAIL;
	HUINT8					szFileName[256] = {0, };
	Handle_t					hFileHandle = (Handle_t)NULL;
	HUINT32					ulWriteSize = 0;
	HUINT32					ulReadSize = 0;
	xsvcEpg_PostprocFile_t			*pTempFile = NULL;
	HUINT32					ulStartTime = 0;
	HUINT32					ulEventTableNum = 0;
	HUINT32					ulFileSize = 0;
	SvcEpg_TvtvEventFileTable_t	*pstFileTable = NULL;
//	HBOOL					bPass = FALSE;

	if(pPostprocFile == NULL )
	{
		HxLOG_Error("[xsvc_epgcontext_SavePostprocessedFile_Tvtv] param NULL!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	pstFileTable = (SvcEpg_TvtvEventFileTable_t*)OxEPG_Malloc(sizeof(SvcEpg_TvtvEventFileTable_t));
	if(pstFileTable == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_SavePostprocessedFile_Tvtv] OxEPG_Malloc fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	pTempFile = pPostprocFile;
	while(pTempFile)
	{
		if(pTempFile->ulFileSize == 0 || pTempFile->pucFilePointer == NULL)
		{
			HxLOG_Error("[xsvc_epgcontext_SavePostprocessedFile_Tvtv] invalid file!!\n");
			goto END_FILESAVE_LOOP;
		}
		hFileHandle = 0;

		switch(pTempFile->eFileType)
		{
			case TVTV_FileType_LOGO :
			case TVTV_FileType_EPG_IMAGE :
				snprintf ((char *)szFileName, 256, "%s%s", EPG_FS_TVTV_IMAGE_PATH, pTempFile->pucFileName);
				hError = xsvc_epgcontext_Fopen_Tvtv(szFileName, &hFileHandle);
				if(hError != ERR_OK)
				{
					HxLOG_Error("[xsvc_epgcontext_SavePostprocessedFile_Tvtv] fopen fail!!\n");
					goto END_FILESAVE_LOOP;
				}

				hError = xsvc_epgcontext_Fwrite_Tvtv(pTempFile->pucFilePointer, pTempFile->ulFileSize, hFileHandle, ePAL_PB_SEEKPOSITION_CURRENT, &ulWriteSize);
				if(hError != ERR_OK || pTempFile->ulFileSize != ulWriteSize)
				{
					HxLOG_Error("[xsvc_epgcontext_SavePostprocessedFile_Tvtv] fwrite fail(%x : %x)!!\n", pTempFile->ulFileSize, ulWriteSize);
					goto END_FILESAVE_LOOP;
				}
				break;

			case TVTV_FileType_EPG_EVENT :

				/**********************************************************************************
				  *
				  * make table file name
				  *
				  **********************************************************************************/
				hError = xsvc_epglib_GetFileStartTime_Tvtv(pTempFile->pucFilePointer, pTempFile->ulFileSize, &ulStartTime);
				if(hError != ERR_OK)
				{
					HxLOG_Error("[xsvc_epgcontext_SavePostprocessedFile_Tvtv] xsvc_epglib_GetFileStartTime_Tvtv fail!!\n");
					goto END_FILESAVE_LOOP;
				}
				ulEventTableNum = ulStartTime/SECONDS_PER_DAY;
				snprintf ((char *)szFileName, 256, "%s%08d%s", EPG_FS_TVTV_DATA_PATH, ulEventTableNum, TVTV_FILENAME_TABLEEXTENSION);

				hError = xsvc_epgcontext_FileLength_Tvtv(szFileName, &ulFileSize);
				if(hError != ERR_OK)
				{
					HxLOG_Print("[xsvc_epgcontext_ReadFullFileToBuffer_Tvtv] filelength read fail!!\n");
				}

				/**********************************************************************************
				  *
				  * open table file
				  *
				  **********************************************************************************/
				hError = xsvc_epgcontext_Fopen_Tvtv(szFileName, &hFileHandle);
				if(hError != ERR_OK)
				{
					HxLOG_Error("[xsvc_epgcontext_SavePostprocessedFile_Tvtv] fopen fail!!\n");
					goto END_FILESAVE_LOOP;
				}

				/**********************************************************************************
				  *
				  * read existed table header
				  *
				  **********************************************************************************/
				VK_memset32(pstFileTable, 0x00, sizeof(SvcEpg_TvtvEventFileTable_t));
				if(ulFileSize != 0)
				{
					hError = xsvc_epgcontext_Fread_Tvtv(pstFileTable, sizeof(SvcEpg_TvtvEventFileTable_t), hFileHandle, ePAL_PB_SEEKPOSITION_START, &ulReadSize);
					if(hError != ERR_OK)
					{
						HxLOG_Error("[xsvc_epgcontext_ReadFullFileToBuffer_Tvtv] xsvc_epgcontext_Fread_Tvtv fail!!\n");
						goto END_FILESAVE_LOOP;
					}
				}
				/**********************************************************************************
				  *
				  * update table header
				  *
				  **********************************************************************************/
				if(pstFileTable->ulNumOfFile >=  TVTV_EVENTFILE_NUM_MAX)
				{
					HxLOG_Error("[xsvc_epgcontext_ReadFullFileToBuffer_Tvtv] stFileTable.ulNumOfFile >=  TVTV_EVENTFILE_NUM_MAX!!\n");
					goto END_FILESAVE_LOOP;
				}
				pstFileTable->ulTableVersion = TVTV_TABLEFILE_CURRENT_VERSION;
				pstFileTable->aulFileSizeTable[pstFileTable->ulNumOfFile] = pTempFile->ulFileSize;
				pstFileTable->ulSizeOfFile +=  pTempFile->ulFileSize;
				pstFileTable->ulNumOfFile++;

				hError = xsvc_epgcontext_Fwrite_Tvtv(pstFileTable, sizeof(SvcEpg_TvtvEventFileTable_t), hFileHandle, ePAL_PB_SEEKPOSITION_START, &ulWriteSize);
				if(hError != ERR_OK)
				{
					HxLOG_Error("[xsvc_epgcontext_SavePostprocessedFile_Tvtv] fwrite fail(%x : %x)!!\n", pTempFile->ulFileSize, ulWriteSize);
					goto END_FILESAVE_LOOP;
				}

				/**********************************************************************************
				  *
				  * write XML file
				  *
				  **********************************************************************************/
				ulWriteSize = 0;
				hError = xsvc_epgcontext_Fwrite_Tvtv(pTempFile->pucFilePointer, pTempFile->ulFileSize, hFileHandle, ePAL_PB_SEEKPOSITION_END, &ulWriteSize);
				if(hError != ERR_OK || pTempFile->ulFileSize != ulWriteSize)
				{
					HxLOG_Error("[xsvc_epgcontext_SavePostprocessedFile_Tvtv] fwrite fail(%x : %x)!!\n", pTempFile->ulFileSize, ulWriteSize);
					goto END_FILESAVE_LOOP;
				}

				break;

			default :
				snprintf ((char *)szFileName, 256, "%s%s", EPG_FS_TVTV_DATA_PATH, pTempFile->pucFileName);
				hError = xsvc_epgcontext_Fopen_Tvtv(szFileName, &hFileHandle);
				if(hError != ERR_OK)
				{
					HxLOG_Error("[xsvc_epgcontext_SavePostprocessedFile_Tvtv] fopen fail!!\n");
					goto END_FILESAVE_LOOP;
				}

				hError = xsvc_epgcontext_Fwrite_Tvtv(pTempFile->pucFilePointer, pTempFile->ulFileSize, hFileHandle, ePAL_PB_SEEKPOSITION_START, &ulWriteSize);
				if(hError != ERR_OK || pTempFile->ulFileSize != ulWriteSize)
				{
					HxLOG_Error("[xsvc_epgcontext_SavePostprocessedFile_Tvtv] fwrite fail(%x : %x)!!\n", pTempFile->ulFileSize, ulWriteSize);
					goto END_FILESAVE_LOOP;
				}
				break;

		}

END_FILESAVE_LOOP :

		if(hFileHandle != 0)
		{
			hError = xsvc_epgcontext_Fclose_Tvtv(hFileHandle);
			if(hError != ERR_OK)
			{
				HxLOG_Print("[xsvc_epgcontext_SavePostprocessedFile_Tvtv] fclose fail!!\n");
			}
#if			1
			HxLOG_Info("[xsvc_epgcontext_SavePostprocessedFile_Tvtv] file name(%s)!!\n", szFileName);
#endif
		}
		pTempFile = pTempFile->pNext;

	}

	hError = ERR_OK;

EXIT_CNXTTVTV_FUNC :
	if(pstFileTable)
	{
		OxEPG_Free(pstFileTable);
		pstFileTable = NULL;
	}


	return hError;
}


#define	______TEMP_FILEIO_END______


STATIC HERROR				xsvc_epgcontext_InitControlProcess_Tvtv(void)
{
	HERROR		hError = ERR_FAIL;

	if(s_stTvtvProcess.ulSemId == 0)
	{
		hError = VK_SEM_Create((unsigned long*)&s_stTvtvProcess.ulSemId, "TVTV_STATE", 0);
		if(hError != ERR_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_InitControlProcess_Tvtv] VK_SEM_Create!!\n");
			return hError;
		}
	}
	VK_SEM_Get(s_stTvtvProcess.ulSemId);

	s_stTvtvProcess.eCurrentProcess = TvtvProcess_Nothing;

	VK_SEM_Release(s_stTvtvProcess.ulSemId);

	return ERR_OK;
}

STATIC HERROR				xsvc_epgcontext_GetControlProcess_Tvtv(xsvcEpg_TvtvProcessState_e *pCurrentState)
{

	VK_SEM_Get(s_stTvtvProcess.ulSemId);

	*pCurrentState = s_stTvtvProcess.eCurrentProcess;

	VK_SEM_Release(s_stTvtvProcess.ulSemId);

	return ERR_OK;
}

STATIC HERROR				xsvc_epgcontext_ControlAction_Tvtv(HUINT32 ulCommand, HBOOL bProgressing, HBOOL *pbPass)
{
	HERROR				hError = ERR_EPG_FAIL;
	HUINT32				ulMessage = 0;
//	Handle_t				hAction = 0;
//	HUINT8				ucHead = 0;
	HUINT16				usResult = 0;

	*pbPass = FALSE;
	if(s_stTvtvProcess.ulSemId == 0)
	{
		hError = VK_SEM_Create((unsigned long*)&s_stTvtvProcess.ulSemId, "TVTV_STATE", VK_SUSPENDTYPE_FIFO);
		if(hError != ERR_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_ControlAction_Tvtv] VK_SEM_Create!!\n");
			return hError;
		}
	}
	VK_SEM_Get(s_stTvtvProcess.ulSemId);

	switch(s_stTvtvProcess.eCurrentProcess)
	{
		case TvtvProcess_Nothing:
			if(bProgressing == TRUE)
			{
				*pbPass = TRUE;
				break;
			}
			if(ulCommand == EPG_CMD_START)
			{
				HxLOG_Info("[xsvc_epgcontext_ControlAction_Tvtv] eCurrentProcess(TvtvProcess_Nothing)ulCommand(EPG_CMD_START)!!\n");
				s_stTvtvProcess.eCurrentProcess = TvtvProcess_DownLoading;
			}
			else if(ulCommand == EPG_CMD_STOP_TVTV_ACTION)
			{
				HxLOG_Info("[xsvc_epgcontext_ControlAction_Tvtv] eCurrentProcess(TvtvProcess_Nothing)ulCommand(EPG_CMD_STOP_TVTV_ACTION)!!\n");
				*pbPass = TRUE;
			}
			else
			{
				HxLOG_Info("[xsvc_epgcontext_ControlAction_Tvtv] eCurrentProcess(TvtvProcess_Nothing)ulCommand(?)!!\n");
			}
			break;

		case TvtvProcess_DownLoading :
			if(bProgressing == TRUE)
			{
				if(ulCommand != EPG_CMD_START)
				{
					*pbPass = TRUE;
				}
				break;
			}
			if(ulCommand == EPG_CMD_START)
			{
				HxLOG_Info("[xsvc_epgcontext_ControlAction_Tvtv] eCurrentProcess(TvtvProcess_DownLoading)ulCommand(EPG_CMD_START)!!\n");
				*pbPass = TRUE;
			}
			else if(ulCommand == EPG_CMD_STOP_TVTV_ACTION)
			{
				HxLOG_Info("[xsvc_epgcontext_ControlAction_Tvtv] eCurrentProcess(TvtvProcess_DownLoading)ulCommand(EPG_CMD_STOP_TVTV_ACTION)!!\n");
				s_stTvtvProcess.eCurrentProcess = TvtvProcess_Nothing;

				hError = SVC_EPG_MakeEpgMessage(EPG_CMD_STOP_FILTER,  eEPG_PROCESS_TVTV, &ulMessage);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Print("[xsvc_epgcontext_PostPAT_Tvtv] MW_EPGMAIN_MakeEpgMessage fail!!\n");
				}
				hError = SVC_EPG_SendMessage(ulMessage, s_stTvtvProcess.hAction, s_stTvtvProcess.ucHead, usResult);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Print("[xsvc_epgcontext_PostPAT_Tvtv] SendMessage fail!!\n");
				}
				*pbPass = TRUE;
			}
			else
			{
				HxLOG_Info("[xsvc_epgcontext_ControlAction_Tvtv] eCurrentProcess(TvtvProcess_DownLoading)ulCommand(?)!!\n");
			}
			break;

		case TvtvProcess_DataLoading:
			if(bProgressing == TRUE)
			{
				break;
			}

			if(ulCommand == EPG_CMD_START)
			{
				HxLOG_Info("[xsvc_epgcontext_ControlAction_Tvtv] eCurrentProcess(TvtvProcess_DataLoading)ulCommand(EPG_CMD_START)!!\n");
				*pbPass = TRUE;
			}
			else if(ulCommand == EPG_CMD_STOP_TVTV_ACTION)
			{
				HxLOG_Info("[xsvc_epgcontext_ControlAction_Tvtv] eCurrentProcess(TvtvProcess_DataLoading)ulCommand(EPG_CMD_STOP_TVTV_ACTION)!!\n");
				s_stTvtvProcess.eCurrentProcess = TvtvProcess_Nothing;

				*pbPass = TRUE;
			}
			else
			{
				HxLOG_Info("[xsvc_epgcontext_ControlAction_Tvtv] eCurrentProcess(TvtvProcess_DataLoading)ulCommand(?)!!\n");
			}
			break;

		case TvtvProcess_DataUnloading :
			if(bProgressing == TRUE)
			{
				break;
			}

			if(ulCommand == EPG_CMD_START)
			{
				HxLOG_Info("[xsvc_epgcontext_ControlAction_Tvtv] eCurrentProcess(TvtvProcess_DataUnloading)ulCommand(EPG_CMD_START)!!\n");
				*pbPass = TRUE;
			}
			else if(ulCommand == EPG_CMD_STOP_TVTV_ACTION)
			{
				HxLOG_Info("[xsvc_epgcontext_ControlAction_Tvtv] eCurrentProcess(TvtvProcess_DataUnloading)ulCommand(EPG_CMD_STOP_TVTV_ACTION)!!\n");
				s_stTvtvProcess.eCurrentProcess = TvtvProcess_Nothing;

				*pbPass = TRUE;
			}
			else
			{
				HxLOG_Info("[xsvc_epgcontext_ControlAction_Tvtv] eCurrentProcess(TvtvProcess_DataUnloading)ulCommand(?)!!\n");
			}
			break;

		default :
			HxLOG_Error("[xsvc_epgcontext_ControlAction_Tvtv] default :!!\n");
			break;

	}


	hError = ERR_EPG_OK;

//EXIT_CNXTTVTV_FUNC :
	VK_SEM_Release(s_stTvtvProcess.ulSemId);

	return hError;
}

STATIC HERROR				xsvc_epgcontext_StartFilterForPmtSignaling_Tvtv(svcEpg_ContextData_t *pCnxtData)
{
	HERROR				hError = ERR_EPG_FAIL;
	SiRetrieveParam_t	stFilterParam = {0, };
	HUINT16				usFilterId = 0;
	HUINT16				usTsUniqueId = 0;

	if(pCnxtData == NULL)
	{
		HxLOG_Print("[xsvc_epgcontext_StartFilterForPmtSignaling_Tvtv] param fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}
	usTsUniqueId = pCnxtData->usTsUniqId;

	/**********************************************************************************
	  *
	  * 1. pat filtering
	  *
	  **********************************************************************************/
	HxSTD_memset(&stFilterParam, 0x00, sizeof(SiRetrieveParam_t));
	hError = svc_epg_SiSetDefaultRetrieveParam(&stFilterParam, pCnxtData->hAction, usTsUniqueId, pCnxtData->ulDemuxId);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_StartFilterForPmtSignaling_Tvtv] SetDefaultRetrieveParam fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}
	stFilterParam.ucMode = eSiFilterMode_Once;
	hError = svc_epg_SiRetrievePatSection(&stFilterParam,	(void*)xsvc_epgcontext_PostPAT_Tvtv,
														(void*)xsvc_epgcontext_FreePAT_Tvtv,
														&usFilterId);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_StartFilterForPmtSignaling_Tvtv] svc_epg_SiRetrievePatSection fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}
	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_PAT].eState = eEPG_CNXT_FILTER_ONCE;
	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_PAT].usPid = PAT_PID;
	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_PAT].ulTable = PAT_TABLE_ID;
	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_PAT].usFilterId = usFilterId;
	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_PAT].ulBufferSize = 0;
	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_PAT].ucCycle = 0;
	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_PAT].bComplete= FALSE;
	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_PAT].bTimeOut = FALSE;
	HxSTD_memset(&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_PAT].stBaseInfo), 0x00, sizeof(svcEpg_ServiceInfo_t));

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :

	return hError;
}

STATIC HERROR				xsvc_epgcontext_StartPmtFilter_Tvtv(svcEpg_ContextData_t *pCnxtData)
{
	HERROR				hError = ERR_EPG_FAIL;
	SiRetrieveParam_t	stFilterParam;
	HUINT16				usFilterId = 0;
	HUINT16				usTsUniqueId = 0;
	HUINT16				usPmtPid = 0;
	HBOOL				bFound = FALSE;

	if(pCnxtData == NULL)
	{
		HxLOG_Error("[local_cnxttvtv_StartSigInfoFilter] param fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}
	usTsUniqueId = pCnxtData->usTsUniqId;

	hError = xsvc_epgcontext_GetPmtPid_Tvtv(usTsUniqueId, pCnxtData->stTvtvTripleId.usSvcId, &bFound, &usPmtPid);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_StartPmtFilter_Tvtv] xsvc_epgcontext_GetPmtPid_Tvtv fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	HxSTD_memset(&stFilterParam, 0x00, sizeof(SiRetrieveParam_t));
	hError = svc_epg_SiSetDefaultRetrieveParam(&stFilterParam, pCnxtData->hAction, usTsUniqueId, pCnxtData->ulDemuxId);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_StartPmtFilter_Tvtv] SetDefaultRetrieveParam fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}
	stFilterParam.usPid = usPmtPid;
	stFilterParam.usExtId = pCnxtData->stTvtvTripleId.usSvcId;
	stFilterParam.bUseExtId = TRUE;
	stFilterParam.bCrc = TRUE;

	hError = svc_epg_SiRetrievePmtSection(&stFilterParam, (void*)xsvc_epgcontext_PostPMT_Tvtv, (void*)xsvc_epgcontext_FreePMT_Tvtv, &usFilterId);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_StartPmtFilter_Tvtv] RetrieveEitPfSection fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}
	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_PMT].eState = eEPG_CNXT_FILTER_ONCE;
	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_PMT].usPid = usPmtPid;
	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_PMT].ulTable = PMT_TABLE_ID;
	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_PMT].usFilterId = usFilterId;
	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_PMT].ulBufferSize = 0;
	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_PMT].ucCycle = 0;
	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_PMT].bComplete= FALSE;
	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_PMT].bTimeOut = FALSE;

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :

	return hError;
}

STATIC HERROR		xsvc_epgcontext_ProcessPAT_Tvtv(svcEpg_ContextData_t *pCnxtData)
{
	HERROR					hError = ERR_EPG_FAIL;


	if(pCnxtData == NULL )
	{
		HxLOG_Error("[xsvc_epgcontext_ProcessPAT_Tvtv] param null!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}
	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_PAT].bComplete = TRUE;

	hError = xsvc_epgcontext_StartPmtFilter_Tvtv(pCnxtData);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_ProcessPAT_Tvtv] xsvc_epgcontext_StartPmtFilter_Tvtv fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :

	return hError;
}

STATIC HERROR		xsvc_epgcontext_ProcessPMT_Tvtv(svcEpg_ContextData_t *pCnxtData)
{
	HERROR					hError = ERR_EPG_FAIL;
	HUINT16					usTsUniqueId = 0;
	HUINT16					usFilterId = 0;
	SiRetrieveParam_t		stFilterParam;
	HUINT16					usDsmccPid = 0;


	if(pCnxtData == NULL )
	{
		HxLOG_Error("[xsvc_epgcontext_ProcessPMT_Tvtv] param null!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}
	usTsUniqueId = pCnxtData->usTsUniqId;


	hError = xsvc_epgcontext_GetDsmccPid_Tvtv(usTsUniqueId, pCnxtData->stTvtvTripleId.usSvcId, TVTV_COMPONENT_TAG, &usDsmccPid);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_ProcessPMT_Tvtv] xsvc_epgcontext_GetDsmccPid_Tvtv fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	hError = xsvc_epgcontext_InitDownloadInfo_Tvtv(pCnxtData);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_ProcessPMT_Tvtv] xsvc_epgcontext_InitDownloadInfo_Tvtv fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	hError = xsvc_epglib_CreateBuffer_Tvtv();
	if(hError != ERR_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_ProcessPMT_Tvtv] xsvc_epglib_CreateBuffer_Tvtv fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

/********************************************************************************************************
 *
 * set dsi & dii filtering
 *
 ********************************************************************************************************/
 #if 1
	if(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DSI].eState == eEPG_CNXT_FILTER_EMPTY)
	{
		hError = PAL_DSMCC_Init();
		if(hError != ERR_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_ProcessPMT_Tvtv] PAL_DSMCC_Init fail!!\n");
			goto EXIT_CNXTTVTV_FUNC;
		}
		hError = UTIL_TASK_AllocTaskItem(	sizeof(svcEpg_Msg_t),
											(void*)xsvc_epgcontext_ProcessDsi_Tvtv,
											(void*)NULL,
											&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DSI].ucTaskSlot));
		if(hError != ERR_TASKUTIL_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_ProcessPMT_Tvtv] EPGCNXT_FITER_SLOT_TVTV_DSI UTIL_TASK_AllocTaskItem fail!!\n");
			goto EXIT_CNXTTVTV_FUNC;
		}
	}
	HxSTD_memset(&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DSI].stBaseInfo), 0x00, sizeof(svcEpg_ServiceInfo_t));

	hError = PAL_DSMCC_FreeDsmcc();
	if(hError != ERR_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_ProcessPMT_Tvtv] PAL_DSMCC_FreeDsmcc fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}
	hError = PAL_DSMCC_SetDsmccLayerType(ePAL_DSMCC_TWO_LAYER);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_ProcessPMT_Tvtv] PAL_DSMCC_SetDsmccLayerType fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	HxSTD_memset(&stFilterParam, 0x00, sizeof(SiRetrieveParam_t));
	hError = svc_epg_SiSetDefaultRetrieveParam(&stFilterParam, pCnxtData->hAction, usTsUniqueId, pCnxtData->ulDemuxId);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_ProcessPMT_Tvtv] SetDefaultParam fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}
	stFilterParam.ucMode = eSiFilterMode_Continue;
	stFilterParam.usPid	= usDsmccPid;
	stFilterParam.bTableFilter = FALSE;
	stFilterParam.bUseExtId = FALSE;
	stFilterParam.bCrc = TRUE;

	hError = svc_epg_SiRetrieveDsiSection(&stFilterParam, xsvc_epgcontext_PostDsi_Tvtv, NULL, &usFilterId);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_ProcessPMT_Tvtv] svc_epg_SiRetrieveDdbSection fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}
	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DSI].eState = eEPG_CNXT_FILTER_CONTINUE;
	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DSI].usFilterId = usFilterId;
	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DSI].ulBufferSize = 0;
	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DSI].ucCycle = 0;
	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DSI].bComplete= FALSE;
	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DSI].bTimeOut = FALSE;
#endif

#if 1
/********************************************************************************************************
 *
 * set ddb filtering
 *
 ********************************************************************************************************/
	if(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DDB].eState == eEPG_CNXT_FILTER_EMPTY)
	{
		hError = UTIL_TASK_AllocTaskItem(	sizeof(svcEpg_Msg_t),
											(void*)xsvc_epgcontext_ProcessDdb_Tvtv,
											(void*)NULL,
											&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DDB].ucTaskSlot));
		if(hError != ERR_TASKUTIL_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_ProcessPMT_Tvtv] EPGCNXT_FITER_SLOT_TVTV_DDB UTIL_TASK_AllocTaskItem fail!!\n");
			goto EXIT_CNXTTVTV_FUNC;
		}
	}
	HxSTD_memset(&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DDB].stBaseInfo), 0x00, sizeof(svcEpg_ServiceInfo_t));

	hError = svc_epg_SiRetrieveDdbSection(&stFilterParam, xsvc_epgcontext_PostDdb_Tvtv, NULL, &usFilterId);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_ProcessPMT_Tvtv] svc_epg_SiRetrieveDdbSection fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}
	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DDB].eState = eEPG_CNXT_FILTER_CONTINUE;
	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DDB].usFilterId = usFilterId;
	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DDB].ulBufferSize = 0;
	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DDB].ucCycle = 0;
	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DDB].bComplete= FALSE;
	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DDB].bTimeOut = FALSE;

	pCnxtData->stDownloadInfo.ulCurrentDownloadedSize = 0;
	pCnxtData->stDownloadInfo.ulEstimatedDownloadSize= 0;
	pCnxtData->stDownloadInfo.ulNumOfDownloadedFile= 0;
	pCnxtData->stDownloadInfo.bCompleteDownload = FALSE;
#endif

#if 1
/********************************************************************************************************
 *
 * set postprocess info
 *
 ********************************************************************************************************/
	if(pCnxtData->stPostprocessInfo.eState == EPG_CNXTTVTV_EMPTY)
	{
		hError = UTIL_TASK_AllocTaskItem(	sizeof(svcEpg_Msg_t),
											(void*)xsvc_epgcontext_Postprosess_Tvtv,
											(void*)NULL,
											&(pCnxtData->stPostprocessInfo.ucTaskSlot));
		if(hError != ERR_TASKUTIL_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_ProcessPMT_Tvtv] xsvc_epgcontext_Postprosess_Tvtv UTIL_TASK_AllocTaskItem fail!!\n");
			goto EXIT_CNXTTVTV_FUNC;
		}
	}
	pCnxtData->stPostprocessInfo.eState= EPG_CNXTTVTV_CONTINUE;
	pCnxtData->stPostprocessInfo.ulNumOfPostprocessedFile = 0;
	pCnxtData->stPostprocessInfo.ulSendPostprocessedFile = 0;
	pCnxtData->stPostprocessInfo.bCompletePostprocess = FALSE;
	pCnxtData->stPostprocessInfo.bSetDgkt = FALSE;
	pCnxtData->stPostprocessInfo.bSetTvst= FALSE;
#endif

#if defined(CONFIG_TVTVEPG_LIVEDOWNLOAD)
/********************************************************************************************************
 *
 * set xmpl parsing process info
 *
 ********************************************************************************************************/
	if(pCnxtData->stXMLParseInfo.eState == EPG_CNXTTVTV_EMPTY)
	{
		hError = UTIL_TASK_AllocTaskItem(	sizeof(svcEpg_Msg_t),
											(void*)xsvc_epgcontext_ProcessXMLParse_Tvtv,
											(void*)NULL,
											&(pCnxtData->stXMLParseInfo.ucTaskSlot));
		if(hError != ERR_TASKUTIL_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_ProcessPMT_Tvtv] xsvc_epgcontext_ProcessXMLParse_Tvtv UTIL_TASK_AllocTaskItem fail!!\n");
			goto EXIT_CNXTTVTV_FUNC;
		}
	}
	pCnxtData->stXMLParseInfo.eState = EPG_CNXTTVTV_CONTINUE;
	pCnxtData->stXMLParseInfo.ulReceivePostprocessedFile = 0;
	pCnxtData->stXMLParseInfo.bCompleteParse = FALSE;
#endif


	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :

	return hError;
}

STATIC HERROR				xsvc_epgcontext_InitDownloadInfo_Tvtv(svcEpg_ContextData_t *pCnxtData)
{
	HERROR				hError = ERR_EPG_FAIL;

	if(pCnxtData == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_StopAllFilter_Tvtv] param fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	HxSTD_memset(&pCnxtData->stDownloadInfo, 0x00, sizeof(svcEpg_DownloadInfo_t));

	if(pCnxtData->stPostprocessInfo.eState == EPG_CNXTTVTV_EMPTY)
	{
		HxSTD_memset(&pCnxtData->stPostprocessInfo, 0x00, sizeof(svcEpg_PostProcessInfo_t));
	}
	else
	{
		pCnxtData->stPostprocessInfo.bSetTvst= FALSE;
		pCnxtData->stPostprocessInfo.bSetDgkt = FALSE;
		pCnxtData->stPostprocessInfo.bCompletePostprocess = FALSE;
		pCnxtData->stPostprocessInfo.ulNumOfConvertedImageFile = 0;
		pCnxtData->stPostprocessInfo.ulNumOfPostprocessedFile = 0;
		pCnxtData->stPostprocessInfo.ulSendPostprocessedFile = 0;
	}

	if(pCnxtData->stXMLParseInfo.eState == EPG_CNXTTVTV_EMPTY)
	{
		HxSTD_memset(&pCnxtData->stXMLParseInfo, 0x00, sizeof(svcEpg_XmlParseInfo_t));
	}
	else
	{
		pCnxtData->stXMLParseInfo.bCompleteParse = FALSE;
		pCnxtData->stXMLParseInfo.bSetTsmt = FALSE;
		pCnxtData->stXMLParseInfo.ulReceivePostprocessedFile = 0;
	}

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :

	return hError;

}

STATIC HERROR				xsvc_epgcontext_StopAllFilter_Tvtv(svcEpg_ContextData_t *pCnxtData)
{
	HERROR				hError = ERR_EPG_FAIL;

	if(pCnxtData == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_StopAllFilter_Tvtv] param fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	if(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_PAT].eState == eEPG_CNXT_FILTER_ONCE)
	{
		hError = xsvc_epgcontext_StopFilter_Tvtv( &(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_PAT]) );
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_StopAllFilter_Tvtv] EPGCNXT_FITER_SLOT_TVTV_PAT fail!!\n");
		}
		HxSTD_memset(&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_PAT].stBaseInfo), 0x00, sizeof(svcEpg_ServiceInfo_t));
		pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_PAT].eState = eEPG_CNXT_FILTER_NOTUSED;
	}

	if(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_PMT].eState == eEPG_CNXT_FILTER_ONCE)
	{
		hError = xsvc_epgcontext_StopFilter_Tvtv( &(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_PMT]) );
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_StopAllFilter_Tvtv] EPGCNXT_FITER_SLOT_TVTV_PMT fail!!\n");
		}
		HxSTD_memset(&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_PMT].stBaseInfo), 0x00, sizeof(svcEpg_ServiceInfo_t));
		pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_PMT].eState = eEPG_CNXT_FILTER_NOTUSED;
	}

	if(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DSI].eState == eEPG_CNXT_FILTER_CONTINUE)
	{
		HxLOG_Info("[xsvc_epgcontext_StopAllFilter_Tvtv] EPGCNXT_FITER_SLOT_TVTV_DSI!!\n");
		hError = xsvc_epgcontext_StopFilter_Tvtv( &(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DSI]) );
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_StopAllFilter_Tvtv] EPGCNXT_FITER_SLOT_TVTV_DSI fail!!\n");
		}
		HxSTD_memset(&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DSI].stBaseInfo), 0x00, sizeof(svcEpg_ServiceInfo_t));
		pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DSI].eState = eEPG_CNXT_FILTER_NOTUSED;
		pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DSI].ulTransactionId = 0;
	}

	if(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DDB].eState == eEPG_CNXT_FILTER_CONTINUE)
	{
		HxLOG_Info("[xsvc_epgcontext_StopAllFilter_Tvtv] EPGCNXT_FITER_SLOT_TVTV_DDB!!\n");
		hError = xsvc_epgcontext_StopFilter_Tvtv( &(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DDB]) );
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_StopAllFilter_Tvtv] EPGCNXT_FITER_SLOT_TVTV_DDB fail!!\n");
		}
		HxSTD_memset(&(pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DDB].stBaseInfo), 0x00, sizeof(svcEpg_ServiceInfo_t));
		pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DDB].eState = eEPG_CNXT_FILTER_NOTUSED;
	}

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :

	return hError;

}

STATIC HERROR				xsvc_epgcontext_StopFilter_Tvtv(svcEpg_Filter_t *pstFilterInfo)
{
	HERROR				hError = ERR_EPG_FAIL;

	if(pstFilterInfo == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_StopFilter_Tvtv] param fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	hError = svc_epg_SiStopOneSection(pstFilterInfo->usFilterId);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_StopFilter_Tvtv] StopOneSection fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :

	return hError;
}

STATIC HERROR				xsvc_epgcontext_GetPmtPid_Tvtv(HUINT16 usTsUniqueId, HUINT16 usSvcId, HBOOL *pbFound, HUINT16 *pusPmtPid)
{
	HERROR					hError = ERR_EPG_FAIL;
	HUINT16					usPmtPid = 0;
	svcEpg_RawPool_t		*pstRawPool = NULL;


	if(pusPmtPid == NULL || pbFound == NULL)
	{
		HxLOG_Print("[xsvc_epgcontext_GetPmtPid_Tvtv] param null!!\n");
		return hError;
	}
	*pusPmtPid = 0;
	*pbFound = FALSE;

	ENTER_CRITICAL_EPGRAW;

	hError = SVC_EPGRAWMGR_GetRawPool(&pstRawPool);
	if(hError != ERR_OK || pstRawPool == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_GetPmtPid_Tvtv] SVC_EPGRAWDB_GetRawPool fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	hError = svc_epg_SiGetPmtPidByServiceId(pstRawPool, usTsUniqueId, usSvcId, &usPmtPid);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_GetPmtPid_Tvtv] svc_epg_SiGetPmtPidByServiceId fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	*pusPmtPid = usPmtPid;
	*pbFound = TRUE;

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :

	LEAVE_CRITICAL_EPGRAW;

	return hError;
}

STATIC HERROR		xsvc_epgcontext_GetDsmccPid_Tvtv(HUINT16 usTsUniqueId, HUINT16 usProgNum, HUINT16 usAssociationTag, HUINT16 *pusDsmccPid)
{
	HERROR					hError = ERR_EPG_FAIL;
	HUINT32					ulPmtSectionCount = 0;
	HUINT32					ulDesCount = 0;
	HUINT32					ulPmtElementCount = 0;
	HUINT16					usSectionIndex = 0;
	HUINT16					usElementIndex = 0;
	HUINT16					usDesIndex = 0;
	PmtProgramInfo_t		stPmtProgInfo;
	PmtElementInfo_t		stPmtEleInfo;
//	svcCas_StmIdentDes_t			*pstStrmIdentDes = NULL;
	SIxStmIdentDes_t		*pstStrmIdentDes = NULL;
	svcEpg_RawPool_t		*pstRawPool = NULL;


	if(pusDsmccPid == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_GetDsmccPid_Tvtv] param null!!\n");
		return hError;
	}
	*pusDsmccPid = 0;
	HxSTD_memset(&stPmtProgInfo, 0x00, sizeof(PmtProgramInfo_t));
	HxSTD_memset(&stPmtEleInfo, 0x00, sizeof(PmtElementInfo_t));

	ENTER_CRITICAL_EPGRAW;

	hError = SVC_EPGRAWMGR_GetRawPool(&pstRawPool);
	if(hError != ERR_OK || pstRawPool == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_GetDsmccPid_Tvtv] SVC_EPGRAWDB_GetRawPool fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	hError = svc_epg_SiCountPmtSection (pstRawPool, usTsUniqueId, &ulPmtSectionCount);
	if(hError != ERR_OK || ulPmtSectionCount == 0)
	{
		HxLOG_Error("[xsvc_epgcontext_GetDsmccPid_Tvtv] CountPmtSection fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

//////////////////////////////////////////////////////////////////////////
// section loop
//////////////////////////////////////////////////////////////////////////
	for(usSectionIndex =0; usSectionIndex < ulPmtSectionCount; usSectionIndex++ )
	{
		hError = svc_epg_SiGetPmtProgramInfo(pstRawPool, usTsUniqueId, usSectionIndex, &stPmtProgInfo);
		if(hError != ERR_OK || stPmtProgInfo.usProgNum != usProgNum)
		{
			HxLOG_Print("[xsvc_epgcontext_GetDsmccPid_Tvtv] GetPmtProgramInfo fail!!\n");
			continue;
		}

		hError = svc_epg_SiCountPmtElement(pstRawPool, usTsUniqueId, usSectionIndex, &ulPmtElementCount);
		if(hError != ERR_OK || ulPmtElementCount == 0)
		{
			HxLOG_Print("[xsvc_epgcontext_GetDsmccPid_Tvtv] CountPmtElement fail!!\n");
			continue;
		}

//////////////////////////////////////////////////////////////////////////
// element loop
//////////////////////////////////////////////////////////////////////////
		for(usElementIndex = 0; usElementIndex < ulPmtElementCount; usElementIndex++)
		{

			hError = svc_epg_SiGetPmtElementInfo(pstRawPool, usTsUniqueId, usSectionIndex, usElementIndex, &stPmtEleInfo);
			if(hError != ERR_OK )
			{
				HxLOG_Print("[xsvc_epgcontext_GetDsmccPid_Tvtv] GetPmtElementInfo fail!!\n");
				continue;
			}

			hError = svc_epg_SiCountPmtElementDescriptor(pstRawPool, usTsUniqueId, usSectionIndex, usElementIndex, eSIxDESCTAG_STREAM_IDENTIFIER, &ulDesCount);
			if(hError != ERR_OK || ulDesCount == 0)
			{
				HxLOG_Print("[xsvc_epgcontext_GetDsmccPid_Tvtv] CountPmtElementDescriptor fail!!\n");
				continue;
			}

//////////////////////////////////////////////////////////////////////////
// descriptor loop
//////////////////////////////////////////////////////////////////////////
			for(usDesIndex = 0; usDesIndex < ulDesCount; usDesIndex++)
			{
				hError = svc_epg_SiGetPmtElementDescriptor(	pstRawPool,
															usTsUniqueId,
															usSectionIndex,
															usElementIndex,
															eSIxDESCTAG_STREAM_IDENTIFIER /*TAG_STREAM_IDENTIFIER*/,
															usDesIndex,
															(SIxDescriptor_t **)&pstStrmIdentDes);
				if( hError != ERR_OK )
				{
					HxLOG_Print("[xsvc_epgcontext_GetDsmccPid_Tvtv] GetPmtElementDescriptor fail!!\n");
					continue;
				}

				// ucCompTag : pmt stream identifier (same with data broadcast des comptag)
				if(pstStrmIdentDes->ucCompTag == (HUINT8)usAssociationTag)
				{
					HxLOG_Info("[xsvc_epgcontext_GetDsmccPid_Tvtv] FOUND!! : dsmcc pid(%x)\n", stPmtEleInfo.usElemPid);
					*pusDsmccPid = stPmtEleInfo.usElemPid;
					hError =  ERR_EPG_OK;
					goto EXIT_CNXTTVTV_FUNC;
				}
			}
		}
	}

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :

	LEAVE_CRITICAL_EPGRAW;

	return hError;
}

STATIC HERROR				xsvc_epgcontext_GetDownloadInfo_Tvtv(Handle_t hAction, svcEpg_DownloadInfo_t *pDownloadInfo)
{
	HERROR				hError = ERR_EPG_FAIL;
	svcEpg_Context_t 		*pContext = NULL;
	svcEpg_ContextData_t		*pCnxtData = NULL;

	if(pDownloadInfo == NULL || hAction == HANDLE_NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_GetDownloadInfo_Tvtv] param fail!!\n");
		return hError;
	}
	hError = SVC_EPGCONTEXT_GetContext(hAction, eEPG_PROCESS_TVTV, &pContext);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_GetDownloadInfo_Tvtv] SVC_EPGCONTEXT_GetContext fail!!\n");
		return hError;
	}
	if(pContext->eStatus == eEPG_CNXT_EMPTY)
	{
		HxLOG_Error("[xsvc_epgcontext_GetDownloadInfo_Tvtv] SVC_EPGCONTEXT_GetContext fail!!\n");
		return hError;
	}
	VK_SEM_Get(pContext->ulSemaphore);
	if(pContext->eStatus != eEPG_CNXT_USED || pContext->pCnxtData == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_GetDownloadInfo_Tvtv] context fail!!\n");
		hError = ERR_OK;
		goto EXIT_CNXTTVTV_FUNC;
	}
	pCnxtData = pContext->pCnxtData;
	HxSTD_memcpy(pDownloadInfo, &(pCnxtData->stDownloadInfo), sizeof(svcEpg_DownloadInfo_t));

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :

	VK_SEM_Release(pContext->ulSemaphore);

	return hError;
}

STATIC HERROR				xsvc_epgcontext_SetDownloadInfo_Tvtv(Handle_t hAction, svcEpg_DownloadInfo_t *pDownloadInfo)
{
	HERROR				hError = ERR_EPG_FAIL;
	svcEpg_Context_t 		*pContext = NULL;
	svcEpg_ContextData_t		*pCnxtData = NULL;

	if(pDownloadInfo == NULL || hAction == HANDLE_NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_SetDownloadInfo_Tvtv] param fail!!\n");
		return hError;
	}
	hError = SVC_EPGCONTEXT_GetContext(hAction, eEPG_PROCESS_TVTV, &pContext);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_SetDownloadInfo_Tvtv] SVC_EPGCONTEXT_GetContext fail!!\n");
		return hError;
	}
	if(pContext->eStatus == eEPG_CNXT_EMPTY)
	{
		HxLOG_Error("[xsvc_epgcontext_SetDownloadInfo_Tvtv] SVC_EPGCONTEXT_GetContext fail!!\n");
		return hError;
	}
	VK_SEM_Get(pContext->ulSemaphore);
	if(pContext->eStatus != eEPG_CNXT_USED || pContext->pCnxtData == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_SetDownloadInfo_Tvtv] context fail!!\n");
		hError = ERR_OK;
		goto EXIT_CNXTTVTV_FUNC;
	}

	pCnxtData = pContext->pCnxtData;
	HxSTD_memcpy(&(pCnxtData->stDownloadInfo), pDownloadInfo, sizeof(svcEpg_DownloadInfo_t));

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :
	VK_SEM_Release(pContext->ulSemaphore);

	return hError;
}

STATIC HERROR				xsvc_epgcontext_GetPostprocessInfo_Tvtv(Handle_t hAction, svcEpg_PostProcessInfo_t *pPostprocessInfo)
{
	HERROR				hError = ERR_EPG_FAIL;
	svcEpg_Context_t 		*pContext = NULL;
	svcEpg_ContextData_t		*pCnxtData = NULL;

	if(pPostprocessInfo == NULL || hAction == HANDLE_NULL)
	{
		HxLOG_Print("[xsvc_epgcontext_GetPostprocessInfo_Tvtv] param fail!!\n");
		return hError;
	}

	hError = SVC_EPGCONTEXT_GetContext(hAction, eEPG_PROCESS_TVTV, &pContext);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_GetPostprocessInfo_Tvtv] SVC_EPGCONTEXT_GetContext fail!!\n");
		return hError;
	}
	if(pContext->eStatus == eEPG_CNXT_EMPTY)
	{
		HxLOG_Error("[xsvc_epgcontext_GetPostprocessInfo_Tvtv] SVC_EPGCONTEXT_GetContext fail!!\n");
		return hError;
	}
	VK_SEM_Get(pContext->ulSemaphore);
	if(pContext->eStatus != eEPG_CNXT_USED || pContext->pCnxtData == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_GetPostprocessInfo_Tvtv] context fail!!\n");
		hError = ERR_OK;
		goto EXIT_CNXTTVTV_FUNC;
	}

	pCnxtData = pContext->pCnxtData;
	HxSTD_memcpy(pPostprocessInfo, &(pCnxtData->stPostprocessInfo), sizeof(svcEpg_PostProcessInfo_t));

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :
	VK_SEM_Release(pContext->ulSemaphore);

	return hError;
}

STATIC HERROR				xsvc_epgcontext_SetPostprocessInfo_Tvtv(Handle_t hAction, svcEpg_PostProcessInfo_t *pPostprocessInfo)
{
	HERROR				hError = ERR_EPG_FAIL;
	svcEpg_Context_t 		*pContext = NULL;
	svcEpg_ContextData_t		*pCnxtData = NULL;

	if(pPostprocessInfo == NULL || hAction == HANDLE_NULL)
	{
		HxLOG_Print("[xsvc_epgcontext_SetPostprocessInfo_Tvtv] param fail!!\n");
		return hError;
	}

	hError = SVC_EPGCONTEXT_GetContext(hAction, eEPG_PROCESS_TVTV, &pContext);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_SetPostprocessInfo_Tvtv] SVC_EPGCONTEXT_GetContext fail!!\n");
		return hError;
	}
	if(pContext->eStatus == eEPG_CNXT_EMPTY)
	{
		HxLOG_Error("[xsvc_epgcontext_SetPostprocessInfo_Tvtv] SVC_EPGCONTEXT_GetContext fail!!\n");
		return hError;
	}

	VK_SEM_Get(pContext->ulSemaphore);
	if(pContext->eStatus != eEPG_CNXT_USED || pContext->pCnxtData == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_SetPostprocessInfo_Tvtv] context fail!!\n");
		hError = ERR_OK;
		goto EXIT_CNXTTVTV_FUNC;
	}

	pCnxtData = pContext->pCnxtData;
	HxSTD_memcpy(&(pCnxtData->stPostprocessInfo), pPostprocessInfo, sizeof(svcEpg_PostProcessInfo_t));

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :

	VK_SEM_Release(pContext->ulSemaphore);

	return hError;
}

STATIC HERROR				xsvc_epgcontext_GetXMLParseInfo_Tvtv(Handle_t hAction, svcEpg_XmlParseInfo_t *pXMLParseInfo)
{
	HERROR				hError = ERR_EPG_FAIL;
	svcEpg_Context_t 		*pContext = NULL;
	svcEpg_ContextData_t		*pCnxtData = NULL;

	if(pXMLParseInfo == NULL || hAction == HANDLE_NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_GetPostprocessInfo_Tvtv] param fail!!\n");
		return hError;
	}

	hError = SVC_EPGCONTEXT_GetContext(hAction, eEPG_PROCESS_TVTV, &pContext);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_GetPostprocessInfo_Tvtv] SVC_EPGCONTEXT_GetContext fail!!\n");
		return hError;
	}
	if(pContext->eStatus == eEPG_CNXT_EMPTY)
	{
		HxLOG_Error("[xsvc_epgcontext_GetPostprocessInfo_Tvtv] SVC_EPGCONTEXT_GetContext fail!!\n");
		return hError;
	}

	VK_SEM_Get(pContext->ulSemaphore);
	if(pContext->eStatus != eEPG_CNXT_USED || pContext->pCnxtData == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_GetXMLParseInfo_Tvtv] context fail!!\n");
		hError = ERR_OK;
		goto EXIT_CNXTTVTV_FUNC;
	}

	pCnxtData = pContext->pCnxtData;
	HxSTD_memcpy(pXMLParseInfo, &(pCnxtData->stXMLParseInfo), sizeof(svcEpg_XmlParseInfo_t));

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :
	VK_SEM_Release(pContext->ulSemaphore);

	return hError;
}

#if defined(CONFIG_TVTVEPG_LIVEDOWNLOAD)
STATIC HERROR				xsvc_epgcontext_SetXMLParseInfo_Tvtv(Handle_t hAction, svcEpg_XmlParseInfo_t *pXMLParseInfo)
{
	HERROR				hError = ERR_EPG_FAIL;
	svcEpg_Context_t 		*pContext = NULL;
	svcEpg_ContextData_t		*pCnxtData = NULL;

	if(pXMLParseInfo == NULL || hAction == HANDLE_NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_SetPostprocessInfo_Tvtv] param fail!!\n");
		return hError;
	}

	hError = SVC_EPGCONTEXT_GetContext(hAction, eEPG_PROCESS_TVTV, &pContext);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_SetPostprocessInfo_Tvtv] SVC_EPGCONTEXT_GetContext fail!!\n");
		return hError;
	}
	if(pContext->eStatus == eEPG_CNXT_EMPTY)
	{
		HxLOG_Error("[xsvc_epgcontext_SetPostprocessInfo_Tvtv] SVC_EPGCONTEXT_GetContext fail!!\n");
		return hError;
	}

	VK_SEM_Get(pContext->ulSemaphore);
	if(pContext->eStatus != eEPG_CNXT_USED || pContext->pCnxtData == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_SetXMLParseInfo_Tvtv] context fail!!\n");
		hError = ERR_OK;
		goto EXIT_CNXTTVTV_FUNC;
	}

	pCnxtData = pContext->pCnxtData;
	HxSTD_memcpy(&(pCnxtData->stXMLParseInfo), pXMLParseInfo, sizeof(svcEpg_XmlParseInfo_t));

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :
	VK_SEM_Release(pContext->ulSemaphore);

	return hError;
}
#endif

STATIC HERROR				xsvc_epgcontext_GetContextInfo_Tvtv(Handle_t hAction, HUINT32 ulIndex, Handle_t *phSvcHandle, HUINT32 *pulOrgDeliType, HUINT8 *pucAntId, EPG_SpecType_e *peSpec, svcEpg_Filter_t *pEpgContextFilterInfo)
{
	HERROR				hError = ERR_EPG_FAIL;
	svcEpg_Context_t 		*pContext = NULL;
	svcEpg_ContextData_t		*pCnxtData = NULL;

	if(phSvcHandle == NULL || pEpgContextFilterInfo == NULL || hAction == HANDLE_NULL)
	{
		HxLOG_Error("[local_cnxtpf_GetContextInfo] param fail!!\n");
		return hError;
	}

	hError = SVC_EPGCONTEXT_GetContext(hAction, eEPG_PROCESS_TVTV, &pContext);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[local_cnxtpf_GetContextInfo] SVC_EPGCONTEXT_GetContext fail!!\n");
		return hError;
	}
	if(pContext->eStatus == eEPG_CNXT_EMPTY)
	{
		HxLOG_Error("[local_cnxtpf_GetContextInfo] SVC_EPGCONTEXT_GetContext fail!!\n");
		return hError;
	}

	VK_SEM_Get(pContext->ulSemaphore);
	if(pContext->eStatus != eEPG_CNXT_USED || pContext->pCnxtData == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_GetContextInfo_Tvtv] context fail!!\n");
		hError = ERR_OK;
		goto EXIT_CNXTTVTV_FUNC;
	}

	pCnxtData = pContext->pCnxtData;
	*phSvcHandle = pCnxtData->hService;
	*pulOrgDeliType = pCnxtData->ulOrgDeliveryType;
	*peSpec = pCnxtData->eSpec;
	*pucAntId = pCnxtData->ucAntId;
	HxSTD_memcpy(pEpgContextFilterInfo, &(pCnxtData->astFilterTable[ulIndex]), sizeof(svcEpg_Filter_t));

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :
	VK_SEM_Release(pContext->ulSemaphore);

	return hError;
}

STATIC HERROR				xsvc_epgcontext_SetContextFilterInfo_Tvtv(Handle_t hAction, HUINT32 ulIndex, svcEpg_Filter_t *pEpgContextFilterInfo)
{
	HERROR				hError = ERR_EPG_FAIL;
	svcEpg_Context_t 		*pContext = NULL;
	svcEpg_ContextData_t		*pCnxtData = NULL;

	if(pEpgContextFilterInfo == NULL || hAction == HANDLE_NULL)
	{
		HxLOG_Print("[local_cnxtpf_SetContextFilterInfo] param fail!!\n");
		return hError;
	}

	hError = SVC_EPGCONTEXT_GetContext(hAction, eEPG_PROCESS_TVTV, &pContext);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[local_cnxtpf_SetContextFilterInfo] SVC_EPGCONTEXT_GetContext fail!!\n");
		return hError;
	}
	if(pContext->eStatus == eEPG_CNXT_EMPTY)
	{
		HxLOG_Error("[local_cnxtpf_SetContextFilterInfo] SVC_EPGCONTEXT_GetContext fail!!\n");
		return hError;
	}

	VK_SEM_Get(pContext->ulSemaphore);
	if(pContext->eStatus != eEPG_CNXT_USED || pContext->pCnxtData == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_SetContextFilterInfo_Tvtv] context fail!!\n");
		hError = ERR_OK;
		goto EXIT_CNXTTVTV_FUNC;
	}

	pCnxtData = pContext->pCnxtData;
	if(pCnxtData->astFilterTable[ulIndex].usFilterId != pEpgContextFilterInfo->usFilterId)
	{
		HxLOG_Error("[xsvc_epgcontext_SetContextFilterInfo_Tvtv] context fail!!\n");
		hError = ERR_OK;
		goto EXIT_CNXTTVTV_FUNC;
	}
	HxSTD_memcpy(&(pCnxtData->astFilterTable[ulIndex]), pEpgContextFilterInfo, sizeof(svcEpg_Filter_t));

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :
	VK_SEM_Release(pContext->ulSemaphore);

	return hError;
}

#define ____RECEIVE_CALLBACK_FUNC____
STATIC void				xsvc_epgcontext_PostPAT_Tvtv(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	HUINT8					ucHead = ACT_HEAD;
	HERROR					hError = ERR_EPG_FAIL;
	HUINT16					usTsUniqueId = 0;
	HUINT16					usFilterId = 0;
	HUINT16					usTableId = 0;
	HUINT16					usResult = 0;
	PalSef_RawData_t				*pSiRawData = NULL;
	HUINT32					ulMessage = 0;
	HUINT16					usIndex = 0;


	/**********************************************************************************
	  *
	  * 1. get filtering result info
	  *
	  **********************************************************************************/
	HxLOG_Info("[xsvc_epgcontext_PostPAT_Tvtv] +!!\n");
	INT_EPGMACRO_GET_FILTERID_N_TSUNIQID(ulFilterAndUniqId, usFilterId, usTsUniqueId);
	INT_EPGMACRO_GET_TABLEID_N_RESULT(ulTableIdAndResult, usTableId, usResult);

	NOT_USED_PARAM(usFilterId);
	NOT_USED_PARAM(usTableId);

	if(usResult != (HUINT32)eSiFilterResult_Complete )
	{
		HxLOG_Info("[xsvc_epgcontext_PostPAT_Tvtv] timeout!!\n");

		// eSiFilterResult_Timeout : context에서 처리
		HxLOG_Print("[xsvc_epgcontext_PostPAT_Tvtv] eSiFilterResult_Complete fail!!\n");
		hError = SVC_EPG_MakeEpgMessage(EPG_CMD_PAT_RECEIVED,  eEPG_PROCESS_TVTV, &ulMessage);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Print("[xsvc_epgcontext_PostPAT_Tvtv] MW_EPGMAIN_MakeEpgMessage fail!!\n");
		}
		hError = SVC_EPG_SendMessage(ulMessage, hAction, ucHead, usResult);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Print("[xsvc_epgcontext_PostPAT_Tvtv] SendMessage fail!!\n");
		}
		goto  EXIT_CNXTTVTV_FUNC;
	}

	pSiRawData = (PalSef_RawData_t*)ulParam;
	if(pSiRawData == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_PostPAT_Tvtv] pSiRawData null!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	hError = SVC_EPGRAWMGR_FreeAllSection (usTsUniqueId, eSI_TableType_PAT);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("[xsvc_epgcontext_PostPAT_Tvtv] SVC_EPGRAWMGR_FreeAllSection fail!!\n");
	}


	for ( usIndex = 0; usIndex < pSiRawData->num_sec && usIndex < PAL_SEF_MAX_SI_RAW_SECTION_NUM; usIndex++ )
	{
		if(pSiRawData->buf[usIndex])
		{
			HUINT8		*pucTemp = NULL;
			HUINT8		*pucNewSec = NULL;
			HUINT16		usSecLen = 0;

			pucTemp = pSiRawData->buf[usIndex];
			usSecLen = (HUINT16)get12bit(pucTemp + 1);
			pucNewSec = (HUINT8 *)OxSI_Malloc((HUINT32)usSecLen + 3);
			if ( pucNewSec == NULL )
			{
				HxLOG_Error("[xsvc_epgcontext_PostPAT_Tvtv] OxEPG_Malloc fail!!\n");
				break;
			}
			HxSTD_memcpy(pucNewSec, pSiRawData->buf[usIndex], (HUINT32)usSecLen + 3);
			hError = SVC_EPGRAWMGR_AddRawSection (usTsUniqueId, eSI_TableType_PAT, pucNewSec);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_PostPAT_Tvtv] SVC_EPGRAWMGR_AddRawSection fail!!\n");
				OxSI_Free(pucNewSec);
			}
		}
	}

	/**********************************************************************************
	  *
	  * 2. set filtering cycle base info
	  *
	  **********************************************************************************/
	hError = SVC_EPG_MakeEpgMessage(EPG_CMD_PAT_RECEIVED,  eEPG_PROCESS_TVTV, &ulMessage);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_PostPAT_Tvtv] MW_EPGMAIN_MakeEpgMessage fail!!\n");
	}
	hError = SVC_EPG_SendMessage(ulMessage, hAction, ucHead, usResult);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_PostPAT_Tvtv] SendMessage fail!!\n");
	}
	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :

	return;

}

STATIC void				xsvc_epgcontext_FreePAT_Tvtv(HUINT32 ulTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2)
{
	HERROR					hError = ERR_EPG_FAIL;

	hError = SVC_EPGRAWMGR_FreeAllSection ((HUINT16)ulTsUniqId, eSI_TableType_PAT);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_FreePAT_Tvtv] SVC_EPGRAWMGR_FreeAllSection fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :

	return;
}

STATIC void				xsvc_epgcontext_PostPMT_Tvtv(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	HUINT8					ucHead = ACT_HEAD;
	HERROR					hError = ERR_EPG_FAIL;
	HUINT16					usTsUniqueId = 0;
	HUINT16					usFilterId = 0;
	HUINT16					usTableId = 0;
	HUINT16					usResult = 0;
	PalSef_RawData_t				*pSiRawData = NULL;
	HUINT32					ulMessage = 0;
	HUINT16					usIndex = 0;


	/**********************************************************************************
	  *
	  * 1. get filtering result info
	  *
	  **********************************************************************************/
	HxLOG_Info("[xsvc_epgcontext_PostPMT_Tvtv] +!!\n");
	INT_EPGMACRO_GET_FILTERID_N_TSUNIQID(ulFilterAndUniqId, usFilterId, usTsUniqueId);
	INT_EPGMACRO_GET_TABLEID_N_RESULT(ulTableIdAndResult, usTableId, usResult);
	ucHead = ACT_HEAD;

	NOT_USED_PARAM(usFilterId);
	NOT_USED_PARAM(usTableId);

	if(usResult != (HUINT32)eSiFilterResult_Complete )
	{
		HxLOG_Info("[xsvc_epgcontext_PostPMT_Tvtv] timeout!!\n");

		// eSiFilterResult_Timeout : context에서 처리
		HxLOG_Print("[xsvc_epgcontext_PostPMT_Tvtv] eSiFilterResult_Complete fail!!\n");
		hError = SVC_EPG_MakeEpgMessage(EPG_CMD_PMT_RECEIVED,  eEPG_PROCESS_TVTV, &ulMessage);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_PostPMT_Tvtv] MW_EPGMAIN_MakeEpgMessage fail!!\n");
		}
		hError = SVC_EPG_SendMessage(ulMessage, hAction, ucHead, usResult);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_PostPMT_Tvtv] SendMessage fail!!\n");
		}
		goto  EXIT_CNXTTVTV_FUNC;
	}
	pSiRawData = (PalSef_RawData_t*)ulParam;
	if(pSiRawData == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_PostPMT_Tvtv] pSiRawData null!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	hError = SVC_EPGRAWMGR_FreeAllSection (usTsUniqueId, eSI_TableType_PMT);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_PostPMT_Tvtv] SVC_EPGRAWMGR_FreeAllSection fail!!\n");
	}

	for ( usIndex = 0; usIndex < pSiRawData->num_sec && usIndex < SECTION_NUM_MAX; usIndex++ )
	{
		if(pSiRawData->buf[usIndex])
		{
			HUINT8		*pucTemp = NULL;
			HUINT8		*pucNewSec = NULL;
			HUINT16		usSecLen = 0;

			pucTemp = pSiRawData->buf[usIndex];
			usSecLen = (HUINT16)get12bit(pucTemp + 1);
			pucNewSec = (HUINT8 *)OxSI_Malloc((HUINT32)usSecLen + 3);
			if ( pucNewSec == NULL )
			{
				HxLOG_Print("[xsvc_epgcontext_PostPMT_Tvtv] OxEPG_Malloc fail!!\n");
				break;
			}
			HxSTD_memcpy(pucNewSec, pSiRawData->buf[usIndex], (HUINT32)usSecLen + 3);

			hError = SVC_EPGRAWMGR_AddRawSection (usTsUniqueId, eSI_TableType_PMT, pucNewSec);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_PostPMT_Tvtv] SVC_EPGRAWMGR_AddRawSection fail!!\n");
				OxSI_Free(pucNewSec);
			}
		}
	}

	/**********************************************************************************
	  *
	  * 2. set filtering cycle base info
	  *
	  **********************************************************************************/
	hError = SVC_EPG_MakeEpgMessage(EPG_CMD_PMT_RECEIVED,  eEPG_PROCESS_TVTV, &ulMessage);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[local_cnxtpf_CheckEitVersion] MW_EPGMAIN_MakeEpgMessage fail!!\n");
	}
	hError = SVC_EPG_SendMessage(ulMessage, hAction, ucHead, usResult);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_PostPMT_Tvtv] SendMessage fail!!\n");
	}
	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :

	return;

}

STATIC void				xsvc_epgcontext_FreePMT_Tvtv(HUINT32 ulTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2)
{
	HERROR					hError = ERR_EPG_FAIL;

	hError = SVC_EPGRAWMGR_FreeAllSection ((HUINT16)ulTsUniqId, eSI_TableType_PMT);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_FreePMT_Tvtv] SVC_EPGRAWMGR_FreeAllSection fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :

	return;
}

STATIC void				xsvc_epgcontext_PostDsi_Tvtv(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	HUINT8					ucHead = ACT_HEAD;
	HERROR					hError = ERR_EPG_FAIL;
	HUINT16					usTsUniqueId = 0;
	HUINT16					usFilterId = 0;
	HUINT16					usTableId = 0;
	HUINT16					usResult = 0;
	PalSef_RawData_t				*pSiRawData = NULL;
	svcEpg_Filter_t				stCnxtFilterInfo;
	Handle_t					hSvcHandle = HANDLE_NULL;
	HUINT32					ulMessage = 0;
	HUINT32					ulCnxtFilterSlot = 0;
	HUINT32					ulOrgDeliType = 0;
	HUINT8					ucAntId = 0;
	EPG_SpecType_e				eSpec;
	HUINT8					*pucSection = NULL;
	HUINT32					ulSystemTime = 0;
	svcEpg_Msg_t					stMsg;
	HUINT16					sSectionLen = 0;
	HUINT32					ulCrc = 0;
	HUINT8					ucVerNum = 0;


	/**********************************************************************************
	  *
	  * 1. get filtering result info
	  *
	  **********************************************************************************/
	INT_EPGMACRO_GET_FILTERID_N_TSUNIQID(ulFilterAndUniqId, usFilterId, usTsUniqueId);
	INT_EPGMACRO_GET_TABLEID_N_RESULT(ulTableIdAndResult, usTableId, usResult);
	NOT_USED_PARAM(usFilterId);
	NOT_USED_PARAM(usTableId);
	ucHead = ACT_HEAD;
	ulCnxtFilterSlot = EPGCNXT_FITER_SLOT_TVTV_DSI;

	hError = xsvc_epgcontext_GetContextInfo_Tvtv(	hAction, EPGCNXT_FITER_SLOT_TVTV_DSI,
											&hSvcHandle,
											&ulOrgDeliType,
											&ucAntId,
											&eSpec,
											&stCnxtFilterInfo);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_PostDsi_Tvtv] SendMessage fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}
	if(stCnxtFilterInfo.bComplete)
	{
		HxLOG_Info("[xsvc_epgcontext_PostDsi_Tvtv] bComplete!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	if(usResult != (HUINT32)eSiFilterResult_Complete )
	{
		HxLOG_Print("[xsvc_epgcontext_PostDsi_Tvtv] timeout!!\n");
		stCnxtFilterInfo.bTimeOut = TRUE;
		hError = xsvc_epgcontext_SetContextFilterInfo_Tvtv(hAction, EPGCNXT_FITER_SLOT_TVTV_DSI, &stCnxtFilterInfo);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_PostDsi_Tvtv] local_cnxtsch_SetContextFilterInfo fail!!\n");
			goto  EXIT_CNXTTVTV_FUNC;
		}

		// eSiFilterResult_Timeout : context에서 처리
		HxLOG_Print("[xsvc_epgcontext_PostDsi_Tvtv] eSiFilterResult_Complete fail!!\n");
		hError = SVC_EPG_MakeEpgMessage(EPG_CMD_DSI_RECEIVED,  eEPG_PROCESS_TVTV, &ulMessage);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_PostDsi_Tvtv] MW_EPGMAIN_MakeEpgMessage fail!!\n");
		}
		hError = SVC_EPG_SendMessage(ulMessage, hAction, ucHead, usResult);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_PostDsi_Tvtv] SendMessage fail!!\n");
		}
		goto  EXIT_CNXTTVTV_FUNC;
	}

	pSiRawData = (PalSef_RawData_t*)ulParam;
	if(pSiRawData == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_PostDsi_Tvtv] pSiRawData null!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}
	pucSection = pSiRawData->buf[0];
	if(pucSection == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_PostDsi_Tvtv] pucSection null!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	hError = PAL_DSMCC_ParseDsmccSectionInfo(pucSection, &sSectionLen, &ulCrc, &ucVerNum);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_PostDsi_Tvtv] PAL_DSMCC_ParseDsmccSectionInfo fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	if(stCnxtFilterInfo.stBaseInfo.ulCrc == 0)
	{
		stCnxtFilterInfo.ulBaseTime = VK_TIMER_GetSystemTick()/1000;
		stCnxtFilterInfo.stBaseInfo.ulCrc = ulCrc;
		stCnxtFilterInfo.stBaseInfo.ucVersion = ucVerNum;

		hError = xsvc_epgcontext_SetContextFilterInfo_Tvtv(hAction, ulCnxtFilterSlot, &stCnxtFilterInfo);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_PostDsi_Tvtv] local_cnxtpf_SetContextFilterInfo fail!!\n");
			HxSTD_memset(&(stCnxtFilterInfo.stBaseInfo), 0x00, sizeof(svcEpg_ServiceInfo_t));
			goto  EXIT_CNXTTVTV_FUNC;
		}
		HxLOG_Info("\n\n[xsvc_epgcontext_PostDsi_Tvtv] start(%d)\n\n", ulSystemTime);
	}

	/**********************************************************************************
	  *
	  * 2. send message
	  *
	  **********************************************************************************/
	stMsg.ulMessage = eEPG_MSG_DSI_TABLE;
	stMsg.ulParam1 = (HUINT32)hAction;
	stMsg.ulParam2 = (HUINT32)usTsUniqueId;
	stMsg.ulParam3 = (HUINT32)pucSection;

	pSiRawData->buf[0] = NULL;
	pSiRawData->num_sec = 0;

	hError = UTIL_TASK_SendMessage(stCnxtFilterInfo.ucTaskSlot, &stMsg);
	if(hError != ERR_TASKUTIL_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_PostDsi_Tvtv] SendMessage fail!!\n");
	}
	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :

	return;

}

STATIC void				xsvc_epgcontext_PostDdb_Tvtv(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	HUINT8					ucHead = 0;
	HERROR					hError = ERR_EPG_FAIL;
	HUINT16					usTableId = 0;
	HUINT16					usTsUniqueId = 0;
	HUINT16					usFilterId = 0;
	HUINT16					usResult = 0;
	PalSef_RawData_t				*pSiRawData = NULL;
	svcEpg_Filter_t			stCnxtFilterInfo;
	svcEpg_Msg_t				stMsg;
	HUINT32					ulCnxtFilterSlot = 0;
	Handle_t				hSvcHandle = HANDLE_NULL;
	UNIXTIME				ulSystemTime = 0;
	HUINT32					ulMessage = 0;
	svcEpg_RawBuffer_t		*pCnxtBuffer = NULL;
	svcEpg_RawBuffer_t		*pBuffer = NULL;
	HUINT32					ulOrgDeliType = 0;
	HUINT8					ucAntId = 0;
	EPG_SpecType_e			eSpec;
	HUINT32					ulTransactionId = 0;
	HUINT32					ulDsiCrc = 0;
	HUINT32					ulRegisteredTime = 0;
	HBOOL					bFirstSection = FALSE;
	HUINT32					ulIndex = 0;

	/**********************************************************************************
	  *
	  * 1. get filtering result info
	  *
	  **********************************************************************************/
	INT_EPGMACRO_GET_FILTERID_N_TSUNIQID(ulFilterAndUniqId, usFilterId, usTsUniqueId);
	INT_EPGMACRO_GET_TABLEID_N_RESULT(ulTableIdAndResult, usTableId, usResult);
	NOT_USED_PARAM(usFilterId);
	NOT_USED_PARAM(usTableId);
	NOT_USED_PARAM(usTsUniqueId);
	ucHead = ACT_HEAD;
	ulCnxtFilterSlot = EPGCNXT_FITER_SLOT_TVTV_DDB;

	hError = xsvc_epgcontext_GetContextInfo_Tvtv(	hAction, EPGCNXT_FITER_SLOT_TVTV_DDB,
											&hSvcHandle,
											&ulOrgDeliType,
											&ucAntId,
											&eSpec,
											&stCnxtFilterInfo);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_PostDdb_Tvtv] xsvc_epgcontext_GetContextInfo_Tvtv fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	if(stCnxtFilterInfo.bComplete)
	{
		HxLOG_Print("[xsvc_epgcontext_PostDdb_Tvtv] bComplete!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	if(usResult != (HUINT32)eSiFilterResult_Complete )
	{
		stCnxtFilterInfo.bTimeOut = TRUE;
		hError = xsvc_epgcontext_SetContextFilterInfo_Tvtv(hAction, ulCnxtFilterSlot, &stCnxtFilterInfo);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Print("[xsvc_epgcontext_PostDdb_Tvtv] xsvc_epgcontext_SetContextFilterInfo_Tvtv fail!!\n");
			goto  EXIT_CNXTTVTV_FUNC;
		}

		// eSiFilterResult_Timeout : context에서 처리
		HxLOG_Info("[xsvc_epgcontext_PostDdb_Tvtv] eSiFilterResult_Complete fail!!\n");
		hError = SVC_EPG_MakeEpgMessage(EPG_CMD_DDB_RECEIVED,  eEPG_PROCESS_TVTV, &ulMessage);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_PostDdb_Tvtv] MW_EPGMAIN_MakeEpgMessage fail!!\n");
		}
		hError = SVC_EPG_SendMessage(ulMessage, hAction, ucHead, usResult);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_PostDdb_Tvtv] SendMessage fail!!\n");
		}
		goto  EXIT_CNXTTVTV_FUNC;
	}

	pSiRawData = (PalSef_RawData_t*)ulParam;
	if(pSiRawData == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_PostDdb_Tvtv] pSiRawData fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}
	if(pSiRawData->buf[0]== NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_PostDdb_Tvtv] pSiRawData->buf[0]== NULL !!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	hError = PAL_DSMCC_GetDsmccSessionInfo(&ulTransactionId, &ulDsiCrc, &ulRegisteredTime);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_PostDdb_Tvtv] PAL_DSMCC_GetDsmccSessionInfo fail !!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}
	if(ulDsiCrc == 0)
	{
		HxLOG_Info("[xsvc_epgcontext_PostDdb_Tvtv] not yet setting dsi !!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	hError = SVC_EPGCONTEXT_GetDsmccSectionBuffer(ulCnxtFilterSlot, &pCnxtBuffer);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_PostDdb_Tvtv] local_cnxttvtv_GetSectionBuffer fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}
	if(ulTransactionId != stCnxtFilterInfo.ulTransactionId)
	{
		bFirstSection = TRUE;
		for(ulIndex = 0; ulIndex < pCnxtBuffer->usSectionCount; ulIndex++)
		{
			if(pCnxtBuffer->paucSection[ulIndex])
			{
				OxSI_Free(pCnxtBuffer->paucSection[ulIndex]);
				pCnxtBuffer->paucSection[ulIndex] = NULL;
			}
		}
		pCnxtBuffer->usSectionCount = 0;
	}

	if(bFirstSection)
	{
		stCnxtFilterInfo.ulBaseTime = VK_TIMER_GetSystemTick()/1000;
		stCnxtFilterInfo.ulTransactionId = ulTransactionId;

		hError = xsvc_epgcontext_SetContextFilterInfo_Tvtv(hAction, ulCnxtFilterSlot, &stCnxtFilterInfo);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_PostDdb_Tvtv] local_cnxtpf_SetContextFilterInfo fail!!\n");
			HxSTD_memset(&(stCnxtFilterInfo.stBaseInfo), 0x00, sizeof(svcEpg_ServiceInfo_t));
			goto  EXIT_CNXTTVTV_FUNC;
		}
		HxLOG_Info("\n\n[xsvc_epgcontext_PostDdb_Tvtv] start(%d)\n\n", ulSystemTime);
	}

	if(bFirstSection == FALSE && pCnxtBuffer->usSectionCount < EPG_DSMCC_SECTION_BUFFER_MAX_NUM - 1)
	{
		pCnxtBuffer->paucSection[pCnxtBuffer->usSectionCount] = pSiRawData->buf[0];
		pCnxtBuffer->usSectionCount++;
	}
	else if(bFirstSection == TRUE || pCnxtBuffer->usSectionCount == EPG_DSMCC_SECTION_BUFFER_MAX_NUM - 1)
	{
		pCnxtBuffer->paucSection[pCnxtBuffer->usSectionCount] = pSiRawData->buf[0];
		pCnxtBuffer->usSectionCount++;

		pBuffer = (svcEpg_RawBuffer_t*)OxEPG_Malloc(sizeof(svcEpg_RawBuffer_t));
		if(pBuffer == NULL)
		{
			HxLOG_Print("[xsvc_epgcontext_PostDdb_Tvtv] OxEPG_Malloc fail!!\n");
			goto  EXIT_CNXTTVTV_FUNC;
		}
		HxSTD_memcpy(pBuffer, pCnxtBuffer, sizeof(svcEpg_RawBuffer_t));
		HxSTD_memset(pCnxtBuffer, 0x00, sizeof(svcEpg_RawBuffer_t));
		/**********************************************************************************
		  *
		  * 3. send message
		  *
		  **********************************************************************************/
		stMsg.ulMessage = eEPG_MSG_DDB_TABLE;
		stMsg.ulParam1 = (HUINT32)hAction;
		stMsg.ulParam2 = (HUINT32)ulTransactionId;
		stMsg.ulParam3 = (HUINT32)pBuffer;

		hError = UTIL_TASK_SendMessage(stCnxtFilterInfo.ucTaskSlot, &stMsg);
		if(hError != ERR_TASKUTIL_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_PostDdb_Tvtv] SendMessage fail!!\n");
		}
	}
	pSiRawData->buf[0] = NULL;
	pSiRawData->num_sec = 0;

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :

	return;

}



#define ____PROCESS_CALLBACK_FUNC____


#define ____PROCESS_DSMCC_FUNC____
// task util proc
STATIC HERROR				xsvc_epgcontext_ProcessDsi_Tvtv(HUINT32 ulParam1, HUINT32 ulParam2)
{
	HERROR						hError = ERR_EPG_FAIL;
	Handle_t						hAction = HANDLE_NULL;
	Handle_t						hSvcHandle = HANDLE_NULL;
	HUINT32						ulEpgMessage = 0;
	HUINT8						ucHead = 0;
	HUINT16						usMsgTsUniqId = 0;
	svcEpg_Msg_t						*pstMsg = NULL;
	HUINT8						*pucRawSection = NULL;
	HUINT32						ulOrgDeliType = 0;
	HUINT8						ucAntId = 0;
	EPG_SpecType_e				eSpec;
	svcEpg_Filter_t					stCnxtFilterInfo = {0, };
	HUINT32						ulCnxtFilterSlot = 0;
	HUINT16						usDsmccMessageId = 0;
	HUINT8						ucIsComplete = 0;
	PalDsmcc_File_t					*pDsmccFile = NULL;
	HUINT32						ulDownloadSize = 0;
	svcEpg_DownloadInfo_t 		stDownloadInfo = {0, };
	HUINT32						ulTransactionId = 0;
	HUINT32						ulDsiCrc = 0;
	HUINT32						ulRegisteredTime = 0;
	HUINT32						ulMessage = 0;
	HUINT32						usResult = 0;
	HUINT32						ulFileSize = 0;
	HUINT32						ulWriteSize = 0;
	HUINT32						ulReadSize = 0;
	HUINT8						szFileName[256] = {0, };
	Handle_t						hFileHandle = (Handle_t)NULL;
	xsvcEpg_TvtvDsiTable_t				stDsiTable = {0, };

#if	defined(CONFIG_TVTVLOG)
	SvcEpg_TvtvLog_e					eLog = eTvtvLog_Max;
	HUINT32						ulOldCrc = 0;
	HUINT32						ulOldTransactionId = 0;
	HUINT32						ulOldComplete = 0;
#endif

	pstMsg = (svcEpg_Msg_t*)ulParam1;
	if(pstMsg == NULL)
	{
		HxLOG_Print("[xsvc_epgcontext_ProcessDsi_Tvtv] param fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	/**********************************************************************************
	  *
	  * 1. get param info
	  *
	  **********************************************************************************/
	ulEpgMessage = pstMsg->ulMessage;
	hAction = (Handle_t)pstMsg->ulParam1;
	usMsgTsUniqId= (HUINT16)pstMsg->ulParam2;
	pucRawSection = (HUINT8*)pstMsg->ulParam3;
	NOT_USED_PARAM(ulEpgMessage);
	NOT_USED_PARAM(usMsgTsUniqId);
	if(pucRawSection == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_ProcessDsi_Tvtv] pucRawSection fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}


	ucHead = ACT_HEAD;
	ulCnxtFilterSlot = EPGCNXT_FITER_SLOT_TVTV_DSI;

	/**********************************************************************************
	  *
	  * 2. set filtering cycle base info
	  *
	  **********************************************************************************/
	hError = xsvc_epgcontext_GetContextInfo_Tvtv(	hAction,
											ulCnxtFilterSlot,
											&hSvcHandle,
											&ulOrgDeliType,
											&ucAntId,
											&eSpec,
											&stCnxtFilterInfo);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_ProcessDsi_Tvtv] xsvc_epgcontext_GetContextInfo_Tvtv fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	hError = PAL_DSMCC_ParseDsmccForDsiNDii(pucRawSection, &usDsmccMessageId);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_ProcessDsi_Tvtv] PAL_DSMCC_ParseDsmccForDsiNDii fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	if(usDsmccMessageId == DSI_MESSAGE_ID)
	{
		hError = PAL_DSMCC_GetDsmccSessionInfo(&ulTransactionId, &ulDsiCrc, &ulRegisteredTime);
		if(hError != ERR_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_ProcessDsi_Tvtv] PAL_DSMCC_GetDsmccSessionInfo fail !!\n");
			goto  EXIT_CNXTTVTV_FUNC;
		}
		HxLOG_Info("[xsvc_epgcontext_ProcessDsi_Tvtv] PAL_DSMCC_GetDsmccSessionInfo OK!!\n");

		/** if first dsi section,  compare with saved dsi transaction id */
		if(stCnxtFilterInfo.ulTransactionId == 0)
		{
			snprintf ((char *)szFileName, 256, "%s%s", EPG_FS_TVTV_DATA_PATH, TVTV_FILENAME_DSI_TABLE);
			hError = xsvc_epgcontext_FileLength_Tvtv(szFileName, &ulFileSize);
			if(hError == ERR_OK && ulFileSize != 0)
			{
				HxLOG_Info("[xsvc_epgcontext_ProcessDsi_Tvtv] DSI transaction id compare!!\n");
				hError = xsvc_epgcontext_Fopen_Tvtv(szFileName, &hFileHandle);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Error("[xsvc_epgcontext_ProcessDsi_Tvtv] fopen fail!!\n");
				}

				hError = xsvc_epgcontext_Fread_Tvtv(&stDsiTable, sizeof(xsvcEpg_TvtvDsiTable_t), hFileHandle, ePAL_PB_SEEKPOSITION_START, &ulReadSize);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Error("[xsvc_epgcontext_ProcessDsi_Tvtv] fread fail!!\n");
				}
#if				defined(CONFIG_TVTVLOG)
				ulOldCrc = stDsiTable.ulDsiCrc;
				ulOldComplete = stDsiTable.ulComplete;
				ulOldTransactionId = stDsiTable.ulDsiTransactioinId;
#endif
				HxLOG_Info("[xsvc_epgcontext_ProcessDsi_Tvtv] SECTION INFO : ulTransactionId(%x)ulDsiCrc(%x)!!\n",
																		ulTransactionId,
																		ulDsiCrc);

				HxLOG_Info("[xsvc_epgcontext_ProcessDsi_Tvtv] FILE INFO : ulTableVersion(%x)ulComplete(%x)ulDsiTransactioinId(%x)ulDsiCrc(%x)!!\n",
																		stDsiTable.ulTableVersion,
																		stDsiTable.ulComplete,
																		stDsiTable.ulDsiTransactioinId,
																		stDsiTable.ulDsiCrc);

				if(	stDsiTable.ulTableVersion == TVTV_DSI_TABLE_CURRENT_VERSION
					&& stDsiTable.ulComplete == TVTV_DSI_COMPLETE_MASK
					&& stDsiTable.ulDsiCrc == ulDsiCrc
					&& stDsiTable.ulDsiTransactioinId == ulTransactionId)
				{
#if					defined(CONFIG_TVTVLOG)
					eLog = eTvtvLog_DL_SkipByHddData;
#endif
					hError = SVC_EPG_MakeEpgMessage(EPG_CMD_STOP,  eEPG_PROCESS_TVTV, &ulMessage);
					if(hError != ERR_EPG_OK)
					{
						HxLOG_Print("[xsvc_epgcontext_ProcessDsi_Tvtv] MW_EPGMAIN_MakeEpgMessage fail!!\n");
					}
					hError = SVC_EPG_SendMessage(ulMessage, hAction, ucHead, usResult);
					if(hError != ERR_EPG_OK)
					{
						HxLOG_Print("[xsvc_epgcontext_ProcessDsi_Tvtv] SendMessage fail!!\n");
					}

					hError = xsvc_epgcontext_Fclose_Tvtv(hFileHandle);
					if(hError != ERR_EPG_OK)
					{
						HxLOG_Error("[xsvc_epgcontext_ProcessDsi_Tvtv] fclose fail!!\n");
					}
					goto  EXIT_CNXTTVTV_FUNC;
				}
				hError = xsvc_epgcontext_Fclose_Tvtv(hFileHandle);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Error("[xsvc_epgcontext_ProcessDsi_Tvtv] fclose fail!!\n");
				}

			}
		}

		HxLOG_Info("[xsvc_epgcontext_ProcessDsi_Tvtv] stCnxtFilterInfo.ulTransactionId(%x)ulTransactionId(%x)!!\n", stCnxtFilterInfo.ulTransactionId, ulTransactionId);
		HxLOG_Info("[xsvc_epgcontext_ProcessDsi_Tvtv] stCnxtFilterInfo.ulDsiCrc(%x)ulDsiCrc(%x)!!\n", stCnxtFilterInfo.ulDsiCrc, ulDsiCrc);
		if(stCnxtFilterInfo.ulTransactionId == ulTransactionId && stCnxtFilterInfo.ulDsiCrc == ulDsiCrc)
		{
			HxLOG_Print("[xsvc_epgcontext_ProcessDsi_Tvtv]ulTransactionId && ulDsiCrc same !!\n");
			goto  EXIT_CNXTTVTV_FUNC;
		}

		/** if version change, filter restart */
		if(stCnxtFilterInfo.ulTransactionId != 0)
		{
			HxLOG_Info("[xsvc_epgcontext_ProcessDsi_Tvtv] EPG_CMD_DSMCC_CHANGE!!\n");
			hError = SVC_EPG_MakeEpgMessage(EPG_CMD_DSMCC_CHANGE,  eEPG_PROCESS_TVTV, &ulMessage);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_ProcessDsi_Tvtv] MW_EPGMAIN_MakeEpgMessage fail!!\n");
			}
			hError = SVC_EPG_SendMessage(ulMessage, hAction, ucHead, usResult);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_ProcessDsi_Tvtv] SendMessage fail!!\n");
			}
			goto  EXIT_CNXTTVTV_FUNC;
		}

		stCnxtFilterInfo.ulTransactionId = ulTransactionId;
		stCnxtFilterInfo.ulDsiCrc = ulDsiCrc;
		hError = xsvc_epgcontext_SetContextFilterInfo_Tvtv(hAction, ulCnxtFilterSlot, &stCnxtFilterInfo);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_ProcessDsi_Tvtv] local_cnxtpf_SetContextFilterInfo fail!!\n");
			goto  EXIT_CNXTTVTV_FUNC;
		}

		hError = PAL_DSMCC_GetDownloadSize(&ulDownloadSize);
		if(hError != ERR_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_ProcessDsi_Tvtv] PAL_DSMCC_GetDownloadSize fail!!\n");
			goto  EXIT_CNXTTVTV_FUNC;
		}

		hError = xsvc_epgcontext_GetDownloadInfo_Tvtv(hAction, &stDownloadInfo);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_ProcessDsi_Tvtv] xsvc_epgcontext_GetDownloadInfo_Tvtv fail!!\n");
			goto  EXIT_CNXTTVTV_FUNC;
		}

		stDownloadInfo.ulEstimatedDownloadSize = ulDownloadSize;
		hError = xsvc_epgcontext_SetDownloadInfo_Tvtv(hAction, &stDownloadInfo);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_ProcessDsi_Tvtv] xsvc_epgcontext_SetDownloadInfo_Tvtv fail!!\n");
			goto  EXIT_CNXTTVTV_FUNC;
		}

		/** dsi info save to HDD */
		hError = xsvc_epgcontext_InitDirectory_Tvtv();
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_ProcessDsi_Tvtv] xsvc_epgcontext_InitDirectory_Tvtv fail!!\n");
			goto  EXIT_CNXTTVTV_FUNC;
		}

		HxLOG_Info("[xsvc_epgcontext_ProcessDsi_Tvtv] DSI save +!!\n");
		snprintf ((char *)szFileName, 256, "%s%s", EPG_FS_TVTV_DATA_PATH, TVTV_FILENAME_DSI_TABLE);
		hError = xsvc_epgcontext_Fopen_Tvtv(szFileName, &hFileHandle);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_ProcessDsi_Tvtv] fopen fail!!\n");
			goto EXIT_CNXTTVTV_FUNC;
		}

		stDsiTable.ulTableVersion = TVTV_DSI_TABLE_CURRENT_VERSION;
		stDsiTable.ulDsiTransactioinId = ulTransactionId;
		stDsiTable.ulDsiCrc = ulDsiCrc;
		stDsiTable.ulRegisteredTime = ulRegisteredTime;
		stDsiTable.ulComplete = 0;

		HxLOG_Info("[xsvc_epgcontext_ProcessDsi_Tvtv] Save!! stDsiTable.ulTableVersion(%x)stDsiTable.ulComplete(%x)stDsiTable.ulDsiTransactioinId(%x)!!\n",
																stDsiTable.ulTableVersion,
																stDsiTable.ulComplete,
																stDsiTable.ulDsiTransactioinId);

		hError = xsvc_epgcontext_Fwrite_Tvtv(&stDsiTable, sizeof(xsvcEpg_TvtvDsiTable_t), hFileHandle, ePAL_PB_SEEKPOSITION_START, &ulWriteSize);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_ProcessDsi_Tvtv] fwrite fail!!\n");
			(void)xsvc_epgcontext_Fclose_Tvtv(hFileHandle);
			goto EXIT_CNXTTVTV_FUNC;
		}

		hError = xsvc_epgcontext_Fclose_Tvtv(hFileHandle);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_ProcessDsi_Tvtv] fclose fail!!\n");
		}
		HxLOG_Info("[xsvc_epgcontext_ProcessDsi_Tvtv] DSI Save -!!\n");

#if		defined(CONFIG_TVTVLOG)
		eLog = eTvtvLog_DL_Progress;
#endif
	}

	hError = PAL_DSMCC_BuildDownloadData(usDsmccMessageId, &ucIsComplete, &pDsmccFile);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_ProcessDsi_Tvtv] PAL_DSMCC_BuildDownloadData fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :
	if(pucRawSection)
	{
		OxSI_Free(pucRawSection);
	}
#if	defined(CONFIG_TVTVLOG)
	if(eLog != eTvtvLog_Max)
	{
		SvcEpg_TvtvLogTable_t		stLogTbl = {0, };
		HUINT32				ulSysTime = 0;
		HxDATETIME_t		stDateTime;

		hError = VK_CLOCK_GetTime((HULONG*)&ulSysTime);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Error("[TVTVLOG] VK_CLOCK_GetTime fail!!\n");
			return hError;
		}

		HLIB_DATETIME_ConvertUnixTimeToDateTime(ulSysTime, &stDateTime);

		snprintf ((char *)szFileName, 256, "%s%s", EPG_FS_HD1, "tvtv.log");
		hError = xsvc_epgcontext_Fopen_Tvtv(szFileName, &hFileHandle);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Error("[TVTVLOG] fopen fail!!\n");
			return hError;
		}
		hError = xsvc_epgcontext_FileLength_Tvtv(szFileName, &ulFileSize);
		if(ulFileSize != 0)
		{
			hError = xsvc_epgcontext_Fread_Tvtv(&stLogTbl, sizeof(SvcEpg_TvtvLogTable_t), hFileHandle, ePAL_PB_SEEKPOSITION_START, &ulReadSize);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("[TVTVLOG] fread fail!!\n");
			}
		}
		switch(eLog)
		{
			case eTvtvLog_DL_Progress :
				sprintf((char*)stLogTbl.aucLog[stLogTbl.ucCurrent],	"Progress(%02d %02d:%02d) : OldComp(%d)OldTid(%x)OldCRC(%x) Tid(%x)CRC(%x)",
															stDateTime.stDate.ucDay,
															stDateTime.stTime.ucHour,
															stDateTime.stTime.ucMinute,
															ulOldComplete,
															ulOldTransactionId,
															ulOldCrc,
															stDsiTable.ulDsiTransactioinId,
															stDsiTable.ulDsiCrc	);
				break;

			case eTvtvLog_DL_SkipByHddData:
				sprintf((char*)stLogTbl.aucLog[stLogTbl.ucCurrent],	"SkipByHddData(%02d %02d:%02d) : OldComp(%d)OldTid(%x)OldCRC(%x) Tid(%x)CRC(%x)",
															stDateTime.stDate.ucDay,
															stDateTime.stTime.ucHour,
															stDateTime.stTime.ucMinute,
															ulOldComplete,
															ulOldTransactionId,
															ulOldCrc,
															stDsiTable.ulDsiTransactioinId,
															stDsiTable.ulDsiCrc	);
				break;

			default :
				break;
		}
		stLogTbl.ucCurrent = (stLogTbl.ucCurrent + 1)%5;
		hError = xsvc_epgcontext_Fwrite_Tvtv(&stLogTbl, sizeof(SvcEpg_TvtvLogTable_t), hFileHandle, ePAL_PB_SEEKPOSITION_START, &ulWriteSize);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Error("[TVTVLOG] fwrite fail!!\n");
			(void)xsvc_epgcontext_Fclose_Tvtv(hFileHandle);
			return hError;
		}
		hError = xsvc_epgcontext_Fclose_Tvtv(hFileHandle);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Error("[TVTVLOG] fclose fail!!\n");
		}
	}
#endif
	return hError;
}

STATIC HERROR				xsvc_epgcontext_ProcessDdb_Tvtv(HUINT32 ulParam1, HUINT32 ulParam2)
{
	HERROR						hError = ERR_EPG_FAIL;
	Handle_t						hAction = HANDLE_NULL;
	Handle_t						hSvcHandle = HANDLE_NULL;
	HUINT32						ulMessage = 0;
	HUINT8						ucHead = 0;
	HUINT32						ulTransactionId = 0;
	svcEpg_Msg_t						stMsg;
	svcEpg_Msg_t						*pstMsg = NULL;
	HUINT8						*pucRawSection = NULL;
	svcEpg_Filter_t 				stCnxtFilterInfo = {0, };
	HUINT32						ulCnxtFilterSlot = 0;
	svcEpg_RawBuffer_t			*pBuffer = NULL;
	HUINT8						ucIndex = 0;
	HUINT32						ulOrgDeliType = 0;
	HUINT8						ucAntId = 0;
	EPG_SpecType_e				eSpec;
	HUINT16						usDsmccMessageId = 0;
	HUINT16						usResult = 0;
	HUINT8						ucIsComplete = 0;
	PalDsmcc_File_t					*pDsmccFile = NULL;
	PalDsmcc_File_t					*pDsmccFileBuffer = NULL;
	svcEpg_DownloadInfo_t 		stDownloadInfo = {0, };
	svcEpg_PostProcessInfo_t		stPostprocessInfo = {0, };
	HBOOL						bFirstFile = FALSE;


	pstMsg = (svcEpg_Msg_t*)ulParam1;
	if(pstMsg == NULL)
	{
		HxLOG_Print("[xsvc_epgcontext_ProcessDdb_Tvtv] param fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	/**********************************************************************************
	  *
	  * 1. get param info
	  *
	  **********************************************************************************/
	ulMessage = pstMsg->ulMessage;
	hAction = (Handle_t)pstMsg->ulParam1;
	ulTransactionId = (HUINT32)pstMsg->ulParam2;
	pBuffer = (svcEpg_RawBuffer_t*)pstMsg->ulParam3;
	if(pBuffer == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_ProcessDdb_Tvtv] param fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}
	ucHead = ACT_HEAD;
	ulCnxtFilterSlot = EPGCNXT_FITER_SLOT_TVTV_DDB;

	/**********************************************************************************
	  *
	  * 2. set filtering cycle base info
	  *
	  **********************************************************************************/
	hError = xsvc_epgcontext_GetContextInfo_Tvtv(	hAction,
											ulCnxtFilterSlot,
											&hSvcHandle,
											&ulOrgDeliType,
											&ucAntId,
											&eSpec,
											&stCnxtFilterInfo);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_ProcessDdb_Tvtv] xsvc_epgcontext_GetContextInfo_Tvtv fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}
	if(ulTransactionId != stCnxtFilterInfo.ulTransactionId)
	{
		HxLOG_Error("[xsvc_epgcontext_ProcessDdb_Tvtv] ulTransactionId != stCnxtFilterInfo.ulTransactionId!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	hError = xsvc_epgcontext_GetDownloadInfo_Tvtv(hAction, &stDownloadInfo);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_ProcessDdb_Tvtv] xsvc_epgcontext_GetDownloadInfo_Tvtv fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}
	hError = xsvc_epgcontext_GetPostprocessInfo_Tvtv(hAction, &stPostprocessInfo);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_ProcessDdb_Tvtv] xsvc_epgcontext_GetPostprocessInfo_Tvtv fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}


	/**********************************************************************************
	  *
	  * 2. secton loop
	  *
	  **********************************************************************************/
	for(ucIndex = 0; ucIndex < pBuffer->usSectionCount && ucIndex < EPG_DSMCC_SECTION_BUFFER_MAX_NUM; ucIndex++)
	{
		ucIsComplete = 0;
		pucRawSection = NULL;
		pDsmccFile = NULL;
		pucRawSection = pBuffer->paucSection[ucIndex];
		if(pucRawSection == NULL)
		{
			HxLOG_Error("[xsvc_epgcontext_ProcessDdb_Tvtv] pucRawSection fail!!\n");
			goto END_DDB_LOOP;
		}

		/**********************************************************************************
		  *
		  * 2.1 parse ddb
		  *
		  **********************************************************************************/
		usDsmccMessageId = 0;
		hError = PAL_DSMCC_ParseDsmccForDdb(pucRawSection, &usDsmccMessageId);
		if(hError != ERR_OK)
		{
			HxLOG_Info("[xsvc_epgcontext_ProcessDdb_Tvtv] PAL_DSMCC_ParseDsmccForDdb fail!!\n");
			goto END_DDB_LOOP;
		}

		/**********************************************************************************
		  *
		  * 2.2 build ddb
		  *
		  **********************************************************************************/
		hError = PAL_DSMCC_BuildDownloadData(usDsmccMessageId, &ucIsComplete, &pDsmccFile);
		if(hError != ERR_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_ProcessDdb_Tvtv] PAL_DSMCC_BuildDownloadData fail!!\n");
			goto END_DDB_LOOP;
		}

END_DDB_LOOP:
		/**********************************************************************************
		  *
		  * 2.3 complete ddb
		  *
		  **********************************************************************************/
		if(pDsmccFile != NULL)
		{
			bFirstFile = FALSE;
			if(stDownloadInfo.ulNumOfDownloadedFile == 0)
			{
				bFirstFile = TRUE;
			}

			HxLOG_Info("[xsvc_epgcontext_ProcessDdb_Tvtv] xsvc_epgcontext_AddDownloadedFile_Tvtv \n");
			pDsmccFileBuffer = NULL;
			hError = xsvc_epgcontext_AddDownloadedFile_Tvtv(bFirstFile, ucIsComplete, pDsmccFile, &stDownloadInfo, &pDsmccFileBuffer);
			if(pDsmccFileBuffer != NULL)
			{
				HxLOG_Info("[xsvc_epgcontext_ProcessDdb_Tvtv] SendMessage \n");
				HxSTD_memset(&stMsg, 0x00, sizeof(svcEpg_Msg_t));
				stMsg.ulMessage = eEPG_MSG_TVTV_POSTPROCESSBUFFER;
				stMsg.ulParam1 = (HUINT32)hAction;
				stMsg.ulParam2 = (HUINT32)pDsmccFileBuffer;
				stMsg.ulParam3 = (HUINT32)bFirstFile;
				hError = UTIL_TASK_SendMessage(stPostprocessInfo.ucTaskSlot, &stMsg);
				if(hError != ERR_TASKUTIL_OK)
				{
					HxLOG_Error("[xsvc_epgcontext_ProcessDdb_Tvtv] SendMessage fail!!\n");
				}
			}
			pDsmccFile = NULL;
		}
		if(ucIsComplete == 1)
		{
			stDownloadInfo.bCompleteDownload = TRUE;
			stCnxtFilterInfo.bComplete = TRUE;
			hError = xsvc_epgcontext_SetContextFilterInfo_Tvtv(hAction, ulCnxtFilterSlot, &stCnxtFilterInfo);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_ProcessDdb_Tvtv] xsvc_epgcontext_SetContextFilterInfo_Tvtv fail!!\n");
			}
			hError = SVC_EPG_MakeEpgMessage(EPG_CMD_STOP_FILTER,  eEPG_PROCESS_TVTV, &ulMessage);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_ProcessDdb_Tvtv] MW_EPGMAIN_MakeEpgMessage fail!!\n");
			}
			hError = SVC_EPG_SendMessage(ulMessage, hAction, ucHead, usResult);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_ProcessDdb_Tvtv] SendMessage fail!!\n");
			}
			break;
		}
	}

	hError = xsvc_epgcontext_SetDownloadInfo_Tvtv(hAction, &stDownloadInfo);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_ProcessDdb_Tvtv] xsvc_epgcontext_SetDownloadInfo_Tvtv fail!!\n");
	}

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :
	if(stCnxtFilterInfo.eState == eEPG_CNXT_FILTER_NOTUSED)
	{
		xsvc_epgcontext_FreeDownloadedFile_Tvtv();
	}

	if(pBuffer)
	{
		for(ucIndex = 0; ucIndex < pBuffer->usSectionCount && ucIndex < EPG_DSMCC_SECTION_BUFFER_MAX_NUM; ucIndex++)
		{
			if(pBuffer->paucSection[ucIndex])
			{
				OxSI_Free(pBuffer->paucSection[ucIndex]);
			}
		}
		OxEPG_Free(pBuffer);
	}
	hError = ERR_EPG_OK;

	return hError;
}


#define	DOWNLOAD_FILE_NUM_MAX			5

STATIC PalDsmcc_File_t		*s_pstDownloadDsmccBuffer = NULL;
STATIC HERROR		xsvc_epgcontext_AddDownloadedFile_Tvtv(HBOOL bFirst, HUINT8 ucIsComplete, PalDsmcc_File_t *pDsmccFile, svcEpg_DownloadInfo_t *pDownloadInfo, PalDsmcc_File_t **ppDsmccFileList)
{
	HERROR					hError = ERR_FAIL;
	PalDsmcc_File_t				*pTempFile = NULL;
	PalDsmcc_File_t				*pAddFile = NULL;
	HUINT32					ulCount = 0;

	if(pDsmccFile == NULL || pDownloadInfo == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_AddDownloadedFile_Tvtv] param NULL!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}
	*ppDsmccFileList = NULL;

	pTempFile = pDsmccFile;
	while(pTempFile)
	{
		pAddFile = NULL;
		if(pTempFile->ulModuleSize == 0 || pTempFile->pucFilePointer == NULL)
		{
			HxLOG_Error("[xsvc_epgcontext_AddDownloadedFile_Tvtv] invalid file!!\n");
			goto END_FILESAVE_LOOP;
		}

END_FILESAVE_LOOP :
		pAddFile = pTempFile;
		pTempFile = pTempFile->pNext;
		pAddFile->pNext = NULL;

		s_pstDownloadDsmccBuffer = xsvc_epgcontext_AppendFile_Tvtv(s_pstDownloadDsmccBuffer, pAddFile);
		pDownloadInfo->ulCurrentDownloadedSize += pAddFile->ulModuleSize;
		pDownloadInfo->ulNumOfDownloadedFile++;
	}

	ulCount = xsvc_epgcontext_GetFileCount_Tvtv(s_pstDownloadDsmccBuffer);
	if(	ucIsComplete
		|| bFirst
		|| ulCount >= DOWNLOAD_FILE_NUM_MAX)
	{
		*ppDsmccFileList = s_pstDownloadDsmccBuffer;
		s_pstDownloadDsmccBuffer = NULL;
	}

	hError = ERR_OK;

EXIT_CNXTTVTV_FUNC :

	return hError;
}

STATIC HERROR		xsvc_epgcontext_FreeDownloadedFile_Tvtv(void)
{
	HERROR					hError = ERR_FAIL;
	PalDsmcc_File_t				*pTempFile = NULL;
	PalDsmcc_File_t				*pDeleteFile = NULL;

	pTempFile = s_pstDownloadDsmccBuffer;
	while(pTempFile)
	{
		pDeleteFile = pTempFile;
		pTempFile = pTempFile->pNext;

		if(pDeleteFile->pucFilePointer)
		{
			OxDSMCC_Free(pDeleteFile->pucFilePointer);
		}
		if(pDeleteFile->pucModuleName)
		{
			OxDSMCC_Free(pDeleteFile->pucModuleName);
		}
		OxDSMCC_Free(pDeleteFile);
	}

	s_pstDownloadDsmccBuffer = NULL;

	hError = ERR_OK;

//EXIT_CNXTTVTV_FUNC :

	return hError;
}

STATIC HUINT32		xsvc_epgcontext_GetFileCount_Tvtv(PalDsmcc_File_t *pRoot)
{
	HUINT32			ulCount = 0;
	PalDsmcc_File_t		*pTemp = NULL;


	pTemp = pRoot;
	while(pTemp)
	{
		ulCount++;
		pTemp = pTemp->pNext;
	}

	return ulCount;
}

STATIC PalDsmcc_File_t*		xsvc_epgcontext_AppendFile_Tvtv(PalDsmcc_File_t *pRoot, PalDsmcc_File_t *pFile)
{
	if(pFile == NULL)
	{
		return pRoot;
	}

	pFile->pNext = NULL;
	if(pRoot == NULL)
	{
		pRoot = pFile;
		return pRoot;
	}
	pFile->pNext = pRoot;
	pRoot = pFile;

	return pRoot;
}

STATIC HERROR				xsvc_epgcontext_Postprosess_Tvtv(HUINT32 ulParam1, HUINT32 ulParam2)
{
	HUINT32						ulMessage = 0;
	HERROR						hError = ERR_EPG_FAIL;
	svcEpg_Msg_t						*pstMsg = NULL;

	pstMsg = (svcEpg_Msg_t*)ulParam1;
	if(pstMsg == NULL)
	{
		HxLOG_Print("[xsvc_epgcontext_Postprosess_Tvtv] param fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	ulMessage = pstMsg->ulMessage;
	switch(ulMessage)
	{
		case eEPG_MSG_TVTV_POSTPROCESSBUFFER :
			HxLOG_Info("[xsvc_epgcontext_Postprosess_Tvtv] eEPG_MSG_TVTV_POSTPROCESSBUFFER !!\n");
			hError= xsvc_epgcontext_PostprosessBuffer_Tvtv(ulParam1, ulParam2);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_Postprosess_Tvtv] xsvc_epgcontext_PostprosessBuffer_Tvtv fail!!\n");
			}
			break;

		case eEPG_MSG_TVTV_POSTPROCESSFILE:
			hError= xsvc_epgcontext_PostprosessFile_Tvtv(ulParam1, ulParam2);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_Postprosess_Tvtv] xsvc_epgcontext_PostprosessFile_Tvtv fail!!\n");
			}
			break;

		default :
			HxLOG_Error("[xsvc_epgcontext_Postprosess_Tvtv] default fail!!\n");
			break;

	}


	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :

	return hError;
}

#define ____PROCESS_SENDTODAMA_FUNC____

void inline TVTVEPG_MemFree(void *mem)
{
	if(mem)	HLIB_STD_MemFree(mem);
}

void * TVTVEPG_MemAlloc(HUINT32 size)
{
	return HLIB_STD_MemAlloc(size);
}
void * TVTVEPG_MemCalloc(HUINT32 size)
{
	return HLIB_STD_MemCalloc(size);
}

HCHAR * TVTVEPG_StrDup(const HCHAR * str)
{
	return HLIB_STD_StrDup(str);
}

#if defined(TEMP_DBG_RPINT)
void xsvc_epgcontext_PrintEventData(DxTvtvEvent_t *pTvtvEvent, HINT32 ulBinSize)
{
	STATIC HUINT32 save_cnt = 0;
#if 0
	HUINT16 i = 0;
	HxDATETIME_t stStartTime, stEndTime;
#endif

	if(pTvtvEvent==NULL || ulBinSize<=0)
	{
		return;
	}

	if(save_cnt%10 == 0)
	{
		HLIB_CMD_Printf("\n******************** PRINT : EPG Data Send to Dama : cnt[%d], size[%d] -- tvtvid(%d), uid(%d),  onid(0x%04x), tsid(0x%04x), svcid(0x%04x) *************\n",
			save_cnt,
			ulBinSize,
			pTvtvEvent->stBase.usTvtvId,
			pTvtvEvent->stBase.ulSvcUid,
			pTvtvEvent->stBase.usOnId,
			pTvtvEvent->stBase.usTsId,
			pTvtvEvent->stBase.usSvcId);
	}
	save_cnt	++;
#if 0
	HLIB_CMD_Printf("*		type = %s\n", pTvtvEvent->stBase.type==eDxEPG_TYPE_TVTV?"TVTV":"NULL");
	HLIB_CMD_Printf("*		Version = %d\n", pTvtvEvent->stBase.usVer);
	HLIB_CMD_Printf("*		TvtvId = %d\n", pTvtvEvent->stBase.usTvtvId);
	HLIB_CMD_Printf("*		ProgId = %d\n", pTvtvEvent->stBase.ulProgId);
	HLIB_CMD_Printf("*		EventId = %d\n", pTvtvEvent->stBase.usEventId);
	HLIB_CMD_Printf("*		SvcUid = %d\n", pTvtvEvent->stBase.ulSvcUid);
	HLIB_CMD_Printf("*		OnId = %d\n", pTvtvEvent->stBase.usOnId);
	HLIB_CMD_Printf("*		TsId = %d\n", pTvtvEvent->stBase.usTsId);
	HLIB_CMD_Printf("*		SvcId = %d\n", pTvtvEvent->stBase.usSvcId);
	HLIB_DATETIME_ConvertUnixTimeToDateTime( pTvtvEvent->stBase.ulStartTime, &stStartTime);
	HLIB_CMD_Printf("*		Stime : Current Update Time [%04d-%02d-%02d %02d:%02d:%02d] ulSystemTime[%d]\n",
		stStartTime.stDate.usYear, stStartTime.stDate.ucMonth, stStartTime.stDate.ucDay , stStartTime.stTime.ucHour, stStartTime.stTime.ucMinute, stStartTime.stTime.ucSecond, pTvtvEvent->stBase.ulStartTime);
	HLIB_DATETIME_ConvertUnixTimeToDateTime( pTvtvEvent->stBase.ulEndTime, &stEndTime);
	HLIB_CMD_Printf("*		Stime : Current Update Time [%04d-%02d-%02d %02d:%02d:%02d] ulSystemTime[%d]\n",
		stEndTime.stDate.usYear, stEndTime.stDate.ucMonth, stEndTime.stDate.ucDay , stEndTime.stTime.ucHour, stEndTime.stTime.ucMinute, stEndTime.stTime.ucSecond, pTvtvEvent->stBase.ulEndTime);
	HLIB_CMD_Printf("*		Rating = %d\n", pTvtvEvent->stBase.ucRating);
	HLIB_CMD_Printf("*		Season = %d\n", pTvtvEvent->stBase.ulSeason);
	HLIB_CMD_Printf("*		Seriesid = %d\n", pTvtvEvent->stBase.ulSeriesId);
	HLIB_CMD_Printf("*		TotalEposodeNum = %d\n", pTvtvEvent->stBase.ulTotalNum);
	HLIB_CMD_Printf("*		EpisodeNum = %d\n", pTvtvEvent->stBase.ulEpisodeNum);
	HLIB_CMD_Printf("*		Link :\n");
	for(i = 0; i< EPG_TVTV_LINK_MAX; i++)
	{
		if(pTvtvEvent->stBase.astEventLink[i].ulProgId > 0 )
		{
			HLIB_CMD_Printf("			type(%d), progId(%d)\n", pTvtvEvent->stBase.astEventLink[i].ucType, pTvtvEvent->stBase.astEventLink[i].ulProgId);
		}
	}
	HLIB_CMD_Printf("*		Content(genre) = %d\n", pTvtvEvent->stBase.aucContent[0]);
	HLIB_CMD_Printf("*		Tip = %d\n", pTvtvEvent->stBase.ucTip);
	HLIB_CMD_Printf("*		Lang = %c%c%c\n", pTvtvEvent->stBase.ucLang[0],pTvtvEvent->stBase.ucLang[1], pTvtvEvent->stBase.ucLang[2]);
	HLIB_CMD_Printf("*		Text :\n");
	for(i = eDxTVTVEPG_TEXT_NAME; i< eDxTVTVEPG_TEXT_MAX; i++)
	{
		if(pTvtvEvent->stText[i].ucStr && pTvtvEvent->stText[i].usSize>0)
		{
			HUINT8 buf[100];
			HxSTD_memset(buf, 0, 100);
			if(i==eDxTVTVEPG_TEXT_NAME)				snprintf(buf, 20, "%s", "name");
			else if(i==eDxTVTVEPG_TEXT_SHORT)		snprintf(buf, 20, "%s", "short");
			else if(i==eDxTVTVEPG_TEXT_DETAIL)		snprintf(buf, 20, "%s", "detail");
			else if(i==eDxTVTVEPG_TEXT_THUMBNAIL)	snprintf(buf, 20, "%s", "thumbnail");
			else if(i==eDxTVTVEPG_TEXT_ACTOR)		snprintf(buf, 20, "%s", "actor");
			else if(i==eDxTVTVEPG_TEXT_ACTRESS)		snprintf(buf, 20, "%s", "actress");
			else if(i==eDxTVTVEPG_TEXT_DIRECTOR)	snprintf(buf, 20, "%s", "director");
			else if(i==eDxTVTVEPG_TEXT_PERSON)		snprintf(buf, 20, "%s", "person");

			HLIB_CMD_Printf("			%s[%d] = %s\n", buf, pTvtvEvent->stText[i].usSize, pTvtvEvent->stText[i].ucStr);
		}
	}
#endif
}
void xsvc_epgcontext_PrintTvtvMapInfo(Print_TvtvMap_t ePrintTvtvMap, svcEpg_TvtvMap_t *pstTvtvMap, HUINT8 ucReset)
{
#if 0
	HUINT8 buf[100];
	HUINT16 cnt = 0;
	if(pstTvtvMap==NULL)
	{
		return;
	}
	HxSTD_memset(&buf, 0, sizeof(buf));
	if(ePrintTvtvMap==eTVTVMAP_PARSE)
	{
		static int parse_cnt = 0;
		sprintf(buf, "%s", "######## TVTV TSMT Parse : ");
		if(ucReset)
			parse_cnt = 0;
		cnt = ++parse_cnt;
	}
	else if(ePrintTvtvMap==eTVTVMAP_NOT_FOUND)
	{
		static int notfound_cnt = 0;
		sprintf(buf, "%s", "@@@@@@@ TVTV TSMT Not Found : ");
		if(ucReset)
			notfound_cnt = 0;
		cnt = ++notfound_cnt;
	}
	else if(ePrintTvtvMap==eTVTVMAP_SET)
	{
		static int set_cnt = 0;
		sprintf(buf, "%s", "************* TVTV TSMT Set : ");
		if(ucReset)
			set_cnt = 0;
		cnt = ++set_cnt;
	}
	else if(ePrintTvtvMap==eTVTVMAP_SENDTODAMA)
	{
		static int sendtodama_cnt = 0;
		sprintf(buf, "%s", "++++++++ TVTV TSMT Send To Dama : ");
		if(ucReset)
			sendtodama_cnt = 0;
		cnt = ++sendtodama_cnt;
	}
	else if(ePrintTvtvMap==eTVTVMAP_VALIDEVENT)
	{
		static int validevent_cnt = 0;
		sprintf(buf, "%s", "++++++++ TVTV TSMT Check Event : ");
		if(ucReset)
			validevent_cnt = 0;
		cnt = ++validevent_cnt;
	}
	else if(ePrintTvtvMap==eTVTVMAP_VALIDEVENT2)
	{
		static int validevent_cnt = 0;
		sprintf(buf, "%s", "++++++++ TVTV TSMT Check Event Second : ");
		if(ucReset)
			validevent_cnt = 0;
		cnt = ++validevent_cnt;
	}
	else if(ePrintTvtvMap==eTVTVMAP_SENDEVENT)
	{
		static int sendevent_cnt = 0;
		sprintf(buf, "%s", "++++++++ TVTV TSMT Event Send To Dama : ");
		if(ucReset)
			sendevent_cnt = 0;
		cnt = ++sendevent_cnt;
	}
	else
	{
		return;
	}

	HLIB_CMD_Printf("%s : cnt[%d], svcUid(%d), tvtvId(%d), onid(%d), tsid(%d), svcid(%d)\n",
		buf, cnt, pstTvtvMap->ulSvcUid, pstTvtvMap->usTvtvId, pstTvtvMap->stTripleId.usOnId,  pstTvtvMap->stTripleId.usTsId, pstTvtvMap->stTripleId.usSvcId);
#endif
}
#endif

#if defined(TVTV_USE_SVCUID)
// 임시로 tvtvid를 이용해서 tripleId, svcUid를 얻는다.
HERROR xsvc_epgcontext_GetSvcInfoFromSvcList(HUINT32 ulTvtvId, HUINT16 *pUsOnId, HUINT16 *pUsTsId, HUINT16 *pUsSvcId, HUINT32 *pUsSvcUid)
{
	HUINT16 i = 0;
	HERROR		hError = ERR_EPG_FAIL;

	for(i=0;i<stTvtvSvcInfoList_Temp.channel_cnt;i++)
	{
		// 현재 channel이 존재하는경우에만 보낸다.
		if(stTvtvSvcInfoList_Temp.stTvtvSvcInfo_Temp[i].ulTvtvId == ulTvtvId && stTvtvSvcInfoList_Temp.stTvtvSvcInfo_Temp[i].ulUid > 0)
		{
			*pUsOnId = stTvtvSvcInfoList_Temp.stTvtvSvcInfo_Temp[i].stTripleId.usOnId;
			*pUsTsId = stTvtvSvcInfoList_Temp.stTvtvSvcInfo_Temp[i].stTripleId.usTsId;
			*pUsSvcId = stTvtvSvcInfoList_Temp.stTvtvSvcInfo_Temp[i].stTripleId.usSvcId;
			*pUsSvcUid = stTvtvSvcInfoList_Temp.stTvtvSvcInfo_Temp[i].ulUid;
			hError = ERR_EPG_OK;
		}
	}

	return hError;
}

#if defined(TVTV_NEW_ONE)
#else
void xsvc_epgcontext_GetSvcUidFromTripleId(HUINT32 ulTvtvId, HUINT16 usOnId, HUINT16 usTsId, HUINT16 usSvcId, HUINT32 *pUsSvcUid)
{

	HUINT16 i = 0;

	*pUsSvcUid= 0;
	for(i=0;i<stTvtvSvcInfoList_Temp.channel_cnt;i++)
	{
		if( (stTvtvSvcInfoList_Temp.stTvtvSvcInfo_Temp[i].stTripleId.usOnId == usOnId) 		&&
		    (stTvtvSvcInfoList_Temp.stTvtvSvcInfo_Temp[i].stTripleId.usTsId == usTsId) 		&&
		    (stTvtvSvcInfoList_Temp.stTvtvSvcInfo_Temp[i].stTripleId.usSvcId == usSvcId)	)
		{
			*pUsSvcUid = stTvtvSvcInfoList_Temp.stTvtvSvcInfo_Temp[i].ulUid;
			stTvtvSvcInfoList_Temp.stTvtvSvcInfo_Temp[i].ulTvtvId = ulTvtvId;		// event data를 보낼경우 tvtv map을 상실함으로 tvtv id를 다시 전달한다.
			break;
		}
	}
}
#endif

HxList_t * xsvc_epgcontext_SetSvcListFromDama_Tvtv(const HCHAR *name, HUID uid, const void *pvData, HUINT32 size, void *pvUser)
{
	DAxIterator_t iter;
	HxList_t	* svclist = NULL;
	xsvcEpg_TvtvSvcInfo_t *pTvtvService = NULL;
	DxService_t	*list = NULL;
#if defined(TEMP_DBG_RPINT)
	static HUINT32 svclistcntforepg = 0;

	svclistcntforepg = 0;
#endif

	if(size == 0 || pvData == NULL)
	{
		return NULL;
	}

	HxLOG_Debug("==> Get Data Size[%d]  svcinfo Size[%d]\n",size, sizeof(DxService_t));
	DAPI_InitIterator(&iter, pvData, size, sizeof(DxService_t));
	while (DAPI_NextIterator(&iter))
	{
		list = (DxService_t *)iter.data;

		if(list == NULL)
		{
			continue;
		}

		if(list->svcid == 0 || list->onid == 0 || list->tsid == 0 || list->lcn == 0 || (list->svcType!=eDxSVC_TYPE_TV && list->svcType!=eDxSVC_TYPE_RADIO))
		{
			continue;
		}
#if defined(TVTV_ADD_USR_TP)
		// 현재 service에 등록된 놈만 add한다.
		if(xsvc_epgcontext_IsExistTVTVEPG(list->tsid, list->svcid) != ERR_OK)
		{
			continue;
		}
#endif
		if(stTvtvSvcInfoList_Temp.channel_cnt >= TVTV_MAXNUM_OF_TRIPLE_CHANNELIST)
		{
			break;
		}

		pTvtvService = (xsvcEpg_TvtvSvcInfo_t *)TVTVEPG_MemCalloc(sizeof(xsvcEpg_TvtvSvcInfo_t));
		if(pTvtvService != NULL)
		{
			pTvtvService->stTripleId.usSvcId = list->svcid;
			pTvtvService->stTripleId.usTsId  = list->tsid;
			pTvtvService->stTripleId.usOnId  = list->onid;
			pTvtvService->ulLcn   = list->lcn;
			pTvtvService->ulUid	= list->uid;
			HLIB_STD_StrUtf8NCpy(pTvtvService->ucName, list->name, DxNAME_LEN);

			stTvtvSvcInfoList_Temp.stTvtvSvcInfo_Temp[stTvtvSvcInfoList_Temp.channel_cnt].stTripleId.usOnId = pTvtvService->stTripleId.usOnId;
			stTvtvSvcInfoList_Temp.stTvtvSvcInfo_Temp[stTvtvSvcInfoList_Temp.channel_cnt].stTripleId.usTsId = pTvtvService->stTripleId.usTsId;
			stTvtvSvcInfoList_Temp.stTvtvSvcInfo_Temp[stTvtvSvcInfoList_Temp.channel_cnt].stTripleId.usSvcId = pTvtvService->stTripleId.usSvcId;
			stTvtvSvcInfoList_Temp.stTvtvSvcInfo_Temp[stTvtvSvcInfoList_Temp.channel_cnt].ulUid = pTvtvService->ulUid;
			stTvtvSvcInfoList_Temp.channel_cnt++;

#if defined(TEMP_DBG_RPINT)
			{
				++svclistcntforepg;
//				static int cnt = 0;
//				HLIB_CMD_Printf("@@@ SVC Info from dama :  cnt[%d] : Uid = %d, onId = %d, tsid = %d, svcId = %d, name = %s\n", ++cnt, pTvtvService->ulUid, pTvtvService->stTripleId.usOnId, pTvtvService->stTripleId.usTsId, pTvtvService->stTripleId.usSvcId, pTvtvService->ucName);
			}
#endif
			svclist = HLIB_LIST_Append(svclist, pTvtvService);
			pTvtvService = NULL;
		}
	}

#if defined(TEMP_DBG_RPINT)
	HLIB_CMD_Printf("===> svclistcntforepg = %d\n", svclistcntforepg);
#endif

	HxLOG_Debug("xsvc_epgcontext_SetSvcListFromDama_Tvtv chlist Count[%d]\n",  HLIB_LIST_Length(svclist));

	return svclist;
}

STATIC void	xsvc_epgcontext_SetSvcList_Tvtv (const HCHAR *name, HUID uid, const void *pvData, HUINT32 size, void *pvUser)
{
	HxList_t	* svclist = NULL;

	HxLOG_Trace();

	svclist = xsvc_epgcontext_SetSvcListFromDama_Tvtv(name, uid, pvData, size, pvUser);

	if(svclist != NULL)
	{
		HLIB_LIST_Foreach(svclist , TVTVEPG_MemFree);
		HLIB_LIST_RemoveAll(svclist);
	}
}

STATIC void xsvc_epgcontext_SetAllSvcInfo_Tvtv(void)
{
	HxSTD_memset(&stTvtvSvcInfoList_Temp, 0, sizeof(xsvcEpg_TvtvSvcInfoList_t));
	DAPI_GetAll(DxNAMEOF(DxService_t), (DAPI_Getter)xsvc_epgcontext_SetSvcList_Tvtv, NULL);
}

STATIC HERROR xsvc_epgcontext_ValidTvtvMapInfo_Tvtv(svcEpg_TvtvMap_t *pstTvtvMap)
{
	HERROR		hError = ERR_EPG_FAIL;

	if(pstTvtvMap == NULL)
	{
		goto EXIT_CNXTTVTV_FUNC;
	}
#if defined(CONFIG_FUNC_EPG_USE_SVCUID)
	if(pstTvtvMap->ulSvcUid == 0)
	{
		goto EXIT_CNXTTVTV_FUNC;
	}
#endif
	if(pstTvtvMap->usTvtvId == 0)
	{
		goto EXIT_CNXTTVTV_FUNC;
	}

	if(pstTvtvMap->stTripleId.usOnId==0 && pstTvtvMap->stTripleId.usTsId==0 && pstTvtvMap->stTripleId.usSvcId==0)
	{
		goto EXIT_CNXTTVTV_FUNC;
	}

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC:

	return hError;
}
#endif

#if defined(TVTV_USE_TRIPLEID)
STATIC void xsvc_epgcontext_SvcInfoListInit_Tvtv(void)
{
	HxSTD_memset(&stTvtvSvcInfoList, 0, sizeof(xsvcEpg_TvtvSvcInfoList_t));
}

STATIC void xsvc_epgcontext_SetTvtvSvcInfoList(HUINT32 ulTvtvId, HUINT16 usOnId, HUINT16 usTsId, HUINT16 usSvcId, HUINT32 *pUsSvcUid)
{
	HUINT16 i = stTvtvSvcInfoList.usTsCnt;

	stTvtvSvcInfoList.stTvtvSvcInfo[i].stTripleId.usOnId = usOnId;
	stTvtvSvcInfoList.stTvtvSvcInfo[i].stTripleId.usTsId = usTsId;
	stTvtvSvcInfoList.stTvtvSvcInfo[i].stTripleId.usSvcId = usSvcId;
	stTvtvSvcInfoList.stTvtvSvcInfo[i].ulTvtvId = ulTvtvId;

	stTvtvSvcInfoList.usTsCnt++;
	*pUsSvcUid = stTvtvSvcInfoList.stTvtvSvcInfo[i].ulSvcUid = stTvtvSvcInfoList.usTsCnt; // 임시로 svcUid를 저장한다. 왜냐하면 svcUid가 같으면 db로 저장이 안되는 문제가 발생함으로.
}

STATIC HERROR xsvc_epgcontext_GetSvcInfoFromSvcInfoList_Tvtv(HUINT16 usMultiTsIdx, HUINT16 *pUsOnId, HUINT16 *pUsTsId, HUINT16 *pUsSvcId, HUINT32 *pUsSvcUid)
{
	HERROR		hError = ERR_EPG_FAIL;
	HUINT16 		i = stTvtvSvcInfoList.ucMultiTsIdx[usMultiTsIdx];

	if(pUsOnId==NULL || pUsTsId==NULL || pUsSvcId==NULL)
	{
		goto EXIT_CNXTTVTV_FUNC;
	}

	*pUsOnId = stTvtvSvcInfoList.stTvtvSvcInfo[i].stTripleId.usOnId;
	*pUsTsId = stTvtvSvcInfoList.stTvtvSvcInfo[i].stTripleId.usTsId;
	*pUsSvcId = stTvtvSvcInfoList.stTvtvSvcInfo[i].stTripleId.usSvcId;
	*pUsSvcUid = stTvtvSvcInfoList.stTvtvSvcInfo[i].ulSvcUid; // 임시로 svcUid를 저장한다. 왜냐하면 svcUid가 같으면 db로 저장이 안되는 문제가 발생함으로.

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC:

	return hError;
}

STATIC HERROR xsvc_epgcontext_GetMultiTsCntFromTvtvId_Tvtv(HUINT32 ulTvtvId, HUINT16 *pUsMultiTsCnt)
{
	HERROR		hError = ERR_EPG_FAIL;
	HUINT16		usTsCnt = 0;
	HUINT16 		i = 0;

	if(pUsMultiTsCnt==NULL)
	{
		goto EXIT_CNXTTVTV_FUNC;
	}

	for(i=0; i<stTvtvSvcInfoList.usTsCnt; i++)
	{
		if(stTvtvSvcInfoList.stTvtvSvcInfo[i].ulTvtvId == ulTvtvId)
		{
			stTvtvSvcInfoList.ucMultiTsIdx[usTsCnt] = i;
			usTsCnt++;
		}
	}

	if(usTsCnt==0)
	{
		goto EXIT_CNXTTVTV_FUNC;
	}

	*pUsMultiTsCnt = usTsCnt;
	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC:

	return hError;
}
#endif

HERROR xsvc_epgcontext_GetDxTvtvEvent(SvcEpg_Event_t *pEvent, DxTvtvEvent_t *pDxEvent, svcEpg_TvtvMap_t *pTvtvMap)
{
	HERROR		hError = ERR_EPG_FAIL;
//	HUINT8		*pucConvertString = NULL;

	if(pEvent==NULL || pDxEvent==NULL || pTvtvMap==NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_GetDxTvtvEvent] param fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	pDxEvent->stBase.type = eDxEPG_TYPE_TVTV;
	pDxEvent->stBase.usVer = pEvent->stEventInfo.stTvtvEventInfo.usVersion;
	pDxEvent->stBase.usTvtvId = pEvent->stEventInfo.stTvtvEventInfo.usTvtvId;
	pDxEvent->stBase.ulProgId = pEvent->stEventInfo.stTvtvEventInfo.ulProgId;
	pDxEvent->stBase.usEventId = pEvent->stEventInfo.stTvtvEventInfo.nEventId;
	pDxEvent->stBase.ulSvcUid = pTvtvMap->ulSvcUid;
	pDxEvent->stBase.usOnId = pTvtvMap->stTripleId.usOnId;
	pDxEvent->stBase.usTsId = pTvtvMap->stTripleId.usTsId;
	pDxEvent->stBase.usSvcId = pTvtvMap->stTripleId.usSvcId;
	pDxEvent->stBase.ulStartTime = pEvent->ulStartTime;
	pDxEvent->stBase.ulEndTime = pEvent->ulStartTime + pEvent->ulDuration;
	pDxEvent->stBase.ucRating = pEvent->stEventInfo.stTvtvEventInfo.astParentalRating[0].ucParentalRating;
	pDxEvent->stBase.ulSeason = pEvent->stEventInfo.stTvtvEventInfo.ulSeason;
	pDxEvent->stBase.ulSeriesId = pEvent->stEventInfo.stTvtvEventInfo.ulSeriesId;
	pDxEvent->stBase.ulTotalNum = pEvent->stEventInfo.stTvtvEventInfo.ulTotalNum;

	hError = xsvc_epglib_SetDataByType_Tvtv(	TVTV_TYPE_DECIMAL,
														pEvent->stEventInfo.stTvtvEventInfo.aucSeriesNumber,
														NULL, 0, 0, NULL,
														&pDxEvent->stBase.ulEpisodeNum);

	HxSTD_memcpy(pDxEvent->stBase.astEventLink, pEvent->stEventInfo.stTvtvEventInfo.astEventLink, sizeof(SvcEpg_EventLink_t)*EPG_TVTV_LINK_MAX);
	pDxEvent->stBase.aucContent[0] = pEvent->aucContent[0];
	pDxEvent->stBase.ucTip = pEvent->stEventInfo.stTvtvEventInfo.ucTip;

	HxSTD_memcpy(pDxEvent->stBase.ucLang, pEvent->stEventInfo.stTvtvEventInfo.aucIsoCountryCode, EPG_TVTV_MAX_LANG);

	//xsvc_epgcontext_ConvertEpgTextToString(pEvent->astShortText[0].pEventName, &pucConvertString);
	pDxEvent->stText[eDxTVTVEPG_TEXT_NAME].ucStr = TVTVEPG_StrDup(pEvent->astShortText[0].pEventName->aucTextChar);
	if(pDxEvent->stText[eDxTVTVEPG_TEXT_NAME].ucStr)
	{
		pDxEvent->stText[eDxTVTVEPG_TEXT_NAME].usSize= HxSTD_StrLen(pDxEvent->stText[eDxTVTVEPG_TEXT_NAME].ucStr);
	}

//	xsvc_epgcontext_ConvertEpgTextToString(pEvent->astShortText[0].pEventText, &pucConvertString);
	pDxEvent->stText[eDxTVTVEPG_TEXT_SHORT].ucStr = TVTVEPG_StrDup(pEvent->astShortText[0].pEventText->aucTextChar);
	if(pDxEvent->stText[eDxTVTVEPG_TEXT_SHORT].ucStr)
	{
		pDxEvent->stText[eDxTVTVEPG_TEXT_SHORT].usSize= HxSTD_StrLen(pDxEvent->stText[eDxTVTVEPG_TEXT_SHORT].ucStr);
	}

	if(strstr(pEvent->stEventInfo.stTvtvEventInfo.aucImageName, ".jpg"))
	{
		HUINT8 buf[128];
		HxSTD_memset(&buf, 0, sizeof(buf));
		snprintf((char *)buf, 100, "%s/%s", TVTVEPG_THUMBNAIL_PATH, pEvent->stEventInfo.stTvtvEventInfo.aucImageName);
		pDxEvent->stText[eDxTVTVEPG_TEXT_THUMBNAIL].ucStr = TVTVEPG_StrDup(buf);
		if(pDxEvent->stText[eDxTVTVEPG_TEXT_THUMBNAIL].ucStr)
		{
			pDxEvent->stText[eDxTVTVEPG_TEXT_THUMBNAIL].usSize= HxSTD_StrLen(pDxEvent->stText[eDxTVTVEPG_TEXT_THUMBNAIL].ucStr);
		}
	}

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC:

	return hError;
}


STATIC HERROR xsvc_epgcontext_SvcInfoSendToDama_tvtv(HTREEMGR hTree)
{
	HERROR						hError = ERR_EPG_FAIL;
	HTREEITEM					hFirstChild = NULL;
	HTREEITEM					hTempItem = NULL;
	svcEpg_TvtvMap_t				stTvtvMap = {0, };
	DxTvtvService_t				stSvcInfo = {0, };

	if(hTree == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_SvcInfoSendToDama_tvtv] param fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	hError = UTIL_TREEUTIL_GetFirstChildItem(hTree, &hFirstChild);
	if(hError != ERR_OK || hFirstChild == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_SvcInfoSendToDama_tvtv] UTIL_TREEUTIL_GetFirstChildItem fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	hTempItem = hFirstChild;

	DAPI_BatchBegin(DxNAMEOF(DxTvtvService_t));

	while(hTempItem)
	{

		HxSTD_memset(&stTvtvMap, 0x00, sizeof(svcEpg_TvtvMap_t));
		HxSTD_memset(&stSvcInfo, 0x00, sizeof(DxTvtvService_t));

		hError = UTIL_TREEUTIL_GetItemUserInfo(hTempItem, sizeof(svcEpg_TvtvMap_t), (void*)&stTvtvMap);
		if(hError != ERR_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_SvcInfoSendToDama_tvtv] UTIL_TREEUTIL_GetNextItem fail!!\n");
			goto  END_TVTVMAP_LOOP;
		}

		stSvcInfo.usTvtvId = stTvtvMap.usTvtvId;
		stSvcInfo.usOnId = stTvtvMap.stTripleId.usOnId;
		stSvcInfo.usTsId = stTvtvMap.stTripleId.usTsId;
		stSvcInfo.usSvcId = stTvtvMap.stTripleId.usSvcId;
		stSvcInfo.ulSvcUid = stTvtvMap.ulSvcUid;


#if defined(TVTV_USE_SVCUID)
		// svcUid가 존재하는경우에만 보낸다.
		if(stTvtvMap.ulSvcUid == 0)
		{
			HxLOG_Debug("DAPI SET FAIL[xsvc_epgcontext_SvcInfoSendToDama_tvtv] serviceUid is empty..\n");
			goto END_TVTVMAP_LOOP;
		}
#endif

		if(stSvcInfo.usOnId==0 && stSvcInfo.usTsId==0 && stSvcInfo.usSvcId==0)
		{
			HxLOG_Debug("DAPI SET FAIL[xsvc_epgcontext_SvcInfoSendToDama_tvtv] triple is empty..\n");
			goto END_TVTVMAP_LOOP;
		}

#if defined(TEMP_DBG_RPINT)
		xsvc_epgcontext_PrintTvtvMapInfo(eTVTVMAP_SENDTODAMA, &stTvtvMap, 0);
#endif

		if(DAPI_Set(DxNAMEOF(DxTvtvService_t), stSvcInfo.ulSvcUid, &stSvcInfo, sizeof(DxTvtvService_t)) != ERR_OK)
		{
			HxLOG_Debug("DAPI SET FAIL[xsvc_epgcontext_SvcInfoSendToDama_tvtv]~~~~~~~~~~~~~~~~~~~~~~~~\n");
		}

END_TVTVMAP_LOOP:
		hError = UTIL_TREEUTIL_GetNextItem(hTempItem, &hTempItem);
		if(hError != ERR_OK )
		{
			HxLOG_Error("[xsvc_epgcontext_SvcInfoSendToDama_tvtv] UTIL_TREEUTIL_GetNextItem fail!!\n");
			break;
		}
	}

	DAPI_BatchEnd(DxNAMEOF(DxTvtvService_t));
	DAPI_Sync(DxNAMEOF(DxTvtvService_t));

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :

	return hError;
}

STATIC HERROR xsvc_epgcontext_EventSendToDama_Tvtv(DxTvtvEvent_t *pTvtvEvent)
{
	HERROR		hError = ERR_EPG_FAIL;
	HUID		eventUid = 0;
	HUINT32 		ulBinSize = sizeof(DxTvtvEventBase_t);
	HUINT8 * 	ucBinBuf = NULL;
	HUINT32 		i = 0, ulSD = 0;

	if(pTvtvEvent==NULL)
	{
		goto EXIT_CNXTTVTV_FUNC;
	}


#ifdef CONFIG_FUNC_EPG_USE_SVCUID
	eventUid = DxEVENT_UID(pTvtvEvent->stBase.ulSvcUid, pTvtvEvent->stBase.ulProgId);
#else
	eventUid = (HUID)(((HUID)eDxEPG_UID_TVTV) << 48 |  pTvtvEvent->stBase.ulProgId);
#endif

	for(i = eDxTVTVEPG_TEXT_NAME; i< eDxTVTVEPG_TEXT_MAX; i++)
	{
		if(pTvtvEvent->stText[i].usSize)	ulBinSize += (pTvtvEvent->stText[i].usSize+1+sizeof(HUINT16));
		else ulBinSize+= sizeof(HUINT16);
	}

	ulSD = HLIB_RWSTREAM_Open(ucBinBuf, ulBinSize);
	HLIB_RWSTREAM_Write(ulSD, (void *)&(pTvtvEvent->stBase) , sizeof(DxTvtvEventBase_t));


	for(i = eDxTVTVEPG_TEXT_NAME; i< eDxTVTVEPG_TEXT_MAX; i++)
	{
		HUINT16 usSize = pTvtvEvent->stText[i].usSize?(pTvtvEvent->stText[i].usSize+1):0;
		HLIB_RWSTREAM_Write(ulSD, (void *)&usSize , sizeof(HUINT16));
		if(pTvtvEvent->stText[i].usSize) HLIB_RWSTREAM_Write(ulSD, (void *)pTvtvEvent->stText[i].ucStr , pTvtvEvent->stText[i].usSize+1);
	}
#if defined(TEMP_DBG_RPINT)
	xsvc_epgcontext_PrintEventData(pTvtvEvent, ulBinSize);
#endif
	if(DAPI_Set(DxNAMEOF(eDxEPG_TYPE_TVTV), eventUid, HLIB_RWSTREAM_GetBuf(ulSD), ulBinSize) != ERR_OK)
	{
		HxLOG_Print("[xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv] dapi : set fail\n");
	}

	HLIB_RWSTREAM_Close(ulSD);


#if 1 // only free // 다른 곳에서 FREE 해야함. 추후 작업 진행.
	for(i = eDxTVTVEPG_TEXT_NAME; i< eDxTVTVEPG_TEXT_MAX; i++)
	{
		if(pTvtvEvent->stText[i].ucStr)
		{
			TVTVEPG_MemFree(pTvtvEvent->stText[i].ucStr);
			pTvtvEvent->stText[i].usSize = 0;
			pTvtvEvent->stText[i].ucStr = NULL;
		}
	}
#endif

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC:
	return hError;
}

#if defined(TVTV_ADD_USR_TP)
STATIC HERROR xsvc_epgcontext_IsExistTVTVEPG(HUINT16 usTsId, HUINT16 usSvcId)
{
	HUINT32	i=0;
	HBOOL	bFound = FALSE;

	for(i=0; i<TVTV_TRIPLE_ID_NUM; i++)
	{
#if 0
		if(s_stTvtv_TripleId[i].usTsId == usTsId && s_stTvtv_TripleId[i].usSvcId == usSvcId)
#else
		if(s_stTvtv_TripleId[i].usSvcId == usSvcId)
#endif
		{
			bFound = TRUE;
			break;
		}
	}

	if(bFound == FALSE)
		return ERR_EPG_FAIL;
	else
		return ERR_OK;
}
#endif

STATIC HERROR xsvc_epgcontext_RegisterEventInfo_Tvtv(HTREEMGR hTree)
{
	HERROR						hError = ERR_EPG_FAIL;
	HTREEITEM					hFirstChild = NULL;
	HTREEITEM					hTempItem = NULL;
	void							*pData = NULL;
	svcEpg_TvtvMap_t				stTvtvMap = {0, };
	POINTER_LIST_T				*pEventList = NULL;
	POINTER_LIST_T				*pTempEventList = NULL;
	SvcEpg_Event_t				*pEvent = NULL;
	svcEpg_TvtvEventList_t			*pTvtvService = NULL;
	DxTvtvEvent_t				stTvtvEvent;
#if defined(TVTV_USE_TRIPLEID)
	HUINT16						usMultiTsIdx= 0;
	HUINT16						usMultiTsCnt = 0;
#endif

	if(hTree == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_RegisterEvent_Tvtv] param fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	hError = UTIL_TREEUTIL_GetFirstChildItem(hTree, &hFirstChild);
	if(hError != ERR_OK || hFirstChild == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_RegisterEvent_Tvtv] UTIL_TREEUTIL_GetFirstChildItem fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	hTempItem = hFirstChild;
	while(hTempItem)
	{
		pData = NULL;
		pEventList = NULL;
		pTvtvService = NULL;

		HxSTD_memset(&stTvtvEvent, 0, sizeof(DxTvtvEvent_t));
		HxSTD_memset(&stTvtvMap, 0x00, sizeof(svcEpg_TvtvMap_t));

		hError = UTIL_TREEUTIL_GetItemUserInfo(hTempItem, sizeof(svcEpg_TvtvMap_t), (void*)&stTvtvMap);
		if(hError != ERR_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_RegisterEvent_Tvtv] UTIL_TREEUTIL_GetNextItem fail!!\n");
			goto  END_TVTVMAP_LOOP;
		}

#if defined(TEMP_DBG_RPINT)
		xsvc_epgcontext_PrintTvtvMapInfo(eTVTVMAP_VALIDEVENT, &stTvtvMap, 0);
#endif
#if defined(TVTV_USE_SVCUID)
		hError = xsvc_epgcontext_ValidTvtvMapInfo_Tvtv(&stTvtvMap);
		if(hError != ERR_OK)
		{
			hError = xsvc_epgcontext_GetSvcInfoFromSvcList(stTvtvMap.usTvtvId, &stTvtvMap.stTripleId.usOnId, &stTvtvMap.stTripleId.usTsId, &stTvtvMap.stTripleId.usSvcId, &stTvtvMap.ulSvcUid);
			if(hError != ERR_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_RegisterEventInfo_Tvtv] xsvc_epgcontext_ValidTvtvMapInfo_Tvtv fail!!\n");
				goto  END_TVTVMAP_LOOP;
			}
		}
#endif
		hError = UTIL_TREEUTIL_GetItemData(hTempItem, &pData);
		if(hError != ERR_OK || pData == NULL)
		{
			HxLOG_Error("[xsvc_epgcontext_RegisterEvent_Tvtv] UTIL_TREEUTIL_GetNextItem fail!!\n");
			goto  END_TVTVMAP_LOOP;
		}

		pTvtvService = (svcEpg_TvtvEventList_t*)pData;
		if(pTvtvService == NULL)
		{
			HxLOG_Print("[xsvc_epgcontext_RegisterEvent_Tvtv] Empty pTvtvService!!\n");
			goto  END_TVTVMAP_LOOP;
		}
		if(pTvtvService->pstEventList == NULL && pTvtvService->pstExtList == NULL)
		{
			HxLOG_Print("[xsvc_epgcontext_RegisterEvent_Tvtv] Empty serivce!!\n");
			goto  END_TVTVMAP_LOOP;
		}

		pEventList = pTvtvService->pstEventList;
		pTempEventList= pEventList;

#if defined(TVTV_USE_TRIPLEID)
		hError = xsvc_epgcontext_GetMultiTsCntFromTvtvId_Tvtv(stTvtvMap.usTvtvId, &usMultiTsCnt);
		if(hError != ERR_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_RegisterEventInfo_Tvtv] xsvc_epgcontext_GetMultiTsCntFromTvtvId_Tvtv fail!!\n");
			goto  END_TVTVMAP_LOOP;
		}

		for(usMultiTsIdx=0; usMultiTsIdx<usMultiTsCnt; usMultiTsIdx++)
		{
			stTvtvMap.stTripleId.usOnId = 0;
			stTvtvMap.stTripleId.usTsId = 0;
			stTvtvMap.stTripleId.usSvcId = 0;
			stTvtvMap.ulSvcUid = 0;
			hError = xsvc_epgcontext_GetSvcInfoFromSvcInfoList_Tvtv(usMultiTsIdx, &stTvtvMap.stTripleId.usOnId, &stTvtvMap.stTripleId.usTsId, &stTvtvMap.stTripleId.usSvcId,  &stTvtvMap.ulSvcUid);

#if defined(TEMP_DBG_RPINT)
			//xsvc_epgcontext_PrintTvtvMapInfo(eTVTVMAP_VALIDEVENT2, &stTvtvMap, 0);
#endif

			if(hError != ERR_OK)
			{
				HxLOG_Print("[xsvc_epgcontext_RegisterEvent_Tvtv] xsvc_epgcontext_GetSvcInfoFromSvcInfoList_Tvtv fail!!\n");
				goto  END_TVTVMAP_LOOP;
			}
#endif
			DAPI_BatchBegin(DxEPG_NAMEOF(eDxEPG_TYPE_TVTV));

			while(pTempEventList)
			{
				pEvent = NULL;
				pEvent = (SvcEpg_Event_t*)UTIL_LINKEDLIST_GetContents(pTempEventList);

				if(pEvent == NULL)
				{
					DAPI_BatchEnd(DxEPG_NAMEOF(eDxEPG_TYPE_TVTV));
					goto  END_TVTVMAP_LOOP;
				}

				if(stTvtvMap.usTvtvId != pEvent->stEventInfo.stTvtvEventInfo.usTvtvId)
				{
					DAPI_BatchEnd(DxEPG_NAMEOF(eDxEPG_TYPE_TVTV));
					goto  END_TVTVMAP_LOOP;
				}

				if(pEvent != NULL)
				{

					hError = xsvc_epgcontext_GetDxTvtvEvent(pEvent, &stTvtvEvent, &stTvtvMap);
					if(hError != ERR_OK)
					{
						DAPI_BatchEnd(DxEPG_NAMEOF(eDxEPG_TYPE_TVTV));
						HxLOG_Error("[xsvc_epgcontext_RegisterEvent_Tvtv] xsvc_epgcontext_GetDxTvtvEvent fail!!\n");
						goto  END_TVTVMAP_LOOP;
					}

#if defined(TEMP_DBG_RPINT)
					xsvc_epgcontext_PrintTvtvMapInfo(eTVTVMAP_SENDEVENT, &stTvtvMap, 0);
#endif
					hError= xsvc_epgcontext_EventSendToDama_Tvtv(&stTvtvEvent);
					if(hError != ERR_OK)
					{
						DAPI_BatchEnd(DxEPG_NAMEOF(eDxEPG_TYPE_TVTV));
						HxLOG_Error("[xsvc_epgcontext_RegisterEvent_Tvtv] xsvc_epgcontext_EventSendToDama_Tvtv fail!!\n");
						goto  END_TVTVMAP_LOOP;
					}
				}
				pTempEventList = UTIL_LINKEDLIST_GetNextPointer(pTempEventList);
			}

			DAPI_BatchEnd(DxEPG_NAMEOF(eDxEPG_TYPE_TVTV));
			HLIB_STD_TaskSleep(1000);
#if defined(TVTV_USE_TRIPLEID)
		}
#endif

		if(hError != ERR_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_RegisterEvent_Tvtv] EPGDB_AddEventList fail!!\n");
			goto  END_TVTVMAP_LOOP;
		}

END_TVTVMAP_LOOP:
		hError = UTIL_TREEUTIL_GetNextItem(hTempItem, &hTempItem);
		if(hError != ERR_OK )
		{
			HxLOG_Error("[xsvc_epgcontext_RegisterEvent_Tvtv] UTIL_TREEUTIL_GetNextItem fail!!\n");
			break;
		}
	}

#if 1
	// dama로 event를 보내면 tree를 remove해야한다. 그런데 삭제할지는 다시 검토하자.
	// free가 안되는 문제로인해 여기서 모든 tree를 remove함.
	hError = UTIL_TREEUTIL_RemoveItems(hTree, hFirstChild);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_RegisterEvent_Tvtv] UTIL_TREEUTIL_RemoveItems fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}
#endif

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :

	return hError;
}

STATIC HERROR xsvc_epgcontext_CheckAstraSvcInfo_Tvtv(void)
{
	HERROR			hError = ERR_EPG_FAIL;
	HBOOL			bFoundAstra = FALSE;
	DbSvc_AntInfo_t	stAntInfo = {0, };
	DbSvc_Attr_t		stSvcAttr;
	HINT32			lTotalAntNum = 0;
	HINT32			lAntCnt = 0;
	HINT32			lSvcCount = 0;
	HINT32			*plAntIdxArray = NULL;

	HxSTD_memset(&stAstraTvtv_Info, 0x00, sizeof(xsvcEpg_AstraTvTv_Info_t));

	DB_SVC_InitSvcAttribute (&stSvcAttr);
	stAstraTvtv_Info.ulOrgDeliType = eDxDELIVERY_TYPE_SAT;

	hError = DB_SVC_FindAllAntIdxList (&lTotalAntNum, &plAntIdxArray);
	if (hError == ERR_OK)
	{
		for(lAntCnt = 0; lAntCnt < lTotalAntNum; lAntCnt++)
		{
			hError = DB_SVC_GetAntInfo((HUINT16)plAntIdxArray[lAntCnt], &stAntInfo);
			if (hError == ERR_OK)
			{
				if (stAntInfo.eSatType == eSatType_ASTRA_1)
				{
					bFoundAstra = TRUE;
					stAstraTvtv_Info.ucAntId = (HUINT32)plAntIdxArray[lAntCnt];
					break;
				}
			}
		}
		DB_SVC_FreeAntList (plAntIdxArray);
	}

	if(!bFoundAstra)
	{
		hError = ERR_EPG_FAIL;
		HxLOG_Error("[xsvc_epgcontext_LoadXMLFile_Tvtv] !bFoundAstra!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	stSvcAttr.eSvcType = eDxSVC_TYPE_TV;
	hError = DB_SVC_GetServiceCount(eSvcGroup_All, &stSvcAttr, &lSvcCount);
	if(hError != ERR_OK || lSvcCount == 0)
	{
		hError = ERR_EPG_FAIL;
		HxLOG_Info("[xsvc_epgcontext_LoadXMLFile_Tvtv] DB_SVC_GetServiceCount fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	hError = ERR_EPG_OK;
EXIT_CNXTTVTV_FUNC:

	return hError;
}

#define ____PROCESS_POSTPROCESS_FUNC____

STATIC PalDsmcc_File_t		*s_pstPostprocessDsmccBuffer = NULL;

STATIC HERROR				xsvc_epgcontext_PostprosessBuffer_Tvtv(HUINT32 ulParam1, HUINT32 ulParam2)
{
	HUINT32						ulMessage = 0;
	HERROR						hError = ERR_EPG_FAIL;
	Handle_t						hAction = HANDLE_NULL;
	svcEpg_Msg_t						*pstMsg = NULL;

	PalDsmcc_File_t					*pDsmccFileList = NULL;
	PalDsmcc_File_t					*pPostprocessingFile = NULL;
	PalDsmcc_File_t					*pCurrentDsmccFile = NULL;
	PalDsmcc_File_t					*pTempDsmccFile = NULL;
	PalDsmcc_File_t					*pPrevDsmccFile = NULL;

	xsvcEpg_TvtvFileType_e				eTvtvFileType = TVTV_FileType_MAX;
#if	0
	STATIC HUINT32				s_ulCurrentTick = 0;
	STATIC HUINT32				s_ulStartTick = 0;
#endif
	svcEpg_PostProcessInfo_t		stPostprocessInfo = {0, };
	svcEpg_XmlParseInfo_t			stXMLParseInfo = {0, };
	xsvcEpg_PostprocFile_t				*pPostprocessedFile = NULL;
	svcEpg_DownloadInfo_t 		stDownloadInfo = {0, };
	HBOOL						bFirstFile = FALSE;

#if	defined(CONFIG_TVTVEPG_LIVEDOWNLOAD)
	svcEpg_Msg_t						stMsg = {0,};
	HBOOL						bFirstPostprocessedFile = FALSE;
#endif
	HUINT32						ulWriteSize = 0;
	HUINT32						ulReadSize = 0;
	HUINT8						szFileName[256] = {0, };
	Handle_t						hFileHandle = (Handle_t)NULL;
	xsvcEpg_TvtvDsiTable_t				stDsiTable = {0, };
	HBOOL						bPass = FALSE;
	xsvcEpg_TvtvProcessState_e					eCurrentState =  TvtvProcess_Nothing;

	pstMsg = (svcEpg_Msg_t*)ulParam1;
	if(pstMsg == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_PostprosessBuffer_Tvtv] param fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

/**********************************************************************************
  *
  * get param info
  *
  **********************************************************************************/
	ulMessage = pstMsg->ulMessage;
	hAction = (Handle_t)pstMsg->ulParam1;
	pDsmccFileList = (PalDsmcc_File_t*)pstMsg->ulParam2;
	bFirstFile = (HBOOL)pstMsg->ulParam3;
	NOT_USED_PARAM(ulMessage);
	NOT_USED_PARAM(bFirstFile);
	if(pDsmccFileList == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_PostprosessBuffer_Tvtv] param fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}


/**********************************************************************************
  *
  * local buffering
  *
  **********************************************************************************/
	pTempDsmccFile = NULL;
	pPrevDsmccFile = NULL;
	pTempDsmccFile = pDsmccFileList;
	while(pTempDsmccFile)
	{
		pPrevDsmccFile = pTempDsmccFile;
		pTempDsmccFile = pTempDsmccFile->pNext;
		s_pstPostprocessDsmccBuffer = xsvc_epgcontext_AppendFile_Tvtv(s_pstPostprocessDsmccBuffer, pPrevDsmccFile);
	}

	hError = xsvc_epgcontext_GetPostprocessInfo_Tvtv(hAction, &stPostprocessInfo);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_PostprosessBuffer_Tvtv] xsvc_epgcontext_GetPostprocessInfo_Tvtv fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	hError = xsvc_epgcontext_GetXMLParseInfo_Tvtv(hAction, &stXMLParseInfo);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_PostprosessBuffer_Tvtv] xsvc_epgcontext_GetXMLParseInfo_Tvtv fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}


/**********************************************************************************
  *
  * local buffer processing
  *
  **********************************************************************************/
	pCurrentDsmccFile = s_pstPostprocessDsmccBuffer;
	while(pCurrentDsmccFile)
	{
		eTvtvFileType = TVTV_FileType_MAX;
		pPostprocessingFile = pCurrentDsmccFile;
		pCurrentDsmccFile = pCurrentDsmccFile->pNext;

#if		defined(CONFIG_TVTV_CONTROL)
		hError = xsvc_epgcontext_ControlAction_Tvtv(EPG_CMD_START, TRUE, &bPass);
		if(bPass == TRUE)
		{
			HxLOG_Info("[xsvc_epgcontext_PostprosessBuffer_Tvtv] bPass == TRUE !!\n");
			break;
		}
#endif


/**********************************************************************************
  *
  * case 1 : state of system info set(dgkt, tvst)
  *
  **********************************************************************************/
		if(stPostprocessInfo.bSetDgkt == TRUE && stPostprocessInfo.bSetTvst == TRUE)
		{
			pPostprocessedFile = NULL;

			hError = xsvc_epglib_GetTvtvFileType_Tvtv(pPostprocessingFile->pucModuleName, &eTvtvFileType);
			if(hError != ERR_OK || eTvtvFileType == TVTV_FileType_MAX)
			{
				HxLOG_Error("[xsvc_epgcontext_PostprosessBuffer_Tvtv] xsvc_epglib_GetTvtvFileType_Tvtv fail!!\n");
				goto  END_DSMCCFILE_LOOP;
			}

			switch(eTvtvFileType)
			{
				case TVTV_FileType_LTOT :
				case TVTV_FileType_TSMT :
				case TVTV_FileType_EPG_EVENT :
#if				defined(CONFIG_TVTVEPG_LOGO)
				case TVTV_FileType_LOGO :
#endif
				case TVTV_FileType_EPG_IMAGE :
					hError = xsvc_epglib_Postprocess_Tvtv(pPostprocessingFile, eTvtvFileType, TRUE, &pPostprocessedFile);
					if(hError != ERR_OK || pPostprocessedFile == NULL)
					{
						HxLOG_Error("[xsvc_epgcontext_PostprosessBuffer_Tvtv] xsvc_epglib_Postprocess_Tvtv fail!!\n");
						break;
					}
#if defined(TVTV_EPGTBL_NOT_SAVE_IN_HDD)
	#if defined(TVTV_NEW_ONE)
					if(eTvtvFileType != TVTV_FileType_EPG_EVENT && eTvtvFileType != TVTV_FileType_LTOT && eTvtvFileType != TVTV_FileType_LOGO)
	#else
					if(eTvtvFileType != TVTV_FileType_EPG_EVENT)
	#endif
#endif
					hError = xsvc_epgcontext_SavePostprocessedFile_Tvtv(pPostprocessedFile);
					if(hError != ERR_OK)
					{
						HxLOG_Error("[xsvc_epgcontext_PostprosessBuffer_Tvtv] xsvc_epglib_Postprocess_Tvtv fail!!\n");
						if(pPostprocessedFile)
						{
							if(pPostprocessedFile->pucFileName)
							{
								OxDSMCC_Free(pPostprocessedFile->pucFileName);
							}
							if(pPostprocessedFile->pucFilePointer)
							{
								OxDSMCC_Free(pPostprocessedFile->pucFilePointer);
							}
							OxEPG_Free(pPostprocessedFile);
							pPostprocessedFile = NULL;
						}
						break;
					}

#if					defined(CONFIG_TVTVEPG_LIVEDOWNLOAD)

					if(	eTvtvFileType == TVTV_FileType_LTOT
						|| eTvtvFileType == TVTV_FileType_EPG_IMAGE
						|| eTvtvFileType == TVTV_FileType_LOGO
					)
					{
						if(pPostprocessedFile)
						{
							if(pPostprocessedFile->pucFileName)
							{
								OxDSMCC_Free(pPostprocessedFile->pucFileName);
							}
							if(pPostprocessedFile->pucFilePointer)
							{
								OxDSMCC_Free(pPostprocessedFile->pucFilePointer);
							}
							OxEPG_Free(pPostprocessedFile);
							pPostprocessedFile = NULL;
						}
						break;
					}

					bFirstPostprocessedFile = FALSE;
					if(stPostprocessInfo.ulSendPostprocessedFile == 0)
					{
						bFirstPostprocessedFile = TRUE;
					}
					HxSTD_memset(&stMsg, 0x00, sizeof(svcEpg_Msg_t));
					stMsg.ulMessage = eEPG_MSG_TVTV_PARSEBUFFER;
					stMsg.ulParam1 = (HUINT32)hAction;
					stMsg.ulParam2 = (HUINT32)pPostprocessedFile;
					stMsg.ulParam3 = (HBOOL)bFirstPostprocessedFile;
#if 1
					xsvc_epgcontext_ProcessXMLParseBuffer_Tvtv((HUINT32)&stMsg, 0);
			#if 1 // tvtv epg에 의한 memory leak 수정사항.
					if(pPostprocessedFile)
					{
						if(pPostprocessedFile->pucFileName)
						{
							OxDSMCC_Free(pPostprocessedFile->pucFileName);
						}
						if(eTvtvFileType == TVTV_FileType_EPG_IMAGE)
						{
							if(pPostprocessedFile->pucFilePointer)
							{
								OxMW_Free(pPostprocessedFile->pucFilePointer);
							}
						}
						else
						{
							if(pPostprocessedFile->pucFilePointer)
							{
								OxEPG_Free(pPostprocessedFile->pucFilePointer);
							}
						}
						OxEPG_Free(pPostprocessedFile);
						pPostprocessedFile = NULL;
					}
			#endif
#else
					hError = UTIL_TASK_SendMessage(stXMLParseInfo.ucTaskSlot, &stMsg);
					if(hError != ERR_TASKUTIL_OK)
					{
						HxLOG_Error("[xsvc_epgcontext_ProcessDdb_Tvtv] SendMessage fail!!\n");
					}
#endif
					pPostprocessedFile = NULL;
					stPostprocessInfo.ulSendPostprocessedFile++;
#else
					if(pPostprocessedFile)
					{
						if(pPostprocessedFile->pucFileName)
						{
							OxDSMCC_Free(pPostprocessedFile->pucFileName);
						}
						if(eTvtvFileType == TVTV_FileType_EPG_IMAGE)
						{
							if(pPostprocessedFile->pucFilePointer)
							{
								OxMW_Free(pPostprocessedFile->pucFilePointer);
							}
						}
						else
						{
							if(pPostprocessedFile->pucFilePointer)
							{
								OxEPG_Free(pPostprocessedFile->pucFilePointer);
							}
						}
						OxEPG_Free(pPostprocessedFile);
						pPostprocessedFile = NULL;
					}
#endif
					break;

				default :
					break;
			}
		}
/**********************************************************************************
  *
  * case 2 : state of not yet system info set(dgkt, tvst)
  *             buffering another info (s_pstPostprocessDsmccBuffer)
  *
  **********************************************************************************/
		else
		{

			hError = xsvc_epglib_GetTvtvFileType_Tvtv(pPostprocessingFile->pucModuleName, &eTvtvFileType);
			if(hError != ERR_OK || eTvtvFileType == TVTV_FileType_MAX)
			{
				//HxLOG_Error("[xsvc_epgcontext_PostprosessBuffer_Tvtv] xsvc_epglib_GetTvtvFileType_Tvtv fail!!\n");
				goto  END_DSMCCFILE_LOOP;
			}

			switch(eTvtvFileType)
			{
				case TVTV_FileType_TVST :
					HxLOG_Info("[xsvc_epgcontext_PostprosessBuffer_Tvtv] TVTV_FileType_TVST +!!\n");
					hError = xsvc_epglib_SetTvstInfo_Tvtv(pPostprocessingFile->pucFilePointer, pPostprocessingFile->ulModuleSize);
					if(hError != ERR_OK)
					{
						HxLOG_Error("[xsvc_epgcontext_PostprosessBuffer_Tvtv] xsvc_epglib_SetTvstInfo_Tvtv fail!!\n");
						break;
					}
					pPostprocessedFile = (xsvcEpg_PostprocFile_t*)OxEPG_Malloc(sizeof(xsvcEpg_PostprocFile_t));
					if(pPostprocessedFile == NULL)
					{
						HxLOG_Error("[xsvc_epgcontext_PostprosessBuffer_Tvtv] OxEPG_Malloc fail!!\n");
						break;
					}
					HxSTD_memset(pPostprocessedFile, 0x00, sizeof(xsvcEpg_PostprocFile_t));

					pPostprocessedFile->eFileType = eTvtvFileType;
					pPostprocessedFile->pucFilePointer = pPostprocessingFile->pucFilePointer;
					pPostprocessedFile->pucFileName = pPostprocessingFile->pucModuleName;
					pPostprocessedFile->ulFileSize = pPostprocessingFile->ulModuleSize;
					if(pPostprocessingFile)
					{
						pPostprocessingFile->pucFilePointer = NULL;
						pPostprocessingFile->pucModuleName = NULL;
					}

					hError = xsvc_epgcontext_SavePostprocessedFile_Tvtv(pPostprocessedFile);
					if(hError != ERR_OK)
					{
						HxLOG_Error("[xsvc_epgcontext_PostprosessBuffer_Tvtv] xsvc_epglib_Postprocess_Tvtv fail!!\n");
						if(pPostprocessedFile)
						{
							if(pPostprocessedFile->pucFileName)
							{
								OxDSMCC_Free(pPostprocessedFile->pucFileName);
							}
							if(pPostprocessedFile->pucFilePointer)
							{
								OxDSMCC_Free(pPostprocessedFile->pucFilePointer);
							}
							OxEPG_Free(pPostprocessedFile);
							pPostprocessedFile = NULL;
						}
						break;
					}

					if(pPostprocessedFile)
					{
						if(pPostprocessedFile->pucFileName)
						{
							OxDSMCC_Free(pPostprocessedFile->pucFileName);
						}
						if(pPostprocessedFile->pucFilePointer)
						{
							OxDSMCC_Free(pPostprocessedFile->pucFilePointer);
						}
						OxEPG_Free(pPostprocessedFile);
						pPostprocessedFile = NULL;
					}

					stPostprocessInfo.bSetTvst = TRUE;
					if(stPostprocessInfo.bSetDgkt == TRUE && stPostprocessInfo.bSetTvst == TRUE)
					{
						hError = xsvc_epglib_SetTvtvSystemInfo_Tvtv();
						if(hError != ERR_OK)
						{
							HxLOG_Error("[xsvc_epgcontext_PostprosessBuffer_Tvtv] xsvc_epglib_SetTvtvSystemInfo_Tvtv fail!!\n");
							break;
						}
					}
					HxLOG_Info("[xsvc_epgcontext_PostprosessBuffer_Tvtv] TVTV_FileType_TVST -!!\n");
					break;

				case TVTV_FileType_DGKT :
					HxLOG_Info("[xsvc_epgcontext_PostprosessBuffer_Tvtv] TVTV_FileType_DGKT +!!\n");
					hError = xsvc_epglib_SetDgktInfo_Tvtv(pPostprocessingFile->pucFilePointer, pPostprocessingFile->ulModuleSize, TRUE);
					if(hError != ERR_OK)
					{
						HxLOG_Error("[xsvc_epgcontext_PostprosessBuffer_Tvtv] xsvc_epglib_SetTvstInfo_Tvtv fail!!\n");
						break;
					}
					pPostprocessedFile = (xsvcEpg_PostprocFile_t*)OxEPG_Malloc(sizeof(xsvcEpg_PostprocFile_t));
					if(pPostprocessedFile == NULL)
					{
						HxLOG_Error("[xsvc_epgcontext_PostprosessBuffer_Tvtv] OxEPG_Malloc fail!!\n");
						break;
					}
					HxSTD_memset(pPostprocessedFile, 0x00, sizeof(xsvcEpg_PostprocFile_t));

					pPostprocessedFile->eFileType = eTvtvFileType;
					pPostprocessedFile->pucFilePointer = pPostprocessingFile->pucFilePointer;
					pPostprocessedFile->pucFileName = pPostprocessingFile->pucModuleName;
					pPostprocessedFile->ulFileSize = pPostprocessingFile->ulModuleSize;
					if(pPostprocessingFile)
					{
						pPostprocessingFile->pucFilePointer = NULL;
						pPostprocessingFile->pucModuleName = NULL;
					}

					hError = xsvc_epgcontext_SavePostprocessedFile_Tvtv(pPostprocessedFile);
					if(hError != ERR_OK)
					{
						HxLOG_Error("[xsvc_epgcontext_PostprosessBuffer_Tvtv] xsvc_epglib_Postprocess_Tvtv fail!!\n");
						if(pPostprocessedFile)
						{
							if(pPostprocessedFile->pucFileName)
							{
								OxDSMCC_Free(pPostprocessedFile->pucFileName);
							}
							if(pPostprocessedFile->pucFilePointer)
							{
								OxDSMCC_Free(pPostprocessedFile->pucFilePointer);
							}
							OxEPG_Free(pPostprocessedFile);
							pPostprocessedFile = NULL;
						}
						break;
					}

					if(pPostprocessedFile)
					{
						if(pPostprocessedFile->pucFileName)
						{
							OxDSMCC_Free(pPostprocessedFile->pucFileName);
						}
						if(pPostprocessedFile->pucFilePointer)
						{
							OxDSMCC_Free(pPostprocessedFile->pucFilePointer);
						}
						OxEPG_Free(pPostprocessedFile);
						pPostprocessedFile = NULL;
					}

					stPostprocessInfo.bSetDgkt = TRUE;
					if(stPostprocessInfo.bSetDgkt == TRUE && stPostprocessInfo.bSetTvst == TRUE)
					{
						hError = xsvc_epglib_SetTvtvSystemInfo_Tvtv();
						if(hError != ERR_OK)
						{
							HxLOG_Error("[xsvc_epgcontext_PostprosessBuffer_Tvtv] xsvc_epglib_SetTvtvSystemInfo_Tvtv fail!!\n");
							break;
						}
					}
					HxLOG_Info("[xsvc_epgcontext_PostprosessBuffer_Tvtv] TVTV_FileType_DGKT -!!\n");
					break;

				case TVTV_FileType_LTOT :
				case TVTV_FileType_TSMT :
				case TVTV_FileType_LOGO :
				case TVTV_FileType_EPG_IMAGE :
				case TVTV_FileType_EPG_EVENT :
					break;
				default :
					break;
			}
		}

END_DSMCCFILE_LOOP :
		pPostprocessingFile = NULL;
		stPostprocessInfo.ulNumOfPostprocessedFile++;
	}


/**********************************************************************************
  *
  * update download info
  *
  **********************************************************************************/
	hError = xsvc_epgcontext_GetDownloadInfo_Tvtv(hAction, &stDownloadInfo);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_PostprosessBuffer_Tvtv] xsvc_epgcontext_GetDownloadInfo_Tvtv fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

#if	0
	HxLOG_Info("[xsvc_epgcontext_PostprosessBuffer_Tvtv] bCompleteDownload(%d)ulEstimatedDownloadSize(%d)ulCurrentDownloadedSize(%d)!!\n",
																		stDownloadInfo.bCompleteDownload,
																		stDownloadInfo.ulEstimatedDownloadSize,
																		stDownloadInfo.ulCurrentDownloadedSize));
	HxLOG_Info("[xsvc_epgcontext_PostprosessBuffer_Tvtv] ulNumOfDownloadedFile(%d)ulNumOfPostprocessedFile(%d)!!\n",
																		stDownloadInfo.ulNumOfDownloadedFile,
																		stPostprocessInfo.ulNumOfPostprocessedFile));
#endif
/**********************************************************************************
  *
  * send to AP with download progress
  *
  **********************************************************************************/
	if(stDownloadInfo.ulCurrentDownloadedSize != 0 && stDownloadInfo.ulEstimatedDownloadSize != 0)
	{

#if defined(TEMP_DBG_RPINT)
		HLIB_CMD_Printf("Complete[%d] ############# TVTV Download[%d] : total size(%d):(%d)\n",stDownloadInfo.bCompleteDownload, ((stDownloadInfo.ulCurrentDownloadedSize*100)/stDownloadInfo.ulEstimatedDownloadSize), stDownloadInfo.ulEstimatedDownloadSize, stDownloadInfo.ulCurrentDownloadedSize);
#endif
		if(hError != ERR_TASKUTIL_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_PostprosessBuffer_Tvtv] SVC_EPG_MgrPostMessage fail!!\n");
		}

#if defined(TVTV_SUPPORT_DOWNLOAD_INTERVAL_SIZE)
		if(stDownloadInfo.bCompleteDownload == FALSE &&
		  ((stDownloadInfo.ulCurrentDownloadedSize*100)/stDownloadInfo.ulEstimatedDownloadSize) == 99 &&
		  (((stDownloadInfo.ulCurrentDownloadedSize+TVTV_INTERVAL_SIZE)*100)/stDownloadInfo.ulEstimatedDownloadSize) == 100)
		{
			stDownloadInfo.bCompleteDownload = TRUE;
		}
#endif

		if(stDownloadInfo.bCompleteDownload)
		{
#if defined(TEMP_DBG_RPINT)
			HLIB_CMD_Printf("========= TVTV Download Complete ===========\n");
			HLIB_CMD_Printf("========= TVTV Download Complete ===========\n");
			HLIB_CMD_Printf("========= TVTV Download Complete ===========\n");
			HLIB_CMD_Printf("\t - ulNumOfDownloadedFile%d), ulNumOfPostprocessedFile(%d)\n", stDownloadInfo.ulNumOfDownloadedFile, stPostprocessInfo.ulNumOfPostprocessedFile);
#endif
			if(stDownloadInfo.ulNumOfDownloadedFile <= stPostprocessInfo.ulNumOfPostprocessedFile)
			{
				#if 0
				if(SVC_EPG_MgrPostMessage(eSEVT_EPG_TVTVLASTUPDATE_TIME, 0, 0, 0, 0) != ERR_OK)
				{
					HxLOG_Error("SVC_EPG_MgrPostMessage() fail\n");
				}
				#endif
#if				defined(CONFIG_TVTVEPG_LIVEDOWNLOAD)
				stMsg.ulMessage = eEPG_MSG_TVTV_POSTPROCESSCOMPLETE;
				stMsg.ulParam1 = (HUINT32)hAction;
				hError = UTIL_TASK_SendMessage(stXMLParseInfo.ucTaskSlot, &stMsg);
				if(hError != ERR_TASKUTIL_OK)
				{
					HxLOG_Print("[xsvc_epgcontext_PostprosessBuffer_Tvtv] SendMessage fail!!\n");
				}
#endif

				HxLOG_Info("[xsvc_epgcontext_PostprosessBuffer_Tvtv] DSI save +!!\n");
				snprintf ((char *)szFileName, 256, "%s%s", EPG_FS_TVTV_DATA_PATH, TVTV_FILENAME_DSI_TABLE);
				hError = xsvc_epgcontext_Fopen_Tvtv(szFileName, &hFileHandle);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Error("[xsvc_epgcontext_PostprosessBuffer_Tvtv] xsvc_epgcontext_Fopen_Tvtv fail!!\n");
					goto EXIT_CNXTTVTV_FUNC;
				}
				hError = xsvc_epgcontext_Fread_Tvtv(&stDsiTable, sizeof(xsvcEpg_TvtvDsiTable_t), hFileHandle, ePAL_PB_SEEKPOSITION_START, &ulReadSize);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Error("[xsvc_epgcontext_PostprosessBuffer_Tvtv] xsvc_epgcontext_Fread_Tvtv fail!!\n");
					goto EXIT_CNXTTVTV_FUNC;
				}
				stDsiTable.ulComplete = TVTV_DSI_COMPLETE_MASK;

				hError = xsvc_epgcontext_Fwrite_Tvtv(&stDsiTable, sizeof(xsvcEpg_TvtvDsiTable_t), hFileHandle, ePAL_PB_SEEKPOSITION_START, &ulWriteSize);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Error("[xsvc_epgcontext_PostprosessBuffer_Tvtv] xsvc_epgcontext_Fwrite_Tvtv fail!!\n");
					goto EXIT_CNXTTVTV_FUNC;
				}

				hError = xsvc_epgcontext_Fclose_Tvtv(hFileHandle);
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Error("[xsvc_epgcontext_PostprosessBuffer_Tvtv] fclose fail!!\n");
				}
				HxLOG_Info("[xsvc_epgcontext_PostprosessBuffer_Tvtv] DSI Save -!!\n");
				HxLOG_Info("[xsvc_epgcontext_PostprosessBuffer_Tvtv] Complete (%d)\n", xsvc_epgcontext_GetSystemTime_Tvtv()%1000);

/**********************************************************************************
  *
  * send to AP with download done
  *
  **********************************************************************************/
				hError = SVC_EPG_MgrPostMessage(eSEVT_EPG_TVTV_DONE, 0, 0, 0, 0);
				if(hError != ERR_TASKUTIL_OK)
				{
					HxLOG_Error("[xsvc_epgcontext_PostprosessBuffer_Tvtv] SVC_EPG_MgrPostMessage fail!!\n");
				}
				hError = xsvc_epgcontext_InitControlProcess_Tvtv();
				if(hError != ERR_OK)
				{
					HxLOG_Error("[xsvc_epgcontext_PostprosessBuffer_Tvtv] xsvc_epgcontext_InitControlProcess_Tvtv fail!!\n");
				}
				// init download info
				stPostprocessInfo.bCompletePostprocess = TRUE;
			}
		}

	}

	hError = xsvc_epgcontext_SetPostprocessInfo_Tvtv(hAction, &stPostprocessInfo);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_PostprosessBuffer_Tvtv] xsvc_epgcontext_SetPostprocessInfo_Tvtv fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :

/**********************************************************************************
  *
  * postprocessed dsmcc file list free!!
  *
  **********************************************************************************/
	xsvc_epgcontext_GetControlProcess_Tvtv(&eCurrentState);

	if(	(stPostprocessInfo.bSetDgkt == TRUE && stPostprocessInfo.bSetTvst == TRUE && s_pstPostprocessDsmccBuffer != NULL)
		|| (eCurrentState != TvtvProcess_DownLoading)
	)
	{
		pTempDsmccFile = NULL;
		pPrevDsmccFile = NULL;

		pPrevDsmccFile = pTempDsmccFile = s_pstPostprocessDsmccBuffer;
		while(pTempDsmccFile)
		{
			pPrevDsmccFile = pTempDsmccFile;
			pTempDsmccFile = pTempDsmccFile->pNext;

			if(pPrevDsmccFile->pucModuleName)
			{
				OxDSMCC_Free(pPrevDsmccFile->pucModuleName);
			}
			if(pPrevDsmccFile->pucFilePointer)
			{
				OxDSMCC_Free(pPrevDsmccFile->pucFilePointer);
			}
			OxDSMCC_Free(pPrevDsmccFile);
			pPrevDsmccFile = NULL;
		}
		s_pstPostprocessDsmccBuffer = NULL;
	}


	return hError;
}

STATIC HERROR				xsvc_epgcontext_PostprosessFile_Tvtv(HUINT32 ulParam1, HUINT32 ulParam2)
{
#if 0
	HUINT32						ulMessage = 0;
	HERROR						hError = ERR_EPG_FAIL;
	Handle_t						hAction = HANDLE_NULL;
	svcEpg_Msg_t						*pstMsg = NULL;
	svcEpg_PostProcessInfo_t		stPostprocessInfo = {0, };


	pstMsg = (svcEpg_Msg_t*)ulParam1;
	if(pstMsg == NULL)
	{
		HxLOG_Print("[xsvc_epgcontext_PostprosessFile_Tvtv] param fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

EXIT_CNXTTVTV_FUNC :

	return hError;
#endif
	return ERR_OK;
}

#define ____PROCESS_XML_FUNC____
#if defined(CONFIG_TVTVEPG_LIVEDOWNLOAD)
STATIC HERROR				xsvc_epgcontext_ProcessXMLParse_Tvtv(HUINT32 ulParam1, HUINT32 ulParam2)
{
	HUINT32						ulMessage = 0;
	HERROR						hError = ERR_EPG_FAIL;
	svcEpg_Msg_t						*pstMsg = NULL;

	pstMsg = (svcEpg_Msg_t*)ulParam1;
	if(pstMsg == NULL)
	{
		HxLOG_Print("[xsvc_epgcontext_ProcessXMLParse_Tvtv] param fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	ulMessage = pstMsg->ulMessage;
	switch(ulMessage)
	{
		case eEPG_MSG_TVTV_PARSEBUFFER :
			hError= xsvc_epgcontext_ProcessXMLParseBuffer_Tvtv(ulParam1, ulParam2);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_ProcessXMLParse_Tvtv] xsvc_epgcontext_ProcessXMLParseBuffer_Tvtv fail!!\n");
			}
			break;

		case eEPG_MSG_TVTV_POSTPROCESSCOMPLETE :
			hError= xsvc_epgcontext_ProcessXMLParseBuffer_Tvtv(ulParam1, ulParam2);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_ProcessXMLParse_Tvtv] xsvc_epgcontext_ProcessXMLParseBuffer_Tvtv fail!!\n");
			}
			hError= xsvc_epgcontext_ProcessXMLParseFile_Tvtv(ulParam1, ulParam2);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_ProcessXMLParse_Tvtv] xsvc_epgcontext_ProcessXMLParseFile_Tvtv fail!!\n");
			}
			break;

		default :
			HxLOG_Error("[xsvc_epgcontext_ProcessXMLParse_Tvtv] default fail!!\n");
			break;

	}


	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :

	return hError;

}

STATIC HERROR				xsvc_epgcontext_ProcessXMLParseFile_Tvtv(HUINT32 ulParam1, HUINT32 ulParam2)
{
	return ERR_OK;
}
#endif

#if !defined(TVTV_NEW_ONE)
STATIC HTREEMGR			hTvtvTree = NULL;
#endif

#if defined(CONFIG_TVTVEPG_LIVEDOWNLOAD)
#if defined(TVTV_NEW_ONE)
HERROR xsvc_epgcontext_GetSvcInfoFormTvtvId(HUINT32 ulTvtvId,  xsvcEpg_TvtvSvcInfo_t *pstSvcInfo)
{
	HERROR hErr = ERR_FAIL;
	HUINT16 i = 0;

	if(pstSvcInfo == NULL)
	{
		HxLOG_Error("pstSvcInfo is NULL\n");
		return ERR_FAIL;
	}

	for(i=0;i<stTvtvSvcInfoList_Temp.channel_cnt;i++)
	{
		if(stTvtvSvcInfoList_Temp.stTvtvSvcInfo_Temp[i].ulTvtvId == ulTvtvId)
		{
			HxSTD_memcpy(pstSvcInfo, &stTvtvSvcInfoList_Temp.stTvtvSvcInfo_Temp[i], sizeof(xsvcEpg_TvtvSvcInfo_t));
			hErr = ERR_OK;
			break;
		}
	}

	return hErr;
}

HBOOL xsvc_epgcontext_IsExistTvtvIdInSvcInfo(HUINT32 ulTvtvId)
{
	HUINT16 i = 0;
	HBOOL	bExist = FALSE;

	for(i=0;i<stTvtvSvcInfoList_Temp.channel_cnt;i++)
	{
		if(stTvtvSvcInfoList_Temp.stTvtvSvcInfo_Temp[i].ulTvtvId == ulTvtvId)
		{
			bExist = TRUE;
			break;
		}
	}

	return bExist;
}

HBOOL xsvc_epgcontext_GetSvcUidFromTripleId(HUINT32 ulTvtvId, HUINT16 usOnId, HUINT16 usTsId, HUINT16 usSvcId, HUINT32 *pUsSvcUid)
{
	HUINT16 i = 0;
	HBOOL bSearched = FALSE;

	*pUsSvcUid= 0;
	for(i=0;i<stTvtvSvcInfoList_Temp.channel_cnt;i++)
	{
		if( (stTvtvSvcInfoList_Temp.stTvtvSvcInfo_Temp[i].stTripleId.usOnId == usOnId) 		&&
		    (stTvtvSvcInfoList_Temp.stTvtvSvcInfo_Temp[i].stTripleId.usTsId == usTsId) 		&&
		    (stTvtvSvcInfoList_Temp.stTvtvSvcInfo_Temp[i].stTripleId.usSvcId == usSvcId)	)
		{
			*pUsSvcUid = stTvtvSvcInfoList_Temp.stTvtvSvcInfo_Temp[i].ulUid;
			stTvtvSvcInfoList_Temp.stTvtvSvcInfo_Temp[i].ulTvtvId = ulTvtvId;		// event data를 보낼경우 tvtv map을 상실함으로 tvtv id를 다시 전달한다.
			bSearched = TRUE;
			break;
		}
	}

	return bSearched;
}

HERROR xsvc_epgcontext_GetDxTvtvEventFromEvent(SvcEpg_Event_t *pEvent, DxTvtvEvent_t *pDxEvent, xsvcEpg_TvtvSvcInfo_t *pSvInfo)
{
	HERROR		hError = ERR_EPG_FAIL;
//	HUINT8		*pucConvertString = NULL;

	if(pEvent==NULL || pDxEvent==NULL || pSvInfo==NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_GetDxTvtvEvent] param fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	pDxEvent->stBase.type = eDxEPG_TYPE_TVTV;
	pDxEvent->stBase.usVer = pEvent->stEventInfo.stTvtvEventInfo.usVersion;
	pDxEvent->stBase.usTvtvId = pEvent->stEventInfo.stTvtvEventInfo.usTvtvId;
	pDxEvent->stBase.ulProgId = pEvent->stEventInfo.stTvtvEventInfo.ulProgId;
	pDxEvent->stBase.usEventId = pEvent->stEventInfo.stTvtvEventInfo.nEventId;
	pDxEvent->stBase.ulSvcUid = pSvInfo->ulUid;
	pDxEvent->stBase.usOnId = pSvInfo->stTripleId.usOnId;
	pDxEvent->stBase.usTsId = pSvInfo->stTripleId.usTsId;
	pDxEvent->stBase.usSvcId = pSvInfo->stTripleId.usSvcId;
	pDxEvent->stBase.ulStartTime = pEvent->ulStartTime;
	pDxEvent->stBase.ulEndTime = pEvent->ulStartTime + pEvent->ulDuration;
	pDxEvent->stBase.ucRating = pEvent->stEventInfo.stTvtvEventInfo.astParentalRating[0].ucParentalRating;
	pDxEvent->stBase.ulSeason = pEvent->stEventInfo.stTvtvEventInfo.ulSeason;
	pDxEvent->stBase.ulSeriesId = pEvent->stEventInfo.stTvtvEventInfo.ulSeriesId;
	pDxEvent->stBase.ulTotalNum = pEvent->stEventInfo.stTvtvEventInfo.ulTotalNum;

	hError = xsvc_epglib_SetDataByType_Tvtv(	TVTV_TYPE_DECIMAL,
														pEvent->stEventInfo.stTvtvEventInfo.aucSeriesNumber,
														NULL, 0, 0, NULL,
														&pDxEvent->stBase.ulEpisodeNum);

	HxSTD_memcpy(pDxEvent->stBase.astEventLink, pEvent->stEventInfo.stTvtvEventInfo.astEventLink, sizeof(SvcEpg_EventLink_t)*EPG_TVTV_LINK_MAX);
	pDxEvent->stBase.aucContent[0] = pEvent->aucContent[0];
	pDxEvent->stBase.ucTip = pEvent->stEventInfo.stTvtvEventInfo.ucTip;

	HxSTD_memcpy(pDxEvent->stBase.ucLang, pEvent->stEventInfo.stTvtvEventInfo.aucIsoCountryCode, EPG_TVTV_MAX_LANG);

#if 1 // tvtv epg에 의한 memory leak 수정사항.
	pDxEvent->stText[eDxTVTVEPG_TEXT_NAME].ucStr = TVTVEPG_StrDup(&pEvent->ucEventName[0]);
	if(pDxEvent->stText[eDxTVTVEPG_TEXT_NAME].ucStr)
	{
		pDxEvent->stText[eDxTVTVEPG_TEXT_NAME].usSize= HxSTD_StrLen(pDxEvent->stText[eDxTVTVEPG_TEXT_NAME].ucStr);
	}

	pDxEvent->stText[eDxTVTVEPG_TEXT_SHORT].ucStr = TVTVEPG_StrDup(&pEvent->ucEventText[0]);
	if(pDxEvent->stText[eDxTVTVEPG_TEXT_SHORT].ucStr)
	{
		pDxEvent->stText[eDxTVTVEPG_TEXT_SHORT].usSize= HxSTD_StrLen(pDxEvent->stText[eDxTVTVEPG_TEXT_SHORT].ucStr);
	}

	pDxEvent->stText[eDxTVTVEPG_TEXT_DETAIL].ucStr = TVTVEPG_StrDup(&pEvent->ucEventDText[0]);
	if(pDxEvent->stText[eDxTVTVEPG_TEXT_DETAIL].ucStr)
	{
		pDxEvent->stText[eDxTVTVEPG_TEXT_DETAIL].usSize= HxSTD_StrLen(pDxEvent->stText[eDxTVTVEPG_TEXT_DETAIL].ucStr);
	}
#else
	//xsvc_epgcontext_ConvertEpgTextToString(pEvent->astShortText[0].pEventName, &pucConvertString);
	pDxEvent->stText[eDxTVTVEPG_TEXT_NAME].ucStr = TVTVEPG_StrDup(pEvent->astShortText[0].pEventName->aucTextChar);
	if(pDxEvent->stText[eDxTVTVEPG_TEXT_NAME].ucStr)
	{
		pDxEvent->stText[eDxTVTVEPG_TEXT_NAME].usSize= HxSTD_StrLen(pDxEvent->stText[eDxTVTVEPG_TEXT_NAME].ucStr);
	}

//	xsvc_epgcontext_ConvertEpgTextToString(pEvent->astShortText[0].pEventText, &pucConvertString);
	pDxEvent->stText[eDxTVTVEPG_TEXT_SHORT].ucStr = TVTVEPG_StrDup(pEvent->astShortText[0].pEventText->aucTextChar);
	if(pDxEvent->stText[eDxTVTVEPG_TEXT_SHORT].ucStr)
	{
		pDxEvent->stText[eDxTVTVEPG_TEXT_SHORT].usSize= HxSTD_StrLen(pDxEvent->stText[eDxTVTVEPG_TEXT_SHORT].ucStr);
	}

	pDxEvent->stText[eDxTVTVEPG_TEXT_DETAIL].ucStr = TVTVEPG_StrDup(pEvent->astShortText[0].pEventDText->aucTextChar);
	if(pDxEvent->stText[eDxTVTVEPG_TEXT_DETAIL].ucStr)
	{
		pDxEvent->stText[eDxTVTVEPG_TEXT_DETAIL].usSize= HxSTD_StrLen(pDxEvent->stText[eDxTVTVEPG_TEXT_DETAIL].ucStr);
	}
#endif

	if(strstr(pEvent->stEventInfo.stTvtvEventInfo.aucImageName, ".jpg"))
	{
		HUINT8 buf[128];
		HxSTD_memset(&buf, 0, sizeof(buf));
		snprintf((char *)buf, 100, "%s/%s", TVTVEPG_THUMBNAIL_PATH, pEvent->stEventInfo.stTvtvEventInfo.aucImageName);
		pDxEvent->stText[eDxTVTVEPG_TEXT_THUMBNAIL].ucStr = TVTVEPG_StrDup(buf);
		if(pDxEvent->stText[eDxTVTVEPG_TEXT_THUMBNAIL].ucStr)
		{
			pDxEvent->stText[eDxTVTVEPG_TEXT_THUMBNAIL].usSize= HxSTD_StrLen(pDxEvent->stText[eDxTVTVEPG_TEXT_THUMBNAIL].ucStr);
		}
	}

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC:

	return hError;
}

HERROR xsvc_epgcontext_SendToDamaEventIfno(SvcEpg_Event_t *pNewEvent)
{
	xsvcEpg_TvtvSvcInfo_t stSvcInfo;
	DxTvtvEvent_t			stTvtvEvent;

	if(pNewEvent == NULL)
	{
		HxLOG_Error("pNewEvent is NULL\n");
		return ERR_FAIL;
	}

	HxSTD_memset(&stSvcInfo, 0, sizeof(xsvcEpg_TvtvSvcInfo_t));
	HxSTD_memset(&stTvtvEvent, 0, sizeof(DxTvtvEvent_t));

	if(xsvc_epgcontext_GetSvcInfoFormTvtvId(pNewEvent->stEventInfo.stTvtvEventInfo.usTvtvId, &stSvcInfo) == ERR_OK)
	{
		if(xsvc_epgcontext_GetDxTvtvEventFromEvent(pNewEvent, &stTvtvEvent, &stSvcInfo) == ERR_EPG_OK)
		{
			xsvc_epgcontext_EventSendToDama_Tvtv(&stTvtvEvent);
		}
	}
	return ERR_OK;
}

STATIC HERROR				xsvc_epgcontext_ProcessXMLParseBuffer_Tvtv(HUINT32 ulParam1, HUINT32 ulParam2)
{
	HUINT32						ulMessage = 0;
	HERROR						hError = ERR_EPG_FAIL;
	Handle_t						hAction = HANDLE_NULL;
	Handle_t						hSvcHandle = HANDLE_NULL;
	svcEpg_Msg_t						*pstMsg = NULL;
	POINTER_LIST_T				*pEventList = NULL;
	svcEpg_PostProcessInfo_t		stPostprocessInfo = {0, };
	svcEpg_XmlParseInfo_t			stXMLParseInfo = {0, };
	xsvcEpg_PostprocFile_t				*pPostprocessedFile = NULL;
	HUINT32						ulCnxtFilterSlot = 0;
	HUINT32						ulOrgDeliType = 0;
	HUINT8						ucAntId = 0;
	EPG_SpecType_e				eSpec;
	svcEpg_Filter_t 				stCnxtFilterInfo = {0, };
	HBOOL						bFirstPostprocessedFile = FALSE;

	pstMsg = (svcEpg_Msg_t*)ulParam1;
	if(pstMsg == NULL)
	{
		HxLOG_Error("param fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	/**********************************************************************************
	  *
	  * 1. get param info
	  *
	  **********************************************************************************/
	ulMessage = pstMsg->ulMessage;
	if(ulMessage == eEPG_MSG_TVTV_POSTPROCESSCOMPLETE)
	{
		hError = ERR_OK;
		HxLOG_Info("eEPG_MSG_TVTV_POSTPROCESSCOMPLETE!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	hAction = (Handle_t)pstMsg->ulParam1;
	pPostprocessedFile = (xsvcEpg_PostprocFile_t*)pstMsg->ulParam2;
	bFirstPostprocessedFile = (HBOOL)pstMsg->ulParam3;
	NOT_USED_PARAM(bFirstPostprocessedFile);
	if(pPostprocessedFile == NULL)
	{
		HxLOG_Error("param fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	if(xsvc_epgcontext_GetContextInfo_Tvtv(hAction, ulCnxtFilterSlot, &hSvcHandle, &ulOrgDeliType, &ucAntId, &eSpec, &stCnxtFilterInfo) != ERR_EPG_OK)
	{
		HxLOG_Error("local_cnxtpf_ProcessEit fail!!\n");
	}

	if(xsvc_epgcontext_GetXMLParseInfo_Tvtv(hAction, &stXMLParseInfo) != ERR_EPG_OK)
	{
		HxLOG_Error("xsvc_epgcontext_GetXMLParseInfo_Tvtv fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	stXMLParseInfo.ulReceivePostprocessedFile++;

	switch(pPostprocessedFile->eFileType)
	{
		case TVTV_FileType_TSMT :
			if(xsvc_epglib_ParseToHumaxServiceList_Tvtv(pPostprocessedFile->pucFilePointer, pPostprocessedFile->ulFileSize) != ERR_OK)
			{
				HxLOG_Error("xsvc_epglib_ParseToHumaxEventList_Tvtv fail!!\n");
				goto  EXIT_CNXTTVTV_FUNC;
			}

			stXMLParseInfo.bSetTsmt = TRUE;
			break;

		case TVTV_FileType_EPG_EVENT :
			if(stXMLParseInfo.bSetTsmt == TRUE)
			{
				if(xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv(pPostprocessedFile->pucFilePointer, pPostprocessedFile->ulFileSize, &pEventList) != ERR_OK)
				{
					HxLOG_Error("xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv fail!!\n");
					goto  EXIT_CNXTTVTV_FUNC;
				}
				#if 0
				if(xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv(pPostprocessedFile->pucFilePointer, pPostprocessedFile->ulFileSize) != ERR_OK)
				{
					goto  EXIT_CNXTTVTV_FUNC;
				}
				#endif
			}
			break;

		default :
			HxLOG_Error("default!!\n");
			break;
	}

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :
	if(pEventList)
	{
		pEventList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pEventList, xsvc_epgdblib_FreeEventListContents);
	}

	if(xsvc_epgcontext_GetPostprocessInfo_Tvtv(hAction, &stPostprocessInfo) != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_ProcessXMLParseBuffer_Tvtv] xsvc_epgcontext_GetPostprocessInfo_Tvtv fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	if(stPostprocessInfo.bCompletePostprocess)
	{
		if(stPostprocessInfo.ulSendPostprocessedFile == stXMLParseInfo.ulReceivePostprocessedFile)
		{
			stXMLParseInfo.bCompleteParse = TRUE;

			if(xsvc_epglib_InitBuffer_Tvtv() != ERR_OK)
			{
				HxLOG_Error("xsvc_epglib_InitBuffer_Tvtv fail!!\n");
			}
		}
	}

	if(xsvc_epgcontext_SetXMLParseInfo_Tvtv(hAction, &stXMLParseInfo) != ERR_EPG_OK)
	{
		HxLOG_Error("xsvc_epgcontext_SetXMLParseInfo_Tvtv fail!!\n");
	}

#if 1 // tvtv epg에 의한 memory leak 수정사항.
#else
	if(pPostprocessedFile)
	{
		if(pPostprocessedFile->pucFileName)
		{
			OxDSMCC_Free(pPostprocessedFile->pucFileName);
		}
		if(pPostprocessedFile->pucFilePointer)
		{
			OxEPG_Free(pPostprocessedFile->pucFilePointer);
		}
		OxEPG_Free(pPostprocessedFile);
		pPostprocessedFile = NULL;
	}
#endif
	return hError;
}
#else
STATIC HERROR				xsvc_epgcontext_ProcessXMLParseBuffer_Tvtv(HUINT32 ulParam1, HUINT32 ulParam2)
{
	HUINT32						ulMessage = 0;
	HERROR						hError = ERR_EPG_FAIL;
	Handle_t						hAction = HANDLE_NULL;
	Handle_t						hSvcHandle = HANDLE_NULL;
	svcEpg_Msg_t						*pstMsg = NULL;
	svcEpg_PostProcessInfo_t		stPostprocessInfo = {0, };
	svcEpg_XmlParseInfo_t			stXMLParseInfo = {0, };
	xsvcEpg_PostprocFile_t				*pPostprocessedFile = NULL;
	POINTER_LIST_T				*pEventList = NULL;
	POINTER_LIST_T				*pExtList = NULL;
	POINTER_LIST_T				*pTvtvMapList = NULL;
	HUINT32						ulCnxtFilterSlot = 0;
	HUINT32						ulOrgDeliType = 0;
	HUINT8						ucAntId = 0;
	EPG_SpecType_e				eSpec;
	svcEpg_Filter_t 				stCnxtFilterInfo = {0, };
	HBOOL						bFirstPostprocessedFile = FALSE;

	pstMsg = (svcEpg_Msg_t*)ulParam1;
	if(pstMsg == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_ProcessXMLParseBuffer_Tvtv] param fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	/**********************************************************************************
	  *
	  * 1. get param info
	  *
	  **********************************************************************************/
	ulMessage = pstMsg->ulMessage;
	if(ulMessage == eEPG_MSG_TVTV_POSTPROCESSCOMPLETE)
	{
		hError = ERR_OK;
		HxLOG_Info("[xsvc_epgcontext_ProcessXMLParseBuffer_Tvtv] eEPG_MSG_TVTV_POSTPROCESSCOMPLETE!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	hAction = (Handle_t)pstMsg->ulParam1;
	pPostprocessedFile = (xsvcEpg_PostprocFile_t*)pstMsg->ulParam2;
	bFirstPostprocessedFile = (HBOOL)pstMsg->ulParam3;
	if(pPostprocessedFile == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_ProcessXMLParseBuffer_Tvtv] param fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	if(bFirstPostprocessedFile == TRUE)
	{
		hError = UTIL_TREEUTIL_DestroyTree(hTvtvTree);
		if(hError != ERR_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_ProcessXMLParseBuffer_Tvtv] UTIL_TREEUTIL_DestroyTree fail!!\n");
		}
		hTvtvTree = NULL;
		hError = UTIL_TREEUTIL_CreateTree(&hTvtvTree);
		if(hError != ERR_OK|| hTvtvTree == NULL)
		{
			HxLOG_Error("[xsvc_epgcontext_ProcessXMLParseBuffer_Tvtv] UTIL_TREEUTIL_CreateTree fail!!\n");
			goto  EXIT_CNXTTVTV_FUNC;
		}
	}

	hError = xsvc_epgcontext_GetContextInfo_Tvtv(hAction, ulCnxtFilterSlot, &hSvcHandle, &ulOrgDeliType, &ucAntId, &eSpec, &stCnxtFilterInfo);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_ProcessXMLParseBuffer_Tvtv] local_cnxtpf_ProcessEit fail!!\n");
	}
	hError = xsvc_epgcontext_GetXMLParseInfo_Tvtv(hAction, &stXMLParseInfo);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_ProcessXMLParseBuffer_Tvtv] xsvc_epgcontext_GetXMLParseInfo_Tvtv fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}
	stXMLParseInfo.ulReceivePostprocessedFile++;

	switch(pPostprocessedFile->eFileType)
	{
		case TVTV_FileType_LTOT :
			break;
		case TVTV_FileType_TSMT :
			hError = xsvc_epglib_ParseToHumaxServiceList_Tvtv(pPostprocessedFile->pucFilePointer, pPostprocessedFile->ulFileSize, &pTvtvMapList);
			if(hError != ERR_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_ProcessXMLParseBuffer_Tvtv] xsvc_epglib_ParseToHumaxEventList_Tvtv fail!!\n");
				goto  EXIT_CNXTTVTV_FUNC;
			}
			hError = xsvc_epgcontext_InsertTvtvEventListByTvtvMap_Tvtv(hTvtvTree, ulOrgDeliType, ucAntId, pTvtvMapList);
			if(hError != ERR_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_ProcessXMLParseBuffer_Tvtv] xsvc_epgcontext_InsertTvtvEventListByEvent_Tvtv fail!!\n");
				goto  EXIT_CNXTTVTV_FUNC;
			}
			pTvtvMapList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pTvtvMapList, xsvc_epgcontext_FreeTvtvMapList_Tvtv);

			if (pTvtvMapList != NULL)
			{
				HxLOG_Error("[xsvc_epgcontext_ProcessXMLParseBuffer_Tvtv] pTvtvMapList is not NULL ( fail )!!\n");
			}

			stXMLParseInfo.bSetTsmt = TRUE;
#if 1
			hError = xsvc_epgcontext_SvcInfoSendToDama_tvtv(hTvtvTree);
#else
			hError = xsvc_epgcontext_RegisterEvent_Tvtv(	hTvtvTree,
												hSvcHandle,
												eSpec,
												ulOrgDeliType,
												ucAntId);
#endif
			if(hError != ERR_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_ProcessXMLParseBuffer_Tvtv] xsvc_epgcontext_RegisterEvent_Tvtv fail!!\n");
				goto  EXIT_CNXTTVTV_FUNC;
			}
			break;

		case TVTV_FileType_EPG_EVENT :
			pExtList = NULL;
			hError = xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv(pPostprocessedFile->pucFilePointer, pPostprocessedFile->ulFileSize, &pEventList);
			if(hError != ERR_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_ProcessXMLParseBuffer_Tvtv] xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv fail!!\n");
				goto  EXIT_CNXTTVTV_FUNC;
			}

			hError = xsvc_epgcontext_InsertTvtvEventListByEvent_Tvtv(hTvtvTree, pPostprocessedFile->ulTableId, pPostprocessedFile->usFileIndex, pEventList, pExtList);
			if(hError != ERR_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_ProcessXMLParseBuffer_Tvtv] xsvc_epgcontext_InsertTvtvEventListByEvent_Tvtv fail!!\n");
				goto  EXIT_CNXTTVTV_FUNC;
			}
			pEventList = NULL;
			pExtList = NULL;

#if defined(TEMP_DBG_RPINT)
			{
				static int epgCnt = 0;
				//HLIB_CMD_Printf("################# [TVTV_FileType_EPG_EVENT] cnt[%d]  -- %s --\n", ++epgCnt, stXMLParseInfo.bSetTsmt==TRUE?"tsmt ok":"..");
			}
#endif


			if(stXMLParseInfo.bSetTsmt == TRUE)
			{
#if 1
				hError = xsvc_epgcontext_RegisterEventInfo_Tvtv(hTvtvTree);
#else
				hError = xsvc_epgcontext_RegisterEvent_Tvtv(	hTvtvTree,
													hSvcHandle,
													eSpec,
													ulOrgDeliType,
													ucAntId);
#endif
				if(hError != ERR_OK)
				{
					HxLOG_Error("[xsvc_epgcontext_ProcessXMLParseBuffer_Tvtv] xsvc_epgcontext_RegisterEvent_Tvtv fail!!\n");
					goto  EXIT_CNXTTVTV_FUNC;
				}
			}
			break;

		default :
			HxLOG_Error("[xsvc_epgcontext_ProcessXMLParseBuffer_Tvtv] default!!\n");
			break;
	}


	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :
	if(pEventList)
	{
		pEventList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pEventList, xsvc_epgdblib_FreeEventListContents);
	}
	if(pExtList)
	{
		pExtList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pExtList, xsvc_epgdblib_FreeExtEventListContents);
	}

	hError = xsvc_epgcontext_GetPostprocessInfo_Tvtv(hAction, &stPostprocessInfo);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_ProcessXMLParseBuffer_Tvtv] xsvc_epgcontext_GetPostprocessInfo_Tvtv fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	if(stPostprocessInfo.bCompletePostprocess)
	{
		if(stPostprocessInfo.ulSendPostprocessedFile == stXMLParseInfo.ulReceivePostprocessedFile)
		{
			stXMLParseInfo.bCompleteParse = TRUE;

			hError = UTIL_TREEUTIL_DestroyTree(hTvtvTree);
			if(hError != ERR_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_ProcessXMLParseBuffer_Tvtv] UTIL_TREEUTIL_DestroyTree fail!!\n");
			}
			hTvtvTree = NULL;

			hError = xsvc_epglib_InitBuffer_Tvtv();
			if(hError != ERR_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_ProcessXMLParseBuffer_Tvtv] xsvc_epglib_InitBuffer_Tvtv fail!!\n");
			}
		}
	}
	hError = xsvc_epgcontext_SetXMLParseInfo_Tvtv(hAction, &stXMLParseInfo);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_ProcessXMLParseBuffer_Tvtv] xsvc_epgcontext_SetXMLParseInfo_Tvtv fail!!\n");
	}

	if(pPostprocessedFile)
	{
		if(pPostprocessedFile->pucFileName)
		{
			OxDSMCC_Free(pPostprocessedFile->pucFileName);
		}
		if(pPostprocessedFile->pucFilePointer)
		{
			OxEPG_Free(pPostprocessedFile->pucFilePointer);
		}
		OxEPG_Free(pPostprocessedFile);
		pPostprocessedFile = NULL;
	}

	return hError;
}
#endif
#endif

STATIC HERROR				xsvc_epgcontext_RegisterEvent_Tvtv(HTREEMGR hTree, Handle_t hSvcHandle, EPG_SpecType_e eSpec, HUINT32 ulOrgDeliType, HUINT8 ucAntId)
{
	HERROR						hError = ERR_EPG_FAIL;
	HTREEITEM					hFirstChild = NULL;
	HTREEITEM					hTempItem = NULL;
#if	0
	STATIC HUINT32				s_ulCurrentTick = 0;
	STATIC HUINT32				s_ulStartTick = 0;
#endif
	void							*pData = NULL;
	svcEpg_TvtvMap_t					stTvtvMap = {0, };
	svcEpg_ServiceInfo_t			stEpgServiceInfo = {0, };
	svcEpg_TvtvEventList_t				*pTvtvService = NULL;

	if(hTree == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_RegisterEvent_Tvtv] param fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	hError = UTIL_TREEUTIL_GetFirstChildItem(hTree, &hFirstChild);
	if(hError != ERR_OK || hFirstChild == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_RegisterEvent_Tvtv] UTIL_TREEUTIL_GetFirstChildItem fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}

	hTempItem = hFirstChild;
	while(hTempItem)
	{
		pData = NULL;
		pTvtvService = NULL;

		HxSTD_memset(&stTvtvMap, 0x00, sizeof(svcEpg_TvtvMap_t));
		HxSTD_memset(&stEpgServiceInfo, 0x00, sizeof(svcEpg_ServiceInfo_t));

		hError = UTIL_TREEUTIL_GetItemUserInfo(hTempItem, sizeof(svcEpg_TvtvMap_t), (void*)&stTvtvMap);
		if(hError != ERR_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_RegisterEvent_Tvtv] UTIL_TREEUTIL_GetNextItem fail!!\n");
			goto  END_TVTVMAP_LOOP;
		}
		stEpgServiceInfo.usOnId = stTvtvMap.stTripleId.usOnId;
		stEpgServiceInfo.usTsId = stTvtvMap.stTripleId.usTsId;
		stEpgServiceInfo.usSvcId = stTvtvMap.stTripleId.usSvcId;
		stEpgServiceInfo.ucTableId = EIT_OTHER_SCHEDULE_TABLE_ID_0;

		hError = UTIL_TREEUTIL_GetItemData(hTempItem, &pData);
		if(hError != ERR_OK || pData == NULL)
		{
			HxLOG_Error("[xsvc_epgcontext_RegisterEvent_Tvtv] UTIL_TREEUTIL_GetNextItem fail!!\n");
			goto  END_TVTVMAP_LOOP;
		}
		pTvtvService = (svcEpg_TvtvEventList_t*)pData;
		if(pTvtvService->pstEventList == NULL && pTvtvService->pstExtList == NULL)
		{
			HxLOG_Print("[xsvc_epgcontext_RegisterEvent_Tvtv] Empty serivce!!\n");
			goto  END_TVTVMAP_LOOP;
		}

		pTvtvService->pstEventList = NULL;
		pTvtvService->pstExtList = NULL;

		// 컴파일 에러 : 임시 막음 .. DAMA에서 적용되어야함.
		if(hError != ERR_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_RegisterEvent_Tvtv] EPGDB_AddEventList fail!!\n");
			goto  END_TVTVMAP_LOOP;
		}
HxLOG_Info("[xsvc_epgcontext_RegisterEvent_Tvtv] SvcId(%d)TvtvId(%d)!!\n", stTvtvMap.stTripleId.usSvcId, stTvtvMap.usTvtvId);
#if		0
		s_ulCurrentTick = VK_TIMER_GetSystemTick();
		if(s_ulStartTick == 0)
		{
			s_ulStartTick = s_ulCurrentTick;
		}

		if(s_ulStartTick - s_ulCurrentTick > 60)
		{
			VK_TASK_Sleep(10);
			s_ulStartTick = 0;
		}
#endif

END_TVTVMAP_LOOP:
		hError = UTIL_TREEUTIL_GetNextItem(hTempItem, &hTempItem);
		if(hError != ERR_OK )
		{
			HxLOG_Error("[xsvc_epgcontext_RegisterEvent_Tvtv] UTIL_TREEUTIL_GetNextItem fail!!\n");
			break;
		}
	}


	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :

	return hError;
}

STATIC HERROR				xsvc_epgcontext_InsertTvtvEventListByTvtvMap_Tvtv(HTREEMGR hTree, HUINT32 ulOrgDeliType, HUINT8 ucAntId, POINTER_LIST_T *pTvtvMapList)
{
	HERROR						hError = ERR_EPG_FAIL;
	POINTER_LIST_T				*pTempTvtvMapList = NULL;
	svcEpg_TvtvMap_t					*pTvtvMap = NULL;
	HTREEITEM					hFindItem = NULL;
	TREEITEM_PARAM_t			stTreeParam = {0, };
	svcEpg_TvtvEventList_t				*pNewtTvtvEventList = NULL;
#if defined(SVC_TRIPLEID_COMPARE)
	HINT32						lCount = 0;
	HINT32						lIndex = 0;
	DbSvc_TripleId_t				*pstTripleIdList = NULL;
	HBOOL						bFound = FALSE;
	HUINT16						usTsId = 0, usSvcId = 0;
#endif
	if(hTree == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_InsertTvtvEventListByTvtvMap_Tvtv] param fail!!\n");
		goto  EXIT_CNXTTVTV_FUNC;
	}
#if defined(SVC_TRIPLEID_COMPARE)
	hError = DB_SVC_FindTripleIdAndSvcTypeList(ulOrgDeliType, ucAntId, &lCount, &pstTripleIdList, NULL, NULL);
	if(hError != ERR_OK || pstTripleIdList == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_InsertTvtvEventListByTvtvMap_Tvtv] DB_SVC_FindTripleIdAndSvcTypeList fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}
#endif

	pTempTvtvMapList= pTvtvMapList;
	while(pTempTvtvMapList)
	{
		pNewtTvtvEventList = NULL;
		pTvtvMap = NULL;
		hFindItem = NULL;

		pTvtvMap = (svcEpg_TvtvMap_t*)UTIL_LINKEDLIST_GetContents(pTempTvtvMapList);
		if(pTvtvMap != NULL)
		{
#if defined(SVC_TRIPLEID_COMPARE)
			bFound = FALSE;
			for(lIndex = 0; lIndex < lCount; lIndex++)
			{
				if(pstTripleIdList[lIndex].usSvcId == pTvtvMap->stTripleId.usSvcId)
				{
					if(pstTripleIdList[lIndex].usTsId == pTvtvMap->stTripleId.usTsId)
					{
						if(pstTripleIdList[lIndex].usOnId == pTvtvMap->stTripleId.usOnId)
						{
							bFound = TRUE;
						}
					}
				}
			}
			if(!bFound)
			{
				HxLOG_Print("[xsvc_epgcontext_InsertTvtvEventListByTvtvMap_Tvtv] !bFound usTvtvId(%d)usSvcId(%d)usTsId(%d)usOnId(%d)\n",
																							pTvtvMap->usTvtvId,
																							pTvtvMap->stTripleId.usSvcId,
																							pTvtvMap->stTripleId.usTsId,
																							pTvtvMap->stTripleId.usOnId);

				hError = xsvc_epgcontext_FindTvtvItem_Tvtv(	hTree,
													pTvtvMap->usTvtvId,
													&hFindItem);
				if(hFindItem != NULL)
				{
#if defined(TEMP_DBG_RPINT)
					xsvc_epgcontext_PrintTvtvMapInfo(eTVTVMAP_NOT_FOUND, pTvtvMap, 0);
#endif
					goto END_TVTVMAP_LOOP;
				}

			}
#if defined(TVTV_ADD_USR_TP)
			else
			{
				usTsId = pTvtvMap->stTripleId.usTsId;
				usSvcId = pTvtvMap->stTripleId.usSvcId;
				hError = xsvc_epgcontext_IsExistTVTVEPG(usTsId, usSvcId);
				if(hError != ERR_OK)
				{
					HxLOG_Print("[local_cnxttvtv_InsertTvtvEventListByTvtvMap] local_cnxttvtv_IsExistTVTVEPG fail!!\n");
					goto END_TVTVMAP_LOOP;
				}

			}
#endif
#endif
			HxLOG_Info("[xsvc_epgcontext_InsertTvtvEventListByTvtvMap_Tvtv] add usTvtvId(%d)usSvcId(%d)usTsId(%d)usOnId(%d)\n",
																						pTvtvMap->usTvtvId,
																						pTvtvMap->stTripleId.usSvcId,
																						pTvtvMap->stTripleId.usTsId,
																						pTvtvMap->stTripleId.usOnId);

#if defined(TVTV_USE_SVCUID)
			xsvc_epgcontext_GetSvcUidFromTripleId(pTvtvMap->usTvtvId, pTvtvMap->stTripleId.usOnId, pTvtvMap->stTripleId.usTsId, pTvtvMap->stTripleId.usSvcId, &pTvtvMap->ulSvcUid);
#endif
#if defined(TVTV_USE_TRIPLEID)
			xsvc_epgcontext_SetTvtvSvcInfoList(pTvtvMap->usTvtvId, pTvtvMap->stTripleId.usOnId, pTvtvMap->stTripleId.usTsId, pTvtvMap->stTripleId.usSvcId, &pTvtvMap->ulSvcUid);
#endif

			hError = xsvc_epgcontext_FindTvtvItem_Tvtv(	hTree,
												pTvtvMap->usTvtvId,
												&hFindItem);
			if(hFindItem == NULL)
			{
				pNewtTvtvEventList = (svcEpg_TvtvEventList_t*)OxEPG_Malloc(sizeof(svcEpg_TvtvEventList_t));
				if(pNewtTvtvEventList == NULL)
				{
					HxLOG_Error("[xsvc_epgcontext_InsertTvtvEventListByTvtvMap_Tvtv] OxEPG_Malloc fail!!\n");
					goto END_TVTVMAP_LOOP;
				}
				VK_memset32(pNewtTvtvEventList, 0x00, sizeof(svcEpg_TvtvEventList_t));

				VK_memset32(&stTreeParam, 0x00, sizeof(TREEITEM_PARAM_t));
				stTreeParam.pfnFreeFunc = xsvc_epgcontext_FreeTvtvEventList_Tvtv;
				stTreeParam.pLocalData = (void*)pNewtTvtvEventList;
				hError = UTIL_TREEUTIL_InsertItem( hTree, NULL, NULL, NULL, &stTreeParam, &hFindItem);
				if(hError != ERR_OK || hFindItem == NULL)
				{
					HxLOG_Error("[xsvc_epgcontext_InsertTvtvEventListByTvtvMap_Tvtv] UTIL_TREEUTIL_InsertItem fail!!\n");
					goto END_TVTVMAP_LOOP;
				}
				HxLOG_Info("[xsvc_epgcontext_InsertTvtvEventListByTvtvMap_Tvtv] add usTvtvId (%d) + \n!!\n", pTvtvMap->usTvtvId);
			}
#if defined(TEMP_DBG_RPINT)
			xsvc_epgcontext_PrintTvtvMapInfo(eTVTVMAP_SET, pTvtvMap, 0);
#endif
			hError = UTIL_TREEUTIL_SetItemUserInfo(hFindItem, sizeof(svcEpg_TvtvMap_t), (void*)(pTvtvMap));
			if(hError != ERR_TREEUTIL_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_InsertTvtvEventListByTvtvMap_Tvtv] SetItemUserInfo fail!!\n");
				goto END_TVTVMAP_LOOP;
			}
		}

END_TVTVMAP_LOOP:
		pTempTvtvMapList = UTIL_LINKEDLIST_GetNextPointer(pTempTvtvMapList);

	}

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :

#if defined(SVC_TRIPLEID_COMPARE)
	if(pstTripleIdList != NULL)
	{
		OxDB_Free(pstTripleIdList);
		pstTripleIdList = NULL;
	}
#endif
	return hError;
}

STATIC HERROR				xsvc_epgcontext_InsertTvtvEventListByEvent_Tvtv(HTREEMGR hTree, HUINT32 ulTableId, HUINT16 usFileIndex, POINTER_LIST_T *pEventList, POINTER_LIST_T *pExtList)
{
	HERROR						hError = ERR_EPG_FAIL;
	POINTER_LIST_T				*pTempEventList = NULL;
	POINTER_LIST_T				*pTempExtList = NULL;
	SvcEpg_Event_t					*pEvent = NULL;
	SvcEpg_ExtEvent_t				*pExtEvent = NULL;
	HTREEITEM					hFindItem = NULL;
	TREEITEM_PARAM_t			stTreeParam = {0, };
	svcEpg_TvtvEventList_t				*pNewtTvtvEventList = NULL;
	svcEpg_TvtvEventList_t				*pTvtvEventList = NULL;
	svcEpg_TvtvMap_t					stTvtvMap = {0, };

	if(hTree == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_InsertTvtvEventListByEvent_Tvtv] param fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	pTempEventList= pEventList;
	while(pTempEventList)
	{
		pNewtTvtvEventList = NULL;
		pTvtvEventList = NULL;
		pEvent = NULL;
		hFindItem = NULL;
		pEvent = (SvcEpg_Event_t*)UTIL_LINKEDLIST_GetContents(pTempEventList);
		if(pEvent != NULL)
		{
			hError = xsvc_epgcontext_FindTvtvItem_Tvtv(	hTree,
												pEvent->stEventInfo.stTvtvEventInfo.usTvtvId,
												&hFindItem);
			if(hFindItem == NULL)
			{
				pNewtTvtvEventList = (svcEpg_TvtvEventList_t*)OxEPG_Malloc(sizeof(svcEpg_TvtvEventList_t));
				if(pNewtTvtvEventList == NULL)
				{
					HxLOG_Error("[xsvc_epgcontext_InsertTvtvEventListByEvent_Tvtv] OxEPG_Malloc fail!!\n");
					goto END_EVENT_LOOP;
				}
				VK_memset32(pNewtTvtvEventList, 0x00, sizeof(svcEpg_TvtvEventList_t));

				VK_memset32(&stTreeParam, 0x00, sizeof(TREEITEM_PARAM_t));
				stTreeParam.pfnFreeFunc = xsvc_epgcontext_FreeTvtvEventList_Tvtv;
				stTreeParam.pLocalData = (void*)pNewtTvtvEventList;
				hError = UTIL_TREEUTIL_InsertItem( hTree, NULL, NULL, NULL, &stTreeParam, &hFindItem);
				if(hError != ERR_OK || hFindItem == NULL)
				{
					HxLOG_Error("[xsvc_epgcontext_InsertTvtvEventListByEvent_Tvtv] UTIL_TREEUTIL_InsertItem fail!!\n");
					goto END_EVENT_LOOP;
				}

				VK_memset32(&stTvtvMap, 0x00, sizeof(svcEpg_TvtvMap_t));
				stTvtvMap.usTvtvId = pEvent->stEventInfo.stTvtvEventInfo.usTvtvId;
				hError = UTIL_TREEUTIL_SetItemUserInfo(hFindItem, sizeof(svcEpg_TvtvMap_t), (void*)&(stTvtvMap));
				if(hError != ERR_TREEUTIL_OK)
				{
					HxLOG_Error("[xsvc_epgcontext_InsertTvtvEventListByEvent_Tvtv] SetItemUserInfo fail!!\n");
					goto END_EVENT_LOOP;
				}
			}
			pEvent->stEventInfo.stTvtvEventInfo.ulTableId = ulTableId;
			pEvent->stEventInfo.stTvtvEventInfo.usFileIndex = usFileIndex;

			hError = UTIL_TREEUTIL_GetItemData(hFindItem, (void **)&pTvtvEventList);
			if(hError != ERR_OK || pTvtvEventList == NULL)
			{
				HxLOG_Error("[xsvc_epgcontext_InsertTvtvEventListByEvent_Tvtv] UTIL_TREEUTIL_GetItemData fail!!\n");
				goto END_EVENT_LOOP;
			}
#if 1
			// event free안되는 문제로인해 여기서 막음.
#else
			pTempEventList->pvContents = NULL;
#endif
			pTvtvEventList->pstEventList = UTIL_LINKEDLIST_AppendListItemToPointerList(pTvtvEventList->pstEventList, pEvent);
		}
END_EVENT_LOOP:
		pTempEventList = UTIL_LINKEDLIST_GetNextPointer(pTempEventList);
	}


	pTempExtList= pExtList;
	while(pTempExtList)
	{
		pNewtTvtvEventList = NULL;
		pTvtvEventList = NULL;
		pExtEvent = NULL;
		hFindItem = NULL;
		pExtEvent = (SvcEpg_ExtEvent_t*)UTIL_LINKEDLIST_GetContents(pTempExtList);
		if(pExtEvent != NULL)
		{
			hError = xsvc_epgcontext_FindTvtvItem_Tvtv(	hTree,
												pExtEvent->stEventId.usTvtvId,
												&hFindItem);
			if(hFindItem == NULL)
			{
				pNewtTvtvEventList = (svcEpg_TvtvEventList_t*)OxEPG_Malloc(sizeof(svcEpg_TvtvEventList_t));
				if(pNewtTvtvEventList == NULL)
				{
					HxLOG_Error("[xsvc_epgcontext_InsertTvtvEventListByEvent_Tvtv] OxEPG_Malloc fail!!\n");
					goto END_EXTEVENT_LOOP;
				}
				VK_memset32(pNewtTvtvEventList, 0x00, sizeof(svcEpg_TvtvEventList_t));

				VK_memset32(&stTreeParam, 0x00, sizeof(TREEITEM_PARAM_t));
				stTreeParam.pfnFreeFunc = xsvc_epgcontext_FreeTvtvEventList_Tvtv;
				stTreeParam.pLocalData = (void*)pNewtTvtvEventList;
				hError = UTIL_TREEUTIL_InsertItem( hTree, NULL, NULL, NULL, &stTreeParam, &hFindItem);
				if(hError != ERR_OK || hFindItem == NULL)
				{
					HxLOG_Error("[xsvc_epgcontext_InsertTvtvEventListByEvent_Tvtv] UTIL_TREEUTIL_InsertItem fail!!\n");
					goto END_EXTEVENT_LOOP;
				}

				VK_memset32(&stTvtvMap, 0x00, sizeof(svcEpg_TvtvMap_t));
				stTvtvMap.usTvtvId = pExtEvent->stEventId.usTvtvId;
				hError = UTIL_TREEUTIL_SetItemUserInfo(hFindItem, sizeof(svcEpg_TvtvMap_t), (void*)&(stTvtvMap));
				if(hError != ERR_TREEUTIL_OK)
				{
					HxLOG_Error("[xsvc_epgcontext_InsertTvtvEventListByEvent_Tvtv] SetItemUserInfo fail!!\n");
					goto END_EXTEVENT_LOOP;
				}
			}

			hError = UTIL_TREEUTIL_GetItemData(hFindItem, (void **)&pTvtvEventList);
			if(hError != ERR_OK || pTvtvEventList == NULL)
			{
				HxLOG_Error("[xsvc_epgcontext_InsertTvtvEventListByEvent_Tvtv] UTIL_TREEUTIL_GetItemData fail!!\n");
				goto END_EXTEVENT_LOOP;
			}
#if 1
#else
			pTempExtList->pvContents = NULL;
#endif
			pTvtvEventList->pstExtList = UTIL_LINKEDLIST_AppendListItemToPointerList(pTvtvEventList->pstExtList, pExtEvent);
		}
END_EXTEVENT_LOOP:
		pTempExtList = UTIL_LINKEDLIST_GetNextPointer(pTempExtList);
	}

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :

	return hError;
}

STATIC HERROR				xsvc_epgcontext_FindTvtvItem_Tvtv(HTREEMGR hTree, HUINT32 usTvtvId, HTREEITEM *phItem)
{
	HERROR						hError = ERR_EPG_FAIL;
	HTREEITEM					hFirstChild = NULL;
	HTREEITEM					hTempItem = NULL;
	svcEpg_TvtvMap_t					stTempTvtvMap = {0, };

	if(hTree == NULL || phItem == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_FindTvtvItem_Tvtv] param fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}
	*phItem = NULL;

	hError = UTIL_TREEUTIL_GetFirstChildItem(hTree, &hFirstChild);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Error("[xsvc_epgcontext_FindTvtvItem_Tvtv] GetChildItem fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	hTempItem = hFirstChild;
	while(hTempItem)
	{
		HxSTD_memset(&stTempTvtvMap, 0x00, sizeof(svcEpg_TvtvMap_t));
		hError = UTIL_TREEUTIL_GetItemUserInfo(hTempItem, sizeof(svcEpg_TvtvMap_t), (void*)&stTempTvtvMap);
		if(hError != ERR_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_FindTvtvItem_Tvtv] GetItemUserInfo fail!!\n");
			hError = UTIL_TREEUTIL_GetNextItem(hTempItem, &hTempItem);
			if(hError != ERR_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_FindTvtvItem_Tvtv] GetNextItem fail!!\n");
				goto EXIT_CNXTTVTV_FUNC;
			}
			continue;
		}
		if(usTvtvId == stTempTvtvMap.usTvtvId)
		{
			*phItem = hTempItem;
			break;
		}

		hError = UTIL_TREEUTIL_GetNextItem(hTempItem, &hTempItem);
		if(hError != ERR_OK)
		{
			HxLOG_Error("[xsvc_epgcontext_FindTvtvItem_Tvtv] GetNextItem fail!!\n");
			goto EXIT_CNXTTVTV_FUNC;
		}
	}

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :

	return hError;
}

#define ____FREE_FUNC____
STATIC void				xsvc_epgcontext_FreeTvtvEventList_Tvtv(void *pvContents)
{
	svcEpg_TvtvEventList_t		*pTvtvEventList = NULL;
	POINTER_LIST_T		*pRoot = NULL;

	if(pvContents == NULL)
	{
		HxLOG_Print("[xsvc_epgcontext_FreeTvtvEventList_Tvtv] param fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}
	pTvtvEventList = (svcEpg_TvtvEventList_t*)pvContents;

	pTvtvEventList->pstExtList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pTvtvEventList->pstExtList, xsvc_epgdblib_FreeExtEventListContents);
	if(pRoot != NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_FreeTvtvEventList_Tvtv] FreeWholeListItemsFromPointerList pExtList fail!!\n");
	}
	pTvtvEventList->pstEventList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pTvtvEventList->pstEventList, xsvc_epgdblib_FreeEventListContents);
	if(pRoot != NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_FreeTvtvEventList_Tvtv] FreeWholeListItemsFromPointerList pEventList fail!!\n");
	}

	OxEPG_Free(pTvtvEventList);

EXIT_CNXTTVTV_FUNC :

	return;
}


STATIC void				xsvc_epgcontext_FreeTvtvMapList_Tvtv(void *pvContents)
{
	svcEpg_TvtvMap_t			*pTvtvMap = NULL;

	if(pvContents == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_FreeTvtvMapList_Tvtv] param fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}
	pTvtvMap = (svcEpg_TvtvMap_t*)pvContents;

	OxEPG_Free(pTvtvMap);

EXIT_CNXTTVTV_FUNC :

	return;
}

STATIC void				xsvc_epgcontext_FreeFileTableList_Tvtv(void *pvContents)
{
	HUINT8				*pucFileName = NULL;

	if(pvContents == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_FreeFileTableList_Tvtv] param fail!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}
	pucFileName = (HUINT8*)pvContents;

	OxEPG_Free(pucFileName);

EXIT_CNXTTVTV_FUNC :

	return;
}


#define ____RANK_FUNC____

// STATIC contents를 붙일 위치에서 break(return 1)
// 오름차순
STATIC HINT8				xsvc_epgcontext_RankFileByFileNumber_Tvtv(void *pvItem1, void *pvItem2)
{
	HUINT8				*pucTemp = (HUINT8*)pvItem1;
	HUINT8 				*pucStatic = (HUINT8*)pvItem2;
	HINT32				lTempNumber = 0;
	HINT32				lStaticNumber = 0;
	HUINT32				ulLen = 0;

	ulLen = MWC_UTIL_DvbStrlen(pucTemp);
	if(ulLen > 4)
	{
		pucTemp[ulLen - 4] = 0;
		lTempNumber = (HINT32)atoi(pucTemp);
		pucTemp[ulLen - 4] = '.';
	}
	else
	{
		return 0;
	}

	ulLen = 0;
	ulLen = MWC_UTIL_DvbStrlen(pucStatic);
	if(ulLen > 4)
	{
		pucStatic[ulLen - 4] = 0;
		lStaticNumber = (HINT32)atoi(pucStatic);
		pucStatic[ulLen - 4] = '.';
	}
	else
	{
		return 0;
	}

	if(lTempNumber == lStaticNumber)
	{
		return 0 ;
	}
	if(lTempNumber < lStaticNumber)
	{
		return -1 ;
	}

	return 1 ;
}


#define	______GLOBAL_FUNCTION______

HERROR				xsvc_epgcontext_Process_Tvtv(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3, HUINT32 ulParam4)
{
	svcEpg_Msg_t			*pstMsg = NULL;
	svcEpg_ContextData_t		*pCnxtData = NULL;
	HUINT32				ulCnxtCommand = 0;
	Handle_t			hAction = HANDLE_NULL;
	HERROR				hError = ERR_EPG_FAIL;
	HUINT32				ulActionId = 0;
	HUINT32		ulDemuxId = RxRSCID_NULL;
	HUINT32				ulEventCount = 0;
	HUINT32				ulExtCount = 0;
	DbSvc_TripleId_t		*pTvtvTripleId = NULL;
	HUINT16				usResult = 0;
	HBOOL				bPass = FALSE;


	ulCnxtCommand = ulParam1;
	pstMsg = (svcEpg_Msg_t*)ulParam2;
	pCnxtData = (svcEpg_ContextData_t*)ulParam3;

	if ( pstMsg == NULL)
	{
		HxLOG_Error("[xsvc_epgcontext_Process_Tvtv] param null!!\n");
		goto EXIT_CNXTTVTV_FUNC;
	}

	switch(ulCnxtCommand)
	{
		case EPG_CMD_START :
			HxLOG_Info("[xsvc_epgcontext_Process_Tvtv] EPG_CMD_START (%d)\n", xsvc_epgcontext_GetSystemTime_Tvtv()%1000);
#if			defined(CONFIG_TVTV_CONTROL)
			hError = xsvc_epgcontext_ControlAction_Tvtv(ulCnxtCommand, FALSE, &bPass);
			if(bPass == TRUE)
			{
				HxLOG_Info("[xsvc_epgcontext_Process_Tvtv] bPass == TRUE !!\n");
				goto EXIT_CNXTTVTV_FUNC;
			}
#endif
			// step 1. local data setting
			hAction = (Handle_t)pstMsg->ulParam1;
			pTvtvTripleId = (DbSvc_TripleId_t*)pstMsg->ulParam2;

			/**********************************************************************************
			  *
			  * get tuning info
			  *
			  **********************************************************************************/
			if(	pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_PAT].eState == eEPG_CNXT_FILTER_ONCE
				|| pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_PMT].eState == eEPG_CNXT_FILTER_ONCE
				|| pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DSI].eState == eEPG_CNXT_FILTER_CONTINUE
				|| pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DDB].eState == eEPG_CNXT_FILTER_CONTINUE
			)
			{
				HxLOG_Print("[xsvc_epgcontext_Process_Tvtv] aleady running!!\n");
				hError = ERR_EPG_OK;
				goto EXIT_CNXTTVTV_FUNC;
			}
			pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DSI].ulTransactionId = 0;
			pCnxtData->astFilterTable[EPGCNXT_FITER_SLOT_TVTV_DDB].ulTransactionId = 0;

#if defined(TVTV_USE_SVCUID)
			xsvc_epgcontext_SetAllSvcInfo_Tvtv();
#endif
#if defined(TVTV_USE_TRIPLEID)
			xsvc_epgcontext_SvcInfoListInit_Tvtv();
#endif
			/**********************************************************************************
			  *
			  * set contex
			  *
			  **********************************************************************************/
			hError = xsvc_epgcontext_CheckAstraSvcInfo_Tvtv();
			if(hError == ERR_EPG_FAIL)
			{
				HxLOG_Info("[xsvc_epgcontext_Process_Tvtv] Astra svc_info is not checked..\n");
				goto EXIT_CNXTTVTV_FUNC;
			}

			pCnxtData->ucAntId = stAstraTvtv_Info.ucAntId;
			pCnxtData->hAction = hAction;
			pCnxtData->hService = HANDLE_NULL;
			pCnxtData->eSpec = eEPG_SPEC_TVTV;
			pCnxtData->ulOrgDeliveryType = (HUINT32)eDxDELIVERY_TYPE_SAT;
			HxSTD_memcpy(&pCnxtData->stTvtvTripleId, pTvtvTripleId, sizeof(DbSvc_TripleId_t));
			pCnxtData->usTsUniqId = DB_SVC_GetTempUniqueId();
			pCnxtData->eState = eEPG_CNXT_STATUS_RUNNING;

			ulActionId = PAL_PIPE_GetActionId(hAction);
			if(ulActionId >= NUM_MAX_ACTION)
			{
				HxLOG_Error("[xsvc_epgcontext_Process_Tvtv] GetActionId fail!!\n");
				goto EXIT_CNXTTVTV_FUNC;
			}
			pCnxtData->ulActionId = ulActionId;

			hError = PAL_PIPE_GetResourceId(ulActionId, eRxRSCTYPE_DEMUX, &ulDemuxId);
			if(hError != ERR_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_Process_Tvtv] GetResourceId fail!!\n");
				goto EXIT_CNXTTVTV_FUNC;
			}
			pCnxtData->ulDemuxId = ulDemuxId;

			hError = xsvc_epgdblib_GetEventPoolSize(&ulEventCount, &ulExtCount);
			if(ulEventCount == 0 || ulExtCount == 0)
			{
				VK_TASK_Sleep(1500);
				hError = xsvc_epgdblib_CreateEpgPool();
				if(hError != ERR_EPG_OK)
				{
					HxLOG_Error("[xsvc_epgcontext_Process_Tvtv] xsvc_epgdblib_CreateEpgPool fail!!\n");
					goto EXIT_CNXTTVTV_FUNC;
				}
			}

			/**********************************************************************************
			  *
			  * start filer
			  *
			  **********************************************************************************/
			HxLOG_Info("[xsvc_epgcontext_Process_Tvtv] local_cnxttvtv_StartFilter\n");
			hError = xsvc_epgcontext_StartFilterForPmtSignaling_Tvtv(pCnxtData);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_Process_Tvtv] xsvc_epgcontext_StartFilterForPmtSignaling_Tvtv fail!!\n");
				goto EXIT_CNXTTVTV_FUNC;
			}
			break;

		case EPG_CMD_PAT_RECEIVED :
			HxLOG_Info("[xsvc_epgcontext_Process_Tvtv] EPG_CMD_PAT_RECEIVED\n");
			usResult = (HUINT16)pstMsg->ulParam3;
			if(usResult !=(HUINT32)eSiFilterResult_Complete )
			{
				HxLOG_Error("[xsvc_epgcontext_Process_Tvtv] eSiFilterResult_Complete fail!!\n");
				goto EXIT_CNXTTVTV_FUNC;
			}
			hError = xsvc_epgcontext_ProcessPAT_Tvtv(pCnxtData);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("[xsvc_epgcontext_Process_Tvtv] xsvc_epgcontext_ProcessPAT_Tvtv fail!!\n");
				goto EXIT_CNXTTVTV_FUNC;
			}
			break;

		case EPG_CMD_PMT_RECEIVED :
			usResult = (HUINT16)pstMsg->ulParam3;
			if(usResult !=(HUINT32)eSiFilterResult_Complete )
			{
				HxLOG_Error("[xsvc_epgcontext_Process_Tvtv] eSiFilterResult_Complete fail!!\n");
				goto EXIT_CNXTTVTV_FUNC;
			}
			hError = xsvc_epgcontext_ProcessPMT_Tvtv(pCnxtData);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_Process_Tvtv] xsvc_epgcontext_ProcessPMT_Tvtv fail!!\n");
				goto EXIT_CNXTTVTV_FUNC;
			}
			break;

		case EPG_CMD_DSMCC_CHANGE :
			HxLOG_Info("[xsvc_epgcontext_Process_Tvtv] EPG_CMD_DSMCC_CHANGE\n");
			hError = xsvc_epgcontext_StopAllFilter_Tvtv(pCnxtData);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("[xsvc_epgcontext_Process_Tvtv] xsvc_epgcontext_StopAllFilter_Tvtv fail!!\n");
			}

			hError = PAL_DSMCC_FreeDsmcc();
			if(hError != ERR_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_Process_Tvtv] PAL_DSMCC_FreeDsmcc fail!!\n");
				goto EXIT_CNXTTVTV_FUNC;
			}

			hError = xsvc_epglib_InitBuffer_Tvtv();
			if(hError != ERR_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_Process_Tvtv] xsvc_epglib_InitBuffer_Tvtv fail!!\n");
				goto EXIT_CNXTTVTV_FUNC;
			}

			HxLOG_Info("[xsvc_epgcontext_Process_Tvtv] local_cnxttvtv_StartFilter\n");
			hError = xsvc_epgcontext_StartFilterForPmtSignaling_Tvtv(pCnxtData);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_Process_Tvtv] xsvc_epgcontext_StartFilterForPmtSignaling_Tvtv fail!!\n");
				goto EXIT_CNXTTVTV_FUNC;
			}
			break;

		case EPG_CMD_STOP :
			HxLOG_Info("[xsvc_epgcontext_Process_Tvtv] EPG_CMD_STOP (%d)\n", xsvc_epgcontext_GetSystemTime_Tvtv()%1000);
			hError = xsvc_epgcontext_StopAllFilter_Tvtv(pCnxtData);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_Process_Tvtv] xsvc_epgcontext_StopAllFilter_Tvtv fail!!\n");
			}
			hError = PAL_DSMCC_FreeDsmcc();
			if(hError != ERR_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_Process_Tvtv] PAL_DSMCC_FreeDsmcc fail!!\n");
				goto EXIT_CNXTTVTV_FUNC;
			}

#if 1 // tvtv epg에 의한 memory leak 수정사항.
			if(s_pstDownloadDsmccBuffer)
			{
				xsvc_epgcontext_FreeDownloadedFile_Tvtv();
			}
#endif

			hError = xsvc_epglib_InitBuffer_Tvtv();
			if(hError != ERR_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_Process_Tvtv] xsvc_epglib_InitBuffer_Tvtv fail!!\n");
				goto EXIT_CNXTTVTV_FUNC;
			}

			hError = SVC_EPG_MgrPostMessage(eSEVT_EPG_TVTV_DONE, 0, 0, 0, 0);
			if(hError != ERR_TASKUTIL_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_Process_Tvtv] SVC_EPG_MgrPostMessage fail!!\n");
			}
			hError = xsvc_epgcontext_InitControlProcess_Tvtv();
			if(hError != ERR_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_Process_Tvtv] xsvc_epgcontext_InitControlProcess_Tvtv fail!!\n");
			}

			break;

		case EPG_CMD_STOP_FILTER :
			HxLOG_Info("[xsvc_epgcontext_Process_Tvtv] EPG_CMD_STOP_FILTER (%d)\n", xsvc_epgcontext_GetSystemTime_Tvtv()%1000);
			hError = xsvc_epgcontext_StopAllFilter_Tvtv(pCnxtData);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_Process_Tvtv] xsvc_epgcontext_StopAllFilter_Tvtv fail!!\n");
				goto EXIT_CNXTTVTV_FUNC;
			}
			hError = PAL_DSMCC_FreeDsmcc();
			if(hError != ERR_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_Process_Tvtv] PAL_DSMCC_FreeDsmcc fail!!\n");
				goto EXIT_CNXTTVTV_FUNC;
			}
			break;
		case EPG_CMD_STOP_TVTV_ACTION :
			HxLOG_Info("[xsvc_epgcontext_Process_Tvtv] EPG_CMD_STOP_TVTV_ACTION\n");
#if			defined(CONFIG_TVTV_CONTROL)
			hError = xsvc_epgcontext_ControlAction_Tvtv(ulCnxtCommand, FALSE, &bPass);
			if(hError != ERR_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_Process_Tvtv] xsvc_epgcontext_ControlAction_Tvtv fail!!\n");
			}
			hError = SVC_EPG_MgrPostMessage(eSEVT_EPG_TVTV_STOP_ACTION, 0, 0, 0, 0);
			if(hError != ERR_TASKUTIL_OK)
			{
				HxLOG_Error("[xsvc_epgcontext_Process_Tvtv] SVC_EPG_MgrPostMessage fail!!\n");
			}
#endif
			break;
		default :
			break;


	}

	VK_TASK_Sleep(100);

	hError = ERR_EPG_OK;

EXIT_CNXTTVTV_FUNC :
	if(pTvtvTripleId)
	{
		OxEPG_Free(pTvtvTripleId);
		pTvtvTripleId = NULL;
	}

	return hError;

}

STATIC svcEpg_RawProcessDataInit_t	s_astEPGProcessInitTable_Tvtv[] =
{
	{
		eEPG_PROCESS_TVTV,
		eEPG_CNXT_EMPTY,
		xsvc_epgcontext_Process_Tvtv,
		eEPG_SPEC_TVTV,
	},
	{
		eEPG_PROCESS_TVTV,
		eEPG_CNXT_NOTUSED,
		NULL,
		eEPG_SPEC_TVTV,
	},

	{
		eEPG_PROCESS_EIT_PF,
		eEPG_CNXT_EMPTY,
		xsvc_epgcontext_PfProcess_Base,
		eEPG_SPEC_BASE,
	},
	{
		eEPG_PROCESS_EIT_PF,
		eEPG_CNXT_NOTUSED,
		NULL,
		eEPG_SPEC_BASE,
	},
	{
		eEPG_PROCESS_EIT_SCH,
		eEPG_CNXT_EMPTY,
		xsvc_epgcontext_SchProcess_Base,
		eEPG_SPEC_BASE,
	},
	{
		eEPG_PROCESS_EIT_SCH,
		eEPG_CNXT_NOTUSED,
		NULL,
		eEPG_SPEC_BASE,
	},
};

svcEpg_RawProcessDataInit_t*	xsvc_epgcontext_GetEpgProcessInitTable_Tvtv(HUINT32* ulNumberOfTable)
{
	*ulNumberOfTable = sizeof(s_astEPGProcessInitTable_Tvtv)/sizeof(svcEpg_RawProcessDataInit_t);
	return s_astEPGProcessInitTable_Tvtv;
}

HERROR xsvc_epg_CreateBuffer_Tvtv(void)
{
	return xsvc_epglib_CreateBuffer_Tvtv();
}

HERROR xsvc_epg_CreateTvstInfo_Tvtv(void)
{
	return xsvc_epglib_CreateTvstInfo_Tvtv();
}

HERROR xsvc_epg_CreateDgktInfo_Tvtv(HBOOL bNeedConverting)
{
	return xsvc_epglib_CreateDgktInfo_Tvtv(bNeedConverting);
}

HERROR	xsvc_epg_CreateTvtvSystemInfo_Tvtv(HBOOL bNeedConverting)
{
	return xsvc_epglib_CreateTvtvSystemInfo_Tvtv(bNeedConverting);
}

HERROR	xsvc_epg_CreateTsmtInfo_Tvtv(HBOOL bNeedConverting)
{
	return xsvc_epglib_CreateTsmtInfo_Tvtv(bNeedConverting);
}

HERROR	xsvc_epg_CreateLtotInfo_Tvtv(HBOOL bNeedConverting)
{
	return xsvc_epglib_CreateLtotInfo_Tvtv(bNeedConverting);
}

HERROR	xsvc_epg_CreateEpgInfo_Tvtv(HBOOL bNeedConverting)
{
	return xsvc_epglib_CreateEpgInfo_Tvtv(bNeedConverting);
}

void xsvc_epg_InitBuffer_Tvtv(void)
{
	(void)xsvc_epglib_InitBuffer_Tvtv();
}

HERROR xsvc_epg_RemoveInfoDirectory_Tvtv(void)
{
	return xsvc_epgcontext_RemoveDirectory_Tvtv();
}

HERROR XSVC_EPG_RemoveInfoDirectory_Tvtv(void)
{
	return xsvc_epg_RemoveInfoDirectory_Tvtv();
}

/*********************** End of File ******************************/
