
/********************************************************************
* Workfile   : vk_ftp.c
* Project    :
* Author     :
* Description: Commands Template
*
*                                 Copyright (c) 2009 HUMAX Co., Ltd.
*                                               All rights reserved.
********************************************************************/

#define _______________________________________________________________________
#define ____HEADER_FILES____

#include "vkernel.h"

#define _______________________________________________________________________
#define ____MACRO_DEFINITION____


#define _______________________________________________________________________
#define ____EXTERNAL_VARIABLE_DEFINITION____


#define _______________________________________________________________________
#define ____EXTERNAL_FUNCTION_DEFINITION____


#define _______________________________________________________________________
#define ____TYPE_DEFINITION____


#define _______________________________________________________________________
#define ____GLOBAL_VARIABLE_DEFINITION____


#define _______________________________________________________________________
#define ____STATIC_VARIABLE_DEFINITION____


#define _______________________________________________________________________
#define ____STATIC_FUNCTION_DEFINITION____

#define _______________________________________________________________________
#define ____STATIC_FUNCTION_BODY____

#define _______________________________________________________________________
#define ____GLOBAL_FUNCTION_BODY____

/********************************************************************
function:  VK_SYSTEM_Command
description :
argument:
return:
*********************************************************************/

int VK_TCP_socket(void)
{
	return VK_OK;
}


int VK_TCP_connect(int s, unsigned char * dest, unsigned short port)
{
	(void)(s);
	(void)(dest);
	(void)(port);
	return VK_OK;
}

int VK_TCP_close(int s)
{
	(void)(s);
	return VK_OK;
}

int VK_TCP_send(int s, unsigned char * buf, int len)
{
	(void)(s);
	(void)(buf);
	(void)(len);
	return VK_OK;
}

int VK_TCP_recv(int s, unsigned char * buf, int len)
{
	(void)(s);
	(void)(buf);
	(void)(len);
	return VK_OK;
}

int VK_TCP_status(int s, int *connflag, int *rxready, int *rxeof)
{
	(void)(s);
	(void)(connflag);
	(void)(rxready);
	(void)(rxeof);
	return VK_OK;
}


