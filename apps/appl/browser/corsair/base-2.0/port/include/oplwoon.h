/**************************************************************************************/
/**
 * @file oplnetwork.h
 *
 * Network Interfaces
 *
 **************************************************************************************
 **/

#ifndef __OPLWOON_H__
#define __OPLWOON_H__

#include "bpltype.h"

/******************************************************************
	Define
******************************************************************/


/******************************************************************
	Enum
 ******************************************************************/


/******************************************************************
	Typedef
******************************************************************/
typedef struct
{
	HINT32	isOperated;
} WOONS_OPERATION_RESULT_t;

typedef struct
{
	HINT32	nState;
	HCHAR	szLocalIp[128]; 	
	HCHAR	szExtIp[128];			
	HCHAR	szPubIp[128];
	HINT32	nPubPort; 
	HINT32	nExtPort;
} WOONS_INFO_t;

typedef struct
{
	HBOOL	bOpen;
	HCHAR	szPinCode[5];
} WOONS_PINCODE_t;

typedef WOONS_OPERATION_RESULT_t WoonServerOpResult;
typedef WOONS_INFO_t WoonServerInfo;
typedef WOONS_PINCODE_t WoonServerPinCode;

typedef void (*OPL_WOONSERVER_EventCallback)(unsigned int event, unsigned state, void *result);


/******************************************************************
	Interfaces
******************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************
	WoonServer Interfaces
******************************************************************/
extern BPLStatus	OPL_WOONSERVER_Start( const HCHAR *szFriendlyName );
extern BPLStatus	OPL_WOONSERVER_Stop( void );
extern BPLStatus 	OPL_WOONSERVER_RegEventListener( OPL_WOONSERVER_EventCallback listener );
extern BPLStatus 	OPL_WOONSERVER_UnRegEventListener();
extern BPLStatus 	OPL_WOONSERVER_TestConnection();
extern BPLStatus 	OPL_WOONSERVER_GetServerName(HCHAR *szFriendlyName, int nLen);
extern BPLStatus 	OPL_WOONSERVER_IsActivated(HBOOL *bActivated);
extern BPLStatus 	OPL_WOONSERVER_SetServerName(const HCHAR *szFriendlyName);

// END : WOONSERVER Interfaces

#ifdef __cplusplus
};
#endif

#endif

