
// Author : Park, Doyu.

#pragma once

#include "mxtypes.h"

typedef struct _MXSATIP_DEVICE
{
	char *baseIp;
	unsigned int bootId;				// discovery data
	unsigned int configId;				// discovery data
	unsigned int deviceId;				// discovery data

	char *specVersion;					// control point is not parsing data, not used.
	char *friendlyName;
	char *manufacturer;
	char *manufacturerURL;
	char *modelDescription;
	char *modelName;
	char *modelNumber;
	char *modelURL;
	char *serialNumber;					// control point is not parsing data, not used.
	char *UDN;

	int IconsLength;
	struct _MXSATIP_ICON *Icons;			// !- same "struct UPnPIcon";

	char *presentationURL;
	char *X_SATIPCAP;
	
	struct _MXSATIP_DEVICE *pNext;

	int Acquisition;					// for SAT>IP Server
	unsigned short searchport;			// dypark : LXC Search Port (2014-06-25)

} MXSATIP_DEVICE;


// ! - same "struct UPnPIcon";
typedef struct _MXSATIP_ICON 
{
	int width;
	int height;
	int colorDepth;
	char *mimeType;
	char *url;
	char *full_path;
	
} MXSATIP_ICON;

MX_RETURN mxsatip_upnp_copy_device(void* in_device_ref, MXSATIP_DEVICE *out_satip_device);
MX_RETURN mxsatip_upnp_displace_device(void* in_device_ref, MXSATIP_DEVICE *out_satip_device);
MX_RETURN mxsatip_upnp_delete_device(MXSATIP_DEVICE *satip_device);
MX_RETURN mxsatip_upnp_duplicate_device(MXSATIP_DEVICE *input_satip_device, MXSATIP_DEVICE *out_satip_device);

//MX_RETURN mxsatip_upnp_copy_device(struct UPnPDevice* in_device, MXSATIP_DEVICE *out_satip_device);
//MX_RETURN mxsatip_upnp_displace_device(struct UPnPDevice* in_device, MXSATIP_DEVICE *out_satip_device);

MXSATIP_ICON * mxsatip_upnp_duplicate_icons(MXSATIP_ICON *in_Icons, int IconsLength);
MX_RETURN mxsatip_upnp_delete_icons(MXSATIP_ICON *Icons, int IconsLength);

extern unsigned long mxsatip_generate_boot_id(void);
