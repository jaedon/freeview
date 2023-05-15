/**************************************************************************************/
/**
 * @file oplrcf.h
 *
 * dlna/dmp Interfaces
 *
 * @author  dikim, mhpark3
 * @date    2011/12/08
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __OPLRCF_H__
#define __OPLRCF_H__

#include	"bpltype.h"

/******************************************************************
	Typedef
******************************************************************/
typedef	void	(OPL_RUIS_EventCallback)(int event, int size, void *data);

typedef void * OplRUICDeviceHandle;

typedef enum
{
	eRCF_EVENT_ReceiveRequest = 0,
	eRCF_EVENT_ReceiveMulticast,
	
	eRCF_EVENT_RequestCreateApp,
	eRCF_EVENT_RequestDestroyApp,
	eRCF_EVENT_Unknown,
	eRCF_EVENT_Max,
} OplRCF_Event_e;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
};

/******************************************************************
	OplRUIManager Class
******************************************************************/
class OplRUIManager
{
private :
public :
	OplRUIManager();
	virtual ~OplRUIManager();
	
	static	void	onEventNotifier( HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3 );
	static	void	registerEventListener( OPL_RUIS_EventCallback callback );
	static	void	unRegisterEventListener( OPL_RUIS_EventCallback callback );

	// Event Callback List로 만들어서 관리해야함.
	static  OPL_RUIS_EventCallback	*s_pfn_contentSearch_callback;

};

/******************************************************************
	OplRUIManager Class
******************************************************************/
class OplRCFObject
{
private :
public :
	OplRCFObject();
	virtual ~OplRCFObject();
};



#endif // __cplusplus
#endif // __OPLRCF_H__
