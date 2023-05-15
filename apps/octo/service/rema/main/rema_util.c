/**************************************************************
*	@file	rema_util.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			rema_util
**************************************************************/

/*
* ⓒ 2011-2012 Humax Co., Ltd.
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

#define ___HEADER_FILES___

#include <vkernel.h>
#include <hlib.h>
#include <napi.h>
#include <hapi.h>

#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <rema_int.h>

#include "rema_util.h"

#define ___DEFINES___

#define ___MACROS___

#define ___TYPEDEF___

#define ___STATIC_VARIABLES___

#define ___INTERNAL_FUNCTIONS___
#define ___PUBLIC_FUNCTIONS___

#define HOST_FOR_EXTERNAL_IP	"wget -qO- -t 5 http://ipecho.net/plain"


#if 0
#define IPIFY_HOST    "api.ipify.org"
#define AGENT_NAME    "lipify/0.2"
#define HTTP_REQUEST				\
	"GET / HTTP/1.0\r\n"			\
	"Host: " IPIFY_HOST "\r\n"		\
	"User-Agent: " AGENT_NAME "\r\n\r\n";

int rema_util_connect(void)
{
#if 0
	int ret, sd;

	struct addrinfo hints, *result;
	int i = 0;

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd < 0)
		return -1;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family   = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	ret = getaddrinfo(IPIFY_HOST, "http", &hints, &result);
	if (ret || !result) {
		close(sd);
		return -1;
	}

	for(i = 0; i < 14; i++)
	{
		printf("%d : %d\n", i, result->ai_addr->sa_data[i]);
	}

	HxLOG_Info("\033[01;35m[%s:%s:%d] szIP : %s (%d) \033[00m\n", __FILE__, __FUNCTION__, __LINE__, result->ai_addr->sa_data,result->ai_addrlen);
	
	ret = connect(sd, result->ai_addr, result->ai_addrlen);
	freeaddrinfo(result);

	if (ret < 0) {
		close(sd);
		return -1;
	}

	return sd;
#else
	int sd;

	struct hostent					*pstHostEnt;
	struct in_addr					stInAddr;
	struct sockaddr_in 				stServerAddr;
	int	i = 0, nResult = ERR_FAIL;
	long int	*add;

	pstHostEnt = gethostbyname(IPIFY_HOST);

	if (pstHostEnt == NULL)
	{
		HxLOG_Error("[CILSC] Error at gethostbyname()!!\n");
		HxLOG_Error("[CILSC] Error : [%d]\n", errno);
		return ERR_FAIL;
	}
	else
	{
		HxLOG_Info("[LSC] Official Host Name : [%s]\n", pstHostEnt->h_name);
		while(pstHostEnt->h_aliases[i] != NULL)
		{
			HxLOG_Info("[LSC] Aliases Name : [%s]\n", pstHostEnt->h_aliases[i]);
			i++;
		}
		HxLOG_Info("[LSC] Host address type : [%d]\n", pstHostEnt->h_addrtype);
		HxLOG_Info("[LSC] Length of Host Addr : [%d]\n", pstHostEnt->h_length);
		i = 0;

		while(*pstHostEnt->h_addr_list != NULL)
		{
			add = (long int *)*pstHostEnt->h_addr_list;

			stInAddr.s_addr = *add;
			++pstHostEnt->h_addr_list;
		}
	}

	sd = socket(AF_INET, SOCK_STREAM, 0);

	/* 연결할 서버의 소켓주소 구조체 작성 */
	bzero((char *)&stServerAddr, sizeof(stServerAddr));
	stServerAddr.sin_family = pstHostEnt->h_addrtype;
	stServerAddr.sin_addr.s_addr = stInAddr.s_addr;

	stServerAddr.sin_port = 80;

	nResult = connect(sd, (struct sockaddr *)&stServerAddr, sizeof(stServerAddr));

	return sd;
#endif	
}

int rema_util_query(int sd, char *addr, size_t len)
{
	int ret;
	char buf[512], *ptr;
	const char *req = HTTP_REQUEST;

	ret = send(sd, req, strlen(req), 0);
	if (ret < 0)
		return ret;
	ret = recv(sd, buf, sizeof(buf), 0);
	if (ret < 0)
		return ret;
	ptr = strstr(buf, "200 OK");
	if (!ptr)
		return 1;
	ptr = strstr(ptr, "\r\n\r\n");
	if (!ptr)
		return 1;
	ptr += 4;
	strncpy(addr, ptr, len);
	return 0;
}

int rema_util_disconnect(int sd)
{
	shutdown(sd, SHUT_RDWR);
	return close(sd);
}
#endif


HINT32 REMA_Util_Get_ExternalIP(HCHAR *addr, size_t len)
{
#if 0
	HINT32 sd, ret;
	sd = rema_util_connect();
	if (sd < 0)
		return 1;

	ret  = rema_util_query(sd, addr, len);
	ret |= rema_util_disconnect(sd);
	return ret;
#else

    FILE *fp = NULL;
    char buff[MAX_IP_LEN];
	HINT32 state = -1;

	REM_FUNC_ENTER;
	
    fp = popen(HOST_FOR_EXTERNAL_IP, "r");
    if (fp == NULL)
    {
        RemLOG_Error("Cannot External IP!!!\n");
        return state;
    }

    while(fgets(buff, MAX_IP_LEN, fp) != NULL)
    {
        RemLOG_Info("IP : %s\n", buff);
    }

	HxSTD_MemCopy(addr, buff, strlen(buff));

	if(fp)
	{
    	state = pclose(fp);
		fp = NULL;
	}
	RemLOG_Info("state = %d\n", state);

	REM_FUNC_LEAVE;

	return state;
#endif	
}


#define ___EXTERNAL_FUNCTIONS___

#define _____END_OF_FILE_________

