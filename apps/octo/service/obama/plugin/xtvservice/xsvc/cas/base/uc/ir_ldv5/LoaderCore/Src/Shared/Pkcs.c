/*****************************************************************************
*                  COPYRIGHT (C) 2011 IRDETO B.V.                            *
******************************************************************************
*          File : Pkcs.c                                                     *
*                                                                            *
*   Description : This module supplies the PKCS1 implementation required by  *
*                 Loader Core.                                               *
*                                                                            *
*****************************************************************************/

#include "LoaderCorePrivate.h"


#define countof(x) (sizeof(x)/sizeof(x[0]))
/* T length definition */
/*Hash T
MD5 30 20 30 0c 06 08 2a 86 48 86 f7 0d 02 05 05 00 04 10 || H
SHA-1 30 21 30 09 06 05 2b 0e 03 02 1a 05 00 04 14 || H
SHA-256 30 31 30 0d 06 09 60 86 48 01 65 03 04 02 01 05 00 04 20 || H
SHA-384 30 41 30 0d 06 09 60 86 48 01 65 03 04 02 02 05 00 04 30 || H
SHA-512 30 51 30 0d 06 09 60 86 48 01 65 03 04 02 03 05 00 04 40 || H
*/
#define T_SHA1_LENGTH   15
#define T_SHA256_LENGTH 19

#define SHA1_HASH_SIZE 20
#define SHA256_HASH_SIZE 32

static const lc_uchar t256[T_SHA256_LENGTH] = {0x30,0x31,0x30,0x0d,0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x01,0x05,0x00,0x04,0x20};
static const lc_uchar t1[T_SHA1_LENGTH] = {0x30,0x21,0x30,0x09,0x06,0x05,0x2b,0x0e,0x03,0x02,0x1a,0x05,0x00,0x04,0x14};

static pkcs_result Rsa_EmsaPkcs(pkcs_buffer_st *pEM, pkcs_buffer_st *pHash, pkcs_hash_mode hashMode);

#if 0 // by jichoi. just for making compiler happy
/**
 * Simple bit counter.
 */
static  unsigned int  Rsa_CountBits(
    const pkcs_long_num *pLongNum)
{
    lc_uint32 count = 0;
    lc_uint32 i = 0;
    lc_uint32 cur = 0;
    for (i=0; i<countof(pLongNum->num); ++i)
    {
        cur = pLongNum->num[i];
        if (cur != 0)
        {
            while (cur != 0)
            {
                cur >>= 1;
                ++count;
            }
            count += (countof(pLongNum->num) - i - 1) * LC_RSA_ELEM_LEN_BITS;
            break;
        }
    }
    return count;
}
#endif

static pkcs_result Rsa_EmsaPkcs(pkcs_buffer_st *pEM, pkcs_buffer_st *pHash, pkcs_hash_mode hashMode)
{
    pkcs_result result = PKCS_ERROR_SUCCESS;
    lc_uint32 tLen = 0;
    lc_uint32 psLength = 0;
    pkcs_buffer_st T = {LC_NULL, 0};


    /**
     * parameter validation
     */
    /*   1.  If the length of M is greater than the input limitation for the
       hash function (2^61 - 1 octets for SHA-1), output "inconsistent"
       and stop.*/

    /*   2.  Let mHash = Hash(M), an octet string of length hLen.*/
    /*   Sha = 0 ---> sha1; sha = 1 ----> sha256 */

    if(PKCS_SHA1_MODE == hashMode)
    {
        tLen += T_SHA1_LENGTH;
    }
    else if(PKCS_SHA256_MODE == hashMode)
    {
        tLen += T_SHA256_LENGTH;
    }

    tLen += pHash->length;

    /*   3.  If emLen < tLen + 11, output "intended encoded message length too short" and stop.*/
    if (PKCS_ERROR_SUCCESS == result && (SIGNATURE_LENGTH < tLen + 11))
    {
        result = PKCS_ERROR_SIGNATURE_VERIFICATION;
    }

    /*   4.  Generate an octet string PS consisting of emLen - tLen - 3 octets with hexadecimal value 0xff.
        The length of PS will be at least 8 octets..*/
    if (PKCS_ERROR_SUCCESS == result)
    {
       psLength = SIGNATURE_LENGTH - tLen - 3;
    }

    if (PKCS_ERROR_SUCCESS == result)
    {
        T.length = tLen;
        T.bytes = LC_MALLOC(T.length);
        if (LC_NULL == T.bytes)
        {
            result = PKCS_ERROR_SIGNATURE_VERIFICATION;
        }
    }

    /*   5.  the DER encoding T..*/
    if (PKCS_ERROR_SUCCESS == result)
    {
        if(PKCS_SHA1_MODE == hashMode)
        {
            LC_memcpy(T.bytes, t1, T_SHA1_LENGTH);
            LC_memcpy((T.bytes + T_SHA1_LENGTH), pHash->bytes, pHash->length);
        }
        else
        {
            LC_memcpy(T.bytes, t256, T_SHA256_LENGTH);
            LC_memcpy((T.bytes + T_SHA256_LENGTH), pHash->bytes, pHash->length);
        }
    }
    /*   6.  Let M = (0x)0x00 || 0x01 || PS || 0x00 || T  ;*/
    if (PKCS_ERROR_SUCCESS == result)
    {
        LC_memset(pEM->bytes, 0, SIGNATURE_LENGTH);
        pEM->length = SIGNATURE_LENGTH;

        LC_memset(pEM->bytes + 1, 0x01, 1);
        LC_memset((pEM->bytes + 2), 0xFF, psLength);
        LC_memcpy((pEM->bytes + 3 + psLength), T.bytes, T.length);
    }

    LC_Free_Memory((void **)&T.bytes);

    return result;
}


/**
 * Basic OS2IP function
 */
static pkcs_result Rsa_OS2IP(
    const lc_uchar *pValue,
    lc_uint32  valueLen,
    pkcs_long_num *pLongNum)
{
    pkcs_result result = PKCS_ERROR_SUCCESS;
    lc_uint32 i = 0;
    lc_uint32 wi = 0;
    lc_uint32 shift = 0;

    if (!pValue
        || !valueLen
        || !pLongNum)
    {
        result = PKCS_ERROR_NULL_PARAM;
    }
    else
    {
        /**
         * initialize the output to 0
         */
        LC_memset(pLongNum, 0,sizeof(*pLongNum));

        /**
         * shove all the values to the LSB of the destination number,
         */
        for (i=0; i<valueLen; ++i)
        {
            wi = countof(pLongNum->num) - ((valueLen - i - 1) / 4) - 1;
			/* zzw 2012/07/04 fixing the SCR 92290 to support big endian feature*/
#ifdef ENDIAN_LITTLE
            shift = (3- (i % 4) ) * 8;

#endif
#ifdef ENDIAN_BIG
            shift = ((i % 4) ) * 8;
#endif
            pLongNum->num[wi] |= pValue[i] << shift;
        }
    }

    return result;
}

/**
 * Basic I2OSP function
 */
static pkcs_result Rsa_I2OSP(
    const pkcs_long_num *pLongNum,
    lc_uchar *pValue,
    lc_uint32  valueLen)
{
    pkcs_result result = PKCS_ERROR_SUCCESS;
    lc_uint32  bytes = 0;
    lc_uint32  vali = 0;
    lc_uint32  i = 0;
    lc_uint32  wi = 0;
    lc_uint32  shift = 0;

    if(!pLongNum || !pValue || !valueLen)
    {
        result = PKCS_ERROR_NULL_PARAM;
    }

    if(PKCS_ERROR_SUCCESS == result)
    {
        bytes = valueLen;
        LC_memset(pValue, 0, valueLen);
        for (vali=0; vali < valueLen; ++vali)
        {
            i = valueLen - (bytes - vali);

            wi = countof(pLongNum->num) - ((valueLen - i - 1) / 4) - 1;
			/* zzw 2012/07/04 fixing the SCR 92290 to support big endian feature*/
#ifdef ENDIAN_LITTLE
            shift = (3- (i % 4) ) * 8;

#endif
#ifdef ENDIAN_BIG
            shift = ((i % 4) ) * 8;
#endif

            pValue[vali] = (unsigned char )((pLongNum->num[wi] & (0xFF << shift)) >> shift);
        }
    }

    return result;
}

/**
 * RSA signature  primitive.
 */
pkcs_result Rsa_SP1(const lc_uchar *pRsaKey, lc_uchar *ps, lc_uchar *pm)
{
    pkcs_result result = PKCS_ERROR_SUCCESS;

    if(!pRsaKey || !ps || !pm)
    {
        result = PKCS_ERROR_NULL_PARAM;
    }
    if(PKCS_ERROR_SUCCESS == result)
    {
        RSA_Operation(ps, pRsaKey);
        LC_memcpy(pm, ps, SIGNATURE_LENGTH);
    }

    return result;
}

/**
 * RSA verification primitive.
 */
pkcs_result Rsa_VP1(const unsigned char *pRsaKey, unsigned char *ps, unsigned char *pm)
{
    pkcs_result result = PKCS_ERROR_SUCCESS;

    if(!pRsaKey || !ps || !pm)
    {
        result = PKCS_ERROR_NULL_PARAM;
    }
    if(PKCS_ERROR_SUCCESS == result)
    {
        RSA_Operation(ps, pRsaKey);
        LC_memcpy(pm, ps, SIGNATURE_LENGTH);
    }

    return result;
}

/* =================================================== */

pkcs_result Rsa_CheckHashMode(
        const lc_uchar *signature,
        const lc_uchar *pRsaKey,
        lc_uchar *em,
        pkcs_hash_mode *pPkcsHashMode)
{
    pkcs_result result = PKCS_ERROR_SUCCESS;
    pkcs_long_num s;
    pkcs_long_num m;
    lc_uint32 emLen = 0;
    lc_uint32 modBits = 0;
    pkcs_hash_mode hashMode = PKCS_UNSUPPORT_MODE;

    LC_memset(&s, 0, sizeof(pkcs_long_num));
    LC_memset(&m, 0, sizeof(pkcs_long_num));

    /* If RSA key is not provided, we assume the signature is clear without any RSA signing. */
    if((LC_NULL == pRsaKey))
    {
        LC_memcpy(em, signature, SIGNATURE_LENGTH);
    }
    else    /* Decrypt signature first using provided RSA public key. */
    {

        /* 1. Length checking: If the length of the signature S is not k octets, output "invalid signature" and stop.
         *    -- This is actually a precondition of this function, so no check here.
         */

        /* 2. RSA verification:
         *    a. Convert the signature S to an integer signature representative s (see Section 4.2):
         *       s = OS2IP(S).
         */
        if (PKCS_ERROR_SUCCESS == result)
        {
            /* Calculate length of modulus, in bits */
            modBits = LC_RSA_MAX_LEN_BITS;
            result = Rsa_OS2IP(signature, SIGNATURE_LENGTH, &s);
        }

        /*    b. Apply the RSAVP1 verification primitive (Section 5.2.2) to the RSA public key (n, e)
         *       and the signature representative s to produce an integer message representative m:
         *       m = RSAVP1 ((n, e), s).
         *       If RSAVP1 output "signature representative out of range", output "invalid signature" and stop.
         */
        if (PKCS_ERROR_SUCCESS == result)
        {
            result = Rsa_VP1(pRsaKey, (lc_uchar *)s.num, (lc_uchar *)m.num);
        }

        /**
         *    c. Convert the message representative m to an encoded message EM
         *       of length emLen = \ceil ((modBits - 1)/8) octets, where modBits
         *       is the length in bits of the RSA modulus n (see Section 4.1):
         *
         *       EM = I2OSP (m, emLen).
         *
         *       Note that emLen will be one less than k if modBits - 1 is
         *       divisible by 8 and equal to k otherwise.  If I2OSP outputs
         *       "integer too large," output "invalid signature" and stop.
         */
        if (PKCS_ERROR_SUCCESS == result)
        {
            emLen = ((modBits-1) / 8);
            if ((modBits-1) % 8)
            {
                ++ emLen;
            }
            result = Rsa_I2OSP(&m, em, emLen);
        }

    }

    if (PKCS_ERROR_SUCCESS == result)
    {
        if(LC_memcmp(em + SIGNATURE_LENGTH - T_SHA1_LENGTH - SHA1_HASH_SIZE, t1, T_SHA1_LENGTH) == 0)
        {
            hashMode = PKCS_SHA1_MODE;
        }
        else if(LC_memcmp((em + SIGNATURE_LENGTH - T_SHA256_LENGTH - SHA256_HASH_SIZE), t256, T_SHA256_LENGTH) == 0)
        {
            hashMode = PKCS_SHA256_MODE;
        }

        /* Return PKCS hash mode. */
        *pPkcsHashMode = hashMode;
    }


    return result;
}

pkcs_result Rsa_VerifyPkcs(
    const lc_uchar em[SIGNATURE_LENGTH],
    pkcs_buffer_st *pHash,
    pkcs_hash_mode hashMode)
{
    pkcs_result result = PKCS_ERROR_SUCCESS;
    pkcs_buffer_st emc = {LC_NULL, 0};

    if(LC_NULL == pHash)
    {
        result = PKCS_ERROR_NULL_PARAM;
    }
    else if((LC_NULL == pHash->bytes) || (LC_NULL == em))
    {
        result = PKCS_ERROR_NULL_PARAM;
    }

    if(PKCS_ERROR_SUCCESS == result)
    {
        emc.length = SIGNATURE_LENGTH;
        emc.bytes = LC_MALLOC(emc.length);
        if (LC_NULL == emc.bytes)
        {
            result = PKCS_ERROR_SIGNATURE_VERIFICATION;
        }

        /* 4. Rsa_SetupSha256 cal EM to compare with EM' */
        Rsa_EmsaPkcs(&emc, pHash, hashMode);

        /* 5. compare the em and emc to get the verify result */
        if (LC_memcmp(emc.bytes, em, SIGNATURE_LENGTH) != 0)
        {
            result = PKCS_ERROR_SIGNATURE_VERIFICATION;
        }

        LC_Free_Memory((void **)&emc.bytes);
    }

    return result;
}

#ifdef GENERATE_PKCS
pkcs_result Rsa_GeneratePkcs(
        const pkcs_buffer_st  *pMessage,
        lc_uchar signature[SIGNATURE_LENGTH],
        const lc_uchar *pRsaKey,
        pkcs_buffer_st  *pHash,
        pkcs_hash_mode  hashMode)
{
    pkcs_result result = PKCS_ERROR_SUCCESS;
    pkcs_long_num s;
    pkcs_long_num m;
    pkcs_buffer_st em = {LC_NULL, 0};
    lc_uint32 emLen = 0;
    lc_uint32 modBits = 0;

    LC_memset(&s, 0, sizeof(pkcs_long_num));
    LC_memset(&m, 0, sizeof(pkcs_long_num));

	em.bytes = LoaderCoreSPI_Memory_Malloc(SIGNATURE_LENGTH);
	em.length = SIGNATURE_LENGTH;
    /*
        1. Length checking: If the length of the signature S is not k octets,
            output "invalid signature" and stop.-- This is actually a precondition of this function, so no check here.

            EMSA-PKCS1-v1_5 encoding: Apply the EMSA-PKCS1-v1_5 encoding operation (Section 9.2) to the message M to
            produce an encoded message EM of length k octets:	 EM = EMSA-PKCS1-V1_5-ENCODE (M, k) .

     */
    if (PKCS_ERROR_SUCCESS == result)
    {
        result = Rsa_EmsaPkcs(&em, pHash, hashMode);
    }


    /* 2. RSA verification:

        a. Convert the signature S to an integer signature representative
            s (see Section 4.2):

        s = OS2IP (S).
     */
    if (PKCS_ERROR_SUCCESS == result)
    {
        /**
         * Calculate length of modulus, in bits
         */

        modBits = LC_RSA_MAX_LEN_BITS;
        result = Rsa_OS2IP(
            em.bytes,
            SIGNATURE_LENGTH,
            &s);
    }

    /*
        b. Apply the RSAVP1 verification primitive (Section 5.2.2) to the
            RSA public key (n, e) and the signature representative s to
            produce an integer message representative m:

            m = RSAVP1 ((n, e), s).

            If RSAVP1 output "signature representative out of range,"
            output "invalid signature" and stop.
     */
    if (PKCS_ERROR_SUCCESS == result)
    {
        result = Rsa_SP1(
                pRsaKey,
                (lc_uchar *)s.num,
                (lc_uchar *)m.num);
    }

    /**
        c. Convert the message representative m to an encoded message EM
            of length emLen = \ceil ((modBits - 1)/8) octets, where modBits
            is the length in bits of the RSA modulus n (see Section 4.1):

            EM = I2OSP (m, emLen).

            Note that emLen will be one less than k if modBits - 1 is
            divisible by 8 and equal to k otherwise.  If I2OSP outputs
            "integer too large," output "invalid signature" and stop.
     */
    if (PKCS_ERROR_SUCCESS == result)
    {
        emLen = ((modBits-1) / 8);
        if ((modBits-1) % 8)
        {
            ++emLen;
        }

        result = Rsa_I2OSP(
                &m,
                em.bytes,
                emLen);
    }

    /**
        c. copy em info to signature
     */

    if (PKCS_ERROR_SUCCESS == result)
    {
        LC_memcpy(signature, em.bytes, em.length);
    }

	LoaderCoreSPI_Memory_Free((void **)&em.bytes);

    return result;
}
#endif


