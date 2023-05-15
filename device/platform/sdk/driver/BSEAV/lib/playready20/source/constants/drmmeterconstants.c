/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmtypes.h>
#include <drmmeterconstants.h>

ENTER_PK_NAMESPACE_CODE;

/*
** ------------------------------------------------------------
** XML strings used in the construction of meter data challenge
** ------------------------------------------------------------
*/
/* "ProcessMeteringData" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataRootTag [] = "ProcessMeteringData";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataRootTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataRootTag, 19 );


/* "challenge" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataChallenge1Tag [] = "challenge";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataChallenge1Tag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataChallenge1Tag, 9 );


/* "Challenge" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataChallenge2Tag [] = "Challenge";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataChallenge2Tag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataChallenge2Tag, 9 );


/* "MeteringData" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataMeteringDataTag [] = "MeteringData";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataMeteringDataTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataMeteringDataTag, 12 );


/* "Version" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataVersionTag [] = "Version";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataVersionTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataVersionTag, 7 );


/* "MID" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataMIDTag [] = "MID";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataMIDTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataMIDTag, 3 );


/* "TID" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataTIDTag [] = "TID";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataTIDTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataTIDTag, 3 );


/* "CustomData" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataCustomDataTag [] = "CustomData";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataCustomDataTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataCustomDataTag, 10 );


/* "PARTIALDATA" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataPartialDataTag [] = "PARTIALDATA";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataPartialDataTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataPartialDataTag, 11 );


/* "Data" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataDataTag [] = "Data";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataDataTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataDataTag, 4 );


/* "CertificateChain" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataDeviceCertTag [] = "CertificateChain";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataDeviceCertTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataDeviceCertTag, 16 );


/* "Records" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataRecordsTag [] = "Records";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataRecordsTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataRecordsTag, 7 );


/* "Record" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataRecordTag [] = "Record";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataRecordTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataRecordTag, 6 );


/* "Action" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataActionTag [] = "Action";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataActionTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataActionTag, 6 );



/* "xmlns" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataRootAttribName [] = "xmlns";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataRootAttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataRootAttribName, 5 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataRootAttribValue [] = "http://schemas.microsoft.com/DRM/2007/03/protocols";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataRootAttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataRootAttribValue, 50 );


/* "xmlns" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataChallenge2AttribName [] = "xmlns";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataChallenge2AttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataChallenge2AttribName, 5 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols/messages" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataChallenge2AttribValue [] = "http://schemas.microsoft.com/DRM/2007/03/protocols/messages";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataChallenge2AttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataChallenge2AttribValue, 59 );


/* "xmlns" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataMeteringDataAttrib1Name [] = "xmlns";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataMeteringDataAttrib1Name = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataMeteringDataAttrib1Name, 5 );


/* "http://schemas.microsoft.com/DRM/2007/03/protocols" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataMeteringDataAttrib1Value [] = "http://schemas.microsoft.com/DRM/2007/03/protocols";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataMeteringDataAttrib1Value = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataMeteringDataAttrib1Value, 50 );


/* "Id" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataMeteringDataAttrib2Name [] = "Id";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataMeteringDataAttrib2Name = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataMeteringDataAttrib2Name, 2 );

/* "SignedData" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataMeteringDataAttrib2Value [] = "SignedData";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataMeteringDataAttrib2Value = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataMeteringDataAttrib2Value, 10 );


/* "KID" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataRecordAttribName [] = "KID";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataRecordAttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataRecordAttribName, 3 );


/* "Name" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataActionAttrib1Name [] = "Name";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataActionAttrib1Name = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataActionAttrib1Name, 4 );


/* "Value" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataActionAttrib2Name [] = "Value";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataActionAttrib2Name = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataActionAttrib2Name, 5 );



/* "2" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataVersionValue [] = "2";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataVersionValue = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataVersionValue, 1 );


/* "1" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataPartialDataValue [] = "1";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataPartialDataValue = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataPartialDataValue, 1 );


/*
** ------------------------------------------------------
** XML strings used in the parsing of meter data response
** ------------------------------------------------------
*/
/* "ProcessMeteringDataResult/Response/MeteringResponse/MeterCert" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataMeterCertPath [] = "ProcessMeteringDataResult/Response/MeteringResponse/MeterCert";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataMeterCertPath= CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataMeterCertPath, 61 );


/* "EncryptedData" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataEncryptedDataPath [] = "EncryptedData";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataEncryptedDataPath = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataEncryptedDataPath, 13 );


/* "MID" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataMIDPath [] = "MID";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataMIDPath = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataMIDPath, 3 );


/* "TID" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataTIDPath [] = "TID";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataTIDPath = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataTIDPath, 3 );


/* "Records" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataRecordsPath [] = "Records";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataRecordsPath = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataRecordsPath, 7 );


/* "KID" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataKIDPath [] = "KID";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataKIDPath = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataKIDPath, 3 );

/* "KeyInfo/KeyName" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataKeyNamePath [] = "KeyInfo/KeyName";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataKeyNamePath = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataKeyNamePath, 15 );

/* "TIDKey" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterDataKeyNameValue [] = "TIDKey";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterDataKeyNameValue = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterDataKeyNameValue, 6 );


/*
** ------------------------------------------------------------
** XML strings used in the construction of meter cert challenge
** ------------------------------------------------------------
*/
/* "GetMeteringCertificate" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterCertRootTag [] = "GetMeteringCertificate";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterCertRootTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterCertRootTag, 22 );

/* "challenge" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeteringChallenge1Tag [] = "challenge";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterCertChallenge1Tag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeteringChallenge1Tag, 9 );

/* "Challenge" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeteringChallenge2Tag [] = "Challenge";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterCertChallenge2Tag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeteringChallenge2Tag, 9 );

/* "Version" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterCertVersionTag [] = "Version";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterCertVersionTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterCertVersionTag, 7 );

/* "MID" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeteringMIDTag [] = "MID";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterCertMIDTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeteringMIDTag, 3 );

/* "SupportsMeterCertSignature" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchSupportsMeterCertSignatureTag [] = "SupportsMeterCertSignature";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSupportsMeterCertSignatureTag = CREATE_DRM_ANSI_STRING_EX( g_rgchSupportsMeterCertSignatureTag, 26 );

/* "true" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchTrueValue [] = "true";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrTrueValue = CREATE_DRM_ANSI_STRING_EX( g_rgchTrueValue, 4 );

/* "CustomData" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterCertCustomDataTag [] = "CustomData";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterCertCustomDataTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterCertCustomDataTag, 10 );


/* "xmlns" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterCertRootAttribName [] = "xmlns";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterCertRootAttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterCertRootAttribName, 5 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterCertRootAttribValue [] = "http://schemas.microsoft.com/DRM/2007/03/protocols";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterCertRootAttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterCertRootAttribValue, 50 );



/* "1" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterCertVersion [] = "1";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterCertVersion = CREATE_DRM_ANSI_STRING_EX( g_rgchMeterCertVersion, 1 );



/*
** ------------------------------------------------------
** XML strings used in the parsing of meter cert response
** ------------------------------------------------------
*/

/* "GetMeteringCertificateResult/Response/MeteringCertificateResponse/MeterCert" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchMeterCertMeterCertPath [] = "GetMeteringCertificateResult/Response/MeteringCertificateResponse/MeterCert";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMeterCertMeterCertPath= CREATE_DRM_ANSI_STRING_EX( g_rgchMeterCertMeterCertPath, 75 );


/*
** -----------------------------
** Additional metering constants
** -----------------------------
*/
const                DRM_WCHAR      g_rgwchTagMeterCert                          [] =  { ONE_WCHAR('M', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('\0', '\0')};
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagMeterCert                          = CREATE_DRM_STRING( g_rgwchTagMeterCert );

EXIT_PK_NAMESPACE_CODE;

