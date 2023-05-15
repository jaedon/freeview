/**************************************************************************************/
/**
 * @file NativeJlabsCDSRecording.cpp
 *
 * jlabsDTTChanne module
 *
 * @author 
 * @date    
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#if defined OIPF && defined JLABS

#include "NativeJlabsCDSSearcher.h"
#include "macros.h"
#include "opldmp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	__TRACE()					do{OOIF_LOG_DUMMY("[%s:%d] trace\n", __FUNCTION__, __LINE__);}while(0)
#define	__RETURN_ERROR_IF(cond)		do{if(cond){OOIF_LOG_ERROR("[%s:%d] error \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_ERROR;}}while(0)
#define	__RETURN_NULL_IF(cond)		do{if(cond){OOIF_LOG_WARNING("[%s:%d] null \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_NULL;}}while(0)
#define	__RETURN_UNDEFINED_IF(cond)	do{if(cond){OOIF_LOG_WARNING("[%s:%d] undefined \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_UNDEFINED;}}while(0)
#define	__RETURN_OK_IF(cond)		do{if(cond)return OOIF_RETURN_ERROR;}while(0)


#define DMP_ACTION_STOP     0
#define DMP_ACTION_START    1

OOIFReturnCode native_jlabsDMPAction( int actionType, OOIFNumber *retval )
{
	switch ( actionType )
	{
		case DMP_ACTION_START :
			OPL_DMP_Start();
			break;
		case DMP_ACTION_STOP :
			OPL_DMP_Stop();
			break;
		default :
			*retval = FALSE;
			return OOIF_RETURN_OK;

	}
	*retval = TRUE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsGetDMSList(unsigned int *nCount, void **Handles)
{
	OplDMSDeviceHandle _dmpDevice;

	if(OPL_DMP_GetListOfDMS( nCount, &_dmpDevice) != BPL_STATUS_OK)
	{
		return OOIF_RETURN_ERROR;
	}

	*Handles = _dmpDevice;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsCDSSearchRegisterEventHandler(OPL_DMP_EventCallback cbFunc )
{
	OPL_DMP_RegisterListener( cbFunc );
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsCDSSearchUnRegisterEventHandler(void)
{
	OPL_DMP_UnRegisterListener();
	return OOIF_RETURN_OK;
}
#endif
