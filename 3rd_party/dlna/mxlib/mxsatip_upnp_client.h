
// Author : Park, Doyu.

#pragma once

typedef void * SATIP_CP_HANDLE;

MX_RETURN mxsatip_upnp_client_start(MXSATIP_CLIENT_DEVICE_ADDREMOVE device_callback);
MX_RETURN mxsatip_upnp_client_stop();
MX_RETURN mxsatip_upnp_client_send_msearch();
MXSATIP_DEVICE *mxsatip_upnp_client_get_server_list();
MXSATIP_DEVICE *mxsatip_upnp_client_get_server_listEx(MXSATIP_DEVICE *exception_device, int added);
MX_RETURN mxsatip_upnp_client_free_server_list(MXSATIP_DEVICE *list);

