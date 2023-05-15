/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmdomainapi.h>
#include <drmcommon.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

DRM_NO_INLINE DRM_API DRM_BOOL DRM_CALL DRM_DOMAIN_IsDomainSupported(DRM_VOID)
{
    CLAW_AUTO_RANDOM_CIPHER
    return FALSE;
}

DRM_API DRM_BOOL DRM_CALL DRM_DOMAIN_IsDomainUnsupported(DRM_VOID)
{
    return TRUE;
}

DRM_API DRM_RESULT DRM_CALL Drm_JoinDomain_GenerateChallenge(
    IN                                         DRM_APP_CONTEXT *f_poAppContext,
    IN                                         DRM_DWORD        f_dwFlags,
    IN                                         DRM_DOMAIN_ID   *f_poDomainID,
    __in_ecount_opt( f_cchFriendlyName ) const DRM_CHAR        *f_pchFriendlyName,
    IN                                         DRM_DWORD        f_cchFriendlyName,
    __in_ecount_opt( f_cchData )         const DRM_CHAR        *f_pchData,
    IN                                         DRM_DWORD        f_cchData,
    __out_bcount_opt( *f_pcbChallenge )        DRM_BYTE        *f_pbChallenge,
    IN OUT                                     DRM_DWORD       *f_pcbChallenge )
{
    UNREFERENCED_PARAMETER( f_poAppContext    );
    UNREFERENCED_PARAMETER( f_dwFlags         );
    UNREFERENCED_PARAMETER( f_poDomainID      );
    UNREFERENCED_PARAMETER( f_pchFriendlyName );
    UNREFERENCED_PARAMETER( f_cchFriendlyName );
    UNREFERENCED_PARAMETER( f_pchData         );
    UNREFERENCED_PARAMETER( f_cchData         );
    UNREFERENCED_PARAMETER( f_pbChallenge     );
    UNREFERENCED_PARAMETER( f_pcbChallenge    );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL Drm_JoinDomain_ProcessResponse(
    IN DRM_APP_CONTEXT *f_poAppContext,
    IN DRM_PROCESS_DJ_RESPONSE_FLAG f_dwFlags,
    __in_bcount( f_cbResponse ) DRM_BYTE *f_pbResponse,
    IN DRM_DWORD f_cbResponse,
    OUT DRM_RESULT *f_pResult,
    OUT DRM_DOMAIN_ID *f_poDomainID )
{
    UNREFERENCED_PARAMETER( f_poAppContext );
    UNREFERENCED_PARAMETER( f_dwFlags      );
    UNREFERENCED_PARAMETER( f_pbResponse   );
    UNREFERENCED_PARAMETER( f_cbResponse   );
    UNREFERENCED_PARAMETER( f_pResult      );
    UNREFERENCED_PARAMETER( f_poDomainID   );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL Drm_LeaveDomain_GenerateChallenge(
    IN                                        DRM_APP_CONTEXT *f_poAppContext,
    IN                                        DRM_DWORD        f_dwFlags,
    IN                                        DRM_DOMAIN_ID   *f_poDomainID,
    __in_ecount_opt( f_cchData )        const DRM_CHAR        *f_pchData,
    IN                                        DRM_DWORD        f_cchData,
    __out_bcount_opt( *f_pcbChallenge )       DRM_BYTE        *f_pbChallenge,
    IN OUT                                    DRM_DWORD       *f_pcbChallenge )
{
    UNREFERENCED_PARAMETER( f_poAppContext );
    UNREFERENCED_PARAMETER( f_dwFlags      );
    UNREFERENCED_PARAMETER( f_poDomainID   );
    UNREFERENCED_PARAMETER( f_pchData      );
    UNREFERENCED_PARAMETER( f_cchData      );
    UNREFERENCED_PARAMETER( f_pbChallenge  );
    UNREFERENCED_PARAMETER( f_pcbChallenge );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL Drm_LeaveDomain_ProcessResponse(
    IN DRM_APP_CONTEXT *f_poAppContext,
    __in_bcount( f_cbResponse ) DRM_BYTE *f_pbResponse,
    IN DRM_DWORD f_cbResponse,
    OUT DRM_RESULT *f_pResult )
{
    UNREFERENCED_PARAMETER( f_poAppContext );
    UNREFERENCED_PARAMETER( f_pbResponse   );
    UNREFERENCED_PARAMETER( f_cbResponse   );
    UNREFERENCED_PARAMETER( f_pResult      );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL Drm_DomainCert_Find(
    IN DRM_APP_CONTEXT *f_poAppContext,
    IN DRM_DOMAIN_ID *f_poDomainID,
    __out_bcount_opt( *f_pcbDomainCert ) DRM_BYTE *f_pbDomainCert,
    IN OUT DRM_DWORD *f_pcbDomainCert )
{
    UNREFERENCED_PARAMETER( f_poAppContext  );
    UNREFERENCED_PARAMETER( f_poDomainID    );
    UNREFERENCED_PARAMETER( f_pbDomainCert  );
    UNREFERENCED_PARAMETER( f_pcbDomainCert );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL Drm_DomainCert_InitEnum(
    IN DRM_APP_CONTEXT *f_poAppContext,
    OUT DRM_DOMAIN_CERT_ENUM_CONTEXT *f_poDomainCertEnumContext )
{
    UNREFERENCED_PARAMETER( f_poAppContext            );
    UNREFERENCED_PARAMETER( f_poDomainCertEnumContext );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL Drm_DomainCert_EnumNext(
    IN DRM_DOMAIN_CERT_ENUM_CONTEXT *f_poDomainCertEnumContext,
    OUT DRM_DWORD *f_pcchDomainCert,
    OUT DRM_DOMAINCERT_INFO *f_poDomainCertInfo )
{
    UNREFERENCED_PARAMETER( f_poDomainCertEnumContext );
    UNREFERENCED_PARAMETER( f_pcchDomainCert          );
    UNREFERENCED_PARAMETER( f_poDomainCertInfo        );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DOMST_OpenStore(
    IN  DRM_DST                 *f_pDatastore,
    OUT DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext )
{
    UNREFERENCED_PARAMETER( f_pDatastore           );
    UNREFERENCED_PARAMETER( f_poDomainStoreContext );
    return DRM_SUCCESS; /* Nothing to open */
}

DRM_API DRM_RESULT DRM_CALL DRM_DOMST_CloseStore(
    IN DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext )
{
    UNREFERENCED_PARAMETER( f_poDomainStoreContext );
    return DRM_SUCCESS; /* Nothing to close */
}

DRM_API DRM_RESULT DRM_CALL DRM_DOMST_AddCert(
    __in_bcount( DRM_SHA1_DIGEST_LEN ) const DRM_BYTE                 f_rgbPasswordSST[__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __inout_opt                              DRM_VOID                *f_pOEMContext,
    IN                                       DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    IN                                       DRM_GUID                *f_poAccountID,
    IN                                       DRM_GUID                *f_poServiceID,
    __in_bcount( f_cbCert )            const DRM_BYTE                *f_pbCert,
    IN                                       DRM_DWORD                f_cbCert )
{
    UNREFERENCED_PARAMETER( f_rgbPasswordSST       );
    UNREFERENCED_PARAMETER( f_pOEMContext          );
    UNREFERENCED_PARAMETER( f_poDomainStoreContext );
    UNREFERENCED_PARAMETER( f_poAccountID          );
    UNREFERENCED_PARAMETER( f_poServiceID          );
    UNREFERENCED_PARAMETER( f_pbCert               );
    UNREFERENCED_PARAMETER( f_cbCert               );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DOMST_GetCert(
    IN DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    IN DRM_GUID *f_poAccountID,
    OUT DRM_GUID *f_poServiceID,
    __out_bcount_opt( *f_pcbCert ) DRM_BYTE *f_pbCert,
    IN OUT DRM_DWORD *f_pcbCert )
{
    UNREFERENCED_PARAMETER( f_poDomainStoreContext );
    UNREFERENCED_PARAMETER( f_poAccountID          );
    UNREFERENCED_PARAMETER( f_poServiceID          );
    UNREFERENCED_PARAMETER( f_pbCert               );
    UNREFERENCED_PARAMETER( f_pcbCert              );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DOMST_DeleteCert(
    IN DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    IN DRM_GUID *f_poAccountID )
{
    UNREFERENCED_PARAMETER( f_poDomainStoreContext );
    UNREFERENCED_PARAMETER( f_poAccountID          );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DOMST_InitEnumCert(
    IN DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    OUT DRM_DOMAINSTORE_ENUM_CONTEXT *f_poDomainStoreEnumContext )
{
    UNREFERENCED_PARAMETER( f_poDomainStoreContext     );
    UNREFERENCED_PARAMETER( f_poDomainStoreEnumContext );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DOMST_EnumNextCert(
    IN DRM_DOMAINSTORE_ENUM_CONTEXT *f_poDomainStoreEnumContext,
    OUT DRM_GUID *f_poAccountID,
    OUT DRM_GUID *f_poServiceID,
    OUT DRM_DST_SLOT_HINT *f_poSlotHint,
    OUT DRM_DWORD *f_pcbData )
{
    UNREFERENCED_PARAMETER( f_poDomainStoreEnumContext );
    UNREFERENCED_PARAMETER( f_poAccountID              );
    UNREFERENCED_PARAMETER( f_poServiceID              );
    UNREFERENCED_PARAMETER( f_poSlotHint               );
    UNREFERENCED_PARAMETER( f_pcbData                  );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DOMST_AddKey(
    IN                           DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    IN                           DRM_GUID                *f_poAccountID,
    IN                           DRM_DWORD                f_dwRevision,
    __in_bcount( f_cbKey ) const DRM_BYTE                *f_pbKey,
    IN                           DRM_DWORD                f_cbKey )
{
    UNREFERENCED_PARAMETER( f_poDomainStoreContext );
    UNREFERENCED_PARAMETER( f_poAccountID          );
    UNREFERENCED_PARAMETER( f_dwRevision           );
    UNREFERENCED_PARAMETER( f_pbKey                );
    UNREFERENCED_PARAMETER( f_cbKey                );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DOMST_GetKey(
    IN DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    IN DRM_GUID *f_poAccountID,
    IN DRM_DWORD f_dwRevision,
    __out_bcount_opt( *f_pcbKey ) DRM_BYTE *f_pbKey,
    IN OUT DRM_DWORD *f_pcbKey )
{
    UNREFERENCED_PARAMETER( f_poDomainStoreContext );
    UNREFERENCED_PARAMETER( f_poAccountID          );
    UNREFERENCED_PARAMETER( f_dwRevision           );
    UNREFERENCED_PARAMETER( f_pbKey                );
    UNREFERENCED_PARAMETER( f_pcbKey               );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DOMST_DeleteKey(
    IN DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    IN DRM_GUID *f_poAccountID,
    IN DRM_DWORD f_dwRevision )
{
    UNREFERENCED_PARAMETER( f_poDomainStoreContext );
    UNREFERENCED_PARAMETER( f_poAccountID          );
    UNREFERENCED_PARAMETER( f_dwRevision           );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DOMST_DeleteKeys(
    IN DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    IN DRM_GUID *f_poAccountID )
{
    UNREFERENCED_PARAMETER( f_poDomainStoreContext );
    UNREFERENCED_PARAMETER( f_poAccountID          );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DOMST_InitEnumKey(
    IN DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    OUT DRM_DOMAINSTORE_ENUM_CONTEXT *f_poDomainStoreEnumContext )
{
    UNREFERENCED_PARAMETER( f_poDomainStoreContext     );
    UNREFERENCED_PARAMETER( f_poDomainStoreEnumContext );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DOMST_EnumNextKey(
    IN DRM_DOMAINSTORE_ENUM_CONTEXT *f_poDomainStoreEnumContext,
    OUT DRM_GUID *f_poAccountID,
    OUT DRM_DWORD *f_pdwRevision,
    OUT DRM_DST_SLOT_HINT *f_poSlotHint,
    OUT DRM_DWORD *f_pcbData )
{
    UNREFERENCED_PARAMETER( f_poDomainStoreEnumContext );
    UNREFERENCED_PARAMETER( f_poAccountID              );
    UNREFERENCED_PARAMETER( f_pdwRevision              );
    UNREFERENCED_PARAMETER( f_poSlotHint               );
    UNREFERENCED_PARAMETER( f_pcbData                  );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DOMST_CreateDomainStorePassword(
    IN  const DRM_GUID          *f_poAccountID,
    IN  DRM_BB_CONTEXT          *f_poBBXContext,
    __out_bcount( DRM_SHA1_DIGEST_LEN ) DRM_BYTE f_rgbPasswordSST[__CB_DECL(DRM_SHA1_DIGEST_LEN)] )
{
    UNREFERENCED_PARAMETER( f_poAccountID    );
    UNREFERENCED_PARAMETER( f_poBBXContext   );
    UNREFERENCED_PARAMETER( f_rgbPasswordSST );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DOMKEYXMR_GetHeader(
   __in_bcount(f_cbData) const DRM_BYTE              *f_pbData,
   __in                        DRM_DWORD              f_cbData,
   __inout_ecount(1)           DRM_DWORD             *f_pbOffset,
   __inout_ecount(1)           DRM_DOMKEYXMR_HEADER  *f_poHeader )
{
    UNREFERENCED_PARAMETER( f_pbData   );
    UNREFERENCED_PARAMETER( f_cbData   );
    UNREFERENCED_PARAMETER( f_pbOffset );
    UNREFERENCED_PARAMETER( f_poHeader );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DOMKEYXMR_GetSessionKey(
   __in_bcount(f_cbData) DRM_BYTE  *f_pbData,
   __in                  DRM_DWORD  f_cbData,
   __inout_ecount(1)     DRM_DWORD *f_pbOffset,
   __inout_ecount(1)     DRM_DOMKEYXMR_SESSIONKEY  *f_poKeyObj)
{
    UNREFERENCED_PARAMETER( f_pbData   );
    UNREFERENCED_PARAMETER( f_cbData   );
    UNREFERENCED_PARAMETER( f_pbOffset );
    UNREFERENCED_PARAMETER( f_poKeyObj );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DOMKEYXMR_GetPrivKeyContainer(
   __in_bcount(f_cbData) const DRM_BYTE                        *f_pbData,
   __in                        DRM_DWORD                        f_cbData,
   __inout_ecount(1)           DRM_DWORD                       *f_pbOffset,
   __inout_ecount(1)           DRM_DOMKEYXMR_PRIVKEYCONTAINER  *f_poKeyContainer )
{
    UNREFERENCED_PARAMETER( f_pbData         );
    UNREFERENCED_PARAMETER( f_cbData         );
    UNREFERENCED_PARAMETER( f_pbOffset       );
    UNREFERENCED_PARAMETER( f_poKeyContainer );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_DOMKEYXMR_GetPrivkey(
   __in_bcount(f_cbData) DRM_BYTE  *f_pbData,
   __in                  DRM_DWORD  f_cbData,
   __inout_ecount(1)     DRM_DWORD *f_pbOffset,
   __inout_ecount(1)     DRM_DOMKEYXMR_PRIVKEY *f_poKeyObj)
{
    UNREFERENCED_PARAMETER( f_pbData   );
    UNREFERENCED_PARAMETER( f_cbData   );
    UNREFERENCED_PARAMETER( f_pbOffset );
    UNREFERENCED_PARAMETER( f_poKeyObj );
    return DRM_E_NOTIMPL;
}

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_DOM_IsLicenseValidForELS(
    __in    DRM_DOMAINSTORE_CONTEXT     *f_pDomainStore,
    __inout DRM_XMR_LICENSE             *f_pXMRLicense,
    __inout DRM_BOOL                    *f_pfValidForELS )
{
    UNREFERENCED_PARAMETER( f_pDomainStore  );
    UNREFERENCED_PARAMETER( f_pXMRLicense   );
    UNREFERENCED_PARAMETER( f_pfValidForELS );
    return DRM_E_NOTIMPL;
}

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_DOM_LA_CalcDomainCertsCharCount(
    __in  DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    __in  DRM_GUID                *f_poServiceID,
    __out DRM_DWORD               *f_pcchDomainCerts )
{
    UNREFERENCED_PARAMETER( f_poDomainStoreContext  );
    UNREFERENCED_PARAMETER( f_poServiceID           );
    UNREFERENCED_PARAMETER( f_pcchDomainCerts       );
    return DRM_SUCCESS; /* Leave *f_pcchDomainCerts alone so it stays zero */
}

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_DOM_LA_BuildDomainCertsXML(
    __in    DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    __in    DRM_GUID                *f_poServiceID,
    __inout _XMBContextA            *f_poXMLContext )
{
    UNREFERENCED_PARAMETER( f_poDomainStoreContext );
    UNREFERENCED_PARAMETER( f_poServiceID          );
    UNREFERENCED_PARAMETER( f_poXMLContext         );
    return DRM_E_NOTIMPL;
}

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_DOM_ARCLK_ClockRollbackDetected(
    IN  DRM_BB_CONTEXT          *f_poBBXContext,
    IN  DRM_UINT64               ui64GlobalSavedTime )
{
    UNREFERENCED_PARAMETER( f_poBBXContext      );
    UNREFERENCED_PARAMETER( ui64GlobalSavedTime );
    return DRM_SUCCESS; /* Nothing to do for domains when clock rollback detected */
}

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_DOM_ARCLK_LEVL_DisableOnRollback(
    __inout DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    __in    DRM_XMR_LICENSE     *f_pXMR,
    __in    DRM_UINT64           f_u64MachineDateTime,
    __out   DRM_BOOL            *f_pfDisabled )
{
    UNREFERENCED_PARAMETER( f_pContextLicEval    );
    UNREFERENCED_PARAMETER( f_pXMR               );
    UNREFERENCED_PARAMETER( f_u64MachineDateTime );
    UNREFERENCED_PARAMETER( f_pfDisabled         );
    return DRM_SUCCESS; /* Leave *f_pfDisabled alone so it stays false */
}

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_DOM_ARCLK_CreateDomainStorePassword(
    __in                          const DRM_GUID       *f_poAccountID,
    __in                                DRM_BB_CONTEXT *f_poBBXContext,
    __out_bcount( DRM_SHA1_DIGEST_LEN ) DRM_BYTE        f_rgbPasswordSST[__CB_DECL(DRM_SHA1_DIGEST_LEN)] )
{
    UNREFERENCED_PARAMETER( f_poAccountID    );
    UNREFERENCED_PARAMETER( f_poBBXContext   );
    UNREFERENCED_PARAMETER( f_rgbPasswordSST );
    return DRM_E_NOTIMPL;
}

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_DOM_ARCLK_GetDomainSavedDateTime(
    __in_bcount( DRM_SHA1_DIGEST_LEN ) const DRM_BYTE                 f_rgbPasswordSST[__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __inout_opt                              DRM_VOID                *f_pOEMContext,
    __in                                     DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    __in                               const DRM_GUID                *f_poAccountID,
    __out                                    DRM_UINT64              *f_puiDomainSavedDateTime )
{
    UNREFERENCED_PARAMETER( f_rgbPasswordSST         );
    UNREFERENCED_PARAMETER( f_pOEMContext            );
    UNREFERENCED_PARAMETER( f_poDomainStoreContext   );
    UNREFERENCED_PARAMETER( f_poAccountID            );
    UNREFERENCED_PARAMETER( f_puiDomainSavedDateTime );
    return DRM_E_NOTIMPL;
}

PREFAST_POP

EXIT_PK_NAMESPACE_CODE;

