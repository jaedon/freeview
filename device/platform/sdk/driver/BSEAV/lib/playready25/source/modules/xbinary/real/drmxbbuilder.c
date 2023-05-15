/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMXBBUILDER_C
#include <drmxb.h>
#include <drmxbbuilder.h>
#include <drmerr.h>
#include <drmbytemanip.h>
#include <byteorder.h>
#include <drmmathsafe.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;


static DRM_BOOL _DRM_XB_IsDuplicateAllowed(
    __in_ecount( 1 ) const DRM_XB_BUILDER_LISTNODE   *f_plistnode,
    __in             const DRM_XB_FORMAT_DESCRIPTION *f_pformat );

static DRM_RESULT DRM_CALL _DRM_XB_GetObjectLength(
    __in             DRM_WORD                   f_wType,
    __in_opt const   DRM_VOID                  *f_pvObject,
    __in     const   DRM_XB_FORMAT_DESCRIPTION *f_pformat,
    __out            DRM_DWORD                 *f_pdwLength );

static DRM_BOOL _DRM_XB_IsNativeObject( __in DRM_WORD f_wType, __in const DRM_XB_FORMAT_DESCRIPTION *f_pFormat )
{
    /* Note: Internal function - No need to check input */

    if( _XB_IsKnownObjectType(f_pFormat, f_wType) )
    {
        if( ! ( f_pFormat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(f_pFormat, f_wType)].wFlags & XB_FLAGS_ALLOW_EXTERNAL_PARSE ) )
        {
            return TRUE;
        }
    }

    DRMASSERT( !"For now XB doesn't support external parser.  Everything is an \"unknown object\" but is still parsed by the system" );

    return FALSE;
}

static DRM_BOOL _DRM_XB_IsDuplicateAllowed( __in_ecount( 1 ) const DRM_XB_BUILDER_LISTNODE *f_plistnode, __in const DRM_XB_FORMAT_DESCRIPTION *f_pFormat )
{
    /* Note: Internal function - No need to check input */
    DRM_BOOL fRetVal = FALSE;
    if( _DRM_XB_IsNativeObject( f_plistnode->Node.wType, f_pFormat ) )
    {
        fRetVal =  f_pFormat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(f_pFormat, f_plistnode->Node.wType)].fDuplicateAllowed;
    }
    else
    {
        DRMASSERT( !"For now XB doesn't support external parser.  Everything is an \"unknown object\" but is still parsed by the system" );
    }

    return fRetVal;
}

/******************************************************************************
**
** Function :   _DRM_XB_GetParent
**
** Synopsis :   Returns the parent object type for a given node.
**
** Arguments :  plistnode - Node for which parent needs to be found
**
******************************************************************************/

static DRM_WORD _DRM_XB_GetParent( __in_ecount( 1 ) const DRM_XB_BUILDER_LISTNODE *f_plistnode, __in const DRM_XB_FORMAT_DESCRIPTION *f_pFormat )
{
    /* Note: Internal function - No need to check input */
    DRM_WORD wRetVal = XB_OBJECT_TYPE_INVALID;
    if( _DRM_XB_IsNativeObject( f_plistnode->Node.wType, f_pFormat ) )
    {
        wRetVal = f_pFormat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(f_pFormat, f_plistnode->Node.wType)].wParent;
    }
    else
    {
        DRMASSERT( !"For now XB doesn't support external parser.  Everything is an \"unknown object\" but is still parsed by the system" );
    }

    return wRetVal;
}

/******************************************************************************
**
** Function :   _DRM_XB_GetBuilderNode
**
** Synopsis :   Returns the builder node for a given XB object type.
**              Returns null if object is not found
**
** Arguments :  f_pcontextBuilder - builder context
**              f_wType     - object type
**
******************************************************************************/

DRM_XB_BUILDER_NODE* _DRM_XB_GetBuilderNode(
    __in_ecount( 1 ) const DRM_XB_BUILDER_CONTEXT_INTERNAL *f_pcontextBuilder,
    __in                   DRM_WORD                         f_wType )
{
    /* Note: Internal function - No need to check input */
    DRM_XB_BUILDER_NODE  *pNode    = NULL;

    if( _DRM_XB_IsNativeObject( f_wType, f_pcontextBuilder->pformat ) )
    {
        pNode = f_pcontextBuilder->rgpObjectNodes[_XB_MapObjectTypeToEntryDescriptionIndex(f_pcontextBuilder->pformat, f_wType)];
    }
    else
    {
        DRMASSERT( !"For now XB doesn't support external parser.  Everything is an \"unknown object\" but is still parsed by the system" );
    }
    return pNode;
}

/******************************************************************************
**
** Function :   _DRM_XB_GetObjectLength
**
** Synopsis :   Estimates the length of an object. For a container, it only
**              returns the length of the base object
**
** Arguments :
**              [f_wType]    : Object type
**              [f_pvObject] : Actual object passed as void (optional)
**
** Returns:
**              Returns the length of the object
**
******************************************************************************/
static DRM_RESULT DRM_CALL _DRM_XB_GetObjectLength(
    __in             DRM_WORD                          f_wType,
    __in_opt const   DRM_VOID                         *f_pvObject,
    __in     const   DRM_XB_FORMAT_DESCRIPTION        *f_pFormat,
    __out            DRM_DWORD                        *f_pdwLength )

{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cbLength = XB_BASE_OBJECT_LENGTH;
    const DRM_XB_ENTRY_DESCRIPTION   *pEntry   = NULL;
    const DRM_XB_ELEMENT_DESCRIPTION *pElement = NULL;
    DRM_DWORD iElement = 0;
    DRM_DWORD dwResult = 0;

    ChkArg( f_pdwLength != NULL );
    *f_pdwLength = 0;

    if( !_XB_IsKnownObjectType(f_pFormat, f_wType) )
    {
        *f_pdwLength = 0;
        goto ErrorExit;
    }

    pEntry = &(f_pFormat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(f_pFormat, f_wType)]);
    if( pEntry->wFlags & XB_FLAGS_CONTAINER )
    {
        /*
        **  For containers we just return the base object length
        */
        *f_pdwLength = cbLength;
        goto ErrorExit;
    }

    /*
    **  Loop through each of the elements in the entry and sum up the length
    */
    for( iElement = 0; iElement < pEntry->cContainerOrObjectDescriptions; iElement++ )
    {
        const DRM_XB_ELEMENT_DESCRIPTION *pElementArray = (const DRM_XB_ELEMENT_DESCRIPTION*)pEntry->pContainerOrObjectDescriptions;
        DRM_VOID *pobject = NULL;
        pElement = &(pElementArray[iElement]);
        ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)f_pvObject, pElement->wOffsetInCurrentStruct, (DRM_DWORD_PTR*)&pobject ) );

        switch( pElement->eElementType )
        {
        case XB_ELEMENT_TYPE_WORD:
            ChkDR( DRM_DWordAdd( cbLength, SIZEOF( DRM_WORD ), &cbLength ) );
            break;
        case XB_ELEMENT_TYPE_DWORD:
            ChkDR( DRM_DWordAdd( cbLength, SIZEOF( DRM_DWORD ), &cbLength ) );
            break;
        case XB_ELEMENT_TYPE_QWORD:
            ChkDR( DRM_DWordAdd( cbLength, SIZEOFQWORD, &cbLength ) );
            break;
        case XB_ELEMENT_TYPE_GUID:
            ChkDR( DRM_DWordAdd( cbLength, SIZEOF( DRM_GUID ), &cbLength ) );
            break;
        case XB_ELEMENT_TYPE_GUIDLIST:
            ChkDR( DRM_DWordMult( ( ( DRM_XB_GUIDLIST *)pobject )->cGUIDs, SIZEOF( DRM_GUID ), &dwResult ) );
            ChkDR( DRM_DWordAdd( cbLength, dwResult, &cbLength ) );
            ChkDR( DRM_DWordAdd( cbLength, SIZEOF( DRM_DWORD ), &cbLength ) );
            break;
        case XB_ELEMENT_TYPE_WORDLIST:
            ChkDR( DRM_DWordMult( ( ( DRM_XB_WORDLIST * )pobject )->cWORDs,  SIZEOF( DRM_WORD ), &dwResult ) );
            ChkDR( DRM_DWordAdd( cbLength, dwResult, &cbLength ) );
            ChkDR( DRM_DWordAdd( cbLength, SIZEOF( DRM_DWORD ), &cbLength ) );
            break;
        case XB_ELEMENT_TYPE_BYTEARRAY:
            ChkDR( DRM_DWordAdd( cbLength, ( ( DRM_XB_BYTEARRAY * )pobject )->cbData, &cbLength ) );
            if( pElement->xbSize.eSizeType == XB_SIZE_IS_RELATIVE_WORD )
            {
                ChkDR( DRM_DWordAdd( cbLength, SIZEOF( DRM_WORD ), &cbLength ) );
            }
            else if( pElement->xbSize.eSizeType == XB_SIZE_IS_RELATIVE_DWORD )
            {
                ChkDR( DRM_DWordAdd( cbLength, SIZEOF( DRM_DWORD ), &cbLength ) );
            }
            break;
        default:
            break;
        }
    }

    *f_pdwLength = cbLength;

ErrorExit:
    return dr;
}


PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_WRITABLE_BUFFER, "f_pbBuffer length defined by f_piBuffer")
static DRM_VOID _DRM_XB_Serialize_UnknownBaseObject(
    __in                                                   DRM_WORD    f_wType,
    __in                                                   DRM_WORD    f_wFlags,
    __in                                                   DRM_DWORD   f_dwLength,
    __inout_bcount( XB_BASE_OBJECT_LENGTH + *f_piBuffer )  DRM_BYTE   *f_pbBuffer,
    __inout_ecount( 1 )                                    DRM_DWORD  *f_piBuffer )
PREFAST_POP
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD dwIndex = *f_piBuffer;

    WORD_TO_NETWORKBYTES(  f_pbBuffer, dwIndex, f_wFlags );
    ChkDR( DRM_DWordAdd( dwIndex, SIZEOF( DRM_WORD ), &dwIndex ) );

    WORD_TO_NETWORKBYTES(  f_pbBuffer, dwIndex, f_wType );
    ChkDR( DRM_DWordAdd( dwIndex, SIZEOF( DRM_WORD ), &dwIndex ) );

    DWORD_TO_NETWORKBYTES( f_pbBuffer, dwIndex, f_dwLength );
    ChkDR( DRM_DWordAdd( dwIndex, SIZEOF( DRM_DWORD ), &dwIndex ) );

    *f_piBuffer = dwIndex;

ErrorExit:
    if( DRM_FAILED( dr ) )
    {
        *f_piBuffer = 0;
    }
}

/*
** Serialize the basic type, flags, and length
*/
static DRM_VOID DRM_CALL _DRM_XB_Serialize_BaseObject(
    __in                                                   DRM_WORD    f_wType,
    __in                                                   DRM_DWORD   f_dwLength,
    __inout_bcount( XB_BASE_OBJECT_LENGTH + *f_piBuffer )  DRM_BYTE   *f_pbBuffer,
    __inout_ecount( 1 )                                    DRM_DWORD  *f_piBuffer,
    __in                                             const DRM_XB_FORMAT_DESCRIPTION   *f_pFormat )
{
    DRMASSERT( _XB_IsKnownObjectType(f_pFormat, f_wType) );
    if ( _XB_IsKnownObjectType(f_pFormat, f_wType) )
    {
        _DRM_XB_Serialize_UnknownBaseObject( f_wType,
                                             f_pFormat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(f_pFormat, f_wType)].wFlags,
                                             f_dwLength,
                                             f_pbBuffer,
                                             f_piBuffer );
    }
}

/*
** A series of serialize functions follow to serialize each of the simple element types that compose an object
*/

static DRM_RESULT DRM_CALL _DRM_XB_Serialize_WORD(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT      dr         = DRM_SUCCESS;
    const DRM_WORD *pword      = ( const DRM_WORD* )f_pvObject;
    DRM_DWORD       dwResult   = 0;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );

    ChkDR( DRM_DWordAdd( *f_piBuffer, SIZEOF( DRM_WORD ), &dwResult ) );
    ChkBOOL( f_cbBuffer >= dwResult,
             DRM_E_BUFFERTOOSMALL );

    WORD_TO_NETWORKBYTES( f_pbBuffer, *f_piBuffer, *pword );
    ChkDR( DRM_DWordAdd( *f_piBuffer, SIZEOF( DRM_WORD ), f_piBuffer ) );

ErrorExit:
    return dr;
}

static DRM_RESULT DRM_CALL _DRM_XB_Serialize_DWORD(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT       dr       = DRM_SUCCESS;
    const DRM_DWORD *pdword   = ( const DRM_DWORD* )f_pvObject;
    DRM_DWORD        dwResult = 0;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );

    ChkDR( DRM_DWordAdd( *f_piBuffer, SIZEOF( DRM_DWORD ), &dwResult ) );
    ChkBOOL( f_cbBuffer >= dwResult,
             DRM_E_BUFFERTOOSMALL );

    DWORD_TO_NETWORKBYTES( f_pbBuffer, *f_piBuffer, *pdword );
    ChkDR( DRM_DWordAdd( *f_piBuffer, SIZEOF( DRM_DWORD ), f_piBuffer ) );

ErrorExit:
    return dr;
}

static DRM_RESULT DRM_CALL _DRM_XB_Serialize_QWORD(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT        dr       = DRM_SUCCESS;
    const DRM_UINT64 *pqword   = ( const DRM_UINT64* )f_pvObject;
    DRM_DWORD         dwResult = 0;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );

    ChkDR( DRM_DWordAdd( *f_piBuffer, SIZEOFQWORD, &dwResult ) );
    ChkBOOL( f_cbBuffer >= dwResult,
             DRM_E_BUFFERTOOSMALL );

    QWORD_TO_NETWORKBYTES( f_pbBuffer, *f_piBuffer, *pqword );
    ChkDR( DRM_DWordAdd( *f_piBuffer, SIZEOFQWORD, f_piBuffer ) );

ErrorExit:
    return dr;
}

static DRM_RESULT DRM_CALL _DRM_XB_Serialize_GUID(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT       dr       = DRM_SUCCESS;
    const DRM_GUID  *pguid    = ( const DRM_GUID * )f_pvObject;
    DRM_DWORD        dwResult = 0;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );

    ChkDR( DRM_DWordAdd( *f_piBuffer, SIZEOF( DRM_GUID ), &dwResult ) );
    ChkBOOL( f_cbBuffer >= dwResult,
             DRM_E_BUFFERTOOSMALL );

    DRM_BYT_CopyBytes( f_pbBuffer,
                       *f_piBuffer,
                       pguid,
                       0,
                       SIZEOF( DRM_GUID ) );
    ChkDR( DRM_DWordAdd( *f_piBuffer, SIZEOF( DRM_GUID ), f_piBuffer ) );

ErrorExit:
    return dr;
}

static DRM_RESULT DRM_CALL _DRM_XB_Serialize_GUIDLIST(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT             dr        = DRM_SUCCESS;
    DRM_DWORD              iBuffer   = 0;
    DRM_DWORD              iGuids    = 0;
    const DRM_XB_GUIDLIST *pguidlist = ( const DRM_XB_GUIDLIST * )f_pvObject;
    DRM_DWORD              dwResult  = 0;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );
    ChkArg( pguidlist->pguidBuffer != NULL );

    iBuffer = *f_piBuffer;

    ChkDR( DRM_DWordMult( SIZEOF( DRM_GUID ), pguidlist->cGUIDs, &dwResult ) );
    ChkDR( DRM_DWordAdd( dwResult, iBuffer, &dwResult ) );
    ChkDR( DRM_DWordAdd( dwResult, SIZEOF( DRM_DWORD ), &dwResult ) );

    ChkBOOL( f_cbBuffer >= dwResult,
             DRM_E_BUFFERTOOSMALL );

    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, pguidlist->cGUIDs );
    ChkDR( DRM_DWordAdd( iBuffer, SIZEOF( DRM_DWORD ), &iBuffer ) );

    for ( iGuids = 0; iGuids < pguidlist->cGUIDs; iGuids++ )
    {
        ChkDR( DRM_DWordMult( iGuids, SIZEOF( DRM_GUID ), &dwResult ) );
        ChkDR( DRM_DWordAdd( pguidlist->iGuids, dwResult, &dwResult ) );
        DRM_BYT_CopyBytes( f_pbBuffer,
                           iBuffer,
                           pguidlist->pguidBuffer,
                           dwResult,
                           SIZEOF( DRM_GUID ) );
        ChkDR( DRM_DWordAdd( iBuffer, SIZEOF( DRM_GUID ), &iBuffer ) );
    }
    *f_piBuffer = iBuffer;

ErrorExit:
    return dr;
}

static DRM_RESULT DRM_CALL _DRM_XB_Serialize_WORDLIST(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT             dr        = DRM_SUCCESS;
    DRM_DWORD              iBuffer   = 0;
    DRM_DWORD              iWords    = 0;
    const DRM_XB_WORDLIST *pwordlist = ( const DRM_XB_WORDLIST * )f_pvObject;
    DRM_DWORD              dwResult  = 0;
    DRM_BYTE              *pbResult  = NULL;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );
    ChkArg( pwordlist->pwordBuffer != NULL );

    iBuffer = *f_piBuffer;

    ChkDR( DRM_DWordMult( SIZEOF( DRM_WORD ), pwordlist->cWORDs, &dwResult ) );
    ChkDR( DRM_DWordAdd( dwResult, iBuffer, &dwResult ) );
    ChkDR( DRM_DWordAdd( dwResult, SIZEOF( DRM_DWORD ), &dwResult ) );

    ChkBOOL( f_cbBuffer >= dwResult,
             DRM_E_BUFFERTOOSMALL );

    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, pwordlist->cWORDs );
    ChkDR( DRM_DWordAdd( iBuffer, SIZEOF( DRM_DWORD ), &iBuffer ) );

    for ( iWords = 0; iWords < pwordlist->cWORDs; iWords++ )
    {
        ChkDR( DRM_DWordMult( iWords, SIZEOF( DRM_WORD ), &dwResult ) );
        ChkDR( DRM_DWordAdd( dwResult, pwordlist->iWords, &dwResult ) );
        ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)pwordlist->pwordBuffer, dwResult, (DRM_DWORD_PTR*)&pbResult ) );

        ChkDR( _DRM_XB_Serialize_WORD( pbResult,
                                       f_pbBuffer,
                                       f_cbBuffer,
                                       &iBuffer ) );
    }
    *f_piBuffer = iBuffer;

ErrorExit:
    return dr;
}

static DRM_RESULT DRM_CALL _DRM_XB_Serialize_BYTEARRAY(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT              dr         = DRM_SUCCESS;
    const DRM_XB_BYTEARRAY *pbytearray = ( const DRM_XB_BYTEARRAY * )f_pvObject;
    DRM_DWORD               dwResult   = 0;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );
    ChkArg( pbytearray->cbData == 0 || pbytearray->pbDataBuffer != NULL );

    ChkDR( DRM_DWordAdd( *f_piBuffer, pbytearray->cbData, &dwResult ) );
    ChkBOOL( f_cbBuffer >= dwResult,
             DRM_E_BUFFERTOOSMALL );

    DRM_BYT_CopyBytes( f_pbBuffer,
                       *f_piBuffer,
                       pbytearray->pbDataBuffer,
                       pbytearray->iData,
                       pbytearray->cbData );
    ChkDR( DRM_DWordAdd( *f_piBuffer, pbytearray->cbData, f_piBuffer ) );

ErrorExit:
    return dr;
}

/*
**  Serialize a while object.  The object header has already been serialized which means
**  just loop through all elements in the object and serailize each one.
*/
static DRM_RESULT DRM_CALL _DRM_XB_Serialize_Object(
    __in_ecount(1)         const DRM_XB_ENTRY_DESCRIPTION *f_pEntry,
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT                        dr        = DRM_SUCCESS;
    const DRM_XB_ELEMENT_DESCRIPTION *pElements = NULL;
    DRM_DWORD                         iElement  = 0;

    ChkArg( f_pEntry   != NULL );
    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );

    pElements = (const DRM_XB_ELEMENT_DESCRIPTION*) f_pEntry->pContainerOrObjectDescriptions;

    for( iElement = 0; iElement < f_pEntry->cContainerOrObjectDescriptions; iElement++ )
    {
        DRM_VOID *pobject = NULL;

        ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)f_pvObject, pElements[iElement].wOffsetInCurrentStruct, (DRM_DWORD_PTR*)&pobject ) );

        switch( pElements[iElement].eElementType )
        {
        case XB_ELEMENT_TYPE_WORD:          ChkDR( _DRM_XB_Serialize_WORD     ( pobject, f_pbBuffer, f_cbBuffer, f_piBuffer ) ); break;
        case XB_ELEMENT_TYPE_DWORD:         ChkDR( _DRM_XB_Serialize_DWORD    ( pobject, f_pbBuffer, f_cbBuffer, f_piBuffer ) ); break;
        case XB_ELEMENT_TYPE_QWORD:         ChkDR( _DRM_XB_Serialize_QWORD    ( pobject, f_pbBuffer, f_cbBuffer, f_piBuffer ) ); break;
        case XB_ELEMENT_TYPE_GUID:          ChkDR( _DRM_XB_Serialize_GUID     ( pobject, f_pbBuffer, f_cbBuffer, f_piBuffer ) ); break;
        case XB_ELEMENT_TYPE_GUIDLIST:      ChkDR( _DRM_XB_Serialize_GUIDLIST ( pobject, f_pbBuffer, f_cbBuffer, f_piBuffer ) ); break;
        case XB_ELEMENT_TYPE_WORDLIST:      ChkDR( _DRM_XB_Serialize_WORDLIST ( pobject, f_pbBuffer, f_cbBuffer, f_piBuffer ) ); break;
        case XB_ELEMENT_TYPE_BYTEARRAY:
            if( pElements[iElement].xbSize.eSizeType == XB_SIZE_IS_RELATIVE_WORD )
            {
                /* If it's a relative size the WORD needs to be serialized as well */
                const DRM_XB_BYTEARRAY   *pbytearray = ( const DRM_XB_BYTEARRAY * )pobject;
                DRM_WORD                  word       = ( DRM_WORD )pbytearray->cbData;

                ChkDR( _DRM_XB_Serialize_WORD     ( &word, f_pbBuffer, f_cbBuffer, f_piBuffer ) );
            }
            else if( pElements[iElement].xbSize.eSizeType == XB_SIZE_IS_RELATIVE_DWORD )
            {
                /* If it's a relative size the DWORD needs to be serialized as well */
                const DRM_XB_BYTEARRAY   *pbytearray = ( const DRM_XB_BYTEARRAY * )pobject;
                DRM_DWORD                 dword      = pbytearray->cbData;

                ChkDR( _DRM_XB_Serialize_DWORD     ( &dword, f_pbBuffer, f_cbBuffer, f_piBuffer ) );
            }
            ChkDR( _DRM_XB_Serialize_BYTEARRAY( pobject, f_pbBuffer, f_cbBuffer, f_piBuffer ) ); break;
        case XB_ELEMENT_TYPE_INVALID:
        default:
            DRMASSERT( !"Unsupported object type.  Most likely there is an error in the format description tables." );
            ChkDR( DRM_E_XB_UNKNOWN_ELEMENT_TYPE );
        }
    }

ErrorExit:
    return dr;
}

/*
** Serialize a conainer.  This function is recursize if the container has subcontainers that need serializing.
** The header for the container has already been serialized so loop through each entry and serialize as appropriate.
*/
static DRM_RESULT DRM_CALL _DRM_XB_Serialize_Container(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer,
    __in                   const DRM_XB_FORMAT_DESCRIPTION   *f_pFormat)
{
    DRM_RESULT                      dr          = DRM_SUCCESS;
    const DRM_XB_BUILDER_LISTNODE  *pnodeChild  = ( const DRM_XB_BUILDER_LISTNODE * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );

    /* Serialize all the child nodes */
    while ( pnodeChild != NULL )
    {
        if( _DRM_XB_IsNativeObject( pnodeChild->Node.wType, f_pFormat ) )
        {
            DRM_DWORD dwResult = 0;

            ChkDR( DRM_DWordAdd( *f_piBuffer, XB_BASE_OBJECT_LENGTH, &dwResult ) );
            ChkBOOL( f_cbBuffer >= dwResult,
                     DRM_E_BUFFERTOOSMALL );

            _DRM_XB_Serialize_BaseObject( pnodeChild->Node.wType,
                                         pnodeChild->Node.cbLength,
                                         f_pbBuffer,
                                         f_piBuffer,
                                         f_pFormat );

            if ( pnodeChild->Node.pvObject != NULL )
            {
                if( f_pFormat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(f_pFormat, pnodeChild->Node.wType)].wFlags & XB_FLAGS_CONTAINER )
                {
                    /* Recurse for the contained container */
                    ChkDR( _DRM_XB_Serialize_Container( pnodeChild->Node.pvObject, f_pbBuffer, f_cbBuffer, f_piBuffer, f_pFormat ) );
                }
                else
                {
                    ChkDR( _DRM_XB_Serialize_Object( &(f_pFormat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(f_pFormat, pnodeChild->Node.wType)]),
                                                    pnodeChild->Node.pvObject,
                                                    f_pbBuffer,
                                                    f_cbBuffer,
                                                    f_piBuffer ) );
                }
            }
        }
        else
        {
            DRMASSERT( !"Unexpected" );
        }
        pnodeChild = pnodeChild->pNext;
    }

ErrorExit:
    return dr;
}

/*
**  Exists to insert the new node in a sorted manner if the format dictates that there is an order to which the elements should appear.
**  Order is deteremined by DRM_XB_ENTRY_DESCRIPTION.wBuilderSortOrder
*/
static DRM_RESULT _InsertNodeInChildListSorted(
    __inout_ecount( 1 ) const DRM_XB_BUILDER_CONTEXT_INTERNAL    *f_pcontextBuilder,
    __inout                   DRM_XB_BUILDER_NODE                *pnodeParent,
    __inout                   DRM_XB_BUILDER_LISTNODE            *plistnodeChild )
{
    DRM_XB_BUILDER_LISTNODE *pCurr = ( DRM_XB_BUILDER_LISTNODE * )pnodeParent->pvObject;
    DRM_XB_BUILDER_LISTNODE *pPrev = NULL;

    while( pCurr != NULL
        && (    f_pcontextBuilder->pformat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(f_pcontextBuilder->pformat, plistnodeChild->Node.wType)].wBuilderSortOrder
             >  f_pcontextBuilder->pformat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(f_pcontextBuilder->pformat, pCurr->Node.wType)].wBuilderSortOrder ) )
    {
        pPrev = pCurr;
        pCurr = pCurr->pNext;
    }

    if( pCurr == ( DRM_XB_BUILDER_LISTNODE * )pnodeParent->pvObject )
    {
        /* Just insert at the head of the list */
        plistnodeChild->pNext = ( DRM_XB_BUILDER_LISTNODE * )pnodeParent->pvObject;
        pnodeParent->pvObject = ( const DRM_VOID * )plistnodeChild;
    }
    else
    {
        /* Insert after pPrev */
        plistnodeChild->pNext = pPrev->pNext;
        pPrev->pNext = plistnodeChild;
    }

    return DRM_SUCCESS;
}

/******************************************************************************
**
** Function :   _XB_AddHierarchy
**
** Synopsis :   Adds all the ancestors of a node, and builds the correct
**              hierarchy
**
** Arguments :  f_pcontextBuilder -
**              f_plistnode       - Linked list node containing the node whose
**                      ancestors are to be added to the builder tree
**
** Returns :    DRM_E_XB_OBJECT_ALREADY_EXISTS if the node already exists, or one
**
******************************************************************************/
static DRM_RESULT _XB_AddHierarchy(
    __inout_ecount( 1 ) DRM_XB_BUILDER_CONTEXT_INTERNAL    *f_pcontextBuilder,
    __inout_ecount( 1 ) DRM_XB_BUILDER_LISTNODE            *f_plistnode )
{
    /* Note: Internal function - No need to check input */
    DRM_RESULT                  dr              = DRM_SUCCESS;
    DRM_WORD                    wParent         = 0;
    DRM_XB_BUILDER_LISTNODE   *plistnodeChild   = f_plistnode;

    wParent = _DRM_XB_GetParent( f_plistnode, f_pcontextBuilder->pformat );
    ChkArg( _XB_IsKnownObjectType(f_pcontextBuilder->pformat, wParent) );

    /* This loop adds any ancestors not already included */
    while ( wParent != XB_OBJECT_TYPE_INVALID )
    {
        ChkArg( _XB_IsKnownObjectType(f_pcontextBuilder->pformat, wParent) );
        if ( f_pcontextBuilder->rgpObjectNodes[_XB_MapObjectTypeToEntryDescriptionIndex(f_pcontextBuilder->pformat, wParent)] == NULL )
        {
            DRM_XB_BUILDER_LISTNODE    *plistnode = NULL;

            /*
            **  Add object for this parent
            */
            ChkDR( DRM_STK_Alloc( &f_pcontextBuilder->contextStack,
                                  SIZEOF( DRM_XB_BUILDER_LISTNODE ),
                                  ( DRM_VOID * * ) &plistnode ) );
            plistnode->Node.wType     = wParent;
            ChkDR( DRM_DWordAdd( XB_BASE_OBJECT_LENGTH, plistnodeChild->Node.cbLength, &plistnode->Node.cbLength ) );
            plistnode->Node.pvObject  = ( const DRM_VOID * )plistnodeChild;
            plistnode->pNext          = NULL;
            f_pcontextBuilder->rgpObjectNodes[_XB_MapObjectTypeToEntryDescriptionIndex(f_pcontextBuilder->pformat, wParent)] = &plistnode->Node;
            plistnodeChild = plistnode;
            wParent = f_pcontextBuilder->pformat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(f_pcontextBuilder->pformat, wParent)].wParent;

        }
        else
        {
            DRM_XB_BUILDER_NODE    *pnodeParent =
                                f_pcontextBuilder->rgpObjectNodes[_XB_MapObjectTypeToEntryDescriptionIndex(f_pcontextBuilder->pformat, wParent)];
            if ( !_DRM_XB_IsDuplicateAllowed( plistnodeChild, f_pcontextBuilder->pformat ) )
            {
                const DRM_XB_BUILDER_LISTNODE    *plistnode = ( const DRM_XB_BUILDER_LISTNODE * )pnodeParent->pvObject;
                /*
                **  Check whether a duplicate exists
                */
                while( plistnode != NULL )
                {
                    if ( plistnode->Node.wType == plistnodeChild->Node.wType )
                    {
                        ChkDR( DRM_E_XB_OBJECT_ALREADY_EXISTS );
                    }
                    plistnode = plistnode->pNext;
                }
            }

            ChkDR( _InsertNodeInChildListSorted( f_pcontextBuilder, pnodeParent, plistnodeChild ) );
            ChkDR( DRM_DWordAdd( pnodeParent->cbLength, plistnodeChild->Node.cbLength, &pnodeParent->cbLength ) );
            break;
        }
    }

    /*
    **  Now update length of all ancestors of the current wParent
    */
    while ( wParent != XB_OBJECT_TYPE_INVALID
         && f_pcontextBuilder->pformat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(f_pcontextBuilder->pformat, wParent)].wParent != XB_OBJECT_TYPE_INVALID )
    {
        ChkDR( DRM_DWordAdd( f_pcontextBuilder->rgpObjectNodes[_XB_MapObjectTypeToEntryDescriptionIndex(f_pcontextBuilder->pformat, f_pcontextBuilder->pformat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(f_pcontextBuilder->pformat, wParent)].wParent)]->cbLength,
                             plistnodeChild->Node.cbLength,
                             &f_pcontextBuilder->rgpObjectNodes[_XB_MapObjectTypeToEntryDescriptionIndex(f_pcontextBuilder->pformat, f_pcontextBuilder->pformat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(f_pcontextBuilder->pformat, wParent)].wParent)]->cbLength ) );


        wParent = f_pcontextBuilder->pformat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(f_pcontextBuilder->pformat, wParent)].wParent;
    }

ErrorExit:
    return dr;
}

static DRM_RESULT _DRM_XB_AddContainer(
    __inout_ecount( 1 )    DRM_XB_BUILDER_CONTEXT    *f_pcontextBuilder,
    __in                   DRM_WORD                   f_wObjectType,
    __in_ecount( 1 ) const DRM_VOID                  *f_pvObject )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD iEntry;
    DRM_XB_BUILDER_CONTEXT_INTERNAL *pcontextBuilder =
                        ( DRM_XB_BUILDER_CONTEXT_INTERNAL * ) f_pcontextBuilder;

    for( iEntry = 0; iEntry < pcontextBuilder->pformat->cEntryDescriptions; iEntry++ )
    {
        const DRM_XB_ENTRY_DESCRIPTION *pContainerEntry = &(pcontextBuilder->pformat->pEntryDescriptions[iEntry]);
        if( pContainerEntry->wParent == f_wObjectType )
        {
            /* This is an object that is a direct child of the global header.
            ** Add the entry to the builder
            */
            if( pContainerEntry->fDuplicateAllowed )
            {
                /* Given there is a linked list here we have to do an extra deref */
                const DRM_XB_BASELIST *pObject   = NULL;
                DRM_VOID             **pvResult  = NULL;

                ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)f_pvObject, pContainerEntry->wOffsetInCurrentStruct, (DRM_DWORD_PTR*)&pvResult ) );
                pObject = (const DRM_XB_BASELIST*)*pvResult;

                while( pObject != NULL && pObject->fValid )
                {
                    ChkDR( DRM_XB_AddEntry( f_pcontextBuilder, pContainerEntry->wType, pObject ) );
                    pObject = pObject->pNext;
                }
            }
            else
            {
                const DRM_XB_EMPTY *pObject = NULL;

                ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)f_pvObject, pContainerEntry->wOffsetInCurrentStruct, (DRM_DWORD_PTR*)&pObject) );

                if( pObject->fValid )
                {
                    ChkDR( DRM_XB_AddEntry( f_pcontextBuilder, pContainerEntry->wType, pObject ) );
                }
            }
        }
    }
ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XB_AddEntry
**
** Synopsis :   Adds any object to the builder tree. This API can be used to add
**              either a leaf object, or a container structure
**
** Arguments :  f_pcontextBuilder   -
**              f_wObjectType   - type
**              f_pvObject      - Object passed as a void. This can be NULL, if
**                  the object is a container.
** Returns :    Unless a duplicate is permissible for this type, adding a
**              duplicate object will cause an error. If a container has been
**              explicitly added and then one of it's child nodes is added
**              separately, it is treated as a duplicate as well
**
** Notes :      The caller can add an empty container to a license.
**              The caller MUST make sure that no part of the object is freed,
**              or goes out of scope before the license is built. This is
**              because the internal builder context refers to the original
**              object ( instead of making a copy for efficiency reasons )
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_XB_AddEntry(
    __inout_ecount( 1 )    DRM_XB_BUILDER_CONTEXT    *f_pcontextBuilder,
    __in                   DRM_WORD                   f_wObjectType,
    __in_ecount( 1 ) const DRM_VOID                  *f_pvObject )
{
    DRM_RESULT                       dr              = DRM_SUCCESS;
    DRM_XB_BUILDER_LISTNODE         *plistnode       = NULL;
    DRM_XB_BUILDER_CONTEXT_INTERNAL *pcontextBuilder =
                        ( DRM_XB_BUILDER_CONTEXT_INTERNAL * ) f_pcontextBuilder;

    ChkArg( pcontextBuilder != NULL );

    ChkArg( _XB_IsKnownObjectType(pcontextBuilder->pformat, f_wObjectType) );
    ChkArg( f_pvObject != NULL );
    ChkArg( pcontextBuilder->rgpObjectNodes != NULL );

    if( f_wObjectType == XB_OBJECT_GLOBAL_HEADER
     || (pcontextBuilder->pformat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(pcontextBuilder->pformat, f_wObjectType)].wFlags & XB_FLAGS_CONTAINER) )
    {
        ChkDR( _DRM_XB_AddContainer( f_pcontextBuilder, f_wObjectType, f_pvObject ) );
    }
    else
    {
        ChkDR( DRM_STK_Alloc( &pcontextBuilder->contextStack,
                              SIZEOF( DRM_XB_BUILDER_LISTNODE ),
                              ( DRM_VOID ** )&plistnode ) );
        ChkDR( _DRM_XB_GetObjectLength( f_wObjectType, f_pvObject, pcontextBuilder->pformat, &plistnode->Node.cbLength ) );
        plistnode->Node.pvObject = f_pvObject;
        plistnode->Node.wType    = f_wObjectType;
        plistnode->pNext         = NULL;
        pcontextBuilder->rgpObjectNodes[_XB_MapObjectTypeToEntryDescriptionIndex(pcontextBuilder->pformat, f_wObjectType)] = &plistnode->Node;
        ChkDR( _XB_AddHierarchy( pcontextBuilder, plistnode ) );
    }

ErrorExit:
    return dr;
}



/******************************************************************************
**
** Function :   DRM_XB_StartFormat
**
** Synopsis :   Builder API used to initiate a builder context
**
** Arguments :  f_pbStack, f_cbStack - Stack used to allocate internal structures
**              f_pcontextBuilder
**
** Notes    :   Memory will not be freed from this stack. The caller should just
**              throw away this stack buffer after the license has been built.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_XB_StartFormat(
    __inout_bcount( f_cbStack ) DRM_BYTE                  *f_pbStack,
    __in                        DRM_DWORD                  f_cbStack,
    __in                        DRM_DWORD                  f_dwVersion,
    __inout_ecount( 1 )         DRM_XB_BUILDER_CONTEXT    *f_pcontextBuilder,
    __in                  const DRM_XB_FORMAT_DESCRIPTION *f_pformat )
{
    DRM_RESULT                          dr                   = DRM_SUCCESS;
    DRM_XB_BUILDER_NODE                *pnodeOuterContainer  = NULL;
    DRM_XB_BUILDER_CONTEXT_INTERNAL    *pcontextBuilder      = ( DRM_XB_BUILDER_CONTEXT_INTERNAL * )f_pcontextBuilder;
    DRM_DWORD                           dwResult             = 0;

    ChkArg( pcontextBuilder != NULL );

    pcontextBuilder->dwVersion              = f_dwVersion;
    pcontextBuilder->contextStack.cbStack   = f_cbStack;
    pcontextBuilder->contextStack.pbStack   = f_pbStack;
    pcontextBuilder->contextStack.nStackTop = 0;
    pcontextBuilder->pformat                = f_pformat;
    pcontextBuilder->pUnknownObjects        = NULL;

    /*
    **  Add a table for list of containers
    */
    ChkDR( DRM_DWordAdd( f_pformat->cEntryDescriptions, 1, &dwResult ) );
    ChkDR( DRM_DWordMult( SIZEOF( DRM_XB_BUILDER_NODE *), dwResult, &dwResult ) );

    ChkDR( DRM_STK_Alloc( &pcontextBuilder->contextStack,
                          dwResult,
                          ( DRM_VOID * * )&pcontextBuilder->rgpObjectNodes ) );
    ZEROMEM( pcontextBuilder->rgpObjectNodes,
             dwResult );

    /*
    **  Add the outer container object
    */
    ChkDR( DRM_STK_Alloc( &pcontextBuilder->contextStack,
                          SIZEOF( DRM_XB_BUILDER_NODE ),
                          ( DRM_VOID * * )&pnodeOuterContainer ) );
    pnodeOuterContainer->wType     = XB_OBJECT_GLOBAL_HEADER;

    pnodeOuterContainer->cbLength = 0;

    pnodeOuterContainer->pvObject  = NULL;
    pcontextBuilder->rgpObjectNodes[XB_OBJECT_GLOBAL_HEADER] = pnodeOuterContainer;

ErrorExit:
    return dr;
}


DRM_API DRM_RESULT DRM_CALL DRM_XB_FinishFormat (
    __in_ecount( 1 ) const              DRM_XB_BUILDER_CONTEXT *f_pcontextBuilder,
    __inout_bcount_opt( *f_pcbFormat )  DRM_BYTE               *f_pbFormat,
    __inout_ecount( 1 )                 DRM_DWORD              *f_pcbFormat )
{
    DRM_BYTE    *pbBuffer         = f_pbFormat;
    DRM_DWORD    cbFormat         = 0;
    DRM_DWORD    iFormat          = 0;
    DRM_RESULT   dr               = DRM_SUCCESS;
    DRM_WORD     iObject          = 0;
    DRM_DWORD    dwResult         = 0;

    const DRM_XB_BUILDER_CONTEXT_INTERNAL    *pcontextBuilder = ( const DRM_XB_BUILDER_CONTEXT_INTERNAL * ) f_pcontextBuilder;

    ChkArg( pcontextBuilder  != NULL );
    ChkArg( f_pcbFormat     != NULL );
    ChkArg( pcontextBuilder->rgpObjectNodes != NULL );

    /*
    **  Ensure that required objects were added
    */
    for( iObject = 0;
         iObject < pcontextBuilder->pformat->cEntryDescriptions;
         iObject++ )
    {
        if(  pcontextBuilder->rgpObjectNodes[iObject] == NULL
         && !pcontextBuilder->pformat->pEntryDescriptions[iObject].fOptional )
        {
            ChkDR( DRM_E_XB_REQUIRED_OBJECT_MISSING );
        }
    }

    ChkDR( DRM_DWordAdd( XB_HEADER_LENGTH( pcontextBuilder->pformat->pHeaderDescription->eFormatIdLength ),
                         pcontextBuilder->rgpObjectNodes[XB_OBJECT_GLOBAL_HEADER]->cbLength,
                         &cbFormat ) );

    if ( *f_pcbFormat < cbFormat || f_pbFormat == NULL )
    {
        *f_pcbFormat = cbFormat;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }
    *f_pcbFormat = cbFormat;

    /* Serialize the Header */
    switch( pcontextBuilder->pformat->pHeaderDescription->eFormatIdLength )
    {
    case XB_FORMAT_ID_LENGTH_DWORD:
        DWORD_TO_NETWORKBYTES( pbBuffer, iFormat, DRM_UI64Low32( pcontextBuilder->pformat->pHeaderDescription->qwFormatIdentifier ) );
        ChkDR( DRM_DWordAdd( iFormat, XB_FORMAT_ID_LENGTH_DWORD, &iFormat ) );
        break;
    case XB_FORMAT_ID_LENGTH_QWORD:
        QWORD_TO_NETWORKBYTES( pbBuffer, iFormat, pcontextBuilder->pformat->pHeaderDescription->qwFormatIdentifier );
        ChkDR( DRM_DWordAdd( iFormat, XB_FORMAT_ID_LENGTH_QWORD, &iFormat ) );
        break;
    default:
        ChkDR( DRM_E_XB_INVALID_OBJECT );
        break;
    }

    DWORD_TO_NETWORKBYTES( pbBuffer, iFormat, pcontextBuilder->pformat->pHeaderDescription->dwFormatVersion );
    ChkDR( DRM_DWordAdd( iFormat, SIZEOF( DRM_DWORD ), &iFormat ) );

    /* Leave room for the final size to be updated after the rest of the serialization */
    ChkDR( DRM_DWordAdd( iFormat, SIZEOF( DRM_DWORD ), &iFormat ) );

    /* Serialize the Outer Container */
    ChkDR( DRM_DWordAdd( XB_BASE_OBJECT_LENGTH, iFormat, &dwResult ) );
    ChkBOOL( dwResult <= *f_pcbFormat, DRM_E_BUFFERTOOSMALL );

    ChkDR( _DRM_XB_Serialize_Container( pcontextBuilder->rgpObjectNodes[XB_OBJECT_GLOBAL_HEADER]->pvObject,
                                        pbBuffer,
                                       *f_pcbFormat,
                                       &iFormat,
                                        pcontextBuilder->pformat) );

    /* Update the total object size in the format header */
    ChkDR( DRM_DWordSub( XB_HEADER_LENGTH( pcontextBuilder->pformat->pHeaderDescription->eFormatIdLength ), SIZEOF( DRM_DWORD ), &dwResult ) );
    DWORD_TO_NETWORKBYTES( pbBuffer, dwResult, iFormat );

    *f_pcbFormat = iFormat;

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;
