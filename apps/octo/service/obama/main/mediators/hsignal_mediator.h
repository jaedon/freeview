/**************************************************************
 *	@file		hsignal_mediator.h
 *	HumaxApplicationMAnager global header for other applications.
 *
 *	http://www.humaxdigital.com
 *
 *	@data
 *	@author			humax
 *	@brief
 **************************************************************/
#ifndef	__HSIGNAL_MEDIATOR_H__
#define	__HSIGNAL_MEDIATOR_H__

#include <htype.h>
#include <hlib.h>

#include <hapi.h>


//Hama Signal mediators
#if	0
typedef struct
{
	HSIGNAL_e	 	eSignal;
	HSIGNALTYPE_e	eSignalType;
	HINT32			size;
	void			*data;
} HSMMsg_t;
#endif

typedef void	(*CBHSMSIGHandler)(HSIGNAL_e eSignal, HSIGNALTYPE_e eSignalType, HINT32 size, void *data);

extern HERROR 	HSMSIG_Main(void);
extern HERROR	HSMSIG_Register(HCHAR *pszModuleName, HUINT32 prio, CBHSMSIGHandler pfnHandler, void *userdata);
extern HERROR	HSMSIG_ConnectSignal(HSIGNAL_e eSignal);
extern HERROR	HSMSIG_SendSignalNumber(HSIGNAL_e eSignal, HUINT32 ulParam);
extern HERROR	HSMSIG_SendSignalString(HSIGNAL_e eSignal, HCHAR *contents);
extern HERROR	HSMSIG_SendSignalBinary(HSIGNAL_e eSignal, HINT32 size, HUINT8 *data);

#endif
