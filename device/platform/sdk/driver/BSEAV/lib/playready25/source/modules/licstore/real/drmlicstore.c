/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMLICSTORE_C
#include <drmlicstore.h>
#include <drmutilitieslite.h>
#include <drmdeletedstore.h>
#include <drmremovaldatestore.h>
#include <drmwmdrm.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/*************************************************************************
** local constants
**************************************************************************/
#define DRM_LICENSE_STORE_NUM_CHILDREN      16

/* Average size of licenses in the various stores */
#define DRM_XML_LICENSE_STORE_SLOT_SIZE     5100
#define DRM_XMR_LICENSE_STORE_SLOT_SIZE     800
#define DRM_TEMP_LICENSE_STORE_SLOT_SIZE    800

static const DRM_DST_NAMESPACE _namespaceLSTXML =
{
    'L', 'i', 'c', 'S',
    't', 'o', 'r', 'e'
};

static const DRM_DST_NAMESPACE _namespaceLSTXMR =
{
    'L', 'i', 'c', 'S',
    't', 'o', 'r', 'e',
    'x', 'm', 'r', '\0'
};

static const DRM_DST_NAMESPACE _namespaceLSTTemp =
{
    'L', 'i', 'c', 'S',
    't', 'o', 'r', 'e',
    'T', 'e', 'm', 'p'
};

/*************************************************************************
** static functions
**************************************************************************/

/**********************************************************************
** Function:    _LoadPrioritizedList
**
** Synopsis:    Load the prioritized list into the enumeration context
**
** Arguments:
**      [f_pLicEnumContext]   : The License store enumeration context
**      [f_fReverseOrder]     : TRUE specifies loading licenses lowest priority first.
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pLicEnumContext is NULL
**
***********************************************************************/
static DRM_NO_INLINE DRM_RESULT DRM_CALL _LoadPrioritizedList(
    DRM_LICSTOREENUM_CONTEXT *f_pLicEnumContext,
    DRM_BOOL f_fReverseOrder )
{
    DRM_RESULT            dr              = DRM_SUCCESS;
    DRM_BOOL              fInserted       = FALSE;
    DRM_DWORD             i               = 0;
    DRM_DWORD             j               = 0;
    DRM_DWORD             iList           = MAX_PRILIC_LIST;
    DRM_DST_SLOT_CONTEXT *pSlotContext    = &f_pLicEnumContext->SlotContext;
    const DRM_DST        *pDatastore      = NULL;

    ChkArg( f_pLicEnumContext != NULL );
    ChkArg( f_pLicEnumContext->pLicContext != NULL );

    pDatastore = f_pLicEnumContext->pLicContext->pDatastore;

    f_pLicEnumContext->List.Prioritized.cLicenses    = 0;
    f_pLicEnumContext->List.Prioritized.iNextLicense = 0;


    while ( TRUE )
    {
        fInserted    = FALSE;
        iList        = MAX_PRILIC_LIST;

        if( f_pLicEnumContext->List.Prioritized.cLicenses == 0 )
        {
            iList = 0;
        }

        dr = DRM_DST_SlotEnumNext( pDatastore,
                                   &f_pLicEnumContext->DstEnumContext,
                                   pSlotContext,
                                   &f_pLicEnumContext->List.Prioritized.KID,
                                   &f_pLicEnumContext->List.Prioritized.rgPrioritizedList[iList].LID,
                                   NULL );

        if ( dr == DRM_E_NOMORE )
        {
            dr = DRM_SUCCESS;
            goto ErrorExit;
        }
        ChkDR( dr );

        ChkDR( DRM_DST_MakeSlotHint( pDatastore,
                                     pSlotContext,
                                     &( f_pLicEnumContext->List.Prioritized.rgPrioritizedList[iList].slotHint ) ) );

        /*
        ** load the license's priority
        */
        ChkDR( DRM_DST_SlotRead( pDatastore,
                                 &f_pLicEnumContext->SlotContext,
                                 SIZEOF( DRM_DWORD ),
                    ( DRM_BYTE* )&( f_pLicEnumContext->List.Prioritized.rgPrioritizedList[iList].dwPriority ),
                                 NULL ) );
        FIX_ENDIAN_DWORD( f_pLicEnumContext->List.Prioritized.rgPrioritizedList[iList].dwPriority );
        ChkDR( DRM_DST_CloseSlot( pDatastore, &f_pLicEnumContext->SlotContext ) );

        /*
        ** First license
        */
        if ( f_pLicEnumContext->List.Prioritized.cLicenses == 0 )
        {
            f_pLicEnumContext->List.Prioritized.cLicenses++;
        }
        else
        {
            /*
            ** Merge this license to the list
            */
            for ( i = 0; i < f_pLicEnumContext->List.Prioritized.cLicenses; i++ )
            {
                DRM_BOOL fInsertLicense = FALSE;

                if ( f_fReverseOrder )
                {
                    fInsertLicense = f_pLicEnumContext->List.Prioritized.rgPrioritizedList[MAX_PRILIC_LIST].dwPriority <
                                     f_pLicEnumContext->List.Prioritized.rgPrioritizedList[i].dwPriority;
                }
                else
                {
                    fInsertLicense = f_pLicEnumContext->List.Prioritized.rgPrioritizedList[MAX_PRILIC_LIST].dwPriority >
                                     f_pLicEnumContext->List.Prioritized.rgPrioritizedList[i].dwPriority;
                }

                if ( fInsertLicense )
                {
                    /*
                    ** bump the licenses from i to rest and insert the new license to pos i
                    */
                    if ( f_pLicEnumContext->List.Prioritized.cLicenses == MAX_PRILIC_LIST )
                    {
                        j = f_pLicEnumContext->List.Prioritized.cLicenses - 1;
                    }
                    else
                    {
                        j = f_pLicEnumContext->List.Prioritized.cLicenses;
                    }

                    for ( ; j > i; j-- )
                    {
                        MEMCPY( &f_pLicEnumContext->List.Prioritized.rgPrioritizedList[j],
                                &f_pLicEnumContext->List.Prioritized.rgPrioritizedList[j - 1],
                                 SIZEOF( DRM_PRIORITIZED_LICENSE ) );
                    }

                    MEMCPY( &f_pLicEnumContext->List.Prioritized.rgPrioritizedList[i],
                            &f_pLicEnumContext->List.Prioritized.rgPrioritizedList[MAX_PRILIC_LIST],
                             SIZEOF( DRM_PRIORITIZED_LICENSE ) );

                    if ( f_pLicEnumContext->List.Prioritized.cLicenses < MAX_PRILIC_LIST )
                    {
                        ++f_pLicEnumContext->List.Prioritized.cLicenses;
                    }

                    fInserted = TRUE;
                    break;
                }
            }

            if ( !fInserted && f_pLicEnumContext->List.Prioritized.cLicenses < MAX_PRILIC_LIST )
            {
                MEMCPY( &f_pLicEnumContext->List.Prioritized.rgPrioritizedList[f_pLicEnumContext->List.Prioritized.cLicenses],
                        &f_pLicEnumContext->List.Prioritized.rgPrioritizedList[MAX_PRILIC_LIST],
                         SIZEOF( DRM_PRIORITIZED_LICENSE ) );

                ++f_pLicEnumContext->List.Prioritized.cLicenses;
            }
        }
    }

ErrorExit:
    return dr;
}

/**************************************************************************
** API functions
* *************************************************************************/

/**********************************************************************
** Function:    DRM_LST_Open
**
** Synopsis:    Initialize the license store for the given license store type.
**
** Arguments:
**      [f_pLicContext]       : The License store context to initialize
**      [f_pDatastore]        : Specifies the underlying data store to use.
**      [f_eLicenseStoreType] : Specifies the type of license store to initialize
**                              ( XML or XMR )
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the pointer arguments was NULL or the license store type
**          is invalid
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LST_Open(
    __out DRM_LICSTORE_CONTEXT    *f_pLicContext,
    __in  DRM_DST                 *f_pDatastore,
    __in  eDRM_LICENSE_STORE_TYPE  f_eLicenseStoreType )
{
    DRM_RESULT   dr  = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMLICSTORE, PERF_FUNC_DRM_LST_Open );

    ChkArg( f_pLicContext != NULL );
    ChkArg( f_pDatastore  != NULL );

    /*
    ** open the namespace for the LicenseStore
    */
    if ( f_eLicenseStoreType == eDRM_LICENSE_STORE_XML )
    {
        ChkDR( DRM_DST_OpenNamespace( f_pDatastore,
                                      &_namespaceLSTXML,
                                      (DRM_DST_OPENMODE)(eDRM_DST_CREATE_NEW | eDRM_DST_LOCKWAIT),
                                      DRM_LICENSE_STORE_NUM_CHILDREN,
                                      DRM_XML_LICENSE_STORE_SLOT_SIZE,
                                      &f_pLicContext->DstNamespaceContext ) );
    }
    else if ( f_eLicenseStoreType == eDRM_LICENSE_STORE_TEMP )
    {
        ChkDR( DRM_DST_OpenNamespace( f_pDatastore,
                                      &_namespaceLSTTemp,
                                      (DRM_DST_OPENMODE)(eDRM_DST_CREATE_NEW | eDRM_DST_LOCKWAIT),
                                      DRM_LICENSE_STORE_NUM_CHILDREN,
                                      DRM_TEMP_LICENSE_STORE_SLOT_SIZE,
                                      &f_pLicContext->DstNamespaceContext ) );
    }
    else
    {
        ChkArg( f_eLicenseStoreType == eDRM_LICENSE_STORE_XMR );
        ChkDR( DRM_DST_OpenNamespace( f_pDatastore,
                                      &_namespaceLSTXMR,
                                      (DRM_DST_OPENMODE)(eDRM_DST_CREATE_NEW | eDRM_DST_LOCKWAIT),
                                      DRM_LICENSE_STORE_NUM_CHILDREN,
                                      DRM_XMR_LICENSE_STORE_SLOT_SIZE,
                                      &f_pLicContext->DstNamespaceContext ) );
    }

    f_pLicContext->eType = f_eLicenseStoreType;
    f_pLicContext->fInited = TRUE;
    f_pLicContext->pDatastore = f_pDatastore;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}




/**********************************************************************
** Function:    DRM_LST_Close
**
** Synopsis:    Close the license store.  This will also Commit before close
**
** Arguments:
**      [f_pLicContext] : The License store context to close
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pLicContext was NULL or not inited
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LST_Close(
    __in DRM_LICSTORE_CONTEXT *f_pLicContext )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMLICSTORE, PERF_FUNC_DRM_LST_Close );

    ChkArg( f_pLicContext != NULL );
    if( !f_pLicContext->fInited )
    {
        /* Avoid ChkArg tracing for a common scenario. */
        dr = DRM_E_INVALIDARG;
        goto ErrorExit;
    }

    ChkDR( DRM_DST_CloseNamespace( f_pLicContext->pDatastore, &f_pLicContext->DstNamespaceContext ) );
    f_pLicContext->pDatastore = NULL;
    f_pLicContext->fInited = FALSE;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/**********************************************************************
** Function:    DRM_LST_AddLicense
**
** Synopsis:    Add a license to the store
**
** Arguments:
**      [f_pLicContext] : The License store context
**      [f_cbLicense]   : The length of the license buffer
**      [f_pbLicense]   : Buffer containing the license string
**      [f_pKID]        : Pointer to the KID
**      [f_pLID]        : Pointer to the LID
**      [dwPriority]    : The priority to give the license
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the required pointer parameters was NULL or the context
**          was not properly initialized
**      DRM_E_ARITHMETIC_OVERFLOW
**          An arithmetic overflow occurred during processing
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LST_AddLicense(
    __inout                          DRM_LICSTORE_CONTEXT *f_pLicContext,
    __in                             DRM_DWORD             f_cbLicense,
    __in_bcount( f_cbLicense )       DRM_BYTE             *f_pbLicense,
    __in_opt                   const DRM_KID              *f_pKID,
    __in_opt                   const DRM_LID              *f_pLID,
    __in                             DRM_DWORD             f_dwPriority )
{
    DRM_RESULT        dr             = DRM_SUCCESS;
    DRM_CONST_STRING  dstrLicense    = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrValue      = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrLIDData    = EMPTY_DRM_STRING;
    DRM_DST_KEY       KeyKID         = { 0 };
    DRM_DST_KEY       KeyLID         = { 0 };
    DRM_DWORD         dwPriorityTemp = 0;
    DRM_DWORD         dwSize         = f_cbLicense + SIZEOF( DRM_DWORD );
    DRM_BOOL          fCompatWrite   = FALSE;

    ChkArg( f_pLicContext != NULL );
    ChkArg( f_pbLicense   != NULL );
    ChkArg( f_cbLicense   !=0 );
    ChkArg( f_pLicContext->fInited );

    ChkOverflow( f_cbLicense + SIZEOF( DRM_DWORD ), f_cbLicense );

    dwPriorityTemp = f_dwPriority;

    if( f_pKID == NULL || f_pLID == NULL )
    {
        /*
        ** To support older clients that just pass in a license buffer with no
        ** preparsed data we can extract it.  The preferred method is for the caller
        ** to pass in this data
        */
        fCompatWrite = TRUE;
        DSTR_FROM_PB( &dstrLicense, f_pbLicense, f_cbLicense );

        ChkDR( DRM_WMDRM_LIC_GetAttribute( &dstrLicense,
                                            NULL,
                                            DRM_LICENSE_ATTRIB_KID,
                                           &dstrLIDData,
                                           &dstrValue,
                                            0 ) );
        ChkDR( DRM_UTL_DecodeKID( &dstrValue, &KeyKID ) );

        ChkDR( DRM_WMDRM_LIC_GetAttribute( &dstrLicense,
                                            NULL,
                                            DRM_LICENSE_ATTRIB_LID,
                                           &dstrLIDData,
                                           &dstrValue,
                                            0 ) );
        ChkDR( DRM_UTL_StringToLittleEndianGuid( &dstrValue, ( DRM_GUID * )( KeyLID.rgb ) ) );
        ChkDR( DRM_WMDRM_LIC_GetAttribute( &dstrLicense,
                                            NULL,
                                            DRM_LICENSE_ATTRIB_PRIORITY,
                                           &dstrLIDData,
                                           &dstrValue,
                                            0 ) );
        ChkDR( DRMCRT_wcsntol( dstrValue.pwszString,
                               dstrValue.cchString,
                 ( DRM_LONG * )&dwPriorityTemp ) );
        f_pKID = &KeyKID;
        f_pLID = &KeyLID;
    }

    /*
    ** allocate a slot in the license store
    */
    ChkDR( DRM_DST_OpenSlot( f_pLicContext->pDatastore,
                             &f_pLicContext->DstNamespaceContext,
                             eDRM_DST_CREATE_NEW | eDRM_DST_LOCKEXCLUSIVE | eDRM_DST_LOCKWAIT,
                             f_pKID,
                             f_pLID,
                             NULL,
                             &dwSize,
                             &f_pLicContext->SlotContext ) );

    FIX_ENDIAN_DWORD( dwPriorityTemp );

    if( fCompatWrite )
    {
        ChkDR( DRM_DST_SlotWrite( f_pLicContext->pDatastore,
                                  &f_pLicContext->SlotContext,
                                  SIZEOF( DRM_DWORD ),
                                  ( DRM_BYTE * )&dwPriorityTemp,
                                  NULL ) );
        ChkDR( DRM_DST_SlotWrite( f_pLicContext->pDatastore,
                                  &f_pLicContext->SlotContext,
                                  f_cbLicense,
                                  f_pbLicense,
                                  NULL ) );
    }
    else
    {
        ChkBOOL( f_cbLicense >= SIZEOF( DRM_DWORD ), DRM_E_INVALIDARG );

        MEMCPY( f_pbLicense, &dwPriorityTemp, SIZEOF( DRM_DWORD ) );
        ChkDR( DRM_DST_SlotWrite( f_pLicContext->pDatastore,
                                  &f_pLicContext->SlotContext,
                                  dwSize,
                                  f_pbLicense,
                                  NULL ) );
    }

    ChkDR( DRM_DST_CloseSlot( f_pLicContext->pDatastore, &f_pLicContext->SlotContext ) );

#if !_DATASTORE_WRITE_THRU || _ADDLICENSE_WRITE_THRU
    /*
    ** Some devices defer file flushes until the user is idle.
    ** If _ADDLICENSE_WRITE_THRU is set, then commit the changes
    ** right now so that they will be immediately flushed to the file.
    ** This ensures that a license is immediately available for use.
    */
    ChkDR( DRM_DST_CommitNamespace( f_pLicContext->pDatastore,
                                    &f_pLicContext->DstNamespaceContext ) );
#endif /* !_DATASTORE_WRITE_THRU || _ADDLICENSE_WRITE_THRU */

ErrorExit:
    /* remap the slot exists to a more meaningful user message */
    if( dr == DRM_E_DST_SLOT_EXISTS )
    {
        dr = DRM_E_DUPLICATE_LICENSE;
    }

    return dr;
}



/**********************************************************************
** Function:    DRM_LST_GetLicense
**
** Synopsis:    Get a license from the store using KID and LID
**
** Arguments:
**      [f_pLicContext] : The License store context
**      [f_pKID]        : Pointer to the KID
**      [f_pLID]        : Pointer to the LID
**      [f_pSlotHint]   : DST slot hint
**      [f_pbLicense]   : Buffer to return the license
**      [f_cbLicense]   : Specified the size of the license buffer in bytes,
**                        and returns the size of the license retrieved.
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the required pointer parameters was NULL or the context
**          was not properly initialized
**      DRM_E_BUFFERTOOSMALL
**          The license buffer provided is not big enough to contain the license
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LST_GetLicense(
    __in                                    DRM_LICSTORE_CONTEXT *f_pLicContext,
    __in                              const DRM_KID              *f_pKID,
    __in                              const DRM_LID              *f_pLID,
    __in_opt                                DRM_DST_SLOT_HINT    *f_pSlotHint,
    __out_bcount_opt( *f_pcbLicense )       DRM_BYTE             *f_pbLicense,
    __inout                                 DRM_DWORD            *f_pcbLicense )
{
    DRM_RESULT dr         = DRM_SUCCESS;
    DRM_DWORD  cbSlotSize = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMLICSTORE, PERF_FUNC_DRM_LST_GetLicense );

    ChkArg( f_pLicContext != NULL );
    ChkArg( f_pcbLicense  != NULL );
    ChkArg( f_pKID        != NULL );
    ChkArg( f_pLID        != NULL );

    /*
    ** get size of SlotContext
    */
    dr = DRM_DST_OpenSlot( f_pLicContext->pDatastore,
                           &f_pLicContext->DstNamespaceContext,
                           eDRM_DST_OPEN_EXISTING | eDRM_DST_LOCKWAIT,
                           f_pKID,
                           f_pLID,
                           f_pSlotHint,
                           &cbSlotSize,
                           &f_pLicContext->SlotContext );

    /*
    ** license has been changed or removed
    */
    if( ( dr == DRM_E_DST_BLOCK_MISMATCH ) || ( dr == DRM_E_DST_SLOT_NOT_FOUND ) )
    {
        MapDR( dr, DRM_E_LICENSE_NOT_FOUND );
    }
    ChkDR( dr );

    /*
    ** check size of license buffer
    */
    if ( *f_pcbLicense < ( cbSlotSize - SIZEOF( DRM_DWORD ) ) )
    {
        *f_pcbLicense = ( cbSlotSize - SIZEOF( DRM_DWORD ) );

        ChkDR( DRM_DST_CloseSlot( f_pLicContext->pDatastore, &f_pLicContext->SlotContext ) );
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /*
    ** read the license
    */
    ChkDR( DRM_DST_SlotSeek( f_pLicContext->pDatastore,
                             &f_pLicContext->SlotContext,
                             SIZEOF( DRM_DWORD ),
                             eDRM_DST_SEEKSET,
                             NULL ) );
    ChkArg( f_pbLicense != NULL );
    ChkDR( DRM_DST_SlotRead( f_pLicContext->pDatastore,
                             &f_pLicContext->SlotContext,
                             cbSlotSize,
                             f_pbLicense,
                             f_pcbLicense ) );
    ChkDR( DRM_DST_CloseSlot( f_pLicContext->pDatastore, &f_pLicContext->SlotContext ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_LST_GetLicensePtr(
    __in                                    DRM_LICSTORE_CONTEXT *f_pLicContext,
    __in                              const DRM_KID              *f_pKID,
    __in                              const DRM_LID              *f_pLID,
    __in_opt                                DRM_DST_SLOT_HINT    *f_pSlotHint,
    __deref_out_bcount( *f_pcbLicense )     DRM_BYTE            **f_ppbLicense,
    __out                                   DRM_DWORD            *f_pcbLicense,
    __out_opt                               DRM_DWORD            *f_pdwPriority )
{
    DRM_RESULT dr         = DRM_SUCCESS;
    DRM_DWORD  cbSlotSize = 0;
    DRM_DWORD  dwPriority = 0;
    DRM_DWORD  cbRead     = 0;

    ChkArg( f_pLicContext != NULL );
    ChkArg( f_pKID        != NULL );
    ChkArg( f_pLID        != NULL );
    ChkArg( f_ppbLicense  != NULL );
    ChkArg( f_pcbLicense  != NULL );

    /*
    ** get size of SlotContext
    */
    dr = DRM_DST_OpenSlot( f_pLicContext->pDatastore,
                           &f_pLicContext->DstNamespaceContext,
                           eDRM_DST_OPEN_EXISTING | eDRM_DST_LOCKWAIT,
                           f_pKID,
                           f_pLID,
                           f_pSlotHint,
                           &cbSlotSize,
                           &f_pLicContext->SlotContext );

    /*
    ** license has been changed or removed
    */
    if( ( dr == DRM_E_DST_BLOCK_MISMATCH ) || ( dr == DRM_E_DST_SLOT_NOT_FOUND ) )
    {
        MapDR( dr, DRM_E_LICENSE_NOT_FOUND );
    }
    ChkDR( dr );

    /*
    ** read the license priority
    */
    ChkDR( DRM_DST_SlotRead( f_pLicContext->pDatastore,
                             &f_pLicContext->SlotContext,
                             SIZEOF( DRM_DWORD ),
                             (DRM_BYTE*)&dwPriority,
                             &cbRead ) );
    DRMASSERT( cbRead == SIZEOF( DRM_DWORD ) );

    /*
    ** read the license pointer
    */
    ChkDR( DRM_DST_SlotReadPtr( f_pLicContext->pDatastore,
                                &f_pLicContext->SlotContext,
                                f_ppbLicense,
                                f_pcbLicense ) );
    DRMASSERT( *f_pcbLicense == ( cbSlotSize - SIZEOF( DRM_DWORD ) ) );
    ChkDR( DRM_DST_CloseSlot( f_pLicContext->pDatastore, &f_pLicContext->SlotContext ) );

    if( f_pdwPriority != NULL )
    {
        *f_pdwPriority = dwPriority;
    }

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    _InitEnum
**
** Synopsis:    Initialize an un-indexed enumeration of the license store
**
** Arguments:
**      [f_pLicContext]     : The License store context
**      [f_pKID]            : KID filter to be used.
**                            If not specified, all license will be enumerated
**      [f_fPrioritized]    : Ignored if pKID is NULL.
**                            TRUE to prioritize returned licenses,
**                            FALSE to enumerate in nature order
**      [f_fReversedOrder]  : Ignored if pKID is NULL or f_fPrioritized is FALSE.
**                            TRUE to enumerate licenses with lowest priority first.
**      [f_pLicEnumContext] : Returns the initialized context
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the required pointer parameters was NULL or the context
**          was not properly initialized
**
***********************************************************************/
static DRM_NO_INLINE DRM_RESULT DRM_CALL _InitEnum(
    __in           DRM_LICSTORE_CONTEXT     *f_pLicContext,
    __in_opt const DRM_KID                  *f_pKID,
    __in           DRM_BOOL                  f_fPrioritized,
    __in           DRM_BOOL                  f_fReversedOrder,
    __out          DRM_LICSTOREENUM_CONTEXT *f_pLicEnumContext )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMLICSTORE, PERF_FUNC__InitEnum );

    ChkArg( f_pLicContext     != NULL );
    ChkArg( f_pLicEnumContext != NULL );
    ChkArg( f_pLicContext->fInited );

    ZEROMEM( f_pLicEnumContext, SIZEOF( DRM_LICSTOREENUM_CONTEXT ) );
    f_pLicEnumContext->pLicContext = f_pLicContext;

    dr = DRM_DST_InitSlotEnum( f_pLicContext->pDatastore,
                               &f_pLicContext->DstNamespaceContext,
                               f_pKID,
                               (DRM_DST_LOCKMODE)(eDRM_DST_LOCKSHARED | eDRM_DST_LOCKWAIT),
                               &f_pLicEnumContext->DstEnumContext );

    /*
    ** No licenses found.
    */
    if( dr == DRM_E_DST_SLOT_NOT_FOUND )
    {
        f_pLicEnumContext->eMode = eDRM_LICENSE_STORE_MODE_NONE;
        f_pLicEnumContext->List.Prioritized.cLicenses = 0;
        f_pLicEnumContext->List.Prioritized.iNextLicense = 0;
        dr = DRM_SUCCESS;
    }
    else
    {
        ChkDR( dr );

        if ( f_pKID != NULL )
        {
            MEMCPY( f_pLicEnumContext->List.NonPrioritized.CurrKID.rgb, f_pKID->rgb, DRM_ID_SIZE );

            if ( f_fPrioritized )
            {
                MEMCPY( f_pLicEnumContext->List.Prioritized.KID.rgb, f_pKID->rgb, DRM_ID_SIZE );
                ChkDR( _LoadPrioritizedList( f_pLicEnumContext, f_fReversedOrder ) );
                f_pLicEnumContext->eMode = f_fReversedOrder? eDRM_LICENSE_STORE_MODE_REVERSE_PRIORITIZED : eDRM_LICENSE_STORE_MODE_PRIORITIZED;
            }
            else
            {
                f_pLicEnumContext->eMode = eDRM_LICENSE_STORE_MODE_FILTERED;
                f_pLicEnumContext->List.NonPrioritized.fCurrIsValid = FALSE;
            }
        }
        else
        {
            f_pLicEnumContext->eMode = eDRM_LICENSE_STORE_MODE_NATURAL;
            f_pLicEnumContext->List.NonPrioritized.fCurrIsValid = FALSE;
        }
    }
    f_pLicEnumContext->fInited = TRUE;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**********************************************************************
** Function:    DRM_LST_InitEnum
**
** Synopsis:    Initialize an un-indexed enumeration of the license store
**
** Arguments:
**      [f_pLicContext]     : The License store context
**      [f_pKID]            : KID filter to be used.
**                            If not specified, all license will be enumerated
**      [f_fPrioritized]    : Ignored if pKID is NULL.
**                            TRUE to prioritize returned licenses,
**                            FALSE to enumerate in nature order
**      [f_fReversedOrder]  : Ignored if pKID is NULL or f_fPrioritized is FALSE.
**                            TRUE to enumerate licenses with lowest priority first.
**      [f_pLicEnumContext] : Returns the initialized context
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the required pointer parameters was NULL or the context
**          was not properly initialized
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LST_InitEnum(
    __in            DRM_LICSTORE_CONTEXT     *f_pLicContext,
    __in_opt  const DRM_KID                  *f_pKID,
    __in            DRM_BOOL                  f_fPrioritized,
    __out           DRM_LICSTOREENUM_CONTEXT *f_pLicEnumContext )
{
    return _InitEnum( f_pLicContext, f_pKID, f_fPrioritized, FALSE, f_pLicEnumContext );
}

/**********************************************************************
** Function:    DRM_LST_InitReverseEnum
**
** Synopsis:    Initialize an un-indexed enumeration of the license store in reverse
**              priority order.
**
** Arguments:
**      [f_pLicContext]     : The License store context
**      [f_pKID]            : KID filter to be used.
**      [f_pLicEnumContext] : Returns the initialized context
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the required pointer parameters was NULL or the context
**          was not properly initialized
**
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_LST_InitReverseEnum(
    __in        DRM_LICSTORE_CONTEXT     *f_pLicContext,
    __in  const DRM_KID                  *f_pKID,
    __out       DRM_LICSTOREENUM_CONTEXT *f_pLicEnumContext )
{
    return _InitEnum( f_pLicContext, f_pKID, TRUE, TRUE, f_pLicEnumContext );
}

/**********************************************************************
** Function:    DRM_LST_EnumNext
**
** Synopsis:    Retrieve the next license from the enumerated license store
**
** Arguments:
**      [f_pLicEnumContext] : The License store enumeration context
**      [f_pKID]            : Returns the KID, if using prioritized or filtered mode.
**      [f_pLID]            : Returns the LID.
**      [f_pSlotHint]       : Returns the DST slot hint
**      [f_pcbLicense]      : Returns the size of the license
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the required pointer parameters was NULL or the context
**          was not properly initialized
**      DRM_E_LICENSE_NOT_FOUND
**          Failed to retrieve the next license
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LST_EnumNext(
    __in      DRM_LICSTOREENUM_CONTEXT *f_pLicEnumContext,
    __out_opt DRM_KID                 *f_pKID,
    __out     DRM_LID                 *f_pLID,
    __out_opt DRM_DST_SLOT_HINT       *f_pSlotHint,
    __out_opt DRM_DWORD               *f_pcbLicense )
{
    DRM_RESULT     dr            = DRM_SUCCESS;
    DRM_DWORD      dwSlot        = 0;
    DRM_BOOL       fKeepLooking  = TRUE;
    DRM_DWORD      i             = 0;
    const DRM_DST *pDatastore    = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMLICSTORE, PERF_FUNC_DRM_LST_EnumNext );

    ChkArg( f_pLicEnumContext != NULL );
    ChkArg( f_pLID            != NULL );
    ChkArg( f_pLicEnumContext->fInited );

    pDatastore = f_pLicEnumContext->pLicContext->pDatastore;

    ChkBOOL( f_pLicEnumContext->eMode != eDRM_LICENSE_STORE_MODE_NONE, DRM_E_NOMORE );

    if ( f_pcbLicense != NULL )
    {
        *f_pcbLicense = 0;
    }

    if ( f_pLicEnumContext->eMode == eDRM_LICENSE_STORE_MODE_PRIORITIZED
      || f_pLicEnumContext->eMode == eDRM_LICENSE_STORE_MODE_REVERSE_PRIORITIZED )
    {
        if ( f_pLicEnumContext->List.Prioritized.iNextLicense
          == f_pLicEnumContext->List.Prioritized.cLicenses ) /* end of list reached */
        {
            /*
            ** end of prioritized list. start to enumerate in non-prioritized way
            ** if we have filled our priority list
            */
            if( f_pLicEnumContext->List.Prioritized.cLicenses < MAX_PRILIC_LIST )
            {
                /* If we didn't fill the prioritized list then we have no more licenses to find */
                ChkDR( DRM_E_NOMORE );
            }

            f_pLicEnumContext->eMode = eDRM_LICENSE_STORE_MODE_POST_PRIORITIZED;
            dr = DRM_DST_InitSlotEnum( pDatastore,
                                       &f_pLicEnumContext->pLicContext->DstNamespaceContext,
                                       &f_pLicEnumContext->List.NonPrioritized.CurrKID,
                                       (DRM_DST_LOCKMODE)(eDRM_DST_LOCKSHARED | eDRM_DST_LOCKWAIT),
                                       &f_pLicEnumContext->DstEnumContext );
        }
        else
        {
            /*
            ** get license size
            */
            if ( f_pcbLicense != NULL )
            {
                dr = DRM_LST_GetLicense( ( DRM_LICSTORE_CONTEXT * )f_pLicEnumContext->pLicContext,
                                        &f_pLicEnumContext->List.Prioritized.KID,
                                        &f_pLicEnumContext->List.Prioritized.rgPrioritizedList[f_pLicEnumContext->List.Prioritized.iNextLicense].LID,
                                        &f_pLicEnumContext->List.Prioritized.rgPrioritizedList[f_pLicEnumContext->List.Prioritized.iNextLicense].slotHint,
                                         NULL,
                                         f_pcbLicense );

                /*
                ** DRM_E_BUFFERTOOSMALL is not an error here
                */
                if( dr == DRM_E_BUFFERTOOSMALL )
                {
                    dr = DRM_SUCCESS;
                }
                else
                {
                    ChkDR( dr );
                }
            }
            if ( f_pKID != NULL )
            {
                MEMCPY( f_pKID->rgb,
                        f_pLicEnumContext->List.Prioritized.KID.rgb,
                        DRM_ID_SIZE );
            }
            if ( f_pSlotHint != NULL )
            {
                MEMCPY( f_pSlotHint,
                       &( f_pLicEnumContext->List.Prioritized.rgPrioritizedList[f_pLicEnumContext->List.Prioritized.iNextLicense].slotHint ),
                        SIZEOF( DRM_DST_SLOT_HINT ) );
            }

            MEMCPY( f_pLID->rgb,
                    f_pLicEnumContext->List.Prioritized.rgPrioritizedList[f_pLicEnumContext->List.Prioritized.iNextLicense].LID.rgb,
                    DRM_ID_SIZE );
            f_pLicEnumContext->List.Prioritized.iNextLicense++;
        }
    }

    /*
    ** Not using prioritized list or the prioritized list is exhausted
    */
    if ( f_pLicEnumContext->eMode != eDRM_LICENSE_STORE_MODE_PRIORITIZED
      && f_pLicEnumContext->eMode != eDRM_LICENSE_STORE_MODE_REVERSE_PRIORITIZED )
    {
        f_pLicEnumContext->List.NonPrioritized.fCurrIsValid = FALSE;

        ChkArg( f_pLicEnumContext->eMode != eDRM_LICENSE_STORE_MODE_NATURAL
             || f_pKID != NULL );

        while ( fKeepLooking )
        {
            dr = DRM_DST_SlotEnumNext( pDatastore,
                                       &f_pLicEnumContext->DstEnumContext,
                                       &f_pLicEnumContext->SlotContext,
                                       &f_pLicEnumContext->List.NonPrioritized.CurrKID,
                                       &f_pLicEnumContext->List.NonPrioritized.CurrLID,
                                       &dwSlot );

            /*
            ** license has been changed or removed
            */
            if( ( dr == DRM_E_DST_BLOCK_MISMATCH ) || ( dr == DRM_E_DST_SLOT_NOT_FOUND ) )
            {
                MapDR( dr, DRM_E_LICENSE_NOT_FOUND );
            }
            ChkDR( dr );

            MEMCPY( f_pLID->rgb, f_pLicEnumContext->List.NonPrioritized.CurrLID.rgb, DRM_ID_SIZE );

            if ( f_pKID != NULL )
            {
                MEMCPY( f_pKID->rgb, f_pLicEnumContext->List.NonPrioritized.CurrKID.rgb, DRM_ID_SIZE );
            }

            f_pLicEnumContext->List.NonPrioritized.fCurrIsValid = TRUE;

            if ( f_pcbLicense != NULL )
            {
                *f_pcbLicense = dwSlot - SIZEOF( DRM_DWORD );
            }

            if ( f_pSlotHint != NULL )
            {
                DRM_DST_MakeSlotHint( pDatastore, &f_pLicEnumContext->SlotContext, f_pSlotHint );
            }

            ChkDR( DRM_DST_CloseSlot( pDatastore, &f_pLicEnumContext->SlotContext ) );

            fKeepLooking = FALSE;
            if ( f_pLicEnumContext->eMode == eDRM_LICENSE_STORE_MODE_POST_PRIORITIZED )
            {
                /*
                ** check if the LID is in the priority list, if so, skip it
                */
                for ( i = 0;
                      i < f_pLicEnumContext->List.Prioritized.cLicenses;
                      i++ )
                {
                    if ( MEMCMP( &f_pLicEnumContext->List.Prioritized.rgPrioritizedList[i].LID,
                                  f_pLID,
                                  DRM_ID_SIZE ) == 0 )
                    {
                        fKeepLooking = TRUE;
                        break;
                    }
                }
            }
        }
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**********************************************************************
** Function:    DRM_LST_EnumDelete
**
** Synopsis:    Delete currently enumerated license
**
** Arguments:
**      [f_pLicEnumContext] : The License store enumeration context
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pLicEnumContext is NULL or not initialized
**      DRM_E_LICENSE_NOT_FOUND
**          Could not find the license to delete
**
***********************************************************************/
DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LST_EnumDelete(
    __in DRM_LICSTOREENUM_CONTEXT *f_pLicEnumContext)
{
    return DRM_LST_EnumDeleteAndSaveDate(f_pLicEnumContext, FALSE);
}


/**********************************************************************
** Function:    DRM_LST_EnumDelete
**
** Synopsis:    Delete currently enumerated license
**
** Arguments:
**      [f_pLicEnumContext] : The License store enumeration context
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pLicEnumContext is NULL or not initialized
**      DRM_E_LICENSE_NOT_FOUND
**          Could not find the license to delete
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LST_EnumDeleteAndSaveDate(
    __in DRM_LICSTOREENUM_CONTEXT *f_pLicEnumContext,
    __in DRM_BOOL                  f_fSaveDate )
{
    DRM_RESULT dr         = DRM_SUCCESS;
    DRM_DST   *pDatastore = NULL;

    ChkArg( f_pLicEnumContext != NULL );
    ChkArg( f_pLicEnumContext->fInited );

    pDatastore = f_pLicEnumContext->pLicContext->pDatastore;
    ChkArg( pDatastore != NULL );

    ChkBOOL( f_pLicEnumContext->eMode != eDRM_LICENSE_STORE_MODE_NONE, DRM_E_LICENSE_NOT_FOUND );

    if ( f_pLicEnumContext->eMode == eDRM_LICENSE_STORE_MODE_PRIORITIZED
      || f_pLicEnumContext->eMode == eDRM_LICENSE_STORE_MODE_REVERSE_PRIORITIZED )
    {
        /*
        ** EnumNext has not been called yet, or the license was already deleted
        */
        ChkBOOL( f_pLicEnumContext->List.Prioritized.iNextLicense != 0
              && !f_pLicEnumContext->List.Prioritized.rgPrioritizedList[f_pLicEnumContext->List.Prioritized.iNextLicense - 1].fDelete,
                 DRM_E_LICENSE_NOT_FOUND );

        /*
        ** Delete the current license
        */
        ChkDR( DRM_DST_DeleteSlot( pDatastore,
                                  &f_pLicEnumContext->pLicContext->DstNamespaceContext,
                                  &f_pLicEnumContext->List.Prioritized.KID,
                                  &f_pLicEnumContext->List.Prioritized.rgPrioritizedList[f_pLicEnumContext->List.Prioritized.iNextLicense - 1].LID,
                                  &( f_pLicEnumContext->List.Prioritized.rgPrioritizedList[f_pLicEnumContext->List.Prioritized.iNextLicense - 1].slotHint ),
                                   TRUE ) );

        f_pLicEnumContext->List.Prioritized.rgPrioritizedList[f_pLicEnumContext->List.Prioritized.iNextLicense-1].fDelete = TRUE;

        if( DRM_DELETEDSTORE_IsDeletedStoreSupported() && f_fSaveDate )
        {
            ChkDR( DRM_DELETEDSTORE_UpdateRecentExpired(
                pDatastore,
                &f_pLicEnumContext->List.Prioritized.KID ) );
        }

        if( f_pLicEnumContext->pLicContext->eType != eDRM_LICENSE_STORE_XML )
        {
            ChkDR( DRM_RDS_DeleteLicenseEntry( pDatastore,
                                              &f_pLicEnumContext->List.NonPrioritized.CurrKID,
                                              &f_pLicEnumContext->List.NonPrioritized.CurrLID ) );
        }
    }
    /*
    ** eDRM_LICENSE_STORE_MODE_FILTERED or eDRM_LICENSE_STORE_MODE_NATURAL
    */
    else
    {
        /*
        ** Check that current license is valid
        */
        ChkBOOL( f_pLicEnumContext->List.NonPrioritized.fCurrIsValid, DRM_E_LICENSE_NOT_FOUND );

        if( pDatastore->eImplementationType == eDRM_DST_EMBEDDED )
        {
            ChkDR( DRM_DST_SlotEnumReloadCurrent ( pDatastore,
                                                  &f_pLicEnumContext->DstEnumContext,
                                                  &f_pLicEnumContext->SlotContext,
                                                   NULL,
                                                   NULL,
                                                   NULL ) );

            ChkDR( DRM_DST_SlotEnumDeleteCurrent( pDatastore,
                                                 &f_pLicEnumContext->DstEnumContext,
                                                 &f_pLicEnumContext->SlotContext ) );
        }
        else
        {
            ChkDR( DRM_DST_DeleteSlot( pDatastore,
                                       &f_pLicEnumContext->pLicContext->DstNamespaceContext,
                                       &f_pLicEnumContext->List.NonPrioritized.CurrKID,
                                       &f_pLicEnumContext->List.NonPrioritized.CurrLID,
                                       NULL,
                                       TRUE ) );
        }

        if( DRM_DELETEDSTORE_IsDeletedStoreSupported() && f_fSaveDate )
        {
            ChkDR( DRM_DELETEDSTORE_UpdateRecentExpired(
                pDatastore,
                &f_pLicEnumContext->List.NonPrioritized.CurrKID ) );
        }

        if ( f_pLicEnumContext->pLicContext->eType != eDRM_LICENSE_STORE_XML )
        {
            ChkDR( DRM_RDS_DeleteLicenseEntry( pDatastore,
                                              &f_pLicEnumContext->List.NonPrioritized.CurrKID,
                                              &f_pLicEnumContext->List.NonPrioritized.CurrLID ) );
        }

        f_pLicEnumContext->List.NonPrioritized.fCurrIsValid = FALSE;
    }

#if !_DATASTORE_WRITE_THRU
    ChkDR( DRM_DST_CommitStore( pDatastore ) );
#endif

ErrorExit:
    return dr;
}


/**********************************************************************
** Function:    DRM_LST_DeleteLicense
**
** Synopsis:    Delete a license based on KID and LID
**
** Arguments:
**      [f_pLicContext] : The License store context
**      [f_pKID]        : Specifies the KID of the license to delete
**      [f_pLID]        : Specifies the LID of the license to delete
**      [f_pSlotHint]   : Specifies the slot hint
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the required pointer parameters was NULL or the context
**          was not properly initialized
**
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_LST_DeleteLicense(
    __inout        DRM_LICSTORE_CONTEXT *f_pLicContext,
    __in     const DRM_KID              *f_pKID,
    __in     const DRM_LID              *f_pLID,
    __in_opt       DRM_DST_SLOT_HINT    *f_pSlotHint )
{
    return DRM_LST_DeleteLicenseAndSaveDate(f_pLicContext, f_pKID, f_pLID, f_pSlotHint, FALSE);
}

/**********************************************************************
** Function:    DRM_LST_UpdateLicense
**
** Synopsis:    Updates a license that already exists in the license store. The
**              slot may need to be resized if the license has changed in size.
**
** Arguments:
**      [f_pLicContext] : The License store context
**      [f_cbLicense]   : The length of the updated license buffer
**      [f_pbLicense]   : Buffer containing the updated license string
**      [f_pKID]        : Specifies the KID of the license to update
**      [f_pLID]        : Specifies the LID of the license to update
**      [f_dwPriority]  : Specifies the priority to give the license
**      [f_pSlotHint]   : Specifies the slot hint (optional)
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the required pointer parameters was NULL or the context
**          was not properly initialized
**      Other error code returned from datastore functions.
**
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_LST_UpdateLicense(
    __inout                          DRM_LICSTORE_CONTEXT     *f_pLicContext,
    __in                             DRM_DWORD                 f_cbLicense,
    __in_bcount( f_cbLicense ) const DRM_BYTE                 *f_pbLicense,
    __in                       const DRM_KID                  *f_pKID,
    __in                       const DRM_LID                  *f_pLID,
    __in                             DRM_DWORD                 f_dwPriority,
    __in_opt                         DRM_DST_SLOT_HINT        *f_pSlotHint )
{
    DRM_RESULT dr         = DRM_SUCCESS;
    DRM_DWORD  cbSlotSize = 0;

    /*
    ** Verify that the license exists in the specified store, and check the size
    */
    ChkDR( DRM_DST_OpenSlot( f_pLicContext->pDatastore,
                             &f_pLicContext->DstNamespaceContext,
                             eDRM_DST_OPEN_EXISTING | eDRM_DST_LOCKEXCLUSIVE | eDRM_DST_LOCKWAIT,
                             f_pKID,
                             f_pLID,
                             f_pSlotHint,
                             &cbSlotSize,
                             &f_pLicContext->SlotContext ) );

    /*
    ** Attempt to resize the slot if necessary
    */
    if ( f_cbLicense != ( cbSlotSize - SIZEOF( DRM_DWORD ) ) )
    {
        ChkDR( DRM_DST_SlotResize( f_pLicContext->pDatastore, &f_pLicContext->SlotContext, f_cbLicense + SIZEOF( DRM_DWORD ) ) );
    }
    ChkDR( DRM_DST_SlotWrite( f_pLicContext->pDatastore,
                              &f_pLicContext->SlotContext,
                              SIZEOF( DRM_DWORD ),
                              ( DRM_BYTE * )&f_dwPriority,
                              NULL ) );
    ChkDR( DRM_DST_SlotWrite( f_pLicContext->pDatastore,
                              &f_pLicContext->SlotContext,
                              f_cbLicense,
                              f_pbLicense,
                              NULL ) );

    ChkDR( DRM_DST_CloseSlot( f_pLicContext->pDatastore, &f_pLicContext->SlotContext ) );

#if !_DATASTORE_WRITE_THRU
    ChkDR( DRM_DST_CommitNamespace( f_pLicContext->pDatastore,
                                    &f_pLicContext->DstNamespaceContext ) );
#endif

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_LST_DeleteLicenseAndSaveData
**
** Synopsis:    Delete a license based on KID and LID and saves the deleted date in the RecentDeleted store
**
** Arguments:
**      [f_pLicContext] : The License store context
**      [f_pKID]        : Specifies the KID of the license to delete
**      [f_pLID]        : Specifies the LID of the license to delete
**      [f_pSlotHint]   : Specifies the slot hint
**      [f_fSaveDate]   : Specifies whether the deleted date should be persisted
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the required pointer parameters was NULL or the context
**          was not properly initialized
**
***********************************************************************/
DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LST_DeleteLicenseAndSaveDate(
    __inout        DRM_LICSTORE_CONTEXT *f_pLicContext,
    __in     const DRM_KID              *f_pKID,
    __in     const DRM_LID              *f_pLID,
    __in_opt       DRM_DST_SLOT_HINT    *f_pSlotHint,
    __in           DRM_BOOL              f_fSaveDate )
{
    DRM_RESULT   dr   = DRM_SUCCESS;

    ChkArg( f_pLicContext  != NULL );
    ChkArg( f_pKID         != NULL );
    ChkArg( f_pLID         != NULL );
    ChkArg( f_pLicContext->fInited );

    ChkDR( DRM_DST_DeleteSlot( f_pLicContext->pDatastore,
                               &f_pLicContext->DstNamespaceContext,
                               f_pKID,
                               f_pLID,
                               f_pSlotHint,
                               FALSE ) );

    if(DRM_DELETEDSTORE_IsDeletedStoreSupported() && f_fSaveDate)
    {
        ChkDR( DRM_DELETEDSTORE_UpdateRecentExpired(
            f_pLicContext->pDatastore,
            f_pKID ) );
    }

    if (   f_pLicContext->pDatastore != NULL
        && f_pLicContext->eType != eDRM_LICENSE_STORE_XML )
    {
        ChkDR( DRM_RDS_DeleteLicenseEntry( f_pLicContext->pDatastore,
                                           f_pKID,
                                           f_pLID ) );
    }

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_LST_LicCount
**
** Synopsis:    Counts the number of licenses in the store
**
** Arguments:
**      [f_pLicEnumContext] : The License store enumeration context
**      [f_pcLicenses]      : Returns the number of licenses in the store
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the required pointer parameters was NULL or the context
**          was not properly initialized
**
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_LST_LicCount(
    __in    DRM_LICSTOREENUM_CONTEXT *f_pLicEnumContext,
    __out   DRM_DWORD                *f_pcLicenses )
{
    DRM_RESULT     dr         = DRM_SUCCESS;
    DRM_DWORD      cLicenses  = 0;
    const DRM_DST *pDatastore = NULL;

    ChkArg( f_pLicEnumContext != NULL );
    ChkArg( f_pcLicenses      != NULL );
    ChkArg( f_pLicEnumContext->fInited );

    pDatastore = f_pLicEnumContext->pLicContext->pDatastore;

    *f_pcLicenses = 0;

    /*
    ** Check whether there are licenses in the store
    */
    if ( f_pLicEnumContext->eMode != eDRM_LICENSE_STORE_MODE_NONE )
    {
        while( TRUE )
        {
            /*
            **  Look for next available license
            */
            ChkDR( DRM_DST_SlotEnumNext( pDatastore,
                                        &f_pLicEnumContext->DstEnumContext,
                                        &f_pLicEnumContext->SlotContext,
                                        &f_pLicEnumContext->List.NonPrioritized.CurrKID,
                                        &f_pLicEnumContext->List.NonPrioritized.CurrLID,
                                        NULL ) );

            /*
            ** Increment the license count
            */
            cLicenses++;

            /*
            ** Close the slot
            */
            ChkDR( DRM_DST_CloseSlot( pDatastore, &f_pLicEnumContext->SlotContext ) );
        }
    }

ErrorExit:
    /*
    ** DRM_E_NOMORE is not an error
    */
    if ( dr == DRM_E_NOMORE )
    {
        dr = DRM_SUCCESS;
        *f_pcLicenses = cLicenses;
    }
    return dr;
}

/**********************************************************************
** Function:    DRM_LST_InitEnumMultiple
**
** Synopsis:    Initializes an enumeration over multiple license stores.
**
** Arguments:
**      [f_prgbLicContexts] : The License stores to enumerate over.
**      [f_cLicContexts]    : The element count of f_prgbLicContexts
**      [f_pKID]            : Optional. Specifies a KID to filter on.
**      [f_fPrioritized]    : Specifies whether to enumerate in priority order.
**      [f_pLicEnumContext] : Returns the enumeration context.
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the required pointer parameters was NULL or the context
**          was not properly initialized
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LST_InitEnumMultiple(
    __in_ecount( f_cLicContexts )       DRM_LICSTORE_CONTEXT              *f_prgbLicContexts[],
    __in                                DRM_DWORD                          f_cLicContexts,
    __in_opt                      const DRM_KID                           *f_pKID,
    __in                                DRM_BOOL                           f_fPrioritized,
    __out                               DRM_LICSTOREENUM_MULTIPLE_CONTEXT *f_pLicEnumContext )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  i  = 0;

    ChkArg( f_prgbLicContexts != NULL );
    ChkArg( f_cLicContexts    >  0 );
    ChkArg( f_cLicContexts    <= DRM_LICSTORE_MAX_STORES );
    ChkArg( f_pLicEnumContext != NULL );

    /*
    ** Initialize per-store members
    */
    f_pLicEnumContext->cLicContexts = 0;

    for ( i = 0; i < f_cLicContexts; i++ )
    {
        ChkDR( DRM_LST_InitEnum( f_prgbLicContexts[i],
                                 f_pKID,
                                 f_fPrioritized,
                                &f_pLicEnumContext->rgLicContexts[f_pLicEnumContext->cLicContexts] ) );
        /*
        ** Exclude stores with no licenses
        */
        if ( f_pLicEnumContext->rgLicContexts[f_pLicEnumContext->cLicContexts].eMode != eDRM_LICENSE_STORE_MODE_NONE
          && ( !f_fPrioritized || f_pLicEnumContext->rgLicContexts[f_pLicEnumContext->cLicContexts].List.Prioritized.cLicenses > 0 ) )
        {
            f_pLicEnumContext->cLicContexts++;
        }
    }

    /*
    ** Initialize cross-store members
    */
    f_pLicEnumContext->eMode   = f_pLicEnumContext->cLicContexts > 0? f_pLicEnumContext->rgLicContexts[0].eMode : eDRM_LICENSE_STORE_MODE_NONE;
    f_pLicEnumContext->fInited = TRUE;

ErrorExit:
    return dr;
}

static DRM_NO_INLINE DRM_RESULT DRM_CALL _EnumNextMultiple_Natural(
    __in       DRM_LICSTOREENUM_MULTIPLE_CONTEXT *f_pLicEnumContext,
    __out_opt  DRM_KID                           *f_pKID,
    __out_opt  DRM_LID                           *f_pLID,
    __out_opt  DRM_LICSTORE_CONTEXT             **f_ppLicStore,
    __out_opt  DRM_DST_SLOT_HINT                 *f_pSlotHint,
    __out_opt  DRM_DWORD                         *f_pcbLicense )
{
    DRM_RESULT dr        = DRM_E_NOMORE;
    DRM_DWORD  iCurStore = 0;

    ChkArg( f_pLicEnumContext != NULL );

    /*
    ** Returns licenses enumerated naturally from the stores
    ** in the order they were initialized
    */
    for ( iCurStore = 0; iCurStore < f_pLicEnumContext->cLicContexts
                      && iCurStore < DRM_LICSTORE_MAX_STORES; iCurStore++ )
    {
        dr = DRM_LST_EnumNext( &f_pLicEnumContext->rgLicContexts[iCurStore],
                                f_pKID,
                                f_pLID,
                                f_pSlotHint,
                                f_pcbLicense );
        if ( dr == DRM_E_NOMORE ) /* Remove from the list of stores */
        {
            DRM_DWORD iStores = 0;

            ZEROMEM( &f_pLicEnumContext->rgLicContexts[iCurStore], SIZEOF( DRM_LICSTOREENUM_CONTEXT ) );
            f_pLicEnumContext->cLicContexts--;

            for ( iStores = iCurStore; iStores     < f_pLicEnumContext->cLicContexts
                                    && iStores + 1 < DRM_LICSTORE_MAX_STORES; iStores++ )
            {
                MEMCPY( &f_pLicEnumContext->rgLicContexts[iStores],
                        &f_pLicEnumContext->rgLicContexts[iStores+1],
                        SIZEOF( DRM_LICSTOREENUM_CONTEXT ) );
            }
            iCurStore--;
        }
        else
        {
            f_pLicEnumContext->pCurEnum = &f_pLicEnumContext->rgLicContexts[iCurStore];
            if ( f_ppLicStore != NULL )
            {
                *f_ppLicStore = f_pLicEnumContext->rgLicContexts[iCurStore].pLicContext;
            }
            break;
        }
    }

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_LST_EnumNextMultiple
**
** Synopsis:    Retrieves the next element in an enumeration across multiple license stores.
**
** Arguments:
**      [f_pLicEnumContext] : The License enumeration context.
**      [f_pKID]            : Optional. Returns the KID for the next license.
**      [f_pLID]            : Optional. Returns the LID for the next license.
**      [f_ppLicStore]      : Optional. Returns a pointer to the license store containing the next license.
**      [f_pSlotHint]       : Optional. Returns a slot hint for the next license.
**      [f_pcbLicense]      : Optional. Returns the size in bytes of the next license.
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the required pointer parameters was NULL or the context
**          was not properly initialized
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LST_EnumNextMultiple(
    __in       DRM_LICSTOREENUM_MULTIPLE_CONTEXT *f_pLicEnumContext,
    __out_opt  DRM_KID                           *f_pKID,
    __out_opt  DRM_LID                           *f_pLID,
    __out_opt  DRM_LICSTORE_CONTEXT             **f_ppLicStore,
    __out_opt  DRM_DST_SLOT_HINT                 *f_pSlotHint,
    __out_opt  DRM_DWORD                         *f_pcbLicense )
{
    DRM_RESULT                dr                                               = DRM_SUCCESS;
    DRM_DWORD                 iCurStore                                        = 0;
    DRM_LICSTOREENUM_CONTEXT *pLicContextsPrioritized[DRM_LICSTORE_MAX_STORES] = { 0 };
    DRM_DWORD                 cLicContextsPrioritized                          = 0;

    ChkArg( f_pLicEnumContext != NULL );
    ChkArg( f_pLicEnumContext->fInited );

    f_pLicEnumContext->pCurEnum = NULL;

    if ( f_ppLicStore != NULL )
    {
        *f_ppLicStore = NULL;
    }

    switch ( f_pLicEnumContext->eMode )
    {
    case eDRM_LICENSE_STORE_MODE_NONE:
        ChkDR( DRM_E_NOMORE );
        break;
    case eDRM_LICENSE_STORE_MODE_PRIORITIZED:
        /*
        ** Returns licenses enumerated in priority order across stores
        */

        /*
        ** Set up a list of enumerations which are still prioritized
        */
        for ( iCurStore = 0; iCurStore               < f_pLicEnumContext->cLicContexts
                          && cLicContextsPrioritized < DRM_LICSTORE_MAX_STORES; iCurStore++ )
        {
            if ( f_pLicEnumContext->rgLicContexts[iCurStore].eMode == eDRM_LICENSE_STORE_MODE_PRIORITIZED
              && f_pLicEnumContext->rgLicContexts[iCurStore].List.Prioritized.iNextLicense < f_pLicEnumContext->rgLicContexts[iCurStore].List.Prioritized.cLicenses )
            {
                pLicContextsPrioritized[cLicContextsPrioritized] = &f_pLicEnumContext->rgLicContexts[iCurStore];
                cLicContextsPrioritized++;
            }
        }

        /*
        ** If all stores are post prioritized (prioritized lists are all empty),
        ** enumerate in natural mode
        */
        if ( cLicContextsPrioritized == 0 )
        {
            f_pLicEnumContext->eMode = eDRM_LICENSE_STORE_MODE_POST_PRIORITIZED;
            ChkDR( _EnumNextMultiple_Natural( f_pLicEnumContext,
                                              f_pKID,
                                              f_pLID,
                                              f_ppLicStore,
                                              f_pSlotHint,
                                              f_pcbLicense ) );
        }
        else
        {
            DRM_DWORD dwPriorityMax = 0;

            for ( iCurStore = 0; iCurStore < cLicContextsPrioritized; iCurStore++ )
            {
                if ( dwPriorityMax <= pLicContextsPrioritized[iCurStore]->List.Prioritized.rgPrioritizedList[pLicContextsPrioritized[iCurStore]->List.Prioritized.iNextLicense].dwPriority )
                {
                    dwPriorityMax = pLicContextsPrioritized[iCurStore]->List.Prioritized.rgPrioritizedList[pLicContextsPrioritized[iCurStore]->List.Prioritized.iNextLicense].dwPriority;
                    f_pLicEnumContext->pCurEnum = pLicContextsPrioritized[iCurStore];
                }
            }

            if ( f_ppLicStore != NULL )
            {
                *f_ppLicStore = f_pLicEnumContext->pCurEnum->pLicContext;
            }
            ChkDR( DRM_LST_EnumNext( f_pLicEnumContext->pCurEnum,
                                     f_pKID,
                                     f_pLID,
                                     f_pSlotHint,
                                     f_pcbLicense ) );
        }
        break;
    case eDRM_LICENSE_STORE_MODE_NATURAL:
    case eDRM_LICENSE_STORE_MODE_POST_PRIORITIZED:
    case eDRM_LICENSE_STORE_MODE_FILTERED:
        ChkDR( _EnumNextMultiple_Natural( f_pLicEnumContext,
                                          f_pKID,
                                          f_pLID,
                                          f_ppLicStore,
                                          f_pSlotHint,
                                          f_pcbLicense ) );
        break;
    default:
        ChkArg( FALSE );
        break;
    }

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_LST_EnumDeleteMultiple
**
** Synopsis:    Delete currently enumerated license
**
** Arguments:
**      [f_pLicEnumContext] : The multi-license store enumeration context
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pLicEnumContext is NULL or not initialized
**      DRM_E_LICENSE_NOT_FOUND
**          Could not find the license to delete
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LST_EnumDeleteMultiple(
    __in DRM_LICSTOREENUM_MULTIPLE_CONTEXT *f_pLicEnumContext )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pLicEnumContext != NULL );

    ChkBOOL( f_pLicEnumContext->pCurEnum != NULL, DRM_E_LICENSE_NOT_FOUND );

    ChkDR( DRM_LST_EnumDeleteAndSaveDate( f_pLicEnumContext->pCurEnum, FALSE ) );

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;
