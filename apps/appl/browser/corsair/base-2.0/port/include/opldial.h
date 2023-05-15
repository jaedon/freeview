#ifndef __OPL_DIAL_H__
#define __OPL_DIAL_H__

#include	"bpltype.h"

#define ________________Typedef____________________________________
typedef void	(*OPL_DIAL_EventCallback)(unsigned int event, char *app, void* pData);

#ifdef __cplusplus
extern "C" {
#endif

BPLStatus	OPL_DIAL_Start( void );
BPLStatus	OPL_DIAL_Stop( void );

BPLStatus	OPL_DIAL_RegisterListener(OPL_DIAL_EventCallback listener );
BPLStatus	OPL_DIAL_UnRegisterListener(void);

BPLStatus 	OPL_DIAL_IsActivated(HBOOL *bEnable);
BPLStatus 	OPL_DIAL_SetFriendlyName( const HCHAR *friendlyName );
BPLStatus 	OPL_DIAL_GetFriendlyName(HCHAR *friendlyName, int nLen);

BPLStatus 	OPL_DIAL_ApplicationRegist(const HCHAR *AppName);
BPLStatus 	OPL_DIAL_ApplicationUnregist(const HCHAR *AppName);
BPLStatus 	OPL_DIAL_ApplicationStateUpdate(const HCHAR *AppName, HUINT32 state);

BPLStatus 	OPL_DIAL_SetCookie( const HCHAR * cookie );

#ifdef __cplusplus
};

#endif // __cplusplus
#endif // __OPL_DIAL_H__
