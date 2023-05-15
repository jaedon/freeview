/**************************************************************************************/
/**
 * @file oplstreamevent.h
 *
 * AIT Interfaces
 *
 **************************************************************************************
 **/

#ifndef __OPL_STREAMEVENT_H__
#define __OPL_STREAMEVENT_H__

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
typedef struct _OplStreamEvent_t {
	HCHAR	*szEventName;
	HCHAR	*szData;
	HCHAR	*szText;
	HCHAR	*szStatus;
} OplStreamEvent_t;

typedef void (* OplStreamEventListener_t)(OplStreamEvent_t *pstEvent);

/******************************************************************
	Interfaces
******************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

BPLStatus OPL_StreamEvent_AddListener(HCHAR *pszTargetUrl, HCHAR *szEventName, OplStreamEventListener_t listener);
BPLStatus OPL_StreamEvent_RemoveListener(HCHAR *pszTargetUrl, HCHAR *szEventName, OplStreamEventListener_t listener);


#ifdef __cplusplus
};

/******************************************************************
	class oplstreameventManager
******************************************************************/
class OplStreamEventManager {
public:
	OplStreamEventManager() {}
	virtual ~OplStreamEventManager() {}

public:
	bool setListener(HCHAR *pszTargetUrl, HCHAR *szEventName, OplStreamEventListener_t listener)
	{ return OPL_StreamEvent_AddListener(pszTargetUrl, szEventName, listener) == BPL_STATUS_OK; }

	bool removeListener(HCHAR *pszTargetUrl, HCHAR *szEventName, OplStreamEventListener_t listener)
	{ return OPL_StreamEvent_RemoveListener(pszTargetUrl, szEventName, listener) == BPL_STATUS_OK; }
};

#endif

#endif	// __OPL_STREAMEVENT_H__
