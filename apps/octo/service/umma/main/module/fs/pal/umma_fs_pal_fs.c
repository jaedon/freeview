/**
	@file     	pal_fs.c
	@brief    	implements file I/O operations.

	Description: 	handles file I/O.	\n
	Module: 		pal/fs			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif

#if 1
	#define	CONFIG_AVOID_RECORD_OVERFLOW
#endif

#if defined(CONFIG_PROD_HDRFOXT2) || defined(CONFIG_PROD_HDR1000S) || defined(CONFIG_PROD_HB1000S)
#define SUPPORT_DECRYPT_COPY
#endif

#if defined(WIN32)
#define O_DIRECT	0x01
#define O_LARGEFILE	0x02
#endif

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <htype.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/statvfs.h>
#include <sys/mount.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>
#if !defined(WIN32)
#include <linux/hdreg.h>
#endif
#include <mntent.h>
#include <sys/vfs.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <limits.h>
#include <vkernel.h>

#include "umma_fs_pal_fs.h"

#include "di_err.h"
#include <di_crypt.h>
#include <di_pvr.h>
#include <di_hdd.h>
#include <di_fs.h>

#include <umma_int.h>
#include "se_uapi.h"

/* for ugly API */
#ifndef UNUSED
#define UNUSED(_x)		((void)(_x))
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef PATH_MAX
#define PATH_MAX			1024
#endif

/* see the linux/fs.h */
#define BLKSSZGET		_IO(0x12,104)
#define BLKGETSIZE64		_IOR(0x12,114,size_t)

#define FILE_MODE		(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define DIR_MODE		(FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)
#define ISSLASH(_c)		('/' == (_c))
#define DOT_OR_DOTDOT(_basename)					\
	((_basename[0] == '.') &&					\
	 (_basename[1] == '\0' || (_basename[1] == '.' && _basename[2] == '\0')))
#define ISEVENT(_x)		(PAL_FS_EVENT_MAX_NUMBER > (_x) && PAL_FS_EVENT_BASE <= (_x))
#define NEVENT			(PAL_FS_EVENT_MAX_NUMBER - PAL_FS_EVENT_BASE)
#define CALLBACK(_x)		(callback_func[(_x) - PAL_FS_EVENT_BASE])

#define IS_DOT_HIDDEN(_x)		((_x[0]=='.') && (_x[1]!='\0'))
#define DOT_OR_DOTDOT_OR_HIDDEN(_basename)					\
	((_basename[0] == '.') &&					\
	 ((_basename[1] == '\0') || (_basename[1] == '.' && _basename[2] == '\0')|| (_basename[1] != '\0')))

#define PAL_128_ENCRYPT_KEY_LEN			16

#define PAL_VALID_MAC_ADDR_LEN			6
#define PAL_VALID_SERIAL_NO_LEN			(PAL_128_ENCRYPT_KEY_LEN - PAL_VALID_MAC_ADDR_LEN)

#if defined(CONFIG_PROD_HDR1000S) || defined(CONFIG_PROD_HB1000S)
#define PAL_ACTUAL_ENCRYPT_KEY_LEN			14
#else
#define PAL_ACTUAL_ENCRYPT_KEY_LEN			PAL_128_ENCRYPT_KEY_LEN
#endif

#define MAX_SECURE_BUFFER 				(1024*1024*2)  // 2 MB
#define ALIGN_UNIT						(192)

#if defined(CONFIG_PROD_HDRFOXT2) || defined(CONFIG_PROD_HDR1000S) || defined(CONFIG_PROD_HB1000S)
#define FS_DECRYPT_COPY_PAGE_NUM	(6)				// optimal page number for copy thread buffer
#else
#define FS_DECRYPT_COPY_PAGE_NUM	(1)
#endif

#define FS_COPY_BUF_SIZE				((getpagesize() * FS_DECRYPT_COPY_PAGE_NUM))

#if (0)//defined(CONFIG_DROP_ROOT_PRIVILEGES)
#define ALLOW_DIR_PERMISSION			(0776)	//USER(rwx)Group(rwx)Other(rw-)

#define remove		PAL_SYS_Remove
#define opendir		PAL_SYS_OpenDir
#define chdir			PAL_SYS_ChDir
#define mkdir			PAL_SYS_MkDir
#define mknod		PAL_SYS_Mknod
#define rename		PAL_SYS_Rename
#if (0)
#define stat			PAL_SYS_Stat
#define lstat			PAL_SYS_Lstat
#endif
#endif

static HBOOL	s_bCryptInit = FALSE;
static HUINT8 	s_aucEncKey[DI_PVR_CRYPTO_KEY_LEN] = {0, };
// same value as s_aucPalSys_EncryptIV in pal_sys.c
static HUINT8	s_aucEncIv[DI_PVR_CRYPTO_KEY_LEN] = {0xad, 0xd6, 0x9e, 0xa3,0x89, 0xc8, 0x17, 0x72, 0x1e, 0xd4, 0x0e, 0xab,0x3d, 0xbc, 0x7a, 0xf2};
static DI_CRYPT_CryptMode s_etCryptMode = DI_CRYPT_TDES;


//static	stCrypt_t	_stDecrypt;


#define _____DEVICE_FILENAME_AND_DEVICE_INDEX_____


#define _______________
STATIC void	pal_fs_cmdInit (void);
STATIC int	pal_fs_deleteSync (int pttype, const char *pathname);

static void (*callback_func[NEVENT])(void *);

#ifdef CONFIG_DEBUG_FILEINFO
#include <sys/queue.h>

struct filedes_info {
	char *pathname;
	int filedes;
	int use_count;
	TAILQ_ENTRY(filedes_info) entries;
};
TAILQ_HEAD(listhead, filedes_info) head;

static pthread_mutex_t listlock = PTHREAD_MUTEX_INITIALIZER;
static volatile int opened;


static int
fileinfo_init(void)
{
	TAILQ_INIT(&head);
	return 0;
}

static int
fileinfo_add(const char *pathname, const int filedes)
{
	struct filedes_info *p;

	if (p = UMMA_Malloc(sizeof(*p)), NULL == p) {
		HxLOG_Error("cannot UMMA_Malloc: %s\n", strerror(errno));
		return -1;
	}
	memset(p, 0, sizeof(*p));

	p->pathname = pal_fs_strdup(pathname);
	p->filedes = filedes;
	p->use_count++;

	pthread_mutex_lock(&listlock);
	TAILQ_INSERT_TAIL(&head, p, entries);
	opened++;
	pthread_mutex_unlock(&listlock);

	return 0;
}

static int
fileinfo_del(const int filedes)
{
	struct filedes_info *p;

	pthread_mutex_lock(&listlock);
	for (p = head.tqh_first; p != NULL; p = p->entries.tqe_next) {
		if (filedes == p->filedes) {
			TAILQ_REMOVE(&head, p, entries);
			UMMA_Free(p->pathname);
			UMMA_Free(p);
			opened--;
			pthread_mutex_unlock(&listlock);
			return 0;
		}
	}
	pthread_mutex_unlock(&listlock);
	HxLOG_Error("no entry for %i\n", filedes);

	return -1;
}

void
PAL_FS_OpenFileList(void)
{
	struct filedes_info *p;
	int num = 0;

	pthread_mutex_lock(&listlock);
	HxLOG_Print("+Total: %i\n", opened);
	if (opened > 0) {
		HxLOG_Print("index count filedes pathname\n");
		for (p = head.tqh_first; p != NULL; p = p->entries.tqe_next) {
			HxLOG_Print("%5i %5i %7i %s\n",
			    num, p->use_count, p->filedes, p->pathname);
			num++;
		}
	}
	pthread_mutex_unlock(&listlock);
}
#else
#define fileinfo_init(...)	((void)0)
#define fileinfo_add(...)	((void)0)
#define fileinfo_del(...)	((void)0)
void
PAL_FS_OpenFileList(void)
{
	HxLOG_Print("set the CONFIG_DEBUG_FILEINFO\n");
}
#endif


#if defined(CONFIG_PROD_OS_EMULATOR)
#define timeval emu_timeval
#define timezone emu_timezone
#define stat emu_stat
#endif


static inline char *PAL_FS_Fullpath(const char *s1, const char *s2);

/*
 * initialize opreation
 */
int
PAL_FS_Init(void)
{
#if 0

#if defined(SUPPORT_DECRYPT_COPY)
	HUINT32			alignedSize = 0;
#if 0
	/* align buffer size */
	if ((FS_COPY_BUF_SIZE % 192) == 0)
		alignedSize = FS_COPY_BUF_SIZE;
	else
		alignedSize = (FS_COPY_BUF_SIZE / 192) * 192;
#else
	alignedSize = FS_COPY_BUF_SIZE;
#endif

	VK_memset32(&_stDecrypt, 0, sizeof(stCrypt_t));

	_stDecrypt.pSrcBuffer = DI_CRYPT_MemAlloc(alignedSize);
	if ( NULL == _stDecrypt.pSrcBuffer )
		HxLOG_Error("decrypt source buffer alloc fail.\n");

	_stDecrypt.pDstBuffer = DI_CRYPT_MemAlloc(alignedSize);
	if ( NULL == _stDecrypt.pDstBuffer )
		HxLOG_Error("decrypt dest buffer alloc fail.\n");

	_stDecrypt.etCryptMode = DI_CRYPT_TDES;
	_stDecrypt.etCryptKey = DI_CRYPT_USER_KEY;
	_stDecrypt.etCryptData = DI_CRYPT_STREAM;
	_stDecrypt.pKey = sEncKey;
	_stDecrypt.ulKeySize = PAL_128_ENCRYPT_KEY_LEN;	// 16 byte
	_stDecrypt.ulBuffSize = alignedSize;
#endif
#if defined(CONFIG_PROD_HDR1000S)
	// 14 Byte Key - Mac Address 6 Byte + Serial 8Byte ( Serial 하위 2Byte 사용 X)
	PAL_SYS_GetField(PAL_FLASH_FIELD_SERIAL_NO, (void *)sEncKey, PAL_128_ENCRYPT_KEY_LEN - PAL_VALID_MAC_ADDR_LEN);
	PAL_SYS_GetField(PAL_FLASH_FIELD_MAC_ADDR, (void *)(sEncKey + PAL_128_ENCRYPT_KEY_LEN - PAL_VALID_MAC_ADDR_LEN), PAL_VALID_MAC_ADDR_LEN);
#else
	/* HDR-FOX-T2: init content encryption key */
	PAL_SYS_GetField(PAL_FLASH_FIELD_MAC_ADDR, (void *)sEncKey, PAL_VALID_MAC_ADDR_LEN);
	PAL_SYS_GetField(PAL_FLASH_FIELD_SERIAL_NO, (void *)(sEncKey + PAL_VALID_MAC_ADDR_LEN), PAL_128_ENCRYPT_KEY_LEN - PAL_VALID_MAC_ADDR_LEN);
#endif
	memset(callback_func, 0, sizeof(*callback_func) * NEVENT);
	fileinfo_init();

#if defined(CONFIG_DEBUG)
	pal_fs_cmdInit();
#endif

#endif

	return ERR_OK;
}

int
PAL_FS_RegCallback(PAL_FS_UINT32 evt, void (*callback)(void *))
{
	if (!ISEVENT(evt) || NULL == callback)
		return PAL_FS_BAD_PARAMETER_ERR;

	CALLBACK(evt) = callback;
	return 0;
}


static void		PAL_FS_Callback(PAL_FS_UINT32 evt, unsigned int param1, unsigned int param2, unsigned int param3)
{
#if 0
	if (ISEVENT(evt) && CALLBACK(evt))
	{
		PalPvr_Msg_t msg;

		msg.ulSender = ePAL_PVR_FS;
		msg.ulEvent = evt;
		msg.ulParam1 = param1;
		msg.ulParam2 = param2;
		msg.ulParam3 = param3;

		(*CALLBACK(evt))((void *)&msg);
	}
#else

#endif
}

/*
 * file operation
 */
int		PAL_FS_Open(int pttype, const char *pathname, PAL_FS_UINT32 flags, PAL_FS_UINT32 *filedes)
{
	int fd = -1;
	int oflags = 0;

	mode_t mode = FILE_MODE;

	(void)pttype;

	if (flags & PAL_FS_MODE_READ)
	{
		oflags |= O_RDONLY;
	}
	else if (flags & PAL_FS_MODE_WRITE)
	{
		oflags |= O_WRONLY;
		oflags |= O_CREAT;
	}
	else if (flags & PAL_FS_MODE_RDWR)
	{
		oflags |= O_RDWR;
		oflags |= O_CREAT;
	}
	else
	{
		HxLOG_Error("invalid flags \"%s\": %li\n", pathname, flags);
		return ERR_FAIL;
	}

	if (flags & PAL_FS_MODE_APPEND)		oflags |= O_APPEND;
	if (flags & PAL_FS_MODE_TRUNC)		oflags |= O_TRUNC;

	if (flags & PAL_FS_MODE_O_DIRECT)	oflags |= O_DIRECT;
	if (flags & PAL_FS_MODE_O_LARGEFILE)	oflags |= O_LARGEFILE;

	if (flags & PAL_FS_MODE_NONBLOCK)	oflags |= O_NONBLOCK;

	if (fd = open(pathname, oflags, mode), fd < 0)
	{
		*filedes = 0;
		HxLOG_Error("cannot open \"%s\": %s\n", pathname, strerror(errno));
		if (ENOENT == errno || ENOTDIR == errno || EACCES == errno)
			return PAL_FS_FOLDER_NOT_EXIST_ERR;
		else if (EIO == errno)
			return PAL_FS_ERROR_IO;
		else if (EROFS == errno)
			return PAL_FS_READ_ONLY_ERR;		// read-only filesystem

		return PAL_FS_ERROR_NO_MORE_RESOURCES;
	}

	*filedes = fd;
	fileinfo_add(pathname, fd);

	return ERR_OK;
}

int
PAL_FS_Close(PAL_FS_UINT32 filedes)
{
	fileinfo_del((int)filedes);

	fsync((int)filedes);
	close((int)filedes);
	return ERR_OK;
}

int
PAL_FS_Read(PAL_FS_UINT32 filedes, PAL_FS_UINT8 *buf, PAL_FS_UINT32 size,
	    PAL_FS_UINT32 *sizep)
{
	ssize_t n;

	do {
		n = read((int)filedes, buf, size);
	} while (-1 == n && EINTR == errno);

	if (-1 == n) {
		*sizep = 0;
		HxLOG_Error("cannot read: %s\n", strerror(errno));
		return PAL_FS_ERROR_NO_MORE_RESOURCES;
	}

	*sizep = n;
	return ERR_OK;
}

int
PAL_FS_Write(PAL_FS_UINT32 filedes, const PAL_FS_UINT8 *buf, PAL_FS_UINT32 size,
	     PAL_FS_UINT32 *sizep)
{
	ssize_t n;

	do {
		n = write((int)filedes, buf, size);
	} while (-1 == n && EINTR == errno);

	if (-1 == n) {
		*sizep = 0;
		HxLOG_Error("cannot write: %s\n", strerror(errno));
		return PAL_FS_ERROR_NO_MORE_RESOURCES;
	}

	*sizep = n;
	return ERR_OK;
}

int
PAL_FS_LSeek(PAL_FS_UINT32 filedes, PAL_FS_INT64 offset, PAL_FS_UINT32 pos,
	     PAL_FS_INT64 *offsetp)
{
	off_t curr;
	int whence;

	if (pos == PAL_FS_POS_START)
		whence = SEEK_SET;
	else if (pos == PAL_FS_POS_END)
		whence = SEEK_END;
	else
		whence = SEEK_CUR;

	if (curr = lseek((int)filedes, offset, whence), -1 == curr)
	{
		*offsetp = 0;
		HxLOG_Error("cannot seek: %s\n", strerror(errno));
		return PAL_FS_ERROR_NO_MORE_RESOURCES;
	}

	*offsetp = curr;
	return ERR_OK;
}

int PAL_FS_Seek(PAL_FS_UINT32 filedes, PAL_FS_INT64 offset, PAL_FS_UINT32 pos, PAL_FS_BOOL iswrite)
{
	PAL_FS_INT64 curr;
	UNUSED(iswrite);
	return PAL_FS_LSeek(filedes, offset, pos, &curr);
}

int PAL_FS_Tell (PAL_FS_UINT32 filedes, PAL_FS_INT64 *pnOffset)
{
	PAL_FS_INT64	 nCurr;
	int				 nErr;

	if (pnOffset == NULL)
	{
		return ERR_FAIL;
	}

	nErr = PAL_FS_LSeek (filedes, 0L, PAL_FS_POS_CURRENT, &nCurr);
	if (nErr == ERR_OK)
	{
		*pnOffset = nCurr;
	}

	return nErr;
}

int
PAL_FS_SetDmaEnable(PAL_FS_UINT32 filedes, PAL_FS_BOOL enable)
{
	int flags;

	if (flags = fcntl((int)filedes, F_GETFL), -1 == flags) {
		HxLOG_Error("cannot get file prop: %s\n", strerror(errno));
		return PAL_FS_ERROR_NO_MORE_RESOURCES;
	}

	if (enable)
		flags |= O_DIRECT;
	else
		flags &= ~O_DIRECT;

	if (-1 == fcntl((int)filedes, F_SETFL, flags)) {
		HxLOG_Error("cannot set file prop: %s\n", strerror(errno));
		return PAL_FS_ERROR_NO_MORE_RESOURCES;
	}

	return ERR_OK;
}

int
PAL_FS_Truncate(PAL_FS_UINT32 filedes, PAL_FS_INT64 size)
{
	return (0 == ftruncate((int)filedes, size)) ? ERR_OK :
		PAL_FS_ERROR_NO_MORE_RESOURCES;
}


int
PAL_FS_GetFileInfo(const char *pathname, PAL_FS_UINT32 type,
		   PAL_FS_FileStat_t *filestatp)
{
	struct stat st;

	if (-1 == lstat(pathname, &st))
	{
		HxLOG_Error("cannot stat \"%s\": %s\n", pathname, strerror(errno));
		return PAL_FS_ERROR_NO_MORE_RESOURCES;
	}

	switch (st.st_mode & S_IFMT) {
	case S_IFREG:
		filestatp->fileType = PAL_FS_FILE_TYPE_FILE;
		break;
	case S_IFDIR:
		filestatp->fileType = PAL_FS_FILE_TYPE_DIR;
		break;
	case S_IFIFO:
		filestatp->fileType = PAL_FS_FILE_TYPE_FIFO;
		break;
	case S_IFLNK:
		filestatp->fileType = PAL_FS_FILE_TYPE_LINK;
		break;
	default:
		filestatp->fileType = PAL_FS_FILE_TYPE_UNKNOWN;
		break;
	}

	/*
	 * when the named file is symbolic link,
	 * return size about the symbolic link,
	 * not the file referenced by the symbolic link.
	 */
	filestatp->ulFileSize = st.st_size;

	filestatp->aTime = st.st_atime;		/* time of last access */
	filestatp->mTime = st.st_mtime;		/* time of last modification */
	filestatp->cTime = st.st_ctime;		/* time of last change */

	/* FIXME PAL_FS_INFO_XXX and FM_FS_LISTTYPE_XXX are not value
	 * for bitwise operation */
	if (PAL_FS_INFO_TIME == type) {
		struct timeval tv;
		struct timezone tz;
		int h, m, c;
		gettimeofday(&tv, &tz);
		h = tz.tz_minuteswest / 60;
		m = ((tz.tz_minuteswest / 15) % 4) * 15;
		c = ((60 * 60) * h) + (60 * m);

		filestatp->aTime = st.st_atime + c;
		filestatp->mTime = st.st_mtime + c;
		filestatp->cTime = st.st_ctime + c;
	}

	return ERR_OK;
}

int
PAL_FS_Rename(int pttype, const char *oldname, const char *newname)
{
	UNUSED(pttype);
	if (-1 == rename(oldname, newname)) {
		HxLOG_Error("cannot rename \"%s\" to \"%s\": %s",
		    oldname, newname, strerror(errno));
		return ERR_FAIL;
	}
	return ERR_OK;
}

int
PAL_FS_Move(int pttype, const char *oldname, const char *newname)
{
	int		err = ERR_OK;

	err = PAL_FS_Rename(pttype, oldname, newname);
	if (err == ERR_OK)
	{
		if (CALLBACK(PAL_FS_EVENT_MOVE_DONE))
			PAL_FS_Callback(PAL_FS_EVENT_MOVE_DONE, 0, 0, 0);
	}
	return err;
}

int
PAL_FS_MoveWithoutCallback(int pttype, const char *oldname, const char *newname)
{
	int		err = ERR_OK;

	err = PAL_FS_Rename(pttype, oldname, newname);

	return err;
}

/*
 * disk operation
 */
int
PAL_FS_GetDiskInfo(const char *pathname, PAL_FS_DiskStat_t *diskstatp)
{
	struct statvfs stfs;

	/* https://bugs.launchpad.net/ubuntu/+source/nautilus/+bug/133567
	 * In the case, vfat, statvfs() has a problem that is wasting
	 * a lot of time to access vfat drives at first time.
	 * so need to upgrade the kernel or
	 * patch the source code http://launchpadlibrarian.net/9899552/fat_optimize-count-freeclus.patch
	 */
	if (-1 == statvfs(pathname, &stfs)) {
		diskstatp->ulDiskTotalKByteSize = 0;
		diskstatp->ulDiskFreeKByteSize  = 0;
		HxLOG_Error("cannot statvfs \"%s\": %s", pathname, strerror(errno));
		return PAL_FS_ERROR_NO_MORE_RESOURCES;
	}

	/* XXX no binary prefix */
	diskstatp->ulDiskTotalKByteSize = (stfs.f_blocks * (long long)stfs.f_bsize) / 1000;
	diskstatp->ulDiskFreeKByteSize  = (stfs.f_bfree  * (long long)stfs.f_bsize) / 1000;

	return ERR_OK;
}




int
PAL_FS_IsDiskError(const char *dev_node)
{
	char line[256];
	char devname[80];
	int major, minor;
	unsigned long long blocks;
	FILE *fp;

	if (fp = fopen("/proc/partitions", "r"), NULL == fp) {
		HxLOG_Error("cannot read \"/proc/partitions\"\n");
		return PAL_FS_DISK_NOT_EXIST_ERR;
	}

	while (fgets(line, 255, fp)) {
		if (4 == sscanf(line, "%i%i%llu%s", &major, &minor, &blocks, devname) &&
		    0 == strcmp(devname, dev_node)) {
			fclose(fp);

			return ERR_OK;
		}
	}

	fclose(fp);
	return PAL_FS_DISK_NOT_EXIST_ERR;
}

int
PAL_FS_GoToStandby(void)
{
	/* XXX ATA control */
	HxLOG_Print("not support yet\n");
	return 0;
}

int
PAL_FS_WakeupFromStandby(void)
{
	/* XXX ATA control */
	HxLOG_Print("not support yet\n");
	return 0;
}

PAL_FS_BOOL
PAL_FS_IsHDDSecurityLocked(void)
{
	/* XXX ATA control */
	HxLOG_Print("not support yet\n");
	return FALSE;
}

int
PAL_FS_ResetSecurity(void)
{
	/* XXX ATA control */
	HxLOG_Print("not support yet\n");
	return ERR_OK;
}

int
PAL_FS_Ioctl(int pttype, int cmd, void *arg)
{
	UNUSED(pttype);

	switch (cmd) {
	case PAL_FS_CMD_GET_DISK_INFO:
	{
		PAL_FS_DiskInfo_t *p = arg;
		PAL_FS_DiskStat_t diskstat;

		if (PAL_FS_GetDiskInfo(".", &diskstat)) {
			p->ulTotalSpace = 0;
			p->ulFreeSpace  = 0;
		} else {
			/* XXX no binary prefix */
			p->ulTotalSpace = diskstat.ulDiskTotalKByteSize / 1000;
			p->ulFreeSpace  = diskstat.ulDiskFreeKByteSize  / 1000;
		}
		break;
	}

	case PAL_FS_CMD_GET_FILE_INFO:
	case PAL_FS_CMD_GET_FILE_INFO_INDIR:
	{
		PAL_FS_FileInfo_t *p = arg;
		struct stat st;
		int r;

		if ((PAL_FS_UINT32)PAL_FS_NULL_FILE_HANDLE == p->unFileHandle)
		{
			r = stat((char *)p->szFileName, &st);
		}
		else
		{
			r = fstat((int)p->unFileHandle, &st);
		}

		if (-1 == r && !S_ISREG(st.st_mode)) {
			p->ulFileSize = 0;
			HxLOG_Error("cannot get filesize\"%s\": %s\n", p->szFileName, strerror(errno));
			return PAL_FS_ERROR_NO_MORE_RESOURCES;
		}

		p->ulFileSize = st.st_size;
		break;
	}

	case PAL_FS_CMD_RENAME:
	{
		PAL_FS_Rename_t *p = arg;
		char *oldname, *newname;

		oldname = (char *)p->aucSrcName;
		newname = (char *)p->aucDestName;
		while (ISSLASH(*oldname))
			oldname++;
		while (ISSLASH(*newname))
			newname++;
		return PAL_FS_Rename(0, oldname, newname);
	}

	default:
		HxLOG_Error("unknown command: %i\n", cmd);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/*
 * directory operation
 */
int
PAL_FS_OpenDir(int pttype, const char *dirname, PAL_FS_UINT32 *dirp)
{
	DIR *dp;

	UNUSED(pttype);

	if (dp = opendir(dirname), NULL == dp) {
		*dirp = 0;
		HxLOG_Error("cannot opendir \"%s\": %s\n", dirname, strerror(errno));
		return ERR_FAIL;
	}

	*dirp = (PAL_FS_UINT32)dp;
	return ERR_OK;
}

int
PAL_FS_CloseDir(PAL_FS_UINT32 dirp)
{
	closedir((DIR *)dirp);
	return ERR_OK;
}

int
PAL_FS_ReadDir(PAL_FS_UINT32 dirp, PAL_FS_DirEntry_t *entry)
{
	struct dirent const *dirent;

	if (dirent = readdir((DIR *)dirp), NULL != dirent) {
		if (!DOT_OR_DOTDOT(dirent->d_name)) {
			strncpy((char *)entry->aucName, dirent->d_name, NAME_MAX-1);
			entry->bFlagDir = 0;
			entry->ulFileSize = 0;
		}
	}
	else
		return ERR_FAIL;

	return ERR_OK;
}


int
PAL_FS_ChangeDir(const char *dirname)
{
	if (chdir(dirname) < 0) {
		HxLOG_Error("cannot chdir \"%s\": %s\n", dirname, strerror(errno));
		return PAL_FS_ERROR_INVALID_VALUE;
	}
	return ERR_OK;
}

static int
PAL_FS_Selector(const struct dirent *dirent, const char *filterstr,
	       unsigned int filtertype)
{
	if (0 == strcmp(dirent->d_name, "lost+found"))
		return 0;

	/* FIXME : How shall I do on dot or dotdot hidden directories */
#if 0
	if ('.' == dirent->d_name[0] && (filtertype != PAL_FS_INFO_ALLLIST))
		return 0;
#endif

	if ((NULL == filterstr) ||
	    (filterstr && NULL != strstr(dirent->d_name, filterstr)))
		return 1;
	else
		return 0;
}

static int
PAL_FS_Dirscan(const char *dirname, struct dirent ***namelist,
	    const char *filterstr, unsigned int filtertype, int(*compar)(const void *, const void *))
{
	DIR *dp = NULL;
	struct dirent *dirent, **direntp = NULL;
	size_t entrysize;
	int pos = 0;
	int ulDirentSize = 0;

	if (dp = opendir(dirname), NULL == dp)
		return -1;

	while ((dirent = readdir(dp)) != NULL)
	{
		if (PAL_FS_Selector(dirent, filterstr, filtertype)) {
			if (pos >= ulDirentSize)
			{
				struct dirent **new_direntp = NULL;

				new_direntp = UMMA_Malloc ((pos + 1024) * sizeof(struct dirent *));
				if (NULL == new_direntp)
				{
					goto err_free_list;
				}

				if (NULL != direntp)
				{
					VK_memcpy (new_direntp, direntp, pos * sizeof(struct dirent *));
					UMMA_Free (direntp);
				}

				direntp = new_direntp;
				ulDirentSize = pos + 1024;
			}

			entrysize = offsetof(struct dirent, d_name) + strlen(dirent->d_name) + 1;
			if (direntp[pos]= UMMA_Malloc(entrysize),
			    NULL == direntp[pos])
				goto err_free_list;

			memcpy(direntp[pos], dirent, entrysize);
			pos++;
		}
	}

	closedir(dp);

	if (compar && direntp)
		qsort(direntp, (size_t)pos, sizeof(struct dirent *), compar);

	*namelist = direntp;
	return pos;

err_free_list:
	closedir(dp);
	if (direntp) {
		while (pos > 0)
			UMMA_Free(direntp[--pos]);
		UMMA_Free(direntp);
	}
	*namelist = NULL;
	return -1;
}

static int
PAL_FS_SelectorExt(const struct dirent *dirent, char *filterstrarray[], unsigned int filtertype)
{
	int		i, len_filename, len_ext;

	if (0 == strcmp(dirent->d_name, "lost+found"))
		return 0;

	if(filterstrarray == NULL)
	{
		return 1;
	}
	for (i = 0; filterstrarray[i] != NULL; i++)
	{
		len_filename = strlen(dirent->d_name);
		len_ext = strlen(filterstrarray[i]);
		if (len_filename < len_ext)
			continue;
		if (strcasecmp(dirent->d_name + (len_filename - len_ext), filterstrarray[i]) == 0)
			return 1;
	}
	return 0;
}


static int
PAL_FS_Dirscan2(const char *dirname, struct dirent ***namelist,
	    	char *filterstrarray[], unsigned int filtertype, int(*compar)(const void *, const void *))
{
	DIR *dp = NULL;
	struct dirent *dirent, **direntp = NULL;
	size_t entrysize;
	int pos = 0;
	char filename[NAME_MAX];
	struct stat status;
	HUINT32	ulDirLen;
	HUINT32	ulDirentSize = 0;

	if (dp = opendir(dirname), NULL == dp)
		return -1;

	ulDirLen = strlen(dirname);
	while ((dirent = readdir(dp)) != NULL)
	{
		memset(filename, 0, sizeof(char) * NAME_MAX);
		if(ulDirLen > 0 && dirname[ulDirLen - 1] == '/')
		{
			snprintf(filename, NAME_MAX, "%s%s", dirname, dirent->d_name);
		}
		else
		{
			snprintf(filename, NAME_MAX, "%s/%s", dirname, dirent->d_name);
		}

		if(lstat(filename, &status) == 0)
		{
			// fileter should be applied on regular files
			if ((S_ISDIR(status.st_mode) && !DOT_OR_DOTDOT_OR_HIDDEN(dirent->d_name)) ||
				(PAL_FS_SelectorExt(dirent, filterstrarray, filtertype) && S_ISREG(status.st_mode)))
			{
				if (pos >= ulDirentSize)
				{
					struct dirent **new_direntp = NULL;

					new_direntp = UMMA_Malloc ((pos + 1024) * sizeof(struct dirent *));
					if (NULL == new_direntp)
					{
						goto err_free_list;
					}

					if (NULL != direntp)
					{
						VK_memcpy (new_direntp, direntp, pos * sizeof(struct dirent *));
						UMMA_Free (direntp);
					}

					direntp = new_direntp;
					ulDirentSize = pos + 1024;
				}

				entrysize = offsetof(struct dirent, d_name) + strlen(dirent->d_name) + 1;
				if (direntp[pos]= UMMA_Malloc(entrysize),
				    NULL == direntp[pos])
					goto err_free_list;

				memcpy(direntp[pos], dirent, entrysize);
				pos++;
			}
		}
	}

	closedir(dp);

	if (compar)
		qsort(direntp, (size_t)pos, sizeof(struct dirent *), compar);

	*namelist = direntp;
	return pos;

err_free_list:
	closedir(dp);
	if (direntp) {
		while (pos > 0)
			UMMA_Free(direntp[--pos]);
		UMMA_Free(direntp);
	}
	*namelist = NULL;
	return -1;
}

static int
PAL_FS_Dirfree(int pos, struct dirent **namelist)
{
	if (namelist) {
		while (pos > 0)
			UMMA_Free(namelist[--pos]);
		UMMA_Free(namelist);
	}
	return 0;
}

int
PAL_FS_CountFile(const char *pathname, char *filterstrarray[], unsigned int *file_num)
{
	struct stat st;
	int i = 0;

	if (-1 == lstat(pathname, &st))
	{
		HxLOG_Error("cannot stat \"%s\": %s\n", pathname, strerror(errno));
		return ERR_FAIL;
	}

	if (S_ISDIR(st.st_mode))
	{
		DIR *dp;
		struct dirent *dirent;
		char *fullpath;

		if (dp = opendir(pathname), NULL == dp)
			return 0;

		while ((dirent = readdir(dp)) != NULL) {
			if (DOT_OR_DOTDOT(dirent->d_name))
				continue;

			if (fullpath = PAL_FS_Fullpath(pathname, dirent->d_name),
			    NULL == fullpath)
				continue;

			PAL_FS_CountFile(fullpath, filterstrarray, file_num);

			UMMA_Free(fullpath);
		}

		closedir(dp);
	}
	else
	{
		// check file's extension, and count
		while (filterstrarray[i] != NULL)
		{
			if (filterstrarray[i] && NULL != strstr(pathname, filterstrarray[i]))
			{
				(*file_num)++;
				break;
			}
			i++;
		}
	}

	return ERR_OK;
}

int
PAL_FS_FindFile(const char *pathname, char *filterstrarray[], unsigned int *file_num, unsigned int *dir_num)
{
	struct stat st;
	int i = 0, num_file = 0, num_dir = 0;

	if (-1 == lstat(pathname, &st))
	{
		HxLOG_Error("cannot stat \"%s\": %s\n", pathname, strerror(errno));
		return ERR_FAIL;
	}

	if (S_ISDIR(st.st_mode))
	{
		DIR *dp;
		struct dirent *dirent;
		char *fullpath;

		if (dp = opendir(pathname), NULL == dp)		// empty directory
			return 0;

		while ((dirent = readdir(dp)) != NULL)
		{
			if (DOT_OR_DOTDOT(dirent->d_name))
				continue;

			if (fullpath = PAL_FS_Fullpath(pathname, dirent->d_name),
			    NULL == fullpath)
				continue;

			i = 0;

			if (lstat(fullpath, &st) == 0)
			{

				/* count # of directories */
				if (S_ISDIR(st.st_mode))
					num_dir++;

				/* count # of files having specific extention */
				if (S_ISREG(st.st_mode))
				{
					while (filterstrarray[i] != NULL)
					{
						if (filterstrarray[i] && NULL != strstr(fullpath, filterstrarray[i]))
						{
							num_file++;
							break;
						}
						i++;
					}
				}
			}

			UMMA_Free(fullpath);
		}

		closedir(dp);
	}

	*file_num = num_file;
	*dir_num = num_dir;

	return ERR_OK;
}

typedef int (*compFuncPointer)(const void *, const void *);

int myalphasort(const void * a, const void * b)
{
    return strcasecmp((*(const struct dirent **)a)->d_name,
            			(*(const struct dirent **)b)->d_name);
}

int
PAL_FS_ScanDir(int pttype, const char *dirname, PAL_FS_UINT8 *filterstr[],
	       PAL_FS_UINT32 *num, void ***namelistp)
{
	struct dirent **direntp;
	int n;

	UNUSED(pttype);
	if (n = PAL_FS_Dirscan2(dirname, &direntp, (char **)filterstr, PAL_FS_INFO_NAMETYPE, myalphasort), n < 0) {
		*num = 0;
		*namelistp = NULL;
		HxLOG_Error("cannot scan dir \"%s\"\n", dirname);
		return ERR_FAIL;
	} else {
		*num = n;
		*namelistp = (void *)direntp;
		return ERR_OK;
	}
}

int
PAL_FS_ReleaseScanDir(PAL_FS_UINT32 num, void **namelist)
{
	PAL_FS_Dirfree((int)num, (struct dirent **)namelist);
	return ERR_OK;
}

int
PAL_FS_GetScanDir(PAL_FS_UINT32 pos, void **namelist, PAL_FS_UINT8 *name)
{
	strncpy((char *)name, ((struct dirent **)namelist)[pos]->d_name, NAME_MAX);
	return ERR_OK;
}

int
PAL_FS_GetDirInfo(const char *dirname, PAL_FS_UINT8 *filterstr, PAL_FS_UINT32 filtertype,
		  PAL_FS_DirStat_t *dirstatp)
{
	PAL_FS_FileStat_t *filestat, *p;
	struct dirent **direntp;
	char fullpath[PATH_MAX];
	int n;
	int i;

	if (n = PAL_FS_Dirscan(dirname, &direntp, (char *)filterstr, filtertype, (compFuncPointer)myalphasort), n < 0)
		return ERR_FAIL;

	if (0 == n) {
		dirstatp->fileNumber = 0;
		dirstatp->s_astfilestats = NULL;
		return ERR_OK;
	}

	if (filestat = UMMA_Malloc(sizeof(*filestat) * n), NULL == filestat) {
		PAL_FS_Dirfree(n, direntp);
		return PAL_FS_ERR_OUTOFMEMORY;
	}

	for (i = 0, p = filestat; i < n; i++, p++) {
		strncpy((char *)p->aucFileName, direntp[i]->d_name, NAME_MAX-1);
		snprintf(fullpath, PATH_MAX, "%s/%s", dirname, direntp[i]->d_name);
		PAL_FS_GetFileInfo(fullpath, filtertype, p);
	}

	dirstatp->fileNumber = n;
	dirstatp->s_astfilestats = filestat;
	PAL_FS_Dirfree(n, direntp);
	return ERR_OK;
}

int
PAL_FS_GetDirInfoFree(PAL_FS_DirStat_t *dirstatp)
{
	if (dirstatp && dirstatp->s_astfilestats)
		UMMA_Free(dirstatp->s_astfilestats);
	return ERR_OK;
}

int
PAL_FS_GetNumOfDirList(int pttype, const char *dirname, PAL_FS_UINT32 *num)
{
	DIR *dp;
	struct dirent *dirent;
	int pos = 0;

	UNUSED(pttype);
	if (dp = opendir(dirname), NULL == dp) {
		*num = 0;
		return ERR_FAIL;
	}

	while ((dirent = readdir(dp)) != NULL)
	{
		if (!DOT_OR_DOTDOT(dirent->d_name))
			pos++;
	}

	closedir(dp);

	*num = pos;
	return ERR_OK;
}

int
PAL_FS_ScanDirRecursive(char *dir_name, PAL_FS_ScanDir_Callback_Func callback)
{
	struct stat 	fs;
	char			*fullpath;

	if (-1 == stat(dir_name, &fs))
	{
		return ERR_FAIL;
	}

	if (S_ISDIR(fs.st_mode))
	{
		DIR 			*dirp;
		struct dirent 	*dp;

		if (dirp = opendir(dir_name), NULL == dirp)
			return FALSE;

		while ((dp = readdir(dirp)) != NULL)
		{
			if (DOT_OR_DOTDOT_OR_HIDDEN(dp->d_name))
				continue;

			if (fullpath = PAL_FS_Fullpath(dir_name, dp->d_name), NULL == fullpath)
				continue;

			PAL_FS_ScanDirRecursive(fullpath, callback);

			UMMA_Free(fullpath);
		}

		/* do the callback */
		if (callback != NULL)
			callback(dir_name);
		else
			HxLOG_Print("dir: %s\n", dir_name);

		closedir(dirp);
	}

	return ERR_OK;
}

int
PAL_FS_CopyDirRecursive(char *src, char *dst, PAL_FS_CopyDir_Callback_Func callback, int *copied_file_num)
{
	struct stat 	fs, dst_st;
	char			*fullpath = NULL, *dstpath = NULL;
	int				ret = 0;

	if (-1 == stat(src, &fs))
	{
		return 0;
	}

	if (S_ISDIR(fs.st_mode))
	{
		DIR 			*dirp;
		struct dirent 	*dp;

		if (dirp = opendir(src), NULL == dirp)
			return FALSE;

		while ((dp = readdir(dirp)) != NULL)
		{
			if (DOT_OR_DOTDOT(dp->d_name))
				continue;

			if (fullpath = PAL_FS_Fullpath(src, dp->d_name), NULL == fullpath)
				continue;

			if (0 == stat(fullpath, &dst_st))
			{
				if (S_ISDIR(dst_st.st_mode))
				{
					dstpath = PAL_FS_Fullpath(dst, dp->d_name);
					if (dstpath != NULL)
					{
						if (-1 == mkdir(dstpath, dst_st.st_mode))
						{
							HxLOG_Error("cannot mkdir \"%s\": %s\n", dstpath, strerror(errno));
						}
					}

					/* do the callback */
					if (callback != NULL)
					{
						ret = callback(fullpath, dstpath);
						if (ret > 0)
							*copied_file_num += ret;
					}
					else
						HxLOG_Print("dir: %s\n", src);

					PAL_FS_CopyDirRecursive(fullpath, dstpath, callback, copied_file_num);

					if (dstpath != NULL)
						UMMA_Free(dstpath);
				}
			}

			if (fullpath != NULL)
			{
				UMMA_Free(fullpath);
				fullpath = NULL;
			}
		}

		closedir(dirp);
	}

	return 0;
}

PAL_FS_BOOL
PAL_FS_IsAvailable2Rec(int pttype, int mode)
{
	UNUSED(pttype);
	UNUSED(mode);
	return TRUE;
}

/*
 * copy and remove operation
 */
static inline char *
PAL_FS_Fullpath(const char *s1, const char *s2)
{
	char *fullpath, *p;
	size_t len;

	len = strlen(s1) + strlen(s2) + 2;

	if (fullpath = UMMA_Malloc(len), NULL == fullpath)
		return NULL;

	strncpy(fullpath, s1, strlen(s1));

	p = fullpath + strlen(s1) - 1;
	while (ISSLASH(*p))
		p--;
	*++p = '/';
	*++p = '\0';

	strncpy(p, s2, len-(p-fullpath)-1);

	return fullpath;
}

static PAL_FS_UINT64
PAL_FS_Estspace(const char *pathname)
{
	struct stat st;
	PAL_FS_UINT64 sum = 0;

	if (-1 == lstat(pathname, &st))
	{
		return 0;
	}

	if (S_ISREG(st.st_mode))
		sum = st.st_size;

	if (S_ISLNK(st.st_mode)) {
#ifdef orignal_size
		/* XXX link file size or original file size? */
		if (-1 == stat(pathname, &st))
		{
			return 0;
		}
#endif
		sum = st.st_size;
	}

	if (S_ISDIR(st.st_mode)) {
		DIR *dp;
		struct dirent *dirent;
		char *fullpath;

		if (dp = opendir(pathname), NULL == dp)
			return 0;

		sum = st.st_size;
		while ((dirent = readdir(dp)) != NULL) {
			if (DOT_OR_DOTDOT(dirent->d_name))
				continue;

			if (fullpath = PAL_FS_Fullpath(pathname, dirent->d_name),
			    NULL == fullpath)
				continue;

			sum += PAL_FS_Estspace(fullpath);

			UMMA_Free(fullpath);
		}

		closedir(dp);
	}

	return sum;
}

int
PAL_FS_EstimateSpace(const char *pathname, PAL_FS_UINT64 *sizep)
{
	*sizep = PAL_FS_Estspace(pathname);
	return ERR_OK;
}

static PAL_FS_UINT64
estimate_fileUsage(const char *pathname, PAL_FS_UINT8 *filterstr[])
{
	struct stat st;
	PAL_FS_UINT64 sum = 0;

	if (-1 == lstat(pathname, &st))
	{
		return 0;
	}

	if (S_ISREG(st.st_mode))
	{
		int		i = 0;

		sum = 0;

		if (filterstr != NULL)
		{
			/* I really hate this part */
			while (filterstr[i] != NULL)
			{
				if (filterstr[i] && NULL != strstr(pathname, filterstr[i]))
					sum = st.st_size;
				i++;
			}
		}
	}

	if (S_ISDIR(st.st_mode)) {
		DIR *dp;
		struct dirent *dirent;
		char *fullpath;

		if (dp = opendir(pathname), NULL == dp)
			return 0;

		sum = st.st_size;
		while ((dirent = readdir(dp)) != NULL) {
			if (DOT_OR_DOTDOT(dirent->d_name))
				continue;

			if (fullpath = PAL_FS_Fullpath(pathname, dirent->d_name),
			    NULL == fullpath)
				continue;

			sum += estimate_fileUsage(fullpath, filterstr);

			UMMA_Free(fullpath);
		}

		closedir(dp);
	}

	return sum;
}

int PAL_FS_EstimateUsage(const char *pathname, PAL_FS_UINT8 *filterstr[], PAL_FS_UINT64 *sizep)
{
	*sizep = estimate_fileUsage(pathname, filterstr);
	return ERR_OK;
}

#if 1
//#if	defined(CONFIG_AVOID_RECORD_OVERFLOW)
#define	FS_OP_MSG_Q_SIZE			64
#define	FS_OP_SLEEP_TIME			20
#define	FS_OP_HIBERATION_TIME		1000
#define	FS_OP_RCV_TIMEOUT			1000

#define	DELETE_SLEEP_TIME			20
#define	MINIMUM_TRUNCATE_UNIT		10240			//	pagesize == 4K * 10K (40MB)

#define	DIRECT_DELETE_SIZE			(MINIMUM_TRUNCATE_UNIT * 4096)

#define	FS_OP_CANCEL				0
#define	FS_OP_DELETE				1
#define	FS_OP_COPY					2

#define	FS_OP_STATUS_NULL			0
#define	FS_OP_STATUS_WAITING		0x01
#define	FS_OP_STATUS_READY			0x02
#define	FS_OP_STATUS_RUNNING		0x04
#define	FS_OP_STATUS_CANCEL			0x08

#define	FS_OP_TODO_MAX				PAL_FS_MAX_COPY_INST_NUM

typedef	struct
{
	HUINT32	ulFSOpType;
	char	*pszSrc;
	char	*pszDest;
	int		bEncypt;
	void	*pvUserdata;
}	FS_OP_Msg_t;

typedef	struct
{
	HUINT32		ulCurStatus;
	HUINT32		ulFSOpType;
	HUINT32		ulPageSize;
	HUINT32		ulRemainSize;
	HUINT32		ulNotify;

	HINT32		lSrcHandle;
	HINT32		lDestHandle;

	HBOOL		bEncrypted;

	char		*pszSrc, *pszDest;
	void		*pvUserdata;
}	FS_TODO_t;

static	HUINT32		_ulFileTaskId = 0;
static	HUINT32		_ulFileTaskMsgId = 0;
static	HUINT32		_ulFileTaskSemId = 0;

static	FS_TODO_t	*_pstFsToDoList = NULL;
static	HUINT32		_ulFSOPCurrentJob, _ulFSOPFreeSlot;

static	char		*_copyBuff;
static	int			_copyBuffSize;

static	void	pal_fs_fileop_task(void *arg);

static char *	pal_fs_strdup (const char *str)
{
	HUINT32	len;
	char *	dup;

	len = strlen(str);
	dup = (char *)UMMA_Malloc(len + 1);
	if (dup)
	{
		if (len)
			memcpy(dup, str, len);
		dup[len] = '\0';
		HLIB_STD_StrRStrip(dup, "/");
	}
	return dup;
}


int	pal_fs_createFileTask(HUINT32 *pulTaskId)
{
	int		i;

	if (_pstFsToDoList == NULL)
	{
		_pstFsToDoList = UMMA_Malloc(sizeof(FS_TODO_t) * FS_OP_TODO_MAX);
		VK_memset32(_pstFsToDoList, 0, sizeof(FS_TODO_t) * FS_OP_TODO_MAX);
		for (i = 0; i < FS_OP_TODO_MAX; i++)
		{
			_pstFsToDoList[i].lSrcHandle = -1;
			_pstFsToDoList[i].lDestHandle = -1;
		}
		_ulFSOPCurrentJob = 0;
		_ulFSOPFreeSlot = 0;
	}

	_copyBuffSize = FS_COPY_BUF_SIZE;
	_copyBuff = memalign(_copyBuffSize, _copyBuffSize);

#if 0
	if (VK_SEM_Create((long unsigned int*)&_ulFileTaskSemId, "FSOP_SEM", VK_SUSPENDTYPE_PRIORITY) != VK_OK)
	{
		HxLOG_Print("error to create Semaphore (FSOP_SEM)\n");
		return ERR_FAIL;
	}
	if (VK_MSG_Create(FS_OP_MSG_Q_SIZE, sizeof(FS_OP_Msg_t), "FSOP_MSG", (long unsigned int*)&_ulFileTaskMsgId, VK_SUSPENDTYPE_FIFO) != VK_OK)
	{
		HxLOG_Print("error to create MessageQueue (FSOP_MSG)\n");
		return ERR_FAIL;
	}
	if (VK_TASK_Create(pal_fs_fileop_task, VK_TASK_PRIORITY_MW_REF, SIZE_64K, "FSOP_TASK", NULL, (long unsigned int*)&_ulFileTaskId, 0) != VK_OK)
	{
		HxLOG_Print("error to create thread (FSOP_TASK)\n");
		return ERR_FAIL;
	}

	VK_TASK_Start(_ulFileTaskId);
#endif

	return ERR_OK;
}

FS_TODO_t	*pal_fs_fileop_addToDoList(FS_OP_Msg_t *pstMsg, HUINT32 ulNotify)
{
	HINT32		i;
	FS_TODO_t	*pstToDo;

	pstToDo = NULL;
	i = 0;
	while ((_pstFsToDoList[_ulFSOPFreeSlot].ulCurStatus != FS_OP_STATUS_NULL) && (i < FS_OP_TODO_MAX))
	{
		i++;
		_ulFSOPFreeSlot = (_ulFSOPFreeSlot + 1) % FS_OP_TODO_MAX;
	}

	if (i == FS_OP_TODO_MAX)
	{
		return pstToDo;
	}

	VK_memset32(&_pstFsToDoList[_ulFSOPFreeSlot], 0, sizeof(FS_TODO_t));
	_pstFsToDoList[_ulFSOPFreeSlot].lSrcHandle	= -1;
	_pstFsToDoList[_ulFSOPFreeSlot].lDestHandle	= -1;
	_pstFsToDoList[_ulFSOPFreeSlot].ulFSOpType	= pstMsg->ulFSOpType;
	_pstFsToDoList[_ulFSOPFreeSlot].pszSrc		= pstMsg->pszSrc;
	_pstFsToDoList[_ulFSOPFreeSlot].pszDest		= pstMsg->pszDest;
	_pstFsToDoList[_ulFSOPFreeSlot].ulCurStatus	= FS_OP_STATUS_WAITING;
	_pstFsToDoList[_ulFSOPFreeSlot].ulNotify	= ulNotify;
	_pstFsToDoList[_ulFSOPFreeSlot].bEncrypted 	= (HBOOL)pstMsg->bEncypt;
	_pstFsToDoList[_ulFSOPFreeSlot].pvUserdata	= pstMsg->pvUserdata;
	pstToDo = &(_pstFsToDoList[_ulFSOPFreeSlot]);
	HxLOG_Print("ToDoList q add(%d)[%d] : [%s] [%x][%x]\n", _ulFSOPFreeSlot, ulNotify, _pstFsToDoList[_ulFSOPFreeSlot].pszSrc, (int)_pstFsToDoList[_ulFSOPFreeSlot].pszSrc, (int)_pstFsToDoList[_ulFSOPFreeSlot].pszDest);

	_ulFSOPFreeSlot = (_ulFSOPFreeSlot + 1) % FS_OP_TODO_MAX;

	return pstToDo;
}

int		pal_fs_fileop_setflagcancel(void)
{
	HINT32		i;

	for (i = 0; i < FS_OP_TODO_MAX; i++)
	{
		if (_pstFsToDoList[i].ulCurStatus == FS_OP_STATUS_RUNNING)
		{
			_pstFsToDoList[i].ulCurStatus = FS_OP_STATUS_CANCEL;
			HxLOG_Print("running task is canceled!!\n ");
		}
		else
		{
			if (_pstFsToDoList[i].ulCurStatus != FS_OP_STATUS_NULL)
			{
				if (_pstFsToDoList[i].pszSrc)
					UMMA_Free(_pstFsToDoList[i].pszSrc);
				if (_pstFsToDoList[i].pszDest)
					UMMA_Free(_pstFsToDoList[i].pszDest);

				_pstFsToDoList[i].ulCurStatus	= FS_OP_STATUS_NULL;
				HxLOG_Print("Clear Wait flag to Cancel!!\n");
			}
		}
	}

	return ERR_OK;
}

void	pal_fs_fileop_addDir(char *pathname, HUINT32 ulOPType, HUINT32 ulNotify)
{
	DIR		*dp;
	struct	stat	st;
	struct	dirent	*dirent;
	char	*fullpath;

	FS_TODO_t		*pstToDo;
	FS_OP_Msg_t		stMsg;

	if (-1 == lstat(pathname, &st))
	{
		return;
	}

	if (S_ISDIR(st.st_mode))
	{
		if (dp = opendir(pathname), NULL == dp)
			return;

		while ((dirent = readdir(dp)) != NULL)
		{
			if (DOT_OR_DOTDOT(dirent->d_name))
				continue;

			if (fullpath = PAL_FS_Fullpath(pathname, dirent->d_name), NULL == fullpath)
				continue;

			pal_fs_fileop_addDir(fullpath, ulOPType, 0);

			UMMA_Free(fullpath);
		}

		closedir(dp);
	}

	HxSTD_MemSet(&stMsg, 0, sizeof(FS_OP_Msg_t));
	stMsg.ulFSOpType	= ulOPType;
	stMsg.pszSrc		= pal_fs_strdup(pathname);
	stMsg.pszDest		= NULL;
	stMsg.bEncypt		= FALSE;

	pstToDo = pal_fs_fileop_addToDoList(&stMsg, ulNotify);
	if (S_ISDIR(st.st_mode) && pstToDo)
		pstToDo->ulCurStatus = FS_OP_STATUS_READY;
}

void	pal_fs_fileop_delete_wait(FS_TODO_t *pstToDo)
{
	struct	stat	st;

	if ( NULL == pstToDo )
		return;

	if (pstToDo->pszSrc == NULL)
		goto end_rtn;

	if (-1 == lstat(pstToDo->pszSrc, &st))
	{
		HxLOG_Print("remove error : lstat return -1 [%s]\n", pstToDo->pszSrc);
		if (pstToDo->ulNotify)
			PAL_FS_Callback(PAL_FS_EVENT_DELETE_PROGRESS, 100, 0, (unsigned int)pstToDo->pvUserdata);

		pstToDo->ulCurStatus = FS_OP_STATUS_NULL;
		goto end_rtn;
	}

	if (S_ISDIR(st.st_mode) && (pstToDo->ulCurStatus == FS_OP_STATUS_WAITING))
	{
		pal_fs_fileop_addDir(pstToDo->pszSrc, FS_OP_DELETE, 1);
		pstToDo->ulCurStatus = FS_OP_STATUS_NULL;
	} else
	{
		if (st.st_size < DIRECT_DELETE_SIZE)
		{
			HxLOG_Print("remove file(W) : [%s]\n", pstToDo->pszSrc);
			if (pstToDo->ulNotify)
				PAL_FS_Callback(PAL_FS_EVENT_DELETE_PROGRESS, 100, 0, (unsigned int)pstToDo->pvUserdata);

			(void)remove(pstToDo->pszSrc);
			pstToDo->ulCurStatus = FS_OP_STATUS_NULL;
			VK_TASK_Sleep(FS_OP_SLEEP_TIME);
		}
		else
		{
			pstToDo->lSrcHandle		= open(pstToDo->pszSrc, O_RDWR, 0);
			pstToDo->ulPageSize		= st.st_blksize;
			if (pstToDo->ulPageSize == 0)
				pstToDo->ulPageSize = 4096;

			pstToDo->ulPageSize		= pstToDo->ulPageSize * MINIMUM_TRUNCATE_UNIT;		//	40MB
			pstToDo->ulRemainSize	= (st.st_size / pstToDo->ulPageSize);
			pstToDo->ulCurStatus	= FS_OP_STATUS_RUNNING;
		}
	}

end_rtn:
	if (pstToDo->ulCurStatus == FS_OP_STATUS_NULL)
	{
		if (pstToDo->pszSrc != NULL)
		{
			UMMA_Free(pstToDo->pszSrc);
			pstToDo->pszSrc = NULL;
		}
		if (pstToDo->pszDest != NULL)
		{
			UMMA_Free(pstToDo->pszDest);
			pstToDo->pszDest = NULL;
		}
	}
}

int		pal_fs_fileop_copy_wait_proc(const char *src, const char *dst, HUINT32 ulNotify)
{
	FS_OP_Msg_t		stMsg;
	FS_TODO_t		*pstToDo;
	struct stat src_st, dst_st;
	int r;

	if (-1 == lstat(src, &src_st))
	{
		HxLOG_Error("cannot stat \"%s\": %s\n", src, strerror(errno));
		return ERR_FAIL;
	}

	r = lstat(dst, &dst_st);
	if (-1 == r && ENOENT != errno)
	{
		HxLOG_Error("cannot stat \"%s\": %s\n", dst, strerror(errno));
		return ERR_FAIL;
	}

	if (-1 == r && S_ISDIR(src_st.st_mode))
	{
		if (-1 == mkdir(dst, src_st.st_mode))
		{
			HxLOG_Error("cannot mkdir \"%s\": %s\n", dst, strerror(errno));
			return ERR_FAIL;
		}
	}

	switch (src_st.st_mode & S_IFMT)
	{
		case S_IFDIR:
		{
			DIR *dp;
			struct dirent *dirent;
			char *nsrc, *ndst;

			if (dp = opendir(src), NULL == dp)
				return ERR_FAIL;

			while ((dirent = readdir(dp)) != NULL) {
				if (DOT_OR_DOTDOT(dirent->d_name))
					continue;

				if (nsrc = PAL_FS_Fullpath(src, dirent->d_name),
				    NULL == nsrc)
					continue;

				if (ndst = PAL_FS_Fullpath(dst, dirent->d_name),
				    NULL == ndst) {
					UMMA_Free(nsrc);
					continue;
				}

				pal_fs_fileop_copy_wait_proc(nsrc, ndst, 0);

				UMMA_Free(nsrc);
				UMMA_Free(ndst);
			}

			closedir(dp);
			break;
		}

		case S_IFREG:
			VK_memset32(&stMsg, 0, sizeof(FS_OP_Msg_t));
			stMsg.ulFSOpType = FS_OP_COPY;
			stMsg.pszSrc = pal_fs_strdup(src);
			stMsg.pszDest = pal_fs_strdup(dst);
			pstToDo = pal_fs_fileop_addToDoList(&stMsg, ulNotify);
			if (pstToDo == NULL)
				return ERR_FAIL;
			break;

		case S_IFLNK:
		{
			int len;
			char llink[PATH_MAX];

			if (-1 == (len = readlink(src, llink, sizeof(llink) - 1))) {
				HxLOG_Error("cannot readlink \"%s\": %s\n", src, strerror(errno));
				return ERR_FAIL;
			}
			llink[len] = '\0';
			if (0 == r && -1 == remove(dst))
			{
				HxLOG_Error("cannot remove \"%s\": %s\n", dst, strerror(errno));
				return ERR_FAIL;
			}
			if (-1 == symlink(llink, dst)) {
				HxLOG_Error("cannot symlink \"%s\": %s\n", dst, strerror(errno));
				return ERR_FAIL;
			}
			break;
		}

		case S_IFBLK:
		case S_IFCHR:
			if (0 == r && -1 == remove(dst))
			{
				HxLOG_Error("cannot remove \"%s\": %s\n", dst, strerror(errno));
				return ERR_FAIL;
			}

			if (-1 == mknod(dst, src_st.st_mode, src_st.st_rdev))
			{
				HxLOG_Error("cannot mknod \"%s\": %s\n", dst, strerror(errno));
				return ERR_FAIL;
			}
			break;

		case S_IFIFO:
			if (0 == r && -1 == remove(dst))
			{
				HxLOG_Error("cannot remove \"%s\": %s\n", dst, strerror(errno));
				return ERR_FAIL;
			}
			if (-1 == mkfifo(dst, src_st.st_mode)) {
				HxLOG_Error("cannot mkfifo \"%s\": %s\n", dst, strerror(errno));
				return ERR_FAIL;
			}
			break;

		case S_IFSOCK:
			HxLOG_Print("%s is a socket (not copied)\n", src);
			break;

		default:
			HxLOG_Error("unrecognized file type \"%s\"\n", src);
			return ERR_FAIL;
	}

	return ERR_OK;
}

void	pal_fs_fileop_copy_wait(FS_TODO_t *pstToDo)
{
	struct	stat	src_st, dst_st;
	int		r;
	HUINT32	ulSaveStatus = FS_OP_STATUS_NULL;

	if (pstToDo == NULL)
		return;

	if (pstToDo->pszSrc == NULL)
		goto end_rtn;

	if (-1 == lstat(pstToDo->pszSrc, &src_st))
	{
		ulSaveStatus = pstToDo->ulCurStatus;
		pstToDo->ulCurStatus = FS_OP_STATUS_NULL;
		HxLOG_Print("copy error : [%s] - src lstat return -1", pstToDo->pszSrc);
		goto end_rtn;
	}

	r = lstat(pstToDo->pszDest, &dst_st);
	if (-1 == r && ENOENT != errno)
	{
		ulSaveStatus = pstToDo->ulCurStatus;
		pstToDo->ulCurStatus = FS_OP_STATUS_NULL;
		HxLOG_Print("copy error : [%s] - dst lstat return -1", pstToDo->pszDest);
		goto end_rtn;
	}

	HxLOG_Print("copy_wait : [%s] - ", pstToDo->pszSrc);
	if ((src_st.st_mode & S_IFMT) == S_IFREG)
	{
		HxLOG_Print("SET RUNNING\n");
		pstToDo->ulCurStatus = FS_OP_STATUS_RUNNING;
	}
	else
	{
		HINT32		tempslot;

		HxLOG_Print("DIR SEARCH\n");
		tempslot = _ulFSOPFreeSlot;
		pal_fs_fileop_copy_wait_proc(pstToDo->pszSrc, pstToDo->pszDest, pstToDo->ulNotify);
		if (tempslot != _ulFSOPFreeSlot)
		{
			//	add notiflag to final start operation.
			tempslot = (_ulFSOPFreeSlot + FS_OP_TODO_MAX - 1) % FS_OP_TODO_MAX;
			//	Copy시 noti한번만 줄려고 하니 어려움..
			//	재수를 바랄수... 거의 대부분은 걸릴것임..
			if (_pstFsToDoList[tempslot].ulCurStatus == FS_OP_STATUS_NULL)
			{
				HxLOG_Print("이러면 안되는데... 큰일이네...\n");
			}
			_pstFsToDoList[tempslot].ulNotify = 1;
		}
		pstToDo->ulCurStatus = FS_OP_STATUS_NULL;
	}

end_rtn:
	if (pstToDo->ulCurStatus == FS_OP_STATUS_NULL)
	{
		/* 여기서 Error가 발생할 경우에도 Message 전달을 해주어야 한다.
		Copy Dialog가 계속 빙빙빙 돌고 있음.... COPY ERROR에 관련된 Message가 없네 ㅡㅡ;;
		다른 곳도 Error이건 Cancel이건 Done이건 모두 PAL_FS_EVENT_COPY_DONE를 사용하고 있어서 따라함. */
		if (pstToDo->ulNotify && ulSaveStatus != FS_OP_STATUS_NULL)
		{
			PAL_FS_Callback(PAL_FS_EVENT_COPY_DONE, 100, 0, (unsigned int)pstToDo->pvUserdata);//PROGRESS, 100, 0);
		}

		if (pstToDo->pszSrc != NULL)
		{
			UMMA_Free(pstToDo->pszSrc);
			pstToDo->pszSrc = NULL;
		}
		if (pstToDo->pszDest != NULL)
		{
			UMMA_Free(pstToDo->pszDest);
			pstToDo->pszDest = NULL;
		}
	}
}

void	pal_fs_fileop_delete_running(FS_TODO_t *pstToDo)
{
	if (pstToDo == NULL)
		return;

	if (pstToDo->ulRemainSize > 1)
	{
		ftruncate(pstToDo->lSrcHandle, (HUINT64)((HUINT64)pstToDo->ulRemainSize * (HUINT64)pstToDo->ulPageSize));
		//HxLOG_Print("truncate : [%s] [%d][%lld]\n", pstToDo->pszSrc, pstToDo->ulRemainSize, (HUINT64)((HUINT64)pstToDo->ulRemainSize * (HUINT64)pstToDo->ulPageSize));
		pstToDo->ulRemainSize--;
		if (!(pstToDo->ulRemainSize & 0xF))
			fsync(pstToDo->lSrcHandle);
	}
	else
	{
		HxLOG_Print("remove file(R) : [%s]\n", pstToDo->pszSrc ? pstToDo->pszSrc : "Null");
		if (pstToDo->ulNotify)
			PAL_FS_Callback(PAL_FS_EVENT_DELETE_PROGRESS, 100, 0, (unsigned int)pstToDo->pvUserdata);

		close(pstToDo->lSrcHandle);

		if (pstToDo->pszSrc != NULL)
		{
			if (-1 == remove(pstToDo->pszSrc))
			{
				HxLOG_Error("cannot remove %s. %s\n", pstToDo->pszSrc, strerror(errno));
			}
		}

		if (pstToDo->pszSrc != NULL)
		{
			UMMA_Free(pstToDo->pszSrc);
			pstToDo->pszSrc = NULL;
		}
		pstToDo->ulCurStatus = FS_OP_STATUS_NULL;

		VK_TASK_Sleep(FS_OP_SLEEP_TIME);
	}
}

int		pal_fs_fileop_decrypt_read(int fd, char **buff, size_t size)
{
#if 0
	int					read_size = 0;

	read_size = read(fd, _stDecrypt.pSrcBuffer, size);
	if (read_size > 0)
	{
		if (read_size != _stDecrypt.ulBuffSize)
		{
			HxLOG_Error("read_size:%d,_stDecrypt.ulBuffSize:%d\n",read_size,_stDecrypt.ulBuffSize);
		}
		//_stDecrypt.ulBuffSize = read_size;//ulBufSize; --> bufsize보다 작게 읽으 면 ?

		if (DI_CRYPT_Decrypt(&_stDecrypt) != DI_ERR_OK)
		{
			HxLOG_Error("decrypt fail.\n");
			return 0;
		}

		*buff = _stDecrypt.pDstBuffer;
		/* copy the decrypted data */
		//VK_memcpy32(buff, _stDecrypt.pDstBuffer, read_size);
	}
	return read_size;
#else
	return 0;
#endif

}

void	pal_fs_fileop_delete_cancel(FS_TODO_t *pstToDo)
{
	HxLOG_Print("delete cancel : [%s]\n", pstToDo->pszSrc);
	if (pstToDo->lSrcHandle != -1)
	{
		close(pstToDo->lSrcHandle);
	}

	if (pstToDo->ulNotify)
	{
		if (CALLBACK(PAL_FS_EVENT_DELETE_PROGRESS))//PROGRESS))
			PAL_FS_Callback(PAL_FS_EVENT_DELETE_PROGRESS, 100, 0, (unsigned int)pstToDo->pvUserdata);//PROGRESS, 100, 0);
	}

	if (pstToDo->pszSrc)
		UMMA_Free(pstToDo->pszSrc);

	pstToDo->ulCurStatus = FS_OP_STATUS_NULL;
	VK_TASK_Sleep(FS_OP_SLEEP_TIME);
}

void	pal_fs_fileop_copy_running(FS_TODO_t *pstToDo)
{
	char		*p = NULL;
	HINT32		n_read, n_written, nLoop;

	HxLOG_Print("copy running\n");
	if (pstToDo->lSrcHandle == -1)
	{
		int		r, s;
		struct	stat	src_st, dest_st;

		s = lstat(pstToDo->pszSrc, &src_st);
		if (s)
		{
			HxLOG_Error("file is not existed..[%s]\n", pstToDo->pszSrc);
			goto err_rtn;
		}

		r = lstat(pstToDo->pszDest, &dest_st);

		pstToDo->lSrcHandle = open(pstToDo->pszSrc, O_RDONLY);
		if (pstToDo->lSrcHandle == -1)
			goto err_rtn;

		if (r)
		{
			pstToDo->lDestHandle = open(pstToDo->pszDest, O_CREAT | O_WRONLY, src_st.st_mode);
		}
		else
		{
			pstToDo->lDestHandle = open(pstToDo->pszDest, O_TRUNC | O_WRONLY, src_st.st_mode);
		}
		if (pstToDo->lDestHandle == -1)
			goto err_rtn;

		if (pstToDo->bEncrypted == FALSE)
			HxLOG_Print("Start copy : [%s] --> [%s]\n", pstToDo->pszSrc, pstToDo->pszDest);
		else
			HxLOG_Print("Start decrypt-copy : [%s] --> [%s]\n", pstToDo->pszSrc, pstToDo->pszDest);
	}
	else
	{
		//	copy...
		nLoop = 0;
		while (nLoop < MINIMUM_TRUNCATE_UNIT)
		{
#if defined(SUPPORT_DECRYPT_COPY)
			if (pstToDo->bEncrypted == FALSE)
			{
				n_read = read(pstToDo->lSrcHandle, _copyBuff, _copyBuffSize);
				p = _copyBuff;
			}
			else
			{
				n_read = pal_fs_fileop_decrypt_read(pstToDo->lSrcHandle, &p, _stDecrypt.ulBuffSize);
			}
#else
			n_read = read(pstToDo->lSrcHandle, _copyBuff, _copyBuffSize);
			p = _copyBuff;
#endif
			if (n_read == 0)
			{
				//	copy done...
				break;
			}

			if (n_read < 0)
			{
				if (EINTR == errno)
					continue;

				goto err_rtn;
			}

			while (n_read > 0)
			{
				n_written = write(pstToDo->lDestHandle, p, n_read);
				if (n_written < 0) {
					if (EINTR == errno)
						continue;
					goto err_rtn;
				}

				p += n_written;
				n_read -= n_written;
			}

			nLoop ++;

#if 0 // Copy시의 FSync은 오히려 더 느리게 만들더라 그래서 삭제.
			if (!(nLoop & 0xF))
				fsync(pstToDo->lDestHandle);
#endif
		}

		if ((nLoop < MINIMUM_TRUNCATE_UNIT) && (n_read == 0))
		{
			HxLOG_Print("copy end ---> noti\n");
			//	copy done...
			if (pstToDo->ulNotify)
			{
				PAL_FS_Callback(PAL_FS_EVENT_COPY_DONE, 100, 0, (unsigned int)pstToDo->pvUserdata);
			}

			close(pstToDo->lSrcHandle);
			close(pstToDo->lDestHandle);

			if (pstToDo->pszSrc)
				UMMA_Free(pstToDo->pszSrc);
			if (pstToDo->pszDest)
				UMMA_Free(pstToDo->pszDest);
			pstToDo->ulCurStatus = FS_OP_STATUS_NULL;
			VK_TASK_Sleep(FS_OP_SLEEP_TIME);
		}
	}

	return;

err_rtn:
	HxLOG_Print("copy error --> noti\n");

	//	copy done...

	if (pstToDo->lSrcHandle != -1)
	{
		close(pstToDo->lSrcHandle);
	}

	if (pstToDo->lDestHandle != -1)
	{
		close(pstToDo->lDestHandle);
	}

	if (pstToDo->ulNotify)
	{
		PAL_FS_Callback(PAL_FS_EVENT_COPY_DONE, 100, 0, (unsigned int)pstToDo->pvUserdata);
	}

	if (pstToDo->pszSrc)
		UMMA_Free(pstToDo->pszSrc);
	if (pstToDo->pszDest)
		UMMA_Free(pstToDo->pszDest);
	pstToDo->ulCurStatus = FS_OP_STATUS_NULL;
	VK_TASK_Sleep(FS_OP_SLEEP_TIME);
	return;
}

void	pal_fs_fileop_copy_cancel(FS_TODO_t *pstToDo)
{
	HxLOG_Print("copy cancel : [%s]\n", pstToDo->pszSrc);
	if (pstToDo->lSrcHandle != -1)
	{
		close(pstToDo->lDestHandle);
		close(pstToDo->lSrcHandle);
	}

	if (pstToDo->ulNotify)
	{
		if (CALLBACK(PAL_FS_EVENT_COPY_DONE))//PROGRESS))
			PAL_FS_Callback(PAL_FS_EVENT_COPY_DONE, 100, 0, (unsigned int)pstToDo->pvUserdata);//PROGRESS, 100, 0);
	}

	if (pstToDo->pszSrc)
		UMMA_Free(pstToDo->pszSrc);
	if (pstToDo->pszDest)
		UMMA_Free(pstToDo->pszDest);
	pstToDo->ulCurStatus = FS_OP_STATUS_NULL;
	VK_TASK_Sleep(FS_OP_SLEEP_TIME);
}


FS_TODO_t		*pal_fs_fileop_findtodobyStatus(HUINT32 ulStatus)
{
	HINT32		i;

	if (_pstFsToDoList[_ulFSOPCurrentJob].ulCurStatus & ulStatus)
		return &(_pstFsToDoList[_ulFSOPCurrentJob]);

	for (i = 0; i < FS_OP_TODO_MAX; i++)
	{
		if (_pstFsToDoList[(_ulFSOPFreeSlot + i) % FS_OP_TODO_MAX].ulCurStatus & ulStatus)
		{
			_ulFSOPCurrentJob = (_ulFSOPFreeSlot + i) % FS_OP_TODO_MAX;
			return &(_pstFsToDoList[_ulFSOPCurrentJob]);
		}
	}

	return NULL;
}

HBOOL	pal_fs_fileop_doit(void)
{
	FS_TODO_t	*pstToDo;

	pstToDo = pal_fs_fileop_findtodobyStatus(FS_OP_STATUS_RUNNING);
	if (pstToDo == NULL)
		pstToDo = pal_fs_fileop_findtodobyStatus(FS_OP_STATUS_CANCEL);
	if (pstToDo == NULL)
		pstToDo = pal_fs_fileop_findtodobyStatus(FS_OP_STATUS_WAITING | FS_OP_STATUS_READY);
	if (pstToDo == NULL)
		return FALSE;

	//	initialize...
	switch (pstToDo->ulCurStatus)
	{
	case FS_OP_STATUS_WAITING:
	case FS_OP_STATUS_READY:
		switch (pstToDo->ulFSOpType)
		{
		case FS_OP_DELETE:
			pal_fs_fileop_delete_wait(pstToDo);
			break;

		case FS_OP_COPY:
			pal_fs_fileop_copy_wait(pstToDo);
			break;

		default:
			break;
		}

		break;

	case FS_OP_STATUS_CANCEL:
		switch (pstToDo->ulFSOpType)
		{
		case FS_OP_DELETE:
			pal_fs_fileop_delete_cancel(pstToDo);
			break;

		case FS_OP_COPY:
			pal_fs_fileop_copy_cancel(pstToDo);
			break;

		default:
			break;
		}

		break;

	case FS_OP_STATUS_RUNNING:
		switch (pstToDo->ulFSOpType)
		{
		case FS_OP_DELETE:
			pal_fs_fileop_delete_running(pstToDo);
			break;

		case FS_OP_COPY:
			pal_fs_fileop_copy_running(pstToDo);
			break;

		default:
			break;
		}

		break;

	default:
		break;
	}

	return TRUE;
}

static	void	pal_fs_fileop_task(void *arg)
{
	FS_OP_Msg_t		stMsg;
	HUINT32	ulRcvTimeOut = FS_OP_SLEEP_TIME;

	while (1)
	{
		if (VK_MSG_ReceiveTimeout(_ulFileTaskMsgId, &stMsg, sizeof(FS_OP_Msg_t), ulRcvTimeOut) == VK_OK)
		{
			VK_SEM_Get(_ulFileTaskSemId);
			switch (stMsg.ulFSOpType)
			{
				case	FS_OP_CANCEL:
					pal_fs_fileop_setflagcancel();
					break;
				case	FS_OP_DELETE:
					if (pal_fs_fileop_addToDoList(&stMsg, 1) == NULL)
						VK_MSG_Send(_ulFileTaskMsgId, &stMsg, sizeof(FS_OP_Msg_t));
					break;
				case	FS_OP_COPY:
					if (pal_fs_fileop_addToDoList(&stMsg, 1) == NULL)
						VK_MSG_Send(_ulFileTaskMsgId, &stMsg, sizeof(FS_OP_Msg_t));
					break;
			}
			VK_SEM_Release(_ulFileTaskSemId);

			ulRcvTimeOut = FS_OP_RCV_TIMEOUT;
		}
		else
		{
			HBOOL	bIsRunning;

			VK_SEM_Get(_ulFileTaskSemId);
			bIsRunning = pal_fs_fileop_doit();
			VK_SEM_Release(_ulFileTaskSemId);

			if (bIsRunning == FALSE)
				ulRcvTimeOut = FS_OP_HIBERATION_TIME;
			else
				ulRcvTimeOut = FS_OP_SLEEP_TIME;
		}
	}
}

int	PAL_FS_DeleteAsynch(int pttype, const char *pathname)
{
	HCHAR			*szDeleteFilePath = (HCHAR *)pathname;
	HCHAR			*szDstPathName;
	HCHAR			 szMovedPathName[308];
	FS_OP_Msg_t		 stMsg;
	HERROR			 hErr;

	if (NULL == pathname)
		return ERR_FAIL;

	if (!HLIB_FILE_Exist(pathname))
		return ERR_FAIL;

	if (_ulFileTaskId == 0)
	{
		if (pal_fs_createFileTask(&_ulFileTaskId) != ERR_OK)
		{
			HxLOG_Print("Fail to delete file asynch, Create task fail!!\n");
			return ERR_FAIL;
		}
	}

	if ('\0' == pathname[0])
	{
		HxLOG_Error ("Error: pathname NULL\n");
		return ERR_FAIL;
	}

	hErr = PAL_FS_GetRemovingFileNameToRecycleDir ((HCHAR *)pathname, szMovedPathName);
	if (ERR_OK == hErr)
	{
		hErr = PAL_FS_Rename (pttype, pathname, szMovedPathName);
		if (ERR_OK == hErr)
		{
			szDeleteFilePath = szMovedPathName;
		}
	}

	szDstPathName = pal_fs_strdup (szDeleteFilePath);
	VK_memset32(&stMsg, 0, sizeof(FS_OP_Msg_t));
	stMsg.ulFSOpType = FS_OP_DELETE;
	stMsg.pszSrc = szDstPathName;

	if (VK_MSG_Send(_ulFileTaskMsgId, &stMsg, sizeof(FS_OP_Msg_t)) != ERR_OK)
	{
		HxLOG_Error ("Error to send message (FSOP_MSG)\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}


int
PAL_FS_DecryptCopy(const char *src, const char *dst)
{
	FS_OP_Msg_t		stMsg;

	if (_ulFileTaskId == 0)
	{
		if (pal_fs_createFileTask(&_ulFileTaskId) != ERR_OK)
		{
			HxLOG_Print("fail to create a decrypt-file task!!\n");
			return ERR_FAIL;
		}
	}

	HxLOG_Print("[%s:%d] FS_COPY_BUF_SIZE:%d\n",__FUNCTION__,__LINE__,FS_COPY_BUF_SIZE);

	VK_memset32(&stMsg, 0, sizeof(FS_OP_Msg_t));
	stMsg.ulFSOpType = FS_OP_COPY;
	stMsg.pszSrc = pal_fs_strdup(src);
	stMsg.pszDest = pal_fs_strdup(dst);
#if defined(SUPPORT_DECRYPT_COPY)
	stMsg.bEncypt = TRUE;
#else
	stMsg.bEncypt = FALSE;
#endif

	if (VK_MSG_Send(_ulFileTaskMsgId, &stMsg, sizeof(FS_OP_Msg_t)) != ERR_OK)
	{
		HxLOG_Print("error to send message (FSOP_MSG)\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

int	PAL_FS_StopDeleteProcess(void)
{
	FS_OP_Msg_t		stMsg;

	VK_memset(&stMsg, 0, sizeof(FS_OP_Msg_t));
	stMsg.ulFSOpType = FS_OP_CANCEL;
	if (VK_MSG_Send(_ulFileTaskMsgId, &stMsg, sizeof(FS_OP_Msg_t)) != ERR_OK)
	{
		HxLOG_Print("error to send message (FSOP_MSG)\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

int
PAL_FS_Copy(const char *src, const char *dst)
{
	return PAL_FS_CopyWithUserdata(src, dst, NULL);
}

int
PAL_FS_CopyWithUserdata(const char *src, const char *dst, void *userdata)
{
	FS_OP_Msg_t		stMsg;

	if (_ulFileTaskId == 0)
	{
		if (pal_fs_createFileTask(&_ulFileTaskId) != ERR_OK)
		{
			HxLOG_Print("Fail to delete file asynch, Create task fail!!\n");
			return ERR_FAIL;
		}
	}

	VK_memset32(&stMsg, 0, sizeof(FS_OP_Msg_t));
	stMsg.ulFSOpType = FS_OP_COPY;
	stMsg.pszSrc = pal_fs_strdup(src);
	stMsg.pszDest = pal_fs_strdup(dst);
	stMsg.bEncypt = FALSE;
	stMsg.pvUserdata = userdata;

	if (VK_MSG_Send(_ulFileTaskMsgId, &stMsg, sizeof(FS_OP_Msg_t)) != ERR_OK)
	{
		HxLOG_Print("error to send message (FSOP_MSG)\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

int
PAL_FS_StopCopyProcess(void)
{
	FS_OP_Msg_t		stMsg;

	VK_memset32(&stMsg, 0, sizeof(FS_OP_Msg_t));
	stMsg.ulFSOpType = FS_OP_CANCEL;
	if (VK_MSG_Send(_ulFileTaskMsgId, &stMsg, sizeof(FS_OP_Msg_t)) != ERR_OK)
	{
		HxLOG_Print("error to send message (FSOP_MSG)\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

int
PAL_FS_StopProcessSync(void)
{
	HINT32		i;

	VK_SEM_Get(_ulFileTaskSemId);

	/* On cases that source or destination dismissed such as storage disconnected, */
	/* pal_fs_fileop_copy_running's error handler would close file descriptors */
	/* but finalize them here for sure */
	for (i = 0; i < FS_OP_TODO_MAX; i++)
	{
		if (_pstFsToDoList[i].ulCurStatus != FS_OP_STATUS_NULL)
		{
			HxLOG_Print("%d - STOP file process: %s\n", i, _pstFsToDoList[i].pszSrc);
			if (_pstFsToDoList[i].lSrcHandle != -1)
			{
				close(_pstFsToDoList[i].lSrcHandle);
			}
			if (_pstFsToDoList[i].lDestHandle != -1)
			{
				close(_pstFsToDoList[i].lDestHandle);
			}

			if (_pstFsToDoList[i].pszSrc)
				UMMA_Free(_pstFsToDoList[i].pszSrc);
			if (_pstFsToDoList[i].pszDest)
				UMMA_Free(_pstFsToDoList[i].pszDest);

			_pstFsToDoList[i].ulCurStatus	= FS_OP_STATUS_NULL;
		}
	}

	VK_SEM_Release(_ulFileTaskSemId);

	return ERR_OK;
}
#endif

STATIC int pal_fs_deleteSync (int pttype, const char *pathname)
{
	struct stat st;

	UNUSED(pttype);

	if (-1 == lstat(pathname, &st))
	{
		HxLOG_Error("cannot stat \"%s\": %s\n", pathname, strerror(errno));
		return ERR_FAIL;
	}

	if (S_ISDIR(st.st_mode)) {
		DIR *dp;
		struct dirent *dirent;
		char *fullpath;

		if (dp = opendir(pathname), NULL == dp)
			return 0;

		while ((dirent = readdir(dp)) != NULL) {
			if (DOT_OR_DOTDOT(dirent->d_name))
				continue;

			if (fullpath = PAL_FS_Fullpath(pathname, dirent->d_name),
			    NULL == fullpath)
				continue;

			pal_fs_deleteSync(0, fullpath);

			UMMA_Free(fullpath);
		}

		closedir(dp);
	}

	HxLOG_Print("remove \"%s\"\n", pathname);
	if (-1 == remove(pathname))
	{
		HxLOG_Error("cannot remove \"%s\": %s\n", pathname, strerror(errno));
		return ERR_FAIL;
	}

	return ERR_OK;
}

// Recycle Bin으로 옮길 수 있으면 move 후에 async로 지우고, 그렇지 않다고 하면 Sync로 반드시 지운다.
int PAL_FS_Delete(int pttype, const char *pathname)
{
	HCHAR			*szDeleteFilePath = (HCHAR *)pathname;
#if 0
	HERROR			 hErr;
	HCHAR			 szMovedPathName[308];

	if (_ulFileTaskId == 0)
	{
		if (pal_fs_createFileTask(&_ulFileTaskId) != ERR_OK)
		{
			HxLOG_Print("Fail to delete file asynch, Create task fail!!\n");
			return ERR_FAIL;
		}
	}

	if ((NULL == pathname) || ('\0' == pathname[0]))
	{
		HxLOG_Error ("Error: pathname NULL\n");
		return ERR_FAIL;
	}

	hErr = PAL_FS_GetRemovingFileNameToRecycleDir ((HCHAR *)pathname, szMovedPathName);
	if (ERR_OK == hErr)
	{
		hErr = PAL_FS_Rename (pttype, pathname, szMovedPathName);
		if (ERR_OK == hErr)
		{
			HCHAR			*szDstPathName = pal_fs_strdup (szMovedPathName);
			FS_OP_Msg_t 	 stMsg;

			VK_memset32(&stMsg, 0, sizeof(FS_OP_Msg_t));
			stMsg.ulFSOpType = FS_OP_DELETE;
			stMsg.pszSrc = szDstPathName;

			if (VK_MSG_Send(_ulFileTaskMsgId, &stMsg, sizeof(FS_OP_Msg_t)) == ERR_OK)
			{
				return ERR_OK;
			}

			// Message를 보내지 못했으면 Memory 잡은 것을 없애고 sync delete로 한다.
			HxLOG_Error ("Error to send message (FSOP_MSG), delete sync!!\n");
			UMMA_Free (szDstPathName);

			szDeleteFilePath = szMovedPathName;
		}
	}
#endif

	// recycle bin으로 옮기고 delete async가 불가능 : Sync로 삭제한다.
	return pal_fs_deleteSync (pttype, (const char *)szDeleteFilePath);
}

int
PAL_FS_Flush(void)
{
	sync();
	return ERR_OK;
}

/*
 * deprecated function
 */
int
PAL_FS_CheckNeedFormatAllPartition(void)
{
	return 0;
}

HERROR PAL_FS_Secure_InitCryptoInfo(PvrEncryptMode_t eEncType, HUINT8 *pucKey, HUINT32 ulKeySize, HUINT8 *pucIv, HUINT32 ulIvSize)
{
	HxLOG_Print("[%s] EncType[%d], KeySize[%d], IvSize[%d]\n", __FUNCTION__, eEncType, ulKeySize, ulIvSize);
	switch(eEncType)
	{
		case ePvrEncrypt3Des:
			s_etCryptMode = DI_CRYPT_TDES;
			break;

		case ePvrEncryptAes:
			s_etCryptMode = DI_CRYPT_AES;
			break;

		default:
			HxLOG_Error("Invalid Crypt Type (%d)\n", eEncType);
			s_etCryptMode = DI_CRYPT_TDES;
			break;
	}

	if((DI_PVR_CRYPTO_KEY_LEN != ulKeySize) || (DI_PVR_CRYPTO_KEY_LEN != ulIvSize))
	{
		HxLOG_Error("Invalid Key or IV Size (%d, %d)\n", ulKeySize, ulIvSize);
		return ERR_FAIL;
	}

	if(pucKey != NULL)
	{
		VK_memcpy(s_aucEncKey, pucKey, ulKeySize);
	}

	if(pucIv != NULL)
	{
		VK_memcpy(s_aucEncIv, pucIv, ulIvSize);
	}

	s_bCryptInit = TRUE;

	return ERR_OK;
}

int PAL_FS_Secure_Open(const char *pathname, PAL_FS_UINT32 flags, PAL_FS_UINT32 *filedes)
{
	int nRet = 0;

	nRet = PAL_FS_Open( 0, pathname, flags, filedes );

	return nRet;
}

int PAL_FS_Secure_Close(PAL_FS_UINT32 filedes)
{
	int nRet = 0;

	nRet = PAL_FS_Close( filedes );

	return nRet;
}

int PAL_FS_Secure_Read(PAL_FS_UINT32 filedes, PAL_FS_UINT8 *buf, PAL_FS_UINT32 size, PAL_FS_UINT32 *sizep)
{
#if 0
	int nRet = 0;
	PAL_FS_UINT8  *pucSecureSrc=NULL, *pucSecureDst=NULL;
	stCrypt_t stCrypt;
	HUINT32 ulChipId = 0;
	HUINT8 aucKey[DI_PVR_CRYPTO_KEY_LEN];
	DI_ERR_CODE errDiError = DI_ERR_OK;
	HERROR ulRet = 0;
	HUINT32 ulBufSize = 0, ulHeadSize=0, ulTailSize = 0;
	PAL_FS_UINT32 ulReadSize = 0;
	PAL_FS_INT64 ulCurPosition = 0;
	PAL_FS_INT64 nOffset = 0, nCur=0;

	if( (buf == NULL) || (sizep == NULL) )
		return PAL_FS_BAD_PARAMETER_ERR;

	if( size > MAX_SECURE_BUFFER )
	{
		HxLOG_Error("MAX_SECURE_BUFFER size is over (%d, %d)\n", (int)size, (int)MAX_SECURE_BUFFER);
		return PAL_FS_ERROR_NO_MORE_RESOURCES;
	}

	if(s_bCryptInit == FALSE)
	{
		ulRet = PAL_SYS_GetChipId( &ulChipId );
		if( ulRet != ERR_OK )
		{
			HxLOG_Error("error PAL_SYS_GetChipId: 0x%X\n", ulRet);
			return ERR_FAIL;
		}
	}

	// make 192 byte align
	PAL_FS_Secure_Tell( filedes, &ulCurPosition );
	ulHeadSize = ulCurPosition % ALIGN_UNIT;
	ulTailSize = (ulCurPosition+size) % ALIGN_UNIT;
	if( ulTailSize != 0 )
	{
		ulTailSize = (ALIGN_UNIT- ulTailSize);
	}

	ulBufSize = ulHeadSize + size + ulTailSize;

//	HxLOG_Print("[PAL_FS_Secure_Read] ulCur:%lld, ulHeadSize:%d, size:%d, ulTailSize:%d, ulBufSize:%d\r\n",
//			ulCurPosition, ulHeadSize, size, ulTailSize, ulBufSize);

	if(s_bCryptInit == FALSE)
	{
		VK_memset( aucKey, 0, DI_PVR_CRYPTO_KEY_LEN );
		VK_memcpy( aucKey, (char *)&ulChipId, 4 );
	}
	else
	{
		VK_memcpy( aucKey, s_aucEncKey, DI_PVR_CRYPTO_KEY_LEN );
	}

	pucSecureSrc = DI_CRYPT_MemAlloc( ulBufSize );
	if( pucSecureSrc == NULL )
	{
		HxLOG_Error("DI_CRYPT_MemAlloc is NULL (%d)\n", (int)ulBufSize);
		return PAL_FS_ERROR_NO_MORE_RESOURCES;
	}

	pucSecureDst = DI_CRYPT_MemAlloc( ulBufSize );
	if( pucSecureDst == NULL )
	{
		HxLOG_Error("DI_CRYPT_MemAlloc is NULL (%d)\n", (int)ulBufSize);
		return PAL_FS_ERROR_NO_MORE_RESOURCES;
	}

	if( ulHeadSize != 0 )
	{
		nOffset = ulCurPosition - ulHeadSize;
		nRet = PAL_FS_LSeek(filedes, nOffset, PAL_FS_POS_START, &nCur);
	if( nRet != ERR_OK )
	{
			HxLOG_Error("error : PAL_FS_Seek nRet:0x%X, size:%d\n", nRet, (int)ulHeadSize);
			return PAL_FS_ERROR_NO_MORE_RESOURCES;
		}

//		HxLOG_Print("  ulHeadSize:%d, nOffset:%lld, nCur:%lld\r\n",
//			ulHeadSize, nOffset, nCur);
	}

	nRet = PAL_FS_Read( filedes, pucSecureSrc, ulBufSize, &ulReadSize );
	if( (nRet != ERR_OK) || (ulReadSize == 0))
	{
		HxLOG_Error("error : PAL_FS_Read nRet:0x%X, size:%d, ulReadSize:%d\n", nRet, (int)ulBufSize, (int)ulReadSize);
		return PAL_FS_ERROR_NO_MORE_RESOURCES;
	}

	if( ulTailSize != 0 )
	{
		nOffset= ulCurPosition+size;
		nRet = PAL_FS_LSeek(filedes, nOffset, PAL_FS_POS_START, &nCur);
		if( nRet != ERR_OK )
		{
			HxLOG_Error("error : PAL_FS_Seek nRet:0x%X, size:%d\n", nRet, (int)ulTailSize);
			return PAL_FS_ERROR_NO_MORE_RESOURCES;
		}
//		HxLOG_Print("  ulTailSize:%d, nOffset:%lld, nCur:%lld\r\n",
//			ulTailSize, nOffset, nCur);
	}

	VK_memset( &stCrypt, 0, sizeof(stCrypt_t) );

	stCrypt.etCryptMode = s_etCryptMode;
	stCrypt.etCryptKey = DI_CRYPT_USER_KEY;
	stCrypt.etCryptData = DI_CRYPT_STREAM;
	stCrypt.ulKeySize = DI_PVR_CRYPTO_KEY_LEN;  // 16 byte
	stCrypt.pSrcBuffer =  pucSecureSrc;
	stCrypt.pDstBuffer = pucSecureDst;
	stCrypt.ulBuffSize = ulBufSize;
	stCrypt.pKey = aucKey;
	stCrypt.pIvKey = s_aucEncIv;

	errDiError = DI_CRYPT_Decrypt( &stCrypt );

	if( errDiError != DI_ERR_OK )
	{
		HxLOG_Error("error DI_CRYPT_Decrypt: 0x%X\n", errDiError);
		DI_CRYPT_MemFree( (void *)pucSecureSrc);
		DI_CRYPT_MemFree( (void *)pucSecureDst);
		return ERR_FAIL;
	}

	VK_memcpy32( buf, pucSecureDst+ulHeadSize, size );
	if( ulReadSize < size )
		*sizep = ulReadSize;
	else
		*sizep= size;

	DI_CRYPT_MemFree( (void *)pucSecureSrc);
	DI_CRYPT_MemFree( (void *)pucSecureDst);

	return nRet;
#else
	return 0;
#endif
}

int PAL_FS_Secure_Seek(PAL_FS_UINT32 filedes, PAL_FS_INT64 offset, PAL_FS_UINT32 whence)
{
	int nRet = 0;
	PAL_FS_INT64 curr;

	nRet = PAL_FS_LSeek( filedes, offset, whence, &curr );
	if(nRet != ERR_OK)
	{
		curr = 0;
	}

	return (int)curr;
}

int PAL_FS_Secure_Tell(PAL_FS_UINT32 filedes, PAL_FS_INT64 *poffset)
{
	if( poffset == NULL )
		return ERR_FAIL;

	*poffset = (PAL_FS_INT64) lseek( filedes, 0, SEEK_CUR );

	return ERR_OK;
}

int PAL_FS_Secure_IsEOF(PAL_FS_UINT32  filedes)
{
	char buf[8];
	ssize_t n;

	n = read((int)filedes, buf, 1);
	if( n <= 0 )
		return 1;

	(void)lseek( filedes, -1, SEEK_CUR );

	return 0;
}

int PAL_FS_Secure_GetFileSize(PAL_FS_UINT32 filedes, PAL_FS_INT64 *pulSize)
{
	struct stat st;

	if( pulSize == NULL )
		return ERR_FAIL;

	if (-1 == fstat(filedes, &st)) {
		HxLOG_Error("cannot fstat: %s\n", strerror(errno));
		return PAL_FS_ERROR_NO_MORE_RESOURCES;
	}

	*pulSize = (PAL_FS_INT64)st.st_size;

	return ERR_OK;
}

#if defined (CONFIG_MW_CAS_VIACCESS) && defined (CONFIG_MW_MM_PVR)

#define HDIO_DRIVE_CMD			0x031f
#define HDIO_DRIVE_RESET		0x031c
#define HDIO_DRIVE_TASK			0x031e
#define HDIO_DRIVE_TASKFILE		0x031d
#define HDIO_GETGEO				0x0301
#define HDIO_GETGEO_BIG			0x0330
#define HDIO_GET_32BIT			0x0309
#define HDIO_GET_ACOUSTIC		0x030f
#define HDIO_GET_BUSSTATE		0x031a
#define HDIO_GET_DMA			0x030b
#define HDIO_GET_IDENTITY		0x030d
#define HDIO_GET_KEEPSETTINGS	0x0308
#define HDIO_GET_MULTCOUNT		0x0304
#define HDIO_GET_NOWERR			0x030a
#define HDIO_GET_QDMA			0x0305
#define HDIO_GET_UNMASKINTR		0x0302
#define HDIO_OBSOLETE_IDENTITY	0x0307
#define HDIO_SCAN_HWIF			0x0328
#define HDIO_SET_32BIT			0x0324
#define HDIO_SET_ACOUSTIC		0x032c
#define HDIO_SET_BUSSTATE		0x032d
#define HDIO_SET_DMA			0x0326
#define HDIO_SET_KEEPSETTINGS	0x0323
#define HDIO_SET_MULTCOUNT		0x0321
#define HDIO_SET_NOWERR			0x0325
#define HDIO_SET_PIO_MODE		0x0327
#define HDIO_SET_QDMA			0x032e
#define HDIO_SET_UNMASKINTR		0x0322
#define HDIO_SET_WCACHE			0x032b
#define HDIO_TRISTATE_HWIF		0x031b
#define HDIO_UNREGISTER_HWIF	0x032a
#define CDROM__SPEED			0x5322

#if defined(CONFIG_PROD_CPU_BCM)
#define SMART_BUF_SIZE 				(516)		// 512+4
#define SMART_CMD					(0x031F)
#define SMART_BLKGETSIZE64		_IOR(0x12,114,size_t)
#define SERIAL_NUM_INDEX 			(10*2)
#define ATA_SERIAL_NUM_LEN			(20)

static unsigned char pszHddSn[ATA_SERIAL_NUM_LEN];
#endif

static unsigned short s_dev_param[256];

static char *strip (char *s)
{
	char *e;

	while (*s == ' ')
	{
		++s;
	}

	if (*s)
	{
		for (e = s + strlen(s); *--e == ' '; *e = '\0')
		{
			;
		}
	}
	return s;
}

//swlee3 : hdd physical serial number 임시 코드 추가...
int PAL_FS_GetHddSerialNumber(const char *dev, char **hddsn)
{
#if defined(CONFIG_PROD_CPU_BCM)
	/**
	  *	BRCM Chip (BXR-HD+)
	  *
	  *	refer DRV_SMART_IdentifyDevice()
	**/

	int i;
	int smartfd = -1;
	unsigned short *pusbuff;
	static unsigned char smartbuf[SMART_BUF_SIZE];
	unsigned long long disksize;

	if (hddsn == NULL)
	{
		HxLOG_Error("invalid parameter !\n");
		return ERR_FAIL;
	}

	smartfd=open(dev, O_RDONLY | O_NONBLOCK);
	if(smartfd < 0)
	{
		HxLOG_Error("open error!\n");
		return ERR_FAIL;
	}

	VK_memset(pszHddSn, 0x00, ATA_SERIAL_NUM_LEN);
	VK_memset(smartbuf, 0x00, SMART_BUF_SIZE);
	smartbuf[0] = 0xec;
	smartbuf[3] = 1;

	if(ioctl(smartfd,SMART_CMD,&smartbuf)!= 0)
	{
		HxLOG_Error("[%d]%s ioctl error!!! \n",__LINE__,__func__);
		close(smartfd);
		return ERR_FAIL;
	}

	/* Get Block Size */
	disksize = 0;
	if (ioctl(smartfd, SMART_BLKGETSIZE64, &disksize) != 0)
	{
		HxLOG_Error("[%d]%s ioctl error!!! \n",__LINE__,__func__);
		close(smartfd);
		return ERR_FAIL;
	}

	/* swap data */
	pusbuff = (unsigned short *)smartbuf;
	for(i=0; i<(SMART_BUF_SIZE-4)/2; i++)
	{
		pusbuff[i] = ntohs(pusbuff[i+2]);
	}

	VK_memcpy(pszHddSn, &smartbuf[SERIAL_NUM_INDEX], ATA_SERIAL_NUM_LEN-1);
	*hddsn = strip(pszHddSn);

	close(smartfd);

#else

	/**
	  *	ST Chip (VHDR-3000S)
	**/
	int fd;
	unsigned short id[256];

	if (fd = open(dev, O_RDONLY), -1 == fd)
	{
		HxLOG_Error("inactive device!\n");
		return ERR_FAIL;
	}

	if(ioctl(fd, HDIO_GET_IDENTITY, s_dev_param))
	{
		HxLOG_Error("ioctl() error\n");
		close(fd);
		return ERR_FAIL;
	}

	*hddsn = strip(strndup((char *)&s_dev_param[10], 20));

	close(fd);

#endif

	return ERR_OK;
}

#endif




#define ____________________________USE_DI_FUNCTIONS_________________________________________________
static DI_FS_StorDevIdx_t pal_fs_DevIDConvertPal2Di(PAL_FS_StorDevIdx_t ePalDevIdx)
{
	switch (ePalDevIdx)
	{
		case ePAL_FS_DEV_sda:		return eDI_FS_DEV_sda;
		case ePAL_FS_DEV_sdb:		return eDI_FS_DEV_sdb;
		case ePAL_FS_DEV_sdc: 		return eDI_FS_DEV_sdc;
		case ePAL_FS_DEV_sdd:		return eDI_FS_DEV_sdd;
		case ePAL_FS_DEV_sde:		return eDI_FS_DEV_sde;
		case ePAL_FS_DEV_sdf:		return eDI_FS_DEV_sdf;
		case ePAL_FS_DEV_sdg:		return eDI_FS_DEV_sdg;
		case ePAL_FS_DEV_sdh:		return eDI_FS_DEV_sdh;
		case ePAL_FS_DEV_sdi:		return eDI_FS_DEV_sdi;
		case ePAL_FS_DEV_MAX:		return eDI_FS_DEV_MAX;
		case ePAL_FS_DEV_NULL:
		default : 					return eDI_FS_DEV_NULL;
	}
}

static DI_FileSystemType_t pal_fs_FSTypeConvertPal2Di(PAL_FS_FsType_t ePalFsType)
{
	switch(ePalFsType)
	{
		case PAL_FS_EXT2_FILESYSTEM:		return DI_FS_EXT2_FILESYSTEM;
		case PAL_FS_EXT3_FILESYSTEM:		return DI_FS_EXT3_FILESYSTEM;
		case PAL_FS_EXT4_FILESYSTEM:		return DI_FS_EXT4_FILESYSTEM;
		case PAL_FS_XFS_FILESYSTEM:			return DI_FS_XFS_FILESYSTEM;
		case PAL_FS_FAT16_FILESYSTEM:		return DI_FS_FAT16_FILESYSTEM;
		case PAL_FS_FAT32_FILESYSTEM:		return DI_FS_FAT32_FILESYSTEM;
		case PAL_FS_NTFS_FILESYSTEM:		return DI_FS_NTFS_FILESYSTEM;
		case PAL_FS_MSDOS_FILESYSTEM:		return DI_FS_MSDOS_FILESYSTEM;
		case PAL_FS_EXFAT_FILESYSTEM:		return DI_FS_EXFAT_FILESYSTEM;
		case PAL_FS_OTHER_FILESYSTEM:
		default:							return DI_FS_OTHER_FILESYSTEM;
	}
}

static PAL_FS_FsType_t pal_fs_FSTypeConvertDi2Pal(DI_FileSystemType_t eDiFsType)
{
	switch(eDiFsType)
	{
		case DI_FS_EXT2_FILESYSTEM:			return PAL_FS_EXT2_FILESYSTEM;
		case DI_FS_EXT3_FILESYSTEM:			return PAL_FS_EXT3_FILESYSTEM;
		case DI_FS_EXT4_FILESYSTEM:			return PAL_FS_EXT4_FILESYSTEM;
		case DI_FS_XFS_FILESYSTEM:			return PAL_FS_XFS_FILESYSTEM;
		case DI_FS_FAT16_FILESYSTEM:		return PAL_FS_FAT16_FILESYSTEM;
		case DI_FS_FAT32_FILESYSTEM:		return PAL_FS_FAT32_FILESYSTEM;
		case DI_FS_NTFS_FILESYSTEM:			return PAL_FS_NTFS_FILESYSTEM;
		case DI_FS_MSDOS_FILESYSTEM:		return PAL_FS_MSDOS_FILESYSTEM;
		case DI_FS_EXFAT_FILESYSTEM:		return PAL_FS_EXFAT_FILESYSTEM;
		case DI_FS_OTHER_FILESYSTEM:		return PAL_FS_OTHER_FILESYSTEM;
		default:							return PAL_FS_OTHER_FILESYSTEM;
	}
}

HERROR PAL_FS_GetDeviceFileName (PAL_FS_StorDevIdx_t eDevIdx, HUINT8 *szFileName)
{
	DI_FS_StorDevIdx_t	eDiDevIdx;

	eDiDevIdx = pal_fs_DevIDConvertPal2Di(eDevIdx);

	return SEUAPI_GetDeviceFileName(eDiDevIdx, szFileName);
}

HERROR PAL_FS_GetDeviceFileFullName (PAL_FS_StorDevIdx_t eDevIdx, HUINT8 *szFileName)
{
	DI_FS_StorDevIdx_t	eDiDevIdx;

	eDiDevIdx = pal_fs_DevIDConvertPal2Di(eDevIdx);

	return SEUAPI_GetDeviceFileFullName(eDiDevIdx, szFileName);
}

HERROR PAL_FS_GetPartitionFileName (PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartitionIdx, HUINT8 *szFileName, HUINT8 nSizeOfFileName)
{
	DI_FS_StorDevIdx_t	eDiDevIdx;

	eDiDevIdx = pal_fs_DevIDConvertPal2Di(eDevIdx);

	return SEUAPI_GetPartitionFileName(eDiDevIdx, ulPartitionIdx, szFileName, nSizeOfFileName);
}

#if 0	//unused
HERROR PAL_FS_GetPartitionFullFileName (PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartitionIdx, HUINT8 *szFileName)
{
	DI_FS_StorDevIdx_t	eDiDevIdx;

	eDiDevIdx = pal_fs_DevIDConvertPal2Di(eDevIdx);

#if defined(CONFIG_DROP_ROOT_PRIVILEGES)
	//2 Need se_umma
	return DI_FS_GetPartitionFullFileName(eDiDevIdx, ulPartitionIdx, szFileName);
#else
	return DI_FS_GetPartitionFullFileName(eDiDevIdx, ulPartitionIdx, szFileName);
#endif
}

PAL_FS_BOOL
PAL_FS_HDDExist(const char *mountp)
{
	return DI_FS_HDDExist(mountp);
}

PAL_FS_BOOL
PAL_FS_IsDirExist(const char *dirname)
{
	return DI_FS_IsDirExist(dirname);
}

int
PAL_FS_Mkdir(const char *dirname)
{
	return DI_FS_Mkdir(dirname);
}
#endif

HERROR
PAL_FS_GetVolumeUuid(PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *pBuf, HUINT32 ulBufsize, HUINT32 *pulUuidsize, PAL_FS_FsType_t eFsType)
{
	HERROR	hErr;
	switch(eFsType)
	{
		case PAL_FS_XFS_FILESYSTEM:
		case PAL_FS_NTFS_FILESYSTEM:
		case PAL_FS_EXT2_FILESYSTEM:
		case PAL_FS_EXT3_FILESYSTEM:
		case PAL_FS_EXT4_FILESYSTEM:
 			hErr = SEUAPI_GetVolumeUuid(eDevIdx, ulPartIdx, pBuf, ulBufsize, pulUuidsize);
			break;
		case PAL_FS_FAT16_FILESYSTEM:
		case PAL_FS_FAT32_FILESYSTEM:
		case PAL_FS_MSDOS_FILESYSTEM:
		case PAL_FS_OTHER_FILESYSTEM:
		default :
			hErr = ERR_FAIL;
			HxLOG_Error("No information\n");
			break;
	}

	return hErr;
}

HERROR
PAL_FS_SetVolumeName(PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *pBuf)

{
	// 주의 : DI 함수에서 지원하는 File system 확인 할 것
	return SEUAPI_SetVolumeName(eDevIdx, ulPartIdx, pBuf);
}


HERROR
PAL_FS_GetVolumeName(PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *pBuf, HUINT32 ulSize)
{
	// 주의 : DI 함수에서 지원하는 File system 확인 할 것
	return SEUAPI_GetVolumeName(eDevIdx, ulPartIdx, pBuf, ulSize);
}

HERROR
PAL_FS_GetFsType(PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, PAL_FS_FsType_t *pFsType)
{
	DI_ERR_CODE			eDiErr;
	DI_FileSystemType_t eDiFsType;

	if (NULL == pFsType)
		return ERR_FAIL;

	eDiErr = SEUAPI_GetFsType(eDevIdx, ulPartIdx, &eDiFsType);

	*pFsType = pal_fs_FSTypeConvertDi2Pal(eDiFsType);

	return (eDiErr == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

static void pal_fs_GetDevInfoFromDeviceName(const char *pszDevName, PAL_FS_StorDevIdx_t *peDevIdx, HUINT32 *pulPartIdx)
{
	HUINT32					 ulDevNameLength;
	PAL_FS_StorDevIdx_t		 eDevIdx;
	HUINT32					 ulPartIdx, ulIdx, ulLength;
	HCHAR					 cDev;
	HBOOL					 bPartIdx	= FALSE;
	HCHAR					*pszIdx = NULL;

	HxLOG_Assert(pszDevName);

	if (FALSE == HLIB_STD_StrStartWith(pszDevName, (const HCHAR *)"/dev/sd"))
	{
		HxLOG_Error("Invalid DevName [%s]\n", pszDevName);
		return;
	}

	eDevIdx = ePAL_FS_DEV_NULL;
	ulPartIdx = PAL_FS_MAX_PARTITIONS;

	ulIdx = HxSTD_StrLen("/dev/sd");

	ulLength = HxSTD_StrLen("/dev/sda");
	ulDevNameLength = HxSTD_StrLen(pszDevName);

	HxLOG_Debug("DevName [%s : (%d)]\n", pszDevName, ulDevNameLength);

	if (ulDevNameLength == ulLength)
	{
		bPartIdx = FALSE;
	}
	else if (ulDevNameLength > ulLength)
	{
		pszIdx = (HCHAR*)&pszDevName[ulIdx+1];
		if (HLIB_STD_IsInteger((const HCHAR *)pszIdx))
		{
			bPartIdx = TRUE;
		}
	}
	else
	{
		return;
	}

	cDev = pszDevName[ulIdx];
	if (cDev >= 'a' && cDev <= 'i')
	{
		eDevIdx = ePAL_FS_DEV_sda + pszDevName[ulDevNameLength-2] - 'a';
	}

	HxLOG_Debug("eDevIdx [%d]\n", eDevIdx);
	if (bPartIdx && pszIdx)
	{
		HxLOG_Debug("pszIdx [%s]\n", pszIdx);
		ulPartIdx = HLIB_STD_StrToUINT32((const HCHAR *)pszIdx) -1;
		HxLOG_Debug("ulPartIdx [%d]\n", ulPartIdx);
	}

	if (peDevIdx)
		*peDevIdx = eDevIdx;
	if (pulPartIdx)
		*pulPartIdx = ulPartIdx;
}

typedef struct
{
	HCHAR				*pszFsName;
	PAL_FS_FsType_t		 eFsType;
} pal_fs_pair_t;

static pal_fs_pair_t	s_astFsPairTable[] =
{
	{
		"ext2",
		PAL_FS_EXT2_FILESYSTEM
	},
	{
		"ext3",
		PAL_FS_EXT3_FILESYSTEM
	},
	{
		"xfs",
		PAL_FS_XFS_FILESYSTEM
	},
	{
		"tntfs",
		PAL_FS_NTFS_FILESYSTEM
	},
	{
		NULL,
		PAL_FS_OTHER_FILESYSTEM
	}
};


static PAL_FS_FsType_t pal_fs_GetPalFsType(const char *pszFsType)
{
	HUINT32	ulCount;
	HUINT32	ulLength = sizeof(s_astFsPairTable) / sizeof(pal_fs_pair_t);

	for (ulCount=0; ulCount<ulLength; ulCount++)
	{
		if (NULL == s_astFsPairTable[ulCount].pszFsName)
			break;
		if (0 == HLIB_STD_StrCaseCmp(pszFsType, s_astFsPairTable[ulCount].pszFsName))
		{
			return s_astFsPairTable[ulCount].eFsType;
		}
	}

	return PAL_FS_OTHER_FILESYSTEM;
}

static PAL_FS_StorDevIdx_t			s_eDevIdx = ePAL_FS_DEV_NULL;
static PAL_FS_DST_Callback_Func		s_pfnDst_NotifyCb = NULL;
#if defined(CONFIG_PROD_DEVICE_STORAGE_HDD)

static PAL_FS_HDD_DST_RESULT_e pal_fs_DiDstResult2PalDstResult(HDD_DST_RESULT dstResult)
{
	PAL_FS_HDD_DST_RESULT_e eDstResult;

	eDstResult = ePAL_FS_DST_COMPLETED_UNKNOWN;
	switch(dstResult)
	{
	case DST_HDD_NOT_DETECTED: 						eDstResult = ePAL_FS_DST_HDD_NOT_DETECTED;						break;
	case DST_FAILED_TRY_AGAIN_LATER:				eDstResult = ePAL_FS_DST_FAILED_TRY_AGAIN_LATER;				break;
	case DST_IS_ON_GOING:							eDstResult = ePAL_FS_DST_IS_ON_GOING;							break;
	case DST_COMPLETED_OK:							eDstResult = ePAL_FS_DST_COMPLETED_OK;							break;
	case DST_COMPLETED_FATAL_ERROR:					eDstResult = ePAL_FS_DST_COMPLETED_FATAL_ERROR;					break;
	case DST_COMPLETED_TEST_ELEMENT_FAILED:			eDstResult = ePAL_FS_DST_COMPLETED_TEST_ELEMENT_FAILED;			break;
	case DST_COMPLETED_ELECTRICAL_ELEMENT_FAILED:	eDstResult = ePAL_FS_DST_COMPLETED_ELECTRICAL_ELEMENT_FAILED;	break;
	case DST_COMPLETED_SERVO_ELEMENT_FAILED:		eDstResult = ePAL_FS_DST_COMPLETED_SERVO_ELEMENT_FAILED;		break;
	case DST_COMPLETED_READ_ELEMENT_FAILED:			eDstResult = ePAL_FS_DST_COMPLETED_READ_ELEMENT_FAILED;			break;
	case DST_COMPLETED_HANDLING_DAMAGE:				eDstResult = ePAL_FS_DST_COMPLETED_HANDLING_DAMAGE;				break;
	case DST_COMPLETED_UNKNOWN:						eDstResult = ePAL_FS_DST_COMPLETED_UNKNOWN;						break;
	default:										break;
	}

	return eDstResult;
}

static void	pal_fs_DstCallback(HDD_DST_RESULT dstResult, HINT32 nProgress)
{
	if (s_pfnDst_NotifyCb)
	{
		static HINT32 s_nProgress = -1;
		PAL_FS_HDD_DST_RESULT_e eDstResult;

		eDstResult = pal_fs_DiDstResult2PalDstResult(dstResult);

		if (ePAL_FS_DST_IS_ON_GOING == eDstResult)
		{
			if (s_nProgress != nProgress)
			{
				s_nProgress = nProgress;
				HxLOG_Print("[%s:%04d] s_eDevIdx [%d], dstResult [%d], nProgress [%d]\n", __FUNCTION__, __LINE__, s_eDevIdx, dstResult, nProgress);
				(s_pfnDst_NotifyCb)(s_eDevIdx, eDstResult, (HUINT32)nProgress);
			}
		}
		else
		{
			HxLOG_Print("[%s:%04d] s_eDevIdx [%d], dstResult [%d], nProgress [%d]\n", __FUNCTION__, __LINE__, s_eDevIdx, dstResult, nProgress);
			(s_pfnDst_NotifyCb)(s_eDevIdx, eDstResult, (HUINT32)nProgress);
		}

		if (ePAL_FS_DST_IS_ON_GOING != eDstResult)
		{
			s_nProgress = -1;
			s_eDevIdx = ePAL_FS_DEV_NULL;
		}
	}
}
#endif

HERROR PAL_FS_DstStart (PAL_FS_StorDevIdx_t eDevIdx, PAL_FS_DST_Callback_Func pfnDstNotify)
{
	if (NULL == pfnDstNotify)
	{
		HxLOG_Error("NULL == pfnDstNotify \n");
		return ERR_FAIL;
	}

	HxLOG_Debug("PAL_FS_DstStart(%d) \n", eDevIdx);

	s_eDevIdx = eDevIdx;
	s_pfnDst_NotifyCb = pfnDstNotify;

#if defined(CONFIG_PROD_DEVICE_STORAGE_HDD)
	SEUAPI_RegisterDstCallback(pal_fs_DstCallback);
	SEUAPI_StartDst();
#endif

	return ERR_OK;
}

STATIC HBOOL pal_fs_cbDeleteFiles (HxDIR_t *pSelf, HCHAR *pszPath, HCHAR *pszName, HBOOL bIsDir, void *userdata)
{
	FS_OP_Msg_t		 stMsg;
	HCHAR			*szDstPathName;
	HCHAR			 szFileName[312];

	HxSTD_PrintToStrN (szFileName, 312, "%s%s", pszPath, pszName);

	szDstPathName = pal_fs_strdup (szFileName);
	VK_memset32(&stMsg, 0, sizeof(FS_OP_Msg_t));
	stMsg.ulFSOpType = FS_OP_DELETE;
	stMsg.pszSrc = szDstPathName;

	if (VK_MSG_Send(_ulFileTaskMsgId, &stMsg, sizeof(FS_OP_Msg_t)) != ERR_OK)
	{
		HxLOG_Error ("Error to send message (FSOP_MSG)\n");
	}

	return FALSE;
}

int	PAL_FS_ClearRecycleBin (PAL_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx)
{
	HCHAR			 szRecycleDir[128], szFileName[8];
	HERROR			 hErr;

	if (_ulFileTaskId == 0)
	{
		if (pal_fs_createFileTask(&_ulFileTaskId) != ERR_OK)
		{
			HxLOG_Print("Fail to delete file asynch, Create task fail!!\n");
			return ERR_FAIL;
		}
	}

	hErr = PAL_FS_GetRecycleDir (eDevIdx, ulPartIdx, szRecycleDir);
	if (ERR_OK == hErr)
	{
		HxSTD_PrintToStrN (szFileName, 8, "*.*");
		HLIB_DIR_Find (szRecycleDir, szFileName, "", pal_fs_cbDeleteFiles, NULL);
	}

	return ERR_OK;
}

#if 0 // unused

HERROR		pal_fs_usbhdd_ProductInfo(PAL_FS_StorDevIdx_t eDevIdx, PAL_HDD_USBHDD_INFO_t *pstUsbHddInfo)
{
	PAL_FS_StorDevIdx_t	eDiDevIdx;
#ifndef	WIN32
	eDiDevIdx = pal_fs_DevIDConvertPal2Di(eDevIdx);

	if(eDiDevIdx == ePAL_FS_DEV_NULL)
	{
		return ERR_FAIL;
	}

#if defined(CONFIG_PROD_DEVICE_STORAGE_HDD)
	DI_HDD_GetInfoUsbHdd(eDiDevIdx, (DI_HDD_USBHDD_INFO_t*)pstUsbHddInfo);
#endif
#endif

	return ERR_OK;
}
HERROR		PAL_FS_USBHDD_GetProductInfo(HUINT32 ulDeviceId, HUINT8* paucUsbHddVendor, HUINT8* paucUsbHddProduct, HUINT8* paucUsbHddSerialNum)
{
	HERROR			 hErr;
	PAL_HDD_USBHDD_INFO_t stUsbHddInfo;

	VK_memset(&stUsbHddInfo, 0x00, sizeof(PAL_HDD_USBHDD_INFO_t));

	hErr = pal_fs_usbhdd_ProductInfo (ulDeviceId, &stUsbHddInfo);

	VK_memcpy(paucUsbHddVendor, stUsbHddInfo.aucUsbHddVendor, sizeof(stUsbHddInfo.aucUsbHddVendor));
	VK_memcpy(paucUsbHddProduct, stUsbHddInfo.aucUsbHddProduct, sizeof(stUsbHddInfo.aucUsbHddProduct));
	VK_memcpy(paucUsbHddSerialNum, stUsbHddInfo.aucUsbHddSerialNum, sizeof(stUsbHddInfo.aucUsbHddSerialNum));

	return ERR_OK;
}


#endif



#if defined(CONFIG_DEBUG)
STATIC int pal_fs_cmdRmFile (void *szArgList)
{
	HCHAR		*szArg1;
	HERROR		 hErr;

	szArg1 = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if ((NULL == szArg1) || ('\0' == szArg1[0]))
	{
		return HxCMD_ERR;
	}

	hErr = PAL_FS_Delete (0, szArg1);
	if (ERR_OK != hErr)
	{
		return HxCMD_ERR;
	}

	return HxCMD_OK;
}


STATIC void pal_fs_cmdInit (void)
{
#define	hCmdHandle		"FS"
	HLIB_CMD_RegisterWord (hCmdHandle,
							pal_fs_cmdRmFile,
							"pal_rm", 						/* keyword */
							"Remove the file using pal_fs",			/*	 help  */
							"pal_rm [filename]"
							);

}
#endif
