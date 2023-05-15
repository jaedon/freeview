#ifndef __OPL_SATIP_H__
#define __OPL_SATIP_H__

#include	"bpltype.h"

#define ________________Typedef____________________________________
typedef void	(*OPL_SATIP_EventCallback)(unsigned int event, char *app, void* pData);

#ifdef __cplusplus
extern "C" {
#endif

BPLStatus	OPL_SATIP_Start( void );
BPLStatus	OPL_SATIP_Stop( void );

BPLStatus	OPL_SATIP_RegisterListener(OPL_SATIP_EventCallback listener );
BPLStatus	OPL_SATIP_UnRegisterListener(void);

BPLStatus 	OPL_SATIP_IsActivated(HBOOL *bEnable);
BPLStatus 	OPL_SATIP_SetFriendlyName( const HCHAR *friendlyName );
BPLStatus 	OPL_SATIP_GetFriendlyName(HCHAR *friendlyName, int nLen);

#ifdef __cplusplus
};

#endif // __cplusplus
#endif // __OPL_DIAL_H__
