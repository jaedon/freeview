/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdio.h>
#include <drmcommon.h>
#include <drmcmdlnpars.h>
#include <drmhds_impl.h>
#include <drmhds.h>
#include <drmutilities.h>
#include <drmtoolsutils.h>
#include <drmdatastore.h>

#define _CHKHDS_ 
#include "drmhdsimplreal.c"


#define MAXBUFF  4096
#define MAXSLOT  4096
#define MAXNAMESPACENAMELEN 20
#define DRMGUID_LENGTH 16


typedef enum {
    eDumpRaw=0,
    eDumpHex,
    eDumpChar
} EDumpMode;

static DRM_DWORD _gdwFileSize=0;
static DRM_DST_CONTEXT oDST;
const DRM_WORD   wLS_BlockSize = 10240;      /* block size is limited to 64K */
const DRM_WORD   wLS_MaxNumChildren=16;      /* # of child per parent is limited to 64K */
const DRM_DST_BLKNUMTYPE eLS_BlkNumType=eDRM_DSTBLKNUM_DWORD;
const DRM_DWORD _majorVersion=1;
const DRM_DWORD _minorVersion=0;

static DRM_RESULT _StringToBytes( 
    const            DRM_WCHAR  *pwszKey,
                     DRM_DWORD   cch,
    __out_bcount(cb) DRM_BYTE   *pb,
                     DRM_DWORD   cb )
{
    DRM_RESULT   dr     = DRM_SUCCESS;
    DRM_DWORD    i      = 0;
    DRM_DWORD    tmp    = 0;
    DRM_CHAR    *pszKey = NULL;
    DRM_CONST_STRING dstrKey = EMPTY_DRM_STRING;
    
    if( pwszKey[0] == '*' )
    {
        dstrKey.pwszString = pwszKey + 1;
        dstrKey.cchString = cch - 1;
        tmp = cb;
        ChkDR( DRM_B64_DecodeW( &dstrKey, &tmp, pb, 0 ) );
        ChkFAIL( tmp == cb );
    }
    else if( cch == DRM_GUID_STRING_LEN 
        && pwszKey[0] == '{' 
        && pwszKey[DRM_GUID_STRING_LEN-1] == '}'
        && cb == SIZEOF( DRM_GUID ) )
    {
        dstrKey.pwszString = pwszKey;
        dstrKey.cchString = cch;
        ChkDR( DRM_UTL_StringToGuid( &dstrKey, (DRM_GUID*)pb ) );
    }
    else if( pwszKey[0] == '$' )
    {
        cch--;
        pwszKey++;
        ChkBOOL( cch <= cb, DRM_E_BUFFERTOOSMALL );
        ZEROMEM( pb, cb );
        /* Safe to use, input parameter is in ASCII */
        DRM_UTL_DemoteUNICODEtoASCII( pwszKey, (DRM_CHAR*)pb, cch );
    }
    else
    {
        ChkMem( pszKey = (DRM_CHAR *)Oem_MemAlloc( cch+1 ) );
        ZEROMEM(pszKey, cch+1);
      
        /* Safe to use, input parameter is in ASCII */
        DRM_UTL_DemoteUNICODEtoASCII( pwszKey, pszKey, cch );

        ZEROMEM(pb, cb);
        for (i=0; ((i<cch) && (pszKey[i*2]!='\0') && (i<cb)); i++)
        {
            /*
            ** Converts one string HEX digit at location of pszKey[i*2] to DRM_DWORD
            ** if ( sscanf(&pszKey[i*2], "%02X", &tmp) == 1 )
            ** If conversion fails we leave original "0" value in "pb" array.
            ** 2  - means we convert up to 2 digits.
            ** 16 - means in string it is  HEX digit.
            */
            if ( DRM_SUCCEEDED( DRMCRT_AtoDWORD( &pszKey[i*2], 2, 16, &tmp ) ) )
            {
                PUT_BYTE(pb,i,(DRM_BYTE)tmp);
            }
        }
    }
    
ErrorExit:    
    SAFE_OEM_FREE( pszKey );
    return dr;
}
    
/*************************************************************************/
static DRM_RESULT
_PrepareNamespaceID(
    const DRM_WCHAR *pwKey,
    DRM_DWORD dwLen,
    DRM_DST_NAMESPACE *pNSID)
{
    return _StringToBytes( pwKey, dwLen, (DRM_BYTE*)pNSID, DRM_HDS_NSNAME_SIZE );
}


/*************************************************************************/
static DRM_RESULT
_PrepareUniqueKey(
    const DRM_WCHAR *pwKey,
    DRM_DWORD dwLen,
    DRM_DST_KEY *pKey)
{
    return _StringToBytes( pwKey, dwLen, (DRM_BYTE*)pKey, DRM_HDS_UNIQUEKEY_SIZE );
}

/*************************************************************************/
static DRM_RESULT
_PrepareHashKey(
    const DRM_WCHAR *pwKey,
    DRM_DWORD dwLen,
    DRM_DST_KEY *pKey)
{
    return _StringToBytes( pwKey, dwLen, (DRM_BYTE*)pKey, DRM_HDS_HASHKEY_SIZE );
}



/***********************************************************************/
static DRM_VOID
_PrintHexString(
    const DRM_CHAR  *pszText1,
    const DRM_CHAR  *pszText2,
    const DRM_BYTE  *pbBuff,
          DRM_DWORD  cbBuff )
{
    DRM_DWORD nIndex=0;

    printf("%s0x", pszText1);
    for (nIndex=0; nIndex < cbBuff; nIndex++)
    {
        printf("%02X", pbBuff[nIndex]);
    }
    printf("%s", pszText2);
}

/***********************************************************************/
static DRM_VOID
_PrintCharString(
    const DRM_CHAR  *pszText1,
    const DRM_CHAR  *pszText2,
          DRM_BYTE  *pbBuff,
          DRM_DWORD  cbBuff )
{
    DRM_DWORD nIndex=0;
    DRM_DWORD dwRow=0;

    printf("%s", pszText1);
    for (nIndex=0; nIndex < cbBuff; nIndex+=2)
    {
        const DRM_WCHAR *pwch = (DRM_WCHAR*)&pbBuff[nIndex];
        DRM_CHAR         ch   = (DRM_CHAR)*pwch;
        
        if ( pbBuff[nIndex] > 20 )
            printf("%c", ch);
        else
            printf(".");

        ++dwRow;
        if ( dwRow == 80 )
        {
            printf("\n%s", pszText1);
            dwRow = 0;
        }
    }
    printf("%s", pszText2);
}



/***********************************************************************/
static DRM_VOID
_PrintTimestamp(
    const DRM_CHAR   *pszText1,
    const DRM_CHAR   *pszText2,
          DRM_UINT64 *pTimestamp )
{
    DRMSYSTEMTIME st;
    DRMFILETIME   ft;
    printf("%s", pszText1);
    UI64_TO_FILETIME( *pTimestamp, ft );
    Oem_Clock_FileTimeToSystemTime(&ft, &st);
    printf("%04d/%02d/%02d %02d:%02d:%02d.%03d (UTC)",
        st.wYear,
        st.wMonth,
        st.wDay,
        st.wHour,
        st.wMinute,
        st.wSecond,
        st.wMilliseconds);
    printf("%s", pszText2);
}


/***********************************************************************/
static DRM_RESULT _OpenStore(
    DRM_CONST_STRING *pPathname,
    DRM_DST          *pDst )
{
    DRM_RESULT    dr                               = DRM_SUCCESS;
    DRM_WCHAR     wszDeviceStoreName[DRM_MAX_PATH] = {0};
    _HdsContext  *pHDS                             = (_HdsContext*)&oDST;

    ZEROMEM(wszDeviceStoreName, DRM_MAX_PATH*SIZEOF(DRM_WCHAR));
    ChkDR( DRM_STR_StringCchCopyNW(wszDeviceStoreName, NO_OF(wszDeviceStoreName), pPathname->pwszString, pPathname->cchString) );

    printf("Accessing data store: %S\n", wszDeviceStoreName);
    ChkDR( DRM_DST_Init( eDRM_DST_HDS, &oDST, SIZEOF( oDST ), pDst ) );
    dr = DRM_DST_OpenStore( NULL, 
                            ( DRM_BYTE * )wszDeviceStoreName, 
                            NO_OF( wszDeviceStoreName ) * SIZEOF( DRM_WCHAR ),
                            0,
                            pDst );
    if ( DRM_SUCCEEDED(dr) || dr==DRM_E_HASHMISMATCH )
    {
        /* get file size */
        if (!Oem_File_GetSize(pHDS->fp, &_gdwFileSize))
        {
            ChkDR(DRM_E_FILEREADERROR);
        }
    }
    
ErrorExit:

    return dr;
}


static DRM_VOID
_PrintHexFormated(
    const DRM_BYTE  *pbBuff,
          DRM_DWORD  cbBuff )
{
    DRM_DWORD i=0;
    DRM_DWORD j=0;
    
    for (i=0; i<cbBuff; i+=j)
    {
        printf("\t %05d: ", i);
        for (j=0; j<16; j++)
        {
            if ( (j % 4) == 0 )
                printf(" ");

            if ( (i+j) < cbBuff )
                printf("%02X ", pbBuff[i+j]);
            else
                printf("   ");
        }

        printf(" |");
        for (j=0; j<16; j++)
        {
            if ( (i+j) >= cbBuff )
                printf(" ");
            else if ( pbBuff[i+j] <= 20 )
                printf(".");
            else
                printf("%c", pbBuff[i+j]);
        }

        printf("|\n");
    }
}


/*
**
*/
static DRM_VOID
_PrintSlots(
    _HdsContext *pHDS,
    DRM_DWORD dwBlocknum,
    DRM_BYTE *pbPayload,
    DRM_DWORD cbPayload,
    DRM_DWORD dwFreeMem,
    EDumpMode eMode)
{
    DRM_DWORD dwSearch=0;
    DRM_DWORD dwBytesToSkip=0;
    _SlotHeader *pSlotHdr=NULL;
    DRM_DWORD dwFilePos=0;

    DRMASSERT(dwBlocknum>0);
    _HdsBlockNum2FilePos(pHDS, dwBlocknum, &dwFilePos);
    dwFilePos += pHDS->oSRN.dwBlockSize - cbPayload;

    printf("\n");
    for (dwSearch=0; dwSearch<(cbPayload-dwFreeMem); dwSearch+=dwBytesToSkip)
    {
        DRM_DWORD dwAttributes = 0;
        pSlotHdr = (_SlotHeader*)&pbPayload[dwSearch];
        if ( pSlotHdr->dwSlotSize == 0 )
        {
            break;
        }

        printf("\t[File Pos]\t%d\n", dwFilePos+dwSearch);
        printf("\t[Slot size]\t%d\n", pSlotHdr->dwSlotSize);
        _PrintHexString("\t[Hashkey]\t", "\n", pSlotHdr->oHashkey.rgb, DRM_HDS_HASHKEY_SIZE);
        _PrintHexString("\t[Uniquekey]\t", "\n", pSlotHdr->oUniquekey.rgb, DRM_HDS_UNIQUEKEY_SIZE);
        printf("\t[Namespace]\t%s\n", (pSlotHdr->dwAttributes & eSlotIsNamespace)? "Yes" : "No");
        printf("\t[Hidden]\t%s\n", (pSlotHdr->dwAttributes & eSlotIsHidden)? "Yes" : "No");
        dwAttributes = pSlotHdr->dwAttributes & (~(eSlotIsHidden|eSlotIsNamespace));
        printf("\t[Attributes]\t0x%04X\n", dwAttributes);

        if ( (SIZEOF(_SlotHeader)+(pSlotHdr->dwSlotSize)) > cbPayload )
        {
            const DRM_DWORD *pdwDatablocknum = (DRM_DWORD*)(&pbPayload[dwSearch] + SIZEOF(_SlotHeader));
            printf("\t[Datablock#]\t%d\n", *pdwDatablocknum);
            dwBytesToSkip = SIZEOF(_SlotHeader)+SIZEOF(DRM_DWORD);
        }
        else
        {
            printf("\t[Payload]\n");
            if ( eMode == eDumpChar )
                _PrintCharString("\t ", "\n", (&pbPayload[dwSearch] + SIZEOF(_SlotHeader)), pSlotHdr->dwSlotSize);
            else
                _PrintHexFormated((&pbPayload[dwSearch] + SIZEOF(_SlotHeader)), pSlotHdr->dwSlotSize);
            
            dwBytesToSkip = SIZEOF(_SlotHeader)+(DRM_WORD)pSlotHdr->dwSlotSize;
        }

        printf("\n");
    }
}


/*
**
*/
static DRM_BOOL
_ParseChildBlock(
    _HdsContext *pHDS,
    DRM_BYTE *pBlockBuff,
    DRM_DWORD *pdwPayloadBeg,
    DRM_DWORD *pdwPayloadSize,
    DRM_DWORD *pdwNumChildren,
    DRM_DWORD  cbCommonBlockHdr)
{
    DRM_DWORD i=0;
    DRM_DWORD dwPayloadSize = 0;
    DRM_DWORD dwPayloadBeg = 0;
    DRM_DWORD dwSearch=0;
    DRM_DWORD dwBytesToSkip=0;
    DRM_DWORD dwRemainingBytes=0;
    _ChildBlockFileHDR *pChildBlockHeader=(_ChildBlockFileHDR*)(pBlockBuff + cbCommonBlockHdr);


    for (i=DRM_HDS_MAXIMUM_CHILDREN; i>0; i--)
    {
        dwPayloadBeg = cbCommonBlockHdr + (SIZEOF(DRM_DWORD) + SIZEOF(DRM_BYTE) + i * pHDS->oSRN.eBlockNumType);
        dwPayloadSize = pHDS->oSRN.dwBlockSize - dwPayloadBeg;

        /* the following must be _SlotHeader's */
        dwRemainingBytes = 0;
        for (dwSearch=dwPayloadBeg; dwSearch<pHDS->oSRN.dwBlockSize; dwSearch+=dwBytesToSkip)
        {
            _SlotHeader oTmpSlotHdr;

            /* determine end of slots */
            dwRemainingBytes = pHDS->oSRN.dwBlockSize - dwSearch;

            DRM_BYT_CopyBytes((DRM_BYTE*)&oTmpSlotHdr, 0, pBlockBuff, dwSearch, SIZEOF(_SlotHeader));

            /* determine end of slots: _SlotHeader + DWORD is the minimum size */
            if ( dwRemainingBytes < (pChildBlockHeader->nFreeMem + SIZEOF(_SlotHeader) + SIZEOF(DRM_DWORD)) )
            {
                break;
            }

            /* walk the slot */

            /* check if the slot's content is in DATA blocks */
            if ( (SIZEOF(_SlotHeader)+(oTmpSlotHdr.dwSlotSize)) > dwPayloadSize )
            {
                DRM_DWORD dwBN=0;
                DRM_DWORD dwFilePos=0;
                DRM_BYT_CopyBytes((DRM_BYTE*)&dwBN, 0, pBlockBuff, dwSearch + SIZEOF(_SlotHeader), SIZEOF(DRM_DWORD));

                _HdsBlockNum2FilePos(pHDS, dwBN, &dwFilePos);
                if ( dwFilePos < _gdwFileSize )
                {
                    dwBytesToSkip = SIZEOF(_SlotHeader)+SIZEOF(DRM_DWORD);
                    continue;
                }
            }
            
            /* nope, let's see if the block size > 0 */
            else if ( oTmpSlotHdr.dwSlotSize > 0 )
            {
                dwBytesToSkip = SIZEOF(_SlotHeader)+(DRM_DWORD)oTmpSlotHdr.dwSlotSize;
                continue;
            }

            break;  /* forget it. */
        }

        /* stop if we have a match */
        if ( dwRemainingBytes == pChildBlockHeader->nFreeMem )
            break;
    }

    if ( dwRemainingBytes != pChildBlockHeader->nFreeMem )
        return FALSE;

    *pdwPayloadBeg = dwPayloadBeg;
    *pdwNumChildren = i;
    *pdwPayloadSize = dwPayloadSize;
    return TRUE;    
}


/*
**
*/
static DRM_DWORD
_GetChildBlockNum(
    _HdsContext *pHDS,
    DRM_BYTE *pbChildAllocTable,
    DRM_DWORD nIndex)
{
    switch (pHDS->oSRN.eBlockNumType)
    {
        case eDRM_DSTBLKNUM_DWORD:
            return ((DRM_DWORD*)pbChildAllocTable)[nIndex];
            break;
        case eDRM_DSTBLKNUM_WORD:
            {
            DRM_WORD wNum=((DRM_WORD*)pbChildAllocTable)[nIndex];
            return  (DRM_DWORD)wNum;
            }
            break;
        default:    /* eDRMLIC_NUMBLOCKS_BYTE */
            return (DRM_DWORD)(pbChildAllocTable[nIndex]);
            break;
    }
}


/**********************************************************************
** Function:    _HdsTraverseBlocksInPreOrder
** Synopsis:    
** Arguments:   [pCFG] -- 
**              [pCurrBlock] -- 
**              [pNextBlock] -- 
**              [pResult] 
** Returns:     DRM_SUCCESS on success
** Notes:       From the given current block, traverse the tree in PRE ORDER fashion.
** Algorithm:
**    PreOrder(CFG, Current, Next, *pLevels)
**    {
**        *pLevels = 0;
**        if ( Current == NULL )
**        {
**            Next = CFG->Root;
**            return TRUE;
**        }
**        else
**        {
**            Next = LeftmostChildren(Current);
**            if ( Next != NULL )
**            {
**                ++*pLevels;
**                return TRUE;
**            }
**
**            while (TRUE)
**            {
**                Next = NextRightSibling(Current);
**                if ( Next != NULL )
**                    return TRUE;
**
**                Current = Parentof(Current);
**                if (Current == NULL)
**                    return FALSE;  // this's it 
**
**                --*pLevels;
**            }
**        }
**    }
**
***********************************************************************
*/
static DRM_RESULT
_HdsTraverseBlocksInPreOrder(
    IN _NsContext *pCFG,
    IN _CommBlockHDR *pCurrBlock,
    OUT _CommBlockHDR *pNextBlock,
    OUT DRM_LONG *pdwLevelsVisited,
    OUT DRM_BOOL *pResult)
{
    DRM_RESULT dr=DRM_SUCCESS;
    DRM_DWORD nBlockNum=0;
    DRM_DWORD nIndex;
    
    ChkArg( pCFG && pNextBlock && pdwLevelsVisited && pResult);
    *pResult = FALSE;

    if ( pCurrBlock == NULL )   /* start from Root of current Namespace */
        nBlockNum = pCFG->nCfgRootBlockNum;
    else
    {
        /* search index of the current child in parent's ChildAlloc table */
        for (nIndex=0; nIndex<(DRM_DWORD)pCFG->wMaxNumChildren; nIndex++)
        {
            ChkDR(_HdsGetPutChildBlockNum(pCFG, ((_ChildBlockHDR*)pCurrBlock)->File._image.bChildAllocTable,
                nIndex, &nBlockNum, TRUE));
            if ( nBlockNum != 0 )
            {
                ++*pdwLevelsVisited;
                goto PrepareReturn;
            }
        }

        ChkDR(_HdsTraverseNextRightSiblingBlock(pCFG, pCurrBlock, &nBlockNum));
        if ( nBlockNum != 0 )   /* Next sibling found */
            goto PrepareReturn;
        if ( pCurrBlock->File._image.nParentBlockNum == ROOTPARENTNUM )
        {
            *pResult = FALSE;   /* current block is the root, that's it */
            goto ErrorExit;
        }

        nBlockNum = pCurrBlock->File._image.nParentBlockNum;
        while (TRUE)    /* traverse up */
        {
            --*pdwLevelsVisited;
            ChkDR(_HdsLoadBlockHDR(pCFG, nBlockNum, &pNextBlock));
            ChkDR(_HdsTraverseNextRightSiblingBlock(pCFG, pNextBlock, &nBlockNum));

            if ( nBlockNum != 0 )   /* no more sibling found */
                goto PrepareReturn;
            if ( pNextBlock->File._image.nParentBlockNum == ROOTPARENTNUM )
            {
                *pResult = FALSE;   /* current block is the root */
                goto ErrorExit;
            }
            nBlockNum = pNextBlock->File._image.nParentBlockNum;
        }
    }

PrepareReturn:

    ChkDR(_HdsLoadBlockHDR(pCFG, nBlockNum, &pNextBlock));
    *pResult = TRUE;
    
ErrorExit:
    return dr;
}


/**********************************************************************
** Function:    DoNamespaceTreeSketch
** Synopsis:    
** Arguments:   [pDatastore] -- 
**              [pNSID] -- 
** Returns:     DRM_SUCCESS on success
** Notes:       
***********************************************************************
*/
static DRM_RESULT
DoNamespaceTreeSketch(
    DRM_DST *pDatastore,
    DRM_DST_NAMESPACE *pNSID)
{
    DRM_RESULT dr=DRM_SUCCESS;
    _HdsContext *pHDS=(_HdsContext*)(pDatastore->poContextDST);
    DRM_DST_NAMESPACE_CONTEXT NsContext;
    _NsContext *pCFG=NULL;
    DRM_BOOL fResult=FALSE;
    _CommBlockHDR *pCurrBlock=NULL;
    DRM_LONG dwLevelsVisited=0;
    DRM_LONG dwCurrLevel=0;
    DRM_LONG i;

    ChkArg(pDatastore && pNSID);

    /* open the namespace */
    dr = DRM_DST_OpenNamespace( pDatastore, 
                                pNSID, 
                                eDRM_DST_OPEN_EXISTING,
                                0,
                                0,
                                &NsContext );
    if ( dr == DRM_E_DSTNAMESPACENOTFOUND )
        printf("\tNamespace not found.\n");

    ChkDR(dr);
    pCFG = (_NsContext*)&NsContext;
    ChkDR(DRM_HDSIMPL_AllocBlockBuffer(pCFG, eCHILDBLOCK, &pCurrBlock));

    /* find the first node in postorder */
    ChkDR(_HdsTraverseBlocksInPreOrder(pCFG, NULL, pCurrBlock, &dwLevelsVisited, &fResult));
    if ( !fResult )
    {
        printf("\tNo CHILD block found.\n");
        goto ErrorExit;
    }

    printf("\t");
    printf("[Root#%2d]\n", pCurrBlock->nBlockNum);
        
    dwLevelsVisited = 0;
    ChkDR(_HdsTraverseBlocksInPreOrder(pCFG, pCurrBlock, pCurrBlock, &dwLevelsVisited, &fResult));
    while (fResult)
    {
        dwCurrLevel += dwLevelsVisited;

        printf("\t");
        for (i=0; i<dwCurrLevel; i++)
            printf("  ");
        printf(" |\n");

        printf("\t");
        for (i=0; i<dwCurrLevel; i++)
            printf("  ");
        printf(" +");

        if ( pCurrBlock->nBlockNum < 10 )
            printf("[Block#%2d, Level:%d]\n", pCurrBlock->nBlockNum, dwCurrLevel);
        else
            printf("[Block#%3d, Level:%d]\n", pCurrBlock->nBlockNum, dwCurrLevel);
            
        dwLevelsVisited = 0;
        ChkDR(_HdsTraverseBlocksInPreOrder(pCFG, pCurrBlock, pCurrBlock, &dwLevelsVisited, &fResult));
    }


ErrorExit:        

    if ( pCurrBlock )
        _Hds_free(pHDS, pCurrBlock);

    return dr;
}




/**********************************************************************
** Function:    DoDumpNamespaces
** Synopsis:    
** Arguments:   [pDatastore] -- 
** Returns:     DRM_SUCCESS on success
**              DRM_E_ARITHMETIC_OVERFLOW on overflow
** Notes:       
***********************************************************************
*/
static DRM_RESULT DoDumpNamespaces(
    DRM_DST         *pDatastore,
    EDumpMode        eMode)
{
    DRM_RESULT                dr=DRM_SUCCESS;
    _HdsContext              *pHDS=(_HdsContext*)(pDatastore->poContextDST);
    DRM_DST_ENUM_CONTEXT      EnumContext;
    DRM_DST_NAMESPACE_CONTEXT oNsStoreCFG;
    DRM_DWORD                 dwCount=0;

    /* no namespace store exist ==> no namespace found */
    if ( pHDS->oSRN.nNsStoreRootBlockNum == 0 )     
    {
        printf("No Namespace(s) found.\n");
        goto ErrorExit;
    }

    ChkDR(_HdsInitNsContext(pHDS, (_NsContext *)&oNsStoreCFG, NULL, 
        pHDS->oSRN.nNsStoreRootBlockNum, NsStoreNumofChildren, 0, 0));
    ChkDR( DRM_DST_InitSlotEnum( pDatastore, &oNsStoreCFG, NULL, eDRM_DST_LOCKSHARED, &EnumContext ) );

    while (TRUE)
    {
        DRM_DST_SLOT_CONTEXT SlotContext;
        _NSEntry *pNsStoreEntry;
        DRM_DWORD cbEntry;
        DRM_DWORD cbRead=0;

        dr = DRM_DST_SlotEnumNext( pDatastore, &EnumContext, &SlotContext, NULL, NULL, &cbEntry );
        if (dr == DRM_E_NOMORE)
        {
            dr = DRM_SUCCESS;
            break;
        }
        ChkDR(dr);

        dwCount++;
        if (dwCount==1)
        {
            printf("\t[Namespace]\n\n");
        }            

        pNsStoreEntry = (_NSEntry*)Oem_MemAlloc(cbEntry);
        ChkDR( DRM_DST_SlotRead( pDatastore, &SlotContext, cbEntry, (DRM_BYTE *)pNsStoreEntry, &cbRead ) );
        if ( cbRead != cbEntry )
        {
            ChkDR(DRM_E_FILEREADERROR);
        }

        DRMASSERT( cbEntry >= cbEntry - SIZEOF( _NSEntry ) );
        ChkOverflow( cbEntry, cbEntry - SIZEOF( _NSEntry ) );

        if (eMode == eDumpRaw )
        {
            _PrintHexString("\t[Namespace ID]\t", "\n", pNsStoreEntry->bNSName, 
                cbEntry - SIZEOF(_NSEntry));
        }
        else
        {
            printf("\t[Namespace ID]\n");
            _PrintHexFormated(pNsStoreEntry->bNSName, cbEntry - SIZEOF(_NSEntry));
        }
        
        printf("\t[# of Children blocks]\t%d\n", pNsStoreEntry->wMaxNumChildren);
        printf("\t[Root block#]\t%d\n\n", pNsStoreEntry->nNSRBlockNum);
        Oem_MemFree(pNsStoreEntry);

        ChkDR( DRM_DST_CloseSlot( pDatastore, &SlotContext ) );
    }

    if (dwCount==0)
    {
        printf("No Namespace(s) found.\n");
    }
    
ErrorExit:
    return dr;
}


/**********************************************************************
** Function:    DoDumpChildblock
** Synopsis:    
** Arguments:   [pContext] -- 
**              [dwBlocknum] 
** Returns:     DRM_SUCCESS on success
** Notes:       
***********************************************************************
*/
static DRM_RESULT
DoDumpChildblock(
    _HdsContext   *pHDS,
    DRM_DWORD      dwBlocknum,
    DRM_BYTE      *pBlockBuff,
    _CommBlockHDR *pCcommonBlockHdr,
    DRM_DWORD      cbCommonBlockHdr,
    EDumpMode      eMode)
{
    DRM_RESULT dr=DRM_SUCCESS;
    _CommBlockHDR *pChildBlock=NULL; 
    DRM_BOOL fValidBlock=FALSE;
    DRM_DWORD dwPayloadBeg=0;
    DRM_DWORD dwPayloadSize=0;
    DRM_DWORD dwNumChildren=0;
    DRM_DWORD i=0;
    _ChildBlockFileHDR *pChildBlockHeader=(_ChildBlockFileHDR*)(pBlockBuff + cbCommonBlockHdr);

    ChkArg(pHDS && pBlockBuff);
    
    printf("\t[Free payload size] \t%d\n", pChildBlockHeader->nFreeMem);

    fValidBlock = _ParseChildBlock(pHDS, pBlockBuff, &dwPayloadBeg, &dwPayloadSize, &dwNumChildren, cbCommonBlockHdr);
    if ( !fValidBlock )
    {
        printf("\n\t*** Corrupted block ***\n\n");
        printf("\t[Child Block (Header & Payload]\n");
         _PrintHexFormated(&pBlockBuff[cbCommonBlockHdr], pHDS->oSRN.dwBlockSize - cbCommonBlockHdr);    

         return DRM_SUCCESS;
    }

    /* verify the block hash value */
    {
        DRM_RESULT dr2=DRM_SUCCESS;
        _NsContext Cfg;

        ChkDR(_HdsInitNsContext(pHDS, &Cfg, NULL, 0, (DRM_WORD)dwNumChildren, 0, 0));  /* some parameters are misiing, but it's okay here */
        ChkDR(DRM_HDSIMPL_AllocBlockBuffer(&Cfg, eCHILDBLOCK, &pChildBlock));
        dr2 = _HdsLoadBlockHDR(&Cfg, dwBlocknum, &pChildBlock);
        if ( dr2 == DRM_E_HASHMISMATCH )
            printf("\n\t** Block hash value mismatched **\n\n");
    }

    /* dump the child alloctable */
    printf("\t[Child AllocTable]\n\t ");
    for (i=0; i<dwNumChildren; i++)
    {
        printf("%5d ", i);
    }
    printf("\n\t ");
    for (i=0; i<dwNumChildren; i++)
    {
        printf("%5d ", _GetChildBlockNum(pHDS, pChildBlockHeader->bChildAllocTable, i));
    }
    printf("\n");

    /* dump payload */
    printf("\t[Payload]\n");
    switch (eMode )
    {
        case eDumpRaw:
            /* dump the payload in raw hex */
            _PrintHexFormated(&pBlockBuff[dwPayloadBeg], dwPayloadSize);    
            break;

        case eDumpChar:
        case eDumpHex:
            /* parse the payload and dump it per slot in char or hex */
            _PrintSlots(pHDS, dwBlocknum, &pBlockBuff[dwPayloadBeg], dwPayloadSize, 
                pChildBlockHeader->nFreeMem, eMode); 
            break;
        default:
            break;
    }

    printf("\n");

ErrorExit:
    if ( pChildBlock )
        _Hds_free(pHDS, pChildBlock);

    return dr;
}

/**********************************************************************
** Function:    DoDumpDatablock
** Synopsis:    
** Arguments:   [pContext] -- 
**              [dwBlocknum] 
** Returns:     DRM_SUCCESS on success
** Notes:       
***********************************************************************
*/
static DRM_RESULT
DoDumpDatablock(
    IN _HdsContext *pHDS,
    DRM_DWORD       dwBlocknum,
    IN DRM_BYTE    *pBlockBuff,
    _CommBlockHDR  *pCcommonBlockHdr,
    DRM_DWORD       cbCommonBlockHdr,
    IN EDumpMode    eMode,
    OUT DRM_DWORD  *pdwCascadingBlock)
{
    DRM_RESULT dr=DRM_SUCCESS;
    _CommBlockHDR *pDataBlock=NULL; 
    DRM_DWORD dwPayloadBeg=0;
    DRM_DWORD dwPayloadSize=0;
    _DataBlockFileHDR *pDataBlockHeader=(_DataBlockFileHDR*)(pBlockBuff + cbCommonBlockHdr);

    ChkArg(pHDS && pBlockBuff);
    
    printf("\t[Cascading Block#]\t%d\n", pDataBlockHeader->nCascadingBlockNum);

    /* dump payload */
    dwPayloadBeg = cbCommonBlockHdr + SIZEOF(_DataBlockFileHDR);
    dwPayloadSize = pHDS->oSRN.dwBlockSize - dwPayloadBeg;

    /* verify the block hash value */
    {
        DRM_RESULT dr2=DRM_SUCCESS;
        _NsContext Cfg;

        ChkDR(_HdsInitNsContext(pHDS, &Cfg, NULL, 0, 0, 0, 0));  /* some parameters are misiing, but it's okay here */
        ChkDR(DRM_HDSIMPL_AllocBlockBuffer(&Cfg, eDATABLOCK, &pDataBlock));
        dr2 = _HdsLoadBlockHDR(&Cfg, dwBlocknum, &pDataBlock);
        if ( dr2 == DRM_E_HASHMISMATCH )
            printf("\n\t** Block hash value mismatched **\n\n");
    }
    
    printf("\t[Payload]\n");
    switch (eMode )
    {
        case eDumpRaw:
        case eDumpHex:
            /* dump the payload in raw hex */
            _PrintHexFormated(&pBlockBuff[dwPayloadBeg], dwPayloadSize);    
            break;

        case eDumpChar:
            _PrintCharString("\t ", "\n", &pBlockBuff[dwPayloadBeg], dwPayloadSize);
            break;
        default:
            break;
    }

    printf("\n");

    if ( pdwCascadingBlock )
        *pdwCascadingBlock = pDataBlockHeader->nCascadingBlockNum;

ErrorExit:
    if ( pDataBlock )
        _Hds_free(pHDS, pDataBlock);

    return dr;
}


/**********************************************************************
** Function:    DoDumpBlock
** Synopsis:    
** Arguments:   [pContext] -- 
**              [dwBlocknum] 
** Returns:     DRM_SUCCESS on success
** Notes:       
***********************************************************************
*/
static DRM_RESULT
DoDumpBlock(
    DRM_DST_CONTEXT *pContext,
    DRM_DWORD dwBlocknum,
    EDumpMode eMode)
{
    DRM_RESULT dr=DRM_SUCCESS;
    _HdsContext *pHDS = (_HdsContext *)pContext;
    DRM_BYTE *pBlockBuff=NULL;
    DRM_DWORD dwFilePos;
    DRM_DWORD dwNumberBytesIO;
    _CommBlockHDR commonBlockHdr;
    DRM_DWORD     cbCommonBlockHdr=0;

    ChkArg(pContext);
    ChkMem(pBlockBuff = (DRM_BYTE *)Oem_MemAlloc(pHDS->oSRN.dwBlockSize));
    
    /* check block# */
    {
        DRM_DWORD nMaxBlockNum=0;

        ChkDR(_HdsFilePos2BlockNum(pHDS, _gdwFileSize, &nMaxBlockNum));
        if ( dwBlocknum >= nMaxBlockNum )
        {
            printf("\n\t** Invalid block#\n");
            printf("\t** Max block#: %d\n", nMaxBlockNum-1);
            goto ErrorExit;
        }
    }

    /* Load the file block in memory */
    ChkDR(_HdsBlockNum2FilePos(pHDS, dwBlocknum, &dwFilePos));

    if (!Oem_File_SetFilePointer(pHDS->fp, dwFilePos, OEM_FILE_BEGIN, NULL))
    {
        ChkDR(DRM_E_FILESEEKERROR);
    }
    if (!_ReadCommonBlockHeader( pHDS, &commonBlockHdr, pHDS->nImagesize_FileBlockHDR, &cbCommonBlockHdr ))
    {
        ChkDR(DRM_E_FILESEEKERROR);
    }

    /* reset file position to read the whole block at a time */
    if (!Oem_File_SetFilePointer(pHDS->fp, dwFilePos, OEM_FILE_BEGIN, NULL))
    {
        ChkDR(DRM_E_FILESEEKERROR);
    }
    if (!Oem_File_Read(pHDS->fp, pBlockBuff, pHDS->oSRN.dwBlockSize, &dwNumberBytesIO) ||
        dwNumberBytesIO != pHDS->oSRN.dwBlockSize)
    {
        ChkDR(DRM_E_FILEREADERROR);
    }

    printf("\n\t[Block#]\t%d\n", dwBlocknum);
    printf("\t[File pos]\t%d\n", dwFilePos);
    printf("\t[Parent block#]\t%d\n", commonBlockHdr.File._image.nParentBlockNum);
    _PrintTimestamp("\t[Timestamp]\t", "\n", &commonBlockHdr.File._image.ui64TimeStamp);
    _PrintHexString("\t[Hash value]\t", "\n", commonBlockHdr.File._image.bBlockHash, DRM_SHA1_DIGEST_LEN);
    printf("\t[Block type]\t");

    switch(GET_BYTE( &commonBlockHdr.File._image.bBlockType,0))
    {
        case eCHILDBLOCK:
            printf("CHILD\n");
            DoDumpChildblock(pHDS, dwBlocknum, pBlockBuff, &commonBlockHdr, cbCommonBlockHdr, eMode);
            break;
        case eDATABLOCK:
            printf("DATA\n");
            DoDumpDatablock(pHDS, dwBlocknum, pBlockBuff, &commonBlockHdr, cbCommonBlockHdr, eMode, NULL);
            break;
        case eFREEBLOCK:
            printf("FREE\n");
            DoDumpDatablock(pHDS, dwBlocknum, pBlockBuff, &commonBlockHdr, cbCommonBlockHdr, eMode, NULL);
            break;
        default:
            printf("UNKNOWN\n");
            ChkOverflow(pHDS->oSRN.dwBlockSize, pHDS->oSRN.dwBlockSize - cbCommonBlockHdr);
            _PrintHexFormated(&pBlockBuff[cbCommonBlockHdr], pHDS->oSRN.dwBlockSize - cbCommonBlockHdr);    
            break;
    }

ErrorExit:

    if ( pBlockBuff )
        Oem_MemFree(pBlockBuff);

    return dr;
}


/*
**
*/
static const DRM_WCHAR *
_FindChar(
    const DRM_WCHAR *pwszString,
    DRM_WCHAR wch)
{
    while (*pwszString != wch && *pwszString != g_wchNull)
        ++pwszString;

    if (*pwszString == wch)
        return pwszString;
    else
        return NULL;
}
   

/**********************************************************************
** Function:    DoSlotTrace
** Synopsis:    
** Arguments:   [pDatastore] -- 
**              [pbNSID] --
**              [pHashkey] -- 
**              [pUniquekey] -- 
** Returns:     DRM_SUCCESS on success
** Notes:       
***********************************************************************
*/
static DRM_RESULT
DoSlotTrace(
    DRM_DST          *pDatastore,
    DRM_CONST_STRING *pwszParam,
    EDumpMode eMode)
{
    DRM_RESULT dr=DRM_SUCCESS;
    _HdsContext *pHDS=(_HdsContext*)(pDatastore->poContextDST);
    DRM_DST_NAMESPACE_CONTEXT NsContext;
    DRM_DWORD cbData=0;
    DRM_BYTE *pbData = NULL;
    DRM_DST_SLOT_CONTEXT SlotContext;
    DRM_DWORD cbRead=0;
    _SlotContext *pSlotCtx;
    DRM_BYTE *pBlockBuff=NULL;
    DRM_DWORD dwDatablock=0;
    DRM_DST_NAMESPACE NSID;
    DRM_DST_KEY Hashkey;
    DRM_DST_KEY Uniquekey;
    const DRM_WCHAR *pParam = pwszParam->pwszString;
    const DRM_WCHAR *pDelimiter=NULL;
    DRM_DWORD dwNumberBytesIO;

    ChkArg(pDatastore && pwszParam);

    /* parse namespaceID */
    pDelimiter = _FindChar(pParam, g_wchForwardSlash);
    if (pDelimiter==NULL)
    {
        printf("Invalid argument.\n");
        ChkArg(FALSE);
    }
    ChkDR( _PrepareNamespaceID(pParam, (DRM_DWORD)(pDelimiter - pParam), &NSID) );

    /* parse KID */
    pParam = pDelimiter+1;
    pDelimiter = _FindChar(pParam, g_wchForwardSlash);
    if (pDelimiter==NULL)
    {
        printf("Invalid argument.\n");
        ChkArg(FALSE);
    }
    ChkDR( _PrepareHashKey(pParam, (DRM_DWORD)(pDelimiter - pParam), &Hashkey) );

    /* parse KID */
    pParam = pDelimiter+1;
    if ( DRMCRT_wcslen(pParam) == 0 )
    {
        printf("Invalid argument.\n");
        ChkArg(FALSE);
    }
    ChkDR( _PrepareUniqueKey(pParam, (DRM_DWORD)DRMCRT_wcslen(pParam), &Uniquekey) );

    /* trace opening the namespace */
    printf("\n\t[Namespace Hashpath]\n");
    dr = DRM_DST_OpenNamespace( pDatastore,
                                &NSID, 
                                eDRM_DST_OPEN_EXISTING,
                                0,
                                0,
                                &NsContext);
    if ( DRM_FAILED(dr) )
        goto ErrorExit;

    /* trace opening the slot */
    printf("\n\t[Slot Hashpath]\n");
    dr = DRM_DST_OpenSlot( pDatastore, &NsContext, eDRM_DST_OPEN_EXISTING, &Hashkey, 
       &Uniquekey, NULL, &cbData, &SlotContext);
    if ( DRM_FAILED(dr) )
        goto ErrorExit;
    
    /* display the slot */
    pbData = (DRM_BYTE *)Oem_MemAlloc(cbData);
    dr = DRM_DST_SlotRead( pDatastore, &SlotContext, cbData, pbData, &cbRead );
    if ( DRM_FAILED(dr) )
        goto ErrorExit;

    pSlotCtx = (_SlotContext *)&SlotContext;
    printf("\n\t[Slot Info]\n");
    printf("\t[Size]\t\t\t%d\n", cbData);
    printf("\t[Pos in CHILD block]\t%d\n", pSlotCtx->dwSlotPosInBlock);
    if ( pSlotCtx->dwFirstDataBlockNum == 0 )
    {
        printf("\t[Payload]\n");
        if ( eMode == eDumpChar )
            _PrintCharString("\t ", "\n", pbData, cbData);
        else
            _PrintHexFormated(pbData, cbData);
    }
    else
    {
        printf("\t[First DATA Block#]\t%d\n\n", pSlotCtx->dwFirstDataBlockNum);

        /* load and display the content in DATA blocks */
        dwDatablock = pSlotCtx->dwFirstDataBlockNum;
        pBlockBuff = (DRM_BYTE *)Oem_MemAlloc(pHDS->oSRN.dwBlockSize);
        while (dwDatablock)
        {
            DRM_DWORD dwFilePos;
            _CommBlockHDR commonBlockHdr;
            DRM_DWORD     cbCommonBlockHdr=0;
            
            /* Load the file block in memory */
            ChkDR(_HdsBlockNum2FilePos(pHDS, dwDatablock, &dwFilePos));
            if (!Oem_File_SetFilePointer(pHDS->fp, dwFilePos, OEM_FILE_BEGIN, NULL))
            {
                ChkDR(DRM_E_FILESEEKERROR);
            }
            if (!_ReadCommonBlockHeader( pHDS, &commonBlockHdr, pHDS->nImagesize_FileBlockHDR, &cbCommonBlockHdr ))
            {
                ChkDR(DRM_E_FILESEEKERROR);
            }

            /* reset file position to load the whole block at a time */
            if (!Oem_File_SetFilePointer(pHDS->fp, dwFilePos, OEM_FILE_BEGIN, NULL))
            {
                printf("\n** File IO error ** \n");
                ChkDR(DRM_E_FILEREADERROR);
            }
            if (!Oem_File_Read(pHDS->fp, pBlockBuff, pHDS->oSRN.dwBlockSize, &dwNumberBytesIO) ||
                dwNumberBytesIO != pHDS->oSRN.dwBlockSize)
            {
                printf("\n** File IO error ** \n");
                ChkDR(DRM_E_FILEREADERROR);
            }
        
            printf("\t[DATA Block#]\t%d\n", dwDatablock);
            printf("\t[Parent block#]\t%d\n", commonBlockHdr.File._image.nParentBlockNum);
            _PrintTimestamp("\t[Timestamp]\t", "\n", &commonBlockHdr.File._image.ui64TimeStamp);
            _PrintHexString("\t[Hash value]\t", "\n", commonBlockHdr.File._image.bBlockHash, DRM_SHA1_DIGEST_LEN);

            switch(GET_BYTE(&commonBlockHdr.File._image.bBlockType,0))
            {
                case eDATABLOCK:
                    DoDumpDatablock(pHDS, dwDatablock, pBlockBuff, &commonBlockHdr, cbCommonBlockHdr, eMode, &dwDatablock);
                    break;
                case eCHILDBLOCK:
                    printf("\n** Incorrect block type: CHILD **\n** Aborting **\n");
                    goto ErrorExit;
                case eFREEBLOCK:
                    printf("\n** Incorrect block type: FREE **\n** Aborting **\n");
                    goto ErrorExit;
                default:
                    printf("UNKNOWN\n");
                    goto ErrorExit;
            }
        }
    }
    
ErrorExit:
    if ( pBlockBuff )
        Oem_MemFree(pBlockBuff);
    if ( pbData )
        Oem_MemFree(pbData);

    return dr;
}

#if defined(_PREFAST_)
#pragma prefast(push)
#pragma prefast(disable:6262) /* Temporarily disable stack limit check. */
#endif
/**********************************************************************
** Function:    DoDumpSRN
** Synopsis:    
** Arguments:   [pHDS] -- 
**              [dwDumpFreeBlocks] 0 = no, 1=brief, 2=detail
** Returns:     DRM_SUCCESS on success
** Notes:       
***********************************************************************
*/
static DRM_RESULT
DoDumpSRN(
    DRM_CONST_STRING *pStoreName,
    DRM_BOOL fFreeBlockDetail)
{
    DRM_RESULT       dr                               = DRM_SUCCESS;
    DRM_RESULT       dr2                              = DRM_SUCCESS;
    DRM_DST          oDataStore                       = { eDRM_DST_NONE, { 0 }, NULL };
    DRM_DWORD        dwStoreVersion                   = 0;
    DRM_DWORD        dwFreeBlocknum                   = 0;
    DRM_BYTE        *pBlockBuff                       = NULL;
    DRM_WCHAR        wszDeviceStoreName[DRM_MAX_PATH] = {0};
    OEM_FILEHDL      fp                               = OEM_INVALID_HANDLE_VALUE;
    _HdsContext     *pHDS                             = (_HdsContext *)&oDST;
    DRM_DWORD        nBlockNum                        = 0;
    DRM_DWORD        dwNumberOfBytesIO                = 0;

    ZEROMEM(wszDeviceStoreName, SIZEOF(wszDeviceStoreName));
    ChkDR( DRM_STR_StringCchCopyNW(wszDeviceStoreName, NO_OF( wszDeviceStoreName ), pStoreName->pwszString, pStoreName->cchString) );

    /* read the version # */
    fp = Oem_File_Open(NULL, wszDeviceStoreName, OEM_GENERIC_READ,OEM_FILE_SHARE_READ, 
        OEM_OPEN_EXISTING, OEM_ATTRIBUTE_NORMAL);
    if (fp != OEM_INVALID_HANDLE_VALUE)
    {
        if (Oem_File_Read (fp, &dwStoreVersion, SIZEOF (DRM_DWORD), &dwNumberOfBytesIO)
        &&  dwNumberOfBytesIO == SIZEOF (DRM_DWORD))
        {
        }
        else
        {
            ChkDR (DRM_E_FILEREADERROR);
        }
        
        Oem_File_Close (fp);
    }
    else
    {
        ChkDR (DRM_E_FILEREADERROR);
    }
    
    dr2 = _OpenStore(pStoreName, &oDataStore);
    if ( DRM_FAILED(dr2) && dr2!=DRM_E_HASHMISMATCH )
    {
        ChkDR(dr2);
    }

    if ( !fFreeBlockDetail )
    {
        /* dump the details in SRN */
        printf("Super Root Node:\n\n");
        printf("\t[Store version] \t%X\n", dwStoreVersion);

        ChkDR(_HdsFilePos2BlockNum(pHDS, _gdwFileSize, &nBlockNum));
        printf("\t[# of Blocks]\t\t%d\n", nBlockNum-1);
        
        _PrintHexString("\t[Hash value] \t\t", "\n", pHDS->oSRN.bSRNHash, DRM_SHA1_DIGEST_LEN);
        printf("\t[SRN size] \t\t%d\n", pHDS->oSRN.dwSRNSize);
        printf("\t[Block size] \t\t%d\n", pHDS->oSRN.dwBlockSize);
        printf("\t[Block num type] \t");
        switch(pHDS->oSRN.eBlockNumType)
        {
            case eDRM_DSTBLKNUM_WORD:
                printf("WORD (0 - 65,535)\n");
                break;
            case eDRM_DSTBLKNUM_DWORD:
                printf("DWORD (0 - 4,294,967,295)\n");
                break;
            default:
                printf("UNKNOWN\n");
                break;
        }

        _PrintTimestamp("\t[Timestamp] \t\t", "\n", &pHDS->oSRN.ui64Timestamp);
        if ( pHDS->oSRN.nFreeListHead == 0 )
            printf("\t[First free block] \tnone\n");
        else
            printf("\t[First free block] \t%d\n", pHDS->oSRN.nFreeListHead);
            
        printf("\t[NS store block#] \t%d\n\n", pHDS->oSRN.nNsStoreRootBlockNum);
    }

    else if ( fFreeBlockDetail && pHDS->oSRN.nFreeListHead==0 )
    {
        printf("\t[First free block] \tnone\n");
    }
    
    else    
    {
        DRM_DWORD dwNumberBytesIO;
        printf("\t[First free block#]\t%d\n\n", pHDS->oSRN.nFreeListHead);
    
        /* load and display the content in DATA blocks */
        dwFreeBlocknum = pHDS->oSRN.nFreeListHead;
        pBlockBuff = (DRM_BYTE *)Oem_MemAlloc(pHDS->oSRN.dwBlockSize);
        
        while (dwFreeBlocknum)
        {
            DRM_DWORD dwFilePos;
            _CommBlockHDR commonBlockHdr;
            DRM_DWORD     cbCommonBlockHdr=0;
            
            /* Load the file block in memory */
            ChkDR(_HdsBlockNum2FilePos(pHDS, dwFreeBlocknum, &dwFilePos));
            if (!Oem_File_SetFilePointer(pHDS->fp, dwFilePos, OEM_FILE_BEGIN, NULL))
            {
                ChkDR(DRM_E_FILESEEKERROR);
            }
            if (!_ReadCommonBlockHeader( pHDS, &commonBlockHdr, pHDS->nImagesize_FileBlockHDR, &cbCommonBlockHdr ))
            {
                ChkDR(DRM_E_FILESEEKERROR);
            }

            /* reset file position to load the whole block at a time */
            if (!Oem_File_SetFilePointer(pHDS->fp, dwFilePos, OEM_FILE_BEGIN, NULL))
            {
                printf("\n** File IO error ** \n");
                ChkDR(DRM_E_FILESEEKERROR);
            }
            if (!Oem_File_Read(pHDS->fp, pBlockBuff, pHDS->oSRN.dwBlockSize, &dwNumberBytesIO) ||
                dwNumberBytesIO!=pHDS->oSRN.dwBlockSize)
            {
                printf("\n** File IO error ** \n");
                ChkDR(DRM_E_FILEREADERROR);
            }
        
            printf("\t[Free block#]\t%d\n", dwFreeBlocknum);
            printf("\t[Parent block#]\t%d\n", commonBlockHdr.File._image.nParentBlockNum);
            _PrintTimestamp("\t[Timestamp]\t", "\n", &commonBlockHdr.File._image.ui64TimeStamp);
            _PrintHexString("\t[Hash value]\t", "\n", commonBlockHdr.File._image.bBlockHash, DRM_SHA1_DIGEST_LEN);
            
            switch(GET_BYTE(&commonBlockHdr.File._image.bBlockType,0))
            {
                case eFREEBLOCK:
                    DoDumpDatablock(pHDS, dwFreeBlocknum, pBlockBuff, &commonBlockHdr, cbCommonBlockHdr, eDumpRaw, &dwFreeBlocknum);
                    break;
                case eCHILDBLOCK:
                    printf("\n** Incorrect block type: CHILD **\n** Aborting **\n");
                    goto ErrorExit;
                case eDATABLOCK:
                    printf("\n** Incorrect block type: DATA **\n** Aborting **\n");
                    goto ErrorExit;
                default:
                    printf("UNKNOWN\n");
                    goto ErrorExit;
            }
        }
    }

    if ( dr2 == DRM_E_HASHMISMATCH )
    {
        printf("\n\t** Hash value mismatched. **\n\n");
    }
    
    ChkDR(DRM_DST_CloseStore( &oDataStore ));
    ChkDR(DRM_DST_Uninit( &oDataStore ));

ErrorExit:
    if ( pBlockBuff )
        Oem_MemFree(pBlockBuff);
    
    return dr;
}


/*************************************************************************/
void
DoMenu()
{
    printf("usage: chkhds -option -s:DataStore\n\n");
    printf("Options: \n");
    printf("  -?                  display this help menu\n");
    printf("  -r                  display the store root node\n");
    printf("  -f                  display the details of free block list\n");
    printf("  -d[:r|h]            display a directory of namespace(s) found in the store.\n");
    printf("                      Optional parameters:\n");
    printf("                          r: dump payload in raw format.\n");
    printf("                          h: dump payload by slot in hex format.\n");
    printf("  -b:##[,r|h|c]       display content of a specific block.\n");
    printf("                      Optional parameters:\n");
    printf("                          r: dump payload in raw format.\n");
    printf("                          h: dump payload by slot in hex format.\n");
    printf("                          c: dump payload by slot in char format.\n");
    printf("  -t:NamespaceID/Hashkey/Uniquekey[,r|h|c]\n");
    printf("                      Trace how a slot is located along the hash path.\n");
    printf("                      Namespace ID, Hashkey and Uniquekey must be \n");
    printf("                      specified in Hex and separated using a slash(/).\n");
    printf("                      Optional parameters:\n");
    printf("                          r: dump payload in raw format.\n");
    printf("                          h: dump payload by slot in hex format.\n");
    printf("                          c: dump payload by slot in char format.\n");
    printf("  -n:NamespaceID      display the namespace in a tree format. \n");
    printf("                      Namespace ID must be specified in Hex format\n");
    printf("  -s:DataStore        specify the Data Store to use. If not given, \n");
    printf("                      the default file name is used.\n");
}


/*************************************************************************/
static DRM_RESULT
Dispatch(
    DRM_LONG argc, 
    DRM_WCHAR *argv[])
{
    DRM_RESULT dr=DRM_SUCCESS;
    DRM_WCHAR wchOptionChar, wchDataStoreOption;
    DRM_CONST_STRING wszParam, wszDataStore={0,0};
    DRM_CONST_STRING wszExtra, wszExtra2;
    DRM_DST oDataStore = { eDRM_DST_NONE, { 0 }, NULL};
    DRM_DWORD dwBlocknum=0;
    
    if ( argc <= 1 )
    {
        dr = DRM_S_MORE_DATA;
        goto _PrintUsage;
    }

    /* parse the option */
    if (!DRM_CMD_ParseCmdLine(argv[1], &wchOptionChar, &wszParam, &wszExtra))
    {
        dr = DRM_E_INVALID_COMMAND_LINE;
        goto _PrintUsage;
    }

    /* parse the DataStore path */
    if ( argc >= 3 )
    {
        if ( !DRM_CMD_ParseCmdLine(argv[2], &wchDataStoreOption, &wszDataStore, &wszExtra2))
        {
            /* See if it's a default parameter that PK knows how to handle */
            if ( DRM_FAILED( DRM_CMD_TryProcessDefaultOption( argv[2] ) ) )
            {
                dr = DRM_E_INVALID_COMMAND_LINE;
                goto _PrintUsage;
            }
        }            
        else if ( (wchDataStoreOption != WCHAR_CAST('s') && wchDataStoreOption != WCHAR_CAST('S'))
          || wszExtra2.cchString != 0 
          || wszDataStore.pwszString == NULL )
        {
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }
    }

    /* Last and optional */
    if ( argc > 3 )
    {
        /* See if it's a default parameter that PK knows how to handle */
        if ( DRM_FAILED( DRM_CMD_TryProcessDefaultOption( argv[3] ) ) )
        {
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }
    }

    /* dispatch the option */
    switch(DRMCRT_towlower(wchOptionChar))
    {
        /* "-?": Show help text */
        case WCHAR_CAST('?'):
            goto _PrintUsage;

        /* "-r": Dump SRN */
        case WCHAR_CAST('r'):
            if ( wszParam.cchString > 0 )
            {
                dr = DRM_E_INVALID_COMMAND_LINE;
                goto _PrintUsage;
            }
            
            ChkDR(DoDumpSRN(&wszDataStore, FALSE));
            break;

        /* "-f": Dump Free block list */
        case WCHAR_CAST('f'):
            if ( wszParam.cchString > 0 )
            {
                dr = DRM_E_INVALID_COMMAND_LINE;
                goto _PrintUsage;
            }

            ChkDR(DoDumpSRN(&wszDataStore, TRUE));
            break;

        /* "-n[:r|h]": Dump namespaces */
        case WCHAR_CAST('d'):
            ChkDR(_OpenStore(&wszDataStore, &oDataStore));
            if ( wszParam.cchString == 0 )
            {
                ChkDR(DoDumpNamespaces(&oDataStore, eDumpRaw));
            }
            else if ( wszParam.cchString == 1 )
            {
                if ( DRMCRT_towlower(wszParam.pwszString[0]) == WCHAR_CAST('h') )
                {
                    ChkDR(DoDumpNamespaces(&oDataStore, eDumpHex));
                }
                else if ( DRMCRT_towlower(wszParam.pwszString[0]) == WCHAR_CAST('r') )
                {
                    ChkDR(DoDumpNamespaces(&oDataStore, eDumpRaw));
                }
                else
                {
                    dr = DRM_E_INVALID_COMMAND_LINE;
                    goto _PrintUsage;
                }
            }
            else
            {
                dr = DRM_E_INVALID_COMMAND_LINE;
                goto _PrintUsage;
            }

            ChkDR(DRM_DST_CloseStore( &oDataStore ));
            ChkDR(DRM_DST_Uninit( &oDataStore ));
            break;

        /* "-b:##[,r|h|c]": Dump block */
        case WCHAR_CAST('b'):
            if (wszParam.cchString==0)
            {
                dr = DRM_E_INVALID_COMMAND_LINE;
                goto _PrintUsage;
            }

            ChkDR(_OpenStore(&wszDataStore, &oDataStore));
            DRMCRT_wcsntol(wszParam.pwszString, wszParam.cchString, (DRM_LONG *) &dwBlocknum);
            if ( wszExtra.cchString == 0 )
            {
                ChkDR(DoDumpBlock(oDataStore.poContextDST, dwBlocknum, eDumpRaw));
            }
            else if ( wszExtra.cchString == 1 )
            {
                if ( DRMCRT_towlower(wszExtra.pwszString[0]) == WCHAR_CAST('h'))
                {
                    ChkDR(DoDumpBlock(oDataStore.poContextDST, dwBlocknum, eDumpHex));
                }
                else if (DRMCRT_towlower(wszExtra.pwszString[0]) == WCHAR_CAST('r'))
                {
                    ChkDR(DoDumpBlock(oDataStore.poContextDST, dwBlocknum, eDumpRaw));
                }
                else if (DRMCRT_towlower(wszExtra.pwszString[0]) == WCHAR_CAST('c'))
                {
                    ChkDR(DoDumpBlock(oDataStore.poContextDST, dwBlocknum, eDumpChar));
                }
                else
                {
                    dr = DRM_E_INVALID_COMMAND_LINE;
                    goto _PrintUsage;
                }
            }
            else
            {
                dr = DRM_E_INVALID_COMMAND_LINE;
                goto _PrintUsage;
            }

            ChkDR(DRM_DST_CloseStore( &oDataStore ));
            ChkDR(DRM_DST_Uninit( &oDataStore ));
            break;

        /* "-t:NamespaceID/Hashkey/Uniquekey[,r|h|c]": Trace slot */
        case WCHAR_CAST('t'):
            if (wszParam.cchString==0)
            {
                dr = DRM_E_INVALID_COMMAND_LINE;
                goto _PrintUsage;
            }

            ChkDR(_OpenStore(&wszDataStore, &oDataStore));
            if ( wszExtra.cchString == 0 )
            {
                ChkDR(DoSlotTrace(&oDataStore, &wszParam, eDumpRaw));
            }
            else if ( wszExtra.cchString == 1 )
            {
                if (DRMCRT_towlower(wszExtra.pwszString[0]) == WCHAR_CAST('h'))
                {
                    ChkDR(DoSlotTrace(&oDataStore, &wszParam, eDumpHex));
                }
                else if (DRMCRT_towlower(wszExtra.pwszString[0]) == WCHAR_CAST('r'))
                {
                    ChkDR(DoSlotTrace(&oDataStore, &wszParam, eDumpRaw));
                }
                else if (DRMCRT_towlower(wszExtra.pwszString[0]) == WCHAR_CAST('c'))
                {
                    ChkDR(DoSlotTrace(&oDataStore, &wszParam, eDumpChar));
                }
                else
                {
                    dr = DRM_E_INVALID_COMMAND_LINE;
                    goto _PrintUsage;
                }
            }
            else
            {
                dr = DRM_E_INVALID_COMMAND_LINE;
                goto _PrintUsage;
            }

            ChkDR(DRM_DST_CloseStore( &oDataStore ));
            ChkDR(DRM_DST_Uninit( &oDataStore ));
            break;

        /* "-s:NamespaceID": Sketch namespace tree */
        case WCHAR_CAST('n'):
            if (wszParam.cchString==0 || wszExtra.cchString!=0)
            {
                dr = DRM_E_INVALID_COMMAND_LINE;
                goto _PrintUsage;
            }
            else
            {
                DRM_DST_NAMESPACE NSID;

                ChkDR(_OpenStore(&wszDataStore, &oDataStore));
                ChkDR( _PrepareNamespaceID(wszParam.pwszString, wszParam.cchString, &NSID) );
                ChkDR(DoNamespaceTreeSketch(&oDataStore, &NSID));
                ChkDR(DRM_DST_CloseStore( &oDataStore ));
                ChkDR(DRM_DST_Uninit( &oDataStore ));
            }
            break;

        default:
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
    }


ErrorExit:
    return dr;

_PrintUsage:
    DoMenu();
    return dr;
}
#if defined(_PREFAST_)
#pragma prefast(pop)
#endif

/*************************************************************************
*   
*   Console Program Main Entry  Point
*
**************************************************************************/

DRM_LONG DRM_CALL DRM_Main(DRM_LONG argc, DRM_WCHAR* argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    
    PrintCopyrightAndVersionInfo( "Hashed Data Store Checker", _majorVersion, _minorVersion );
    
    DRMSIZEASSERT( SIZEOF(DRM_DST_ENUM_CONTEXT), CALC_MAXENUMCONTEXTLEN );
    DRMSIZEASSERT( SIZEOF( _NsContext ), SIZEOF( DRM_DST_NAMESPACE_CONTEXT ) );

    ChkDR( Dispatch( argc, argv ) );

ErrorExit:
    if ( DRM_SUCCEEDED( dr ) )
    {
        printf( "Success!\r\n" );
    }
    else
    {
        printf( "Failed with error code: 0x%X\r\n.", dr );
    }
    return dr;
}   


