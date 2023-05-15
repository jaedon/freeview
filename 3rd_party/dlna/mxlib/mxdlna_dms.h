
/*******************************************************************************
 * HUMAX R&D Lab., HUMAX INC., SEOUL, KOREA
 * COPYRIGHT(c) 2010 by HUMAX Inc.
 *
 * All rights reserved. No part of this work may be reproduced, stored in a
 * retrieval system, or transmitted by any means without prior written
 * permission of HUMAX Inc.
 ******************************************************************************/



#ifndef _MXDLNA_DMS_H__
#define _MXDLNA_DMS_H__



#include "mxapi_define.h"
#include "mxlib_config.h"



#ifdef __cplusplus
extern "C"{
#endif

#if defined(WIN32) || defined(_WIN32_WCE)
#if defined(WINSOCK2)
	#include <winsock2.h>
	#include <ws2tcpip.h>
#elif defined(WINSOCK1)
	#include <winsock.h>
	#include <wininet.h>
#endif
#endif

// ###############################################
// Native DMS (Strart)
// ###############################################

// **********************************************
// include
// **********************************************
#include "mxtypes.h"
#include "mxutil.h"
#include "mxdlna_struct.h"
#include "mxdlna_define.h"
// **********************************************
// **********************************************
#include "mxdbc.h"				// db관련 헤더.. 현재는 DB를 빼고 DMS를 사용할 수 없다.
// **********************************************








// **********************************************
	// type
// **********************************************

// **********************************************
// dms support functions // protocolinfo parser reference
// **********************************************
#define DMS_SUPPROTS_TIME_BASED_SEEK		0	// DMS가 기본적으로 TimeSeek 기능을 지원하는지 유무
#define DMS_SUPPROTS_BYTE_BASED_SEEK		1	// DMS가 기본적으로 ByteSeek 기능을 지원하는지 유무
#define DMS_SUPPORTS_DLNA_PLAY_CONTAINER	0	// DMS가 DLNA의 PlayContainer URI operation 기능을 지원하는지 유무
#define DMS_SUPPORTS_HTTP_STALLING			0	// DMS 내부에서 사용하는 HTTP가 Stalling 기능을 지원하는지 유무
#define DMS_SUPPORTS_RTSP_PAUSE				0	// DMS 내부에서 사용하는 RTSP가 PAUSE 기능을 지원하는지 유무
#define DMS_SUPPORTS_MAXSPEED_MAJOR			0	// DMS가 지원하는 MAXSPEED값의 MAJOR
#define DMS_SUPPORTS_MAXSPEED_MINOR			0	// DMS가 지원하는 MAXSPEED값의 MINOR
#define DLNA_DMS_MAJOR_VERSION				1	// DMS 의 DLNA MAJOR 버전
#define DLNA_DMS_MINOR_VERSION				5	// DMS 의 DLNA MINOR 버전
// **********************************************


#define DMS_EXTENSION_UPLOAD_TEMP			".tmp"
#define DMS_EXTENSION_UPLOAD_TEMP_LEN		4


typedef void *	MXDLNA_DMS_HANDLE;
typedef char *	MXDLNA_DMS_STRING;



#define MXDLNA_DMS_CALLTYPE(x)	((x)&0xffff0000)



typedef void (*MXDLNA_DMSSTATUS_CALLBACK)(
	MXDLNADMS_CALL_STATUS call_status ,
	void * pValue
);


#ifdef DMS_SERVER_FUNC_CONFIG
// 20120328

#ifdef DLNA_USER_DEVICE_CAPABILITY
typedef struct _MXDLNA_DMS_USER_DEVICECAP
{
	const char * Namespace;			// Device Description에서 XML로 표현할 때 사용할 Namespace
	const char * NamespacePrefix;	// df
	const char * AttributeName;		// <AttributeName></AttributeName>
	const char * Value;				// <AttributeName>Value</AttributeName>

	// 2012-09-03 mklim modified - jcngs 추가 속성 구현 작업 - DLNA_USER_DEVICE_CAPABILITY 확장
	struct _MXDLNA_DMS_USER_DEVICECAP * next;
}MXDLNA_DMS_USER_DEVICECAP;
#endif // DLNA_USER_DEVICE_CAPABILITY


#ifdef DMS_USER_VIRTUAL_MULTIPLERESOURCE
// ex) 기본 URL : http://ipaddr:port/tuner/1235.ts
typedef struct _MXDLNA_DMS_VIRTUAL_MULTIRESOURCE_ITEM
{
	const char * PostFix;				// Lib에서 생성하는 기본 URL 뒤에 붙이고 싶은 값 http://ipaddr:port/tuner/1235.tsPostfix
	const char * DlnaProtocolInfo;		// ex) "http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_NTSC;DLNA.ORG_OP=10;DLNA.ORG_CI=1;DLNA.ORG_FLAGS=01500000000000000000000000000000"
	MX_INT64 Size_byte;					// Size					, 알수 없으면 0
	int Duration;						// Duration=3600		, 알수 없으면 0
	int Bitrate;						// bitrate=250000		, 알수 없으면 0
	int resX;							// 720					, 알수 없으면 0
	int resY;							// 480					, 알수 없으면 0
}MXDLNA_DMS_VIRTUAL_MULTIRESOURCE_ITEM;



// 사용자가 MXDLNA_DMS_VIRTUAL_MULTIRESOURCE 구조체에 등록하고, 라이브러리에서는 Browse() or Search()시
// 특정 MXDLNA_CDSOBJECT에 대해서, 사용자가 정의한 가상 리소스로 보여줄지 여부를 판단한다.
// 리턴값이 0이면 생성하지 않는다, 1이면 생성한다.
typedef int (*MXDLNA_DMS_BE_NEED_VIRTUALMULTIRESOURCE)(
	MXDLNA_CDSOBJECT * pObject,
	void * pMXDLNA_DMS_VIRTUAL_MULTIRESOURCE
);
// MediaTransport에서 File을 Open하기 위한 Path를 DirectiveObj를 이용하여 생성한다.
typedef int (*MXDLNA_DMS_CHANGE_VIRTUALURL)(
	char * pDirectiveObj,
	char ** ppPathX
);



typedef struct _MXDLNA_DMS_VIRTUAL_MULTIRESOURCE
{
	const char * WebDelimiter;					// tuner
	MXDLNA_DMS_VIRTUAL_MULTIRESOURCE_ITEM * pTblOfVirtualMultiResource;	// 멀티 리소스로 만들 가상 구조. 메모리 해제는 Application에서 명시적으로 수행한다.
	int szDelimiter;							// pTblOfVirtualMultiResource의 개수
	MXDLNA_DMS_BE_NEED_VIRTUALMULTIRESOURCE fnBeNeedVirtualMultiResource;
	MXDLNA_DMS_CHANGE_VIRTUALURL				fnChangeVirtualURL;
}MXDLNA_DMS_VIRTUAL_MULTIRESOURCE;
#endif // DMS_USER_VIRTUAL_MULTIPLERESOURCE



#ifdef DMS_USER_MEDIATRANSPORT_CONFIG
// 20120413
// MediaTransport에서 http://ipaddr.port/web/cmd/startTranscoding?id=123 과 같이 web/cmd 아래에 들어온 명령을 User에게 전달한다.
// 리턴값 : Lib
typedef char * (*MXDLNA_DMS_PROCESS_MEDIATRANSPORT_COMMAND)(
	char * pInUrlParam,
	int * pOutHttpRespVal,		// HTTP의 응답값으로서, 200 인 경우, 리턴값을 함께 포함해서 응답. 200 이 아닌경우 *pOutHttpRespVal만 응답에 사용
	int * pbFreeReturnVal		// Return 값을 Lib에서 Free해야 하는지 명시적으로 선언. 1이면 Lib이 Free해야 한다.
);

typedef int (*MXDLNA_DMS_CONVERT_TIME_TO_BYTES)(const char * path, MX_UINT32 inTime_ms, MX_INT64 * pOutBytes, MX_UINT32 * pOutTime_ms);
typedef int (*MXDLNA_DMS_GET_TOTAL_DURATION)(const char * path, MX_UINT32 * pOutDuration_ms);

typedef struct _MXDLNA_DMS_MEDIATRANSPORT_DATA
{
	int sizeOfMediaTransBuffer;				// 0 이면 DMS의 기본값을 사용한다.
	MXDLNA_DMS_PROCESS_MEDIATRANSPORT_COMMAND fnUserCmdProcess;	// NULL이면 UserCmd를 사용하지 않는다.

#ifdef DMS_TIMESEEK_SUPPORT_20120702
	MXDLNA_DMS_CONVERT_TIME_TO_BYTES	fnConvertTimeToBytesForByteSeek;
	MXDLNA_DMS_GET_TOTAL_DURATION		fnGetTotalDurationForByteSeek;
#endif
}MXDLNA_DMS_MEDIATRANSPORT_DATA;
#endif // DMS_USER_MEDIATRANSPORT_CONFIG

#ifdef DMS_USER_RESOURCE_MODIFY_M3U8_POSITION_EPG
typedef enum _eM3U8_POSITION
{
	eMP_NotUse = 0,
	eMP_Apply_Item_ExConfig,
	eMP_Normal,
	eMP_Remove_Origin,
	eMP_AddFront,
	eMP_MAX,
}eM3U8_POSITION;
#endif

// 20120328
typedef struct _MXDLNA_DMS_SERVER_FUNC_CONFIG
{
#ifdef DLNA_USER_DEVICE_CAPABILITY
	MXDLNA_DMS_USER_DEVICECAP			* pUserDeviceCapability;
#endif
#ifdef DMS_USER_VIRTUAL_MULTIPLERESOURCE
	MXDLNA_DMS_VIRTUAL_MULTIRESOURCE	* pUserSpecificMultiResource;
#endif

	// 20120413
#ifdef DMS_USER_MEDIATRANSPORT_CONFIG
	MXDLNA_DMS_MEDIATRANSPORT_DATA * pUserMediaTransport;
#endif

#ifdef DMS_USER_RESOURCE_MODIFY_M3U8_POSITION_EPG
	eM3U8_POSITION      nUserM3U8PositionforTuner;
#endif
#ifdef DMS_USER_RESOURCE_MODIFY_M3U8_POSITION_MEDIA
	eM3U8_POSITION      nUserM3U8PositionforMedia;
#endif

	// 20120524 jonghyun
	/* CDS:Browse()시 SOAP의 Header를 체크하여 JLab에서 요구하는, Rating을 처리할지 유무*/
	int bUseJLABParentalRating;
}MXDLNA_DMS_SERVER_FUNC_CONFIG;

#endif // DMS_SERVER_FUNC_CONFIG


//MediaServer 핸들을 생성할 때 매개변수로 넘겨주는 서버 정보로, 이를 통해 설정된 값을 사용하여 ControlPoint가 서버를 검색할 때 서버 정보로 알려준다.
typedef struct _MXDLNA_DMS_SERVER_INFO
{
	char *FriendlyName;				// ControlPoint에서 해당 서버를 지칭하는 이름
	char *UDN;						// UPnP 장치들을 구별하기 위한 이름
	char *SerialNumber;				// 장치의 시리얼 번호
	char *SortCapabilities;			// 정렬 가능한 필드들을 CSV 형태로 지정
	char *SearchCapabilities;		// 검색 가능한 필드들을 CSV 형태로 지정
	char *SourceProtocolInfo;		// 서버가 지원하는 미디어 타입을 나열함
	char *FeatureList;				// 서버가 지원하는 Feature들에 대한 리스트를 XML 문서형태로 지정

	char * manufacturer;			// 제조사
	char * manufacturerURL;			// 제조사의 설명이 들어 있는 페이지의 URL
	char * ModelName;				// 제품의 모델 이름
	char * ModelDescription;		// 모델에 대한 세부 설명
	char * ModelURL;				// 모델에 대한 더 자세한 설명이 들어 있는 페이지의 URL

	char * defaultCreator;			// 명시적이지 않은 Creator의 기본 값
	char * Corporation;				// 회사이름
	char * ModelNumber;				// 모델에 대한 넘버
	char * productCode;				// 제품 코드

	char * defaultSortCapabilities;	// 기본적인 서버의 소팅 방법
	char * uploadPath;				// DMS 내부에서 upload할 경로
	char * tmpUploadPath;			// 20091110 for 임시 파일을 스캔에서 제외하기 위해.
	char * thumbnailPath;			// DMS 내부에서 thumbnail을 저장할 경로

	unsigned short upnp_port;		// MediaServer의 UPnP 서비스들을 제어하기 위한 통신을 할 때 사용하는 포트로 Dlna 라이브러리 내부에서 모든 동작이 처리된다. 보통은 0값을 주어 임의로 설정하도록 하면되고, 특정 포트만 사용해야 하는 환경에서는 이 값을 지정해 주면 된다
	unsigned short webserver_port;		// webserver의 port
	unsigned short upnp_dtcp_port;		// dtcp의 port
	unsigned short uploadserver_port;	// upload port


	MXDLNA_DMSSTATUS_CALLBACK fnDMSStatus;
	char * Pairing;

}MXDLNA_DMS_SERVER_INFO;


typedef struct _MAKE_PROTOCOLINFO
{
	char * profile;
	char * mimeType;
	int isConvertedContent;
	MXDLNA_PROTOCOLTYPE protocol;
	int isTimeSeek;
	int isByteSeek;
	int isStream;	//sp-flag
	int isImage;
	int isProtection;

	#ifdef DMS_SUPPORTS_DLNA_PLAY_CONTAINER
	int DLNAPlayContainer;
	#endif
}MXDLNA_DMS_MAKEPROTOCOLINFO;


// 20091113 DmsIcon
typedef struct _DmsIcon
{
	char * mimeType;
	int width;
	int height;
	int depth;
	char * url;
}MXDLNA_DMS_ICON;





// 일반 스트링 문자가 하나라도 있는 경우
typedef enum _MXDLNA_DMS_SPACEINFO
{
	SPACEINFO_PARAMERR = -1,		// 입력값 오류
	SPACEINFO_NORMALSTR = 0,		// 일반 스트링 문자가 하나라도 있는 경우
	SPACEINFO_SPACESTR = 1			// 전체가 Space인 스트링
}MXDLNA_DMS_SPACEINFO;


typedef enum _MXDMS_UPLOADTYPE
{
	UT_NONE = 0,
	UT_CREATE_OBJECTNPOST,
	UT_IMPORTRESOURCE_GET,
	UT_CREATE_OBJECT_CONTAINER
}MXDMS_UPLOADTYPE;



typedef enum _MXDMS_DESTROYTYPE
{
	DT_NONE = 0,
	DT_ITEM,
	DT_CONTAINER
}MXDMS_DESTROYTYPE;




typedef enum _MXDMS_USER_PROCESS_RET
{
	MXDMS_USER_CALLBACK_NOT_INIT = 0,
	MXDMS_USER_CALLBACK_SUCCESS = 1,
	MXDMS_USER_CALLBACK_FAIL = -1,
	MXDMS_USER_CALLBACK_FAIL2 = -2,

}MXDMS_USER_CALLBACK_RET;





typedef MXDMS_USER_CALLBACK_RET (*MXDLNA_DMS_UPLOADDONE) (
	MXDMS_UPLOADTYPE uploadType,
	const char * tmpFilename,
	const char * titlePath,
	const char * parent_phy_location,
	MXDLNA_CDSOBJECT * pObject,
	char ** savedFilename
);

typedef MXDMS_USER_CALLBACK_RET (*MXDLNA_DMS_DESTROYDONE) (
	MXDMS_DESTROYTYPE destroyType,
	const char * tmpFilename,
	const char * titlePath,
	const char * parent_phy_location,
	MXDLNA_CDSOBJECT * pObject,
	int refCount
);


typedef MXDMS_USER_CALLBACK_RET (*MXDLNA_DMS_MOVEOBJECT) (
	const MXDLNA_CDSOBJECT * pCurrObject,
	const MXDLNA_CDSOBJECT * pNewParentObject,
	const int isFolder,
	const char * currentCdsTitlePath_APP,
	const char * currentRealLocation_APP,
	const char * newParentCdsTitlePath_APP,
	const char * newParentRealLocation_APP,
	char ** ppNewRealLocation_UTF8
);


typedef MXDMS_USER_CALLBACK_RET (*MXDLNA_DMS_UPDATEOBJECT) (
	const MXDLNA_CDSOBJECT * pObject,
	const int isFolder,
	const char * currentCdsTitlePath_APP,
	const char * currentRealLocation_APP,
	const char * currTitle_APP,
	const char * newTitle_APP,
	char ** ppNewRealLocation_UTF8
);

// ------------------------------------------------------------


// **********************************************



// **********************************************
// API 1
// **********************************************
/**	\brief mxDlnaDms_createDms
	\brief 새로운 MediaServer 객체를 생성한다*/

__MXAPI
MXDLNA_DMS_HANDLE mxDlnaDms_createDms (
	MXDLNA_DMS_SERVER_INFO *info
);


/**	\brief mxDlnaDms_destroyDms
	\brief MediaServer 객체의 서비스들을 중단하고 할당되었던 리소스들을 반납한다.*/
__MXAPI
MX_INT32 mxDlnaDms_destroyDms (
	MXDLNA_DMS_HANDLE dms
);


// 호출 시점: mxDlnaDms_createDms() 이후,  mxDlnaDms_startDms() 이전
// DMS를 시작한다.
/**	\brief mxDlnaDms_configure
	\brief DMS의 부가적인 기능들을 설정한다.*/
#ifdef DMS_SERVER_FUNC_CONFIG
__MXAPI
int mxDlnaDms_configure(
	MXDLNA_DMS_HANDLE hDms,
	MXDLNA_DMS_SERVER_FUNC_CONFIG * pConfig
);
#endif


/**	\brief mxDlnaDms_startDms
	\brief DMS를 시작한다.*/
__MXAPI
void mxDlnaDms_startDms(MXDLNA_DMS_HANDLE handle);


/**	\brief mxDlnaDms_duplicateString
	\brief Str과 동일한 DMS_STRING을 생성한다.*/
__MXAPI
MXDLNA_DMS_STRING mxDlnaDms_duplicateString (
	const char *str
);



/**	\brief	mxDlnaDms_checkedDuplicateString
	\brief	복사할 대상에 이미 문자열이 존재하면 해제한 후 복사한다.
	\param	target	: MXDLNA_DMS_STRING * 타입
	\param	str		: 복사할 문자열
	\return	    MXDLNA_OK  MXDLNA_FAIL MXDLNA_ERROR*/
__MXAPI
MX_INT32 mxDlnaDms_checkedDuplicateString(
	MXDLNA_DMS_STRING * target ,
	const char *str
);


/**	\brief mxDlnaDms_destroyString
	\brief Str 객체에 할당된 메모리를 반납한다*/
__MXAPI
void mxDlnaDms_destroyString (
	MXDLNA_DMS_STRING str
);



/**	\brief	mxDlnaDms_setAppCallback
	\brief	CreateObject/DestroyObject에 의해서 아이템이 생성되거나 삭제될 때의 처리할 callback 함수
	\param	handle	: MXDLNA_DMS_HANDLE 타입의 DMS 핸들
	\return	MX_INT32 타입으로 성공시 MXDLNA_OK, 실패시 MXDLNA_ERROR*/
__MXAPI
MX_INT32 mxDlnaDms_setAppCallback(
	MXDLNA_DMS_HANDLE handle,
	MXDLNA_DMS_UPLOADDONE fUploadDone,
	MXDLNA_DMS_DESTROYDONE fDestroyDone,
	MXDLNA_DMS_MOVEOBJECT fMoveObject,
	MXDLNA_DMS_UPDATEOBJECT fUpdateObject
);




__MXAPI
char * mxDlnaCds_makeProtocolInfo(
	MXDLNA_DMS_MAKEPROTOCOLINFO * pInfo
);



__MXAPI
MX_INT32 mxDlnaCds_freeMakeProtocolInfoData(
	MXDLNA_DMS_MAKEPROTOCOLINFO * pInfo
);





__MXAPI
MXDLNA_DMS_SPACEINFO mxDlnaDms_isAllSpaceString(
	const char * str ,
	const MX_UINT32 size
);


__MXAPI
const char * mxDlnaDms_GetSortCapabilities(void);
__MXAPI
const char * mxDlnaDms_GetSearchCapabilities(void);


// DMS Create 직 후 호출되어야 한다.
__MXAPI
int mxDlnaDms_addDmsIcon(
	MXDLNA_DMS_HANDLE handle ,
	MXDLNA_DMS_ICON * iconProp
);


// DMS 가 생성된 이후에 호출 되어야 한다.
__MXAPI
MX_INT32 mxDlnaDms_FriendlyRename(
	MXDLNA_DMS_HANDLE handle,
	const char * rename_utf8
);

__MXAPI
int mxDlnaDms_makeRelPath(
	const char * shareLocationUTF8,
	const char * fileLocationUTF8 ,
	char ** relPath_UTF8,
	int isSharedFolderName
);


// **********************************************
// ###############################################
// Native DMS (End)
// ###############################################










// ###############################################
// Directory Builder (start )
// ###############################################



// **********************************************
// define
// **********************************************
#ifdef WIN32
	#define FOLDER_SEP "\\"
	#define FOLDER_SEP_CH '\\'
	#define FOLDER_SEP_CH_L L'\\'
#else
	#define FOLDER_SEP "/"
	#define FOLDER_SEP_CH '/'
#endif

#define CONTAINER_DELIMITER_CH		'\\'
#define CONTAINER_DELIMITER_STR		"\\"
// **********************************************




// **********************************************
// type
// **********************************************
typedef void * DMS_DB_HANDLE;	// directoryBuilder
typedef void * DIRECTORYBUILDER_ADD_TYPE;
typedef void * DIRECTORYBUILDER_REMOVE_TYPE;
typedef void * MXDLNA_DMS_SYSTEMFILTER_INFO;

typedef enum _DirectoryBuilderFolderType
{
	SYSTEM_CONTAINER = 0,		// SYSTEM이 알아서 CDS에 아이템을 구성할 때 사용
	USER_CONTAINER,				// 사용자가 알아서 CDS에 아이템을 구성할 때 사용 (Not Yet Implementation )
}DIRECTORY_BUILDER_FOLDER_TYPE;



// SYSTEM FOLDER MGR
// System이 CDS TREE를 구성할때 힌트가되는 정보들의 종류이다
typedef enum _MXDLNA_SYSTEMFILTER_TYPE
{
	SYSTEMFILTER_FOLDER = 1,		// Scan을 한 상대 경로
	SYSTEMFILTER_ARTIST,			// 아이템의 아티스트
	SYSTEMFILTER_ALBUM,				// 아이템의 앨범
	SYSTEMFILTER_GENRE,				// 아이템의 장르
	SYSTEMFILTER_YEAR,				// 아이템이 생성된 년/월/일/시
	SYSTEMFILTER_ACTOR,				// 아이템이 알려주는 배우
	SYSTEMFILTER_RATING,			// 아이템의 등급
	SYSTEMFILTER_KEYWORD,			// 아이템의 키워드
	SYSTEMFILTER_PLAYLIST,			// 아이템의 플레이리스트 정보
	SYSTEMFILTER_PUBLISHER,			// 아이템의 출판자
	SYSTEMFILTER_TRACK,				// 아이템의 트랙정보
	SYSTEMFILTER_COMPOSER,			// 아이템의 작곡가

}MXDLNA_SYSTEMFILTER_TYPE;




// Not Yet Use
typedef struct _MXDLNA_DMS_DFTPNLIST
{
	char * mimeType;
	char * PNStr;
	struct _MXDLNA_DMS_DFTPNLIST * pNext;
}MXDLNA_DMS_DFTPNLIST;


// 20100625
typedef enum _MXDLNA_ADDRES_TYPE
{
	ADDRES_ALL_TYPE = 0,		// 모든 리소스를 다 집어 넣는다 - 실제 Physical 파일이 존재하지 않는 경우도 넣는다. 단 이때는 base64로 인코딩된 값이 보인다
	ADDRES_ONLYPHYSICAL_TYPE = 1,
}MXDLNA_ADDRES_TYPE;

// 사용자가 아이템을 추가할 때 사용하는 DMS_ADDITEM_PROP의 원소이다.
typedef struct _MXDLNA_DMS_RESOURCE
{
	char * contentURI;
	char * importURI;

	char * ifoFileUri;			// added : 090706
	char * importIfoFileUri;	// added : 090706

	char * mimeType;
	char * protocol;			// PN
	MX_INT64 size;
	long bitrate;
	long bitPerSample;
	long colorDepth;
	int duration;

	long nrAudioChannels;
	char * protection;			// drmtype
	MX_INT32 resolutionX;
	MX_INT32 resolutionY;
	long sampleFrequency;

	char * userData;			// usrdata="sample:http://123123" >
	struct _MXDLNA_DMS_RESOURCE * pNext;

	int dtcp_emi;				//char dtcp_emi;	20120405 changed by jonghyun
	long dtcp_pcplength;
	MXDLNA_ADDRES_TYPE resAddType;

}MXDLNA_DMS_RESOURCE;




#define DMS_ADDITEM_EXTERN_CONFIG_M3U8_USE                           0x04
#define DMS_ADDITEM_EXTERN_CONFIG_M3U8_POS_BASIC					 0x00
#define DMS_ADDITEM_EXTERN_CONFIG_M3U8_POS_REMOVE_SOURCE			 0x01
#define DMS_ADDITEM_EXTERN_CONFIG_M3U8_POS_ADDFRONT					 0x02
#define DMS_ADDITEM_EXTERN_CONFIG_M3U8_POS_FILTER					 0x03


// 사용자가 scanFilter에서 mxDlnaDmsDb_addCdsItem() 함수를 부를 때 집어넣을 아이템의 속성으로 사용해야 하는 값이다
typedef struct _MXDLNA_DMS_ADDITEM_PROP
{
	char * title;			// 생성하게 될 객체의 title
	char * creator;			// 생성하게 될 객체의 creatro

	char * localUrl;		// 미디어를 전송하기 위해서 대표가 되는 url
	char * uuid;
	char * mimeType;		// 위의 localUrl이 가르키는 아이템의 mimetype

	MX_INT64 size;			// 파일의 사이즈
	MXDLNA_DMS_SYSTEMFILTER_INFO * treeInfo;		// CDS TREE를 구성하는 정보가 된다
	MXDLNA_DMS_RESOURCE * pRes;					// 생성하는 객체의 리소스를 구성하게 될 정보

	//void * userData;		// 사용자 데이터
	char * subTitleUri;		// 자막이 존재하는 곳의 Uri
	char * classType;		// classType을 사용자가 직접 명시하고 싶을 때 사용한다

	int mediaType;
	int protection;

	char * searchRootPath;

	char * albumarturi;

	// 20100924 - CDSObject로는 생성하지 말것
	// 특정 밴더의 경우 자막 파일 처리의 경우 내부 리소스로는 사용되야 하지만
	// CDSObject로 Expose 하는것을 원하지 않기 때문
	int objectNotCreate;

	// Folder Container를 생성할지 유무
	int bCreateFolderContainer;

#if  defined(DMS_USER_RESOURCE_MODIFY_M3U8_POSITION_EPG) || defined(DMS_USER_RESOURCE_MODIFY_M3U8_POSITION_MEDIA)
	int nExConfig;
#endif
	char *Channel_Additional_Info;
	int isRecordingsFolder;	// 2015/12/18 mhkang: http://svn.humaxdigital.com:3000/issues/109277#note-19

}MXDLNA_DMS_ADDITEM_PROP;



// 사용자가 scanFilter에서 mxDlnaDmsDb_removeCdsItem() 함수를 부를 때 집어넣을 아이템의 속성으로 사용해야 하는 값이다
typedef struct _MXDLNA_DMS_REMOVE_PROP
{
	char * savedUri;	// 실제 저장된 위치 R , Main Key
	int bReserved1;
}MXDLNA_DMS_REMOVE_PROP;
// **********************************************




// **********************************************
// APIs
// **********************************************
/**	\brief	database 핸들 생성
	\return	DMS_DB_HANDLE	*/
__MXAPI	DMS_DB_HANDLE mxDlnaDmsDb_createHandle(	MXDBC_DB_INFO * pDb_info);

/**	\brief	database 파일 생성
*/
__MXAPI	MX_BOOL mxDlnaDmsDb_createFile(MXDBC_DB_INFO * pDb_info);

/**	\brief	database 초기화
	\return	MX_INT32	*/
__MXAPI	MX_INT32 mxDlnaDmsDb_initialize(void);

/**	\brief	database 제거
	\return	MX_INT32*/
__MXAPI	MX_INT32 mxDlnaDmsDb_destroyHandle(void);


/**	\brief	DirectoryBuilder Rebuild
	\return	MX_INT32	*/
__MXAPI	MX_INT32 mxDlnaDmsDb_rebuild(void);



// **********************************************
// CDS 데이터를 삽입 삭제 하기 위해 필요한 구조 만드는 APIs
// **********************************************
/**	\brief	mxDlnaDmsDb_newAddItemProp
	\brief DMS의 CDS에 Object를 생성하기 위해서 기반이되는 아이템을 생성하는 함수 이다.

	\param	localUrl	: 파일의 실제 위치
	\param	title		: 파일의 타이틀( 꼭 파일명일 필요는 없다 )
	\param	size		: 파일의 사이즈
	\return	MXDLNA_DMS_ADDITEM_PROP *	*/
__MXAPI
MXDLNA_DMS_ADDITEM_PROP * mxDlnaDmsDb_newAddItemProp(
	char * localUrl ,
	char * title ,
	MX_INT64 size
);


/**	\brief	mxDlnaDmsDb_freeAddItemProp
	\brief	mxDlnaDmsDb_newAddItemProp()을 이용하여 만든 MXDLNA_DMS_ADDITEM_PROP 파일을 제거하는 함수 이다
	\param	pData	: MXDLNA_DMS_ADDITEM_PROP * 타입의 데이터 이다
	\return	MX_INT32	*/
__MXAPI
MX_INT32 mxDlnaDmsDb_freeAddItemProp(
	MXDLNA_DMS_ADDITEM_PROP * pData
);

// 20091029 added
__MXAPI
MX_INT32 mxDlnaDmsDb_freeResourceInfo(
	MXDLNA_DMS_RESOURCE * pRes
);

/**	\brief	mxDlnaDmsDb_changeTitle
	\brief	MXDLNA_DMS_ADDITEM_PROP 데이터를 처리하는 중간에 title 값을 변경하고 싶을때 사용하는 함수
	\param	pData	: mxDlnaDmsDb_newAddItemProp() 함수로 생성된 값을 사용한다.
	\param	title	: 변경할 문자열을 입력한다.
	\return	MX_INT32	*/
__MXAPI
MX_INT32 mxDlnaDmsDb_changeTitle(
	MXDLNA_DMS_ADDITEM_PROP * pData,
	char * title
);



/**	\brief	mxDlnaDmsDb_addCreator
	\brief	creator를 추가하는 함수 이다.
	\param	pData	: mxDlnaDmsDb_newAddItemProp() 함수로 생성된 값을 사용한다.
	\param	creator	: 추가할 creator를 입력한다.
	\return	MX_INT32	*/
__MXAPI
MX_INT32 mxDlnaDmsDb_addCreator(
	MXDLNA_DMS_ADDITEM_PROP * pData,
	char * creator
);

/**	\brief	mxDlnaDmsDb_addResProp
	\brief	MXDLNA_DMS_ADDITEM_PROP 데이터에 MXDLNA_DMS_RESOURCE를 추가한다.
	\param	inpHandle	: mxDlnaDmsDb_newAddItemProp() 함수로 생성된 값을 사용한다.
	\param	res			: 추가할 MXDLNA_DMS_RESOURCE 데이터
	\return	MX_INT32	*/
__MXAPI
MX_INT32 mxDlnaDmsDb_addResProp(
	MXDLNA_DMS_ADDITEM_PROP * inpHandle ,
	MXDLNA_DMS_RESOURCE * pRes
);


/**	\brief	mxDlnaDmsDb_setTreeInfo
	\brief	MXDLNA_DMS_ADDITEM_PROP 데이터에 MXDLNA_SYSTEMFILTER_TYPE 타입을 설정하고 데이터를 추가한다.
	\brief	추가한 데이터는 mxDlnaDmsDb_getTreeInfo() 함수를 이용하여 확인 할 수 있다.

	\param	handle	: mxDlnaDmsDb_newAddItemProp() 함수로 생성된 값을 사용한다.
	\param	type	: MXDLNA_SYSTEMFILTER_TYPE의 열거형 값 ( SYSTEMFILTER_GENRE , SYSTEMFILTER_FOLDER ... )
	\param	pAddData	: 추가할 문자열 데이터

	\return	MX_INT32	*/
__MXAPI
MX_INT32 mxDlnaDmsDb_setTreeInfo(
	MXDLNA_DMS_ADDITEM_PROP * handle ,
	MXDLNA_SYSTEMFILTER_TYPE type ,
	char * pAddData
);



/**	\brief	mxDlnaDmsDb_getTreeInfo
	\brief	MXDLNA_DMS_ADDITEM_PROP데이터 안에 들어 있는 MXDLNA_SYSTEMFILTER_INFO 데이터의 내용을 얻어 오는 함수 이다.
	\brief	mxDlnaDmsDb_setTreeInfo()로 설정한 값을 확인 할 수 있다.

	\param	pInfo		: MXDLNA_SYSTEMFILTER_INFO 데이터 타입으로 MXDLNA_DMS_ADDITEM_PROP데이터 안에 들어 있다.
	\param	FindType	: MXDLNA_SYSTEMFILTER_TYPE의 열거형 값 ( SYSTEMFILTER_GENRE , SYSTEMFILTER_FOLDER ... )

	\return	char *		: 넘겨 받을 문자열	*/
__MXAPI
char * mxDlnaDmsDb_getTreeInfo(
	MXDLNA_DMS_ADDITEM_PROP * pInfo,
	MXDLNA_SYSTEMFILTER_TYPE FindType
);





/**	\brief	mxDlnaDmsDb_newRemoveProp
	\brief	DMS의 CDS에 Object를 제거하기 위해서 기반이되는 아이템을 생성하는 함수 이다.
	\brief	mxDlnaDmsDb_freeRemoveProp() 함수를 이용해서 제거한다.
	\param	uri	: 파일이 존재하는 곳의 URI
	\return	MXDLNA_DMS_REMOVE_PROP * : 메모리가 할당되어서 생성된다.*/
__MXAPI
MXDLNA_DMS_REMOVE_PROP * mxDlnaDmsDb_newRemoveProp(
	char * uri
);

/**	\brief	mxDlnaDmsDb_freeRemoveProp
	\brief	mxDlnaDmsDb_newRemoveProp() 함수로 생성된 데이터를 제거하는 함수 이다.
	\param	data	: MXDLNA_DMS_REMOVE_PROP 타입의 데이터 이다.

	\return	MX_INT32	*/
__MXAPI
MX_INT32 mxDlnaDmsDb_freeRemoveProp(
	MXDLNA_DMS_REMOVE_PROP * data
);
// **********************************************


// 사용자가 특정 경로에 CdsItem을 넣는 함수.
__MXAPI
MX_INT32 mxDlnaDmsDb_addCdsItem(
	MXDLNA_DMS_HANDLE handle,
	DMS_DB_HANDLE h_db,
	DIRECTORYBUILDER_ADD_TYPE inpData ,
	char * path,	// USER_CONTAINER  전용..
	DIRECTORY_BUILDER_FOLDER_TYPE type
);


__MXAPI
MX_INT32 mxDlnaDmsDb_addCdsContainer(
	MXDLNA_DMS_HANDLE handle,
	DMS_DB_HANDLE h_db,
	DIRECTORYBUILDER_ADD_TYPE inpData,
	char * path,
	DIRECTORY_BUILDER_FOLDER_TYPE type
);


//----------------------------------------
__MXAPI
MX_INT32 mxDlnaDmsDb_removeCdsItem(
//----------------------------------------
	MXDLNA_DMS_HANDLE handle,
	DMS_DB_HANDLE h_db,
	MXDLNA_DMS_REMOVE_PROP * pRemoveData,
	char * requiredCdsPath,
	DIRECTORY_BUILDER_FOLDER_TYPE type
);


__MXAPI
MX_INT32 mxDlnaDmsDb_removeCdsContainer(
	MXDLNA_DMS_HANDLE handle,
	DMS_DB_HANDLE h_db,
	MXDLNA_DMS_REMOVE_PROP * pRemoveData,
	char * objectID,
	DIRECTORY_BUILDER_FOLDER_TYPE type
);

__MXAPI
MX_INT32 mxDlnaDmsDb_changeObjectTitle(
	MXDLNA_DMS_HANDLE handle,
	DMS_DB_HANDLE h_db,
	char * objectID,
	char * title
);


// 사용자가 특정 미디어에 리소스를 추가하는 함수
// 20091029
__MXAPI
MX_INT32 mxDlnaDmsDb_addResource(
	MXDLNA_DMS_HANDLE handle,
	DMS_DB_HANDLE h_db,
	MXDLNA_DMS_RESOURCE * addResource,
	char * existedPath_UTF8
);

// 사용자가 특정 미디어들에 리소스를 제거하는 함수
__MXAPI
MX_INT32 mxDlnaDmsDb_removeResource(
	 MXDLNA_DMS_HANDLE handle,
	 DMS_DB_HANDLE h_db,
	 char * existedContentValue_UTF8
);


__MXAPI
int mxDlnaDms_existedURI(
	const char * uri ,
	char ** ppObject
);


// 특정 ObjectID를 부모로 갖는 아이템들에 동일한 Title이 존재하는지 확인하는 API
__MXAPI
int mxDlnaDmsDb_isExistedTitle(
	DMS_DB_HANDLE h_db,
	char * parentID,
	char * titleUTF8,
	char ** ppOutObjectID
);


__MXAPI
int mxDlnaDmsDb_removeThumbnail(
	MXDLNA_DMS_HANDLE handle,
	int mediaID
);



// 20110205
__MXAPI
int mxDlnaDms_setUDNInfo(
	DMS_DB_HANDLE h_db,
	const char * UDN
);
__MXAPI
int mxDlnaDms_getUDNINfo(
	DMS_DB_HANDLE h_db,
	char ** ppOutUDN,
	int * pbIsUDN
);


// 20110215
//#ifdef TRANSCODING_INFO_MANAGE
__MXAPI
int mxDlnaDms_setTranscodingInfo(DMS_DB_HANDLE h_db, const char * folderPath);
__MXAPI
int mxDlnaDms_getTranscodingInfo(DMS_DB_HANDLE h_db, char ** ppTranscodingFolder , int * pIsInit);
//#endif
// ###############################################
// Directory Builder ( end )
// ###############################################




#ifdef HUMAX_EPG_TUNER_FEATURE_20120328
// ############################################
// epg_manager ( start )
// ############################################


// **********************************************
// define
// **********************************************
#define DBS_EPG_ROOT_CONTAINER_TITLE	"EPG"
#ifdef HUMAX_HMS_2012
#define DBS_EPG_ROOT_CONTAINER_ID		"epg"
#else
#define DBS_EPG_ROOT_CONTAINER_ID		"0\\epg"
#endif
#define DBS_EPG_CHANNEL_PREFIX			"ech_"

#define DBS_EPG_EVENT_PREFIX			"eev_"

/*
livetv app이 "tuner"로 invoke browsing했을 때 dms_cds.db의 tblObject 테이블에서 class가 "object.container.channelGroup" 인 objectid으로 바꿔서 처리하도록 한다.
*/
#define DBS_TUNER_SPECIAL_ID			"tuner"

#define DBS_TUNER_ROOT_CONTAINER_TITLE	"TUNER"
#define DBS_TUNER_ROOT_CONTAINER_TITLE_DB	"CHANNEL"
#ifdef HUMAX_HMS_2012
#define DBS_TUNER_ROOT_CONTAINER_ID		"tuner"
#else
#define DBS_TUNER_ROOT_CONTAINER_ID		"0\\tuner"
#endif
#define DBS_TUNER_ROOT_CONTAINER_ID_DB	"0\\channel"
#define DBS_TUNER_CHANNEL_PREFIX		"tch_"

#define DBS_PVR_ROOT_CONTAINER_TITLE	"PVR"
#ifdef HUMAX_HMS_2012
#define DBS_PVR_ROOT_CONTAINER_ID		"0\\1\\pvr"
#else
#define DBS_PVR_ROOT_CONTAINER_ID		"0\\pvr"
#endif
#define DBS_PVR_CONTENT_PREFIX		"pcontent_"

#define DBS_MULTIVALUE_DELIMINATOR		""

#define MXDLNA_DMS_EPG_CHANNELID_LEN		32
#define MXDLNA_DMS_EPG_CHANNELID_TYPE_LEN	16
#define MXDLNA_DMS_EPG_CHANNELID_DISTRINETWORKNAME_LEN	24
#define MXDLNA_DMS_EPG_CHANNELNAME_LEN		128

#define MXDLNA_DMS_EPG_TITLE_LEN			256
#define MXDLNA_DMS_EPG_LONGDESCRIPTION_LEN	256
#define MXDLNA_DMS_EPG_LANGUAGE_LEN			16
#define MXDLNA_DMS_EPG_RATING_LEN			16
#define MXDLNA_DMS_EPG_RATING_TYPE_LEN		32
#define MXDLNA_DMS_EPG_RATING_ADVICE_LEN	32
#define MXDLNA_DMS_EPG_TIME					32
#define MXDLNA_DMS_EPG_SCHEDULEDSTARTTIME_USAGE_LEN	32
#define MXDLNA_DMS_EPG_GENRE_LEN		64
#define MXDLNA_DMS_EPG_ACTOR_LEN		256
#define MXDLNA_DMS_EPG_DIRECTOR_LEN		256


#define MXDLNA_DMS_EPG_LOCALURL_LEN			256
#define MXDLNA_DMS_EPG_RESOURCEPROTOCOL_LEN 256
// **********************************************



typedef enum _MXDLNA_DMS_EPG_CHANNEL_TYPE
{
	MXDLNA_DMS_EPG_CHANNEL_NONE		= 0,
	MXDLNA_DMS_EPG_CHANNEL_TV		= 1,
	MXDLNA_DMS_EPG_CHANNEL_RADIO	= 2
}MXDLNA_DMS_EPG_CHANNEL_TYPE;

// **********************************************
// type
// **********************************************
typedef enum _MXDLNA_DMS_EPG_TARGET
{
	MXDLNA_DMS_EPG_TARGET_TUNER_ONLY = 0,
	MXDLNA_DMS_EPG_TARGET_EPG_ONLY = 1,
	MXDLNA_DMS_EPG_TARGET_BOTH = 2
}MXDLNA_DMS_EPG_TARGET;

typedef enum _MXDLNA_DMS_EPG_DAYLIGHTSAVING
{
	MXDLNA_DMS_EPG_DAYLIGHTSAVING_UNKNOWN = 0,		//"UNKNOWN"
	MXDLNA_DMS_EPG_DAYLIGHTSAVING_STANDARD = 1,		//"STANDARD"
	MXDLNA_DMS_EPG_DAYLIGHTSAVING_DAYLIGHTSAVING = 2	//"DAYLIGHTSAVING"
}MXDLNA_DMS_EPG_DAYLIGHTSAVING;




#ifdef HUMAX_EPG_CALLBACK_FEATURE_20121206


typedef struct _MXDLNA_DMS_EPG_CHANNEL
{
	//================================================
	// Raw EPG data related
	//================================================
	char* logoURL;
	//================================================

	//================================================
	// Channel identification related
	//================================================
	int uniqValue;
	//================================================

	//================================================
	// DLNA Mandatory property related
	//================================================
	char channelID[MXDLNA_DMS_EPG_CHANNELID_LEN];			// source channel의 ID		(7.4.6.5.7.2절 참조)
	char channelID_type[MXDLNA_DMS_EPG_CHANNELID_TYPE_LEN];			// source channel의 type		(7.4.6.5.7.2절 참조)
	int  channelID_distriNetworkID; 	// distribution source of channel의 ID	(7.4.6.5.8.2절 참조)
	char channelID_distriNetworkName[MXDLNA_DMS_EPG_CHANNELID_DISTRINETWORKNAME_LEN]; 	// distribution source of channel의 name	(7.4.6.5.9.2절 참조)
	//================================================

	//================================================
	// UPnP property related
	//================================================
	int   channelNr; // channel 번호			(Annex B.3.3절 참조)
	char channelName[MXDLNA_DMS_EPG_CHANNELNAME_LEN]; // channel 이름 eg.NCN, NCN HD	(Annex B.3.4절 참조)
	//================================================


	//================================================
	// MXDLNA_DMS_EPG_USERSETPROP related
	//================================================
	char channel_title[MXDLNA_DMS_EPG_TITLE_LEN];
	char channel_localUrl[MXDLNA_DMS_EPG_LOCALURL_LEN];
	char resource_protocol[MXDLNA_DMS_EPG_RESOURCEPROTOCOL_LEN];
	int   resource_bitrate;
	int   resource_duration;
	int   resource_resolutionX;
	int   resource_resolutionY;
	//================================================
	MXDLNA_DMS_EPG_CHANNEL_TYPE  nChannel_Type;

#if  defined(DMS_USER_RESOURCE_MODIFY_M3U8_POSITION_EPG) || defined(DMS_USER_RESOURCE_MODIFY_M3U8_POSITION_MEDIA)
	int nExConfig;
#endif

#if	defined(HUMAX_X_CHANNEL_ADDITIONAL_INFO)
	char  * pChannel_additional_info;
#endif

	 struct _MXDLNA_DMS_EPG_CHANNEL *pNextChannel;

	 int	dtcp_protection;		// 0 : none, 1 : dtcp_protected
	 int	dtcp_emi;				// char dtcp_emi;	20120405 changed by jonghyun
	 long	dtcp_pcplength;
}MXDLNA_DMS_EPG_CHANNEL;

typedef struct _MXDLNA_DMS_EPG_EVENT
{
	//================================================
	// Raw EPG data related
	//================================================
	int eventID;
	char* thumbnailURL;
	//================================================

	//================================================
	// DLNA Mandatory property related
	//================================================
	char title[MXDLNA_DMS_EPG_TITLE_LEN]; 			// event title 			(7.4.6.5.3.2절 참조)
	char longDescription[MXDLNA_DMS_EPG_LONGDESCRIPTION_LEN]; 		// event description 			(7.4.6.5.4.2절 참조)

	//unsigned int num_language;		// language 수
	char language[MXDLNA_DMS_EPG_LANGUAGE_LEN]; 			// languages of audio tracks 		(7.4.6.5.5.2절 참조)

	char rating[MXDLNA_DMS_EPG_RATING_LEN]; 			// Table L.1의 <Valid rating> column 	(7.4.6.5.6.2절 참조)
	char rating_type[MXDLNA_DMS_EPG_RATING_TYPE_LEN]; 			// Table L.1의 <Domain> column 	(7.4.6.5.6.2절 참조)
	char rating_advice[MXDLNA_DMS_EPG_RATING_ADVICE_LEN]; 			// Table L.1의 <Valid Advice> column 	(7.4.6.5.6.2절 참조)
	unsigned char rating_equivalenceAge; 	// Table L.1의 <Age Equivalence> column 	(7.4.6.5.6.2절 참조)

	unsigned int scheduledStartTime; 		// event의 시작 시각			(7.4.6.5.10절 참조)
	unsigned int scheduledEndTime; 		// event의 종료 시각			(7.4.6.5.10절 참조)
	//char scheduledStartTime[MXDLNA_DMS_EPG_TIME]; 		// event의 시작 시각			(7.4.6.5.10절 참조)
	//char scheduledEndTime[MXDLNA_DMS_EPG_TIME]; 		// event의 종료 시각			(7.4.6.5.10절 참조)
	char  scheduledStartTime_usage[MXDLNA_DMS_EPG_SCHEDULEDSTARTTIME_USAGE_LEN]; 	// startTime의 의미			(7.4.6.5.10절 참조)
	unsigned int    scheduledDuration; 		// event의 재생 시간			(7.4.6.5.10절 참조)

	MXDLNA_DMS_EPG_DAYLIGHTSAVING scheduledStartTime_dayLightSaving;
					// starttime의 daylightsaving 적용 여부	(7.4.6.4.11절 참조)
	MXDLNA_DMS_EPG_DAYLIGHTSAVING scheduledEndTime_dayLightSaving;
					// endtime의 daylightsaving 적용 여부	(7.4.6.4.11절 참조)
	//================================================

	//================================================
	// UPnP property related
	//================================================
	//unsigned int num_genre;		// 장르 수
	char genre[MXDLNA_DMS_EPG_GENRE_LEN]; 			// 장르 eg. Movie, News
	//unsigned int num_actor;		// 배우 수
	char actor[MXDLNA_DMS_EPG_ACTOR_LEN]; 			// 배우
	//unsigned int num_director;		// 감독(연출) 수

	char director[MXDLNA_DMS_EPG_DIRECTOR_LEN]; 			// 감독(연출)
	int episodeCount; 			// series인 경우, 전체 episode 수
	int episodeNumber; 			// series인 경우, 현 event의 episode number
	//================================================

	 struct _MXDLNA_DMS_EPG_EVENT *pNextEvent;
	 int uniqValue;

	 int dtcp_protection;	// 2015/5/17 mhkang: Channel에 DTCP걸어야하는 여부(0: clear TS, 1: dtcp)
}MXDLNA_DMS_EPG_EVENT;







/**	\brief	MXDLNA_DMS_EPG_GET_CHANNEL_COUNT
	\brief	전체 channel 수를 return (only for Browse action)

	\return	MX_INT32	*/
typedef MX_INT32 (*MXDLNA_DMS_EPG_GET_CHANNEL_COUNT) ( void );

/**	\brief	MXDLNA_DMS_EPG_GET_CHANNEL
	\brief	uniqValue를 가지는 channel의 정보를 return (only for Browse action)

	\param	[IN] uniqValue	: 채널의 unique한 값

	\return	MX_INT32	*/
typedef MXDLNA_DMS_EPG_CHANNEL* (*MXDLNA_DMS_EPG_GET_CHANNEL) ( int uniqValue );


/**	\brief	MXDLNA_DMS_EPG_SEARCH_CHANNEL
	\brief	Channel search 결과를 return (for Browse/Search action)

	\param	[IN] search_criteria	: 검색조건, Null 일경우 모든 채널이 return 대상
	\param	[IN] sort_criteria	: Channel list의 정렬 기준
	\param	[IN] startIdx		: 넘겨 받는 channel list의 시작 위치
	\param	[IN] requestedCount	: 넘겨 받는 max channel 수

	\return	MXDLNA_DMS_EPG_CHANNEL*		*/
typedef MXDLNA_DMS_EPG_CHANNEL*	(*MXDLNA_DMS_EPG_SEARCH_CHANNEL) ( char *search_criteria, char *sort_criteria ,int startIdx, int requestedCount);


/**	\brief	MXDLNA_DMS_EPG_SEARCH_EVENT
	\brief	Event search 결과를 return (for Browse/Search action)

	\param	[IN] search_criteria	: 검색조건, Null 일 경우 없음
	\param	[IN] sort_criteria	: Event list의 정렬 기준
	\param	[IN] startIdx		: 넘겨 받는 event list의 시작 위치
	\param	[IN] requestedCount	: 넘겨 받는 max event 수
	\param	[OUT] pnTotal    	: search 검색된 total event 갯수

	\return	MXDLNA_DMS_EPG_EVENT*		*/
typedef MXDLNA_DMS_EPG_EVENT*	(*MXDLNA_DMS_EPG_SEARCH_EVENT) ( char *search_criteria, char *sort_criteria ,int startIdx, int requestedCount, int *pnTotal );


// **********************************************
// type
// **********************************************

typedef struct _MXDLNA_DMS_EPG_CALLBACK
{
	MXDLNA_DMS_EPG_GET_CHANNEL_COUNT fnGetChannelCount;
	MXDLNA_DMS_EPG_GET_CHANNEL fnGetChannel;
	MXDLNA_DMS_EPG_SEARCH_CHANNEL fnSearchChannel;
	MXDLNA_DMS_EPG_SEARCH_EVENT fnSearchEvent;
} MXDLNA_DMS_EPG_CALLBACK;

// **********************************************
// APIs
// **********************************************
// EPG
__MXAPI void mxDlnaDmsEpg_setCallback(MXDLNA_DMS_EPG_CALLBACK *callback);
__MXAPI MX_INT32 mxDms_EPG_Get_Channel_Count(void);
__MXAPI MXDLNA_DMS_EPG_CHANNEL* mxDms_EPG_Get_Channel ( int uniqValue );
__MXAPI MXDLNA_DMS_EPG_CHANNEL*	mxDms_EPG_Search_Channel( char *search_criteria, char *sort_criteria ,int startIdx, int requestedCount );
__MXAPI MXDLNA_DMS_EPG_EVENT*	mxDms_EPG_Search_Event ( char *search_criteria, char *sort_criteria ,int startIdx, int requestedCount , int *pnTotal);
__MXAPI void mxDms_EPG_Free_Channels(MXDLNA_DMS_EPG_CHANNEL *pCh);
__MXAPI void mxDms_EPG_Free_Events(MXDLNA_DMS_EPG_EVENT *pEv);

#else
typedef struct _MXDLNA_DMS_EPG_CHANNEL
{
	//================================================
	// Raw EPG data related
	//================================================
	char* logoURL;
	//================================================

	//================================================
	// Channel identification related
	//================================================
	int uniqValue;
	//================================================

	//================================================
	// DLNA Mandatory property related
	//================================================
	char channelID[MXDLNA_DMS_EPG_CHANNELID_LEN];			// source channel의 ID		(7.4.6.5.7.2절 참조)
	char channelID_type[MXDLNA_DMS_EPG_CHANNELID_TYPE_LEN];			// source channel의 type		(7.4.6.5.7.2절 참조)
	int  channelID_distriNetworkID; 	// distribution source of channel의 ID	(7.4.6.5.8.2절 참조)
	char channelID_distriNetworkName[MXDLNA_DMS_EPG_CHANNELID_DISTRINETWORKNAME_LEN]; 	// distribution source of channel의 name	(7.4.6.5.9.2절 참조)
	//================================================

	//================================================
	// UPnP property related
	//================================================
	int   channelNr; // channel 번호			(Annex B.3.3절 참조)
	char channelName[MXDLNA_DMS_EPG_CHANNELNAME_LEN]; // channel 이름 eg.NCN, NCN HD	(Annex B.3.4절 참조)
	//================================================

#ifdef HUMAX_JP_PROPERTIES_20120212
	//================================================
	// Arib property related
	//================================================
	char * arib_objectType;	// 일본 스팩의 arib_objectType
	char * arib_copyControlInfo;
	char * arib_longDescription;
	char * arib_videoComponentType;
	char * arib_audioComponentType;
	char * arib_audioComponentType_qualityIndicator;
	char * arib_dataProgramInfo;
	char * arib_dataProgramInfo_sync;
	char * arib_multiViewInfo;
	char * arib_captionInfo;
	char * arib_multiESInfo;
	char * arib_caProgramInfo;
	char * arib_caProgramInfo_price;
	char * arib_caProgramInfo_available;
	//================================================
#endif

	struct _MXDLNA_DMS_EPG_EVENT *epgEvent;

	struct _MXDLNA_DMS_EPG_CHANNEL *pNextChannel;
}MXDLNA_DMS_EPG_CHANNEL;

typedef struct _MXDLNA_DMS_EPG_EVENT
{
	//================================================
	// Raw EPG data related
	//================================================
	int eventID;
	char* thumbnailURL;
	//================================================

	//================================================
	// DLNA Mandatory property related
	//================================================
	char title[MXDLNA_DMS_EPG_TITLE_LEN]; 			// event title 			(7.4.6.5.3.2절 참조)
	char longDescription[MXDLNA_DMS_EPG_LONGDESCRIPTION_LEN]; 		// event description 			(7.4.6.5.4.2절 참조)

	//unsigned int num_language;		// language 수
	//char** language; 			// languages of audio tracks 		(7.4.6.5.5.2절 참조)
	char language[MXDLNA_DMS_EPG_LANGUAGE_LEN]; 			// languages of audio tracks 		(7.4.6.5.5.2절 참조)

	char rating[MXDLNA_DMS_EPG_RATING_LEN]; 			// Table L.1의 <Valid rating> column 	(7.4.6.5.6.2절 참조)
	char rating_type[MXDLNA_DMS_EPG_RATING_TYPE_LEN]; 			// Table L.1의 <Domain> column 	(7.4.6.5.6.2절 참조)
	char rating_advice[MXDLNA_DMS_EPG_RATING_ADVICE_LEN]; 			// Table L.1의 <Valid Advice> column 	(7.4.6.5.6.2절 참조)
	unsigned char rating_equivalenceAge; 	// Table L.1의 <Age Equivalence> column 	(7.4.6.5.6.2절 참조)

	unsigned int scheduledStartTime; 		// event의 시작 시각			(7.4.6.5.10절 참조)
	unsigned int scheduledEndTime; 		// event의 종료 시각			(7.4.6.5.10절 참조)
	char  scheduledStartTime_usage[MXDLNA_DMS_EPG_SCHEDULEDSTARTTIME_USAGE_LEN]; 	// startTime의 의미			(7.4.6.5.10절 참조)
	unsigned int    scheduledDuration; 		// event의 재생 시간			(7.4.6.5.10절 참조)

	MXDLNA_DMS_EPG_DAYLIGHTSAVING scheduledStartTime_dayLightSaving;
					// starttime의 daylightsaving 적용 여부	(7.4.6.4.11절 참조)
	MXDLNA_DMS_EPG_DAYLIGHTSAVING scheduledEndTime_dayLightSaving;
					// endtime의 daylightsaving 적용 여부	(7.4.6.4.11절 참조)
	//================================================

	//================================================
	// UPnP property related
	//================================================
	//unsigned int num_genre;		// 장르 수
	//char** genre; 			// 장르 eg. Movie, News
	char genre[MXDLNA_DMS_EPG_GENRE_LEN]; 			// 장르 eg. Movie, News
	//unsigned int num_actor;		// 배우 수
	//char** actor; 			// 배우
	char actor[MXDLNA_DMS_EPG_ACTOR_LEN]; 			// 배우
	//unsigned int num_director;		// 감독(연출) 수
	//char** director; 			// 감독(연출)
	char director[MXDLNA_DMS_EPG_DIRECTOR_LEN]; 			// 감독(연출)
	int episodeCount; 			// series인 경우, 전체 episode 수
	int episodeNumber; 			// series인 경우, 현 event의 episode number
	//================================================

	struct _MXDLNA_DMS_EPG_EVENT *pNextEvent;
}MXDLNA_DMS_EPG_EVENT;

typedef struct _MXDLNA_DMS_EPG_USERSETPROP
{
	char* channel_title_tuner_dr;
	char* channel_title_tuner_tr1;
	char* channel_title_tuner_tr2;
	char* channel_title_epg;
	char* channel_localUrl_dr;
	char* channel_localUrl_tr1;
	char* channel_localUrl_tr2;
	char* resource_protocol_dr;
	char* resource_protocol_tr1;
	char* resource_protocol_tr2;
	int   resource_bitrate_dr;
	int   resource_bitrate_tr1;
	int   resource_bitrate_tr2;
	int   resource_duration_dr;
	int   resource_duration_tr1;
	int   resource_duration_tr2;
	int   resource_resolutionX_dr;
	int   resource_resolutionX_tr1;
	int   resource_resolutionX_tr2;
	int   resource_resolutionY_dr;
	int   resource_resolutionY_tr1;
	int   resource_resolutionY_tr2;
	int   using_dtcp_dr;
	int   using_dtcp_tr1;
	int   using_dtcp_tr2;
	int   dtcp_emi_dr;
	int   dtcp_emi_tr1;
	int   dtcp_emi_tr2;
	int   channel_rating;
	long  dtcp_pcplength_dr;
	long  dtcp_pcplength_tr1;
	long  dtcp_pcplength_tr2;
}MXDLNA_DMS_EPG_USERSETPROP;
#endif
// **********************************************
typedef struct _MXDLNA_DMS_EPG_USERSETPROP
{
	char* channel_title_tuner_dr;
	char* channel_title_tuner_tr1;
	char* channel_title_tuner_tr2;
	char* channel_title_epg;
	char* channel_localUrl_dr;
	char* channel_localUrl_tr1;
	char* channel_localUrl_tr2;
	char* resource_protocol_dr;
	char* resource_protocol_tr1;
	char* resource_protocol_tr2;
	int   resource_bitrate_dr;
	int   resource_bitrate_tr1;
	int   resource_bitrate_tr2;
	int   resource_duration_dr;
	int   resource_duration_tr1;
	int   resource_duration_tr2;
	int   resource_resolutionX_dr;
	int   resource_resolutionX_tr1;
	int   resource_resolutionX_tr2;
	int   resource_resolutionY_dr;
	int   resource_resolutionY_tr1;
	int   resource_resolutionY_tr2;
	int   using_dtcp_dr;
	int   using_dtcp_tr1;
	int   using_dtcp_tr2;
	int   dtcp_emi_dr;
	int   dtcp_emi_tr1;
	int   dtcp_emi_tr2;
	int   channel_rating;
	long  dtcp_pcplength_dr;
	long  dtcp_pcplength_tr1;
	long  dtcp_pcplength_tr2;
}MXDLNA_DMS_EPG_USERSETPROP;

// **********************************************
// APIs
// **********************************************

//
typedef MXDLNA_DMS_EPG_USERSETPROP* (*MXDLNA_DMS_EPG_ADD_UPDATE_CALLBACK) (
	MXDLNA_DMS_EPG_CHANNEL *pChannel
);

/**	\brief	mxDlnaDmsEpg_createRootContainer
	\brief	EPG/TUNER feature의 callback 방식의 root container를 생성, 이미 생성되어 있다면 아무작업하지 않고 리턴.
			Container의 Title은 "TUNER" 으로 고정되어 있음.
	\param	handle	: MXDLNA_DMS_HANDLE 타입의 DMS 핸들
	\param	target	: Root container 생성 대상을 지정(type 참조, [MXDLNA_DMS_EPG_TARGET_TUNER_ONLY|MXDLNA_DMS_EPG_TARGET_EPG_ONLY|MXDLNA_DMS_EPG_TARGET_BOTH])

	\return	MX_INT32	*/
__MXAPI
MX_INT32 mxDlnaDmsEpg_createRootContainer(
	MXDLNA_DMS_HANDLE handle,
	MXDLNA_DMS_EPG_TARGET target
);


MX_INT32 mxDlnaDmsEpg_createRootContainerEx(
	MXDLNA_DMS_HANDLE handle,
	const char* Title
);

MX_INT32 mxDlnaDmsEpg_createContainer(
	MXDLNA_DMS_HANDLE handle,
	const char* objectid,
	const char* title,
	MX_BOOL	searchable,
	MX_BOOL	restricted
);

/**	\brief	mxDlnaDmsEpg_createRootContainerDB
	\brief	TUNER feature의 DB 방식의 CDS 공개되는 root container를 생성, 이미 생성되어 있다면 아무작업하지 않고 리턴
	\param	handle	: MXDLNA_DMS_HANDLE 타입의 DMS 핸들
	\param	title	: Tuner root container의 title을 지정할 수 있음, NULL로 설정할 경우 default title로 "CHANNEL" 이 설정됨

	\return	MX_INT32	*/

__MXAPI
MX_INT32 mxDlnaDmsEpg_createRootContainerDB(
	MXDLNA_DMS_HANDLE handle,
	const char *title
);


/**	\brief	mxDlnaDmsEpg_addEpgChannel
	\brief	EPG/TUNER feature의 채널 및 이벤트를 추가
	\param	handle		: MXDLNA_DMS_HANDLE 타입의 DMS 핸들
	\param	pChannel	: 추가할 채널 및 이벤트 정보 전달을 위한 구조체 (type 참조)_DMS_EPG_EVENT
	\param	target		: 추가할 대상 (type 참조, [MXDLNA_DMS_EPG_TARGET_TUNER_ONLY|MXDLNA_DMS_EPG_TARGET_EPG_ONLY|MXDLNA_DMS_EPG_TARGET_BOTH])

	\return	MX_INT32
	*/
__MXAPI
MX_INT32 mxDlnaDmsEpg_addEpgChannel(
	MXDLNA_DMS_HANDLE handle,
	MXDLNA_DMS_EPG_CHANNEL * pChannel,
	MXDLNA_DMS_EPG_ADD_UPDATE_CALLBACK addUpdateEvent,
	MXDLNA_DMS_EPG_TARGET target
);

/**	\brief	mxDlnaDmsEpg_setChannelRating
	\brief	uniqValue에 해당하는 channel item의 rating 값을 설정
	\param	handle		: MXDLNA_DMS_HANDLE 타입의 DMS 핸들
	\param	uniqValue	: 채널 지시자
	\param	rating		: rating 정보 (eg. 15/18)

	\return	MX_INT32
	*/
__MXAPI
MX_INT32 mxDlnaDmsEpg_setChannelRating(
	MXDLNA_DMS_HANDLE handle,
	int uniqValue,
	int rating
);

/**	\brief	mxDlnaDmsEpg_removeEpgChannel
	\brief	EPG/TUNER feature의 채널 및 해당 채널의 모든 이벤트를 삭제
	\param	handle		: MXDLNA_DMS_HANDLE 타입의 DMS 핸들
	\param	pChannel	: 삭제할 채널 정보 전달을 위한 구조체 (이벤트 정보는 채워넣을 필요 없다. 삭제할 대상의 기준은 pChannel->uniqValue 가 된다.)
	\param	target		: 삭제할 대상  (type 참조, [MXDLNA_DMS_EPG_TARGET_TUNER_ONLY|MXDLNA_DMS_EPG_TARGET_EPG_ONLY|MXDLNA_DMS_EPG_TARGET_BOTH])

	\return	MX_INT32
*/
__MXAPI
MX_INT32 mxDlnaDmsEpg_removeEpgChannel(
   MXDLNA_DMS_HANDLE handle,
   MXDLNA_DMS_EPG_CHANNEL * pChannel,
   MXDLNA_DMS_EPG_TARGET target
);

/**	\brief	mxDlnaDmsEpg_updateEpgChannel
	\brief	EPG/TUNER feature의 채널정보를 변경, 변경할 대상의 기준은 pChannel->uniqValue
	\param	handle		: MXDLNA_DMS_HANDLE 타입의 DMS 핸들
	\param	pChannel	: 변경할 채널 정보 전달을 위한 구조체 (이벤트 정보는 채워넣을 필요 없다. 변경할 대상의 기준은 pChannel->uniqValue 가 된다.)
	\param	addUpdateEvent	: User 설정 property를 위한 callback function pointer
	\param	target		: 변경할 대상 (type 참조, [MXDLNA_DMS_EPG_TARGET_TUNER_ONLY|MXDLNA_DMS_EPG_TARGET_EPG_ONLY|MXDLNA_DMS_EPG_TARGET_BOTH])

	\return	MX_INT32
*/
__MXAPI
MX_INT32 mxDlnaDmsEpg_updateEpgChannel(
	MXDLNA_DMS_HANDLE handle,
	MXDLNA_DMS_EPG_CHANNEL * pChannel,
	MXDLNA_DMS_EPG_ADD_UPDATE_CALLBACK addUpdateEvent,
	MXDLNA_DMS_EPG_TARGET target
);

/**	\brief	mxDlnaDmsEpg_allClear
	\brief	모든 TUNER/EPG의 채널 및 이벤트 정보를 삭제(TUNER/EPG root container 포함)
	\param	handle		: MXDLNA_DMS_HANDLE 타입의 DMS 핸들

	\return	MX_INT32
*/
__MXAPI
MX_INT32 mxDlnaDmsEpg_allClear(
	MXDLNA_DMS_HANDLE * handle
);


/**	\brief	mxDlnaDmsEpg_addEpgEvent
	\brief	부분 Event 정보를 추가 (이미 channel이 추가되어 있어야 한다.)
	\param	handle		: MXDLNA_DMS_HANDLE 타입의 DMS 핸들
	\param	uniqValue	: 추가될 event의 channel을 지정하는 channel uniqValue
	\param	bUpdate		: 추가할 event가 이미 존재할 경우 skip/update 여부 (0:skip, 1:update)

	\return	MX_INT32
*/
__MXAPI
MX_INT32 mxDlnaDmsEpg_addEpgEvent(
	MXDLNA_DMS_HANDLE handle,
	int uniqValue,
	MXDLNA_DMS_EPG_EVENT *pEvent,
	int bUpdate
);

/**	\brief	mxDlnaDmsEpg_clearEventList
	\brief	특정 channel의 event를 모두 삭제 (channel은 삭제하지 않음)
	\param	handle		: MXDLNA_DMS_HANDLE 타입의 DMS 핸들
	\param	uniqValue	: 삭제할 event들의 channel을 지정하는 channel uniqValue

	\return	MX_INT32
*/
__MXAPI
MX_INT32 mxDlnaDmsEpg_clearEventList(
	MXDLNA_DMS_HANDLE handle,
	int uniqValue
);

/**	\brief	mxDlnaDmsEpg_allClearEventList
	\brief	모든 channel의 event를 모두 삭제 (channel은 삭제하지 않음)
	\param	handle		: MXDLNA_DMS_HANDLE 타입의 DMS 핸들

	\return	MX_INT32
*/
__MXAPI
MX_INT32 mxDlnaDmsEpg_allClearEventList(
	MXDLNA_DMS_HANDLE handle
);

/**	\brief	mxDlnaDmsEpg_SetEMIbyUniqValue
	\brief	TUNER channel의 resource property 중 dtcp_emi value를 변경
	\param	handle		: MXDLNA_DMS_HANDLE 타입의 DMS 핸들
	\param	uniqValue	: 변경할 channel의 uniqValue
	\param	emi			: 설정하고자 하는 dtcp emi 값

	\return	MX_INT32
*/
__MXAPI
MX_INT32 mxDlnaDmsEpg_SetEMIbyUniqValue(
	MXDLNA_DMS_HANDLE handle,
	int uniqValue,
	int emi
);

// **********************************************

// ############################################
// epg_manager ( end )
// ############################################
#endif



// ############################################
// pvr_manager ( end )
// ############################################

#ifdef THIRDPARTY_PVR_FEATURE_20150509

#define MXDLNA_DMS_PVR_CONTENT_TITLE_LEN				256
#define MXDLNA_DMS_PVR_CONTENT_CREATOR_LEN				256
#define MXDLNA_DMS_PVR_CONTENT_URI_LEN					256
#define MXDLNA_DMS_PVR_CONTENT_RESOURCEPROTOCOL_LEN		256
#define MXDLNA_DMS_PVR_ALBUMARTURI_LEN					256
#define MXDLNA_DMS_PVR_CHANNELNAME_LEN					128

typedef enum _MXDLNA_DMS_PVR_CONTENT_TYPE
{
	MXDLNA_DMS_PVR_CONTENT_NONE		= 0,
	MXDLNA_DMS_PVR_CONTENT_TV		= 1,
	MXDLNA_DMS_PVR_CONTENT_RADIO	= 2
}MXDLNA_DMS_PVR_CONTENT_TYPE;

typedef struct _MXDLNA_DMS_PVR_CONTENT
{
	char  		title[MXDLNA_DMS_PVR_CONTENT_TITLE_LEN];			// PVR title
	char 		creator[MXDLNA_DMS_PVR_CONTENT_CREATOR_LEN];		// creator (ex. zodiac)
	char		albumarturi[MXDLNA_DMS_PVR_ALBUMARTURI_LEN];		// <upnp:albumArtUri>

	int 		uniqueValue;	// PVR db id
	int			mediaType;		// TV / RADIO
//	char * 		classType;		// TV : object.item.videoItem.move, RADIO : object.item.audioItem
//	char * 		mimeType;		// TV : video/mpeg, RADIO : audio/mpeg

	// (MXDLNA_DMS_RESOURCE)
	// (MXDLNA_PROTOCOLINFO)
	// video / audio resource protocol info related
	char		resource_contentURI[MXDLNA_DMS_PVR_CONTENT_URI_LEN];		// content URI
	char		resource_protocol[MXDLNA_DMS_PVR_CONTENT_RESOURCEPROTOCOL_LEN];		// protocol info
	int			resource_bitrate;
	int			resource_duration;
	int			resource_resolutionX;
	int			resource_resolutionY;

	// thumbnail resource protocol info related
	char		thumbnail_contentURI[MXDLNA_DMS_PVR_CONTENT_URI_LEN];		// content URI
	char		thumbnail_protocol[MXDLNA_DMS_PVR_CONTENT_RESOURCEPROTOCOL_LEN];		// protocol info

	MX_INT64	size;			// PVR file size
	char			channelName[MXDLNA_DMS_PVR_CHANNELNAME_LEN]; // channel name
	unsigned int	startTime; 		// record start time
	unsigned int	endTime; 		// record end time

	int			dtcp_protection;		// 0 : none, 1 : dtcp_protected
	int			dtcp_emi;				//char dtcp_emi;	20120405 changed by jonghyun
	long		dtcp_pcplength;
	int			nExConfig;			//	for HLS support

	char		*pContent_Additional_Info;

	struct _MXDLNA_DMS_PVR_CONTENT *pNextContent;
}MXDLNA_DMS_PVR_CONTENT;


typedef MX_INT32 (*MXDLNA_DMS_PVR_GET_CONTENT_COUNT) ( void );

typedef MXDLNA_DMS_PVR_CONTENT* (*MXDLNA_DMS_PVR_GET_CONTENT) ( int uniqValue );

typedef MXDLNA_DMS_PVR_CONTENT*	(*MXDLNA_DMS_PVR_SEARCH_CONTENT) ( char *search_criteria, char *sort_criteria ,int startIdx, int requestedCount);


// **********************************************
// type
// **********************************************

typedef struct _MXDLNA_DMS_PVR_CALLBACK
{
	MXDLNA_DMS_PVR_GET_CONTENT_COUNT fnGetContentCount;
	MXDLNA_DMS_PVR_GET_CONTENT fnGetContent;
	MXDLNA_DMS_PVR_SEARCH_CONTENT fnSearchContent;
} MXDLNA_DMS_PVR_CALLBACK;

// **********************************************
// APIs
// **********************************************
// PVR
__MXAPI void mxDlnaDmsPvr_setCallback(MXDLNA_DMS_PVR_CALLBACK *callback);
__MXAPI void mxDms_PVR_Free_Contents(MXDLNA_DMS_PVR_CONTENT *pCh);



/**	\brief	mxDlnaDmsPvr_createRootContainer
	\brief
	\param	handle

	\return	MX_INT32	*/
__MXAPI MX_INT32 mxDlnaDmsPvr_createRootContainer(MXDLNA_DMS_HANDLE handle, const char *title);
__MXAPI MX_INT32 mxDlnaDmsPvr_createContainer(MXDLNA_DMS_HANDLE handle,const char* objectid, const char *title, MX_BOOL searchable,  MX_BOOL restricted);
__MXAPI MX_INT32 mxDlnaDms_createContainer(DMS_DB_HANDLE hDB, const char* objectid, const char *title, const char* upnpclass, MX_BOOL searchable,  MX_BOOL restricted);
__MXAPI MX_INT32 mxDlnaDms_getCountOfContainer(DMS_DB_HANDLE h_db, const char* title, const char* upnpclass);
__MXAPI MX_BOOL  mxDlnaDms_getContainerID(DMS_DB_HANDLE h_db, const char* title, const char* upnpclass, char* outContainerID);

#define CONTAINER_COND_MUSIC_ARTIST	"%music\\artist"
#define CONTAINER_COND_MUSIC_GENRE	"%music\\genre"
#define CONTAINER_COND_MUSIC_ALBUM	"%music\\album"
__MXAPI MX_BOOL mxDlnaDms_getNewContainerID(DMS_DB_HANDLE h_db, const char* container_cond, char* outContainerID);
__MXAPI int		mxDlnaDms_createReferenceItem(DMS_DB_HANDLE hDB, char* localUrl, char* ContainerID);

#endif


// ############################################
// pvr_manager ( end )
// ############################################




// ############################################
// SYSTEM_FOLDER_MGR (Start)
// ############################################

// **********************************************
// Type
// **********************************************

// SystemFolder handle
typedef void* SFM_HANDLE;



// SystemFolder내부에 child로 가질 objectItem들의 타입을 선택한다.
// audio/video/image/text 등 각각의 타입 을 선택하거나 두개 이상의 선택 조합도 된다.
typedef enum _MXDLNA_SFM_MEDIATYPE
{
	MEDIATYPE_NONE = -1,
	MEDIATYPE_ALL = 0,
	MEDIATYPE_UNKNOWN	= 1,
	MEDIATYPE_IMAGE	= 2,
	MEDIATYPE_AUDIO	= 4,
	MEDIATYPE_VIDEO	= 8,
	MEDIATYPE_TEXT = 16
} MXDLNA_SFM_MEDIATYPE;



// SysTemFolder 의 타입으로써 UPnP classType도 이것과 Media Type을 조합하여 판단되어진다.
// SYSTEMTYPE_ARTIST 와 MEDIATYPE_AUDIO 인 경우 object.container.person.musicArtist 처럼
// 추후 계속 업데이트 될 것임. //a 로 닫혀있는 부분도 업데이트 예정
typedef enum _MXDLNA_DMS_SYSTEMFOLDERTYPE
{
	SYSTEMTYPE_CONTAINER = 0,   // must have title

	SYSTEMTYPE_DATE,
	SYSTEMTYPE_DATE_YEAR, 		// music, image, video
//a	SYSTEMTYPE_DATE_MONTH,
//a	SYSTEMTYPE_DATE_DAY,
	SYSTEMTYPE_RATING,
	SYSTEMTYPE_TITLE,			// maybe "a","b","c" ...

	SYSTEMTYPE_ALBUM,			// music
	SYSTEMTYPE_ARTIST,
//a	SYSTEMTYPE_ALBUMARTIST,
	SYSTEMTYPE_GENRE,
	SYSTEMTYPE_COMPOSER,

//a	SYSTEMTYPE_RESOLUTION,		// video, image
	SYSTEMTYPE_ACTOR,			// video,

	SYSTEMTYPE_KEYWORD,			// etc
	SYSTEMTYPE_PLAYLIST,

	SYSTEMTYPE_REALFOLDER,      // realPath etc
								//  c:\music\album\1  -> cdsobject 0\music\album\1

	SYSTEMTYPE_REALFOLDER_PARENT, // realPath etc

	SYSTEMTYPE_MAX
} MXDLNA_DMS_SYSTEMFOLDERTYPE;


typedef struct _MXDLNA_DMS_SYSTEMFOLDER_PROPERTY
{
	MXDLNA_DMS_SYSTEMFOLDERTYPE folderType;
	MXDLNA_SFM_MEDIATYPE mediaType;
	int isRestricted;			// SYSTEMTYPE_REALFOLDER 인경우에만 0으로 설정 할 수 있다.
	int isSearchable;

}MXDLNA_DMS_SYSTEMFOLDER_PROPERTY;
// **********************************************




// **********************************************
// APIs
// **********************************************

// 최상위 Root SystemFolder SFM_HANDLE을 받아온다.
__MXAPI
SFM_HANDLE mxDlnaDmsSfm_getRootSystemFolder(
	MXDLNA_DMS_HANDLE handle ,
	int isRestricted,
	int isSearchable
);


// SystemFolder SFM_HANDLE을 생성한다. root SystemFolder 는 생성할수 없으며 getRootSystemFolder로 얻어낼수 있다.
// Parent가 되는 SFM_HANDLE을 가지고 그 하위의 SFM_HANDLE을 생성할수 있다. 최상위 Parent는 root이다.
// SystemFolder는 두가지 종류가 있는데 title을 가지고 있는, makeSystemFolder로 생성할수 있는 것과,
// makeSystemFolder로 생성된 SystemFolder에 addSubSystemFolder로 추가될수 있는 SystemFolder.
// addSubSystemFolder로 추가되는 것은 title이 따로 있을수 없는 media들의 데이터로 title이 결정되는 SubSystemFolder이다.
// 즉 SYSTEMTYPE_ARTIS타입의 addSubSystemFolder를 추가하였다면, Media의 Artist에 따른 SystemFolder가 동적으로 생성되는 것이다.
//SFM_HANDLE mxDlnaDmsSfm_makeSystemFolder( SFM_HANDLE parent, char* title, MXDLNA_DMS_SYSTEMFOLDERTYPE folderType, MXDLNA_SFM_MEDIATYPE MediaType );
__MXAPI
SFM_HANDLE mxDlnaDmsSfm_makeSystemFolder(
	MXDLNA_DMS_HANDLE handle,
	SFM_HANDLE parent,
	char * utf8_title,
	MXDLNA_DMS_SYSTEMFOLDER_PROPERTY folderProperty
);
__MXAPI
int mxDlnaDmsSfm_addSubSystemFolder(
	SFM_HANDLE parent,
	MXDLNA_DMS_SYSTEMFOLDER_PROPERTY folderProperty
);



// 생성한 모든 SystemFolder 를 제거한다.
__MXAPI
int mxDlnaDmsSfm_removeAllSystemFolder(void);





// addSubSystemFolder 를 생성할때 그 하위의 모든 media들을 가지는 all SystemFolder를 자동으로 구성해 주는가
// 를 지정해 줄수 있다. 즉 mxDlnaDmsSfm_setAllFolder( TRUE, "EveryOne"); 이라면
// addSubSystemFolder를 생성할때 마다 그 하위의 모든 media들이 포함되는 EveryOne이라는 이름의 SystemFolder가 자동으로 생성된다.
// 즉 root - video (SystemFolder) - ARTIST (SubSystemFolder) 의 tree를 만들어 놓았다면
// ARTIST 마이클베이 의 video가 하나 scan되어서 추가되었다면,
//  root - video - EveryOne
//               - 마이클베이
//  의 트리가 자동으로 만들어 진다.
__MXAPI
int mxDlnaDmsSfm_setAllFolder(
	int Enable,
	char* title
);


// Debug 용으로 현재 SystemFolder Tree 가 어떻게 구성되어 있는지 printf 문으로 보여준다.
// 위의 SYSTEMTYPE_ 과 MEDIATYPE_ 도 출력된다.
__MXAPI
int mxDlnaDmsSfm_printAllSystemFolder(void);




// FolderProperty 값을 깔끔하게 채워주는 API
__MXAPI
int mxDlnaDmsSfm_setFolderProperty(
	MXDLNA_DMS_SYSTEMFOLDER_PROPERTY * pProperty ,
	MXDLNA_DMS_SYSTEMFOLDERTYPE folderType ,
	MXDLNA_SFM_MEDIATYPE mediaType ,
	int isRestricted,
	int isSearchable
);



// playlist Item을 Folder & File 구조로 CDS에 생성해주는 API
__MXAPI
MX_INT32 mxDlnaDmsSfm_addPlaylistItem(
	MXDLNA_DMS_HANDLE handle,
	MXDBC_HANDLE h_db,
	MXDLNA_DMS_ADDITEM_PROP * pData,
	SFM_HANDLE requiredLocation
);


// CDS에 Folder & File 구조로 생성된 아이템을 제거하는 API
__MXAPI
MX_INT32 mxDlnaDmsSfm_removePlaylistItem(
	MXDLNA_DMS_HANDLE handle,
	DMS_DB_HANDLE h_db,
	MXDLNA_DMS_REMOVE_PROP * pData,
	const char * title_UTF8,
	SFM_HANDLE requiredLocation
);


// 아이템 생성가능한 컨테이너의 createClass 값을 설정하는 함수. 기본값은 Lib이 정의해 놓았다.
__MXAPI
int mxDlnaDmsSfm_setCreateClassesString(
	char * createClasses
);

// Unknown에 해당하는 기본 값을 설정 한다
__MXAPI
void mxDlnaDmsSfm_setUnknownValues(
	const char * unknown_utf8_max1024,
	const char * unknown_year_utf8_max1024,
	const char * unknown_date_utf8_max1024,
	const char * unknown_month_utf8_max1024,
	const char * unknown_day_utf8_max1024
);


// **********************************************
// ############################################
// SYSTEM_FOLDER_MGR (End)
// ############################################









// ############################################
// Device Auth (start)
// ############################################
struct _MXDLNA_DMS_DEVICEINFO;

typedef enum _MXDLNA_DMS_AUTH_DEVICETYPE
{
	MXDLNA_DMS_AUTH_UNKNOWN = 0,			// 상태를 알기 어려운 정보 or 초기 값
	MXDLNA_DMS_AUTH_MEDIACONTROLLOR,		// DMC or DMP
	MXDLNA_DMS_AUTH_MEDIARENDERER,			// DMR
	MXDLNA_DMS_AUTH_MEDIASERVER				// DMS
}MXDLNA_DMS_AUTH_DEVICETYPE;

typedef enum _MXDLNA_DMS_CONNECTIONTYPE
{
	MXDLNA_DMS_CONNECTION_DISCONNECTED = 0,
	MXDLNA_DMS_CONNECTION_CONNECTED = 1
}MXDLNA_DMS_CONNECTIONTYPE;

typedef enum _MXDLNA_DMS_ACCESSCONTROLTYPE
{
	MXDLNA_DMS_ACCESSCONTROL_DENY = 0,
	MXDLNA_DMS_ACCESSCONTROL_BASE_ALLOW = 1,	// isProtection 아이템은 제어하지 못함
	MXDLNA_DMS_ACCESSCONTROL_ALLOW = 2,
}MXDLNA_DMS_ACCESSCONTROLTYPE;


typedef struct _MXDLNA_DMS_DEVICEINFO
{
	MXDLNA_DMS_AUTH_DEVICETYPE	deviceType;
	char * manufacturer;
	char * friendlyname;
	char * headerInfo;
	char * UDN;
	MXDLNA_DMS_CONNECTIONTYPE	connection;
	unsigned long time_sec;
	unsigned short port;

	struct _MXDLNA_DMS_DEVICEINFO * pNext;
}MXDLNA_DMS_DEVICEINFO;

typedef struct _MXDLNA_DMS_DEVICEINFO_ETH
{
	char * macaddr;
	unsigned long ipaddr;
	int count;
	unsigned long eth_time_sec;
	MXDLNA_DMS_ACCESSCONTROLTYPE	eth_access;
	MXDLNA_DMS_CONNECTIONTYPE		eth_connection;
	MXDLNA_DMS_DEVICEINFO * pInfo;

	struct _MXDLNA_DMS_DEVICEINFO_ETH * pNext;
}MXDLNA_DMS_DEVICEINFO_ETH;



__MXAPI
int mxDlnaDms_deviceAuthSetAccessControlByIpAddr(
	MXDLNA_DMS_HANDLE handle,
	unsigned int ipaddr,
	MXDLNA_DMS_ACCESSCONTROLTYPE accessControl
);



// ethernet
__MXAPI
int mxDlnaDms_deviceAuthGetEthCount(
	MXDLNA_DMS_HANDLE handle
);

__MXAPI
int mxDlnaDms_deviceAuthGetEthItem(
	MXDLNA_DMS_HANDLE handle,
	int index,						// index >= 0 && index < total
	const MXDLNA_DMS_DEVICEINFO_ETH ** ppOutDeviceEthItem
);


// item
__MXAPI
int mxDlnaDms_deviceAuthGetCount(
	MXDLNA_DMS_HANDLE handle,
	MXDLNA_DMS_DEVICEINFO_ETH * pDeviceEth
);
__MXAPI
int mxDlnaDms_deviceAuthGetItem(
	MXDLNA_DMS_HANDLE handle,
	MXDLNA_DMS_DEVICEINFO_ETH * pDeviceEth,
	int index,						// index >= 0 && index < total
	const MXDLNA_DMS_DEVICEINFO ** ppOutDeviceItem
);


// Remove
__MXAPI
int mxDlnaDms_deviceAuthDeleteItem(
	MXDLNA_DMS_HANDLE handle,
	int eth_index ,
	int device_index,
	int isAllDelete
);


// ############################################
// Device Auth (End)
// ############################################



// ############################################
// ParentalLock Access List (Start)
// ############################################
#define DMS_MAX_PARENTALLOCK_ACCESS_LIST 20

typedef struct _MXDLNA_DMS_PARENTALLOCK
{
	int bParentalLock;			// browse/search action의 soap header에 parental lock 구문 추가 여부 [0|1]
	char ipAddr[32];			// 접속했던 CP의 ip주소
	char macAddr[13];			// 접속했던 CP의 mac주소
}MXDLNA_DMS_PARENTALLOCK;

typedef struct _MXDLNA_DMS_PARENTALLOCK_LIST
{
	unsigned int listCount;		// 현재까지 접속했던 CP의 숫자 (최대 DMS_MAX_PARENTALLOCK_ACCESS_LIST)
	MXDLNA_DMS_PARENTALLOCK parentalLockList[DMS_MAX_PARENTALLOCK_ACCESS_LIST];		// 접속했던 CP의 parentallock 정보, listCount가 1 이상일 경우
																					// parentalLockList[0] 부터 parentalLockList[listCount-1] 까지
																					// 최근 추가된 CP 순으로 저장
}MXDLNA_DMS_PARENTALLOCK_LIST;

// Remove
__MXAPI
MXDLNA_DMS_PARENTALLOCK_LIST mxDlnaDms_getParentalLockAccessList(
	MXDLNA_DMS_HANDLE handle
);
// ############################################
// ParentalLock Access List (Start)
// ############################################




// ############################################
// VALIDCONNECTION (start)
// ############################################
// BETA API - 정식 API가 아님
typedef struct _MXDLNA_DMS_VALIDCONNECTION_INFO
{
	int initTag;
	unsigned long ipaddr;	// ipv4 ...
	unsigned short port;	// port도 쓰려면 쓰면 됨
	unsigned char mac[6];	// mac...
}MXDLNA_DMS_VALIDCONNECTION_INFO;

__MXAPI
void mxDlnaDms_initValidConnection(
	void
);
__MXAPI
MX_INT32 mxDlnaDms_insertValidConnection(
	MXDLNA_DMS_VALIDCONNECTION_INFO * pValidInfo
);

__MXAPI
MX_INT32 mxDlnaDms_removeValidConnection(
	MXDLNA_DMS_VALIDCONNECTION_INFO * pValidInfo
);


__MXAPI
MX_INT32 mxDlnaDms_isValidConnection(
	int socket,
	unsigned long ipaddr
);

// ############################################
// VALIDCONNECTION (end)
// ############################################




// ################################################
// TEMP API ( 잠시 동안 있는 임시 API 계속 필요할 경우 정식으로 바꾸자 )
// ################################################

__MXAPI int mxDlnaDms_makeMediaIDbyImportURI(const char * importURI );

//#if defined _WIN32_WCE || defined _ANDROID
__MXAPI char *mxDlnaDms_getURI2Path(char *uri);
//__MXAPI char *mxDlnaDms_getPath2URI(char *path);
__MXAPI char *mxDlnaDms_getPath2URI(MXDLNA_DMS_HANDLE handle , char *uri);

//dypark 2012-11-15
__MXAPI MXDLNA_CDSOBJECT *mxDlnaDms_getCdsObjectByPath(MXDLNA_DMS_HANDLE handle, char *path);
__MXAPI MXDLNA_CDSOBJECT *mxDlnaDms_getCdsObjectByUniqValue(MXDLNA_DMS_HANDLE handle, int uniqValue);
__MXAPI MXDLNA_CDSOBJECT *mxDlnaDms_getCdsObjectByContentID(MXDLNA_DMS_HANDLE handle,char* mount_path, MX_INT64 cid);

/**	\brief	mxDlnaDms_compareMediaByDate
	\brief	입력된 date string과 DB상의 파일 경로에 해당하는 media의 date와 비교 (같을경우 0, 다를경우 1 return, 오류일 경우 -1)
	\param	h_db : db 핸들
	\param	localUrl : 파일 경로
	\param	date : 비교할 date string

	\return	 [-1|0|1]*/

__MXAPI int mxDlnaDms_compareMediaByDate(
	MXDBC_HANDLE h_db,
	char * localUrl,
	char * date
);


//#endif
// ################################################

// ################################################
//MRCP 연동을 위한 모듈
// ################################################
// DB로 부터 CDS Object를 가져오는 모듈
// DB로 부토 CDS Object Item을 가져오는 모듈
// MRCP 연동
__MXAPI int mxDlnaDms_openServerObjectList(MXDLNA_DMS_HANDLE dms, char *id, char *filter, int order, int idx, int *cnt,  int *total, MXDLNA_CDSOBJECT **resultList);
__MXAPI int mxDlnaDms_closeServerObjectList(MXDLNA_DMS_HANDLE dms, MXDLNA_CDSOBJECT *resultList);
__MXAPI char *mxDlnaDms_makeValue2URI(MXDLNA_DMS_HANDLE hDms, char *value);
__MXAPI char *mxDlnaDms_makeCurrentURIMetaData(MXDLNA_CDSOBJECT *obj, int *error);



enum MXDLNADMS_CAI_ERROR
{
	MXDLNADMS_CAI_ERROR = -1,
	MXDLNADMS_CAI_ERROR_INSERT_MEDIANRESOURCE = -2,
	MXDLNADMS_CAI_ERROR_INSERT_OBJECT = -3,

	MXDLNADMS_CAI_ERROR_REMOVE_MEDIANRESOURCE = -6,
	MXDLNADMS_CAI_ERROR_REMOVE_OBJECT = -7,
	MXDLNADMS_CAI_ERROR_DONT_HAVE_OBJECT = -10,
	MXDLNADMS_CAI_ERROR_WRONG_OBJECTTYPE = -20,
};
#ifdef DMS_CONTENT_AGGREGATION_INTERFACE_SUPPORT
// ######################################################
// ContentAggregation Interface (CAI)
// ######################################################



// item 추가
/**	\brief	mxDlnaDms_CaiAddItem()
	\brief	다른 서버의 CDS Content 중 object.item 를 수집하여, DB에 넣는 작업을 수행하기 위한 함수이다.
	\brief	이와 반대되는 함수는, mxDlnaDms_CaiRemoveItem() 이다.
	\param	handle	: DMS의 핸들
	\param	h_db	: database의 핸들
	\param	pParamInpData : CDSObject 값이다.
		- 주요 값
			parentID: 현재 DB상에 ParentID에 해당하는 값으로 변경하여 설정한다.
			ID: mxDlnaDms_CaiAddItem() 함수가 성공적으로 완료되었을때, 만들어진 object의 ID값이 설정된다.
	\param	pInpOriginalObjectID 	: Source DMS의 ObjectID에 해당 하는값을 입력한다.
									(구성에 따라서, prefix로 어떤 DMS의 값인지 유추할 수 있게 넣어준다. ex) dmsudn_0$1
	\return	성공시, MXDLNA_OK
			실패시, enum MXDLNADMS_CAI_ERROR 사용
*/
__MXAPI
int mxDlnaDms_CaiAddItem(
	MXDLNA_DMS_HANDLE handle, DMS_DB_HANDLE h_db ,
	MXDLNA_CDSOBJECT * pParamInpData,
	char *pInpOriginalObjectID
);


// item 삭제
/**	\brief	mxDlnaDms_CaiRemoveItem()
	\brief	DB에 다른 서버의 CDS Content 를 수집한 object.item제거 하기 위한 함수이다.
	\brief	이와 반대되는 함수는, mxDlnaDms_CaiAddItem() 이다.
	\param	handle	: DMS의 핸들
	\param	h_db	: database의 핸들
	\param	objectID : 삭제하려고 하는 Object의 ID값을 입력한다.
	\return	성공시, MXDLNA_OK
			실패시, enum MXDLNADMS_CAI_ERROR 사용
*/
__MXAPI
int mxDlnaDms_CaiRemoveItem(
	MXDLNA_DMS_HANDLE handle ,
	DMS_DB_HANDLE h_db ,
	char * objectID
);


// Container 추가
/**	\brief	mxDlnaDms_CaiAddContainer()
	\brief	다른 서버의 CDS Content 중 object.container 를 수집하여, DB에 넣는 작업을 수행하기 위한 함수이다.
	\brief	이와 반대되는 함수는, mxDlnaDms_CaiRemoveContainer() 이다.
	\param	handle	: DMS의 핸들
	\param	h_db	: database의 핸들
	\param	pParamInpData : CDSObject 값이다.
		- 주요 값
			parentID: 현재 DB상에 ParentID에 해당하는 값으로 변경하여 설정한다.
			ID: mxDlnaDms_CaiAddContainer() 함수가 성공적으로 완료되었을때, 만들어진 object의 ID값이 설정된다.
	\param	originalObjectID 	: Source DMS의 ObjectID에 해당 하는값을 입력한다.
									(구성에 따라서, prefix로 어떤 DMS의 값인지 유추할 수 있게 넣어준다. ex) dmsudn_0$1
	\param	bTitleCheck	: database의 핸들
	\return	성공시, MXDLNA_OK
			실패시, enum MXDLNADMS_CAI_ERROR 사용
*/
__MXAPI
int mxDlnaDms_CaiAddContainer(
	MXDLNA_DMS_HANDLE handle,
	DMS_DB_HANDLE h_db ,
	MXDLNA_CDSOBJECT * pParamInpData,
	char * originalObjectID,
	int bTitleCheck					 // 같은 타이틀에 대해 검사할지 유무
);




// Container 삭제
/**	\brief	mxDlnaDms_CaiRemoveContainer()
	\brief	DB에 다른 서버의 CDS Content 를 수집한 object.container제거 하기 위한 함수이다.
	\brief	이와 반대되는 함수는, mxDlnaDms_CaiAddContainer() 이다.
	\brief	하위의, 아이템 및 Container도 함께 제거한다.
	\param	handle	: DMS의 핸들
	\param	h_db	: database의 핸들
	\param	objectID : 삭제하려고 하는 Object의 ID값을 입력한다.
	\return	성공시, MXDLNA_OK
			실패시, enum MXDLNADMS_CAI_ERROR 사용
*/
__MXAPI
int mxDlnaDms_CaiRemoveContainer(
	MXDLNA_DMS_HANDLE handle,
	DMS_DB_HANDLE h_db ,
	char * objectID
);


//=================================================================================
// dypark :

// URL를 가져온다.
__MXAPI
int mxDlnaDms_CaiGetLocalUrl(
	char *contentURI,
	char ** ppLocalUrl
);

// Update 중 삭제된 Object를 확인하는 작업을 시작한다.
__MXAPI
int mxDlnaDms_CaiStartDelStatus(
	MXDLNA_DMS_HANDLE handle,
	DMS_DB_HANDLE h_db,
	char *objectID
);

// Update 중 삭제된 Object를 확인하는 작업을 종료한다. 종료시 필요없는 Object는 삭제한다.
__MXAPI
int mxDlnaDms_CaiEndDelStatus(
	MXDLNA_DMS_HANDLE handle,
	DMS_DB_HANDLE h_db,
	char *objectID
);

enum {
	MXDMS_CAI_EXIST_OK,
	MXDMS_CAI_EXIST_NO,
	MXDMS_CAI_EXIST_CHANGED,
};

__MXAPI
int mxDlnaDms_CaiExistContainer(
	MXDLNA_DMS_HANDLE handle,
	DMS_DB_HANDLE h_db,
	char *originalObjectID,
	char *title,
	char **retObjectID
);

__MXAPI
int mxDlnaDms_CaiExistItem(
	MXDLNA_DMS_HANDLE handle,
	DMS_DB_HANDLE h_db,
	char *originalObjectID,
	char *title,
	MX_INT64 size,
	char **retObjectID
);

__MXAPI
char *mxDlnaDms_CaiGetObjectID(
	MXDLNA_DMS_HANDLE handle,
	DMS_DB_HANDLE h_db,
	char *originalObjectID
);

__MXAPI
int mxDlnaDms_CaiAddRootContainer(
	MXDLNA_DMS_HANDLE handle,
	char *title,
	char *objectID
);


__MXAPI
int mxDlnaDms_CaiSetMainServerName(
	MXDLNA_DMS_HANDLE handle,
	char *title
);

__MXAPI
char *mxDlnaDms_CaiGetMainServerName(
	MXDLNA_DMS_HANDLE handle
);


// 관리 되는 DMS에서 가져오는 CDSObject가 추가/삭제 될 때, 호출 되는 Callback의 정의이다.
typedef void (*MXDLNA_DMS_CAI_ADD_REMOVE_FN) (MXDLNA_DMS_HANDLE handle, DMS_DB_HANDLE h_db, char *deviceID, char *objectID, char *path, int error, int bContainer, int added);			// MXRAS_COLLECT_EVENT_CALLBACK
typedef char *(*MXDLNA_DMS_CAI_GET_HTTP_ADDRESS_FN) (void *handle, char *UDN, char *deviceID);

__MXAPI
int mxDlnaDms_CaiSetCallback(
	MXDLNA_DMS_HANDLE handle,
	MXDLNA_DMS_CAI_ADD_REMOVE_FN add_remove_fn,				// 등록하는 함수가 추가됨.
	MXDLNA_DMS_CAI_GET_HTTP_ADDRESS_FN get_http_address_fn
);


__MXAPI
int mxDlnaDms_CaiDestroyObject(
	MXDLNA_DMS_HANDLE handle,
	char *objectID
);

__MXAPI
int mxDlnaDms_CaiUpdateObject(
	MXDLNA_DMS_HANDLE dms,
	char *ObjectID,
	char *CurrentTagValue,
	char *NewTagValue
);


__MXAPI
char *mxDlnaDms_CaiMakeThumbnailFileName(
	char *objectID
);


#ifdef RAS_TRANSCODER_THUMBNAIL_SUPPORT
typedef struct _MXDMS_THUMBNAIL_DATA
{
	int type;
	char *deviceID;
	char *objectID;
	char *path;
	char *name;
	int status;
} MXDMS_THUMBNAIL_DATA;


__MXAPI
int mxDlnaDms_CaiInsertWaitingThumbnail(DMS_DB_HANDLE h_db, int type, char *deviceID, char *objectID, char *path, char *name);

__MXAPI
int mxDlnaDms_CaiDeleteWaitingThumbnail(DMS_DB_HANDLE h_db, char *deviceID, char *objectID);

__MXAPI
char *mxDlnaDms_CaiGetWaitingThumbnailPath(DMS_DB_HANDLE h_db, char *objectID);

__MXAPI
MXDMS_THUMBNAIL_DATA *mxDlnaDms_CaiGetWaitingThumbnail(DMS_DB_HANDLE h_db, int idx, int cnt, int *result);

__MXAPI
int mxDlnaDms_CaiRemoveThumbnail(MXDLNA_DMS_HANDLE handle, DMS_DB_HANDLE h_db, char *objectID);

#endif

//=================================================================================


// ######################################################
#endif

#ifdef DMS_DLNA_FUNC_SWITCH
/**	\brief mxDlnaDms_setDlnaFunc
	\brief set DMS DLNA functionality on or off */
__MXAPI
void mxDlnaDms_setDlnaFunc(int nOn );
#endif

__MXAPI
int mxDlnaDms_getCurrnetStreamingCnt(void);

#ifdef __cplusplus
}
#endif

#endif // _MXDLNA_DMS_H__











