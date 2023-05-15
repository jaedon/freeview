/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_rtsp.h  $
 * Version:			$Revision:   1.0  $
 * Original Author:	JungWook Chai
 * Current Author:	$Author: juchae@humaxdigital.com $
 * Date:			$Date: Thur Oct 29 10:20:48 KST 2009 $
 * File Description:	Real-Time STREAMMING IP Tuner Header File
 * Module: 		
 * Remarks:	
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd.							  */
/* All rights reserved.											*/
/*******************************************************************/



#ifndef __STREAM_RTSP_H__
#define __STREAM_RTSP_H__




#define STREAM_RTSP_CMD_OPTION 				"OPTIONS"
#define STREAM_RTSP_CMD_DESCRIBE			"DESCRIBE"
#define STREAM_RTSP_CMD_SETUP				"SETUP"
#define STREAM_RTSP_CMD_SET_PARAMETER	"SET_PARAMETER"
#define STREAM_RTSP_CMD_GET_PARAMETER	"GET_PARAMETER"
#define STREAM_RTSP_CMD_PLAY				"PLAY"
#define STREAM_RTSP_CMD_PAUSE				"PAUSE"
#define STREAM_RTSP_CMD_TEARDOWN			"TEARDOWN"

enum {
	MINIMUM_RESUME_START_TIME = 10,   /* second	 */
	RESUME_BUFFERING_TIME = 3 * 1000, /* milisecond */
};

enum {
	STREAM_CHECK_DATA_TIME = 5,
};

enum {
	AUTO_RETRY_DEFAULT = 10,
};

enum {
	RTSP_STATUS_OK = 200,
	RTSP_STATUS_SET_PARAMETER = 10,
	RTSP_STATUS_OPTIONS = 20,
	RTSP_STATUS_ANNOUNCE = 30,
};

enum {
	RTSP_UNKNOWN_PROTOCOL = 0,
	RTSP_REAL_PROTOCOL,
	RTSP_WMS_PROTOCOL,
};

enum {
	FORCE_NOT_METAFILE = -1,
	IS_NOT_METAFILE = 0,
	IS_METAFILE = 1,
};

/*
 * for parsing command line and environment options
 */
struct options_t {
	char *local_filename;
	char *logfile;
	char *protocol;
	char *username;
	char *password;
	char *http_proxy;

	HINT32 loglevel; /* 0:quiet   1:normal   2:verbose   3:debug */
	HINT32 auto_retry_times;
	HINT32 resume;
	
	HUINT32 bandwidth;

	/* ftp  */
	HINT32 no_passive_ftp_f; /* default is passive ftp */

	/* rtsp */
	char *speed;
	char *range;

	/* http & ftp */
	char *byterange;
	
	struct RTSP_LinkedListHdr_s *targets; /* target_t list */
};

struct target_t {
	HINT32 metafile_f; /* tri-state */
	char *target_name;
};

struct rtsp_field_t {
	char *field_name;
	struct rtsp_field_t *next;
};

/*
 * option to pass to download_streaming function.
 * everything not file pointer and url goes here.
 */
struct download_opts_t {
	HINT32 dl_protocol;	  /* specify protocol */
	
	HUINT32 bandwidth;
	char *speed;
	char *range;
	char *byterange;
	HINT32 udp_port;
	HINT32 data_transport_protocol;
	
	HINT32 auto_retry;
	HINT32 resume_download;  /* resume flag	 */

	union {
		char *proxy;	  /* raw string data */
		char *http_proxy; /* proxy string	*/
	} proxy; /* Added union name: warning: ISO C doesn't support unnamed structs/unions */

	/* ftp optoins */
	HINT32 no_passive_ftp; /* ftp default is passive mode */

	/* username & password */
	char *username;
	char *password;
};

struct dlresult_t {
	struct RTSP_LinkedListHdr_s *success_list; /* char[] array */
	struct RTSP_LinkedListHdr_s *failed_list;  /* char[] array */
};

struct rtsp_ctrl_t {
	HINT32 rtsp_protocol;	/* which rtsp protocol, such as RTSP_REAL_PROTOCOL */
  
	char *mrl;			/* this is rtsp-conpatible URL. rtsp://ss:port/pp  */
	char *server;
	HUINT32 server_state;
	HUINT32 server_caps;

	HUINT32 cseq;
	HUINT32 uiLastAnswerCSeq;
	char *session;
	HUINT32 keepalive_interval; /* MS-RTSP 3.1.2 min 10 sec max 4294967 sec */
	char *etag;
	char *challenge; /* only in Real */

	HINT32 interleaved_data_packet_size;
	
	HUINT32 resume_start_ts; /* only in wmserver */

	HINT32 (*get_media_packet)(struct RTSP_ACCESS_SYS_s *stream,
				HUINT8 *buffer,size_t max_size);
  
	/* data for rtsp_read. it has to hold last previous packet data. */
	HINT32 prev_ts;
	HINT32 prev_stream_number;
	
	union {
	//	struct rmff_header_t *rmff_header;
		ASF_HEADER_t *pAsfHeaderInfo;
	} asf; /* Added union name: warning: ISO C doesn't support unnamed structs/unions */
};


struct RTSP_PROT_HEADER_s {
	char *protocol;
	char *method;
	char *uri;
	HUINT32 status_code;
	char *reason_phrase;
	HUINT32 rtsp_minor_version;
	HUINT32 content_length;
  

	/* field variables */
	struct rtsp_field_t *first_field;
	struct rtsp_field_t *last_field;
	HUINT32 field_nb;
	char *field_search;
	struct rtsp_field_t *field_search_pos;
  
	/* body variables */
	char *body;
	size_t body_len;				 /* NOT include '\0' at the end. */
	char *buffer;
	size_t buffer_len;			   /* NOT include '\0' at the end. */
	HUINT32 is_parsed;
  
};

/**/


/**/

#if 0
struct target_t *new_target_t(char *name,HINT32 meta_f);
void free_target_t(struct target_t *t);
struct options_t *new_options_t(void);
void free_options_t(struct options_t *opt);
#endif
struct download_opts_t *STREAM_RTSP_NewRtspOption(void);
void STREAM_RTSP_FreeRtspOption(struct download_opts_t *dlopts);
struct dlresult_t *new_dlresult_t(void);
void free_dlresult_t(struct dlresult_t *dlr);




/**/

struct RTSP_PROT_HEADER_s *STREAM_RTSP_NewRtspProtocolHeader(void);
void STREAM_RTSP_FreeRtspProtoclHeader(struct RTSP_PROT_HEADER_s *h);
char *STREAM_RTSP_GetNextField(struct RTSP_PROT_HEADER_s *rtsp_hdr);
char *STREAM_RTSP_FindFieldFromRtspHeader(struct RTSP_PROT_HEADER_s *rtsp_hdr, const char *field_name);
void STREAM_RTSP_SetFieldForRtspHeader(struct RTSP_PROT_HEADER_s *rtsp_hdr, const char *field_name);

void STREAM_RTSP_SetUriForRtspHeader(struct RTSP_PROT_HEADER_s *rtsp_hdr,const char *uri);
void STREAM_RTSP_SetProtoclCmdForRtspHeader(struct RTSP_PROT_HEADER_s *rtsp_hdr, const char *method);
char *STREAM_RTSP_SerializeRtspHeader(struct RTSP_PROT_HEADER_s *rtsp_hdr);

HINT32 STREAM_RTSP_IsEndOfRtspHeader(struct RTSP_PROT_HEADER_s *rtsp_hdr);
HINT32 STREAM_RTSP_DeserializeRtspResponse(struct RTSP_PROT_HEADER_s *rtsp_hdr);
struct RTSP_ACCESS_SYS_s *STREAM_RTSP_NewRtspAccessSysAndRtspCtrl(void);
void STREAM_RTSP_SendTeardownAndFreeRtspAccessSys(struct RTSP_ACCESS_SYS_s *stream);

struct RTSP_PROT_HEADER_s *STREAM_RTSP_NewRtspHeaderAndFillBasicFields(struct rtsp_ctrl_t *rtsp_ctrl);
HINT32 STREAM_RTSP_SendSerializedRtspRequestAndFree(struct RTSP_ACCESS_SYS_s *stream,struct RTSP_PROT_HEADER_s *rtsp_hdr);
HINT32 STREAM_RTSP_CheckRtspResponseOk(HINT32 status_code);
HINT32 STREAM_RTSP_ReadRtspResponseAndFree(struct RTSP_ACCESS_SYS_s *stream);
HINT32 STREAM_RTSP_ReadAndFreeBody(struct RTSP_ACCESS_SYS_s *stream,struct RTSP_PROT_HEADER_s *rtsp_hdr);
HINT32 STREAM_RTSP_ReadRtspHeaderAndParseBasic(struct RTSP_ACCESS_SYS_s *stream,struct RTSP_PROT_HEADER_s *rtsp_hdr);

HINT32 STREAM_RTSP_SetRtspHeaderWithBasicFields(struct rtsp_ctrl_t *rtsp_ctrl,
				 struct RTSP_PROT_HEADER_s *rtsp_hdr);

HINT32 STREAM_RTSP_SendRtspOk(struct RTSP_ACCESS_SYS_s *stream,HINT32 cseq,char *session);

char *rtsp_request_options(struct RTSP_PROT_HEADER_s *rtsp_hdr,char *request_uri);
char *rtsp_request_describe(struct RTSP_PROT_HEADER_s *rtsp_hdr,char *request_uri);
char *rtsp_request_set_parameter(struct RTSP_PROT_HEADER_s *rtsp_hdr,char *request_uri);
char *rtsp_request_get_parameter(struct RTSP_PROT_HEADER_s *rtsp_hdr,char *request_uri);
char *rtsp_request_setup(struct RTSP_PROT_HEADER_s *rtsp_hdr,char *request_uri);
char *rtsp_request_play(struct RTSP_PROT_HEADER_s *rtsp_hdr,char *request_uri);
char *rtsp_request_teardown(struct RTSP_PROT_HEADER_s *rtsp_hdr,char *request_uri);
char *rtsp_request_pause(struct RTSP_PROT_HEADER_s *rtsp_hdr,char *request_uri);



HINT32 STREAM_RTSP_SendSerializedRtspRequest(struct RTSP_ACCESS_SYS_s *stream,struct RTSP_PROT_HEADER_s *rtsp_hdr);
HINT32 STREAM_RTSP_ReadRtspHeaderAndParseBasic(struct RTSP_ACCESS_SYS_s *stream, struct RTSP_PROT_HEADER_s *rtsp_hdr);
HINT32 STREAM_RTSP_NewRefinedUrl(struct rtsp_ctrl_t *rtsp_ctrl,char *host,HINT32 port,char *filepath);


char *rtsp_make_range_from_timestamp(HUINT32 range_ts);
HINT32 rtsp_npt_time_valid(const char *str,char **reason_ret);
HINT32 rtsp_npt_range_valid(const char *str,char **reason_ret);
char *rtsp_time_to_npt_time(const char *str,char **reason_ret);
char *rtsp_range_to_npt_range(const char *str,char **reason_ret);


void STREAM_RTSP_SetRangeField(struct RTSP_PROT_HEADER_s *rtsp_hdr,const char *rangestr);
void STREAM_RTSP_SetSpeedField(struct RTSP_PROT_HEADER_s *rtsp_hdr,const char *speedstr);

HINT32 rtsp_prepare_download(char *target_str,struct options_t *options,
				URL_t **url_ret,struct download_opts_t **dlopts_ret,
				char **local_name);





HINT32  MMSR_Open ( STREAM_ACCESS_t *pAccess );
ssize_t MMSR_Read ( STREAM_ACCESS_t *pAccess, HUINT8 *p_buffer, size_t i_len );
HINT32 MMSR_Seek ( STREAM_ACCESS_t * pAccess, HINT64 i_pos );
HINT32 MMSR_Control ( STREAM_ACCESS_t *pAccess, DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2);
void MMSR_Close ( STREAM_ACCESS_t *pAccess );



#endif /* __STREAM_RTSP_H__ */
