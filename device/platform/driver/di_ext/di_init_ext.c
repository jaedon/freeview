/*********************************************************************

DI Extension Init module

Module : DI Extension Init
File name : di_ext_init.c

**********************************************************************/
/* header */
#include <stdio.h>

#include "nexus_base_types.h"
#include "nexus_platform.h"

#include "htype.h"
#include "vkernel.h"
#include "di_err.h"
#include "di_demux_ext.h"
#include "di_err_ext.h"

/*********************************************************************/
/* define */

/*********************************************************************/
/* function */

/*********************************************************************/
/* static */

/*********************************************************************/
/* code */

DI_ERR_CODE DI_EXT_Init(void)
{
	NEXUS_Error eNexusErr = NEXUS_SUCCESS;
	HUINT32 vkErr = 0;
	DI_ERR_CODE eDiErr = DI_ERR_ERROR;

	(void)NEXUS_Platform_Join();

	vkErr = VK_Init();
	if(vkErr != VK_OK)
	{
		PrintError(" VK_Init(): Error Code(%d)\n", vkErr);
		return DI_ERR_ERROR;
	}
	else
	{
		PrintDebug(" VK_Init(): OK!\n");
	}

	eDiErr = DI_DEMUX_EXT_Init();
	if(eDiErr != 0)
	{
		PrintError(" DI_DEMUX_EXT_Init(): Error Code(%d)\n", eDiErr);
		return DI_ERR_ERROR;
	}
	else
	{
		PrintDebug(" DI_MHI_DEMUX_Init(): OK!\n");
	}

	return DI_ERR_OK;
}

