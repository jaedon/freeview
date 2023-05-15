/*
 * $Header:   $
 */
/*********************************************************************
 * $Workfile:   di_drm  $
 * $Modtime:   Mar 16 2012 17:20:04  $
 *
 * Author:
 * Description:
 *
 *                                 Copyright (c) 2012 HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/



/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* include  common utils headers */
#include "htype.h"
#include "linden_trace.h"
#include "vkernel.h"

/* include  drv headers */
#include "drv_err.h"
#include "drv_drm.h"

#include "di_drm.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#ifndef UNUSED_PARAM
#define UNUSED_PARAM(x)  ((void)x)
#endif

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/


/*******************************************************************/
/****************** Static Function Prototypes *********************/
/*******************************************************************/


/*******************************************************************/
/****************** Global Function Prototypes *********************/
/*******************************************************************/
DI_ERR_CODE DI_DRM_CreateSession(SESSION_HANDLE *media_session_handle)
{
	DRV_Error err;
	
	if(*media_session_handle == NULL)
	{
		*media_session_handle = (SESSION_HANDLE *)DD_MEM_Alloc(sizeof(int *));
	}

	err = DRV_DRM_CreateSession(*media_session_handle);
	if(err != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DRM_GetInfo(SESSION_HANDLE media_session_handle, DRM_Info_t *drm_info)
{
	DRV_Error err;
	
	err = DRV_DRM_GetInfo(media_session_handle, drm_info);
	if(err != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DRM_ResigterCallback(SESSION_HANDLE media_session_handle, DRM_Info_t *drm_info)
{
	DRV_Error err;
	
	err = DRV_DRM_ResigterCallback(media_session_handle, drm_info);
	if(err != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DRM_RegisterOutputControlCallback(SESSION_HANDLE media_session_handle, DI_DRM_OutputControlCallback_t OutputControlCallback)
{
	DRV_Error err;
	
	err = DRV_DRM_RegisterOutputControlCallback(media_session_handle, OutputControlCallback);
	if(err != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DRM_UnregisterOutputControlCallback(SESSION_HANDLE media_session_handle)
{
	DRV_Error err;
	
	err = DRV_DRM_UnregisterOutputControlCallback(media_session_handle);
	if(err != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DRM_GetLicense(SESSION_HANDLE media_session_handle)
{
	DRV_Error err;
	
	err = DRV_DRM_GetLicence(media_session_handle);
	if(err != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DRM_GetKey(SESSION_HANDLE media_session_handle, DRM_Key_Info_t *drm_key_info)
{
	DRV_Error err;
	
	err = DRV_DRM_GetKey(media_session_handle, drm_key_info);
	if(err != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DRM_GetDRMStatus(SESSION_HANDLE media_session_handle)
{
	DRV_Error err;
	
	err = DRV_DRM_GetDRMStatus(media_session_handle);
	if(err != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DRM_DestroySession(SESSION_HANDLE media_session_handle)
{
	DRV_Error err;
	
	err = DRV_DRM_DestroySession(media_session_handle);
	if(err != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	if(media_session_handle)
	{
		DD_MEM_Free(media_session_handle);
		media_session_handle = NULL;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DRM_SecureClock(SESSION_HANDLE media_session_handle)
{
#if defined (CONFIG_PLAYREADY_25_BRCM)
	DRV_Error err;

	err = DRV_DRM_SecureClock(media_session_handle);
	if(err != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
#else
	UNUSED_PARAM(media_session_handle);
	return DI_ERR_OK;
#endif
}

DI_ERR_CODE DI_DRM_GetStoredLicense(SESSION_HANDLE media_session_handle)
{
#if defined (CONFIG_PLAYREADY_25_BRCM)
	DRV_Error err;

	err = DRV_DRM_GetStoredLicense(media_session_handle);
	if(err != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
#else
	UNUSED_PARAM(media_session_handle);
	return DI_ERR_OK;
#endif
}

DI_ERR_CODE DI_DRM_PushPumpBuffer(SESSION_HANDLE media_session_handle, void *pvDst, void *pvSrc, HUINT32 unSize, HBOOL bflush)
{
#if defined (CONFIG_PLAYREADY_25_BRCM)
	DRV_Error err;
	err = DRV_DRM_PushPumpBuffer(media_session_handle, pvDst, pvSrc, unSize, bflush);
	if(err != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
#else
	UNUSED_PARAM(media_session_handle);
	UNUSED_PARAM(pvDst);
	UNUSED_PARAM(pvSrc);
	UNUSED_PARAM(unSize);
	UNUSED_PARAM(bflush);
	return DI_ERR_OK;
#endif
}

void* DI_DRM_DataMemAlloc(HUINT32 unSize)
{
	return DRV_DRM_DataMemAlloc(unSize);
}

DI_ERR_CODE DI_DRM_DataMemFree(void *pvBuffer)
{
	DRV_Error err;
	err = DRV_DRM_DataMemFree(pvBuffer);
	if(err != DRV_OK)
        {
                return DI_ERR_ERROR;
}
        return DI_ERR_OK;

}

DI_ERR_CODE DI_DRM_DoDecrypt(SESSION_HANDLE media_session_handle, HUINT8 *pBuff, HINT32 nBuffSize, HUINT8 *pIv, HINT32 nIvSize)
{
	DRV_Error err;
	err = DRV_DRM_DoDecrypt(media_session_handle, pBuff, nBuffSize, pIv, nIvSize);
	if(err != DRV_OK)
    {
            return DI_ERR_ERROR;
    }
    return DI_ERR_OK;
}

