#ifndef MX_INC_RESOURCE_COMMON_H__
#define MX_INC_RESOURCE_COMMON_H__


#include "mxapi_define.h"


#ifdef __cplusplus
extern "C"{
#endif


#include "mxres_define.h"
#include "mxtypes.h"





// Video 의 열거형 명령
typedef enum _MXRES_VIDEO_CMD
{
	ExVideo_BPlayable = 1,	// 재생 가능여부
	ExVideo_Direct,	// 원본 데이터 여부
	ExVideo_ByteSeek,	// BYTE SEEK 가능여부
	ExVideo_TimeSeek,	// TIME SEEK 가능여부
	ExVideo_Speed,		// Speed 값 가중치
	ExVideo_PN,		// ORG_PN 값 가중치
	ExVideo_Label,		// 기타 값 가중치
	ExVideo_RES_Count,	// 먼저 나오는 리소스 일수록 값이 크다.
	ExVideo_Max = ExVideo_RES_Count,		
}MXRES_VIDEO_CMD;



// Audio 의 열거형 명령
typedef enum _MXRES_AUDIO_CMD
{
	ExAudio_BPlayable = 1,	// 재생 가능 여부
	ExAudio_Direct,	// 원본 데이터 여부
	ExAudio_ByteSeek,	// BYTE SEEK 가능 여부
	ExAudio_TimeSeek,	// TIME SEEK 가능 여부
	ExAudio_Speed,		// Speed 값 가중치
	ExAudio_PN,		// ORG_PN 값 가중치
	ExAudio_Channel,	// channel 값 가중치
	ExAudio_RES_Count,	// 먼저 나오는 리소스 일수록 값이 크다.
	ExAudio_Max = ExAudio_RES_Count,
}MXRES_AUDIO_CMD;



// Image의 열거형 명령
typedef enum _MXRES_IMAGE_CMD
{
	ExImage_BPlayable = 1,	// 재생 가능 여부
	ExImage_Direct,	// 원본 데이터 여부
	ExImage_Resolution,	// 해상도 가중치
	ExImage_PN,		// ORG_PN 값 가중치
	ExImage_RES_Count,	// 먼저 나오는 리소스 일수록 값이 크다.
	ExImage_Max = ExImage_RES_Count,
}MXRES_IMAGE_CMD;






//Info 명령
typedef enum _MXRES_INFO_CMD
{
	ExInfo_ProfileID = 0,
	ExInfo_Direct,
	ExInfo_ByteSeek,
	ExInfo_TimeSeek,
	ExInfo_Channel,
}MXRES_INFO_CMD;








// preTable을 구성할때 사용되는 명령이 차지하는 바이트수를 표시
typedef struct _MXRES_PRE_TABLE
{
	MX_INT32 ExXXXCMD;		// 명령
	MX_INT32 maxBit;		// 차지하는 비트수
}MXRES_PRE_TABLE;


// Resource Selection의 우선 순위..
typedef struct _MXPN_PRI_MAP
{
    MX_INT8 *name;    
    MX_INT32 grade_bit;
}MXPN_PRI_MAP;





// 우선순위 테이블 가져오는 함수.
// custom에 정의 되어 있다
// 20101006-whiterub callback 타입으로 변경
__MXAPI 
MXRES_PRE_TABLE * mxRes_getPreCmdTable(
	MX_INT32 media_type 
);

__MXAPI 
MXPN_PRI_MAP * mxRes_getProfilePriTable(
	MX_INT32 media_type 
);




#ifdef __cplusplus
}
#endif





#endif

