/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_OEMNDR_C
#include <oemndr.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_RESULT DRM_CALL Oem_Ndr_GetSupportedProximityDetectionTypes(
    __out_ecount(1) DRM_DWORD   *f_pdwSupportedProximityDetectionTypes )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pdwSupportedProximityDetectionTypes != NULL );

    *f_pdwSupportedProximityDetectionTypes = DRM_PRND_PROXIMITY_DETECTION_TYPE_TRANSPORT_AGNOSTIC;

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

