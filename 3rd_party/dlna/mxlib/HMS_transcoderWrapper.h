#ifndef HMS_TRANSCODERWRAPPER_H_
#define HMS_TRANSCODERWRAPPER_H_




#ifdef __cplusplus
extern "C" {
#endif

#if defined(WIN32)
#include "stdafx.h"
#endif

#include "TransExternApi.h"
#include "Transcoder_data_struct.h"
#include "queue_manager.h"

#if defined( HUMAX_HW_TRANSCODER)

#define MAX_CONTAINER 10				// 2 sec * 10 = 20 sec

typedef struct _trans_Info_tag_HMS
{
    int						ph_Stream;
    TRANSCODER_BUFFER       tbSegmenterDataBuf[MAX_CONTAINER];
    char    bClearBuffer;  // 2013/12/12 mhkang: RTSP PLAY호출할때 1로 셋팅, RTP streaming thread에서 Get TS할때 0으로 셋팅
}HMS_TRANSCODER_INSTANCE, *PHMS_TRANSCODER_INSTANCE;


int HMS_TransWrap_Init(void * pExApi);
int HMS_TransWrap_Deinit();
int HMS_TransWrap_getError(PTRANSCODER_INSTANCE ptranscoder, char **pszError);


Streaming_RetCode_e HMS_TransWrap_Common_startLiveStreaming(
				PTRANSCODER_INSTANCE ptranscoder,
				TRANSCODER_REQUSET_DEVICE_TYPE svc,
				TRANSCODER_REQUEST_FLAG_TYPE media,
				const char *device_name,
				const char *szUser_Agent,
				const char* filename,
				const char* subtitle_filename,
				int socketfd[2]
				);

int HMS_TransWrap_HLS_startLiveStreaming(
/*******************************************************************************/
				PTRANSCODER_INSTANCE ptranscoder, 
				TRANSCODER_REQUSET_DEVICE_TYPE svc,
				TRANSCODER_REQUEST_FLAG_TYPE media,
				const char *device_name, 
				const char *szUser_Agent,
				const char* filename, 
                const char* subtitle_filename,
                int socketfd[2]
				);


int HMS_TransWrap_HLS_getM3U8NameByFileName(
				PTRANSCODER_INSTANCE ptranscoder, 
				const char* device_name, //yhkang added for profile 20120103
				const char* filename,
				const char* subtitle_filename,
				char **szM3U8Name //I assign, you free														 
				);

int HMS_TransWrap_HLS_getM3U8Contents(PTRANSCODER_INSTANCE ptranscoder, char*	m3u8_filename, PTRANSCODER_BUFFER m3u8_contents);
int HMS_TransWrap_HLS_getSegment(PTRANSCODER_INSTANCE ptranscoder, const char* ts_filename, PTRANSCODER_BUFFER psegment_contents);
void  HMS_TransWrap_HLS_StopLiveStreaming(PTRANSCODER_INSTANCE ptranscoder);
void HMS_TransWrap_RegisterCall(fp_TRANSCODE_HLS_State pfunc);
int HMS_TransWrap_SATIP_getInfo(PTRANSCODER_INSTANCE ptranscoder, TRANSCODER_REQUSET_INFO_TYPE type, const char* url, char **info, int *size);

// 2013/12/12 mhkang: SATIP용 TS GET함수
int HMS_TransWrap_SATIP_getSegment(PTRANSCODER_INSTANCE ptranscoder, const char* ts_filename, PTRANSCODER_BUFFER psegment_contents);


#endif
#ifdef __cplusplus
}
#endif

#endif 

