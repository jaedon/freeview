/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmtypes.h>
#include <drmlicacqv3constants.h>

ENTER_PK_NAMESPACE_CODE;

/*
** ---------------------------------------------------------
** XML strings used in the construction of license challenge
** ---------------------------------------------------------
*/
/* "AcquireLicense" */
const DRM_CHAR g_rgchLicAcqRootTag [] = "AcquireLicense";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqRootTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqRootTag, 14 );


/* "challenge" */
const DRM_CHAR g_rgchLicAcqChallenge1Tag [] = "challenge";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqChallenge1Tag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqChallenge1Tag, 9 );


/* "Challenge" */
const DRM_CHAR g_rgchLicAcqChallenge2Tag [] = "Challenge";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqChallenge2Tag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqChallenge2Tag, 9 );


/* "LA" */
const DRM_CHAR g_rgchLicAcqLATag [] = "LA";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqLATag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqLATag, 2 );


/* "Version" */
const DRM_CHAR g_rgchLicAcqVersionTag [] = "Version";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqVersionTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqVersionTag, 7 );


/* "ContentHeader" */
const DRM_CHAR g_rgchLicAcqContentHeaderTag [] = "ContentHeader";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqContentHeaderTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqContentHeaderTag, 13 );


/* "SecurityVersion" */
const DRM_CHAR g_rgchLicAcqSecurityVersionTag [] = "SecurityVersion";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqSecurityVersionTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqSecurityVersionTag, 15 );


/* "AppSecurity" */
const DRM_CHAR g_rgchLicAcqAppSecurityTag [] = "AppSecurity";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqAppSecurityTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqAppSecurityTag, 11 );


/* "SubjectID1" */
const DRM_CHAR g_rgchLicAcqSubjectID1Tag [] = "SubjectID1";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqSubjectID1Tag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqSubjectID1Tag, 10 );


/* "SubjectID2" */
const DRM_CHAR g_rgchLicAcqSubjectID2Tag [] = "SubjectID2";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqSubjectID2Tag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqSubjectID2Tag, 10 );


/* "DRMKVersion" */
const DRM_CHAR g_rgchLicAcqDRMKVersionTag [] = "DRMKVersion";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqDRMKVersionTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqDRMKVersionTag, 11 );


/* "RevocationLists" */
const DRM_CHAR g_rgchLicAcqRevocationListsTag [] = "RevocationLists";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqRevocationListsTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqRevocationListsTag, 15 );


/* "RevListInfo" */
const DRM_CHAR g_rgchLicAcqRevListInfoTag [] = "RevListInfo";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqRevListInfoTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqRevListInfoTag, 11 );


/* "ListID" */
const DRM_CHAR g_rgchLicAcqRevListIDTag [] = "ListID";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqRevListIDTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqRevListIDTag, 6 );


/* "Version" */
const DRM_CHAR g_rgchLicAcqRevVersionTag [] = "Version";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqRevVersionTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqRevVersionTag, 7 );


/* "Data" */
const DRM_CHAR g_rgchLicAcqDataTag [] = "Data";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqDataTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqDataTag, 4 );


/* "CertificateChains" */
const DRM_CHAR g_rgchLicAcqCertificateChainsTag [] = "CertificateChains";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqCertificateChainsTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqCertificateChainsTag, 17 );


/* "CertificateChain" */
const DRM_CHAR g_rgchLicAcqCertificateChainTag [] = "CertificateChain";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqCertificateChainTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqCertificateChainTag, 16 );


/* "UPLINKS" */
const DRM_CHAR g_rgchLicAcqUplinksTag [] = "UPLINKS";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqUplinksTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqUplinksTag, 7 );


/* "UPLINK" */
const DRM_CHAR g_rgchLicAcqUplinkTag [] = "UPLINK";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqUplinkTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqUplinkTag, 6 );


/* "KID" */
const DRM_CHAR g_rgchLicAcqKIDTag [] = "KID";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqKIDTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqKIDTag, 3 );


/* "Category" */
const DRM_CHAR g_rgchLicAcqCategoryTag [] = "Category";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqCategoryTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqCategoryTag, 8 );


/* "Count" */
const DRM_CHAR g_rgchLicAcqCountTag [] = "Count";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqCountTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqCountTag, 5 );


/* "Date" */
const DRM_CHAR g_rgchLicAcqDateTag [] = "Date";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqDateTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqDateTag, 4 );


/* "Vague" */
const DRM_CHAR g_rgchLicAcqVagueTag [] = "Vague";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqVagueTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqVagueTag, 5 );


/* "CustomData" */
const DRM_CHAR g_rgchLicAcqCustomDataTag [] = "CustomData";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqCustomDataTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqCustomDataTag, 10 );


/* "LicenseNonce" */
const DRM_CHAR g_rgchLicAcqLicenseNonceTag [] = "LicenseNonce";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqLicenseNonceTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqLicenseNonceTag, 12 );



/* "xmlns" */
const DRM_CHAR g_rgchLicAcqRootAttribName [] = "xmlns";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqRootAttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqRootAttribName, 5 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols" */
const DRM_CHAR g_rgchLicAcqRootAttribValue [] = "http://schemas.microsoft.com/DRM/2007/03/protocols";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqRootAttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqRootAttribValue, 50 );


/* "xmlns" */
const DRM_CHAR g_rgchLicAcqChallenge2AttribName [] = "xmlns";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqChallenge2AttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqChallenge2AttribName, 5 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols/messages" */
const DRM_CHAR g_rgchLicAcqChallenge2AttribValue [] = "http://schemas.microsoft.com/DRM/2007/03/protocols/messages";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqChallenge2AttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqChallenge2AttribValue, 59 );


/* "xmlns" */
const DRM_CHAR g_rgchLicAcqChallengeVAAttrib1Name [] = "xmlns";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqChallengeVAAttrib1Name = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqChallengeVAAttrib1Name, 5 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols" */
const DRM_CHAR g_rgchLicAcqChallengeVAAttrib1Value [] = "http://schemas.microsoft.com/DRM/2007/03/protocols";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqChallengeVAAttrib1Value = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqChallengeVAAttrib1Value, 50 );


/* "Id" */
const DRM_CHAR g_rgchLicAcqChallengeVAAttrib2Name [] = "Id";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqChallengeVAAttrib2Name = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqChallengeVAAttrib2Name, 2 );

/* "SignedData" */
const DRM_CHAR g_rgchLicAcqChallengeVAAttrib2Value [] = "SignedData";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqChallengeVAAttrib2Value = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqChallengeVAAttrib2Value, 10 );


/* "1" */
const DRM_CHAR g_rgchLicAcqVersionValue [] = "1";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAcqVersionValue = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAcqVersionValue, 1 );



/*
** ---------------------------------------------------------------
** XML strings used in the construction of license acknowledgement
** ---------------------------------------------------------------
*/
/* "AcknowledgeLicense" */
const DRM_CHAR g_rgchLicAckRootTag [] = "AcknowledgeLicense";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAckRootTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAckRootTag, 18 );


/* "Version" */
const DRM_CHAR g_rgchLicAckVersionTag [] = "Version";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAckVersionTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAckVersionTag, 7 );


/* "challenge" */
const DRM_CHAR g_rgchLicAckChallenge1Tag [] = "challenge";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAckChallenge1Tag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAckChallenge1Tag, 9 );


/* "Challenge" */
const DRM_CHAR g_rgchLicAckChallenge2Tag [] = "Challenge";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAckChallenge2Tag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAckChallenge2Tag, 9 );


/* "Ack" */
const DRM_CHAR g_rgchLicAckAckTag [] = "Ack";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAckAckTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAckAckTag, 3 );


/* "Data" */
const DRM_CHAR g_rgchLicAckDataTag [] = "Data";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAckDataTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAckDataTag, 4 );


/* "TransactionID" */
const DRM_CHAR g_rgchLicAckTransactionIDTag [] = "TransactionID";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAckTransactionIDTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAckTransactionIDTag, 13 );


/* "LicenseStorageResults" */
const DRM_CHAR g_rgchLicAckLicenseStorageResultsTag [] = "LicenseStorageResults";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAckLicenseStorageResultsTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAckLicenseStorageResultsTag, 21 );


/* "License" */
const DRM_CHAR g_rgchLicAckLicenseTag [] = "License";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAckLicenseTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAckLicenseTag, 7 );


/* "KID" */
const DRM_CHAR g_rgchLicAckKIDTag [] = "KID";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAckKIDTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAckKIDTag, 3 );


/* "LID" */
const DRM_CHAR g_rgchLicAckLIDTag [] = "LID";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAckLIDTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAckLIDTag, 3 );


/* "Result" */
const DRM_CHAR g_rgchLicAckResultTag [] = "Result";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAckResultTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAckResultTag, 6 );


/* "CertificateChain" */
const DRM_CHAR g_rgchLicAckCertificateChainTag [] = "CertificateChain";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAckCertificateChainTag = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAckCertificateChainTag, 16 );



/* "xmlns" */
const DRM_CHAR g_rgchLicAckRootAttribName [] = "xmlns";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAckRootAttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAckRootAttribName, 5 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols" */
const DRM_CHAR g_rgchLicAckRootAttribValue [] = "http://schemas.microsoft.com/DRM/2007/03/protocols";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAckRootAttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAckRootAttribValue, 50 );


/* "xmlns" */
const DRM_CHAR g_rgchLicAckChallenge2AttribName [] = "xmlns";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAckChallenge2AttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAckChallenge2AttribName, 5 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols/messages" */
const DRM_CHAR g_rgchLicAckChallenge2AttribValue [] = "http://schemas.microsoft.com/DRM/2007/03/protocols/messages";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAckChallenge2AttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAckChallenge2AttribValue, 59 );


/* "xmlns" */
const DRM_CHAR g_rgchLicAckAckAttrib1Name [] = "xmlns";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAckAckAttrib1Name = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAckAckAttrib1Name, 5 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols" */
const DRM_CHAR g_rgchLicAckAckAttrib1Value [] = "http://schemas.microsoft.com/DRM/2007/03/protocols";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAckAckAttrib1Value = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAckAckAttrib1Value, 50 );


/* "Id" */
const DRM_CHAR g_rgchLicAckAckAttrib2Name [] = "Id";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAckAckAttrib2Name = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAckAckAttrib2Name, 2 );

/* "SignedData" */
const DRM_CHAR g_rgchLicAckAckAttrib2Value [] = "SignedData";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAckAckAttrib2Value = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAckAckAttrib2Value, 10 );



/* "1" */
const DRM_CHAR g_rgchLicAckVersionValue [] = "1";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicAckVersionValue = CREATE_DRM_ANSI_STRING_EX( g_rgchLicAckVersionValue, 1 );



/*
** -------------------------------------------------------------
** XML strings used in the parsing of license challenge response
** -------------------------------------------------------------
*/
/* "AcquireLicenseResult/Response/LicenseResponse/Licenses" */
const DRM_CHAR g_rgchLicLicensesPath [] = "AcquireLicenseResult/Response/LicenseResponse/Licenses";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicLicensesPath = CREATE_DRM_ANSI_STRING_EX( g_rgchLicLicensesPath, 54 );


/* "AcquireLicenseResult/Response/LicenseResponse/LicenseNonce" */
const DRM_CHAR g_rgchLicNoncePath [] = "AcquireLicenseResult/Response/LicenseResponse/LicenseNonce";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicNoncePath = CREATE_DRM_ANSI_STRING_EX( g_rgchLicNoncePath, 58 );


/* "AcquireLicenseResult/Response/LicenseResponse/ResponseID" */
const DRM_CHAR g_rgchLicResponseIdPath [] = "AcquireLicenseResult/Response/LicenseResponse/ResponseID";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicResponseIdPath = CREATE_DRM_ANSI_STRING_EX( g_rgchLicResponseIdPath, 56 );


/* "AcquireLicenseResult/Response/LicenseResponse/SigningCertificateChain" */
const DRM_CHAR g_rgchLicSigningCertPath [] = "AcquireLicenseResult/Response/LicenseResponse/SigningCertificateChain";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicSigningCertPath = CREATE_DRM_ANSI_STRING_EX( g_rgchLicSigningCertPath, 69 );

/* "License" */
const DRM_CHAR g_rgchLicLicensePath [] = "License";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicLicensePath = CREATE_DRM_ANSI_STRING_EX( g_rgchLicLicensePath, 7 );

/* "AcquireLicenseResult" */
const DRM_CHAR g_rgchAcquireLicenseResultTag [] = "AcquireLicenseResult";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrAcquireLicenseResultTag = CREATE_DRM_ANSI_STRING_EX( g_rgchAcquireLicenseResultTag, 20 );

/* "AcquireLicenseResult/Response/LicenseResponse/RevInfo" */
const DRM_CHAR g_rgchLicRevInfoPath [] = "AcquireLicenseResult/Response/LicenseResponse/RevInfo";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicRevInfoPath = CREATE_DRM_ANSI_STRING_EX( g_rgchLicRevInfoPath, 53 );

/* "RevInfo" */
const DRM_CHAR g_rgchRevInfo [] = "RevInfo";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrRevInfo = CREATE_DRM_ANSI_STRING_EX( g_rgchRevInfo, 7 );


/* "Revocation" */
const DRM_CHAR g_rgchLicRevocationPath [] = "Revocation";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicRevocationPath = CREATE_DRM_ANSI_STRING_EX( g_rgchLicRevocationPath, 10 );


/* "ListID" */
const DRM_CHAR g_rgchLicListIdPath [] = "ListID";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicListIdPath = CREATE_DRM_ANSI_STRING_EX( g_rgchLicListIdPath, 6 );


/* "ListData" */
const DRM_CHAR g_rgchLicListDataPath [] = "ListData";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicListDataPath = CREATE_DRM_ANSI_STRING_EX( g_rgchLicListDataPath, 8 );


/* "AcquireLicenseResult/Response/LicenseResponse/Acknowledgement/TransactionID" */
const DRM_CHAR g_rgchLicTransactionIDPath [] = "AcquireLicenseResult/Response/LicenseResponse/Acknowledgement/TransactionID";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicTransactionIDPath = CREATE_DRM_ANSI_STRING_EX( g_rgchLicTransactionIDPath, 75 );


/* "AcquireLicenseResult/Response/LicenseResponse/SLK" */
const DRM_CHAR g_rgchLicSLKPath [] = "AcquireLicenseResult/Response/LicenseResponse/SLK";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrLicSLKPath= CREATE_DRM_ANSI_STRING_EX( g_rgchLicSLKPath, 49 );


/*
** ---------------------------------------------------------------
** XML strings used in the parsing of license acknowledge response
** ---------------------------------------------------------------
*/

/* "AcquireLicenseResponse" */
const DRM_CHAR g_rgchAcquireLicenseResponseTag [] = "AcquireLicenseResponse";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrAcquireLicenseResponseTag = CREATE_DRM_ANSI_STRING_EX( g_rgchAcquireLicenseResponseTag, 22 );


EXIT_PK_NAMESPACE_CODE;
