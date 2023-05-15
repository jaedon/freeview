
/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */
#include <hlib.h>


#if defined(WIN32)
#else
#include <mntent.h>
#include <sys/vfs.h>
#include <sys/mount.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/statvfs.h>
#include <sys/stat.h>
#include <unistd.h>

//#include <sys/ioctl.h>
//#include <sys/vfs.h>
//#include <sys/mount.h>
//#include <sys/types.h>
//#include <linux/fs.h>
//#include <mntent.h>
//#include <unistd.h>
#include <pwd.h>
//#include <errno.h>

//#include "vkernel.h"
#endif


/* *************************************************************************************************** */
#define ___GLOBAL_HEADER_________________________________________________________________________________
/* *************************************************************************************************** */



/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */

#define DOT_OR_DOTDOT(_basename)					\
	((_basename[0] == '.') &&					\
	 (_basename[1] == '\0' || (_basename[1] == '.' && _basename[2] == '\0')))

#define ISSLASH(_c)		('/' == (_c))

#if defined(WIN32)
#define BLKSSZGET			0
#define BLKGETSIZE64		0
#else
#define BLKSSZGET			_IO(0x12,104)
#define BLKGETSIZE64		_IOR(0x12,114,size_t)
#endif

/* partition related */
#define MAX_SEC_SIZE		2048
#define MIN_SEC_SIZE		512
#define MMC_PORT_MAX 2

/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */


/* *************************************************************************************************** */
#define ___STATIC_VARIABLES______________________________________________________________________________
/* *************************************************************************************************** */


/* *************************************************************************************************** */
#define ___PUBLIC_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */


INLINE STATIC HUINT64 hlib_storage_GetKbyteFromStatfs (unsigned long ulNumBlock, unsigned long ulBlockSize)
{
	// 반올림 계산을 한다.
	return ((HUINT64)ulNumBlock * (HUINT64)ulBlockSize + 1024/2) / 1024;
}

HERROR	hlib_storage_GetPathSize(HCHAR *pszPath, HUINT64 *pullSize)
{
#if defined(WIN32)
	// TODO::
	return ERR_FAIL;
#else
	struct stat st;
	HUINT64 ullSum = 0, ullSize = 0;

	if (-1 == lstat(pszPath, &st))
	{
		return ERR_FAIL;
	}

	if (S_ISREG(st.st_mode))
		ullSum = st.st_size;

	if (S_ISLNK(st.st_mode)) {
#ifdef orignal_size
		/* XXX link file size or original file size? */
		if (-1 == stat(pszPath, &st))
		{
			return ERR_FAIL;
		}
#endif
		ullSum = st.st_size;
	}

	if (S_ISDIR(st.st_mode)) {
		DIR *dp;
		struct dirent *dirent;
		int nLen;
		char *pszFullPath;

		if (dp = opendir(pszPath), NULL == dp)
			return ERR_FAIL;

		ullSum = st.st_size;
		while ((dirent = readdir(dp)) != NULL) {
			if (DOT_OR_DOTDOT(dirent->d_name))
				continue;

			nLen = strlen(pszPath) + strlen(dirent->d_name) + 2;

			pszFullPath = HLIB_STD_MemCalloc(nLen);
			if (pszFullPath == NULL) 	continue;

			HLIB_PATH_Join(pszPath, dirent->d_name, pszFullPath, nLen);

			hlib_storage_GetPathSize(pszFullPath, &ullSize);
			ullSum += ullSize;

			HLIB_MEM_Free(pszFullPath);
		}

		closedir(dp);
	}

	if (pullSize) *pullSize = ullSum;

	return ERR_OK;
#endif
}

static int hlib_storage_CheckSuperUser(void)
{
#if defined(WIN32)
	// TODO::
	return 0;
#else
	struct passwd *pw = NULL;
	uid_t uid = getuid();

	if (uid != 0) return 0;

	pw = getpwuid(uid);
	if (pw == NULL) return 0;

	if (pw->pw_name && strncmp(pw->pw_name, "root", 4) == 0) {
		return 1;
	}

	return 0;
#endif
}

// From DI_MMC_GetMMCInfo()
HERROR HLIB_STORAGE_GetMMCSize(HUINT32 ulPortNum, HCHAR *pszMountPath, HUINT64 *pullTotalSizeKb, HUINT64 *pullUsedSize, HUINT64 *pullAvailableSize)
{
#if defined(WIN32)
	// TODO::
	return ERR_FAIL;
#else
	FILE			*fp;
	int				blocks;
	int				sectorsize;
	char			strbuffer[32];
	int				fd;
	struct statfs	tStatFs;
	struct mntent 	*tMntEnt;
	HUINT32	ulTotalKb, ulUsedKb, ulAvailableKb;

	ulTotalKb = ulUsedKb = ulAvailableKb = 0;

	if (ulPortNum >= MMC_PORT_MAX) {
		return ERR_FAIL;
	}

	HxSTD_memset(strbuffer, 0, sizeof(strbuffer));
	HxSTD_snprintf(strbuffer, sizeof(strbuffer)-1, "/dev/mmcblk%d", ulPortNum - 1);
	fd = open(strbuffer, O_RDONLY);
	if (fd < 0 ) {
		HxLOG_Error("[%s] Can't open device file (%s)\n", __FUNCTION__, strbuffer);
		return ERR_FAIL;
	}
	if (ioctl(fd, BLKGETSIZE, &blocks) < 0) {
		HxLOG_Error("[%s] Can't get blocks (%s)\n", __FUNCTION__, strbuffer);
		close(fd);
		return ERR_FAIL;
	}
	if (ioctl(fd, BLKSSZGET, &sectorsize) < 0) {
		HxLOG_Error("[%s] Can't get sector size (%s)\n", __FUNCTION__, strbuffer);
		close(fd);
		return ERR_FAIL;
	}
	close(fd);

	ulTotalKb = (HUINT32)((blocks/1024) * sectorsize);

	if (!pszMountPath || HxSTD_StrEmpty(pszMountPath))
	{
		if (pullTotalSizeKb)	{*pullTotalSizeKb = (HUINT64)ulTotalKb; }
		return ERR_OK;
	}

	fp = setmntent("/proc/mounts", "r");
	if (!fp) return ERR_OK;

	while(1) {
		tMntEnt = getmntent(fp);
		if (!tMntEnt) break;
		if (strstr(tMntEnt->mnt_dir, pszMountPath)) {
			/* Mounted */
			if (statfs(pszMountPath, &tStatFs) == 0) {
				if (hlib_storage_CheckSuperUser() > 0) {
					/* super user */
					ulAvailableKb = (HUINT32)((tStatFs.f_bfree*tStatFs.f_bsize + 1023)/1024); //in KB
					ulUsedKb = (HUINT32)(((tStatFs.f_blocks - tStatFs.f_bfree)*tStatFs.f_bsize + 1023)/1024); //in KB
				}
				else {
					ulAvailableKb = (HUINT32)((tStatFs.f_bavail*tStatFs.f_bsize + 1023)/1024); //in KB
					ulUsedKb = (HUINT32)(((tStatFs.f_blocks - tStatFs.f_bavail)*tStatFs.f_bsize + 1023)/1024); //in KB
				}
			}
			break;
		}
	}
	endmntent(fp);

	if (pullTotalSizeKb) 	{*pullTotalSizeKb = (HUINT64)ulTotalKb; }
	if (pullUsedSize) 		{*pullUsedSize = (HUINT64)ulUsedKb; }
	if (pullAvailableSize) 	{*pullAvailableSize = (HUINT64)ulAvailableKb; }

	return ERR_OK;
#endif
}

// From local_difs_OpenDeviceMbr()
// szDevName : ex. "/dev/sda"
HERROR HLIB_STORAGE_GetDevSize(HCHAR *szDevName, HUINT64 *pullTotalSizeKb)
{
#if defined(WIN32)
	// TODO::
	return ERR_FAIL;
#else
	int			 nFileDsc;
	// ioctl을 통해 얻는 정보들
	HUINT32		ulSecSize;
	HUINT64		 ullDiskSize;
	HULONG		 ulBlockNum;
	int nCnt = 0;

	for( nCnt = 0 ; nCnt < 3 ; nCnt++ )
	{
		nFileDsc = open ((char *)szDevName, O_RDONLY);
		if (nFileDsc == -1)
		{
			HxLOG_Error("Device File (%s) open fail \n", szDevName);
			HLIB_STD_TaskSleep(50);
		}
		else
		{
			break;
		}
	}
	if (nCnt == 3)
	{
		HxLOG_Error("Device File (%s) open fail \n", szDevName);
		return ERR_FAIL;
	}

	if (lseek (nFileDsc, 0LL, SEEK_SET) == -1)
	{
		HxLOG_Error("cannot seek MBR. %d\n", 0);
		close (nFileDsc);
		return ERR_FAIL;
	}

	if (ioctl (nFileDsc, BLKSSZGET, &ulSecSize) == -1)
	{
		ulSecSize = MIN_SEC_SIZE;
	}

	// Disk Size 값을 알아오는 방법:
	// http://www.joinc.co.kr/modules/moniwiki/wiki.php/Code/C/GetDiskSize
	if (ioctl (nFileDsc, BLKGETSIZE64, &ullDiskSize) == -1)
	{
		if (ioctl (nFileDsc, BLKGETSIZE, &ulBlockNum) == -1)
		{
			ulBlockNum = 0;
		}

		ullDiskSize = ((unsigned long long)ulBlockNum / 1024) * ulSecSize;
	}

	if (pullTotalSizeKb)	*pullTotalSizeKb = ullDiskSize / 1024;

	close(nFileDsc);

	return ERR_OK;
#endif
}

//szMountPath : ex. "/media/drive1/
HERROR HLIB_STORAGE_GetPartitionSize (HCHAR *szMountPath, HUINT64 *pullTotalSize, HUINT64 *pullUsedSize, HBOOL *pbWriteEnable)
{
#if defined(WIN32)
	// TODO::
	return ERR_FAIL;
#else
	char			*szMtabFilePath = "/proc/mounts";		/* It could be '/etc/mtab' under the some of RFS */
	char			*szDevicePath, *szMountedPath;
	HBOOL			 bFound = FALSE, bWriteEnabled = TRUE;
	HUINT64			 ullTotalSize = 0, ullUsedSize = 0;
	FILE			*pstMntTblFile = NULL;
	struct mntent 	*pstMntEntry = NULL;
	struct statfs	 stStatFs;
	HLONG 			lfreeBlkCnt;

	if (NULL == szMountPath)
	{
		HxLOG_Error("Error \n");
		return ERR_FAIL;
	}

	pstMntTblFile = setmntent (szMtabFilePath, "r");
	if (!pstMntTblFile)
	{
		HxLOG_Error("setmntent err\n");
		return ERR_FAIL;
	}

	HxLOG_Debug("[%s:%04d] szPath [%s] >>>\n", __FUNCTION__, __LINE__, szMountPath);

	while (bFound != TRUE)
	{
		if (pstMntTblFile)
		{
			pstMntEntry = getmntent (pstMntTblFile);
			if (!pstMntEntry)
			{
				endmntent (pstMntTblFile);
				break;
			}
		}

		szDevicePath	= pstMntEntry->mnt_fsname;
		szMountedPath	= pstMntEntry->mnt_dir;

		if (NULL != strstr (szDevicePath, szMountPath) || NULL != strstr (szMountedPath, szMountPath))
		{
			HxLOG_Debug("[%s:%04d] szDevicePath [%s], szMountedPath [%s] >>>\n", __FUNCTION__, __LINE__, szDevicePath, szMountedPath);

			// http://teamblog.joinc.co.kr/yundream/110
			// statfs() 설명 참조
			if (statfs (szMountedPath, &stStatFs) == 0)
			{
				// File System 내 여유공간은 f_ffree이고, 비 수퍼유저를 위한 여유 블럭은 f_bavail 이다.
				// 비 수퍼유저 권한으로 쓸 수 있는 공간 사이즈를 구해야 하므로 기존 코드에서 f_ffree로 된 것이
				// f_bavail로 바뀌어 있어야 한다.
				ullTotalSize	+= hlib_storage_GetKbyteFromStatfs (stStatFs.f_blocks, stStatFs.f_bsize);

				// 현재 STB의 실행 모드를 검사하여 사용 가능한 사이즈를 구한다.
				if (hlib_storage_CheckSuperUser() > 0) {
					lfreeBlkCnt = stStatFs.f_bfree;		// file system 내 free blocks
				}
				else {
					lfreeBlkCnt = stStatFs.f_bavail; 	// non-super user의 free blocks
				}

				ullUsedSize 	+= hlib_storage_GetKbyteFromStatfs (stStatFs.f_blocks - lfreeBlkCnt, stStatFs.f_bsize);
			}

			// Write Enable/Disable
			if (strstr(pstMntEntry->mnt_opts, "rw"))
			{
				bWriteEnabled = TRUE;
			}
			else if (strstr(pstMntEntry->mnt_opts, "ro"))
			{
				bWriteEnabled = FALSE;
			}

			if (ullTotalSize != 0)
			{
				endmntent (pstMntTblFile);
				bFound = TRUE;
			}
		}
	}

	if (bFound == TRUE)
	{
		if(pullTotalSize != NULL)	*pullTotalSize	= ullTotalSize;
		if(pullUsedSize != NULL)	*pullUsedSize	= ullUsedSize;
		if(pbWriteEnable != NULL)	*pbWriteEnable	= bWriteEnabled;

		return ERR_OK;
	}

	HxLOG_Error("Error (szMountPath:%s) \n", szMountPath);
	return ERR_FAIL;
#endif
}

// From PAL_FS_Estspace()
HERROR	HLIB_STORAGE_GetPathSize(HCHAR *pszPath, HUINT64 *pullSize)
{
	return hlib_storage_GetPathSize(pszPath, pullSize);
}


