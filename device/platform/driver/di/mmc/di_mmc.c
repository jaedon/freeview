/*
** di_mmc.c
**

**
** @defgroup
** @author
** @note
** @brief
** @file
*/

/*----------------------------------------------------------------------------------
** Headers
**--------------------------------------------------------------------------------*/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/vfs.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/fs.h>
#include <mntent.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>

#include "vkernel.h"
#include "taskdef.h"

#include "drv_hotplug.h"
#include "drv_mmc.h"

#include "di_mmc.h"
#include "drv_gpio.h"


/*----------------------------------------------------------------------------------
** defines
**--------------------------------------------------------------------------------*/
//#define DI_MMC_DEBUG
#ifdef DI_MMC_DEBUG
#define PrintDebug(fmt, ...)    VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)    VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintInfo(fmt, ...)     VK_DBG_Print (fmt, ## __VA_ARGS__)
#else
#define PrintDebug(...)
#define PrintError(...)         VK_DBG_Print (__VA_ARGS__)
#define PrintInfo(fmt, ...)     VK_DBG_Print (fmt, ## __VA_ARGS__)
#endif

#define DI_MMC_MSG_SIZE			8

/*----------------------------------------------------------------------------------
** typedef
**--------------------------------------------------------------------------------*/
typedef enum {
	DI_MMC_SVC_FORMAT = 0,
	DI_MMC_SVC_MOUNT,
	DI_MMC_SVC_UMOUNT,
	DI_MMC_SVC_POWER,
	DI_MMC_SVC_MAX
} DI_MMC_SVC_CMD_E;

typedef struct {
	unsigned int  cmd;
	int           portnum;
	unsigned int  partition_idx;
	unsigned int  mount_flags;
	unsigned int  power;
	char          mount_path[96];
	char          mount_opts[96];
} DI_MMC_SVC_EVT_T;

typedef enum {
	FSTYPE_UNKNOWN = 0,
	FSTYPE_FAT16,
	FSTYPE_FAT32,
	FSTYPE_EXFAT,
	FSTYPE_EXT2,
	FSTYPE_EXT3,
	FSTYPE_EXT4,
	FSTYPE_XFS,
	FSTYPE_NTFS,
	FSTYPE_MAX
} DI_MMC_FSTYPE_E;

/*----------------------------------------------------------------------------------
** Variables
**--------------------------------------------------------------------------------*/
static unsigned long	MmcHotplugMsgId;
static unsigned long	MmcServiceMsgId;
static unsigned long	MmcHotplugTaskId;
static unsigned long	MmcServiceTaskId;
static unsigned long	MmcIntrTaskId;
static unsigned long    MmcPwrState; /* atomic. this variable means logical power state, not mmc hw power state */
static pfnDI_MMC_CallbackFunc		pfnCallback;
static pfnDI_MMC_SvcCallbackFunc	pfnSvcCallback;

/*----------------------------------------------------------------------------------
** Functions
**--------------------------------------------------------------------------------*/
static int P_MMC_IsDirExist(const char *dirname)
{
	struct stat st;
	return (0 == stat(dirname, &st) && S_ISDIR(st.st_mode)) ? 1 : 0;
}

static int P_MMC_Mkdir(const char *dirname)
{
	char *odirname, *p;

	odirname = p = strdup(dirname);
	do
	{
		p = VK_strchr(p + 1, '/');
		if (p)
			*p = '\0';

		if (access(odirname, F_OK) == -1)
		{
			if (mkdir(odirname, 0755) == -1)
			{
				PrintError("fail to make a %s. %s\n", odirname, strerror(errno));
				VK_free(odirname);
				return -1;
			}
		}
		if (p)
			*p = '/';
	} while(p);

	VK_free(odirname);
	return 0;
}

static int P_MMC_CheckSuperUser(void)
{
	struct passwd *pw = NULL;
	uid_t uid = getuid();

	if (uid != 0)
		return 0;

	pw = getpwuid(uid);
	if (pw == NULL)
		return 0;

	if (pw->pw_name && VK_strncmp(pw->pw_name, "root", 4) == 0)
	{
		return 1;
	}

	return 0;
}

static int P_MMC_GetFileSystemType(const char *dev)
{
	FILE *fp;
	char strbuf[32];
	int ret = FSTYPE_UNKNOWN;

	if (!dev)
	{
		return ret;
	}

	VK_MEM_Memset(strbuf, 0, sizeof(strbuf));
	VK_snprintf(strbuf, sizeof(strbuf), "/sbin/probefs %s", dev);
	fp = popen(strbuf, "r");
	if (!fp)
	{
		return ret;
	}

	VK_MEM_Memset(strbuf, 0, sizeof(strbuf));
	fgets(strbuf, sizeof(strbuf)-1, fp);
	pclose(fp);

	if(!VK_strncmp(strbuf, "ntfs", 4))
	{
		ret = FSTYPE_NTFS;
	}
	else if(!VK_strncmp(strbuf, "ext2", 4))
	{
		ret = FSTYPE_EXT2;
	}
	else if(!VK_strncmp(strbuf, "ext3", 4))
	{
		ret = FSTYPE_EXT3;
	}
	else if(!VK_strncmp(strbuf, "ext4", 4))
	{
		ret = FSTYPE_EXT4;
	}
	else if(!VK_strncmp(strbuf, "xfs", 4))
	{
		ret = FSTYPE_XFS;
	}
	else if(!VK_strncmp(strbuf, "fat16", 5))
	{
		ret = FSTYPE_FAT16;
	}
	else if(!VK_strncmp(strbuf, "fat32", 5))
	{
		ret = FSTYPE_FAT32;
	}
	else if(!VK_strncmp(strbuf, "exfat", 5))
	{
		ret = FSTYPE_EXFAT;
	}

	return ret;
}

static int P_MMC_DoMount(DI_MMC_PORT_E port, char *mount_dir, DI_MMC_MNTOPT_E eMntFlags, char *pMntOpts)
{
	FILE *fp = NULL;
	char *p = NULL;
	char devname[32] = {0,};
	char lineBuf[256];
	int	ret;
	char *fstype = NULL;
	char *default_opts = NULL;
	char *mnt_opts = NULL;
	unsigned long mnt_flags = 0;

#if defined(CONFIG_EMMC_FLASH)
	/*The MMC(SD CARD) is 1 when eMMC exist. So ignore mmcblk0*/
	/*mmcblk0 -> eMMC*/
	/*mmcblk1 -> MMC(SD CARD)*/
	VK_snprintf(devname, sizeof(devname)-1, "mmcblk%dp1", port);
#else
	VK_snprintf(devname, sizeof(devname)-1, "mmcblk%dp1", port-1);
#endif
	fp = fopen("/proc/partitions", "r");
	if (!fp)
	{
		PrintError("[%s] Error! can't open file\n", __FUNCTION__);
		return -1;
	}
	while(fgets(lineBuf, sizeof(lineBuf)-1, fp))
	{
		/* find target partition device */
		p = VK_strstr((const char*)lineBuf, (const char*)devname);
		if (p) break;
	}
	fclose(fp);

	/* if there is no target directory, make it */
	if (P_MMC_IsDirExist(mount_dir) == FALSE)
	{
		P_MMC_Mkdir(mount_dir);
	}

	VK_MEM_Memset(devname, 0, sizeof(devname));
	if (p)
	{
#if defined(CONFIG_EMMC_FLASH)
		VK_snprintf(devname, sizeof(devname)-1, "/dev/mmcblk%dp1", port);
#else	
		VK_snprintf(devname, sizeof(devname)-1, "/dev/mmcblk%dp1", port-1);
#endif
	}
	else
	{
		/* if there is no partition device, then try to mount mmclk0,1,2.. directly */
		/* this is for mmc which doesn't have MBR */
#if defined(CONFIG_EMMC_FLASH)
		VK_snprintf(devname, sizeof(devname)-1, "/dev/mmcblk%d", port);
#else		
		VK_snprintf(devname, sizeof(devname)-1, "/dev/mmcblk%d", port-1);
#endif
	}

	ret = P_MMC_GetFileSystemType(devname);
	switch(ret) {
	case FSTYPE_FAT16:
	case FSTYPE_FAT32:
		fstype = "vfat";
		default_opts = "iocharset=utf8";
		break;
	case FSTYPE_EXT2:
		fstype = "ext2";
		break;
	case FSTYPE_EXT3:
		fstype = "ext3";
		break;
	case FSTYPE_EXT4:
		fstype = "ext4";
		break;
	case FSTYPE_NTFS:
		fstype = "tntfs";
		default_opts = "nls=utf8";
		break;
	case FSTYPE_EXFAT:
	case FSTYPE_XFS:
	case FSTYPE_UNKNOWN:
	default:
		PrintError("[P_MMC_DoMount] Unknown or not supported file system type(%d)\n", ret);
		return -1;
	}

	/* set mount option */
	if (eMntFlags & DI_MMC_MNTOPT_MANDLOCK)     mnt_flags |= MS_MANDLOCK;
	if (eMntFlags & DI_MMC_MNTOPT_NOATIME)      mnt_flags |= MS_NOATIME;
	if (eMntFlags & DI_MMC_MNTOPT_NODEV)        mnt_flags |= MS_NODEV;
	if (eMntFlags & DI_MMC_MNTOPT_NODIRATIME)   mnt_flags |= MS_NODIRATIME;
	if (eMntFlags & DI_MMC_MNTOPT_NOEXEC)       mnt_flags |= MS_NOEXEC;
	if (eMntFlags & DI_MMC_MNTOPT_NOSUID)       mnt_flags |= MS_NOSUID;
	if (eMntFlags & DI_MMC_MNTOPT_RDONLY)       mnt_flags |= MS_RDONLY;
	if (eMntFlags & DI_MMC_MNTOPT_REMOUNT)      mnt_flags |= MS_REMOUNT;
	if (eMntFlags & DI_MMC_MNTOPT_SYNCHRONOUS)  mnt_flags |= MS_SYNCHRONOUS;

	switch(eMntFlags) {
	case DI_MMC_MNTOPT_BIND:
	case DI_MMC_MNTOPT_MOVE:
		PrintError("[%s] Ignored! option(0x%x) is not supported in this function\n", __FUNCTION__, eMntFlags);
		break;
	default:
		break;
	}

	if (VK_strlen(pMntOpts))
	{
		mnt_opts = (char*)pMntOpts;
	}
	else
	{
		mnt_opts = default_opts;
	}

	ret = mount((char*)devname, (char*)mount_dir, fstype, mnt_flags, mnt_opts);
#if defined(CONFIG_DOSFSTOOLS)
	if (ret < 0 && errno == EROFS && VK_strncmp(fstype, "vfat", 4)==0)
	{
		/* in case that error occures and have fs, excute check-disk */
		VK_MEM_Memset(lineBuf, 0, sizeof(lineBuf)-1);
		VK_snprintf(lineBuf, sizeof(lineBuf)-1, "dosfsck -a %s", devname);
		VK_SYSTEM_Command(lineBuf);

		ret = mount((char*)devname, (char*)mount_dir, fstype, mnt_flags, mnt_opts);
	}
#endif

	if (ret < 0 && errno == EROFS)
	{
		/* try to mount as a read-only filesystem */
		mnt_flags |= MS_RDONLY;
		ret = mount((char*)devname, (char*)mount_dir, fstype, mnt_flags, mnt_opts);
	}

	PrintInfo("[P_MMC_DoMount] Mount Done!! device(%s), type(%s), flags(0x%x), opts(%s), ret(%d) errno(%d)\n", devname, fstype, mnt_flags, mnt_opts, ret, errno);
	return ret;
}

static HBOOL P_MMC_GetLockState(DI_MMC_PORT_E port)
{
	int   fd;
	char  dev_path[128];
	char  readonly = 0;
	int   len;
	HBOOL bRet = FALSE;

#if defined(CONFIG_EMMC_FLASH)
	VK_snprintf(dev_path, sizeof(dev_path), "/sys/block/mmcblk%d/device/lock", (int)(port));
#else
	VK_snprintf(dev_path, sizeof(dev_path), "/sys/block/mmcblk%d/device/lock", (int)(port-1));
#endif
	fd = open(dev_path, O_RDONLY);
	if (fd < 0)
	{
		PrintInfo("[%s] Warning. card doesn't support write-protection\n", __FUNCTION__);
		return bRet;
	}

	len = read(fd, &readonly, 1);
	if (len > 0 && readonly == '1')
	{
		bRet = TRUE;
	}
	close(fd);

	return bRet;
}

static HUINT8 P_MMC_GetBusWidths(DI_MMC_PORT_E port)
{
	int   fd;
	char  dev_path[128];
	int   len;
	HUINT8 width; //default

#if defined(CONFIG_EMMC_FLASH)
	VK_snprintf(dev_path, sizeof(dev_path), "/sys/block/mmcblk%d/device/bus_width", (int)(port));
#else
	VK_snprintf(dev_path, sizeof(dev_path), "/sys/block/mmcblk%d/device/bus_width", (int)(port-1));
#endif
	fd = open(dev_path, O_RDONLY);
	if (fd < 0)
	{
		PrintInfo("[%s] Error. can't open %s file\n", __FUNCTION__, dev_path);
		return 0;
	}

	len = read(fd, &width, sizeof(width));
	if (len > 0)
	{
		close(fd);
		return (HUINT8)(width-'0');
	}
	close(fd);

	return 0;
}

static int P_MMC_GetProductSerialNumber(DI_MMC_PORT_E port, HUINT8 *buf, HUINT32 bufsize)
{
	int   fd;
	char  dev_path[128];
	int   len;
	char  ucaTmp[11];
	HUINT32 ulPSN = 0;

	if(buf == NULL || bufsize < 4)
	{
		return 0;
	}
#if defined(CONFIG_EMMC_FLASH)
	VK_snprintf(dev_path, sizeof(dev_path), "/sys/block/mmcblk%d/device/serial", (int)(port));
#else
	VK_snprintf(dev_path, sizeof(dev_path), "/sys/block/mmcblk%d/device/serial", (int)(port-1));
#endif
	fd = open(dev_path, O_RDONLY);
	if (fd < 0)
	{
		PrintInfo("[%s] Error. can't open %s file\n", __FUNCTION__, dev_path);
		return 0;
	}

	VK_memset(ucaTmp,0,sizeof(ucaTmp));
	len = read(fd, ucaTmp, 10);
	if (len > 0)
	{
		ulPSN = VK_strtoul(&ucaTmp[2],NULL,16);

		buf[0] = (ulPSN&0xFF000000) >> 24;
		buf[1] = (ulPSN&0x00FF0000) >> 16;
		buf[2] = (ulPSN&0x0000FF00) >> 8;
		buf[3] = (ulPSN&0x000000FF) ;
		close(fd);
		return len;
	}
	close(fd);

	return 0;
}


static void P_MMC_HotplugTask(void *param)
{
	hp_evt_t        tHotplugEvent;
	DI_MMC_HP_EVT_T	mmc_info_t;

	(void)param;

	/* register drv hotplug callback */
	DRV_HOTPLUG_MsgRegister(DRV_HP_SD, MmcHotplugMsgId);

	while(1)
	{
		if (VK_MSG_Receive(MmcHotplugMsgId, &tHotplugEvent, sizeof(hp_evt_t)) != VK_OK)
			continue;

		if (tHotplugEvent.portnum >= DI_MMC_PORT_MAX || tHotplugEvent.portnum < DI_MMC_PORT_1)
		{
			PrintError("[%s] Error! Not support mmc portnum #%d\n", __FUNCTION__, tHotplugEvent.portnum);
			continue;
		}

		VK_MEM_Memset(&mmc_info_t, 0, sizeof(DI_MMC_HP_EVT_T));
		VK_strncpy(mmc_info_t.dev_name, &(tHotplugEvent.devpath[7]), sizeof(mmc_info_t.dev_name));
		mmc_info_t.portnum = tHotplugEvent.portnum;

		switch(tHotplugEvent.eConnEvt) {
		case DRV_HP_CONN:
			/* when normal event occurs by drv hotplug, send event as condition of power */
			if (pfnCallback && (MmcPwrState & (1<<(mmc_info_t.portnum-1)))) {
				pfnCallback(DI_MMC_EVT_CONNECTED, (void*)&mmc_info_t);
			}
			break;
		case DRV_HP_DISCONN:
			/* when normal event occurs by drv hotplug, send event as condition of power */
			if (pfnCallback && (MmcPwrState & (1<<(mmc_info_t.portnum-1)))) {
				pfnCallback(DI_MMC_EVT_DISCONNECTED, (void*)&mmc_info_t);
			}
			break;
		case DRV_HP_CONN_BY_PWR:
			/* when event occurs by power control, send event to app unconditionally */
			if (pfnCallback)
			{
				pfnCallback(DI_MMC_EVT_CONNECTED, (void*)&mmc_info_t);
			}
			break;
		case DRV_HP_DISCONN_BY_PWR:
			/* when event occurs by power control, send event to app unconditionally */
			if (pfnCallback)
			{
				pfnCallback(DI_MMC_EVT_DISCONNECTED, (void*)&mmc_info_t);
			}
			break;
		default:
			break;
		}
	}

	return;
}

static void P_MMC_ServiceTask(void *param)
{
	DI_MMC_SVC_EVT_T	tSvcEvt;
	char				cmdline[256] = {0,};
	DI_MMC_SVC_EVT_E	svc_evt_e;
	char 				*pValue;
	char				key[32];
	int					ret;
	char            szDev[32] = {0,};
	FILE            *fp;
	hp_evt_t        tEvt;

	(void)param;
	
	while(1)
	{
		if (VK_MSG_Receive(MmcServiceMsgId, &tSvcEvt, sizeof(DI_MMC_SVC_EVT_T)) != VK_OK)
			continue;

		switch(tSvcEvt.cmd) {
		case DI_MMC_SVC_POWER:
			/* when mmc power is controlled, check mmc state and send hotplug event */
			fp = fopen("/proc/partitions", "r");
			if (fp)
			{
				while(fscanf(fp, "%30s", szDev) != EOF)
				{
					if (VK_strncmp(szDev, "mmcblk", VK_strlen("mmcblk")) == 0 && szDev[7] != 'p')
					{
						VK_MEM_Memset(&tEvt, 0, sizeof(hp_evt_t));
#if defined(CONFIG_EMMC_FLASH)
						tEvt.portnum = (int)(szDev[6] - '0');
#else
						tEvt.portnum = (int)(szDev[6] - '0' + 1);
#endif
						if (tEvt.portnum == tSvcEvt.portnum)
						{
							tEvt.eConnEvt = (tSvcEvt.power) ? DRV_HP_CONN_BY_PWR : DRV_HP_DISCONN_BY_PWR;
							VK_snprintf(tEvt.devpath, sizeof(tEvt.devpath), "/block/%s", szDev);
							tEvt.len = VK_strlen(tEvt.devpath);
							/* send hotplug event to mmc hotplug task */
							(void)VK_MSG_SendTimeout(MmcHotplugMsgId, &tEvt, sizeof(hp_evt_t), 0);
						}
					}
					VK_MEM_Memset(szDev, 0, sizeof(szDev));
				}
				fclose(fp);
			}

			if (tSvcEvt.power)
			{
				MmcPwrState |= (1 << (tSvcEvt.portnum-1));
			}
			else
			{
				MmcPwrState &= ~(1 << (tSvcEvt.portnum-1));
			}
			break;
		case DI_MMC_SVC_FORMAT:
#if defined(CONFIG_DOSFSTOOLS)
			/* make partitions : MBR + 1-BR */
			VK_MEM_Memset(cmdline, 0, sizeof(cmdline));
#if defined(CONFIG_EMMC_FLASH)
			VK_snprintf(cmdline, sizeof(cmdline)-1, "parted /dev/mmcblk%d --script mklabel msdos", tSvcEvt.portnum);
#else
			VK_snprintf(cmdline, sizeof(cmdline)-1, "parted /dev/mmcblk%d --script mklabel msdos", tSvcEvt.portnum-1);
#endif
			if (VK_SYSTEM_Command(cmdline) != VK_OK)
			{
				PrintError("[%s] Error!! fail to excute parted\n", __FUNCTION__);
				svc_evt_e = DI_MMC_SVC_EVT_FORMAT_FAIL;
				break;
			}
#if defined(CONFIG_EMMC_FLASH)
			VK_snprintf(cmdline, sizeof(cmdline)-1, "parted /dev/mmcblk%d --script -- mkpart primary fat32 0 -1", tSvcEvt.portnum);
#else			
			VK_snprintf(cmdline, sizeof(cmdline)-1, "parted /dev/mmcblk%d --script -- mkpart primary fat32 0 -1", tSvcEvt.portnum-1);
#endif
			if (VK_SYSTEM_Command(cmdline) != VK_OK)
			{
				PrintError("[%s] Error!! fail to excute parted\n", __FUNCTION__);
				svc_evt_e = DI_MMC_SVC_EVT_FORMAT_FAIL;
				break;
			}

			/* excute format */
			VK_MEM_Memset(cmdline, 0, sizeof(cmdline));
#if defined(CONFIG_EMMC_FLASH)
			VK_snprintf(cmdline, sizeof(cmdline)-1, "mkdosfs -F 32 /dev/mmcblk%dp1", tSvcEvt.portnum);
#else			
			VK_snprintf(cmdline, sizeof(cmdline)-1, "mkdosfs -F 32 /dev/mmcblk%dp1", tSvcEvt.portnum-1);
#endif
			if (VK_SYSTEM_Command(cmdline) == VK_OK)
			{
				svc_evt_e = DI_MMC_SVC_EVT_FORMAT_DONE;
			}
			else
			{
				svc_evt_e = DI_MMC_SVC_EVT_FORMAT_FAIL;
				PrintError("[%s] Error!! fail to format partition(port %d)\n", __FUNCTION__, tSvcEvt.portnum-1);
			}
#else
			svc_evt_e = DI_MMC_SVC_EVT_FORMAT_FAIL;
#endif
			break;
		case DI_MMC_SVC_MOUNT:
			ret = P_MMC_DoMount(tSvcEvt.portnum, tSvcEvt.mount_path, (DI_MMC_MNTOPT_E)tSvcEvt.mount_flags, tSvcEvt.mount_opts);
			if (ret == 0)
			{
				/* mount success */
				svc_evt_e = DI_MMC_SVC_EVT_MOUNT_DONE;

				/* save mount path */
				VK_MEM_Memset(key, 0, sizeof(key));
#if defined(CONFIG_EMMC_FLASH)
				VK_snprintf(key, sizeof(key)-1, "fs.mmc.mmcblk%d", tSvcEvt.portnum);
#else				
				VK_snprintf(key, sizeof(key)-1, "fs.mmc.mmcblk%d", tSvcEvt.portnum-1);
#endif
				setenv(key, tSvcEvt.mount_path, 1);
			}
			else
			{
				svc_evt_e = DI_MMC_SVC_EVT_MOUNT_FAIL;
			}
			break;
		case DI_MMC_SVC_UMOUNT:
			/* get mount path */
			VK_MEM_Memset(key, 0, sizeof(key));
#if defined(CONFIG_EMMC_FLASH)
			VK_snprintf(key, sizeof(key)-1, "fs.mmc.mmcblk%d", tSvcEvt.portnum);
#else			
			VK_snprintf(key, sizeof(key)-1, "fs.mmc.mmcblk%d", tSvcEvt.portnum-1);
#endif
			pValue = getenv(key);
			if (!pValue)
			{
				svc_evt_e = DI_MMC_SVC_EVT_UMOUNT_FAIL;
				PrintError("[%s] Can't get mount path(port:%d)!!\n", __FUNCTION__, tSvcEvt.portnum-1);
				break;
			}

			ret = umount(pValue);
			if (ret == 0)
			{
				svc_evt_e = DI_MMC_SVC_EVT_UMOUNT_DONE;
				unsetenv(key);
			}
			else
			{
				svc_evt_e = DI_MMC_SVC_EVT_UMOUNT_FAIL;
				PrintError("[%s]: Umount Fail!!\n", __FUNCTION__);
#if defined(CONFIG_PRODUCT_IMAGE_DEBUG)
				/* print messages of file lists */
				PrintError("[%s] Print open file lists\n", __FUNCTION__, pValue);
				VK_MEM_Memset(cmdline, 0, sizeof(cmdline));
				VK_snprintf(cmdline, sizeof(cmdline)-1, "lsof | grep %s", pValue);
				VK_SYSTEM_Command(cmdline);
#endif
			}
			break;
		default:
			continue;
		}

		VK_INTR_Event(MmcIntrTaskId, (void*)&svc_evt_e);
	}

	return;
}

static void P_MMC_Intr(void *arg)
{
	DI_MMC_SVC_EVT_E evt = *(unsigned long*)arg;

	if (pfnSvcCallback)
	{
		pfnSvcCallback(evt);
	}

	PrintDebug("[%s] Intr(USB) callback function called! \n", __FUNCTION__);
}

static int P_MMC_GetVolumeUuid(DI_MMC_PORT_E port, HUINT8 *buf, HUINT32 bufsize)
{
	FILE *fp = NULL;
	char *p = NULL;
	char devname[32] = {0,};
	char szBuf[256] = {0,};

	if (port >= DI_MMC_PORT_MAX || port < DI_MMC_PORT_1 || !buf || !bufsize)
	{
		PrintError("[%s] Error. invalid arguments (port:%d, buf:0x%x, bufsize:%d)\n", __FUNCTION__, port, (unsigned long)buf, bufsize);
		return 0;
	}

#if defined(CONFIG_EMMC_FLASH)
	VK_snprintf(devname, sizeof(devname)-1, "mmcblk%dp1", port);
#else
	VK_snprintf(devname, sizeof(devname)-1, "mmcblk%dp1", port-1);
#endif
	fp = fopen("/proc/partitions", "r");
	if (!fp)
	{
		PrintError("[%s] Error! can't open file\n", __FUNCTION__);
		return 0;
	}
	while(fgets(szBuf, sizeof(szBuf)-1, fp))
	{
		/* find target partition device */
		p = VK_strstr((const char*)szBuf, (const char*)devname);
		if (p) break;
	}
	fclose(fp);

	if (p)
	{
#if defined(CONFIG_EMMC_FLASH)
		VK_snprintf(devname, sizeof(devname)-1, "/dev/mmcblk%dp1", port);
#else	
		VK_snprintf(devname, sizeof(devname)-1, "/dev/mmcblk%dp1", port-1);
#endif
	}
	else
	{
		/* if there is no partition device, then try to mount mmclk0,1,2.. directly */
		/* this is for mmc which doesn't have MBR */
#if defined(CONFIG_EMMC_FLASH)
		VK_snprintf(devname, sizeof(devname)-1, "/dev/mmcblk%d", port);
#else
		VK_snprintf(devname, sizeof(devname)-1, "/dev/mmcblk%d", port-1);
#endif
	}

	VK_snprintf(szBuf, sizeof(szBuf), "/sbin/probefs -u %s", (char*)devname);
	fp = popen(szBuf, "r");
	if (!fp)
		return 0;

	VK_MEM_Memset(szBuf, 0, sizeof(szBuf));
	fgets(szBuf, sizeof(szBuf)-1, fp);
	pclose(fp);

	/* remove char '\n' and add null byte */
	p = strrchr(szBuf, '\n');
	if (p) *p = 0;

	VK_snprintf(buf, bufsize, "%s", szBuf);

	return VK_strlen(szBuf);
}

DI_ERR_CODE	DI_MMC_PowerControl(DI_MMC_PORT_E port, DI_MMC_PWR_E on)
{
	DI_MMC_SVC_EVT_T 	SvcEvtBuf;
	unsigned int		ret;

	if (port >= DI_MMC_PORT_MAX || port < DI_MMC_PORT_1)
	{
		PrintError("[%s] Error. invalid arguments (port:%d, on:%d)\n", __FUNCTION__, port, on);
		return DI_ERR_ERROR;
	}

	VK_MEM_Memset(&SvcEvtBuf, 0, sizeof(DI_MMC_SVC_EVT_T));
	SvcEvtBuf.cmd = (unsigned int)DI_MMC_SVC_POWER;
	SvcEvtBuf.portnum = (unsigned int)port;
	SvcEvtBuf.power = (on == DI_MMC_PWR_ON) ? 1 : 0;

	ret = VK_MSG_SendTimeout(MmcServiceMsgId, &SvcEvtBuf, sizeof(DI_MMC_SVC_EVT_T), 0);
	if (ret != VK_OK)
	{
		PrintError("[%s] VK_MSG_Send Error!\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_MMC_GetMMCInfo(DI_MMC_PORT_E port, DI_MMC_INFO_T *tMmcInfo)
{
	FILE			*fp;
	int				blocks;
	int				sectorsize;
	char			strbuffer[32];
	int				fd;
	int             len;
	char			*value;
	struct statfs	tStatFs;
	struct mntent 	*tMntEnt;

	if (port >= DI_MMC_PORT_MAX || port < DI_MMC_PORT_1 || !tMmcInfo)
	{
		PrintError("[%s] Error. invalid arguments (port:%d, tMmcInfo:0x%x)\n", __FUNCTION__, port, (unsigned long)tMmcInfo);
		return DI_ERR_ERROR;
	}

	VK_MEM_Memset(strbuffer, 0, sizeof(strbuffer));
#if defined(CONFIG_EMMC_FLASH)
	VK_snprintf(strbuffer, sizeof(strbuffer)-1, "/dev/mmcblk%d", port);
#else	
	VK_snprintf(strbuffer, sizeof(strbuffer)-1, "/dev/mmcblk%d", port - 1);
#endif
	fd = open(strbuffer, O_RDONLY);
	if (fd < 0 )
	{
		PrintError("[%s] Can't open device file (%s)\n", __FUNCTION__, strbuffer);
		return DI_ERR_ERROR;
	}
	if (ioctl(fd, BLKGETSIZE, &blocks) < 0)
	{
		PrintError("[%s] Can't get blocks (%s)\n", __FUNCTION__, strbuffer);
		close(fd);
		return DI_ERR_ERROR;
	}
	if (ioctl(fd, BLKSSZGET, &sectorsize) < 0)
	{
		PrintError("[%s] Can't get sector size (%s)\n", __FUNCTION__, strbuffer);
		close(fd);
		return DI_ERR_ERROR;
	}
	close(fd);

	tMmcInfo->nPort = (HUINT32)port;
	tMmcInfo->uiDiskSize = (HUINT32)((blocks/1024) * sectorsize);
	tMmcInfo->uiUsedSize = (HUINT32)0;
	tMmcInfo->uiAvailableSize = (HUINT32)0;
	tMmcInfo->nMounted = (HUINT32)0;
	tMmcInfo->bCardLock = P_MMC_GetLockState(port);
	tMmcInfo->ucBusWidths = P_MMC_GetBusWidths(port);
	len = P_MMC_GetProductSerialNumber(port,tMmcInfo->ucPSN, sizeof(tMmcInfo->ucPSN));
	if (len == 0)
	{
		VK_memset(tMmcInfo->ucPSN,0,sizeof(tMmcInfo->ucPSN));
	}

	len = P_MMC_GetVolumeUuid(port, tMmcInfo->szUuid, sizeof(tMmcInfo->szUuid));
	if (len == 0)
	{
		tMmcInfo->szUuid[0] = 0;
	}

	/* Check whether mounted or not */
	VK_MEM_Memset(strbuffer, 0, sizeof(strbuffer));
#if defined(CONFIG_EMMC_FLASH)
	VK_snprintf(strbuffer, sizeof(strbuffer)-1, "fs.mmc.mmcblk%d", port );
#else	
	VK_snprintf(strbuffer, sizeof(strbuffer)-1, "fs.mmc.mmcblk%d", port - 1);
#endif
	value = getenv(strbuffer);
	if (!value)
		return DI_ERR_OK;

	fp = setmntent("/proc/mounts", "r");
	if (!fp)
		return DI_ERR_OK;

	while(1)
	{
		tMntEnt = getmntent(fp);
		if (!tMntEnt)
			break;
		if (VK_strstr(tMntEnt->mnt_dir, value))
		{
			/* Mounted */
			tMmcInfo->nMounted = (HUINT32)1;
			if (statfs(value, &tStatFs) == 0)
			{
				if (P_MMC_CheckSuperUser() > 0)
				{
					/* super user */
					tMmcInfo->uiAvailableSize = (HUINT32)((tStatFs.f_bfree*tStatFs.f_bsize + 1023)/1024); //in KB
					tMmcInfo->uiUsedSize = (HUINT32)(((tStatFs.f_blocks - tStatFs.f_bfree)*tStatFs.f_bsize + 1023)/1024); //in KB
				}
				else
				{
					tMmcInfo->uiAvailableSize = (HUINT32)((tStatFs.f_bavail*tStatFs.f_bsize + 1023)/1024); //in KB
					tMmcInfo->uiUsedSize = (HUINT32)(((tStatFs.f_blocks - tStatFs.f_bavail)*tStatFs.f_bsize + 1023)/1024); //in KB
				}
			}
			break;
		}
	}
	endmntent(fp);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_MMC_Format(DI_MMC_PORT_E port)
{
	DI_MMC_SVC_EVT_T 	SvcEvtBuf;
	unsigned int		ret;

	if (port >= DI_MMC_PORT_MAX || port < DI_MMC_PORT_1)
	{
		PrintError("[%s] Error. invalid arguments (port:%d)\n", __FUNCTION__, port);
		return DI_ERR_ERROR;
	}

	VK_MEM_Memset(&SvcEvtBuf, 0, sizeof(DI_MMC_SVC_EVT_T));
	SvcEvtBuf.cmd = (unsigned int)DI_MMC_SVC_FORMAT;
	SvcEvtBuf.portnum = (unsigned int)port;

	ret = VK_MSG_SendTimeout(MmcServiceMsgId, &SvcEvtBuf, sizeof(DI_MMC_SVC_EVT_T), 0);
	if (ret != VK_OK)
	{
		PrintError("[%s] VK_MSG_Send Error!\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_MMC_MountExt(DI_MMC_PORT_E port, HUINT8 *mount_dir, DI_MMC_MNTOPT_E eMntFlags, char *pMntOpts)
{
	DI_MMC_SVC_EVT_T 	SvcEvtBuf;
	unsigned int		ret;

	if (port >= DI_MMC_PORT_MAX || port < DI_MMC_PORT_1 || !mount_dir)
	{
		PrintError("[%s] Error. invalid arguments (port:%d, mount_dir:0x%x)\n", __FUNCTION__, port, (unsigned long)mount_dir);
		return DI_ERR_ERROR;
	}

	VK_MEM_Memset(&SvcEvtBuf, 0, sizeof(DI_MMC_SVC_EVT_T));
	SvcEvtBuf.cmd = (unsigned int)DI_MMC_SVC_MOUNT;
	SvcEvtBuf.portnum = (unsigned int)port;
	SvcEvtBuf.mount_flags = (unsigned int)eMntFlags;
	if (pMntOpts)
	{
		VK_MEM_Memcpy(SvcEvtBuf.mount_opts, pMntOpts, sizeof(SvcEvtBuf.mount_opts)-1);
	}
	VK_strncpy(SvcEvtBuf.mount_path, mount_dir, sizeof(SvcEvtBuf.mount_path)-1);

	ret = VK_MSG_SendTimeout(MmcServiceMsgId, &SvcEvtBuf, sizeof(DI_MMC_SVC_EVT_T), 0);
	if (ret != VK_OK)
	{
		PrintError("[%s] VK_MSG_Send Error!\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_MMC_Umount(DI_MMC_PORT_E port)
{
	DI_MMC_SVC_EVT_T 	SvcEvtBuf;
	unsigned int		ret;

	if (port >= DI_MMC_PORT_MAX || port < DI_MMC_PORT_1)
	{
		PrintError("[%s] Error. invalid arguments (port:%d)\n", __FUNCTION__, port);
		return DI_ERR_ERROR;
	}

	VK_MEM_Memset(&SvcEvtBuf, 0, sizeof(DI_MMC_SVC_EVT_T));
	SvcEvtBuf.cmd = (unsigned int)DI_MMC_SVC_UMOUNT;
	SvcEvtBuf.portnum = (unsigned int)port;

	ret = VK_MSG_SendTimeout(MmcServiceMsgId, &SvcEvtBuf, sizeof(DI_MMC_SVC_EVT_T), 0);
	if (ret != VK_OK)
	{
		PrintError("[%s] VK_MSG_Send Error!\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_MMC_GetVolumeName(DI_MMC_PORT_E port, HUINT8 *buf, HUINT32 bufsize)
{
	FILE 	*fp = NULL;
	char	*p = NULL;
	char	devname[32] = {0,};
	char	szBuf[256] = {0,};

	if (port >= DI_MMC_PORT_MAX || port < DI_MMC_PORT_1 || !buf || !bufsize)
	{
		PrintError("[%s] Error. invalid arguments (port:%d, buf:0x%x, bufsize:%d)\n", __FUNCTION__, port, (unsigned long)buf, bufsize);
		return DI_ERR_ERROR;
	}
#if defined(CONFIG_EMMC_FLASH)
	VK_snprintf(devname, sizeof(devname)-1, "mmcblk%dp1", port);
#else
	VK_snprintf(devname, sizeof(devname)-1, "mmcblk%dp1", port-1);
#endif
	fp = fopen("/proc/partitions", "r");
	if (!fp)
	{
		PrintError("[%s] Error! can't open file\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}
	while(fgets(szBuf, sizeof(szBuf)-1, fp))
	{
		/* find target partition device */
		p = VK_strstr((const char*)szBuf, (const char*)devname);
		if (p) break;
	}
	fclose(fp);

	if (p)
	{
#if defined(CONFIG_EMMC_FLASH)
		VK_snprintf(devname, sizeof(devname)-1, "/dev/mmcblk%dp1", port);
#else	
		VK_snprintf(devname, sizeof(devname)-1, "/dev/mmcblk%dp1", port-1);
#endif
	}
	else
	{
		/* if there is no partition device, then try to mount mmclk0,1,2.. directly */
		/* this is for mmc which doesn't have MBR */
#if defined(CONFIG_EMMC_FLASH)
		VK_snprintf(devname, sizeof(devname)-1, "/dev/mmcblk%d", port);
#else
		VK_snprintf(devname, sizeof(devname)-1, "/dev/mmcblk%d", port-1);
#endif
	}

	VK_snprintf(szBuf, sizeof(szBuf), "/sbin/probefs -l %s", (char*)devname);
	fp = popen(szBuf, "r");
	if (!fp)
		return DI_ERR_ERROR;

	VK_MEM_Memset(szBuf, 0, sizeof(szBuf));
	fgets(szBuf, sizeof(szBuf)-1, fp);
	pclose(fp);

	/* remove char '\n' and add null byte */
	p = strrchr(szBuf, '\n');
	if (p) *p = 0;

	VK_snprintf(buf, bufsize, "%s", szBuf);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_MMC_SetVolumeName(DI_MMC_PORT_E port, HUINT8 *buf, HUINT32 bufsize)
{
	FILE	*fp = NULL;
	char	*p = NULL;
	char	devname[32] = {0,};
	char	szBuf[256] = {0,};

	if (port >= DI_MMC_PORT_MAX || port < DI_MMC_PORT_1 || !buf || !bufsize)
	{
		PrintError("[%s] Error. invalid arguments (port:%d, buf:0x%x, bufsize:%d)\n", __FUNCTION__, port, (unsigned long)buf, bufsize);
		return DI_ERR_ERROR;
	}

#if defined(CONFIG_EMMC_FLASH)
	VK_snprintf(devname, sizeof(devname)-1, "mmcblk%dp1", port);
#else
	VK_snprintf(devname, sizeof(devname)-1, "mmcblk%dp1", port-1);
#endif
	fp = fopen("/proc/partitions", "r");
	if (!fp)
	{
		PrintError("[%s] Error! can't open file\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}
	while(fgets(szBuf, sizeof(szBuf)-1, fp))
	{
		/* find target partition device */
		p = VK_strstr((const char*)szBuf, (const char*)devname);
		if (p) break;
	}
	fclose(fp);

	VK_MEM_Memset(devname, 0, sizeof(devname));
	if (p)
	{
#if defined(CONFIG_EMMC_FLASH)
		VK_snprintf(devname, sizeof(devname)-1, "/dev/mmcblk%dp1", port);	
#else
		VK_snprintf(devname, sizeof(devname)-1, "/dev/mmcblk%dp1", port-1);
#endif
	}
	else
	{
		/* if there is no partition device, then try to mount mmclk0,1,2.. directly */
		/* this is for mmc which doesn't have MBR */
#if defined(CONFIG_EMMC_FLASH)
		VK_snprintf(devname, sizeof(devname)-1, "/dev/mmcblk%d", port);
#else		
		VK_snprintf(devname, sizeof(devname)-1, "/dev/mmcblk%d", port-1);
#endif
	}

	VK_snprintf(szBuf, sizeof(szBuf), "/sbin/probefs -L \"%s\" %s", buf, (char*)devname);
	if (VK_SYSTEM_Command(szBuf) != VK_OK)
		return DI_ERR_ERROR;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_MMC_RegisterCallbackFunc(pfnDI_MMC_CallbackFunc pfnMmcCallback)
{
	if (!pfnMmcCallback) return DI_ERR_ERROR;

	pfnCallback = pfnMmcCallback;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_MMC_RegisterSvcCallbackFunc(pfnDI_MMC_SvcCallbackFunc pfnMmcSvcCallback)
{
	if (!pfnMmcSvcCallback) return DI_ERR_ERROR;

	pfnSvcCallback = pfnMmcSvcCallback;

	return DI_ERR_OK;
}

DRV_Error DRV_MMC_Init(void)
{
	if (VK_MSG_Create(DI_MMC_MSG_SIZE, sizeof(hp_evt_t), "MmcHotplugMsg", &MmcHotplugMsgId, VK_SUSPENDTYPE_FIFO) != VK_OK)
	{
		return DRV_ERR;
	}

	if (VK_MSG_Create(DI_MMC_MSG_SIZE, sizeof(DI_MMC_SVC_EVT_T), "MmcMsg", &MmcServiceMsgId, VK_SUSPENDTYPE_FIFO) != VK_OK)
	{
		return DRV_ERR;
	}

	if (VK_TASK_Create(P_MMC_HotplugTask, USB_TASK_PRIORITY, USB_TASK_STACK_SIZE, "MMC_HotplugTask", NULL, &MmcHotplugTaskId, FALSE) != VK_OK)
	{
		return DRV_ERR;
	}

	if (VK_TASK_Create(P_MMC_ServiceTask, USB_TASK_PRIORITY, USB_TASK_STACK_SIZE, "MMC_ServiceTask", NULL, &MmcServiceTaskId, FALSE) != VK_OK)
	{
		return DRV_ERR;
	}

	if (VK_INTR_Register(P_MMC_Intr, "mmc_service", sizeof(unsigned long), &MmcIntrTaskId, VK_INTR_TASK0) != VK_OK)
	{
		return DRV_ERR;
	}

	VK_TASK_Start(MmcHotplugTaskId);
	VK_TASK_Start(MmcServiceTaskId);

	return DRV_OK;
}

DI_ERR_CODE DI_MMC_Init(void)
{
	DRV_Error err;

	err = DRV_MMC_Init();
	if (err != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}
