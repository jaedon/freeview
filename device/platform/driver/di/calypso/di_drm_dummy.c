#include "di_drm.h"
#include "di_err.h"
#include "di_media20.h"

#define UNUSED_PARAM(x)	(void)x

DI_ERR_CODE DI_DRM_RegisterOutputControlCallback(SESSION_HANDLE media_session_handle, DI_DRM_OutputControlCallback_t OutputControlCallback)
{
	UNUSED_PARAM(media_session_handle);
	UNUSED_PARAM(OutputControlCallback);
	return DI_ERR_OK;
}
DI_ERR_CODE DI_DRM_UnregisterOutputControlCallback(SESSION_HANDLE media_session_handle)
{
	UNUSED_PARAM(media_session_handle);
	return DI_ERR_OK;
}
