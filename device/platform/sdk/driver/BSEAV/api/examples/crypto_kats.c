/***************************************************************************
 *     (c)2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: crypto_kats.c $
 * $brcm_Revision: 14 $
 * $brcm_Date: 9/6/12 6:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/crypto_kats.c $
 * 
 * 14   9/6/12 6:06p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include <stdio.h>
#include "bsettop.h"
#include "bsettop_crypto.h"

/* settop api CRYPTO Known Answer Test Sample */
#define TEST_DES_ECB
#define TEST_DES_CBC
#define TEST_3DES_ECB_MCT
#define TEST_AES_ECB
#define TEST_AES_ECB_MCT
#define TEST_AES_CTR
#define TEST_RC4
#define TEST_MD5
#define TEST_SHA1
#define TEST_CMAC
#define TEST_RSA
#define TEST_HMAC_SHA1

/* Compile time options: */
/*#define SWAP_BUFFER*/      /* swap the buffers before and after processing */
/*#define SendErrsToStdErr*/ /* print something on standard error output to see which operation is causing 'berr_not_available' messages */

#ifdef SendErrsToStdErr
#define outputChannel stderr
#else
#define outputChannel stdout
#endif

typedef enum { eNever, eOnError, eAlways } when;
static when WHEN = eOnError; /* this defines when we'll dump (print) the output buffers.
                                Change this to 'eAlways' to print results after each test! */

static void dump(char *header, uint8_t *buf, int len, when w)
{
#if 1
    int i;
    if (w != eNever && w == WHEN) {
        fprintf(outputChannel, "%s: ", header);
        for (i=0; i < len; i++ ) fprintf(outputChannel, "%02x ", buf[i]);
        fprintf(outputChannel, "\n");
    }
#endif
}

#define PASSES 3 /* this tests the three options for control mode (passed as setting) */
typedef enum {
    eDefault,  /* this lets the hardware decide */
    eSoftware, /* perform the operation using software */
    eHardware  /* perform the operation using hardware */
} testType;
static char *pass[PASSES] = { "default", "software", "hardware" };

bcrypto_hardware_ctrl setCtrlMode(int test)
{
    switch (test) {
        /* on default pass (0), we don't set the mode */
        case eSoftware: return bcrypto_hardware_none;
        case eHardware: return bcrypto_hardware_m2m;
    }
}

#ifdef SWAP_BUFFER
static void swap_buffer(uint8_t *buf, int len)
{
    int i;
    for (i=0; i<len-3; i+=4) {
        uint8_t t;
        t = buf[i+0]; buf[i+0] = buf[i+3]; buf[i+3] = t;
        t = buf[i+1]; buf[i+1] = buf[i+2]; buf[i+2] = t;
    }
}
#endif

/*\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\*/

int main(void)
{
    int test; /* loop counter for tests below */
	bresult rc = bsettop_init(BSETTOP_VERSION);
#if 0
	bdecode_t decode = bdecode_open(B_ID(0));
	bdisplay_t display = bdisplay_open(B_ID(0));
	bdecode_window_t window = bdecode_window_open(B_ID(0),display);
#endif

	if (rc != b_ok) {
        fprintf(outputChannel, "settop failed to init!\n");
        return -1;
    }

    {
        /* First, check if we can successfully allocate memory for crypto functions */
        uint8_t *dma_test;
        dma_test = bcrypto_alloc(24);
        if (!dma_test) {
            fprintf(outputChannel, "Unable to allocate DMA memory for M2M tests.\n");
            fprintf(outputChannel, "Please build settopapi with BCRYPTO_SUPPORT=y\n");
            return -1;
        }
        bcrypto_free(dma_test);
    }

	printf("Performing tests ...\n");
#if 1
    for (test=0; test < PASSES; test++)
#else
    test = eSoftware; /* just do a specific test */
#endif
    {
        printf("\n********************   Performing %s tests  ********************\n", pass[test]);
        
#ifdef TEST_DES_ECB
        /* FIPS 81 standard DES Known Answer Tests */
        {
            bcrypto_t crypt;
            bcrypto_settings settings;
            uint8_t des_in[24] = "Now is the time for all ";
            uint8_t des_out[24] = {0};
#if 0
            uint8_t des_ecb_key[8] = {0}; /* use this to test out zero key test */
#else
            uint8_t des_ecb_key[8] = {0x01, 0x23, 0x45, 0x67,  0x89, 0xab, 0xcd, 0xef};
#endif
            uint8_t des_ecb_kat[24] = {
                0x3f, 0xa4, 0x0e, 0x8a, 0x98, 0x4d, 0x48, 0x15, 0x6a, 0x27, 0x17, 0x87,
                0xab, 0x88, 0x83, 0xf9, 0x89, 0x3d, 0x51, 0xec, 0x4b, 0x56, 0x3b, 0x53 };
            uint8_t des_ecb_zerokey_kat[24] = {
                0x7d, 0x65, 0x48, 0x4b, 0xe5, 0x78, 0xdd, 0x0d, 0xbb, 0x70, 0x39, 0xa9,
                0xc2, 0x37, 0xb4, 0xf4, 0x4e, 0x69, 0x30, 0x77, 0x0d, 0x3a, 0x7f, 0xac };

            uint8_t *dma_des_in  = bcrypto_alloc(24);
            uint8_t *dma_des_out = bcrypto_alloc(24);

#if 0
            memset(dma_des_in, 0, 24); /* use this to see if this changes the output
                                          i.e. is it reading in this data or something else? */
#else
            memcpy(dma_des_in, des_in, 24);
#endif
            memcpy(dma_des_out, des_out, 24);
        
            bcrypto_settings_init(&settings);
            settings.encryption.type = bencryption_type_des;
            memcpy(settings.encryption.key, des_ecb_key, sizeof(des_ecb_key));
            settings.encryption.key_length = 64;
            settings.operation = bcrypto_operation_encrypt;
            settings.encryption.blockmode = bcrypto_blockmode_ecb;

            if (test) settings.hardware_ctrl = setCtrlMode(test);

            crypt = bcrypto_open(&settings);
	
#ifdef SWAP_BUFFER
            /* What if I swap the buffer myself -- is the hardware swapping it correctly??? */
            if (test == 2) swap_buffer(dma_des_in, 24);
#endif
        
            if (bcrypto_process(crypt, dma_des_in, 24, dma_des_out, 24)) {
                printf("DES ECB (%s) failed crypto process\n", pass[test]);
            }

#ifdef SWAP_BUFFER
            if (test == 2) swap_buffer(dma_des_out, 24);
#endif
        
            if (0 == memcmp(dma_des_out, des_ecb_kat, 24)) {
                printf("DES ECB (%s) SUCCESS!\n", pass[test]);
            } else {
                if (0 == memcmp(dma_des_out, des_ecb_zerokey_kat, 24))
                    fprintf(outputChannel, "DES ECB (%s) used an all-zero key!  (key loading probably failed)\n", pass[test]);
                else
                    fprintf(outputChannel, "DES ECB (%s) failed to encrypt correctly!\n", pass[test]);
                dump("IS", dma_des_out, 24, eOnError);
                dump("SB", des_ecb_kat, 24, eOnError);
            }

            dump("IS", dma_des_out, 24, eAlways);
            dump("SB", des_ecb_kat, 24, eAlways);
            bcrypto_close(crypt);

#if 0
            /* Just for grins, try decrypting what we got back using the same key -- what comes out? */
            settings.operation = bcrypto_operation_decrypt;
            crypt = bcrypto_open(&settings);
            if (bcrypto_process(crypt, dma_des_out, 24, dma_des_in, 24)) {
                printf("DES ECB (%s) failed crypto process\n", pass[test]);
            } else if (0 == memcmp(dma_des_in, des_in, 24)) {
                printf("DES ECB (%s) decrypt SUCCESS!\n", pass[test]);
            } else {
                fprintf(outputChannel, "DES ECB (%s) failed to decrypt correctly!\n", pass[test]);
                dump("IS", dma_des_in, 24, eOnError);
                dump("SB", des_in, 24, eOnError);
            }
            dump("IS", dma_des_in, 24, eAlways);
            dump("SB", des_in, 24, eAlways);
            bcrypto_close(crypt);
#endif

            bcrypto_free(dma_des_in);
            bcrypto_free(dma_des_out);
        }
#endif /* TEST_DES_ECB */

#ifdef TEST_DES_CBC
        /* FIPS 81 standard DES Known Answer Tests */
#if (BCHP_CHIP == 7400) && (BCHP_VER == A0)
        if (test != eSoftware) /* only run the software test */
            printf("DES CBC tests not supported by %s on this platform -- skipping test!\n", pass[test]);
        else
#endif
        {
            bcrypto_t crypt;
            bcrypto_settings settings;
            uint8_t des_in[24] = "Now is the time for all ";
            uint8_t des_out[24] = {0};
            uint8_t des_cbc_key[8] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};
            uint8_t des_cbc_iv[8] = {0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef};
            uint8_t des_cbc_kat[24] = {
                0xe5, 0xc7, 0xcd, 0xde, 0x87, 0x2b, 0xf2, 0x7c, 0x43, 0xe9, 0x34, 0x00,
                0x8c, 0x38, 0x9c, 0x0f, 0x68, 0x37, 0x88, 0x49, 0x9a, 0x7c, 0x05, 0xf6 };

            uint8_t *dma_des_in  = bcrypto_alloc(24);
            uint8_t *dma_des_out = bcrypto_alloc(24);
            memcpy(dma_des_in, des_in, 24);
            memcpy(dma_des_out, des_out, 24);

            bcrypto_settings_init(&settings);
            settings.encryption.type = bencryption_type_des;
            memcpy(settings.encryption.key, des_cbc_key, sizeof(des_cbc_key));
            memcpy(settings.encryption.iv, des_cbc_iv, sizeof(des_cbc_iv));
            settings.encryption.key_length = 64;
            settings.operation = bcrypto_operation_encrypt;
            settings.encryption.blockmode = bcrypto_blockmode_cbc;

            if (test) settings.hardware_ctrl = setCtrlMode(test);

            crypt = bcrypto_open(&settings);
	
            if (bcrypto_process(crypt, dma_des_in, 24, dma_des_out, 24)) {
                printf("DES CBC (%s) failed crypto process\n", pass[test]);
            } else if (0 != memcmp(dma_des_out, des_cbc_kat, 24)) {
                fprintf(outputChannel, "DES CBC (%s) failed to encrypt correctly!\n", pass[test]);
                dump("IS", dma_des_out, 24, eOnError);
                dump("SB", des_cbc_kat, 24, eOnError);
            } else
                printf("DES CBC (%s) SUCCESS!\n", pass[test]);

            dump("IS", dma_des_out, 24, eAlways);
            dump("SB", des_cbc_kat, 24, eAlways);

            bcrypto_close(crypt);
            bcrypto_free(dma_des_in);
            bcrypto_free(dma_des_out);
        }
#endif /* TEST_DES_CBC */

#ifdef TEST_3DES_ECB_MCT
        /* FIPS 81 standard DES Known Answer Test */
        if (test == eSoftware) /* don't run the software test */
            printf("T-DES ECB tests not supported by %s on this platform -- skipping test!\n", pass[test]);
        else
        {
            int i;
            bcrypto_t crypt;
            bcrypto_settings settings;
            uint8_t tdes_in[8] = {0x4e, 0x6f, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74};
            uint8_t tdes_out[8] = {0};
            uint8_t tdes_ecb_key[16] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01};
#if 0
            uint8_t tdes_ecb_key[16] = {0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01, 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};
#endif
            uint8_t tdes_ecb_kat[8] = {0x03, 0xe6, 0x9f, 0x5b, 0xfa, 0x58, 0xeb, 0x42 };

            uint8_t *dma_tdes_in  = bcrypto_alloc(8);
            uint8_t *dma_tdes_out = bcrypto_alloc(8);
            memcpy(dma_tdes_in, tdes_in, 8);
            memcpy(dma_tdes_out, tdes_out, 8);

            bcrypto_settings_init(&settings);
            settings.encryption.type = bencryption_type_3des;
            memcpy(settings.encryption.key, tdes_ecb_key, sizeof(tdes_ecb_key));
            settings.encryption.key_length = 128;
            settings.operation = bcrypto_operation_encrypt;
            settings.encryption.blockmode = bcrypto_blockmode_ecb;
	
            if (test) settings.hardware_ctrl = setCtrlMode(test);

            crypt = bcrypto_open(&settings);
	
#if 0
            bcrypto_process(crypt, dma_tdes_in, 8, dma_tdes_out, 8);
#endif
            /* Monte Carlo Test -- 10k iterations... */
            for (i=0; i < 10000; i++) {
                if (bcrypto_process(crypt, dma_tdes_in, 8, dma_tdes_out, 8)) {
                    printf("T-DES ECB (%s) failed crypto process on pass %d", pass[test], i);
                    break;
                }
                memcpy(dma_tdes_in, dma_tdes_out, 8);
                if (i % 100 == 0) {
                    printf("."); fflush(stdout);
                }
            }
            printf("\n");

            if (0 != memcmp(dma_tdes_out, tdes_ecb_kat, 8)) {
                fprintf(outputChannel, "T-DES ECB (%s) failed to encrypt correctly!\n", pass[test]);
                dump("IS", dma_tdes_out, 8, eOnError);
                dump("SB", tdes_ecb_kat, 8, eOnError);
            } else
                printf("T-DES ECB (%s) SUCCESS!\n", pass[test]);

            dump("IS", dma_tdes_out, 8, eAlways);
            dump("SB", tdes_ecb_kat, 8, eAlways);

            bcrypto_close(crypt);
            bcrypto_free(dma_tdes_in);
            bcrypto_free(dma_tdes_out);
        }
#endif /* TEST_3DES_ECB_MCT */

#ifdef TEST_AES_ECB
        /* FIPS 197 AES example */
        {
            bcrypto_t crypt;
            bcrypto_settings settings;
            uint8_t aes_in[16] = {0x50, 0x68, 0x12, 0xA4, 0x5F, 0x08, 0xC8, 0x89, 0xB9, 0x7F, 0x59, 0x80, 0x03, 0x8B, 0x83, 0x59};
            uint8_t aes_out[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
            uint8_t aes_ecb_key[16] = {0x00, 0x01, 0x02, 0x03, 0x05, 0x06, 0x07, 0x08, 0x0A, 0x0B, 0x0C, 0x0D, 0x0F, 0x10, 0x11, 0x12};
            uint8_t aes_ecb_kat[16] = {0xD8, 0xF5, 0x32, 0x53, 0x82, 0x89, 0xEF, 0x7D, 0x06, 0xB5, 0x06, 0xA4, 0xFD, 0x5B, 0xE9, 0xC9};

            uint8_t *dma_aes_in  = bcrypto_alloc(16);
            uint8_t *dma_aes_out = bcrypto_alloc(16);
            memcpy(dma_aes_in, aes_in, 16);
            memcpy(dma_aes_out, aes_out, 16);

            bcrypto_settings_init(&settings);
            settings.encryption.type = bencryption_type_aes;
            memcpy(settings.encryption.key, aes_ecb_key, sizeof(aes_ecb_key));
            settings.encryption.key_length = 128;
            settings.operation = bcrypto_operation_encrypt;
            settings.encryption.blockmode = bcrypto_blockmode_ecb;

            if (test) settings.hardware_ctrl = setCtrlMode(test);
	
            crypt = bcrypto_open(&settings);
            
            if (bcrypto_process(crypt, dma_aes_in, 16, dma_aes_out, 16)) {
                printf("AES ECB (%s) failed crypto process\n", pass[test]);
            } else if (0 != memcmp(dma_aes_out, aes_ecb_kat, 16)) {
                fprintf(outputChannel, "AES ECB (%s) failed to encrypt correctly!\n", pass[test]);
                dump("IS", dma_aes_out, 16, eOnError);
                dump("SB", aes_ecb_kat, 16, eOnError);
            } else
                printf("AES ECB (%s) SUCCESS!\n", pass[test]);

            dump("IS", dma_aes_out, 16, eAlways);
            dump("SB", aes_ecb_kat, 16, eAlways);

            bcrypto_close(crypt);
            bcrypto_free(dma_aes_in);
            bcrypto_free(dma_aes_out);
        }
#endif /* TEST_AES_ECB */

#ifdef TEST_AES_ECB_MCT
        /* FIPS 197 AES example */
        {
            int i;
            bcrypto_t crypt;
            bcrypto_settings settings;
            uint8_t aes_in[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
            uint8_t aes_out[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
            uint8_t aes_ecb_key[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
            uint8_t aes_ecb_kat[16] = {0xC3, 0x4C, 0x05, 0x2C, 0xC0, 0xDA, 0x8D, 0x73, 0x45, 0x1A, 0xFE, 0x5F, 0x03, 0xBE, 0x29, 0x7F};

            uint8_t *dma_aes_in  = bcrypto_alloc(16);
            uint8_t *dma_aes_out = bcrypto_alloc(16);
            memcpy(dma_aes_in, aes_in, 16);
            memcpy(dma_aes_out, aes_out, 16);

            bcrypto_settings_init(&settings);
            settings.encryption.type = bencryption_type_aes;
            memcpy(settings.encryption.key, aes_ecb_key, sizeof(aes_ecb_key));
            settings.encryption.key_length = 128;
            settings.operation = bcrypto_operation_encrypt;
            settings.encryption.blockmode = bcrypto_blockmode_ecb;

            if (test) settings.hardware_ctrl = setCtrlMode(test);
	
            crypt = bcrypto_open(&settings);

            /* Monte Carlo Test -- 10k iterations... */
            for (i=0; i < 10000; i++) {
                if (bcrypto_process(crypt, dma_aes_in, 16, dma_aes_out, 16)) {
                    printf("AES ECB (%s) Monte Carlo failed crypto process on pass %d", pass[test], i);
                    break;
                }
                memcpy(dma_aes_in, dma_aes_out, 16);
                if (i % 100 == 0) {
                    printf("."); fflush(stdout);
                }
            }
            printf("\n");

            if (0 != memcmp(dma_aes_out, aes_ecb_kat, 16)) {
                fprintf(outputChannel, "AES ECB (%s) Monte Carlo failed to encrypt correctly!\n", pass[test]);
                dump("IS", dma_aes_out, 16, eOnError);
                dump("SB", aes_ecb_kat, 16, eOnError);
            } else
                printf("AES ECB (%s) Monte Carlo SUCCESS!\n", pass[test]);

            dump("IS", dma_aes_out, 16, eAlways);
            dump("SB", aes_ecb_kat, 16, eAlways);

            bcrypto_close(crypt);
            bcrypto_free(dma_aes_in);
            bcrypto_free(dma_aes_out);
        }
#endif /* TEST_AES_ECB_MCT */

#if (BCHP_CHIP == 7401)
#ifdef TEST_AES_CTR
        /* Ali's test vector */
        {
            bcrypto_t crypt;
            bcrypto_settings settings;
            uint8_t aes_in[160] = {
                0x32, 0x43, 0xF6, 0xA8, 0x88, 0x5A, 0x30, 0x8D, 0x31, 0x31, 0x98, 0xA2, 0xE0, 0x37, 0x07, 0x34, 
                0x6A, 0x78, 0x4D, 0x90, 0x45, 0x19, 0x0C, 0xFE, 0x76, 0x2E, 0x71, 0x60, 0xF3, 0x8B, 0x4D, 0xA5, 
                0x32, 0x43, 0xF6, 0xA8, 0x88, 0x5A, 0x30, 0x8D, 0x31, 0x31, 0x98, 0xA2, 0xE0, 0x37, 0x07, 0x34, 
                0x6A, 0x78, 0x4D, 0x90, 0x45, 0x19, 0x0C, 0xFE, 0x76, 0x2E, 0x71, 0x60, 0xF3, 0x8B, 0x4D, 0xA5, 
                0xCF, 0x94, 0xEA, 0xEB, 0x1E, 0x66, 0xF6, 0x29, 0xA1, 0x18, 0xBE, 0x79, 0x80, 0x28, 0x55, 0xB3, 
                0x09, 0x4B, 0x4F, 0x0B, 0x98, 0x3A, 0x32, 0x1C, 0xF8, 0x6B, 0xC7, 0xBB, 0xCF, 0xC2, 0x69, 0x65, 
                0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C, 
                0x6A, 0x78, 0x4D, 0x90, 0x45, 0x19, 0x0C, 0xFE, 0x76, 0x2E, 0x71, 0x60, 0xF3, 0x8B, 0x4D, 0xA5, 
                0x31, 0xA5, 0xB6, 0x69, 0x6E, 0x2A, 0x49, 0x3B, 0x9A, 0xD0, 0x3A, 0x5C, 0xC2, 0xD9, 0xFB, 0x00, 
                0x5D, 0xAC, 0x02, 0xBC, 0xDE, 0x4F, 0x03, 0x23, 0x78, 0xF4, 0x16, 0x93, 0x07, 0xC5, 0x47, 0x35 };
            uint8_t aes_out[160] = {0};
            uint8_t aes_ctr_key[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
            uint8_t aes_ctr_iv[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
            uint8_t aes_ctr_kat[160] = {
                0xBF, 0xB7, 0x1F, 0x02, 0x4D, 0x9D, 0x67, 0xB7, 0x16, 0xE9, 0x48, 0xF7, 0x36, 0xD3, 0xD1, 0x7F, 
                0xAE, 0xCC, 0x3C, 0xB3, 0xA2, 0x27, 0xA1, 0x04, 0x83, 0x26, 0x7D, 0x02, 0x37, 0x86, 0x33, 0xD4, 
                0x40, 0x27, 0xB0, 0x80, 0xCB, 0x7A, 0x72, 0xC3, 0xAC, 0xCC, 0x7B, 0x17, 0xA1, 0x5A, 0xE6, 0x8F, 
                0xF0, 0x3B, 0x9A, 0x3A, 0xCE, 0x11, 0x7E, 0xBB, 0x59, 0x4C, 0xAE, 0xC8, 0x7E, 0x3E, 0x51, 0x0B, 
                0xA5, 0x79, 0x10, 0xB1, 0x4E, 0xA4, 0x6B, 0x0C, 0xAF, 0x1D, 0x26, 0xB1, 0xD9, 0x05, 0x59, 0x71, 
                0x0D, 0x01, 0xEC, 0x34, 0x32, 0x0E, 0xDA, 0x9B, 0x04, 0x3C, 0x92, 0x6B, 0x4F, 0x24, 0x46, 0xBA, 
                0x3E, 0xF5, 0x6A, 0xD3, 0x89, 0xBF, 0x7E, 0x7E, 0xDA, 0x47, 0x12, 0x94, 0xF0, 0x03, 0x67, 0x38, 
                0xB0, 0x36, 0x55, 0x9E, 0x5B, 0x5D, 0x6D, 0x31, 0xCE, 0xE9, 0xBB, 0xAE, 0xA8, 0x44, 0x76, 0x83, 
                0x69, 0x57, 0xB0, 0x42, 0x78, 0x2E, 0xAE, 0xC7, 0x8C, 0xA4, 0x38, 0x9D, 0xB0, 0x73, 0x64, 0x5F, 
                0x3F, 0x1D, 0xF4, 0x0F, 0x6C, 0x94, 0x81, 0xDA, 0xDF, 0x52, 0x8A, 0x63, 0x30, 0x80, 0x92, 0x44 };

            uint8_t *dma_aes_in  = bcrypto_alloc(160);
            uint8_t *dma_aes_out = bcrypto_alloc(160);
            memcpy(dma_aes_in, aes_in, 160);
            memcpy(dma_aes_out, aes_out, 160);

            bcrypto_settings_init(&settings);
            settings.encryption.type = bencryption_type_aes;
            memcpy(settings.encryption.key, aes_ctr_key, sizeof(aes_ctr_key));
            memcpy(settings.encryption.iv, aes_ctr_iv, sizeof(aes_ctr_iv));
            settings.encryption.key_length = 128;
            settings.operation = bcrypto_operation_encrypt;
            settings.encryption.blockmode = bcrypto_blockmode_ctr;

            if (test) settings.hardware_ctrl = setCtrlMode(test);
	
            crypt = bcrypto_open(&settings);

            if (bcrypto_process(crypt, dma_aes_in, 160, dma_aes_out, 160)) {
                printf("AES CTR (%s) failed crypto process\n", pass[test]);
            } else if (0 != memcmp(dma_aes_out, aes_ctr_kat, 160)) {
                fprintf(outputChannel, "AES CTR 1 (%s) failed to encrypt correctly!\n", pass[test]);
                dump("IS", dma_aes_out, 160, eOnError);
                dump("SB", aes_ctr_kat, 160, eOnError);
            } else
                printf("AES CTR 1 (%s) SUCCESS!\n", pass[test]);

            dump("IS", dma_aes_out, 160, eAlways);
            dump("SB", aes_ctr_kat, 160, eAlways);

            bcrypto_close(crypt);
            bcrypto_free(dma_aes_in);
            bcrypto_free(dma_aes_out);
        }

        /* WM DRM AES CTR test vector */
        {
            bcrypto_t crypt;
            bcrypto_settings settings;
            uint8_t aes_in[48] = {
                0x95, 0xdd, 0xad, 0xbf, 0x43, 0x03, 0xa6, 0x05, 0xe7, 0x95, 0xbf, 0x06,
                0x18, 0x46, 0xeb, 0xaf, 0xff, 0x2a, 0xfe, 0xfa, 0xa7, 0x64, 0x4e, 0x19,
                0x94, 0x1d, 0xf4, 0x52, 0x43, 0x0a, 0xf7, 0x03, 0x87, 0x9c, 0xd5, 0x35,
                0xc3, 0x77, 0x09, 0x1a, 0x2e, 0x7c, 0xfe, 0x8e, 0x86, 0x6e, 0x94, 0x45 };
            uint8_t aes_out[48] = {0};
            uint8_t aes_ctr_key[16] = {0x60, 0x09, 0x5e, 0xcd, 0xf6, 0x1e, 0xa4, 0x6b, 0xac, 0x69, 0xc4, 0xab, 0x32, 0x5d, 0x72, 0x08};
            uint8_t aes_ctr_iv[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
            uint8_t aes_ctr_kat[48] = {
                0xaa, 0x9b, 0x36, 0x83, 0xeb, 0x9d, 0xee, 0xab, 0x06, 0xbe, 0xb3, 0x63,
                0xa3, 0x90, 0x84, 0xee, 0x29, 0x3c, 0xf9, 0x91, 0xf3, 0x7b, 0x59, 0xd5,
                0x63, 0x9e, 0x50, 0xeb, 0x1f, 0x70, 0x07, 0x3a, 0xeb, 0x38, 0x50, 0x9d,
                0xa4, 0x30, 0x6a, 0x8c, 0x94, 0x04, 0x13, 0x96, 0x53, 0x7d, 0x4b, 0xb7 };

            uint8_t *dma_aes_in  = bcrypto_alloc(48);
            uint8_t *dma_aes_out = bcrypto_alloc(48);
            memcpy(dma_aes_in, aes_in, 48);
            memcpy(dma_aes_out, aes_out, 48);

            bcrypto_settings_init(&settings);
            settings.encryption.type = bencryption_type_aes;
            memcpy(settings.encryption.key, aes_ctr_key, sizeof(aes_ctr_key));
            memcpy(settings.encryption.iv, aes_ctr_iv, sizeof(aes_ctr_iv));
            settings.encryption.key_length = 128;
            settings.operation = bcrypto_operation_encrypt;
            settings.encryption.blockmode = bcrypto_blockmode_ctr;

            if (test) settings.hardware_ctrl = setCtrlMode(test);
	
            crypt = bcrypto_open(&settings);

            if (bcrypto_process(crypt, dma_aes_in, 48, dma_aes_out, 48)) {
                printf("AES CTR 2 (%s) failed crypto process\n", pass[test]);
            } else if (0 != memcmp(dma_aes_out, aes_ctr_kat, 48)) {
                fprintf(outputChannel, "AES CTR 2 (%s) failed to encrypt correctly!\n", pass[test]);
                dump("IS", dma_aes_out, 48, eOnError);
                dump("SB", aes_ctr_kat, 48, eOnError);
            } else
                printf("AES CTR 2 (%s) SUCCESS!\n", pass[test]);

            dump("IS", dma_aes_out, 48, eAlways);
            dump("SB", aes_ctr_kat, 48, eAlways);

            bcrypto_close(crypt);
            bcrypto_free(dma_aes_in);
            bcrypto_free(dma_aes_out);
        }
#endif /* TEST_AES_CTR */
#endif /* BCHP_CHIP == 7401 */

#ifdef TEST_RC4
/* RC4 sample vector tests taken from http://en.wikipedia.org/wiki/RC4:
   RC4( "Key", "Plaintext" ) == "bbf316e8 d940af0a d3"
   RC4( "Wiki", "pedia" ) == "1021bf0420"
   RC4( "Secret", "Attack at dawn" ) == "45a01f64 5fc35b38 3552544b 9bf5"
*/
        { /* test 1 */
            unsigned i;
            bcrypto_t crypt;
            bcrypto_settings settings;
            uint8_t rc4_key[3] = "Key";
            uint8_t rc4_in[9] = "Plaintext";
            uint8_t rc4_out[9] = {0};
            uint8_t rc4_kat[9] = {0xbb, 0xf3, 0x16, 0xe8, 0xd9, 0x40, 0xaf, 0x0a, 0xd3};
	
            uint8_t *dma_rc4_in  = bcrypto_alloc(9);
            uint8_t *dma_rc4_out = bcrypto_alloc(9);
            memcpy(dma_rc4_in, rc4_in, 9);
            memcpy(dma_rc4_out, rc4_out, 9);

            bcrypto_settings_init(&settings);
            settings.encryption.type = bencryption_type_rc4;
            memcpy(settings.encryption.key, rc4_key, sizeof(rc4_key));
            settings.encryption.key_length = sizeof(rc4_key); /* RC4 is weird, it's in bytes, not bits */
            settings.operation = bcrypto_operation_encrypt;

            if (test) settings.hardware_ctrl = setCtrlMode(test);
	
            crypt = bcrypto_open(&settings);
	
            if (bcrypto_process(crypt, dma_rc4_in, 9, dma_rc4_out, 9)) {
                printf("RC4 (test 1 - %s) failed crypto process\n", pass[test]);
            } else if (0 != memcmp(dma_rc4_out, rc4_kat, 9)) {
                fprintf(outputChannel, "RC4 (test 1 - %s) failed to encrypt correctly!\n", pass[test]);
                dump("IS", dma_rc4_out, 9, eOnError);
                dump("SB", rc4_kat, 9, eOnError);
            } else
                printf("RC4 (test 1 - %s) SUCCESS!\n", pass[test]);

            dump("IS", dma_rc4_out, 9, eAlways);
            dump("SB", rc4_kat, 9, eAlways);

            bcrypto_close(crypt);
            bcrypto_free(dma_rc4_in);
            bcrypto_free(dma_rc4_out);
        }
        { /* test 2 */
            unsigned i;
            bcrypto_t crypt;
            bcrypto_settings settings;
            uint8_t rc4_in[5] = "pedia";
            uint8_t rc4_out[5] = {0};
            uint8_t rc4_key[4] = "Wiki";
            uint8_t rc4_kat[5] = {0x10, 0x21, 0xbf, 0x04, 0x20};
	
            uint8_t *dma_rc4_in  = bcrypto_alloc(5);
            uint8_t *dma_rc4_out = bcrypto_alloc(5);
            memcpy(dma_rc4_in, rc4_in, 5);
            memcpy(dma_rc4_out, rc4_out, 5);

            bcrypto_settings_init(&settings);
            settings.encryption.type = bencryption_type_rc4;
            memcpy(settings.encryption.key, rc4_key, sizeof(rc4_key));
            settings.encryption.key_length = sizeof(rc4_key); /* RC4 is weird, it's in bytes, not bits */
            settings.operation = bcrypto_operation_encrypt;
	
            if (test) settings.hardware_ctrl = setCtrlMode(test);

            crypt = bcrypto_open(&settings);
	
            if (bcrypto_process(crypt, dma_rc4_in, 5, dma_rc4_out, 5)) {
                printf("RC4 (test 2 - %s) failed crypto process\n", pass[test]);
            } else if (0 != memcmp(dma_rc4_out, rc4_kat, 5)) {
                fprintf(outputChannel, "RC4 (test 2 - %s) failed to encrypt correctly!\n", pass[test]);
                dump("IS", dma_rc4_out, 5, eOnError);
                dump("SB", rc4_kat, 5, eOnError);
            } else
                printf("RC4 (test 2 - %s) SUCCESS!\n", pass[test]);

            dump("IS", dma_rc4_out, 5, eAlways);
            dump("SB", rc4_kat, 5, eAlways);

            bcrypto_close(crypt);
            bcrypto_free(dma_rc4_in);
            bcrypto_free(dma_rc4_out);
        }
        { /* test 3 */
            unsigned i;
            bcrypto_t crypt;
            bcrypto_settings settings;
            uint8_t rc4_in[14] = "Attack at dawn";
            uint8_t rc4_out[14] = {0};
            uint8_t rc4_key[6] = "Secret";
            uint8_t rc4_kat[14] = {0x45, 0xa0, 0x1f, 0x64, 0x5f, 0xc3, 0x5b, 0x38, 0x35, 0x52, 0x54, 0x4b, 0x9b, 0xf5};
	
            uint8_t *dma_rc4_in  = bcrypto_alloc(14);
            uint8_t *dma_rc4_out = bcrypto_alloc(14);
            memcpy(dma_rc4_in, rc4_in, 14);
            memcpy(dma_rc4_out, rc4_out, 14);

            bcrypto_settings_init(&settings);
            settings.encryption.type = bencryption_type_rc4;
            memcpy(settings.encryption.key, rc4_key, sizeof(rc4_key));
            settings.encryption.key_length = sizeof(rc4_key); /* RC4 is weird, it's in bytes, not bits */
            settings.operation = bcrypto_operation_encrypt;
	
            if (test) settings.hardware_ctrl = setCtrlMode(test);

            crypt = bcrypto_open(&settings);
	
            if (bcrypto_process(crypt, dma_rc4_in, 14, dma_rc4_out, 14)) {
                printf("RC4 (test 3 - %s) failed crypto process\n", pass[test]);
            } else if (0 != memcmp(dma_rc4_out, rc4_kat, 14)) {
                fprintf(outputChannel, "RC4 (test 3 - %s) failed to encrypt correctly!\n", pass[test]);
                dump("IS", dma_rc4_out, 14, eOnError);
                dump("SB", rc4_kat, 14, eOnError);
            } else
                printf("RC4 (test 3 - %s) SUCCESS!\n", pass[test]);

            dump("IS", dma_rc4_out, 14, eAlways);
            dump("SB", rc4_kat, 14, eAlways);

            bcrypto_close(crypt);
            bcrypto_free(dma_rc4_in);
            bcrypto_free(dma_rc4_out);
        }
#endif /* TEST_RC4 */

#ifdef TEST_SHA1
/* SHA1 sample test taken from http://en.wikipedia.org/wiki/SHA_hash_functions:
   SHA1("The quick brown fox jumps over the lazy dog") = 2fd4e1c67a2d28fced849ee1bb76e7391b93eb12
*/
        { /* Test 1: test vector tested all at once */
            unsigned i;
            bcrypto_t crypt;
            bcrypto_settings settings;
            uint8_t sha1_in[43] = "The quick brown fox jumps over the lazy dog";
            uint8_t sha1_out[20] = {0};
            uint8_t sha1_kat[20] = {
                0x2f, 0xd4, 0xe1, 0xc6, 0x7a, 0x2d, 0x28, 0xfc, 0xed, 0x84,
                0x9e, 0xe1, 0xbb, 0x76, 0xe7, 0x39, 0x1b, 0x93, 0xeb, 0x12 };
	
            uint8_t *dma_sha1_in  = bcrypto_alloc(43);
            uint8_t *dma_sha1_out = bcrypto_alloc(20);
            memcpy(dma_sha1_in, sha1_in, 43);
            memcpy(dma_sha1_out, sha1_out, 20);

            bcrypto_settings_init(&settings);
            settings.encryption.type = bencryption_type_sha1;
            settings.operation = bcrypto_operation_hash;
            settings.multiple_hashes = false;
	
            if (test) settings.hardware_ctrl = setCtrlMode(test);

            crypt = bcrypto_open(&settings);
	
            if (bcrypto_process(crypt, dma_sha1_in, 43, dma_sha1_out, 20)) {
                printf("SHA1 (test 1 - %s) failed crypto process\n", pass[test]);
            } else if (0 != memcmp(dma_sha1_out, sha1_kat, 20)) {
                fprintf(outputChannel, "SHA1 (test 1 - %s) failed to hash correctly!\n", pass[test]);
                dump("IS", dma_sha1_out, 20, eOnError);
                dump("SB", sha1_kat, 20, eOnError);
            } else
                printf("SHA1 (test 1 - %s) SUCCESS!\n", pass[test]);

            dump("IS", dma_sha1_out, 20, eAlways);
            dump("SB", sha1_kat, 20, eAlways);

            bcrypto_close(crypt);
            bcrypto_free(dma_sha1_in);
            bcrypto_free(dma_sha1_out);
        }

        { /* Test 2: test 1 done iteratively */
            unsigned i;
            bcrypto_t crypt;
            bcrypto_settings settings;
            uint8_t sha1_in[43] = "The quick brown fox jumps over the lazy dog";
            uint8_t sha1_out[20] = {0};
            uint8_t sha1_kat[20] = {
                0x2f, 0xd4, 0xe1, 0xc6, 0x7a, 0x2d, 0x28, 0xfc, 0xed, 0x84,
                0x9e, 0xe1, 0xbb, 0x76, 0xe7, 0x39, 0x1b, 0x93, 0xeb, 0x12 };

            uint8_t *dma_sha1_in  = bcrypto_alloc(43);
            uint8_t *dma_sha1_out = bcrypto_alloc(20);
            memcpy(dma_sha1_in, sha1_in, 43);
            memcpy(dma_sha1_out, sha1_out, 20);

            bcrypto_settings_init(&settings);
            settings.encryption.type = bencryption_type_sha1;
            settings.operation = bcrypto_operation_hash;
            settings.multiple_hashes = true;
	
            if (test) settings.hardware_ctrl = setCtrlMode(test);

            crypt = bcrypto_open(&settings);
	
            if (bcrypto_process(crypt, dma_sha1_in, 20, dma_sha1_out, 20) ||
                bcrypto_process(crypt, &dma_sha1_in[20], 23, dma_sha1_out, 20) ||
                bcrypto_process(crypt, NULL, 0, dma_sha1_out, 20) ) /* required to close out the operation */
            {
                printf("SHA1 (test 2 - %s) failed crypto process\n", pass[test]);
            } else if (0 != memcmp(dma_sha1_out, sha1_kat, 20)) {
                fprintf(outputChannel, "SHA1 (test 2 - %s) failed to hash correctly!\n", pass[test]);
                dump("IS", dma_sha1_out, 20, eOnError);
                dump("SB", sha1_kat, 20, eOnError);
            } else
                printf("SHA1 (test 2 - %s) SUCCESS!\n", pass[test]);
		
            dump("IS", dma_sha1_out, 20, eAlways);
            dump("SB", sha1_kat, 20, eAlways);

            bcrypto_close(crypt);
            bcrypto_free(dma_sha1_in);
            bcrypto_free(dma_sha1_out);
        }
#endif /* TEST_SHA1 */

#ifdef TEST_MD5
/* MD5 sample test taken from http://en.wikipedia.org/wiki/MD5:
   MD5("The quick brown fox jumps over the lazy dog") = 9e107d9d372bb6826bd81d3542a419d6
*/
        { /* Test 1: test vector tested all at once */
            unsigned i;
            bcrypto_t crypt;
            bcrypto_settings settings;
            uint8_t md5_in[43] = "The quick brown fox jumps over the lazy dog";
            uint8_t md5_out[16] = {0};
            uint8_t md5_kat[16] = {0x9e, 0x10, 0x7d, 0x9d, 0x37, 0x2b, 0xb6, 0x82, 0x6b, 0xd8, 0x1d, 0x35, 0x42, 0xa4, 0x19, 0xd6 };
	
            uint8_t *dma_md5_in  = bcrypto_alloc(43);
            uint8_t *dma_md5_out = bcrypto_alloc(16);
            memcpy(dma_md5_in, md5_in, 43);
            memcpy(dma_md5_out, md5_out, 16);

            bcrypto_settings_init(&settings);
            settings.encryption.type = bencryption_type_md5;
            settings.operation = bcrypto_operation_hash;
	
            if (test) settings.hardware_ctrl = setCtrlMode(test);

            crypt = bcrypto_open(&settings);
	
            if (bcrypto_process(crypt, dma_md5_in, 43, dma_md5_out, 16)) {
                printf("MD5 (test 1 - %s) failed crypto process\n", pass[test]);
            } else if (0 != memcmp(dma_md5_out, md5_kat, 16)) {
                fprintf(outputChannel, "MD5 (test 1 - %s) failed to hash correctly!\n", pass[test]);
                dump("IS", dma_md5_out, 16, eOnError);
                dump("SB", md5_kat, 16, eOnError);
            } else
                printf("MD5 (test 1 - %s) SUCCESS!\n", pass[test]);
		
            dump("IS", dma_md5_out, 16, eAlways);
            dump("SB", md5_kat, 16, eAlways);

            bcrypto_close(crypt);
            bcrypto_free(dma_md5_in);
            bcrypto_free(dma_md5_out);
        }

#if 0 /* This test temporarily removed as the underlying library does not currently support non-discrete hashing */
        { /* Test 2: test 1 done iteratively */
            unsigned i;
            bcrypto_t crypt;
            bcrypto_settings settings;
            uint8_t md5_in[43] = "The quick brown fox jumps over the lazy dog";
            uint8_t md5_out[16] = {0};
            uint8_t md5_kat[16] = {0x9e, 0x10, 0x7d, 0x9d, 0x37, 0x2b, 0xb6, 0x82, 0x6b, 0xd8, 0x1d, 0x35, 0x42, 0xa4, 0x19, 0xd6 };
	
            uint8_t *dma_md5_in  = bcrypto_alloc(43);
            uint8_t *dma_md5_out = bcrypto_alloc(16);
            memcpy(dma_md5_in, md5_in, 43);
            memcpy(dma_md5_out, md5_out, 16);

            bcrypto_settings_init(&settings);
            settings.encryption.type = bencryption_type_md5;
            settings.operation = bcrypto_operation_hash;
	
            if (test) settings.hardware_ctrl = setCtrlMode(test);

            crypt = bcrypto_open(&settings);
	
            if (bcrypto_process(crypt, dma_md5_in, 20, dma_md5_out, 16) ||
                bcrypto_process(crypt, &dma_md5_in[20], 23, dma_md5_out, 16)) {
                printf("MD5 (test 2 - %s) failed crypto process\n", pass[test]);
            } else if (0 != memcmp(dma_md5_out, md5_kat, 16)) {
                fprintf(outputChannel, "MD5 (test 2 - %s) failed to hash correctly!\n", pass[test]);
                dump("IS", dma_md5_out, 16, eOnError);
                dump("SB", md5_kat, 16, eOnError);
            } else
                printf("MD5 (test 2 - %s) SUCCESS!\n", pass[test]);
		
            dump("IS", dma_md5_out, 16, eAlways);
            dump("SB", md5_kat, 16, eAlways);
            
            bcrypto_close(crypt);
            bcrypto_free(dma_md5_in);
            bcrypto_free(dma_md5_out);
        }
#endif
#endif /* TEST_MD5 */

#ifdef TEST_CMAC
        {
            bcrypto_t crypt;
            bcrypto_settings settings;
            uint8_t cmac_in[16]  = {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a};
            uint8_t cmac_out[16] = {0};
            uint8_t cmac_key[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
            uint8_t cmac_kat[16] = {0x07, 0x0a, 0x16, 0xb4, 0x6b, 0x4d, 0x41, 0x44, 0xf7, 0x9b, 0xdd, 0x9d, 0xd0, 0x4a, 0x28, 0x7c};

            uint8_t *dma_cmac_in  = bcrypto_alloc(16);
            uint8_t *dma_cmac_out = bcrypto_alloc(16);
            memcpy(dma_cmac_in, cmac_in, 16);
            memcpy(dma_cmac_out, cmac_out, 16);

            bcrypto_settings_init(&settings);
            settings.encryption.type = bencryption_type_aes;
            memcpy(settings.encryption.key, cmac_key, sizeof(cmac_key));
            settings.encryption.key_length = 128;
            settings.operation = bcrypto_operation_mac;
            settings.encryption.blockmode = bcrypto_blockmode_cbc;
	
            if (test) settings.hardware_ctrl = setCtrlMode(test);

            crypt = bcrypto_open(&settings);
	
            if (bcrypto_process(crypt, dma_cmac_in, 16, dma_cmac_out, 16)) {
                printf("CMAC (%s) failed crypto process\n", pass[test]);
            } else if (0 != memcmp(dma_cmac_out, cmac_kat, 16)) {
                fprintf(outputChannel, "CMAC (%s) failed to encrypt correctly!\n", pass[test]);
                dump("IS", dma_cmac_out, 16, eOnError);
                dump("SB", cmac_kat, 16, eOnError);
            } else
                printf("CMAC (%s) SUCCESS!\n", pass[test]);

            dump("IS", dma_cmac_out, 16, eAlways);
            dump("SB", cmac_kat, 16, eAlways);

            bcrypto_close(crypt);
            bcrypto_free(dma_cmac_in);
            bcrypto_free(dma_cmac_out);
        }
#endif /* TEST_CMAC */

#ifdef TEST_HMAC_SHA1
/* Test data taken from RFC 2202 */
        {
            bcrypto_t crypt;
            bcrypto_settings settings;
            uint8_t hmac_in[]  = "Test With Truncation";
            uint8_t hmac_out[20] = {0};
            uint8_t hmac_key[20] = {0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c};
            uint8_t hmac_kat[20] = {0x4c, 0x1a, 0x03, 0x42, 0x4b, 0x55, 0xe0, 0x7f, 0xe7, 0xf2, 0x7b, 0xe1, 0xd5, 0x8b, 0xb9, 0x32, 0x4a, 0x9a, 0x5a, 0x04};

            uint8_t *dma_hmac_in  = bcrypto_alloc(sizeof(hmac_in));
            uint8_t *dma_hmac_out = bcrypto_alloc(20);
            memcpy(dma_hmac_in, hmac_in, sizeof(hmac_in));
            memcpy(dma_hmac_out, hmac_out, sizeof(hmac_out));

            bcrypto_settings_init(&settings);
            settings.encryption.type = bencryption_type_sha1;
            memcpy(settings.encryption.key, hmac_key, sizeof(hmac_key));
            settings.encryption.key_length = 160;
            settings.operation = bcrypto_operation_hmac;

            if (test) settings.hardware_ctrl = setCtrlMode(test);

            crypt = bcrypto_open(&settings);

            if (bcrypto_process(crypt, dma_hmac_in, 20, dma_hmac_out, 20)) {
                printf("HMAC (%s) failed crypto process\n", pass[test]);
            } else if (0 != memcmp(dma_hmac_out, hmac_kat, 20)) {
                fprintf(outputChannel, "CMAC (%s) failed to encrypt correctly!\n", pass[test]);
                dump("IS", dma_hmac_out, 20, eOnError);
                dump("SB", hmac_kat, 20, eOnError);
            } else
                printf("HMAC (%s) SUCCESS!\n", pass[test]);

            dump("IS", dma_hmac_out, 20, eAlways);
            dump("SB", hmac_kat, 20, eAlways);

            bcrypto_close(crypt);
            bcrypto_free(dma_hmac_in);
            bcrypto_free(dma_hmac_out);
        }
#endif /* TEST_HMAC_SHA1 */

#ifdef TEST_RSA
#if (BCHP_CHIP == 7400) && (BCHP_VER == A0)
        printf("RSA tests not supported by %s on this platform -- skipping test!\n", pass[test]);
#else
        {
            bcrypto_t crypt;
            bcrypto_settings settings;
            bcrypto_rsa_1024 rsa_key;
            uint8_t rsa_in[128] = {
                0x9f, 0x53, 0xd2, 0x60, 0x14, 0xf5, 0x19, 0xe1,
                0x7f, 0x58, 0x86, 0x84, 0x36, 0x4f, 0x6c, 0x26,
                0xbf, 0x2a, 0x5f, 0xcd, 0x1b, 0xce, 0x58, 0xc9,
                0xb8, 0x14, 0x5d, 0x97, 0x74, 0x1a, 0x40, 0xa6,
                0x40, 0xce, 0xba, 0x25, 0x22, 0x2b, 0x36, 0xf4,
                0x7e, 0xcd, 0x18, 0x80, 0x85, 0x54, 0xfc, 0xdb,
                0xfd, 0x4c, 0x1b, 0x7e, 0x81, 0x30, 0x40, 0x4e,
                0xe5, 0x03, 0xa7, 0x6c, 0x84, 0xf6, 0xbe, 0xd3,
                0xa3, 0x93, 0x21, 0xb2, 0x81, 0x40, 0xa4, 0x5c,
                0x3b, 0x2a, 0x81, 0x38, 0x39, 0x62, 0x92, 0x8a,
                0x8c, 0x00, 0xc6, 0xcb, 0xe1, 0xf5, 0xd0, 0x09,
                0xc2, 0x8d, 0x21, 0x0c, 0xc7, 0x7b, 0xb6, 0x2d,
                0x0f, 0x10, 0x0d, 0x4b, 0x4a, 0xf1, 0x49, 0x33,
                0xaa, 0x68, 0xd5, 0x30, 0xfc, 0xee, 0x1b, 0xb9,
                0x31, 0xdf, 0xc4, 0xfb, 0x06, 0x2e, 0x77, 0xe8,
                0xcc, 0xbc, 0xed, 0x24, 0xf8, 0x64, 0x0d, 0x63 };

            uint8_t rsa_out[128] = {0};
            uint8_t rsa_kat[128] = {
                0x72, 0x70, 0x9d, 0x0c, 0xb3, 0x73, 0xd0, 0xe6,
                0xd1, 0x41, 0x56, 0x19, 0xc7, 0xaf, 0xf5, 0x01,
                0xba, 0xf7, 0xb7, 0xd9, 0x2c, 0xd4, 0x31, 0xcf,
                0x38, 0x58, 0x71, 0xfb, 0xb2, 0x66, 0x7a, 0x69,
                0xba, 0x2b, 0x76, 0x15, 0x0e, 0x25, 0xb2, 0x9a,
                0xbc, 0x55, 0x01, 0xf1, 0xd3, 0x87, 0xd1, 0x4b,
                0x06, 0x41, 0xd5, 0x02, 0x88, 0xb9, 0xfb, 0x2e,
                0xea, 0x24, 0xa9, 0xf9, 0xf9, 0x20, 0xdd, 0x2c,
                0x50, 0x70, 0x49, 0x34, 0x84, 0xde, 0x41, 0xb8,
                0x0b, 0xd8, 0x76, 0x39, 0xe5, 0x43, 0x72, 0xf1,
                0xa8, 0x4b, 0x9e, 0x64, 0x1e, 0x8e, 0x92, 0x45,
                0xfd, 0xd8, 0x29, 0x79, 0x38, 0xb0, 0x8d, 0xd1,
                0x64, 0xdb, 0x4b, 0x94, 0x5f, 0x33, 0xa5, 0x7d,
                0x48, 0xb6, 0x93, 0xb8, 0xf6, 0xea, 0x6f, 0x98,
                0x44, 0x38, 0x8a, 0x60, 0x0d, 0x8b, 0xf5, 0x85,
                0xcc, 0xc9, 0xf0, 0xd3, 0x90, 0x2e, 0xc0, 0x2a };
            uint8_t n[128] = {
                0xd4, 0xe9, 0xed, 0xc3, 0xd5, 0x46, 0xc2, 0xc5,
                0x97, 0x7d, 0x41, 0x1d, 0x39, 0x9b, 0x0a, 0x4f,
                0x05, 0x32, 0x59, 0x60, 0x02, 0x9e, 0x96, 0xd6,
                0xfc, 0x28, 0xfa, 0xa4, 0xb7, 0x77, 0x39, 0x72,
                0x67, 0x67, 0xc3, 0x3e, 0x8b, 0xf6, 0x19, 0x75,
                0xf6, 0x48, 0x8e, 0x5d, 0x30, 0x6d, 0x09, 0x8c,
                0xc5, 0x83, 0x14, 0xcd, 0x80, 0x45, 0x43, 0xc4,
                0xf4, 0xf3, 0xc6, 0x4a, 0xc8, 0x7c, 0xa1, 0x30,
                0x69, 0xdf, 0x9f, 0x9d, 0xa2, 0xc3, 0x2e, 0x8c,
                0x2e, 0xa3, 0xb0, 0x2f, 0x5e, 0xe8, 0x2b, 0xd0,
                0x81, 0x71, 0x7b, 0x63, 0x97, 0x9c, 0x92, 0xcb,
                0x63, 0xed, 0xba, 0x1b, 0xe2, 0x93, 0x9f, 0x05,
                0x98, 0x0a, 0xeb, 0x5a, 0xd0, 0x82, 0x0e, 0xc3,
                0x13, 0xd0, 0x5e, 0x39, 0x09, 0xab, 0xfa, 0x0f,
                0x75, 0xef, 0x9c, 0x68, 0xe7, 0xa7, 0xc7, 0x57,
                0xdf, 0xe3, 0x18, 0xaf, 0xc5, 0x9c, 0x45, 0xe3 };
            uint8_t d[128] = {
                0x94, 0x74, 0x87, 0xd4, 0x84, 0x61, 0x52, 0x6d,
                0x73, 0xb3, 0x62, 0x33, 0x7e, 0x77, 0x1c, 0x38,
                0xfc, 0xe6, 0x9e, 0x6a, 0xd3, 0x68, 0xc0, 0xc4,
                0x26, 0x0e, 0x6c, 0xb5, 0x6d, 0x37, 0x34, 0x57,
                0x10, 0xc2, 0xc8, 0xe0, 0x53, 0xb6, 0xc5, 0x09,
                0xfb, 0x2f, 0x9c, 0x14, 0x5e, 0x2f, 0xa8, 0xf8,
                0xfe, 0xbe, 0xe7, 0x42, 0x62, 0xad, 0xe9, 0x12,
                0xeb, 0xb9, 0x0e, 0x0a, 0x16, 0xc0, 0xcf, 0xa2,
                0x2c, 0x98, 0xb9, 0x1b, 0x78, 0x1a, 0x46, 0x48,
                0xaa, 0x2a, 0x8a, 0xa7, 0x9a, 0x78, 0x7f, 0x55,
                0xd8, 0x15, 0x43, 0xa3, 0xae, 0x4e, 0x5b, 0x49,
                0x09, 0x46, 0xaf, 0x83, 0x11, 0xab, 0xef, 0xa5,
                0x0b, 0xf3, 0xf6, 0xfd, 0x94, 0x70, 0x83, 0xea,
                0x09, 0xd1, 0x42, 0xe0, 0xec, 0x39, 0x3c, 0x13,
                0x9e, 0xc5, 0x6a, 0xa8, 0x2d, 0x4e, 0xad, 0x72,
                0xc4, 0x2d, 0x1a, 0x61, 0x7f, 0x23, 0x50, 0x01 };
            uint8_t e[128] = {
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01};

            bcrypto_settings_init(&settings);
            settings.encryption.type = bencryption_type_rsa;
            settings.encryption.key_length = sizeof(bcrypto_rsa_1024) * 8;
	
            memset(&rsa_key, 0, sizeof(rsa_key));
            rsa_key.key_length = 1024;
            memcpy(rsa_key.n,n,sizeof(n));
            memcpy(rsa_key.d,d,sizeof(d));
            memcpy(rsa_key.e,e,sizeof(e));

            settings.encryption.long_key = &rsa_key;
            settings.operation = bcrypto_operation_sign;

            if (test) settings.hardware_ctrl = setCtrlMode(test);
	
            crypt = bcrypto_open(&settings);
	
            if (bcrypto_process(crypt, rsa_in, 128, rsa_out, 128)) {
                printf("RSA1024 (%s) failed crypto process\n", pass[test]);
            } else if (0 != memcmp(rsa_out, rsa_kat, 128)) {
                fprintf(outputChannel, "RSA1024 (%s) failed to sign correctly!\n", pass[test]);
                dump("IS", rsa_out, 128, eOnError);
                dump("SB", rsa_kat, 128, eOnError);
            } else
                printf("RSA1024 sign (%s) SUCCESS!\n", pass[test]);

            dump("IS", rsa_out, 128, eAlways);
            dump("SB", rsa_kat, 128, eAlways);
            bcrypto_close(crypt);
        }
        {
            bcrypto_t crypt;
            bcrypto_settings settings;
            bcrypto_rsa_1024 rsa_key;
            uint8_t rsa_in[128] = {
                0x72, 0x70, 0x9d, 0x0c, 0xb3, 0x73, 0xd0, 0xe6,
                0xd1, 0x41, 0x56, 0x19, 0xc7, 0xaf, 0xf5, 0x01,
                0xba, 0xf7, 0xb7, 0xd9, 0x2c, 0xd4, 0x31, 0xcf,
                0x38, 0x58, 0x71, 0xfb, 0xb2, 0x66, 0x7a, 0x69,
                0xba, 0x2b, 0x76, 0x15, 0x0e, 0x25, 0xb2, 0x9a,
                0xbc, 0x55, 0x01, 0xf1, 0xd3, 0x87, 0xd1, 0x4b,
                0x06, 0x41, 0xd5, 0x02, 0x88, 0xb9, 0xfb, 0x2e,
                0xea, 0x24, 0xa9, 0xf9, 0xf9, 0x20, 0xdd, 0x2c,
                0x50, 0x70, 0x49, 0x34, 0x84, 0xde, 0x41, 0xb8,
                0x0b, 0xd8, 0x76, 0x39, 0xe5, 0x43, 0x72, 0xf1,
                0xa8, 0x4b, 0x9e, 0x64, 0x1e, 0x8e, 0x92, 0x45,
                0xfd, 0xd8, 0x29, 0x79, 0x38, 0xb0, 0x8d, 0xd1,
                0x64, 0xdb, 0x4b, 0x94, 0x5f, 0x33, 0xa5, 0x7d,
                0x48, 0xb6, 0x93, 0xb8, 0xf6, 0xea, 0x6f, 0x98,
                0x44, 0x38, 0x8a, 0x60, 0x0d, 0x8b, 0xf5, 0x85,
                0xcc, 0xc9, 0xf0, 0xd3, 0x90, 0x2e, 0xc0, 0x2a };

            uint8_t rsa_out[128] = {0};
            uint8_t rsa_kat[128] = {
                0x9f, 0x53, 0xd2, 0x60, 0x14, 0xf5, 0x19, 0xe1,
                0x7f, 0x58, 0x86, 0x84, 0x36, 0x4f, 0x6c, 0x26,
                0xbf, 0x2a, 0x5f, 0xcd, 0x1b, 0xce, 0x58, 0xc9,
                0xb8, 0x14, 0x5d, 0x97, 0x74, 0x1a, 0x40, 0xa6,
                0x40, 0xce, 0xba, 0x25, 0x22, 0x2b, 0x36, 0xf4,
                0x7e, 0xcd, 0x18, 0x80, 0x85, 0x54, 0xfc, 0xdb,
                0xfd, 0x4c, 0x1b, 0x7e, 0x81, 0x30, 0x40, 0x4e,
                0xe5, 0x03, 0xa7, 0x6c, 0x84, 0xf6, 0xbe, 0xd3,
                0xa3, 0x93, 0x21, 0xb2, 0x81, 0x40, 0xa4, 0x5c,
                0x3b, 0x2a, 0x81, 0x38, 0x39, 0x62, 0x92, 0x8a,
                0x8c, 0x00, 0xc6, 0xcb, 0xe1, 0xf5, 0xd0, 0x09,
                0xc2, 0x8d, 0x21, 0x0c, 0xc7, 0x7b, 0xb6, 0x2d,
                0x0f, 0x10, 0x0d, 0x4b, 0x4a, 0xf1, 0x49, 0x33,
                0xaa, 0x68, 0xd5, 0x30, 0xfc, 0xee, 0x1b, 0xb9,
                0x31, 0xdf, 0xc4, 0xfb, 0x06, 0x2e, 0x77, 0xe8,
                0xcc, 0xbc, 0xed, 0x24, 0xf8, 0x64, 0x0d, 0x63 };
            uint8_t n[128] = {
                0xd4, 0xe9, 0xed, 0xc3, 0xd5, 0x46, 0xc2, 0xc5,
                0x97, 0x7d, 0x41, 0x1d, 0x39, 0x9b, 0x0a, 0x4f,
                0x05, 0x32, 0x59, 0x60, 0x02, 0x9e, 0x96, 0xd6,
                0xfc, 0x28, 0xfa, 0xa4, 0xb7, 0x77, 0x39, 0x72,
                0x67, 0x67, 0xc3, 0x3e, 0x8b, 0xf6, 0x19, 0x75,
                0xf6, 0x48, 0x8e, 0x5d, 0x30, 0x6d, 0x09, 0x8c,
                0xc5, 0x83, 0x14, 0xcd, 0x80, 0x45, 0x43, 0xc4,
                0xf4, 0xf3, 0xc6, 0x4a, 0xc8, 0x7c, 0xa1, 0x30,
                0x69, 0xdf, 0x9f, 0x9d, 0xa2, 0xc3, 0x2e, 0x8c,
                0x2e, 0xa3, 0xb0, 0x2f, 0x5e, 0xe8, 0x2b, 0xd0,
                0x81, 0x71, 0x7b, 0x63, 0x97, 0x9c, 0x92, 0xcb,
                0x63, 0xed, 0xba, 0x1b, 0xe2, 0x93, 0x9f, 0x05,
                0x98, 0x0a, 0xeb, 0x5a, 0xd0, 0x82, 0x0e, 0xc3,
                0x13, 0xd0, 0x5e, 0x39, 0x09, 0xab, 0xfa, 0x0f,
                0x75, 0xef, 0x9c, 0x68, 0xe7, 0xa7, 0xc7, 0x57,
                0xdf, 0xe3, 0x18, 0xaf, 0xc5, 0x9c, 0x45, 0xe3 };
            uint8_t d[128] = {
                0x94, 0x74, 0x87, 0xd4, 0x84, 0x61, 0x52, 0x6d,
                0x73, 0xb3, 0x62, 0x33, 0x7e, 0x77, 0x1c, 0x38,
                0xfc, 0xe6, 0x9e, 0x6a, 0xd3, 0x68, 0xc0, 0xc4,
                0x26, 0x0e, 0x6c, 0xb5, 0x6d, 0x37, 0x34, 0x57,
                0x10, 0xc2, 0xc8, 0xe0, 0x53, 0xb6, 0xc5, 0x09,
                0xfb, 0x2f, 0x9c, 0x14, 0x5e, 0x2f, 0xa8, 0xf8,
                0xfe, 0xbe, 0xe7, 0x42, 0x62, 0xad, 0xe9, 0x12,
                0xeb, 0xb9, 0x0e, 0x0a, 0x16, 0xc0, 0xcf, 0xa2,
                0x2c, 0x98, 0xb9, 0x1b, 0x78, 0x1a, 0x46, 0x48,
                0xaa, 0x2a, 0x8a, 0xa7, 0x9a, 0x78, 0x7f, 0x55,
                0xd8, 0x15, 0x43, 0xa3, 0xae, 0x4e, 0x5b, 0x49,
                0x09, 0x46, 0xaf, 0x83, 0x11, 0xab, 0xef, 0xa5,
                0x0b, 0xf3, 0xf6, 0xfd, 0x94, 0x70, 0x83, 0xea,
                0x09, 0xd1, 0x42, 0xe0, 0xec, 0x39, 0x3c, 0x13,
                0x9e, 0xc5, 0x6a, 0xa8, 0x2d, 0x4e, 0xad, 0x72,
                0xc4, 0x2d, 0x1a, 0x61, 0x7f, 0x23, 0x50, 0x01 };
            uint8_t e[128] = {
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01 };

            bcrypto_settings_init(&settings);
            settings.encryption.type = bencryption_type_rsa;
            settings.encryption.key_length = sizeof(bcrypto_rsa_1024) * 8;
	
            memset(&rsa_key, 0, sizeof(rsa_key));
            rsa_key.key_length = 1024;
            memcpy(rsa_key.n,n,sizeof(n));
            memcpy(rsa_key.d,d,sizeof(d));
            memcpy(rsa_key.e,e,sizeof(e));

            settings.encryption.long_key = &rsa_key;
            settings.operation = bcrypto_operation_encrypt;
	
            if (test) settings.hardware_ctrl = setCtrlMode(test);

            crypt = bcrypto_open(&settings);
	
            if (bcrypto_process(crypt, rsa_in, 128, rsa_out, 128)) {
                printf("RSA1024 (%s) failed crypto process\n", pass[test]);
            } else if (0 != memcmp(rsa_out, rsa_kat, 128)) {
                fprintf(outputChannel, "RSA1024 (%s) failed to encrypt correctly!\n", pass[test]);
                dump("IS", rsa_out, 128, eOnError);
                dump("SB", rsa_kat, 128, eOnError);
            } else
                printf("RSA1024 encryption (%s) SUCCESS!\n", pass[test]);

            dump("IS", rsa_out, 128, eAlways);
            dump("SB", rsa_kat, 128, eAlways);
            bcrypto_close(crypt);
        }
        {
            bcrypto_t crypt;
            bcrypto_settings settings;
            bcrypto_rsa_1024 rsa_key;
            uint8_t rsa_in[128] = {
                0x9f, 0x53, 0xd2, 0x60, 0x14, 0xf5, 0x19, 0xe1,
                0x7f, 0x58, 0x86, 0x84, 0x36, 0x4f, 0x6c, 0x26,
                0xbf, 0x2a, 0x5f, 0xcd, 0x1b, 0xce, 0x58, 0xc9,
                0xb8, 0x14, 0x5d, 0x97, 0x74, 0x1a, 0x40, 0xa6,
                0x40, 0xce, 0xba, 0x25, 0x22, 0x2b, 0x36, 0xf4,
                0x7e, 0xcd, 0x18, 0x80, 0x85, 0x54, 0xfc, 0xdb,
                0xfd, 0x4c, 0x1b, 0x7e, 0x81, 0x30, 0x40, 0x4e,
                0xe5, 0x03, 0xa7, 0x6c, 0x84, 0xf6, 0xbe, 0xd3,
                0xa3, 0x93, 0x21, 0xb2, 0x81, 0x40, 0xa4, 0x5c,
                0x3b, 0x2a, 0x81, 0x38, 0x39, 0x62, 0x92, 0x8a,
                0x8c, 0x00, 0xc6, 0xcb, 0xe1, 0xf5, 0xd0, 0x09,
                0xc2, 0x8d, 0x21, 0x0c, 0xc7, 0x7b, 0xb6, 0x2d,
                0x0f, 0x10, 0x0d, 0x4b, 0x4a, 0xf1, 0x49, 0x33,
                0xaa, 0x68, 0xd5, 0x30, 0xfc, 0xee, 0x1b, 0xb9,
                0x31, 0xdf, 0xc4, 0xfb, 0x06, 0x2e, 0x77, 0xe8,
                0xcc, 0xbc, 0xed, 0x24, 0xf8, 0x64, 0x0d, 0x63 };

            uint8_t rsa_out[128] = {0};
            uint8_t rsa_kat[128] = {
                0x72, 0x70, 0x9d, 0x0c, 0xb3, 0x73, 0xd0, 0xe6,
                0xd1, 0x41, 0x56, 0x19, 0xc7, 0xaf, 0xf5, 0x01,
                0xba, 0xf7, 0xb7, 0xd9, 0x2c, 0xd4, 0x31, 0xcf,
                0x38, 0x58, 0x71, 0xfb, 0xb2, 0x66, 0x7a, 0x69,
                0xba, 0x2b, 0x76, 0x15, 0x0e, 0x25, 0xb2, 0x9a,
                0xbc, 0x55, 0x01, 0xf1, 0xd3, 0x87, 0xd1, 0x4b,
                0x06, 0x41, 0xd5, 0x02, 0x88, 0xb9, 0xfb, 0x2e,
                0xea, 0x24, 0xa9, 0xf9, 0xf9, 0x20, 0xdd, 0x2c,
                0x50, 0x70, 0x49, 0x34, 0x84, 0xde, 0x41, 0xb8,
                0x0b, 0xd8, 0x76, 0x39, 0xe5, 0x43, 0x72, 0xf1,
                0xa8, 0x4b, 0x9e, 0x64, 0x1e, 0x8e, 0x92, 0x45,
                0xfd, 0xd8, 0x29, 0x79, 0x38, 0xb0, 0x8d, 0xd1,
                0x64, 0xdb, 0x4b, 0x94, 0x5f, 0x33, 0xa5, 0x7d,
                0x48, 0xb6, 0x93, 0xb8, 0xf6, 0xea, 0x6f, 0x98,
                0x44, 0x38, 0x8a, 0x60, 0x0d, 0x8b, 0xf5, 0x85,
                0xcc, 0xc9, 0xf0, 0xd3, 0x90, 0x2e, 0xc0, 0x2a };
            uint8_t n[128] = {
                0xd4, 0xe9, 0xed, 0xc3, 0xd5, 0x46, 0xc2, 0xc5,
                0x97, 0x7d, 0x41, 0x1d, 0x39, 0x9b, 0x0a, 0x4f,
                0x05, 0x32, 0x59, 0x60, 0x02, 0x9e, 0x96, 0xd6,
                0xfc, 0x28, 0xfa, 0xa4, 0xb7, 0x77, 0x39, 0x72,
                0x67, 0x67, 0xc3, 0x3e, 0x8b, 0xf6, 0x19, 0x75,
                0xf6, 0x48, 0x8e, 0x5d, 0x30, 0x6d, 0x09, 0x8c,
                0xc5, 0x83, 0x14, 0xcd, 0x80, 0x45, 0x43, 0xc4,
                0xf4, 0xf3, 0xc6, 0x4a, 0xc8, 0x7c, 0xa1, 0x30,
                0x69, 0xdf, 0x9f, 0x9d, 0xa2, 0xc3, 0x2e, 0x8c,
                0x2e, 0xa3, 0xb0, 0x2f, 0x5e, 0xe8, 0x2b, 0xd0,
                0x81, 0x71, 0x7b, 0x63, 0x97, 0x9c, 0x92, 0xcb,
                0x63, 0xed, 0xba, 0x1b, 0xe2, 0x93, 0x9f, 0x05,
                0x98, 0x0a, 0xeb, 0x5a, 0xd0, 0x82, 0x0e, 0xc3,
                0x13, 0xd0, 0x5e, 0x39, 0x09, 0xab, 0xfa, 0x0f,
                0x75, 0xef, 0x9c, 0x68, 0xe7, 0xa7, 0xc7, 0x57,
                0xdf, 0xe3, 0x18, 0xaf, 0xc5, 0x9c, 0x45, 0xe3 };
            uint8_t d[128] = {
                0x94, 0x74, 0x87, 0xd4, 0x84, 0x61, 0x52, 0x6d,
                0x73, 0xb3, 0x62, 0x33, 0x7e, 0x77, 0x1c, 0x38,
                0xfc, 0xe6, 0x9e, 0x6a, 0xd3, 0x68, 0xc0, 0xc4,
                0x26, 0x0e, 0x6c, 0xb5, 0x6d, 0x37, 0x34, 0x57,
                0x10, 0xc2, 0xc8, 0xe0, 0x53, 0xb6, 0xc5, 0x09,
                0xfb, 0x2f, 0x9c, 0x14, 0x5e, 0x2f, 0xa8, 0xf8,
                0xfe, 0xbe, 0xe7, 0x42, 0x62, 0xad, 0xe9, 0x12,
                0xeb, 0xb9, 0x0e, 0x0a, 0x16, 0xc0, 0xcf, 0xa2,
                0x2c, 0x98, 0xb9, 0x1b, 0x78, 0x1a, 0x46, 0x48,
                0xaa, 0x2a, 0x8a, 0xa7, 0x9a, 0x78, 0x7f, 0x55,
                0xd8, 0x15, 0x43, 0xa3, 0xae, 0x4e, 0x5b, 0x49,
                0x09, 0x46, 0xaf, 0x83, 0x11, 0xab, 0xef, 0xa5,
                0x0b, 0xf3, 0xf6, 0xfd, 0x94, 0x70, 0x83, 0xea,
                0x09, 0xd1, 0x42, 0xe0, 0xec, 0x39, 0x3c, 0x13,
                0x9e, 0xc5, 0x6a, 0xa8, 0x2d, 0x4e, 0xad, 0x72,
                0xc4, 0x2d, 0x1a, 0x61, 0x7f, 0x23, 0x50, 0x01 };
            uint8_t e[128] = {
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01 };

            bcrypto_settings_init(&settings);
            settings.encryption.type = bencryption_type_rsa;
            settings.encryption.key_length = sizeof(bcrypto_rsa_1024) * 8;
	
            memset(&rsa_key, 0, sizeof(rsa_key));
            rsa_key.key_length = 1024;
            memcpy(rsa_key.n,n,sizeof(n));
            memcpy(rsa_key.d,d,sizeof(d));
            memcpy(rsa_key.e,e,sizeof(e));

            settings.encryption.long_key = &rsa_key;
            settings.operation = bcrypto_operation_decrypt;
	
            if (test) settings.hardware_ctrl = setCtrlMode(test);

            crypt = bcrypto_open(&settings);
	
            if (bcrypto_process(crypt, rsa_in, 128, rsa_out, 128)) {
                printf("RSA1024 (%s) failed crypto process\n", pass[test]);
            } else if (0 != memcmp(rsa_out, rsa_kat, 128)) {
                fprintf(outputChannel, "RSA1024 (%s) failed to decrypt correctly!\n", pass[test]);
                dump("IS", rsa_out, 128, eOnError);
                dump("SB", rsa_kat, 128, eOnError);
            } else
                printf("RSA1024 decryption (%s) SUCCESS!\n", pass[test]);

            dump("IS", rsa_out, 128, eAlways);
            dump("SB", rsa_kat, 128, eAlways);
            bcrypto_close(crypt);
        }
#endif /* (BCHP_CHIP == 7400) && (BCHP_VER == A0) */
#endif /* TEST_RSA */

    } /*for(test)*/

	return 0;
  error:
	return 1;
}
