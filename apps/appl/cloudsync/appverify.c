/**************************************************************
*	http://www.humaxdigital.com
*	@author			humax - Europe/CIS team, hcyoo@humaxdigital.com
**************************************************************/
/*
* ¨Ï 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <vkernel.h>
#include <openssl/crypto.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <apk.h>
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define ZIGGO_ALPHA_GET_TOKEN	"http://wsl-ot.ziggo.nl/getToken"
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

HERROR _av_GetToken( HCHAR **ppToken, HUINT32 *ulSize )
{
	HERROR  hErr;
	HxTRANSPORT_Http_t*	hCurl;
	HINT8 *buf = NULL;
	HFLOAT64 size;
	HUINT32 bufSize = 0;
	HxTRANSPORT_HttpResponse_e eResponse;

	HxLOG_Trace();

	if ( ppToken == NULL || ulSize == NULL )
	{
		HxLOG_Error( "[%s] Param error!!!\n", __FUNCTION__ );
		return ERR_FAIL;
	}

	/* Open curl. */
	hCurl = HLIB_TRANSPORT_HttpOpen();
	if ( hCurl == NULL )
	{
		HxLOG_Error( "[%s] Open curl fail!!!\n", __FUNCTION__ );
		return ERR_FAIL;
	}

	/* Get data. */
	hErr = HLIB_TRANSPORT_HttpGet( hCurl, ZIGGO_ALPHA_GET_TOKEN, &buf, &size );
	if ( hErr != ERR_OK )
	{
		HLIB_TRANSPORT_HttpFreeBuffer( buf );
		HxLOG_Error( "[%s] GetWithCallback fail!!!\n", __FUNCTION__ );
		return ERR_FAIL;
	}

	/* Get ResponseCode. */
	hErr = HLIB_TRANSPORT_HttpGetHttpResponseCode( hCurl, &eResponse );
	if( eResponse != eHxTP_HTTP_RESCODE_OK )
	{
		HxLOG_Error( "[%s] GetHttpResponseCode %d\n", __FUNCTION__, eResponse );

		HLIB_TRANSPORT_HttpFreeBuffer( hCurl );
		HLIB_TRANSPORT_HttpClose( hCurl );

		return ERR_FAIL;
	}

	bufSize = (HUINT32)HxSTD_StrLen( buf );

	/* Debug Print. */
	if ( buf != NULL )
	{
		HxLOG_Debug( "[%s]\nsize - %d\nstring - %s\n", __FUNCTION__, (HUINT32)bufSize, buf );
	}
	else
	{
		HxLOG_Debug( "[%s] No data.\n", __FUNCTION__ );

		/* Close. */
		HLIB_TRANSPORT_HttpFreeBuffer( buf );
		HLIB_TRANSPORT_HttpClose( hCurl );

		return ERR_FAIL;
	}

	if ( buf != NULL && bufSize > 0 )
	{
		HCHAR *p = NULL;

		p = (HCHAR *)HLIB_MEM_Calloc( bufSize + 1 );
		HxSTD_MemCopy( p, buf, bufSize );
		*ppToken = p;
	}

	HxLOG_Debug( "[%s] test.\n%s\n", __FUNCTION__, *ppToken);

	*ulSize = bufSize;

	/* Close. */
	HLIB_TRANSPORT_HttpFreeBuffer( buf );
	HLIB_TRANSPORT_HttpClose( hCurl );

	return ERR_OK;
}


/*
 *	1.The server token is base64 encoded byte array and should be unpacked first.
 *	2.The client token that is generated consists of 2 parts:
 *		a.A hashed byte array
 *		b.The random byte array that was used to construct the hash.  Both the random number and hash should have an equal size (both are 20 bytes in length).
 *	3.To construct the hash, the following operations are performed conscutivelya.Create an empty SHA-1 hash (20 bytes)
 *		b.Update the hash using the server token (base64 decoded)
 *		c.Update the hash using a random byte-array (20 bytes)
 *		d.Update the hash using the hidden secret Perform a byte-wise exclusive OR using the SHA-1 hash and the random number
 *	4.Encode the concatenate byte string (of 40 byes) using base64 encoding
 */

HCHAR* _av_CalculateToken ( HCHAR *pServerToken, HUINT32 ulSize )
{
	SHA_CTX	ctx;
	HUINT8	hash[SHA_DIGEST_LENGTH];
	HUINT8	randBytes[SHA_DIGEST_LENGTH];
	HCHAR	*pBase64ServerToken = NULL;
	HUINT32	len = 0;
	HCHAR	token[SHA_DIGEST_LENGTH*2];
	HUINT32 i;
	HCHAR	*pBase64Token = NULL;

	HxLOG_Trace();

	if ( pServerToken == NULL || ulSize == 0 )
	{
		HxLOG_Error( "[%s] Server Token NULL!!\n", __FUNCTION__ );
		return NULL;
	}

	/* Init SHA1 context. */
	if ( SHA1_Init( &ctx ) != 1 /* success (1) */ )
	{
		HxLOG_Error( "[%s] Init SHA1 context error!\n", __FUNCTION__ );
		return NULL;
	}

	/* 1.The server token is base64 encoded byte array and should be unpacked first. */
	len = ulSize / 4 * 3;
	if ( pServerToken[ulSize - 1] == '=')	len--;
	if ( pServerToken[ulSize - 2] == '=')	len--;

	pBase64ServerToken = (HCHAR *)HLIB_MEM_Calloc( len + 1 );
	if ( pBase64ServerToken == NULL )
	{
		HxLOG_Assert( pBase64ServerToken == NULL );
		return NULL;
	}

	len = HLIB_MATH_DecodeBase64( pBase64ServerToken, pServerToken, ulSize );

#if defined(CONFIG_DEBUG)
	HxLOG_Debug( "ServerToken Base64 : \n");
	for ( i = 0; i < len; i++ )
	{
		printf( "%02x ", pBase64ServerToken[i] );
	}

	HxLOG_Debug( "\n");
#endif

	/*
	 *	2.The client token that is generated consists of 2 parts:
	 *	   a.A hashed byte array
	 *	   b.The random byte array that was used to construct the hash.  Both the random number and hash should have an equal size (both are 20 bytes in length).
	 */

	/* Get Random Value (20bytes) */
	HxSTD_memset( randBytes, 0, SHA_DIGEST_LENGTH );
	if ( RAND_pseudo_bytes( randBytes, SHA_DIGEST_LENGTH ) <= 0 )
	{
		HxLOG_Error( "[%s] Get Random Value error!\n", __FUNCTION__ );
		return NULL;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Debug( "randBytes : \n");

	for ( i = 0; i < SHA_DIGEST_LENGTH; i++ )
	{
		printf( "%02x ", randBytes[i] );
	}

	HxLOG_Debug( "\n");
#endif

	/*
	 *	3.To construct the hash, the following operations are performed conscutively
	 *	   a.Create an empty SHA-1 hash (20 bytes)
	 *	   b.Update the hash using the server token (base64 decoded)
	 *	   c.Update the hash using a random byte-array (20 bytes)
	 *	   d.Update the hash using the hidden secret Perform a byte-wise exclusive OR using the SHA-1 hash and the random number
	 */
	SHA1_Update( &ctx, (HUINT8 *) pBase64ServerToken, len );
	SHA1_Update( &ctx, (HUINT8 *) randBytes, SHA_DIGEST_LENGTH );
	// TODO: update hidden secret Perform.

	if ( pBase64ServerToken != NULL )
		HLIB_MEM_Free( pBase64ServerToken );

	if ( SHA1_Final( hash, &ctx ) != 1 /* success (1) */ )
	{
		HxLOG_Error( "[%s] SHA1_Final error!\n", __FUNCTION__ );
		return NULL;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Debug( "hash : \n");

	for ( i = 0; i < SHA_DIGEST_LENGTH; i++ )
	{
		printf( "%02x ", hash[i] );
	}

	HxLOG_Debug( "\n");
#endif

	HxSTD_memset( token, 0, SHA_DIGEST_LENGTH * 2 );
	for ( i = 0; i < SHA_DIGEST_LENGTH; i++ )
	{
		token[i] = hash[i] ^ randBytes[i];
	}

	for ( i = 0; i < SHA_DIGEST_LENGTH; i++ )
	{
		token[i + SHA_DIGEST_LENGTH] = randBytes[i];
	}

	/*  4.Encode the concatenate byte string (of 40 byes) using base64 encoding. */
	len = SHA_DIGEST_LENGTH * 2;
	pBase64Token = (HCHAR *)HLIB_MEM_Calloc ((len / 3 + ((len % 3) ? 1 : 0)) * 4 + 1 + 1);

	len = HLIB_MATH_EncodeBase64( pBase64Token, token, len );
	HxLOG_Debug( "pBase64Token : %s\n", pBase64Token );

	return pBase64Token;
}

HERROR AV_GetAppVerifyToken( HCHAR **ppToken )
{
	HERROR  hErr;
	HUINT32 size = 0;
	HCHAR 	*pServerToken = NULL;

	HxLOG_Trace();

	hErr = _av_GetToken( &pServerToken, &size );
	if ( hErr != ERR_OK )
	{
		HxLOG_Error( "[%s] Get Token error!\n", __FUNCTION__ );
		return ERR_FAIL;
	}

	*ppToken = _av_CalculateToken( pServerToken, size );

	return ERR_OK;
}


