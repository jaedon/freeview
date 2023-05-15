/*******************************************************************
	File Description
********************************************************************/
/**
	@file	   \n
	@brief	   \n

	Description:   \n
	Module: \n

	Copyright (c) 2012 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

#ifndef	__MAIN_SECURE_HANDLER_H__
#define	__MAIN_SECURE_HANDLER_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <htype.h>
#include <vkernel.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "di_hdd.h"
#include "di_err.h"
#include "di_usb.h"
#include "di_usb_wireless.h"
#include "di_usb_hid.h"
#include "di_fs.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define IPC_SYS_MSGQ_MAX_MSG			(2)	// HAMA, OCTO 현재 2개

#define IPC_SYS_MSGQ_PATH_SECMA		"/tmp/.IPC_SYS_MSGQ_PATH_SECMA"		/* MAIN SECURE HANDLER */

#define IPC_SYS_MSGQ_PATH_TO_OCTO	"/tmp/.IPC_SYS_MSGQ_PATH_TO_OCTO"		/* SECURE HANDLER -> OCTO */
#define IPC_SYS_MSGQ_PATH_FR_OCTO	"/tmp/.IPC_SYS_MSGQ_PATH_FR_OCTO"		/* OCTO -> SECURE HANDLER */

#define IPC_SYS_MSGQ_PATH_TO_HAMA	"/tmp/.IPC_SYS_MSGQ_PATH_TO_SECMA"		/* SECURE HANDLER -> HAMA */
#define IPC_SYS_MSGQ_PATH_FR_HAMA	"/tmp/.IPC_SYS_MSGQ_PATH_FR_SECMA"		/* HAMA -> SECURE HANDLER */

#define IPC_SYS_MSGQ_PATH_TO_DAMA	"/tmp/.IPC_SYS_MSGQ_PATH_TO_DAMA"		/* SECURE HANDLER -> DAMA */
#define IPC_SYS_MSGQ_PATH_FR_DAMA	"/tmp/.IPC_SYS_MSGQ_PATH_FR_DAMA"		/* DAMA -> SECURE HANDLER */


#define IPC_MSG_Q_BUFF_MAX			(1024-4*2)
#define IPC_TEMPFILE_MAX_SIZE			(1024*2)

#define HOTPLUG_TASK_PRIORITY 			(VK_TASK_PRIORITY_MW_REF + 2)
#define HOTPLUG_TASK_STACK_SIZE		(1024 * 2)

#define IPC_MOUNT_MAX_SIZE			(100)
#define IPC_PATH_MAX_SIZE				(255)

#define IPC_SYS_CMD_ARGC_MAX_SIZE	(10)
#define IPC_SYS_CMD_ARGV_MAX_SIZE	(100)


#if !defined(SMART_CMD)
#define SMART_CMD						(unsigned int)(0x031F)
#endif
#if !defined(SMART_BLKGETSIZE64)
#define SMART_BLKGETSIZE64				_IOR(0x12,114,size_t)
#endif
#if !defined(BLKSSZGET)
#define BLKSSZGET						_IO(0x12,104)
#endif
#if !defined(HDIO_GETGEO)
#define HDIO_GETGEO					(unsigned int)(0x0301)
#endif
#if !defined(WLAN_PRIVIOCTL_SET)
#define WLAN_PRIVIOCTL_SET				(unsigned int)(0x8be2)
#endif
#if !defined(WLAN_PRIVIOCTL_CONNSTAT)
#define WLAN_PRIVIOCTL_CONNSTAT		(unsigned int)(0x0004)
#endif
#if !defined(WLAN_PRIVIOCTL_DRIVERVER)
#define WLAN_PRIVIOCTL_DRIVERVER		(unsigned int)(0x0005)
#endif
#if !defined(WLAN_PRIVIOCTL_BAINFO)
#define WLAN_PRIVIOCTL_BAINFO			(unsigned int)(0x0006)
#endif
#if !defined(WLAN_PRIVIOCTL_DESCINFO)
#define WLAN_PRIVIOCTL_DESCINFO		(unsigned int)(0x0007)
#endif
#if !defined(WLAN_PRVIIOCTL_RADIO_OFF)
#define WLAN_PRVIIOCTL_RADIO_OFF		(unsigned int)(0x000a)
#endif
#if !defined(WLAN_PRIVIOCTL_RADIO_ON)
#define WLAN_PRIVIOCTL_RADIO_ON		(unsigned int)(0x000b)
#endif
#if !defined(WLAN_PRIVIOCTL_SHOW)
#define WLAN_PRIVIOCTL_SHOW			(unsigned int)(0x0014)
#endif
#if !defined(WLAN_PRIVIOCTL_ADHOCENTRY)
#define WLAN_PRIVIOCTL_ADHOCENTRY	(unsigned int)(0x0015)
#endif
#if !defined(WLAN_PRIVIOCTL_STAT)
#define WLAN_PRIVIOCTL_STAT			(unsigned int)(0x8be9)
#endif
#if !defined(WLAN_PRIVIOCTL_GET_SITE_SURVEY)
#define WLAN_PRIVIOCTL_GET_SITE_SURVEY	(unsigned int)(0x8bed)
#endif


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	IPC_COMM_TYPE_OCTO = 0,
	IPC_COMM_TYPE_HAMA,
	IPC_COMM_TYPE_DAMA,
	IPC_COMM_TYPE_MAX
} IpcSecMsgType;

typedef struct
{
	IpcSecMsgType 	eMsgType;
	HUINT8			ucPath[2][IPC_MOUNT_MAX_SIZE];
	HUINT8			ucTarget[IPC_MOUNT_MAX_SIZE];
} IpcSecManager_t;

typedef struct
{
	DI_FS_StorDevIdx_t 	eDevIdx;
	HUINT32 				ulPartIdx;
	HUINT8 				szDstMountDir[IPC_MOUNT_MAX_SIZE];
	DI_FileSystemType_t 	eFsType;
} IpcSecMount_t;

typedef struct
{
	VK_CLOCK_SET_MODE eSetMode;
	unsigned long ulUnixTime;
} IpcSecSetTime_t;

typedef struct
{
	HINT32 	nHour;
	HINT32 	nMin;
} IpcSecSettimeofday_t;

typedef struct
{
	HUINT8	ucArgc;
	HUINT8 	ucArgv[IPC_SYS_CMD_ARGC_MAX_SIZE][IPC_SYS_CMD_ARGV_MAX_SIZE];
} IpcSecSysCmd_t;

typedef struct
{
	HUINT8	ucPath[IPC_PATH_MAX_SIZE];
	HUINT8 	ucNewPath[IPC_PATH_MAX_SIZE];
	HUINT32	ulMode;
	HUINT32 ulDev;
} IpcSecFileCtl_t;

typedef struct
{
	HUINT8	ucPath[IPC_PATH_MAX_SIZE];
	struct stat stStat;
} IpcSecStat_t;

typedef struct
{
	HINT32 nDomain;
	HINT32 nType;
	HINT32 nProtocol;
} IpcSecSocket_t;

typedef struct
{
	HINT32 nFd;
	fd_set stReadFds;
	fd_set stWriteFds;
	fd_set stExceptFds;
	struct timeval stTimeout;
} IpcSecSelect_t;

typedef struct
{
	HUINT8 ucPath[IPC_PATH_MAX_SIZE];
	HINT32 nFlags;
	mode_t mode;
} IpcSecFileOpen_t;

typedef struct
{
	HINT32	fd;
	off_t		pos;
	HINT32	origin;
} IpcSecLseek_t;

typedef struct
{
	HINT32	fd;
	HUINT32	ulSize;
	HUINT32 ulFlag;
} IpcSecRead_t;

typedef struct
{
	HINT32	nPid;
	HUINT8	ucProcName[IPC_PATH_MAX_SIZE];
} IpcSecVfork_t;

typedef struct
{
	DI_USB_Event_t etEventType;
	unsigned long devCount;
	unsigned long devNameSize;
	char devName[4];
	DI_USB_PORT_Num_t ePortNum;
	DI_USB_SPEED_t	eSpeed;
	unsigned char isOverCurrent;
} IpcSecUsbCb_t;

typedef struct
{
	DI_HDD_Event_t etEventType;
	hdd_info_t	   etHddInfo ;
} IpcSecHddCb_t;

typedef struct
{
	DI_USB_Wireless_Event_t etEventType;
	usb_wireless_info_t	   etWlanInfo ;
} IpcSecWlanCb_t;

typedef struct
{
	DI_USB_HID_DEVICE etDevice;
	DI_USB_HID_DATA	etData ;
} IpcSecHidCb_t;


#if (1)
typedef struct
{
	DI_FS_StorDevIdx_t eDevIdx;
	HUINT8 szFileName[IPC_PATH_MAX_SIZE];
} IpcSecDevName_t;

typedef struct
{
	DI_FS_StorDevIdx_t eDevIdx;
	HUINT32 ulPartitionIdx;
	HUINT8 szFileName[IPC_PATH_MAX_SIZE];
	HUINT32 nSizeOfFileName;
	HUINT32 nUuIdSize;
} IpcSecPartitionName_t;

typedef struct
{
	DI_FS_StorDevIdx_t eDevIdx;
	HUINT32 ulPartitionIdx;
	union
	{
		HUINT8 szDstMountDir[IPC_PATH_MAX_SIZE];
		HUINT8 szMapper[IPC_PATH_MAX_SIZE];
		HUINT8 szVolumn[IPC_PATH_MAX_SIZE];
	} data;
	HUINT8 szPassword[IPC_PATH_MAX_SIZE];
	DI_FileSystemType_t eFsType;
	HUINT32 ulStartUnit;
	HUINT32 ulEndUnit;
} IpcSecDevMbr_t;

typedef struct
{
	DI_FS_StorDevIdx_t eDevIdx;
	DI_FS_PartitionList_t stPartitionList;
} IpcSecPartitionList_t;

typedef struct
{
	HUINT32 nonblock;
	DI_FS_FormatItem_t stFormatItem;
} IpcSecFormatList_t;

typedef struct
{
	DI_FS_StorDevIdx_t eDevIdx;
	HUINT32 ulPartIdx;
	DI_FileSystemType_t eFsType;
	HUINT64 ullTotalKbSize;
	HUINT64 ullUsedKbSize;
	HBOOL bWritable;
} IpcSecMountdInfo_t;

typedef struct
{
	HDD_DST_RESULT dstResult;
	HINT32 nProgress;
} IpcSecHddDstEvtInfo_t;

typedef struct
{
	HUINT32 ulEvent;
	HUINT32 ulParam1;
	HUINT32 ulParam2;
	HUINT32 ulParam3;
} IpcSecHddFormatEvtInfo_t;
#endif

typedef enum
{
	IPC_SEC_MOUNT_NO_ERR = 0,
	IPC_SEC_MOUNT_EROFS,
	IPC_SEC_MOUNT_EINVAL,
	IPC_SEC_MOUNT_UNKONWN
} IpcSecMountErr_e;

typedef enum
{
	IPC_SEC_SYS_CMD_NO_ERR = 0,
	IPC_SEC_SYS_CMD_VFORK_ERR,
	IPC_SEC_SYS_CMD_EXCELP_ERR,
	IPC_SEC_SYS_CMD_EXCELP_RET_ERR,
} IpcSecSysCmdErr_e;

typedef enum
{
	IPC_SEC_CONV_FN_START = 0,

	IPC_SEC_CONV_FN_InitSec,
	IPC_SEC_CONV_FN_Mount,
	IPC_SEC_CONV_FN_Umount,
	IPC_SEC_CONV_FN_Stime,
	IPC_SEC_CONV_FN_StimeSetTime,
	IPC_SEC_CONV_FN_StimeSetMode,
	IPC_SEC_CONV_FN_StimeGetMode,
	IPC_SEC_CONV_FN_Settimeofday,
	IPC_SEC_CONV_FN_Chmod,
	IPC_SEC_CONV_FN_Chown,
	IPC_SEC_CONV_FN_Remove,
	IPC_SEC_CONV_FN_Mkdir,
	IPC_SEC_CONV_FN_Mknod,
	IPC_SEC_CONV_FN_SysCmd,
	IPC_SEC_CONV_FN_Ioctl,
	IPC_SEC_CONV_FN_Stat,
	IPC_SEC_CONV_FN_Lstat,
	IPC_SEC_CONV_FN_Socket,
	IPC_SEC_CONV_FN_Select,
	IPC_SEC_CONV_FN_Send,
	IPC_SEC_CONV_FN_Recv,
	IPC_SEC_CONV_FN_Open,
	IPC_SEC_CONV_FN_Open2,
	IPC_SEC_CONV_FN_Close,
	IPC_SEC_CONV_FN_Lseek,
	IPC_SEC_CONV_FN_Read,
	IPC_SEC_CONV_FN_Write,
	IPC_SEC_CONV_FN_Rename,
	IPC_SEC_CONV_FN_Vfork,
	IPC_SEC_CONV_FN_HddPwrCtrl,
	IPC_SEC_CONV_FN_Hdd2PwrCtrl,
	IPC_SEC_CONV_FN_UsbPwrCtrl,
	IPC_SEC_CONV_FN_UsbHostConPwrCtrl,
	IPC_SEC_CONV_FN_UsbStop,
	IPC_SEC_CONV_FN_HidCbNoti,
	IPC_SEC_CONV_FN_HddCbNoti,
	IPC_SEC_CONV_FN_UsbCbNoti,
	IPC_SEC_CONV_FN_WlanCbNoti,

#if (1)
	IPC_SEC_CONV_FN_HddDstCbNoti,
	IPC_SEC_CONV_FN_HddFormatCbNoti,
	IPC_SEC_CONV_FN_GetDevFileName,
	IPC_SEC_CONV_FN_GetDevFileFullName,
	IPC_SEC_CONV_FN_GetPartitionFileName,
	IPC_SEC_CONV_FN_GetPartitionFileFullName,
	IPC_SEC_CONV_FN_IsFileExist,
	IPC_SEC_CONV_FN_IsHddExist,
	IPC_SEC_CONV_FN_IsDirExist,
	IPC_SEC_CONV_FN_DiMkDir,
	IPC_SEC_CONV_FN_GetVolumeUuid,
	IPC_SEC_CONV_FN_GetVolumeNtfsUuid,
	IPC_SEC_CONV_FN_MountPartition,
	IPC_SEC_CONV_FN_UmountPartition,
	IPC_SEC_CONV_FN_HddDstStart,
	IPC_SEC_CONV_FN_OpenDevMbrBuf,
	IPC_SEC_CONV_FN_DeletePartitionInDevMbrBuf,
	IPC_SEC_CONV_FN_DeleteAllPartitionInDevMbrBuf,
	IPC_SEC_CONV_FN_UpdatePartitionInDevMbrBuf,
	IPC_SEC_CONV_FN_SyncDevMbrBuf,
	IPC_SEC_CONV_FN_CloseDevMbrBuf,
	IPC_SEC_CONV_FN_GetPartitionFromDeviceMbr,
	IPC_SEC_CONV_FN_EraseAllPartitionInDeviceMbr,
	IPC_SEC_CONV_FN_InitFormatList,
	IPC_SEC_CONV_FN_AddFormatItemToList,
	IPC_SEC_CONV_FN_ClearFormatList,
	IPC_SEC_CONV_FN_FormatLists,
	IPC_SEC_CONV_FN_GetMountedPartitionStatus,
#endif

#if (1)
	IPC_SEC_CONV_FN_FncrFormat,
	IPC_SEC_CONV_FN_FncrOpen,
	IPC_SEC_CONV_FN_FncrClose,
	IPC_SEC_CONV_FN_FncrMakeFs,
	IPC_SEC_CONV_FN_FncrMount,
	IPC_SEC_CONV_FN_FncrUmount,
#endif

	IPC_SEC_CONV_FN_HddCb,
	IPC_SEC_CONV_FN_UsbCb,
	IPC_SEC_CONV_FN_WlanCb,
	IPC_SEC_CONV_FN_HidCb,

#if (1)
	IPC_SEC_CONV_FN_HddDstCb,
	IPC_SEC_CONV_FN_HddFormatCb,
#endif
	IPC_SEC_CONV_FN_MAX
} IpcSecConvFuncHandle_e;


/* 여기에 추가되는 process를 관리하자 */
static IpcSecManager_t s_stMsgMgr[IPC_SYS_MSGQ_MAX_MSG] =
{
	{ 	IPC_COMM_TYPE_OCTO,
   		{ IPC_SYS_MSGQ_PATH_TO_OCTO, IPC_SYS_MSGQ_PATH_FR_OCTO },
	  	 "octo"
	},
	{ 	IPC_COMM_TYPE_HAMA,
   		{ IPC_SYS_MSGQ_PATH_TO_HAMA, IPC_SYS_MSGQ_PATH_FR_HAMA },
	  	 "hama"
	}
};


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

#endif /* !__MAIN_SECURE_HANDLER_H__ */


