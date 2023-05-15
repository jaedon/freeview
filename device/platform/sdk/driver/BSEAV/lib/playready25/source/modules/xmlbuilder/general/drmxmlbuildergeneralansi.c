/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMXMLBUILDERGENERALANSI_C
#include <drmcrt.h>
#include <drmutilities.h>
#include <drmxmlbuildera.h>
#include <oemaes.h>
#include <drmxmlbuilderconstants.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.")

#define CB_XML_BUFFER_MINIMUM_A  100

const DRM_EXPORT_VAR DRM_DWORD g_cbXMLBuilderMinimumA = SIZEOF ( _XMBContextA ) +
                                         __CB_DECL( CB_XML_BUFFER_MINIMUM_A ) +
                                         SIZEOF (DRM_CHAR);

/******************************************************************************
**
** Function:   _PushDWORD
**
** Synopsis:   Push a DWORD value onto the stack.
**
** Arguments:
**
** [f_poXMBContextA]        -- Pointer to an initialized _XMBContextA.
** [f_dwValue]              -- DWORD value to be pushed to the stack.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
******************************************************************************/
static DRM_NO_INLINE DRM_RESULT _PushDWORD(
    __inout _XMBContextA *f_poXMBContextA,
    __in    DRM_DWORD     f_dwValue )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD iPos = 0;

    ChkArg( f_poXMBContextA != NULL &&
            f_poXMBContextA->fInited );

    ChkBOOL( f_poXMBContextA->ichNextOpenNode >= SIZEOF( DRM_DWORD ),
             DRM_E_BUFFERTOOSMALL );

    iPos = f_poXMBContextA->ichNextOpenNode;

    DRM_BYT_CopyBytes( f_poXMBContextA->rgchXML, iPos, &f_dwValue, 0, SIZEOF( DRM_DWORD ) );

    f_poXMBContextA->ichNextOpenNode -= SIZEOF( DRM_DWORD );

ErrorExit:

    return dr;
}

/******************************************************************************
**
** Function:   _PopDWORD
**
** Synopsis:   Pop a DWORD value from the stack.
**
** Arguments:
**
** [f_poXMBContextA]        -- Pointer to an initialized _XMBContextA.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
******************************************************************************/
static DRM_RESULT _PopDWORD(
    __inout _XMBContextA *f_poXMBContextA )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_poXMBContextA != NULL &&
            f_poXMBContextA->fInited );

    f_poXMBContextA->ichNextOpenNode += SIZEOF( DRM_DWORD );

ErrorExit:

    return dr;
}

/******************************************************************************
**
** Function:   _GetTopDWORD
**
** Synopsis:   Get a DWORD value currently at the top of the stack.
**
** Arguments:
**
** [f_poXMBContextA]        -- Pointer to an initialized _XMBContextA.
** [f_pdwValue]             -- Pointer to a DWORD variable to receive the top
**                             DWORD value on the stack.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid or
**                          the stack is empty.
**
******************************************************************************/
static DRM_NO_INLINE DRM_RESULT _GetTopDWORD(
    __in  const _XMBContextA *f_poXMBContextA,
    __out       DRM_DWORD    *f_pdwValue )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD iPos = 0;

    ChkArg( f_poXMBContextA != NULL &&
            f_poXMBContextA->fInited );

    ChkArg( f_pdwValue != NULL );

    iPos = f_poXMBContextA->ichNextOpenNode + SIZEOF( DRM_DWORD );

    ChkArg( iPos >= SIZEOF( DRM_DWORD ) );

    DRM_BYT_CopyBytes( f_pdwValue,
                       0,
                       f_poXMBContextA->rgchXML,
                       iPos,
                       SIZEOF( DRM_DWORD ) );

ErrorExit:

    return dr;
}

/******************************************************************************
**
** Function:   _AllTrim2
**
** Synopsis:   Trim all leading and trailing blanks of a DRM_CHAR string.
**
** Arguments:
**
** [f_pszBase]              -- Pointer to a DRM_CHAR string to be trimmed.
** [f_pdasstr]              -- Pointer to a DRM_SUBSTRING that receives the
**                             portion of a DRM_CHAR string that has been trimmed
**                             (with no leading and trailing blanks).
**
** Returns:                 DRM_TRUE if resulting string length > 0. Otherwise
**                          DRM_FALSE will be returned.
**
******************************************************************************/
static DRM_NO_INLINE DRM_BOOL _AllTrim2(
    __in_ecount(f_pdasstr->m_cch) const DRM_CHAR      *f_pszBase,
    __inout                             DRM_SUBSTRING *f_pdasstr )
{
    DRM_BOOL fResult = FALSE;

    if ( f_pszBase == NULL ||
         f_pdasstr == NULL ||
         f_pdasstr->m_cch == 0 )
    {
        goto ErrorExit;
    }

    /* Trim the leading blanks. */
    while ( GET_CHAR( f_pszBase, f_pdasstr->m_ich ) == ' ' &&
            ( f_pdasstr->m_cch > 0 ) )
    {
        f_pdasstr->m_ich++;
        f_pdasstr->m_cch--;
    }

    /* Trim the trailing blanks. */
    while ( f_pdasstr->m_cch > 0 &&
            GET_CHAR( f_pszBase,
                      f_pdasstr->m_ich + f_pdasstr->m_cch - 1 ) == ' ' )
    {
        f_pdasstr->m_cch--;
    }

    fResult = f_pdasstr->m_cch > 0;

ErrorExit:

    return fResult;
}

/******************************************************************************
**
** Function:   _CreateContextA
**
** Synopsis:   Function that creates and initializes a XML builder context
**             on a passed in buffer.
**
** Arguments:
**
** [f_pbXMBContextA]        -- Pointer to a buffer on which a XML builder context
**                             will be built.
** [f_cbXMBContextA]        -- Size (number of bytes) of a buffer on which a
**                             _XMBContextA structure will be built.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the size of buffer is not big
**                          enough.
**
******************************************************************************/
static DRM_NO_INLINE DRM_RESULT _CreateContextA(
    __inout_bcount(f_cbXMBContextA) DRM_BYTE *f_pbXMBContextA,
    __in                            DRM_DWORD f_cbXMBContextA )
{
    DRM_RESULT dr = DRM_SUCCESS;
    _XMBContextA *poXMBContextA = NULL;

    ChkArg( f_pbXMBContextA != NULL &&
            f_cbXMBContextA > 0 );

    if ( f_cbXMBContextA < g_cbXMLBuilderMinimumA )
    {
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    ZEROMEM( f_pbXMBContextA, f_cbXMBContextA );

    poXMBContextA = ( _XMBContextA * )f_pbXMBContextA;

    poXMBContextA->cbContext = f_cbXMBContextA;

    poXMBContextA->cchBuffer = ( ( f_cbXMBContextA - SIZEOF( _XMBContextA ) ) );

    poXMBContextA->ichNextString  = 0;

    /* Next position to be used on stack */
    poXMBContextA->ichNextOpenNode = poXMBContextA->cchBuffer - SIZEOF( DRM_DWORD );

    poXMBContextA->fInited = TRUE;

    poXMBContextA->cbMaxStackUsed = 0;

ErrorExit:

    return dr;
}

/******************************************************************************
**
** Function:   _OpenNodeA
**
** Synopsis:   Function that creates a new node in the XML builder context.
**
** Arguments:
**
** [f_poXMBContextA]        -- Pointer to an initialized XML builder context.
** [f_pdastrNodeName]       -- Pointer to a DRM_ANSI_CONST_STRING that contains the
**                             name of the node to be created.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the size of buffer in the XML
**                          builder context is not big enough to accomodate the new
**                          node.
**
******************************************************************************/
static DRM_NO_INLINE DRM_RESULT _OpenNodeA(
    __inout       _XMBContextA          *f_poXMBContextA,
    __in    const DRM_ANSI_CONST_STRING *f_pdastrNodeName )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD ichNode = 0;
    DRM_SUBSTRING dasstrTrimmed = EMPTY_DRM_SUBSTRING;
    DRM_DWORD cchRequired = 0;

    ChkArg( f_poXMBContextA != NULL &&
            f_poXMBContextA->fInited );

    ChkDRMANSIString( f_pdastrNodeName );

    dasstrTrimmed.m_ich = 0;
    dasstrTrimmed.m_cch = f_pdastrNodeName->cchString;

    ChkArg( _AllTrim2( f_pdastrNodeName->pszString, &dasstrTrimmed ) );

    /* Calculate the size of the tag (length of the tag name plus two chars for '<>'). */
    cchRequired = f_poXMBContextA->ichNextString + dasstrTrimmed.m_cch + 2;

    ChkFAIL( cchRequired > f_poXMBContextA->ichNextString );
    ChkFAIL( cchRequired > dasstrTrimmed.m_cch );

    if ( cchRequired < f_poXMBContextA->ichNextString )
    {
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    ChkFAIL( ( cchRequired + SIZEOF( DRM_DWORD ) ) > cchRequired );

    cchRequired += SIZEOF( DRM_DWORD );

    if ( cchRequired >= f_poXMBContextA->ichNextOpenNode )
    {
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /* Output "<tag>" to the output buffer. */
    PUT_CHAR( f_poXMBContextA->rgchXML,
              f_poXMBContextA->ichNextString,
              '<' );

    f_poXMBContextA->ichNextString++;

    ichNode = f_poXMBContextA->ichNextString;

    DRM_BYT_CopyBytes( f_poXMBContextA->rgchXML,
                       f_poXMBContextA->ichNextString,
                       f_pdastrNodeName->pszString,
                       dasstrTrimmed.m_ich,
                       dasstrTrimmed.m_cch );

    f_poXMBContextA->ichNextString += dasstrTrimmed.m_cch;

    PUT_CHAR( f_poXMBContextA->rgchXML,
              f_poXMBContextA->ichNextString,
              '>' );

    f_poXMBContextA->ichNextString++;

    /* Push the position of the new node onto the stack. */
    ChkDR( _PushDWORD( f_poXMBContextA, ichNode ) );

ErrorExit:

    return dr;
}

/******************************************************************************
**
** Function:   _CloseCurrNodeA
**
** Synopsis:   Function that closes the current node in the XML builder context.
**
** Arguments:
**
** [f_poXMBContextA]        -- Pointer to an initialized XML builder context.
** [f_fOkayToCloseRoot]     -- Flag indicating whether the root node should be closed.
** [f_pdasstrXML]           -- Pointer to a DRM_SUBSTRING to receive the XML fragment
**                             of the node to be closed (including tags).
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_NOMORE if there is no node to be closed.
**                          DRM_E_NO_XML_CLOSE_TAG if the current node is not
**                          followed by a '>'.
**                          DRM_E_BUFFERTOOSMALL if the size of buffer in the XML
**                          builder context is not big enough to accomodate the data
**                          to be added.
**
******************************************************************************/
static DRM_NO_INLINE DRM_RESULT _CloseCurrNodeA(
    __inout   _XMBContextA  *f_poXMBContextA,
    __in      DRM_BOOL       f_fOkayToCloseRoot,
    __out_opt DRM_SUBSTRING *f_pdasstrXML )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cch = 0;
    DRM_DWORD ichNode = 0;
    DRM_DWORD cchMinStack = 0;

    ChkArg( f_poXMBContextA != NULL &&
            f_poXMBContextA->fInited );

    /*
    ** Make sure the stack is not empty. Stack is count by DRM_DWORD.
    ** to close a node, there must be at least one DRM_DWORD on the stack.
    ** if f_fOkayToCloseRoot is FALSE, there must be at least two DRM_DWORDs
    ** on the stack.
    */
    if ( f_fOkayToCloseRoot )
    {
        /* Should have the at least a Root node on the stack. */
        cchMinStack = SIZEOF( DRM_DWORD );
    }
    else
    {
        /* Should have at least a Root node plus the last open node on the stack. */
        cchMinStack = 2 * SIZEOF( DRM_DWORD );
    }

    if ( f_poXMBContextA->ichNextOpenNode + cchMinStack  < f_poXMBContextA->ichNextOpenNode ||
         f_poXMBContextA->ichNextOpenNode + cchMinStack >= f_poXMBContextA->cchBuffer )
    {
        /* Stack is empty. */
        ChkDR( DRM_E_NOMORE );
    }

    /* Retrieve the length of name of the current node on top of the stack. */
    ChkDR( _GetTopDWORD( f_poXMBContextA, &ichNode ) );

    if ( f_pdasstrXML != NULL )
    {
        /* Save the starting position of the current node. */
        f_pdasstrXML->m_ich = ichNode - 1;
    }

    /*
    ** Calculate the length of the name of the current node.
    ** If a space (<Tag Attr-"...">) or '>' (<Tag>) is found, all characters
    ** in front of it and after the '<' character (pointed by ichNode) are part
    ** of the name.
    */
    while ( ichNode + cch < f_poXMBContextA->cchBuffer &&
            ( GET_CHAR( f_poXMBContextA->rgchXML, ichNode + cch ) != '>'  &&
              GET_CHAR( f_poXMBContextA->rgchXML, ichNode + cch ) != ' ' ) )
    {
        cch++;
    }

    /* Check whether the '>' or the ' ' character is found. */
    if ( ichNode + cch >= f_poXMBContextA->cchBuffer ||
         ( GET_CHAR( f_poXMBContextA->rgchXML, ichNode + cch ) != '>'  &&
          GET_CHAR( f_poXMBContextA->rgchXML, ichNode + cch ) != ' ' ) )
    {
        ChkDR( DRM_E_NO_XML_CLOSE_TAG );
    }

    /* The constant 3 means the additional three characters for a close tag: </>. */
    if ( f_poXMBContextA->ichNextString + cch < f_poXMBContextA->ichNextString ||
         f_poXMBContextA->ichNextString + cch + 3 < f_poXMBContextA->ichNextString ||
         f_poXMBContextA->ichNextString + cch + 3 >= f_poXMBContextA->ichNextOpenNode )
    {
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /* Output </tag> to the output buffer. */
    DRM_BYT_CopyBytes( f_poXMBContextA->rgchXML,
                       f_poXMBContextA->ichNextString,
                       g_dastrOpenEndTag.pszString,
                       0,
                       g_dastrOpenEndTag.cchString );

    f_poXMBContextA->ichNextString += g_dastrOpenEndTag.cchString;

    DRM_BYT_CopyBytes( f_poXMBContextA->rgchXML,
                       f_poXMBContextA->ichNextString,
                       f_poXMBContextA->rgchXML,
                       ichNode,
                       cch );

    f_poXMBContextA->ichNextString += cch;

    PUT_CHAR( f_poXMBContextA->rgchXML,
              f_poXMBContextA->ichNextString,
              '>' );

    f_poXMBContextA->ichNextString++;

    /* Pop the current node from the stack. */
    _PopDWORD( f_poXMBContextA );

    if (f_pdasstrXML != NULL)
    {
        /* Save the length of the node that has just been closed. */
        f_pdasstrXML->m_cch = f_poXMBContextA->ichNextString - ichNode + 1;
    }

ErrorExit:

    return dr;
}


/*
** calcuate the current node size
*/
DRM_RESULT DRM_CALL DRM_XMB_CalcNodeSizeA(
    __in  const _XMBContextA  *f_pbContextXMLA,
    __out       DRM_DWORD     *f_pcchContent,  /* ch count of node content */
    __out       DRM_DWORD     *f_pcchTagName)  /* ch count of node tag */
{
    DRM_RESULT          dr           = DRM_SUCCESS;
    const _XMBContextA *pcontextXMLA = (const _XMBContextA *) f_pbContextXMLA;
    DRM_DWORD           cch          = 0;
    DRM_DWORD           ichNode      = 0;

    ChkArg(f_pbContextXMLA != NULL
         && f_pcchContent  != NULL
         && f_pcchTagName  != NULL
         && pcontextXMLA->fInited);

    /* parse length of node name */

    ChkDR( _GetTopDWORD( pcontextXMLA, &ichNode ) );

    while (GET_CHAR(pcontextXMLA->rgchXML, ichNode + cch) != ' '
        && GET_CHAR(pcontextXMLA->rgchXML, ichNode + cch) != '>')
    {
        cch++;
    }

    *f_pcchTagName = cch;

    /* parse first position of node content */
    while (GET_CHAR(pcontextXMLA->rgchXML, ichNode + cch) != '>')
    {
        cch++;
    }

    *f_pcchContent = pcontextXMLA->ichNextString - ichNode - cch - 1;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function:   DRM_XMB_CreateDocumentA
**
** Synopsis:   API that creates and initializes a XML builder context on a passed
**             in buffer. It also opens the root node of the whole XML document
**             to be created.
**
** Arguments:
**
** [f_cbXMBContextA]        -- Size (number of bytes) of a passed in buffer
**                             on which a XML builder context will be built.
** [f_pbXMBContextA]        -- Pointer to buffer on which a XML builder context
**                             will be created.
** [f_pdastrRootNode]       -- Pointer to a DRM_ANSI_CONST_STRING that contains the
**                             name of the root node of the XML document to be
**                             created.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMB_CreateDocumentA(
    __in                      const DRM_DWORD              f_cbXMBContextA,
    __inout_bcount(f_cbXMBContextA) DRM_BYTE              *f_pbXMBContextA,
    __in                      const DRM_ANSI_CONST_STRING *f_pdastrRootNode )
{
    DRM_RESULT dr = DRM_SUCCESS;
    _XMBContextA *poXMBContextA = ( _XMBContextA * )f_pbXMBContextA;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_CreateDocumentA );

    ChkArg( f_pbXMBContextA != NULL &&
            f_cbXMBContextA != 0 );

    /* Make sure the passed in buffer is aligned properly. */
    DRMASSERT( ( ( ( DRM_DWORD_PTR )f_pbXMBContextA) % SIZEOF( DRM_DWORD ) ) == 0 );

    ChkDRMANSIString( f_pdastrRootNode );
    ChkBOOL( f_cbXMBContextA >= SIZEOF( _XMBContextA ), DRM_E_BUFFERTOOSMALL);

    /* Create the XML builder context. */
    ChkDR( _CreateContextA( f_pbXMBContextA, f_cbXMBContextA ) );

    /* Open the root node. */
    ChkDR( _OpenNodeA( poXMBContextA, f_pdastrRootNode ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function:   DRM_XMB_CloseDocumentA
**
** Synopsis:   API that closes a XML document being created and returns the
**             complete XML string that has been built.
**
** Arguments:
**
** [f_poXMBContextA]        -- Pointer to an initialized XML builder context.
** [f_pdasstrXML]           -- Pointer to a DRM_SUBSTRING to receive the complete
**                             XML string that has been built.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMB_CloseDocumentA(
    __inout _XMBContextA  *f_poXMBContextA,
    __out   DRM_SUBSTRING *f_pdasstrXML )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_CloseDocumentA );

    ChkArg( f_poXMBContextA != NULL &&
            f_poXMBContextA->fInited );

    ChkArg( f_pdasstrXML != NULL );

    for ( ; ; )
    {
        /* Closes all nodes on the stack. */
        dr = _CloseCurrNodeA( f_poXMBContextA, TRUE, NULL );

        if ( dr == DRM_E_NOMORE )
        {
            dr = DRM_SUCCESS;

            break;
        }
        else
        {
            ChkDR( dr );
        }
    }

    f_pdasstrXML->m_ich = 0;
    f_pdasstrXML->m_cch = f_poXMBContextA->ichNextString;

    /* Invalidate the XML builder context. */
    f_poXMBContextA->fInited = FALSE;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function:   DRM_XMB_OpenNodeA
**
** Synopsis:   API that creates a new node in the XML builder context.
**
** Arguments:
**
** [f_poXMBContextA]        -- Pointer to an initialized XML builder context.
** [f_pdastrNodeName]       -- Pointer to a DRM_ANSI_CONST_STRING that contains the
**                             name of the node to be created.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMB_OpenNodeA(
    __inout       _XMBContextA          *f_poXMBContextA,
    __in    const DRM_ANSI_CONST_STRING *f_pdastrNodeName )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_OpenNodeA );

    ChkArg( f_poXMBContextA != NULL &&
            f_poXMBContextA->fInited );

    ChkDRMANSIString( f_pdastrNodeName );

    ChkDR( _OpenNodeA( f_poXMBContextA, f_pdastrNodeName ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function:   DRM_XMB_CloseCurrNodeA
**
** Synopsis:   API that closes the current node in the XML builder context.
**
** Arguments:
**
** [f_poXMBContextA]        -- Pointer to an initialized XML builder context.
** [f_pdasstrXML]           -- Pointer to a DRM_SUBSTRING to receive the XML fragment
**                             of the node to be closed (including tags).
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMB_CloseCurrNodeA(
    __inout   _XMBContextA  *f_pbXMBContextA,
    __out_opt DRM_SUBSTRING *f_pdasstrXML )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_CloseCurrNodeA );

    ChkArg( f_pbXMBContextA != NULL );

    ChkDR( _CloseCurrNodeA( f_pbXMBContextA, FALSE, f_pdasstrXML ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_XMB_AESEncryptAndCloseCurrNodeA(
    __inout       _XMBContextA  *f_pbContextXMLA,
    __in    const DRM_AES_KEY   *f_pkeyAES,
    __in          DRM_DWORD      f_dwNonce,
    __out         DRM_SUBSTRING *f_pdasstrXMLFragment )
{
    DRM_RESULT    dr = DRM_SUCCESS;
    _XMBContextA *pcontextXMLA = (_XMBContextA *) f_pbContextXMLA;
    DRM_DWORD     cchContent   = 0;
    DRM_DWORD     cchEncoded   = 0;
    DRM_DWORD     cchTagName   = 0;
    DRM_DWORD     ichStart     = 0;
    DRM_BYTE     *pbContent    = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_AESEncryptAndCloseCurrNodeA );

    ChkArg(f_pbContextXMLA != NULL
        && f_pkeyAES      != NULL
        && pcontextXMLA->fInited);

    /*
    ** 1. check available buffer size:
    **    req size = (blob_size) / 3 * 4)
    ** 2. call AES code to encrypt the blob in place
    ** 3. call Base64Encode the encrypted data
    ** 4. close the node
    */

    /* check available space */

    ChkDR(DRM_XMB_CalcNodeSizeA(f_pbContextXMLA, &cchContent, &cchTagName));

    /* cache the starting index */

    ichStart  = pcontextXMLA->ichNextString - cchContent;


    cchEncoded = CCH_BASE64_EQUIV(cchContent);

    if (pcontextXMLA->ichNextString - cchContent  > pcontextXMLA->ichNextString
    ||  pcontextXMLA->ichNextString - cchContent >= pcontextXMLA->cchBuffer)
    {
        ChkDR(DRM_E_BUFFERTOOSMALL);
    }

    pbContent = (DRM_BYTE*)pcontextXMLA->rgchXML + ichStart;
    /* encrypt the content */

    if (cchContent > 0)
    {
        DRM_AES_COUNTER_MODE_CONTEXT ctx = { 0 };
        ctx.qwInitializationVector = DRM_UI64(f_dwNonce);
        ctx.qwBlockOffset = DRM_UI64(0);
        ctx.bByteOffset = 0;


        ChkDR(Oem_Aes_CtrProcessData(f_pkeyAES, pbContent, cchContent, &ctx));
        /* base64 encode the encrypted blob */

        ChkDR(DRM_B64_EncodeA(pbContent,
                              cchContent,
                   (DRM_CHAR*)pbContent,
                             &cchEncoded,
                              0));
    }

    /* update ichNextString with the difference between raw and B64-encoded data sizes */

    pcontextXMLA->ichNextString = pcontextXMLA->ichNextString - cchContent + cchEncoded;

    /* now close the node */
    ChkDR(_CloseCurrNodeA(f_pbContextXMLA, FALSE, f_pdasstrXMLFragment));

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_XMB_HashAndCloseCurrNodeA(
    __inout                                                   _XMBContextA      *f_pbContextXMLA,
    __in                                                      DRM_SHA1_CONTEXT  *f_pcontextSHA,
    __in                                                      DRM_BOOL           f_fIncludeTag,
    __out_bcount(DRM_SHA1_DIGEST_LEN)                         DRM_BYTE           f_rgbSHA [__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __out_ecount(DRM_SHA1_B64ENC_DIGEST_LEN)                  DRM_CHAR           f_rgchSHA[__CB_DECL(DRM_SHA1_B64ENC_DIGEST_LEN)],
    __out_opt                                                 DRM_SUBSTRING     *f_pdasstrXMLFragment )
{
    DRM_RESULT      dr = DRM_SUCCESS;
    _XMBContextA   *pcontextXMLA   = (_XMBContextA *) f_pbContextXMLA;
    DRM_SUBSTRING   dasstrFragment = { 0 };
    DRM_DWORD       cchContent = 0;
    DRM_DWORD       cchTagName = 0;
    DRM_DWORD       cchSHA     = 0;
    const DRM_BYTE *pbContent  = 0;
    DRM_DWORD       ichStart   = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_HashAndCloseCurrNodeA );

    ChkArg(f_pbContextXMLA != NULL
         && f_pcontextSHA  != NULL
         && f_rgbSHA       != NULL
         && pcontextXMLA->fInited);

    ChkDR(DRM_XMB_CalcNodeSizeA(f_pbContextXMLA, &cchContent, &cchTagName));

    ichStart  = pcontextXMLA->ichNextString - cchContent;

    pbContent = (DRM_BYTE*)pcontextXMLA->rgchXML + ichStart;

    DRM_SHA1_Init(f_pcontextSHA);

    if (! f_fIncludeTag)
    {
        /* sign the content */

        DRM_SHA1_Update(pbContent, cchContent, f_pcontextSHA);
    }

    /* close the node */
    ChkDR(_CloseCurrNodeA(f_pbContextXMLA, FALSE, &dasstrFragment));

    if (f_fIncludeTag)
    {
        pbContent = (DRM_BYTE*)pcontextXMLA->rgchXML + dasstrFragment.m_ich;

        /* sign the content */
        DRM_SHA1_Update(pbContent, dasstrFragment.m_cch, f_pcontextSHA);
    }
    DRM_SHA1_Finalize(f_pcontextSHA, f_rgbSHA);

    /* Base64 encode the signature */

    if (f_rgchSHA != NULL)
    {
        cchSHA = DRM_SHA1_B64ENC_DIGEST_LEN;

        ChkDR(DRM_B64_EncodeA(f_rgbSHA,
                              DRM_SHA1_DIGEST_LEN,
                              f_rgchSHA,
                             &cchSHA,
                              0));
    }

    if (f_pdasstrXMLFragment != NULL)
    {
        *f_pdasstrXMLFragment = dasstrFragment;
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function:   DRM_XMB_AddAttributeA
**
** Synopsis:   API that adds an atribute pair to the current node in the XML
**             builder context.
**
** Arguments:
**
** [f_poXMBContextA]        -- Pointer to an initialized XML builder context.
** [f_pdastrAttrName]       -- Pointer to a DRM_ANSI_CONST_STRING that contains
**                             the name of the attribute to be added.
** [f_pdastrAttrValue]      -- Pointer to a DRM_ANSI_CONST_STRING that contains
**                             the value of the attribute to be added.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_NOMORE if there is no node opened.
**                          DRM_E_NO_XML_CLOSE_TAG if the current node is not
**                          followed by a '>'.
**                          DRM_E_BUFFERTOOSMALL if the size of buffer in the XML
**                          builder context is not big enough to accomodate the data
**                          to be added.
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMB_AddAttributeA(
    __inout       _XMBContextA          *f_poXMBContextA,
    __in    const DRM_ANSI_CONST_STRING *f_pdastrAttrName,
    __in    const DRM_ANSI_CONST_STRING *f_pdastrAttrValue )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchRequired = 0;
    DRM_SUBSTRING dasstrNameTrimmed = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrValueTrimmed = EMPTY_DRM_SUBSTRING;
    DRM_DWORD ichInsert = 0;
    DRM_DWORD ichDest = 0;
    DRM_DWORD ichSource = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_AddAttributeA );

    ChkArg( f_poXMBContextA != NULL &&
            f_poXMBContextA->fInited );

    ChkDRMANSIString( f_pdastrAttrName );
    ChkDRMANSIString( f_pdastrAttrValue );

    if ( f_poXMBContextA->ichNextOpenNode + 1 >= f_poXMBContextA->cchBuffer )
    {
        /* Stack is empty. */
        ChkDR(DRM_E_NOMORE);
    }

    dasstrNameTrimmed.m_ich = 0;
    dasstrValueTrimmed.m_ich = 0;
    dasstrNameTrimmed.m_cch = f_pdastrAttrName->cchString;
    dasstrValueTrimmed.m_cch = f_pdastrAttrValue->cchString;

    _AllTrim2( f_pdastrAttrName->pszString, &dasstrNameTrimmed );

    /* Make sure the attribute name is not blank. */
    if ( dasstrNameTrimmed.m_cch == 0 )
    {
        ChkDR( DRM_E_INVALIDARG );
    }

    /* Blank attrib value is allowed. */
    _AllTrim2( f_pdastrAttrValue->pszString, &dasstrValueTrimmed );

    /* Calculate the buffer size needed to insert the ' attr="value"'. 4 = space + equal sign + "". */
    cchRequired = dasstrNameTrimmed.m_cch +
                  dasstrValueTrimmed.m_cch +
                  4;

    /* Check whether there is enough space. */
    if ( f_poXMBContextA->ichNextString + cchRequired  < f_poXMBContextA->ichNextString ||
         f_poXMBContextA->ichNextString + cchRequired >= f_poXMBContextA->ichNextOpenNode )
    {
        ChkDR(DRM_E_BUFFERTOOSMALL);
    }

    /* Retrieve the length of name of the current node from the top of the stack. */
    ChkDR( _GetTopDWORD( f_poXMBContextA, &ichInsert ) );

    /* Search for the position of the '>' character. */
    while ( ichInsert < f_poXMBContextA->cchBuffer &&
            GET_CHAR( f_poXMBContextA->rgchXML, ichInsert ) != '>' )
    {
        ichInsert++;
    }

    /* Check whether the '>' character is found. */
    if ( ichInsert >= f_poXMBContextA->cchBuffer ||
         GET_CHAR( f_poXMBContextA->rgchXML, ichInsert ) != '>' )
    {
        ChkDR( DRM_E_NO_XML_CLOSE_TAG );
    }

    /*
    ** Now, rgchXML[ichInsert] contains the '>' char, output attr=value there.
    ** First, shift everything, if any, from '>' to allow space for the output.
    */
    ichDest = ichInsert + cchRequired;
    ichSource = ichInsert;

    if ( ichInsert + cchRequired  < ichInsert ||
         ichInsert + cchRequired >= f_poXMBContextA->cchBuffer )
    {
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    DRM_BYT_MoveBytes( f_poXMBContextA->rgchXML,
                       ichDest,
                       f_poXMBContextA->rgchXML,
                       ichSource,
                       f_poXMBContextA->ichNextString - ichInsert );

    /* Output the attribute name. */
    PUT_CHAR( f_poXMBContextA->rgchXML, ichInsert++, ' ' );

    DRM_BYT_CopyBytes( f_poXMBContextA->rgchXML,
                       ichInsert,
                       f_pdastrAttrName->pszString,
                       dasstrNameTrimmed.m_ich,
                       dasstrNameTrimmed.m_cch );

    ichInsert += dasstrNameTrimmed.m_cch;

    /* Output =". */
    PUT_CHAR( f_poXMBContextA->rgchXML, ichInsert++, '=' );
    PUT_CHAR( f_poXMBContextA->rgchXML, ichInsert++, '\"' );

    /* Output the attribute value. */
    DRM_BYT_CopyBytes( f_poXMBContextA->rgchXML,
                       ichInsert,
                       f_pdastrAttrValue->pszString,
                       dasstrValueTrimmed.m_ich,
                       dasstrValueTrimmed.m_cch );

    ichInsert += dasstrValueTrimmed.m_cch;

    /* Output " */
    PUT_CHAR( f_poXMBContextA->rgchXML, ichInsert++, '\"' );

    /* Update the output buffer pointer. */
    f_poXMBContextA->ichNextString += cchRequired;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function:   DRM_XMB_AddDataA
**
** Synopsis:   API that adds a string data to the current node in the XML
**             builder context.
**
** Arguments:
**
** [f_poXMBContextA]        -- Pointer to an initialized XML builder context.
** [f_pdastrData]           -- Pointer to a DRM_ANSI_CONST_STRING that contains
**                             the data to be added.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_NOMORE if there is no node opened.
**                          DRM_E_NO_XML_CLOSE_TAG if the current node is not
**                          followed by a '>'.
**                          DRM_E_BUFFERTOOSMALL if the size of buffer in the XML
**                          builder context is not big enough to accomodate the data
**                          to be added.
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMB_AddDataA(
    __inout       _XMBContextA          *f_poXMBContextA,
    __in    const DRM_ANSI_CONST_STRING *f_pdastrData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD i = 0;
    DRM_SUBSTRING dasstrDataTrimmed = EMPTY_DRM_SUBSTRING;
    DRM_DWORD ichInsert = 0;
    DRM_DWORD ichDest = 0;
    DRM_DWORD ichSource = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_AddDataA );

    ChkArg( f_poXMBContextA != NULL &&
            f_poXMBContextA->fInited );

    ChkDRMANSIString( f_pdastrData );

    if ( f_poXMBContextA->ichNextOpenNode + 1 >= f_poXMBContextA->cchBuffer )
    {
        /* Stack is empty. */
        ChkDR( DRM_E_NOMORE );
    }

    dasstrDataTrimmed.m_ich = 0;
    dasstrDataTrimmed.m_cch = f_pdastrData->cchString;

    if ( !_AllTrim2( f_pdastrData->pszString, &dasstrDataTrimmed ) )
    {
        /* If the data being added is a blank string, nothing needs to be done. */
        goto ErrorExit;
    }

    /* Check whether there is enough space. */
    if ( ( f_poXMBContextA->ichNextString + dasstrDataTrimmed.m_cch ) < f_poXMBContextA->ichNextString ||
         ( f_poXMBContextA->ichNextString + dasstrDataTrimmed.m_cch ) >= f_poXMBContextA->ichNextOpenNode )
    {
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /* Retrieve the length of name of the current node on top of the stack. */
    ChkDR( _GetTopDWORD( f_poXMBContextA, &ichInsert ) );

    /* Search for the position of the '>' character. */
    while ( ichInsert < f_poXMBContextA->cchBuffer &&
            GET_CHAR( f_poXMBContextA->rgchXML, ichInsert ) != '>' )
    {
        ichInsert++;
    }

    /* Check whether the '>' character is found. */
    if ( ichInsert >= f_poXMBContextA->cchBuffer ||
         GET_CHAR( f_poXMBContextA->rgchXML, ichInsert ) != '>' )
    {
        ChkDR( DRM_E_NO_XML_CLOSE_TAG );
    }

    /*
    ** At this point, rgchXML[ichInsert] contains a '>' char, output data after it.
    ** first, shift all the existing output, if any, after '>' to allow space for the
    ** output.
    */
    ichInsert++;

    ichDest = f_poXMBContextA->ichNextString + dasstrDataTrimmed.m_cch - 1;
    ichSource = f_poXMBContextA->ichNextString - 1;

    for ( i = 0; i < ( f_poXMBContextA->ichNextString - ichInsert ); i++ )
    {
        PUT_CHAR( f_poXMBContextA->rgchXML,
                  ichDest--,
                  GET_CHAR( f_poXMBContextA->rgchXML, ichSource-- ) );
    }

    /* Make sure there is enough space left. */
    ChkBOOL( ( f_poXMBContextA->cchBuffer - ichInsert) > dasstrDataTrimmed.m_cch, DRM_E_BUFFERTOOSMALL );

    /* Output the data. */
    DRM_BYT_CopyBytes( f_poXMBContextA->rgchXML,
                       ichInsert,
                       f_pdastrData->pszString,
                       dasstrDataTrimmed.m_ich,
                       dasstrDataTrimmed.m_cch);

    /* Update the output buffer pointer. */
    f_poXMBContextA->ichNextString += dasstrDataTrimmed.m_cch;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function:   DRM_XMB_WriteTagA
**
** Synopsis:   API that creates a new node that has optional data and attribute
**             pair in the XML builder context. The node is left open or closed
**             depending on an input enumeration value.
**
** Arguments:
**
** [f_poXMBContextA]        -- Pointer to an initialized XML builder context.
** [f_pdastrTag]            -- Pointer to a DRM_ANSI_CONST_STRING that contains
**                             the name of the node to be added.
** [f_pdastrData]           -- Pointer to a DRM_ANSI_CONST_STRING that contains
**                             the data (optional) of the node to be added.
** [f_pdastrAttrName]       -- Pointer to a DRM_ANSI_CONST_STRING that contains
**                             the attribute name (optional) of the node to be added.
** [f_pdastrAttrValue]      -- Pointer to a DRM_ANSI_CONST_STRING that contains
**                             the attribute value (optional) of the node to be added.
** [f_wtt]                  -- Enumeration value indicating whether the node will
**                             remain open or closed after this API call.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMB_WriteTagA(
    __inout        _XMBContextA          *f_poXMBContextA,
    __in     const DRM_ANSI_CONST_STRING *f_pdastrTag,
    __in_opt const DRM_ANSI_CONST_STRING *f_pdastrData,
    __in_opt const DRM_ANSI_CONST_STRING *f_pdastrAttrName,
    __in_opt const DRM_ANSI_CONST_STRING *f_pdastrAttrValue,
    __in           WriteTagType           f_wtt )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_WriteTagA );

    ChkArg( f_poXMBContextA != NULL &&
            f_poXMBContextA->fInited );

    ChkDRMANSIString( f_pdastrTag );

    ChkArg( ( f_pdastrAttrName == NULL && f_pdastrAttrValue == NULL) ||
            ( f_pdastrAttrName != NULL && f_pdastrAttrValue != NULL ) );

    /* Create the node. */
    ChkDR( DRM_XMB_OpenNodeA( f_poXMBContextA, f_pdastrTag ) );

    /* Add optional attribute pair. */
    if ( f_pdastrAttrName != NULL && f_pdastrAttrValue != NULL )
    {
        ChkDR( DRM_XMB_AddAttributeA( f_poXMBContextA, f_pdastrAttrName, f_pdastrAttrValue ) );
    }

    /* Add optional data. */
    if ( f_pdastrData != NULL )
    {
        ChkDR( DRM_XMB_AddDataA( f_poXMBContextA, f_pdastrData ) );
    }

    if ( f_wtt == wttClosed )
    {
        ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMBContextA, NULL ) );
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function:   DRM_XMB_GetXMLBaseA
**
** Synopsis:   API that retrieves the base pointer to an internal buffer of
**             the XML builder context.
**
** Arguments:
**
** [f_poXMBContextA]        -- Pointer to an initialized XML builder context.
** [f_ppchBase]             -- Pointer to pointer to an internal buffer of the
**                             XML builder context.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMB_GetXMLBaseA(
    __inout _XMBContextA *f_poXMBContextA,
    __out   DRM_CHAR    **f_ppchBase )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_poXMBContextA != NULL &&
            f_ppchBase != NULL );

    *f_ppchBase = f_poXMBContextA->rgchXML;

ErrorExit:

    return dr;
}

/******************************************************************************
** Function:   DRM_XMB_AddCDataA
**
** Synopsis:   API that adds a string data as a CDATA node to the current node
**             in the XML builder context.
**
** Arguments:
**
** [f_poXMBContextA]        -- Pointer to an initialized XML builder context.
** [f_pdastrCData]          -- Pointer to a DRM_ANSI_CONST_STRING that contains
**                             the data to be added as a CDATA node.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_NOMORE if there is no node opened.
**                          DRM_E_NO_XML_CLOSE_TAG if the current node is not
**                          followed by a '>'.
**                          DRM_E_BUFFERTOOSMALL if the size of buffer in the XML
**                          builder context is not big enough to accomodate the data
**                          to be added.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_XMB_AddCDataA(
    __inout       _XMBContextA          *f_poXMBContextA,
    __in    const DRM_ANSI_CONST_STRING *f_pdastrCData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD i = 0;
    DRM_SUBSTRING dasstrDataTrimmed = EMPTY_DRM_SUBSTRING;
    DRM_DWORD ichInsert = 0;
    DRM_DWORD cchReqSize = 0;
    DRM_DWORD ichDest = 0;
    DRM_DWORD ichSource = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_AddCDataA );

    ChkArg( f_poXMBContextA != NULL &&
            f_poXMBContextA->fInited );

    ChkDRMANSIString( f_pdastrCData );

    if ( f_poXMBContextA->ichNextOpenNode + 1 >= f_poXMBContextA->cchBuffer )
    {
        /* Stack is empty. */
        ChkDR( DRM_E_NOMORE );
    }

    dasstrDataTrimmed.m_ich = 0;
    dasstrDataTrimmed.m_cch = f_pdastrCData->cchString;

    /* Trim leading and trialing blanks, but blank CDATA value is allowed */
    if ( !_AllTrim2( f_pdastrCData->pszString, &dasstrDataTrimmed ) )
    {
        /* If the data being added is a blank string, nothing needs to be done. */
        goto ErrorExit;
    }

    /* Calculate the buffer size needed to insert <!<CDATA[xxx]]>. */
    cchReqSize = dasstrDataTrimmed.m_cch;

    ChkOverflow( cchReqSize + g_dastrOpenCDATATag.cchString, cchReqSize );
    cchReqSize += g_dastrOpenCDATATag.cchString;

    ChkOverflow( cchReqSize + g_dastrCloseCDATATag.cchString , cchReqSize );
    cchReqSize += g_dastrCloseCDATATag.cchString;

    if ( ( f_poXMBContextA->ichNextString + cchReqSize ) < f_poXMBContextA->ichNextString ||
         ( f_poXMBContextA->ichNextString + cchReqSize ) >= f_poXMBContextA->ichNextOpenNode )
    {
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /* Retrieve the length of name of the current node on top of the stack. */
    ChkDR( _GetTopDWORD( f_poXMBContextA, &ichInsert ));

    /* Search for the position of the '>' character. */
    while ( ichInsert < f_poXMBContextA->cchBuffer &&
            GET_CHAR( f_poXMBContextA->rgchXML, ichInsert ) != '>' )
    {
        ichInsert++;
    }

    /* Check whether the '>' character is found. */
    if ( ichInsert >= f_poXMBContextA->cchBuffer ||
         GET_CHAR( f_poXMBContextA->rgchXML, ichInsert ) != '>' )
    {
        ChkDR( DRM_E_NO_XML_CLOSE_TAG );
    }

    /*
    ** At this point, rgchXML[ichInsert] contains a '>' char, output data after it.
    ** first, shift all the existing output, if any, after '>' to allow space for the
    ** output.
    */
    ichInsert++;

    ichDest = f_poXMBContextA->ichNextString + cchReqSize - 1;
    ichSource = f_poXMBContextA->ichNextString - 1;

    for ( i = 0; i < ( f_poXMBContextA->ichNextString - ichInsert ); i++ )
    {
        PUT_CHAR( f_poXMBContextA->rgchXML,
                  ichDest--,
                  GET_CHAR( f_poXMBContextA->rgchXML, ichSource-- ) );
    }

    /* Make sure there is enough space left. */
    ChkBOOL( ( f_poXMBContextA->cchBuffer - ichInsert) > cchReqSize, DRM_E_BUFFERTOOSMALL );

    /* Output the CDATA node. */
    DRM_BYT_CopyBytes( f_poXMBContextA->rgchXML,
                       ichInsert,
                       g_dastrOpenCDATATag.pszString,
                       0,
                       g_dastrOpenCDATATag.cchString );

    ichInsert += g_dastrOpenCDATATag.cchString;

    DRM_BYT_CopyBytes( f_poXMBContextA->rgchXML,
                       ichInsert,
                       f_pdastrCData->pszString,
                       dasstrDataTrimmed.m_ich,
                       dasstrDataTrimmed.m_cch );

    ichInsert += dasstrDataTrimmed.m_cch;

    DRM_BYT_CopyBytes( f_poXMBContextA->rgchXML,
                       ichInsert,
                       g_dastrCloseCDATATag.pszString,
                       0,
                       g_dastrCloseCDATATag.cchString );

     /* Update the output buffer pointer. */
    f_poXMBContextA->ichNextString += cchReqSize;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function:   DRM_XMB_WriteTagA
**
** Synopsis:   API that creates a new node that has optional CDATA node data and
**             attribute pair in the XML builder context. The node is left open
**             or closed depending on an input enumeration value.
**
** Arguments:
**
** [f_poXMBContextA]        -- Pointer to an initialized XML builder context.
** [f_pdastrTag]            -- Pointer to a DRM_ANSI_CONST_STRING that contains
**                             the name of the node to be added.
** [f_pdastrData]           -- Pointer to a DRM_ANSI_CONST_STRING that contains
**                             the data (optional) that is to be put into the CDATA
**                             node within the node to be added.
** [f_pdastrAttrName]       -- Pointer to a DRM_ANSI_CONST_STRING that contains
**                             the attribute name (optional) of the node to be added.
** [f_pdastrAttrValue]      -- Pointer to a DRM_ANSI_CONST_STRING that contains
**                             the attribute value (optional) of the node to be added.
** [f_wtt]                  -- Enumeration value indicating whether the node will
**                             remain open or closed after this API call.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_XMB_WriteCDATATagA(
    __inout       _XMBContextA *f_poXMBContextA,
    __in    const DRM_ANSI_CONST_STRING *f_pdastrTag,
    __in    const DRM_ANSI_CONST_STRING *f_pdastrCData,
    __in    const DRM_ANSI_CONST_STRING *f_pdastrAttrName,
    __in    const DRM_ANSI_CONST_STRING *f_pdastrAttrValue,
    __in          WriteTagType f_wtt )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_WriteCDATATagA );

    ChkArg( f_poXMBContextA != NULL &&
            f_poXMBContextA->fInited );

    ChkDRMANSIString( f_pdastrTag );

    ChkArg( ( f_pdastrAttrName == NULL && f_pdastrAttrValue == NULL) ||
            ( f_pdastrAttrName != NULL && f_pdastrAttrValue != NULL ) );

    /* Create the node. */
    ChkDR( DRM_XMB_OpenNodeA( f_poXMBContextA, f_pdastrTag ) );

    /* Add optional attribute pair. */
    if ( f_pdastrAttrName != NULL && f_pdastrAttrValue != NULL )
    {
        ChkDR( DRM_XMB_AddAttributeA( f_poXMBContextA, f_pdastrAttrName, f_pdastrAttrValue ) );
    }

    /* Add optional CDATA. */
    if ( f_pdastrCData != NULL )
    {
        ChkDR( DRM_XMB_AddCDataA( f_poXMBContextA, f_pdastrCData ) );
    }

    if ( f_wtt == wttClosed )
    {
        ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMBContextA, NULL ) );
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function:   DRM_XMB_ReserveSpaceA
**
** Synopsis:   API that reserve a specific number of characters in the XML builder
**             context.
**
** Arguments:
**
** [f_poXMBContextA]        -- Pointer to an initialized XML builder context.
** [f_cchAlignment]         -- An integer that defines the alignment of the start
**                             address of the buffer space reserved. For example,
**                             if the start address should be aligned to DWORD,
**                             f_cchAlignment should be SIZEOF(DRM_DWORD).
** [f_cchData]              -- Number of characters to reserve in the XML builder
**                             context.
** [f_pdasstrData]          -- Pointer to a sub string that receives the portion of
**                             the internal XML builder context buffer that has been
**                             reserved.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the size of buffer in the XML
**                          builder context is not big enough to accomodate the data
**                          to be added.
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMB_ReserveSpaceA(
    __inout       _XMBContextA  *f_poXMBContextA,
    __in          DRM_DWORD      f_cchAlignment,
    __in    const DRM_DWORD      f_cchData,
    __out         DRM_SUBSTRING *f_pdasstrData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD ichStart = 0, cchShift = 0, ich = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_ReserveSpaceA );

    ChkArg( f_poXMBContextA != NULL &&
            f_poXMBContextA->fInited );

    ChkArg( f_cchAlignment != 0 );
    ChkArg( f_cchData > 0 );
    ChkArg( f_pdasstrData != NULL );

    /* Check whether there is enough space. */
    if ( ( f_poXMBContextA->ichNextString + f_cchData ) < f_poXMBContextA->ichNextString ||
         ( f_poXMBContextA->ichNextString + f_cchData ) >= f_poXMBContextA->ichNextOpenNode )
    {
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    ichStart = f_poXMBContextA->ichNextString;

    if ( ichStart % f_cchAlignment != 0)
    {
        cchShift = f_cchAlignment - ( ichStart % f_cchAlignment );

        /* Fill the 'hole' with spaces. */
        for ( ich = 0; ich < cchShift; ich++ )
        {
            PUT_CHAR( f_poXMBContextA->rgchXML,
                      ichStart + ich,
                      ' ' );
        }

        f_poXMBContextA->ichNextString += cchShift;
    }

    f_pdasstrData->m_ich = f_poXMBContextA->ichNextString;
    f_pdasstrData->m_cch = f_cchData;

    /* Update the output buffer pointer. */
    f_poXMBContextA->ichNextString += f_cchData;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function:   DRM_XMB_ShiftDataFromCurrentPositionA
**
** Synopsis:   API that shift a specific number of characters from the current
**             buffer position in the XML builderto a buffer position of a
**             higer address.
**
** Arguments:
**
** [f_poXMBContextA]        -- Pointer to an initialized XML builder context.
** [f_cchShift]             -- Amount (number of characters) of the shift from
**                             the current bufer position in the XML builder
**                             context.
** [f_cchData]              -- Number of characters to shift from the current
**                             buffer position in the XML builder context.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the size of buffer in the XML
**                          builder context is not big enough to satisfy the
**                          operation.
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMB_ShiftDataFromCurrentPositionA(
    __inout       _XMBContextA *f_poXMBContextA,
    __in    const DRM_DWORD     f_cchShift,
    __in    const DRM_DWORD     f_cchData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD ichTarget = 0, ich = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_ShiftDataFromCurrentPositionA );

    ChkArg( f_poXMBContextA != NULL &&
            f_poXMBContextA->fInited );

    ChkArg( f_cchData > 0 );

    /* Check whether there is enough space. */
    if ( ( f_poXMBContextA->ichNextString + f_cchShift + f_cchData ) < f_poXMBContextA->ichNextString ||
         ( f_poXMBContextA->ichNextString + f_cchShift + f_cchData ) >= f_poXMBContextA->ichNextOpenNode )
    {
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    ichTarget = f_poXMBContextA->ichNextString + f_cchShift;

    DRM_BYT_MoveBytes( f_poXMBContextA->rgchXML,
                       ichTarget,
                       f_poXMBContextA->rgchXML,
                       f_poXMBContextA->ichNextString,
                       f_cchData );

    /* Fill the 'hole' with spaces. */
    for ( ich = 0; ich < f_cchShift; ich++ )
    {
        PUT_CHAR( f_poXMBContextA->rgchXML,
                  f_poXMBContextA->ichNextString + ich,
                  ' ' );
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function:   DRM_XMB_ShiftCurrentPointerA
**
** Synopsis:   API that shift the current buffer pointer in the XML builder
**             context to a position of a higer address.
**
** Arguments:
**
** [f_poXMBContextA]        -- Pointer to an initialized XML builder context.
** [f_cchShift]             -- Amount (number of characters) of the shift from
**                             the current position in the XML builder context.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the size of buffer in the XML
**                          builder context is not big enough to satisfy the
**                          operation.
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMB_ShiftCurrentPointerA(
    __inout       _XMBContextA *f_poXMBContextA,
    __in    const DRM_DWORD     f_cchShift )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_ShiftCurrentPointerA );

    ChkArg( f_poXMBContextA != NULL &&
            f_poXMBContextA->fInited );

    /* Check whether there is enough space. */
    if ( ( f_poXMBContextA->ichNextString + f_cchShift ) < f_poXMBContextA->ichNextString ||
         ( f_poXMBContextA->ichNextString + f_cchShift ) >= f_poXMBContextA->ichNextOpenNode )
    {
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    f_poXMBContextA->ichNextString += f_cchShift;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function:   DRM_XMB_GetCurrentBufferPointerA
**
** Synopsis:   API that returns the current aligned buffer pointer in the XML
**             builder context.
**
** Arguments:
**
** [f_poXMBContextA]        -- Pointer to an initialized XML builder context.
** [f_fAligned]             -- Flag indicating whether the returned buffer pointer
**                             should be aligned.
** [f_ppchBufferPointer]    -- Pointer to a variable to receive the current
**                             aligned buffer pointer in the XML builder context.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMB_GetCurrentBufferPointerA(
    __inout _XMBContextA *f_poXMBContextA,
    __in    DRM_BOOL      f_fAligned,
    __out DRM_CHAR      **f_ppchBufferPointer )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchExtra = 0;
    DRM_DWORD ich = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_GetCurrentBufferPointerA );

    ChkArg( f_poXMBContextA != NULL &&
            f_poXMBContextA->fInited );
    ChkArg( f_ppchBufferPointer != NULL );

    if ( f_fAligned )
    {
        cchExtra = f_poXMBContextA->ichNextString % SIZEOF( DRM_WORD );

        if ( cchExtra != 0 )
        {
            cchExtra = SIZEOF( DRM_WORD ) - cchExtra;

            /* Fill the 'hole' with spaces. */
            for ( ich = 0; ich < cchExtra; ich++ )
            {
                PUT_CHAR( f_poXMBContextA->rgchXML,
                          f_poXMBContextA->ichNextString + ich,
                          ' ' );
            }

            f_poXMBContextA->ichNextString += cchExtra;
        }
    }

    *f_ppchBufferPointer = f_poXMBContextA->rgchXML +
                           __CB_DECL( f_poXMBContextA->ichNextString );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/**********************************************************************
** Function:    DRM_XMB_AppendNodeA
** Synopsis:    Appends a caller provided node nested from current opened node
** Arguments:   [pXmlContextA]    -- UTF-8 XML context
**              [pdastrXmlString] -- pointer to UTF-8 string to add
** Returns:     DRM_SUCCESS on success
** Notes:       The caller is responsible for the validity of the appending node
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_XMB_AppendNodeA(
    __inout       _XMBContextA          *f_poXMBContextA,
    __in    const DRM_ANSI_CONST_STRING *f_pdastrXmlString )
{
    DRM_RESULT    dr        = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_AppendNodeA );

    ChkArg( f_poXMBContextA != NULL );
    ChkArg( f_poXMBContextA->fInited );
    ChkDRMANSIString( f_pdastrXmlString );

    if ( f_poXMBContextA->ichNextString + f_pdastrXmlString->cchString < f_poXMBContextA->ichNextString
      || f_poXMBContextA->ichNextString + f_pdastrXmlString->cchString >= f_poXMBContextA->ichNextOpenNode)
    {
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    ChkOverflow( f_poXMBContextA->cchBuffer, f_poXMBContextA->cchBuffer - f_poXMBContextA->ichNextString );

    ChkDR( DRM_STR_StringCchCopyNA( &f_poXMBContextA->rgchXML [f_poXMBContextA->ichNextString],
                                    f_poXMBContextA->cchBuffer - f_poXMBContextA->ichNextString,
                                    f_pdastrXmlString->pszString,
                                    f_pdastrXmlString->cchString ) );

    f_poXMBContextA->ichNextString += f_pdastrXmlString->cchString;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

EXIT_PK_NAMESPACE_CODE;

