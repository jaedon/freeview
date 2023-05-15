/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

/*
** NOTES:
** - init muti-access in _SlotContext and _ChildBlockHDR using ui64TimeStamp.
** - Delete a Namespace may conflict with an opened Namespace.
*/

/*
** Rules:
** - parent of any FreeBlock is SRN (ROOTPARENTNUM, 0x00)
** - parent of any Namespace Root block is SRN (ROOTPARENTNUM, ie: 0x00)
** - The NamespaceStore's Root block will not be free'd even when it is empty.
** - The Namespace Root block of any Namespaces will not be free'd even when it is empty.
** - When deleting a namespace and its blocks, ALL of the blocks including the Root will be free'd.
** ? Each block header contains the namespace's HashKey that it belongs for error checking.
**
*/

#define DRM_BUILDING_DRMHDSIMPLREAL_C
#include <drmhds.h>
#include <oemmd5.h>
#include <drmstkalloc.h>
#include <drmhdsblockheadercache.h>
#include <drmutilitieslite.h>
#include <drmprofile.h>
#include <drmbytemanip.h>
#include <drmconstants.h>
#include <drmmathsafe.h>
#include <drmlastinclude.h>

PRAGMA_STRICT_GS_PUSH_ON;

ENTER_PK_NAMESPACE_CODE;

/* undefine to print trace messages for lock/unlock functions */
/* #define _TRACELOCK 1 */


#define ISOVERSIZESLOT(slotsize, cfg) \
    ((SIZEOF(_SlotHeader)+(slotsize))>(cfg)->nImagesize_ChildBlockPayload?TRUE:FALSE)

static const DRM_WORD  NsStoreNumofChildren=16;
static const DRM_BYTE  BLOCK_SIGNATURE [] =
{
    0xFF, 0xFE, 0xFD, 0xFC
};

/* Intentionally do NOT null-terminate using double-quote ("") syntax in order to maintain HDS bit-equivalence */
static const DRM_CHAR  DEFAULTNS_HASHKEY1 [] = { 'D', 'R', 'M', '_', 'H', 'D', 'S', '_', 'H', 'A', 'S', 'H', 'K', 'E', 'Y', '1' };
static const DRM_CHAR  DEFAULTNS_HASHKEY2 [] = { 'D', 'R', 'M', '_', 'H', 'D', 'S', '_', 'H', 'A', 'S', 'H', 'K', 'E', 'Y', '2' };

#define DRM_HDS_FILE_VERSION_0   0x000A0000
#define DRM_HDS_FILE_VERSION_1   0x000A0001
#define DRM_HDS_FILE_VERSION_2   0x000A0002
#define HDS_FILEHEADERSIZE          4096


/**********************************************************************
**
** Function :   DRM_HDS_InitializeDST
**
** Synopsis :   Initializes function pointers and other members of the DST
**              structure for use with an HDS implementation.
**
** Arguments :
**      [f_pDst] : Returns the initialized DST structure.
**
** Returns :
**      DRM_SUCCESS      - Success.
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HDS_InitializeDST( __out DRM_DST *f_pDst )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pDst != NULL );

    f_pDst->eImplementationType                          = eDRM_DST_HDS;
    f_pDst->oDataStoreInterface.pfnInit                  = DRM_HDS_Init;
    f_pDst->oDataStoreInterface.pfnUninit                = DRM_HDS_Uninit;
    f_pDst->oDataStoreInterface.pfnCreateStore           = DRM_HDS_CreateStore;
    f_pDst->oDataStoreInterface.pfnOpenStore             = DRM_HDS_OpenStore;
    f_pDst->oDataStoreInterface.pfnCloseStore            = DRM_HDS_CloseStore;
    f_pDst->oDataStoreInterface.pfnCommitStore           = DRM_HDS_CommitStore;
    f_pDst->oDataStoreInterface.pfnCleanupStore          = DRM_HDS_CleanupStore;
    f_pDst->oDataStoreInterface.pfnOpenNamespace         = DRM_HDS_OpenNamespace;
    f_pDst->oDataStoreInterface.pfnDeleteNamespace       = DRM_HDS_DeleteNamespace;
    f_pDst->oDataStoreInterface.pfnCloseNamespace        = DRM_HDS_CloseNamespace;
    f_pDst->oDataStoreInterface.pfnCommitNamespace       = DRM_HDS_CommitNamespace;
    f_pDst->oDataStoreInterface.pfnMakeSlotHint          = DRM_HDS_MakeSlotHint;
    f_pDst->oDataStoreInterface.pfnOpenSlot              = DRM_HDS_OpenSlot;
    f_pDst->oDataStoreInterface.pfnCloseSlot             = DRM_HDS_CloseSlot;
    f_pDst->oDataStoreInterface.pfnDeleteSlot            = DRM_HDS_DeleteSlot;
    f_pDst->oDataStoreInterface.pfnInitSlotEnum          = DRM_HDS_InitSlotEnum;
    f_pDst->oDataStoreInterface.pfnSlotEnumNext          = DRM_HDS_SlotEnumNext;
    f_pDst->oDataStoreInterface.pfnSlotEnumReloadCurrent = DRM_HDS_SlotEnumReloadCurrent;
    f_pDst->oDataStoreInterface.pfnSlotEnumDeleteCurrent = DRM_HDS_SlotEnumDeleteCurrent;
    f_pDst->oDataStoreInterface.pfnBlockScanDelete       = DRM_HDS_BlockScanDelete;
    f_pDst->oDataStoreInterface.pfnSlotResize            = DRM_HDS_SlotResize;
    f_pDst->oDataStoreInterface.pfnSlotSeek              = DRM_HDS_SlotSeek;
    f_pDst->oDataStoreInterface.pfnSlotRead              = DRM_HDS_SlotRead;
    f_pDst->oDataStoreInterface.pfnSlotWrite             = DRM_HDS_SlotWrite;
    f_pDst->oDataStoreInterface.pfnSlotReadPtr           = DRM_HDS_SlotReadPtr;

ErrorExit:
    return dr;
}


/* forward declaration */
static DRM_RESULT _HdsRemoveSlot(
    __in _SlotContext    *pSlotCtx,
    __in _ERemoveSlotMode eMode );

static DRM_RESULT _HdsBlockNum2FilePos(
    __in const _HdsContext *f_pHDS,
    __in       DRM_DWORD    f_nBlkNum,
    __out      DRM_DWORD   *f_pnFilePos );

static DRM_RESULT _HdsPreAlloc(
    __in      _HdsContext *f_pHDS,
    __in      DRM_DWORD    f_PreAllocFileSizeInKB,
    __in      DRM_BOOL     f_fUpToSpecifiedSize,
    __out_opt DRM_DWORD   *f_pnNextBlockNum );



/*
** Validate an _NSEntry from disk
*/
static DRM_NO_INLINE DRM_RESULT _HdsValidateNSEntryFromDisk(
    __in       _HdsContext *f_pHDS,
    __in const _NSEntry    *f_pNsEntry )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD dwFilePos  = 0;
    DRM_DWORD dwFileSize = 0;

    DRMASSERT( f_pHDS     != NULL
        &&     f_pNsEntry != NULL );
    __analysis_assume( f_pHDS != NULL );
    __analysis_assume( f_pNsEntry != NULL );

    /* verify # of child blocks in Alloc table */
    if(  f_pNsEntry->wMaxNumChildren == 0
      || f_pNsEntry->wMaxNumChildren > DRM_HDS_MAXIMUM_CHILDREN
      || f_pNsEntry->nNSRBlockNum    == 0
      || f_pNsEntry->wBloomBytesM    > HDS_CHILDBLOCK_BLOOM_MAX_M
      || f_pNsEntry->wBloomK         > HDS_CHILDBLOCK_BLOOM_MAX_K )
    {
        ChkDR( DRM_E_DST_CORRUPTED );
    }

    /* validate root block# */
    ChkDR(_HdsBlockNum2FilePos(f_pHDS, f_pNsEntry->nNSRBlockNum, &dwFilePos));
    ChkBOOL(Oem_File_GetSize(f_pHDS->fp, &dwFileSize), DRM_E_DST_CORRUPTED);
    ChkBOOL((dwFilePos < dwFileSize), DRM_E_DST_CORRUPTED);

ErrorExit:

    return dr;
}

/*
** Compute the block# from a block's given file pos.
*/
static DRM_RESULT _HdsFilePos2BlockNum(
    __in  const _HdsContext *f_pHDS,
    __in        DRM_DWORD    f_dwFilePos,
    __out       DRM_DWORD   *f_pnBlockNum)
{
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_DWORD  nBlockNum = 0;

    ChkArg(ISVALIDCONTEXT(f_pHDS, eHdsContextSignature));

    DRMASSERT(f_pHDS->fInited == TRUE
           && f_pnBlockNum    != NULL);

    if ( f_dwFilePos < HDS_FILEHEADERSIZE )
    {
        DRMASSERT(FALSE);
    }

    nBlockNum = (f_dwFilePos - HDS_FILEHEADERSIZE) / f_pHDS->oSRN.dwBlockSize;

    ++nBlockNum;    /* any block other than SRN starts from 1 */

    /* mask the block# according the BlockNumType */
    switch(f_pHDS->oSRN.eBlockNumType)
    {
        case eDRM_DSTBLKNUM_WORD:
            nBlockNum &= 0x0000FFFF;
            break;
        default:
            break;
    }

    *f_pnBlockNum = nBlockNum;

ErrorExit:
    return dr;
}

/*
** Compute the file pos of a given block
*/
static DRM_NO_INLINE DRM_RESULT _HdsBlockNum2FilePos(
    __in const _HdsContext *f_pHDS,
    __in       DRM_DWORD    f_nBlkNum,
    __out      DRM_DWORD   *f_pnFilePos )
{
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_DWORD  dwFilePos = 0;

    ChkArg(ISVALIDCONTEXT(f_pHDS, eHdsContextSignature));

    DRMASSERT(f_pHDS->fInited == TRUE
           && f_pnFilePos     != NULL);

    dwFilePos = HDS_FILEHEADERSIZE;
    dwFilePos += (f_nBlkNum-1) * f_pHDS->oSRN.dwBlockSize;
    *f_pnFilePos = dwFilePos;

ErrorExit:
    return dr;
}


/**********************************************************************
** Function:    _HdsLockSRN
** Synopsis:    Accquire lock to the Super Root Node
** Arguments:   [pHDS] --
**              [eMode] --
** Returns:     DRM_SUCCESS on success
** Notes:
***********************************************************************
*/
static DRM_NO_INLINE DRM_RESULT _HdsLockSRN(
    _HdsContext *pHDS,
    DRM_DWORD    eMode)
{
#if DRM_SUPPORT_MULTI_THREADING

    DRM_RESULT dr=DRM_SUCCESS;

#ifdef _TRACELOCK
    TRACE(("LockSRN requested: %c%c\n",
           (eMode & eDRM_DST_LOCKEXCLUSIVE)? 'X' : 'S',
           (eMode & eDRM_DST_LOCKWAIT)? 'W' : ' '));
#endif
    /*
    **  All calls to lock the SRN should wait regardless of the wait mode passed
    **  into the various API.  It makes sense that the caller wants control over
    **  whether to wait for a slot lock since those locks are long lasting (and
    **  under control of the calling application).  But the user doesn't want
    **  control over a frequently grabbed, internal, short-term lock like the
    **  SRN.  Doing so will result is random failures that the caller won't be
    **  able to understand.
    */
    if ( !Oem_File_Lock( pHDS->fp,
                        ((eMode & eDRM_DST_LOCKEXCLUSIVE) != 0),
                        0,
                        HDS_FILEHEADERSIZE,
                        TRUE ) )
    {
        ChkDR(DRM_E_DST_LOCK_FAILED);
    }
#ifdef _TRACELOCK
    TRACE(("LockSRN obtained\n"));
#endif
ErrorExit:
    return dr;

#else
    return DRM_SUCCESS;
#endif
}


/**********************************************************************
** Function:    _HdsUnlockSRN
** Synopsis:    Release lock to a specific store block
** Arguments:   [pHDS] --
** Returns:     DRM_SUCCESS on success
** Notes:
***********************************************************************
*/
static DRM_NO_INLINE DRM_RESULT _HdsUnlockSRN(
    _HdsContext *pHDS)
{
#if DRM_SUPPORT_MULTI_THREADING

    DRM_RESULT dr=DRM_SUCCESS;

    if ( pHDS->fp == OEM_INVALID_HANDLE_VALUE )
    {
        ChkDR(DRM_E_DST_LOCK_FAILED);
    }

#ifdef _TRACELOCK
    TRACE(("\tUnlockSRN requested \n"));
#endif
    if (!Oem_File_Unlock(pHDS->fp, 0, HDS_FILEHEADERSIZE))
    {
        ChkDR(DRM_E_DST_LOCK_FAILED);
    }

ErrorExit:
    return dr;

#else
    return DRM_SUCCESS;
#endif
}

/**********************************************************************
** Function:    _HdsLockStore
** Synopsis:    Accquire lock for the store
** Arguments:   [pHDS] --
**              [fExclusive] --
** Returns:     DRM_SUCCESS on success
** Notes:       It is essentially the same as _HdsLockSlot where we are
**              grabbing a lock on a bigger slot (all the blocks)
***********************************************************************
*/
static DRM_NO_INLINE DRM_RESULT _HdsLockStore(
    _HdsContext *pHDS,
    DRM_DWORD    eMode)
{
#if DRM_SUPPORT_MULTI_THREADING

    DRM_RESULT dr=DRM_SUCCESS;

#ifdef _TRACELOCK
    TRACE(("LockStore requested: %c\n", (eMode & eDRM_DST_LOCKEXCLUSIVE) != 0 ? 'X' : 'S'));
#endif

    /* lock the whole file from the start */
    if (!Oem_File_Lock(
            pHDS->fp,
            ((eMode & eDRM_DST_LOCKEXCLUSIVE) != 0),
            0,
            MAX_SIGNED_TYPE(DRM_LONG),
            ((eMode & eDRM_DST_LOCKWAIT) != 0) ) )
    {
        ChkDR(DRM_E_DST_LOCK_FAILED);
    }
#ifdef _TRACELOCK
    TRACE(("LockStore obtained\n"));
#endif

ErrorExit:
    return dr;
#else
    return DRM_SUCCESS;
#endif
}


/**********************************************************************
** Function:    _HdsUnlockStore
** Synopsis:    Release lock for the store
** Arguments:   [pHDS] --
**
** Returns:     DRM_SUCCESS on success
** Notes:
***********************************************************************
*/
static DRM_RESULT _HdsUnlockStore(
    _HdsContext *pHDS )
{
#if DRM_SUPPORT_MULTI_THREADING
    DRM_RESULT dr=DRM_SUCCESS;

    /* unlock the whole file from the start */
#ifdef _TRACELOCK
    TRACE(("\tUnlockStore requested\n"));
#endif
    if (!Oem_File_Unlock(pHDS->fp, 0, MAX_SIGNED_TYPE(DRM_LONG)))
    {
        ChkDR(DRM_E_DST_LOCK_FAILED);
    }

ErrorExit:
    return dr;
#else
    return DRM_SUCCESS;
#endif
}


/* Lock Order:
** - Full Store: to lock the full HDS exclusively among other processes
** - SRN: to lock the SRN SHARED/EXCLUSIVE among other processes
** - Slot/Block: to lock a slot or block SHARED/EXCLUSIVE among other processes
** - CS:  to guard exclusive access to file handle and local stack allocator
**        among other threads
*/


#if DRM_SUPPORT_MULTI_THREADING

    /*
    ** On mac there is no pragma that can be used to disable warnings about unreferenced
    ** variables, so instead we define a macro to reference each variable at least once
    ** in order to avoid the warnings. This will also work on PC, so we can remove
    ** the pragma to disable the warning on PC
    */
    #define UNREFERENCED_LOCK_VARS \
        UNREFERENCED_PARAMETER( fStoreLocked );\
        UNREFERENCED_PARAMETER( fSRNLocked );\
        UNREFERENCED_PARAMETER( fCSEntered );

    #define DEFINE_LOCK_VARS \
        DRM_BOOL fStoreLocked=FALSE;\
        DRM_BOOL fSRNLocked=FALSE;\
        DRM_BOOL fCSEntered=FALSE;\
        UNREFERENCED_LOCK_VARS

    #define ENTER_HDS_CS(p)  do {\
        Oem_CritSec_Enter(&((p)->oCS));\
        fCSEntered = TRUE;\
    } while(FALSE)

    #define LEAVE_HDS_CS(p)  do {\
        if ( fCSEntered )\
        {\
            Oem_CritSec_Leave(&((p)->oCS));\
            fCSEntered = FALSE;\
        }\
    } while(FALSE)

    #define LOCK_SRN( hds, mode ) do {\
        ChkDR(_HdsLockSRN((hds), (mode)));\
        fSRNLocked = TRUE;\
        } while(FALSE)

    #define UNLOCK_SRN(hds)  do {\
        if (fSRNLocked)\
        {\
            _HdsUnlockSRN((hds));\
            fSRNLocked = FALSE;\
        }\
    } while(FALSE)

    #define LOCK_SRN_AND_ENTER_CS(hds, mode) do{\
        LOCK_SRN( (hds), (mode) );\
        ENTER_HDS_CS((hds));\
        } while(FALSE)

    #define LEAVE_CS_AND_UNLOCK_SRN(hds) do{\
        LEAVE_HDS_CS((hds));\
        UNLOCK_SRN((hds));\
        } while(FALSE)

    #define LOCK_STORE(hds, mode) do{\
        ChkDR( _HdsLockStore( (hds), (mode) ) );\
        fStoreLocked = TRUE;\
        } while(FALSE)

    #define UNLOCK_STORE(hds) do{\
        if ( fStoreLocked )\
        {\
            _HdsUnlockStore((hds));\
            fStoreLocked = FALSE;\
        }\
    } while(FALSE)

#else

    #define DEFINE_LOCK_VARS
    #define ENTER_HDS_CS(p)
    #define LEAVE_HDS_CS(p)
    #define LOCK_SRN( hds, mode )
    #define UNLOCK_SRN(hds)
    #define LOCK_SRN_AND_ENTER_CS(hds, mode)
    #define LEAVE_CS_AND_UNLOCK_SRN(hds)
    #define LOCK_STORE(hds, mode)
    #define UNLOCK_STORE(hds)

#endif




/******************************************************************************
** File Flushing scheme:
*******************************************************************************
** For performance, the following file flushing scheme is applied:
** If _DATASTORE_WRITE_THRU is #define'd, all write operation will be flushed to
** disk immediately. If not, only when expanding file or creating new namespace
** will cause flush. Caller is responsible to call DRM_HDS_CommitStore or
** DRM_HDS_CommitNamespace to flush the store at higher level.
*******************************************************************************
*/



/******************************************************************************
** Store locking scheme:
*******************************************************************************
** Function             Access       CS   Lock (temp)      Lock (Persistent)
*******************************************************************************
** Init                 n/a          n/a  n/a              n/a
** Uninit               n/a          n/a  n/a              n/a
** Commit store         n/a           Y   n/a              n/a
** Cleanup store        file          Y   SRN Exclusive    n/a
** Checkpoint           SRN           N   SRN Exclusive    n/a
** Create store         file          Y   file Exclusive   n/a
** Open store           SRN           Y   SRN Shared       n/a
** Close store          n/a           Y   n/a              n/a
** Commit Namespace     n/a          n/a  n/a              n/a
** Create Namespace     SRN,BLOCKS    Y   SRN Exclusive    n/a
** Open Namespace       SRN,BLOCKS    Y   SRN Shared       n/a
** Close Namespace      n/a          n/a  n/a              n/a
** Delete Namespace     SRN,BLOCKS    Y   Store Exclusive  n/a
** Create/Open Slot     SRN,BLOCKS    Y   SRN Exclusive    Slot Shared/Exclusive
** Open Existing Slot   SRN,BLOCKS    Y   SRN Shared       Slot Shared/Exclusive
** Close Slot           n/a          n/a  n/a              n/a
** Seek Slot            n/a          n/a  n/a              Slot Shared/Exclusive
** Read Slot            n/a          n/a  n/a              Slot Shared/Exclusive
** Write Slot           n/a          n/a  n/a              Slot Shared/Exclusive


** Resize Slot          SRN,BLOCKS    Y   SRN Exclusive    Slot Shared/Exclusive
** Note: Resize slot must first close the slot to be resized, then acquire the SRN exclusively,
**       then re-open the slot. It cannot directly acquire the SRN lock while holding the slot
**       lock as this could cause deadlocks


** Delete Slot          SRN,BLOCKS    Y   SRN Exclusive
** Init enum Slot       SRN,BLOCKS    Y   SRN Shared       Slot Shared/Exclusive
** Enum next Slot       SRN,BLOCKS    Y   SRN Shared       Slot Shared/Exclusive
**
*******************************************************************************
*/



/**********************************************************************
** Function:    _Hds_malloc
** Synopsis:    HDS Component memory allocation redirector
** Arguments:   [pHDS] --
**              [cbBuffer] --
**              [ppbBuffer] --
** Returns:     DRM_SUCCESS on success
** Notes:
***********************************************************************
*/
static DRM_NO_INLINE DRM_RESULT _Hds_malloc(
    __in                           _HdsContext *pHDS,
    __in                           DRM_DWORD    cbBuffer,
    __deref_out_bcount( cbBuffer ) DRM_VOID   **ppbBuffer )
{
    DRM_RESULT dr=DRM_SUCCESS;
    DRMASSERT(pHDS && ppbBuffer && cbBuffer);
    __analysis_assume( pHDS != NULL );
    __analysis_assume( ppbBuffer != NULL );

    ChkDR(DRM_STK_Alloc(&pHDS->oHeap, cbBuffer, ppbBuffer));
    ZEROMEM( (DRM_BYTE*)*ppbBuffer, cbBuffer );

ErrorExit:
    return dr;
}


/**********************************************************************
** Function:    _Hds_free
** Synopsis:    HDS Component memory allocation redirector
** Arguments:   [pHDS] -- Can only be NULL if pBuffer is also NULL
**              [pBuffer] --
** Returns:     DRM_SUCCESS on success
** Notes:
***********************************************************************
*/
static DRM_NO_INLINE DRM_RESULT _Hds_free(
    __in_opt _HdsContext *pHDS,
    __in_opt DRM_VOID    *pBuffer )
{
    if( pBuffer == NULL )
    {
        return DRM_SUCCESS;
    }

    DRMASSERT( pHDS!=NULL );
    __analysis_assume( pHDS != NULL );

    return DRM_STK_Free(&pHDS->oHeap, pBuffer);
}

/**********************************************************************
** Function:    _HdsLockSlot
** Synopsis:    Accquire lock to a specific slot
** Arguments:   [pSlotCtx] --
**              [eMode] --
** Returns:     DRM_SUCCESS on success
** Notes:       The slot header will only be locked for SHARED. THis is
**              necessary for making Search to work.
**
**              THE CALLER MUST ALREADY HOLD THE HDS CRITICAL SECTION!
**              This is because we must release the HDS CS before trying
**              to acquire the slot lock since this is the required
**              locking order to avoid a deadlock where one thread may
**              already have the slot lock and need the CS but another
**              thread has the CS and needs the slot lock.
***********************************************************************
*/
static DRM_NO_INLINE DRM_RESULT _HdsLockSlot(
    _SlotContext *pSlotCtx,
    DRM_DWORD     eMode)
{
#if DRM_SUPPORT_MULTI_THREADING

    DRM_RESULT dr           = DRM_SUCCESS;
    DRM_DWORD  dwFilePos    = 0;
    DRM_DWORD  dwLockSize   = 0;
    DEFINE_LOCK_VARS;

    /* The caller MUST have already acquired the CS */
    fCSEntered = TRUE;

    /* compute filepos of the block */
    ChkDR(_HdsBlockNum2FilePos(pSlotCtx->pNS->pHDS,
        pSlotCtx->pCurrChildBlock->nBlockNum, &dwFilePos));

    /* adjust filepos relative to the block */
    dwFilePos += pSlotCtx->pNS->nChildBlockPayloadPos;
    dwFilePos += pSlotCtx->dwSlotPosInBlock;
    dwFilePos += SIZEOF(_SlotHeader);

    if ( ISOVERSIZESLOT(pSlotCtx->oSlotHeader.dwSlotSize, pSlotCtx->pNS) )
    {
        dwLockSize = SIZEOF(DRM_DWORD);
    }
    else
    {
        dwLockSize = pSlotCtx->oSlotHeader.dwSlotSize;
    }

#ifdef _TRACELOCK
    TRACE(("LockSlot requested:  %c%c - pos %d,  size %d\n",
           (eMode & eDRM_DST_LOCKEXCLUSIVE)? 'X' : 'S',
           (eMode & eDRM_DST_LOCKWAIT)? 'W' : ' ',
           dwFilePos,
           dwLockSize));
#endif

    /* Release the critical section since to preserve the correct locking order we
    ** aren't allowed to hold the CS while waiting on the file lock
    */
    LEAVE_HDS_CS( pSlotCtx->pNS->pHDS );
    /* lock slot content */
    if ( !Oem_File_Lock( pSlotCtx->pNS->pHDS->fp,
                        ((eMode & eDRM_DST_LOCKEXCLUSIVE) != 0),
                        dwFilePos,
                        dwLockSize,
                        ( (eMode & eDRM_DST_LOCKWAIT) != 0 ) ) )
    {
        dr = DRM_E_DST_LOCK_FAILED;
    }
    /* Re-acquire the HDS CS */
    ENTER_HDS_CS( pSlotCtx->pNS->pHDS );
    ChkDR( dr );

    pSlotCtx->eLockMode = (DRM_DST_LOCKMODE)(eMode & eDRM_DST_LOCKMODE_MASK);



#ifdef _TRACELOCK
    TRACE(("LockSlot obtained\n"));
#endif

ErrorExit:
    return dr;

#else

    pSlotCtx->eLockMode = (DRM_DST_LOCKMODE)(eMode & eDRM_DST_LOCKMODE_MASK);
    return DRM_SUCCESS;

#endif
}


/**********************************************************************
** Function:    _HdsUnlockSlot
** Synopsis:    release lock to a specific slot
** Arguments:   [pSlotCtx] --
**              [fExclusive] --
** Returns:     DRM_SUCCESS on success
** Notes:
***********************************************************************
*/
static DRM_NO_INLINE DRM_RESULT _HdsUnlockSlot(
    __in _SlotContext *pSlotCtx)
{
#if DRM_SUPPORT_MULTI_THREADING

    DRM_RESULT dr=DRM_SUCCESS;
    DRM_DWORD  dwFilePos=0;
    DRM_DWORD  dwLockSize=0;

    DRMASSERT(pSlotCtx!=NULL);
    __analysis_assume( pSlotCtx != NULL );

    /* compute filepos of the block */
    ChkDR(_HdsBlockNum2FilePos(pSlotCtx->pNS->pHDS,
        pSlotCtx->pCurrChildBlock->nBlockNum, &dwFilePos));

    /* adjust filepos relative to the block */
    dwFilePos += pSlotCtx->pNS->nChildBlockPayloadPos;
    dwFilePos += pSlotCtx->dwSlotPosInBlock;
    dwFilePos += SIZEOF(_SlotHeader);

    if ( ISOVERSIZESLOT(pSlotCtx->oSlotHeader.dwSlotSize, pSlotCtx->pNS) )
    {
        dwLockSize = SIZEOF(DRM_DWORD);
    }
    else
    {
        dwLockSize = pSlotCtx->oSlotHeader.dwSlotSize;
    }

#ifdef _TRACELOCK
    TRACE(("\tUnlockSlot requested:  pos %d,  size %d\n", dwFilePos, dwLockSize));
#endif

    if ( !Oem_File_Unlock( pSlotCtx->pNS->pHDS->fp,
                          dwFilePos,
                          dwLockSize ) )
    {
        ChkDR(DRM_E_DST_LOCK_FAILED);
    }

ErrorExit:

    return dr;

#else
    return DRM_SUCCESS;
#endif
}


/**********************************************************************
** Function:    _HdsLockBlock2DeleteSlot
** Synopsis:    lock block payload from the given slot to end of block.
**              It is essentially the same as _HdsLockSlot where we are
**              grabbing a lock on a bigger slot.
** Arguments:   [pSlotCtx] --
**              [fExclusive] --
** Returns:     DRM_SUCCESS on success
** Notes:       To remove a slot from a block, it is necessary to lock
**              the slot to end of block (to do adjustments).
**
**              THE CALLER MUST ALREADY HOLD THE HDS CRITICAL SECTION!
**              This is because we must release the HDS CS before trying
**              to acquire the slot lock since this is the required
**              locking order to avoid a deadlock where one thread may
**              already have the slot lock and need the CS but another
**              thread has the CS and needs the slot lock.
***********************************************************************
*/
static DRM_NO_INLINE DRM_RESULT _HdsLockBlock2DeleteSlot(
    _SlotContext *pSlotCtx,
    DRM_DWORD     eMode)
{
#if DRM_SUPPORT_MULTI_THREADING

    DRM_RESULT dr=DRM_SUCCESS;
    DRM_DWORD  dwFilePos=0;
    DRM_DWORD  dwLockSize=0;
    DEFINE_LOCK_VARS;

    /* The caller MUST have already acquired the CS */
    fCSEntered = TRUE;


    /* compute filepos of the block */
    ChkDR(_HdsBlockNum2FilePos(pSlotCtx->pNS->pHDS,
        pSlotCtx->pCurrChildBlock->nBlockNum, &dwFilePos));

    /* adjust filepos relative to the block */
    dwLockSize = dwFilePos + pSlotCtx->pNS->pHDS->oSRN.dwBlockSize;
    dwFilePos += (pSlotCtx->pNS->nChildBlockPayloadPos + pSlotCtx->dwSlotPosInBlock);
    dwLockSize -= dwFilePos;

#ifdef _TRACELOCK
    TRACE(("LockSlot2RestOfBlock requested:  %c%c - pos %d,  size %d\n",
          (eMode & eDRM_DST_LOCKEXCLUSIVE)? 'X' : 'S',
          (eMode & eDRM_DST_LOCKWAIT)? 'W' : ' ',
          dwFilePos,
          dwLockSize));
#endif

    /* Release the critical section since to preserve the correct locking order we
    ** aren't allowed to hold the CS while waiting on the file lock
    */
    LEAVE_HDS_CS( pSlotCtx->pNS->pHDS );

    /* lock it */
    if ( !Oem_File_Lock(
            pSlotCtx->pNS->pHDS->fp,
            ((eMode & eDRM_DST_LOCKEXCLUSIVE) != 0),
            dwFilePos,
            dwLockSize,
            ((eMode & eDRM_DST_LOCKWAIT) != 0) ) )
    {
        dr = DRM_E_DST_LOCK_FAILED;
    }
    /* Re-enter HDS CS */
    ENTER_HDS_CS( pSlotCtx->pNS->pHDS );
    ChkDR( dr );

#ifdef _TRACELOCK
    TRACE(("LockSlot2RestOfBlock obtained\n"));
#endif

ErrorExit:
    return dr;

#else

    return DRM_SUCCESS;

#endif
}


static DRM_NO_INLINE DRM_RESULT _HdsUnlockBlock2DeleteSlot(
    _SlotContext *pSlotCtx)
{
#if DRM_SUPPORT_MULTI_THREADING

    DRM_RESULT dr=DRM_SUCCESS;
    DRM_DWORD  dwFilePos=0;
    DRM_DWORD  dwLockSize=0;
    DRM_BOOL   fResult = FALSE;

    /* compute filepos of the block */
    ChkDR(_HdsBlockNum2FilePos(pSlotCtx->pNS->pHDS,
        pSlotCtx->pCurrChildBlock->nBlockNum, &dwFilePos));

    /* adjust filepos relative to the block */
    dwLockSize = dwFilePos + pSlotCtx->pNS->pHDS->oSRN.dwBlockSize;
    dwFilePos += (pSlotCtx->pNS->nChildBlockPayloadPos+pSlotCtx->dwSlotPosInBlock);
    dwLockSize -= dwFilePos;

#ifdef _TRACELOCK
    TRACE(("_HdsUnlockBlock2DeleteSlot requested:  - pos %d,  size %d\n", dwFilePos,
        dwLockSize));
#endif

    /* unlock it, do not return even if this fail */
    fResult = Oem_File_Unlock(pSlotCtx->pNS->pHDS->fp, dwFilePos, dwLockSize);

    if ( !fResult )
    {
        dr = DRM_E_DST_LOCK_FAILED;
    }

ErrorExit:
    return dr;

#else

    return DRM_SUCCESS;

#endif
}


/**********************************************************************
** Function:    _GetTimeStamp
** Synopsis:    Generate 8-byte timestamp
** Arguments:   [pdwStamp] --
** Returns:     DRM_SUCCESS on success
** Notes:
***********************************************************************
*/
static DRM_UINT64 _GetTimeStamp( DRM_VOID *pOEMContext )
{
    DRM_UINT64  u64      = { 0 };
    DRMFILETIME filetime = { 0, 0 };

    Oem_Clock_GetSystemTimeAsFileTime( pOEMContext, &filetime );

    FILETIME_TO_UI64( filetime, u64 );

    return u64;
}

/*
** generate hash keys for namespace name
*/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_WRITABLE_BUFFER, "Buffer sizes are fixed and the function is static.")
static DRM_NO_INLINE DRM_VOID _GenNamespaceKeys(
    __in                                                  DRM_MD5_CTX *pcontextMD5,
    __in                                            const DRM_CHAR    *pbNamespace,
    __in                                                  DRM_WORD     cbNamespace,
    __out_ecount(__CB_DECL(DRM_HDS_HASHKEY_SIZE))         DRM_BYTE     bNsHashKey   [__CB_DECL(DRM_HDS_HASHKEY_SIZE)],
    __out_ecount(__CB_DECL(DRM_HDS_UNIQUEKEY_SIZE))       DRM_BYTE     bNsUniqueKey [__CB_DECL(DRM_HDS_UNIQUEKEY_SIZE)])
PREFAST_POP
{
    DRM_MD5_Init(pcontextMD5 );
    DRM_MD5_Update(pcontextMD5, pbNamespace, cbNamespace);
    DRM_MD5_Update(pcontextMD5, DEFAULTNS_HASHKEY1, SIZEOF(DEFAULTNS_HASHKEY1) );
    DRM_MD5_Final( pcontextMD5 );

    MEMCPY(bNsHashKey, pcontextMD5->digest, DRM_HDS_HASHKEY_SIZE);

    DRM_MD5_Init(pcontextMD5 );
    DRM_MD5_Update(pcontextMD5, pbNamespace, cbNamespace);
    DRM_MD5_Update(pcontextMD5, DEFAULTNS_HASHKEY2, SIZEOF(DEFAULTNS_HASHKEY2) );
    DRM_MD5_Final( pcontextMD5 );

    MEMCPY(bNsUniqueKey, pcontextMD5->digest, DRM_HDS_HASHKEY_SIZE);
}



/**********************************************************************
** Function:    _GenSRNHash
** Synopsis:    Generate hash of the Super Root Node
** Arguments:   [pLSContext] --
**              [hash] --
** Returns:     DRM_SUCCESS on success
** Notes:
***********************************************************************
*/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_WRITABLE_BUFFER, "Buffer sizes are fixed and the function is static.")
static DRM_NO_INLINE DRM_RESULT _HdsGenSRNHash(
    __in                                  _HdsContext *f_pHDS,
    __out_ecount(__CB_DECL(MD5DIGESTLEN)) DRM_BYTE     f_hash [__CB_DECL(MD5DIGESTLEN)])
PREFAST_POP
{
    DRM_RESULT dr     = DRM_SUCCESS;
    DRM_DWORD  dwByte = 0;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsGenSRNHash);

    ChkArg(ISVALIDCONTEXT(f_pHDS, eHdsContextSignature));

    DRMASSERT(f_pHDS->fInited == TRUE);

    DRM_MD5_Init( &f_pHDS->contextMD5 );

    BYTES_TO_DWORD( dwByte, ((DRM_BYTE*)&(f_pHDS->oSRN.dwSRNSize)) );
    DRM_MD5_Update(&f_pHDS->contextMD5, (DRM_BYTE*)&dwByte, SIZEOF(DRM_DWORD));

    BYTES_TO_DWORD( dwByte, ((DRM_BYTE*)&(f_pHDS->oSRN.dwBlockSize)) );
    DRM_MD5_Update(&f_pHDS->contextMD5, (DRM_BYTE*)&dwByte, SIZEOF(DRM_DWORD));

    BYTES_TO_DWORD( dwByte, ((DRM_BYTE*)&(f_pHDS->oSRN.eBlockNumType)) );
    DRM_MD5_Update(&f_pHDS->contextMD5, (DRM_BYTE*)&dwByte, SIZEOF(DRM_DWORD));

    BYTES_TO_DWORD( dwByte, ((DRM_BYTE*)&(f_pHDS->oSRN.nFreeListHead)) );
    DRM_MD5_Update(&f_pHDS->contextMD5, (DRM_BYTE*)&dwByte, SIZEOF(DRM_DWORD));

    BYTES_TO_DWORD( dwByte, ((DRM_BYTE*)&(f_pHDS->oSRN.nNsStoreRootBlockNum)) );
    DRM_MD5_Update(&f_pHDS->contextMD5, (DRM_BYTE*)&dwByte, SIZEOF(DRM_DWORD));

    DRM_MD5_Final( &f_pHDS->contextMD5 );
    MEMCPY( f_hash, f_pHDS->contextMD5.digest, SIZEOF( f_pHDS->contextMD5.digest ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*
**
*/
static DRM_NO_INLINE DRM_RESULT _HdsInitBlockBuffer(
    __in  _NsContext    *f_pNS,
    __out _CommBlockHDR *f_pBlock,
    __in  DRM_DWORD      f_nParentBlockNum,
    __in  _EBlockType    f_eBlockType)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(ISVALIDCONTEXT(f_pNS, eCfgContextSignature));

    DRMASSERT(f_pNS->fInited == TRUE
           && f_pBlock       != NULL);

    /* set up the block */
    f_pBlock->File._image.nParentBlockNum = f_nParentBlockNum;
    f_pBlock->File._image.ui64TimeStamp   = _GetTimeStamp(f_pNS->pHDS->pOEMContext);

    {
        DRM_BYTE bBlockType = 0;
        ChkDR( DRM_DWordToByte( f_eBlockType, &bBlockType ) );
        PUT_BYTE( &f_pBlock->File._image.bBlockType, 0, bBlockType );
    }

    if (f_eBlockType==eCHILDBLOCK)
    {
        _ChildBlockHDR *pChild = (_ChildBlockHDR*)f_pBlock;

        ChkArg(f_pNS->nImagesize_ChildAllocTable <= SIZEOF( pChild->File._image.bChildAllocTable ));

        pChild->nCurrSlotPos = MAXSLOTPOS;
        pChild->nNextSlotPos = MAXSLOTPOS;
        pChild->nPayloadSize = f_pNS->nImagesize_ChildBlockPayload;
        pChild->File._image.nFreeMem = f_pNS->nImagesize_ChildBlockPayload;
        ZEROMEM(pChild->File._image.bBloomData, SIZEOF(pChild->File._image.bBloomData));
        ZEROMEM(pChild->File._image.bChildAllocTable, f_pNS->nImagesize_ChildAllocTable);
    }
    else   /* eDATABLOCK or eFREEBLOCK */
    {
        _DataBlockHDR *pData = (_DataBlockHDR*)f_pBlock;
        pData->nPayloadSize = f_pNS->pHDS->nImagesize_DataBlockPayload;
        pData->File._image.nCascadingBlockNum = 0;
    }

ErrorExit:
    return dr;
}

/*
** create a buffer big enough for a store block
*/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_HDSIMPL_AllocBlockBuffer(
    __in  _NsContext     *f_pNS,
    __in  _EBlockType     f_eBlockType,
    __out _CommBlockHDR **f_ppBlock )
{
    DRM_RESULT dr          = DRM_SUCCESS;
    DRM_DWORD  cbBlocksize = 0;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC_DRM_HDSIMPL_AllocBlockBuffer);
    DRM_PROFILING_DETAIL_LEVEL(6);

    ChkArg(ISVALIDCONTEXT(f_pNS, eCfgContextSignature));

    DRMASSERT( f_pNS->fInited
            && f_ppBlock != NULL
            && ( f_eBlockType==eCHILDBLOCK || f_eBlockType==eDATABLOCK ) );

    /* allocate buffer for the block */
    if ( f_eBlockType==eCHILDBLOCK )
    {
        cbBlocksize = GetMemsize_ChildBlock(f_pNS);
    }
    else /* eDATABLOCK */
    {
        cbBlocksize = GetMemsize_DataBlock();
    }

    ChkDR(_Hds_malloc(f_pNS->pHDS, cbBlocksize, (DRM_VOID**)f_ppBlock));
    ChkDR(_HdsInitBlockBuffer(f_pNS, *f_ppBlock, 0, f_eBlockType));

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


#define _MEMBER_ENCODE(pb, ib, x) \
    DRM_BYT_CopyBytes(pb, ib, (DRM_BYTE*)&x, 0, SIZEOF(x)); \
    ib += SIZEOF(x)

#define _MEMBER_DECODE(pb, ib, x) \
    DRM_BYT_CopyBytes((DRM_BYTE*)&x, 0, pb, ib, SIZEOF(x)); \
    ib += SIZEOF(x)

static DRM_NO_INLINE DRM_BOOL _WriteSRN(
    _HdsContext *pHDS)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  cbWritten = 0;
    DRM_BOOL   fOK       = FALSE;
    DRM_BYTE  *pbBuffer  = NULL;
    DRM_DWORD  cbBuffer  = 0;
    DRM_DWORD  ibBuffer  = 0;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__WriteSRN);

    FIX_ENDIAN_QWORD( pHDS->oSRN.ui64Timestamp );
    FIX_ENDIAN_DWORD( pHDS->oSRN.dwSRNSize );
    FIX_ENDIAN_DWORD( pHDS->oSRN.dwBlockSize );
    FIX_ENDIAN_DWORD( pHDS->oSRN.nFreeListHead );
    FIX_ENDIAN_DWORD( pHDS->oSRN.nNsStoreRootBlockNum );
    FIX_ENDIAN_DWORD( pHDS->oSRN.nHighestFormattedBlockNum );
    FIX_ENDIAN_DWORD( pHDS->oSRN.eBlockNumType );

    /* this must be updated when the structure of SRN is modified */
    cbBuffer = SIZEOF(DRM_UINT64) + 6 * SIZEOF(DRM_DWORD) + MD5DIGESTLEN;
    dr = _Hds_malloc(pHDS, cbBuffer, (DRM_VOID **) &pbBuffer);
    if ( DRM_FAILED(dr) )
    {
        goto ErrorExit;
    }

    /* encode structure to byte array */
    _MEMBER_ENCODE(pbBuffer, ibBuffer, pHDS->oSRN.bSRNHash);
    _MEMBER_ENCODE(pbBuffer, ibBuffer, pHDS->oSRN.dwSRNSize);
    _MEMBER_ENCODE(pbBuffer, ibBuffer, pHDS->oSRN.dwBlockSize);
    _MEMBER_ENCODE(pbBuffer, ibBuffer, pHDS->oSRN.eBlockNumType);
    _MEMBER_ENCODE(pbBuffer, ibBuffer, pHDS->oSRN.ui64Timestamp);
    _MEMBER_ENCODE(pbBuffer, ibBuffer, pHDS->oSRN.nFreeListHead);
    _MEMBER_ENCODE(pbBuffer, ibBuffer, pHDS->oSRN.nNsStoreRootBlockNum);
    _MEMBER_ENCODE(pbBuffer, ibBuffer, pHDS->oSRN.nHighestFormattedBlockNum);

    /* write to file */
    if ( !Oem_File_Write( pHDS->fp, pbBuffer, cbBuffer, &cbWritten )
      || cbWritten != cbBuffer)
    {
        goto ErrorExit;
    }

    fOK = TRUE;

ErrorExit:
    (void)_Hds_free(pHDS, pbBuffer);

    FIX_ENDIAN_QWORD( pHDS->oSRN.ui64Timestamp );
    FIX_ENDIAN_DWORD( pHDS->oSRN.dwSRNSize );
    FIX_ENDIAN_DWORD( pHDS->oSRN.dwBlockSize );
    FIX_ENDIAN_DWORD( pHDS->oSRN.nFreeListHead );
    FIX_ENDIAN_DWORD( pHDS->oSRN.nNsStoreRootBlockNum );
    FIX_ENDIAN_DWORD( pHDS->oSRN.nHighestFormattedBlockNum );
    FIX_ENDIAN_DWORD( pHDS->oSRN.eBlockNumType );

    DRM_PROFILING_LEAVE_SCOPE;

    return fOK;
}


static DRM_NO_INLINE DRM_BOOL _ReadSRN(
    __in _HdsContext *pHDS )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  cbRead = 0;
    DRM_BOOL   fOK = FALSE;
    DRM_BYTE  *pbBuffer  = NULL;
    DRM_DWORD  cbBuffer  = 0;
    DRM_DWORD  ibBuffer  = 0;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__ReadSRN);

    /* this must be updated when the structure of SRN is modified */
    cbBuffer = SIZEOF(DRM_UINT64) + 6 * SIZEOF(DRM_DWORD) + MD5DIGESTLEN;
    dr = _Hds_malloc(pHDS, cbBuffer, (DRM_VOID **) &pbBuffer);
    if ( DRM_FAILED(dr) )
    {
        goto ErrorExit;
    }

    /* read byte array from file */
    if ( !Oem_File_Read( pHDS->fp, pbBuffer, cbBuffer, &cbRead )
      || cbRead != cbBuffer)
    {
        goto ErrorExit;
    }

    /* decode structure members from byte array */
    _MEMBER_DECODE(pbBuffer, ibBuffer, pHDS->oSRN.bSRNHash);
    _MEMBER_DECODE(pbBuffer, ibBuffer, pHDS->oSRN.dwSRNSize);
    _MEMBER_DECODE(pbBuffer, ibBuffer, pHDS->oSRN.dwBlockSize);
    _MEMBER_DECODE(pbBuffer, ibBuffer, pHDS->oSRN.eBlockNumType);
    _MEMBER_DECODE(pbBuffer, ibBuffer, pHDS->oSRN.ui64Timestamp);
    _MEMBER_DECODE(pbBuffer, ibBuffer, pHDS->oSRN.nFreeListHead);
    _MEMBER_DECODE(pbBuffer, ibBuffer, pHDS->oSRN.nNsStoreRootBlockNum);
    _MEMBER_DECODE(pbBuffer, ibBuffer, pHDS->oSRN.nHighestFormattedBlockNum);

    fOK = TRUE;

ErrorExit:
    (void)_Hds_free(pHDS, pbBuffer);
    FIX_ENDIAN_QWORD( pHDS->oSRN.ui64Timestamp );
    FIX_ENDIAN_DWORD( pHDS->oSRN.dwSRNSize );
    FIX_ENDIAN_DWORD( pHDS->oSRN.dwBlockSize );
    FIX_ENDIAN_DWORD( pHDS->oSRN.nFreeListHead );
    FIX_ENDIAN_DWORD( pHDS->oSRN.nNsStoreRootBlockNum );
    FIX_ENDIAN_DWORD( pHDS->oSRN.nHighestFormattedBlockNum );
    FIX_ENDIAN_DWORD( pHDS->oSRN.eBlockNumType );

    DRM_PROFILING_LEAVE_SCOPE;

    return fOK;
}

static DRM_NO_INLINE DRM_BOOL _WriteCommonBlockHeader(
    __in                _HdsContext   *pHDS,
    __inout             _CommBlockHDR *pBlock,
    __in                DRM_DWORD      cbToWrite,
    __out_ecount_opt(1) DRM_DWORD     *pcbWritten )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  cbWritten = 0;
    DRM_BOOL   fOK = FALSE;
    DRM_DWORD  ibBuffer = 0;
    DRM_DWORD  cbBuffer = 0;
    DRM_BYTE  *pbBuffer = NULL;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__WriteCommonBlockHeader);

    DRMASSERT( pBlock != NULL );
    __analysis_assume( pBlock != NULL );

    FIX_ENDIAN_QWORD( pBlock->File._image.ui64TimeStamp );
    FIX_ENDIAN_DWORD( pBlock->File._image.nParentBlockNum );

    /* this must be updated when the structure of _CommBlockHDR is modified */
    cbBuffer = SIZEOF(DRM_UINT64) + MD5DIGESTLEN + SIZEOF(DRM_DWORD) + 1;
    dr = _Hds_malloc(pHDS, cbBuffer, (DRM_VOID **) &pbBuffer);
    if ( DRM_FAILED(dr) )
    {
        goto ErrorExit;
    }

    /* encode structure to byte array */
    _MEMBER_ENCODE(pbBuffer, ibBuffer, pBlock->File._image.ui64TimeStamp);
    _MEMBER_ENCODE(pbBuffer, ibBuffer, pBlock->File._image.bBlockHash );
    _MEMBER_ENCODE(pbBuffer, ibBuffer, pBlock->File._image.nParentBlockNum );
    DRM_BYT_CopyBytes(pbBuffer, ibBuffer, &(pBlock->File._image.bBlockType), 0, 1);
    ibBuffer += 1;

    /* write byte array to file */
    if ( !Oem_File_Write( pHDS->fp, pbBuffer, cbBuffer, &cbWritten )
      || cbWritten != cbBuffer)
    {
        goto ErrorExit;
    }

    fOK = TRUE;

ErrorExit:
    (void)_Hds_free(pHDS, pbBuffer);
    FIX_ENDIAN_QWORD( pBlock->File._image.ui64TimeStamp );
    FIX_ENDIAN_DWORD( pBlock->File._image.nParentBlockNum );

    if ( pcbWritten!= NULL )
    {
        *pcbWritten = cbWritten;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return fOK;
}

static DRM_NO_INLINE DRM_BOOL _ReadCommonBlockHeader(
    __in                _HdsContext   *pHDS,
    __inout             _CommBlockHDR *pBlock,
    __in                DRM_DWORD      cbToRead,
    __out_ecount_opt(1) DRM_DWORD     *pcbRead )
{
    DRM_RESULT dr  = DRM_SUCCESS;
    DRM_BOOL   fOK = FALSE;
    DRM_DWORD  cbRead   = 0;
    DRM_DWORD  ibBuffer = 0;
    DRM_DWORD  cbBuffer = 0;
    DRM_BYTE  *pbBuffer = NULL;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__ReadCommonBlockHeader);

    DRMASSERT( pBlock != NULL );
    __analysis_assume( pBlock != NULL );

    /* this must be updated when the structure of _CommBlockHDR is modified */
    cbBuffer = SIZEOF(DRM_UINT64) + MD5DIGESTLEN + SIZEOF(DRM_DWORD) + 1;
    dr = _Hds_malloc(pHDS, cbBuffer, (DRM_VOID **) &pbBuffer);
    if ( DRM_FAILED(dr) )
    {
        goto ErrorExit;
    }

    /* read byte array from file */
    if ( !Oem_File_Read( pHDS->fp, pbBuffer, cbBuffer, &cbRead )
      || cbRead != cbBuffer)
    {
        goto ErrorExit;
    }

    /* decode structur member from file */
    _MEMBER_DECODE(pbBuffer, ibBuffer, pBlock->File._image.ui64TimeStamp);
    _MEMBER_DECODE(pbBuffer, ibBuffer, pBlock->File._image.bBlockHash );
    _MEMBER_DECODE(pbBuffer, ibBuffer, pBlock->File._image.nParentBlockNum );
    DRM_BYT_CopyBytes(&(pBlock->File._image.bBlockType), 0, pbBuffer, ibBuffer, 1);

    ibBuffer += 1;

    fOK = TRUE;

ErrorExit:
    (void)_Hds_free(pHDS, pbBuffer);
    FIX_ENDIAN_QWORD( pBlock->File._image.ui64TimeStamp );
    FIX_ENDIAN_DWORD( pBlock->File._image.nParentBlockNum );

    if ( pcbRead!= NULL )
    {
        *pcbRead = cbRead;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return fOK;
}

static DRM_NO_INLINE DRM_BOOL _ReadChildBlockHeader(
    _NsContext     *pNS,
    _ChildBlockHDR *pChild )
{
    DRM_RESULT dr  = DRM_SUCCESS;
    DRM_BOOL   fOK = FALSE;
    DRM_DWORD  cbRead   = 0;
    DRM_DWORD  ibBuffer = 0;
    DRM_BYTE  *pbBuffer = NULL;
    DRM_DWORD  dwSize = 0;
    _HdsContext *pHDS = NULL;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__ReadChildBlockHeader);

    ChkArg( pNS    != NULL
         && pChild != NULL );

    /* this must be updated when the structure of _ChildBlockHDR is modified */

    dwSize = pNS->nImagesize_ChildBlockHDR;
    pHDS = pNS->pHDS;

    dr = _Hds_malloc(pHDS, dwSize, (DRM_VOID **) &pbBuffer);
    if ( DRM_FAILED(dr) )
    {
        goto ErrorExit;
    }

    /* read byte array from file */
    if ( !Oem_File_Read( pHDS->fp, pbBuffer, dwSize, &cbRead )
      || cbRead != dwSize)
    {
        goto ErrorExit;
    }

    /* decode nFreeMem structure member from byte array */
    _MEMBER_DECODE(pbBuffer, ibBuffer, pChild->File._image.nFreeMem);

    /* load the child allocation table */
    ChkOverflow( ibBuffer + pNS->nImagesize_ChildAllocTable, ibBuffer );
    ChkBOOL( dwSize >= ibBuffer + pNS->nImagesize_ChildAllocTable, DRM_E_DST_CORRUPTED );
    DRM_BYT_CopyBytes(&(pChild->File._image.bChildAllocTable),
                      0,
                      pbBuffer,
                      ibBuffer,
                      pNS->nImagesize_ChildAllocTable );
    ibBuffer += pNS->nImagesize_ChildAllocTable;

    /* load the Bloom filter data if present */
    ChkOverflow( ibBuffer + pNS->wBloomBytesM, ibBuffer );
    if( dwSize >= ibBuffer + pNS->wBloomBytesM )
    {
        DRM_BYT_CopyBytes(&(pChild->File._image.bBloomData),
                      0,
                      pbBuffer,
                      ibBuffer,
                      pNS->wBloomBytesM);
        ibBuffer += pNS->wBloomBytesM;
    }

    fOK = TRUE;

ErrorExit:
    (void)_Hds_free(pHDS, pbBuffer);
    FIX_ENDIAN_DWORD( pChild->File._image.nFreeMem );

    DRM_PROFILING_LEAVE_SCOPE;

    return fOK;
}

static DRM_NO_INLINE DRM_BOOL _WriteChildBlockHeader(
    _NsContext     *pNS,
    _ChildBlockHDR *pChild )
{
    DRM_RESULT dr  = DRM_SUCCESS;
    DRM_BOOL   fOK       = FALSE;
    DRM_DWORD  cbWritten = 0;
    DRM_DWORD  ibBuffer  = 0;
    DRM_BYTE  *pbBuffer  = NULL;
    DRM_DWORD  dwSize = 0;
    _HdsContext *pHDS = NULL;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__WriteChildBlockHeader);

    ChkArg( pNS != NULL
        && pChild != NULL );

    dwSize = pNS->nImagesize_ChildBlockHDR;
    pHDS = pNS->pHDS;

    FIX_ENDIAN_DWORD( pChild->File._image.nFreeMem );

    if( dwSize < SIZEOF(pChild->File._image.nFreeMem) )
    {
        goto ErrorExit;
    }

    /* this must be updated when the structure of _ChildBlockHDR is modified */
    ChkDR( _Hds_malloc(pHDS, dwSize, (DRM_VOID **) &pbBuffer) );

    /* encode structure member to byte array */
    _MEMBER_ENCODE(pbBuffer, ibBuffer, pChild->File._image.nFreeMem);

    DRM_BYT_CopyBytes(pbBuffer,
                      ibBuffer,
                      &(pChild->File._image.bChildAllocTable),
                      0,
                      pNS->nImagesize_ChildAllocTable );
    ibBuffer += pNS->nImagesize_ChildAllocTable;

    /* only store the Bloom filter data if there's room */
    if( dwSize - ibBuffer >= pNS->wBloomBytesM )
    {
        DRM_BYT_CopyBytes(pbBuffer,
                          ibBuffer,
                          &(pChild->File._image.bBloomData),
                          0,
                          pNS->wBloomBytesM);
        ibBuffer += pNS->wBloomBytesM;
    }

    /* write byte array to file */
    if ( !Oem_File_Write( pHDS->fp, pbBuffer, dwSize, &cbWritten )
      || cbWritten != dwSize)
    {
        goto ErrorExit;
    }

    fOK = TRUE;

ErrorExit:
    (void)_Hds_free(pHDS, pbBuffer);
    FIX_ENDIAN_DWORD( pChild->File._image.nFreeMem );

    DRM_PROFILING_LEAVE_SCOPE;

    return fOK;
}

static DRM_NO_INLINE DRM_BOOL _ReadDataBlockHeader(
    __in _HdsContext   *pHDS,
    __in _DataBlockHDR *pBlock )
{
    DRM_RESULT dr  = DRM_SUCCESS;
    DRM_BOOL   fOK = FALSE;
    DRM_DWORD  cbRead    = 0;
    DRM_DWORD  ibBuffer  = 0;
    DRM_DWORD  cbBuffer  = 0;
    DRM_BYTE  *pbBuffer  = NULL;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__ReadDataBlockHeader);

    DRMASSERT( pBlock != NULL );
    __analysis_assume( pBlock != NULL );

    /* this must be updated when the structure of _DataBlockHDR is modified */
    cbBuffer = SIZEOF(pBlock->File._image.nCascadingBlockNum);
    dr = _Hds_malloc(pHDS, cbBuffer, (DRM_VOID **) &pbBuffer);
    if ( DRM_FAILED(dr) )
    {
        goto ErrorExit;
    }

    /* read byte array from file */
    if ( !Oem_File_Read( pHDS->fp, pbBuffer, cbBuffer, &cbRead )
      || cbRead != cbBuffer)
    {
        goto ErrorExit;
    }

    /* decode structure member from byte array */
    _MEMBER_DECODE(pbBuffer, ibBuffer, pBlock->File._image.nCascadingBlockNum);

    fOK = TRUE;

ErrorExit:
    (void)_Hds_free(pHDS, pbBuffer);
    FIX_ENDIAN_DWORD( pBlock->File._image.nCascadingBlockNum );

    DRM_PROFILING_LEAVE_SCOPE;

    return fOK;
}

static DRM_NO_INLINE DRM_BOOL _WriteDataBlockHeader(
    _HdsContext   *pHDS,
    _DataBlockHDR *pBlock )
{
    DRM_BOOL   fOK = FALSE;
    DRM_RESULT dr  = DRM_SUCCESS;
    DRM_DWORD  cbWritten = 0;
    DRM_DWORD  ibBuffer  = 0;
    DRM_DWORD  cbBuffer  = 0;
    DRM_BYTE  *pbBuffer  = NULL;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__WriteDataBlockHeader);

    ChkArg( pHDS != NULL
       && pBlock != NULL );

    /* this must be updated when the structure of _DataBlockHDR is modified */
    cbBuffer = SIZEOF(pBlock->File._image.nCascadingBlockNum);
    ChkDR( _Hds_malloc(pHDS, cbBuffer, (DRM_VOID **) &pbBuffer) );

    FIX_ENDIAN_DWORD( pBlock->File._image.nCascadingBlockNum );
    _MEMBER_ENCODE(pbBuffer, ibBuffer, pBlock->File._image.nCascadingBlockNum);
    if ( !Oem_File_Write( pHDS->fp, pbBuffer, cbBuffer, &cbWritten )
      || cbWritten != cbBuffer)
    {
        goto ErrorExit;
    }

    fOK = TRUE;

ErrorExit:
    (void)_Hds_free(pHDS, pbBuffer);
    FIX_ENDIAN_DWORD( pBlock->File._image.nCascadingBlockNum );

    DRM_PROFILING_LEAVE_SCOPE;

    return fOK;
}

/**********************************************************************
** Function:    GetPutBlock
** Synopsis:
** Arguments:   [pHDS] -- pointer to LicenseStore Context structure
**              [nBlkNum] --block number
**              [ppBlock] --buffer to receive the new block, if *ppBlock is NULL,
**                         a new buffer will be allocated from the context.
** Returns:     DRM_SUCCESS on success
** Notes:
***********************************************************************
*/

#define GPBH_GENERIC_ONLY   TRUE
#define GPBH_ANY_HEADER     FALSE

#define GPBH_OPERATION_READ  TRUE
#define GPBH_OPERATION_WRITE  FALSE

static DRM_NO_INLINE DRM_RESULT _HdsGetPutBlockHDR(
    __in    _NsContext     *f_pNS,
    __in    DRM_DWORD       f_nBlkNum,
    __inout _CommBlockHDR **f_ppBlock,
    __in    DRM_BOOL        f_fGenericHeaderOnly,
    __in    DRM_BOOL        f_fGet)
{
    DRM_RESULT   dr        = DRM_SUCCESS;
    DRM_DWORD    dwFilePos = 0;
    _HdsContext *pHDS      = NULL;
    DRM_BOOL     fFound    = FALSE;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsGetPutBlockHDR);

    /* Seek to the begining of the block and read the data. */
    ChkArg(ISVALIDCONTEXT(f_pNS, eCfgContextSignature));

    DRMASSERT(f_pNS->fInited == TRUE
           && f_ppBlock      != NULL
           && f_nBlkNum       > 0);

    pHDS = f_pNS->pHDS;

    if( f_fGet )
    {
        ChkDR( DRM_HBHC_GetBlockHeaderFromCache(
            f_pNS,
            f_nBlkNum,
            f_ppBlock,
            f_fGenericHeaderOnly,
            &fFound ) );
        if( fFound )
        {
            /* Found it, was in the cache. We're done. */
            goto ErrorExit;
        }
    }

    ChkDR(_HdsBlockNum2FilePos(pHDS, f_nBlkNum, &dwFilePos));

    /* read/write the file block header */
    if ( f_fGet )
    {
        _CommBlockHDR tmpBlock;
        DRM_DWORD     dwFileSize = 0;

        /* verify the file position:
        ** file header must be less than or equal to file size - block size
        */
        ChkBOOL(Oem_File_GetSize(pHDS->fp, &dwFileSize), DRM_E_DST_CORRUPTED);
        ChkBOOL((dwFilePos <= (dwFileSize - pHDS->oSRN.dwBlockSize)), DRM_E_DST_CORRUPTED);

        {
            DRM_LONG lPos = 0;
            ChkDR( DRM_DWordToLong( dwFilePos, &lPos ) );
            if( !Oem_File_SetFilePointer(pHDS->fp, lPos, OEM_FILE_BEGIN, NULL) )
            {
                ChkDR(DRM_E_FILE_READ_ERROR);
            }
        }

        if( !_ReadCommonBlockHeader( pHDS, &tmpBlock, pHDS->nImagesize_FileBlockHDR, NULL ) )
        {
            ChkDR(DRM_E_FILE_READ_ERROR);
        }

        /* allocate new buffer if not given */
        if ( *f_ppBlock == NULL )
        {
            ChkDR(DRM_HDSIMPL_AllocBlockBuffer(f_pNS, (_EBlockType)tmpBlock.File._image.bBlockType, f_ppBlock));
        }
        /* verify if the given buffer is the correct type */
        else if ( GET_BYTE( &(*f_ppBlock)->File._image.bBlockType, 0 ) != GET_BYTE( &tmpBlock.File._image.bBlockType, 0 )
               &&  ! ( ( (*f_ppBlock)->File._image.bBlockType == eDATABLOCK
                        && tmpBlock.File._image.bBlockType    == eFREEBLOCK )
                    || ( (*f_ppBlock)->File._image.bBlockType == eFREEBLOCK
                        && tmpBlock.File._image.bBlockType    == eDATABLOCK ) ) )
        {
            ChkDR(DRM_E_DST_BLOCK_MISMATCH);
        }

        /* copy block header to new buffer */
        MEMCPY((*f_ppBlock)->File.bFileImage, tmpBlock.File.bFileImage, pHDS->nImagesize_FileBlockHDR);
    }
    else
    {
        _CommBlockHDR tmpBlock;

        if( f_fGenericHeaderOnly )
        {
            /* Update the block header cache with the new generic header. If they gave a
            ** full header, we'll update the cache further down */
            ChkDR( DRM_HBHC_UpdateBlockHeaderCache( f_nBlkNum,
                                                    f_pNS->pHDS->fp,
                                                    (*f_ppBlock)->File._image.bBlockType,
                                                    (*f_ppBlock)->File.bFileImage,
                                                    SIZEOF( _CommBlockFileHDR ) ) );
        }

        {
            DRM_LONG lPos = 0;
            ChkDR( DRM_DWordToLong( dwFilePos, &lPos ) );
            if( !Oem_File_SetFilePointer(pHDS->fp, lPos, OEM_FILE_BEGIN, NULL) )
            {
                ChkDR(DRM_E_FILE_READ_ERROR);
            }
        }

        MEMCPY(tmpBlock.File.bFileImage, (*f_ppBlock)->File.bFileImage, SIZEOF(tmpBlock.File.bFileImage));

        if(!_WriteCommonBlockHeader(pHDS, &tmpBlock, pHDS->nImagesize_FileBlockHDR, NULL ) )
        {
            ChkDR(DRM_E_FILE_WRITE_ERROR);
        }
    }

    if ( f_fGenericHeaderOnly )
    {
        goto ErrorExit;
    }

    switch ( GET_BYTE( &(*f_ppBlock)->File._image.bBlockType, 0 ) )
    {
        case eCHILDBLOCK:
            {
                /* read/write _ChildBlockHDR */
                _ChildBlockHDR *pChild = (_ChildBlockHDR*)(*f_ppBlock);
                if ( f_fGet )
                {
                    if (!_ReadChildBlockHeader(f_pNS, pChild))
                    {
                        ChkDR(DRM_E_FILE_READ_ERROR);
                    }
                    pChild->nCurrSlotPos = MAXSLOTPOS;
                    pChild->nNextSlotPos = MAXSLOTPOS;
                    pChild->nPayloadSize = f_pNS->nImagesize_ChildBlockPayload;
                }
                else
                {
                    /* Update the block header cache with the new child block header */
                    ChkDR( DRM_HBHC_UpdateBlockHeaderCache( f_nBlkNum,
                                    f_pNS->pHDS->fp,
                                (*f_ppBlock)->File._image.bBlockType,
                                (*f_ppBlock)->File.bFileImage,
                                GetMemsize_ChildBlock(f_pNS) - ( SIZEOF( _CommBlockHDR ) - SIZEOF( _CommBlockFileHDR ) ) ) );

                    if (!_WriteChildBlockHeader(f_pNS, pChild) )
                    {
                        ChkDR(DRM_E_FILE_WRITE_ERROR);
                    }
                }
            }
            break;

        case eDATABLOCK:
        case eFREEBLOCK:
            {
                /* read the HDSData */
                _DataBlockHDR *pData = (_DataBlockHDR*)(*f_ppBlock);
                if ( f_fGet )
                {
                    if (!_ReadDataBlockHeader(pHDS, pData ) )
                    {
                        ChkDR(DRM_E_FILE_READ_ERROR);
                    }
                    pData->nPayloadSize = pHDS->nImagesize_DataBlockPayload;
                }
                else
                {
                    /* Update the block header cache with the new data block header */
                    ChkDR( DRM_HBHC_UpdateBlockHeaderCache( f_nBlkNum,
                                    f_pNS->pHDS->fp,
                                (*f_ppBlock)->File._image.bBlockType,
                                (*f_ppBlock)->File.bFileImage,
                                GetMemsize_DataBlock() - ( SIZEOF( _CommBlockHDR ) - SIZEOF( _CommBlockFileHDR ) ) ) );

                    if (!_WriteDataBlockHeader(pHDS, pData ) )
                    {
                        ChkDR(DRM_E_FILE_WRITE_ERROR);
                    }
                }
            }
            break;

        case eINVALIDBLOCK:
        default:
            ChkDR(DRM_E_DST_CORRUPTED);
    }

ErrorExit:

    if( DRM_SUCCEEDED( dr )
      && f_fGet )
    {
        (*f_ppBlock)->nBlockNum = f_nBlkNum;
        (*f_ppBlock)->pNS       = f_pNS;
    }

#if !_DATASTORE_WRITE_THRU
    if ( DRM_SUCCEEDED(dr) && !f_fGet )
    {
        if ( !Oem_File_FlushBuffers(pHDS->fp) )
        {
            dr = DRM_E_FILE_WRITE_ERROR;
        }
    }
#endif

    DRM_PROFILING_USER_DATA_1(f_nBlkNum);
    DRM_PROFILING_USER_DATA_2((DRM_DWORD)f_fGet);
    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/**********************************************************************
** Function:    _GenBlockHash
** Synopsis:    Generate hash of the given block
** Arguments:   [pLSContext] --
**              [pBlock] --
**              [blockHash] --
** Returns:     DRM_SUCCESS on success
** Notes:
***********************************************************************
*/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_WRITABLE_BUFFER, "Buffer sizes are fixed and the function is static.")
static DRM_NO_INLINE DRM_RESULT _HdsGenBlockHash(
    __in                                        _HdsContext   *f_pHDS,
    __in_opt                              const _NsContext    *f_pNS,
    __in                                        _CommBlockHDR *f_pBlock,
    __out_ecount(__CB_DECL(MD5DIGESTLEN))       DRM_BYTE       f_Hash [__CB_DECL(MD5DIGESTLEN)])
PREFAST_POP
{
    DRM_RESULT dr                                         = DRM_SUCCESS;
    DRM_BYTE   rgbBlockType[__CB_DECL(SIZEOF(DRM_DWORD))] = {0};

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsGenBlockHash);

    if ( GET_BYTE(&f_pBlock->File._image.bBlockType, 0 ) == eCHILDBLOCK )
    {
        ChkArg(ISVALIDCONTEXT(f_pNS, eCfgContextSignature));

        DRMASSERT(f_pNS->fInited == TRUE);
    }

    DRMASSERT(f_pBlock!=NULL);
    DRM_MD5_Init(&f_pHDS->contextMD5 );

    /* _CommBlockHDR header */
    FIX_ENDIAN_DWORD( f_pBlock->File._image.nParentBlockNum );
    FIX_ENDIAN_QWORD( f_pBlock->File._image.ui64TimeStamp );
    PUT_BYTE( rgbBlockType, 0, GET_BYTE( &f_pBlock->File._image.bBlockType, 0 ) );
    DRM_MD5_Update( &f_pHDS->contextMD5, (DRM_BYTE*)&f_pBlock->File._image.nParentBlockNum, SIZEOF(DRM_DWORD));
    DRM_MD5_Update( &f_pHDS->contextMD5, (DRM_BYTE*)&f_pBlock->File._image.ui64TimeStamp,   SIZEOF(DRM_UINT64));
    DRM_MD5_Update( &f_pHDS->contextMD5, rgbBlockType,                           SIZEOF(DRM_DWORD));
    FIX_ENDIAN_DWORD( f_pBlock->File._image.nParentBlockNum );
    FIX_ENDIAN_QWORD( f_pBlock->File._image.ui64TimeStamp );

    switch (GET_BYTE(&f_pBlock->File._image.bBlockType, 0 ))
    {
        case eCHILDBLOCK:
            {
                /* _ChildBlockHDR header */
                _ChildBlockHDR *pChild = (_ChildBlockHDR*)f_pBlock;
                FIX_ENDIAN_DWORD( pChild->File._image.nFreeMem );
                DRM_MD5_Update(&f_pHDS->contextMD5, (DRM_BYTE*)&pChild->File._image.nFreeMem, SIZEOF(DRM_DWORD));
                DRM_MD5_Update(&f_pHDS->contextMD5, pChild->File._image.bChildAllocTable, f_pNS->nImagesize_ChildAllocTable);
                FIX_ENDIAN_DWORD( pChild->File._image.nFreeMem );
            }
            break;
        case eDATABLOCK:
        case eFREEBLOCK:
            {
                /* _DataBlockHDR header */
                _DataBlockHDR *pData = (_DataBlockHDR*)f_pBlock;
                FIX_ENDIAN_DWORD( pData->File._image.nCascadingBlockNum );
                DRM_MD5_Update(&f_pHDS->contextMD5, (DRM_BYTE*)&pData->File._image.nCascadingBlockNum, SIZEOF(DRM_DWORD));
                FIX_ENDIAN_DWORD( pData->File._image.nCascadingBlockNum );
            }
            break;
        default:
            ChkDR(DRM_E_DST_CORRUPTED);
    }
    DRM_MD5_Final( &f_pHDS->contextMD5 );

    MEMCPY( f_Hash, f_pHDS->contextMD5.digest, MD5DIGESTLEN );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**********************************************************************
** Function:    _LoadFileBlock
** Synopsis:    Load/read a specific block from file
** Arguments:   [f_pHDS]    -- pointer to LicenseStore Context structure
**              [f_nBlkNum] -- block number
**              [f_ppBlock] -- buffer to receive the new block
** Returns:     DRM_SUCCESS on success
** Notes:       Caller is responsible to call Oem_MemFree(*ppBlock);
***********************************************************************
*/
static DRM_NO_INLINE DRM_RESULT _HdsLoadBlockHDR(
    __in     _NsContext     *f_pNS,
    __in     DRM_DWORD       f_nBlkNum,
    __inout  _CommBlockHDR **f_ppBlock)
{
    DRM_RESULT dr                                = DRM_SUCCESS;
    DRM_BYTE   rgbHash [__CB_DECL(MD5DIGESTLEN)] = {0};
    DRM_BOOL   fAllocBuff                        = TRUE;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsLoadBlockHDR);

    ChkArg(ISVALIDCONTEXT(f_pNS, eCfgContextSignature));

    DRMASSERT(f_pNS->fInited == TRUE
           && f_ppBlock      != NULL);

    if ( *f_ppBlock != NULL)
    {
        fAllocBuff = FALSE;
    }

    ChkDR(_HdsGetPutBlockHDR(f_pNS, f_nBlkNum, f_ppBlock, GPBH_ANY_HEADER, GPBH_OPERATION_READ));
    ChkDR(_HdsGenBlockHash(f_pNS->pHDS, f_pNS, *f_ppBlock, rgbHash));
    if (MEMCMP((*f_ppBlock)->File._image.bBlockHash, rgbHash, MD5DIGESTLEN ) != 0)
    {
        ChkDR(DRM_E_HASH_MISMATCH);
    }

    if ( DRM_HBHC_IsHdsBlockHeaderCacheSupported() )
    {
        ChkDR( DRM_HBHC_UpdateBlockHeaderCache( f_nBlkNum,
                                                f_pNS->pHDS->fp,
                                                (*f_ppBlock)->File._image.bBlockType,
                                                (*f_ppBlock)->File.bFileImage,
                                                SIZEOF( _CommBlockFileHDR ) ) );
    }

ErrorExit:
    if ( DRM_FAILED(dr) && fAllocBuff && ( *f_ppBlock != NULL ) && ( f_pNS != NULL ) )
    {
        _Hds_free(f_pNS->pHDS, *f_ppBlock);
        *f_ppBlock = NULL;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/**********************************************************************
** Function:    _WriteFileBlock
** Synopsis:    write the given block to file
** Arguments:   [pLSContext] -- pointer to LicenseStore Context structure
**              [pBlock] --buffer containing the file block content.
** Returns:     DRM_SUCCESS on success
** Notes:
***********************************************************************
*/
static DRM_NO_INLINE DRM_RESULT _HdsWriteBlockHDR(
    __inout _CommBlockHDR *pBlock)
{
    DRM_RESULT dr=DRM_SUCCESS;

    ChkBOOL( pBlock != NULL, DRM_E_FILE_WRITE_ERROR );

    pBlock->File._image.ui64TimeStamp = _GetTimeStamp(pBlock->pNS->pHDS->pOEMContext);    /* update timestamp */
    ChkDR(_HdsGenBlockHash(pBlock->pNS->pHDS, pBlock->pNS, pBlock, pBlock->File._image.bBlockHash));
    ChkDR(_HdsGetPutBlockHDR(pBlock->pNS, pBlock->nBlockNum, &pBlock, GPBH_ANY_HEADER, GPBH_OPERATION_WRITE));

ErrorExit:
    return dr;
}

/*
** Copy block
*/
static DRM_NO_INLINE DRM_RESULT _HdsCopyBlockBuffer(
    __out _CommBlockHDR *pDestBlock,
    __in  _CommBlockHDR *pSrcBlock)
{
    DRM_RESULT dr=DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsCopyBlockBuffer);

    if (pDestBlock == pSrcBlock)
    {
       goto ErrorExit;
    }

    DRMASSERT( pDestBlock != NULL
            && pSrcBlock  != NULL
            && GET_BYTE( &pDestBlock->File._image.bBlockType, 0 ) == GET_BYTE( &pSrcBlock->File._image.bBlockType, 0 ) );

    if ( GET_BYTE( &pSrcBlock->File._image.bBlockType, 0 ) == eCHILDBLOCK )
    {
        MEMCPY((DRM_BYTE*)pDestBlock,
               (DRM_BYTE*)pSrcBlock,
               GetMemsize_ChildBlock(pSrcBlock->pNS));
    }
    else if ( GET_BYTE( &pSrcBlock->File._image.bBlockType, 0 ) == eDATABLOCK )
    {
        const _DataBlockHDR *pDataSrc = (const _DataBlockHDR*)pSrcBlock;
        _DataBlockHDR *pDataDest = (_DataBlockHDR*)pDestBlock;


        MEMCPY(pDestBlock, pSrcBlock, SIZEOF(_CommBlockHDR));
        pDataDest->nPayloadSize = pDataSrc->nPayloadSize;
        MEMCPY(pDataDest->File.bFileImage,
               pDataSrc->File.bFileImage,
               pSrcBlock->pNS->pHDS->nImagesize_DataBlockHDR);
    }
    else
    {
        MEMCPY(pDestBlock, pSrcBlock, SIZEOF(_CommBlockHDR));
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*
**
*/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_WRITABLE_BUFFER, "Buffer sizes are fixed and the function is static.")
static DRM_RESULT _HdsGetPutChildBlockNum(
    __in                                                          const _NsContext *pNS,
    __inout_ecount(DRM_HDS_MAXIMUM_CHILDREN*eDRM_DSTBLKNUM_DWORD)       DRM_BYTE   *pbChildAllocTable,
    __in                                                                DRM_DWORD   nIndex,
    __inout                                                             DRM_DWORD  *pnChildBlkNum,
    __in                                                                DRM_BOOL    fGet)
PREFAST_POP
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsGetPutChildBlockNum);

    DRMASSERT(pNS && pNS->fInited && pbChildAllocTable && pnChildBlkNum
        && nIndex < pNS->wMaxNumChildren);
    __analysis_assume( pNS != NULL );

    switch (pNS->pHDS->oSRN.eBlockNumType)
    {
        case eDRM_DSTBLKNUM_DWORD:
            if ( fGet )
            {
                *pnChildBlkNum = ((DRM_DWORD*)pbChildAllocTable)[nIndex];
                FIX_ENDIAN_DWORD( *pnChildBlkNum );
            }
            else
            {
                DRM_DWORD *rgdwTab = (DRM_DWORD*)pbChildAllocTable;
                DRM_DWORD dwIndex  = *pnChildBlkNum;
                FIX_ENDIAN_DWORD( dwIndex );
                rgdwTab[nIndex] = dwIndex;
            }
            break;
        case eDRM_DSTBLKNUM_WORD:
            if ( fGet )
            {
                DRM_WORD wNum=((DRM_WORD*)pbChildAllocTable)[nIndex];
                FIX_ENDIAN_WORD( wNum );
                *pnChildBlkNum = (DRM_DWORD)wNum;
            }
            else
            {
                DRM_WORD *rgwTab = (DRM_WORD*)pbChildAllocTable;
                DRM_WORD wNum    = (DRM_WORD)*pnChildBlkNum;

                DRMASSERT( ((*pnChildBlkNum) & 0xFFFF0000) == 0 );

                FIX_ENDIAN_WORD( wNum );
                rgwTab[nIndex] = wNum;
            }
            break;
        default:
            dr = DRM_E_DST_BLOCK_MISMATCH;
            break;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/* Natural logarithm of 2 multiplied by 8 bits in a byte */
#define _HDS_BLOOM_8_TIMES_LN_OF_2 (8 * 0.693147181)

/* Log base 10 of the target probability of false positives divided by log of 0.6185
** log(0.001) / log(0.6185) = */
#define _HDS_BLOOM_LOG_OF_TARGET_PROB_DIV_BY_LOG_POINT_6185 14.37743575

static DRM_NO_INLINE DRM_VOID _HdsCalcOptimalBloom(
    __in  DRM_WORD  wBloomN,
    __out DRM_WORD *pwBloomBytesM,
    __out DRM_WORD *pwBloomK )
{
    *pwBloomBytesM = (DRM_WORD)min((DRM_WORD)((wBloomN * _HDS_BLOOM_LOG_OF_TARGET_PROB_DIV_BY_LOG_POINT_6185 + 8) / 8), HDS_CHILDBLOCK_BLOOM_MAX_M );
    *pwBloomK = (DRM_WORD)min((DRM_WORD)(((double)*pwBloomBytesM * _HDS_BLOOM_8_TIMES_LN_OF_2) / (double)wBloomN), HDS_CHILDBLOCK_BLOOM_MAX_K );
    return;
}

/**********************************************************************
** Function:    AddHashToBloom
** Synopsis:
** Arguments:   [pbDRMData] --
** Returns:     DRM_SUCCESS on success
** Notes:
***********************************************************************
*/

static DRM_NO_INLINE DRM_RESULT _HdsAddHashToBloom(
    __in          _NsContext     *f_pNS,
    __in    const DRM_BYTE        f_rgbHashKey [__CB_DECL(DRM_HDS_HASHKEY_SIZE)],
    __inout       _ChildBlockHDR *f_pChildHDR )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_WORD i = 0;
    DRM_WORD iBit = 0;
    DRM_WORD iByte = 0;
    _ChildBlockFileHDR *pHDR = (_ChildBlockFileHDR*)&f_pChildHDR->File;
    DRM_BYTE rgbHashKey[__CB_DECL(DRM_HDS_HASHKEY_SIZE)];
    DRM_BYTE iHash = 0;

    ChkArg( pHDR != NULL );
    ChkArg( f_pNS != NULL );
    ChkArg( f_pNS->wBloomBytesM <= HDS_CHILDBLOCK_BLOOM_MAX_M );
    ChkArg( f_pNS->wBloomK <= HDS_CHILDBLOCK_BLOOM_MAX_K );

    MEMCPY( rgbHashKey, f_rgbHashKey, DRM_HDS_HASHKEY_SIZE );

    for( i = 0; i < f_pNS->wBloomK; i++ )
    {
        iBit  = rgbHashKey[iHash++];
        if( f_pNS->wBloomBytesM > 32 )
        {
            /* If we've got more than 32 bytes (ie 256 bits) then we need to grab two bytes
            ** since just the one can't index this many bytes
            */
            ChkBOOL( iHash < SIZEOF( rgbHashKey ), DRM_E_ARITHMETIC_OVERFLOW ); /* Keep PREfast happy */
            iBit += rgbHashKey[iHash++] << 8;
        }
        iBit %= f_pNS->wBloomBytesM * 8;
        iByte = (DRM_WORD)(iBit / 8);
        iBit %= 8;
        pHDR->bBloomData[iByte] |= 1 << iBit;

        if( iHash >= DRM_HDS_HASHKEY_SIZE )
        {
            iHash = 0;
            DRM_MD5_Init( &f_pNS->pHDS->contextMD5 );
            DRM_MD5_Update( &f_pNS->pHDS->contextMD5, rgbHashKey, DRM_HDS_HASHKEY_SIZE);
            DRM_MD5_Final( &f_pNS->pHDS->contextMD5 );
            MEMCPY( rgbHashKey, f_pNS->pHDS->contextMD5.digest, MD5DIGESTLEN );
        }
    }

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    CheckHashInBloom
** Synopsis:
** Arguments:   [pbDRMData] --
** Returns:     DRM_SUCCESS on success
** Notes:
***********************************************************************
*/
static DRM_NO_INLINE DRM_RESULT _HdsCheckHashInBloom(
    __in          _NsContext     *f_pNS,
    __in    const DRM_BYTE        f_rgbHashKey [__CB_DECL(DRM_HDS_HASHKEY_SIZE)],
    __inout       _ChildBlockHDR *f_pChildHDR,
    __out         DRM_BOOL       *f_pfResult )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD i = 0;
    DRM_WORD iBit = 0;
    DRM_WORD iByte = 0;
    const _ChildBlockFileHDR *pHDR = (const _ChildBlockFileHDR*)&f_pChildHDR->File;
    DRM_BYTE rgbHashKey[__CB_DECL(DRM_HDS_HASHKEY_SIZE)];
    DRM_BYTE iHash = 0;

    ChkArg( pHDR != NULL
        && f_pfResult != NULL );
    ChkArg( f_pNS != NULL );
    ChkArg( f_pNS->wBloomBytesM <= HDS_CHILDBLOCK_BLOOM_MAX_M );
    ChkArg( f_pNS->wBloomK <= HDS_CHILDBLOCK_BLOOM_MAX_K );

    *f_pfResult = TRUE;

    if( f_pNS->wBloomBytesM == 0 )
    {
        /* There is no Bloom data at all */
        goto ErrorExit;
    }

    MEMCPY( rgbHashKey, f_rgbHashKey, DRM_HDS_HASHKEY_SIZE );

    for( i = 0; i < f_pNS->wBloomK; i++ )
    {
        iBit  = rgbHashKey[iHash++];
        if( f_pNS->wBloomBytesM > 32 )
        {
            /* If we've got more than 32 bytes (ie 256 bits) then we need to grab two bytes
            ** since just the one can't index this many bytes
            */
            ChkBOOL( iHash < SIZEOF( rgbHashKey ), DRM_E_ARITHMETIC_OVERFLOW ); /* Keep PREfast happy */
            iBit += rgbHashKey[iHash++] << 8;
        }
        iBit %= f_pNS->wBloomBytesM * 8;
        iByte = (DRM_WORD)( iBit / 8 );
        iBit %= 8;

        if( (pHDR->bBloomData[iByte] & (1 << iBit)) == 0 )
        {
            *f_pfResult = FALSE;
            break;
        }

        if( iHash >= DRM_HDS_HASHKEY_SIZE )
        {
            iHash = 0;
            DRM_MD5_Init( &f_pNS->pHDS->contextMD5 );
            DRM_MD5_Update( &f_pNS->pHDS->contextMD5, rgbHashKey, DRM_HDS_HASHKEY_SIZE);
            DRM_MD5_Final( &f_pNS->pHDS->contextMD5 );
            MEMCPY( rgbHashKey, f_pNS->pHDS->contextMD5.digest, MD5DIGESTLEN );
        }
    }

ErrorExit:
    return dr;
}


/**********************************************************************
** Function:    HashKeyToIndex
** Synopsis:
** Arguments:   [pbDRMData] --
** Returns:     DRM_SUCCESS on success
** Notes:
***********************************************************************
*/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_WRITABLE_BUFFER, "Buffer sizes are fixed and the function is static.")
static DRM_NO_INLINE DRM_RESULT _HdsHashKeyToIndex(
    __in                                               _NsContext *pNS,
    __in_ecount(__CB_DECL(DRM_HDS_HASHKEY_SIZE)) const DRM_BYTE    HashKey [__CB_DECL(DRM_HDS_HASHKEY_SIZE)],
    __in                                               DRM_DWORD   nBlkNum,
    __out_ecount(1)                                    DRM_DWORD  *pnIndex)
PREFAST_POP
{
#if DRM_SUPPORT_TRACING
    DRM_RESULT dr = DRM_SUCCESS;
#endif
    DRM_DWORD nFilePos;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsHashKeyToIndex);

    DRMASSERT(pNS && pNS->fInited && pnIndex);
    __analysis_assume( pNS != NULL );

    _HdsBlockNum2FilePos(pNS->pHDS, nBlkNum, &nFilePos);

    DRM_MD5_Init( &pNS->pHDS->contextMD5 );
    DRM_MD5_Update( &pNS->pHDS->contextMD5, HashKey, DRM_HDS_HASHKEY_SIZE);
    FIX_ENDIAN_DWORD( nFilePos );
    DRM_MD5_Update( &pNS->pHDS->contextMD5, (DRM_BYTE*)&nFilePos, SIZEOF(DRM_DWORD) );
    DRM_MD5_Final( &pNS->pHDS->contextMD5 );

    /* mod the first 4 bytes of the hash value with number of children per parent block */
    BYTES_TO_DWORD( *pnIndex, pNS->pHDS->contextMD5.digest );
    *pnIndex %= (DRM_DWORD)pNS->wMaxNumChildren;

/*  TRACE(("HashKeyToIndex %d, blocknum %d\n", *pnIndex, nBlkNum)); */

    DRM_PROFILING_LEAVE_SCOPE;

    return DRM_SUCCESS;
}


/**********************************************************************
** Function:    GoDownOneLevel
** Synopsis:    Traverse to a child block according to the given CID and SKUID.
** Arguments:   [pHDS] --
**              [pCurrBlock] -- NULL indicates no parent "child" node. the RootNode will
**                 used as the parent.
**              [rgbCID]
**              [rgbSKUID]
**              [pBlock] -- retrieved child block
** Returns:     DRM_SUCCESS on success
** Notes:
***********************************************************************
*/
static DRM_NO_INLINE DRM_RESULT _HdsHashToChildBlock(
    __in            _NsContext     *pNS,
    __in_opt        _CommBlockHDR  *pParentBlock,  /* NULL if start from NSRoot of current Namespace */
    __in      const DRM_BYTE        bHashKey [__CB_DECL(DRM_HDS_HASHKEY_SIZE)],
    __out           _CommBlockHDR **ppChildBlock,
    __out           DRM_BOOL       *pfResult,
    __out_opt       DRM_DWORD      *pdwChildIndex )
{
    DRM_RESULT dr=DRM_SUCCESS;
    DRM_DWORD  nBlockNum=0;
    DRM_DWORD  nIndex=0;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsHashToChildBlock);

    DRMASSERT(pNS && bHashKey && pNS->fInited && ppChildBlock && pfResult);
    __analysis_assume( pNS != NULL );

    *pfResult=FALSE;

    if ( pParentBlock == NULL )   /* start from Root of current Namespace */
    {
        nBlockNum = pNS->nCfgRootBlockNum;
    }
    else
    {
        /* compute the child's hash index */
        ChkDR(_HdsHashKeyToIndex(pNS, bHashKey, pParentBlock->nBlockNum, &nIndex));
        ChkDR(_HdsGetPutChildBlockNum(pNS, ((_ChildBlockHDR*)pParentBlock)->File._image.bChildAllocTable,
            nIndex, &nBlockNum, TRUE));
        if ( nBlockNum == 0 )
        {
            goto ErrorExit;  /* not found */
        }
    }

    ChkDR(_HdsLoadBlockHDR(pNS, nBlockNum, ppChildBlock));
    if ( GET_BYTE( &(*ppChildBlock)->File._image.bBlockType, 0) == eCHILDBLOCK )
    {
        *pfResult = TRUE;
        if ( pdwChildIndex )
        {
            *pdwChildIndex = nIndex;
        }
    }
    else
    {
        ChkDR( DRM_E_DST_CORRUPTED );
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*
**
*/
static DRM_NO_INLINE DRM_RESULT _HdsExpandStore(
    __in  _HdsContext *pHDS,
    __in  DRM_DWORD    nGrowNumofBlocks,
    __out DRM_DWORD   *pnNextBlockNum)
{
    DRM_RESULT dr=DRM_SUCCESS;
    DRM_DWORD  dwFilePos=0, dwTmpPos=0, nBlockNum=0;
    DRM_DWORD  dwNumberOfBytesIO=0;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsExpandStore);

    ChkArg(pnNextBlockNum != NULL);

    if (!Oem_File_GetSize(pHDS->fp, &dwFilePos))
    {
        ChkDR(DRM_E_FILE_SEEK_ERROR);
    }

    ChkDR(_HdsFilePos2BlockNum(pHDS, dwFilePos, &nBlockNum));

    /* verify block num */
    ChkDR(_HdsBlockNum2FilePos(pHDS, nBlockNum, &dwTmpPos));
    if ( dwTmpPos != dwFilePos )
    {
        /* max block number reached */
        TRACE( ("_HdsExpandStore(): Data store is full.\n" ));
        ChkDR(DRM_E_DST_STORE_FULL);
    }

    /* expand the file:
    ** - seek to block-size -SIZEOF(BLOCK_SIGNATURE) bytes after current EOF,
    ** - write the BLOCK_SIGNATURE at the position,
    ** - flush the file
    */
    {
        DRM_LONG lPos = 0;
        ChkDR( DRM_DWordToLong( (nGrowNumofBlocks * pHDS->oSRN.dwBlockSize) - SIZEOF(BLOCK_SIGNATURE), &lPos ) );
        if( !Oem_File_SetFilePointer( pHDS->fp, lPos, OEM_FILE_END, NULL ) )
        {
            ChkDR(DRM_E_DST_STORE_FULL);
        }
    }
    if (!Oem_File_Write(pHDS->fp, (DRM_VOID*)BLOCK_SIGNATURE, SIZEOF(BLOCK_SIGNATURE),
        &dwNumberOfBytesIO) || dwNumberOfBytesIO!=SIZEOF(BLOCK_SIGNATURE))
    {
        ChkDR(DRM_E_FILE_WRITE_ERROR);
    }

    *pnNextBlockNum = nBlockNum;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

#define GPBP_OPERATION_READ  TRUE
#define GPBP_OPERATION_WRITE FALSE

/*
** Get the specified size of payload at the specified pos of the block's child payload
*/
static DRM_NO_INLINE DRM_RESULT _HdsGetPutBlockPayload(
    __in    _CommBlockHDR *pBlock,
    __in    DRM_DWORD      nPos,   /* position relative to given block's payload, starts from 0 */
    __in    DRM_DWORD      nSize,
    __inout DRM_VOID      *pBuff,
    __in    DRM_BOOL       fGet )
{
    DRM_RESULT dr=DRM_SUCCESS;
    DRM_DWORD dwFilePos=0;
    DRM_DWORD ioCount=0;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsGetPutBlockPayload);

    DRMASSERT( pBlock && pBuff );
    __analysis_assume( pBlock != NULL );

    if( nSize == 0 )
    {
        /* No data read or written */
        goto ErrorExit;
    }

    /* compute filepos of the block */
    ChkDR(_HdsBlockNum2FilePos(pBlock->pNS->pHDS, pBlock->nBlockNum, &dwFilePos));

    /* adjust filepos relative to the block */
    switch (GET_BYTE( &pBlock->File._image.bBlockType, 0 ))
    {
        case eCHILDBLOCK:
            {
                DRMASSERT( (nPos+nSize)<=((_ChildBlockHDR*)pBlock)->nPayloadSize );
                dwFilePos += pBlock->pNS->nChildBlockPayloadPos + nPos;
            }
            break;
        case eDATABLOCK:
            {
                DRMASSERT( (nPos+nSize)<=((_DataBlockHDR*)pBlock)->nPayloadSize );
                dwFilePos += pBlock->pNS->pHDS->nDataBlockPayloadPos + nPos;
            }
            break;
        case eFREEBLOCK:
            break;
        default:
            DRMASSERT(FALSE);
    }

    /* seek to the appropriate file position */
    {
        DRM_LONG lPos = 0;
        ChkDR( DRM_DWordToLong( dwFilePos, &lPos ) );
        if (Oem_File_SetFilePointer(pBlock->pNS->pHDS->fp, lPos, OEM_FILE_BEGIN, NULL))
        {
            DRM_BOOL fIO;

            if ( fGet )
            {
                /* Note: we do not verify file position here because we already
                **       verified the block header position is at least
                **       filesize - block size.
                */
                fIO = Oem_File_Read(pBlock->pNS->pHDS->fp, pBuff, nSize, &ioCount);
            }
            else
            {
                fIO = Oem_File_Write(pBlock->pNS->pHDS->fp, pBuff, nSize, &ioCount);
            }

            if (fIO)
            {

#if !_DATASTORE_WRITE_THRU
                if ( !fGet )
                {
                    ChkBOOL( Oem_File_FlushBuffers(pBlock->pNS->pHDS->fp), DRM_E_FILE_WRITE_ERROR );
                }
#endif
                goto ErrorExit;
            }
        }
    }

    if ( fGet )
    {
        ChkDR(DRM_E_FILE_READ_ERROR);
    }
    else
    {
        ChkDR(DRM_E_FILE_WRITE_ERROR);
    }

ErrorExit:

    DRM_PROFILING_USER_DATA_1(pBlock->nBlockNum);
    DRM_PROFILING_USER_DATA_2((DRM_DWORD)fGet);
    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/**********************************************************************
** Function:    _HdsUpdateSRN
** Synopsis:
** Arguments:   [pbHdsContext] --
** Returns:     DRM_SUCCESS on success
** Notes:
***********************************************************************
*/
static DRM_NO_INLINE DRM_RESULT _HdsUpdateSRN(
    __in _HdsContext *f_pHDS )
{
    DRM_RESULT dr                                 = DRM_SUCCESS;
    DRM_BYTE   bSRNHash [__CB_DECL(MD5DIGESTLEN)] = {0};
    DRM_DWORD  dwFileSize                         = 0;
    DRM_DWORD  dwNumberOfBytesIO                  = 0;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsUpdateSRN);

    /* update hash of the root node */
    ChkArg(ISVALIDCONTEXT(f_pHDS, eHdsContextSignature));

    DRMASSERT( f_pHDS->fInited == TRUE
            && f_pHDS->fp      != OEM_INVALID_HANDLE_VALUE );

    /* read the SRN Hash in file and make sure it is the same as we have now
    ** if not, someone has been updated the SRN and we should not update ours to the file.
    */
    if (Oem_File_GetSize(f_pHDS->fp, &dwFileSize))
    {
        if ( dwFileSize < HDS_FILEHEADERSIZE )
        {
            /* expand the file to accomodate file header */
            {
                DRM_LONG lPos = 0;
                ChkDR( DRM_DWordToLong( HDS_FILEHEADERSIZE-SIZEOF(BLOCK_SIGNATURE), &lPos ) );
                if( !Oem_File_SetFilePointer(f_pHDS->fp, lPos, OEM_FILE_BEGIN, NULL) )
                {
                    ChkDR(DRM_E_FILE_SEEK_ERROR);
                }
            }
            if (!Oem_File_Write(f_pHDS->fp, (DRM_VOID*)BLOCK_SIGNATURE, SIZEOF(BLOCK_SIGNATURE), &dwNumberOfBytesIO)
                || dwNumberOfBytesIO!=SIZEOF(BLOCK_SIGNATURE))
            {
                ChkDR(DRM_E_FILE_WRITE_ERROR);
            }

            goto CommitStore;  /* this is a new file */
        }

        if( Oem_File_SetFilePointer(f_pHDS->fp, SIZEOF(DRM_DWORD), OEM_FILE_BEGIN, NULL) )
        {
            /* verify hash value of SRN */
            if (Oem_File_Read(f_pHDS->fp, bSRNHash, MD5DIGESTLEN, &dwNumberOfBytesIO) &&
                dwNumberOfBytesIO == MD5DIGESTLEN)
            {
                if (MEMCMP(bSRNHash, f_pHDS->oSRN.bSRNHash, MD5DIGESTLEN) == 0 )
                {
                    goto CommitStore;
                }
                else
                {
                    TRACE(("DRM_HDS_UpdateSRN(): Super root node's hash value mismatched\n"));
                    ChkDR(DRM_E_HDSSRNCORRUPTED);
                }
            }
        }
    }
    ChkDR(DRM_E_FILE_SEEK_ERROR);

CommitStore:

    /* update the SRN in the file */
    f_pHDS->oSRN.ui64Timestamp = _GetTimeStamp(f_pHDS->pOEMContext);
    ChkDR(_HdsGenSRNHash(f_pHDS, f_pHDS->oSRN.bSRNHash));

    /* Seek to the begining of the file and write out updated SRN data. */
    if (Oem_File_SetFilePointer(f_pHDS->fp, 0, OEM_FILE_BEGIN, NULL))
    {
        DRM_DWORD dwStoreVersion = f_pHDS->dwStoreVersion;
        FIX_ENDIAN_DWORD( dwStoreVersion );
        if (Oem_File_Write(f_pHDS->fp, (DRM_VOID*)&dwStoreVersion, SIZEOF(DRM_DWORD),
            &dwNumberOfBytesIO) && dwNumberOfBytesIO == SIZEOF(DRM_DWORD))
        {
            if (_WriteSRN(f_pHDS))
            {
#if !_DATASTORE_WRITE_THRU
                ChkBOOL( Oem_File_FlushBuffers(f_pHDS->fp), DRM_E_FILE_WRITE_ERROR );
#endif
                goto ErrorExit;
            }
        }
    }
    ChkDR(DRM_E_FILE_WRITE_ERROR);

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/*
**
*/
static DRM_NO_INLINE DRM_RESULT _HdsLoadSRN(
    __in _HdsContext *pHDS )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  dwStoreVersion = 0;
    DRM_DWORD  nFileSize=0;
    DRM_BYTE   rgbHash [__CB_DECL(MD5DIGESTLEN)];
    DRM_DWORD  dwNumberOfBytesIO;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsLoadSRN);

    DRMASSERT(pHDS!=NULL);
    __analysis_assume( pHDS != NULL );

    /* read store version # */
    if (!Oem_File_SetFilePointer(pHDS->fp, 0, OEM_FILE_BEGIN, NULL) ||
        !Oem_File_Read(pHDS->fp, &dwStoreVersion, SIZEOF(DRM_DWORD), &dwNumberOfBytesIO) ||
        dwNumberOfBytesIO!=SIZEOF(DRM_DWORD))
    {
        ChkDR(DRM_E_FILE_READ_ERROR);
    }
    FIX_ENDIAN_DWORD( dwStoreVersion );

    /* verify the version # and set file header size appropriately */
    pHDS->dwStoreVersion = dwStoreVersion;
    switch (dwStoreVersion)
    {
        case DRM_HDS_FILE_VERSION_0:
            break;

        case DRM_HDS_FILE_VERSION_1:
            break;

        case DRM_HDS_FILE_VERSION_2:
            break;

        default:
            TRACE( ("HDS file version incorrect. Can't handle this file.\n" ));
            ChkDR(DRM_E_HDSINVALIDSTORE);
    }

    /* Read the SRN */
    ZEROMEM(&pHDS->oSRN, SIZEOF(_SRN));
    if (!Oem_File_SetFilePointer(pHDS->fp, SIZEOF(DRM_DWORD), OEM_FILE_BEGIN, NULL) || /* skip size of version DWORD */
        !_ReadSRN(pHDS) ) /* read the SRN data from file */
    {
        ChkDR(DRM_E_FILE_READ_ERROR);
    }

    /* check SRN and block size */
    if( pHDS->oSRN.dwSRNSize   != SIZEOF(_SRN)
     || pHDS->oSRN.dwBlockSize == 0 )
    {
        /* Incorrect Super root node size */
        ChkDR(DRM_E_HDSINVALIDSTORE);
    }

    if (!Oem_File_GetSize(pHDS->fp, &nFileSize))
    {
        ChkDR(DRM_E_FILE_SEEK_ERROR);
    }

    if ( nFileSize < HDS_FILEHEADERSIZE
        || ((nFileSize - HDS_FILEHEADERSIZE) % pHDS->oSRN.dwBlockSize) != 0 )
    {
        ChkDR(DRM_E_DST_CORRUPTED);
    }

    if ( pHDS->oSRN.eBlockNumType != eDRM_DSTBLKNUM_DWORD
      && pHDS->oSRN.eBlockNumType != eDRM_DSTBLKNUM_WORD )
    {
        ChkDR(DRM_E_DST_CORRUPTED);
    }

    /* verify the hash value */
    ChkDR(_HdsGenSRNHash(pHDS, rgbHash));
    if (MEMCMP(rgbHash, pHDS->oSRN.bSRNHash, SIZEOF( pHDS->oSRN.bSRNHash ) ) != 0)
    {
        ChkDR(DRM_E_HASH_MISMATCH);      /* SRN Hash failed! */
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/**********************************************************************
** Function:    _HdsAllocBlock
** Synopsis:    Allocate a File Block.
** Arguments:   [f_pNS] --
**              [f_nParentBlockNum] -- parent block num of of the new block
**              [f_ppBlock]         -- buffer to receive the new block
**              [f_eBlockType]      -- type of the block
** Returns:     DRM_SUCCESS on success
** Notes:       A recycle free block list is kept in the root node. _AllocFileBlock will first
**              check if any free block is available.
***********************************************************************
*/
static DRM_NO_INLINE DRM_RESULT _HdsAllocBlock(
    __in     _NsContext     *f_pNS,
    __in     DRM_DWORD       f_nParentBlockNum,
    __out    _CommBlockHDR **f_ppBlock,
    __in     _EBlockType     f_eBlockType)
{
    DRM_RESULT    dr                                = DRM_SUCCESS;
    DRM_BOOL      fAllocBuff                        = FALSE;
    _HdsContext  *pHDS                              = NULL;
    DRM_BYTE     *pCopyBuffer                       = NULL;
    DRM_DWORD     dwFilePos                         = 0;
    DRM_BYTE      rgbHash [__CB_DECL(MD5DIGESTLEN)] = {0};
    _CommBlockHDR tmpBlock                          = {0};

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsAllocBlock);

    ChkArg(ISVALIDCONTEXT(f_pNS, eCfgContextSignature));

    DRMASSERT(f_pNS->fInited == TRUE
           && f_ppBlock      != NULL
           && (f_eBlockType==eCHILDBLOCK || f_eBlockType==eDATABLOCK));

    pHDS = f_pNS->pHDS;
    if ( *f_ppBlock == NULL )
    {
        fAllocBuff = TRUE;
        ChkDR(DRM_HDSIMPL_AllocBlockBuffer(f_pNS, f_eBlockType, f_ppBlock));
    }

    /* flush SRN in context */
    ChkDR(_HdsLoadSRN(pHDS));

    /* check if there is any recycle blocks kept in RootNode */
    if ( pHDS->oSRN.nFreeListHead == 0 )
    {
        ChkDR(_HdsPreAlloc(pHDS, pHDS->nGrowbySizeinKB, FALSE, NULL));
    }


    /* Free list in SRN must have some free block(s) now
    */

    /* Seek to the begining of the free block and read the generic header. */
    ChkDR(_HdsBlockNum2FilePos(pHDS, pHDS->oSRN.nFreeListHead, &dwFilePos));

    {
        DRM_LONG lPos = 0;
        ChkDR( DRM_DWordToLong( dwFilePos, &lPos ) );
        if (!Oem_File_SetFilePointer(pHDS->fp, lPos, OEM_FILE_BEGIN, NULL))
        {
            ChkDR(DRM_E_FILE_READ_ERROR);
        }
    }
    if( !_ReadCommonBlockHeader( pHDS, &tmpBlock, pHDS->nImagesize_FileBlockHDR, NULL ) )
    {
        ChkDR(DRM_E_FILE_READ_ERROR);
    }

    if ( GET_BYTE( &(tmpBlock.File._image.bBlockType), 0 ) != eFREEBLOCK )  /* verify if the given buffer is the correct type */
    {
        ChkDR(DRM_E_DST_BLOCK_MISMATCH);
    }

    /* copy block header to new buffer */
    MEMCPY((*f_ppBlock)->File.bFileImage, tmpBlock.File.bFileImage, pHDS->nImagesize_FileBlockHDR);

    /* read the rest of the free block */
    if (!_ReadDataBlockHeader(pHDS, ((_DataBlockHDR*)(*f_ppBlock)) ) )
    {
        ChkDR(DRM_E_FILE_READ_ERROR);
    }

    /* everything is alright */
    ((_DataBlockHDR*)(*f_ppBlock))->nPayloadSize = pHDS->nImagesize_DataBlockPayload;
    (*f_ppBlock)->nBlockNum = pHDS->oSRN.nFreeListHead;
    (*f_ppBlock)->pNS = f_pNS;

    /* validate the block's hash value */
    ChkDR(_HdsGenBlockHash(f_pNS->pHDS, f_pNS, *f_ppBlock, rgbHash));
    if (MEMCMP((*f_ppBlock)->File._image.bBlockHash, rgbHash, SIZEOF( rgbHash ) ) != 0)
    {
        ChkDR(DRM_E_HASH_MISMATCH);
    }

    /* update free list and commit changes of root node to file */
    pHDS->oSRN.nFreeListHead = ((_DataBlockHDR*)(*f_ppBlock))->File._image.nCascadingBlockNum;
    ChkDR(_HdsUpdateSRN(pHDS));

    /* set up the block */
    ChkDR(_HdsInitBlockBuffer(f_pNS, *f_ppBlock, f_nParentBlockNum, f_eBlockType));

    /* wipe the whole block with NULL char */
    {
        DRM_DWORD nIndex = 0;
        DRM_DWORD dwAmount = 0;

        if ( f_eBlockType == eCHILDBLOCK )
        {
            dwAmount = (*f_ppBlock)->pNS->nImagesize_ChildBlockPayload - SIZEOF(BLOCK_SIGNATURE);
        }
        else  /* eBlockType == eDATABLOCK */
        {
            dwAmount = (*f_ppBlock)->pNS->pHDS->nImagesize_DataBlockPayload - SIZEOF(BLOCK_SIGNATURE);
        }

        ChkDR(_Hds_malloc((*f_ppBlock)->pNS->pHDS, DRM_HDS_COPY_BUFFER_SIZE, (DRM_VOID**)&pCopyBuffer));
        ChkMem( pCopyBuffer );
        ZEROMEM( pCopyBuffer, DRM_HDS_COPY_BUFFER_SIZE );

        for (nIndex=0; nIndex<dwAmount;)
        {
            ChkDR(_HdsGetPutBlockPayload((*f_ppBlock), nIndex, min(DRM_HDS_COPY_BUFFER_SIZE, dwAmount-nIndex),
                pCopyBuffer, GPBP_OPERATION_WRITE));
            nIndex += min(DRM_HDS_COPY_BUFFER_SIZE, dwAmount-nIndex);
        }

        _Hds_free((*f_ppBlock)->pNS->pHDS, pCopyBuffer);
        pCopyBuffer = NULL;

        /* write the block signature at end of block */
        ChkDR(_HdsGetPutBlockPayload((*f_ppBlock), dwAmount, SIZEOF(BLOCK_SIGNATURE),
            (DRM_VOID*)BLOCK_SIGNATURE, GPBP_OPERATION_WRITE));
    }

    /* update the new block */
    ChkDR(_HdsWriteBlockHDR(*f_ppBlock));

    /* make sure the new block is written to disk before the parent block */
    ChkBOOL( Oem_File_FlushBuffers(pHDS->fp), DRM_E_FILE_WRITE_ERROR );

ErrorExit:
    if ( pCopyBuffer != NULL )
    {
        _Hds_free((*f_ppBlock)->pNS->pHDS, pCopyBuffer);
    }

    if ( DRM_FAILED(dr) && fAllocBuff && *f_ppBlock!=NULL )
    {
        _Hds_free(pHDS, *f_ppBlock);
        *f_ppBlock = NULL;
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/**********************************************************************
** Function:    _HdsFreeBlock
** Synopsis:    Free an allocated File Block.
** Arguments:   [pHDS] --
**              [pBlock] --block to be free'd
** Returns:     DRM_SUCCESS on success
** Notes:       A recycle free block list is kept in the root node. All free'd blocks
**              are put in a linked list.
***********************************************************************
*/
static DRM_NO_INLINE DRM_RESULT _HdsFreeBlock(
    __inout _CommBlockHDR *pBlock)
{
    DRM_RESULT dr=DRM_SUCCESS;
    _DataBlockHDR *pDataBlock=NULL;
    DRM_BYTE *pCopyBuffer=NULL;
    DRM_DWORD nIndex=0;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsFreeBlock);

    DRMASSERT(pBlock && pBlock->pNS && pBlock->pNS->fInited);
    __analysis_assume( pBlock != NULL );
    __analysis_assume( pBlock->pNS != NULL );

    /* flush SRN in context */
    ChkDR(_HdsLoadSRN(pBlock->pNS->pHDS));

    /* wipe the block buffer */
    ZEROMEM(pBlock->File.bFileImage, pBlock->pNS->pHDS->nImagesize_FileBlockHDR);
    _HdsInitBlockBuffer(pBlock->pNS, pBlock, 0, eDATABLOCK);

    /* optional: wipe the whole block with NULL char */
    {
        DRM_DWORD dwAmount = pBlock->pNS->pHDS->nImagesize_DataBlockPayload - SIZEOF(BLOCK_SIGNATURE);

        ChkDR(_Hds_malloc(pBlock->pNS->pHDS, DRM_HDS_COPY_BUFFER_SIZE, (DRM_VOID**)&pCopyBuffer));
        ChkMem( pCopyBuffer );
        ZEROMEM((DRM_BYTE *) pCopyBuffer, DRM_HDS_COPY_BUFFER_SIZE);

        for (nIndex=0; nIndex<dwAmount;)
        {
            ChkDR(_HdsGetPutBlockPayload(pBlock, nIndex, min(DRM_HDS_COPY_BUFFER_SIZE, dwAmount-nIndex),
                pCopyBuffer, GPBP_OPERATION_WRITE));
            nIndex += min(DRM_HDS_COPY_BUFFER_SIZE, dwAmount-nIndex);
        }

        _Hds_free(pBlock->pNS->pHDS, pCopyBuffer);
        pCopyBuffer = NULL;

        /* write the block signature at end of block */
        ChkDR(_HdsGetPutBlockPayload(pBlock, dwAmount, SIZEOF(BLOCK_SIGNATURE),
            (DRM_VOID*)BLOCK_SIGNATURE, GPBP_OPERATION_WRITE));
    }

    PUT_BYTE( &pBlock->File._image.bBlockType, 0, eFREEBLOCK );

    /* chain to SRN's freelist */
    pDataBlock = (_DataBlockHDR*)pBlock;
    pDataBlock->File._image.nCascadingBlockNum = pBlock->pNS->pHDS->oSRN.nFreeListHead;
    pBlock->pNS->pHDS->oSRN.nFreeListHead = pBlock->nBlockNum;

    /* update block to file */
    ChkDR(_HdsWriteBlockHDR(pBlock));

    /* commit changes of root node to file */
    ChkDR(_HdsUpdateSRN(pBlock->pNS->pHDS));


ErrorExit:
    if ( pCopyBuffer )
    {
        _Hds_free(pBlock->pNS->pHDS, pCopyBuffer);
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/**********************************************************************
** Function:    _HdsLocateFreeBlockForSlot
** Synopsis:    Locate a block with timestamp >= given timestamp and have
**              enough space to store the license.
** Arguments:   [pHDS] --
** Returns:     DRM_SUCCESS on success
** Notes:
***********************************************************************
*/
static DRM_NO_INLINE DRM_RESULT _HdsLocateFreeBlockForSlot(
    __in        _NsContext    *pNS,
    __in  const DRM_BYTE       bHashKey [__CB_DECL(DRM_HDS_HASHKEY_SIZE)],
    __in        DRM_DWORD      cbSlot,
    __out       _SlotContext  *pSlotCtx,
    __in        DRM_UINT64     ui64TimeStamp )
{
    DRM_RESULT     dr=DRM_SUCCESS;
    DRM_BOOL       fResult=FALSE;
    _CommBlockHDR *pParentBlock=NULL;

#ifdef _DEBUG
    DRM_DWORD nNumLevels=0;
#endif

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsLocateFreeBlockForSlot);

    DRMASSERT(pNS && bHashKey && pNS->fInited && pSlotCtx &&
        pSlotCtx->eStatus!=eSlotCtxUninit && cbSlot);
    __analysis_assume( pNS != NULL );

    /* pSlotCtx might have something we can start with */
    if ( pSlotCtx->eStatus==eSlotCtxReady &&   /* pSlotCtx has info we can use */
        MEMCMP(bHashKey, pSlotCtx->oSlotHeader.oHashkey.rgb, DRM_HDS_HASHKEY_SIZE)==0 )   /* make sure key1 is the same */
    {
        ChkDR(DRM_HDSIMPL_AllocBlockBuffer(pNS, eCHILDBLOCK, &pParentBlock));
        if ( pSlotCtx->nBlockWithLargestFreeSpace!=0 )
        {
            if ( pSlotCtx->dwLargestSpace >= cbSlot )
            {
                ChkDR(_HdsLoadBlockHDR(pNS, pSlotCtx->nBlockWithLargestFreeSpace,
                    &pSlotCtx->pCurrChildBlock));
                goto ErrorExit;
            }
            else
            {
                /* let's start with this block */
                ChkDR(_HdsLoadBlockHDR(pNS, pSlotCtx->nBlockWithLargestFreeSpace, &pParentBlock));
            }
        }
        else
        {
            /* start with current child block in pSlotCtx */
            ChkDR(_HdsCopyBlockBuffer(pParentBlock, pSlotCtx->pCurrChildBlock));
        }
    }

    while (TRUE)
    {
        /* find a child block according to bHashKey and parentBlockNum */
        ChkDR(_HdsHashToChildBlock(pNS, pParentBlock, bHashKey, &pSlotCtx->pCurrChildBlock,
            &fResult, NULL));
#ifdef _DEBUG
        ++nNumLevels;
#endif
        if ( !fResult )
        {
            /* no children block found, let's create one */
            DRM_DWORD nIndex=0;

            /* can't create a child under a null parent */
            ChkArg( pParentBlock != NULL );

            ChkDR(_HdsHashKeyToIndex(pNS, bHashKey, pParentBlock->nBlockNum, &nIndex));
            ChkDR(_HdsAllocBlock(pNS, pParentBlock->nBlockNum, &pSlotCtx->pCurrChildBlock, eCHILDBLOCK));
            pSlotCtx->eStatus = eSlotCtxReady;

            /* assign new block num to parent block and update the parent block to file */
            ChkDR(_HdsGetPutChildBlockNum(pNS, ((_ChildBlockHDR*)pParentBlock)->File._image.bChildAllocTable,
                nIndex, &(pSlotCtx->pCurrChildBlock->nBlockNum), FALSE));

            ChkDR(_HdsWriteBlockHDR(pParentBlock));                         /* write Block HDR */
            break;
        }

        /* found one, check if we can use it */
        else if ( ((_ChildBlockHDR*)(pSlotCtx->pCurrChildBlock))->File._image.nFreeMem >= cbSlot &&   /* check avail space */
                  DRM_UI64Les( ui64TimeStamp, pSlotCtx->pCurrChildBlock->File._image.ui64TimeStamp ) )    /* time stamp later than what we need */
        {
            pSlotCtx->eStatus = eSlotCtxReady;
            break;
        }

        /* keep digging along the path */
        if ( pParentBlock == NULL )
        {
            ChkDR(DRM_HDSIMPL_AllocBlockBuffer(pNS, eCHILDBLOCK, &pParentBlock));
        }

        ChkDR(_HdsCopyBlockBuffer(pParentBlock, (pSlotCtx->pCurrChildBlock)));
    }

#ifdef _DEBUG
    /*TRACE(("[Levels traversed down=%d]\n", nNumLevels)); */
#endif

ErrorExit:
    if (pParentBlock)
    {
        _Hds_free(pNS->pHDS, pParentBlock);
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/**********************************************************************
** Function:    _HdsCreateOversizedSlot
** Synopsis:    store an oversized license over 1+ cascading _DATANODE's
** Arguments:   [pHDS] --
**              [pBlock] --
**              [pbLicense] --
**              [cbLicense] --
** Returns:     DRM_SUCCESS on success
** Notes:
***********************************************************************
*/
static DRM_NO_INLINE DRM_RESULT _HdsCreateOversizedSlot(
    __in  _NsContext    *pNS,
    __in  DRM_DWORD      nParentBlockNum,
    __in  DRM_DWORD      cbSlot,
    __out _CommBlockHDR *pFirstDataBlock )
{
    DRM_RESULT dr=DRM_SUCCESS;
    DRM_DWORD nBytesLeft=0;
    _CommBlockHDR *pDataBlock=NULL;
    _CommBlockHDR *pTmpBlock=NULL;
    DRM_BOOL fCopied=FALSE;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsCreateOversizedSlot);

    DRMASSERT(pNS && pNS->fInited && cbSlot>0 && pFirstDataBlock);
    __analysis_assume( pNS != NULL );

    /* allocate working buffer */
    ChkDR(DRM_HDSIMPL_AllocBlockBuffer(pNS, eDATABLOCK, &pDataBlock));
    ChkDR(DRM_HDSIMPL_AllocBlockBuffer(pNS, eDATABLOCK, &pTmpBlock));

    /* allocate the first _DataBlockHDR */
    ChkDR(_HdsAllocBlock(pNS, nParentBlockNum, &pDataBlock, eDATABLOCK));

    /* divide the slot content into packets and store the packets in _DataBlockHDR's */
    nBytesLeft = cbSlot;
    while (nBytesLeft > 0)
    {
        if ( nBytesLeft <= pNS->pHDS->nImagesize_DataBlockPayload )
        {
            /* we are done */
            ((_DataBlockHDR*)pDataBlock)->File._image.nCascadingBlockNum = 0;
            ChkDR(_HdsWriteBlockHDR(pDataBlock));
            if ( !fCopied )
            {
                ChkDR(_HdsCopyBlockBuffer(pFirstDataBlock, pDataBlock));
            }
            break;
        }
        else
        {
            /* switch buffer and allocate a new block, update the block to file and keep on */
            ChkDR(_HdsAllocBlock(pNS, pDataBlock->nBlockNum, &pTmpBlock, eDATABLOCK));
            ((_DataBlockHDR*)pDataBlock)->File._image.nCascadingBlockNum = pTmpBlock->nBlockNum;
            ChkDR(_HdsWriteBlockHDR(pDataBlock));
            if ( !fCopied )
            {
                fCopied = TRUE;
                ChkDR(_HdsCopyBlockBuffer(pFirstDataBlock, pDataBlock));
            }

            nBytesLeft -= pNS->pHDS->nImagesize_DataBlockPayload;
            _HdsCopyBlockBuffer(pDataBlock, pTmpBlock);
        }
    }

    ChkBOOL( Oem_File_FlushBuffers(pNS->pHDS->fp), DRM_E_FILE_WRITE_ERROR );

ErrorExit:
    /* free these have to be in reverse order of alloc */
    if (pTmpBlock)
    {
        _Hds_free(pNS->pHDS, pTmpBlock);
    }
    if (pDataBlock)
    {
        _Hds_free(pNS->pHDS, pDataBlock);
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**********************************************************************
** Function:    _HdsOpenSlotFromHint
** Synopsis:
** Arguments:   [f_pNS]         --
**              [f_bHashkey]    -- hashkey to be searched
**              [f_pUniquekey]  -- uniquekey to be searched
**              [f_pSlotHint]   -- hint to be used for finding slot
**              [f_fIsDataSlot] --
**              [f_pcbSlotSize] -- slot size
**              [f_pSlotCtx]    -- slot context to be returned
** Returns:     DRM_SUCCESS on success
** Notes:       Slot content is preceded with a _SlotHeader structure in the payload
**              of _ChildBlockHDR. _SlotHeader.dwSlotSize indicates the actual size of the
**              slot.
***********************************************************************
*/
static DRM_NO_INLINE DRM_RESULT _HdsOpenSlotFromHint(
    __in          _NsContext       *f_pNS,
    __in    const DRM_DST_KEY      *f_pHashkey,
    __in    const DRM_DST_KEY      *f_pUniquekey,
    __in    const _SlotHint        *f_pSlotHint,
    __in          DRM_BOOL          f_fIsDataSlot,
    __inout       DRM_DWORD        *f_pcbSlotSize,
    __out         _SlotContext     *f_pSlotCtx )
{
    DRM_RESULT          dr              = DRM_SUCCESS;
    _CommBlockHDR      *pBlock          = NULL;
    _SlotHeader         oHeader         = {0};
    _ESlotSearchMode    eSearchMode     = f_fIsDataSlot? eSearchDataSlot : eSearchNamespace;
    DRM_BOOL            fAcceptSlot     = FALSE;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsOpenSlotFromHint);

    ChkArg(ISVALIDCONTEXT(f_pNS, eCfgContextSignature)
        && f_pHashkey  != NULL
        && !DRM_UTL_IsZeros(f_pHashkey->rgb, DRM_HDS_HASHKEY_SIZE)
        && f_pSlotHint != NULL );

    if( DRM_UTL_IsZeros((const DRM_BYTE*)f_pSlotHint, SIZEOF(_SlotHint)) )
    {
        /* The struct is blank, we can't open with it. It's normal for us to
        ** be called with a blank struct, and ChkArg spits out TRACEs, so avoiding
        ** ChkArg here
        */
        dr = DRM_E_INVALIDARG;
        goto ErrorExit;
    }

    DRMASSERT(f_pNS->fInited          == TRUE
           && f_pNS->pHDS             != NULL
           && f_pNS->pHDS->fp         != OEM_INVALID_HANDLE_VALUE
           && f_pSlotCtx              != NULL
           && f_pUniquekey            != NULL
           && f_pNS->nCfgRootBlockNum  > 0);

    f_pSlotCtx->nBlockWithLargestFreeSpace = 0;
    f_pSlotCtx->dwLargestSpace = 0;
    *f_pcbSlotSize = 0;

    ChkDR(_HdsLoadBlockHDR(f_pNS, f_pSlotHint->nBlockNum, &pBlock));

    ChkDR(_HdsGetPutBlockPayload(pBlock, f_pSlotHint->nSlotPosInBlock,
        SIZEOF(_SlotHeader), (DRM_BYTE*)&oHeader, GPBP_OPERATION_READ));
    FIX_ENDIAN_DWORD( oHeader.dwSlotSize );
    FIX_ENDIAN_DWORD( oHeader.dwAttributes );

    if ((oHeader.dwAttributes & eSlotIsHidden) != 0)
    {
        fAcceptSlot = (eSearchMode == eSearchHiddenOnly);
    }
    else
    {
        fAcceptSlot = (eSearchMode == eSearchSlotDontCare
                || (eSearchMode == eSearchNamespace &&  (oHeader.dwAttributes & eSlotIsNamespace))
                || (eSearchMode == eSearchDataSlot  && !(oHeader.dwAttributes & eSlotIsNamespace)));

    }

    if ( !fAcceptSlot )
    {
        ChkDR(DRM_E_DST_SLOT_NOT_FOUND);
    }

    /* check the keys */
    if (!((MEMCMP(oHeader.oHashkey.rgb,   f_pHashkey->rgb,   DRM_HDS_HASHKEY_SIZE)   == 0)
    &&  (f_pUniquekey == NULL || MEMCMP(oHeader.oUniquekey.rgb, f_pUniquekey->rgb, DRM_HDS_UNIQUEKEY_SIZE) == 0)))
    {
        ChkDR(DRM_E_DST_SLOT_NOT_FOUND);
    }


    /* prepare for return */

    /* fill in the fields in _SlotContext */
    MEMCPY(&f_pSlotCtx->oFileBlock, pBlock, SIZEOF(_CommBlockHDR));
    ChkDR(_HdsCopyBlockBuffer(f_pSlotCtx->pCurrChildBlock, pBlock));  /* copy curr block to slot context */

    MEMCPY(&f_pSlotCtx->oSlotHeader, &oHeader, SIZEOF(_SlotHeader));
    f_pSlotCtx->dwSlotPosInBlock = f_pSlotHint->nSlotPosInBlock;
    f_pSlotCtx->dwSeekPointer = 0;

    /* check if we need to load _DataBlockHDR */
    if ( !ISOVERSIZESLOT(oHeader.dwSlotSize, f_pSlotCtx->pNS) )
    {
        f_pSlotCtx->pCurrDataBlock = NULL;
    }
    else
    {
        /* get _DataBlockHDR block # */
        ChkDR(_HdsGetPutBlockPayload(pBlock, f_pSlotHint->nSlotPosInBlock+SIZEOF(_SlotHeader),
            SIZEOF(DRM_DWORD), (DRM_BYTE*)&(f_pSlotCtx->dwFirstDataBlockNum), GPBP_OPERATION_READ));
        FIX_ENDIAN_DWORD( f_pSlotCtx->dwFirstDataBlockNum );
        /* load the first _DataBlockHDR */
        ChkDR(_HdsLoadBlockHDR(f_pSlotCtx->pNS, f_pSlotCtx->dwFirstDataBlockNum, &f_pSlotCtx->pCurrDataBlock));
        f_pSlotCtx->dwNumofDataBlockVisited = 0;
    }

    *f_pcbSlotSize = oHeader.dwSlotSize;
    f_pSlotCtx->eStatus = eSlotCtxReady;

ErrorExit:

    if ( pBlock != NULL )
    {
        _Hds_free(f_pNS->pHDS, pBlock);
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/**********************************************************************
** Function:    SearchSlotInBlock
** Synopsis:    search a license in the block at next searchable position.
** Arguments:   [pCurrBlock] -- block to search in
**              [bHashKey] --hashkey to be searched
**              [pbUniqueKey] --uniquekey to be searched
**              [pcbSlotSize] -- slot size
**              [pSlotCtx] -- slot context to be returned
**              [pfResult] --TRUE if a matching slot is found
** Returns:     DRM_SUCCESS on success
** Notes:       Slot content is preceded with a _SlotHeader structure in the payload
**              of _ChildBlockHDR. _SlotHeader.dwSlotSize indicates the actual size of the
**              slot.
***********************************************************************
*/
static DRM_NO_INLINE DRM_RESULT _HdsSearchSlotInBlock(
    __in            _CommBlockHDR     *pCurrBlock,
    __in_opt  const DRM_DST_KEY       *pHashkey,
    __in_opt  const DRM_DST_KEY       *pUniquekey,
    __in            _ESlotSearchMode   eSearchMode,
    __out           _SlotContext      *pSlotCtx,  /* user given Dataslot context buffer */
    __out           DRM_BOOL          *pfResult )
{
    DRM_RESULT dr=DRM_SUCCESS;
    DRM_DWORD nBytesToSearch=0;
    DRM_DWORD nBytesToSkip=0;
    _ChildBlockHDR *pChild=(_ChildBlockHDR*)pCurrBlock;
    _SlotHeader oHeader = {0};
    DRM_BOOL fBloomResult = FALSE;
    DRM_BYTE *pbBlockBuffer = NULL;
    DRM_DWORD ibBlockBufferBase = 0;
    DRM_DWORD cbBlockBuffer = 0;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsSearchSlotInBlock);

    /* Note: It is safe not to issue any lock when search each slot header in
    ** the block. It is because the caller function should already lock the SRN
    ** either shared or exclusive before calling this function. Functions which
    ** writing to the slot header(s) must issued exclusive lock to the SRN. So
    ** if we reach this point, there are no thread(s) or process(es) writing to
    ** the slot header(s).
    */

    DRMASSERT( pCurrBlock && pSlotCtx && pfResult
        && pSlotCtx->eStatus>=eSlotCtxInited && pSlotCtx->pCurrChildBlock
        && GET_BYTE( &pSlotCtx->pCurrChildBlock->File._image.bBlockType, 0 ) == eCHILDBLOCK);
    __analysis_assume( pCurrBlock != NULL );
    __analysis_assume( pSlotCtx != NULL );

    *pfResult = FALSE;
    ZEROMEM(&oHeader, SIZEOF(_SlotHeader));
    nBytesToSearch = pSlotCtx->pNS->nImagesize_ChildBlockPayload - pChild->File._image.nFreeMem;
    if ( nBytesToSearch == 0 )
    {
        goto ErrorExit;
    }

    if( pHashkey != NULL )
    {
        /* If they're searching for a specific hashkey, check the Bloom filter
           to see if the slot exists in this block */
        ChkDR( _HdsCheckHashInBloom( pSlotCtx->pNS, pHashkey->rgb, pChild, &fBloomResult ) );
        if( !fBloomResult )
        {
            /* Slot doesn't exist in this block */
            goto ErrorExit;
        }
        /* Filter returned that there is a probability that the slot exists in this block */
    }

    /* walk the payload of child node to find matching HashKey and UniqueKey. Each license are stored
    ** one by one in the payload ...
    */
    if ( pChild->nNextSlotPos == MAXSLOTPOS )
    {
        pChild->nNextSlotPos = 0;
    }

    if( DRM_HDS_COPY_BUFFER_SIZE >= nBytesToSearch - pChild->nNextSlotPos )
    {
        ChkDR(_Hds_malloc(pCurrBlock->pNS->pHDS, DRM_HDS_COPY_BUFFER_SIZE, (DRM_VOID**)&pbBlockBuffer));
        ChkMem( pbBlockBuffer );

        cbBlockBuffer = min( DRM_HDS_COPY_BUFFER_SIZE, nBytesToSearch - pChild->nNextSlotPos );

        dr = _HdsGetPutBlockPayload((_CommBlockHDR*)pChild, pChild->nNextSlotPos,
            cbBlockBuffer, pbBlockBuffer, GPBP_OPERATION_READ);
        if( dr == DRM_E_FILE_READ_ERROR )
        {
            /* If there's a slot in the current block that is locked exclusively, we can't cache the whole block.
                    ** So we'll just fall back to loading each individual slot header one at a time as even if a slot is locked
                    ** the slot header won't be
                    */
            _Hds_free(pCurrBlock->pNS->pHDS, pbBlockBuffer);
            pbBlockBuffer = NULL;
            cbBlockBuffer = 0;
            dr = DRM_SUCCESS;
        }
        else
        {
            ChkDR( dr );
            ibBlockBufferBase = pChild->nNextSlotPos;
        }
    }

    for (; pChild->nNextSlotPos<nBytesToSearch; pChild->nNextSlotPos+=nBytesToSkip)
    {
        DRM_BOOL fAcceptSlot = FALSE;

        /* read the _SlotHeader structure */

        if(  cbBlockBuffer > SIZEOF( _SlotHeader )
         && (ibBlockBufferBase + cbBlockBuffer) >= (pChild->nNextSlotPos + SIZEOF( _SlotHeader )) )
        {
            DRM_BYT_CopyBytes( (DRM_BYTE*)&oHeader,
                                0, pbBlockBuffer,
                                pChild->nNextSlotPos - ibBlockBufferBase,
                                SIZEOF( _SlotHeader ) );
        }
        else
        {
            ChkDR(_HdsGetPutBlockPayload((_CommBlockHDR*)pChild, pChild->nNextSlotPos,
                SIZEOF(_SlotHeader), (DRM_BYTE*)&oHeader, GPBP_OPERATION_READ));
        }
        FIX_ENDIAN_DWORD( oHeader.dwSlotSize );
        FIX_ENDIAN_DWORD( oHeader.dwAttributes );

        if ((oHeader.dwAttributes & eSlotIsHidden) != 0)
        {
            fAcceptSlot = (eSearchMode == eSearchHiddenOnly);
        }
        else
        {
            fAcceptSlot = (eSearchMode == eSearchSlotDontCare
                       || (eSearchMode == eSearchNamespace &&  (oHeader.dwAttributes & eSlotIsNamespace))
                       || (eSearchMode == eSearchDataSlot  && !(oHeader.dwAttributes & eSlotIsNamespace)));

        }

        if (fAcceptSlot)
        {
            /* check the keys */
            if ((pHashkey   == NULL || MEMCMP(oHeader.oHashkey.rgb,   pHashkey->rgb,   DRM_HDS_HASHKEY_SIZE)   == 0)
            &&  (pUniquekey == NULL || MEMCMP(oHeader.oUniquekey.rgb, pUniquekey->rgb, DRM_HDS_UNIQUEKEY_SIZE) == 0))
            {
                if( pbBlockBuffer != NULL )
                {
                    _Hds_free(pCurrBlock->pNS->pHDS, pbBlockBuffer);
                    pbBlockBuffer = NULL;
                }
                goto PrepareSlot;  /* we get a match */
            }
        }

        /* skip this slot */
        if ( ISOVERSIZESLOT(oHeader.dwSlotSize, pSlotCtx->pNS) )
        {
            nBytesToSkip = SIZEOF(_SlotHeader) + SIZEOF(DRM_DWORD);
        }
        else
        {
            nBytesToSkip = SIZEOF(_SlotHeader) + oHeader.dwSlotSize;
        }
    }

    /* slot not found !! */
    goto ErrorExit;

PrepareSlot:
    /* prepare for return */

    /* fill in the fields in _SlotContext */
    MEMCPY(&pSlotCtx->oFileBlock, &pChild->oBlockHeader, SIZEOF(_CommBlockHDR));
    ChkDR(_HdsCopyBlockBuffer(pSlotCtx->pCurrChildBlock, pCurrBlock));  /* copy curr block to slot context */

    MEMCPY(&pSlotCtx->oSlotHeader, &oHeader, SIZEOF(_SlotHeader));
    pSlotCtx->dwSlotPosInBlock = pChild->nNextSlotPos;
    pSlotCtx->dwSeekPointer = 0;

    /* check if we need to load _DataBlockHDR */
    if ( !ISOVERSIZESLOT(oHeader.dwSlotSize, pSlotCtx->pNS) )
    {
        pSlotCtx->pCurrDataBlock = NULL;
    }
    else
    {
        /* For oversized slots, load the first data block number */
        _CommBlockHDR *pChildBlock = pSlotCtx->pCurrChildBlock;

        ChkDR(_HdsGetPutBlockPayload((_CommBlockHDR*)pChildBlock,
                                     pSlotCtx->dwSlotPosInBlock + SIZEOF(_SlotHeader),
                                     SIZEOF(DRM_DWORD),
                                     (DRM_BYTE*)&(pSlotCtx->dwFirstDataBlockNum), TRUE));
        FIX_ENDIAN_DWORD( pSlotCtx->dwFirstDataBlockNum );

        /* load the first _DataBlockHDR */
        ChkDR(_HdsLoadBlockHDR(pSlotCtx->pNS,
                               pSlotCtx->dwFirstDataBlockNum,
                               &pSlotCtx->pCurrDataBlock));
        pSlotCtx->dwNumofDataBlockVisited = 0;
    }

    /* update slot pointer in child block */
    pChild->nCurrSlotPos = pChild->nNextSlotPos; /* set pointer to current Slot */

    /* move next pointer to next slot */
    if ( ISOVERSIZESLOT(oHeader.dwSlotSize, pSlotCtx->pNS) )
    {
        pChild->nNextSlotPos += SIZEOF(_SlotHeader) + SIZEOF(DRM_DWORD);
    }
    else
    {
        pChild->nNextSlotPos += SIZEOF(_SlotHeader) + oHeader.dwSlotSize;
    }

    pSlotCtx->eStatus = eSlotCtxReady;
    *pfResult = TRUE;
ErrorExit:
    if( pbBlockBuffer != NULL )
    {
        _Hds_free(pCurrBlock->pNS->pHDS, pbBlockBuffer);
        pbBlockBuffer = NULL;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**********************************************************************
** Function:    SearchSlotInFile
** Synopsis:
** Arguments:   [pHDS] --
** Returns:     DRM_SUCCESS on success
** Notes:
***********************************************************************
*/
static DRM_NO_INLINE DRM_RESULT _HdsSearchSlotInFile(
    __in            _NsContext        *pNS,
    __in      const DRM_DST_KEY       *pHashkey,
    __in_opt  const DRM_DST_KEY       *pUniquekey,
    __in            _ESlotSearchMode   eSearchMode,
    __out_opt       DRM_DWORD         *pcbSlotSize, /* current dataslot size, can be NULL */
    __out           _SlotContext      *pSlotCtx,    /* user given Dataslot context buffer */
    __out           DRM_BOOL          *pfResult )
{
    DRM_RESULT     dr=DRM_SUCCESS;
    _CommBlockHDR *pParentBlock=NULL;
    DRM_DWORD      dwChildIndex=0;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsSearchSlotInFile);

    DRMASSERT( pNS && pHashkey && pSlotCtx && pfResult );
    __analysis_assume( pNS != NULL );
    __analysis_assume( pHashkey != NULL );

    pSlotCtx->nBlockWithLargestFreeSpace = 0;
    pSlotCtx->dwLargestSpace = 0;
    while (TRUE)
    {
        /* locate a child block which might contain the slot */
        *pfResult = FALSE;
        ChkDR(_HdsHashToChildBlock(pNS, pParentBlock, pHashkey->rgb,
            &pSlotCtx->pCurrChildBlock, pfResult, &dwChildIndex));
        pSlotCtx->eStatus = eSlotCtxInited;
        if ( !*pfResult )
        {
            break;
        }

#ifdef _CHKHDS_
        if ( pParentBlock!=NULL )
        {
            printf("\t[Hashing]\tBlock%5d - Alloctable[%d]\n",
                pParentBlock->nBlockNum, dwChildIndex);
        }
#endif
        /* cache the result of this search path for later use */
        if ( ((_ChildBlockHDR*)pSlotCtx->pCurrChildBlock)->File._image.nFreeMem > pSlotCtx->dwLargestSpace )
        {
            pSlotCtx->nBlockWithLargestFreeSpace = pSlotCtx->pCurrChildBlock->nBlockNum;
            pSlotCtx->dwLargestSpace = ((_ChildBlockHDR*)pSlotCtx->pCurrChildBlock)->File._image.nFreeMem;
        }

        /* search the possible slot in the block */
        ChkDR(_HdsSearchSlotInBlock(pSlotCtx->pCurrChildBlock, pHashkey, pUniquekey,
            eSearchMode, pSlotCtx, pfResult));

        if ( *pfResult )
        {
            if ( pcbSlotSize )
            {
                *pcbSlotSize = pSlotCtx->oSlotHeader.dwSlotSize;
            }
#ifdef _CHKHDS_
            printf("\t[Found in]\tBlock%5d\n", pSlotCtx->pCurrChildBlock->nBlockNum);
#endif
            break;
        }

        /* keep digging along the hash path */
        if ( pParentBlock == NULL )
        {
            /* allocate buffer for the process */
            ChkDR(DRM_HDSIMPL_AllocBlockBuffer(pNS, eCHILDBLOCK, &pParentBlock));
        }
        _HdsCopyBlockBuffer(pParentBlock, pSlotCtx->pCurrChildBlock);
    }

ErrorExit:
    if ( pParentBlock )
    {
        _Hds_free(pNS->pHDS, pParentBlock);
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**********************************************************************
** Function:    _HdsAllocSlotInFile
** Synopsis:    Allocate a slot of specified size in the file.
** Arguments:   [pHDS] --
**              [pbLicense] --
**              [cbLicense] --size must be at least the size of DRM_LICENSE_V1
** Returns:     DRM_SUCCESS on success
** Notes:
***********************************************************************
*/
static DRM_NO_INLINE DRM_RESULT _HdsAllocSlotInFile(
    __in        _NsContext     *pNS,
    __in  const DRM_BYTE        bHashKey   [__CB_DECL(DRM_HDS_HASHKEY_SIZE)],
    __in  const DRM_BYTE        bUniqueKey [__CB_DECL(DRM_HDS_UNIQUEKEY_SIZE)],
    __in        DRM_BOOL        fIsDataSlot,
    __in        DRM_DWORD       cbSlot,     /* current dataslot size */
    __out       _SlotContext   *pSlotCtx )  /* user given Dataslot context buffer */
{
    DRM_RESULT      dr=DRM_SUCCESS;
    DRM_DWORD       dwPhysicalSize=0, dwSlotPos=0;
    _ChildBlockHDR *pChild=NULL;
    _SlotHeader     oHeader = { 0 };

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsAllocSlotInFile);

    DRMASSERT(pNS && bHashKey && bUniqueKey && pNS->fInited && cbSlot && pSlotCtx);
    __analysis_assume( pNS != NULL );

    /* compute the space needed in _ChildBlockHDR */
    if ( ISOVERSIZESLOT(cbSlot, pNS) )
    {
        dwPhysicalSize = SIZEOF(_SlotHeader)+SIZEOF(DRM_DWORD);
    }
    else
    {
        dwPhysicalSize = SIZEOF(_SlotHeader)+cbSlot;
    }

    /***************************************************************************
    **** Note: exclusive access to SRN guard the concurrency of this action. ***
    **** Caller of this function is expect to lock SRN for exclusive access. ***
    ****************************************************************************
    */

    /* locate a _ChildBlock for this slot */
    ChkDR(_HdsLocateFreeBlockForSlot(pNS, bHashKey, dwPhysicalSize, pSlotCtx, DRM_UI64( 0 ) ));
    pChild = (_ChildBlockHDR*)(pSlotCtx->pCurrChildBlock);

    /* init slot content */
    dwSlotPos = pNS->nImagesize_ChildBlockPayload -pChild->File._image.nFreeMem;
    oHeader.dwSlotSize = cbSlot;
    MEMCPY(oHeader.oHashkey.rgb, bHashKey, DRM_HDS_HASHKEY_SIZE);
    MEMCPY(oHeader.oUniquekey.rgb, bUniqueKey, DRM_HDS_UNIQUEKEY_SIZE);
    if( !fIsDataSlot )
    {
        oHeader.dwAttributes |= eSlotIsNamespace;
    }

    /* store _SlotHeader of the slot */
    FIX_ENDIAN_DWORD( oHeader.dwSlotSize );
    FIX_ENDIAN_DWORD( oHeader.dwAttributes );
    dr = _HdsGetPutBlockPayload(pSlotCtx->pCurrChildBlock, dwSlotPos, SIZEOF(_SlotHeader),
        (DRM_BYTE *)&oHeader, GPBP_OPERATION_WRITE);
    FIX_ENDIAN_DWORD( oHeader.dwSlotSize );
    FIX_ENDIAN_DWORD( oHeader.dwAttributes );
    ChkDR( dr );

    /* allocate _DataBlock if necessary */
    if ( !ISOVERSIZESLOT(cbSlot, pNS) )
    {
        pSlotCtx->pCurrDataBlock = NULL;
    }
    else
    {
        ChkDR(_HdsCreateOversizedSlot(pNS, pSlotCtx->pCurrChildBlock->nBlockNum, cbSlot,
            pSlotCtx->pCurrDataBlock));
        pSlotCtx->dwNumofDataBlockVisited = 0;
        pSlotCtx->dwFirstDataBlockNum = pSlotCtx->pCurrDataBlock->nBlockNum;

        /* store the datablock# to this slot */
        FIX_ENDIAN_DWORD( pSlotCtx->pCurrDataBlock->nBlockNum );
        dr = _HdsGetPutBlockPayload(pSlotCtx->pCurrChildBlock, dwSlotPos+SIZEOF(_SlotHeader),
            SIZEOF(DRM_DWORD), (DRM_BYTE *)&(pSlotCtx->pCurrDataBlock->nBlockNum), GPBP_OPERATION_WRITE);
        FIX_ENDIAN_DWORD( pSlotCtx->pCurrDataBlock->nBlockNum );
        ChkDR( dr );
    }

    /* init pSlotCtx */
    MEMCPY(&pSlotCtx->oFileBlock, &pChild->oBlockHeader, SIZEOF(_CommBlockHDR));
    MEMCPY(&pSlotCtx->oSlotHeader, &oHeader, SIZEOF(_SlotHeader));
    pSlotCtx->dwSlotPosInBlock = dwSlotPos;
    pSlotCtx->dwSeekPointer = 0;
    pSlotCtx->eLockMode = (DRM_DST_LOCKMODE)(eDRM_DST_LOCKEXCLUSIVE | eDRM_DST_LOCKWAIT);

    /* set the slot content to NULL. We are overloading the function here.  */
    /* oversize slot payload is always NULL by default */
    if ( !ISOVERSIZESLOT(cbSlot, pNS) )
    {
        _HdsRemoveSlot(pSlotCtx, eRemoveSlotSetNull);
        pSlotCtx->oSlotHeader.dwAttributes = 0;     /* _HdsRemoveSlot() sets dwAttributes to hidden, we have to revert it here. */
        pSlotCtx->dwSeekPointer = 0;                /* reset seek pointer */
    }

    ChkDR( _HdsAddHashToBloom( pNS, bHashKey, pChild ) );

    /* update freemem in block and write the block to file */
    pChild->File._image.nFreeMem -= dwPhysicalSize;
    ChkDR(_HdsWriteBlockHDR(pSlotCtx->pCurrChildBlock));

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*
 *
 * NAMESPACE CACHE FUNCTIONS
 *
 */

/* Note that we're thread safe under the
 * protection of the HDS critical section
 * for all these 3 functions.
 */

/* Deletes the specified namespace from the cache */
static DRM_NO_INLINE DRM_RESULT _HdsDeleteNSCacheEntry(
                    _HdsContext       *pHDS,
              const DRM_DST_NAMESPACE *pNamespace )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  i = 0;

    ChkArg( pHDS != NULL
        && pNamespace != NULL );

    for( i = 0; i < DRM_HDS_NAMESPACECACHE_MAX_ENTRIES; i++ )
    {
        if( (pHDS->namespaceCache[i].dwCacheFlags & DRM_HDS_NAMESPACECACHE_FLAG_VALID) == 0 )
        {
            continue;
        }

        /* The entry is valid, check if it's what we're looking for
        */
        if( MEMCMP( &pHDS->namespaceCache[i].nsEntry.bNSName, pNamespace->rgbOpaqueData, DRM_HDS_NSNAME_SIZE ) != 0 )
        {
            continue;
        }

        /* Got it, let's clear it
        */
        ZEROMEM( &pHDS->namespaceCache[i], SIZEOF( _NamespaceCacheEntry ) );
        break;
    }

ErrorExit:
    return dr;
}


/* Retrieves the specified namespace entry from the cache */
static DRM_NO_INLINE DRM_RESULT _HdsLookupNSCacheEntry(
                    _HdsContext       *pHDS,
              const DRM_DST_NAMESPACE *pNamespace,
                    _NSEntry         **pNsEntry)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  i = 0;

    ChkArg( pHDS != NULL
        && pNamespace != NULL
        && pNsEntry   != NULL );

    *pNsEntry = NULL;

    for( i = 0; i < DRM_HDS_NAMESPACECACHE_MAX_ENTRIES; i++ )
    {
        if( (pHDS->namespaceCache[i].dwCacheFlags & DRM_HDS_NAMESPACECACHE_FLAG_VALID) == 0 )
        {
            continue;
        }

        /* The entry is valid, check if it's what we're looking for
        */
        if( MEMCMP( &pHDS->namespaceCache[i].nsEntry.bNSName, pNamespace->rgbOpaqueData, DRM_HDS_NSNAME_SIZE ) != 0 )
        {
            continue;
        }

        /* Sweet, it's in the cache, let's use it.
        */
        *pNsEntry = &pHDS->namespaceCache[i].nsEntry;
        break;
    }

ErrorExit:
    return dr;
}

/* Adds the specified namespace entry to the cache */
static DRM_NO_INLINE DRM_RESULT _HdsAddNSCacheEntry(
    __in       _HdsContext       *pHDS,
    __in const _NSEntry          *pNsEntry )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  i = 0;
    DRM_DWORD  iFree = DRM_HDS_NAMESPACECACHE_MAX_ENTRIES;

    ChkArg( pHDS != NULL
        && pNsEntry   != NULL );

    for( i = 0; i < DRM_HDS_NAMESPACECACHE_MAX_ENTRIES; i++ )
    {
        if( (pHDS->namespaceCache[i].dwCacheFlags & DRM_HDS_NAMESPACECACHE_FLAG_VALID) != 0 )
        {
            /* If you are getting an assert here it means your namespace cache is
               too small to fit all the namespaces in the system. It is safe to remove
               this assert, but you will miss perf gains unless you increase the cache
               size. */
            DRMASSERT( !(i == DRM_HDS_NAMESPACECACHE_MAX_ENTRIES && iFree == DRM_HDS_NAMESPACECACHE_MAX_ENTRIES) );
            if( MEMCMP( &pHDS->namespaceCache[i].nsEntry.bNSName, pNsEntry->bNSName, DRM_HDS_NSNAME_SIZE ) != 0 )
            {
                continue;
            }
            else
            {
                /* Entry already exists */
                goto ErrorExit;
            }
        }

        if( iFree == DRM_HDS_NAMESPACECACHE_MAX_ENTRIES )
        {
            iFree = i;
        }
    }

    if( iFree < DRM_HDS_NAMESPACECACHE_MAX_ENTRIES )
    {
        MEMCPY( &pHDS->namespaceCache[iFree].nsEntry, pNsEntry, SIZEOF( _NSEntry ) );
        pHDS->namespaceCache[iFree].dwCacheFlags = DRM_HDS_NAMESPACECACHE_FLAG_VALID;
    }

ErrorExit:
    return dr;
}

PRAGMA_WARNING_PUSH             /* BEGIN: Disable warning 4189 (unreferenced local) for this function due to variable(s) being declared soley for calls to SIZEOF. */
PRAGMA_WARNING_DISABLE(4189)

/*
** Note: when pNamespace is NULL, it is the CFG of the namespace store.
*/
static DRM_NO_INLINE DRM_RESULT _HdsInitNsContext(
    __in            _HdsContext       *pHDS,
    __out           _NsContext        *pCfg,
    __in_opt  const DRM_DST_NAMESPACE *pNamespace,
    __in            DRM_DWORD          nCfgRootBlockNum,
    __in            DRM_WORD           wMaxNumChildren,
    __in            DRM_WORD           wBloomBytesM,
    __in            DRM_WORD           wBloomK )
{
    DRM_RESULT dr = DRM_SUCCESS;
    const _ChildBlockHDR *pChild = NULL; /* declared solely for SIZEOF below */

    DRMASSERT(pHDS && pHDS->fInited && pCfg && wMaxNumChildren > 0 && wMaxNumChildren <= DRM_HDS_MAXIMUM_CHILDREN );
    __analysis_assume( pHDS != NULL );

    ChkArg( wMaxNumChildren <= DRM_HDS_MAXIMUM_CHILDREN );

    /* initialize the computed info for this namespace in the context */
    ZEROMEM(pCfg, SIZEOF(_NsContext));
    pCfg->wContextSize = SIZEOF(_NsContext);
    pCfg->eContextSignature = eCfgContextSignature;
    pCfg->pHDS = pHDS;
    if ( pNamespace != NULL )
    {
        DRMSIZEASSERT( SIZEOF( pCfg->oNamespace ), SIZEOF( *pNamespace ) );
        MEMCPY( &pCfg->oNamespace, pNamespace, SIZEOF( *pNamespace ) );
    }

    pCfg->fInited                      = TRUE;
    pCfg->nCfgRootBlockNum             = nCfgRootBlockNum;
    pCfg->wMaxNumChildren              = wMaxNumChildren;
    pCfg->nImagesize_ChildAllocTable   = wMaxNumChildren * pHDS->oSRN.eBlockNumType;
    pCfg->nImagesize_ChildBlockHDR     = SIZEOF(pChild->File._image.nFreeMem)
                                       + pCfg->nImagesize_ChildAllocTable
                                       + 1 /* bReserved */;

    if( pNamespace == NULL
     && pHDS->dwStoreVersion == DRM_HDS_FILE_VERSION_1 )
    {
        pCfg->nImagesize_ChildBlockHDR += HDS_CHILDBLOCK_PRERELEASE_BLOOM_M;
        pCfg->wBloomBytesM = HDS_CHILDBLOCK_PRERELEASE_BLOOM_M;
        pCfg->wBloomK = HDS_CHILDBLOCK_PRERELEASE_BLOOM_K;
    }
    else
    {
        /* only if we're not using an old HDS will there be Bloom data */
        pCfg->nImagesize_ChildBlockHDR += wBloomBytesM;
        pCfg->wBloomBytesM = wBloomBytesM;
        pCfg->wBloomK = wBloomK;
    }

    pCfg->nChildBlockPayloadPos        = pHDS->nImagesize_FileBlockHDR + pCfg->nImagesize_ChildBlockHDR;
    pCfg->nImagesize_ChildBlockPayload = pHDS->oSRN.dwBlockSize - pCfg->nChildBlockPayloadPos;

    /* Not enough room in each block for any data */
    ChkBOOL((pHDS->oSRN.dwBlockSize > pCfg->nChildBlockPayloadPos), DRM_E_HDSINVALIDSTORE);

ErrorExit:
    return dr;
}
PRAGMA_WARNING_POP  /* END: Disable warning 4189 (unreferenced local) for this function due to variable(s) being declared soley for calls to SIZEOF. */

/*
**
*/
static DRM_NO_INLINE DRM_RESULT _HdsInitSlotContext(
    __in                        _NsContext *pNS,
    __out_ecount(cbSlotContext) DRM_BYTE   *pbSlotContext,    /* user given Dataslot context buffer */
    __in                        DRM_DWORD   cbSlotContext)    /* Dataslot context size */
{
    DRM_RESULT dr=DRM_SUCCESS;
    _SlotContext *pSlotCtx=NULL;

    DRMASSERT(pNS && pbSlotContext && cbSlotContext);
    __analysis_assume( pbSlotContext != NULL );

    ZEROMEM(pbSlotContext, cbSlotContext);

    pSlotCtx = (_SlotContext*)pbSlotContext;

    pSlotCtx->eContextSignature = eSlotContextSignature;
    pSlotCtx->pNS = pNS;
    pSlotCtx->eStatus = eSlotCtxInited;
    pSlotCtx->dwContextSize = cbSlotContext;
    pSlotCtx->pCurrChildBlock = (_CommBlockHDR *)pSlotCtx->bBuff;
    pSlotCtx->pCurrDataBlock  = (_CommBlockHDR *) (pSlotCtx->bBuff + __CB_DECL(GetMemsize_ChildBlock(pNS)));
    ChkDR(_HdsInitBlockBuffer(pNS, pSlotCtx->pCurrChildBlock, 0, eCHILDBLOCK));
    ChkDR(_HdsInitBlockBuffer(pNS, pSlotCtx->pCurrDataBlock, 0, eDATABLOCK));

ErrorExit:
    return dr;
}

/*
** Create or open dataslot using given key1 and key2
*/
static DRM_NO_INLINE DRM_RESULT _HdsOpenSlot(
    __in       _NsContext        *f_pNS,
    __in const DRM_DST_KEY       *f_pHashKey,
    __in const DRM_DST_KEY       *f_pUniqueKey,
    __in       DRM_BOOL           f_fIsDataSlot,
    __in       DRM_DWORD          f_dwMode,
    __in       DRM_BOOL           f_fSRNNotAlreadyExclusivelyLocked,
    __inout    DRM_DWORD         *f_pcbSlotSize,
    __out      _SlotContext      *f_pSlotCtx )
{
    DRM_RESULT dr      = DRM_SUCCESS;
    DRM_BOOL   fResult = FALSE;
    DRM_DWORD  cbSlot  = 0;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsOpenSlot);

    DRMASSERT( (( f_dwMode & eDRM_DST_CREATE_NEW ) != 0)
            ^ (( f_dwMode & eDRM_DST_OPEN_EXISTING ) != 0)
            ^ (( f_dwMode & eDRM_DST_OPEN_ALWAYS   ) != 0 ) );

    ChkArg(ISVALIDCONTEXT(f_pNS, eCfgContextSignature)
        && f_pHashKey != NULL
        && !DRM_UTL_IsZeros(f_pHashKey->rgb, DRM_HDS_HASHKEY_SIZE));

    DRMASSERT(f_pNS->fInited          == TRUE
           && f_pNS->pHDS             != NULL
           && f_pNS->pHDS->fp         != OEM_INVALID_HANDLE_VALUE
           && f_pSlotCtx              != NULL
           && f_pUniqueKey            != NULL
           && f_pNS->nCfgRootBlockNum  > 0);

    /* Save the slot size they gave us in case _HdsSearchSlotInFile overwrites it */
    cbSlot = *f_pcbSlotSize;

    ChkDR(_HdsSearchSlotInFile(f_pNS, f_pHashKey, f_pUniqueKey,
        f_fIsDataSlot? eSearchDataSlot : eSearchNamespace, f_pcbSlotSize, f_pSlotCtx, &fResult));
    if ( !fResult )
    {
        if( f_dwMode & eDRM_DST_OPEN_EXISTING )
        {
            *f_pcbSlotSize = cbSlot;
            ChkDR(DRM_E_DST_SLOT_NOT_FOUND);
        }

        if ( (f_dwMode & eDRM_DST_LOCKEXCLUSIVE) == 0 )
        {
            ChkDR(DRM_E_DST_EXCLUSIVE_LOCK_ONLY);
        }

        /* If they didn't already have the SRN exclusively locked, we can't create the slot.
        ** Their arguments are invalid since you can only OPEN_EXISTING if you don't
        ** have the SRN exlusively locked
        */
        if( f_fSRNNotAlreadyExclusivelyLocked )
        {
            ChkArg( FALSE );
        }

        *f_pcbSlotSize = cbSlot;
        ChkDR(_HdsAllocSlotInFile(f_pNS, f_pHashKey->rgb, f_pUniqueKey->rgb, f_fIsDataSlot, cbSlot, f_pSlotCtx));
        f_pSlotCtx->eLockMode = (DRM_DST_LOCKMODE)(f_dwMode & eDRM_DST_LOCKMODE_MASK);

        /* this will not be blocked because this is a new slot and we have SRN exclusive lock */
        ChkDR(_HdsLockSlot(f_pSlotCtx, f_dwMode & eDRM_DST_LOCKMODE_MASK));

        if( !(f_dwMode & eDRM_DST_CREATE_NEW) )
        {
            /* Return S_FALSE to indicate we created the slot */
            dr = DRM_S_FALSE;
        }
    }
    else
    {
        if( f_dwMode & eDRM_DST_CREATE_NEW )
        {
            *f_pcbSlotSize = cbSlot;
            ChkDR(DRM_E_DST_SLOT_EXISTS);
        }
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*
** Verify the given slot context to make sure the Child and Data block are consistent.
*/
static DRM_NO_INLINE DRM_RESULT _HdsVerifySlotContext(
    __inout         _SlotContext *pSlotCtx,
    __out_ecount(1) DRM_BOOL     *pfValid )
{
    DRM_RESULT dr=DRM_SUCCESS;
    _CommBlockHDR *pBlock=NULL;
    _CommBlockHDR *pCurrBlock = NULL;
    _SlotHeader slotHeader;
    _CommBlockHDR *pTmpBlock=NULL;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsVerifySlotContext);

    DRMASSERT(pSlotCtx && pSlotCtx->eStatus == eSlotCtxReady && pfValid);
    __analysis_assume( pSlotCtx != NULL );
    *pfValid = FALSE;

    /* load generic block header */
    pBlock = &pSlotCtx->oFileBlock;
    pCurrBlock = pSlotCtx->pCurrChildBlock;

    /* load generic block header */
    ChkDR(_HdsGetPutBlockHDR(pSlotCtx->pNS, pSlotCtx->oFileBlock.nBlockNum, &pBlock, GPBH_GENERIC_ONLY, GPBH_OPERATION_READ));

    /* verify the generic header */
    if ( DRM_UI64Eql( pSlotCtx->oFileBlock.File._image.ui64TimeStamp, pCurrBlock->File._image.ui64TimeStamp )
      && MEMCMP(pSlotCtx->oFileBlock.File._image.bBlockHash, pCurrBlock->File._image.bBlockHash, SIZEOF( pCurrBlock->File._image.bBlockHash ))==0 )
    {
       *pfValid = TRUE;
        goto ErrorExit;
    }

    /* the current block has been updated/written since last read */

    /* check if it is still a CHILD block or belong to the same parent */
    if ( GET_BYTE( &pSlotCtx->oFileBlock.File._image.bBlockType, 0 ) != GET_BYTE( &pCurrBlock->File._image.bBlockType, 0 )
      || pSlotCtx->oFileBlock.File._image.nParentBlockNum            != pCurrBlock->File._image.nParentBlockNum )
    {
        goto ReloadChildBlock;
    }

    /* load the child block in a tmp buffer */
    ChkDR(DRM_HDSIMPL_AllocBlockBuffer(pSlotCtx->pNS, eCHILDBLOCK, &pTmpBlock));
    ChkDR(_HdsGetPutBlockHDR(pSlotCtx->pNS, pSlotCtx->oFileBlock.nBlockNum, &pTmpBlock, GPBH_ANY_HEADER, GPBH_OPERATION_READ));

    /* try to load the current slot header from file */
    dr = _HdsGetPutBlockPayload(pTmpBlock, pSlotCtx->dwSlotPosInBlock,
        SIZEOF(_SlotHeader), &slotHeader, GPBP_OPERATION_READ);
    FIX_ENDIAN_DWORD( slotHeader.dwSlotSize );
    FIX_ENDIAN_DWORD( slotHeader.dwAttributes );
    if ( DRM_SUCCEEDED(dr) )
    {
        if (MEMCMP(&slotHeader, &pSlotCtx->oSlotHeader, SIZEOF(_SlotHeader))==0 )
        {
            /* the current slot is intact, update the block buffer and continue */
            ChkDR(_HdsGetPutBlockHDR(pSlotCtx->pNS, pSlotCtx->oFileBlock.nBlockNum, &pCurrBlock, GPBH_ANY_HEADER, GPBH_OPERATION_READ));
            *pfValid = TRUE;
            goto ErrorExit;
        }
    }

ReloadChildBlock:
{

#if DRM_SUPPORT_MULTI_THREADING
    /*
    ** This might happen if slot has changed since it was opened by this thread.
    */
    dr = DRM_S_FALSE;
    goto ErrorExit;
#else

    /* reload and initialize the slot context using the hashkey and uniquekey in
       current slot context */

    /*
    _NsContext *pNS = pSlotCtx->pNS;
    _SlotHeader oSlotHeader = pSlotCtx->oSlotHeader;
    DRM_DWORD dwSeekPointer = pSlotCtx->dwSeekPointer;
    _SlotContext *pSlotContext=NULL;

    ChkDR(_HdsOpenExistingSlot(pNS,&oSlotHeader.oHashkey, TRUE, &oSlotHeader.oUniquekey, NULL, pSlotCtx));

    pSlotCtx->dwSeekPointer = dwSeekPointer;
    *pfValid = TRUE;
    */
#endif
}

ErrorExit:
    if ( pTmpBlock )
    {
        _Hds_free(pSlotCtx->pNS->pHDS, pTmpBlock);
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*
**
*/
static DRM_NO_INLINE DRM_RESULT DRM_CALL _HdsReadWriteSlot(
    __in                                      _SlotContext *pSlotCtx,
    __in                                const DRM_DWORD     cbData,      /* count in bytes to read/write */
    __out_bcount_part( cbData, *pcbIO )       DRM_BYTE     *pbData,      /* buffer for read/write */
    __out_ecount_opt(1)                       DRM_DWORD    *pcbIO,       /* actual amount of count in bytes read/written */
    __in                                      DRM_BOOL      fGet )
{
    DRM_RESULT dr=DRM_SUCCESS;
    DRM_DWORD cbIO=0;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsReadWriteSlot);

    DRMASSERT(pSlotCtx && pSlotCtx->eStatus==eSlotCtxReady && pbData);
    __analysis_assume( pSlotCtx != NULL );

    if ( pcbIO != NULL )
    {
        *pcbIO = 0;
    }

    if ( !ISOVERSIZESLOT(pSlotCtx->oSlotHeader.dwSlotSize, pSlotCtx->pNS) )
    {
        /* calc the position relative to the _ChildBlockHDR */
        DRM_DWORD wPos = pSlotCtx->dwSlotPosInBlock + SIZEOF(_SlotHeader) + pSlotCtx->dwSeekPointer;
        if ( (pSlotCtx->dwSeekPointer + cbData) > pSlotCtx->oSlotHeader.dwSlotSize )
        {
            cbIO = pSlotCtx->oSlotHeader.dwSlotSize - pSlotCtx->dwSeekPointer;
        }
        else
        {
            cbIO = cbData;
        }

        ChkDR(_HdsGetPutBlockPayload(pSlotCtx->pCurrChildBlock, wPos, cbIO, pbData, fGet));
    }
    else    /* Slot data is contained in a chain of _DataBlockHDR's */
    {
        DRM_DWORD nRelativeSeekPtr=0;
        DRM_DWORD nBytesLeft=0;
        DRM_DWORD nNextBlockNum=0;
        DRM_BYTE *pBuff=NULL;

        /* calc which _DataBlockHDR we need to load that contains the position of seekpointer */
        DRM_DWORD nthBlock = pSlotCtx->dwSeekPointer / pSlotCtx->pNS->pHDS->nImagesize_DataBlockPayload;

        /* load the appropriate data block */
        if ( nthBlock > pSlotCtx->dwNumofDataBlockVisited )
        {
            while ( nthBlock > pSlotCtx->dwNumofDataBlockVisited )
            {
                nNextBlockNum = ((_DataBlockHDR*)pSlotCtx->pCurrDataBlock)->File._image.nCascadingBlockNum;
                ChkDR(_HdsLoadBlockHDR(pSlotCtx->pNS, nNextBlockNum, &pSlotCtx->pCurrDataBlock));
                ++pSlotCtx->dwNumofDataBlockVisited;
            }
        }
        else if ( nthBlock < pSlotCtx->dwNumofDataBlockVisited )
        {
            /* start from the first data block in the chain */
            nNextBlockNum = pSlotCtx->dwFirstDataBlockNum;
            ChkDR(_HdsLoadBlockHDR(pSlotCtx->pNS, nNextBlockNum, &pSlotCtx->pCurrDataBlock));
            pSlotCtx->dwNumofDataBlockVisited = 0;

            while (pSlotCtx->dwNumofDataBlockVisited < nthBlock )
            {
                nNextBlockNum = ((_DataBlockHDR*)(pSlotCtx->pCurrDataBlock))->File._image.nCascadingBlockNum;
                ChkDR(_HdsLoadBlockHDR(pSlotCtx->pNS, nNextBlockNum, &pSlotCtx->pCurrDataBlock));
                ++pSlotCtx->dwNumofDataBlockVisited;
            }
        }

        /* calc # of bytes to read/write */
        if ( (pSlotCtx->dwSeekPointer + cbData) > pSlotCtx->oSlotHeader.dwSlotSize )
        {
            cbIO = pSlotCtx->oSlotHeader.dwSlotSize - pSlotCtx->dwSeekPointer;
        }
        else
        {
            cbIO = cbData;
        }

        /* calc relative pos of dwSeekPointer in this _DataBlockHDR */
        nRelativeSeekPtr = pSlotCtx->dwSeekPointer - (nthBlock * pSlotCtx->pNS->pHDS->nImagesize_DataBlockPayload);
        pBuff = pbData;
        nBytesLeft=cbIO;

        /* read/write slot content to buffer */
        ChkDR(_HdsGetPutBlockPayload(pSlotCtx->pCurrDataBlock, nRelativeSeekPtr,
            min(nBytesLeft, pSlotCtx->pNS->pHDS->nImagesize_DataBlockPayload - nRelativeSeekPtr), pBuff, fGet));
        pBuff      += __CB_DECL(min(nBytesLeft, pSlotCtx->pNS->pHDS->nImagesize_DataBlockPayload - nRelativeSeekPtr));
        nBytesLeft -=           min(nBytesLeft, pSlotCtx->pNS->pHDS->nImagesize_DataBlockPayload - nRelativeSeekPtr);

        /* more data is contained along the _DataBlockHDR's chain */
        while (nBytesLeft > 0)
        {
            nNextBlockNum = ((_DataBlockHDR*)pSlotCtx->pCurrDataBlock)->File._image.nCascadingBlockNum;
            ChkDR(_HdsLoadBlockHDR(pSlotCtx->pNS, nNextBlockNum, &pSlotCtx->pCurrDataBlock));
            ++pSlotCtx->dwNumofDataBlockVisited;
            ChkDR(_HdsGetPutBlockPayload(pSlotCtx->pCurrDataBlock, 0,
                min(nBytesLeft, pSlotCtx->pNS->pHDS->nImagesize_DataBlockPayload), pBuff, fGet));

            /* check if we are done */
            if ( nBytesLeft <= pSlotCtx->pNS->pHDS->nImagesize_DataBlockPayload )
            {
                break;
            }

            pBuff      += __CB_DECL(pSlotCtx->pNS->pHDS->nImagesize_DataBlockPayload);
            nBytesLeft -=           pSlotCtx->pNS->pHDS->nImagesize_DataBlockPayload;
        }
    }

    pSlotCtx->dwSeekPointer += cbIO;
    if ( pcbIO )
    {
        *pcbIO = cbIO;
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

#define ADJUST_PAYLOAD_TO_END   0xFFFFFFFF

/*
** Adjust content of Child payload from SrcPos to DestPos,
** _ChildBlockHDR's nFreeMem is adjusted appropriately.
*/
static DRM_NO_INLINE DRM_RESULT _HdsAdjustChildPayload(
    __in _CommBlockHDR *f_pblockheader,
    __in DRM_DWORD      f_oFileDest,
    __in DRM_DWORD      f_oFileSrc,
    __in DRM_DWORD      f_cbMove )
{
    DRM_RESULT      dr        = DRM_SUCCESS;
    DRM_DWORD       oFileCurr = 0;
    DRM_DWORD       cbMove    = 0;
    DRM_DWORD       cb        = 0;
    DRM_BYTE       *pbBuffer  = NULL;
    _ChildBlockHDR *pblockheaderChild = (_ChildBlockHDR *) f_pblockheader;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsAdjustChildPayload);

    DRMASSERT(f_pblockheader!=NULL);
    __analysis_assume( f_pblockheader != NULL );

    /* nothing to do */

    if (f_oFileDest == f_oFileSrc)
    {
        goto ErrorExit;
    }

    ChkDR(_Hds_malloc(f_pblockheader->pNS->pHDS, DRM_HDS_COPY_BUFFER_SIZE, (DRM_VOID **) &pbBuffer));

    if (f_cbMove == ADJUST_PAYLOAD_TO_END)
    {
        cbMove = pblockheaderChild->nPayloadSize
               - pblockheaderChild->File._image.nFreeMem
               - f_oFileSrc;
    }
    else
    {
        cbMove = f_cbMove;
    }

    if (f_oFileSrc > f_oFileDest)   /* Shrinking the payload */
    {
        /* copy it */
        for (oFileCurr  = 0;
             oFileCurr  < cbMove;
             oFileCurr += min(cbMove - oFileCurr, DRM_HDS_COPY_BUFFER_SIZE))
        {
            cb = min(cbMove - oFileCurr, DRM_HDS_COPY_BUFFER_SIZE);

            ChkDR(_HdsGetPutBlockPayload(f_pblockheader,
                                         oFileCurr + f_oFileSrc,
                                         cb,
                                         pbBuffer,
                                         GPBP_OPERATION_READ));

            ChkDR(_HdsGetPutBlockPayload(f_pblockheader,
                                         oFileCurr + f_oFileDest,
                                         cb,
                                         pbBuffer,
                                         GPBP_OPERATION_WRITE));
        }

        /* wipe the space left behind */

        if (f_cbMove == ADJUST_PAYLOAD_TO_END)
        {
            DRM_DWORD oFileWipeEnd = pblockheaderChild->nPayloadSize
                                   - pblockheaderChild->File._image.nFreeMem;

            ZEROMEM(pbBuffer, DRM_HDS_COPY_BUFFER_SIZE);

            for (oFileCurr = f_oFileDest + cbMove;
                 oFileCurr < oFileWipeEnd;
                 oFileCurr += min(oFileWipeEnd - oFileCurr, DRM_HDS_COPY_BUFFER_SIZE))
            {
                ChkDR(_HdsGetPutBlockPayload(f_pblockheader, oFileCurr, min(oFileWipeEnd-oFileCurr, DRM_HDS_COPY_BUFFER_SIZE), pbBuffer, FALSE));
            }

            /* update nFreeMem in block .. only valid if we are moving the tail*/

            pblockheaderChild->File._image.nFreeMem += f_oFileSrc - f_oFileDest;
        }
    }
    else    /* enlarging the payload (wSrcPos < wDestPos) */
    {
        ChkArg(f_cbMove != ADJUST_PAYLOAD_TO_END);

        if (cbMove > (pblockheaderChild->nPayloadSize - f_oFileDest))
        {
            TRACE(("_ChildBlockHDR does not have enough room for the move.\n"));
            DRMASSERT(FALSE);
        }

        /* copy from the end */
        for (oFileCurr  = cbMove;
             oFileCurr  > 0;
             oFileCurr -= cb)
        {

            cb = min(oFileCurr, DRM_HDS_COPY_BUFFER_SIZE);

            ChkDR(_HdsGetPutBlockPayload(f_pblockheader,
                                         oFileCurr + f_oFileSrc - cb,
                                         cb,
                                         pbBuffer,
                                         GPBP_OPERATION_READ));

            ChkDR(_HdsGetPutBlockPayload(f_pblockheader,
                                         oFileCurr + f_oFileDest - cb,
                                         cb,
                                         pbBuffer,
                                         GPBP_OPERATION_WRITE));
        }

        /* wipe the enlarged area of the payload */
        cb = f_oFileDest - f_oFileSrc;

        ZEROMEM(pbBuffer, DRM_HDS_COPY_BUFFER_SIZE);

        for (oFileCurr = 0;
             oFileCurr < cb;
             oFileCurr += min(cb - oFileCurr, DRM_HDS_COPY_BUFFER_SIZE))
        {
            ChkDR(_HdsGetPutBlockPayload(f_pblockheader,
                                         oFileCurr + f_oFileSrc,
                                         min(cb - oFileCurr, DRM_HDS_COPY_BUFFER_SIZE),
                                         pbBuffer,
                                         GPBP_OPERATION_WRITE));
        }

        /* update nFreeMem in block */
        if (pblockheaderChild->File._image.nFreeMem > (f_oFileDest - f_oFileSrc))
        {
            pblockheaderChild->File._image.nFreeMem -= f_oFileDest - f_oFileSrc;
        }
        else
        {
            pblockheaderChild->File._image.nFreeMem = 0;
        }
    }

ErrorExit:
    if (pbBuffer != NULL)
    {
        _Hds_free(f_pblockheader->pNS->pHDS, pbBuffer);
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**********************************************************************
** Function:    FindLeftmostLeafBlock
** Synopsis:    find the leftmost block which does any children blocks
** Returns:     DRM_SUCCESS on success
** Notes:
***********************************************************************
*/
static DRM_NO_INLINE DRM_RESULT _HdsFindLeftmostLeafBlock(
    __in            _CommBlockHDR *pRefBlock,
    __out_ecount(1) DRM_DWORD     *pnLeafBlock )
{
    DRM_RESULT dr=DRM_SUCCESS;
    DRM_DWORD nChildBlkNum=0;
    DRM_DWORD nIndex=0;
    _CommBlockHDR *pTmpBlock=NULL;
    _CommBlockHDR *pCurrBlock=NULL;
    _NsContext *pNS=NULL;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsFindLeftmostLeafBlock);

    DRMASSERT(pRefBlock && pnLeafBlock);
    __analysis_assume( pRefBlock != NULL );
    __analysis_assume( pnLeafBlock != NULL );

    pNS = pRefBlock->pNS;
    *pnLeafBlock = pRefBlock->nBlockNum;  /* init to itself first */
    ChkDR(DRM_HDSIMPL_AllocBlockBuffer(pNS, eCHILDBLOCK, &pTmpBlock));
    pCurrBlock = pRefBlock;

    /* for loop for each level of the tree */
    while (TRUE)
    {
        /* find the first occupied slot at this level */
        for (nIndex=0; nIndex < pCurrBlock->pNS->wMaxNumChildren; nIndex++)
        {
            ChkDR(_HdsGetPutChildBlockNum(pNS, ((_ChildBlockHDR*)pCurrBlock)->File._image.bChildAllocTable,
                nIndex, &nChildBlkNum, TRUE));
            if ( nChildBlkNum != 0 )  /* child node exists */
            {
                if ( pCurrBlock == pRefBlock )
                {
                    pCurrBlock = pTmpBlock;
                }

                /* load the child block */
                ChkDR(_HdsLoadBlockHDR(pNS, nChildBlkNum, &pCurrBlock));
                *pnLeafBlock = pCurrBlock->nBlockNum;
                break;
            }
        }

        /* no occupied slot found in this level, that means this block is a leaf block */
        if ( nIndex >= pCurrBlock->pNS->wMaxNumChildren )
        {
            break;
        }
    }

ErrorExit:
    if ( pTmpBlock )
    {
        _Hds_free(pNS->pHDS, pTmpBlock);
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*
**
*/
static DRM_NO_INLINE DRM_RESULT _HdsCopySlot_Child2Child(
    __in _CommBlockHDR *pDestBlock,
    __in DRM_DWORD      dwPosInDestBlock,
    __in _CommBlockHDR *pSourceBlock,
    __in DRM_DWORD      dwPosInSrcBlock,
    __in DRM_DWORD      dwSize )
{
    DRM_RESULT dr=DRM_SUCCESS;
    _NsContext *pNS=NULL;
    DRM_DWORD nIndex;
    DRM_BYTE *pCopyBuffer=NULL;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsCopySlot_Child2Child);

    DRMASSERT(pDestBlock && pSourceBlock
        && GET_BYTE( &pDestBlock->File._image.bBlockType, 0 )   == (DRM_BYTE)eCHILDBLOCK
        && GET_BYTE( &pSourceBlock->File._image.bBlockType, 0 ) == (DRM_BYTE)eCHILDBLOCK);
    __analysis_assume( pSourceBlock != NULL );

    pNS = pSourceBlock->pNS;

    ChkDR(_Hds_malloc(pNS->pHDS, DRM_HDS_COPY_BUFFER_SIZE, (DRM_VOID**)&pCopyBuffer));
    for (nIndex=0; nIndex < dwSize; )
    {
        ChkDR(_HdsGetPutBlockPayload(pSourceBlock, nIndex+dwPosInSrcBlock,  min(DRM_HDS_COPY_BUFFER_SIZE, dwSize-nIndex), pCopyBuffer, GPBP_OPERATION_READ));
        ChkDR(_HdsGetPutBlockPayload(pDestBlock,   nIndex+dwPosInDestBlock, min(DRM_HDS_COPY_BUFFER_SIZE, dwSize-nIndex), pCopyBuffer, GPBP_OPERATION_WRITE));

        nIndex += min(DRM_HDS_COPY_BUFFER_SIZE, dwSize-nIndex);
    }

ErrorExit:
    if ( pCopyBuffer != NULL )
    {
        _Hds_free(pNS->pHDS, pCopyBuffer);
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*
** Copy the payload of the source child block to the destination child block
*/
static DRM_NO_INLINE DRM_RESULT _HdsCopyChildPayload(
    __out _CommBlockHDR *pDestBlock,
    __in  _CommBlockHDR *pSourceBlock )
{
    DRM_RESULT dr=DRM_SUCCESS;
    DRM_DWORD nBytesToSkip=0;
    DRM_DWORD nNextSlotPos=0, nBytesToSearch=0;
    _SlotHeader oHeader;
    _CommBlockHDR *pDataBlock=NULL;
    _ChildBlockHDR *pChildDest = (_ChildBlockHDR*)pDestBlock;
    const _ChildBlockHDR *pChildSrc = (const _ChildBlockHDR*)pSourceBlock;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsCopyChildPayload);

    DRMASSERT(pDestBlock && pSourceBlock);
    __analysis_assume( pDestBlock != NULL );
    __analysis_assume( pSourceBlock != NULL );

    /* copy payload */
    ChkDR(_HdsCopySlot_Child2Child(pDestBlock, 0, pSourceBlock, 0,
        pDestBlock->pNS->nImagesize_ChildBlockPayload));

    /* update freemem */
    pChildDest->File._image.nFreeMem = pChildSrc->File._image.nFreeMem;

    /* copy Bloom filter data */
    MEMCPY( pChildDest->File._image.bBloomData, pChildSrc->File._image.bBloomData, SIZEOF( pChildSrc->File._image.bBloomData ) );

    /* walk each slot in the pDestBlock, adopt, if any, all data blocks to pDestBlock */
    nBytesToSearch = pDestBlock->pNS->nImagesize_ChildBlockPayload - ((_ChildBlockHDR*)pDestBlock)->File._image.nFreeMem;
    if ( nBytesToSearch == 0 )
    {
        goto ErrorExit;
    }

    /* walk the payload of child node to find matching HashKey and UniqueKey. Each license are stored
    ** one by one in the payload ...
    */
    ChkDR(DRM_HDSIMPL_AllocBlockBuffer(pSourceBlock->pNS, eDATABLOCK, &pDataBlock));
    for (; nNextSlotPos<nBytesToSearch; nNextSlotPos+=nBytesToSkip)
    {
        /* read the _SlotHeader structure */
        ChkDR(_HdsGetPutBlockPayload(pDestBlock, nNextSlotPos,
            SIZEOF(_SlotHeader), (DRM_BYTE*)&oHeader, GPBP_OPERATION_READ));
        FIX_ENDIAN_DWORD( oHeader.dwSlotSize );
        FIX_ENDIAN_DWORD( oHeader.dwAttributes );

        /* skip this license if not oversized */
        if ( !ISOVERSIZESLOT(oHeader.dwSlotSize, pDestBlock->pNS) )
        {
            nBytesToSkip = SIZEOF(_SlotHeader) + oHeader.dwSlotSize;
        }
        else
        {
            /* load the data block, update its parent block # to pDestBlock's block # */
            DRM_DWORD dwDataBlockNum=0;
            ChkDR(_HdsGetPutBlockPayload(pDestBlock, nNextSlotPos+SIZEOF(_SlotHeader),
                SIZEOF(DRM_DWORD), (DRM_BYTE*)&dwDataBlockNum, GPBP_OPERATION_READ));
            FIX_ENDIAN_DWORD( dwDataBlockNum );
            ChkDR(_HdsLoadBlockHDR(pSourceBlock->pNS, dwDataBlockNum, &pDataBlock));

            /* update parent block # */
            pDataBlock->File._image.nParentBlockNum = pDestBlock->nBlockNum;
            ChkDR(_HdsWriteBlockHDR(pDataBlock));

            nBytesToSkip = SIZEOF(_SlotHeader)+SIZEOF(DRM_DWORD);
        }
    }

ErrorExit:
    if (pDataBlock)
    {
        _Hds_free(pSourceBlock->pNS->pHDS, pDataBlock);
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/**********************************************************************
** Function:    _HdsDefragmentFile
** Synopsis:    Defragment the file from the given block
** Arguments:   [pRefBlock] --
** Returns:     DRM_SUCCESS on success
** Notes:       We cannot simply free a block even if it's empty. If we do so, the
**              structure of its subtree will be modified and all of the licenses stored
**              in this subtree will no longer be able to be retrieved. Instead, we
**              traverse to the leftmost leaf block of this subtree and move its
**              content to the given block. Then we can free that emptied leaf
**              block. This way, since the leaf block does not have any children,
**              its subtree can be freed.
***********************************************************************
*/
static DRM_NO_INLINE DRM_RESULT _HdsDefragmentFile(
    __in      _CommBlockHDR *pRefBlock,
    __out_opt DRM_BOOL      *pfRefBlockFreed )
{
    DRM_RESULT dr=DRM_SUCCESS;
    DRM_DWORD nParentBlock=0;
    DRM_DWORD nLeafBlock=0;
    DRM_DWORD nIndex=0;
    _CommBlockHDR *pLeafBlock=NULL;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsDefragmentFile);

    DRMASSERT(pRefBlock && pRefBlock->pNS && pRefBlock->pNS->fInited);
    __analysis_assume( pRefBlock != NULL );
    __analysis_assume( pRefBlock->pNS != NULL );

    if ( ((_ChildBlockHDR*)pRefBlock)->File._image.nFreeMem < (pRefBlock->pNS->nImagesize_ChildBlockPayload) )
    {
        goto ErrorExit;     /* this block is not empty! */
    }

    /* find the leftmost leaf node in the 'hash path' */
    ChkDR(DRM_HDSIMPL_AllocBlockBuffer(pRefBlock->pNS, eCHILDBLOCK, &pLeafBlock));
    ChkDR(_HdsFindLeftmostLeafBlock(pRefBlock, &nLeafBlock));

    if ( pfRefBlockFreed )
    {
        *pfRefBlockFreed = FALSE;
    }

    /* ref block is a leaf */
    if ( nLeafBlock == pRefBlock->nBlockNum )
    {
        if ( pRefBlock->File._image.nParentBlockNum == ROOTPARENTNUM )
        {
            goto ErrorExit;     /* This is the root node of namespace, leave it */
        }

        nParentBlock = pRefBlock->File._image.nParentBlockNum;
        _HdsFreeBlock(pRefBlock);
        if ( pfRefBlockFreed )
        {
            *pfRefBlockFreed = TRUE;
        }
        goto UpdateParentBlock;
    }

    ChkDR(_HdsLoadBlockHDR(pRefBlock->pNS, nLeafBlock, &pLeafBlock)); /* load the leaf block */
    ChkDR(_HdsCopyChildPayload(pRefBlock, pLeafBlock));    /* copy its content to the ref block */
    ChkDR(_HdsWriteBlockHDR(pRefBlock));  /* update ref block */

    /* get the parent block num of the leaf block and free the leaf block */
    nParentBlock = pLeafBlock->File._image.nParentBlockNum;
    _HdsFreeBlock(pLeafBlock);  /* delete the ref block in file */

UpdateParentBlock:

    /* update the leaf block's parent's alloctable so that it won't point to the leaf node */
    _HdsInitBlockBuffer(pRefBlock->pNS, pLeafBlock, 0, eCHILDBLOCK);
    ChkDR(_HdsLoadBlockHDR(pRefBlock->pNS, nParentBlock, &pLeafBlock));

    /* walk the allocTable to find the slot contains the leaf block # */
    for (nIndex=0; nIndex<pLeafBlock->pNS->wMaxNumChildren; nIndex++)
    {
        DRM_DWORD nChildBlkNum = 0;
        ChkDR(_HdsGetPutChildBlockNum(pRefBlock->pNS, ((_ChildBlockHDR*)pLeafBlock)->File._image.bChildAllocTable,
            nIndex, &nChildBlkNum, TRUE));
        if ( nChildBlkNum == nLeafBlock )
        {
            nChildBlkNum = 0;
            ChkDR(_HdsGetPutChildBlockNum(pRefBlock->pNS, ((_ChildBlockHDR*)pLeafBlock)->File._image.bChildAllocTable,
                nIndex, &nChildBlkNum, FALSE));

            ChkDR(_HdsWriteBlockHDR(pLeafBlock));
            goto ErrorExit;
        }
    }

    ChkDR(DRM_E_DST_CORRUPTED);     /* something must be wrong */

ErrorExit:
    _Hds_free(pRefBlock->pNS->pHDS, pLeafBlock);

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*
**
*/
static DRM_NO_INLINE DRM_RESULT _HdsSlotRead(
    __in                   _SlotContext *f_pSlotCtx,
    __in                   DRM_DWORD     f_cbData,
    __out_ecount(f_cbData) DRM_BYTE     *f_pbData,
    __out_ecount(1)        DRM_DWORD    *f_pcbRead )
{
    DRM_RESULT dr              = DRM_SUCCESS;
    DRM_BOOL   fSlotCtxIsValid = FALSE;

    ChkArg(ISVALIDCONTEXT(f_pSlotCtx, eSlotContextSignature));

    DRMASSERT(f_pSlotCtx->pNS           != NULL
           && f_pSlotCtx->pNS->pHDS     != NULL
           && f_pSlotCtx->pNS->pHDS->fp != OEM_INVALID_HANDLE_VALUE);

    ChkDR(_HdsVerifySlotContext(f_pSlotCtx, &fSlotCtxIsValid));
    if (f_pSlotCtx->eStatus!=eSlotCtxReady || !fSlotCtxIsValid)
    {
        ChkDR(DRM_E_DST_SLOT_NOT_FOUND);
    }
    ChkDR(_HdsReadWriteSlot(f_pSlotCtx, f_cbData, f_pbData, f_pcbRead, TRUE));

ErrorExit:
    return dr;
}


/*
**
*/
static DRM_NO_INLINE DRM_RESULT _HdsSlotWrite(
    __in                        _SlotContext  *f_pSlotCtx,
    __in                        DRM_DWORD      f_cbData,
    __in_ecount(f_cbData) const DRM_BYTE      *f_pbData,
    __out_ecount(1)             DRM_DWORD     *f_pcbWritten )
{
    DRM_RESULT dr              = DRM_SUCCESS;
    DRM_BOOL   fSlotCtxIsValid = FALSE;

    ChkArg(ISVALIDCONTEXT(f_pSlotCtx, eSlotContextSignature));

    DRMASSERT(f_pSlotCtx->pNS           != NULL
           && f_pSlotCtx->pNS->pHDS     != NULL
           && f_pSlotCtx->pNS->pHDS->fp != OEM_INVALID_HANDLE_VALUE );

    ChkDR(_HdsVerifySlotContext(f_pSlotCtx, &fSlotCtxIsValid));
    if (f_pSlotCtx->eStatus!=eSlotCtxReady || !fSlotCtxIsValid)
    {
        ChkDR(DRM_E_DST_SLOT_NOT_FOUND);
    }

    /* slot must be locked EXCLUSIVE */
    if ( (f_pSlotCtx->eLockMode & eDRM_DST_LOCKEXCLUSIVE) == 0 )
    {
        ChkDR(DRM_E_DST_NOT_LOCKED_EXCLUSIVE);
    }

    /* NOTE: We cast away the const-ness here because we know _HdsReadWriteSlot
    ** is safe when the operation is Write */
    ChkDR(_HdsReadWriteSlot(f_pSlotCtx, f_cbData, (DRM_BYTE*)f_pbData, f_pcbWritten, FALSE));

ErrorExit:
    return dr;
}


/**********************************************************************
** Function:    _HdsRemoveSlot
** Synopsis:    Remove the slot from the block. If it is oversized, the
**              associated _DataBlocks are also removed.
** Arguments:   [pSlotCtx] --
**              [fPermanent] -- TRUE if to remove the slot permanently,
**                              FALSE if just to set it as 'hidden'
** Returns:     DRM_SUCCESS on success
** Notes:       It is the callers responsibility to lock the block for
**              EXCLUSIVE.
***********************************************************************
*/
static DRM_NO_INLINE DRM_RESULT _HdsRemoveSlot(
    __in _SlotContext    *pSlotCtx,
    __in _ERemoveSlotMode eMode )
{
    DRM_RESULT dr=DRM_SUCCESS;
    DRM_DWORD   wDestPos=pSlotCtx->dwSlotPosInBlock;
    DRM_DWORD   wSrcPos=0;

    if ( eMode == eRemoveSlotPermanent )
    {
        /* remove _DataBlockHDR's, if necessary */
        if ( !ISOVERSIZESLOT(pSlotCtx->oSlotHeader.dwSlotSize, pSlotCtx->pNS) )
        {
            /* lock the associated block */
            wSrcPos = pSlotCtx->dwSlotPosInBlock + SIZEOF(_SlotHeader) +
                pSlotCtx->oSlotHeader.dwSlotSize;
        }
        else
        {
            DRM_DWORD nDataBlockNum = pSlotCtx->dwFirstDataBlockNum;

            wSrcPos = pSlotCtx->dwSlotPosInBlock + SIZEOF(_SlotHeader) + SIZEOF(DRM_DWORD);
            while (nDataBlockNum > 0)
            {
                ChkDR(_HdsLoadBlockHDR(pSlotCtx->pNS, nDataBlockNum, &pSlotCtx->pCurrDataBlock));
                nDataBlockNum = ((_DataBlockHDR*)(pSlotCtx->pCurrDataBlock))->File._image.nCascadingBlockNum;
                ChkDR(_HdsFreeBlock(pSlotCtx->pCurrDataBlock));
            }
        }

        /* remove slot in _ChildBlockHDR */
        ChkDR(_HdsAdjustChildPayload(pSlotCtx->pCurrChildBlock, wDestPos, wSrcPos, ADJUST_PAYLOAD_TO_END));
        ChkDR(_HdsWriteBlockHDR(pSlotCtx->pCurrChildBlock));
    }
    else if ( eMode == eRemoveSlotSetNull )
    {
        if ( !ISOVERSIZESLOT(pSlotCtx->oSlotHeader.dwSlotSize, pSlotCtx->pNS) )
        {
            /* trash content of slot to NULL */
            DRM_BYTE *pbCopyBuffer=NULL;
            DRM_DWORD  cbWrite=0, cbWritten=0;

            dr = _Hds_malloc(pSlotCtx->pNS->pHDS, DRM_HDS_COPY_BUFFER_SIZE, (DRM_VOID**)&pbCopyBuffer);
            if ( DRM_SUCCEEDED(dr) )
            {
                ChkMem( pbCopyBuffer );
                pSlotCtx->dwSeekPointer=0;
                while (pSlotCtx->dwSeekPointer < pSlotCtx->oSlotHeader.dwSlotSize )
                {
                    cbWrite = min(DRM_HDS_COPY_BUFFER_SIZE, pSlotCtx->oSlotHeader.dwSlotSize-pSlotCtx->dwSeekPointer);
                    dr = _HdsSlotWrite(pSlotCtx, cbWrite, pbCopyBuffer, &cbWritten);
                    if ( DRM_FAILED(dr) )
                    {
                        break;
                    }
                    else if ( cbWrite != cbWritten )
                    {
                        dr = DRM_E_FILE_WRITE_ERROR;
                        break;
                    }
                }

                _Hds_free(pSlotCtx->pNS->pHDS, pbCopyBuffer);
            }
        }
        else
        {
            /* delete all _DataBlock and set Datablock number to 0 in slot */
            DRM_DWORD nDataBlockNum = pSlotCtx->dwFirstDataBlockNum;

            /*wSrcPos = pSlotCtx->dwSlotPosInBlock + SIZEOF(_SlotHeader) + SIZEOF(DRM_DWORD);*/
            while (nDataBlockNum > 0)
            {
                ChkDR(_HdsLoadBlockHDR(pSlotCtx->pNS, nDataBlockNum, &pSlotCtx->pCurrDataBlock));
                nDataBlockNum = ((_DataBlockHDR*)(pSlotCtx->pCurrDataBlock))->File._image.nCascadingBlockNum;
                ChkDR(_HdsFreeBlock(pSlotCtx->pCurrDataBlock));
            }

            /* make sure the flag is hidden */
            pSlotCtx->oSlotHeader.dwAttributes |= eSlotIsHidden;
            FIX_ENDIAN_DWORD( pSlotCtx->oSlotHeader.dwAttributes );
            FIX_ENDIAN_DWORD( pSlotCtx->oSlotHeader.dwSlotSize );
            ChkDR( _HdsGetPutBlockPayload(pSlotCtx->pCurrChildBlock, pSlotCtx->dwSlotPosInBlock,
                                           SIZEOF(_SlotHeader), &pSlotCtx->oSlotHeader, GPBP_OPERATION_WRITE) );
            FIX_ENDIAN_DWORD( pSlotCtx->oSlotHeader.dwAttributes );
            FIX_ENDIAN_DWORD( pSlotCtx->oSlotHeader.dwSlotSize );

            /* update _DataBlock number in Slot */
            pSlotCtx->dwFirstDataBlockNum = 0;
            ChkDR( _HdsGetPutBlockPayload(pSlotCtx->pCurrChildBlock, pSlotCtx->dwSlotPosInBlock+SIZEOF(_SlotHeader),
                                          SIZEOF(DRM_DWORD), &pSlotCtx->dwFirstDataBlockNum, GPBP_OPERATION_WRITE));
        }
    }
    else if ( eMode == eRemoveSlotSetHidden )
    {
        /* set the flag to hidden */
        pSlotCtx->oSlotHeader.dwAttributes |= eSlotIsHidden;
        FIX_ENDIAN_DWORD( pSlotCtx->oSlotHeader.dwAttributes );
        FIX_ENDIAN_DWORD( pSlotCtx->oSlotHeader.dwSlotSize );
        ChkDR( _HdsGetPutBlockPayload(pSlotCtx->pCurrChildBlock, pSlotCtx->dwSlotPosInBlock,
                                      SIZEOF(_SlotHeader), &pSlotCtx->oSlotHeader, GPBP_OPERATION_WRITE));
        FIX_ENDIAN_DWORD( pSlotCtx->oSlotHeader.dwAttributes );
        FIX_ENDIAN_DWORD( pSlotCtx->oSlotHeader.dwSlotSize );
    }

ErrorExit:
    return dr;
}

/*
** upon success, ppNewSlotCtx contains a new slot. Caller is responsible
** for freeing the buffer. Upon success, the new slot will have the same lock
** as the source slot.
*/
static DRM_NO_INLINE DRM_RESULT _HdsCopyAndLockNewSlot(
    __in  _SlotContext  *pSlotCtx,
    __in  DRM_DWORD      cbNewSize,
    __out _SlotContext **ppNewSlotCtx)
{
    DRM_RESULT    dr=DRM_SUCCESS;
    DRM_DWORD     cbSlotContext=0;
    DRM_BYTE     *pbCopyBuffer=NULL;
    _HdsContext  *pHDS=NULL;
    _NsContext   *pNS=NULL;
    _SlotContext *pNewSlotCtx=NULL;
    _SlotHeader   oTmpHeader;
    DRM_DWORD     cbRead=0, cbWritten=0;

    DRMASSERT(pSlotCtx && ppNewSlotCtx);
    __analysis_assume( pSlotCtx != NULL );
    __analysis_assume( ppNewSlotCtx != NULL );

    pHDS = pSlotCtx->pNS->pHDS;
    pNS  = pSlotCtx->pNS;

    /* alloc copy buffer & context for new slot */
    ChkOverflowSLOTCONTEXTLEN(pNS);
    cbSlotContext = CALC_SLOTCONTEXTLEN(pNS);
    ChkDR(_Hds_malloc(pHDS, cbSlotContext, (DRM_VOID **)&pNewSlotCtx));
    ChkDR(_HdsInitSlotContext(pNS, (DRM_BYTE*)pNewSlotCtx, cbSlotContext));

    /* Copy key from original slot and create new slot with temp keys.
    ** if this succeed, an EXCLUSIVE lock is issued for the new slot */
    MEMCPY(&oTmpHeader, &pSlotCtx->oSlotHeader, SIZEOF(_SlotHeader));
    ChkDR(_HdsOpenSlot(
        pNewSlotCtx->pNS,
        &oTmpHeader.oHashkey,
        &oTmpHeader.oUniquekey,
        TRUE,
        (DRM_DWORD)( eDRM_DST_CREATE_NEW | pSlotCtx->eLockMode ),
        FALSE,
       &cbNewSize,
        pNewSlotCtx));

    /* copy content from original slot to new slot
    ** NOTE: This copy process can be enhanced for oversized slot:
    ** it may be more efficient to simple detach the head of DataBlock list
    ** to the new slot.
    */
    ChkDR(_Hds_malloc(pHDS, DRM_HDS_COPY_BUFFER_SIZE, (DRM_VOID**)&pbCopyBuffer));
    ChkMem( pbCopyBuffer );
    pSlotCtx->dwSeekPointer=0;
    pNewSlotCtx->dwSeekPointer=0;
    while (pNewSlotCtx->dwSeekPointer < min(cbNewSize, pSlotCtx->oSlotHeader.dwSlotSize))
    {
        dr = _HdsSlotRead(pSlotCtx, min(cbNewSize-pSlotCtx->dwSeekPointer, DRM_HDS_COPY_BUFFER_SIZE), pbCopyBuffer, &cbRead);
        if ( DRM_SUCCEEDED(dr) )
        {
            dr = _HdsSlotWrite(pNewSlotCtx, cbRead, pbCopyBuffer, &cbWritten);
            if ( DRM_SUCCEEDED(dr) )
            {
                if ( cbRead == cbWritten)
                {
                    continue;
                }
                else if (cbRead != DRM_HDS_COPY_BUFFER_SIZE)
                {
                    break;
                }
                else /* (cbRead != cbWritten) */
                {
                    dr = DRM_E_FILE_WRITE_ERROR;
                }
            }
        }
        break;
    }

    if (DRM_FAILED(dr))
    {
        _HdsUnlockSlot(pNewSlotCtx);  /* some error occured, unlock the slot */
    }

ErrorExit:
    if( pbCopyBuffer != NULL )
    {
        _Hds_free(pHDS, pbCopyBuffer);
    }
    if ( DRM_FAILED(dr)
      && pNewSlotCtx != NULL )
    {
        _Hds_free(pHDS, pNewSlotCtx);
    }
    else
    {
        *ppNewSlotCtx = pNewSlotCtx;
    }
    return dr;
}


/**********************************************************************
** Function:    _HdsRelocateSlotAndKeepCurrLock
** Synopsis:    Relocate a slot
** Arguments:   [pSlotCtx] -- Slot context of interest
**              [cbSize] -- new size
** Returns:     DRM_SUCCESS on success
** Notes:       The slot and SRN must be locked exclusive before calling,
**              but HDS CS must not be held.
***********************************************************************
*/
static DRM_NO_INLINE DRM_RESULT _HdsRelocateSlotAndKeepCurrLock(
    __in _SlotContext *pSlotCtx,
    __in DRM_DWORD     cbSize)
{
    DRM_RESULT    dr=DRM_SUCCESS;
    _SlotContext *pNewSlotCtx=NULL;
    _HdsContext  *pHDS=NULL;
    _NsContext   *pNS=NULL;
    DEFINE_LOCK_VARS

    DRMASSERT(pSlotCtx != NULL
          &&  pSlotCtx->pNS != NULL
          &&  pSlotCtx->pNS->pHDS != NULL
          &&  pSlotCtx->eStatus == eSlotCtxReady
          &&  cbSize > 0
          &&  (pSlotCtx->eLockMode & eDRM_DST_LOCKEXCLUSIVE) != 0 );

    __analysis_assume( pSlotCtx != NULL );
    __analysis_assume( pSlotCtx->pNS != NULL );
    __analysis_assume( pSlotCtx->pNS->pHDS != NULL );

    pHDS = pSlotCtx->pNS->pHDS;
    pNS  = pSlotCtx->pNS;

    ENTER_HDS_CS(pHDS);

    ChkDR(_HdsRemoveSlot(pSlotCtx, eRemoveSlotSetHidden));  /* set slot as 'hidden' */
    dr = _HdsCopyAndLockNewSlot(pSlotCtx, cbSize, &pNewSlotCtx);
    if ( DRM_SUCCEEDED(dr) )
    {
        DRM_BOOL fResult=FALSE;

        /* set old slot to null first */
        _HdsRemoveSlot(pSlotCtx, eRemoveSlotSetNull);

        /* remove old slot permanently if we can lock the block. It's okay to
        ** leave it as 'hidden'. we can pick it up when doing store cleanup. */
        _HdsUnlockSlot(pSlotCtx);       /* ignore return values */
        _HdsUnlockSlot(pNewSlotCtx);

        if ( DRM_SUCCEEDED(_HdsLockBlock2DeleteSlot(
                pSlotCtx,
                eDRM_DST_LOCKEXCLUSIVE | eDRM_DST_LOCKWAIT ) ) )
        {
            _HdsRemoveSlot(pSlotCtx, eRemoveSlotPermanent);
            _HdsUnlockBlock2DeleteSlot(pSlotCtx);
        }

        /* re-initialize the new slot. This is necessary because _HdsRemoveSlot()
        ** may move the location of the new slot
        */
        ChkOverflowSLOTCONTEXTLEN( pNS );
        ChkDR(_HdsInitSlotContext(pNS, (DRM_BYTE*)pSlotCtx, CALC_SLOTCONTEXTLEN(pNS)));

        ChkDR(_HdsSearchSlotInFile(pNS, &pNewSlotCtx->oSlotHeader.oHashkey,
            &pNewSlotCtx->oSlotHeader.oUniquekey, eSearchDataSlot, NULL, pSlotCtx, &fResult));
        DRMASSERT(fResult); /* this should not happen, something must be wrong */

        /* issue lock to the relocated slot */
        dr = _HdsLockSlot(pSlotCtx, eDRM_DST_LOCKEXCLUSIVE | eDRM_DST_LOCKWAIT);
    }
    else
    {
        /* try recovering ... try unhide the slot */
        pSlotCtx->oSlotHeader.dwAttributes &= ~eSlotIsHidden;

        /* we do not care if this success or not. that's all we can do ... */
        FIX_ENDIAN_DWORD( pSlotCtx->oSlotHeader.dwAttributes );
        FIX_ENDIAN_DWORD( pSlotCtx->oSlotHeader.dwSlotSize );
        _HdsGetPutBlockPayload(pSlotCtx->pCurrChildBlock, pSlotCtx->dwSlotPosInBlock,
            SIZEOF(_SlotHeader), &pSlotCtx->oSlotHeader, GPBP_OPERATION_WRITE);
        FIX_ENDIAN_DWORD( pSlotCtx->oSlotHeader.dwAttributes );
        FIX_ENDIAN_DWORD( pSlotCtx->oSlotHeader.dwSlotSize );

        /* Report the error from above */
        ChkDR( dr );
    }

ErrorExit:
    _Hds_free(pHDS, pNewSlotCtx);
    LEAVE_HDS_CS(pHDS);

    return dr;
}


/*
**
*/
static DRM_NO_INLINE DRM_RESULT _HdsDeleteSubTree(
    __in _NsContext *f_pNS,
    __in DRM_DWORD   f_nSubtreeRootBlockNum )
{
    DRM_RESULT     dr            = DRM_SUCCESS;
    DRM_DWORD      nIndex        = 0;
    _CommBlockHDR *pCurrBlock    = NULL;
    DRM_DWORD      nBlkNum       = f_nSubtreeRootBlockNum;
    DRM_DWORD      nParentBlkNum = 0;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsDeleteSubTree);

    ChkArg(ISVALIDCONTEXT(f_pNS, eCfgContextSignature));
    DRMASSERT(f_pNS->fInited == TRUE);

    ChkDR(DRM_HDSIMPL_AllocBlockBuffer(f_pNS, eCHILDBLOCK, &pCurrBlock));

UPLEVEL:

    _HdsInitBlockBuffer(f_pNS, pCurrBlock, 0, eCHILDBLOCK);
    ChkDR(_HdsLoadBlockHDR(f_pNS, nBlkNum, &pCurrBlock));
    if (nParentBlkNum == 0 )    /* this should be run the first time */
    {
        nParentBlkNum = pCurrBlock->nBlockNum;
        DRMASSERT(nParentBlkNum!=0);  /* the root block of any Config sub-tree should NOT have parent block num == 0 */
    }

DOWNLEVEL:

    for (nIndex=0; nIndex < pCurrBlock->pNS->wMaxNumChildren; nIndex++)
    {
        ChkDR(_HdsGetPutChildBlockNum(f_pNS, ((_ChildBlockHDR*)pCurrBlock)->File._image.bChildAllocTable,
            nIndex, &nBlkNum, TRUE));
        if ( nBlkNum != 0 )  /* child node exists */
        {
            /* update child alloc table */
            DRM_DWORD n=0;
            ChkDR(_HdsGetPutChildBlockNum(f_pNS, ((_ChildBlockHDR*)pCurrBlock)->File._image.bChildAllocTable,
                nIndex, &n, FALSE));
            ChkDR(_HdsWriteBlockHDR(pCurrBlock));

            /* load the child block */
            ChkDR(_HdsLoadBlockHDR(f_pNS, nBlkNum, &pCurrBlock));
            goto DOWNLEVEL;
        }
    }

    if ( pCurrBlock->nBlockNum == nParentBlkNum )   /* terminating condition */
    {
        ChkDR(_HdsFreeBlock(pCurrBlock));
        goto ErrorExit; /* this is the root */
    }

    nBlkNum = pCurrBlock->File._image.nParentBlockNum;
    ChkDR(_HdsFreeBlock(pCurrBlock));
    goto UPLEVEL;

ErrorExit:
    if ( f_pNS )
    {
        _Hds_free(f_pNS->pHDS, pCurrBlock);
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*
**
*/
static DRM_NO_INLINE DRM_RESULT _HdsTraverseNextRightSiblingBlock(
    __in        _NsContext    *pNS,
    __in  const _CommBlockHDR *pCurrBlock,
    __out       DRM_DWORD     *pnNextSiblingBlkNum )    /* 0 if no more child can be found */
{
    DRM_RESULT dr=DRM_SUCCESS;
    _CommBlockHDR *pParentBlock=NULL;
    DRM_DWORD nIndex=0;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsTraverseNextRightSiblingBlock);

    DRMASSERT(pNS && pCurrBlock && pnNextSiblingBlkNum);
    __analysis_assume( pNS != NULL );
    __analysis_assume( pCurrBlock != NULL );

    *pnNextSiblingBlkNum = 0;

    /* get ChildAllocTable of the parent node */
    if ( pCurrBlock->File._image.nParentBlockNum == ROOTPARENTNUM )
    {
        goto ErrorExit;     /* Currblock is the root, that's it */
    }

    ChkDR(DRM_HDSIMPL_AllocBlockBuffer(pNS, eCHILDBLOCK, &pParentBlock));
    ChkDR(_HdsLoadBlockHDR(pNS, pCurrBlock->File._image.nParentBlockNum, &pParentBlock));

    /* search index of the current child in parent's ChildAlloc table */
    for (nIndex=0; nIndex<(DRM_DWORD)pNS->wMaxNumChildren; nIndex++)
    {
        DRM_DWORD nChildNum = 0;
        ChkDR(_HdsGetPutChildBlockNum(pNS, ((_ChildBlockHDR*)pParentBlock)->File._image.bChildAllocTable,
            nIndex, &nChildNum, TRUE));

        if ( nChildNum == pCurrBlock->nBlockNum ) /* found current child */
        {
            /* search for next child */
            ++nIndex;
            for (; nIndex<(DRM_DWORD)pNS->wMaxNumChildren; nIndex++)
            {
                ChkDR(_HdsGetPutChildBlockNum(pNS, ((_ChildBlockHDR*)pParentBlock)->File._image.bChildAllocTable,
                    nIndex, &nChildNum, TRUE));
                if ( nChildNum!= 0 )    /* found one */
                {
                    *pnNextSiblingBlkNum = nChildNum;
                    break;
                }
            }
            goto ErrorExit;
        }
    }

    /* cannot find current child from parent, this should never happen */
    ChkDR(DRM_E_DST_CORRUPTED);  /* internal error */

ErrorExit:
    if (pParentBlock)
    {
        _Hds_free(pNS->pHDS, pParentBlock);
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**********************************************************************
** Function:    _HdsTraverseBlocksInPostOrder
** Synopsis:
** Arguments:   [pNS] --
**              [pCurrBlock] --
**              [pNextBlock] --
**              [pResult]
** Returns:     DRM_SUCCESS on success
** Notes:       From the given current block, traverse the tree in POST ORDER fashion.
** Algorithm:
**    PostOrder(CFG, Current, Next)
**    {
**        if ( Current == NULL )
**            Next = CFG->Root;
**        else
**        {
**            Next = NextRightSibling(Current);
**            if ( Next == NULL )
**            {
**                Next = Current->Parent;
**                if ( Next == NULL )
**                    return FALSE;  // that's it
**                else
**                    return TRUE;
**            }
**        }
**        Next = LeftmostLeaf(Next);
**        return TRUE;
**    }
**
***********************************************************************
*/
static DRM_NO_INLINE DRM_RESULT _HdsTraverseBlocksInPostOrder(
    __in            _NsContext    *pNS,
    __in_opt const  _CommBlockHDR *pCurrBlock,
    __out           _CommBlockHDR *pNextBlock,
    __out           DRM_BOOL      *pResult )
{
    DRM_RESULT dr=DRM_SUCCESS;
    DRM_DWORD nBlockNum=0;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDSIMPL, PERF_FUNC__HdsTraverseBlocksInPostOrder);

    DRMASSERT( pNS && pNextBlock && pResult);
    __analysis_assume( pNS != NULL );

    *pResult = FALSE;

    if ( pCurrBlock == NULL )   /* start from Root of current Namespace */
    {
        nBlockNum = pNS->nCfgRootBlockNum;
    }
    else
    {
        ChkDR(_HdsTraverseNextRightSiblingBlock(pNS, pCurrBlock, &nBlockNum));
        if ( nBlockNum == 0 )   /* no more sibling found */
        {
            /* no more sibling found, load the parent, if any, and return
            ** if no more sibling found and the parent is root, that's it
            */
            if ( pCurrBlock->File._image.nParentBlockNum == ROOTPARENTNUM )
            {
                *pResult = FALSE;   /* current block is the root */
            }
            else
            {   /* load parent block and return */
                ChkDR(_HdsLoadBlockHDR(pNS, pCurrBlock->File._image.nParentBlockNum,
                    &pNextBlock));
                *pResult = TRUE;
            }

            goto ErrorExit;
        }
    }

    ChkDR(_HdsLoadBlockHDR(pNS, nBlockNum, &pNextBlock));
    ChkDR(_HdsFindLeftmostLeafBlock(pNextBlock, &nBlockNum));
    if ( nBlockNum != pNextBlock->nBlockNum )
        ChkDR(_HdsLoadBlockHDR(pNS, nBlockNum, &pNextBlock));

    *pResult = TRUE;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*
**
*/
static DRM_NO_INLINE DRM_RESULT _HdsInitSlotEnum(
    __in            _NsContext       *pNS,
    __in_opt  const DRM_DST_KEY      *pHashKey,
    __in            DRM_DST_LOCKMODE  eLockMode,
    __out           _EnumContext     *pEnum )
{
    DRM_RESULT dr=DRM_SUCCESS;
    DRM_BOOL   fResult=FALSE;

    /* init Enum context */
    ZEROMEM(pEnum, CALC_MAXENUMCONTEXTLEN);
    pEnum->wContextSize = CALC_MAXENUMCONTEXTLEN;
    pEnum->pNS = pNS;
    pEnum->fInited = TRUE;
    pEnum->eContextSignature = eEnumContextSignature;
    pEnum->fCurrBlockIsValid = FALSE;
    pEnum->eLockMode = eLockMode;

    /* locate and prepare pEnum->oCurrBlock, if possible, for EnumNext() */
    ChkDR(_HdsInitBlockBuffer(pEnum->pNS, &pEnum->oCurrBlock, 0, eCHILDBLOCK));
    if ( pHashKey != NULL )     /* Hashkey is given, do query on the Hashkey */
    {
        DRM_DWORD      cbSlotCtx;
        _SlotContext *pSlotCtx=NULL;

        /* find the first block containing the Hashkey */
        pEnum->fIsQuery = TRUE;
        MEMCPY(pEnum->oHashKey.rgb, pHashKey->rgb, DRM_HDS_HASHKEY_SIZE);

        /* allocate temp slotcontext */
        ChkOverflowSLOTCONTEXTLEN(pNS);
        cbSlotCtx = CALC_SLOTCONTEXTLEN(pNS);
        ChkDR(_Hds_malloc(pNS->pHDS, cbSlotCtx, (DRM_VOID**)&pSlotCtx));
        dr = _HdsInitSlotContext(pNS, (DRM_BYTE *)pSlotCtx, cbSlotCtx);
        if ( DRM_SUCCEEDED(dr) )
        {
            /* search the first slot matching given hashkey */
            dr = _HdsSearchSlotInFile(pNS, pHashKey, NULL, eSearchSlotDontCare, NULL,
                pSlotCtx, &fResult);
            if ( DRM_SUCCEEDED(dr) )
            {
                if ( !fResult )
                {
                    dr = DRM_E_DST_SLOT_NOT_FOUND;
                }
                else
                {
                    dr = _HdsCopyBlockBuffer(&pEnum->oCurrBlock, pSlotCtx->pCurrChildBlock);
                    if ( DRM_SUCCEEDED(dr) )
                    {
                        ((_ChildBlockHDR*)(&pEnum->oCurrBlock))->nCurrSlotPos = MAXSLOTPOS;
                        ((_ChildBlockHDR*)(&pEnum->oCurrBlock))->nNextSlotPos = MAXSLOTPOS;
                    }
                }
            }
        }

        _Hds_free(pNS->pHDS, pSlotCtx);
        ChkDR(dr);
    }
    else
    {
        /* find the first block in postorder */
        pEnum->fIsQuery = FALSE;
        ChkDR(_HdsTraverseBlocksInPostOrder(pNS, NULL, &pEnum->oCurrBlock, &fResult));
        if ( !fResult )
        {
            ChkDR(DRM_E_DST_SLOT_NOT_FOUND);
        }
    }

    pEnum->fCurrBlockIsValid = TRUE;
    MEMCPY(&pEnum->oFileBlock, &pEnum->oCurrBlock, SIZEOF(_CommBlockHDR));

ErrorExit:
    return dr;
}

static DRM_NO_INLINE DRM_RESULT _HdsReadNsEntryFromDisk(
    __in  _HdsContext      *pHDS,
    __in  _SlotContext     *pNsSlotCtx,
    __in  DRM_DWORD         cbNsSlotSize,
    __out _NSEntry        **ppNsDiskEntry )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  cbRead = 0;

    ChkArg( pHDS != NULL );
    ChkArg( pNsSlotCtx != NULL );
    ChkArg( cbNsSlotSize > 0 );
    ChkArg( ppNsDiskEntry != NULL );

    *ppNsDiskEntry = NULL;
    ChkDR(_Hds_malloc(pHDS, cbNsSlotSize, (DRM_VOID**)ppNsDiskEntry));

    ChkDR(_HdsSlotRead(pNsSlotCtx, cbNsSlotSize, (DRM_BYTE *)*ppNsDiskEntry, &cbRead));

    ChkBOOL( cbRead == cbNsSlotSize, DRM_E_FILE_READ_ERROR );

    FIX_ENDIAN_WORD( (*ppNsDiskEntry)->wMaxNumChildren );
    FIX_ENDIAN_DWORD((*ppNsDiskEntry)->nNSRBlockNum );
    if( cbNsSlotSize >= SIZEOF( _NSEntry ) + DRM_HDS_NSNAME_SIZE )
    {
        /* The slot does have the Bloom parameters, so let's use them */
        FIX_ENDIAN_WORD( (*ppNsDiskEntry)->wBloomBytesM );
        FIX_ENDIAN_WORD( (*ppNsDiskEntry)->wBloomK );
    }
    else if( pHDS->dwStoreVersion == DRM_HDS_FILE_VERSION_0 )
    {
        /* There was no Bloom filter in this version of the HDS format */
        (*ppNsDiskEntry)->wBloomBytesM = 0;
        (*ppNsDiskEntry)->wBloomK = 0;
    }
    else if( pHDS->dwStoreVersion == DRM_HDS_FILE_VERSION_1 )
    {
        /* Default to original hardcoded Bloom values */
        (*ppNsDiskEntry)->wBloomBytesM = HDS_CHILDBLOCK_PRERELEASE_BLOOM_M;
        (*ppNsDiskEntry)->wBloomK = HDS_CHILDBLOCK_PRERELEASE_BLOOM_K;
    }
    else
    {
        ChkDR( DRM_E_DST_CORRUPTED );
    }
    ChkDR(_HdsValidateNSEntryFromDisk( pHDS, *ppNsDiskEntry ) );

ErrorExit:
    if( DRM_FAILED( dr )
     &&  ppNsDiskEntry != NULL
     && *ppNsDiskEntry != NULL )
    {
        _Hds_free( pHDS, *ppNsDiskEntry );
        *ppNsDiskEntry = NULL;
    }
    return dr;
}


/*
**
*/
static DRM_NO_INLINE DRM_RESULT _HdsCleanupNamespace(
    __in _NsContext *f_pNS)
{
    DRM_RESULT     dr         = DRM_SUCCESS;
    DRM_BOOL       fResult    = TRUE;
    _CommBlockHDR *pCurrBlock = NULL;
    _CommBlockHDR *pNextBlock = NULL;

    ChkArg(ISVALIDCONTEXT(f_pNS, eCfgContextSignature));
    DRMASSERT(f_pNS->fInited  == TRUE
           && f_pNS->pHDS     != NULL
           && f_pNS->pHDS->fp != OEM_INVALID_HANDLE_VALUE);

    ChkDR(DRM_HDSIMPL_AllocBlockBuffer(f_pNS, eCHILDBLOCK, &pCurrBlock));
    ChkDR(DRM_HDSIMPL_AllocBlockBuffer(f_pNS, eCHILDBLOCK, &pNextBlock));

    /* traverse the namespace in POST ORDER and process each block */
    ChkDR(_HdsTraverseBlocksInPostOrder(f_pNS, NULL,  pCurrBlock, &fResult));
    while (fResult)
    {
        DRM_BOOL fEmpty = FALSE;

        if ( ((_ChildBlockHDR*)pCurrBlock)->File._image.nFreeMem == f_pNS->nImagesize_ChildBlockPayload )
        {
            fEmpty = TRUE;
        }

        ChkDR(_HdsTraverseBlocksInPostOrder(f_pNS, pCurrBlock, pNextBlock, &fResult)); /* traverse next */
        if ( fEmpty )
        {
            /* perform defragment now.  note: pCurrBlock might get free'd */
            ChkDR(_HdsDefragmentFile(pCurrBlock, NULL));

            if( fResult )
            {
                /* Refresh the block allocation table in the next block as it could
                ** have changed during the defrag
                */
                ChkDR(_HdsLoadBlockHDR(f_pNS, pNextBlock->nBlockNum, &pNextBlock));
            }
        }
        if ( !fResult )
        {
            goto ErrorExit;  /* that's it */
        }

        /* If it was freed then the block type will have been changed to FREEBLOCK
        ** and will cause the CopyBlockBuffer function below to assert, so we need
        ** to just change it back to CHILDBLOCK.
        */
        pCurrBlock->File._image.bBlockType = eCHILDBLOCK;

        /* the block becomes the current block */
        ChkDR(_HdsCopyBlockBuffer(pCurrBlock, pNextBlock));
    }

ErrorExit:
    if ( pNextBlock )
    {
        _Hds_free(f_pNS->pHDS, pNextBlock);
    }
    if ( pCurrBlock )
    {
        _Hds_free(f_pNS->pHDS, pCurrBlock);
    }

    return dr;
}



/*
**
*/
static DRM_NO_INLINE DRM_RESULT _HdsCloseSlot(
    __in _SlotContext *f_pSlotCtx)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(ISVALIDCONTEXT(f_pSlotCtx, eSlotContextSignature));

    DRMASSERT(f_pSlotCtx->pNS           != NULL
           && f_pSlotCtx->pNS->pHDS     != NULL
           && f_pSlotCtx->pNS->pHDS->fp != OEM_INVALID_HANDLE_VALUE);

#if DBG
    /* trash the slot context and return */
    MEMSET(f_pSlotCtx, 0xdb, f_pSlotCtx->dwContextSize);
#endif

    /* to indicate the slot is closed */
    f_pSlotCtx->eStatus = eSlotCtxUninit;
ErrorExit:
    return dr;
}


/*
** Re-opens a slot context in order to acquire an SRN lock
**
** Lock preconditions: Must not already hold CS or SRN before calling
** When returns successfully caller will hold SRN lock only, on failure no locks will be held.
*/
static DRM_NO_INLINE DRM_RESULT _HdsReopenSlotWithSRNLock(
    __inout    _SlotContext              *pSlotCtx,
    __in       DRM_DWORD                  dwSRNLockMode,
    __in       DRM_DWORD                  dwSlotLockMode )
{
    DRM_RESULT    dr = DRM_SUCCESS;
    _NsContext   *pNS = NULL;
    DRM_BOOL      fSlotLocked = FALSE;
    DRM_BOOL      fValid      = FALSE;
    DRM_DST_KEY   hashKey     = {0};
    DRM_DST_KEY   uniqueKey   = {0};
    DRM_DST_SLOT_HINT slotHint = {0};
    DRM_DWORD     cbSlotSize  = 0;
    DEFINE_LOCK_VARS

    DRMASSERT( pSlotCtx != NULL );
    __analysis_assume( pSlotCtx != NULL );

    /* Save off the parameters of the slot to be re-opened */
    pNS = pSlotCtx->pNS;
    ChkDR( DRM_HDS_MakeSlotHint( (DRM_DST_SLOT_CONTEXT*)pSlotCtx, &slotHint ) );
    MEMCPY( &hashKey, &pSlotCtx->oSlotHeader.oHashkey, SIZEOF( DRM_DST_KEY ) );
    MEMCPY( &uniqueKey, &pSlotCtx->oSlotHeader.oUniquekey, SIZEOF( DRM_DST_KEY ) );

    /* Unlock and close the slot */
    ChkDR(_HdsUnlockSlot(pSlotCtx));
    ChkDR(_HdsCloseSlot(pSlotCtx));

    /* Re-initialize slot context structure */
    ChkDR(_HdsInitSlotContext(pNS, (DRM_BYTE*)pSlotCtx, CALC_MAXSLOTCONTEXTLEN));

    while ( TRUE )
    {
        /* Lock SRN EXCLUSIVE among other processes if we may have to create the slot,
        ** otherwise if we're only opening an existing slot we can lock SRN SHARED. */
        LOCK_SRN_AND_ENTER_CS(pNS->pHDS, dwSRNLockMode);


        /* Try opening the slot with the hint */
        dr = _HdsOpenSlotFromHint(pNS, &hashKey, &uniqueKey, (const _SlotHint*)&slotHint, TRUE, &cbSlotSize, pSlotCtx);


        /* If the slot hint open failed, try doing a regular open  */
        if( DRM_FAILED( dr ) )
        {
            ChkDR( _HdsOpenSlot(pNS,
                                &hashKey,
                                &uniqueKey,
                                TRUE,
                                dwSlotLockMode | eDRM_DST_OPEN_EXISTING,
                                TRUE,
                                &cbSlotSize,
                                pSlotCtx ) );
        }

        /* issue specific slot lock */
        ChkDR(_HdsLockSlot(pSlotCtx, dwSlotLockMode));
        fSlotLocked = TRUE;

        ChkDR(_HdsVerifySlotContext(pSlotCtx, &fValid));

        LEAVE_HDS_CS(pNS->pHDS);

        if ( fValid )
        {
            break;
        }

        UNLOCK_SRN(pNS->pHDS);

        /* slot context verified failed, try again */
        (void)_HdsUnlockSlot(pSlotCtx);
        fSlotLocked = FALSE;
    }

ErrorExit:
    if ( pNS != NULL  &&  pNS->pHDS != NULL )
    {
        if ( DRM_FAILED(dr) )
        {
            if( fSlotLocked )
            {
                /* error occured during verifying slot context, return error */
                _HdsUnlockSlot(pSlotCtx);
            }
            LEAVE_CS_AND_UNLOCK_SRN(pNS->pHDS);
        }
        else
        {
            LEAVE_HDS_CS(pNS->pHDS);
        }

    }

    return dr;
}




/**********************************************************************
** Function:    _HdsSlotEnumNext
** Synopsis:
** Arguments:   [pEnumContext] -- Enum Context init'd by DRM_HDS_InitSlotEnum()
**              [pSlotContext] -- Slot Context of the current slot
**              [pHashKey] -- Hashkey of the current slot
**              [pUniqueKey] -- Uniquekey of the current slot
**              [pcbSlotSize] -- size of the current slot
** Returns:     DRM_SUCCESS on success
** Notes:
***********************************************************************
*/
static DRM_NO_INLINE DRM_RESULT _HdsSlotEnumNext(
    __in      _EnumContext      *pEnum,
    __out     _SlotContext      *pSlotCtx,
    __out_opt DRM_DST_KEY       *pHashKey,
    __out_opt DRM_DST_KEY       *pUniqueKey,
    __out     DRM_DWORD         *pcbSlotSize )
{
    DRM_RESULT         dr = DRM_SUCCESS;
    DRM_BOOL           fResult=FALSE;
    const DRM_DST_KEY *pQueriedHashkey=NULL;
    _CommBlockHDR     *pNextBlock=NULL;
    _CommBlockHDR     *pTmpBlock=NULL;

    DRMASSERT(pEnum && pEnum->pNS);
    ChkArg(ISVALIDCONTEXT(pEnum, eEnumContextSignature));
    ChkArg(ISVALIDCONTEXT(pEnum->pNS, eCfgContextSignature));
    DRMASSERT(pEnum->pNS->fInited && pEnum->pNS->pHDS && pSlotCtx
        && pEnum->pNS->pHDS->fp!=OEM_INVALID_HANDLE_VALUE);

    /*
    ** Verify validity of the current block of the slot context
    */
    {
        _CommBlockHDR *pBlock = &pEnum->oFileBlock;
        _CommBlockHDR *pCurrBlock = &pEnum->oCurrBlock;

        /* load generic block header */
        ChkDR(_HdsGetPutBlockHDR(pEnum->pNS, pEnum->oFileBlock.nBlockNum, &pBlock,
            GPBH_GENERIC_ONLY, GPBH_OPERATION_READ));

        /* verify the generic header */
        if ( !DRM_UI64Eql(pEnum->oFileBlock.File._image.ui64TimeStamp, pCurrBlock->File._image.ui64TimeStamp) ||
           MEMCMP(pEnum->oFileBlock.File._image.bBlockHash, pCurrBlock->File._image.bBlockHash, SIZEOF(pCurrBlock->File._image.bBlockHash))!=0 )
        {
            /* the current block has been updated/written since last read */
            _SlotHeader slotHeader;
            DRM_DWORD   dwCurrPos = 0;

            /* check if it is still a CHILD block or belong to the same parent */
            if ( GET_BYTE( &pEnum->oFileBlock.File._image.bBlockType, 0 ) != GET_BYTE( &pCurrBlock->File._image.bBlockType, 0 )
              || pEnum->oFileBlock.File._image.nParentBlockNum            != pCurrBlock->File._image.nParentBlockNum )
            {
                /* there is nothing we can do when the current block is
                ** totally invalid
                */
                ChkDR(DRM_E_NOMORE);
            }

            /* check if query started yet */
            if ( ((_ChildBlockHDR*)pCurrBlock)->nNextSlotPos == MAXSLOTPOS )
            {
                /* query has not been started, reload the current block from file and keep on */
                ChkDR(_HdsGetPutBlockHDR(pEnum->pNS, pEnum->oFileBlock.nBlockNum,
                    &pCurrBlock, GPBH_ANY_HEADER, GPBH_OPERATION_READ));
                goto Verified_Okay;
            }

            /* load the child block in a tmp buffer */
            ChkDR(DRM_HDSIMPL_AllocBlockBuffer(pEnum->pNS, eCHILDBLOCK, &pTmpBlock));
            ChkDR(_HdsGetPutBlockHDR(pEnum->pNS, pEnum->oFileBlock.nBlockNum,
                &pTmpBlock, GPBH_ANY_HEADER, GPBH_OPERATION_READ));

            /* load the current slot header from file */
            ChkDR(_HdsGetPutBlockPayload(pTmpBlock, ((_ChildBlockHDR*)pCurrBlock)->nCurrSlotPos,
                SIZEOF(_SlotHeader), &slotHeader, GPBP_OPERATION_READ));
            FIX_ENDIAN_DWORD( slotHeader.dwAttributes );
            FIX_ENDIAN_DWORD( slotHeader.dwSlotSize );
            if (MEMCMP(&slotHeader, &pEnum->oSlotHeader, SIZEOF(_SlotHeader))==0 )
            {
                /* the current slot is intact, update the block buffer and continue */
                DRM_DWORD _dwCurrPos = ((_ChildBlockHDR*)pCurrBlock)->nCurrSlotPos;
                DRM_DWORD _dwNextPos = ((_ChildBlockHDR*)pCurrBlock)->nNextSlotPos;

                ChkDR(_HdsGetPutBlockHDR(pEnum->pNS, pEnum->oFileBlock.nBlockNum, &pCurrBlock, GPBH_ANY_HEADER, GPBH_OPERATION_READ));
                ((_ChildBlockHDR*)pCurrBlock)->nCurrSlotPos = _dwCurrPos;
                ((_ChildBlockHDR*)pCurrBlock)->nNextSlotPos = _dwNextPos;
                goto Verified_Okay;
            }

            /* walk the new block, try seek to >=  the current slot position. */
            dwCurrPos = ((_ChildBlockHDR*)pCurrBlock)->nCurrSlotPos;
            if ( pEnum->fIsQuery )
            {
                pQueriedHashkey = &pEnum->oHashKey;
            }

            ChkDR(_HdsInitSlotContext(pEnum->pNS, (DRM_BYTE*)pSlotCtx, CALC_MAXSLOTCONTEXTLEN));
            while (TRUE)
            {
                /* search current block at next postion */
                ChkDR(_HdsSearchSlotInBlock(pTmpBlock, pQueriedHashkey, NULL,
                    eSearchSlotDontCare, pSlotCtx, &fResult));
                if ( !fResult )
                {
                    /* the current block has less slots than before, restart query at next block, if any */
                    ChkDR(_HdsCopyBlockBuffer(&pEnum->oCurrBlock, pTmpBlock));
                    goto Verified_Okay;
                }
                if ( ((_ChildBlockHDR*)pTmpBlock)->nCurrSlotPos >= dwCurrPos )
                {
                    /* walk to position >= current slot position in old block. restart query from here in new block*/
                    ((_ChildBlockHDR*)pTmpBlock)->nNextSlotPos = ((_ChildBlockHDR*)pTmpBlock)->nCurrSlotPos;
                    ChkDR(_HdsCopyBlockBuffer(&pEnum->oCurrBlock, pTmpBlock));

                    goto Verified_Okay;
                }
            }
        }
    }

Verified_Okay:

    if ( pEnum->fIsQuery )
    {
        pQueriedHashkey = &pEnum->oHashKey;
    }

    ChkDR(_HdsInitSlotContext(pEnum->pNS, (DRM_BYTE*)pSlotCtx, CALC_MAXSLOTCONTEXTLEN));
    ChkDR(DRM_HDSIMPL_AllocBlockBuffer(pEnum->pNS, eCHILDBLOCK, &pNextBlock));
    while (TRUE)
    {
        /* search current block at next postion */
        ChkDR(_HdsSearchSlotInBlock(&pEnum->oCurrBlock, pQueriedHashkey, NULL,
            eSearchSlotDontCare, pSlotCtx, &fResult));
        if ( fResult )
        {
            if ( pHashKey )
            {
                MEMCPY(pHashKey->rgb, pSlotCtx->oSlotHeader.oHashkey.rgb, DRM_HDS_HASHKEY_SIZE);
            }
            if ( pUniqueKey )
            {
                MEMCPY(pUniqueKey->rgb, pSlotCtx->oSlotHeader.oUniquekey.rgb, DRM_HDS_UNIQUEKEY_SIZE);
            }
            if ( pcbSlotSize )
            {
               *pcbSlotSize = pSlotCtx->oSlotHeader.dwSlotSize;
            }

            pSlotCtx->eStatus = eSlotCtxReady;
            break;
        }

        /* keep digging */
        if ( pEnum->fIsQuery )
        {
            ChkDR(_HdsHashToChildBlock(pEnum->pNS, &pEnum->oCurrBlock, pEnum->oHashKey.rgb,
                &pNextBlock, &fResult, NULL));
        }
        else
        {
            ChkDR(_HdsTraverseBlocksInPostOrder(pEnum->pNS, &pEnum->oCurrBlock,
                pNextBlock, &fResult));
        }

        if ( !fResult )
        {
            /* no more block found */
            dr = DRM_E_NOMORE;
            goto ErrorExit;
        }

        /* the block becomes the current block */
        ChkDR(_HdsCopyBlockBuffer(&pEnum->oCurrBlock, pNextBlock));
    }

    MEMCPY(&pEnum->oFileBlock, &pEnum->oCurrBlock, SIZEOF(_CommBlockHDR));
    MEMCPY(&pEnum->oSlotHeader, &pSlotCtx->oSlotHeader, SIZEOF(_SlotHeader));

ErrorExit:

    if ( pNextBlock )
    {
        _Hds_free(pEnum->pNS->pHDS, pNextBlock);
    }
    if ( pTmpBlock )
    {
        _Hds_free(pEnum->pNS->pHDS, pTmpBlock);
    }
    return dr;
}



/**********************************************************************
** Function:    _HdsCleanupStore
** Synopsis:
** Arguments:   [pHDS] --
** Returns:     DRM_SUCCESS on success
** Notes:
***********************************************************************
*/
static DRM_NO_INLINE DRM_RESULT _HdsCleanupStore(
    __in _HdsContext *pHDS)
{
    DRM_RESULT            dr=DRM_SUCCESS;
    DRM_DWORD             cbEntry=0;
    _EnumContext         *pEnumContext=NULL;
    _NsContext           *pNsStoreCTX=NULL;
    _NsContext           *pNsCtx=NULL;
    DRM_DST_SLOT_CONTEXT *pSlotContext=NULL;
    DRM_DST_NAMESPACE    *pNamespace=NULL;
    _NSEntry             *pNsStoreEntry=NULL;

    /* no namespace store exist ==> no namespace found */
    DRMASSERT(pHDS && pHDS->oSRN.nNsStoreRootBlockNum>0);
    __analysis_assume( pHDS != NULL );

    /* allocate memory */
    ChkDR(_Hds_malloc(pHDS, DRM_HDS_ENUM_CONTEXT_LEN,      (DRM_VOID**)&pEnumContext));
    ChkDR(_Hds_malloc(pHDS, DRM_HDS_NAMESPACE_CONTEXT_LEN, (DRM_VOID**)&pNsStoreCTX));
    ChkDR(_Hds_malloc(pHDS, DRM_HDS_NAMESPACE_CONTEXT_LEN, (DRM_VOID**)&pNsCtx));
    ChkDR(_Hds_malloc(pHDS, SIZEOF(DRM_DST_SLOT_CONTEXT),  (DRM_VOID**)&pSlotContext));
    ChkDR(_Hds_malloc(pHDS, SIZEOF(DRM_DST_NAMESPACE),     (DRM_VOID**)&pNamespace));

    /* Enum each namespace, call cleanupNamespace */
    ChkDR(_HdsInitNsContext(pHDS, pNsStoreCTX, NULL, pHDS->oSRN.nNsStoreRootBlockNum,
        NsStoreNumofChildren, 0, 0));
    ChkDR(_HdsInitSlotEnum(pNsStoreCTX, NULL, eDRM_DST_LOCKSHARED, pEnumContext));
    while (TRUE)
    {
        dr = _HdsSlotEnumNext(pEnumContext, (_SlotContext*)pSlotContext, NULL, NULL, &cbEntry);
        if (dr == DRM_E_NOMORE)
        {
            dr = DRM_SUCCESS;
            break;
        }
        ChkDR(dr);

        ChkDR( _HdsReadNsEntryFromDisk( pHDS, (_SlotContext*)pSlotContext, cbEntry, &pNsStoreEntry ) );

        ChkDR(_HdsCloseSlot((_SlotContext*)pSlotContext));

        /* cleanup this namespace */
        MEMCPY(pNamespace->rgbOpaqueData, pNsStoreEntry->bNSName, DRM_HDS_NSNAME_SIZE);
        ChkDR(_HdsInitNsContext(pHDS, pNsCtx, pNamespace, pNsStoreEntry->nNSRBlockNum,
            pNsStoreEntry->wMaxNumChildren, pNsStoreEntry->wBloomBytesM, pNsStoreEntry->wBloomK));
        ChkDR(_HdsCleanupNamespace(pNsCtx));
        ChkDR(_Hds_free(pHDS, pNsStoreEntry));
        pNsStoreEntry = NULL;
    }

ErrorExit:
    /* free the local memory in reverse order */
    _Hds_free(pHDS, pNsStoreEntry);
    _Hds_free(pHDS, pNamespace);
    _Hds_free(pHDS, pSlotContext);
    _Hds_free(pHDS, pNsCtx);
    _Hds_free(pHDS, pNsStoreCTX);
    _Hds_free(pHDS, pEnumContext);
    return dr;
}


/**********************************************************************
** Function:    _HdsSlotResize
** Synopsis:
** Arguments:   [pSlotCtx]  -- slot context
**              [cbNewSize] -- new size to be used
** Returns:     DRM_SUCCESS on success
** Notes:       The resized slot will have the same type of lock as before.
***********************************************************************
*/
static DRM_RESULT _HdsSlotResize(
    __in _SlotContext *pSlotCtx,
    __in DRM_DWORD     cbNewSize)
{
    DRM_RESULT     dr=DRM_SUCCESS;
    DRM_DWORD      dwOldSize=0;

    DRMASSERT(pSlotCtx && cbNewSize && pSlotCtx->pNS && pSlotCtx->pNS->pHDS);
    __analysis_assume( pSlotCtx != NULL );

    dwOldSize = pSlotCtx->oSlotHeader.dwSlotSize;
    if ( cbNewSize == dwOldSize )    /* same size, do nothing */
    {
        goto ErrorExit;
    }

    /* slot must be locked EXCLUSIVE */
    if ( (pSlotCtx->eLockMode & eDRM_DST_LOCKEXCLUSIVE) == 0 )
    {
        ChkDR(DRM_E_DST_NOT_LOCKED_EXCLUSIVE);
    }

    /*************************************************************************
    ** all other cases will involve copying or moving slot content
    */
    ChkDR(_HdsRelocateSlotAndKeepCurrLock(pSlotCtx, cbNewSize));
    pSlotCtx->dwSeekPointer = 0;

ErrorExit:
    if( DRM_SUCCEEDED( dr ) )
    {
        /* On success the size of the slot should be the user given size */
        pSlotCtx->oSlotHeader.dwSlotSize = cbNewSize;
    }
    return dr;
}

static DRM_NO_INLINE DRM_VOID _ZeroHDSContextIgnoringCritSec(
    __in _HdsContext *f_pcontextHDS)
{
    f_pcontextHDS->dwContextSize               = 0;
    f_pcontextHDS->fInited                     = FALSE;
    f_pcontextHDS->eContextSignature           = 0;
    f_pcontextHDS->fp                          = OEM_INVALID_HANDLE_VALUE;
    f_pcontextHDS->nImagesize_FileBlockHDR     = 0;
    f_pcontextHDS->nImagesize_DataBlockHDR     = 0;
    f_pcontextHDS->nImagesize_DataBlockPayload = 0;
    f_pcontextHDS->nDataBlockPayloadPos        = 0;

    ZEROMEM(&f_pcontextHDS->oHeap,    SIZEOF(f_pcontextHDS->oHeap));
    ZEROMEM(&f_pcontextHDS->oSRN,     SIZEOF(f_pcontextHDS->oSRN));
    ZEROMEM(&f_pcontextHDS->contextMD5, SIZEOF(f_pcontextHDS->contextMD5));
}


static DRM_NO_INLINE DRM_RESULT _HdsCreateNameSpaceStore(
    __in _HdsContext *pHDS)
{
    DRM_RESULT     dr   = DRM_SUCCESS;
    _NsContext     oNsStoreCFG;
    _CommBlockHDR *pNsStoreBlock = NULL;

    /* if namespace store does not exist */
    if ( pHDS->oSRN.nNsStoreRootBlockNum == 0 )
    {
        /* NamespaceStore root block does not exist, create one and update SRN */
        ChkDR(_HdsInitNsContext(pHDS, &oNsStoreCFG, NULL, 0, NsStoreNumofChildren, 0, 0));
        ChkDR(_HdsLoadSRN(pHDS));   /* update SRN in memory */

        ChkDR(_HdsAllocBlock(&oNsStoreCFG, ROOTPARENTNUM, &pNsStoreBlock, eCHILDBLOCK));
        pHDS->oSRN.nNsStoreRootBlockNum = pNsStoreBlock->nBlockNum;

        ChkDR(_HdsUpdateSRN(pHDS));

        /* we do not create namespace too often, let's flush it now */
        if ( !Oem_File_FlushBuffers(pHDS->fp) )
        {
            ChkDR(DRM_E_FILE_WRITE_ERROR);
        }
    }

ErrorExit:

    _Hds_free(pHDS, pNsStoreBlock);
    return dr;
}


static DRM_NO_INLINE DRM_RESULT _HdsCreateNamespace(
    __in       _HdsContext       *pHDS,
    __in const DRM_DST_NAMESPACE *pNamespace,
    __in const DRM_WORD           wMaxNumChildren, /* wMaxNumChildren must be <= DRM_HDS_MAXIMUM_CHILDREN */
    __in       DRM_DWORD          dwExpectedSlotSize,
    __in       DRM_BOOL           fWait)
{
    DRM_RESULT        dr               = DRM_SUCCESS;
    DRM_DWORD         cbNsStoreSlot    = 0;
    _NsContext        oNsStoreCFG      = {0};
    _SlotContext     *pNsStoreSlotCtx  = NULL;
    _NSEntry         *pNsStoreEntry    = NULL;
    _CommBlockHDR    *pNsRootBlock     = NULL;
    DRM_BOOL          fCreated         = FALSE;
    _NsContext        oNsCtx           = {0};
    DRM_WORD          wBloomBytesM     = 0;
    DRM_WORD          wBloomK          = 0;

    ChkArg( pHDS        != NULL
        &&  pNamespace  != NULL
        &&  pHDS->fp    != OEM_INVALID_HANDLE_VALUE
        &&  wMaxNumChildren > 0
        &&  wMaxNumChildren <= DRM_HDS_MAXIMUM_CHILDREN );

    /* flush SRN in context */
    ChkDR(_HdsLoadSRN(pHDS));

    /* First, set up a HDS Config for accessing the NamespaceStore
    ** ===========================================================
    */
    ChkDR(_HdsCreateNameSpaceStore(pHDS));


    /* Second, set up tmp Config for the new namespace, allocate and set up
    ** the Root block for the new namespace
    ** Note, parent of NamespaceRoot block is always 'ROOTPARENTNUM'
    ** ============================================================
    */
    {
        DRM_DWORD dwTemp1 = 0;
        DRM_DWORD dwTemp2 = 0;
        DRM_WORD   wTemp  = 0;
        ChkDR( DRM_DWordSub( pHDS->oSRN.dwBlockSize, SIZEOF( _CommBlockFileHDR ), &dwTemp1 ) );
        ChkDR( DRM_DWordAdd( dwExpectedSlotSize, SIZEOF( _SlotHeader ), &dwTemp2 ) );
        dwTemp1 = dwTemp1 / dwTemp2;
        ChkDR( DRM_DWordToWord( dwTemp1, &wTemp ) );
        _HdsCalcOptimalBloom( wTemp, &wBloomBytesM, &wBloomK );
    }
    ChkDR(_HdsInitNsContext(pHDS, &oNsCtx, pNamespace, 0, wMaxNumChildren, wBloomBytesM, wBloomK));
    ChkDR(_HdsAllocBlock(&oNsCtx, ROOTPARENTNUM, &pNsRootBlock, eCHILDBLOCK));

    /* Third, alloc buffer and setup _SlotContext to store the given
    ** namespace in NamespaceStore
    ** ==============================================================
    */
    {
        DRM_DST_KEY oNsHashKey;
        DRM_DST_KEY oNsUniqueKey;
        DRM_DWORD   cbNsStoreSlotCtx = 0;

        ChkOverflowSLOTCONTEXTLEN(&oNsStoreCFG);
        ChkDR(_HdsInitNsContext(pHDS, &oNsStoreCFG, NULL, pHDS->oSRN.nNsStoreRootBlockNum,
                                NsStoreNumofChildren, 0, 0));     /* set up the Config for NamespaceStore */

        cbNsStoreSlotCtx = CALC_SLOTCONTEXTLEN(&oNsStoreCFG);  /* size of slot context */
        ChkDR(_Hds_malloc(pHDS, cbNsStoreSlotCtx, (DRM_VOID**)&pNsStoreSlotCtx));
        ChkDR(_HdsInitSlotContext(&oNsStoreCFG, (DRM_BYTE *)pNsStoreSlotCtx, cbNsStoreSlotCtx));

        /* prepare the namespace content and store it in NamespaceStore
        ** ===================================================================
        */
        _GenNamespaceKeys (&pHDS->contextMD5,
               (DRM_CHAR *) pNamespace->rgbOpaqueData,
                            SIZEOF( DRM_DST_KEY ),
                            oNsHashKey.rgb,
                            oNsUniqueKey.rgb);  /* gen key for given namespace */
        cbNsStoreSlot = SIZEOF(_NSEntry) + DRM_HDS_NSNAME_SIZE;    /* size of the namespace slot in NamespaceStore */
        dr = _HdsOpenSlot(
                &oNsStoreCFG,
                &oNsHashKey,
                &oNsUniqueKey,
                FALSE,
                (DRM_DWORD)( eDRM_DST_CREATE_NEW | eDRM_DST_LOCKEXCLUSIVE | (fWait? eDRM_DST_LOCKWAIT : 0) ),
                FALSE,
               &cbNsStoreSlot,
                pNsStoreSlotCtx );
        if ( DRM_SUCCEEDED(dr) )
        {
            /* store the namespace into NamespaceStore */
            DRM_DWORD cbWritten=0;

            fCreated = TRUE;

            /* set up tmp Config for the new namespace and allocate and set up
            ** the Root block for the new namespace
            ** Note, parent of NamespaceRoot block is always 'ROOTPARENTNUM'*/

            /* allocate buffer and prepare the namespace slot */
            ChkDR(_Hds_malloc(pHDS, cbNsStoreSlot, (DRM_VOID**)&pNsStoreEntry));
            pNsStoreEntry->nNSRBlockNum = pNsRootBlock->nBlockNum;
            pNsStoreEntry->wMaxNumChildren = wMaxNumChildren;
            pNsStoreEntry->wBloomBytesM = wBloomBytesM;
            pNsStoreEntry->wBloomK = wBloomK;
            MEMCPY(pNsStoreEntry->bNSName, pNamespace->rgbOpaqueData, DRM_HDS_NSNAME_SIZE);

            ChkDR( _HdsValidateNSEntryFromDisk( pHDS, pNsStoreEntry ) );

            /* write the namespace slot in NsStore */
            FIX_ENDIAN_WORD( pNsStoreEntry->wMaxNumChildren );
            FIX_ENDIAN_DWORD( pNsStoreEntry->nNSRBlockNum );
            FIX_ENDIAN_WORD( pNsStoreEntry->wBloomBytesM );
            FIX_ENDIAN_WORD( pNsStoreEntry->wBloomK );
            ChkDR( _HdsSlotWrite(pNsStoreSlotCtx, cbNsStoreSlot, (DRM_BYTE *)pNsStoreEntry, &cbWritten) );
            FIX_ENDIAN_WORD( pNsStoreEntry->wMaxNumChildren );
            FIX_ENDIAN_DWORD( pNsStoreEntry->nNSRBlockNum );
            FIX_ENDIAN_WORD( pNsStoreEntry->wBloomBytesM );
            FIX_ENDIAN_WORD( pNsStoreEntry->wBloomK );

            /* we do not create namespace too often, let's flush it now */
            if ( !Oem_File_FlushBuffers(pHDS->fp) )
            {
                ChkDR( DRM_E_FILE_WRITE_ERROR );
            }

            ChkDR( _HdsAddNSCacheEntry( pHDS, pNsStoreEntry ) );

        }
        else if ( dr == DRM_E_DST_SLOT_EXISTS )
        {
            TRACE(("DRM_HDS_CreateNamespace(): namespace '%s' already exist.\n", pNamespace->rgbOpaqueData));
            dr = DRM_E_DST_NAMESPACE_EXISTS;
        }
    }

ErrorExit:

    if ( DRM_FAILED(dr)  &&  pNsRootBlock != NULL )
    {
        /* we have error, remove the allocated slot */
        _HdsFreeBlock((_CommBlockHDR *)pNsRootBlock);
    }

    if ( fCreated )
    {
        _HdsUnlockSlot(pNsStoreSlotCtx);
        _HdsCloseSlot(pNsStoreSlotCtx);
    }

    _Hds_free(pHDS, pNsStoreEntry);
    _Hds_free(pHDS, pNsStoreSlotCtx);
    _Hds_free(pHDS, pNsRootBlock);
    return dr;
}

static DRM_NO_INLINE DRM_RESULT _HdsOpenExistingNamespace(
    __in        _HdsContext               *pHDS,
    __in  const DRM_DST_NAMESPACE         *pNamespace,
    __out       DRM_DST_NAMESPACE_CONTEXT *pcontextNS) /* user given NS context buffer, NULL to get size */
{
    DRM_RESULT    dr=DRM_SUCCESS;
    DRM_DST_KEY   oNsHashKey;
    DRM_DST_KEY   oNsUniqueKey;
    DRM_DWORD     cbNsStoreSlotCtx=0;
    DRM_DWORD     cbNsSlotSize=0;
    _NsContext    oNsStoreCFG;
    _SlotContext *pNsStoreSlotCtx=NULL;
    _NSEntry     *pNsEntry=NULL;
    _NSEntry     *pNsDiskEntry=NULL;
    _NsContext   *pNsCFG=NULL;

    DRMASSERT(pHDS && pNamespace && pcontextNS && pHDS->fp!=OEM_INVALID_HANDLE_VALUE);
    __analysis_assume( pHDS != NULL );
    __analysis_assume( pNamespace != NULL );

    /* First, set up a HDS Config (oNsStoreCFG) for accessing the NamespaceStore
    */
    ChkDR(_HdsInitNsContext(pHDS, &oNsStoreCFG, NULL, pHDS->oSRN.nNsStoreRootBlockNum,
        NsStoreNumofChildren, 0, 0));

    /* Second, alloc buffer and setup _SlotContext (pNsStoreSlotCtx) to store the given
    ** namespace in NamespaceStore
    */
    cbNsStoreSlotCtx = CALC_MAXSLOTCONTEXTLEN;
    ChkDR(_Hds_malloc(pHDS, cbNsStoreSlotCtx, (DRM_VOID**)&pNsStoreSlotCtx));
    ChkDR(_HdsInitSlotContext(&oNsStoreCFG, (DRM_BYTE *)pNsStoreSlotCtx, cbNsStoreSlotCtx));

    /* Third, check the namespace cache to try to avoid hitting the HDS
    */
    ChkDR( _HdsLookupNSCacheEntry( pHDS, pNamespace, &pNsEntry ) );

    if( pNsEntry == NULL )
    {
        /* Fourth, prepare the namespace key and search it in NamespaceStore
        */
        _GenNamespaceKeys (&pHDS->contextMD5,
               (DRM_CHAR *) pNamespace->rgbOpaqueData,
                            SIZEOF( DRM_DST_KEY ),
                            oNsHashKey.rgb,
                            oNsUniqueKey.rgb);
        dr = _HdsOpenSlot(&oNsStoreCFG, &oNsHashKey, &oNsUniqueKey, FALSE, eDRM_DST_OPEN_EXISTING, FALSE,
            &cbNsSlotSize, pNsStoreSlotCtx);
        if ( DRM_SUCCEEDED(dr) )
        {
            ChkDR( _HdsReadNsEntryFromDisk( pHDS, pNsStoreSlotCtx, cbNsSlotSize, &pNsDiskEntry ) );

            ChkDR(_HdsCloseSlot(pNsStoreSlotCtx));

            pNsEntry = pNsDiskEntry;

            /* Now we should cache the entry if there's room.
            */
            ChkDR( _HdsAddNSCacheEntry( pHDS, pNsEntry ) );
        }
        else if ( dr == DRM_E_DST_SLOT_NOT_FOUND )
        {
            ChkDR(DRM_E_DST_NAMESPACE_NOT_FOUND);
        }
        else
        {
            ChkDR(dr);
        }
    }

    pNsCFG = (_NsContext *)pcontextNS;
    ChkDR(_HdsInitNsContext(pHDS, pNsCFG, pNamespace, pNsEntry->nNSRBlockNum, pNsEntry->wMaxNumChildren, pNsEntry->wBloomBytesM, pNsEntry->wBloomK));

#ifdef _CHKHDS_
        printf("\n\t[Namespace Info]\n");
        printf("\t[# of Children blocks]\t\t%d\n", pNsEntry->wMaxNumChildren);
        printf("\t[Namespace Root block#]\t\t%d\n", pNsEntry->nNSRBlockNum);
        printf("\t[Bloom filter size in bytes (m)]\t\t%d\n", pNsEntry->wBloomBytesM);
        printf("\t[Bloom filter coefficient (k)]\t\t%d\n", pNsEntry->wBloomK);
        printf("\t[Generic block header size]\t%d\n", pHDS->nImagesize_FileBlockHDR);
        printf("\t[CHILD block header size]\t%d\n", pNsCFG->nImagesize_ChildBlockHDR);
        printf("\t[CHILD alloctable size]\t\t%d\n", pNsCFG->nImagesize_ChildAllocTable);
        printf("\t[CHILD block payload size]\t%d\n", pNsCFG->nImagesize_ChildBlockPayload);
        printf("\t[CHILD block payload pos]\t%d\n", pNsCFG->nChildBlockPayloadPos);
#endif

ErrorExit:
    if ( pNsDiskEntry )
    {
        _Hds_free(pHDS, pNsDiskEntry);
    }
    if ( pNsStoreSlotCtx )
    {
        _Hds_free(pHDS, pNsStoreSlotCtx);
    }
    return dr;
}


static DRM_NO_INLINE DRM_RESULT _HdsFormatExpandedSpace(
    __in  _HdsContext *f_pHDS,
    __in  DRM_DWORD    f_nBeginBlockNum,
    __in  DRM_DWORD    f_nNumBlocks)
{
    DRM_RESULT     dr     = DRM_SUCCESS;
    _CommBlockHDR *pBlock = NULL;
    DRM_DWORD      i;

    ChkArg( f_pHDS != NULL
        && f_nNumBlocks > 0 );

    ChkDR(_Hds_malloc(f_pHDS, GetMemsize_DataBlock(), (DRM_VOID**)&pBlock));

    /* format the expanded space in blocks, start from last block */
    for (i = (f_nBeginBlockNum + f_nNumBlocks - 1); i >= f_nBeginBlockNum; i--)
    {
        DRM_DWORD      dwFilePos  = 0;
        _DataBlockHDR *pDataBlock = (_DataBlockHDR*)pBlock;

        /* initialize the tmp block */
        pBlock->nBlockNum = i;
        pBlock->File._image.nParentBlockNum = 0;
        pBlock->File._image.ui64TimeStamp   = _GetTimeStamp(f_pHDS->pOEMContext);

        PUT_BYTE( &pBlock->File._image.bBlockType, 0, eFREEBLOCK );

        pDataBlock->nPayloadSize = f_pHDS->nImagesize_DataBlockPayload;
        pDataBlock->File._image.nCascadingBlockNum = f_pHDS->oSRN.nFreeListHead;

        /* chain the block to SRN's freelist */
        f_pHDS->oSRN.nFreeListHead = pBlock->nBlockNum;
        if ( f_pHDS->oSRN.nHighestFormattedBlockNum < pBlock->nBlockNum )
        {
            f_pHDS->oSRN.nHighestFormattedBlockNum = pBlock->nBlockNum;
        }

        ChkDR(_HdsGenBlockHash(f_pHDS, NULL, pBlock, pBlock->File._image.bBlockHash));

        /* update block to file */
        ChkDR(_HdsBlockNum2FilePos(f_pHDS, i, &dwFilePos));
        {
            DRM_LONG lPos = 0;
            ChkDR( DRM_DWordToLong( dwFilePos, &lPos ) );
            if ( Oem_File_SetFilePointer(f_pHDS->fp, lPos, OEM_FILE_BEGIN, NULL)
              && _WriteCommonBlockHeader(f_pHDS, pBlock, f_pHDS->nImagesize_FileBlockHDR, NULL)
              && _WriteDataBlockHeader(f_pHDS, pDataBlock) )
            {
                /* commit changes of root node to file */
                ChkDR(_HdsUpdateSRN(f_pHDS));
            }
            else
            {
                ChkDR(DRM_E_FILE_WRITE_ERROR);
            }
        }
    }


ErrorExit:

    _Hds_free(f_pHDS, pBlock);
    return dr;
}

/* number of blocks to format in the expanded/raw store space.
** this number must be >= 1
*/
#define NUMBLOCKSTOFORMAT  1

static DRM_NO_INLINE DRM_RESULT _HdsPreAlloc(
    __in      _HdsContext *f_pHDS,
    __in      DRM_DWORD    f_PreAllocFileSizeInKB,
    __in      DRM_BOOL     f_fUpToSpecifiedSize,
    __out_opt DRM_DWORD   *f_pnNextBlockNum )
{
    DRM_RESULT     dr   = DRM_SUCCESS;
    DRM_DWORD      dwFileSize = 0;
    DRM_DWORD      nNumBlocks    = 0;
    DRM_DWORD      nNextBlockNum = 0;
    DRM_DWORD      nLastBlockNum = 0;

    /* calc # of blocks to be appended at end of file, then append them */
    if (!Oem_File_GetSize(f_pHDS->fp, &dwFileSize))
    {
        ChkDR(DRM_E_FILE_READ_ERROR);
    }

    /* get the block number of the last block */
    ChkDR(_HdsFilePos2BlockNum(f_pHDS, dwFileSize, &nLastBlockNum));
    --nLastBlockNum;

    /* check if we need to expand the file */
    if ( f_pHDS->oSRN.nHighestFormattedBlockNum > 0
      && f_pHDS->oSRN.nHighestFormattedBlockNum < nLastBlockNum )
    {
        /* unformated store space exist, format the next NUMBLOCKSTOFORMAT blocks  */
        nNextBlockNum = f_pHDS->oSRN.nHighestFormattedBlockNum + 1;
        nNumBlocks = nLastBlockNum - f_pHDS->oSRN.nHighestFormattedBlockNum;
    }
    else
    {
        /* expand the store */

        /* make sure the prealloc size is bigger than the current file size */
        if ( f_fUpToSpecifiedSize )
        {
            ChkArg( dwFileSize < (f_PreAllocFileSizeInKB * 1024) );
        }

        nNumBlocks = (f_PreAllocFileSizeInKB * 1024) / f_pHDS->oSRN.dwBlockSize;

        /* make sure # of blocks to prealloc is not zero */
        if (nNumBlocks == 0)
        {
            nNumBlocks = 1;
        }

        /* expand the file by n blocks */
        ChkDR(_HdsExpandStore(f_pHDS, nNumBlocks, &nNextBlockNum));

    }

    /* format NUMBLOCKSTOFORMAT blocks */
    if ( nNumBlocks > NUMBLOCKSTOFORMAT )
    {
        nNumBlocks = NUMBLOCKSTOFORMAT;
    }

    /* format at most NUMBLOCKSTOFORMAT blocks in the raw space  */
    ChkDR(_HdsFormatExpandedSpace(f_pHDS, nNextBlockNum, nNumBlocks));

    /* flush SRN in context */
    ChkDR( _HdsLoadSRN(f_pHDS) );

    if ( f_pnNextBlockNum != NULL )
    {
        *f_pnNextBlockNum = nNextBlockNum;
    }
ErrorExit:

    return dr;
}


/**********************************************************************
** Function:    _HdsLockRestOfBlock2DeleteSlot
** Synopsis:    lock block payload from the given slot to end of block.
**              It is essentially the same as _HdsLockSlot where we are
**              grabbing a lock on a bigger slot.
** Arguments:   [pSlotCtx] --
**              [fExclusive] --
** Returns:     DRM_SUCCESS on success
** Notes:       To remove a slot from a block, it is necessary to lock
**              the slot to end of block (to do adjustments).
**
**              THE CALLER MUST ALREADY HOLD THE HDS CRITICAL SECTION!
**              This is because we must release the HDS CS before trying
**              to acquire the slot lock since this is the required
**              locking order to avoid a deadlock where one thread may
**              already have the slot lock and need the CS but another
**              thread has the CS and needs the slot lock.
***********************************************************************
*/
static DRM_RESULT _HdsLockRestOfBlock2DeleteSlot(
    __in _SlotContext *pSlotCtx,
    __in DRM_DWORD     eMode)
{
#if DRM_SUPPORT_MULTI_THREADING

    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  dwFilePos    = 0;
    DRM_DWORD  dwLockSize   = 0;
    DRM_DWORD  nBytesToSkip = 0;
    DEFINE_LOCK_VARS;

    /* The caller MUST have already acquired the CS */
    fCSEntered = TRUE;

    /* compute filepos of the block */
    ChkDR(_HdsBlockNum2FilePos(pSlotCtx->pNS->pHDS, pSlotCtx->pCurrChildBlock->nBlockNum, &dwFilePos));

    /* skip current slot */
    if ( ISOVERSIZESLOT(pSlotCtx->oSlotHeader.dwSlotSize, pSlotCtx->pNS) )
    {
        nBytesToSkip = SIZEOF(_SlotHeader) + SIZEOF(DRM_DWORD);
    }
    else
    {
        nBytesToSkip = SIZEOF(_SlotHeader) + pSlotCtx->oSlotHeader.dwSlotSize;
    }

    /* adjust filepos relative to the block */
    ChkDR( DRM_DWordAdd( dwFilePos, pSlotCtx->pNS->pHDS->oSRN.dwBlockSize, &dwLockSize ) );
    dwFilePos += (pSlotCtx->pNS->nChildBlockPayloadPos + pSlotCtx->dwSlotPosInBlock + nBytesToSkip);
    dwLockSize -= dwFilePos;

#ifdef _TRACELOCK
    TRACE(("_HdsLockRestOfBlock2DeleteSlot requested:  %c%c - pos %d,  size %d\n",
          (eMode & eDRM_DST_LOCKEXCLUSIVE)? 'X' : 'S',
          (eMode & eDRM_DST_LOCKWAIT)? 'W' : ' ',
          dwFilePos,
          dwLockSize));
#endif

    /* Release the critical section since to preserve the correct locking order we
    ** aren't allowed to hold the CS while waiting on the file lock
    */
    LEAVE_HDS_CS( pSlotCtx->pNS->pHDS );

    /* lock it */
    if ( !Oem_File_Lock(pSlotCtx->pNS->pHDS->fp,
                       ((eMode & eDRM_DST_LOCKEXCLUSIVE) != 0),
                       dwFilePos,
                       dwLockSize,
                       ((eMode & eDRM_DST_LOCKWAIT) != 0) ) )
    {
        dr = DRM_E_DST_LOCK_FAILED;
    }
    /* Re-enter HDS CS */
    ENTER_HDS_CS( pSlotCtx->pNS->pHDS );
    ChkDR( dr );

#ifdef _TRACELOCK
    TRACE(("_HdsLockRestOfBlock2DeleteSlot obtained\n"));
#endif

ErrorExit:
    return dr;

#else   /* !DRM_SUPPORT_MULTI_THREADING */

    return DRM_SUCCESS;

#endif
}


static DRM_NO_INLINE DRM_RESULT _HdsUnlockRestOfBlock2DeleteSlot(
    __in _SlotContext *pSlotCtx)
{
#if DRM_SUPPORT_MULTI_THREADING

    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  dwFilePos    = 0;
    DRM_DWORD  dwLockSize   = 0;
    DRM_DWORD  nBytesToSkip = 0;
    DRM_BOOL   fResult = FALSE;

    /* compute filepos of the block */
    ChkDR(_HdsBlockNum2FilePos(pSlotCtx->pNS->pHDS, pSlotCtx->pCurrChildBlock->nBlockNum, &dwFilePos));

    /* skip current slot */
    if ( ISOVERSIZESLOT(pSlotCtx->oSlotHeader.dwSlotSize, pSlotCtx->pNS) )
    {
        nBytesToSkip = SIZEOF(_SlotHeader) + SIZEOF(DRM_DWORD);
    }
    else
    {
        nBytesToSkip = SIZEOF(_SlotHeader) + pSlotCtx->oSlotHeader.dwSlotSize;
    }

    /* adjust filepos relative to the block */
    dwLockSize = dwFilePos + pSlotCtx->pNS->pHDS->oSRN.dwBlockSize;
    dwFilePos += (pSlotCtx->pNS->nChildBlockPayloadPos+pSlotCtx->dwSlotPosInBlock + nBytesToSkip);
    dwLockSize -= dwFilePos;

#ifdef _TRACELOCK
    TRACE(("_HdsUnlockRestOfBlock2DeleteSlot requested:  - pos %d,  size %d\n", dwFilePos,
        dwLockSize));
#endif

    /* unlock it, do not return even if this fail */
    fResult = Oem_File_Unlock(pSlotCtx->pNS->pHDS->fp, dwFilePos, dwLockSize);

    if ( !fResult )
    {
        dr = DRM_E_DST_LOCK_FAILED;
    }

ErrorExit:
    return dr;

#else   /* !DRM_SUPPORT_MULTI_THREADING */

    return DRM_SUCCESS;

#endif
}




/**********************************************************************
** Function:    _HdsSlotEnumDeleteCurrent
** Synopsis:
** Arguments:   [pEnumContext] -- Enum Context init'd by DRM_HDS_InitSlotEnum()
**              [pSlotContext] -- Slot Context of the current slot
** Returns:     DRM_SUCCESS on success
** Notes:       A shared SRN lock and HDS CS must be held prior to calling this function
***********************************************************************
*/
static DRM_NO_INLINE DRM_RESULT _HdsSlotEnumDeleteCurrent(
    __in  _EnumContext *pEnum,
    __out _SlotContext *pSlotCtx)
{
    DRM_RESULT       dr = DRM_SUCCESS;
    DRM_BOOL         fRestOfBlockLocked = FALSE;
    _CommBlockHDR   *pTmpBlock = NULL;

    DRMASSERT(pEnum && pEnum->pNS);
    ChkArg(ISVALIDCONTEXT(pEnum, eEnumContextSignature));
    ChkArg(ISVALIDCONTEXT(pEnum->pNS, eCfgContextSignature));
    DRMASSERT(pEnum->pNS->fInited && pEnum->pNS->pHDS && pSlotCtx
        && pEnum->pNS->pHDS->fp!=OEM_INVALID_HANDLE_VALUE);

    /*
    ** Verify validity of the current block of the slot context
    */
    {
        _CommBlockHDR *pBlock = &pEnum->oFileBlock;
        _CommBlockHDR *pCurrBlock = &pEnum->oCurrBlock;

        /* check if query started yet */
        if ( ((_ChildBlockHDR*)pCurrBlock)->nNextSlotPos == MAXSLOTPOS )
        {
            /* query has not been started, cannot proceed */
            ChkDR(DRM_E_DST_SLOT_NOT_FOUND);
        }

        /* load generic block header */
        ChkDR(_HdsGetPutBlockHDR(pEnum->pNS, pEnum->oFileBlock.nBlockNum, &pBlock, GPBH_GENERIC_ONLY, GPBH_OPERATION_READ));

        /* verify the generic header */
        if ( !DRM_UI64Eql(pEnum->oFileBlock.File._image.ui64TimeStamp, pCurrBlock->File._image.ui64TimeStamp)
          || MEMCMP(pEnum->oFileBlock.File._image.bBlockHash, pCurrBlock->File._image.bBlockHash, SIZEOF(pCurrBlock->File._image.bBlockHash))!=0 )
        {
            /* the current block has been updated/written since last read */
            _SlotHeader slotHeader;

            /* check if it is still a CHILD block or belong to the same parent */
            if ( GET_BYTE( &pEnum->oFileBlock.File._image.bBlockType, 0 ) != GET_BYTE( &pCurrBlock->File._image.bBlockType, 0 )
              || pEnum->oFileBlock.File._image.nParentBlockNum            != pCurrBlock->File._image.nParentBlockNum )
            {
                /* there is nothing we can do when the current block is totally invalid
                */
                ChkDR(DRM_E_DST_SLOT_NOT_FOUND);
            }

            /* load the child block in a tmp buffer */
            ChkDR(DRM_HDSIMPL_AllocBlockBuffer(pEnum->pNS, eCHILDBLOCK, &pTmpBlock));
            ChkDR(_HdsGetPutBlockHDR(pEnum->pNS, pEnum->oFileBlock.nBlockNum,
                &pTmpBlock, GPBH_ANY_HEADER, GPBH_OPERATION_READ));

            /* load the current slot header from file */
            ChkDR(_HdsGetPutBlockPayload(pTmpBlock, ((_ChildBlockHDR*)pCurrBlock)->nCurrSlotPos,
                SIZEOF(_SlotHeader), &slotHeader, GPBP_OPERATION_READ));
            FIX_ENDIAN_DWORD( slotHeader.dwAttributes );
            FIX_ENDIAN_DWORD( slotHeader.dwSlotSize );
            if (MEMCMP(&slotHeader, &pEnum->oSlotHeader, SIZEOF(_SlotHeader))==0 )
            {
                /* the current slot is intact, update the block buffer and continue */
                DRM_DWORD _dwCurrPos = ((_ChildBlockHDR*)pCurrBlock)->nCurrSlotPos;
                DRM_DWORD _dwNextPos = ((_ChildBlockHDR*)pCurrBlock)->nNextSlotPos;

                ChkDR(_HdsGetPutBlockHDR(pEnum->pNS, pEnum->oFileBlock.nBlockNum, &pCurrBlock, GPBH_ANY_HEADER, GPBH_OPERATION_READ));
                ((_ChildBlockHDR*)pCurrBlock)->nCurrSlotPos = _dwCurrPos;
                ((_ChildBlockHDR*)pCurrBlock)->nNextSlotPos = _dwNextPos;
                goto Verified_Okay;
            }

            ChkDR(DRM_E_DST_SLOT_NOT_FOUND);
        }
    }

Verified_Okay:

    /* we have exclusive lock of the slot already. Let's lock the rest of the block */
    ChkDR( _HdsLockRestOfBlock2DeleteSlot(pSlotCtx, eDRM_DST_LOCKEXCLUSIVE | eDRM_DST_LOCKWAIT) );
    fRestOfBlockLocked = TRUE;

    /* Remove the slot */
    ChkDR( _HdsRemoveSlot(pSlotCtx, eRemoveSlotPermanent) );

ErrorExit:

    if ( fRestOfBlockLocked )
    {
        _HdsUnlockRestOfBlock2DeleteSlot(pSlotCtx);
    }

    if ( pTmpBlock )
    {
        _Hds_free(pEnum->pNS->pHDS, pTmpBlock);
    }

    if ( DRM_SUCCEEDED(dr) )
    {
        _HdsUnlockSlot(pSlotCtx);
        _HdsCloseSlot(pSlotCtx);
    }
    return dr;
}


static DRM_NO_INLINE DRM_RESULT _HdsBlockScanInit(
    __in       _NsContext        *f_pContextNS,
    __in const DRM_DST_KEY        f_rgKeyHash [],
    __in const DRM_DWORD          f_cKeysHash,
    __in const DRM_DST_KEY       *f_pkeyUnique,
    __in const DRM_DST_LOCKMODE   f_lockmode,
    __out      _BlockScanContext *f_pcontextBlockScan)
{
    DRM_RESULT dr = DRM_SUCCESS;

    /* init Enum context */

    ZEROMEM(f_pcontextBlockScan, SIZEOF(_BlockScanContext));
    f_pcontextBlockScan->cbContext         = SIZEOF(_BlockScanContext);
    f_pcontextBlockScan->pcontextNS        = f_pContextNS;
    f_pcontextBlockScan->fInited           = TRUE;
    f_pcontextBlockScan->eContextSignature = eEnumContextSignature;
    f_pcontextBlockScan->fCurrBlockIsValid = FALSE;
    f_pcontextBlockScan->lockmode          = f_lockmode;
    f_pcontextBlockScan->pkeyHash          = f_rgKeyHash;
    f_pcontextBlockScan->cKeysHash         = f_cKeysHash;
    f_pcontextBlockScan->iKeyCurrent       = 0;

    /* locate and prepare pEnum->oCurrBlock, if possible, for EnumNext() */

    ChkDR(_HdsInitBlockBuffer(f_pcontextBlockScan->pcontextNS,
                              (_CommBlockHDR*)&f_pcontextBlockScan->blockheaderCurrent,
                              0,
                              eCHILDBLOCK));

    /* save the hash key in the blockscan context */

    MEMCPY(f_pcontextBlockScan->keyUnique.rgb, f_pkeyUnique->rgb, SIZEOF( DRM_DST_KEY ));

ErrorExit:
    return dr;
}

static DRM_NO_INLINE DRM_BOOL _IsMatchingKey(
    __in const DRM_DST_KEY *f_pkey,
    __in const DRM_DST_KEY  f_rgkeys [],
    __in       DRM_DWORD    f_cKeys,
    __in       DRM_DWORD    f_iHint)
{
    DRM_BOOL  fMatch = FALSE;
    DRM_DWORD iKey   = 0;

    if (f_iHint >= f_cKeys)
    {
        f_iHint = 0;
    }

    for (iKey = f_iHint; iKey < f_cKeys; iKey++)
    {
        if (MEMCMP(f_pkey, f_rgkeys + iKey, SIZEOF(DRM_DST_KEY)) == 0)
        {
            fMatch = TRUE;
            goto MatchFound;
        }
    }

    if (f_iHint > 0)
    {
        for (iKey = 0; iKey < f_iHint; iKey++)
        {
            if (MEMCMP(f_pkey, f_rgkeys + iKey, SIZEOF(DRM_DST_KEY)) == 0)
            {
                fMatch = TRUE;
                break;
            }
        }
    }

MatchFound:
    return fMatch;
}

#define COA_BLOCK_NOT_SET 0xFFFFFFFF

/* Note: The HDS CS and SRN must be held prior to calling this function  */
static DRM_NO_INLINE DRM_RESULT _CoalesceBlock(
    __in        _CommBlockHDR   *f_pblockheader,
    __in        _SlotContext    *f_pcontextSlotLock,
    __in  const DRM_DST_KEY     *f_pkeyUnique,
    __in        DRM_DST_LOCKMODE f_lockmode,
    __out       DRM_BOOL        *f_fLocked )
{
    DRM_RESULT      dr                   = DRM_SUCCESS;
    DRM_BOOL        fFound               = TRUE;
    DRM_BOOL        fMove                = FALSE;
    DRM_DWORD       ibSource             = COA_BLOCK_NOT_SET;
    DRM_DWORD       ibNext               = COA_BLOCK_NOT_SET;
    DRM_DWORD       ibDest               = COA_BLOCK_NOT_SET;
    _SlotContext   *pcontextSlot         = NULL;
    const DRM_DWORD cbSlotContext        = CALC_SLOTCONTEXTLEN(f_pblockheader->pNS);

    ChkOverflowSLOTCONTEXTLEN( f_pblockheader->pNS );

    /* allocate temporary slotcontext */

    ChkDR(_Hds_malloc(f_pblockheader->pNS->pHDS,
                      cbSlotContext,
       (DRM_VOID **) &pcontextSlot));

    ((_ChildBlockHDR *) f_pblockheader)->nNextSlotPos = 0;

   *f_fLocked = FALSE;

    while (fFound)
    {
        ChkDR(_HdsInitSlotContext(f_pblockheader->pNS,
                     (DRM_BYTE *) pcontextSlot,
                                  cbSlotContext));

        /* get the next hidden block for this HASHKEY */

        ChkDR(_HdsSearchSlotInBlock(f_pblockheader,
                                    NULL,
                                    f_pkeyUnique,
                                    eSearchHiddenOnly,
                                    pcontextSlot,
                                   &fFound));

        if (fFound)
        {
            fMove = TRUE;

            /* if this is the first of >= 1 adjacent blocks, cache the offset of the beginning  */

            if (ibDest == COA_BLOCK_NOT_SET)
            {
                ibDest = pcontextSlot->dwSlotPosInBlock;

                /* save the first slot to be affected by the move */

                MEMCPY(f_pcontextSlotLock, pcontextSlot, cbSlotContext);
                /* Set the pointers within the copied context that refer back to the source context,
                   since we will be freeing the source context at the end of this function */
                f_pcontextSlotLock->pCurrChildBlock = (_CommBlockHDR *)f_pcontextSlotLock->bBuff;
                f_pcontextSlotLock->pCurrDataBlock  = (_CommBlockHDR *) (f_pcontextSlotLock->bBuff + __CB_DECL(GetMemsize_ChildBlock(f_pblockheader->pNS)));

                ChkDR(_HdsLockBlock2DeleteSlot(f_pcontextSlotLock, f_lockmode));

               *f_fLocked = TRUE;
            }

            /* if this isn't contiguous with the last hidden block then close that previous hole
            ** and continue from where we are
            */

            else if (pcontextSlot->dwSlotPosInBlock != ibNext)
            {
                DRM_DWORD cbMove = pcontextSlot->dwSlotPosInBlock - ibNext;

                ChkDR(_HdsAdjustChildPayload(pcontextSlot->pCurrChildBlock,
                                             ibDest,
                                             ibSource,
                                             cbMove));

                /* the new destination is the end of the blocks just moved */

                ibDest += cbMove;
            }

            /* remove _DataBlockHDR's, if necessary */

            if (ISOVERSIZESLOT(pcontextSlot->oSlotHeader.dwSlotSize,
                               pcontextSlot->pNS))
            {
                DRM_DWORD nBlockData = pcontextSlot->dwFirstDataBlockNum;

                ibSource = pcontextSlot->dwSlotPosInBlock
                         + SIZEOF(_SlotHeader)
                         + SIZEOF(DRM_DWORD);

                while (nBlockData > 0)
                {
                    ChkDR(_HdsLoadBlockHDR(pcontextSlot->pNS,
                                           nBlockData,
                                          &pcontextSlot->pCurrDataBlock));

                    nBlockData = ((_DataBlockHDR *)(pcontextSlot->pCurrDataBlock))->File._image.nCascadingBlockNum;

                    ChkDR(_HdsFreeBlock(pcontextSlot->pCurrDataBlock));
                }
            }
            else
            {
                ibSource = pcontextSlot->dwSlotPosInBlock
                         + pcontextSlot->oSlotHeader.dwSlotSize
                         + SIZEOF(_SlotHeader);
            }

            ibNext = ibSource;
        } /* endif found a hidden slot */
    }

    /* if we have an unprocessed move do it now */

    if (fMove)
    {
        /* move up the remaining space and update the free space */

        ChkDR(_HdsAdjustChildPayload(f_pblockheader,
                                     ibDest,
                                     ibSource,
                                     ADJUST_PAYLOAD_TO_END));

        /* remove slot in _ChildBlockHDR */

        ChkDR(_HdsWriteBlockHDR     (f_pblockheader));
    }

ErrorExit:

    if (pcontextSlot != NULL)
    {
        _Hds_free(f_pblockheader->pNS->pHDS, pcontextSlot);
    }

    return dr;
}


/**********************************************************************
** API functions
***********************************************************************
*/

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HDS_Init(
    __inout DRM_DST_CONTEXT *f_pContextDST )
{
    DRM_RESULT   dr=DRM_SUCCESS;
    _HdsContext *pHDS=(_HdsContext*)f_pContextDST;

    ChkArg(f_pContextDST != NULL);

    ZEROMEM(pHDS, SIZEOF(_HdsContext));
    pHDS->fp = OEM_INVALID_HANDLE_VALUE;

#if DRM_SUPPORT_MULTI_THREADING
    Oem_CritSec_Initialize(&pHDS->oCS);
    pHDS->fCSInited = TRUE;
#endif

ErrorExit:
    return dr;
}



DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HDS_Uninit(
    __inout DRM_DST_CONTEXT *f_pContextDST )
{
    DRM_RESULT   dr=DRM_SUCCESS;

#if DRM_SUPPORT_MULTI_THREADING
    _HdsContext *pHDS=(_HdsContext*)f_pContextDST;

    ChkArg(pHDS != NULL);

    if( pHDS->fCSInited )
    {
        /* delete ciritical section */
        Oem_CritSec_Delete(&pHDS->oCS);
        pHDS->fCSInited = FALSE;
    }
#else
    ChkArg(f_pContextDST != NULL);
#endif

ErrorExit:
    return dr;
}


DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HDS_CommitStore(
    __inout DRM_DST_CONTEXT *f_pContextDST )
{
    DRM_RESULT dr = DRM_SUCCESS;

    _HdsContext *pHDS=(_HdsContext*)f_pContextDST;
    DEFINE_LOCK_VARS

    ChkArg(ISVALIDCONTEXT(pHDS, eHdsContextSignature));
    DRMASSERT(f_pContextDST != NULL
          &&  pHDS->fInited
          &&  pHDS->fp != OEM_INVALID_HANDLE_VALUE);

    /* enter CS to get exclusive access to file handle */
    ENTER_HDS_CS(pHDS);

    if (!Oem_File_FlushBuffers(pHDS->fp))
    {
        dr = DRM_E_FILE_WRITE_ERROR;
    }

ErrorExit:
    if( pHDS != NULL )
    {
        LEAVE_HDS_CS(pHDS);
    }

    return dr;
}


DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HDS_CleanupStore(
    __inout DRM_DST_CONTEXT *f_pContextDST,
    __in    DRM_BOOL         f_fWait )
{
    DRM_RESULT   dr=DRM_SUCCESS;
    _HdsContext *pHDS = (_HdsContext*)f_pContextDST;
    DEFINE_LOCK_VARS

    ChkArg(ISVALIDCONTEXT(pHDS, eHdsContextSignature));
    DRMASSERT(f_pContextDST
           && pHDS->fInited
           && pHDS->fp!=OEM_INVALID_HANDLE_VALUE);

    /*
    ** First, lock the store for EXCLUSIVE. So if there are any slots locked,
    ** this call will fail, or wait until all locks are released
    */
    LOCK_STORE( pHDS, (DRM_DWORD)( eDRM_DST_LOCKEXCLUSIVE | (f_fWait ? eDRM_DST_LOCKWAIT : 0) ) );
    ENTER_HDS_CS(pHDS);

    dr = _HdsCleanupStore(pHDS);
    if ( DRM_SUCCEEDED(dr) )
    {
        if (!Oem_File_FlushBuffers(pHDS->fp))
        {
            dr = DRM_E_FILE_WRITE_ERROR;
        }
    }

ErrorExit:
    LEAVE_HDS_CS(pHDS);
    UNLOCK_STORE(pHDS);
    return dr;
}

PRAGMA_WARNING_PUSH             /* BEGIN: Disable warning 4189 (unreferenced local) for this function due to variable(s) being declared soley for calls to SIZEOF. */
PRAGMA_WARNING_DISABLE(4189)

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HDS_CreateStore(
    __in_opt                           DRM_VOID          *f_pvOEMContext,
    __in_bcount( f_cbStoreData ) const DRM_BYTE          *f_pbStoreData,
    __in                               DRM_DWORD          f_cbStoreData,
    __in                         const DRM_DWORD          f_dwBlockSize,/* block size is limited up to 64K */
    __in                               DRM_DST_BLKNUMTYPE f_eBlNumType,
    __in                               DRM_BOOL           f_fTruncateExist,
    __in                         const DRM_DWORD          f_dwInitSizeInKB,
    __out                              DRM_DST_CONTEXT   *f_pContextDST )
{
    DRM_RESULT             dr            = DRM_SUCCESS;
    DRM_DWORD              nMinBlockSize = 0;
    const _CommBlockHDR   *tmpBlock      = NULL;    /* declared solely for SIZEOF below */
    const _ChildBlockHDR  *tmpChild      = NULL;    /* declared solely for SIZEOF below */
    _HdsContext           *pHDS          = NULL;
    DRM_DWORD              i             = 0;
    DRM_DWORD              dwAttributes  = OEM_ATTRIBUTE_NORMAL;
    const DRM_WCHAR       *wszHdsName    = ( const DRM_WCHAR * )f_pbStoreData;
    DRM_DWORD              cbLen         = 0;
    DRM_DWORD              dwFileSize    = 0;
    DRM_DWORD              dwCreationDisposition = 0;
    DEFINE_LOCK_VARS

    /* Lock order:
    **   - CS
    **   - Whole Store
    */

    ChkArg(f_pbStoreData != NULL
           && f_cbStoreData > 0
           && f_dwBlockSize != 0
           && f_pContextDST != NULL);

    DRMASSERT(f_eBlNumType == eDRM_DSTBLKNUM_WORD
           || f_eBlNumType == eDRM_DSTBLKNUM_DWORD);

    ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( wszHdsName ), &cbLen ) );

    /* check store name
    ** Note: we expect the filename is NULL terminated.
    */
    for (i=0; i < cbLen ; i++)
    {
        if (f_pbStoreData[i] == g_wchNull)   /* empty filename is not allowed */
        {
            DRMASSERT(FALSE);
        }
        if (wszHdsName[i] != g_wchSpace)
        {
            break;
        }
    }

    /* verify required min block size:
    ** SIZEOF generic header + SIZEOF child header + 1 Alloc table elem + 1 slotheader + SIZEOF datablock#
    */
    nMinBlockSize = SIZEOF(tmpBlock->File.bFileImage)        /* minimum _CommBlockHDR size */
                  + SIZEOF(tmpChild->File.bFileImage)        /* minimum _ChildBlockHDR size */
                  + (DRM_WORD)f_eBlNumType                    /* 1 element in alloctable */
                  + SIZEOF(_SlotHeader)                      /* 1 _SlotHeader */
                  + SIZEOF(DRM_DWORD);                       /* Block num of _DataBlockHDR */
    if (f_dwBlockSize < nMinBlockSize)
    {
        TRACE(("Block size too small. Minimum size is %d.\n", nMinBlockSize));
        ChkDR(DRM_E_BUFFERTOOSMALL);    /* DRM_E_HDSBLOCKTOOSMALL */
    }

    {
        /* The few fields of RootNode is const, we cannot assign values
        ** to them. Instead, we init values to a tmp and use memcpy to
        ** the structure.
        ** Note: the reason for const of the first 4 fields is to prevent
        ** accidental alteration of the values in the program. If so, the
        ** compiler will catch it.
        */
        _SRN tmpSRN;

        ZEROMEM(&tmpSRN, SIZEOF (tmpSRN));

        tmpSRN.dwSRNSize     = SIZEOF(_SRN);
        tmpSRN.dwBlockSize   = f_dwBlockSize;
        tmpSRN.eBlockNumType = f_eBlNumType;

        pHDS=(_HdsContext*)f_pContextDST;
        ENTER_HDS_CS( pHDS );

        /* Check that the HDS isn't already open */
        ChkArg( pHDS->fp == OEM_INVALID_HANDLE_VALUE );

        MEMCPY(&pHDS->oSRN, &tmpSRN, SIZEOF(_SRN));
        ZEROMEM(pHDS->oSRN.bSRNHash, SIZEOF( pHDS->oSRN.bSRNHash ) );

        /* The following logic ensures:
        ** - if the store file exists, it is opened, locked exclusive and
        **   truncated (if requested)
        ** - if the store file does not exist, it is created and locked exclusive.
        ** - All of the above are thread safe.
        */

#if _DATASTORE_WRITE_THRU
        dwAttributes |= OEM_FILE_FLAG_WRITE_THROUGH;
#endif

        pHDS->pOEMContext = f_pvOEMContext;

        if ( f_fTruncateExist )
        {
            dwCreationDisposition = OEM_TRUNCATE_EXISTING;
        }
        else
        {
            dwCreationDisposition = OEM_OPEN_EXISTING;
        }

        /* try Open the exist store */
        pHDS->fp = Oem_File_Open(f_pvOEMContext, wszHdsName, OEM_GENERIC_READ|OEM_GENERIC_WRITE,
            OEM_FILE_SHARE_READ|OEM_FILE_SHARE_WRITE, dwCreationDisposition, dwAttributes);

        if (pHDS->fp == OEM_INVALID_HANDLE_VALUE)
        {
            /* store does not exist */
            pHDS->fp = Oem_File_Open(f_pvOEMContext, wszHdsName, OEM_GENERIC_READ|OEM_GENERIC_WRITE,
                OEM_FILE_SHARE_READ|OEM_FILE_SHARE_WRITE, OEM_CREATE_NEW, dwAttributes);
            if (pHDS->fp == OEM_INVALID_HANDLE_VALUE)
            {
                ChkDR(DRM_E_FILE_WRITE_ERROR);
            }
        }

        /* Lock store EXCLUSIVE among other processes. We don't need to drop the CS because
        ** nobody else should have been able to call any other HDS functions before this
        ** one since we weren't opened yet
        */
        LOCK_STORE( pHDS, eDRM_DST_LOCKEXCLUSIVE | eDRM_DST_LOCKWAIT );

        ChkBOOL( Oem_File_GetSize(pHDS->fp, &dwFileSize), DRM_E_FILE_READ_ERROR );
        if( dwFileSize > 0 )
        {
            if ( f_fTruncateExist )
            {
                /* Oem_File_Open() should have truncated the file or created it with size zero */
                ChkDR( DRM_E_FILE_SEEK_ERROR );
            }
            else
            {
                ChkDR( DRM_E_DST_EXISTS );
            }
        }

        /* make sure we point to the start of the file */
        if (!Oem_File_SetFilePointer(pHDS->fp, 0, OEM_FILE_BEGIN, NULL))
        {
            ChkDR(DRM_E_FILE_SEEK_ERROR);
        }

        pHDS->dwContextSize = HDS_CONTEXT_LENGTH(SIZEOF(_SRN));
        pHDS->eContextSignature = eHdsContextSignature;
        pHDS->dwStoreVersion = DRM_HDS_FILE_VERSION_2;
        pHDS->fInited = TRUE;

        /* init internal stack info */
        ChkOverflow( pHDS->dwContextSize, pHDS->dwContextSize - SIZEOF(_HdsContext) );
        ChkDR( DRM_STK_Init( &pHDS->oHeap,
                   (DRM_BYTE*)pHDS + SIZEOF(_HdsContext),
                              pHDS->dwContextSize - SIZEOF(_HdsContext) ) );

        ChkDR(_HdsUpdateSRN(pHDS));
        if ( !Oem_File_FlushBuffers(pHDS->fp) )
        {
            dr = DRM_E_FILE_WRITE_ERROR;
        }

        // prealloc file size
        if ( f_dwInitSizeInKB > 0 )
        {
            /* ignore the error. */
            (void)_HdsPreAlloc( pHDS, f_dwInitSizeInKB, TRUE, NULL);
        }
    }

ErrorExit:

    if ( pHDS != NULL )
    {
        UNLOCK_STORE( pHDS );

        if (pHDS->fp != OEM_INVALID_HANDLE_VALUE)
        {
            Oem_File_Close(pHDS->fp);
        }

        /* we don't need it now, clear it */
        _ZeroHDSContextIgnoringCritSec(pHDS);

        LEAVE_HDS_CS(pHDS);
    }

    return dr;
}
PRAGMA_WARNING_POP      /* END: Disable warning 4189 (unreferenced local) for this function due to variable(s) being declared soley for calls to SIZEOF. */

PRAGMA_WARNING_PUSH             /* BEGIN: Disable warning 4189 (unreferenced local) for this function due to variable(s) being declared soley for calls to SIZEOF. */
PRAGMA_WARNING_DISABLE(4189)

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HDS_OpenStore(
    __in_opt                     DRM_VOID        *f_pvOEMContext,
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM, "Parameters must match generic drmdatastore.h interface")
    __in_bcount( f_cbStoreData ) DRM_BYTE        *f_pbStoreData,
PREFAST_POP
    __in                         DRM_DWORD        f_cbStoreData,
    __in                         DRM_DWORD        f_dwGrowBySizeinKB,
    __out                        DRM_DST_CONTEXT *f_pContextDST )
{
    DRM_RESULT             dr           = DRM_SUCCESS;
    _HdsContext           *pHDS         = (_HdsContext *)f_pContextDST;
    const _CommBlockHDR   *tmpBlock     = NULL;     /* declared solely for SIZEOF below */
    const _DataBlockHDR   *dataBlock    = NULL;     /* declared solely for SIZEOF below */
    OEM_FILEHDL            fp           = OEM_INVALID_HANDLE_VALUE;
    DRM_DWORD              dwAttributes = OEM_ATTRIBUTE_NORMAL;
    const DRM_WCHAR       *wszHdsName   = ( const DRM_WCHAR * )f_pbStoreData;
    DRM_DWORD              dwFileSize   = 0;
    DEFINE_LOCK_VARS

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDS, PERF_FUNC_DRM_HDS_OpenStore);

    ChkArg( f_pbStoreData != NULL );
    ChkArg( f_pContextDST != NULL );

    ENTER_HDS_CS(pHDS);

    /* Ensure the HDS hasn't already been opened */
    ChkArg( pHDS->fp == OEM_INVALID_HANDLE_VALUE );

    pHDS->pOEMContext = f_pvOEMContext;

#if _DATASTORE_WRITE_THRU
    dwAttributes |= OEM_FILE_FLAG_WRITE_THROUGH;
#endif

    /* initialize context */
    _ZeroHDSContextIgnoringCritSec(pHDS);

    pHDS->dwContextSize     = HDS_CONTEXT_LENGTH(SIZEOF(_SRN));
    pHDS->fInited           = TRUE;
    pHDS->nGrowbySizeinKB   = f_dwGrowBySizeinKB;
    pHDS->eContextSignature = eHdsContextSignature;

    /*
    ** Open the file only *after* pHDS is initialized.
    ** If you open it first:
    **    Then if you always call DRM_HDS_CloseStore,
    **        you will assert if file open failed inside DRM_HDS_CloseStore.
    **    Then if you don't always call DRM_HDS_CloseStore,
    **        you will leak the file handle if something in
    **        this function fails after the handle is open.
    **    You can't tell from the DRM_DST layer whether the handle is open
    **        to determine whether you should call DRM_HDS_CloseStore or not.
    ** By opening the file at this point, you can call DRM_HDS_CloseStore
    ** regardless of whether DRM_HDS_OpenStore succeeds or fails and
    ** be certain that no resource leak will occur.
    */
#ifdef _CHKHDS_
    /* chkhds utility opens the store in read only mode */
    fp = Oem_File_Open(f_pvOEMContext, wszHdsName, OEM_GENERIC_READ,OEM_FILE_SHARE_READ,
        OEM_OPEN_EXISTING, dwAttributes);
#else
    fp = Oem_File_Open(f_pvOEMContext, wszHdsName, OEM_GENERIC_READ|OEM_GENERIC_WRITE,
        OEM_FILE_SHARE_READ|OEM_FILE_SHARE_WRITE, OEM_OPEN_EXISTING, dwAttributes);
#endif
    ChkBOOL( fp != OEM_INVALID_HANDLE_VALUE, DRM_E_FILENOTFOUND );

    pHDS->fp                = fp;

    /* Lock SRN SHARED among other processes. We don't need to drop the CS because
    ** nobody else should have been able to call any other HDS functions before this
    ** one since we weren't opened yet
    */
    LOCK_SRN(pHDS, (eDRM_DST_LOCKSHARED | eDRM_DST_LOCKWAIT));

    ChkBOOL( Oem_File_GetSize(fp, &dwFileSize), DRM_E_FILE_READ_ERROR );
    if( dwFileSize == 0 )
    {
        /* Treat a zero-size file as if it wasn't there, it must be re-created */
        ChkDR( DRM_E_FILENOTFOUND );
    }


    /* init internal stack info */
    ChkOverflow( pHDS->dwContextSize, pHDS->dwContextSize - SIZEOF(_HdsContext) );
    ChkDR( DRM_STK_Init( &pHDS->oHeap,
               (DRM_BYTE*)pHDS + SIZEOF(_HdsContext),
                          pHDS->dwContextSize - SIZEOF(_HdsContext) ) );

    ChkDR(_HdsLoadSRN(pHDS));

    /* compute handy info */
    pHDS->nImagesize_FileBlockHDR = SIZEOF(tmpBlock->File._image.ui64TimeStamp)
                                  + SIZEOF(tmpBlock->File._image.bBlockHash)
                                  + SIZEOF(tmpBlock->File._image.nParentBlockNum)
                                  + 1; /* SIZEOF(tmpBlock->File._image.bBlockType) */;
    pHDS->nImagesize_DataBlockHDR = SIZEOF(dataBlock->File._image.nCascadingBlockNum);
    pHDS->nDataBlockPayloadPos    = pHDS->nImagesize_FileBlockHDR
                                  + pHDS->nImagesize_DataBlockHDR;
    if( pHDS->oSRN.dwBlockSize <= pHDS->nDataBlockPayloadPos )
    {
        /* Not enough room in each block for any data */
        ChkDR(DRM_E_HDSINVALIDSTORE);
    }

    pHDS->nImagesize_DataBlockPayload = pHDS->oSRN.dwBlockSize - pHDS->nDataBlockPayloadPos;

ErrorExit:

    UNLOCK_SRN(pHDS);

    if (DRM_FAILED(dr))
    {
        if ( pHDS != NULL )
        {
            _ZeroHDSContextIgnoringCritSec(pHDS);
        }
        if ( fp != OEM_INVALID_HANDLE_VALUE )
        {
            Oem_File_Close( fp );
        }
    }

    LEAVE_HDS_CS(pHDS);

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}
PRAGMA_WARNING_POP  /* END: Disable warning 4189 (unreferenced local) for this function due to variable(s) being declared soley for calls to SIZEOF. */

/*
**
*/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HDS_CloseStore(
    __inout DRM_DST_CONTEXT *f_pContextDST )
{
    DRM_RESULT   dr = DRM_SUCCESS;
    _HdsContext *pHDS=(_HdsContext*)f_pContextDST;
    DEFINE_LOCK_VARS

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDS, PERF_FUNC_DRM_HDS_CloseStore);

    ChkArg(pHDS && ISVALIDCONTEXT(pHDS, eHdsContextSignature));
    DRMASSERT(f_pContextDST && pHDS->fInited);


    /* Lock Order:
    ** - CS:  to guard exclusive access to file handle
    */
    ENTER_HDS_CS(pHDS);

    if ( pHDS->fp != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( pHDS->fp );
        pHDS->fp = OEM_INVALID_HANDLE_VALUE;
    }

    _ZeroHDSContextIgnoringCritSec(pHDS);

ErrorExit:

    if( pHDS != NULL )
    {
        LEAVE_HDS_CS(pHDS);
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}



/*
**
*/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HDS_CommitNamespace(
    __inout    DRM_DST_NAMESPACE_CONTEXT  *f_pContextNS )
{
    DRM_RESULT  dr = DRM_SUCCESS;
    _NsContext *pUserNsCFG = (_NsContext *)f_pContextNS;

    ChkArg( f_pContextNS != NULL
        &&  ISVALIDCONTEXT(pUserNsCFG, eCfgContextSignature));

    /* we do not need to do any locking, DRM_HDS_CommitStore will do it
    */
    ChkDR(DRM_HDS_CommitStore( (DRM_DST_CONTEXT *)(pUserNsCFG->pHDS) ));

ErrorExit:

    return dr;
}




/******************************************************************************
**
** Function :   DRM_HDS_OpenNamespace
**
** Synopsis :   Opens/creates a namespace
**
** Arguments :  f_pContextDST - HDS context
**              f_pNamespace  - Namespace identifier to be opened/created
**              f_eOpenMode   - eDRM_DST_CREATE_NEW / eDRM_DST_OPEN_EXISTING
**              f_wMaxNumChildren - Number of children if a new namespace is to
**                                  be created
**              f_dwExpectedSlotSize - Expected size in bytes of slots in this namespace.
**                                     Used for the Bloom filter optimization. Best guess
**                                     is adequate.
**              f_pContextNS  - Namespace context
**
** Returns :    DRM_E_DST_NAMESPACE_NOT_FOUND - if namespace is not found and
**                  request is only to open existing existing namespace
**
** Notes :
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HDS_OpenNamespace(
    __inout                            DRM_DST_CONTEXT           *f_pContextDST,
    __in    const                      DRM_DST_NAMESPACE         *f_pNamespace,
    __in                               DRM_DST_OPENMODE           f_eOpenMode,
    __in                               DRM_WORD                   f_wMaxNumChildren,
    __in                               DRM_DWORD                  f_dwExpectedSlotSize,
    __out                              DRM_DST_NAMESPACE_CONTEXT *f_pContextNS )
{
    DRM_RESULT   dr     = DRM_E_DST_NAMESPACE_NOT_FOUND;
    _HdsContext *pHDS   = (_HdsContext*)f_pContextDST;
    DEFINE_LOCK_VARS

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDS, PERF_FUNC_DRM_HDS_OpenNamespace);

    DRMASSERT( ( f_eOpenMode & eDRM_DST_CREATE_NEW ) != 0
            || ( f_eOpenMode & eDRM_DST_OPEN_EXISTING ) != 0
            || ( f_eOpenMode & eDRM_DST_OPEN_ALWAYS   ) != 0 );
    DRMASSERT( f_pContextDST != NULL );
    __analysis_assume( f_pContextDST != NULL );

    /* Lock SRN EXCLUSIVE */
    LOCK_SRN_AND_ENTER_CS(pHDS, (DRM_DWORD)(f_eOpenMode | eDRM_DST_LOCKEXCLUSIVE));

    if( (pHDS->oSRN.nNsStoreRootBlockNum == 0) )
    {
        /*
        ** If we're doing eDRM_DST_CREATE_NEW or eDRM_DST_OPEN_ALWAYS,
        ** we're about to assume that if nNsStoreRootBlockNum is zero,
        ** then the NamespaceStore has not been created, and then we're going to
        ** create the namespace which will include creating the NamespaceStore.
        ** However, this assumption may be invalid if another process has
        ** created the NamespaceStore in the meantime.
        ** If that happens, we'll end up calling _HdsCreateNamespace when
        ** the namespace already exists which will fail with
        ** DRM_E_DST_NAMESPACE_EXISTS.  This is obviously not what we want.
        ** So, load the SRN at this point to ensure it's up to date.
        ** We're inside the SRN file lock now, so this is process-safe.
        */
        ChkDR(_HdsLoadSRN(pHDS));
    }

    dr = DRM_E_DST_NAMESPACE_NOT_FOUND;
    if ( (pHDS->oSRN.nNsStoreRootBlockNum > 0) )
    {
        /* open existing */
        dr = _HdsOpenExistingNamespace(pHDS, f_pNamespace, f_pContextNS);
        if ( DRM_SUCCEEDED(dr) )
        {
            goto ErrorExit;
        }
    }

    /* namespace not found and caller ask to create new */
    if ( dr == DRM_E_DST_NAMESPACE_NOT_FOUND
      && ( ( f_eOpenMode & eDRM_DST_CREATE_NEW  ) != 0
        || ( f_eOpenMode & eDRM_DST_OPEN_ALWAYS ) != 0 ) )
    {
        ChkDR( _HdsCreateNamespace(
                    pHDS,
                    f_pNamespace,
                    f_wMaxNumChildren,
                    f_dwExpectedSlotSize,
                    (f_eOpenMode & eDRM_DST_LOCKWAIT)!=0 ) );

        dr = _HdsOpenExistingNamespace(pHDS, f_pNamespace, f_pContextNS);
    }

ErrorExit:

    LEAVE_CS_AND_UNLOCK_SRN(pHDS);

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/*
**
*/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HDS_CloseNamespace(
    __inout DRM_DST_NAMESPACE_CONTEXT *f_pContextNS )
{
    DRM_RESULT    dr              = DRM_SUCCESS;
    _NsContext   *pUserNsCFG      = (_NsContext *)f_pContextNS;
#if DBG
    _HdsContext  *pHDS            = NULL;
    _SlotContext *pNsStoreSlotCtx = NULL;
#endif
    DEFINE_LOCK_VARS


    ChkArg(f_pContextNS != NULL
        && ISVALIDCONTEXT(pUserNsCFG, eCfgContextSignature));

    DRMASSERT(pUserNsCFG->fInited
           && pUserNsCFG->pHDS != NULL
           && pUserNsCFG->pHDS->fp != OEM_INVALID_HANDLE_VALUE );

#if DBG
{
    /* let's check if the namespace is valid
    ** this may be redandunt so just do it in DBG build
    */

    DRM_DST_KEY   oNsHashKey;
    DRM_DST_KEY   oNsUniqueKey;
    DRM_BOOL      fResult=FALSE;
    DRM_DWORD     cbNsStoreSlotCtx=0;
    _NsContext    oNsStoreCFG;

    __analysis_assume( pUserNsCFG->pHDS != NULL );
    pHDS = pUserNsCFG->pHDS;

    /* lock SRN SHARED among other processes */
    LOCK_SRN_AND_ENTER_CS(pHDS, (eDRM_DST_LOCKSHARED | eDRM_DST_LOCKWAIT));

    /* First, set up a NS context for accessing the NamespaceStore */
    ChkDR(_HdsInitNsContext(pUserNsCFG->pHDS, &oNsStoreCFG, NULL,
        pUserNsCFG->pHDS->oSRN.nNsStoreRootBlockNum, NsStoreNumofChildren, 0, 0));

    /* Second, alloc buffer and setup _SlotContext (pNsStoreSlotCtx) to store
    ** the given namespace in NamespaceStore
    */
    ChkOverflowSLOTCONTEXTLEN(&oNsStoreCFG);
    cbNsStoreSlotCtx = CALC_SLOTCONTEXTLEN(&oNsStoreCFG);  /* size of slot context */
    ChkDR(_Hds_malloc(pUserNsCFG->pHDS, cbNsStoreSlotCtx, (DRM_VOID**)&pNsStoreSlotCtx));
    ChkDR(_HdsInitSlotContext(&oNsStoreCFG, (DRM_BYTE *)pNsStoreSlotCtx,
        cbNsStoreSlotCtx));

    /* Last, search if the namespace is still exist in file */
    _GenNamespaceKeys(&pUserNsCFG->pHDS->contextMD5, (DRM_CHAR *)pUserNsCFG->oNamespace,
        DRM_HDS_NSNAME_SIZE, oNsHashKey.rgb, oNsUniqueKey.rgb);
    ChkDR(_HdsSearchSlotInFile(&oNsStoreCFG, &oNsHashKey, &oNsUniqueKey, eSearchNamespace,
        NULL, pNsStoreSlotCtx, &fResult));
    if ( !fResult )
    {
        TRACE(("DRM_HDS_CloseNamespace(): namespace '%s' does not exist.\n", pUserNsCFG->oNamespace));
        ChkDR(DRM_E_DST_NAMESPACE_NOT_FOUND);
    }
}
#endif /* DBG */

    /* wipe the context */
    ZEROMEM(f_pContextNS, SIZEOF(_NsContext));

ErrorExit:

#if DBG
    if ( DRM_FAILED( dr ) && dr != DRM_E_INVALIDARG )
    {
        DRMASSERT( FALSE );

        dr = DRM_SUCCESS;
    }

    if ( pNsStoreSlotCtx )
    {
        _Hds_free(pHDS, pNsStoreSlotCtx);
    }

    LEAVE_CS_AND_UNLOCK_SRN(pHDS);
#endif

    return dr;
}

/*
**  This function is not being used anywhere. It was included for completeness
**  of the HDS API. It will be updated if needed.
**
**  IMPORTANT
**  NOTE: If this function ever needs to be used, the namespace cache should be
**        disabled on concurrent context systems. If a namespace is deleted in one
**        context, the entry will still be in the cache of the other context. This
**        shouldn't be a major issue because you're going to error out even without
**        the cache if you try to access a deleted namespace. But it needs to be
**        considered.
*/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HDS_DeleteNamespace(
    __inout    DRM_DST_CONTEXT   *f_pContextDST,
    __inout    DRM_DST_NAMESPACE *f_pNS,
    __in       DRM_BOOL           f_fWait )
{
    DRM_RESULT         dr               = DRM_SUCCESS;
    DRM_DST_KEY        oNsHashKey       = {0};
    DRM_DST_KEY        oNsUniqueKey     = {0};
    DRM_BOOL           fResult          = FALSE;
    _HdsContext       *pHDS             = (_HdsContext*)f_pContextDST;
    _NsContext         oNsStoreCFG      = {0};
    _SlotContext      *pNsStoreSlotCtx  = NULL;
    DRM_DWORD          cbNsStoreSlotCtx = 0;
    DRM_DWORD          cbNsSlotSize     = 0;
    _NsContext        *pcontextNS       = NULL;
    _NSEntry          *pNsEntry         = NULL;
    DEFINE_LOCK_VARS

    ChkArg(f_pContextDST != NULL
        && f_pNS         != NULL
        && ISVALIDCONTEXT(pHDS, eHdsContextSignature));

    DRMASSERT(pHDS->fInited
          &&  pHDS->fp!=OEM_INVALID_HANDLE_VALUE);

    /*
    ** Lock STORE EXCLUSIVE among other processes. So if there are any
    ** locked slots, this call will fail, or wait until all locks are released
    */
    LOCK_STORE( pHDS,  (DRM_DWORD)( eDRM_DST_LOCKEXCLUSIVE | (f_fWait ? eDRM_DST_LOCKWAIT : 0) ) );
    ENTER_HDS_CS(pHDS);

    /*  First, set up a HDS Config (oNsStoreCFG) for accessing the NamespaceStore
    */
    ChkDR(_HdsInitNsContext(pHDS, &oNsStoreCFG, NULL, pHDS->oSRN.nNsStoreRootBlockNum,
        NsStoreNumofChildren, 0, 0));

    /* Second, alloc buffer and setup _SlotContext (pNsStoreSlotCtx) to store the given
    ** namespace in NamespaceStore
    */
    ChkOverflowSLOTCONTEXTLEN(&oNsStoreCFG);
    cbNsStoreSlotCtx = CALC_SLOTCONTEXTLEN(&oNsStoreCFG);  /* size of slot context */
    ChkDR(_Hds_malloc(pHDS, cbNsStoreSlotCtx, (DRM_VOID**)&pNsStoreSlotCtx));
    ChkDR(_HdsInitSlotContext(&oNsStoreCFG, (DRM_BYTE *)pNsStoreSlotCtx, cbNsStoreSlotCtx));

    /* Third, delete the namespace from our cache
    */
    ChkDR( _HdsDeleteNSCacheEntry( pHDS, f_pNS ) );

    /* Fourth, prepare the namespace key and search it in NamespaceStore
    */
    _GenNamespaceKeys (&pHDS->contextMD5,
           (DRM_CHAR *) f_pNS->rgbOpaqueData,
                        DRM_HDS_NSNAME_SIZE,
                        oNsHashKey.rgb,
                        oNsUniqueKey.rgb);
    ChkDR(_HdsSearchSlotInFile(&oNsStoreCFG, &oNsHashKey, &oNsUniqueKey, eSearchNamespace,
        &cbNsSlotSize, pNsStoreSlotCtx, &fResult));
    if ( !fResult )
    {
        TRACE(("DRM_HDS_DeleteNamespace(): namespace '%s' does not exist.\n", f_pNS->rgbOpaqueData));
        ChkDR(DRM_E_DST_NAMESPACE_NOT_FOUND);
    }
    else
    {
        DRM_DST_KEY   oHashKey;
        DRM_DST_KEY oUniqueKey;

        ChkDR( _HdsReadNsEntryFromDisk( pHDS, pNsStoreSlotCtx, cbNsSlotSize, &pNsEntry ) );

        /* setup cfg context for the namespace */
        ChkDR(_Hds_malloc(pHDS, SIZEOF(_NsContext), (DRM_VOID**)&pcontextNS));
        ChkDR(_HdsInitNsContext(pHDS, pcontextNS, f_pNS, pNsEntry->nNSRBlockNum,
            pNsEntry->wMaxNumChildren, pNsEntry->wBloomBytesM, pNsEntry->wBloomK));

        /* delete the namespace's subtree */
        ChkDR(_HdsDeleteSubTree(pcontextNS, pNsEntry->nNSRBlockNum));

        /* delete the Config's slot in NamespaceStore */
        /*ChkDR(_HdsCloseSlot(pNsStoreSlotCtx)); */

        MEMCPY(oHashKey.rgb,   oNsHashKey.rgb,   DRM_HDS_HASHKEY_SIZE);
        MEMCPY(oUniqueKey.rgb, oNsUniqueKey.rgb, DRM_HDS_UNIQUEKEY_SIZE);

        ChkDR( _HdsRemoveSlot(pNsStoreSlotCtx, eRemoveSlotPermanent ) );
    }

ErrorExit:
    if ( pcontextNS )
    {
        _Hds_free(pHDS, pcontextNS);
    }
    if ( pNsEntry )
    {
        _Hds_free(pHDS, pNsEntry);
    }
    if ( pNsStoreSlotCtx )
    {
        _Hds_free(pHDS, pNsStoreSlotCtx);
    }

    LEAVE_HDS_CS(pHDS);
    UNLOCK_STORE(pHDS);
    return dr;
}

/*
**
*/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HDS_DeleteSlot(
    __in       DRM_DST_NAMESPACE_CONTEXT *f_pContextNS, /* Namespace context returned from DRM_HDS_OpenNamespace */
    __in const DRM_DST_KEY               *f_pHashKey,
    __in const DRM_DST_KEY               *f_pUniqueKey,
    __in_opt   DRM_DST_SLOT_HINT         *f_pSlotHint,
    __in       DRM_BOOL                   f_fWait )
{
    DRM_RESULT         dr                                          = DRM_SUCCESS;
    _NsContext        *pNS                                         = (_NsContext*)f_pContextNS;
    _SlotContext      *pSlotCtx                                    = NULL;
    DRM_BOOL           fValid                                      = FALSE;
    DRM_BOOL           fBlockLocked                                = FALSE;
    DRM_DWORD          cbSlotSize                                  = 0;
    DRM_BYTE           rgbBuff [__CB_DECL(CALC_MAXSLOTCONTEXTLEN)] = {0};
    DEFINE_LOCK_VARS

    ChkArg(f_pContextNS != NULL
        && ISVALIDCONTEXT(pNS, eCfgContextSignature));

    DRMASSERT(pNS->fInited
          &&  f_pHashKey    != NULL
          &&  f_pUniqueKey  != NULL
          &&  pNS->pHDS     != NULL
          &&  pNS->pHDS->fp != OEM_INVALID_HANDLE_VALUE);

    /* initialize slot context structure */

    pSlotCtx = (_SlotContext *) rgbBuff;

    ChkOverflowSLOTCONTEXTLEN( pNS );
    ChkDR(_HdsInitSlotContext(pNS, (DRM_BYTE*)pSlotCtx, CALC_SLOTCONTEXTLEN(pNS)));

    while ( TRUE )
    {
        /* Lock SRN EXCLUSIVE among other processes. */
        LOCK_SRN_AND_ENTER_CS( pNS->pHDS, (DRM_DWORD)( eDRM_DST_LOCKEXCLUSIVE | (f_fWait ? eDRM_DST_LOCKWAIT : 0) ) );

        if ( f_pSlotHint != NULL )
        {
            dr = _HdsOpenSlotFromHint(pNS, f_pHashKey, f_pUniqueKey, (const _SlotHint*)f_pSlotHint, TRUE, &cbSlotSize, pSlotCtx);
        }

        if ( f_pSlotHint == NULL
            || DRM_FAILED ( dr ) )
        {
            ChkDR( _HdsOpenSlot(pNS, f_pHashKey, f_pUniqueKey, TRUE, eDRM_DST_OPEN_EXISTING, FALSE, &cbSlotSize, pSlotCtx) );
        }

        ChkDR( _HdsLockBlock2DeleteSlot(
            pSlotCtx,
            (DRM_DWORD)( eDRM_DST_LOCKEXCLUSIVE | (f_fWait ? eDRM_DST_LOCKWAIT : 0) ) ) );
        fBlockLocked = TRUE;

        ChkDR(_HdsVerifySlotContext(pSlotCtx, &fValid));

        if ( fValid )
        {
            break;
        }

        /* slot context verified failed, try again */
        (void)_HdsUnlockBlock2DeleteSlot(pSlotCtx);
        fBlockLocked = FALSE;

        /* drop all the locks and CS again */
        LEAVE_CS_AND_UNLOCK_SRN( pNS->pHDS );
    }

    /*
    **  We currently have the lock on the block as well as CS and SRN lock
    */
    ChkDR( _HdsRemoveSlot(pSlotCtx, eRemoveSlotPermanent) );

    if ( f_pSlotHint != NULL )
    {
        ZEROMEM ( f_pSlotHint, DRM_HDS_SLOT_HINT_LEN );
    }

ErrorExit:

    if ( fBlockLocked )
    {
        /* error occured during verifying slot context, return error */
        (void)_HdsUnlockBlock2DeleteSlot(pSlotCtx);
    }

    if (pNS != NULL)
    {
        LEAVE_CS_AND_UNLOCK_SRN(pNS->pHDS);
    }
    return dr;
}

/* Initialize a slot hint based on the slot context passed */
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM, "Parameters must match generic drmdatastore.h interface")
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HDS_MakeSlotHint(
    __in        DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __out       DRM_DST_SLOT_HINT     *f_pSlotHint )
PREFAST_POP
{
    DRM_RESULT          dr        = DRM_SUCCESS;
    const _SlotContext *pSlotCtx  = (const _SlotContext*)f_pContextSlot;
    _SlotHint          *pHint     = (_SlotHint*)f_pSlotHint;

    ChkArg(ISVALIDCONTEXT(pSlotCtx, eSlotContextSignature));
    DRMASSERT(f_pContextSlot != NULL
          &&  f_pSlotHint    != NULL
          &&  pSlotCtx->pNS != NULL
          &&  pSlotCtx->pNS->pHDS != NULL
          &&  pSlotCtx->pNS->pHDS->fp!=OEM_INVALID_HANDLE_VALUE);

    pHint->nBlockNum = pSlotCtx->oFileBlock.nBlockNum;
    pHint->nSlotPosInBlock = pSlotCtx->dwSlotPosInBlock;

ErrorExit:
    if( DRM_FAILED( dr )
        && pHint != NULL )
    {
        ZEROMEM( pHint, SIZEOF( _SlotHint ) );
    }

    return dr;
}

/*
** Create or open dataslot using given key1 and key2
*/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HDS_OpenSlot(
    __in       DRM_DST_NAMESPACE_CONTEXT *f_pContextNS,/* Namespace context returned from DRM_HDS_OpenNamespace */
    __in       DRM_DWORD                  f_dwMode,
    __in const DRM_DST_KEY               *f_pHashKey,
    __in const DRM_DST_KEY               *f_pUniqueKey,
    __in_opt   DRM_DST_SLOT_HINT         *f_pSlotHint,
    __inout    DRM_DWORD                 *f_pcbSlotSize,/* current dataslot size */
    __out      DRM_DST_SLOT_CONTEXT      *f_pContextSlot )/* user given Dataslot context buffer */
{
    DRM_RESULT    dr=DRM_SUCCESS;
    _NsContext   *pNS=(_NsContext*)f_pContextNS;
    _SlotContext *pSlotCtx=NULL;
    DRM_BOOL      fSlotLocked = FALSE;
    DRM_BOOL      fValid      = FALSE;
    DEFINE_LOCK_VARS

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDS, PERF_FUNC_DRM_HDS_OpenSlot);

    ChkArg(ISVALIDCONTEXT(pNS, eCfgContextSignature)
        && f_pHashKey != NULL
        && f_pUniqueKey != NULL
        && !DRM_UTL_IsZeros(( DRM_BYTE * )f_pHashKey, DRM_HDS_HASHKEY_SIZE));
    DRMASSERT(f_pContextNS != NULL
          && f_pContextSlot != NULL
          && pNS->fInited
          && pNS->nCfgRootBlockNum > 0
          && pNS->pHDS != NULL
          && pNS->pHDS->fp != OEM_INVALID_HANDLE_VALUE
          && ((f_dwMode & eDRM_DST_CREATE_NEW) || (f_dwMode & eDRM_DST_OPEN_EXISTING) || (f_dwMode & eDRM_DST_OPEN_ALWAYS)) );

    /* check slot size */
    if ( (DRM_DWORD)(SIZEOF(_SlotHeader) + (*f_pcbSlotSize)) < (*f_pcbSlotSize) )
    {
        /* (*f_pcbSlotSize) is a negative number */
        ChkDR(DRM_E_INVALID_DST_SLOT_SIZE);
    }

    /* initialize slot context structure */
    ChkDR(_HdsInitSlotContext(pNS, (DRM_BYTE*)f_pContextSlot, CALC_MAXSLOTCONTEXTLEN));
    pSlotCtx = (_SlotContext*)f_pContextSlot;

    while ( TRUE )
    {
        /* Lock SRN EXCLUSIVE among other processes if we may have to create the slot,
        ** otherwise if we're only opening an existing slot we can lock SRN SHARED. */
        LOCK_SRN_AND_ENTER_CS(pNS->pHDS,
            (f_dwMode & eDRM_DST_OPEN_EXISTING) ? (f_dwMode & (~eDRM_DST_LOCKEXCLUSIVE)) : (f_dwMode | eDRM_DST_LOCKEXCLUSIVE));


        /* Try opening the slot with the provided hint if we were given one */
        ChkDR( DRM_SUCCESS );
        if ( f_pSlotHint != NULL )
        {
            dr = _HdsOpenSlotFromHint(pNS, f_pHashKey, f_pUniqueKey, (const _SlotHint*)f_pSlotHint, TRUE, f_pcbSlotSize, pSlotCtx);

            /* slot exists but caller ask to create new only */
            if ( DRM_SUCCEEDED(dr)  &&  (f_dwMode & eDRM_DST_CREATE_NEW) != 0 )
            {
                ChkDR(DRM_E_DST_SLOT_EXISTS);
            }
        }

        /* If the slot hint open failed or if we weren't given a hint at all, try doing a regular open  */
        if( DRM_FAILED( dr ) || f_pSlotHint == NULL )
        {
            ChkDR( _HdsOpenSlot(pNS,
                                f_pHashKey,
                                f_pUniqueKey,
                                TRUE,
                                f_dwMode,
                                0 != (f_dwMode & eDRM_DST_OPEN_EXISTING),
                                f_pcbSlotSize,
                                pSlotCtx ) );
        }

        /* MUST persist dr from _HdsOpenSlot for the block below the following block */
        if ( f_pSlotHint != NULL )
        {
            DRM_HDS_MakeSlotHint(f_pContextSlot, f_pSlotHint);
        }

        /* If the slot was created, we're done */
        if (  ( ( ( f_dwMode & eDRM_DST_CREATE_NEW    ) != 0 ) && DRM_SUCCEEDED( dr ) )
           || ( ( ( f_dwMode & eDRM_DST_OPEN_ALWAYS   ) != 0 ) && dr == DRM_S_FALSE ) )
        {
            fSlotLocked = TRUE;
            break;
        }

        /* here, a slot exists and caller is asking to open existing ...
        */

        /* issue specific slot lock */
        ChkDR(_HdsLockSlot(pSlotCtx, f_dwMode));
        fSlotLocked = TRUE;

        ChkDR(_HdsVerifySlotContext(pSlotCtx, &fValid));

        /* drop all the locks and CS again */
        LEAVE_CS_AND_UNLOCK_SRN(pNS->pHDS);

        if ( fValid )
        {
            break;
        }

        /* slot context verified failed, try again */
        (void)_HdsUnlockSlot(pSlotCtx);
        fSlotLocked = FALSE;
    }

ErrorExit:
    if ( pNS != NULL  &&  pNS->pHDS != NULL )
    {
        if ( DRM_FAILED(dr)  &&  fSlotLocked )
        {
            /* error occured during verifying slot context, return error */
            _HdsUnlockSlot(pSlotCtx);
        }

        LEAVE_CS_AND_UNLOCK_SRN(pNS->pHDS);
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*
**
*/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HDS_CloseSlot(
    __inout  DRM_DST_SLOT_CONTEXT  *f_pContextSlot )
{
    DRM_RESULT    dr       = DRM_SUCCESS;
    _SlotContext *pSlotCtx = (_SlotContext*)f_pContextSlot;
    DEFINE_LOCK_VARS

    ChkArg(f_pContextSlot != NULL
        && ISVALIDCONTEXT(pSlotCtx, eSlotContextSignature));

    DRMASSERT(pSlotCtx->pNS
           && pSlotCtx->pNS->pHDS
           && pSlotCtx->pNS->pHDS->fp != OEM_INVALID_HANDLE_VALUE);

    if (pSlotCtx->eStatus == eSlotCtxUninit)
    {
        ChkDR(DRM_E_DST_SLOT_NOT_FOUND);
    }

    ChkDR(_HdsUnlockSlot(pSlotCtx));
    ChkDR(_HdsCloseSlot(pSlotCtx));

ErrorExit:
    return dr;
}


/*
**
*/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HDS_SlotSeek(
    __inout   DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __in      DRM_LONG               f_lOffset,
    __in      DRM_DST_SEEKMODE       f_eOrigin,
    __out_opt DRM_DWORD             *f_pdwSeekPointer )/* optional parameter, can be NULL */
{
    DRM_RESULT    dr       = DRM_SUCCESS;
    _SlotContext *pSlotCtx = (_SlotContext*)f_pContextSlot;

    ChkArg(f_pContextSlot != NULL
        && ISVALIDCONTEXT(pSlotCtx, eSlotContextSignature));

    DRMASSERT(pSlotCtx->pNS           != NULL
           && pSlotCtx->pNS->pHDS     != NULL
           && pSlotCtx->pNS->pHDS->fp != OEM_INVALID_HANDLE_VALUE);

    /* Note:
    ** - We do not need to enter CS because we do not need access to file handle
    **   and Local stack
    ** - this slot is either locked for SHARED or EXCLUSIVE. We do not need to
    **   worry if it is changed after it is opened
    */
    if (pSlotCtx->eStatus!=eSlotCtxReady)
    {
        ChkDR(DRM_E_DST_SLOT_NOT_FOUND);
    }
    switch ( f_eOrigin )
    {
        case eDRM_DST_SEEKCUR:
            if ( f_lOffset > 0 )
            {
                if ( (pSlotCtx->dwSeekPointer+f_lOffset) > pSlotCtx->oSlotHeader.dwSlotSize )
                {
                    ChkDR(DRM_E_DST_SEEK_ERROR);
                }
                pSlotCtx->dwSeekPointer += f_lOffset;
            }
            else
            {
                if ( pSlotCtx->dwSeekPointer < (DRM_DWORD)DRMCRT_abs(f_lOffset) )
                {
                    ChkDR(DRM_E_DST_SEEK_ERROR);
                }
                pSlotCtx->dwSeekPointer -= DRMCRT_abs(f_lOffset);
            }
            break;
        case eDRM_DST_SEEKEND:
            if ( f_lOffset > 0 || (DRM_DWORD)DRMCRT_abs(f_lOffset) > pSlotCtx->oSlotHeader.dwSlotSize )
            {
                ChkDR(DRM_E_DST_SEEK_ERROR);
            }
            pSlotCtx->dwSeekPointer = pSlotCtx->oSlotHeader.dwSlotSize - DRMCRT_abs(f_lOffset);
            break;
        case eDRM_DST_SEEKSET:
            if ( f_lOffset < 0 || f_lOffset > (DRM_LONG) pSlotCtx->oSlotHeader.dwSlotSize )
            {
                ChkDR(DRM_E_DST_SEEK_ERROR);
            }
            pSlotCtx->dwSeekPointer = (DRM_DWORD)f_lOffset;
            break;
        default:
            ChkDR(DRM_E_INVALIDARG);
    }

ErrorExit:
    if ( DRM_SUCCEEDED(dr) && f_pdwSeekPointer)
    {
        *f_pdwSeekPointer = pSlotCtx->dwSeekPointer;
    }
    return dr;
}



/*
**
*/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HDS_SlotRead(
    __inout                  DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __in                     DRM_DWORD              f_cbData,
    __out_bcount( f_cbData ) DRM_BYTE              *f_pbData,
    __out_opt                DRM_DWORD             *f_pcbRead )
{
    DRM_RESULT    dr=DRM_SUCCESS;
    DRM_BOOL      fValidHDSContext = FALSE;
    _SlotContext *pSlotCtx=(_SlotContext*)f_pContextSlot;

    DEFINE_LOCK_VARS

    if ( f_pcbRead != NULL )
    {
        *f_pcbRead = 0;
    }

    ChkArg(ISVALIDCONTEXT(pSlotCtx, eSlotContextSignature));
    DRMASSERT(f_pContextSlot != NULL
          &&  pSlotCtx->pNS  != NULL
          &&  pSlotCtx->pNS->pHDS != NULL
          &&  pSlotCtx->pNS->pHDS->fp!=OEM_INVALID_HANDLE_VALUE);

    /* Note:
    ** - this slot is either locked for SHARED or EXCLUSIVE. We do not need to
    **   worry if it is changed after it is opened
    */

    /* Lock Order:
    ** - CS:  to guard exclusive access to file handle
    */
    __analysis_assume( pSlotCtx->pNS != NULL );
    __analysis_assume( pSlotCtx->pNS->pHDS != NULL );
    ENTER_HDS_CS(pSlotCtx->pNS->pHDS);

    fValidHDSContext = ( f_pContextSlot != NULL
                     &&  pSlotCtx->pNS != NULL
                     &&  pSlotCtx->pNS->pHDS != NULL
                     &&  pSlotCtx->pNS->pHDS->fp!=OEM_INVALID_HANDLE_VALUE );

    ChkDR(_HdsReadWriteSlot(pSlotCtx, f_cbData, f_pbData, f_pcbRead, TRUE));

ErrorExit:

    if( fValidHDSContext == TRUE )
    {
        LEAVE_HDS_CS(pSlotCtx->pNS->pHDS);
    }

    return dr;
}



/*
**
*/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HDS_SlotWrite(
    __inout                       DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __in                          DRM_DWORD              f_cbData,
    __in_bcount( f_cbData ) const DRM_BYTE              *f_pbData,
    __out                         DRM_DWORD             *f_pcbWritten )
{
    DRM_RESULT    dr=DRM_SUCCESS;
    DRM_BOOL      fValidHDSContext = FALSE;
    _SlotContext *pSlotCtx=(_SlotContext*)f_pContextSlot;
    DEFINE_LOCK_VARS

    ChkArg(ISVALIDCONTEXT(pSlotCtx, eSlotContextSignature));
    DRMASSERT(f_pContextSlot != NULL
          &&  pSlotCtx->pNS != NULL
          &&  pSlotCtx->pNS->pHDS != NULL
          &&  pSlotCtx->pNS->pHDS->fp!=OEM_INVALID_HANDLE_VALUE);

    /* Note:
    ** - this slot is locked for EXCLUSIVE. We do not need to
    **   worry if it is changed after it is opened
    */

    /* slot must be locked EXCLUSIVE */
    if ( (pSlotCtx->eLockMode & eDRM_DST_LOCKEXCLUSIVE) == 0 )
    {
        ChkDR(DRM_E_DST_NOT_LOCKED_EXCLUSIVE);
    }

    /* Lock Order:
    ** - CS:  to guard exclusive access to file handle
    */
    __analysis_assume( pSlotCtx->pNS != NULL );
    __analysis_assume( pSlotCtx->pNS->pHDS != NULL );
    ENTER_HDS_CS(pSlotCtx->pNS->pHDS);

    fValidHDSContext = ( f_pContextSlot != NULL
                     &&  pSlotCtx->pNS  != NULL
                     &&  pSlotCtx->pNS->pHDS != NULL
                     &&  pSlotCtx->pNS->pHDS->fp!=OEM_INVALID_HANDLE_VALUE );

    /* NOTE: We cast away the const-ness here because we know _HdsReadWriteSlot
    ** is safe when the operation is Write
    */
    ChkDR(_HdsReadWriteSlot(pSlotCtx, f_cbData, (DRM_BYTE*)f_pbData, f_pcbWritten, FALSE));

    /*
    ** Note:
    ** we do not need to flush if _DATASTORE_WRITE_THRU is defined. file buffers
    ** are already flushed.
    ** if _DATASTORE_WRITE_THRU is not defined, caller should call DRM_HDS_Commit*
    ** to flush file buffer at higher level
    */

ErrorExit:

    if( fValidHDSContext == TRUE )
    {
        LEAVE_HDS_CS(pSlotCtx->pNS->pHDS);
    }

    return dr;
}


DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HDS_SlotResize(
    __inout  DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __in     DRM_DWORD              f_cbSize )
{
    DRM_RESULT     dr=DRM_SUCCESS;
    _SlotContext  *pSlotCtx=(_SlotContext*)f_pContextSlot;
    DEFINE_LOCK_VARS

    ChkArg(ISVALIDCONTEXT(pSlotCtx, eSlotContextSignature));
    DRMASSERT(f_pContextSlot != NULL
          &&  pSlotCtx->pNS  != NULL
          &&  pSlotCtx->pNS->pHDS != NULL
          &&  pSlotCtx->pNS->pHDS->fp != OEM_INVALID_HANDLE_VALUE
          &&  f_cbSize > 0);

    /* we do not need to verify slot content since we must have either shared
    ** or exclusive lock on the slot
    */
    if (pSlotCtx->eStatus!=eSlotCtxReady)
    {
        ChkDR(DRM_E_DST_SLOT_NOT_FOUND);
    }

    /* We need to acquire an exclusive SRN lock to be able to resize the slot,
    ** but we can't just directly acquire the SRN lock while we hold a slot lock.
    ** Need to call _HdsReopenSlotWithSRNLock to close then re-open the slot after
    ** having acquired an exclusive SRN lock
    */
    ChkDR( _HdsReopenSlotWithSRNLock( pSlotCtx, (DRM_DWORD)( eDRM_DST_LOCKEXCLUSIVE | eDRM_DST_LOCKWAIT ), (DRM_DWORD)( pSlotCtx->eLockMode & eDRM_DST_LOCKMODE_MASK ) ) );

#if DRM_SUPPORT_MULTI_THREADING
    fSRNLocked = TRUE;
#endif /* DRM_SUPPORT_MULTI_THREADING */

    ChkDR(_HdsSlotResize(pSlotCtx, f_cbSize));

ErrorExit:
    if ( pSlotCtx!=NULL  &&  pSlotCtx->pNS!=NULL )
    {
        UNLOCK_SRN(pSlotCtx->pNS->pHDS);
    }
    return dr;
}



DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HDS_InitSlotEnum(
    __in           DRM_DST_NAMESPACE_CONTEXT *f_pContextNS, /* Namespace context returned from DRM_HDS_OpenNamespace */
    __in_opt const DRM_DST_KEY               *f_pHashKey,
    __in           DRM_DST_LOCKMODE           f_eLockMode,
    __out          DRM_DST_ENUM_CONTEXT      *f_pContextEnum )
{
    DRM_RESULT    dr    = DRM_SUCCESS;
    _NsContext   *pNS   = (_NsContext   *) f_pContextNS;
    _EnumContext *pEnum = (_EnumContext *) f_pContextEnum;
    DEFINE_LOCK_VARS

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDS, PERF_FUNC_DRM_HDS_InitSlotEnum);

    ChkArg( f_pContextNS != NULL );
    ChkArg( f_pContextEnum != NULL );

    ChkArg( pNS->pHDS != NULL );
    ChkArg( pNS->pHDS->fp != OEM_INVALID_HANDLE_VALUE );
    ChkArg( ISVALIDCONTEXT(pNS, eCfgContextSignature) );
    ChkArg( ISVALIDCONTEXT(pNS->pHDS, eHdsContextSignature) );

    /* Lock SRN SHARED among other processes. */
    LOCK_SRN_AND_ENTER_CS(pNS->pHDS, (DRM_DWORD)((f_eLockMode & (~eDRM_DST_LOCKEXCLUSIVE))));

    /* _HdsInitSlotEnum prepares the enum context. It will not block so we are okay */
    ChkDR(_HdsInitSlotEnum(pNS, f_pHashKey, f_eLockMode, pEnum));

ErrorExit:

    if ( pNS != NULL )
    {
        LEAVE_CS_AND_UNLOCK_SRN(pNS->pHDS);  /* unlock SRN */
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HDS_SlotEnumReloadCurrent(
    __inout    DRM_DST_ENUM_CONTEXT  *f_pContextEnum,       /* setup by DRM_HDS_InitSlotEnum() */
    __out      DRM_DST_SLOT_CONTEXT  *f_pContextSlot,       /* user given Dataslot context buffer */
    __out_opt  DRM_DST_KEY           *f_pHashKey,           /* optional parameter */
    __out_opt  DRM_DST_KEY           *f_pUniqueKey,         /* optional parameter */
    __out_opt  DRM_DWORD             *f_pcbSlot )           /* current dataslot size, optional parameter */
{
    DRM_RESULT     dr         = DRM_SUCCESS;
    _EnumContext  *pEnum      = (_EnumContext *)f_pContextEnum;
    _CommBlockHDR *pCurrBlock = NULL;

    PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_REDUNDANTTEST, "Ignore prefast warning about redundant checks.")
    ChkArg(pEnum != NULL
       &&  pEnum->pNS != NULL
       &&  ISVALIDCONTEXT(pEnum,      eEnumContextSignature)
       &&  ISVALIDCONTEXT(pEnum->pNS, eCfgContextSignature));
    DRMASSERT(pEnum->pNS->fInited
          &&  pEnum->pNS->pHDS  != NULL
          &&  pEnum->pNS->pHDS->fp != OEM_INVALID_HANDLE_VALUE
          &&  f_pContextSlot != NULL);
    PREFAST_POP

    if ( !pEnum->fCurrBlockIsValid )
    {
        dr = DRM_E_NOMORE;
        goto ErrorExit;
    }
    pCurrBlock = &pEnum->oCurrBlock;

    /* use next positon as current position */
    ((_ChildBlockHDR*)pCurrBlock)->nNextSlotPos = ((_ChildBlockHDR*)pCurrBlock)->nCurrSlotPos;

    dr = DRM_HDS_SlotEnumNext(f_pContextEnum, f_pContextSlot, f_pHashKey, f_pUniqueKey, f_pcbSlot);

ErrorExit:
    return dr;
}



/******************************************************************************
** Function:   DRM_HDS_SlotEnumDeleteCurrent
** Synopsis:   delete the current enumerated slot
** Arguments:  f_pContextEnum - HDS enum context
**             f_pContextSlot - HDS slot context of the current enumerated slot
** Returns:
** Notes:
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HDS_SlotEnumDeleteCurrent(
    __inout   DRM_DST_ENUM_CONTEXT  *f_pContextEnum,
    __inout   DRM_DST_SLOT_CONTEXT  *f_pContextSlot )
{
    DRM_RESULT    dr       = DRM_SUCCESS;
    _EnumContext *pEnum    = (_EnumContext *) f_pContextEnum;
    _SlotContext *pSlotCtx = (_SlotContext *) f_pContextSlot;
    DEFINE_LOCK_VARS

    PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_REDUNDANTTEST, "Ignore prefast warning about redundant checks.")
    ChkArg(pEnum        != NULL
       &&  pEnum->pNS   != NULL
       &&  f_pContextSlot != NULL
       &&  ISVALIDCONTEXT(pEnum,      eEnumContextSignature)
       &&  ISVALIDCONTEXT(pEnum->pNS, eCfgContextSignature)
       &&  ISVALIDCONTEXT(pSlotCtx, eSlotContextSignature));

    DRMASSERT(pEnum->pNS->fInited
          &&  pEnum->pNS->pHDS        != NULL
          &&  pEnum->pNS->pHDS->fp    != OEM_INVALID_HANDLE_VALUE
          &&  pSlotCtx->pNS
          &&  pSlotCtx->pNS->pHDS
          &&  pSlotCtx->pNS->pHDS->fp != OEM_INVALID_HANDLE_VALUE);
    PREFAST_POP

    if (pSlotCtx->eStatus != eSlotCtxReady
     || !pEnum->fCurrBlockIsValid)
    {
        ChkDR(DRM_E_DST_SLOT_NOT_FOUND);
    }

    /* We need to get an exclusive SRN lock, to do this we must drop our slot lock, then
    ** acquire the exclusive SRN lock, then get back our slot lock again
    */
    ChkDR( _HdsReopenSlotWithSRNLock( pSlotCtx, (DRM_DWORD)( pEnum->eLockMode | eDRM_DST_LOCKEXCLUSIVE ), pEnum->eLockMode ) );
#if DRM_SUPPORT_MULTI_THREADING
    fSRNLocked = TRUE;
#endif /* DRM_SUPPORT_MULTI_THREADING */

    ENTER_HDS_CS(pEnum->pNS->pHDS);

    dr = _HdsSlotEnumDeleteCurrent(pEnum, pSlotCtx);

ErrorExit:

    LEAVE_CS_AND_UNLOCK_SRN(pEnum->pNS->pHDS);
    return dr;
}



DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HDS_SlotEnumNext(
    __inout    DRM_DST_ENUM_CONTEXT  *f_pContextEnum,/* setup by DRM_HDS_InitSlotEnum() */
    __out      DRM_DST_SLOT_CONTEXT  *f_pContextSlot,/* user given Dataslot context buffer */
    __out_opt  DRM_DST_KEY           *f_pHashKey,    /* optional parameter */
    __out_opt  DRM_DST_KEY           *f_pUniqueKey,  /* optional parameter */
    __out_opt  DRM_DWORD             *f_pcbSlot )    /* current dataslot size, optional parameter */
{
    DRM_RESULT    dr = DRM_SUCCESS;
    _EnumContext *pEnum    = (_EnumContext *) f_pContextEnum;
    _SlotContext *pSlotCtx = (_SlotContext *) f_pContextSlot;
    DRM_BOOL      fValid   = FALSE;
    DRM_BOOL      fSlotLocked = FALSE;
    DEFINE_LOCK_VARS

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMHDS, PERF_FUNC_DRM_HDS_SlotEnumNext);

    PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_REDUNDANTTEST, "Ignore prefast warning about redundant checks.")
    ChkArg(pEnum != NULL
       &&  pEnum->pNS != NULL
       &&  ISVALIDCONTEXT(pEnum,      eEnumContextSignature)
       &&  ISVALIDCONTEXT(pEnum->pNS, eCfgContextSignature));
    ChkArg(pEnum->pNS->fInited
          &&  pEnum->pNS->pHDS  != NULL
          &&  pEnum->pNS->pHDS->fp != OEM_INVALID_HANDLE_VALUE
          &&  f_pContextSlot != NULL);
    PREFAST_POP

    if ( !pEnum->fCurrBlockIsValid )
    {
        dr = DRM_E_NOMORE;
        goto ErrorExit;
    }

    while (TRUE)
    {
        /* Lock SRN SHARED among other processes. */
        LOCK_SRN_AND_ENTER_CS(pEnum->pNS->pHDS, (DRM_DWORD)(pEnum->eLockMode & (~eDRM_DST_LOCKEXCLUSIVE)));

        /* locate and open the slot */
        ChkDR(_HdsSlotEnumNext(pEnum, pSlotCtx, f_pHashKey, f_pUniqueKey, f_pcbSlot));

        /* issue specific slot lock */
        ChkDR(_HdsLockSlot(pSlotCtx, pEnum->eLockMode));
        fSlotLocked = TRUE;

        ChkDR(_HdsVerifySlotContext(pSlotCtx, &fValid));

        /* drop all the locks and CS again */
        LEAVE_CS_AND_UNLOCK_SRN(pEnum->pNS->pHDS);

        if ( fValid )
        {
            break;
        }

        /* slot context verified failed, try again */
        (void)_HdsUnlockSlot(pSlotCtx);
        fSlotLocked = FALSE;
    }

ErrorExit:

    if ( pEnum != NULL  && pEnum->pNS )
    {
        if ( DRM_FAILED(dr)  &&  fSlotLocked )
        {
            /* error occured during verifying slot context, return error */
            _HdsUnlockSlot(pSlotCtx);
        }
        LEAVE_CS_AND_UNLOCK_SRN(pEnum->pNS->pHDS);
    }

    if (DRM_FAILED(dr) && pSlotCtx != NULL)
    {
        /* trash the slot context and return */
        ZEROMEM(pSlotCtx, min( SIZEOF( DRM_DST_SLOT_CONTEXT), pSlotCtx->dwContextSize) );
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM, "Parameters must match generic drmdatastore.h interface")
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HDS_BlockScanDelete(
    __inout                          DRM_DST_NAMESPACE_CONTEXT  *f_pContextNS,
    __in_ecount( f_cKeysHash )       DRM_DST_KEY                *f_rgKeyHash,
    __in                             DRM_DWORD                   f_cKeysHash,
    __in                             DRM_DST_KEY                *f_pkeyUnique,
    __in                             DRM_BOOL                    f_fWait )
PREFAST_POP
{
    DRM_RESULT dr = DRM_SUCCESS;
    _NsContext        *pcontextNS = (_NsContext *)f_pContextNS;
    _BlockScanContext  oContextBS = { 0 };
    DRM_DST_LOCKMODE   lockmode   = eDRM_DST_LOCKEXCLUSIVE;
    DRM_BOOL           fOK               = FALSE;
    DRM_BOOL           fLocked           = FALSE;
    const DRM_DST_KEY *pkeyUnique        = NULL;
    _CommBlockHDR     *pblockheaderNext  = NULL;
    _SlotContext      *pcontextSlot      = NULL;
    _SlotContext      *pcontextSlotLock  = NULL;
    DRM_DWORD          cbSlotContext     = 0;
    DEFINE_LOCK_VARS;

    ChkArg(f_pContextNS        != NULL );

    ChkArg(pcontextNS->pHDS     != NULL
       &&  pcontextNS->pHDS->fp != OEM_INVALID_HANDLE_VALUE);

    ChkArg(f_pkeyUnique    != NULL
        && f_rgKeyHash != NULL
        && f_cKeysHash  > 0);

    ChkArg(ISVALIDCONTEXT(pcontextNS,       eCfgContextSignature)
       &&  ISVALIDCONTEXT(pcontextNS->pHDS, eHdsContextSignature));

    if (f_fWait)
    {
        lockmode = (DRM_DST_LOCKMODE)(lockmode | eDRM_DST_LOCKWAIT);
    }

    /* Lock SRN SHARED among other processes. */

    LOCK_SRN_AND_ENTER_CS(pcontextNS->pHDS, eDRM_DST_LOCKSHARED);

    ChkDR(_HdsBlockScanInit  (pcontextNS, f_rgKeyHash, f_cKeysHash, f_pkeyUnique, lockmode, &oContextBS));

    pkeyUnique = &oContextBS.keyUnique;

    ChkOverflowSLOTCONTEXTLEN(oContextBS.pcontextNS);
    cbSlotContext = CALC_SLOTCONTEXTLEN(oContextBS.pcontextNS);
    ChkDR(_Hds_malloc(oContextBS.pcontextNS->pHDS, cbSlotContext, (DRM_VOID **) &pcontextSlot));
    ChkDR(_Hds_malloc(oContextBS.pcontextNS->pHDS, cbSlotContext, (DRM_VOID **) &pcontextSlotLock));

    ChkDR(DRM_HDSIMPL_AllocBlockBuffer(oContextBS.pcontextNS, eCHILDBLOCK, &pblockheaderNext));

    while (dr != DRM_E_NOMORE)
    {
        DRM_BOOL fAnyDeletedFromBlock = FALSE;
        DRM_BOOL fFound               = TRUE;

        fLocked = FALSE;

        /* unlock the SHARED from above or the EXCLUSIVE (below) upon loop */

        LEAVE_CS_AND_UNLOCK_SRN(pcontextNS->pHDS);

        /* Lock SRN exclusively among other processes. */

        LOCK_SRN_AND_ENTER_CS(pcontextNS->pHDS, eDRM_DST_LOCKEXCLUSIVE | eDRM_DST_LOCKWAIT);

        /* open the next block and continue */

        if( oContextBS.fCurrBlockIsValid )
        {
            ChkDR( _HdsTraverseBlocksInPostOrder( oContextBS.pcontextNS,
                                       (_CommBlockHDR*)&oContextBS.blockheaderCurrent,
                                        pblockheaderNext,
                                       &fOK ) );
        }
        else
        {
            ChkDR( _HdsTraverseBlocksInPostOrder( oContextBS.pcontextNS,
                                        NULL,
                                        pblockheaderNext,
                                       &fOK ) );
            oContextBS.fCurrBlockIsValid = fOK;
        }

        if (fOK)
        {
            /* the child block becomes the current block */

            ChkDR(_HdsCopyBlockBuffer((_CommBlockHDR*)&oContextBS.blockheaderCurrent, pblockheaderNext));

            MEMCPY(&oContextBS.blockheader, &oContextBS.blockheaderCurrent, SIZEOF(_CommBlockHDR));
        }
        else
        {
            LEAVE_CS_AND_UNLOCK_SRN(oContextBS.pcontextNS->pHDS);

            /* no more blocks found */
            dr = DRM_E_NOMORE;
        }

        while (fOK && fFound)
        {
            ChkDR(_HdsInitSlotContext(oContextBS.pcontextNS,
                         (DRM_BYTE *) pcontextSlot,
                                      cbSlotContext));

            /* search current block for any matching UNIQUEs under the HASH */

            ChkDR(_HdsSearchSlotInBlock((_CommBlockHDR*)&oContextBS.blockheaderCurrent,
                                         NULL,
                                         pkeyUnique,
                                         eSearchSlotDontCare,
                                         pcontextSlot,
                                        &fFound));
            if (fFound)
            {
                if (_IsMatchingKey(&pcontextSlot->oSlotHeader.oHashkey,
                                    oContextBS.pkeyHash,
                                    oContextBS.cKeysHash,
                                    oContextBS.iKeyCurrent))
                {
                    /* mark the slot as hidden and record that we dirtied this block */

                    ChkDR(_HdsRemoveSlot(pcontextSlot,
                                         eRemoveSlotSetHidden));

                    fAnyDeletedFromBlock = TRUE;
                    oContextBS.iKeyCurrent++;
                    oContextBS.cKeysFound++;

                    /* if we've already found all the keys we can complete this loop and exit both loops */

                    if (oContextBS.cKeysFound == oContextBS.cKeysHash)
                    {
                        fFound = FALSE;
                        dr     = DRM_E_NOMORE;
                    }
                }
            }
        }

        /* if any hidden slots were found for this HASKEY then remove those slots */

        if (fAnyDeletedFromBlock)
        {
            ChkDR( _CoalesceBlock((_CommBlockHDR*)&oContextBS.blockheaderCurrent,
                                                    pcontextSlotLock,
                                                    pkeyUnique,
                                                    oContextBS.lockmode,
                                                   &fLocked) );
            if (fLocked)
            {
                ChkDR(_HdsUnlockBlock2DeleteSlot(pcontextSlotLock));
                fLocked = FALSE;
            }
        }
    } /* end block loop */

    if (dr == DRM_E_NOMORE)
    {
        dr = DRM_SUCCESS;
    }
ErrorExit:

    if (pblockheaderNext != NULL)
    {
        _Hds_free(oContextBS.pcontextNS->pHDS, pblockheaderNext);
    }

    if (pcontextSlotLock != NULL)
    {
        if (fLocked)
        {
            _HdsUnlockBlock2DeleteSlot(pcontextSlotLock);
        }

        _Hds_free(oContextBS.pcontextNS->pHDS, pcontextSlotLock);
    }

    if (pcontextSlot != NULL)
    {
        _Hds_free(oContextBS.pcontextNS->pHDS, pcontextSlot);
    }

    if (pcontextNS != NULL)
    {
        LEAVE_CS_AND_UNLOCK_SRN(pcontextNS->pHDS);  /* unlock SRN */
    }

    return dr;
}

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM, "Changing parameters to const to satisfy warning would make them not match the interface.")
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM, "Changing parameters to const to satisfy warning would make them not match the interface.")
DRM_API DRM_RESULT DRM_CALL DRM_HDS_SlotReadPtr(
    __inout                           DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __deref_out_bcount( *f_pcbData )  DRM_BYTE             **f_ppbData,
    __out                             DRM_DWORD             *f_pcbData )
{
    /* This method is not supported for disk-backed stores. */
    UNREFERENCED_PARAMETER( f_pContextSlot );
    UNREFERENCED_PARAMETER( f_ppbData      );
    UNREFERENCED_PARAMETER( f_pcbData      );
    return DRM_E_NOTIMPL;
}
PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the interface. */
PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the interface. */

EXIT_PK_NAMESPACE_CODE;

PRAGMA_STRICT_GS_POP;
