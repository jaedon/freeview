/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   util_url.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:		SungYong Jeon $
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date: Fri June 05 10:20:48 KST 2009 $
 * File Description:	URL Parser.
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd. 								*/
/* All rights reserved.												*/
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "htype.h"
#include "vkernel.h"
#include "di_uart.h"
#include "util_url.h"
#include "di_iptuner_config.h"
/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define UTIL_URL_DEBUG
//#define STREAM_HTTP_CRITICAL_DEBUG

#ifdef IPTUNER_ALL_DEBUG	/* DEBUG Define */

#ifdef UTIL_URL_DEBUG
#define PrintData		DI_UART_Print
#define PrintDebug		DI_UART_Print
#define PrintError		DI_UART_Print
#define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#define PrintError		DI_UART_Print
#endif

#else

#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#endif /* The END of DEBUG DEFINE */

#ifdef UTIL_URL_DEBUG
#define PrintCriticalDebug      DI_UART_Print
#else
#define PrintCriticalDebug      while (0) ((int (*)(char *, ...)) 0)
#endif

#define UNUSED_PARAM(x)  ((void)x)
/* End #define */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
/* End typedef */


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
/* Start global variable */

/* End global variable */


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
/* Start static variable */
/* End static variable */


/*******************************************************************/
/************************ static funtions **************************/
/*******************************************************************/
/**
 * Replaces invalid/overlong UTF-8 sequences with question marks.
 * Note that it is not possible to convert from Latin-1 to UTF-8 on the fly,
 * so we don't try that, even though it would be less disruptive.
 *
 * @return str if it was valid UTF-8, NULL if not.
 */
static char *int_CheckUTF8( char *str, char rep )
{
    HUINT8 *ptr = (HUINT8 *)str;

    assert (str != NULL);

    for (;;)
    {
        HUINT8 c = ptr[0];
        int charlen = -1;

        if (c == '\0')
		{
            break;
		}

        for (int i = 0; i < 7; i++)
		{
            if ((c >> (7 - i)) == ((0xff >> (7 - i)) ^ 1))
            {
                charlen = i;
                break;
            }
		}

        switch (charlen)
        {
            case 0: // 7-bit ASCII character -> OK
                ptr++;
                continue;

            case -1: // 1111111x -> error
            case 1: // continuation byte -> error
                goto error;
        }

        assert (charlen >= 2);

        HINT32 cp = c & ~((0xff >> (7 - charlen)) << (7 - charlen));
        for (int i = 1; i < charlen; i++)
        {
            assert (cp < (1 << 26));
            c = ptr[i];

            if ((c == '\0') // unexpected end of string
             || ((c >> 6) != 2)) // not a continuation byte
			 {
                goto error;
			 }

            cp = (cp << 6) | (ptr[i] & 0x3f);
        }

        if (cp < 128) // overlong (special case for ASCII)
		{
            goto error;
		}
#if 0
        if (cp < (1u << (5 * charlen - 3))) // overlong
#else
        if (cp < (1 << (5 * charlen - 3))) // overlong
#endif
		{
            goto error;
		}

        ptr += charlen;
        continue;

    error:
        if (rep == 0)
		{
            return NULL;
		}
        *ptr++ = rep;
        str = NULL;
    }

    return str;
}

static void int_decode_URI( char *psz )
{
    unsigned char *in = (unsigned char *)psz, *out = in, c;
    if( psz == NULL )
	{
        return;
	}

    while( ( c = *in++ ) != '\0' )
    {
        switch( c )
        {
            case '%':
            {
                char hex[3];

                if( ( ( hex[0] = *in++ ) == 0 )
                 || ( ( hex[1] = *in++ ) == 0 ) )
				{
                    return;
				}

                hex[2] = '\0';
                *out++ = (unsigned char)strtoul( hex, NULL, 0x10 );
                break;
            }

            case '+':
                *out++ = ' ';
                break;

            default:
                /* Inserting non-ASCII or non-printable characters is unsafe,
                 * and no sane browser will send these unencoded */
                if( ( c < 32 ) || ( c > 127 ) )
				{
                    *out++ = '?';
				}
                else
				{
                    *out++ = c;
				}
        }
    }
    *out = '\0';
    UTIL_EnsureUTF8( psz );
	return;
}

int INT_GetLengthFromUINT (unsigned int nNumber)
{
	char pTmp[64];
	VK_memset(pTmp,0,sizeof(pTmp));
	(void) VK_snprintf (pTmp,sizeof(pTmp), "%u",nNumber);
	return VK_strlen(pTmp);
}

char *UTIL_AllocUniqueURL (unsigned int nTunerId, const char* pUrl)
{
    char *pTarget=NULL;
    int nOrgLength = 0;
    int nTargetLength=0;

    if(pUrl == NULL) return NULL;

	nOrgLength = VK_strlen (pUrl);
    nTargetLength = 6 + nOrgLength + INT_GetLengthFromUINT (nTunerId) + 1 ;

    pTarget = (char*) VK_malloc (nTargetLength);
    if(pTarget==NULL) return NULL;

    VK_memset(pTarget,0,nTargetLength);

    if ( VK_snprintf ( pTarget,nTargetLength, "{TID=%u}%s", nTunerId, pUrl) < 0)
	{
		VK_free (pTarget);
		nTargetLength = 0;
		return NULL;
	}

    return pTarget;
}


char *UTIL_EnsureUTF8( char *str )
{
    return int_CheckUTF8( str, '?' );
}
/**
 * Decode encoded URI string in place
 * \return nothing
 */

char* UTIL_GetOriginalURL (const char *pUniqueUrl)
{
	char *p = NULL;
	char *q = NULL;
	char *r = NULL;

	p = (char *)pUniqueUrl;

	if (p==NULL) return NULL;

	q = VK_strstr (pUniqueUrl,"://");
	if ( q==NULL ) return NULL;

	// skip [DLNA] or [DMP] prefix
	while (VK_strchr(p, '[') != NULL)
	{
		if(*(p + 0) != '[')
		{
			break;
		}
		p = VK_strchr(p, ']');
		p++;
	}

	for ( r=p; r<q ; r++)
	{
		if (*r == '}')
		{
			break;
		}
	}

	if (r == q)
		return p;

	return (char*)++r;
}
void UTIL_UrlClean ( URL_t *url )
{
	if(url != NULL)
	{
		if(url->psz_buffer != NULL)
		{
	   		VK_MEM_Free ( url->psz_buffer );
			url->psz_buffer = NULL;
		}

		if(url->psz_host != NULL)
		{
	   		VK_MEM_Free ( url->psz_host );
			url->psz_host = NULL;
		}
#if SUPPORT_COOKIE
		if(url->psz_cookie != NULL)
		{
			DD_MEM_Free(url->psz_cookie);
			url->psz_cookie = NULL;
		}
#endif
	    url->psz_protocol = NULL;
	    url->psz_username = NULL;
	    url->psz_password = NULL;
	    url->psz_host     = NULL;
	    url->i_port       = 0;
	    url->psz_path     = NULL;
	    url->psz_option   = NULL;

	}
	return;
}

/* psz_url example
 * mms://username:password@xxx.xxx.xxx.xxx:port_number/PublishingPoint1/test.asf
 */
void UTIL_UrlParse ( URL_t *url, const char *purl, char option )
{
    char *pdup=NULL;
    char *pparse=NULL;
    char *p=NULL;
    char *p2=NULL;

#if SUPPORT_COOKIE
	int  sz_cookie_url = 0;
	char *p_cookie_path=NULL;
	char *p_path=NULL;
#endif
	char *porig=NULL;

	PrintEnter;

    url->psz_buffer   = NULL;
    url->psz_protocol = NULL;
    url->psz_username = NULL;
    url->psz_password = NULL;
    url->psz_host     = NULL;
    url->i_port       = 0;
    url->psz_path     = NULL;
    url->psz_option   = NULL;

    if( purl == NULL )
    {
        url->psz_buffer = NULL;
        return;
    }

	porig = UTIL_GetOriginalURL (purl);
	if (porig == NULL)
	{
        url->psz_buffer = NULL;
        return;
	}

	/* The url doesn't include TID String. */
	if (porig == purl)
	{
		url->psz_buffer = pparse = pdup = strdup ( purl );
	}
	/* The url includes TID String.*/
	else
	{
		/* store the pointer before getting real url string from unique string.*/
		url->psz_buffer = pparse = pdup = strdup ( porig );
	}

    /* Search a valid protocol */
    p  = VK_strstr( pparse, ":/" );
    if( p != NULL )
    {
        for( p2 = pparse; p2 < p; p2++ )
        {
#define I(string,start,end) ( (start) <= (string) && (string) <= (end) )
            if( !I(*p2, 'a', 'z' ) && !I(*p2, 'A', 'Z') && !I(*p2, '0', '9') && *p2 != '+' && *p2 != '-' && *p2 != '.' )
            {
                p = NULL;
                break;
            }
#undef I
        }
    }

    if( p != NULL )
    {
        /* we have a protocol */

        /* skip :// */
        *p++ = '\0';
        if( p[1] == '/' )
		{
            p += 2;
		}
        url->psz_protocol = pparse;
        pparse = p;
    }
	/** pparse : xxx.xxx.xxx.xxx/pp1/pororo.asf */
    p = VK_strchr( pparse, '@' );
	/** p */
    p2 = VK_strchr( pparse, '/' );
	/** p2 : /pp1/proro.asf */
    if( p != NULL) /* exist user name */
    {
		if( p2 == NULL ? 1 : p < p2 ) /* if p2 is null, 1 or p<p2 */
    {
        /* We have a login */
		    url->psz_username = pparse;
        *p++ = '\0';

		    pparse = VK_strchr( pparse, ':' );
		    if( pparse != NULL ) /* exist password */
        {
            /* We have a password */
		        *pparse++ = '\0';
		        url->psz_password = pparse;
            int_decode_URI( url->psz_password );

#ifdef UTIL_URL_DEBUG
				PrintData ("Line : %d, [URL_t] psz_password  = [%s]\n", __LINE__, url->psz_password);
#endif
        }

        int_decode_URI( url->psz_username );
#ifdef UTIL_URL_DEBUG
			PrintData ("Line : %d, [URL_t] psz_username  = [%s]\n", __LINE__, url->psz_username);
#endif
		    pparse = p;
    }
    }

    p = VK_strchr( pparse, '/' );

    if( ( p == NULL ) || ( pparse < p ) )
    {
        /* We have a host[:port] */
        url->psz_host = strdup( pparse );
        if( p != NULL )
        {
            url->psz_host[p - pparse] = '\0';
#ifdef UTIL_URL_DEBUG
			PrintData ("Line : %d, [URL_t] psz_host  = [%s]\n", __LINE__, url->psz_host);
#endif
        }

        if( *url->psz_host != '[' )
        {
            p2 = VK_strchr( url->psz_host, ':' );
        }
		else
        {
            /* Ipv6 */
            p2 = VK_strchr( url->psz_host, ']' );
            if( p2 != NULL )
            {
                p2 = VK_strchr( p2, ':' );
            }
        }

        if( p2 != NULL )
        {
            *p2++ = '\0';
            url->i_port = VK_atoi( p2 );
#ifdef UTIL_URL_DEBUG
			PrintData ("Line : %d, [URL_t] i_port  = [%d]\n", __LINE__, url->i_port);
#endif
        }
    }
    pparse = p;

    /* Now parse psz_path and psz_option */
    if( pparse != NULL )
    {
        url->psz_path = pparse;
#ifdef UTIL_URL_DEBUG
		PrintData ("Line : %d, [URL_t] psz_path  = [%s]\n", __LINE__, url->psz_path);
#endif
        if( option != '\0' )
        {
            p = VK_strchr( url->psz_path, option );
            if( p != NULL )
            {
                *p++ = '\0';
                url->psz_option = p;
#ifdef UTIL_URL_DEBUG
				PrintData ("Line : %d, [URL_t] psz_option  = [%s]\n", __LINE__, url->psz_option);
#endif
            }
        }
    }

#if SUPPORT_COOKIE
	if(url->psz_path != NULL)
	{
		p_path = VK_strstr(url->psz_path,"/HMXCOOKIE=");
		if(p_path != NULL)
		{
			sz_cookie_url = VK_strlen(p_path);
			if(sz_cookie_url>0)
			{
				sz_cookie_url = sz_cookie_url - 11;
				p_cookie_path = DD_MEM_Alloc(sz_cookie_url+1);
				if(p_cookie_path != NULL)
				{
					VK_MEM_Memset(p_cookie_path, 0x0, sz_cookie_url);
					VK_MEM_Memcpy(p_cookie_path, p_path+11, sz_cookie_url);
				}
				else
				{
					PrintError ( "%s (%d) Error> Fail Mem Allloc \n",__FUNCTION__,__LINE__ );
				}
			}

			p_path[0] = '\0';

			if(NULL != p_cookie_path)
			{
				p_cookie_path[sz_cookie_url-1] = '\0';
				url->psz_cookie = p_cookie_path;
			}
		}
	}
#endif

#ifdef UTIL_URL_DEBUG
	PrintData ("[URL_t] psz_protocol  = [%s]\n",url->psz_protocol);
	PrintData ("[URL_t] psz_username  = [%s]\n",url->psz_username);
	PrintData ("[URL_t] psz_password  = [%s]\n",url->psz_password);
	PrintData ("[URL_t] psz_host      = [%s]\n",url->psz_host);
	PrintData ("[URL_t] i_port        = [%d]\n",url->i_port);
	PrintData ("[URL_t] psz_path      = [%s]\n",url->psz_path);
	PrintData ("[URL_t] psz_option    = [%s]\n",url->psz_option);
	PrintData ("[URL_t] psz_buffer    = [%s]\n",url->psz_buffer);
#if SUPPORT_COOKIE
	PrintData ("[URL_t] psz_cookie    = [%s]\n",url->psz_cookie);
#endif
#endif
	PrintExit;
	return;
}

/*
 * Notice :
 * This function return a pointer which is allocated in heap.
 * After this function , a pointer should be free.
 * This function is made exactly like strdup in posix.
 */
char* UTIL_UrlAllocWithNewName (const char *pOrg, const char *pNewName )
{
	char *pDest=NULL;
	int nDir=0,nName=0;

	if(pOrg==NULL)
		return NULL;

	nDir  = strrchr(pOrg,'/') - pOrg + 1;
	nName = VK_strlen (pNewName);

	pDest = DD_MEM_Alloc (nDir+nName+1);
	if(pDest == NULL)
	{
		PrintError ("%s(%d) Error> DD_MEM_Alloc failed\n",__FUNCTION__,__LINE__);
		return NULL;
	}

	VK_memset(pDest,0,nDir+nName+1);

	(void) VK_strncpy (pDest,pOrg,nDir);
	(void) VK_strncat (pDest,pNewName,nName);
	pDest[nDir+nName]='\0';

	return pDest;

}
