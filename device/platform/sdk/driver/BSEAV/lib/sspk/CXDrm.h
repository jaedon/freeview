///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.
//
//  This file is marked as "sample".
//
///////////////////////////////////////////////////////////////////////////////


#pragma once

#include "IXDrm.h"

#include "nexus_platform.h"
#include "nexus_wmdrmpd.h"
#include "common_crypto.h"

#define DMA_BLOCK_POOL  (32)

typedef IXDRM_HRESULT (*XDrmPolicyCallbackPtr)(
    const void *f_pvCallbackData,
    uint32_t    f_dwCallbackType,
    const void *f_pv );

class CXDrm : public IXDrm
{
private:
    CXDrm();
    ~CXDrm();

public:
    static IXDRM_HRESULT _CreateInstance( IXDrm **ppXDrm );
    static IXDRM_HRESULT _DestroyInstance( IXDrm *pXDrm );
        
// IXDrm implementation:

    IXDRM_HRESULT Init( XDrmOPLCallbackPtr pfnCallback, void* pvCallbackContext );

    IXDRM_HRESULT SetRights( unsigned long ulRights );

    IXDRM_HRESULT SetEnhancedData(
            size_t cbHdr,
            const uint8_t* pbHdr,
            size_t cbKeyID,
            const uint8_t* pbKeyID );

    IXDRM_HRESULT AcquireDecryptContext(
            size_t cbKeyID,
            const uint8_t* pbKeyID,
            void **ppvDecryptContext );

    IXDRM_HRESULT ReleaseDecryptContext(
            size_t cbKeyID,
            const uint8_t* pbKeyID );

    IXDRM_HRESULT CanDecrypt(
            void *pvDecryptContext,
            bool fAbortPlayback );

    IXDRM_HRESULT Decrypt(
            void *pvDecryptContext,
            uint8_t* pbData,
            size_t cbData,
            bool fIsAES,
            uint64_t qwSampleID,
            uint64_t qwOffset );

    IXDRM_HRESULT DecryptBufferChain(
            void *pvDecryptContext,
            void* pInBufList,
            void* pOutBufList,
            bool fIsAES,
            uint64_t qwSampleID,
            uint64_t qwOffset );

    IXDRM_HRESULT Commit();

    IXDRM_HRESULT GenerateChallenge(
            char** ppszUrl,
            const char* pszCustomData,
            char** ppszChallenge,
            bool fAllowCustomDataOverride );

    IXDRM_HRESULT ProcessResponse( const char* pszResponse );
    
    IXDRM_HRESULT SendHttp(
            const char* szContentType,
            const char* szRequest,
            char** pszResponse,
            const char* szURL );

    void Reset();
    void MemFree( void* ptr );

    XDRM_VERSION_CODE GetDRMVersionCode();

    IXDRM_HRESULT GetContentProperty(
            XDRM_CONTENT_PROPERTY eProperty,
            uint8_t* pbProperty,
            uint32_t* pcbProperty );

    IXDRM_HRESULT GetPropertyFromResponse(
            const uint8_t *pbResponse,
            size_t cbResponse,
            XDRM_RESPONSE_PROPERTY eProperty,
            char **ppszPropertyData );

    void BeginTransaction();
    void EndTransaction();

    bool SetDiagDelegate( IXDrmDiagDelegate* poIXDrmDiagDelegate );    

private: // methods

    IXDRM_HRESULT _Init();
    IXDRM_HRESULT _InitDRMIfRequired( bool *f_pfJustInitialized );
    void _RequireTransaction();

private: // state

    XDrmOPLCallbackPtr m_pfnXDrmPolicyCallback;
    void *m_pfnXDrmPolicyCallbackContext;
    IXDRM_HRESULT CXDrmPolicyCallback(NEXUS_WmDrmPdPolicyStatus  *pPolicyStatus);

    bool m_fInTransaction;
    bool m_fInit;

    NEXUS_WmDrmPdHandle            m_DrmHandle;
    NEXUS_KeySlotHandle            m_DrmKeyHandle;
    NEXUS_WmDrmPdLicenseChallenge  m_Challenge;
    NEXUS_WmDrmPdSettings          m_drmSettings;
    NEXUS_DmaJobBlockSettings      m_decryptionBlocks[DMA_BLOCK_POOL];
    NEXUS_DmaJobBlockSettings      m_copyBlocks[DMA_BLOCK_POOL];
    CommonCryptoHandle             m_cryptoHandle;
    CommonCryptoJobSettings        m_copyJobSettings;

    unsigned m_DrmPolicy; /*Nb of DRM policy to enforce */

    IXDrmDiagDelegate* m_poIXDrmDiagDelegate;
    static void _PolicyCallback(void *context, int param);
    //
    // Singleton reference tracking state:
    //
    static Lockable s_FactoryLock;

    static int s_nRefCount;
    static IXDrm *s_poXDrm;

    // Method operation lock
    static Lockable s_OperationLock;
    
};

