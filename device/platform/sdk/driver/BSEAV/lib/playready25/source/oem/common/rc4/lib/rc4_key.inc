/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

/*
 * This implementation file is #included by Rc4.c
 */
DRM_API DRM_NO_INLINE DRM_VOID DRM_CALL DRM_RC4_KeySetupImpl(
    __out      RC4_KEYSTRUCT  *pKS,
    __in       DRM_DWORD       cbKey,
    __in const DRM_BYTE       *pbKey )
{
    DRM_BYTE j;
    DRM_BYTE k;
    DRM_BYTE t;
    DRM_LONG i;

    for (i=0;i<RC4_TABLESIZE;i++)
    {
        PUT_BYTE( pKS->S, i, (DRM_BYTE)i);
    }
    
    pKS->i = 0;
    pKS->j = 0;
    j      = 0;
    k      = 0;
    for (i=0;i<RC4_TABLESIZE;i++)
    {
        t = GET_BYTE(pKS->S,i);
        j = (DRM_BYTE)((j + t + GET_BYTE(pbKey, k)) % RC4_TABLESIZE);        
        PUT_BYTE( pKS->S, i, GET_BYTE(pKS->S, j) );
        PUT_BYTE(pKS->S, j, t);
        k = (DRM_BYTE) ((k + 1) % cbKey);
    }
}

DRM_API DRM_VOID DRM_CALL DRM_RC4_KeySetup(
    __out      RC4_KEYSTRUCT  *pKS,
    __in       DRM_DWORD       cbKey,
    __in const DRM_BYTE       *pbKey )
{
    DRM_RC4_KeySetupImpl( pKS, cbKey, pbKey );
}

/*********************************************************************
**
**  Function:  DRM_RC4_Cipher
**
**  Synopsis:
**
**  Arguments:
**     [pKS] -- Pointer to the KEYSTRUCT created using DRM_RC4_KeySetup.
**     [cbBuffer] -- Size of buffer, in bytes.
**     [pbBuffer] -- Buffer to be encrypted in place.
**
**  Returns:  None
*********************************************************************/

DRM_API DRM_VOID DRM_CALL DRM_RC4_Cipher(
    __inout                  RC4_KEYSTRUCT *pKS,
    __in                     DRM_DWORD      cbBuffer,
    __inout_bcount(cbBuffer) DRM_BYTE      *pbBuffer )
{
    DRM_RC4_CipherImpl( pKS, cbBuffer, pbBuffer );
}
 