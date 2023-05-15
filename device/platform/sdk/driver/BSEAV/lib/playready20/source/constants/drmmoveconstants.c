/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmtypes.h>

#include <drmmoveconstants.h>

ENTER_PK_NAMESPACE_CODE;

const DRM_CHAR g_rgchTrue [] = "True";

const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrTrue  = { g_rgchTrue, 4 };

/*
** -------------------------------------------------------------
** XML strings used in the construction of move challenge
** -------------------------------------------------------------
*/
/* "MoveLicense" */
const DRM_CHAR g_rgchMovMoveRootTag [] = "MoveLicense";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovMoveRootTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMovMoveRootTag, 11 );

/* "Version" */
const DRM_CHAR g_rgchMovMoveVersionTag [] = "Version";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovMoveVersionTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMovMoveVersionTag, 7 );


/* "challenge" */
const DRM_CHAR g_rgchMovMoveChallenge1Tag [] = "challenge";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovMoveChallenge1Tag = CREATE_DRM_ANSI_STRING_EX( g_rgchMovMoveChallenge1Tag, 9 );


/* "Challenge" */
const DRM_CHAR g_rgchMovMoveChallenge2Tag [] = "Challenge";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovMoveChallenge2Tag = CREATE_DRM_ANSI_STRING_EX( g_rgchMovMoveChallenge2Tag, 9 );


/* "Move" */
const DRM_CHAR g_rgchMovMoveTag [] = "Move";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovMoveTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMovMoveTag, 4 );

/* "Transaction" */
const DRM_CHAR g_rgchMovTransactionTag [] = "Transaction";

const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovTransactionTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMovTransactionTag, 11 );

const DRM_CHAR g_rgchMovTransactionIdTag [] = "TransactionId";

const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovTransactionIdTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMovTransactionIdTag, 13 );

/* "Data" */
const DRM_CHAR g_rgchMovMoveDataTag [] = "Data";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovMoveDataTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMovMoveDataTag, 4 );


/* "LID" */
const DRM_CHAR g_rgchMovMoveLidTag [] = "LID";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovMoveLidTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMovMoveLidTag, 3 );


/* "CertificateChain" */
const DRM_CHAR g_rgchMovMoveCertificateChainTag [] = "CertificateChain";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovMoveCertificateChainTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMovMoveCertificateChainTag, 16 );

/* "Nonce" */
const DRM_CHAR g_rgchMovMoveNonceTag [] = "Nonce";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovMoveNonceTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMovMoveNonceTag, 5 );


/* "xmlns" */
const DRM_CHAR g_rgchMovMoveRootAttribName [] = "xmlns";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovMoveRootAttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchMovMoveRootAttribName, 5 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols" */
const DRM_CHAR g_rgchMovMoveRootAttribValue [] = "http://schemas.microsoft.com/DRM/2007/03/protocols";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovMoveRootAttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchMovMoveRootAttribValue, 50 );


/* "xmlns" */
const DRM_CHAR g_rgchMovMoveChallenge2AttribName [] = "xmlns";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovMoveChallenge2AttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchMovMoveChallenge2AttribName, 5 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols/messages" */
const DRM_CHAR g_rgchMovMoveChallenge2AttribValue [] = "http://schemas.microsoft.com/DRM/2007/03/protocols/messages";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovMoveChallenge2AttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchMovMoveChallenge2AttribValue, 59 );


/* "xmlns" */
const DRM_CHAR g_rgchMovMoveAttrib1Name [] = "xmlns";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovMoveAttrib1Name = CREATE_DRM_ANSI_STRING_EX( g_rgchMovMoveAttrib1Name, 5 );

/* "http://schemas.microsoft.com/DRM/2007/03/protocols" */
const DRM_CHAR g_rgchMovMoveAttrib1Value [] = "http://schemas.microsoft.com/DRM/2007/03/protocols";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovMoveAttrib1Value = CREATE_DRM_ANSI_STRING_EX( g_rgchMovMoveAttrib1Value, 50 );


/* "Id" */
const DRM_CHAR g_rgchMovMoveAttrib2Name [] = "Id";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovMoveAttrib2Name = CREATE_DRM_ANSI_STRING_EX( g_rgchMovMoveAttrib2Name, 2 );

/* "SignedData" */
const DRM_CHAR g_rgchMovMoveAttrib2Value [] = "SignedData";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovMoveAttrib2Value = CREATE_DRM_ANSI_STRING_EX( g_rgchMovMoveAttrib2Value, 10 );


/* "Type" */
const DRM_CHAR g_rgchMovMoveCertificateChainAttribName [] = "Type";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovMoveCertificateChainAttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchMovMoveCertificateChainAttribName, 4 );

/* "device" */
const DRM_CHAR g_rgchMovMoveCertificateChainAttribValue [] = "device";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovMoveCertificateChainAttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchMovMoveCertificateChainAttribValue, 6 );



/* "1" */
const DRM_CHAR g_rgchMoveVersion [] = "1";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovMoveVersion = CREATE_DRM_ANSI_STRING_EX( g_rgchMoveVersion, 1 );


/*
** -------------------------------------------------------
** XML strings used in the parsing of move response
** -------------------------------------------------------
*/
/* "MoveLicenseResult/Response/MoveResponse/Nonce" */
const DRM_CHAR g_rgchMovMoveNoncePath [] = "MoveLicenseResult/Response/MoveResponse/Nonce";

const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovMoveNoncePath    = CREATE_DRM_ANSI_STRING_EX( g_rgchMovMoveNoncePath, 45 );


/* "MoveLicenseResult/Response/MoveResponse/MoveIndex" */
const DRM_CHAR g_rgchMovMovePageIndexPath [] = "MoveLicenseResult/Response/MoveResponse/MoveIndex";

const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovMovePageIndexPath    = { g_rgchMovMovePageIndexPath, 49 };

/* "MoveLicenseResult/Response/MoveResponse/Version" */
const DRM_CHAR g_rgchMovMoveVersionPath [] = "MoveLicenseResult/Response/MoveResponse/Version";

const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovMoveVersionPath  = CREATE_DRM_ANSI_STRING_EX( g_rgchMovMoveVersionPath, 47 );

/* "MoveLicenseResult/Response/MoveResponse/MoveAllowed" */
const DRM_CHAR g_rgchMovMoveAllowedPath [] = "MoveLicenseResult/Response/MoveResponse/MoveAllowed";

const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovMoveAllowedPath  = CREATE_DRM_ANSI_STRING_EX( g_rgchMovMoveAllowedPath, 51 );


/* "CurrentIndexVersion" */
const DRM_CHAR g_rgchMovRefreshCurIndexTag [] = "CurrentIndexVersion";

/* "Page" */
const DRM_CHAR g_rgchMovRefreshPageTag [] = "Page";

/* "MoveRefresh" */
const DRM_CHAR g_rgchMovRefreshRootTag [] = "MoveRefresh";

/* "Refresh" */
const DRM_CHAR g_rgchMovRefreshTag [] = "Refresh";

const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovRefreshCurIndexTag = CREATE_DRM_ANSI_STRING_EX( g_rgchMovRefreshCurIndexTag, 19 );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovRefreshPageTag     = CREATE_DRM_ANSI_STRING_EX( g_rgchMovRefreshPageTag,     4 );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovRefreshRootTag     = CREATE_DRM_ANSI_STRING_EX( g_rgchMovRefreshRootTag,     11 );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovRefreshTag         = CREATE_DRM_ANSI_STRING_EX( g_rgchMovRefreshTag,         7 );


/* "MoveRefreshResult/Response/RefreshResponse/Nonce" */
const DRM_CHAR g_rgchMovRefreshNoncePath [] = "MoveRefreshResult/Response/RefreshResponse/Nonce";

const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovRefreshNoncePath    = CREATE_DRM_ANSI_STRING_EX( g_rgchMovRefreshNoncePath, 48 );

/* "MoveRefreshResult/Response/RefreshResponse/Version" */
const DRM_CHAR g_rgchMovRefreshVersionPath [] = "MoveRefreshResult/Response/RefreshResponse/Version";

const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovRefreshVersionPath  = CREATE_DRM_ANSI_STRING_EX( g_rgchMovRefreshVersionPath, 50 );

/* "MoveRefreshResult/Response/RefreshResponse/MoveIndex" */
const DRM_CHAR g_rgchMovRefreshPageIndexPath [] = "MoveRefreshResult/Response/RefreshResponse/MoveIndex";

const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovRefreshPageIndexPath    = { g_rgchMovRefreshPageIndexPath, 52 };

/* "MovePage" */
const DRM_CHAR g_rgchTagMovePage [] = "MovePage";

const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrTagMovePage    = { g_rgchTagMovePage, 8 };


/* "CopyLicense" */
const DRM_CHAR g_rgchMovCopyRootTag [] = "CopyLicense";

const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovCopyRootTag = { g_rgchMovCopyRootTag, 11 };

/* "Copy" */
const DRM_CHAR g_rgchMovCopyTag [] = "Copy";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovCopyTag = { g_rgchMovCopyTag, 4 };

const DRM_CHAR g_rgchMovCopyLidInfoTag [] = "LIDInfo";

const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovCopyLidInfoTag = {g_rgchMovCopyLidInfoTag, 7};

/* "OriginalCopyCount" */
const DRM_CHAR g_rgchMovOriginalCopyCountTag [] = "OriginalCopyCount";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovOriginalCopyCountTag = { g_rgchMovOriginalCopyCountTag, 17 };

/* "CopyLicenseResult/Response/CopyResponse/Version" */
const DRM_CHAR g_rgchMovCopyVersionPath [] = "CopyLicenseResult/Response/CopyResponse/Version";

const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovCopyVersionPath = { g_rgchMovCopyVersionPath, 47 };

/* "CopyLicenseResult/Response/CopyResponse/Nonce" */
const DRM_CHAR g_rgchMovCopyNoncePath [] = "CopyLicenseResult/Response/CopyResponse/Nonce";

const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovCopyNoncePath = {g_rgchMovCopyNoncePath, 45};

/* "CopyLicenseResult/Response/CopyResponse/TransactionId" */
const DRM_CHAR g_rgchMovCopyTransactionIdPath [] = "CopyLicenseResult/Response/CopyResponse/TransactionId";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovCopyTransactionIdPath = {g_rgchMovCopyTransactionIdPath, 53};

/* "CopyLicenseResult/Response/CopyResponse/CopyAllowed" */
const DRM_CHAR g_rgchMovCopyAllowedPath [] = "CopyLicenseResult/Response/CopyResponse/CopyAllowed";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrMovCopyAllowedPath = {g_rgchMovCopyAllowedPath, 51};

EXIT_PK_NAMESPACE_CODE;
