/********************************************************************
 * $Workfile:   drv_smart.c  $
 * $Modtime:   June 30 2008  $
 *
 * Author : Hyun Seok Choi
 * Description : Read temperature from HDD as used by S.M.A.R.T
 *
 *
 *                                 Copyright (c) 2008 HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

/*******************************************************************/
/* Copyright (c) 2008 HUMAX Co., Ltd.				   */
/* All rights reserved.						   */
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* include common utils first! */
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#endif
#include "vkernel.h"
#include "taskdef.h"

#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <linux/fs.h>

#include "di_hdd.h"
#include "drv_smart.h"

#if defined(CONFIG_DI_SHARED_LIB) || defined(CONFIG_SHARED_COMPILE)
//NULL
#else
#include "linden_trace.h"
#endif

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#if defined(CONFIG_DI_SHARED_LIB) || defined(CONFIG_SHARED_COMPILE)
//#define DI_SMART_DEBUG
#ifdef FAN_DEBUG
#define PrintInfo(...)				VK_printf("(+)%s\n", __FUNCTION__)
#define PrintDebug(fmt, ...) 		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)    	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...)				VK_printf("(+)%s\n", __FUNCTION__)
#define PrintExit(...)				VK_printf("(-)%s\n", __FUNCTION__)
#else
#define PrintInfo(...)
#define PrintDebug(...)
#define PrintError(...)				VK_DBG_Print (__VA_ARGS__)
#define PrintEnter(...)
#define PrintExit(...)
#endif
#else
#define DEBUG_MODULE	TRACE_MODULE_DI_FAN

#ifndef DEBUG_MODULE
#ifdef DI_SMART_DEBUG
#define PrintDebug(fmt, ...)	VK_printf (fmt , ## __VA_ARGS__)
#define PrintError(fmt, ...)	VK_printf (fmt , ## __VA_ARGS__)
#else
#define PrintDebug(fmt, ...)
#define PrintError(fmt, ...)	VK_printf (fmt , ## __VA_ARGS__)
#endif

#ifdef DI_SMART_DEBUG
#define PrintEnter()			VK_printf("(+)%s\n", __FUNCTION__)
#define PrintExit()				VK_printf("(-)%s\n", __FUNCTION__)
#else
#define PrintEnter()			while (0) ((int (*)(char *, ...)) 0)
#define PrintExit()				while (0) ((int (*)(char *, ...)) 0)
#endif
#endif
#endif

#define ATA_OP_PIDENTIFY			0xa1
#define ATA_OP_FLUSHCACHE			0xe7
#define ATA_OP_FLUSHCACHE_EXT		0xea
#define ATA_OP_IDENTIFY				0xec
#define ATA_OP_STANDBYIMMEDIATE		0xe0

#define SMART_CMD					(0x031F)
#define SMART_RESET					(0x031c)

#define SMART_IDENTITY_CMD			(0x030D)

#define SMART_BUF_SIZE 				(516)		// 512+4

#define TEMP_INDEX 					(204)
#define MAX_TEMP_INDEX 				(206)

#define SERIAL_NUM_INDEX 			(10*2)
#define MODEL_NUM_INDEX 			(27*2)
#define FIRMWARE_NUM_INDEX 			(23*2)


#define BLKGETSIZE64		_IOR(0x12,114,size_t)

#define SMART_ATTR_ENTRY_NUM 30

#define SMART_SHORT_TEST	1
#define SMART_LONG_TEST 	2

#define SMART_DSTTEST_TIMEOUT		(15*60*2*500) // minute*second*2*500millisecond=15minutes
#define SMART_DSTTEST_PROGRESS_CHECK_PERIOD	(3000) /*3sec*/

#define SMART_MAX_ATA_DEVICE		3

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static int smartfd[SMART_MAX_ATA_DEVICE] = {-1,-1,-1};
static int dsttestmode = 1;			// short = 1, long = 2
static unsigned long sulDSTQueueID = 0;
static unsigned long sulDSTTaskID = 0;
static unsigned long ulDstTimerId;
static unsigned long sulDstSemId;
static pfnDI_HDD_DSTNotify pCallbackDstNotify = NULL;

DRV_Error DRV_SMART_DstInit(void);

DRV_Error DRV_SMART_ATA_OP_FlushCache(int portnum)
{
	int i;
	int nRet, nRetry;
	unsigned char buf[SMART_BUF_SIZE];
	unsigned short *pusbuff;
	unsigned char command = 0;

	if (portnum < ATA_FIRST_HDD_PORT_NUM || portnum >= ATA_HDD_PORT_NUM_MAX)
	{
		PrintError("%s: Invalid Port Num!\n", __FUNCTION__);
		return DRV_ERR;
	}

	if(smartfd[portnum - 1] < 0)
	{
		PrintError("[%s] smartfd(port %d) not opened \n", __FUNCTION__, portnum);
		return DRV_ERR;
	}

	VK_memset(buf, 0x00, SMART_BUF_SIZE);
	buf[0] = ATA_OP_IDENTIFY;
	buf[1] = 0;
	buf[2] = 0;
	buf[3] = 1;	/* sector count */

	PrintDebug("[ %s ] ATA_OP_IDENTIFY\n",__FUNCTION__);

	if (ioctl(smartfd[portnum - 1], SMART_CMD, &buf) != 0)
	{
		VK_memset(buf, 0x00, SMART_BUF_SIZE);
		buf[0] = ATA_OP_PIDENTIFY;
		buf[1] = 0;
		buf[2] = 0;
		buf[3] = 1; /* sector count */

		PrintDebug("[ %s ] ATA_OP_PIDENTIFY\n",__FUNCTION__);

		if (ioctl(smartfd[portnum - 1], SMART_CMD, &buf) != 0)
		{
			PrintError("[%d]%s ioctl error!!! \n",__LINE__,__func__);
			return DRV_ERR;
		}
	}

	/* swap data */
	pusbuff = (unsigned short *)buf;
	for(i=0; i<(SMART_BUF_SIZE-4)/2; i++)
	{
		pusbuff[i] = ntohs(pusbuff[i+2]);
	}

	if ((buf[83] & 0xe000) == 0x6000)
	{
		command = ATA_OP_FLUSHCACHE_EXT;
		PrintDebug("[ %s ] ATA_OP_FLUSHCACHE_EXT\n",__FUNCTION__);
	}
	else
	{
		command = ATA_OP_FLUSHCACHE;
		PrintDebug("[ %s ] ATA_OP_FLUSHCACHE\n",__FUNCTION__);
	}

	VK_memset(buf, 0x00, SMART_BUF_SIZE);
	buf[0] = command;
	buf[1] = 0;
	buf[2] = 0;
	buf[3] = 0;

	for (nRetry = 0; nRetry<5; nRetry++)	 /* Check Flush Cache command is completed */
	{
		nRet = ioctl(smartfd[portnum - 1], SMART_CMD, &buf);
		if(nRet == 0)					/* On success zero is returned  */
		{
			if(buf[0] == 0x50)			/* Status: 50h => Devices is ready to accept a command and a command is complete. */
			{
				break;
			}
		}
		PrintError("%s: Try to ioctl again (ret=%d), (status=0x%02x)\n",__FUNCTION__, nRet, buf[0]);
		VK_TASK_Sleep(100);
	}

	return DRV_OK;
}

DRV_Error DRV_SMART_ATA_OP_FlushBufferCache(int portnum)
{
	int rc = 0;

	if (portnum < ATA_FIRST_HDD_PORT_NUM || portnum >= ATA_HDD_PORT_NUM_MAX)
	{
		PrintError("%s: Invalid Port Num!\n", __FUNCTION__);
		return DRV_ERR;
	}

	if(smartfd[portnum - 1] < 0)
	{
		PrintError("[%s] smartfd(port %d) not opened \n", __FUNCTION__, portnum);
		return DRV_ERR;
	}

	sync();
	fsync(smartfd[portnum - 1]);				/* flush buffers */
	fdatasync(smartfd[portnum - 1]);			/* flush buffers */
	sync();

	PrintDebug("[ %s ] Block Device Flush Buffer\n", __FUNCTION__);

	rc = ioctl(smartfd[portnum - 1], BLKFLSBUF, NULL);

	/* Block Device Flush Buffer */
	if (rc < 0)		/* do it again, big time */
	{
//		perror("BLKFLSBUF failed");
		return DRV_ERR;
	}
	else
	{
		rc = ioctl(smartfd[portnum - 1], SMART_CMD, NULL);	/* IDE: await completion */
		if(rc < 0){
			return DRV_ERR;
		}
	}

	sync();

	return DRV_OK;
}

DRV_Error DRV_SMART_ATA_OP_StandbyImmediate(int portnum)
{
	int nRet, nRetry;
	unsigned char buf[SMART_BUF_SIZE];


	if (portnum < ATA_FIRST_HDD_PORT_NUM || portnum >= ATA_HDD_PORT_NUM_MAX)
	{
		PrintError("%s: Invalid Port Num!\n", __FUNCTION__);
		return DRV_ERR;
	}

	if(smartfd[portnum - 1] < 0)
	{
		PrintError("[%s] smartfd(port %d) not opened \n", __FUNCTION__, portnum);
		return DRV_ERR;
	}

	VK_MEM_Memset(buf, 0x00, SMART_BUF_SIZE);

	buf[0] = ATA_OP_STANDBYIMMEDIATE;	/* command - Standby Immediate ATA command */
	buf[1] = 0x00;						/*  Sector Count */
	buf[2] = 0x00;						/* Feaure set */
	buf[3] = 0x00;						/* LBA Low */
	buf[4] = 0x00;						/* LBA Mid */
	buf[5] = 0x00;						/* LBA High */

	for (nRetry = 0; nRetry<5; nRetry++)		/* Check Standby Immediate command is completed */
	{
		nRet = ioctl(smartfd[portnum - 1], SMART_CMD, &buf);
		if(nRet == 0)						/* On success zero is returned  */
		{
			if(buf[0] == 0x50)				/* Status: 50h => Devices is ready to accept a command and a command is complete. */
			{
				VK_TASK_Sleep(1500);	/* Ready 1500ms to complete 0RPM */
				break;
			}
		}
		PrintError("%s: Try to ioctl again (ret=%d), (status=0x%02x)\n",__FUNCTION__, nRet, buf[0]);
		VK_TASK_Sleep(100);
	}

	return DRV_OK;
}

#if 0	//err log
#include <errno.h>
#endif

DRV_Error DRV_SMART_ATA_OP_Reset(int portnum)
{
#if 0	//err log
	int err = 0;
#endif
	if (portnum < ATA_FIRST_HDD_PORT_NUM || portnum >= ATA_HDD_PORT_NUM_MAX)
	{
		PrintError("%s: Invalid Port Num!\n", __FUNCTION__);
		return DRV_ERR;
	}

	if(smartfd[portnum - 1] < 0)
	{
		PrintError("[%s] smartfd(port %d) not opened \n", __FUNCTION__, portnum);
		return DRV_ERR;
	}

	PrintDebug("[ %s ] Calling ioctl() to re-read partition table.\n",__FUNCTION__);

	if (ioctl(smartfd[portnum - 1], BLKRRPART, NULL))
	{
		PrintError("[%d]%s ioctl error!!! \n",__LINE__,__func__);
#if 0	//err log
		err = errno;
		perror(" BLKRRPART failed");
#endif
		return DRV_ERR;
	}
#if 0	//kernel bug?
	PrintDebug("[ %s ]  resetting drive.\n",__FUNCTION__);

	if (ioctl(smartfd[portnum - 1], SMART_RESET, NULL))
	{
		PrintError("[%d]%s ioctl error!!! \n",__LINE__,__func__);
#if 0	//err log
		err = errno;
		perror(" HDIO_DRIVE_RESET failed");
#endif
		return DRV_ERR;
	}
#endif
	return DRV_OK;
}

/********************************************************************
 Function   : DDI_SMART_Read_Attr
 Description : Read attribute values from HDD as used by S.M.A.R.T
 Input      :
 Output     :
 Return     : NONE
 ********************************************************************/
DRV_Error DRV_SMART_Read_Attr(int portnum, DRV_SMART_READ_ATTR_t *pstAttrInfo)
{
	unsigned char buf[SMART_BUF_SIZE];

	if (portnum < ATA_FIRST_HDD_PORT_NUM || portnum >= ATA_HDD_PORT_NUM_MAX)
	{
		PrintError("%s: Invalid Port Num!\n", __FUNCTION__);
		return DRV_ERR;
	}

	VK_memset(buf, 0x00, SMART_BUF_SIZE);
	buf[0] = 0xb0;
	buf[1] = 0xe0;
	buf[2] = 0xd5;
	buf[3] = 1;
	buf[4] = 0x4f;
	buf[5] = 0xc2;

	if(smartfd[portnum - 1] < 0)
	{
		PrintError("[%s] smartfd(port %d) not opened. return THRESHOLD_TEMP(51) for fan low voltage working.\n", __FUNCTION__, portnum);
		pstAttrInfo->ucCurrentTemp = 51;
		return DRV_ERR;
	}
	else
	{
		if(ioctl(smartfd[portnum - 1], SMART_CMD, &buf)!= 0)
		{
			PrintError("ioctl error!!!:: so return THRESHOLD_TEMP(51) for fan low voltage working. LINE : %d\n", __LINE__);
			pstAttrInfo->ucCurrentTemp = 51;
			return DRV_ERR;
		}

		pstAttrInfo->ucCurrentTemp = (char)buf[TEMP_INDEX];
		pstAttrInfo->ucMaxTemp = (char)buf[MAX_TEMP_INDEX];
	}

	return DRV_OK;
}

/********************************************************************
 Function   : DDI_SMART_Read_Attr
 Description : Read attribute values from HDD as used by S.M.A.R.T
 Input      :
 Output     :
 Return     : NONE
 ********************************************************************/
DRV_Error DRV_SMART_Read_Attr_Temp(int portnum, char *Temp)
{
	DRV_SMART_READ_ATTR_t	pstAttrInfo;

	VK_MEM_Memset(&pstAttrInfo, 0, sizeof(DRV_SMART_READ_ATTR_t));
	if (DRV_SMART_Read_Attr(portnum, &pstAttrInfo) != DRV_OK) {
		return DRV_ERR;
	}

	*Temp = pstAttrInfo.ucCurrentTemp;

	return DRV_OK;
}

DRV_Error DRV_SMART_IdentifyDevice(int portnum, DRV_SMART_SATA_INFO_t *pSTAInfo)
{
	int i;
	unsigned short *pusbuff;
	unsigned char buf[SMART_BUF_SIZE];
	unsigned long long disksize;

	if (portnum < ATA_FIRST_HDD_PORT_NUM || portnum >= ATA_HDD_PORT_NUM_MAX)
	{
		PrintError("%s: Invalid Port Num!\n", __FUNCTION__);
		return DRV_ERR;
	}

	if(smartfd[portnum - 1] < 0)
	{
		PrintError("[%s] smartfd(port %d) not opened \n", __FUNCTION__, portnum);
		return DRV_ERR;
	}

	if( pSTAInfo == NULL )
	{
		PrintError("Invalid Parm!! LINE : %d\n", __LINE__);
		return DRV_ERR;
	}

	VK_memset((void *)pSTAInfo, 0x00, sizeof(DRV_SMART_SATA_INFO_t));

	VK_memset(buf, 0x00, SMART_BUF_SIZE);
	buf[0] = 0xec;
	buf[3] = 1;

	if(ioctl(smartfd[portnum - 1], SMART_CMD, &buf)!= 0)
	{
		PrintError("[%d]%s ioctl error!!! \n",__LINE__,__func__);
		return DRV_ERR;
	}

	/* Get Block Size */
	disksize = 0;
	if (ioctl(smartfd[portnum - 1], BLKGETSIZE64, &disksize) != 0)
	{
		PrintError("[%d]%s ioctl error!!! \n",__LINE__,__func__);
		return DRV_ERR;
	}

	/* swap data */
	pusbuff = (unsigned short *)buf;
	for(i=0; i<(SMART_BUF_SIZE-4)/2; i++)
	{
		pusbuff[i] = ntohs(pusbuff[i+2]);
	}

	/* Save SATA Info */
	pSTAInfo->ulSizeMB = disksize/(1000*1000);
	VK_memcpy(pSTAInfo->ucModelName, &buf[MODEL_NUM_INDEX], ATA_MODEL_NUM_LEN);
	VK_memcpy(pSTAInfo->ucSerialNo, &buf[SERIAL_NUM_INDEX], ATA_SERIAL_NUM_LEN);
	VK_memcpy(pSTAInfo->ucFirmwareVer, &buf[FIRMWARE_NUM_INDEX], ATA_FIRMWARE_NUM_LEN);

	/* print Identify */
	PrintDebug("[SIZE] %ldMB\n", pSTAInfo->ulSizeMB);
	PrintDebug("[MODEL] %s\n", pSTAInfo->ucModelName);
	PrintDebug("[FIRMWARE] %s\n", pSTAInfo->ucFirmwareVer);
	PrintDebug("[SERIAL] %s\n", pSTAInfo->ucSerialNo);

	return DRV_OK;
}


/********************************************************************
 Function   : DRV_SMART_Init
 Description : Init specific HDD device
 Input      : void
 Output     : void
 Return     : DRV_OK
 ********************************************************************/
DRV_Error DRV_SMART_Init(int portnum, char *devname)
{
	unsigned char buf[SMART_BUF_SIZE];
	char devpath[16];

	if (portnum < ATA_FIRST_HDD_PORT_NUM || portnum >= ATA_HDD_PORT_NUM_MAX)
	{
		PrintError("%s: Invalid Port Num!\n", __FUNCTION__);
		return DRV_ERR;
	}

	/* Get dev name and make string like this */
	VK_snprintf(devpath, 15, "/dev/%s",devname);

	smartfd[portnum - 1] = open(devpath, O_RDONLY | O_NONBLOCK);
	if(smartfd[portnum - 1] < 0)
	{
		PrintError("open error!!! LINE : %d, portnum(%d)\n", __LINE__, portnum);

		return DRV_ERR;
	}

	/* Read Identify Device info from HDD as used by S.M.A.R.T
		Check 82th-byte whether S.M.A.R.T Set Support or not */
	VK_memset(buf, 0x00, SMART_BUF_SIZE);
	buf[0] = 0xec;
	buf[3] = 1;

	if (ioctl(smartfd[portnum - 1], SMART_CMD, &buf) != 0)
	{
		PrintError("[%d]%s ioctl error!!! portnum(%d)\n", __LINE__, __func__, portnum);
		return DRV_ERR;
	}

	/* SWAP BYTE */
	PrintDebug("Smart Log 82th-word 0x%x%x, portnum(%d)\n", buf[169], buf[168], portnum);

	//DRV_SMART_DstInit();

	if ((buf[168] & 0x01))
	{
		PrintDebug("S.M.A.R.T Support LINE : %d, portnum(%d)\n", __LINE__, portnum);
		return DRV_OK;
	}
	else
	{
		PrintError("HDD do not Support S.M.A.R.T LINE : %d, portnum(%d)\n", __LINE__, portnum);
		return DRV_ERR;
	}

	return DRV_OK;
}

/********************************************************************
 Function   : DRV_SMART_Uninit
 Description : Uninit specific HDD device
 Input      : void
 Output     : void
 Return     : DRV_OK
 ********************************************************************/
DRV_Error DRV_SMART_Uninit(int portnum)
{
	if (portnum < ATA_FIRST_HDD_PORT_NUM || portnum >= ATA_HDD_PORT_NUM_MAX)
	{
		PrintError("%s: Invalid Port Num!\n", __FUNCTION__);
		return DRV_ERR;
	}

	/* If hdd detech, close file descriptor */
	close(smartfd[portnum - 1]);
	smartfd[portnum - 1] = -1;
	return DRV_OK;
}

DRV_Error P_SMART_SendAtaCmd(int fd, unsigned char *buf, AtaCmd eCmd)
{
	unsigned char *pbuf = buf;
	/* Read Identify Device info from HDD as used by S.M.A.R.T
	   Check 82th-byte whether S.M.A.R.T Set Support or not */
	if(fd < 0)
	{
		PrintError("fd not opened (Maybe DRV_SMART_Init() Failed). LINE : %d\n", __LINE__);
		return DRV_ERR;
	}

	if(eCmd >= ATA_CMD_MAX)
	{
	    PrintError("CMD is out of range. LINE : %d\n", __LINE__);
	    return DRV_ERR_INVALID_PARAMETER;
	}

	switch (eCmd)
	{
		case ATA_IDENTIFY_DEVICE:
			pbuf[0] = 0xec;
			pbuf[1] = 0;
			pbuf[2] = 0;
			pbuf[3] = 1;
			pbuf[4] = 0;
			pbuf[5] = 0;
			break;
		case ATA_SMART_READ_ATTR_VALUE:
			pbuf[0] = 0xb0;
			pbuf[1] = pbuf[3] = 1;
			pbuf[2] = 0xd0;
			pbuf[4] = 0x4f;
			pbuf[5] = 0xc2;
			break;
		case ATA_SMART_READ_ATTR_THRE_VALUE:
			pbuf[0] = 0xb0;
			pbuf[1] = pbuf[3] = 1;
			pbuf[2] = 0xd1;
			pbuf[4] = 0x4f;
			pbuf[5] = 0xc2;
			break;
		case ATA_SMART_ENABLE:
			pbuf[0] = 0xb0;
			pbuf[1] = 0;
			pbuf[2] = 0xd8;
			pbuf[3] = 0;
			pbuf[4] = 0x4f;
			pbuf[5] = 0xc2;
			break;
		case ATA_SMART_EXECUTE_OFFLINE_IMMEDIATE:
			pbuf[0] = 0xb0;
			pbuf[2] = 0xd4;
			pbuf[3] = 0;
			pbuf[4] = 0x4f;
			pbuf[5] = 0xc2;
			break;
		case ATA_SMART_STANDBY_IMMEDIATE:
			pbuf[0] = 0xe0;	// command
			pbuf[1] = 0x00; // Sector Count
			pbuf[2] = 0x00; // Feaure set
			pbuf[3] = 0x00; // LBA Low
			pbuf[4] = 0x00; // LBA Mid
			pbuf[5] = 0x00; // LBA High

		default:
			break;
	}

	if(ioctl(fd,SMART_CMD,pbuf)!= 0)
	{
		PrintError("IOCTL error!!! LINE : %d \n", __LINE__);
		return -1;
	}

	if((pbuf[4] == 0xf4) && (pbuf[5] == 0x2c))
	{
		PrintError("Failed reading attribute data. LINE : %d\n", __LINE__);
		return -1;
	}

	return 0;
}

int P_SMART_GetDstStatus(int fd, int *progress)
{
	int ret=0;
	unsigned char buf[SMART_BUF_SIZE];
	unsigned char status=0;
	VK_memset(buf,0x00,SMART_BUF_SIZE);

	ret=P_SMART_SendAtaCmd(fd, buf, ATA_SMART_READ_ATTR_VALUE);
	if(ret < 0)
	{
	    PrintError("Error to read Attr values LINE : %d \n", __LINE__);
	    return -1;
	}

	status = ((buf[367] >> 4) & 0x0F);
	switch(status)
	{
		case 15:
			PrintDebug("Long  Test : ON (%d0%%)\n",(10-(buf[367] & 0x0F)));
			*progress= (10-(buf[367] & 0x0F))*10;
			ret = 10;
			break;
		case 0:
			PrintDebug("Long  Test : Complete No Error LINE : %d\n", __LINE__);
			*progress = DST_COMPLETED_OK;
			ret = 11;
			break;
		case 3:
			PrintDebug("Long  Test : Fatal error LINE : %d\n", __LINE__);
			*progress = DST_COMPLETED_FATAL_ERROR;
			ret = 12;
			break;
		case 4:
			PrintDebug("Long  Test : Test element failed LINE : %d\n", __LINE__);
			*progress = DST_COMPLETED_TEST_ELEMENT_FAILED;
			ret = 12;
			break;
		case 5:
			VK_printf("Long  Test : Electrical element failed LINE : %d\n", __LINE__);
			*progress = DST_COMPLETED_ELECTRICAL_ELEMENT_FAILED;
			ret = 12;
			break;
		case 6:
			PrintDebug("Long  Test : Servo(seek) element failed LINE : %d\n", __LINE__);
			*progress = DST_COMPLETED_SERVO_ELEMENT_FAILED;
			ret = 12;
			break;
		case 7:
			PrintDebug("Long  Test : Read element failed LINE : %d\n", __LINE__);
			*progress = DST_COMPLETED_READ_ELEMENT_FAILED;
			ret = 12;
			break;
		case 8:
			PrintDebug("Long  Test : Handling Damage LINE : %d\n", __LINE__);
			*progress = DST_COMPLETED_HANDLING_DAMAGE;
			ret = 12;
			break;
		default:
			ret = 0;
			break;
	}
	return ret;
}


static int P_SMART_DstStart(int fd)
{
    int ret=0;
    unsigned char buf[SMART_BUF_SIZE];

    VK_memset(buf,0x00,SMART_BUF_SIZE);

    ret=P_SMART_SendAtaCmd(fd, buf, ATA_SMART_ENABLE);
    if(ret < 0)
    {
        PrintError(" Error to read Attr values LINE : %d \n", __LINE__ );
        return -1;
    }

    VK_memset(buf,0x00,SMART_BUF_SIZE);
    buf[1] = SMART_SHORT_TEST;
    dsttestmode = SMART_SHORT_TEST;
    ret=P_SMART_SendAtaCmd(fd, buf, ATA_SMART_EXECUTE_OFFLINE_IMMEDIATE);
    if(ret < 0)
    {
        PrintError(" Error to do SMART EXECUTE OFFLINE IMMEDIATE LINE : %d\n", __LINE__ );
        return -1;
    }

    VK_memset(buf,0x00,SMART_BUF_SIZE);
    ret=P_SMART_SendAtaCmd(fd, buf, ATA_SMART_READ_ATTR_VALUE);
    if(ret < 0)
    {
        PrintError("Error to read Attr values LINE : %d\n", __LINE__ );
        return -1;
    }

    if(!(buf[367] & 0xF0))
    {
        PrintError("Error to do LongDST or not Start LINE : %d\n", __LINE__);
        return -1;
    }

    return 0;
}

#if 0
static int DRV_SMART_GetHddInfo(struct HddInfo *pHddInfo, int portnum)
{
    unsigned char buf[SMART_BUF_SIZE];
    unsigned short *pbuf;
    unsigned int size;
    int ret=0,i=0;
    ret=P_SMART_SendAtaCmd(smartfd[portnum - 1], buf, ATA_IDENTIFY_DEVICE);
    if(ret < 0)
    {
        PrintError("Error to get Idetify Device Info\n");
        goto done;
    }

    pbuf = (unsigned short *)buf;
    for(i=0;i < ((SMART_BUF_SIZE-4)/2);i++)
       pbuf[i] = swap16(pbuf[i+2]);

    pHddInfo->SerialNum[21] = 0;
    pHddInfo->FWVersion[9] = 0;
    pHddInfo->ModelName[41] = 0;
    VK_memcpy(pHddInfo->SerialNum,&buf[20],20);
    VK_memcpy(pHddInfo->FWVersion,&buf[46],8);
    VK_memcpy(pHddInfo->ModelName,&buf[54],40);
    pHddInfo->NumMaxLba = (swap16(pbuf[101]) << 16) | swap16(pbuf[100]);
    size = (pHddInfo->NumMaxLba/(1000*1000))*512;

done:
    return ret;
}
#endif

static int P_SMART_CheckAttributes(struct SmartAttrEntry *Entry, struct SmartAttrThreEntry *ThreEntry)
{
	struct SmartAttrEntry *pEntry;
	struct SmartAttrThreEntry *pThreEntry;
	int i;

	if(Entry == NULL || ThreEntry == NULL)
	{
		return -1;
	}

	for(i=0;i<SMART_ATTR_ENTRY_NUM;i++)
	{
		pEntry = Entry + i;
		pThreEntry = ThreEntry + i;

		if(pEntry == NULL || pThreEntry == NULL)
		{
		     break;
		}

		if((pEntry->id == pThreEntry->id) && (pThreEntry->thres != 0))
		{
			if(pEntry->value <= pThreEntry->thres)
			{
				PrintDebug("id = %d is over threshold! (value = %d threshold = %d)\n",pEntry->id,pEntry->value,pThreEntry->thres);
			}
			else
			{
				PrintDebug("id = %d value = %d threshold = %d\n",pEntry->id,pEntry->value,pThreEntry->thres);
			}
		}
		else
		{
			PrintDebug("id = %d value = %d no thres value\n",pEntry->id,pEntry->value);
		}

	}
	return 0;
}

static int P_SMART_DstStartCheck(int fd, int Start)
{
	unsigned char buf[SMART_BUF_SIZE];
	int ret=0;
	struct SmartAttrEntry sEntry[SMART_ATTR_ENTRY_NUM];
	struct SmartAttrThreEntry sThreEntry[SMART_ATTR_ENTRY_NUM];

	if(Start == 1)
	{
		ret=P_SMART_DstStart(fd);
		if(ret < 0)
		{
			PrintError("Error to do Long Dst LINE : %d\n", __LINE__);
			goto done;
		}
	}

	VK_memset(sEntry,0x00,SMART_ATTR_ENTRY_NUM*sizeof(struct SmartAttrEntry));
	VK_memset(sThreEntry,0x00,SMART_ATTR_ENTRY_NUM*sizeof(struct SmartAttrThreEntry));
	VK_memset(buf, 0x00, SMART_BUF_SIZE);

	ret=P_SMART_SendAtaCmd(fd, buf, ATA_SMART_READ_ATTR_VALUE);
	if(ret < 0)
	{
		PrintError("Error to read Attr values LINE : %d \n", __LINE__);
		goto done;
	}
	VK_memcpy(sEntry,&buf[6],SMART_ATTR_ENTRY_NUM*sizeof(struct SmartAttrEntry));
	VK_memset(buf, 0x00, SMART_BUF_SIZE);

	ret=P_SMART_SendAtaCmd(fd, buf, ATA_SMART_READ_ATTR_THRE_VALUE);
	if(ret < 0)
	{
		PrintError("Error to read Attr values LINE : %d\n", __LINE__);
		goto done;
	}

	VK_memcpy(sThreEntry,&buf[6],SMART_ATTR_ENTRY_NUM*sizeof(struct SmartAttrThreEntry));
	ret = P_SMART_CheckAttributes(sEntry, sThreEntry);
	if(ret < 0)
	{
		PrintError("Error to check Attr values LINE : %d\n", __LINE__);
		goto done;
	}
	return 0;

done:

	return ret;

}

static void P_SMART_DstCheckProgress(unsigned long size, void *p)
{
	SMART_DST_TEST_MSG_T Dstmsg;
	SMART_DST_TEST_MSG_T *param;
	int rc;

	HAPPY(size);

	if (!p) return;

	param = (SMART_DST_TEST_MSG_T*)p;

	Dstmsg.dstMsg = DI_SMART_MSG_CHECK_STATUS;
	Dstmsg.port_num = param->port_num;
	rc = VK_MSG_SendTimeout(sulDSTQueueID, &Dstmsg, sizeof(SMART_DST_TEST_MSG_T), 0);
	if (rc != VK_OK)
	{
		PrintError("[%s,%d] Error! timeout(rc=%d)\n", __FUNCTION__, __LINE__, rc);
	}
}

static void P_SMART_DstTestTask(void *pParam)
{
	static int portnum;
	SMART_DST_TEST_MSG_T dstTestMsg;
	unsigned long dstchecktime=0; // millisecond
	int ret=0;
	int rcvRet;
	int progress;
	HAPPY(pParam);

	dstTestMsg.port_num = 0;
	dstTestMsg.dstMsg = 0;

	while(1)
	{
		rcvRet = VK_MSG_Receive(sulDSTQueueID, &dstTestMsg, sizeof(SMART_DST_TEST_MSG_T));
		if(rcvRet != VK_OK)
		{
			PrintError("[%s] Error(0x%x) in VK_MSG_Receive()\n\r", __func__, rcvRet);
			continue;
		}

		if(pCallbackDstNotify == NULL)
		{
			continue;
		}

		if (dstTestMsg.port_num < ATA_FIRST_HDD_PORT_NUM || dstTestMsg.port_num >= ATA_HDD_PORT_NUM_MAX)
		{
			PrintError("%s: Invalid Port Num!\n", __FUNCTION__);
			continue;
		}

		switch(dstTestMsg.dstMsg)
		{
			case DI_SMART_MSG_DST_START:
				portnum = dstTestMsg.port_num;

				if(smartfd[portnum - 1] == -1)
				{
					PrintError("No HDD to test. LINE : %d\n", __LINE__);
					VK_SEM_Get(sulDstSemId);
					pCallbackDstNotify(DST_HDD_NOT_DETECTED, 0);
					VK_SEM_Release(sulDstSemId);
					continue;
				}

				ret=P_SMART_DstStartCheck(smartfd[portnum - 1], 1);
				if(ret < 0)
				{
					VK_SEM_Get(sulDstSemId);
					pCallbackDstNotify(DST_FAILED_TRY_AGAIN_LATER, 0);
					VK_SEM_Release(sulDstSemId);
					continue;
				}

				VK_TIMER_EventAfter(SMART_DSTTEST_PROGRESS_CHECK_PERIOD, P_SMART_DstCheckProgress, &dstTestMsg, sizeof(dstTestMsg), &ulDstTimerId);
				break;

			case DI_SMART_MSG_CHECK_STATUS:
				ret = P_SMART_GetDstStatus(smartfd[portnum - 1], &progress);
				if ( ret == 11 )
				{
					//P_SMART_DstStartCheck(smartfd[portnum - 1],0);
					VK_SEM_Get(sulDstSemId);
					pCallbackDstNotify(DST_COMPLETED_OK, 0);
					VK_SEM_Release(sulDstSemId);
				}
				else if ( ret == 12 )
				{
					//P_SMART_DstStartCheck(smartfd[portnum - 1],0);
					VK_SEM_Get(sulDstSemId);
					pCallbackDstNotify(progress, 0);
					VK_SEM_Release(sulDstSemId);
				}
				else if(ret == 10)
				{
					if (dsttestmode == SMART_SHORT_TEST) {
						if (SMART_DSTTEST_TIMEOUT > dstchecktime) {
							VK_SEM_Get(sulDstSemId);
							pCallbackDstNotify(DST_IS_ON_GOING, progress);
							VK_SEM_Release(sulDstSemId);
							VK_TIMER_EventAfter(SMART_DSTTEST_PROGRESS_CHECK_PERIOD, P_SMART_DstCheckProgress, &dstTestMsg, sizeof(dstTestMsg), &ulDstTimerId);
							dstchecktime = dstchecktime + SMART_DSTTEST_PROGRESS_CHECK_PERIOD;
						} else {
							VK_SEM_Get(sulDstSemId);
							pCallbackDstNotify(DST_COMPLETED_UNKNOWN, 0);
							VK_SEM_Release(sulDstSemId);
						}
					} else {
						VK_SEM_Get(sulDstSemId);
						pCallbackDstNotify(DST_IS_ON_GOING, progress);
						VK_SEM_Release(sulDstSemId);
						VK_TIMER_EventAfter(SMART_DSTTEST_PROGRESS_CHECK_PERIOD, P_SMART_DstCheckProgress, &dstTestMsg, sizeof(dstTestMsg), &ulDstTimerId);
				    }
				}
				else
				{
					VK_SEM_Get(sulDstSemId);
					pCallbackDstNotify(DST_COMPLETED_UNKNOWN, 0);
					VK_SEM_Release(sulDstSemId);
				}
				break;
			default:
				break;
		}

	}
}

static int P_SMART_SpinUp(int fd)
{
    unsigned char buf[SMART_BUF_SIZE];
	int ret=0;

	if(fd == -1) {
		PrintError("[%s] fd not opened.\n", __FUNCTION__);
		return DRV_ERR;
	}

    VK_memset(buf,0x00,SMART_BUF_SIZE);
    ret=P_SMART_SendAtaCmd(fd, buf, ATA_SMART_ENABLE);
    if(ret < 0)
    {
        PrintError(" Error to read Attr values LINE : %d\n", __LINE__ );
        return DRV_ERR;
    }
	return DRV_OK;
}

static int P_SMART_SpinDown(int fd)
{
	unsigned char buf[SMART_BUF_SIZE];
	int ret=0;

	if(fd == -1) {
		PrintError("[%s] fd not opened.\n", __FUNCTION__);
		return DRV_ERR;
	}

	VK_memset(buf,0x00,SMART_BUF_SIZE);
	ret=P_SMART_SendAtaCmd(fd, buf, ATA_SMART_STANDBY_IMMEDIATE);
	if(ret < 0)
	{
		PrintError("Error to read Attr values LINE : %d\n", __LINE__);
		return DRV_ERR;
	}
	return DRV_OK;
}

DRV_Error DRV_SMART_DstInit(void)
{
	int vkRet;

	vkRet = VK_SEM_Create(&sulDstSemId, "semDst", DEFAULT_SUSPEND_TYPE);
	if ( vkRet != VK_OK )
	{
		PrintError ("[%s] [%d] HDD_DST sema creation fail err = 0x%x\n", __func__,__LINE__, vkRet );
		return DRV_ERR;
	}

	vkRet = VK_MSG_Create(32*sizeof(unsigned long), sizeof(SMART_DST_TEST_MSG_T), "DSTHDD_Q", &sulDSTQueueID, VK_SUSPENDTYPE_FIFO);
	if ( vkRet != VK_OK )
	{
		PrintError ("[%s] [%d] HDD_DST msgQ creation fail err = 0x%x\n", __func__,__LINE__, vkRet );
		return DRV_ERR;
	}

	vkRet = VK_TASK_Create(P_SMART_DstTestTask, USER_PRIORITY7, 32*1024, "HDDDST_T", NULL, &sulDSTTaskID,FALSE);
	if ( vkRet != VK_OK )
	{
		PrintError ("[%s] [%d] HDD_DST TASK creation fail err = 0x%x\n", __func__,__LINE__, vkRet );
		return DRV_ERR;
	}

	VK_TASK_Start(sulDSTTaskID);

	return DRV_OK;
}

void DRV_SMART_DstStart(int portnum)
{
	int rc;
	SMART_DST_TEST_MSG_T Dstmsg;

	if (portnum < ATA_FIRST_HDD_PORT_NUM || portnum >= ATA_HDD_PORT_NUM_MAX)
	{
		PrintError("%s: Invalid Port Num!\n", __FUNCTION__);
		return;
	}

	Dstmsg.port_num = portnum;
	Dstmsg.dstMsg = DI_SMART_MSG_DST_START;

	rc = VK_MSG_SendTimeout(sulDSTQueueID, &Dstmsg, sizeof(SMART_DST_TEST_MSG_T), 0);
	if (rc != VK_OK)
	{
		PrintError("[%s,%d] Error! timeout(rc=%d)\n", __FUNCTION__, __LINE__, rc);
	}
}

DRV_Error DRV_SMART_ATA_SpinUp(int portnum)
{
	if (portnum < ATA_FIRST_HDD_PORT_NUM || portnum >= ATA_HDD_PORT_NUM_MAX)
	{
		PrintError("%s: Invalid Port Num!\n", __FUNCTION__);
		return DRV_ERR;
	}

	if(smartfd[portnum - 1] < 0)
	{
		PrintError("[%s] smartfd(port %d) not opened \n", __FUNCTION__, portnum);
		return DRV_ERR;
	}

	return P_SMART_SpinUp(smartfd[portnum - 1]);
}

DRV_Error DRV_SMART_ATA_SpinDown(int portnum)
{
	if (portnum < ATA_FIRST_HDD_PORT_NUM || portnum >= ATA_HDD_PORT_NUM_MAX)
	{
		PrintError("%s: Invalid Port Num!\n", __FUNCTION__);
		return DRV_ERR;
	}

	if(smartfd[portnum - 1] < 0)
	{
		PrintError("[%s] smartfd(port %d) not opened \n", __FUNCTION__, portnum);
		return DRV_ERR;
	}

	return P_SMART_SpinDown(smartfd[portnum - 1]);
}

void DRV_SMART_RegisterDstCallback(pfnDRV_HDD_DSTNotify pfnNotify)
{
	VK_SEM_Get(sulDstSemId);
	pCallbackDstNotify = pfnNotify;
	VK_SEM_Release(sulDstSemId);
}

