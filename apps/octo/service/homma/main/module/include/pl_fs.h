/**************************************************************************************/
/**
 * @file pal_fs.h
 * defines APIs's body
 *
 * file operation
 *
 * @attention   Copyright (c) 2013 Humax - All rights reserved.
 *
**************************************************************************************
**/

#ifndef __PL_FS_H__
#define __PL_FS_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "common.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#if defined(CONFIG_MW_MM_INTERNAL_SATA)
#define __APP_CONTROLS_HDD_POWER__
#endif

#define	CONV_PALFS_2_DIFS_TEST

#if WIN32
#define BLKSSZGET			0
#define BLKGETSIZE64		0
#else
#define BLKSSZGET			_IO(0x12,104)
#define BLKGETSIZE64		_IOR(0x12,114,size_t)
#endif

/* partition related */
#define MAX_SEC_SIZE		2048
#define MIN_SEC_SIZE		512

#define PL_FS_FILE_DIRECTORY		0x00000001
#define PL_FS_FILE_STREAM			0x00000002
#define PL_FS_FILE_CYCLIC			0x00000004
#define PL_FS_FILE_BOOT			0x00000008
#define PL_FS_MODE_READ			0x00000010
#define PL_FS_MODE_WRITE			0x00000020
#define PL_FS_MODE_CYCLIC			PAL_FS_FILE_CYCLIC
#define PL_FS_MODE_PATH			0x00000080
#define PL_FS_MODE_RDWR			0x00000100
#define PL_FS_MODE_APPEND			0x00000200
#define PL_FS_MODE_TRUNC			0x00000400
#define PL_FS_MODE_NONBLOCK		0x00000800
#define PL_FS_MODE_O_DIRECT		0x00001000
#define PL_FS_MODE_O_LARGEFILE		0x00002000

#define	PL_FS_NULL_FILE_HANDLE		(~0U)

/* directory mask */
#define PL_FS_DIRINFO_ALL							0xFFFFFFFF
#define PL_FS_DIRINFO_FILE							0x0000FFFF
#define PL_FS_DIRINFO_DIR							0xFFFF0000

#define PL_FS_DIRINFO_VIDEO						0x00000001
#define PL_FS_DIRINFO_PLAYLIST						0x00000002
#define PL_FS_DIRINFO_MOVIE						0x00000010
#define PL_FS_DIRINFO_MUSIC						0x00000020
#define PL_FS_DIRINFO_IMAGE						0x00000040


#define PL_FS_EXT_STORAGE_MOUNT_PATH	"/media/"
#define PL_FS_EXT_STOR_INTER_PATH		"drive"

#define PL_USB_MOUNT_PATH_LEN			4

#define PL_MAX_PARTITION				10			// USB partitions support
#define PL_FS_LOGICAL_PARTITION_NUM	4

#define MEGA_BYTES						(1000 * 1000) /* XXX no binary prefix */

#define EXT2_SUPER_MAGIC 				0xEF53
#define VOLNAMSZ 						16
#define VOLUUIDSZ						36			/* maybe 36, unparsed */

#define PL_FS_MAX_COPY_INST_NUM		4096

#define PL_FS_DEVFILE_LEN				16			/* /dev/sda1 /dev/sda2 등 */
#define PL_FS_MOUNT_PATH_LEN			32			/* /mnt/hd1, /media/drive1 등 */

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	PL_FS_FILE_TYPE_FILE,
	PL_FS_FILE_TYPE_DIR,
	PL_FS_FILE_TYPE_CHAR_DEV,
	PL_FS_FILE_TYPE_BLOCK,
	PL_FS_FILE_TYPE_FIFO,
	PL_FS_FILE_TYPE_LINK,
	PL_FS_FILE_TYPE_SOCKET,
	PL_FS_FILE_TYPE_UNKNOWN

} PL_FS_FILE_TYPE_e;

enum
{
	PL_FS_POS_START,	/* SEEK_SET */
	PL_FS_POS_CURRENT,	/* SEEK_CUR */
	PL_FS_POS_END		/* SEEK_END */
};

enum
{
	PL_FS_INFO_NAMETYPE,
	PL_FS_INFO_EXCEPTHIDDEN,
	PL_FS_INFO_ALLLIST,
	PL_FS_INFO_RECURSIVEALL,
	PL_FS_INFO_TIME
};

// File System Error codes
enum
{
	PL_FS_EOF_ERR					= 0xA100,
	PL_FS_DISK_ERROR,
	PL_FS_REACH_READ_POINTER,
	PL_FS_DISK_FULL,
	PL_FS_SEEK_OUTSIDE_FILE,
	PL_FS_NOT_FORMATTED_DISK_ERR,
	PL_FS_DISK_NOT_EXIST_ERR,
	PL_FS_FILE_NOT_EXIST_ERR,
	PL_FS_FOLDER_NOT_EXIST_ERR,
	PL_FS_ERROR_FILE_IN_USE,
	PL_FS_NOT_SUPPORTED_CMD_ERR,
	PL_FS_BAD_PARAMETER_ERR,
	PL_FS_OPERATION_PROTECT_ERR,
	PL_FS_READ_ONLY_ERR,
	PL_FS_ERROR_INVALID_HANDLE,
	PL_FS_ERROR_INVALID_OPENMODE,
	PL_FS_ERROR_NO_MEMORY,
	PL_FS_ERROR_NO_MORE_RESOURCES,
	PL_FS_ERROR_SECURITY_LOCKED,
	PL_FS_ERROR_INVALID_VALUE,
	PL_FS_ERROR_FORK,
	PL_FS_ERROR_FORK_PID,
	PL_FS_ERROR_FILE_OPEN,
	PL_FS_ERROR_FILE_READ,
	PL_FS_ERROR_FILE_WRITE,
	PL_FS_ERROR_FILE_CLOSE,
	PL_FS_ERROR_IO,					// EIO
	PL_FS_ERR_OUTOFMEMORY,
} PL_FS_ERR_CODE;

typedef char				PL_FS_INT8;
typedef short				PL_FS_INT16;
typedef long				PL_FS_INT32;
typedef long long			PL_FS_INT64;
typedef unsigned char		PL_FS_UINT8;
typedef unsigned short		PL_FS_UINT16;
typedef unsigned long		PL_FS_UINT32;
typedef unsigned long long	PL_FS_UINT64;
typedef int					PL_FS_BOOL;
typedef int					PL_FS_ResultCode_t;

#if WIN32
#define NAME_MAX			256
#endif

#ifndef NAME_MAX
#include <limits.h>
#endif

#define PL_FS_MAX_NAME_LEN	NAME_MAX

typedef struct
{
	PL_FS_UINT8		aucFileName[NAME_MAX];
	PL_FS_UINT32		fileType;
	PL_FS_UINT64		ulFileSize;
	PL_FS_UINT32		aTime;
	PL_FS_UINT32		mTime;
	PL_FS_UINT32		cTime;
} PL_FS_FileStat_t;

typedef struct
{
	PL_FS_UINT32		unFileHandle;
	PL_FS_UINT8		*szFileName;
	PL_FS_UINT64		ulFileSize;
} PL_FS_FileInfo_t;

typedef struct
{
	PL_FS_UINT8		aucSrcName[NAME_MAX];
	PL_FS_UINT8		aucDestName[NAME_MAX];
} PL_FS_Rename_t;

typedef struct
{
	PL_FS_INT32		fileNumber;
	PL_FS_FileStat_t	*s_astfilestats;
} PL_FS_DirStat_t;

typedef struct
{
	PL_FS_UINT8		aucName[NAME_MAX];
	PL_FS_BOOL			bFlagDir;
	PL_FS_UINT64		ulFileSize;
} PL_FS_DirEntry_t;

typedef struct
{
	char			*sourceName;
	char			*destName;
	int				flag;
} PL_FS_FILE_COMMAND_INFO;

/*******************************************************************/
/********************    Functions       *************************/
/*******************************************************************/
int PL_FS_Init(void);
char *PL_FS_GetEncryptKey(void);

int PL_FS_Open(const char *, PL_FS_UINT32, PL_FS_UINT32 *);
int PL_FS_Close(PL_FS_UINT32);
int PL_FS_Read(PL_FS_UINT32, PL_FS_UINT8 *, PL_FS_UINT32, PL_FS_UINT32 *);
int PL_FS_Write(PL_FS_UINT32, const PL_FS_UINT8 *, PL_FS_UINT32, PL_FS_UINT32 *);
int PL_FS_LSeek(PL_FS_UINT32, PL_FS_INT64, PL_FS_UINT32, PL_FS_INT64 *);
int PL_FS_Seek(PL_FS_UINT32, PL_FS_INT64, PL_FS_UINT32);
int PL_FS_Tell (PL_FS_UINT32, PL_FS_INT64 *);
int PL_FS_IsEOF(PL_FS_UINT32  filedes);
int PL_FS_GetFileSize(PL_FS_UINT32 filedes, PL_FS_INT64 *pulSize);
PL_FS_BOOL PL_FS_IsFileExist(const char *);
int PL_FS_GetFileInfo(const char *, PL_FS_UINT32, PL_FS_FileStat_t *);
int PL_FS_Rename(int, const char *, const char *);
void PL_FS_OpenFileList(void);


PL_FS_BOOL PL_FS_IsDirExist(const char *);
int PL_FS_OpenDir(int, const char *, PL_FS_UINT32 *);
int PL_FS_CloseDir(PL_FS_UINT32);
int PL_FS_ReadDir(PL_FS_UINT32, PL_FS_DirEntry_t *);
int PL_FS_Mkdir(const char *);
int PL_FS_ChangeDir(const char *);
int PL_FS_CountFile(const char *, char **, unsigned int *);
int PL_FS_FindFile(const char *, char **, unsigned int *, unsigned int *);


// for crypted api
int PL_FS_Secure_Open(const char *pathname, PL_FS_UINT32 flags, PL_FS_UINT32 *filedes);
int PL_FS_Secure_Close(PL_FS_UINT32 filedes);
int PL_FS_Secure_Read(PL_FS_UINT32 filedes, PL_FS_UINT8 *buf, PL_FS_UINT32 readsize, PL_FS_UINT32 *sizep);
int PL_FS_Secure_ReadWithKey(PL_FS_UINT32 filedes, PL_FS_UINT8 *buf, PL_FS_UINT32 readsize, PL_FS_UINT32 *sizep, PL_FS_UINT8 *keydata, PL_FS_UINT32 keysize);
int PL_FS_Secure_Seek(PL_FS_UINT32 filedes, PL_FS_INT64 offset, PL_FS_UINT32 whence);
int PL_FS_Secure_Tell(PL_FS_UINT32 filedes, PL_FS_INT64 *poffset);
int PL_FS_Secure_IsEOF(PL_FS_UINT32  filedes);
int PL_FS_Secure_GetFileSize(PL_FS_UINT32 filedes, PL_FS_INT64 *pulSize);

/*
	CHUNKED File을 위한 Layer
*/
HBOOL		PL_FS_CHUNK_IsChunkedFile (HCHAR *szPath);
HOM_Handle_t	PL_FS_CHUNK_Open (HCHAR *szPath, HCHAR *szMode, HBOOL bSecure);
HERROR		PL_FS_CHUNK_Close (HOM_Handle_t hChunk);
HERROR		PL_FS_CHUNK_Read (HOM_Handle_t hChunk, HUINT8 *pucBuf, HULONG ulSize, HULONG *pulSizeRead);
HERROR		PL_FS_CHUNK_GetFileSize (HOM_Handle_t hChunk, HINT64 *pnSize);
HERROR		PL_FS_CHUNK_Seek (HOM_Handle_t hChunk, HINT64 nOffset, HULONG ulWhence, HINT64 *pnAbsOffset);
HERROR		PL_FS_CHUNK_Tell (HOM_Handle_t hChunk, HINT64 *pnAbsOffset);
HBOOL		PL_FS_CHUNK_IsEof (HOM_Handle_t hChunk);
#endif /* !__PL_FS_H__ */

