/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmbytemanip.h>
#include <drmcommon.h>

ENTER_PK_NAMESPACE_CODE;


DRM_API DRM_NO_INLINE DRM_VOID DRM_CALL DRM_BYT_ReverseBytes(
    __inout_bcount(cbData)  DRM_BYTE  *pbData,
    __in                    DRM_DWORD  cbData )
{
    DRM_BYTE bTemp;
    DRM_DWORD i;

    if( cbData < 2 )
    {
        return;
    }

    cbData--;
    for( i=0; i<cbData; i++,cbData-- )
    {
        bTemp = GET_BYTE(pbData, i);
        PUT_BYTE(pbData, i, GET_BYTE(pbData, cbData));
        PUT_BYTE(pbData, cbData, bTemp);
    }
}

#if SIXTEEN_BIT_ADDRESSING

DRM_API DRM_VOID DRM_CALL DRM_BYT_CopyBytes(
    __out_ecount( f_ibTo + f_cb )        DRM_VOID *f_pbTo,
    __in                                 DRM_DWORD f_ibTo,
    __in_ecount( f_ibFrom + f_cb ) const DRM_VOID *f_pbFrom,
    __in                                 DRM_DWORD f_ibFrom,
    __in                                 DRM_DWORD f_cb )
{
    DRM_BYTE *pbTo   = (DRM_BYTE*) f_pbTo;
    DRM_BYTE *pbFrom = (DRM_BYTE*) f_pbFrom;

    if( !ISODD(f_ibTo)
     && !ISODD(f_ibFrom) )
    {
        /*
        ** Offsets are both even just use memcpy
        */
        memcpy( pbTo   + (f_ibTo   / CB_NATIVE_BYTE),
                pbFrom + (f_ibFrom / CB_NATIVE_BYTE),
                f_cb / CB_NATIVE_BYTE );
        if( ISODD(f_cb) )
        {
            /*
            ** Copy the last odd byte
            */
            PUT_BYTE(pbTo, f_ibTo+(f_cb-1), GET_BYTE( pbFrom, f_ibFrom+(f_cb-1) ) );
        }
    }
    else if( ISODD(f_ibTo)
          && ISODD(f_ibFrom) )
    {
        /*
        ** Offsets are both odd.
        ** 1. Put the first byte in the right place
        ** 2. Use memcpy for all bytes 1 to f_cb/2
        ** 3. If there is a dangling byte write that at the end
        */
        PUT_BYTE(pbTo, f_ibTo, GET_BYTE( pbFrom, f_ibFrom) );
        f_cb--;
        if( f_cb >= CB_NATIVE_BYTE )
        {
            memcpy( pbTo   + ((f_ibTo+1)   / CB_NATIVE_BYTE),
                    pbFrom + ((f_ibFrom+1) / CB_NATIVE_BYTE),
                    f_cb / CB_NATIVE_BYTE );
        }
        if( ISODD(f_cb) )
        {
            /*
            ** Copy the last odd byte (note that f_cp has previously been decremented)
            */
            PUT_BYTE(pbTo, f_ibTo+f_cb, GET_BYTE( pbFrom, f_ibFrom+f_cb ) );
        }
    }
    else
    {
        /*
        ** The input offset and output offset are unfortuantly not both even or both odd.
        ** This means that each byte must be independantly shifted from one to another
        */
        for( ; f_cb > 0; f_cb--, f_ibTo++, f_ibFrom++ )
        {
            PUT_BYTE(pbTo, f_ibTo, GET_BYTE( pbFrom, f_ibFrom ) );
        }
    }
}

DRM_API DRM_VOID DRM_CALL DRM_BYT_MoveBytes(
    __out_ecount( f_ibTo + f_cb )        DRM_VOID *f_pbTo,
    __in                                 DRM_DWORD f_ibTo,
    __in_ecount( f_ibFrom + f_cb ) const DRM_VOID *f_pbFrom,
    __in                                 DRM_DWORD f_ibFrom,
    __in                                 DRM_DWORD f_cb )
{
    DRM_BYTE *pbTo   = (DRM_BYTE*) f_pbTo;
    DRM_BYTE *pbFrom = (DRM_BYTE*) f_pbFrom;

    if( ( ((DRM_DWORD)f_pbTo * CB_NATIVE_BYTE) + f_ibTo )
        - ( ((DRM_DWORD)f_pbFrom * CB_NATIVE_BYTE) + f_ibFrom ) < f_cb )
    {
        /* The source and destination overlap, */
        /* and the destination is later in memory than the source */

        f_ibTo += f_cb - 1;
        f_ibFrom += f_cb - 1;

        for( ; f_cb > 0; f_cb--, f_ibTo--, f_ibFrom-- )
        {
            PUT_BYTE(pbTo, f_ibTo, GET_BYTE( pbFrom, f_ibFrom ) );
        }
    }
    else if( ( ((DRM_DWORD)f_pbFrom * CB_NATIVE_BYTE) + f_ibFrom )
        - ( ((DRM_DWORD)f_pbTo * CB_NATIVE_BYTE) + f_ibTo ) < f_cb )
    {
        /* The source and destination overlap, */
        /* and the source is later in memory than the destination */

        for( ; f_cb > 0; f_cb--, f_ibTo++, f_ibFrom++ )
        {
            PUT_BYTE(pbTo, f_ibTo, GET_BYTE( pbFrom, f_ibFrom ) );
        }
    }
    else if( !ISODD(f_ibTo)
     && !ISODD(f_ibFrom) )
    {
        /*
        ** Offsets are both even just use memcpy
        */
        memmove(pbTo   + (f_ibTo   / CB_NATIVE_BYTE),
                pbFrom + (f_ibFrom / CB_NATIVE_BYTE),
                f_cb / CB_NATIVE_BYTE );
        if( ISODD(f_cb) )
        {
            /*
            ** Copy the last odd byte
            */
            PUT_BYTE(pbTo, f_ibTo+(f_cb-1), GET_BYTE( pbFrom, f_ibFrom+(f_cb-1) ) );
        }
    }
    else if( ISODD(f_ibTo)
          && ISODD(f_ibFrom) )
    {
        /*
        ** Offsets are both odd.
        ** 1. Put the first byte in the right place
        ** 2. Use memcpy for all bytes 1 to f_cb/2
        ** 3. If there is a dangling byte write that at the end
        */
        PUT_BYTE(pbTo, f_ibTo, GET_BYTE( pbFrom, f_ibFrom) );
        f_cb--;
        if( f_cb >= CB_NATIVE_BYTE )
        {
            memmove( pbTo   + ((f_ibTo+1)   / CB_NATIVE_BYTE),
                     pbFrom + ((f_ibFrom+1) / CB_NATIVE_BYTE),
                     f_cb / CB_NATIVE_BYTE  );
        }
        if( ISODD(f_cb) )
        {
            /*
            ** Copy the last odd byte (note that f_cp has previously been decremented)
            */
            PUT_BYTE(pbTo, f_ibTo+f_cb, GET_BYTE( pbFrom, f_ibFrom+f_cb ) );
        }
    }
    else
    {
        /*
        ** The input offset and output offset are unfortuantly not both even or both odd.
        ** This means that each byte must be independantly shifted from one to another
        */
        for( ; f_cb > 0; f_cb--, f_ibTo++, f_ibFrom++ )
        {
            PUT_BYTE(pbTo, f_ibTo, GET_BYTE( pbFrom, f_ibFrom ) );
        }
    }
}

DRM_API DRM_VOID DRM_CALL DRM_BYT_SetBytes(
    __out_ecount( f_ib + f_cb ) DRM_VOID *f_pb,
    __in                        DRM_DWORD f_ib,
    __in                        DRM_DWORD f_cb,
    __in                        DRM_BYTE  f_b )
{
    DRM_BYTE *pb = (DRM_BYTE*) f_pb;
    DRM_DWORD cb = 0;
    if( ISODD(f_ib) )
    {
        /*
        ** Offset is odd.  Set the first bit before calling memset
        */
        PUT_BYTE( pb, f_ib, f_b );
        f_cb--;
        f_ib++;
    }

    cb = f_cb / CB_NATIVE_BYTE;

    DRMCRT_memset( pb + (f_ib / CB_NATIVE_BYTE), MAKE_MACHINE_BYTE(f_b,f_b), cb );

    f_ib += cb * CB_NATIVE_BYTE;
    if( ISODD(f_cb) )
    {
        /*
        ** Copy the last odd byte
        */
        PUT_BYTE(pb, f_ib, f_b );
    }
}

DRM_API DRM_LONG DRM_CALL DRM_BYT_CompareBytes(
    __in_ecount( f_ibA + f_cb ) const DRM_VOID *f_pbA,
    __in                              DRM_DWORD f_ibA,
    __in_ecount( f_ibB + f_cb ) const DRM_VOID *f_pbB,
    __in                              DRM_DWORD f_ibB,
    __in                              DRM_DWORD f_cb )
{
    DRM_LONG iResult = 0;
    DRM_NATIVE_BYTE bA     = 0;
    DRM_NATIVE_BYTE bB     = 0;
    DRM_BYTE *pbA = (DRM_BYTE*) f_pbA;
    DRM_BYTE *pbB = (DRM_BYTE*) f_pbB;

    if( !ISODD(f_ibA)
     && !ISODD(f_ibB) )
    {
        /*
        ** Offsets are both even just use memcmp
        */
        iResult = memcmp( pbA + (f_ibA / CB_NATIVE_BYTE),
                          pbB + (f_ibB / CB_NATIVE_BYTE),
                          f_cb / CB_NATIVE_BYTE );
        if( iResult == 0
         && ISODD(f_cb) )
        {
            /*
            ** Compare the last odd bytes if needed
            */
            bA = GET_BYTE( pbA, f_ibA+(f_cb-1) );
            bB = GET_BYTE( pbB, f_ibB+(f_cb-1) );
            if( bA == bB )
            {
                iResult = 0;
            }
            else if( bA > bB )
            {
                iResult = 1;
            }
            else
            {
                iResult = -1;
            }
        }
    }
    else if( ISODD(f_ibA)
          && ISODD(f_ibB) )
    {
        /*
        ** Offsets are both odd.
        ** 1. Compare the first byte
        ** 2. Use memcmp for all bytes 1 to f_cb/2
        ** 3. If there is a dangling byte at the end compare that one
        */
        bA = GET_BYTE( pbA, f_ibA);
        bB = GET_BYTE( pbB, f_ibB);

        if( bA == bB )
        {
            iResult = 0;
        }
        else if( bA > bB )
        {
            iResult = 1;
        }
        else
        {
            iResult = -1;
        }

        if( iResult == 0 )
        {
            f_cb--;
            if( f_cb >= CB_NATIVE_BYTE )
            {
                iResult = memcmp( pbA + ((f_ibA+1) / CB_NATIVE_BYTE),
                                  pbB + ((f_ibB+1) / CB_NATIVE_BYTE),
                                  f_cb / CB_NATIVE_BYTE );
            }
            if( iResult == 0
             && ISODD(f_cb) )
            {
                /*
                ** Compare the last odd byte (note that f_cp has previously been decremented)
                */
                bA = GET_BYTE( pbA, f_ibA+f_cb );
                bB = GET_BYTE( pbB, f_ibB+f_cb );
                if( bA == bB )
                {
                    iResult = 0;
                }
                else if( bA > bB )
                {
                    iResult = 1;
                }
                else
                {
                    iResult = -1;
                }
            }
        }
    }
    else
    {
        /*
        ** The input offset and output offset are unfortuantly not both even or both odd.
        ** This means that each byte must be independantly compared
        */
        for( ; f_cb > 0; f_cb--, f_ibA++, f_ibB++ )
        {
            bA = GET_BYTE( pbA, f_ibA );
            bB = GET_BYTE( pbB, f_ibB );
            if( bA == bB )
            {
                iResult = 0;
            }
            else if( bA > bB )
            {
                iResult = 1;
                break;
            }
            else
            {
                iResult = -1;
                break;
            }
        }
    }

    return iResult;
}



/*********************************************************************
**  Function: DRM_16B_Read8BitByte
**
**  Synopsis:  allow one-byte quantities to by read on
**             architectures that cannot address them.
**
**  Arguments:
**     [f_pv]  -- buffer to read with 8-bit bytes
**     [f_ib]  -- index of the 8-bit byte to read
*********************************************************************/

DRM_API DRM_NATIVE_BYTE DRM_CALL DRM_16B_Read8BitByte(
    __in_ecount( f_ib / CB_NATIVE_BYTE ) const DRM_VOID  *f_pv,
    __in                                       DRM_DWORD  f_ib)
{
    DRM_NATIVE_BYTE bNative = 0;

    DRMCASSERT(CB_NATIVE_BYTE == 2);

    /* read the MB-sized word containing the sought byte */

    bNative = ((DRM_NATIVE_BYTE *) f_pv) [f_ib / CB_NATIVE_BYTE];

    /* shift the sought byte into the LSB */

    bNative >>= (8 * ( (f_ib+1) % CB_NATIVE_BYTE));

    /* mask off the other bytes */

    bNative &= ((DRM_NATIVE_BYTE) 0xFF);

    return bNative;
}

/*********************************************************************
**  Function: DRM_16B_Write8BitByte
**
**  Synopsis:  allow one-byte quantities to by written on
**             architectures that cannot address them.
**
**  Arguments:
**     [f_pv]  -- buffer to write with 8-bit bytes
**     [f_ib]  -- index of the byte to write
**     [f_mb]  -- machine byte to write into the address
**
** Note: the other 8-bit byte in the 16-bit byte being written is
**       preserved
*********************************************************************/

DRM_API DRM_VOID DRM_CALL DRM_16B_Write8BitByte(
    __out_ecount( f_ib / CB_NATIVE_BYTE ) DRM_VOID        *f_pv,
    __in                                  DRM_DWORD        f_ib,
    __in                                  DRM_NATIVE_BYTE  f_mb)
{
    DRM_NATIVE_BYTE bNative = 0;
    DRM_NATIVE_BYTE bMask   = 0xFF;

    DRMCASSERT((CB_NATIVE_BYTE == 2));

    /* shift the LSB into the target position in the machine word: |0|C| --> |C|0| */

    f_mb  <<= (8 * ((f_ib+1) % CB_NATIVE_BYTE));
    bMask <<= (8 * ((f_ib+1) % CB_NATIVE_BYTE));

    /* read to the MB-sized word containing the sought byte: |A|B| */

    bNative = ((DRM_NATIVE_BYTE *) f_pv) [f_ib / CB_NATIVE_BYTE];

    /* mask off the position where the new byte will be written: |A|0| */
    /* compose the two MBs to the new one: |A|0| | |0|C| --> |A|C| */

    bNative &= ~bMask;
    bNative |= f_mb;

    /* write the composed MB */

    ((DRM_NATIVE_BYTE *) f_pv) [f_ib / CB_NATIVE_BYTE] = bNative;
}

/*********************************************************************
**  Functions:  DRM_16B_Read8BitChar
**              DRM_16B_Write8BitChar
**
**  Synopsis:  signed-char versions of the above
*********************************************************************/

DRM_API DRM_NATIVE_CHAR DRM_CALL DRM_16B_Read8BitChar(
    __in_ecount( f_cb / CB_NATIVE_BYTE ) DRM_VOID *f_pch,
    __in                                 DRM_DWORD f_ich)
{
    return (DRM_NATIVE_CHAR) DRM_16B_Read8BitByte(f_pch, f_ich);
}

DRM_API DRM_VOID DRM_CALL DRM_16B_Write8BitChar(
    __out DRM_VOID        *f_pch,
    __in  DRM_DWORD        f_ich,
    __in  DRM_NATIVE_CHAR  f_mch)
{
    DRM_16B_Write8BitByte(f_pch, f_ich, (DRM_NATIVE_BYTE) f_mch);
}

/*********************************************************************
**  Functions:  DRM_16B_Read8BitBytes
**              DRM_16B_Write8BitBytes
**
**  Synopsis:  allow one-byte streams to by read and written on
**             architectures that cannot read and write them.
**
**  Arguments:
**     [f_pv]  -- buffer to read/write with 8-bit bytes
**     [f_cb]  -- number of the bytes to read/write
**     [f_pb]  -- buffer of native machine bytes to read to/write from
*********************************************************************/

DRM_API DRM_VOID DRM_CALL DRM_16B_Read8BitBytes(
    __in_ecount( f_cb / CB_NATIVE_BYTE ) DRM_VOID        *f_pv,
    __in                                 DRM_DWORD        f_cb,
    __out_ecount( f_cb )                 DRM_NATIVE_BYTE *f_pb)
{
    DRM_DWORD ib8In       = 0;
    DRM_DWORD ibNativeOut = 0;

    /* read a machine-byte chunk from the 8-bit input stream */

    for (ib8In  = 0;
         ib8In  < f_cb;
         ib8In += CB_NATIVE_BYTE)
    {
        DRM_NATIVE_BYTE bNative = ((DRM_NATIVE_BYTE *) f_pv) [ib8In / CB_NATIVE_BYTE];

        f_pb [ibNativeOut++] = (bNative & 0x00FF);

        if (ibNativeOut >= f_cb)
        {
            break;
        }

        f_pb [ibNativeOut++] = (bNative >> 8);
    }
}

DRM_API DRM_VOID DRM_CALL DRM_16B_Write8BitBytes(
    __out_ecount( f_cb / CB_NATIVE_BYTE ) DRM_VOID        *f_pv,
    __in                                  DRM_DWORD        f_cb,
    __in_ecount( f_cb )                   DRM_NATIVE_BYTE *f_pb)
{
    DRM_DWORD ibNativeIn = 0;

    for (ibNativeIn  = 0;
         ibNativeIn  < f_cb;
         ibNativeIn += CB_NATIVE_BYTE)
    {
        DRM_NATIVE_BYTE bOut = 0;

        bOut  = f_pb [ibNativeIn] & 0xFF;
        bOut |= f_pb [ibNativeIn  + 1] << 8;

        ((DRM_NATIVE_BYTE *) f_pv) [ibNativeIn / 2] = bOut;
    }
}

DRM_API DRM_VOID DRM_CALL DRM_16B_Read8BitString(
    __in_ecount( f_cch / CB_NATIVE_BYTE ) DRM_VOID        *f_pv,
    __in                                  DRM_DWORD        f_cch,
    __out_ecount( f_cch )                 DRM_NATIVE_CHAR *f_pch)
{
    DRM_16B_Read8BitBytes(f_pv, f_cch, (DRM_NATIVE_BYTE *) f_pch);
}

DRM_API DRM_VOID DRM_CALL DRM_16B_Write8BitString(
    __out_ecount( f_cch / CB_NATIVE_BYTE ) DRM_VOID        *f_pv,
    __in                                   DRM_DWORD        f_cch,
    __in_ecount( f_cch )                   DRM_NATIVE_CHAR *f_pch)
{
    DRM_16B_Write8BitBytes(f_pv, f_cch, (DRM_NATIVE_BYTE *) f_pch);
}

/*****************************************************************************
** Function: DRM_16B_Pack8BitBytes
**
** Synopsis: Convert an array of 16-bit bytes into an array of 8-bit bytes
**           packed into the nibbles of a 16-bit array
**
** Arguments:
**           [f_pbNative] -- buffer of 16-bit bytes
**           [f_cbNative] -- size of f_pbNative
**           [f_pvPacked]  -- recipient buffer for packed bytes, NULL to
**                            pack in place
**           [f_fOptions]  -- flags.  The following are supported:
**
**                            DRM_PACK_BYTES_IN_PLACE - use f_pbNative as
**                                                      the output buffer
*****************************************************************************/

DRM_API DRM_VOID *DRM_CALL DRM_16B_Pack8BitBytes(
    __inout_ecount( f_cbMachine )                    DRM_NATIVE_BYTE *f_pbNative,
    __in                                             DRM_DWORD        f_cbNative,
    __out_ecount_opt( f_cbMachine / CB_NATIVE_BYTE ) DRM_VOID        *f_pvPacked,
    __in                                             DRM_DWORD        f_fOptions)
{
    DRM_DWORD ib     = 0;
    DRM_NATIVE_BYTE *pbOut = NULL;

    if (f_pvPacked != NULL)
    {
        DRMASSERT((f_fOptions & DRM_PACK_BYTES_IN_PLACE) == 0);

        pbOut = (DRM_NATIVE_BYTE *) f_pvPacked;
    }
    else
    {
        DRMASSERT((f_fOptions & DRM_PACK_BYTES_IN_PLACE) != 0);

        pbOut = f_pbNative;
    }

    for (ib = 0; ib < f_cbNative; ib += CB_NATIVE_BYTE)
    {
        pbOut [ib / 2] = ((f_pbNative [ib + 1] << 8) | (f_pbNative [ib] & 0xFF));
    }

    return (DRM_VOID *) pbOut;
}

/*****************************************************************************
** Function: DRM_16B_IncrementPackedPointer
**
** Synopsis: moves an array of 16-bit byte one nibble to the left, effectively
**           executing a psz++
**
**  |a'b|c'd|e'f| --> |b'c|d'e|f'0| with 'a' returned in f_pbHead
**
** Arguments:
**           [f_pbNative] -- buffer of 16-bit bytes
**           [f_cbNative] -- size of f_pbNative
**           [f_pbHead]    -- optional pointer to BYTE to receive value
**                            overwritten by the left-shift
**
** Notes: this function changes data and should be reversed with
**        DRM_16B_DecrementPackedPointer
*****************************************************************************/

DRM_API DRM_VOID *DRM_CALL DRM_16B_IncrementPackedPointer(
    __inout_ecount( f_cbNative / / CB_NATIVE_BYTE ) DRM_NATIVE_BYTE *f_pbNative,
    __in                                            DRM_DWORD        f_cbNative,
    __out                                           DRM_NATIVE_BYTE *f_pbHead)
{
    DRM_DWORD ib = 0;

    DRMASSERT(f_pbNative != NULL
           && f_cbNative != 0);

    if (f_pbHead != NULL)
    {
        *f_pbHead = GET_BYTE(f_pbNative, 0);
    }

    /* repack the remaining string to align with the pointer */

    for (ib  = 0;
         ib  < f_cbNative;
         ib += CB_NATIVE_BYTE)
    {
        f_pbNative [ib / CB_NATIVE_BYTE] = MAKE_MACHINE_BYTE(GET_BYTE(f_pbNative, ib + 1),
                                                             GET_BYTE(f_pbNative, ib + 2));
    }

    f_pbNative [f_cbNative / CB_NATIVE_BYTE] = MAKE_MACHINE_BYTE(GET_BYTE(f_pbNative, ib + 1), 0);

    return f_pbNative;
}

/*****************************************************************************
** Function: DRM_16B_DecrementPackedPointer
**
** Synopsis: moves an array of 16-bit byte one nibble to the right, effectively
**           executing a psz--
**
**  |a'b|c'd|e'f| --> |X'a|b'c|d'e| where X is passed in as f_bHead
**
** Arguments:
**           [f_pbNative] -- buffer of 16-bit bytes
**           [f_cbNative] -- size of f_pbNative
**           [f_bHead]     -- BYTE to place at the new first nibble
**
** Notes: this function reverses DRM_16B_IncrementPackedPointer
*****************************************************************************/

DRM_API DRM_VOID *DRM_CALL DRM_16B_DecrementPackedPointer(
    __inout_ecount( f_cbNative / / CB_NATIVE_BYTE ) DRM_NATIVE_BYTE *f_pbNative,
    __in                                            DRM_DWORD        f_cbNative,
    __in                                            DRM_NATIVE_BYTE  f_bHead)
{
    DRM_DWORD        ibNative = 0;
    DRM_NATIVE_BYTE  bCarry   = f_bHead;

    DRMASSERT(f_pbNative != NULL
           && f_cbNative != 0);

    /*  |b'c|d'e| and a ---> |a'b|c'd|e'0| */

    /* repack the remaining string to align with the pointer */

    for (ibNative  = 0;
         ibNative  < f_cbNative;
         ibNative += CB_NATIVE_BYTE)
    {
        DRM_NATIVE_BYTE b1 = GET_BYTE(f_pbNative, ibNative + 1);

        f_pbNative [ibNative / CB_NATIVE_BYTE] = MAKE_MACHINE_BYTE(bCarry,
                                                                   GET_BYTE(f_pbNative, ibNative));

        bCarry = b1;
    }

    f_pbNative [f_cbNative / CB_NATIVE_BYTE] = MAKE_MACHINE_BYTE(bCarry, 0);

    return f_pbNative;
}


#endif /* SIXTEEN_BIT_ADDRESSING */

EXIT_PK_NAMESPACE_CODE;
