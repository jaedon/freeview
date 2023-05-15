#ifndef MX_INC_RES_DEFINE_H__
#define MX_INC_RES_DEFINE_H__



#ifdef __cplusplus
extern "C" 
{
#endif



// PN 값의 최대 값
#define MX_MAX_PN_VALUE	(255)



// 공통적으로 사용할 OPERATION 값.. No Operation
#define ExCommon_NOP (0)


#define CMD_USE		(1)
#define CMD_NOT_USE	(0)



#if defined WIN32 && !defined strcasecmp
	#define strncasecmp(x,y,z) _strnicmp(x,y,z)
	#define strcasecmp(x,y) _stricmp(x,y)
#endif




// 에러 정의 값
#define MXRES_OK							(1)
#define MXRES_ERR							(-1)
#define MXRES_ERR_CMD_TABLE					(-2)	// command Table이 이상하다
#define MXRES_ERR_PREV_TABLE				(-3)	// prev command table이 이상하다
#define MXRES_ERR_MEDIA_FORMAT				(-4)	// media format 값이 이상하다.

#define MXRES_ERR_PREV_TABLE_SAME_CMD		(-5)	// prev table에 같은 명령이 존재
#define MXRES_ERR_PREV_TABLE_UNKNOWN_CMD	(-6)	// prev table에 모르는 명령 존재
#define MXRES_ERR_PREV_TABLE_BIT_OVF		(-7)	// prev table에 비트가 넘침.

#define MXRES_ERR_INPVALUE_OVF				(-8)	// 입력값이 할당된 비트보다 큰값이 들어왔음
#define MXRES_ERR_OUTPUT_PTR_NULL			(-9)	// output 포인터값이 널이다.

#define MXRES_ERR_CMD_PARAMETER				(-10)	// commandParameter 오류

#define MXRES_ERR_AUDIO_INIT				(-11)	// Audio 초기화 작업에 실패 하였다
#define MXRES_ERR_VIDEO_INIT				(-12)	// Video 초기화 작업에 실패 하였다
#define MXRES_ERR_IMAGE_INIT				(-13)	// Image 초기화 작업에 실패 하였다


// 정보값
#define MXRES_INFO_CMD_NOT_USE		(2)			// 사용하지 않는 명령 값이다





// ExInfo_Channel 명령에 들어 갈 수 있는 값
#define CHANNELS_UNKNOWN		0x1     // 알 수 없는 채널 값일 때 
#define CHANNELS_MONO		0x2
#define CHANNELS_2CH		0x3
#define CHANNELS_MULTI		0x4
#define CHANNELS_2CH_MULTI		0x5

// ExInfo_Channel , ExInfoProfileID를 제외한 명령에 들어 갈 수 있는 값
#define INFO_FALSE			0x0      // 해당 값이 없을 때
#define INFO_UNKNOWN		0x1      // 해당 값을 알 수 없을 때
#define INFO_TRUE			0x2      // 해당 값이 존재 할 때

// ExInfo_ProfileID에 들어가는 값
#define PROFILE_UNKNOWN		0xffff   // 알 수 없는 profile 값일 때








#ifdef __cplusplus
}
#endif



#endif

