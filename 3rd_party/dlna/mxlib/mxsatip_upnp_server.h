

// Author : Park, Doyu.

#pragma once

#include "mxapi_define.h"
#include "mxlib_config.h"
#include "mxsatip_upnp.h"

typedef enum _MXSATIP_NEGOTIATION_DEVICE_STATUS {
	MXSATIP_NEGOTIATION_DEVICE_STATUS_ACQUISITION,
	MXSATIP_NEGOTIATION_DEVICE_STATUS_COLLISION_OWNER_MINE,
	MXSATIP_NEGOTIATION_DEVICE_STATUS_COLLISION_OWNER_OTHER,
} MXSATIP_NEGOTIATION_DEVICE_STATUS;


typedef void* MXSATIP_UPNP_PARENT_HANDLE;
typedef void (*MXSATIP_UPNP_SERVER_NEGOTIATION_CALLBACK)(MXSATIP_UPNP_PARENT_HANDLE handle, int deviceId, int status);


typedef struct _MXSATIP_SERVER_CHANGE_DATA {
	char *name;	
	unsigned int bootId;

	int notify_flag;
} MXSATIP_SERVER_CHANGE_DATA, *MXSATIP_SERVER_CHANGE_DATA_HANDLE;

typedef void* MXSATIP_UPNP_SERVER_HANDLE ;
MXSATIP_UPNP_SERVER_HANDLE mxsatip_upnp_server_start(void *parent_handle, MXSATIP_DEVICE *device_info, int web_port, MXSATIP_UPNP_SERVER_NEGOTIATION_CALLBACK device_negotiation_callback);
MX_RETURN mxsatip_upnp_server_stop(MXSATIP_UPNP_SERVER_HANDLE upnp_server_handle);
int mxsatip_upnp_server_getDeviceID(MXSATIP_UPNP_SERVER_HANDLE upnp_server_handle);
MX_RETURN mxsatip_upnp_server_rename(MXSATIP_UPNP_SERVER_HANDLE upnp_server_handle, char *rename);
MX_RETURN mxsatip_upnp_server_set_bootid(MXSATIP_UPNP_SERVER_HANDLE upnp_server_handle,unsigned int bootId);
MX_RETURN mxsatip_upnp_server_change_data(MXSATIP_UPNP_SERVER_HANDLE upnp_server_handle, MXSATIP_SERVER_CHANGE_DATA_HANDLE data); 
