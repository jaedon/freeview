/*************************************************************************************************************
	File 		: maxdome_sdi.cpp
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2012/11/09
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (¡°Humax¡±) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*************************************************************************************************************/

#include "maxdome_sdi.h"

#include <openssl/rsa.h>
#include <openssl/ssl.h>
#include <openssl/engine.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/rand.h>
#include <openssl/aes.h>

#include <hlib.h>

/* Maxdom secure serialnumber options , RSA encryption */
#define MAXDOME_PRIVKEY_PATH              "/usr/browser/opera_dir/maxdome/auth.humax"
#define MAXDOM_SHAREDSECRET_PATH         "/usr/browser/opera_dir/maxdome/ss.humax"
#define MAXDOME_SERIAL_AREA_SIZE     28
#define MAXDOME_AED_PRIVKEY             "HUMAX-2010-0825-EIKI-REV01"
#define MAXDOME_RSA_KEY_SIZE             512
#define MAXDOME_PLAIN_SIZE               256
#define MAXDOME_GUARD_PAD                1
#define MAXDOME_SALT_PATTERN             20100826,20100208
#define MAXDOME_AES_BUF_SIZE            4096
#define MAXDOME_SEPERATOR                ":"
#define MAXDOME_SEPERATOR_SIZE           1
#define MAXDOME_SECURE_BUF_SIZE   1024

using namespace Maxdome;

/******************************************************************************************
 * Static Functions
 ******************************************************************************************/
static unsigned int convBase64to(const unsigned char *input, unsigned char *output, int length)
{
    BIO *bmem, *b64;
    BUF_MEM *bptr;

    b64 = BIO_new(BIO_f_base64());
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, input, length);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);
    memcpy(output, bptr->data, bptr->length-1);
    output[bptr->length-1] = 0;
	BIO_free_all(b64);

    return 0;
}

static unsigned char *convBase64(const unsigned char *input, int length)
{
    BIO *bmem, *b64;
    BUF_MEM *bptr;
	unsigned char *buff;

    b64 = BIO_new(BIO_f_base64());
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, input, length);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);

    buff = (unsigned char *)HLIB_STD_MemAlloc(bptr->length);
    memcpy(buff, bptr->data, bptr->length-1);
    buff[bptr->length-1] = 0;
    BIO_free_all(b64);

    return buff;
}

static int aesInit(unsigned char *key_data, int key_data_len, unsigned char *salt,
		EVP_CIPHER_CTX *e_ctx, EVP_CIPHER_CTX *d_ctx)
{
    int i, nrounds = 5;
    unsigned char key[32], iv[32];

    i = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha1(), salt, key_data, key_data_len, nrounds, key, iv);
    if (i != 32) return -1;

    EVP_CIPHER_CTX_init(e_ctx);
    EVP_EncryptInit_ex(e_ctx, EVP_aes_256_cbc(), NULL, key, iv);
    EVP_CIPHER_CTX_init(d_ctx);
    EVP_DecryptInit_ex(d_ctx, EVP_aes_256_cbc(), NULL, key, iv);

    return 0;
}

static unsigned char *aesDecrypt(EVP_CIPHER_CTX *e, unsigned char *ciphertext, int *len)
{
    int p_len = *len, f_len = 0;
    unsigned char *plaintext = (unsigned char *)HLIB_STD_MemAlloc(p_len);

    EVP_DecryptInit_ex(e, NULL, NULL, NULL, NULL);
    EVP_DecryptUpdate(e, plaintext, &p_len, ciphertext, *len);
    EVP_DecryptFinal_ex(e, plaintext+p_len, &f_len);

    *len = p_len + f_len;

    return plaintext;
}
/******************************************************************************************
 * MaxdomeSecureDeviceInfo
 ******************************************************************************************/
bool MaxdomeSecureDeviceInfo::encrypt(unsigned char *buf, const char src_str)
{
    BIO *key_bio = NULL;
    RSA *rsa_priv_read = NULL;
    EVP_CIPHER_CTX 	en, de;
    FILE *privfp = NULL;
    FILE *ssfp = NULL;
    int privkey_len = 0;
    int secret_len = 0;
    int filesize = 0;
    unsigned int aes_salt[] = {MAXDOME_SALT_PATTERN};
    char *plain_text = NULL, *secret;
	unsigned char *temp = NULL, *salt = NULL;
	unsigned char num_buf[4] = {0,};
    unsigned char aes_buf[MAXDOME_AES_BUF_SIZE + MAXDOME_GUARD_PAD] = {0,};
    unsigned char enc_buf[MAXDOME_RSA_KEY_SIZE + MAXDOME_GUARD_PAD] = {0,};
    unsigned char plain_buf[MAXDOME_PLAIN_SIZE + MAXDOME_GUARD_PAD] = {0,};

    OpenSSL_add_all_algorithms();

    if ( (ssfp = fopen(MAXDOM_SHAREDSECRET_PATH, "rb")) == NULL )
         return false;

    fseek(ssfp,0,SEEK_END);
    filesize = ftell(ssfp);
	if ( filesize > 0 )
	{
	    fseek(ssfp,0,SEEK_SET);
   		if ( filesize != fread(aes_buf, filesize, 1, ssfp) )
   		{
   		    fclose(ssfp);
   		    return false;
   		}
	}
    secret_len = filesize;

	if ( ssfp != NULL )
        fclose(ssfp);

    if ( aesInit((unsigned char *)MAXDOME_AED_PRIVKEY,
			sizeof(MAXDOME_AED_PRIVKEY), (unsigned char *)&aes_salt, &en, &de) )
        return false;

    secret = (char *)aesDecrypt(&de, aes_buf, &secret_len); // Shared Secret
    secret_len--;
    memset(aes_buf, 0x00, sizeof(aes_buf));

	if ( (privfp = fopen(MAXDOME_PRIVKEY_PATH, "rb")) == NULL )
         return false;

    fseek(privfp,0,SEEK_END);
    filesize = ftell(privfp);
	if ( filesize > 0 )
	{
	    fseek(privfp,0,SEEK_SET);
   		if ( filesize != fread(aes_buf, filesize, 1, privfp) )
   		{
   		    fclose(privfp);
   		    return false;
   		}
	}
    privkey_len = filesize;

	if ( privfp != NULL )
       fclose(privfp);

    plain_text = (char *)aesDecrypt(&de, aes_buf, &privkey_len);

	EVP_CIPHER_CTX_cleanup(&en);
    EVP_CIPHER_CTX_cleanup(&de);

    key_bio = BIO_new_mem_buf(plain_text, privkey_len);

    rsa_priv_read = PEM_read_bio_RSAPrivateKey(key_bio,NULL,0,0);

	if ( key_bio != NULL )
        BIO_free(key_bio);

    if ( rsa_priv_read == NULL )
        return false;

    if ( !RSA_check_key(rsa_priv_read) )
        return false;

    salt = (unsigned char *)HLIB_STD_MemAlloc(MAXDOME_SERIAL_AREA_SIZE - secret_len + MAXDOME_GUARD_PAD);
	if ( salt == NULL )
		return false;

	if ( salt != NULL )
    	memset(salt, 0x00, MAXDOME_SERIAL_AREA_SIZE - secret_len + MAXDOME_GUARD_PAD);

    RAND_status();
    RAND_bytes(salt, MAXDOME_SERIAL_AREA_SIZE - secret_len);
    temp = convBase64(salt, MAXDOME_SERIAL_AREA_SIZE - secret_len);

	if ( temp != NULL && salt != NULL )
   		memcpy(salt, temp, MAXDOME_SERIAL_AREA_SIZE - secret_len);

    if ( temp != NULL )
        HLIB_STD_MemFree(temp);

    temp = plain_buf;

    memcpy(temp, (const char *)salt, strlen((const char *)salt));
    temp += strlen((const char *)salt);
    memcpy(temp,MAXDOME_SEPERATOR,MAXDOME_SEPERATOR_SIZE);
    temp+=MAXDOME_SEPERATOR_SIZE;
    if(salt != NULL)
        HLIB_STD_MemFree(salt);

    memcpy(temp, secret, secret_len);
    temp += secret_len;
    memcpy(temp, MAXDOME_SEPERATOR, MAXDOME_SEPERATOR_SIZE);
    temp += MAXDOME_SEPERATOR_SIZE;

	for ( int i=0; i<14; ++i )
	{
		snprintf((char *)num_buf, 4, "%X", src_str);
		strncpy((char *)buf, (const char *)num_buf, 4);
		//MWC_UTIL_DvbUtf8Sprintf(num_buf, "%X", src_str[i]);
		//MWC_UTIL_DvbStrcat(buf, num_buf);
	}

	memcpy(temp, buf, strlen((const char *)buf));

    if ( !RSA_private_encrypt(MAXDOME_PLAIN_SIZE,plain_buf,enc_buf,rsa_priv_read,RSA_PKCS1_PADDING) )
        return false;

	memset(buf, 0x00, MAXDOME_SECURE_BUF_SIZE);

    convBase64to(enc_buf, buf, MAXDOME_RSA_KEY_SIZE);
	memcpy(buf, temp, strlen((const char *)temp));

    if ( rsa_priv_read != NULL )
        RSA_free(rsa_priv_read);

	return true;
}
