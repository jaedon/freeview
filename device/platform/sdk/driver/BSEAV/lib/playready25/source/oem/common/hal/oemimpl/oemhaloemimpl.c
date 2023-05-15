/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

/******************************************************************************
**
** This file defines the interface between the CoreAPI portion of the secure crypto core
**      (which the OEM should not need to change [except to get it to compile properly])
**      and the OEM-hardware-specific portion of the secure crypto core.
**      Refer to additional comments in oemhaloemimpl.h.
** If the device supports a secure crypto core:
**      This file should define the interface for code the OEM needs to re-implement
**      from scratch with OEM-hardware-specific functionality.  The code in this
**      file is a reference implementation only and is likely not representative of
**      any realistic OEM-hardware-specific implementation.  The OEM should study the
**      code in this file carefully and replace it with an appropriate implementation.
** If the device does not support a secure crypto core:
**      OEMs should not need to change this file in any way (except to get it to compile properly).
**
*******************************************************************************
*/
#define DRM_BUILDING_OEMHALOEMIMPL_C

#include <drmbcrltypes.h>
#include <oemhaloemimpl.h>
#include <oemkeyfileconsts.h>
#include <drmblackboxtypes.h>
#include <stdio.h>

#if DRM_TEST_EXPOSE_TEST_FUNCTIONS
#include <oemhaltest.h>
static DRM_BOOL g_fSkipLeafCertificateValidityCheckDuringSignOperation = FALSE;
static DRM_BOOL g_fSkipLeafCertificateKeysValidityCheckDuringSignOperation = FALSE;
#endif

#include <drmlastinclude.h>

/*********************************************************************************************
** Function:  _XOR
**
** Synopsis:  Byte-by-byte XOR of two buffers using two indices.
**
** Arguments: [f_pbLHS]  : Pointer to the left-hand side
**            [f_ibLHS]  : Index into the LHS to start XOR at
**            [f_pbRHS]  : Pointer to the right-hand side
**            [f_ibRHS]  : Index into the RHS to start XOR at
**            [f_cb]     : Byte count to XOR
**
** Returns:   DRM_VOID
**********************************************************************************************/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_WRITABLE_BUFFER_25057, "f_pbLHS length defined by f_ibLHS and f_cb" )
static DRMFORCEINLINE DRM_VOID _XOR(
    __inout_bcount( f_ibLHS + f_cb )    DRM_BYTE   *f_pbLHS,
    __in                                DRM_DWORD   f_ibLHS,
    __in_bcount( f_ibRHS + f_cb ) const DRM_BYTE   *f_pbRHS,
    __in                                DRM_DWORD   f_ibRHS,
    __in                                DRM_DWORD   f_cb )
PREFAST_POP
{
    DRM_DWORD i = 0;

    for(  ; i < f_cb; i++ )
    {
        PUT_BYTE( f_pbLHS,
                  i + f_ibLHS,
                  (DRM_BYTE)( GET_BYTE( f_pbLHS, i + f_ibLHS ) ^ GET_BYTE( f_pbRHS, i + f_ibRHS ) ) );
    }
}

/*
** OEM_MANDATORY:
** This constant MUST be set to 2000 in production environments.
*/
#define OEM_KEY_GENERATION_SECURITY_LEVEL 2000 /*Modified*/

#ifdef HWDECODING
/*
** OEM_MANDATORY:
** OEM_DECRYPTOR_TYPE_AES_128_CTR dependency value In Calypso Decryptor.h (enum DECRYPTOR_Type_e)
** IF Calypso Decryptor.h FILE IS MODIFIED, "OEM_DECRYPTOR_TYPE_AES_128_CTR" MUST BE SET TO Calypso Decryptor.h enum value
*/
#define OEM_DECRYPTOR_TYPE_AES_128_CTR	1
#endif
/*
** Note to OEMs: Unless keyfiles created before PK 2.5 are being used with a post-PK 2.5 implementation,
** i.e. unless key history is being maintained from a previous key file, this #define should be set to 0.
** Setting this #define to a non-zero value opens a security risk when processesing PK 2.5 keyfiles.
** This #define should NEVER be set to a non-zero value when running this code on a secure crypto core.
*/
#define OEM_SUPPORT_KEY_HISTORY_PRE_PK_2_5  0

#if DRM_SUPPORT_SECUREOEMHAL
#if OEM_SUPPORT_KEY_HISTORY_PRE_PK_2_5
#error DRM_SUPPORT_SECUREOEMHAL is set.  You cannot implement a secure HAL that supports key history from pre-PK 2.5.  You must set OEM_SUPPORT_KEY_HISTORY_PRE_PK_2_5 to 0.
#endif /* DRM_SUPPORT_SECUREOEMHAL */
#endif /* OEM_SUPPORT_KEY_HISTORY_PRE_PK_2_5 */

#if DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
#if !DRM_SUPPORT_SECUREOEMHAL
#error DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY is set to 1.  You must also set DRM_SUPPORT_SECUREOEMHAL to 1.
#endif /* !DRM_SUPPORT_SECUREOEMHAL */
#endif /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */

/******************************************************************************
**
** This section deals with revocation.
**
******************************************************************************/
typedef struct __tagOEM_HALIMPL_REVOCATION_INFO
{
    DRM_DWORD            dwStoredRIV;                               /* Version of the RIV data              */
    DRM_DWORD            dwStoredRequiredCRLVersionFromRevInfo;     /* CRL Version reported in the RIV data */
    DRM_DWORD            dwStoredCRLVersion;                        /* CRL Version reported in the CRL      */
    DRM_RevocationEntry *pCRLEntries;                               /* Revocation entries for current CRL   */
    DRM_DWORD            cCRLEntries;                               /* Number of revocation entries         */
} OEM_HALIMPL_REVOCATION_INFO;

static OEM_HALIMPL_REVOCATION_INFO g_RevocationInfo = { 0, 0, 0, NULL, 0 };

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_ReadStoredRIV(
    __out                                           DRM_DWORD                       *f_pdwStoredRIV )
{
    DRM_RESULT dr = DRM_SUCCESS;

    AssertChkArg( f_pdwStoredRIV != NULL );
    *f_pdwStoredRIV = g_RevocationInfo.dwStoredRIV;

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_WriteStoredRIV(
    __in                                            DRM_DWORD                        f_dwStoredRIV )
{
    g_RevocationInfo.dwStoredRIV = f_dwStoredRIV;
    return DRM_SUCCESS;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_ReadStoredRequiredCRLVersionFromRevInfo(
    __out                                           DRM_DWORD                       *f_pdwStoredRequiredCRLVersionFromRevInfo )
{
    DRM_RESULT dr = DRM_SUCCESS;

    AssertChkArg( f_pdwStoredRequiredCRLVersionFromRevInfo != NULL );
    *f_pdwStoredRequiredCRLVersionFromRevInfo = g_RevocationInfo.dwStoredRequiredCRLVersionFromRevInfo;

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_WriteStoredRequiredCRLVersionFromRevInfo(
    __in                                            DRM_DWORD                        f_dwStoredRequiredCRLVersionFromRevInfo )
{
    g_RevocationInfo.dwStoredRequiredCRLVersionFromRevInfo = f_dwStoredRequiredCRLVersionFromRevInfo;
    return DRM_SUCCESS;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_ReadStoredCRLVersion(
    __out                                           DRM_DWORD                       *f_pdwStoredCRLVersion )
{
    DRM_RESULT dr = DRM_SUCCESS;

    AssertChkArg( f_pdwStoredCRLVersion != NULL );
    *f_pdwStoredCRLVersion = g_RevocationInfo.dwStoredCRLVersion;

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_WriteStoredCRLVersion(
    __in                                            DRM_DWORD                        f_dwStoredCRLVersion )
{
    g_RevocationInfo.dwStoredCRLVersion = f_dwStoredCRLVersion;
    return DRM_SUCCESS;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_ReadStoredCRLEntryCount(
    __out                                           DRM_DWORD                       *f_pdwStoredCRLEntryCount )
{
    DRM_RESULT dr = DRM_SUCCESS;

    AssertChkArg( f_pdwStoredCRLEntryCount != NULL );
    *f_pdwStoredCRLEntryCount = g_RevocationInfo.cCRLEntries;

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_ReadStoredCRLEntry(
    __in                                            DRM_DWORD                        f_dwEntryIndex,
    __out                                           DRM_RevocationEntry             *f_pCRLEntry )
{
    DRM_RESULT dr = DRM_SUCCESS;

    AssertChkArg( f_pCRLEntry != NULL );
    AssertChkArg( f_dwEntryIndex < g_RevocationInfo.cCRLEntries );
    OEM_SECURE_MEMCPY( f_pCRLEntry, g_RevocationInfo.pCRLEntries[f_dwEntryIndex], SIZEOF( DRM_RevocationEntry ) );

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_WriteStoredCRLEntries(
    __in                                            DRM_DWORD                        f_cCRLEntries,
    __in_ecount_opt(f_cCRLEntries)                  DRM_RevocationEntry             *f_pCRLEntries )
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_DWORD   iEntry  = 0;

    g_RevocationInfo.cCRLEntries = 0;
    SAFE_OEM_FREE( g_RevocationInfo.pCRLEntries );

    if( f_cCRLEntries > 0 )
    {
        AssertChkArg( f_pCRLEntries != NULL );

        ChkMem( g_RevocationInfo.pCRLEntries = ( DRM_RevocationEntry* )Oem_MemAlloc( f_cCRLEntries * SIZEOF( DRM_RevocationEntry ) ) );
        OEM_SECURE_ZERO_MEMORY( g_RevocationInfo.pCRLEntries, f_cCRLEntries * SIZEOF( DRM_RevocationEntry ) );

        for( iEntry = 0; iEntry < f_cCRLEntries; iEntry++ )
        {
            OEM_SECURE_MEMCPY( g_RevocationInfo.pCRLEntries[iEntry], f_pCRLEntries[iEntry], SIZEOF( DRM_RevocationEntry ) );
        }

        g_RevocationInfo.cCRLEntries = f_cCRLEntries;
    }

ErrorExit:
    return dr;
}

/******************************************************************************
**
** This section deals with key registers.
**
******************************************************************************/

/*
** Each blackbox (i.e. app context) has a fixed number of keys of each type it can hold (defined in drmblackbox.h).
** This constant defines the number of concurrent blackboxes supported.
*/
#define OEM_HAL_MAX_NUM_BLACKBOXES                     11

#define OEM_HAL_NUMREGISTERS_ECC_P256_CONTENT_ENCRYPT       ( OEM_BB_KEYMAP_SIZE_ECC_P256_CONTENT_ENCRYPT       * OEM_HAL_MAX_NUM_BLACKBOXES )
#define OEM_HAL_NUMREGISTERS_ECC_P256_DOMAIN_ENCRYPT        ( OEM_BB_KEYMAP_SIZE_ECC_P256_DOMAIN_ENCRYPT        * OEM_HAL_MAX_NUM_BLACKBOXES )
#define OEM_HAL_NUMREGISTERS_ECC_P256_MESSAGE_ENCRYPT       ( OEM_BB_KEYMAP_SIZE_ECC_P256_MESSAGE_ENCRYPT       * OEM_HAL_MAX_NUM_BLACKBOXES )
#define OEM_HAL_NUMREGISTERS_ECC_P256_SIGN                  ( OEM_BB_KEYMAP_SIZE_ECC_P256_SIGN                  * OEM_HAL_MAX_NUM_BLACKBOXES )
#define OEM_HAL_NUMREGISTERS_ECC_P256_PUBLIC                ( OEM_BB_KEYMAP_SIZE_ECC_P256_PUBLIC                * OEM_HAL_MAX_NUM_BLACKBOXES )
#define OEM_HAL_NUMREGISTERS_ECC_P256_MODEL_SIGN            ( OEM_BB_KEYMAP_SIZE_ECC_P256_MODEL_SIGN            * OEM_HAL_MAX_NUM_BLACKBOXES )
#define OEM_HAL_NUMREGISTERS_AES_ECB_ROOT                   ( OEM_BB_KEYMAP_SIZE_AES_ECB_ROOT                   * OEM_HAL_MAX_NUM_BLACKBOXES )
#define OEM_HAL_NUMREGISTERS_AES_CTR_CONTENT                ( OEM_BB_KEYMAP_SIZE_AES_CTR_CONTENT                * OEM_HAL_MAX_NUM_BLACKBOXES )
#define OEM_HAL_NUMREGISTERS_AES_ECB_MESSAGE_ENCRYPT        ( OEM_BB_KEYMAP_SIZE_AES_ECB_MESSAGE_ENCRYPT        * OEM_HAL_MAX_NUM_BLACKBOXES )
#define OEM_HAL_NUMREGISTERS_AES_ECB_DOMAIN_PROTECT         ( OEM_BB_KEYMAP_SIZE_AES_ECB_DOMAIN_PROTECT         * OEM_HAL_MAX_NUM_BLACKBOXES )
#define OEM_HAL_NUMREGISTERS_AES_OMAC1_SECURE_STORE         ( OEM_BB_KEYMAP_SIZE_AES_OMAC1_SECURE_STORE         * OEM_HAL_MAX_NUM_BLACKBOXES )
#define OEM_HAL_NUMREGISTERS_AES_KEYFILE                    ( OEM_BB_KEYMAP_SIZE_AES_KEYFILE                    * OEM_HAL_MAX_NUM_BLACKBOXES )
#define OEM_HAL_NUMREGISTERS_AES_GENERIC_KEY_CONTENT        ( OEM_BB_KEYMAP_SIZE_AES_GENERIC_KEY_CONTENT        * OEM_HAL_MAX_NUM_BLACKBOXES )
#define OEM_HAL_NUMREGISTERS_AES_GENERIC_KEY_ROOT           ( OEM_BB_KEYMAP_SIZE_AES_GENERIC_KEY_ROOT           * OEM_HAL_MAX_NUM_BLACKBOXES )
#define OEM_HAL_NUMREGISTERS_AES_ECB_NEW_LICENSE_ROOT       ( OEM_BB_KEYMAP_SIZE_AES_ECB_NEW_LICENSE_ROOT       * OEM_HAL_MAX_NUM_BLACKBOXES )
#define OEM_HAL_NUMREGISTERS_AES_CTR_NEW_LICENSE_CONTENT    ( OEM_BB_KEYMAP_SIZE_AES_CTR_NEW_LICENSE_CONTENT    * OEM_HAL_MAX_NUM_BLACKBOXES )
#if !DRM_SUPPORT_SECUREOEMHAL
#define OEM_HAL_NUMREGISTERS_LEGACY                         ( OEM_BB_KEYMAP_SIZE_LEGACY                         * OEM_HAL_MAX_NUM_BLACKBOXES )
#define OEM_HAL_NUMREGISTERS_AES_SLK                        ( OEM_BB_KEYMAP_SIZE_SLK                            * OEM_HAL_MAX_NUM_BLACKBOXES )
#endif /* !DRM_SUPPORT_SECUREOEMHAL */

/* Audio and Video */
#define OEM_HAL_MAX_NUM_STREAMS  2

/*
** We allow a number of encryption contexts up to OEM_HAL_MAX_NUM_STREAMS times the number of supported blackboxes.
** This enables each piece of content to have two encrypted streams using unique keys.
*/
#define OEM_HAL_NUM_ENCRYPTION_CONTEXTS                ( OEM_HAL_MAX_NUM_BLACKBOXES * OEM_HAL_MAX_NUM_STREAMS )

/*
** Set of key registers to hold the actual key material and key-type-specific state information for each key
*/
static OEM_HAL_ECC256_KEY_PAIR   g_rgKeyRegistersEccP256ContentEncrypt  [OEM_HAL_NUMREGISTERS_ECC_P256_CONTENT_ENCRYPT]     = {0};
static OEM_HAL_ECC256_KEY_PAIR   g_rgKeyRegistersEccP256DomainEncrypt   [OEM_HAL_NUMREGISTERS_ECC_P256_DOMAIN_ENCRYPT]      = {0};
static OEM_HAL_ECC256_KEY_PAIR   g_rgKeyRegistersEccP256MessageEncrypt  [OEM_HAL_NUMREGISTERS_ECC_P256_MESSAGE_ENCRYPT]     = {0};
static OEM_HAL_ECC256_KEY_PAIR   g_rgKeyRegistersEccP256Sign            [OEM_HAL_NUMREGISTERS_ECC_P256_SIGN]                = {0};
static OEM_HAL_ECC256_PUBLIC_KEY g_rgKeyRegistersEccP256Public          [OEM_HAL_NUMREGISTERS_ECC_P256_PUBLIC]              = {0};
static OEM_HAL_ECC256_KEY_PAIR   g_rgKeyRegistersEccP256ModelSign       [OEM_HAL_NUMREGISTERS_ECC_P256_MODEL_SIGN]          = {0};
static OEM_HAL_AES_ROOT_KEY_PAIR g_rgKeyRegistersAesEcbRoot             [OEM_HAL_NUMREGISTERS_AES_ECB_ROOT]                 = {0};
static OEM_HAL_AES_KEY_PAIR      g_rgKeyRegistersAesCtrContent          [OEM_HAL_NUMREGISTERS_AES_CTR_CONTENT]              = {0};
static OEM_HAL_AES_KEY_PAIR      g_rgKeyRegistersAesEcbMessageEncrypt   [OEM_HAL_NUMREGISTERS_AES_ECB_MESSAGE_ENCRYPT]      = {0};
static OEM_HAL_AES_KEY_PAIR      g_rgKeyRegistersAesEcbDomainProtect    [OEM_HAL_NUMREGISTERS_AES_ECB_DOMAIN_PROTECT]       = {0};
static OEM_HAL_AES_KEY           g_rgKeyRegistersAesOmac1SecureStore    [OEM_HAL_NUMREGISTERS_AES_OMAC1_SECURE_STORE]       = {0};
static OEM_HAL_AES_KEY           g_rgKeyRegistersAesKeyFile             [OEM_HAL_NUMREGISTERS_AES_KEYFILE]                  = {0};
#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
static OEM_HAL_AES_KEY           g_rgKeyRegistersAesGenericKeyContent   [OEM_HAL_NUMREGISTERS_AES_GENERIC_KEY_CONTENT]      = {0};
static OEM_HAL_AES_KEY           g_rgKeyRegistersAesGenericKeyRoot      [OEM_HAL_NUMREGISTERS_AES_GENERIC_KEY_ROOT]         = {0};
static OEM_HAL_AES_ROOT_KEY_PAIR g_rgKeyRegistersAesEcbNewLicenseRoot   [OEM_HAL_NUMREGISTERS_AES_ECB_NEW_LICENSE_ROOT]     = {0};
static OEM_HAL_AES_KEY_PAIR      g_rgKeyRegistersAesCtrNewLicenseContent[OEM_HAL_NUMREGISTERS_AES_CTR_NEW_LICENSE_CONTENT]  = {0};
#endif /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
#if !DRM_SUPPORT_SECUREOEMHAL
static OEM_HAL_LEGACY_KEY        g_rgKeyRegistersLegacy                 [OEM_HAL_NUMREGISTERS_LEGACY]                       = {0};
static OEM_HAL_AES_KEY           g_rgKeyRegistersAesSlk                 [OEM_HAL_NUMREGISTERS_AES_SLK]                      = {0};
#endif /* !DRM_SUPPORT_SECUREOEMHAL */

static OEM_HAL_KEY_REGISTER_STATE g_rgKeyRegStatesEccP256ContentEncrypt  [OEM_HAL_NUMREGISTERS_ECC_P256_CONTENT_ENCRYPT] = { eOEM_HAL_KEY_REGISTER_STATE_UNALLOCATED };
static OEM_HAL_KEY_REGISTER_STATE g_rgKeyRegStatesEccP256DomainEncrypt   [OEM_HAL_NUMREGISTERS_ECC_P256_DOMAIN_ENCRYPT]  = { eOEM_HAL_KEY_REGISTER_STATE_UNALLOCATED };
static OEM_HAL_KEY_REGISTER_STATE g_rgKeyRegStatesEccP256MessageEncrypt  [OEM_HAL_NUMREGISTERS_ECC_P256_MESSAGE_ENCRYPT] = { eOEM_HAL_KEY_REGISTER_STATE_UNALLOCATED };
static OEM_HAL_KEY_REGISTER_STATE g_rgKeyRegStatesEccP256Sign            [OEM_HAL_NUMREGISTERS_ECC_P256_SIGN]            = { eOEM_HAL_KEY_REGISTER_STATE_UNALLOCATED };
static OEM_HAL_KEY_REGISTER_STATE g_rgKeyRegStatesEccP256Public          [OEM_HAL_NUMREGISTERS_ECC_P256_PUBLIC]          = { eOEM_HAL_KEY_REGISTER_STATE_UNALLOCATED };
static OEM_HAL_KEY_REGISTER_STATE g_rgKeyRegStatesEccP256ModelSign       [OEM_HAL_NUMREGISTERS_ECC_P256_MODEL_SIGN]      = { eOEM_HAL_KEY_REGISTER_STATE_UNALLOCATED };
static OEM_HAL_KEY_REGISTER_STATE g_rgKeyRegStatesAesEcbRoot             [OEM_HAL_NUMREGISTERS_AES_ECB_ROOT]             = { eOEM_HAL_KEY_REGISTER_STATE_UNALLOCATED };
static OEM_HAL_KEY_REGISTER_STATE g_rgKeyRegStatesAesCtrContent          [OEM_HAL_NUMREGISTERS_AES_CTR_CONTENT]          = { eOEM_HAL_KEY_REGISTER_STATE_UNALLOCATED };
static OEM_HAL_KEY_REGISTER_STATE g_rgKeyRegStatesAesEcbMessageEncrypt   [OEM_HAL_NUMREGISTERS_AES_ECB_MESSAGE_ENCRYPT]  = { eOEM_HAL_KEY_REGISTER_STATE_UNALLOCATED };
static OEM_HAL_KEY_REGISTER_STATE g_rgKeyRegStatesAesEcbDomainProtect    [OEM_HAL_NUMREGISTERS_AES_ECB_DOMAIN_PROTECT]   = { eOEM_HAL_KEY_REGISTER_STATE_UNALLOCATED };
static OEM_HAL_KEY_REGISTER_STATE g_rgKeyRegStatesAesOmac1SecureStore    [OEM_HAL_NUMREGISTERS_AES_OMAC1_SECURE_STORE]   = { eOEM_HAL_KEY_REGISTER_STATE_UNALLOCATED };
static OEM_HAL_KEY_REGISTER_STATE g_rgKeyRegStatesAesKeyFile             [OEM_HAL_NUMREGISTERS_AES_KEYFILE]              = { eOEM_HAL_KEY_REGISTER_STATE_UNALLOCATED };
static OEM_HAL_KEY_REGISTER_STATE g_rgKeyRegStatesAesGenericKeyContent   [OEM_HAL_NUMREGISTERS_AES_GENERIC_KEY_CONTENT]  = { eOEM_HAL_KEY_REGISTER_STATE_UNALLOCATED };
static OEM_HAL_KEY_REGISTER_STATE g_rgKeyRegStatesAesGenericKeyRoot      [OEM_HAL_NUMREGISTERS_AES_GENERIC_KEY_ROOT]     = { eOEM_HAL_KEY_REGISTER_STATE_UNALLOCATED };
static OEM_HAL_KEY_REGISTER_STATE g_rgKeyRegStatesAesEcbNewLicenseRoot   [OEM_HAL_NUMREGISTERS_AES_ECB_ROOT]             = { eOEM_HAL_KEY_REGISTER_STATE_UNALLOCATED };
static OEM_HAL_KEY_REGISTER_STATE g_rgKeyRegStatesAesCtrNewLicenseContent[OEM_HAL_NUMREGISTERS_AES_CTR_CONTENT]          = { eOEM_HAL_KEY_REGISTER_STATE_UNALLOCATED };
#if !DRM_SUPPORT_SECUREOEMHAL
static OEM_HAL_KEY_REGISTER_STATE g_rgKeyRegStatesLegacy                 [OEM_HAL_NUMREGISTERS_LEGACY]                   = { eOEM_HAL_KEY_REGISTER_STATE_UNALLOCATED };
static OEM_HAL_KEY_REGISTER_STATE g_rgKeyRegStatesSlk                    [OEM_HAL_NUMREGISTERS_AES_SLK]                  = { eOEM_HAL_KEY_REGISTER_STATE_UNALLOCATED };
#endif /* !DRM_SUPPORT_SECUREOEMHAL */

static OEM_HAL_PRELOADED_KEY_ID   g_rgPreloadedKeyInfo[OEM_HAL_MAX_PRELOADED_KEYS]                                      = {0};
static DRM_LONG                   g_cPreloadedKeyInfoSlotsUsed                                                          = 0;
#ifdef HWDECODING
static void*  					  Decryptor_CTX																			= NULL;
#endif
/*
**
** It is important that the entries in g_rgRegisterStateTables are positioned such that
** using OEM_HAL_KEY_TYPE as an index resolves to the DRM_BB_KEY_REGISTER_TABLE
** representing the keys of that type.
**
*/
#define KEY_REGSTATE_TABLE_ENTRY( StateTable ) { NO_OF( StateTable ), StateTable }

#if !DRM_SUPPORT_SECUREOEMHAL
static OEM_HAL_REGISTER_STATE_TABLE g_rgRegisterStateTables[ OEM_HAL_KEY_TYPE_NUM_VALID_TYPES ] =
#else /* !DRM_SUPPORT_SECUREOEMHAL */
static OEM_HAL_REGISTER_STATE_TABLE g_rgRegisterStateTables[ OEM_HAL_KEY_TYPE_NUM_VALID_TYPES - 2 ] =
#endif /* !DRM_SUPPORT_SECUREOEMHAL */
{
    KEY_REGSTATE_TABLE_ENTRY( g_rgKeyRegStatesEccP256ContentEncrypt ),
    KEY_REGSTATE_TABLE_ENTRY( g_rgKeyRegStatesEccP256DomainEncrypt ),
    KEY_REGSTATE_TABLE_ENTRY( g_rgKeyRegStatesEccP256MessageEncrypt ),
    KEY_REGSTATE_TABLE_ENTRY( g_rgKeyRegStatesEccP256Sign ),
    KEY_REGSTATE_TABLE_ENTRY( g_rgKeyRegStatesEccP256Public ),
    KEY_REGSTATE_TABLE_ENTRY( g_rgKeyRegStatesEccP256ModelSign ),
    KEY_REGSTATE_TABLE_ENTRY( g_rgKeyRegStatesAesEcbRoot ),
    KEY_REGSTATE_TABLE_ENTRY( g_rgKeyRegStatesAesCtrContent ),
    KEY_REGSTATE_TABLE_ENTRY( g_rgKeyRegStatesAesEcbMessageEncrypt ),
    KEY_REGSTATE_TABLE_ENTRY( g_rgKeyRegStatesAesEcbDomainProtect ),
    KEY_REGSTATE_TABLE_ENTRY( g_rgKeyRegStatesAesOmac1SecureStore ),
    KEY_REGSTATE_TABLE_ENTRY( g_rgKeyRegStatesAesKeyFile ),
    KEY_REGSTATE_TABLE_ENTRY( g_rgKeyRegStatesAesGenericKeyContent ),
    KEY_REGSTATE_TABLE_ENTRY( g_rgKeyRegStatesAesGenericKeyRoot ),
    KEY_REGSTATE_TABLE_ENTRY( g_rgKeyRegStatesAesEcbNewLicenseRoot ),
    KEY_REGSTATE_TABLE_ENTRY( g_rgKeyRegStatesAesCtrNewLicenseContent ),
#if !DRM_SUPPORT_SECUREOEMHAL
    KEY_REGSTATE_TABLE_ENTRY( g_rgKeyRegStatesLegacy ),
    KEY_REGSTATE_TABLE_ENTRY( g_rgKeyRegStatesSlk )
#endif /* !DRM_SUPPORT_SECUREOEMHAL */
};


static OEM_CRITICAL_SECTION                g_csProtectRegisters;
static OEM_HAL_ENCRYPTION_CONTEXT_INTERNAL g_rgEncryptionContexts[ OEM_HAL_NUM_ENCRYPTION_CONTEXTS ] = {0};
static DRM_LONG                            g_nInitCount = 0;

#define LOCK_REGISTERS( LockFlag ) do {             \
        Oem_CritSec_Enter( &g_csProtectRegisters ); \
        LockFlag = TRUE;                            \
    } while(FALSE)

/*
** Note: This macro should NEVER jump to ErrorExit or call macros (like ChkDR) that do so.
** Doing so could cause an infinite loop because it is used after the ErrorExit label.
*/
#define UNLOCK_REGISTERS( LockFlag ) do {               \
        if( LockFlag )                                  \
        {                                               \
            Oem_CritSec_Leave( &g_csProtectRegisters ); \
        }                                               \
    } while(FALSE)

static DRM_FRE_INLINE DRM_RESULT _GetKeyRegisterStateTable(
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __out_ecount( 1 )                               OEM_HAL_REGISTER_STATE_TABLE       **f_pTable ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_RESULT _GetKeyRegisterStateTable(
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __out_ecount( 1 )                               OEM_HAL_REGISTER_STATE_TABLE       **f_pTable )
{
    if( f_eKeyType           == OEM_HAL_KEY_TYPE_INVALID                        || f_eKeyType           == OEM_HAL_KEY_TYPE_UNDEFINED
#if DRM_SUPPORT_SECUREOEMHAL
     || f_eKeyType           == OEM_HAL_KEY_TYPE_LEGACY                         || f_eKeyType           == OEM_HAL_KEY_TYPE_AES_SLK
#endif /* DRM_SUPPORT_SECUREOEMHAL */
#if DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
     || f_eKeyType           == OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT        || f_eKeyType           == OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT
     || f_eKeyType           == OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT    || f_eKeyType           == OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT
#endif /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
     || (DRM_LONG)f_eKeyType <  (DRM_LONG)OEM_HAL_KEY_TYPE_INVALID              || (DRM_LONG)f_eKeyType >= (DRM_LONG)OEM_HAL_KEY_TYPE_NUM_VALID_TYPES
     )
    {
        *f_pTable = NULL;
        return DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE;
    }
    *f_pTable = &g_rgRegisterStateTables[ f_eKeyType ];
    return DRM_SUCCESS;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_VerifyHalInitialized()
{
    if( g_nInitCount > 0 )
    {
        return DRM_SUCCESS;
    }
    else
    {
        return DRM_E_OEMHAL_NOT_INITIALIZED;
    }
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_VerifyValidKeyRegister(
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_keyRegisterIndex )
{
    DRM_RESULT                    dr     = DRM_SUCCESS;
    OEM_HAL_REGISTER_STATE_TABLE *pTable = NULL;

    ChkDR( _GetKeyRegisterStateTable( f_eKeyType, &pTable ) );

    if( f_keyRegisterIndex < 0
     || f_keyRegisterIndex >= pTable->cState
     || pTable->pfState[ f_keyRegisterIndex ] == eOEM_HAL_KEY_REGISTER_STATE_UNALLOCATED )
    {
        ChkDR( DRM_E_INVALIDARG );
    }

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    _FreeDerivedRootKeyInfo
**
** Synopsis:    Allocate a register to hold derived root key information.
**
** Arguments:   [f_pRootKeyRegister]  : The root key register
**
******************************************************************************/
static DRM_FRE_INLINE DRM_VOID _FreeDerivedRootKeyInfo(
    __inout_ecount(1)                               OEM_HAL_AES_ROOT_KEY_PAIR           *f_pRootKeyRegister ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_VOID _FreeDerivedRootKeyInfo(
    __inout_ecount(1)                               OEM_HAL_AES_ROOT_KEY_PAIR           *f_pRootKeyRegister )
{
    DRMASSERT( f_pRootKeyRegister != NULL );
    __analysis_assume( f_pRootKeyRegister != NULL );

    if( f_pRootKeyRegister->pDerivedKey != NULL )
    {
        DRM_DWORD dwBytesToClean = SIZEOF( OEM_HAL_DERIVED_ROOT_KEY );

        /*
        ** Zero the additional memory allocated past the end of the OEM_HAL_DERIVED_ROOT_KEY
        */
        if( f_pRootKeyRegister->pDerivedKey->AuxKey.cEntries > 0 )
        {
            dwBytesToClean += ( f_pRootKeyRegister->pDerivedKey->AuxKey.cEntries - 1 ) * SIZEOF( DRM_XMR_AUX_KEY_ENTRY );
        }

        OEM_SECURE_ZERO_MEMORY( f_pRootKeyRegister->pDerivedKey, dwBytesToClean );

        Oem_MemFree( f_pRootKeyRegister->pDerivedKey );
        f_pRootKeyRegister->pDerivedKey = NULL;
    }
}

/*********************************************************************
**
** Function: _FreeKeyRegisterResources
**
** Synopsis: Free any resources associated with a key register
**
** Arguments:
**
** [f_eKeyType]:         A key type.
** [f_keyRegisterIndex]: A register index of the given type.
**
** Notes:      This function assumes that both type and register are valid.
**
**********************************************************************/
static DRM_FRE_INLINE DRM_VOID _FreeKeyRegisterResources(
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_keyRegisterIndex ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_VOID _FreeKeyRegisterResources(
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_keyRegisterIndex )
{
    switch( f_eKeyType )
    {
    case OEM_HAL_KEY_TYPE_AES_ECB_ROOT:
        _FreeDerivedRootKeyInfo( &g_rgKeyRegistersAesEcbRoot[ f_keyRegisterIndex ] );
        break;

    default:
        /*
        ** Just drop through: nothing to free
        */
        break;
    }
}

/*****************************************************************************
** Function:    _ZeroAesKey
**
** Synopsis:    Initialize the key structure to zero.
**
** Arguments:   [f_pKey]  : The key
**
******************************************************************************/
static DRM_FRE_INLINE DRM_VOID _ZeroAesKey(
    __inout_ecount(1)                               OEM_HAL_AES_KEY                 *f_pKey ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_VOID _ZeroAesKey(
    __inout_ecount(1)                               OEM_HAL_AES_KEY                 *f_pKey )
{
    DRMASSERT( f_pKey != NULL );
    __analysis_assume( f_pKey != NULL );
    OEM_SECURE_ZERO_MEMORY( f_pKey, SIZEOF( OEM_HAL_AES_KEY ) );
}

/*****************************************************************************
** Function:    _ZeroAesKeyPair
**
** Synopsis:    Initialize the key pair structure to zero.
**
** Arguments:   [f_pKeyPair]  : The key pair
**
******************************************************************************/
static DRM_FRE_INLINE DRM_VOID _ZeroAesKeyPair(
    __inout_ecount(1)                               OEM_HAL_AES_KEY_PAIR                *f_pKeyPair ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_VOID _ZeroAesKeyPair(
    __inout_ecount(1)                               OEM_HAL_AES_KEY_PAIR                *f_pKeyPair )
{
    DRMASSERT( f_pKeyPair != NULL );
    __analysis_assume( f_pKeyPair != NULL );
    OEM_SECURE_ZERO_MEMORY( f_pKeyPair, SIZEOF( OEM_HAL_AES_KEY_PAIR ) );
}

/*****************************************************************************
** Function:    _ZeroAesRootKeyPair
**
** Synopsis:    Initialize the root key pair structure to zero.
**
** Arguments:   [f_pRootKeyPair]  : The key pair
**
******************************************************************************/
static DRM_FRE_INLINE DRM_VOID _ZeroAesRootKeyPair(
    __inout_ecount(1)                               OEM_HAL_AES_ROOT_KEY_PAIR       *f_pRootKeyPair ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_VOID _ZeroAesRootKeyPair(
    __inout_ecount(1)                               OEM_HAL_AES_ROOT_KEY_PAIR       *f_pRootKeyPair )
{
    DRMASSERT( f_pRootKeyPair != NULL );
    __analysis_assume( f_pRootKeyPair != NULL );
    OEM_SECURE_ZERO_MEMORY( f_pRootKeyPair, SIZEOF( OEM_HAL_AES_ROOT_KEY_PAIR ) );
}

/*****************************************************************************
** Function:    _ZeroEcc256KeyPair
**
** Synopsis:    Initialize the key pair structure to zero.
**
** Arguments:   [f_pKeyPair]  : The key pair
**
******************************************************************************/
static DRM_FRE_INLINE DRM_VOID _ZeroEcc256KeyPair(
    __inout_ecount(1)                               OEM_HAL_ECC256_KEY_PAIR             *f_pKeyPair ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_VOID _ZeroEcc256KeyPair(
    __inout_ecount(1)                               OEM_HAL_ECC256_KEY_PAIR             *f_pKeyPair )
{
    DRMASSERT( f_pKeyPair != NULL );
    __analysis_assume( f_pKeyPair != NULL );

    OEM_ECC_ZeroPrivateKey_P256( &f_pKeyPair->privateKey );
    OEM_ECC_ZeroPublicKey_P256( &f_pKeyPair->publicKey );
    f_pKeyPair->fPublicKeyPresent = FALSE;
}

/*****************************************************************************
** Function:    _ResetPublicKeyValidation
**
** Synopsis:    Resets a public key as invalidated
**
** Arguments:   [f_pKey]  : The key
**
******************************************************************************/
static DRM_FRE_INLINE DRM_VOID _ResetPublicKeyValidation(
    __inout_ecount(1)                               OEM_HAL_ECC256_PUBLIC_KEY           *f_pKey ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_VOID _ResetPublicKeyValidation(
    __inout_ecount(1)                               OEM_HAL_ECC256_PUBLIC_KEY           *f_pKey )
{
    DRM_BOOL fGotLock = FALSE;

    DRMASSERT( f_pKey != NULL );
    __analysis_assume( f_pKey != NULL );

    LOCK_REGISTERS( fGotLock );

    OEM_SECURE_ZERO_MEMORY( f_pKey->validationInfo.certificateDigests, SIZEOF( f_pKey->validationInfo.certificateDigests ) );
    f_pKey->validationInfo.cDigests           = 0;
    f_pKey->validationInfo.eVerificationState = eOEM_HAL_PUBKEY_NOT_VERIFIED;

    UNLOCK_REGISTERS( fGotLock );
}

/*****************************************************************************
** Function:    _ZeroEcc256PublicKey
**
** Synopsis:    Initialize the public key structure to zero.
**
** Arguments:   [f_pKey]  : The key
**
******************************************************************************/
static DRM_FRE_INLINE DRM_VOID _ZeroEcc256PublicKey(
    __inout_ecount(1)                               OEM_HAL_ECC256_PUBLIC_KEY           *f_pKey ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_VOID _ZeroEcc256PublicKey(
    __inout_ecount(1)                               OEM_HAL_ECC256_PUBLIC_KEY           *f_pKey )
{
    DRMASSERT( f_pKey != NULL );
    __analysis_assume( f_pKey != NULL );

    OEM_ECC_ZeroPublicKey_P256( &f_pKey->key );
    _ResetPublicKeyValidation( f_pKey );

    f_pKey->dwSecurityLevel             = 0;
    f_pKey->dwKeyUsageBitmap            = 0;
    f_pKey->dwFullCertKeyUsageBitmap    = 0;
    f_pKey->fRootKey                    = FALSE;
}

DRM_API DRM_VOID DRM_CALL Oem_HalOemImpl_ZeroKeyRegister(
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_keyRegisterIndex )
{
    /*
    ** Free any resources associated with this register
    ** Caller should already have called Oem_HalOemImpl_VerifyValidKeyRegister
    */
    DRMASSERT( DRM_SUCCEEDED( Oem_HalOemImpl_VerifyValidKeyRegister( f_eKeyType, f_keyRegisterIndex ) ) );
    _FreeKeyRegisterResources( f_eKeyType, f_keyRegisterIndex );

    switch( f_eKeyType )
    {
    case OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT:
        _ZeroEcc256KeyPair( &g_rgKeyRegistersEccP256ContentEncrypt[ f_keyRegisterIndex ] );
        break;

    case OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT:
        _ZeroEcc256KeyPair( &g_rgKeyRegistersEccP256DomainEncrypt[ f_keyRegisterIndex ] );
        break;

    case OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT:
        _ZeroEcc256KeyPair( &g_rgKeyRegistersEccP256MessageEncrypt[ f_keyRegisterIndex ] );
        break;

    case OEM_HAL_KEY_TYPE_ECC_P256_SIGN:
        _ZeroEcc256KeyPair( &g_rgKeyRegistersEccP256Sign[ f_keyRegisterIndex ] );
        break;

    case OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC:
        _ZeroEcc256PublicKey( &g_rgKeyRegistersEccP256Public[ f_keyRegisterIndex ] );
        break;

    case OEM_HAL_KEY_TYPE_ECC_P256_MODEL_SIGN:
        _ZeroEcc256KeyPair( &g_rgKeyRegistersEccP256ModelSign[ f_keyRegisterIndex ] );
        break;

    case OEM_HAL_KEY_TYPE_AES_ECB_ROOT:
        _ZeroAesRootKeyPair( &g_rgKeyRegistersAesEcbRoot[ f_keyRegisterIndex ] );
        break;

    case OEM_HAL_KEY_TYPE_AES_CTR_CONTENT:
        _ZeroAesKeyPair( &g_rgKeyRegistersAesCtrContent[ f_keyRegisterIndex ] );
        break;

    case OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT:
        _ZeroAesKeyPair( &g_rgKeyRegistersAesEcbMessageEncrypt[ f_keyRegisterIndex ] );
        break;

    case OEM_HAL_KEY_TYPE_AES_ECB_DOMAIN_PROTECT:
        _ZeroAesKeyPair( &g_rgKeyRegistersAesEcbDomainProtect[ f_keyRegisterIndex ] );
        break;

    case OEM_HAL_KEY_TYPE_AES_OMAC1_SECURE_STORE:
        _ZeroAesKey( &g_rgKeyRegistersAesOmac1SecureStore[ f_keyRegisterIndex ] );
        break;

    case OEM_HAL_KEY_TYPE_AES_KEYFILE:
        _ZeroAesKey( &g_rgKeyRegistersAesKeyFile[ f_keyRegisterIndex ] );
        break;

#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    case OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT:
        _ZeroAesKey( &g_rgKeyRegistersAesGenericKeyContent[ f_keyRegisterIndex ] );
        break;
#endif /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */

#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    case OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT:
        _ZeroAesKey( &g_rgKeyRegistersAesGenericKeyRoot[ f_keyRegisterIndex ] );
        break;
#endif /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */

#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    case OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT:
        _ZeroAesRootKeyPair( &g_rgKeyRegistersAesEcbNewLicenseRoot[ f_keyRegisterIndex ] );
        break;
#endif /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */

#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    case OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT:
        _ZeroAesKeyPair( &g_rgKeyRegistersAesCtrNewLicenseContent[ f_keyRegisterIndex ] );
        break;
#endif /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */

#if !DRM_SUPPORT_SECUREOEMHAL
    case OEM_HAL_KEY_TYPE_LEGACY:
        /*
        ** Note to OEMs: Cocktail handling should not be included in OEM HAL unless explicitly required.
        ** This case statement should not exist if feature flag DRM_SUPPORT_BBXLEGACY == 0.
        */
        OEM_SECURE_ZERO_MEMORY( &g_rgKeyRegistersLegacy[ f_keyRegisterIndex ], SIZEOF( g_rgKeyRegistersLegacy[ f_keyRegisterIndex ] ) );
        break;
#endif /* !DRM_SUPPORT_SECUREOEMHAL */

#if !DRM_SUPPORT_SECUREOEMHAL
    case OEM_HAL_KEY_TYPE_AES_SLK:
        /*
        ** Note to OEMs: Cocktail handling should not be included in OEM HAL unless explicitly required.
        ** This case statement should not exist if feature flag DRM_SUPPORT_BBXSLK == 0.
        */
        _ZeroAesKey( &g_rgKeyRegistersAesSlk[ f_keyRegisterIndex ] );
        break;
#endif /* !DRM_SUPPORT_SECUREOEMHAL */

    default:
        /*
        ** This should never be reached
        */
        DRMASSERT( FALSE );
    }
}

/*****************************************************************************
** Function:    Oem_HalOemImpl_AllocateDerivedRootKeyInfo
**
** Synopsis:    Allocate a register to hold derived root key information.
**
** Arguments:   [f_pRootKeyRegister]  : The key register
**              [f_pAuxKey]           : pointer to a valid Aux key structure
**
** Returns:     DRM_SUCCESS          - on success
**              DRM_E_INVALIDARG     - if invalid arguments.
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_AllocateDerivedRootKeyInfo(
    __inout_ecount(1)                               OEM_HAL_AES_ROOT_KEY_PAIR           *f_pRootKeyRegister,
    __in_ecount(1)                            const DRM_XMR_AUX_KEY                     *f_pAuxKey )
{
    DRM_RESULT dr                          = DRM_SUCCESS;
    DRM_DWORD  cAdditionalEntryAllocations = 0;
    DRM_DWORD  dwBytesToAllocate           = 0;

    ChkPtr( f_pRootKeyRegister );
    ChkPtr( f_pAuxKey );
    ChkArg( f_pRootKeyRegister->pDerivedKey == NULL );
    ChkArg( f_pAuxKey->fValid );

    /*
    ** Additional buffer space needs to be allocated beyond OEM_HAL_DERIVED_ROOT_KEY.rgAuxKeyEntries
    ** to hold the aux key entries
    */
    if( f_pAuxKey->cEntries > 0 )
    {
        cAdditionalEntryAllocations = (DRM_DWORD)( f_pAuxKey->cEntries - 1 );
    }

    dwBytesToAllocate = SIZEOF( OEM_HAL_DERIVED_ROOT_KEY )
                      + ( cAdditionalEntryAllocations * SIZEOF( DRM_XMR_AUX_KEY_ENTRY ) );

    ChkMem( f_pRootKeyRegister->pDerivedKey = ( OEM_HAL_DERIVED_ROOT_KEY* )Oem_MemAlloc( dwBytesToAllocate ) );

    OEM_SECURE_ZERO_MEMORY( f_pRootKeyRegister->pDerivedKey, dwBytesToAllocate );

    /*
    ** Copy the aux key across, allocating a memory buffer to hold the keys themselves
    */
    f_pRootKeyRegister->pDerivedKey->AuxKey.cEntries = f_pAuxKey->cEntries;
    f_pRootKeyRegister->pDerivedKey->AuxKey.fValid   = TRUE;

    /*
    ** Fix up the entries pointer
    */
    f_pRootKeyRegister->pDerivedKey->AuxKey.pEntries = f_pRootKeyRegister->pDerivedKey->rgAuxKeyEntries;

    OEM_SECURE_MEMCPY( f_pRootKeyRegister->pDerivedKey->AuxKey.pEntries, f_pAuxKey->pEntries, SIZEOF( DRM_XMR_AUX_KEY_ENTRY ) * f_pAuxKey->cEntries );

ErrorExit:
    if( DRM_FAILED( dr )
     && f_pRootKeyRegister != NULL )
    {
        _FreeDerivedRootKeyInfo( f_pRootKeyRegister );
    }

    return dr;
}

/*****************************************************************************
** Function:    _CheckAesKeyExpanded
**
** Synopsis:    Expands an AES key pair if the key data hasn't been expanded yet.
**
** Arguments:   [f_pbKeyMaterial] : The unexpanded key material
**              [f_pExpandedKey]  : The expanded key
**
** Returns:     DRM_SUCCESS          - on success
**              DRM_E_INVALIDARG     - if invalid arguments.
**
** Notes:       None.
**
******************************************************************************/
static DRM_FRE_INLINE DRM_RESULT _CheckAesKeyExpanded(
    __in_bcount( DRM_AES_KEYSIZE_128 )        const DRM_BYTE                            *f_pbKeyMaterial,
    __inout_ecount(1)                               OEM_HAL_EXPANDED_AES_KEY            *f_pExpandedKey ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_RESULT _CheckAesKeyExpanded(
    __in_bcount( DRM_AES_KEYSIZE_128 )        const DRM_BYTE                            *f_pbKeyMaterial,
    __inout_ecount(1)                               OEM_HAL_EXPANDED_AES_KEY            *f_pExpandedKey )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkPtr( f_pbKeyMaterial );
    ChkPtr( f_pExpandedKey );

    if( !f_pExpandedKey->fExpanded )
    {

        ChkDR( Oem_Aes_SetKey( f_pbKeyMaterial, &f_pExpandedKey->rgExpandedKey ) );
        f_pExpandedKey->fExpanded = TRUE;
    }

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    _GetOmacSigningKey
**
** Synopsis:    Get the content integrity key from a register.
**
** Arguments:   [f_eKeyType]        : The key type
**              [f_indexKey]        : The key index
**              [f_fCreateSignature]: TRUE indicates the key will be used for signature creation
**                                    FALSE indicates the key will be used for signature validation
**              [f_ppSigningKey]    : return a pointer to the expanded signing key
**
** Returns:     DRM_SUCCESS                       - on success
**              DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE - if the register has no CI key
**
** Notes:       Assumes that key type and index have already been validated.
**
******************************************************************************/
static DRM_FRE_INLINE DRM_RESULT _GetOmacSigningKey(
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_indexKey,
    __in                                            DRM_BOOL                             f_fCreateSignature,
    __out_ecount(1)                                 DRM_AES_KEY                        **f_ppSigningKey,
    __out_opt                                       DRM_DWORD                           *f_pdwSecurityLevelRequiredToWrapThisKey,
    __out_opt                                       DRM_DWORD                           *f_pdwRIVRequiredToWrapThisKey ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_RESULT _GetOmacSigningKey(
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_indexKey,
    __in                                            DRM_BOOL                             f_fCreateSignature,
    __out_ecount(1)                                 DRM_AES_KEY                        **f_ppSigningKey,
    __out_opt                                       DRM_DWORD                           *f_pdwSecurityLevelRequiredToWrapThisKey,
    __out_opt                                       DRM_DWORD                           *f_pdwRIVRequiredToWrapThisKey )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  dwUnused;

    ChkPtr( f_ppSigningKey );

    /*
    ** Initialize the pointers to a local if not specified so we can dereference
    ** them without having if checks all over the place.
    ** We can point them to the same local because we are ignoring the data written
    ** if the output parameters are NULL.
    */
    if( f_pdwSecurityLevelRequiredToWrapThisKey == NULL )
    {
        f_pdwSecurityLevelRequiredToWrapThisKey = &dwUnused;
    }
    if( f_pdwRIVRequiredToWrapThisKey == NULL )
    {
        f_pdwRIVRequiredToWrapThisKey = &dwUnused;
    }

    *f_pdwSecurityLevelRequiredToWrapThisKey = 0;
    *f_pdwRIVRequiredToWrapThisKey           = 0;

    /*
    ** Expand (if necessary) and assign the appropriate AES128 signing key or integrity key
    */
    switch( f_eKeyType )
    {
    case OEM_HAL_KEY_TYPE_AES_KEYFILE:

        /* No need to check if keyfile key is expanded - keyfile keys are always created directly in their expanded form */
        *f_ppSigningKey = &g_rgKeyRegistersAesKeyFile[ f_indexKey ].expandedKey.rgExpandedKey;
        break;

    case OEM_HAL_KEY_TYPE_AES_ECB_ROOT:

        /* Can both create new and verify existing signatures with an existing root license key. (Create during rebind, verify during bind.) */

#if DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
        ChkBOOL( !f_fCreateSignature, DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
#endif /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */

        ChkDR( _CheckAesKeyExpanded(
            &g_rgKeyRegistersAesEcbRoot[f_indexKey].keyPair.rgbKeys[OEM_HAL_KEY_PAIR_CI_INDEX][0],
            &g_rgKeyRegistersAesEcbRoot[f_indexKey].keyPair.rgExpandedKeys[OEM_HAL_KEY_PAIR_CI_INDEX] ) );

        *f_ppSigningKey = &g_rgKeyRegistersAesEcbRoot[f_indexKey].keyPair.rgExpandedKeys[OEM_HAL_KEY_PAIR_CI_INDEX].rgExpandedKey;
        *f_pdwSecurityLevelRequiredToWrapThisKey = g_rgKeyRegistersAesEcbRoot[f_indexKey].keyPair.dwSecurityLevelRequiredToWrapThisKey;
        *f_pdwRIVRequiredToWrapThisKey           = g_rgKeyRegistersAesEcbRoot[f_indexKey].keyPair.dwRIVRequiredToWrapThisKey;
        break;

#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    case OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT:

        /* Can only create new signatures with a local root license key. */
        ChkBOOL( f_fCreateSignature, DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );

        ChkDR( _CheckAesKeyExpanded(
            &g_rgKeyRegistersAesEcbNewLicenseRoot[f_indexKey].keyPair.rgbKeys[OEM_HAL_KEY_PAIR_CI_INDEX][0],
            &g_rgKeyRegistersAesEcbNewLicenseRoot[f_indexKey].keyPair.rgExpandedKeys[OEM_HAL_KEY_PAIR_CI_INDEX] ) );

        *f_ppSigningKey = &g_rgKeyRegistersAesEcbNewLicenseRoot[f_indexKey].keyPair.rgExpandedKeys[OEM_HAL_KEY_PAIR_CI_INDEX].rgExpandedKey;
        *f_pdwSecurityLevelRequiredToWrapThisKey = g_rgKeyRegistersAesEcbNewLicenseRoot[f_indexKey].keyPair.dwSecurityLevelRequiredToWrapThisKey;
        *f_pdwRIVRequiredToWrapThisKey           = g_rgKeyRegistersAesEcbNewLicenseRoot[f_indexKey].keyPair.dwRIVRequiredToWrapThisKey;
        break;
#endif /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */

    case OEM_HAL_KEY_TYPE_AES_CTR_CONTENT:

        /* Can both create new and verify existing signatures with an existing leaf/simple license key. (Create during rebind, verify during bind.) */

#if DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
        ChkBOOL( !f_fCreateSignature, DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
#endif /* DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */

        ChkDR( _CheckAesKeyExpanded(
            &g_rgKeyRegistersAesCtrContent[f_indexKey].rgbKeys[OEM_HAL_KEY_PAIR_CI_INDEX][0],
            &g_rgKeyRegistersAesCtrContent[f_indexKey].rgExpandedKeys[OEM_HAL_KEY_PAIR_CI_INDEX] ) );

        *f_ppSigningKey = &g_rgKeyRegistersAesCtrContent[f_indexKey].rgExpandedKeys[OEM_HAL_KEY_PAIR_CI_INDEX].rgExpandedKey;
        *f_pdwSecurityLevelRequiredToWrapThisKey = g_rgKeyRegistersAesCtrContent[f_indexKey].dwSecurityLevelRequiredToWrapThisKey;
        *f_pdwRIVRequiredToWrapThisKey           = g_rgKeyRegistersAesCtrContent[f_indexKey].dwRIVRequiredToWrapThisKey;
        break;

#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    case OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT:

        /* Can only create new signatures with a local leaf/simple license key. */
        ChkBOOL( f_fCreateSignature, DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );

        ChkDR( _CheckAesKeyExpanded(
            &g_rgKeyRegistersAesCtrNewLicenseContent[f_indexKey].rgbKeys[OEM_HAL_KEY_PAIR_CI_INDEX][0],
            &g_rgKeyRegistersAesCtrNewLicenseContent[f_indexKey].rgExpandedKeys[OEM_HAL_KEY_PAIR_CI_INDEX] ) );

        *f_ppSigningKey = &g_rgKeyRegistersAesCtrNewLicenseContent[f_indexKey].rgExpandedKeys[OEM_HAL_KEY_PAIR_CI_INDEX].rgExpandedKey;
        *f_pdwSecurityLevelRequiredToWrapThisKey = g_rgKeyRegistersAesCtrNewLicenseContent[f_indexKey].dwSecurityLevelRequiredToWrapThisKey;
        *f_pdwRIVRequiredToWrapThisKey           = g_rgKeyRegistersAesCtrNewLicenseContent[f_indexKey].dwRIVRequiredToWrapThisKey;
        break;
#endif /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */

    case OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT:

        ChkDR( _CheckAesKeyExpanded(
            &g_rgKeyRegistersAesEcbMessageEncrypt[f_indexKey].rgbKeys[OEM_HAL_KEY_PAIR_CI_INDEX][0],
            &g_rgKeyRegistersAesEcbMessageEncrypt[f_indexKey].rgExpandedKeys[OEM_HAL_KEY_PAIR_CI_INDEX] ) );

        *f_ppSigningKey = &g_rgKeyRegistersAesEcbMessageEncrypt[f_indexKey].rgExpandedKeys[OEM_HAL_KEY_PAIR_CI_INDEX].rgExpandedKey;
        break;

    case OEM_HAL_KEY_TYPE_AES_OMAC1_SECURE_STORE:

        ChkDR( _CheckAesKeyExpanded(
            g_rgKeyRegistersAesOmac1SecureStore[f_indexKey].rgbKey,
            &g_rgKeyRegistersAesOmac1SecureStore[f_indexKey].expandedKey ) );

        *f_ppSigningKey = &g_rgKeyRegistersAesOmac1SecureStore[f_indexKey].expandedKey.rgExpandedKey;
        break;

#if !DRM_SUPPORT_SECUREOEMHAL
    case OEM_HAL_KEY_TYPE_LEGACY:
        /*
        ** Note to OEMs: Cocktail handling should not be included in OEM HAL unless explicitly required.
        ** This case statement should not exist if DRM_SUPPORT_BBXLEGACY == 0.
        */

        ChkDR( _CheckAesKeyExpanded(
            &g_rgKeyRegistersLegacy[f_indexKey].rgbKeyBuffer[0],
            &g_rgKeyRegistersLegacy[f_indexKey].integrityKey ) );

        *f_ppSigningKey = &g_rgKeyRegistersLegacy[f_indexKey].integrityKey.rgExpandedKey;
        break;
#endif /* !DRM_SUPPORT_SECUREOEMHAL */

    default:
        ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
        break;
    }

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_VerifyOMAC1Signature(
    __in_bcount( f_cbData )                   const DRM_BYTE                            *f_rgbData,
    __in                                            DRM_DWORD                            f_cbData,
    __in_bcount( f_cbSignature )              const DRM_BYTE                            *f_rgbSignature,
    __in                                            DRM_DWORD                            f_cbSignature,
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_indexKey )
{
    DRM_RESULT   dr          = DRM_SUCCESS;
    DRM_AES_KEY *pSigningKey = NULL;

    ChkDR( Oem_HalOemImpl_VerifyHalInitialized() );
    ChkDR( Oem_HalOemImpl_VerifyValidKeyRegister( f_eKeyType, f_indexKey ) );
    ChkPtr( f_rgbData );
    ChkPtr( f_rgbSignature );
    ChkArg( f_cbSignature == DRM_AES_BLOCKLEN );

    ChkDR( _GetOmacSigningKey( f_eKeyType, f_indexKey, FALSE, &pSigningKey, NULL, NULL ) );

    DRMASSERT( pSigningKey != NULL );

    ChkDR( Oem_Omac1_Verify( pSigningKey, f_rgbData, 0, f_cbData, f_rgbSignature, 0 ) );

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_CreateOMAC1Signature(
    __in_bcount( f_cbData )                   const DRM_BYTE                            *f_rgbData,
    __in                                            DRM_DWORD                            f_cbData,
    __out_bcount_opt( *f_pcbSignature )             DRM_BYTE                            *f_rgbSignature,
    __inout                                         DRM_DWORD                           *f_pcbSignature,
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_indexKey,
    __in                                            DRM_DWORD                            f_dwRequestedSecurityLevel,
    __in                                            DRM_DWORD                            f_dwRequestedRIV )
{
    DRM_RESULT   dr                                     = DRM_SUCCESS;
    DRM_AES_KEY *pSigningKey                            = NULL;
    DRM_DWORD    dwSecurityLevelRequiredToWrapThisKey   = MAX_UNSIGNED_TYPE(DRM_DWORD);
    DRM_DWORD    dwRIVRequiredToWrapThisKey             = MAX_UNSIGNED_TYPE(DRM_DWORD);

    ChkDR( Oem_HalOemImpl_VerifyHalInitialized() );
    ChkDR( Oem_HalOemImpl_VerifyValidKeyRegister( f_eKeyType, f_indexKey ) );
    ChkPtr( f_rgbData );
    ChkPtr( f_rgbSignature );
    ChkPtr( f_pcbSignature );

    if( *f_pcbSignature < DRM_AES_BLOCKLEN )
    {
        *f_pcbSignature = DRM_AES_BLOCKLEN;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /*
    ** _GetOmacSigningKey will verify that when SecureHalPlayOnly is enabled:
    ** 1. You can't create signatures with existing license keys and
    ** 2. You can't create signatures for new licenses.
    */
    ChkDR( _GetOmacSigningKey( f_eKeyType, f_indexKey, TRUE, &pSigningKey, &dwSecurityLevelRequiredToWrapThisKey, &dwRIVRequiredToWrapThisKey ) );

    /*
    ** You can't create an OMAC1 signature for a lower security level license using a higher security level license's key.
    ** Otherwise, an attacker could create an SL150 license containing the key material from an SL2000 license, sign it with
    ** the SL2000 license's key, and then rebind it to an SL150 certificate for which you have the private key thus
    ** granting you access to the SL2000 license's content key.
    */
    ChkBOOL( dwSecurityLevelRequiredToWrapThisKey <= f_dwRequestedSecurityLevel, DRM_E_DEVICE_SECURITY_LEVEL_TOO_LOW );

    /*
    ** You can't create an OMAC1 signature for a license with a lower RIV required using a license's key
    ** that came from a license with a higher RIV required.  Otherwise, an attacker could create an SL2000 license
    ** containing the key material from another SL2000 license but with a lower required RIV, sign it with the original
    ** SL2000 license's key, reinitialize the HAL with a lower RIV, and rebind the new license to a revoked certificate.
    */
    ChkBOOL( dwRIVRequiredToWrapThisKey <= f_dwRequestedRIV, DRM_E_RIV_TOO_SMALL );

    DRMASSERT( pSigningKey != NULL );

    ChkDR( Oem_Omac1_Sign( pSigningKey, f_rgbData, 0, f_cbData, f_rgbSignature ) );

    *f_pcbSignature = DRM_AES_BLOCKLEN;

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_EncryptDataBlock(
    __in_bcount( f_cbData )                   const DRM_BYTE                            *f_rgbBlock,
    __in                                            DRM_DWORD                            f_cbBlock,
    __out_bcount_opt( *f_pcbEncryptedBlock )        DRM_BYTE                            *f_rgbEncryptedBlock,
    __inout                                         DRM_DWORD                           *f_pcbEncryptedBlock,
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_indexMessageEncryptKey )
{
    DRM_RESULT            dr          = DRM_SUCCESS;
    OEM_HAL_AES_KEY_PAIR *pMessageKey = NULL;

    ChkDR( Oem_HalOemImpl_VerifyHalInitialized() );
    ChkDR( Oem_HalOemImpl_VerifyValidKeyRegister( f_eKeyType, f_indexMessageEncryptKey ) );
    ChkPtr( f_rgbBlock );
    ChkPtr( f_rgbEncryptedBlock );
    ChkPtr( f_pcbEncryptedBlock );
    ChkArg( f_cbBlock == DRM_AES_BLOCKLEN );

    if( *f_pcbEncryptedBlock < DRM_AES_BLOCKLEN )
    {
        *f_pcbEncryptedBlock = DRM_AES_BLOCKLEN;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    switch( f_eKeyType )
    {
    case OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT:
        /* Scenario: Encrypt nonce */
        pMessageKey = &g_rgKeyRegistersAesEcbMessageEncrypt[f_indexMessageEncryptKey];
        break;

#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    case OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT:
        /* Scenario: Calculate checksum */
        pMessageKey = &g_rgKeyRegistersAesEcbNewLicenseRoot[f_indexMessageEncryptKey].keyPair;
        break;
#endif /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */

    default:
        ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
        break;
    }

    OEM_SECURE_MEMCPY( f_rgbEncryptedBlock, f_rgbBlock, DRM_AES_BLOCKLEN );

    ChkDR( _CheckAesKeyExpanded(
        &pMessageKey->rgbKeys[OEM_HAL_KEY_PAIR_CK_INDEX][0],
        &pMessageKey->rgExpandedKeys[OEM_HAL_KEY_PAIR_CK_INDEX] ) );

    ChkDR( Oem_Aes_EcbEncryptData(
        &pMessageKey->rgExpandedKeys[OEM_HAL_KEY_PAIR_CK_INDEX].rgExpandedKey,
        f_rgbEncryptedBlock,
        DRM_AES_BLOCKLEN ) );

    *f_pcbEncryptedBlock = DRM_AES_BLOCKLEN;

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    Oem_HalOemImpl_GenerateEcc256KeyPair
**
** Synopsis:    Generate a random, valid, ECC256 public/private key pair.
**
** Arguments:   [f_pContext] : The crypto context
**              [f_eKeyType] : The key type to generate
**              [f_indexKey] : The key index
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG - if the register is not an ECC256 pair
**
** Notes:       Assumes that key type and index have already been validated.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_GenerateEcc256KeyPair(
    __inout_ecount( 1 )                             DRM_CRYPTO_CONTEXT                  *f_pContext,
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_indexKey )
{
    DRM_RESULT               dr       = DRM_SUCCESS;
    OEM_HAL_ECC256_KEY_PAIR *pKeyPair = NULL;

    /*
    ** Caller should already have called Oem_HalOemImpl_VerifyValidKeyRegister
    */
    DRMASSERT( DRM_SUCCEEDED( Oem_HalOemImpl_VerifyValidKeyRegister( f_eKeyType, f_indexKey ) ) );

    switch( f_eKeyType )
    {
    case OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT:
        pKeyPair = &g_rgKeyRegistersEccP256ContentEncrypt[f_indexKey];
        break;

    case OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT:
        pKeyPair = &g_rgKeyRegistersEccP256MessageEncrypt[f_indexKey];
        break;

    case OEM_HAL_KEY_TYPE_ECC_P256_SIGN:
        pKeyPair = &g_rgKeyRegistersEccP256Sign[f_indexKey];
        break;

    default:
        ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
    }

    /*
    ** Always generate ECC-p256 encryptable keys: the 'restricted' set
    */
    ChkDR( OEM_ECC_GenKeyPairRestrictedPriv_P256( &pKeyPair->publicKey, &pKeyPair->privateKey, ( struct bigctx_t* )f_pContext ) );

    pKeyPair->fPublicKeyPresent = TRUE;

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    Oem_HalOemImpl_GenerateAes128Key
**
** Synopsis:    Generate a random AES128 key.
**
** Arguments:   [f_pOEMContext] : The OEM context pointer
**              [f_eKeyType]    : The key type to generate
**              [f_indexKey]    : The key index
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG - if the register is not an AES key
**
** Notes:       Assumes that key type and index have already been validated.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_GenerateAes128Key(
    __in                                            DRM_VOID                            *f_pOEMContext,
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_indexKey )
{
    DRM_RESULT       dr   = DRM_SUCCESS;
    OEM_HAL_AES_KEY *pKey = NULL;

    /*
    ** Caller should already have called Oem_HalOemImpl_VerifyValidKeyRegister
    */
    DRMASSERT( DRM_SUCCEEDED( Oem_HalOemImpl_VerifyValidKeyRegister( f_eKeyType, f_indexKey ) ) );

    switch( f_eKeyType )
    {
    case OEM_HAL_KEY_TYPE_AES_OMAC1_SECURE_STORE:
        pKey = &g_rgKeyRegistersAesOmac1SecureStore[f_indexKey];
        break;

#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    case OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT:
        pKey = &g_rgKeyRegistersAesGenericKeyContent[f_indexKey];
        break;
#endif /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */

#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    case OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT:
        pKey = &g_rgKeyRegistersAesGenericKeyRoot[f_indexKey];
        break;
#endif /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */

    default:
        ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
    }

    ChkDR( Oem_Random_GetBytes( f_pOEMContext, pKey->rgbKey, SIZEOF( pKey->rgbKey ) ) );

ErrorExit:
    return dr;
}

static DRM_FRE_INLINE DRM_RESULT _GenerateAes128KeyPairEncryptableWithEcc256(
    __in                                            DRM_VOID                            *f_pOEMContext,
    __inout_ecount( 1 )                             DRM_CRYPTO_CONTEXT                  *f_pCryptoContext,
    __out_ecount(ECC_P256_PLAINTEXT_SIZE_IN_BYTES)  DRM_BYTE                             f_rgbKeyPair[ECC_P256_PLAINTEXT_SIZE_IN_BYTES] ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_RESULT _GenerateAes128KeyPairEncryptableWithEcc256(
    __in                                            DRM_VOID                            *f_pOEMContext,
    __inout_ecount( 1 )                             DRM_CRYPTO_CONTEXT                  *f_pCryptoContext,
    __out_ecount(ECC_P256_PLAINTEXT_SIZE_IN_BYTES)  DRM_BYTE                             f_rgbKeyPair[ECC_P256_PLAINTEXT_SIZE_IN_BYTES] )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    /*
    ** Randomly generate an ECC P256 encryptable pair of AES keys
    ** There is ~.01% change that this will fail (the entropy may fail to give a legit value), hence the loop.
    ** Any other error should fall out of the loop and be caught by the ChkDR below.
    ** See OEM_ECC_GenerateHMACKey_P256 declaration for more info on this failure case.
    */
    DRMCASSERT( SIZEOF( PLAINTEXT_P256 ) == DRM_AES_KEYSIZE_128 * 2          );
    DRMCASSERT( SIZEOF( PLAINTEXT_P256 ) == DRM_AES_KEYSIZE_128_X2           );
    DRMCASSERT( SIZEOF( PLAINTEXT_P256 ) == ECC_P256_PLAINTEXT_SIZE_IN_BYTES );

    do
    {
        ChkDR( Oem_Random_GetBytes(
            f_pOEMContext,
            f_rgbKeyPair,
            ECC_P256_PLAINTEXT_SIZE_IN_BYTES ) );

        dr = OEM_ECC_GenerateHMACKey_P256(
            ( PLAINTEXT_P256* )f_rgbKeyPair,
            ( struct bigctx_t* )f_pCryptoContext->rgbCryptoContext );

    } while( dr == DRM_E_P256_HMAC_KEYGEN_FAILURE );

    ChkDR( dr );
ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    Oem_HalOemImpl_GenerateAes128KeyPair
**
** Synopsis:    Generate a random AES128 key pair, ensuring it can be encrypted
**              by an ECC256 private key, specifically NOT for new license creation.
**
** Arguments:   [f_pOEMContext]    : the OEM context pointer
**              [f_pCryptoContext] : The crypto context
**              [f_eKeyType]       : The key type to generate
**              [f_indexKey]       : The key index
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG - if the register is not an AES key pair
**
** Notes:       Assumes that key type and index have already been validated.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_GenerateAes128KeyPair(
    __in                                            DRM_VOID                            *f_pOEMContext,
    __inout_ecount( 1 )                             DRM_CRYPTO_CONTEXT                  *f_pCryptoContext,
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_indexKey )
{
    DRM_RESULT            dr    = DRM_SUCCESS;
    OEM_HAL_AES_KEY_PAIR *pKeys = NULL;

    /*
    ** Caller should already have called Oem_HalOemImpl_VerifyValidKeyRegister
    */
    DRMASSERT( DRM_SUCCEEDED( Oem_HalOemImpl_VerifyValidKeyRegister( f_eKeyType, f_indexKey ) ) );

    switch( f_eKeyType )
    {
#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    case OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT:
        pKeys = &g_rgKeyRegistersAesEcbMessageEncrypt[f_indexKey];
        break;
#endif /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */

    default:
        ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
    }

    ChkDR( _GenerateAes128KeyPairEncryptableWithEcc256( f_pOEMContext, f_pCryptoContext, &pKeys->rgbKeys[0][0] ) );

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    Oem_HalOemImpl_GenerateAes128NewLicenseKeyPair
**
** Synopsis:    Generate a random AES128 key pair, ensuring it can be encrypted
**              by an ECC256 private key, specifically for new license creation.
**
** Arguments:   [f_pOEMContext]             : the OEM context pointer
**              [f_pCryptoContext]          : The crypto context
**              [f_eKeyType]                : The key type to generate
**              [f_indexKey]                : The key index
**              [f_dwRequestedSecurityLevel]: The requested security level of the key.
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG - if the register is not a new license AES key pair
**
** Notes:       Assumes that key type and index have already been validated.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_GenerateAes128NewLicenseKeyPair(
    __in                                            DRM_VOID                            *f_pOEMContext,
    __inout_ecount( 1 )                             DRM_CRYPTO_CONTEXT                  *f_pCryptoContext,
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_indexKey,
    __in                                            DRM_DWORD                            f_dwRequestedSecurityLevel )
{
    DRM_RESULT            dr    = DRM_SUCCESS;
    OEM_HAL_AES_KEY_PAIR *pKeys = NULL;

    /*
    ** Caller should already have called Oem_HalOemImpl_VerifyValidKeyRegister
    */
    DRMASSERT( DRM_SUCCEEDED( Oem_HalOemImpl_VerifyValidKeyRegister( f_eKeyType, f_indexKey ) ) );

    switch( f_eKeyType )
    {
#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    case OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT:
        pKeys = &g_rgKeyRegistersAesEcbNewLicenseRoot[f_indexKey].keyPair;
        break;
#endif /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */

#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    case OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT:
        pKeys = &g_rgKeyRegistersAesCtrNewLicenseContent[f_indexKey];
        break;
#endif /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */

    default:
        ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
    }

    /*
    ** You can only generate a key with a security level GREATER THAN OR EQUAL to the fixed HAL level.
    ** This prevents the generation of SL 150 keys (which could then be bound to SL 150 certificates) on an SL 2000 device,
    **  but it does not prevent generation of SL 2000 keys (which could only be bound to SL 2000 certificates) on an SL 150 device.
    ** In other words, you can't create a test license on a real device because that would allow an attacker above the HAL
    **  to trick a production device into generating and binding a license to a test certificate.
    ** You can create a real license on a test device because you can only bind that license to a real device,
    **  i.e. it further restricts what you can do with that license compared to what you could do if you generated an SL 150 license.
    */
    ChkBOOL( f_dwRequestedSecurityLevel >= OEM_KEY_GENERATION_SECURITY_LEVEL, DRM_E_DEVICE_SECURITY_LEVEL_TOO_LOW );

    ChkDR( _GenerateAes128KeyPairEncryptableWithEcc256( f_pOEMContext, f_pCryptoContext, &pKeys->rgbKeys[0][0] ) );

    pKeys->dwSecurityLevelRequiredToWrapThisKey = f_dwRequestedSecurityLevel;
    pKeys->dwRIVRequiredToWrapThisKey           = g_RevocationInfo.dwStoredRIV;

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_AllocateKeyRegister(
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in                                            DRM_BOOL                             f_fPreloaded,
    __out_ecount( 1 )                               OEM_HAL_KEY_REGISTER_INDEX          *f_pKeyRegisterIndex )
{
    DRM_RESULT                    dr       = DRM_SUCCESS;
    DRM_BOOL                      fGotLock = FALSE;
    OEM_HAL_REGISTER_STATE_TABLE *pTable   = NULL;
    OEM_HAL_KEY_REGISTER_INDEX    iState   = 0;

    ChkDR( Oem_HalOemImpl_VerifyHalInitialized() );

    ChkPtr( f_pKeyRegisterIndex );

    LOCK_REGISTERS( fGotLock );
    *f_pKeyRegisterIndex = OEM_HAL_KEY_REGISTER_INDEX_INVALID;

    /*
    ** Look for an unallocated register of the requested type
    */
    ChkDR( _GetKeyRegisterStateTable( f_eKeyType, &pTable ));

    for( iState = 0; iState < pTable->cState && *f_pKeyRegisterIndex == OEM_HAL_KEY_REGISTER_INDEX_INVALID; iState++ )
    {
        if( pTable->pfState[iState] == eOEM_HAL_KEY_REGISTER_STATE_UNALLOCATED )
        {
            if( f_fPreloaded )
            {
                pTable->pfState[iState] = eOEM_HAL_KEY_REGISTER_STATE_PRELOADED;
            }
            else
            {
                pTable->pfState[iState] = eOEM_HAL_KEY_REGISTER_STATE_ALLOCATED;
            }

            *f_pKeyRegisterIndex = iState;
        }
    }

    if( *f_pKeyRegisterIndex == OEM_HAL_KEY_REGISTER_INDEX_INVALID )
    {
        /*
        ** No free register was found
        */
        ChkDR( DRM_E_OEMHAL_OUT_OF_KEY_REGISTERS );
    }

#ifdef HWDECODING
    if(f_eKeyType == OEM_HAL_KEY_TYPE_AES_CTR_CONTENT && Decryptor_CTX == NULL)
    {
	    Decryptor_CTX = BCM_DECRYPTOR_SOC_Create(OEM_DECRYPTOR_TYPE_AES_128_CTR);
	    if(Decryptor_CTX == NULL)
		{
			printf("%s(%d) Err> Decryptor Creation Fail IN PLDY !\n",__FUNCTION__,__LINE__);
			return DRM_E_HANDLE;
		}
	}
#endif
#ifndef HWDECODING
	printf("@#@#@#@#@#@#@#@ PRDY #SWDECODING @ #@#@#@#@#@#@#@#\n");
#endif
ErrorExit:
    UNLOCK_REGISTERS( fGotLock );
    return dr;
}

static DRM_RESULT DRM_FRE_INLINE _ValidateEncryptionContext(
    __in                                            OEM_ENCRYPTION_HANDLE                 f_hEncryptionContext,
    __out_opt                                       OEM_HAL_ENCRYPTION_CONTEXT_INTERNAL **f_ppContext ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_RESULT DRM_FRE_INLINE _ValidateEncryptionContext(
    __in                                            OEM_ENCRYPTION_HANDLE                 f_hEncryptionContext,
    __out_opt                                       OEM_HAL_ENCRYPTION_CONTEXT_INTERNAL **f_ppContext )
{
    if( f_hEncryptionContext != OEM_ENCRYPTION_HANDLE_INVALID )
    {
              OEM_HAL_ENCRYPTION_CONTEXT_INTERNAL *pUnsafeEntry = ( OEM_HAL_ENCRYPTION_CONTEXT_INTERNAL* )f_hEncryptionContext;
        const OEM_HAL_ENCRYPTION_CONTEXT_INTERNAL *pMin         = &g_rgEncryptionContexts[ 0 ];
        const OEM_HAL_ENCRYPTION_CONTEXT_INTERNAL *pMax         = &g_rgEncryptionContexts[ OEM_HAL_NUM_ENCRYPTION_CONTEXTS - 1 ];

        /*
        ** Check the location is within range
        */
        if( pUnsafeEntry >= pMin && pUnsafeEntry <= pMax )
        {
            /*
            ** If the location is in range, check it aligns with a valid entry
            */
            DRM_SIZE_T offset = ( DRM_DWORD_PTR )( DRM_BYTE* )pUnsafeEntry - ( DRM_DWORD_PTR )( DRM_BYTE* )pMin;

            if( ( offset % SIZEOF( OEM_HAL_ENCRYPTION_CONTEXT_INTERNAL ) ) == 0
             && ( ( OEM_HAL_ENCRYPTION_CONTEXT_INTERNAL* )f_hEncryptionContext )->fInUse )
            {
                if( f_ppContext != NULL )
                {
                    /*
                    ** Convert the handle into a data structure
                    */
                    *f_ppContext = ( OEM_HAL_ENCRYPTION_CONTEXT_INTERNAL* )f_hEncryptionContext;
                }
                return DRM_SUCCESS;
            }
        }
    }

    return DRM_E_HANDLE;
}

typedef enum
{
    eOEM_OPAQUE_BUFFER_HANDLE_INTERNAL_TYPE_INVALID             = 0,
    eOEM_OPAQUE_BUFFER_HANDLE_INTERNAL_TYPE_CLEAR_DATA_SOURCE   = 1,    /* Input to encrypt (clear source about to be encrypted) */
    eOEM_OPAQUE_BUFFER_HANDLE_INTERNAL_TYPE_ENCRYPTED_DATA      = 2,    /* Output from encrypt (clear source after encryption -> encrypted) OR Input to decrypt (encrypted source about to be decrypted) */
    eOEM_OPAQUE_BUFFER_HANDLE_INTERNAL_TYPE_CLEAR_DATA_RENDER   = 3,    /* Output from decrypt (encrypted source after decryption -> clear) */
} OEM_OPAQUE_BUFFER_HANDLE_INTERNAL_TYPE;

typedef struct __tag_OEM_OPAQUE_BUFFER_HANDLE_INTERNAL
{
    OEM_OPAQUE_BUFFER_HANDLE_INTERNAL_TYPE   eType;
    DRM_BYTE                                *pbData;
    DRM_DWORD                                cbData;
} OEM_OPAQUE_BUFFER_HANDLE_INTERNAL;

/*****************************************************************************
** Function:    _EncryptSecureSampleBuffer
**
** Synopsis:    Encrypt a secure sample buffer.
**
** Arguments:   [f_pCounterContext]       : an initialized AES counter context
**              [f_pKey]                  : the AES encryption key to use
**              [f_eSampleEncryptionMode] : the sample encryption mode
**              [f_hInBuf]                : the input buffer handle
**              [f_hOutBuf]               : the output buffer handle
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_POINTER    - if the parameter pointers are NULL
**              DRM_E_INVALIDARG - if the parameter data is invalid
**              DRM_E_OEMHAL_SAMPLE_ENCRYPTION_MODE_NOT_PERMITTED -
**                  the encryption mode is not supported
**              DRM_E_NOTIMPL    - if f_pBufferChain requires features that aren't
**                                 currently implemented
**
** Notes:
**
******************************************************************************/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: When implemented parameters should not be const")
static DRM_RESULT DRM_FRE_INLINE _EncryptSecureSampleBuffer(
    __inout_ecount(1)                               DRM_AES_COUNTER_MODE_CONTEXT        *f_pCounterContext,
    __in_ecount( 1 )                          const DRM_AES_KEY                         *f_pKey,
    __in                                            OEM_HAL_SAMPLE_ENCRYPTION_MODE       f_eSampleEncryptionMode,
    __in                                            OEM_OPAQUE_BUFFER_HANDLE             f_hInBuf,
    __in                                            OEM_OPAQUE_BUFFER_HANDLE             f_hOutBuf,
    __in                                            DRM_DWORD                            f_cbData ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_RESULT DRM_FRE_INLINE _EncryptSecureSampleBuffer(
    __inout_ecount(1)                               DRM_AES_COUNTER_MODE_CONTEXT        *f_pCounterContext,
    __in_ecount( 1 )                          const DRM_AES_KEY                         *f_pKey,
    __in                                            OEM_HAL_SAMPLE_ENCRYPTION_MODE       f_eSampleEncryptionMode,
    __in                                            OEM_OPAQUE_BUFFER_HANDLE             f_hInBuf,
    __in                                            OEM_OPAQUE_BUFFER_HANDLE             f_hOutBuf,
    __in                                            DRM_DWORD                            f_cbData )
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */
{
    DRM_RESULT                               dr      = DRM_SUCCESS;
    DRM_BYTE                                *pbData  = NULL;
    const OEM_OPAQUE_BUFFER_HANDLE_INTERNAL *pInBuf  = (const OEM_OPAQUE_BUFFER_HANDLE_INTERNAL*)f_hInBuf;
    OEM_OPAQUE_BUFFER_HANDLE_INTERNAL       *pOutBuf = (OEM_OPAQUE_BUFFER_HANDLE_INTERNAL*)f_hOutBuf;

    ChkArg( f_hInBuf        != OEM_OPAQUE_BUFFER_HANDLE_INVALID );
    ChkPtr( pInBuf );
    ChkArg( pInBuf->eType   == eOEM_OPAQUE_BUFFER_HANDLE_INTERNAL_TYPE_CLEAR_DATA_SOURCE );
    ChkArg( pInBuf->cbData  == f_cbData );
    ChkArg( f_hOutBuf       != OEM_OPAQUE_BUFFER_HANDLE_INVALID );
    ChkPtr( pOutBuf );
    ChkArg( pOutBuf->eType  == eOEM_OPAQUE_BUFFER_HANDLE_INTERNAL_TYPE_INVALID );

    /*
    ** Only FULL encryption mode is supported in this reference implementation
    */
    ChkBOOL( f_eSampleEncryptionMode == OEM_HAL_SAMPLE_ENCRYPTION_MODE_FULL, DRM_E_OEMHAL_SAMPLE_ENCRYPTION_MODE_NOT_PERMITTED );

    ChkMem( pbData = (DRM_BYTE*)Oem_MemAlloc( f_cbData ) );
    OEM_SECURE_MEMCPY( pbData, pInBuf->pbData, f_cbData );

    ChkDR( Oem_Aes_CtrProcessData( f_pKey, pbData, f_cbData, f_pCounterContext ) );

    pOutBuf->cbData = f_cbData;
    pOutBuf->eType  = eOEM_OPAQUE_BUFFER_HANDLE_INTERNAL_TYPE_ENCRYPTED_DATA;
    pOutBuf->pbData = pbData;
    pbData          = NULL;

ErrorExit:
    SAFE_OEM_FREE( pbData );
    return dr;
}

static DRM_FRE_INLINE DRM_RESULT _EncryptSampleData(
    __in                                            DRM_VOID                            *f_pOEMContext,
    __in                                      const OEM_HAL_AES_KEY_PAIR                *f_pKeyPair,
    __in                                            OEM_HAL_ENCRYPTION_CONTEXT_INTERNAL *f_pContext,
    __out_ecount_opt(1)                             DRM_UINT64                          *f_pqwInitializationVector,
    __out_ecount_opt(1)                             OEM_HAL_SAMPLE_METADATA             *f_pMetadata,
    __in_opt                                        OEM_OPAQUE_BUFFER_HANDLE             f_hInBuf,
    __in_opt                                        OEM_OPAQUE_BUFFER_HANDLE             f_hOutBuf,
    __in                                            DRM_DWORD                            f_cbData,
    __inout_bcount_opt( f_cbData )                  DRM_BYTE                            *f_pbData ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_RESULT _EncryptSampleData(
    __in                                            DRM_VOID                            *f_pOEMContext,
    __in                                      const OEM_HAL_AES_KEY_PAIR                *f_pKeyPair,
    __in                                            OEM_HAL_ENCRYPTION_CONTEXT_INTERNAL *f_pContext,
    __out_ecount_opt(1)                             DRM_UINT64                          *f_pqwInitializationVector,
    __out_ecount_opt(1)                             OEM_HAL_SAMPLE_METADATA             *f_pMetadata,
    __in_opt                                        OEM_OPAQUE_BUFFER_HANDLE             f_hInBuf,
    __in_opt                                        OEM_OPAQUE_BUFFER_HANDLE             f_hOutBuf,
    __in                                            DRM_DWORD                            f_cbData,
    __inout_bcount_opt( f_cbData )                  DRM_BYTE                            *f_pbData )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    ChkBOOL( f_pKeyPair->rgExpandedKeys[OEM_HAL_KEY_PAIR_CK_INDEX].fExpanded, DRM_E_LOGICERR );

    /*
    ** If f_pqwInitializationVector is not NULL a new IV is requested
    */
    if( f_pqwInitializationVector != NULL )
    {
        /*
        ** Reset the counter context
        */
        OEM_SECURE_ZERO_MEMORY( &f_pContext->counterContext, SIZEOF( f_pContext->counterContext ) );

        ChkDR( Oem_Random_GetBytes(
            f_pOEMContext,
            (DRM_BYTE *)&f_pContext->counterContext.qwInitializationVector,
            SIZEOF( f_pContext->counterContext.qwInitializationVector ) ) );

        f_pContext->fIvSet = TRUE;
    }
    else
    {
        /*
        ** Otherwise, make sure the IV has been set
        */
        ChkArg( f_pContext->fIvSet );
    }

    /*
    ** Carry out different processing dependent upon which input data type has been provided
    */
    if( f_hInBuf != OEM_OPAQUE_BUFFER_HANDLE_INVALID )
    {
        ChkDR( _EncryptSecureSampleBuffer(
            &f_pContext->counterContext,
            &f_pKeyPair->rgExpandedKeys[OEM_HAL_KEY_PAIR_CK_INDEX].rgExpandedKey,
            f_pContext->eSampleEncryptionMode,
            f_hInBuf,
            f_hOutBuf,
            f_cbData ) );
    }
    else
    {
        /*
        ** Currently only FULL encryption mode is supported for sample buffers
        */
        ChkBOOL( f_pContext->eSampleEncryptionMode == OEM_HAL_SAMPLE_ENCRYPTION_MODE_FULL,
                 DRM_E_OEMHAL_SAMPLE_ENCRYPTION_MODE_NOT_PERMITTED );

        ChkDR( Oem_Aes_CtrProcessData(
            &f_pKeyPair->rgExpandedKeys[OEM_HAL_KEY_PAIR_CK_INDEX].rgExpandedKey,
            f_pbData,
            f_cbData,
            &f_pContext->counterContext ) );
    }

    if( f_pqwInitializationVector != NULL )
    {
        *f_pqwInitializationVector = f_pContext->counterContext.qwInitializationVector;
    }

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_EncryptSampleData(
    __in                                            DRM_VOID                            *f_pOEMContext,
    __in                                            OEM_ENCRYPTION_HANDLE                f_hEncryptionContext,
    __out_ecount_opt(1)                             DRM_UINT64                          *f_pqwInitializationVector,
    __out_ecount_opt(1)                             OEM_HAL_SAMPLE_METADATA             *f_pMetadata,
    __in_opt                                        OEM_OPAQUE_BUFFER_HANDLE             f_hInBuf,
    __in_opt                                        OEM_OPAQUE_BUFFER_HANDLE             f_hOutBuf,
    __in                                            DRM_DWORD                            f_cbData,
    __inout_bcount_opt( f_cbData )                  DRM_BYTE                            *f_pbData )
{
    DRM_RESULT                           dr       = DRM_SUCCESS;
    OEM_HAL_ENCRYPTION_CONTEXT_INTERNAL *pContext = NULL;
    const OEM_HAL_AES_KEY_PAIR          *pKeyPair = NULL;

    ChkDR( Oem_HalOemImpl_VerifyHalInitialized() );
    ChkDR( _ValidateEncryptionContext( f_hEncryptionContext, &pContext ) );

    /*
    ** Only one of the input methods should be valid, and supply of f_hInBuf must be the same as f_hOutBuf
    */
    ChkArg( ( f_hInBuf == OEM_OPAQUE_BUFFER_HANDLE_INVALID ) != ( f_pbData  == NULL ) );
    ChkArg( ( f_hInBuf == OEM_OPAQUE_BUFFER_HANDLE_INVALID ) == ( f_hOutBuf == OEM_OPAQUE_BUFFER_HANDLE_INVALID ) );

    ChkDR( Oem_HalOemImpl_VerifyValidKeyRegister( pContext->contentKeyType, pContext->contentKeyIndex ) );
    switch( pContext->contentKeyType )
    {
#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    case OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT:
        pKeyPair = &g_rgKeyRegistersAesCtrNewLicenseContent[pContext->contentKeyIndex];
        break;
#endif /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
    default:
        AssertChkArg( FALSE );
    }

    ChkDR( _EncryptSampleData(
        f_pOEMContext,
        pKeyPair,
        pContext,
        f_pqwInitializationVector,
        f_pMetadata,
        f_hInBuf,
        f_hOutBuf,
        f_cbData,
        f_pbData ) );

ErrorExit:
    return dr;
}

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: Real implementation will require non-const parameters.")
DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_QuerySampleMetadata(
    __in                                            OEM_ENCRYPTION_HANDLE                f_hEncryptionContext,
    __out_ecount(1)                                 OEM_HAL_SAMPLE_METADATA             *f_pMetadata )
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */
{
    UNREFERENCED_PARAMETER( f_hEncryptionContext );
    UNREFERENCED_PARAMETER( f_pMetadata );

    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_DestroySampleEncryptionContext(
    __in                                            OEM_ENCRYPTION_HANDLE                f_hEncryptionContext )
{
    DRM_RESULT                           dr       = DRM_SUCCESS;
    OEM_HAL_ENCRYPTION_CONTEXT_INTERNAL *pContext = NULL;

    ChkDR( Oem_HalOemImpl_VerifyHalInitialized() );
    ChkDR( _ValidateEncryptionContext( f_hEncryptionContext, &pContext ) );

    OEM_SECURE_ZERO_MEMORY( &pContext->counterContext, SIZEOF( pContext->counterContext ) );

    pContext->fInUse          = FALSE;
    pContext->fIvSet          = FALSE;
    pContext->contentKeyType  = OEM_HAL_KEY_TYPE_UNDEFINED;
    pContext->contentKeyIndex = OEM_HAL_KEY_REGISTER_INDEX_INVALID;

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_FreeRegister(
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_keyRegisterIndex )
{
    DRM_RESULT                    dr       = DRM_SUCCESS;
    DRM_BOOL                      fGotLock = FALSE;
    OEM_HAL_REGISTER_STATE_TABLE *pTable   = NULL;

    ChkDR( Oem_HalOemImpl_VerifyHalInitialized() );

    LOCK_REGISTERS( fGotLock );

    ChkDR( Oem_HalOemImpl_VerifyValidKeyRegister( f_eKeyType, f_keyRegisterIndex ) );

    ChkDR( _GetKeyRegisterStateTable( f_eKeyType, &pTable ) );

    /*
    ** Don't free preloaded registers, as these are re-used
    */
    if( pTable->pfState[ f_keyRegisterIndex ] == eOEM_HAL_KEY_REGISTER_STATE_ALLOCATED )
    {
        Oem_HalOemImpl_ZeroKeyRegister( f_eKeyType, f_keyRegisterIndex );

        pTable->pfState[ f_keyRegisterIndex ] = eOEM_HAL_KEY_REGISTER_STATE_UNALLOCATED;
    }

#ifdef HWDECODING

    if(f_eKeyType == OEM_HAL_KEY_TYPE_AES_CTR_CONTENT && Decryptor_CTX != NULL)
    {
	    if(BCM_DECRYPTOR_SOC_Destroy(Decryptor_CTX) != 0)
	    {
			printf("Decryptor Destroy Fail!!!\n");
			return DRM_E_HANDLE;
		}
		Decryptor_CTX = NULL;
	}
#endif
ErrorExit:
    UNLOCK_REGISTERS( fGotLock );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_RegisterCount(
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __out_ecount(1)                                 DRM_DWORD                           *f_pdwTotalRegisters,
    __out_ecount_opt(1)                             DRM_DWORD                           *f_pdwAllocatedRegisters )
{
    DRM_RESULT                    dr       = DRM_SUCCESS;
    DRM_BOOL                      fGotLock = FALSE;
    OEM_HAL_REGISTER_STATE_TABLE *pTable   = NULL;
    OEM_HAL_KEY_REGISTER_INDEX    iState   = 0;

    ChkPtr( f_pdwTotalRegisters );
    ChkDR( Oem_HalOemImpl_VerifyHalInitialized() );

    LOCK_REGISTERS( fGotLock );

    /*
    ** Count the unallocated registers
    */
    ChkDR( _GetKeyRegisterStateTable( f_eKeyType, &pTable ) );

    *f_pdwTotalRegisters     = ( DRM_DWORD )pTable->cState;
    if( f_pdwAllocatedRegisters != NULL )
    {
        *f_pdwAllocatedRegisters = 0;

        for( iState = 0; iState < pTable->cState; iState++ )
        {
            if( pTable->pfState[iState] != eOEM_HAL_KEY_REGISTER_STATE_UNALLOCATED )
            {
                ( *f_pdwAllocatedRegisters )++;
            }
        }
    }

ErrorExit:
    UNLOCK_REGISTERS( fGotLock );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_LookupPreloadedKey(
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in_ecount( 1 )                          const DRM_GUID                            *f_pKeyId,
    __out_ecount( 1 )                               OEM_HAL_KEY_REGISTER_INDEX          *f_pIndexKey )
{
    DRM_RESULT dr    = DRM_SUCCESS;
    DRM_LONG   iSlot = 0;

    ChkPtr( f_pKeyId );
    ChkPtr( f_pIndexKey );

    DRMASSERT( g_cPreloadedKeyInfoSlotsUsed <= OEM_HAL_MAX_PRELOADED_KEYS );

    *f_pIndexKey = OEM_HAL_KEY_REGISTER_INDEX_INVALID;

    for( iSlot = 0; iSlot < g_cPreloadedKeyInfoSlotsUsed; iSlot++ )
    {
        if( g_rgPreloadedKeyInfo[iSlot].ekeyType == f_eKeyType
         && OEM_SECURE_ARE_EQUAL( f_pKeyId, &g_rgPreloadedKeyInfo[iSlot].id, SIZEOF( DRM_GUID ) ) )
        {
            *f_pIndexKey = g_rgPreloadedKeyInfo[iSlot].index;
        }
    }

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_VerifyPreloadedKeyInfoSlotIsAvailable()
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkBOOL( g_cPreloadedKeyInfoSlotsUsed < OEM_HAL_MAX_PRELOADED_KEYS, DRM_E_OEMHAL_OUT_OF_KEY_REGISTERS );

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_SetAesKeyfileExpandedKey(
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_keyRegisterIndex,
    __in                                      const DRM_AES_KEY                         *f_pExpandedKey )
{
    /*
    ** Copy the expanded key directly into the expanded buffer, marking it expanded
    */
    OEM_SECURE_MEMCPY( &g_rgKeyRegistersAesKeyFile[ f_keyRegisterIndex ].expandedKey.rgExpandedKey, f_pExpandedKey, SIZEOF( DRM_AES_KEY ) );
    g_rgKeyRegistersAesKeyFile[ f_keyRegisterIndex ].expandedKey.fExpanded = TRUE;
    return DRM_SUCCESS;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_SetModelSigningKey(
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_keyRegisterIndex,
    __in                                      const PRIVKEY_P256                        *f_pModelSigningKey )
{
    OEM_SECURE_MEMCPY( &g_rgKeyRegistersEccP256ModelSign[ f_keyRegisterIndex ].privateKey, f_pModelSigningKey, SIZEOF( PRIVKEY_P256 ) );
    return DRM_SUCCESS;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_SetEcc256PublicKeyRoot(
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_keyRegisterIndex,
    __in                                      const PUBKEY_P256                         *f_pEcc256PublicKey,
    __in                                            DRM_DWORD                            f_dwKeyUsageBitmap )
{
    _ResetPublicKeyValidation( &g_rgKeyRegistersEccP256Public[ f_keyRegisterIndex ] );
    OEM_SECURE_MEMCPY( &g_rgKeyRegistersEccP256Public[ f_keyRegisterIndex ].key, f_pEcc256PublicKey, SIZEOF( PUBKEY_P256 ) );
    g_rgKeyRegistersEccP256Public[ f_keyRegisterIndex ].dwKeyUsageBitmap                  = f_dwKeyUsageBitmap;
    g_rgKeyRegistersEccP256Public[ f_keyRegisterIndex ].dwFullCertKeyUsageBitmap          = f_dwKeyUsageBitmap;
    g_rgKeyRegistersEccP256Public[ f_keyRegisterIndex ].fRootKey                          = TRUE;
    g_rgKeyRegistersEccP256Public[ f_keyRegisterIndex ].validationInfo.eVerificationState = eOEM_HAL_PUBKEY_VERIFIED;     /* Root keys can't be revoked, so they're always verified. */
    return DRM_SUCCESS;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_SavePreloadedKeyInfo(
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in_ecount( 1 )                          const DRM_GUID                            *f_pKeyId,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_indexKey )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkBOOL( g_cPreloadedKeyInfoSlotsUsed < OEM_HAL_MAX_PRELOADED_KEYS, DRM_E_LOGICERR );
    ChkPtr( f_pKeyId );

    g_rgPreloadedKeyInfo[g_cPreloadedKeyInfoSlotsUsed].ekeyType = f_eKeyType;
    g_rgPreloadedKeyInfo[g_cPreloadedKeyInfoSlotsUsed].index    = f_indexKey;
    g_rgPreloadedKeyInfo[g_cPreloadedKeyInfoSlotsUsed].id       = *f_pKeyId;

    g_cPreloadedKeyInfoSlotsUsed++;

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_Initialize()
{
    DRM_RESULT dr = DRM_S_FALSE;

    g_nInitCount++;

    /*
    ** If this is the first time this has been called, initialize the critical section
    */
    if( g_nInitCount == 1 )
    {
        Oem_CritSec_Initialize( &g_csProtectRegisters );

        dr = DRM_SUCCESS;
    }

    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_Deinitialize()
{
    DRM_RESULT dr  = DRM_E_OEMHAL_NOT_INITIALIZED;
    DRM_DWORD  idx = 0;

    if( g_nInitCount > 0 )
    {
        g_nInitCount--;

        if( g_nInitCount == 0 )
        {
            SAFE_OEM_FREE( g_RevocationInfo.pCRLEntries );
            OEM_SECURE_ZERO_MEMORY( &g_RevocationInfo, SIZEOF( OEM_HALIMPL_REVOCATION_INFO ) );

            for( idx = 0; idx < NO_OF( g_rgEncryptionContexts ); idx++)
            {
                OEM_SECURE_ZERO_MEMORY( &(g_rgEncryptionContexts[idx]), SIZEOF( OEM_HAL_ENCRYPTION_CONTEXT_INTERNAL ) );
            }

            for( idx = 0; idx < NO_OF( g_rgKeyRegistersAesEcbRoot ); idx++ )
            {
                DRMASSERT( g_rgKeyRegistersAesEcbRoot[idx].pDerivedKey == NULL );
            }

#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
            for( idx = 0; idx < NO_OF( g_rgKeyRegistersAesEcbNewLicenseRoot ); idx++ )
            {
                DRMASSERT( g_rgKeyRegistersAesEcbNewLicenseRoot[idx].pDerivedKey == NULL );
            }
#endif /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */

            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegistersEccP256ContentEncrypt  , SIZEOF( g_rgKeyRegistersEccP256ContentEncrypt   ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegistersEccP256DomainEncrypt   , SIZEOF( g_rgKeyRegistersEccP256DomainEncrypt    ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegistersEccP256MessageEncrypt  , SIZEOF( g_rgKeyRegistersEccP256MessageEncrypt   ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegistersEccP256Sign            , SIZEOF( g_rgKeyRegistersEccP256Sign             ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegistersEccP256Public          , SIZEOF( g_rgKeyRegistersEccP256Public           ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegistersEccP256ModelSign       , SIZEOF( g_rgKeyRegistersEccP256ModelSign        ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegistersAesEcbRoot             , SIZEOF( g_rgKeyRegistersAesEcbRoot              ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegistersAesCtrContent          , SIZEOF( g_rgKeyRegistersAesCtrContent           ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegistersAesEcbMessageEncrypt   , SIZEOF( g_rgKeyRegistersAesEcbMessageEncrypt    ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegistersAesEcbDomainProtect    , SIZEOF( g_rgKeyRegistersAesEcbDomainProtect     ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegistersAesOmac1SecureStore    , SIZEOF( g_rgKeyRegistersAesOmac1SecureStore     ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegistersAesKeyFile             , SIZEOF( g_rgKeyRegistersAesKeyFile              ) );
#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegistersAesGenericKeyContent   , SIZEOF( g_rgKeyRegistersAesGenericKeyContent    ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegistersAesGenericKeyRoot      , SIZEOF( g_rgKeyRegistersAesGenericKeyRoot       ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegistersAesEcbNewLicenseRoot   , SIZEOF( g_rgKeyRegistersAesEcbNewLicenseRoot    ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegistersAesCtrNewLicenseContent, SIZEOF( g_rgKeyRegistersAesCtrNewLicenseContent ) );
#endif /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
#if !DRM_SUPPORT_SECUREOEMHAL
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegistersLegacy                 , SIZEOF( g_rgKeyRegistersLegacy                  ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegistersAesSlk                 , SIZEOF( g_rgKeyRegistersAesSlk                  ) );
#endif /* !DRM_SUPPORT_SECUREOEMHAL */

            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegStatesEccP256ContentEncrypt  , SIZEOF( g_rgKeyRegStatesEccP256ContentEncrypt   ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegStatesEccP256DomainEncrypt   , SIZEOF( g_rgKeyRegStatesEccP256DomainEncrypt    ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegStatesEccP256MessageEncrypt  , SIZEOF( g_rgKeyRegStatesEccP256MessageEncrypt   ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegStatesEccP256Sign            , SIZEOF( g_rgKeyRegStatesEccP256Sign             ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegStatesEccP256Public          , SIZEOF( g_rgKeyRegStatesEccP256Public           ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegStatesEccP256ModelSign       , SIZEOF( g_rgKeyRegStatesEccP256ModelSign        ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegStatesAesEcbRoot             , SIZEOF( g_rgKeyRegStatesAesEcbRoot              ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegStatesAesCtrContent          , SIZEOF( g_rgKeyRegStatesAesCtrContent           ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegStatesAesEcbMessageEncrypt   , SIZEOF( g_rgKeyRegStatesAesEcbMessageEncrypt    ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegStatesAesEcbDomainProtect    , SIZEOF( g_rgKeyRegStatesAesEcbDomainProtect     ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegStatesAesOmac1SecureStore    , SIZEOF( g_rgKeyRegStatesAesOmac1SecureStore     ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegStatesAesKeyFile             , SIZEOF( g_rgKeyRegStatesAesKeyFile              ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegStatesAesGenericKeyContent   , SIZEOF( g_rgKeyRegStatesAesGenericKeyContent    ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegStatesAesGenericKeyRoot      , SIZEOF( g_rgKeyRegStatesAesGenericKeyRoot       ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegStatesAesEcbNewLicenseRoot   , SIZEOF( g_rgKeyRegStatesAesEcbNewLicenseRoot    ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegStatesAesCtrNewLicenseContent, SIZEOF( g_rgKeyRegStatesAesCtrNewLicenseContent ) );
#if !DRM_SUPPORT_SECUREOEMHAL
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegStatesLegacy                 , SIZEOF( g_rgKeyRegStatesLegacy                  ) );
            OEM_SECURE_ZERO_MEMORY( g_rgKeyRegStatesSlk                    , SIZEOF( g_rgKeyRegStatesSlk                     ) );
#endif /* !DRM_SUPPORT_SECUREOEMHAL */

            OEM_SECURE_ZERO_MEMORY( g_rgPreloadedKeyInfo                   , SIZEOF( g_rgPreloadedKeyInfo                    ) );
            g_cPreloadedKeyInfoSlotsUsed = 0;

            Oem_CritSec_Delete( &g_csProtectRegisters );
        }

        dr = DRM_SUCCESS;
    }

    return dr;
}

DRM_API DRM_BOOL DRM_CALL Oem_HalOemImpl_CanSkipLeafCertificateValidityCheckDuringSignOperation()
{
#if DRM_TEST_EXPOSE_TEST_FUNCTIONS
    return g_fSkipLeafCertificateValidityCheckDuringSignOperation;
#else   /* DRM_TEST_EXPOSE_TEST_FUNCTIONS */
    return FALSE;
#endif  /* DRM_TEST_EXPOSE_TEST_FUNCTIONS */
}

DRM_API DRM_BOOL DRM_CALL Oem_HalOemImpl_CanSkipLeafCertificateKeysValidityCheckDuringSignOperation()
{
#if DRM_TEST_EXPOSE_TEST_FUNCTIONS
    return g_fSkipLeafCertificateKeysValidityCheckDuringSignOperation;
#else   /* DRM_TEST_EXPOSE_TEST_FUNCTIONS */
    return FALSE;
#endif  /* DRM_TEST_EXPOSE_TEST_FUNCTIONS */
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_VerifyMessageSignature(
    __in_bcount( f_cbMessage )                const DRM_BYTE                            *f_rgbMessage,
    __in                                            DRM_DWORD                            f_cbMessage,
    __in_bcount( f_cbSignature )              const DRM_BYTE                            *f_rgbSignature,
    __in                                            DRM_DWORD                            f_cbSignature,
    __in                                            OEM_HAL_HASH_TYPE                    f_eHashType,
    __in                                            OEM_HAL_SIGNATURE_SCHEME             f_eSignatureScheme,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_indexIntegrityKey )
{
    UNREFERENCED_PARAMETER( f_rgbMessage );
    UNREFERENCED_PARAMETER( f_cbMessage );
    UNREFERENCED_PARAMETER( f_rgbSignature );
    UNREFERENCED_PARAMETER( f_cbSignature );
    UNREFERENCED_PARAMETER( f_eHashType );
    UNREFERENCED_PARAMETER( f_eSignatureScheme );
    UNREFERENCED_PARAMETER( f_indexIntegrityKey );

    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_SignWithPrivateKey(
    __inout_ecount( 1 )                             DRM_CRYPTO_CONTEXT                  *f_pContext,
    __in_bcount( f_cbMessage )                const DRM_BYTE                            *f_rgbMessage,
    __in                                            DRM_DWORD                            f_cbMessage,
    __out_bcount_opt( *f_pcbSignature )             DRM_BYTE                            *f_rgbSignature,
    __inout                                         DRM_DWORD                           *f_pcbSignature,
    __in                                            DRM_DWORD                            f_dwRequestedSecurityLevelForLeafDeviceCertificate,
    __in                                            OEM_HAL_HASH_TYPE                    f_eHashType,
    __in                                            OEM_HAL_SIGNATURE_SCHEME             f_eSignatureScheme,
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_indexIntegrityKey )
{
    DRM_RESULT        dr               = DRM_SUCCESS;
    SIGNATURE_P256    oECC256Signature = {0};


    ChkDR( Oem_HalOemImpl_VerifyHalInitialized() );
    ChkPtr( f_rgbMessage );
    ChkPtr( f_pcbSignature );
    ChkBOOL( f_eHashType        == OEM_HAL_HASH_TYPE_SHA256, DRM_E_OEMHAL_UNSUPPORTED_HASH_TYPE );
    ChkBOOL( f_eSignatureScheme == OEM_HAL_SIGNATURE_SCHEME_ECDSA_P256, DRM_E_OEMHAL_UNSUPPORTED_SIGNATURE_SCHEME );


    ChkDR( Oem_HalOemImpl_VerifyValidKeyRegister( f_eKeyType, f_indexIntegrityKey ) );


    if( ECDSA_P256_SIGNATURE_SIZE_IN_BYTES > *f_pcbSignature
     || f_rgbSignature                     == NULL )
    {

        dr = DRM_E_BUFFERTOOSMALL;
    }
    else
    {
        const PRIVKEY_P256 *pSigningKey = NULL;


        switch( f_eKeyType )
        {
        case OEM_HAL_KEY_TYPE_ECC_P256_SIGN:

            pSigningKey = &g_rgKeyRegistersEccP256Sign[f_indexIntegrityKey].privateKey;
            break;

        case OEM_HAL_KEY_TYPE_ECC_P256_MODEL_SIGN:

            pSigningKey = &g_rgKeyRegistersEccP256ModelSign[f_indexIntegrityKey].privateKey;

            /* You can't sign a model certificate with a higher security level than this device. */
            ChkBOOL( f_dwRequestedSecurityLevelForLeafDeviceCertificate <= OEM_KEY_GENERATION_SECURITY_LEVEL, DRM_E_DEVICE_SECURITY_LEVEL_TOO_LOW );
            break;

        default:

            ChkDR( DRM_E_OEMHAL_UNSUPPORTED_SIGNATURE_SCHEME );
        }

        DRMASSERT( pSigningKey != NULL );


        ChkDR( OEM_ECDSA_Sign_P256(
            f_rgbMessage,
            f_cbMessage,
            pSigningKey,
            &oECC256Signature,
            ( struct bigctx_t* )f_pContext->rgbCryptoContext ) );


        OEM_SECURE_MEMCPY( f_rgbSignature, oECC256Signature.m_rgbSignature, ECDSA_P256_SIGNATURE_SIZE_IN_BYTES );

    }


    *f_pcbSignature = ECDSA_P256_SIGNATURE_SIZE_IN_BYTES;

ErrorExit:

    return dr;
}

/*Added For Get Content Key*/
#if 0
DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_GetContentKey(
    __in_ecount( 1 )           					DRM_CIPHER_CONTEXT           *f_pCipherContext,
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_indexContentKey)
{
    DRM_RESULT        			 dr              = DRM_SUCCESS;
    OEM_HAL_AES_KEY_PAIR         *pKeyPair       = NULL;
	int i;
    ChkDR( Oem_HalOemImpl_VerifyHalInitialized() );
    ChkDR( Oem_HalOemImpl_VerifyValidKeyRegister( f_eKeyType, f_indexContentKey ) );


	switch( f_eKeyType )
	{
	case OEM_HAL_KEY_TYPE_AES_CTR_CONTENT:
		pKeyPair = &g_rgKeyRegistersAesCtrContent[f_indexContentKey];
		break;
#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
	case OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT:
		pKeyPair = &g_rgKeyRegistersAesCtrNewLicenseContent[f_indexContentKey];
		break;
#endif /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
	default:
		AssertChkArg( FALSE );
	}
	//ChkMem( pbData = (DRM_BYTE*)Oem_MemAlloc( f_cbData ) );
	OEM_SECURE_MEMCPY( f_pCipherContext->cipher.cipherAES.rgbContentKey, pKeyPair->rgbKeys[OEM_HAL_KEY_PAIR_CK_INDEX], DRM_AES_KEYSIZE_128 );
ErrorExit:
		return dr;

	//f_pCipherContext->cipher.cipherAES.rgbContentKey
}
#endif
DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_DecryptContent(
    __inout_bcount( f_cbData )                      DRM_BYTE                            *f_pbData,
    __in                                            DRM_DWORD                            f_cbData,
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_indexContentKey,
    __in                                            DRM_UINT64                           f_qwInitializationVector,
    __in                                            DRM_UINT64                           f_qwSampleByteOffset )
{
    DRM_RESULT                    dr             = DRM_SUCCESS;
    OEM_HAL_AES_KEY_PAIR         *pKeyPair       = NULL;
#ifdef HWDECODING
    DRM_BYTE qwIVCorrectEndianness2[16] = {0,};
#endif
#ifndef HWDECODING
	DRM_AES_COUNTER_MODE_CONTEXT counterContext = {0};
#endif
    ChkDR( Oem_HalOemImpl_VerifyHalInitialized() );
    ChkPtr( f_pbData );
    ChkDR( Oem_HalOemImpl_VerifyValidKeyRegister( f_eKeyType, f_indexContentKey ) );
#ifdef HWDECODING
/*	QWORD_TO_NETWORKBYTES_SECURE( (DRM_BYTE*)&qwIVCorrectEndianness, 0, f_qwInitializationVector );*/
/*	QWORD_TO_NETWORKBYTES_SECURE( (DRM_BYTE*)&qwIVCorrectEndianness, 8, DRM_UI64ShR(f_qwSampleByteOffset, 4 ));*/
#endif
#if 0
	printf("qwIVCorrectEndianness	= %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
		qwIVCorrectEndianness[0], qwIVCorrectEndianness[1], qwIVCorrectEndianness[2], qwIVCorrectEndianness[3], qwIVCorrectEndianness[4], qwIVCorrectEndianness[5], qwIVCorrectEndianness[6], qwIVCorrectEndianness[7],
		qwIVCorrectEndianness[8], qwIVCorrectEndianness[9], qwIVCorrectEndianness[10], qwIVCorrectEndianness[11], qwIVCorrectEndianness[12], qwIVCorrectEndianness[13], qwIVCorrectEndianness[14], qwIVCorrectEndianness[15]);
#endif
	switch( f_eKeyType )
    {
    case OEM_HAL_KEY_TYPE_AES_CTR_CONTENT:
        pKeyPair = &g_rgKeyRegistersAesCtrContent[f_indexContentKey];
        break;
#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    case OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT:
        pKeyPair = &g_rgKeyRegistersAesCtrNewLicenseContent[f_indexContentKey];
        break;
#endif /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
    default:
        AssertChkArg( FALSE );
    }

    /*
    ** Ensure the AES Content key is initialized
    */
    ChkDR( _CheckAesKeyExpanded(
        &pKeyPair->rgbKeys[OEM_HAL_KEY_PAIR_CK_INDEX][0],
        &pKeyPair->rgExpandedKeys[OEM_HAL_KEY_PAIR_CK_INDEX] ) );

#ifdef HWDECODING
	if(Decryptor_CTX ==NULL)
	{
		printf("Decryptor_CTX = NULL\n");
		return DRM_E_HANDLE;
	}
	else
	{
		DRM_UINT64 qwIVCorrectEndianness;
		DRM_BYTE	bByteOffset = (DRM_BYTE)DRM_UI64Low32( DRM_UI64Mod( f_qwSampleByteOffset, DRM_UI64HL( 0, DRM_AES_BLOCKLEN ) ) );  /* Byte offset within the current block */
		DRM_UINT64	bBlockOffset = DRM_UI64ShR( f_qwSampleByteOffset, 4 ) ;
		DRM_DWORD  cbDataToUse	= 0;
		DRM_DWORD  cbDataLeft	= f_cbData;
		DRM_DWORD  cbDataLeft2	= 0;
		DRM_UINT64 rllDataOut[__CB_DECL( DRM_AES_BLOCKLEN )/SIZEOF(DRM_UINT64)];
		DRM_DWORD  ibDataOutCur = 0;
		DRM_BYTE *pucBuf = NULL;

		QWORD_TO_NETWORKBYTES_SECURE( (DRM_BYTE*)&qwIVCorrectEndianness, 0, f_qwInitializationVector );

		if( bByteOffset > 0 )
		{
			/*
			** The data is in the middle of a block.  Handle the special case first
			*/
			cbDataToUse = (DRM_DWORD)( DRM_AES_BLOCKLEN - bByteOffset );
			cbDataToUse = min( cbDataLeft, cbDataToUse );

			QWORD_TO_NETWORKBYTES_SECURE( (DRM_BYTE*)&( rllDataOut[1] ), 0, bBlockOffset );
			rllDataOut[0] = qwIVCorrectEndianness;

			dr = Oem_Aes_EncryptOne( &pKeyPair->rgExpandedKeys[OEM_HAL_KEY_PAIR_CK_INDEX].rgExpandedKey, (DRM_BYTE*) rllDataOut );
			ChkBOOL( DRM_SUCCEEDED( dr ), DRM_E_CRYPTO_FAILED );

			_XOR( f_pbData, ibDataOutCur, (DRM_BYTE*)rllDataOut, bByteOffset, cbDataToUse );

			ibDataOutCur += cbDataToUse;

			ChkOverflow( cbDataLeft, cbDataLeft - cbDataToUse );
			cbDataLeft -= cbDataToUse;

			/*
			** If we used all of the bytes in the current block, then the block offset needs to be increased by one.
			*/
			if (bByteOffset + cbDataToUse == DRM_AES_BLOCKLEN )
			{
				bBlockOffset = DRM_UI64Add( bBlockOffset , DRM_UI64( 1 ) );
			}
		}
		cbDataLeft2 = cbDataLeft % DRM_AES_BLOCKLEN;
		if(	cbDataLeft >= DRM_AES_BLOCKLEN)
		{
			memcpy(qwIVCorrectEndianness2, (DRM_BYTE*)&qwIVCorrectEndianness, 8);
			QWORD_TO_NETWORKBYTES_SECURE( (DRM_BYTE*)qwIVCorrectEndianness2, 8, bBlockOffset);
			pucBuf = BCM_DECRYPTOR_SOC_Malloc(cbDataLeft-cbDataLeft2);
			memcpy(pucBuf, f_pbData+ibDataOutCur, cbDataLeft-cbDataLeft2);

			BCM_DECRYPTOR_SOC_DoDecrypt(Decryptor_CTX, pucBuf, cbDataLeft-cbDataLeft2, &pKeyPair->rgExpandedKeys[OEM_HAL_KEY_PAIR_CK_INDEX].rgExpandedKey, (uint8_t*)qwIVCorrectEndianness2);

			memcpy(f_pbData+ibDataOutCur, pucBuf, cbDataLeft-cbDataLeft2);
			BCM_DECRYPTOR_SOC_Free(pucBuf);
			bBlockOffset = DRM_UI64Add( bBlockOffset, DRM_UI64( cbDataLeft / DRM_AES_BLOCKLEN ) );

			ibDataOutCur += DRM_AES_BLOCKLEN * (cbDataLeft / DRM_AES_BLOCKLEN) ;
		}
		if( cbDataLeft2 > 0 ) /* at this point it is strictly less than DRM_AES_BLOCKLEN */
		{
			cbDataToUse = cbDataLeft2;
			QWORD_TO_NETWORKBYTES_SECURE( (DRM_BYTE*)&( rllDataOut[1] ), 0, bBlockOffset );
			rllDataOut[0] = qwIVCorrectEndianness;

			dr = Oem_Aes_EncryptOne( &pKeyPair->rgExpandedKeys[OEM_HAL_KEY_PAIR_CK_INDEX].rgExpandedKey, (DRM_BYTE*)rllDataOut );
			ChkBOOL( DRM_SUCCEEDED( dr ), DRM_E_CRYPTO_FAILED );

			_XOR( f_pbData, ibDataOutCur, (DRM_BYTE*)rllDataOut, 0, cbDataToUse );
		}
		bByteOffset = ( bByteOffset + f_cbData ) % DRM_AES_BLOCKLEN;

	}
#endif
#ifndef HWDECODING
	counterContext.qwInitializationVector = f_qwInitializationVector;
	counterContext.qwBlockOffset = DRM_UI64ShR( f_qwSampleByteOffset, 4 );
	counterContext.bByteOffset = (DRM_BYTE)DRM_UI64Low32( DRM_UI64Mod( f_qwSampleByteOffset, DRM_UI64HL( 0, DRM_AES_BLOCKLEN ) ) );

	ChkDR( Oem_Aes_CtrProcessData(
		&pKeyPair->rgExpandedKeys[OEM_HAL_KEY_PAIR_CK_INDEX].rgExpandedKey,
		f_pbData,
		f_cbData,
		&counterContext ) );
#endif

/*ORIGINAL CODE PLAYREADY 2.5 S/W DECRYPT*/

/*	counterContext.qwInitializationVector = f_qwInitializationVector;*/
/*
** 	Take the bottom four bits of f_qwSampleByteOffset to specify the byte offset, and use the
** 	remainder, shifted down, as the block offset
*/
/*	counterContext.qwBlockOffset = DRM_UI64ShR( f_qwSampleByteOffset, 4 );*/
/*	counterContext.bByteOffset = (DRM_BYTE)DRM_UI64Low32( DRM_UI64Mod( f_qwSampleByteOffset, DRM_UI64HL( 0, DRM_AES_BLOCKLEN ) ) );*/

/*
	ORIGINAL CODE PLAYREADY 2.5 S/W DECRYPT
	ChkDR( Oem_Aes_CtrProcessData(
        &pKeyPair->rgExpandedKeys[OEM_HAL_KEY_PAIR_CK_INDEX].rgExpandedKey,
        f_pbData,
        f_cbData,
        &counterContext ) );
*/
ErrorExit:
    return dr;
}

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: Real implementation will require non-const parameters.")
DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_DecryptContentOpaque(
    __in                                            OEM_OPAQUE_BUFFER_HANDLE             f_hInBuf,
    __in                                            OEM_OPAQUE_BUFFER_HANDLE             f_hOutBuf,
    __in                                            DRM_DWORD                            f_cbData,
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_indexContentKey,
    __in                                            DRM_UINT64                           f_qwInitializationVector,
    __in                                            DRM_UINT64                           f_qwSampleByteOffset )
{
    DRM_RESULT                               dr             = DRM_SUCCESS;
    DRM_AES_COUNTER_MODE_CONTEXT             counterContext = {0};
    OEM_HAL_AES_KEY_PAIR                    *pKeyPair       = NULL;
    DRM_BYTE                                *pbData         = NULL;
    const OEM_OPAQUE_BUFFER_HANDLE_INTERNAL *pInBuf         = (const OEM_OPAQUE_BUFFER_HANDLE_INTERNAL*)f_hInBuf;
    OEM_OPAQUE_BUFFER_HANDLE_INTERNAL       *pOutBuf        = (OEM_OPAQUE_BUFFER_HANDLE_INTERNAL*)f_hOutBuf;

    ChkDR( Oem_HalOemImpl_VerifyHalInitialized() );

    ChkDR( Oem_HalOemImpl_VerifyValidKeyRegister( f_eKeyType, f_indexContentKey ) );

    ChkArg( f_hInBuf        != OEM_OPAQUE_BUFFER_HANDLE_INVALID );
    ChkPtr( pInBuf );
    ChkArg( pInBuf->eType   == eOEM_OPAQUE_BUFFER_HANDLE_INTERNAL_TYPE_ENCRYPTED_DATA );
    ChkArg( pInBuf->cbData  == f_cbData );
    ChkArg( f_hOutBuf       != OEM_OPAQUE_BUFFER_HANDLE_INVALID );
    ChkPtr( pOutBuf );
    ChkArg( pOutBuf->eType  == eOEM_OPAQUE_BUFFER_HANDLE_INTERNAL_TYPE_INVALID );

    switch( f_eKeyType )
    {
    case OEM_HAL_KEY_TYPE_AES_CTR_CONTENT:
        pKeyPair = &g_rgKeyRegistersAesCtrContent[f_indexContentKey];
        break;
#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    case OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT:
        pKeyPair = &g_rgKeyRegistersAesCtrNewLicenseContent[f_indexContentKey];
        break;
#endif /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
    default:
        AssertChkArg( FALSE );
    }

    /*
    ** Ensure the AES Content key is initialized
    */
    ChkDR( _CheckAesKeyExpanded( &pKeyPair->rgbKeys[OEM_HAL_KEY_PAIR_CK_INDEX][0],
                                 &pKeyPair->rgExpandedKeys[OEM_HAL_KEY_PAIR_CK_INDEX] ) );

    counterContext.qwInitializationVector = f_qwInitializationVector;

    /*
    ** Take the bottom four bits of f_qwSampleByteOffset to specify the byte offset, and use the
    ** remainder, shifted down, as the block offset
    */
    counterContext.qwBlockOffset = DRM_UI64ShR( f_qwSampleByteOffset, 4 );
    counterContext.bByteOffset = (DRM_BYTE)DRM_UI64Low32( DRM_UI64Mod( f_qwSampleByteOffset, DRM_UI64HL( 0, DRM_AES_BLOCKLEN ) ) );

    ChkMem( pbData = (DRM_BYTE*)Oem_MemAlloc( f_cbData ) );
    OEM_SECURE_MEMCPY( pbData, pInBuf->pbData, f_cbData );

    ChkDR( Oem_Aes_CtrProcessData( &pKeyPair->rgExpandedKeys[OEM_HAL_KEY_PAIR_CK_INDEX].rgExpandedKey,
                                   pbData,
                                   f_cbData,
                                   &counterContext ) );

    pOutBuf->cbData = f_cbData;
    pOutBuf->eType  = eOEM_OPAQUE_BUFFER_HANDLE_INTERNAL_TYPE_CLEAR_DATA_RENDER;
    pOutBuf->pbData = pbData;
    pbData          = NULL;

ErrorExit:
    SAFE_OEM_FREE( pbData );
    return dr;
}
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_CreateSampleEncryptionContext(
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_indexKey,
    __in                                            OEM_HAL_SAMPLE_ENCRYPTION_MODE       f_eSampleEncryptionMode,
    __out_ecount(1)                                 OEM_ENCRYPTION_HANDLE               *f_phEncryptionContext )
{
    DRM_RESULT               dr       = DRM_SUCCESS;
    DRM_BOOL                 fGotLock = FALSE;
    DRM_LONG                 idx      = 0;
    OEM_HAL_AES_KEY_PAIR    *pKeyPair = NULL;

    ChkDR( Oem_HalOemImpl_VerifyHalInitialized() );

    ChkPtr( f_phEncryptionContext );
    ChkArg( f_eSampleEncryptionMode >= OEM_HAL_SAMPLE_ENCRYPTION_MODE_PASSTHRU
         && f_eSampleEncryptionMode <= OEM_HAL_SAMPLE_ENCRYPTION_MODE_SUBSAMPLE_CFF );

    ChkDR( Oem_HalOemImpl_VerifyValidKeyRegister( f_eKeyType, f_indexKey ) );

    *f_phEncryptionContext = NULL;

    switch( f_eKeyType )
    {
#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    case OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT:
        pKeyPair = &g_rgKeyRegistersAesCtrNewLicenseContent[f_indexKey];
        break;
#endif /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
    default:
        AssertChkArg( FALSE );
    }

    /*
    ** Before taking the lock, ensure the AES Content key is initialized
    */
    ChkDR( _CheckAesKeyExpanded(
        &pKeyPair->rgbKeys[OEM_HAL_KEY_PAIR_CK_INDEX][0],
        &pKeyPair->rgExpandedKeys[OEM_HAL_KEY_PAIR_CK_INDEX] ) );

    /*
    ** Now find an empty context to use
    */
    LOCK_REGISTERS( fGotLock );

    for( idx = 0; idx < NO_OF( g_rgEncryptionContexts ); idx++ )
    {
        if( !g_rgEncryptionContexts[idx].fInUse )
        {
            g_rgEncryptionContexts[idx].fInUse                = TRUE;
            g_rgEncryptionContexts[idx].eSampleEncryptionMode = f_eSampleEncryptionMode;
            g_rgEncryptionContexts[idx].contentKeyType        = f_eKeyType;
            g_rgEncryptionContexts[idx].contentKeyIndex       = f_indexKey;
            *f_phEncryptionContext                            = ( OEM_ENCRYPTION_HANDLE )&g_rgEncryptionContexts[idx];
            break;
        }
    }

    ChkBOOL( *f_phEncryptionContext != NULL, DRM_E_OUTOFMEMORY );

    DRMASSERT( _ValidateEncryptionContext( *f_phEncryptionContext, NULL ) == DRM_SUCCESS );

ErrorExit:
    UNLOCK_REGISTERS( fGotLock );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_GetEcc256PublicKeyUsageBitmap(
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_keyRegisterIndex,
    __out_ecount(1)                                 DRM_DWORD                           *f_pdwKeyUsageBitmap,
    __out_ecount_opt(1)                             DRM_DWORD                           *f_pdwFullCertKeyUsageBitmap )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_BOOL   fGotLock = FALSE;

    AssertChkArg( f_pdwKeyUsageBitmap != NULL );
    LOCK_REGISTERS( fGotLock );

    ChkDR( Oem_HalOemImpl_VerifyValidKeyRegister( OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC, f_keyRegisterIndex ) );

    *f_pdwKeyUsageBitmap = g_rgKeyRegistersEccP256Public[ f_keyRegisterIndex ].dwKeyUsageBitmap;
    if( f_pdwFullCertKeyUsageBitmap != NULL )
    {
        *f_pdwFullCertKeyUsageBitmap = g_rgKeyRegistersEccP256Public[ f_keyRegisterIndex ].dwFullCertKeyUsageBitmap;
    }

ErrorExit:
    UNLOCK_REGISTERS( fGotLock );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_GetEcc256PublicKeyCertDigests(
    __in                                                            OEM_HAL_KEY_REGISTER_INDEX           f_keyRegisterIndex,
    __out_ecount_part(DRM_BCERT_MAX_CERTS_PER_CHAIN,*f_pcDigests)   DRM_SHA256_Digest                    f_rgCertDigests[DRM_BCERT_MAX_CERTS_PER_CHAIN],
    __out                                                           DRM_DWORD                           *f_pcDigests )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_BOOL   fGotLock = FALSE;

    AssertChkArg( f_rgCertDigests != NULL );
    AssertChkArg( f_pcDigests     != NULL );
    LOCK_REGISTERS( fGotLock );

    ChkDR( Oem_HalOemImpl_VerifyValidKeyRegister( OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC, f_keyRegisterIndex ) );

    *f_pcDigests = g_rgKeyRegistersEccP256Public[f_keyRegisterIndex].validationInfo.cDigests;
    DRMASSERT( ( *f_pcDigests == 0 ) == g_rgKeyRegistersEccP256Public[f_keyRegisterIndex].fRootKey );
    OEM_SECURE_MEMCPY( f_rgCertDigests, &g_rgKeyRegistersEccP256Public[f_keyRegisterIndex].validationInfo.certificateDigests, SIZEOF( DRM_SHA256_Digest ) * (*f_pcDigests) );

ErrorExit:
    UNLOCK_REGISTERS( fGotLock );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_SetEcc256PublicKeyVerificationState(
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_keyRegisterIndex,
    __in                                            OEM_HAL_PUBKEY_VERIFICATION_STATE    f_eVerificationState )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_BOOL   fGotLock = FALSE;

    LOCK_REGISTERS( fGotLock );

    ChkDR( Oem_HalOemImpl_VerifyValidKeyRegister( OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC, f_keyRegisterIndex ) );

    g_rgKeyRegistersEccP256Public[f_keyRegisterIndex].validationInfo.eVerificationState = f_eVerificationState;

ErrorExit:
    UNLOCK_REGISTERS( fGotLock );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_GetEcc256PublicKeyVerificationState(
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_keyRegisterIndex,
    __in                                            OEM_HAL_PUBKEY_VERIFICATION_STATE   *f_peVerificationState )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_BOOL   fGotLock = FALSE;

    AssertChkArg( f_peVerificationState != NULL );

    LOCK_REGISTERS( fGotLock );

    ChkDR( Oem_HalOemImpl_VerifyValidKeyRegister( OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC, f_keyRegisterIndex ) );

    *f_peVerificationState = g_rgKeyRegistersEccP256Public[f_keyRegisterIndex].validationInfo.eVerificationState;

ErrorExit:
    UNLOCK_REGISTERS( fGotLock );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_ECDSA_Verify_P256(
    __inout_ecount( 1 )                             DRM_CRYPTO_CONTEXT                  *f_pCryptoContext,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_indexSigningKey,
    __in_ecount( f_cbMessageLen )            const  DRM_BYTE                             f_rgbMessage[],
    __in                                     const  DRM_DWORD                            f_cbMessageLen,
    __in                                     const  SIGNATURE_P256                      *f_pSignature )
{
    DRM_RESULT dr   = DRM_SUCCESS;

    ChkDR( Oem_HalOemImpl_VerifyValidKeyRegister( OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC, f_indexSigningKey ) );

    ChkDR( OEM_ECDSA_Verify_P256(
        f_rgbMessage,
        f_cbMessageLen,
        &g_rgKeyRegistersEccP256Public[ f_indexSigningKey ].key,
        f_pSignature,
        ( struct bigctx_t* )f_pCryptoContext ) );

ErrorExit:
    return dr;
}

typedef union
{
    OEM_HAL_ECC256_KEY_PAIR   pEcc256Pair;
    OEM_HAL_ECC256_PUBLIC_KEY pEcc256Public;
    OEM_HAL_AES_KEY           pAes128;
    OEM_HAL_AES_KEY_PAIR      pAes128Pair;
    OEM_HAL_AES_ROOT_KEY_PAIR pAes128RootPair;
#if !DRM_SUPPORT_SECUREOEMHAL
    OEM_HAL_LEGACY_KEY        pLegacy;
#endif /* !DRM_SUPPORT_SECUREOEMHAL */
} OEM_HAL_UNKNOWN_KEY_TYPE;

static DRM_FRE_INLINE DRM_VOID _AssignEcc256KeyPair(
    __inout_ecount( 1 )                             OEM_HAL_RAW_KEY_REGISTER            *f_pRegisterInfo,
    __in_ecount( 1 )                                OEM_HAL_UNKNOWN_KEY_TYPE            *f_pKey ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_VOID _AssignEcc256KeyPair(
    __inout_ecount( 1 )                             OEM_HAL_RAW_KEY_REGISTER            *f_pRegisterInfo,
    __in_ecount( 1 )                                OEM_HAL_UNKNOWN_KEY_TYPE            *f_pKey )
{
    DRMASSERT( f_pKey          != (OEM_HAL_UNKNOWN_KEY_TYPE*)-1 );
    DRMASSERT( f_pKey          != NULL );
    DRMASSERT( f_pRegisterInfo != NULL );
    __analysis_assume( f_pKey          != NULL );
    __analysis_assume( f_pRegisterInfo != NULL );
    f_pRegisterInfo->eType                 = eOEM_HAL_KEYREGISTER_TYPE_P256_KEYPAIR;
    f_pRegisterInfo->keyBuffer.pEcc256Pair = &f_pKey->pEcc256Pair;
}

static DRM_FRE_INLINE DRM_VOID _AssignEcc256PublicKey(
    __inout_ecount( 1 )                             OEM_HAL_RAW_KEY_REGISTER            *f_pRegisterInfo,
    __in_ecount( 1 )                                OEM_HAL_UNKNOWN_KEY_TYPE            *f_pKey ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_VOID _AssignEcc256PublicKey(
    __inout_ecount( 1 )                             OEM_HAL_RAW_KEY_REGISTER            *f_pRegisterInfo,
    __in_ecount( 1 )                                OEM_HAL_UNKNOWN_KEY_TYPE            *f_pKey )
{
    DRMASSERT( f_pKey          != (OEM_HAL_UNKNOWN_KEY_TYPE*)-1 );
    DRMASSERT( f_pKey          != NULL );
    DRMASSERT( f_pRegisterInfo != NULL );
    __analysis_assume( f_pKey          != NULL );
    __analysis_assume( f_pRegisterInfo != NULL );
    f_pRegisterInfo->eType                   = eOEM_HAL_KEYREGISTER_TYPE_P256_PUBLIC;
    f_pRegisterInfo->keyBuffer.pEcc256Public = &f_pKey->pEcc256Public;
}

static DRM_FRE_INLINE DRM_VOID _AssignAes128Key(
    __inout_ecount( 1 )                             OEM_HAL_RAW_KEY_REGISTER            *f_pRegisterInfo,
    __in_ecount( 1 )                                OEM_HAL_UNKNOWN_KEY_TYPE            *f_pKey ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_VOID _AssignAes128Key(
    __inout_ecount( 1 )                             OEM_HAL_RAW_KEY_REGISTER            *f_pRegisterInfo,
    __in_ecount( 1 )                                OEM_HAL_UNKNOWN_KEY_TYPE            *f_pKey )
{
    DRMASSERT( f_pKey          != (OEM_HAL_UNKNOWN_KEY_TYPE*)-1 );
    DRMASSERT( f_pKey          != NULL );
    DRMASSERT( f_pRegisterInfo != NULL );
    __analysis_assume( f_pKey          != NULL );
    __analysis_assume( f_pRegisterInfo != NULL );
    f_pRegisterInfo->eType             = eOEM_HAL_KEYREGISTER_TYPE_AES128;
    f_pRegisterInfo->keyBuffer.pAes128 = &f_pKey->pAes128;
}

static DRM_FRE_INLINE DRM_VOID _AssignAes128KeyPair(
    __inout_ecount( 1 )                             OEM_HAL_RAW_KEY_REGISTER            *f_pRegisterInfo,
    __in_ecount( 1 )                                OEM_HAL_UNKNOWN_KEY_TYPE            *f_pKey ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_VOID _AssignAes128KeyPair(
    __inout_ecount( 1 )                             OEM_HAL_RAW_KEY_REGISTER            *f_pRegisterInfo,
    __in_ecount( 1 )                                OEM_HAL_UNKNOWN_KEY_TYPE            *f_pKey )
{
    DRMASSERT( f_pKey          != (OEM_HAL_UNKNOWN_KEY_TYPE*)-1 );
    DRMASSERT( f_pKey          != NULL );
    DRMASSERT( f_pRegisterInfo != NULL );
    __analysis_assume( f_pKey          != NULL );
    __analysis_assume( f_pRegisterInfo != NULL );
    f_pRegisterInfo->eType                 = eOEM_HAL_KEYREGISTER_TYPE_AES128_PAIR;
    f_pRegisterInfo->keyBuffer.pAes128Pair = &f_pKey->pAes128Pair;
}

static DRM_FRE_INLINE DRM_VOID _AssignAes128RootKeyPair(
    __inout_ecount( 1 )                             OEM_HAL_RAW_KEY_REGISTER             *f_pRegisterInfo,
    __in_ecount( 1 )                                OEM_HAL_UNKNOWN_KEY_TYPE             *f_pKey ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_VOID _AssignAes128RootKeyPair(
    __inout_ecount( 1 )                             OEM_HAL_RAW_KEY_REGISTER             *f_pRegisterInfo,
    __in_ecount( 1 )                                OEM_HAL_UNKNOWN_KEY_TYPE             *f_pKey )
{
    DRMASSERT( f_pKey          != (OEM_HAL_UNKNOWN_KEY_TYPE*)-1 );
    DRMASSERT( f_pKey          != NULL );
    DRMASSERT( f_pRegisterInfo != NULL );
    __analysis_assume( f_pKey          != NULL );
    __analysis_assume( f_pRegisterInfo != NULL );
    f_pRegisterInfo->eType                     = eOEM_HAL_KEYREGISTER_TYPE_AES128_ROOT_PAIR;
    f_pRegisterInfo->keyBuffer.pAes128RootPair = &f_pKey->pAes128RootPair;
}

#if !DRM_SUPPORT_SECUREOEMHAL
static DRM_FRE_INLINE DRM_VOID _AssignLegacyKey(
    __inout_ecount( 1 )                             OEM_HAL_RAW_KEY_REGISTER            *f_pRegisterInfo,
    __in_ecount( 1 )                                OEM_HAL_UNKNOWN_KEY_TYPE            *f_pKey ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_VOID _AssignLegacyKey(
    __inout_ecount( 1 )                             OEM_HAL_RAW_KEY_REGISTER            *f_pRegisterInfo,
    __in_ecount( 1 )                                OEM_HAL_UNKNOWN_KEY_TYPE            *f_pKey )
{
    /*
    ** Note to OEMs: Cocktail handling should not be included in OEM HAL unless explicitly required.
    ** This function should not exist if feature flag DRM_SUPPORT_BBXLEGACY == 0.
    */

    DRMASSERT( f_pKey          != (OEM_HAL_UNKNOWN_KEY_TYPE*)-1 );
    DRMASSERT( f_pKey          != NULL );
    DRMASSERT( f_pRegisterInfo != NULL );
    __analysis_assume( f_pKey          != NULL );
    __analysis_assume( f_pRegisterInfo != NULL );
    f_pRegisterInfo->eType             = eOEM_HAL_KEYREGISTER_TYPE_LEGACY;
    f_pRegisterInfo->keyBuffer.pLegacy = &f_pKey->pLegacy;
}
#endif /* !DRM_SUPPORT_SECUREOEMHAL */

static DRM_FRE_INLINE DRM_VOID _AssignNullKey(
    __inout_ecount( 1 )                             OEM_HAL_RAW_KEY_REGISTER            *f_pRegisterInfo,
    __in_ecount( 1 )                                OEM_HAL_UNKNOWN_KEY_TYPE            *f_pKey ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_VOID _AssignNullKey(
    __inout_ecount( 1 )                             OEM_HAL_RAW_KEY_REGISTER            *f_pRegisterInfo,
    __in_ecount( 1 )                                OEM_HAL_UNKNOWN_KEY_TYPE            *f_pKey )
{
    DRMASSERT( f_pRegisterInfo != NULL );
    __analysis_assume( f_pRegisterInfo != NULL );
    f_pRegisterInfo->eType                 = eOEM_HAL_KEYREGISTER_TYPE_INVALID;
    f_pRegisterInfo->keyBuffer.pEcc256Pair = NULL;
}

typedef DRM_VOID ( *_PFNAssignKey )( OEM_HAL_RAW_KEY_REGISTER *f_pRegisterInfo, OEM_HAL_UNKNOWN_KEY_TYPE *f_pKey );

typedef struct ___tagInitializeUnwrapContextData
{
    OEM_HAL_KEY_TYPE            eKeyType;
    OEM_HAL_KEY_TYPE            eWrappingKeyType;
    OEM_HAL_SLK_ENCRYPTION_TYPE eSlkCryptoType;
    OEM_HAL_CRYPTO_TYPE         eDecryptionType;
    OEM_HAL_KEYDATA_TYPE        eKeyDataType;
    _PFNAssignKey               pfnAssignTargetKey;
    _PFNAssignKey               pfnAssignWrappingKey;
} InitializeUnwrapContextData;

static InitializeUnwrapContextData g_rgInitializeUnwrapContextData[] = {
   /* eKeyType                                      eWrappingKeyType                            eSlkCryptoType                                  eDecryptionType                                 eKeyDataType                                pfnAssignTargetKey           pfnAssignWrappingKey */
    { OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,    OEM_HAL_KEY_TYPE_AES_KEYFILE,               OEM_HAL_SLK_ENCRYPTION_TYPE_INVALID,            eOEM_HAL_CRYPTO_TYPE_DEFAULT,                   eOEM_HAL_KEYDATA_TYPE_DRM_ENCRYPTED_KEY,    _AssignEcc256KeyPair,        _AssignAes128Key         },
    { OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT,     OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,  OEM_HAL_SLK_ENCRYPTION_TYPE_INVALID,            eOEM_HAL_CRYPTO_TYPE_DERIVED_SYMMETRIC_KEY,     eOEM_HAL_KEYDATA_TYPE_ENCRYPTED_BYTES,      _AssignEcc256KeyPair,        _AssignEcc256KeyPair     },
    { OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT,     OEM_HAL_KEY_TYPE_AES_ECB_DOMAIN_PROTECT,    OEM_HAL_SLK_ENCRYPTION_TYPE_INVALID,            eOEM_HAL_CRYPTO_TYPE_TWO_STAGE_ECB,             eOEM_HAL_KEYDATA_TYPE_ENCRYPTED_BYTES,      _AssignEcc256KeyPair,        _AssignAes128KeyPair     },
    { OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT,    OEM_HAL_KEY_TYPE_AES_KEYFILE,               OEM_HAL_SLK_ENCRYPTION_TYPE_INVALID,            eOEM_HAL_CRYPTO_TYPE_DEFAULT,                   eOEM_HAL_KEYDATA_TYPE_DRM_ENCRYPTED_KEY,    _AssignEcc256KeyPair,        _AssignAes128Key         },
    { OEM_HAL_KEY_TYPE_ECC_P256_SIGN,               OEM_HAL_KEY_TYPE_AES_KEYFILE,               OEM_HAL_SLK_ENCRYPTION_TYPE_INVALID,            eOEM_HAL_CRYPTO_TYPE_DEFAULT,                   eOEM_HAL_KEYDATA_TYPE_DRM_ENCRYPTED_KEY,    _AssignEcc256KeyPair,        _AssignAes128Key         },
    { OEM_HAL_KEY_TYPE_AES_ECB_DOMAIN_PROTECT,      OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,  OEM_HAL_SLK_ENCRYPTION_TYPE_INVALID,            eOEM_HAL_CRYPTO_TYPE_AES_KEY_PAIR,              eOEM_HAL_KEYDATA_TYPE_ENCRYPTED_BYTES,      _AssignAes128KeyPair,        _AssignEcc256KeyPair     },
    { OEM_HAL_KEY_TYPE_AES_ECB_ROOT,                OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,  OEM_HAL_SLK_ENCRYPTION_TYPE_INVALID,            eOEM_HAL_CRYPTO_TYPE_CHAINED_LICENSE_ECC256,    eOEM_HAL_KEYDATA_TYPE_XMR_LICENSE,          _AssignAes128RootKeyPair,    _AssignEcc256KeyPair     },
    { OEM_HAL_KEY_TYPE_AES_ECB_ROOT,                OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT,   OEM_HAL_SLK_ENCRYPTION_TYPE_INVALID,            eOEM_HAL_CRYPTO_TYPE_CHAINED_LICENSE_ECC256,    eOEM_HAL_KEYDATA_TYPE_XMR_LICENSE,          _AssignAes128RootKeyPair,    _AssignEcc256KeyPair     },
    { OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT,    OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT,      OEM_HAL_SLK_ENCRYPTION_TYPE_INVALID,            eOEM_HAL_CRYPTO_TYPE_AES_ECB,                   eOEM_HAL_KEYDATA_TYPE_ENCRYPTED_BYTES,      _AssignAes128RootKeyPair,    _AssignAes128Key         },
/* */ { OEM_HAL_KEY_TYPE_AES_CTR_CONTENT,             OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,  OEM_HAL_SLK_ENCRYPTION_TYPE_INVALID,            eOEM_HAL_CRYPTO_TYPE_SIMPLE_LICENSE_ECC256,     eOEM_HAL_KEYDATA_TYPE_XMR_LICENSE,          _AssignAes128KeyPair,        _AssignEcc256KeyPair     },
    { OEM_HAL_KEY_TYPE_AES_CTR_CONTENT,             OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT,   OEM_HAL_SLK_ENCRYPTION_TYPE_INVALID,            eOEM_HAL_CRYPTO_TYPE_SIMPLE_LICENSE_ECC256,     eOEM_HAL_KEYDATA_TYPE_XMR_LICENSE,          _AssignAes128KeyPair,        _AssignEcc256KeyPair     },
    { OEM_HAL_KEY_TYPE_AES_CTR_CONTENT,             OEM_HAL_KEY_TYPE_AES_ECB_ROOT,              OEM_HAL_SLK_ENCRYPTION_TYPE_INVALID,            eOEM_HAL_CRYPTO_TYPE_AES_ECB,                   eOEM_HAL_KEYDATA_TYPE_XMR_LICENSE,          _AssignAes128KeyPair,        _AssignAes128RootKeyPair },
    { OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT, OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT,   OEM_HAL_SLK_ENCRYPTION_TYPE_INVALID,            eOEM_HAL_CRYPTO_TYPE_AES_ECB,                   eOEM_HAL_KEYDATA_TYPE_ENCRYPTED_BYTES,      _AssignAes128KeyPair,        _AssignAes128Key         },
    { OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT,     OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT,  OEM_HAL_SLK_ENCRYPTION_TYPE_INVALID,            eOEM_HAL_CRYPTO_TYPE_AES_KEY_PAIR,              eOEM_HAL_KEYDATA_TYPE_ENCRYPTED_BYTES,      _AssignAes128KeyPair,        _AssignEcc256KeyPair     },
    { OEM_HAL_KEY_TYPE_AES_OMAC1_SECURE_STORE,      OEM_HAL_KEY_TYPE_AES_KEYFILE,               OEM_HAL_SLK_ENCRYPTION_TYPE_INVALID,            eOEM_HAL_CRYPTO_TYPE_DEFAULT,                   eOEM_HAL_KEYDATA_TYPE_DRM_ENCRYPTED_KEY,    _AssignAes128Key,            _AssignAes128Key         },
    { OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC,             OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC,           OEM_HAL_SLK_ENCRYPTION_TYPE_INVALID,            eOEM_HAL_CRYPTO_TYPE_DEFAULT,                   eOEM_HAL_KEYDATA_TYPE_XMR_CERT,             _AssignEcc256PublicKey,      _AssignEcc256PublicKey   },

#if !DRM_SUPPORT_SECUREOEMHAL
    /*
    ** Note to OEMs: SLK handling should not be included in OEM HAL unless explicitly required.
    ** These entries should be removed if feature flag DRM_SUPPORT_BBXSLK == 0.
    */
    { OEM_HAL_KEY_TYPE_AES_CTR_CONTENT,             OEM_HAL_KEY_TYPE_AES_SLK,                   OEM_HAL_SLK_ENCRYPTION_TYPE_INVALID,            eOEM_HAL_CRYPTO_TYPE_AES_ECB,                   eOEM_HAL_KEYDATA_TYPE_ENCRYPTED_BYTES,      _AssignAes128KeyPair,        _AssignAes128Key         },
    { OEM_HAL_KEY_TYPE_AES_SLK,                     OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,  OEM_HAL_SLK_ENCRYPTION_TYPE_ASYMMETRIC,         eOEM_HAL_CRYPTO_TYPE_AES_PADDED_KEY,            eOEM_HAL_KEYDATA_TYPE_ENCRYPTED_BYTES,      _AssignAes128Key,            _AssignEcc256KeyPair     },
    { OEM_HAL_KEY_TYPE_AES_SLK,                     OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,  OEM_HAL_SLK_ENCRYPTION_TYPE_DERIVED_SYMMETRIC,  eOEM_HAL_CRYPTO_TYPE_DERIVED_SYMMETRIC_KEY,     eOEM_HAL_KEYDATA_TYPE_ENCRYPTED_BYTES,      _AssignAes128Key,            _AssignEcc256KeyPair     },

    /*
    ** Note to OEMs: Cocktail handling should not be included in OEM HAL unless explicitly required.
    ** These entries should be removed if feature flag DRM_SUPPORT_BBXLEGACY == 0.
    */
    { OEM_HAL_KEY_TYPE_LEGACY,                      OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,  OEM_HAL_SLK_ENCRYPTION_TYPE_INVALID,            eOEM_HAL_CRYPTO_TYPE_SIMPLE_LICENSE_ECC256,     eOEM_HAL_KEYDATA_TYPE_XMR_LICENSE,          _AssignLegacyKey,            _AssignEcc256KeyPair     },
    { OEM_HAL_KEY_TYPE_LEGACY,                      OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT,   OEM_HAL_SLK_ENCRYPTION_TYPE_INVALID,            eOEM_HAL_CRYPTO_TYPE_SIMPLE_LICENSE_ECC256,     eOEM_HAL_KEYDATA_TYPE_XMR_LICENSE,          _AssignLegacyKey,            _AssignEcc256KeyPair     },
    { OEM_HAL_KEY_TYPE_LEGACY,                      OEM_HAL_KEY_TYPE_AES_ECB_ROOT,              OEM_HAL_SLK_ENCRYPTION_TYPE_INVALID,            eOEM_HAL_CRYPTO_TYPE_AES_ECB,                   eOEM_HAL_KEYDATA_TYPE_XMR_LICENSE,          _AssignLegacyKey,            _AssignAes128RootKeyPair },
#endif /* !DRM_SUPPORT_SECUREOEMHAL */
};

static DRM_FRE_INLINE OEM_HAL_UNKNOWN_KEY_TYPE* _MapKeyTypeIndexToKeyRegisterArrayEntry(
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_indexKey ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE OEM_HAL_UNKNOWN_KEY_TYPE* _MapKeyTypeIndexToKeyRegisterArrayEntry(
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_indexKey )
{
    DRMASSERT( ( f_eKeyType == OEM_HAL_KEY_TYPE_INVALID ) || DRM_SUCCEEDED( Oem_HalOemImpl_VerifyValidKeyRegister( f_eKeyType, f_indexKey ) ) );
    switch( f_eKeyType )
    {
    case OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT:
        return (OEM_HAL_UNKNOWN_KEY_TYPE*)&g_rgKeyRegistersEccP256ContentEncrypt[f_indexKey];
    case OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT:
        return (OEM_HAL_UNKNOWN_KEY_TYPE*)&g_rgKeyRegistersEccP256DomainEncrypt[f_indexKey];
    case OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT:
        return (OEM_HAL_UNKNOWN_KEY_TYPE*)&g_rgKeyRegistersEccP256MessageEncrypt[f_indexKey];
    case OEM_HAL_KEY_TYPE_ECC_P256_SIGN:
        return (OEM_HAL_UNKNOWN_KEY_TYPE*)&g_rgKeyRegistersEccP256Sign[f_indexKey];
    case OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC:
        return (OEM_HAL_UNKNOWN_KEY_TYPE*)&g_rgKeyRegistersEccP256Public[f_indexKey];
    case OEM_HAL_KEY_TYPE_ECC_P256_MODEL_SIGN:
        return (OEM_HAL_UNKNOWN_KEY_TYPE*)&g_rgKeyRegistersEccP256ModelSign[f_indexKey];
    case OEM_HAL_KEY_TYPE_AES_ECB_ROOT:
        return (OEM_HAL_UNKNOWN_KEY_TYPE*)&g_rgKeyRegistersAesEcbRoot[f_indexKey];
    case OEM_HAL_KEY_TYPE_AES_CTR_CONTENT:
        return (OEM_HAL_UNKNOWN_KEY_TYPE*)&g_rgKeyRegistersAesCtrContent[f_indexKey];
    case OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT:
        return (OEM_HAL_UNKNOWN_KEY_TYPE*)&g_rgKeyRegistersAesEcbMessageEncrypt[f_indexKey];
    case OEM_HAL_KEY_TYPE_AES_ECB_DOMAIN_PROTECT:
        return (OEM_HAL_UNKNOWN_KEY_TYPE*)&g_rgKeyRegistersAesEcbDomainProtect[f_indexKey];
    case OEM_HAL_KEY_TYPE_AES_OMAC1_SECURE_STORE:
        return (OEM_HAL_UNKNOWN_KEY_TYPE*)&g_rgKeyRegistersAesOmac1SecureStore[f_indexKey];
    case OEM_HAL_KEY_TYPE_AES_KEYFILE:
        return (OEM_HAL_UNKNOWN_KEY_TYPE*)&g_rgKeyRegistersAesKeyFile[f_indexKey];
#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    case OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT:
        return (OEM_HAL_UNKNOWN_KEY_TYPE*)&g_rgKeyRegistersAesGenericKeyContent[f_indexKey];
#endif /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    case OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT:
        return (OEM_HAL_UNKNOWN_KEY_TYPE*)&g_rgKeyRegistersAesGenericKeyRoot[f_indexKey];
#endif /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    case OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT:
        return (OEM_HAL_UNKNOWN_KEY_TYPE*)&g_rgKeyRegistersAesEcbNewLicenseRoot[f_indexKey];
#endif /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    case OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT:
        return (OEM_HAL_UNKNOWN_KEY_TYPE*)&g_rgKeyRegistersAesCtrNewLicenseContent[f_indexKey];
#endif /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
#if !DRM_SUPPORT_SECUREOEMHAL
    case OEM_HAL_KEY_TYPE_LEGACY:
        return (OEM_HAL_UNKNOWN_KEY_TYPE*)&g_rgKeyRegistersLegacy[f_indexKey];
#endif /* !DRM_SUPPORT_SECUREOEMHAL */
#if !DRM_SUPPORT_SECUREOEMHAL
    case OEM_HAL_KEY_TYPE_AES_SLK:
        return (OEM_HAL_UNKNOWN_KEY_TYPE*)&g_rgKeyRegistersAesSlk[f_indexKey];
#endif /* !DRM_SUPPORT_SECUREOEMHAL */
    case OEM_HAL_KEY_TYPE_INVALID:
        return (OEM_HAL_UNKNOWN_KEY_TYPE*)-1;
    default:
        return NULL;
    }
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_InitializeUnwrapContext(
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType, /* type of clear key*/
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_indexKey, /* index of clear key*/
    __in                                            OEM_HAL_KEY_TYPE                     f_eWrappingKeyType, /* private Key Type (Decryption Key of Key)*/
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_indexWrappingKey, /* private Key Index (Decryption Key of Key)*/
    __out_ecount( 1 )                               OEM_HAL_UNWRAP_CONTEXT              *f_pUnwrapContext,
    __in_bcount_opt( f_cbParameterData )      const DRM_BYTE                            *f_pbParameterData,
    __in                                            DRM_DWORD                            f_cbParameterData )
{
    DRM_RESULT dr  = DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT;  /* MUST be initialized to an error code. */
    DRM_DWORD  idx = 0;

    OEM_HAL_UNKNOWN_KEY_TYPE  *pTargetKey   = _MapKeyTypeIndexToKeyRegisterArrayEntry( f_eKeyType,         f_indexKey );
    OEM_HAL_UNKNOWN_KEY_TYPE  *pWrappingKey = _MapKeyTypeIndexToKeyRegisterArrayEntry( f_eWrappingKeyType, f_indexWrappingKey );
/* { OEM_HAL_KEY_TYPE_AES_CTR_CONTENT, OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,  OEM_HAL_SLK_ENCRYPTION_TYPE_INVALID, eOEM_HAL_CRYPTO_TYPE_SIMPLE_LICENSE_ECC256, eOEM_HAL_KEYDATA_TYPE_XMR_LICENSE, _AssignAes128KeyPair, _AssignEcc256KeyPair 	},*/
    if( pTargetKey != NULL && pWrappingKey != NULL )
    {
        for( idx = 0; idx < NO_OF( g_rgInitializeUnwrapContextData ) && DRM_FAILED( dr ); idx++ )
        {
            if( g_rgInitializeUnwrapContextData[idx].eKeyType         != f_eKeyType
             || g_rgInitializeUnwrapContextData[idx].eWrappingKeyType != f_eWrappingKeyType )
            {
                continue;
            }
            if( ( g_rgInitializeUnwrapContextData[idx].eSlkCryptoType != OEM_HAL_SLK_ENCRYPTION_TYPE_INVALID )
             && ( ( SIZEOF( OEM_HAL_SLK_ENCRYPTION_TYPE ) != f_cbParameterData )
               || ( g_rgInitializeUnwrapContextData[idx].eSlkCryptoType != *( ( OEM_HAL_SLK_ENCRYPTION_TYPE* )f_pbParameterData ) ) ) )
            {
                continue;
            }
            f_pUnwrapContext->eDecryptionType = g_rgInitializeUnwrapContextData[idx].eDecryptionType; /*eOEM_HAL_CRYPTO_TYPE_SIMPLE_LICENSE_ECC256*/
            f_pUnwrapContext->eKeyDataType    = g_rgInitializeUnwrapContextData[idx].eKeyDataType;    /*eOEM_HAL_KEYDATA_TYPE_XMR_LICENSE*/
            g_rgInitializeUnwrapContextData[idx].pfnAssignTargetKey(   &f_pUnwrapContext->targetKey,   pTargetKey );
            g_rgInitializeUnwrapContextData[idx].pfnAssignWrappingKey( &f_pUnwrapContext->wrappingKey, pWrappingKey );
            dr = DRM_SUCCESS;
        }
    }
    return dr;
}

typedef struct ___tagInitializeWrapContextData
{
    OEM_HAL_KEY_TYPE            eKeyType;
    OEM_HAL_KEY_TYPE            eWrappingKeyType;
    OEM_HAL_CRYPTO_TYPE         eEncryptionType;
    _PFNAssignKey               pfnAssignTargetKey;
    _PFNAssignKey               pfnAssignWrappingKey;
    DRM_BOOL                    fRequirePublicKey;
} InitializeWrapContextData;

static InitializeWrapContextData g_rgInitializeWrapContextData[] = {
   /* eKeyType                                      eWrappingKeyType                            eEncryptionType                                 pfnAssignTargetKey           pfnAssignWrappingKey         fRequirePublicKey */
    { OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,    OEM_HAL_KEY_TYPE_AES_KEYFILE,               eOEM_HAL_CRYPTO_TYPE_AES_CBC,                   _AssignEcc256KeyPair,        _AssignAes128Key,            FALSE   },
    { OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT,     OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,  eOEM_HAL_CRYPTO_TYPE_DERIVED_SYMMETRIC_KEY,     _AssignEcc256KeyPair,        _AssignEcc256KeyPair,        FALSE   },
    { OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT,    OEM_HAL_KEY_TYPE_AES_KEYFILE,               eOEM_HAL_CRYPTO_TYPE_AES_CBC,                   _AssignEcc256KeyPair,        _AssignAes128Key,            FALSE   },
    { OEM_HAL_KEY_TYPE_ECC_P256_SIGN,               OEM_HAL_KEY_TYPE_AES_KEYFILE,               eOEM_HAL_CRYPTO_TYPE_AES_CBC,                   _AssignEcc256KeyPair,        _AssignAes128Key,            FALSE   },
#if !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
    /*
    ** Rebinding licenses, generating new licenses, and generating ECB message encryption keys
    ** are all unsupported features when SecureHalPlayOnly is enabled.
    ** (The last is only required for PRND transmitters, not receivers.)
    */
    { OEM_HAL_KEY_TYPE_AES_ECB_ROOT,                OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC,           eOEM_HAL_CRYPTO_TYPE_ECC256_PUBLIC_KEY,         _AssignAes128RootKeyPair,    _AssignEcc256PublicKey,      FALSE   },
    { OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT,    OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC,           eOEM_HAL_CRYPTO_TYPE_ECC256_PUBLIC_KEY,         _AssignAes128RootKeyPair,    _AssignEcc256PublicKey,      FALSE   },
    { OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT,    OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT,      eOEM_HAL_CRYPTO_TYPE_AES_ECB,                   _AssignAes128RootKeyPair,    _AssignAes128Key,            FALSE   },
    { OEM_HAL_KEY_TYPE_AES_CTR_CONTENT,             OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC,           eOEM_HAL_CRYPTO_TYPE_ECC256_PUBLIC_KEY,         _AssignAes128KeyPair,        _AssignEcc256PublicKey,      FALSE   },
    { OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT, OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC,           eOEM_HAL_CRYPTO_TYPE_ECC256_PUBLIC_KEY,         _AssignAes128KeyPair,        _AssignEcc256PublicKey,      FALSE   },
    { OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT, OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT,  eOEM_HAL_CRYPTO_TYPE_AES_ECB,                   _AssignAes128KeyPair,        _AssignAes128RootKeyPair,    FALSE   },
    { OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT, OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT,   eOEM_HAL_CRYPTO_TYPE_AES_ECB,                   _AssignAes128KeyPair,        _AssignAes128Key,            FALSE   },
    { OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT,     OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC,           eOEM_HAL_CRYPTO_TYPE_ECC256_PUBLIC_KEY,         _AssignAes128KeyPair,        _AssignEcc256PublicKey,      FALSE   },
#endif /* !DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY */
    { OEM_HAL_KEY_TYPE_AES_OMAC1_SECURE_STORE,      OEM_HAL_KEY_TYPE_AES_KEYFILE,               eOEM_HAL_CRYPTO_TYPE_AES_CBC,                   _AssignAes128Key,            _AssignAes128Key,            FALSE   },
    { OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,    OEM_HAL_KEY_TYPE_INVALID,                   eOEM_HAL_CRYPTO_TYPE_PASSTHROUGH,               _AssignEcc256KeyPair,        _AssignNullKey,              TRUE    },
    { OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT,     OEM_HAL_KEY_TYPE_INVALID,                   eOEM_HAL_CRYPTO_TYPE_PASSTHROUGH,               _AssignEcc256KeyPair,        _AssignNullKey,              TRUE    },
    { OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT,    OEM_HAL_KEY_TYPE_INVALID,                   eOEM_HAL_CRYPTO_TYPE_PASSTHROUGH,               _AssignEcc256KeyPair,        _AssignNullKey,              TRUE    },
    { OEM_HAL_KEY_TYPE_ECC_P256_SIGN,               OEM_HAL_KEY_TYPE_INVALID,                   eOEM_HAL_CRYPTO_TYPE_PASSTHROUGH,               _AssignEcc256KeyPair,        _AssignNullKey,              TRUE    },

#if !DRM_SUPPORT_SECUREOEMHAL
    /*
    ** Note to OEMs: Symmetric optimization handling should not be included in OEM HAL unless explicitly required.
    ** These entries should be removed if feature flag DRM_SUPPORT_SYMOPT == 0.
    */
    { OEM_HAL_KEY_TYPE_AES_ECB_ROOT,                OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,  eOEM_HAL_CRYPTO_TYPE_DERIVED_SYMMETRIC_KEY,     _AssignAes128RootKeyPair,    _AssignEcc256KeyPair,        FALSE   },
    { OEM_HAL_KEY_TYPE_AES_ECB_ROOT,                OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT,   eOEM_HAL_CRYPTO_TYPE_DERIVED_SYMMETRIC_KEY,     _AssignAes128RootKeyPair,    _AssignEcc256KeyPair,        FALSE   },
    { OEM_HAL_KEY_TYPE_AES_CTR_CONTENT,             OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,  eOEM_HAL_CRYPTO_TYPE_DERIVED_SYMMETRIC_KEY,     _AssignAes128KeyPair,        _AssignEcc256KeyPair,        FALSE   },
    { OEM_HAL_KEY_TYPE_AES_CTR_CONTENT,             OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT,   eOEM_HAL_CRYPTO_TYPE_DERIVED_SYMMETRIC_KEY,     _AssignAes128KeyPair,        _AssignEcc256KeyPair,        FALSE   },

    /*
    ** Note to OEMs: Cocktail handling should not be included in OEM HAL unless explicitly required.
    ** These entries should be removed if feature flag DRM_SUPPORT_BBXLEGACY == 0.
    */
    { OEM_HAL_KEY_TYPE_LEGACY,                      OEM_HAL_KEY_TYPE_INVALID,                   eOEM_HAL_CRYPTO_TYPE_PASSTHROUGH,               _AssignLegacyKey,            _AssignNullKey,              FALSE   },

    /*
    ** Note to OEMs: SLK handling should not be included in OEM HAL unless explicitly required.
    ** These entries should be removed if feature flag DRM_SUPPORT_BBXSLK == 0.
    */
    { OEM_HAL_KEY_TYPE_AES_SLK,                     OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,  eOEM_HAL_CRYPTO_TYPE_DERIVED_SYMMETRIC_KEY,     _AssignAes128Key,            _AssignEcc256KeyPair,        FALSE   },

    /*
    ** Note to OEMs: Cocktail handling AND Symmetric optimization handling should not be included in OEM HAL unless explicitly required.
    ** These entries should be removed if feature flag DRM_SUPPORT_BBXLEGACY == 0 *OR* DRM_SUPPORT_SYMOPT == 0.
    */
    { OEM_HAL_KEY_TYPE_LEGACY,                      OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,  eOEM_HAL_CRYPTO_TYPE_DERIVED_SYMMETRIC_KEY,     _AssignLegacyKey,            _AssignEcc256KeyPair,        FALSE   },
    { OEM_HAL_KEY_TYPE_LEGACY,                      OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT,   eOEM_HAL_CRYPTO_TYPE_DERIVED_SYMMETRIC_KEY,     _AssignLegacyKey,            _AssignEcc256KeyPair,        FALSE   },
#endif /* !DRM_SUPPORT_SECUREOEMHAL */
};

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_InitializeWrapContext(
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_indexKey,
    __in                                            OEM_HAL_KEY_TYPE                     f_eWrappingKeyType,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_indexWrappingKey,
    __out_ecount( 1 )                               OEM_HAL_WRAP_CONTEXT                *f_pWrapContext )
{
    DRM_RESULT dr  = DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT;  /* MUST be initialized to an error code. */
    DRM_DWORD  idx = 0;

    OEM_HAL_UNKNOWN_KEY_TYPE  *pTargetKey   = _MapKeyTypeIndexToKeyRegisterArrayEntry( f_eKeyType,         f_indexKey );
    OEM_HAL_UNKNOWN_KEY_TYPE  *pWrappingKey = _MapKeyTypeIndexToKeyRegisterArrayEntry( f_eWrappingKeyType, f_indexWrappingKey );

    if( pTargetKey != NULL && pWrappingKey != NULL )
    {
        for( idx = 0; idx < NO_OF( g_rgInitializeWrapContextData ) && DRM_FAILED( dr ); idx++ )
        {
            if( g_rgInitializeWrapContextData[idx].eKeyType         != f_eKeyType
             || g_rgInitializeWrapContextData[idx].eWrappingKeyType != f_eWrappingKeyType )
            {
                continue;
            }
            if( g_rgInitializeWrapContextData[idx].fRequirePublicKey )
            {
                DRMASSERT( pTargetKey != (OEM_HAL_UNKNOWN_KEY_TYPE*)-1 );
                if( !pTargetKey->pEcc256Pair.fPublicKeyPresent )
                {
                    continue;
                }
            }
            f_pWrapContext->eEncryptionType = g_rgInitializeWrapContextData[idx].eEncryptionType;
            g_rgInitializeWrapContextData[idx].pfnAssignTargetKey(   &f_pWrapContext->targetKey,   pTargetKey );
            g_rgInitializeWrapContextData[idx].pfnAssignWrappingKey( &f_pWrapContext->wrappingKey, pWrappingKey );
            dr = DRM_SUCCESS;
        }
    }
    return dr;
}

/*********************************************************************
**
** Function: Oem_HalOemImpl_WrapOrUnwrapWithSymmetricCipherRaw
**
** Synopsis: Function that does symmetric ciphering (encrypt or decrypt)
**           of the passed in data using a symmetric key, which
**           is a key derived from a private key.
**
**           The first part of the private key is used to hash a constant
**           with OMAC1, and the hash is used as an AES key to cipher the data.
**
** Arguments:
** [f_fEncrypt]         : Specifies that encryption should be done instead of decryption
** [f_pUnwrapContext]   : Unwrap context in which to find the key to hash to the symmetric key.
**                        Should not be specified with f_pWrapContext.
** [f_pWrapContext]     : Wrap context in which to find the key to hash to the symmetric key.
**                        Should not be specified with f_pUnwrapContext.
** [f_pbDataIn]         : Pointer to a buffer of data to be
**                        ciphered.
** [f_pbDataOut]        : Pointer to a buffer that the result of
**                        the ciphering is stored. If NULL, ciphering
**                        will be done in place.
** [f_cbData]           : Size (number of bytes) of both the input
**                        and output buffers. The size of the input
**                        buffer and output buffer is the same
**                        since symmetric ciphering is used.
**
** Returns:
**      DRM_SUCCESS
**          Success.
**      DRM_E_INVALIDARG
**          One of the arguments is invalid or improperly initialized
**
** Notes:
**      Only f_pUnwrapContext OR f_pWrapContext should be specified, not both.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_WrapOrUnwrapWithSymmetricCipherRaw(
    __in                                             DRM_BOOL                            f_fEncrypt,
    __in_opt                                   const OEM_HAL_UNWRAP_CONTEXT             *f_pUnwrapContext,
    __in_opt                                   const OEM_HAL_WRAP_CONTEXT               *f_pWrapContext,
    __in_bcount( f_cbData )                    const DRM_BYTE                           *f_pbDataIn,
    __out_bcount( f_cbData )                         DRM_BYTE                           *f_pbDataOut,
    __in                                             DRM_DWORD                           f_cbData )
{
    DRM_RESULT                   dr                                                                             = DRM_SUCCESS;
    DRM_BYTE                    *pbOutputKeyAligned                                                             = NULL;
    DRM_STACK_ALLOCATOR_CONTEXT  stkContext                                                                     = {0};
    DRM_AES_KEY                  aesKey;                                                                                /* Don't initialize key material */
    DRM_BYTE                     rgbOutputKeyUnalignedStackSpace[ DRM_AES_BLOCKLEN + SIZEOF(DRM_DWORD_PTR) ];           /* Don't initialize key material */
    const DRM_BYTE               rgbConstantBlock[DRM_AES_BLOCKLEN]                                             = {0};  /* MUST be zero-initialized */
    const DRM_BYTE              *pbKey                                                                          = NULL;

    ChkPtr( f_pbDataIn );
    ChkPtr( f_pbDataOut );
    ChkArg( f_cbData >  0 );
    ChkArg( ( f_pUnwrapContext == NULL ) != ( f_pWrapContext == NULL ));

    if( f_pUnwrapContext != NULL )
    {
        pbKey = (const DRM_BYTE*)&f_pUnwrapContext->wrappingKey.keyBuffer.pEcc256Pair->privateKey;
    }
    else
    {
        pbKey = (const DRM_BYTE*)&f_pWrapContext->wrappingKey.keyBuffer.pEcc256Pair->privateKey;
    }

    OEM_SECURE_MEMCPY( f_pbDataOut, f_pbDataIn, f_cbData );

    /*
    **  1. Get an OMAC1 of the privkey
    **  2. Generate an AES key using this hash
    **  3. Decrypt or encrypt ciphertext using this AES key
    */

    /*
    ** pbOutputKeyAligned will be cast to a DWORD aligned
    ** structure in Oem_Aes_SetKey so make sure pbOutputKeyAligned
    ** is DWORD aligned.
    */
    ChkDR( DRM_STK_Init( &stkContext, rgbOutputKeyUnalignedStackSpace, SIZEOF( rgbOutputKeyUnalignedStackSpace ) ) );

    ChkDR( DRM_STK_Alloc_Aligned(
        &stkContext,
        DRM_AES_BLOCKLEN,
        SIZEOF( DRM_DWORD ),
        NULL,
        ( DRM_VOID **) &pbOutputKeyAligned ) );


    ChkDR( Oem_Aes_SetKey( pbKey, &aesKey ) );
    ChkDR( Oem_Omac1_Sign( &aesKey, rgbConstantBlock, 0, SIZEOF(rgbConstantBlock), pbOutputKeyAligned ) );
    ChkDR( Oem_Aes_ZeroKey( &aesKey ) );


    ChkDR( Oem_Aes_SetKey( pbOutputKeyAligned, &aesKey ) );

    if( f_fEncrypt )
    {
        ChkDR( Oem_Aes_EcbEncryptData( &aesKey, f_pbDataOut, f_cbData ) );
    }
    else
    {
        ChkDR( Oem_Aes_EcbDecryptData( &aesKey, f_pbDataOut, f_cbData ) );
    }

ErrorExit:
    (DRM_VOID)Oem_Aes_ZeroKey( &aesKey );
    OEM_SECURE_ZERO_MEMORY( rgbOutputKeyUnalignedStackSpace, SIZEOF( rgbOutputKeyUnalignedStackSpace ) );
    /* Note: no need to zero pbOutputKeyAligned because it points into rgbOutputKeyUnalignedStackSpace */
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_UnwrapContextGetMetaData(
    __in                                      const OEM_HAL_UNWRAP_CONTEXT              *f_pUnwrapContext,
    __out_ecount_opt( 1 )                           OEM_HAL_KEYDATA_TYPE                *f_peKeyDataType,
    __out_ecount_opt( 1 )                           OEM_HAL_CRYPTO_TYPE                 *f_peDecryptionType,
    __out_ecount_opt( 1 )                           OEM_HAL_KEYREGISTER_TYPE            *f_peWrappingKeyType,
    __out_ecount_opt( 1 )                           OEM_HAL_KEYREGISTER_TYPE            *f_peTargetKeyType )
{
    DRM_RESULT dr = DRM_SUCCESS;

    AssertChkArg( f_pUnwrapContext != NULL );

    if( f_peKeyDataType != NULL )
    {
        *f_peKeyDataType = f_pUnwrapContext->eKeyDataType;
    }
    if( f_peDecryptionType != NULL )
    {
        *f_peDecryptionType = f_pUnwrapContext->eDecryptionType;
    }
    if( f_peWrappingKeyType != NULL )
    {
        *f_peWrappingKeyType = f_pUnwrapContext->wrappingKey.eType;
    }
    if( f_peTargetKeyType != NULL )
    {
        *f_peTargetKeyType = f_pUnwrapContext->targetKey.eType;
    }

ErrorExit:
    return dr;
}


/*****************************************************************************
** Function:    Oem_HalOemImpl_UnwrapEncryptedKey
**
** Synopsis:    Decrypts data (a key) using a key stored in keyfile.
**
** Arguments:   [f_eKeyType]        : The expected key type for the unwrapped key
**              [f_pUnwrapContext]  : A pointer to an initialized unwrap context
**              [f_pEncryptedKey]   : A structure containing encrypted key data
**
** Returns:     DRM_SUCCESS          - on success
**              DRM_E_PTR            - if the input parameters are NULL
**              DRM_E_INVALIDARG - if the unwrap context is incorrectly initialized
**              DRM_E_OEMHAL_UNSUPPORTED_KEY_LENGTH - if the key data size is not
**              compatible with the key type
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_UnwrapEncryptedKey(
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in                                      const OEM_HAL_UNWRAP_CONTEXT              *f_pUnwrapContext,
    __in                                      const DRM_ENCRYPTED_KEY                   *f_pEncryptedKey )
{
    DRM_RESULT  dr                                           = DRM_SUCCESS;
    DRM_BYTE    rgbDecryptBuffer[ KF_MAX_ENCRYPTION_BUFFER ];  /* Don't initialize key material */
    DRM_DWORD   cbKeySize                                    = 0;
    DRM_DWORD   cbSignedKeySize                              = 0;
    DRM_BYTE   *pbTargetBuffer                               = NULL;
    DRM_DWORD   dwType                                       = 0;

    ChkPtr( f_pUnwrapContext );
    ChkPtr( f_pEncryptedKey );

    /*
    ** The unwrap context should be initialized, run a few checks anyway
    */
    ChkArg( f_pUnwrapContext->eKeyDataType      != eOEM_HAL_KEYDATA_TYPE_INVALID );
    ChkArg( f_pUnwrapContext->wrappingKey.eType != eOEM_HAL_KEYREGISTER_TYPE_INVALID );
    ChkArg( f_pUnwrapContext->targetKey.eType   == eOEM_HAL_KEYREGISTER_TYPE_P256_KEYPAIR
         || f_pUnwrapContext->targetKey.eType   == eOEM_HAL_KEYREGISTER_TYPE_AES128 );
    ChkArg( f_pUnwrapContext->eDecryptionType   == eOEM_HAL_CRYPTO_TYPE_DEFAULT );

    switch( f_pUnwrapContext->targetKey.eType )
    {
    case eOEM_HAL_KEYREGISTER_TYPE_P256_KEYPAIR:
        cbKeySize      = SIZEOF( PRIVKEY_P256 );
        pbTargetBuffer = ( DRM_BYTE* )&f_pUnwrapContext->targetKey.keyBuffer.pEcc256Pair->privateKey;
        break;

    case eOEM_HAL_KEYREGISTER_TYPE_AES128:
        cbKeySize      = DRM_AES_KEYSIZE_128;
        pbTargetBuffer = ( DRM_BYTE* )&f_pUnwrapContext->targetKey.keyBuffer.pAes128->rgbKey;
        break;

    default:
        ChkDR( DRM_E_INVALIDARG );
    }

    /*
    ** Key type plus encrypted key data plus sizeof AES signature
    */
    cbSignedKeySize = SIZEOF(DRM_DWORD) + cbKeySize + DRM_AES_BLOCKLEN;

    /*
    ** If the Key is encrypted, get the encryption KeyID and then decrypt the key.
    */
    switch( f_pEncryptedKey->wKeypairDecryptionMethod )
    {
        case KF_DECRYPTION_TYPE_AES_CBC_SIG:
            DRMASSERT( f_pUnwrapContext->wrappingKey.eType == eOEM_HAL_KEYREGISTER_TYPE_AES128 );
            ChkBOOL( ( DRM_DWORD ) ( f_pEncryptedKey->wKeySize / BITS_PER_BYTE ) == cbSignedKeySize, DRM_E_OEMHAL_UNSUPPORTED_KEY_LENGTH );

            /*
            ** Copy the data into the temp output buffer. Then Decrypt it and return the correct portion.
            */
            if( f_pEncryptedKey->cbEncryptedKey > KF_MAX_ENCRYPTION_BUFFER )
            {
                TRACE(("The key in the keyfile is too large to load into the decryption buffer."));
                ChkDR( DRM_E_KEYFILE_INTERNAL_DECRYPTION_BUFFER_TOO_SMALL );
            }

            OEM_SECURE_MEMCPY( rgbDecryptBuffer, f_pEncryptedKey->pbEncryptedKey, f_pEncryptedKey->cbEncryptedKey );

            /*
            ** Verify the signature over the keyfile's key type plus the encrypted key
            */
            ChkDR( Oem_Omac1_Verify(
                &f_pUnwrapContext->wrappingKey.keyBuffer.pAes128->expandedKey.rgExpandedKey,
                rgbDecryptBuffer,
                0,
                SIZEOF(DRM_DWORD) + cbKeySize,
                &rgbDecryptBuffer[SIZEOF(DRM_DWORD) + cbKeySize],
                0 ) );

            /*
            ** Verify that f_eKeyType matches the one from the keyfile
            ** This, plus the signature check, prevents key repurposing
            */
            NETWORKBYTES_TO_DWORD( dwType, rgbDecryptBuffer, 0 );
            ChkBOOL( dwType == (DRM_DWORD)f_eKeyType, DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );

            /*
            ** Now decrypt the actual key and hand it back to the caller
            */
            ChkDR( _CheckAesKeyExpanded(
                f_pUnwrapContext->wrappingKey.keyBuffer.pAes128->rgbKey,
                &f_pUnwrapContext->wrappingKey.keyBuffer.pAes128->expandedKey ) );

            ChkDR( Oem_Aes_CbcDecryptData(
                &f_pUnwrapContext->wrappingKey.keyBuffer.pAes128->expandedKey.rgExpandedKey,
                &rgbDecryptBuffer[SIZEOF(DRM_DWORD)],
                cbKeySize,
                f_pEncryptedKey->rgbInitializationVector ) );

            OEM_SECURE_MEMCPY( pbTargetBuffer, &rgbDecryptBuffer[SIZEOF(DRM_DWORD)], cbKeySize );
            break;

/*
** NOTE: This is the ONLY place where DRM_TEST_EXPOSE_TEST_FUNCTIONS should be used to modify
** existing, production functions rather than expose new, test-specific functions.
*/
#if OEM_SUPPORT_KEY_HISTORY_PRE_PK_2_5 || DRM_TEST_EXPOSE_TEST_FUNCTIONS
        case KF_DECRYPTION_TYPE_AES_CBC:
            /*
            ** Note to OEMs: Refer to the declaration of OEM_SUPPORT_KEY_HISTORY_PRE_PK_2_5 at the top of this file.
            */
            DRMASSERT( f_pUnwrapContext->wrappingKey.eType == eOEM_HAL_KEYREGISTER_TYPE_AES128 );
            ChkBOOL( ( DRM_DWORD ) ( f_pEncryptedKey->wKeySize / BITS_PER_BYTE ) == cbKeySize, DRM_E_OEMHAL_UNSUPPORTED_KEY_LENGTH );

            /*
            ** Copy the data into the temp output buffer. then Decrypt it and return the correct portion.
            */
            if( f_pEncryptedKey->cbEncryptedKey > KF_MAX_ENCRYPTION_BUFFER )
            {
                TRACE(("The key in the keyfile is too large to load into the decryption buffer."));
                ChkDR( DRM_E_KEYFILE_INTERNAL_DECRYPTION_BUFFER_TOO_SMALL );
            }

            OEM_SECURE_MEMCPY( rgbDecryptBuffer, f_pEncryptedKey->pbEncryptedKey, f_pEncryptedKey->cbEncryptedKey );

            ChkDR( _CheckAesKeyExpanded(
                f_pUnwrapContext->wrappingKey.keyBuffer.pAes128->rgbKey,
                &f_pUnwrapContext->wrappingKey.keyBuffer.pAes128->expandedKey ) );

            ChkDR( Oem_Aes_CbcDecryptData(
                &f_pUnwrapContext->wrappingKey.keyBuffer.pAes128->expandedKey.rgExpandedKey,
                rgbDecryptBuffer,
                f_pEncryptedKey->cbEncryptedKey,
                f_pEncryptedKey->rgbInitializationVector ) );

            OEM_SECURE_MEMCPY( pbTargetBuffer, rgbDecryptBuffer, cbKeySize );
            break;

        case KF_DECRYPTION_TYPE_CLEAR:
            /*
            ** Note to OEMs: Refer to the declaration of OEM_SUPPORT_KEY_HISTORY_PRE_PK_2_5 at the top of this file.
            */
            ChkBOOL( ( DRM_DWORD ) ( f_pEncryptedKey->wKeySize / BITS_PER_BYTE ) == cbKeySize, DRM_E_OEMHAL_UNSUPPORTED_KEY_LENGTH );
            ChkBOOL( cbKeySize == f_pEncryptedKey->cbEncryptedKey, DRM_E_OEMHAL_UNSUPPORTED_KEY_LENGTH );
            OEM_SECURE_MEMCPY( pbTargetBuffer, f_pEncryptedKey->pbEncryptedKey, f_pEncryptedKey->cbEncryptedKey );
            break;
#endif  /* OEM_SUPPORT_KEY_HISTORY_PRE_PK_2_5 || DRM_TEST_EXPOSE_TEST_FUNCTIONS */

        default:
            ChkDR( DRM_E_KEYFILE_UNKNOWN_DECRYPTION_METHOD );
    }

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( rgbDecryptBuffer, SIZEOF( rgbDecryptBuffer ) );
    return dr;
}

/*****************************************************************************
** Function:    Oem_HalOemImpl_UnwrapWithSymmetricCipher
**
** Synopsis:    Decrypts a key using an AES key derived from a private key.
**
** Arguments:   [f_pUnwrapContext]   : A pointer to an initialized unwrap context
**              [f_pbWrappedKeyData] : A buffer containing encrypted key data
**              [f_cbWrappedKeyData] : the length of the data in f_pbWrappedKeyData
**
** Returns:     DRM_SUCCESS                         - on success
**              DRM_E_OEMHAL_UNSUPPORTED_KEY_LENGTH - if the key data is invalid
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_UnwrapWithSymmetricCipher(
    __in                                            OEM_HAL_UNWRAP_CONTEXT              *f_pUnwrapContext,
    __in_bcount( f_cbWrappedKeyData )         const DRM_BYTE                            *f_pbWrappedKeyData,
    __in                                            DRM_DWORD                            f_cbWrappedKeyData )
{
    DRM_RESULT  dr          = DRM_SUCCESS;
    DRM_BYTE   *pbDecrypted = NULL;

    ChkPtr( f_pUnwrapContext );
    ChkPtr( f_pbWrappedKeyData );

    /*
    ** The unwrap context should be initialized, run a few checks anyway
    */
    ChkArg( f_pUnwrapContext->wrappingKey.eType == eOEM_HAL_KEYREGISTER_TYPE_P256_KEYPAIR );

    switch( f_pUnwrapContext->targetKey.eType )
    {
    case eOEM_HAL_KEYREGISTER_TYPE_P256_KEYPAIR:
        ChkArg( f_cbWrappedKeyData == SIZEOF( PRIVKEY_P256 ) );
        pbDecrypted = ( DRM_BYTE* )&f_pUnwrapContext->targetKey.keyBuffer.pEcc256Pair->privateKey;
        break;

    case eOEM_HAL_KEYREGISTER_TYPE_AES128:
        ChkArg( f_cbWrappedKeyData == DRM_AES_KEYSIZE_128 );
        pbDecrypted = ( DRM_BYTE* )f_pUnwrapContext->targetKey.keyBuffer.pAes128->rgbKey;
        break;

    default:
        /*
        ** Code should never reach here
        */
        DRMASSERT( FALSE );
        ChkDR( DRM_E_LOGICERR );
    }

    ChkDR( Oem_HalOemImpl_WrapOrUnwrapWithSymmetricCipherRaw(
        FALSE,
        f_pUnwrapContext,
        NULL,
        f_pbWrappedKeyData,
        pbDecrypted,
        f_cbWrappedKeyData ) );

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    Oem_HalOemImpl_UnwrapAesKeyPairWithEcc256PrivKey
**
** Synopsis:    Decrypts an AES key using an ECC256 private key.
**
** Arguments:   [f_pCryptoContext]   : A pointer to a crypto context
**              [f_pUnwrapContext]   : A pointer to an initialized unwrap context
**              [f_pbWrappedKeyData] : A buffer containing encrypted key data
**              [f_cbWrappedKeyData] : the length of the data in f_pbWrappedKeyData
**
** Returns:     DRM_SUCCESS                         - on success
**              DRM_E_OEMHAL_UNSUPPORTED_KEY_LENGTH - if the key data is invalid
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_UnwrapAesKeyPairWithEcc256PrivKey(
    __inout_ecount( 1 )                             DRM_CRYPTO_CONTEXT                  *f_pCryptoContext,
    __in                                            OEM_HAL_UNWRAP_CONTEXT              *f_pUnwrapContext,
    __in_bcount( f_cbWrappedKeyData )         const DRM_BYTE                            *f_pbWrappedKeyData,
    __in                                            DRM_DWORD                            f_cbWrappedKeyData )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkPtr( f_pUnwrapContext );
    ChkPtr( f_pbWrappedKeyData );
    ChkBOOL( f_cbWrappedKeyData == SIZEOF( CIPHERTEXT_P256 ), DRM_E_OEMHAL_UNSUPPORTED_KEY_LENGTH );

    /*
    ** The unwrap context should be initialized, run a few checks anyway
    */
    ChkArg( f_pUnwrapContext->wrappingKey.eType == eOEM_HAL_KEYREGISTER_TYPE_P256_KEYPAIR );
    ChkArg( f_pUnwrapContext->targetKey.eType   == eOEM_HAL_KEYREGISTER_TYPE_AES128_PAIR );

    ChkDR( OEM_ECC_Decrypt_P256(
        &f_pUnwrapContext->wrappingKey.keyBuffer.pEcc256Pair->privateKey,
        ( const CIPHERTEXT_P256* )f_pbWrappedKeyData,
        ( PLAINTEXT_P256* )f_pUnwrapContext->targetKey.keyBuffer.pAes128Pair->rgbKeys,
        ( struct bigctx_t* )f_pCryptoContext->rgbCryptoContext ) );

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_UnwrapXmrAesKeyPairWithEcc256PrivKey(
    __inout_ecount( 1 )                             DRM_CRYPTO_CONTEXT                  *f_pCryptoContext,
    __in                                      const OEM_HAL_UNWRAP_CONTEXT              *f_pUnwrapContext,
    __in_ecount( SIZEOF( CIPHERTEXT_P256 ) )  const DRM_BYTE                            *f_pbEncKeys,
    __out_ecount( SIZEOF( PLAINTEXT_P256 ) )        DRM_BYTE                            *f_pbDecKeys )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( OEM_ECC_Decrypt_P256(
        &f_pUnwrapContext->wrappingKey.keyBuffer.pEcc256Pair->privateKey,
        ( const CIPHERTEXT_P256 * )f_pbEncKeys,
        ( PLAINTEXT_P256* )f_pbDecKeys,
        ( struct bigctx_t* )f_pCryptoContext ) );

ErrorExit:
    return dr;
}


/*****************************************************************************
** Function:    Oem_HalOemImpl_UnwrapEcc256WithAesKeyPair
**
** Synopsis:    Decrypts an ECC256 private key using an AES key pair.
**
** Arguments:   [f_pUnwrapContext]   : A pointer to an initialized unwrap context
**              [f_pbWrappedKeyData] : A buffer containing encrypted key data
**              [f_cbWrappedKeyData] : the length of the data in f_pbWrappedKeyData
**
** Returns:     DRM_SUCCESS                         - on success
**              DRM_E_OEMHAL_UNSUPPORTED_KEY_LENGTH - if the key data is invalid
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_UnwrapEcc256WithAesKeyPair(
    __in                                            OEM_HAL_UNWRAP_CONTEXT              *f_pUnwrapContext,
    __in_bcount( f_cbWrappedKeyData )         const DRM_BYTE                            *f_pbWrappedKeyData,
    __in                                            DRM_DWORD                            f_cbWrappedKeyData )
{
    DRM_RESULT   dr             = DRM_SUCCESS;
    PRIVKEY_P256 oPrivKeyBuffer;    /* Do not zero-init key material */

    ChkPtr( f_pUnwrapContext );
    ChkPtr( f_pbWrappedKeyData );
    ChkBOOL( f_cbWrappedKeyData == ECC_P256_PRIVKEY_SIZE_IN_BYTES, DRM_E_OEMHAL_UNSUPPORTED_KEY_LENGTH );

    /*
    ** The unwrap context should be initialized, run a few checks anyway
    */
    ChkArg( f_pUnwrapContext->wrappingKey.eType == eOEM_HAL_KEYREGISTER_TYPE_AES128_PAIR );
    ChkArg( f_pUnwrapContext->targetKey.eType   == eOEM_HAL_KEYREGISTER_TYPE_P256_KEYPAIR );

    OEM_SECURE_MEMCPY( &oPrivKeyBuffer, f_pbWrappedKeyData, ECC_P256_PRIVKEY_SIZE_IN_BYTES );

    ChkDR( _CheckAesKeyExpanded(
        &f_pUnwrapContext->wrappingKey.keyBuffer.pAes128Pair->rgbKeys[OEM_HAL_KEY_PAIR_CI_INDEX][0],
        &f_pUnwrapContext->wrappingKey.keyBuffer.pAes128Pair->rgExpandedKeys[OEM_HAL_KEY_PAIR_CI_INDEX] ) );
    ChkDR( _CheckAesKeyExpanded(
        &f_pUnwrapContext->wrappingKey.keyBuffer.pAes128Pair->rgbKeys[OEM_HAL_KEY_PAIR_CK_INDEX][0],
        &f_pUnwrapContext->wrappingKey.keyBuffer.pAes128Pair->rgExpandedKeys[OEM_HAL_KEY_PAIR_CK_INDEX] ) );

    ChkDR( Oem_Aes_EcbDecryptData(
        &f_pUnwrapContext->wrappingKey.keyBuffer.pAes128Pair->rgExpandedKeys[OEM_HAL_KEY_PAIR_CI_INDEX].rgExpandedKey,
        (DRM_BYTE*) &oPrivKeyBuffer,
        DRM_AES_KEYSIZE_128 ) );

    ChkDR( Oem_Aes_EcbDecryptData(
        &f_pUnwrapContext->wrappingKey.keyBuffer.pAes128Pair->rgExpandedKeys[OEM_HAL_KEY_PAIR_CK_INDEX].rgExpandedKey,
        (DRM_BYTE*)&oPrivKeyBuffer + DRM_AES_KEYSIZE_128,
        DRM_AES_KEYSIZE_128 ) );

    OEM_SECURE_MEMCPY( &f_pUnwrapContext->targetKey.keyBuffer.pEcc256Pair->privateKey, &oPrivKeyBuffer, ECC_P256_PRIVKEY_SIZE_IN_BYTES );

ErrorExit:
    OEM_ECC_ZeroPrivateKey_P256( &oPrivKeyBuffer );
    return dr;
}

/*****************************************************************************
** Function:    Oem_HalOemImpl_UnwrapAesPaddedKeyWithEcc256PrivKey
**
** Synopsis:    Decrypts an padded AES key using an ECC256 private key.
**
** Arguments:   [f_pCryptoContext]   : A pointer to a crypto context
**              [f_pUnwrapContext]   : A pointer to an initialized unwrap context
**              [f_pbWrappedKeyData] : A buffer containing encrypted key data
**              [f_cbWrappedKeyData] : the length of the data in f_pbWrappedKeyData
**
** Returns:     DRM_SUCCESS                         - on success
**              DRM_E_OEMHAL_UNSUPPORTED_KEY_LENGTH - if the key data is invalid
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_UnwrapAesPaddedKeyWithEcc256PrivKey(
    __inout_ecount( 1 )                             DRM_CRYPTO_CONTEXT                  *f_pCryptoContext,
    __in                                            OEM_HAL_UNWRAP_CONTEXT              *f_pUnwrapContext,
    __in_bcount( f_cbWrappedKeyData )         const DRM_BYTE                            *f_pbWrappedKeyData,
    __in                                            DRM_DWORD                            f_cbWrappedKeyData )
{
    DRM_RESULT        dr          = DRM_SUCCESS;
    DRM_BYTE          rgbPaddedKey[ECC_P256_PLAINTEXT_SIZE_IN_BYTES];   /* Maximum size */  /* Do not zero-init key material */

    ChkPtr( f_pUnwrapContext );
    ChkPtr( f_pbWrappedKeyData );
    ChkBOOL( f_cbWrappedKeyData == SIZEOF( CIPHERTEXT_P256 ), DRM_E_OEMHAL_UNSUPPORTED_KEY_LENGTH );

    /*
    ** The unwrap context should be initialized, run a few checks anyway
    */
    ChkArg( f_pUnwrapContext->wrappingKey.eType == eOEM_HAL_KEYREGISTER_TYPE_P256_KEYPAIR );
    ChkArg( f_pUnwrapContext->targetKey.eType   == eOEM_HAL_KEYREGISTER_TYPE_AES128 );

    ChkDR( OEM_ECC_Decrypt_P256(
        &f_pUnwrapContext->wrappingKey.keyBuffer.pEcc256Pair->privateKey,
        ( const CIPHERTEXT_P256* )f_pbWrappedKeyData,
        ( PLAINTEXT_P256* )rgbPaddedKey,
        (struct bigctx_t *)f_pCryptoContext->rgbCryptoContext ) );

    /*
    ** The first half of the P256 Plaintext block is just padding, copy the second 16byte
    ** block into the key register
    */
    OEM_SECURE_MEMCPY(
        f_pUnwrapContext->targetKey.keyBuffer.pAes128->rgbKey,
        &rgbPaddedKey[DRM_AES_KEYSIZE_128],
        DRM_AES_KEYSIZE_128 );

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( rgbPaddedKey, SIZEOF( rgbPaddedKey ) );
    return dr;
}

/*****************************************************************************
** Function:    Oem_HalOemImpl_UnwrapAesKeyWithAesEcb
**
** Synopsis:    Decrypts a key encrypted as raw bytes. Uses an
**              AES ECB key
**
** Arguments:   [f_pUnwrapContext]   : A pointer to an initialized unwrap context
**              [f_pbWrappedKeyData] : A buffer containing encrypted key data
**              [f_cbWrappedKeyData] : the length of the data in f_pbWrappedKeyData
**
** Returns:     DRM_SUCCESS          - on success
**              DRM_E_NOTIMPL        - if this operation isn't supported
**              DRM_E_OEMHAL_UNSUPPORTED_KEY_LENGTH - if key data is not
**              2 * AES key length
**
** Notes:
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_UnwrapAesKeyWithAesEcb(
    __in                                            OEM_HAL_UNWRAP_CONTEXT              *f_pUnwrapContext,
    __in_bcount( f_cbWrappedKeyData )         const DRM_BYTE                            *f_pbWrappedKeyData,
    __in                                            DRM_DWORD                            f_cbWrappedKeyData )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkPtr( f_pUnwrapContext );
    ChkPtr( f_pbWrappedKeyData );
    ChkBOOL( f_cbWrappedKeyData == DRM_AES_KEYSIZE_128_X2, DRM_E_OEMHAL_UNSUPPORTED_KEY_LENGTH );

    ChkDR( _CheckAesKeyExpanded(
        f_pUnwrapContext->wrappingKey.keyBuffer.pAes128->rgbKey,
        &f_pUnwrapContext->wrappingKey.keyBuffer.pAes128->expandedKey ) );

    OEM_SECURE_MEMCPY( f_pUnwrapContext->targetKey.keyBuffer.pAes128Pair->rgbKeys, f_pbWrappedKeyData, f_cbWrappedKeyData );

    ChkDR( Oem_Aes_EcbDecryptData(
        &f_pUnwrapContext->wrappingKey.keyBuffer.pAes128->expandedKey.rgExpandedKey,
        ( DRM_BYTE* )f_pUnwrapContext->targetKey.keyBuffer.pAes128Pair->rgbKeys,
        f_cbWrappedKeyData ) );

    /*
    ** There are two scenarios in which this function is called.
    ** (1) Unwrapping an existing leaf license content key from a root license content key.
    **     In scenario (1), dwSecurityLevelRequiredToWrapThisKey is irrelevant.
    **     That unwrapped key can never be re-wrapped with another key, so
    **     dwSecurityLevelRequiredToWrapThisKey will never even be examined.
    ** (2) Unwrapping a locally generated leaf/simple/root license content key from
    **     the corresponding content/root generic key.
    **     In scenario (2), the key may have been generated with a HIGHER
    **     (not lower - see Oem_HalOemImpl_GenerateAes128NewLicenseKeyPair)
    **     security level than OEM_KEY_GENERATION_SECURITY_LEVEL.
    **     That information was then lost when we used key-caching via a generic key
    **     to reduce the set of key registers in use.  In that specific scenario, we
    **     are DOWNGRADING the security level of the key from higher to lower.
    **     On a test device, we're (potentially) downgrading an SL 2000 key to an
    **     SL 150 key, but since it's a test device we don't consider this a risk.
    **     On a production device, OEM_KEY_GENERATION_SECURITY_LEVEL == 2000,
    **     and we don't allow key generation below that level.
    **     Thus, we've correctly maintained the pre-wrap security level of 2000
    **     across the generic key wrap/unwrap boundary.
    */
    f_pUnwrapContext->targetKey.keyBuffer.pAes128Pair->dwSecurityLevelRequiredToWrapThisKey = OEM_KEY_GENERATION_SECURITY_LEVEL;
    f_pUnwrapContext->targetKey.keyBuffer.pAes128Pair->dwRIVRequiredToWrapThisKey           = g_RevocationInfo.dwStoredRIV;

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_UnwrapXmrAesKeyPair(
    __inout                                         OEM_HAL_UNWRAP_CONTEXT              *f_pUnwrapContext,
    __in_ecount( DRM_AES_KEYSIZE_128_X2 )     const DRM_BYTE                            *f_pbDecKeys,
    __in                                            DRM_WORD                             f_wSecurityLevel,
    __in                                            DRM_DWORD                            f_dwRIV )
{
    DRM_RESULT               dr                 = DRM_SUCCESS;
    OEM_HAL_AES_KEY_PAIR    *pAesTargetKeyPair  = NULL;
    switch( f_pUnwrapContext->targetKey.eType )
    {
    case eOEM_HAL_KEYREGISTER_TYPE_AES128_PAIR:
        pAesTargetKeyPair = f_pUnwrapContext->targetKey.keyBuffer.pAes128Pair;
        break;
    case eOEM_HAL_KEYREGISTER_TYPE_AES128_ROOT_PAIR:
        pAesTargetKeyPair = &f_pUnwrapContext->targetKey.keyBuffer.pAes128RootPair->keyPair;
        break;
    default:
        ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
        break;
    }

    OEM_SECURE_MEMCPY( pAesTargetKeyPair->rgbKeys, f_pbDecKeys, DRM_AES_KEYSIZE_128_X2 );
    pAesTargetKeyPair->dwSecurityLevelRequiredToWrapThisKey = f_wSecurityLevel;
    pAesTargetKeyPair->dwRIVRequiredToWrapThisKey           = f_dwRIV;

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_UnwrapXmrLegacyKey(
    __inout                                         OEM_HAL_UNWRAP_CONTEXT              *f_pUnwrapContext,
    __in                                            DRM_WORD                             f_wLegacyCryptoType,
    __in_ecount( f_cbDecKeys )                const DRM_BYTE                            *f_pbDecKeys,
    __in                                            DRM_DWORD                            f_cbDecKeys )
{
#if !DRM_SUPPORT_SECUREOEMHAL
    /*
    ** Note to OEMs: Cocktail handling should not be included in OEM HAL unless explicitly required.
    ** This function should return DRM_E_NOTIMPL if feature flag DRM_SUPPORT_BBXLEGACY == 0.
    */

    DRM_RESULT dr = DRM_SUCCESS;

    ChkPtr( f_pUnwrapContext );
    ChkPtr( f_pUnwrapContext->targetKey.keyBuffer.pLegacy );

    f_pUnwrapContext->targetKey.keyBuffer.pLegacy->wLegacyCryptoType = f_wLegacyCryptoType;







    OEM_SECURE_MEMCPY( f_pUnwrapContext->targetKey.keyBuffer.pLegacy->rgbKeyBuffer, f_pbDecKeys, f_cbDecKeys );
    f_pUnwrapContext->targetKey.keyBuffer.pLegacy->dwKeyLength       = f_cbDecKeys;

ErrorExit:
    return dr;
#else /* !DRM_SUPPORT_SECUREOEMHAL */
    DRMASSERT( FALSE );
    return DRM_E_NOTIMPL;
#endif /* !DRM_SUPPORT_SECUREOEMHAL */
}



/*********************************************************************
**
** Function: Oem_HalOemImpl_UnwrapAuxKeyMaterial
**
** Synopsis: Unpack Auxiliary key material from a scaleable root license
**
** Arguments:
** [f_pUnwrapContext]   : the unwrap context to unpack to
** [f_pXmrLicense]      : the license
** [f_pbChecksumData]   : the checksum data
** [f_pKID]             : the KID for checksum validation
**
** Returns:
**      DRM_SUCCESS
**          Success.
**      DRM_E_INVALIDARG
**          One of the arguments is invalid or improperly initialized
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_UnwrapAuxKeyMaterial(
    __inout_ecount( 1 )                             OEM_HAL_UNWRAP_CONTEXT              *f_pUnwrapContext,
    __in_ecount( 1 )                          const DRM_XMR_LICENSE                     *f_pXmrLicense,
    __in_ecount( DRM_MAX_CHECKSUM )           const DRM_BYTE                            *f_pbChecksumData,
    __in                                      const DRM_KID                             *f_pKID )
{
          DRM_RESULT                 dr                     = DRM_SUCCESS;
    const DRM_XMR_OUTER_CONTAINER   *pXmrContainerOuterRoot = NULL;
          OEM_HAL_AES_ROOT_KEY_PAIR *pRootKeyRegister       = NULL;

    ChkPtr( f_pUnwrapContext );
    pRootKeyRegister = f_pUnwrapContext->targetKey.keyBuffer.pAes128RootPair;
    ChkPtr( pRootKeyRegister );
    ChkPtr( f_pXmrLicense );
    ChkArg( pRootKeyRegister->pDerivedKey == NULL );

    pXmrContainerOuterRoot = &f_pXmrLicense->containerOuter;


    /*
    ** Allocate a buffer to store the aux key material, and copy the aux keys across
    */
    ChkDR( Oem_HalOemImpl_AllocateDerivedRootKeyInfo( pRootKeyRegister, &pXmrContainerOuterRoot->containerKeys.AuxKey ) );

    ChkBOOL( pRootKeyRegister->pDerivedKey                  != NULL, DRM_E_OUTOFMEMORY );
    ChkBOOL( pRootKeyRegister->pDerivedKey->AuxKey.pEntries != NULL, DRM_E_LOGICERR );

    DRMASSERT( pRootKeyRegister->pDerivedKey->AuxKey.cEntries
            == pXmrContainerOuterRoot->containerKeys.AuxKey.cEntries );

    /*
    ** The Content Key XMR object has the encrypted secondary key at offset
    ** ECC_P256_CIPHERTEXT_SIZE_IN_BYTES of the EncryptedKeyBuffer field.
    */
    DRMASSERT( pXmrContainerOuterRoot->containerKeys.ContentKey.wKeyEncryptionCipherType ==
               XMR_ASYMMETRIC_ENCRYPTION_TYPE_ECC_256_WITH_KZ );

    ChkBOOL( pXmrContainerOuterRoot->containerKeys.ContentKey.cbEncryptedKey == ECC_P256_CIPHERTEXT_SIZE_IN_BYTES + DRM_AES_KEYSIZE_128, DRM_E_INVALID_LICENSE );

    OEM_SECURE_MEMCPY(
        pRootKeyRegister->pDerivedKey->rgbSecondaryKey,
        ( pXmrContainerOuterRoot->containerKeys.ContentKey.pbEncryptedKeyBuffer
        + pXmrContainerOuterRoot->containerKeys.ContentKey.iEncryptedKey
        + ECC_P256_CIPHERTEXT_SIZE_IN_BYTES ),
        DRM_AES_KEYSIZE_128 );

    /*
    ** Decrypt the secondary key with the root content key (note: this uses an AES ECB *Encrypt* operation!)
    */
    ChkDR( _CheckAesKeyExpanded(
        &pRootKeyRegister->keyPair.rgbKeys[OEM_HAL_KEY_PAIR_CK_INDEX][0],
        &pRootKeyRegister->keyPair.rgExpandedKeys[OEM_HAL_KEY_PAIR_CK_INDEX] ) );

    ChkDR( Oem_Aes_EcbEncryptData(
        &pRootKeyRegister->keyPair.rgExpandedKeys[OEM_HAL_KEY_PAIR_CK_INDEX].rgExpandedKey,
        pRootKeyRegister->pDerivedKey->rgbSecondaryKey,
        DRM_AES_KEYSIZE_128 ) );

    OEM_SECURE_MEMCPY(
        pRootKeyRegister->pDerivedKey->checksumInfo.rgbChecksumData, f_pbChecksumData, DRM_MAX_CHECKSUM );

    OEM_SECURE_MEMCPY( &pRootKeyRegister->pDerivedKey->checksumInfo.KID, f_pKID, SIZEOF( DRM_KID ) );

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_UnwrapGetDerivedAuxKey(
    __in_ecount( 1 )                          const OEM_HAL_UNWRAP_CONTEXT              *f_pUnwrapContext,
    __out_ecount( 1 )                               DRM_XMR_AUX_KEY                     *f_poAuxKey )
{
    OEM_SECURE_MEMCPY( f_poAuxKey, &f_pUnwrapContext->wrappingKey.keyBuffer.pAes128RootPair->pDerivedKey->AuxKey, SIZEOF( DRM_XMR_AUX_KEY ) );
    return DRM_SUCCESS;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_UnwrapGetRootCK(
    __in_ecount( 1 )                          const OEM_HAL_UNWRAP_CONTEXT              *f_pUnwrapContext,
    __out_ecount( DRM_AES_KEYSIZE_128 )             DRM_BYTE                            *f_pbRootCK )
{
    OEM_SECURE_MEMCPY( f_pbRootCK, &f_pUnwrapContext->wrappingKey.keyBuffer.pAes128RootPair->keyPair.rgbKeys[OEM_HAL_KEY_PAIR_CK_INDEX], DRM_AES_KEYSIZE_128 );
    return DRM_SUCCESS;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_UnwrapGetKeyDerivationData(
    __in_ecount( 1 )                          const OEM_HAL_UNWRAP_CONTEXT              *f_pUnwrapContext,
    __out_ecount( DRM_AES_KEYSIZE_128 )             DRM_BYTE                            *f_pbSecondaryKey,
    __out_ecount_opt( 1 )                           DRM_KID                             *f_pKID,
    __out_ecount_opt( DRM_AES_CK_CHECKSUM_LENGTH )  DRM_BYTE                            *f_pbChecksumData )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkPtr( f_pUnwrapContext );

    /*
    ** The unwrap context should be initialized, run a few checks anyway
    */
    ChkArg( f_pUnwrapContext->wrappingKey.keyBuffer.pAes128RootPair->pDerivedKey != NULL );

    OEM_SECURE_MEMCPY( f_pbSecondaryKey, f_pUnwrapContext->wrappingKey.keyBuffer.pAes128RootPair->pDerivedKey->rgbSecondaryKey, DRM_AES_KEYSIZE_128 );
    if( f_pKID != NULL )
    {
        OEM_SECURE_MEMCPY( f_pKID, &f_pUnwrapContext->wrappingKey.keyBuffer.pAes128RootPair->pDerivedKey->checksumInfo.KID, SIZEOF( DRM_KID ) );
    }
    if( f_pbChecksumData != NULL )
    {
        OEM_SECURE_MEMCPY( f_pbChecksumData, &f_pUnwrapContext->wrappingKey.keyBuffer.pAes128RootPair->pDerivedKey->checksumInfo.rgbChecksumData, DRM_AES_CK_CHECKSUM_LENGTH );
    }

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_UnwrapLeafContentKey(
    __in_ecount( 1 )                          const OEM_HAL_UNWRAP_CONTEXT              *f_pUnwrapContext,
    __out_ecount( DRM_AES_KEYSIZE_128_X2 )          DRM_BYTE                            *f_pbDecKeys )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMSIZEASSERT( DRM_AES_KEYSIZE_128_X2, ECC_P256_PLAINTEXT_SIZE_IN_BYTES );

    ChkPtr( f_pUnwrapContext );

    /*
    ** The unwrap context should be initialized, run a few checks anyway
    */
    ChkArg( f_pUnwrapContext->wrappingKey.keyBuffer.pAes128RootPair->pDerivedKey == NULL );

    ChkDR( _CheckAesKeyExpanded(
        &f_pUnwrapContext->wrappingKey.keyBuffer.pAes128RootPair->keyPair.rgbKeys[OEM_HAL_KEY_PAIR_CK_INDEX][0],
        &f_pUnwrapContext->wrappingKey.keyBuffer.pAes128RootPair->keyPair.rgExpandedKeys[OEM_HAL_KEY_PAIR_CK_INDEX] ) );

    ChkDR( Oem_Aes_EcbDecryptData(
        &f_pUnwrapContext->wrappingKey.keyBuffer.pAes128RootPair->keyPair.rgExpandedKeys[OEM_HAL_KEY_PAIR_CK_INDEX].rgExpandedKey,
        f_pbDecKeys,
        DRM_AES_KEYSIZE_128_X2 ) );

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_UnwrapGetWrappingEcc256PublicKeyData(
    __in_ecount( 1 )                          const OEM_HAL_UNWRAP_CONTEXT              *f_pUnwrapContext,
    __out_ecount_opt( 1 )                           PUBKEY_P256                         *f_pKey,
    __out_ecount_opt( 1 )                           DRM_DWORD                           *f_pdwSecurityLevel,
    __out_ecount_opt( 1 )                           DRM_DWORD                           *f_pdwKeyUsageBitmap,
    __out_ecount_opt( 1 )                           DRM_BOOL                            *f_pfRootKey )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    ChkPtr( f_pUnwrapContext );
    ChkPtr( f_pUnwrapContext->wrappingKey.keyBuffer.pEcc256Public );

    if( f_pKey != NULL )
    {
        OEM_SECURE_MEMCPY( f_pKey, &f_pUnwrapContext->wrappingKey.keyBuffer.pEcc256Public->key, SIZEOF( PUBKEY_P256 ) );
    }
    if( f_pdwSecurityLevel != NULL )
    {
        *f_pdwSecurityLevel = f_pUnwrapContext->wrappingKey.keyBuffer.pEcc256Public->dwSecurityLevel;
    }
    if( f_pdwKeyUsageBitmap != NULL )
    {
        *f_pdwKeyUsageBitmap = f_pUnwrapContext->wrappingKey.keyBuffer.pEcc256Public->dwKeyUsageBitmap;
    }
    if( f_pfRootKey != NULL )
    {
        *f_pfRootKey = f_pUnwrapContext->wrappingKey.keyBuffer.pEcc256Public->fRootKey;
    }

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_UnwrapSetTargetEcc256PublicKeyData(
    __inout_ecount( 1 )                             OEM_HAL_UNWRAP_CONTEXT              *f_pUnwrapContext,
    __in_ecount( 1 )                          const PUBKEY_P256                         *f_pKey,
    __in                                            DRM_DWORD                            f_dwSecurityLevel,
    __in                                            DRM_DWORD                            f_dwKeyUsageBitmap,
    __in                                            DRM_DWORD                            f_dwFullCertKeyUsageBitmap,
    __in_ecount( 1 )                          const DRM_SHA256_Digest                   *f_pDigest,
    __in                                            OEM_HAL_PUBKEY_VERIFICATION_STATE    f_eVerificationState )
{
    DRM_RESULT                       dr             = DRM_SUCCESS;
    OEM_HAL_ECC256_PUBLIC_KEY       *pEcc256Public  = NULL;
    const OEM_HAL_ECC256_PUBLIC_KEY *pEcc256Parent  = NULL;

    ChkPtr( f_pUnwrapContext );
    pEcc256Public = f_pUnwrapContext->targetKey.keyBuffer.pEcc256Public;
    ChkPtr( pEcc256Public );
    pEcc256Parent = f_pUnwrapContext->wrappingKey.keyBuffer.pEcc256Public;
    ChkPtr( pEcc256Parent );
    AssertChkArg( pEcc256Parent->validationInfo.cDigests < DRM_BCERT_MAX_CERTS_PER_CHAIN );

    /* Full certificate key usage bitmap must fully contain this key's usage bitmap. */
    AssertChkArg( ( f_dwFullCertKeyUsageBitmap & f_dwKeyUsageBitmap ) == f_dwKeyUsageBitmap );

    pEcc256Public->fRootKey                 = FALSE;
    pEcc256Public->dwSecurityLevel          = f_dwSecurityLevel;
    pEcc256Public->dwKeyUsageBitmap         = f_dwKeyUsageBitmap;
    pEcc256Public->dwFullCertKeyUsageBitmap = f_dwFullCertKeyUsageBitmap;

    OEM_SECURE_MEMCPY( &pEcc256Public->key, f_pKey, SIZEOF( PUBKEY_P256 ) );

    OEM_SECURE_MEMCPY( &pEcc256Public->validationInfo.certificateDigests[0], f_pDigest, SIZEOF(DRM_SHA256_Digest) );
    pEcc256Public->validationInfo.cDigests = 1;

    /* Copy all the parent digests.  If any parent key is revoked, this key is too. */
    if( pEcc256Parent->validationInfo.cDigests > 0 )
    {
        OEM_SECURE_MEMCPY(
            &pEcc256Public->validationInfo.certificateDigests[1],
            &pEcc256Parent->validationInfo.certificateDigests[0],
            pEcc256Parent->validationInfo.cDigests * SHA256_DIGEST_SIZE_IN_BYTES );
        pEcc256Public->validationInfo.cDigests += pEcc256Parent->validationInfo.cDigests;
    }
    else
    {
        DRMASSERT( pEcc256Parent->fRootKey );
    }

    /* If the parent has a worse verification state than the specified verification state, use the parent's verification state instead. */
    switch( pEcc256Parent->validationInfo.eVerificationState )
    {
    case eOEM_HAL_PUBKEY_REVOKED:
        f_eVerificationState = eOEM_HAL_PUBKEY_REVOKED;
        break;
    case eOEM_HAL_PUBKEY_NOT_VERIFIED:
        if( f_eVerificationState != eOEM_HAL_PUBKEY_REVOKED )
        {
            f_eVerificationState = eOEM_HAL_PUBKEY_NOT_VERIFIED;
        }
        break;
    case eOEM_HAL_PUBKEY_VERIFIED:
        /* Nothing to do */
        break;
    default:
        AssertChkArg( FALSE );
    }
    pEcc256Public->validationInfo.eVerificationState = f_eVerificationState;

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_WrapGetEncryptionType(
    __in                                      const OEM_HAL_WRAP_CONTEXT                *f_pWrapContext,
    __out_ecount(1)                                 OEM_HAL_CRYPTO_TYPE                 *f_peEncryptionType )
{
    *f_peEncryptionType = f_pWrapContext->eEncryptionType;
    return DRM_SUCCESS;
}

/*****************************************************************************
** Function:    Oem_HalOemImpl_WrapWithSymmetricCipher
**
** Synopsis:    Encrypts a key using an AES key derived from a private key.
**
** Arguments:   [f_pUnwrapContext] : A pointer to an initialized unwrap context
**              [f_pbWrappedKey]   : A buffer in which to store the encrypted key data
**              [f_pcbWrappedKey]  : the length of the f_pbWrappedKeyData buffer
**
** Returns:     DRM_SUCCESS          - on success
**              DRM_E_BUFFERTOOSMALL - if the key data is invalid
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_WrapWithSymmetricCipher(
    __in                                            OEM_HAL_WRAP_CONTEXT                *f_pWrapContext,
    __out_bcount_opt( *f_pcbWrappedKey )            DRM_BYTE                            *f_pbWrappedKey,
    __inout_ecount(1)                               DRM_DWORD                           *f_pcbWrappedKey )
{
          DRM_RESULT dr                              = DRM_SUCCESS;
          DRM_BYTE   rgbTemp[SIZEOF( PRIVKEY_P256 )];   /* Do not zero-init key material */
    const DRM_BYTE  *pbToEncrypt = NULL;
          DRM_DWORD  cbToEncrypt = 0;

    ChkPtr( f_pWrapContext );
    ChkPtr( f_pcbWrappedKey );

    /*
    ** The wrap context should be initialized, run a few checks anyway
    */
    ChkArg( f_pWrapContext->wrappingKey.eType == eOEM_HAL_KEYREGISTER_TYPE_P256_KEYPAIR );

    switch( f_pWrapContext->targetKey.eType )
    {
    case eOEM_HAL_KEYREGISTER_TYPE_P256_KEYPAIR:
        pbToEncrypt = ( const DRM_BYTE* )&f_pWrapContext->targetKey.keyBuffer.pEcc256Pair->privateKey;
        cbToEncrypt = SIZEOF( PRIVKEY_P256 );
        break;

    case eOEM_HAL_KEYREGISTER_TYPE_AES128_ROOT_PAIR:
        pbToEncrypt = ( const DRM_BYTE* )f_pWrapContext->targetKey.keyBuffer.pAes128RootPair->keyPair.rgbKeys;
        cbToEncrypt = SIZEOF( PRIVKEY_P256 );
        break;

    case eOEM_HAL_KEYREGISTER_TYPE_AES128_PAIR:
        pbToEncrypt = ( const DRM_BYTE* )f_pWrapContext->targetKey.keyBuffer.pAes128Pair->rgbKeys;
        cbToEncrypt = SIZEOF( PRIVKEY_P256 );
        break;

    case eOEM_HAL_KEYREGISTER_TYPE_AES128:
        pbToEncrypt = ( const DRM_BYTE* )f_pWrapContext->targetKey.keyBuffer.pAes128->rgbKey;
        cbToEncrypt = DRM_AES_KEYSIZE_128;
        break;

#if !DRM_SUPPORT_SECUREOEMHAL
    case eOEM_HAL_KEYREGISTER_TYPE_LEGACY:
        /*
        ** Note to OEMs: Cocktail handling should not be included in OEM HAL unless explicitly required.
        ** This case statement should not exist if DRM_SUPPORT_BBXLEGACY == 0.
        */

        pbToEncrypt = ( const DRM_BYTE* )f_pWrapContext->targetKey.keyBuffer.pLegacy->rgbKeyBuffer;
        cbToEncrypt = SIZEOF( PRIVKEY_P256 );
        break;
#endif /* !DRM_SUPPORT_SECUREOEMHAL */

    default:
        /*
        ** Code should never reach here
        */
        DRMASSERT( FALSE );
        ChkDR( DRM_E_LOGICERR );
    }

    if( f_pbWrappedKey == NULL || *f_pcbWrappedKey < cbToEncrypt )
    {
        dr = DRM_E_BUFFERTOOSMALL;
    }
    else
    {
        ChkDR( Oem_HalOemImpl_WrapOrUnwrapWithSymmetricCipherRaw(
            TRUE,
            NULL,
            f_pWrapContext,
            pbToEncrypt,
            rgbTemp,
            cbToEncrypt ) );

        OEM_SECURE_MEMCPY( f_pbWrappedKey, rgbTemp, cbToEncrypt );
    }

    *f_pcbWrappedKey = cbToEncrypt;

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( rgbTemp, SIZEOF( rgbTemp ) );
    return dr;
}

/*****************************************************************************
** Function:    Oem_HalOemImpl_WrapWithPassThrough
**
** Synopsis:    Pass the key back unencrypted.
**
** Arguments:   [f_pUnwrapContext] : A pointer to an initialized unwrap context
**              [f_pbWrappedKey]   : A buffer in which to store the encrypted key data
**              [f_pcbWrappedKey]  : the length of the f_pbWrappedKeyData buffer
**
** Returns:     DRM_SUCCESS          - on success
**              DRM_E_BUFFERTOOSMALL - if the key data is invalid
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_WrapWithPassThrough(
    __in                                            OEM_HAL_WRAP_CONTEXT                *f_pWrapContext,
    __out_bcount_opt( *f_pcbWrappedKey )            DRM_BYTE                            *f_pbWrappedKey,
    __inout_ecount(1)                               DRM_DWORD                           *f_pcbWrappedKey )
{
          DRM_RESULT  dr          = DRM_SUCCESS;
          DRM_DWORD   dwKeyLength = 0;
    const DRM_BYTE   *pbKey       = NULL;

    ChkPtr( f_pWrapContext );
    ChkPtr( f_pcbWrappedKey );

    /*
    ** The wrap context should be initialized, run a few checks anyway
    */
    ChkArg( f_pWrapContext->wrappingKey.eType == eOEM_HAL_KEYREGISTER_TYPE_INVALID );

    if( f_pWrapContext->targetKey.eType == eOEM_HAL_KEYREGISTER_TYPE_LEGACY )
    {
#if !DRM_SUPPORT_SECUREOEMHAL
        /*
        ** Note to OEMs: Cocktail handling should not be included in OEM HAL unless explicitly required.
        ** This if block should fail with DRM_E_NOTIMPL if feature flag DRM_SUPPORT_BBXLEGACY == 0.
        */

        DRM_DWORD  dwKeyOffset = 0;

        ChkArg( f_pWrapContext->targetKey.keyBuffer.pLegacy->dwKeyLength <= OEM_HAL_LEGACY_KEY_BUFFER_SIZE );

        /*
        ** As this is returning the key in the clear, only the actual legacy key (and not the
        ** integrity key is required.
        */
        switch( f_pWrapContext->targetKey.keyBuffer.pLegacy->wLegacyCryptoType )
        {
        case XMR_SYMMETRIC_ENCRYPTION_TYPE_RC4_CIPHER:

            dwKeyLength = GET_BYTE( f_pWrapContext->targetKey.keyBuffer.pLegacy->rgbKeyBuffer, DRM_AES_KEYSIZE_128 );

            ChkBOOL( dwKeyLength == DRMCIPHERKEYLEN_RC4 || dwKeyLength == DRMCIPHERKEYLEN_RC4_PDRM, DRM_E_LIC_KEY_DECODE_FAILURE );

            dwKeyOffset = DRM_AES_KEYSIZE_128 + 1;

            break;

        case XMR_SYMMETRIC_ENCRYPTION_TYPE_COCKTAIL:
            dwKeyLength = DRMCIPHERKEYLEN_RC4;
            dwKeyOffset = DRM_AES_KEYSIZE_128;
            break;

        default:
            ChkDR( DRM_E_UNSUPPORTED_ALGORITHM );
            break;
        }

        pbKey = &f_pWrapContext->targetKey.keyBuffer.pLegacy->rgbKeyBuffer[dwKeyOffset];
#else /* !DRM_SUPPORT_SECUREOEMHAL */
        DRMASSERT( FALSE );
        ChkDR( DRM_E_NOTIMPL );
#endif /* !DRM_SUPPORT_SECUREOEMHAL */
    }
    else if( f_pWrapContext->targetKey.eType == eOEM_HAL_KEYREGISTER_TYPE_P256_KEYPAIR )
    {
        ChkArg( f_pWrapContext->targetKey.keyBuffer.pEcc256Pair->fPublicKeyPresent );

        pbKey       = ( const DRM_BYTE* )&f_pWrapContext->targetKey.keyBuffer.pEcc256Pair->publicKey;
        dwKeyLength = SIZEOF( PUBKEY_P256 );
    }
    else
    {
        ChkDR( DRM_E_INVALIDARG );
    }

    *f_pcbWrappedKey = dwKeyLength;
    if( f_pbWrappedKey == NULL || *f_pcbWrappedKey < dwKeyLength )
    {
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }
    else
    {
        OEM_SECURE_MEMCPY( f_pbWrappedKey, pbKey, dwKeyLength );
    }

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    Oem_HalOemImpl_WrapEncryptedKey
**
** Synopsis:    Encrypts data (a key) using the keyfile key to prepare to write it to the keyfile.
**
** Arguments:   [f_pOEMContext]     : the OEM context pointer
**              [f_eKeyType]        : The key type for the wrapped key
**              [f_pWrapContext]    : A pointer to an initialized wrap context
**              [f_pEncryptedKey]   : A structure containing encrypted key data
**
** Returns:     DRM_SUCCESS          - on success
**              DRM_E_PTR            - if the input parameters are NULL
**              DRM_E_INVALIDARG     - if the wrap context is incorrectly initialized
**              DRM_E_BUFFERTOOSMALL - if the key buffer is too small
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_WrapEncryptedKey(
    __in                                            DRM_VOID                            *f_pOEMContext,
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in                                      const OEM_HAL_WRAP_CONTEXT                *f_pWrapContext,
    __out_ecount(1)                                 DRM_ENCRYPTED_KEY                   *f_pEncryptedKey )
{
          DRM_RESULT  dr                                                = DRM_SUCCESS;
    const DRM_BYTE   *pbKeyData                                         = NULL;
          DRM_BYTE    rgbEncryptBuffer[ECC_P256_INTEGER_SIZE_IN_BYTES];     /* Do not zero-init key material */
          DRM_DWORD   cbEncryptedKey                                    = 0;
          DRM_DWORD   cbSignedEncryptedKey                              = 0;

    ChkPtr( f_pWrapContext );
    ChkPtr( f_pEncryptedKey );

    /*
    ** The unwrap context should be initialized, run a few checks anyway
    */
    ChkArg( f_pWrapContext->wrappingKey.eType == eOEM_HAL_KEYREGISTER_TYPE_AES128 );
    ChkArg( f_pWrapContext->eEncryptionType   == eOEM_HAL_CRYPTO_TYPE_AES_CBC );

    switch( f_pWrapContext->targetKey.eType )
    {
    case eOEM_HAL_KEYREGISTER_TYPE_P256_KEYPAIR:
        cbEncryptedKey = ECC_P256_INTEGER_SIZE_IN_BYTES;
        pbKeyData      = ( const DRM_BYTE* )f_pWrapContext->targetKey.keyBuffer.pEcc256Pair->privateKey.m_rgbPrivkey;
        break;

    case eOEM_HAL_KEYREGISTER_TYPE_AES128:
        cbEncryptedKey = DRM_AES_KEYSIZE_128;
        pbKeyData      = f_pWrapContext->targetKey.keyBuffer.pAes128->rgbKey;
        break;

    default:
        /*
        ** Code should never reach here
        */
        DRMASSERT( FALSE );
        ChkDR( DRM_E_LOGICERR );
        break;
    }

    /*
    ** Key type plus encrypted key data plus sizeof AES signature
    */
    cbSignedEncryptedKey = SIZEOF(DRM_DWORD) + cbEncryptedKey + DRM_AES_BLOCKLEN;

    if( f_pEncryptedKey->pbEncryptedKey == NULL
     || f_pEncryptedKey->cbEncryptedKey <  cbSignedEncryptedKey )
    {
        f_pEncryptedKey->cbEncryptedKey = cbSignedEncryptedKey;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /*
    ** Fill-in the struct
    */
    f_pEncryptedKey->wKeypairDecryptionMethod = KF_DECRYPTION_TYPE_AES_CBC_SIG;
    f_pEncryptedKey->wKeySize                 = (DRM_WORD)( (DRM_WORD)(cbSignedEncryptedKey) * (DRM_WORD)(BITS_PER_BYTE) ); /* size when decrypted */

    /*
    ** Generate random IV
    */
    ChkDR( Oem_Random_GetBytes( f_pOEMContext, f_pEncryptedKey->rgbInitializationVector, DRM_AES_BLOCKLEN ) );

    OEM_SECURE_MEMCPY( rgbEncryptBuffer, pbKeyData, cbEncryptedKey );

    ChkDR( _CheckAesKeyExpanded(
        f_pWrapContext->wrappingKey.keyBuffer.pAes128->rgbKey,
        &f_pWrapContext->wrappingKey.keyBuffer.pAes128->expandedKey ) );

    ChkDR( Oem_Aes_CbcEncryptData(
        &f_pWrapContext->wrappingKey.keyBuffer.pAes128->expandedKey.rgExpandedKey,
        rgbEncryptBuffer,
        cbEncryptedKey,
        f_pEncryptedKey->rgbInitializationVector ) );

    OEM_SECURE_ZERO_MEMORY( f_pEncryptedKey->pbEncryptedKey, f_pEncryptedKey->cbEncryptedKey );

    /*
    ** Copy the key type as the first 4 bytes
    */
    DWORD_TO_NETWORKBYTES( f_pEncryptedKey->pbEncryptedKey, 0, (DRM_DWORD)f_eKeyType );

    /*
    ** Copy the encrypted key into the final buffer as the next cbEncryptedKey bytes
    */
    OEM_SECURE_MEMCPY( &f_pEncryptedKey->pbEncryptedKey[SIZEOF(DRM_DWORD)], rgbEncryptBuffer, cbEncryptedKey );

    /*
    ** Sign the key type plus the encrypted key and append that signature as the 16 bytes following the encrypted key
    */
    ChkDR( Oem_Omac1_Sign(
        &f_pWrapContext->wrappingKey.keyBuffer.pAes128->expandedKey.rgExpandedKey,
        f_pEncryptedKey->pbEncryptedKey,
        0,
        SIZEOF(DRM_DWORD) + cbEncryptedKey,
        &f_pEncryptedKey->pbEncryptedKey[SIZEOF(DRM_DWORD) + cbEncryptedKey] ) );

    f_pEncryptedKey->cbEncryptedKey = cbSignedEncryptedKey;

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( rgbEncryptBuffer, SIZEOF( rgbEncryptBuffer ) );
    return dr;
}

/*****************************************************************************
** Function:    Oem_HalOemImpl_WrapWithAesEcb
**
** Synopsis:    Wrap one AES key pair with an AES ECB key pair. This is used
**              to wrap content keys with root keys for licenses
**              and wrap root/leaf/simple content keys with generic keys.
**
** Arguments:   [f_pUnwrapContext] : A pointer to an initialized unwrap context
**              [f_pbWrappedKey]   : A buffer in which to store the encrypted key data
**              [f_pcbWrappedKey]  : the length of the f_pbWrappedKeyData buffer
**
** Returns:     DRM_SUCCESS          - on success
**              DRM_E_BUFFERTOOSMALL - if the key data is invalid
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_WrapWithAesEcb(
    __in                                            OEM_HAL_WRAP_CONTEXT                *f_pWrapContext,
    __out_bcount_opt( *f_pcbWrappedKey )            DRM_BYTE                            *f_pbWrappedKey,
    __inout_ecount(1)                               DRM_DWORD                           *f_pcbWrappedKey )
{
          DRM_RESULT   dr           = DRM_SUCCESS;
    const DRM_AES_KEY *pWrappingKey = NULL;
          DRM_BYTE     rgbPlainTextKeyPair[ DRM_AES_KEYSIZE_128_X2 ];    /* Do not zero-init key material */


    ChkPtr( f_pWrapContext );
    ChkPtr( f_pcbWrappedKey );

    /*
    ** The wrap context should be initialized, run a few checks anyway
    */
    ChkArg( f_pWrapContext->targetKey.eType == eOEM_HAL_KEYREGISTER_TYPE_AES128_PAIR
         || f_pWrapContext->targetKey.eType == eOEM_HAL_KEYREGISTER_TYPE_AES128_ROOT_PAIR );

    if( *f_pcbWrappedKey < DRM_AES_KEYSIZE_128_X2
     || f_pbWrappedKey  == NULL )
    {
        *f_pcbWrappedKey = DRM_AES_KEYSIZE_128_X2;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    switch( f_pWrapContext->wrappingKey.eType )
    {
    case eOEM_HAL_KEYREGISTER_TYPE_AES128_ROOT_PAIR:

        /* You can only wrap a non-root pair with a root pair. */
        AssertChkArg( f_pWrapContext->targetKey.eType == eOEM_HAL_KEYREGISTER_TYPE_AES128_PAIR );

        /* You can't wrap a higher security level leaf with a lower security level root. */
        ChkBOOL( f_pWrapContext->targetKey.keyBuffer.pAes128Pair->dwSecurityLevelRequiredToWrapThisKey <= f_pWrapContext->wrappingKey.keyBuffer.pAes128RootPair->keyPair.dwSecurityLevelRequiredToWrapThisKey, DRM_E_DEVICE_SECURITY_LEVEL_TOO_LOW );

        ChkDR( _CheckAesKeyExpanded(
            &f_pWrapContext->wrappingKey.keyBuffer.pAes128RootPair->keyPair.rgbKeys[OEM_HAL_KEY_PAIR_CK_INDEX][0],
            &f_pWrapContext->wrappingKey.keyBuffer.pAes128RootPair->keyPair.rgExpandedKeys[OEM_HAL_KEY_PAIR_CK_INDEX] ) );

        pWrappingKey = &f_pWrapContext->wrappingKey.keyBuffer.pAes128RootPair->keyPair.rgExpandedKeys[OEM_HAL_KEY_PAIR_CK_INDEX].rgExpandedKey;

        break;

    case eOEM_HAL_KEYREGISTER_TYPE_AES128:

        ChkDR( _CheckAesKeyExpanded(
            &f_pWrapContext->wrappingKey.keyBuffer.pAes128->rgbKey[0],
            &f_pWrapContext->wrappingKey.keyBuffer.pAes128->expandedKey ) );

        pWrappingKey = &f_pWrapContext->wrappingKey.keyBuffer.pAes128->expandedKey.rgExpandedKey;

        break;

    default:
        /*
        ** Should never reach here
        */
        DRMASSERT( FALSE );
        ChkDR( DRM_E_LOGICERR );
    }

    /*
    ** Copy the key to be wrapped into a temporary buffer, encrypt it, and then copy it to the output buffer
    */
    OEM_SECURE_MEMCPY(
        rgbPlainTextKeyPair,
        f_pWrapContext->targetKey.eType == eOEM_HAL_KEYREGISTER_TYPE_AES128_PAIR
        ? &f_pWrapContext->targetKey.keyBuffer.pAes128Pair->rgbKeys[0]
        : &f_pWrapContext->targetKey.keyBuffer.pAes128RootPair->keyPair.rgbKeys[0],
        DRM_AES_BLOCKLEN * 2 );

    ChkDR( Oem_Aes_EcbEncryptData( pWrappingKey, rgbPlainTextKeyPair, DRM_AES_BLOCKLEN * 2 ) );

    OEM_SECURE_MEMCPY( f_pbWrappedKey, rgbPlainTextKeyPair, DRM_AES_BLOCKLEN * 2 );

    *f_pcbWrappedKey = DRM_AES_KEYSIZE_128_X2;

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( rgbPlainTextKeyPair, SIZEOF( rgbPlainTextKeyPair ) );
    return dr;
}

/*****************************************************************************
** Function:    Oem_HalOemImpl_WrapWithPublicKey
**
** Synopsis:    Decrypts data (a key) using a key stored in keyfile.
**
** Arguments:   [f_pCryptoContext]  : the crypto context pointer
**              [f_pWrapContext]    : A pointer to an initialized unwrap context
**              [f_pbWrappedKey]    : A pointer to the output buffer
**              [f_pcbWrappedKey]   : A pointer to a variable holding the size of the output
**                                    buffer. Populated with the required size of the buffer
**
** Returns:     DRM_SUCCESS          - on success
**              DRM_E_BUFFERTOOSMALL - if the buffer is NULL or too small
**              DRM_E_KEYFILE_UNKNOWN_DECRYPTION_METHOD - if invalid decryption method
**              if specified in the f_pcbEncryptedKey struct.
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_HalOemImpl_WrapWithPublicKey(
    __inout_ecount( 1 )                             DRM_CRYPTO_CONTEXT                  *f_pCryptoContext,
    __in                                      const OEM_HAL_WRAP_CONTEXT                *f_pWrapContext,
    __out_bcount_opt( *f_pcbWrappedKey )            DRM_BYTE                            *f_pbWrappedKey,
    __inout_ecount(1)                               DRM_DWORD                           *f_pcbWrappedKey )
{
    DRM_RESULT      dr  = DRM_SUCCESS;
    PLAINTEXT_P256  oToEncrypt;     /* Do not zero-init key material */

    ChkPtr( f_pCryptoContext );
    ChkPtr( f_pWrapContext );
    ChkPtr( f_pcbWrappedKey );


    /*
    ** The wrap context should be initialized, run a few checks anyway
    */
    ChkArg( f_pWrapContext->targetKey.eType   == eOEM_HAL_KEYREGISTER_TYPE_AES128_PAIR
         || f_pWrapContext->targetKey.eType   == eOEM_HAL_KEYREGISTER_TYPE_AES128_ROOT_PAIR );
    ChkArg( f_pWrapContext->wrappingKey.eType == eOEM_HAL_KEYREGISTER_TYPE_P256_PUBLIC );
    ChkArg( f_pWrapContext->eEncryptionType   == eOEM_HAL_CRYPTO_TYPE_ECC256_PUBLIC_KEY );

    DRMCASSERT( SIZEOF( PLAINTEXT_P256 ) == DRM_AES_KEYSIZE_128_X2 );

    if( f_pbWrappedKey == NULL
     || *f_pcbWrappedKey <  SIZEOF( CIPHERTEXT_P256 ) )
    {
        *f_pcbWrappedKey = SIZEOF( CIPHERTEXT_P256 );
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /* ECC P256 encrypt */
    switch( f_pWrapContext->targetKey.eType )
    {
    case eOEM_HAL_KEYREGISTER_TYPE_AES128_PAIR:
        /* You can't wrap a higher security level simple license with a lower security level certificate. */
        ChkBOOL( f_pWrapContext->targetKey.keyBuffer.pAes128Pair->dwSecurityLevelRequiredToWrapThisKey <= f_pWrapContext->wrappingKey.keyBuffer.pEcc256Public->dwSecurityLevel, DRM_E_DEVICE_SECURITY_LEVEL_TOO_LOW );

        OEM_SECURE_MEMCPY( &oToEncrypt, f_pWrapContext->targetKey.keyBuffer.pAes128Pair->rgbKeys, SIZEOF( PLAINTEXT_P256 ) );
        break;
    case eOEM_HAL_KEYREGISTER_TYPE_AES128_ROOT_PAIR:
        /* You can't wrap a higher security level root license with a lower security level certificate. */
        ChkBOOL( f_pWrapContext->targetKey.keyBuffer.pAes128RootPair->keyPair.dwSecurityLevelRequiredToWrapThisKey <= f_pWrapContext->wrappingKey.keyBuffer.pEcc256Public->dwSecurityLevel, DRM_E_DEVICE_SECURITY_LEVEL_TOO_LOW );

        if( f_pWrapContext->targetKey.keyBuffer.pAes128RootPair->pDerivedKey != NULL )
        {
            /*
            ** Because this is a scalable root license, we have to re-shuffle
            ** the currently un-shuffled CI/CK before we encrypt it with ECC P256.
            */
            DRM_DWORD        idx            = 0;
            DRM_DWORD        jdx            = 0;
            DRM_BYTE        *pbToEncrypt    = (DRM_BYTE*)&oToEncrypt;
            const DRM_BYTE  *pbSource       = (DRM_BYTE*)f_pWrapContext->targetKey.keyBuffer.pAes128RootPair->keyPair.rgbKeys;

            for( idx = 0; idx < DRM_AES_KEYSIZE_128; idx++ )
            {
                pbToEncrypt[ jdx++ ] = pbSource[ idx ];
                pbToEncrypt[ jdx++ ] = pbSource[ idx + DRM_AES_KEYSIZE_128 ];
            }
        }
        else
        {
            OEM_SECURE_MEMCPY( &oToEncrypt, f_pWrapContext->targetKey.keyBuffer.pAes128RootPair->keyPair.rgbKeys, SIZEOF( PLAINTEXT_P256 ) );
        }

        break;
    default:
        /*
        ** Code should never reach here
        */
        DRMASSERT( FALSE );
        ChkDR( DRM_E_LOGICERR );
    }

    ChkDR( OEM_ECC_Encrypt_P256(
        &f_pWrapContext->wrappingKey.keyBuffer.pEcc256Public->key,
        &oToEncrypt,
        ( CIPHERTEXT_P256* )f_pbWrappedKey,
        (struct bigctx_t *)f_pCryptoContext->rgbCryptoContext ) );

    *f_pcbWrappedKey = SIZEOF( CIPHERTEXT_P256 );

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( &oToEncrypt, SIZEOF( oToEncrypt ) );
    return dr;
}

#if DRM_TEST_EXPOSE_TEST_FUNCTIONS

static DRM_FRE_INLINE DRM_VOID _AssignEcc256KeyPairRaw(
    __inout_ecount( 1 )                             OEM_HAL_RAW_KEY_REGISTER            *f_pRegisterInfo,
    __in_ecount( 1 )                                OEM_HAL_ECC256_KEY_PAIR             *f_pKey ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_VOID _AssignEcc256KeyPairRaw(
    __inout_ecount( 1 )                             OEM_HAL_RAW_KEY_REGISTER            *f_pRegisterInfo,
    __in_ecount( 1 )                                OEM_HAL_ECC256_KEY_PAIR             *f_pKey )
{
    f_pRegisterInfo->eType                 = eOEM_HAL_KEYREGISTER_TYPE_P256_KEYPAIR;
    f_pRegisterInfo->keyBuffer.pEcc256Pair = f_pKey;
}

static DRM_FRE_INLINE DRM_VOID _AssignAes128KeyPairRaw(
    __inout_ecount( 1 )                             OEM_HAL_RAW_KEY_REGISTER            *f_pRegisterInfo,
    __in_ecount( 1 )                                OEM_HAL_AES_KEY_PAIR                *f_pKey ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_VOID _AssignAes128KeyPairRaw(
    __inout_ecount( 1 )                             OEM_HAL_RAW_KEY_REGISTER            *f_pRegisterInfo,
    __in_ecount( 1 )                                OEM_HAL_AES_KEY_PAIR                *f_pKey )
{
    f_pRegisterInfo->eType                 = eOEM_HAL_KEYREGISTER_TYPE_AES128_PAIR;
    f_pRegisterInfo->keyBuffer.pAes128Pair = f_pKey;
}

static DRM_FRE_INLINE DRM_VOID _AssignNullKeyRaw(
    __inout_ecount( 1 )                             OEM_HAL_RAW_KEY_REGISTER            *f_pRegisterInfo ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_VOID _AssignNullKeyRaw(
    __inout_ecount( 1 )                             OEM_HAL_RAW_KEY_REGISTER            *f_pRegisterInfo )
{
    f_pRegisterInfo->eType                 = eOEM_HAL_KEYREGISTER_TYPE_INVALID;
    f_pRegisterInfo->keyBuffer.pEcc256Pair = NULL;
}

/******************************************************************************
**
** This section implements test functions (declared in oemhaltest.h).
**
******************************************************************************/

static       DRM_BOOL     g_fEnableTestWrapUnwrap = TRUE;

/*********************************************************************
**
** Function:   OEM_HAL_TEST_EnableTestWrapUnwrap
**
** Synopsis:   Switch the test wrap/unwrap functionality on and off.
**
** Arguments:
**
** [f_fEnable]:  Flag to specify whether test wrap/unwrap key combinations
**               are allowed
**
** Notes:      By default, for the test version of the lib, test unwrap/wrap
**             behaviour is on, so this function must be called if strict
**             production behaviour is required.
**
**********************************************************************/
DRM_API DRM_VOID DRM_CALL OEM_HAL_TEST_EnableTestWrapUnwrap(
    __in DRM_BOOL f_fEnable )
{
    g_fEnableTestWrapUnwrap = f_fEnable;
}

/*********************************************************************
**
** Function:   OEM_HAL_TEST_CompareKeyMaterial
**
** Synopsis:   Compares the provided key material with a key register,
**             performing any internal transformation required to
**             unpack the key material to be equivalent to the register
**             (for instance, initializing an AES key).
**
** Arguments:
**
** [f_eKeyType]:         The key type to compare
** [f_keyRegisterIndex]: The key index to compare
** [f_pbKeyMaterial]:    The key material to compare
** [f_cbKeyMaterial]:    The length of the key material buffer
**
** Returns:    TRUE if key material is equivalent to the key in the register.
**             FALSE if key meterial is different.
**
** Notes:      None.
**
**********************************************************************/
DRM_API DRM_BOOL DRM_CALL OEM_HAL_TEST_CompareKeyMaterial(
    __in                                 OEM_HAL_KEY_TYPE            f_eKeyType,
    __in                                 OEM_HAL_KEY_REGISTER_INDEX  f_keyRegisterIndex,
    __in_bcount( f_cbKeyMaterial ) const DRM_BYTE                   *f_pbKeyMaterial,
    __in                                 DRM_DWORD                   f_cbKeyMaterial )
{
    DRM_BOOL   fEquivalent = FALSE;
    DRM_RESULT dr          = DRM_SUCCESS;

    ChkPtr( f_pbKeyMaterial );
    ChkDR( Oem_HalOemImpl_VerifyValidKeyRegister( f_eKeyType, f_keyRegisterIndex ) );

    switch( f_eKeyType )
    {
    case OEM_HAL_KEY_TYPE_AES_ECB_DOMAIN_PROTECT:
        if( f_cbKeyMaterial == ECC_P256_PLAINTEXT_SIZE_IN_BYTES )
        {
            if( OEM_SECURE_ARE_EQUAL( f_pbKeyMaterial, g_rgKeyRegistersAesEcbDomainProtect[f_keyRegisterIndex].rgbKeys, ECC_P256_PLAINTEXT_SIZE_IN_BYTES ) )
            {
                fEquivalent = TRUE;
            }
        }
        break;

    default:

        break;
    }

ErrorExit:
    return fEquivalent;
}

/*********************************************************************
**
** Function:   OEM_HAL_TEST_CompareKeys
**
** Synopsis:   Compares two key buffers with each other.
**
** Arguments:
**
** [f_eKeyType]:           The type of key
** [f_keyRegisterIndex1]:  The first key buffer to compare
** [f_keyRegisterIndex2]:  The second key buffer to compare
**
** Returns:    TRUE:  keys match.
**             FALSE: Keys are different
**
** Notes:      None.
**
**********************************************************************/
DRM_API DRM_BOOL DRM_CALL OEM_HAL_TEST_CompareKeys(
    __in                                 OEM_HAL_KEY_TYPE            f_eKeyType,
    __in                                 OEM_HAL_KEY_REGISTER_INDEX  f_keyRegisterIndex1,
    __in                                 OEM_HAL_KEY_REGISTER_INDEX  f_keyRegisterIndex2 )
{
    DRM_BOOL   fEquivalent = FALSE;
    DRM_RESULT dr          = DRM_SUCCESS;

    ChkDR( Oem_HalOemImpl_VerifyValidKeyRegister( f_eKeyType, f_keyRegisterIndex1 ) );
    ChkDR( Oem_HalOemImpl_VerifyValidKeyRegister( f_eKeyType, f_keyRegisterIndex2 ) );

    switch( f_eKeyType )
    {
    case OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT:
        if( OEM_SECURE_ARE_EQUAL(
            g_rgKeyRegistersEccP256ContentEncrypt[f_keyRegisterIndex1].privateKey.m_rgbPrivkey,
            g_rgKeyRegistersEccP256ContentEncrypt[f_keyRegisterIndex2].privateKey.m_rgbPrivkey,
            ECC_P256_INTEGER_SIZE_IN_BYTES ) )
        {
            fEquivalent = TRUE;
        }
        break;

    case OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT:
        if( OEM_SECURE_ARE_EQUAL(
            g_rgKeyRegistersEccP256DomainEncrypt[f_keyRegisterIndex1].privateKey.m_rgbPrivkey,
            g_rgKeyRegistersEccP256DomainEncrypt[f_keyRegisterIndex2].privateKey.m_rgbPrivkey,
            ECC_P256_INTEGER_SIZE_IN_BYTES ) )
        {
            fEquivalent = TRUE;
        }
        break;

    default:
        break;
    }

ErrorExit:
    return fEquivalent;
}

static DRM_DWORD g_nTEST_OemUnwrapCount = 0;

/*********************************************************************
**
** Function:   OEM_HAL_TEST_GetUnwrapCount
**
** Synopsis:   Allow test code to check how many times Oem_Hal_UnwrapKey was called since black box creation.
**
** Returns:    Number of OEM HAL unwraps.
**
** Notes:      None.
**
**********************************************************************/
DRM_API DRM_DWORD DRM_CALL OEM_HAL_TEST_GetUnwrapCount()
{
    return g_nTEST_OemUnwrapCount;
}

/*********************************************************************
**
** Function:   OEM_HAL_TEST_InitTestKeysUnwrap
**
** Synopsis:   Allow test code to unwrap a number of combinations
**             not permitted in production code
**
** Arguments:
**
** [f_eKeyType]:         The type of key to be unwrapped.
** [f_indexKey]        : The index of the register in which to place the key.
** [f_eWrappingKeyType]: The type of the wrapping key.
** [f_indexWrappingKey]: The index of the wrapping key.
** [f_cbWrappedKeyData]: The size of the supplied key data.
** [f_pUnwrapContext]  : The unwrap context to initialize
**
** Returns:    TRUE if key material is equivalent to the key in the register.
**             FALSE if key meterial is different.
**
** Notes:      None.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL OEM_HAL_TEST_InitTestKeysUnwrap(
    __in                                    OEM_HAL_KEY_TYPE            f_eKeyType,
    __in                                    OEM_HAL_KEY_REGISTER_INDEX  f_indexKey,
    __in                                    OEM_HAL_KEY_TYPE            f_eWrappingKeyType,
    __in                                    OEM_HAL_KEY_REGISTER_INDEX  f_indexWrappingKey,
    __in                                    DRM_DWORD                   f_cbWrappedKeyData,
    __out_ecount( 1 )                       OEM_HAL_UNWRAP_CONTEXT     *f_pUnwrapContext )
{
    DRM_RESULT dr = DRM_S_FALSE;

    g_nTEST_OemUnwrapCount++;

    if( g_fEnableTestWrapUnwrap )
    {
        ChkPtr( f_pUnwrapContext );

        OEM_SECURE_ZERO_MEMORY( f_pUnwrapContext, SIZEOF( OEM_HAL_UNWRAP_CONTEXT ) );

        /*
        ** Zero the register that is to be the target key
        */
        Oem_HalOemImpl_ZeroKeyRegister( f_eKeyType, f_indexKey );

        /*
        ** This switch statement contains all the valid combinations of key/unwrapping key pairs
        */
        switch( f_eKeyType )
        {
        case OEM_HAL_KEY_TYPE_ECC_P256_MODEL_SIGN:
            switch( f_eWrappingKeyType )
            {
            case OEM_HAL_KEY_TYPE_INVALID:
                /*
                ** Load device model signing key directly.
                */
                f_pUnwrapContext->eKeyDataType = eOEM_HAL_KEYDATA_TYPE_TEST_CLEAR_BYTES;

                _AssignEcc256KeyPairRaw( &f_pUnwrapContext->targetKey, &g_rgKeyRegistersEccP256ModelSign[f_indexKey] );
                _AssignNullKeyRaw( &f_pUnwrapContext->wrappingKey );

                dr = DRM_SUCCESS;
                break;
            }

            break;

        case OEM_HAL_KEY_TYPE_AES_CTR_CONTENT:
            switch( f_eWrappingKeyType )
            {
            case OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT:
                /*
                ** In the special case where a content buffer is just the ciphertext key, directly
                ** unwrap it without going through license validation as this key is being directly
                ** injected by test code
                */
                if( f_cbWrappedKeyData == SIZEOF( CIPHERTEXT_P256 ) )
                {
                    f_pUnwrapContext->eDecryptionType = eOEM_HAL_CRYPTO_TYPE_AES_KEY_PAIR;
                    f_pUnwrapContext->eKeyDataType    = eOEM_HAL_KEYDATA_TYPE_ENCRYPTED_BYTES;

                    _AssignAes128KeyPairRaw( &f_pUnwrapContext->targetKey, &g_rgKeyRegistersAesCtrContent[f_indexKey] );
                    _AssignEcc256KeyPairRaw( &f_pUnwrapContext->wrappingKey, &g_rgKeyRegistersEccP256ContentEncrypt[f_indexWrappingKey] );

                    dr = DRM_SUCCESS;
                }

                break;

            case OEM_HAL_KEY_TYPE_INVALID:
                /*
                ** Load AES CTR content key directly.
                */
                f_pUnwrapContext->eKeyDataType = eOEM_HAL_KEYDATA_TYPE_TEST_CLEAR_BYTES;

                _AssignAes128KeyPairRaw( &f_pUnwrapContext->targetKey, &g_rgKeyRegistersAesCtrContent[f_indexKey] );
                _AssignNullKeyRaw( &f_pUnwrapContext->wrappingKey );

                dr = DRM_SUCCESS;
                break;
            }

            break;

        default:
            break;
        }
    }

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function:   OEM_HAL_TEST_AddPublicKeyUsage
**
** Synopsis:   Manually overrides the defined public key by adding
**             the specified flag.
**
** Arguments:
**
** [f_eKeyType]:         Must be OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC
** [f_keyRegisterIndex]: The key index to modify
** [f_dwUsageAdd]:       The usage bits to add
**
** Returns:    DRM_SUCCESS if arguments valid.
**             DRM_E_INVALIDARG if arguments not valid.
**
** Notes:      None.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL OEM_HAL_TEST_AddPublicKeyUsage(
    __in OEM_HAL_KEY_TYPE            f_eKeyType,
    __in OEM_HAL_KEY_REGISTER_INDEX  f_keyRegisterIndex,
    __in DRM_DWORD                   f_dwUsageAdd )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_eKeyType == OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC );
    ChkDR( Oem_HalOemImpl_VerifyValidKeyRegister( f_eKeyType, f_keyRegisterIndex ) );

    g_rgKeyRegistersEccP256Public[f_keyRegisterIndex].dwKeyUsageBitmap |= f_dwUsageAdd;

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    OEM_HAL_TEST_InjectCleartextKey
**
** Synopsis:    Loads a key directly into a register
**
** Arguments:   [f_pUnwrapContext]   : A pointer to an initialized unwrap context
**              [f_pbWrappedKeyData] : A buffer containing key data
**              [f_cbWrappedKeyData] : the length of the data in f_pbWrappedKeyData
**
** Returns:     DRM_SUCCESS       - on success
**              DRM_E_INVALIDARG  - the key data isn't valid
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL OEM_HAL_TEST_InjectCleartextKey(
    __in                                    OEM_HAL_UNWRAP_CONTEXT *f_pUnwrapContext,
    __in_bcount( f_cbWrappedKeyData ) const DRM_BYTE               *f_pbWrappedKeyData,
    __in                                    DRM_DWORD               f_cbWrappedKeyData )
{
    DRM_RESULT  dr         = DRM_SUCCESS;
    DRM_BYTE   *pbRegister = NULL;

    ChkPtr( f_pUnwrapContext );
    ChkPtr( f_pbWrappedKeyData );
    ChkArg( f_cbWrappedKeyData == SIZEOF( PRIVKEY_P256 ) );
    ChkArg( f_pUnwrapContext->wrappingKey.eType == eOEM_HAL_KEYREGISTER_TYPE_INVALID );

    switch( f_pUnwrapContext->targetKey.eType )
    {
    case eOEM_HAL_KEYREGISTER_TYPE_P256_KEYPAIR:
        ChkArg( f_cbWrappedKeyData == SIZEOF( PRIVKEY_P256 ) );
        pbRegister = ( DRM_BYTE* )&f_pUnwrapContext->targetKey.keyBuffer.pEcc256Pair->privateKey;
        break;

    case eOEM_HAL_KEYREGISTER_TYPE_AES128_PAIR:
        ChkArg( f_cbWrappedKeyData == DRM_AES_KEYSIZE_128_X2 );
        pbRegister = ( DRM_BYTE* )&f_pUnwrapContext->targetKey.keyBuffer.pAes128Pair->rgbKeys;
        break;

    default:
        /*
        ** Should never reach here
        */
        DRMASSERT( FALSE );
        ChkDR( DRM_E_INVALIDARG );
        break;
    }

    OEM_SECURE_MEMCPY( pbRegister, f_pbWrappedKeyData, f_cbWrappedKeyData );

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL OEM_HAL_TEST_GenerateEcc256KeyPair(
    __inout_ecount( 1 )                             DRM_CRYPTO_CONTEXT                  *f_pContext,
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_indexKey )
{
    DRM_RESULT               dr       = DRM_SUCCESS;
    OEM_HAL_ECC256_KEY_PAIR *pKeyPair = NULL;

    switch( f_eKeyType )
    {
    case OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT:  /* Allow test code to generate domain keys */
        pKeyPair = &g_rgKeyRegistersEccP256DomainEncrypt[f_indexKey];
        break;

    default:
        ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
    }

    /*
    ** Always generate ECC-p256 encryptable keys: the 'restricted' set
    */
    ChkDR( OEM_ECC_GenKeyPairRestrictedPriv_P256( &pKeyPair->publicKey, &pKeyPair->privateKey, ( struct bigctx_t* )f_pContext ) );

    pKeyPair->fPublicKeyPresent = TRUE;

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL OEM_HAL_TEST_GenerateAes128KeyPair(
    __in                                            DRM_VOID                            *f_pOEMContext,
    __inout_ecount( 1 )                             DRM_CRYPTO_CONTEXT                  *f_pCryptoContext,
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_indexKey )
{
    DRM_RESULT            dr    = DRM_SUCCESS;
    OEM_HAL_AES_KEY_PAIR *pKeys = NULL;

    switch( f_eKeyType )
    {
    case OEM_HAL_KEY_TYPE_AES_ECB_ROOT:     /* Allow test code to generate root keys as if they came from a license */
        pKeys = &g_rgKeyRegistersAesEcbRoot[f_indexKey].keyPair;
        break;

    case OEM_HAL_KEY_TYPE_AES_CTR_CONTENT:  /* Allow test code to generate leaf/simple keys as if they came from a license */
        pKeys = &g_rgKeyRegistersAesCtrContent[f_indexKey];
        break;

    case OEM_HAL_KEY_TYPE_AES_ECB_DOMAIN_PROTECT: /* Allow test code to generate domain keys as if they came from a license */
        pKeys = &g_rgKeyRegistersAesEcbDomainProtect[f_indexKey];
        break;

    default:
        ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
    }

    ChkDR( _GenerateAes128KeyPairEncryptableWithEcc256( f_pOEMContext, f_pCryptoContext, &pKeys->rgbKeys[0][0] ) );

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL OEM_HAL_TEST_OpaqueBufferCreate(
    __out                                           OEM_OPAQUE_BUFFER_HANDLE            *f_phBuf )
{
    DRM_RESULT                           dr     = DRM_SUCCESS;
    OEM_OPAQUE_BUFFER_HANDLE_INTERNAL   *pBuf   = NULL;

    ChkPtr( f_phBuf );
    *f_phBuf = OEM_OPAQUE_BUFFER_HANDLE_INVALID;

    ChkMem( pBuf = (OEM_OPAQUE_BUFFER_HANDLE_INTERNAL*)Oem_MemAlloc( SIZEOF(OEM_OPAQUE_BUFFER_HANDLE_INTERNAL) ) );
    OEM_SECURE_ZERO_MEMORY( pBuf, SIZEOF(OEM_OPAQUE_BUFFER_HANDLE_INTERNAL) );

    DRMASSERT( pBuf->eType == eOEM_OPAQUE_BUFFER_HANDLE_INTERNAL_TYPE_INVALID );

    *f_phBuf = (OEM_OPAQUE_BUFFER_HANDLE)pBuf;
    pBuf     = NULL;

ErrorExit:
    SAFE_OEM_FREE( pBuf );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL OEM_HAL_TEST_OpaqueBufferCreateWithData(
    __in_bcount( f_cbData )                   const DRM_BYTE                            *f_pbData,
    __in                                            DRM_DWORD                            f_cbData,
    __out                                           OEM_OPAQUE_BUFFER_HANDLE            *f_phBuf )
{
    DRM_RESULT                           dr     = DRM_SUCCESS;
    OEM_OPAQUE_BUFFER_HANDLE_INTERNAL   *pBuf   = NULL;
    DRM_BYTE                            *pbData = NULL;

    ChkPtr( f_pbData );
    ChkArg( f_cbData != 0    );
    ChkPtr( f_phBuf );
    *f_phBuf = OEM_OPAQUE_BUFFER_HANDLE_INVALID;

    ChkMem( pBuf = (OEM_OPAQUE_BUFFER_HANDLE_INTERNAL*)Oem_MemAlloc( SIZEOF(OEM_OPAQUE_BUFFER_HANDLE_INTERNAL) ) );
    OEM_SECURE_ZERO_MEMORY( pBuf, SIZEOF(OEM_OPAQUE_BUFFER_HANDLE_INTERNAL) );

    ChkMem( pbData = (DRM_BYTE*)Oem_MemAlloc( f_cbData ) );
    OEM_SECURE_MEMCPY( pbData, f_pbData, f_cbData );

    pBuf->cbData = f_cbData;
    pBuf->eType  = eOEM_OPAQUE_BUFFER_HANDLE_INTERNAL_TYPE_CLEAR_DATA_SOURCE;
    pBuf->pbData = pbData;
    pbData       = NULL;
    *f_phBuf = (OEM_OPAQUE_BUFFER_HANDLE)pBuf;
    pBuf     = NULL;

ErrorExit:
    SAFE_OEM_FREE( pBuf );
    SAFE_OEM_FREE( pbData );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL OEM_HAL_TEST_OpaqueBufferDestroy(
    __out                                           OEM_OPAQUE_BUFFER_HANDLE            *f_phBuf )
{
    if( f_phBuf != NULL )
    {
        OEM_OPAQUE_BUFFER_HANDLE_INTERNAL *pBuf = (OEM_OPAQUE_BUFFER_HANDLE_INTERNAL*)*f_phBuf;
        if( *f_phBuf != OEM_OPAQUE_BUFFER_HANDLE_INVALID && pBuf != NULL )
        {
            SAFE_OEM_FREE( pBuf->pbData );
            SAFE_OEM_FREE( pBuf );
        }
        *f_phBuf = OEM_OPAQUE_BUFFER_HANDLE_INVALID;
    }
    return DRM_SUCCESS;
}

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: f_hBuf already const")
DRM_API DRM_RESULT DRM_CALL OEM_HAL_TEST_OpaqueBufferRead(
    __in                                      const OEM_OPAQUE_BUFFER_HANDLE             f_hBuf,
    __out_bcount( f_cbData )                        DRM_BYTE                            *f_pbData,
    __in                                            DRM_DWORD                            f_cbData )
{
    DRM_RESULT                               dr   = DRM_SUCCESS;
    const OEM_OPAQUE_BUFFER_HANDLE_INTERNAL *pBuf = (const OEM_OPAQUE_BUFFER_HANDLE_INTERNAL*)f_hBuf;

    ChkArg( f_hBuf   != OEM_OPAQUE_BUFFER_HANDLE_INVALID );
    ChkPtr( pBuf );
    ChkPtr( f_pbData );
    ChkArg( f_cbData == pBuf->cbData );
    ChkArg( pBuf->eType == eOEM_OPAQUE_BUFFER_HANDLE_INTERNAL_TYPE_CLEAR_DATA_RENDER );

    OEM_SECURE_MEMCPY( f_pbData, pBuf->pbData, f_cbData );

ErrorExit:
    return dr;
}
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */

DRM_API DRM_RESULT DRM_CALL OEM_HAL_TEST_GenerateAes128Key(
    __in                                            DRM_VOID                            *f_pOEMContext,
    __inout_ecount( 1 )                             DRM_CRYPTO_CONTEXT                  *f_pCryptoContext,
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_indexKey )
{
#if DRM_SUPPORT_SECUREOEMHAL
    return DRM_E_NOTIMPL;
#else  /* DRM_SUPPORT_SECUREOEMHAL */
    DRM_RESULT               dr         = DRM_SUCCESS;
    DRM_BYTE                *prgbKey    = NULL;
    DRM_WORD                 cbKey      = DRM_AES_KEYSIZE_128;
    OEM_HAL_AES_KEY_PAIR     oKeyPair;  /* Don't initialize key material */

    switch( f_eKeyType )
    {
    case OEM_HAL_KEY_TYPE_LEGACY:
        prgbKey = g_rgKeyRegistersLegacy[f_indexKey].rgbKeyBuffer;
        break;

    case OEM_HAL_KEY_TYPE_AES_SLK:
        prgbKey = g_rgKeyRegistersAesSlk[f_indexKey].rgbKey;
        break;

    default:
        ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
    }

    /*
    ** Randomly generate an ECC P256 encryptable pair of AES keys
    ** There is ~.01% change that this will fail (the entropy may fail to give a legit value), hence the loop.
    ** Any other error should fall out of the loop and be caught by the ChkDR below.
    ** See OEM_ECC_GenerateHMACKey_P256 declaration for more info on this failure case.
    */
    DRMCASSERT( SIZEOF( PLAINTEXT_P256 ) == DRM_AES_KEYSIZE_128 * 2          );
    DRMCASSERT( SIZEOF( PLAINTEXT_P256 ) == DRM_AES_KEYSIZE_128_X2           );
    DRMCASSERT( SIZEOF( PLAINTEXT_P256 ) == ECC_P256_PLAINTEXT_SIZE_IN_BYTES );

    do
    {
        ChkDR( Oem_Random_GetBytes(
            f_pOEMContext,
            &oKeyPair.rgbKeys[0][0],
            ECC_P256_PLAINTEXT_SIZE_IN_BYTES ) );

        dr = OEM_ECC_GenerateHMACKey_P256(
            (PLAINTEXT_P256*)oKeyPair.rgbKeys,
            (struct bigctx_t *)f_pCryptoContext->rgbCryptoContext );

    } while( dr == DRM_E_P256_HMAC_KEYGEN_FAILURE );

    ChkDR( dr );

    /* Keep only the first 128 bits */
    OEM_SECURE_MEMCPY( prgbKey, &oKeyPair.rgbKeys[0][0], cbKey );

ErrorExit:
    _ZeroAesKeyPair( &oKeyPair );
    return dr;
#endif /* DRM_SUPPORT_SECUREOEMHAL */
}

DRM_API DRM_RESULT DRM_CALL OEM_HAL_TEST_EnableSkipLeafCertificateValidityCheckDuringSignOperation()
{
    g_fSkipLeafCertificateValidityCheckDuringSignOperation = TRUE;
    return DRM_SUCCESS;
}

DRM_API DRM_RESULT DRM_CALL OEM_HAL_TEST_EnableSkipLeafCertificateKeysValidityCheckDuringSignOperation()
{
    g_fSkipLeafCertificateKeysValidityCheckDuringSignOperation = TRUE;
    return DRM_SUCCESS;
}

DRM_API DRM_RESULT DRM_CALL OEM_HAL_TEST_DisableSkipLeafCertificateKeysValidityCheckDuringSignOperation()
{
    g_fSkipLeafCertificateKeysValidityCheckDuringSignOperation = FALSE;
    return DRM_SUCCESS;
}

#else /* DRM_TEST_EXPOSE_TEST_FUNCTIONS */

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: DRM_CRYPTO_CONTEXT should never be const")

DRM_API DRM_RESULT DRM_CALL OEM_HAL_TEST_GenerateEcc256KeyPair(
    __inout_ecount( 1 )                             DRM_CRYPTO_CONTEXT                  *f_pContext,
    __in                                            OEM_HAL_KEY_TYPE                     f_eKeyType,
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_indexKey )
{
    UNREFERENCED_PARAMETER( f_pContext );
    UNREFERENCED_PARAMETER( f_eKeyType );
    UNREFERENCED_PARAMETER( f_indexKey );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL OEM_HAL_TEST_EnableSkipLeafCertificateValidityCheckDuringSignOperation()
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL OEM_HAL_TEST_EnableSkipLeafCertificateKeysValidityCheckDuringSignOperation()
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL OEM_HAL_TEST_DisableSkipLeafCertificateKeysValidityCheckDuringSignOperation()
{
    return DRM_E_NOTIMPL;
}

PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */

#endif /* DRM_TEST_EXPOSE_TEST_FUNCTIONS */

EXIT_PK_NAMESPACE_CODE;

