/**************************************************************
 *	@file		tr069.c
 *	TR-069 Application for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/07/20
 *	@author			humax
 *	@brief			TR-069 Application
 **************************************************************/
#include <fcntl.h>
#include <sys/ioctl.h>
#include <ondk.h>
#include <oapi.h>
#include <hapi.h>

#include "trport_common.h"

#define __________define__________

#define NOT_READY_TO_WRITE 	(-1)
#define RAWFW_OFFSET	(0x86f0000)
int g_fd = NOT_READY_TO_WRITE;


HERROR TRP_FLASH_Open(void)
{
	unsigned char *mntNode = "/dev/mtdblock6";
	if(g_fd != NOT_READY_TO_WRITE)
	{
		HxLOG_Error("already opened![%s:%d]\n", __FUNCTION__, __LINE__);
		TRLEAVE(ERR_FAIL);
	}

	/* Open Device Node */
	if((g_fd = open(mntNode, O_SYNC | O_RDWR)) < 0)
	{
		HxLOG_Error("open error (%s)[%s:%d]\n", mntNode, __FUNCTION__, __LINE__);
		g_fd = NOT_READY_TO_WRITE;
		TRLEAVE(ERR_FAIL);
	}

	/* set init position */
	if(RAWFW_OFFSET != lseek(g_fd, RAWFW_OFFSET, SEEK_SET))
	{
		HxLOG_Error("lseek error[%s:%d]\n", __FUNCTION__, __LINE__);
		TRP_FLASH_Close();
		TRLEAVE(ERR_FAIL);
	}

	TRLEAVE(ERR_OK);
}

HERROR TRP_FLASH_Close(void)
{
	if(g_fd == NOT_READY_TO_WRITE)
	{
		HxLOG_Error("there is nothing to close![%s:%d]\n", __FUNCTION__, __LINE__);
		TRLEAVE(ERR_FAIL);
	}

	/* flush buffer cache */
	//nRet = ioctl(fd, BLKFLSBUF, 0);
	if(ioctl(g_fd, 0x20001261, 0) < 0)
	{
		HxLOG_Error("Sync MTD block error[%s:%d]\n", __FUNCTION__, __LINE__);
		TRLEAVE(0);
	}

	close(g_fd);
	g_fd = NOT_READY_TO_WRITE;
	TRLEAVE(ERR_OK);
}


unsigned int TRP_FLASH_Write(const void *buf, size_t nbyte)
{
	int nRet;

	if(g_fd == NOT_READY_TO_WRITE)
	{
		HxLOG_Error("not ready to write! open first![%s:%d]\n", __FUNCTION__, __LINE__);
		TRLEAVE(0);
	}

	nRet = write(g_fd, buf, nbyte);
	if(nRet <= 0)
	{
		HxLOG_Error("write MTD block error[%s:%d]\n", __FUNCTION__, __LINE__);
		TRLEAVE(0);
	}
	HLIB_STD_TaskSleep(10);
	TRLEAVE(nRet);
}


