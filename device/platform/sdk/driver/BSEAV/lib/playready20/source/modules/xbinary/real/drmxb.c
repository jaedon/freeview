/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcontextsizes.h>
#include <byteorder.h>
#include <drmxb.h>

ENTER_PK_NAMESPACE_CODE;


/*********************************************************************
**
** Function: _XB_MapObjectTypeToEntryDescriptionIndex
**
** Synopsis: API that searches a format description table for a given object
**           type and returns the index in the table of that type.
**
** Arguments:
**
** [f_pFormat]         -- Pointer to a format description table
** [f_wType]           -- WORD object type that is being searched for
**
** Returns:               Integer index in the format table of the object type.
**
**********************************************************************/
DRM_WORD DRM_CALL _XB_MapObjectTypeToEntryDescriptionIndex(
    IN  const   DRM_XB_FORMAT_DESCRIPTION   *f_pFormat,
    IN          DRM_WORD                     f_wType )
{
    DRM_WORD    iEntry    = 0;

    for( iEntry = 0; iEntry < f_pFormat->cEntryDescriptions; iEntry++ )
    {
        if( f_pFormat->pEntryDescriptions[iEntry].wType == f_wType )
        {
            return iEntry;
        }
    }

    return 0;
}

/*********************************************************************
**
** Function: _XB_IsKnownObjectType
**
** Synopsis: API that searches a format description table for a given object
**           type and returns TRUE if the object type is in the table
**
** Arguments:
**
** [f_pFormat]         -- Pointer to a format description table
** [f_wType]           -- WORD object type that is being searched for
**
** Returns:            TRUE if the object type is found in the description table
**                     FALSE if the object type is NOT found in the description table
**
**********************************************************************/

DRM_NO_INLINE DRM_BOOL DRM_CALL _XB_IsKnownObjectType(
    IN  const   DRM_XB_FORMAT_DESCRIPTION   *f_pFormat,
    IN          DRM_WORD                     f_wType )
{
    DRM_WORD    iEntry    = 0;

    for( iEntry = 0; iEntry < f_pFormat->cEntryDescriptions; iEntry++ )
    {
        if( f_pFormat->pEntryDescriptions[iEntry].wType == f_wType )
        {
            return TRUE;
        }
    }

    return FALSE;
}

EXIT_PK_NAMESPACE_CODE;
