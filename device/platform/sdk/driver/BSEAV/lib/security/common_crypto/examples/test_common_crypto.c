/******************************************************************************
 *    (c)2009-2012 Broadcom Corporation
 * 
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
 * USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: test_common_crypto.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 11/19/12 5:26p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /BSEAV/lib/security/common_crypto/examples/test_common_crypto.c $
 * 
 * 4   11/19/12 5:26p celysee
 * SWSECURITY-195: [SECURITY - Common Crypto] Expand the number of user-
 * defined parameters for cyphered key operation
 * 
 * 3   7/18/12 12:51p cdisc
 * SW7425-3487: adding more flexibility to common crypto
 * 
 * 2   7/12/12 3:34p cdisc
 * SW7425-3448: fixing test app to return non-zero rc in case of error
 * 
 * 1   7/9/12 1:48p cdisc
 * SW7425-3448: adding example folder for common_crypto
 * 
 * SW7405-5666/2   6/15/12 2:30p hugost
 * Modified code to pass down dma handle to common crypto layer
 * 
 * SW7405-5666/1   6/12/12 11:39a hugost
 * Common crypto test application.
 * 
 * SW7425-2717/1   3/23/12 10:48a hugost
 * Common crypto example.
 * 
 * 4   10/20/11 4:38p mphillip
 * SW7346-481: 7346/7425 B0 support exposed a lack of
 *  NEXUS_Security_GetDefaultClearKey calls
 * 
 * 3   4/15/11 2:52p jtna
 * SW7422-420: replace deprecated NEXUS_DmaJob_Start() with
 *  NEXUS_DmaJob_ProcessBlocks()
 * 
 * 2   4/6/09 5:30p yili
 * PR53745:Minor fix
 * 
 * 1   4/6/09 11:26a yili
 * PR53745:Adding security sample code to Nexus
 * 
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_memory.h"
#include "nexus_security.h"
#include "common_crypto.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

BDBG_MODULE(common_crypto);

CommonCryptoKeyLadderSettings  custKeySettings = { 0x00,/* cust key*/
                                                       0xFF, /* key var high */
                                                       0x00, /* key var low */
                                                       {0xbf, 0x98, 0x38, 0xe1, 0x74, 0x78, 0xe8, 0x5e, 0xf4, 0x96, 0x25, 0xce, 0xb8, 0x63, 0xce, 0x76}, /* proc_in for key 3 */
                                                       16,
                                                       {0x3f, 0xd5, 0xb3, 0x3e, 0x60, 0x10, 0x86, 0xb4, 0x08, 0xc8, 0xae, 0x1d, 0x49, 0xc1, 0x73, 0x8c}, /* proc_in for key 4 */
                                                       16,
                                                       false,
                                                       {NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityOperation_eDecrypt},
                                                       false,
                                                       {NEXUS_SecurityVirtualKeyladderID_eVKL6 ,NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4},
                                                       NEXUS_SecurityKeyladderID_eA,
                                                       NEXUS_SecurityKeyladderType_e3Des,
                                                       NEXUS_SecuritySwizzleType_eSwizzle0,
                                                       false,
                                                       false
                                                     };

/* ****************************************************************************
 * AES tests vectors 
 ******************************************************************************/
      
CommonCryptoKeyIvSettings aes_ecb_key = {
                                                {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c},
                                                16,
                                                {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
                                                0
                                            };
/* Decrypted buffer */
uint8_t aes_plaintext[] =   {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
                             0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a};
/* AES ECB */
/* Buffer encrypted with user key */
uint8_t aes_ecb_usr_cipher_text[] = {0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
                                     0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97};
uint8_t aes_ecb_cust_cipher_text[16]; /* Buffer encrypted with cust key */
uint8_t aes_ecb_otp_cipher_text[16];  /* Buffer encrypted with otp key */

/* AES CBC */
CommonCryptoKeyIvSettings aes_cbc_key = {
                                                {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c},
                                                16,
                                                {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
                                                16
                                            };

/* aes_plaintext Buffer encrypted with user key */
uint8_t aes_cbc_usr_cipher_text[] = {0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46,
                                     0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d};
uint8_t aes_cbc_cust_cipher_text[16]; /* Buffer encrypted with cust key */
uint8_t aes_cbc_otp_cipher_text[16]; /* Buffer encrypted with otp key */

/* AES CTR */
CommonCryptoKeyIvSettings aes_ctr_key = {
                                                {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c},
                                                16,
                                                {0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff},
                                                16
                                            };

/* aes_plaintext Buffer encrypted with user key */
uint8_t aes_ctr_usr_cipher_text[] = {0x87, 0x4d, 0x61, 0x91, 0xb6, 0x20, 0xe3, 0x26, 
                                       0x1b, 0xef, 0x68, 0x64, 0x99, 0x0d, 0xb6, 0xce};

uint8_t aes_ctr_cust_cipher_text[16]; /* Buffer encrypted with cust key */
uint8_t aes_ctr_otp_cipher_text[16];  /* Buffer encrypted with otp key */

typedef struct test_case_clear_key{
    CommonCryptoKeyIvSettings *pKeyIvSettings;
    bool use_CommonCryptoSetupKey;  /* true = uses CommonCrypto_SetupKey() to load and config the key
                                        false = uses CommonCrypto_LoadClearKeyIv() and CommonCrypto_LoadKeyConfig() */
    NEXUS_SecurityOperation opType;
    NEXUS_SecurityAlgorithm algType;
    NEXUS_SecurityAlgorithmVariant algVariant;
    NEXUS_SecurityKeyType keySlotType;
    NEXUS_SecurityTerminationMode termMode;
    bool enableExtKey;
    bool enableExtIv;
    NEXUS_SecurityAesCounterSize aesCounterSize;
    uint8_t *pClear;
    uint8_t *pCiphered;
}test_case_clear_key;


test_case_clear_key tests_clear_key[] = { 

    {&aes_ecb_key, false, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eEcb, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ecb_usr_cipher_text},
    {&aes_ecb_key, false, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eEcb, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ecb_usr_cipher_text},
    {&aes_ecb_key, false, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eEcb, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, true, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ecb_usr_cipher_text},
    {&aes_ecb_key, false, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eEcb, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, true, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ecb_usr_cipher_text},

    {&aes_cbc_key, false, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCbc, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_cbc_usr_cipher_text},
    {&aes_cbc_key, false, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCbc, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_cbc_usr_cipher_text},
    {&aes_cbc_key, false, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCbc, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, true, true, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_cbc_usr_cipher_text},
    {&aes_cbc_key, false, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCbc, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, true, true, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_cbc_usr_cipher_text},

    {&aes_ctr_key, false, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCounter, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ctr_usr_cipher_text},
    {&aes_ctr_key, false, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCounter, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ctr_usr_cipher_text},
    {&aes_ctr_key, false, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCounter, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, true, true, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ctr_usr_cipher_text},
    {&aes_ctr_key, false, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCounter, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, true, true, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ctr_usr_cipher_text},

    {&aes_ecb_key, true, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eEcb, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ecb_usr_cipher_text},
    {&aes_ecb_key, true, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eEcb, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ecb_usr_cipher_text},
    {&aes_ecb_key, true, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eEcb, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, true, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ecb_usr_cipher_text},
    {&aes_ecb_key, true, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eEcb, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, true, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ecb_usr_cipher_text},

    {&aes_cbc_key, true, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCbc, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_cbc_usr_cipher_text},
    {&aes_cbc_key, true, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCbc, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_cbc_usr_cipher_text},
    {&aes_cbc_key, true, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCbc, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, true, true, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_cbc_usr_cipher_text},
    {&aes_cbc_key, true, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCbc, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, true, true, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_cbc_usr_cipher_text},

    {&aes_ctr_key, true, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCounter, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ctr_usr_cipher_text},
    {&aes_ctr_key, true, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCounter, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ctr_usr_cipher_text},

    {&aes_ctr_key, true, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCounter, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, true, true, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ctr_usr_cipher_text},
    {&aes_ctr_key, true, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCounter, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, true, true, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ctr_usr_cipher_text},

};


typedef struct test_case_key_ladder{
    CommonCryptoKeyIvSettings *pIvSettings;
    CommonCryptoKeySrc   keySrc;
    CommonCryptoKeyLadderSettings *pKeyLadderSettings;
    bool use_CommonCryptoSetupKey;  /* true = uses CommonCrypto_SetupKey() to load and config the key
                                           false = uses CommonCrypto_LoadClearKeyIv() and CommonCrypto_LoadKeyConfig() */
    NEXUS_SecurityOperation opType;
    NEXUS_SecurityAlgorithm algType;
    NEXUS_SecurityAlgorithmVariant algVariant;
    NEXUS_SecurityKeyType keySlotType;
    NEXUS_SecurityTerminationMode termMode;
    bool enableExtKey;
    bool enableExtIv;
    NEXUS_SecurityAesCounterSize aesCounterSize;
    uint8_t *pClear;
    uint8_t *pCiphered;

}test_case_key_ladder;


test_case_key_ladder tests_key_ladder[] = { 


    {NULL,         CommonCrypto_eCustKey, &custKeySettings, false, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eEcb, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ecb_cust_cipher_text},
    {NULL,         CommonCrypto_eCustKey, &custKeySettings, false, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eEcb, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ecb_cust_cipher_text},
    {&aes_cbc_key, CommonCrypto_eCustKey, &custKeySettings, false, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCbc, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_cbc_cust_cipher_text},
    {&aes_cbc_key, CommonCrypto_eCustKey, &custKeySettings, false, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCbc, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_cbc_cust_cipher_text},


    /*{&aes_cbc_key, CommonCrypto_eCustKey, &custKeySettings, false, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCbc, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, true, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_cbc_cust_cipher_text},
    {&aes_cbc_key, CommonCrypto_eCustKey, &custKeySettings, false, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCbc, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, true, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_cbc_cust_cipher_text},
    */

    {&aes_ctr_key, CommonCrypto_eCustKey, &custKeySettings, false, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCounter, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ctr_cust_cipher_text},
    {&aes_ctr_key, CommonCrypto_eCustKey, &custKeySettings, false, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCounter, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ctr_cust_cipher_text},
    /*{&aes_ctr_key, CommonCrypto_eCustKey, &custKeySettings, false, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCounter, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, true, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ctr_cust_cipher_text},
    {&aes_ctr_key, CommonCrypto_eCustKey, &custKeySettings, false, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCounter, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, true, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ctr_cust_cipher_text},
    */
    {NULL,         CommonCrypto_eOtpKey,  &custKeySettings, false, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eEcb, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ecb_otp_cipher_text},
    {NULL,         CommonCrypto_eOtpKey,  &custKeySettings, false, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eEcb, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ecb_otp_cipher_text},

    {&aes_cbc_key, CommonCrypto_eOtpKey,  &custKeySettings, false, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCbc, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_cbc_otp_cipher_text},
    {&aes_cbc_key, CommonCrypto_eOtpKey,  &custKeySettings, false, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCbc, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_cbc_otp_cipher_text},
    /*{&aes_cbc_key, CommonCrypto_eOtpKey,  &custKeySettings, false, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCbc, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, true, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_cbc_otp_cipher_text},
    {&aes_cbc_key, CommonCrypto_eOtpKey,  &custKeySettings, false, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCbc, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, true, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_cbc_otp_cipher_text},
    */
    {&aes_ctr_key, CommonCrypto_eOtpKey,  &custKeySettings, false, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCounter, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ctr_otp_cipher_text},
    {&aes_ctr_key, CommonCrypto_eOtpKey,  &custKeySettings, false, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCounter, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ctr_otp_cipher_text},
    /*{&aes_ctr_key, CommonCrypto_eOtpKey,  &custKeySettings, false, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCounter, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, true, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ctr_otp_cipher_text},
    {&aes_ctr_key, CommonCrypto_eOtpKey,  &custKeySettings, false, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCounter, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, true, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ctr_otp_cipher_text},
    */
    {NULL,         CommonCrypto_eCustKey, &custKeySettings, true, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eEcb, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ecb_cust_cipher_text},
    {NULL,         CommonCrypto_eCustKey, &custKeySettings, true, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eEcb, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ecb_cust_cipher_text},

    {&aes_cbc_key, CommonCrypto_eCustKey, &custKeySettings, true, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCbc, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_cbc_cust_cipher_text},
    {&aes_cbc_key, CommonCrypto_eCustKey, &custKeySettings, true, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCbc, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_cbc_cust_cipher_text},
   /* {&aes_cbc_key, CommonCrypto_eCustKey, &custKeySettings, true, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCbc, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, true, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_cbc_cust_cipher_text},
    {&aes_cbc_key, CommonCrypto_eCustKey, &custKeySettings, true, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCbc, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, true, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_cbc_cust_cipher_text},
   */

    {&aes_ctr_key, CommonCrypto_eCustKey, &custKeySettings, true, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCounter, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ctr_cust_cipher_text},
    {&aes_ctr_key, CommonCrypto_eCustKey, &custKeySettings, true, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCounter, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ctr_cust_cipher_text},
    /*{&aes_ctr_key, CommonCrypto_eCustKey, &custKeySettings, true, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCounter, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, true, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ctr_cust_cipher_text},
    {&aes_ctr_key, CommonCrypto_eCustKey, &custKeySettings, true, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCounter, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, true, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ctr_cust_cipher_text},
   */
    {NULL,         CommonCrypto_eOtpKey,  &custKeySettings, true, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eEcb, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ecb_otp_cipher_text},
    {NULL,         CommonCrypto_eOtpKey,  &custKeySettings, true, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eEcb, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ecb_otp_cipher_text},

    {&aes_cbc_key, CommonCrypto_eOtpKey,  &custKeySettings, true, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCbc, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_cbc_otp_cipher_text},
    {&aes_cbc_key, CommonCrypto_eOtpKey,  &custKeySettings, true, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCbc, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_cbc_otp_cipher_text},
    /*{&aes_cbc_key, CommonCrypto_eOtpKey,  &custKeySettings, true, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCbc, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, true, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_cbc_otp_cipher_text},
    {&aes_cbc_key, CommonCrypto_eOtpKey,  &custKeySettings, true, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCbc, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, true, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_cbc_otp_cipher_text},
    */
    {&aes_ctr_key, CommonCrypto_eOtpKey,  &custKeySettings, true, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCounter, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ctr_otp_cipher_text},
    {&aes_ctr_key, CommonCrypto_eOtpKey,  &custKeySettings, true, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCounter, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, false, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ctr_otp_cipher_text},
    /*{&aes_ctr_key, CommonCrypto_eOtpKey,  &custKeySettings, true, NEXUS_SecurityOperation_eDecrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCounter, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, true, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ctr_otp_cipher_text},
    {&aes_ctr_key, CommonCrypto_eOtpKey,  &custKeySettings, true, NEXUS_SecurityOperation_eEncrypt, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCounter, NEXUS_SecurityKeyType_eOdd, NEXUS_SecurityTerminationMode_eClear, false, true, NEXUS_SecurityAesCounterSize_e128Bits, aes_plaintext, aes_ctr_otp_cipher_text},
    */
};


void generate_test_data(
    CommonCryptoHandle handle, 
    NEXUS_SecurityAlgorithm algType, 
    NEXUS_SecurityAlgorithmVariant algVariant, 
    uint8_t *pClearText, 
    uint32_t size, 
    uint8_t *pCiphered, 
    CommonCryptoKeyLadderSettings *pCustKeySettings,
    CommonCryptoKeySrc keySrc,
    CommonCryptoKeyIvSettings *pIvSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    CommonCryptoClearKeySettings ivSettings;
    CommonCryptoKeyConfigSettings  configSettings;
    CommonCryptoCipheredKeySettings cipheredKeySettings;
    CommonCryptoJobSettings jobSettings;
    NEXUS_KeySlotHandle      keySlot = 0;
    NEXUS_SecurityKeySlotSettings keySlotSettings;
    void *pBuf = NULL;
    NEXUS_DmaJobBlockSettings blk;

    /* Alloacte a key slot for the test */
    NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
    keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;

    keySlot = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
    if(!keySlot) 
    {
        BDBG_ERR(("%s - NEXUS_Security_AllocateKeySlot failed\n", __FUNCTION__));
        goto errorExit;
    }


    CommonCrypto_GetDefaultKeyConfigSettings(&configSettings);
    configSettings.keySlot = keySlot;
    configSettings.settings.opType = NEXUS_SecurityOperation_eEncrypt;
    configSettings.settings.algType = algType;
    configSettings.settings.algVariant = algVariant;
    configSettings.settings.termMode = NEXUS_SecurityTerminationMode_eClear;
    configSettings.settings.aesCounterSize = NEXUS_SecurityAesCounterSize_e128Bits;

    rc = CommonCrypto_LoadKeyConfig(handle,  &configSettings);
    if(rc != NEXUS_SUCCESS){
        BDBG_ERR(("%s - CommonCrypto_LoadKeyConfig failed, rc = %d\n", __FUNCTION__, rc));
        goto errorExit;
    }

    CommonCrypto_GetDefaultCipheredKeySettings(&cipheredKeySettings);
    cipheredKeySettings.keySlot = keySlot;
    cipheredKeySettings.keySrc = keySrc;
    BKNI_Memcpy(&cipheredKeySettings.settings, pCustKeySettings, sizeof(CommonCryptoKeyLadderSettings));

    rc = CommonCrypto_LoadCipheredKey(handle, &cipheredKeySettings);
    if(rc != NEXUS_SUCCESS){
        BDBG_ERR(("%s - CommonCrypto_LoadCipheredKey failed, rc = %d\n", __FUNCTION__, rc));
        goto errorExit;
    }

    if(algVariant == NEXUS_SecurityAlgorithmVariant_eCbc
        || algVariant ==  NEXUS_SecurityAlgorithmVariant_eCounter)
    {
        if(pIvSettings == NULL){
            BDBG_ERR(("%s - Invalid parameter\n", __FUNCTION__));
            goto errorExit;
        }

        CommonCrypto_GetDefaultClearKeySettings(&ivSettings);
        ivSettings.keySlot = keySlot;
        BKNI_Memcpy(ivSettings.settings.iv, pIvSettings->iv, 16);
        ivSettings.settings.ivSize = 16;

        rc =  CommonCrypto_LoadClearKeyIv(handle, &ivSettings);
        if(rc != NEXUS_SUCCESS){
            BDBG_ERR(("%s - CommonCrypto_LoadClearKeyIv failed, rc = %d\n", __FUNCTION__, rc));
            goto errorExit;
        }
    }

    rc = NEXUS_Memory_Allocate(size, NULL, &pBuf);
    if(rc != NEXUS_SUCCESS){
        BDBG_ERR(("%s - NEXUS_Memory_Allocate failed, rc = %d\n", __FUNCTION__, rc));
        goto errorExit;
    }

    BKNI_Memcpy(pBuf, pClearText, size);
    NEXUS_DmaJob_GetDefaultBlockSettings(&blk);

    blk.pSrcAddr =  pBuf;
    blk.pDestAddr = pBuf;
    blk.blockSize = size;
    blk.scatterGatherCryptoStart = true;
    blk.scatterGatherCryptoEnd = true;

    CommonCrypto_GetDefaultJobSettings(&jobSettings);
    jobSettings.keySlot = keySlot;

    rc = CommonCrypto_DmaXfer(handle, &jobSettings, &blk, 1);
    if(rc != NEXUS_SUCCESS){
        BDBG_ERR(("%s - CommonCrypto_DmaXfer failed, rc = %d\n", __FUNCTION__, rc));
        goto errorExit;
    }

    BKNI_Memcpy(pCiphered, pBuf, size);

errorExit:
    if(keySlot) NEXUS_Security_FreeKeySlot(keySlot);
    if(pBuf) NEXUS_Memory_Free(pBuf);
}


void init_test_vectors(CommonCryptoHandle handle)
{
    generate_test_data(handle, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eEcb,     aes_plaintext, 16, aes_ecb_cust_cipher_text, &custKeySettings, CommonCrypto_eCustKey, NULL);
    generate_test_data(handle, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCbc,     aes_plaintext, 16, aes_cbc_cust_cipher_text, &custKeySettings, CommonCrypto_eCustKey, &aes_cbc_key);
    generate_test_data(handle, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCounter, aes_plaintext, 16, aes_ctr_cust_cipher_text, &custKeySettings, CommonCrypto_eCustKey, &aes_ctr_key);

    generate_test_data(handle, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eEcb,     aes_plaintext, 16, aes_ecb_otp_cipher_text, &custKeySettings, CommonCrypto_eOtpKey, NULL);
    generate_test_data(handle, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCbc,     aes_plaintext, 16, aes_cbc_otp_cipher_text, &custKeySettings, CommonCrypto_eOtpKey, &aes_cbc_key);
    generate_test_data(handle, NEXUS_SecurityAlgorithm_eAes128, NEXUS_SecurityAlgorithmVariant_eCounter, aes_plaintext, 16, aes_ctr_otp_cipher_text, &custKeySettings, CommonCrypto_eOtpKey, &aes_ctr_key);

}

NEXUS_Error run_test_keyladder(CommonCryptoHandle handle, test_case_key_ladder *pTest)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_KeySlotHandle      keySlot;
    uint8_t *pBuf = NULL;
    uint8_t *pIv = NULL;

    uint32_t nb_blks = 0;
    NEXUS_DmaJobBlockSettings blks[2];
    NEXUS_SecurityKeySlotSettings keySlotSettings;
    CommonCryptoClearKeySettings ivSettings;
    CommonCryptoJobSettings jobSettings;
    uint8_t *pResult;
    int ii;
    
    /* Alloacte a key slot for the test */
    NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
    keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
    
    keySlot = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
    if(!keySlot) 
    {
        BDBG_ERR(("%s - NEXUS_Security_AllocateKeySlot failed\n", __FUNCTION__));
        return NEXUS_NOT_AVAILABLE; 
    }
    
    if(pTest->use_CommonCryptoSetupKey){
        CommonCryptoKeySettings keySettings;

        CommonCrypto_GetDefaultKeySettings(&keySettings, pTest->keySrc);
        keySettings.keySrc = pTest->keySrc;
        keySettings.keySlot = keySlot;
        BKNI_Memcpy(&keySettings.src.keyLadderInfo,  pTest->pKeyLadderSettings, sizeof(CommonCryptoKeyLadderSettings));
        keySettings.alg.opType = pTest->opType;
        keySettings.alg.algType =  pTest->algType;
        keySettings.alg.algVariant = pTest->algVariant;
        keySettings.alg.keySlotType = pTest->keySlotType;
        keySettings.alg.termMode = pTest->termMode;
        keySettings.alg.enableExtKey = pTest->enableExtKey;
        keySettings.alg.enableExtIv = pTest->enableExtIv;
        keySettings.alg.aesCounterSize = pTest->aesCounterSize;

        rc = CommonCrypto_SetupKey(handle, &keySettings);
        if(rc != NEXUS_SUCCESS){
            BDBG_ERR(("%s - CommonCrypto_SetupKey failed, rc = %d\n", __FUNCTION__, rc));
            goto errorExit;
        }
    }
    else {
        CommonCryptoKeyConfigSettings configSettings;
        CommonCryptoCipheredKeySettings cipheredKeySettings;

        CommonCrypto_GetDefaultKeyConfigSettings(&configSettings);

        configSettings.keySlot = keySlot;
        configSettings.settings.opType = pTest->opType;
        configSettings.settings.algType = pTest->algType;
        configSettings.settings.algVariant = pTest->algVariant;
        configSettings.settings.keySlotType = pTest->keySlotType;
        configSettings.settings.termMode = pTest->termMode;
        configSettings.settings.enableExtKey = pTest->enableExtKey;
        configSettings.settings.enableExtIv = pTest->enableExtIv;
        configSettings.settings.aesCounterSize = pTest->aesCounterSize;

        rc = CommonCrypto_LoadKeyConfig(handle, &configSettings);
        if(rc != NEXUS_SUCCESS){
            BDBG_ERR(("%s - CommonCrypto_LoadKeyConfig failed, rc = %d\n", __FUNCTION__, rc));
            goto errorExit;
        }

        CommonCrypto_GetDefaultCipheredKeySettings(&cipheredKeySettings);
        cipheredKeySettings.keySlot = keySlot;
        cipheredKeySettings.keySrc = pTest->keySrc;
        cipheredKeySettings.keySlotType = pTest->keySlotType;
        BKNI_Memcpy(&cipheredKeySettings.settings, pTest->pKeyLadderSettings, sizeof(CommonCryptoKeyLadderSettings));

        rc = CommonCrypto_LoadCipheredKey(handle, &cipheredKeySettings);
        if(rc != NEXUS_SUCCESS){
            BDBG_ERR(("%s - CommonCrypto_LoadCipheredKey failed, rc = %d\n", __FUNCTION__, rc));
            goto errorExit;
        }
    }

    rc = NEXUS_Memory_Allocate(16, NULL, (void *)&pBuf);
    if(rc != NEXUS_SUCCESS){
        BDBG_ERR(("%s - NEXUS_Memory_Allocate failed, rc = %d\n", __FUNCTION__, rc));
        goto errorExit;
    }

    if(pTest->opType == NEXUS_SecurityOperation_eEncrypt){
        BKNI_Memcpy(pBuf, pTest->pClear, 16);
        pResult = pTest->pCiphered;
    }
    else {
        /* Assume we are decryption */
        BKNI_Memcpy(pBuf, pTest->pCiphered, 16);
        pResult = pTest->pClear;
    }

    for(ii = 0; ii < 2; ii++) {
        NEXUS_DmaJob_GetDefaultBlockSettings(&blks[ii]);
    }

    blks[0].scatterGatherCryptoStart = true;
    blks[0].resetCrypto = true;
    
    if(pTest->algVariant == NEXUS_SecurityAlgorithmVariant_eCbc
        || pTest->algVariant == NEXUS_SecurityAlgorithmVariant_eCounter)
    {
        if(!pTest->enableExtIv){
            CommonCrypto_GetDefaultClearKeySettings(&ivSettings);
            BKNI_Memcpy(ivSettings.settings.iv, pTest->pIvSettings->iv, 16);
            ivSettings.settings.ivSize = 16;
            ivSettings.keySlot = keySlot;
            ivSettings.keySlotType = pTest->keySlotType;

            rc =  CommonCrypto_LoadClearKeyIv(handle, &ivSettings);
            if(rc != NEXUS_SUCCESS){
                BDBG_ERR(("%s - CommonCrypto_LoadClearKeyIv failed, rc = %d\n", __FUNCTION__, rc));
                goto errorExit;
            }
        }
        else {
            rc = NEXUS_Memory_Allocate(16, NULL, (void *)&pIv);
            if(rc != NEXUS_SUCCESS){
                BDBG_ERR(("%s - NEXUS_Memory_Allocate failed, rc = %d\n", __FUNCTION__, rc));
                goto errorExit;
            }
            printf("%s - %d\n", __FUNCTION__, __LINE__);

            /*BKNI_Memcpy(pIv, pTest->pIvSettings->iv, 16);*/
             /* Copy IV.  H and L need to be swapped */
            BKNI_Memcpy(pIv,  pTest->pIvSettings->iv + 8, 8);
            BKNI_Memcpy(pIv + 8, pTest->pIvSettings->iv, 8);

            /*  Set up block*/
            blks[nb_blks].pSrcAddr =  pIv;
            blks[nb_blks].pDestAddr = pIv;
            blks[nb_blks].blockSize = 16;
            nb_blks++;
        }
    }
    blks[nb_blks].pSrcAddr =  pBuf;
    blks[nb_blks].pDestAddr = pBuf;
    blks[nb_blks].blockSize = 16;
    blks[nb_blks].scatterGatherCryptoEnd = true;
    nb_blks++;

    CommonCrypto_GetDefaultJobSettings(&jobSettings);
    jobSettings.keySlot = keySlot;

    /*printf("pBuf (before) %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n", pBuf[0], pBuf[1], pBuf[2], pBuf[3], pBuf[4], pBuf[5], pBuf[6], pBuf[7], pBuf[8], pBuf[9], pBuf[10], pBuf[11], pBuf[12], pBuf[13], pBuf[14], pBuf[15]);
    if(pIv) printf("pIv %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n", pIv[0], pIv[1], pIv[2], pIv[3], pIv[4], pIv[5], pIv[6], pIv[7], pIv[8], pIv[9], pIv[10], pIv[11], pIv[12], pIv[13], pIv[14], pIv[15]);
    */
    rc = CommonCrypto_DmaXfer(handle, &jobSettings, blks, nb_blks);
    if(rc != NEXUS_SUCCESS){
        BDBG_ERR(("%s - CommonCrypto_DmaXfer failed, rc = %d\n", __FUNCTION__, rc));
        goto errorExit;
    }

    /*printf("pBuf (after) %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n", pBuf[0], pBuf[1], pBuf[2], pBuf[3], pBuf[4], pBuf[5], pBuf[6], pBuf[7], pBuf[8], pBuf[9], pBuf[10], pBuf[11], pBuf[12], pBuf[13], pBuf[14], pBuf[15]);*/
    if(BKNI_Memcmp(pBuf, pResult, 16) != 0){
        BDBG_ERR(("%s - DMA operation failed\n", __FUNCTION__));
        rc = NEXUS_UNKNOWN;
    }

errorExit:
    if(keySlot) NEXUS_Security_FreeKeySlot(keySlot);
    if(pBuf) NEXUS_Memory_Free(pBuf);
    if(pIv) NEXUS_Memory_Free(pIv);

    return rc;
}

NEXUS_Error run_test_clear_key(CommonCryptoHandle handle, test_case_clear_key *pTest)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_KeySlotHandle      keySlot;
    uint8_t *pBuf = NULL;
    uint8_t *pKey = NULL;
    uint8_t *pIv = NULL;
    uint32_t nb_blks = 0;
    NEXUS_DmaJobBlockSettings blks[3];
    NEXUS_SecurityKeySlotSettings keySlotSettings;
    CommonCryptoClearKeySettings keyIvSettings;
    CommonCryptoJobSettings jobSettings;
    uint8_t *pResult;
    int ii;

    /* Alloacte a key slot for the test */
    NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
    keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;

    keySlot = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
    if(!keySlot) 
    {
        BDBG_ERR(("%s - NEXUS_Security_AllocateKeySlot failed\n", __FUNCTION__));
        return NEXUS_NOT_AVAILABLE; 
    }

    if(pTest->use_CommonCryptoSetupKey){
        CommonCryptoKeySettings keySettings;

        CommonCrypto_GetDefaultKeySettings(&keySettings, CommonCrypto_eClearKey);
        keySettings.keySrc = CommonCrypto_eClearKey;
        keySettings.keySlot = keySlot;
        BKNI_Memcpy(&keySettings.src.keyIvInfo,  pTest->pKeyIvSettings, sizeof(CommonCryptoKeyIvSettings));
        keySettings.alg.keySlotType = pTest->keySlotType;
        keySettings.alg.opType = pTest->opType;
        keySettings.alg.algType =  pTest->algType;
        keySettings.alg.algVariant = pTest->algVariant;
        keySettings.alg.termMode = pTest->termMode;
        keySettings.alg.enableExtKey = pTest->enableExtKey;
        keySettings.alg.enableExtIv = pTest->enableExtIv;
        keySettings.alg.aesCounterSize = pTest->aesCounterSize;

        rc = CommonCrypto_SetupKey(handle, &keySettings);
        if(rc != NEXUS_SUCCESS){
            BDBG_ERR(("%s - CommonCrypto_SetupKey failed, rc = %d\n", __FUNCTION__, rc));
            goto errorExit;
        }
    }
    else {
        CommonCryptoKeyConfigSettings configSettings;

        CommonCrypto_GetDefaultKeyConfigSettings(&configSettings);
        configSettings. keySlot = keySlot;
        configSettings.settings.opType = pTest->opType;
        configSettings.settings.algType =  pTest->algType;
        configSettings.settings.algVariant = pTest->algVariant;
        configSettings.settings.keySlotType = pTest->keySlotType;
        configSettings.settings.termMode = pTest->termMode;
        configSettings.settings.enableExtKey = pTest->enableExtKey;
        configSettings.settings.enableExtIv = pTest->enableExtIv;
        configSettings.settings.aesCounterSize = pTest->aesCounterSize;

        rc = CommonCrypto_LoadKeyConfig(handle, &configSettings);
        if(rc != NEXUS_SUCCESS){
            BDBG_ERR(("%s - CommonCrypto_LoadKeyConfig failed, rc = %d\n", __FUNCTION__, rc));
            goto errorExit;
        }


        CommonCrypto_GetDefaultClearKeySettings(&keyIvSettings);
        keyIvSettings.keySlot = keySlot;
        keyIvSettings.keySlotType =pTest->keySlotType;
        /* Handle external key/iv cases */
        if(!pTest->enableExtKey){
            BKNI_Memcpy(keyIvSettings.settings.key, pTest->pKeyIvSettings->key, pTest->pKeyIvSettings->keySize);
            keyIvSettings.settings.keySize = pTest->pKeyIvSettings->keySize;
        }
        if(!pTest->enableExtIv){
            BKNI_Memcpy(keyIvSettings.settings.iv, pTest->pKeyIvSettings->iv,  pTest->pKeyIvSettings->ivSize);
            keyIvSettings.settings.ivSize = pTest->pKeyIvSettings->ivSize;
        }
        /* Load key and or IV */
        /*printf("keyIvSettings.settings.keySize %x, keyIvSettings.settings.ivSize %x\n", keyIvSettings.settings.keySize, keyIvSettings.settings.ivSize);*/
        if(keyIvSettings.settings.keySize != 0 || keyIvSettings.settings.ivSize != 0) {
            rc =  CommonCrypto_LoadClearKeyIv(handle, &keyIvSettings);
            if(rc != NEXUS_SUCCESS){
                BDBG_ERR(("%s - CommonCrypto_LoadClearKeyIv failed, rc = %d\n", __FUNCTION__, rc));
                goto errorExit;
            }
        }
    }

    rc = NEXUS_Memory_Allocate(16, NULL, (void *)&pBuf);
    if(rc != NEXUS_SUCCESS){
        BDBG_ERR(("%s - NEXUS_Memory_Allocate failed, rc = %d\n", __FUNCTION__, rc));
        goto errorExit;
    }

    if(pTest->opType == NEXUS_SecurityOperation_eEncrypt){
        BKNI_Memcpy(pBuf, pTest->pClear, 16);
        pResult = pTest->pCiphered;
    }
    else {
        /* Assume we are decryptin */
        BKNI_Memcpy(pBuf, pTest->pCiphered, 16);
        pResult = pTest->pClear;
    }

    for(ii = 0; ii < 3; ii++) {
        NEXUS_DmaJob_GetDefaultBlockSettings(&blks[ii]);
    }

    blks[0].scatterGatherCryptoStart = true;
    blks[0].resetCrypto = true;

    if(pTest->enableExtKey){

        rc = NEXUS_Memory_Allocate(pTest->pKeyIvSettings->keySize, NULL, (void *)&pKey);
        if(rc != NEXUS_SUCCESS){
            BDBG_ERR(("%s - NEXUS_Memory_Allocate failed, rc = %d\n", __FUNCTION__, rc));
            goto errorExit;
        }
        /* Copy key.  H and L need to be swapped */
        BKNI_Memcpy(pKey,  pTest->pKeyIvSettings->key + 8, 8);
        BKNI_Memcpy(pKey + 8, pTest->pKeyIvSettings->key, 8);

        /*  Set up block*/
        blks[nb_blks].pSrcAddr =  pKey;
        blks[nb_blks].pDestAddr = pKey;
        blks[nb_blks].blockSize = pTest->pKeyIvSettings->keySize;
        nb_blks++;
    }

    if(pTest->enableExtIv){
        rc = NEXUS_Memory_Allocate(pTest->pKeyIvSettings->ivSize, NULL, (void *)&pIv);
        if(rc != NEXUS_SUCCESS){
            BDBG_ERR(("%s - NEXUS_Memory_Allocate failed, rc = %d\n", __FUNCTION__, rc));
            goto errorExit;
        }

        /* Copy IV.  H and L need to be swapped */
        BKNI_Memcpy(pIv,  pTest->pKeyIvSettings->iv + 8, 8);
        BKNI_Memcpy(pIv + 8, pTest->pKeyIvSettings->iv, 8);

        /*  Set up block*/
        blks[nb_blks].pSrcAddr =  pIv;
        blks[nb_blks].pDestAddr = pIv;
        blks[nb_blks].blockSize = pTest->pKeyIvSettings->ivSize;
        nb_blks++;
    }

    blks[nb_blks].pSrcAddr =  pBuf;
    blks[nb_blks].pDestAddr = pBuf;
    blks[nb_blks].blockSize = 16;
    blks[nb_blks].scatterGatherCryptoEnd = true;
    nb_blks++;

    /*printf("pBuf (before) %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n", pBuf[0], pBuf[1], pBuf[2], pBuf[3], pBuf[4], pBuf[5], pBuf[6], pBuf[7], pBuf[8], pBuf[9], pBuf[10], pBuf[11], pBuf[12], pBuf[13], pBuf[14], pBuf[15]);*/
    CommonCrypto_GetDefaultJobSettings(&jobSettings);
    jobSettings.keySlot = keySlot;

    rc = CommonCrypto_DmaXfer(handle, &jobSettings, blks, nb_blks);
    if(rc != NEXUS_SUCCESS){
        BDBG_ERR(("%s - CommonCrypto_DmaXfer failed, rc = %d\n", __FUNCTION__, rc));
        goto errorExit;
    }

    /*printf("pBuf (after) %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n", pBuf[0], pBuf[1], pBuf[2], pBuf[3], pBuf[4], pBuf[5], pBuf[6], pBuf[7], pBuf[8], pBuf[9], pBuf[10], pBuf[11], pBuf[12], pBuf[13], pBuf[14], pBuf[15]);
    if(pKey != NULL) {
        printf("pKey         %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n", pKey[0], pKey[1], pKey[2], pKey[3], pKey[4], pKey[5], pKey[6], pKey[7], pKey[8], pKey[9], pKey[10], pKey[11], pKey[12], pKey[13], pKey[14], pKey[15]);
    }
    printf("pResult      %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n", pResult[0], pResult[1], pResult[2], pResult[3], pResult[4], pResult[5], pResult[6], pResult[7], pResult[8], pResult[9], pResult[10], pResult[11], pResult[12], pResult[13], pResult[14], pResult[15]);
    */
    if(BKNI_Memcmp(pBuf, pResult, 16) != 0){
        BDBG_ERR(("%s -  DMA operation failed\n", __FUNCTION__));
        rc = NEXUS_UNKNOWN;
    }

errorExit:
    if(keySlot) NEXUS_Security_FreeKeySlot(keySlot);
    if(pBuf) NEXUS_Memory_Free(pBuf);
    if(pKey) NEXUS_Memory_Free(pKey);
    if(pIv) NEXUS_Memory_Free(pIv);

    return rc;
}

int main(void)
{
    NEXUS_PlatformSettings platformSettings;
    CommonCryptoHandle handle;
    CommonCryptoSettings  cryptoSettings;
    int nb_tests= 0;
    NEXUS_Error rc = NEXUS_SUCCESS;
    int ii;
    int global_error_count = 0;

    /* Platform init */
    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);

    CommonCrypto_GetDefaultSettings(&cryptoSettings);
    handle = CommonCrypto_Open(&cryptoSettings);

    init_test_vectors(handle);

    nb_tests = sizeof(tests_clear_key) / sizeof(tests_clear_key[0]);
    printf("\n%s - Running clear key tests\n", __FUNCTION__);
    for(ii = 0; ii < nb_tests; ii++) 
    {
        printf("Clear key test no %2d:", ii);
        rc = run_test_clear_key(handle, &tests_clear_key[ii]);
        if(rc != NEXUS_SUCCESS)
        {
            printf("  clear key test FAILED (%u)\n", global_error_count);
            global_error_count++;
        }
        else{
            printf("  Succeeded\n");
        }
    }

    printf("\n%s - Running key ladder tests\n", __FUNCTION__);
    nb_tests = sizeof(tests_key_ladder) / sizeof(tests_key_ladder[0]);
    for(ii = 0; ii < nb_tests; ii++)
    {
        printf("key ladder test no %2d:", ii);
        rc = run_test_keyladder(handle, &tests_key_ladder[ii]);
        if(rc != NEXUS_SUCCESS)
        {
        	printf("  key ladder test FAILED (%u)\n", global_error_count);
            global_error_count++;
        }
        else{
            printf(" Succeeded\n");
        }
    }
    
    printf("\n");

    CommonCrypto_Close(handle);
    NEXUS_Platform_Uninit();

    return global_error_count;
}
