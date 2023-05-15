
/****************************************************************************
*                                                                            *
*        File : Rsa.c                                                        *
*                                                                            *
* Description : This is an efficient general RSA operation implementation.   *
*               The modulus operation was streamlined using ideas from the   *
*               Philips P83C852 documentation, though the general operation  *
*               was primarily based on theory.                               *
*                                                                            *
*       Notes : This module is tailored for 16 bit machines.  Although it    *
*               has been written to be platform independent, it is a         *
*               simple matter to increase the range of platforms on which    *
*               it will run efficiently through the use of conditional       *
*               compilation.                                                 *
*                                                                            *
*               For secrecy encryption, a public key of the pair is used for *
*               encryption, and the secret key for decryption.  For          *
*               authentication, the secret key is used for signature         *
*               generation, and the public key for verification.  Weaknesses *
*               of the system must be kept in mind when designing            *
*               cryptographic applications.  Some guidelines are not to use  *
*               the same secret key for signature generation and secrecy     *
*               encryption, and to include a large amount of redundancy      *
*               (minimum 64 bits) for signature verification purposes.       *
*
*      Status : Final
*                                                                            *
*      Author : Manfred von Willich                                          *
*                                                                            *
* Date written: 1994-07-30                                                   *
* Last Changed: 1995-02-09                                                   *
*                                                                            *
*****************************************************************************/

#include "LoaderCorePrivate.h"


/* The following three statements should be adapted to the hardware multiply*/
#define UBITS     (16)                /* number of bits in type un1 */
typedef lc_uint16 un1;
typedef lc_uint32 un2;

#define MAXUN     (255/(UBITS/8))     /* defines local array sizes */
typedef un1 aun [MAXUN + 1];

/*Use them as global variable so that it will need big stack.*/
aun axRN, axRA, axRB, axRE;


static lc_uchar bModLen = 0;   /* the number of "units" (words) in the modulus */
static un1 *paxRN = 0;

/* Gets a multi-precision value from an array of bytes */
/* ... assumes high-to-low element ordering in both source and dest */
static void mp_get (un1 *paxDest, const lc_uchar *pabSrce)
{
   lc_uchar bI = 0;
   un1 temp = 0;

   /*NEIL_TODO:Find out why crash!!!!!!!!!!!!!!!!!!!!!*/
   for (bI = 0; bI != bModLen; bI++)
   {
      temp = (((un1)pabSrce[2*bI+0]) << 8);
      temp = temp | (un1)pabSrce[2*bI+1];
      paxDest[bI] = temp ;
   }
}

/* Puts a multi-precision value into an array of bytes */
/* This may be replaced by memcpy on high-to-low machines */
static void mp_put (lc_uchar *pabDest, const un1 *paxSrce)
{
   lc_uchar bI;
   for (bI = 0; bI != bModLen; bI++)
   {
      pabDest[2*bI+0] = (lc_uchar)(paxSrce[bI] >> 8);
      pabDest[2*bI+1] = (lc_uchar)(paxSrce[bI] >> 0);
   }
}

/* Zeroes a long_data variable */
static void mp_clear (un1 *paxR)
{
   lc_uchar bI;
   bI = bModLen;
   while (bI-- != 0)
   {
      paxR[bI] = 0;
   }
}

/* Copies the value of one long_data variable into another */
static void mp_copy (un1 *paxDest, const un1 *paxSrce)
{
   lc_uchar bI;
   bI = bModLen;
   while (bI-- != 0)
   {
      paxDest[bI] = paxSrce[bI];
   }
}


/* Performs a word by multi-precision multiply */
static void mp_opmult (un1 *pxRem, un1 *paxR0, un1 xR1n, const un1 *paxR2)
{
   lc_uchar bI0;
   un2 prod;
   un1 xCarry = 0;
   for (bI0 = bModLen; bI0-- != 0; )
   {
      prod = (un2)paxR0[bI0] + (un2)xCarry + (un2)xR1n*(un2)paxR2[bI0];
      paxR0[bI0] = (un1)prod;
      xCarry = (un1)(prod>>UBITS);
   }
   *pxRem = xCarry;
}

/* Performs a word shift of a multi-precision value */
static void mp_opshift (un1 *pxRem, un1 *paxR0)
{
   un1 xCarry;
   lc_uchar bI0;
   un1 xTmpu;

   xCarry = 0;
   for (bI0 = bModLen; bI0-- != 0; )
   {
      xTmpu = paxR0[bI0];
      paxR0[bI0] = xCarry;
      xCarry = xTmpu;
   }
   *pxRem = xCarry;
}

/* Performs a multi-precision by multi-precision modulus multiplication */
/* Calculation time climbs if the modulus has few leading 1s */

static void mp_modmult (un1 *paxR1, const un1 *paxR2)
{
   un1 xRem;
   lc_uchar bI;
   aun axR0;

   mp_clear(axR0);
   for (bI = 0; bI != bModLen; bI++)
   {
      mp_opshift(&xRem, axR0);
      while (xRem != 0)
      {
         mp_opmult(&xRem, axR0, xRem, paxRN);
      }
      mp_opmult(&xRem, axR0, paxR2[bI], paxR1);
      while (xRem != 0)
      {
         mp_opmult(&xRem, axR0, xRem, paxRN);
      }
   }
   mp_copy(paxR1, axR0);
}

/* "Left-to-right" modulus exponentiation - uses no own long number storage */
/* First parameter must be distinct from the other two */
static void mp_modexp (un1 *paxRB, const un1 *paxRA, const un1 *paxRE, lc_uchar bExpLen)
{
   lc_uchar bI, bB;
   un1 xEu;

   /* Working result initialisation: C=1 */
   for (bI = 0; bI != bModLen-1; bI++)
   {
      paxRB[bI] = 0;
   }
   paxRB[bModLen-1] = 1;

   /* Skip leading zeroes of exponent */
   for (bI = 0; paxRE[bI] == 0 && bI != bExpLen-1; bI++) {}
   xEu = paxRE[bI];
   bB = UBITS;
   while ((xEu & ((un1)1<<(UBITS-1))) == 0 && bB != 0)
   {
      xEu <<= 1;
      bB--;
   }

   /* Perform exponentiation with remaining bits in exponent */
   do
   {
      while (bB-- != 0)
      {
         mp_modmult(paxRB, paxRB);   /* this could be skipped first time */
         if ((xEu & ((un1)1<<(UBITS-1))) != 0)
         {
            mp_modmult(paxRB, paxRA);   /* this could be mp_copy first time */
         }
         xEu <<= 1;
      }
      xEu = paxRE[bI+1];   /* dud value not used last time around */
      bB = UBITS;

   } while (bI++ != bExpLen-1);
}


/*****************************************************************************
*                                                                            *
*     Routine : RSA_Operation                                                *
*                                                                            *
* Description : This routine performs the RSA cryptographic operation.  It   *
*               may be used for digital signature generation and             *
*               verification, and public key secrecy encryption and          *
*               decryption.  Due to the operation being identical in each    *
*               case, the neutral name has been given - the difference is in *
*               the keys used.  The module is optimised for 16 bit machines. *
*                                                                            *
*       Input : pabWork : A pointer to the target work area for the RSA      *
*                         operation.  This is the data to be encrypted,      *
*                         decrypted, signed or verified.  Its length is the  *
*                         same as that of the modulus in the key.            *
*               pabKey  : A pointer to the key for the operation, which is   *
*                         left unaltered.  This is an array of bytes, the    *
*                         first of which gives the length of the modulus and *
*                         the second of which gives the length of the        *
*                         exponent (in bytes).  This is followed by the      *
*                         two's complement of the modulus and the exponent,  *
*                         with lengths as given.  A key pair must be chosen  *
*                         according to the principles of the RSA security    *
*                         mechanism.                                         *
*                                                                            *
*      Output : *pabWork: The target work area is overwritten with the       *
*                         result of the operation.                           *
*                                                                            *
*       Notes : This routine deduces the length of the data from its second  *
*               parameter.  Should this parameter contain the incorrect      *
*               value in the first byte of the array pointed to,             *
*               unauthorised memory may be overwritten.                      *
*                                                                            *
*               This operation uses what I call carry modulus arithmetic,    *
*               which in effect means that the original data (plaintext)     *
*               must have its most significant bit set to ensure that the    *
*               correct value is returned by the inverse operation (i.e. bit *
*               7 of pabWork[0] must be set).                                *
*                                                                            *
*               To allow optimisation and to keep the permissible text value *
*               range large, the modulus must have its most significant      *
*               bit set (i.e. bit 7 of pabKey[2] must be clear).  To         *
*               minimise execution time, the several adjacent bits should be *
*               the same.                                                    *
*                                                                            *
*               All multi-precision input and output values are regarded as  *
*               being arranged as an array of bytes, most significant byte   *
*               first.  They MUST also be a multiple of sixteen (but         *
*               preferably 32) bits long.                                    *
*                                                                            *
* Last Changed: 1995-02-09                                                   *
*                                                                            *
*****************************************************************************/

void RSA_Operation (lc_uchar *pabWork, const lc_uchar *pabKey)
{
   lc_uchar bExpLen;

   LC_memset(axRN, 0x0, sizeof(axRN));
   LC_memset(axRA, 0x0, sizeof(axRA));
   LC_memset(axRB, 0x0, sizeof(axRB));
   LC_memset(axRE, 0x0, sizeof(axRE));

   bModLen = pabKey[0]/(UBITS/8);
   bExpLen = pabKey[1]/(UBITS/8);
   paxRN = &axRN[0];
   mp_get(axRN, pabKey + 2);
   mp_get(axRE, pabKey + 2 + pabKey[0]);
   mp_get(axRA, pabWork);
   mp_modexp(axRB, axRA, axRE, bExpLen);
   mp_put(pabWork, axRB);
}
