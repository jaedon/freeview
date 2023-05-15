/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include "stdafx.h"
#include <PKTestNameSpace.h>

USING_PK_NAMESPACE
USING_PKTEST_NAMESPACE

#include <logger.h>

#include <drmcommon.h>
#include <drmbase64.h>
#include <drmxmlparser.h>
#include <oemaes.h>
#include <oemrsa.h>
#include <drmcrt.h>
#include <trightsparser.h>
#include <drmcert.h>
#include <drmxmrbuilder.h>
#include <oemeccp256.h>
#include <xmrlicgen.h>
#include <tlicgen.h>
#include <tstutils.h>
#include <drmderivedkey.h>

#ifndef TEST_SUPPORT_XMR_EXTENSIBLE_RESTRICTON_STATE_DATE
/*
** XPolicy with state is not supported by server
** and we don't want cptool to have a dependency
** on the expression evaluator.
*/
#define TEST_SUPPORT_XMR_EXTENSIBLE_RESTRICTON_STATE_DATE 0
#endif /* TEST_SUPPORT_XMR_EXTENSIBLE_RESTRICTON_STATE_DATE */

/*Ignore*/
static const DRM_WCHAR g_pwszIgnore[]                   = { WCHAR_CAST('I'), WCHAR_CAST('g'), WCHAR_CAST('n'), WCHAR_CAST('o'), WCHAR_CAST('r'), WCHAR_CAST('e'), WCHAR_CAST('\0') };
/*Copy*/
static const DRM_WCHAR g_pwszCopy[]                     = { WCHAR_CAST('C'), WCHAR_CAST('o'), WCHAR_CAST('p'), WCHAR_CAST('y'), WCHAR_CAST('\0') };
/*NoEmbeddingObject*/
static const DRM_WCHAR g_pwszNoEmbeddingObject[]        = { WCHAR_CAST('N'), WCHAR_CAST('o'), WCHAR_CAST('E'), WCHAR_CAST('m'), WCHAR_CAST('b'), WCHAR_CAST('e'), WCHAR_CAST('d'), WCHAR_CAST('d'), WCHAR_CAST('i'), WCHAR_CAST('n'), WCHAR_CAST('g'), WCHAR_CAST('O'), WCHAR_CAST('b'), WCHAR_CAST('j'), WCHAR_CAST('e'), WCHAR_CAST('c'), WCHAR_CAST('t'), WCHAR_CAST('\0') };
/*NumberOfEntries*/
static const DRM_WCHAR dwchAttribNumberOfEntries[]      = { WCHAR_CAST('n'), WCHAR_CAST('u'), WCHAR_CAST('m'), WCHAR_CAST('b'), WCHAR_CAST('e'), WCHAR_CAST('r'), WCHAR_CAST('_'), WCHAR_CAST('o'), WCHAR_CAST('f'), WCHAR_CAST('_'), WCHAR_CAST('e'), WCHAR_CAST('n'), WCHAR_CAST('t'), WCHAR_CAST('r'), WCHAR_CAST('i'), WCHAR_CAST('e'), WCHAR_CAST('s'), WCHAR_CAST('\0') };
/*key*/
static const DRM_WCHAR dwchAttribKey[]                  = { WCHAR_CAST('k'), WCHAR_CAST('e'), WCHAR_CAST('y'), WCHAR_CAST('\0') };
/*index*/
static const DRM_WCHAR dwchAttribIndex[]                = { WCHAR_CAST('i'), WCHAR_CAST('n'), WCHAR_CAST('d'), WCHAR_CAST('e'), WCHAR_CAST('x'), WCHAR_CAST('\0') };
/*location*/
static const DRM_WCHAR dwchAttribLocation[]             = { WCHAR_CAST('l'), WCHAR_CAST('o'), WCHAR_CAST('c'), WCHAR_CAST('a'), WCHAR_CAST('t'), WCHAR_CAST('i'), WCHAR_CAST('o'), WCHAR_CAST('n'), WCHAR_CAST('\0') };
/*AuxKeyEntry*/
static const DRM_WCHAR dwchTagAuxKeyEntry[]             = { WCHAR_CAST('A'), WCHAR_CAST('u'), WCHAR_CAST('x'), WCHAR_CAST('K'), WCHAR_CAST('e'), WCHAR_CAST('y'), WCHAR_CAST('E'), WCHAR_CAST('n'), WCHAR_CAST('t'), WCHAR_CAST('r'), WCHAR_CAST('y'), WCHAR_CAST('\0') };
/*MID*/
static const DRM_WCHAR dwchMid[]                        = { WCHAR_CAST('M'), WCHAR_CAST('I'), WCHAR_CAST('D'), WCHAR_CAST('\0') };
/*DATA*/
static const DRM_WCHAR dwchData[]                       = { WCHAR_CAST('D'), WCHAR_CAST('A'), WCHAR_CAST('T'), WCHAR_CAST('A'), WCHAR_CAST('\0') };
/*METERCERT*/
static const DRM_WCHAR dwchMETERCERT[]                  = { WCHAR_CAST('M'), WCHAR_CAST('E'), WCHAR_CAST('T'), WCHAR_CAST('E'), WCHAR_CAST('R'), WCHAR_CAST('C'), WCHAR_CAST('E'), WCHAR_CAST('R'), WCHAR_CAST('T'), WCHAR_CAST('\0') };
/*Root*/
static const DRM_WCHAR dwchRoot[]                       = { WCHAR_CAST('R'), WCHAR_CAST('o'), WCHAR_CAST('o'), WCHAR_CAST('t'), WCHAR_CAST('\0') };
/*Leaf*/
static const DRM_WCHAR dwchLeaf[]                       = { WCHAR_CAST('L'), WCHAR_CAST('e'), WCHAR_CAST('a'), WCHAR_CAST('f'), WCHAR_CAST('\0') };
/*1*/
static const DRM_WCHAR dwchXMRVersion1[]                = { WCHAR_CAST('1'), WCHAR_CAST('\0') };
/*2*/
static const DRM_WCHAR dwchXMRVersion2[]                = { WCHAR_CAST('2'), WCHAR_CAST('\0') };
/*3*/
static const DRM_WCHAR dwchXMRVersion3[]                = { WCHAR_CAST('3'), WCHAR_CAST('\0') };
/*NoMinimumSecurityLevel*/
static const DRM_WCHAR dwchNoMinimumSecurityLevel[]     = { WCHAR_CAST('N'), WCHAR_CAST('o'), WCHAR_CAST('M'), WCHAR_CAST('i'), WCHAR_CAST('n'), WCHAR_CAST('i'), WCHAR_CAST('m'), WCHAR_CAST('u'), WCHAR_CAST('m'), WCHAR_CAST('S'), WCHAR_CAST('e'), WCHAR_CAST('c'), WCHAR_CAST('u'), WCHAR_CAST('r'), WCHAR_CAST('i'), WCHAR_CAST('t'), WCHAR_CAST('y'), WCHAR_CAST('L'), WCHAR_CAST('e'), WCHAR_CAST('v'), WCHAR_CAST('e'), WCHAR_CAST('l'), WCHAR_CAST('\0') };
/*Yes*/
static const DRM_WCHAR dwchYes[]                        = { WCHAR_CAST('Y'), WCHAR_CAST('e'), WCHAR_CAST('s'), WCHAR_CAST('\0') };
/*NoRIV*/
static const DRM_WCHAR dwchNoRIV[]                      = { WCHAR_CAST('N'), WCHAR_CAST('o'), WCHAR_CAST('R'), WCHAR_CAST('I'), WCHAR_CAST('V'), WCHAR_CAST('\0') };
/*DWORD*/
static const DRM_WCHAR dwchDWORD[]                      = { WCHAR_CAST('D'), WCHAR_CAST('W'), WCHAR_CAST('O'), WCHAR_CAST('R'), WCHAR_CAST('D'), WCHAR_CAST('\0') };
/*WCHAR*/
static const DRM_WCHAR dwchWCHAR[]                      = { WCHAR_CAST('W'), WCHAR_CAST('C'), WCHAR_CAST('H'), WCHAR_CAST('A'), WCHAR_CAST('R'), WCHAR_CAST('\0') };
/*BINARY*/
static const DRM_WCHAR dwchBINARY[]                     = { WCHAR_CAST('B'), WCHAR_CAST('I'), WCHAR_CAST('N'), WCHAR_CAST('A'), WCHAR_CAST('R'), WCHAR_CAST('Y'), WCHAR_CAST('\0') };

static DRM_BYTE                 g_bPubKeyECC[ ECC_P256_POINT_SIZE_IN_BYTES ]    = { 0 };
static DRM_DWORD                g_cbPubKeyECC                                   = 0;
static const DRM_DWORD          g_dwPubModulus                                  = 65537;
static const DRM_CONST_STRING   g_cszMID                                        = CREATE_DRM_STRING(dwchMid);
static const DRM_CONST_STRING   g_cszData                                       = CREATE_DRM_STRING(dwchData);
static const DRM_CONST_STRING   g_cszMETERCERT                                  = CREATE_DRM_STRING(dwchMETERCERT);

/*"zWVMVj8hZun5DyUU38zET5B1E4sl6QsmPIrqzkDP"*/
const DRM_WCHAR g_rgwchLicGenDefaultKeySeedCocktail[] = { 
    WCHAR_CAST('z'), WCHAR_CAST('W'), WCHAR_CAST('V'), WCHAR_CAST('M'),
    WCHAR_CAST('V'), WCHAR_CAST('j'), WCHAR_CAST('8'), WCHAR_CAST('h'),
    WCHAR_CAST('Z'), WCHAR_CAST('u'), WCHAR_CAST('n'), WCHAR_CAST('5'),
    WCHAR_CAST('D'), WCHAR_CAST('y'), WCHAR_CAST('U'), WCHAR_CAST('U'),
    WCHAR_CAST('3'), WCHAR_CAST('8'), WCHAR_CAST('z'), WCHAR_CAST('E'),
    WCHAR_CAST('T'), WCHAR_CAST('5'), WCHAR_CAST('B'), WCHAR_CAST('1'),
    WCHAR_CAST('E'), WCHAR_CAST('4'), WCHAR_CAST('s'), WCHAR_CAST('l'),
    WCHAR_CAST('6'), WCHAR_CAST('Q'), WCHAR_CAST('s'), WCHAR_CAST('m'),
    WCHAR_CAST('P'), WCHAR_CAST('I'), WCHAR_CAST('r'), WCHAR_CAST('q'),
    WCHAR_CAST('z'), WCHAR_CAST('k'), WCHAR_CAST('D'), WCHAR_CAST('P'),
    WCHAR_CAST('\0') };

/*"XVBovsmzhP9gRIZxWfFta3VVRPzVEWmJsazEJ46I"*/
const DRM_WCHAR g_rgwchLicGenDefaultKeySeedAES[] = { 
    WCHAR_CAST('X'), WCHAR_CAST('V'), WCHAR_CAST('B'), WCHAR_CAST('o'),
    WCHAR_CAST('v'), WCHAR_CAST('s'), WCHAR_CAST('m'), WCHAR_CAST('z'),
    WCHAR_CAST('h'), WCHAR_CAST('P'), WCHAR_CAST('9'), WCHAR_CAST('g'),
    WCHAR_CAST('R'), WCHAR_CAST('I'), WCHAR_CAST('Z'), WCHAR_CAST('x'),
    WCHAR_CAST('W'), WCHAR_CAST('f'), WCHAR_CAST('F'), WCHAR_CAST('t'),
    WCHAR_CAST('a'), WCHAR_CAST('3'), WCHAR_CAST('V'), WCHAR_CAST('V'),
    WCHAR_CAST('R'), WCHAR_CAST('P'), WCHAR_CAST('z'), WCHAR_CAST('V'),
    WCHAR_CAST('E'), WCHAR_CAST('W'), WCHAR_CAST('m'), WCHAR_CAST('J'),
    WCHAR_CAST('s'), WCHAR_CAST('a'), WCHAR_CAST('z'), WCHAR_CAST('E'),
    WCHAR_CAST('J'), WCHAR_CAST('4'), WCHAR_CAST('6'), WCHAR_CAST('I'),
    WCHAR_CAST('\0') };

/*"QCl1EY0ISE2qXlCDaIMJ0A=="*/
const DRM_WCHAR g_rgwchLicGenDefaultKID[] = {
    WCHAR_CAST('Q'), WCHAR_CAST('C'), WCHAR_CAST('l'), WCHAR_CAST('1'),
    WCHAR_CAST('E'), WCHAR_CAST('Y'), WCHAR_CAST('0'), WCHAR_CAST('I'),
    WCHAR_CAST('S'), WCHAR_CAST('E'), WCHAR_CAST('2'), WCHAR_CAST('q'),
    WCHAR_CAST('X'), WCHAR_CAST('l'), WCHAR_CAST('C'), WCHAR_CAST('D'),
    WCHAR_CAST('a'), WCHAR_CAST('I'), WCHAR_CAST('M'), WCHAR_CAST('J'),
    WCHAR_CAST('0'), WCHAR_CAST('A'), WCHAR_CAST('='), WCHAR_CAST('='),
    WCHAR_CAST('\0') };

/*"Acl2BY3ITE0qe11DalmK0D=="*/
const DRM_WCHAR g_rgwchLicGenDefaultUplinkKID[] = {
    WCHAR_CAST('A'), WCHAR_CAST('c'), WCHAR_CAST('l'), WCHAR_CAST('2'),
    WCHAR_CAST('B'), WCHAR_CAST('Y'), WCHAR_CAST('3'), WCHAR_CAST('I'),
    WCHAR_CAST('T'), WCHAR_CAST('E'), WCHAR_CAST('0'), WCHAR_CAST('q'),
    WCHAR_CAST('e'), WCHAR_CAST('1'), WCHAR_CAST('1'), WCHAR_CAST('D'),
    WCHAR_CAST('a'), WCHAR_CAST('l'), WCHAR_CAST('m'), WCHAR_CAST('K'),
    WCHAR_CAST('0'), WCHAR_CAST('D'), WCHAR_CAST('='), WCHAR_CAST('='),
    WCHAR_CAST('\0') };

DRM_CONST_STRING g_dstrDefaultCocktailKeySeed           = CREATE_DRM_STRING(g_rgwchLicGenDefaultKeySeedCocktail);
DRM_CONST_STRING g_dstrDefaultAESKeySeed                = CREATE_DRM_STRING(g_rgwchLicGenDefaultKeySeedAES);
static DRM_CONST_STRING g_dstrDefaultKid                = CREATE_DRM_STRING(g_rgwchLicGenDefaultKID);
static DRM_CONST_STRING g_dstrDefaultUplinkKid          = CREATE_DRM_STRING(g_rgwchLicGenDefaultUplinkKID);
DRM_CONST_STRING g_dstrAttribNumberOfEntries            = CREATE_DRM_STRING(dwchAttribNumberOfEntries);
DRM_CONST_STRING g_dstrAttribKey                        = CREATE_DRM_STRING(dwchAttribKey);
DRM_CONST_STRING g_dstrAttribIndex                      = CREATE_DRM_STRING(dwchAttribIndex);
DRM_CONST_STRING g_dstrAttribLocation                   = CREATE_DRM_STRING(dwchAttribLocation);
DRM_CONST_STRING g_dstrTagAuxKeyEntry                   = CREATE_DRM_STRING(dwchTagAuxKeyEntry);
DRM_DWORD        g_dwClearToBaseRotateCounter           = 0x12345678;
DRM_DWORD        g_dwClearToSecondKeyRotateCounter      = 0x9ABCDEF0;

#define STACK_SIZE 10000

/*****************************************************************************
** Function: _tRotateContentKey
**
** Synopsis: Rotates a content key as done by OCURs.
**
** Arguments:   f_pbBaseContentKey - the key to rotate.
**              f_cbBaseContentKey - the length of the key pointed to by pbBaseContentKey
**              f_dwCounter - the counter value to rotate by
**              f_pbKey - buffer to put the rotated key
**              f_cbKey - Length of the buffer pointed to by pbKey
**
** Returns:
**          DRM_SUCCESS or an error code
*****************************************************************************/
DRM_RESULT DRM_CALL _tRotateContentKey( __in DRM_BYTE                *f_pbBaseContentKey,
                                        __in DRM_DWORD                f_cbBaseContentKey,
                                        __in DRM_DWORD                f_dwCounter,
                                        __out_bcount(cbKey) DRM_BYTE *f_pbKey,
                                        __in DRM_DWORD                f_cbKey)
{
    DRM_RESULT      dr           = DRM_SUCCESS;
    DRM_BYTE        shaContentKeyDigest[ DRM_SHA_DIGEST_LEN_MAX ] = {0};
    DRM_ID          drmidCounter = {0};
    DRM_SHA_CONTEXT ShaContext   = {0};

    ChkArg( f_pbBaseContentKey != NULL );
    ChkArg( f_pbKey != NULL );
    ChkArg( f_cbKey <=  DRM_SHA_DIGEST_LEN_MAX ); /* cbKey is supposed to be <= DRM_AES_KEYSIZE_128 which is < DRM_SHA_DIGEST_LEN_MAX */

    DWORD_TO_NETWORKBYTES( drmidCounter.rgb,
                           SIZEOF( drmidCounter.rgb ) - SIZEOF( f_dwCounter ),
                           f_dwCounter );

    DRM_SHA_Init( &ShaContext, eDRM_SHA_1 );
    DRM_SHA_Update( f_pbBaseContentKey, f_cbBaseContentKey, &ShaContext );
    DRM_SHA_Update( drmidCounter.rgb, SIZEOF(drmidCounter.rgb), &ShaContext );
    DRM_SHA_Finalize( &ShaContext, DRM_SHA_DIGEST_LEN_MAX, shaContentKeyDigest );

    memcpy( f_pbKey, shaContentKeyDigest, f_cbKey );

ErrorExit:
    return dr;
}

DRM_RESULT _MakeRandomKID(DRM_CONST_STRING *pKID)
{

    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_CHAR   szKID[24] = {0};
    DRM_DWORD  cbKID     = SIZEOF(szKID);
    DRM_GUID   keyGuid   = {0};

    ChkArg( pKID != NULL );
    FREEDRMSTRINGPOINTER( pKID );

    ChkDR(Oem_Random_GetBytes(NULL, (DRM_BYTE*)&keyGuid, SIZEOF(DRM_GUID)));

    ChkMem( pKID->pwszString = ( DRM_WCHAR * )Oem_MemAlloc( 24 * SIZEOF( DRM_WCHAR ) ) );
    pKID->cchString = cbKID;
    ChkDR(DRM_B64_EncodeW((DRM_BYTE *)&keyGuid,
                          SIZEOF(DRM_GUID),
                          (DRM_WCHAR *)pKID->pwszString,
                          &(pKID->cchString),
                          0));


ErrorExit:
    return dr;
}

/**
    name@ _ReverseBytes
    desc@ flip over the buffer
    param@ pbData - the buffer have th data need to be flip over
    param@ cbData - the length of the buffer
    return@ void
*/

void _ReverseBytes(
    __in __out  DRM_BYTE  *pbData,
    __in        DRM_DWORD  cbData )
{
    DRM_BYTE bTemp;
    DRM_DWORD i;

    if( cbData < 2 )
    {
        return;
    }

    cbData--;
    for( i=0; i<cbData; i++,cbData-- )
    {
        bTemp = GET_BYTE(pbData, i);
        PUT_BYTE(pbData, i, GET_BYTE(pbData, cbData));
        PUT_BYTE(pbData, cbData, bTemp);
    }
}


/**
    name@ _GetNonRandomBytes
    desc@ Fills a buffer with non-random bytes. Starts at 0x00 and wrapps at 0xFF
    param@ pBuffer - the buffer to fill
    param@ cbLen - the length of the buffer
    return@ a valid DRM_RESULT value.
*/
static DRM_RESULT _GetNonRandomBytes(DRM_BYTE *pBuffer, DRM_DWORD cbLen)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE val = 0;
    DRM_DWORD index = 0;

    ChkArg(pBuffer);

    for (index = 0; index < cbLen; index++)
    {
        pBuffer[index] = val;
        val++;  /* don't care about wraparound. */
    }

ErrorExit:
    return val;
}

/**
    name@ _GetNumberOfSecondsInTimeString
    desc@ Calculates the number of seconds represented in time string. in the format "1d 20h 5m 45s 2"
            d - days
            h - hours
            m - minutes
            s - seconds [default]
    param@ pTimeString - the time string
    param@ pSeconds pointer to return the seconds in
    return@ A valid DRM_RESULT value
*/
static DRM_RESULT _GetNumberOfSecondsInTimeString(const DRM_WCHAR *pTimeString, DRM_DWORD *pSeconds)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD dwSeconds = 0;
    DRM_DWORD dwTemp = 0;
    const DRM_WCHAR *pTemp = 0;
    DRM_WCHAR buffer[9] = {0};
    DRM_DWORD index = 0;

    ChkArg(pTimeString);
    ChkArg(pSeconds);

    pTemp = pTimeString;

    while (*pTemp)
    {
        while (*pTemp && DRMCRT_iswspace(*pTemp))
        {
            pTemp++;
            continue;
        }

        index = 0;
        ZEROMEM( buffer, SIZEOF( buffer ) );
        while (DRMCRT_iswdigit(*pTemp) && index < 9)
        {
            buffer[index] = *pTemp;
            index++;
            pTemp++;
        }
        ChkDR( DRMCRT_WtoDWORD(buffer, index, 10, &dwTemp, NULL ) );
        if(*pTemp == WCHAR_CAST('d') )
        {
            dwTemp *= 3600 * 24;
        }
        else if(*pTemp == WCHAR_CAST('h') )
        {
            dwTemp *= 3600;
        }
        else if(*pTemp == WCHAR_CAST('m') )
        {
            dwTemp *= 60;
        }
        
        if ( *pTemp != g_wchNull )
        {
            pTemp++;
        }
        dwSeconds += dwTemp;
    }

    *pSeconds = dwSeconds;
ErrorExit:
    return dr;
}


DRM_RESULT tSetECCPubKey(DRM_BYTE *pPubKey, DRM_DWORD cbPubKey)
{
    DRM_RESULT dr = DRM_SUCCESS;

    if( pPubKey != NULL )
    {
        ChkArg( cbPubKey == ECC_P256_POINT_SIZE_IN_BYTES );

        memcpy( g_bPubKeyECC, pPubKey, cbPubKey );

        g_cbPubKeyECC = cbPubKey;
    }
    else
    {
        g_cbPubKeyECC = 0;
    }

ErrorExit:
    return dr;
}


DRM_RESULT TestFreeXMRLicenseList( XMRLICENSELINKLIST *pList)
{
    DRM_RESULT dr = DRM_SUCCESS;

    if( pList == NULL )
    {
        goto ErrorExit;
    }

    if (pList->pNext)
    {
        TestFreeXMRLicenseList( (XMRLICENSELINKLIST*)pList->pNext );
    }

    if (pList->pLeaf)
    {
        TestFreeXMRLicenseList((XMRLICENSELINKLIST*)pList->pLeaf);
    }

    SAFE_OEM_FREE( pList->pLicense );

    FREEDRMSTRING( pList->szRevInfoForLicenseResponse );
    pList->szRevInfoForLicenseResponse.pwszString = NULL;
    pList->szRevInfoForLicenseResponse.cchString = 0;

    SAFE_OEM_FREE( pList );

ErrorExit:
    return dr;
}

#define     DRM_ENCRYPTED_KEYLEN    128
#define     DRM_AES_KEYLEN          16

/******************************************************************************
**
** Function :   _CreateOMACSignature
**
** Synopsis :   This is implementation of DRM_XMR_CB_SIGNATURE callback function
**
** Arguments :  f_pvSignatureContext  - Context needed by caller to retrieve
**                                      appropriate integrity key
**              f_pbData              - Pointer to data to sign
**              f_cbData              - Size of data buffer
**              f_pbSignatureBuffer   - Pointer to output signature buffer
**              f_cbSignatureBuffer   - Size of signature buffer
**              f_ibSignatureBuffer   - Offset (index) in signature buffer
**                                      where to start signature writing
**
** Returns :    DRM_SUCCESS if signing was successful
**
******************************************************************************/
static DRM_RESULT DRM_CALL _CreateOMACSignature(
    __in                                const DRM_VOID  *f_pvSignatureContext,
    __in_bcount( f_cbData )             const DRM_BYTE  *f_pbData,
    __in                                const DRM_DWORD  f_cbData,
    __out_bcount( f_cbSignatureBuffer )       DRM_BYTE  *f_pbSignatureBuffer,
    __in                                const DRM_DWORD  f_cbSignatureBuffer,
    __in                                const DRM_DWORD  f_ibSignatureBuffer )
{
    DRM_RESULT  dr              = DRM_SUCCESS;
    DRM_AES_KEY oIntegrityKey   = {0};


    ChkArg( f_pvSignatureContext != NULL );
    ChkArg( f_pbSignatureBuffer  != NULL );
    ChkArg( f_pbData             != NULL );
    ChkBOOL( f_cbSignatureBuffer - f_ibSignatureBuffer >= XMR_AES_OMAC_SIGNATURE_LENGTH,
             DRM_E_BUFFERTOOSMALL);

    /*
    ** get the integrity key from the content integrity key
    */

    ChkDR( Oem_Aes_SetKey( ( DRM_BYTE * )f_pvSignatureContext,
                           &oIntegrityKey ) );

    ChkDR( Oem_Omac1_Sign( &oIntegrityKey,
                           f_pbData,
                           0,
                           f_cbData,
                           f_pbSignatureBuffer + f_ibSignatureBuffer ) );

ErrorExit:
    return dr;
}


/******************************************************************************
    name@ _AddAnyUnknownObject
    desc@ - Adds unknown object to the builder tree even if it's invalid one
           This API can be used to add either a leaf object, or a container structure
    param@ f_pcontextBuilder -
    param@ f_wObjectType   - type
    param@ f_fDuplicateAllowed-
    param@ f_wParent       - parents of this object
    param@ f_wFlags        - flag
    param@ f_pbObject      - pointer to the object
    param@ f_cbObject      - length of the object
/******************************************************************************/
DRM_RESULT _AddAnyUnknownObject(
    __inout_ecount( 1 )                 DRM_XMR_BUILDER_CONTEXT *f_pcontextBuilder,
    __in                                DRM_WORD                 f_wObjectType,
    __in                                DRM_BOOL                 f_fDuplicateAllowed,
    __in                                DRM_WORD                 f_wParent,
    __in                                DRM_WORD                 f_wFlags,
    __in_bcount_opt( f_cbObject ) const DRM_BYTE                *f_pbObject,
    __in                                DRM_DWORD                f_cbObject )
{
    DRM_RESULT dr  = DRM_SUCCESS;
    DRM_XMR_BUILDER_UNKNOWNOBJECT_LISTNODE *plistnode  = NULL;
    DRM_XMR_BUILDER_CONTEXT_INTERNAL       *pcontextBuilder =
            ( DRM_XMR_BUILDER_CONTEXT_INTERNAL * ) f_pcontextBuilder;

    ChkArg( pcontextBuilder != NULL );

    ChkArg( f_pbObject != NULL || f_wFlags & XMR_FLAGS_CONTAINER );

    ChkDR( DRM_STK_Alloc( &pcontextBuilder->contextStack,
        SIZEOF( DRM_XMR_BUILDER_UNKNOWNOBJECT_LISTNODE ),
        ( DRM_VOID ** )&plistnode ) );

    ZEROMEM( plistnode, SIZEOF( DRM_XMR_BUILDER_UNKNOWNOBJECT_LISTNODE ) );

    plistnode->listNode.Node.wType      = f_wObjectType;
    plistnode->listNode.Node.cbLength   = XMR_BASE_OBJECT_LENGTH + f_cbObject;
    plistnode->listNode.Node.pvObject   = f_pbObject;
    plistnode->wParent                  = f_wParent;
    plistnode->pNext                    = NULL;
    plistnode->fDuplicateAllowed        = f_fDuplicateAllowed;
    plistnode->wFlags                   = f_wFlags;

    /*
    ** Add it to linked list
    */
    plistnode->pNext    = pcontextBuilder->pUnknownObjects;
    pcontextBuilder->pUnknownObjects = plistnode;

    ChkDR( DRM_XMR_AddUnknownHierarchy ( pcontextBuilder, plistnode ) );

ErrorExit:
    if ( DRM_FAILED( dr ) && ( plistnode != NULL ) )
    {
        ( void ) DRM_STK_Free( &pcontextBuilder->contextStack,
            ( DRM_VOID ** )&plistnode );
    }
    return dr;
}


DRM_VOID _DRM_XMR_Serialize_UnknownBaseObject(
    __in       DRM_WORD    f_wType,
    __in       DRM_WORD    f_wFlags,
    __in       DRM_DWORD   f_dwLength,
    __in __out DRM_BYTE   *f_pbBuffer,
    __in __out DRM_DWORD  *f_piBuffer )
{
    DRM_DWORD   dwIndex = *f_piBuffer;
    WORD_TO_NETWORKBYTES( f_pbBuffer, dwIndex, f_wFlags );
    dwIndex += SIZEOF( DRM_WORD );
    WORD_TO_NETWORKBYTES( f_pbBuffer, dwIndex, f_wType );
    dwIndex += SIZEOF( DRM_WORD );
    DWORD_TO_NETWORKBYTES( f_pbBuffer, dwIndex, f_dwLength );
    dwIndex += SIZEOF( DRM_DWORD );
    *f_piBuffer = dwIndex;
}

/*****************************************************************************
** Function: _GetAuxKeyEntries
**
** Synopsis: Generates an array of DRM_XMR_AUX_KEY_ENTRY objects based on data in an XML blob
**
** Arguments:   f_pdstrParent - The XML data blob
**              f_cbBaseKey - the length of base key used to derive license from
**              f_pbBaseKey - the base key to derive licenses from.  Can be overriden by data in the XML blob
**              f_pdwEntries - returns the number of DRM_XMR_AUX_KEY_ENTRY objects created
**              f_ppAuxKeyEntries - returns the array of DRM_XMR_AUX_KEY_ENTRY objects.  Caller frees using SAFE_OEM_FREE
**
** Returns:
**          DRM_SUCCESS or an error code
*****************************************************************************/
DRM_RESULT _GetAuxKeyEntries( __in DRM_CONST_STRING                                *f_pdstrParent,
                              __in DRM_DWORD                                        f_cbBaseKey,
                              __in_bcount( f_cbBaseKey ) DRM_BYTE                  *f_pbBaseKey,
                              __out DRM_DWORD                                      *f_pdwEntries,
                              __out_ecount( *f_pdwEntries ) DRM_XMR_AUX_KEY_ENTRY **f_ppAuxKeyEntries )
{
    DRM_RESULT             dr                                = DRM_SUCCESS;
    DRM_DWORD              dwLen                             = 0;
    DRM_DWORD              iAuxKeyEntry                      = 0;
    DRM_BYTE               rgbBaseKey[DRM_AES_KEYSIZE_128]   = {0};
    DRM_DWORD              dwLocation                        = 0;
    DRM_DWORD              dwIndex                           = 0;
    DRM_BYTE               rgbKey[DRM_AES_KEYSIZE_128]       = {0};
    DRM_CONST_STRING       dstrDWORD                         = EMPTY_DRM_STRING;
    DRM_CONST_STRING       dstrKey                           = EMPTY_DRM_STRING;
    DRM_CONST_STRING       dstrAuxKeyEntryNode               = EMPTY_DRM_STRING;
    DRM_XMR_AUX_KEY_ENTRY *pAuxKeyEntries                    = NULL;

    ChkArg( f_pdstrParent != NULL );
    ChkArg( f_pdwEntries != NULL );
    ChkArg( f_ppAuxKeyEntries != NULL );
    ChkArg( f_pbBaseKey != NULL );
    ChkArg( f_cbBaseKey == DRM_AES_KEYSIZE_128 );

    *f_pdwEntries = 0;
    *f_ppAuxKeyEntries = NULL;
    memcpy( rgbBaseKey, f_pbBaseKey, DRM_AES_KEYSIZE_128 );

    ChkDR( DRM_XML_GetNodeAttribute( f_pdstrParent,
                                    &g_dstrAttribNumberOfEntries,
                                    &dstrDWORD ) );

    ChkDR( DRMCRT_WtoDWORD( dstrDWORD.pwszString, dstrDWORD.cchString, 10, f_pdwEntries, NULL ) );

    if ( *f_pdwEntries > 0 )
    {
        dwLen = (*f_pdwEntries) * SIZEOF( DRM_XMR_AUX_KEY_ENTRY );
        ChkMem( pAuxKeyEntries = ( DRM_XMR_AUX_KEY_ENTRY * )Oem_MemAlloc( dwLen ) );
        ZEROMEM( pAuxKeyEntries, dwLen );
        *f_ppAuxKeyEntries = pAuxKeyEntries;

        dr = DRM_XML_GetNodeAttribute( f_pdstrParent,
                                      &g_dstrAttribKey,
                                      &dstrKey );
        if( dr == DRM_SUCCESS )
        {
            dwLen = SIZEOF( rgbBaseKey );
            ChkDR( DRM_B64_DecodeW( &dstrKey, &dwLen, rgbBaseKey, 0) );
        }

        dr = DRM_XML_GetSubNode( f_pdstrParent,
                                &g_dstrTagAuxKeyEntry,
                                 NULL,
                                 NULL,
                                 iAuxKeyEntry,
                                &dstrAuxKeyEntryNode,
                                 NULL,
                                 1 );
        if( dr != DRM_SUCCESS )
        {
            /* create virtual aux key entry nodes */
            for( iAuxKeyEntry = 0; iAuxKeyEntry < *f_pdwEntries; iAuxKeyEntry++ )
            {
                ChkDR( TSTUTIL_DeriveAuxKey( iAuxKeyEntry, rgbBaseKey, rgbKey ) );
                pAuxKeyEntries[iAuxKeyEntry].dwLocation = iAuxKeyEntry;
                memcpy( pAuxKeyEntries[iAuxKeyEntry].rgbKey, rgbKey, DRM_AES_KEYSIZE_128 );
            }
        }
        else
        {
            do
            {
                ChkDR( DRM_XML_GetNodeAttribute( &dstrAuxKeyEntryNode,
                                                 &g_dstrAttribIndex,
                                                 &dstrDWORD ) );

                ChkDR( DRMCRT_WtoDWORD( dstrDWORD.pwszString, dstrDWORD.cchString, 10, &dwIndex, NULL ) );

                ChkBOOL( dwIndex < *f_pdwEntries, DRM_E_BUFFER_BOUNDS_EXCEEDED );

                dr = DRM_XML_GetNodeAttribute( &dstrAuxKeyEntryNode,
                                               &g_dstrAttribLocation,
                                               &dstrDWORD );
                if( dr == DRM_SUCCESS )
                {
                    ChkDR( DRMCRT_WtoDWORD( dstrDWORD.pwszString, dstrDWORD.cchString, 10, &dwLocation, NULL ) );
                }
                else
                {
                    dwLocation = iAuxKeyEntry;
                }

                pAuxKeyEntries[dwIndex].dwLocation = dwLocation;

                dr = DRM_XML_GetNodeAttribute( &dstrAuxKeyEntryNode,
                                               &g_dstrAttribKey,
                                               &dstrKey );
                if( dr == DRM_SUCCESS && dstrKey.pwszString[0] != WCHAR_CAST('*') )
                {
                    dwLen = SIZEOF( rgbKey );
                    ChkDR( DRM_B64_DecodeW(&dstrKey, &dwLen, rgbKey, 0) );
                }
                else
                {
                    ChkDR( TSTUTIL_DeriveAuxKey( dwLocation, rgbBaseKey, rgbKey ) );
                }
                memcpy( pAuxKeyEntries[dwIndex].rgbKey, rgbKey, DRM_AES_KEYSIZE_128 );

                iAuxKeyEntry++;
                dr = DRM_XML_GetSubNode( f_pdstrParent,
                                        &g_dstrTagAuxKeyEntry,
                                         NULL,
                                         NULL,
                                         iAuxKeyEntry,
                                        &dstrAuxKeyEntryNode,
                                         NULL,
                                         1 );
            } while( dr == DRM_SUCCESS );
            if( dr == DRM_E_XMLNOTFOUND )
            {
                dr = DRM_SUCCESS;
            }
            ChkDR( dr );
       }
   }

ErrorExit:
    return dr;
}

DRM_RESULT IsValidExtensibleRightRestriction( __in DRM_WORD wRFlags )
{
    DRM_RESULT   dr  = DRM_SUCCESS;

    /* Verify that object ID is a valid extensible right/restriction */
    ChkArg( ( ( wRFlags & XMR_OBJECT_TYPE_RINGTONE_POLICY_CONTAINER )
           || ( wRFlags & XMR_OBJECT_TYPE_RINGTONE_ENABLER_OBJECT   )
           || ( wRFlags & XMR_OBJECT_TYPE_EXECUTE_POLICY_CONTAINER  ) ) );

ErrorExit:
    return dr;
}

DRM_RESULT FreeOptionalTestMakeXMRLicenseParams( __in OptionalTestMakeXMRLicenseParams* f_pOptparams )
{
    DRM_RESULT dr = DRM_SUCCESS;
    ChkArg( NULL != f_pOptparams );
    
    FREEDRMSTRING( f_pOptparams->dstrKIDUsed );
    FREEDRMSTRING( f_pOptparams->dstrRootKIDUsed );
    FREEDRMSTRING( f_pOptparams->dstrLIDUsed );

ErrorExit:
    return dr;        
}

DRM_RESULT _MakeXMRLicense(
    DRM_DWORD dwOptions,
    DRM_CONST_STRING *pdstrRootKID,
    LICENSERIGHTS *pRights,
    XMRLICENSELINKLIST **ppList,
    DRM_CONST_STRING *pdstrKIDUsed,
    OptionalTestMakeXMRLicenseParams *pOptionalParams
 )
{

    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE abBaseLicStack[STACK_SIZE] = {0};
    DRM_BYTE *pPubKeyModulus = 0;
    DRM_BYTE * pbClear = 0;
    DRM_DWORD cbClear = 0;
    DRM_BYTE * pbClearRoot = 0;
    DRM_DWORD cbClearRoot = 0;
    DRM_BYTE bCheckSum[64] = {0};
    DRM_DWORD cbCheckSum = 0;
    DRM_BYTE *pbLicense = NULL;
    DRM_DWORD cbLicense = 0;
    DRM_BYTE   *pbSerialNumber = NULL;
    DRM_DWORD cbSerialNumber = 0;
    XMRLICENSELINKLIST *pList = NULL;
    DRM_WCHAR wszExpirationDate[64] = {0};
    DRM_WCHAR wszBeginDate[64] = {0};
    DRM_WCHAR wszRemovalDate[64] = {0};
    DRM_WCHAR wszIssueDate[64] = {0};
    DRM_CONST_STRING *pdstrSeed = 0;
    DRM_CONST_STRING *pdstrKID = 0;
    DRM_CONST_STRING dstrRandomKID = EMPTY_DRM_STRING;
    ELicenseType eLicenseType = MAKE_SIMPLE_LICENSE;
    DRM_BYTE rgbKeyMaterial[DRM_AES_KEYSIZE_128*2] = {0};
    DRM_BYTE rgbUplinkKey[DRM_AES_KEYSIZE_128*2] = {0};
    DRM_BYTE rgbDestKey[DRM_AES_KEYSIZE_128*2] = {0};
    DRM_BYTE rgbIntegrityKey[DRM_AES_KEYSIZE_128*2] = {0};
    DRM_BYTE abEncryptedKeyBlob[DRM_RSA_CB_CIPHERTEXT( eDRM_RSA_1024 ) + DRM_AES_KEYSIZE_128] = {0};
    DRM_DWORD cbEncryptedKeyBlob = 0;
    DRM_AES_KEY AesUplinkKey;
    DRM_KID drmKID = {0};
    DRM_KID drmUplinkKID = {0};
    DRM_LID drmLID = {0};
    DRM_MID drmMID = {0};
    DRM_DWORD cb = 0;
    DRM_CONST_STRING * pdstrUplinkKID = 0;
    DRM_CONST_STRING dstrRandomRootKID = EMPTY_DRM_STRING;
    DRM_DWORD cbLen = 0;
    DRMFILETIME ft = {0};
    DRM_CONST_STRING dstrLID = EMPTY_DRM_STRING;
    DRM_BYTE  rgbLGPubKeyModulus [__CB_DECL(DRM_RSA_CB_MODULUS( eDRM_RSA_1024 ) * 2 + 8)] = { 0 };
    DRM_DWORD cbPubKeyModulus = SIZEOF(rgbLGPubKeyModulus);
    DRM_BYTE *pbPubKeyECC = NULL;
    DRM_BOOL fNoEmbeddingObject = FALSE;
    DRM_DWORD dwXmrVersion = XMR_VERSION_INVALID;
    DRM_DWORD idx         = 0;
    DRM_XMR_RIGHTS xmrRights = {0};
    DRM_XMR_CONTENT_KEY xmrContentKey = {0};
    DRM_XMR_ECC_DEVICE_KEY xmrDeviceKey = {0};
    DRM_XMR_BUILDER_CONTEXT xmrBuilder = {0};
    DRM_XMR_MINIMUM_ENVIRONMENT xmrMinimumEnvironment = {0};
    DRM_XMR_EXPIRATION_AFTER_FIRSTSTORE xmrExpirationAfterFirstStore = {0};
    DRM_XMR_EXPIRATION_AFTER_FIRSTUSE xmrExpirationAfterFirstUse = {0};
    DRM_XMR_DWORD_VERSIONED xmrExpirationAfterFirstPlay = {0};
    DRM_XMR_EXPIRATION xmrExpiration = {0};
    DRM_XMR_REMOVAL_DATE xmrRemovalDate = {0};
    DRM_XMR_ISSUEDATE xmrIssueDate = {0};
    DRM_XMR_PRIORITY xmrPriority = {0};
    DRM_XMR_RESTRICTED_SOURCEID xmrRestrictedSourceID = {0};
    DRM_XMR_MINIMUM_OUTPUT_PROTECTION_LEVELS opl = {0};
    DRM_XMR_VIDEO_OUTPUT_CONFIGURATION_LIST *pxmrExplicitAnalogVideoTemp = 0;
    DRM_XMR_EXPLICIT_ANALOG_VIDEO_PROTECTION xmrExplicitAnalogVideoContainer = {0};
    DRM_XMR_METERING xmrMetering = {0};
    DRM_XMR_PLAYBACK_RIGHTS xmrPlayRights = {0};
    DRM_XMR_PLAYCOUNT xmrPlayCount = {0};
    DRM_XMR_COPY_RIGHTS xmrCopyRights = {0};
    DRM_XMR_COPY_COUNT xmrCopyCount = {0};
    DRM_XMR_PLAYLIST_BURN_RIGHTS xmrPlayListBurn = {0};
    DRM_XMR_PLAYLIST_BURN_RESTRICTIONS xmrBurnCount = {0};
    DRM_XMR_UPLINK_KID xmrUplink = {0};
    DRM_XMR_SOURCEID xmrSourceID = {0};
    DRM_XMR_INCLUSION_LIST xmrInclusionList = {0};
    DRM_XMR_GRACE_PERIOD xmrGracePeriod = {0};
    DRM_XMR_USERID xmrUID = {0};
    DRM_XMR_RSA_PUBKEY xmrLGPubKey = {0};
    DRM_XMR_REVOCATION_INFORMATION_VERSION xmrRiv = {0};
    DRM_XMR_EXPLICIT_DIGITAL_AUDIO_PROTECTION xmrExplicitDigitalAudioContainer = {0};
    DRM_XMR_AUDIO_OUTPUT_CONFIGURATION_LIST *pxmrExplicitDigitalAudioTemp = 0;
    DRM_XMR_OUTPUT_CONFIGURATION_LIST  *pxmrPolicyMetadataTemp = 0;
    DRM_XMR_OUTPUT_CONFIGURATION_LIST  *pxmrPolicyMetadataList = 0;
    DRM_XMR_EMBEDDING_BEHAVIOR xmrEmbeddingBehavior = {0};
    DRM_XMR_DOMAIN_ID xmrDomainID = {0};
    DRM_GUID guidAccountID = {0};

    DRM_XMR_SERIAL_NUMBER xmrSerialNumber = {0};

    DRM_XMR_OUTPUT_CONFIGURATION_LIST  *pxmrPlayEnablerGUIDTemp = 0;
    DRM_XMR_OUTPUT_CONFIGURATION_LIST  *pxmrPlayEnablerGUIDList = 0;

    DRM_XMR_COPY_RIGHTS xmrCopyRights2 = {0};

    DRM_XMR_OUTPUT_CONFIGURATION_LIST  *pxmrCopyEnablerGUIDTemp = 0;
    DRM_XMR_OUTPUT_CONFIGURATION_LIST  *pxmrCopyEnablerGUIDList = 0;

    DRM_BOOL                            fAESCrypto              = TRUE;
    DRM_DWORD                           dwTemp                  = 0;
    DRM_BOOL                            fUseRandomLID           = TRUE;
    DRM_AES_KEY                         oAesKey                 = {0};
    DRM_XMR_MOVE_ENABLER                xmrMoveEnabler          = {0};
    DRM_XMR_AUX_KEY                     xmrAuxKey               = {0};
    DRM_XMR_AUX_KEY_ENTRY              *pxmrAuxKeyEntries       = NULL;
    DRM_XMR_UPLINKX                     xmrUplinkX              = {0};
    DRM_DWORD                          *pdwLocations            = NULL;
    DRM_DWORD                           cLocations              = 0;
    DRM_DWORD                           cbKeyToRotate           = 0;
    DRM_BYTE                           *pbKeyToRotate           = 0; /* do not free this */
    DRM_CRYPTO_CONTEXT                  ctx                     = {0};
    DRM_DWORD                           cbCheckSumUplinkX       = DRM_AES_CK_CHECKSUM_LENGTH;
    DRM_BYTE                            bCheckSumUplinkX[DRM_AES_CK_CHECKSUM_LENGTH] = {0};
    DRM_BYTE                            rgbBaseKey[ DRM_AES_KEYSIZE_128 ]            = {0};
    DRM_BYTE                            rgbSecondKey[ DRM_AES_KEYSIZE_128 ]          = {0};
    DRM_BYTE                            rgbMixedKey[ DRM_AES_KEYSIZE_128 ]           = {0};
    DRM_BYTE                            rgbKeyPrime[ DRM_AES_KEYSIZE_128 ]           = {0};
    DRM_DWORD                           dwTestOptions                                = 0;

    ChkArg(pRights);
    ChkArg(ppList);

    ChkMem( pbPubKeyECC = (DRM_BYTE*)Oem_MemAlloc(ECC_P256_POINT_SIZE_IN_BYTES ));

    ZEROMEM( pbPubKeyECC, ECC_P256_POINT_SIZE_IN_BYTES );

    if( IsRightSet(pRights->TestOptions) )
    {
        ChkDR( DRMCRT_WtoDWORD( pRights->TestOptions.pwszString, pRights->TestOptions.cchString, 0, &dwTestOptions, NULL ) );

        // dwTestOptions should contain the bits in dwOptions and the TestOptions attribute
        dwTestOptions |= dwOptions;
    }

    if (IsRightSet(pRights->LicenseType))
    {
        if ( TST_UTL_wcsistr(pRights->LicenseType.pwszString, dwchRoot) )
        {
            eLicenseType = MAKE_ROOT_LICENSE;
        }
        else if ( TST_UTL_wcsistr(pRights->LicenseType.pwszString, dwchLeaf) )
        {
            eLicenseType = MAKE_LEAF_LICENSE;
        }
    }

    if (eLicenseType == MAKE_ROOT_LICENSE && pOptionalParams && pOptionalParams->pdstrRootKIDOverride)
    {
        if (pOptionalParams->pdstrRootKIDOverride->pwszString[0] == WCHAR_CAST('*') )
        {
            ChkDR(_MakeRandomKID(&dstrRandomKID));
            pdstrKID = &dstrRandomKID;
        }
        else
        {
            pdstrKID = pOptionalParams->pdstrRootKIDOverride;
        }
    }
    else if (eLicenseType == MAKE_ROOT_LICENSE && pRights->pLeaf && pOptionalParams && pOptionalParams->pdstrKIDOverride)
    {
        /* do nothing */
    }
    else if (pOptionalParams && pOptionalParams->pdstrKIDOverride)
    {
        if (pOptionalParams->pdstrKIDOverride->pwszString[0] == WCHAR_CAST('*') )
        {
            ChkDR(_MakeRandomKID(&dstrRandomKID));
            pdstrKID = &dstrRandomKID;
        }
        else
        {
            pdstrKID = pOptionalParams->pdstrKIDOverride;
        }
    }

    if (pdstrKID == NULL)
    {
        if (IsRightSet(pRights->KID))
        {
            if (pRights->KID.pwszString[0] == WCHAR_CAST('*'))
            {
                ChkDR(_MakeRandomKID(&dstrRandomKID));
                pdstrKID = &dstrRandomKID;
            }
            else
            {
                pdstrKID = &pRights->KID;
            }
        }
        else if (eLicenseType == MAKE_ROOT_LICENSE)
        {
            pdstrKID = &g_dstrDefaultUplinkKid;
        }
        else
        {
            pdstrKID = &g_dstrDefaultKid;
        }
    }

    if (pOptionalParams)
    {
        ChkDR(CopyDRMString(&pOptionalParams->dstrKIDUsed, pdstrKID));
    }

    if (pdstrKIDUsed)
    {
        ChkDR(CopyDRMString(pdstrKIDUsed, pdstrKID));
    }

    cb = SIZEOF(DRM_KID);
    ChkDR(DRM_B64_DecodeW(pdstrKID, &cb, (DRM_BYTE  *)&drmKID, 0));

    if (pdstrRootKID)
    {
        pdstrUplinkKID = pdstrRootKID;
    }
    else if (pOptionalParams && pOptionalParams->pdstrRootKIDOverride)
    {
        pdstrUplinkKID = pOptionalParams->pdstrRootKIDOverride;
    }
    else if (IsRightSet(pRights->UplinkKIDs))
    {
        if (pRights->UplinkKIDs.pwszString[0] == WCHAR_CAST('*'))
        {
            ChkDR(_MakeRandomKID(&dstrRandomRootKID));
            pdstrUplinkKID = &dstrRandomRootKID;
        }
        else
        {
            pdstrUplinkKID = &pRights->UplinkKIDs;
        }
    }

    if (pdstrUplinkKID)
    {
        if (pOptionalParams)
        {
            ChkDR(CopyDRMString(&pOptionalParams->dstrRootKIDUsed, pdstrUplinkKID));
        }
        cb = SIZEOF(DRM_KID);
        ChkDR(DRM_B64_DecodeW(pdstrUplinkKID, &cb, (DRM_BYTE  *)&drmUplinkKID, 0));
    }

    if ( IsRightSet(pRights->UseAESContentKey) && !IsRightTrue(pRights->UseAESContentKey) )
    {
        fAESCrypto = FALSE;
    }

    if ( IsRightSet(pRights->XMRVersion) )
    {
        if ( 0 == DRMCRT_wcsicmp( pRights->XMRVersion.pwszString, dwchXMRVersion1 ) )
        {
            dwXmrVersion = XMR_VERSION_1;
        }
        else if ( 0 == DRMCRT_wcsicmp( pRights->XMRVersion.pwszString, dwchXMRVersion2 ) )
        {
            dwXmrVersion = XMR_VERSION_2;
        }
        else if ( 0 == DRMCRT_wcsicmp( pRights->XMRVersion.pwszString, dwchXMRVersion3 ) )
        {
            dwXmrVersion = XMR_VERSION_3;
        }
    }
    else
    {
        dwXmrVersion = XMR_VERSION_3;
    }

    if (IsRightSet(pRights->ServerSeed))
    {
        pdstrSeed = &pRights->ServerSeed;
    }
    else
    {
        if( fAESCrypto )
        {
            pdstrSeed = &g_dstrDefaultAESKeySeed;
        }
        else
        {
            pdstrSeed = &g_dstrDefaultCocktailKeySeed;
        }
    }

    if (pOptionalParams && pOptionalParams->pdstrRIDOverride)
    {
        if ( pOptionalParams->pdstrRIDOverride->pwszString[ pOptionalParams->pdstrRIDOverride->cchString -1] == WCHAR_CAST('=')
            && pOptionalParams->pdstrRIDOverride->pwszString[ pOptionalParams->pdstrRIDOverride->cchString -2] == WCHAR_CAST('=') )
        {   /* B64 encoded string */
            DRM_DWORD   cbLID = pOptionalParams->pdstrRIDOverride->cchString * SIZEOF(DRM_WCHAR);
            ChkDR(DRM_B64_DecodeW(pOptionalParams->pdstrRIDOverride, &cbLID, drmLID.rgb, 0));
            fUseRandomLID = FALSE;
        }
        else if( pOptionalParams->pdstrRIDOverride->pwszString[0] == WCHAR_CAST('{') )
        {   /* GUID text string including '{' and '}' */
            CopyDRMString( &dstrLID, pOptionalParams->pdstrRIDOverride);
            ChkDR(DRM_UTL_StringToGuid( &dstrLID, (DRM_GUID *)&drmLID));
            fUseRandomLID = FALSE;
        }
    }
    else if( IsRightSet(pRights->LID) )
    {
        if ( pRights->LID.pwszString[ pRights->LID.cchString -1] == WCHAR_CAST('=') && pRights->LID.pwszString[ pRights->LID.cchString -2] == WCHAR_CAST('=') )
        {   /* B64 encoded string */
            DRM_DWORD   cbLID = pRights->LID.cchString * SIZEOF(DRM_WCHAR);
            ChkDR(DRM_B64_DecodeW(&pRights->LID, &cbLID, drmLID.rgb, 0));
            fUseRandomLID = FALSE;
        }
        else if( pRights->LID.pwszString[0] ==  WCHAR_CAST('{') )
        {   /* GUID text string including '{' and '}' */
            CopyDRMString( &dstrLID, &pRights->LID);
            ChkDR(DRM_UTL_StringToGuid( &dstrLID, (DRM_GUID *)&drmLID));
            fUseRandomLID = FALSE;
        }
    }

    if( fUseRandomLID )
    {    /* default to a random value ... */
        ChkDR( _MakeRandomKID( &dstrLID ) );
        cb = SIZEOF(DRM_KID);
        ChkDR(DRM_B64_DecodeW(&dstrLID, &cb, (DRM_BYTE  *)&drmLID, 0));
    }


    if (IsRightSet(pRights->ECCPublicKey))
    {   /* B64 encoded string */
        cbLen = ECC_P256_POINT_SIZE_IN_BYTES;
        ChkDR(DRM_B64_DecodeW(&pRights->ECCPublicKey, &cbLen, pbPubKeyECC, 0));
    }
    else
    {
        /* the ECC pubkey must have been set */
        ChkArg( g_cbPubKeyECC == ECC_P256_POINT_SIZE_IN_BYTES );
        memcpy( pbPubKeyECC, g_bPubKeyECC, ECC_P256_POINT_SIZE_IN_BYTES );
    }

    ChkDR(DRM_XMR_StartLicense(abBaseLicStack, STACK_SIZE, &drmLID, dwXmrVersion, &xmrBuilder));

    /* by initializing Content Key object, Key container will be created */
    ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_CONTENT_KEY_OBJECT, &xmrContentKey));

    xmrContentKey.pbguidKeyID  = (DRM_BYTE *) drmKID.rgb;
    xmrContentKey.iguidKeyID   = 0;

    if (pOptionalParams && pOptionalParams->pbContentKeyOverride)
    {
        ChkMem(pbClear = (DRM_BYTE*)Oem_MemAlloc(pOptionalParams->cbContentKeyOverride));
        memcpy(pbClear, pOptionalParams->pbContentKeyOverride, pOptionalParams->cbContentKeyOverride);
        cbClear = pOptionalParams->cbContentKeyOverride;
    }
    else if (IsRightSet(pRights->ContentKey))
    {   /* B64 encoded string */
        cbClear = 0;
        dr = DRM_B64_DecodeW(&pRights->ContentKey, &cbClear, NULL, 0);
        if (dr == DRM_E_BUFFERTOOSMALL)
        {
            ChkMem(pbClear = (DRM_BYTE*)Oem_MemAlloc(cbClear));
            ChkDR(DRM_B64_DecodeW(&pRights->ContentKey, &cbClear, pbClear, 0));
        }
        else
        {
            ChkDR(dr);
            ChkDR(DRM_E_XMLSIG_INVALID_KEY_FORMAT);  /* error out if DRM_SUCCESS */
        }
    }
    else
    {
        ChkDR( _GenerateContentKey(pdstrSeed, pdstrKID, &cbClear, &pbClear, 0, NULL, dwXmrVersion, fAESCrypto ) );
    }

    
    if ( pOptionalParams )
    {
        ChkMem( pOptionalParams->dstrLIDUsed.pwszString = (DRM_WCHAR*)Oem_MemAlloc( ( DRM_GUID_STRING_LEN + 1 ) * SIZEOF( DRM_WCHAR ) ) );
        ZEROMEM( (DRM_BYTE*)pOptionalParams->dstrLIDUsed.pwszString, ( DRM_GUID_STRING_LEN + 1 ) * SIZEOF( DRM_WCHAR ) );
        pOptionalParams->dstrLIDUsed.cchString = DRM_GUID_STRING_LEN;

        ChkDR( DRM_UTL_GuidToString( (DRM_GUID*)&drmLID, (DRM_WCHAR*)pOptionalParams->dstrLIDUsed.pwszString ) );
    }

    if (pOptionalParams && pOptionalParams->pbContentKeyUsed)
    {
        if(pOptionalParams->cbContentKeyUsed < DRM_AES_KEYSIZE_128)
        {
            ChkDR(DRM_E_BUFFERTOOSMALL);
        }
        memcpy(pOptionalParams->pbContentKeyUsed, pbClear, cbClear);
        pOptionalParams->cbContentKeyUsed = cbClear;
    }

    pbKeyToRotate = pbClear;
    cbKeyToRotate = cbClear;

    if (pOptionalParams && pOptionalParams->pbIntegrityKeyOverride)
    {
        memset(rgbKeyMaterial, 0, DRM_AES_KEYSIZE_128);
        memcpy(rgbKeyMaterial, pOptionalParams->pbIntegrityKeyOverride, min(pOptionalParams->cbIntegrityKeyOverride, DRM_AES_KEYSIZE_128));
    }
    else
    {
        ChkDR( _GetNonRandomBytes( rgbKeyMaterial, DRM_AES_KEYSIZE_128 ) );
    }


    if ( eLicenseType == MAKE_LEAF_LICENSE )
    {
        cbCheckSum = (IsRightTrue(pRights->Use16ByteCheckSum) ? 16 : DRM_AES_CK_CHECKSUM_LENGTH);

        xmrContentKey.wKeyEncryptionCipherType = XMR_ASYMMETRIC_ENCRYPTION_TYPE_CHAINED_LICENSE;

        if (IsRightSet(pRights->UplinkContentKey))
        {   /* B64 encoded string */
            cbClearRoot = 0;
            dr = DRM_B64_DecodeW(&pRights->UplinkContentKey, &cbClearRoot, NULL, 0);
            if (dr == DRM_E_BUFFERTOOSMALL)
            {
                ChkMem(pbClearRoot = (DRM_BYTE*)Oem_MemAlloc(cbClearRoot));
                ChkDR(DRM_B64_DecodeW(&pRights->UplinkContentKey, &cbClearRoot, pbClearRoot, 0));
                ChkDR(CalculateCheckSum(pbClearRoot, cbClearRoot, bCheckSum, cbCheckSum, &drmUplinkKID ));
            }
            else
            {
                ChkDR(dr);
                ChkDR(DRM_E_XMLSIG_INVALID_KEY_FORMAT);  /* error out if DRM_SUCCESS */
            }
        }
        else
        {
            /* here we are generating the key for the uplinked license, which
               should never be Cocktail (so we pass TRUE for the last argument) */
            ChkDR( _GenerateContentKey( pdstrSeed, pdstrUplinkKID, &cbClearRoot, &pbClearRoot, cbCheckSum, bCheckSum, dwXmrVersion, TRUE ) );
        }

        memcpy(rgbUplinkKey, pbClearRoot, cbClearRoot);

        pbKeyToRotate = pbClearRoot;
        cbKeyToRotate = cbClearRoot;
    }

    ChkDR( _tRotateContentKey( pbKeyToRotate,
                               cbKeyToRotate,
                               g_dwClearToBaseRotateCounter,
                               rgbBaseKey,
                               SIZEOF( rgbBaseKey ) ) );
    ChkDR( _tRotateContentKey( pbKeyToRotate,
                               cbKeyToRotate,
                               g_dwClearToSecondKeyRotateCounter,
                               rgbSecondKey,
                               SIZEOF( rgbSecondKey ) ) );

    if( IsRightSet( pRights->UplinkX ) )
    {
        DRM_DWORD dwLen    = 0;
        DRM_DWORD iEntry   = 0;
        DRM_DWORD iKeyByte = 0;

        ChkDR( CalculateCheckSum( pbClear, cbClear, bCheckSumUplinkX, cbCheckSumUplinkX, &drmKID ) );
        if ( dwTestOptions & OP_LICGEN_INVALID_UPLINKX_CHECKSUM )
        {
            bCheckSumUplinkX[0] = ~bCheckSumUplinkX[0];
        }

        ChkDR( _GetAuxKeyEntries( &pRights->UplinkX, SIZEOF(rgbBaseKey), rgbBaseKey, &cLocations, &pxmrAuxKeyEntries ) );

        if ( cLocations > 0 )
        {
            dwLen = cLocations * SIZEOF( DRM_DWORD );
            ChkMem( pdwLocations = ( DRM_DWORD * )Oem_MemAlloc( dwLen ) );
            ZEROMEM( pdwLocations, dwLen );
            for( iEntry = 0; iEntry < cLocations; iEntry++ )
            {
                pdwLocations[iEntry] = pxmrAuxKeyEntries[iEntry].dwLocation;
                for( iKeyByte = 0; iKeyByte < DRM_AES_KEYSIZE_128; iKeyByte++ )
                {
                    rgbMixedKey[ iKeyByte ] ^= pxmrAuxKeyEntries[ iEntry ].rgbKey[ iKeyByte ];
                }
            }
        }

        SAFE_OEM_FREE( pxmrAuxKeyEntries );

        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_UPLINKX_OBJECT, &xmrUplinkX) );

        xmrUplinkX.pbguidUplinkKID = drmUplinkKID.rgb;
        xmrUplinkX.iguidUplinkKID = 0;
        xmrUplinkX.pbCheckSumBuffer = bCheckSumUplinkX;
        xmrUplinkX.cbCheckSum = SIZEOF( bCheckSumUplinkX );
        xmrUplinkX.iCheckSum = 0;

        xmrUplinkX.cEntries = cLocations;
        xmrUplinkX.pdwLocation = pdwLocations;

        ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_UPLINKX_OBJECT, &xmrUplinkX ) );
    }

    if (eLicenseType == MAKE_LEAF_LICENSE)
    {
        if ( fAESCrypto )
        {
            /* XMR V3 leaf licenses MIGHT use AES CTR to encrypt the content
              XMR V1 leaf licenses MIGHT use AES CTR to encrypt the content */
            xmrContentKey.wSymmetricCipherType = XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_CTR;
            memcpy(rgbKeyMaterial + DRM_AES_KEYSIZE_128, pbClear, cbClear);
        }
        else if ( dwXmrVersion == XMR_VERSION_1 )
        {
            /* XMR V1 leaf licenses MIGHT use RC4 (cocktail) to encrypt the content */
            xmrContentKey.wSymmetricCipherType = XMR_SYMMETRIC_ENCRYPTION_TYPE_RC4_CIPHER;
            rgbKeyMaterial[DRM_AES_KEYSIZE_128] = (DRM_BYTE)cbClear;
            memcpy(&rgbKeyMaterial[DRM_AES_KEYSIZE_128+1], pbClear, cbClear);
        }
        else if ( dwXmrVersion == XMR_VERSION_3 )
        {
            /* XMR V3 leaf licenses MIGHT use COCKTAIL to encrypt the content */
            xmrContentKey.wSymmetricCipherType = XMR_SYMMETRIC_ENCRYPTION_TYPE_COCKTAIL;
            memcpy( rgbKeyMaterial + DRM_AES_KEYSIZE_128, pbClear, cbClear );
        }
        else
        {
            ChkDR( DRM_E_NOTIMPL );
        }

        if( IsRightSet( pRights->UplinkX ) )
        {
            memcpy( rgbKeyMaterial + DRM_AES_KEYSIZE_128, pbClear, cbClear );
            memcpy( rgbDestKey, rgbKeyMaterial, SIZEOF( rgbDestKey ) );

            ChkDR( Oem_Aes_SetKey( rgbSecondKey, &oAesKey ) );
            ChkDR( Oem_Aes_EcbDecryptData( &oAesKey, rgbDestKey, SIZEOF( rgbDestKey ) ) );

            ChkDR( Oem_Aes_SetKey( rgbMixedKey, &oAesKey ) );
            ChkDR( Oem_Aes_EcbDecryptData( &oAesKey, rgbDestKey, SIZEOF( rgbDestKey ) ) );
        }
        else
        {
            /* Grab the AES key from the uplink license. */
            ChkDR(Oem_Aes_SetKey(rgbUplinkKey, &AesUplinkKey));

            /* ECB encryption is in place; make a copy of the keys */
            MEMCPY(rgbDestKey, rgbKeyMaterial, SIZEOF(rgbDestKey));

            /* Encrypt the content/integrity key pairs */
            ChkDR(Oem_Aes_EcbEncryptData(&AesUplinkKey, rgbDestKey, SIZEOF(rgbDestKey)));
        }

        cbEncryptedKeyBlob = SIZEOF(rgbDestKey);
        memcpy(abEncryptedKeyBlob, rgbDestKey, cbEncryptedKeyBlob);
        memcpy(rgbIntegrityKey, rgbKeyMaterial, SIZEOF(rgbKeyMaterial));
    }
    else /* Simple or Root */
    {
        xmrContentKey.wKeyEncryptionCipherType = XMR_ASYMMETRIC_ENCRYPTION_TYPE_ECC_256;

        if ( fAESCrypto )
        {
            /* XMR V2 simple licenses ALWAYS use AES CTR to encrypt the content
               XMR V1 simple licenses MIGHT use AES CTR to encrypt the content */
            if ( eLicenseType == MAKE_ROOT_LICENSE )
            {
                xmrContentKey.wSymmetricCipherType = XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_ECB;
            }
            else
            {
                xmrContentKey.wSymmetricCipherType = XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_CTR;
            }
            ChkBOOL( cbClear <= DRM_AES_KEYSIZE_128, DRM_E_TEST_UNEXPECTED_OUTPUT );
            memcpy(rgbKeyMaterial + DRM_AES_KEYSIZE_128, pbClear, cbClear);
        }
        else if ( dwXmrVersion == XMR_VERSION_1 )
        {
            /* XMR V1 simple licenses MIGHT use RC4 (cocktail) to encrypt the content */
            xmrContentKey.wSymmetricCipherType = XMR_SYMMETRIC_ENCRYPTION_TYPE_RC4_CIPHER;
            ChkBOOL( cbClear < DRM_AES_KEYSIZE_128, DRM_E_TEST_UNEXPECTED_OUTPUT );
            rgbKeyMaterial[DRM_AES_KEYSIZE_128] = (DRM_BYTE)cbClear;
            memcpy(rgbKeyMaterial + DRM_AES_KEYSIZE_128 + 1, pbClear, cbClear);
        }
        else if ( dwXmrVersion == XMR_VERSION_3 )
        {
            /* XMR V3 simple licenses MIGHT use COCKTAIL to encrypt the content */
            xmrContentKey.wSymmetricCipherType = XMR_SYMMETRIC_ENCRYPTION_TYPE_COCKTAIL;
            ChkBOOL( cbClear < DRM_AES_KEYSIZE_128, DRM_E_TEST_UNEXPECTED_OUTPUT );
            memcpy( rgbKeyMaterial + DRM_AES_KEYSIZE_128, pbClear, cbClear );
        }
        else
        {
            ChkDR( DRM_E_NOTIMPL );
        }

        memcpy(rgbIntegrityKey, rgbKeyMaterial, SIZEOF(rgbKeyMaterial));
        ChkDR( OEM_ECC_GenerateHMACKey_P256( (PLAINTEXT_P256 *)rgbIntegrityKey, (struct bigctx_t *)&ctx ) );

        cbEncryptedKeyBlob = ECC_P256_CIPHERTEXT_SIZE_IN_BYTES;

        ChkDR( DRM_PK_Encrypt( NULL, &ctx, eDRM_ECC_P256,
                                ECC_P256_POINT_SIZE_IN_BYTES, pbPubKeyECC,
                                ECC_P256_PLAINTEXT_SIZE_IN_BYTES, rgbIntegrityKey,
                                &cbEncryptedKeyBlob, abEncryptedKeyBlob) ); /* Both DRM_RSA_CB_CIPHERTEXT and ECC_P256_CIPHERTEXT_SIZE_IN_BYTES are all 128 */
    }

    if( IsRightSet( pRights->AuxKey ) )
    {
        DRM_DWORD iEntry = 0;

        memcpy(rgbIntegrityKey, rgbKeyMaterial, SIZEOF(rgbKeyMaterial));
        ChkDR( OEM_ECC_GenerateHMACKey_P256( (PLAINTEXT_P256 *)rgbKeyMaterial, (struct bigctx_t *)&ctx ) );

        cbEncryptedKeyBlob = ECC_P256_CIPHERTEXT_SIZE_IN_BYTES;

        ChkDR( DRM_PK_Encrypt( NULL, &ctx, eDRM_ECC_P256,
                                ECC_P256_POINT_SIZE_IN_BYTES, pbPubKeyECC,
                                ECC_P256_PLAINTEXT_SIZE_IN_BYTES, rgbKeyMaterial,
                                &cbEncryptedKeyBlob, abEncryptedKeyBlob) ); /* Both DRM_RSA_CB_CIPHERTEXT and ECC_P256_CIPHERTEXT_SIZE_IN_BYTES are all 128 */

        /* The clear CI|CK in rgbKey is shuffled, need to get the unshuffled CI|CK. */
        ChkDR( TSTUTIL_UnshuffleContentKey( rgbKeyMaterial ) );
        memcpy( rgbIntegrityKey, rgbKeyMaterial, SIZEOF( rgbKeyMaterial ) );
        memcpy( rgbKeyPrime, rgbKeyMaterial + DRM_AES_KEYSIZE_128, DRM_AES_KEYSIZE_128 );
        ChkDR( Oem_Aes_SetKey( rgbKeyMaterial + DRM_AES_KEYSIZE_128, &oAesKey ) );
        ChkDR( Oem_Aes_EcbDecryptData( &oAesKey, rgbKeyPrime, DRM_AES_KEYSIZE_128 ) );

        ChkDR( TSTUTIL_CalcContentKeyPrime( rgbKeyMaterial + DRM_AES_KEYSIZE_128, rgbKeyPrime ) );

        ChkDR( _GetAuxKeyEntries( &pRights->AuxKey, SIZEOF(rgbBaseKey), rgbBaseKey, &cLocations, &pxmrAuxKeyEntries ) );

        for ( iEntry = 0; iEntry < cLocations; iEntry++ )
        {
            ChkDR( Oem_Aes_SetKey( rgbKeyPrime, &oAesKey ) );
            ChkDR( Oem_Aes_EcbDecryptData( &oAesKey, pxmrAuxKeyEntries[ iEntry ].rgbKey, DRM_AES_KEYSIZE_128 ) );
        }

        memcpy( rgbKeyPrime, rgbSecondKey, DRM_AES_KEYSIZE_128 );

        ChkDR( Oem_Aes_SetKey( rgbKeyMaterial + DRM_AES_KEYSIZE_128, &oAesKey ) );
        ChkDR( Oem_Aes_EcbDecryptData( &oAesKey, rgbKeyPrime, DRM_AES_KEYSIZE_128 ) );

        MEMCPY( abEncryptedKeyBlob + ECC_P256_CIPHERTEXT_SIZE_IN_BYTES, rgbKeyPrime, DRM_AES_KEYSIZE_128 );
        cbEncryptedKeyBlob = cbEncryptedKeyBlob + DRM_AES_KEYSIZE_128;

        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_AUX_KEY_OBJECT, &xmrAuxKey) );
        xmrAuxKey.cEntries = cLocations;
        xmrAuxKey.pEntries = pxmrAuxKeyEntries;
        ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_AUX_KEY_OBJECT, (DRM_VOID*) &xmrAuxKey ) );

        xmrContentKey.wKeyEncryptionCipherType = XMR_ASYMMETRIC_ENCRYPTION_TYPE_ECC_256_WITH_KZ;
    }


    if (pOptionalParams && pOptionalParams->pbIntegrityKeyUsed)
    {
        if(pOptionalParams->cbIntegrityKeyUsed < DRM_AES_KEYSIZE_128)
        {
            ChkDR(DRM_E_BUFFERTOOSMALL);
        }
        memcpy(pOptionalParams->pbIntegrityKeyUsed, rgbIntegrityKey, DRM_AES_KEYSIZE_128);
        pOptionalParams->cbIntegrityKeyUsed = DRM_AES_KEYSIZE_128;
    }

    xmrContentKey.cbEncryptedKey = (DRM_WORD)cbEncryptedKeyBlob;
    xmrContentKey.pbEncryptedKeyBuffer = abEncryptedKeyBlob;
    xmrContentKey.iEncryptedKey        = 0;
    ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_CONTENT_KEY_OBJECT, (DRM_VOID*) &xmrContentKey));


    /* by initalizing MinumumEnvironment object, Global Policy container and Outer container will be created */
    ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_SECURITY_LEVEL, &xmrMinimumEnvironment));

    if (IsRightSet(pRights->MinimumSecurityLevel) && (CompareDSTRToWCHAR(&pRights->MinimumSecurityLevel, dwchNoMinimumSecurityLevel) == DRM_SUCCESS))
    {
       /* do not add a MinimumSecurityLevel for XMR_OBJECT_TYPE_SECURITY_LEVEL object */
    }
    else
    {
        /* wMinimumSecurityLevel is set to 150 by default if not specified in the xml input file */
        xmrMinimumEnvironment.wMinimumSecurityLevel = 150;
        if (IsRightSet(pRights->MinimumSecurityLevel))
        {
            ChkDR( DRMCRT_WtoDWORD( pRights->MinimumSecurityLevel.pwszString, pRights->MinimumSecurityLevel.cchString, 10, &dwTemp, NULL) );
            xmrMinimumEnvironment.wMinimumSecurityLevel = (DRM_WORD)dwTemp;
        }
    }

    if (IsRightSet(pRights->MinimumAppCrlVersion))
    {
        ChkDR( DRMCRT_WtoDWORD( pRights->MinimumAppCrlVersion.pwszString, pRights->MinimumAppCrlVersion.cchString, 10, &dwTemp, NULL) );
        xmrMinimumEnvironment.dwMinimumAppRevocationListVersion = dwTemp;
    }

    if (IsRightSet(pRights->MinimumDeviceCrlVersion))
    {
        ChkDR( DRMCRT_WtoDWORD( pRights->MinimumDeviceCrlVersion.pwszString, pRights->MinimumDeviceCrlVersion.cchString, 10, &dwTemp, NULL) );
        xmrMinimumEnvironment.dwMinimumDeviceRevocationListVersion = dwTemp;
    }

    ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_SECURITY_LEVEL, (DRM_VOID*) &xmrMinimumEnvironment));

    /* Add Rights object */
    ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_SETTINGS_OBJECT, &xmrRights));

    if(IsRightTrue(pRights->CannotPersist))
    {
        xmrRights.wValue |= XMR_RIGHTS_CANNOT_PERSIST;
    }
    if(IsRightTrue(pRights->TempStoreOnly))
    {
        xmrRights.wValue |= XMR_RIGHTS_TEMP_STORE_ONLY;
    }
    if(IsRightTrue(pRights->AllowBackUpRestore))
    {
        xmrRights.wValue |= XMR_RIGHTS_ALLOW_BACKUP_RESTORE;
    }
    if (IsRightTrue(pRights->AllowCollaborativePlay))
    {
        xmrRights.wValue |= XMR_RIGHTS_COLLABORATIVE_PLAY;
    }
    if (IsRightTrue(pRights->CanNotBind))
    {
        xmrRights.wValue |= XMR_RIGHTS_CANNOT_BIND_LICENSE;
    }

    if ( xmrRights.wValue != 0)
        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_SETTINGS_OBJECT, (DRM_VOID*) &xmrRights));

    /* Add restricted source ID object */
    if(IsRightTrue(pRights->RestrictedSourceID))
    {
        ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_RESTRICTED_SOURCEID_OBJECT, &xmrRestrictedSourceID));
        xmrRestrictedSourceID.fValid = TRUE;
        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_RESTRICTED_SOURCEID_OBJECT, (DRM_VOID*) &xmrRestrictedSourceID));
    }

    if (IsRightSet(pRights->Priority))
    {
        /* initalizing Priority object */
        ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_PRIORITY_OBJECT, &xmrPriority));
        ChkDR( DRMCRT_WtoDWORD( pRights->Priority.pwszString, pRights->Priority.cchString, 10, &xmrPriority.dwValue, NULL ) );
        xmrPriority.fValid = TRUE;
        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_PRIORITY_OBJECT, (DRM_VOID*) &xmrPriority));
    }

    if (IsRightSet(pRights->ExpirationDate) || IsRightSet(pRights->BeginDate))
    {
        ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_EXPIRATION_OBJECT,&xmrExpiration));

        if (IsRightSet(pRights->ExpirationDate))
        {
            if (pRights->ExpirationDate.pwszString[0] == g_wchPound)
            {
                TST_UTL_wcsncpy(wszExpirationDate, pRights->ExpirationDate.pwszString, 64);
            }
            else
            {
                ChkDR(_CalcRelativeDate(wszExpirationDate, pRights->ExpirationDate.pwszString, 64, NULL ));
            }
            _TextTimeToFileTime(wszExpirationDate, &ft);
            tFileTimeToDWORD((DRMFILETIME*)&ft, &xmrExpiration.dwEndDate);
        }

        if (IsRightSet(pRights->BeginDate))
        {
            if (pRights->BeginDate.pwszString[0] == g_wchPound)
            {
                TST_UTL_wcsncpy(wszBeginDate, pRights->BeginDate.pwszString, 64);
            }
            else
            {
                ChkDR(_CalcRelativeDate(wszBeginDate, pRights->BeginDate.pwszString, 64, NULL ));
            }
            _TextTimeToFileTime(wszBeginDate, &ft);
            tFileTimeToDWORD((DRMFILETIME*)&ft, &xmrExpiration.dwBeginDate);
        }

        xmrExpiration.fValid = TRUE;
        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_EXPIRATION_OBJECT,&xmrExpiration));
    }

    if (IsRightSet(pRights->RemovalDate))
    {
        ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_REMOVAL_DATE_OBJECT,&xmrRemovalDate));
        if (pRights->RemovalDate.pwszString[0] == g_wchPound)
        {
            TST_UTL_wcsncpy(wszRemovalDate, pRights->RemovalDate.pwszString, 64);
        }
        else
        {
            ChkDR(_CalcRelativeDate( wszRemovalDate,
                                     pRights->RemovalDate.pwszString,
                                     64,
                                     pOptionalParams != NULL ? pOptionalParams->pTestStartTime : NULL ));
        }
        _TextTimeToFileTime(wszRemovalDate, &ft);
        tFileTimeToDWORD((DRMFILETIME*)&ft, &xmrRemovalDate.dwRemovalDate);
        xmrRemovalDate.fValid = TRUE;
        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_REMOVAL_DATE_OBJECT,&xmrRemovalDate));
    }

    if (IsRightSet(pRights->IssueDate))
    {
        ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_ISSUEDATE_OBJECT,&xmrIssueDate));
        if (pRights->IssueDate.pwszString[0] == g_wchPound)
        {
            TST_UTL_wcsncpy(wszIssueDate, pRights->IssueDate.pwszString, 64);
        }
        else
        {
            ChkDR(_CalcRelativeDate(wszIssueDate, pRights->IssueDate.pwszString, 64, NULL ));
        }
        _TextTimeToFileTime(wszIssueDate, &ft);
        tFileTimeToDWORD((DRMFILETIME*)&ft, &xmrIssueDate.dwValue);
        xmrIssueDate.fValid = TRUE;
        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_ISSUEDATE_OBJECT,&xmrIssueDate));
    }

    if( IsRightSet( pRights->MeteringID ) )
    {
        DRM_DWORD cbMID = SIZEOF(DRM_MID);
        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_METERING_OBJECT, &xmrMetering ) );
        ChkDR( DRM_B64_DecodeW( &pRights->MeteringID, &cbMID, (DRM_BYTE *)&drmMID, 0) );

        xmrMetering.iGuid = 0;
        xmrMetering.pguidBuffer = (DRM_BYTE*)drmMID.rgb;
        xmrMetering.fValid = TRUE;
        ChkDR(DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_METERING_OBJECT, &xmrMetering ) );
    }
    else if( IsRightSet( pRights->MeteringCert ) )
    {
        DRM_CONST_STRING dstrMID = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrListTop = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrListData = EMPTY_DRM_STRING;
        DRM_DWORD cbMID = SIZEOF(DRM_MID);
        ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_METERING_OBJECT,&xmrMetering));
        ChkDR(DRM_XML_GetNode(&pRights->MeteringCert, &g_cszMETERCERT, NULL, NULL, 0, NULL, &dstrListTop));
        ChkDR(DRM_XML_GetNode(&dstrListTop, &g_cszData, NULL, NULL, 0, NULL, &dstrListData));
        ChkDR(DRM_XML_GetNode(&dstrListData, &g_cszMID, NULL, NULL, 0, NULL, &dstrMID));
        ChkDR(DRM_B64_DecodeW(&dstrMID, &cbMID, (DRM_BYTE *)&drmMID, 0));

        xmrMetering.iGuid = 0;
        xmrMetering.pguidBuffer = (DRM_BYTE*)drmMID.rgb;
        xmrMetering.fValid = TRUE;
        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_METERING_OBJECT,&xmrMetering));
    }

    if (IsRightSet(pRights->ExpirationAfterFirstUse))
    {
        /* Add Expire After First Store object */
        ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTUSE_OBJECT, &xmrExpirationAfterFirstUse));
        ChkDR( DRMCRT_WtoDWORD( pRights->ExpirationAfterFirstUse.pwszString, pRights->ExpirationAfterFirstUse.cchString, 10, &xmrExpirationAfterFirstUse.dwValue, NULL ) );

        ChkDR(_GetNumberOfSecondsInTimeString(pRights->ExpirationAfterFirstUse.pwszString, &xmrExpirationAfterFirstUse.dwValue));

        xmrExpirationAfterFirstUse.fValid = TRUE;
        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTUSE_OBJECT, (DRM_VOID*) &xmrExpirationAfterFirstUse));
    }

    if (IsRightSet(pRights->ExpirationAfterFirstPlay))
    {
        /* Add Expire After First Play object */
        ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTPLAY_OBJECT, &xmrExpirationAfterFirstPlay));
        ChkDR( DRMCRT_WtoDWORD( pRights->ExpirationAfterFirstPlay.pwszString, pRights->ExpirationAfterFirstPlay.cchString, 10, &xmrExpirationAfterFirstPlay.dwValue, NULL ) );

        ChkDR(_GetNumberOfSecondsInTimeString(pRights->ExpirationAfterFirstPlay.pwszString, &xmrExpirationAfterFirstPlay.dwValue));

        xmrExpirationAfterFirstPlay.fValid = TRUE;
        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTPLAY_OBJECT, (DRM_VOID*) &xmrExpirationAfterFirstPlay));
    }

    if (IsRightSet(pRights->ExpirationOnStore))
    {
        /* Add Expire After First Store object */
        ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTSTORE_OBJECT, &xmrExpirationAfterFirstStore));

        ChkDR(_GetNumberOfSecondsInTimeString(pRights->ExpirationOnStore.pwszString, &xmrExpirationAfterFirstStore.dwValue));

        xmrExpirationAfterFirstStore.fValid = TRUE;
        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTSTORE_OBJECT, (DRM_VOID*) &xmrExpirationAfterFirstStore));
    }

    ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_EMBEDDING_BEHAVIOR_OBJECT, &xmrEmbeddingBehavior));
    if (IsRightSet(pRights->EmbedLicense))
    {
        if (CompareDSTRToWCHAR(&pRights->EmbedLicense, g_pwszIgnore) == DRM_SUCCESS)
        {
            xmrEmbeddingBehavior.wValue = XMR_EMBEDDING_BEHAVIOR_IGNORE;
        }
        else if (CompareDSTRToWCHAR(&pRights->EmbedLicense, g_pwszCopy) == DRM_SUCCESS)
        {
            xmrEmbeddingBehavior.wValue = XMR_EMBEDDING_BEHAVIOR_COPY;
        }
        else if (CompareDSTRToWCHAR(&pRights->EmbedLicense, g_pwszNoEmbeddingObject) == DRM_SUCCESS)
        {
            fNoEmbeddingObject = TRUE;
        }
        else if (DRMCRT_iswdigit(pRights->EmbedLicense.pwszString[0]))
        {
            ChkDR( DRMCRT_WtoWORD( pRights->EmbedLicense.pwszString, pRights->EmbedLicense.cchString, 10, &xmrEmbeddingBehavior.wValue, NULL ) );
        }
        else
        {
            ChkDR( DRM_E_INVALIDARG );
        }
    }
    else
    {   /* default is to ignore */
        xmrEmbeddingBehavior.wValue = XMR_EMBEDDING_BEHAVIOR_IGNORE;
    }

    if (!fNoEmbeddingObject)
    {
        xmrEmbeddingBehavior.fValid = TRUE;
        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_EMBEDDING_BEHAVIOR_OBJECT, (DRM_VOID*) &xmrEmbeddingBehavior));
    }

    if ( IsRightSet( pRights->DomainID ) || ( pOptionalParams && pOptionalParams->pguidDomainAccountID != NULL ) )
    {
        /* set up the domain id object */
        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_DOMAIN_ID_OBJECT, &xmrDomainID ) );

        if ( pOptionalParams && pOptionalParams->pguidDomainAccountID )
        {
            xmrDomainID.pbAccountID = (DRM_BYTE*) pOptionalParams->pguidDomainAccountID;
        }
        else if ( IsRightSet( pRights->DomainAccountID ) )
        {
            ChkDR( DRM_UTL_StringToGuid( &pRights->DomainAccountID, &guidAccountID ) );
            xmrDomainID.pbAccountID = (DRM_BYTE*) &guidAccountID;
        }

        if ( pOptionalParams && pOptionalParams->dwDomainRevision )
        {
            xmrDomainID.dwRevision = pOptionalParams->dwDomainRevision;
        }
        else if ( IsRightSet( pRights->DomainRevision ) )
        {
            ChkDR( DRMCRT_WtoDWORD( pRights->DomainRevision.pwszString, pRights->DomainRevision.cchString, 10, &xmrDomainID.dwRevision, NULL ) );
        }

        xmrDomainID.cbAccountID = SIZEOF( DRM_GUID );
        xmrDomainID.fValid = TRUE;

        ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_DOMAIN_ID_OBJECT, (DRM_VOID*) &xmrDomainID ) );
    }

    /* setup device key */
    if ( eLicenseType != MAKE_LEAF_LICENSE)
    {
        /* The only time we don't need a device key object is for a leaf license, which is bound to the root license */
        ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_ECC_DEVICE_KEY_OBJECT, &xmrDeviceKey));

        xmrDeviceKey.fValid = TRUE;
        xmrDeviceKey.wEccCurveType = XMR_ECC_CURVE_TYPE_P256;
        xmrDeviceKey.iKeyData = 0;
        xmrDeviceKey.cbKeyData = ECC_P256_POINT_SIZE_IN_BYTES;
        xmrDeviceKey.pbKeyData = pbPubKeyECC;

        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_ECC_DEVICE_KEY_OBJECT, (DRM_VOID*) &xmrDeviceKey));
    }

    /* setup Playback_policy_container */
    if (IsRightTrue(pRights->AllowPlay))
    {
        ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER, &xmrPlayRights));
        xmrPlayRights.fValid = TRUE;
        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER, (DRM_VOID*) &xmrPlayRights));
    }

    /* Add PlayEnablerGUIDList */
    if (IsRightSet(pRights->PlayEnablerGUIDList))
    {
        DRM_DWORD index = 0;
        DRM_CONST_STRING dstrPlayEnablerGUID = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrGUID      = EMPTY_DRM_STRING;

        if (!xmrPlayRights.fValid)
        {
            ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER, &xmrPlayRights));
            xmrPlayRights.fValid = TRUE;
            ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER, (DRM_VOID*) &xmrPlayRights));
        }

        while ( DRM_SUCCEEDED( dr = DRM_XML_GetSubNode( &pRights->PlayEnablerGUIDList,
                                                 &g_dstrTagPlayEnablerGUID,
                                                 NULL,
                                                 NULL,
                                                 index++,
                                                 &dstrPlayEnablerGUID,
                                                 NULL,
                                                 0) ) )  /* is the first layer */
        {
            cbLen = SIZEOF(DRM_XMR_OUTPUT_CONFIGURATION_LIST );
            ChkMem(pxmrPlayEnablerGUIDTemp = (DRM_XMR_OUTPUT_CONFIGURATION_LIST *)Oem_MemAlloc(cbLen));
            memset(pxmrPlayEnablerGUIDTemp, 0, cbLen);

            ChkDR( DRM_XML_GetNodeAttribute( &dstrPlayEnablerGUID,
                                             &g_dstrAttribGuid,
                                             &dstrGUID ) );

            cbLen = SIZEOF( DRM_GUID );
            ChkMem(pxmrPlayEnablerGUIDTemp->Config.pguidBuffer = (DRM_BYTE*)Oem_MemAlloc(cbLen));
            memset(pxmrPlayEnablerGUIDTemp->Config.pguidBuffer, 0, cbLen);
            ChkDR( DRM_UTL_StringToGuid( &dstrGUID,
                                         (DRM_GUID*)(pxmrPlayEnablerGUIDTemp->Config.pguidBuffer) ) );

            pxmrPlayEnablerGUIDTemp->pNext = pxmrPlayEnablerGUIDList;
            pxmrPlayEnablerGUIDList = pxmrPlayEnablerGUIDTemp;

            ChkDR( DRM_XMR_AddUnknownObject(
                    &xmrBuilder, XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER, TRUE, XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER, XMR_FLAGS_CONTAINER, NULL, 0));

            ChkDR( DRM_XMR_AddUnknownObject(
                    &xmrBuilder, XMR_OBJECT_TYPE_PLAY_ENABLER_OBJECT, TRUE, XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER, 0,  (DRM_BYTE*)pxmrPlayEnablerGUIDTemp->Config.pguidBuffer, cbLen));

            pxmrPlayEnablerGUIDTemp = NULL;
        }

        if ( DRM_E_XMLNOTFOUND == dr ) /* no more EXTENSION nodes */
        {
            dr = DRM_SUCCESS;
        }
        ChkDR( dr );
    }


    if (IsRightSet(pRights->PlayCount))
    {
        ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_PLAYCOUNT_OBJECT, &xmrPlayCount));
        ChkDR( DRMCRT_WtoDWORD( pRights->PlayCount.pwszString, pRights->PlayCount.cchString, 10, &xmrPlayCount.dwValue, NULL ) );
        xmrPlayCount.fValid = TRUE;
        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_PLAYCOUNT_OBJECT, (DRM_VOID*) &xmrPlayCount));
    }

    if (IsRightSet(pRights->AnalogVideo) || IsRightSet(pRights->UnCompressedDigitalVideo)
        || IsRightSet(pRights->UnCompressedDigitalAudio) || IsRightSet(pRights->CompressedDigitalVideo)
        || IsRightSet(pRights->CompressedDigitalAudio))
    {
        DRM_DWORD dwTemp = 0;

        ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_OUTPUT_PROTECTION_OBJECT, &opl));
        if (IsRightSet(pRights->AnalogVideo))
        {
            ChkDR( DRMCRT_WtoDWORD( pRights->AnalogVideo.pwszString, pRights->AnalogVideo.cchString, 10, &dwTemp, NULL ) );
            opl.wAnalogVideo = (DRM_WORD)dwTemp;
        }
        if (IsRightSet(pRights->UnCompressedDigitalVideo))
        {
            ChkDR( DRMCRT_WtoDWORD( pRights->UnCompressedDigitalVideo.pwszString, pRights->UnCompressedDigitalVideo.cchString, 10, &dwTemp, NULL ) );
            opl.wUncompressedDigitalVideo = (DRM_WORD)dwTemp;
        }
        if (IsRightSet(pRights->UnCompressedDigitalAudio))
        {
            ChkDR( DRMCRT_WtoDWORD( pRights->UnCompressedDigitalAudio.pwszString, pRights->UnCompressedDigitalAudio.cchString, 10, &dwTemp, NULL ) );
            opl.wUncompressedDigitalAudio = (DRM_WORD)dwTemp;
        }
        if (IsRightSet(pRights->CompressedDigitalVideo))
        {
            ChkDR( DRMCRT_WtoDWORD( pRights->CompressedDigitalVideo.pwszString, pRights->CompressedDigitalVideo.cchString, 10, &dwTemp, NULL ) );
            opl.wCompressedDigitalVideo = (DRM_WORD)dwTemp;
        }
        if (IsRightSet(pRights->CompressedDigitalAudio))
        {
            ChkDR( DRMCRT_WtoDWORD( pRights->CompressedDigitalAudio.pwszString, pRights->CompressedDigitalAudio.cchString, 10, &dwTemp, NULL ) );
            opl.wCompressedDigitalAudio = (DRM_WORD)dwTemp;
        }
        opl.fValid = TRUE;
        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_OUTPUT_PROTECTION_OBJECT, (DRM_VOID*) &opl));
    }
    if (IsRightSet(pRights->AnalogVideoExtensions))
    {
        DRM_DWORD dwData = 0;
        DRM_DWORD index = 0;
        DRM_CONST_STRING dstrExtension = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrGUID      = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrConfigData = EMPTY_DRM_STRING;

        ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_EXPLICIT_ANALOG_VIDEO_OUTPUT_PROTECTION_CONTAINER, &xmrExplicitAnalogVideoContainer));

        while ( DRM_SUCCEEDED( dr = DRM_XML_GetSubNode( &pRights->AnalogVideoExtensions,
                                                 &g_dstrTagOPLExtension,
                                                 NULL,
                                                 NULL,
                                                 index++,
                                                 &dstrExtension,
                                                 NULL,
                                                 1) ) )  /* EXTENSION is the second layer */
        {
            cbLen = SIZEOF(DRM_XMR_VIDEO_OUTPUT_CONFIGURATION_LIST);
            ChkMem(pxmrExplicitAnalogVideoTemp = (DRM_XMR_VIDEO_OUTPUT_CONFIGURATION_LIST*)Oem_MemAlloc(cbLen));
            memset(pxmrExplicitAnalogVideoTemp, 0, cbLen);

            pxmrExplicitAnalogVideoTemp->Config.fValid       = TRUE;
            pxmrExplicitAnalogVideoTemp->Config.iGuid        = 0;
            pxmrExplicitAnalogVideoTemp->Config.iConfigData  = 0;

            ChkDR( DRM_XML_GetNodeAttribute( &dstrExtension,
                                             &g_dstrAttribGuid,
                                             &dstrGUID ) );

            cbLen = SIZEOF( DRM_GUID );
            ChkMem(pxmrExplicitAnalogVideoTemp->Config.pguidBuffer = (DRM_BYTE*)Oem_MemAlloc(cbLen));
            memset(pxmrExplicitAnalogVideoTemp->Config.pguidBuffer, 0, cbLen);
            ChkDR( DRM_UTL_StringToGuid( &dstrGUID,
                                         (DRM_GUID*)(pxmrExplicitAnalogVideoTemp->Config.pguidBuffer) ) );

            ChkDR( DRM_XML_GetNodeAttribute( &dstrExtension,
                                             &g_dstrAttribValue,
                                             &dstrConfigData ) );

            ChkDR( DRMCRT_WtoDWORD( dstrConfigData.pwszString, dstrConfigData.cchString, 10, &dwData, NULL ) );

            dr = DRM_XML_GetNodeAttribute( &dstrExtension,
                                             &g_dstrAttribLength,
                                             &dstrConfigData );
            if (dr == DRM_SUCCESS)
            {
                ChkDR( DRMCRT_WtoDWORD( dstrConfigData.pwszString, dstrConfigData.cchString, 10, &cbLen, NULL ) );
            }
            else
            {
                cbLen = SIZEOF( DRM_DWORD );
            }

            pxmrExplicitAnalogVideoTemp->Config.cbConfigData = cbLen;

            ChkMem( pxmrExplicitAnalogVideoTemp->Config.pbConfigDataBuffer = (DRM_BYTE*)Oem_MemAlloc( cbLen ) );
            memcpy( pxmrExplicitAnalogVideoTemp->Config.pbConfigDataBuffer, &dwData, min( cbLen, SIZEOF( DRM_DWORD ) ) );
            _ReverseBytes(pxmrExplicitAnalogVideoTemp->Config.pbConfigDataBuffer, min( cbLen, SIZEOF( DRM_DWORD ) ) );
            pxmrExplicitAnalogVideoTemp->pNext = xmrExplicitAnalogVideoContainer.plistOutputConfiguration;
            xmrExplicitAnalogVideoContainer.plistOutputConfiguration = pxmrExplicitAnalogVideoTemp;
            xmrExplicitAnalogVideoContainer.cOutputProtectionIDs++;

            pxmrExplicitAnalogVideoTemp = NULL;

        }

        if ( DRM_E_XMLNOTFOUND == dr ) /* no more EXTENSION nodes */
        {
            dr = DRM_SUCCESS;
        }
        ChkDR( dr );

        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_EXPLICIT_ANALOG_VIDEO_OUTPUT_PROTECTION_CONTAINER, (DRM_VOID*) &xmrExplicitAnalogVideoContainer));
    }

    if (IsRightSet(pRights->DigitalAudioRestrictions))
    {
        DRM_DWORD dwData = 0;
        DRM_DWORD index = 0;
        DRM_CONST_STRING dstrExtension = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrGUID      = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrConfigData = EMPTY_DRM_STRING;

        ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_EXPLICIT_DIGITAL_AUDIO_OUTPUT_PROTECTION_CONTAINER, &xmrExplicitDigitalAudioContainer));

        while ( DRM_SUCCEEDED( dr = DRM_XML_GetSubNode( &pRights->DigitalAudioRestrictions,
                                                 &g_dstrTagOPLExtension,
                                                 NULL,
                                                 NULL,
                                                 index++,
                                                 &dstrExtension,
                                                 NULL,
                                                 1) ) )  /* EXTENSION is the second layer */
        {
            cbLen = SIZEOF(DRM_XMR_AUDIO_OUTPUT_CONFIGURATION_LIST);
            ChkMem(pxmrExplicitDigitalAudioTemp = (DRM_XMR_AUDIO_OUTPUT_CONFIGURATION_LIST*)Oem_MemAlloc(cbLen));
            memset(pxmrExplicitDigitalAudioTemp, 0, cbLen);

            pxmrExplicitDigitalAudioTemp->Config.fValid       = TRUE;
            pxmrExplicitDigitalAudioTemp->Config.iGuid        = 0;
            pxmrExplicitDigitalAudioTemp->Config.iConfigData  = 0;

            ChkDR( DRM_XML_GetNodeAttribute( &dstrExtension,
                                             &g_dstrAttribGuid,
                                             &dstrGUID ) );

            cbLen = SIZEOF( DRM_GUID );
            ChkMem(pxmrExplicitDigitalAudioTemp->Config.pguidBuffer = (DRM_BYTE*)Oem_MemAlloc(cbLen));
            memset(pxmrExplicitDigitalAudioTemp->Config.pguidBuffer, 0, cbLen);
            ChkDR( DRM_UTL_StringToGuid( &dstrGUID,
                                         (DRM_GUID*)(pxmrExplicitDigitalAudioTemp->Config.pguidBuffer) ) );

            ChkDR( DRM_XML_GetNodeAttribute( &dstrExtension,
                                             &g_dstrAttribValue,
                                             &dstrConfigData ) );

            ChkDR( DRMCRT_WtoDWORD( dstrConfigData.pwszString, dstrConfigData.cchString, 10, &dwData, NULL ) );

            dr = DRM_XML_GetNodeAttribute( &dstrExtension,
                                             &g_dstrAttribLength,
                                             &dstrConfigData );
            if (dr == DRM_SUCCESS)
            {
                ChkDR( DRMCRT_WtoDWORD( dstrConfigData.pwszString, dstrConfigData.cchString, 10, &cbLen, NULL ) );
            }
            else
            {
                cbLen = SIZEOF( DRM_DWORD );
            }

            pxmrExplicitDigitalAudioTemp->Config.cbConfigData = cbLen;

            ChkMem( pxmrExplicitDigitalAudioTemp->Config.pbConfigDataBuffer = (DRM_BYTE*)Oem_MemAlloc( cbLen ) );
            memcpy( pxmrExplicitDigitalAudioTemp->Config.pbConfigDataBuffer, &dwData, min( cbLen, SIZEOF( DRM_DWORD ) ) );
            _ReverseBytes(pxmrExplicitDigitalAudioTemp->Config.pbConfigDataBuffer, min( cbLen, SIZEOF( DRM_DWORD ) ) );

            pxmrExplicitDigitalAudioTemp->pNext = xmrExplicitDigitalAudioContainer.plistOutputConfiguration;
            xmrExplicitDigitalAudioContainer.plistOutputConfiguration = pxmrExplicitDigitalAudioTemp;
            xmrExplicitDigitalAudioContainer.cOutputProtectionIDs++;

            pxmrExplicitDigitalAudioTemp = NULL;
        }

        if ( DRM_E_XMLNOTFOUND == dr ) /* no more EXTENSION nodes */
        {
            dr = DRM_SUCCESS;
        }
        ChkDR( dr );

        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_EXPLICIT_DIGITAL_AUDIO_OUTPUT_PROTECTION_CONTAINER, (DRM_VOID*) &xmrExplicitDigitalAudioContainer));
    }

    if (IsRightSet(pRights->PolicyMetadataList))
    {
        DRM_DWORD dwData = 0;
        DRM_DWORD index = 0;
        DRM_DWORD dwLen = 0;
        EValueType eValueType = ETYPE_DWORD;
        DRM_CONST_STRING dstrPolicyMetadata = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrGUID      = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrConfigData = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrType = EMPTY_DRM_STRING;

        while ( DRM_SUCCEEDED( dr = DRM_XML_GetSubNode( &pRights->PolicyMetadataList,
                                                 &g_dstrTagPolicyMetadata,
                                                 NULL,
                                                 NULL,
                                                 index++,
                                                 &dstrPolicyMetadata,
                                                 NULL,
                                                 0) ) )  /* is the first layer */
        {
            cbLen = SIZEOF(DRM_XMR_OUTPUT_CONFIGURATION_LIST );
            ChkMem(pxmrPolicyMetadataTemp = (DRM_XMR_OUTPUT_CONFIGURATION_LIST *)Oem_MemAlloc(cbLen));
            memset(pxmrPolicyMetadataTemp, 0, cbLen);

            ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_POLICY_METADATA_OBJECT, &pxmrPolicyMetadataTemp->Config));

            ChkDR( DRM_XML_GetNodeAttribute( &dstrPolicyMetadata,
                                             &g_dstrAttribGuid,
                                             &dstrGUID ) );

            cbLen = SIZEOF( DRM_GUID );
            ChkMem(pxmrPolicyMetadataTemp->Config.pguidBuffer = (DRM_BYTE*)Oem_MemAlloc(cbLen));
            memset(pxmrPolicyMetadataTemp->Config.pguidBuffer, 0, cbLen);
            ChkDR( DRM_UTL_StringToGuid( &dstrGUID,
                                         (DRM_GUID*)(pxmrPolicyMetadataTemp->Config.pguidBuffer) ) );

            dr = DRM_XML_GetNodeAttribute( &dstrPolicyMetadata,
                                             &g_dstrAttribType,
                                             &dstrType );
            if (dr == DRM_E_INVALIDXMLTAG)
            {
                dr = DRM_SUCCESS;
                eValueType = ETYPE_DWORD;
            }
            else if (dr == DRM_SUCCESS)
            {
                if (CompareDSTRToWCHAR(&dstrType, dwchDWORD) == DRM_SUCCESS)
                {
                    eValueType = ETYPE_DWORD;
                }
                else if (CompareDSTRToWCHAR(&dstrType, dwchWCHAR) == DRM_SUCCESS)
                {
                    eValueType = ETYPE_WCHAR;
                }
                else if (CompareDSTRToWCHAR(&dstrType, dwchBINARY) == DRM_SUCCESS)
                {
                    eValueType = ETYPE_BINARY;
                }
                else
                    ChkArg(0);
            }
            ChkDR(dr);


            ChkDR( DRM_XML_GetNodeAttribute( &dstrPolicyMetadata,
                                             &g_dstrAttribValue,
                                             &dstrConfigData ) );

            switch(eValueType)
            {
            case ETYPE_DWORD:
                pxmrPolicyMetadataTemp->Config.cbConfigData = SIZEOF( DRM_DWORD );
                ChkDR( DRMCRT_WtoDWORD( dstrConfigData.pwszString, dstrConfigData.cchString, 10, &dwData, NULL ) );
                ChkMem( pxmrPolicyMetadataTemp->Config.pbConfigDataBuffer = (DRM_BYTE*)Oem_MemAlloc( SIZEOF( DRM_DWORD) ));
                memcpy( pxmrPolicyMetadataTemp->Config.pbConfigDataBuffer, &dwData, SIZEOF( DRM_DWORD ) );
                break;

            case ETYPE_WCHAR:
                dwLen = dstrConfigData.cchString * 2;
                pxmrPolicyMetadataTemp->Config.cbConfigData = dwLen;
                ChkMem( pxmrPolicyMetadataTemp->Config.pbConfigDataBuffer = (DRM_BYTE*)Oem_MemAlloc( dwLen ) );
                memcpy( pxmrPolicyMetadataTemp->Config.pbConfigDataBuffer, dstrConfigData.pwszString, dwLen );
                break;

            case ETYPE_BINARY:
                dr = DRM_B64_DecodeW( &dstrConfigData, &dwLen, NULL, 0 );
                if ( DRM_E_BUFFERTOOSMALL == dr )
                {
                    ChkMem( pxmrPolicyMetadataTemp->Config.pbConfigDataBuffer = (DRM_BYTE*)Oem_MemAlloc( dwLen ) );
                    ChkDR( DRM_B64_DecodeW( &dstrConfigData
                                            , &dwLen
                                            , pxmrPolicyMetadataTemp->Config.pbConfigDataBuffer
                                            , 0 ) );
                }
                ChkDR(dr);
                pxmrPolicyMetadataTemp->Config.cbConfigData = dwLen;
                break;

            default:
                ChkArg(0);
            }

            pxmrPolicyMetadataTemp->pNext = pxmrPolicyMetadataList;
            pxmrPolicyMetadataList = pxmrPolicyMetadataTemp;

            ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_POLICY_METADATA_OBJECT, (DRM_VOID*) &pxmrPolicyMetadataTemp->Config));

            pxmrPolicyMetadataTemp = NULL;
        }

        if ( DRM_E_XMLNOTFOUND == dr ) /* no more EXTENSION nodes */
        {
            dr = DRM_SUCCESS;
        }
        ChkDR( dr );
    }


/************************************E X P O L I C Y ***********************************************/
    if (IsRightSet(pRights->XPolicy))
    {
        DRM_CONST_STRING dstrRight = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrObjectType = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrObjectFlags = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrRestrictionsList = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrRestriction = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrPolicyData = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrSecureStateData = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrSecureStateType = EMPTY_DRM_STRING;
        DRM_BYTE* pExtendedRestriction;
        DRM_WORD wRightType = 0;
        DRM_WORD wRightFlags = 0;
        DRM_WORD wSubRightType = 0;
        DRM_WORD wSubRightFlags = 0;
        DRM_WORD wRestrictionType = 0;
        DRM_WORD wRestrictionFlags = 0;
        DRM_DWORD dwExtendedRestrictionSize = 0;
        DRM_BYTE bSecureStateType = 0;
        DRM_WORD wSecureStateCount = 0;
        int iXRight = 0;
        int iXRestriction = 0;
        void* pStateDateCountorBinary = NULL;
        DRM_WORD cStateDateCountorBinary = 0;
        DRM_BYTE* pBinaryData = NULL;
        while( DRM_SUCCEEDED( dr = DRM_XML_GetSubNode( &pRights->XPolicy,
                                                   &g_dstrTagExPolicyRight,
                                                   NULL,
                                                   NULL,
                                                   iXRight++,
                                                   &dstrRight,
                                                   NULL,
                                                   1) ) )
        {
            ChkDR( DRM_XML_GetNodeAttribute( &dstrRight,
                                             &g_dstrAttribExPolicyObjectType,
                                             &dstrObjectType ) );

            ChkDR( DRM_XML_GetNodeAttribute( &dstrRight,
                                             &g_dstrAttribExPolicyObjectFlags,
                                             &dstrObjectFlags ) );

            ChkDR( DRMCRT_WtoWORD( dstrObjectType.pwszString, dstrObjectType.cchString, 10, &wRightType, NULL ) );
            ChkDR( DRMCRT_WtoWORD( dstrObjectFlags.pwszString, dstrObjectFlags.cchString, 10, &wRightFlags, NULL ) );

            if (IsRightSet(pRights->VerifyExtensibleObject)
                && (CompareDSTRToWCHAR(&pRights->VerifyExtensibleObject, dwchYes) == DRM_SUCCESS))
            {
                if (IsRightSet(pRights->VerifyPlayRightsExtensibleContainer)
                && (CompareDSTRToWCHAR(&pRights->VerifyPlayRightsExtensibleContainer, dwchYes) == DRM_SUCCESS))
                {
                    if (xmrPlayRights.fValid )
                    {
                        /* Add any unknown container without checking the type and flag even if the invalid one
                           to XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER */
                        ChkDR( _AddAnyUnknownObject( &xmrBuilder,
                                                 wRightType,
                                                 TRUE,
                                                 XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER,
                                                 wRightFlags,
                                                 NULL,
                                                 0 ) );
                    }
                    else
                    {
                        ChkDR(DRM_E_TEST_VARIABLE_NOT_SET);
                    }
                }
                else if (IsRightSet(pRights->VerifyPlayRightsExtensibleObject)
                    && (CompareDSTRToWCHAR(&pRights->VerifyPlayRightsExtensibleObject, dwchYes) == DRM_SUCCESS))
                {
                    /* Not adding any container but only add extensible object into XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER later. */
                }
                else
                {
                    /* Add any unknown object without checking the type and flag even if the invalid one */
                    ChkDR( _AddAnyUnknownObject( &xmrBuilder,
                                             wRightType,
                                             TRUE,
                                             XMR_OBJECT_TYPE_OUTER_CONTAINER,
                                             wRightFlags,
                                             NULL,
                                             0 ) );
                 }

                 if (IsRightSet(pRights->ExtensibleDepth))
                 {
                     ChkDR( DRM_XML_GetNodeAttribute( &dstrRight,
                                                 &g_dstrAttribExPolicySubObjectType,
                                                 &dstrObjectType ) );

                     ChkDR( DRM_XML_GetNodeAttribute( &dstrRight,
                                                 &g_dstrAttribExPolicySubObjectFlags,
                                                 &dstrObjectFlags ) );

                     ChkDR( DRMCRT_WtoWORD( dstrObjectType.pwszString, dstrObjectType.cchString, 10, &wSubRightType, NULL ) );
                     ChkDR( DRMCRT_WtoWORD( dstrObjectFlags.pwszString, dstrObjectFlags.cchString, 10, &wSubRightFlags, NULL ) );

                     /* Add any unknown sub container as a child of previous unknown container */
                     ChkDR( _AddAnyUnknownObject( &xmrBuilder,
                                         wSubRightType,
                                         TRUE,
                                         wRightType,
                                         wSubRightFlags,
                                         NULL,
                                         0 ) );

                 }
            }
            else
            {
                ChkArg( IsValidExtensibleRightRestriction(wRightFlags) );

                ChkDR( DRM_XMR_AddUnknownObject( &xmrBuilder,
                                     wRightType,
                                     TRUE,
                                     XMR_OBJECT_TYPE_OUTER_CONTAINER,
                                     wRightFlags,
                                     NULL,
                                     0 ) );
             }


            /*
              Cannot call DRM_XMR_AddUnknownObject here because
              a right can only to added to the overall license after all restrictions have been
              added to that right. The call to DRM_XMR_AddUnknownObject therefore appears later
              after the right object is fully constructed.
            */

            dr = DRM_XML_GetSubNode( &dstrRight,
                                     &g_dstrTagExPolicyRestrictionsList,
                                     NULL,
                                     NULL,
                                     0,
                                     &dstrRestrictionsList,
                                     NULL,
                                     1 );

           if (dr == DRM_E_XMLNOTFOUND)
           {
               dr = DRM_SUCCESS;   /*  not fatal error.  Probably did not find a restriction to this extended right */
               continue;
           }

           while ( DRM_SUCCEEDED( dr = DRM_XML_GetSubNode( &dstrRestrictionsList,
                                                       &g_dstrTagExPolicyRestriction,
                                                       NULL,
                                                       NULL,
                                                       iXRestriction++,
                                                       &dstrRestriction,
                                                       NULL,
                                                       1) ) )
           {

               ChkDR ( DRM_XML_GetNodeAttribute( &dstrRestriction,
                                                 &g_dstrAttribExPolicyObjectType,
                                                 &dstrObjectType) );

                ChkDR ( DRM_XML_GetNodeAttribute( &dstrRestriction,
                                                  &g_dstrAttribExPolicyObjectFlags,
                                                  &dstrObjectFlags) );

                ChkDR ( DRM_XML_GetNodeAttribute( &dstrRestriction,
                                                  &g_dstrAttribExPolicyPolicyData,
                                                  &dstrPolicyData) );

                ChkDR ( DRM_XML_GetNodeAttribute( &dstrRestriction,
                                                  &g_dstrAttribExPolicySecureStateType,
                                                  &dstrSecureStateType) );

                ChkDR( DRMCRT_WtoWORD( dstrObjectType.pwszString, dstrObjectType.cchString, 10, &wRestrictionType, NULL ) );
                ChkDR( DRMCRT_WtoWORD( dstrObjectFlags.pwszString, dstrObjectFlags.cchString, 10, &wRestrictionFlags, NULL ) )
                ChkDR( DRMCRT_WtoDWORD( dstrSecureStateType.pwszString, dstrSecureStateType.cchString, 10, &dwTemp, NULL ) );
                bSecureStateType = (DRM_BYTE)dwTemp;

                switch(bSecureStateType)
                    {
                        case XMR_EXTENSIBLE_RESTRICTON_STATE_COUNT:
                                ChkDR ( DRM_XML_GetNodeAttribute( &dstrRestriction,
                                                              &g_dstrAttribExPolicySecureStateCount,
                                                              &dstrSecureStateData) );
                                if( IsRightSet(dstrSecureStateData) )
                                {
                                    ChkDR( DRMCRT_WtoWORD( dstrSecureStateData.pwszString, dstrSecureStateData.cchString, 10, &wSecureStateCount, NULL ) );
                                }
                                pStateDateCountorBinary = (void*)&wSecureStateCount;
                                cStateDateCountorBinary = SIZEOF(DRM_DWORD);
                                break;
                        case XMR_EXTENSIBLE_RESTRICTON_STATE_DATE:
#if TEST_SUPPORT_XMR_EXTENSIBLE_RESTRICTON_STATE_DATE
                                {
                                    DRM_UINT64 ui64SecureStateDate = DRM_UI64LITERAL(0,0);
                                    TOKEN mytoken;
                                    ChkDR ( DRM_XML_GetNodeAttribute( &dstrRestriction,
                                                                      &g_dstrAttribExPolicySecureStateDate,
                                                                      &dstrSecureStateData) );
                                    ChkDR( DRM_EXPR_ExtractDateToken( &dstrSecureStateData, &mytoken ) );
                                    ui64SecureStateDate = mytoken.val.u64DateTime;
                                    pStateDateCountorBinary = (void*)&ui64SecureStateDate;
                                    cStateDateCountorBinary = SIZEOF(DRM_UINT64);
                                }
#else /* TEST_SUPPORT_XMR_EXTENSIBLE_RESTRICTON_STATE_DATE */
                                ChkDR( DRM_E_NOTIMPL );
#endif /* TEST_SUPPORT_XMR_EXTENSIBLE_RESTRICTON_STATE_DATE */
                                break;
                        case XMR_EXTENSIBLE_RESTRICTON_STATE_BYTEARRAY:
                                ChkDR ( DRM_XML_GetNodeAttribute( &dstrRestriction,
                                                                  &g_dstrAttribExPolicySecureStateData,
                                                                  &dstrSecureStateData) );
                                ChkDR(DRM_B64_DecodeW(&dstrSecureStateData,(DRM_DWORD*)&cStateDateCountorBinary, pBinaryData, 0));
                                pStateDateCountorBinary = (void*)pBinaryData;
                                break;
                        default:
                                ChkDR( DRM_E_INVALIDARG );
                       }

                ChkDR(DRM_XMR_CreateExtensibleRestriction(&xmrBuilder,
                                                          (DRM_BYTE*)dstrPolicyData.pwszString,
                                                          (DRM_WORD)SIZEOF(dstrPolicyData.pwszString),
                                                          cStateDateCountorBinary,
                                                          bSecureStateType,
                                                          pStateDateCountorBinary,
                                                          cStateDateCountorBinary,
                                                          &pExtendedRestriction,
                                                          &dwExtendedRestrictionSize));

                if (IsRightSet(pRights->VerifyExtensibleObject)
                    && (CompareDSTRToWCHAR(&pRights->VerifyExtensibleObject, dwchYes) == DRM_SUCCESS))
                {
                    /* Add any unknown object without checking the type and flag even if the invalid one */
                    if (IsRightSet(pRights->ExtensibleDepth)) /* more than one lever of container */
                    {
                        /* Add the restriction to sub unknown container */
                        ChkDR( _AddAnyUnknownObject( &xmrBuilder,
                                        wRestrictionType,
                                        TRUE,
                                        wSubRightType,
                                        wRestrictionFlags,
                                        pExtendedRestriction,
                                        dwExtendedRestrictionSize ) );
                    }
                    else /* only one level of container under outer container */
                    {
                        if (IsRightSet(pRights->VerifyPlayRightsExtensibleObject)
                        && (CompareDSTRToWCHAR(&pRights->VerifyPlayRightsExtensibleObject, dwchYes) == DRM_SUCCESS))
                        {
                            if (xmrPlayRights.fValid )
                            {
                                /* Add the restriction to XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER */
                                ChkDR( _AddAnyUnknownObject( &xmrBuilder,
                                            wRestrictionType,
                                            TRUE,
                                            XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER,
                                            wRestrictionFlags,
                                            pExtendedRestriction,
                                            dwExtendedRestrictionSize ) );
                            }
                            else
                            {
                                ChkDR(DRM_E_TEST_VARIABLE_NOT_SET);
                            }
                        }
                        else
                        {
                            /* Add the restriction to unknown container */
                            ChkDR( _AddAnyUnknownObject( &xmrBuilder,
                                        wRestrictionType,
                                        TRUE,
                                        wRightType,
                                        wRestrictionFlags,
                                        pExtendedRestriction,
                                        dwExtendedRestrictionSize ) );
                        }

                    }
                }
                else
                {
                    ChkArg( IsValidExtensibleRightRestriction(wRestrictionFlags) );

                    ChkDR( DRM_XMR_AddUnknownObject( &xmrBuilder,
                                        wRestrictionType,
                                        TRUE,
                                        wRightType,
                                        wRestrictionFlags,
                                        pExtendedRestriction,
                                        dwExtendedRestrictionSize ) );
                }

            }

            if ( DRM_E_XMLNOTFOUND == dr ) /* no more Rights nodes */
            {
                dr = DRM_SUCCESS;
            }

            ChkDR( dr );

        }


        if ( DRM_E_XMLNOTFOUND == dr ) /* no more Rights nodes */
        {
            dr = DRM_SUCCESS;
        }

        ChkDR( dr );
    }

    if (IsRightTrue(pRights->AllowCopy) ||
        IsRightTrue(pRights->AllowTransferToSDMI) ||
        IsRightTrue(pRights->AllowTransferToNonSDMI))
    {
        ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_COPY_POLICY_CONTAINER, &xmrCopyRights));
        xmrCopyRights.fValid = TRUE;
        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_COPY_POLICY_CONTAINER, (DRM_VOID*) &xmrCopyRights));
    }


    /* Add CopyEnablerGUIDList */
    if (IsRightSet(pRights->CopyEnablerGUIDList))
    {
        DRM_DWORD index = 0;
        DRM_CONST_STRING dstrCopyEnablerGUID = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrGUID      = EMPTY_DRM_STRING;


        ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_COPY_POLICY_2_CONTAINER, &xmrCopyRights2));
        xmrCopyRights2.fValid = TRUE;
        xmrCopyRights2.dwVersion = 2;
        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_COPY_POLICY_2_CONTAINER, (DRM_VOID*) &xmrCopyRights2));


        while ( DRM_SUCCEEDED( dr = DRM_XML_GetSubNode( &pRights->CopyEnablerGUIDList,
                                                 &g_dstrTagCopyEnablerGUID,
                                                 NULL,
                                                 NULL,
                                                 index++,
                                                 &dstrCopyEnablerGUID,
                                                 NULL,
                                                 0) ) )  /* is the first layer */
        {
            cbLen = SIZEOF(DRM_XMR_OUTPUT_CONFIGURATION_LIST );
            ChkMem(pxmrCopyEnablerGUIDTemp = (DRM_XMR_OUTPUT_CONFIGURATION_LIST *)Oem_MemAlloc(cbLen));
            memset(pxmrCopyEnablerGUIDTemp, 0, cbLen);

            ChkDR( DRM_XML_GetNodeAttribute( &dstrCopyEnablerGUID,
                                             &g_dstrAttribGuid,
                                             &dstrGUID ) );

            cbLen = SIZEOF( DRM_GUID );
            ChkMem(pxmrCopyEnablerGUIDTemp->Config.pguidBuffer = (DRM_BYTE*)Oem_MemAlloc(cbLen));
            memset(pxmrCopyEnablerGUIDTemp->Config.pguidBuffer, 0, cbLen);
            ChkDR( DRM_UTL_StringToGuid( &dstrGUID,
                                         (DRM_GUID*)(pxmrCopyEnablerGUIDTemp->Config.pguidBuffer) ) );

            pxmrCopyEnablerGUIDTemp->pNext = pxmrCopyEnablerGUIDList;
            pxmrCopyEnablerGUIDList = pxmrCopyEnablerGUIDTemp;

            ChkDR( DRM_XMR_AddUnknownObject(
                   &xmrBuilder, XMR_OBJECT_TYPE_COPY_ENABLER_CONTAINER, TRUE, XMR_OBJECT_TYPE_COPY_POLICY_2_CONTAINER, XMR_FLAGS_CONTAINER, NULL, 0));

            ChkDR( DRM_XMR_AddUnknownObject(
                   &xmrBuilder, XMR_OBJECT_TYPE_COPY_ENABLER_OBJECT, TRUE, XMR_OBJECT_TYPE_COPY_ENABLER_CONTAINER, 0,  (DRM_BYTE*)pxmrCopyEnablerGUIDTemp->Config.pguidBuffer, cbLen));

            pxmrCopyEnablerGUIDTemp = NULL;
        }

        if ( DRM_E_XMLNOTFOUND == dr ) /* no more EXTENSION nodes */
        {
            dr = DRM_SUCCESS;
        }

        ChkDR( dr );
    }

    if (IsRightSet(pRights->CopyCount2))
    {
        ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_COPYCOUNT_2_OBJECT, &xmrCopyCount));
        ChkDR( DRMCRT_WtoDWORD( pRights->CopyCount2.pwszString, pRights->CopyCount2.cchString, 10, &xmrCopyCount.dwValue, NULL ) );
        xmrCopyCount.dwVersion = 2;
        xmrCopyCount.fValid = TRUE;
        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_COPYCOUNT_2_OBJECT, (DRM_VOID*) &xmrCopyCount));
    }

    if (IsRightSet(pRights->MoveEnabler))
    {
        ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_MOVE_ENABLER_OBJECT, &xmrMoveEnabler));
        ChkDR( DRMCRT_WtoDWORD( pRights->MoveEnabler.pwszString, pRights->MoveEnabler.cchString, 10,&xmrMoveEnabler.dwValue, NULL ) );
        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_MOVE_ENABLER_OBJECT, (DRM_VOID*) &xmrMoveEnabler));
    }

    for( idx = 0; idx < g_cdwExtensiblePolicyMapMax; idx++ )
    {
        if( IsRightTrue( pRights->AllowExtensibleRights[idx] ) )
        {
            ChkDR( DRM_XMR_AddUnknownObject(
                &xmrBuilder,
                g_XMRLicGenExtensiblePolicyMap[idx].dwXMRObjectTypeId,
                TRUE,
                XMR_OBJECT_TYPE_OUTER_CONTAINER,
                XMR_FLAGS_CONTAINER | XMR_FLAGS_ALLOW_EXTERNAL_PARSE,
                NULL,
                0 ) );
        }
    }

    if (IsRightTrue(pRights->AllowPlaylistBurn))
    {
        ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_ALLOW_PLAYLISTBURN_POLICY_CONTAINER, &xmrPlayListBurn));
        xmrPlayListBurn.fValid = TRUE;
        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_ALLOW_PLAYLISTBURN_POLICY_CONTAINER, (DRM_VOID*) &xmrPlayListBurn));
    }

    if (IsRightSet(pRights->PlaylistBurnTrackCount) || IsRightSet(pRights->MaxPlaylistBurnCount))
    {
        ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_PLAYLISTBURN_COUNT_OBJECT, &xmrBurnCount));
        if (IsRightSet(pRights->PlaylistBurnTrackCount))
        {
            ChkDR( DRMCRT_WtoDWORD( pRights->PlaylistBurnTrackCount.pwszString, pRights->PlaylistBurnTrackCount.cchString, 10, &xmrBurnCount.dwPlaylistBurnTrackCount, NULL ) );
        }
        if (IsRightSet(pRights->MaxPlaylistBurnCount))
        {
            ChkDR( DRMCRT_WtoDWORD( pRights->MaxPlaylistBurnCount.pwszString, pRights->MaxPlaylistBurnCount.cchString, 10, &xmrBurnCount.dwMaxPlaylistBurnCount, NULL ) );
        }
        xmrBurnCount.fValid = TRUE;
        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_PLAYLISTBURN_COUNT_OBJECT, (DRM_VOID*) &xmrBurnCount));
    }

    SAFE_OEM_FREE( pbClear );

    if( pdstrUplinkKID && !IsRightSet( pRights->UplinkX ) )
    {
        ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_UPLINK_KID_2_OBJECT, &xmrUplink));

        xmrUplink.pbguidUplinkKID = (DRM_BYTE*)drmUplinkKID.rgb;
        xmrUplink.iguidUplinkKID = 0;
        xmrUplink.cbChainedCheckSum = (DRM_WORD)cbCheckSum;
        xmrUplink.pbChainedCheckSumBuffer = bCheckSum;

        if ( dwTestOptions & OP_LICGEN_INVALID_UPLINK_CHECKSUM )
        {
            bCheckSum[0] = ~bCheckSum[0];    /* should be enough to invalidate the checksum. */
        }
        xmrUplink.iChainedCheckSum = 0;
        xmrUplink.fValid = TRUE;
        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_UPLINK_KID_2_OBJECT, (DRM_VOID*) &xmrUplink));
    }

    if (IsRightSet(pRights->SourceID))
    {
        ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_SOURCEID_OBJECT, &xmrSourceID));
        ChkDR( DRMCRT_WtoDWORD( pRights->SourceID.pwszString, pRights->SourceID.cchString, 10, &xmrSourceID.dwValue, NULL ) );
        xmrSourceID.fValid = TRUE;
        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_SOURCEID_OBJECT, (DRM_VOID*) &xmrSourceID));
    }

    if (IsRightSet(pRights->GracePeriod))
    {
        ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_GRACE_PERIOD_OBJECT, &xmrGracePeriod));
        ChkDR( DRMCRT_WtoDWORD( pRights->GracePeriod.pwszString, pRights->GracePeriod.cchString, 10, &xmrGracePeriod.dwValue, NULL ) );
        if ( pRights->ExpirationOnStore.cchString > 0)
        {
            if ( pRights->GracePeriod.pwszString[pRights->GracePeriod.cchString-1]==L'm')
                xmrGracePeriod.dwValue = xmrGracePeriod.dwValue * 60;
            else if ( pRights->GracePeriod.pwszString[pRights->GracePeriod.cchString-1]==L'h')
                xmrGracePeriod.dwValue = xmrGracePeriod.dwValue * 60*60;
            else if ( pRights->GracePeriod.pwszString[pRights->GracePeriod.cchString-1]==L'd')
                xmrGracePeriod.dwValue = xmrGracePeriod.dwValue * 60*60*24;
        }
        xmrGracePeriod.fValid = TRUE;
        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_GRACE_PERIOD_OBJECT, (DRM_VOID*) &xmrGracePeriod));
    }

    if (IsRightSet(pRights->UID))
    {
        ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_USERID_OBJECT, &xmrUID));

        xmrUID.pbUserID = (DRM_BYTE *)pRights->UID.pwszString;
        xmrUID.cbUserID = pRights->UID.cchString * SIZEOF(DRM_WCHAR);
        xmrUID.iUserID = 0;
        xmrUID.fValid = TRUE;
        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_USERID_OBJECT, (DRM_VOID*) &xmrUID));
    }

    if (IsRightSet(pRights->InclusionList))
    {
        DRM_DWORD dwGuids=0, i=0;
        DRM_CONST_STRING wszGUIDString;

        ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_INCLUSION_LIST_OBJECT, &xmrInclusionList));
        for( i=0; i<pRights->InclusionList.cchString; ++i)
        {
            if ( *(pRights->InclusionList.pwszString+i) == L'{' )
                dwGuids++;
        }

        ChkMem( xmrInclusionList.pguidBuffer = (DRM_BYTE*)Oem_MemAlloc(dwGuids*SIZEOF(DRM_GUID)));
        MEMSET(xmrInclusionList.pguidBuffer, 0, dwGuids*SIZEOF(DRM_GUID));

        xmrInclusionList.cGUIDs = 0;
        for( i=0; i<pRights->InclusionList.cchString && (xmrInclusionList.cGUIDs)<dwGuids; ++i)
        {
            if ( *(pRights->InclusionList.pwszString+i) == L'{' )
            {
                wszGUIDString.cchString = DRM_GUID_STRING_LEN; /* includes both '{' and '}' */
                wszGUIDString.pwszString = pRights->InclusionList.pwszString+i;
                ChkDR( DRM_UTL_StringToGuid( &wszGUIDString, (DRM_GUID *)(xmrInclusionList.pguidBuffer + xmrInclusionList.cGUIDs*SIZEOF(DRM_GUID))));
                xmrInclusionList.cGUIDs++;
            }
        }
        xmrInclusionList.fValid = TRUE;
        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_INCLUSION_LIST_OBJECT, (DRM_VOID*) &xmrInclusionList));
    }

    if (IsRightSet(pRights->SerialNumber))
    {
        ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_SERIAL_NUMBER_OBJECT, &xmrSerialNumber));
        cbSerialNumber= 0;
        dr = DRM_B64_DecodeW(&pRights->SerialNumber, &cbSerialNumber, NULL, 0);
        if (dr == DRM_E_BUFFERTOOSMALL)
        {
            ChkMem(pbSerialNumber = (DRM_BYTE*)Oem_MemAlloc(cbSerialNumber));
            ChkDR(DRM_B64_DecodeW(&pRights->SerialNumber, &cbSerialNumber, pbSerialNumber, 0));
        }
        else
        {
            ChkDR(dr);
            ChkDR(DRM_E_XMLSIG_INVALID_KEY_FORMAT);  /* error out if DRM_SUCCESS */
        }

        xmrSerialNumber.cbData = cbSerialNumber;
        xmrSerialNumber.pbDataBuffer = pbSerialNumber;
        xmrSerialNumber.iData = 0;
        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_SERIAL_NUMBER_OBJECT, (DRM_VOID*) &xmrSerialNumber));
    }


    if ( IsRightSet(pRights->LGPUBKEY))
    {
        ChkDR( DRM_B64_DecodeW( &pRights->LGPUBKEY,
                                &cbPubKeyModulus,
                                 rgbLGPubKeyModulus,
                                 0 ) );

        ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_RSA_LICENSE_GRANTER_KEY_OBJECT, &xmrLGPubKey));

        xmrLGPubKey.fValid = TRUE;
        xmrLGPubKey.dwExponent = 0x010001;  /* 65537 */
        xmrLGPubKey.cbModulus = (DRM_WORD)cbPubKeyModulus;
        xmrLGPubKey.pbModulusBuffer = rgbLGPubKeyModulus;
        xmrLGPubKey.iModulus = 0;

        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_RSA_LICENSE_GRANTER_KEY_OBJECT, (DRM_VOID*) &xmrLGPubKey));
    }

    if (IsRightSet(pRights->RevInfoVersion) && (CompareDSTRToWCHAR(&pRights->RevInfoVersion, dwchNoRIV) == DRM_SUCCESS))
    {
       /* do not add a RIV2 object */
    }
    else
    {
        ChkDR(DRM_XMR_InitializeObject(XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_2_OBJECT, &xmrRiv));
        /* RIV is set to 0 by default if not specified in the xml input file */
        xmrRiv.dwValue = 0;
        xmrRiv.fValid = TRUE;

        if (IsRightSet(pRights->RevInfoVersion))
        {
            if (pRights->RevInfoVersion.pwszString[0] == L'*')
            {
                /* set RIV to the current RIV2 value in REV_INFO1 */
                ChkArg(pOptionalParams);
                xmrRiv.dwValue = pOptionalParams->dwMinimalRIV2;
            }
            else
            {
                ChkDR( DRMCRT_WtoDWORD( pRights->RevInfoVersion.pwszString, pRights->RevInfoVersion.cchString, 10, &xmrRiv.dwValue, NULL ) );
            }
        }
        ChkDR(DRM_XMR_AddObject(&xmrBuilder, XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_2_OBJECT, (DRM_VOID*) &xmrRiv));
    }


/* TODO : Replace common functionality by a function call */
    if (IsRightSet(pRights->XPolicyPlayExtended))
    {
        DRM_CONST_STRING dstrObjectType = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrObjectFlags = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrRestriction = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrPolicyData = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrSecureStateData = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrSecureStateType = EMPTY_DRM_STRING;
        DRM_BYTE* pExtendedRestriction;
        DRM_WORD wRestrictionType = 0;
        DRM_WORD wRestrictionFlags = 0;
        DRM_DWORD dwExtendedRestrictionSize = 0;
        DRM_BYTE bSecureStateType = 0;
        DRM_WORD wSecureStateCount = 0;
        int iXRestriction = 0;
        void* pStateDateCountorBinary = NULL;
        DRM_WORD cStateDateCountorBinary = 0;
        DRM_BYTE* pBinaryData = NULL;

        while ( DRM_SUCCEEDED( dr = DRM_XML_GetSubNode( &pRights->XPolicyPlayExtended,
                                                    &g_dstrTagExPolicyRestriction,
                                                    NULL,
                                                    NULL,
                                                    iXRestriction++,
                                                    &dstrRestriction,
                                                    NULL,
                                                    1) ) )
        {
            ChkDR ( DRM_XML_GetNodeAttribute( &dstrRestriction,
                                              &g_dstrAttribExPolicyObjectType,
                                              &dstrObjectType) );

            ChkDR ( DRM_XML_GetNodeAttribute( &dstrRestriction,
                                              &g_dstrAttribExPolicyObjectFlags,
                                              &dstrObjectFlags) );

            ChkDR ( DRM_XML_GetNodeAttribute( &dstrRestriction,
                                              &g_dstrAttribExPolicyPolicyData,
                                              &dstrPolicyData) );

            ChkDR ( DRM_XML_GetNodeAttribute( &dstrRestriction,
                                              &g_dstrAttribExPolicySecureStateType,
                                              &dstrSecureStateType) );

            ChkDR( DRMCRT_WtoWORD( dstrObjectType.pwszString, dstrObjectType.cchString, 10, &wRestrictionType, NULL ) );
            ChkDR( DRMCRT_WtoWORD( dstrObjectFlags.pwszString, dstrObjectFlags.cchString, 10, &wRestrictionFlags, NULL ) );
            if( IsRightSet(dstrSecureStateType) )
            {
                ChkDR( DRMCRT_WtoDWORD( dstrSecureStateType.pwszString, dstrSecureStateType.cchString, 10, &dwTemp, NULL ) );
                bSecureStateType = (DRM_BYTE)dwTemp;
            }

            switch(bSecureStateType)
            {
                case XMR_EXTENSIBLE_RESTRICTON_STATE_COUNT:
                    ChkDR ( DRM_XML_GetNodeAttribute( &dstrRestriction,
                                                      &g_dstrAttribExPolicySecureStateCount,
                                                      &dstrSecureStateData) );
                    if( IsRightSet(dstrSecureStateData) )
                    {
                        ChkDR( DRMCRT_WtoWORD( dstrSecureStateData.pwszString, dstrSecureStateData.cchString, 10, &wSecureStateCount, NULL ) );
                    }

                    pStateDateCountorBinary = (void*)&wSecureStateCount;
                    cStateDateCountorBinary = SIZEOF(pStateDateCountorBinary);
                    break;
                case XMR_EXTENSIBLE_RESTRICTON_STATE_DATE:
#if TEST_SUPPORT_XMR_EXTENSIBLE_RESTRICTON_STATE_DATE
                    {
                        DRM_UINT64 ui64SecureStateDate = DRM_UI64LITERAL(0,0);
                        TOKEN mytoken;
                        ChkDR ( DRM_XML_GetNodeAttribute( &dstrRestriction,
                                                          &g_dstrAttribExPolicySecureStateDate,
                                                          &dstrSecureStateData) );
                        ChkDR(DRM_EXPR_ExtractDateToken( &dstrSecureStateData, &mytoken ));
                        ui64SecureStateDate = mytoken.val.u64DateTime;
                        pStateDateCountorBinary = (void*)&ui64SecureStateDate;
                        cStateDateCountorBinary = SIZEOF(pStateDateCountorBinary);
                    }
#else /* TEST_SUPPORT_XMR_EXTENSIBLE_RESTRICTON_STATE_DATE */
                    ChkDR( DRM_E_NOTIMPL );
#endif /* TEST_SUPPORT_XMR_EXTENSIBLE_RESTRICTON_STATE_DATE */
                    break;
                case XMR_EXTENSIBLE_RESTRICTON_STATE_BYTEARRAY:
                     ChkDR ( DRM_XML_GetNodeAttribute( &dstrRestriction,
                                                       &g_dstrAttribExPolicySecureStateData,
                                                       &dstrSecureStateData) );
                     ChkDR(DRM_B64_DecodeW(&dstrSecureStateData,(DRM_DWORD*)&cStateDateCountorBinary, pBinaryData, 0));
                     pStateDateCountorBinary = (void*)pBinaryData;
                     break;
                default:
                     ChkDR( DRM_E_INVALIDARG );
            }
            ChkDR(DRM_XMR_CreateExtensibleRestriction(&xmrBuilder,
                                                      (DRM_BYTE*)dstrPolicyData.pwszString,
                                                      (DRM_WORD)SIZEOF(dstrPolicyData.pwszString),
                                                      cStateDateCountorBinary,
                                                      bSecureStateType,
                                                      pStateDateCountorBinary,
                                                      cStateDateCountorBinary,
                                                      &pExtendedRestriction,
                                                      &dwExtendedRestrictionSize));

            ChkDR( DRM_XMR_AddUnknownObject( &xmrBuilder, wRestrictionType, TRUE,XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER, wRestrictionFlags, pExtendedRestriction, dwExtendedRestrictionSize));

        }
    }

    if (IsRightSet(pRights->XPolicyBurnExtended))
    {
        DRM_CONST_STRING dstrObjectType = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrObjectFlags = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrRestriction = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrPolicyData = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrSecureStateData = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrSecureStateType = EMPTY_DRM_STRING;
        DRM_BYTE* pExtendedRestriction;
        DRM_WORD wRestrictionType = 0;
        DRM_WORD wRestrictionFlags = 0;
        DRM_DWORD dwExtendedRestrictionSize = 0;
        DRM_BYTE bSecureStateType = 0;
        DRM_WORD wSecureStateCount = 0;
        int iXRestriction = 0;
        void* pStateDateCountorBinary = NULL;
        DRM_WORD cStateDateCountorBinary = 0;
        DRM_BYTE* pBinaryData = NULL;

        while ( DRM_SUCCEEDED( dr = DRM_XML_GetSubNode( &pRights->XPolicyPlayExtended,
                                                    &g_dstrTagExPolicyRestriction,
                                                    NULL,
                                                    NULL,
                                                    iXRestriction++,
                                                    &dstrRestriction,
                                                    NULL,
                                                    1) ) )
        {
            ChkDR ( DRM_XML_GetNodeAttribute( &dstrRestriction,
                                              &g_dstrAttribExPolicyObjectType,
                                              &dstrObjectType) );

            ChkDR ( DRM_XML_GetNodeAttribute( &dstrRestriction,
                                              &g_dstrAttribExPolicyObjectFlags,
                                              &dstrObjectFlags) );
            ChkDR ( DRM_XML_GetNodeAttribute( &dstrRestriction,
                                              &g_dstrAttribExPolicyPolicyData,
                                              &dstrPolicyData) );
            ChkDR ( DRM_XML_GetNodeAttribute( &dstrRestriction,
                                              &g_dstrAttribExPolicySecureStateType,
                                              &dstrSecureStateType) );

            ChkDR( DRMCRT_WtoWORD( dstrObjectType.pwszString, dstrObjectType.cchString, 10, &wRestrictionType, NULL ) );
            ChkDR( DRMCRT_WtoWORD( dstrObjectFlags.pwszString, dstrObjectFlags.cchString, 10, &wRestrictionFlags, NULL ) );
            ChkDR( DRMCRT_WtoDWORD( dstrSecureStateType.pwszString, dstrSecureStateType.cchString, 10, &dwTemp, NULL ) );
            if( IsRightSet(dstrSecureStateType) )
            {
                ChkDR( DRMCRT_WtoDWORD( dstrSecureStateType.pwszString, dstrSecureStateType.cchString, 10, &dwTemp, NULL ) );
                bSecureStateType = (DRM_BYTE)dwTemp;
            }

            switch(bSecureStateType)
            {
                case XMR_EXTENSIBLE_RESTRICTON_STATE_COUNT:
                    ChkDR ( DRM_XML_GetNodeAttribute( &dstrRestriction,
                                                      &g_dstrAttribExPolicySecureStateCount,
                                                      &dstrSecureStateData) );
                    ChkDR( DRMCRT_WtoWORD( dstrSecureStateData.pwszString, dstrSecureStateData.cchString, 10, &wSecureStateCount, NULL ) );
                    if( IsRightSet(dstrSecureStateData) )
                    {
                        pStateDateCountorBinary = (void*)&wSecureStateCount;
                    }
                    cStateDateCountorBinary = SIZEOF(pStateDateCountorBinary);
                    break;
                case XMR_EXTENSIBLE_RESTRICTON_STATE_DATE:
#if TEST_SUPPORT_XMR_EXTENSIBLE_RESTRICTON_STATE_DATE
                    {
                        DRM_UINT64 ui64SecureStateDate = DRM_UI64LITERAL(0,0);
                        TOKEN mytoken;
                        ChkDR ( DRM_XML_GetNodeAttribute( &dstrRestriction,
                                                          &g_dstrAttribExPolicySecureStateDate,
                                                          &dstrSecureStateData) );
                        ChkDR(DRM_EXPR_ExtractDateToken( &dstrSecureStateData, &mytoken ));
                        ui64SecureStateDate = mytoken.val.u64DateTime;
                        pStateDateCountorBinary = (void*)&ui64SecureStateDate;
                        cStateDateCountorBinary = SIZEOF(pStateDateCountorBinary);
                    }
#else /* TEST_SUPPORT_XMR_EXTENSIBLE_RESTRICTON_STATE_DATE */
                    ChkDR( DRM_E_NOTIMPL );
#endif /* TEST_SUPPORT_XMR_EXTENSIBLE_RESTRICTON_STATE_DATE */
                    break;
                case XMR_EXTENSIBLE_RESTRICTON_STATE_BYTEARRAY:
                    ChkDR ( DRM_XML_GetNodeAttribute( &dstrRestriction,
                                                      &g_dstrAttribExPolicySecureStateData,
                                                      &dstrSecureStateData) );
                    ChkDR(DRM_B64_DecodeW(&dstrSecureStateData,(DRM_DWORD*)&cStateDateCountorBinary, pBinaryData, 0));
                    pStateDateCountorBinary = (void*)pBinaryData;
                    break;
                default:
                    ChkDR( DRM_E_INVALIDARG );
             }

             ChkDR(DRM_XMR_CreateExtensibleRestriction(&xmrBuilder,
                                                       (DRM_BYTE*)dstrPolicyData.pwszString,
                                                       (DRM_WORD)SIZEOF(dstrPolicyData.pwszString),
                                                       cStateDateCountorBinary,
                                                       bSecureStateType,
                                                       pStateDateCountorBinary,
                                                       cStateDateCountorBinary,
                                                       &pExtendedRestriction,
                                                       &dwExtendedRestrictionSize));
            ChkDR( DRM_XMR_AddUnknownObject( &xmrBuilder, wRestrictionType, TRUE,XMR_OBJECT_TYPE_ALLOW_PLAYLISTBURN_POLICY_CONTAINER, wRestrictionFlags, pExtendedRestriction, dwExtendedRestrictionSize));

        }
    }

    cbLicense = 0;

    dr = DRM_XMR_FinishLicense(&xmrBuilder, rgbIntegrityKey, _CreateOMACSignature, NULL, &cbLicense);
    if(dr != DRM_E_BUFFERTOOSMALL)
    {
        ChkDR(dr);
    }

    ChkMem(pbLicense = (DRM_BYTE*) (Oem_MemAlloc(CCH_BASE64_EQUIV(cbLicense) + 1)));

    ChkDR(DRM_XMR_FinishLicense(&xmrBuilder, rgbIntegrityKey, _CreateOMACSignature, pbLicense, &cbLicense));

    ChkMem(pList = (XMRLICENSELINKLIST*)Oem_MemAlloc(SIZEOF(XMRLICENSELINKLIST)));
    memset(pList, 0, SIZEOF(XMRLICENSELINKLIST));

    if (IsRightTrue(pRights->DoNotSign))
    {
        cbLicense = cbLicense -XMR_AES_OMAC_SIGNATURE_LENGTH -SIZEOF(DRM_XMR_SIGNATURE);
    }

    dwTemp = 0;
    if( IsRightSet( pRights->TestOptions ) )
    {
        ChkDR( DRMCRT_WtoDWORD( pRights->TestOptions.pwszString,  pRights->TestOptions.cchString, 10, &dwTemp, NULL ) );
    }

    if ( dwTestOptions & OP_LICGEN_TAMPERED_SIGNATURE )
    {
        if (cbLicense > 0)
        {
           pbLicense[cbLicense-1] = ~pbLicense[cbLicense-1]; /* temp the last bit */
        }
    }

    pList->pLicense = pbLicense;
    pList->cbLicense = cbLicense;
    pbLicense = NULL;   /* make sure to not delete it. */


    if (pRights->RevInfoForLicenseResponse.pwszString != NULL)
    {
        ChkDR(
            CopyDRMString(&(pList->szRevInfoForLicenseResponse),
            &(pRights->RevInfoForLicenseResponse)));
    }

    *ppList = pList;
    pList = NULL;   /* make sure to not delete it. */

ErrorExit:
    if (pList)
    {
        TestFreeXMRLicenseList(pList);
    }
    SAFE_OEM_FREE( pbLicense );
    SAFE_OEM_FREE( pbPubKeyECC );
    SAFE_OEM_FREE( pPubKeyModulus );
    SAFE_OEM_FREE( pbClear );
    SAFE_OEM_FREE( pbClearRoot );
    SAFE_OEM_FREE( pbSerialNumber );
    FREEDRMSTRING( dstrLID );

    while (xmrExplicitAnalogVideoContainer.plistOutputConfiguration)
    {
        pxmrExplicitAnalogVideoTemp = xmrExplicitAnalogVideoContainer.plistOutputConfiguration;
        xmrExplicitAnalogVideoContainer.plistOutputConfiguration = pxmrExplicitAnalogVideoTemp->pNext;

        SAFE_OEM_FREE( pxmrExplicitAnalogVideoTemp->Config.pbConfigDataBuffer );
        SAFE_OEM_FREE( pxmrExplicitAnalogVideoTemp->Config.pguidBuffer );
        SAFE_OEM_FREE( pxmrExplicitAnalogVideoTemp );
    }

    while (xmrExplicitDigitalAudioContainer.plistOutputConfiguration)
    {
        pxmrExplicitDigitalAudioTemp = xmrExplicitDigitalAudioContainer.plistOutputConfiguration;
        xmrExplicitDigitalAudioContainer.plistOutputConfiguration = pxmrExplicitDigitalAudioTemp->pNext;

        SAFE_OEM_FREE( pxmrExplicitDigitalAudioTemp->Config.pbConfigDataBuffer );
        SAFE_OEM_FREE( pxmrExplicitDigitalAudioTemp->Config.pguidBuffer );
        SAFE_OEM_FREE( pxmrExplicitDigitalAudioTemp );
    }

    while (pxmrPolicyMetadataList)
    {
        pxmrPolicyMetadataTemp = pxmrPolicyMetadataList;
        pxmrPolicyMetadataList = pxmrPolicyMetadataTemp->pNext;

        SAFE_OEM_FREE( pxmrPolicyMetadataTemp->Config.pbConfigDataBuffer );
        SAFE_OEM_FREE( pxmrPolicyMetadataTemp->Config.pguidBuffer );
        SAFE_OEM_FREE( pxmrPolicyMetadataTemp );
    }

    while (pxmrPlayEnablerGUIDList)
    {
        pxmrPlayEnablerGUIDTemp  = pxmrPlayEnablerGUIDList;
        pxmrPlayEnablerGUIDList = pxmrPlayEnablerGUIDTemp ->pNext;

        SAFE_OEM_FREE( pxmrPlayEnablerGUIDTemp ->Config.pbConfigDataBuffer );
        SAFE_OEM_FREE( pxmrPlayEnablerGUIDTemp ->Config.pguidBuffer );
        SAFE_OEM_FREE( pxmrPlayEnablerGUIDTemp );
    }

    while (pxmrCopyEnablerGUIDList)
    {
        pxmrCopyEnablerGUIDTemp  = pxmrCopyEnablerGUIDList;
        pxmrCopyEnablerGUIDList = pxmrCopyEnablerGUIDTemp ->pNext;

        SAFE_OEM_FREE( pxmrCopyEnablerGUIDTemp ->Config.pbConfigDataBuffer );
        SAFE_OEM_FREE( pxmrCopyEnablerGUIDTemp->Config.pguidBuffer );
        SAFE_OEM_FREE( pxmrCopyEnablerGUIDTemp );
    }

    SAFE_OEM_FREE( xmrInclusionList.pguidBuffer );

    FREEDRMSTRING(dstrRandomKID);
    FREEDRMSTRING(dstrRandomRootKID);

    SAFE_OEM_FREE( pxmrAuxKeyEntries );
    SAFE_OEM_FREE( pdwLocations );
    return dr;
};

DRM_RESULT TestMakeXMRLicense(
    DRM_DWORD                         dwOptions,
    LICENSERIGHTS                    *pRights,
    XMRLICENSELINKLIST              **ppList,
    OptionalTestMakeXMRLicenseParams *pOptionalParams
 )
{
    DRM_RESULT          dr                          = DRM_SUCCESS;
    XMRLICENSELINKLIST *pTempNode                   = NULL;
    XMRLICENSELINKLIST *pPreviousNode               = NULL;
    XMRLICENSELINKLIST *pNextNode                   = NULL;
    DRM_CONST_STRING    dstrKID                     = EMPTY_DRM_STRING;
    LICENSERIGHTS      *pLeaf                       = NULL;
    LICENSERIGHTS      *pNextRight                  = NULL;
    DRM_BOOL            fPassOptionalParamsToLeaf   = FALSE;
    ELicenseType        eLicenseType                = MAKE_SIMPLE_LICENSE;


    pNextRight = pRights;
    while( pNextRight != NULL )
    {
        ChkDR( _MakeXMRLicense( dwOptions, NULL, pNextRight, &pTempNode, &dstrKID, pOptionalParams ) );

        if( pPreviousNode == NULL )
        {
            *ppList = pTempNode;
            pPreviousNode = pTempNode;
        }
        else
        {
            pNextNode = pPreviousNode;
            while(pNextNode->pNext !=  NULL)
            {
                pNextNode = pNextNode->pNext;
            }
            pNextNode->pNext = pTempNode;
        }

        eLicenseType = MAKE_SIMPLE_LICENSE;

        if (IsRightSet(pNextRight->LicenseType))
        {
            if ( TST_UTL_wcsistr(pNextRight->LicenseType.pwszString, dwchRoot) )
            {
                eLicenseType = MAKE_ROOT_LICENSE;
            }
            else if ( TST_UTL_wcsistr(pNextRight->LicenseType.pwszString, dwchLeaf) )
            {
                eLicenseType = MAKE_LEAF_LICENSE;
            }
        }

        if (eLicenseType == MAKE_ROOT_LICENSE && pOptionalParams && pOptionalParams->pdstrRootKIDOverride)
        {
            fPassOptionalParamsToLeaf = TRUE;
        }
        else if (eLicenseType == MAKE_ROOT_LICENSE && pNextRight->pLeaf && pOptionalParams && pOptionalParams->pdstrKIDOverride)
        {
            fPassOptionalParamsToLeaf = TRUE;
        }
        else
        {
            fPassOptionalParamsToLeaf = FALSE;
        }

        pLeaf = pNextRight->pLeaf;
        while( pLeaf != NULL )
        {
            ChkDR( _MakeXMRLicense( dwOptions
                , &dstrKID
                , pLeaf
                , &pTempNode
                , NULL
                , ( fPassOptionalParamsToLeaf ? pOptionalParams : NULL ) ) );

            pNextNode = pPreviousNode;
            while(pNextNode->pNext !=  NULL)
            {
                pNextNode = pNextNode->pNext;
            }
            pNextNode->pNext = pTempNode;

            pLeaf = pLeaf->pNext;
        }
        pNextRight = pNextRight->pNext;
    }

ErrorExit:
    FREEDRMSTRING(dstrKID);
    return dr;
};

