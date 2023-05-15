
// Author : Park, Doyu.

#pragma once

#include "mxapi_define.h"
#include "mxlib_config.h"
#include "mxtypes.h"
#include "mxsatip_upnp.h"
#include "mxsatip_upnp_server.h"

#define MXSATIP_MAX_STREAMID_SIZE		32															// max int 32 (  -2,147,483,648 ~ 2,147,438,647 )
#define MXSATIP_MAX_IP_SIZE				17															// 123.123.123.123 + 'A'
#define MXSATIP_MAX_SESSIONID_SIZE		(33 + MXSATIP_MAX_IP_SIZE)								 	// max int 32 (  -2,147,483,648 ~ 2,147,438,647 ) + 'A' + 'B'
#define MXSATIP_MAX_KEY_SIZE			(MXSATIP_MAX_SESSIONID_SIZE + MXSATIP_MAX_STREAMID_SIZE + 1)	// 'A' + 'B' + 'C'

#define MXSATIP_SERVER_TIME_OUT_UNICAST		60
#define MXSATIP_SERVER_TIME_OUT_MULTICAST	0

__MXAPI MX_RETURN mxsatip_server_create(MXSATIP_DEVICE *devce_info, int web_port, int rtsp_port, int rtsp_max_connection,int max_frontends, MXSATIP_UPNP_SERVER_NEGOTIATION_CALLBACK device_negotiation_callback);
__MXAPI MX_RETURN mxsatip_server_destroy(void);

__MXAPI MX_RETURN mxsatip_server_change_data(MXSATIP_SERVER_CHANGE_DATA_HANDLE change_data);

__MXAPI MX_RETURN mxsatip_server_rename(char *server_name);
__MXAPI MX_RETURN mxsatip_server_set_bootid(unsigned int bootId);

