/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include "drmcontract.h"

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_BOOL DRM_CALL OEM_ECC_ExpectedErrorCode(
    __in const  DRM_DWORD   f_uiAPIid,
    __in const  DRM_RESULT  f_dr )
{
    UNREFERENCED_PARAMETER( f_uiAPIid );
    UNREFERENCED_PARAMETER( f_dr );
    return TRUE;
}

EXIT_PK_NAMESPACE_CODE;

