/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMWMDRMOPLREAL_C
#include <drmwmdrm.h>
#include <drmxmlparser.h>
#include <drmutilitieslite.h>
#include <drmwmdrmconstants.h>
#include <drmconstants.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

static const DRM_WORD g_wDefaultMinimumLevel = 100;

/*********************************************************************
**
**  Function:  _ProcessExtensions
**
**  Synopsis:  Process all EXTENSION tags from an output level XML block
**
**  Arguments:
**     [f_pdstrRestrictions] -- String from XML blob that contains the entire RESTRICTIONS section
**     [f_pPlayOPL]          -- Play OPL struct that will be filled in
**     [f_pstack]            -- Stack allocator context that will be used to allocate room for the extensions
**
**  Notes:     Data may be allocated from the stack context.  This data will be returned in the
**             f_pPlayOPL.vopi.rgVop pointer.
**
*********************************************************************/
static DRM_RESULT _ProcessExtensions(
    __in    const DRM_CONST_STRING             *f_pdstrRestrictions,
    __inout       DRM_PLAY_OPL_EX2             *f_pPlayOPL,
    __in          DRM_STACK_ALLOCATOR_CONTEXT  *f_pstack )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrAnalogAudio   = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrExtensionList = EMPTY_DRM_STRING;
    DRM_VIDEO_OUTPUT_PROTECTION_EX *rgVop               = NULL;
    DRM_WORD iExtension  = 0;
    DRM_WORD cExtensions = 0;
    DRM_DWORD dwBase     = 10;

    dr = DRM_XML_GetNode( f_pdstrRestrictions,
                         &g_dstrTagAnalogVideo,
                          NULL,
                          NULL,
                          0,
                          NULL,
                         &dstrAnalogAudio );

    if( DRM_FAILED( dr ) )
    {
        /* There is no Analog Video OPL */
        dr = DRM_SUCCESS;
        goto ErrorExit;
    }

    dr = DRM_XML_GetNode( &dstrAnalogAudio,
                          &g_dstrTagExtensionList,
                           NULL,
                           NULL,
                           0,
                           NULL,
                          &dstrExtensionList );

    if( DRM_FAILED( dr ) )
    {
        /* There is no Analog Video OPL */
        dr = DRM_SUCCESS;
        goto ErrorExit;
    }

    /* Count the number of extensions that exist in the license */
    for( iExtension = 0; DRM_SUCCEEDED( dr ); iExtension++ )
    {
        DRM_CONST_STRING dstrExtension = EMPTY_DRM_STRING;

        dr = DRM_XML_GetNode( &dstrExtensionList,
                              &g_dstrTagExtension,
                               NULL,
                               NULL,
                               iExtension,
                              &dstrExtension,
                               NULL );
    }

    dr = DRM_SUCCESS;
    iExtension--;
    if( iExtension == 0 )
    {
        goto ErrorExit;
    }

    cExtensions = iExtension;
    ChkDR( DRM_STK_Alloc( f_pstack, cExtensions * SIZEOF( DRM_VIDEO_OUTPUT_PROTECTION_EX ), (DRM_VOID**)&rgVop ) );
    ZEROMEM(rgVop, cExtensions * SIZEOF( DRM_VIDEO_OUTPUT_PROTECTION_EX ) );

    /* Populate the data structure with data from the license */
    for( iExtension = 0; iExtension < cExtensions; iExtension++ )
    {
        DRM_CONST_STRING dstrExtension = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrGuid      = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrData      = EMPTY_DRM_STRING;

        ChkDR( DRM_XML_GetNode( &dstrExtensionList,
                                &g_dstrTagExtension,
                                 NULL,
                                 NULL,
                                 iExtension,
                                &dstrExtension,
                                 NULL ) );

        ChkDR( DRM_XML_GetNodeAttribute( &dstrExtension, &g_dstrLabelGuid,  &dstrGuid ) );
        ChkDR( DRM_XML_GetNodeAttribute( &dstrExtension, &g_dstrLabelValue, &dstrData ) );
        rgVop[iExtension].dwVersion = VER_DRM_VIDEO_OUTPUT_PROTECTION;
        ChkDR( DRM_UTL_StringToLittleEndianGuid( &dstrGuid, &rgVop[iExtension].guidId ) );


        /*
         * Check if dstrData is a hex number  (dwBase is 10 by default)
         */
        if (     dstrData.cchString > 1
             &&  ( *(dstrData.pwszString) ) == g_wch0
             && (( *(dstrData.pwszString + 1 ) ) == g_wchx || ( *(dstrData.pwszString + 1 ) ) == g_wchX ) )
        {
            dwBase = 16;
        }

        ChkDR( DRMCRT_WtoDWORD( dstrData.pwszString,
                                dstrData.cchString,
                                dwBase,
                               &rgVop[iExtension].dwConfigData,
                                NULL ) );

    }

    f_pPlayOPL->vopi.dwVersion= VER_DRM_PLAY_OPL;
    f_pPlayOPL->vopi.cEntries = cExtensions;
    f_pPlayOPL->vopi.rgVop    = rgVop;
    rgVop = NULL;

    /* Explicit Digital Audio restrictions not supported in XML licenses */
    f_pPlayOPL->aopi.dwVersion= 0;
    f_pPlayOPL->aopi.cEntries = 0;
    f_pPlayOPL->aopi.rgAop    = NULL;


ErrorExit:
    SAFE_STK_FREE( f_pstack, rgVop );
    return dr;
}

/*********************************************************************
**
**  Function:  _ProcessIdList
**
**  Synopsis:  Finds ID tags in a generic way and populates an ID struct with the enumerated
**             guids.
**
**  Arguments:
**     [f_pdstrRestrictions] -- String from XML blob that contains the entire RESTRICTIONS section
**     [f_pdstrType]         -- XML tag to be searched for in RESTRICTIONS
**     [f_pdstrTypeList]     -- LIST XML tag to be searched for in RESTRICTIONs
**     [f_pOPLIds]           -- A OPL_OUTPUT_IDS structure that will be filled in.
**     [f_pstack]            -- Stack allocator context that will be used to allocate room for the Ids
**
*********************************************************************/
static DRM_RESULT _ProcessIdList(
    __in    const DRM_CONST_STRING             *f_pdstrRestrictions,
    __in    const DRM_CONST_STRING             *f_pdstrType,
    __in    const DRM_CONST_STRING             *f_pdstrTypeList,
    __inout       DRM_OPL_OUTPUT_IDS           *f_pOPLIds,
    __in          DRM_STACK_ALLOCATOR_CONTEXT  *f_pstack )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrIdList = EMPTY_DRM_STRING;
    DRM_GUID *rgIds         = NULL;
    DRM_WORD  iId   = 0;
    DRM_WORD  cIds = 0;

    dr = DRM_XML_GetNode( f_pdstrRestrictions,
                          f_pdstrTypeList,
                          NULL,
                          NULL,
                          0,
                          NULL,
                         &dstrIdList );

    if( DRM_FAILED( dr ) )
    {
        /* There is no Analog Video OPL */
        dr = DRM_SUCCESS;
        goto ErrorExit;
    }

    /* Count the number of extensions that exist in the license */
    for( iId = 0; DRM_SUCCEEDED( dr ); iId++ )
    {
        DRM_CONST_STRING dstrId = EMPTY_DRM_STRING;

        dr = DRM_XML_GetNode( &dstrIdList,
                               f_pdstrType,
                               NULL,
                               NULL,
                               iId,
                              &dstrId,
                               NULL );
    }

    dr = DRM_SUCCESS;
    iId--;

    if( iId == 0 )
    {
        goto ErrorExit;
    }

    cIds = iId;
    ChkDR( DRM_STK_Alloc( f_pstack, cIds * SIZEOF( DRM_GUID ), (DRM_VOID**)&rgIds ) );
    ZEROMEM( rgIds, cIds * SIZEOF( DRM_GUID ) );

    /* Populate the data structure with data from the license */
    for( iId = 0; iId < cIds; iId++ )
    {
        DRM_CONST_STRING dstrId   = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrGuid = EMPTY_DRM_STRING;

        ChkDR( DRM_XML_GetNode( &dstrIdList,
                                 f_pdstrType,
                                 NULL,
                                 NULL,
                                 iId,
                                &dstrId,
                                 NULL ) );

        ChkDR( DRM_XML_GetNodeAttribute( &dstrId, &g_dstrLabelGuid, &dstrGuid ) );
        ChkDR( DRM_UTL_StringToLittleEndianGuid( &dstrGuid, &(rgIds[iId]) ) );
    }

    f_pOPLIds->cIds  = cIds;
    f_pOPLIds->rgIds = rgIds;
    rgIds = NULL;

ErrorExit:
    SAFE_STK_FREE( f_pstack, rgIds );
    return dr;
}

/*********************************************************************
**
**  Function:  DRM_WMDRM_LICOPL_GetOPLLevel
**
**  Synopsis:  Returns the level of a specific category as defined in the XML
**             restriction blob.  If the category doesn't exist the default level
**             is returned.
**
**  Arguments:
**     [f_pdstrRestrictions] -- XML blob that contains the RESTRICTIONS tag.
**                              If not provided the default will be returned
**     [f_pdstrCategory]     -- Category to extract the level from
**
*********************************************************************/
DRM_API DRM_WORD DRM_CALL DRM_WMDRM_LICOPL_GetOPLLevel(
    __in const DRM_CONST_STRING *f_pdstrRestrictions,
    __in const DRM_CONST_STRING *f_pdstrCategory )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrCategoryNode = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrLevel        = EMPTY_DRM_STRING;
    DRM_WORD level = g_wDefaultMinimumLevel;
    DRM_LONG lval  = 0;

    ChkDRMString( f_pdstrCategory );

    if( f_pdstrRestrictions != NULL )
    {

        ChkDR( DRM_XML_GetNode( f_pdstrRestrictions,
                                f_pdstrCategory,
                                NULL,
                                NULL,
                                0,
                            &dstrCategoryNode,
                                NULL ) );

        /* Get the level from the node */

        ChkDR( DRM_XML_GetNodeAttribute( &dstrCategoryNode,
                                         &g_dstrLabelLevel,
                                         &dstrLevel ) );

        ChkDR( DRMCRT_wcsntol( dstrLevel.pwszString, dstrLevel.cchString, &lval ) );
        if( lval > 0 && lval <= (DRM_WORD) MAX_UNSIGNED_TYPE( DRM_WORD ) )
        {
            level = (DRM_WORD)lval;
        }
    }

ErrorExit:
    return level;
}

/*********************************************************************
**
**  Function:  DRM_WMDRM_LICOPL_ProcessExclusions
**
**  Synopsis:  Finds exlude tags in the restrictions XML
**
**  Arguments:
**     [f_pdstrRestrictions] -- String from XML blob that contains the entire RESTRICTIONS section
**     [f_pOPLIds]           -- Id struct that will be populated
**     [f_pstack]            -- Stack allocator context that will be used to allocate room for the Ids
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LICOPL_ProcessExclusions(
    __in    const DRM_CONST_STRING             *f_pdstrRestrictions,
    __inout       DRM_OPL_OUTPUT_IDS           *f_pOPLIds,
    __in          DRM_STACK_ALLOCATOR_CONTEXT  *f_pstack )
{
    return _ProcessIdList( f_pdstrRestrictions,
                          &g_dstrTagExclude,
                          &g_dstrTagExcludeList,
                           f_pOPLIds,
                           f_pstack );
}

/*********************************************************************
**
**  Function:  DRM_WMDRM_LICOPL_ProcessInclusions
**
**  Synopsis:  Finds include tags in the restrictions XML
**
**  Arguments:
**     [f_pdstrRestrictions] -- String from XML blob that contains the entire RESTRICTIONS section
**     [f_pOPLIds]           -- Id struct that will be populated
**     [f_pstack]            -- Stack allocator context that will be used to allocate room for the Ids
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LICOPL_ProcessInclusions(
    __in    const DRM_CONST_STRING             *f_pdstrRestrictions,
    __inout       DRM_OPL_OUTPUT_IDS           *f_pOPLIds,
    __in          DRM_STACK_ALLOCATOR_CONTEXT  *f_pstack )
{
    return _ProcessIdList( f_pdstrRestrictions,
                          &g_dstrTagInclude,
                          &g_dstrTagIncludeList,
                           f_pOPLIds,
                           f_pstack );
}

/*********************************************************************
**
**  Function:  DRM_WMDRM_LICOPL_ProcessPlayOutputLevelData
**
**  Synopsis:
**
**  Arguments:
**     [f_pdstrRestrictions] -- String from XML blob that contains the entire RESTRICTIONS section
**     [f_pPlayOPL]          -- Play OPL struct that will be populated with output level data
**     [f_pstack]            -- Stack allocator context that will be used to allocate room for the Ids
**
**  Notes:  Caller should provide a clean, unused stack context.  Caller shouldn't free data directly from stack
**          context.  Instead just throw entire stack context away when finished with the OPL structures.
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LICOPL_ProcessPlayOutputLevelData(
    __in    const DRM_CONST_STRING             *f_pdstrRestrictions,
    __inout       DRM_PLAY_OPL_EX2             *f_pPlayOPL,
    __in          DRM_STACK_ALLOCATOR_CONTEXT  *f_pstack )
{
    DRM_RESULT dr = DRM_SUCCESS;

    if( f_pdstrRestrictions != NULL )
    {
        ChkDRMString( f_pdstrRestrictions );
    }
    ChkArg( f_pPlayOPL        != NULL
         && f_pstack          != NULL
         && f_pstack->pbStack != NULL
         && f_pstack->cbStack  > 0 );

    ZEROMEM( f_pPlayOPL, SIZEOF( *f_pPlayOPL ) );

    f_pPlayOPL->minOPL.wCompressedDigitalVideo   = DRM_WMDRM_LICOPL_GetOPLLevel( f_pdstrRestrictions, &g_dstrTagCompressedDigitalVideo   );
    f_pPlayOPL->minOPL.wUncompressedDigitalVideo = DRM_WMDRM_LICOPL_GetOPLLevel( f_pdstrRestrictions, &g_dstrTagUncompressedDigitalVideo );
    f_pPlayOPL->minOPL.wAnalogVideo              = DRM_WMDRM_LICOPL_GetOPLLevel( f_pdstrRestrictions, &g_dstrTagAnalogVideo              );
    f_pPlayOPL->minOPL.wCompressedDigitalAudio   = DRM_WMDRM_LICOPL_GetOPLLevel( f_pdstrRestrictions, &g_dstrTagCompressedDigitalAudio   );
    f_pPlayOPL->minOPL.wUncompressedDigitalAudio = DRM_WMDRM_LICOPL_GetOPLLevel( f_pdstrRestrictions, &g_dstrTagUncompressedDigitalAudio );

    if( f_pdstrRestrictions != NULL )
    {
        ChkDR( _ProcessExtensions( f_pdstrRestrictions, f_pPlayOPL, f_pstack ) );
    }
ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LICCOPYOPL_ASD_ExamineOutputLevels(
    __in  const DRM_VIEW_RIGHTS_CONTEXT *f_pcontextLQ,
    __in  const DRM_COPY_OPL            *f_pOutputLevels,
    __out       DRM_DWORD               *f_pdwResult )
{
    DRM_RESULT dr = DRM_S_FALSE;
    DRM_DWORD  i  = 0;

    ChkArg( f_pOutputLevels != NULL );

    /* examine the excluded list first */
    for ( i=0; i<f_pOutputLevels->oplIdExcludes.cIds; i++ )
    {
        if( DRMCRT_memcmp(&(f_pOutputLevels->oplIdExcludes.rgIds[i]), &f_pcontextLQ->deviceGUID, sizeof(DRM_GUID)) == 0 )
        {
            *f_pdwResult |= DRM_LICQUERY_NOT_ENABLED_COPY_OPL_EXCLUDED;
            dr = DRM_S_FALSE;
            goto ErrorExit;
        }
    }

    /* check the OPL level */
    if( f_pOutputLevels->wMinimumCopyLevel <= f_pcontextLQ->dwDeviceCopyOPL )
    {
        dr = DRM_SUCCESS;
        goto ErrorExit;
    }

    /* examine the included list */
    for ( i=0; i<f_pOutputLevels->oplIdIncludes.cIds; i++ )
    {
        if( DRMCRT_memcmp(&(f_pOutputLevels->oplIdIncludes.rgIds[i]), &f_pcontextLQ->deviceGUID, sizeof(DRM_GUID)) == 0 )
        {
            dr = DRM_SUCCESS;
            goto ErrorExit;
        }
    }

    *f_pdwResult |= DRM_LICQUERY_NOT_ENABLED_COPY_OPL_TOO_LOW;
    dr = DRM_S_FALSE;

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LICCOPYOPL_ProcessCopyOutputLevelData(
    __in    const DRM_CONST_STRING             *f_pdstrRestrictions,
    __inout       DRM_COPY_OPL                 *f_pCopyOPL,
    __in          DRM_STACK_ALLOCATOR_CONTEXT  *f_pstack )
{
    DRM_RESULT dr = DRM_SUCCESS;

    if( f_pdstrRestrictions != NULL )
    {
        ChkDRMString( f_pdstrRestrictions );
    }
    ChkArg( f_pCopyOPL        != NULL
         && f_pstack          != NULL
         && f_pstack->pbStack != NULL
         && f_pstack->cbStack  > 0 );

    ZEROMEM( f_pCopyOPL, SIZEOF( *f_pCopyOPL ) );

    f_pCopyOPL->wMinimumCopyLevel = DRM_WMDRM_LICOPL_GetOPLLevel( f_pdstrRestrictions, &g_dstrTagCopy );

    if( f_pdstrRestrictions != NULL )
    {
        /* Process exclusions first because the inclusion processing will expect exlusions to have been done first */
        ChkDR( DRM_WMDRM_LICOPL_ProcessExclusions( f_pdstrRestrictions, &(f_pCopyOPL->oplIdExcludes), f_pstack ) );
        ChkDR( DRM_WMDRM_LICOPL_ProcessInclusions( f_pdstrRestrictions, &(f_pCopyOPL->oplIdIncludes), f_pstack ) );
    }

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

