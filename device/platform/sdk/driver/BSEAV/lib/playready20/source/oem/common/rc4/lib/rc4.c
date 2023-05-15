/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <oemrc4.h>

ENTER_PK_NAMESPACE_CODE;

#include "rc4_key.inc"

/******************************************************************************/
DRM_API DRM_NO_INLINE DRM_VOID DRM_CALL DRM_RC4_Cipher(
    IN OUT                   RC4_KEYSTRUCT *pKS,
    IN                       DRM_DWORD      cbBuffer,
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

        i = ((i + 1) & (RC4_TABLESIZE - 1));
        bTemp1 = GET_BYTE(p,i);
        j = ((j + bTemp1) & (RC4_TABLESIZE - 1));
        
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

EXIT_PK_NAMESPACE_CODE;
