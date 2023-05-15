/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   util_sys.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:		SungYong Jeon $
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date: Fri June 05 10:20:48 KST 2009 $
 * File Description:	not so strong MD5 hashing
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
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <unistd.h>

#include "di_iptuner_common.h"
#include "di_iptuner_config.h"
#include "util_sys.h"
#include "vkernel.h"
/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
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
HBOOL UTIL_SYS_GetLinkStatus (void)
{
	int retVal;
	struct ifreq ifr;
	int sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);

	if(sock < 0)
	{
		//PrintError("%s(%d) Error> socket() Error\n", __FUNCTION__, __LINE__);
		return FALSE;
	}

	VK_memset(&ifr, 0x0, sizeof(struct ifreq));
	VK_strncpy(ifr.ifr_name, "eth0", IF_NAMESIZE-1);

	retVal = ioctl (sock, SIOCGIFFLAGS, &ifr);
	if (retVal < 0)
	{
//		PrintError ("%s(%d) Error> Interface %s not found!\n", ifr.ifr_name);
		close(sock);
		return FALSE;
	}

	if ( ((ifr.ifr_flags & IFF_UP) == IFF_UP)
			&& ((ifr.ifr_flags & IFF_RUNNING) == IFF_RUNNING) )
	{
		close(sock);
		return TRUE;
	}
	else
	{
		close(sock);
		return FALSE;
	}
}
