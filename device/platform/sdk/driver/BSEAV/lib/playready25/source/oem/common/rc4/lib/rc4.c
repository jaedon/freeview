/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_RC4_C
#include <oemrc4.h>
#include <oemcommon.h>
#include <drmbytemanip.h>
#include <drmlastinclude.h>

ENTER_EXTERN_C_NAMESPACE;

#include "rc4_key.inc"

/******************************************************************************/
DRM_API DRM_NO_INLINE DRM_VOID DRM_CALL DRM_RC4_CipherImpl(
    __inout                  RC4_KEYSTRUCT *pKS,
    __in                     DRM_DWORD      cbBuffer,
    __inout_bcount(cbBuffer) DRM_BYTE      *pbBuffer )
{
    DRM_BYTE  i = pKS->i;
    DRM_BYTE  j = pKS->j;
    DRM_BYTE *p = pKS->S;
    DRM_DWORD ib = 0;

    while (cbBuffer--)
    {
        DRM_BYTE bTemp1 = 0;
        DRM_BYTE bTemp2 = 0;

        i = (DRM_BYTE)(((i + 1) & (RC4_TABLESIZE - 1)));
        bTemp1 = GET_BYTE(p,i);
        j = (DRM_BYTE)(((j + bTemp1) & (RC4_TABLESIZE - 1)));

        PUT_BYTE(p,i,GET_BYTE(p,j));
        PUT_BYTE(p,j,bTemp1);
        bTemp2 = GET_BYTE(pbBuffer,ib);

        bTemp2 ^= GET_BYTE(p, (GET_BYTE(p,i) + bTemp1) & (RC4_TABLESIZE - 1));
        PUT_BYTE(pbBuffer,ib,bTemp2);
        ib++;
    }

    pKS->i = i;
    pKS->j = j;
}

EXIT_EXTERN_C_NAMESPACE;


ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_VOID DRM_CALL DRM_RC4_ZeroKey(
    __inout           RC4_KEYSTRUCT    *pKS )
{
    if( pKS != NULL )
    {
        OEM_SECURE_ZERO_MEMORY( pKS, SIZEOF( *pKS ) );
    }
}


EXIT_PK_NAMESPACE_CODE;
