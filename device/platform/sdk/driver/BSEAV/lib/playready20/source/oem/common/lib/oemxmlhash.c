/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <oemxmlhash.h>
#include <oemplatform.h>

ENTER_PK_NAMESPACE_CODE;  

/* Global variable of an arry that stores pointers to DRM_XML_HASH_CONTEXT structures. */
static DRM_XML_HASH_CONTEXT *g_rgpoXMLHashContexts[ MAX_XML_HASH_THREAD ] = { 0 };

/* Global variable of a critical section that serializes access to g_rgpoXMLHashContexts. */
static OEM_CRITICAL_SECTION g_oXMLHashCS = { 0 };

/*
** Global variable of a flag indicating whether the global variables used for XML hashing
** has been initialized by Oem_XMLHash_Init.
*/
static DRM_BOOL g_fInited = FALSE;

/*********************************************************************
**
** Function: _AllocContext
**
** Synopsis: Allocate a DRM_XML_HASH_CONTEXT for the current thread.
**
** Arguments:               N/A
**
** Returns:                 Pointer to a DRM_XML_HASH_CONTEXT being allocated.
**                          NULL if therer are already MAX_XML_HASH_THREAD
**                          being allocated.
**
**********************************************************************/
static DRM_XML_HASH_CONTEXT *_AllocContext( DRM_VOID )
{
    DRM_DWORD i = MAX_XML_HASH_THREAD;
    DRM_DWORD dwThreadID = 0;
  
    if ( g_fInited )
    {
        dwThreadID = Oem_GetCurrentThreadId();
        
        Oem_CritSec_Enter( &g_oXMLHashCS );

        for ( i = 0; i < MAX_XML_HASH_THREAD; i++ )
        {
            if ( g_rgpoXMLHashContexts[ i ] == NULL )
            {
                if ( ( g_rgpoXMLHashContexts[ i ] =
                    ( DRM_XML_HASH_CONTEXT * )Oem_MemAlloc( SIZEOF( DRM_XML_HASH_CONTEXT ) ) ) != NULL )
                {
                    ZEROMEM( g_rgpoXMLHashContexts[ i ], SIZEOF( DRM_XML_HASH_CONTEXT ) );
                    g_rgpoXMLHashContexts[ i ]->m_dwThreadID = dwThreadID;
                    g_rgpoXMLHashContexts[ i ]->m_fXMLHashStackInit = FALSE;
                }

                break;
            }
        }

        Oem_CritSec_Leave( &g_oXMLHashCS );
    }

    return ( i < MAX_XML_HASH_THREAD ? g_rgpoXMLHashContexts[ i ] : NULL );
}

/*********************************************************************
**
** Function: _FreeContext
**
** Synopsis: Free a DRM_XML_HASH_CONTEXT for the current thread.
**
** Arguments:               N/A
**
** Returns:                 N/A.
**
**********************************************************************/
static DRM_VOID _FreeContext( DRM_VOID )
{
    DRM_DWORD i = 0;
    DRM_DWORD dwThreadID = 0;

    if ( g_fInited )
    {
        dwThreadID = Oem_GetCurrentThreadId();

        Oem_CritSec_Enter( &g_oXMLHashCS );

        for ( i = 0; i < MAX_XML_HASH_THREAD; i++ )
        {
            if ( g_rgpoXMLHashContexts[ i ] != NULL &&
                 g_rgpoXMLHashContexts[ i ]->m_dwThreadID == dwThreadID )
            {
                Oem_MemFree( g_rgpoXMLHashContexts[ i ] );
                g_rgpoXMLHashContexts[ i ] = NULL;
                break;
            }
        }

        Oem_CritSec_Leave( &g_oXMLHashCS );
    }
}

/*********************************************************************
**
** Function: _GetContext
**
** Synopsis: Find out whether there is a DRM_XML_HASH_CONTEXT allocated
**           for the current thread.
**
** Arguments:               N/A
**
** Returns:                 Pointer to a DRM_XML_HASH_CONTEXT if exists.
**                          NULL if there is not a DRM_XML_HASH_CONTEXT
**                          allocated for the current thread.
**
**********************************************************************/
static DRM_XML_HASH_CONTEXT *_GetContext( DRM_VOID )
{
    DRM_DWORD i = 0;
    DRM_DWORD dwThreadID = 0;
    DRM_XML_HASH_CONTEXT *poContext = NULL;
    
    if ( g_fInited )
    {
        dwThreadID = Oem_GetCurrentThreadId();

        Oem_CritSec_Enter( &g_oXMLHashCS );

        for ( i = 0; i < MAX_XML_HASH_THREAD; i++ )
        {
            if ( g_rgpoXMLHashContexts[ i ] != NULL &&
                 g_rgpoXMLHashContexts[ i ]->m_dwThreadID == dwThreadID )
            {
                poContext = g_rgpoXMLHashContexts[ i ];
                break;
            }
        }

        Oem_CritSec_Leave( &g_oXMLHashCS );

        if ( poContext == NULL )
        {
            /* 
            ** If there is not a DRM_XML_HASH_CONTEXT allocated
            ** for the current thread, try to allocate one.
            */
            poContext = _AllocContext();
        }
    }

    return poContext;
}

/*********************************************************************
**
** Function: _CalcStringHash
**
** Synopsis: Function that calculates hashes for the input string using
**           two string hashing algorithm.
**
** Arguments:
**
** [f_poContext]            -- Pointer to a DRM_XML_HASH_CONTEXT.
** [f_pdstrString]          -- Pointer to a DRM_STRING to be hashed.
** [f_pdwResult1]           -- Pointer to a DWORD to receive the first hash.
** [f_pdwResult2]           -- Pointer to a DWORD to receive the second hash.
**
** Returns:                 DRM_SUCCESS on success.
**
** Note:                    For performance reason, function argument check
**                          is skipped for this internal function.
**
**********************************************************************/
static DRM_RESULT _CalcStringHash(
    __in DRM_XML_HASH_CONTEXT *f_poContext,
    __in const DRM_CONST_STRING *f_pdstrString,
    __out DRM_DWORD *f_pdwResult1,
    __out DRM_DWORD *f_pdwResult2 )
{
    /* Skip parameter checking for internal static functions. */

    /*
    ** Use of CPU register-capable variables
    ** is preferred because registers can be
    ** read/written without relatively
    ** expensive memory cycles.
    **
    ** pwszString = f_pdstrString->pwszString
    ** could be moved into the 'if (0 != i)'
    ** block to save cycles in the (0 == i) 
    ** case.
    ** 
    ** However, the (0 == i) case is likely
    ** quite rare, so it is probably better
    ** to load pwszString earlier to avoid
    ** an otherwise possible register stall.
    */
    DRM_DWORD c;
    DRM_DWORD i = f_pdstrString->cchString;
    const DRM_WCHAR *pwszString = f_pdstrString->pwszString;
    DRM_DWORD dwInterimSDBM = 0;
    DRM_DWORD dwInterimDJB2 = 5381;

    if ( 0 != i )
    {
        do
        {
            /*
            ** Post-increment with lower-sophistication
            ** compilers is typically faster.  This is
            ** especially true for the ARM architecture
            ** which supports a post-increment addressing
            ** mode that achieves read and post-increment 
            ** with a single CPU instruction.
            */
            c = ( DRM_DWORD )( *pwszString++ );

            /* sdbm hashing algorithm. */
            dwInterimSDBM = c +
                            ( ( dwInterimSDBM ) << 6 ) +
                            ( ( dwInterimSDBM ) << 16) -
                            ( dwInterimSDBM );
        
            /* djb2 hashing algorithm. */
            dwInterimDJB2 = ( ( ( dwInterimDJB2 ) << 5 ) +
                            ( dwInterimDJB2 ) ) +
                            c;

            /* 
            ** Count down is typically easier
            ** for a compiler to turn into good code.
            ** 
            ** != is more often directly implemented
            ** in single CPU instruction, e.g. MIPS
            ** requires multiple CPU instructions for
            ** certain conditionals
            */
        } while ( 0 != --i );
    }

    /*
    ** For compilers that are less sophisticated 
    ** it is usually better to reuse a register 
    ** capable variable to contain a multiple 
    ** referenced value (e.g. f_poContext->m_dwThreadID).
    **
    ** This is especially true when a good number
    ** of CPU registers are available (as in the 
    ** ARM case).  This reuse of a register-capable
    ** variable virtually never hurts optimization.
    */

    /* 
    ** For security purpose, use thread ID as a nonce to make 
    ** the hash values session dependent. 
    */
    i = f_poContext->m_dwThreadID;
    *f_pdwResult1 = dwInterimSDBM ^ i;
    *f_pdwResult2 = dwInterimDJB2 ^ i;

    /* 
    ** It seems to me that this internal static
    ** function could be void since it has no 
    ** parameter checks or other error conditions.
    ** 
    ** Returning DRM_SUCCES is not that expensive,
    ** rather it is the check (by the caller) 
    ** and resulting conditional logic that is
    ** more expensive.
    */
    return DRM_SUCCESS;
}

/*********************************************************************
**
** Function: _CalcRootHash
**
** Synopsis: Function that calculates the hash value of the passed in
**           DRM_XML_HASH_SIG structure.
**
** Arguments:
**
** [f_poSig]                -- Pointer to a DRM_XML_HASH_SIG structure
**                             to calculate the root hash.
** [f_pdwHash]              -- Pointer to a DWORD to receive the calculated
**                             root hash.
**
** Returns:                 DRM_SUCCESS on success.
**
** Note:                    For performance reason, function argument check
**                          is skipped for this internal function.
**
**********************************************************************/
static DRM_RESULT _CalcRootHash(
    __in DRM_XML_HASH_SIG *f_poSig,
    __out DRM_DWORD *f_pdwHash )
{
    /* Skip parameter checking for internal static functions. */

    *f_pdwHash = ( f_poSig->m_dwSourceHash1 + 
                   f_poSig->m_dwSourceHash2 +
                   f_poSig->m_dwTagHash1 +
                   f_poSig->m_dwTagHash2 +
                   f_poSig->m_dwNode +
                   f_poSig->m_dwLayer ) % MAX_XML_ROOT_HASH;

    return DRM_SUCCESS;
}

/*********************************************************************
**
** Function: _HashExists
**
** Synopsis: Function that checks whether a DRM_XML_HASH_NODE exists
**           for the DRM_XML_HASH_SIG structure being passed in.
**
** Arguments:
**
** [f_poContext]            -- Pointer to an initialized DRM_XML_HASH_CONTEXT.
** [f_poSig]                -- Pointer to a DRM_XML_HASH_SIG structure
**                             to search for the corresponding DRM_XML_HASH_NODE.
** [f_ppoHashNode]          -- Pointer to a pointer to receive the DRM_XML_HASH_NODE
**                             if found.
**
** Returns:                 DRM_SUCCESS on success.
**
** Note:                    For performance reason, function argument check
**                          is skipped for this internal function.
**
**********************************************************************/
static DRM_RESULT _HashExists(
    __in DRM_XML_HASH_CONTEXT *f_poContext,
    __in DRM_XML_HASH_SIG *f_poSig,
    __out DRM_XML_HASH_NODE **f_ppoHashNode )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD dwRootHash = 0;
    DRM_XML_HASH_NODE *poNode = NULL;

    /* Skip parameter checking for internal static functions. */

    *f_ppoHashNode = NULL;

    ChkDR( _CalcRootHash( f_poSig, &dwRootHash ) );

    if ( ( poNode = f_poContext->m_rgpoHashNodes[ dwRootHash ] ) != NULL  )
    {
        while ( poNode != NULL )
        {
            if ( MEMCMP( f_poSig, 
                         &poNode->m_oSig,
                         SIZEOF( DRM_XML_HASH_SIG ) ) != 0 )
            {
                poNode = poNode->m_poNext;
            }
            else
            {
#if ENABLE_OEM_XMLHASH_STATISTICS                
                (poNode->m_dwHit)++;
#endif /* ENABLE_OEM_XMLHASH_STATISTICS */
                break;
            }
        }

        if ( poNode != NULL )
        {
            *f_ppoHashNode = poNode;
        }
    }

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: Oem_XMLHash_Update
**
** Synopsis: Function that updates the hash table by creating a mapping
**           between the DRM_XML_HASH_SIG and the XML search result.
**
** Arguments:
**
** [f_poContext]            -- Pointer to a DRM_XML_HASH_CONTEXT.
** [f_poSig]                -- Pointer to a DRM_XML_HASH_SIG structure
**                             to create a mapping.
** [f_pdstrXML]             -- Pointer to a DRM string that is the source XML
**                             string to be searched.
** [f_pdstrNode]            -- Pointer to a DRM string that is a substring
**                             for a XML node (including tag) within the
**                             buffer of f_pdstrXML.
** [f_pdstrData]            -- Pointer to a DRM string that is a substring
**                             for a XML node (excluding tag) within the
**                             buffer of f_pdstrXML.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_API Oem_XMLHash_Update(
    __in DRM_XML_HASH_CONTEXT *f_poContext,
    __in DRM_XML_HASH_SIG *f_poSig,
    __in const DRM_CONST_STRING *f_pdstrXML,
    __in DRM_CONST_STRING *f_pdstrNode,
    __in DRM_CONST_STRING *f_pdstrNodeData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_XML_HASH_NODE *poHashNode = NULL;
    DRM_DWORD dwRootHash = 0;

    ChkArg( f_poContext != NULL );
    
    if ( !f_poContext->m_fXMLHashStackInit )
    {
        goto ErrorExit;
    }

    ChkArg( f_poSig != NULL );
    ChkDRMString( f_pdstrXML );
    ChkDRMString( f_pdstrNode );
    ChkArg( f_pdstrNodeData != NULL );

    /* Allocate a hash node. */
    dr = DRM_STK_Alloc( &f_poContext->m_oXMLHashStackContext,
                        SIZEOF( DRM_XML_HASH_NODE ),
                        ( DRM_VOID ** )&poHashNode );

    if ( dr == DRM_E_OUTOFMEMORY )
    {
        /*
        ** If the stack is full, just exit the function without
        ** creating the hash mapping.
        */
        dr = DRM_SUCCESS;
        goto ErrorExit;
    }
    else
    {
        ChkDR( dr );
    }

    ZEROMEM( poHashNode, SIZEOF( DRM_XML_HASH_NODE ) );

    MEMCPY( &poHashNode->m_oSig, f_poSig, SIZEOF( DRM_XML_HASH_SIG ) );

    /* Calculate the starting point of the XML node substring. */
    poHashNode->m_dasstrNode.m_ich = (DRM_DWORD) (f_pdstrNode->pwszString - f_pdstrXML->pwszString);

    /* Calculate the length of the XML node substring. */
    poHashNode->m_dasstrNode.m_cch = f_pdstrNode->cchString;
    
    if ( f_pdstrNodeData->pwszString != NULL )
    {
        /* Calculate the starting point of the XML node data substring. */
        poHashNode->m_dasstrNodeData.m_ich = (DRM_DWORD)(f_pdstrNodeData->pwszString - f_pdstrXML->pwszString);
        
        /* Calculate the length of the XML node data substring. */
        poHashNode->m_dasstrNodeData.m_cch = f_pdstrNodeData->cchString;
    }
    else
    {
        poHashNode->m_dasstrNodeData.m_ich = 0;
        poHashNode->m_dasstrNodeData.m_cch = 0;
    }

    ChkDR( _CalcRootHash( f_poSig, &dwRootHash ) );

    /* Add the hash node to the hash table. */
    if ( f_poContext->m_rgpoHashNodes[ dwRootHash ] == NULL )
    {
        f_poContext->m_rgpoHashNodes[ dwRootHash ] = poHashNode;
    }
    else
    {
        poHashNode->m_poNext = f_poContext->m_rgpoHashNodes[ dwRootHash ];
        f_poContext->m_rgpoHashNodes[ dwRootHash ] = poHashNode;
    }

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: Oem_XMLHash_Lookup
**
** Synopsis: Function that search the hash table to find whether there
**           is an entry for the passed in DRM_XML_HASH_SIG. If the
**           seach succeeded, f_pdstrNode contains a substring of a XML
**           node (including tag) within the XML string pointed by 
**           f_pdstrXML. f_pdstrNodeData contains a substring of a XML
**           node data (excluding tag) within the XML string pointed by 
**           f_pdstrXML.
**
** Arguments:
**
** 
** [f_poContext]            -- Pointer to a DRM_XML_HASH_CONTEXT.
** [f_poSig]                -- Pointer to a DRM_XML_HASH_SIG structure
**                             to search for a mapping.
** [f_pdstrXML]             -- Pointer to a DRM string that is the source XML
**                             string to be searched.
** [f_pdstrTag]             -- Pointer to a DRM string that is a substring
**                             within f_pdstrXML to be searched.
** [f_pdstrNode]            -- Pointer to a DRM string to receive a substring
**                             of a XML node (including tag) within the
**                             buffer of f_pdstrXML.
** [f_pdstrData]            -- Pointer to a DRM string to receive a substring
**                             of a XML node (excluding tag) within the
**                             buffer of f_pdstrXML.
**
** Returns:                 DRM_SUCCESS on success and the seach finds a result.
**                          DRM_S_FALSE if the seach does not find a result.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_API Oem_XMLHash_Lookup(
    __in DRM_XML_HASH_CONTEXT *f_poContext,
    __in DRM_XML_HASH_SIG *f_poSig,
    __in const DRM_CONST_STRING *f_pdstrXML,
    __in const DRM_CONST_STRING *f_pdstrTag,
    __out DRM_CONST_STRING *f_pdstrNodeOut,
    __out DRM_CONST_STRING *f_pdstrNodeDataOut )
{
    DRM_RESULT dr = DRM_S_FALSE;
    DRM_XML_HASH_NODE *poHashNode = NULL;

    ChkArg( f_poContext != NULL );

    if ( !f_poContext->m_fXMLHashStackInit )
    {
        goto ErrorExit;
    }

    ChkArg( f_poSig != NULL );
    ChkDRMString( f_pdstrXML );
    ChkDRMString( f_pdstrTag );
    ChkArg( f_pdstrNodeOut != NULL && f_pdstrNodeDataOut != NULL );

    ChkDR( _HashExists( f_poContext,
                        f_poSig,
                        &poHashNode ) );

    if ( poHashNode != NULL )
    {
        /* 
        ** Doing some additional sanity checks here.
        ** The matched XML substring (including tag) should be within the range of the source XML string.
        ** The matched XML substring (including tag) should be at least five characters plus two times 
        ** the length of the passed in tag to be searched (<xxx></xxx>).
        ** The first character of the matched XML substring (including tag) should be '<'.
        ** The last character of the matched XML substring (including tag) should be '>'.
        */

        ChkOverflow( f_pdstrTag->cchString * 2, f_pdstrTag->cchString );
        ChkOverflow( 5 + f_pdstrTag->cchString * 2, f_pdstrTag->cchString * 2 );

        if ( ( poHashNode->m_dasstrNode.m_ich + poHashNode->m_dasstrNode.m_cch <= f_pdstrXML->cchString ) &&
             ( poHashNode->m_dasstrNode.m_cch >= 5 + f_pdstrTag->cchString * 2 ) &&
             ( f_pdstrXML->pwszString[ poHashNode->m_dasstrNode.m_ich ] == g_wchLessThan ) &&
             ( f_pdstrXML->pwszString[ poHashNode->m_dasstrNode.m_ich + poHashNode->m_dasstrNode.m_cch - 1 ] == g_wchGreaterThan ) && 
             ( DRMCRT_wcsncmp( f_pdstrXML->pwszString + poHashNode->m_dasstrNode.m_ich + 1,
                               f_pdstrTag->pwszString,
                               f_pdstrTag->cchString ) == 0 ) )
        {
            f_pdstrNodeOut->pwszString = f_pdstrXML->pwszString + poHashNode->m_dasstrNode.m_ich;
            f_pdstrNodeOut->cchString = poHashNode->m_dasstrNode.m_cch;
        
            if ( poHashNode->m_dasstrNodeData.m_ich == 0 &&
                 poHashNode->m_dasstrNodeData.m_cch == 0 )
            {
                f_pdstrNodeDataOut->pwszString = NULL;
                f_pdstrNodeDataOut->cchString = 0;
            }
            else
            {
                f_pdstrNodeDataOut->pwszString = f_pdstrXML->pwszString + poHashNode->m_dasstrNodeData.m_ich;
                f_pdstrNodeDataOut->cchString = poHashNode->m_dasstrNodeData.m_cch;
            }
        
            dr = DRM_SUCCESS;
            
            goto ErrorExit;
        }
    }

    dr = DRM_S_FALSE;

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: Oem_XMLHash_CalcParamSig
**
** Synopsis: Function that calculates a signature for the passed in XML
**           string seach parameters and store the result in a
**           DRM_XML_HASH_SIG data structure.
**
** Arguments:
**
** [f_poContext]            -- Pointer to a DRM_XML_HASH_CONTEXT.
** [f_pdstrXML]             -- Pointer to a DRM string that is the source XML
**                             string to be searched.
** [f_pdstrTag]             -- Pointer to a DRM string that is a substring
**                             within f_pdstrXML to be searched.
** [f_iNode]                -- A DWORD value indicating the f_iNode-th occurence
**                             of f_pdstrTag within f_pdstrXML to be seached.
** [f_iLayer]               -- A DWORD number indicating the XML hierarchy levels
**                             to skip down before beginning to match XML tags.
** [f_poSig]                -- Pointer to a DRM_XML_HASH_SIG structure
**                             to receive the calculated signature.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_API Oem_XMLHash_CalcParamSig(
    __in DRM_XML_HASH_CONTEXT *f_poContext,
    __in const DRM_CONST_STRING *f_pdstrXML,
    __in const DRM_CONST_STRING *f_pdstrTag,
    __in const DRM_DWORD f_iNode,
    __in const DRM_DWORD f_iLayer,
    __out DRM_XML_HASH_SIG *f_poSig )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_poContext != NULL );
    ChkDRMString( f_pdstrXML );
    ChkDRMString( f_pdstrTag );
    ChkArg( f_poSig != NULL );

    /* 
    ** The code assumes that the source XML string remains constant
    ** in memory during the life span of the activated XML hash cache.
    ** Therefore the starting address plus the length of the source XML
    ** string can be used to uniquely identify the source XML string.
    */
    f_poSig->m_dwSourceHash1 = ( DRM_DWORD_PTR )f_pdstrXML->pwszString;
    f_poSig->m_dwSourceHash2 = f_pdstrXML->cchString;

    ChkDR( _CalcStringHash( f_poContext,
                            f_pdstrTag,
                            &f_poSig->m_dwTagHash1,
                            &f_poSig->m_dwTagHash2 ) );

#if ENABLE_OEM_XMLHASH_STATISTICS
    f_poSig->m_dwLen = f_pdstrXML->cchString + f_pdstrTag->cchString;
#endif /* ENABLE_OEM_XMLHASH_STATISTICS */

    f_poSig->m_dwNode = f_iNode;
    f_poSig->m_dwLayer = f_iLayer;

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: Oem_XMLHash_GetContext
**
** Synopsis: Function that returns a pointer to a DRM_XML_HASH_CONTEXT
**           that can be used by the current thread.
**           It is possible that the returned pointer is NULL. If that
**           is the case the caller should not try to call other APIs
**           that takes a DRM_XML_HASH_CONTEXT pointer as an input.
**
** Arguments:
**
** [f_ppoContext]           -- Pointer to a variable to receive the 
**                             DRM_XML_HASH_CONTEXT pointer.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_API Oem_XMLHash_GetContext(
    __out DRM_XML_HASH_CONTEXT **f_ppoContext )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_ppoContext != NULL );

    *f_ppoContext = _GetContext();

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: Oem_XMLHash_Start
**
** Synopsis: Function that starts the XML hashing until Oem_XMLHash_Stop
**           is called.
**
** Arguments:               N/A
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_API Oem_XMLHash_Start( DRM_VOID )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_XML_HASH_CONTEXT *poContext = NULL;

    if ( ( poContext = _GetContext() ) == NULL )
    {
        goto ErrorExit;
    }

    ZEROMEM( &poContext->m_oXMLHashStackContext,
             SIZEOF( DRM_STACK_ALLOCATOR_CONTEXT ) );

    ChkDR( DRM_STK_Init( &poContext->m_oXMLHashStackContext,
                         poContext->m_rgbXMLHashStackBuffer,
                         MAX_XML_HASH_STACK ) );

    ZEROMEM( poContext->m_rgpoHashNodes,
             SIZEOF( poContext->m_rgpoHashNodes ) );

    poContext->m_fXMLHashStackInit = TRUE;

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: Oem_XMLHash_Stop
**
** Synopsis: Function that stops the XML hashing started by 
**           Oem_XMLHash_Start.
**
** Arguments:               N/A
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_API Oem_XMLHash_Stop( DRM_VOID )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_XML_HASH_CONTEXT *poContext = NULL;

    if ( ( poContext = _GetContext() ) == NULL )
    {
        goto ErrorExit;
    }

    poContext->m_fXMLHashStackInit = FALSE;

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: Oem_XMLHash_Init
**
** Synopsis: Function that initializes the global variables to support
**           XML hashing.
**
** Arguments:               N/A
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_API Oem_XMLHash_Init( DRM_VOID )
{
    Oem_CritSec_Initialize( &g_oXMLHashCS );

    ZEROMEM( g_rgpoXMLHashContexts, SIZEOF( g_rgpoXMLHashContexts ) );

    g_fInited = TRUE;

    return DRM_SUCCESS;
}

/*********************************************************************
**
** Function: Oem_XMLHash_UnInit
**
** Synopsis: Function that uninitializes the global variables to support
**           XML hashing.
**
** Arguments:               N/A
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_API Oem_XMLHash_UnInit( DRM_VOID )
{
    Oem_CritSec_Delete( &g_oXMLHashCS );

    g_fInited = FALSE;

    return DRM_SUCCESS;
}

/*********************************************************************
**
** Function: Oem_XMLHash_Cleanup
**
** Synopsis: Function that is called by a thread when it finishes to use
**           XML hashing. It can be called when the thread is about to
**           exit or stop using DRM.
**
** Arguments:               N/A
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_API Oem_XMLHash_Cleanup( DRM_VOID )
{
    _FreeContext();

    return DRM_SUCCESS;
}

EXIT_PK_NAMESPACE_CODE;
