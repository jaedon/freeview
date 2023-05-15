
// Author : Park, Doyu.

#pragma once

#include "mxapi_define.h"
#include "mxlib_config.h"
#include "mxsatip_upnp.h"

typedef void (*MXSATIP_CLIENT_DEVICE_ADDREMOVE) (MXSATIP_DEVICE *device, int added);

__MXAPI MX_RETURN mxsatip_client_create(MXSATIP_CLIENT_DEVICE_ADDREMOVE device_callback);
__MXAPI MX_RETURN mxsatip_client_destroy();
__MXAPI MX_RETURN mxsatip_client_msearch();
__MXAPI MXSATIP_DEVICE *mxsatip_client_getServerList();
__MXAPI MXSATIP_DEVICE *mxsatip_client_getServerListEx(MXSATIP_DEVICE *device, int added);
__MXAPI MX_RETURN  mxsatip_client_freeServerList(MXSATIP_DEVICE *list);
