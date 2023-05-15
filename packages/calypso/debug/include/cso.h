/** 
 * @defgroup CSO Wrapper
 * @brief 
 */ 
 
/** 
 * @author Young-Ki,Kim(ykkim3@humaxdigital.com)
 * @brief 
 */ 
 
/** 
 * @note
 * Copyright (C) 2013 Humax Corporation. All Rights Reserved. 
 * This software is the confidential and proprietary information 
 * of Humax Corporation. You may not use or distribute this software 
 * except in compliance with the terms and conditions of any applicable license 
 * agreement in writing between Humax Corporation and you. 
 */ 
 
/*@{*/
 
/** 
 * @file cso.h
 */ 
 
#ifndef __CSO_H__
#define __CSO_H__

#ifdef  __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <stdint.h> /* temp : will be replaced to cso_def.h*/

#include "cso_def.h"
#include "cso_version.h"

/**	
 *	CSO ERROR CODE
 */	

#define USE_IPC

typedef struct SOCK_MEDIAEVENT_t
{
	uint32_t ev;
	int64_t param;
} SOCK_MEDIAEVENT_t;

typedef struct SOCK_ERROREVENT_t
{
	uint32_t ev;
	int64_t param;
} SOCK_ERROREVENT_t;

typedef struct SOCK_CALLBACK_t
{
	int							used;
	int 						type;
	uint32_t 				sessionid;
	void *						noti_function;
} SOCK_CALLBACK_T;

typedef enum
{
	eCSO_IPCMSG_INIT,

	eCSO_IPCMSG_OPEN,

	eCSO_IPCMSG_CLOSE,	
	eCSO_IPCMSG_PROBE,		
	eCSO_IPCMSG_START,
	eCSO_IPCMSG_STOP,
	eCSO_IPCMSG_RESET,
	eCSO_IPCMSG_CONTROL,
	eCSO_IPCMSG_BUFFERINFO,	
	eCSO_IPCMSG_GET_SETTINGS,
	eCSO_IPCMSG_SET_SETTINGS,	
	eCSO_IPCMSG_GET_DEFSETTINGS,
	eCSO_IPCMSG_SET_DEFSETTINGS,	
	eCSO_IPCMSG_SET_RESOURCEMAP,		
	eCSO_IPCMSG_PROBEINFO,
	eCSO_IPCMSG_DRMINFO,

	eCSO_IPCMSG_INIT_RETURN,	
	eCSO_IPCMSG_OPEN_RETURN,		
	eCSO_IPCMSG_CLOSE_RETURN,
	eCSO_IPCMSG_RESET_RETURN,
	eCSO_IPCMSG_PROBE_RETURN,	
	eCSO_IPCMSG_START_RETURN,
	eCSO_IPCMSG_STOP_RETURN,
	eCSO_IPCMSG_CONTROL_RETURN,
	eCSO_IPCMSG_BUFFERINFO_RETURN,	
	eCSO_IPCMSG_GET_SETTINGS_RETURN,
	eCSO_IPCMSG_SET_SETTINGS_RETURN,	
	eCSO_IPCMSG_GET_DEFSETTINGS_RETURN,
	eCSO_IPCMSG_SET_DEFSETTINGS_RETURN,	
	eCSO_IPCMSG_SET_RESOURCEMAP_RETURN,			
	eCSO_IPCMSG_PROBEINFO_RETURN,
	eCSO_IPCMSG_DRMINFO_RETURN,
	
	eCSO_IPCMSG_REGISTERCALLBACK_MEDIAEVENT,
	eCSO_IPCMSG_REGISTERCALLBACK_ERROREVENT,
	eCSO_IPCMSG_MEDIAEVENT_FIRE,
	eCSO_IPCMSG_ERROREVENT_FIRE,

	eCSO_IPCMSG_GET_OUTPUTCONTROL,
	eCSO_IPCMSG_GET_OUTPUTCONTROL_RETURN,
	eCSO_IPCMSG_NONE
}SOCKMSG_Type;

/*	
	uint32_t tag;
	uint32_t length;
	if( length > 0 )
	{
		char value[length];
	}
	else
	{
		uint32_t count;
		{
			uint32_t length;
			char value[length];
		}[count]
	}
*/

typedef enum
{
	/* Params */
	eCSO_IPCTAG_URL,
	eCSO_IPCTAG_PLAYMODE, 	
	eCSO_IPCTAG_DEVICEID, 
	eCSO_IPCTAG_STOPPEDPOS, 
	eCSO_IPCTAG_CONTROL,	

	/* Settings */
	eCSO_IPCTAG_MAXSESSION,
	eCSO_IPCTAG_SAVEFILEPATH,	
	eCSO_IPCTAG_USERTIMEBASE,
	eCSO_IPCTAG_BUFFERINGMODE,	
	eCSO_IPCTAG_BUFFERTHRESHOLD,
	eCSO_IPCTAG_FASTFILLISLIMITED,
	eCSO_IPCTAG_FASTFILLLIMIT,
	eCSO_IPCTAG_MANAGEDFILLLIMIT,
	eCSO_IPCTAG_STARTTHRESHOLD,
	eCSO_IPCTAG_STARTCONSTANT,
	eCSO_IPCTAG_UNDERFLOWMODE,
	eCSO_IPCTAG_STARTTIME,
	eCSO_IPCTAG_ENDTIME,	
	eCSO_IPCTAG_ACCURATESEEK,	
	eCSO_IPCTAG_DECODERRATE,
	eCSO_IPCTAG_ASYNCPROBE,
	eCSO_IPCTAG_BYPASSING,
	eCSO_IPCTAG_STARTLOWESTSTREAM,
	eCSO_IPCTAG_STARTSTREAM,
	eCSO_IPCTAG_SELECTAUDIODESC,
	eCSO_IPCTAG_PREFERREDLANG,
	eCSO_IPCTAG_TLS_CANAME,	
	eCSO_IPCTAG_TLS_CAPATH,	
	eCSO_IPCTAG_TLS_CERTIPATH,
	eCSO_IPCTAG_TLS_PRIKEYPATH,	
	eCSO_IPCTAG_TLS_PRIPW,
	eCSO_IPCTAG_TLS_INSTANCE,		
	eCSO_IPCTAG_TLS_VALUE,
	eCSO_IPCTAG_PRDY_PRIKEYPATH,
	eCSO_IPCTAG_PRDY_CERTIPATH,
	eCSO_IPCTAG_PRDY_LICENSEDATA,
	eCSO_IPCTAG_PRDY_LAURL,
	eCSO_IPCTAG_PRDY_LIVETV,
	eCSO_IPCTAG_PRDY_CUSTOMDATA,
	eCSO_IPCTAG_VRW_COMPANYNAME,
	eCSO_IPCTAG_VRW_VCASADDR,
	eCSO_IPCTAG_NAGRAPRM_HDL,
	eCSO_IPCTAG_NAGRAPRM_EMI,
	eCSO_IPCTAG_NAGRAPRM_KEYSIZE,
	eCSO_IPCTAG_NAGRAPRM_KEY,
	eCSO_IPCTAG_RAMBUFFERINGMODE,	
	eCSO_IPCTAG_BLOCKSIZE,
	eCSO_IPCTAG_RAMBUFFERSIZE,
	eCSO_IPCTAG_PROBEBUFFERSIZE,	
	eCSO_IPCTAG_DLNA_BYTERATE,	
	eCSO_IPCTAG_DLNA_FILESIZE,		
	eCSO_IPCTAG_DLNA_DURATION,	
	eCSO_IPCTAG_DLNA_SAMPLERATE,	
	eCSO_IPCTAG_DLNA_BITSPERSAMPLE,	
	eCSO_IPCTAG_DLNA_ISSTEREO,		
	eCSO_IPCTAG_DTCPIP_AKEHANDLE,
	eCSO_IPCTAG_DTCPIP_KEY,
	eCSO_IPCTAG_RAOP_DST_IPADDRESS,
	eCSO_IPCTAG_RAOP_AES_KEY,
	eCSO_IPCTAG_RAOP_AES_IV,
	eCSO_IPCTAG_RAOP_DATA_PORT,
	eCSO_IPCTAG_RAOP_CONTROL_PORT,
	eCSO_IPCTAG_RAOP_TIMING_PORT,
	eCSO_IPCTAG_RAOP_FMT_STR,
	eCSO_IPCTAG_RAOP_AUDIO_CODEC,
	eCSO_IPCTAG_PIPETYPE,
	eCSO_IPCTAG_TRANSCODEMODE,
	eCSO_IPCTAG_DRMTYPE,	
	eCSO_IPCTAG_PROTOCOLINFO,
	eCSO_IPCTAG_USERAGENTSTRING,
	eCSO_IPCTAG_THRESHOLD_SW_DEC,
	eCSO_IPCTAG_EXITLIVE_BEFORE_UPDATE,
	eCSO_IPCTAG_DELAY_THRESHOLD_FOR_GEARUP,	

	/* Probe Info */
	eCSO_IPCTAG_MAXBITRATE,
	eCSO_IPCTAG_DURATION,
	eCSO_IPCTAG_PROGRAMS,
	eCSO_IPCTAG_TRACKS,
	eCSO_IPCTAG_VIDEOPESID,
	eCSO_IPCTAG_VIDEOSTREAMID,
	eCSO_IPCTAG_VIDEOCODEC,
	eCSO_IPCTAG_VIDEOWIDTH,
	eCSO_IPCTAG_VIDEOHEIGHT,
	eCSO_IPCTAG_AVAILSTARTTIME,
	eCSO_IPCTAG_INTERNALSEEKPOS,
	eCSO_IPCTAG_CURRENT_VIDEOTRACK,
	eCSO_IPCTAG_CURRENT_AUDIOTRACK,
	eCSO_IPCTAG_TOTALAUDIOTRACK,
	eCSO_IPCTAG_AUDIOTRACK_STREAMID,
	eCSO_IPCTAG_AUDIOTRACK_STREAMSRCID,
	eCSO_IPCTAG_AUDIOTRACK_STREAMIDENTIFIER,
	eCSO_IPCTAG_AUDIOTRACK_CODEC,
	eCSO_IPCTAG_AUDIOTRACK_CHANNELS,
	eCSO_IPCTAG_AUDIOTRACK_BITRATE,
	eCSO_IPCTAG_AUDIOTRACK_PROFILE,
	eCSO_IPCTAG_AUDIOTRACK_SAMPLE_RATE,	
	eCSO_IPCTAG_AUDIOTRACK_CODEC_ID,	
	eCSO_IPCTAG_AUDIOTRACK_BLOCKALIGN,		
	eCSO_IPCTAG_AUDIOTRACK_LANG,
	eCSO_IPCTAG_AUDIOTRACK_NAME,
	eCSO_IPCTAG_AUDIOTRACK_ROLE,
	eCSO_IPCTAG_TOTALVIDEOTRACK,
	eCSO_IPCTAG_VIDEOTRACK_STREAMID,
	eCSO_IPCTAG_VIDEOTRACK_STREAMSRCID,
	eCSO_IPCTAG_VIDEOTRACK_STREAMIDENTIFIER,
	eCSO_IPCTAG_VIDEOTRACK_CODEC,
	eCSO_IPCTAG_VIDEOTRACK_WIDTH,
	eCSO_IPCTAG_VIDEOTRACK_HEIGHT,
	eCSO_IPCTAG_VIDEOTRACK_NAME,
	eCSO_IPCTAG_VIDEOTRACK_ROLE,
	eCSO_IPCTAG_TOTALSUBTRACK,
	eCSO_IPCTAG_SUBTRACK_TYPE,
	eCSO_IPCTAG_SUBTRACK_LANG,
	eCSO_IPCTAG_SUBTRACK_STREAMID,
	eCSO_IPCTAG_SUBTRACK_STREAMSRCID,
	eCSO_IPCTAG_SUBTRACK_STREAMIDENTIFIER,
	eCSO_IPCTAG_SUBTRACK_DVB_COMPOSITION_PAGEID,
	eCSO_IPCTAG_SUBTRACK_DVB_ANCILLARY_PAGEID,
	eCSO_IPCTAG_SUBTRACK_EBU_MAGAZINE_NUM,
	eCSO_IPCTAG_SUBTRACK_EBU_PAGE_NUM,
	eCSO_IPCTAG_AUDIOPESID,
	eCSO_IPCTAG_AUDIOSTREAMID,
	eCSO_IPCTAG_AUDIOCODEC,
	eCSO_IPCTAG_SUPPORTTRICK,
	eCSO_IPCTAG_SUPPORTSEEK,
	eCSO_IPCTAG_PCRPID,
	eCSO_IPCTAG_CAPID,
	eCSO_IPCTAG_IS_ADAPTIVESTREAMING,

	/* BufferInfo */
	eCSO_IPCTAG_LASTDECODETIME,
	eCSO_IPCTAG_BUFFEREDBYTES,
	eCSO_IPCTAG_TOTALBYTESREMAINING,
	eCSO_IPCTAG_TOTALSTREAMBYTES,
	eCSO_IPCTAG_ARRIVALBYTEDPERSEC,
	eCSO_IPCTAG_STREAMBYTESPERSEC,
	eCSO_IPCTAG_BUFFEREDMILLISEC,
	eCSO_IPCTAG_BUFFERPERCENTAGE,
	eCSO_IPCTAG_REMAINBUFFERSEC,
	eCSO_IPCTAG_BUFFERCHUNKS,
	eCSO_IPCTAG_STARTOFFSETS,
	eCSO_IPCTAG_ENDOFFSETS,
	eCSO_IPCTAG_SEEKABLE_CHUNKS,
	eCSO_IPCTAG_SEEKABLE_START,
	eCSO_IPCTAG_SEEKABLE_END,

	/* Control Params  */
	eCSO_IPCTAG_CTRL_TRICKRATE,
	eCSO_IPCTAG_CTRL_SEEKPOS,	
	eCSO_IPCTAG_CTRL_PARAM0,	
	eCSO_IPCTAG_CTRL_PARAM1, 
	eCSO_IPCTAG_CTRL_PARAM2, 
	eCSO_IPCTAG_CTRL_PARAM3, 

	/* ResourceMap */
	eCSO_IPCTAG_RESOURCE_PUMPHANDLES,
	eCSO_IPCTAG_RESOURCE_DMAHANDLES,	
	eCSO_IPCTAG_RESOURCE_NUMOFPUMP,
	eCSO_IPCTAG_RESOURCE_NUMOFDMA,

	/* OutputControl for PlayReady */
	eCSO_IPCTAG_OUTPUTCTRL_PLDY_OUTPUTCONTROL_VERSION,
	eCSO_IPCTAG_OUTPUTCTRL_PLDY_MINOPL,
	eCSO_IPCTAG_OUTPUTCTRL_PLDY_OPLIDRESERVED_CIDS,
	eCSO_IPCTAG_OUTPUTCTRL_PLDY_OPLIDRESERVED_RGIDS,
	eCSO_IPCTAG_OUTPUTCTRL_PLDY_VOPI_VERSION,
	eCSO_IPCTAG_OUTPUTCTRL_PLDY_VOPI_ENTRIES,
	eCSO_IPCTAG_OUTPUTCTRL_PLDY_VOPI_PROTECTION_EX,
	eCSO_IPCTAG_OUTPUTCTRL_PLDY_AOPI_VERSION,
	eCSO_IPCTAG_OUTPUTCTRL_PLDY_AOPI_ENTRIES,
	eCSO_IPCTAG_OUTPUTCTRL_PLDY_AOPI_PROTECTION_EX,

	/* DrmInfo */
	eCSO_IPCTAG_DRMINFO_VRW_UNIQUEID_LENGTH,
	eCSO_IPCTAG_DRMINFO_VRW_UNIQUEID,
	eCSO_IPCTAG_DRMINFO_VRW_VERSION,
	eCSO_IPCTAG_DRMINFO_NAGRAPRM_PRMSYNTAX_LENGTH,
	eCSO_IPCTAG_DRMINFO_NAGRAPRM_PRMSYNTAX,

	/* HTTP Custom Header */
	eCSO_IPCTAG_HTTPCUSTOMHEADER_COUNT,	
	eCSO_IPCTAG_HTTPCUSTOMHEADER_NAME,
	eCSO_IPCTAG_HTTPCUSTOMHEADER_VALUE,
} SOCKMSG_Tag;

#define SOCK_MSG_HEADER_LEN		(5*4)
#define SOCK_MSG_MAXLEN			(32*1024 - SOCK_MSG_HEADER_LEN)
#define	SOCK_MSG_HEADER			0x01F2F304

typedef struct
{
	int header;
	int type;
	uint32_t sessionid;
	int ret;
	int len;
} SOCKMSG_MsgHeader;

typedef struct
{
	int header;
	int type;
	uint32_t sessionid;
	int ret;
	int len;
	char msg[SOCK_MSG_MAXLEN];
} SOCKMSG_Msg;

#define CSO_DEFAULT_BUFFER_BLOCKSIZE 16*1024		/**< Default value of Buffering block ( When used in 16 bytes, performance was good. ) */
#define CSO_DEFAULT_PROBE_BUFFERSIZE 6*1024*1024	/**< Default Value of Buffer Size for Probing */
#define CSO_DEFAULT_RAM_BUFFERSIZE 32*1024*1024		/**< Default Value of Buffer Size for RamBuffering */

/*! 
 *	\brief SYS_XXX Util Function 사용을 위한 Initialize
 *	\return CSO_ERR
 *	\see 
 */
CSO_ERR CSO_SYS_Init (void); // Temp for IPC 

/*! 
 *	\brief Calypso API 사용을 위한 Initialize
 *	\param settings : Calypso Default Setting Parameter (NULL 일 경우 Code 내 Default Setting 사용)
 *	\return CSO_ERR
 *	\see CSO_Settings_t
 */
CSO_ERR CSO_Init (CSO_Settings_t *settings);

/*! 
 *	\brief Calypso Uninitialize
 *	\return CSO_ERR
 *	\see CSO_Init
 */
CSO_ERR CSO_Uninit();

/*! 
 *	\brief Calypso Status 획득 ( Open된 Session 개수, Play 중인 Session 개수 등) 및 특정 Session 정보 (미구현)
 *	\param nSessionId : Status를 확인할 Session의 Id
 *	\param[out] pStatus : Status
 *	\return CSO_ERR
 *	\see CSO_Status_t
 */
CSO_ERR CSO_Status(int32_t nSessiondId, CSO_Status_t *pStatus);

/*! 
 *	\brief Calypso에 Setting 된 Default Setting을 획득 
 *	\param[out] settings : Default Setting
 *	\return CSO_ERR
 *	\see CSO_Settings_t
 */
CSO_ERR CSO_GetDefaultSettings (CSO_Settings_t *settings);

/*! 
 *	\brief Calypso에 Default Setting 값을 Setting.
 *	\param settings : Default Setting
 *	\return CSO_ERR
 *	\see CSO_Settings_t
 */
CSO_ERR CSO_SetDefaultSettings (CSO_Settings_t *settings);

/*! 
 *	\brief Calypso에 H/W Resource 정보(Playpump Index List, DMA Index List, ...) 를 Setting
 *	\param pMap : Setting 한 H/W Resource 정보
 *	\return CSO_ERR
 *	\see CSO_RESOURCE_MAP_t
 */
CSO_ERR CSO_SetResourceMap (CSO_RESOURCE_MAP_t* pMap);

/*! 
 *	\brief 컨텐츠의 URL과 재생 방식을 이용하여 Session을 할당
 *	\param pUrl : 사용할 컨텐츠의 URL
 *	\param eMode : Play 방식 ( eCSO_MODE_PLAY, eCSO_MODE_DNP, eCSO_MODE_DOWNLOAD )
 *	\param[out] puSessionId : 할당 받은 Session의 Id
 *	\return CSO_ERR
 *	\see CSO_PlayMode_e
 */
CSO_ERR CSO_Open (uint8_t *pUrl, CSO_PlayMode_e eMode, uint32_t *puSessionId);

/*! 
 *	\brief 외부에서 open 된 file decription 정보와 재생 방식을 이용하여 Session을 할당
 *	\param fd : 사용할 컨텐츠의 file decription
 *	\param offset : 사용할 컨텐츠의 현재 offset
 *	\param length : 사용할 컨텐츠의 length
 *	\param eMode : Play 방식 ( eCSO_MODE_PLAY, eCSO_MODE_DNP, eCSO_MODE_DOWNLOAD )
 *	\param[out] puSessionId : 할당 받은 Session의 Id
 *	\return CSO_ERR
 *	\see CSO_PlayMode_e
 */
CSO_ERR CSO_OpenFromFD(int32_t fd, int64_t offset, int64_t length, CSO_PlayMode_e eMode, uint32_t *puSessionId);

/*! 
 *	\brief Session에 설정된 컨텐츠의 정보를 획득하여 Session에 저장
 *	\param ulSessionId : Session Id
 *	\return CSO_ERR
 *	\see
 */
CSO_ERR CSO_Probe (uint32_t ulSessionId);

/*! 
 *	\brief Session에 Probe 된 정보와 Setting 된 설정을 이용해 컨텐츠 재생
 *	\param ulSessionId : Session Id
 *	\return CSO_ERR
 *	\see CSO_Stop
 */
CSO_ERR CSO_Start (uint32_t ulSessionId, uint32_t ulDeviceId);
/*! 
 *	\brief Session 에 전달되는 pUrl 정보를 통해 IP Live 수신 시작. 
 *	\param ulSessionId : Session Id
 *	\param pUrl : rtp://xxx.xxx.xxx:yyy (xxx: URI, yyy:port number)
 *	\return CSO_ERR
 *	\see
 */
CSO_ERR CSO_Tune(uint8_t *pUrl, uint32_t ulSessionId, uint32_t ulDeviceId);

/*! 
 *	\brief Session 에 전달되는 pUrl 정보를 통해 IP Live 수신 시작. 
 *	\param ulSessionId : Session Id
 *	\return CSO_ERR
 *	\see
 */
CSO_ERR CSO_TuneStop(uint32_t ulSessionId);

/*! 
 *	\brief 재생 중인 Session 을 중지 
 *	\param ulSessionId : Session Id
 *	\return CSO_ERR
 *	\see CSO_Start
 */
CSO_ERR CSO_Stop (uint32_t ulSessionId, int64_t llStoppedPos);

/*! 
 *	\brief Session을 해제 
 *	\param ulSessionId : Session Id
 *	\return CSO_ERR
 *	\see CSO_Open
 */
CSO_ERR CSO_Close (uint32_t ulSessionId);

/*! 
 *	\brief Session 의 FFMPEG IO Context 해제 
 *	\param ulSessionId : Session Id
 *	\return CSO_ERR
 *	\see CSO_Close
 */
CSO_ERR CSO_Reset(uint32_t ulSessionId);

CSO_ERR CSO_Control (uint32_t ulSessionId, CSO_CTRL_e eCtrl, CSO_CTRL_PARAM_t *pParam);
//CSO_ERR CSO_PlayInfo (uint32_t ulSessionId);

/*! 
 *	\brief Buffer 정보 획득 
 *	\param ulSessionId : Session Id
 *	\param[out] pInfo : Buffering Information 
 *	\return CSO_ERR
 *	\see CSO_BufferInfo_t
 */
CSO_ERR CSO_BufferInfo (uint32_t ulSessionId, CSO_BufferInfo_t *pInfo);

/*! 
 *	\brief Probe 정보 획득 
 *	\param ulSessionId : Session Id
 *	\param[out] probeInfo : Probe Information 
 *	\return CSO_ERR
 *	\see CSO_Probe, CSO_ProbeInfo_t
 */
CSO_ERR CSO_ProbeInfo (uint32_t ulSessionId, CSO_ProbeInfo_t *probeInfo);

/*! 
 *	\brief Playback 정보 획득, Platform 에 따라 지원 여부의 차이 존재
 *	\param ulSessionId : Session Id
 *	\param[out] pInfo : Playback Information 
 *	\return CSO_ERR
 *	\see 
 */
CSO_ERR CSO_PlayInfo(uint32_t ulSessionId, CSO_PlayInfo_t *pInfo);

/*! 
 *	\brief Session에 Setting 적용
 *	\param ulSessionId : Session Id
 *	\param settings : Setting Parameter 
 *	\return CSO_ERR
 *	\see CSO_GetSettings, CSO_Settings_t
 */
CSO_ERR CSO_SetSettings (uint32_t ulSessionId,CSO_Settings_t *settings);

/*! 
 *	\brief Session에 적용된 Setting 획득 
 *	\param ulSessionId : Session Id
 *	\param[out] settings : Setting Parameter 
 *	\return CSO_ERR
 *	\see CSO_SetSettings, CSO_Settings_t
 */
CSO_ERR CSO_GetSettings (uint32_t ulSessionId,CSO_Settings_t *settings);


CSO_ERR CSO_AddSource(uint32_t ulSessionId, void *src, CSO_ATTACHTYPE eType);

void CSO_IPC_ResigterCallback_MediaEvent(void *function);

/*! 
 *	\brief Session에 적용된 DRM 정보 획득 
 *	\param ulSessionId : Session Id
 *	\param[out] info : DRM Information
 *	\return CSO_ERR
 *	\see CSO_DrmInfo_t
 */
CSO_ERR CSO_DrmInfo(uint32_t ulSessionId, CSO_DrmInfo_t *info);

/*! 
 *	\brief Session에 적용된 Output Control 획득 
 *	\param ulSessionId : Session Id
 *	\param[out] pstOutputControl : Output Control
 *	\return CSO_ERR
 *	\see CSO_OutputControl_t
 */
CSO_ERR CSO_GetOutputControl(uint32_t ulSessionId, CSO_OutputControl_t *pstOutputControl);

/*! 
 *	\brief Generic Media Event Callback 등록
 *	\param function : Callback Function
 *	\return void
 *	\see
 */
void CSO_ResigterCallback_MediaEvent(void *function);

/*! 
 *	\brief Session Media Event Callback 등록 ( Session Media Event Callback을 등록할 경우 Generic Media Event Callback은 사용하지 않음)
 *	\param function : Callback Function
 *	\return void
 *	\see
 */
void CSO_RegisterCallback_SessionMediaEvent(uint32_t ulSessionId, void *function, void *owner_obj);

/*! 
 *	\brief Session Media Event Callback 등록 ( Session Media Event Callback을 등록할 경우 Generic Media Event Callback은 사용하지 않음)
 *	\param function : Callback Function
 *	\return void
 *	\see
 */
void CSO_RegisterCallback_DrmActuator(CSO_DrmActuatorCallback_t cbActuatorSet);


/*! 
 *	\brief Error Event Callback 등록
 *	\param function : Callback Function
 *	\return void
 *	\see
 */
void CSO_ResigterCallback_ErrorEvent(void *function);

#ifdef  __cplusplus
}
#endif

#endif
