/**************************************************************************************/
/**
 * @file NativeJlabsDMRController.cpp
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

#include "NativeJlabsDMRController.h"

#include "opldmr.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	__TRACE()					do{OOIF_LOG_DUMMY("[%s:%d] trace\n", __FUNCTION__, __LINE__);}while(0)
#define	__RETURN_ERROR_IF(cond)		do{if(cond){OOIF_LOG_ERROR("[%s:%d] error \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_ERROR;}}while(0)
#define	__RETURN_NULL_IF(cond)		do{if(cond){OOIF_LOG_WARNING("[%s:%d] null \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_NULL;}}while(0)
#define	__RETURN_UNDEFINED_IF(cond)	do{if(cond){OOIF_LOG_WARNING("[%s:%d] undefined \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_UNDEFINED;}}while(0)
#define	__RETURN_OK_IF(cond)		do{if(cond)return OOIF_RETURN_ERROR;}while(0)

OOIFReturnCode  native_jlabsDMRControllerEventListener(DlnaDmrEventListener listener)
{
	OPL_DMR_RegisterListener(listener);
	return OOIF_RETURN_OK;
}

OOIFReturnCode  native_jlabsDMRControllerUnRegEventListener()
{
	OPL_DMR_UnRegisterListener();
	return OOIF_RETURN_OK;
}

/* dmr function start */
OOIFReturnCode  native_jlabsDMRControllerStart(OOIFBoolean *retval)
{
	HBOOL ret = FALSE;

	/* obama base dmr */
	OPL_DMR_SetOperationMode(eOPL_DMR_MODE_OCTO_BASE);

	if(OPL_DMR_Start() == BPL_STATUS_OK)
		ret = TRUE;

	*retval = ret;

	return OOIF_RETURN_OK;
}

/* dmr function stop */
OOIFReturnCode  native_jlabsDMRControllerStop(OOIFBoolean *retval)
{
	HBOOL ret = FALSE;

	if(OPL_DMR_Stop() == BPL_STATUS_OK)
		ret = TRUE;

	*retval = ret;
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_jlabsDMRControllerIsPlay(OOIFBoolean *retval) 
{
	OOIF_LOG_DUMMY("native_jlabsDMRControllerIsPlay!");
	*retval = FALSE;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsDMRControllerPlayingStop(OOIFBoolean *retval) 
{
	OOIF_LOG_DUMMY("native_jlabsDMRControllerPlayingStop!");
	*retval = FALSE;
	return OOIF_RETURN_OK;
}
#endif

