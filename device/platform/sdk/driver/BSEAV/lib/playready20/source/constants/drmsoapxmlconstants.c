/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmtypes.h>
#include <drmsoapxmlconstants.h>

ENTER_PK_NAMESPACE_CODE;

/*
** -------------------------------------------------------------------
** XML strings used in the parsing of status code from server response
** -------------------------------------------------------------------
*/
/* "detail/Exception" */
const DRM_CHAR g_rgchSOAPExceptionRootPath [] = "detail/Exception";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSOAPExceptionRootPath = CREATE_DRM_ANSI_STRING_EX( g_rgchSOAPExceptionRootPath, 16 );


/* "detail/Signature" */
const DRM_CHAR g_rgchSOAPExceptionSignaturePath [] = "detail/Signature";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSOAPExceptionSignaturePath = CREATE_DRM_ANSI_STRING_EX( g_rgchSOAPExceptionSignaturePath, 16 );


/* "detail/Exception/StatusCode" */
const DRM_CHAR g_rgchSOAPStatusCodePath [] = "detail/Exception/StatusCode";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSOAPStatusCodePath = CREATE_DRM_ANSI_STRING_EX( g_rgchSOAPStatusCodePath, 27 );

/* "detail/Exception/CustomData" */
const DRM_CHAR g_rgchSOAPExceptionCustomDataPath [] = "detail/Exception/CustomData";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSOAPExceptionCustomDataPath = CREATE_DRM_ANSI_STRING_EX( g_rgchSOAPExceptionCustomDataPath, 27 );

/* "CustomData" */
const DRM_CHAR g_rgchSOAPGARDCustomDataPath [] = "CustomData";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSOAPGARDCustomDataPath = CREATE_DRM_ANSI_STRING_EX( g_rgchSOAPGARDCustomDataPath, 10 );


/* "RedirectUrl" */
const DRM_CHAR g_rgchSOAPGARDRedirectUrlPath [] = "RedirectUrl";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSOAPGARDRedirectUrlPath = CREATE_DRM_ANSI_STRING_EX( g_rgchSOAPGARDRedirectUrlPath, 11 );


/* "ServiceId" */
const DRM_CHAR g_rgchSOAPGARDServiceIdPath [] = "ServiceId";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSOAPGARDServiceIdPath = CREATE_DRM_ANSI_STRING_EX( g_rgchSOAPGARDServiceIdPath, 9 );


/* "AccountId" */
const DRM_CHAR g_rgchSOAPGARDAccountIdPath [] = "AccountId";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSOAPGARDAccountIdPath = CREATE_DRM_ANSI_STRING_EX( g_rgchSOAPGARDAccountIdPath, 9 );



/*
** -----------------------------------------------------
** XML strings used in the construction of SOAP envelope
** -----------------------------------------------------
*/
/* "<?xml version= 1.0  encoding= utf-8 ?>" */
const DRM_CHAR g_rgchXMLRootTag [] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrXMLRootTag = CREATE_DRM_ANSI_STRING_EX( g_rgchXMLRootTag, 38 );


/* "soap:Envelope" */
const DRM_CHAR g_rgchSOAPEnvelopeTag [] = "soap:Envelope";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSOAPEnvelopeTag = CREATE_DRM_ANSI_STRING_EX( g_rgchSOAPEnvelopeTag, 13 );

/* "s:Envelope" */
const DRM_CHAR g_rgchSOAPEnvelopeAbbrevTag [] = "s:Envelope";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSOAPEnvelopeAbbrevTag = CREATE_DRM_ANSI_STRING_EX( g_rgchSOAPEnvelopeAbbrevTag, 10 );

/* "soap:Body" */
const DRM_CHAR g_rgchSOAPBodyTag [] = "soap:Body";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSOAPBodyTag = CREATE_DRM_ANSI_STRING_EX( g_rgchSOAPBodyTag, 9 );

/* "s:Body" */
const DRM_CHAR g_rgchSOAPBodyAbbrevTag [] = "s:Body";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSOAPBodyAbbrevTag = CREATE_DRM_ANSI_STRING_EX( g_rgchSOAPBodyAbbrevTag, 6 );



/* "xmlns:xsi" */
const DRM_CHAR g_rgchSOAPEnvelopeAttrib1Name [] = "xmlns:xsi";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSOAPEnvelopeAttrib1Name = CREATE_DRM_ANSI_STRING_EX( g_rgchSOAPEnvelopeAttrib1Name, 9 );

/* "http://www.w3.org/2001/XMLSchema-instance" */
const DRM_CHAR g_rgchSOAPEnvelopeAttrib1Value [] = "http://www.w3.org/2001/XMLSchema-instance";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSOAPEnvelopeAttrib1Value = CREATE_DRM_ANSI_STRING_EX( g_rgchSOAPEnvelopeAttrib1Value, 41 );


/* "xmlns:xsd" */
const DRM_CHAR g_rgchSOAPEnvelopeAttrib2Name [] = "xmlns:xsd";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSOAPEnvelopeAttrib2Name = CREATE_DRM_ANSI_STRING_EX( g_rgchSOAPEnvelopeAttrib2Name, 9 );

/* "http://www.w3.org/2001/XMLSchema" */
const DRM_CHAR g_rgchSOAPEnvelopeAttrib2Value [] = "http://www.w3.org/2001/XMLSchema";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSOAPEnvelopeAttrib2Value = CREATE_DRM_ANSI_STRING_EX( g_rgchSOAPEnvelopeAttrib2Value, 32 );


/* "xmlns:soap" */
const DRM_CHAR g_rgchSOAPEnvelopeAttrib3Name [] = "xmlns:soap";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSOAPEnvelopeAttrib3Name = CREATE_DRM_ANSI_STRING_EX( g_rgchSOAPEnvelopeAttrib3Name, 10 );

/* "http://schemas.xmlsoap.org/soap/envelope/" */
const DRM_CHAR g_rgchSOAPEnvelopeAttrib3Value [] = "http://schemas.xmlsoap.org/soap/envelope/";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSOAPEnvelopeAttrib3Value = CREATE_DRM_ANSI_STRING_EX( g_rgchSOAPEnvelopeAttrib3Value, 41 );

/* "xml:space" */
const DRM_CHAR g_rgchSOAPPreserveSpaceAttribName [] = "xml:space";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSOAPPreserveSpaceAttribName = CREATE_DRM_ANSI_STRING_EX( g_rgchSOAPPreserveSpaceAttribName, 9 );

/* "preserve" */
const DRM_CHAR g_rgchSOAPPreserveSpaceAttribValue [] = "preserve";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrSOAPPreserveSpaceAttribValue = CREATE_DRM_ANSI_STRING_EX( g_rgchSOAPPreserveSpaceAttribValue, 8 );

/*
** -------------------------------------------------------
** XML strings used in the construction of ClientInfo node
** -------------------------------------------------------
*/
const                DRM_CHAR       g_rgchReqTagClientInfo                       [] = "CLIENTINFO";
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrReqTagClientInfo                     = CREATE_DRM_ANSI_STRING_EX( g_rgchReqTagClientInfo, 10);

const                DRM_CHAR       g_rgchReqTagClientVersion                    [] = "CLIENTVERSION";    /* ODD count */
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrReqTagClientVersion                  = CREATE_DRM_ANSI_STRING_EX( g_rgchReqTagClientVersion, 13);

EXIT_PK_NAMESPACE_CODE;

