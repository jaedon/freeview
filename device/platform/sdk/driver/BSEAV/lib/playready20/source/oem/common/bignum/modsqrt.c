/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include "bignum.h"
#include "mprand.h"

ENTER_PK_NAMESPACE_CODE;
/*
//      DRM_BOOL mod_sqrt(base, answer, modulo, psquare) --
//              Compute a square root
//              answer = +- sqrt(base) mod modulo->modulus.
//              modulo->modulus should be a prime.
//              Set *psquare = TRUE if successful, FALSE if unsuccessful
*/
/***************************************************************************/
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL mod_sqrt
        (const digit_t       base[],           // IN
         digit_t        answer[],         // OUT
         const mp_modulus_t *modulo,           // IN
         DRM_BOOL          *pperfect_square,  // OUT
         struct bigctx_t *f_pBigCtx)
/*
        Compute a square root answer = +- sqrt(base) mod modulo->modulus.
        modulo->modulus should be prime.
        Set *perfect_square = TRUE is successful, FALSE if not.

        base and answer must not overlap.
*/
{
    DRM_BOOL        OK             = TRUE;
    DRM_BOOL        perfect_square = TRUE;
    const DRM_DWORD   elng           = modulo->length;
    digit_t    *dtemps         = NULL;

    if ((elng > 0) &&
        (6*elng + modulo->modmul_algorithm_temps > 6*elng))  /* check for integer underflow/overflow */
    {
        dtemps = digit_allocate(6*elng + modulo->modmul_algorithm_temps, f_pBigCtx);
    }
    
    if (dtemps == NULL) {
        OK = FALSE;
    } else if (base == NULL || answer == NULL) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_NULL_POINTER, "mod_sqrt");
    } else if (base == answer) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_OVERLAPPING_ARGS, "mod_sqrt");
    } else if (significant_digit_count(base,  elng) == 0) {
        perfect_square = TRUE;
        OEM_SECURE_DIGITTCPY( answer,base, elng );      // sqrt(0) = 0
    } else {
        DRM_LONG ntry = 0;
        DRM_BOOL verified = FALSE;
        digit_t *C0 = dtemps;
        digit_t *exponent = dtemps + elng;
        digit_t *temp1 = dtemps + 2*elng;
        digit_t *temp2 = dtemps + 3*elng;
        digit_t *U = dtemps + 4*elng;
        digit_t *V = dtemps + 5*elng;
        digit_t *modmultemps = dtemps + 6*elng;

        switch (modulo->modulus[0] & 7) {
        case 0:
        case 2:
        case 4:
        case 6:
            if (compare_immediate(modulo->modulus, 2, elng) == 0) {
                OEM_SECURE_DIGITTCPY(answer, base, elng );// sqrt(x) == x mod 2
            } else {
                OK = FALSE;
                SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                                "mod_sqrt -- not prime");
            }
            break;
        case 3:
        case 7:
                  // p == 3 mod 4
                  // Let answer == base^((p+1)/4)
                  // Then answer^2 == base*((p+1)/2) == base
                  //      if base is a square

            OK = OK && mp_shift(modulo->modulus, -2, exponent,
                                elng);// (p-3)/4
            if (OK) add_immediate(exponent, 1, exponent, elng);      // (p+1)/4
            OK = OK && mod_exp(base, exponent, elng,
                               answer, modulo, f_pBigCtx);
            break;
        case 5:
            // p == 5 mod 8.
            // Let temp1 == (2*base)^((p - 5)/8))
            // answer = base*temp1 * (2*base*temp1^2 - 1)
            // Then
            //      4*base^2 * temp1^4 = (2 * base)^(2 + (p-5)/2)
            //                         = (2 * base)^ ((p-1)/2) == -1
            //      if base is a square, since 2 is a non-residue.
            // Hence
            //      answer^2
            //       = (base*temp1)^2 * (4*base^2*temp1^4 - 4*base*temp1^2 + 1)
            //       == (base^2 * temp1^2) * (-4*base*temp1^2)
            //       == -4 * base^3 * temp1^4
            //       == base

            OK = OK && mp_shift(modulo->modulus, -3, exponent,
                                elng);// (p-5)/8
            OK = OK && add_mod(base, base, temp2,
                               modulo->modulus, modulo->length);    // 2*base
            OK = OK && mod_exp(temp2, exponent, elng, temp1,
                               modulo, f_pBigCtx);

            OK = OK && mod_mul(base, temp1, temp2, modulo,
                               modmultemps, f_pBigCtx);
                                    // temp2 = base*temp1
            OK = OK && mod_mul(temp2, temp1, temp1, modulo,
                               modmultemps, f_pBigCtx);
            OK = OK && add_mod(temp1, temp1, temp1, modulo->modulus, modulo->length);
            OK = OK && sub_mod(temp1, modulo->one, temp1,
                               modulo->modulus, modulo->length);
            OK = OK && mod_mul(temp2, temp1, answer, modulo,
                               modmultemps, f_pBigCtx);
                                   // answer = temp2 * (2*temp2*temp1 - 1)
            break;
        case 1:
            // p == 1 mod 8
            // Let C0 = base/4
            // Choose C1 in GF(p) randomly.
            // Let alpha, beta satisfy C1 = alpha + beta
            //                     and C0 = alpha * beta

            // Compute
            //        V = alpha^((p+1)/2) + beta^((p+1)/2)
            //        U = (alpha^((p+1)/2) - beta^((p+1)/2)) / (alpha - beta)

            // Observe X^2 - C1*X + C0 = (X - alpha) * (X - beta).

            // If alpha, beta in GF(p),
            //        then alpha^p = alpha and beta^p = beta
            //        (alpha*beta)^((p-1)/2) = C0^((p-1)/2) = 1.
            //        alpha^((p-1)/2) = beta^((p-1)/2)) = k, where k = +-1.
            //        U = (alpha^((p+1)/2) - beta^((p+1)/2)) / (alpha - beta)
            //          = (alpha*k - beta*k)/(alpha - beta) = +- 1.

            // Otherwise alpha, beta in GF(p^2) \ GF(p).
            //        Here alpha^p = beta and beta^p = alpha.
            //        Then V^2 = alpha^(p+1) + 2*(alpha*beta)^((p+1)/2)
            //                               + beta^(p+1)
            //                 = 2*alpha*beta + 2*C0^((p+1)/2)) = 4*C0 = base
            //        if base is a square.

            // In other words, either V^2 = base or U = +- 1,
            //        if base is a square.

            OK = OK && mod_shift(base, -2, C0,
                                 modulo);     // C0 = base/4
            OK = OK && mp_shift(modulo->modulus, -1, exponent,
                                elng);
                                                            // (p-1)/2 (even)
            exponent[0] |= 1;                               // (p+1)/2
            ntry = 0;
            while (OK && !verified) {
                ntry++;
                if (ntry > 100) {
                    OK = FALSE;
                    SetMpErrno_clue(MP_ERRNO_TOO_MANY_ITERATIONS,
                                    "mod_sqrt");
                }
                OK = OK && random_mod(modulo->modulus, temp1,
                                      elng, f_pBigCtx);   // C1
                OK = OK && mod_LucasUV(temp1, C0, exponent,
                                       elng, U, V, modulo, f_pBigCtx);
                OK = OK && mod_mul(V, V, temp2, modulo,
                                   modmultemps, f_pBigCtx);
                if (!OK) {
                } else if (compare_same(temp2, base, elng) == 0) {
                    verified = TRUE;
                    perfect_square = TRUE;
                    OEM_SECURE_DIGITTCPY( answer,V,elng);
                } else {
                    sub_same(modulo->modulus, modulo->one, temp1, elng);  // -1
                    if (OK && compare_same(U, modulo->one, elng) != 0
                           && compare_same(U, temp1, elng) != 0) {
                        verified = TRUE;
                        perfect_square = FALSE;
                                // non-residue or non-prime modulus
                    }
                }
            } // while
            break;
        } /* switch */

        if (!verified) {
            OK = OK && mod_mul(answer, answer, temp2, modulo,
                               modmultemps, f_pBigCtx);
            if (OK && compare_same(temp2, base, elng) != 0) {
                perfect_square = FALSE;
            }
        }
    }
    if (dtemps != NULL) {
        Free_Temporaries(dtemps, f_pBigCtx);
    }
    if (OK) *pperfect_square = perfect_square;
    return OK;
} /* mod_sqrt */

EXIT_PK_NAMESPACE_CODE;