/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <oemdes.h>
#include <byteorder.h>
#include "sel.h"
#include "spb.h"

ENTER_PK_NAMESPACE_CODE;

/* Compress bit flags into a WORD as there are 16 of them
   {0,0,1,1,1,1,1,1,0,1,1,1,1,1,1,0} = 0x3F7E */
static const DRM_WORD double_shift = 0x3F7E;

#define PERM_OP(a,b,t,n,m) ((t)=((((a)>>(n))^(b))&(m)),\
    (b)^=(t),\
    (a)^=((t)<<(n)))

#define HPERM_OP(a,t,n,m) ((t)=((((a)<<(16-(n)))^(a))&(m)),\
    (a)=(a)^(t)^(t>>(16-(n))))


/******************************************************************************/
DRM_API DRM_VOID DRM_CALL DRM_DES_KeySetup(
    OUT       DESTable *pTable,
    __in_bcount( DES_KEYSIZE ) const DRM_BYTE  rgbKey[__CB_DECL(DES_KEYSIZE)] )
{
    DRM_DWORD csel,
              dsel,
              t,
              s,
              i;
    
    DRM_DWORD *kp = (DRM_DWORD *)pTable;

    BYTES_TO_DWORD(csel, rgbKey);
    BYTES_TO_DWORD(dsel, rgbKey + __CB_DECL(SIZEOF(DRM_DWORD)) );

    PERM_OP (dsel,csel,t,4,0x0f0f0f0f);
    dsel = dsel & 0xFFFFFFFF;
    csel = csel & 0xFFFFFFFF;
    t    = t    & 0xFFFFFFFF;
    HPERM_OP(csel,t,-2,0xcccc0000);
    csel = csel & 0xFFFFFFFF;
    t    = t    & 0xFFFFFFFF;
    HPERM_OP(dsel,t,-2,0xcccc0000);
    csel = csel & 0xFFFFFFFF;
    t    = t    & 0xFFFFFFFF;
    PERM_OP (dsel,csel,t,1,0x55555555);
    dsel = dsel & 0xFFFFFFFF;
    csel = csel & 0xFFFFFFFF;
    t    = t    & 0xFFFFFFFF;
    PERM_OP (csel,dsel,t,8,0x00ff00ff);
    dsel = dsel & 0xFFFFFFFF;
    csel = csel & 0xFFFFFFFF;
    t    = t    & 0xFFFFFFFF;
    PERM_OP (dsel,csel,t,1,0x55555555);
    dsel = dsel & 0xFFFFFFFF;
    csel = csel & 0xFFFFFFFF;
    t    = t    & 0xFFFFFFFF;

    dsel=  (((dsel & 0x000000ff) << 16) | (dsel & 0x0000ff00)    |
            ((dsel & 0x00ff0000) >> 16) | ((csel & 0xf0000000)>>4));
    csel &= 0x0fffffff;

    for(i = 0; i < 16; i++)
    {
        DRM_DWORD temp;

        if(double_shift & (1 << (15 - i)))
        {
            csel = ((csel >> 2) | (csel << 26));
            dsel = ((dsel >> 2) | (dsel << 26));
        }
        else
        {
            csel = ((csel >> 1) | (csel << 27));
            dsel = ((dsel >> 1) | (dsel << 27));
        }

        csel &= 0x0fffffff;
        dsel &= 0x0fffffff;

        s =  _DRM_Sel[0][ (csel      )&0x3f                ];
        s |= _DRM_Sel[1][((csel >>  6)&0x03) | ((csel >>  7)&0x3c)];
        s |= _DRM_Sel[2][((csel >> 13)&0x0f) | ((csel >> 14)&0x30)];
        s |= _DRM_Sel[3][((csel >> 20)&0x01) | ((csel >> 21)&0x06)|((csel >> 22)&0x38)];
        t =  _DRM_Sel[4][ (dsel      )&0x3f                ];
        t |= _DRM_Sel[5][((dsel >>  7)&0x03) | ((dsel >>  8)&0x3c)];
        t |= _DRM_Sel[6][ (dsel >> 15)&0x3f                ];
        t |= _DRM_Sel[7][((dsel >> 21)&0x0f) | ((dsel >> 22)&0x30)];

        temp = ((t << 16) | (s & 0x0000ffff)) ;
        temp = temp & 0xFFFFFFFF;
        *(kp++) = ROTATE_LEFT(temp, 2) &0xFFFFFFFF;

        temp = ((s >> 16) | (t & 0xffff0000));
        temp = temp & 0xFFFFFFFF;
        *(kp++) = ROTATE_LEFT(temp, 6) & 0xFFFFFFFF;
    }
}


/******************************************************************************/

/* Primitive function F.
 * Input is r, subkey array in keys, output is XORed into l.
 * Each round consumes eight 6-bit subkeys, one for
 * each of the 8 S-boxes, 2 longs for each round.
 * Each long contains four 6-bit subkeys, each taking up a byte.
 * The first long contains, from high to low end, the subkeys for
 * S-boxes 1, 3, 5 & 7; the second contains the subkeys for S-boxes
 * 2, 4, 6 & 8 (using the origin-1 S-box numbering in the standard,
 * not the origin-0 numbering used elsewhere in this code)
 * See comments elsewhere about the pre-rotated values of r and Spbox.
 */


#define F(l,r,S) { \
    work = ((DRM_DWORD *)pTable)[S  ]; \
    zork = ((DRM_DWORD *)pTable)[S+1];  \
    work ^= r; \
    zork ^= r; \
    zork = ROTATE_RIGHT(zork, 4); \
    l ^= *(DRM_DWORD *)((DRM_BYTE*)_DRM_Spbox[0] + __CB_DECL((work      & 0xfc))); \
    l ^= *(DRM_DWORD *)((DRM_BYTE*)_DRM_Spbox[1] + __CB_DECL((zork      & 0xfc))); \
    l ^= *(DRM_DWORD *)((DRM_BYTE*)_DRM_Spbox[2] + __CB_DECL(((work>> 8)& 0xfc))); \
    l ^= *(DRM_DWORD *)((DRM_BYTE*)_DRM_Spbox[3] + __CB_DECL(((zork>> 8)& 0xfc))); \
    l ^= *(DRM_DWORD *)((DRM_BYTE*)_DRM_Spbox[4] + __CB_DECL(((work>>16)& 0xfc))); \
    l ^= *(DRM_DWORD *)((DRM_BYTE*)_DRM_Spbox[5] + __CB_DECL(((zork>>16)& 0xfc))); \
    l ^= *(DRM_DWORD *)((DRM_BYTE*)_DRM_Spbox[6] + __CB_DECL(((work>>24)& 0xfc))); \
    l ^= *(DRM_DWORD *)((DRM_BYTE*)_DRM_Spbox[7] + __CB_DECL(((zork>>24)& 0xfc))); }


/******************************************************************************/
/* Encrypt or decrypt a block of data in ECB mode */
DRM_API DRM_VOID DRM_CALL DRM_DES_Cipher(
    __in_bcount( DES_BLOCKLEN ) const DRM_BYTE  rgbIn [__CB_DECL(DES_BLOCKLEN)],
    __out_bcount( DES_BLOCKLEN ) DRM_BYTE  rgbOut[__CB_DECL(DES_BLOCKLEN)],
          DESTable *pTable,
          DRM_LONG  op)
{
    DRM_DWORD left, 
              work, 
              right, 
              zork;

    /* Read input block and place in left, right */
    BYTES_TO_DWORD( right, rgbIn);
    BYTES_TO_DWORD( left,  rgbIn + __CB_DECL(SIZEOF(DRM_DWORD)) );

    /* Hoey's clever initial permutation algorithm, from Outerbridge
     * (see Schneier p 478)
     *
     * The convention here is the same as Outerbridge: rotate each
     * register left by 1 bit, i.e., so that "left" contains permuted
     * input bits 2, 3, 4, ... 1 and "right" contains 33, 34, 35, ... 32
     * (using origin-1 numbering as in the FIPS). This allows us to avoid
     * one of the two rotates that would otherwise be required in each of
     * the 16 rounds.
     */

    right = ROTATE_LEFT(right, 4) & 0xFFFFFFFF;
    work = right;
    right ^= left;
    right &= 0xf0f0f0f0;
    work ^= right;
    left ^= right;

    left = ROTATE_LEFT(left, 20) & 0xFFFFFFFF;
    right = work;
    work ^= left;
    work &= 0xfff0000f;
    right ^= work;
    left ^= work;

    left = ROTATE_LEFT(left,14) &0xFFFFFFFF;
    work = right;
    right ^= left;
    right &= 0x33333333;
    work ^= right;
    left ^= right;

    work = ROTATE_LEFT(work, 22) & 0xFFFFFFFF;
    right = work;
    work ^= left;
    work &= 0x03fc03fc;
    right ^= work;
    left ^= work;

    right = ROTATE_LEFT(right, 9) & 0xFFFFFFFF;
    work = right;
    right ^= left;
    right &= 0xaaaaaaaa;
    work ^= right;
    left ^= right;

    left = ROTATE_LEFT(left, 1) & 0xFFFFFFFF;
    right = work;


    /* Now do the 16 rounds */
    if(op == DES_DECRYPT)
    {
        F(left, right, 30);
        F(right, left, 28);
        F(left, right, 26);
        F(right, left, 24);
        F(left, right, 22);
        F(right, left, 20);
        F(left, right, 18);
        F(right, left, 16);
        F(left, right, 14);
        F(right, left, 12);
        F(left, right, 10);
        F(right, left, 8);
        F(left, right, 6);
        F(right, left, 4);
        F(left, right, 2);
        F(right, left, 0);

    }
    else
    {
        F(left, right, 0);
        F(right, left, 2);
        F(left, right, 4);
        F(right, left, 6);
        F(left, right, 8);
        F(right, left, 10);
        F(left, right, 12);
        F(right, left, 14);
        F(left, right, 16);
        F(right, left, 18);
        F(left, right, 20);
        F(right, left, 22);
        F(left, right, 24);
        F(right, left, 26);
        F(left, right, 28);
        F(right, left, 30);
    }


    /* Inverse permutation, also from Hoey via Outerbridge and Schneier */

    right = ROTATE_RIGHT(right, 1)& 0xFFFFFFFF;
    work = left;
    work ^= right;
    work &= 0xaaaaaaaa;
    right ^= work;
    left ^= work;

    left = ROTATE_RIGHT(left, 9)& 0xFFFFFFFF;
    work = right;
    right ^= left;
    right &= 0x03fc03fc;
    work ^= right;
    left ^= right;

    left = ROTATE_RIGHT(left, 22)& 0xFFFFFFFF;
    right = work;
    work ^= left;
    work &= 0x33333333;
    right ^= work;
    left ^= work;

    right = ROTATE_RIGHT(right, 14) & 0xFFFFFFFF;
    work = right;
    right ^= left;
    right &= 0xfff0000f;
    work ^= right;
    left ^= right;

    work = ROTATE_RIGHT(work, 20) &0xFFFFFFFF;
    right = work;
    work ^= left;
    work &= 0xf0f0f0f0;
    right ^= work;
    left ^= work;

    left = ROTATE_RIGHT(left, 4)& 0xFFFFFFFF;

    /* Put the block back into the user's buffer with final swap */
    DWORD_TO_BYTES(rgbOut, left);
    DWORD_TO_BYTES(rgbOut+__CB_DECL(SIZEOF(DRM_DWORD)), right);
}

EXIT_PK_NAMESPACE_CODE;
