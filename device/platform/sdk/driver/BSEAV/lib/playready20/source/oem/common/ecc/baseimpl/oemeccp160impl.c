/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmprofile.h>
#include <oempkcrypto.h>
#include <oemsha1.h>
#include <oem.h>
#include <oemeccp160impl.h>

ENTER_PK_NAMESPACE_CODE;

static const DRM_DWORD ecdw_q[5] =
{
    0x141424F7, 0x31415926, 0x27182818, 0x01234567, 0x89ABCDEF
};
/* ecdw_q = 785963102379428822376694789446897396207498568951 */

static const DRM_DWORD ecdw_order[5] =
{
    0x28C2A675, 0xEC149044, 0x2716B26E, 0x01234567, 0x89ABCDEF
};
/* ecdw_order = 785963102379428822376693024881714957612686157429 */

static const DRM_DWORD ecdw_a[5] =
{
    0xEBE41497, 0x4780C009, 0x7632FF3D, 0xD277BCE8, 0x37A5ABCC
};
/* ecdw_a = 317689081251325503476317476413827693272746955927 */

static const DRM_DWORD ecdw_b[5] =
{
    0x9328239E, 0xD78FDEDF, 0x28E85F1A, 0x725E2F32, 0x0DD8DABF
};
/* ecdw_b = 79052896607878758718120572025718535432100651934 */

static const DRM_DWORD ecdw_genx[5] =
{
    0x109FA120, 0xBA38DAF0, 0x3510C07D, 0xD6A3A1E5, 0x8723947F
};
/* ecdw_genx = 771507216262649826170648268565579889907769254176 */

static const DRM_DWORD ecdw_geny[5] =
{
    0xA379936F, 0xD4ED7ACD, 0x8C3C5856, 0x1075522D, 0x44574491
};
/* ecdw_geny = 390157510246556628525279459266514995562533196655 */


static DRM_VOID endian_reverse_dwords( const DRM_DWORD f_rgdwIn[], DRM_DWORD f_rgdwOut[], DRM_DWORD cdw )
{
    if( f_rgdwOut != f_rgdwIn )
    {
        OEM_SECURE_DWORDCPY( f_rgdwOut, f_rgdwIn, cdw );
    }

#if !TARGET_LITTLE_ENDIAN
    while( cdw>0 )
    {
        DRM_DWORD dw;

        cdw--;
        BYTES_TO_DWORD( dw, (DRM_BYTE*) &f_rgdwOut[cdw] );
        FIX_ENDIAN_DWORD( dw );
        DWORD_TO_BYTES( (DRM_BYTE*) &f_rgdwOut[cdw], dw );
    }
#endif

}

static DRM_NO_INLINE DRM_BOOL dw_to_modular(
    const  DRM_DWORD        *dwnum,
    const  DRM_DWORD         lngdw,
           digit_t          *answer,      /* OUT, length LNGQ */
    const  mp_modulus_t     *modulo,
    struct bigctx_t         *f_pBigCtx    ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_BOOL dw_to_modular(
    const  DRM_DWORD        *dwnum,
    const  DRM_DWORD         lngdw,
           digit_t          *answer,      /* OUT, length LNGQ */
    const  mp_modulus_t     *modulo,
    struct bigctx_t         *f_pBigCtx    )
{
    CLAW_AUTO_RANDOM_CIPHER
    digit_t temp1[LNGQ];
    DRM_BOOL OK = FALSE;

    if( temp1 != NULL )
    {

        if (DWORDS_TO_DIGITS(lngdw) <= LNGQ)
        {
            dwords_to_digits(dwnum, temp1, lngdw );
            OK = to_modular(temp1, DWORDS_TO_DIGITS(lngdw), answer, modulo, f_pBigCtx);
        }

        OEM_SECURE_ZERO_MEMORY( temp1, SIZEOF (digit_t) * LNGQ );
    }

    return OK;
} /* end dw_to_modular */


DRM_NO_INLINE DRM_BOOL words_to_ecaffine(
    const DRM_BYTE *in, digit_t *P,        /* OUT */
    const ecurve_t  *E,
    struct bigctx_t *f_pBigCtx )
/* map bits of "in" to a point on the curve such that the first n-1 bytes can be recovered

  NOTE: this is only supposed to work for my ec over FIELD_Q_MP and not intended to be
        used as a general purpose library function.    marcuspe

 NOTE -- We never return the point at infinity,
         so this may fail if the group has no other points,
         such as y^2 == x^3 - x - 1 (mod 3)
 NOTE -- A given point of order 2 (with y = 0) is twice as
         likely to be chosen as a given point of order > 2.
*/
{
    DRM_BOOL found = FALSE;
    DRM_LONG tries;
    flongest_t *t1;
    DRM_DWORD *tmp; /*[LNGQDW] */

    tmp = (DRM_DWORD*) bignum_alloc (SIZEOF (DRM_DWORD) * LNGQDW + SIZEOF (flongest_t), f_pBigCtx);

    if( tmp == NULL )
    {
        return found;
    }

    t1 = (flongest_t*)(tmp + LNGQDW);
    OEM_SECURE_DIGITTCPY( tmp, in,( LNGQDW-1 ));
    switch(E->fdesc->ftype) {
    case FIELD_Q_MP:
    case FIELD_2_POLYNOMIAL:
    case FIELD_2_NORMAL:
        for (tries = 0;
            tries < 100 && !found;
            tries++) {

                tmp[LNGQDW-1] = tries;
                if( !dw_to_modular( tmp, LNGQDW, P, E->fdesc->modulo, f_pBigCtx) )
                {
                    found = FALSE;
                    break;
                }
                if (E->fdesc->ftype == FIELD_Q_MP || Kiszero(P, E->fdesc, f_pBigCtx) )
                {
                    if (Kmuladd (P, P,   E->a, *t1, E->fdesc, NULL, f_pBigCtx)
                    &&  Kmuladd (P, *t1, E->b, *t1, E->fdesc, NULL, f_pBigCtx))
                    /*      t1 = x^2 + a                      t1 = x^3 + ax + b */
                    {
                        /* fSquare must be volatile to work in optimized builds of some compilers */
                        volatile DRM_BOOL fSquare = FALSE;
                        found = Kprime_sqrter(*t1, (P + E->fdesc->elng), 1, E->fdesc, ( DRM_BOOL* )&fSquare, f_pBigCtx) && fSquare;/* Is x^3 + ax + b  a square? */
                        continue;
                    }
                    found = FALSE;
                    break;
                }
                else
                {
                    found = FALSE;
                    break;
                }
            } /* for tries */
            break;
    default:
        break;
    } /* switch */

    OEM_SECURE_ZERO_MEMORY( tmp, SIZEOF (DRM_DWORD) * LNGQDW + SIZEOF (flongest_t) );
    bignum_free (tmp, f_pBigCtx);
    return found;
} /* words_to_ecaffine */


static DRM_BOOL ecaffine_to_dwords( const digit_t *P, DRM_DWORD *out, const ecurve_t  *E)
{
    /* inverts the mapping of dwords_to_ecaffine */
    DRM_DWORD tmp[LNGQDW + (SIZEOF( digit_t ) * LNGQ)/SIZEOF( DRM_DWORD )]; /*[LNGQDW]; */
    digit_t *digs; /*[LNGQ]; */

    if ( E != NULL &&
         E->fdesc != NULL &&
         E->fdesc->modulo != NULL &&
         E->fdesc->modulo->length == LNGQ &&
         tmp != NULL )
    {
        digs = (digit_t*)(tmp + LNGQDW );
        from_modular ( P, digs, E->fdesc->modulo);
        digits_to_dwords( digs, (DRM_BYTE *)tmp, LNGQDW );
        endian_reverse_dwords( (const DRM_DWORD *)tmp, out, LNGQDW-1 );

        OEM_SECURE_ZERO_MEMORY( tmp, SIZEOF(DRM_DWORD) * LNGQDW + SIZEOF( digit_t ) * LNGQ );
        return( TRUE );
    }

    return FALSE;
}

#if defined( _PREFAST_ )
#pragma prefast(suppress:22112) /* security mitigation warning */
#endif

#if DRM_THREAD_SAFE
static DRM_VOID _PKLock();
static DRM_VOID _PKUnlock();
#define USEPKLOCK DRM_BOOL fLocked = FALSE;
#define PKLOCK {_PKLock();fLocked=TRUE;}
#define PKUNLOCK {if(fLocked){_PKUnlock();}}
#else
#define USEPKLOCK
#define PKLOCK
#define PKUNLOCK
#endif

static DRM_NO_INLINE DRM_RESULT ec_init(
    PK_DATA         *pEc,
    const DRM_DWORD *q,
    const DRM_DWORD *order,
    const DRM_DWORD *a,
    const DRM_DWORD *b,
    const DRM_DWORD *genx,
    const DRM_DWORD *geny,
    struct bigctx_t *f_pBigCtx)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( pEc       != NULL
         && q         != NULL
         && order     != NULL
         && a         != NULL
         && b         != NULL
         && genx      != NULL
         && geny      != NULL
         && f_pBigCtx != NULL );

    dwords_to_digits( q, pEc->q, LNGQDW );
    dwords_to_digits( order, pEc->r, LNGQDW );

    ChkBOOL( create_modulus( pEc->q, LNGQ, FROM_RIGHT, &pEc->qmodulus, f_pBigCtx, (struct bigctx_t*)&pEc->pkdHeap ), DRM_E_PKCRYPTO_FAILURE);
    ChkBOOL( dw_to_modular( a, LNGQDW, pEc->a, &pEc->qmodulus, f_pBigCtx ), DRM_E_PKCRYPTO_FAILURE);
    ChkBOOL( dw_to_modular( b, LNGQDW, pEc->b, &pEc->qmodulus, f_pBigCtx ), DRM_E_PKCRYPTO_FAILURE);
    ChkBOOL( Kinitialize_prime( &pEc->qmodulus, &pEc->fdesc, f_pBigCtx , (struct bigctx_t*)&pEc->pkdHeap ), DRM_E_PKCRYPTO_FAILURE);
    ChkBOOL( ec_initialize( pEc->a, pEc->b, &pEc->fdesc, &pEc->ecurve, f_pBigCtx, (struct bigctx_t*)&pEc->pkdHeap ), DRM_E_PKCRYPTO_FAILURE);
    ChkBOOL( dw_to_modular( ecdw_genx, LNGQDW, pEc->g, &pEc->qmodulus, f_pBigCtx ), DRM_E_PKCRYPTO_FAILURE);
    ChkBOOL( dw_to_modular( ecdw_geny, LNGQDW, pEc->g + LNGQ, &pEc->qmodulus, f_pBigCtx ), DRM_E_PKCRYPTO_FAILURE);
    ChkBOOL( ecaffine_on_curve(pEc->g, &pEc->ecurve, NULL, NULL, f_pBigCtx), DRM_E_PKCRYPTO_FAILURE);
    ChkBOOL( ecaffine_table_construction( pEc->g, TABLE_SPACING, TABLE_LAST, pEc->TABLE, &pEc->ecurve, f_pBigCtx ), DRM_E_PKCRYPTO_FAILURE);

    pEc->lngr = LNGQ;
    divide_precondition_1( pEc->r, pEc->lngr, &pEc->rrecip );

ErrorExit:
    return dr;
}

DRM_API DRM_VOID DRM_CALL OEM_ECC_ZeroPublicKey_P160Impl(
    __inout        PUBKEY     *f_pPubkey )
{
    if( NULL != f_pPubkey )
    {
        OEM_SECURE_ZERO_MEMORY( f_pPubkey, SIZEOF( *f_pPubkey ) );
    }
}

DRM_API DRM_VOID DRM_CALL OEM_ECC_ZeroPrivateKey_P160Impl(
    __inout        PRIVKEY    *f_pPrivkey )
{
    if( NULL != f_pPrivkey )
    {
        OEM_SECURE_ZERO_MEMORY( f_pPrivkey, SIZEOF( *f_pPrivkey ) );
    }
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_ECC_InitializeBignumStackImpl( DRM_VOID* f_pContext ) DRM_NO_INLINE_ATTRIBUTE;
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_ECC_InitializeBignumStackImpl( DRM_VOID* f_pContext )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    DRMBIGNUM_CONTEXT_STRUCT *pContext = (DRMBIGNUM_CONTEXT_STRUCT*) f_pContext;
    DRMCASSERT( SIZEOF( DRMBIGNUM_CONTEXT_STRUCT ) <= DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE );
    ChkArg( f_pContext != NULL );


    pContext->oHeap.nStackTop = 0;
    pContext->oHeap.cbStack = DRM_PKCRYPTO_CONTEXT_BUFFER_INTERNAL_SIZE; /* This is the size of rgbHeap */
    pContext->oHeap.pbStack = pContext->rgbHeap;
ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_ECC_ThreadUnSafePKInitImpl(
    DRM_VOID* f_pContext,
    DRM_VOID* f_pOEMContext )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_E_PKCRYPTO_FAILURE;
    DRMBIGNUM_CONTEXT_STRUCT *pContext = (DRMBIGNUM_CONTEXT_STRUCT*) f_pContext;

    ChkDR( OEM_ECC_InitializeBignumStackImpl( pContext ) ); /* we have to initialize the bignum context since ec_init needs it */

    if( pContext->fInited )
    {
        return DRM_SUCCESS;
    }

    pContext->pOEMContext = f_pOEMContext;

    MEMSET( (DRM_BYTE*)&pContext->pkData, 0, SIZEOF( pContext->pkData ));

    pContext->pkData.pkdHeap.nStackTop = 0;
    pContext->pkData.pkdHeap.cbStack = PK_DATA_HEAP_SIZE;
    pContext->pkData.pkdHeap.pbStack = pContext->pkData.rgbHeap;

    dr = ec_init(&pContext->pkData, ecdw_q, ecdw_order, ecdw_a, ecdw_b, ecdw_genx, ecdw_geny, (struct bigctx_t*)&pContext->oHeap);
    if( DRM_SUCCEEDED( dr ) )
    {
        pContext->fInited = TRUE;
    }

ErrorExit:
    return dr;
}

DRM_API DRM_VOID DRM_CALL DRM_RSA_PKInitSizeImpl(
    IN OUT DRM_VOID *f_pContext,
    IN     DRM_DWORD f_dwSize )
{
    DRMBIGNUM_CONTEXT_STRUCT *pContext = (DRMBIGNUM_CONTEXT_STRUCT*) f_pContext;

    DRMASSERT( f_dwSize >= SIZEOF(DRM_STACK_ALLOCATOR_CONTEXT) );

    f_dwSize -= SIZEOF( DRM_STACK_ALLOCATOR_CONTEXT );

    /* init stack */
    pContext->oHeap.nStackTop = 0;
    pContext->oHeap.cbStack = f_dwSize;
    pContext->oHeap.pbStack = pContext->rgbHeap;
}

/*
    Note: the following operations are needed since current design allows OEM_ECC_PKInit to be thread safe.
    If it is thread safe then we must assume that between OEM_ECC_PKinit and the pklock; the bignum context
    can be run over. the locking should be thought over. eccp256 does not have any locking mechanism on context.
*/

/*********************************************************************
**
**  Function:  OEM_ECC_GenKeyPair_P160Impl
**
**  Synopsis:  Generate a matched public and private key pair
**
**  Arguments:
**     [f_pContext] -- Pointer to context the size of DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE
**     [f_ppubkey]  -- New public key
**     [f_pprivkey] -- New private key
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL OEM_ECC_GenKeyPair_P160Impl(
    IN     DRM_VOID *f_pContext,
       OUT PUBKEY   *f_ppubkey,
       OUT PRIVKEY  *f_pprivkey )
{
    DRM_RESULT dr = DRM_E_PKCRYPTO_FAILURE;
    DRMBIGNUM_CONTEXT_STRUCT *pContext = (DRMBIGNUM_CONTEXT_STRUCT *)f_pContext;
    struct bigctx_t *pBigCtx                                                            = NULL;
    digit_t    tmp[2 * LNGQ + ( SIZEOF( digit_t ) * 2 *LNGQ )/SIZEOF(digit_t)]          = {0}; /* [LNGQ] */
    digit_t   *priv                                                                     = NULL; /* [LNGQ] */
    digit_t   *pub                                                                      = NULL; /* [2*LNGQ] */

    USEPKLOCK;

    ChkArg( f_pContext != NULL
         && f_ppubkey  != NULL
         && f_pprivkey != NULL );

    DRMASSERT( (DRM_DWORD_PTR)f_ppubkey % SIZEOF( DRM_DWORD ) == 0 );
    DRMASSERT( (DRM_DWORD_PTR)f_pprivkey % SIZEOF( DRM_DWORD ) == 0 );

    ChkDR( OEM_ECC_PKInit( pContext, NULL ) );


    PKLOCK;

    ChkDR( OEM_ECC_InitializeBignumStackImpl( pContext ) );

    pBigCtx = (struct bigctx_t *) &pContext->oHeap;

    priv = (digit_t*)(tmp  + LNGQ);
    pub  = (digit_t*)(priv + LNGQ);

    random_mod_nonzero( pContext->pkData.q, priv, LNGQ, pBigCtx );
    if( ecaffine_exponentiation_tabular( pContext->pkData.TABLE, TABLE_SPACING, TABLE_LAST, priv, LNGQ, pub, &(pContext->pkData.ecurve), pBigCtx ) )
    {

        digits_to_dwords( priv, (DRM_BYTE *)f_pprivkey->x, LNGQDW );
        endian_reverse_dwords( (DRM_DWORD*)f_pprivkey->x, (DRM_DWORD*)f_pprivkey->x, DRM_ECC160_PRIVATE_KEY_LEN / SIZEOF( DRM_DWORD ) );

        from_modular ( pub, tmp, &(pContext->pkData.qmodulus) );
        digits_to_dwords( tmp, (DRM_BYTE *)f_ppubkey->y, LNGQDW );

        from_modular ( pub+LNGQ, tmp, &(pContext->pkData.qmodulus) );
        digits_to_dwords( tmp, (DRM_BYTE *)(((DRM_DWORD*)f_ppubkey->y) + LNGQDW), LNGQDW );
        endian_reverse_dwords( (DRM_DWORD*)f_ppubkey->y, (DRM_DWORD *)f_ppubkey->y, DRM_ECC160_PUBLIC_KEY_LEN / SIZEOF( DRM_DWORD ) );

        dr = DRM_SUCCESS;
    }

    OEM_SECURE_ZERO_MEMORY( tmp, (SIZEOF ( digit_t ) * 2 * LNGQ) + ( SIZEOF( digit_t ) * 2 *LNGQ ) );

ErrorExit:
    PKUNLOCK;

    return dr;
}


/*********************************************************************
**
**  Function:  OEM_ECC_Encrypt_P160Impl
**
**  Synopsis:  Encrypt a block of data with a given public key.
**
**  Arguments:
**     [f_pContext] -- Pointer to context the size of DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE
**     [f_ppubkey]  -- Public key to encrypt with
**     [f_rgbIn]    -- Data to be encrypted
**     [f_rgbOut]   -- Buffer to hold the encrypted data value.
**
**  Notes:  Data is not encrpyted in place.  It is put in the rgbOut buffer.
*********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_ECC_Encrypt_P160Impl(
    IN        DRM_VOID *f_pContext,
    IN  const PUBKEY   *f_ppubkey,
    __in_bcount( DRM_ECC160_PLAINTEXT_LEN ) const DRM_BYTE  f_rgbIn[__CB_DECL(DRM_ECC160_PLAINTEXT_LEN)],
    __out_bcount( DRM_ECC160_CIPHERTEXT_LEN ) DRM_BYTE  f_rgbOut[__CB_DECL(DRM_ECC160_CIPHERTEXT_LEN)] )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRMBIGNUM_CONTEXT_STRUCT *pContext = (DRMBIGNUM_CONTEXT_STRUCT *)f_pContext;
    DRM_DWORD rgdwPubKey[ DRM_ECC160_PUBLIC_KEY_LEN / SIZEOF( DRM_DWORD )  ];
    digit_t    k[LNGQ + ( SIZEOF(digit_t) * 5*2*LNGQ )/SIZEOF(digit_t)]; /* [LNGQ]   */
    struct bigctx_t *pBigCtx = NULL;
    digit_t *a   = NULL; /* [2*LNGQ] */
    digit_t *b   = NULL; /* [2*LNGQ] */
    digit_t *tmp = NULL; /* [2*LNGQ] */
    digit_t *ecM = NULL; /* [2*LNGQ] */
    digit_t *pk  = NULL; /* [2*LNGQ] */
    USEPKLOCK;

    ChkArg( f_pContext    != NULL
         && f_ppubkey     != NULL
         && f_rgbIn       != NULL
         && f_rgbOut      != NULL );

    DRMASSERT( (DRM_DWORD_PTR)f_ppubkey % SIZEOF( DRM_DWORD ) == 0 );

    ChkDR( OEM_ECC_PKInit( pContext, NULL ) );

    PKLOCK;

    ChkDR( OEM_ECC_InitializeBignumStackImpl( pContext ) );

    pBigCtx = (struct bigctx_t *) &pContext->oHeap;

    a   = (digit_t*)(k   + LNGQ);
    b   = (digit_t*)(a   + 2 * LNGQ);
    tmp = (digit_t*)(b   + 2 * LNGQ);
    ecM = (digit_t*)(tmp + 2 * LNGQ);
    pk  = (digit_t*)(ecM + 2 * LNGQ);

    endian_reverse_dwords( (const DRM_DWORD*)f_ppubkey->y, rgdwPubKey, DRM_ECC160_PUBLIC_KEY_LEN / SIZEOF( DRM_DWORD ) );

    ChkArg( pContext->pkData.qmodulus.length == LNGQ );

    dw_to_modular( rgdwPubKey,          LNGQDW, pk,      &(pContext->pkData.qmodulus), pBigCtx );
    dw_to_modular( rgdwPubKey + LNGQDW, LNGQDW, pk+LNGQ, &(pContext->pkData.qmodulus), pBigCtx );

    if( !ecaffine_on_curve(pk, &(pContext->pkData.ecurve), NULL, NULL, pBigCtx ) )
    {
        ChkDR( DRM_E_PKCRYPTO_FAILURE );
    }

    /* Do a byte copy from the input buffer to the output buffer since
    ** only the output buffer is guaranteed to be DWORD aligned
    */
    OEM_SECURE_MEMCPY( f_rgbOut, f_rgbIn, DRM_ECC160_PLAINTEXT_LEN );

    endian_reverse_dwords( (const DRM_DWORD*)f_rgbOut, (DRM_DWORD*)f_rgbOut, DRM_ECC160_PLAINTEXT_LEN / SIZEOF(DRM_DWORD) );
    if( !words_to_ecaffine( f_rgbOut, ecM, &(pContext->pkData.ecurve), pBigCtx ) )
    {
        ChkDR( DRM_E_PKCRYPTO_FAILURE );
    }

    random_mod_nonzero( pContext->pkData.q, k, LNGQ, pBigCtx );

    if( !ecaffine_exponentiation_tabular( pContext->pkData.TABLE, TABLE_SPACING, TABLE_LAST, k, LNGQ, a, &(pContext->pkData.ecurve), pBigCtx ) )
    {
        ChkDR( DRM_E_PKCRYPTO_FAILURE );
    }

    if( !ecaffine_on_curve( pk, &pContext->pkData.ecurve, NULL, NULL, pBigCtx ) )
    {
        ChkDR( DRM_E_PKCRYPTO_FAILURE );
    }

    if( !ecaffine_exponentiation_tabular( pk, TABLE_SPACING, 0, k, LNGQ, tmp, &(pContext->pkData.ecurve), pBigCtx ) )
    {
        ChkDR( DRM_E_PKCRYPTO_FAILURE );
    }

    if( !ecaffine_addition( tmp, ecM, b, 1, &(pContext->pkData.ecurve), NULL, pBigCtx  ) )
    {
        ChkDR( DRM_E_PKCRYPTO_FAILURE );
    }


    from_modular ( a, tmp, &(pContext->pkData.qmodulus) );
    digits_to_dwords( tmp, (DRM_BYTE *)f_rgbOut, LNGQDW );
    from_modular ( a+LNGQ, tmp, &pContext->pkData.qmodulus );
    digits_to_dwords( tmp, (DRM_BYTE *)(((DRM_DWORD *)f_rgbOut)+LNGQDW), LNGQDW );

    from_modular ( b, tmp, &pContext->pkData.qmodulus );
    digits_to_dwords( tmp, (DRM_BYTE *)(((DRM_DWORD *)f_rgbOut) + 2*LNGQDW), LNGQDW );
    from_modular ( b+LNGQ, tmp, &pContext->pkData.qmodulus );
    digits_to_dwords( tmp, (DRM_BYTE *)(((DRM_DWORD *)f_rgbOut) + 3*LNGQDW), LNGQDW );

    endian_reverse_dwords( (DRM_DWORD *)f_rgbOut, (DRM_DWORD*)f_rgbOut, 4*LNGQDW );

ErrorExit:
    PKUNLOCK;
    if( k )
    {
        OEM_SECURE_ZERO_MEMORY( k, (SIZEOF( digit_t) * LNGQ ) + ( SIZEOF (digit_t) * 5*2*LNGQ ) );
    }
    return dr;
}


/*********************************************************************
**
**  Function:  OEM_ECC_Decrypt_P160Impl
**
**  Synopsis:
**
**  Arguments:
**     [f_pContext] -- Pointer to context the size of DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE
**     [f_pprivkey] -- Private key to decrypt with
**     [f_rgbIn]    -- Encrypted bytes that are to be decrypted
**     [f_rgbOut]   -- Clear text result
**
**  Notes:  Data is not decrpyted in place.  It is put in the rgbOut buffer.
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL OEM_ECC_Decrypt_P160Impl(
    IN        DRM_VOID *f_pContext,
    IN  const PRIVKEY  *f_pprivkey,
    __in_bcount( DRM_ECC160_CIPHERTEXT_LEN ) const DRM_BYTE  f_rgbIn[__CB_DECL(DRM_ECC160_CIPHERTEXT_LEN)],
    __out_bcount( DRM_ECC160_PLAINTEXT_LEN ) DRM_BYTE  f_rgbOut[__CB_DECL(DRM_ECC160_PLAINTEXT_LEN)] )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRMBIGNUM_CONTEXT_STRUCT *pContext = (DRMBIGNUM_CONTEXT_STRUCT *)f_pContext;
    DRM_DWORD rgdwPrivKey[ DRM_ECC160_PRIVATE_KEY_LEN / SIZEOF( DRM_DWORD ) ];
    DRM_DWORD rgdwIn     [DRM_ECC160_CIPHERTEXT_LEN / SIZEOF( DRM_DWORD )];
    DRM_DWORD rgdwOut    [DRM_ECC160_PLAINTEXT_LEN / SIZEOF( DRM_DWORD )];
    digit_t *a   = NULL; /* [2*LNGQ] */
    digit_t *b   = NULL; /* [2*LNGQ] */
    digit_t *tmp = NULL; /* [2*LNGQ] */
    digit_t *ecM = NULL; /* [2*LNGQ] */
    digit_t    pk[(SIZEOF( digit_t) * LNGQ ) + ( SIZEOF (digit_t) * 4*2*LNGQ )] = {0}; /* [LNGQ]   */
    struct bigctx_t *pBigCtx = NULL;

    USEPKLOCK;

    ChkArg( f_pContext    != NULL
         && f_pprivkey    != NULL
         && f_rgbIn       != NULL
         && f_rgbOut      != NULL );

    DRMASSERT( (DRM_DWORD_PTR)f_pprivkey % SIZEOF( DRM_DWORD ) == 0 );

    ChkDR( OEM_ECC_PKInit( pContext, NULL ) );

    PKLOCK;

    ChkDR( OEM_ECC_InitializeBignumStackImpl( pContext ) );

    pBigCtx = (struct bigctx_t *) &pContext->oHeap;

    a   = (digit_t*)(pk  +     LNGQ);
    b   = (digit_t*)(a   + 2 * LNGQ);
    tmp = (digit_t*)(b   + 2 * LNGQ);
    ecM = (digit_t*)(tmp + 2 * LNGQ);


    endian_reverse_dwords( (const DRM_DWORD *)f_pprivkey->x, rgdwPrivKey, DRM_ECC160_PRIVATE_KEY_LEN / SIZEOF( DRM_DWORD ) );
    dwords_to_digits( rgdwPrivKey, pk, LNGQDW );

    /* Do a byte copy from the input buffer to the local buffer since
    ** only the local buffer is guaranteed to be DWORD aligned
    */
    OEM_SECURE_MEMCPY( rgdwIn, f_rgbIn, DRM_ECC160_CIPHERTEXT_LEN );

    endian_reverse_dwords( (const DRM_DWORD *)rgdwIn, rgdwIn, DRM_ECC160_CIPHERTEXT_LEN / SIZEOF(DRM_DWORD) );

    dw_to_modular( rgdwIn,          LNGQDW, a,      &pContext->pkData.qmodulus, pBigCtx );
    dw_to_modular( rgdwIn + LNGQDW, LNGQDW, a+LNGQ, &pContext->pkData.qmodulus, pBigCtx );
    dw_to_modular( rgdwIn+2*LNGQDW, LNGQDW, b,      &pContext->pkData.qmodulus, pBigCtx );
    dw_to_modular( rgdwIn+3*LNGQDW, LNGQDW, b+LNGQ, &pContext->pkData.qmodulus, pBigCtx );

    if( !ecaffine_on_curve( a, &pContext->pkData.ecurve, NULL, NULL, pBigCtx ) )
    {
        ChkDR( DRM_E_PKCRYPTO_FAILURE );
    }

    if( !ecaffine_exponentiation_tabular( a, TABLE_SPACING, 0, pk, LNGQ, tmp, &(pContext->pkData.ecurve), pBigCtx ) )
    {
        ChkDR( DRM_E_PKCRYPTO_FAILURE );
    }
    if( !ecaffine_addition( b, tmp, ecM, -1, &(pContext->pkData.ecurve), NULL, pBigCtx ) )
    {
        ChkDR( DRM_E_PKCRYPTO_FAILURE );
    }
    if( !ecaffine_to_dwords( ecM, rgdwOut, &(pContext->pkData.ecurve) ) )
    {
        ChkDR( DRM_E_PKCRYPTO_FAILURE );
    }
    OEM_SECURE_MEMCPY( f_rgbOut, rgdwOut, DRM_ECC160_PLAINTEXT_LEN );
    OEM_SECURE_ZERO_MEMORY( rgdwOut, DRM_ECC160_PLAINTEXT_LEN );

ErrorExit:
    PKUNLOCK;
    if( pContext )
    {
        if( pk )
        {
            OEM_SECURE_ZERO_MEMORY( pk, (SIZEOF( digit_t) * LNGQ ) + ( SIZEOF (digit_t) * 4*2*LNGQ ) );
        }

    }

    return( dr );
}

#define SIGN_BUF_LEN  (LNGQDW*SIZEOF(DRM_DWORD))

#define PACKED_CHAR_BIT (CHAR_BIT/CB_NATIVE_BYTE)
#define BYTES_PER_DIGIT (RADIX_BITS/PACKED_CHAR_BIT)

#if (RADIX_BITS % CHAR_BIT != 0)
#error "Radix_bits not multiple of CHAR_BIT"
#endif

static void DRMFORCEINLINE byte_array_to_digits(
    const  DRM_BYTE     buf[__CB_DECL(SIGN_BUF_LEN)],
            digit_t     dividend[LNGQDW] )
{
    DRM_DWORD ib;
    ZEROMEM(dividend, LNGQDW*SIZEOF( digit_t ));

    for (ib = 0; ib < SIGN_BUF_LEN; ib++)
    {
        const DRM_DWORD idigit = ib/BYTES_PER_DIGIT;
        const DRM_DWORD ishift = PACKED_CHAR_BIT*ib - RADIX_BITS*idigit;
        dividend[idigit] |= ((digit_t)GET_BYTE(buf,ib)) << ishift;
    }
}


static DRM_NO_INLINE DRM_BOOL DRM_CALL byte_array_mod_bignum(
    const  DRM_BYTE     buf[__CB_DECL(SIGN_BUF_LEN)],
           digit_t      remainder[],
           PK_DATA     *pPKData )               /* Given a message to be signed, in byte form reduce it modulo pkData.r[].  */
{
    digit_t dividend[LNGQDW];

    if ( pPKData == NULL )
        return FALSE;

    byte_array_to_digits( buf, dividend );

    /* Do the division, discard quotient. */
    divide(dividend, LNGQDW, pPKData->r, pPKData->lngr, &pPKData->rrecip, NULL, remainder);

    OEM_SECURE_ZERO_MEMORY( dividend, sizeof(dividend) );

    return TRUE;
} /* end byte_array_mod_bignum */


/* Convert (i.e., cast) a field element to an integer, */
/* reduce the value modulo divisor.  FE2IP is a P1363 function. */
/*
    for prime case:
    fdata is xcoordinate of a point in modular format.
    we call from_modular and look for the value mod divisor.
    so, given P
    return from_modular(P.x) mod divisor.
*/
static DRM_NO_INLINE DRM_BOOL FE2IPmod(
    const digit_t     *fdata,
    const field_desc_t    *fdesc,
    const digit_t          divisor[],
    const DRM_DWORD        ldivisor,
    const reciprocal_1_t  *recip,
    digit_t                remainder[],    /* OUT */
    DRM_VOID              *pContext )
{
    DRM_BOOL OK = TRUE;
    digit_t *mdata; /*[MP_LONGEST]; */
    const digit_t *data;
    struct bigctx_t *pBigCtx = (struct bigctx_t*) pContext;

    mdata = (digit_t*)bignum_alloc( SIZEOF( digit_t ) * MP_LONGEST, pBigCtx );
    if( mdata == NULL )
    {
        return FALSE;
    }

    if (fdesc->ftype == FIELD_Q_MP)
    {
        from_modular(fdata, mdata, fdesc->modulo);    /* Get standard representation */
        data = mdata;
    }
    else
    {  /* Characteristic 2 */
        data = fdata;
        if( !(mp_significant_bit_count(fdata, fdesc->elng) <= fdesc->degree) )
        {
            OK = FALSE;
            /* Leading zeros expected */
        }
    }
    if( OK )
    {
        divide(data, fdesc->elng, divisor, ldivisor, recip, NULL, remainder);
    }
    OEM_SECURE_ZERO_MEMORY( mdata, SIZEOF( digit_t ) * MP_LONGEST );
    bignum_free( mdata, pBigCtx );
    return OK;
} /* end FE2IPmod */



/*********************************************************************
**
**  Function:  OEM_ECC_Sign_P160Impl
**
**  Synopsis:  Generate a digital signature with a private key,
**             using ECDSA algorithm.
**
**  Arguments:
**     [f_pContext]     -- Pointer to context the size of DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE
**     [f_privkey]      -- Private key to create a signature with
**     [f_pbData]       -- Array of bytes to create a signature over
**     [f_cbData]       -- Length of pbBuffer in bytes
**     [f_rgbSignature] -- Buffer to hold result signature
**
**  Notes:     Notation used in the comments is the one of a book "Guide to ECC",
**             see page 184 for the implemented algorithm.
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL OEM_ECC_Sign_P160Impl(
    IN       DRM_VOID  *f_pContext,
    IN const PRIVKEY   *f_privkey,
    __out_bcount( f_cbData ) const DRM_BYTE  *f_pbData,
    IN       DRM_DWORD  f_cbData,
    __out_bcount( DRM_ECC160_SIGNATURE_LEN ) DRM_BYTE   f_rgbSignature[__CB_DECL(DRM_ECC160_SIGNATURE_LEN)] )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRMBIGNUM_CONTEXT_STRUCT *pContext = (DRMBIGNUM_CONTEXT_STRUCT *)f_pContext;
    DRM_DWORD rgdwPrivKey[ DRM_ECC160_PRIVATE_KEY_LEN / SIZEOF( DRM_DWORD )];
    DRM_DWORD ntry = 0;
    DRM_DWORD    lgcd   = 0;
    digit_t  *Gpriv2    = NULL;  /* [2*MP_LONGEST] */
    digit_t  *gcd       = NULL;  /* [MP_LONGEST] */
    digit_t  *priv2     = NULL;  /* [MP_LONGEST] */
    digit_t  *priv2inv  = NULL;  /* [MP_LONGEST] */
    digit_t  *sigc      = NULL;  /* [LNGQ] */
    digit_t  *sigd      = NULL;  /* [LNGQ] */
    digit_t  *privexpon = NULL;  /* [LNGQ] */
    DRM_BYTE *buffer    = NULL;  /* [SIGN_BUF_LEN]     */
    struct bigctx_t *pBigCtx = NULL;

    USEPKLOCK;

    ChkArg( f_pContext     != NULL
         && f_privkey      != NULL
         && f_pbData       != NULL
         && f_cbData       != 0
         && f_rgbSignature != NULL );

    DRMASSERT( (DRM_DWORD_PTR)f_privkey % SIZEOF( DRM_DWORD ) == 0 );

    ChkDR( OEM_ECC_PKInit( pContext, NULL ) );

    PKLOCK;

    ChkDR( OEM_ECC_InitializeBignumStackImpl( pContext ) );

    pBigCtx = (struct bigctx_t *) &pContext->oHeap;

    ChkMem( gcd = (digit_t*)bignum_alloc( (SIZEOF( digit_t) * (3*LNGQ + 3*MP_LONGEST) )
                                         +(SIZEOF(digit_t) * 2*MP_LONGEST )
                                         +SIGN_BUF_LEN,
                                         pBigCtx ) );

    priv2     = gcd      + MP_LONGEST;
    priv2inv  = priv2    + MP_LONGEST;
    sigc      = priv2inv + MP_LONGEST;
    sigd      = sigc     + LNGQ;
    privexpon = sigd     + LNGQ;

    Gpriv2    = (digit_t*)(privexpon + LNGQ);
    buffer    = (DRM_BYTE*)(Gpriv2      + 2 * MP_LONGEST);

    endian_reverse_dwords( (const DRM_DWORD *)f_privkey->x, rgdwPrivKey, DRM_ECC160_PRIVATE_KEY_LEN / SIZEOF( DRM_DWORD ) );
    dwords_to_digits( rgdwPrivKey, privexpon, LNGQDW );

    {
        DRM_SHA_CONTEXT shadata;

        MEMSET( buffer, 0, SIGN_BUF_LEN );

        ChkDR( DRM_SHA_Init( &shadata, eDRM_SHA_1 ) );
        ChkDR( DRM_SHA_Update( (DRM_BYTE*) f_pbData, f_cbData, &shadata ) );
        ChkDR( DRM_SHA_Finalize( &shadata, DRM_SHA1_DIGEST_LEN, buffer ) ); /* buffer is e */
    }

    for( ntry = 0; ntry < 1000; ntry++ )
    {
        /* generate 0 <= k < n, pContext->pkData.r aka order or n */
        ChkBOOL( random_mod_nonzero( pContext->pkData.r, priv2, LNGQ, pBigCtx ), DRM_E_PKCRYPTO_FAILURE); // priv2 is k

        ChkBOOL( ecaffine_exponentiation_tabular( pContext->pkData.TABLE, TABLE_SPACING, TABLE_LAST, priv2, LNGQ,
            Gpriv2, /* outpuot Gpriv2 == k*P */
            &pContext->pkData.ecurve, pBigCtx ), DRM_E_PKCRYPTO_FAILURE );

        /* get r = sigc = from_modular(P.x)%order */
        ChkBOOL( FE2IPmod(Gpriv2, pContext->pkData.ecurve.fdesc, pContext->pkData.r, pContext->pkData.lngr, &pContext->pkData.rrecip, sigc, pBigCtx ), DRM_E_PKCRYPTO_FAILURE);

        /* if r == 0 then restart. */
        if ( significant_digit_count( sigc, LNGQDW ) == 0 )
        {
            continue;
        }

        /* look for k^-1 over mod n aka order, gcd and lgcd are not used values */
        ChkBOOL( mp_gcdex(priv2, pContext->pkData.lngr, pContext->pkData.r, pContext->pkData.lngr,  priv2inv, NULL, gcd, NULL, &lgcd, NULL, pBigCtx), DRM_E_PKCRYPTO_FAILURE );

        /* reuse Gpriv2 to get d*r, privexpon = d, private key  */
        ChkBOOL( multiply(sigc, pContext->pkData.lngr, privexpon, pContext->pkData.lngr, Gpriv2), DRM_E_PKCRYPTO_FAILURE );

        /* move from lsb to digits, reuse of gcd */
        byte_array_to_digits(buffer, gcd);

        /* Overflow impossible, since Gpriv2 is at most (2^160-1)^2 == 2^320-1-2*2^160
           gcd contents at most 2^160 - 1, thus sum <= 2^320-1-2^160-1 < 2^320 */
        ChkBOOL( add_diff(Gpriv2, 2*pContext->pkData.lngr, gcd, pContext->pkData.lngr, Gpriv2, NULL), DRM_E_PKCRYPTO_FAILURE);

        /* take mod of the multiplication and sum */
        ChkBOOL( divide(Gpriv2, 2*pContext->pkData.lngr, pContext->pkData.r, pContext->pkData.lngr, &pContext->pkData.rrecip, NULL, gcd), DRM_E_PKCRYPTO_FAILURE );

        /* multiply the inverse */
        ChkBOOL( multiply(priv2inv, pContext->pkData.lngr, gcd, pContext->pkData.lngr, Gpriv2), DRM_E_PKCRYPTO_FAILURE );
        ChkBOOL( divide(Gpriv2, 2*pContext->pkData.lngr, pContext->pkData.r, pContext->pkData.lngr, &pContext->pkData.rrecip, NULL, sigd), DRM_E_PKCRYPTO_FAILURE );

        /* sigc is already non zero */
        if( significant_digit_count(sigd,  pContext->pkData.lngr) != 0 )
        {
            /* in case of success, finalize the signature */
            ChkBOOL( digits_to_dwords( sigc, (DRM_BYTE *)f_rgbSignature, LNGQDW ), DRM_E_PKCRYPTO_FAILURE );
            ChkBOOL( digits_to_dwords( sigd, (DRM_BYTE *)(((DRM_DWORD *)f_rgbSignature) + LNGQDW), LNGQDW ), DRM_E_PKCRYPTO_FAILURE );
            endian_reverse_dwords( (const DRM_DWORD *)f_rgbSignature, (DRM_DWORD*) f_rgbSignature, 2*LNGQDW );

            /* we are successful */
            goto ErrorExit;
        }

    }

    /* we went through n tries and failed */
    ChkDR( DRM_E_PKCRYPTO_FAILURE );

ErrorExit:
    if( pContext != NULL )
    {
        if( gcd )
        {
            /* This erases priv2, priv2inv, sigc, sigd, privexpon up to Gpriv2*/
            OEM_SECURE_ZERO_MEMORY( gcd, (SIZEOF( digit_t) * (3*LNGQ + 3*MP_LONGEST) ) + (SIZEOF(digit_t) * 2*MP_LONGEST ) + SIGN_BUF_LEN );
        }
        bignum_free( gcd, pBigCtx );
    }
    PKUNLOCK;

    return dr;
} /* end OEM_ECC_Sign_P160Impl */


/*********************************************************************
**
**  Function:  OEM_ECC_Verify_P160Impl
**
**  Synopsis:  Verify an ECDSA digital signature created
**             by OEM_ECC_Sign_P160.
**
**  Arguments:
**     [f_pContext]     -- Pointer to context the size of DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE
**     [f_ppubkey]      -- Pubkey to check the signature with
**     [f_pbData]       -- Data buffer that the signature was created over
**     [f_cbData]       -- Length of pbBuffer in bytes
**     [f_rgbSignature] -- The signature to verify
**
**  Returns:  TRUE if the signature verified correctly.  FALSE is it didn't
**
**  Notes:     Notation used in the comments is the one of a book "Guide to ECC",
**             see page 184 for the implemented algorithm.

*********************************************************************/
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL OEM_ECC_Verify_P160Impl(
    IN       DRM_VOID  *f_pContext,
    IN const PUBKEY    *f_ppubkey,
    __in_bcount( f_cbData ) const DRM_BYTE  *f_pbData,
    IN       DRM_DWORD  f_cbData,
    __in_bcount( DRM_ECC160_SIGNATURE_LEN ) const DRM_BYTE   f_rgbSignature[__CB_DECL(DRM_ECC160_SIGNATURE_LEN)] )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRMBIGNUM_CONTEXT_STRUCT *pContext = (DRMBIGNUM_CONTEXT_STRUCT *)f_pContext;
    DRM_DWORD rgdwPubKey[ DRM_ECC160_PUBLIC_KEY_LEN / SIZEOF( DRM_DWORD ) ];
    DRM_DWORD rgdwSignature[ 2*LNGQDW ];
    DRM_DWORD   lgcd     = 0;
    digit_t    *sigdinv  = NULL; /* [MP_LONGEST] */
    digit_t    *temp     = NULL; /* [MP_LONGEST], a place reused in our calculations */
    digit_t    *sigc     = NULL; /* [LNGQ] */
    digit_t    *sigd     = NULL; /* [LNGQ] */
    digit_t    *h1G      = NULL; /* [2*MP_LONGEST] */
    digit_t    *h2pubkey = NULL; /* [2*MP_LONGEST] */
    digit_t    *pubkey   = NULL; /* [2*LNGQ] */
    DRM_BYTE   *buffer   = NULL; /* [SIGN_BUF_LEN] */
    struct bigctx_t *pBigCtx  = NULL;
    const ecurve_t  *ecurve   = NULL;

    USEPKLOCK;

    ChkArg( f_pContext     != NULL
         && f_ppubkey      != NULL
         && f_pbData       != NULL
         && f_cbData       != 0
         && f_rgbSignature != NULL );

    DRMASSERT( (DRM_DWORD_PTR)f_ppubkey % SIZEOF( DRM_DWORD ) == 0 );

    ChkDR( OEM_ECC_PKInit( pContext, NULL ) );

    PKLOCK;

    ChkDR( OEM_ECC_InitializeBignumStackImpl( pContext ) );

    pBigCtx = (struct bigctx_t *) &pContext->oHeap;

    ChkMem( sigdinv = (digit_t*)bignum_alloc( (SIZEOF( digit_t) * (2*LNGQ + 2*MP_LONGEST) )
                                             +(SIZEOF(digit_t) * (2*2*MP_LONGEST + 2*LNGQ) )
                                             + SIGN_BUF_LEN,
                                             pBigCtx ) );

    temp     = sigdinv  + MP_LONGEST;
    sigc     = temp     + MP_LONGEST;
    sigd     = sigc     + LNGQ;
    h1G      = (digit_t*)(sigd     +  LNGQ);
    h2pubkey = h1G      + 2*MP_LONGEST;
    pubkey   = h2pubkey + 2*MP_LONGEST;
    buffer   = (DRM_BYTE*)(pubkey   +2*LNGQ);

    ecurve = &pContext->pkData.ecurve;

    /* get the correct endianness */
    endian_reverse_dwords( (const DRM_DWORD *)f_rgbSignature, rgdwSignature, 2*LNGQDW );

    ChkBOOL( dwords_to_digits( rgdwSignature,          sigc, LNGQDW ), DRM_E_PKCRYPTO_FAILURE );
    ChkBOOL( dwords_to_digits( rgdwSignature + LNGQDW, sigd, LNGQDW ), DRM_E_PKCRYPTO_FAILURE );

    endian_reverse_dwords( (const DRM_DWORD *)f_ppubkey->y, rgdwPubKey, DRM_ECC160_PUBLIC_KEY_LEN / SIZEOF( DRM_DWORD ) );

    ChkBOOL( dw_to_modular( rgdwPubKey,        LNGQDW, pubkey,      &(pContext->pkData.qmodulus), pBigCtx ), DRM_E_PKCRYPTO_FAILURE );
    ChkBOOL( dw_to_modular( rgdwPubKey+LNGQDW, LNGQDW, pubkey+LNGQ, &(pContext->pkData.qmodulus), pBigCtx ), DRM_E_PKCRYPTO_FAILURE );


    {
        DRM_SHA_CONTEXT shadata;

        OEM_SECURE_ZERO_MEMORY( buffer, SIGN_BUF_LEN);

        ChkDR( DRM_SHA_Init( &shadata, eDRM_SHA_1 ) );
        ChkDR( DRM_SHA_Update( (DRM_BYTE*)f_pbData, f_cbData, &shadata ) );
        ChkDR( DRM_SHA_Finalize( &shadata, DRM_SHA1_DIGEST_LEN, buffer ) );
    }


    ChkBOOL( ecaffine_on_curve(pubkey, ecurve, NULL, NULL, pBigCtx), DRM_E_PKCRYPTO_FAILURE );

    /* Verify that r and s (r == sigc, s == sigd ) are in interval [1,n-1] */

    /* First check that r != 0 != s */
    if( significant_digit_count(sigc,  pContext->pkData.lngr) == 0
     || significant_digit_count(sigd,  pContext->pkData.lngr) == 0 )
    {
        ChkDR( DRM_E_PKCRYPTO_FAILURE );
    }

    /* Verify that r < n-1; pContext->pkData.r is n */
    if( compare_same(sigc, pContext->pkData.r, pContext->pkData.lngr) >= 0 )
    {
        ChkDR( DRM_E_PKCRYPTO_FAILURE );
    }
    /* Verify that s < n-1; pContext->pkData.r is n */
    if( compare_same(sigd, pContext->pkData.r, pContext->pkData.lngr) >= 0 )
    {
        ChkDR( DRM_E_PKCRYPTO_FAILURE );
    }

    /* Compute sigdinv = w = s^-1, ignore values from gcd and lgcd */
    ChkBOOL( mp_gcdex(sigd, pContext->pkData.lngr, pContext->pkData.r, pContext->pkData.lngr, sigdinv, NULL, temp, NULL, &lgcd, NULL, pBigCtx), DRM_E_PKCRYPTO_FAILURE );


    /* e = H(m) */
    ChkBOOL( byte_array_mod_bignum(buffer, temp, &pContext->pkData), DRM_E_PKCRYPTO_FAILURE );

    /* h1G = u1 = e * w */
    ChkBOOL( multiply(sigdinv, pContext->pkData.lngr, temp, pContext->pkData.lngr, h1G), DRM_E_PKCRYPTO_FAILURE);

    /* temp = u1 % n */
    ChkBOOL( divide(h1G, 2*pContext->pkData.lngr, pContext->pkData.r, pContext->pkData.lngr, &(pContext->pkData.rrecip), NULL, temp), DRM_E_PKCRYPTO_FAILURE );

    /* TABLE represents P, looking for h1G (now represents a point) = u1 * P */
    ChkBOOL( ecaffine_exponentiation_tabular(pContext->pkData.TABLE, TABLE_SPACING, TABLE_LAST, temp, pContext->pkData.lngr, h1G, ecurve, pBigCtx),  DRM_E_PKCRYPTO_FAILURE );

    /* calculate temp = u2 = rw mod n*/
    ChkBOOL( multiply(sigdinv, pContext->pkData.lngr, sigc, pContext->pkData.lngr, h2pubkey),  DRM_E_PKCRYPTO_FAILURE );
    ChkBOOL( divide(h2pubkey, 2*pContext->pkData.lngr, pContext->pkData.r, pContext->pkData.lngr, &(pContext->pkData.rrecip), NULL, temp),  DRM_E_PKCRYPTO_FAILURE );

    /* calculate u2 * Q */
    ChkBOOL( ecaffine_exponentiation_tabular(pubkey, TABLE_SPACING, 0, temp, pContext->pkData.lngr, h2pubkey, ecurve, pBigCtx ), DRM_E_PKCRYPTO_FAILURE );

    /* X = u1P + u2Q */
    ChkBOOL( ecaffine_addition(h1G, h2pubkey, h1G, +1, ecurve, NULL, pBigCtx ), DRM_E_PKCRYPTO_FAILURE );

    /* if X is infinite, reject signature */
    ChkBOOL( !ecaffine_is_infinite(h1G, ecurve, pBigCtx ), DRM_E_PKCRYPTO_FAILURE );

    /* recover v = x coordinate from the point X mod n */
    ChkBOOL( FE2IPmod(h1G, ecurve->fdesc, pContext->pkData.r, pContext->pkData.lngr, &(pContext->pkData.rrecip), temp, pBigCtx ), DRM_E_PKCRYPTO_FAILURE );

    /* different values reject signature */
    if( compare_same(sigc, temp, pContext->pkData.lngr) != 0 )
    {
        ChkDR( DRM_E_PKCRYPTO_FAILURE );
    }

ErrorExit:
    PKUNLOCK;
    if( pContext )
    {
        if( sigdinv )
        {
            OEM_SECURE_ZERO_MEMORY( sigdinv, (SIZEOF( digit_t) * (2*LNGQ + 2*MP_LONGEST) ) + (SIZEOF(digit_t) * (2*2*MP_LONGEST + 2*LNGQ) ) + SIGN_BUF_LEN );
        }
        bignum_free( sigdinv, pBigCtx );
    }

    return DRM_SUCCEEDED( dr );
}


/*
**  random_bytes is a function bignum code calls when it needs random bytes.
*/
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL random_bytes(
    DRM_BYTE* byte_array,
    const DRM_DWORD nbyte,
    struct bigctx_t *f_pBigCtx )
{
    /*
    ** BIGNUM context is passed all around BIGNUM code.
    ** Convert to proper structure and get pOEMContext.
    ** pOEMContext can be NULL on certain cases, for example in tools. It is set in Drm_Initialize
    ** If Drm_Initialize is called, then pBigNumCtx is present and pOEMContext also present.
    */

    DRMBIGNUM_CONTEXT_STRUCT *pBigNumCtx  = (DRMBIGNUM_CONTEXT_STRUCT *)f_pBigCtx;
    DRM_VOID                 *pOEMContext = NULL;

    DRMASSERT( pBigNumCtx != NULL );

    if ( pBigNumCtx != NULL )
    {
        pOEMContext = pBigNumCtx->pOEMContext;
    }

    if( DRM_SUCCEEDED( Oem_Random_GetBytes( pOEMContext, byte_array, (DRM_DWORD)nbyte ) ) )
    {
        return TRUE;
    }

    return FALSE;
}


DRM_API DRM_NO_INLINE DRM_VOID* DRM_CALL bignum_alloc(const DRM_DWORD cblen, struct bigctx_t *f_pBigCtx )
{
    DRM_VOID *pbRet = NULL;

    DRMASSERT(  f_pBigCtx != NULL );

    if (DRM_FAILED (DRM_STK_Alloc ((DRM_STACK_ALLOCATOR_CONTEXT*) f_pBigCtx, (DRM_DWORD)cblen, &pbRet)))
    {
        return NULL;
    }

    return pbRet;
}

/*********************************************************************
**
**  Function:    bignum_alloc_align
**
**  Description:  Allocates a buffer from a given stack context using
                  DRM_STK_Alloc_Aligned
**
**  Arguments:
**  [f_pBigCtx]     -- Stack allocator context to allocate from.
**  [cblen]         -- Size of the buffer needed.
**  [cbAlign]       -- Value of the alignment.
**  [ppbBufferRaw]  -- Pointer to pointer to hold the new memory offset.
**                     See comments on top of DRM_STK_Alloc_Aligned
**  [pbRet]         -- Pointer to pointer to hold the aligned buffer.
**
*********************************************************************/
DRM_API DRM_NO_INLINE DRM_VOID* DRM_CALL bignum_alloc_align(const DRM_DWORD cblen,
                                                            const DRM_DWORD cbAlign,
                                                            struct bigctx_t *f_pBigCtx,
                                                            __out DRM_VOID **ppbBufferRaw)
{
    DRM_VOID *pbRet = NULL;

    DRMASSERT(  f_pBigCtx != NULL );

    if (DRM_FAILED (DRM_STK_Alloc_Aligned ((DRM_STACK_ALLOCATOR_CONTEXT*) f_pBigCtx, (DRM_DWORD)cblen, (DRM_DWORD)cbAlign, ppbBufferRaw, &pbRet)))
    {
        return NULL;
    }

    return pbRet;
}

DRM_API DRM_NO_INLINE DRM_VOID DRM_CALL bignum_free(DRM_VOID *pvMem, struct bigctx_t *f_pBigCtx)
{

    if ( pvMem != NULL &&  f_pBigCtx != NULL )
    {
        DRM_STK_Free( (DRM_STACK_ALLOCATOR_CONTEXT*) f_pBigCtx, pvMem );
    }
}

EXIT_PK_NAMESPACE_CODE;
