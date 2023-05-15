/**@@@+++@@@@******************************************************************
**
** Microsoft PlayReady
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include <drmcommon.h>
#include <oemcommon.h>
#include <drmtypes.h>
#include <drmutilities.h>

ENTER_PK_NAMESPACE_CODE;


#if PRDY_ROBUSTNESS


DRM_API DRM_VOID * Oem_Create()
{
    DRM_RESULT  dr;
	DRM_VOID  *pOEMContext;

	pOEMContext = BCM_Oem_Create_1st();

    // Read Bin file from file system.
    dr = _ReadBinFile(pOEMContext);
    if ( DRM_FAILED(dr) ){
        printf("%s - _ReadBinFile() failed. Please make sure the pr_bin_filename variable in drm_playready.inc point to the right file\n", __FUNCTION__);
        goto ErrorExit;
    }

	BCM_Oem_Create_2nd(pOEMContext);
	if(pOEMContext == NULL)
    {
		return NULL;
    }

    dr = Oem_Device_SaveRevocationInfo((DRM_VOID *)pOEMContext);
    if ( DRM_FAILED(dr) ){
        goto ErrorExit;
    }
	OEM_TimeInit((DRM_VOID *)pOEMContext) ;
    return (DRM_VOID *)pOEMContext;

ErrorExit:
    BCM_Oem_Destroy(pOEMContext);

    return NULL;
}


DRM_VOID Oem_Destroy(__in_opt DRM_VOID *f_pOEMContext)
{
	OEM_TimeUnInit(f_pOEMContext) ;
    BCM_Oem_Destroy(f_pOEMContext);

    return;
}
#endif

DRM_API DRM_VOID * DRM_CALL Oem_MemHeapAlloc(
    __in DRM_DWORD f_cbSize
)
{
    return BCM_Oem_MemHeapAlloc(f_cbSize);
}

DRM_API DRM_VOID DRM_CALL Oem_MemHeapFree(
    __in DRM_VOID *f_pv
)
{
    BCM_Oem_MemHeapFree(f_pv);
    return;
}

EXIT_PK_NAMESPACE_CODE;
