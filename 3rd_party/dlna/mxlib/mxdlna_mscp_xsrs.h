
#ifndef ____MXDLNA_MSCP_XSRS_H__
#define ____MXDLNA_MSCP_XSRS_H__

// Warning: mxdlna_mscp.h 에서만 include 하도록 한다.
#include "mxdlna_struct_xsrs.h"

#ifdef __cplusplus
extern "C"{
#endif



#ifdef XSRS_CP_SUPPORT
// ####################################################################

#define MXDLNA_MSCP_XSRS_INTARGS_INIT_VAL	4294967295		// 정수형 매개변수의 초기값


// Return Value
#define MSCP_XSRSACTION_OK									(0)
#define MSCP_XSRSACTION_SUCCESS								(1)
#define MSCP_XSRSACTION_ERROR								(-100)
#define MSCP_XSRSACTION_ERROR_PARAMISNULL					(MSCP_XSRSACTION_ERROR-1)
#define MSCP_XSRSACTION_ERROR_UDNISNULL						(MSCP_XSRSACTION_ERROR-2)
#define MSCP_XSRSACTION_ERROR_COULDNT_FIND_MEDIASERVER		(MSCP_XSRSACTION_ERROR-3)
#define MSCP_XSRSACTION_ERROR_DONTHAVEACTION				(MSCP_XSRSACTION_ERROR-4)
#define MSCP_XSRSACTION_ERROR_WRONGPARAM					(MSCP_XSRSACTION_ERROR-5)




// Refer to SP_RL Part 3 X_SRS for HDRL Protocol Spec 1.1.0.0 by Sony
enum MXDLNA_MSCP_XSRS_ERROR
{
	MXDLNA_MSCP_XSRS_ERROR_INVALID_SYNTAX						= 801, //	Invalid Syntax
	MXDLNA_MSCP_XSRS_ERROR_INSUFFICIENT_PROPERTIES				= 802, //	Insufficient Properties
	MXDLNA_MSCP_XSRS_ERROR_INVALID_VALUE						= 803, //	Invalid Value
	MXDLNA_MSCP_XSRS_ERROR_NOSUCH_RECORDSCHDULEID				= 804, //	No such RecordSchduleID
	MXDLNA_MSCP_XSRS_ERROR_RECORDSCHEDULEIS_ACTIVELYRECORDING	= 805, //	RecordSchedule is actively recording
	MXDLNA_MSCP_XSRS_ERROR_UNSUPPORTED_OR_INVALIDSORTCRITERIA	= 809, //	Unsupported or invalid sort criteria
	MXDLNA_MSCP_XSRS_ERROR_CANNOTPROCCES_THEREQUEST				= 820, //	Cannot Procces the Request
	MXDLNA_MSCP_XSRS_ERROR_CONFLICT								= 830, //	Conflict
	MXDLNA_MSCP_XSRS_ERROR_PROTECTED_CONTENTS					= 831, //	Protected Contents
	MXDLNA_MSCP_XSRS_ERROR_TOOMANY_RECORDSCHEDULES				= 836, //	Too Many recordSchedules
	MXDLNA_MSCP_XSRS_ERROR_DIRECT_RECORDING						= 841, //	Direct Recording
	MXDLNA_MSCP_XSRS_ERROR_SYSTEM_STARTINGUP					= 845, //	System starting up
	MXDLNA_MSCP_XSRS_ERROR_INVALIDVALUETO_EDITRECORDSCEHDULE	= 853, //	Invalid Value to edit RecordScehdule
	MXDLNA_MSCP_XSRS_ERROR_OTHERERROR_ABOUTRECORDSCHEDULE		= 860, //	Other Error about Record Schedule
};





enum MXDLNA_MSCP_XSRS_BROWSEFLAG
{
	MXDLNA_MSCP_XSRS_BROWSEFLAG_RECORDSCHEDULE = 0,	// RecordSchedule
	MXDLNA_MSCP_XSRS_BROWSEFLAG_RECORDTITLE			// RecordTask
};

enum MXDLNA_MSCP_XSRS_SERVICETYPE
{
	MXDLNA_MSCP_XSRS_X_SCHEDULEDRECORDINGEXT	= 0,	// JLab Spec 호환
	MXDLNA_MSCP_XSRS_X_SCHEDULEDRECORDING	
};



// -----------------------------------------------
// API ARG STRUCTURE +++ 
// -----------------------------------------------
typedef struct _MXDLNA_MSCP_XSRS_ARGS_BROWSE
{
	enum MXDLNA_MSCP_XSRS_BROWSEFLAG BrowseFlag;	
	char *Filter;             
	MX_UINT32 StartingIndex;
	MX_UINT32 RequestedCount;
	char *SearchCriteria;
	char *SortCriteria;	
}MXDLNA_MSCP_XSRS_ARGS_BROWSE;


typedef struct _MXDLNA_MSCP_XSRS_ARGS_XCREATERECORDSCHEDULE
{	
	// Required
	char * scheduledStartDateTime;	
	unsigned long  scheduledDuration;
	char * scheduledChannelID;			
	unsigned long scheduledChannelID_channelType;		// 999: receive channel or external input or record device internal content
	unsigned long scheduledChannelID_broadcastingType;	// 200: Network input, 0: Not use, 1~199,201~	
	XSRS_BOOL priorityFlag;								// Required, 0 : if conflict , error return. 1 : if conflict , just insert recordschedule list

	// Optional	
	unsigned long scheduledChannelID_branchNum;			// use condition: if( xsrs:scheduledChannelID@channelType == 999(receive channel) )
	char * title;					

	// 20120611 for change type int to char *
	//int scheduledConditionID;							// range is 0~999 number.          JEITA: If use , MUST set 1
	char * scheduledConditionID;						// range is 0~999 number.          JEITA: If use , MUST set 1, JEITA added value 'mv1'
	
	char * desiredMatchingID;
	char * desiredMatchingID_type;						// Allowed Value: "SI_PROGRAMID"
	enum MXDLNAXSRS_DESIREDQUALITYMODE desiredQualityMode;	// Allowed Value: 100: DR(Digital broadcasting's Direct recording), Reserved: 0~99, 101~
	char * genreID;		
	enum MXDLNAXSRS_GENRETYPE genreID_type;
	char * reservationCreatorID;						// 2000: user rec, 2220: user rec reservation/remote reservation/reservation from the home network 

	char * recordDestinationID;
}MXDLNA_MSCP_XSRS_ARGS_XCREATERECORDSCHEDULE;


typedef struct _MXDLNA_MSCP_XSRS_ARGS_XGETCONFLICTLIST
{	
	// Required
	char * scheduledStartDateTime;	
	unsigned long  scheduledDuration;
	char * scheduledChannelID;		
	unsigned long scheduledChannelID_channelType;		// 999: receive channel or external input or record device internal content
	unsigned long scheduledChannelID_broadcastingType;	// 200: Network input, 0: Not use, 1~199,201~	

	// Optional	
	unsigned long scheduledChannelID_branchNum;			// use condition: if( xsrs:scheduledChannelID@channelType == 999(receive channel) )
	char * title;										// ONLY JLAB SUPPORT, JEITA DO NOT SUPPORT
	
	// 20120611 for change type int to char *
	//int scheduledConditionID;							// ONLY JLAB SUPPORT, range is 0~999 number. , JEITA DO NOT SUPPORT
	char * scheduledConditionID;						// ONLY JLAB SUPPORT, range is 0~999 number. , JEITA DO NOT SUPPORT
	
	char * desiredMatchingID;							// ONLY JLAB SUPPORT, JEITA DO NOT SUPPORT
	char * desiredMatchingID_type;						// ONLY JLAB SUPPORT, Allowed Value: "SI_PROGRAMID" , JEITA DO NOT SUPPORT
	enum MXDLNAXSRS_DESIREDQUALITYMODE desiredQualityMode;		// ONLY JLAB SUPPORT, Allowed Value: 100: DR(Digital broadcasting's Direct recording), Reserved: 0~99, 101~ , JEITA DO NOT SUPPORT
	char * genreID;										// ONLY JLAB SUPPORT, JEITA DO NOT SUPPORT
	enum MXDLNAXSRS_GENRETYPE genreID_type;						// ONLY JLAB SUPPORT, JEITA DO NOT SUPPORT
}MXDLNA_MSCP_XSRS_ARGS_XGETCONFLICTLIST;



// X_UpdateRecordSchedule Argument
typedef struct _MXDLNA_MSCP_XSRS_ARGS_XUPDATERECORD
{
	char * id;			// Required : XSRS @id
	int durationPerSec;	// ScheduledDuration
	int priorityFlag;	// JEITA SPEC is support this Option.  - 0 set
}MXDLNA_MSCP_XSRS_ARGS_XUPDATERECORD;



enum MXDLNA_MSCP_XSRS_MEDIATYPE
{
	MXDLNA_MSCP_XSRS_MEDIATYPE_CONTAINER	= 1,
	MXDLNA_MSCP_XSRS_MEDIATYPE_DEFAULTITEM	= 2,
	MXDLNA_MSCP_XSRS_MEDIATYPE_AUDIOITEM	= 4,
	MXDLNA_MSCP_XSRS_MEDIATYPE_VIDEOITEM	= 8,
	MXDLNA_MSCP_XSRS_MEDIATYPE_IMAGEITEM	= 16,
};



typedef struct _MXDLNA_MSCP_XSRS_ARGS_CREATEOBJECT_MOREINFO
{
	int bUseDubbing;
	unsigned long  duration;
	MX_INT64 clearTextSize;
}MXDLNA_MSCP_XSRS_ARGS_CREATEOBJECT_MOREINFO;


// CreateObject Argument
typedef struct _MXDLNA_MSCP_XSRS_ARGS_CREATEOBJECT
{
	// Upload할 ParentID, AnyContainer인경우, "DLNA.ORG_AnyContainer"를 입력
	char *ContainerID;		

	// control point로부터의 액세스를 제한한다. 
	// 본 속성값이 "1”일 때 네트워크를 통한 해당 타이틀 삭제 불가 (속성의 read만 가능) "0"일 때 삭제 가능으로 한다.
	// 업로드 녹화시는 "0"을 삽입한다.
	int Restricted;

	// 이벤트명을 나타낸다. 
	// 원칙적으로 EIT의 이벤트 루프에 포함되는 단형식 이벤트 기술자의, 제1 루프에 포함되는 event_name_char의 정보를 삽입한다. 
	// 알 수 없는 경우, DLNA가이드 라인에 따라, upnp:channelName와 동일한 문자열을 삽입한다. 
	// 또한, channel item의 경우는 삽입하는 정보는 EIT 변경에 맞추어 갱신하는 것이 바람직하다.
	char *Title;


	enum MXDLNA_MSCP_XSRS_MEDIATYPE mediaClass;

	// 채널 item가 「ARIB STD-B21 9.2 IP 인터페이스 사양」및 「JCTEA STD-007-5. 39.2 IP 인터페이스 사양」에 준수한 것을 나타낸다. 
	// 축적 콘텐츠의 경우 property가 「JCTEA STD-007-5 . 3　9.2 IP인터페이스 사양」에 준수한 것을 나타낸다. 
	// 두 경우 모두, CATV 수신기 및 그 콘텐츠는 “ARIB_TB”, “ARIB_BS”, ”ARIB_CS” "CATV_CB" 또는"CATV_JC"를 반각 문자열로 기술한다.
	char *Arib_ObjectType;

	// dc:title로 표시된 이벤트가 속하는 장르를 나타낸다. 
	// 원칙적으로 EIT의 이벤트 루프에 포함되는 컨텐트 기술자에 포함되는 content_nibble_level_1(대분류)의 값으로부터 JCTEA STD-003에 기재된 기술에 문자열 변환해서 사용한다.
	char * Genre;

	// 서비스(편성 채널) 명을 나타낸다. 원칙적으로 SDT서비스 루프에 포함되는 서비스 기술자의 제2루프에 포함되는 char 정보를 삽입한다. 
	// char를 알 수 없는 경우 upnp:channelNr의 값을 삽입한다.
	char * ChannelName;


	// 3 자리 번호만 대상으로 한다. 
	// (원터치의 개념 없음) 원칙적으로 NIT의 제2루프(TS 루프)에 포함되는 서비스 리스트 기술자의 service_id를 이용한다. 
	// 단, 하위 14비트를 10 진 변환한 값을 공개하는 프로그램 번호로 삽입한다.
	// NIT를 취득할 수 없는 경우 이전 선국 설정되던 3 자리 번호를 삽입하는 것이 바람직하다.
	int ChannelNr;


	// 녹화기 측에서 예약 오브젝트와 녹화 콘텐츠를 대응시킬 때 사용한다.
	// X_CreateRecordSchedule액션에 의해서 REC 로부터 반환된 RecordScheduleID의 값을 삽입 한다.
	char * XSrs_ReservationID;
	
	// 예약 녹화시는 업로드 시작시각이 아니라 예약된 녹화 시작시각을 반드시 삽입한다.
	// 즉시 녹화시는 CDS:CreateObject를 리퀘스트 한 시각 정보를 삽입한다. 
	// 단, 즉시 녹화시의 해당 속성 삽입에 대해서는 임의로 한다. 
	// 시각 동기 하고 있는 DMS는 해당 콘텐츠의 축적 시작시에 STB로부터 전달 받은 해당 속성의 시각 정보, 
	// 또는 자기가 보유한 시스템 시각부터 해당 콘텐츠의 축적을 시작한 시각 정보를 삽입한다. 
	// 시각 동기 하지 않은 DMS는 해당 콘텐츠의 축적 시작시에 STB로부터 해당 속성이 전달 받은 경우 그 시각 정보를 삽입한다. 
	// STB로부터 해당 속성이 전달 받지 않은 상태로 축적 시작했을 경우 자신이 보유한 시스템 시각부터 해당 콘텐츠의 축적을 시작한 시각 정보를 삽입한다. 
	// 원칙적으로 EIT의 이벤트 루프 내에 있는 대상 이벤트의 시작시각을 나타내는 start_time를 DLNA 가이드 라인에 규정된 서식으로 변환해서 삽입한다.
	char *Date;

	char *ProtocolInfo;


	// 시청 연령 제한을 나타낸다. 
	// 원칙적으로 파렌탈레이트 기술자의 rating의 값을 0xXX로 하는 16진수 문자열로 변환해서 삽입한다. 
	// Rating 값이 없는 경우 이 property는 부여하지 않다. (조건부 필수) 
	// 보충하면, 다음 식으로 제한 연령을 나타낸다. age = rating + 3 years
	// 이 연령 이상을 시청 허가.
	// 실제 운용 범위는 rating값은 0x01~0x11, age = rating + 3 years(=4세~20세), 이 연령 이상을 시청 허가.
	char * Rating;


	// 20120608 for DTCP Dubbing by jonghyun
	MXDLNA_MSCP_XSRS_ARGS_CREATEOBJECT_MOREINFO MoreInfo;
}MXDLNA_MSCP_XSRS_ARGS_CREATEOBJECT;


// -----------------------------------------------
// API ARG STRUCTURE ---
// -----------------------------------------------



// -----------------------------------------------
// API RESPONSE STRUCTURE +++
// -----------------------------------------------
typedef struct _MXDLNA_MSCP_XSRS_RESPONSE_XCREATERECORDSCHEDULE
{
	char * RecordScheduleID;
	char * ID;
	char * MediaRemainAlertID;
	char * UpdateID;
	char * RecordDestinationID;				// 20120417 for JEITA SPEC and IO Data Device, DO NOT SUPPORT JLAB
}MXDLNA_MSCP_XSRS_RESPONSE_XCREATERECORDSCHEDULE;





// X_UpdateRecordSchedule Response
typedef struct _MXDLNA_MSCP_XSRS_RESPONSE_XUPDATERECORDSCHEDULE
{
	char * ID;
	char * MediaRemainAlertID;
	char * RecordDestinationID;							// 20120417 for JEITA SPEC and IO Data Device
}MXDLNA_MSCP_XSRS_RESPONSE_XUPDATERECORDSCHEDULE;	

// -----------------------------------------------
// API RESPONSE STRUCTURE ---
// -----------------------------------------------






__MXAPI
MX_INT32 mxDlnaMscpXSrs_isSupportXSRSService( 
	MXDLNA_CP_HANDLE cp, 
	enum MXDLNA_MSCP_XSRS_SERVICETYPE ServiceType, 
	char * UDN
);


__MXAPI
MX_INT32 mxDlnaMscpXSrs_X_GetRecordScheduleList(
	MSCP_ACTION_HANDLE hAction, 
	enum MXDLNA_MSCP_XSRS_SERVICETYPE	ServiceType,
	char * pInUdn, 
	MXDLNA_MSCP_XSRS_ARGS_BROWSE* pInArgs
);


__MXAPI
MX_INT32 mxDlnaMscpXSrs_X_CreateRecordSchedule(
	MSCP_ACTION_HANDLE hAction, 
	enum MXDLNA_MSCP_XSRS_SERVICETYPE	ServiceType,
	char * pInUdn, 
	MXDLNA_MSCP_XSRS_ARGS_XCREATERECORDSCHEDULE * pInCreateRecordArgs
);

__MXAPI
MX_INT32 mxDlnaMscpXSrs_X_GetConflictList(
	MSCP_ACTION_HANDLE hAction, 
	enum MXDLNA_MSCP_XSRS_SERVICETYPE	ServiceType,
	char * pInUdn, 
	MXDLNA_MSCP_XSRS_ARGS_XGETCONFLICTLIST * pInGetConflicListArgs
);

__MXAPI
MX_INT32 mxDlnaMscpXSrs_X_DeleteRecordSchedule(
	MSCP_ACTION_HANDLE hAction, 
	enum MXDLNA_MSCP_XSRS_SERVICETYPE	ServiceType,
	char * pInUdn, 
	char * pInRecordScheduleID
);

__MXAPI
MX_INT32 mxDlnaMscpXSrs_X_UpdateRecordSchedule(
	MSCP_ACTION_HANDLE hAction, 
	enum MXDLNA_MSCP_XSRS_SERVICETYPE	ServiceType,
	char * pInUdn, 
	MXDLNA_MSCP_XSRS_ARGS_XUPDATERECORD *pInUpdateRecordArgs
);


// 20120425
__MXAPI 
MX_INT32 mxDlnaMscpXSrs_createObject(
	MSCP_ACTION_HANDLE hAction, 
	char * pInUdn,
	MXDLNA_MSCP_XSRS_ARGS_CREATEOBJECT * pInCreateObjArgs
);
#endif // XSRS_CP_SUPPORT








#ifdef __cplusplus
}
#endif


#endif // ____MXDLNA_MSCP_XSRS_H__