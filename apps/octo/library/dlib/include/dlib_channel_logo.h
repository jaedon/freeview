/* $Header: $ */

#ifndef DLIB_CHANNEL_LOGO_H__
#define DLIB_CHANNEL_LOGO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <hlib.h>

#define CHANNEL_LOGO_URL_LENGTH		256

typedef struct
{
	HINT32				svcuid;
	HCHAR				ucServerLogoUrl[CHANNEL_LOGO_URL_LENGTH];
	HCHAR				ucLocalLogoUrl[CHANNEL_LOGO_URL_LENGTH];
} DxChannelLogo_t;


#ifdef __cplusplus
}
#endif

#endif


