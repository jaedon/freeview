/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmxb.h>
#include <drmxbbuilder.h>

ENTER_PK_NAMESPACE_CODE;


DRM_BOOL DRM_XB_IsDuplicateAllowed(
    __in_ecount( 1 ) const DRM_XB_BUILDER_LISTNODE   *f_plistnode,
    __in             const DRM_XB_FORMAT_DESCRIPTION *f_pformat );

DRM_WORD DRM_XB_GetParent(
    __in_ecount( 1 ) const DRM_XB_BUILDER_LISTNODE   *f_plistnode,
    __in             const DRM_XB_FORMAT_DESCRIPTION *f_pformat );

DRM_API DRM_DWORD DRM_CALL DRM_XB_GetObjectLength(
    __in             DRM_WORD    f_wType,
    __in_opt const   DRM_VOID   *f_pvObject,
    __in     const   DRM_XB_FORMAT_DESCRIPTION *f_pformat );

static DRM_BOOL DRM_XB_IsNativeObject( __in DRM_WORD f_wType, __in const DRM_XB_FORMAT_DESCRIPTION *f_pFormat )
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

DRM_BOOL DRM_XB_IsDuplicateAllowed( __in_ecount( 1 ) const DRM_XB_BUILDER_LISTNODE *f_plistnode, __in const DRM_XB_FORMAT_DESCRIPTION *f_pFormat )
{
    /* Note: Internal function - No need to check input */
    DRM_BOOL fRetVal = FALSE;
    if( DRM_XB_IsNativeObject( f_plistnode->Node.wType, f_pFormat ) )
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
** Function :   DRM_XB_GetParent
**
** Synopsis :   Returns the parent object type for a given node.
**
** Arguments :  plistnode - Node for which parent needs to be found
**
******************************************************************************/

DRM_WORD DRM_XB_GetParent( __in_ecount( 1 ) const DRM_XB_BUILDER_LISTNODE *f_plistnode, __in const DRM_XB_FORMAT_DESCRIPTION *f_pFormat )
{
    /* Note: Internal function - No need to check input */
    DRM_WORD wRetVal = XB_OBJECT_TYPE_INVALID;
    if( DRM_XB_IsNativeObject( f_plistnode->Node.wType, f_pFormat ) )
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

    if( DRM_XB_IsNativeObject( f_wType, f_pcontextBuilder->pformat ) )
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
** Function :   DRM_XB_GetObjectLength
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
DRM_API DRM_DWORD DRM_CALL DRM_XB_GetObjectLength(
    __in             DRM_WORD                          f_wType,
    __in_opt const   DRM_VOID                         *f_pvObject,
    __in     const   DRM_XB_FORMAT_DESCRIPTION        *f_pFormat)
{
    DRM_DWORD cbLength = XB_BASE_OBJECT_LENGTH;
    const DRM_XB_ENTRY_DESCRIPTION   *pEntry   = NULL;
    const DRM_XB_ELEMENT_DESCRIPTION *pElement = NULL;
    DRM_DWORD iElement = 0;

    if( !_XB_IsKnownObjectType(f_pFormat, f_wType) )
    {
        return 0;
    }

    pEntry = &(f_pFormat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(f_pFormat, f_wType)]);
    if( pEntry->wFlags & XB_FLAGS_CONTAINER )
    {
        /*
        **  For containers we just return the base object length
        */
        return cbLength;
    }

    /*
    **  Loop through each of the elements in the entry and sum up the length
    */
    for( iElement = 0; iElement < pEntry->cContainerOrObjectDescriptions; iElement++ )
    {
        const DRM_XB_ELEMENT_DESCRIPTION *pElementArray = (DRM_XB_ELEMENT_DESCRIPTION*)pEntry->pContainerOrObjectDescriptions;
        DRM_VOID *pobject = NULL;
        pElement = &(pElementArray[iElement]);
        pobject = (((DRM_BYTE*)f_pvObject) + pElement->wOffsetInCurrentStruct);
        switch( pElement->eElementType )
        {
        case XB_ELEMENT_TYPE_WORD:
            cbLength += SIZEOF( DRM_WORD );
            break;
        case XB_ELEMENT_TYPE_DWORD:
            cbLength += SIZEOF( DRM_DWORD );
            break;
        case XB_ELEMENT_TYPE_QWORD:
            cbLength += SIZEOFQWORD;
            break;
        case XB_ELEMENT_TYPE_GUID:
            cbLength += SIZEOF( DRM_GUID );
            break;
        case XB_ELEMENT_TYPE_GUIDLIST:
            cbLength += ( ( DRM_XB_GUIDLIST * )pobject )->cGUIDs * SIZEOF( DRM_GUID );
            cbLength += SIZEOF( DRM_DWORD );
            break;
        case XB_ELEMENT_TYPE_WORDLIST:
            cbLength += ( ( DRM_XB_WORDLIST * )pobject )->cWORDs * SIZEOF( DRM_WORD );
            cbLength += SIZEOF( DRM_DWORD );
            break;
        case XB_ELEMENT_TYPE_BYTEARRAY:
            cbLength += ( ( DRM_XB_BYTEARRAY * )pobject )->cbData;
            if( pElement->xbSize.eSizeType == XB_SIZE_IS_RELATIVE_WORD )
            {
                cbLength += SIZEOF( DRM_WORD );
            }
            else if( pElement->xbSize.eSizeType == XB_SIZE_IS_RELATIVE_DWORD )
            {
                cbLength += SIZEOF( DRM_DWORD );
            }
            break;
        default:
            break;
        }
    }

    return cbLength;
}


static DRM_VOID _DRM_XB_Serialize_UnknownBaseObject(
    __in                                                   DRM_WORD    f_wType,
    __in                                                   DRM_WORD    f_wFlags,
    __in                                                   DRM_DWORD   f_dwLength,
    __inout_bcount( XB_BASE_OBJECT_LENGTH + *f_piBuffer )  DRM_BYTE   *f_pbBuffer,
    __inout_ecount( 1 )                                    DRM_DWORD  *f_piBuffer )
{
    DRM_DWORD dwIndex = *f_piBuffer;

    WORD_TO_NETWORKBYTES(  f_pbBuffer, dwIndex, f_wFlags );    dwIndex += SIZEOF( DRM_WORD );
    WORD_TO_NETWORKBYTES(  f_pbBuffer, dwIndex, f_wType );     dwIndex += SIZEOF( DRM_WORD );
    DWORD_TO_NETWORKBYTES( f_pbBuffer, dwIndex, f_dwLength );  dwIndex += SIZEOF( DRM_DWORD );
    *f_piBuffer = dwIndex;
}

/*
** Serialize the basic type, flags, and length
*/
DRM_API DRM_VOID DRM_CALL DRM_XB_Serialize_BaseObject(
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

DRM_API DRM_RESULT DRM_CALL DRM_XB_Serialize_WORD(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT      dr     = DRM_SUCCESS;
    const DRM_WORD *pword  = ( DRM_WORD* )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );

    ChkBOOL( f_cbBuffer >= *f_piBuffer + SIZEOF( DRM_WORD ),
             DRM_E_BUFFERTOOSMALL );

    WORD_TO_NETWORKBYTES( f_pbBuffer, *f_piBuffer, *pword );
    *f_piBuffer += SIZEOF( DRM_WORD );

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_XB_Serialize_DWORD(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT       dr      = DRM_SUCCESS;
    const DRM_DWORD *pdword  = ( DRM_DWORD* )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );

    ChkBOOL( f_cbBuffer >= *f_piBuffer + SIZEOF( DRM_DWORD ),
             DRM_E_BUFFERTOOSMALL );

    DWORD_TO_NETWORKBYTES( f_pbBuffer, *f_piBuffer, *pdword );
    *f_piBuffer += SIZEOF( DRM_DWORD );

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_XB_Serialize_QWORD(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT        dr      = DRM_SUCCESS;
    const DRM_UINT64 *pqword  = ( DRM_UINT64* )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );

    ChkBOOL( f_cbBuffer >= *f_piBuffer + SIZEOFQWORD,
             DRM_E_BUFFERTOOSMALL );

    QWORD_TO_NETWORKBYTES( f_pbBuffer, *f_piBuffer, *pqword );
    *f_piBuffer += SIZEOFQWORD;

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_XB_Serialize_GUID(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT       dr    = DRM_SUCCESS;
    DRM_GUID    *pguid = ( DRM_GUID * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );

    ChkBOOL( f_cbBuffer >= *f_piBuffer + SIZEOF( DRM_GUID ),
             DRM_E_BUFFERTOOSMALL );

    DRM_BYT_CopyBytes( f_pbBuffer,
                       *f_piBuffer,
                       pguid,
                       0,
                       SIZEOF( DRM_GUID ) );
    *f_piBuffer += SIZEOF( DRM_GUID );

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_XB_Serialize_GUIDLIST(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT           dr        = DRM_SUCCESS;
    DRM_DWORD            iBuffer   = 0;
    DRM_DWORD            iGuids    = 0;
    DRM_XB_GUIDLIST    *pguidlist = ( DRM_XB_GUIDLIST * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );
    ChkArg( pguidlist->pguidBuffer != NULL );

    iBuffer = *f_piBuffer;

    ChkBOOL( f_cbBuffer >= iBuffer + SIZEOF( DRM_DWORD ) + SIZEOF( DRM_GUID ) * pguidlist->cGUIDs,
             DRM_E_BUFFERTOOSMALL );

    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, pguidlist->cGUIDs );
    iBuffer += SIZEOF( DRM_DWORD );

    for ( iGuids = 0; iGuids < pguidlist->cGUIDs; iGuids++ )
    {
        DRM_BYT_CopyBytes( f_pbBuffer,
                           iBuffer,
                           pguidlist->pguidBuffer,
                           pguidlist->iGuids + iGuids*SIZEOF( DRM_GUID ),
                           SIZEOF( DRM_GUID ) );
        iBuffer += SIZEOF( DRM_GUID );
    }
    *f_piBuffer = iBuffer;

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_XB_Serialize_WORDLIST(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT           dr        = DRM_SUCCESS;
    DRM_DWORD            iBuffer   = 0;
    DRM_DWORD            iWords    = 0;
    DRM_XB_WORDLIST     *pwordlist = ( DRM_XB_WORDLIST * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );
    ChkArg( pwordlist->pwordBuffer != NULL );

    iBuffer = *f_piBuffer;

    ChkBOOL( f_cbBuffer >= iBuffer + SIZEOF( DRM_DWORD ) + SIZEOF( DRM_WORD ) * pwordlist->cWORDs,
             DRM_E_BUFFERTOOSMALL );

    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, pwordlist->cWORDs );
    iBuffer += SIZEOF( DRM_DWORD );

    for ( iWords = 0; iWords < pwordlist->cWORDs; iWords++ )
    {
        ChkDR( DRM_XB_Serialize_WORD( pwordlist->pwordBuffer + pwordlist->iWords + iWords * SIZEOF( DRM_WORD ),
                                      f_pbBuffer,
                                      f_cbBuffer,
                                      &iBuffer ) );
    }
    *f_piBuffer = iBuffer;

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_XB_Serialize_BYTEARRAY(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT          dr  = DRM_SUCCESS;
    DRM_XB_BYTEARRAY   *pbytearray = ( DRM_XB_BYTEARRAY * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );
    ChkArg( pbytearray->cbData == 0 || pbytearray->pbDataBuffer != NULL );

    ChkBOOL( f_cbBuffer >= *f_piBuffer + pbytearray->cbData,
             DRM_E_BUFFERTOOSMALL );

    DRM_BYT_CopyBytes( f_pbBuffer,
                       *f_piBuffer,
                       pbytearray->pbDataBuffer,
                       pbytearray->iData,
                       pbytearray->cbData );
    *f_piBuffer += pbytearray->cbData;

ErrorExit:
    return dr;
}

/*
**  Serialize a while object.  The object header has already been serialized which means
**  just loop through all elements in the object and serailize each one.
*/
DRM_API DRM_RESULT DRM_CALL DRM_XB_Serialize_Object(
    __in_ecount(1)         const DRM_XB_ENTRY_DESCRIPTION *f_pEntry,
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT                  dr        = DRM_SUCCESS;
    DRM_XB_ELEMENT_DESCRIPTION *pElements = NULL;
    DRM_DWORD                   iElement  = 0;

    ChkArg( f_pEntry   != NULL );
    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );

    pElements = (DRM_XB_ELEMENT_DESCRIPTION*) f_pEntry->pContainerOrObjectDescriptions;

    for( iElement = 0; iElement < f_pEntry->cContainerOrObjectDescriptions; iElement++ )
    {
        DRM_VOID *pobject = ((DRM_BYTE*)f_pvObject + pElements[iElement].wOffsetInCurrentStruct);
        switch( pElements[iElement].eElementType )
        {
        case XB_ELEMENT_TYPE_WORD:          ChkDR( DRM_XB_Serialize_WORD     ( pobject, f_pbBuffer, f_cbBuffer, f_piBuffer ) ); break;
        case XB_ELEMENT_TYPE_DWORD:         ChkDR( DRM_XB_Serialize_DWORD    ( pobject, f_pbBuffer, f_cbBuffer, f_piBuffer ) ); break;
        case XB_ELEMENT_TYPE_QWORD:         ChkDR( DRM_XB_Serialize_QWORD    ( pobject, f_pbBuffer, f_cbBuffer, f_piBuffer ) ); break;
        case XB_ELEMENT_TYPE_GUID:          ChkDR( DRM_XB_Serialize_GUID     ( pobject, f_pbBuffer, f_cbBuffer, f_piBuffer ) ); break;
        case XB_ELEMENT_TYPE_GUIDLIST:      ChkDR( DRM_XB_Serialize_GUIDLIST ( pobject, f_pbBuffer, f_cbBuffer, f_piBuffer ) ); break;
        case XB_ELEMENT_TYPE_WORDLIST:      ChkDR( DRM_XB_Serialize_WORDLIST ( pobject, f_pbBuffer, f_cbBuffer, f_piBuffer ) ); break;
        case XB_ELEMENT_TYPE_BYTEARRAY:
            if( pElements[iElement].xbSize.eSizeType == XB_SIZE_IS_RELATIVE_WORD )
            {
                /* If it's a relative size the WORD needs to be serialized as well */
                DRM_XB_BYTEARRAY   *pbytearray = ( DRM_XB_BYTEARRAY * )pobject;
                DRM_WORD            word       = ( DRM_WORD )pbytearray->cbData;

                ChkDR( DRM_XB_Serialize_WORD     ( &word, f_pbBuffer, f_cbBuffer, f_piBuffer ) );
            }
            else if( pElements[iElement].xbSize.eSizeType == XB_SIZE_IS_RELATIVE_DWORD )
            {
                /* If it's a relative size the DWORD needs to be serialized as well */
                DRM_XB_BYTEARRAY   *pbytearray = ( DRM_XB_BYTEARRAY * )pobject;
                DRM_DWORD           dword      = pbytearray->cbData;

                ChkDR( DRM_XB_Serialize_DWORD     ( &dword, f_pbBuffer, f_cbBuffer, f_piBuffer ) );
            }
            ChkDR( DRM_XB_Serialize_BYTEARRAY( pobject, f_pbBuffer, f_cbBuffer, f_piBuffer ) ); break;
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
DRM_API DRM_RESULT DRM_CALL DRM_XB_Serialize_Container(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer,
    __in                   const DRM_XB_FORMAT_DESCRIPTION   *f_pFormat)
{
    DRM_RESULT                 dr          = DRM_SUCCESS;
    DRM_XB_BUILDER_LISTNODE  *pnodeChild  = ( DRM_XB_BUILDER_LISTNODE * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );

    /* Serialize all the child nodes */
    while ( pnodeChild != NULL )
    {
        if( DRM_XB_IsNativeObject( pnodeChild->Node.wType, f_pFormat ) )
        {
             ChkBOOL( f_cbBuffer >= *f_piBuffer + XB_BASE_OBJECT_LENGTH,
                      DRM_E_BUFFERTOOSMALL );

            DRM_XB_Serialize_BaseObject( pnodeChild->Node.wType,
                                         pnodeChild->Node.cbLength,
                                         f_pbBuffer,
                                         f_piBuffer,
                                         f_pFormat );

            if ( pnodeChild->Node.pvObject != NULL )
            {
                if( f_pFormat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(f_pFormat, pnodeChild->Node.wType)].wFlags & XB_FLAGS_CONTAINER )
                {
                    /* Recurse for the contained container */
                    ChkDR( DRM_XB_Serialize_Container( pnodeChild->Node.pvObject, f_pbBuffer, f_cbBuffer, f_piBuffer, f_pFormat ) );
                }
                else
                {
                    ChkDR( DRM_XB_Serialize_Object( &(f_pFormat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(f_pFormat, pnodeChild->Node.wType)]),
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


DRM_RESULT _DRM_XB_InsertObject(
    IN  OUT DRM_XB_BUILDER_CONTEXT_INTERNAL    *f_pcontextBuilder,
    IN      DRM_XB_OBJECT                      *f_pobject,
    IN      DRM_WORD                            f_wparenttype )

{
    DRM_RESULT                                     dr             = DRM_SUCCESS;
    DRM_XB_BUILDER_LISTNODE                       *pUnklistnode   = NULL;
    DRM_BYTE                                      *pbUnkXBObject = NULL;
    DRM_DWORD                                      cbUnkXBObject = 0;
    ChkArg( f_pcontextBuilder != NULL );
    ChkArg( f_pobject != NULL );

    if ( f_pobject->fValid )
    {
        ChkDR( DRM_STK_Alloc( &f_pcontextBuilder->contextStack,
                  SIZEOF( DRM_XB_BUILDER_LISTNODE ),
                  ( DRM_VOID ** )&pUnklistnode ) );

        cbUnkXBObject = f_pobject->cbData;
        ChkOverflow( cbUnkXBObject, f_pobject->cbData );

        ChkDR( DRM_STK_Alloc( &f_pcontextBuilder->contextStack,
                  cbUnkXBObject,
                  ( DRM_VOID ** )&pbUnkXBObject ) );

        DRM_BYT_CopyBytes( pbUnkXBObject,
                           0,
                           f_pobject->pbBuffer,
                           f_pobject->ibData,
                           f_pobject->cbData );

        ZEROMEM( pUnklistnode, SIZEOF( DRM_XB_BUILDER_LISTNODE ) );
        pUnklistnode->Node.wType      = f_pobject->wType;
        pUnklistnode->Node.cbLength   = cbUnkXBObject + XB_BASE_OBJECT_LENGTH;
        pUnklistnode->Node.pvObject   = pbUnkXBObject;
        pUnklistnode->pNext           = NULL;

        pUnklistnode->pNext    = f_pcontextBuilder->pUnknownObjects;
        f_pcontextBuilder->pUnknownObjects = pUnklistnode;

        if ( DRM_XB_IsNativeObject( f_wparenttype, f_pcontextBuilder->pformat ) )
        {
            pUnklistnode->pNext = ( DRM_XB_BUILDER_LISTNODE * )f_pcontextBuilder->rgpObjectNodes[_XB_MapObjectTypeToEntryDescriptionIndex(f_pcontextBuilder->pformat, f_wparenttype)]->pvObject;
            f_pcontextBuilder->rgpObjectNodes[_XB_MapObjectTypeToEntryDescriptionIndex(f_pcontextBuilder->pformat, f_wparenttype)]->pvObject = ( DRM_VOID * )&( pUnklistnode );
            f_pcontextBuilder->rgpObjectNodes[_XB_MapObjectTypeToEntryDescriptionIndex(f_pcontextBuilder->pformat, f_wparenttype)]->cbLength += pUnklistnode->Node.cbLength;
        }
        else
        {
            DRMASSERT(!"Unexpected");
        }
    }

ErrorExit:
    return dr;
}

/*
**  Exists to insert the new node in a sorted manner if the format dictates that there is an order to which the elements should appear.
**  Order is deteremined by DRM_XB_ENTRY_DESCRIPTION.wBuilderSortOrder
*/
DRM_RESULT _InsertNodeInChildListSorted(
    __inout_ecount( 1 ) DRM_XB_BUILDER_CONTEXT_INTERNAL    *f_pcontextBuilder,
    DRM_XB_BUILDER_NODE     *pnodeParent,
    DRM_XB_BUILDER_LISTNODE *plistnodeChild )
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
        pnodeParent->pvObject = ( DRM_VOID * )plistnodeChild;
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
    DRM_XB_BUILDER_LISTNODE   *plistnodeChild  = f_plistnode;

    wParent = DRM_XB_GetParent( f_plistnode, f_pcontextBuilder->pformat );
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
            plistnode->Node.cbLength  = XB_BASE_OBJECT_LENGTH
                                      + plistnodeChild->Node.cbLength;
            plistnode->Node.pvObject  = ( DRM_VOID * )plistnodeChild;
            plistnode->pNext          = NULL;
            f_pcontextBuilder->rgpObjectNodes[_XB_MapObjectTypeToEntryDescriptionIndex(f_pcontextBuilder->pformat, wParent)] = &plistnode->Node;
            plistnodeChild = plistnode;
            wParent = f_pcontextBuilder->pformat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(f_pcontextBuilder->pformat, wParent)].wParent;

        }
        else
        {
            DRM_XB_BUILDER_NODE    *pnodeParent =
                                f_pcontextBuilder->rgpObjectNodes[_XB_MapObjectTypeToEntryDescriptionIndex(f_pcontextBuilder->pformat, wParent)];
            if ( !DRM_XB_IsDuplicateAllowed( plistnodeChild, f_pcontextBuilder->pformat ) )
            {
                DRM_XB_BUILDER_LISTNODE    *plistnode =
                            ( DRM_XB_BUILDER_LISTNODE * )pnodeParent->pvObject;
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
            pnodeParent->cbLength += plistnodeChild->Node.cbLength;
            break;
        }
    }

    /*
    **  Now update length of all ancestors of the current wParent
    */
    while ( wParent != XB_OBJECT_TYPE_INVALID
         && f_pcontextBuilder->pformat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(f_pcontextBuilder->pformat, wParent)].wParent != XB_OBJECT_TYPE_INVALID )
    {
        f_pcontextBuilder->rgpObjectNodes[_XB_MapObjectTypeToEntryDescriptionIndex(f_pcontextBuilder->pformat, f_pcontextBuilder->pformat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(f_pcontextBuilder->pformat, wParent)].wParent)]->cbLength +=
                                                plistnodeChild->Node.cbLength;
        wParent = f_pcontextBuilder->pformat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(f_pcontextBuilder->pformat, wParent)].wParent;
    }

ErrorExit:
    return dr;
}

DRM_RESULT DRM_XB_AddContainer(
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
                DRM_XB_BASELIST *pObject = (DRM_XB_BASELIST*)*((DRM_VOID**)(((DRM_BYTE*)f_pvObject) + pContainerEntry->wOffsetInCurrentStruct));
                while( pObject != NULL && pObject->fValid )
                {
                    ChkDR( DRM_XB_AddEntry( f_pcontextBuilder, pContainerEntry->wType, pObject ) );
                    pObject = pObject->pNext;
                }
            }
            else
            {
                DRM_XB_EMPTY *pObject = (DRM_XB_EMPTY*)(((DRM_BYTE*)f_pvObject) + pContainerEntry->wOffsetInCurrentStruct);
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
        ChkDR( DRM_XB_AddContainer( f_pcontextBuilder, f_wObjectType, f_pvObject ) );
    }
    else
    {
        ChkDR( DRM_STK_Alloc( &pcontextBuilder->contextStack,
                              SIZEOF( DRM_XB_BUILDER_LISTNODE ),
                              ( DRM_VOID ** )&plistnode ) );
        plistnode->Node.cbLength = DRM_XB_GetObjectLength( f_wObjectType, f_pvObject, pcontextBuilder->pformat );
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
    DRM_RESULT                           dr  = DRM_SUCCESS;
    DRM_XB_BUILDER_NODE                *pnodeOuterContainer  = NULL;
    DRM_XB_BUILDER_CONTEXT_INTERNAL    *pcontextBuilder =
                            ( DRM_XB_BUILDER_CONTEXT_INTERNAL * )f_pcontextBuilder;

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
    ChkDR( DRM_STK_Alloc( &pcontextBuilder->contextStack,
                          SIZEOF( DRM_XB_BUILDER_NODE * ) * ( f_pformat->cEntryDescriptions + 1 ),
                          ( DRM_VOID * * )&pcontextBuilder->rgpObjectNodes ) );
    ZEROMEM( pcontextBuilder->rgpObjectNodes,
             SIZEOF( DRM_XB_BUILDER_NODE * ) * ( f_pformat->cEntryDescriptions + 1 ) );

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

    DRM_XB_BUILDER_CONTEXT_INTERNAL    *pcontextBuilder =
                            ( DRM_XB_BUILDER_CONTEXT_INTERNAL * ) f_pcontextBuilder;

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

    cbFormat = XB_HEADER_LENGTH
              + pcontextBuilder->rgpObjectNodes[XB_OBJECT_GLOBAL_HEADER]->cbLength;

    if ( *f_pcbFormat < cbFormat || f_pbFormat == NULL )
    {
        *f_pcbFormat = cbFormat;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }
    *f_pcbFormat = cbFormat;

    /* Serialize the Header */
    DWORD_TO_NETWORKBYTES( pbBuffer, iFormat, pcontextBuilder->pformat->pHeaderDescription->dwFormatIdentifier );
    iFormat += sizeof( DRM_DWORD );

    DWORD_TO_NETWORKBYTES( pbBuffer, iFormat, pcontextBuilder->pformat->pHeaderDescription->dwFormatVersion );
    iFormat += sizeof( DRM_DWORD );

    /* Leave room for the final size to be updated after the rest of the serialization */
    iFormat += sizeof( DRM_DWORD );

    /* Serialize the Outer Container */
    ChkBOOL( XB_BASE_OBJECT_LENGTH + iFormat <= *f_pcbFormat, DRM_E_BUFFERTOOSMALL );

    ChkDR( DRM_XB_Serialize_Container( pcontextBuilder->rgpObjectNodes[XB_OBJECT_GLOBAL_HEADER]->pvObject,
                                       pbBuffer,
                                      *f_pcbFormat,
                                      &iFormat,
                                       pcontextBuilder->pformat) );

    /* Update the total object size in the format header */
    DWORD_TO_NETWORKBYTES( pbBuffer, SIZEOF(DRM_DWORD)*2, iFormat );

    *f_pcbFormat = iFormat;

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;
