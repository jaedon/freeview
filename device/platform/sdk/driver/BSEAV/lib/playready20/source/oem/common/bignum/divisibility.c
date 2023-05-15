/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

/*
        File divisibility.c.    Version 06 December 2002


        digit_t *low_prime_prod_construction(struct bigctx_t *f_pBigCtx)
               -- Returns pointer
                  to a table used by low_prime_divisibility, to
                  check for divisibility by low odd primes.
                  The pointer returned will be NULL
                  if an error is detected.

        DRM_VOID low_prime_prod_destruction(lowprods, struct bigctx_t *f_pBigCtx) --
                  Free memory allocated during low_prime_prod_construction.

        DRM_BOOL low_prime_divisibility(array, lng, lowprods,
                  &pdivisor, struct bigctx_t *f_pBigCtx) --
                  Check our tables for a (not necessarily prime)
                  divisor of array (length lng).  lowprods must come
                  from low_prime_prod_construction.
                  If we succeed, then *pdivisor > 1 will be a product
                  of primes from the tables.  If no divisor is found,
                  then output with *pdivisor = 1.
*/
#include "bignum.h"
#include "bigpriv.h"

ENTER_PK_NAMESPACE_CODE;

//    The LOW_PROD table has 22 32-bit products or 11 64-bit products.
//    Every odd prime below JPFIRST divides one table entry.

#if RADIX_BITS == 32                             // Two products < 2^32
    #define TWO_PRODS(p1, p2) (digit_t)(p1), (digit_t)(p2)
#elif RADIX_BITS == 64                           // One product  < 2^64
    #define TWO_PRODS(p1, p2) (digit_t)(p1) * (digit_t)(p2)
#else
    #error -- "Unexpected RADIX_BITS"
#endif

#define JPFIRST 521
static const DRM_DWORD LOW_PROD_MAGIC = 0x50574F4C;  //  "LOWP"

static const digit_t LOW_PROD[] = {
    TWO_PRODS(4240068105UL, 3842999413UL), // 3*5*7*13*29*43*47*53
                                       //              & 11*17*19*23*31*37*41
    TWO_PRODS(3059475739UL, 4294251953UL), // 59*67*79*97*101    & 61*367*433*443
    TWO_PRODS(4294770617UL, 4294737533UL), // 71*349*353*491     & 73*283*449*463
    TWO_PRODS(4293835597UL, 4294901213UL), // 83*293*383*461     & 89*197*487*503
    TWO_PRODS(4294933861UL, 1353041857UL), // 103*263*331*479    & 107*157*239*337
    TWO_PRODS(4148193053UL, 4286755457UL), // 109*257*373*397    & 113*251*359*421
    TWO_PRODS(4291682933UL, 4294875479UL), // 127*241*281*499    & 131*271*311*389
    TWO_PRODS(4294914791UL, 4294097393UL), // 137*181*379*457    & 139*229*313*431
    TWO_PRODS(4289642801UL, 4280410741UL), // 149*163*347*509    & 151*223*317*401
    TWO_PRODS(4294824337UL, 4294927237UL), // 167*227*277*409    & 173*193*307*419
    TWO_PRODS(4294924747UL, 4294928843UL)};// 179*191*269*467    & 199*211*233*439
#undef TWO_PRODS

static const DRM_DWORD NUM_LOW_PROD =(DRM_DWORD)(sizeof(LOW_PROD)/sizeof(LOW_PROD[0]));

#ifndef HPRODUU
    #define HPRODUU(u1, u2) DRM_UI64High32(DRM_UI64Mul(DRM_UI64((u1)), DRM_UI64((u2))))
#endif

digit_t* DRM_CALL low_prime_prod_construction(struct bigctx_t *f_pBigCtx, DRM_DWORD *pclowprods)
/*
     Construct table with (products of) low primes.
*/
{
    // On 32-bit hosts, 3200 products of two primes cover
    // all odd primes < 65500.  It goes higher on 64-bit hosts
    // (where many products have 3 or 4 primes).
    const DRM_DWORD NUM_PRODUCT = NUM_LOW_PROD + 3200;

    digit_t *lowprods = NULL;        
    DRM_BOOL OK = TRUE;

    if ( pclowprods == NULL )
    {
        OK = FALSE;
    }
    else
    {
        *pclowprods = 0;
    }

    if ( OK )
    {
        lowprods = digit_allocate( NUM_PRODUCT + 2,
                                   f_pBigCtx);

        if (lowprods == NULL)
        {
            OK = FALSE;
        }
        else
        {
            *pclowprods = NUM_PRODUCT + 2;
        }
    }

    if (OK) {
        DRM_DWORD ilp, nprod;
        digit_t jp, jpinc, partial_product;

// Loop over integers jp >= 5 with GCD(jp, 6) = 1.
// Store (products of) primes, until prods table fills.

        lowprods[0] = (digit_t)NUM_PRODUCT;   // First entry has length
        lowprods[1] = (digit_t)LOW_PROD_MAGIC;  // Second entry has magic value
        for (nprod = 0, partial_product = 1, jp = 5, jpinc = 2;
             nprod != NUM_PRODUCT && OK;
            jp += jpinc, jpinc = 6 - jpinc) {  // GCD(jp, 6) = 1
            DRM_BOOL jp_factor_found = FALSE;

                // Check for divisors from LOW_PROD table.

            for (ilp = 0; ilp != NUM_LOW_PROD && !jp_factor_found; ilp++) {
                digit_t gcdtest = 0;
                OK = OK && digit_ogcd(jp, LOW_PROD[ilp],&gcdtest);
                if (OK && gcdtest > 1) jp_factor_found = TRUE;
            }
            if (!OK) {
            } else if (jp_factor_found) {
            } else if (jp < JPFIRST) {
                OK = FALSE;
                SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                                "low_prime_prod_construction");
            } else if (HPRODUU(partial_product, jp) == 0) {
                // If jp has no factor below JPFIRST, treat it as a prime.
                // We combine multiple primes into one word.
                // On 64-bit machines, three 21-bit primes can be combined.

                partial_product *= jp;    // Merge with old product
            } else {    // partial_product * jp would give integer overflow
                lowprods[nprod+2] = partial_product;
                nprod++;
                partial_product = jp; // Start new product
            }
        } // for jp, nprod
    }
    if (!OK && lowprods != NULL) {
        Free_Temporaries(lowprods, f_pBigCtx);
        lowprods = NULL;
    }
    return lowprods;
} // end low_prime_prod_construction
/******************************************************************************/
DRM_VOID DRM_CALL low_prime_prod_destruction
        (digit_t *lowprods,          // IN (freed)
        struct bigctx_t *f_pBigCtx)
{
    Free_Temporaries(lowprods, f_pBigCtx);
}  // end low_prime_prod_destruction
/******************************************************************************/
DRM_BOOL DRM_CALL low_prime_divisibility
        (const digit_t    array[],        // IN
         const DRM_DWORD   lng,            // IN
         __in_ecount( clowprods ) const digit_t *lowprods,
         __in const DRM_DWORD clowprods,
         digit_t    *pdivisor,       // OUT
         struct bigctx_t *f_pBigCtx)
{
    DRM_BOOL OK = TRUE;
    DRM_DWORD itab, ilp, i, ntab = 2;

    if (lng == 0 || IS_EVEN(array[0])) {
        *pdivisor = 2;  // Even
        return OK;
    } else if (lowprods == NULL || clowprods != ( NUM_LOW_PROD + 3200 + 2 ) ) {
        // NUM_LOW_PROD + 3200 + 2 is the size of the prime table constructed
        // in low_prime_prod_construction() at the beginning of the same file.
        ntab = 1;    // Search only the fixed LOW_PROD table
    } else if (lowprods[1] != LOW_PROD_MAGIC) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                        "low_prime_divisibility");
    }

    for (itab = 0; OK && itab != ntab; itab++) {
        const digit_t *table  = (itab == 0 ? LOW_PROD : lowprods + 2);
        const DRM_DWORD tablng = (itab == 0 ? NUM_LOW_PROD : (DRM_DWORD)lowprods[0]);

        for (ilp = 0; ilp != tablng; ilp++) {
            const digit_t prodnow = table[ilp];
            digit_t prodinv = 0, rem = 0, mulby;
            OK = OK && two_adic_inverse(prodnow, &prodinv);
            if (OK) {
                for (i = 0; i != lng; i++) {
                     // array[0:i-1] == rem*RADIX^i (mod prodnow)
                     // 0 <= rem <= prodnow
                    rem += array[i];
                    if (rem < array[i]) rem -= prodnow;   // Unsigned compare

                     // Find mulby so
                     // (new rem) = (rem - mulby*prodnow)/RADIX + prodnow
                     // is an positive integer.
                     // Solution is mulby == rem*prodinv (mod RADIX)

                    mulby = prodinv*rem;      // mod RADIX
                    //assert((digit_t)(mulby*prodnow) == rem);
                    rem = prodnow - HPRODUU(mulby, prodnow);
                }
                OK = OK && digit_ogcd(rem, prodnow, pdivisor);
                if (OK && *pdivisor != 1) return OK;
            }
        } // for ilp
    } // for itab
    if (OK) *pdivisor = 1;
    return OK;
} // low_prime_divisibility

EXIT_PK_NAMESPACE_CODE;
