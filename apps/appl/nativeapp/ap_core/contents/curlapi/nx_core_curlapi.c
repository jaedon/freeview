/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/******************************************************************************/
/**
 * @file	  		nx_core_curlapi.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#include <nx_core_curlapi.h>
#include <dlfcn.h>
#include <openssl/engine.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>

#include <nx_core_curlapi.h>

/**************************************************************************************************/
#define ___________________Private_typedef________________________________________________
/**************************************************************************************************/

typedef struct curlResponseData {
    char *response;
    size_t size;
}CURL_RES_DATA;

typedef struct curlMultiStatus {
    HUINT32 status;
    CURLM *multi_handle;
}CURL_STATUS;

typedef struct curlMultiData{
    CURL *http_handle;
    CURL_RES_DATA *easyResData;
	struct curl_slist *slistHeader;
    NxCurlApiReq    *stReqInfo;
}CURL_DATA;

typedef void * (*pthread_routine)(void *);

#define CURL_DEFAULT_REQ_TIMEOUT    30000
#define CURL_DEFAULT_GET_MAXSIZE    1024*1024 //1MB MAX

/**************************************************************************************************/
#define ___________________Private_variable________________________________________________
/**************************************************************************************************/

static CURL_STATUS  s_stCurlStatus;
static HUINT32		s_ulCurlThreadId = 0;

/**************************************************************************************************/
#define ___________________Private_prototype________________________________________________
/**************************************************************************************************/



/**************************************************************************************************/
#define ___________________Private_functions________________________________________________
/**************************************************************************************************/
curl_t*		NX_CORE_CURLAPI_GetInstance(void)
{
	static curl_t	*s_pstCurl = NULL;

	if (s_pstCurl == NULL)
	{
		s_pstCurl = HLIB_MEM_Calloc(sizeof(curl_t));

#if defined(CONFIG_PROD_OS_EMULATOR)	//EMULATOR
		s_pstCurl->_curl_global_init = curl_global_init;
		s_pstCurl->_curl_global_cleanup = curl_global_cleanup;
		s_pstCurl->_curl_easy_init = curl_easy_init;
		s_pstCurl->_curl_easy_setopt = curl_easy_setopt;
		s_pstCurl->_curl_easy_cleanup = curl_easy_cleanup;
		s_pstCurl->_curl_easy_perform = curl_easy_perform;
		s_pstCurl->_curl_easy_getinfo = curl_easy_getinfo;
		s_pstCurl->_curl_slist_append = curl_slist_append;
		s_pstCurl->_curl_slist_free_all = curl_slist_free_all;
		s_pstCurl->_curl_formadd = curl_formadd;
		s_pstCurl->_curl_formfree = curl_formfree;

		s_pstCurl->_curl_multi_init = curl_multi_init;
        s_pstCurl->_curl_multi_perform = curl_multi_perform;
        s_pstCurl->_curl_multi_remove_handle = curl_multi_remove_handle;
		s_pstCurl->_curl_multi_add_handle = curl_multi_add_handle;


#if defined(CONFIG_PROD_M1)
//This API need to check version. please Find more good way.
        s_pstCurl->_curl_multi_wait = curl_multi_wait;
#endif //CONFIG_PROD_M1
        s_pstCurl->_curl_multi_fdset = curl_multi_fdset;
        s_pstCurl->_curl_multi_info_read = curl_multi_info_read;
        s_pstCurl->_curl_multi_cleanup = curl_multi_cleanup;
        s_pstCurl->_curl_multi_timeout = curl_multi_timeout;

#else
        s_pstCurl->pvCurlSo = dlopen("/usr/lib/libcurl.so", RTLD_LAZY);
		if (s_pstCurl->pvCurlSo == NULL)
		{
			HxLOG_Critical("Can't find /usr/lib/libcurl.so\n");
			HxLOG_Assert(0);
			HLIB_MEM_Free(s_pstCurl);
			return NULL;
		}

		s_pstCurl->_curl_global_init = dlsym(s_pstCurl->pvCurlSo, "curl_global_init");
		s_pstCurl->_curl_global_cleanup = dlsym(s_pstCurl->pvCurlSo, "curl_global_cleanup");
		s_pstCurl->_curl_easy_init = dlsym(s_pstCurl->pvCurlSo, "curl_easy_init");
		s_pstCurl->_curl_easy_setopt = dlsym(s_pstCurl->pvCurlSo, "curl_easy_setopt");
		s_pstCurl->_curl_easy_cleanup = dlsym(s_pstCurl->pvCurlSo, "curl_easy_cleanup");
		s_pstCurl->_curl_easy_perform = dlsym(s_pstCurl->pvCurlSo, "curl_easy_perform");
		s_pstCurl->_curl_easy_getinfo = dlsym(s_pstCurl->pvCurlSo, "curl_easy_getinfo");
		s_pstCurl->_curl_slist_append = dlsym(s_pstCurl->pvCurlSo, "curl_slist_append");
		s_pstCurl->_curl_slist_free_all = dlsym(s_pstCurl->pvCurlSo, "curl_slist_free_all");
		s_pstCurl->_curl_formadd = dlsym(s_pstCurl->pvCurlSo, "curl_formadd");
		s_pstCurl->_curl_formfree = dlsym(s_pstCurl->pvCurlSo, "curl_formfree");

		s_pstCurl->_curl_multi_init = dlsym(s_pstCurl->pvCurlSo ,"curl_multi_init");
		s_pstCurl->_curl_multi_remove_handle = dlsym(s_pstCurl->pvCurlSo ,"curl_multi_remove_handle");
		s_pstCurl->_curl_multi_add_handle = dlsym(s_pstCurl->pvCurlSo ,"curl_multi_add_handle");


#if defined(CONFIG_PROD_M1)
//This API need to check version. please Find more good way.
		s_pstCurl->_curl_multi_wait = dlsym(s_pstCurl->pvCurlSo ,"curl_multi_wait");
		s_pstCurl->_curl_multi_perform = dlsym(s_pstCurl->pvCurlSo ,"curl_multi_perform");
#endif //CONFIG_PROD_M1
		s_pstCurl->_curl_multi_fdset = dlsym(s_pstCurl->pvCurlSo ,"curl_multi_fdset");
		s_pstCurl->_curl_multi_info_read = dlsym(s_pstCurl->pvCurlSo ,"curl_multi_info_read");
		s_pstCurl->_curl_multi_cleanup = dlsym(s_pstCurl->pvCurlSo ,"curl_multi_cleanup");
		s_pstCurl->_curl_multi_timeout = dlsym(s_pstCurl->pvCurlSo ,"curl_multi_timeout");

#endif

		HxLOG_Assert(s_pstCurl->_curl_global_init && s_pstCurl->_curl_global_init && s_pstCurl->_curl_easy_init);
		HxLOG_Assert(s_pstCurl->_curl_easy_setopt && s_pstCurl->_curl_easy_cleanup && s_pstCurl->_curl_easy_perform);
		HxLOG_Assert(s_pstCurl->_curl_easy_getinfo && s_pstCurl->_curl_slist_append && s_pstCurl->_curl_slist_free_all);
		HxLOG_Assert(s_pstCurl->_curl_formadd && s_pstCurl->_curl_formfree);

		HxLOG_Warning("libCurl dynamic loading is done...\n");
	}

	return s_pstCurl;
}

/**************************************************************************************************/
#define ___________________Public_functions________________________________________________
/**************************************************************************************************/


#define ____________________________________MY_TEST__________________________________________

#if 0

size_t curlWriteFunction (void *ptr, size_t size, size_t nmemb, CURL_RES_DATA *res) {
    size_t newLen = res->size + size*nmemb;
    res->response = realloc(res->response, newLen+1);
    if (res->response == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    HxSTD_memcpy(res->response+res->size, ptr, size*nmemb);
    res->response[newLen] = '\0';
    res->size = newLen;

    return size*nmemb;
}

static void hmac_sha256(
    const unsigned char *text,      /* pointer to data stream        */
    int                 text_len,   /* length of data stream         */
    const unsigned char *key,       /* pointer to authentication key */
    int                 key_len,    /* length of authentication key  */
    void                *digest)    /* caller digest to be filled in */
{
#ifdef  EVP_MAX_MD_SIZE

    unsigned int result_len;
    unsigned char result[EVP_MAX_MD_SIZE];

    HMAC(EVP_sha256(),
         key, HxSTD_StrLen(key),
         text, HxSTD_StrLen(text),
         result, &result_len);
    HxSTD_memcpy(digest, result, result_len);

#else

    unsigned char k_ipad[65];   /* inner padding -
                                 * key XORd with ipad
                                 */
    unsigned char k_opad[65];   /* outer padding -
                                 * key XORd with opad
                                 */
    unsigned char tk[SHA256_DIGEST_LENGTH];
    unsigned char tk2[SHA256_DIGEST_LENGTH];
    unsigned char bufferIn[1024];
    unsigned char bufferOut[1024];
    int           i;

    /* if key is longer than 64 bytes reset it to key=sha256(key) */
    if ( key_len > 64 ) {
        SHA256( key, key_len, tk );
        key     = tk;
        key_len = SHA256_DIGEST_LENGTH;
    }

    /*
     * the HMAC_SHA256 transform looks like:
     *
     * SHA256(K XOR opad, SHA256(K XOR ipad, text))
     *
     * where K is an n byte key
     * ipad is the byte 0x36 repeated 64 times
     * opad is the byte 0x5c repeated 64 times
     * and text is the data being protected
     */

    /* start out by storing key in pads */
    HxSTD_memset( k_ipad, 0, sizeof k_ipad );
    HxSTD_memset( k_opad, 0, sizeof k_opad );
    HxSTD_memcpy( k_ipad, key, key_len );
    HxSTD_memcpy( k_opad, key, key_len );

    /* XOR key with ipad and opad values */
    for ( i = 0; i < 64; i++ ) {
        k_ipad[i] ^= 0x36;
        k_opad[i] ^= 0x5c;
    }

    /*
     * perform inner SHA256
     */
    HxSTD_memset( bufferIn, 0x00, 1024 );
    HxSTD_memcpy( bufferIn, k_ipad, 64 );
    HxSTD_memcpy( bufferIn + 64, text, text_len );

    SHA256( bufferIn, 64 + text_len, tk2 );

    /*
     * perform outer SHA256
     */
    HxSTD_memset( bufferOut, 0x00, 1024 );
    HxSTD_memcpy( bufferOut, k_opad, 64 );
    HxSTD_memcpy( bufferOut + 64, tk2, SHA256_DIGEST_LENGTH );

    SHA256( bufferOut, 64 + SHA256_DIGEST_LENGTH, digest );

#endif
}

static char b[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static char*   base64( const unsigned char *src, size_t sz )
{
    unsigned char               *pp, *p, *q;
    static unsigned char *qq = NULL;
    size_t                      i, safe = sz;

    if ( qq ) {
        free( qq );
        qq = NULL;
    }
    if ( !src || (sz == 0) )
        return ( NULL );

    if ( (sz % 3) == 1 ) {
        p = (unsigned char *)malloc( sz + 2 );
        if ( !p )
            return ( NULL );
        HxSTD_memcpy( p, src, sz );
        p[sz] = p[sz + 1] = '=';
        sz += 2;
    }
    else if ( (sz % 3) == 2 ) {
        p = (unsigned char *)malloc( sz + 1 );
        if ( !p )
            return ( NULL );
        HxSTD_memcpy( p, src, sz );
        p[sz] = '=';
        sz++;
    }
    else
        p = (unsigned char *)src;

    q = (unsigned char *)malloc( (sz / 3) * 4 + 2 );
    if ( !q ) {
        if ( p != src )
            free( p );
        return ( NULL );
    }

    pp = p;
    qq = q;
    for ( i = 0; i < sz; i += 3 ) {
        q[0] = b[(p[0] & 0xFC) >> 2];
        q[1] = b[((p[0] & 0x03) << 4) | ((p[1] & 0xF0) >> 4)];
        q[2] = b[((p[1] & 0x0F) << 2) | ((p[2] & 0xC0) >> 6)];
        q[3] = b[p[2] & 0x3F];
        p += 3;
        q += 4;
    }
    *q = NULL;
    if ( (safe % 3) == 1 ) {
        *(q - 1) = '=';
        *(q - 2) = '=';
    }
    if ( (safe % 3) == 2 )
        *(q - 1) = '=';

    if ( pp != src )
        free( pp );
    return ( (char *)qq );
}

static void nx_curl_GetURL(HCHAR* url)
{
  	CURL *curl = NULL;
  	CURLcode res;

   	const int timeout_ms = 5000;
	CURL_RES_DATA resData;

    curl = nxCurlSo(curl_easy_init());

	nx_core_curlapi_InitCurlResponseData(&resData);

	nxCurlSo(curl_easy_setopt(curl, CURLOPT_URL, url));
	nxCurlSo(curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L));
	nxCurlSo(curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout_ms));

	/* DATA */
	nxCurlSo(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, nx_core_curlapi_OptWriteFunction));
	nxCurlSo(curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resData));

	res = nxCurlSo(curl_easy_perform(curl));

	if(res != CURLE_OK){
		HxLOG_Print("\n\n\n\n[%s.%d]curl_easy_perform() failed -- %d\n\n\n\n",__FUNCTION__,__LINE__,  res);
	}
	else
	{
		HxLOG_Print("\n\n\n[%s.%d] ----------->> Receivced data : %s \n\n\n",__FUNCTION__,__LINE__, resData.response);
    }

	nxCurlSo(curl_easy_cleanup(curl));
}

static void nx_http_Task(void *arg)
{
	CURL *curl = NULL;
	CURLcode res;
	struct curl_slist *chunk = NULL;
	const int timeout_ms = 5000;

    const HCHAR *JSON_NEWENCRYPTEDAPIKEY="newEncryptedApiKey";
    const HCHAR *JSON_DATA="data";
	const HCHAR *BASE_SALT_KEY = "aYrlSYTufFGMvg2J";
    const HCHAR* VALUE_APIKEY = "sh@hid0nlin3";
    const HCHAR* TEST_PARAM = "channelParam={\"sort\":{\"name\":\"ASC\"}}&apiKey=sh@hid0nlin3";
    const HCHAR* VALUE_CHANNEL = "{\"sort\":{\"name\":\"ASC\"}}";


    static const HINT32 LENGTH=128;
	CURL_RES_DATA resData;
	HxJSON_t		root, data;
    HCHAR           *strNewEncryptedApiKey = NULL;
    HBOOL           bBase64 = FALSE;
    HUINT32         size;
    HUINT8	        *decodedKey = NULL;
    HCHAR           strAccessKey[128] = {0,};
    HCHAR           *strBase64Hash;
    HCHAR           strGetChannelQueryURL[256]={0,};
   HxLOG_Print("\n[%s.%d] ############################# TEST START ############################# \n\n\n",__FUNCTION__,__LINE__);
	nxCurlSo(curl_global_init(CURL_GLOBAL_ALL));
	curl = nxCurlSo(curl_easy_init());
	if(!curl){
		HxLOG_Print("\n\n\n\n[%s.%d]curl_easy_init() failed\n", __FUNCTION__,__LINE__);
		return;
	}

	nx_core_curlapi_InitCurlResponseData(&resData);

	nxCurlSo(curl_easy_setopt(curl, CURLOPT_URL, "http://api.shahid.net/api/apiAccess"));
	nxCurlSo(curl_easy_setopt(curl, CURLOPT_POST, 1));
	nxCurlSo(curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{\"apiKey\":\"sh@hid0nlin3\"}"));
	nxCurlSo(curl_easy_setopt(curl, CURLOPT_VERBOSE, 2L));

	if(timeout_ms){
		nxCurlSo(curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout_ms));
		nxCurlSo(curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, timeout_ms));
	}

	/* DATA */
	nxCurlSo(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteFunction));
	nxCurlSo(curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resData));


	chunk = nxCurlSo(curl_slist_append(chunk, "Content-type: application/json"));
	chunk = nxCurlSo(curl_slist_append(chunk, "charset: utf-8"));
	chunk = nxCurlSo(curl_slist_append(chunk, "Cache-Control: no-cache"));
	nxCurlSo(curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk));

	HxLOG_Print("\n\n\n[%s.%d]curl_easy_perform() start\n\n\n",__FUNCTION__,__LINE__);
	res = nxCurlSo(curl_easy_perform(curl));

	if(res != CURLE_OK){
		HxLOG_Print("\n\n\n\n[%s.%d]curl_easy_perform() failed -- %d\n\n\n\n",__FUNCTION__,__LINE__,  res);
	}
	else
	{
		HxLOG_Print("\n\n\n[%s.%d] ----------->> Receivced data : %s \n\n\n",__FUNCTION__,__LINE__, resData.response);

        {
        	root = HLIB_JSON_Decode(resData.response);
            data = HxJSON_Object_Get(root, JSON_DATA);

            strNewEncryptedApiKey = HLIB_JSON_Object_GetString(data,JSON_NEWENCRYPTEDAPIKEY);
            bBase64 = HLIB_MATH_IsBase64(strNewEncryptedApiKey);
            decodedKey = HLIB_MATH_DecodeBase64Easy(strNewEncryptedApiKey,&size);
            HxSTD_memset(strAccessKey, 0, 128);
            HLIB_CRYPT_DEC_AES_ECB(BASE_SALT_KEY,decodedKey,strAccessKey);
            HxLOG_Print("\n[%s.%d] ----------->> strAccessKey : %s \n\n\n",__FUNCTION__,__LINE__, strAccessKey);
        }

		NX_APP_Free(resData.response);
	#if 0
		HLIB_JSON_Get(resData.response, "apiKey");
	#endif
	}

//SHA256 & Base64 ENCORDING
    {
        HCHAR digest[256] ={0,};
        hmac_sha256(TEST_PARAM, strlen(TEST_PARAM),  strAccessKey, HxSTD_StrLen(strAccessKey), digest);
        strBase64Hash = base64(digest,SHA256_DIGEST_LENGTH);

        if( strBase64Hash )
        {
            HxLOG_Print("\n[%s.%d] ----------->> base64 RESULT : %s \n\n\n",__FUNCTION__,__LINE__, strBase64Hash);
        }
    }

	nxCurlSo(curl_slist_free_all(chunk));
	nxCurlSo(curl_easy_cleanup(curl));

//URL ENCORDING
    {
    	HUINT32		len = 0;
	    HCHAR		*strUrlChannelParam= NULL;
        HCHAR       *strUrlApiKey = NULL;
        HCHAR       *strUrlHash = NULL;

    	len = HLIB_STD_URLStrLen(VALUE_CHANNEL) + 1;
        strUrlChannelParam = DLIB_Calloc(len+1);
        HLIB_STD_URLEncode(strUrlChannelParam, VALUE_CHANNEL, len);

    	len = HLIB_STD_URLStrLen(VALUE_APIKEY) + 1;
        strUrlApiKey = DLIB_Calloc(len+1);
        HLIB_STD_URLEncode(strUrlApiKey, VALUE_APIKEY, len);
        HxLOG_Print("\n[%s.%d] ############################# \n\n\n",__FUNCTION__,__LINE__);
        HxLOG_Print("\n[%s.%d] ----------->> strUrlApiKey: %s \n\n\n",__FUNCTION__,__LINE__, strUrlApiKey);
        if(strBase64Hash)
        {
        	len = HLIB_STD_URLStrLen(strBase64Hash) + 1;
            strUrlHash= DLIB_Calloc(len+1);
            HLIB_STD_URLEncode(strUrlHash, strBase64Hash, len);
    		HxSTD_snprintf(strGetChannelQueryURL, 256,
						"http://api.shahid.net/api/channel/getChannels?channelParam=%s&apiKey=%s&hash=%s",
						strUrlChannelParam,strUrlApiKey,strUrlHash);
        }

        HxLOG_Print("\n[%s.%d] ----------->> URL ENCORDING RESULT : %s \n\n\n",__FUNCTION__,__LINE__, strGetChannelQueryURL);

        if(NULL != strUrlHash)
        {
            DLIB_Free(strUrlHash);
        }

        if(NULL != strUrlChannelParam)
        {
            DLIB_Free(strUrlChannelParam);
        }

        if(NULL != strUrlApiKey)
        {
            DLIB_Free(strUrlApiKey);
        }
    }

    nx_curl_GetURL(strGetChannelQueryURL);

    if(NULL != decodedKey)
    {
        HLIB_MATH_FreeBase64(decodedKey);
    }

	nxCurlSo(curl_global_cleanup());
}

//Multi interface code doing two parallel HTTP transfers
void nx_http_MultiTask(void *arg)
{
    CURL *http_handle;
    CURL *http_handle2;
    CURLM *multi_handle;

	struct curl_slist *chunk = NULL;
	const int timeout_ms = 5000;

    CURL_RES_DATA resData;

    int still_running; /* keep number of running handles */

    http_handle = nxCurlSo(curl_easy_init());
    http_handle2 = nxCurlSo(curl_easy_init());


	chunk = nxCurlSo(curl_slist_append(chunk, "Content-type: application/json"));
	chunk = nxCurlSo(curl_slist_append(chunk, "charset: utf-8"));
	chunk = nxCurlSo(curl_slist_append(chunk, "Cache-Control: no-cache"));

	nx_core_curlapi_InitCurlResponseData(&resData);

    /* set options */
    nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_URL, "http://api.shahid.net/api/apiAccess"));
	nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_POST, 1));
	nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_POSTFIELDS, "{\"apiKey\":\"sh@hid0nlin3\"}"));
	nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_VERBOSE, 2L));


	if(timeout_ms){
		nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_TIMEOUT_MS, timeout_ms));
		nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_CONNECTTIMEOUT_MS, timeout_ms));
	}

	/* DATA */
	nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_WRITEFUNCTION, curlWriteFunction));
	nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_WRITEDATA, &resData));
	nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_HTTPHEADER, chunk));


    /* set options */
    nxCurlSo(curl_easy_setopt(http_handle2, CURLOPT_URL, "http://api.shahid.net/api/apiAccess"));
	nxCurlSo(curl_easy_setopt(http_handle2, CURLOPT_POST, 1));
	nxCurlSo(curl_easy_setopt(http_handle2, CURLOPT_POSTFIELDS, "{\"apiKey\":\"sh@hid0nlin3\"}"));
	nxCurlSo(curl_easy_setopt(http_handle2, CURLOPT_VERBOSE, 2L));

	if(timeout_ms){
		nxCurlSo(curl_easy_setopt(http_handle2, CURLOPT_TIMEOUT_MS, timeout_ms));
		nxCurlSo(curl_easy_setopt(http_handle2, CURLOPT_CONNECTTIMEOUT_MS, timeout_ms));
	}

	/* DATA */
	nxCurlSo(curl_easy_setopt(http_handle2, CURLOPT_WRITEFUNCTION, curlWriteFunction));
	nxCurlSo(curl_easy_setopt(http_handle2, CURLOPT_WRITEDATA, &resData));
	nxCurlSo(curl_easy_setopt(http_handle2, CURLOPT_HTTPHEADER, chunk));

    /* init a multi stack */
    multi_handle = nxCurlSo(curl_multi_init());

    /* add the individual transfers */
    nxCurlSo(curl_multi_add_handle(multi_handle, http_handle));
    nxCurlSo(curl_multi_add_handle(multi_handle, http_handle2));

    /* we start some action by calling perform right away */
    curl_multi_perform(multi_handle, &still_running);

    do {
    struct timeval timeout;
    int rc; /* select() return code */
    CURLMcode mc; /* curl_multi_fdset() return code */

    fd_set fdread;
    fd_set fdwrite;
    fd_set fdexcep;
    int maxfd = -1;

    long curl_timeo = -1;

    FD_ZERO(&fdread);
    FD_ZERO(&fdwrite);
    FD_ZERO(&fdexcep);

    /* set a suitable timeout to play around with */
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    nxCurlSo(curl_multi_timeout(multi_handle, &curl_timeo));

    if(curl_timeo >= 0) {
      timeout.tv_sec = curl_timeo / 1000;
      if(timeout.tv_sec > 1)
        timeout.tv_sec = 1;
      else
        timeout.tv_usec = (curl_timeo % 1000) * 1000;
    }

    /* get file descriptors from the transfers */
    mc = nxCurlSo(curl_multi_fdset(multi_handle, &fdread, &fdwrite, &fdexcep, &maxfd));

    if(mc != CURLM_OK)
    {
      fprintf(stderr, "curl_multi_fdset() failed, code %d.\n", mc);
      break;
    }

    /* On success the value of maxfd is guaranteed to be >= -1. We call
       select(maxfd + 1, ...); specially in case of (maxfd == -1) there are
       no fds ready yet so we call select(0, ...) --or Sleep() on Windows--
       to sleep 100ms, which is the minimum suggested value in the
       curl_multi_fdset() doc. */

    if(maxfd == -1) {
#ifdef _WIN32
      Sleep(100);
      rc = 0;
#else
      /* Portable sleep for platforms other than Windows. */
      struct timeval wait = { 0, 100 * 1000 }; /* 100ms */
      rc = select(0, NULL, NULL, NULL, &wait);
#endif
    }
    else {
      /* Note that on some platforms 'timeout' may be modified by select().
         If you need access to the original value save a copy beforehand. */
      rc = select(maxfd+1, &fdread, &fdwrite, &fdexcep, &timeout);
    }

    switch(rc) {
    case -1:
      /* select error */
      break;
    case 0:
    default:
      /* timeout or readable/writable sockets */
      nxCurlSo(curl_multi_perform(multi_handle, &still_running));
      break;
    }
    } while(still_running);

  nxCurlSo(curl_slist_free_all(chunk));

  nxCurlSo(curl_multi_cleanup(multi_handle));
  nxCurlSo(curl_easy_cleanup(http_handle));
  nxCurlSo(curl_easy_cleanup(http_handle2));
}

static HERROR nx_core_curlapi_MakeEasyHandle(HCHAR* url, CURL **easy_handle)
{
    CURL_DATA   *curl_data;
    CURL *http_handle  = NULL;
	struct curl_slist *chunk = NULL;
    CURL_RES_DATA *easyResData;

    curl_data = (CURL_DATA*)NX_APP_Calloc(sizeof(CURL_DATA));
    curl_data->easyResData = (CURL_RES_DATA*)NX_APP_Calloc(sizeof(CURL_RES_DATA));

    curl_data->http_handle = nxCurlSo(curl_easy_init());
    http_handle = curl_data->http_handle;
    easyResData = curl_data->easyResData;
	nx_core_curlapi_InitCurlResponseData(curl_data->easyResData);

	chunk = nxCurlSo(curl_slist_append(chunk, "Content-type: application/json"));
	chunk = nxCurlSo(curl_slist_append(chunk, "charset: utf-8"));
	chunk = nxCurlSo(curl_slist_append(chunk, "Cache-Control: no-cache"));

    curl_data->chunk = chunk;

    /* set options */
    nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_URL, url));
	nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_POST, 1));
	nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_POSTFIELDS, "{\"apiKey\":\"sh@hid0nlin3\"}"));
	nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_VERBOSE, 2L));

	nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_TIMEOUT_MS, 5000));
	nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_CONNECTTIMEOUT_MS, 5000));
	nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_HTTPHEADER, chunk));


	/* DATA */
	nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_WRITEFUNCTION, curlWriteFunction));
	nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_WRITEDATA, easyResData));

    //Set Private Data
	nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_PRIVATE, (void*)curl_data));

    *easy_handle = http_handle;
    return ERR_OK;
}

HERROR	NX_HTTP_INIT(void)
{
	HULONG	ulTaskId = 0;

//    VK_TASK_Create(nx_http_Task, 40, 0x4000, "nx_http_task", (void *)NULL, &ulTaskId, 0);
//    VK_TASK_Create(nx_http_MultiTask, 40, 0x4000, "nx_http_task", (void *)NULL, &ulTaskId, 0); // molink_ksjung_0160112
//    VK_TASK_Start(ulTaskId);

	return ERR_OK;
}

static void *nx_core_curlapi_MultiCurlTask2(void *data)
{
    CURL *http_handle;
    CURL_RES_DATA *easyResData;
    CURLMcode curlMCode;
	struct curl_slist *chunk = NULL;
    CURL_DATA   *curl_data = (CURL_DATA*)data;
    struct  CURLMsg *m;
    HUINT32 msgq = 0;

    http_handle = curl_data->http_handle;
    easyResData = curl_data->easyResData;
    chunk = curl_data->chunk;

    do
    {
        struct timeval timeout;
        int rc; /* select() return code */
        CURLMcode mc; /* curl_multi_fdset() return code */

        fd_set fdread;
        fd_set fdwrite;
        fd_set fdexcep;
        int maxfd = -1;

        long curl_timeo = -1;

        FD_ZERO(&fdread);
        FD_ZERO(&fdwrite);
        FD_ZERO(&fdexcep);

        /* set a suitable timeout to play around with */
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        nxCurlSo(curl_multi_timeout(s_stCurlStatus.multi_handle, &curl_timeo));

        if(curl_timeo >= 0) {
          timeout.tv_sec = curl_timeo / 1000;
          if(timeout.tv_sec > 1)
            timeout.tv_sec = 1;
          else
            timeout.tv_usec = (curl_timeo % 1000) * 1000;
        }

        /* get file descriptors from the transfers */
        mc = nxCurlSo(curl_multi_fdset(s_stCurlStatus.multi_handle, &fdread, &fdwrite, &fdexcep, &maxfd));

        if(mc != CURLM_OK)
        {
          fprintf(stderr, "curl_multi_fdset() failed, code %d.\n", mc);
          break;
        }

        /* On success the value of maxfd is guaranteed to be >= -1. We call
           select(maxfd + 1, ...); specially in case of (maxfd == -1) there are
           no fds ready yet so we call select(0, ...) --or Sleep() on Windows--
           to sleep 100ms, which is the minimum suggested value in the
           curl_multi_fdset() doc. */

        if(maxfd == -1) {
#ifdef _WIN32
          Sleep(100);
          rc = 0;
#else
          /* Portable sleep for platforms other than Windows. */
          struct timeval wait = { 0, 100 * 1000 }; /* 100ms */
          rc = select(0, NULL, NULL, NULL, &wait);
#endif
        }
        else {
          /* Note that on some platforms 'timeout' may be modified by select().
             If you need access to the original value save a copy beforehand. */
          rc = select(maxfd+1, &fdread, &fdwrite, &fdexcep, &timeout);
        }
        HxLOG_Print("\n[%s.%d] ----------->> rc : %d \n\n\n",__FUNCTION__,__LINE__,rc);
        switch(rc)
        {
            case -1:
              /* select error */
              break;
            case 0:
            default:
              /* timeout or readable/writable sockets */
              m = nxCurlSo(curl_multi_info_read(s_stCurlStatus.multi_handle,&msgq));
              if(m && m->msg == CURLMSG_DONE)
              {
                CURL *easy_handle;
                HINT32 res_status;
                HxLOG_Print("\n[%s.%d] ----------->> CURLMSG_DONE msg_remain : %d \n\n\n",__FUNCTION__,__LINE__,msgq);
                easy_handle = m->easy_handle;
                nxCurlSo(curl_easy_getinfo(easy_handle,CURLINFO_RESPONSE_CODE,&res_status));
                HxLOG_Print("\n[%s.%d] ----------->> CURLINFO_RESPONSE_CODE : %d \n\n\n",__FUNCTION__,__LINE__,res_status);
                nxCurlSo(curl_easy_cleanup(easy_handle));
              }

              nxCurlSo(curl_multi_perform(s_stCurlStatus.multi_handle, &(s_stCurlStatus.status)));

              break;
        }
        } while(s_stCurlStatus.status);

        while(m = nxCurlSo(curl_multi_info_read(s_stCurlStatus.multi_handle,&msgq))){
            if(m && m->msg == CURLMSG_DONE)
            {
                CURL *easy_handle;
                HINT32 res_status;
                CURLcode return_code=0;

                HxLOG_Print("\n[%s.%d] ----------->> CURLMSG_DONE msg_remain : %d \n\n\n",__FUNCTION__,__LINE__,msgq);
                easy_handle = m->easy_handle;
                return_code = m->data.result;

                if(return_code != CURLE_OK)
                {
                    HxLOG_Print("\n[%s.%d] ----------->> CURLERROR : %d \n\n\n",__FUNCTION__,__LINE__,return_code);
                    continue;
                }

                nxCurlSo(curl_easy_getinfo(easy_handle,CURLINFO_RESPONSE_CODE,&res_status));

                HxLOG_Print("\n[%s.%d] ----------->> CURLINFO_RESPONSE_CODE : %d \n\n\n",__FUNCTION__,__LINE__,res_status);
                nxCurlSo(curl_easy_cleanup(easy_handle));
            }
            else
            {
                HxLOG_Print("\n[%s.%d] ----------->> error: after curl_multi_info_read : %s \n\n\n",__FUNCTION__,__LINE__,m->msg);
            }

        }

      nxCurlSo(curl_slist_free_all(chunk));
      nxCurlSo(curl_multi_cleanup(s_stCurlStatus.multi_handle));
}

static void *nx_core_curlapi_MultiCurlTask3(void *data)
{
    CURL *http_handle;
    CURL_RES_DATA *easyResData;
    CURLMcode curlMCode;
	struct curl_slist *chunk = NULL;
    CURL_DATA   *curl_data = (CURL_DATA*)data;

    http_handle = curl_data->http_handle;
    easyResData = curl_data->easyResData;
    chunk = curl_data->chunk;

    do
    {
        struct timeval timeout;
        int rc; /* select() return code */
        CURLMcode mc; /* curl_multi_fdset() return code */

        fd_set fdread;
        fd_set fdwrite;
        fd_set fdexcep;
        int maxfd = -1;

        long curl_timeo = -1;

        FD_ZERO(&fdread);
        FD_ZERO(&fdwrite);
        FD_ZERO(&fdexcep);

        /* set a suitable timeout to play around with */
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        nxCurlSo(curl_multi_timeout(s_stCurlStatus.multi_handle, &curl_timeo));

        if(curl_timeo >= 0) {
          timeout.tv_sec = curl_timeo / 1000;
          if(timeout.tv_sec > 1)
            timeout.tv_sec = 1;
          else
            timeout.tv_usec = (curl_timeo % 1000) * 1000;
        }

        /* get file descriptors from the transfers */
        mc = nxCurlSo(curl_multi_fdset(s_stCurlStatus.multi_handle, &fdread, &fdwrite, &fdexcep, &maxfd));

        if(mc != CURLM_OK)
        {
          fprintf(stderr, "curl_multi_fdset() failed, code %d.\n", mc);
          break;
        }

        /* On success the value of maxfd is guaranteed to be >= -1. We call
           select(maxfd + 1, ...); specially in case of (maxfd == -1) there are
           no fds ready yet so we call select(0, ...) --or Sleep() on Windows--
           to sleep 100ms, which is the minimum suggested value in the
           curl_multi_fdset() doc. */

        if(maxfd == -1) {
#ifdef _WIN32
          Sleep(100);
          rc = 0;
#else
          /* Portable sleep for platforms other than Windows. */
          struct timeval wait = { 0, 100 * 1000 }; /* 100ms */
          rc = select(0, NULL, NULL, NULL, &wait);
#endif
        }
        else {
          /* Note that on some platforms 'timeout' may be modified by select().
             If you need access to the original value save a copy beforehand. */
          rc = select(maxfd+1, &fdread, &fdwrite, &fdexcep, &timeout);
        }

        switch(rc)
        {
            case -1:
              /* select error */
              break;
            case 0:
            default:
              /* timeout or readable/writable sockets */
              nxCurlSo(curl_multi_perform(s_stCurlStatus.multi_handle, &(s_stCurlStatus.status)));
              break;
        }
        } while(s_stCurlStatus.status);
      nxCurlSo(curl_slist_free_all(chunk));
      nxCurlSo(curl_multi_cleanup(s_stCurlStatus.multi_handle));
      nxCurlSo(curl_easy_cleanup(http_handle));
}


// API TEST CODE
HERROR NX_CORE_CURLAPI_DoMultiTest()
{
    CURL *http_handle = NULL;
    CURL_RES_DATA *easyResData;
	struct curl_slist *chunk = NULL;
    CURLMcode curlMCode ;
    CURL_DATA   *curl_data;

    HxLOG_Print("\n[%s.%d] ----------->> START \n\n\n",__FUNCTION__,__LINE__);
    HxLOG_Print("\n[%s.%d] ----------->> s_stCurlStatus.status : %d \n\n\n",__FUNCTION__,__LINE__,s_stCurlStatus.status);
    if(s_stCurlStatus.status>0)
    {
        /* add the individual transfers */
        //ERROR
        nx_core_curlapi_MakeEasyHandle("http://api.shahid.net/api/", &http_handle);
        nxCurlSo(curl_multi_add_handle(s_stCurlStatus.multi_handle, http_handle));
    }
    else
    {
        nx_core_curlapi_MakeEasyHandle("http://api.shahid.net/api/apiAccess", &http_handle);
        /* init a multi stack */
        s_stCurlStatus.multi_handle = nxCurlSo(curl_multi_init());
        /* add the individual transfers */
        nxCurlSo(curl_multi_add_handle(s_stCurlStatus.multi_handle, http_handle));

        /* we start some action by calling perform right away */
        curlMCode = curl_multi_perform(s_stCurlStatus.multi_handle, &(s_stCurlStatus.status));
        //pthread_create
		pthread_create( &s_ulCurlThreadId, NULL, (pthread_routine)nx_core_curlapi_MultiCurlTask, NULL);
    }

    return ERR_OK;
}

#endif

void nx_core_curlapi_InitCurlResponseData (CURL_RES_DATA *res) {
    res->size = 0;
    res->response = malloc(res->size+1);

    if ( res->response == NULL ) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }

    res->response[0] = '\0';
}

size_t nx_core_curlapi_OptWriteFunction (void *ptr, size_t size, size_t nmemb, CURL_RES_DATA *res)
{
    size_t newLen = res->size + size*nmemb;
    size_t dataSize = size*nmemb;
    size_t bufSize = 0;

    if(newLen>CURL_DEFAULT_GET_MAXSIZE)
    {
        bufSize = newLen;
    }
    else
    {
        bufSize = CURL_DEFAULT_GET_MAXSIZE;
    }

    if(res->size == 0)
    {
        res->response = HLIB_STD_MemCalloc(bufSize);

        if (res->response == NULL) {
            fprintf(stderr, "realloc() failed\n");
            exit(EXIT_FAILURE);
        }

        HLIB_STD_MemCpySafe(res->response, bufSize, ptr, dataSize);
    }
    else
    {
        HxSTD_memcpy(res->response+res->size, ptr, size*nmemb);
    }

    res->response[newLen] = '\0';
    res->size = newLen;

    return size*nmemb;
}

static HERROR nx_core_curlapi_SendResponse(CURL *easy_handle)
{
    CURL *http_handle;
    CURL_DATA   *curl_data = NULL;

    http_handle = easy_handle;
    nxCurlSo(curl_easy_getinfo(http_handle, CURLINFO_PRIVATE, &curl_data));
    if(NULL != curl_data)
    {
        CURL_RES_DATA *easyResData = curl_data->easyResData;
        NxCurlApiReq    *stReqInfo = curl_data->stReqInfo;

        HINT32 res_status = 0;
        NxCurlApiRes    *stResInfo = NX_APP_Calloc(sizeof(NxCurlApiRes));

        if(NULL != stResInfo)
        {
            nxCurlSo(curl_easy_getinfo(http_handle,CURLINFO_RESPONSE_CODE,&res_status));

            //Set Response Data
            if(NULL != stReqInfo)
            {
                stResInfo->reqParam = stReqInfo->reqParam;
            }

            stResInfo->resStatus = res_status;
            stResInfo->resData = (void*) easyResData->response;

            if(NULL != stReqInfo)
            {
                if(NULL != stReqInfo->pfnResCallback)
                {
                    stReqInfo->pfnResCallback(stResInfo);
                }
                else
                {
                    HxLOG_Print("\n[%s.%d] ----------->> No pfnResCallback \n\n\n",__FUNCTION__,__LINE__);
                }
            }
            else
            {
                HxLOG_Print("\n[%s.%d] ----------->> No stReqInfo \n\n\n",__FUNCTION__,__LINE__);
            }
            NX_APP_Free(stResInfo);
        }
    }
    return ERR_OK;
}

static HERROR nx_core_curlapi_CleanEasyHandle(CURL **easy_handle)
{
    CURL *http_handle = NULL;
    CURL_DATA   *curl_data = NULL;

    CURL_RES_DATA *easyResData = NULL;
    NxCurlApiReq    *stReqInfo = NULL;

    http_handle = *easy_handle;
    nxCurlSo(curl_easy_getinfo(http_handle, CURLINFO_PRIVATE, &curl_data));
    nxCurlSo(curl_easy_cleanup(http_handle));
    if(NULL != curl_data)
    {
        nxCurlSo(curl_slist_free_all(curl_data->slistHeader));

        stReqInfo = curl_data->stReqInfo;
        easyResData = curl_data->easyResData;

        if(NULL != stReqInfo)
        {
            NX_APP_Free(stReqInfo);
        }

        if(NULL != easyResData->response)
        {
            NX_APP_Free(easyResData->response);
        }

        NX_APP_Free(easyResData);
        NX_APP_Free(curl_data);
    }
    return ERR_OK;
}

static HERROR nx_core_curlapi_MakeReqHandle(NxCurlApiReq *stReq, CURL **easy_handle)
{
    CURL_DATA   *curl_data;
    CURL *http_handle  = NULL;
	struct curl_slist *slistHeader = NULL;
    CURL_RES_DATA *easyResData;

    curl_data = (CURL_DATA*)NX_APP_Calloc(sizeof(CURL_DATA));
    curl_data->easyResData = (CURL_RES_DATA*)NX_APP_Calloc(sizeof(CURL_RES_DATA));
    curl_data->http_handle = nxCurlSo(curl_easy_init());
    curl_data->stReqInfo = stReq;

    http_handle = curl_data->http_handle;
    easyResData = curl_data->easyResData;
	nx_core_curlapi_InitCurlResponseData(curl_data->easyResData);

	slistHeader = nxCurlSo(curl_slist_append(slistHeader, "Content-type: application/json"));
	slistHeader = nxCurlSo(curl_slist_append(slistHeader, "charset: utf-8"));
	slistHeader = nxCurlSo(curl_slist_append(slistHeader, "Cache-Control: no-cache"));

    curl_data->slistHeader = slistHeader;

    /* set options */
    nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_URL, stReq->strUrl));

    if(eCurlReqMethodGET == stReq->eReqMethod)
    {
    	nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_HTTPGET, 1L));//CURLOPT_NOBODY
    }
    else if(eCurlReqMethodPOST== stReq->eReqMethod)
    {
    	nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_POST, 1));
	    nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_POSTFIELDS, stReq->strPostParam));
    }

#if defined(CONFIG_DEBUG)
	/* turn off the debugging. if you want to display msg of curl, chane 0L -> 1L */
	nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_VERBOSE, 1L));
#endif

	nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_TIMEOUT_MS, CURL_DEFAULT_REQ_TIMEOUT));
    nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_CONNECTTIMEOUT_MS, 5000));
	nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_HTTPHEADER, slistHeader));

	/* DATA */
	nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_WRITEFUNCTION, nx_core_curlapi_OptWriteFunction));
	nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_WRITEDATA, easyResData));

    //Set Private Data
	nxCurlSo(curl_easy_setopt(http_handle, CURLOPT_PRIVATE, (void*)curl_data));

    *easy_handle = http_handle;

    return ERR_OK;
}

static void *nx_core_curlapi_MultiCurlTask(void *data)
{
    CURLMcode curlMCode;
    struct  CURLMsg *m;
    HUINT32 msgq = 0;
#if defined(CONFIG_PROD_M1)
    do {
        int numfds=0;
        int res = nxCurlSo(curl_multi_wait(s_stCurlStatus.multi_handle, NULL, 0, 30000, &numfds));
        if(res != CURLM_OK)
        {
            HxLOG_Print("\n[%s.%d] ----------->> curl_multi_wait Error : %d \n\n\n",__FUNCTION__,__LINE__,res);
            return NULL;
        }
        m = nxCurlSo(curl_multi_info_read(s_stCurlStatus.multi_handle,&msgq));
        if(m && m->msg == CURLMSG_DONE)
        {
            CURL *easy_handle;
            HINT32 res_status;
            easy_handle = m->easy_handle;
            nxCurlSo(curl_easy_getinfo(easy_handle,CURLINFO_RESPONSE_CODE,&res_status));
            HxLOG_Print("\n[%s.%d] ----------->> CURLINFO_RESPONSE_CODE : %d \n\n\n",__FUNCTION__,__LINE__,res_status);
            nx_core_curlapi_SendResponse(easy_handle);
            nx_core_curlapi_CleanEasyHandle(&easy_handle);
        }
        nxCurlSo(curl_multi_perform(s_stCurlStatus.multi_handle, &(s_stCurlStatus.status)));
    } while(s_stCurlStatus.status);

    while( (m = nxCurlSo(curl_multi_info_read(s_stCurlStatus.multi_handle,&msgq))) )
    {
        if(m && m->msg == CURLMSG_DONE)
        {
            CURL *easy_handle;
            HINT32 res_status;
            CURLcode return_code=0;
            easy_handle = m->easy_handle;
            return_code = m->data.result;

            if(return_code != CURLE_OK)
            {
                HxLOG_Print("\n[%s.%d] ----------->> CURLERROR : %d \n\n\n",__FUNCTION__,__LINE__,return_code);
                continue;
            }

            nxCurlSo(curl_easy_getinfo(easy_handle,CURLINFO_RESPONSE_CODE,&res_status));
            HxLOG_Print("\n[%s.%d] ----------->> CURLINFO_RESPONSE_CODE : %d \n\n\n",__FUNCTION__,__LINE__,res_status);
            nx_core_curlapi_SendResponse(easy_handle);
            nx_core_curlapi_CleanEasyHandle(&easy_handle);
        }
        else
        {
            HxLOG_Print("\n[%s.%d] ----------->> error: after curl_multi_info_read : %s \n\n\n",__FUNCTION__,__LINE__,m->msg);
        }
    }

    curlMCode = nxCurlSo(curl_multi_cleanup(s_stCurlStatus.multi_handle));
    if(curlMCode == CURLM_OK)
    {
        s_stCurlStatus.multi_handle = NULL;
    }
#endif
    return NULL;
}

HERROR NX_CORE_CURLAPI_DoMultiInit(void)
{
    HxSTD_memset(&s_stCurlStatus, 0, sizeof(CURL_STATUS));

    return ERR_OK;
}

HERROR NX_CORE_CURLAPI_Req(NxCurlApiReq *stReq)
{
    NxCurlApiReq *_stReq;
    CURL *http_handle = NULL;
    CURLMcode curlMCode ;

    HxLOG_Print("\n[%s.%d] ----------->> START \n\n\n",__FUNCTION__,__LINE__);
#if defined(CONFIG_PROD_M1)

    _stReq  = NX_APP_MemDup(stReq, sizeof(NxCurlApiReq));

    nx_core_curlapi_MakeReqHandle(_stReq, &http_handle);

    if(s_stCurlStatus.status>0)
    {
        //add easy handler to multi statck
        nxCurlSo(curl_multi_add_handle(s_stCurlStatus.multi_handle, http_handle));
    }
    else
    {
        /* init a multi stack */
        s_stCurlStatus.multi_handle = nxCurlSo(curl_multi_init());
        /* add the individual transfers */
        nxCurlSo(curl_multi_add_handle(s_stCurlStatus.multi_handle, http_handle));
        /* we start some action by calling perform right away */
        curlMCode = nxCurlSo(curl_multi_perform(s_stCurlStatus.multi_handle, &(s_stCurlStatus.status)));

        if((HINT32)CURLE_OK != (HINT32)curlMCode)
        {
            HxLOG_Print("\n[%s.%d] ----------->> curl_multi_perform Error : %d \n\n\n",__FUNCTION__,__LINE__,curlMCode);
            return ERR_FAIL;
        }

        //pthread_create

#if defined(CONFIG_PROD_OS_EMULATOR)
		pthread_create( &s_ulCurlThreadId, NULL, (pthread_routine)nx_core_curlapi_MultiCurlTask, NULL);
#else
		pthread_create( (pthread_t*)&s_ulCurlThreadId, NULL, (pthread_routine)nx_core_curlapi_MultiCurlTask, NULL);
#endif
    }
#endif
    return ERR_OK;
}
