/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

/*
        File mprand.c.  This file has some routines which
        generate random numbers with special distributions.

        DRM_BOOL random_digit_interval(dlow, dhigh, pdout)
              -- Generate pseudorandom value *pdout, dlow <= *pdout <= dhigh.

        DRM_BOOL random_mod(n, array, lng)
                 -- Generate random multiple-precision
                    value -array-, 0 <= array < n.

        DRM_BOOL random_mod_nonzero(n, array, lng)
                 -- Generate random multiple-precision
                    value -array-, 1 <= array < n.

  Alse see file randminv.c
*/

#include "bignum.h"
#include "mprand.h"

ENTER_PK_NAMESPACE_CODE;


DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL random_digit_interval
        (const digit_t  dlow,
         const digit_t  dhigh,
         digit_t  *pdout,
         struct bigctx_t *f_pBigCtx)

{                       /* Return random integer in [dlow, dhigh] */
    DRM_BOOL OK = TRUE;

    if (dhigh < dlow) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                        "random_digit_interval");
    } else {
        const digit_t spread = dhigh - dlow;
        const DRM_DWORD shift_count = RADIX_BITS - significant_bit_count(spread | 1);
        digit_t result = 0;
        do {
            OK = OK && random_digits(&result, 1, f_pBigCtx);
            result >>= shift_count;
        } while (OK && result > spread);
        *pdout = dlow + result;
    }
    return OK;
} /* random_digit_interval */

/****************************************************************************/
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL random_mod
        (const digit_t   n[],
         digit_t    arr[],
         const DRM_DWORD  lng,
         struct bigctx_t *f_pBigCtx)
/*
        Generate pseudorandom value in [0, n - 1].
        n has length lng and must be nonzero.
*/
{
    DRM_DWORD lngsig = lng;
    DRM_BOOL OK = TRUE;

    while (lngsig > 0 && n[lngsig-1] == 0) {
        arr[lngsig-1] = 0;
        lngsig--;
    }

    if (!OK) {
    } else if (n == arr) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_OVERLAPPING_ARGS,
                        "random_mod");
    } else if (lngsig == 0) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_ZERO_OPERAND,
                        "random_mod");
    } else {
        const digit_t nlead = n[lngsig-1];
        DRM_LONG ntry = 0;
        do {
            ntry++;
            if (ntry > 100) {
                OK = FALSE;
                SetMpErrno_clue(MP_ERRNO_TOO_MANY_ITERATIONS,
                                "random_mod");
            }
            OK = OK && random_digits(arr, lngsig-1, f_pBigCtx);
            OK = OK && random_digit_interval(0, nlead,
                                        &arr[lngsig-1], f_pBigCtx);
        } while (OK && compare_same(arr, n, lngsig) >= 0);
    }
    return OK;
} /* random_mod */

/****************************************************************************/
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL random_mod_nonzero
        (const digit_t   n[],
         digit_t    arr[],
         const DRM_DWORD  lng,
         struct bigctx_t *f_pBigCtx)
/*
        Generate pseudorandom value in [1, n-1].  Require n > 1.
*/
{
    DRM_BOOL OK = TRUE;

    if (compare_immediate(n, DIGIT_ONE, lng) <= 0) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                        "random_mod_nonzero");
    } else {
        DRM_LONG ntry = 0;
        do {
            ntry++;
            if (ntry > 100) {
                OK = FALSE;
                SetMpErrno_clue(MP_ERRNO_TOO_MANY_ITERATIONS,
                                "random_mod_nonzero");
            } else {
                OK = random_mod(n, arr, lng, f_pBigCtx);
            }
        } while (OK && significant_digit_count(arr,  lng) == 0);
    }
    return OK;
} /* random_mod_nonzero */

/****************************************************************************/
DRM_BOOL DRM_CALL random_digits(digit_t dtArray[],
                                      const DRM_DWORD lng,
                                      struct bigctx_t *f_pBigCtx)
{    /* Generate random multiple-precision number */
     /* It may have a leading zero. */
    return random_bytes((DRM_BYTE*)dtArray,
                        (DRM_DWORD)lng*SIZEOF(digit_t),
                        f_pBigCtx);
} /* random digits */


EXIT_PK_NAMESPACE_CODE;
