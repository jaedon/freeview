/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
/*

       File bignum.c
       Version 20 February 2005

       This file has extern definitions of functions
       which are ordinarily inlined.  In particular, it exists to
       support compilers which don't support inlining.
       It also has simple routines defined nowhere else.
*/
#define DRM_BUILDING_BIGNUM_C
#include "bignum.h"
#include "drmsal.h"
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;
/*
        DRM_DWORD mp_significant_digit_count(a, lng) --  Count the number
                                            of significant bits in a.
                                            This is one more than the
                                            truncated base 2 logarithm of a.

        DRM_DWORD significant_bit_count(d) -- Compute the number of
                                          significant bits in d (d != 0).
                                          This is one more than the
                                          truncated base 2 logarithm of d.

        DRM_BOOL two_adic_inverse(d, &dinv, &ctx) -- Returns dinv so that
                                           d*dinv == 1 (mod RADIX).
                                           d must be odd.

*/
/****************************************************************************/

DRM_NO_INLINE DRM_API DRM_BOOL DRM_CALL two_adic_inverse( const digit_t d, digit_t *pdinv )
/*
      Find *pdinv so that (*pdinv)*d == 1 (mod RADIX)
*/
{
    DRM_BOOL OK = TRUE;

    if (IS_EVEN(d)) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                        "two_adic_inverse");
    } else {
        digit_t dinv = (3*d)^2;     /* 5-bit  2-adic inverse approximation */
        digit_t err = 1 - d*dinv;
        DRM_DWORD nbits;   /* Bits of accuracy so far */

        DRMASSERT((err & 31) == 0);
        for (nbits = 5; nbits < RADIX_BITS/2; nbits *= 2) {
            dinv += dinv*err;
            err = err*err;
            DRMASSERT(err == (digit_t)(1 - d*dinv));
        }
        *pdinv = dinv*err + dinv;
    }
    return OK;
} /* two_adic_inverse */

/****************************************************************************/


/*
        Here are some simple routines which fit nowhere else.

      carry  = accumulate(a, mult, b, lng)   -- b := b + mult*a, where a, b
                                                           have length lng


borrow = decumulate(a, mult, b, lng)   -- b := b - mult*a, where a, b
                                                           have length lng
carry  = multiply_immediate(a, mult, b, lng) -- b := mult*a, where a, b
                                                            have length lng

        DRM_BOOL mp_shift(a, amt, b, lng)    -- Compute lower lng words
                                            of b = a*2^amt, where
                                            amt can be arbitrarily large.
                                            Overflow/underflow is lost.
                                            Does left shift if amt >= 0,
                                            right shift if amt < 0.

        digit_t mp_shift_lost(a, amt, b, lng, plost)
                                         -- Compute lower lng words
                                            of b = a*2^amt, where
                                            |amt| <= RADIX_BITS.
                                            *plost will receive bits
                                            lost due to overflow/underflow.

        DRM_BOOL multiply(a, lnga, b, lngb, c) - Compute c = a * b
                                             (classical algorithm).

        DRM_BOOL set_immediate(a, ivalue, lngs) -- Set  a = ivalue (a scalar)

         multiply_low(a, b, c, lng)    -- c := a * b (mod RADIX^lng)
         (see file multlow.c)
*/
/***************************************************************************/
DRM_API DRM_NO_INLINE digit_t DRM_CALL accumulate(
                        const digit_t   a[],
                        const digit_t   mult,
    __inout_ecount(lng)       digit_t   b[],
                        const DRM_DWORD lng)
/*
        Compute b = b + mult*a, where a and b have length lng.
        Function value is carry out of leftmost digit.
*/
{
    digit_t carry = 0;
    DRM_DWORD i;

    for (i = 0; i != lng; i++) {
        const DRM_UINT64 dtemp = MULTIPLY_ADD2(mult, a[i], b[i], carry);
        b[i] = DRM_UI64Low32(dtemp);
        carry = DRM_UI64High32(dtemp);
    }
    return carry;
}  /* accumulate */

/****************************************************************************/
DRM_API DRM_NO_INLINE digit_t DRM_CALL decumulate(
                        const digit_t   a[],
                        const digit_t   mult,
    __inout_ecount(lng)       digit_t   b[],
                        const DRM_DWORD lng )
/*
        Compute b = b - mult*a, where a and b have length lng.
        Function value is borrow out of leftmost digit.
*/
{
    digit_t borrow = 0;
    DRM_DWORD i;

    for (i = 0; i != lng; i++) {
        const DRM_UINT64 dtemp = DRM_UI64Sub(DRM_UI64Sub(DRM_UI64(b[i]),
                                                DRM_UI64(borrow)),
                                     DPRODUU(mult, a[i]));
        b[i] = DRM_UI64Low32(dtemp);
        borrow = 0 - DRM_UI64High32(dtemp);
    }
    return borrow;
}  /* decumulate */
/***************************************************************************/
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL mp_shift(
                        const digit_t   a[],
                        const DRM_LONG  ishift,
    __inout_ecount(lng)       digit_t   b[],
                        const DRM_DWORD lng)
/*
**              Compute  b = a << ishift     (if ishift >= 0)
**                   or  b = a >> (-ishift)  (if ishift < 0).
**
**              Both input and output are length lng.
**              Unlike mp_shift, the shift count may
**              exceed RADIX_BITS bits (either direction).
**              It may even exceed lng*RADIX_BITS.
**              Bits shifted past either end are lost.
*/
{
    const DRM_LONG itranslate = (ishift >= 0 ?   (DRM_LONG) (DRM_DWORD)ishift/RADIX_BITS
                                   : - (DRM_LONG)((DRM_DWORD)(-ishift)/RADIX_BITS));
                   /* Round quotient ishift/RADIX_BITS towards zero */
    DRM_DWORD i;
    DRM_BOOL OK = TRUE;
    digit_t lost = 0;

    OK = OK && mp_shift_lost(a, ishift - RADIX_BITS*itranslate, b,
                             lng, &lost);

    if (!OK) {
    } else if (itranslate < 0) {               /* Right shift, multiple words */
        const DRM_DWORD dtranslate = (DRM_DWORD)(-itranslate);
        for (i = 0; i < lng; i++) {
            DRM_DWORD j = i + dtranslate;
            b[i] = 0;

            /* Skip the following assignment if integer overflow occurrs. */
            if ( j >= i && j >= dtranslate && j < lng )
            {
                b[i] = b[j];
            }
        }
    } else if (itranslate > 0) {        /* Left shift, multiple words */
        const DRM_DWORD dtranslate = (DRM_DWORD)itranslate;
        for (i = lng; (i--) > 0; /*null*/) {
            b[i] = (i >= dtranslate ? b[i - dtranslate] : 0);
        }
    }
    return OK;
} /* mp_shift */
/****************************************************************************/
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL mp_shift_lost(
                        const digit_t    a[],
                        const DRM_LONG   shift_amt,
    __inout_ecount(lng)       digit_t    b[],
                        const DRM_DWORD  lng,
                              digit_t   *plost)
/*
**      Compute b = a*2^shift_amt.
**      Require -RADIX_BITS <= shift_amt <= RADIX_BITS.
**      Function value reflects bits shifted off the
**      right or off the left.
**
**      The arrays a and b should be identical, or not overlap.
*/
{
    DRM_DWORD i;
    DRM_BOOL OK = TRUE;
    digit_t bits_lost = 0;

    if (DRMCRT_abs(shift_amt) > RADIX_BITS) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                        "mp_shift_lost -- bad shift count");
    } else if (lng == 0) {
    } else if (shift_amt > 0) {           /* left shift */
        if (shift_amt == RADIX_BITS) {
            bits_lost = a[lng-1];
            for (i = lng-1; i != 0; i--) b[i] = a[i-1];
            b[0] = 0;
        } else {
            for (i = 0; i != lng; i++) {
                const digit_t bnew = (a[i] << shift_amt) | bits_lost;

                bits_lost = a[i] >> (RADIX_BITS - shift_amt);
                b[i] = bnew;
            }
        }
    } else if (shift_amt < 0) {
        if (shift_amt == -RADIX_BITS) {
            bits_lost = a[0];
            for (i = 1; i != lng; i++) b[i-1] = a[i];
            b[lng-1] = 0;
        } else {

            for (i = lng; (i--) > 0; /*null*/) {
                const digit_t bnew = (a[i] >> (-shift_amt) ) | bits_lost;
                bits_lost = a[i] << (RADIX_BITS + shift_amt);
                b[i] = bnew;
            }
            bits_lost >>= (RADIX_BITS + shift_amt); /* Move to bottom of word */
        }
    } else {
        OEM_SECURE_DIGITTCPY( b,a,lng );
    }
    if (OK) *plost = bits_lost;
    return OK;
} /* mp_shift_lost */

/****************************************************************************/
DRM_API DRM_NO_INLINE digit_t DRM_CALL multiply_immediate(
                        const digit_t     a[],
                        const digit_t     mult,
    __inout_ecount(lng)       digit_t     b[],
                        const DRM_DWORD   lng )
/*
        Compute b = mult*a, where a and b have length lng.
        Function value is carry out of leftmost digit.
*/
{
    digit_t carry = 0;
    DRM_DWORD i;

    for (i = 0; i < lng; i++) {
        const DRM_UINT64 dtemp = MULTIPLY_ADD1(mult, a[i], carry);
        b[i] = DRM_UI64Low32(dtemp);
        carry = DRM_UI64High32(dtemp);
    }
    return carry;
} /* multiply_immediate */
/****************************************************************************/
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL set_immediate(
    __out_ecount(lnga)         digit_t             a[],
    __in                 const digit_t             ivalue,
    __in                 const DRM_DWORD           lnga,
    __inout                    struct bigctx_t    *f_pBigCtx )
{
    DRM_BOOL OK = TRUE;
    if (lnga == 0) {
        if (ivalue != 0) {
            OK = FALSE;
            SetMpErrno_clue(MP_ERRNO_OVERFLOW,
                            "set_immediate");
        }
    } else {
        a[0] = ivalue;
        OEM_SECURE_ZERO_MEMORY(a + 1,( lnga - 1)*SIZEOF( digit_t ));
    }
    return OK;
} /* end set_immediate */
/****************************************************************************/


/*
** This is pretty much a no-op function that is needed for prebuilt
** stublibs for testing.
*/
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL mp_initialization( struct bigctx_t *f_pBigCtx )
{
    return TRUE;
}

/***************************************************************************/
DRM_LONG DRM_CALL compare_immediate
        (const digit_t  a[],
         const digit_t  ivalue,
         const DRM_DWORD    lng)
/*
        Compare a multiple-precision number to a scalar.
*/
{
    return compare_diff(a, lng, &ivalue, 1);
} /* compare_immediate */

EXIT_PK_NAMESPACE_CODE;
