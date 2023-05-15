/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmtypes.h>
#include <drmactivationconstants.h>

ENTER_PK_NAMESPACE_CODE;

/*
** ---------------------------------------------------------------
** XML strings used in the construction of an activation challenge
** ---------------------------------------------------------------
*/

/* "ActivateResult/payload/primaryPayload" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchActivationResponsePrimaryPayloadPath [] = "ActivateResult/payload/primaryPayload";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrActivationResponsePrimaryPayloadPath =
    CREATE_DRM_ANSI_STRING_EX( g_rgchActivationResponsePrimaryPayloadPath, 37 );

/* "ActivateResult/payload/auxilaryPayloadUrl" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchActivationResponseAuxPayloadUrlPath [] = "ActivateResult/payload/auxilaryPayloadUrl";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrActivationResponseAuxPayloadUrlPath =
    CREATE_DRM_ANSI_STRING_EX( g_rgchActivationResponseAuxPayloadUrlPath, 41 );

/* "ActivateResponse" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchActivationResponseTag [] = "ActivateResponse";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrActivationResponseTag =
    CREATE_DRM_ANSI_STRING_EX( g_rgchActivationResponseTag, 16 );

/* "ActivateResult" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchActivationResultTag [] = "ActivateResult";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrActivationResultTag =
    CREATE_DRM_ANSI_STRING_EX( g_rgchActivationResultTag, 14 );

/* "auxilaryPayloadUrl" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchauxilaryPayloadUrlTag [] = "auxilaryPayloadUrl";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrauxilaryPayloadUrlTag =
    CREATE_DRM_ANSI_STRING_EX( g_rgchauxilaryPayloadUrlTag, 18 );

/* "Activate" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchActivationRequestTag [] = "Activate";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrActivationRequestTag =
    CREATE_DRM_ANSI_STRING_EX( g_rgchActivationRequestTag, 8 );

/* "http://schemas.microsoft.com/PlayReady/ActivationService/v1" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchActivationServiceNamespaceV1AttribValue [] = "http://schemas.microsoft.com/PlayReady/ActivationService/v1";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrActivationServiceNamespaceV1AttribValue =
    CREATE_DRM_ANSI_STRING_EX( g_rgchActivationServiceNamespaceV1AttribValue, 59 );

/* "clientSdkVersion" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchclientSdkVersionTag [] = "clientSdkVersion";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrclientSdkVersionTag =
    CREATE_DRM_ANSI_STRING_EX( g_rgchclientSdkVersionTag, 16 );

/* "clientSdkType" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchclientSdkTypeTag [] = "clientSdkType";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrclientSdkTypeTag =
    CREATE_DRM_ANSI_STRING_EX( g_rgchclientSdkTypeTag, 13 );

/* "platform" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchplatformTag [] = "platform";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrplatformTag =
    CREATE_DRM_ANSI_STRING_EX( g_rgchplatformTag, 8 );

/* "tid" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchtidTag [] = "tid";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrtidTag =
    CREATE_DRM_ANSI_STRING_EX( g_rgchtidTag, 3 );

/* "payload" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchpayloadTag [] = "payload";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrpayloadTag =
    CREATE_DRM_ANSI_STRING_EX( g_rgchpayloadTag, 7 );

/* "primaryPayload" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchprimaryPayloadTag [] = "primaryPayload";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrprimaryPayloadTag =
    CREATE_DRM_ANSI_STRING_EX( g_rgchprimaryPayloadTag, 14 );

/* "auxilaryPayload" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchauxilaryPayloadTag [] = "auxilaryPayload";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrauxilaryPayloadTag =
    CREATE_DRM_ANSI_STRING_EX( g_rgchauxilaryPayloadTag, 15 );

/* "wsdl:service" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchwsdlServiceTag [] = "wsdl:service";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrwsdlServiceTag =
    CREATE_DRM_ANSI_STRING_EX( g_rgchwsdlServiceTag, 12 );

/* "name" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchnameAttribute [] = "name";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrnameAttribute =
    CREATE_DRM_ANSI_STRING_EX( g_rgchnameAttribute, 4 );

/* "WsHandler" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchwsdlServiceNameAttributeValue [] = "WsHandler";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrwsdlServiceNameAttributeValue =
    CREATE_DRM_ANSI_STRING_EX( g_rgchwsdlServiceNameAttributeValue, 9 );

/* "wsdl:port" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchwsdlPortTag [] = "wsdl:port";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrwsdlPortTag =
    CREATE_DRM_ANSI_STRING_EX( g_rgchwsdlPortTag, 9 );

/* "WsHandlerHttpPost" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchwsdlPortNameAttributeValue [] = "WsHandlerHttpPost";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrwsdlPortNameAttributeValue =
    CREATE_DRM_ANSI_STRING_EX( g_rgchwsdlPortNameAttributeValue, 17 );

/* "http:address" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchhttpAddressTag [] = "http:address";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrhttpAddressTag =
    CREATE_DRM_ANSI_STRING_EX( g_rgchhttpAddressTag, 12 );

/* "location" */
const DRM_EXPORT_VAR DRM_CHAR g_rgchlocationAttributeName [] = "location";
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrlocationAttributeName =
    CREATE_DRM_ANSI_STRING_EX( g_rgchlocationAttributeName, 8 );

EXIT_PK_NAMESPACE_CODE;
