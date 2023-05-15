/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/


#define DRM_BUILDING_OEMRSAKEYGEN_C
#include <bignum.h>
#include <mprand.h>
#include <oemrsaimpl.h>
#include <oemrsa.h>
#include <oemrsacommon.h>
#include <oemeccp160.h>
#include <drmlastinclude.h>

/******************************************************************************
**
** WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
**
** DO NOT MOVE OR USE THESE CONSTANTS OR TYPES OUTSIDE THIS FILE!!!
**
** Current maximum supported length is 4096BIT.
** If you reuse them, you will prevent extending RSA support beyond 4096BIT.
** They should only be defined/used inside the RSA implementation.
**
** WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
**
******************************************************************************/
#define IS_VALID_DRM_RSA_KEY_LENGTH_ALLBIT(x)      IS_VALID_DRM_RSA_KEY_LENGTH_4096BIT(x)
/******************************************************************************
**
** WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
**
******************************************************************************/

ENTER_PK_NAMESPACE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_WRITABLE_BUFFER_25057, "f_pbData and f_pDigits lengths defined by f_cBitsData" )
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL _BigEndianBytesToDigits(
    __in_ecount( BITS_TO_BYTES( f_cBitsData ) ) const DRM_BYTE  *f_pbData,
    __in const                                        DRM_DWORD  f_cBitsData,
    __out_ecount( BITS_TO_DIGITS( f_cBitsData ) )     digit_t   *f_pDigits ) DRM_NO_INLINE_ATTRIBUTE;
PREFAST_POP

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_WRITABLE_BUFFER_25057, "f_pbData and f_pDigits lengths defined by f_cBitsData" )
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL _DigitsToBigEndianBytes(
    __in_ecount( BITS_TO_DIGITS( f_cBitsData ) ) const digit_t   *f_pDigits,
    __in                                               DRM_DWORD  f_cBitsData,
    __out_ecount( BITS_TO_BYTES( f_cBitsData ) )       DRM_BYTE  *f_pbData ) DRM_NO_INLINE_ATTRIBUTE;
PREFAST_POP

EXIT_PK_NAMESPACE;

ENTER_PK_NAMESPACE_CODE;


typedef struct {
    DRM_DWORD  candidates;                   /* Candidates tried */
    DRM_DWORD  primes_found;
    DRM_DWORD  trial_division_eliminations;  /* Rejections, small divisor found */
    DRM_DWORD  rabin_eliminations;           /* Rejections, Rabin test failed */
    DRM_DWORD  other_eliminations;           /* Other rejections */
} big_prime_search_stat_t;

/* 25K for low primes */
#define DRM_RSA_PKCRYPTO_LOW_PRIME_SIZE 25000

static const DRM_DWORD NUM_RABIN_TEST       = 50;
static const DRM_DWORD MAX_PRIME_CANDIDATES = 5000;


/******************************************************************************
**
** Function :   _big_prime_search
**
** Synopsis :   Search for a huge prime, perhaps 512 or 1024 bits.
**
**              Choose a random bit pattern of the requested length.
**              Set its two most significant bits and the lower bit.
**              Test for divisibility by primes in our table.
**              If no factor found, do some Rabin-like primality checks.
**              When not prime, restart with new random bit-pattern.
**
**              An odd 512-bit integer is prime with probabilty
**              approximately 2 / ln(2^512) or 1 in 180.
**              If we have checked for divisors below a bound B,
**              this probability grows to ln(B)/ln(2^256) ~= log10(B)/78.
**              With B = 10^5, this probability is 15%.
**
** Arguments :  [bitlen_p]     - Length in bits (must be at least 32)
**              [p]            - primes
**              [lowprods]     - From low_prime_prod_construction
**              [clowprods]    - count of elements in the prime table.
**              [pbpstat]      - statistics
**              [f_pCryptoCtx] - Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code
**                               we assume the ctx is already initialized.
**
** Returns :    TRUE  - if primes were successfully created
**              FALSE - otherwise
******************************************************************************/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_WRITABLE_BUFFER_25057, "p length defined by bitlen_p" )
static DRM_BOOL _big_prime_search(
    __in                                     const DRM_DWORD                bitlen_p,
    __inout_ecount(BITS_TO_DIGITS(bitlen_p))       digit_t                 *p,
    __in_ecount( clowprods )                 const digit_t                 *lowprods,
    __in                                     const DRM_DWORD                clowprods,
    __inout                                        big_prime_search_stat_t *pbpstat,
    __inout                                        DRM_CRYPTO_CONTEXT      *f_pCryptoCtx )
PREFAST_POP
{
    const DRM_DWORD diglen_p     = BITS_TO_DIGITS(bitlen_p);
    DRM_BOOL     OK              = TRUE;
    DRM_BOOL     big_prime_found = FALSE;
    DRM_BOOL     modulus_created = FALSE;
    digit_t     *dtemps          = NULL;
    mp_modulus_t p_modulus       = { 0 };
    struct bigctx_t *pBigCtx     = NULL;

    DRMASSERT( p != NULL);
    DRMASSERT( lowprods != NULL && clowprods > 0 );
    DRMASSERT( pbpstat != NULL );
    __analysis_assume( p != NULL );
    __analysis_assume( pbpstat != NULL );

    if( f_pCryptoCtx != NULL )
    {
        pBigCtx = (struct bigctx_t *)f_pCryptoCtx->rgbCryptoContext; /* of size DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE*/

        if (bitlen_p < 32)
        {   /* Comes too close to primes in prods[] array */
            OK = FALSE;
            SetMpErrno_clue(MP_ERRNO_INVALID_DATA, "_big_prime_search");
        } else
        {
            dtemps = digit_allocate(4*diglen_p, pBigCtx);
            if (dtemps == NULL) OK = FALSE;
        }
    }
    else
    {
        OK = FALSE;
    }

    //   Search for a huge prime, perhaps 512 or 1024 bits.

    //   Choose a random bit pattern of the requested length.
    //   Set its two most significant bits and the lower bit.
    //   Test for divisibility by primes in our table.
    //   If no factor found, do some Rabin-like primality checks.
    //   When not prime, restart with new random bit-pattern.

    //   An odd 512-bit integer is prime with probabilty
    //   approximately 2 / ln(2^512) or 1 in 180.
    //   If we have checked for divisors below a bound B,
    //   this probability grows to ln(B)/ln(2^256) ~= log10(B)/78.
    //   With B = 10^5, this probability is 15%.

    while (OK && !big_prime_found)
    {
        digit_t *base     = dtemps;              /* All length diglen_p */
        digit_t *exponent = dtemps +   diglen_p;
        digit_t *minus1   = dtemps + 2*diglen_p;
        digit_t *result   = dtemps + 3*diglen_p;
        DRM_BOOL hit_minus1 = FALSE;
        digit_t gcdtest = 0;

        if (pbpstat->candidates >= MAX_PRIME_CANDIDATES)
        {
            OK = FALSE;
            SetMpErrno_clue(MP_ERRNO_TOO_MANY_ITERATIONS, "_big_prime_search");
        } else
        {
            pbpstat->candidates++;
/*
 *  If job is running interactively, perhaps print pbpstat->candidates
 *  and pbpstat->primes_found to the screen occasionally.
 */
        }
        OK = OK && random_digits(p, diglen_p, pBigCtx);
        p[diglen_p - 1] >>= (RADIX_BITS*diglen_p - bitlen_p);
        p[0] |= 1;
        mp_setbit(p, BITS_TO_DIGITS(bitlen_p), bitlen_p-1, 1);
        mp_setbit(p, BITS_TO_DIGITS(bitlen_p), bitlen_p-2, 1);

        /* Test for a divisor from the table of small primes */
        OK = OK && low_prime_divisibility(p, diglen_p, lowprods, clowprods, &gcdtest, pBigCtx);
        if (!OK)
        {
        }
        else if (gcdtest != 1)
        {
            pbpstat->trial_division_eliminations++;
        }
        else
        {
            DRM_DWORD irabin;
            DRM_BOOL reject_p = FALSE;
            OEM_SECURE_ZERO_MEMORY(&p_modulus, sizeof(p_modulus));  /* Avoid warnings */
            MEMSET(&p_modulus, 0, sizeof(p_modulus));  /* Avoid warnings */
            OK = OK && create_modulus(p, diglen_p, FROM_RIGHT, &p_modulus, pBigCtx, pBigCtx);
            modulus_created = OK;

/*
 *   p has no divisor from our tables.
 *   Do some Rabin tests.  Use base 2 for first test.
 */

            OK = OK && neg_mod(p_modulus.one, minus1, p_modulus.modulus, p_modulus.length);
            OK = OK && mp_shift(p, -1, exponent, diglen_p);

            /* exponent = (p - 1)/2 */
            for (irabin = 1;
                OK && irabin <= NUM_RABIN_TEST && !reject_p;
                irabin++)
            {
                if (irabin == 1)
                {  /* base = 1 + 1 = 2 */
                    OK = OK && add_mod(p_modulus.one, p_modulus.one,
                                       base, p_modulus.modulus, p_modulus.length);
                }
                else
                {
                    OK = OK && random_mod_nonzero(p, base, diglen_p, pBigCtx);
                }
                OK = OK && mod_exp(base, exponent, diglen_p,
                                   result, &p_modulus, pBigCtx);

                /* If p is prime, result of exponentiation will be +1 or -1 */
                if (!OK)
                {
                }
                else if (compare_same(result, minus1, diglen_p) == 0)
                {
                    hit_minus1 = TRUE;   /* Got a -1 */
                }
                else if (compare_same(result, p_modulus.one, diglen_p) != 0)
                {
                    reject_p = TRUE;     /* Neither -1 nor +1 */
                    pbpstat->rabin_eliminations++;
                }
            } /* for irabin */

            if (OK && !reject_p)
            {
                big_prime_found = hit_minus1;
                if (!hit_minus1)
                {
                    pbpstat->other_eliminations++;
                    TRACE( ( "Potential prime passed %d Rabin tests but never gave -1\n",
                           ( DRM_LONG )NUM_RABIN_TEST ) );
                }  /* Don't use it */
            }
            if (modulus_created)
            {
                uncreate_modulus(&p_modulus, pBigCtx);
            }
            modulus_created = FALSE;
        } /* Radin tests */
    }  /* while (!big_prime_found) */

    if (dtemps != NULL)
    {
        Free_Temporaries(dtemps, pBigCtx);
    }
    if (modulus_created)
    {
        uncreate_modulus(&p_modulus, pBigCtx);
    }
    return OK;
} /* end _big_prime_search */

/******************************************************************************
**
** Function :   _rsa_construction
**
** Synopsis :
**    Construct an RSA modulus for which the primes have lengths
**    bitlen_p1 and bitlen_p2.  Both primes will start with 11 (binary),
**    ensuring their product has a full bitlen_p1 + bitlen_p2 bits.
**    The public exponent will be 65537.
**
** Arguments :  [bitlen_p1]    -
**              [bitlen_p2]    -
**              [f_pbModulus]  - pointer to public key modulus
**              [f_pbPrivExp]  - pointer to private key exponent
**              [f_pbPrime0]   - pointer to first (greater) prime
**              [f_pbPrime1]   - pointer to second (smaller) prime
**              [f_pCryptoCtx] - Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
** Returns :    DRM_SUCCESS -
******************************************************************************/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_WRITABLE_BUFFER_25057, "f_pbPrime0 and f_pbPrime1 lengths defined by bitlen_p1 and bitlen_p2" )
static DRM_RESULT _rsa_construction(
    __in const                                                  DRM_DWORD           bitlen_p1,
    __in const                                                  DRM_DWORD           bitlen_p2,
    __out_ecount( DRM_RSA_CB_MODULUS( bitlen_p1 + bitlen_p2 ) ) DRM_BYTE           *f_pbModulus,
    __out_ecount( DRM_RSA_CB_PRIVEXP( bitlen_p1 + bitlen_p2 ) ) DRM_BYTE           *f_pbPrivExp,
    __out_ecount( DRM_RSA_CB_PRIME( bitlen_p1 + bitlen_p2 ) )   DRM_BYTE           *f_pbPrime0,
    __out_ecount( DRM_RSA_CB_PRIME( bitlen_p1 + bitlen_p2 ) )   DRM_BYTE           *f_pbPrime1,
    __inout                                                     DRM_CRYPTO_CONTEXT *f_pCryptoCtx )
PREFAST_POP
{
/*
      Construct an RSA modulus for which the primes have lengths
      bitlen_p1 and bitlen_p2.  Both primes will start with 11 (binary),
      ensuring their product has a full bitlen_p1 + bitlen_p2 bits.
      The public exponent will be 65537.
*/
    DRM_RESULT dr = DRM_SUCCESS;
    big_prime_search_stat_t stats = { 0 };
    big_prime_search_stat_t* pbpstat = &stats;
    const DRM_BYTE pubexp_bytes[] = {1, 0x00, 0x01}; /* Public expon 65537 = 0x101 */
    const DRM_DWORD bitlen_pubexp = 17;       /* Length of public exponent in bits */
    const DRM_DWORD bitlen_modulus = bitlen_p1 + bitlen_p2;
                                              /* No leading zero after multiply */

    const DRM_DWORD diglen_pubexp  = BITS_TO_DIGITS(bitlen_pubexp);
    const DRM_DWORD diglen_p1      = BITS_TO_DIGITS(bitlen_p1);
    const DRM_DWORD diglen_p2      = BITS_TO_DIGITS(bitlen_p2);
    const DRM_DWORD diglen_plonger = MAX(diglen_p1, diglen_p2);
    const DRM_DWORD diglen_pboth   = diglen_p1 + diglen_p2;
    const DRM_DWORD num_dtemps     = 5*diglen_plonger + 3*diglen_pboth + diglen_pubexp;

    digit_t *primes   = NULL;
    digit_t *privexp  = NULL;
    digit_t *modulus  = NULL;
    digit_t *pubexp   = NULL;
    digit_t *gcd      = NULL;
    digit_t *temp1    = NULL;
    digit_t *temp2    = NULL;
    digit_t *temp3    = NULL;

    DRM_DWORD lgcd = 0, nprime_found = 0;    /* Need 2 primes -- none found yet */
    digit_t *dtemps                  = NULL; /* Freed at end of procedure */
    digit_t *lowprods                = NULL; /* Freed at end of procedure */
    DRM_DWORD clowprods              = 0;
    struct bigctx_t *pBigCtx         = NULL;

    struct bigctx_t *pBigCtxPrimes            = NULL;
    ChkMem( pBigCtxPrimes = (struct bigctx_t*)Oem_MemAlloc( DRM_RSA_PKCRYPTO_LOW_PRIME_SIZE ) );
    DRM_RSA_PKInitSize( pBigCtxPrimes, DRM_RSA_PKCRYPTO_LOW_PRIME_SIZE );

    ChkArg( IS_VALID_DRM_RSA_KEY_LENGTH_ALLBIT( bitlen_p1 + bitlen_p2 ) );
    ChkArg( NULL != f_pbModulus );
    ChkArg( NULL != f_pbPrivExp );
    ChkArg( NULL != f_pbPrime0 );
    ChkArg( NULL != f_pbPrime1 );
    ChkArg( NULL != f_pCryptoCtx );
    ChkBOOL( bitlen_p1 == bitlen_p2, DRM_E_NOTIMPL ); /* limitation of current implementation */

    pBigCtx = (struct bigctx_t *)f_pCryptoCtx->rgbCryptoContext;
    DRM_RSA_PKInitSize( pBigCtx, SIZEOF(f_pCryptoCtx->rgbCryptoContext) );

    ChkArg( bitlen_pubexp > 1
         && IS_ODD(pubexp_bytes[0]) );

    ChkMem( dtemps = digit_allocate(num_dtemps, pBigCtx) );
    ChkMem( lowprods = low_prime_prod_construction(pBigCtxPrimes, &clowprods) );

    primes   = dtemps;                     /* Lengths diglen_p1 and diglen_p2 */
    privexp  = dtemps  + diglen_pboth;     /* Lengths diglen_p1 and diglen_p2 */
    modulus  = privexp + diglen_pboth;     /* Length  2*diglen_plonger */
    pubexp   = modulus + 2*diglen_plonger; /* Length  diglen_pubexp */
    gcd      = pubexp  + diglen_pubexp;    /* Length  diglen_plonger */
    temp1    = gcd     + diglen_plonger;   /* Length  diglen_plonger */
    temp2    = temp1   + diglen_plonger;   /* Length  diglen_plonger */
    temp3    = temp2   + diglen_plonger;   /* Length  diglen_pboth */

    {
        DRMASSERT(dtemps + num_dtemps == temp3 + diglen_pboth);

        ChkDR( _BigEndianBytesToDigits(pubexp_bytes,
                                       bitlen_pubexp, pubexp));
        OEM_SECURE_ZERO_MEMORY(pbpstat, sizeof(big_prime_search_stat_t)); // Clear statistics
        while (nprime_found != 2)
        {
            const DRM_DWORD bitlen_pnow = /*bitlen_p[nprime_found];*/ (nprime_found==0)?bitlen_p1:bitlen_p2;
            const DRM_DWORD poffset = diglen_p1*nprime_found;  /* Offset in primes array */
            digit_t *pnow = primes + poffset;            /* Location of latest prime */

            ChkBOOL( _big_prime_search(bitlen_pnow, pnow, lowprods, clowprods, pbpstat, f_pCryptoCtx), DRM_E_FAIL );

            if( nprime_found == 1
             && compare_diff(primes            , diglen_p1,
                             primes + diglen_p1, diglen_p2) == 0)
            {
                pbpstat->other_eliminations++;
            }
            else
            {
                pbpstat->primes_found++;  /* same as nprime_found */
                nprime_found++;
            }
        } /* while */

        multiply(primes, diglen_p1, primes + diglen_p1, diglen_p2, modulus);

        sub_immediate(primes,             1, temp1, diglen_p1);    /* temp1 = p - 1 */
        sub_immediate(primes + diglen_p1, 1, temp2, diglen_p2);    /* temp2 = q - 1 */

        multiply(temp1, diglen_p1, temp2, diglen_p2, temp3);

        ChkBOOL( mp_gcdex( pubexp,
                           diglen_pubexp,
                           temp3,
                           diglen_pboth,
                           privexp,
                           NULL,
                           gcd,
                           NULL,
                          &lgcd,
                           NULL,
                           pBigCtx), DRM_E_FAIL );

        if( compare_immediate(gcd, 1, lgcd) != 0 )
        {
            /* Cannot solve privexp*pubexp == 1 (mod p-1) */
            ChkDR( DRM_E_FAIL );
        }

        if( mp_significant_bit_count(modulus, diglen_pboth ) != bitlen_modulus )
        {
            SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                             "_rsa_construction -- wrong bit count" );

            ChkDR( DRM_E_FAIL );
        }

        /* Limitation of current implementation */
        DRMASSERT( bitlen_p1 == bitlen_p2 && bitlen_p1%BITS_PER_STANDARD_BYTE == 0);
        /* OACR validations */
        ChkOverflow( f_pbPrime0 + DRM_RSA_CB_PRIME( bitlen_modulus ) / 2, f_pbPrime0 );
        ChkOverflow( f_pbPrime1 + DRM_RSA_CB_PRIME( bitlen_modulus ) / 2, f_pbPrime1 );
        DRMASSERT( BITS_TO_BYTES( bitlen_modulus ) - DRM_RSA_CB_PRIME( bitlen_p2 ) >= BITS_TO_BYTES( bitlen_p1 ) );
        DRMASSERT( BITS_TO_BYTES( bitlen_modulus ) - DRM_RSA_CB_PRIME( bitlen_p1 ) >= BITS_TO_BYTES( bitlen_p2 ) );

        ChkDR( _DigitsToBigEndianBytes(modulus, bitlen_modulus,      f_pbModulus));
        ChkDR( _DigitsToBigEndianBytes(privexp, bitlen_p1+bitlen_p2, f_pbPrivExp));

        OEM_SECURE_ZERO_MEMORY( f_pbPrime0, DRM_RSA_CB_PRIME( bitlen_modulus ) );
        OEM_SECURE_ZERO_MEMORY( f_pbPrime1, DRM_RSA_CB_PRIME( bitlen_modulus ) );

        ChkBOOL( BITS_TO_BYTES( bitlen_p1 + bitlen_p2 ) >= DRM_RSA_CB_PRIME( bitlen_p2 ), DRM_E_BUFFERTOOSMALL );

        ChkDR( _DigitsToBigEndianBytes(primes                    ,   bitlen_p1, f_pbPrime0 + DRM_RSA_CB_PRIME( bitlen_p2 )));

        ChkBOOL( BITS_TO_BYTES( bitlen_p1 + bitlen_p2 ) >= DRM_RSA_CB_PRIME( bitlen_p1 ), DRM_E_BUFFERTOOSMALL );

        ChkDR( _DigitsToBigEndianBytes(primes + ( 1 * diglen_p1 ),   bitlen_p2, f_pbPrime1 + DRM_RSA_CB_PRIME( bitlen_p1 ) ));
    }

ErrorExit:
    if (dtemps != NULL)
    {
        Free_Temporaries(dtemps, pBigCtx);
    }
    if (lowprods != NULL)
    {
        low_prime_prod_destruction(lowprods, pBigCtxPrimes);
    }
    SAFE_OEM_FREE( pBigCtxPrimes );

    return dr;
}

/**********************************************************************
**
**  Function: OEM_RSA_GenerateKeyPair_2048BIT
**
**  Synopsis: Generates RSA public/private key pair
**
**  Arguments:
**     [f_eKeyLength]  : RSA key length in bits
**     [f_pPrivateKey] : Pointer to RSA private key structure to be filled with data on return.
**                       The caller should allocate memory for it before calling this API.
**     [f_pPublicKey]  : Pointer to RSA public key structure to be filled with data on return.
**                       The caller should allocate memory for it before calling this API.
**     [f_pCryptoCtx]  : Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL or out of range
**      DRM_E_OEM_RSA_DECRYPTION_ERROR
**          An error occurred during encryption
**
**  Notes:
**      Both data buffers and key elements are in big endian byte format.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL OEM_RSA_GenerateKeyPair_2048BIT(
    __in const      DRM_RSA_SUPPORTED_KEY_LENGTHS_2048BIT  f_eKeyLength,
    __out_ecount(1) DRM_RSA_PRIVATE_KEY_2048BIT           *f_pPrivateKey,
    __out_ecount(1) DRM_RSA_PUBLIC_KEY_2048BIT            *f_pPublicKey,
    __inout         DRM_CRYPTO_CONTEXT                    *f_pCryptoCtx
    )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE   rgbModulus  [__CB_DECL( DRM_RSA_CB_MODULUS_MAX_2048BIT )];
    DRM_BYTE   rgbPrivExp  [__CB_DECL( DRM_RSA_CB_PRIVEXP_MAX_2048BIT )];
    DRM_BYTE   rgbPrimes[2][__CB_DECL( DRM_RSA_CB_PRIME_MAX_2048BIT )];
    DRM_DWORD  nbit = f_eKeyLength;

    ChkArg( IS_VALID_DRM_RSA_KEY_LENGTH_2048BIT( f_eKeyLength ) );
    ChkArg( NULL != f_pPrivateKey );
    ChkArg( NULL != f_pPublicKey );
    ChkArg( NULL != f_pCryptoCtx );

    ChkDR( _rsa_construction( nbit / 2,
                             nbit / 2,
                             rgbModulus,
                             rgbPrivExp,
                             rgbPrimes[ 0 ],
                             rgbPrimes[ 1 ],
                             f_pCryptoCtx) );

    ChkDR( OEM_RSA_SetPublicKey_2048BIT( f_eKeyLength,
                                         sizeof( rgbDRM_RSA_DEFAULT_PUBEXP ),
                                         rgbDRM_RSA_DEFAULT_PUBEXP,
                                         DRM_RSA_CB_MODULUS( f_eKeyLength ),
                                         rgbModulus,
                                         f_pPublicKey ) );

    ChkDR( OEM_RSA_SetPrivateKey_2048BIT( f_eKeyLength,
                                          DRM_RSA_CB_PRIVEXP( f_eKeyLength ),
                                          rgbPrivExp,
                                          DRM_RSA_CB_PRIME( f_eKeyLength ),
                                          rgbPrimes[ 0 ],
                                          rgbPrimes[ 1 ],
                                          f_pPrivateKey,
                                          f_pCryptoCtx ) );

ErrorExit:
    if( f_pCryptoCtx != NULL )
    {
        MEMSET( f_pCryptoCtx, 0, sizeof( DRM_CRYPTO_CONTEXT ) );
    }
    return dr;
}

/**********************************************************************
**
**  Function: OEM_RSA_GenerateKeyPair_4096BIT
**
**  Synopsis: Generates RSA public/private key pair
**
**  Arguments:
**     [f_eKeyLength]  : RSA key length in bits
**     [f_pPrivateKey] : Pointer to RSA private key structure to be filled with data on return.
**                       The caller should allocate memory for it before calling this API.
**     [f_pPublicKey]  : Pointer to RSA public key structure to be filled with data on return.
**                       The caller should allocate memory for it before calling this API.
**     [f_pCryptoCtx]  : Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL or out of range
**      DRM_E_OEM_RSA_DECRYPTION_ERROR
**          An error occurred during encryption
**
**  Notes:
**      Both data buffers and key elements are in big endian byte format.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL OEM_RSA_GenerateKeyPair_4096BIT(
    __in const      DRM_RSA_SUPPORTED_KEY_LENGTHS_4096BIT  f_eKeyLength,
    __out_ecount(1) DRM_RSA_PRIVATE_KEY_4096BIT           *f_pPrivateKey,
    __out_ecount(1) DRM_RSA_PUBLIC_KEY_4096BIT            *f_pPublicKey,
    __inout         DRM_CRYPTO_CONTEXT                    *f_pCryptoCtx
    )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE   rgbModulus  [__CB_DECL( DRM_RSA_CB_MODULUS_MAX_4096BIT )];
    DRM_BYTE   rgbPrivExp  [__CB_DECL( DRM_RSA_CB_PRIVEXP_MAX_4096BIT )];
    DRM_BYTE   rgbPrimes[2][__CB_DECL( DRM_RSA_CB_PRIME_MAX_4096BIT )];
    DRM_DWORD  nbit = f_eKeyLength;

    ChkArg( IS_VALID_DRM_RSA_KEY_LENGTH_4096BIT( f_eKeyLength ) );
    ChkArg( NULL != f_pPrivateKey );
    ChkArg( NULL != f_pPublicKey );
    ChkArg( NULL != f_pCryptoCtx );

    ChkDR( _rsa_construction( nbit / 2,
                             nbit / 2,
                             rgbModulus,
                             rgbPrivExp,
                             rgbPrimes[ 0 ],
                             rgbPrimes[ 1 ],
                             f_pCryptoCtx) );

    ChkDR( OEM_RSA_SetPublicKey_4096BIT( f_eKeyLength,
                                         sizeof( rgbDRM_RSA_DEFAULT_PUBEXP ),
                                         rgbDRM_RSA_DEFAULT_PUBEXP,
                                         DRM_RSA_CB_MODULUS( f_eKeyLength ),
                                         rgbModulus,
                                         f_pPublicKey ) );

    ChkDR( OEM_RSA_SetPrivateKey_4096BIT( f_eKeyLength,
                                          DRM_RSA_CB_PRIVEXP( f_eKeyLength ),
                                          rgbPrivExp,
                                          DRM_RSA_CB_PRIME( f_eKeyLength ),
                                          rgbPrimes[ 0 ],
                                          rgbPrimes[ 1 ],
                                          f_pPrivateKey,
                                          f_pCryptoCtx ) );

ErrorExit:
    if( f_pCryptoCtx != NULL )
    {
        MEMSET( f_pCryptoCtx, 0, sizeof( DRM_CRYPTO_CONTEXT ) );
    }
    return dr;
}


EXIT_PK_NAMESPACE_CODE;
