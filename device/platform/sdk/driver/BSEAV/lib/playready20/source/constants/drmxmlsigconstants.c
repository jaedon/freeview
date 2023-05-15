/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmtypes.h>
#include <drmxmlsigconstants.h>

ENTER_PK_NAMESPACE_CODE;

/* A well known WMRM ECC-256 public key */
const DRM_DISCARDABLE PUBKEY_P256 g_oWMRMECC256PubKey =
{
    0XC8, 0XB6, 0XAF, 0X16, 0XEE, 0X94, 0X1A, 0XAD,
    0XAA, 0X53, 0X89, 0XB4, 0XAF, 0X2C, 0X10, 0XE3,
    0X56, 0XBE, 0X42, 0XAF, 0X17, 0X5E, 0XF3, 0XFA,
    0XCE, 0X93, 0X25, 0X4E, 0X7B, 0X0B, 0X3D, 0X9B,
    0X98, 0X2B, 0X27, 0XB5, 0XCB, 0X23, 0X41, 0X32,
    0X6E, 0X56, 0XAA, 0X85, 0X7D, 0XBF, 0XD5, 0XC6,
    0X34, 0XCE, 0X2C, 0XF9, 0XEA, 0X74, 0XFC, 0XA8,
    0XF2, 0XAF, 0X59, 0X57, 0XEF, 0XEE, 0XA5, 0X62
};

/*
** Well known Move Service ECC-256 public key
** Public key that should be used to validate both SOAP responses
** from the move service as well as move indicies.
*/
const DRM_DISCARDABLE PUBKEY_P256 g_pubkeyMoveService =
{
    0x6F, 0xD5, 0x4F, 0x22, 0xFB, 0x9D, 0x13, 0x95,
    0x6A, 0x12, 0xD7, 0x0B, 0xD1, 0x11, 0x51, 0x57,
    0xB3, 0x23, 0x7B, 0xC0, 0xFC, 0x19, 0xB8, 0x6C,
    0x40, 0x54, 0x19, 0x5F, 0x62, 0x66, 0xA1, 0x76,
    0x6F, 0xD5, 0xE2, 0xDF, 0xDA, 0x31, 0xA5, 0xC2,
    0x66, 0x37, 0x3A, 0x81, 0x9E, 0xE8, 0x0E, 0xE3,
    0xDC, 0x96, 0xC3, 0x3C, 0xBD, 0x0E, 0x6F, 0xD7,
    0xA8, 0x0D, 0x15, 0x98, 0xF3, 0xCA, 0x21, 0x2B,
};

/*
** -------------------------------------------------------------
** XML strings used in the construction of signature XML segment
** -------------------------------------------------------------
*/
/* "Signature" */
const DRM_CHAR g_rgchSigRootTag [] = "Signature";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigRootTag = CREATE_DRM_ANSI_STRING_EX( g_rgchSigRootTag, 9 );


/* "SignedInfo" */
const DRM_CHAR g_rgchSigSignedInfoTag [] = "SignedInfo";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigSignedInfoTag = CREATE_DRM_ANSI_STRING_EX( g_rgchSigSignedInfoTag, 10 );


/* "CanonicalizationMethod" */
const DRM_CHAR g_rgchSigCanonicalizationMethodTag [] = "CanonicalizationMethod";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigCanonicalizationMethodTag = CREATE_DRM_ANSI_STRING_EX( g_rgchSigCanonicalizationMethodTag, 22 );


/* "SignatureMethod" */
const DRM_CHAR g_rgchSigSignatureMethodTag [] = "SignatureMethod";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigSignatureMethodTag = CREATE_DRM_ANSI_STRING_EX( g_rgchSigSignatureMethodTag, 15 );


/* "Reference" */
const DRM_CHAR g_rgchSigReferenceTag [] = "Reference";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigReferenceTag = CREATE_DRM_ANSI_STRING_EX( g_rgchSigReferenceTag, 9 );


/* "DigestMethod" */
const DRM_CHAR g_rgchSigDigestMethodTag [] = "DigestMethod";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigDigestMethodTag = CREATE_DRM_ANSI_STRING_EX( g_rgchSigDigestMethodTag, 12 );


/* "DigestValue" */
const DRM_CHAR g_rgchSigDigestValueTag [] = "DigestValue";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigDigestValueTag = CREATE_DRM_ANSI_STRING_EX( g_rgchSigDigestValueTag, 11 );


/* "SignatureValue" */
const DRM_CHAR g_rgchSigSignatureValueTag [] = "SignatureValue";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigSignatureValueTag = CREATE_DRM_ANSI_STRING_EX( g_rgchSigSignatureValueTag, 14 );


/* "KeyInfo" */
const DRM_CHAR g_rgchSigKeyInfoTag [] = "KeyInfo";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigKeyInfoTag = CREATE_DRM_ANSI_STRING_EX( g_rgchSigKeyInfoTag, 7 );


/* "KeyValue" */
const DRM_CHAR g_rgchSigKeyValueTag [] = "KeyValue";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigKeyValueTag = CREATE_DRM_ANSI_STRING_EX( g_rgchSigKeyValueTag, 8 );


/* "ECCKeyValue" */
const DRM_CHAR g_rgchSigECCKeyValueTag [] = "ECCKeyValue";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigECCKeyValueTag = CREATE_DRM_ANSI_STRING_EX( g_rgchSigECCKeyValueTag, 11 );


/* "PublicKey" */
const DRM_CHAR g_rgchSigPublicKeyTag [] = "PublicKey";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigPublicKeyTag = CREATE_DRM_ANSI_STRING_EX( g_rgchSigPublicKeyTag, 9 );



/* "xmlns" */
const DRM_CHAR g_rgchSigRootAttribName [] = "xmlns";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigRootAttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchSigRootAttribName, 5 );

/* "http://www.w3.org/2000/09/xmldsig#" */
const DRM_CHAR g_rgchSigRootAttribValue [] = "http://www.w3.org/2000/09/xmldsig#";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigRootAttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchSigRootAttribValue, 34 );


/* "xmlns" */
const DRM_CHAR g_rgchSigSignedInfoAttribName [] = "xmlns";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigSignedInfoAttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchSigSignedInfoAttribName, 5 );

/* "xmlns:m" */
const DRM_CHAR  g_rgchRSAPrefixMSLP                [] = "xmlns:m";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrRSAPrefixMSLP = CREATE_DRM_ANSI_STRING_EX( g_rgchRSAPrefixMSLP, 7 );

/* "http://www.w3.org/2000/09/xmldsig#" */
const DRM_CHAR g_rgchSigSignedInfoAttribValue [] = "http://www.w3.org/2000/09/xmldsig#";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigSignedInfoAttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchSigSignedInfoAttribValue, 34 );


/* "Algorithm" */
const DRM_CHAR g_rgchSigCanonicalizationMethodAttribName [] = "Algorithm";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigCanonicalizationMethodAttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchSigCanonicalizationMethodAttribName, 9 );

/* "http://www.w3.org/TR/2001/REC-xml-c14n-20010315" */
const DRM_CHAR g_rgchSigCanonicalizationMethodAttribValue [] = "http://www.w3.org/TR/2001/REC-xml-c14n-20010315";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigCanonicalizationMethodAttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchSigCanonicalizationMethodAttribValue, 47 );


/* "Algorithm" */
const DRM_CHAR g_rgchSigSignatureMethodAttribName [] = "Algorithm";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigSignatureMethodAttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchSigSignatureMethodAttribName, 9 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols#ecdsa-sha256" */
const DRM_CHAR g_rgchSigSignatureMethodAttribValue [] = "http://schemas.microsoft.com/DRM/2007/03/protocols#ecdsa-sha256";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigSignatureMethodAttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchSigSignatureMethodAttribValue, 63 );


/* "Algorithm" */
const DRM_CHAR g_rgchSigDigestMethodAttribName [] = "Algorithm";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigDigestMethodAttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchSigDigestMethodAttribName, 9 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols#sha256" */
const DRM_CHAR g_rgchSigDigestMethodAttribValue [] = "http://schemas.microsoft.com/DRM/2007/03/protocols#sha256";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigDigestMethodAttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchSigDigestMethodAttribValue, 57 );


/* "xmlns" */
const DRM_CHAR g_rgchSigKeyInfoAttribName [] = "xmlns";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigKeyInfoAttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchSigKeyInfoAttribName, 5 );

/* "http://www.w3.org/2000/09/xmldsig#" */
const DRM_CHAR g_rgchSigKeyInfoAttribValue [] = "http://www.w3.org/2000/09/xmldsig#";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigKeyInfoAttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchSigKeyInfoAttribValue, 34 );




/*
** ------------------------------------------------------------------
** XML strings used in the construction of XML encrypted data segment
** ------------------------------------------------------------------
*/
/* "EncryptedData" */
const DRM_CHAR g_rgchSigEncryptedDataTag [] = "EncryptedData";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigEncryptedDataTag = CREATE_DRM_ANSI_STRING_EX( g_rgchSigEncryptedDataTag, 13 );


/* "EncryptionMethod" */
const DRM_CHAR g_rgchSigEncryptionMethodTag [] = "EncryptionMethod";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigEncryptionMethodTag = CREATE_DRM_ANSI_STRING_EX( g_rgchSigEncryptionMethodTag, 16 );


/* "EncryptedKey" */
const DRM_CHAR g_rgchSigEncryptedKeyTag [] = "EncryptedKey";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigEncryptedKeyTag = CREATE_DRM_ANSI_STRING_EX( g_rgchSigEncryptedKeyTag, 12 );


/* "CipherData" */
const DRM_CHAR g_rgchSigCipherDataTag [] = "CipherData";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigCipherDataTag = CREATE_DRM_ANSI_STRING_EX( g_rgchSigCipherDataTag, 10 );


/* "CipherValue" */
const DRM_CHAR g_rgchSigCipherValueTag [] = "CipherValue";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigCipherValueTag = CREATE_DRM_ANSI_STRING_EX( g_rgchSigCipherValueTag, 11 );



/* "xmlns" */
const DRM_CHAR g_rgchSigEncryptedDataAttrib1Name [] = "xmlns";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigEncryptedDataAttrib1Name = CREATE_DRM_ANSI_STRING_EX( g_rgchSigEncryptedDataAttrib1Name, 5 );

/* "http://www.w3.org/2001/04/xmlenc#" */
const DRM_CHAR g_rgchSigEncryptedDataAttrib1Value [] = "http://www.w3.org/2001/04/xmlenc#";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigEncryptedDataAttrib1Value = CREATE_DRM_ANSI_STRING_EX( g_rgchSigEncryptedDataAttrib1Value, 33 );


/* "Type" */
const DRM_CHAR g_rgchSigEncryptedDataAttrib2Name [] = "Type";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigEncryptedDataAttrib2Name = CREATE_DRM_ANSI_STRING_EX( g_rgchSigEncryptedDataAttrib2Name, 4 );

/* "http://www.w3.org/2001/04/xmlenc#Element" */
const DRM_CHAR g_rgchSigEncryptedDataAttrib2Value [] = "http://www.w3.org/2001/04/xmlenc#Element";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigEncryptedDataAttrib2Value = CREATE_DRM_ANSI_STRING_EX( g_rgchSigEncryptedDataAttrib2Value, 40 );


/* "Algorithm" */
const DRM_CHAR g_rgchSigEncryptionMethodAttrib1Name [] = "Algorithm";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigEncryptionMethodAttrib1Name = CREATE_DRM_ANSI_STRING_EX( g_rgchSigEncryptionMethodAttrib1Name, 9 );

/* "http://www.w3.org/2001/04/xmlenc#aes128-cbc" */
const DRM_CHAR g_rgchSigEncryptionMethodAttrib1Value [] = "http://www.w3.org/2001/04/xmlenc#aes128-cbc";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigEncryptionMethodAttrib1Value = CREATE_DRM_ANSI_STRING_EX( g_rgchSigEncryptionMethodAttrib1Value, 43 );


/* "Algorithm" */
const DRM_CHAR g_rgchSigEncryptionMethodAttrib2Name [] = "Algorithm";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigEncryptionMethodAttrib2Name = CREATE_DRM_ANSI_STRING_EX( g_rgchSigEncryptionMethodAttrib2Name, 9 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols#ecc256" */
const DRM_CHAR g_rgchSigEncryptionMethodAttrib2Value [] = "http://schemas.microsoft.com/DRM/2007/03/protocols#ecc256";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigEncryptionMethodAttrib2Value = CREATE_DRM_ANSI_STRING_EX( g_rgchSigEncryptionMethodAttrib2Value, 57 );


/* "xmlns" */
const DRM_CHAR g_rgchSigKeyInfoAttrib1Name [] = "xmlns";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigKeyInfoAttrib1Name = CREATE_DRM_ANSI_STRING_EX( g_rgchSigKeyInfoAttrib1Name, 5 );

/* "http://www.w3.org/2000/09/xmldsig#" */
const DRM_CHAR g_rgchSigKeyInfoAttrib1Value [] = "http://www.w3.org/2000/09/xmldsig#";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigKeyInfoAttrib1Value = CREATE_DRM_ANSI_STRING_EX( g_rgchSigKeyInfoAttrib1Value, 34 );


/* "KeyName" */
const DRM_CHAR g_rgchSigKeyNameTag [] = "KeyName";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigKeyNameTag = CREATE_DRM_ANSI_STRING_EX( g_rgchSigKeyNameTag, 7 );


/* "WMRMServer" */
const DRM_CHAR g_rgchSigKeyInfoKeyName1 [] = "WMRMServer";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigKeyInfoKeyName1 = CREATE_DRM_ANSI_STRING_EX( g_rgchSigKeyInfoKeyName1, 10 );

/* "Client" */
const DRM_CHAR g_rgchSigKeyInfoKeyName2 [] = "Client";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigKeyInfoKeyName2 = CREATE_DRM_ANSI_STRING_EX( g_rgchSigKeyInfoKeyName2, 6 );

/* "Metering" */
const DRM_CHAR g_rgchSigKeyInfoKeyName3 [] = "Metering";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigKeyInfoKeyName3 = CREATE_DRM_ANSI_STRING_EX( g_rgchSigKeyInfoKeyName3, 8 );

/* "KeyInfo/KeyName" */
const DRM_CHAR g_rgchKeyInfoKeyNamePath [] = "KeyInfo/KeyName";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrKeyInfoKeyNamePath = CREATE_DRM_ANSI_STRING_EX( g_rgchKeyInfoKeyNamePath, 15 );


/* "URI" */
const DRM_CHAR g_rgchSigReferenceAttribName [] = "URI";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigReferenceAttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchSigReferenceAttribName, 3 );

/* "#SignedData" */
const DRM_CHAR g_rgchSigReferenceAttribValue [] = "#SignedData";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigReferenceAttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchSigReferenceAttribValue, 11 );


/* "xmlns" */
const DRM_CHAR g_rgchSigEncryptedKeyAttribName [] = "xmlns";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigEncryptedKeyAttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchSigEncryptedKeyAttribName, 5 );

/* "http://www.w3.org/2001/04/xmlenc#" */
const DRM_CHAR g_rgchSigEncryptedKeyAttribValue [] = "http://www.w3.org/2001/04/xmlenc#";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigEncryptedKeyAttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchSigEncryptedKeyAttribValue, 33 );




/*
** --------------------------------------------------------
** XML strings used in the parsing of signature XML segment
** --------------------------------------------------------
*/
/* "SignedInfo" */
const DRM_CHAR g_rgchSigSignedInfoPath [] = "SignedInfo";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigSignedInfoPath = CREATE_DRM_ANSI_STRING_EX( g_rgchSigSignedInfoPath, 10 );


/* "SignatureValue" */
const DRM_CHAR g_rgchSigSignatureValuePath [] = "SignatureValue";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigSignatureValuePath = CREATE_DRM_ANSI_STRING_EX( g_rgchSigSignatureValuePath, 14 );


/* "SignedInfo/Reference/DigestValue" */
const DRM_CHAR g_rgchSigHashPath [] = "SignedInfo/Reference/DigestValue";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigHashPath = CREATE_DRM_ANSI_STRING_EX( g_rgchSigHashPath, 32 );


/* "KeyValue/ECCKeyValue/PublicKey" */
const DRM_CHAR g_rgchSigPublicKeyPath [] = "KeyValue/ECCKeyValue/PublicKey";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigPublicKeyPath = CREATE_DRM_ANSI_STRING_EX( g_rgchSigPublicKeyPath, 30 );


/* "CopyLicenseResult/Response/Signature" */
const DRM_CHAR g_rgchMovCopySignaturePath [] = "CopyLicenseResult/Response/Signature";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovCopySignaturePath = CREATE_DRM_ANSI_STRING_EX( g_rgchMovCopySignaturePath, 36 );


/* "CopyLicenseResult/Response/CopyResponse" */
const DRM_CHAR g_rgchMovCopyResponsePath [] = "CopyLicenseResult/Response/CopyResponse";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovCopyResponsePath = { g_rgchMovCopyResponsePath, 39 };


/* "MoveRefreshResult/Response/Signature" */
const DRM_CHAR g_rgchMovRefreshSignaturePath [] = "MoveRefreshResult/Response/Signature";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovRefreshSignaturePath = CREATE_DRM_ANSI_STRING_EX( g_rgchMovRefreshSignaturePath, 36 );


/* "MoveRefreshResult/Response/RefreshResponse" */
const DRM_CHAR g_rgchMovRefreshResponsePath [] = "MoveRefreshResult/Response/RefreshResponse";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovRefreshResponsePath = CREATE_DRM_ANSI_STRING_EX( g_rgchMovRefreshResponsePath, 42 );


/* "MoveLicenseResult/Response/Signature" */
const DRM_CHAR g_rgchMovMoveSignaturePath [] = "MoveLicenseResult/Response/Signature";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovMoveSignaturePath = CREATE_DRM_ANSI_STRING_EX( g_rgchMovMoveSignaturePath, 36 );


/* "MoveLicenseResult/Response/MoveResponse" */
const DRM_CHAR g_rgchMovMoveResponsePath [] = "MoveLicenseResult/Response/MoveResponse";

const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovMoveResponsePath    = CREATE_DRM_ANSI_STRING_EX( g_rgchMovMoveResponsePath, 39 );

/* "MeterCert" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeteringMeterCertTag [] = "MeterCert";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeteringMeterCertTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeteringMeterCertTag, 9 );

/* "ProcessMeteringDataResult/Response/Signature" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataSignaturePath [] = "ProcessMeteringDataResult/Response/Signature";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataSignaturePath = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataSignaturePath, 44 );

/* "ProcessMeteringDataResult/Response/MeteringResponse" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataMeteringResponsePath [] = "ProcessMeteringDataResult/Response/MeteringResponse";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataMeteringResponsePath = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataMeteringResponsePath, 51 );

/* "ProcessMeteringDataResult/Response/MeteringResponse/CustomData" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataCustomDataPath [] = "ProcessMeteringDataResult/Response/MeteringResponse/CustomData";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataCustomDataPath = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataCustomDataPath, 62 );

/* "GetMeteringCertificateResult/Response/Signature" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterCertSignaturePath [] = "GetMeteringCertificateResult/Response/Signature";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterCertSignaturePath= CREATE_DRM_ANSI_STRING_EX( g_rgchMeterCertSignaturePath, 47 );

/* "GetMeteringCertificateResult/Response/MeteringCertificateResponse" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterCertMeteringCertificateResponsePath [] = "GetMeteringCertificateResult/Response/MeteringCertificateResponse";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterCertMeteringCertificateResponsePath= CREATE_DRM_ANSI_STRING_EX( g_rgchMeterCertMeteringCertificateResponsePath, 65 );

/* "GetMeteringCertificateResult/Response/MeteringCertificateResponse/CustomData" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterCertCustomDataPath [] = "GetMeteringCertificateResult/Response/MeteringCertificateResponse/CustomData";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterCertCustomDataPath = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterCertCustomDataPath, 76 );

/* "JoinDomainResult/Response/Domain/CustomData" */
const DRM_CHAR g_rgchDomJoinCustomDataPath [] = "JoinDomainResult/Response/Domain/CustomData";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinCustomDataPath = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinCustomDataPath, 43 );

/* "JoinDomainResult/Response/Domain" */
const DRM_CHAR g_rgchDomJoinDomainPath [] = "JoinDomainResult/Response/Domain";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinDomainPath = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinDomainPath, 32 );

/* "JoinDomainResult/Response/Signature" */
const DRM_CHAR g_rgchDomJoinSignaturePath [] = "JoinDomainResult/Response/Signature";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinSignaturePath = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinSignaturePath, 35 );

/* "SigningCertificateChain" */
const DRM_CHAR g_rgchDomJoinSigningCertTag [] = "SigningCertificateChain";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinSigningCertTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinSigningCertTag, 23 );

/* "LeaveDomainResult/Response/Domain/CustomData" */
const DRM_CHAR g_rgchDomLeaveCustomDataPath [] = "LeaveDomainResult/Response/Domain/CustomData";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveCustomDataPath = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveCustomDataPath, 44 );

/* "LeaveDomainResult/Response/Domain" */
const DRM_CHAR g_rgchDomLeaveDomainPath [] = "LeaveDomainResult/Response/Domain";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveDomainPath = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveDomainPath, 33 );

/* "LeaveDomainResult/Response/Signature" */
const DRM_CHAR g_rgchDomLeaveSignaturePath [] = "LeaveDomainResult/Response/Signature";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveSignaturePath = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveSignaturePath, 36 );

/* "AcquireLicenseResult/Response/LicenseResponse/CustomData" */
const DRM_CHAR g_rgchLicAcqCustomDataPath [] = "AcquireLicenseResult/Response/LicenseResponse/CustomData";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqCustomDataPath = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqCustomDataPath, 56 );

/* "AcquireLicenseResult/Response/LicenseResponse" */
const DRM_CHAR g_rgchLicLicenseResponsePath [] = "AcquireLicenseResult/Response/LicenseResponse";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicLicenseResponsePath = CREATE_DRM_ANSI_STRING_EX( g_rgchLicLicenseResponsePath, 45 );

/* "AcquireLicenseResult/Response/Signature" */
const DRM_CHAR g_rgchLicSignaturePath [] = "AcquireLicenseResult/Response/Signature";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicSignaturePath = CREATE_DRM_ANSI_STRING_EX( g_rgchLicSignaturePath, 39 );

/* "SigningCertificateChain" */
const DRM_CHAR g_rgchLicSigningCertTag [] = "SigningCertificateChain";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqSigningCertTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicSigningCertTag, 23 );

/* "AcknowledgeLicenseResult/Response/LAck/CustomData" */
const DRM_CHAR g_rgchLicAckCustomDataPath [] = "AcknowledgeLicenseResult/Response/LAck/CustomData";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAckCustomDataPath = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAckCustomDataPath, 49 );

/* "AcknowledgeLicenseResult/Response/LAck" */
const DRM_CHAR g_rgchLicAcqLAckPath [] = "AcknowledgeLicenseResult/Response/LAck";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqLAckPath = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqLAckPath, 38 );

/* "AcknowledgeLicenseResult/Response/Signature" */
const DRM_CHAR g_rgchLicAckSignaturePath [] = "AcknowledgeLicenseResult/Response/Signature";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAckSignaturePath = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAckSignaturePath, 43 );

/*
** -------------------------------------------------------------
** XML strings used in the parsing of XML encrypted data segment
** -------------------------------------------------------------
*/
/* "KeyInfo" */
const DRM_CHAR g_rgchSigKeyInfoPath [] = "KeyInfo";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigKeyInfoPath = CREATE_DRM_ANSI_STRING_EX( g_rgchSigKeyInfoPath, 7 );


/* "KeyInfo/EncryptedKey" */
const DRM_CHAR g_rgchSigEncryptedKeyPath [] = "KeyInfo/EncryptedKey";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigEncryptedKeyPath = CREATE_DRM_ANSI_STRING_EX( g_rgchSigEncryptedKeyPath, 20 );


/* "CipherData/CipherValue" */
const DRM_CHAR g_rgchSigCipherValuePath [] = "CipherData/CipherValue";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSigCipherValuePath = CREATE_DRM_ANSI_STRING_EX( g_rgchSigCipherValuePath, 22 );

/* "http://www.microsoft.com/DRM/2004/11/mslp" */
const DRM_CHAR  g_rgchRSAURIMSLP [] = "http://www.microsoft.com/DRM/2004/11/mslp";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrRSAURIMSLP = CREATE_DRM_ANSI_STRING_EX( g_rgchRSAURIMSLP, 41 );

/* "KeyPair" */
const DRM_CHAR  g_rgchTagRSAKeyPair [] = "KeyPair";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrTagRSAKeyPair = CREATE_DRM_ANSI_STRING_EX( g_rgchTagRSAKeyPair, 7 ) ;

EXIT_PK_NAMESPACE_CODE;
