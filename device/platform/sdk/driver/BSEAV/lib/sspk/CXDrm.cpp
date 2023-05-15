///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.
//
//  This file is marked as "sample".
//
///////////////////////////////////////////////////////////////////////////////

// Implementation of IXDrm using NEXUS WmdrmPd module

#include "pkPAL.h"
#include "pkExecutive.h"

#include "strsafe.h"

#include <new>
#include <map>
#include <string>

#include "IPTVDecoderHal.h"

#include "AutoLock.h"

#include <drmtypes.h>
#include <drmdebug.h>
#include <drmresults.h>
#include <drmerr.h>

#include "CXHttp.h"

#include "CXDrm.h"

#ifdef DEBUG
#define CXDRM_TRACE_ENABLE
#endif
// #define CXDRM_TRACE_VERBOSE


#if defined(CXDRM_TRACE_ENABLE)
#define CXDRM_LOG_MESSAGE( printf_exp ) ( Executive_DebugPrintf printf_exp )
#else
#define CXDRM_LOG_MESSAGE( printf_exp )   ( void ) 0
#endif

#if defined(CXDRM_TRACE_VERBOSE)
#define CXDRM_LOG_MESSAGE_VERBOSE( printf_exp ) ( Executive_DebugPrintf printf_exp )
#else
#define CXDRM_LOG_MESSAGE_VERBOSE( printf_exp )   ( void ) 0
#endif

static const uint32_t DRM_TRANSACTION_LOOP_DELAY = 200;

static const uint32_t MAXIMUM_LICACQ_RESPONSE_BODY_SIZE = 1000000;

// The following GUIDs are defined by the PlayReady compliance rule document.
static NEXUS_WmDrmPdGuid ANALOG_VIDEO_AGC_OUTPUT_ID              = {{0xC3, 0xFD, 0x11, 0xC6, 0xF8, 0xB7, 0x4D, 0x20, 0xB0, 0x08, 0x1D, 0xB1, 0x7D, 0x61, 0xF2, 0xDA}};
static NEXUS_WmDrmPdGuid ANALOG_VIDEO_EXPLICIT_OUTPUT_ID         = {{0x20, 0x98, 0xDE, 0x8D, 0x7D, 0xDD, 0x4B, 0xAB, 0x96, 0xC6, 0x32, 0xEB, 0xB6, 0xFA, 0xBE, 0xA3}};
static NEXUS_WmDrmPdGuid BEST_EFFORT_CGMSA_ID                    = {{0x22, 0x5C, 0xD3, 0x6F, 0xF1, 0x32, 0x49, 0xEF, 0xBA, 0x8C, 0xC9, 0x1E, 0xA2, 0x8E, 0x43, 0x69}};
static NEXUS_WmDrmPdGuid ANALOG_VIDEO_COMPONENT_OUTPUT_ID        = {{0x81, 0x1C, 0x51, 0x10, 0x46, 0xC8, 0x4C, 0x6E, 0x81, 0x63, 0xC0, 0x48, 0x2A, 0x15, 0xD4, 0x7E}};
static NEXUS_WmDrmPdGuid ANALOG_VIDEO_COMPUTER_MONITOR_OUTPUT_ID = {{0xD7, 0x83, 0xA1, 0x91, 0xE0, 0x83, 0x4B, 0xAF, 0xB2, 0xDA, 0xE6, 0x9F, 0x91, 0x0B, 0x37, 0x72}};
static NEXUS_WmDrmPdGuid DIGITIAL_AUDIO_SCMS_ID                  = {{0x6D, 0x5C, 0xFA, 0x59, 0xC2, 0x50, 0x44, 0x26, 0x93, 0x0E, 0xFA, 0xC7, 0x2C, 0x8F, 0xCF, 0xA6}};


// ================================================================
// Class statics
// ================================================================

// lock for method operation:
Lockable CXDrm::s_OperationLock;

// Singleton operation:
Lockable CXDrm::s_FactoryLock;
int CXDrm::s_nRefCount = 0;
IXDrm* CXDrm::s_poXDrm = NULL;

//
// Factory method that creates an IXDrm instance. XDrm is implemented
// as a singleton so if it has already been created before, a reference
// to the extsing instance is returned and the reference count is
// incremented.
//
//
IXDRM_HRESULT CXDrm::_CreateInstance( IXDrm **ppXDrm )
{
    AutoLock lock(&s_FactoryLock);

    if ( s_poXDrm == NULL )
    {
        s_poXDrm = NEW_NO_THROW CXDrm;
        if (NULL == s_poXDrm)
        {
            return (IXDRM_HRESULT) pkE_UNEXPECTED;
        }
    }

    s_nRefCount++;
    *ppXDrm = s_poXDrm;

    return (IXDRM_HRESULT) pkS_OK;
}

//
// Destroy an IXDrm instance. First the reference count is decremented
// and if is reaches 0, the singleton XDrm instance is destroyed,
// otherwise the singleton XDrm instance is left alone.
//
IXDRM_HRESULT CXDrm::_DestroyInstance( IXDrm *pXDrm )
{
    AutoLock lock(&s_FactoryLock);

    if( pXDrm == NULL || pXDrm != s_poXDrm )
    {
        return (IXDRM_HRESULT) pkE_UNEXPECTED;
    }

    s_nRefCount--;
    if ( s_nRefCount == 0 )
    {
        delete s_poXDrm;
        s_poXDrm = NULL;
    }

    return (IXDRM_HRESULT) pkS_OK;
}

// ================================================================
// Private methods
// ================================================================

// Constructor/destructor
CXDrm::CXDrm()
    : m_pfnXDrmPolicyCallback(NULL)
    , m_pfnXDrmPolicyCallbackContext(NULL)
    , m_fInTransaction(false)
    , m_fInit(false)
    , m_DrmHandle(0)
    , m_DrmKeyHandle(0)
    , m_poIXDrmDiagDelegate(NULL)
    , m_DrmPolicy(0)
    , m_cryptoHandle(0)
{
    CXDRM_LOG_MESSAGE( ("CXDrm::CXDrm object CREATED\n" ) );
}

CXDrm::~CXDrm()
{
    CXDRM_LOG_MESSAGE( ("CXDrm::~CXDrm \n" ) );

    if(m_DrmKeyHandle){
        NEXUS_Security_FreeKeySlot(m_DrmKeyHandle);
    }

    if(m_cryptoHandle) CommonCrypto_Close(m_cryptoHandle);

    NEXUS_WmDrmPd_Destroy(m_DrmHandle);

}

// Initialize a XDrm instance.
// This performs all initialization EXCEPT DRM porting kit Initialize
// which happens in _InitDRMIfRequired.
//
// Arguments:   none.
//
// Returns:
// DRM_SUCCESS if the function finishes successfully.
// Corresponding error code if error occurrs.
//
IXDRM_HRESULT CXDrm::_Init()
{
    IXDRM_HRESULT dr = pkS_OK;

    CommonCryptoSettings cryptoSettings;

    NEXUS_SecurityKeySlotSettings drmKeySettings;

    NEXUS_Security_GetDefaultKeySlotSettings(&drmKeySettings);
    drmKeySettings.keySlotEngine = NEXUS_SecurityEngine_eGeneric;

    m_DrmKeyHandle = NEXUS_Security_AllocateKeySlot(&drmKeySettings);
    if(m_DrmKeyHandle == NULL)
    {
        dr = pkE_FAIL;
    }

    CommonCrypto_GetDefaultSettings(&cryptoSettings);

    m_cryptoHandle = CommonCrypto_Open(&cryptoSettings);
    if(m_cryptoHandle == NULL)
    {
        dr = pkE_FAIL;
    }

    CommonCrypto_GetDefaultJobSettings(&m_copyJobSettings);
    
    return dr;
}


void  CXDrm::_PolicyCallback(void *context, int)
{
    CXDrm* drm = (CXDrm*)context;
    drm->m_DrmPolicy++;
}


//
// Initialize PlayReady DRM
// If DRM porting kit Initialize has not yet been called, this function
// calls it and Revocation Set Buffer.
// Otherwise, it does nothing.
// If DRM Initialize fails because Activation is required, it should
// synchronously performs Activation and then call Initialize again.
//
// Preconditions:
//    Must be called within the operation lock
//
// Arguments:
// [f_pfJustInitialized] On output:
//                       Set to true if DRM Initialize was called successfully
//                       regardless of whether Activation was performed or not.
//                       Set to false otherwise.
//
//
// Returns:
// DRM_SUCCESS if the function finishes successfully.
// Corresponding error code if error occurrs.
//
IXDRM_HRESULT CXDrm::_InitDRMIfRequired( bool *f_pfJustInitialized )
{
    IXDRM_HRESULT dr = pkS_OK;
    NEXUS_Error  rc;
    *f_pfJustInitialized = false;

    if( m_DrmHandle == 0)
    {
        NEXUS_WmDrmPd_GetDefaultSettings(&m_drmSettings);
        m_drmSettings.transportType = NEXUS_TransportType_eMp4Fragment;

        m_drmSettings.policyCallback.callback = _PolicyCallback;
        m_drmSettings.policyCallback.context = this;
        m_drmSettings.policyCallback.param = 0;

        m_DrmHandle = NEXUS_WmDrmPd_Create(&m_drmSettings);
        if(!m_DrmHandle){
            dr = pkE_FAIL;
            goto errorExit;
        }

        rc = NEXUS_WmDrmPd_ConfigureKeySlot(m_DrmHandle, m_DrmKeyHandle);
        if(rc != NEXUS_SUCCESS){
            dr = pkE_FAIL;
            goto errorExit;
        }

        *f_pfJustInitialized = true;
    }

errorExit:
    return dr;
}

// ================================================================
// IXDrm implementation methods
// ================================================================

//
// Initialize a XDrm instance. 
//
// Arguments:
// [pfnCallback]    Policy callback.
// [pfnCallbackArg] Parameter to be passed to the policy callback.
//
// Returns:
// DRM_SUCCESS if the function finishes successfully.
// Corresponding error code if error occurrs.
//
IXDRM_HRESULT CXDrm::Init( XDrmOPLCallbackPtr pfnCallback, void* pvCallbackContext )
{
    IXDRM_HRESULT dr = pkS_OK;

    AutoLock lock(&s_OperationLock);

    if (!m_fInit)
    {
        ChkDR( _Init() );
        m_fInit = true;
    }

    //
    // This function gets called a LOT with NULL callbacks,
    // but we only care about the first valid callback it's called with.
    // However, it is also repeatedly called with different callback args
    // as the key rotates and different CPlayReadyLicense objects are created.
    // Thus, the arg changes even as the callback remains the same.
    //
    if( pfnCallback != NULL )
    {
        if( m_pfnXDrmPolicyCallback != NULL )
        {
            //
            // Callback is immutable once set
            //
            pkASSERT( m_pfnXDrmPolicyCallback == pfnCallback );
        }
        else
        {
            m_pfnXDrmPolicyCallback = pfnCallback;
        }
        m_pfnXDrmPolicyCallbackContext = pvCallbackContext;
    }
    else
    {
        //
        // Argument should always be null if callback is null
        //
        pkASSERT( pvCallbackContext == NULL );
    }

ErrorExit:
    
    if (DRM_FAILED(dr))
    {
        CXDRM_LOG_MESSAGE( ("CXDrm::Init failed = 0x%x\n", dr ) );
    }    
    else
    {
        CXDRM_LOG_MESSAGE( ("CXDrm::Init succeeded\n") );
    }
    return dr;
}

IXDRM_HRESULT CXDrm::SetRights( unsigned long ulRights )
{
return pkS_OK; // ALWAYS SUCCEED
}


IXDRM_HRESULT CXDrm::SetEnhancedData(
        size_t cbHdr,
        const uint8_t* pbHdr,
        size_t cbKeyID,
        const uint8_t* pbKeyID )
{
    IXDRM_HRESULT dr = pkS_OK;
    bool fJustInitialized = false;
    NEXUS_WmDrmPdMp4PsshBoxInfo pssh = {{0x9A, 0x04, 0xF0, 0x79, 0x98, 0x40, 0x42, 0x86, 0xAB, 0x92, 0xE6, 0x5B, 0xE0, 0x88, 0x5F, 0x95}};
    
    _RequireTransaction();

    AutoLock lock(&s_OperationLock);

    dr = _InitDRMIfRequired( &fJustInitialized );
    ChkDR(dr);
    
    if( !fJustInitialized )
    {
        /* Reinitialize the DRM nable setting a new header. The only way to do this is to shut it down and restart it. */
        NEXUS_WmDrmPd_Destroy(m_DrmHandle);
        m_DrmHandle = NEXUS_WmDrmPd_Create(&m_drmSettings);
        if(m_DrmHandle == 0) {
            ChkDR(pkE_FAIL);
        }

        if(NEXUS_WmDrmPd_ConfigureKeySlot(m_DrmHandle, m_DrmKeyHandle) != NEXUS_SUCCESS){
            ChkDR(pkE_FAIL);
        }
    }

    if(NEXUS_WmDrmPd_SetPsshBox(m_DrmHandle, &pssh, pbHdr, cbHdr) != NEXUS_SUCCESS){
        ChkDR(pkE_FAIL);
    }
    

ErrorExit:
    return dr;
}

IXDRM_HRESULT CXDrm::AcquireDecryptContext(
        size_t cbKeyID,
        const uint8_t* pbKeyID,
        void **ppvDecryptContext )
{
    IXDRM_HRESULT dr = pkS_OK;

    /* Does nothing. Decryption context is hidden under NEXUS_WmdrmPd */
    return dr;
}

IXDRM_HRESULT CXDrm::ReleaseDecryptContext(
        size_t cbKeyID,
        const uint8_t* pbKeyID )
{
    IXDRM_HRESULT dr = pkS_OK;

    /* Does nothing. Decryption context is hidden under NEXUS_WmdrmPd */

    return dr;
}


//
// Decrypt a subsample buffer.
//
// Arguments:
// [pvDecryptContext]   Pointer to a decrypt context to use.
// [pbData]     Pointer to a buffer containing the subsample data.
// [cbData]     Size of the buffer mentioned above.
// [fIsAES]     Currently it must be true.
// [qwSampleID] The Sample ID associated with the subsample.
// [qwOffset]   Offset of the subsample within the sample it belongs to.
//
// Returns:
// DRM_SUCCESS if the function finishes successfully.
// Corresponding error code if error occurrs.
//
IXDRM_HRESULT CXDrm::Decrypt(
        void *pvDecryptContext,
        uint8_t* pbData,
        size_t cbData,
        bool fIsAES,
        uint64_t qwSampleID,
        uint64_t qwOffset ) 
{ 
    IXDRM_HRESULT dr = pkS_OK;
    NEXUS_WmdrmPdAesCounterInfo aesInfo;
    NEXUS_Error rc;

    CXDRM_LOG_MESSAGE_VERBOSE( ("CXDrm::Decrypt [%p] cbData: %u iv:%llu offset:%llu\n", 
        pvDecryptContext, cbData, qwSampleID, qwOffset ) );

    AutoLock lock(&s_OperationLock);
    
    pkASSERT( m_DrmHandle != 0);

    ChkArg( pvDecryptContext != NULL );

    ChkBOOL( fIsAES, DRM_E_FAIL );

    NEXUS_WmDrmPd_GetDefaultAesCounterInfo(&aesInfo);
    aesInfo.nonce = qwSampleID;

    if(qwOffset != 0){
        aesInfo.blockCounter = qwOffset / 16;
        aesInfo.byteOffset = qwOffset % 16;
    }

    NEXUS_DmaJob_GetDefaultBlockSettings(&m_decryptionBlocks[0]);

    m_decryptionBlocks[0].pSrcAddr  = pbData;
    m_decryptionBlocks[0].pDestAddr = (void *)pbData;
    m_decryptionBlocks[0].blockSize = cbData;
    m_decryptionBlocks[0].cached = true;

    rc = NEXUS_WmDrmPd_ProcessBlocksAesCounter(m_DrmHandle, (const NEXUS_WmdrmPdAesCounterInfo *)&aesInfo, (const NEXUS_DmaJobBlockSettings *)&m_decryptionBlocks, 1);
    if(rc != NEXUS_SUCCESS){
        dr = pkE_FAIL;
    }

ErrorExit:
    if (pkFAILED(dr))
    {
        CXDRM_LOG_MESSAGE( ("CXDrm::Decrypt FAILED [0x%X]\n", dr ));
    }
    return dr;
}

//
// Decrypt a chain of subsamples. The list entry is in the form of
// IPTV_HAL_BUFFER. The implementation only uses the input list to
// decrypt the data in place. 
//
// Arguments:
// [pvDecryptContext]   Pointer to a decrypt context to use.
// [pInBufList]     Input list of subsample buffers to be decrypted.
// [pOutBufList]    Output list of subsample buffers after being decrypted.
// [fIsAES]         Currently it must be true.
// [qwSampleID]     The Sample ID associated with the subsample.
// [qwOffset]       Offset of the subsample within the sample it belongs to.
//
// Returns:
// DRM_SUCCESS if the function finishes successfully.
// Corresponding error code if error occurrs.
//
IXDRM_HRESULT CXDrm::DecryptBufferChain(
        void *pvDecryptContext,
        void* pInBufList,
        void* pOutBufList,
        bool fIsAES,
        uint64_t qwSampleID,
        uint64_t qwOffset ) 
{ 
    IXDRM_HRESULT dr = pkS_OK;
    NEXUS_Error rc;
    uint64_t qwOffset1 = qwOffset;    
    uint32_t nBlocksToDecrypt = 0;
    NEXUS_WmdrmPdAesCounterInfo aesInfo;

    CXDRM_LOG_MESSAGE_VERBOSE( ("CXDrm::DecryptBufferChain [%p] in:%p out:%p iv:%llu offset:%llu\n", 
        pvDecryptContext, pInBufList, pOutBufList, qwSampleID, qwOffset ) );
    
    AutoLock lock(&s_OperationLock);
    
    pkASSERT( m_DrmHandle != 0);

    ChkArg( pInBufList != NULL && pOutBufList != NULL );
    
    ChkBOOL( fIsAES, DRM_E_FAIL );


    NEXUS_WmDrmPd_GetDefaultAesCounterInfo(&aesInfo);
    aesInfo.nonce = qwSampleID;

    if(qwOffset1 != 0){
        aesInfo.blockCounter = qwOffset1 / 16;
        aesInfo.byteOffset = qwOffset1 % 16;
    }

    // loop through the input buffer chain
    {
        PIPTV_HAL_BUFFER pBufListItem = (PIPTV_HAL_BUFFER) pInBufList;

        while (NULL != pBufListItem)
        {
            if ( ( ( pBufListItem->u32Flags & IPTV_HAL_BUFFER_FLAG_DECRYPT ) != 0 ) )
            {
                uint32_t cbSize;

                NEXUS_DmaJob_GetDefaultBlockSettings(&m_decryptionBlocks[nBlocksToDecrypt]);

                cbSize = pBufListItem->u32DataEnd - pBufListItem->u32DataStart;

                m_decryptionBlocks[nBlocksToDecrypt].pSrcAddr  = pBufListItem->pBuf + pBufListItem->u32DataStart;
                m_decryptionBlocks[nBlocksToDecrypt].pDestAddr = (void *)(pBufListItem->pBuf + pBufListItem->u32DataStart);
                m_decryptionBlocks[nBlocksToDecrypt].blockSize = cbSize;
                m_decryptionBlocks[nBlocksToDecrypt].cached = true;
                
                nBlocksToDecrypt++;
                if(nBlocksToDecrypt >= DMA_BLOCK_POOL){
                    ChkDR(pkE_NO_MORE_ITEMS);
                }

                qwOffset1 += cbSize;
            }
        
            pBufListItem = pBufListItem->pNext;
        }

         rc = NEXUS_WmDrmPd_ProcessBlocksAesCounter(m_DrmHandle, (const NEXUS_WmdrmPdAesCounterInfo *)&aesInfo, (const NEXUS_DmaJobBlockSettings *)&m_decryptionBlocks, nBlocksToDecrypt);
        if(rc != NEXUS_SUCCESS){
            dr = pkE_FAIL;
        }

    }

ErrorExit:
    if (pkFAILED(dr))
    {
        CXDRM_LOG_MESSAGE( ("CXDrm::DecryptBufferChain [%p] FAILED [0x%X]\n", 
            pvDecryptContext, dr )); 
    }
    return dr;
}

#if 0
//
// Decrypt a chain of subsamples. The list entry is in the form of
// IPTV_HAL_BUFFER. This implementation decrypt the data in the ouput buffer
// list. However we can't use it. To use it, we need to modify 
// CPlayReadyDecrypter::DecryptBufferChain(). Since they do this
// _pIXDrm->DecryptBufferChain(pvDecryptContext, pIn, pIn, true, _sampleID, _sampleOffset);
// ... 
// return IPTV_HAL_Crypto_DecryptAVPayload(pIn, pOut, IPTV_HAL_CRYPTO_KEY_REGISTER_PASSTHRU, 0, 0, NULL, 0);
// instead of 
// _pIXDrm->DecryptBufferChain(pvDecryptContext, pIn, pOut, true, _sampleID, _sampleOffset);
// return IPTV_HAL_ERROR_SUCCESS;
// But, per our license agreement with Microsoft. We are not allowed to modify
// any of the Component source files. As a result, we are unable to support their
// secure data path requierement.
//
// Arguments:
// [pvDecryptContext]   Pointer to a decrypt context to use.
// [pInBufList]     Input list of subsample buffers to be decrypted.
// [pOutBufList]    Output list of subsample buffers after being decrypted.
// [fIsAES]         Currently it must be true.
// [qwSampleID]     The Sample ID associated with the subsample.
// [qwOffset]       Offset of the subsample within the sample it belongs to.
//
// Returns:
// DRM_SUCCESS if the function finishes successfully.
// Corresponding error code if error occurrs.
//
IXDRM_HRESULT CXDrm::DecryptBufferChain(
        void *pvDecryptContext,
        void* pInBufList,
        void* pOutBufList,
        bool fIsAES,
        uint64_t qwSampleID,
        uint64_t qwOffset ) 
{ 
    IXDRM_HRESULT dr = pkS_OK;
    NEXUS_Error rc;
    uint64_t qwOffset1 = qwOffset;    
    uint32_t nBlocksToDecrypt = 0;
    uint32_t nBlocksToCopy = 0;
    uint8_t *p;
    NEXUS_WmdrmPdAesCounterInfo aesInfo;
    UINT32 cbCapacity;
    UINT32 cbCopy;
    UINT32 cbTakenFromInBuffer = 0;
    PIPTV_HAL_BUFFER pInBuffer;
    AutoLock lock(&s_OperationLock);

    pkASSERT( m_DrmHandle != 0);

    int iteration = 0;

    ChkArg( pInBufList != NULL && pOutBufList != NULL );
    
    ChkBOOL( fIsAES, DRM_E_FAIL );

    // loop through the input buffer chain
    {

        NEXUS_WmDrmPd_GetDefaultAesCounterInfo(&aesInfo);
        aesInfo.nonce = qwSampleID;

        if(qwOffset1 != 0){
            aesInfo.blockCounter = qwOffset1 / 16;
            aesInfo.byteOffset = qwOffset1 % 16;
        }

        pInBuffer = (PIPTV_HAL_BUFFER) pInBufList;

        // For each buffer in the output chain
        for( PIPTV_HAL_BUFFER pOutBuffer = (PIPTV_HAL_BUFFER)pOutBufList; pOutBuffer != NULL; pOutBuffer = pOutBuffer->pNext )
        {
            // Initialize the output buffer

            pOutBuffer->u32DataStart = 0;
            pOutBuffer->u32DataEnd = 0;

            // While there's room in the output buffer and data
            // in the input chain, copy as much data as possible

            while( ( pOutBuffer->u32DataEnd < pOutBuffer->u32Size ) && ( pInBuffer != NULL ) )
            {

                if( pInBuffer->u32DataEnd < pInBuffer->u32DataStart )
                {
                    dr = IPTV_HAL_ERROR_INVALID_PARAMETER;
                    break;
                }

                cbCapacity = pOutBuffer->u32Size - pOutBuffer->u32DataEnd;
                cbCopy = pInBuffer->u32DataEnd - pInBuffer->u32DataStart - cbTakenFromInBuffer;

                cbCopy = ( cbCapacity < cbCopy ) ? cbCapacity : cbCopy;

                
                if ( ( ( pInBuffer->u32Flags & IPTV_HAL_BUFFER_FLAG_DECRYPT ) != 0 ) )
                {
                    NEXUS_DmaJob_GetDefaultBlockSettings(&m_decryptionBlocks[nBlocksToDecrypt]);

                    m_decryptionBlocks[nBlocksToDecrypt].pSrcAddr  = &pInBuffer->pBuf[ pInBuffer->u32DataStart + cbTakenFromInBuffer ];
                    m_decryptionBlocks[nBlocksToDecrypt].pDestAddr = (void *)(&pOutBuffer->pBuf[ pOutBuffer->u32DataEnd ]);
                    m_decryptionBlocks[nBlocksToDecrypt].blockSize = cbCopy;
                    m_decryptionBlocks[nBlocksToDecrypt].cached = true;
                
                    nBlocksToDecrypt++;
                    if(nBlocksToDecrypt >= DMA_BLOCK_POOL){
                        ChkDR(pkE_NO_MORE_ITEMS);
                    }
                }
               else {

                    /* Data not encrypted copy it directly into the destination buffer */
                    /* Use the DMA engine to do the copy the unencrypted data in the secure region. */
                    NEXUS_DmaJob_GetDefaultBlockSettings(&m_copyBlocks[nBlocksToCopy]);
                    m_copyBlocks[nBlocksToCopy].pSrcAddr  = &pInBuffer->pBuf[ pInBuffer->u32DataStart + cbTakenFromInBuffer ];
                    m_copyBlocks[nBlocksToCopy].pDestAddr = (void *)(&pOutBuffer->pBuf[ pOutBuffer->u32DataEnd ]);
                    m_copyBlocks[nBlocksToCopy].blockSize = cbCopy;
                    m_copyBlocks[nBlocksToCopy].cached = true;
                
                    nBlocksToCopy++;
                    if(nBlocksToCopy >= DMA_BLOCK_POOL){
                        ChkDR(pkE_NO_MORE_ITEMS);
                    }
                }
                pOutBuffer->u32DataEnd += cbCopy;

                cbTakenFromInBuffer += cbCopy;
                if( cbTakenFromInBuffer >= pInBuffer->u32DataEnd - pInBuffer->u32DataStart )
                {
                    // Current input buffer fully copied.
                    // Move to the next.
                    cbTakenFromInBuffer = 0;
                    pInBuffer = pInBuffer->pNext;
                }
            }
        }

        if( ( dr == IPTV_HAL_ERROR_SUCCESS ) && ( pInBuffer != NULL ) )
        {
            // Unexpected failure. There's data left in the input chain.
            // The output chain was not large enough.
            dr = pkE_INSUFFICIENT_BUFFER;
        }
        else {
            if(nBlocksToCopy > 0){
                CommonCrypto_DmaXfer(m_cryptoHandle,  &m_copyJobSettings, (const NEXUS_DmaJobBlockSettings *)&m_copyBlocks, nBlocksToCopy);
            }

            rc = NEXUS_WmDrmPd_ProcessBlocksAesCounter(m_DrmHandle, (const NEXUS_WmdrmPdAesCounterInfo *)&aesInfo, (const NEXUS_DmaJobBlockSettings *)&m_decryptionBlocks, nBlocksToDecrypt);
            if(rc != NEXUS_SUCCESS){
                dr = pkE_FAIL;
            }
        }
    }

ErrorExit:
    if (pkFAILED(dr))
    {
        CXDRM_LOG_MESSAGE( ("CXDrm::DecryptBufferChain FAILED [0x%X]\n", dr ));
    }
    return dr;
}
#endif

IXDRM_HRESULT CXDrm::CXDrmPolicyCallback(
    NEXUS_WmDrmPdPolicyStatus  *pPolicyStatus)
{
    IXDRM_HRESULT dr = pkS_OK;

    XDRM_OPL_DATA *pOPLData = NULL;
    uint32_t wUncompressedVideoOPL;

    switch(pPolicyStatus->policyType){
        case NEXUS_WmDrmPdPolicyType_ePlay:

            ChkMem( pOPLData = ( XDRM_OPL_DATA * )BKNI_Malloc( sizeof( XDRM_OPL_DATA ) ) );
            memset( pOPLData, 0, sizeof( XDRM_OPL_DATA ) );

            wUncompressedVideoOPL = pPolicyStatus->policy.play.uncompressedDigitalVideo;
            if ( wUncompressedVideoOPL <= 250 )
            {
                // No restriction.
            }
            else if ( wUncompressedVideoOPL <= 270 )
            {
                // Allow output to external display but the content has to down-res.
                pOPLData->HDCPAction = XDRM_OPL_ENABLE_DOWN_RES;
            }
            else
            {
                // Does not allow output to external display.
                pOPLData->HDCPAction = XDRM_OPL_ENABLE_ALWAYS;
            }
        
            // Check for explicit analog video/audio protections.
            // There are two categories of analog video/audio protections: One does not allow output if there
            // is an external display and the other down-res when there is an external display.
            // Since the behavior is the same as that of HDCP OPLs we are reusing HDCP bits to achieve the same goal. 
            for ( unsigned i = 0; i < pPolicyStatus->policy.play.numVideoOutputEntries; i++ )
            {
                
                if ( BKNI_Memcmp( &pPolicyStatus->policy.play.videoOutputEntries[i].guid, &ANALOG_VIDEO_COMPONENT_OUTPUT_ID, sizeof( NEXUS_WmDrmPdGuid ) ) == 0 ||
                     BKNI_Memcmp( &pPolicyStatus->policy.play.videoOutputEntries[i].guid, &ANALOG_VIDEO_COMPUTER_MONITOR_OUTPUT_ID, sizeof( NEXUS_WmDrmPdGuid ) ) == 0 )
                {
                    // Allow output to external display but the content has to down-res.
                    pOPLData->HDCPAction = XDRM_OPL_ENABLE_DOWN_RES;
                }
                else if ( BKNI_Memcmp( &pPolicyStatus->policy.play.videoOutputEntries[i].guid, &ANALOG_VIDEO_AGC_OUTPUT_ID, sizeof( NEXUS_WmDrmPdGuid ) ) == 0 ||
                          BKNI_Memcmp( &pPolicyStatus->policy.play.videoOutputEntries[i].guid, &ANALOG_VIDEO_EXPLICIT_OUTPUT_ID, sizeof( NEXUS_WmDrmPdGuid ) ) == 0 )
                {
                    // Does not allow output to external display.
                    pOPLData->HDCPAction = XDRM_OPL_ENABLE_ALWAYS;
                }
                else if ( BKNI_Memcmp( &pPolicyStatus->policy.play.videoOutputEntries[i].guid, &BEST_EFFORT_CGMSA_ID, sizeof( NEXUS_WmDrmPdGuid ) ) == 0 )
                {
                    // Do nothing for "Best effort".
                }
            }

            ChkDR(m_pfnXDrmPolicyCallback( pOPLData, m_pfnXDrmPolicyCallbackContext ) );

            break;
        case NEXUS_WmDrmPdPolicyType_eInclusionList:
        default:
            ChkDR( DRM_E_NOTIMPL );
            break;
    }

ErrorExit:
    if(pOPLData) BKNI_Free(pOPLData);
    return dr;
}

//
// Perform the DRM bind function to locate a license of a
// piece of content with a specific rights.
//
// Arguments:
// [pvDecryptContext]   Pointer to a decrypt context to use.
// [fAbortPlayback]     Flag indicating whether playback should be aborted if error occurrs.
//
// Returns:
// DRM_SUCCESS if the function finishes successfully.
// Corresponding error code if error occurrs.
//
IXDRM_HRESULT CXDrm::CanDecrypt( void *pvDecryptContext, bool fAbortPlayback )
{
    IXDRM_HRESULT dr = pkS_OK;
    NEXUS_Error rc;
    _RequireTransaction();

    AutoLock lock(&s_OperationLock);

    pkASSERT( m_DrmHandle != 0);

    if(NEXUS_WmDrmPd_LoadLicense(m_DrmHandle) != NEXUS_SUCCESS){
        dr = pkE_FAIL;
    }
    else {
        if(m_DrmPolicy > 0) {
            while(m_DrmPolicy > 0)
            {
                NEXUS_WmDrmPdPolicyStatus  policyStatus;

                if(NEXUS_WmDrmPd_GetPolicyStatus(m_DrmHandle, &policyStatus) == NEXUS_SUCCESS)
                {
                    CXDrmPolicyCallback(&policyStatus);
                } else {
                    ChkDR(pkE_FAIL);
                }
                m_DrmPolicy--;
            }
        }
    }

    CXDRM_LOG_MESSAGE( ("CXDrm::Commit\n" ) );
ErrorExit:
    return dr;
}

IXDRM_HRESULT CXDrm::GenerateChallenge(
        char** ppszUrl,
        const char* pszCustomData,
        char** ppszChallenge,
        bool fAllowCustomDataOverride )
{
    IXDRM_HRESULT dr = pkS_OK;
    NEXUS_Error  rc;
    DRM_CHAR *pszCustomDataUsed = NULL;
    DRM_DWORD cchCustomDataLength = 0;

    _RequireTransaction();

    AutoLock lock(&s_OperationLock);

    pkASSERT( m_DrmHandle != 0);

    if((pszCustomData != NULL)&& (0 != pszCustomData[0])){
        cchCustomDataLength = ( DRM_DWORD )strlen(pszCustomData);
        pszCustomDataUsed =  ( DRM_CHAR * )pszCustomData;
        rc = NEXUS_WmDrmPd_GetCustomLicenseChallenge(m_DrmHandle, pszCustomData, cchCustomDataLength, &m_Challenge);
    }
    else {
        rc = NEXUS_WmDrmPd_GetLicenseChallenge(m_DrmHandle,  &m_Challenge);
    }

    if(rc == NEXUS_SUCCESS){

        /*CPlayreadyLicense assumes that memory is allocated for the challenge and frees it later on.
            But in our case, Nexus manage the buffer internally since nexus talk to the Playready SDK, not CXDrm.
            Per our license agreement, we are not allowed to modify the CPlayreadyLicense class. As a result, 
            allocate buffers here.
        */
        *ppszChallenge = (char *)BKNI_Malloc( m_Challenge.dataLength + 1);
        BKNI_Memcpy(*ppszChallenge, m_Challenge.pData, m_Challenge.dataLength + 1);

        *ppszUrl = (char *)BKNI_Malloc( strlen((char*)m_Challenge.pUrl) + 1);
        BKNI_Memcpy(*ppszUrl, m_Challenge.pUrl, strlen((char*)m_Challenge.pUrl) + 1);

        NEXUS_WmDrmPd_CleanupLicenseStore(m_DrmHandle);
    }

    return dr;
}


//
// Commit the bind operation.
//
// Arguments:   none.
//
// Returns:
// DRM_SUCCESS if the function finishes successfully.
// Corresponding error code if error occurrs.
//
IXDRM_HRESULT CXDrm::Commit() 
{ 

    IXDRM_HRESULT dr = pkS_OK;
    /* Nothing to do. Everything has been taken care of in NEXUS_WmDrmPd_LoadLicense() */
    return dr;

}

IXDRM_HRESULT CXDrm::ProcessResponse( const char* pszResponse )
{
    IXDRM_HRESULT dr = pkS_OK;
    unsigned responseOffset = 0;
    size_t responseLength = strlen(pszResponse);

    AutoLock lock(&s_OperationLock);
    
    BKNI_Memcpy(m_Challenge.pResponseBuffer, pszResponse, responseLength);

    if (NEXUS_WmDrmPd_LicenseChallengeComplete(m_DrmHandle, responseLength, responseOffset) != NEXUS_SUCCESS) {
        dr = pkE_FAIL;
    }

    CXDRM_LOG_MESSAGE( ("CXDrm::ProcessResponse\n" ) );
    
    return dr;
}

IXDRM_HRESULT CXDrm::SendHttp(
        const char* szContentType,
        const char* szRequest,
        char** pszResponse,
        const char* szURL)
{
    IXDRM_HRESULT dr = pkS_OK;

    CXHttp* poHttp = NULL;

    std::string httpBody = szRequest;
    std::string httpServer;
    std::string resource = szURL;
    std::string httpRequest;

    CXDRM_LOG_MESSAGE( ("CXDrm::SendHttp URL: %s\n", szURL ) );

    // NOTE: does NOT _RequireTransaction();

    ChkBOOL( strcmp( szContentType, PLAYREADY_GETLICENSE_CONTENT_TYPE ) == 0, pkE_FAIL );

    // Parse out the httpServer and resource strings
    {
        size_t ixProto = resource.find("http://");

        ChkBOOL(0 == ixProto, pkE_INVALIDARG);
        resource = resource.substr(ixProto+7, resource.size() - (ixProto+7));

        size_t ixUri = resource.find_first_of('/');

        ChkBOOL(std::string::npos != ixUri, pkE_INVALIDARG);
        httpServer = resource.substr(0, ixUri);
        resource = resource.substr(ixUri, resource.size()-(ixUri));
    }

    // Create the request header string
    {
        const int uintStrSize = 12; // max string size of %u
        char contentSizeStr[uintStrSize];

        StringCbPrintfA(contentSizeStr, uintStrSize, "%u", (unsigned int) httpBody.size());

        httpRequest =  "POST " + resource + " HTTP/1.1\r\n";
        httpRequest +=
            "Accept: */*\r\n"
            "Accept-Language: en-US\r\n"
            "Content-Length: "
            ;
        httpRequest += contentSizeStr;
        httpRequest +=
            "\r\n"
            "User-Agent: Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)\r\n"
            "Pragma: no-cache\r\n"
            ;
        httpRequest += "Host: " + httpServer + "\r\n";
        if (NULL != szContentType)
        {
            httpRequest += szContentType;
        }
        else
        {
            httpRequest += "Content-Type: text/xml; charset=utf-8\r\n";
        }

        // finally, a second cr/lf in a row signals end of headers
        httpRequest += "\r\n";
    }

    // Perform the HTTP transaction
    {
        char *pchResponseBody = NULL;

        poHttp = NEW_NO_THROW CXHttp;
        ChkMem( poHttp );

        std::string responseHeader;

        bool isResponseOk = poHttp->HttpRequestResponse(httpServer, httpRequest, httpBody, responseHeader);

        if (isResponseOk)
        {
            CXDRM_LOG_MESSAGE_VERBOSE( ("CXDrm::SendHttp challenge:\n%s\n===RESPONSE HEADER===\n%s\n",
                httpBody.c_str(), responseHeader.c_str()) );
        }
        else
        {
            CXDRM_LOG_MESSAGE( ("CXDrm::SendHttp challenge FAILED with response header\n%s\n",
                responseHeader.c_str()) );
        }

        ChkBOOL( isResponseOk, pkE_FAIL );

        uint32_t cbContent = poHttp->GetContentLength();

        ChkBOOL( cbContent < MAXIMUM_LICACQ_RESPONSE_BODY_SIZE, pkE_INSUFFICIENT_BUFFER);

        ChkMem( pchResponseBody = (char*)BKNI_Malloc( cbContent + 1 ) );

        // Recv response body into buffer

        uint32_t ixContent = 0;
        while (ixContent < cbContent)
        {
            int rc = poHttp->Recv((byte*)&pchResponseBody[ixContent], cbContent - ixContent);
            if (rc <= 0)
            {
                CXDRM_LOG_MESSAGE( ("Receiving response body failed rc: %d len: %d received: %d contentLen: %d\n",
                    rc,
                    cbContent - ixContent,
                    ixContent,
                    poHttp->GetContentLength()));
                MemFree(pchResponseBody);
                dr = pkE_FAIL;
                goto ErrorExit;
            }
            ixContent += rc;
        }
        pkASSERT(ixContent == cbContent);

        pchResponseBody[cbContent] = 0; // null terminate the response body

        CXDRM_LOG_MESSAGE_VERBOSE( ("CXDrm::SendHttp response (%u):\n%s\n", cbContent, pchResponseBody ) );

        *pszResponse = pchResponseBody;
        // Note: *pszResponse must be freed by caller using IXDrm::MemFree
    }

ErrorExit:
    delete poHttp;

    if(!pkSUCCEEDED(dr))
    {
        CXDRM_LOG_MESSAGE( ("CXDrm::SendHttp failed: 0x%X\n", dr ) );
    }
    return dr;
}

void CXDrm::Reset()
{
    /* do nothing */
}

void CXDrm::MemFree( void* ptr )
{
    if (ptr)
    {
        BKNI_Free(ptr);
    }
}

XDRM_VERSION_CODE CXDrm::GetDRMVersionCode()
{
    return XDRM_VERSIONCODE_PLAYREADY;
}

IXDRM_HRESULT CXDrm::GetContentProperty(
        XDRM_CONTENT_PROPERTY eProperty,
        uint8_t* pbProperty,
        uint32_t* pcbProperty )
{
    IXDRM_HRESULT dr = pkE_FAIL;

    _RequireTransaction();

    AutoLock lock(&s_OperationLock);

    pkASSERT( m_DrmHandle != 0);


    switch ( eProperty )
    {
        case XDRM_PROP_DECRYPTORSETUP:
            /* Playready 2.0 feature, not currently supported */
            *pbProperty = 0;
            dr = DRM_SUCCESS;
            break;

        default:
            dr = pkE_FAIL;
    }

    return( dr );
}


IXDRM_HRESULT CXDrm::GetPropertyFromResponse(
            const uint8_t *pbResponse,
            size_t cbResponse,
            XDRM_RESPONSE_PROPERTY eProperty,
            char **ppszPropertyData )
{
    IXDRM_HRESULT dr = pkS_OK;

    // NOTE: does NOT _RequireTransaction();

    AutoLock lock(&s_OperationLock);

    pkASSERT( m_DrmHandle != 0 );

    if( NULL == pbResponse || 0 == cbResponse || NULL == ppszPropertyData )
    {
        CXDRM_LOG_MESSAGE( ("CXDrm::GetCustomDataFromResponse: invalid params: %p, %u, %p\n",
            pbResponse, cbResponse, ppszPropertyData ) );

        dr = pkE_INVALIDARG;
        goto errExit;
    }

    *ppszPropertyData = NULL;

    switch ( eProperty )
    {
        case XDRM_RESPONSE_CUSTOM_DATA:
            // map type to DRM porting kit type
            break;

        case XDRM_RESPONSE_REDIRECT_URL:
            // map type to DRM porting kit type
            break;

        default:
            dr = pkE_INVALIDARG;
            goto errExit;
    }

    // Note: should call DRM porting kit to get additional response data based on type

    CXDRM_LOG_MESSAGE( ("CXDrm::GetCustomDataFromResponse\n" ) );

errExit:
    return dr;
}


void CXDrm::BeginTransaction()
{
    //
    // Wait for any existing transaction to complete before starting another one
    //
    s_OperationLock.Lock();

    while( m_fInTransaction )
    {
        s_OperationLock.Unlock();
        Executive_Sleep( DRM_TRANSACTION_LOOP_DELAY );
        s_OperationLock.Lock();
    }
    m_fInTransaction = true;

    s_OperationLock.Unlock();
}

void CXDrm::EndTransaction()
{
    s_OperationLock.Lock();

    if( !m_fInTransaction )
    {
        pkASSERT( false );
    }
    m_fInTransaction = false;

    s_OperationLock.Unlock();
}

void CXDrm::_RequireTransaction()
{
    bool fInTransaction = false;
    s_OperationLock.Lock();
    fInTransaction = m_fInTransaction;
    s_OperationLock.Unlock();
    if( !fInTransaction )
    {
        pkASSERT( false );
    }
}

bool CXDrm::SetDiagDelegate( IXDrmDiagDelegate* poIXDrmDiagDelegate )
{
    s_OperationLock.Lock();
    m_poIXDrmDiagDelegate = poIXDrmDiagDelegate;
    s_OperationLock.Unlock();

    return true;
}


//
// ========================================================================
// IXDrm factory API implementation
// ========================================================================

IXDRM_HRESULT WINAPI XDRM_CreateInstance( IXDrm **ppXDrm )
{
    return CXDrm::_CreateInstance(ppXDrm);
}

//
// Destroy an IXDrm instance. First the reference count is decremented
// and if is reaches 0, the singleton XDrm instance is destroyed,
// otherwise the singleton XDrm instance is left alone.
//
IXDRM_HRESULT WINAPI XDRM_DestroyInstance( IXDrm *pXDrm )
{
    return CXDrm::_DestroyInstance(pXDrm);
}

