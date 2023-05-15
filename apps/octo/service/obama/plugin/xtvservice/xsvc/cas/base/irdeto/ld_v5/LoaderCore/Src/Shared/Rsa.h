
/*****************************************************************************
*                                                                            *
*        File : Rsa.h                                                        *
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
*                                                                            *
*      Status : Final
*                                                                            *
*      Author : Manfred von Willich                                          *
*                                                                            *
* Date written: 1994-07-30                                                   *
* Last Changed: 1995-02-09                                                   *
*                                                                            *
*****************************************************************************/

#ifndef _RSAENCR_H_
#define _RSAENCR_H_

#ifdef __cplusplus
extern "C" {
#endif

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

void RSA_Operation (lc_uchar *pabWork, const lc_uchar *pabKey);

#ifdef __cplusplus
}
#endif

#endif   /* _RSAENCR_H_ */


