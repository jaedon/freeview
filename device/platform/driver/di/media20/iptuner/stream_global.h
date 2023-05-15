/*****************************************************************************
 * stream_global.h:
 *****************************************************************************
 *****************************************************************************/

#ifndef __STREAM_GLOBAL_H__
#define __STREAM_GLOBAL_H__


#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <stdarg.h>
#include "htype.h"

typedef enum
{
	PROTOCOL_HTTP,
	PROTOCOL_MMSTU,
	PROTOCOL_MMSH,
	PROTOCOL_RTSP,
	PROTOCOL_UDP,  /* Multicast/UDP */
	PROTOCOL_RTP,  /* Multicast/RTP */
	PROTOCOL_UNKNOWN
}STREAM_PROTOCOL_e;

typedef void (*STREAM_VerifyExternalCallback) (void *context, void *userdata, void *verifyhandle, HINT32 *result);

typedef struct STREAM_VerifyExternalCallback_s {
	STREAM_VerifyExternalCallback callback;
	void *context;
	void *userdata;
} STREAM_VerifyExternalCallback_t;

typedef void (*STREAM_Callback) (void *context, int param1, char *param2);

typedef struct STREAM_CallbackDesc_t {
	STREAM_Callback callback;
	void *context;
	int param1;
	char *param2;
} STREAM_CallbackDesc;

/* DLNA.ORG_PS: play speed parameter (integer)
 *     0 invalid play speed
 *     1 normal play speed
 *     2 trick mode
 */
typedef enum {
  STREAM_DLNA_SPEED_INVALID = 0,
  STREAM_DLNA_SPEED_NORMAL = 1,
  STREAM_DLNA_SPEED_TRICK = 2,
} STREAM_DLNA_SPEED_e;

typedef struct STREAM_DLNA_Setting_s {
	HBOOL						b_dlna;
	HBOOL						b_byte_seek;
	HBOOL						b_time_seek;
	HBOOL						b_limited_seek;
	HINT8						i_limited_mode;
	HINT64      				i_first_pos;
	HINT64      				i_end_pos;
	HINT32						i_byterate;
	HUINT32						i_duration;
	STREAM_DLNA_SPEED_e			i_server_trickmode;
	HINT8						sz_trick_type[5];
} STREAM_DLNA_Setting_t;

typedef struct STREAM_DTCPIP_Settings_s
{
	HBOOL						bDtcpIp;
	void						*psz_pcp_packethandle;
	HBOOL						b_dtcp_pcpparsing;
	HUINT32						i_extendedemi;
	HUINT32						i_pcp_contentlength;
	HUINT32						i_pcp_contentreadlength;
} STREAM_DTCPIP_Settings_t;

typedef struct stream_access_s
{
	/* Access path/url/filename/.... */
	char        	*psz_path;
	char			*psz_cookie;

	/* YouView Specific Parameters */
	char 			*psz_user_agent;
	char 			*psz_index_file;
	char 			*psz_bps_value;

	/* TLS Specific Parameters */
	char			*psz_prikey_passwd;
	char			*psz_ca_name;
	char			*psz_ca_path;
	char			*psz_certi_path;
	char			*psz_prikey_path;
	STREAM_VerifyExternalCallback_t	*cbTlsCerficate;

	/* Access has to maintain them uptodate */
	STREAM_PROTOCOL_e  	proto;
	HBOOL				b_alive;
	HBOOL				b_openfromnexus;

	/* Access has to be mutual-excluded each other Access */
	unsigned long   	ulStreamSema;

	/* For Back-Off Algo. */
	HBOOL				b_immediately_exit;
	HBOOL				b_retrying;

	/* For Dlna. */
	char				*psz_protocolinfo;
	STREAM_DLNA_Setting_t	t_dlna;
	STREAM_DTCPIP_Settings_t	t_dtcpip;

	/* Error Event Callback */
	STREAM_CallbackDesc	*callback;

	struct
	{
		unsigned int 	i_update;  	/* Access sets them on change, Input removes them once take into account*/
		HINT64      	i_size;    		/* Write only for access, read only for input */
		HINT64      	i_pos;     		/* idem */
		HBOOL   		b_eof;     		/* idem */
		int				i_title;    	/* idem, start from 0 (could be menu) */
		int				i_seekpoint;	/* idem, start from 0 */
		HBOOL   		b_prebuffered; 	/* Read only for input */
		int				i_httpstatecode; /* HTTP State Code (for YouTube Redirection */
	} info;
	void *p_sys;	/** Stream Specific Structure . see protocol the controller's header file*/
} STREAM_ACCESS_t;

#endif /* __STREAM_GLOBAL_H__ */
