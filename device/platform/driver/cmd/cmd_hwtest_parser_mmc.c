/*----------------------------------------------------------------------------
** Include
** --------------------------------------------------------------------------*/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <limits.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

#include "htype.h"
#include "di_err.h"
#include "vkernel.h"
#include "taskdef.h"
#include "cmdtool.h"
#include "di_mmc.h"

/*----------------------------------------------------------------------------
** Define
** --------------------------------------------------------------------------*/
#define HWTEST_CMD          szCmd
#define HWTEST_PARAM        szParam
#define HWTEST_PARAM1       szParam1
#define HWTEST_PARAM2       szParam2
#define HWTEST_PARAM3       szParam3
#define GET_ARGS            int iResult=CMD_ERR; \
                            char *HWTEST_CMD=CMD_NextArg((char **)&szArgs); \
                            char *HWTEST_PARAM=CMD_NextArg((char **)&szArgs); \
                            char *HWTEST_PARAM1=CMD_NextArg((char **)&szArgs); \
                            char *HWTEST_PARAM2=CMD_NextArg((char **)&szArgs); \
                            char *HWTEST_PARAM3=CMD_NextArg((char **)&szArgs);

#define CMD_IS(x)           (HWTEST_CMD!=NULL && VK_strcmp(HWTEST_CMD, x)==0)
#define PARAM_IS(x)         (HWTEST_PARAM!=NULL && VK_strcmp(HWTEST_PARAM, x)==0)
#define PARAM1_IS(x)        (HWTEST_PARAM1!=NULL && VK_strcmp(HWTEST_PARAM1, x)==0)
#define PARAM2_IS(x)        (HWTEST_PARAM2!=NULL && VK_strcmp(HWTEST_PARAM2, x)==0)
#define PARAM3_IS(x)        (HWTEST_PARAM3!=NULL && VK_strcmp(HWTEST_PARAM3, x)==0)

#define READABLE_IN_HEX(sz,val)    CMD_ReadAsHex(sz, &val)
#define READABLE_IN_DEC(sz,val)    CMD_ReadAsDec(sz, &val)

/*----------------------------------------------------------------------------
** Variable
** --------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
** Functions
** --------------------------------------------------------------------------*/
int CMD_HwTest_Parser_MMC(void *szArgs)
{
	int port;

	GET_ARGS;

	(void)HWTEST_PARAM2;
	(void)HWTEST_PARAM3;

	if CMD_IS("power")
	{
		DI_MMC_PWR_E on = DI_MMC_PWR_OFF;

		if (!HWTEST_PARAM || !HWTEST_PARAM1)
		{
			CMD_Printf("Usage:\n");
			CMD_Printf("\tmmc power [portnum] [on|off]\n");
			return iResult;
		}

		port = strtol(HWTEST_PARAM, NULL, 10);
		if (!VK_strcmp(HWTEST_PARAM1, "on"))
		{
			on = DI_MMC_PWR_ON;
		}

		iResult = (int)DI_MMC_PowerControl((DI_MMC_PORT_E)port, on);
	}
	if CMD_IS("format")
	{
		if (!HWTEST_PARAM)
		{
			CMD_Printf("Usage:\n");
			CMD_Printf("\tmmc format [portnum]\n");
			return iResult;
		}

		port = strtol(HWTEST_PARAM, NULL, 10);
		iResult = (int)DI_MMC_Format((DI_MMC_PORT_E)port);
	}
	if CMD_IS("mount")
	{
		if (!HWTEST_PARAM || !HWTEST_PARAM1)
		{
			CMD_Printf("Usage:\n");
			CMD_Printf("\tmmc mount [portnum] [mount dir]\n");
			return iResult;
		}

		port = strtol(HWTEST_PARAM, NULL, 10);
		iResult = (int)DI_MMC_Mount((DI_MMC_PORT_E)port, (HUINT8*)HWTEST_PARAM1);
	}
	if CMD_IS("umount")
	{
		if (!HWTEST_PARAM)
		{
			CMD_Printf("Usage:\n");
			CMD_Printf("\tmmc umount [portnum]\n");
			return iResult;
		}

		port = strtol(HWTEST_PARAM, NULL, 10);
		iResult = (int)DI_MMC_Umount((DI_MMC_PORT_E)port);
	}
	if CMD_IS("info")
	{
		DI_MMC_INFO_T tMmcInfo;
		char label[128] = {0,};

		port = strtol(HWTEST_PARAM, NULL, 10);
		iResult = (int)DI_MMC_GetMMCInfo(port, &tMmcInfo);

		VK_printf("port: %d\n", tMmcInfo.nPort);
		VK_printf("disk size: %d KB\n", tMmcInfo.uiDiskSize);
		VK_printf("used size: %d KB\n", tMmcInfo.uiUsedSize);
		VK_printf("available size: %d KB\n", tMmcInfo.uiAvailableSize);
		VK_printf("mounted: %d\n", (tMmcInfo.nMounted)?"yes":"no");
		VK_printf("card lock: %s\n", (tMmcInfo.bCardLock)?"lock":"unlock");
		VK_printf("bus width: %d\n", tMmcInfo.ucBusWidths);
		VK_printf("uuid: %s\n", tMmcInfo.szUuid);
		VK_printf("serial number: 0x%02x%02x%02x%02x\n", tMmcInfo.ucPSN[0],tMmcInfo.ucPSN[1],tMmcInfo.ucPSN[2],tMmcInfo.ucPSN[3]);

		DI_MMC_GetVolumeName(port, label, sizeof(label));
		VK_printf("label: %s\n", label);
	}

	return iResult;
}

