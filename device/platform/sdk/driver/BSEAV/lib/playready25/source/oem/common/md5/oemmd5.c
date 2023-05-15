/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_OEMMD5_C
#include <drmcrt.h>
#include <oemmd5.h>
#include <drmbytemanip.h>
#include <oemcommon.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

static const DRM_DWORD MD5_cons[64] =
/* Round 1 */
 {0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
  0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
  0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
  0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,

/* Round 2 */
  0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
  0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
  0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
  0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,

/* Round 3 */
  0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
  0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
  0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
  0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,

/* Round 4 */
  0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
  0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
  0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
  0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};

static const DRM_DWORD magic_md5 = 0x2035444DUL;
                          /* "MD5 " in ASCII */


/*
** Update the MD5 hash from a fresh 64 bytes of data.
*/

static DRM_NO_INLINE DRM_VOID _MD5_block( __inout DRM_MD5_CTX *f_contextMD5)
{
    DRM_DWORD a = f_contextMD5->partial_hash[0];
    DRM_DWORD b = f_contextMD5->partial_hash[1];
    DRM_DWORD c = f_contextMD5->partial_hash[2];
    DRM_DWORD d = f_contextMD5->partial_hash[3];
    DRM_DWORD msg16[32];   /* Two copies of message */
    const DRM_DWORD *cons = MD5_cons;
    DRM_LONG i;

    for (i = 0; i != 16; i++)
    {
        /*
        ** Copy to local array, zero original
        ** Make two copies, to simplify indexing
        */
        msg16[i] = msg16[i+16] = f_contextMD5->awaiting_data[i];
        f_contextMD5->awaiting_data[i] = 0;
    }


    /* Round 1 */
    for (i = -16; i != 0; i += 4)
    {
        /*
        ** Rewrite (X & Y) | (~X & Z)  as  Z ^ (X & (Y ^ Z))
        ** [Easily validated by checking X = 0 and X = 1 cases.]
        ** This avoids ANDNOT (which X86 lacks) and needs only
        ** one temporary register.
        ** On register-rich architectures, the Y ^ Z computation
        ** can start early, before X is computed.
        */

        a += msg16[i+16] + cons[0] + (d ^ (b & (c ^ d)));
        a = b + ROTATE_LEFT(a, 7);

        d += msg16[i+17] + cons[1] + (c ^ (a & (b ^ c)));
        d = a + ROTATE_LEFT(d, 12);

        c += msg16[i+18] + cons[2] + (b ^ (d & (a ^ b)));
        c = d + ROTATE_LEFT(c, 17);

        b += msg16[i+19] + cons[3] + (a ^ (c & (d ^ a)));
        b = c + ROTATE_LEFT(b, 22);

        cons += 4;
    }

    /* Round 2 */
    for (i = -16; i != 0; i += 4)
    {
        /* Rewrite (Z & X) | (~Z & Y)  as  Y ^ (Z & (X ^ Y)) */
        a += msg16[i+17] + cons[0] + (c ^ (d & (b ^ c)));
        a = b + ROTATE_LEFT(a, 5);

        d += msg16[i+22] + cons[1] + (b ^ (c & (a ^ b)));
        d = a + ROTATE_LEFT(d, 9);

        c += msg16[i+27] + cons[2] + (a ^ (b & (d ^ a)));
        c = d + ROTATE_LEFT(c, 14);

        b += msg16[i+16] + cons[3] + (d ^ (a & (c ^ d)));
        b = c + ROTATE_LEFT(b, 20);

        cons += 4;
    }

    /* Round 3 */
    for (i = 16; i != 0; i -= 4)
    {
        a += msg16[i+5]  + cons[0] + ((b ^ c) ^ d);
        a = b + ROTATE_LEFT(a, 4);

        d += msg16[i+8]  + cons[1] + (a ^ (b ^ c));
        d = a + ROTATE_LEFT(d, 11);

        c += msg16[i+11] + cons[2] + ((d ^ a) ^ b);
        c = d + ROTATE_LEFT(c, 16);

        b += msg16[i+14] + cons[3] + (c ^ (d ^ a));
        b = c + ROTATE_LEFT(b, 23);

        cons += 4;
    }

    /* Round 4 */
    for (i = 16; i != 0; i -= 4)
    {
        a += msg16[i  ]  + cons[0] + (c ^ (~d | b));
        a = b + ROTATE_LEFT(a, 6);

        d += msg16[i+7]  + cons[1] + (b ^ (~c | a));
        d = a + ROTATE_LEFT(d, 10);

        c += msg16[i+14] + cons[2] + (a ^ (~b | d));
        c = d + ROTATE_LEFT(c, 15);

        b += msg16[i+5]  + cons[3] + (d ^ (~a | c));
        b = c + ROTATE_LEFT(b, 21);

        cons += 4;
    }

    f_contextMD5->partial_hash[0] += a;
    f_contextMD5->partial_hash[1] += b;
    f_contextMD5->partial_hash[2] += c;
    f_contextMD5->partial_hash[3] += d;
} /* end _MD5_block */


DRM_API DRM_NO_INLINE DRM_VOID DRM_CALL DRM_MD5_Init(
    __inout DRM_MD5_CTX *f_contextMD5 )
{
    DRM_DWORD i;

    f_contextMD5->nbit_total[0] = f_contextMD5->nbit_total[1] = 0;
    for (i = 0; i != 16; i++)
    {
        f_contextMD5->awaiting_data[i] = 0;
    }

     /*
     **  Initialize hash variables.
     **
     **  The initial values in RFC 1321 appear
     **  in byte-reversed order.  Bruce Schneier's
     **  2nd edition neglects to rearrange them.
     */

    f_contextMD5->partial_hash[0] = 0x67452301;
    f_contextMD5->partial_hash[1] = 0xefcdab89;
    f_contextMD5->partial_hash[2] = ~f_contextMD5->partial_hash[0];
    f_contextMD5->partial_hash[3] = ~f_contextMD5->partial_hash[1];
}


/*
**  Append data to a partially hashed MD5 message.
*/
DRM_API DRM_NO_INLINE DRM_VOID DRM_CALL DRM_MD5_Update(
    __inout                     DRM_MD5_CTX *f_contextMD5,
    __in_bcount(f_cbData) const DRM_VOID    *f_rgbData,
    __in                        DRM_DWORD    f_cbData )
{
    const DRM_BYTE *rgbData  = (const DRM_BYTE *)f_rgbData;
    DRM_DWORD cbData         = f_cbData;
    DRM_DWORD iData          = 0;
    DRM_DWORD nbit_occupied  = f_contextMD5->nbit_total[0] & 511;
    DRM_DWORD *awaiting_data = NULL;
    DRM_DWORD nbitnew_low    = 8 * f_cbData;

    f_contextMD5->nbit_total[0] += nbitnew_low;
    f_contextMD5->nbit_total[1] += (DRM_DWORD)(f_cbData >> 29)
                                 + (f_contextMD5->nbit_total[0] < nbitnew_low);

    /* Advance to word boundary in waiting_data */

    if ((nbit_occupied & 31) != 0)
    {
        awaiting_data = f_contextMD5->awaiting_data + nbit_occupied/32;

        while ((nbit_occupied & 31) != 0 && cbData != 0)
        {
            *awaiting_data |= (DRM_DWORD)GET_BYTE(rgbData, iData) << (nbit_occupied & 31);
            iData++;
            cbData--;
            nbit_occupied += 8;
        }
    }

    /* Transfer 4 bytes at a time */
    do
    {
        DRM_DWORD nword_occupied = nbit_occupied/32;
        DRM_DWORD nwcopy         = min(cbData/4, 16 - nword_occupied);

        awaiting_data  = f_contextMD5->awaiting_data
                       + nword_occupied;

        /* check for integer underflow */
        if ((cbData - (4 * nwcopy)) > f_cbData)
        {
            break;
        }

        cbData        -=  4 * nwcopy;
        nbit_occupied += 32 * nwcopy;

        while (nwcopy != 0)
        {
            DRM_BYTE rgbDWORD[__CB_DECL( 4 )];

            PUT_BYTE( rgbDWORD, 0, GET_BYTE( rgbData, iData++ ) );
            PUT_BYTE( rgbDWORD, 1, GET_BYTE( rgbData, iData++ ) );
            PUT_BYTE( rgbDWORD, 2, GET_BYTE( rgbData, iData++ ) );
            PUT_BYTE( rgbDWORD, 3, GET_BYTE( rgbData, iData++ ) );

            /* this check will never fail, but having it here keeps the source */
            /* code diagnostic tools happy.                                    */
            if (awaiting_data < f_contextMD5->awaiting_data + 16)
            {
                BYTES_TO_DWORD( *awaiting_data, rgbDWORD );
                awaiting_data++;
                nwcopy--;
            }
        }

        /* nbit_occupied can never be > 512, however, source code diagnostic tools */
        /* believe that it is possible, so > was added to make the tools happy.    */
        if (nbit_occupied >= 512)
        {
            _MD5_block( f_contextMD5 );
            nbit_occupied  = 0;
            awaiting_data -= 16;
        }
    } while (cbData >= 4);

    while ((cbData != 0) && (cbData < f_cbData))
    {
        DRM_DWORD new_byte = (DRM_DWORD)GET_BYTE( rgbData, iData++ );

        /* this check will never fail, but having it here keeps the source */
        /* code diagnostic tools happy.                                    */
        if (awaiting_data < f_contextMD5->awaiting_data + 16)
        {
            *awaiting_data |= new_byte << (nbit_occupied & 31);
            nbit_occupied  += 8;
            cbData--;
        }
    }
}



/*
** Finish an MD5 hash.
*/
DRM_API DRM_NO_INLINE DRM_VOID DRM_CALL DRM_MD5_Final(
    __inout DRM_MD5_CTX *f_contextMD5 )
{
    DRM_DWORD nbit0 = f_contextMD5->nbit_total[0];
    DRM_DWORD nbit1 = f_contextMD5->nbit_total[1];
    DRM_DWORD nbit_occupied = nbit0 & 511;
    DRM_DWORD i;

    f_contextMD5->awaiting_data[nbit_occupied/32] |= (DRM_DWORD)0x80 << (nbit_occupied & 31);
                          /* Append a 1 bit */
    nbit_occupied += 8;

    /*
    ** Append zero bits until length (in bits) is 448 mod 512.
    ** Then append the length, in bits.
    ** Here we assume the buffer was zeroed earlier.
    */

    if (nbit_occupied > 448)
    {   /* If fewer than 64 bits left */
        _MD5_block( f_contextMD5 );
    }

    f_contextMD5->awaiting_data[14] = nbit0;
    f_contextMD5->awaiting_data[15] = nbit1;

    _MD5_block(f_contextMD5);

    /* Copy final digest to byte array */

    for (i = 0; i != 4; i++)
    {
        DRM_BYTE rgbDWORD[__CB_DECL( 4 )];

        DWORD_TO_BYTES( rgbDWORD, f_contextMD5->partial_hash[i] );
        OEM_SECURE_MEMCPY( (DRM_BYTE*)(f_contextMD5->digest) + 4 * i, rgbDWORD, 4 );
    }
}

EXIT_PK_NAMESPACE_CODE;
