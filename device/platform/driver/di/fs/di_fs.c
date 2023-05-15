/*
 * @defgroup        			DI_FS : DI_FS module
 * @author						lslee
 * @note						DI_FS APIs
 * @brief						Define DI_FS APIs
 * @file						di_fs.c											*/
/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#include 	<malloc.h>
#endif

#ifndef WIN32
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/statvfs.h>
#include <sys/mount.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>
#include <linux/hdreg.h>
#endif
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <limits.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <linux/types.h>
#include <pthread.h>
#include <errno.h>

#include "htype.h"
#include "di_fs.h"
#include "taskdef.h"
#include "di_uart.h"
#include "di_err.h"
#include "drv_err.h"
#include "vkernel.h"


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define UNUSED(x)	(x) = (x)

#ifdef DI_FS_DEBUG
#define PrintDebug(fmt, ...) 		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)    		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter()				VK_DBG_Print("(+)%s\n", __FUNCTION__)
#define PrintExit()				VK_DBG_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintDebug(fmt, ...)    		while (0) ((int (*)(char *, ...)) 0)
#define PrintError(fmt, ...)			VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter() 			while (0) ((int (*)(char *, ...)) 0)
#define PrintExit()				while (0) ((int (*)(char *, ...)) 0)
#endif /* DI_FS_DEBUG */


/*******************************************************************/
/******************************* extern ****************************/
/*******************************************************************/

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/

DI_ERR_CODE
DI_FS_GetDeviceFileName (DI_FS_StorDevIdx_t eDevIdx, HUINT8 *szFileName)
{
	if (szFileName == NULL)			{ return DI_ERR_ERROR; }

	//prevent : defect Fix / 2011.03.27 mhpakr3
	//VK_sprintf (szFileName, "sd%c", 'a' + eDevIdx - eDI_FS_DEV_sda);
	VK_snprintf ((char *)szFileName, DI_DEV_NODE_LEN ,"sd%c", 'a' + eDevIdx - eDI_FS_DEV_sda);
	return DI_ERR_OK;
}

DI_ERR_CODE
DI_FS_GetDeviceFileFullName (DI_FS_StorDevIdx_t eDevIdx, HUINT8 *szFileName)
{
	if (szFileName == NULL)			{ return DI_ERR_ERROR; }
	//prevent : defect Fix / 2011.03.29 mhpark3
	//VK_sprintf (szFileName, "/dev/sd%c", 'a' + eDevIdx - eDI_FS_DEV_sda);
	VK_snprintf ((char *)szFileName, DI_FS_DEVFILE_LEN,"/dev/sd%c", 'a' + eDevIdx - eDI_FS_DEV_sda);
	return DI_ERR_OK;
}

DI_ERR_CODE
DI_FS_GetPartitionFileName (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartitionIdx, HUINT8 *szFileName, HUINT8 nSizeOfFileName)
{
	if (szFileName == NULL)			{ return DI_ERR_ERROR; }
	//prevent : defect Fix / 2011.03.29 mhpark3
	//VK_sprintf (szFileName, "sd%c%d", 'a' + eDevIdx - eDI_FS_DEV_sda, ulPartitionIdx + 1);
	VK_snprintf ((char *)szFileName, nSizeOfFileName, "sd%c%d", 'a' + eDevIdx - eDI_FS_DEV_sda, ulPartitionIdx + 1);
	return DI_ERR_OK;
}

DI_ERR_CODE
DI_FS_GetPartitionFullFileName (DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartitionIdx, HUINT8 *szFileName)
{
	char		szKey[32];
	char		*pValue;
	FILE		*fp;
	char		devname[8];
	char		szBuf[256];
	char		*p;

	if (szFileName == NULL || eDevIdx >= eDI_FS_DEV_MAX || eDevIdx < eDI_FS_DEV_sda)	{
		return DI_ERR_ERROR;
	}

	VK_snprintf(szKey, sizeof(szKey)-1, "fs.encr.sd%c%d", eDevIdx+'a', ulPartitionIdx+1);
	pValue = getenv(szKey);
	if (pValue) {
		PrintDebug("[%s] Encrypted Partition!! (sd%c%d)\n", __FUNCTION__, eDevIdx+'a', ulPartitionIdx+1);
		VK_snprintf(szFileName, 31, "/dev/mapper/%s", pValue);
	}
	else {
		PrintDebug("[%s] Normal Partition!! (sd%c%d)\n", __FUNCTION__, eDevIdx+'a', ulPartitionIdx+1);

		/* we consider only the case of partition #1 */
		if (ulPartitionIdx == 0) {
			VK_snprintf(devname, sizeof(devname)-1, "sd%c%d", (char)('a'+eDevIdx-eDI_FS_DEV_sda), ulPartitionIdx+1);
			fp = fopen("/proc/partitions", "r");
			if (!fp) {
				PrintError("[%s] Error! can't open file\n", __FUNCTION__);
				return -1;
			}
			szBuf[sizeof(szBuf)-1] = 0;
			while(fgets(szBuf, sizeof(szBuf)-1, fp)) {
				/* check target partition device exists*/
				p = VK_strstr((const char*)szBuf, (const char*)devname);
				if (p) break;
			}
			fclose(fp);

			if (p) {
				/* if exists */
				VK_snprintf ((char *)szFileName, DI_FS_DEVFILE_LEN, "/dev/sd%c%d", 'a' + eDevIdx - eDI_FS_DEV_sda, ulPartitionIdx + 1);
			}
			else {
				/* if not exists, mapping partition device to sda/sdb...directly */
				VK_snprintf ((char *)szFileName, DI_FS_DEVFILE_LEN, "/dev/sd%c", 'a' + eDevIdx - eDI_FS_DEV_sda);
			}
		}
		else {
			VK_snprintf ((char *)szFileName, DI_FS_DEVFILE_LEN, "/dev/sd%c%d", 'a' + eDevIdx - eDI_FS_DEV_sda, ulPartitionIdx + 1);
		}
	}

	PrintDebug("[DI_FS_GetPartitionFullFileName] szFileName: %s \n", szFileName);
	return DI_ERR_OK;
}

/*
 * directory operation
 */
DI_FS_BOOL
DI_FS_IsDirExist(const HCHAR *dirname)
{
	struct stat st;
	return (0 == stat(dirname, &st) && S_ISDIR(st.st_mode)) ? TRUE : FALSE;
}

DI_ERR_CODE
DI_FS_Mkdir(const HCHAR *dirname)
{
	HCHAR *odirname, *p;

	PrintEnter();

	odirname = p = strdup(dirname);
	do {
		p = VK_strchr(p + 1, '/');
		if (p)
			*p = '\0';

			if (access(odirname, F_OK) == -1) {
				if (mkdir(odirname, 0755) == -1) {
					PrintError("fail to make a %s. %s\n", odirname, strerror(errno));
					VK_free(odirname);
					return DI_ERR_ERROR;
				}
			}
			if (p)
					*p = '/';
	} while (p);
	PrintDebug("mkdir:%s\n", odirname);

	VK_free(odirname);
	PrintExit();
	return DI_ERR_OK;
}

DI_FS_BOOL
DI_FS_HDDExist(const HCHAR *pucDeviceName)
{
	struct stat st;
	HCHAR line[256];
	HCHAR devname[80];
	HCHAR mountdir[80];
	HCHAR fstype[12];
	FILE *fp;

	PrintEnter();

	if (fp = fopen("/proc/mounts", "r"), NULL == fp) {
		PrintDebug("cannot read \"/proc/mounts\" %d\n", 0);
		return FALSE;
	}
	while (fgets(line, 255, fp)) {
		VK_sscanf(line, "%80s%80s%12s", devname, mountdir, fstype);
		if ((0 == VK_strcmp(pucDeviceName, mountdir)) &&
			  (0 == stat(devname, &st)) && S_ISBLK(st.st_mode)) {
			  	fclose(fp);
			  	return TRUE;
		}
	}
	fclose(fp);
	PrintExit();
	return FALSE;
}

DI_FS_BOOL
DI_FS_IsFileExist(const HCHAR *pathname)
{
	struct stat st;
	return (0 == stat(pathname, &st) && S_ISREG(st.st_mode)) ? TRUE : FALSE;
}

#if defined(CONFIG_NTFS_RW) || defined(CONFIG_XFS)
STATIC DI_ERR_CODE
local_difs_Read_SuperBlock(const HUINT8 *dev, HUINT32 offset, HUINT32 size, HUINT8 *buf)
{
	int 					 	nFD;

	nFD = open((char *)dev, O_RDONLY);
	if (nFD == -1) {
		PrintError("[local_difs_Read_SuperBlock] Device File (%s) open fail \n", dev);
		return DI_ERR_ERROR;
	}

	if (lseek(nFD, offset, SEEK_SET) == -1) {
		close(nFD);
		PrintError("[local_difs_Read_SuperBlock] cannot seek SB. %d\n", __LINE__);
		return DI_ERR_ERROR;
	}

	if (read(nFD, buf, size) != (ssize_t)size) {
		close(nFD);
		return DI_ERR_ERROR;
	}

	close(nFD);
	return DI_ERR_OK;
}
#endif

#if defined(CONFIG_NTFS_RW)
DI_ERR_CODE
DI_FS_GetVolumeNtfsUuid(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *buf, HUINT32 size)
{
	char					szDevName[32];
	DI_ERR_CODE		 hErr = DI_ERR_ERROR;

	if (buf == NULL || size <= NTFS_UUID_IN_SUPERBLOCK_SIZE)
	{
		PrintError("invalid function param. (%s)\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	DI_FS_GetPartitionFullFileName (eDevIdx, ulPartIdx, (HUINT8 *)szDevName);

	hErr = local_difs_Read_SuperBlock((HUINT8 *)szDevName, NTFS_UUID_IN_SUPERBLOCK_OFFSET, NTFS_UUID_IN_SUPERBLOCK_SIZE, buf);
	if (hErr!=DI_ERR_OK)
	{
		PrintError("local_difs_Read_SuperBlock fail. (%s)\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	buf[NTFS_UUID_IN_SUPERBLOCK_SIZE] = '\0';

	return DI_ERR_OK;
}
#endif

#if defined(CONFIG_XFS)
DI_ERR_CODE
DI_FS_GetVolumeXfsUuid(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *buf, HUINT32 size)
{
	char					szDevName[32];
	DI_ERR_CODE		 		hErr = DI_ERR_ERROR;

	if (buf == NULL || size <= XFS_UUID_IN_SUPERBLOCK_SIZE)
	{
		PrintError("invalid function param. (%s)\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	DI_FS_GetPartitionFullFileName (eDevIdx, ulPartIdx, (HUINT8 *)szDevName);

	hErr = local_difs_Read_SuperBlock((HUINT8 *)szDevName, XFS_UUID_IN_SUPERBLOCK_OFFSET, XFS_UUID_IN_SUPERBLOCK_SIZE, buf);
	if (hErr!=DI_ERR_OK)
	{
		PrintError("local_difs_Read_SuperBlock fail. (%s)\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	buf[XFS_UUID_IN_SUPERBLOCK_SIZE] = '\0';

	return DI_ERR_OK;
}
#endif
