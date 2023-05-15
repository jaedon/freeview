/**************************************************************************************/
/**
 * @file hlib_transport.c
 * defines APIs's body
 *
 * Dynamic loading module for libCurl - Http / Ftp
 *
**************************************************************************************
**/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <dlfcn.h>

#include <hlib.h>

#include "hlib_transport_int.h"

/*******************************************************************************************
 * Definitions
 ******************************************************************************************/


/*******************************************************************************************
 * Static functions
 ******************************************************************************************/

/*******************************************************************************************
 * Extern functions
 ******************************************************************************************/
curl_t*		HLIB_TRANSPORT_GetInstance(void)
{
	static curl_t	*s_pstCurl = NULL;

	if (s_pstCurl == NULL)
	{
		s_pstCurl = HLIB_MEM_Calloc(sizeof(curl_t));
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

		HxLOG_Assert(s_pstCurl->_curl_global_init && s_pstCurl->_curl_global_init && s_pstCurl->_curl_easy_init);
		HxLOG_Assert(s_pstCurl->_curl_easy_setopt && s_pstCurl->_curl_easy_cleanup && s_pstCurl->_curl_easy_perform);
		HxLOG_Assert(s_pstCurl->_curl_easy_getinfo && s_pstCurl->_curl_slist_append && s_pstCurl->_curl_slist_free_all);
		HxLOG_Assert(s_pstCurl->_curl_formadd && s_pstCurl->_curl_formfree);

		HxLOG_Warning("libCurl dynamic loading is done...\n");
	}

	return s_pstCurl;
}

