
/*******************************************************************************
 * HUMAX R&D Lab., HUMAX INC., SEOUL, KOREA
 * COPYRIGHT(c) 2010 by HUMAX Inc.
 * 
 * All rights reserved. No part of this work may be reproduced, stored in a
 * retrieval system, or transmitted by any means without prior written
 * permission of HUMAX Inc.
 ******************************************************************************/

 



#ifndef __MXDLNA_STRUCT_H__
#define __MXDLNA_STRUCT_H__

// ---------------------------------------
#include "mxlib_config.h"
// ---------------------------------------

#ifdef __cplusplus
extern "C" 
{
#endif

#ifdef _WIN32_WCE 
#include <winbase.h>
#endif

#include "mxtypes.h"
#include "mxdlna_define.h"

typedef void*				MXDLNA_HANDLE;						// 상위 API에서 사용하는 HANDLE
typedef void*				MXDLNA_CP_HANDLE;					// 조작하고 있는 ControlPoint의 핸들
typedef void*				MXDLNA_ACTION_HANDLE;				// Action을 수행하는데 필요한 데이터를 가지고 있는 핸들
typedef char				MXDLNA_UDN_DATA;
typedef int					MXDLNA_KEY_CODE;
typedef MXDLNA_UDN_DATA*	MXDLNA_UDN;
typedef MXDLNA_UDN_DATA*	MXDLNA_UDN_HANDLE;
typedef int					MXDLNA_CMID;						
typedef void*				MXDLNA_TAG;							// 내부적으로 사용되는 태그
typedef void*				MXDLNA_CALLBACK;					// 콜백을 저장함.
typedef void*				MXDLNA_USER_TAG;					// 유저만 사용하는 태그

/*
// 20110501 TEST ==================
typedef struct _mxjni_int_test
{
	int id;
	int	id2;
} MXJNI_INT_TEST;
*/


// 20101104 whiterub
typedef struct _MXDLNA_PERFORMANCE_CONFIG
{
	int * pDmsUpload_delay_ms;			// upload 시 delay 설정 : 느린 환경의 경우 이 값이 0이면 CPU 점유율이 100프로가 될 수 있음
										// 너무 지연시간을 많이 줄경우 업로드 속도가 느려짐
	int * pbDmsBrowse_Reentrant_block;	// Browse or Search() 함수의 재진입을 Block 시킬지 유무 : 
										// 느린 환경의 경우 비동기적으로 Response를 확인하지 않고 호출 할 경우 CPU 점유율이 높아짐
	int * pbDescriptionDataSend_delay_ms;	// Browse/Search/Description 등등을 보낼때 오버해드 감소를 위한 Sleep
	int * pbDmsUploadProtocolInfoCheck;	// upload 시 ProtocolInfo 정보를 확인하고 거부 할지 말지를 결정하는 Flag 
}MXDLNA_PERFORMANCE_CONFIG;


typedef struct _MXDLNA_DEVICE
{
	const int ReservedID;
	const char* DeviceType;        // URN
	const char* UDN;
	const char* LocationURL;
	const char* PresentationURL;
	const char* FriendlyName;
	const char* ManufacturerName;
	const char* ManufacturerURL;
	const char* ModelName;
	const char* ModelDescription;
	const char* ModelNumber;
	const char* ModelURL;

	//>> 2010/08/09 upload flag
	int X_Dlnacap;
	//<<

	// 2012-09-03 mklim modified - jcngs 추가 속성 구현 작업 - X_DLNACAP string 추가
	char* pX_Dlnacap;

	// 2010-12-17
	int nDeviceType;

	// 2012-09-03 mklim modified - jcngs 추가 속성 구현 작업 - sort/search capabilities public으로 올리기
	char	*pSortCapabilities;
	char	*pSearchCapabilities;

#ifdef HDRL_CP_SUPPORT
	const char* X_HDLNKDOC;
#endif

	struct mxdlna_device *Parent;
	struct mxdlna_device *EmbeddedDevices;
	struct mxdlna_device *Next;
} MXDLNA_DEVICE;



typedef struct _MXDLNA_DEVICE_INFO
{
	int ReservedID;
	char* DeviceType;        // URN
	char* UDN;
	char* LocationURL;
	char* PresentationURL;
	char* FriendlyName;
	char* ManufacturerName;
	char* ManufacturerURL;
	char* ModelName;
	char* ModelDescription;
	char* ModelNumber;
	char* ModelURL;

	//>> 2010/08/09 upload flag
	int X_Dlnacap;
	//<<

	// 2012-09-03 mklim modified - jcngs 추가 속성 구현 작업 - X_DLNACAP string 추가
	char * pX_Dlnacap;

	// 2010-12-17
	int nDeviceType;

	// 2012-09-03 mklim modified - jcngs 추가 속성 구현 작업 - sort/search capabilities public으로 올리기
	char	*pSortCapabilities;
	char	*pSearchCapabilities;

} MXDLNA_DEVICE_INFO;

typedef struct _MXDLNA_SERVICE
{
	int MaxVersion;
	char* ServiceType;
	char* ServiceId;
	char* ControlURL;
	char* SubscriptionURL;
	char* SCPDURL;
	char* SubscriptionID;	
} MXDLNA_SERVICE;

typedef struct _MXDLNA_ACTION
{
	char* Name;
} MXDLNA_ACTION;

typedef struct _MXDLNA_STATE_VARIABLE
{
	char* Name;
	char **AllowedValues;
	int NumAllowedValues;
	char* Min;
	char* Max;
	char* Step;

} MXDLNA_STATE_VARIABLE;

// 아래 구조체 와 동일 const 때문에 하나더만듬.
typedef struct _MXDLNA_LOAD_CDS_DATA
{
	char *Value;               // URI
	char *ProtocolInfo;
	char *ImportUri;
	char *IfoFileUri;
	char *ImportIfoFileUri;

	/* Negative value means value is unset or unknown. */
	int ResolutionX;
	int ResolutionY;
	int Duration;
	int Bitrate;
	int ColorDepth;

	MX_INT64 Size;

	int BitsPerSample;
	int SampleFrequency;
	int NrAudioChannels;

	/*App-defined DRM protection string. NULL if not present. */
	char *Protection;

	int TrackTotal;
	int ResumeUpload;
	MX_INT64 UploadedSize;

	/* user-defined Field, NULL if not present. */	
	char * UserProperty;	

	struct mxdlna_cdsresource *Next;
} MXDLNA_LOAD_CDS_DATA;


#ifndef _ANDROID
typedef struct mxdlna_cdsresource
{
	const char *Value;               // URI
	const char *ProtocolInfo;
	const char *ImportUri;
	const char *IfoFileUri;
	const char *ImportIfoFileUri;

	/* Negative value means value is unset or unknown. */
	const int ResolutionX;
	const int ResolutionY;
	const int Duration;
	const int Bitrate;
	const int ColorDepth;

	// 20091008
	const MX_INT64 Size;

	const int BitsPerSample;
	const int SampleFrequency;
	const int NrAudioChannels;

	/*App-defined DRM protection string. NULL if not present. */
	const char *Protection;

	const int TrackTotal;
	const int ResumeUpload;
	const MX_INT64 UploadedSize;

	/* user-defined Field, NULL if not present. */	
	const char * UserProperty;	

	struct mxdlna_cdsresource *Next;
	
} MXDLNA_CDSRESOURCE;


typedef struct mxdlna_cdsobject
{
	const char* ID;
	const char* ParentID;
	const char* Title;
	const int	TitleLength;
	const char* Creator;
	const unsigned int Flags;

#ifdef HUMAX_EPG_TUNER_FEATURE_20120328
	const char* Rating;
	const char* Rating_Type;
	const char* Rating_Advice;
	const unsigned char Rating_EquivalentAge;
#endif

#ifdef XSRS_CP_SUPPORT
	char* reservationID;
#endif

#if  defined(DMS_USER_RESOURCE_MODIFY_M3U8_POSITION_EPG) || defined(DMS_USER_RESOURCE_MODIFY_M3U8_POSITION_MEDIA)
	int nExConfig;
#endif
	struct mxdlna_cdsresource *res;

	struct mxdlna_cdsobject *next;
	
} MXDLNA_CDSOBJECT;
#else

typedef struct mxdlna_cdsresource
{
	char *Value;               // URI
	char *ProtocolInfo;
	char *ImportUri;
	char *IfoFileUri;
	char *ImportIfoFileUri;

	/* Negative value means value is unset or unknown. */
	int ResolutionX;
	int ResolutionY;
	int Duration;
	int Bitrate;
	int ColorDepth;

	MX_INT64 Size;

	int BitsPerSample;
	int SampleFrequency;
	int NrAudioChannels;

	/*App-defined DRM protection string. NULL if not present. */
	char *Protection;

	int TrackTotal;
	int ResumeUpload;
	MX_INT64 UploadedSize;

	/* user-defined Field, NULL if not present. */	
	char * UserProperty;	

	struct mxdlna_cdsresource *Next;
	
} MXDLNA_CDSRESOURCE;


typedef struct mxdlna_cdsobject
{
	char* ID;
	char* ParentID;
	char* Title;
	int	TitleLength;
	char* Creator;
	unsigned int Flags;

#ifdef HUMAX_EPG_TUNER_FEATURE_20120328
	const char* Rating;
	const char* Rating_Type;
	const char* Rating_Advice;
	const unsigned char Rating_EquivalentAge;
#endif
#if  defined(DMS_USER_RESOURCE_MODIFY_M3U8_POSITION_EPG) || defined(DMS_USER_RESOURCE_MODIFY_M3U8_POSITION_MEDIA)
	int nExConfig;
#endif

	struct mxdlna_cdsresource *res;

	struct mxdlna_cdsobject *next;
	
} MXDLNA_CDSOBJECT;
#endif



typedef enum
{
	MXDLNA_ProtocolType_UNKNOWN	= 0,
	MXDLNA_ProtocolType_ANY		= 3,
	MXDLNA_ProtocolType_HTTP	= 1,
	MXDLNA_ProtocolType_RTP		= 2
} MXDLNA_PROTOCOLTYPE;


typedef struct mxdlna_protocolinfo
{
	int SupportsTimeBasedSeek;
	int SupportsByteBasedSeek;
	
	char *Profile;
	char *MimeType;

	char **SupportedPlaySpeeds;
	int SupportedPlaySpeedsLength;

	int IsConvertedContent;
	MXDLNA_PROTOCOLTYPE Protocol;
	int IsPlaySingleUri;

	int SenderPaced;
	int LimitedOperations_TimeBasedSeek;
	int LimitedOperations_ByteBasedSeek;

	int DLNAPlayContainer;
	int S0_Increasing;
	int SN_Increasing;

	int RTSP_Pause;
	int HTTP_Stalling;
	
	int TM_S;
	int TM_I;
	int TM_B;

	int MaxSpeed_Major;
	int MaxSpeed_Minor;

	int LinkProtection;
	int ClearText;

	int DLNA_Major_Version;
	int DLNA_Minor_Version;

	void* reserved;
	void* reserved2;

} MXDLNA_PROTOCOLINFO;


typedef struct _MXDLNA_DEVICE_PROTOCOL_INFO {
	char **protocolInfo;
	int protocolInfoLength;
} MXDLNA_DEVICE_PROTOCOL_INFO;


#ifdef _WIN32_WCE

#ifndef  USER_TYPE_STRING 
	#define USER_TYPE_STRING wchar_t
#endif

#endif

#define UDN_MAX_LENGTH  256
typedef struct _MXDLNA_DEVICE_TINY {

#ifndef USER_TYPE_STRING
	char	szName[UDN_MAX_LENGTH + 1];
#else
	USER_TYPE_STRING	szName[UDN_MAX_LENGTH + 1];
#endif

	char	szUDN[UDN_MAX_LENGTH + 1];

#ifdef RUI_SUPPORT
	char szUIListUrl[UDN_MAX_LENGTH + 1];		// 120717 add by jhlim
#endif

#ifdef DMC_DEVICE_TINY_INFO_EX_SUPPORT
	/* start humax jyhwang added */
	char	szLocationURL[UDN_MAX_LENGTH*2];
	char 	szManufacturerName[UDN_MAX_LENGTH];
	/* end humax jyhwang added */
#endif

	MXDLNA_DEVICE_TYPE		nDeviceType;

	struct _MXDLNA_DEVICE_TINY	*pNext;
} MXDLNA_DEVICE_TINY; 

typedef MXDLNA_DEVICE_TINY MXDLNA_USER_DEVICE_LIST;
typedef MXDLNA_DEVICE_TINY MXDLNA_DEVICE_CARDINAL;

// =============================================
// CP관련 구조체
// =============================================

// --------------------------------
#ifdef DLNA_USER_DEVICE_CAPABILITY
// --------------------------------
// 20120328
/*
DeviceDescription에 있는 속성 중에.. LIB에 정의되지 않은 특정한 속성을 파싱해서 가져오기 위한 자료 구조
ex) 
	xmlns:hmx="urn:schemas-humax-co-kr:metadata-1-0"
	<hmx:X_HMXCAP>TSR,MULTIVALUE</hmx:X_HMXCAP>
*/
typedef struct _MXDLNA_USER_DEVICECAP
{
	char * Namespace;			// urn:schemas-humax-co-kr:metadata-1-0
	int szNamespace;			// 36
	char * NamespacePrefix;		// hmx
	int szNamespacePrefix;		// 3
	char * AttributeName;		// X_HMXCAP
	int szAttributeName;		// 8

	int nDeviceType;			// cap에 의한 Device 속성 변경. 

	// 2012-09-03 mklim modified - jcngs 추가 속성 구현 작업 - DLNA_USER_DEVICE_CAPABILITY 확장
	struct _MXDLNA_USER_DEVICECAP * next;
}MXDLNA_USER_DEVICECAP;
// --------------------------------
#endif // #ifdef DLNA_USER_DEVICE_CAPABILITY
// --------------------------------

// 2013/10/23 mhkang: UPNP Device Type Name & Length
typedef struct {
    char*   UserDeviceTypeName;
    int     UserDeviceTypeLength;
} DLNA_USER_DEVICE_TYPE_INFO;



typedef struct _MXDLNACP_INVOKE_STATE 
{
	int index;					

	MXDLNA_ACTION_HANDLE handle;
	MXDLNA_CALLBACK CB;
	MXDLNA_TAG tag;			

} MXDLNACP_INVOKE_STATE;


typedef struct _MXDLNA_CP_CONFIG 
{
	void *DeviceAddOrRemove;
	void *DeviceResponse;
	void *DeviceEvent;

	void *MsearchResponse;

	void *BrowserResponseCallBack;
	void *user_make;
	void *user_free;

#ifdef XSRS_CP_SUPPORT
	void *XSrsBrowseResponseCallback;
#endif

	unsigned int	user_data_size;

//	MXDLNA_TITLE_LIST_TYPE	user_parentfolder_title_type;
//	char user_parentfolder_title[10];

#ifdef DLNA_USER_DEVICE_CAPABILITY
	MXDLNA_USER_DEVICECAP *pUserDeviceCap;
#endif

    char* UserDeviceTypeName;    // 2013/10/22 mhkang: 일반적인 UPNP Device를 지원하기 위해 추가 ex) "urn:schemas-upnp-org:device:HMXRCUServer:1"

} MXDLNA_CP_CONFIG;

typedef struct _MXDLNA_ACTION_DATA {
	MXDLNA_CP_HANDLE	handle;
	MXDLNA_UDN_DATA		udn[UDN_MAX_LENGTH + 1];
	MXDLNA_CMID			cmid;
	MXDLNA_USER_TAG		user;
	MXDLNA_DEVICE_TYPE	deviceType;
} MXDLNA_ACTION_DATA, MXDLNA_INVOKE;


typedef void	(*MXDLNA_DEVICE_ADDREMOVE)		(MXDLNA_CP_HANDLE hCp, MXDLNA_DEVICE *device, int added);
typedef void	(*MXDLNA_DEVICE_RESPONSE_SINK)	(MXDLNA_CP_HANDLE hCp, MXDLNA_ACTION_HANDLE hAction, int code, int error, void *data);
typedef void	(*MXDLNA_DEVICE_EVENT_SINK)		(MXDLNA_CP_HANDLE hCp, MXDLNA_UDN udn, MXDLNA_CMID cmid, int service, int flag, void *data);
typedef void	(*MXDLNA_DEVICE_MSEARCH_RESPONSE) (char * UDN);


// =============================================
// Unicode
// =============================================
typedef enum _MXDLNA_CODE_PAGE
{
	MXDLNA_CP_ACP = 0,
		MXDLNA_CP_KOREA = 949,		// 한국어	ks_c_5601-1987
		MXDLNA_CP_JAPAN = 932,		// 일본어(Shift-JIS)	iso-2022-jp
		MXDLNA_CP_CHINA = 936,		// 중국어 간체(GB2312)	gb2312	936
		MXDLNA_CP_ANSILATIN = 1252,	// 서유럽어(Windows)	Windows-1252
		
		//MXDLNA_CP_ISO8859_1 = 28591,// ISO8859-1
		MXDLNA_CP_UTF8 = 65001,		// UTF8 유니코드. 그냥 계산하면 된다.
		MXDLNA_CP_WIDECHAR = 90001	// 기본적인 WideChar , 윈도우 2Byte , 리눅스  4Byte?
}MXDLNA_CODE_PAGE;



typedef char *(*MXDLNA_DMS_SYS_TO_LIB) (	
	const char* systemStr,	// mbcs?
	int codePage, /*MXDLNA_CODE_PAGE*/
	int sysStrLength,
	int *dstStrLength
);

typedef char *(*MXDLNA_DMS_APP_TO_LIB)(
    const char* appStr,		// utf8
	int codePage,			// MXDLNA_CODE_PAGE
	int	appStrLength,		// DMC는 전체 길이중 일부만..-_-;; 변환하길 바래요
	int *dstStrLength		// 돌려주는 Str의 길이를 받는다. 이렇게 사용한 이유는 위에 함수랑 동일함 함수를 사용한다.-_-; 대부분. 그래서 필요없어도 받아야 한다.
);

typedef char *(*MXDLNA_DMS_LIB_TO_SYS) ( 
	const char* libStr,		// utf8
	int codePage,			// MXDLNA_CODE_PAGE
	int libStrLength,		// DMC는 전체 길이중 일부만..-_-;; 변환하길 바래요
	int *sysStrLength		// 돌려주는 Str의 길이를 받는다. DMC에서 캐쉬에 보관하다가 복사할때 필요합니다. 필요없을때는 NULL를 사용. 사용자 작성시 NULL 체크필요
);

typedef char *(*MXDLNA_DMS_LIB_TO_APP)(
    const char* libStr,		// utf8
	int codePage,			// MXDLNA_CODE_PAGE
	int libStrLength,		// DMC는 전체 길이중 일부만..-_-;; 변환하길 바래요
	int *appStrLength		// 돌려주는 Str의 길이를 받는다. 이렇게 사용한 이유는 위에 함수랑 동일함 함수를 사용한다.-_-; 대부분. 그래서 필요없어도 받아야 한다.
);

typedef struct _MXDLNA_UNICODE_DATA
{
	MXDLNA_CODE_PAGE systemCodePage;
	MXDLNA_CODE_PAGE appCodePage;
	
	MXDLNA_DMS_LIB_TO_SYS fLib2Sys; 
	MXDLNA_DMS_SYS_TO_LIB fSys2Lib;	
	MXDLNA_DMS_LIB_TO_APP fLib2App;
	MXDLNA_DMS_APP_TO_LIB fApp2Lib;

}MXDLNA_UNICODE_DATA;
// =============================================


enum DH_RequestTypes
{
	DH_RequestType_GET = 0,
	DH_RequestType_HEAD,
	DH_RequestType_POST
};


enum DH_TransferModes
{
	DH_TransferMode_Unspecified = 0x00,
	DH_TransferMode_Bulk = 0x01,
	DH_TransferMode_Interactive = 0x02,
	DH_TransferMode_Streaming = 0x04
};



#ifdef HDRL_CP_SUPPORT
#include "mxdlna/mxdlna_struct_xsrs.h"
		 
#endif

// dypark 20130227 
// ILibDisableIPAddress 와 동일함. 수정시 같이 수정되어야 합니다. 
typedef struct _MXDLNA_DISABLE_IP {
	unsigned long ip_start;
	unsigned long ip_end;
} MXDLNA_DISABLE_IP;
// <<


#ifdef __cplusplus
}
#endif

#endif

