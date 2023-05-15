/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmutilities.h>
#include <drmcrt.h>
#include <drmxmlparser.h>
#include <drmdevcert.h>
#include <oemdes.h>
#include <oempkcrypto.h>

#include <oemrsaoaeppss.h>

#include <drmdevcert.h>
#include <drmcert.h>
#include <drmxmlbuilder.h>
#include <drmcertparser.h>
#include <drmbase64.h>

ENTER_PK_NAMESPACE_CODE;

const                DRM_CHAR  g_rgchFeatureSampleProtection   [] = "m:WMDRMSampleProtection";
#define g_rgchFeatureSampleProtection_LEN 23
const                DRM_CHAR  g_rgchFeatureExport             [] = "m:WMDRMExport";
#define g_rgchFeatureExport_LEN 13
const                DRM_CHAR  g_rgchFeatureExportIncl         [] = "m:WMDRMExportIncl";
#define g_rgchFeatureExportIncl_LEN 17
const                DRM_CHAR  g_rgchKeyUsageSignCRL           [] = "c:SignCRL";
#define g_rgchKeyUsageSignCRL_LEN 9
const                DRM_CHAR  g_rgchTagModelNumber            [] = "c:ModelNumber";
#define g_rgchTagModelNumber_LEN 13
const                DRM_CHAR  g_rgchTagHardwareID             [] = "c:HardwareID";
#define g_rgchTagHardwareID_LEN 12

const DRM_ANSI_CONST_STRING  g_dastrFeatureSampleProtection             = CREATE_DRM_ANSI_STRING_EX( g_rgchFeatureSampleProtection, g_rgchFeatureSampleProtection_LEN );
const DRM_ANSI_CONST_STRING  g_dastrFeatureExport                       = CREATE_DRM_ANSI_STRING_EX( g_rgchFeatureExport, g_rgchFeatureExport_LEN );
const DRM_ANSI_CONST_STRING  g_dastrFeatureExportIncl                   = CREATE_DRM_ANSI_STRING_EX( g_rgchFeatureExportIncl, g_rgchFeatureExportIncl_LEN);
const DRM_ANSI_CONST_STRING  g_dastrKeyUsageSignCRL                     = CREATE_DRM_ANSI_STRING_EX( g_rgchKeyUsageSignCRL, g_rgchKeyUsageSignCRL_LEN );
const DRM_ANSI_CONST_STRING  g_dastrTagModelNumber                      = CREATE_DRM_ANSI_STRING_EX( g_rgchTagModelNumber, g_rgchTagModelNumber_LEN );
const DRM_ANSI_CONST_STRING  g_dastrTagHardwareID                       = CREATE_DRM_ANSI_STRING_EX( g_rgchTagHardwareID, g_rgchTagHardwareID_LEN );

/******************************************************************************
** Function :   DRM_WCP_GetCertificateType
** Synopsis :   determine the 'level' of a certificate (CertificateAuthority,
**              BlackBox, etc).
** Arguments :  [f_pszBase]     : base of buffer
**              [f_pdasstrCert] : SUBSTRING pointing to the entire certificate
**              [f_pcerttype]   : variable to receive the type
******************************************************************************/

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WCP_GetCertificateType(
  IN    const DRM_CHAR             *f_pszBase,
  IN    const DRM_SUBSTRING        *f_pdasstrCert,
  OUT   enum  DRM_CERTIFICATE_TYPE *f_pcerttype)
{
    DRM_RESULT    dr = DRM_SUCCESS;
    DRM_SUBSTRING dasstrData     = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrBody     = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrSecurity = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrManufacturerData    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrModel    = EMPTY_DRM_SUBSTRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_DRM_WCP_GetCertificateType );

    ChkArg(f_pszBase       != NULL
        && f_pdasstrCert   != NULL
        && f_pcerttype     != NULL);

    ChkDR(DRM_XML_GetNodeA(f_pszBase,
                           f_pdasstrCert,
                          &g_dastrTagWMDRMCertificate,
                           NULL,
                           NULL,
                           0,
                           NULL,
                          &dasstrBody));

    /* HWID is one of 2 tags only found in machine certs */

    ChkDR(DRM_XML_GetNodeA(f_pszBase,
                          &dasstrBody,
                          &g_dastrTagWMDRMData,
                           NULL,
                           NULL,
                           0,
                           NULL,
                          &dasstrData));

    dr = DRM_XML_GetNodeA(f_pszBase,
                          &dasstrData,
                          &g_dastrTagHardwareID,
                           NULL,
                           NULL,
                           0,
                          &dasstrSecurity,
                           NULL);

    /* the distinction between machine and other certs is the only one that matters here */

    if (dr == DRM_SUCCESS)
    {
        *f_pcerttype = certtypeMachine;
    }
    else
    {
        DRM_SUBSTRING dasstrDataNodes = EMPTY_DRM_SUBSTRING;
        DRM_SUBSTRING dasstrFeature   = EMPTY_DRM_SUBSTRING;
        DRM_SUBSTRING dasstrOne       = EMPTY_DRM_SUBSTRING;

        /* It's one of the leafmost certificate types; narrow it down */
        /* can't use GetAttribute here, it calls this function */

        ChkDR(DRM_XML_GetNodeA (f_pszBase, &dasstrBody,      &g_dastrTagWMDRMData,         NULL, NULL, 0, NULL, &dasstrDataNodes));
        dr =  DRM_XML_GetNodeA (f_pszBase, &dasstrDataNodes, &g_dastrTagWMDRMCertFeatures, NULL, NULL, 0, NULL, &dasstrDataNodes);

        if (DRM_SUCCEEDED(dr))
        {
            ChkDR(DRM_XML_EnumNextNodeA(f_pszBase, &dasstrDataNodes, 0, &dasstrFeature,               NULL, &dasstrOne, NULL,  NULL));

            if (DRM_UTL_DASSTRStringsEqual    (f_pszBase, &dasstrFeature, &g_dastrFeatureExport))
            {
                *f_pcerttype = certtypeExport;
            }
            else if (DRM_UTL_DASSTRStringsEqual(f_pszBase, &dasstrFeature, &g_dastrFeatureExportIncl))
            {
                *f_pcerttype = certtypeExportIncl;
            }
            else if (DRM_UTL_DASSTRStringsEqual(f_pszBase, &dasstrFeature, &g_dastrFeatureSampleProtection))
            {
                *f_pcerttype = certtypeSampleProtection;
            }
            else
            {
                /* If the cert has a ManufacturerData->ModelNumber tag, it must be a non-machine, device cert.
                 * Every cert MUST have a ManufacturerData node
                 */
                dr = DRM_XML_GetNodeA(f_pszBase, &dasstrData, &g_dastrTagManufacturerData, NULL, NULL, 0, NULL, &dasstrManufacturerData );
                ChkDR( dr );

                dr = DRM_XML_GetNodeA(f_pszBase, &dasstrManufacturerData, &g_dastrTagModelNumber, NULL, NULL, 0, &dasstrModel, NULL);
                if( dr == DRM_SUCCESS )
                {
                    *f_pcerttype = certtypeDevice;
                }
                else
                {
                    *f_pcerttype = certtypeCertAuthority;
                    dr = DRM_SUCCESS;
                }
            }
        }
        else
        {
            /* This code is reached when a CRL signing cert is being parsed */
            *f_pcerttype = certtypeCertAuthority;
            dr = DRM_SUCCESS;
        }
    }

    ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

        return dr;
} /* DRM_WCP_GetCertificateType */

/******************************************************************************
** Function :
** Synopsis :
** Arguments :  [f_pszBase]     : base of buffer
**              [f_pdasstrCert] : SUBSTRING pointing to the entire certificate
**
******************************************************************************/

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WCP_GetAttribute(
  IN const DRM_CHAR              *f_pszBase,
  IN const DRM_SUBSTRING         *f_pdasstrCert,
  IN     DRM_WMDRM_CERT_ATTRIBUTE f_attribute,
     OUT DRM_SUBSTRING            f_rgdasstrValue [],
  IN OUT DRM_DWORD               *f_pcEntries)
{
    DRM_RESULT    dr                = DRM_SUCCESS;
    DRM_DWORD     iNode             = 0;
    DRM_DWORD     cNodes            = 0;
    DRM_SUBSTRING dasstrData        = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrDataNodes   = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrUsage       = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstr0           = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstr1           = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstr2           = EMPTY_DRM_SUBSTRING;
    enum DRM_CERTIFICATE_TYPE certtype = certtypeUnknown;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_DRM_WCP_GetAttribute );

    ChkArg(f_pszBase       != NULL
        && f_pdasstrCert   != NULL
        && f_pcEntries     != NULL
        && f_pdasstrCert->m_cch > 0);

    ChkDR(DRM_WCP_GetCertificateType(f_pszBase,
                                     f_pdasstrCert,
                                    &certtype));

    if (certtype == certtypeMachine)
    {
        ChkArg(f_attribute  == DRM_WMDRM_CERT_ATTR_SECURITY_LEVEL
            || f_attribute  == DRM_WMDRM_CERT_ATTR_KEY_USAGE
            || f_attribute  == DRM_WMDRM_CERT_ATTR_FEATURES
            || f_attribute  == DRM_WMDRM_CERT_ATTR_SECURITY_VERSION
            || f_attribute  == DRM_WMDRM_CERT_ATTR_PUBLIC_KEY_SELF_RSA
            || f_attribute  == DRM_WMDRM_CERT_ATTR_PUBLIC_KEY_SIGNER_RSA
            || f_attribute  == DRM_WMDRM_CERT_ATTR_DIGEST_VALUE );
    }
    else if (certtype == certtypeDevice)
    {
        ChkArg(f_attribute  == DRM_WMDRM_CERT_ATTR_SECURITY_LEVEL
            || f_attribute  == DRM_WMDRM_CERT_ATTR_KEY_USAGE
            || f_attribute  == DRM_WMDRM_CERT_ATTR_FEATURES
            || f_attribute  == DRM_WMDRM_CERT_ATTR_PUBLIC_KEY_SELF_RSA
            || f_attribute  == DRM_WMDRM_CERT_ATTR_PUBLIC_KEY_SIGNER_RSA
            || f_attribute  == DRM_WMDRM_CERT_ATTR_DIGEST_VALUE);
    }
    else
    {
        ChkArg(f_attribute  == DRM_WMDRM_CERT_ATTR_SECURITY_LEVEL
            || f_attribute  == DRM_WMDRM_CERT_ATTR_KEY_USAGE
            || f_attribute  == DRM_WMDRM_CERT_ATTR_FEATURES
            || f_attribute  == DRM_WMDRM_CERT_ATTR_SECURITY_VERSION
            || f_attribute  == DRM_WMDRM_CERT_ATTR_PUBLIC_KEY_SELF_RSA
            || f_attribute  == DRM_WMDRM_CERT_ATTR_PUBLIC_KEY_SIGNER_RSA
            || f_attribute  == DRM_WMDRM_CERT_ATTR_DIGEST_VALUE );
    }

    ChkDR(DRM_XML_GetNodeA(f_pszBase, f_pdasstrCert, &g_dastrTagWMDRMCertificate, NULL, NULL, 0, NULL, &dasstrData));

    /* this attribute is retrieved from the <Signature> node, not from <c:Data> */

    if (f_attribute != DRM_WMDRM_CERT_ATTR_PUBLIC_KEY_SIGNER_RSA)
    {
        ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrData,   &g_dastrTagWMDRMData,        NULL, NULL, 0, NULL, &dasstrDataNodes));
    }

    if( f_attribute == DRM_WMDRM_CERT_ATTR_KEY_USAGE )
    {
        /*
        ** <c:KeyUsage>
        **    <c:SignCertificate/>
        **    <c:SignCRL/>
        **    <c:EncryptKey/>
        ** </c:KeyUsage>
        */

        ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrDataNodes, &g_dastrTagWMDRMCertKeyUsage, NULL, NULL, 0, NULL, &dasstrUsage));

        ChkDR(DRM_XML_CountMatchingNodesA(f_pszBase, &dasstrUsage, NULL, NULL, NULL, &cNodes));

        if (f_rgdasstrValue == NULL
        || *f_pcEntries      < cNodes)
        {
            *f_pcEntries = cNodes;
            ChkDR(DRM_E_BUFFERTOOSMALL);
        }

        *f_pcEntries = cNodes;

        for (iNode = 0; iNode < cNodes; iNode++)
        {
            DRM_SUBSTRING dasstrTag = EMPTY_DRM_SUBSTRING;

            ChkDR(DRM_XML_EnumNextNodeA(f_pszBase,
                                       &dasstrUsage,
                                        iNode,
                                       &dasstrTag, /* strings are in the <Tag> */
                                        f_rgdasstrValue + iNode,
                                        NULL,
                                        NULL,
                                        NULL));

        }
    }
    else if( f_attribute == DRM_WMDRM_CERT_ATTR_SECURITY_LEVEL )
    {
        if (f_rgdasstrValue == NULL
        || *f_pcEntries      < 1)
        {
            *f_pcEntries = 1;
            ChkDR(DRM_E_BUFFERTOOSMALL);
        }

       *f_pcEntries = 1;

       ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrDataNodes, &g_dastrTagWMDRMCertSecurityLevel, NULL, NULL, 0, NULL, f_rgdasstrValue));
    }
    else if( f_attribute == DRM_WMDRM_CERT_ATTR_SECURITY_VERSION )
    {
        if (f_rgdasstrValue == NULL
        || *f_pcEntries      < 1)
        {
            *f_pcEntries = 1;
            ChkDR(DRM_E_BUFFERTOOSMALL);
        }

       *f_pcEntries = 1;

        ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrDataNodes, &g_dastrTagWMDRMCertSecurityVersion, NULL, NULL, 0, NULL, f_rgdasstrValue));
    }
    else if( f_attribute == DRM_WMDRM_CERT_ATTR_FEATURES )
    {
        ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrDataNodes, &g_dastrTagWMDRMCertFeatures,        NULL, NULL, 0, NULL, &dasstrDataNodes));

        ChkDR(DRM_XML_CountMatchingNodesA(f_pszBase,
                                         &dasstrDataNodes,
                                          NULL,
                                          NULL,
                                          NULL,
                                         &cNodes));

        if (f_rgdasstrValue == NULL
        || *f_pcEntries      < cNodes)
        {
            *f_pcEntries = cNodes;
            ChkDR(DRM_E_BUFFERTOOSMALL);
        }

        *f_pcEntries = cNodes;

        for (iNode = 0; iNode < cNodes; iNode++)
        {
            DRM_SUBSTRING dasstrTag = EMPTY_DRM_SUBSTRING;
            DRM_SUBSTRING dasstrOne = EMPTY_DRM_SUBSTRING;

            ChkDR(DRM_XML_EnumNextNodeA(f_pszBase,
                                       &dasstrDataNodes,
                                        iNode,
                                       &dasstrTag,
                                        f_rgdasstrValue + iNode,
                                       &dasstrOne,
                                        NULL,
                                        NULL));

            /* only a string "1" is allowed for the data */

            ChkArg(DRM_UTL_DASSTRStringsEqual(f_pszBase, &dasstrOne, &g_dastrOne));
        }
    }
    else if( f_attribute == DRM_WMDRM_CERT_ATTR_PUBLIC_KEY_SELF_RSA )
    {
        if (f_rgdasstrValue == NULL
        || *f_pcEntries      < 2)
        {
            *f_pcEntries = 2;
            ChkDR(DRM_E_BUFFERTOOSMALL);
        }

        *f_pcEntries = 2;

        ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrDataNodes, &g_dastrTagWMDRMCertPublicKey, NULL, NULL, 0, NULL, &dasstr0));
        ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstr0,         &g_dastrTagKeyValue,           NULL, NULL, 0, NULL, &dasstr1));
        ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstr1,         &g_dastrTagRSAKeyValue,        NULL, NULL, 0, NULL, &dasstr2));
        ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstr2,         &g_dastrTagExponent,           NULL, NULL, 0, NULL, f_rgdasstrValue));
        ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstr2,         &g_dastrTagModulus,            NULL, NULL, 0, NULL, f_rgdasstrValue + 1));

    }
    else if( f_attribute == DRM_WMDRM_CERT_ATTR_PUBLIC_KEY_SIGNER_RSA )
    {
        if (f_rgdasstrValue == NULL
        || *f_pcEntries      < 2)
        {
            *f_pcEntries = 2;
            ChkDR(DRM_E_BUFFERTOOSMALL);
        }

        *f_pcEntries = 2;

        ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrData, &g_dastrTagSignature,   NULL, NULL, 0, NULL, &dasstr0));
        ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstr0,    &g_dastrTagKeyInfo,     NULL, NULL, 0, NULL, &dasstr1));
        ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstr1,    &g_dastrTagKeyValue,    NULL, NULL, 0, NULL, &dasstr2));
        ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstr2,    &g_dastrTagRSAKeyValue, NULL, NULL, 0, NULL, &dasstr0));
        ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstr0,    &g_dastrTagExponent,    NULL, NULL, 0, NULL, f_rgdasstrValue));
        ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstr0,    &g_dastrTagModulus,     NULL, NULL, 0, NULL, f_rgdasstrValue + 1));
    }
    else if( f_attribute == DRM_WMDRM_CERT_ATTR_DIGEST_VALUE )
    {
        if (f_rgdasstrValue == NULL
        || *f_pcEntries < 1)
        {
            *f_pcEntries = 1;
            ChkDR( DRM_E_BUFFERTOOSMALL);
        }

        *f_pcEntries = 1;

        ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrData, &g_dastrTagSignature,   NULL, NULL, 0, NULL, &dasstr0));
        ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstr0,    &g_dastrTagSignedInfo,  NULL, NULL, 0, NULL, &dasstr1));
        ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstr1,    &g_dastrTagReference,   NULL, NULL, 0, NULL, &dasstr2));
        ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstr2,    &g_dastrTagDigestValue, NULL, NULL, 0, NULL, f_rgdasstrValue));
    }
    else
    {
        ChkDR(DRM_E_INVALIDARG);
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

        return dr;
} /* DRM_WCP_GetAttribute */

/******************************************************************************
** Function :
** Synopsis :
** Arguments :  [f_pszBase]     : base of buffer
**              [f_pdasstrCert] : SUBSTRING pointing to the entire certificate
**
******************************************************************************/
DRM_NO_INLINE DRM_RESULT DRM_CALL  _ExtractValueFromAttribute(
    IN const DRM_CHAR              *f_pszBase,
    IN const DRM_SUBSTRING         *f_pdasstrAttribValue,
    OUT      DRM_DWORD             *f_pdwValue)
{
    DRM_RESULT dr        = DRM_E_LOGICERR;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC__ExtractValueFromAttribute );

    ChkArg(f_pszBase               != NULL
        && f_pdasstrAttribValue    != NULL
        && f_pdwValue              != NULL);

    ChkDR( DRMCRT_AtoDWORD( f_pszBase + f_pdasstrAttribValue->m_ich, f_pdasstrAttribValue->m_cch, 10, f_pdwValue ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/******************************************************************************
** Function :
** Synopsis :
** Arguments :
**
**
**
**
******************************************************************************/
DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WCP_GetKeyUsage(
  IN const DRM_CHAR      *f_pszBase,
  IN const DRM_SUBSTRING *f_pdasstrCert,
     OUT   DRM_DWORD     *f_pdwUsage)
{
#define WMDRM_CERT_MAX_KEY_USAGE 20
    DRM_RESULT     dr            = DRM_SUCCESS;
    DRM_DWORD      cUsages       = 0;
    DRM_SUBSTRING  rgdasstrUsages[WMDRM_CERT_MAX_KEY_USAGE] = {0};
    DRM_DWORD      i             = 0;
    DRM_DWORD      fUsages       = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_DRM_WCP_GetKeyUsage );

    ChkArg(f_pszBase     != NULL
        && f_pdasstrCert != NULL
        && f_pdwUsage    != NULL);

    cUsages = NO_OF( rgdasstrUsages );
    ChkDR( DRM_WCP_GetAttribute(f_pszBase,
                                f_pdasstrCert,
                                DRM_WMDRM_CERT_ATTR_KEY_USAGE,
                                rgdasstrUsages,
                               &cUsages) );

    for (i = 0; i < cUsages; i++)
    {
        DRM_SUBSTRING dasstrData = EMPTY_DRM_SUBSTRING;
        DRM_DWORD     dwValue    = 0;

        if (DRM_SUCCEEDED(DRM_XML_GetNodeA(f_pszBase,
                                           &rgdasstrUsages[i],
                                          &g_dastrKeyUsageSignCRL,
                                           NULL,
                                           NULL,
                                           0,
                                           NULL,
                                          &dasstrData)))
        {
            ChkDR(_ExtractValueFromAttribute(f_pszBase, &dasstrData, &dwValue));

            if (dwValue > 0)
            {
                fUsages |= DRM_WMDRM_CERT_KEYUSAGE_SIGNCRL;
            }
        }
        else if (DRM_SUCCEEDED(DRM_XML_GetNodeA(f_pszBase,
                                                &rgdasstrUsages[i],
                                               &g_dastrKeyUsageSignCert,
                                                NULL,
                                                NULL,
                                                0,
                                                NULL,
                                               &dasstrData)))
        {
            ChkDR(_ExtractValueFromAttribute(f_pszBase, &dasstrData, &dwValue));

            if (dwValue > 0)
            {
                fUsages |= DRM_WMDRM_CERT_KEYUSAGE_SIGNCERT;
            }
        }
        else if (DRM_SUCCEEDED(DRM_XML_GetNodeA(f_pszBase,
                                              &rgdasstrUsages[i],
                                              &g_dastrKeyUsageEncryptKey,
                                              NULL,
                                              NULL,
                                              0,
                                              NULL,
                                              &dasstrData)))
        {
            ChkDR(_ExtractValueFromAttribute(f_pszBase, &dasstrData, &dwValue));

            if (dwValue > 0)
            {
                fUsages |= DRM_WMDRM_CERT_KEYUSAGE_ENCRYPTKEY;
            }
        }
    }

    *f_pdwUsage = fUsages;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
} /* DRM_WCP_GetAttribute */

/******************************************************************************
** Function :
** Synopsis :
** Arguments :
**
**
**
**
******************************************************************************/

DRM_RESULT DRM_CALL DRM_WCP_GetFeatures(
  IN const DRM_CHAR              *f_pszBase,
  IN const DRM_SUBSTRING         *f_pdasstrCert,
     OUT DRM_DWORD               *f_pfFeatures)
{
#define WMDRM_CERT_MAX_FEATURES 20
    DRM_RESULT     dr              = DRM_SUCCESS;
    DRM_DWORD      cFeatures       = 0;
    DRM_SUBSTRING  rgdasstrFeatures[WMDRM_CERT_MAX_FEATURES] = {0};
    DRM_DWORD      i               = 0;
    DRM_DWORD      fFeatures       = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_DRM_WCP_GetFeatures );

    ChkArg(f_pszBase       != NULL
        && f_pdasstrCert   != NULL
        && f_pfFeatures    != NULL);

    cFeatures = NO_OF( rgdasstrFeatures );
    dr = DRM_WCP_GetAttribute(f_pszBase,
                              f_pdasstrCert,
                              DRM_WMDRM_CERT_ATTR_FEATURES,
                              rgdasstrFeatures,
                             &cFeatures);
    if( DRM_E_BUFFERTOOSMALL == dr )
    {
        dr = DRM_E_INVALID_DEVCERT_ATTRIBUTE;
    }
    ChkDR( dr );

    for (i = 0; i < cFeatures; i++)
    {
        DRM_SUBSTRING dasstrData = EMPTY_DRM_SUBSTRING;
        DRM_DWORD     dwValue    = 0;

        if (DRM_SUCCEEDED(DRM_XML_GetNodeA(f_pszBase,
                                           &rgdasstrFeatures[i],
                                          &g_dastrFeaturesWMDRMTransmitter,
                                           NULL,
                                           NULL,
                                           0,
                                           NULL,
                                          &dasstrData)))
        {
            ChkDR(_ExtractValueFromAttribute(f_pszBase, &dasstrData, &dwValue));

            if (dwValue > 0)
            {
                fFeatures |= DRM_WMDRM_CERT_FEATURES_TRUSTEDTRANSMITTER;
            }
        }
        else if (DRM_SUCCEEDED(DRM_XML_GetNodeA(f_pszBase,
                                                &rgdasstrFeatures[i],
                                               &g_dastrFeaturesWMDRMReceiver,
                                                NULL,
                                                NULL,
                                                0,
                                                NULL,
                                               &dasstrData)))
        {
            ChkDR(_ExtractValueFromAttribute(f_pszBase, &dasstrData, &dwValue));

            if (dwValue > 0)
            {
                fFeatures |= DRM_WMDRM_CERT_FEATURES_TRUSTEDRECEIVER;
            }
        }
        else if (DRM_SUCCEEDED(DRM_XML_GetNodeA(f_pszBase,
                                                &rgdasstrFeatures[i],
                                               &g_dastrFeatureSampleProtection,
                                                NULL,
                                                NULL,
                                                0,
                                                NULL,
                                               &dasstrData)))
        {
            ChkDR(_ExtractValueFromAttribute(f_pszBase, &dasstrData, &dwValue));

            if (dwValue > 0)
            {
                fFeatures |= DRM_WMDRM_CERT_FEATURES_SAMPLEPROTECTION;
            }
        }
    }

    *f_pfFeatures = fFeatures;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
} /* DRM_WCP_GetAttribute */

/******************************************************************************
** Function :
** Synopsis :
** Arguments :
**
**
**
**
******************************************************************************/

DRM_RESULT DRM_CALL DRM_WCP_GetSecurityLevel(
  IN const DRM_CHAR      *f_pszBase,
  IN const DRM_SUBSTRING *f_pdasstrCert,
     OUT   DRM_DWORD     *f_pdwSecurityLevel)
{
    DRM_RESULT    dr                  = DRM_SUCCESS;
    DRM_DWORD     cStrs               = 1;
    DRM_SUBSTRING dasstrSecurityLevel = EMPTY_DRM_SUBSTRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_DRM_WCP_GetSecurityLevel );

    ChkArg(f_pszBase          != NULL
        && f_pdasstrCert      != NULL
        && f_pdwSecurityLevel != NULL);

    ChkDR(DRM_WCP_GetAttribute(f_pszBase,
                               f_pdasstrCert,
                               DRM_WMDRM_CERT_ATTR_SECURITY_LEVEL,
                              &dasstrSecurityLevel,
                              &cStrs));

    ChkDR(_ExtractValueFromAttribute(f_pszBase,
                                    &dasstrSecurityLevel,
                                     f_pdwSecurityLevel));

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
} /* DRM_WCP_GetAttribute */

/******************************************************************************
** Function :
** Synopsis :
** Arguments :
**
**
**
**
******************************************************************************/

DRM_RESULT DRM_CALL DRM_WCP_GetPublicKey(
  IN const DRM_CHAR              *f_pszBase,
  IN const DRM_SUBSTRING         *f_pdasstrCert,
     OUT DRM_BYTE                *f_pbModulus,
     OUT DRM_DWORD               *f_pcbModulus)
{
    DRM_RESULT    dr                    = DRM_SUCCESS;
    DRM_SUBSTRING dasstrKeyParts[2];
    DRM_DWORD     cKeyParts             = NO_OF( dasstrKeyParts );
    DRM_BYTE      rgbModulus [__CB_DECL(400)];
    DRM_DWORD     cbModulus             = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_DRM_WCP_GetPublicKey );

    ChkArg(f_pszBase            != NULL
        && f_pdasstrCert        != NULL
        && f_pcbModulus         != NULL);

    ChkDR(DRM_WCP_GetAttribute(f_pszBase,
                               f_pdasstrCert,
                               DRM_WMDRM_CERT_ATTR_PUBLIC_KEY_SELF_RSA,
                               dasstrKeyParts,
                              &cKeyParts));

    cbModulus = SIZEOF(rgbModulus);
    ChkDR(DRM_WCP_ReadPubKeyModulus( (DRM_CHAR *)f_pszBase,
                                     dasstrKeyParts + 1,
                                     rgbModulus,
                                     &cbModulus ) );

    if( f_pbModulus == NULL ||
        *f_pcbModulus < cbModulus )
    {
        *f_pcbModulus = cbModulus;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    ChkBOOL(cbModulus <= SIZEOF(rgbModulus), DRM_E_BUFFERTOOSMALL);
    MEMCPY( f_pbModulus, rgbModulus, cbModulus );
    *f_pcbModulus = cbModulus;

    ChkArg(DRM_UTL_DASSTRStringsEqual(f_pszBase, dasstrKeyParts, &g_dastrWMDRMCertExponent));

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
} /* DRM_WCP_GetAttribute */

/******************************************************************************
** Function :
** Synopsis :
** Arguments :
**
**
**
**
******************************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_WCP_GetHardwareID(
  IN const DRM_CHAR      *f_pszBase,
  IN const DRM_SUBSTRING *f_pdasstrCert,
     OUT   DRM_BYTE       f_rgbHardwareID [DRM_SHA1_DIGEST_LEN])
{
    DRM_RESULT    dr = DRM_SUCCESS;
    DRM_SUBSTRING dasstrData        = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrDataNodes   = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrHardwareID  = EMPTY_DRM_SUBSTRING;
    DRM_DWORD     cbHardwareID      = DRM_SHA1_DIGEST_LEN;
    enum DRM_CERTIFICATE_TYPE certtype = certtypeUnknown;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_DRM_WCP_GetHardwareID );

    ChkArg(f_pszBase       != NULL
        && f_pdasstrCert   != NULL
        && f_rgbHardwareID != NULL);

    /* only machine certs have a HardwareID */

    ChkDR(DRM_WCP_GetCertificateType(f_pszBase,
                                     f_pdasstrCert,
                                    &certtype));

    ChkArg(certtype == certtypeMachine);

    ChkDR(DRM_XML_GetNodeA(f_pszBase,  f_pdasstrCert,    &g_dastrTagWMDRMCertificate, NULL, NULL, 0, NULL, &dasstrData));
    ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrData,       &g_dastrTagWMDRMData,        NULL, NULL, 0, NULL, &dasstrDataNodes));
    ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrDataNodes,  &g_dastrTagHardwareID,       NULL, NULL, 0, NULL, &dasstrHardwareID));

    ChkDR(DRM_B64_DecodeA (f_pszBase, &dasstrHardwareID, &cbHardwareID, f_rgbHardwareID, 0));

    ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

        return dr;
}


/******************************************************************************
** Function :   DRM_WCP_GetSignatureDigest
** Synopsis :   retrieve the certificate's SHA digest from the signature
** Arguments :  [f_pszBase]     : base of buffer
**              [f_pdasstrCert] : SUBSTRING pointing to the entire certificate
**              [f_rgbDigest]   : buffer to receive the SHA digest
******************************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_WCP_GetSignatureDigest(
  IN const DRM_CHAR                *f_pszBase,
  IN const DRM_SUBSTRING           *f_pdasstrCert,
     OUT   DRM_BYTE                 f_rgbDigest [DRM_SHA1_DIGEST_LEN])
{
    DRM_RESULT    dr = DRM_SUCCESS;
    DRM_SUBSTRING dasstrData         = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrSignature    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrSignedInfo   = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrReference    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrDigestValue  = EMPTY_DRM_SUBSTRING;
    DRM_DWORD     cbDigestValue      = DRM_SHA1_DIGEST_LEN;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_DRM_WCP_GetSignatureDigest );

    ChkArg(f_pszBase     != NULL
        && f_pdasstrCert != NULL
        && f_rgbDigest   != NULL);

    ChkDR(DRM_XML_GetNodeA(f_pszBase,  f_pdasstrCert,    &g_dastrTagWMDRMCertificate, NULL, NULL, 0, NULL, &dasstrData));
    ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrData,       &g_dastrTagSignature,        NULL, NULL, 0, NULL, &dasstrSignature));
    ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrSignature,  &g_dastrTagSignedInfo,       NULL, NULL, 0, NULL, &dasstrSignedInfo));
    ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrSignedInfo, &g_dastrTagReference,        NULL, NULL, 0, NULL, &dasstrReference));
    ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrReference,  &g_dastrTagDigestValue,      NULL, NULL, 0, NULL, &dasstrDigestValue));

    ChkDR(DRM_B64_DecodeA (f_pszBase, &dasstrDigestValue, &cbDigestValue, f_rgbDigest, 0));

    ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

        return dr;
}


/******************************************************************************
** Function :
** Synopsis :
** Arguments :
**
**
**
**
******************************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_WCP_GetManufacturerNodes(
IN const DRM_CHAR      *f_pszBase,
IN const DRM_SUBSTRING *f_pdasstrCertificate,
     OUT DRM_SUBSTRING *f_pdasstrNamespace,
     OUT DRM_SUBSTRING  f_rgdastrTag        [],
     OUT DRM_SUBSTRING  f_rgdasstrAttrLabel [],
     OUT DRM_SUBSTRING  f_rgdasstrAttrValue [],
     OUT DRM_SUBSTRING  f_rgdasstrValue     [],
     OUT DRM_DWORD     *f_pcEntries)
{
    DRM_RESULT    dr                = DRM_SUCCESS;
    DRM_DWORD     cNodes            = 0;
    DRM_DWORD     iNode             = 0;
    DRM_SUBSTRING dasstrData        = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrDataNodes   = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrNode        = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrEntries     = EMPTY_DRM_SUBSTRING;
    enum DRM_CERTIFICATE_TYPE certtype = certtypeUnknown;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_DRM_WCP_GetManufacturerNodes );

    ChkArg(f_pszBase            != NULL
        && f_pdasstrCertificate != NULL
        && f_rgdasstrValue      != NULL
        && f_pcEntries          != NULL);

    /* only machine certs have ManufacturerData */

    ChkDR(DRM_WCP_GetCertificateType(f_pszBase,
                                     f_pdasstrCertificate,
                                    &certtype));

    ChkDR(DRM_XML_GetNodeA(f_pszBase,  f_pdasstrCertificate, &g_dastrTagWMDRMCertificate, NULL, NULL, 0, NULL,        &dasstrData));
    ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrData,           &g_dastrTagWMDRMData,        NULL, NULL, 0, NULL,        &dasstrDataNodes));
    ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrDataNodes,      &g_dastrTagManufacturerData, NULL, NULL, 0, &dasstrNode, &dasstrEntries));

    if (f_pdasstrNamespace != NULL)
    {
        f_pdasstrNamespace->m_ich = 0;
        f_pdasstrNamespace->m_cch = 0;

        /*
        ** return code ignored because absence of a namespace is OK
        */

        DRM_XML_GetNodeAttributeA(f_pszBase, &dasstrNode, &g_dastrPrefixManufacturer, f_pdasstrNamespace);
    }

    /* count the nodes */

    ChkDR(DRM_XML_CountMatchingNodesA(f_pszBase, &dasstrEntries, NULL, NULL, NULL, &cNodes));

    if (*f_pcEntries < cNodes)
    {
        *f_pcEntries = cNodes;
        ChkDR(DRM_E_BUFFERTOOSMALL);
    }

    *f_pcEntries = cNodes;

    for (iNode = 0; iNode < cNodes; iNode++)
    {
        ChkDR(DRM_XML_EnumNextNodeA(f_pszBase,
                                    &dasstrEntries,
                                    iNode,
                                    f_rgdastrTag + iNode,
                                    NULL,
                                  ((f_rgdasstrValue     == NULL) ? NULL : f_rgdasstrValue     + iNode),
                                  ((f_rgdasstrAttrLabel == NULL) ? NULL : f_rgdasstrAttrLabel + iNode),
                                  ((f_rgdasstrAttrValue == NULL) ? NULL : f_rgdasstrAttrValue + iNode)));
    }

    ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

        return dr;
}

static DRM_NO_INLINE DRM_RESULT _VerifyCertificateMethods(
    const DRM_CHAR      *f_pszBase,
    const DRM_SUBSTRING *f_pdasstrCert,
    const DRM_BOOL       f_fWMDRMNET)
{
    DRM_RESULT    dr = DRM_SUCCESS;
    DRM_SUBSTRING dasstrSignature      = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrReference      = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrSignedInfo     = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrElements       = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrTransforms     = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrDummy          = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrTag            = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrNodeData       = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrAttributeName  = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrAttributeValue = EMPTY_DRM_SUBSTRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC__VerifyCertificateMethods );

    /* descend into the <c:Certificate>/<Signature> node */

    ChkDR(DRM_XML_GetNodeA(f_pszBase, f_pdasstrCert,     &g_dastrTagWMDRMCertificate,  NULL, NULL, 0, NULL, &dasstrElements));
    ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrElements,   &g_dastrTagSignature,         NULL, NULL, 0, NULL, &dasstrSignature));
    ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrSignature,  &g_dastrTagSignedInfo,        NULL, NULL, 0, NULL, &dasstrSignedInfo));

    /* verify both the presence and the correct values of signature parameters */

    ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrSignedInfo, &g_dastrTagCanonicalization, &g_dastrAttributeAlgorithm, &g_dastrURIC14N,        0, NULL, &dasstrDummy));

    if (f_fWMDRMNET == DRM_DSIG_TYPE_WMDRMNET)
    {
        ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrSignedInfo, &g_dastrTagSignatureMethod,  &g_dastrAttributeAlgorithm, &g_dastrURIRSASHA1_Old, 0, NULL, &dasstrDummy));
    }
    else
    {
        ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrSignedInfo, &g_dastrTagSignatureMethod,  &g_dastrAttributeAlgorithm, &g_dastrURIRSASHA1,     0, NULL, &dasstrDummy));
    }

    ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrSignedInfo, &g_dastrTagReference,         NULL,                       NULL,                  0, NULL, &dasstrReference));
    ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrReference,  &g_dastrTagDigestMethod,     &g_dastrAttributeAlgorithm, &g_dastrURIDSigSHA1,    0, NULL, &dasstrDummy));
    ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrReference,  &g_dastrTagTransforms,        NULL,                       NULL,                  0, NULL, &dasstrTransforms));

    /*
    ** <Transform Algorithm="http://www.microsoft.com/DRM/CERT/v2/Data"/>
    ** <Transform Algorithm="http://www.w3.org/TR/2001/REC-xml-c14n-20010315"/>
    **
    ** must both be present and in the given order
    */

    ChkDR(DRM_XML_EnumNextNodeA(f_pszBase, &dasstrTransforms, 0, &dasstrTag, NULL, &dasstrNodeData, &dasstrAttributeName, &dasstrAttributeValue));

    ChkArg(DRM_UTL_DASSTRStringsEqual(f_pszBase, &dasstrTag,            &g_dastrTagTransform));
    ChkArg(DRM_UTL_DASSTRStringsEqual(f_pszBase, &dasstrAttributeName,  &g_dastrAttributeAlgorithm));

    if (f_fWMDRMNET == DRM_DSIG_TYPE_WMDRMNET)
    {
        ChkArg(DRM_UTL_DASSTRStringsEqual(f_pszBase, &dasstrAttributeValue, &g_dastrURITransformMSCert_Old));
    }
    else
    {
        ChkArg(DRM_UTL_DASSTRStringsEqual(f_pszBase, &dasstrAttributeValue, &g_dastrURITransformMSCert));
    }
    ChkDR(DRM_XML_EnumNextNodeA(f_pszBase, &dasstrTransforms, 1, &dasstrTag, NULL, &dasstrNodeData, &dasstrAttributeName, &dasstrAttributeValue));

    ChkArg(DRM_UTL_DASSTRStringsEqual(f_pszBase, &dasstrTag,            &g_dastrTagTransform));
    ChkArg(DRM_UTL_DASSTRStringsEqual(f_pszBase, &dasstrAttributeName,  &g_dastrAttributeAlgorithm));
    ChkArg(DRM_UTL_DASSTRStringsEqual(f_pszBase, &dasstrAttributeValue, &g_dastrURITransformC14N));

ErrorExit:
    if (DRM_FAILED(dr))
    {
        dr = DRM_E_VERIFICATION_FAILURE;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


static DRM_NO_INLINE DRM_RESULT _VerifyCertificateSignature(
    IN  const DRM_CHAR            *f_pszBase,
    IN  const DRM_SUBSTRING       *f_pdasstrCert,
    IN  const DRM_BOOL             f_fWMDRMNET,
    IN  const WMDRMNET_CRL_ENTRY  *f_pRevocationEntries,
    IN        DRM_DWORD            f_cRevocationEntries,
        OUT   DRM_SUBSTRING       *f_pdasstrPubkeySigner,
    IN        DRM_CRYPTO_CONTEXT  *f_pCryptoCtx)
{
    DRM_RESULT   dr = DRM_SUCCESS;
    DRM_SUBSTRING dasstrElements       = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrSignatureIncl  = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrDataIncl       = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING rgdasstrPubKey [2]   = { 0 };
    DRM_DWORD     cEntries             = NO_OF(rgdasstrPubKey);

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC__VerifyCertificateSignature );

    ChkArg( f_pszBase != NULL );
    ChkArg( f_pCryptoCtx != NULL );
    /* cache some nodes to parse further below */

    ChkDR(DRM_XML_GetNodeA(f_pszBase,  f_pdasstrCert,   &g_dastrTagWMDRMCertificate, NULL, NULL, 0,  NULL,                 &dasstrElements));
    ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrElements,  &g_dastrTagSignature,        NULL, NULL, 0, &dasstrSignatureIncl,   NULL));
    ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrElements,  &g_dastrTagWMDRMData,        NULL, NULL, 0, &dasstrDataIncl,        NULL));

    /* get the signer's public key */

    ChkDR(DRM_WCP_GetAttribute(f_pszBase,
                               f_pdasstrCert,
                               DRM_WMDRM_CERT_ATTR_PUBLIC_KEY_SIGNER_RSA,
                               rgdasstrPubKey,
                              &cEntries));

    /* verify that the exponent matches the certificate version */

    ChkBOOL(DRM_UTL_DASSTRStringsEqual(f_pszBase, rgdasstrPubKey, &g_dastrWMDRMCertExponent), DRM_E_VERIFICATION_FAILURE);

    /* pass out the modulus if requested */

    if (f_pdasstrPubkeySigner != NULL)
    {
        *f_pdasstrPubkeySigner = rgdasstrPubKey [1];
    }

    ChkDR(DRM_WCP_VerifyDigitalSignature(f_pszBase,
                                         &dasstrDataIncl,
                                        &dasstrSignatureIncl,
                                         f_fWMDRMNET,
                                         f_pRevocationEntries,
                                         f_cRevocationEntries,
                                         f_pCryptoCtx ));

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}



static DRM_NO_INLINE DRM_RESULT _VerifyCertificateRequiredTags(
    const DRM_CHAR      *f_pszBase,
    const DRM_SUBSTRING *f_pdasstrCert)
{
    DRM_RESULT   dr = DRM_SUCCESS;
    DRM_SUBSTRING dasstrNode     = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrData     = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrDataNodes= EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrDummy    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrElements = EMPTY_DRM_SUBSTRING;
    DRM_DWORD     iNode          = 0;
    enum DRM_CERTIFICATE_TYPE certtype = certtypeUnknown;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC__VerifyCertificateRequiredTags );

    ChkDR(DRM_WCP_GetCertificateType(f_pszBase,
                                     f_pdasstrCert,
                                    &certtype));

    ChkDR  (DRM_XML_GetNodeA          (f_pszBase,  f_pdasstrCert, &g_dastrTagWMDRMCertificate,     NULL, NULL, 0, &dasstrNode, &dasstrElements));

    if (DRM_SUCCEEDED(DRM_XML_GetNodeAttributeA (f_pszBase, &dasstrNode,    &g_dastrAttributeVersionWMDRM,  &dasstrData)))
    {
        /* If there is a c:Version attribute validate that it is 2.0 */
        ChkBOOL(DRM_UTL_DASSTRStringsEqual(f_pszBase, &dasstrData, &g_dastrVersionWMDRM), DRM_E_INVALID_DEVCERT_ATTRIBUTE);
    }

    if (DRM_SUCCEEDED(DRM_XML_GetNodeAttributeA (f_pszBase, &dasstrNode,    &g_dastrPrefixMicrosoftCert,    &dasstrData)))
    {
        /* If there is a xmlns:c attribute validate that it is what we expect */
        ChkBOOL(DRM_UTL_DASSTRStringsEqual(f_pszBase, &dasstrData, &g_dastrURITransformMSCertColl), DRM_E_INVALID_DEVCERT_ATTRIBUTE);
    }

    ChkDR(DRM_XML_GetNodeA            (f_pszBase, &dasstrElements,  &g_dastrTagWMDRMData,          NULL, NULL, 0, NULL, &dasstrDataNodes));

    /* cert public key node */

    ChkDR  (DRM_XML_GetNodeA          (f_pszBase, &dasstrDataNodes, &g_dastrTagWMDRMCertPublicKey,   NULL, NULL, 0, NULL, &dasstrDummy));
    ChkDR  (DRM_XML_GetNodeA          (f_pszBase, &dasstrDummy,     &g_dastrTagKeyValue,             NULL, NULL, 0, NULL, &dasstrDummy));
    ChkDR  (DRM_XML_GetNodeA          (f_pszBase, &dasstrDummy,     &g_dastrTagRSAKeyValue,          NULL, NULL, 0, NULL, &dasstrDummy));
    ChkDR  (DRM_XML_GetNodeA          (f_pszBase, &dasstrDummy,     &g_dastrTagExponent,             NULL, NULL, 0, NULL, &dasstrData));
    ChkBOOL(DRM_UTL_DASSTRStringsEqual(f_pszBase, &dasstrData,      &g_dastrWMDRMCertExponent),      DRM_E_BAD_RSA_EXPONENT);
    ChkDR  (DRM_XML_GetNodeA          (f_pszBase, &dasstrDummy,     &g_dastrTagModulus,              NULL, NULL, 0, NULL, &dasstrData));
    ChkBOOL(dasstrData.m_cch > 0, DRM_E_INVALID_DEVCERT_ATTRIBUTE);

    /* <KeyUsage> */

    ChkDR(DRM_XML_GetNodeA            (f_pszBase, &dasstrDataNodes, &g_dastrTagWMDRMCertKeyUsage,  NULL, NULL, 0, NULL, &dasstrDummy));

    /* verify legal values */

    for (iNode = 0; ; iNode++)
    {
        dr = DRM_XML_EnumNextNodeA    (f_pszBase, &dasstrDummy, iNode, &dasstrData, &dasstrDummy, NULL, NULL, NULL);

        if (dr == DRM_E_XMLNOTFOUND)
        {
            ChkDR(DRM_SUCCESS);   /* This sets 'dr' and keeps the PREfast tool happy. */
            break;
        }
        else
        {
            ChkDR(dr);

            if (certtype == certtypeMachine
            ||  certtype == certtypeExport
            ||  certtype == certtypeExportIncl
            ||  certtype == certtypeDevice
            ||  certtype == certtypeSampleProtection)
            {
                if (! DRM_UTL_DASSTRStringsEqual(f_pszBase, &dasstrData, &g_dastrKeyUsageEncryptKey))
                {
                    ChkDR(DRM_E_INVALID_DEVCERT_ATTRIBUTE);
                }
            }
            else
            {
                if (! DRM_UTL_DASSTRStringsEqual(f_pszBase, &dasstrData, &g_dastrKeyUsageSignCert)
                &&  ! DRM_UTL_DASSTRStringsEqual(f_pszBase, &dasstrData, &g_dastrKeyUsageSignCRL))
                {
                    ChkDR(DRM_E_INVALID_DEVCERT_ATTRIBUTE);
                }
            }

        }
    } /* end-for KeyUsage nodes */

    /* <SecurityLevel> */

    dr = DRM_XML_GetNodeA(f_pszBase, &dasstrDataNodes, &g_dastrTagWMDRMCertSecurityLevel, NULL, NULL, 0, NULL, &dasstrDummy);

    if (certtype == certtypeMachine
    ||  certtype == certtypeExport
    ||  certtype == certtypeExportIncl
    ||  certtype == certtypeDevice
    ||  certtype == certtypeSampleProtection)
    {
        ChkDR(dr);
    }
    else
    {
        ChkDR(DRM_SUCCESS);  /* This sets 'dr' and keeps the PREfast tool happy. */
    }

    /* <SecurityVersion> */

    dr = DRM_XML_GetNodeA(f_pszBase, &dasstrDataNodes, &g_dastrTagWMDRMCertSecurityVersion, NULL, NULL, 0, NULL, &dasstrDummy);

    if (certtype == certtypeMachine
    ||  certtype == certtypeExport
    ||  certtype == certtypeExportIncl)
    {
        /* Enforce that all machine certs have this value */
        ChkDR(dr);
    }

    /* if <ManufacturerData> is present then it must follow the scheme */

    dr =  DRM_XML_GetNodeA(f_pszBase, &dasstrDataNodes, &g_dastrTagManufacturerData, NULL, NULL, 0, NULL, &dasstrNode);

    if (dr == DRM_E_XMLNOTFOUND)
    {
        dr = DRM_SUCCESS;
    }
    else
    {
        ChkDR(dr);

        ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrNode, &g_dastrTagManufacturerName,      NULL, NULL, 0, NULL, &dasstrDummy));
    }

ErrorExit:
    if (DRM_FAILED(dr))
    {
        dr = DRM_E_VERIFICATION_FAILURE;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/******************************************************************************
** Function :
** Synopsis :
** Arguments :
**
**
**
**
******************************************************************************/

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WCP_VerifyCertificate(
    IN const DRM_CHAR            *f_pszBase,
    IN const DRM_SUBSTRING       *f_pdasstrCert,
    IN       DRM_DWORD            f_fVerify,
    IN const WMDRMNET_CRL_ENTRY  *f_pRevocationEntries,
    IN       DRM_DWORD            f_cRevocationEntries,
       OUT   DRM_SUBSTRING       *f_pdasstrPublicRSA,
    IN       DRM_CRYPTO_CONTEXT  *f_pCryptoCtx)
{
    DRM_RESULT          dr                  = DRM_SUCCESS;
    DRM_BOOL            fWMDRMNET           = DRM_DSIG_TYPE_WMDRMNET;
    DRM_SUBSTRING       dasstrSignedInfo    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING       dasstrCertType      = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING       dasstrSignature     = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING       dasstrElements      = EMPTY_DRM_SUBSTRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_DRM_WCP_VerifyCertificate );

    ChkArg(f_pdasstrCert           != NULL
        && f_pszBase               != NULL
        && f_pdasstrCert->m_cch    != 0);
    ChkArg( f_pCryptoCtx != NULL );

    if (f_cRevocationEntries > 0)
    {
        ChkArg(f_pRevocationEntries != NULL);
    }

    ChkArg((f_fVerify &  DRM_WCP_VERIFY_CERT_ALL) != 0);
    ChkArg((f_fVerify & ~DRM_WCP_VERIFY_CERT_ALL) == 0);

    if ((f_fVerify & DRM_WCP_VERIFY_CERT_REQUIRED_TAGS) != 0)
    {
        ChkDR(_VerifyCertificateRequiredTags(f_pszBase, f_pdasstrCert));
    }

    /*
    ** first we find out what type the cert is so that we can pass the appropriate flag
    */
    ChkDR(DRM_XML_GetNodeA(f_pszBase, f_pdasstrCert,     &g_dastrTagWMDRMCertificate,  NULL, NULL, 0, NULL, &dasstrElements));
    ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrElements,   &g_dastrTagSignature,         NULL, NULL, 0, NULL, &dasstrSignature));
    ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrSignature,  &g_dastrTagSignedInfo,        NULL, NULL, 0, NULL, &dasstrSignedInfo));

    dr = DRM_XML_GetNodeA(  f_pszBase,
                            &dasstrSignedInfo,
                            &g_dastrTagSignatureMethod,
                            &g_dastrAttributeAlgorithm,
                            &g_dastrURIRSASHA1_Old,
                            0,
                            NULL,
                            &dasstrCertType );
    if( DRM_FAILED( dr ) )
    {
        ChkDR( DRM_XML_GetNodeA(    f_pszBase,
                                    &dasstrSignedInfo,
                                    &g_dastrTagSignatureMethod,
                                    &g_dastrAttributeAlgorithm,
                                    &g_dastrURIRSASHA1,
                                    0,
                                    NULL,
                                    &dasstrCertType) );

        fWMDRMNET = DRM_DSIG_TYPE_STANDARD;
    }
    else
    {
        fWMDRMNET = DRM_DSIG_TYPE_WMDRMNET;
    }

    if ((f_fVerify & DRM_WCP_VERIFY_CERT_METHODS) != 0)
    {
        ChkDR(_VerifyCertificateMethods(f_pszBase, f_pdasstrCert, fWMDRMNET));
    }

    if ((f_fVerify & DRM_WCP_VERIFY_CERT_SIGNATURE) != 0)
    {
        ChkDR(_VerifyCertificateSignature(f_pszBase, f_pdasstrCert, fWMDRMNET, f_pRevocationEntries, f_cRevocationEntries, f_pdasstrPublicRSA, f_pCryptoCtx));
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

static DRM_NO_INLINE DRM_RESULT _GetPublicKey(
    const DRM_CHAR *f_pszBase,
    DRM_SUBSTRING  *f_pdasstrCert,
    DRM_SUBSTRING  *f_pdasstrPubkey)
{
    DRM_RESULT    dr = DRM_SUCCESS;
    DRM_SUBSTRING dasstrData = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstr0    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstr1    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstr2    = EMPTY_DRM_SUBSTRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC__GetPublicKey );

    ChkDR(DRM_XML_GetNodeA   (f_pszBase,  f_pdasstrCert, &g_dastrTagWMDRMCertificate,   NULL, NULL, 0, NULL, &dasstrData));
    ChkDR(DRM_XML_GetSubNodeA(f_pszBase, &dasstrData,    &g_dastrTagWMDRMCertPublicKey, NULL, NULL, 0, NULL, &dasstr0,    1));
    ChkDR(DRM_XML_GetNodeA   (f_pszBase, &dasstr0,       &g_dastrTagKeyValue,           NULL, NULL, 0, NULL, &dasstr1));
    ChkDR(DRM_XML_GetNodeA   (f_pszBase, &dasstr1,       &g_dastrTagRSAKeyValue,        NULL, NULL, 0, NULL, &dasstr2));
    ChkDR(DRM_XML_GetNodeA   (f_pszBase, &dasstr2,       &g_dastrTagModulus,            NULL, NULL, 0, NULL,  f_pdasstrPubkey));

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


static DRM_NO_INLINE DRM_RESULT _ExtractPublicKey(
    const DRM_CHAR            *f_pszBase,
          DRM_SUBSTRING       *f_pdasstrKeyInfo,
          DRM_RSA_PUBLIC_KEY  *f_pPubkeyRSA )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE            rgbModulus [__CB_DECL( DRM_RSA_CB_MODULUS_MAX )] = { 0 };
    DRM_DWORD           cbModulus         = SIZEOF(rgbModulus);
    DRM_SUBSTRING       dasstrKeyValue    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING       dasstrRSAKeyValue = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING       dasstrModulus     = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING       dasstrExponent    = EMPTY_DRM_SUBSTRING;
    DRM_RSA_SUPPORTED_KEY_LENGTHS eKeyLength = eDRM_RSA_UNKNOWN;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC__ExtractPublicKey );

    ChkArg( f_pszBase != NULL );

    ChkDR(DRM_XML_GetNodeA(f_pszBase,  f_pdasstrKeyInfo,  &g_dastrTagKeyValue,    NULL, NULL, 0, NULL, &dasstrKeyValue));
    ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrKeyValue,    &g_dastrTagRSAKeyValue, NULL, NULL, 0, NULL, &dasstrRSAKeyValue));
    ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrRSAKeyValue, &g_dastrTagModulus,     NULL, NULL, 0, NULL, &dasstrModulus));
    ChkDR(DRM_XML_GetNodeA(f_pszBase, &dasstrRSAKeyValue, &g_dastrTagExponent,    NULL, NULL, 0, NULL, &dasstrExponent));

    ChkArg(DRM_UTL_DASSTRStringsEqual(f_pszBase, &dasstrExponent, &g_dastrWMDRMCertExponent));

    ChkDR(DRM_WCP_ReadPubKeyModulus( f_pszBase,
                                    &dasstrModulus,
                                     rgbModulus,
                                    &cbModulus ) );

    ChkDR( OEM_RSA_GetKeyLengthFromModulusLength( cbModulus, &eKeyLength ) );

    ChkDR( OEM_RSA_SetPublicKey(
        eKeyLength,
        sizeof( rgbDRM_RSA_DEFAULT_PUBEXP ),
        rgbDRM_RSA_DEFAULT_PUBEXP,
        cbModulus,
        rgbModulus,
        f_pPubkeyRSA ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/************************************************************************************************
** Function :   DRM_WCP_VerifyCertificateCollection
** Synopsis :
** Arguments :  [f_pszBase]                : Data string buffer
**              [f_pdasstrCollection]      : Pointer to substring of certificates collection
**              [f_pdastrRootPubKey ]      : Pointer to ANSI string with Root key,
**                                           may be NULL (then validation is optional)
**              [f_pRevocationEntries]     : List of revocation entries
**              [f_cRevocationEntries]     : Number of elements in f_pRevocationEntries
**              [f_pdasstrLeafCertificate] : Pointer to substring with leaf certificate,
**                                           output param (optional)
**              [f_pdasstrPubkeyMachine]   : Pointer to substring with leaf cert's public key,
**                                           output param (optional)
**              [f_pCryptoCtx]             : Pointer to crypto context
**
************************************************************************************************/

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WCP_VerifyCertificateCollection(
    IN const DRM_CHAR              *f_pszBase,
    IN const DRM_SUBSTRING         *f_pdasstrCollection,
    IN const DRM_ANSI_CONST_STRING *f_pdastrRootPubKey,
    IN const WMDRMNET_CRL_ENTRY    *f_pRevocationEntries,
    IN       DRM_DWORD              f_cRevocationEntries,
       OUT   DRM_SUBSTRING         *f_pdasstrLeafCertificate,
       OUT   DRM_SUBSTRING         *f_pdasstrPubkeyMachine,
    IN       DRM_CRYPTO_CONTEXT    *f_pCryptoCtx )
{
    DRM_RESULT      dr                   = DRM_SUCCESS;
    DRM_DWORD       iNode                = 0;
    DRM_DWORD       iCert                = 0;
    DRM_DWORD       cNodes               = 0;
    DRM_SUBSTRING   dasstrData           = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING   dasstrNode           = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING   dasstrCollectionVersion = EMPTY_DRM_SUBSTRING;
    DRM_DWORD       dwRootCertIndex         = (DRM_DWORD) MAX_UNSIGNED_TYPE( DRM_DWORD );
    DRM_DWORD       dwLeafCertIndex         = (DRM_DWORD) MAX_UNSIGNED_TYPE( DRM_DWORD );

    struct
    {
        DRM_SUBSTRING   dasstrCertificate;
        DRM_SUBSTRING   dasstrPubKeyCert;
        DRM_SUBSTRING   dasstrPubKeySigner;
        DRM_DWORD       dwChildCertIndex;
    } rgcertInfo [DRM_CERTIFICATE_COLLECTION_MAX] = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_DRM_WCP_VerifyCertificateCollection );

    ChkArg( f_pszBase != NULL);
    ChkArg( f_pCryptoCtx != NULL );

    ChkDR(DRM_XML_GetNodeA(f_pszBase,
                           f_pdasstrCollection,
                          &g_dastrTagCertificateCollection,
                           NULL,
                           NULL,
                           0,
                          &dasstrNode,
                          &dasstrData));

    /* check for certificate collection c:Version attribute */

    if( DRM_SUCCEEDED( DRM_XML_GetNodeAttributeA (f_pszBase, &dasstrNode, &g_dastrAttributeVersionWMDRM, &dasstrCollectionVersion ) ) )
    {
        ChkBOOL(DRM_UTL_DASSTRStringsEqual(f_pszBase, &dasstrCollectionVersion, &g_dastrVersionWMDRM), DRM_E_INVALID_DEVCERT_ATTRIBUTE);
    }

    /*
    ** Enumerate c:Certificate nodes
    */
    ChkDR(DRM_XML_CountMatchingNodesA(f_pszBase,
                                     &dasstrData,
                                     &g_dastrTagWMDRMCertificate,
                                      NULL,
                                      NULL,
                                     &cNodes));

    ChkArg(cNodes >= 1
        && cNodes <= DRM_CERTIFICATE_COLLECTION_MAX);

    /*
    ** Collect public keys and verify each certificate in chain
    */
    for (iNode = 0; iNode < cNodes; iNode++)
    {
        DRM_SUBSTRING dasstrTag         = EMPTY_DRM_SUBSTRING;

        rgcertInfo[iNode].dwChildCertIndex = (DRM_DWORD) MAX_UNSIGNED_TYPE( DRM_DWORD ); /* not initialized */

        ChkDR(DRM_XML_EnumNextNodeA(f_pszBase,
                                   &dasstrData,
                                    iNode,
                                   &dasstrTag,
                                   &rgcertInfo[iNode].dasstrCertificate,
                                    NULL,
                                    NULL,
                                    NULL));

        /* there should be nothing but <c:Certificate>s in <CertificateCollection> */

        ChkArg(DRM_UTL_DASSTRStringsEqual(f_pszBase,
                                         &dasstrTag,
                                         &g_dastrTagWMDRMCertificate));

        ChkDR(DRM_WCP_VerifyCertificate(f_pszBase,
                                       &rgcertInfo[iNode].dasstrCertificate,
                                        DRM_WCP_VERIFY_CERT_ALL,
                                        f_pRevocationEntries,
                                        f_cRevocationEntries,
                                       &rgcertInfo [iNode].dasstrPubKeySigner,
                                        f_pCryptoCtx ));

        ChkDR(_GetPublicKey(f_pszBase,
                           &rgcertInfo[iNode].dasstrCertificate,
                           &rgcertInfo[iNode].dasstrPubKeyCert));
    }

    /*
    ** Now we have the public keys parsed out;
    ** 1. Make sure that no cert in the chain signs more that one cert, and there is only one root cert
    **    (i.e. no branches or loops in cert chain)
    ** 2. Find the root cert in chain to compare its signing key with MS root key (optionally).
    ** 3. Find leaf cert in chain.
    ** For each cert:
    **  - find its child cert:
    **      - if it does not exist then it's a leaf cert; otherwise remember child cert's index.
    **      - if > 1 child cert is found error out;
    **  - check for its parent cert:
    **      - if none exists then root cert is found;
    **  - if > 1 root cert exist then error out
    **  - if > 1 leaf cert exists then error out
    **  Note: this algorithm assumes that all public keys are different.
    */
    for ( iNode = 0; iNode < cNodes; iNode++ )
    {
        DRM_BOOL fCertWithNoSignerFound = TRUE;
        DRM_BOOL fCertWithNoParentFound = TRUE;

        for ( iCert = 0; iCert < cNodes; iCert++ )
        {
            /*
            ** Make Sure that the node isn't signing itself
            */
            if ( iNode == iCert )
            {
                continue;
            }

            /*
            ** Check that iNode's public key is somebody's signer, and it signs only one cert
            */
            if ( DRM_UTL_DASSSTRStringsEqual( f_pszBase,
                                              &rgcertInfo [iNode].dasstrPubKeyCert,
                                              f_pszBase,
                                              &rgcertInfo [iCert].dasstrPubKeySigner) )
            {
                DRM_DWORD dwChildSecLevel = 0;
                DRM_DWORD dwParentSecLevel = 0;
                ChkBOOL( rgcertInfo [iNode].dwChildCertIndex == (DRM_DWORD) MAX_UNSIGNED_TYPE( DRM_DWORD ),
                         DRM_E_VERIFICATION_FAILURE );
                fCertWithNoParentFound = FALSE;
                rgcertInfo [iNode].dwChildCertIndex = iCert;

                /*
                ** check that security level of the child cert is not higher than parent's
                */
                ChkDR( DRM_WCP_GetSecurityLevel( f_pszBase,
                                                 &rgcertInfo [iNode].dasstrCertificate,
                                                 &dwParentSecLevel ) );
                ChkDR( DRM_WCP_GetSecurityLevel( f_pszBase,
                                                 &rgcertInfo [iCert].dasstrCertificate,
                                                 &dwChildSecLevel ) );
                ChkBOOL( dwChildSecLevel <= dwParentSecLevel, DRM_E_VERIFICATION_FAILURE );
            }
            /* check if iNode is signed by any cert in the chain, or it's a root cert */
            if ( DRM_UTL_DASSSTRStringsEqual( f_pszBase,
                                              &rgcertInfo [iNode].dasstrPubKeySigner,
                                              f_pszBase,
                                              &rgcertInfo [iCert].dasstrPubKeyCert) )
            {
                fCertWithNoSignerFound = FALSE;
            }
        }
        if ( fCertWithNoSignerFound )
        {
            ChkBOOL( dwRootCertIndex == (DRM_DWORD) MAX_UNSIGNED_TYPE( DRM_DWORD ),
                     DRM_E_VERIFICATION_FAILURE );

            dwRootCertIndex = iNode;
            /*
            ** Optionally match with MS root key
            */
            if ( f_pdastrRootPubKey != NULL )
            {
                ChkBOOL( DRM_UTL_DASSTRStringsEqual( f_pszBase,
                                                     &rgcertInfo [iNode].dasstrPubKeySigner,
                                                     f_pdastrRootPubKey ),
                         DRM_E_VERIFICATION_FAILURE );
            }
        }
        if ( fCertWithNoParentFound )
        {
            ChkBOOL( dwLeafCertIndex == (DRM_DWORD) MAX_UNSIGNED_TYPE( DRM_DWORD ),
                     DRM_E_VERIFICATION_FAILURE );
            dwLeafCertIndex = iNode;
        }
    }

    /*
    ** Optionally pass out the pubkey of the leafmost node and leaf certificate
    */
    if (f_pdasstrPubkeyMachine != NULL)
    {
        ChkBOOL( dwLeafCertIndex < DRM_CERTIFICATE_COLLECTION_MAX, DRM_E_VERIFICATION_FAILURE );
        f_pdasstrPubkeyMachine->m_ich = rgcertInfo[dwLeafCertIndex].dasstrPubKeyCert.m_ich;
        f_pdasstrPubkeyMachine->m_cch = rgcertInfo[dwLeafCertIndex].dasstrPubKeyCert.m_cch;
    }

    if (f_pdasstrLeafCertificate != NULL)
    {
        ChkBOOL( dwLeafCertIndex < DRM_CERTIFICATE_COLLECTION_MAX, DRM_E_VERIFICATION_FAILURE );
        f_pdasstrLeafCertificate->m_ich = rgcertInfo[dwLeafCertIndex].dasstrCertificate.m_ich;
        f_pdasstrLeafCertificate->m_cch = rgcertInfo[dwLeafCertIndex].dasstrCertificate.m_cch;
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WCP_ReadPubKeyModulus(
    IN                  const DRM_CHAR      *f_rgchB64,
    IN                        DRM_SUBSTRING *f_pdasstrB64,
    OUT                       DRM_BYTE      *f_pbMod,
    OUT                       DRM_DWORD     *f_pcbMod)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_rgchB64 && f_pdasstrB64 && f_pbMod && f_pcbMod );

    ChkDR( DRM_B64_DecodeA( f_rgchB64, f_pdasstrB64, f_pcbMod, f_pbMod, 0 ) );

ErrorExit:
    return dr;
}


DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WCP_VerifyDigitalSignature(
    IN const DRM_CHAR             *f_pszBase,
    IN const DRM_SUBSTRING        *f_pdasstrData,      /* inclusive */
    IN const DRM_SUBSTRING        *f_pdasstrSignature, /* inclusive */
    IN const DRM_BOOL              f_fWMDRMNET,
    IN const WMDRMNET_CRL_ENTRY   *f_pRevocationEntries,
    IN       DRM_DWORD             f_cRevocationEntries,
    IN       DRM_CRYPTO_CONTEXT   *f_pCryptoCtx )
{
    DRM_RESULT          dr                   = DRM_SUCCESS;
    DRM_SHA_CONTEXT     contextSHA           = DRM_SHA_CONTEXT_DEFAULT;
    DRM_SUBSTRING       dasstrSignedInfo     = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING       dasstrSignatureInner = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING       dasstrReference      = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING       dasstrSignatureValue = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING       dasstrSignedInfoIncl = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING       dasstrDigestValue    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING       dasstrKeyInfo        = EMPTY_DRM_SUBSTRING;
    DRM_BYTE            rgbSHAEncoded    [__CB_DECL(DRM_SHA1_DIGEST_LEN)];
    DRM_BYTE            rgbSHACalculated [__CB_DECL(DRM_SHA1_DIGEST_LEN)];
    DRM_BYTE            rgbSignature     [__CB_DECL(DRM_RSA_CB_MODULUS_MAX)];
    DRM_DWORD           cbSHAEncoded         = SIZEOF(rgbSHAEncoded);
    DRM_DWORD           cbSignature          = SIZEOF(rgbSignature);
    DRM_RSA_PUBLIC_KEY  pubkeyRSA            = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_DRM_WCP_VerifyDigitalSignature );

    ChkArg( f_pszBase          != NULL
        &&  f_pdasstrData      != NULL
        &&  f_pdasstrSignature != NULL
        &&  f_pCryptoCtx       != NULL );

    ChkDR( DRM_XML_GetNodeA( f_pszBase,
                             f_pdasstrSignature,
                            &g_dastrTagSignature,
                             NULL,
                             NULL,
                             0,
                             NULL,
                            &dasstrSignatureInner ) );
    ChkDR( DRM_XML_GetNodeA( f_pszBase,
                            &dasstrSignatureInner,
                            &g_dastrTagSignedInfo,
                             NULL,
                             NULL,
                             0,
                            &dasstrSignedInfoIncl,
                            &dasstrSignedInfo ) );

    /* get the signature value */

    ChkDR( DRM_XML_GetNodeA( f_pszBase,
                            &dasstrSignatureInner,
                            &g_dastrTagSignatureValue,
                             NULL,
                             NULL,
                             0,
                             NULL,
                            &dasstrSignatureValue ) );
    ChkDR( DRM_XML_GetNodeA( f_pszBase,
                            &dasstrSignatureInner,
                            &g_dastrTagKeyInfo,
                             NULL,
                             NULL,
                             0,
                             NULL,
                            &dasstrKeyInfo ) );

    ChkDR( DRM_B64_DecodeA ( f_pszBase,
                            &dasstrSignatureValue,
                            &cbSignature,
                            rgbSignature,
                            0 ) );

    ChkDR( _ExtractPublicKey( (DRM_CHAR *) f_pszBase,
                              (DRM_SUBSTRING *) &dasstrKeyInfo,
                              &pubkeyRSA ) );

    if (f_fWMDRMNET == DRM_DSIG_TYPE_WMDRMNET)
    {
        ChkDR( OEM_RSA_PssVerify( &pubkeyRSA,
                                   f_pdasstrData->m_cch,
                                  ( const DRM_BYTE * )f_pszBase,
                                   f_pdasstrData->m_ich,
                                   cbSignature,
                                   rgbSignature,
                                   f_pCryptoCtx ) );
    }
    else
    {
        ChkDR( OEM_RSA_PssVerify( &pubkeyRSA,
                                   dasstrSignedInfoIncl.m_cch,
                                  ( const DRM_BYTE * )f_pszBase,
                                   dasstrSignedInfoIncl.m_ich,
                                   cbSignature,
                                   rgbSignature,
                                   f_pCryptoCtx  ) );
    }

    /* extract and decode the DigestValue */

    ChkDR( DRM_XML_GetNodeA( f_pszBase,
                            &dasstrSignedInfo,
                            &g_dastrTagReference,
                            NULL,
                            NULL,
                            0,
                            NULL,
                           &dasstrReference ) );
    ChkDR( DRM_XML_GetNodeA( f_pszBase,
                            &dasstrReference,
                            &g_dastrTagDigestValue,
                            NULL,
                            NULL,
                            0,
                            NULL,
                           &dasstrDigestValue ) );
    ChkDR( DRM_B64_DecodeA( f_pszBase,
                           &dasstrDigestValue,
                           &cbSHAEncoded,
                           rgbSHAEncoded,
                           0 ) );

    /*
    **  Search the revocation entries for this digest value to check if the certificate is revoked.
    */
    while(f_cRevocationEntries > 0)
    {
        f_cRevocationEntries--;
        if ( MEMCMP( &f_pRevocationEntries[f_cRevocationEntries],
                      rgbSHAEncoded,
                      SIZEOF(rgbSHAEncoded) ) == 0 )
        {
            /* matched a revocation entry */
            ChkDR( DRM_E_CERTIFICATE_REVOKED );
        }
    }

    /* calculate the SHA hash from the Data node  */

    ChkDR( DRM_SHA_Init( &contextSHA, eDRM_SHA_1 ) );
    ChkDR( DRM_SHA_UpdateOffset( (const DRM_BYTE*)f_pszBase,
                                 f_pdasstrData->m_ich,
                                 f_pdasstrData->m_cch,
                                &contextSHA ) );
    ChkDR( DRM_SHA_Finalize( &contextSHA, DRM_SHA1_DIGEST_LEN, rgbSHACalculated ) );

    /* compare them */

    ChkBOOL( MEMCMP( rgbSHAEncoded,
                     rgbSHACalculated,
                     DRM_SHA1_DIGEST_LEN ) == 0,
                     DRM_E_INVALID_SIGNATURE );

ErrorExit:
    OEM_RSA_ZeroPublicKey( &pubkeyRSA );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

EXIT_PK_NAMESPACE_CODE;

