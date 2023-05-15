/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMXBPARSER_C
#include <drmcontextsizes.h>
#include <byteorder.h>
#include <drmxbparser.h>
#include <drmmathsafe.h>
#include <drmlastinclude.h>

PRAGMA_STRICT_GS_PUSH_ON;

ENTER_PK_NAMESPACE_CODE;


/*********************************************************************
**
** Function: _DRM_XB_Parse_WORD
**
** Synopsis: Internal API that deserializes a WORD value from a XB stream
**
** Arguments:
**
** [f_pbBuffer]         -- Pointer to the buffer to read from
** [f_piObject]         -- Index in the buffer to start reading from
** [f_pcbObject]        -- Number of bytes reamaining in the buffer (updated on exit)
** [f_pvObject]         -- WORD to store the deserialized result
**
** Returns:            DRM_SUCESS on success.
**                     DRM_E_XB_INVALID_OBJECT is there is an issue with the object size.
**
**********************************************************************/
static DRMFORCEINLINE DRM_RESULT DRM_CALL _DRM_XB_Parse_WORD(
    __in                         const DRM_BYTE    *f_pbBuffer,
    __in                               DRM_DWORD   *f_piObject,
    __in                               DRM_DWORD   *f_pcbObject,
    __inout_bcount(*f_pcbObject)       DRM_VOID    *f_pvObject )
{
    DRM_RESULT    dr    = DRM_SUCCESS;
    DRM_WORD *pword = ( DRM_WORD * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( *f_pcbObject >= SIZEOF( DRM_WORD ), DRM_E_XB_INVALID_OBJECT );

    NETWORKBYTES_TO_WORD( *pword,
                          f_pbBuffer,
                          *f_piObject );

    ChkDR( DRM_DWordAdd( *f_piObject, SIZEOF( DRM_WORD ), f_piObject ) );
    ChkDR( DRM_DWordSub( *f_pcbObject, SIZEOF( DRM_WORD ), f_pcbObject ) );

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: _DRM_XB_Parse_DWORD
**
** Synopsis: Internal API that deserializes a DWORD value from a XB stream
**
** Arguments:
**
** [f_pbBuffer]         -- Pointer to the buffer to read from
** [f_piObject]         -- Index in the buffer to start reading from
** [f_pcbObject]        -- Number of bytes reamaining in the buffer (updated on exit)
** [f_pvObject]         -- DWORD to store the deserialized result
**
** Returns:            DRM_SUCESS on success.
**                     DRM_E_XB_INVALID_OBJECT is there is an issue with the object size.
**
**********************************************************************/
static DRMFORCEINLINE DRM_RESULT DRM_CALL _DRM_XB_Parse_DWORD(
    __in                         const DRM_BYTE    *f_pbBuffer,
    __in                               DRM_DWORD   *f_piObject,
    __in                               DRM_DWORD   *f_pcbObject,
    __inout_bcount(*f_pcbObject)       DRM_VOID    *f_pvObject )
{
    DRM_RESULT     dr     = DRM_SUCCESS;
    DRM_DWORD *pdword = ( DRM_DWORD * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( *f_pcbObject >= SIZEOF( DRM_DWORD ), DRM_E_XB_INVALID_OBJECT );

    NETWORKBYTES_TO_DWORD( *pdword,
                           f_pbBuffer,
                           *f_piObject);

    ChkDR( DRM_DWordAdd( *f_piObject, SIZEOF( DRM_DWORD ), f_piObject ) );
    ChkDR( DRM_DWordSub( *f_pcbObject, SIZEOF( DRM_DWORD ), f_pcbObject ) );

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: _DRM_XB_Parse_QWORD
**
** Synopsis: Internal API that deserializes a QWORD value from a XB stream
**
** Arguments:
**
** [f_pbBuffer]         -- Pointer to the buffer to read from
** [f_piObject]         -- Index in the buffer to start reading from
** [f_pcbObject]        -- Number of bytes reamaining in the buffer (updated on exit)
** [f_pvObject]         -- QWORD to store the deserialized result
**
** Returns:            DRM_SUCESS on success.
**                     DRM_E_XB_INVALID_OBJECT is there is an issue with the object size.
**
**********************************************************************/
static DRMFORCEINLINE DRM_RESULT DRM_CALL _DRM_XB_Parse_QWORD(
    __in                         const DRM_BYTE    *f_pbBuffer,
    __in                               DRM_DWORD   *f_piObject,
    __in                               DRM_DWORD   *f_pcbObject,
    __inout_bcount(*f_pcbObject)       DRM_VOID    *f_pvObject )
{
    DRM_RESULT     dr     = DRM_SUCCESS;
    DRM_UINT64 *pqword    = ( DRM_UINT64 * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( *f_pcbObject >= SIZEOF( DRM_DWORD ), DRM_E_XB_INVALID_OBJECT );

    NETWORKBYTES_TO_QWORD( *pqword,
                            f_pbBuffer,
                           *f_piObject);

    ChkDR( DRM_DWordAdd( *f_piObject, SIZEOFQWORD, f_piObject ) );
    ChkDR( DRM_DWordSub( *f_pcbObject, SIZEOFQWORD, f_pcbObject ) );

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: _DRM_XB_Parse_GUID
**
** Synopsis: Internal API that deserializes a GUID value from a XB stream
**
** Arguments:
**
** [f_pbBuffer]         -- Pointer to the buffer to read from
** [f_piObject]         -- Index in the buffer to start reading from
** [f_pcbObject]        -- Number of bytes reamaining in the buffer (updated on exit)
** [f_pvObject]         -- GUID to store the deserialized result
**
** Returns:            DRM_SUCESS on success.
**                     DRM_E_XB_INVALID_OBJECT is there is an issue with the object size.
**
**********************************************************************/
static DRMFORCEINLINE DRM_RESULT DRM_CALL _DRM_XB_Parse_GUID(
    __in                         const DRM_BYTE    *f_pbBuffer,
    __in                               DRM_DWORD   *f_piObject,
    __in                               DRM_DWORD   *f_pcbObject,
    __inout_bcount(*f_pcbObject)       DRM_VOID    *f_pvObject )
{
    DRM_RESULT    dr    = DRM_SUCCESS;
    DRM_GUID *pguid = ( DRM_GUID * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( *f_pcbObject >= SIZEOF( DRM_GUID ), DRM_E_XB_INVALID_OBJECT );

    DRM_BYT_CopyBytes( pguid, 0, f_pbBuffer, *f_piObject, SIZEOF(DRM_GUID) );

    ChkDR( DRM_DWordAdd( *f_piObject, SIZEOF( DRM_GUID ), f_piObject ) );
    ChkDR( DRM_DWordSub( *f_pcbObject, SIZEOF( DRM_GUID ), f_pcbObject ) );

ErrorExit:
    return dr;
}



/*********************************************************************
**
** Function: _DRM_XB_Parse_GUIDLIST
**
** Synopsis: Internal API that deserializes a GUID list sequense from a XB stream
**
** Arguments:
**
** [f_pbBuffer]         -- Pointer to the buffer to read from
** [f_piObject]         -- Index in the buffer to start reading from
** [f_pcbObject]        -- Number of bytes reamaining in the buffer (updated on exit)
** [f_pvObject]         -- DRM_XB_GUIDLIST to store the deserialized result
**
** Returns:            DRM_SUCESS on success.
**                     DRM_E_XB_INVALID_OBJECT is there is an issue with the object size.
**
**********************************************************************/
static DRMFORCEINLINE DRM_RESULT DRM_CALL _DRM_XB_Parse_GUIDLIST(
    __in                         const DRM_BYTE    *f_pbBuffer,
    __in                               DRM_DWORD   *f_piObject,
    __in                               DRM_DWORD   *f_pcbObject,
    __inout_bcount(*f_pcbObject)       DRM_VOID    *f_pvObject )
{
    DRM_RESULT        dr        = DRM_SUCCESS;
    DRM_DWORD         iObject   = *f_piObject;
    DRM_XB_GUIDLIST  *pguidlist = ( DRM_XB_GUIDLIST * )f_pvObject;
    DRM_DWORD         dwResult  = 0;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( *f_pcbObject >= SIZEOF( DRM_DWORD ), DRM_E_XB_INVALID_OBJECT );

    NETWORKBYTES_TO_DWORD( pguidlist->cGUIDs, f_pbBuffer, iObject );
    ChkDR( DRM_DWordAdd( iObject, SIZEOF( DRM_DWORD ), &iObject ) );

    ChkDR( DRM_DWordMult( pguidlist->cGUIDs, SIZEOF( DRM_GUID ), &dwResult ) );
    ChkDR( DRM_DWordAdd( SIZEOF( DRM_DWORD ), dwResult, &dwResult ) );

    ChkBOOL( *f_pcbObject >= dwResult, DRM_E_XB_INVALID_OBJECT );

    pguidlist->pguidBuffer = ( DRM_BYTE * )f_pbBuffer;
    pguidlist->iGuids      = iObject;
    pguidlist->fValid      = TRUE;

    ChkDR( DRM_DWordAdd( *f_piObject, SIZEOF( DRM_DWORD ), f_piObject ) );

    ChkDR( DRM_DWordMult( pguidlist->cGUIDs, SIZEOF( DRM_GUID ), &dwResult ) );
    ChkDR( DRM_DWordAdd( *f_piObject, dwResult, f_piObject ) );

    ChkDR( DRM_DWordAdd( SIZEOF( DRM_DWORD ), dwResult, &dwResult ) );
    ChkDR( DRM_DWordSub( *f_pcbObject, dwResult, f_pcbObject ) );

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: _DRM_XB_Parse_WORDLIST
**
** Synopsis: Internal API that deserializes a WORD list sequense from a XB stream
**
** Arguments:
**
** [f_pbBuffer]         -- Pointer to the buffer to read from
** [f_piObject]         -- Index in the buffer to start reading from
** [f_pcbObject]        -- Number of bytes reamaining in the buffer (updated on exit)
** [f_pvObject]         -- DRM_XB_WORDLIST to store the deserialized result
**
** Returns:            DRM_SUCESS on success.
**                     DRM_E_XB_INVALID_OBJECT is there is an issue with the object size.
**
**********************************************************************/
static DRMFORCEINLINE DRM_RESULT DRM_CALL _DRM_XB_Parse_WORDLIST(
    __in                               DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    __in                         const DRM_BYTE                    *f_pbBuffer,
    __in                               DRM_DWORD                   *f_piObject,
    __in                               DRM_DWORD                   *f_pcbObject,
    __inout_bcount(*f_pcbObject)       DRM_VOID                    *f_pvObject )
{
    DRM_RESULT        dr        = DRM_SUCCESS;
    DRM_DWORD         iObject   = *f_piObject;
    DRM_XB_WORDLIST  *pwordlist = ( DRM_XB_WORDLIST * )f_pvObject;
    DRM_DWORD         i         = 0;
    DRM_DWORD         dwResult  = 0;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( *f_pcbObject >= SIZEOF( DRM_DWORD ), DRM_E_XB_INVALID_OBJECT );

    NETWORKBYTES_TO_DWORD( pwordlist->cWORDs, f_pbBuffer, iObject );
    ChkDR( DRM_DWordAdd( iObject, SIZEOF( DRM_DWORD ), &iObject ) );

    ChkDR( DRM_DWordMult( pwordlist->cWORDs, SIZEOF( DRM_WORD ), &dwResult ) );
    ChkDR( DRM_DWordAdd( SIZEOF( DRM_DWORD ), dwResult, &dwResult ) );
    ChkBOOL( *f_pcbObject >= dwResult, DRM_E_XB_INVALID_OBJECT );

    ChkDR( DRM_DWordMult( pwordlist->cWORDs, SIZEOF( DRM_WORD ), &dwResult ) );
    ChkDR( DRM_STK_Alloc( f_pStack,
                          dwResult,
                         (DRM_VOID**)&pwordlist->pwordBuffer ) );

    pwordlist->iWords      = 0;
    pwordlist->fValid      = TRUE;

    ChkDR( DRM_DWordAdd( *f_piObject, SIZEOF( DRM_DWORD ), f_piObject ) );
    ChkDR( DRM_DWordSub( *f_pcbObject, SIZEOF( DRM_DWORD ), f_pcbObject ) );

    for ( i = 0; i < pwordlist->cWORDs; i++ )
    {
        DRM_VOID *pvResult = NULL;

        ChkDR( DRM_DWordMult( SIZEOF( DRM_WORD ), i, &dwResult ) );
        ChkDR( DRM_DWordAdd( pwordlist->iWords, dwResult, &dwResult ) );
        ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)pwordlist->pwordBuffer, dwResult, (DRM_DWORD_PTR*)&pvResult ) );

        ChkDR( _DRM_XB_Parse_WORD( f_pbBuffer,
                                   f_piObject,
                                   f_pcbObject,
                                   pvResult ) );
    }

ErrorExit:
    return dr;
}



/*********************************************************************
**
** Function: _DRM_XB_Parse_BYTEARRAY
**
** Synopsis: Internal API that deserializes an array of bytes from a XB stream
**
** Arguments:
**
** [f_pbBuffer]         -- Pointer to the buffer to read from
** [f_piObject]         -- Index in the buffer to start reading from
** [f_pcbObject]        -- Number of bytes reamaining in the buffer (updated on exit)
** [f_pvObject]         -- DRM_XB_BYTEARRAY to store the deserialized result
**
** Returns:            DRM_SUCESS on success.
**                     DRM_E_XB_INVALID_OBJECT is there is an issue with the object size.
**
**********************************************************************/
static DRMFORCEINLINE DRM_RESULT DRM_CALL _DRM_XB_Parse_BYTEARRAY(
    __in                         const DRM_BYTE    *f_pbBuffer,
    __in                               DRM_DWORD   *f_piObject,
    __in                               DRM_DWORD   *f_pcbObject,
    __in                               DRM_DWORD    f_cbDataToRead,
    __inout_bcount(*f_pcbObject)       DRM_VOID    *f_pvObject )
{
    DRM_RESULT         dr          = DRM_SUCCESS;
    DRM_XB_BYTEARRAY *pbytearray  = ( DRM_XB_BYTEARRAY * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( *f_pcbObject >= f_cbDataToRead, DRM_E_XB_INVALID_OBJECT );

    pbytearray->cbData        = f_cbDataToRead;
    pbytearray->pbDataBuffer  = ( DRM_BYTE * )f_pbBuffer;
    pbytearray->iData         = *f_piObject;

    ChkDR( DRM_DWordAdd( *f_piObject, f_cbDataToRead, f_piObject ) );
    ChkDR( DRM_DWordSub( *f_pcbObject, f_cbDataToRead, f_pcbObject ) );

    pbytearray->fValid = TRUE;

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: _DRM_XB_Parse_Object
**
** Synopsis: API that initiates the parsing of a XBinary oject
**
** Arguments:
**
** [f_pStack]         -- Pointer to a stack context for dynamic allocations
** [f_pEntry]         -- object entry description
** [f_pbBinaryFormat] -- binary stream to deserialized the object from
** [f_iObject]        -- index into f_pbBinaryFormat
** [f_cbObject]       -- count of bytes in f_pbBinaryFormat
** [f_pvObject]       -- VOID pointer of a struct to deserialized the object into
** [f_pcbRead]        -- out value of the number of bytes read from the stream
**
**********************************************************************/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_VOIDPTR_BUFFER_25120, "f_pvObject cannot be annotated in a way that PREFAST will understand" )
static DRM_NO_INLINE DRM_RESULT DRM_CALL _DRM_XB_Parse_Object(
    __in            DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    __in    const   DRM_XB_ENTRY_DESCRIPTION    *f_pEntry,
    __in    const   DRM_BYTE                    *f_pbBinaryFormat,
    __in            DRM_DWORD                    f_iObject,
    __in            DRM_DWORD                    f_cbObject,
    __inout         DRM_VOID                    *f_pvObject,
    __out_opt       DRM_DWORD                   *f_pcbRead ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT DRM_CALL _DRM_XB_Parse_Object(
    __in            DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    __in    const   DRM_XB_ENTRY_DESCRIPTION    *f_pEntry,
    __in    const   DRM_BYTE                    *f_pbBinaryFormat,
    __in            DRM_DWORD                    f_iObject,
    __in            DRM_DWORD                    f_cbObject,
    __inout         DRM_VOID                    *f_pvObject,
    __out_opt       DRM_DWORD                   *f_pcbRead )
PREFAST_POP
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT                        dr         = DRM_SUCCESS;
    DRM_DWORD                         iCurrent   = f_iObject;
    DRM_DWORD                         cbLengthLeft = f_cbObject;
    const DRM_XB_ELEMENT_DESCRIPTION *pElements  = (const DRM_XB_ELEMENT_DESCRIPTION*)f_pEntry->pContainerOrObjectDescriptions;
    DRM_XB_EMPTY                     *pContainer = ( DRM_XB_EMPTY* )f_pvObject;
    DRM_DWORD                         iElement   = 0;
    ChkArg( f_pbBinaryFormat != NULL );
    ChkArg( f_pvObject  != NULL );

    for( iElement = 0; iElement < f_pEntry->cContainerOrObjectDescriptions; iElement++ )
    {
        DRM_VOID *pobject = NULL;
        DRM_DWORD cbDataToRead = 0;

        ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)f_pvObject, pElements[iElement].wOffsetInCurrentStruct, (DRM_DWORD_PTR*)&pobject ) );

        switch ( pElements[iElement].eElementType )
        {
        case XB_ELEMENT_TYPE_WORD:          ChkDR( _DRM_XB_Parse_WORD     ( f_pbBinaryFormat, &iCurrent, &cbLengthLeft, pobject ) ); break;
        case XB_ELEMENT_TYPE_DWORD:         ChkDR( _DRM_XB_Parse_DWORD    ( f_pbBinaryFormat, &iCurrent, &cbLengthLeft, pobject ) ); break;
        case XB_ELEMENT_TYPE_QWORD:         ChkDR( _DRM_XB_Parse_QWORD    ( f_pbBinaryFormat, &iCurrent, &cbLengthLeft, pobject ) ); break;
        case XB_ELEMENT_TYPE_GUID:          ChkDR( _DRM_XB_Parse_GUID     ( f_pbBinaryFormat, &iCurrent, &cbLengthLeft, pobject ) ); break;
        case XB_ELEMENT_TYPE_GUIDLIST:      ChkDR( _DRM_XB_Parse_GUIDLIST ( f_pbBinaryFormat, &iCurrent, &cbLengthLeft, pobject ) ); break;
        case XB_ELEMENT_TYPE_WORDLIST:      ChkDR( _DRM_XB_Parse_WORDLIST ( f_pStack, f_pbBinaryFormat, &iCurrent, &cbLengthLeft, pobject ) ); break;
        case XB_ELEMENT_TYPE_BYTEARRAY:
            if( pElements[iElement].xbSize.eSizeType == XB_SIZE_IS_RELATIVE_WORD )
            {
                /* If it's a relative size the WORD needs to be serialized as well */
                ChkDR( _DRM_XB_Parse_WORD     ( f_pbBinaryFormat, &iCurrent, &cbLengthLeft, pobject ) );
                cbDataToRead = *((const DRM_WORD*)pobject);
            }
            else if( pElements[iElement].xbSize.eSizeType == XB_SIZE_IS_RELATIVE_DWORD )
            {
                /* If it's a relative size the WORD needs to be serialized as well */
                ChkDR( _DRM_XB_Parse_DWORD     ( f_pbBinaryFormat, &iCurrent, &cbLengthLeft, pobject ) );
                cbDataToRead = *((const DRM_DWORD*)pobject);
            }
            else
            {
                cbDataToRead = pElements[iElement].xbSize.wSize;
            }
            ChkDR( _DRM_XB_Parse_BYTEARRAY( f_pbBinaryFormat, &iCurrent, &cbLengthLeft, cbDataToRead, pobject ) ); break;
        case XB_ELEMENT_TYPE_INVALID:
        default:
            DRMASSERT( !"Unsupported object type.  Most likely there is an error in the format description tables." );
            ChkDR( DRM_E_XB_UNKNOWN_ELEMENT_TYPE );
        }
    }

    if( f_pcbRead != NULL )
    {
        ChkDR( DRM_DWordSub( f_cbObject, cbLengthLeft, f_pcbRead ) );
    }

    pContainer->fValid = TRUE;

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: _DRM_XB_Parse_Container
**
** Synopsis: API that initiates the parsing of a XBinary container
**
** Arguments:
**
** [f_pStack]             -- Pointer to a stack context for dynamic allocations
** [f_wCurrentContainer]  -- The container that we are currently parsing
** [f_pFormat]            -- Descriptor for the entire format
** [f_pbBinaryFormat]     -- binary stream to deserialized the object from
** [f_iObject]            -- index into f_pbBinaryFormat
** [f_cbObject]           -- count of bytes in f_pbBinaryFormat
** [f_pvObject]           -- VOID pointer of a struct to deserialized the object into
**
**********************************************************************/
static DRM_RESULT DRM_CALL _DRM_XB_Parse_Container(
    __in            DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    __in            DRM_WORD                     f_wCurrentContainer,
    __in    const   DRM_XB_FORMAT_DESCRIPTION   *f_pFormat,
    __in    const   DRM_BYTE                    *f_pbBinaryFormat,
    __in            DRM_DWORD                    f_iObject,
    __in            DRM_DWORD                    f_cbObject,
    __inout         DRM_VOID                    *f_pvObject )
{
    CLAW_AUTO_RANDOM_CIPHER

    DRM_RESULT                dr = DRM_SUCCESS;
    DRM_DWORD                 iCurrent = 0;
    DRM_WORD                  wFlags = 0;
    DRM_WORD                  wType = 0;
    DRM_DWORD                 cbChild    = 0;
    DRM_XB_EMPTY             *pContainer = ( DRM_XB_EMPTY* )f_pvObject;

    ChkArg( f_pbBinaryFormat != NULL );
    ChkArg( f_pFormat   != NULL );
    ChkArg( f_pvObject  != NULL );

    pContainer->fValid = TRUE;

    for( iCurrent = f_iObject;
         iCurrent < f_iObject + f_cbObject;
         iCurrent += cbChild )
    {
        DRM_VOID  *pobject   = NULL;
        DRM_DWORD  dwResult  = 0;
        DRM_DWORD  dwCurrent = 0;
        DRM_DWORD  dwCbChild = 0;

        /* Verify that there's at least enough space for a valid empty object */
        ChkDR( DRM_DWordAdd( f_iObject, f_cbObject, &dwResult ) );
        ChkDR( DRM_DWordSub( dwResult, iCurrent, &dwResult ) );
        ChkBOOL( dwResult >= XB_BASE_OBJECT_LENGTH, DRM_E_XB_INVALID_OBJECT );

        /* Parse flags, type, and size (base object length) */
        NETWORKBYTES_TO_WORD( wFlags,  f_pbBinaryFormat, iCurrent );

        ChkDR( DRM_DWordAdd( iCurrent, SIZEOF( DRM_WORD ), &dwResult ) );
        NETWORKBYTES_TO_WORD( wType,   f_pbBinaryFormat, dwResult );

        ChkDR( DRM_DWordAdd( iCurrent, SIZEOF( DRM_WORD ) * 2, &dwResult ) );
        NETWORKBYTES_TO_DWORD( cbChild, f_pbBinaryFormat, dwResult );

        /* Verify that the claimed object size has at least enough space for a valid empty object */
        ChkBOOL( cbChild >= XB_BASE_OBJECT_LENGTH, DRM_E_XB_INVALID_OBJECT );

        /* Verify that there's at least enough space for the claimed object size */
        ChkDR( DRM_DWordAdd( f_iObject, f_cbObject, &dwResult ) );
        ChkDR( DRM_DWordSub( dwResult, iCurrent, &dwResult ) );
        ChkBOOL( cbChild <= dwResult, DRM_E_XB_INVALID_OBJECT );

        if( !(wFlags & XB_FLAGS_MUST_UNDERSTAND) )
        {
            /* It is not "must understand".  See if it should be skipped because we don't understand it */
            if( !_XB_IsKnownObjectType(f_pFormat, wType) )
            {
                /* We don't understand this object.  Skip it */
                continue;
            }
        }

        /* The container is either must understand or we do understand it anyway -- perform basic validity checks */
        ChkBOOL( _XB_IsKnownObjectType(f_pFormat, wType), DRM_E_XB_INVALID_OBJECT );
        ChkBOOL( f_wCurrentContainer == f_pFormat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(f_pFormat, wType)].wParent, DRM_E_XB_INVALID_OBJECT );

        if( f_pFormat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(f_pFormat, wType)].fDuplicateAllowed )
        {
            /* If duplicates are allowed this means that the offset pobject points to needs to be
               the head of a linked list.  We always allocate an object -- it will be placed at the head
               if the list is NULL */

            DRM_XB_BASELIST  *pNewObj = NULL;
            DRM_XB_BASELIST **ppHead  = NULL;

            ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)f_pvObject,
                                    (f_pFormat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(f_pFormat, wType)].wOffsetInCurrentStruct),
                                    (DRM_DWORD_PTR*)&ppHead ) );

            ChkDR( DRM_STK_Alloc( f_pStack,
                                  f_pFormat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(f_pFormat, wType)].dwStructureSize,
                                 (DRM_VOID**)&pNewObj ) );

            pNewObj->fValid = FALSE;
            pNewObj->pNext  = *ppHead;
            *ppHead = pNewObj;
            pobject = pNewObj;
        }
        else
        {
            ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)f_pvObject,
                                    f_pFormat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(f_pFormat, wType)].wOffsetInCurrentStruct,
                                    (DRM_DWORD_PTR*)&pobject ) );

        }

        if( wFlags & XB_FLAGS_CONTAINER )
        {
            /* Special case for non-std XB formats.
            ** If the container had extra space in it we skip that space
            ** as it's listed in cContainerOrObjectDescriptions
            ** V1 KeyFile format uses this
            */
            DRM_WORD  cbExtraSkip = f_pFormat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(f_pFormat, wType)].cContainerOrObjectDescriptions;

            /* Found a container -- recurse to parse the container */
            ChkDR( DRM_DWordAdd( XB_BASE_OBJECT_LENGTH, cbExtraSkip, &dwResult ) );
            ChkDR( DRM_DWordAdd( iCurrent, dwResult, &dwCurrent ) );
            ChkDR( DRM_DWordSub( cbChild, dwResult, &dwCbChild ) );

            ChkDR( _DRM_XB_Parse_Container(
                f_pStack,
                wType,
                f_pFormat,
                f_pbBinaryFormat,
                dwCurrent,
                dwCbChild,
                pobject ) );
        }
        else
        {
            ChkDR( DRM_DWordAdd( iCurrent, XB_BASE_OBJECT_LENGTH, &dwCurrent ) );
            ChkDR( DRM_DWordSub( cbChild, XB_BASE_OBJECT_LENGTH, &dwCbChild ) );

            ChkDR( _DRM_XB_Parse_Object(
                f_pStack,
               &(f_pFormat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(f_pFormat, wType)]),
                f_pbBinaryFormat,
                dwCurrent,
                dwCbChild,
                pobject,
                NULL ) );
        }
    }

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: DRM_XB_UnpackBinary
**
** Synopsis: API that initiates the parsing of a XBinary container
**
** Arguments:
**
** [f_pb]              -- binary stream to deserialized
** [f_cb]              -- count of bytes in f_pb
** [f_pStack]          -- Pointer to a stack context for dynamic allocations
** [f_pformat]         -- Array of descriptors for the entire format (multiple versions)
** [f_cformat]         -- Count of descriptors in f_pformat
** [f_pdwVersionFound] -- Once successfully deserialized the version of
**                        the format found is placed in this out value.
** [f_pStruct]         -- VOID pointer of a struct to deserialized the object into
**
**********************************************************************/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XB_UnpackBinary(
    __in_bcount( f_cb ) const DRM_BYTE                    *f_pb,
    __in                      DRM_DWORD                    f_cb,
    __inout                   DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    __in                const DRM_XB_FORMAT_DESCRIPTION   *f_pformat,
    __in                      DRM_DWORD                    f_cformat,
    __out_opt                 DRM_DWORD                   *f_pdwVersionFound,
    __out                     DRM_VOID                    *f_pStruct )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr               = DRM_SUCCESS;
    DRM_UINT64 qwMagicConstant  = DRM_UI64LITERAL( 0, 0 );
    DRM_DWORD  dwVersion        = 0;
    DRM_DWORD  cbOuterContainer = 0;
    DRM_DWORD  iFormat          = 0;
    DRM_DWORD  icb              = 0;
    DRM_BOOL   fFound           = FALSE;
    DRM_DWORD  dwResult         = 0;

    ChkArg( f_pb     != NULL );
    ChkArg( f_cb     != 0 );
    ChkArg( f_pStruct!= NULL );
    ChkArg( f_pformat!= NULL );

    ChkBOOL( f_cb > XB_HEADER_LENGTH( f_pformat->pHeaderDescription->eFormatIdLength ), DRM_E_XB_INVALID_OBJECT );

    switch( f_pformat->pHeaderDescription->eFormatIdLength )
    {
    case XB_FORMAT_ID_LENGTH_DWORD:
        {
            DRM_DWORD dwMagicConstant = 0;
            NETWORKBYTES_TO_DWORD( dwMagicConstant, f_pb, icb );
            qwMagicConstant = DRM_UI64HL( 0, dwMagicConstant );
        }
        ChkDR( DRM_DWordAdd( icb, XB_FORMAT_ID_LENGTH_DWORD, &icb ) );
        break;
    case XB_FORMAT_ID_LENGTH_QWORD:
        NETWORKBYTES_TO_QWORD( qwMagicConstant, f_pb, icb );
        ChkDR( DRM_DWordAdd( icb, XB_FORMAT_ID_LENGTH_QWORD, &icb ) );
        break;
    default:
        ChkDR( DRM_E_XB_INVALID_OBJECT );
        break;
    }

    NETWORKBYTES_TO_DWORD( dwVersion,        f_pb, icb );
    ChkDR( DRM_DWordAdd( icb, SIZEOF( DRM_DWORD ), &icb ) );

    for( iFormat = 0; !fFound && iFormat < f_cformat; iFormat++ )
    {
        if( DRM_UI64Eql( f_pformat[iFormat].pHeaderDescription->qwFormatIdentifier, qwMagicConstant )
         && f_pformat[iFormat].pHeaderDescription->dwFormatVersion    == dwVersion )
        {
            fFound = TRUE;
        }
    }

    iFormat--;

    if( f_pdwVersionFound != NULL )
    {
        *f_pdwVersionFound = dwVersion;
    }

    if( !fFound )
    {
        ChkDR( DRM_E_XB_INVALID_OBJECT );
    }

    if( f_pformat[iFormat].pHeaderDescription->pEntryDescription != NULL )
    {
        DRM_DWORD     cbRead     = 0;
        DRM_DWORD    *pdwStruct  = NULL;

        /* Special case.  This format has extended header data, which means that it's possible
           the length may not be where we expect */
        ChkDR( DRM_DWordSub( f_cb, icb, &dwResult ) );

        /* Offset of header extra data */
        ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)f_pStruct,
                                f_pformat[iFormat].pHeaderDescription->pEntryDescription->wOffsetInCurrentStruct,
                                (DRM_DWORD_PTR*)&pdwStruct ) );

        ChkDR( _DRM_XB_Parse_Object(
                f_pStack,
                f_pformat[iFormat].pHeaderDescription->pEntryDescription,
                f_pb,
                icb,
                dwResult,
                pdwStruct,
                &cbRead) );

        ChkDR( DRM_DWordAdd( icb, cbRead, &icb ) );

        /* Offset in extra data */
        ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)pdwStruct,
                                f_pformat[iFormat].pHeaderDescription->wOffsetOfSizeInHeaderStruct,
                                (DRM_DWORD_PTR*)&pdwStruct ) );

        cbOuterContainer = *pdwStruct;
    }
    else
    {
        NETWORKBYTES_TO_DWORD( cbOuterContainer, f_pb, icb );
        ChkDR( DRM_DWordAdd( icb, SIZEOF( DRM_DWORD ), &icb ) );
    }

    ChkBOOL( cbOuterContainer <= f_cb, DRM_E_XB_INVALID_OBJECT );

    ChkDR( DRM_DWordSub( cbOuterContainer, icb, &dwResult ) );
    ChkDR( _DRM_XB_Parse_Container(
        f_pStack,
        XB_OBJECT_GLOBAL_HEADER,
        &(f_pformat[iFormat]),
        f_pb,
        icb,
        dwResult,
        f_pStruct ) );

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: _XB_FindChild
**
** Synopsis: API that given the start of an object with find a child object
**           with the type specified in f_wType
**
** Arguments:
**
** [f_wType]              -- Type of child object to locate
** [f_pbBuffer]           -- XBinary format to search
** [f_iCurrentNode]       -- Index in f_pBuffer to start from
** [f_cbCurrentNode]      -- Count of bytes in the current object
** [f_piChild]            -- Index of the child object found
** [f_pcbChild]           -- Count of bytes in the child object
**********************************************************************/
static DRMFORCEINLINE DRM_RESULT DRM_CALL _XB_FindChild(
    __in        DRM_WORD         f_wType,
    __in  const DRM_BYTE        *f_pbBuffer,
    __in        DRM_DWORD        f_iCurrentNode,
    __in        DRM_DWORD        f_cbCurrentNode,
    __out       DRM_DWORD       *f_piChild,
    __out       DRM_DWORD       *f_pcbChild )
{
    DRM_RESULT  dr        = DRM_SUCCESS;
    DRM_DWORD   iNext     = f_iCurrentNode;
    DRM_DWORD   cbNext    = 0;
    DRM_WORD    wNextType = 0;
    DRM_DWORD   dwResult  = 0;

    ChkArg( f_pbBuffer  != NULL );
    ChkArg( f_piChild   != NULL );
    ChkArg( f_pcbChild  != NULL );

    ChkOverflow( iNext, f_iCurrentNode );
    ChkOverflow( f_iCurrentNode + f_cbCurrentNode, f_iCurrentNode );
    ChkOverflow( f_iCurrentNode + f_cbCurrentNode,
                 f_iCurrentNode + f_cbCurrentNode - XB_BASE_OBJECT_LENGTH );   /* Underflow check */

    while ( wNextType != f_wType
          && iNext < ( f_iCurrentNode + f_cbCurrentNode - XB_BASE_OBJECT_LENGTH ) )
    {
        ChkDR( DRM_DWordAdd( iNext, SIZEOF( DRM_WORD ), &dwResult ) );
        NETWORKBYTES_TO_WORD( wNextType,
                              f_pbBuffer,
                              dwResult );

        ChkDR( DRM_DWordAdd( iNext, SIZEOF( DRM_WORD ) * 2, &dwResult ) );
        NETWORKBYTES_TO_DWORD( cbNext,
                               f_pbBuffer,
                               dwResult );

        ChkDR( DRM_DWordAdd( iNext, cbNext, &iNext ) );
    }
    ChkDR( DRM_DWordSub( iNext, cbNext, &iNext ) );

    ChkBOOL( wNextType == f_wType, DRM_E_XB_OBJECT_NOTFOUND );

    ChkDR( DRM_DWordAdd( f_iCurrentNode, f_cbCurrentNode, &dwResult ) );
    ChkDR( DRM_DWordSub( dwResult, iNext, &dwResult ) );
    ChkBOOL( cbNext <= dwResult, DRM_E_XB_INVALID_OBJECT );

    *f_piChild = iNext;
    *f_pcbChild = cbNext;

ErrorExit:
    return dr;
}



/*********************************************************************
**
** Function: DRM_XB_FindObject
**
** Synopsis: API that given the start of an object with find a child object
**           with the type specified in f_wObjectType
**
** Arguments:
**
** [f_wObjectType]    -- Type of object to locate
** [f_pformat]        -- format descriptor
** [f_pb]             -- Index in f_pBuffer to start from
** [f_cb]             -- Count of bytes in the format
** [f_piObject]       -- Index of the object found
** [f_pcbObject]      -- Count of bytes in the object
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XB_FindObject(
    __in                       DRM_WORD                     f_wObjectType,
    __in                 const DRM_XB_FORMAT_DESCRIPTION   *f_pformat,
    __in_bcount( f_cb )  const DRM_BYTE                    *f_pb,
    __in                       DRM_DWORD                    f_cb,
    __out                      DRM_DWORD                   *f_piObject,
    __out                      DRM_DWORD                   *f_pcbObject )
{
    CLAW_AUTO_RANDOM_CIPHER

    DRM_RESULT  dr          = DRM_SUCCESS;
    DRM_WORD    wParent     = f_wObjectType;
    DRM_WORD    wLevel      = 1;
    DRM_DWORD   iCurrent    = XB_HEADER_LENGTH( f_pformat->pHeaderDescription->eFormatIdLength );
    DRM_DWORD   iNext       = XB_HEADER_LENGTH( f_pformat->pHeaderDescription->eFormatIdLength );
    DRM_DWORD   cbCurrent   = f_cb;
    DRM_DWORD   cbNext      = 0;
    DRM_WORD    wHierarchy[XB_MAXIMUM_OBJECT_DEPTH] = { 0 };

    ChkArg( f_pb        != NULL );
    ChkArg( f_piObject  != NULL );
    ChkArg( f_pcbObject != NULL );

    /* Substract the XB header as it won't be taken into account during the FindChild calls */
    ChkDR( DRM_DWordSub( cbCurrent, XB_HEADER_LENGTH( f_pformat->pHeaderDescription->eFormatIdLength ), &cbCurrent ) );

    if( f_pformat->pHeaderDescription->pEntryDescription != NULL )
    {
        /* Special case for extra header data.
        ** Given the special nature only word, dword, and qword types are supported
        */
        DRM_DWORD iDesc;

        for( iDesc = 0; iDesc < f_pformat->pHeaderDescription->pEntryDescription->cContainerOrObjectDescriptions; iDesc++ )
        {
            const DRM_XB_ELEMENT_DESCRIPTION *pElements  = (const DRM_XB_ELEMENT_DESCRIPTION*)f_pformat->pHeaderDescription->pEntryDescription->pContainerOrObjectDescriptions;
            switch( pElements->eElementType )
            {
            case XB_ELEMENT_TYPE_WORD:
                ChkDR( DRM_DWordAdd( iCurrent, SIZEOF( DRM_WORD ), &iCurrent ) );
                ChkDR( DRM_DWordSub( cbCurrent, SIZEOF( DRM_WORD ), &cbCurrent ) );
                break;
            case XB_ELEMENT_TYPE_DWORD:
                ChkDR( DRM_DWordAdd( iCurrent, SIZEOF( DRM_DWORD ), &iCurrent ) );
                ChkDR( DRM_DWordSub( cbCurrent, SIZEOF( DRM_DWORD ), &cbCurrent ) );
                break;
            case XB_ELEMENT_TYPE_QWORD:
                ChkDR( DRM_DWordAdd( iCurrent, SIZEOFQWORD, &iCurrent ) );
                ChkDR( DRM_DWordSub( cbCurrent, SIZEOFQWORD, &cbCurrent ) );
                break;
            default:
                ChkDR( DRM_E_XB_INVALID_OBJECT );
            }
        }
    }

    for ( ; ( wParent != XB_OBJECT_GLOBAL_HEADER
         && wLevel   < XB_MAXIMUM_OBJECT_DEPTH
         && _XB_IsKnownObjectType(f_pformat, wParent) ); wLevel++ )
    {
        wHierarchy[wLevel] = wParent;
        wParent = f_pformat->pEntryDescriptions[_XB_MapObjectTypeToEntryDescriptionIndex(f_pformat, wParent)].wParent;
    }
    wLevel--;

    /*
    **  First find node wHierarchy[wLevel], followed by wHierarchy[wLevel-1],
    **  etc till wHierarchy[1]
    */
    while ( wLevel > 0 && wLevel < XB_MAXIMUM_OBJECT_DEPTH )
    {
        ChkDR( _XB_FindChild( wHierarchy[wLevel],
                              f_pb,
                              iCurrent,
                              cbCurrent,
                             &iNext,
                             &cbNext ) );
        iCurrent = iNext;
        cbCurrent = cbNext;
        wLevel--;
    }
    *f_piObject  = iCurrent;
    *f_pcbObject = cbCurrent;

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

PRAGMA_STRICT_GS_POP;
