/**
	@file	  port_itk_appmmi.h												\n
	@brief	  itk file for CI+ Application MMI									\n

	Module: MW/mheg	/itk								 						\n

	Copyright (c) 2008 HUMAX Co., Ltd.											\n
	All rights reserved.														\n

	[Description]

*/


#ifndef _PORT_ITK_APMMI_H_
#define _PORT_ITK_APMMI_H_

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "itk_appmmi.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/********************************************************************/
/********************      proto-type functions  ********************/
/********************************************************************/
extern void					PORT_ITK_APPMMI_Init(void);
extern void					PORT_ITK_APPMMI_CreateObject(void);
extern void					PORT_ITK_APPMMI_RegisterObject(void);
extern HERROR				PORT_ITK_APPMMI_NotifyEngineStatus(redkey_mheg_app_state_t status);
extern itk_appmmi_status_t	PORT_ITK_APPMMI_STOP(itk_service_tune_event_t eTuneEvent);

#if defined(CONFIG_DEBUG)
void 					PORT_ITK_APPMMI_DEBUG_ShowStatus(void);
#endif

#endif	/* _PORT_ITK_APMMI_H_ */



