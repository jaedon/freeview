/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmbase64.h>
#include <drmcommon.h>

ENTER_PK_NAMESPACE_CODE;

#define CCH_B64_IN_QUARTET  4
#define CB_B64_OUT_TRIO     3

static const DRM_BYTE   g_rgbBase64Decode [] =
{
    /* Note we also accept ! and + interchangably. */
    /* Note we also accept * and / interchangably. */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*   0 -   7 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*   8 -  15 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*  16 -  23 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*  24 -  31 */
    0x00, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*  32 -  39 */
    0x00, 0x00, 0x3f, 0x3e, 0x00, 0x00, 0x00, 0x3f, /*  40 -  47 */
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, /*  48 -  55 */
    0x3c, 0x3d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*  56 -  63 */
    0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, /*  64 -  71 */
    0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, /*  72 -  79 */
    0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, /*  80 -  87 */
    0x17, 0x18, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, /*  88 -  95 */
    0x00, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, /*  96 - 103 */
    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, /* 104 - 111 */
    0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, /* 112 - 119 */
    0x31, 0x32, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00  /* 120 - 127 */
};

/*
** Character 65 ('A') is the legitimate zero. All other indices into the above
** array that have the value of zero are invalid Base64 characters.
*/
static const DRM_DWORD g_iBase64ZeroChar = 65;

static const DRM_DWORD g_cbBase64Decode = SIZEOF( g_rgbBase64Decode );

/* when decode in-place, the decoded string will be the start of the input buffer */

static const DRM_DWORD g_fDecodeAllowed = DRM_BASE64_DECODE_IN_PLACE; /* OR in any new supported flags */

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_B64_DecodeW(
    __in                            const DRM_CONST_STRING *f_pdstrSource,
    __inout                               DRM_DWORD        *f_pcbDestination,
    __out_bcount_opt( *f_pcbDestination ) DRM_BYTE         *f_pbDestination,
    __in                                  DRM_DWORD         f_fFlags)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  cchDecode = 0;
    DRM_DWORD  ichSource = 0;
    DRM_DWORD  ibInsert  = 0;
    DRM_DWORD  ibDest    = 0;

    ChkDRMString(f_pdstrSource);
    ChkArg     ((f_pdstrSource->cchString  % CCH_B64_IN_QUARTET) == 0);
    ChkArg      (f_pcbDestination != NULL);
    ChkArg     ((f_fFlags & ~g_fDecodeAllowed) == 0);

    /* Maximum buffer size needed.     */

    cchDecode = (DRM_DWORD) (((f_pdstrSource->cchString + (CCH_B64_IN_QUARTET - 1)) / CCH_B64_IN_QUARTET) * CB_B64_OUT_TRIO);

    if (f_pdstrSource->pwszString [f_pdstrSource->cchString  - 1] == g_wchEqual)
    {
        cchDecode--;
        if (f_pdstrSource->pwszString [f_pdstrSource->cchString  - 2] == g_wchEqual)
        {
            cchDecode--;
        }
    }

    if (((cchDecode > *f_pcbDestination)
       || f_pbDestination == NULL)
    && (f_fFlags & DRM_BASE64_DECODE_IN_PLACE) == 0)
    {
        *f_pcbDestination = cchDecode;
        ChkDR(DRM_E_BUFFERTOOSMALL);
    }

    if (f_fFlags & DRM_BASE64_DECODE_IN_PLACE )
    {
        f_pbDestination = (DRM_BYTE *) f_pdstrSource->pwszString;
        ibInsert        = (f_pdstrSource->cchString * SIZEOF(DRM_WCHAR)) - cchDecode;
    }

    *f_pcbDestination = cchDecode;

    for (ichSource  = f_pdstrSource->cchString;
         ichSource  > 0;
         ichSource -= CCH_B64_IN_QUARTET)
    {
        DRM_BYTE  rgbOutput [__CB_DECL(CB_B64_OUT_TRIO + 1)] = { 0 };
        DRM_DWORD iBase64Decode                              = 0;

        for (ibDest = 0;
             ibDest < CB_B64_OUT_TRIO + 1;
             ibDest++ )
        {
            if (f_pdstrSource->pwszString [ichSource + ibDest - CCH_B64_IN_QUARTET] == g_wchEqual)
            {
                if (ibDest     < 2
                 || ichSource != f_pdstrSource->cchString)
                {
                    ChkDR (DRM_E_INVALID_BASE64);
                }
                break;
            }

            iBase64Decode = (DRM_DWORD) NATIVE_WCHAR(f_pdstrSource->pwszString [((ichSource - CCH_B64_IN_QUARTET) + ibDest)]);

            if ( iBase64Decode >= g_cbBase64Decode )
            {
                /*
                ** The index is larger than the size of g_rgbBase64Decode and
                ** should not be used as an index into g_rgbBase64Decode.  The
                ** string may have been base64Encoded incorrectly.
                */
                ChkDR (DRM_E_INVALID_BASE64);
            }

            if ( 0 == GET_BYTE( g_rgbBase64Decode, iBase64Decode ) && iBase64Decode != g_iBase64ZeroChar )
            {
                /*
                ** The value of this index is zero and it isn't the legitimate
                ** zero char, so this character is not actually a valid Base64
                ** character
                */
                ChkDR( DRM_E_INVALID_BASE64 );
            }

            PUT_BYTE( rgbOutput,
                      ibDest,
                      GET_BYTE(g_rgbBase64Decode, iBase64Decode ) );
        }

        switch (ibDest)
        {
        default:
            PUT_BYTE(f_pbDestination,
                     ibInsert  + --cchDecode,
                     ((GET_BYTE(rgbOutput, 2) & 0x03) << 6) |   GET_BYTE(rgbOutput, 3));

            /*fall through*/

        case 3:
            PUT_BYTE(f_pbDestination,
                     ibInsert  + --cchDecode,
                     ((GET_BYTE(rgbOutput, 1) & 0x0F) << 4) | ((GET_BYTE(rgbOutput, 2) & 0x3C) >> 2));

            /*fall through*/

        case 2:
            PUT_BYTE(f_pbDestination,
                     ibInsert  + --cchDecode,
                      (GET_BYTE(rgbOutput, 0)         << 2) | ((GET_BYTE(rgbOutput, 1) & 0x30) >> 4));
        }
    }

    /* move the buffer to start of input buffer */
    if (f_fFlags & DRM_BASE64_DECODE_IN_PLACE)
    {
        DRM_BYT_MoveBytes((DRM_BYTE *) f_pdstrSource->pwszString,
                           0,
                           f_pbDestination,
                           ibInsert,
                          *f_pcbDestination);
    }

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_B64_DecodeA(
    __in_ecount( f_pdasstrSource->m_ich + f_pdasstrSource->m_cch ) const DRM_CHAR        *f_pszBase,
    __in                                                           const DRM_SUBSTRING   *f_pdasstrSource,
    __inout                                                              DRM_DWORD       *f_pcbDestination,
    __out_bcount_opt( ( f_fFlags & DRM_BASE64_DECODE_IN_PLACE ) == 0 ? *f_pcbDestination : f_pdasstrSource->m_ich + *f_pcbDestination ) DRM_BYTE *f_pbDestination,
    __in                                                                 DRM_DWORD        f_fFlags)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  cchDecode = 0;
    DRM_DWORD  ichSource = 0;
    DRM_DWORD  ichDest   = 0;
    DRM_DWORD  ibDest    = 0;

    ChkArg(f_pdasstrSource  != NULL);
    ChkArg(f_pcbDestination != NULL);
    ChkArg((f_pdasstrSource->m_cch % CCH_B64_IN_QUARTET) == 0);
    ChkArg((f_fFlags & ~g_fDecodeAllowed) == 0);
    ChkArg( f_pdasstrSource->m_cch > 0 );

    cchDecode = (DRM_DWORD) (((f_pdasstrSource->m_cch + (CCH_B64_IN_QUARTET - 1)) / CCH_B64_IN_QUARTET) * CB_B64_OUT_TRIO); /* Maximum buffer size needed.     */

    if (GET_CHAR(f_pszBase,     f_pdasstrSource->m_ich + f_pdasstrSource->m_cch  - 1) == '=')
    {
        cchDecode--;
        if (GET_CHAR(f_pszBase, f_pdasstrSource->m_ich + f_pdasstrSource->m_cch  - 2) == '=')
        {
            cchDecode--;
        }
    }

    if (((cchDecode > *f_pcbDestination)
       || f_pbDestination == NULL)
    && (f_fFlags & DRM_BASE64_DECODE_IN_PLACE) == 0)
    {
        *f_pcbDestination = cchDecode;
        ChkDR (DRM_E_BUFFERTOOSMALL);
    }

    if (f_fFlags & DRM_BASE64_DECODE_IN_PLACE )
    {
        ichDest = f_pdasstrSource->m_ich
                + f_pdasstrSource->m_cch
                - cchDecode;

        f_pbDestination = (DRM_BYTE *) f_pszBase;
    }

    *f_pcbDestination = cchDecode;

    for (ichSource  = f_pdasstrSource->m_cch;
         ichSource  > 0;
         ichSource -= CCH_B64_IN_QUARTET)
    {
        DRM_BYTE  rgbOutput [__CB_DECL(CB_B64_OUT_TRIO + 1)] = { 0 };
        DRM_DWORD iBase64Decode                              = 0;

        for (ibDest = 0;
             ibDest < CB_B64_OUT_TRIO + 1;
             ibDest++ )
        {
            DRM_DWORD ichGet = f_pdasstrSource->m_ich + ichSource + ibDest - CCH_B64_IN_QUARTET;
            if (GET_CHAR(f_pszBase, ichGet) == '=')
            {
                if (ibDest     < 2
                ||  ichSource != f_pdasstrSource->m_cch)
                {
                    ChkDR( DRM_E_INVALID_BASE64 );
                }
                break;
            }

            iBase64Decode = (DRM_DWORD) GET_CHAR(f_pszBase, ichGet);
            if ( iBase64Decode >= g_cbBase64Decode )
            {
                /*
                ** The index is larger than the size of g_rgbBase64Decode and
                ** should not be used as an index into g_rgbBase64Decode.  The
                ** string may have been base64Encoded incorrectly.
                */
                ChkDR( DRM_E_INVALID_BASE64 );
            }

            if ( 0 == GET_BYTE( g_rgbBase64Decode, iBase64Decode ) && iBase64Decode != g_iBase64ZeroChar )
            {
                /*
                ** The value of this index is zero and it isn't the legitimate
                ** zero char, so this character is not actually a valid Base64
                ** character
                */
                ChkDR( DRM_E_INVALID_BASE64 );
            }

            PUT_BYTE( rgbOutput,
                      ibDest,
                      GET_BYTE(g_rgbBase64Decode, iBase64Decode ) );
        }

        ChkArg(f_pbDestination != NULL);
        
        switch (ibDest)
        {
        default:
            PUT_BYTE(f_pbDestination,
                     ichDest + --cchDecode,
                     ((GET_BYTE(rgbOutput, 2) & 0x03) << 6) | GET_BYTE(rgbOutput, 3));

            /*fall through*/

        case 3:
            PUT_BYTE(f_pbDestination,
                     ichDest + --cchDecode,
                     ((GET_BYTE(rgbOutput, 1) & 0x0F) << 4) | ((GET_BYTE(rgbOutput, 2) &0x3C) >> 2));

            /*fall through*/

        case 2:
            PUT_BYTE(f_pbDestination,
                     ichDest + --cchDecode,
                     (GET_BYTE(rgbOutput, 0) << 2) | ((GET_BYTE(rgbOutput, 1) & 0x30) >> 4));
        }
    }

    /* move the buffer to start of input buffer */
    if( f_fFlags & DRM_BASE64_DECODE_IN_PLACE )
    {
        ChkBOOL( f_pdasstrSource->m_cch >= *f_pcbDestination, DRM_E_INVALIDARG );

        DRM_BYT_MoveBytes(f_pbDestination, f_pdasstrSource->m_ich, f_pbDestination, ichDest, *f_pcbDestination);
    }

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}


static const DRM_CHAR g_rgchBase64Encoding[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!*";

static const DRM_CHAR g_rgchBase64EncodingStandard[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
**
*/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_B64_EncodeA(
    __in_bcount( f_cbSource ) const    DRM_BYTE  *f_pbSource,    /* input buffer */
    __in                               DRM_DWORD  f_cbSource,    /* input len */
    __out_ecount_opt( *f_pcchEncoded ) DRM_CHAR  *f_pszB64,      /* output char */
    __inout                            DRM_DWORD *f_pcchEncoded, /* output ch len */
    __in                               DRM_DWORD  f_fFlags )
{
    DRM_DWORD cchRequired = 0;
    DRM_DWORD iInput      = 0;
    DRM_DWORD iOutput     = 0;
    DRM_RESULT dr         = DRM_SUCCESS;
    const DRM_CHAR *pszMapping = NULL;

    /*
    ** Null input buffer, null output size pointer, and a nonzero
    ** encoded size with a null output buffer are all invalid
    ** parameters.
    **
    ** Check the size first so that caller can allocate enough output buffer before generating
    ** the input buffer.
    */

    ChkArg(f_cbSource     > 0
        && f_pbSource    != NULL
        && f_pcchEncoded != NULL);

    if (f_fFlags == 0)
    {
        pszMapping = g_rgchBase64EncodingStandard;
    }
    else if (f_fFlags & DRM_BASE64_ENCODE_WMDRM)
    {
        pszMapping = g_rgchBase64Encoding;
    }
    else
    {
        ChkDR(DRM_E_INVALIDARG);
    }

    cchRequired = CCH_BASE64_EQUIV(f_cbSource);

    /* if the buffer is too small or both buffers are NULL, we return the required buffer size */

    if ((*f_pcchEncoded < cchRequired)
    ||    f_pszB64     == NULL)
    {
        *f_pcchEncoded = cchRequired;
        ChkDR(DRM_E_BUFFERTOOSMALL);
    }

    *f_pcchEncoded = cchRequired;

    /* encoding starts from end of string */

    /*
    ** Convert the input buffer bytes through the encoding table and
    ** out into the output buffer.
    */
    iInput  = (cchRequired / CCH_B64_IN_QUARTET) * CB_B64_OUT_TRIO - CB_B64_OUT_TRIO;
    iOutput =  cchRequired - CCH_B64_IN_QUARTET;

    while (TRUE)
    {
        const DRM_BYTE uc0 =                               GET_BYTE(f_pbSource, iInput);
        const DRM_BYTE uc1 = ((iInput + 1) < f_cbSource) ? GET_BYTE(f_pbSource, iInput + 1) : 0;
        const DRM_BYTE uc2 = ((iInput + 2) < f_cbSource) ? GET_BYTE(f_pbSource, iInput + 2) : 0;

        PUT_BYTE(f_pszB64, iOutput,     GET_BYTE(pszMapping,   uc0 >> 2));
        PUT_BYTE(f_pszB64, iOutput + 1, GET_BYTE(pszMapping, ((uc0 << 4) & 0x30) | ((uc1 >> 4) & 0xf)));
        PUT_BYTE(f_pszB64, iOutput + 2, GET_BYTE(pszMapping, ((uc1 << 2) & 0x3c) | ((uc2 >> 6) & 0x3)));
        PUT_BYTE(f_pszB64, iOutput + 3, GET_BYTE(pszMapping,                               uc2 & 0x3f));

        if (iInput ==0)
        {
            break;
        }

        iOutput -= CCH_B64_IN_QUARTET;
        iInput  -= CB_B64_OUT_TRIO;
    }

    /*
    ** Fill in leftover bytes at the end
    */
    switch (f_cbSource % CB_B64_OUT_TRIO)
    {
        /*
        ** One byte out of three, add padding and fall through
        */
        case 1:
            PUT_CHAR(f_pszB64, cchRequired - 2,  '=');
        /*
        ** Two bytes out of three, add padding.
        */
        case 2:
            PUT_CHAR(f_pszB64, cchRequired - 1,  '=');

            break;
        case 0:
        default:
            break;
    }

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_B64_EncodeW(
    __in_bcount( f_cbSource ) const    DRM_BYTE   *f_pbSource,    /* input buffer */
    __in                               DRM_DWORD   f_cbSource,    /* input len */
    __out_ecount_opt( *f_pcchEncoded ) DRM_WCHAR  *f_pwszEncoded, /* output WCHAR */
    __inout                            DRM_DWORD  *f_pcchEncoded, /* output ch len */
    __in                               DRM_DWORD   f_dwFlags )
{
    DRM_DWORD cchRequired = 0;
    DRM_DWORD iInput      = 0;
    DRM_DWORD iOutput     = 0;
    DRM_RESULT dr         = DRM_SUCCESS;
    const DRM_CHAR *pszMapping = NULL;

    /*
    ** Null input buffer, null output size pointer, and a nonzero
    ** encoded size with a null output buffer are all invalid
    ** parameters.
    **
    ** Check the size first so that caller can allocate enough output buffer before generating
    ** the input buffer.
    */

    ChkArg(f_cbSource     > 0
        && f_pbSource    != NULL
        && f_pcchEncoded != NULL);

    if (f_dwFlags == 0)
    {
        pszMapping = g_rgchBase64EncodingStandard;
    }
    else if (f_dwFlags & DRM_BASE64_ENCODE_WMDRM)
    {
        pszMapping = g_rgchBase64Encoding;
    }
    else
    {
        ChkDR(DRM_E_INVALIDARG);
    }

    cchRequired = CCH_BASE64_EQUIV(f_cbSource);

    /* if the buffer is too small or both buffers are NULL, we return the required buffer size */

    if ((*f_pcchEncoded < cchRequired)
    ||    f_pwszEncoded == NULL)
    {
        *f_pcchEncoded = cchRequired;
        ChkDR(DRM_E_BUFFERTOOSMALL);
    }

    *f_pcchEncoded = cchRequired;

    /* encoding starts from end of string */

    /*
    ** Convert the input buffer bytes through the encoding table and
    ** out into the output buffer.
    */
    iInput  = (cchRequired / CCH_B64_IN_QUARTET) * CB_B64_OUT_TRIO - CB_B64_OUT_TRIO;
    iOutput =  cchRequired - CCH_B64_IN_QUARTET;

    while (TRUE)
    {
        const DRM_BYTE uc0 =                               GET_BYTE(f_pbSource, iInput);
        const DRM_BYTE uc1 = ((iInput + 1) < f_cbSource) ? GET_BYTE(f_pbSource, iInput + 1) : 0;
        const DRM_BYTE uc2 = ((iInput + 2) < f_cbSource) ? GET_BYTE(f_pbSource, iInput + 2) : 0;

        f_pwszEncoded[iOutput]     = WCHAR_CAST(GET_BYTE(pszMapping,   uc0 >> 2));
        f_pwszEncoded[iOutput + 1] = WCHAR_CAST(GET_BYTE(pszMapping, ((uc0 << 4) & 0x30) | ((uc1 >> 4) & 0xf)));
        f_pwszEncoded[iOutput + 2] = WCHAR_CAST(GET_BYTE(pszMapping, ((uc1 << 2) & 0x3c) | ((uc2 >> 6) & 0x3)));
        f_pwszEncoded[iOutput + 3] = WCHAR_CAST(GET_BYTE(pszMapping,                               uc2 & 0x3f));

        if (iInput ==0)
        {
            break;
        }

        iOutput -= CCH_B64_IN_QUARTET;
        iInput  -= CB_B64_OUT_TRIO;
    }

    /*
    ** Fill in leftover bytes at the end
    */
    switch (f_cbSource % CB_B64_OUT_TRIO)
    {
        /*
        ** One byte out of three, add padding and fall through
        */
        case 1:
            f_pwszEncoded[cchRequired - 2] = g_wchEqual;

        /*
        ** Two bytes out of three, add padding.
        */
        case 2:
            f_pwszEncoded[cchRequired - 1] = g_wchEqual;

            break;
        case 0:
        default:
            break;
    }

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}


EXIT_PK_NAMESPACE_CODE;
