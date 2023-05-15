/**************************************************************************************************/
#define ________Sever_Sec_Handler_Private_Include___________________________________________
/**************************************************************************************************/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/mount.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if.h>
#include <linux/wireless.h>

#include <time.h> //stime
#include <sys/time.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/stat.h>

#include <linux/if_arp.h>
#include <linux/route.h>
#include <linux/fs.h>
#include <linux/hdreg.h>

#include "secma_main.h"
#include "secma_init.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif

#define MAX_MSGSIZE				(1024)
#define IPC_IOCTL_MAX_DATA_SIZE	(512)

#define IPC_IOCTL_DEBUG 			(0)

#ifdef CONFIG_DEBUG
#if !defined(VK_Print)
#define VK_Print(_fmt, ...)				printf(_fmt, ## __VA_ARGS__)
#endif
#else
#if !defined(VK_Print)
#define VK_Print(_fmt, ...)				((void)0)
#endif
#endif


#if defined(VK_Print)
#undef VK_Print
#define VK_Print(_fmt, ...)				((void)0) //printf(_fmt, ## __VA_ARGS__)
#endif


/**************************************************************************************************/
#define ________Sever_Sec_Handler_Golbal_Value_____________________________________________
/**************************************************************************************************/
extern char **environ;


/**************************************************************************************************/
#define ________Sever_Sec_Handler_Private_Define____________________________________________
/**************************************************************************************************/
static HINT32 s_ulMsgQId[2] = {0,};
static DI_FS_FormatList_t s_FsFormatList_;


/**************************************************************************************************/
#define ________Sever_Sec_Handler_Private_Type_Define_______________________________________
/**************************************************************************************************/
#define SMART_BUF_SIZE 		(516) // 512+4


/**************************************************************************************************/
#define ________Sever_Sec_Handler_Private_Static_Value_______________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________Sever_Sec_Handler_Private_Static_Prototype___________________________________
/**************************************************************************************************/
static HINT32 	local_secma_handle_SysCommand(const HINT8 *command);
static HINT32 	local_secma_handle_FileWrite(HUINT8 *pucFile, HINT32 nOffset, HINT32 nSize, HUINT8 *pucBuff);
static HINT32 	local_secma_handle_Mount(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_Umount(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_Stime(void *pvMsg, HUINT32 ulSize);
#if (0)
static HINT32 	local_secma_handle_StimeSetTime(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_StimeSetMode(void *pvMsg, HUINT32 ulSize);
#endif
static HINT32 	local_secma_handle_StimeGetMode(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_Settimeofday(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_Chmod(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_Chown(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_Remove(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_SysCmd(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_Ioctl(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_Stat(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_Lstat(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_Socket(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_Select(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_Send(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_Recv(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_Open(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_Close(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_Lseek(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_Read(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_Write(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_Rename(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_Vfork(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_HddPwrCtrl(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_UsbPwrCtrl(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_UsbHostConPwrCtrl(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_UsbStop(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_HidCbNoti(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_HddCbNoti(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_UsbCbNoti(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_WlanCbNoti(void *pvMsg, HUINT32 ulSize);
#if (1)
static HINT32 	local_secma_handle_HddDstCbNoti(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_HddFormatCbNoti(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_FsGetDeviceFileName(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_FsGetDeviceFileFullName(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_FsGetPartitionFileName(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_FsGetPartitionFullFileName(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_FsIsFileExist(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_FsIsHDDExist(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_FsIsDirExist(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_FsMkdir(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_FsGetVolumeUuid(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_FsGetVolumeNtfsUuid(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_FsMountPartition(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_FsUmountPartition(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_FsHddDstStart(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_FsOpenDevMbrBuf(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_FsDeletePartitionInDevMbrBuf(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_FsDeleteAllPartitionInDevMbrBuf(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_FsUpdatePartitionInDevMbrBuf(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_FsSyncDevMbrBuf(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_FsCloseDevMbrBuf(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_FsGetPartitionFromDeviceMbr(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_FsEraseAllPartitionInDeviceMbr(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_FsInitFormatList(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_FsAddFormatItemToList(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_FsClearFormatList(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_FsFormatLists(void *pvMsg, HUINT32 ulSize);
static HINT32 	local_secma_handle_FsGetMountedPartitionStatus(void *pvMsg, HUINT32 ulSize);
#endif

#if (1)
static HINT32 local_secma_handle_EncrFormat(void *pvMsg, HUINT32 ulSize);
static HINT32 local_secma_handle_EncrOpen(void *pvMsg, HUINT32 ulSize);
static HINT32 local_secma_handle_EncrClose(void *pvMsg, HUINT32 ulSize);
static HINT32 local_secma_handle_EncrMakeFs(void *pvMsg, HUINT32 ulSize);
static HINT32 local_secma_handle_EncrMount(void *pvMsg, HUINT32 ulSize);
static HINT32 local_secma_handle_EncrUmount(void *pvMsg, HUINT32 ulSize);
#endif

static HINT32 	local_secma_handle_HidCallback(DI_USB_HID_DEVICE eDevice, DI_USB_HID_DATA tData);
static HINT32 	local_secma_handle_HddCallback(DI_HDD_Event_t etEventType, void *pucHddInfo);
static HINT32 	local_secma_handle_UsbCallback(DI_USB_Event_t etEventType, void *pucUsbInfo);
static HINT32 	local_secma_handle_WlanCallback(DI_USB_Wireless_Event_t etEventType, void *pucWlanInfo);
#if (1)
static void 	local_secma_handle_DstCallback(HDD_DST_RESULT dstResult, HINT32 nProgress);
static void 	local_secma_handle_FormatCallback(HUINT32 ulEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
#endif

static HINT32 local_secma_handle_findMsgByType(IpcSecMsgType eMsgType, IpcSecManager_t *pstSecMgr);
static HINT32	local_secma_handle_findMsgByTarget(HUINT8 *pucTarget, IpcSecManager_t *pstSecMgr);



/**************************************************************************************************/
#define ________Sever_Sec_Handler_Private_Static_Prototype_Body______________________________
/**************************************************************************************************/
static HINT32 local_secma_handle_findMsgByType(IpcSecMsgType eMsgType, IpcSecManager_t *pstSecMgr)
{
	int i;

	if (pstSecMgr == NULL)
		return ERR_FAIL;

	for (i=0; i< IPC_SYS_MSGQ_MAX_MSG; i++)
	{
		if (eMsgType == s_stMsgMgr[i].eMsgType)
		{
			VK_memcpy(pstSecMgr, &s_stMsgMgr[i], sizeof(IpcSecManager_t));
			break;
		}
	}

	return ((i < IPC_SYS_MSGQ_MAX_MSG) ? ERR_OK : ERR_FAIL);
}

static HINT32 local_secma_handle_findMsgByTarget(HUINT8 *pucTarget, IpcSecManager_t *pstSecMgr)
{
	HINT32 i;

	if (pucTarget == NULL)
		return ERR_FAIL;

	for (i=0; i< IPC_SYS_MSGQ_MAX_MSG; i++)
	{
		if (strncmp(pucTarget, s_stMsgMgr[i].ucTarget, IPC_MOUNT_MAX_SIZE) == 0)
 		{
			VK_memcpy(pstSecMgr, &s_stMsgMgr[i], sizeof(IpcSecManager_t));
			break;
		}
	}

	return ((i < IPC_SYS_MSGQ_MAX_MSG) ? ERR_OK : ERR_FAIL);
}


static HINT32 local_secma_handle_SysCommand(const HINT8 *command)
{
	HINT32 nPid, nStatus;

	if (command == NULL)
	{
		VK_Print("[%s][%d] command == NULL\n",__func__, __LINE__);
		return -1;
	}

	nPid = vfork();
	if (nPid == -1)
	{
		if (errno == EAGAIN)
		{
			VK_Print("[%s][%d] errno = EAGAIN\n",__func__, __LINE__);
		}
		if (errno == ENOMEM)
		{
			VK_Print("[%s][%d] errno = ENOMEM\n",__func__, __LINE__);
		}
		return -1;
	}

	if (nPid == 0)
	{
		char *argv[4];
		char *temp_command;
		unsigned long len = 0;

		len = strlen(command);
		temp_command = HLIB_STD_MemAlloc(len + 1);
		strncpy(temp_command, command, len);
		temp_command[len] = '\0';

		argv[0] = "sh";
		argv[1] = "-c";
		argv[2] = temp_command;
		argv[3] = 0;
		execve("/bin/sh", argv, environ);
		HLIB_STD_MemFree(temp_command);
		exit(127);
	}

	do
	{
		if (waitpid(nPid, &nStatus, 0) == -1)
		{
			VK_Print("[%s][%d] nPid == -1, errno = %d\n",__func__, __LINE__, errno);
			if (errno != EINTR)
				return -1;
		}
		else
		{
			return (nStatus & 0xff);
		}
	} while(1);

	return -1;
}


static HINT32 local_secma_handle_FileWrite(HUINT8 *pucFile, HINT32 nOffset, HINT32 nSize, HUINT8 *pucBuff)
{
	HINT32 	nFd, nErr;
	HUINT8 	ucPathName[50] = "/tmp/";
	struct 	stat st;
	struct 	passwd *u_info;
	struct 	group  *g_info;

	if ( (pucFile == NULL) || (pucBuff == NULL) )
	{
		VK_Print("target are null...\n");
		return ERR_FAIL;
	}

	strncpy(ucPathName+5, pucFile, 45);

	if ( (stat(ucPathName, &st) == 0) && (S_ISREG(st.st_mode)) )
	{
		VK_Print("%s is existed, so remove it before writing...\n", ucPathName);
		nErr = remove(ucPathName);
		if (nErr < 0)
		{
			VK_Print("error in remove (0x%x)\n", nErr);
			return ERR_FAIL;
		}
	}

	nFd = open(ucPathName, O_CREAT | O_WRONLY | O_SYNC);
	if (nFd < 0)
	{
		VK_Print("error in open (%d)\n", nFd);
		return ERR_FAIL;
	}

	nErr = lseek(nFd, nOffset, SEEK_SET);
	if (nErr < 0)
	{
		VK_Print("error in lseek (0x%x)\n", nErr);
		return ERR_FAIL;
	}

	nErr = write(nFd, pucBuff, nSize);
	if (nErr < 0)
	{
		VK_Print("error in write (0x%x)\n", nErr);
		return ERR_FAIL;
	}

	fsync(nFd);
	close(nFd);

	u_info = getpwnam("settop");
    	g_info = getgrnam("settop");

	if (u_info != NULL && g_info != NULL)
	{
		nErr = chown(ucPathName, u_info->pw_uid, g_info->gr_gid);
		if (nErr < 0)
		{
			VK_Print("error in chown (0x%x)\n", nErr);
		}
	}

	nErr = chmod(ucPathName, 0664);
	if (nErr < 0)
	{
		VK_Print("error in chown (0x%x)\n", nErr);
	}

	return nErr;
}


static HINT32 local_secma_handle_Mount(void *pvMsg, HUINT32 ulSize)
{
	HINT32 nErr;
	IpcSecMount_t stMountInfo;

	VK_memset(&stMountInfo, 0, sizeof(IpcSecMount_t));
	VK_memcpy(&stMountInfo, pvMsg, sizeof(IpcSecMount_t));

	nErr = DI_FS_MountPartition(stMountInfo.eDevIdx, stMountInfo.ulPartIdx, stMountInfo.szDstMountDir, stMountInfo.eFsType);
	if (nErr == DI_ERR_OK)
	{
		nErr = IPC_SEC_MOUNT_NO_ERR;
	}
	else
	{
		if (errno == EROFS)
		{
			nErr = IPC_SEC_MOUNT_EROFS;
		}
		else if (errno == EINVAL)
		{
			nErr = IPC_SEC_MOUNT_EINVAL;
		}
		else
		{
			nErr = IPC_SEC_MOUNT_UNKONWN;
		}
	}

	/* HDD는 mount할때 해당 마운트 위치를 settop 으로 변경한다. */
	{
		if (strstr(stMountInfo.szDstMountDir, "/mnt/hd") != NULL)
		{
			struct passwd *u_info;
			struct group  *g_info;

			u_info = getpwnam("settop");
			g_info = getgrnam("settop");

			if (u_info != NULL && g_info != NULL)
			{
				nErr = chown(stMountInfo.szDstMountDir, u_info->pw_uid, g_info->gr_gid);
			}
			else
			{
				nErr = -1;
			}
		}
	}


	return nErr;
}


static HINT32 local_secma_handle_Umount(void *pvMsg, HUINT32 ulSize)
{
	HINT32 nErr;
	IpcSecMount_t stMountInfo;

	VK_Print("#### [%s][%d] %s\n", __FUNCTION__, __LINE__, stMountInfo.eDevIdx);

	VK_memset(&stMountInfo, 0, sizeof(IpcSecMount_t));
	VK_memcpy(&stMountInfo, pvMsg, sizeof(IpcSecMount_t));

	nErr = DI_FS_UnmountPartition(stMountInfo.eDevIdx, stMountInfo.ulPartIdx, stMountInfo.eFsType);

	return nErr;
}


static HINT32 local_secma_handle_Stime(void *pvMsg, HUINT32 ulSize)
{
	HINT32 nErr;
	IpcSecSetTime_t stSetTime;
	(void)(ulSize);

	VK_memset(&stSetTime, 0, sizeof(IpcSecSetTime_t));
	VK_memcpy(&stSetTime, pvMsg, sizeof(IpcSecSetTime_t));

	VK_Print("[local_secma_handle_Stime] eSetMode(%d), ulUnixTime=%ld\n", stSetTime.eSetMode, stSetTime.ulUnixTime);

	nErr = VK_CLOCK_SetTime(stSetTime.eSetMode, stSetTime.ulUnixTime);

	VK_Print("[local_secma_handle_Stime] nErr = [%d]\n", nErr);

	return nErr;
}


#if (0)
// 사용안함. 정리되면 지우자.
static HINT32 local_secma_handle_StimeSetTime(void *pvMsg, HUINT32 ulSize)
{
	HINT32 nErr;
	unsigned long *pulUnixTime = NULL;
	(void)(ulSize);

	pulUnixTime = (unsigned long *)pvMsg;

	//nErr = stime((time_t *)pulUnixTime);

	return nErr;
}


// 사용안함. 정리되면 지우자.
static HINT32 local_secma_handle_StimeSetMode(void *pvMsg, HUINT32 ulSize)
{
	HINT32 nErr;
	VK_CLOCK_SET_MODE *eSetMode = NULL;
	(void)(ulSize);

	eSetMode = (VK_CLOCK_SET_MODE *)pvMsg;

	VK_Print("[local_secma_handle_StimeSetMode] eSetMode(%d)\n", eSetMode);

	//nErr = VK_CLOCK_SetTimeSource(eSetMode);

	return nErr;
}
#endif


static HINT32 local_secma_handle_StimeGetMode(void *pvMsg, HUINT32 ulSize)
{
	HINT32 nErr;
	VK_CLOCK_SET_MODE eGetMode;
	(void)(ulSize);

	nErr = VK_CLOCK_GetTimeSource(&eGetMode);
	VK_Print("[local_secma_handle_StimeGetMode] eSetMode(%d)\n", eGetMode);
	VK_memcpy(pvMsg, &eGetMode, sizeof(VK_CLOCK_SET_MODE));

	return nErr;
}


static HINT32 local_secma_handle_Settimeofday(void *pvMsg, HUINT32 ulSize)
{
	HINT32 nErr;
	struct timeval stTv;
	struct timezone  stTimeZone;
	IpcSecSettimeofday_t stSettimeofDayInfo;

	VK_memset(&stSettimeofDayInfo, 0, sizeof(IpcSecSettimeofday_t));
	VK_memcpy(&stSettimeofDayInfo, pvMsg, sizeof(IpcSecSettimeofday_t));

	nErr = gettimeofday(&stTv, &stTimeZone);
	// Base unit : 15 min.
	stTimeZone.tz_minuteswest = ((stSettimeofDayInfo.nHour * 60 + stSettimeofDayInfo.nMin) / 15) * 15;
	stTimeZone.tz_dsttime	  = 0;
	nErr = settimeofday(&stTv, &stTimeZone);

	return nErr;
}


static HINT32 local_secma_handle_Chmod(void *pvMsg, HUINT32 ulSize)
{
	HINT32 nErr;
	IpcSecFileCtl_t stFileCtrl;

	VK_memset(&stFileCtrl, 0, sizeof(IpcSecFileCtl_t));
	VK_memcpy(&stFileCtrl, pvMsg, sizeof(IpcSecFileCtl_t));

	nErr = chmod(stFileCtrl.ucPath, stFileCtrl.ulMode);

	VK_Print("##### [%s][%d] File Name:%s, mode:%d: %s\n", __FUNCTION__, __LINE__, stFileCtrl.ucPath, stFileCtrl.ulMode, strerror(errno));

	return nErr;
}


static HINT32 local_secma_handle_Chown(void *pvMsg, HUINT32 ulSize)
{
	HINT32 nErr;
	IpcSecFileCtl_t stFileCtrl;

	VK_memset(&stFileCtrl, 0, sizeof(IpcSecFileCtl_t));
	VK_memcpy(&stFileCtrl, pvMsg, sizeof(IpcSecFileCtl_t));

#if (1)
{
	struct passwd *u_info;
	struct group  *g_info;

	u_info = getpwnam("settop");
    	g_info = getgrnam("settop");

	if (u_info != NULL && g_info != NULL)
	{
		nErr = chown(stFileCtrl.ucPath, u_info->pw_uid, g_info->gr_gid);
	}
	else
	{
		nErr = -1;
	}
}
#else
{
	HINT8 ucCmd[IPC_PATH_MAX_SIZE];

	snprintf(ucCmd, IPC_PATH_MAX_SIZE, "chown -R settop:settop %s", stFileCtrl.ucPath);
	nErr = local_secma_handle_SysCommand(ucCmd);
}
#endif
	VK_Print("##### [%s][%d] File Name:%s : %s\n", __FUNCTION__, __LINE__, stFileCtrl.ucPath, strerror(errno));

	return nErr;
}


static HINT32 local_secma_handle_Remove(void *pvMsg, HUINT32 ulSize)
{
	HINT32 nErr;
	IpcSecFileCtl_t stFileCtrl;

	VK_memset(&stFileCtrl, 0, sizeof(IpcSecFileCtl_t));
	VK_memcpy(&stFileCtrl, pvMsg, sizeof(IpcSecFileCtl_t));

	VK_Print("##### [%s][%d] File Name:%s : \n", __FUNCTION__, __LINE__, stFileCtrl.ucPath);

	nErr = remove(stFileCtrl.ucPath);

	return nErr;
}


static HINT32 local_secma_handle_Mkdir(void *pvMsg, HUINT32 ulSize)
{
	HINT32 nErr;
	IpcSecFileCtl_t stFileCtrl;

	VK_memset(&stFileCtrl, 0, sizeof(IpcSecFileCtl_t));
	VK_memcpy(&stFileCtrl, pvMsg, sizeof(IpcSecFileCtl_t));

	VK_Print("##### [%s][%d] File Name:%s : ##### \n", __FUNCTION__, __LINE__, stFileCtrl.ucPath);

	nErr = mkdir(stFileCtrl.ucPath, stFileCtrl.ulMode);
	if (nErr < 0)
		VK_Print("##### error = %s \n ##### ", strerror(errno));

	if (nErr != -1)
	{
		if (strstr(stFileCtrl.ucPath, "/mnt/hd") != NULL)
		{
			struct passwd *u_info;
			struct group  *g_info;

			u_info = getpwnam("settop");
			g_info = getgrnam("settop");

			if (u_info != NULL && g_info != NULL)
			{
				nErr = chown(stFileCtrl.ucPath, u_info->pw_uid, g_info->gr_gid);
			}
			else
			{
				nErr = -1;
			}
		}
	}

	return nErr;
}


static HINT32 local_secma_handle_Mknod(void *pvMsg, HUINT32 ulSize)
{
	HINT32 nErr;
	IpcSecFileCtl_t stFileCtrl;

	VK_memset(&stFileCtrl, 0, sizeof(IpcSecFileCtl_t));
	VK_memcpy(&stFileCtrl, pvMsg, sizeof(IpcSecFileCtl_t));

	VK_Print("##### [%s][%d] File Name:%s : ##### \n", __FUNCTION__, __LINE__, stFileCtrl.ucPath);

	nErr = mknod(stFileCtrl.ucPath, stFileCtrl.ulMode, stFileCtrl.ulDev);
	if (nErr < 0)
		VK_Print("##### error = %s \n ##### ", strerror(errno));

	return nErr;
}


static HINT32 local_secma_handle_SysCmd(void *pvMsg, HUINT32 ulSize)
{
	HINT32 nPid, nStatus;
	IpcSecSysCmd_t stSysCmd;
	IpcSecSysCmdErr_e eErr = IPC_SEC_SYS_CMD_NO_ERR;

	VK_memset(&stSysCmd, 0, sizeof(IpcSecSysCmd_t));
	VK_memcpy(&stSysCmd, pvMsg, sizeof(IpcSecSysCmd_t));

	VK_Print("local_secma_handle_SysCmd Start : NUm = %d \n", stSysCmd.ucArgc);

#if (0)
	VK_Print("local_secma_handle_SysCmd call : Argv[0] = %s\n", stSysCmd.ucArgv[0]);
	VK_Print("local_secma_handle_SysCmd call : Argv[1] = %s\n", stSysCmd.ucArgv[1]);
	VK_Print("local_secma_handle_SysCmd call : Argv[2] = %s\n", stSysCmd.ucArgv[2]);
	VK_Print("local_secma_handle_SysCmd call : Argv[3] = %s\n", stSysCmd.ucArgv[3]);
	VK_Print("local_secma_handle_SysCmd call : Argv[4] = %s\n", stSysCmd.ucArgv[4]);
	VK_Print("local_secma_handle_SysCmd call : Argv[5] = %s\n", stSysCmd.ucArgv[5]);
	VK_Print("local_secma_handle_SysCmd call : Argv[6] = %s\n", stSysCmd.ucArgv[6]);
	VK_Print("local_secma_handle_SysCmd call : Argv[7] = %s\n", stSysCmd.ucArgv[7]);
	VK_Print("local_secma_handle_SysCmd call : Argv[8] = %s\n", stSysCmd.ucArgv[8]);
	VK_Print("local_secma_handle_SysCmd call : Argv[9] = %s\n", stSysCmd.ucArgv[9]);
#endif

	nPid = vfork();
	if (nPid == -1)
	{
		if (errno == EAGAIN)
		{
			VK_Print("errno = EAGAIN\n");
		}
		if (errno == ENOMEM)
		{
			VK_Print("errno = ENOMEM\n");
		}

		return IPC_SEC_SYS_CMD_VFORK_ERR;
	}

	if (nPid == 0)
	{
		HINT32 nErr;

		if (stSysCmd.ucArgc == 1)
		{
			nErr = execlp(stSysCmd.ucArgv[0], (char *)0);
		}
		else if (stSysCmd.ucArgc == 2)
		{
			nErr = execlp(stSysCmd.ucArgv[0],
						stSysCmd.ucArgv[1], (char *)0);
		}
		else if (stSysCmd.ucArgc == 3)
		{
			nErr = execlp(stSysCmd.ucArgv[0],
						stSysCmd.ucArgv[1],
						stSysCmd.ucArgv[2], (char *)0);
		}
		else if (stSysCmd.ucArgc == 4)
		{
			nErr = execlp(stSysCmd.ucArgv[0],
						stSysCmd.ucArgv[1],
						stSysCmd.ucArgv[2],
						stSysCmd.ucArgv[3], (char *)0);
		}
		else if (stSysCmd.ucArgc == 5)
		{
			nErr = execlp(stSysCmd.ucArgv[0],
						stSysCmd.ucArgv[1],
						stSysCmd.ucArgv[2],
						stSysCmd.ucArgv[3],
						stSysCmd.ucArgv[4], (char *)0);
		}
		else if (stSysCmd.ucArgc == 6)
		{
			nErr = execlp(stSysCmd.ucArgv[0],
						stSysCmd.ucArgv[1],
						stSysCmd.ucArgv[2],
						stSysCmd.ucArgv[3],
						stSysCmd.ucArgv[4],
						stSysCmd.ucArgv[5], (char *)0);
		}
		else if (stSysCmd.ucArgc == 7)
		{
			nErr = execlp(stSysCmd.ucArgv[0],
						stSysCmd.ucArgv[1],
						stSysCmd.ucArgv[2],
						stSysCmd.ucArgv[3],
						stSysCmd.ucArgv[4],
						stSysCmd.ucArgv[5],
						stSysCmd.ucArgv[6], (char *)0);
		}
		else if (stSysCmd.ucArgc == 8)
		{
			nErr = execlp(stSysCmd.ucArgv[0],
						stSysCmd.ucArgv[1],
						stSysCmd.ucArgv[2],
						stSysCmd.ucArgv[3],
						stSysCmd.ucArgv[4],
						stSysCmd.ucArgv[5],
						stSysCmd.ucArgv[6],
						stSysCmd.ucArgv[7], (char *)0);
		}
		else if (stSysCmd.ucArgc == 9)
		{
			nErr = execlp(stSysCmd.ucArgv[0],
						stSysCmd.ucArgv[1],
						stSysCmd.ucArgv[2],
						stSysCmd.ucArgv[3],
						stSysCmd.ucArgv[4],
						stSysCmd.ucArgv[5],
						stSysCmd.ucArgv[6],
						stSysCmd.ucArgv[7],
						stSysCmd.ucArgv[8], (char *)0);
		}
		else if (stSysCmd.ucArgc == 10)
		{
			nErr = execlp(stSysCmd.ucArgv[0],
						stSysCmd.ucArgv[1],
						stSysCmd.ucArgv[2],
						stSysCmd.ucArgv[3],
						stSysCmd.ucArgv[4],
						stSysCmd.ucArgv[5],
						stSysCmd.ucArgv[6],
						stSysCmd.ucArgv[7],
						stSysCmd.ucArgv[8],
						stSysCmd.ucArgv[9], (char *)0);
		}
/*
		else if (stSysCmd.ucArgc == 11)
		{
			nErr = execlp(stSysCmd.ucArgv[0],
						stSysCmd.ucArgv[1],
						stSysCmd.ucArgv[2],
						stSysCmd.ucArgv[3],
						stSysCmd.ucArgv[4],
						stSysCmd.ucArgv[5],
						stSysCmd.ucArgv[6],
						stSysCmd.ucArgv[7],
						stSysCmd.ucArgv[8],
						stSysCmd.ucArgv[9],
						stSysCmd.ucArgv[10], (char *)0);
		}
		else if (stSysCmd.ucArgc == 12)
		{
			nErr = execlp(stSysCmd.ucArgv[0],
						stSysCmd.ucArgv[1],
						stSysCmd.ucArgv[2],
						stSysCmd.ucArgv[3],
						stSysCmd.ucArgv[4],
						stSysCmd.ucArgv[5],
						stSysCmd.ucArgv[6],
						stSysCmd.ucArgv[7],
						stSysCmd.ucArgv[8],
						stSysCmd.ucArgv[9],
						stSysCmd.ucArgv[10],
						stSysCmd.ucArgv[11], (char *)0);
		}
		else if (stSysCmd.ucArgc == 13)
		{
			nErr = execlp(stSysCmd.ucArgv[0],
						stSysCmd.ucArgv[1],
						stSysCmd.ucArgv[2],
						stSysCmd.ucArgv[3],
						stSysCmd.ucArgv[4],
						stSysCmd.ucArgv[5],
						stSysCmd.ucArgv[6],
						stSysCmd.ucArgv[7],
						stSysCmd.ucArgv[8],
						stSysCmd.ucArgv[9],
						stSysCmd.ucArgv[10],
						stSysCmd.ucArgv[11],
						stSysCmd.ucArgv[12], (char *)0);
		}
		else if (stSysCmd.ucArgc == 14)
		{
			nErr = execlp(stSysCmd.ucArgv[0],
						stSysCmd.ucArgv[1],
						stSysCmd.ucArgv[2],
						stSysCmd.ucArgv[3],
						stSysCmd.ucArgv[4],
						stSysCmd.ucArgv[5],
						stSysCmd.ucArgv[6],
						stSysCmd.ucArgv[7],
						stSysCmd.ucArgv[8],
						stSysCmd.ucArgv[9],
						stSysCmd.ucArgv[10],
						stSysCmd.ucArgv[11],
						stSysCmd.ucArgv[12],
						stSysCmd.ucArgv[13], (char *)0);
		}
		else if (stSysCmd.ucArgc == 15)
		{
			nErr = execlp(stSysCmd.ucArgv[0],
						stSysCmd.ucArgv[1],
						stSysCmd.ucArgv[2],
						stSysCmd.ucArgv[3],
						stSysCmd.ucArgv[4],
						stSysCmd.ucArgv[5],
						stSysCmd.ucArgv[6],
						stSysCmd.ucArgv[7],
						stSysCmd.ucArgv[8],
						stSysCmd.ucArgv[9],
						stSysCmd.ucArgv[10],
						stSysCmd.ucArgv[11],
						stSysCmd.ucArgv[12],
						stSysCmd.ucArgv[13],
						stSysCmd.ucArgv[14], (char *)0);
		} */

		eErr = IPC_SEC_SYS_CMD_EXCELP_ERR;
		exit(127);
	}

	do
	{
		if (waitpid(nPid, &nStatus, 0) == -1)
		{
			VK_Print("local_secma_handle_SysCmd killed child proc...\n");
			break;
		}
	} while(1);

	eErr = ((nStatus & 0xff) == 0) ? IPC_SEC_SYS_CMD_NO_ERR : IPC_SEC_SYS_CMD_EXCELP_RET_ERR;
	VK_Print("local_secma_handle_SysCmd end : eErr = 0x%x\n", eErr);

	return eErr;
}


static HINT32 	local_secma_handle_Ioctl(void *pvMsg, HUINT32 ulSize)
{
	HINT32 nErr = ERR_OK;
	HINT32 nId=0;
	HINT32 nRequest=0;

	VK_memcpy(&nId, pvMsg, sizeof(HINT32));
	VK_memcpy(&nRequest, pvMsg+4, sizeof(HINT32));

#if !(IPC_IOCTL_DEBUG)
	VK_Print("=== [%s][%d] nid : %d, nRequest : 0x%x === \n", __FUNCTION__, __LINE__, nId, nRequest);
#endif

	switch(nRequest)
	{
		case SIOCGIWNAME :		//0x8b01
		case SIOCGIWNWID :		//0x8b03
		case SIOCGIWFREQ :		//0x8b05
		case SIOCGIWAP :		//0x8b15
		case SIOCSIWESSID : 			//0x8b1a
		case WLAN_PRIVIOCTL_SET :	//0x8be2

		{
			struct iwreq iwrequest;
			HINT8 szBuf[IPC_IOCTL_MAX_DATA_SIZE];

			VK_memset(szBuf, 0x00, IPC_IOCTL_MAX_DATA_SIZE);
			VK_memset(&iwrequest, 0x00, sizeof(struct iwreq));

			VK_memcpy(&iwrequest, pvMsg+8, sizeof(struct iwreq));
			VK_memcpy(szBuf, pvMsg+8+sizeof(struct iwreq), iwrequest.u.data.length);

			iwrequest.u.data.pointer = szBuf;

#if (IPC_IOCTL_DEBUG)
			VK_Print("\n");
			VK_Print("\t\t [SERVER] [%d] ifr_name : %s \n",  __LINE__, iwrequest.ifr_name);
			VK_Print("\t\t [SERVER] [%d] pointer : %s \n",  __LINE__, szBuf);
			VK_Print("\t\t ioctl(nId, nRequest, (HINT8 *)&iwrequest) [SendData - X]\n\n");
#endif

			nErr = ioctl(nId, nRequest, (HINT8 *)&iwrequest);

			break;
		}

		case SIOCGIWMODE :		//0x8b07
		case SIOCGIWRATE :		//0x8b21
		{
			struct iwreq iwrequest;

			VK_memset(&iwrequest, 0x00, sizeof(struct iwreq));
			VK_memcpy(&iwrequest, pvMsg+8, sizeof(struct iwreq));

			nErr = ioctl(nId, nRequest, (HINT8 *)&iwrequest);

#if (IPC_IOCTL_DEBUG)
			VK_Print("\t\t [SERVER] [%d] [return  iwrequest]\n",	__LINE__);
#endif
			VK_memcpy(pvMsg, &iwrequest, sizeof(struct iwreq));

			break;
		}

		case SIOCGIWRANGE :		//0x8b0b
		case SIOCGIWSTATS : 	//0x8b0f
		case SIOCGIWESSID : 	//0x8b1b
		case SIOCGIWENCODE :	//0x8b2b
		case WLAN_PRIVIOCTL_CONNSTAT :		//0x0004
		case WLAN_PRIVIOCTL_DRIVERVER :		//0x0005
		case WLAN_PRIVIOCTL_BAINFO :		//0x0006
		case WLAN_PRIVIOCTL_DESCINFO :		//0x0007
		case WLAN_PRVIIOCTL_RADIO_OFF :		//0x000a
		case WLAN_PRIVIOCTL_RADIO_ON :		//0x000b
		case WLAN_PRIVIOCTL_SHOW :			//0x0014
		case WLAN_PRIVIOCTL_ADHOCENTRY :		//0x0015
		case WLAN_PRIVIOCTL_STAT :				//0x8be9
		case WLAN_PRIVIOCTL_GET_SITE_SURVEY :	//0x8bed
		case 0x8bf1 :
		{
			struct iwreq iwrequest;
			HINT8 szBuf[IPC_IOCTL_MAX_DATA_SIZE];

			VK_memset(szBuf, 0x00, IPC_IOCTL_MAX_DATA_SIZE);
			VK_memset(&iwrequest, 0x00, sizeof(struct iwreq));
			VK_memcpy(&iwrequest, pvMsg+8, sizeof(struct iwreq));

			iwrequest.u.data.pointer = szBuf;

#if (IPC_IOCTL_DEBUG)
			VK_Print("\n");
			VK_Print("\t\t [SERVER] [%d] ifr_name : %s \n",  __LINE__, iwrequest.ifr_name);
			VK_Print("\t\t [SERVER] [%d] length : %d \n",  __LINE__, iwrequest.u.data.length);
			VK_Print("\t\t [SERVER] [%d] flags : %d \n\n",  __LINE__, iwrequest.u.data.flags);
			VK_Print("\t\t ioctl(nId, nRequest, (HINT8 *)&iwrequest) \n\n");
#endif
			nErr = ioctl(nId, nRequest, (HINT8 *)&iwrequest);

#if (IPC_IOCTL_DEBUG)
			VK_Print("\t\t [SERVER] [%d] pointer = %s \n",  __LINE__, szBuf);
			VK_Print("\t\t [SERVER] [%d] length = %d \n\n",  __LINE__, iwrequest.u.data.length);
#endif

			VK_memset(pvMsg, 0x00, ulSize);
			VK_memcpy(pvMsg,  &iwrequest.u.data.length, sizeof(HUINT16));
			VK_memcpy(pvMsg+sizeof(HUINT16), szBuf, iwrequest.u.data.length);
			break;
		}

		case SIOCGIWPRIV :		//0x8b0d
		{
			struct iwreq iwrequest;
			struct iw_priv_args *pIW_privates = NULL;

			VK_memset(&iwrequest, 0x00, sizeof(struct iwreq));
			VK_memcpy(&iwrequest, pvMsg+8, sizeof(struct iwreq));

			pIW_privates = HLIB_STD_MemAlloc(iwrequest.u.data.length * sizeof(struct iw_priv_args));
			if ( pIW_privates == NULL )
			{
				VK_Print("[%s][%d] mem alloc. fail  \n", __FUNCTION__, __LINE__);
				return -1;
			}

			iwrequest.u.data.pointer = (caddr_t) pIW_privates;

#if (IPC_IOCTL_DEBUG)
			VK_Print("\n");
			VK_Print("\t\t [SERVER] [%d] ifr_name : %s \n",  __LINE__, iwrequest.ifr_name);
			VK_Print("\t\t [SERVER] [%d] length : %d \n",  __LINE__, iwrequest.u.data.length);
			VK_Print("\t\t [SERVER] [%d] flags : %d \n\n",	__LINE__, iwrequest.u.data.flags);
			VK_Print("\t\t ioctl(nId, nRequest, (HINT8 *)&iwrequest) \n\n");
#endif

			nErr = ioctl(nId, nRequest, (HINT8 *)&iwrequest);

#if (IPC_IOCTL_DEBUG)
			VK_Print("\t\t [SERVER] [%d] pointer = %s \n",	__LINE__, pIW_privates);
			VK_Print("\t\t [SERVER] [%d] length = %d \n\n",  __LINE__, iwrequest.u.data.length);
#endif

			if (nErr <0)
			{
				nErr = -errno;
			}
			else
			{
				VK_memset(pvMsg, 0x00, ulSize);
				VK_memcpy(pvMsg,  &iwrequest.u.data.length, sizeof(HUINT16));
				VK_memcpy(pvMsg+sizeof(HUINT16), pIW_privates, iwrequest.u.data.length * sizeof(struct iw_priv_args));
			}

			HLIB_STD_MemFree(pIW_privates);

			break;
		}

		case SIOCSIWSCAN :		//0x8b18
		{
			struct iwreq iwrequest;

			VK_memset(&iwrequest, 0x00, sizeof(struct iwreq));
			VK_memcpy(&iwrequest, pvMsg+8, sizeof(struct iwreq));

#if (IPC_IOCTL_DEBUG)
			VK_Print("\n");
			VK_Print("\t\t [SERVER] [%d] ifr_name : %s \n",  __LINE__, iwrequest.ifr_name);
			VK_Print("\t\t ioctl(nId, nRequest, (HINT8 *)&iwrequest)[SendData - X] \n\n");
#endif

			nErr = ioctl(nId, nRequest, (HINT8 *)&iwrequest);

			break;
		}

		case SIOCGIWSCAN :		//0x8b19
		{
			struct iwreq iwrequest;
			HINT8 *pszBuf = NULL;

			VK_memset(&iwrequest, 0x00, sizeof(struct iwreq));
			VK_memcpy(&iwrequest, pvMsg+8, sizeof(struct iwreq));

			pszBuf = HLIB_STD_MemAlloc(iwrequest.u.data.length);
			if (pszBuf == NULL)
			{
				return -1;
			}

			iwrequest.u.data.pointer = pszBuf;

#if (IPC_IOCTL_DEBUG)
			VK_Print("\n");
			VK_Print("\t\t [SERVER] [%d] ifr_name : %s \n",  __LINE__, iwrequest.ifr_name);
			VK_Print("\t\t [SERVER] [%d] length : %d \n",  __LINE__, iwrequest.u.data.length);
			VK_Print("\t\t [SERVER] [%d] flags : %d \n\n",	__LINE__, iwrequest.u.data.flags);
			VK_Print("\t\t ioctl(nId, nRequest, (HINT8 *)&iwrequest) \n\n");
#endif

			nErr = ioctl(nId, nRequest, (HINT8 *)&iwrequest);

#if (IPC_IOCTL_DEBUG)
			VK_Print("\t\t [SERVER] [%d] pointer = %s \n",	__LINE__, pszBuf);
			VK_Print("\t\t [SERVER] [%d] length = %d \n\n",  __LINE__, iwrequest.u.data.length);
#endif

			if (nErr <0)
			{
				nErr = -errno;
			}
			else
			{
				VK_memset(pvMsg,  0x00, ulSize);
				VK_memcpy(pvMsg,  &iwrequest.u.data.length, sizeof(HUINT16));
				local_secma_handle_FileWrite("SIOCGIWSCAN.raw" , 0 , iwrequest.u.data.length, pszBuf);
			}

			HLIB_STD_MemFree(pszBuf);

			break;
		}

		case SIOCADDRT :	// 0x890b
		case SIOCDELRT :		// 0x890c
		{
			struct rtentry rm;
			char szEthernetName[16];

			VK_memset(&rm, 0x00, sizeof(struct rtentry));
			VK_memcpy(&rm, pvMsg+8, sizeof(struct rtentry));
			VK_memset(szEthernetName, 0x00, 16);
			VK_memcpy(szEthernetName, pvMsg+8+sizeof(struct rtentry), 16);

			rm.rt_dev = szEthernetName;

			nErr = ioctl(nId, nRequest, (HINT8 *)&rm);

			break;
		}

		case SIOCGIFFLAGS :		//0x8913
		case SIOCGIFADDR :		//0x8915
		case SIOCGIFBRDADDR :	//0x8919
		case SIOCGIFNETMASK :	//0x891b
		case SIOCGIFHWADDR :	//0x8927
		case SIOCGIFCOUNT :		//0x8938
		{
			struct ifreq ifrequest;

			VK_memset(&ifrequest, 0x00, sizeof(struct ifreq));
			VK_memcpy(&ifrequest, pvMsg+8, sizeof(struct ifreq));

			nErr = ioctl(nId, nRequest, (HINT8 *)&ifrequest);

			VK_memset(pvMsg, 0x00, ulSize);
			VK_memcpy(pvMsg, &ifrequest, sizeof(struct ifreq));

			break;
		}

		case SIOCSIFFLAGS :		//0x8914
		case SIOCSIFADDR :		//0x8916
		case SIOCSIFBRDADDR :	//0x891a
		case SIOCSIFNETMASK :	//0x891c
		case SIOCSIFHWADDR :	//0x8924
		{
			struct ifreq ifrequest;

			VK_memset(&ifrequest, 0x00, sizeof(struct ifreq));
			VK_memcpy(&ifrequest, pvMsg+8, sizeof(struct ifreq));

			nErr = ioctl(nId, nRequest, (HINT8 *)&ifrequest);

			break;
		}

		case SIOCGARP :			// 0x8954
		{
			struct arpreq areq;

			VK_memset(&areq, 0x00, sizeof(struct arpreq));
			VK_memcpy(&areq, pvMsg+8, sizeof(struct arpreq));

			nErr = ioctl(nId, nRequest, (HINT8 *)&areq);

			VK_memset(pvMsg, 0x00, ulSize);
			VK_memcpy(pvMsg, &areq, sizeof(struct arpreq));

			break;
		}

		case HDIO_DRIVE_TASK :	//0x031e
		case SMART_CMD :		//0x031f
		{
			unsigned char ucBuf[SMART_BUF_SIZE];

			VK_memset(ucBuf, 0x00, SMART_BUF_SIZE);
			VK_memcpy(ucBuf, pvMsg+8, SMART_BUF_SIZE);

			nErr = ioctl(nId, nRequest, (HINT8 *)&ucBuf);

			if (nRequest == SMART_CMD)
			{
				VK_memset(pvMsg, 0x00, ulSize);
				VK_memcpy(pvMsg, ucBuf, SMART_BUF_SIZE);
			}
			break;
		}

		case HDIO_GET_IDENTITY : // 0x030d
		{
			unsigned char dev_param[256];

			VK_memset(dev_param , 0x00, 256);
			VK_memcpy(dev_param , pvMsg+8, 256);

			nErr = ioctl(nId, nRequest, (HINT8 *)&dev_param );

			VK_memset(pvMsg, 0x00, ulSize);
			VK_memcpy(pvMsg, dev_param , 256);

			break;
		}

		case BLKGETSIZE64 :
		{
			unsigned long long ullDisksize;

			nErr = ioctl(nId, nRequest, (HINT8 *)&ullDisksize);

			VK_memset(pvMsg, 0x00, ulSize);
			VK_memcpy(pvMsg, &ullDisksize, sizeof(unsigned long long));

			break;
		}

		case BLKFLSBUF:
		case BLKRRPART :
		{
			nErr = ioctl(nId, nRequest, NULL);
			break;
		}

		case BLKSSZGET :
		{
			unsigned int ulSecSize;

			nErr = ioctl(nId, nRequest, (HINT8 *)&ulSecSize);

			VK_memset(pvMsg, 0x00, ulSize);
			VK_memcpy(pvMsg, &ulSecSize, sizeof(unsigned int));

			break;
		}

		case BLKGETSIZE :
		{
			unsigned long ulBlockNum;

			nErr = ioctl(nId, nRequest, (HINT8 *)&ulBlockNum);

			VK_memset(pvMsg, 0x00, ulSize);
			VK_memcpy(pvMsg, &ulBlockNum, sizeof(unsigned long));

			break;
		}

		case HDIO_GETGEO :
		{
			struct hd_geometry stGeometry;

			nErr = ioctl(nId, nRequest, (HINT8 *)&stGeometry);

			VK_memset(pvMsg, 0x00, ulSize);
			VK_memcpy(pvMsg, &stGeometry, sizeof(struct hd_geometry));

			VK_Print("\t\t=== [%s][%d] ulHeads = %d === \n", __FUNCTION__, __LINE__, stGeometry.heads);
			VK_Print("\t\t=== [%s][%d] sectors = %d=== \n", __FUNCTION__, __LINE__, stGeometry.sectors);

			break;
		}

		case SIOCGIFCONF : 		// 0x8912 mslee -> server, client OK
		{
			struct ifconf ifcfg;

			VK_memset(&ifcfg, 0x00, sizeof(struct ifconf));
			VK_memcpy(&ifcfg, pvMsg+8, sizeof(struct ifconf));

			nErr = ioctl(nId, nRequest, (HINT8 *)&ifcfg);

			VK_memset(pvMsg, 0x00, ulSize);
			VK_memcpy(pvMsg, &ifcfg, sizeof(struct ifconf));

			break;
		}

		default :
			VK_Print("##### [%s][%d] 0x%x ##### \n", __FUNCTION__, __LINE__, nRequest);
			break;
	}

#if !(IPC_IOCTL_DEBUG)
	VK_Print("=== END [%s][%d] nid : %d, nRequest : 0x%x === \n", __FUNCTION__, __LINE__, nId, nRequest);
#endif

	return nErr;
}


static HINT32 local_secma_handle_Stat(void *pvMsg, HUINT32 ulSize)
{
	HINT32 nErr;
	IpcSecStat_t stStat;

	VK_memset(&stStat, 0, sizeof(IpcSecStat_t));
	VK_memcpy(&stStat, pvMsg, sizeof(IpcSecStat_t));

	VK_Print("##### [%s][%d] File Name:%s : ##### \n", __FUNCTION__, __LINE__, stStat.ucPath);

	nErr = stat(stStat.ucPath, &stStat.stStat);
	if (nErr < 0)
		VK_Print("##### error = %s \n ##### ", strerror(errno));

	VK_memcpy(pvMsg, &stStat, sizeof(IpcSecStat_t));

	return nErr;
}


static HINT32 local_secma_handle_Lstat(void *pvMsg, HUINT32 ulSize)
{
	HINT32 nErr;
	IpcSecStat_t stStat;

	VK_memset(&stStat, 0, sizeof(IpcSecStat_t));
	VK_memcpy(&stStat, pvMsg, sizeof(IpcSecStat_t));

	VK_Print("##### [%s][%d] File Name:%s : ##### \n", __FUNCTION__, __LINE__, stStat.ucPath);

	nErr = lstat(stStat.ucPath, &stStat.stStat);
	if (nErr < 0)
		VK_Print("##### error = %s \n ##### ", strerror(errno));

	VK_memcpy(pvMsg, &stStat, sizeof(IpcSecStat_t));

	return nErr;
}


static HINT32 local_secma_handle_Socket(void *pvMsg, HUINT32 ulSize)
{
	HINT32 nErr;
	IpcSecSocket_t stSocket;

	VK_memset(&stSocket, 0, sizeof(IpcSecSocket_t));
	VK_memcpy(&stSocket, pvMsg, sizeof(IpcSecSocket_t));

	VK_Print("[%s][%d] nId : %d, nRequest : 0x%x \n", __FUNCTION__, __LINE__, stSocket.nDomain, stSocket.nType);

	nErr = socket(stSocket.nDomain, stSocket.nType, stSocket.nProtocol);
	VK_Print("end [%s][%d] nId : %d, nRequest : 0x%x \n", __FUNCTION__, __LINE__, stSocket.nDomain, stSocket.nType);

	return nErr;
}


static HINT32 	local_secma_handle_Select(void *pvMsg, HUINT32 ulSize)
{
	HINT32 nRet;
	IpcSecSelect_t stSelect;

	VK_memset(&stSelect, 0, sizeof(IpcSecSelect_t));
	VK_memcpy(&stSelect, pvMsg, sizeof(IpcSecSelect_t));

	nRet = select(stSelect.nFd, &stSelect.stReadFds, &stSelect.stWriteFds, &stSelect.stExceptFds, &stSelect.stTimeout);

	if (nRet < 0)
		nRet = -errno;

	return nRet;
}


static HINT32 local_secma_handle_Send(void *pvMsg, HUINT32 ulSize)
{
	HLONG nErr;
	HINT32 nFd;
	HUINT8 *pcBuf = NULL;
	HLONG nLen;
	HINT32 nFlag;

	VK_memcpy(&nFd, pvMsg, sizeof(HINT32));
	VK_memcpy(&nLen, pvMsg+4, sizeof(HLONG));
	VK_memcpy(&nFlag, pvMsg+8, sizeof(HINT32));

	pcBuf = HLIB_STD_MemAlloc(nLen);
	if (pcBuf == NULL)
		return -1;

	VK_memcpy(pcBuf, pvMsg+12, nLen);

	nErr = send(nFd, pcBuf, nLen, nFlag);

	HLIB_STD_MemFree(pcBuf);

	return nErr;
}


static HINT32 local_secma_handle_Recv(void *pvMsg, HUINT32 ulSize)
{
	HINT32 nErr;
	IpcSecRead_t stRead;
	HUINT8 ucData[IPC_TEMPFILE_MAX_SIZE];

	VK_memset(&stRead, 0, sizeof(IpcSecRead_t));
	VK_memcpy(&stRead, pvMsg, sizeof(IpcSecRead_t));

	nErr = recv(stRead.fd, ucData, stRead.ulSize, stRead.ulFlag);
	if (nErr != -1)
	{
		VK_Print("##### ucData[0]=[0x%x] ucData[1]=[0x%x]\n ##### ", ucData[0], ucData[1]);
		local_secma_handle_FileWrite("TempFileForRead", 0, nErr, ucData);
	}
	else
	{
		VK_Print("##### error = %s \n ##### ", strerror(errno));
	}

	return nErr;
}


static HINT32 local_secma_handle_Open(void *pvMsg, HUINT32 ulSize)
{
	HINT32 nErr;
	IpcSecFileOpen_t stFileOpen;

	VK_memset(&stFileOpen, 0, sizeof(IpcSecFileOpen_t));
	VK_memcpy(&stFileOpen, pvMsg, sizeof(IpcSecFileOpen_t));

	VK_Print("##### [%s][%d] File Name:%s : ##### \n", __FUNCTION__, __LINE__, stFileOpen.ucPath);

#if 0
	3rd parameter nMode
	: /opt/toolchains/stbgcc-4.5.3-1.3/mipsel-linux-uclibc/sys-root/usr/include/bits/stat.h

	#define __S_IREAD       	0400    /* Read by owner.  */
	#define __S_IWRITE      	0200    /* Write by owner.  */
	#define __S_IEXEC       	0100    /* Execute by owner.  */
#endif

	nErr = open(stFileOpen.ucPath, stFileOpen.nFlags);
	if (nErr < 0)
	{
		VK_Print("##### error = %s \n ##### ", strerror(errno));
	}

	return nErr;
}


static HINT32 local_secma_handle_Open2(void *pvMsg, HUINT32 ulSize)
{
	HINT32 nErr;
	IpcSecFileOpen_t stFileOpen;

	VK_memset(&stFileOpen, 0, sizeof(IpcSecFileOpen_t));
	VK_memcpy(&stFileOpen, pvMsg, sizeof(IpcSecFileOpen_t));

	VK_Print("##### [%s][%d] File Name:%s : ##### \n", __FUNCTION__, __LINE__, stFileOpen.ucPath);

#if 0
	3rd parameter nMode
	: /opt/toolchains/stbgcc-4.5.3-1.3/mipsel-linux-uclibc/sys-root/usr/include/bits/stat.h

	#define __S_IREAD       	0400    /* Read by owner.  */
	#define __S_IWRITE      	0200    /* Write by owner.  */
	#define __S_IEXEC       	0100    /* Execute by owner.  */
#endif

	nErr = open(stFileOpen.ucPath, stFileOpen.nFlags, stFileOpen.mode);
	if (nErr < 0)
	{
		VK_Print("##### error = %s \n ##### ", strerror(errno));
	}

	return nErr;
}


static HINT32 local_secma_handle_Close(void *pvMsg, HUINT32 ulSize)
{
	HINT32 nErr;
	HINT32 nFd;

	VK_memcpy(&nFd, pvMsg, sizeof(HINT32));

	nErr = close(nFd);

	return nErr;
}


static HINT32 local_secma_handle_Lseek(void *pvMsg, HUINT32 ulSize)
{
	HINT32 nErr;
	IpcSecLseek_t stLseek;

	VK_memset(&stLseek, 0, sizeof(IpcSecLseek_t));
	VK_memcpy(&stLseek, pvMsg, sizeof(IpcSecLseek_t));

	nErr = lseek(stLseek.fd, stLseek.pos, stLseek.origin);
	if (nErr < 0)
	{
		VK_Print("##### error = %s \n ##### ", strerror(errno));
	}

	return nErr;
}


static HINT32 local_secma_handle_Read(void *pvMsg, HUINT32 ulSize)
{
	HINT32 nErr;
	IpcSecRead_t stRead;
	HUINT8 ucData[IPC_TEMPFILE_MAX_SIZE];

	VK_memset(&stRead, 0, sizeof(IpcSecRead_t));
	VK_memcpy(&stRead, pvMsg, sizeof(IpcSecRead_t));

	nErr = read(stRead.fd, ucData, stRead.ulSize);
	if (nErr != -1)
	{
		VK_Print("##### ucData[0]=[0x%x] ucData[1]=[0x%x]\n ##### ", ucData[0], ucData[1]);
		local_secma_handle_FileWrite("TempFileForRead", 0, nErr, ucData);
	}
	else
	{
		VK_Print("##### error = %s \n ##### ", strerror(errno));
	}

	return nErr;
}


static HINT32 local_secma_handle_Write(void *pvMsg, HUINT32 ulSize)
{
	HLONG nErr;
	HINT32 nFd;
	HUINT8 *pcBuf = NULL;
	HLONG nLen;

	VK_memcpy(&nFd, pvMsg, sizeof(HINT32));
	VK_memcpy(&nLen, pvMsg+4, sizeof(HLONG));

	pcBuf = HLIB_STD_MemAlloc(nLen);
	if (pcBuf == NULL)
		return -1;

	VK_memcpy(pcBuf, pvMsg+8, nLen);

	nErr = write(nFd, pcBuf, nLen);

	HLIB_STD_MemFree(pcBuf);

	return nErr;
}


static HINT32 local_secma_handle_Rename(void *pvMsg, HUINT32 ulSize)
{
	HINT32  nErr;
	IpcSecFileCtl_t stFileCtl;

	VK_memset(&stFileCtl, 0, sizeof(IpcSecFileCtl_t));
	VK_memcpy(&stFileCtl, pvMsg, sizeof(IpcSecFileCtl_t));

	nErr = rename(stFileCtl.ucPath, stFileCtl.ucNewPath);

	return nErr;
}


static HINT32 local_secma_handle_Vfork(void *pvMsg, HUINT32 ulSize)
{
	HINT32 nErr;
	HINT32 nPid;
	HINT32 nStatus;
	IpcSecVfork_t stVfork;
	(void)(ulSize);

	VK_memset(&stVfork, 0, sizeof(IpcSecVfork_t));
	VK_memcpy(&stVfork, pvMsg, sizeof(IpcSecVfork_t));

	VK_Print("[local_secma_handle_Vfork] Proc_Name(%s)\n", stVfork.ucProcName);

	nPid = vfork();
	switch (nPid)
	{
		case -1:
			perror("Error to Launch HAMA Application Manager...\n");
			break;

		case  0:
			setsid();
			VK_Print("Launching HAMA Application Manager...[%s]\n", stVfork.ucProcName);
			if (execvp(stVfork.ucProcName, NULL) < 0)
				exit(SIGABRT);

			exit(0);
			break;

		default:
			break;
	}

//	waitpid(nPid, &nStatus, 0);
	stVfork.nPid = nPid;

	VK_memcpy(pvMsg, &stVfork, sizeof(IpcSecVfork_t));

	return nErr;
}


static HINT32 local_secma_handle_HddPwrCtrl(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	DI_HDD_PWR nPwr;

	VK_memset(&nPwr, 0, sizeof(DI_HDD_PWR));
	VK_memcpy(&nPwr, pvMsg, sizeof(DI_HDD_PWR));

	nErr = DI_HDD_PWR_Ctrl(nPwr);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	return 0;
}


// 2nd internal HDD (detachable HDD)만 power control 할때
static HINT32 local_secma_handle_Hdd2PwrCtrl(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	DI_HDD_PWR nPwr;

	VK_memset(&nPwr, 0, sizeof(DI_HDD_PWR));
	VK_memcpy(&nPwr, pvMsg, sizeof(DI_HDD_PWR));

	nErr = DI_HDD_ControlPower(DI_HDD_PORT_NUM_2, nPwr);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	return 0;
}


static HINT32 local_secma_handle_UsbPwrCtrl(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	DI_USB_PWR_t nPwr;

	VK_memset(&nPwr, 0, sizeof(DI_HDD_PWR));
	VK_memcpy(&nPwr, pvMsg, sizeof(DI_USB_PWR_t));

	nErr = DI_USB_PWR_Ctrl(nPwr);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	return 0;
}


static HINT32 local_secma_handle_UsbHostConPwrCtrl(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	DI_USB_PWR_t nPwr;

	VK_memset(&nPwr, 0, sizeof(DI_HDD_PWR));
	VK_memcpy(&nPwr, pvMsg, sizeof(DI_USB_PWR_t));

	nErr = DI_USB_HOST_CONTROLLER_PWR_Ctrl(nPwr);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	return 0;
}


static HINT32 local_secma_handle_UsbStop(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;

	pvMsg = pvMsg;
	ulSize = ulSize;

	nErr = DI_USB_Stop();
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	return 0;
}


static HINT32 local_secma_handle_HidCbNoti(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	DI_USB_HID_DEVICE eDevices;

	VK_memset(&eDevices, 0, sizeof(DI_USB_HID_DEVICE));
	VK_memcpy(&eDevices, pvMsg, sizeof(DI_USB_HID_DEVICE));

	nErr = DI_USB_HID_RegisterHIDEventCallback(eDevices, local_secma_handle_HidCallback);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	return 0;
}


static HINT32 local_secma_handle_HddCbNoti(void *pvMsg, HUINT32 ulSize)
{
	DI_HDD_RegisterCallback(local_secma_handle_HddCallback);
	DI_HDD_PWR_Ctrl(DI_HDD_PWR_ON);

	return 0;
}


static HINT32 local_secma_handle_UsbCbNoti(void *pvMsg, HUINT32 ulSize)
{
	DI_USB_RegisterCallback(local_secma_handle_UsbCallback);

	return 0;
}


static HINT32 local_secma_handle_WlanCbNoti(void *pvMsg, HUINT32 ulSize)
{
	DI_USB_WIRELESS_RegisterCallback(local_secma_handle_WlanCallback);

	return 0;
}


#if (1)
static HINT32 local_secma_handle_HddDstCbNoti(void *pvMsg, HUINT32 ulSize)
{
	DI_HDD_RegisterDstCallback(local_secma_handle_DstCallback);

	return 0;
}


static HINT32 local_secma_handle_HddFormatCbNoti(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;

	nErr = DI_FS_RegisterFormatProgressCallback((DI_FS_Event_Callback_Func)local_secma_handle_FormatCallback);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	return 0;
}


static HINT32 local_secma_handle_FsGetDeviceFileName(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	IpcSecDevName_t stDevName;

	VK_memset(&stDevName, 0, sizeof(IpcSecDevName_t));
	VK_memcpy(&stDevName, pvMsg, sizeof(IpcSecDevName_t));

	nErr = DI_FS_GetDeviceFileName(stDevName.eDevIdx, stDevName.szFileName);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	VK_memcpy(pvMsg, &stDevName, sizeof(IpcSecDevName_t));

	return 0;
}


static HINT32 local_secma_handle_FsGetDeviceFileFullName(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	IpcSecDevName_t stDevName;

	VK_memset(&stDevName, 0, sizeof(IpcSecDevName_t));
	VK_memcpy(&stDevName, pvMsg, sizeof(IpcSecDevName_t));

	nErr = DI_FS_GetDeviceFileFullName(stDevName.eDevIdx, stDevName.szFileName);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	VK_memcpy(pvMsg, &stDevName, sizeof(IpcSecDevName_t));

	return 0;
}


static HINT32 local_secma_handle_FsGetPartitionFileName(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	IpcSecPartitionName_t stPartName;

	VK_memset(&stPartName, 0, sizeof(IpcSecPartitionName_t));
	VK_memcpy(&stPartName, pvMsg, sizeof(IpcSecPartitionName_t));

	nErr = DI_FS_GetPartitionFileName(stPartName.eDevIdx, stPartName.ulPartitionIdx, stPartName.szFileName, stPartName.nSizeOfFileName);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	VK_memcpy(pvMsg, &stPartName, sizeof(IpcSecPartitionName_t));

	return 0;
}


static HINT32 local_secma_handle_FsGetPartitionFullFileName(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	IpcSecPartitionName_t stPartName;

	VK_memset(&stPartName, 0, sizeof(IpcSecPartitionName_t));
	VK_memcpy(&stPartName, pvMsg, sizeof(IpcSecPartitionName_t));

	nErr = DI_FS_GetPartitionFullFileName(stPartName.eDevIdx, stPartName.ulPartitionIdx, stPartName.szFileName);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	VK_memcpy(pvMsg, &stPartName, sizeof(IpcSecPartitionName_t));

	return 0;
}


static HINT32 local_secma_handle_FsIsFileExist(void *pvMsg, HUINT32 ulSize)
{
	HUINT8 nErr;
	IpcSecFileCtl_t stFileCtl;

	VK_memset(&stFileCtl, 0, sizeof(IpcSecFileCtl_t));
	VK_memcpy(&stFileCtl, pvMsg, sizeof(IpcSecFileCtl_t));

	nErr = DI_FS_IsFileExist(stFileCtl.ucPath); // bool type (true / flase)

	return nErr;
}


static HINT32 local_secma_handle_FsIsHDDExist(void *pvMsg, HUINT32 ulSize)
{
	HUINT8 nErr;
	IpcSecFileCtl_t stFileCtl;

	VK_memset(&stFileCtl, 0, sizeof(IpcSecFileCtl_t));
	VK_memcpy(&stFileCtl, pvMsg, sizeof(IpcSecFileCtl_t));

	nErr = DI_FS_HDDExist(stFileCtl.ucPath); // bool type (true / flase)

	return nErr;
}


static HINT32 local_secma_handle_FsIsDirExist(void *pvMsg, HUINT32 ulSize)
{
	HUINT8 nErr;
	IpcSecFileCtl_t stFileCtl;

	VK_memset(&stFileCtl, 0, sizeof(IpcSecFileCtl_t));
	VK_memcpy(&stFileCtl, pvMsg, sizeof(IpcSecFileCtl_t));

	nErr = DI_FS_IsDirExist(stFileCtl.ucPath); // bool type (true / flase)

	return nErr;
}


static HINT32 local_secma_handle_FsMkdir(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	IpcSecFileCtl_t stFileCtl;

	VK_memset(&stFileCtl, 0, sizeof(IpcSecFileCtl_t));
	VK_memcpy(&stFileCtl, pvMsg, sizeof(IpcSecFileCtl_t));

	VK_Print("##### [%s][%d] File Name:%s : ##### \n", __FUNCTION__, __LINE__, stFileCtl.ucPath);

	nErr = DI_FS_Mkdir(stFileCtl.ucPath);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	{
		if (strstr(stFileCtl.ucPath, "/mnt/hd") != NULL)
		{
			struct passwd *u_info;
			struct group  *g_info;

			u_info = getpwnam("settop");
			g_info = getgrnam("settop");

			if (u_info != NULL && g_info != NULL)
			{
				nErr = chown(stFileCtl.ucPath, u_info->pw_uid, g_info->gr_gid);
			}
			else
			{
				nErr = -1;
			}
		}
	}

	return nErr;
}


static HINT32 local_secma_handle_FsGetVolumeUuid(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	IpcSecPartitionName_t stPartName;

	VK_Print("\n\n\t ========> local_secma_handle_FsGetVolumeUuid : start\n");

	VK_memset(&stPartName, 0, sizeof(IpcSecPartitionName_t));
	VK_memcpy(&stPartName, pvMsg, sizeof(IpcSecPartitionName_t));

	nErr = DI_FS_GetVolumeUuid(stPartName.eDevIdx, stPartName.ulPartitionIdx, stPartName.szFileName, stPartName.nSizeOfFileName, &stPartName.nUuIdSize);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	VK_memcpy(pvMsg, &stPartName, sizeof(IpcSecPartitionName_t));
	VK_Print("\t ========> local_secma_handle_FsGetVolumeUuid : end\n");

	return 0;
}


static HINT32 local_secma_handle_FsGetVolumeNtfsUuid(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	IpcSecPartitionName_t stPartName;

	VK_Print("\n\n\t ========> local_secma_handle_FsGetVolumeNtfsUuid : start\n");

	VK_memset(&stPartName, 0, sizeof(IpcSecPartitionName_t));
	VK_memcpy(&stPartName, pvMsg, sizeof(IpcSecPartitionName_t));

	nErr = DI_FS_GetVolumeNtfsUuid(stPartName.eDevIdx, stPartName.ulPartitionIdx, stPartName.szFileName, stPartName.nSizeOfFileName);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	VK_memcpy(pvMsg, &stPartName, sizeof(IpcSecPartitionName_t));
	VK_Print("\t ========> local_secma_handle_FsGetVolumeNtfsUuid : end\n");

	return 0;
}


static HINT32 local_secma_handle_FsMountPartition(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	IpcSecDevMbr_t stDevMbr;

	VK_Print("\n\n\t ========> local_secma_handle_FsMountPartition : start\n");

	VK_memset(&stDevMbr, 0, sizeof(IpcSecDevMbr_t));
	VK_memcpy(&stDevMbr, pvMsg, sizeof(IpcSecDevMbr_t));

	nErr = DI_FS_MountPartition(stDevMbr.eDevIdx, stDevMbr.ulPartitionIdx, stDevMbr.data.szDstMountDir, stDevMbr.eFsType);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	/* HDD는 mount할때 해당 마운트 위치를 settop 으로 변경한다. */
	{
		if (strstr(stDevMbr.data.szDstMountDir, "/mnt/hd") != NULL)
		{
			struct passwd *u_info;
			struct group  *g_info;

			u_info = getpwnam("settop");
			g_info = getgrnam("settop");

			if (u_info != NULL && g_info != NULL)
			{
				nErr = chown(stDevMbr.data.szDstMountDir, u_info->pw_uid, g_info->gr_gid);
			}
			else
			{
				nErr = -1;
			}
		}
	}

	VK_Print("\t ========> local_secma_handle_FsMountPartition : end\n");

	return 0;
}


static HINT32 local_secma_handle_FsUmountPartition(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	IpcSecDevMbr_t stDevMbr;

	VK_Print("\n\n\t ========> local_secma_handle_FsUmountPartition : start\n");

	VK_memset(&stDevMbr, 0, sizeof(IpcSecDevMbr_t));
	VK_memcpy(&stDevMbr, pvMsg, sizeof(IpcSecDevMbr_t));

	nErr = DI_FS_UnmountPartition(stDevMbr.eDevIdx, stDevMbr.ulPartitionIdx, stDevMbr.eFsType);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	VK_Print("\t ========> local_secma_handle_FsUmountPartition : end\n");

	return 0;
}


static HINT32 local_secma_handle_FsHddDstStart(void *pvMsg, HUINT32 ulSize)
{
	VK_Print("\n\n\t ========> local_secma_handle_FsHddDstStart : start\n");

	DI_HDD_DstStart();

	VK_Print("\t ========> local_secma_handle_FsHddDstStart : end\n");

	return 0;
}


static HINT32 local_secma_handle_FsOpenDevMbrBuf(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	IpcSecDevMbr_t stDevMbr;

	VK_Print("\n\n\t ========> local_secma_handle_FsOpenDevMbrBuf : start\n");

	VK_memset(&stDevMbr, 0, sizeof(IpcSecDevMbr_t));
	VK_memcpy(&stDevMbr, pvMsg, sizeof(IpcSecDevMbr_t));

	nErr = DI_FS_OpenDevMbrBuf(stDevMbr.eDevIdx);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	VK_Print("\t ========> local_secma_handle_FsOpenDevMbrBuf : end\n");

	return 0;
}


static HINT32 local_secma_handle_FsDeletePartitionInDevMbrBuf(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	IpcSecDevMbr_t stDevMbr;

	VK_Print("\n\n\t ========> local_secma_handle_FsDeletePartitionInDevMbrBuf : start\n");

	VK_memset(&stDevMbr, 0, sizeof(IpcSecDevMbr_t));
	VK_memcpy(&stDevMbr, pvMsg, sizeof(IpcSecDevMbr_t));

	nErr = DI_FS_DeletePartitionInDevMbrBuf(stDevMbr.eDevIdx, stDevMbr.ulPartitionIdx);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	VK_Print("\t ========> local_secma_handle_FsDeletePartitionInDevMbrBuf : end\n");

	return 0;
}


static HINT32 local_secma_handle_FsDeleteAllPartitionInDevMbrBuf(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	IpcSecDevMbr_t stDevMbr;

	VK_Print("\n\n\t ========> local_secma_handle_FsDeleteAllPartitionInDevMbrBuf : start\n");

	VK_memset(&stDevMbr, 0, sizeof(IpcSecDevMbr_t));
	VK_memcpy(&stDevMbr, pvMsg, sizeof(IpcSecDevMbr_t));

	nErr = DI_FS_DeleteAllPartitionInDevMbrBuf(stDevMbr.eDevIdx);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	VK_Print("\t ========> local_secma_handle_FsDeleteAllPartitionInDevMbrBuf : end\n");

	return 0;
}


static HINT32 local_secma_handle_FsUpdatePartitionInDevMbrBuf(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	IpcSecDevMbr_t stDevMbr;

	VK_Print("\n\n\t ========> local_secma_handle_FsUpdatePartitionInDevMbrBuf : start\n");

	VK_memset(&stDevMbr, 0, sizeof(IpcSecDevMbr_t));
	VK_memcpy(&stDevMbr, pvMsg, sizeof(IpcSecDevMbr_t));

	nErr = DI_FS_UpdatePartitionInDevMbrBuf(stDevMbr.eDevIdx, stDevMbr.ulPartitionIdx, stDevMbr.ulStartUnit, stDevMbr.ulEndUnit, stDevMbr.eFsType);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	VK_Print("\t ========> local_secma_handle_FsUpdatePartitionInDevMbrBuf : end\n");

	return 0;
}


static HINT32 local_secma_handle_FsSyncDevMbrBuf(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	IpcSecDevMbr_t stDevMbr;

	VK_Print("\n\n\t ========> local_secma_handle_FsSyncDevMbrBuf : start\n");

	VK_memset(&stDevMbr, 0, sizeof(IpcSecDevMbr_t));
	VK_memcpy(&stDevMbr, pvMsg, sizeof(IpcSecDevMbr_t));

	nErr = DI_FS_SyncDevMbrBuf(stDevMbr.eDevIdx);
	if (nErr != DI_ERR_OK)
	{
		VK_Print("\t ========> local_secma_handle_FsSyncDevMbrBuf : filed end\n");
		return -1;
	}

	VK_Print("\t ========> local_secma_handle_FsSyncDevMbrBuf : end\n");

	return 0;
}


static HINT32 local_secma_handle_FsCloseDevMbrBuf(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	IpcSecDevMbr_t stDevMbr;

	VK_Print("\n\n\t ========> local_secma_handle_FsCloseDevMbrBuf : start\n");

	VK_memset(&stDevMbr, 0, sizeof(IpcSecDevMbr_t));
	VK_memcpy(&stDevMbr, pvMsg, sizeof(IpcSecDevMbr_t));

	nErr = DI_FS_CloseDevMbrBuf(stDevMbr.eDevIdx);
	if (nErr != DI_ERR_OK)
	{
		VK_Print("\t ========> local_secma_handle_FsCloseDevMbrBuf : filed end\n");
		return -1;
	}

	VK_Print("\t ========> local_secma_handle_FsCloseDevMbrBuf : end\n");

	return 0;
}


static HINT32 local_secma_handle_FsGetPartitionFromDeviceMbr(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	IpcSecPartitionList_t stPartitionList;

	VK_Print("\n\n\t ========> local_secma_handle_FsGetPartitionFromDeviceMbr : start\n");

	VK_memset(&stPartitionList, 0, sizeof(IpcSecPartitionList_t));
	VK_memcpy(&stPartitionList, pvMsg, sizeof(IpcSecPartitionList_t));

	nErr = DI_FS_GetPartitionFromDeviceMbr(stPartitionList.eDevIdx, &stPartitionList.stPartitionList);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	VK_memcpy(pvMsg, &stPartitionList, sizeof(IpcSecPartitionList_t));

	VK_Print("\t ========> local_secma_handle_FsGetPartitionFromDeviceMbr : end\n");

	return 0;
}


static HINT32 local_secma_handle_FsEraseAllPartitionInDeviceMbr(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	IpcSecPartitionList_t stPartitionList;

	VK_Print("\n\n\t ========> local_secma_handle_FsEraseAllPartitionInDeviceMbr : start\n");

	VK_memset(&stPartitionList, 0, sizeof(IpcSecPartitionList_t));
	VK_memcpy(&stPartitionList, pvMsg, sizeof(IpcSecPartitionList_t));

	nErr = DI_FS_EraseAllPartitionInDeviceMbr(stPartitionList.eDevIdx);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	VK_Print("\t ========> local_secma_handle_FsEraseAllPartitionInDeviceMbr : end\n");

	return 0;
}


static HINT32 local_secma_handle_FsInitFormatList(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;

	VK_Print("\n\n\t ========> local_secma_handle_FsInitFormatList : start\n");

	nErr = DI_FS_InitFormatList(&s_FsFormatList_);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	VK_Print("\t ========> local_secma_handle_FsInitFormatList : end\n");

	return 0;
}


static HINT32 local_secma_handle_FsAddFormatItemToList(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	IpcSecFormatList_t stFormatList;

	VK_Print("\n\n\t ========> local_secma_handle_FsAddFormatItemToList : start\n");

	VK_memset(&stFormatList, 0, sizeof(IpcSecFormatList_t));
	VK_memcpy(&stFormatList, pvMsg, sizeof(IpcSecFormatList_t));

	nErr = DI_FS_AddFormatItemToList(&s_FsFormatList_, &stFormatList.stFormatItem);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	VK_Print("\t ========> local_secma_handle_FsAddFormatItemToList : end\n");

	return 0;
}


static HINT32 local_secma_handle_FsClearFormatList(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;

	VK_Print("\n\n\t ========> local_secma_handle_FsClearFormatList : enter\n");

	nErr = DI_FS_ClearFormatList(&s_FsFormatList_);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	VK_Print("\t ========> local_secma_handle_FsClearFormatList : end\n");

	return 0;
}


static HINT32 local_secma_handle_FsFormatLists(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	IpcSecFormatList_t stFormatList;

	VK_Print("\n\n\t ========> local_secma_handle_FsFormatLists : enter\n");

	VK_memset(&stFormatList, 0, sizeof(IpcSecFormatList_t));
	VK_memcpy(&stFormatList, pvMsg, sizeof(IpcSecFormatList_t));

	nErr = DI_FS_FormatLists(&s_FsFormatList_, stFormatList.nonblock);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	VK_Print("\t ========> local_secma_handle_FsFormatLists : end\n");

	return 0;
}


static HINT32 local_secma_handle_FsGetMountedPartitionStatus(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	IpcSecMountdInfo_t stMountedInfo;

	VK_memset(&stMountedInfo, 0, sizeof(IpcSecMountdInfo_t));
	VK_memcpy(&stMountedInfo, pvMsg, sizeof(IpcSecMountdInfo_t));

	nErr = DI_FS_GetMountedPartitionStatus(stMountedInfo.eDevIdx, stMountedInfo.ulPartIdx, stMountedInfo.eFsType,
											&stMountedInfo.ullTotalKbSize, &stMountedInfo.ullUsedKbSize, &stMountedInfo.bWritable);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	VK_memcpy(pvMsg, &stMountedInfo, sizeof(IpcSecMountdInfo_t));

	return 0;
}
#endif


#if (1)
static HINT32 local_secma_handle_EncrFormat(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	IpcSecDevMbr_t stDevMbr;

	VK_memset(&stDevMbr, 0, sizeof(IpcSecDevMbr_t));
	VK_memcpy(&stDevMbr, pvMsg, sizeof(IpcSecDevMbr_t));

	nErr = DI_FS_ENCR_Format(stDevMbr.eDevIdx, stDevMbr.ulPartitionIdx, stDevMbr.szPassword);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	return 0;
}


static HINT32 local_secma_handle_EncrOpen(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	IpcSecDevMbr_t stDevMbr;

	VK_memset(&stDevMbr, 0, sizeof(IpcSecDevMbr_t));
	VK_memcpy(&stDevMbr, pvMsg, sizeof(IpcSecDevMbr_t));

	nErr = DI_FS_ENCR_Open(stDevMbr.eDevIdx, stDevMbr.ulPartitionIdx, stDevMbr.data.szMapper, stDevMbr.szPassword);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	return 0;
}


static HINT32 local_secma_handle_EncrClose(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	IpcSecDevMbr_t stDevMbr;

	VK_memset(&stDevMbr, 0, sizeof(IpcSecDevMbr_t));
	VK_memcpy(&stDevMbr, pvMsg, sizeof(IpcSecDevMbr_t));

	nErr = DI_FS_ENCR_Close(stDevMbr.eDevIdx, stDevMbr.ulPartitionIdx);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	return 0;
}


static HINT32 local_secma_handle_EncrMakeFs(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	IpcSecDevMbr_t stDevMbr;

	VK_memset(&stDevMbr, 0, sizeof(IpcSecDevMbr_t));
	VK_memcpy(&stDevMbr, pvMsg, sizeof(IpcSecDevMbr_t));

	nErr = DI_FS_ENCR_MakeFS(stDevMbr.eDevIdx, stDevMbr.ulPartitionIdx, stDevMbr.data.szVolumn, stDevMbr.eFsType);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	return 0;
}


static HINT32 local_secma_handle_EncrMount(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	IpcSecDevMbr_t stDevMbr;

	VK_memset(&stDevMbr, 0, sizeof(IpcSecDevMbr_t));
	VK_memcpy(&stDevMbr, pvMsg, sizeof(IpcSecDevMbr_t));

	nErr = DI_FS_ENCR_Mount(stDevMbr.eDevIdx, stDevMbr.ulPartitionIdx, stDevMbr.data.szDstMountDir, stDevMbr.eFsType);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	return 0;
}


static HINT32 local_secma_handle_EncrUmount(void *pvMsg, HUINT32 ulSize)
{
	DI_ERR_CODE nErr;
	IpcSecDevMbr_t stDevMbr;

	VK_memset(&stDevMbr, 0, sizeof(IpcSecDevMbr_t));
	VK_memcpy(&stDevMbr, pvMsg, sizeof(IpcSecDevMbr_t));

	nErr = DI_FS_ENCR_Umount(stDevMbr.eDevIdx, stDevMbr.ulPartitionIdx);
	if (nErr != DI_ERR_OK)
	{
		return -1;
	}

	return 0;
}
#endif

static HINT32 local_secma_handle_HidCallback(DI_USB_HID_DEVICE eDevice, DI_USB_HID_DATA tData)
{
	DI_ERR_CODE nErr;
	IpcSecHidCb_t stCb;

	stCb.etDevice = eDevice;
	VK_memcpy(&stCb.etData, &tData, sizeof(DI_USB_HID_DATA));

	if (VK_IPC_MSG_Call(s_ulMsgQId[1], IPC_SEC_CONV_FN_HidCb, &nErr, (void*)&stCb, sizeof(IpcSecHidCb_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	return nErr;
}


static HINT32 local_secma_handle_HddCallback(DI_HDD_Event_t etEventType, void *pucHddInfo)
{
	HERROR nErr;
	IpcSecHddCb_t stCb;
	hdd_info_t *stHddInfo = (hdd_info_t *)pucHddInfo;

	if (pucHddInfo == NULL)
	{
		VK_Print("[%s][%d], invalid parameter\n", __FUNCTION__, __LINE__);
		return -1;
	}

	stCb.etEventType = etEventType;
	stCb.etHddInfo.devCount = stHddInfo->devCount;
	stCb.etHddInfo.portnum = stHddInfo->portnum;
	memcpy(stCb.etHddInfo.devName, stHddInfo->devName, 3);

	if (VK_IPC_MSG_Call(s_ulMsgQId[1], IPC_SEC_CONV_FN_HddCb, &nErr, (void*)&stCb, sizeof(IpcSecHddCb_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	return nErr;
}


static HINT32 local_secma_handle_UsbCallback(DI_USB_Event_t etEventType, void *pucUsbInfo)
{
	HERROR nErr;
	IpcSecUsbCb_t stCb;
	usb_info_t *stUsbInfo = (usb_info_t *)pucUsbInfo;

	if (pucUsbInfo == NULL)
	{
		VK_Print("[%s][%d], invalid parameter\n", __FUNCTION__, __LINE__);
		return -1;
	}

	if (stUsbInfo->devCount != 1 || stUsbInfo->devNameSize != 4)
	{
		VK_Print("[%s][%d], invalid parameter\n", __FUNCTION__, __LINE__);
		return -1;
	}

	stCb.etEventType = etEventType;
	stCb.devCount = stUsbInfo->devCount;
	stCb.devNameSize = stUsbInfo->devNameSize;
	stCb.ePortNum = stUsbInfo->ePortNum;
	stCb.eSpeed = stUsbInfo->eSpeed;
	stCb.isOverCurrent = stUsbInfo->isOverCurrent;
	memcpy(stCb.devName, stUsbInfo->devName[stCb.devCount - 1], stCb.devNameSize);

	if (VK_IPC_MSG_Call(s_ulMsgQId[1], IPC_SEC_CONV_FN_UsbCb, &nErr, (void*)&stCb, sizeof(IpcSecUsbCb_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	return nErr;
}


static HINT32 local_secma_handle_WlanCallback(DI_USB_Wireless_Event_t etEventType, void *pucWlanInfo)
{
	HERROR nErr;
	IpcSecWlanCb_t stCb;
	usb_wireless_info_t *stWlanInfo = (usb_wireless_info_t *)pucWlanInfo;

	if (pucWlanInfo == NULL)
	{
		VK_Print("[%s][%d], invalid parameter\n", __FUNCTION__, __LINE__);
		return -1;
	}

	stCb.etEventType = etEventType;
	stCb.etWlanInfo.devCount = stWlanInfo->devCount;
	stCb.etWlanInfo.ePortNum = stWlanInfo->ePortNum;
	strncpy(stCb.etWlanInfo.devName, stWlanInfo->devName, DEV_MAX_NAME);

	if (VK_IPC_MSG_Call(s_ulMsgQId[1], IPC_SEC_CONV_FN_WlanCb, &nErr, (void*)&stCb, sizeof(IpcSecWlanCb_t)) != VK_OK)
	{
		return ERR_FAIL;
	}

	return nErr;
}


#if (1)
static void local_secma_handle_DstCallback(HDD_DST_RESULT dstResult, HINT32 nProgress)
{
	HERROR nErr;
	IpcSecHddDstEvtInfo_t stHddDstCb;

	stHddDstCb.dstResult = dstResult;
	stHddDstCb.nProgress = nProgress;

	if (VK_IPC_MSG_Call(s_ulMsgQId[1], IPC_SEC_CONV_FN_HddDstCb, &nErr, (void*)&stHddDstCb, sizeof(IpcSecHddDstEvtInfo_t)) != VK_OK)
	{
		VK_Print("error in local_secma_handle_DstCallback\n");
	}

	return;
}


static void local_secma_handle_FormatCallback(HUINT32 ulEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	HERROR nErr;
	IpcSecHddFormatEvtInfo_t stHddFormatCb;

	stHddFormatCb.ulEvent = ulEvent;
	stHddFormatCb.ulParam1 = ulParam1;
	stHddFormatCb.ulParam2 = ulParam2;
	stHddFormatCb.ulParam3 = ulParam3;

	if (VK_IPC_MSG_Call(s_ulMsgQId[1], IPC_SEC_CONV_FN_HddFormatCb, &nErr, (void*)&stHddFormatCb, sizeof(IpcSecHddFormatEvtInfo_t)) != VK_OK)
	{
		VK_Print("error in local_secma_handle_FormatCallback\n");
	}

	return;

}
#endif


/**************************************************************************************************/
#define __________Sever_Sec_Handler_Public_Functions_Body___________________________________
/**************************************************************************************************/
HINT32 SECMA_HANDLE_Init(IpcSecMsgType eMsgType)
{
	IpcSecManager_t stSecMgr;

	if (local_secma_handle_findMsgByType(eMsgType, &stSecMgr) != ERR_OK)
	{
		VK_Print("error in local_secma_handle_findMsgByType....\n");
		return ERR_FAIL;
	}

	if (eMsgType == IPC_COMM_TYPE_OCTO)
	{
		SECMA_HANDLE_DiInit();
	}

	// client (target -> secure_handler)
	if (VK_IPC_MSG_Open(0, IPC_MSG_Q_BUFF_MAX, stSecMgr.ucPath[0], &s_ulMsgQId[0]) != VK_OK)
	{
		VK_Print("error in VK_IPC_MSG_Open....\n");
		return ERR_FAIL;
	}

	// server(secure handler -> target)
	if (VK_IPC_MSG_Open(0, IPC_MSG_Q_BUFF_MAX, stSecMgr.ucPath[1], &s_ulMsgQId[1]) != VK_OK)
	{
		VK_Print("error in VK_IPC_MSG_Open....\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}


HINT32 SECMA_HANDLE_InitRegisterFunctions(IpcSecMsgType eMsgType)
{
	HERROR ulErr = ERR_OK;

	if (eMsgType == IPC_COMM_TYPE_OCTO)
	{
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_Mount, local_secma_handle_Mount);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_Umount, local_secma_handle_Umount);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_Stime, local_secma_handle_Stime);
#if (0)
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_StimeSetTime, local_secma_handle_StimeSetTime);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_StimeSetMode, local_secma_handle_StimeSetMode);
#endif
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_StimeGetMode, local_secma_handle_StimeGetMode);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_Settimeofday, local_secma_handle_Settimeofday);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_Chmod, local_secma_handle_Chmod);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_Chown, local_secma_handle_Chown);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_Remove, local_secma_handle_Remove);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_Mkdir, local_secma_handle_Mkdir);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_Mknod, local_secma_handle_Mknod);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_SysCmd, local_secma_handle_SysCmd);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_Ioctl, local_secma_handle_Ioctl);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_Stat, local_secma_handle_Stat);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_Lstat, local_secma_handle_Lstat);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_Socket, local_secma_handle_Socket);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_Select, local_secma_handle_Select);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_Send, local_secma_handle_Send);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_Recv, local_secma_handle_Recv);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_Open, local_secma_handle_Open);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_Open2, local_secma_handle_Open2);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_Close, local_secma_handle_Close);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_Lseek, local_secma_handle_Lseek);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_Read, local_secma_handle_Read);
	       ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_Write, local_secma_handle_Write);
	       ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_Rename, local_secma_handle_Rename);
	       ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_Vfork, local_secma_handle_Vfork);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_HddPwrCtrl, local_secma_handle_HddPwrCtrl);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_Hdd2PwrCtrl, local_secma_handle_Hdd2PwrCtrl);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_UsbPwrCtrl, local_secma_handle_UsbPwrCtrl);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_UsbHostConPwrCtrl, local_secma_handle_UsbHostConPwrCtrl);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_UsbStop, local_secma_handle_UsbStop);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_HidCbNoti, local_secma_handle_HidCbNoti);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_HddCbNoti, local_secma_handle_HddCbNoti);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_UsbCbNoti, local_secma_handle_UsbCbNoti);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_WlanCbNoti, local_secma_handle_WlanCbNoti);

#if (1)
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_HddDstCbNoti, local_secma_handle_HddDstCbNoti);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_HddFormatCbNoti, local_secma_handle_HddFormatCbNoti);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_GetDevFileName, local_secma_handle_FsGetDeviceFileName);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_GetDevFileFullName, local_secma_handle_FsGetDeviceFileFullName);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_GetPartitionFileName, local_secma_handle_FsGetPartitionFileName);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_GetPartitionFileFullName, local_secma_handle_FsGetPartitionFullFileName);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_IsFileExist, local_secma_handle_FsIsFileExist);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_IsHddExist, local_secma_handle_FsIsHDDExist);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_IsDirExist, local_secma_handle_FsIsDirExist);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_DiMkDir, local_secma_handle_FsMkdir);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_GetVolumeUuid, local_secma_handle_FsGetVolumeUuid);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_GetVolumeNtfsUuid, local_secma_handle_FsGetVolumeNtfsUuid);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_MountPartition, local_secma_handle_FsMountPartition);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_UmountPartition, local_secma_handle_FsUmountPartition);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_HddDstStart, local_secma_handle_FsHddDstStart);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_OpenDevMbrBuf, local_secma_handle_FsOpenDevMbrBuf);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_DeletePartitionInDevMbrBuf, local_secma_handle_FsDeletePartitionInDevMbrBuf);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_DeleteAllPartitionInDevMbrBuf, local_secma_handle_FsDeleteAllPartitionInDevMbrBuf);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_UpdatePartitionInDevMbrBuf,  local_secma_handle_FsUpdatePartitionInDevMbrBuf);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_SyncDevMbrBuf, local_secma_handle_FsSyncDevMbrBuf);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_CloseDevMbrBuf, local_secma_handle_FsCloseDevMbrBuf);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_GetPartitionFromDeviceMbr, local_secma_handle_FsGetPartitionFromDeviceMbr);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_EraseAllPartitionInDeviceMbr, local_secma_handle_FsEraseAllPartitionInDeviceMbr);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_InitFormatList,  local_secma_handle_FsInitFormatList);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_AddFormatItemToList,  local_secma_handle_FsAddFormatItemToList);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_ClearFormatList,  local_secma_handle_FsClearFormatList);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_FormatLists,  local_secma_handle_FsFormatLists);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_GetMountedPartitionStatus,  local_secma_handle_FsGetMountedPartitionStatus);
#endif

#if (1)
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_FncrFormat, local_secma_handle_EncrFormat);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_FncrOpen, local_secma_handle_EncrOpen);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_FncrClose, local_secma_handle_EncrClose);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_FncrMakeFs, local_secma_handle_EncrMakeFs);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_FncrMount, local_secma_handle_EncrMount);
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_FncrUmount, local_secma_handle_EncrUmount);
#endif

	}
	else if (eMsgType == IPC_COMM_TYPE_HAMA)
	{
		ulErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId[0], IPC_SEC_CONV_FN_Vfork, local_secma_handle_Vfork);
	}
	else
	{
		VK_Print("[%d, %s] error in undefined MsgType (0x%x)\n", __LINE__, __FUNCTION__, eMsgType);
	}

	return ulErr;
}


HINT32 SECMA_HANDLE_FindMsgByType(IpcSecMsgType eMsgType, IpcSecManager_t *pstSecMgr)
{
	return local_secma_handle_findMsgByType(eMsgType, pstSecMgr);
}


HINT32 SECMA_HANDLE_FindMsgByTarget(HUINT8 *pucTarget, IpcSecManager_t *pstSecMgr)
{
	return local_secma_handle_findMsgByTarget(pucTarget, pstSecMgr);
}



