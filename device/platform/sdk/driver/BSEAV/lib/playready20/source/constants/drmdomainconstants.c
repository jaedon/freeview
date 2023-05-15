/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmtypes.h>
#include <drmdomainconstants.h>

ENTER_PK_NAMESPACE_CODE;

/*
** -------------------------------------------------------------
** XML strings used in the construction of domain join challenge
** -------------------------------------------------------------
*/
/* "JoinDomain" */
const DRM_CHAR g_rgchDomJoinRootTag [] = "JoinDomain";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinRootTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinRootTag, 10 );


/* "Version" */
const DRM_CHAR g_rgchDomJoinVersionTag [] = "Version";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinVersionTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinVersionTag, 7 );


/* "challenge" */
const DRM_CHAR g_rgchDomJoinChallenge1Tag [] = "challenge";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinChallenge1Tag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinChallenge1Tag, 9 );


/* "Challenge" */
const DRM_CHAR g_rgchDomJoinChallenge2Tag [] = "Challenge";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinChallenge2Tag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinChallenge2Tag, 9 );


/* "Domain" */
const DRM_CHAR g_rgchDomJoinDomainTag [] = "Domain";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinDomainTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinDomainTag, 6 );


/* "Data" */
const DRM_CHAR g_rgchDomJoinDataTag [] = "Data";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinDataTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinDataTag, 4 );


/* "ServiceID" */
const DRM_CHAR g_rgchDomJoinServiceIDTag [] = "ServiceID";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinServiceIDTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinServiceIDTag, 9 );


/* "AccountID" */
const DRM_CHAR g_rgchDomJoinAccountIDTag [] = "AccountID";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinAccountIDTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinAccountIDTag, 9 );


/* "Revision" */
const DRM_CHAR g_rgchDomJoinRevisionTag [] = "Revision";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinRevisionTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinRevisionTag, 8 );


/* "FriendlyName" */
const DRM_CHAR g_rgchDomJoinFriendlyNameTag [] = "FriendlyName";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinFriendlyNameTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinFriendlyNameTag, 12 );


/* "CustomData" */
const DRM_CHAR g_rgchDomJoinCustomDataTag [] = "CustomData";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinCustomDataTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinCustomDataTag, 10 );


/* "CertificateChain" */
const DRM_CHAR g_rgchDomJoinCertificateChainTag [] = "CertificateChain";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinCertificateChainTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinCertificateChainTag, 16 );



/* "xmlns" */
const DRM_CHAR g_rgchDomJoinRootAttribName [] = "xmlns";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinRootAttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinRootAttribName, 5 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols" */
const DRM_CHAR g_rgchDomJoinRootAttribValue [] = "http://schemas.microsoft.com/DRM/2007/03/protocols";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinRootAttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinRootAttribValue, 50 );


/* "xmlns" */
const DRM_CHAR g_rgchDomJoinChallenge2AttribName [] = "xmlns";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinChallenge2AttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinChallenge2AttribName, 5 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols/messages" */
const DRM_CHAR g_rgchDomJoinChallenge2AttribValue [] = "http://schemas.microsoft.com/DRM/2007/03/protocols/messages";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinChallenge2AttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinChallenge2AttribValue, 59 );


/* "xmlns" */
const DRM_CHAR g_rgchDomJoinDomainAttrib1Name [] = "xmlns";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinDomainAttrib1Name = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinDomainAttrib1Name, 5 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols" */
const DRM_CHAR g_rgchDomJoinDomainAttrib1Value [] = "http://schemas.microsoft.com/DRM/2007/03/protocols";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinDomainAttrib1Value = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinDomainAttrib1Value, 50 );


/* "Id" */
const DRM_CHAR g_rgchDomJoinDomainAttrib2Name [] = "Id";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinDomainAttrib2Name = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinDomainAttrib2Name, 2 );

/* "SignedData" */
const DRM_CHAR g_rgchDomJoinDomainAttrib2Value [] = "SignedData";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinDomainAttrib2Value = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinDomainAttrib2Value, 10 );


/* "Type" */
const DRM_CHAR g_rgchDomJoinCertificateChainAttribName [] = "Type";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinCertificateChainAttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinCertificateChainAttribName, 4 );

/* "device" */
const DRM_CHAR g_rgchDomJoinCertificateChainAttribValue [] = "device";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinCertificateChainAttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinCertificateChainAttribValue, 6 );



/* "2" */
const DRM_CHAR g_rgchDomainJoinVersion [] = "2";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomainJoinVersion = CREATE_DRM_ANSI_STRING_EX( g_rgchDomainJoinVersion, 1 );




/*
** --------------------------------------------------------------
** XML strings used in the construction of domain leave challenge
** --------------------------------------------------------------
*/
/* "LeaveDomain" */
const DRM_CHAR g_rgchDomLeaveRootTag [] = "LeaveDomain";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveRootTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveRootTag, 11 );


/* "Version" */
const DRM_CHAR g_rgchDomLeaveVersionTag [] = "Version";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveVersionTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveVersionTag, 7 );


/* "challenge" */
const DRM_CHAR g_rgchDomLeaveChallenge1Tag [] = "challenge";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveChallenge1Tag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveChallenge1Tag, 9 );


/* "Challenge" */
const DRM_CHAR g_rgchDomLeaveChallenge2Tag [] = "Challenge";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveChallenge2Tag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveChallenge2Tag, 9 );


/* "Domain" */
const DRM_CHAR g_rgchDomLeaveDomainTag [] = "Domain";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveDomainTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveDomainTag, 6 );


/* "Data" */
const DRM_CHAR g_rgchDomLeaveDataTag [] = "Data";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveDataTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveDataTag, 4 );


/* "AccountID" */
const DRM_CHAR g_rgchDomLeaveAccountIDTag [] = "AccountID";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveAccountIDTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveAccountIDTag, 9 );


/* "CustomData" */
const DRM_CHAR g_rgchDomLeaveCustomDataTag [] = "CustomData";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveCustomDataTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveCustomDataTag, 10 );


/* "CertificateChain" */
const DRM_CHAR g_rgchDomLeaveCertificateChainTag [] = "CertificateChain";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveCertificateChainTag = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveCertificateChainTag, 16 );



/* "xmlns" */
const DRM_CHAR g_rgchDomLeaveRootAttribName [] = "xmlns";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveRootAttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveRootAttribName, 5 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols" */
const DRM_CHAR g_rgchDomLeaveRootAttribValue [] = "http://schemas.microsoft.com/DRM/2007/03/protocols";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveRootAttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveRootAttribValue, 50 );


/* "xmlns" */
const DRM_CHAR g_rgchDomLeaveChallenge2AttribName [] = "xmlns";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveChallenge2AttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveChallenge2AttribName, 5 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols/messages" */
const DRM_CHAR g_rgchDomLeaveChallenge2AttribValue [] = "http://schemas.microsoft.com/DRM/2007/03/protocols/messages";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveChallenge2AttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveChallenge2AttribValue, 59 );


/* "xmlns" */
const DRM_CHAR g_rgchDomLeaveDomainAttrib1Name [] = "xmlns";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveDomainAttrib1Name = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveDomainAttrib1Name, 5 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols" */
const DRM_CHAR g_rgchDomLeaveDomainAttrib1Value [] = "http://schemas.microsoft.com/DRM/2007/03/protocols";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveDomainAttrib1Value = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveDomainAttrib1Value, 50 );


/* "Id" */
const DRM_CHAR g_rgchDomLeaveDomainAttrib2Name [] = "Id";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveDomainAttrib2Name = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveDomainAttrib2Name, 2 );

/* "SignedData" */
const DRM_CHAR g_rgchDomLeaveDomainAttrib2Value [] = "SignedData";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveDomainAttrib2Value = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveDomainAttrib2Value, 10 );


/* "Type" */
const DRM_CHAR g_rgchDomLeaveCertificateChainAttribName [] = "Type";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveCertificateChainAttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveCertificateChainAttribName, 4 );

/* "device" */
const DRM_CHAR g_rgchDomLeaveCertificateChainAttribValue [] = "device";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveCertificateChainAttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveCertificateChainAttribValue, 6 );


/* "1" */
const DRM_CHAR g_rgchDomainLeaveVersion [] = "1";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomainLeaveVersion = CREATE_DRM_ANSI_STRING_EX( g_rgchDomainLeaveVersion, 1 );




/*
** -------------------------------------------------------
** XML strings used in the parsing of domain join response
** -------------------------------------------------------
*/
/* "JoinDomainResponse" */
const DRM_CHAR g_rgchDomJoinResponsePath [] = "JoinDomainResponse";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinResponsePath = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinResponsePath, 18 );


/* "Version" */
const DRM_CHAR g_rgchDomJoinVersionPath [] = "Version";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinVersionPath = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinVersionPath, 7 );


/* "ServiceID" */
const DRM_CHAR g_rgchDomJoinServiceIDPath [] = "ServiceID";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinServiceIDPath = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinServiceIDPath, 9 );


/* "AccountID" */
const DRM_CHAR g_rgchDomJoinAccountIDPath [] = "AccountID";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinAccountIDPath = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinAccountIDPath, 9 );


/* "CertificateChain" */
const DRM_CHAR g_rgchDomJoinCertificateChainPath [] = "CertificateChain";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinCertificateChainPath = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinCertificateChainPath, 16 );


/* "DomainKeys" */
const DRM_CHAR g_rgchDomJoinDomainKeysPath [] = "DomainKeys";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinDomainKeysPath = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinDomainKeysPath, 10 );

/* "JoinDomainResult/Response/Domain/SigningCertificateChain" */
const DRM_CHAR g_rgchDomJoinSigningCertPath [] = "JoinDomainResult/Response/Domain/SigningCertificateChain";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomJoinSigningCertPath = CREATE_DRM_ANSI_STRING_EX( g_rgchDomJoinSigningCertPath, 56 );

/*
** --------------------------------------------------------
** XML strings used in the parsing of domain leave response
** --------------------------------------------------------
*/
/* "LeaveDomainResult/Response/Domain/AccountID" */
const DRM_CHAR g_rgchDomLeaveAccountIDPath [] = "LeaveDomainResult/Response/Domain/AccountID";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrDomLeaveAccountIDPath = CREATE_DRM_ANSI_STRING_EX( g_rgchDomLeaveAccountIDPath, 43 );


EXIT_PK_NAMESPACE_CODE;
