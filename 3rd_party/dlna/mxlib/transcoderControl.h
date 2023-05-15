
/*******************************************************************************
* HUMAX R&D Lab., HUMAX INC., SEOUL, KOREA
* COPYRIGHT(c) 2011 by HUMAX Inc.
*
* Author : Kang, Yonghee.
*
* All rights reserved. No part of this work may be reproduced, stored in a
* retrieval system, or transmitted by any means without prior written
* permission of HUMAX Inc.
******************************************************************************/
#ifndef TRANSCODER_CONTROL_H
#define TRANSCODER_CONTROL_H

#if defined(__cplusplus)
extern "C"{
#endif

#include "transcoderControlConfig.h"
#if defined(TRANSCODER_AS_DLL)
#ifdef TRANSCODER_EXPORTS
#define TRANSCODER_API __declspec(dllexport)
#else
#define TRANSCODER_API __declspec(dllimport)
#endif
#include "mxlog.h"
#else
#define TRANSCODER_API
#endif

#include "Transcoder_data_struct.h"
#include "TransExternApi.h"

#if defined _DEBUG || defined _ANDROID_LOG
#define TRANSCODER_FUNCTION_PRINT()		mxlog( MX_LEVEL_ERROR, "[TRANSCODER: %s %05d]\n",__FUNCTION__ ,__LINE__ )
#else
#define TRANSCODER_FUNCTION_PRINT()		//
#endif

#define  TRC_OPTION_HLS_LOCK_TRANS_INST   0x01

	/*******************************************************************************
	*	transcoder_Log_apis : for Log management
	******************************************************************************/

#if defined(KT_HES)
	TRANSCODER_API void transcoder_Log_init(MX_INT32 level , MXLOG_FILE_PROP * fileProp, LOG_CALLBACK fn );
	TRANSCODER_API void transcoder_Log_setLevel( MX_UINT32 level );
#endif

	/**
	**********************************************************************************************************************
	TransExApi 정의된 Callback 함수를 설정하고 Callback 함수 fp_ExInit가 설정되어 있으면 fp_ExInit 콜을 합니다.
	그리고 fp_ExRegisterStateCallback 함수를 호출하여 TransCtrl_HLS_State 함수포인터를 등록합니다.
	TransCtrl_HLS_State 함수는 Homma에서 특정 상태일때 호출됩니다(ex:Streaming 강제 종료).
	부가적으로 필요한 semaphore 들을 생성합니다.
	  @param[IN] pExApi TransExApi 구조체안의 콜백함수들이 정의되어 있다.
	  @param[OUT] pulTimeOutTime_ms
	  @return 성공하면 MX_RETURN_OK를 반환한다.
	**********************************************************************************************************************
	*/
	TRANSCODER_API int TransCtrl_Init(void * pExApi, unsigned long *pulTimeOutTime_ms);

	/**
	**********************************************************************************************************************
	사용중인 TRANSCODER_INSTANCE가 있다면 종료시키고 생성했던 semaphore들을 삭제한 후 Transcoder Control 모듈을 de-init합니다.
	  @return 성공하면 MX_RETURN_OK 반환합니다.
	**********************************************************************************************************************
	*/
	TRANSCODER_API int TransCtrl_Deinit(void);

	/**
	**********************************************************************************************************************
	  @param[IN] szIPAdrr
	  @param[IN] szDeviceid
	  @param[IN] szStreamid
	  @param[OUT] szKey
	  @param[IN] session_cnt
	  @return 성공하면 MX_RETURN_OK 반환합니다.
	**********************************************************************************************************************
	*/
	TRANSCODER_API int TransCtrl_Make_Key(const char* szIPAdrr, const char* szDeviceid, const char *szStreamid, char *szKey, int session_cnt);

	/*******************************************************************************
	*	transcoder_converter_apis : for offline converting Audio/Video.
	******************************************************************************/

	TRANSCODER_API int TransCtrl_Cnvrt_Init(char * ConveterDirectory,
		TRANSCODER_FINISH_NOTIFY_CALLBACK notify_converter_done,
		TRANSCODER_FINISH_EMPTY_CALLBACK empty_finished);
	//:HQ, LQ bitrate, width, height,: predefined by iProfile.
	TRANSCODER_API int TransCtrl_Cnvrt_Deinit(void);// : kill thread, empty queue,

	TRANSCODER_API int TransCtrl_Cnvrt_AddQueue(const char* device_id,
		const char* object_id,
		const char* FullPathFileName,
		const char* FullPathSubtitleName,
		void*		 user_data,
		const char* OutputName,
		const char* ConvertingPath,
		TRANSCODER_TYPE iProfile);
	// convert Audio/Video with predefined Quality. can resize Image\A1\A6but need it?
	TRANSCODER_API int TransCtrl_Cnvrt_RemoveQueue(const char* device_id, const char* object_id);
	TRANSCODER_API char* TransCtrl_Cnvrt_GetOutFileName(char* InFullPathFileName, char* OutputName, char* ConvertingPath, TRANSCODER_TYPE iProfile);
	TRANSCODER_API int TransCtrl_Cnvrt_GetQueueItemCount(void);
	/*******************************************************************************
	*	transcoder_thumbnail_apis : for thumbnail generation for Audio/Video/Picture
	******************************************************************************/
	TRANSCODER_API int TransCtrl_Thmbnl_Init(char* ThumbnailDirectory,
		int width, int height,
		TRANSCODER_FINISH_NOTIFY_CALLBACK notify_thumbnail_done,
		TRANSCODER_FINISH_EMPTY_CALLBACK empty_finished);

	TRANSCODER_API int TransCtrl_Thmbnl_Deinit(void);// kill thread, empty queue


	TRANSCODER_API int TransCtrl_Thmbnl_AddQueue(const char* device_id,
		const char* object_id,
		const char* FullPathFileName,
		void*		 user_data,
		const char* OutputName,
		const char* ThumbnailPath,
		int iProfile,
		TRANSCODER_METADATA *pMetaData
		);
	TRANSCODER_API int TransCtrl_Thmbnl_Get_instant(char* FullPathFileName, char* ThumbnailPath);
	TRANSCODER_API int TransCtrl_Thmbnl_RemoveQueue(const char* device_id, const char* object_id);
	TRANSCODER_API char* TransCtrl_Thmbnl_GetOutFileName(char* InFullPathFileName, char* OutputName, char* ThumbnailPath); //iProfile not surpported at this time.
	TRANSCODER_API int TransCtrl_Thmbnl_GetQueueItemCount(void);

	/*******************************************************************************
	*	transcoder_apis : getting metadata, terminate etc.
	******************************************************************************/
	/*
	transcoder_getFFmpegErrMsg
	*/
	TRANSCODER_API void TransCtrl_SetSubtitle(BOOL bSubtitleOn);
	TRANSCODER_API void TransCtrl_SetWorkingdir(char* WorkingDirectory);
	TRANSCODER_API void TransCtrl_SetAppdir(char* AppDirectory);
	TRANSCODER_API int  TransCtrl_GetMetadata(const char* FileName, PTRANSCODER_METADATA pMetadata);
	TRANSCODER_API int  TransCtrl_Benchmark(const char* filename, TRANSCODER_BENCHMARK_CALLBACK BenchmarkCallback);
	TRANSCODER_API int  TransCtrl_GetThumbnail(char* FileName, char* ThumbnailName, int width, int height);
	TRANSCODER_API int  TransCtrl_ResizeImage(char* srcImgName,  char* resizedImgName, int width, int height, BOOL keep_aspect_ratio);
	TRANSCODER_API int  TransCtrl_ExtractAlbumArt(char* srcImgName,  char* resizedImgName, int width, int height, BOOL keep_aspect_ratio);
	TRANSCODER_API int TransCtrl_GetVersion(char* pszOutVersion, int nBuflen);

	/********************************************************************************************************************
	*	HLS API
	*********************************************************************************************************************/

	/**
	**********************************************************************************************************************
	  @param[IN] device_type
	  @param[IN] flag
	  @param[IN] szIPAdrr
	  @param[IN] szDeviceid
	  @param[IN] szStreamid
	  @param[IN] szDevice_name
	  @param[IN] szUser_Agent
	  @param[IN] szFilename
	  @param[IN] szSubtitle_filename
	  @param[IN] callback
	  @param[IN] user_data
	  @return
	**********************************************************************************************************************
	*/
	TRANSCODER_API int TransCtrl_HLS_Start(TRANSCODER_REQUSET_DEVICE_TYPE device_type,
		TRANSCODER_REQUEST_FLAG_TYPE flag,
		const char* szIPAdrr,
		const char* szDeviceid,
		const char* szStreamid,
		const char *szDevice_name,
		const char *szUser_Agent,
		const char* szFilename,
		const char* szSubtitle_filename,
		int socketfd[2],
		TRASCODER_CALLBACK_KILL callback,
		void *user_data);

	/**
	**********************************************************************************************************************
	  @param[IN] device_type
	  @param[IN] flag
	  @param[IN] szIPAdrr
	  @param[IN] szDeviceid
	  @param[IN] szDevice_name
	  @param[IN] szUser_Agent
	  @param[IN] szFilename
	  @param[IN] szSubtitle_filename
	  @param[IN] ppszM3U8Name
	  @param[IN] pRoot_m3u8_contents
	  @return
	**********************************************************************************************************************
	*/
	TRANSCODER_API int TransCtrl_HLS_Get_Root_M3U8(TRANSCODER_REQUSET_DEVICE_TYPE device_type,
		TRANSCODER_REQUEST_FLAG_TYPE flag,
		const char* szIPAdrr,
		const char* szDeviceid,
		const char* szDevice_name,
		const char *szUser_Agent,
		const char* szFilename,
		const char* szSubtitle_filename,
		char **ppszM3U8Name,
		PTRANSCODER_BUFFER pRoot_m3u8_contents);

	/**
	**********************************************************************************************************************
	szRequestURL의 TS chunk를 가져옵니다.
	  @param[IN] szRuestURL
	  @param[OUT] pContents
	  @return
	**********************************************************************************************************************
	*/
	TRANSCODER_API int TransCtrl_HLS_Get_Data(const char * szRuestURL, PTRANSCODER_BUFFER pContents);

	/**
	**********************************************************************************************************************
	  HLS를 종료시킵니다.
	  @param[IN] szKey
	  @return
	**********************************************************************************************************************
	*/
	TRANSCODER_API int TransCtrl_HLS_Stop(const char* szKey);

	/**
	**********************************************************************************************************************
	  @param[IN] szKey
	  @return
	**********************************************************************************************************************
	*/
	TRANSCODER_API int TransCtrl_HLS_GetError(const char* szKey, char ** ppszError);

#ifdef SATIP_DEVICE_SUPPORT
	/*********************************************************************************************************************
	*   SAT>IP API
	**********************************************************************************************************************/

	/**
	**********************************************************************************************************************
	  @brief
	  @param[IN] IP
	  @param[IN] sessionID
	  @param[IN] streamID
	  @param[IN] URL
	  @param[IN] callback
	  @param[IN] user_Data
	  @return
	**********************************************************************************************************************
	*/
	TRANSCODER_API Streaming_RetCode_e TransCtrl_SATIP_Create(const char* IP, const char* sessionID, const char*streamID, const char* URL, int socketfd[2], TRASCODER_CALLBACK_KILL callback, void *user_data);

	/**
	**********************************************************************************************************************

	  @param[IN] szKey
	  @param[IN] URL
	  @return
	**********************************************************************************************************************
	*/
	TRANSCODER_API int TransCtrl_SATIP_Start(const char* szKey, const char* URL);

	/**
	**********************************************************************************************************************
	  @brief
	  @param[IN] szKey
	  @return
	**********************************************************************************************************************
	*/
	TRANSCODER_API int TransCtrl_SATIP_Stop(const char* szKey);

	/**
	**********************************************************************************************************************
	  RTP Streaming thread에서 RTP패킷을 전송할 때마다 호출되는 함수.  __satip_server_callback_rtp_read_ts(...)함수 안에서 호출됨.
	  @param[IN] szKey
	  @param[IN] URL
	  @param[IN] pContents
	  @param[OUT] bClearBuffer bClearBuffer가 1이면 RTP streaming thread의 TS Buffer를 clear해준다.
	**********************************************************************************************************************
	*/
	TRANSCODER_API int TransCtrl_SATIP_Get_Data(const char* szKey, const char* szURL, PTRANSCODER_BUFFER pContents, char* bClearBuffer);

	/**
	**********************************************************************************************************************
	  TP 관련 정보를 얻어오기 위한 함수입니다.
	  ex) "tuner=1,0,0,0,11836,h,dvbs,qpsk,off,0.35,27500,56;pids=0,100,101,102,103,106"
	  @param[IN] szKey
	  @param[IN] szURL
	  @param[OUT] info
	  @param[OUT] info_size
	  @return
	**********************************************************************************************************************
	*/
	TRANSCODER_API int TransCtrl_SATIP_GetInfo(const char* szKey, const char* szURL, char **info, int *info_size);
#endif

	/*********************************************************************************************************************
	*	NTLS API
	**********************************************************************************************************************/

	/**
	**********************************************************************************************************************
	  @param cbKill streaming을 강제 종료해야할 때 호출되는 callback함수. 예를 들면 리모콘에서 streaming 종료버튼 눌렀을때 또는 기타 에러로 streaming을 종료해야할 때. timeout처리.
	  @param user_data cbKill 호출될 때 paramter로 값으로 넘기는 user data.
	  @param fd uds socket을 사용하는 경우, fd[0] is used by sender, fd[1] is used by receiver.
	  @param[out] out_key transcoder instance의 key값.
	  @retval 0  성공. 그 이외 값은 실패.
	***********************************************************************************************************************
	*/
	TRANSCODER_API int TransCtrl_Ntls_Start(TRANSCODER_REQUEST_FLAG_TYPE flag, const char* IP, const char*deviceName, const char* userAgent, const char* URL, TRASCODER_CALLBACK_KILL cbKill, void *user_data, int fd[2],  char** out_key);

	/**
	**********************************************************************************************************************
	Homma에게 TS packet을 얻어올 때 호출하는 함수입니다.
	@param[IN] szKey TransCtrl_Ntls_Start 호출시 out_key 값. NTLS streaming session 마다 고유한 string.
	@param[IN] RequestURL Client가 요청한 HTTP URL
	@param[OUT] pContents TRANSCODER_BUFFER구조체에 TS 패킷과 size 정보를 담아 봔환됩니다.
	@return 실패하면 0이 아닌값을 반환한다.
	**********************************************************************************************************************
	*/
	TRANSCODER_API int TransCtrl_Ntls_GetTS(const char* szKey, char* RequestURL,  PTRANSCODER_BUFFER pContents);

	/**
	**********************************************************************************************************************
	NTLS 종료시키는 함수
	@param[IN] szKey
	@return 실패하면 0이 아닌값을 반환한다.
	**********************************************************************************************************************
	*/
	TRANSCODER_API int TransCtrl_Ntls_Stop(const char* szKey);

#ifdef __cplusplus
}
#endif

#endif//#ifndef TRANSCODER_H

