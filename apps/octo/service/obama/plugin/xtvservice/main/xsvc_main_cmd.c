/********************************************************************
 * $Workfile:   cmd_channel.c  $
 * Project    : COMMON(ST)
 * Author     : Jino Lee
 * Description:
 *
 *                                 Copyright (c) 2002 HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/


/********************************************************************
 Header Files
 ********************************************************************/
#include <stdio.h>
#include <string.h>

#include <htype.h>
#include <vkernel.h>

//#include <cmd_tvsvc.h>
#include <xsvc_cmd.h>


/********************************************************************
 Macro Definition
 ********************************************************************/

/********************************************************************
 Global/Static Variable Definition
 ********************************************************************/

void CMD_MW_EXT_Init(void)
{
#if defined(CONFIG_OP_FREESAT)
	XCMD_RegisterFsat();
#endif
}


