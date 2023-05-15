/**************************************************************************************/
/**
 * @file opldms.h
 *
 * dlna/dms Interfaces
 *
 * @author  Sung-Su Yun(ssyun@humaxdigital.com)
 * @date    2013/02/23
 * @attention   Copyright (c) 2013 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __OPLDMS_H__
#define __OPLDMS_H__

#include	"bpltype.h"



/******************************************************************
	Typedef
******************************************************************/
typedef void	(*OPL_DMS_EventCallback)(unsigned int event, unsigned int size, void* pData);


#ifdef __cplusplus
extern "C" {
#endif

BPLStatus	OPL_DMS_Start( void );
BPLStatus	OPL_DMS_Stop( void );
BPLStatus	OPL_DMS_GetActionStatus( HUINT32 *status );
BPLStatus 	OPL_DMS_IsActivated(HBOOL *bEnable);
BPLStatus 	OPL_DMS_GetDmsFriendlyName(HCHAR *friendlyName, int nlen);
BPLStatus 	OPL_DMS_SetDmsFriendlyName(const HCHAR *friendlyName);

BPLStatus	OPL_DMS_RegisterListener( OPL_DMS_EventCallback listener );
BPLStatus	OPL_DMS_UnRegisterListener(void);



#ifdef __cplusplus
};

/******************************************************************
	class 
******************************************************************/



#endif // __cplusplus
#endif // __OPLDMP_H__
