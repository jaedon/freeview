/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmmoveformat.h>
#include <drmmoveimp.h>
#include <drmmovestore.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

DRM_API DRM_RESULT DRM_CALL DRM_MOV_Page_Start(
    __inout_bcount( f_cbStack ) DRM_BYTE                *f_pbStack,
    __in                        DRM_DWORD                f_cbStack,
    __inout_ecount( 1 )         DRM_XB_BUILDER_CONTEXT  *f_pcontextBuilder )
{
    UNREFERENCED_PARAMETER( f_pbStack         );
    UNREFERENCED_PARAMETER( f_cbStack         );
    UNREFERENCED_PARAMETER( f_pcontextBuilder );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MOV_Page_Finish(
    __inout_ecount( 1 )            const DRM_XB_BUILDER_CONTEXT  *f_pcontextBuilder,
    __inout_bcount(*f_pcbMovePage)       DRM_BYTE                *f_pbMovePage,
    __inout                              DRM_DWORD               *f_pcbMovePage )
{
    UNREFERENCED_PARAMETER( f_pcontextBuilder );
    UNREFERENCED_PARAMETER( f_pbMovePage      );
    UNREFERENCED_PARAMETER( f_pcbMovePage     );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MP_ParsePage(
    __inout_bcount( f_cbStack )       DRM_BYTE                *f_pbStack,
    __in                              DRM_DWORD                f_cbStack,
    __inout_bcount( f_cbStack ) const DRM_BYTE                *f_pbPage,
    __in                              DRM_DWORD                f_cbPage,
    __out                             DRM_MOVE_PAGE           *f_pPage )
{
    UNREFERENCED_PARAMETER( f_pbStack );
    UNREFERENCED_PARAMETER( f_cbStack );
    UNREFERENCED_PARAMETER( f_pbPage  );
    UNREFERENCED_PARAMETER( f_cbPage  );
    UNREFERENCED_PARAMETER( f_pPage   );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MOV_Index_Start(
    __inout_bcount( f_cbStack ) DRM_BYTE                *f_pbStack,
    __in                        DRM_DWORD                f_cbStack,
    __inout_ecount( 1 )         DRM_XB_BUILDER_CONTEXT  *f_pcontextBuilder )
{
    UNREFERENCED_PARAMETER( f_pbStack         );
    UNREFERENCED_PARAMETER( f_cbStack         );
    UNREFERENCED_PARAMETER( f_pcontextBuilder );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MOV_Index_Finish(
    __inout_ecount( 1 )             const DRM_XB_BUILDER_CONTEXT  *f_pcontextBuilder,
    __inout_bcount(*f_pcbMoveIndex)       DRM_BYTE                *f_pbMoveIndex,
    __inout                               DRM_DWORD               *f_pcbMoveIndex,
    __in                                  DRM_MOV_CB_SIGNATURE     f_pfnSignatureCallback,
    __in                            const DRM_VOID                *f_pvSignatureContext )
{
    UNREFERENCED_PARAMETER( f_pcontextBuilder      );
    UNREFERENCED_PARAMETER( f_pbMoveIndex          );
    UNREFERENCED_PARAMETER( f_pcbMoveIndex         );
    UNREFERENCED_PARAMETER( f_pfnSignatureCallback );
    UNREFERENCED_PARAMETER( f_pvSignatureContext   );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MI_ParseIndex(
    __inout_bcount( f_cbStack )       DRM_BYTE                *f_pbStack,
    __in                              DRM_DWORD                f_cbStack,
    __inout_bcount( f_cbStack ) const DRM_BYTE                *f_pbIndex,
    __in                              DRM_DWORD                f_cbIndex,
    __out                             DRM_MOVE_INDEX          *f_pIndex)
{
    UNREFERENCED_PARAMETER( f_pbStack );
    UNREFERENCED_PARAMETER( f_cbStack );
    UNREFERENCED_PARAMETER( f_pbIndex );
    UNREFERENCED_PARAMETER( f_cbIndex );
    UNREFERENCED_PARAMETER( f_pIndex  );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MOV_ParseIndex(
    __inout_bcount( f_cbStack )       DRM_BYTE                *f_pbStack,
    __in                              DRM_DWORD                f_cbStack,
    __inout_bcount( f_cbStack ) const DRM_BYTE                *f_pbIndex,
    __in                              DRM_DWORD                f_cbIndex,
    __out                             DRM_MOVE_INDEX          *f_pIndex,
    __inout                           DRM_BYTE                 f_rgbCryptoContext[__CB_DECL(DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE)])
{
    UNREFERENCED_PARAMETER( f_pbStack          );
    UNREFERENCED_PARAMETER( f_cbStack          );
    UNREFERENCED_PARAMETER( f_pbIndex          );
    UNREFERENCED_PARAMETER( f_cbIndex          );
    UNREFERENCED_PARAMETER( f_pIndex           );
    UNREFERENCED_PARAMETER( f_rgbCryptoContext );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MOV_GenerateMoveChallenge(
    __in                                      DRM_VOID              *f_poOEMContext,
    __in                                      DRM_BB_CONTEXT        *f_poBBXContext,
    __in                                const DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext,
    __in_bcount( f_cbDeviceCert )       const DRM_BYTE              *f_pbDeviceCert,
    __in                                      DRM_DWORD              f_cbDeviceCert,
    __in_ecount(f_cLicensesToMove)      const DRM_LID               *f_prgLIDsToMove,
    __in                                      DRM_DWORD              f_cLicensesToMove,
    __out                                     DRM_ID                *f_pidNonce,
    __out_bcount_opt( *f_pcbChallenge )       DRM_BYTE              *f_pbChallenge,
    __inout                                   DRM_DWORD             *f_pcbChallenge )
{
    UNREFERENCED_PARAMETER( f_poOEMContext       );
    UNREFERENCED_PARAMETER( f_poBBXContext       );
    UNREFERENCED_PARAMETER( f_poMoveStoreContext );
    UNREFERENCED_PARAMETER( f_pbDeviceCert       );
    UNREFERENCED_PARAMETER( f_cbDeviceCert       );
    UNREFERENCED_PARAMETER( f_prgLIDsToMove      );
    UNREFERENCED_PARAMETER( f_cLicensesToMove    );
    UNREFERENCED_PARAMETER( f_pidNonce           );
    UNREFERENCED_PARAMETER( f_pbChallenge        );
    UNREFERENCED_PARAMETER( f_pcbChallenge       );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MOV_ProcessMoveResponse(
    __in                           DRM_BB_CONTEXT        *f_poBBXContext,
    __in                           DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext,
    __inout_bcount( f_cbResponse ) DRM_BYTE              *f_pbResponse,
    __in                           DRM_DWORD              f_cbResponse,
    __in                     const DRM_ID                *f_pNonce,
    __out                          DRM_RESULT            *f_pResult )
{
    UNREFERENCED_PARAMETER( f_poBBXContext       );
    UNREFERENCED_PARAMETER( f_poMoveStoreContext );
    UNREFERENCED_PARAMETER( f_pbResponse         );
    UNREFERENCED_PARAMETER( f_cbResponse         );
    UNREFERENCED_PARAMETER( f_pNonce             );
    UNREFERENCED_PARAMETER( f_pResult            );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MOV_GenerateRefreshChallenge(
    __in                                DRM_VOID              *f_poOEMContext,
    __in                                DRM_BB_CONTEXT        *f_poBBXContext,
    __in_bcount( f_cbDeviceCert ) const DRM_BYTE              *f_pbDeviceCert,
    __in                                DRM_DWORD              f_cbDeviceCert,
    __in                                DRM_DWORD              f_dwIndexSerialNumber,
    __in_ecount(f_cPagesToRefresh)const DRM_DWORD             *f_pdwPagesToRefresh,
    __in                                DRM_DWORD              f_cPagesToRefresh,
    __out                               DRM_ID                *f_pidNonce,
    __out_bcount_opt( *f_pcbChallenge ) DRM_BYTE              *f_pbChallenge,
    __inout                             DRM_DWORD             *f_pcbChallenge )
{
    UNREFERENCED_PARAMETER( f_poOEMContext        );
    UNREFERENCED_PARAMETER( f_poBBXContext        );
    UNREFERENCED_PARAMETER( f_pbDeviceCert        );
    UNREFERENCED_PARAMETER( f_cbDeviceCert        );
    UNREFERENCED_PARAMETER( f_dwIndexSerialNumber );
    UNREFERENCED_PARAMETER( f_pdwPagesToRefresh   );
    UNREFERENCED_PARAMETER( f_cPagesToRefresh     );
    UNREFERENCED_PARAMETER( f_pidNonce            );
    UNREFERENCED_PARAMETER( f_pbChallenge         );
    UNREFERENCED_PARAMETER( f_pcbChallenge        );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MOV_ProcessRefreshResponse(
    __in                           DRM_BB_CONTEXT        *f_poBBXContext,
    __in                           DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext,
    __inout_bcount( f_cbResponse ) DRM_BYTE              *f_pbResponse,
    __in                           DRM_DWORD              f_cbResponse,
    __in                     const DRM_ID                *f_pidNonce,
    __out                          DRM_RESULT            *f_pResult )
{
    UNREFERENCED_PARAMETER( f_poBBXContext       );
    UNREFERENCED_PARAMETER( f_poMoveStoreContext );
    UNREFERENCED_PARAMETER( f_pbResponse         );
    UNREFERENCED_PARAMETER( f_cbResponse         );
    UNREFERENCED_PARAMETER( f_pidNonce           );
    UNREFERENCED_PARAMETER( f_pResult            );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MOV_GenerateCopyChallenge(
    __in                                      DRM_VOID              *f_poOEMContext,
    __in                                      DRM_BB_CONTEXT        *f_poBBXContext,
    __in                                const DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext,
    __in_bcount( f_cbDeviceCert )       const DRM_BYTE              *f_pbDeviceCert,
    __in                                      DRM_DWORD              f_cbDeviceCert,
    __in_ecount(f_cLicensesToCopy)      const DRM_LID               *f_prgLIDsToCopy,
    __in_ecount(f_cLicensesToCopy)      const DRM_DWORD             *f_rgdwOriginalCopyCount,
    __in                                      DRM_DWORD              f_cLicensesToCopy,
    __out                                     DRM_ID                *f_pidNonce,
    __out_bcount_opt( *f_pcbChallenge )       DRM_BYTE              *f_pbChallenge,
    __inout                                   DRM_DWORD             *f_pcbChallenge )
{
    UNREFERENCED_PARAMETER( f_poOEMContext          );
    UNREFERENCED_PARAMETER( f_poBBXContext          );
    UNREFERENCED_PARAMETER( f_poMoveStoreContext    );
    UNREFERENCED_PARAMETER( f_pbDeviceCert          );
    UNREFERENCED_PARAMETER( f_cbDeviceCert          );
    UNREFERENCED_PARAMETER( f_prgLIDsToCopy         );
    UNREFERENCED_PARAMETER( f_rgdwOriginalCopyCount );
    UNREFERENCED_PARAMETER( f_cLicensesToCopy       );
    UNREFERENCED_PARAMETER( f_pidNonce              );
    UNREFERENCED_PARAMETER( f_pbChallenge           );
    UNREFERENCED_PARAMETER( f_pcbChallenge          );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MOV_ProcessCopyResponse(
    __in                           DRM_BB_CONTEXT        *f_poBBXContext,
    __in                           DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext,
    __inout_bcount( f_cbResponse ) DRM_BYTE              *f_pbResponse,
    __in                           DRM_DWORD              f_cbResponse,
    __in                     const DRM_ID                *f_pNonce,
    __out                          DRM_RESULT            *f_pResult )
{
    UNREFERENCED_PARAMETER( f_poBBXContext       );
    UNREFERENCED_PARAMETER( f_poMoveStoreContext );
    UNREFERENCED_PARAMETER( f_pbResponse         );
    UNREFERENCED_PARAMETER( f_cbResponse         );
    UNREFERENCED_PARAMETER( f_pNonce             );
    UNREFERENCED_PARAMETER( f_pResult            );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MOVST_UpdateMovePage(
    __in                          DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext,
    __in                          DRM_DWORD              f_iPage,
    __in_bcount( f_cbPage ) const DRM_BYTE              *f_pbPage,
    __in                          DRM_DWORD              f_cbPage )
{
    UNREFERENCED_PARAMETER( f_poMoveStoreContext );
    UNREFERENCED_PARAMETER( f_iPage              );
    UNREFERENCED_PARAMETER( f_pbPage             );
    UNREFERENCED_PARAMETER( f_cbPage             );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MOVST_GetMovePage(
    __in                           DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext,
    __in                           DRM_DWORD              f_iPage,
    __out_bcount_opt( *f_pcbPage ) DRM_BYTE              *f_pbPage,
    __inout                        DRM_DWORD             *f_pcbPage )
{
    UNREFERENCED_PARAMETER( f_poMoveStoreContext );
    UNREFERENCED_PARAMETER( f_iPage              );
    UNREFERENCED_PARAMETER( f_pbPage             );
    UNREFERENCED_PARAMETER( f_pcbPage            );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MOVST_OpenStore(
    __in  DRM_DST               *f_pDatastore,
    __out DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext )
{
    UNREFERENCED_PARAMETER( f_pDatastore         );
    UNREFERENCED_PARAMETER( f_poMoveStoreContext );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MOVST_CloseStore(
    __in DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext )
{
    UNREFERENCED_PARAMETER( f_poMoveStoreContext );
    return DRM_E_NOTIMPL;
}

PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */
PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */

EXIT_PK_NAMESPACE_CODE;

