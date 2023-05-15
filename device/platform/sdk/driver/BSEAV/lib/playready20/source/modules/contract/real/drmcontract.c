/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include "drmcontract.h"

ENTER_PK_NAMESPACE_CODE;

/*
** Set of error codes listed for each API. DRM_SUCCESS is used as the terminator of each API list.
**
** IF YOU MODIFY/ADD ANY OF THESE DATA STRUCTURES, YOU MUST ALSO ENSURE THAT THE CHM FILE IS ACCURATE.
*/

DRM_RESULT g_rgdrGenericErrorList[] =
{
    DRM_E_INVALIDARG,
    DRM_E_BUFFERTOOSMALL,
    DRM_E_DSTBLOCKMISMATCH,
    DRM_E_DSTCORRUPTED,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_Reinitialize[] =
{
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_Content_UpdateEmbeddedStore[] =
{
    DRM_E_CH_INCOMPATIBLE_HEADER_TYPE,
    DRM_E_HEADER_NOT_SET,
    DRM_S_FALSE,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_Content_UpdateEmbeddedStore_Commit[] =
{
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_Content_SetProperty[] =
{
    DRM_E_CH_INVALID_HEADER,
    DRM_E_CH_INCOMPATIBLE_HEADER_TYPE,
    DRM_E_HEADER_ALREADY_SET,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_Content_GetProperty[] =
{
    DRM_E_CH_INVALID_HEADER,
    DRM_E_HEADER_NOT_SET,
    DRM_E_CH_INCOMPATIBLE_HEADER_TYPE,
    DRM_E_CH_KID_MISSING,
    DRM_E_XMLNOTFOUND,
    DRM_E_RECORD_NOT_FOUND,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_LicenseAcq_GenerateChallenge[] =
{
    DRM_E_V1_NOT_SUPPORTED,
    DRM_E_HEADER_NOT_SET,
    DRM_E_SYNC_ENTRYNOTFOUND,
    DRM_E_NO_URL,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_Device_GetProperty[] =
{
    DRM_E_INVALIDDEVICECERTIFICATETEMPLATE,
    DRM_E_DEVCERTREADERROR,
    DRM_E_UNKNOWN_DEVICE_PROPERTY,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_SyncListItem_InitEnum[] =
{
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_SyncListItem_EnumNext[] =
{
    DRM_E_SYNC_ENTRYNOTFOUND,
    DRM_E_NOMORE,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_LicenseAcq_ProcessResponse[] =
{
    DRM_E_FAILED_TO_STORE_LICENSE,
    DRM_E_NOT_ALL_STORED,
    DRM_E_DRMNOTINIT,
    DRM_E_REVOCATION_BUFFERTOOSMALL,
    DRM_E_CERTIFICATE_REVOKED,
    DRM_E_PRIVKEYREADERROR,
    DRM_E_INVALID_REVOCATION_LIST,
    DRM_E_XMLNOTFOUND,
    DRM_E_SOAPXML_XML_FORMAT,
    DRM_E_XMLSIG_ECDSA_VERIFY_FAILURE,
    DRM_E_INVALIDLICENSE,
    DRM_E_UNKNOWN_BINDING_KEY,
    DRM_E_LICACQ_TOO_MANY_LICENSES,
    DRM_E_INVALID_LICENSE_RESPONSE_SIGNATURE,
    DRM_E_INVALID_LICENSE_RESPONSE_RESPONSEID,
    DRM_E_LICENSE_RESPONSE_SIGNATURE_MISSING,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_License_GetProperty[] =
{
    DRM_E_LICENSENOTBOUND,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_Reader_Commit[] =
{
    DRM_E_DRMNOTINIT,
    DRM_E_LICENSENOTBOUND,
    DRM_E_PRIVKEYREADERROR,
    DRM_E_METERING_STORE_CORRUPT,
    DRM_E_METERCERTNOTFOUND,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_Reader_InitDecrypt[] =
{
    DRM_E_CIPHER_NOTINITIALIZED,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_Reader_Decrypt[] =
{
    DRM_E_CIPHER_NOTINITIALIZED,
    DRM_E_DECRYPT_NOTINITIALIZED,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_LicenseQuery_GetState[] =
{
    DRM_E_HEADER_NOT_SET,
    DRM_E_DRMNOTINIT,
    DRM_E_LIC_CHAIN_TOO_DEEP,
    DRM_E_CH_KID_MISSING,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_MeterCert_InitEnum[] =
{
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_MeterCert_EnumNext[] =
{
    DRM_E_METERCERTNOTFOUND,
    DRM_E_NOMORE,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_MeterCert_Update[] =
{
    DRM_E_METERCERTNOTFOUND,
    DRM_E_INVALID_METERCERT_SIGNATURE,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_MeterCert_Delete[] =
{
    DRM_E_METERCERTNOTFOUND,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_Metering_GenerateChallenge[] =
{
    DRM_E_DRMNOTINIT,
    DRM_E_INVALID_METERCERT_SIGNATURE,
    DRM_E_METERING_NOT_SUPPORTED,
    DRM_E_CERTIFICATE_REVOKED,
    DRMUTIL_UNSUPPORTED_VERSION,
    DRM_E_DEVCERTEXCEEDSIZELIMIT,
    DRM_E_BUFFERTOOSMALL,
    DRM_E_NOMORE,
    DRM_S_FALSE,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_LicenseAcq_GetContentHeader[] =
{
    DRM_E_DRMNOTINIT,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_Sync_GenerateChallenge[] =
{
    DRM_E_DRMNOTINIT,
    DRM_E_NOMORE,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_Metering_ProcessResponse[] =
{
    DRM_E_DRMNOTINIT,
    DRM_E_METERING_NOT_SUPPORTED,
    DRM_E_METERSTORE_DATA_NOT_FOUND,
    DRM_E_METERING_MID_MISMATCH,
    DRM_E_METERING_INVALID_COMMAND,
    DRM_E_INVALID_METERRESPONSE_SIGNATURE,
    DRM_E_METERCERTNOTFOUND,
    DRM_E_CERTIFICATE_REVOKED,
    DRM_E_PROTOCOL_VERSION_NOT_SUPPORTED,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_SecureClock_ProcessResponse[] =
{
    DRM_E_CLK_NOT_SUPPORTED,
    DRM_E_PRIVKEYREADERROR,
    DRM_E_CLK_INVALID_RESPONSE,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_SecureClock_GenerateChallenge[] =
{
    DRM_E_CLK_NOT_SUPPORTED,
    DRM_E_PRIVKEYREADERROR,
    DRM_E_DEVCERTREADERROR,
    DRM_E_INVALIDDEVICECERTIFICATETEMPLATE,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_SecureClock_GetValue[] =
{
    DRM_E_CLK_NOT_SUPPORTED,
    DRM_E_PRIVKEYREADERROR,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_Reader_Bind[] =
{
    DRM_E_HEADER_NOT_SET,
    DRM_E_LICENSENOTFOUND,
    DRM_E_CLK_NOT_SET,
    DRM_E_NO_CLK_SUPPORTED,
    DRM_E_CONDITIONNOTSUPPORTED,
    DRM_E_INVALIDLICENSE,
    DRM_E_LICENSEEXPIRED,
    DRM_E_RIGHTSNOTAVAILABLE,
    DRM_E_NO_OPL_CALLBACK,
    DRM_E_LICEVAL_REQUIRED_REVOCATION_LIST_NOT_AVAILABLE,
    DRM_E_RIV_TOO_SMALL,
    DRM_E_INVALID_REVOCATION_LIST,
    DRM_E_EXTENDED_RESTRICTION_NOT_UNDERSTOOD,
    DRM_E_DOMAIN_BIND_LICENSE,
    DRM_E_V1_LICENSE_CHAIN_NOT_SUPPORTED,
    DRM_E_CH_BAD_KEY,
    DRM_E_LIC_KEY_DECODE_FAILURE,
    DRM_E_UNABLE_TO_RESOLVE_LOCATION_TREE,
    DRM_E_UPLINKLICENSENOTFOUND,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_StoreMgmt_DeleteLicenses[] =
{
    DRM_E_LICENSENOTFOUND,
    DRM_E_LOGICERR,
    DRM_E_CH_KID_MISSING,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_ProcessRequest[] =
{
    DRM_E_NOTIMPL,
    DRM_SUCCESS
};
DRM_RESULT g_rgdrDrm_ProcessCommand[] =
{
    DRM_E_INVALID_LICENSE_REVOCATION_LIST_SIGNATURE,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_MeterCert_Invalidate[] =
{
    DRM_E_METERCERTNOTFOUND,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_StoreMgmt_CleanupStore[] =
{
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_LicenseQuery_IsAllowed[] =
{
    DRM_E_INVALIDRIGHT,
    DRM_E_HEADER_NOT_SET,
    DRM_E_LICENSENOTFOUND,
    DRM_E_CLK_NOT_SET,
    DRM_E_NO_CLK_SUPPORTED,
    DRM_E_CONDITIONNOTSUPPORTED,
    DRM_E_INVALIDLICENSE,
    DRM_E_LICENSEEXPIRED,
    DRM_E_RIGHTSNOTAVAILABLE,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_Uninitialize[] =
{
    DRM_E_INVALIDLICENSESTORE,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_Initialize[] =
{
    DRM_E_DEVCERTREADERROR,
    DRM_E_INVALIDDEVICECERTIFICATETEMPLATE,
    DRM_E_INVALIDDEVICECERTIFICATE,
    DRM_E_INVALIDLICENSESTORE,
    DRM_E_CLK_INVALID_DATE,
    DRM_E_BCERT_ISSUERKEY_KEYINFO_MISMATCH,
    DRM_E_KEYFILE_KEY_NOT_FOUND,
    DRM_E_ACTIVATION_REQUIRED,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_MeterCert_GenerateChallenge[] =
{
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_MeterCert_ProcessResponse[] =
{
    DRM_E_XMLNOTFOUND,
    DRM_E_SOAPXML_XML_FORMAT,
    DRM_E_METERCERTNOTFOUND,
    DRM_E_CERTIFICATE_REVOKED,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_JoinDomain_GenerateChallenge[] =
{
    DRM_E_DEVCERTEXCEEDSIZELIMIT,
    DRM_E_DOMAIN_INVALID_CUSTOM_DATA_TYPE,
    DRM_E_DOMAIN_INVALID_CUSTOM_DATA,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_JoinDomain_ProcessResponse[] =
{
    DRM_E_SOAPXML_XML_FORMAT,
    DRM_E_XMLNOTFOUND,
    DRM_S_MORE_DATA,
    DRM_E_PROTOCOL_VERSION_NOT_SUPPORTED,
    DRM_E_INVALID_SIGNATURE,
    DRM_E_INVALID_DOMAIN_JOIN_RESPONSE_SIGNATURE,
    DRM_E_DOMAIN_JOIN_RESPONSE_SIGNATURE_MISSING,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_LeaveDomain_GenerateChallenge[] =
{
    DRM_E_DEVCERTEXCEEDSIZELIMIT,
    DRM_E_DOMAIN_INVALID_CUSTOM_DATA_TYPE,
    DRM_E_DOMAIN_INVALID_CUSTOM_DATA,
    DRM_E_DOMAIN_NOT_FOUND,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_LeaveDomain_ProcessResponse[] =
{
    DRM_E_SOAPXML_XML_FORMAT,
    DRM_E_XMLNOTFOUND,
    DRM_S_MORE_DATA,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_DomainCert_Find[] =
{
    DRM_E_DOMAIN_STORE_GET_DATA,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_DomainCert_InitEnum[] =
{
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_DomainCert_EnumNext[] =
{
    DRM_E_NOMORE,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_GetAdditionalResponseData[] =
{
    DRM_E_SOAPXML_DATA_NOT_FOUND,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_LicenseAcq_GenerateAck[] =
{
    DRM_SUCCESS,
    DRM_S_FALSE
};

DRM_RESULT g_rgdrDrm_LicenseAcq_ProcessAckResponse[] =
{
    DRM_E_SOAPXML_XML_FORMAT,
    DRM_E_XMLNOTFOUND,
    DRM_S_MORE_DATA,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_Envelope_Close[] =
{
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_Envelope_DuplicateFileContext[] =
{
    DRM_E_FILEOPEN,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_Envelope_GetSize[] =
{
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_Envelope_GetOriginalFileName[] =
{
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_Envelope_InitializeRead[] =
{
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_Envelope_Open[] =
{
    DRM_E_FILEOPEN,
    DRM_E_ENVELOPE_CORRUPT,
    DRM_E_CH_INVALID_HEADER,
    DRM_E_ENVELOPE_FILE_NOT_COMPATIBLE,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_Envelope_Read[] =
{
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_Envelope_Seek[] =
{
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_Envelope_WritePlayReadyObject[] =
{
    DRM_E_HEADER_NOT_SET,
    DRM_E_CH_INCOMPATIBLE_HEADER_TYPE,
    DRM_E_FILEOPEN,
    DRM_E_ENVELOPE_CORRUPT,
    DRM_E_FILESEEKERROR,
    DRM_E_FILEWRITEERROR,
    DRM_SUCCESS
};

DRM_RESULT g_rgdrDrm_PlayReadyObject_ConvertFromWmdrmHeader[] =
{
    DRM_E_CH_INCOMPATIBLE_HEADER_TYPE,
    DRM_SUCCESS
};

/*
** The order of this list must exactly match that of the ECC_DRM_API_List enum in 'drmcontract.h'
*/
DRM_RESULT * const g_prgdrErrorCodelist[] =
{
    g_rgdrGenericErrorList,

    g_rgdrDrm_Content_UpdateEmbeddedStore,
    g_rgdrDrm_Content_SetProperty,
    g_rgdrDrm_Device_GetProperty,
    g_rgdrDrm_DomainCert_EnumNext,
    g_rgdrDrm_DomainCert_Find,
    g_rgdrDrm_DomainCert_InitEnum,
    g_rgdrDrm_Envelope_Close,
    g_rgdrDrm_Envelope_DuplicateFileContext,
    g_rgdrDrm_Envelope_GetSize,
    g_rgdrDrm_Envelope_GetOriginalFileName,
    g_rgdrDrm_Envelope_InitializeRead,
    g_rgdrDrm_Envelope_Open,
    g_rgdrDrm_Envelope_Read,
    g_rgdrDrm_Envelope_Seek,
    g_rgdrDrm_GetAdditionalResponseData,
    g_rgdrDrm_Initialize,
    g_rgdrDrm_JoinDomain_GenerateChallenge,
    g_rgdrDrm_JoinDomain_ProcessResponse,
    g_rgdrDrm_LeaveDomain_GenerateChallenge,
    g_rgdrDrm_LeaveDomain_ProcessResponse,
    g_rgdrDrm_License_GetProperty,
    g_rgdrDrm_LicenseAcq_GenerateChallenge,
    g_rgdrDrm_LicenseAcq_GenerateAck,
    g_rgdrDrm_LicenseAcq_GetContentHeader,
    g_rgdrDrm_LicenseAcq_ProcessAckResponse,
    g_rgdrDrm_LicenseAcq_ProcessResponse,
    g_rgdrDrm_LicenseQuery_GetState,
    g_rgdrDrm_LicenseQuery_IsAllowed,
    g_rgdrDrm_MeterCert_Delete,
    g_rgdrDrm_MeterCert_EnumNext,
    g_rgdrDrm_MeterCert_GenerateChallenge,
    g_rgdrDrm_MeterCert_InitEnum,
    g_rgdrDrm_MeterCert_Invalidate,
    g_rgdrDrm_MeterCert_ProcessResponse,
    g_rgdrDrm_MeterCert_Update,
    g_rgdrDrm_Metering_GenerateChallenge,
    g_rgdrDrm_Metering_ProcessResponse,
    g_rgdrDrm_ProcessCommand,
    g_rgdrDrm_ProcessRequest,
    g_rgdrDrm_Reader_Bind,
    g_rgdrDrm_Reader_Commit,
    g_rgdrDrm_Reader_Decrypt,
    g_rgdrDrm_Reader_InitDecrypt,
    g_rgdrDrm_Reinitialize,
    g_rgdrDrm_SecureClock_GenerateChallenge,
    g_rgdrDrm_SecureClock_GetValue,
    g_rgdrDrm_SecureClock_ProcessResponse,
    g_rgdrDrm_StoreMgmt_CleanupStore,
    g_rgdrDrm_StoreMgmt_DeleteLicenses,
    g_rgdrDrm_Sync_GenerateChallenge,
    g_rgdrDrm_SyncListItem_EnumNext,
    g_rgdrDrm_SyncListItem_InitEnum,
    g_rgdrDrm_Uninitialize,
    g_rgdrDrm_Content_GetProperty,
    g_rgdrDrm_Envelope_WritePlayReadyObject,
    g_rgdrDrm_Content_UpdateEmbeddedStore_Commit,
    g_rgdrDrm_PlayReadyObject_ConvertFromWmdrmHeader
};

/******************************************************************************
** Function         :   _ECC_TableLookUp
**
** Synopsis         :   The function which does the actual look up and determines
**                      if the error code is present in the contract or not.
**
** ARGUMENTS        :   [f_uiAPIid] - # defined value of the API.
**                      [f_dr]      - dr value returned by the respective function.
**
** RETURN           :   TRUE   if the error code is present in the contract.
**                      FALSE  if the error code is not present in the contract.
**
******************************************************************************/
static DRM_BOOL DRM_CALL _ECC_TableLookUp(
    __in const DRM_DWORD  f_uiAPIid,
    __in const DRM_RESULT f_dr )
{
    DRM_BOOL fValue  = FALSE;
    DRM_DWORD uiVar  = 0;
    while( g_prgdrErrorCodelist[f_uiAPIid][uiVar] != DRM_SUCCESS )
    {
        if( g_prgdrErrorCodelist[f_uiAPIid][uiVar] == f_dr )
        {
            fValue = TRUE;
            break;
        }
    uiVar++;
    }
    return fValue;
}



/******************************************************************************
** Function         :   OEM_ECC_ExpectedErrorCode
**
** Synopsis         :   Check the dr returned by the function against the corresponding
**                      API's expected error code list and the generic error code list.
**
** ARGUMENTS        :   [f_uiAPIid]  - # defined value of the API.
**                      [f_dr]       - dr value returned by the respective function.
**
** RETURN           :   TRUE   if the error code is present in the contract.
**                      FALSE  if the error code is not present in the contract.
**
******************************************************************************/
DRM_API DRM_BOOL DRM_CALL OEM_ECC_ExpectedErrorCode(
    __in const  DRM_DWORD   f_uiAPIid,
    __in const  DRM_RESULT  f_dr )
{
    DRM_BOOL fExist = FALSE;

    /*
    ** Check the generic error list
    */
    fExist = _ECC_TableLookUp( ECC_DRM_GENERIC_ERROR_LIST, f_dr );

    /*
    ** If the entry is not there in the generic error list, Check the API errorcode list
    */
    if( !fExist )
    {
        fExist = _ECC_TableLookUp( f_uiAPIid, f_dr );
    }
    return fExist;
}

EXIT_PK_NAMESPACE_CODE;

